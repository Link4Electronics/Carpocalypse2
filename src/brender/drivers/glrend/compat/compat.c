/* glue between the BRender 1.4-style glrend driver and our core (port only) */
#include "drv.h"
#include "core/math/matrix4.h"
#include "core/v1db/dbsetup.h"

#include <math.h>
#include <string.h>

br_error C2_HOOK_CDECL BrCompatDevicePixelmapFail(void) {
    return BRE_FAIL;
}

void C2_HOOK_CDECL BrMatrix4Copy23(br_matrix4* A, const br_matrix23* B) {
    BrMatrix4Identity(A);
    A->m[0][0] = B->m[0][0]; A->m[0][1] = B->m[0][1];
    A->m[1][0] = B->m[1][0]; A->m[1][1] = B->m[1][1];
    A->m[2][0] = B->m[2][0]; A->m[2][1] = B->m[2][1];
}

void C2_HOOK_CDECL BrMatrix4Transpose(br_matrix4* A) {
    int i, j;
    for (i = 0; i < 4; ++i) {
        for (j = i + 1; j < 4; ++j) {
            br_scalar tmp = A->m[i][j];
            A->m[i][j] = A->m[j][i];
            A->m[j][i] = tmp;
        }
    }
}

void C2_HOOK_CDECL BrVector4Normalise(br_vector4* v1, const br_vector4* v2) {
    br_scalar l = sqrtf(v2->v[0] * v2->v[0] + v2->v[1] * v2->v[1] + v2->v[2] * v2->v[2] + v2->v[3] * v2->v[3]);
    if (l > 1e-12f) {
        BrVector4Scale(v1, v2, 1.0f / l);
    } else {
        BrVector4Set(v1, 1.0f, 0.0f, 0.0f, 0.0f);
    }
}

br_boolean C2_HOOK_CDECL BrTokenValueComparePartial(br_token_value* tv1, br_token_value* tv2, const br_token* insignificant) {
    const br_token* t;
    int i;

    if (tv1 == NULL && tv2 == NULL) {
        return BR_TRUE;
    }
    if (tv1 == NULL || tv2 == NULL) {
        return BR_FALSE;
    }
    for (;; tv1++, tv2++) {
        if (tv1->t != tv2->t) {
            return BR_FALSE;
        }
        if (tv1->t == BR_NULL_TOKEN) {
            return BR_TRUE;
        }
        for (t = insignificant; *t != BR_NULL_TOKEN; t++) {
            if (tv1->t == *t) {
                break;
            }
        }
        if (*t != BR_NULL_TOKEN) {
            continue;
        }
        switch (BrTokenType(tv1->t)) {
        case BRT_NONE:
            break;
        case BRT_STRING:
        case BRT_CONSTANT_STRING:
            if ((tv1->v.str == NULL) != (tv2->v.str == NULL) || (tv1->v.str != NULL && strcmp(tv1->v.str, tv2->v.str) != 0)) {
                return BR_FALSE;
            }
            break;
        case BRT_OBJECT_LIST:
        case BRT_TOKEN_LIST:
        case BRT_POINTER_LIST:
            for (i = 0;; i++) {
                if (tv1->v.pl[i] != tv2->v.pl[i]) {
                    return BR_FALSE;
                }
                if (tv1->v.pl[i] == NULL) {
                    break;
                }
            }
            break;
        default:
            if (tv1->v.u32 != tv2->v.u32) {
                return BR_FALSE;
            }
            break;
        }
    }
}

/*
 * Fixed size block pools: the 1.2 core has no pool.c, so blocks come straight from the
 * BRender allocator (the driver only uses pools for transient render state copies).
 */
br_pool* C2_HOOK_CDECL BrPoolAllocate(int block_size, int chunk_size, br_uint_8 mem_type) {
    br_pool* pool = BrMemAllocate(sizeof(br_pool), mem_type);
    pool->free = NULL;
    pool->block_size = (block_size + 7) & ~7;
    pool->chunk_size = chunk_size;
    pool->mem_type = mem_type;
    return pool;
}

void* C2_HOOK_CDECL BrPoolBlockAllocate(br_pool* pool) {
    br_pool_block* b = pool->free;
    if (b != NULL) {
        pool->free = b->next;
        return b;
    }
    return BrMemAllocate(pool->block_size, (br_uint_8)pool->mem_type);
}

void C2_HOOK_CDECL BrPoolBlockFree(br_pool* pool, void* block) {
    br_pool_block* b = block;
    b->next = pool->free;
    pool->free = b;
}

void C2_HOOK_CDECL BrPoolFree(br_pool* pool) {
    br_pool_block* b;
    while ((b = pool->free) != NULL) {
        pool->free = b->next;
        BrMemFree(b);
    }
    BrMemFree(pool);
}

void C2_HOOK_CDECL BrRendererFrameBegin(void) {
    if (v1db.renderer != NULL && v1db.renderer->dispatch->_frameBegin != NULL) {
        v1db.renderer->dispatch->_frameBegin(v1db.renderer);
    }
}

/*
 * Null BRT_GEOMETRY_LIGHTING format: BrV1dbRendererBegin requires one (1.4 takes it from
 * nulldev). The GL shaders do the lighting, so it renders nothing.
 */
typedef struct br_geometry_lighting_null {
    const br_geometry_lighting_dispatch* dispatch;
    const char* identifier;
    br_device* device;
    br_renderer_facility* renderer_facility;
} br_geometry_lighting_null;

static void C2_HOOK_CDECL lighting_null_free(br_object* self) {
    br_geometry_lighting_null* l = (br_geometry_lighting_null*)self;
    ObjectContainerRemove(l->renderer_facility, self);
    BrResFreeNoCallback(self);
}
static char* C2_HOOK_CDECL lighting_null_identifier(br_object* self) {
    return (char*)((br_geometry_lighting_null*)self)->identifier;
}
static br_token C2_HOOK_CDECL lighting_null_type(br_object* self) {
    return BRT_GEOMETRY_LIGHTING;
}
static br_boolean C2_HOOK_CDECL lighting_null_isType(br_object* self, br_token t) {
    return t == BRT_GEOMETRY_LIGHTING || t == BRT_GEOMETRY || t == BRT_OBJECT;
}
static br_device* C2_HOOK_CDECL lighting_null_device(br_object* self) {
    return ((br_geometry_lighting_null*)self)->device;
}
static br_int_32 C2_HOOK_CDECL lighting_null_space(br_object* self) {
    return sizeof(br_geometry_lighting_null);
}
static br_tv_template_entry lightingNullTemplateEntries[] = {
    { BRT_IDENTIFIER_CSTR, NULL, offsetof(br_geometry_lighting_null, identifier), BRTV_QUERY | BRTV_ALL, BRTV_CONV_COPY, 0, 0 },
    { BRT_RENDERER_FACILITY_O, NULL, offsetof(br_geometry_lighting_null, renderer_facility), BRTV_QUERY | BRTV_ALL, BRTV_CONV_COPY, 0, 0 },
};
static br_tv_template* lightingNullTemplate;
static br_tv_template* C2_HOOK_CDECL lighting_null_templateQuery(br_object* self) {
    if (lightingNullTemplate == NULL) {
        lightingNullTemplate = BrTVTemplateAllocate(self, lightingNullTemplateEntries, BR_ASIZE(lightingNullTemplateEntries));
    }
    return lightingNullTemplate;
}
static br_error C2_HOOK_CDECL lighting_null_render(void) {
    return BRE_OK;
}

static const br_geometry_lighting_dispatch lightingNullDispatch = {
    NULL, NULL, NULL, NULL,
    lighting_null_free,
    lighting_null_identifier,
    lighting_null_type,
    lighting_null_isType,
    lighting_null_device,
    lighting_null_space,
    lighting_null_templateQuery,
    (void*)_M_br_object_query,
    (void*)_M_br_object_queryBuffer,
    (void*)_M_br_object_queryMany,
    (void*)_M_br_object_queryManySize,
    (void*)_M_br_object_queryAll,
    (void*)_M_br_object_queryAllSize,
    (void*)lighting_null_render,
    (void*)lighting_null_render,
};

br_geometry_lighting* GeometryLightingNullAllocate(br_renderer_facility* type, const char* id) {
    br_geometry_lighting_null* self = BrResAllocate(type, sizeof(*self), BR_MEMORY_OBJECT);
    if (self == NULL) {
        return NULL;
    }
    self->dispatch = &lightingNullDispatch;
    self->identifier = id;
    self->device = type->device;
    self->renderer_facility = type;
    ObjectContainerAddFront(type, (br_object*)self);
    return (br_geometry_lighting*)self;
}
