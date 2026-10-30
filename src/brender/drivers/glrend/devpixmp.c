/*
 * Device pixelmap implementation, offscreen/back edition — OpenGL.
 *
 * The GL-specific halves of the (former) shared commonrend/devpixmp.c: the
 * OpenGL texture custom token, the offscreen/depth template, match (which
 * allocates the GL framebuffer/texture), the GL clear paths in
 * rectangleFill/rectangleCopyTo, the GL flush (locked-pixels -> overlay
 * texture -> framebuffer), directLock and the offscreen dispatch table. The
 * driver-agnostic object methods live in commonrend/devpixmp.c.
 */

#include "drv.h"
#include <brassert.h>
#include <brfont.h>
#include <string.h>

/*
 * Default dispatch table for device (defined at end of file)
 */
static const struct br_device_pixelmap_dispatch devicePixelmapDispatch;

static br_error custom_query(br_value* pvalue, void** extra, br_size_t* pextra_size, void* block, struct br_tv_template_entry* tep) {
    const br_device_pixelmap* self = block;

    if (tep->token == BRT_OPENGL_TEXTURE_U32) {
        if (self->use_type == BRT_OFFSCREEN)
            pvalue->u32 = self->asBack.glTex;
        else if (self->use_type == BRT_DEPTH)
#ifdef __vita__
            pvalue->u32 = 0; /* Native renderbuffer is not a sampled texture. */
#else
            pvalue->u32 = self->asDepth.glDepth;
#endif
        else
            pvalue->u32 = 0;

        return BRE_OK;
    }

    return BRE_UNKNOWN;
}

static const br_tv_custom custom = {
    .query = custom_query,
    .set = NULL,
    .extra_size = NULL,
};

/*
 * Device pixelmap info. template
 */
#define F(f) offsetof(struct br_device_pixelmap, f)
static struct br_tv_template_entry devicePixelmapTemplateEntries[] = {
    { BRT(WIDTH_I32), F(pm_width), BRTV_QUERY | BRTV_ALL, BRTV_CONV_I32_U16, 0 },
    { BRT(HEIGHT_I32), F(pm_height), BRTV_QUERY | BRTV_ALL, BRTV_CONV_I32_U16, 0 },
    { BRT(PIXEL_TYPE_U8), F(pm_type), BRTV_QUERY | BRTV_ALL, BRTV_CONV_I32_U8, 0 },
    { BRT(OUTPUT_FACILITY_O), F(output_facility), BRTV_QUERY | BRTV_ALL, BRTV_CONV_POINTER, 0 },
    { BRT(FACILITY_O), F(output_facility), BRTV_QUERY, BRTV_CONV_POINTER, 0 },
    { BRT(IDENTIFIER_CSTR), F(pm_identifier), BRTV_QUERY | BRTV_ALL, BRTV_CONV_POINTER, 0 },
    { BRT(MSAA_SAMPLES_I32), F(msaa_samples), BRTV_QUERY | BRTV_ALL, BRTV_CONV_COPY, 0 },
    { DEV(OPENGL_TEXTURE_U32), 0, BRTV_QUERY | BRTV_ALL, BRTV_CONV_CUSTOM, (br_uintptr_t)&custom },
};
#undef F

/*
 * Backend resource teardown, called from the shared free(). The GL driver
 * currently owns no per-pixelmap GPU state at free time; the old cleanup is
 * kept commented out for reference.
 */
void BREND_FN(DevicePixelmap, DeleteResources)(br_device_pixelmap* self) {
    if (self->use_type == BRT_DEPTH) {
#ifdef __vita__
        glDeleteRenderbuffers(1, &self->asDepth.glDepth);
#else
        glDeleteTextures(1, &self->asDepth.glDepth);
#endif
        self->asDepth.glDepth = 0;
    } else if (self->use_type == BRT_OFFSCREEN) {
        glDeleteFramebuffers(1, &self->asBack.glFbo);
        glDeleteTextures(1, &self->asBack.glTex);
        self->asBack.glFbo = 0;
        self->asBack.glTex = 0;
    }

    //BrLogPrintf("GLREND: Freeing %s", self->pm_identifier);

    // delete_gl_resources(self);

    // ObjectContainerRemove(self->output_facility, (br_object*)self);

    // --self->screen->asFront.num_refs;

    // BrResFreeNoCallback(self);
}

struct br_tv_template* BREND_CMETHOD_DECL(BREND_CLASS(br_device_pixelmap_), templateQuery)(br_object* _self) {
    br_device_pixelmap* self = (br_device_pixelmap*)_self;

    if (self->device->templates.devicePixelmapTemplate == NULL)
        self->device->templates.devicePixelmapTemplate = BrTVTemplateAllocate(self->device, devicePixelmapTemplateEntries,
            BR_ASIZE(devicePixelmapTemplateEntries));

    return self->device->templates.devicePixelmapTemplate;
}

/*
 * Structure used to unpack the 'match' tokens/values
 */
struct pixelmapMatchTokens {
    br_int_32 width;
    br_int_32 height;
    br_int_32 pixel_bits;
    br_uint_8 type;
    br_token use_type;
    br_int_32 msaa_samples;
};

#define F(f) offsetof(struct pixelmapMatchTokens, f)
static struct br_tv_template_entry pixelmapMatchTemplateEntries[] = {
    { BRT_WIDTH_I32, NULL, F(width), BRTV_SET, BRTV_CONV_COPY },
    { BRT_HEIGHT_I32, NULL, F(height), BRTV_SET, BRTV_CONV_COPY },
    { BRT_PIXEL_BITS_I32, NULL, F(pixel_bits), BRTV_SET, BRTV_CONV_COPY },
    { BRT_PIXEL_TYPE_U8, NULL, F(type), BRTV_SET, BRTV_CONV_COPY },
    { BRT_USE_T, NULL, F(use_type), BRTV_SET, BRTV_CONV_COPY },
    { BRT_MSAA_SAMPLES_I32, NULL, F(msaa_samples), BRTV_SET, BRTV_CONV_COPY },
};
#undef F

br_error BREND_CMETHOD_DECL(BREND_CLASS(br_device_pixelmap_), match)(br_device_pixelmap* self, br_device_pixelmap** newpm, br_token_value* tv) {
    br_int_32 count;
    br_error err;
    br_device_pixelmap* pm;
    const char* typestring;
    GLint gl_internal_format;
    GLenum gl_format, gl_type;
    GLsizeiptr gl_elem_bytes;
    HVIDEO hVideo;
    struct pixelmapMatchTokens mt = {
        .width = self->pm_width,
        .height = self->pm_height,
        .pixel_bits = -1,
        .type = BR_PMT_MAX,
        .use_type = BRT_NONE,
        .msaa_samples = 0,
    };
    char tmp[80];

    hVideo = &self->screen->asFront.video;

    if (self->device->templates.pixelmapMatchTemplate == NULL) {
        self->device->templates.pixelmapMatchTemplate = BrTVTemplateAllocate(self->device, pixelmapMatchTemplateEntries,
            BR_ASIZE(pixelmapMatchTemplateEntries));
    }

    err = BrTokenValueSetMany(&mt, &count, NULL, tv, self->device->templates.pixelmapMatchTemplate);
    if (err != BRE_OK)
        return err;

    if (mt.use_type == BRT_NO_RENDER)
        mt.use_type = BRT_OFFSCREEN;

    switch (mt.use_type) {
    case BRT_OFFSCREEN:
        typestring = "Backbuffer";
        break;
    case BRT_DEPTH:
        typestring = "Depth";

        /*
         * Depth buffers must be matched with the backbuffer.
         */
        if (self->use_type != BRT_OFFSCREEN)
            return BRE_UNSUPPORTED;

        /*
         * Can't have >1 depth buffer.
         */
        if (self->asBack.depthbuffer != NULL)
            return BRE_FAIL;

        /*
         * Not supporting non-16bpp depth buffers.
         */
        if (mt.pixel_bits != 16)
            return BRE_UNSUPPORTED;

        mt.type = BR_PMT_DEPTH_16;
        break;
    default:
        return BRE_UNSUPPORTED;
    }

    /*
     * Only allow backbuffers to be instantiated from the frontbuffer.
     */
    if (self->use_type == BRT_NONE && mt.use_type != BRT_OFFSCREEN)
        return BRE_UNSUPPORTED;

    if (mt.type == BR_PMT_MAX)
        mt.type = self->pm_type;

    err = VIDEOI_BrPixelmapGetTypeDetails(mt.type, &gl_internal_format, &gl_format, &gl_type, &gl_elem_bytes, NULL);
    if (err != BRE_OK)
        return err;

    if (mt.msaa_samples < 0)
        mt.msaa_samples = 0;
    else if (mt.msaa_samples > hVideo->maxSamples)
        mt.msaa_samples = hVideo->maxSamples;

    pm = BrResAllocate(self->device, sizeof(br_device_pixelmap), BR_MEMORY_OBJECT);
    memset(pm, 0, sizeof(br_device_pixelmap));
    pm->dispatch = &devicePixelmapDispatch;
    BrSprintfN(tmp, sizeof(tmp) - 1, "OpenGL:%s:%dx%d", typestring, mt.width, mt.height);
    pm->pm_identifier = BrResStrDup(self, tmp);
    pm->device = self->device;
    pm->output_facility = self->output_facility;
    pm->use_type = mt.use_type;
    pm->msaa_samples = mt.msaa_samples;
    pm->screen = self->screen;
    ++self->screen->asFront.num_refs;

    pm->pm_type = mt.type;
    pm->pm_width = mt.width;
    pm->pm_height = mt.height;
    pm->pm_row_bytes = gl_elem_bytes * mt.width;
    pm->pm_flags = BR_PMF_NO_ACCESS;
    pm->pm_origin_x = 0;
    pm->pm_origin_y = 0;
    pm->pm_base_x = 0;
    pm->pm_base_y = 0;
    pm->sub_pixelmap = 0;
    if (mt.use_type == BRT_OFFSCREEN) {
        pm->root = pm; /* FBO created by RecreateRenderBuffers */
    } else {
        pm->root = self->root != NULL ? self->root : self;
        ASSERT(mt.use_type == BRT_DEPTH);
        self->asBack.depthbuffer = pm;
        pm->asDepth.backbuffer = self;
    }

    if (BREND_FN(DevicePixelmap, RecreateRenderBuffers)(pm) != BRE_OK) {
        --self->screen->asFront.num_refs;
        BREND_FN(DevicePixelmap, DeleteResources)(pm);
        BrResFreeNoCallback(pm);
        return BRE_FAIL;
    }

    /* port: like the 3Dfx back buffer, the game may write pixels without locking
     * (headup text): keep the CPU overlay mapped all the time */
    if (mt.use_type == BRT_OFFSCREEN) {
        GLBackEnsureOverlay(pm);
        pm->pm_pixels = pm->asBack.lockedPixels;
        pm->pm_flags &= ~BR_PMF_NO_ACCESS;
    }

    /*
     * Copy origin over.
     */
    pm->pm_origin_x = self->pm_origin_x;
    pm->pm_origin_y = self->pm_origin_y;

    *newpm = pm;
    ObjectContainerAddFront(self->output_facility, (br_object*)pm);
    GL_CHECK_ERROR();
    return BRE_OK;
}

br_error GLBackRectangleFill(br_device_pixelmap* self, br_rectangle* rect, br_uint_32 colour);
br_error GLBackRectangleCopy(br_device_pixelmap* self, br_point* p, br_device_pixelmap* src, br_rectangle* sr);
br_error BREND_CMETHOD_DECL(BREND_CLASS(br_device_pixelmap_), rectangleFill)(br_device_pixelmap* self, br_rectangle* rect, br_uint_32 colour) {
    return GLBackRectangleFill(self, rect, colour); /* backbuf.c */
}

br_error GLBackRectangleCopyTo(br_device_pixelmap* self, br_point* p, br_device_pixelmap* src, br_rectangle* sr);
br_error BREND_CMETHOD_DECL(BREND_CLASS(br_device_pixelmap_), rectangleCopyTo)(br_device_pixelmap* self, br_point* p,
    br_device_pixelmap* src, br_rectangle* sr) {
    return GLBackRectangleCopyTo(self, p, src, sr); /* backbuf.c */
}

/*
 * Device->Pixelmap, addressable same-size copy.
 */
br_error BR_CMETHOD_DECL(br_device_pixelmap_gl, rectangleCopyFrom)(br_device_pixelmap* self, br_point* p,
    br_device_pixelmap* dest, br_rectangle* r) {
    br_error err;
    GLint internalFormat;
    GLenum format, type;
    GLsizeiptr elemBytes;
    void* rowTemp;

    (void)self;
    (void)p;
    (void)dest;
    (void)r;
    (void)err;
    (void)internalFormat;
    (void)format;
    (void)type;
    (void)elemBytes;
    (void)rowTemp;

    return BRE_FAIL;
}

br_error BR_CMETHOD(br_device_pixelmap_gl, rectangleStretchCopyTo)(br_device_pixelmap* self, br_rectangle* dr,
    br_device_pixelmap* _src, br_rectangle* sr) {
    (void)self;
    (void)dr;
    (void)_src;
    (void)sr;
    return BRE_FAIL;
}

/*
 * Font atlas management. Glyphs are stamped into a 16x16 grid of
 * glyph_x*glyph_y cells (cell c = column c&15, row c>>4); glyph bit 0x80 is
 * the left-most pixel, the glyph's top row is the first row of its cell. Set
 * pixels are opaque white, empty pixels transparent, so the fragment shader
 * can blend glyph coverage over the scene.
 */
static GLuint DeviceGLTextBuildAtlas(HVIDEO hVideo, br_font* font, int* outW, int* outH) {
    int gw = font->glyph_x;
    int gh = font->glyph_y;
    int aw = gw * 16;
    int ah = gh * 16;
    br_uint_8* rgba;
    GLuint tex;
    int c, px, py;

    rgba = BrScratchAllocate((size_t)aw * (size_t)ah * 4);
    memset(rgba, 0, (size_t)aw * (size_t)ah * 4);

    for (c = 0; c < 256; c++) {
        int col = c & 15;
        int row = c >> 4;
        int w = (font->flags & BR_FONTF_PROPORTIONAL) ? font->width[c] : gw;
        int stride = (w + 7) / 8;
        const br_uint_8* glyph = font->glyphs + font->encoding[c];
        for (py = 0; py < gh; py++) {
            for (px = 0; px < w; px++) {
                if (glyph[(py * stride) + (px / 8)] & (0x80 >> (px % 8))) {
                    br_uint_8* dst = &rgba[((row * gh + py) * aw + (col * gw + px)) * 4];
                    dst[0] = 0xFF;
                    dst[1] = 0xFF;
                    dst[2] = 0xFF;
                    dst[3] = 0xFF;
                }
            }
        }
    }

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, aw, ah, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba);
    glBindTexture(GL_TEXTURE_2D, 0);

    BrScratchFree(rgba);

    {
        int slot = hVideo->textAtlasCount < TEXT_ATLAS_CACHE_MAX ? hVideo->textAtlasCount++ : hVideo->textAtlasReplace;
        if (hVideo->textAtlas[slot].texture != 0)
            glDeleteTextures(1, &hVideo->textAtlas[slot].texture);
        hVideo->textAtlas[slot].font = font;
        hVideo->textAtlas[slot].texture = tex;
        hVideo->textAtlas[slot].atlasWidth = aw;
        hVideo->textAtlas[slot].atlasHeight = ah;
        hVideo->textAtlasReplace = (hVideo->textAtlasReplace + 1) % TEXT_ATLAS_CACHE_MAX;
    }

    *outW = aw;
    *outH = ah;
    GL_CHECK_ERROR();
    return tex;
}

static GLuint DeviceGLTextEnsureAtlas(HVIDEO hVideo, br_font* font, int* outW, int* outH) {
    int i;

    for (i = 0; i < hVideo->textAtlasCount; i++) {
        if (hVideo->textAtlas[i].font == font) {
            *outW = hVideo->textAtlas[i].atlasWidth;
            *outH = hVideo->textAtlas[i].atlasHeight;
            return hVideo->textAtlas[i].texture;
        }
    }

    return DeviceGLTextBuildAtlas(hVideo, font, outW, outH);
}

static void DeviceGLTextColour(br_device_pixelmap* self, br_uint_32 colour, float* out) {
    float r, g, b, a;
    br_uint_32* entries = self->clut != NULL ? self->clut->entries : ObjectDevice(self)->clut->entries;

    /* Palette-index colours (0-255, as used by BrPixelmapText on palette-based
     * games such as Carmageddon) are resolved through the device CLUT on any
     * pixel type, matching the software renderer where the palette is applied
     * when the INDEX_8 screen is displayed. Larger values are direct RGB. */
    if ((self->pm_type == BR_PMT_INDEX_8 || colour <= 0xFF) && entries != NULL) {
        br_uint_32 entry = entries[colour & 0xFF];
        r = BR_RED(entry) / 255.0f;
        g = BR_GRN(entry) / 255.0f;
        b = BR_BLU(entry) / 255.0f;
        a = 1.0f;
    } else {
        r = BR_RED(colour) / 255.0f;
        g = BR_GRN(colour) / 255.0f;
        b = BR_BLU(colour) / 255.0f;
        /* Sub-24bpp pixelmaps do not store alpha. */
        a = (self->pm_type == BR_PMT_RGBA_8888 || self->pm_type == BR_PMT_RGBA_4444 ||
                self->pm_type == BR_PMT_ARGB_4444) ?
            BR_ALPHA(colour) / 255.0f :
            1.0f;
    }

    out[0] = r;
    out[1] = g;
    out[2] = b;
    out[3] = a;
}

br_error BR_CMETHOD(br_device_pixelmap_gl, text)(br_device_pixelmap* self, br_point* point, br_font* font,
    const char* text, br_uint_32 colour) {
    /* The text quad lives for the process lifetime (the GL device holds a
     * single context for its whole life); it is lazily initialised below. */
    static br_device_pixelmap_gl_quad s_textQuad;
    HVIDEO hVideo;
    GLuint atlas;
    int x, y, gw, gh, aw, ah;
    float colour4[4];
    int vx, vy;
    float rx, ry;
    GLint prevProgram, prevViewport[4];
    GLboolean blendEnabled, depthEnabled;
    GLint prevTexture, prevFramebuffer;
    int count;
    const unsigned char* s;

    if (self->screen == NULL)
        return BRE_FAIL;

    hVideo = &self->screen->asFront.video;

    if (font == NULL || text == NULL)
        return BRE_OK;

    x = point->x + self->pm_origin_x;
    y = point->y + self->pm_origin_y;

    if (y <= -(int)font->glyph_y || y >= self->pm_height || x >= self->pm_width)
        return BRE_OK;

    atlas = DeviceGLTextEnsureAtlas(hVideo, font, &aw, &ah);
    if (atlas == 0)
        return BRE_FAIL;

    DeviceGLTextColour(self, colour, colour4);

    if (s_textQuad.buffers[0] == 0)
        DeviceGLInitQuad(&s_textQuad, hVideo);
    if (s_textQuad.buffers[0] == 0)
        return BRE_FAIL;

    /* Save the GL state we are about to touch. */
    glGetIntegerv(GL_CURRENT_PROGRAM, &prevProgram);
    blendEnabled = glIsEnabled(GL_BLEND);
    depthEnabled = glIsEnabled(GL_DEPTH_TEST);
    glGetIntegerv(GL_VIEWPORT, prevViewport);
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &prevTexture);

    /* Text is addressed in game pixels, like the scene output. */
    glGetIntegerv(GL_FRAMEBUFFER_BINDING,&prevFramebuffer);
    if (self->use_type == BRT_OFFSCREEN) {
        int vw,vh;
        GLBackBindTarget(self);
        GLBackViewport(self,&vx,&vy,&vw,&vh);
        glViewport(vx,vy,vw,vh);
    } else {
        BREND_FN(DevicePixelmap, GetViewport)(self->screen, &vx, &vy, &rx, &ry);
        glViewport(vx,vy,(GLsizei)(self->pm_width*rx),(GLsizei)(self->pm_height*ry));
    }

    glUseProgram(hVideo->textProgram.program);
    glUniform4f(hVideo->textProgram.uTextColour, colour4[0], colour4[1], colour4[2], colour4[3]);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, atlas);
    glUniform1i(hVideo->textProgram.uSampler, 0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    glBindVertexArray(s_textQuad.defaultVao);

    gw = font->glyph_x;
    gh = font->glyph_y;

    count = 0;
    for (s = (const unsigned char*)text; *s != '\0' && count < 256; s++) {
        int c = *s;
        int w = (font->flags & BR_FONTF_PROPORTIONAL) ? font->width[c] : gw;
        float dx0, dy0, dx1, dy1, u0, v0, u1, v1;
        int col, row;

        if (x + w <= 0) {
            x += w + 1;
            continue;
        }
        if (x >= self->pm_width)
            break;

        col = c & 15;
        row = c >> 4;
        u0 = (float)(col * gw) / (float)aw;
        u1 = (float)(col * gw + w) / (float)aw;
        v0 = (float)(row * gh) / (float)ah;
        v1 = (float)(row * gh + gh) / (float)ah;

        /* OpenGL NDC is y-up: clip row 0 (game top) maps to NDC +1, and
         * memory row 0 is sampled at v=0, so the glyph's top edge carries
         * v = cell top. */
        dx0 = 2.0f * (float)x / (float)self->pm_width - 1.0f;
        dx1 = 2.0f * (float)(x + w) / (float)self->pm_width - 1.0f;
        dy0 = 1.0f - 2.0f * (float)(y + gh) / (float)self->pm_height;
        dy1 = 1.0f - 2.0f * (float)y / (float)self->pm_height;

        /* Bottom-left, top-left, top-right, bottom-right (matches defaultVao
         * and the quad index buffer). */
        s_textQuad.tris[0].x = dx0;
        s_textQuad.tris[0].y = dy0;
        s_textQuad.tris[0].z = 0.5f;
        s_textQuad.tris[0].r = 1.0f;
        s_textQuad.tris[0].g = 1.0f;
        s_textQuad.tris[0].b = 1.0f;
        s_textQuad.tris[0].u = u0;
        s_textQuad.tris[0].v = v1;

        s_textQuad.tris[1].x = dx0;
        s_textQuad.tris[1].y = dy1;
        s_textQuad.tris[1].z = 0.5f;
        s_textQuad.tris[1].r = 1.0f;
        s_textQuad.tris[1].g = 1.0f;
        s_textQuad.tris[1].b = 1.0f;
        s_textQuad.tris[1].u = u0;
        s_textQuad.tris[1].v = v0;

        s_textQuad.tris[2].x = dx1;
        s_textQuad.tris[2].y = dy1;
        s_textQuad.tris[2].z = 0.5f;
        s_textQuad.tris[2].r = 1.0f;
        s_textQuad.tris[2].g = 1.0f;
        s_textQuad.tris[2].b = 1.0f;
        s_textQuad.tris[2].u = u1;
        s_textQuad.tris[2].v = v0;

        s_textQuad.tris[3].x = dx1;
        s_textQuad.tris[3].y = dy0;
        s_textQuad.tris[3].z = 0.5f;
        s_textQuad.tris[3].r = 1.0f;
        s_textQuad.tris[3].g = 1.0f;
        s_textQuad.tris[3].b = 1.0f;
        s_textQuad.tris[3].u = u1;
        s_textQuad.tris[3].v = v1;

        glBindBuffer(GL_ARRAY_BUFFER, s_textQuad.buffers[0]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(s_textQuad.tris), s_textQuad.tris);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

        x += w + 1;
        count++;
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(prevProgram);
    glBindFramebuffer(GL_FRAMEBUFFER,prevFramebuffer);
    glViewport(prevViewport[0], prevViewport[1], prevViewport[2], prevViewport[3]);
    if (blendEnabled)
        glEnable(GL_BLEND);
    else
        glDisable(GL_BLEND);
    if (depthEnabled)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);

    GL_CHECK_ERROR();
    return BRE_OK;
}

br_error BREND_CMETHOD_DECL(BREND_CLASS(br_device_pixelmap_), flush)(br_device_pixelmap* self) {
    GLBackFlushOverlay(self); /* backbuf.c */
    return BRE_OK;
}

br_error BREND_CMETHOD_DECL(BREND_CLASS(br_device_pixelmap_), directLock)(br_device_pixelmap* self, br_boolean block) {
    br_device_pixelmap* root = self->root != NULL ? self->root : self;
    (void)block;

    if (self->use_type != BRT_OFFSCREEN)
        return BRE_FAIL;

    GLBackEnsureOverlay(root);
    /* sub-pixelmaps address the root's pixels through pm_base_x/y */
    self->pm_pixels = root->asBack.lockedPixels;
    self->pm_row_bytes = root->pm_width * 2;
    self->asBack.locked = 1;
    root->asBack.possiblyDirty = 1;
    return BRE_OK;
}

/*
 * Default dispatch table for device pixelmap
 */
static const struct br_device_pixelmap_dispatch devicePixelmapDispatch = {
    .__reserved0 = NULL,
    .__reserved1 = NULL,
    .__reserved2 = NULL,
    .__reserved3 = NULL,
    ._free = BREND_CMETHOD_REF(BREND_CLASS(br_device_pixelmap_), free),
    ._identifier = BREND_CMETHOD_REF(BREND_CLASS(br_device_pixelmap_), identifier),
    ._type = BREND_CMETHOD_REF(BREND_CLASS(br_device_pixelmap_), type),
    ._isType = BREND_CMETHOD_REF(BREND_CLASS(br_device_pixelmap_), isType),
    ._device = BREND_CMETHOD_REF(BREND_CLASS(br_device_pixelmap_), device),
    ._space = BREND_CMETHOD_REF(BREND_CLASS(br_device_pixelmap_), space),

    ._templateQuery = BREND_CMETHOD_REF(BREND_CLASS(br_device_pixelmap_), templateQuery),
    ._query = BR_CMETHOD_REF(br_object, query),
    ._queryBuffer = BR_CMETHOD_REF(br_object, queryBuffer),
    ._queryMany = BR_CMETHOD_REF(br_object, queryMany),
    ._queryManySize = BR_CMETHOD_REF(br_object, queryManySize),
    ._queryAll = BR_CMETHOD_REF(br_object, queryAll),
    ._queryAllSize = BR_CMETHOD_REF(br_object, queryAllSize),

    ._validSource = BR_CMETHOD_REF(br_device_pixelmap_mem, validSource),
    ._resize = BREND_CMETHOD_REF(BREND_CLASS(br_device_pixelmap_), resize),
    ._match = BREND_CMETHOD_REF(BREND_CLASS(br_device_pixelmap_), match),
    ._allocateSub = BREND_CMETHOD_REF(BREND_CLASS(br_device_pixelmap_), allocateSub),

    ._copy = BR_CMETHOD_REF(br_device_pixelmap_gen, copy),
    ._copyTo = BR_CMETHOD_REF(br_device_pixelmap_gen, copyTo),
    ._copyFrom = BR_CMETHOD_REF(br_device_pixelmap_gen, copyFrom),
    ._fill = BR_CMETHOD_REF(br_device_pixelmap_gen, fill),
    ._doubleBuffer = BR_CMETHOD_REF(br_device_pixelmap_fail, doubleBuffer),

    ._copyDirty = BR_CMETHOD_REF(br_device_pixelmap_gen, copyDirty),
    ._copyToDirty = BR_CMETHOD_REF(br_device_pixelmap_gen, copyToDirty),
    ._copyFromDirty = BR_CMETHOD_REF(br_device_pixelmap_gen, copyFromDirty),
    ._fillDirty = BR_CMETHOD_REF(br_device_pixelmap_gen, fillDirty),
    ._doubleBufferDirty = BR_CMETHOD_REF(br_device_pixelmap_gen, doubleBufferDirty),

    ._rectangle = BR_CMETHOD_REF(br_device_pixelmap_gen, rectangle),
    ._rectangle2 = BR_CMETHOD_REF(br_device_pixelmap_gen, rectangle2),
    ._rectangleCopy = GLBackRectangleCopy, /* backbuf.c */
    ._rectangleCopyTo = BREND_CMETHOD_REF(BREND_CLASS(br_device_pixelmap_), rectangleCopyTo),
    ._rectangleCopyFrom = BR_CMETHOD_REF(br_device_pixelmap_gl, rectangleCopyFrom),
    ._rectangleStretchCopy = BREND_CMETHOD_REF(BREND_CLASS(br_device_pixelmap_), rectangleStretchCopy),
    ._rectangleStretchCopyTo = BR_CMETHOD_REF(br_device_pixelmap_gl, rectangleStretchCopyTo),
    ._rectangleStretchCopyFrom = BR_CMETHOD_REF(br_device_pixelmap_fail, rectangleStretchCopyFrom),
    ._rectangleFill = BREND_CMETHOD_REF(BREND_CLASS(br_device_pixelmap_), rectangleFill),
    ._pixelSet = BR_CMETHOD_REF(br_device_pixelmap_mem, pixelSet),
    ._line = BR_CMETHOD_REF(br_device_pixelmap_mem, line),
    ._copyBits = BR_CMETHOD_REF(br_device_pixelmap_fail, copyBits),

    ._text = BR_CMETHOD_REF(br_device_pixelmap_gl, text),
    ._textBounds = BR_CMETHOD_REF(br_device_pixelmap_gen, textBounds),

    ._rowSize = BR_CMETHOD_REF(br_device_pixelmap_fail, rowSize),
    ._rowQuery = BR_CMETHOD_REF(br_device_pixelmap_fail, rowQuery),
    ._rowSet = BR_CMETHOD_REF(br_device_pixelmap_fail, rowSet),

    ._pixelQuery = BR_CMETHOD_REF(br_device_pixelmap_fail, pixelQuery),
    ._pixelAddressQuery = BR_CMETHOD_REF(br_device_pixelmap_fail, pixelAddressQuery),

    ._pixelAddressSet = BR_CMETHOD_REF(br_device_pixelmap_fail, pixelAddressSet),
    ._originSet = BR_CMETHOD_REF(br_device_pixelmap_mem, originSet),

    ._flush = BREND_CMETHOD_REF(BREND_CLASS(br_device_pixelmap_), flush),
    ._synchronise = BR_CMETHOD_REF(br_device_pixelmap_fail, synchronise),
    ._directLock = BREND_CMETHOD_REF(BREND_CLASS(br_device_pixelmap_), directLock),
    ._directUnlock = BREND_CMETHOD_REF(BREND_CLASS(br_device_pixelmap_), directUnlock)
};
