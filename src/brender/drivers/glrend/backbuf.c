/*
 * port: FBO-backed back buffers for glrend.
 *
 * Every offscreen pixelmap made by match() owns an FBO with a colour texture (and the
 * depth pixelmap matched from it attaches its depth texture to that FBO). The FBO is
 * render_scale times the pixelmap size, so the 3D is drawn at the window resolution
 * while the game keeps addressing a 640x480 screen.
 *
 * CPU access (directLock, rectangleCopyTo, fills while locked) goes into a 640x480
 * RGB565 overlay that starts out as the magenta key colour. Before any GPU work on the
 * buffer (scene begin, fill, copy, double buffer) the overlay is drawn into the FBO
 * with the magenta pixels discarded and reset, which keeps the order the game drew in.
 */
#include "drv.h"
#include <brassert.h>
#include <string.h>
#include "brgl_display.h"
#if defined(__vita__) || defined(C2_FAST_OVERLAY)
#include "overlay_scan.h"
#if defined(__vita__) || defined(C2_FAST_OVERLAY)
#define C2_OVERLAY_FAST 1
#else
#define C2_OVERLAY_FAST 0
#endif
#if C2_OVERLAY_FAST && !defined(__vita__)
/* desktop test build of the Vita overlay path: statistics hooks are no-ops */
static void C2VitaGLRecordOverlayScope(void) {}
static void C2VitaGLRecordOverlayCheck(int empty, unsigned long long ns) { (void)empty; (void)ns; }
static void C2VitaGLRecordOverlayRows(int rows) { (void)rows; }
static unsigned long long C2VitaGLTicksNS(void) { return 0; }
#endif
#if C2_OVERLAY_FAST
/* root whose overlay the caller has proven all key since its last flush (no 2D writes since) */
static br_device_pixelmap* gl_overlay_clean_root;
static unsigned char gl_overlay_rows[1024]; /* rows with content found by the last flush */
static GLuint gl_overlay_draw_texture;
#endif
#if defined(__vita__)
#include "../../../carma2/platform/sdl3/vita_perf.h"
#define VPB(x) do { if (C2VitaPerfDetail) C2VitaPerfBegin(x); } while (0)
#define VPE(x) do { if (C2VitaPerfDetail) C2VitaPerfEnd(x); } while (0)
#else
#define VPB(x) ((void)0)
#define VPE(x) ((void)0)
#endif
#endif

#define OVERLAY_KEY BR_COLOUR_565(31, 0, 31)

static br_device_pixelmap* root_of(br_device_pixelmap* self) {
    if (self->use_type == BRT_DEPTH && self->asDepth.backbuffer != NULL)
        self = self->asDepth.backbuffer;
    return self->root != NULL ? self->root : self;
}

#if C2_OVERLAY_FAST
/* Flush pm's overlay now and declare it clean until C2VitaOverlayEndClean: the caller guarantees that
 * no CPU 2D writes to that target happen in between (e.g. the 3D part of a render pass). */
void C2VitaOverlayBeginClean(void* pm);
#endif
int GLBackScale(const br_device_pixelmap* self) {
    int s = self->screen != NULL ? self->screen->asFront.render_scale : 1;
    return s > 0 ? s : 1;
}

/* Map a logical sub-area into its FBO; UI occupies a centred proportional area.
 * World passes use the entire target and a correspondingly wider camera. */
void GLBackViewport(br_device_pixelmap* self, int* x, int* y, int* w, int* h) {
    br_device_pixelmap* root = root_of(self);
    int s = GLBackScale(root);
    *x = self->pm_base_x * s;
    /* Keep the renderer's existing base_y==0 convention for cropped views. */
    *y = self->pm_base_y == 0 ? 0 :
        ((self->sub_pixelmap ? self->parent_height : root->pm_height) - self->pm_base_y - self->pm_height) * s;
    *w = self->pm_width * s; *h = self->pm_height * s;
    if (root->screen != NULL && root->pm_width == root->screen->pm_width && root->pm_height == root->screen->pm_height)
        C2GLScaleUIRect(root->pm_width * s, root->pm_height * s, x,y,w,h);
}

void GLBackDrawTexture(br_device_pixelmap* screen, GLuint tex, int vx, int vy, int vw, int vh, int flip, int discard_purple) {
    HVIDEO hVideo = &screen->asFront.video;

    glViewport(vx, vy, vw, vh);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);
    glDisable(GL_SCISSOR_TEST);
    glBindTexture(GL_TEXTURE_2D, tex);
    glBindVertexArray(screen->asFront.screen_buffer_vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, screen->asFront.screen_buffer_ebo);
    glUseProgram(hVideo->defaultProgram.program);
    glUniform1f(hVideo->defaultProgram.uFlipVertically, (float)flip);
    glUniform1i(hVideo->defaultProgram.uDiscardPurplePixels, discard_purple);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
    GL_CHECK_ERROR();
}

#ifdef __vita__
/* One bounded read-only calibration on the first newly filled buffer. It lets
 * the hardware log distinguish old scalar scan cost from other scene work. */
static void benchmark_overlay_check(const br_uint_16* pixels, int count) {
    static int done;
    unsigned long long scalar_ns = 0, vector_ns = 0, start;
    volatile int valid = 1;
    if (done || count <= 0) return;
    done = 1;
    for (int pass = 0; pass < 4; ++pass) {
        start = C2VitaGLTicksNS();
        valid &= GLOverlayAllKeyScalar(pixels, (size_t)count, OVERLAY_KEY);
        scalar_ns += C2VitaGLTicksNS() - start;
        start = C2VitaGLTicksNS();
        valid &= GLOverlayAllKey(pixels, (size_t)count, OVERLAY_KEY);
        vector_ns += C2VitaGLTicksNS() - start;
    }
    C2VitaGLLogOverlayBenchmark((unsigned)count, scalar_ns, vector_ns, valid);
}
#endif

void GLBackEnsureOverlay(br_device_pixelmap* root) {
    br_uint_16* px;
    int n;
#ifndef __vita__
    int i;
#endif

    if (root->asBack.lockedPixels != NULL)
        return;

    n = root->pm_width * root->pm_height;
    root->asBack.lockedPixels = BrMemAllocate(n * sizeof(br_uint_16), BR_MEMORY_PIXELS);
    px = root->asBack.lockedPixels;
#ifdef __vita__
    GLOverlayFillKey(px, (size_t)n, OVERLAY_KEY);
#else
    for (i = 0; i < n; i++)
        px[i] = OVERLAY_KEY;
#endif

#ifdef __vita__
    benchmark_overlay_check(px, n);
#endif
    glGenTextures(1, &root->asBack.overlayTexture);
    glBindTexture(GL_TEXTURE_2D, root->asBack.overlayTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, root->pm_width, root->pm_height, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);
    GL_CHECK_ERROR();
}

void GLBackBindTarget(br_device_pixelmap* self) {
    glBindFramebuffer(GL_FRAMEBUFFER, root_of(self)->asBack.glFbo);
}

#if defined(__vita__)
static void GLBackFlushOverlay_timed(br_device_pixelmap* self);
void GLBackFlushOverlay(br_device_pixelmap* self) {
    VPB(C2VP_G_OVERLAY);
    GLBackFlushOverlay_timed(self);
    VPE(C2VP_G_OVERLAY);
}
static void GLBackFlushOverlay_timed(br_device_pixelmap* self) {
#else
void GLBackFlushOverlay(br_device_pixelmap* self) {
#endif
    br_device_pixelmap* root = root_of(self);
    int n;
#ifndef __vita__
    int i;
#endif
    br_uint_16* px;

    /* the overlay is always mapped, so any CPU write may be pending: always flush */
    if (root->use_type != BRT_OFFSCREEN || root->asBack.lockedPixels == NULL)
        return;
#if C2_OVERLAY_FAST
    C2VitaGLRecordOverlayScope();
    if (root == gl_overlay_clean_root)
        return; /* caller proved the buffer is still all key since the previous flush */
#endif

#if C2_OVERLAY_FAST
    /* CPU pixels stay mapped and can be written without a lock, so a dirty
     * flag cannot prove this is empty. Do not upload/draw an all-key overlay:
     * VitaGL must otherwise clone a recently drawn640x480texture per flush. */
    n = root->pm_width * root->pm_height;
    px = root->asBack.lockedPixels;
    unsigned long long check_start = C2VitaGLTicksNS();
    int empty = GLOverlayAllKey(px, (size_t)n, OVERLAY_KEY);
    C2VitaGLRecordOverlayCheck(empty, C2VitaGLTicksNS() - check_start);
    if (empty) {
        root->asBack.possiblyDirty = 0;
        return;
    }
#endif

    /* These uploads are packed RGBA8/RGB565; VitaGL uses alignment1. */
#ifndef __vita__

    glPixelStorei(GL_UNPACK_ALIGNMENT, 2);
#endif
#if C2_OVERLAY_FAST
    {
        /* Upload only rows that hold content now or held content in this texture after its previous
         * upload (those must be reset to the key), so the texture still equals the CPU buffer and the
         * full-screen draw below is unchanged. A full 640x480 upload costs ~12 ms on the Vita.
         * Rotate through textures: updating a texture drawn within the last FRAME_PURGE_FREQ (4) VitaGL
         * frames makes VitaGL clone the whole 640x480 texture first (~7 ms even for a few rows), so the
         * ring must be longer than that window (one non-empty flush per frame in a race). */
#define GL_OVERLAY_RING 6
        static br_device_pixelmap* band_root;
        static GLuint ring[GL_OVERLAY_RING];
        static unsigned char ring_rows[GL_OVERLAY_RING][1024]; /* 1 = row in that texture is not all key */
        static int ring_index;
        unsigned char* texture_rows;
        unsigned char* rows = gl_overlay_rows;
        int w = root->pm_width, h = root->pm_height, y, uploaded = 0;
        if (h > 1024 || band_root != root) {
            int r;
            for (r = 0; r < GL_OVERLAY_RING; ++r) {
                if (ring[r] == 0) {
                    glGenTextures(1, &ring[r]);
                    glBindTexture(GL_TEXTURE_2D, ring[r]);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, NULL);
                }
                memset(ring_rows[r], 1, sizeof(ring_rows[r])); /* unknown texture contents: refresh all */
            }
            band_root = root;
        }
        ring_index = (ring_index + 1) % GL_OVERLAY_RING;
        gl_overlay_draw_texture = ring[ring_index];
        texture_rows = ring_rows[ring_index];
        glBindTexture(GL_TEXTURE_2D, gl_overlay_draw_texture);
        for (y = 0; y < h && y < 1024; ++y)
            rows[y] = !GLOverlayAllKey(px + (size_t)y * w, (size_t)w, OVERLAY_KEY);
        for (y = 0; y < h;) {
            int y0, y1;
            if (y < 1024 && !rows[y] && !texture_rows[y]) { ++y; continue; }
            y0 = y;
            while (y < h && (y >= 1024 || rows[y] || texture_rows[y])) ++y;
            y1 = y;
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, y0, w, y1 - y0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, px + (size_t)y0 * w);
            uploaded += y1 - y0;
        }
        for (y = 0; y < h && y < 1024; ++y) texture_rows[y] = rows[y];
        C2VitaGLRecordOverlayRows(uploaded);
    }
#else
    glBindTexture(GL_TEXTURE_2D, root->asBack.overlayTexture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, root->pm_width, root->pm_height, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, root->asBack.lockedPixels);
#endif
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, root->asBack.glFbo);
    {
        int x,y,w,h;
        GLBackViewport(root,&x,&y,&w,&h);
#if C2_OVERLAY_FAST
        GLBackDrawTexture(root->screen, gl_overlay_draw_texture, x,y,w,h,0,1);
#else
        GLBackDrawTexture(root->screen, root->asBack.overlayTexture, x,y,w,h,0,1);
#endif
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    n = root->pm_width * root->pm_height;
    px = root->asBack.lockedPixels;
#if C2_OVERLAY_FAST
    {   /* only rows that held content need resetting; the rest is already key */
        int w = root->pm_width, h = root->pm_height, y;
        for (y = 0; y < h; ++y)
            if (y >= 1024 || gl_overlay_rows[y])
                GLOverlayFillKey(px + (size_t)y * w, (size_t)w, OVERLAY_KEY);
    }
#else
    for (i = 0; i < n; i++)
        px[i] = OVERLAY_KEY;
#endif
    root->asBack.possiblyDirty = 0;
}

/* Pixelmap rectangle (top-left origin, pixelmap units) -> FBO pixels (bottom-left origin). */
static void rect_to_fbo(br_device_pixelmap* self, br_device_pixelmap* root, const br_rectangle* r, GLint out[4]) {
    int s = GLBackScale(root);
    int x = self->pm_base_x + r->x;
    int y = self->pm_base_y + r->y;

    out[0] = x * s;
    out[1] = (root->pm_height - (y + r->h)) * s;
    out[2] = r->w * s;
    out[3] = r->h * s;
    /* Whole-target clears/copies include the unused menu borders as well. */
    if (root->screen != NULL && root->pm_width == root->screen->pm_width && root->pm_height == root->screen->pm_height
            && !(x == 0 && y == 0 && r->w == root->pm_width && r->h == root->pm_height))
        C2GLScaleUIRect(root->pm_width * s, root->pm_height * s,&out[0],&out[1],&out[2],&out[3]);
}

br_error GLBackRectangleFill(br_device_pixelmap* self, br_rectangle* rect, br_uint_32 colour) {
    br_device_pixelmap* root = root_of(self);
    br_rectangle r;
    GLint fr[4];

    if (PixelmapRectangleClip(&r, rect, (br_pixelmap*)self) == BR_CLIP_REJECT)
        return BRE_OK;

    /* keep the order: whatever the CPU drew before this fill goes in first */
    GLBackFlushOverlay(self);

    rect_to_fbo(self, root, &r, fr);
    glBindFramebuffer(GL_FRAMEBUFFER, root->asBack.glFbo);
    glEnable(GL_SCISSOR_TEST);
    glScissor(fr[0], fr[1], fr[2], fr[3]);
    if (self->use_type == BRT_DEPTH) {
        glDepthMask(GL_TRUE);
        glClearDepth(1.0);
        glClear(GL_DEPTH_BUFFER_BIT);
    } else {
        float cr, cg, cb;
        if (self->pm_type == BR_PMT_RGB_565) {
            cr = ((colour >> 11) & 0x1f) / 31.f;
            cg = ((colour >> 5) & 0x3f) / 63.f;
            cb = (colour & 0x1f) / 31.f;
        } else {
            cr = BR_RED(colour) / 255.f;
            cg = BR_GRN(colour) / 255.f;
            cb = BR_BLU(colour) / 255.f;
        }
        glClearColor(cr, cg, cb, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);
    }
    glDisable(GL_SCISSOR_TEST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    GL_CHECK_ERROR();
    return BRE_OK;
}

/* port: the frontend backdrop on a wide display, drawn straight into the FBO at a uniform zoom
 * that fills the width (cropping a little at the top and bottom) rather than into the overlay. */
static void draw_wide_backdrop(br_device_pixelmap* root, br_device_pixelmap* src) {
    static GLuint tex;
    static const void* cached_pixels;
    static br_uint_32 cached_sum;
    const br_uint_16* px = src->pm_pixels;
    br_uint_32 sum = 0;
    int s = GLBackScale(root), i, fbo_w, fbo_h, h;

    for (i = 0; i < 4096; i++) {
        int y = (i * 37) % src->pm_height, x = (i * 101) % src->pm_width;
        sum = sum * 31 + ((const br_uint_16*)((const br_uint_8*)px + y * src->pm_row_bytes))[x];
    }
    /* keep the order: CPU pixels drawn before the backdrop go first */
    GLBackFlushOverlay(root);
    if (tex == 0) {
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, root->pm_width, root->pm_height, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, NULL);
        cached_pixels = NULL;
    }
    glBindTexture(GL_TEXTURE_2D, tex);
    if (cached_pixels != src->pm_pixels || cached_sum != sum) {
        if (src->pm_row_bytes == root->pm_width * 2) {
            glPixelStorei(GL_UNPACK_ALIGNMENT, 2);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, root->pm_width, root->pm_height, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, src->pm_pixels);
        } else {
            for (i = 0; i < root->pm_height; i++) {
                glTexSubImage2D(GL_TEXTURE_2D, 0, 0, i, root->pm_width, 1, GL_RGB, GL_UNSIGNED_SHORT_5_6_5,
                    (const br_uint_8*)src->pm_pixels + i * src->pm_row_bytes);
            }
        }
        cached_pixels = src->pm_pixels;
        cached_sum = sum;
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    fbo_w = root->pm_width * s;
    fbo_h = root->pm_height * s;
    h = (int)(fbo_h * gGLPresentationAspect * 3.f / 4.f + .5f);
    glBindFramebuffer(GL_FRAMEBUFFER, root->asBack.glFbo);
    GLBackDrawTexture(root->screen, tex, 0, (fbo_h - h) / 2, fbo_w, h, 0, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    gGLFrameHasWide = 1;
}

/* Memory pixelmap -> device: written into the CPU overlay. */
br_error GLBackRectangleCopyTo(br_device_pixelmap* self, br_point* p, br_device_pixelmap* src, br_rectangle* sr) {
    br_device_pixelmap* root = root_of(self);
    br_rectangle r;
    br_point dp;
    br_uint_16* dst;
    br_uint_32* map = NULL;
    int x, y;

    if (self->use_type != BRT_OFFSCREEN)
        return BRE_UNSUPPORTED;
    if (PixelmapRectangleClipTwo(&r, &dp, sr, p, (br_pixelmap*)self, (br_pixelmap*)src) == BR_CLIP_REJECT)
        return BRE_OK;
    if (src->pm_pixels == NULL)
        return BRE_FAIL;

    GLBackEnsureOverlay(root);
    if (gGLWideBackdrop && !gGLWorldRendering && gGLPresentationAspect > 4.f / 3.f + .01f && self == root
            && root->screen != NULL && src->pm_type == BR_PMT_RGB_565 && dp.x == 0 && dp.y == 0
            && r.x == 0 && r.y == 0 && r.w == root->pm_width && r.h == root->pm_height) {
        draw_wide_backdrop(root, src);
        return BRE_OK;
    }
    if (src->pm_type == BR_PMT_INDEX_8) {
        if (src->pm_map != NULL)
            map = src->pm_map->pixels;
        else if (root->clut != NULL)
            map = root->clut->entries;
        else
            return BRE_FAIL;
    } else if (src->pm_type != BR_PMT_RGB_565) {
        return BRE_UNSUPPORTED;
    }

    for (y = 0; y < r.h; y++) {
        int sy = src->pm_base_y + r.y + y;
        int dy = self->pm_base_y + dp.y + y;
        dst = (br_uint_16*)root->asBack.lockedPixels + dy * root->pm_width + self->pm_base_x + dp.x;
        if (src->pm_type == BR_PMT_RGB_565) {
            const br_uint_16* s16 = (const br_uint_16*)((const br_uint_8*)src->pm_pixels + sy * src->pm_row_bytes) + src->pm_base_x + r.x;
            memcpy(dst, s16, r.w * sizeof(br_uint_16));
        } else {
            const br_uint_8* s8 = (const br_uint_8*)src->pm_pixels + sy * src->pm_row_bytes + src->pm_base_x + r.x;
            for (x = 0; x < r.w; x++) {
                br_uint_32 c = map[s8[x]];
                dst[x] = (br_uint_16)(((BR_RED(c) >> 3) << 11) | ((BR_GRN(c) >> 2) << 5) | (BR_BLU(c) >> 3));
            }
        }
    }
    root->asBack.possiblyDirty = 1;
    return BRE_OK;
}

/* Device -> device copy between back buffers. */
br_error GLBackRectangleCopy(br_device_pixelmap* self, br_point* p, br_device_pixelmap* src, br_rectangle* sr) {
    br_device_pixelmap* droot = root_of(self);
    br_device_pixelmap* sroot = root_of(src);
    br_rectangle r;
    br_point dp;
    br_rectangle dr;
    GLint s4[4], d4[4];

    if (ObjectDevice(src) != self->device || src->use_type != BRT_OFFSCREEN || self->use_type != BRT_OFFSCREEN)
        return BRE_UNSUPPORTED;
    if (PixelmapRectangleClipTwo(&r, &dp, sr, p, (br_pixelmap*)self, (br_pixelmap*)src) == BR_CLIP_REJECT)
        return BRE_OK;

    GLBackFlushOverlay(src);
    GLBackFlushOverlay(self);

    rect_to_fbo(src, sroot, &r, s4);
    dr.x = dp.x;
    dr.y = dp.y;
    dr.w = r.w;
    dr.h = r.h;
    rect_to_fbo(self, droot, &dr, d4);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, sroot->asBack.glFbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, droot->asBack.glFbo);
    glBlitFramebuffer(s4[0], s4[1], s4[0] + s4[2], s4[1] + s4[3], d4[0], d4[1], d4[0] + d4[2], d4[1] + d4[3],
        GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    GL_CHECK_ERROR();
    return BRE_OK;
}

#if C2_OVERLAY_FAST
void C2VitaOverlayBeginClean(void* pm) {
    br_device_pixelmap* self = (br_device_pixelmap*)pm;
    br_device_pixelmap* root;
    if (self == NULL || self->dispatch == NULL) return;
    root = root_of(self);
    if (root->use_type != BRT_OFFSCREEN || root->asBack.lockedPixels == NULL) return;
    gl_overlay_clean_root = NULL;
    GLBackFlushOverlay(self); /* leaves the buffer all key */
    gl_overlay_clean_root = root;
}
void C2VitaOverlayEndClean(void) {
    gl_overlay_clean_root = NULL;
}
#endif
