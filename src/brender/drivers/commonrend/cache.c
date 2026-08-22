#include "brassert.h"

#include "drv.h"
#include "commonrend.h"

#include "shortcut.h"

#include "vecifns.h"

/*
** Process each light, doing as much once-per-frame work as possible.
*/
static void ProcessSceneLights(state_cache* cache, const state_light* lights) {
    cache->scene.num_lights = 0;
    for (uint32_t i = 0; i < MAX_STATE_LIGHTS; ++i) {
        const state_light* light = lights + i;

        if (light->type == BRT_NONE)
            continue;

        shader_data_light* alp = cache->scene.lights + cache->scene.num_lights;

        /* See enables.c:194, BrSetupLights(). All the lights are already converted into view space. */
        BrVector4Set(&alp->position, light->position.v[0], light->position.v[1], light->position.v[2],
            light->type == BRT_DIRECT ? 0.0f : 1.0f);

        BrVector4Set(&alp->direction, light->direction.v[0], light->direction.v[1], light->direction.v[2], 0.0f);

        float intensity = 16384.0f; /* Effectively infinite */
        if (light->attenuation_c != 0)
            intensity = BR_RCP(light->attenuation_c);

        if (light->type == BRT_DIRECT) {
            BrVector4Copy(&alp->half, &alp->direction);
            alp->half.v[2] += 1.0f;
            BrVector4Normalise(&alp->half, &alp->half);

            BrVector4Scale(&alp->direction, &alp->direction, intensity);
        }

        BrVector4Set(&alp->iclq, intensity, light->attenuation_c, light->attenuation_l, light->attenuation_q);

        BrVector4Set(&alp->colour, BR_RED(light->colour) / 255.0f, BR_GRN(light->colour) / 255.0f,
            BR_BLU(light->colour) / 255.0f, light->type == BRT_AMBIENT ? 1.0f : 0.0f);

        if (light->type == BRT_SPOT) {
            BrVector2Set(&alp->spot_angles, BrAngleToRadian(light->spot_inner), BrAngleToRadian(light->spot_outer));
        } else {
            BrVector2Set(&alp->spot_angles, 0, 0);
        }
        ++alp;
        ++cache->scene.num_lights;
    }
}

static void ProcessClipPlanes(state_cache* cache, const state_clip* clips) {
    cache->scene.num_clip_planes = 0;
    for (uint32_t i = 0; i < MAX_STATE_CLIP_PLANES; i++) {
        const state_clip* clip = &clips[i];
        if (clip->type == BRT_NONE) {
            continue;
        }
        BrVector4Copy(&cache->scene.clip_planes[cache->scene.num_clip_planes], &clip->plane);
        cache->scene.num_clip_planes++;
    }
}

#if defined(BREND_DRIVER_SDL3GPUREND)
/* Vulkan clip space has Z in [0, w] and Y down, so the BRender projection
 * matrix needs a z-column flip (perspective only) and a y-row flip. SDL3 GPU
 * uses the same NDC conventions. */
static void negate_z_column(br_matrix4* m) {
    m->m[0][2] = -m->m[0][2];
    m->m[1][2] = -m->m[1][2];
    m->m[2][2] = -m->m[2][2];
    m->m[3][2] = -m->m[3][2];
}

static void negate_y_row(br_matrix4* m) {
    m->m[1][0] = -m->m[1][0];
    m->m[1][1] = -m->m[1][1];
    m->m[1][2] = -m->m[1][2];
    m->m[1][3] = -m->m[1][3];
    m->m[3][1] = -m->m[3][1];
}
#endif

/*
** Update the per-model matrices.
**
** A good reference of the types is here:
** http://cse.csusb.edu/tongyu/courses/cs520/notes/glsl.php
*/
static void UpdateMatrices(state_cache* cache, state_matrix* matrix) {
    if (matrix->view_to_environment_hint != BRT_DONT_CARE) {
        br_matrix34 tmp;
        BrMatrix34Mul(&tmp, &matrix->model_to_view, &matrix->view_to_environment);
        BrMatrix4Copy34(&cache->model.environment, &tmp);
    }

    /*
     * Projection Matrix
     */
    BrMatrix4Copy(&cache->model.p_br, &matrix->view_to_screen);
    BrMatrix4Copy(&cache->model.p, &matrix->view_to_screen);

#if defined(BREND_DRIVER_GL)
    VIDEOI_D3DtoGLProjection(&cache->model.p);
#elif defined(BREND_DRIVER_SDL3GPUREND)
    if (matrix->view_to_screen_hint == BRT_PERSPECTIVE) {
        negate_z_column(&cache->model.p);
    }
    negate_y_row(&cache->model.p);
#endif

    /*
     * ModelView Matrix
     */
    BrMatrix4Copy34(&cache->model.mv, &matrix->model_to_view);

    /*
     * Inverse of ModelView.
     */
    BrMatrix4Inverse(&cache->model.view_to_model, &cache->model.mv);

    /*
     * MVP Matrix
     */
    BrMatrix4Mul(&cache->model.mvp, &cache->model.mv, &cache->model.p);

    /*
     * Normal Matrix
     */
    BrMatrix4Inverse(&cache->model.normal, &cache->model.mv);
    BrMatrix4Transpose(&cache->model.normal);
}

/*
 * Find centre of projection in model space
 */
static br_vector4 EyeInModel(state_cache* cache, const state_matrix* matrix) {
    br_matrix4 s2m;
    br_vector4 eye_m;

    /*
     * Spot special, easy, cases
     */
    if (matrix->model_to_view_hint == BRT_LENGTH_PRESERVING) {
        if (matrix->view_to_screen_hint == BRT_PERSPECTIVE) {
            eye_m.v[0] = -BR_MAC3(matrix->model_to_view.m[3][0], matrix->model_to_view.m[0][0],
                matrix->model_to_view.m[3][1], matrix->model_to_view.m[0][1],
                matrix->model_to_view.m[3][Z], matrix->model_to_view.m[0][2]);
            eye_m.v[1] = -BR_MAC3(matrix->model_to_view.m[3][0], matrix->model_to_view.m[1][0],
                matrix->model_to_view.m[3][1], matrix->model_to_view.m[1][1],
                matrix->model_to_view.m[3][Z], matrix->model_to_view.m[1][2]);
            eye_m.v[2] = -BR_MAC3(matrix->model_to_view.m[3][0], matrix->model_to_view.m[2][0],
                matrix->model_to_view.m[3][1], matrix->model_to_view.m[2][1],
                matrix->model_to_view.m[3][Z], matrix->model_to_view.m[2][2]);

            eye_m.v[3] = BR_SCALAR(1.0);

            return eye_m;
        }

        if (matrix->view_to_screen_hint == BRT_PARALLEL) {
            BrVector3CopyMat34Col((br_vector3*)&eye_m, &matrix->model_to_view, 2);
            eye_m.v[3] = BR_SCALAR(0.0);
            return eye_m;
        }

    } else {
        if (matrix->view_to_screen_hint == BRT_PERSPECTIVE) {
            BrVector3CopyMat34Row((br_vector3*)&eye_m, &cache->model.view_to_model, 3);
            eye_m.v[3] = BR_SCALAR(1.0);
            return eye_m;
        }

        if (matrix->view_to_screen_hint == BRT_PARALLEL) {
            BrVector3CopyMat34Row((br_vector3*)&eye_m, &cache->model.view_to_model, 2);
            eye_m.v[3] = BR_SCALAR(0.0);
            return eye_m;
        }
    }

    /*
     * If reached here, then we need to invert model_to_screen
     */
    BrMatrix4Inverse(&s2m, &cache->model.mvp);

    eye_m.v[0] = s2m.m[Z][0];
    eye_m.v[1] = s2m.m[Z][1];
    eye_m.v[2] = s2m.m[Z][2];
    eye_m.v[3] = s2m.m[Z][3];
    return eye_m;
}

void BREND_FN(State, UpdateModel)(state_cache* cache, state_matrix* matrix) {
    UpdateMatrices(cache, matrix);

    cache->model.eye_m = EyeInModel(cache, matrix);
}

void BREND_FN(State, UpdateScene)(state_cache* cache, state_stack* state) {
#if defined(BREND_DRIVER_GL)
    ASSERT(state->output.colour);
    cache->fbo = state->output.colour->asBack.glFbo;
#endif

    BrVector4Set(&cache->scene.eye_view, 0.0f, 0.0f, 1.0f, 0.0f);

    ProcessSceneLights(cache, state->light);
    ProcessClipPlanes(cache, state->clip);

    cache->scene.yon_z = state->matrix.yon_z;
}

static void ResetCacheLight(shader_data_light* alp) {
    BrVector4Set(&alp->position, 0, 0, 0, 0);
    BrVector4Set(&alp->direction, 0, 0, 0, 0);
    BrVector4Set(&alp->half, 0, 0, 0, 0);
    BrVector4Set(&alp->colour, 0, 0, 0, 0);
    BrVector4Set(&alp->iclq, 0, 0, 0, 0);
    BrVector2Set(&alp->spot_angles, 0, 0);
}

void BREND_FN(State, Reset)(state_cache* cache) {
    BrMatrix4Identity(&cache->model.p_br);
    BrMatrix4Identity(&cache->model.p);
    BrMatrix4Identity(&cache->model.mv);
    BrMatrix4Identity(&cache->model.mvp);
    BrMatrix4Identity(&cache->model.normal);
    BrMatrix4Identity(&cache->model.environment);
    BrVector4Set(&cache->model.eye_m, 0, 0, 0, 1);

    BrVector4Set(&cache->scene.eye_view, 0, 0, 0, 0);

    for (int i = 0; i < BR_ASIZE(cache->scene.lights); ++i) {
        ResetCacheLight(&cache->scene.lights[i]);
    }

    cache->scene.num_lights = 0;
    cache->scene.num_clip_planes = 0;
}
