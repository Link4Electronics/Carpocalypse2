#include <stdlib.h>
#include "brassert.h"
#include "drv.h"
#include "formats.h"
#include "commonrend.h"
#include "state.h"
#include "video.h"
#include "sbuffer.h"
#include <string.h>
#if defined(__vita__) && defined(BREND_DRIVER_GL)
#include "../../../carma2/platform/sdl3/vita_perf.h"
#define VPB(x) do { if (C2VitaPerfDetail) C2VitaPerfBegin(x); } while (0)
#define VPE(x) do { if (C2VitaPerfDetail) C2VitaPerfEnd(x); } while (0)
#else
#define VPB(x) ((void)0)
#define VPE(x) ((void)0)
#endif
#if defined(BREND_DRIVER_GL)
#include "transparency.h"

/* BRender front faces in GL window space (determined on the desktop port). */
#define C2_GL_FRONT GL_CW
#define C2_GL_FRONT_REVERSED GL_CCW
/* Sign of det(mvp): a negative determinant (mirror view, flipped target) reverses the
 * screen-space winding of every face. */
static int mvp_reverses_winding(const br_matrix4* m) {
    float a = m->m[0][0], b = m->m[0][1], c = m->m[0][2], d = m->m[0][3];
    float e = m->m[1][0], f = m->m[1][1], g = m->m[1][2], h = m->m[1][3];
    float i = m->m[2][0], j = m->m[2][1], k = m->m[2][2], l = m->m[2][3];
    float n = m->m[3][0], o = m->m[3][1], p = m->m[3][2], q = m->m[3][3];
    float det = (a * f - b * e) * (k * q - l * p) - (a * g - c * e) * (j * q - l * o)
        + (a * h - d * e) * (j * p - k * o) + (b * g - c * f) * (i * q - l * n)
        - (b * h - d * f) * (i * p - k * n) + (c * h - d * g) * (i * o - j * n);
    return det < 0.f;
}
#endif
#include "core/v1db/custsupt.h"

/*
 * Per-group model render, shared by the glrend/sdl3gpurend drivers.
 *
 * Both drivers follow the same shape:
 *   1. refresh the per-model matrix/light cache
 *   2. copy the matrices into the shader_data_model
 *   3. resolve the group's material state (stored, else default) into
 *      renderer->state.current
 *   4. clear_colour (the vertex shader adds it to every lit model)
 *   5. apply material state and record the draw (backend-specific)
 *   6. tally scene stats
 *
 * Steps 1-4 and 6 are identical; step 5 differs because GL is immediate mode
 * (glBufferData + glDrawElements) while VK uses ring buffers + descriptors.
 * Each driver's v1model.c provides Stored*ApplyProperties; the ring/dim
 * overlay logic stays VK-only.
 */
#if defined(BREND_DRIVER_GL)
void StoredGLRenderGroup(br_geometry_stored* self, br_renderer* renderer, const gl_groupinfo* groupinfo)
#else
void StoredSDL3GPURENDRenderGroup(br_geometry_stored* self, br_renderer* renderer, sdl3_groupinfo* groupinfo)
#endif
{
    state_cache* cache = &renderer->state.cache;
    br_device_pixelmap* screen = renderer->pixelmap->screen;
    HVIDEO hVideo = &screen->asFront.video;
    br_renderer_state_stored* stored = groupinfo->stored;
    br_renderer_state_stored* default_state = groupinfo->default_state;
    shader_data_model model = {0};

#if defined(__vita__) && defined(BREND_DRIVER_GL)
    C2VitaPerfCount(C2VPC_GROUPS);
#endif
    /* 1. Update the per-model cache (matrices and lights). */
    VPB(C2VP_G_UPDATEMODEL);
    BREND_FN(State, UpdateModel)(cache, &renderer->state.current->matrix);
    VPE(C2VP_G_UPDATEMODEL);

    /* 2. Copy matrices into the model UBO. */
    model.projection_brender = cache->model.p_br;
    model.projection = cache->model.p;
    model.model_view = cache->model.mv;
    model.mvp = cache->model.mvp;
    model.normal_matrix = cache->model.normal;
    model.environment_matrix = cache->model.environment;
    model.eye_m = cache->model.eye_m;

    /* 3. Resolve the group's material state into state.current. The matrix in
     * state.current is left untouched so deferred (order-table) renders keep
     * the model's matrix that was snapshotted at insertion time. */
    { static int dbg; if (dbg++ < 120)
        BrLogPrintf("DBG grp %d cnt=%u stored=%p scm=%p dflt=%p dcm=%p\n",
            dbg, (unsigned)groupinfo->count, (void*)stored,
            stored ? (void*)stored->state.prim.colour_map : NULL,
            (void*)default_state,
            default_state ? (void*)default_state->state.prim.colour_map : NULL); }
    VPB(C2VP_G_STATECOPY);
    if (stored) {
        BREND_FN(State, Copy)(renderer->state.current, &stored->state,
            MASK_STATE_SURFACE | MASK_STATE_PRIMITIVE | MASK_STATE_CULL);
    } else if (default_state != NULL) {
        renderer->state.current->surface = default_state->state.surface;
        renderer->state.current->prim = default_state->state.prim;
        renderer->state.current->cull = default_state->state.cull;
    }
    VPE(C2VP_G_STATECOPY);

    /* 4. The vertex shader adds clear_colour to every lit model when no direct
     * light exists; the game driver leaves it black. */
    BrVector4Set(&model.clear_colour, 0.0f, 0.0f, 0.0f, 0.0f);

#if defined(BREND_DRIVER_GL)
    {
#if DEBUG
        { /* Check that sceneBegin() actually did it's shit. */
            GLint p;
            glGetIntegerv(GL_CURRENT_PROGRAM, &p);
            ASSERT(p == hVideo->brenderProgram.program);

            glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &p);
            ASSERT(p == renderer->state.current->output.colour->asBack.glFbo);

            glGetIntegerv(GL_UNIFORM_BUFFER_BINDING, &p);
            ASSERT(p == hVideo->brenderProgram.uboModel);
        }
#endif

        glBindVertexArray(self->gl_vao);

        GLuint default_tex;
        if (stored) {
            default_tex = screen->asFront.tex_white;
        } else if (groupinfo->default_state != NULL && groupinfo->default_state->state.prim.colour_map) {
            default_tex = groupinfo->default_state->state.prim.colour_map->gl_tex;
        } else {
            default_tex = screen->asFront.tex_white; /* port: renderer->pixelmap is a back buffer, not the screen */
        }

        VPB(C2VP_G_PROPS);
        /* port: also apply the material cull state (BRT_ONE_SIDED culls back faces, as the
         * software renderer did); it was resolved above but never reached GL, so every face
         * was rasterised two-sided. */
        StoredGLApplyProperties(hVideo, renderer->state.current,
            MASK_STATE_PRIMITIVE | MASK_STATE_SURFACE | MASK_STATE_CULL, &model, default_tex);
        glFrontFace(mvp_reverses_winding(&model.mvp) ? C2_GL_FRONT_REVERSED : C2_GL_FRONT);
        VPE(C2VP_G_PROPS);
        { static int skip = -1; /* port debug: C2_GL_SKIPUNTEX=1 skips untextured draws */
            if (skip < 0) { const char* e = getenv("C2_GL_SKIPUNTEX"); skip = e != NULL && e[0] == '1'; }
            if (skip && model.disable_texture) { glBindVertexArray(0); return; }
        }
        /* BrSetScreenZOffset (C2 custsupt): softrend adds gScreenZOffset to every screen z, in units
         * of the 16-bit depth buffer. Car shadows (drawn before the track at offset 2) and other
         * decals depend on it to win the depth test; one GL polygon offset unit is one step of the
         * GL_DEPTH_COMPONENT16 buffer. */
#ifdef __vita__
        /* Vita: the GXM depth bias behind glPolygonOffset works in units of the 32-bit float depth
         * buffer and pulled the car shadows (re-drawn ground faces) far in front of the cars, hiding
         * their wheels and underside. Apply the softrend offset itself: N steps of a 16-bit buffer. */
        model.depth_bias = gScreenZOffset * (2.f / 65535.f);
        glDisable(GL_POLYGON_OFFSET_FILL);
#else
        if (gScreenZOffset != 0.f) {
            glEnable(GL_POLYGON_OFFSET_FILL);
            glPolygonOffset(0.f, gScreenZOffset);
        } else {
            glDisable(GL_POLYGON_OFFSET_FILL);
        }
#endif
        /* A forced foreground draw must remain after the pending alpha surfaces. */
        if (renderer->state.current->surface.force_front) GLFlushTransparent(renderer);
        if (self->gl_primitive != GL_TRIANGLES || !GLQueueTransparent(renderer, groupinfo->group, &model)) {
#ifdef __vita__
            /* Per-draw uniforms are transient, not long-lived GPU geometry. */
            VPB(C2VP_G_UBO);
            glBufferData(GL_UNIFORM_BUFFER, sizeof(model), &model, GL_STREAM_DRAW);
            VPE(C2VP_G_UBO);
#else
            glBufferData(GL_UNIFORM_BUFFER, sizeof(model), &model, GL_STATIC_DRAW);
#endif
            VPB(C2VP_G_DRAW);
            glDrawElements(self->gl_primitive, groupinfo->count, GL_UNSIGNED_SHORT, groupinfo->offset);
            VPE(C2VP_G_DRAW);
        }
        GL_CHECK_ERROR();
    }
#else
    {
        SDL_GPUTexture* texture = NULL;
        SDL_GPUSampler* sampler = NULL;

        StoredSDL3GPURENDApplyProperties(hVideo, renderer->state.current,
            MASK_STATE_PRIMITIVE | MASK_STATE_SURFACE, &model, NULL, &texture, &sampler);

        br_boolean blending_on = (renderer->state.current->prim.flags & PRIMF_BLEND) ||
            (renderer->state.current->prim.colour_map != NULL && renderer->state.current->prim.colour_map->blended);
        /* Match glrend: hardware depth testing is only valid when the scene
         * actually supplied a depth output buffer. BrZbsSceneRenderBegin sets
         * BRT_DEPTH_BUFFER_O to NULL (the Zs path relies on CPU hidden-surface
         * sorting), and its geometry sits at the cleared depth, so testing
         * against the depth attachment would reject every fragment. */
        br_boolean depth_valid = BR_TRUE;
        if (renderer->state.current->valid & BR_STATE_OUTPUT)
            depth_valid = renderer->state.current->output.depth != NULL;
        br_boolean depth_off = !depth_valid ||
            renderer->state.current->surface.force_front || renderer->state.current->surface.force_back;
        br_boolean depth_le = renderer->state.current->prim.depth_test == BRT_LESS_OR_EQUAL;

        SDL_GPUGraphicsPipeline* pipeline = blending_on
            ? (depth_off ? hVideo->brenderBlendPipelineNoDepth
                : (depth_le ? hVideo->brenderBlendPipelineLE : hVideo->brenderBlendPipeline))
            : (depth_off ? hVideo->brenderPipelineNoDepth
                : (depth_le ? hVideo->brenderPipelineLE : hVideo->brenderPipeline));
        if (pipeline != hVideo->lastPipeline) {
            SDL3_BindGPUGraphicsPipeline(hVideo->currentPass, pipeline);
            hVideo->lastPipeline = pipeline;
        }

        /* Small ring models (sub-allocated from the per-frame dynamic rings in
         * build_vbo/build_ibo) are only valid within the frame they were rebuilt:
         * the ring cursors reset in SDL3GPUREND_EnsureRecording, so a ring model that
         * persists across frames references clobbered data. Re-upload the geometry
         * from the v11model into the current frame's ring slot when stale. Models
         * rebuilt this frame already stamped ringEpoch == frameEpoch and skip. */
        if (self->inDynamicRing && self->ringEpoch != hVideo->frameEpoch) {
            SDL3GPUREND_RefreshRingStored(hVideo, self);
        }

        if (self->vbo != hVideo->lastVbo || self->vboOffset != hVideo->lastVboOffset ||
            self->ibo != hVideo->lastIbo || self->iboOffset != hVideo->lastIboOffset) {
            SDL_GPUBufferBinding vb = { self->vbo, (Uint32)self->vboOffset };
            SDL3_BindGPUVertexBuffers(hVideo->currentPass, 0, &vb, 1);
            SDL_GPUBufferBinding ib = { self->ibo, (Uint32)self->iboOffset };
            SDL3_BindGPUIndexBuffer(hVideo->currentPass, &ib, SDL_GPU_INDEXELEMENTSIZE_16BIT);
            hVideo->lastVbo = self->vbo;
            hVideo->lastVboOffset = self->vboOffset;
            hVideo->lastIbo = self->ibo;
            hVideo->lastIboOffset = self->iboOffset;
        }

        if (texture != NULL) {
            if (texture != hVideo->lastTexture || sampler != hVideo->lastSampler) {
                SDL_GPUTextureSamplerBinding tsb = { texture, sampler };
                SDL3_BindGPUFragmentSamplers(hVideo->currentPass, SDL3GPUREND_FRAGMENT_SAMPLER_SLOT, &tsb, 1);
                hVideo->lastTexture = texture;
                hVideo->lastSampler = sampler;
            }
        }

        SDL3GPUREND_PushModel(hVideo, &model, sizeof(model));

        SDL3_DrawGPUIndexedPrimitives(hVideo->currentPass, groupinfo->count, 1,
            groupinfo->offset / sizeof(br_uint_16), 0, 0);

        /* Dim quad detection: a blended black quad, depth off, untextured
         * (game DimRectangle -> DeviouslyDimRectangle). The GPU quad dims the
         * 3D underneath, but any CPU overlay content covering the rect sits on
         * top of it, so the CPU locked buffer must be dealt with here:
         *   - overlay-primary frames (map screen): the buffer holds the flushed
         *     2D map image, which must be dimmed IN PLACE; text drawn into the
         *     buffer after this scene (blips, timer) lands on it bright.
         *   - racing frames: the buffer holds the 2D cockpit, which must be
         *     PURGED so the dim quad dims the 3D scene instead; headup text
         *     drawn after lands on the magenta and stays bright. */
        br_boolean is_dim = (renderer->state.current->surface.colour == 0 &&
            blending_on && depth_off &&
            renderer->state.current->prim.colour_map == NULL);

        /* Consume the main viewport purge armed at the first scene's sceneBegin,
         * now that the frame type is known from what this first scene draws. A
         * dim-quad first model means 2D-primary (the game flushed the map image
         * and the dim quad dims it in place): cancel the purge and keep the
         * overlay-primary classification. Any other model means a racing frame
         * whose pre-scene flush was only the sky/fog fill: purge the rect so the
         * 3D shows through, and drop the classification so the mid-frame HUD dims
         * purge instead of dimming the headup text in place. */
        if (hVideo->pendingMainPurge) {
            hVideo->pendingMainPurge = 0;
            if (!is_dim) {
                hVideo->overlayPrimaryFrame = 0;
                int bpp = (hVideo->pm_type == BR_PMT_RGB_565 || hVideo->pm_type == BR_PMT_RGB_555) ? 2 : 4;
                br_uint_32 magenta = (bpp == 2) ? BR_COLOUR_565(31, 0, 31) : BR_COLOUR_RGB(255, 0, 255);
                SDL3GPUREND_PurgeRect(bpp, magenta, hVideo->lockedPixels,
                    hVideo->pm_width, hVideo->pm_height, hVideo->pm_row_bytes,
                    hVideo->mainViewportX, hVideo->mainViewportY,
                    hVideo->mainViewportW, hVideo->mainViewportH);
            }
        }

        if (is_dim && screen != NULL &&
            screen->pm_type == BR_PMT_RGB_565 && hVideo->lockedPixels != NULL) {
            br_matrix4 combined;
            BrMatrix4Mul(&combined, &cache->model.p, &cache->model.mv);
            br_rectangle aabb;
            if (SDL3GPUREND_ComputeScreenAABB(&combined, groupinfo->group, hVideo, renderer->state.current->output.colour, &aabb)) {
                if (hVideo->overlayPrimaryFrame) {
                    SDL3GPUREND_DimRect(hVideo->lockedPixels,
                        hVideo->pm_width, hVideo->pm_height, hVideo->pm_row_bytes,
                        aabb.x, aabb.y, aabb.w, aabb.h);
                } else {
                    SDL3GPUREND_PurgeRect(2, BR_COLOUR_565(31, 0, 31), hVideo->lockedPixels,
                        hVideo->pm_width, hVideo->pm_height, hVideo->pm_row_bytes,
                        aabb.x, aabb.y, aabb.w, aabb.h);
                }
            }
        }

        br_boolean is_pratcam = (depth_off &&
            renderer->state.current->prim.colour_map != NULL &&
            screen != NULL && screen->pm_type == BR_PMT_RGB_565);
        if (is_pratcam) {
            br_matrix4 combined;
            BrMatrix4Mul(&combined, &cache->model.p, &cache->model.mv);
            br_rectangle aabb;
            if (SDL3GPUREND_ComputeScreenAABB(&combined, groupinfo->group, hVideo, renderer->state.current->output.colour, &aabb)) {
                hVideo->pratcamAreaCount = 1;
                hVideo->pratcamArea = aabb;
            }
        }
    }
#endif

    /* 6. Tally scene stats. */
    renderer->scene_stats.face_group_count++;
#if defined(BREND_DRIVER_GL)
    if (self->gl_primitive == GL_LINES) {
        renderer->scene_stats.vertices_rendered_count += groupinfo->count;
        return;
    }
#endif
    renderer->scene_stats.triangles_rendered_count += groupinfo->group->nfaces;
    renderer->scene_stats.triangles_drawn_count += groupinfo->group->nfaces;
    renderer->scene_stats.vertices_rendered_count += groupinfo->group->nfaces * 3;
}
