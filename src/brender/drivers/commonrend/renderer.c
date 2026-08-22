/*
 * Renderer methods
 *
 * Shared by the glrend/sdl3gpurend drivers. All methods are identical between
 * the two backends except sceneBegin/sceneEnd (backend-specific viewport/
 * program/UBO/render-pass setup), the frameBegin clear, partSet/partSetMany
 * (GL applies template actions), and the per-backend allocate functions.
 */
#include "brassert.h"
#include "drv.h"
#include "commonrend.h"
#include <string.h>
#include <math.h>

/*
 * Default dispatch table for renderer (defined at end of file)
 */
static const struct br_renderer_dispatch rendererDispatch;

#define F(f) offsetof(struct br_renderer, f)

static struct br_tv_template_entry rendererTemplateEntries[] = {
    { BRT(IDENTIFIER_CSTR), F(identifier), BRTV_QUERY | BRTV_ALL, BRTV_CONV_COPY },
    { BRT(FACE_GROUP_COUNT_U32), F(scene_stats.face_group_count), BRTV_QUERY | BRTV_ALL, BRTV_CONV_COPY },
    { BRT(TRIANGLES_DRAWN_COUNT_U32), F(scene_stats.triangles_drawn_count), BRTV_QUERY | BRTV_ALL, BRTV_CONV_COPY },
    { BRT(TRIANGLES_RENDERED_COUNT_U32), F(scene_stats.triangles_rendered_count), BRTV_QUERY | BRTV_ALL, BRTV_CONV_COPY },
    { BRT(VERTICES_RENDERED_COUNT_U32), F(scene_stats.vertices_rendered_count), BRTV_QUERY | BRTV_ALL, BRTV_CONV_COPY },
};
#undef F

/*
 * Create a new renderer
 */
br_renderer* BREND_FN(Renderer, Allocate)(br_device* device, br_renderer_facility* facility, br_device_pixelmap* dest) {
    br_renderer* self;

    /*
     * Check that destination is valid
     */
    if (dest == NULL || ObjectDevice(dest) != device)
        return NULL;

    self = BrResAllocate(facility, sizeof(*self), BR_MEMORY_OBJECT);
    self->dispatch = &rendererDispatch;
    self->identifier = facility->identifier;
    self->device = device;
    self->object_list = BrObjectListAllocate(self);
    self->pixelmap = dest;
    self->renderer_facility = facility;
    self->state_pool = BrPoolAllocate(sizeof(state_stack), 1024, BR_MEMORY_OBJECT_DATA);

    ObjectContainerAddFront(facility, (br_object*)self);

    BREND_FN(State, Init)(&self->state, self->device);

    /*
     * State starts out as default
     */
    RendererStateDefault(self, (br_uint_32)BR_STATE_ALL);

    self->has_begun = 0;
    dest->renderer = self;
    return (br_renderer*)self;
}

/*
 * Mirror a colour target's base_y about its containing surface's vertical
 * extent, since both backends draw the frame inverted vs game coordinates.
 * Full-screen targets (pm_base_y == 0) are unaffected; the non-sub path is
 * parity-only.
 */
static br_uint_16 RendererFlipBaseY(br_device_pixelmap* colour_target, br_uint_16 screen_height) {
    if (colour_target->pm_base_y == 0)
        return 0;
    if (colour_target->sub_pixelmap)
        return colour_target->parent_height - colour_target->pm_height - colour_target->pm_base_y;
    return screen_height - colour_target->pm_height - colour_target->pm_base_y;
}

static void BREND_CMETHOD_DECL(BREND_CLASS(br_renderer), sceneBegin)(br_renderer* self) {
    br_device_pixelmap* screen = self->pixelmap->screen;
    HVIDEO hVideo = &screen->asFront.video;
    br_device_pixelmap *colour_target = NULL, *depth_target = NULL;

    self->scene_stats.face_group_count = 0;
    self->scene_stats.triangles_drawn_count = 0;
    self->scene_stats.triangles_rendered_count = 0;
    self->scene_stats.vertices_rendered_count = 0;

    /* First draw call, so do all the per-scene crap */
    if (self->state.current->valid & BR_STATE_OUTPUT) {
        colour_target = self->state.current->output.colour;
        depth_target = self->state.current->output.depth;
    }

    if (colour_target != NULL && ObjectDevice(colour_target) != self->device) {
        BR_ERROR0("Can't render to a non-device colour pixelmap");
    }

    if (depth_target != NULL && ObjectDevice(depth_target) != self->device) {
        BR_ERROR0("Can't render to a non-device depth pixelmap");
    }

    /*
     * TODO: Consider if we want to handle depth-only renders.
     */
    if (colour_target == NULL) {
        BR_ERROR("Can't render without a destination");
    }

    BREND_FN(State, Reset)(&self->state.cache);
    BREND_FN(State, UpdateScene)(&self->state.cache, self->state.current);

#if defined(BREND_DRIVER_GL)
    {
        extern int g_wireframe_mode;
        br_uint_16 base_y = 0;
        int x, y;
        float rx, ry;

        glPolygonMode(GL_FRONT_AND_BACK, g_wireframe_mode ? GL_LINE : GL_FILL);

        glUseProgram(hVideo->brenderProgram.program);
        glBindBufferBase(GL_UNIFORM_BUFFER, hVideo->brenderProgram.blockBindingScene, hVideo->brenderProgram.uboScene);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(self->state.cache.scene), &self->state.cache.scene);

        /* OpenGL upside downness: mirror sub-area base_y (shared with SDL3). */
        base_y = RendererFlipBaseY(colour_target, screen->pm_height);

        BREND_FN(DevicePixelmap, GetViewport)(colour_target->screen, &x, &y, &rx, &ry);
        glViewport(colour_target->pm_base_x * rx + x, base_y * ry + y, colour_target->pm_width * rx, colour_target->pm_height * ry);

        /* Bind the model UBO here, it's faster than doing it for each model group */
        glBindBufferBase(GL_UNIFORM_BUFFER, hVideo->brenderProgram.blockBindingModel, hVideo->brenderProgram.uboModel);
        glUniform1i(hVideo->brenderProgram.uniforms.main_texture, hVideo->brenderProgram.mainTextureBinding);

        if (self->pixelmap->msaa_samples)
            glEnable(GL_MULTISAMPLE);

        GL_CHECK_ERROR();
    }
#else
    {
        /* Whether this is the FIRST scene of the frame. renderingStarted is 0 at
         * frame start (reset by SDL3GPUREND_EnsureRecording) and set to 1 by the first
         * sceneBegin. The mainViewport purge must only run for that first scene:
         * the dim quads (DimRectangle -> BrZbSceneRender) call sceneBegin
         * mid-frame with a full-screen target, and purging there would erase the
         * post-scene 2D (headup text, damage meter, cockpit dashboard) already
         * written into lockedPixels. */
        int firstScene = !hVideo->renderingStarted;

        /* Capture whether the CPU overlay was flushed before this scene began,
         * ONCE PER FRAME (firstScene only): the mid-frame cockpit flush before
         * the pratcam also sets overlayDirty, but re-capturing then would
         * misclassify the racing HUD dims as overlay-primary and dim the yellow
         * headup text. This capture is PROVISIONAL — a pre-scene flush is either
         * a 2D-primary map image (kept) or a racing sky/fog fill (purged), and
         * they are told apart at the first model draw of this scene. */
        if (firstScene) {
            hVideo->overlayPrimaryFrame = hVideo->overlayDirty;
        } else {
            /* A scene after the first must never run the armed purge: if the
             * first scene drew no models the flag would otherwise fire here. */
            hVideo->pendingMainPurge = 0;
        }

        if (hVideo->sceneCount == 0) {
            if (!hVideo->isRecording) {
                SDL3GPUREND_EnsureRecording(hVideo);
            }
            hVideo->renderingStarted = 1;

            if (colour_target != NULL &&
                colour_target->pm_width >= screen->pm_width &&
                colour_target->pm_height >= screen->pm_height)
                hVideo->primaryColourTarget = colour_target;

            if (!hVideo->renderPassActive) {
                SDL3GPUREND_BeginRenderPass(hVideo);
                hVideo->renderPassActive = 1;
            }
        }

        hVideo->sceneCount++;

        SDL3GPUREND_UpdateScene(hVideo, &self->state.cache.scene, sizeof(self->state.cache.scene));
        SDL3GPUREND_SceneBegin(hVideo);

        {
            int x = 0, y = 0;
            float rx = 1.0f, ry = 1.0f;
            float vp_x, vp_y, vp_w, vp_h;
            int win_w = hVideo->windowWidth, win_h = hVideo->windowHeight;

            if (colour_target != NULL) {
                /* Map the game screen into the window: plain 4:3 letterbox, or
                 * the glrend-style full-window stretch when the harness reports
                 * a widescreen viewport override (see
                 * SDL3GPUREND_ViewportTransform). */
                SDL3GPUREND_ViewportTransform(hVideo, win_w, win_h,
                    screen->pm_width, screen->pm_height,
                    &x, &y, &rx, &ry);
                /* The present blit flips the transfer vertically, so sub-area
                 * scenes render into mirrored rows (RendererFlipBaseY); the
                 * CPU-side purge (sceneEnd) stays in game coordinates. */
                int32_t vp_base_y = RendererFlipBaseY(colour_target, screen->pm_height);
                vp_x = (float)colour_target->pm_base_x * rx + (float)x;
                vp_y = (float)vp_base_y * ry + (float)y;
                vp_w = (float)colour_target->pm_width * rx;
                vp_h = (float)colour_target->pm_height * ry;
            } else {
                vp_x = 0;
                vp_y = 0;
                vp_w = (float)win_w;
                vp_h = (float)win_h;
            }

            SDL_GPUViewport gpu_viewport = {vp_x, vp_y, vp_w, vp_h, 0.0f, 1.0f};
            SDL3_SetGPUViewport(hVideo->currentPass, &gpu_viewport);
            int32_t sc_x = (int32_t)floorf(vp_x);
            int32_t sc_y = (int32_t)floorf(vp_y);
            SDL_Rect scissor = {sc_x, sc_y,
                (int)((int32_t)ceilf(vp_x + vp_w) - sc_x),
                (int)((int32_t)ceilf(vp_y + vp_h) - sc_y)};
            SDL3_SetGPUScissor(hVideo->currentPass, &scissor);

            hVideo->viewportX = (int)vp_x;
            hVideo->viewportY = (int)vp_y;
            hVideo->viewportW = (int)vp_w;
            hVideo->viewportH = (int)vp_h;

            /* Sub-area scenes (rear-view mirror, wreck summary, 3D PIP) draw
             * CPU pre-scene content (grey fills, grid lines) into lockedPixels
             * that must persist UNDER the 3D. Snapshot that content into a
             * background texture and draw it here, before the 3D, so it shows
             * through where the 3D does not cover the rect. The sceneEnd purge
             * erases the same content from the composite so it does not also
             * appear on top of the 3D. Also skipped when the CPU overlay was
             * just flushed (overlayDirty)
             * by a MID-FRAME flush: the rear-view mirror flush uploads the
             * cockpit, and re-drawing it as the mirror's background would put it
             * under the 3D inside the mirror window. This must read the per-scene
             * flag, not overlayPrimaryFrame (once per frame), because the mirror
             * flush happens mid-frame. The FIRST scene of the frame must still
             * draw its background even if a flush preceded it: the wreck-gallery
             * draw proc flushes the grey grid immediately before its (only)
             * scene, and that grid must persist UNDER the 3D. */
            if (colour_target != NULL &&
                (colour_target->pm_width < screen->pm_width ||
                 colour_target->pm_height < screen->pm_height) &&
                !(hVideo->overlayDirty && !firstScene)) {
                SDL3GPUREND_DrawSceneBackground(hVideo,
                    colour_target->pm_base_x, colour_target->pm_base_y,
                    colour_target->pm_width, colour_target->pm_height);
            }

            /* Full-screen FIRST scene with a pre-scene flush: the flushed CPU
             * content is the racing sky/fog fill, and the 3D does NOT cover the
             * far-culling distance, so without a background the transfer's black
             * clear shows there (glrend shows the fill colour). Snapshot the fill
             * into a background texture and draw it under the 3D, confined to the
             * scene viewport/scissor (the letterbox bars keep the black clear).
             * The first-model purge (modelrender.c) erases the same content from
             * the composite so it doesn't paint over the 3D. On 2D-primary frames
             * (dim quad first scene) the flushed map image is dimmed in place and
             * re-drawn by the composite on top, so this background is covered and
             * harmless. Gated on firstScene (mid-frame dim-quad sceneBegins must
             * not re-draw pre-scene content under the 3D) and overlayDirty (a
             * flush actually preceded this scene; otherwise the buffer is stale). */
            if (colour_target != NULL &&
                colour_target->pm_width >= screen->pm_width &&
                colour_target->pm_height >= screen->pm_height &&
                firstScene && hVideo->overlayDirty) {
                SDL3GPUREND_DrawSceneBackground(hVideo,
                    colour_target->pm_base_x, colour_target->pm_base_y,
                    colour_target->pm_width, colour_target->pm_height);
            }

            if (colour_target != NULL &&
                colour_target->pm_width >= screen->pm_width &&
                colour_target->pm_height >= screen->pm_height) {
                hVideo->mainViewportX = (int)((vp_x - (float)x) / rx);
                hVideo->mainViewportY = (int)((vp_y - (float)y) / ry);
                hVideo->mainViewportW = (int)(vp_w / rx);
                hVideo->mainViewportH = (int)(vp_h / ry);

                /* ARM the main scene's rect purge from the CPU locked buffer, to
                 * run at the first model draw of this first scene (see
                 * modelrender.c). Pre-scene 2D inside the scene rect (the fog/sky
                 * fill when the sky texture is off) is erased so the 3D scene
                 * shows through; post-scene 2D drawn into the same rect lands on
                 * the magenta and survives to the composite. The purge must NOT
                 * run unconditionally here: on a 2D-primary frame the pre-scene
                 * content is the flushed map image and the first scene is a dim
                 * quad that dims it in place. Gated on firstScene so the mid-frame
                 * dim-quad sceneBegins (which reuse a full-screen target) don't
                 * wipe that post-scene content. The old flush-time purge relied
                 * on the previous frame's mainViewport, so it fired on 2D-only
                 * frames (ESC pause menu) and erased the whole menu. */
                if (firstScene && hVideo->lockedPixels != NULL && hVideo->mainViewportW > 0 && hVideo->mainViewportH > 0) {
                    hVideo->pendingMainPurge = 1;
                }
            }
        }
    }
#endif

    self->has_begun = 1;
}

static void BREND_CMETHOD_DECL(BREND_CLASS(br_renderer), sceneEnd)(br_renderer* self) {
#if defined(BREND_DRIVER_GL)
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindVertexArray(0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glUseProgram(0);
    self->has_begun = 0;
    GL_CHECK_ERROR();
#else
    {
        br_device_pixelmap* screen = self->pixelmap->screen;
        HVIDEO hVideo = &screen->asFront.video;

        if (hVideo->sceneCount > 0) {
            hVideo->sceneCount--;
        }

        if (hVideo->clearAreaCount < 4 && (self->state.current->valid & BR_STATE_OUTPUT)) {
            br_device_pixelmap* colour_target = self->state.current->output.colour;
            if (colour_target != NULL) {
                int is_sub_area = (colour_target->pm_width < screen->pm_width ||
                                   colour_target->pm_height < screen->pm_height);
                int is_primary = (colour_target == hVideo->primaryColourTarget);
                if (is_sub_area && !is_primary) {
                    int idx = hVideo->clearAreaCount++;
                    hVideo->clearAreas[idx].x = colour_target->pm_base_x;
                    hVideo->clearAreas[idx].y = colour_target->pm_base_y;
                    hVideo->clearAreas[idx].w = colour_target->pm_width;
                    hVideo->clearAreas[idx].h = colour_target->pm_height;
                    hVideo->overlayDirty = 1;

                    // Purge the scene's render rect from the CPU locked buffer NOW,
                    // so 2D content drawn AFTER the scene (cockpit dashboard surround,
                    // map blips, race HUD) lands on the magenta and survives to the
                    // overlay composite. Purging only at the next flush would also
                    // erase that post-scene content, since it is written into the
                    // same rect before the flush runs.
                    if (hVideo->lockedPixels != NULL && hVideo->pm_width > 0) {
                        int bpp = (hVideo->pm_type == BR_PMT_RGB_565 || hVideo->pm_type == BR_PMT_RGB_555) ? 2 : 4;
                        br_uint_32 magenta = (bpp == 2) ? BR_COLOUR_565(31, 0, 31) : BR_COLOUR_RGB(255, 0, 255);
                        SDL3GPUREND_PurgeRect(bpp, magenta, hVideo->lockedPixels,
                            hVideo->pm_width, hVideo->pm_height, hVideo->pm_row_bytes,
                            hVideo->clearAreas[idx].x, hVideo->clearAreas[idx].y,
                            hVideo->clearAreas[idx].w, hVideo->clearAreas[idx].h);
                        // Already purged here, so the flush must not re-purge the same
                        // rect (which would erase post-scene 2D drawn into it).
                        hVideo->clearAreaCount = 0;
                    }
                }
            }
        }

        self->has_begun = 0;
    }
#endif
}

static void BREND_CMETHOD_DECL(BREND_CLASS(br_renderer), free)(br_object* _self) {
    br_renderer* self = (br_renderer*)_self;

    BrPoolFree(self->state_pool);

    ObjectContainerRemove(self->renderer_facility, (br_object*)self);

    BrObjectContainerFree((br_object_container*)self, BR_NULL_TOKEN, NULL, NULL);

    BrResFreeNoCallback(self);
}

static char* BREND_CMETHOD_DECL(BREND_CLASS(br_renderer), identifier)(br_object* self) {
    return (char*)((br_renderer*)self)->identifier;
}

static br_token BREND_CMETHOD_DECL(BREND_CLASS(br_renderer), type)(br_object* self) {
    return BRT_RENDERER;
}

static br_boolean BREND_CMETHOD_DECL(BREND_CLASS(br_renderer), isType)(br_object* self, br_token t) {
    return (t == BRT_RENDERER) || (t == BRT_OBJECT);
}

static struct br_device* BREND_CMETHOD_DECL(BREND_CLASS(br_renderer), device)(br_object* self) {
    return ((br_renderer*)self)->device;
}

static int BREND_CMETHOD_DECL(BREND_CLASS(br_renderer), space)(br_object* self) {
    return sizeof(br_renderer);
}

static struct br_tv_template* BREND_CMETHOD_DECL(BREND_CLASS(br_renderer), templateQuery)(br_object* _self) {
    br_renderer* self = (br_renderer*)_self;

    if (self->device->templates.rendererTemplate == NULL) {
        self->device->templates.rendererTemplate = BrTVTemplateAllocate(self->device, rendererTemplateEntries,
            BR_ASIZE(rendererTemplateEntries));
    }

    return self->device->templates.rendererTemplate;
}

static void* BREND_CMETHOD_DECL(BREND_CLASS(br_renderer), listQuery)(br_object_container* self) {
    return ((br_renderer*)self)->object_list;
}

static br_error BREND_CMETHOD_DECL(BREND_CLASS(br_renderer), validDestination)(br_renderer* self, br_boolean* bp, br_object* h) {
    return BRE_OK;
}

static br_error BREND_CMETHOD_DECL(BREND_CLASS(br_renderer), stateStoredNew)(br_renderer* self, br_renderer_state_stored** pss,
    br_uint_32 mask, br_token_value* tv) {
    br_renderer_state_stored* ss;

    if ((ss = BREND_FN(RendererStateStored, Allocate)(self, self->state.current, mask, tv)) == NULL)
        return BRE_FAIL;

    *pss = ss;
    return BRE_OK;
}

static br_error BREND_CMETHOD_DECL(BREND_CLASS(br_renderer), stateStoredAvail)(br_renderer* self, br_int_32* psize, br_uint_32 mask,
    br_token_value* tv) {
    return BRE_FAIL;
}

static br_error BREND_CMETHOD_DECL(BREND_CLASS(br_renderer), bufferStoredNew)(br_renderer* self, br_buffer_stored** psm, br_token use,
    br_device_pixelmap* pm, br_token_value* tv) {
    br_buffer_stored* sm;

#if defined(BREND_DRIVER_GL)
    if ((sm = BufferStoredGLAllocate(self, use, pm, tv)) == NULL)
#else
    if ((sm = BufferStoredSDL3GPURENDAllocate(self, use, pm, tv)) == NULL)
#endif
        return BRE_FAIL;

    *psm = sm;
    return BRE_OK;
}

static br_error BREND_CMETHOD_DECL(BREND_CLASS(br_renderer), bufferStoredAvail)(br_renderer* self, br_int_32* space, br_token use,
    br_token_value* tv) {
    /*
     * Should return free VRAM
     */
    return BRE_FAIL;
}

static br_error BREND_CMETHOD_DECL(BREND_CLASS(br_renderer), partSet)(br_renderer* self, br_token part, br_int_32 index, br_token t, br_value value) {
    br_error r;
    br_uint_32 m;
    struct br_tv_template* tp;

    if ((tp = BREND_FN(State, GetStateTemplate)(&self->state, part, index)) == NULL)
        return BRE_FAIL;

    m = 0;
    r = BrTokenValueSet(self->state.current, &m, t, value, tp);
#if defined(BREND_DRIVER_GL)
    if (m)
        StateGLTemplateActions(&self->state, m);
#endif

    return r;
}

static br_error BREND_CMETHOD_DECL(BREND_CLASS(br_renderer), partSetMany)(br_renderer* self, br_token part, br_int_32 index,
    br_token_value* tv, br_int_32* pcount) {
    br_error r;
    br_uint_32 m;
    struct br_tv_template* tp;

    if ((tp = BREND_FN(State, GetStateTemplate)(&self->state, part, index)) == NULL)
        return BRE_FAIL;

    m = 0;
    r = BrTokenValueSetMany(self->state.current, pcount, &m, tv, tp);
#if defined(BREND_DRIVER_GL)
    if (m)
        StateGLTemplateActions(&self->state, m);
#endif

    return r;
}

/*
 * Reading current state
 */
static br_error BREND_CMETHOD_DECL(BREND_CLASS(br_renderer), partQuery)(br_renderer* self, br_token part, br_int_32 index,
    void* pvalue, br_token t) {
    struct br_tv_template* tp;

    if ((tp = BREND_FN(State, GetStateTemplate)(&self->state, part, index)) == NULL)
        return BRE_FAIL;

    return BrTokenValueQuery(pvalue, NULL, 0, t, self->state.current, tp);
}

static br_error BREND_CMETHOD_DECL(BREND_CLASS(br_renderer), partQueryBuffer)(br_renderer* self, br_token part, br_int_32 index,
    void* pvalue, void* buffer, br_size_t buffer_size, br_token t) {
    struct br_tv_template* tp;

    if ((tp = BREND_FN(State, GetStateTemplate)(&self->state, part, index)) == NULL)
        return BRE_FAIL;

    return BrTokenValueQuery(pvalue, buffer, buffer_size, t, self->state.current, tp);
}

static br_error BREND_CMETHOD_DECL(BREND_CLASS(br_renderer), partQueryMany)(br_renderer* self, br_token part, br_int_32 index,
    br_token_value* tv, void* extra, br_size_t extra_size,
    br_int_32* pcount) {
    struct br_tv_template* tp;

    if ((tp = BREND_FN(State, GetStateTemplate)(&self->state, part, index)) == NULL)
        return BRE_FAIL;

    return BrTokenValueQueryMany(tv, extra, extra_size, pcount, self->state.current, tp);
}

static br_error BREND_CMETHOD_DECL(BREND_CLASS(br_renderer), partQueryManySize)(br_renderer* self, br_token part, br_int_32 index,
    br_size_t* pextra_size, br_token_value* tv) {
    struct br_tv_template* tp;

    if ((tp = BREND_FN(State, GetStateTemplate)(&self->state, part, index)) == NULL)
        return BRE_FAIL;

    return BrTokenValueQueryManySize(pextra_size, tv, self->state.current, tp);
}

static br_error BREND_CMETHOD_DECL(BREND_CLASS(br_renderer), partQueryAll)(br_renderer* self, br_token part, br_int_32 index,
    br_token_value* buffer, br_size_t buffer_size) {
    struct br_tv_template* tp;

    if ((tp = BREND_FN(State, GetStateTemplate)(&self->state, part, index)) == NULL)
        return BRE_FAIL;

    return BrTokenValueQueryAll(buffer, buffer_size, self->state.current, tp);
}

static br_error BREND_CMETHOD_DECL(BREND_CLASS(br_renderer), partQueryAllSize)(br_renderer* self, br_token part, br_int_32 index,
    br_size_t* psize) {
    struct br_tv_template* tp;

    if ((tp = BREND_FN(State, GetStateTemplate)(&self->state, part, index)) == NULL)
        return BRE_FAIL;

    return BrTokenValueQueryAllSize(psize, self->state.current, tp);
}

static br_error BREND_CMETHOD_DECL(BREND_CLASS(br_renderer), partIndexQuery)(br_renderer* self, br_token part, br_int_32* pnindex) {
    (void)self;

    if (pnindex == NULL)
        return BRE_FAIL;

    switch (part) {
    /* Renderer states. */
    case BRT_CULL:
    case BRT_SURFACE:
    case BRT_MATRIX:
    case BRT_ENABLE:
    case BRT_BOUNDS:
    case BRT_HIDDEN_SURFACE:
        *pnindex = 1;
        return BRE_OK;

    case BRT_LIGHT:
        *pnindex = MAX_STATE_LIGHTS;
        return BRE_OK;

    case BRT_CLIP:
        *pnindex = MAX_STATE_CLIP_PLANES;
        return BRE_OK;

    /* Primitive states. */
    case BRT_OUTPUT:
    case BRT_PRIMITIVE:
        *pnindex = 1;
        return BRE_OK;

    default:
        break;
    }

    return BRE_FAIL;
}

static br_error BREND_CMETHOD_DECL(BREND_CLASS(br_renderer), commandModeSet)(br_renderer* self, br_token mode) {
    return BRE_FAIL;
}

static br_error BREND_CMETHOD_DECL(BREND_CLASS(br_renderer), commandModeQuery)(br_renderer* self, br_token* mode) {
    return BRE_FAIL;
}

static br_error BREND_CMETHOD_DECL(BREND_CLASS(br_renderer), commandModeDefault)(br_renderer* self) {
    return BRE_FAIL;
}

static br_error BREND_CMETHOD_DECL(BREND_CLASS(br_renderer), commandModePush)(br_renderer* self) {
    return BRE_FAIL;
}

static br_error BREND_CMETHOD_DECL(BREND_CLASS(br_renderer), commandModePop)(br_renderer* self) {
    return BRE_FAIL;
}

static br_error BREND_CMETHOD_DECL(BREND_CLASS(br_renderer), modelMul)(br_renderer* self, br_matrix34_f* m) {
    br_matrix34 om = self->state.current->matrix.model_to_view;

    BrMatrix34Mul(&self->state.current->matrix.model_to_view, (br_matrix34*)m, &om);

    self->state.current->matrix.model_to_view_hint = BRT_NONE;

    return BRE_OK;
}

static br_error BREND_CMETHOD_DECL(BREND_CLASS(br_renderer), modelPopPushMul)(br_renderer* self, br_matrix34_f* m) {
    if (self->state.top == 0)
        return BRE_UNDERFLOW;

    BrMatrix34Mul(&self->state.current->matrix.model_to_view, (br_matrix34*)m, &self->state.stack[0].matrix.model_to_view);

    self->state.current->matrix.model_to_view_hint = BRT_NONE;

    return BRE_OK;
}

static br_error BREND_CMETHOD_DECL(BREND_CLASS(br_renderer), modelInvert)(br_renderer* self) {
    br_matrix34 old;

    BrMatrix34Copy(&old, &self->state.current->matrix.model_to_view);

    if (self->state.current->matrix.model_to_view_hint == BRT_LENGTH_PRESERVING)
        BrMatrix34LPInverse(&self->state.current->matrix.model_to_view, &old);
    else
        BrMatrix34Inverse(&self->state.current->matrix.model_to_view, &old);

    return BRE_OK;
}

static br_error BREND_CMETHOD_DECL(BREND_CLASS(br_renderer), statePush)(br_renderer* self, br_uint_32 mask) {
    return BREND_FN(State, Push)(&self->state, mask) ? BRE_OK : BRE_OVERFLOW;
}

static br_error BREND_CMETHOD_DECL(BREND_CLASS(br_renderer), statePop)(br_renderer* self, br_uint_32 mask) {
    return BREND_FN(State, Pop)(&self->state, mask) ? BRE_OK : BRE_OVERFLOW;
}

static br_error BREND_CMETHOD_DECL(BREND_CLASS(br_renderer), stateSave)(br_renderer* self, br_renderer_state_stored* save, br_uint_32 mask) {
    BREND_FN(State, Copy)(&save->state, self->state.current, mask);
    return BRE_OK;
}

static br_error BREND_CMETHOD_DECL(BREND_CLASS(br_renderer), stateRestore)(br_renderer* self, br_renderer_state_stored* save, br_uint_32 mask) {
    BREND_FN(State, Copy)(self->state.current, &save->state, mask);
    return BRE_OK;
}

static br_error BREND_CMETHOD_DECL(BREND_CLASS(br_renderer), stateDefault)(br_renderer* self, br_uint_32 mask) {
    BREND_FN(State, Default)(&self->state, mask);
    return BRE_OK;
}

static br_error BREND_CMETHOD_DECL(BREND_CLASS(br_renderer), stateMask)(br_renderer* self, br_uint_32* mask, br_token* parts, int n_parts) {
    br_uint_32 m;

    (void)self;

    if (mask == NULL)
        return BRE_FAIL;

    m = 0;
    for (int i = 0; i < n_parts; i++) {
        switch (parts[i]) {
        case BRT_SURFACE:
            m |= MASK_STATE_SURFACE;
            break;

        case BRT_MATRIX:
            m |= MASK_STATE_MATRIX;
            break;

        case BRT_ENABLE:
            m |= MASK_STATE_ENABLE;
            break;

        case BRT_LIGHT:
            m |= MASK_STATE_LIGHT;
            break;

        case BRT_CLIP:
            m |= MASK_STATE_CLIP;
            break;

        case BRT_BOUNDS:
            m |= MASK_STATE_BOUNDS;
            break;

        case BRT_CULL:
            m |= MASK_STATE_CULL;
            break;

        case BRT_OUTPUT:
            m |= MASK_STATE_OUTPUT;
            break;

        case BRT_PRIMITIVE:
            m |= MASK_STATE_PRIMITIVE;
            break;

        default:
            break;
        }
    }

    *mask = m;

    return BRE_OK;
}

static br_error BREND_CMETHOD_DECL(BREND_CLASS(br_renderer), boundsTest)(br_renderer* self, br_token* r, br_bounds3_f* bounds) {
    // FIXME: Should probably cache this.
    br_matrix4 m2s;
    BrMatrix4Mul34(&m2s, &self->state.current->matrix.model_to_view, &self->state.current->matrix.view_to_screen);
    *r = BREND_FNPREFIX(OnScreenCheck)(self, &m2s, bounds);
    return BRE_OK;
}

static br_error BREND_CMETHOD_DECL(BREND_CLASS(br_renderer), coverageTest)(br_renderer* self, br_float* r, br_bounds3_f* bounds) {
    return BRE_FAIL;
}

static br_error BREND_CMETHOD_DECL(BREND_CLASS(br_renderer), viewDistance)(br_renderer* self, br_float* r) {
    return BRE_FAIL;
}

static br_error BREND_CMETHOD_DECL(BREND_CLASS(br_renderer), flush)(br_renderer* self, br_boolean wait) {
    return BRE_OK;
}

static br_error BREND_CMETHOD_DECL(BREND_CLASS(br_renderer), synchronise)(br_renderer* self, br_token sync_type, br_boolean block) {
    return BRE_UNSUPPORTED;
}

static br_error BREND_CMETHOD_DECL(BREND_CLASS(br_renderer), partQueryCapability)(br_renderer* self, br_token part, br_int_32 index,
    br_token_value* buffer, br_size_t buffer_size) {
    return BRE_FAIL;
}

static br_error BREND_CMETHOD_DECL(BREND_CLASS(br_renderer), stateQueryPerformance)(br_renderer* self, br_fixed_lu* speed) {
    return BRE_FAIL;
}

static br_error BREND_CMETHOD_DECL(BREND_CLASS(br_renderer), frameBegin)(br_renderer* self) {
#if defined(BREND_DRIVER_GL)
    /* The sky/horizon dome is positioned at sky_distance = yon_z - 1 (see MungeSkyModel),
     * i.e. right on the far clip plane. At certain camera pitch/yaw angles a thin strip of
     * the farthest (horizon) band crosses the far plane and is clipped away, leaving an
     * uncovered band at the world horizon (black against the cleared framebuffer). Depth
     * clamping disables far-plane clipping and clamps depth instead, so the horizon band
     * always rasterizes (clamped to max depth, staying behind terrain). */
    glEnable(GL_DEPTH_CLAMP);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#endif
    self->frame_stats.model_count = 0;
    return BRE_OK;
}

static br_error BREND_CMETHOD_DECL(BREND_CLASS(br_renderer), frameEnd)(br_renderer* self) {
    return BRE_OK;
}

static br_error BREND_CMETHOD_DECL(BREND_CLASS(br_renderer), focusLossBegin)(br_renderer* self) {
    return BRE_OK;
}

static br_error BREND_CMETHOD_DECL(BREND_CLASS(br_renderer), focusLossEnd)(br_renderer* self) {
    return BRE_OK;
}

/*
 * Default dispatch table for renderer
 */
static const struct br_renderer_dispatch rendererDispatch = {
    .__reserved0 = NULL,
    .__reserved1 = NULL,
    .__reserved2 = NULL,
    .__reserved3 = NULL,
    ._free = BREND_CMETHOD_REF(BREND_CLASS(br_renderer), free),
    ._identifier = BREND_CMETHOD_REF(BREND_CLASS(br_renderer), identifier),
    ._type = BREND_CMETHOD_REF(BREND_CLASS(br_renderer), type),
    ._isType = BREND_CMETHOD_REF(BREND_CLASS(br_renderer), isType),
    ._device = BREND_CMETHOD_REF(BREND_CLASS(br_renderer), device),
    ._space = BREND_CMETHOD_REF(BREND_CLASS(br_renderer), space),
    ._templateQuery = BREND_CMETHOD_REF(BREND_CLASS(br_renderer), templateQuery),
    ._query = BR_CMETHOD_REF(br_object, query),
    ._queryBuffer = BR_CMETHOD_REF(br_object, queryBuffer),
    ._queryMany = BR_CMETHOD_REF(br_object, queryMany),
    ._queryManySize = BR_CMETHOD_REF(br_object, queryManySize),
    ._queryAll = BR_CMETHOD_REF(br_object, queryAll),
    ._queryAllSize = BR_CMETHOD_REF(br_object, queryAllSize),
    ._listQuery = BREND_CMETHOD_REF(BREND_CLASS(br_renderer), listQuery),
    ._tokensMatchBegin = BR_CMETHOD_REF(br_object_container, tokensMatchBegin),
    ._tokensMatch = BR_CMETHOD_REF(br_object_container, tokensMatch),
    ._tokensMatchEnd = BR_CMETHOD_REF(br_object_container, tokensMatchEnd),
    ._addFront = BR_CMETHOD_REF(br_object_container, addFront),
    ._removeFront = BR_CMETHOD_REF(br_object_container, removeFront),
    ._remove = BR_CMETHOD_REF(br_object_container, remove),
    ._find = BR_CMETHOD_REF(br_object_container, find),
    ._findMany = BR_CMETHOD_REF(br_object_container, findMany),
    ._count = BR_CMETHOD_REF(br_object_container, count),

    ._validDestination = BREND_CMETHOD_REF(BREND_CLASS(br_renderer), validDestination),
    ._stateStoredNew = BREND_CMETHOD_REF(BREND_CLASS(br_renderer), stateStoredNew),
    ._stateStoredAvail = BREND_CMETHOD_REF(BREND_CLASS(br_renderer), stateStoredAvail),
    ._bufferStoredNew = BREND_CMETHOD_REF(BREND_CLASS(br_renderer), bufferStoredNew),
    ._bufferStoredAvail = BREND_CMETHOD_REF(BREND_CLASS(br_renderer), bufferStoredAvail),
    ._partSet = BREND_CMETHOD_REF(BREND_CLASS(br_renderer), partSet),
    ._partSetMany = BREND_CMETHOD_REF(BREND_CLASS(br_renderer), partSetMany),
    ._partQuery = BREND_CMETHOD_REF(BREND_CLASS(br_renderer), partQuery),
    ._partQueryBuffer = BREND_CMETHOD_REF(BREND_CLASS(br_renderer), partQueryBuffer),
    ._partQueryMany = BREND_CMETHOD_REF(BREND_CLASS(br_renderer), partQueryMany),
    ._partQueryManySize = BREND_CMETHOD_REF(BREND_CLASS(br_renderer), partQueryManySize),
    ._partQueryAll = BREND_CMETHOD_REF(BREND_CLASS(br_renderer), partQueryAll),
    ._partQueryAllSize = BREND_CMETHOD_REF(BREND_CLASS(br_renderer), partQueryAllSize),
    ._partIndexQuery = BREND_CMETHOD_REF(BREND_CLASS(br_renderer), partIndexQuery),
    ._modelMulF = BREND_CMETHOD_REF(BREND_CLASS(br_renderer), modelMul),
    ._modelPopPushMulF = BREND_CMETHOD_REF(BREND_CLASS(br_renderer), modelPopPushMul),
    ._modelInvert = BREND_CMETHOD_REF(BREND_CLASS(br_renderer), modelInvert),
    ._statePush = BREND_CMETHOD_REF(BREND_CLASS(br_renderer), statePush),
    ._statePop = BREND_CMETHOD_REF(BREND_CLASS(br_renderer), statePop),
    ._stateSave = BREND_CMETHOD_REF(BREND_CLASS(br_renderer), stateSave),
    ._stateRestore = BREND_CMETHOD_REF(BREND_CLASS(br_renderer), stateRestore),
    ._stateMask = BREND_CMETHOD_REF(BREND_CLASS(br_renderer), stateMask),
    ._stateDefault = BREND_CMETHOD_REF(BREND_CLASS(br_renderer), stateDefault),
    ._boundsTestF = BREND_CMETHOD_REF(BREND_CLASS(br_renderer), boundsTest),
    ._coverageTestF = BREND_CMETHOD_REF(BREND_CLASS(br_renderer), coverageTest),
    ._viewDistanceF = BREND_CMETHOD_REF(BREND_CLASS(br_renderer), viewDistance),
    ._commandModeSet = BREND_CMETHOD_REF(BREND_CLASS(br_renderer), commandModeSet),
    ._commandModeQuery = BREND_CMETHOD_REF(BREND_CLASS(br_renderer), commandModeQuery),
    ._commandModeDefault = BREND_CMETHOD_REF(BREND_CLASS(br_renderer), commandModeDefault),
    ._commandModePush = BREND_CMETHOD_REF(BREND_CLASS(br_renderer), commandModePush),
    ._commandModePop = BREND_CMETHOD_REF(BREND_CLASS(br_renderer), commandModePop),
    ._flush = BREND_CMETHOD_REF(BREND_CLASS(br_renderer), flush),
    ._synchronise = BREND_CMETHOD_REF(BREND_CLASS(br_renderer), synchronise),
    ._partQueryCapability = BREND_CMETHOD_REF(BREND_CLASS(br_renderer), partQueryCapability),
    ._stateQueryPerformance = BREND_CMETHOD_REF(BREND_CLASS(br_renderer), stateQueryPerformance),
    ._frameBegin = BREND_CMETHOD_REF(BREND_CLASS(br_renderer), frameBegin),
    ._frameEnd = BREND_CMETHOD_REF(BREND_CLASS(br_renderer), frameEnd),
    ._focusLossBegin = BREND_CMETHOD_REF(BREND_CLASS(br_renderer), focusLossBegin),
    ._focusLossEnd = BREND_CMETHOD_REF(BREND_CLASS(br_renderer), focusLossEnd),
    ._sceneBegin = BREND_CMETHOD_REF(BREND_CLASS(br_renderer), sceneBegin),
    ._sceneEnd = BREND_CMETHOD_REF(BREND_CLASS(br_renderer), sceneEnd),
};
