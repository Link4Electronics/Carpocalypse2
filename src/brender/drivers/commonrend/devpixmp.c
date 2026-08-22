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
    (void)self;
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
    ASSERT(self->pm_pixels != NULL);
    ASSERT(self->use_type == BRT_OFFSCREEN);

    self->pm_pixels = NULL;
    self->asBack.possiblyDirty = 1;
    self->asBack.locked = 0;

    return BRE_OK;
}
