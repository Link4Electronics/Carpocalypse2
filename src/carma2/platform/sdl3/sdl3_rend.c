/*
 * Minimal BRender output device for the SDL3 platform.
 *
 * The retail game begins its front screen with
 *   BrDevBeginVar(&gScreen, <renderer>, WIDTH/HEIGHT/PIXEL_TYPE/
 *                                WINDOW_FULLSCREEN/WINDOW_HANDLE, ...)
 * which requires a registered BRender device exposing an OUTPUT_FACILITY.
 *
 * This module provides such a device ("SDLREND"). The screen pixelmap itself
 * is a plain memory-backed device pixelmap (core/pixelmap/pmmem.c); presenting
 * its pixels to the SDL window is handled by the surrounding platform code.
 *
 * Patterns follow drivers/d3d/outfcty.c, which targets the same core.
 */

#include <SDL3/SDL.h>

#include <brender/brender.h>
#include "core/fw/devlist.h"
#include "core/fw/devsetup.h"
#include "core/fw/object.h"
#include "core/fw/objectc.h"
#include "core/fw/resource.h"
#include "core/fw/tokenval.h"
#include "core/pixelmap/pixelmap.h"
#include "core/pixelmap/pmmem.h"

#include "errors.h"

/* From drivers/softrend/device.c */
extern br_device* C2_HOOK_STDCALL DeviceSoftAllocate(const char* identifier);

/* From sdl3_plib.c */
extern br_error PrimitiveLibrarySDLAllocate(br_device* dev, void** ppl);

typedef struct br_output_facility_sdl {
    br_output_facility_dispatch* dispatch;

    char* identifier;
    br_device* device;
    void* object_list;
    int num_instances;

    int width;
    int height;
    int colour_bits;
    int colour_type;
    int indexed;
    int fullscreen;
} br_output_facility_sdl;

static br_tv_template_entry outputFacilitySDLTemplateEntries[] = {
    { BRT_IDENTIFIER_CSTR,      0,  offsetof(br_output_facility_sdl, identifier),   0x5,    0x3, },
    { BRT_WIDTH_I32,            0,  offsetof(br_output_facility_sdl, width),        0x5,    0x3, },
    { BRT_WIDTH_MIN_I32,        0,  offsetof(br_output_facility_sdl, width),        0x5,    0x3, },
    { BRT_WIDTH_MAX_I32,        0,  offsetof(br_output_facility_sdl, width),        0x5,    0x3, },
    { BRT_HEIGHT_I32,           0,  offsetof(br_output_facility_sdl, height),       0x5,    0x3, },
    { BRT_HEIGHT_MIN_I32,       0,  offsetof(br_output_facility_sdl, height),       0x5,    0x3, },
    { BRT_HEIGHT_MAX_I32,       0,  offsetof(br_output_facility_sdl, height),       0x5,    0x3, },
    { BRT_PIXEL_TYPE_U8,        0,  offsetof(br_output_facility_sdl, colour_type),  0x5,    0x3, },
    { BRT_PIXEL_BITS_I32,       0,  offsetof(br_output_facility_sdl, colour_bits),  0x5,    0x3, },
    { BRT_INDEXED_B,            0,  offsetof(br_output_facility_sdl, indexed),      0x5,    0x3, },
    { BRT_WINDOW_FULLSCREEN_B,  0,  offsetof(br_output_facility_sdl, fullscreen),   0x5,    0x3, },
};

static br_tv_template outputFacilitySDLTemplate = {
    BR_ASIZE(outputFacilitySDLTemplateEntries),
    outputFacilitySDLTemplateEntries,
};

static void C2_HOOK_CDECL _M_br_output_facility_sdl_free(br_output_facility_sdl* self) {
    br_device* dev = self->dispatch->_device((br_object*)self);
    dev->dispatch->_remove((br_object_container*)dev, (br_object*)self);
    BrObjectContainerFree((br_object_container*)self, BR_NULL_TOKEN, NULL, NULL);
    BrResFreeNoCallback(self);
}

static char* C2_HOOK_CDECL _M_br_output_facility_sdl_identifier(br_output_facility_sdl* self) {
    return self->identifier;
}

static br_token C2_HOOK_CDECL _M_br_output_facility_sdl_type(br_output_facility_sdl* self) {
    (void)self;
    return BRT_OUTPUT_FACILITY;
}

static br_boolean C2_HOOK_CDECL _M_br_output_facility_sdl_isType(br_output_facility_sdl* self, br_token t) {
    (void)self;
    return t == BRT_OUTPUT_FACILITY || t == BRT_OBJECT_CONTAINER || t == BRT_OBJECT;
}

static br_device* C2_HOOK_CDECL _M_br_output_facility_sdl_device(br_output_facility_sdl* self) {
    return self->device;
}

static br_int_32 C2_HOOK_CDECL _M_br_output_facility_sdl_space(br_output_facility_sdl* self) {
    (void)self;
    return sizeof(br_output_facility_sdl);
}

static struct br_tv_template* C2_HOOK_CDECL _M_br_output_facility_sdl_queryTemplate(br_output_facility_sdl* self) {
    (void)self;
    return &outputFacilitySDLTemplate;
}

static br_error C2_HOOK_CDECL _M_br_output_facility_sdl_validSource(br_output_facility_sdl* self, br_boolean* bp, br_object* h) {
    (void)self;
    (void)h;
    if (bp != NULL) {
        *bp = 0;
    }
    return 0;
}

static br_error C2_HOOK_CDECL _M_br_output_facility_sdl_pixelmapNew(br_output_facility_sdl* self, br_device_pixelmap** ppmap, br_token_value* tv) {
    br_device_pixelmap* pm;

    (void)tv;
    pm = DevicePixelmapMemAllocate(self->colour_type, self->width, self->height, NULL, 0);
    if (pm == NULL) {
        return 0x1002;
    }
    /* The mem pixelmap has no identifier; the game reads gScreen->identifier. */
    if (pm->pm_identifier == NULL) {
        pm->pm_identifier = "SDLREND screen";
    }
    *ppmap = pm;
    self->num_instances += 1;
    return 0;
}

static br_error C2_HOOK_CDECL _M_br_output_facility_sdl_clutNew(br_output_facility_sdl* self, br_device_clut** pclut, br_token_value* tv) {
    (void)self;
    (void)pclut;
    (void)tv;
    return 0x1002;
}

static void* C2_HOOK_CDECL _M_br_output_facility_sdl_listQuery(br_output_facility_sdl* self) {
    return self->object_list;
}

static br_output_facility_dispatch outputFacilitySDLDispatch = {
    NULL,
    NULL,
    NULL,
    NULL,
    (void*)_M_br_output_facility_sdl_free,
    (void*)_M_br_output_facility_sdl_identifier,
    (void*)_M_br_output_facility_sdl_type,
    (void*)_M_br_output_facility_sdl_isType,
    (void*)_M_br_output_facility_sdl_device,
    (void*)_M_br_output_facility_sdl_space,

    (void*)_M_br_output_facility_sdl_queryTemplate,
    (void*)_M_br_object_query,
    (void*)_M_br_object_queryBuffer,
    (void*)_M_br_object_queryMany,
    (void*)_M_br_object_queryManySize,
    (void*)_M_br_object_queryAll,
    (void*)_M_br_object_queryAllSize,

    (void*)_M_br_output_facility_sdl_listQuery,
    _M_br_object_container_tokensMatchBegin,
    _M_br_object_container_tokensMatch,
    _M_br_object_container_tokensMatchEnd,
    _M_br_object_container_addFront,
    _M_br_object_container_removeFront,
    _M_br_object_container_remove,
    _M_br_object_container_find,
    _M_br_object_container_findMany,
    _M_br_object_container_count,

    (void*)_M_br_output_facility_sdl_validSource,
    (void*)_M_br_output_facility_sdl_pixelmapNew,
    (void*)_M_br_output_facility_sdl_clutNew,
};

static br_output_facility_sdl* OutputFacilitySDLAllocate(br_device* dev) {
    br_output_facility_sdl* self;

    self = BrResAllocate(dev, sizeof(*self), BR_MEMORY_OBJECT);

    self->dispatch = &outputFacilitySDLDispatch;
    self->identifier = BrResStrDup(self, "SDLREND-640x480x565");
    self->device = dev;
    self->object_list = BrObjectListAllocate(dev);
    self->num_instances = 0;

    self->width = 640;
    self->height = 480;
    self->colour_bits = 16;
    self->colour_type = BR_PMT_RGB_565;
    self->indexed = 0;
    self->fullscreen = 1;

    dev->dispatch->_addFront((br_object_container*)dev, (br_object*)self);

    return self;
}

/*
 * Driver entry point, registered with BrDevAddStatic().
 */
static br_device* C2_HOOK_CDECL BrDrv1SDLBegin(const char* arguments) {
    br_device* dev;
    void* prim_lib = NULL;

    (void)arguments;
    dev = DeviceSoftAllocate("SDLREND");
    if (dev == NULL) {
        return NULL;
    }

    OutputFacilitySDLAllocate(dev);

    if (PrimitiveLibrarySDLAllocate(dev, &prim_lib) != 0) {
        dr_dprintf("PrimitiveLibrarySDLAllocate failed");
    }

    return dev;
}

void carpocalypse2_RegisterSDLRenderer(void) {
    static int registered;

    if (!registered) {
        BrDevAddStatic(NULL, BrDrv1SDLBegin, NULL);
        registered = 1;
    }
}
