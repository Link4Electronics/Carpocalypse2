#include "drv.h"
#include <brassert.h>
#include <string.h>
#include <stdlib.h>

#include "sdl3_shaders.h"

/*
 * Front-screen (device pixelmap) creation and the frame end (doubleBuffer)
 * path. The GPU-facing swap work lives in video.c (SDL3GPUREND_EnsureRecording /
 * SDL3GPUREND_BeginRenderPass / SDL3GPUREND_OverlayDraw / SDL3GPUREND_Present); this
 * file only owns the front's BRender object model, its template, and the
 * dispatch that routes most methods to the shared sdl3gpurend implementations
 * in commonrend/devpixmp.c.
 */

static const struct br_device_pixelmap_dispatch devicePixelmapFrontDispatch;

static br_uint_8 DeviceSDL3GPURENDTypeOrBits(br_uint_8 pixel_type, br_int_32 pixel_bits) {
    if (pixel_type != BR_PMT_MAX)
        return pixel_type;

    switch (pixel_bits) {
    case 16:
        return BR_PMT_RGB_565;
    case 24:
        return BR_PMT_RGB_888;
    case 32:
        return BR_PMT_RGBX_888;
    default:
        break;
    }

    return BR_PMT_MAX;
}

#define F(f) offsetof(br_device_pixelmap, f)
static struct br_tv_template_entry devicePixelmapFrontTemplateEntries[] = {
    { BRT(WIDTH_I32), F(pm_width), BRTV_QUERY | BRTV_ALL, BRTV_CONV_I32_U16, 0 },
    { BRT(HEIGHT_I32), F(pm_height), BRTV_QUERY | BRTV_ALL, BRTV_CONV_I32_U16, 0 },
    { BRT(PIXEL_TYPE_U8), F(pm_type), BRTV_QUERY | BRTV_ALL, BRTV_CONV_I32_U8, 0 },
    { BRT(OUTPUT_FACILITY_O), F(output_facility), BRTV_QUERY | BRTV_ALL, BRTV_CONV_POINTER, 0 },
    { BRT(FACILITY_O), F(output_facility), BRTV_QUERY, BRTV_CONV_POINTER, 0 },
    { BRT(IDENTIFIER_CSTR), F(pm_identifier), BRTV_QUERY | BRTV_ALL, BRTV_CONV_POINTER, 0 },
    { BRT(MSAA_SAMPLES_I32), F(msaa_samples), BRTV_QUERY | BRTV_ALL, BRTV_CONV_COPY, 0 },
    { BRT(SDL3GPU_CALLBACKS_P), 0, BRTV_QUERY | BRTV_ALL, BRTV_CONV_DIRECT },
    { BRT_CLUT_O, 0, F(clut), BRTV_QUERY | BRTV_ALL, BRTV_CONV_POINTER, 0 }
};
#undef F

struct pixelmapNewTokens {
    br_int_32 width;
    br_int_32 height;
    br_int_32 pixel_bits;
    br_uint_8 pixel_type;
    int msaa_samples;
    int debug_mode;
    br_device_sdl3gpu_callback_procs* callbacks;
};

#define F(f) offsetof(struct pixelmapNewTokens, f)
static struct br_tv_template_entry pixelmapNewTemplateEntries[] = {
    { BRT(WIDTH_I32), F(width), BRTV_SET, BRTV_CONV_COPY },
    { BRT(HEIGHT_I32), F(height), BRTV_SET, BRTV_CONV_COPY },
    { BRT(PIXEL_BITS_I32), F(pixel_bits), BRTV_SET, BRTV_CONV_COPY },
    { BRT(PIXEL_TYPE_U8), F(pixel_type), BRTV_SET, BRTV_CONV_COPY },
    { BRT(MSAA_SAMPLES_I32), F(msaa_samples), BRTV_SET, BRTV_CONV_COPY },
    { BRT(SDL3GPU_DEBUG_MODE), F(debug_mode), BRTV_SET, BRTV_CONV_COPY },
    { BRT(SDL3GPU_CALLBACKS_P), F(callbacks), BRTV_SET, BRTV_CONV_POINTER },
};
#undef F

/*
 * Frame end: flush the offscreen 2D content, composite the overlay inside the
 * frame's render pass, present, and hand control back to the host.
 */
static br_error BR_CMETHOD_DECL(br_device_pixelmap_sdl3gpurendf, doubleBuffer)(br_device_pixelmap* self,
    br_device_pixelmap* src) {
    if (self == src)
        return BRE_OK;

    if (ObjectDevice(src) != self->device)
        return BRE_UNSUPPORTED;

    if (self->use_type != BRT_NONE || src->use_type != BRT_OFFSCREEN)
        return BRE_UNSUPPORTED;

    BrPixelmapFlush((br_pixelmap*)src);

    HVIDEO hVideo = &self->asFront.video;

    SDL3GPUREND_EnsureRecording(hVideo);
    if (!hVideo->renderPassActive)
        SDL3GPUREND_BeginRenderPass(hVideo);

    /* Composite the 2D overlay (if any) inside the frame's render pass so it
     * lands on top of the GPU-rendered 3D content. */
    if (hVideo->overlayDirty)
        SDL3GPUREND_OverlayDraw(hVideo);

    SDL3GPUREND_EndRenderPass(hVideo);
    SDL3GPUREND_Present(hVideo);

    hVideo->frameFlushed = 0;
    hVideo->renderingStarted = 0;

    /* Call the platform swap callback (which includes the FPS limiter).
     * The GL driver does the same in ext_procs.c:DevicePixelmapGLSwapBuffers. */
    if (self->asFront.callbacks.swap_buffers)
        self->asFront.callbacks.swap_buffers((br_pixelmap*)self);

    BrRendererFrameBegin();

    return BRE_OK;
}

static void BR_CMETHOD_DECL(br_device_pixelmap_sdl3gpurendf, free)(br_object* _self) {
    br_device_pixelmap* self = (br_device_pixelmap*)_self;

    SDL3GPUREND_VideoClose(&self->asFront.video);

    if (self->asFront.video.lockedPixels) {
        BrMemFree(self->asFront.video.lockedPixels);
        self->asFront.video.lockedPixels = NULL;
    }

    if (self->asFront.callbacks.free)
        self->asFront.callbacks.free((br_pixelmap*)self, NULL);

    BrResFreeNoCallback(self);
}

struct br_tv_template* BR_CMETHOD_DECL(br_device_pixelmap_sdl3gpurendf, templateQuery)(br_object* _self) {
    br_device_pixelmap* self = (br_device_pixelmap*)_self;

    if (self->device->templates.devicePixelmapFrontTemplate == NULL)
        self->device->templates.devicePixelmapFrontTemplate = BrTVTemplateAllocate(
            self->device, devicePixelmapFrontTemplateEntries, BR_ASIZE(devicePixelmapFrontTemplateEntries));

    return self->device->templates.devicePixelmapFrontTemplate;
}

br_device_pixelmap* DevicePixelmapSDL3GPURENDAllocateFront(br_device* dev, br_output_facility* outfcty, br_token_value* tv) {
    br_device_pixelmap* self;
    br_int_32 count;
    struct pixelmapNewTokens pt = {
        .width = -1,
        .height = -1,
        .pixel_bits = -1,
        .pixel_type = BR_PMT_MAX,
        .msaa_samples = 0,
        .debug_mode = 0,
        .callbacks = NULL,
    };
    char tmp[80];

    if (dev->templates.pixelmapNewTemplate == NULL) {
        dev->templates.pixelmapNewTemplate = BrTVTemplateAllocate(dev, pixelmapNewTemplateEntries,
            BR_ASIZE(pixelmapNewTemplateEntries));
    }

    BrTokenValueSetMany(&pt, &count, NULL, tv, dev->templates.pixelmapNewTemplate);

    if (pt.callbacks == NULL || pt.width <= 0 || pt.height <= 0) {
        return NULL;
    }

    if ((pt.pixel_type = DeviceSDL3GPURENDTypeOrBits(pt.pixel_type, pt.pixel_bits)) == BR_PMT_MAX)
        return NULL;

    self = BrResAllocate(dev->res, sizeof(br_device_pixelmap), BR_MEMORY_OBJECT);

    BrSprintfN(tmp, sizeof(tmp) - 1, "SDL3GPU:Screen:%dx%d", pt.width, pt.height);
    self->pm_identifier = BrResStrDup(self, tmp);
    self->dispatch = &devicePixelmapFrontDispatch;
    self->device = dev;
    self->output_facility = outfcty;
    self->use_type = BRT_NONE;
    self->msaa_samples = pt.msaa_samples;
    self->screen = self;
    self->clut = dev->clut;

    self->pm_type = pt.pixel_type;
    self->pm_width = pt.width;
    self->pm_height = pt.height;
    self->pm_flags |= BR_PMF_NO_ACCESS;

    self->asFront.callbacks = *pt.callbacks;

    if (SDL3GPUREND_VideoOpen(&self->asFront.video, self,
            NULL, NULL, NULL,
            &self->asFront.callbacks, pt.width, pt.height,
            pt.debug_mode != 0) == NULL) {
        BrResFree(self);
        return NULL;
    }

    self->asFront.sdl3_device = self->asFront.video.device;
    self->asFront.sdl3_window = self->asFront.video.window;
    self->asFront.sdl3_version = NULL;
    self->asFront.sdl3_vendor = NULL;
    self->asFront.sdl3_renderer = NULL;

    BrLogPrintf("SDL3GPU: GPU Initialized\n");

    self->asFront.num_refs = 0;

    ObjectContainerAddFront(self->output_facility, (br_object*)self);
    return self;
}

static const struct br_device_pixelmap_dispatch devicePixelmapFrontDispatch = {
    .__reserved0 = NULL,
    .__reserved1 = NULL,
    .__reserved2 = NULL,
    .__reserved3 = NULL,
    ._free = BR_CMETHOD_REF(br_device_pixelmap_sdl3gpurendf, free),
    ._identifier = BREND_CMETHOD_REF(BREND_CLASS(br_device_pixelmap_), identifier),
    ._type = BREND_CMETHOD_REF(BREND_CLASS(br_device_pixelmap_), type),
    ._isType = BREND_CMETHOD_REF(BREND_CLASS(br_device_pixelmap_), isType),
    ._device = BREND_CMETHOD_REF(BREND_CLASS(br_device_pixelmap_), device),
    ._space = BREND_CMETHOD_REF(BREND_CLASS(br_device_pixelmap_), space),

    ._templateQuery = BR_CMETHOD_REF(br_device_pixelmap_sdl3gpurendf, templateQuery),
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
    ._fill = BREND_CMETHOD_REF(BREND_CLASS(br_device_pixelmap_), fill),
    ._doubleBuffer = BR_CMETHOD_REF(br_device_pixelmap_sdl3gpurendf, doubleBuffer),

    ._copyDirty = BR_CMETHOD_REF(br_device_pixelmap_gen, copyDirty),
    ._copyToDirty = BR_CMETHOD_REF(br_device_pixelmap_gen, copyToDirty),
    ._copyFromDirty = BR_CMETHOD_REF(br_device_pixelmap_gen, copyFromDirty),
    ._fillDirty = BR_CMETHOD_REF(br_device_pixelmap_gen, fillDirty),
    ._doubleBufferDirty = BR_CMETHOD_REF(br_device_pixelmap_gen, doubleBufferDirty),

    ._rectangle = BR_CMETHOD_REF(br_device_pixelmap_gen, rectangle),
    ._rectangle2 = BR_CMETHOD_REF(br_device_pixelmap_gen, rectangle2),
    ._rectangleCopy = BREND_CMETHOD_REF(BREND_CLASS(br_device_pixelmap_), rectangleCopy),
    ._rectangleCopyTo = BREND_CMETHOD_REF(BREND_CLASS(br_device_pixelmap_), rectangleCopyTo),
    ._rectangleCopyFrom = BR_CMETHOD_REF(br_device_pixelmap_fail, rectangleCopyFrom),
    ._rectangleStretchCopy = BREND_CMETHOD_REF(BREND_CLASS(br_device_pixelmap_), rectangleStretchCopy),
    ._rectangleStretchCopyTo = BR_CMETHOD_REF(br_device_pixelmap_fail, rectangleStretchCopyTo),
    ._rectangleStretchCopyFrom = BR_CMETHOD_REF(br_device_pixelmap_fail, rectangleStretchCopyFrom),
    ._rectangleFill = BREND_CMETHOD_REF(BREND_CLASS(br_device_pixelmap_), rectangleFill),
    ._pixelSet = BR_CMETHOD_REF(br_device_pixelmap_mem, pixelSet),
    ._line = BR_CMETHOD_REF(br_device_pixelmap_mem, line),
    ._copyBits = BR_CMETHOD_REF(br_device_pixelmap_fail, copyBits),

    ._text = BR_CMETHOD_REF(br_device_pixelmap_fail, text),
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
