/*
 * Support routines for rendering models (SDL3 GPU)
 */
#include "brassert.h"
#include "drv.h"
#include "formats.h"
#include "commonrend.h"
#include "state.h"
#include "video.h"
#include "sbuffer.h"
#include <string.h>

/*
 * Computes the screen-space axis-aligned bounding box of a group, for the
 * overlay compositing (dim areas / pratcam). Mirrors the old VK helper but
 * reads the window size from the SDL3 window rather than a swapchain extent.
 */
br_boolean SDL3GPUREND_ComputeScreenAABB(const br_matrix4* mvp, struct v11group* gp,
    HVIDEO hVideo, br_device_pixelmap* colour_target, br_rectangle* out) {
    float dvp_x = 0, dvp_y = 0, dvp_w, dvp_h;
    if (colour_target != NULL) {
        dvp_x = (float)colour_target->pm_base_x;
        dvp_y = (float)colour_target->pm_base_y;
        dvp_w = (float)colour_target->pm_width;
        dvp_h = (float)colour_target->pm_height;
    } else {
        dvp_w = (float)hVideo->windowWidth;
        dvp_h = (float)hVideo->windowHeight;
    }

    float min_x = 1e30f, max_x = -1e30f, min_y = 1e30f, max_y = -1e30f;
    for (br_uint_16 v = 0; v < gp->nvertices; v++) {
        br_vector3_f* pos = (br_vector3_f*)&gp->vertices[v].p; /* our core: array-of-structs fmt_vertex */
        float cx = mvp->m[0][0] * pos->v[0] + mvp->m[1][0] * pos->v[1] + mvp->m[2][0] * pos->v[2] + mvp->m[3][0];
        float cy = mvp->m[0][1] * pos->v[0] + mvp->m[1][1] * pos->v[1] + mvp->m[2][1] * pos->v[2] + mvp->m[3][1];
        float cw = mvp->m[0][3] * pos->v[0] + mvp->m[1][3] * pos->v[1] + mvp->m[2][3] * pos->v[2] + mvp->m[3][3];
        float nx = cx / cw, ny = cy / cw;
        float sx = (nx + 1.0f) * 0.5f * dvp_w + dvp_x;
        float sy = (ny + 1.0f) * 0.5f * dvp_h + dvp_y;
        if (sx < min_x) min_x = sx;
        if (sx > max_x) max_x = sx;
        if (sy < min_y) min_y = sy;
        if (sy > max_y) max_y = sy;
    }

    out->x = (int)min_x;
    out->y = (int)min_y;
    out->w = (int)(max_x - min_x + 0.5f);
    out->h = (int)(max_y - min_y + 0.5f);
    return BR_TRUE;
}

/*
 * Applies the current material state into `model` (the shader UBO payload) and
 * fills `texture`/`sampler` with the SDL3 GPU texture/sampler to bind on the
 * fragment sampler slot used by the draw. Called per group from modelrender.c.
 */
void StoredSDL3GPURENDApplyProperties(HVIDEO hVideo, state_stack* state, uint32_t states,
    shader_data_model* model, br_buffer_stored* default_texture,
    SDL_GPUTexture** texture, SDL_GPUSampler** sampler) {
    (void)default_texture;

    br_buffer_stored* colour_map;
    br_boolean filter_linear;
    br_boolean palette_dirty;
    const br_uint_32* palette_entries;

    /* Only use the states we want (if valid). */
    states = state->valid & states;

    BREND_FN(State, FillModel)(state, states, model);
    BREND_FN(State, FillModelTexture)(state, states, model, &colour_map, &filter_linear, &palette_dirty, &palette_entries);
    (void)palette_dirty;
    (void)palette_entries;

    if (colour_map) {
        BufferStoredSDL3GPURENDReupload(colour_map);
        if (colour_map->image != NULL && colour_map->sampler != NULL) {
            *texture = colour_map->image;
            *sampler = filter_linear ? hVideo->samplerLinear : hVideo->samplerNearest;
        } else {
            model->disable_texture = 1;
            model->disable_colour_key = 1;

            *texture = hVideo->defaultTexture;
            *sampler = hVideo->samplerLinear;
        }
    } else {
        *texture = hVideo->defaultTexture;
        *sampler = hVideo->samplerLinear;
    }
}
