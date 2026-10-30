#ifndef VIDEO_H_
#define VIDEO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>

#include "shader_data.h"
#include "sdl3_shaders.h"
#include "sdl3_dyn.h"

struct br_device_pixelmap;
struct br_font;

#define MAX_FRAMES_IN_FLIGHT 2

/* Sub-area scenes (rear-view mirror, wreck summary, 3D PIP) draw CPU
 * pre-scene content (grey fills, grid lines) into lockedPixels that must
 * persist under the 3D. That content is uploaded into a pooled BGRA texture
 * at sceneBegin and drawn as an opaque quad under the 3D. A pool is required
 * because the uploads happen mid-frame while every draw of the main command
 * buffer executes at present — a single texture would be clobbered by the
 * next scene's upload before the earlier scene's draws sample it. */
#define SDL3GPUREND_BG_POOL 4

/* Uniform slot / sampler slot mapping (matches the shared GLSL bindings). */
#define SDL3GPUREND_MODEL_UNIFORM_SLOT     0
#define SDL3GPUREND_SCENE_UNIFORM_SLOT     1
#define SDL3GPUREND_TEXT_UNIFORM_SLOT      2
#define SDL3GPUREND_FRAGMENT_SAMPLER_SLOT  0

/* Maximum number of font atlases cached in the VIDEO instance. */
#define TEXT_ATLAS_CACHE_MAX 8

typedef struct {
    struct br_font* font;
    SDL_GPUTexture* texture;
    int atlasWidth;
    int atlasHeight;
} text_atlas_cache_entry;

/*
 * SDL3 GPU uniform slot mapping (matches the shared GLSL bindings):
 *
 *   set1 binding0 = shader_data_model  (vertex)  -> vertex   uniform slot 0
 *   set1 binding1 = shader_data_scene  (vertex)  -> vertex   uniform slot 1
 *   set2 binding0 = main_texture       (fragment)-> fragment sampler slot 0
 *   set3 binding0 = shader_data_model  (fragment)-> fragment uniform slot 0
 *   set3 binding1 = shader_data_scene  (fragment)-> fragment uniform slot 1
 *
 * Uniform data is pushed with SDL_PushGPUVertexUniformData /
 * SDL_PushGPUFragmentUniformData. NOTE: each SDL3 GPU backend caps the
 * readable window of a pushed uniform slot at 4096 bytes
 * (MAX_UBO_SECTION_SIZE in SDL3's backends) even though the push itself can
 * be larger, so everything a shader reads must live within the first 4096
 * bytes of its block (see the field order comment on shader_data_scene).
 * No UBO buffers, no descriptor sets, no per-draw bind changes.
 *
 * The scene is pushed once per sceneBegin (slots 1); the model is pushed per
 * draw (slots 0) by modelrender.c. Uniform slot data is stored on the command
 * buffer, so pushes done outside a render pass persist for the whole frame.
 */

typedef struct _VIDEO {
    void* res;

    SDL_GPUDevice* device;
    SDL_GPUShaderFormat shaderFormat;
    SDL_Window* window;
    SDL_GPUTextureFormat swapchainTextureFormat;
    SDL_GPUTextureFormat depthFormat;
    int windowWidth;
    int windowHeight;

    /* Offscreen render target (isle-portable pattern). Every render pass
     * targets transferTexture + depthTexture, so there is exactly one depth
     * attachment for the whole frame and the pipelines are created once
     * against a fixed format. At present the frame is blitted to the
     * swapchain texture (SDL_BlitGPUTexture), decoupling rendering from the
     * swapchain lifecycle entirely. */
    SDL_GPUTexture* transferTexture;
    SDL_GPUTexture* depthTexture;

    /* Shaders (per-format sources from drivers/commonrend/*.glsl, embedded via
     * sdl3_shaders.c and selected by the device's shader format — SPIR-V /
     * MSL / DXIL) and pipelines. Pipelines render into transferTexture's
     * format, so they never need rebuilding on swapchain recreation. */
    SDL_GPUShader* brenderVertShader;
    SDL_GPUShader* brenderFragShader;
    SDL_GPUShader* overlayVertShader;
    SDL_GPUShader* overlayFragShader;
    SDL_GPUShader* defaultVertShader;
    SDL_GPUShader* defaultFragShader;
    SDL_GPUShader* textVertShader;
    SDL_GPUShader* textFragShader;

    SDL_GPUGraphicsPipeline* brenderPipeline;
    SDL_GPUGraphicsPipeline* brenderPipelineNoDepth;
    SDL_GPUGraphicsPipeline* brenderPipelineLE;
    SDL_GPUGraphicsPipeline* brenderBlendPipeline;
    SDL_GPUGraphicsPipeline* brenderBlendPipelineNoDepth;
    SDL_GPUGraphicsPipeline* brenderBlendPipelineLE;
    SDL_GPUGraphicsPipeline* defaultPipeline;
    SDL_GPUGraphicsPipeline* overlayPipeline;
    SDL_GPUGraphicsPipeline* textPipeline;

    /* Lazily-built font atlases (16x16 glyph grid) for device BrPixelmapText. */
    text_atlas_cache_entry textAtlas[TEXT_ATLAS_CACHE_MAX];
    int textAtlasCount;
    int textAtlasReplace;

    /* Command recording: one SDL_GPUCommandBuffer per frame, alternating copy
     * passes and render passes (SDL3 GPU forbids nesting but allows
     * interleaving). renderPassActive tracks whether a render pass is open. */
    SDL_GPUCommandBuffer* commandBuffer;
    SDL_GPURenderPass* currentPass;
    int isRecording;
    int renderPassActive;
    int sceneCount;
    uint32_t currentFrame;
    int frameFlushed;
    int renderingStarted;

    /* Fences per frame slot. frameFence signals the main submit; ringUploadFence
     * signals the ring upload submit. The queue is FIFO so waiting frameFence
     * also implies the ring upload completed, but both are waited before the
     * slot's transfer buffers are reused in SDL3GPUREND_EnsureRecording. */
    SDL_GPUFence* frameFence[MAX_FRAMES_IN_FLIGHT];
    SDL_GPUFence* ringUploadFence[MAX_FRAMES_IN_FLIGHT];

    /* Per-frame CPU->GPU staging for texture uploads. SDL3GPUREND_UploadBufferToImage
     * memcpy's into the current slot's mapped transfer buffer, then records a
     * copy pass into the slot's own upload command buffer and submits it
     * immediately with uploadFence. The slot is never reused until that fence
     * signals, so uploads are safe to record from anywhere (frame loop or
     * out-of-frame asset loading). No manual barriers — SDL3 GPU inserts them. */
    SDL_GPUTransferBuffer* stagingTransfer[MAX_FRAMES_IN_FLIGHT];
    void* stagingMapped[MAX_FRAMES_IN_FLIGHT];
    size_t stagingOffset[MAX_FRAMES_IN_FLIGHT];
    size_t stagingSize;
    SDL_GPUFence* uploadFence[MAX_FRAMES_IN_FLIGHT];

    /* Shared persistent dynamic VBO/IBO rings for small models (electro-ray
     * segments, sparks, dim quads, pratcam quad), mirroring the sdl3gpurend
     * driver. SDL3 GPU cannot bind host-visible memory as vertex/index buffers,
     * so the ring is split in two halves:
     *
     *   dyn*Transfer (mapped SDL_GPUTransferBuffer)  — written by memcpy during
     *       frame recording (build_vbo/build_ibo/SDL3GPUREND_RefreshRingStored).
     *   dynVbo/dynIbo (SDL_GPUBuffer, VERTEX/INDEX usage) — bound by draws.
     *
     * The cursors only advance within a frame. At present, before the main
     * submit, a ring-upload command buffer copies the written region
     * [0, dyn*Written) from the transfer buffer into the GPU ring. Because it
     * is submitted first and the queue is FIFO, every draw in the main buffer
     * sees the ring data regardless of where in the frame it was written —
     * RefreshRingStored mid-render-pass works exactly like the VK driver.
     * The per-slot ring data is never reused while the GPU may still reference
     * it (cursors reset only after the fence waits in EnsureRecording).
     * Ring usage is gated on isRecording (models built at load time keep
     * dedicated buffers). */
    SDL_GPUTransferBuffer* dynVboTransfer[MAX_FRAMES_IN_FLIGHT];
    void* dynVboMapped[MAX_FRAMES_IN_FLIGHT];
    SDL_GPUBuffer* dynVbo[MAX_FRAMES_IN_FLIGHT];
    size_t dynVboOffset[MAX_FRAMES_IN_FLIGHT];
    size_t dynVboWritten[MAX_FRAMES_IN_FLIGHT];
    size_t dynVboCapacity;
    SDL_GPUTransferBuffer* dynIboTransfer[MAX_FRAMES_IN_FLIGHT];
    void* dynIboMapped[MAX_FRAMES_IN_FLIGHT];
    SDL_GPUBuffer* dynIbo[MAX_FRAMES_IN_FLIGHT];
    size_t dynIboOffset[MAX_FRAMES_IN_FLIGHT];
    size_t dynIboWritten[MAX_FRAMES_IN_FLIGHT];
    size_t dynIboCapacity;
    /* Monotonic counter bumped once per frame (in SDL3GPUREND_EnsureRecording after
     * the ring cursors reset). Stored geometries that sub-allocate from the ring
     * stamp ringEpoch with this value; a mismatch at render time means the ring
     * slot was reset since the model was built, so its geometry is stale and
     * must be re-uploaded. */
    uint32_t frameEpoch;

    /* Overlay / 3DFX 2D composite state (unchanged semantics from sdl3gpurend).
     * lockedPixels holds the CPU 2D surface; dirty regions are uploaded into
     * overlayTexture; the overlay quad (overlayPipeline) is drawn inside the
     * scene's render pass so the 2D content composites on top of the 3D. */
    SDL_GPUSampler* overlaySampler;
    SDL_GPUBuffer* overlayQuadVbo;
    SDL_GPUBuffer* overlayQuadIbo;
    int overlayDirty;
    /* Per-frame flag: the CPU overlay (lockedPixels) had been flushed to the
     * overlay texture before the FIRST scene of the frame began. Captured from
     * overlayDirty once per frame (firstScene only) in sceneBegin, then
     * RE-CLASSIFIED at the first model draw of that scene (see pendingMainPurge):
     * a dim-quad first model keeps it (2D-primary map frame), any other first
     * model clears it (racing frame whose pre-scene flush was only the sky/fog
     * fill). It gates the dim-quad handling: on the map screen the flushed map
     * image is dimmed in place and text drawn after stays bright, while racing
     * dim quads purge the cockpit so the GPU dim dims the 3D underneath. The
     * mid-frame flush before the pratcam also sets overlayDirty, but must NOT
     * re-classify the frame, so sub-area background handling reads overlayDirty
     * directly. This replaces the old get_map_mode host callback, so the driver
     * no longer depends on any game logic. */
    int overlayPrimaryFrame;
    /* Armed at the first scene's sceneBegin when that scene targets the full
     * screen; consumed at the first model draw of that scene. Whether the purge
     * runs is decided there, because a pre-first-scene flush means either a
     * 2D-primary map frame (first model is a dim quad, which dims the flushed
     * map in place — cancel the purge) or a racing frame with a pre-scene
     * sky/fog fill that must not cover the 3D (purge the rect and clear the
     * overlay-primary classification). Reset every frame. */
    int pendingMainPurge;
    SDL_GPUTexture* overlayTexture;
    /* Background texture pool for sub-area scenes (see SDL3GPUREND_BG_POOL).
     * Created lazily at game-screen size when a sub-area scene first needs
     * one; see SDL3GPUREND_DrawSceneBackground. */
    SDL_GPUTexture* bgTexture[SDL3GPUREND_BG_POOL];
    int bgSceneIndex;
    int clearAreaCount;
    br_rectangle clearAreas[4];
    int pratcamAreaCount;
    br_rectangle pratcamArea;
    void* lockedPixels;
    int pm_type;
    int pm_width;
    int pm_height;
    int pm_row_bytes;
    struct br_device_pixelmap* primaryColourTarget;

    int viewportX;
    int viewportY;
    int viewportW;
    int viewportH;
    int mainViewportX;
    int mainViewportY;
    int mainViewportW;
    int mainViewportH;

    /* Draw-state caching to avoid redundant binds. */
    SDL_GPUBuffer* lastVbo;
    SDL_GPUBuffer* lastIbo;
    size_t lastVboOffset;
    size_t lastIboOffset;
    SDL_GPUTexture* lastTexture;
    SDL_GPUSampler* lastSampler;
    SDL_GPUGraphicsPipeline* lastPipeline;

    /* Default / fallback resources. */
    SDL_GPUTexture* defaultTexture;
    SDL_GPUSampler* samplerLinear;
    SDL_GPUSampler* samplerNearest;

    shader_data_scene sceneData;
    shader_data_model modelData;
    shader_data_light lightData;

    /* Host-side hooks copied from br_device_sdl3gpu_callback_procs at
     * SDL3GPUREND_VideoOpen. Optional — the driver tolerates NULL (no resize
     * detection), which is what keeps the driver buildable/runnable outside
     * dethrace. */
    br_device_sdl3gpu_get_window_size_cbfn   *get_window_size;
    br_device_sdl3gpu_getviewport_cbfn       *get_viewport;
} VIDEO, *HVIDEO;

static inline void SDL3GPUREND_GetWindowSize(HVIDEO hVideo, int* width, int* height) {
    if (hVideo->get_window_size) {
        hVideo->get_window_size(width, height);
    } else {
        *width = 0;
        *height = 0;
    }
}

HVIDEO SDL3GPUREND_VideoOpen(HVIDEO hVideo, void* parent,
    const SDL3GPUREND_ShaderSource* brender,
    const SDL3GPUREND_ShaderSource* overlay,
    const SDL3GPUREND_ShaderSource* defaultShaders,
    br_device_sdl3gpu_callback_procs* callbacks, int width, int height,
    bool debug_mode);

void SDL3GPUREND_VideoClose(HVIDEO hVideo);

void SDL3GPUREND_VideoResize(HVIDEO hVideo);

/* Creates a shader for the device's backend. Picks the matching format from
 * `source` (SPIR-V / MSL / DXIL) and fails with BR_FATAL if the format the
 * device needs was not produced by this build. */
/* Creates a shader for the device's backend. Picks the matching format from
 * `source` (SPIR-V / MSL / DXIL) and fails with BR_FATAL if the format the
 * device needs was not produced by this build. `fragUniformBuffers` sizes the
 * fragment stage's uniform-slot count (3 for the text shader, whose colour
 * block lives at slot 2; the vertex stage always reserves 2). */
SDL_GPUShader* SDL3GPUREND_CreateShader(HVIDEO hVideo, const SDL3GPUREND_ShaderSource* source, SDL_GPUShaderStage stage,
    Uint32 fragUniformBuffers);

SDL_GPUGraphicsPipeline* SDL3GPUREND_CreateGraphicsPipeline(HVIDEO hVideo,
    SDL_GPUShader* vertModule, SDL_GPUShader* fragModule,
    const SDL_GPUVertexBufferDescription* bindingDesc,
    const SDL_GPUVertexAttribute* attrDescs, uint32_t attrCount,
    uint32_t width, uint32_t height, bool blendEnable,
    bool depthTestEnable, bool depthWriteEnable,
    SDL_GPUCompareOp compareOp);

/* Copies the scene UBO payload into hVideo->sceneData. The actual push to the
 * GPU happens in SDL3GPUREND_SceneBegin (uniform slot 1 on both stages), so the
 * scene only needs one push per pass, not one per draw. */
void SDL3GPUREND_UpdateScene(HVIDEO hVideo, void* data, size_t size);

/* Pushes the current scene UBO (hVideo->sceneData) to uniform slot 1 on both
 * stages. Called once per scene before the first model draw. */
void SDL3GPUREND_SceneBegin(HVIDEO hVideo);

/* Pushes the per-draw model payload to uniform slot 0 on both stages. Called
 * by modelrender.c immediately before the draw. */
void SDL3GPUREND_PushModel(HVIDEO hVideo, const void* data, size_t size);

void SDL3GPUREND_BeginRenderPass(HVIDEO hVideo);

void SDL3GPUREND_EndRenderPass(HVIDEO hVideo);

/* Clears the frame's shared depth attachment to 1.0 mid-frame. SDL3 GPU has no
 * in-pass clear (unlike VK's vkCmdClearAttachments), so this ends the current
 * render pass and begins a new one against the same transferTexture with the
 * color target LOADed (frame so far preserved) and depth LOADOP_CLEAR'd. Used
 * by BrPixelmapFill on BRT_DEPTH pixelmaps: the game clears a depth buffer
 * before every z-buffered scene (rear-view mirror, wreck summary), and all
 * scenes share the one depth texture, so without this the mirror/wreck scenes
 * would depth-test against stale main-view depth. */
void SDL3GPUREND_ClearDepthAttachment(HVIDEO hVideo);

void SDL3GPUREND_EnsureRecording(HVIDEO hVideo);

/* Uploads the current render pass's framebuffer contents to the swapchain and
 * submits the frame. First submits the ring upload (copy ring transfer buffer
 * -> ring GPU buffer) so the main submit's draws see this frame's ring data,
 * then submits the main command buffer. Returns nonzero on error. */
int SDL3GPUREND_Present(HVIDEO hVideo);

struct br_geometry_stored;
void SDL3GPUREND_RefreshRingStored(HVIDEO hVideo, struct br_geometry_stored* self);

/* Draws the overlay quad (samples overlayTexture) into the currently active
 * render pass. */
void SDL3GPUREND_OverlayDraw(HVIDEO hVideo);

/* Letterbox: centre pm_w x pm_h in win_w x win_h, returning the scaled,
 * centered rect and scale factors (rx, ry). Any output may be NULL. Shared by
 * sceneBegin and the overlay draw. */
void SDL3GPUREND_LetterboxViewport(int win_w, int win_h, int pm_w, int pm_h,
    int* vp_x, int* vp_y, int* vp_w, int* vp_h, float* rx, float* ry);

/* Viewport used by the shared renderer for the game screen: honours a
 * platform get_viewport override (widescreen), else 4:3 letterbox. */
void SDL3GPUREND_ViewportTransform(HVIDEO hVideo, int win_w, int win_h,
    int pm_w, int pm_h, int* x, int* y, float* rx, float* ry);

/* Snapshot the colour_target rect (gx,gy,gw,gh) of the CPU locked buffer
 * (lockedPixels) into a pooled BGRA texture and draw it as an opaque quad
 * mapped to the scene viewport, UNDER the 3D. Call from sceneBegin after the
 * viewport is set, before the first model draw. The sceneEnd purge erases the
 * same rect from the 2D composite, so the content only appears behind the 3D.
 * No-op when there is nothing to draw or the pool is exhausted. */
void SDL3GPUREND_DrawSceneBackground(HVIDEO hVideo, int gx, int gy, int gw, int gh);

/* Uploads `hostDataSize` bytes of host memory into `texture` (width x height
 * at dstX,dstY) through the current frame slot's staging transfer buffer. The
 * data is memcpy'd synchronously, then a copy pass is recorded into the slot's
 * upload command buffer and submitted immediately with the slot's uploadFence.
 * The next call on this slot waits that fence before reusing the staging, so
 * uploads are safe from the frame loop or out-of-frame track/asset loading.
 * SDL3 GPU handles the layout transitions. Returns 0 on success, nonzero on
 * failure. */
int SDL3GPUREND_UploadBufferToImage(HVIDEO hVideo, SDL_GPUTexture* texture,
    uint32_t width, uint32_t height, uint32_t dstX, uint32_t dstY,
    const void* hostData, size_t hostDataSize);

/* Uploads host memory into a GPU-local buffer (vertex/index/any) through the
 * current frame slot's staging transfer buffer. Same lifecycle as
 * SDL3GPUREND_UploadBufferToImage; safe from the frame loop or out-of-frame
 * asset loading. Returns 0 on success, nonzero on failure. */
int SDL3GPUREND_UploadBufferToBuffer(HVIDEO hVideo, SDL_GPUBuffer* buffer,
    const void* hostData, size_t hostDataSize);

/* Releases a texture/sampler/buffer/pipeline that may still be in use by the
 * GPU. SDL3 GPU resources are reference-counted and SDL_ReleaseGPU* schedules
 * the safe destruction, so this is just a direct release — no deferred-free
 * lists needed (unlike the VK driver's manual memory management). */
void SDL3GPUREND_DeferFreeImage(HVIDEO hVideo, SDL_GPUTexture* texture, SDL_GPUSampler* sampler);
void SDL3GPUREND_DeferFreeBuffer(HVIDEO hVideo, SDL_GPUBuffer* buffer);

/* Fills a screen-space rectangle in the CPU locked buffer with the transparent
 * magenta sentinel so it isn't composited over GPU-rendered content. */
static inline void SDL3GPUREND_PurgeRect(int bpp, br_uint_32 magenta, void* pixels,
    int pm_width, int pm_height, int pm_row_bytes,
    int x, int y, int w, int h) {
    int row_w = pm_row_bytes / bpp;
    for (int dy = 0; dy < h; dy++) {
        int py = y + dy;
        if (py < 0 || py >= pm_height) continue;
        int off = py * row_w + x;
        int cw = w;
        if (x < 0) { off -= x; cw += x; }
        if (x + cw > pm_width) cw = pm_width - x;
        if (off < 0) continue;
        for (int dx = 0; dx < cw; dx++) {
            if (bpp == 2)
                ((br_uint_16*)pixels)[off + dx] = (br_uint_16)magenta;
            else
                ((br_uint_32*)pixels)[off + dx] = magenta;
        }
    }
}

/* Halves the RGB channels of every non-magenta pixel in a screen-space
 * rectangle of the CPU locked buffer (RGB565 only), simulating the game's
 * DimRectangle quad for 2D-primary frames where the CPU map image sits on top
 * of the GPU dim quad and must be dimmed in place. */
static inline void SDL3GPUREND_DimRect(void* pixels,
    int pm_width, int pm_height, int pm_row_bytes,
    int x, int y, int w, int h) {
    int row_w = pm_row_bytes / 2;
    for (int dy = 0; dy < h; dy++) {
        int py = y + dy;
        if (py < 0 || py >= pm_height) continue;
        int px = x;
        int cw = w;
        if (px < 0) { px = 0; cw = w + x; }
        if (px + cw > pm_width) cw = pm_width - px;
        br_uint_16* row = &((br_uint_16*)pixels)[py * row_w];
        for (int dx = 0; dx < cw; dx++) {
            br_uint_16 p = row[px + dx];
            if (p == BR_COLOUR_565(31, 0, 31)) continue;
            int r5 = (p >> 11) & 0x1F, g6 = (p >> 5) & 0x3F, b5 = p & 0x1F;
            row[px + dx] = (br_uint_16)(((r5 >> 1) << 11) | ((g6 >> 1) << 5) | (b5 >> 1));
        }
    }
}

#ifdef __cplusplus
};
#endif

#endif
