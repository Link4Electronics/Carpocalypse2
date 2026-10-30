#ifndef _GSTORED_H_
#define _GSTORED_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>

/* Byte-identical vertex layout to glrend/sdl3gpurend: brender.vert (shared GLSL)
 * consumes it, so the CPU-side struct must match on every backend. */
typedef struct sdl3_vertex_f {
    br_vector3_f p;
    br_vector2_f map;
    br_vector3_f n;
    br_vector4_f c;
} sdl3_vertex_f;

typedef struct sdl3_groupinfo {
    uint32_t count;
    uint32_t offset;
    struct v11group* group;
    br_renderer_state_stored* stored;
    br_renderer_state_stored* default_state;
} sdl3_groupinfo;

#ifdef BR_GEOMETRY_STORED_PRIVATE

#include "gstored_base.h"

typedef struct br_geometry_stored {
    BR_GEOMETRY_STORED_BASE;

    SDL_GPUBuffer* vbo;
    SDL_GPUBuffer* ibo;

    /* Non-zero when vbo/ibo are sub-allocations of the shared dynamic rings
     * (see VIDEO.dynVbo/dynIbo). The ring is owned by the video context, so
     * the free path must NOT destroy them, and the render path must bind with
     * vboOffset/iboOffset instead of offset 0. */
    int inDynamicRing;
    size_t vboOffset;
    size_t iboOffset;
    /* Frame epoch when this stored's ring sub-allocations were last written.
     * The ring cursor resets every frame, so a ring model whose ringEpoch no
     * longer matches VIDEO.frameEpoch references clobbered data and must be
     * re-uploaded (see SDL3GPUREND_RefreshRingStored). */
    uint32_t ringEpoch;

    int num_groups;

    SDL_GPUDevice* deviceHandle;
    struct _VIDEO* hVideo;

    sdl3_groupinfo* sdl3_groups;
} br_geometry_stored;

#endif

#ifdef __cplusplus
};
#endif
#endif /* _GSTORED_H_ */
