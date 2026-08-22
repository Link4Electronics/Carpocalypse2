/*
 * Geometry format for version 1 models — shared glrend/sdl3gpurend.
 *
 * The two drivers differ only in how they build and free the GPU-side
 * vertex/index storage and in GL's legacy order-table (bucket) divert path,
 * which is never enabled by the game (state_hidden.divert stays BRT_NONE).
 * Everything else — the template/object bookkeeping, the allocate scaffolding
 * and the per-group render loop — is identical and lives here.
 */
#include "brassert.h"
#include "drv.h"
#include "formats.h"
#include "commonrend.h"
#include "state.h"
#include <math.h>
#include <string.h>

/*
 * Default dispatch table for geometry type (defined at end of file)
 */
static const struct br_geometry_stored_dispatch geometryStoredDispatch;

/*
 * Geometry format info. template
 */
#define F(f) offsetof(br_geometry_stored, f)

static struct br_tv_template_entry templateEntries[] = {
    { BRT(IDENTIFIER_CSTR), F(identifier), BRTV_QUERY | BRTV_ALL, BRTV_CONV_COPY },
    { BRT(GEOMETRY_V1_MODEL), F(gv1model), BRTV_QUERY | BRTV_ALL, BRTV_CONV_COPY },
    { BRT(SHARED_B), F(shared), BRTV_QUERY | BRTV_ALL, BRTV_CONV_COPY },
};
#undef F

#if defined(BREND_DRIVER_GL)
/* ------------------------------------------------------------------ GL --- */

static GLuint create_vao(HVIDEO hVideo, GLuint vbo_posn, GLuint vbo, GLuint ibo) {
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    /* Separate buffer for positions. Makes it easier on tiling (i.e. mobile) GPUs. */
    glBindBuffer(GL_ARRAY_BUFFER, vbo_posn);
    glEnableVertexAttribArray(hVideo->brenderProgram.attributes.aPosition);
    glVertexAttribPointer(hVideo->brenderProgram.attributes.aPosition, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    if (hVideo->brenderProgram.attributes.aUV >= 0) {
        glEnableVertexAttribArray(hVideo->brenderProgram.attributes.aUV);
        glVertexAttribPointer(hVideo->brenderProgram.attributes.aUV, 2, GL_FLOAT, GL_FALSE, sizeof(gl_vertex_f),
            (void*)offsetof(gl_vertex_f, map));
    }

    if (hVideo->brenderProgram.attributes.aNormal >= 0) {
        glEnableVertexAttribArray(hVideo->brenderProgram.attributes.aNormal);
        glVertexAttribPointer(hVideo->brenderProgram.attributes.aNormal, 3, GL_FLOAT, GL_FALSE, sizeof(gl_vertex_f),
            (void*)offsetof(gl_vertex_f, n));
    }

    if (hVideo->brenderProgram.attributes.aColour >= 0) {
        glEnableVertexAttribArray(hVideo->brenderProgram.attributes.aColour);
        glVertexAttribPointer(hVideo->brenderProgram.attributes.aColour, 4, GL_FLOAT, GL_FALSE,
            sizeof(gl_vertex_f), (void*)offsetof(gl_vertex_f, c));
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBindVertexArray(0);
    return vao;
}

static GLuint build_vbo_posn(const struct v11model* model, size_t total_vertices) {
    br_vector3_f* vtx = BrScratchAllocate(total_vertices * sizeof(br_vector3_f));
    br_vector3_f* next = vtx;
    GLuint buf;

    for (br_uint_16 i = 0; i < model->ngroups; ++i) {
        const struct v11group* gp = model->groups + i;
        memcpy(next, gp->position, gp->nvertices * sizeof(br_vector3_f));
        next += gp->nvertices;
    }

    glGenBuffers(1, &buf);
    glBindBuffer(GL_ARRAY_BUFFER, buf);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(total_vertices * sizeof(br_vector3_f)), vtx, GL_STATIC_DRAW);
    BrScratchFree(vtx);
    return buf;
}

static GLuint build_vbo(const struct v11model* model, size_t total_vertices) {
    /* Collate and upload the vertex data. */
    gl_vertex_f* vtx = (gl_vertex_f*)BrScratchAllocate(total_vertices * sizeof(gl_vertex_f));
    gl_vertex_f* nextVtx = vtx;
    GLuint buf;

    for (br_uint_16 i = 0; i < model->ngroups; ++i) {
        const struct v11group* gp = model->groups + i;
        for (br_uint_16 v = 0; v < gp->nvertices; ++v, ++nextVtx) {
            nextVtx->map = *(br_vector2_f*)(gp->map + v);
            nextVtx->n = *(br_vector3_f*)(gp->normal + v);
            nextVtx->c.v[0] = BR_RED(gp->vertex_colours[v]) / 255.0f;
            nextVtx->c.v[1] = BR_GRN(gp->vertex_colours[v]) / 255.0f;
            nextVtx->c.v[2] = BR_BLU(gp->vertex_colours[v]) / 255.0f;
            nextVtx->c.v[3] = BR_ALPHA(gp->vertex_colours[v]) / 255.0f;
        }
    }

    glGenBuffers(1, &buf);
    glBindBuffer(GL_ARRAY_BUFFER, buf);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(total_vertices * sizeof(gl_vertex_f)), vtx, GL_STATIC_DRAW);
    BrScratchFree(vtx);
    return buf;
}

static GLuint build_ibo(const struct v11model* model, size_t total_faces, gl_groupinfo* groups) {
    br_uint_16* idx = (br_uint_16*)BrScratchAllocate(total_faces * 3 * sizeof(br_uint_16));
    GLuint buf;

    br_uint_16* nextIdx = idx;
    br_uint_16 offset = 0;
    br_size_t face_offset = 0;

    for (br_uint_16 i = 0; i < model->ngroups; ++i) {
        const struct v11group* gp = model->groups + i;
        for (br_uint_16 f = 0; f < gp->nfaces; ++f) {
            const br_vector3_u16* fp = gp->vertex_numbers + f;
            *nextIdx++ = fp->v[0] + offset;
            *nextIdx++ = fp->v[1] + offset;
            *nextIdx++ = fp->v[2] + offset;
        }

        groups[i].count = (GLsizei)gp->nfaces * 3;
        groups[i].offset = (void*)(br_uintptr_t)face_offset;
        groups[i].group = model->groups + i;

        face_offset += (br_size_t)gp->nfaces * 3 * sizeof(br_uint_16);

        offset += model->groups[i].nvertices;
    }

    glGenBuffers(1, &buf);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)face_offset, idx, GL_STATIC_DRAW);
    BrScratchFree(idx);
    return buf;
}

static br_primitive* heapPrimitiveAdd(br_primitive_heap* heap, br_token type) {
    br_primitive* p = (br_primitive*)heap->current;

    ASSERT_MESSAGE("Out of heap space", p != NULL);

    p->type = type;

    heap->current += sizeof(br_primitive);
    return p;
}

enum {
    RM_FORCE_BACK = 0,
    RM_OPAQUE = 1,
    RM_TRANS = 2,
    RM_FORCE_FRONT = 3,
    RM_MAX = 4,
};

static int get_render_mode(const state_stack* state) {
    if (state->valid & MASK_STATE_SURFACE) {
        if (state->surface.force_back)
            return RM_FORCE_BACK;

        if (state->surface.force_front)
            return RM_FORCE_FRONT;

        /* Transparent? Defer. */
        if (state->surface.opacity < BR_SCALAR(1.0f))
            return RM_TRANS;
    }

    if (state->valid & MASK_STATE_PRIMITIVE) {
        /* Blend flags set? Defer.*/
        if (state->prim.flags & PRIMF_BLEND)
            return RM_TRANS;

        /* Has a blend table? Defer. */
        if (state->prim.index_blend != NULL)
            return RM_TRANS;

        if (state->prim.colour_map && state->prim.colour_map->source) {
            /* Keyed transparency? Defer. */
            if (state->prim.colour_map->source->flags & BR_PMF_KEYED_TRANSPARENCY)
                return RM_TRANS;
        }
    }

    return RM_OPAQUE;
}

/*
 * Determine which bucket to dump things into. At least four are needed
 * for the various render types.
 *
 * | Type        | Ratio | DT | Order |
 * |-------------|-------|----|-------|
 * | Force back  |  5%   |    | BtF   |
 * | Opaque      | 80%   | X  | FtB   |
 * | Transparent | 10%   | X  | BtF   |
 * | Force front |  5%   |    | BtF   |
 *
 * DT  = Depth Tested
 * BtF = Back-to-front
 * TtB = Front-to-back
 */
static br_uint_16 calculate_bucket(const br_order_table* ot, const state_stack* state, br_scalar* depth) {
    const br_scalar ratio_force_frontback = BR_SCALAR(0.05);
    const br_scalar ratio_transparent = BR_SCALAR(0.10);
    br_uint_16 count_forced, count_opaque, count_trans;
    int render_mode;
    br_scalar ot_size;
    br_uint_16 base, count;
    br_scalar tmp_depth;
    br_boolean force_btf = (state->valid & MASK_STATE_OUTPUT) && state->output.depth == NULL;

    ASSERT(BR_ADD(BR_MUL(ratio_force_frontback, BR_SCALAR(2)), ratio_transparent) < BR_SCALAR(1.0));

    render_mode = get_render_mode(state);

    /*
     * Case 1 - A single bucket.
     * Force back/front geometry to the back/front and pray
     * for the best
     */
    if (ot->size == 1) {
        if (render_mode == RM_FORCE_BACK)
            *depth = BR_SCALAR_MAX;
        else if (render_mode == RM_FORCE_FRONT)
            *depth = BR_SCALAR(0.0);
        return 0;
    }

    /*
     * Case 2 - 2 buckets.
     */
    if (ot->size == 2) {
        switch (render_mode) {
        case RM_FORCE_BACK:
            *depth = BR_SCALAR_MAX;
        case RM_OPAQUE:
        default:
            return 1;
        case RM_FORCE_FRONT:
            *depth = BR_SCALAR(0.0);
        case RM_TRANS:
            return 0;
        }
    }

    /*
     * Case 3 - 3 buckets.
     */
    if (ot->size == 3) {
        switch (render_mode) {
        case RM_FORCE_BACK:
            *depth = BR_SCALAR_MAX;
        case RM_OPAQUE:
        default:
            return 2;

        case RM_TRANS:
            return 1;
        case RM_FORCE_FRONT:
            return 0;
        }
    }

    /*
     * Case 4 - 4 buckets.
     */
    if (ot->size == 4) {
        switch (render_mode) {
        case RM_FORCE_BACK:
            return 3;
        case RM_OPAQUE:
        default:
            return 2;
        case RM_TRANS:
            return 1;
        case RM_FORCE_FRONT:
            return 0;
        }
    }

    /*
     * Case 5 - >4 buckets
     */
    ot_size = BR_SCALAR(ot->size);
    count_forced = (br_uint_16)ceilf(ot_size * ratio_force_frontback);
    count_trans = (br_uint_16)((float)(ot->size - (count_forced << 1)) - ceilf(ot_size * ratio_transparent));
    count_opaque = ot->size - count_trans - (count_forced << 1);

    ASSERT(count_forced + count_opaque + count_trans + count_forced == ot->size);

    tmp_depth = *depth;
    switch (render_mode) {
    case RM_FORCE_FRONT:
        base = 0;
        count = count_forced;
        break;
    case RM_TRANS:
        base = count_forced;
        count = count_opaque;
        break;
    case RM_OPAQUE:
    default:
        base = count_forced + count_trans;
        count = count_opaque;

        if (!force_btf)
            tmp_depth = -tmp_depth;
        break;
    case RM_FORCE_BACK:
        base = count_forced + count_trans + count_opaque;
        count = count_forced;
        break;
    }

    return base + BrZsPrimitiveBucketSelect(&tmp_depth, BR_PRIMITIVE_POINT, ot->min_z, ot->max_z, count, ot->type);
}

static br_boolean want_defer(const state_hidden* hidden) {
    if (hidden->type != BRT_BUCKET_SORT)
        return BR_FALSE;

    if (hidden->divert == BRT_NONE)
        return BR_FALSE;

    if (hidden->divert == BRT_ALL)
        return BR_TRUE;

    UASSERT(hidden->divert == BRT_BLENDED);

    return hidden->order_table != NULL && hidden->heap != NULL;
}

#else
/* ------------------------------------------------------------ SDL3GPUREND --- */

#define SDL3GPUREND_DYN_SMALL_MAX_VERTEX_BYTES (16u * 1024u)
#define SDL3GPUREND_DYN_SMALL_MAX_INDEX_BYTES (16u * 1024u)

static sdl3_vertex_f* GenerateVBOData(const struct v11model* model, size_t total_vertices) {
    sdl3_vertex_f* vtx = BrScratchAllocate(total_vertices * sizeof(sdl3_vertex_f));
    sdl3_vertex_f* next = vtx;

    for (br_uint_16 i = 0; i < model->ngroups; ++i) {
        const struct v11group* gp = model->groups + i;
        for (br_uint_16 v = 0; v < gp->nvertices; ++v, ++next) {
            next->p = *(br_vector3_f*)(gp->position + v);
            next->map = *(br_vector2_f*)(gp->map + v);
            next->n = *(br_vector3_f*)(gp->normal + v);
            next->c.v[0] = BR_RED(gp->vertex_colours[v]) / 255.0f;
            next->c.v[1] = BR_GRN(gp->vertex_colours[v]) / 255.0f;
            next->c.v[2] = BR_BLU(gp->vertex_colours[v]) / 255.0f;
            next->c.v[3] = BR_ALPHA(gp->vertex_colours[v]) / 255.0f;
        }
    }
    return vtx;
}

static br_uint_16* GenerateIBOData(const struct v11model* model, size_t total_faces, br_size_t* out_size, sdl3_groupinfo* sdl3_groups) {
    br_uint_16* idx = BrScratchAllocate(total_faces * 3 * sizeof(br_uint_16));
    br_uint_16* next = idx;
    br_uint_16 offset = 0;
    br_size_t face_offset = 0;

    for (br_uint_16 i = 0; i < model->ngroups; ++i) {
        const struct v11group* gp = model->groups + i;
        if (sdl3_groups != NULL) {
            sdl3_groups[i].count = gp->nfaces * 3;
            sdl3_groups[i].offset = (uint32_t)(face_offset);
            sdl3_groups[i].group = model->groups + i;
            sdl3_groups[i].stored = NULL;
        }

        for (br_uint_16 f = 0; f < gp->nfaces; ++f) {
            const br_vector3_u16* fp = gp->vertex_numbers + f;
            *next++ = fp->v[0] + offset;
            *next++ = fp->v[1] + offset;
            *next++ = fp->v[2] + offset;
        }

        face_offset += (br_size_t)gp->nfaces * 3 * sizeof(br_uint_16);
        offset += model->groups[i].nvertices;
    }

    *out_size = face_offset;
    return idx;
}

static void UploadVBOToDedicated(HVIDEO hVideo, br_geometry_stored* self, const sdl3_vertex_f* vtx, size_t size) {
    self->inDynamicRing = 0;
    self->vboOffset = 0;

    SDL_GPUBufferCreateInfo bi = {0};
    bi.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
    bi.size = (Uint32)size;
    self->vbo = SDL3_CreateGPUBuffer(hVideo->device, &bi);
    if (!self->vbo) {
        BR_FATAL("SDL3GPU: Failed to create dedicated VBO.");
        return;
    }
    SDL3GPUREND_UploadBufferToBuffer(hVideo, self->vbo, vtx, size);
}

static void UploadIBOToDedicated(HVIDEO hVideo, br_geometry_stored* self, const br_uint_16* idx, size_t size) {
    self->iboOffset = 0;

    SDL_GPUBufferCreateInfo bi = {0};
    bi.usage = SDL_GPU_BUFFERUSAGE_INDEX;
    bi.size = (Uint32)size;
    self->ibo = SDL3_CreateGPUBuffer(hVideo->device, &bi);
    if (!self->ibo) {
        BR_FATAL("SDL3GPU: Failed to create dedicated IBO.");
        return;
    }
    SDL3GPUREND_UploadBufferToBuffer(hVideo, self->ibo, idx, size);
}

static void build_vbo(HVIDEO hVideo, br_geometry_stored* self, const struct v11model* model, size_t total_vertices) {
    sdl3_vertex_f* vtx = GenerateVBOData(model, total_vertices);

    size_t size = total_vertices * sizeof(sdl3_vertex_f);
    int f = hVideo->currentFrame;

    /* Small models rebuilt during frame recording (electro-ray segments, sparks,
     * dim quads, pratcam quad) are sub-allocated from the persistent ring: a plain
     * memcpy, no SDL_CreateGPUBuffer/upload per rebuild. The ring cursor only
     * advances within a frame (no wrap), so every build gets its own slot; it
     * resets next frame after the fence wait. Models built outside recording
     * (load time) and oversized models keep dedicated buffers. Ring data is only
     * valid until the next frame's reset, so ringEpoch stamps the frame and a
     * stale model is re-uploaded at render time. */
    if (hVideo->isRecording && hVideo->dynVboMapped[f] != NULL &&
        size <= SDL3GPUREND_DYN_SMALL_MAX_VERTEX_BYTES &&
        hVideo->dynVboOffset[f] + size <= hVideo->dynVboCapacity) {
        memcpy((char*)hVideo->dynVboMapped[f] + hVideo->dynVboOffset[f], vtx, size);
        self->vbo = hVideo->dynVbo[f];
        self->inDynamicRing = 1;
        self->vboOffset = hVideo->dynVboOffset[f];
        self->ringEpoch = hVideo->frameEpoch;
        hVideo->dynVboOffset[f] += size;
        hVideo->dynVboWritten[f] = hVideo->dynVboOffset[f];
    } else {
        UploadVBOToDedicated(hVideo, self, vtx, size);
    }

    BrScratchFree(vtx);
}

static void build_ibo(HVIDEO hVideo, br_geometry_stored* self, const struct v11model* model, size_t total_faces) {
    br_size_t size = 0;
    br_uint_16* idx = GenerateIBOData(model, total_faces, &size, self->sdl3_groups);
    int f = hVideo->currentFrame;

    if (hVideo->isRecording && hVideo->dynIboMapped[f] != NULL &&
        size <= SDL3GPUREND_DYN_SMALL_MAX_INDEX_BYTES &&
        hVideo->dynIboOffset[f] + size <= hVideo->dynIboCapacity) {
        memcpy((char*)hVideo->dynIboMapped[f] + hVideo->dynIboOffset[f], idx, size);
        self->ibo = hVideo->dynIbo[f];
        self->inDynamicRing = 1;
        self->iboOffset = hVideo->dynIboOffset[f];
        self->ringEpoch = hVideo->frameEpoch;
        hVideo->dynIboOffset[f] += size;
        hVideo->dynIboWritten[f] = hVideo->dynIboOffset[f];
    } else {
        UploadIBOToDedicated(hVideo, self, idx, size);
    }

    BrScratchFree(idx);
}

/* Re-uploads a stale ring sub-allocation into the current frame's ring slot.
 * The ring cursors are reset every frame in SDL3GPUREND_EnsureRecording, so any
 * ring model that is not rebuilt this frame (built during a previous frame's
 * recording and persisted) references clobbered data. This regenerates the
 * vertex/index data from the v11model and copies it into the current slot,
 * updating the bind offsets. Falls back to a dedicated buffer if the ring is
 * full, permanently graduating the model out of the ring. Only the components
 * actually in the ring (inDynamicRing) are refreshed; a mixed
 * vbo-in-ring/ibo-dedicated model keeps its dedicated component. */
void SDL3GPUREND_RefreshRingStored(HVIDEO hVideo, br_geometry_stored* self) {
    int f = hVideo->currentFrame;
    int wasInRing = self->inDynamicRing;

    size_t total_vertices = 0, total_faces = 0;
    for (br_uint_16 i = 0; i < self->model->ngroups; ++i) {
        total_vertices += self->model->groups[i].nvertices;
        total_faces += self->model->groups[i].nfaces;
    }

    if (wasInRing) {
        sdl3_vertex_f* vtx = GenerateVBOData(self->model, total_vertices);
        size_t size = total_vertices * sizeof(sdl3_vertex_f);
        if (hVideo->dynVboMapped[f] != NULL &&
            hVideo->dynVboOffset[f] + size <= hVideo->dynVboCapacity) {
            memcpy((char*)hVideo->dynVboMapped[f] + hVideo->dynVboOffset[f], vtx, size);
            self->vbo = hVideo->dynVbo[f];
            self->vboOffset = hVideo->dynVboOffset[f];
            hVideo->dynVboOffset[f] += size;
            hVideo->dynVboWritten[f] = hVideo->dynVboOffset[f];
        } else {
            UploadVBOToDedicated(hVideo, self, vtx, size);
        }
        BrScratchFree(vtx);
    }

    if (wasInRing) {
        br_size_t size = 0;
        br_uint_16* idx = GenerateIBOData(self->model, total_faces, &size, NULL);
        if (hVideo->dynIboMapped[f] != NULL &&
            hVideo->dynIboOffset[f] + size <= hVideo->dynIboCapacity) {
            memcpy((char*)hVideo->dynIboMapped[f] + hVideo->dynIboOffset[f], idx, size);
            self->ibo = hVideo->dynIbo[f];
            self->iboOffset = hVideo->dynIboOffset[f];
            hVideo->dynIboOffset[f] += size;
            hVideo->dynIboWritten[f] = hVideo->dynIboOffset[f];
        } else {
            UploadIBOToDedicated(hVideo, self, idx, size);
        }
        BrScratchFree(idx);
    }

    self->ringEpoch = hVideo->frameEpoch;
}

#endif /* BREND_DRIVER_GL / SDL3GPUREND */

/* --------------------------------------------------------- Allocate --- */

br_geometry_stored* BREND_FN(GeometryStored, Allocate)(br_geometry_v1_model* gv1model, const char* id, br_renderer* r, struct v11model* model) {
    size_t total_vertices = 0, total_faces = 0;
    br_geometry_stored* self;

    self = BrResAllocate(gv1model->renderer_facility->object_list, sizeof(*self), BR_MEMORY_OBJECT);
    self->dispatch = &geometryStoredDispatch;
    self->identifier = id;
    self->device = gv1model->device;
    self->gv1model = gv1model;

    ObjectContainerAddFront(gv1model->renderer_facility, (br_object*)self);

    self->model = model;
    self->shared = BR_TRUE;

    for (br_uint_16 i = 0; i < model->ngroups; ++i) {
        total_vertices += model->groups[i].nvertices;
        total_faces += model->groups[i].nfaces;
    }

#if defined(BREND_DRIVER_GL)
    self->groups = BrResAllocate(gv1model, sizeof(gl_groupinfo) * model->ngroups, BR_MEMORY_OBJECT_DATA);

    glBindVertexArray(0);

    self->gl_vbo_posn = build_vbo_posn(model, total_vertices);
    self->gl_vbo = build_vbo(model, total_vertices);
    self->gl_ibo = build_ibo(model, total_faces, self->groups);
    self->gl_vao = create_vao(&r->pixelmap->screen->asFront.video, self->gl_vbo_posn, self->gl_vbo, self->gl_ibo);

    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
#else
    {
        br_device_pixelmap* screen = r->pixelmap->screen;
        HVIDEO hVideo = &screen->asFront.video;

        self->deviceHandle = hVideo->device;
        self->hVideo = hVideo;

        self->num_groups = model->ngroups;
        self->sdl3_groups = BrResAllocate(gv1model, sizeof(sdl3_groupinfo) * model->ngroups, BR_MEMORY_OBJECT_DATA);

        build_vbo(hVideo, self, model, total_vertices);
        build_ibo(hVideo, self, model, total_faces);
    }
#endif

    return (br_geometry_stored*)self;
}

/* ------------------------------------------------------------ Object --- */

#if defined(BREND_DRIVER_GL)
static void BREND_CMETHOD_DECL(BREND_CLASS(br_geometry_stored), free)(br_object* _self) {
    br_geometry_stored* self = (br_geometry_stored*)_self;

    ObjectContainerRemove(self->gv1model->renderer_facility, (br_object*)self);

    glDeleteVertexArrays(1, &self->gl_vao);
    glDeleteBuffers(1, &self->gl_vbo_posn);
    glDeleteBuffers(1, &self->gl_vbo);
    glDeleteBuffers(1, &self->gl_ibo);
    BrResFreeNoCallback(self);
}
#else
static void BREND_CMETHOD_DECL(BREND_CLASS(br_geometry_stored), free)(br_object* _self) {
    br_geometry_stored* self = (br_geometry_stored*)_self;

    ObjectContainerRemove(self->gv1model->renderer_facility, (br_object*)self);

    /* SDL3 GPU resources are reference-counted: SDL_ReleaseGPUBuffer schedules
     * the safe destruction, so freeing a buffer that may still be referenced by
     * in-flight command buffers is handled by the backend. Ring sub-allocations
     * (inDynamicRing) share the video context's ring buffers (owned by the video
     * context, released in ReleaseRings) and must not be released here. */
    HVIDEO hVideo = self->hVideo;
    if (hVideo != NULL && !self->inDynamicRing) {
        if (self->vbo) SDL3GPUREND_DeferFreeBuffer(hVideo, self->vbo);
        if (self->ibo) SDL3GPUREND_DeferFreeBuffer(hVideo, self->ibo);
        self->vbo = NULL;
        self->ibo = NULL;
    }

    BrResFreeNoCallback(self);
}
#endif

static char* BREND_CMETHOD_DECL(BREND_CLASS(br_geometry_stored), identifier)(br_object* self) {
    return (char*)((br_geometry_stored*)self)->identifier;
}

static br_device* BREND_CMETHOD_DECL(BREND_CLASS(br_geometry_stored), device)(br_object* self) {
    return ((br_geometry_stored*)self)->device;
}

static br_token BREND_CMETHOD_DECL(BREND_CLASS(br_geometry_stored), type)(br_object* self) {
    (void)self;
    return BRT_GEOMETRY_STORED;
}

static br_boolean BREND_CMETHOD_DECL(BREND_CLASS(br_geometry_stored), isType)(br_object* self, br_token t) {
    (void)self;
    return (t == BRT_GEOMETRY_STORED) || (t == BRT_GEOMETRY) || (t == BRT_OBJECT);
}

static br_int_32 BREND_CMETHOD_DECL(BREND_CLASS(br_geometry_stored), space)(br_object* self) {
    (void)self;
    return (br_int_32)sizeof(br_geometry_stored);
}

static struct br_tv_template* BREND_CMETHOD_DECL(BREND_CLASS(br_geometry_stored), templateQuery)(br_object* _self) {
    br_geometry_stored* self = (br_geometry_stored*)_self;

    if (self->device->templates.geometryStoredTemplate == NULL) {
        self->device->templates.geometryStoredTemplate = BrTVTemplateAllocate(self->device, templateEntries,
            BR_ASIZE(templateEntries));
    }

    return self->device->templates.geometryStoredTemplate;
}

/* ------------------------------------------------------- Render loop --- */

static br_error V1Model_RenderStored(br_geometry_stored* self, br_renderer* renderer, struct br_renderer_state_stored* default_state) {
#if defined(BREND_DRIVER_GL)
    state_stack* state;
    br_primitive* prim;
    br_vector3 pos;
    br_boolean defer;
    br_scalar distance_from_zero;

    state = renderer->state.current;

    BrVector3Set(&pos, state->matrix.model_to_view.m[3][0], state->matrix.model_to_view.m[3][1],
        state->matrix.model_to_view.m[3][2]);
    distance_from_zero = BrVector3Length(&pos);

    defer = want_defer(&state->hidden);
#endif

    for (int i = 0; i < self->model->ngroups; ++i) {
        struct v11group* group = self->model->groups + i;
#if defined(BREND_DRIVER_GL)
        gl_groupinfo* groupinfo = self->groups + i;
#else
        sdl3_groupinfo* groupinfo = self->sdl3_groups + i;
#endif
        br_renderer_state_stored* stored = (br_renderer_state_stored*)group->stored;

        groupinfo->stored = stored;
        groupinfo->default_state = default_state;

#if defined(BREND_DRIVER_GL)
        if (defer) {
            br_order_table* ot = state->hidden.order_table;
            br_uint_16 bucket;
            state_stack* tmpstate;

            tmpstate = BrPoolBlockAllocate(renderer->state_pool);

            prim = heapPrimitiveAdd(state->hidden.heap, BRT_GEOMETRY_STORED);
            prim->stored = stored;
            prim->v[0] = self;
            prim->v[1] = tmpstate;
            prim->v[2] = groupinfo;

            *tmpstate = *state;

            /*
             * If the user set a function defer to them.
             */
            if (state->hidden.insert_fn != NULL) {
                state->hidden.insert_fn(prim, state->hidden.insert_arg1, state->hidden.insert_arg2,
                    state->hidden.insert_arg3, ot, &prim->depth);
                continue;
            }

            bucket = calculate_bucket(ot, stored ? &stored->state : state, &distance_from_zero);
            prim->depth = distance_from_zero;
            BrZsOrderTablePrimitiveInsert(ot, prim, bucket);
        } else {
            StoredGLRenderGroup(self, renderer, groupinfo);
        }
#else
        StoredSDL3GPURENDRenderGroup(self, renderer, groupinfo);
#endif
    }
    renderer->frame_stats.model_count++;
    return BRE_OK;
}

static br_error BREND_CMETHOD_DECL(BREND_CLASS(br_geometry_stored), render)(br_geometry_stored* self, br_renderer* renderer, struct br_renderer_state_stored* default_state) {
    return V1Model_RenderStored(self, renderer, default_state);
}

/*
 * Default dispatch table for geometry type
 */
static const struct br_geometry_stored_dispatch geometryStoredDispatch = {
    .__reserved0 = NULL,
    .__reserved1 = NULL,
    .__reserved2 = NULL,
    .__reserved3 = NULL,
    ._free = BREND_CMETHOD_REF(BREND_CLASS(br_geometry_stored), free),
    ._identifier = BREND_CMETHOD_REF(BREND_CLASS(br_geometry_stored), identifier),
    ._type = BREND_CMETHOD_REF(BREND_CLASS(br_geometry_stored), type),
    ._isType = BREND_CMETHOD_REF(BREND_CLASS(br_geometry_stored), isType),
    ._device = BREND_CMETHOD_REF(BREND_CLASS(br_geometry_stored), device),
    ._space = BREND_CMETHOD_REF(BREND_CLASS(br_geometry_stored), space),

    ._templateQuery = BREND_CMETHOD_REF(BREND_CLASS(br_geometry_stored), templateQuery),
    ._query = BR_CMETHOD_REF(br_object, query),
    ._queryBuffer = BR_CMETHOD_REF(br_object, queryBuffer),
    ._queryMany = BR_CMETHOD_REF(br_object, queryMany),
    ._queryManySize = BR_CMETHOD_REF(br_object, queryManySize),
    ._queryAll = BR_CMETHOD_REF(br_object, queryAll),
    ._queryAllSize = BR_CMETHOD_REF(br_object, queryAllSize),

    ._render = BREND_CMETHOD_REF(BREND_CLASS(br_geometry_stored), render),
    ._renderOnScreen = BREND_CMETHOD_REF(BREND_CLASS(br_geometry_stored), render),
};
