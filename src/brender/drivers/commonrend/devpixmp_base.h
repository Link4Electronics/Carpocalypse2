/*
 * Shared private device pixelmap structure and method declarations for the
 * glrend/sdl3gpurend drivers. Compiled once per driver.
 */
#ifndef REND_DEVPIXMP_BASE_H_
#define REND_DEVPIXMP_BASE_H_

#include "commonrend.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Common br_device_pixelmap header fields, identical across drivers. Each
 * driver extends this with its backend-specific union payload.
 */
#define BR_DEVICE_PIXELMAP_BASE \
    const struct br_device_pixelmap_dispatch* dispatch; \
    const char* pm_identifier; \
    BR_PIXELMAP_MEMBERS \
    struct br_device* device; \
    struct br_output_facility* output_facility; \
    br_token use_type; \
    br_int_32 msaa_samples; \
    struct br_renderer* renderer; \
    struct br_device_pixelmap* screen; \
    br_uint_16 parent_height; \
    br_boolean sub_pixelmap

/*
 * Forward declarations for the shared methods defined in commonrend/devpixmp.c.
 * Expand after the br_device_pixelmap typedef in each driver's devpixmp.h.
 */
#define BR_DEVICE_PIXELMAP_COMMON_DECLS \
    extern void BREND_CMETHOD_DECL(BREND_CLASS(br_device_pixelmap_), free)(br_object* _self); \
    extern const char* BREND_CMETHOD_DECL(BREND_CLASS(br_device_pixelmap_), identifier)(br_object* self); \
    extern br_token BREND_CMETHOD_DECL(BREND_CLASS(br_device_pixelmap_), type)(br_object* self); \
    extern br_boolean BREND_CMETHOD_DECL(BREND_CLASS(br_device_pixelmap_), isType)(br_object* self, br_token t); \
    extern br_device* BREND_CMETHOD_DECL(BREND_CLASS(br_device_pixelmap_), device)(br_object* self); \
    extern br_size_t BREND_CMETHOD_DECL(BREND_CLASS(br_device_pixelmap_), space)(br_object* self); \
    extern struct br_tv_template* BREND_CMETHOD_DECL(BREND_CLASS(br_device_pixelmap_), templateQuery)(br_object* _self); \
    extern br_error BREND_CMETHOD_DECL(BREND_CLASS(br_device_pixelmap_), resize)(br_device_pixelmap* self, br_int_32 width, br_int_32 height); \
    extern br_error BREND_CMETHOD_DECL(BREND_CLASS(br_device_pixelmap_), match)(br_device_pixelmap* self, br_device_pixelmap** newpm, br_token_value* tv); \
    extern br_error BREND_CMETHOD_DECL(BREND_CLASS(br_device_pixelmap_), rectangleStretchCopy)(br_device_pixelmap* self, br_rectangle* d, br_device_pixelmap* src, br_rectangle* s); \
    extern br_error BREND_CMETHOD_DECL(BREND_CLASS(br_device_pixelmap_), rectangleCopy)(br_device_pixelmap* self, br_point* p, br_device_pixelmap* src, br_rectangle* sr); \
    extern br_error BREND_CMETHOD_DECL(BREND_CLASS(br_device_pixelmap_), rectangleCopyTo)(br_device_pixelmap* self, br_point* p, br_device_pixelmap* src, br_rectangle* sr); \
    extern br_error BREND_CMETHOD_DECL(BREND_CLASS(br_device_pixelmap_), rectangleFill)(br_device_pixelmap* self, br_rectangle* rect, br_uint_32 colour); \
    extern br_error BREND_CMETHOD_DECL(BREND_CLASS(br_device_pixelmap_), fill)(br_device_pixelmap* self, br_uint_32 colour); \
    extern br_error BREND_CMETHOD_DECL(BREND_CLASS(br_device_pixelmap_), allocateSub)(br_device_pixelmap* self, br_device_pixelmap** newpm, br_rectangle* rect); \
    extern br_error BREND_CMETHOD_DECL(BREND_CLASS(br_device_pixelmap_), flush)(br_device_pixelmap* self); \
    extern br_error BREND_CMETHOD_DECL(BREND_CLASS(br_device_pixelmap_), directLock)(br_device_pixelmap* self, br_boolean block); \
    extern br_error BREND_CMETHOD_DECL(BREND_CLASS(br_device_pixelmap_), directUnlock)(br_device_pixelmap* self); \
    /* Backend hooks: defined per driver (commonrend/devpixmp.c calls them). */ \
    extern br_error BREND_FN(DevicePixelmap, RecreateRenderBuffers)(br_device_pixelmap* self); \
    extern void BREND_FN(DevicePixelmap, DeleteResources)(br_device_pixelmap* self)

#ifdef __cplusplus
};
#endif
#endif /* REND_DEVPIXMP_BASE_H_ */
