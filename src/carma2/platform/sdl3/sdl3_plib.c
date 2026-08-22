/*
 * Stub BRender primitive library for the SDL3 platform.
 *
 * Retail CARMA2_HW.EXE obtains its PRIMITIVE_LIBRARY from its hardware
 * rendering backends (3dfx/Direct3D). This module registers a
 * functional-enough stub ("Default-Primitives-Float") so that softrend's
 * renderer setup succeeds and boot can proceed; it renders nothing. Real
 * rasterization will come from a GPU backend later.
 */

#include <brender/brender.h>
#include "core/fw/devlist.h"
#include "core/fw/object.h"
#include "core/fw/objectc.h"
#include "core/fw/resource.h"
#include "core/fw/tokenval.h"

#include "c2_string.h"

#define SDL_PRIM_UNSUPPORTED 0x1002

typedef struct br_primitive_library_sdl {
    br_primitive_library_dispatch* dispatch;

    char* identifier;
    br_device* device;
    void* object_list;
} br_primitive_library_sdl;

typedef struct br_primitive_state_sdl {
    br_primitive_state_dispatch* dispatch;

    char* identifier;
    br_device* device;
} br_primitive_state_sdl;

/* ============================ primitive state ============================ */

static void C2_HOOK_CDECL state_free(br_primitive_state_sdl* self) {
    BrResFreeNoCallback(self);
}

static char* C2_HOOK_CDECL state_identifier(br_primitive_state_sdl* self) {
    (void)self;
    return "SDL Primitive State";
}

static br_token C2_HOOK_CDECL state_type(br_primitive_state_sdl* self) {
    (void)self;
    return BRT_PRIMITIVE_STATE;
}

static br_boolean C2_HOOK_CDECL state_isType(br_primitive_state_sdl* self, br_token t) {
    (void)self;
    return t == BRT_PRIMITIVE_STATE || t == BRT_OBJECT;
}

static br_device* C2_HOOK_CDECL state_device(br_primitive_state_sdl* self) {
    return self->device;
}

static br_int_32 C2_HOOK_CDECL state_space(br_primitive_state_sdl* self) {
    (void)self;
    return sizeof(br_primitive_state_sdl);
}

static br_tv_template_entry stateNullTemplateEntries[1];

static br_tv_template stateNullTemplate = {
    BR_ASIZE(stateNullTemplateEntries),
    stateNullTemplateEntries,
};

static br_tv_template* C2_HOOK_CDECL state_templateQuery(br_primitive_state_sdl* self) {
    (void)self;
    return &stateNullTemplate;
}

static br_error C2_HOOK_CDECL state_query(br_primitive_state_sdl* self, br_uint_32* pvalue, br_token t) {
    (void)self; (void)t;
    if (pvalue != NULL) {
        *pvalue = 0;
    }
    return 0;
}

static br_error C2_HOOK_CDECL state_queryBuffer(br_primitive_state_sdl* self, br_uint_32* pvalue, void* extra, br_size_t extra_size, br_token t) {
    (void)self; (void)pvalue; (void)extra; (void)extra_size; (void)t;
    return 0;
}

static br_error C2_HOOK_CDECL state_queryMany(br_primitive_state_sdl* self, br_token_value* tv, void* extra, br_size_t extra_size, br_int_32* pcount) {
    (void)self; (void)tv; (void)extra; (void)extra_size;
    if (pcount != NULL) {
        *pcount = 0;
    }
    return 0;
}

static br_error C2_HOOK_CDECL state_queryManySize(br_primitive_state_sdl* self, br_size_t* psize, br_token_value* tv) {
    (void)self; (void)tv;
    if (psize != NULL) {
        *psize = 0;
    }
    return 0;
}

static br_error C2_HOOK_CDECL state_queryAll(br_primitive_state_sdl* self, br_token_value* buffer, br_size_t buffer_size) {
    (void)self; (void)buffer; (void)buffer_size;
    return 0;
}

static br_error C2_HOOK_CDECL state_queryAllSize(br_primitive_state_sdl* self, br_size_t* psize) {
    (void)self;
    if (psize != NULL) {
        *psize = 0;
    }
    return 0;
}

static br_error C2_HOOK_CDECL state_partSet(br_primitive_state_sdl* self, br_token part, br_int_32 index, br_token t, uintptr_t value) {
    (void)self; (void)part; (void)index; (void)t; (void)value;
    return 0;
}

static br_error C2_HOOK_CDECL state_partSetMany(br_primitive_state_sdl* self, br_token part, br_int_32 index, br_token_value* tv, br_int_32* pcount) {
    (void)self; (void)part; (void)index; (void)tv;
    if (pcount != NULL) {
        *pcount = 0;
    }
    return 0;
}

static br_error C2_HOOK_CDECL state_partQuery(br_primitive_state_sdl* self, br_token part, br_int_32 index, br_uint_32* pvalue, br_token t) {
    (void)self; (void)part; (void)index; (void)t;
    if (pvalue != NULL) {
        *pvalue = 0;
    }
    return 0;
}

static br_error C2_HOOK_CDECL state_partQueryBuffer(br_primitive_state_sdl* self, br_token part, br_int_32 index, br_uint_32* pvalue, br_uint_32* extra, br_size_t extra_size, br_token t) {
    (void)self; (void)part; (void)index; (void)pvalue; (void)extra; (void)extra_size; (void)t;
    return 0;
}

static br_error C2_HOOK_CDECL state_partQueryMany(br_primitive_state_sdl* self, br_token part, br_int_32 index, br_token_value* tv, void* extra, br_size_t extra_size, br_int_32* pcount) {
    (void)self; (void)part; (void)index; (void)tv; (void)extra; (void)extra_size;
    if (pcount != NULL) {
        *pcount = 0;
    }
    return 0;
}

static br_error C2_HOOK_CDECL state_partQueryManySize(br_primitive_state_sdl* self, br_token part, br_int_32 index, br_size_t* psize, br_token_value* tv) {
    (void)self; (void)part; (void)index; (void)tv;
    if (psize != NULL) {
        *psize = 0;
    }
    return 0;
}

static br_error C2_HOOK_CDECL state_partQueryAll(br_primitive_state_sdl* self, br_token part, br_int_32 index, br_token_value* buffer, br_size_t buffer_size) {
    (void)self; (void)part; (void)index; (void)buffer; (void)buffer_size;
    return 0;
}

static br_error C2_HOOK_CDECL state_partQueryAllSize(br_primitive_state_sdl* self, br_token part, br_int_32 index, br_size_t* psize) {
    (void)self; (void)part; (void)index;
    if (psize != NULL) {
        *psize = 0;
    }
    return 0;
}

static br_error C2_HOOK_CDECL state_partIndexQuery(br_primitive_state_sdl* self, br_token part, br_int_32* pindex) {
    (void)self; (void)part;
    if (pindex != NULL) {
        *pindex = -1;
    }
    return 0;
}

static br_error C2_HOOK_CDECL state_stateDefault(br_primitive_state_sdl* self, br_uint_32 mask) {
    (void)self; (void)mask;
    return 0;
}

static br_error C2_HOOK_CDECL state_stateCopy(br_primitive_state_sdl* self, br_primitive_state* source, br_uint_32 mask) {
    (void)self; (void)source; (void)mask;
    return 0;
}

static br_error C2_HOOK_CDECL state_renderBegin(br_primitive_state_sdl* self, brp_block** blocks, br_boolean* block_changed, br_boolean* ranges_changed, br_boolean use_custom, br_int_32 subdivide_tolerance) {
    (void)self; (void)blocks; (void)block_changed; (void)ranges_changed; (void)use_custom; (void)subdivide_tolerance;
    return 0;
}

static br_error C2_HOOK_CDECL state_renderEnd(br_primitive_state_sdl* self, brp_block* block) {
    (void)self; (void)block;
    return 0;
}

static br_error C2_HOOK_CDECL state_rangesQueryF(br_primitive_state_sdl* self, br_float* dest, br_float* src, br_int_32 size) {
    (void)self; (void)dest; (void)src; (void)size;
    return SDL_PRIM_UNSUPPORTED;
}

static br_error C2_HOOK_CDECL state_rangesQueryX(br_primitive_state_sdl* self, br_fixed_ls* dest, br_fixed_ls* src, br_int_32 size) {
    (void)self; (void)dest; (void)src; (void)size;
    return SDL_PRIM_UNSUPPORTED;
}

static br_error C2_HOOK_CDECL state_partQueryCapability(br_primitive_state_sdl* self, br_token part, br_int_32 index, br_token_value* tv, br_size_t size) {
    (void)self; (void)part; (void)index; (void)tv; (void)size;
    return SDL_PRIM_UNSUPPORTED;
}

static br_error C2_HOOK_CDECL state_stateQueryPerformance(br_primitive_state_sdl* self, br_fixed_lu* perf) {
    (void)self; (void)perf;
    return SDL_PRIM_UNSUPPORTED;
}

static br_primitive_state_dispatch stateSDLDispatch = {
    NULL,
    NULL,
    NULL,
    NULL,
    (void*)state_free,
    (void*)state_identifier,
    (void*)state_type,
    (void*)state_isType,
    (void*)state_device,
    (void*)state_space,
    (void*)state_templateQuery,
    (void*)state_query,
    (void*)state_queryBuffer,
    (void*)state_queryMany,
    (void*)state_queryManySize,
    (void*)state_queryAll,
    (void*)state_queryAllSize,
    (void*)state_partSet,
    (void*)state_partSetMany,
    (void*)state_partQuery,
    (void*)state_partQueryBuffer,
    (void*)state_partQueryMany,
    (void*)state_partQueryManySize,
    (void*)state_partQueryAll,
    (void*)state_partQueryAllSize,
    (void*)state_partIndexQuery,
    (void*)state_stateDefault,
    (void*)state_stateCopy,
    (void*)state_renderBegin,
    (void*)state_renderEnd,
    (void*)state_rangesQueryF,
    (void*)state_rangesQueryX,
    (void*)state_partQueryCapability,
    (void*)state_stateQueryPerformance,
};

/* =========================== primitive library =========================== */

static void C2_HOOK_CDECL lib_free(br_primitive_library_sdl* self) {
    BrObjectContainerFree((br_object_container*)self, BR_NULL_TOKEN, NULL, NULL);
    BrResFreeNoCallback(self);
}

static char* C2_HOOK_CDECL lib_identifier(br_primitive_library_sdl* self) {
    return self->identifier;
}

static br_token C2_HOOK_CDECL lib_type(br_primitive_library_sdl* self) {
    (void)self;
    return BRT_PRIMITIVE_LIBRARY;
}

static br_boolean C2_HOOK_CDECL lib_isType(br_primitive_library_sdl* self, br_token t) {
    (void)self;
    return t == BRT_PRIMITIVE_LIBRARY || t == BRT_OBJECT_CONTAINER || t == BRT_OBJECT;
}

static br_device* C2_HOOK_CDECL lib_device(br_primitive_library_sdl* self) {
    return self->device;
}

static br_int_32 C2_HOOK_CDECL lib_space(br_primitive_library_sdl* self) {
    (void)self;
    return sizeof(br_primitive_library_sdl);
}

static br_tv_template_entry libNullTemplateEntries[1];

static br_tv_template libNullTemplate = {
    BR_ASIZE(libNullTemplateEntries),
    libNullTemplateEntries,
};

static br_tv_template* C2_HOOK_CDECL lib_templateQuery(br_primitive_library_sdl* self) {
    (void)self;
    return &libNullTemplate;
}

/*
 * softrend reads this right after allocating a renderer:
 *   self->plib->dispatch->_query(..., &m, BRT_PARTS_U32);
 * 'm' feeds state.valid, so it must be written even when unsupported.
 */
static br_error C2_HOOK_CDECL lib_query(br_primitive_library_sdl* self, br_uint_32* pvalue, br_token t) {
    (void)self;
    if (pvalue != NULL) {
        *pvalue = 0;
    }
    (void)t;
    return 0;
}

static br_error C2_HOOK_CDECL lib_queryBuffer(br_primitive_library_sdl* self, br_uint_32* pvalue, void* extra, br_size_t extra_size, br_token t) {
    (void)self; (void)pvalue; (void)extra; (void)extra_size; (void)t;
    return 0;
}

static br_error C2_HOOK_CDECL lib_queryMany(br_primitive_library_sdl* self, br_token_value* tv, void* extra, br_size_t extra_size, br_int_32* pcount) {
    (void)self; (void)tv; (void)extra; (void)extra_size;
    if (pcount != NULL) {
        *pcount = 0;
    }
    return 0;
}

static br_error C2_HOOK_CDECL lib_queryManySize(br_primitive_library_sdl* self, br_size_t* psize, br_token_value* tv) {
    (void)self; (void)tv;
    if (psize != NULL) {
        *psize = 0;
    }
    return 0;
}

static br_error C2_HOOK_CDECL lib_queryAll(br_primitive_library_sdl* self, br_token_value* buffer, br_size_t buffer_size) {
    (void)self; (void)buffer; (void)buffer_size;
    return 0;
}

static br_error C2_HOOK_CDECL lib_queryAllSize(br_primitive_library_sdl* self, br_size_t* psize) {
    (void)self;
    if (psize != NULL) {
        *psize = 0;
    }
    return 0;
}

static void* C2_HOOK_CDECL lib_listQuery(br_primitive_library_sdl* self) {
    return self->object_list;
}

static br_error C2_HOOK_CDECL lib_stateNew(br_primitive_library_sdl* self, br_primitive_state** pstate) {
    br_primitive_state_sdl* s;

    s = BrResAllocate(self, sizeof(*s), BR_MEMORY_OBJECT);
    if (s == NULL) {
        return SDL_PRIM_UNSUPPORTED;
    }
    s->dispatch = &stateSDLDispatch;
    s->identifier = "SDL Primitive State";
    s->device = self->device;

    *pstate = (br_primitive_state*)s;
    return 0;
}

static br_error C2_HOOK_CDECL lib_bufferStoredNew(br_primitive_library_sdl* self, br_buffer_stored** psm, br_token use, br_device_pixelmap* pm, br_token_value* tv) {
    (void)self; (void)psm; (void)use; (void)pm; (void)tv;
    /* Texture uploads are not implemented yet. */
    return SDL_PRIM_UNSUPPORTED;
}

static br_error C2_HOOK_CDECL lib_bufferStoredAvail(br_primitive_library_sdl* self, br_int_32* pspace, br_token use, br_token_value* tv) {
    (void)self; (void)use; (void)tv;
    if (pspace != NULL) {
        *pspace = 0;
    }
    return 0;
}

static br_error C2_HOOK_CDECL lib_flush(br_primitive_library_sdl* self, br_boolean wait) {
    (void)self; (void)wait;
    return 0;
}

static br_error C2_HOOK_CDECL lib_synchronise(br_primitive_library_sdl* self, br_token sync_type, br_boolean wait) {
    (void)self; (void)sync_type; (void)wait;
    return 0;
}

static br_error C2_HOOK_CDECL lib_mask(br_primitive_library_sdl* self, br_uint_32* pmask, br_token* pt, int t_count) {
    (void)self; (void)pt; (void)t_count;
    if (pmask != NULL) {
        *pmask = 0;
    }
    return 0;
}

static br_primitive_library_dispatch libSDLDispatch = {
    NULL,
    NULL,
    NULL,
    NULL,
    (void*)lib_free,
    (void*)lib_identifier,
    (void*)lib_type,
    (void*)lib_isType,
    (void*)lib_device,
    (void*)lib_space,
    (void*)lib_templateQuery,
    (void*)lib_query,
    (void*)lib_queryBuffer,
    (void*)lib_queryMany,
    (void*)lib_queryManySize,
    (void*)lib_queryAll,
    (void*)lib_queryAllSize,
    (void*)lib_listQuery,
    (void*)_M_br_object_container_tokensMatchBegin,
    (void*)_M_br_object_container_tokensMatch,
    (void*)_M_br_object_container_tokensMatchEnd,
    (void*)_M_br_object_container_addFront,
    (void*)_M_br_object_container_removeFront,
    (void*)_M_br_object_container_remove,
    (void*)_M_br_object_container_find,
    (void*)_M_br_object_container_findMany,
    (void*)_M_br_object_container_count,
    (void*)lib_stateNew,
    (void*)lib_bufferStoredNew,
    (void*)lib_bufferStoredAvail,
    (void*)lib_flush,
    (void*)lib_synchronise,
    (void*)lib_mask,
};

br_error PrimitiveLibrarySDLAllocate(br_device* dev, br_primitive_library_sdl** ppl) {
    br_primitive_library_sdl* self;

    self = BrResAllocate(dev, sizeof(*self), BR_MEMORY_OBJECT);
    if (self == NULL) {
        return SDL_PRIM_UNSUPPORTED;
    }

    self->dispatch = &libSDLDispatch;
    self->identifier = BrResStrDup(self, "Default-Primitives-Float");
    self->device = dev;
    self->object_list = BrObjectListAllocate(dev);

    dev->dispatch->_addFront((br_object_container*)dev, (br_object*)self);

    *ppl = self;
    return 0;
}
