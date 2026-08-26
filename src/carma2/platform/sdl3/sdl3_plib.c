/*
 * Real BRender primitive library for the SDL3 platform ("Default-Primitives-Float").
 *
 * softrend (the same software renderer the retail DOS build used) transforms,
 * clips and lights geometry, then hands screen-space triangles to this library
 * through br_primitive_state_renderBegin/renderEnd. This module rasterizes
 * them into the SDLREND memory pixelmaps:
 *
 *   - textured triangles (RGBA_4444 / RGB_565 colour maps, nearest sampling,
 *     wrapping repeats)
 *   - per-pixel 16-bit depth buffering (test + write)
 *   - magenta chroma key plus texel-alpha transparency
 *   - constant/vertex colour modulation and opacity blending
 *
 * Interface patterns follow drivers/d3d/{pstate,match,plib,sbuffer}.c, which
 * target the same core in the retail hardware path.
 */

#include <brender/brender.h>
#include "core/fw/devlist.h"
#include "core/fw/object.h"
#include "core/fw/objectc.h"
#include "core/fw/resource.h"
#include "core/fw/tokenval.h"

#include "c2_string.h"
#include <math.h>
#include <stdio.h>
#include <string.h>


#define SDL_PRIM_UNSUPPORTED 0x1002

/* Driver-private primitive flags (values follow drivers/d3d/types.h) */
#define PRIMF_SMOOTH            0x001
#define PRIMF_PERSPECTIVE       0x002
#define PRIMF_DECAL             0x004
#define PRIMF_DEPTH_WRITE       0x010
#define PRIMF_COLOUR_WRITE      0x020
#define PRIMF_BLEND             0x080
#define PRIMF_MODULATE          0x200
#define PRIMF_TEXTURE_BUFFER    0x400
#define PRIMF_DEPTH_BUFFER      0x800

/* Public block flags (drivers/softrend/types.h) */
#define BR_PRIMF_BLENDED        0x00000008

static br_uint_32 sdlplib_timestamp;

#define SDLPLIB_TIMESTAMP() (sdlplib_timestamp += 2)

/* Component masks (1 << brp_components) */
#define CM_SX   (1u << 5)
#define CM_SY   (1u << 6)
#define CM_SZ   (1u << 7)
#define CM_U    (1u << 8)
#define CM_V    (1u << 9)
#define CM_A    (1u << 10)
#define CM_R    (1u << 11)
#define CM_G    (1u << 12)
#define CM_B    (1u << 13)

typedef struct br_buffer_stored_sdl {
    br_buffer_stored_dispatch* dispatch;

    char* identifier;
    br_device* device;
    void* plib;

    br_device_pixelmap* pm;
    int blended;   /* format carries alpha */
} br_buffer_stored_sdl;

typedef struct br_primitive_state_sdl {
    br_primitive_state_dispatch* dispatch;

    char* identifier;
    br_device* device;
    void* plib;

    struct {
        br_uint_32 flags;
        br_buffer_stored* colour_map;
        br_token colour_type;
        br_float alpha_val;
        br_token map_interpolation;
        br_uint_32 timestamp;
        br_uint_32 timestamp_major;
    } prim;

    struct {
        br_device_pixelmap* pixelmap;
        br_device_pixelmap* depth;
        br_uint_32 timestamp;
        br_uint_32 timestamp_major;
    } out;

    struct {
        void* last_block;
        br_token last_type;
        br_uint_32 timestamp_prim;
        br_uint_32 timestamp_out;
    } cache;
} br_primitive_state_sdl;

typedef struct br_primitive_library_sdl {
    br_primitive_library_dispatch* dispatch;

    char* identifier;
    br_device* device;
    void* object_list;
} br_primitive_library_sdl;

/* ============================ timestamps ============================ */

static br_uint_32 C2_HOOK_CDECL Timestamp(void) {
    return SDLPLIB_TIMESTAMP();
}

/* ========================= buffer stored object ========================= */

static void C2_HOOK_CDECL buffer_free(br_buffer_stored_sdl* self) {
    ((br_primitive_library_sdl*)self->plib)->dispatch->_remove((br_object_container*)self->plib, (br_object*)self);
    BrResFreeNoCallback(self);
}

static char* C2_HOOK_CDECL buffer_identifier(br_buffer_stored_sdl* self) {
    return self->identifier;
}

static br_token C2_HOOK_CDECL buffer_type(br_buffer_stored_sdl* self) {
    (void)self;
    return BRT_BUFFER_STORED;
}

static br_boolean C2_HOOK_CDECL buffer_isType(br_buffer_stored_sdl* self, br_token t) {
    (void)self;
    return t == BRT_BUFFER_STORED || t == BRT_OBJECT;
}

static br_device* C2_HOOK_CDECL buffer_device(br_buffer_stored_sdl* self) {
    return self->device;
}

static br_int_32 C2_HOOK_CDECL buffer_space(br_buffer_stored_sdl* self) {
    (void)self;
    return sizeof(br_buffer_stored_sdl);
}

static br_tv_template_entry bufferNullTemplateEntries[1];

static br_tv_template bufferNullTemplate = {
    BR_ASIZE(bufferNullTemplateEntries),
    bufferNullTemplateEntries,
};

static br_tv_template* C2_HOOK_CDECL buffer_templateQuery(br_buffer_stored_sdl* self) {
    (void)self;
    return &bufferNullTemplate;
}

static br_error C2_HOOK_CDECL buffer_query(br_buffer_stored_sdl* self, br_uint_32* pvalue, br_token t) {
    (void)self; (void)t;
    if (pvalue != NULL) {
        *pvalue = 0;
    }
    return 0;
}

static br_error C2_HOOK_CDECL buffer_queryBuffer(br_buffer_stored_sdl* self, br_uint_32* pvalue, void* extra, br_size_t extra_size, br_token t) {
    (void)self; (void)pvalue; (void)extra; (void)extra_size; (void)t;
    return 0;
}

static br_error C2_HOOK_CDECL buffer_queryMany(br_buffer_stored_sdl* self, br_token_value* tv, void* extra, br_size_t extra_size, br_int_32* pcount) {
    (void)self; (void)tv; (void)extra; (void)extra_size;
    if (pcount != NULL) {
        *pcount = 0;
    }
    return 0;
}

static br_error C2_HOOK_CDECL buffer_queryManySize(br_buffer_stored_sdl* self, br_size_t* psize, br_token_value* tv) {
    (void)self; (void)psize; (void)tv;
    if (psize != NULL) {
        *psize = 0;
    }
    return 0;
}

static br_error C2_HOOK_CDECL buffer_queryAll(br_buffer_stored_sdl* self, br_token_value* buffer, br_size_t buffer_size) {
    (void)self; (void)buffer; (void)buffer_size;
    return 0;
}

static br_error C2_HOOK_CDECL buffer_queryAllSize(br_buffer_stored_sdl* self, br_size_t* psize) {
    (void)self;
    if (psize != NULL) {
        *psize = 0;
    }
    return 0;
}

static br_error C2_HOOK_CDECL buffer_update(br_buffer_stored_sdl* self, br_device_pixelmap* pm, br_token_value* tv) {
    (void)tv;
    self->pm = pm;
    return 0;
}

static br_buffer_stored_dispatch bufferSDLDipatch = {
    NULL,
    NULL,
    NULL,
    NULL,
    (void*)buffer_free,
    (void*)buffer_identifier,
    (void*)buffer_type,
    (void*)buffer_isType,
    (void*)buffer_device,
    (void*)buffer_space,
    (void*)buffer_templateQuery,
    (void*)buffer_query,
    (void*)buffer_queryBuffer,
    (void*)buffer_queryMany,
    (void*)buffer_queryManySize,
    (void*)buffer_queryAll,
    (void*)buffer_queryAllSize,
    (void*)buffer_update,
};

static br_error BufferStoredSDLAllocate(br_primitive_library_sdl* plib, br_buffer_stored** psm, br_token use, br_device_pixelmap* pm, br_token_value* tv) {
    br_buffer_stored_sdl* sm;
    const char* ident;

    (void)tv;
    switch (use) {
    case BRT_TEXTURE_O:
    case BRT_COLOUR_MAP_O:
        ident = "Texture";
        break;
    default:
        return SDL_PRIM_UNSUPPORTED;
    }

    if (pm == NULL) {
        return SDL_PRIM_UNSUPPORTED;
    }

    sm = BrResAllocate(plib, sizeof(*sm), BR_MEMORY_OBJECT);
    if (sm == NULL) {
        return SDL_PRIM_UNSUPPORTED;
    }
    sm->dispatch = &bufferSDLDipatch;
    sm->identifier = BrResStrDup(sm, ident);
    sm->device = plib->device;
    sm->plib = plib;
    sm->pm = pm;
    sm->blended = (pm->pm_type == (br_uint_8)BR_PMT_RGBA_4444 || pm->pm_type == (br_uint_8)BR_PMT_ARGB_1555);

    plib->dispatch->_addFront((br_object_container*)plib, (br_object*)sm);

    *psm = (br_buffer_stored*)sm;
    return 0;
}

/* ============================ primitive state ============================ */

/* 64-bit-safe object converter: conv=3 truncates pointers to 32 bits
 * (retail x86 behaviour), so pointer entries use a custom converter. */
static br_error C2_HOOK_CDECL ptr_custom_query(br_uint_32* pvalue, void** extra, br_size_t* extra_size, void* block, br_tv_template_entry* tep) {
    (void)extra; (void)extra_size;
    *(void**)pvalue = *(void**)((char*)block + tep->offset);
    return 0;
}

static br_error C2_HOOK_CDECL ptr_custom_set(void* block, br_uint_32* pvalue, br_tv_template_entry* tep) {
    *(void**)((char*)block + tep->offset) = *(void**)pvalue;
    *pvalue = 1;
    return 0;
}

static br_tv_custom ptr_custom = { ptr_custom_query, ptr_custom_set, NULL };

static br_tv_template_entry primitiveSdlTemplateEntries[] = {
    { BRT_SMOOTH_B,             0, offsetof(br_primitive_state_sdl, prim.flags),              0x7, 0x1f, PRIMF_SMOOTH,       1, },
    { BRT_PERSPECTIVE_B,        0, offsetof(br_primitive_state_sdl, prim.flags),              0x7, 0x1f, PRIMF_PERSPECTIVE,  1, },
    { BRT_DECAL_B,              0, offsetof(br_primitive_state_sdl, prim.flags),              0x7, 0x1f, PRIMF_DECAL,        1, },
    { BRT_DEPTH_WRITE_B,        0, offsetof(br_primitive_state_sdl, prim.flags),              0x7, 0x1f, PRIMF_DEPTH_WRITE,  1, },
    { BRT_COLOUR_WRITE_B,       0, offsetof(br_primitive_state_sdl, prim.flags),              0x7, 0x1f, PRIMF_COLOUR_WRITE, 1, },
    { BRT_BLEND_B,              0, offsetof(br_primitive_state_sdl, prim.flags),              0x7, 0x1f, PRIMF_BLEND,        1, },
    { BRT_MODULATE_B,           0, offsetof(br_primitive_state_sdl, prim.flags),              0x7, 0x1f, PRIMF_MODULATE,     1, },
    { BRT_COLOUR_T,             0, offsetof(br_primitive_state_sdl, prim.colour_type),        0x7, 0x3,  0,                  1, },
    { BRT_COLOUR_B,             0, offsetof(br_primitive_state_sdl, prim.colour_type),        0x7, 0x1e, BRT_DEFAULT,        1, },
    { BRT_COLOUR_MAP_O,         0, offsetof(br_primitive_state_sdl, prim.colour_map),         0x7, 0x2, (uintptr_t)&ptr_custom, },
    { BRT_TEXTURE_O,            0, offsetof(br_primitive_state_sdl, prim.colour_map),         0x7, 0x2, (uintptr_t)&ptr_custom, },
    { BRT_OPACITY_X,            0, offsetof(br_primitive_state_sdl, prim.alpha_val),          0x7, 0xe,  },
    { BRT_OPACITY_F,            0, offsetof(br_primitive_state_sdl, prim.alpha_val),          0x7, 0x3,  },
    { BRT_MAP_INTERPOLATION_T,  0, offsetof(br_primitive_state_sdl, prim.map_interpolation),  0x7, 0x3,  },
};

static br_tv_template primitiveSdlTemplate = {
    BR_ASIZE(primitiveSdlTemplateEntries),
    primitiveSdlTemplateEntries,
};

static br_tv_template_entry outputSdlTemplateEntries[] = {
    { BRT_COLOUR_BUFFER_O, 0, offsetof(br_primitive_state_sdl, out.pixelmap), 0x7, 0x2, (uintptr_t)&ptr_custom, },
    { BRT_DEPTH_BUFFER_O,  0, offsetof(br_primitive_state_sdl, out.depth),    0x7, 0x2, (uintptr_t)&ptr_custom, },
};

static br_tv_template outputSdlTemplate = {
    BR_ASIZE(outputSdlTemplateEntries),
    outputSdlTemplateEntries,
};

static br_tv_template* C2_HOOK_CDECL state_findTemplate(br_token part) {
    switch (part) {
    case BRT_PRIMITIVE:
        return &primitiveSdlTemplate;
    case BRT_OUTPUT:
        return &outputSdlTemplate;
    default:
        break;
    }
    return NULL;
}

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

static br_tv_template* C2_HOOK_CDECL state_templateQuery(br_primitive_state_sdl* self) {
    (void)self;
    return &primitiveSdlTemplate;
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
    (void)self; (void)psize; (void)tv;
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
    br_error r;
    br_tv_template* tp = state_findTemplate(part);
    br_uint_32 m;

    if (tp == NULL) {
        return 0xa001;
    }
    m = 0;
    r = BrTokenValueSet(self, &m, t, value, tp);
    if (r != 0) {
        return r;
    }
    switch (part) {
    case BRT_PRIMITIVE:
        self->prim.timestamp = Timestamp();
        if (m != 0) {
            self->prim.timestamp_major = Timestamp();
        }
        break;
    case BRT_OUTPUT:
        self->out.timestamp = Timestamp();
        if (m != 0) {
            self->out.timestamp_major = Timestamp();
        }
        break;
    default:
        break;
    }
    return 0;
}

static br_error C2_HOOK_CDECL state_partSetMany(br_primitive_state_sdl* self, br_token part, br_int_32 index, br_token_value* tv, br_int_32* pcount) {
    br_error r;
    br_tv_template* tp = state_findTemplate(part);
    br_uint_32 m;
    br_int_32 c;

    if (tp == NULL) {
        return 0xa001;
    }
    m = 0;
    r = BrTokenValueSetMany(self, &c, &m, tv, tp);
    if (r != 0 || c == 0) {
        return r;
    }
    if (pcount != NULL) {
        *pcount = c;
    }
    switch (part) {
    case BRT_PRIMITIVE:
        self->prim.timestamp = Timestamp();
        if (m != 0) {
            self->prim.timestamp_major = Timestamp();
        }
        break;
    case BRT_OUTPUT:
        self->out.timestamp = Timestamp();
        if (m != 0) {
            self->out.timestamp_major = Timestamp();
        }
        break;
    default:
        break;
    }
    return 0;
}

static br_error C2_HOOK_CDECL state_partQuery(br_primitive_state_sdl* self, br_token part, br_int_32 index, br_uint_32* pvalue, br_token t) {
    br_tv_template* tp = state_findTemplate(part);

    (void)index;
    if (tp == NULL) {
        return 0xa001;
    }
    return BrTokenValueQuery(pvalue, NULL, 0, t, self, tp);
}

static br_error C2_HOOK_CDECL state_partQueryBuffer(br_primitive_state_sdl* self, br_token part, br_int_32 index, br_uint_32* pvalue, br_uint_32* extra, br_size_t extra_size, br_token t) {
    br_tv_template* tp = state_findTemplate(part);

    (void)index;
    if (tp == NULL) {
        return 0xa001;
    }
    return BrTokenValueQuery(pvalue, extra, extra_size, t, self, tp);
}

static br_error C2_HOOK_CDECL state_partQueryMany(br_primitive_state_sdl* self, br_token part, br_int_32 index, br_token_value* tv, void* extra, br_size_t extra_size, br_int_32* pcount) {
    br_tv_template* tp = state_findTemplate(part);

    (void)index; (void)extra; (void)extra_size;
    if (tp == NULL) {
        return 0xa001;
    }
    return BrTokenValueQueryMany(tv, extra, extra_size, pcount, self, tp);
}

static br_error C2_HOOK_CDECL state_partQueryManySize(br_primitive_state_sdl* self, br_token part, br_int_32 index, br_size_t* psize, br_token_value* tv) {
    br_tv_template* tp = state_findTemplate(part);

    (void)index;
    if (tp == NULL) {
        return 0xa001;
    }
    return BrTokenValueQueryManySize(psize, tv, self, tp);
}

static br_error C2_HOOK_CDECL state_partQueryAll(br_primitive_state_sdl* self, br_token part, br_int_32 index, br_token_value* buffer, br_size_t buffer_size) {
    br_tv_template* tp = state_findTemplate(part);

    (void)index;
    if (tp == NULL) {
        return 0xa001;
    }
    return BrTokenValueQueryAll(buffer, buffer_size, self, tp);
}

static br_error C2_HOOK_CDECL state_partQueryAllSize(br_primitive_state_sdl* self, br_token part, br_int_32 index, br_size_t* psize) {
    br_tv_template* tp = state_findTemplate(part);

    (void)index;
    if (tp == NULL) {
        return 0xa001;
    }
    return BrTokenValueQueryAllSize(psize, self, tp);
}

static br_error C2_HOOK_CDECL state_partIndexQuery(br_primitive_state_sdl* self, br_token part, br_int_32* pindex) {
    (void)self; (void)part;
    if (pindex != NULL) {
        *pindex = -1;
    }
    return 0;
}

static br_error C2_HOOK_CDECL state_stateDefault(br_primitive_state_sdl* self, br_uint_32 mask) {
    (void)mask;
    self->prim.flags = 0;
    self->prim.colour_map = NULL;
    self->prim.alpha_val = 1.f;
    self->prim.colour_type = BRT_DEFAULT;
    self->prim.map_interpolation = BRT_NONE;
    return 0;
}

static br_error C2_HOOK_CDECL state_stateCopy(br_primitive_state_sdl* self, br_primitive_state* source, br_uint_32 mask) {
    br_primitive_state_sdl* src = (br_primitive_state_sdl*)source;

    /* BR_STATE_PRIMITIVE (0x80000000): without copying the primitive part,
     * stored/restore loses colour_map, blend flags and opacity, so every
     * material renders with whatever live state happened to be set last */
    if ((mask & 0x80000000u) && src != NULL) {
        self->prim.flags = src->prim.flags;
        self->prim.colour_map = src->prim.colour_map;
        self->prim.colour_type = src->prim.colour_type;
        self->prim.alpha_val = src->prim.alpha_val;
        self->prim.map_interpolation = src->prim.map_interpolation;
    }
    return 0;
}

/* =========================== triangle rasterizer =========================== */

typedef struct local_block_sdl {
    brp_block p;
    br_primitive_state_sdl* state;
    br_uint_32 sig;
} local_block_sdl;

/*
 * Screen-space affine triangle with texture, depth, key and blend.
 * Vertices carry comp_f: C_SX, C_SY (pixels), C_SZ (depth), C_U, C_V,
 * and optionally C_R, C_G, C_B (0..255) when smooth.
 */
static void C2_HOOK_CDECL sdlTriangleRender(brp_block* block, brp_vertex* v0, brp_vertex* v1, brp_vertex* v2) {
    local_block_sdl* lb = (local_block_sdl*)block;
    br_primitive_state_sdl* state = lb->state;
    br_buffer_stored_sdl* tex;
    br_device_pixelmap* colour_pm;
    br_device_pixelmap* depth_pm;
    br_uint_16* colour_base;
    br_uint_16* depth_base;
    int cb_row, db_row;
    int cw, ch;
    float x0, y0, x1, y1, x2, y2;
    float den;
    int y_start, y_end, y;
    float alpha_val;
    br_uint_32 prim_flags;
    int smooth;

    colour_pm = state->out.pixelmap;
    depth_pm = state->out.depth;
    if (colour_pm == NULL || colour_pm->pm_pixels == NULL) {
        return;
    }

    tex = (br_buffer_stored_sdl*)state->prim.colour_map;
    prim_flags = state->prim.flags;
    alpha_val = state->prim.alpha_val;
    /* retail passes opacity on a 0..256 scale */
    if (alpha_val > 1.f) {
        alpha_val = alpha_val / 256.f;
    }
    if (alpha_val < 0.f) alpha_val = 0.f;
    if (alpha_val > 1.f) alpha_val = 1.f;
    smooth = (prim_flags & PRIMF_SMOOTH) != 0;

    colour_base = (br_uint_16*)colour_pm->pm_pixels;
    cb_row = colour_pm->pm_row_bytes / 2;
    cw = colour_pm->pm_width;
    ch = colour_pm->pm_height;
    if (depth_pm != NULL && depth_pm->pm_pixels != NULL) {
        depth_base = (br_uint_16*)depth_pm->pm_pixels;
        db_row = depth_pm->pm_row_bytes / 2;
    } else {
        depth_base = NULL;
        db_row = 0;
    }

    x0 = v0->comp_f[C_SX]; y0 = v0->comp_f[C_SY];
    x1 = v1->comp_f[C_SX]; y1 = v1->comp_f[C_SY];
    x2 = v2->comp_f[C_SX]; y2 = v2->comp_f[C_SY];

    den = (y1 - y2) * (x0 - x2) + (x2 - x1) * (y0 - y2);
    if (fabsf(den) < 0.0001f) {
        return;
    }

    y_start = (int)ceilf(fminf(y0, fminf(y1, y2)));
    y_end = (int)floorf(fmaxf(y0, fmaxf(y1, y2)));
    if (y_start < 0) y_start = 0;
    if (y_end > ch - 1) y_end = ch - 1;

    for (y = y_start; y <= y_end; y++) {
        float py = y + 0.5f;
        float xa, xb;
        int x_start, x_end, x;
        float l0, l1, l2;
        float l1_step, l2_step;
        float z_l, u_l, v_l, r_l, g_l, b_l;
        float z_step, u_step, v_step, r_step, g_step, b_step;
        br_uint_16* drow;
        br_uint_16* crow;

        /* Horizontal span: intersect triangle edges with the scanline */
        {
            float xi_a = 1e9f, xi_b = -1e9f;
            int have = 0;
            int ei;
            for (ei = 0; ei < 3; ei++) {
                float ex0, ey0, ex1, ey1;
                if (ei == 0) { ex0 = x0; ey0 = y0; ex1 = x1; ey1 = y1; }
                else if (ei == 1) { ex0 = x1; ey0 = y1; ex1 = x2; ey1 = y2; }
                else { ex0 = x2; ey0 = y2; ex1 = x0; ey1 = y0; }
                if ((ey0 <= py && ey1 >= py) || (ey1 <= py && ey0 >= py)) {
                    if (ey0 == ey1) {
                        float e0 = ex0 < ex1 ? ex0 : ex1;
                        float e1 = ex0 < ex1 ? ex1 : ex0;
                        if (!have) { xa = e0; xb = e1; have = 1; }
                        else {
                            if (e0 < xa) xa = e0;
                            if (e1 > xb) xb = e1;
                        }
                    } else {
                        float t = (py - ey0) / (ey1 - ey0);
                        float xi = ex0 + (ex1 - ex0) * t;
                        if (!have) { xa = xi; xb = xi; have = 1; }
                        else {
                            if (xi < xa) xa = xi;
                            if (xi > xb) xb = xi;
                        }
                    }
                }
            }
            if (!have) {
                continue;
            }
        }

        x_start = (int)ceilf(xa - 0.5f);
        x_end = (int)floorf(xb - 0.5f);
        if (x_start < 0) x_start = 0;
        if (x_end > cw - 1) x_end = cw - 1;
        if (x_start > x_end) {
            continue;
        }

        /* Barycentric weights at the first pixel centre; per-pixel steps */
        {
            float px = xa + 0.5f;
            l1 = ((y2 - y0) * (px - x2) + (x0 - x2) * (py - y2)) / den;
            l2 = ((y0 - y1) * (px - x2) + (x1 - x2) * (py - y2)) / den;
            l0 = 1.f - l1 - l2;
            l1_step = (y2 - y0) / den;
            l2_step = (y0 - y1) / den;
            z_step = l1_step * (v1->comp_f[C_SZ] - v0->comp_f[C_SZ]) + l2_step * (v2->comp_f[C_SZ] - v0->comp_f[C_SZ]);
            u_step = l1_step * (v1->comp_f[C_U] - v0->comp_f[C_U]) + l2_step * (v2->comp_f[C_U] - v0->comp_f[C_U]);
            v_step = l1_step * (v1->comp_f[C_V] - v0->comp_f[C_V]) + l2_step * (v2->comp_f[C_V] - v0->comp_f[C_V]);
            if (smooth) {
                r_step = l1_step * (v1->comp_f[C_R] - v0->comp_f[C_R]) + l2_step * (v2->comp_f[C_R] - v0->comp_f[C_R]);
                g_step = l1_step * (v1->comp_f[C_G] - v0->comp_f[C_G]) + l2_step * (v2->comp_f[C_G] - v0->comp_f[C_G]);
                b_step = l1_step * (v1->comp_f[C_B] - v0->comp_f[C_B]) + l2_step * (v2->comp_f[C_B] - v0->comp_f[C_B]);
            } else {
                r_step = g_step = b_step = 0.f;
            }
            z_l = l0 * v0->comp_f[C_SZ] + l1 * v1->comp_f[C_SZ] + l2 * v2->comp_f[C_SZ];
            u_l = l0 * v0->comp_f[C_U] + l1 * v1->comp_f[C_U] + l2 * v2->comp_f[C_U];
            v_l = l0 * v0->comp_f[C_V] + l1 * v1->comp_f[C_V] + l2 * v2->comp_f[C_V];
            if (smooth) {
                r_l = l0 * v0->comp_f[C_R] + l1 * v1->comp_f[C_R] + l2 * v2->comp_f[C_R];
                g_l = l0 * v0->comp_f[C_G] + l1 * v1->comp_f[C_G] + l2 * v2->comp_f[C_G];
                b_l = l0 * v0->comp_f[C_B] + l1 * v1->comp_f[C_B] + l2 * v2->comp_f[C_B];
            } else {
                r_l = g_l = b_l = 255.f;
            }
        }

        drow = depth_base ? depth_base + y * db_row : NULL;
        crow = colour_base + y * cb_row;

        for (x = x_start; x <= x_end; x++) {
            unsigned short dest;
            unsigned short dval;
            float rr = r_l, gg = g_l, bb = b_l;

            if (drow != NULL) {
                dval = (unsigned short)(z_l * 65535.f);
                if (dval >= drow[x]) {
                    goto next;
                }
            } else {
                dval = 0;
            }

            if (tex != NULL && tex->pm != NULL) {
                br_device_pixelmap* tpm = tex->pm;
                br_uint_16* tpix = (br_uint_16*)tpm->pm_pixels;
                int tw = tpm->pm_width;
                int th = tpm->pm_height;
                int tu, tv;
                unsigned short t;
                if (tw <= 0 || th <= 0 || tpm->pm_pixels == NULL) {
                    goto next;
                }
                tu = (int)(u_l * tw);
                tv = (int)(v_l * th);
                tu = tu % tw;
                tv = tv % th;
                if (tu < 0) tu += tw;
                if (tv < 0) tv += th;
                t = tpix[tv * (tpm->pm_row_bytes / 2) + tu];

                if (tpm->pm_type == (br_uint_8)BR_PMT_RGBA_4444) {
                    unsigned r = (t >> 8) & 0xF;
                    unsigned g = (t >> 4) & 0xF;
                    unsigned b = t & 0xF;
                    unsigned ta = (t >> 12) & 0xF;
                    float sa = alpha_val;
                    if (ta == 0) {
                        /* texel-alpha transparency: fully transparent texel */
                        goto next;
                    }
                    if (r == 15 && g == 0 && b == 15) {
                        /* magenta chroma key (retail colour-key semantics) */
                        goto next;
                    }
                    r = r * 17;
                    g = g * 17;
                    b = b * 17;
                    if (prim_flags & PRIMF_BLEND) {
                        /* retail blends with the material opacity constant */
                        sa = alpha_val;
                        if (sa <= 0.002f) {
                            goto next;
                        }
                        {
                            unsigned short d = crow[x];
                            unsigned dr5 = (d >> 11) & 0x1F;
                            unsigned dg6 = (d >> 5) & 0x3F;
                            unsigned db5 = d & 0x1F;
                            unsigned sr5 = (r * 31) / 255;
                            unsigned sg6 = (g * 63) / 255;
                            unsigned sb5 = (b * 31) / 255;
                            unsigned or5 = (unsigned)(sr5 * sa + dr5 * (1.f - sa));
                            unsigned og6 = (unsigned)(sg6 * sa + dg6 * (1.f - sa));
                            unsigned ob5 = (unsigned)(sb5 * sa + db5 * (1.f - sa));
                            dest = (unsigned short)((or5 << 11) | (og6 << 5) | ob5);
                        }
                    } else {
                        dest = (unsigned short)((((r * 31) / 255) << 11) | (((g * 63) / 255) << 5) | ((b * 31) / 255));
                    }
                } else {
                    if (t == 0xF81F) {
                        goto next;
                    }
                    if (smooth) {
                        unsigned r = (unsigned)(((t >> 11) & 0x1F) * rr / 255.f);
                        unsigned g = (unsigned)(((t >> 5) & 0x3F) * gg / 255.f);
                        unsigned b = (unsigned)((t & 0x1F) * bb / 255.f);
                        dest = (unsigned short)((r << 11) | (g << 5) | b);
                    } else if (prim_flags & PRIMF_BLEND) {
                        unsigned short d = crow[x];
                        float sa = alpha_val;
                        unsigned r = ((t >> 11) & 0x1F);
                        unsigned g = ((t >> 5) & 0x3F);
                        unsigned b = (t & 0x1F);
                        unsigned dr5 = (d >> 11) & 0x1F;
                        unsigned dg6 = (d >> 5) & 0x3F;
                        unsigned db5 = d & 0x1F;
                        unsigned or5 = (unsigned)(r * sa + dr5 * (1.f - sa));
                        unsigned og6 = (unsigned)(g * sa + dg6 * (1.f - sa));
                        unsigned ob5 = (unsigned)(b * sa + db5 * (1.f - sa));
                        dest = (unsigned short)((or5 << 11) | (og6 << 5) | ob5);
                    } else {
                        dest = t;
                    }
                }
            } else {
                /* flat: colour comes from the constant components
                 * (material->colour filled by softrend's surface fns) */
                unsigned r = (unsigned)v0->comp_f[C_R];
                unsigned g = (unsigned)v0->comp_f[C_G];
                unsigned b = (unsigned)v0->comp_f[C_B];
                if (prim_flags & PRIMF_BLEND) {
                    unsigned short d = crow[x];
                    float sa = alpha_val;
                    unsigned dr5 = (d >> 11) & 0x1F;
                    unsigned dg6 = (d >> 5) & 0x3F;
                    unsigned db5 = d & 0x1F;
                    unsigned or5 = (unsigned)((r * 31 / 255) * sa + dr5 * (1.f - sa));
                    unsigned og6 = (unsigned)((g * 63 / 255) * sa + dg6 * (1.f - sa));
                    unsigned ob5 = (unsigned)((b * 31 / 255) * sa + db5 * (1.f - sa));
                    dest = (unsigned short)((or5 << 11) | (og6 << 5) | ob5);
                } else {
                    dest = (unsigned short)(((r * 31 / 255) << 11) | ((g * 63 / 255) << 5) | (b * 31 / 255));
                }
            }

            crow[x] = dest;
            if (drow != NULL) {
                drow[x] = dval;
            }
        next:
            z_l += z_step;
            u_l += u_step;
            v_l += v_step;
            r_l += r_step;
            g_l += g_step;
            b_l += b_step;
        }
    }
}

static br_error C2_HOOK_CDECL state_renderBegin(br_primitive_state_sdl* self, brp_block** blocks, br_boolean* block_changed, br_boolean* ranges_changed, br_boolean no_render, br_int_32 subdivide_tolerance) {
    local_block_sdl* lb;
    br_uint_32 flags;

    (void)subdivide_tolerance;
    *ranges_changed = 1;

    if (self->cache.last_type == BRT_TRIANGLE) {
        lb = (local_block_sdl*)self->cache.last_block;
        *blocks = &lb->p;
        *block_changed = 0;
        (void)no_render;
        return 0;
    }

    flags = self->prim.flags;
    if (self->prim.colour_map != NULL) {
        flags |= PRIMF_TEXTURE_BUFFER;
    }
    if (self->out.depth != NULL) {
        flags |= PRIMF_DEPTH_BUFFER;
    }

    lb = BrResAllocate(self, sizeof(*lb), BR_MEMORY_OBJECT);
    if (lb == NULL) {
        return SDL_PRIM_UNSUPPORTED;
    }
    lb->sig = flags;
    lb->p.render = (brp_render_fn*)sdlTriangleRender;
    lb->p.chain = NULL;
    lb->p.type = BRT_TRIANGLE;
    lb->p.flags = 0;
    if (flags & PRIMF_BLEND) {
        lb->p.flags |= BR_PRIMF_BLENDED;
    }
    if (flags & PRIMF_SMOOTH) {
        lb->p.constant_components = 0;
        lb->p.vertex_components = CM_SX | CM_SY | CM_SZ | CM_U | CM_V | CM_R | CM_G | CM_B;
        lb->p.convert_mask_f = CM_SX | CM_SY | CM_SZ | CM_U | CM_V;
        lb->p.convert_mask_i = CM_R | CM_G | CM_B;
        lb->p.constant_mask = 0;
    } else {
        lb->p.constant_components = CM_R | CM_G | CM_B;
        lb->p.vertex_components = CM_SX | CM_SY | CM_SZ | CM_U | CM_V;
        lb->p.convert_mask_f = CM_SX | CM_SY | CM_SZ | CM_U | CM_V;
        lb->p.convert_mask_i = 0;
        lb->p.constant_mask = CM_R | CM_G | CM_B;
    }
    if (flags & PRIMF_BLEND) {
        lb->p.flags |= BR_PRIMF_BLENDED;
        /* softrend's SurfaceAlpha fills comp[C_A] from the material opacity */
        lb->p.constant_components |= CM_A;
        lb->p.constant_mask |= CM_A;
    }
    lb->p.subdivide_tolerance = 0;
    lb->state = self;

    self->cache.last_block = lb;
    self->cache.last_type = BRT_TRIANGLE;
    *blocks = &lb->p;
    *block_changed = 1;
    (void)no_render;
    return 0;
}

static br_error C2_HOOK_CDECL state_renderEnd(br_primitive_state_sdl* self, brp_block* block) {
    (void)self; (void)block;
    return 0;
}

static br_error C2_HOOK_CDECL state_rangesQueryF(br_primitive_state_sdl* self, br_float* dest, br_float* src, br_int_32 size) {
    br_device_pixelmap* pm = self->out.pixelmap;
    int i;
    float w = 320.f, h = 240.f;
    int base_x = 0, base_y = 0;

    if (pm != NULL) {
        w = pm->pm_width / 2.f;
        h = pm->pm_height / 2.f;
        base_x = pm->pm_base_x;
        base_y = pm->pm_base_y;
    }

    for (i = 0; i < size && i < 16; i++) {
        dest[i] = 0.f;
        src[i] = 1.f;
    }
    if (size > C_SX) {
        dest[C_SX] = base_x + w;
        src[C_SX] = w;
    }
    if (size > C_SY) {
        dest[C_SY] = base_y + h;
        src[C_SY] = -h;
    }
    if (size > C_SZ) {
        dest[C_SZ] = 0.f;
        src[C_SZ] = 1.f;
    }
    if (size > C_U) {
        dest[C_U] = 0.f;
        src[C_U] = 1.f;
    }
    if (size > C_V) {
        dest[C_V] = 0.f;
        src[C_V] = 1.f;
    }
    if (size > C_R) {
        dest[C_R] = 0.f;
        src[C_R] = 255.f;
    }
    if (size > C_G) {
        dest[C_G] = 0.f;
        src[C_G] = 255.f;
    }
    if (size > C_B) {
        dest[C_B] = 0.f;
        src[C_B] = 255.f;
    }
    if (size > C_I) {
        dest[C_I] = 0.f;
        src[C_I] = 255.f;
    }
    return 0;
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

static br_error C2_HOOK_CDECL lib_query(br_primitive_library_sdl* self, br_uint_32* pvalue, br_token t) {
    (void)self;
    if (pvalue != NULL) {
        if (t == BRT_PARTS_U32) {
            *pvalue = BR_STATE_PRIMITIVE | BR_STATE_OUTPUT;
        } else {
            *pvalue = 0;
        }
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
    (void)self; (void)psize; (void)tv;
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
    s->plib = self;
    s->prim.flags = 0;
    s->prim.colour_map = NULL;
    s->prim.alpha_val = 1.f;
    s->prim.colour_type = BRT_DEFAULT;
    s->prim.map_interpolation = BRT_NONE;
    s->cache.last_block = NULL;
    s->cache.last_type = BR_NULL_TOKEN;

    *pstate = (br_primitive_state*)s;
    return 0;
}

static br_error C2_HOOK_CDECL lib_bufferStoredNew(br_primitive_library_sdl* self, br_buffer_stored** psm, br_token use, br_device_pixelmap* pm, br_token_value* tv) {
    return BufferStoredSDLAllocate(self, psm, use, pm, tv);
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
