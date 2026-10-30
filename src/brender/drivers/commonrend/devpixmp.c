/*
 * Device pixelmap methods shared by the glrend/sdl3gpurend drivers.
 *
 * Only the driver-agnostic object bookkeeping lives here: the object methods
 * (identifier/type/isType/device/space), free (which delegates teardown to the
 * backend hook BREND_FN(DevicePixelmap, DeleteResources)), resize, the
 * CPU-side rectangle stubs, allocateSub and directUnlock. Everything that
 * touches backend state (custom token, match, rectangleFill/rectangleCopyTo,
 * fill, flush, directLock and the offscreen dispatch table) is implemented per
 * driver in glrend/devpixmp.c and sdl3gpurend/devpixmp.c.
 *
 * Note: rectangleFill's 565/index colour conversion (in the per-driver files)
 * follows the Vulkan driver's BR_RED/GRN/BLU decode (correct BRender
 * br_colour semantics). It is bit-identical to GL's old `colour & 0xffff` /
 * BR_ALPHA(colour) interpretation for the values the game actually uses (0 and
 * 0xFFFFFFFF).
 */

#include "brassert.h"
#include "drv.h"

/*
 * (Re)create the renderbuffers and attach them to the framebuffer.
 */
br_error BREND_FN(DevicePixelmap, RecreateRenderBuffers)(br_device_pixelmap* self) {
#if defined(BREND_DRIVER_GL)
    /* port: from BRender 1.4 glrend (the copy we got from upstream had this stubbed out).
     * Colour targets are RGBA8 and depth is DEPTH_COMPONENT24: both are renderable on
     * GL 3.2 core and GLES 3.0. */
    GLuint fbo = 0;

    if (self->use_type == BRT_OFFSCREEN) {
        const GLenum draw_buffers[1] = { GL_COLOR_ATTACHMENT0 };

        if (self->asBack.glFbo == 0)
            glGenFramebuffers(1, &self->asBack.glFbo);
        fbo = self->asBack.glFbo;

        if (self->asBack.glTex != 0)
            glDeleteTextures(1, &self->asBack.glTex);
        glGenTextures(1, &self->asBack.glTex);
        glBindTexture(GL_TEXTURE_2D, self->asBack.glTex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, self->pm_width * GLBackScale(self), self->pm_height * GLBackScale(self), 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glBindTexture(GL_TEXTURE_2D, 0);

        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, self->asBack.glTex, 0);
#ifndef __vita__
        glDrawBuffers(1, draw_buffers);
#endif
    } else if (self->use_type == BRT_DEPTH) {
        fbo = self->asDepth.backbuffer->asBack.glFbo;

#ifdef __vita__
        /* VitaGL allocates depth storage with its render target. Its ordinary
         * texture upload API does not accept desktop depth texture formats. */
        if (self->asDepth.glDepth != 0)
            glDeleteRenderbuffers(1, &self->asDepth.glDepth);
        glGenRenderbuffers(1, &self->asDepth.glDepth);
        glBindRenderbuffer(GL_RENDERBUFFER, self->asDepth.glDepth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24,
            self->pm_width * GLBackScale(self), self->pm_height * GLBackScale(self));
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, self->asDepth.glDepth);
#else
        if (self->asDepth.glDepth != 0)
            glDeleteTextures(1, &self->asDepth.glDepth);
        glGenTextures(1, &self->asDepth.glDepth);
        glBindTexture(GL_TEXTURE_2D, self->asDepth.glDepth);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, self->pm_width * GLBackScale(self), self->pm_height * GLBackScale(self), 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
        glBindTexture(GL_TEXTURE_2D, 0);

        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, self->asDepth.glDepth, 0);
#endif
    } else {
        return BRE_OK;
    }

    {
        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            BrWarning("glrend: framebuffer %s incomplete (0x%x)", self->pm_identifier, (unsigned)status);
            return BRE_FAIL;
        }
    }
    GL_CHECK_ERROR();
#else
    (void)self;
#endif
    return BRE_OK;
}

void BREND_CMETHOD_DECL(BREND_CLASS(br_device_pixelmap_), free)(br_object* _self) {
    br_device_pixelmap* self = (br_device_pixelmap*)_self;

    if (self->sub_pixelmap) {
        return;
    }

    BREND_FN(DevicePixelmap, DeleteResources)(self);
}

const char* BREND_CMETHOD_DECL(BREND_CLASS(br_device_pixelmap_), identifier)(br_object* self) {
    return ((br_device_pixelmap*)self)->pm_identifier;
}

br_token BREND_CMETHOD_DECL(BREND_CLASS(br_device_pixelmap_), type)(br_object* self) {
    (void)self;
    return BRT_DEVICE_PIXELMAP;
}

br_boolean BREND_CMETHOD_DECL(BREND_CLASS(br_device_pixelmap_), isType)(br_object* self, br_token t) {
    (void)self;
    return (t == BRT_DEVICE_PIXELMAP) || (t == BRT_OBJECT);
}

br_device* BREND_CMETHOD_DECL(BREND_CLASS(br_device_pixelmap_), device)(br_object* self) {
    (void)self;
    return ((br_device_pixelmap*)self)->device;
}

br_size_t BREND_CMETHOD_DECL(BREND_CLASS(br_device_pixelmap_), space)(br_object* self) {
    (void)self;
    return sizeof(br_device_pixelmap);
}

br_error BREND_CMETHOD_DECL(BREND_CLASS(br_device_pixelmap_), resize)(br_device_pixelmap* self, br_int_32 width, br_int_32 height) {
    self->pm_width = width;
    self->pm_height = height;
    return BREND_FN(DevicePixelmap, RecreateRenderBuffers)(self);
}

br_error BREND_CMETHOD_DECL(BREND_CLASS(br_device_pixelmap_), rectangleStretchCopy)(br_device_pixelmap* self, br_rectangle* d,
    br_device_pixelmap* src, br_rectangle* s) {

    (void)self;
    (void)d;
    (void)src;
    (void)s;
    return BRE_FAIL;
}

br_error BREND_CMETHOD_DECL(BREND_CLASS(br_device_pixelmap_), rectangleCopy)(br_device_pixelmap* self, br_point* p,
    br_device_pixelmap* src, br_rectangle* sr) {

    (void)self;
    (void)p;
    (void)src;
    (void)sr;
    return BRE_FAIL;
}

br_error BREND_CMETHOD_DECL(BREND_CLASS(br_device_pixelmap_), allocateSub)(br_device_pixelmap* self, br_device_pixelmap** newpm, br_rectangle* rect) {
    br_device_pixelmap* pm;
    br_rectangle out;

    /*
     * Create the new structure and copy
     */
    pm = BrResAllocate(self->device, sizeof(*pm), BR_MEMORY_PIXELMAP);

    /*
     * Set all the fields to be the same as the parent pixelmap for now
     */
    *pm = *self;

    /*
     * Create sub-window (clipped against original)
     */
    if (PixelmapRectangleClip(&out, rect, (br_pixelmap*)self) == BR_CLIP_REJECT)
        return BRE_FAIL;

    pm->sub_pixelmap = BR_TRUE;
    pm->parent_height = self->pm_height;

    /*
     * Pixel rows are not contiguous
     */
    if (out.w != self->pm_width)
        pm->pm_flags &= ~BR_PMF_LINEAR;

    pm->pm_base_x += out.x;
    pm->pm_base_y += out.y;

    pm->pm_width = out.w;
    pm->pm_height = out.h;

    pm->pm_origin_x = 0;
    pm->pm_origin_y = 0;

    *newpm = (br_device_pixelmap*)pm;

    return BRE_OK;
}

br_error BREND_CMETHOD_DECL(BREND_CLASS(br_device_pixelmap_), directUnlock)(br_device_pixelmap* self) {
    ASSERT(self->use_type == BRT_OFFSCREEN);

    self->asBack.locked = 0;
#if defined(BREND_DRIVER_GL)
    /* port: the overlay stays mapped (backbuf.c) */
    if (self->root != NULL) {
        self->pm_pixels = self->root->asBack.lockedPixels;
        self->root->asBack.possiblyDirty = 1;
    }
#elif defined(BREND_DRIVER_SDL3GPUREND)
    /* port: same contract as glrend — the offscreen CPU overlay is mapped for
     * the whole life of the pixelmap (devpixmp.c pixelmapMatch) because the
     * game writes headup text/screen captures without locking. PossibleUnlock()
     * is called all over the game, so clearing pm_pixels here left gBack_screen
     * with NULL pixels and DRPixelmapText() wrote through it. */
    if (self->use_type == BRT_OFFSCREEN) {
        self->pm_pixels = self->asBack.lockedPixels;
        self->asBack.possiblyDirty = 1;
    }
#else
    self->pm_pixels = NULL;
    self->asBack.possiblyDirty = 1;
#endif

    return BRE_OK;
}
