/*
 * SDL3 GPU video driver for BRender.
 *
 * Rendering model: every render pass targets an offscreen
 * (transferTexture + depthTexture) so the depth attachment and all pipeline
 * formats are fixed for the lifetime of the window; the frame is blitted to
 * the swapchain texture at present. SDL3 GPU inserts the necessary layout
 * transitions, so this driver needs no explicit barriers.
 */

#include <string.h>

#include "drv.h"
#include "drv_ip.h"
#include "brsdl3gpurend.h"
#include "gstored.h"
#include "sdl3_shaders.h"
#include "sdl3gpurend_shader_formats.h"
#include "video.h"

#define SDL3GPUREND_DEFAULT_RING_VBO_CAPACITY (512 * 1024)
#define SDL3GPUREND_DEFAULT_RING_IBO_CAPACITY (256 * 1024)
#define SDL3GPUREND_DEFAULT_STAGING_CAPACITY  (16 * 1024 * 1024)

#define SDL3GPUREND_OVERLAY_QUAD_VERTS   4
#define SDL3GPUREND_OVERLAY_QUAD_INDICES 6

static HVIDEO g_sdl3gpurend_video = NULL;
static void (*g_sdl3gpurend_external_cb)(void* cmd, void* ud) = NULL;
static void* g_sdl3gpurend_external_ud = NULL;

static void WaitFence(SDL_GPUDevice* device, SDL_GPUFence* fence) {
    if (!fence) return;
    SDL_GPUFence* fences[1] = { fence };
    SDL3_WaitForGPUFences(device, true, fences, 1);
    SDL3_ReleaseGPUFence(device, fence);
}

int SDL3GPUREND_UploadBufferToBuffer(HVIDEO hVideo, SDL_GPUBuffer* buffer, const void* hostData, size_t size);

static int CreateOffscreenTargets(HVIDEO hVideo) {
    SDL_GPUTextureCreateInfo ti = {0};
    ti.type = SDL_GPU_TEXTURETYPE_2D;
    ti.format = hVideo->swapchainTextureFormat;
    ti.usage = SDL_GPU_TEXTUREUSAGE_COLOR_TARGET | SDL_GPU_TEXTUREUSAGE_SAMPLER;
    ti.width = hVideo->windowWidth;
    ti.height = hVideo->windowHeight;
    ti.layer_count_or_depth = 1;
    ti.num_levels = 1;
    ti.sample_count = SDL_GPU_SAMPLECOUNT_1;

    hVideo->transferTexture = SDL3_CreateGPUTexture(hVideo->device, &ti);
    if (!hVideo->transferTexture) {
        BR_FATAL("SDL3GPU: Failed to create transfer texture.");
        return 0;
    }

    ti.format = hVideo->depthFormat;
    ti.usage = SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET;
    hVideo->depthTexture = SDL3_CreateGPUTexture(hVideo->device, &ti);
    if (!hVideo->depthTexture) {
        BR_FATAL("SDL3GPU: Failed to create depth texture.");
        return 0;
    }
    return 1;
}

static int CreateSamplers(HVIDEO hVideo) {
    SDL_GPUSamplerCreateInfo si = {0};
    si.min_filter = SDL_GPU_FILTER_LINEAR;
    si.mag_filter = SDL_GPU_FILTER_LINEAR;
    si.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR;
    si.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
    si.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
    si.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    si.compare_op = SDL_GPU_COMPAREOP_INVALID;
    si.max_anisotropy = 0.0f;
    si.enable_anisotropy = false;

    hVideo->samplerLinear = SDL3_CreateGPUSampler(hVideo->device, &si);
    if (!hVideo->samplerLinear) {
        BR_FATAL("SDL3GPU: Failed to create linear sampler.");
        return 0;
    }

    si.min_filter = SDL_GPU_FILTER_NEAREST;
    si.mag_filter = SDL_GPU_FILTER_NEAREST;
    si.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST;
    hVideo->samplerNearest = SDL3_CreateGPUSampler(hVideo->device, &si);
    if (!hVideo->samplerNearest) {
        BR_FATAL("SDL3GPU: Failed to create nearest sampler.");
        return 0;
    }

    hVideo->overlaySampler = hVideo->samplerLinear;
    return 1;
}

static int CreateDefaultTexture(HVIDEO hVideo) {
    SDL_GPUTextureCreateInfo ti = {0};
    ti.type = SDL_GPU_TEXTURETYPE_2D;
    ti.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
    ti.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER;
    ti.width = 1;
    ti.height = 1;
    ti.layer_count_or_depth = 1;
    ti.num_levels = 1;
    ti.sample_count = SDL_GPU_SAMPLECOUNT_1;

    hVideo->defaultTexture = SDL3_CreateGPUTexture(hVideo->device, &ti);
    if (!hVideo->defaultTexture) {
        BR_FATAL("SDL3GPU: Failed to create default texture.");
        return 0;
    }

    const uint32_t white = 0xFFFFFFFF;
    if (SDL3GPUREND_UploadBufferToImage(hVideo, hVideo->defaultTexture, 1, 1, 0, 0,
            &white, sizeof(white)) != 0)
        return 0;

    return 1;
}

static int CreateOverlayQuad(HVIDEO hVideo) {
    float quad[] = {
         1.0f, -1.0f,   1.0f, 1.0f,
         1.0f,  1.0f,   1.0f, 0.0f,
        -1.0f,  1.0f,   0.0f, 0.0f,
        -1.0f, -1.0f,   0.0f, 1.0f,
    };
    uint16_t quadIdx[] = {0, 1, 3, 1, 2, 3};

    SDL_GPUBufferCreateInfo bi = {0};
    bi.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
    bi.size = sizeof(quad);
    hVideo->overlayQuadVbo = SDL3_CreateGPUBuffer(hVideo->device, &bi);
    if (!hVideo->overlayQuadVbo) {
        BR_FATAL("SDL3GPU: Failed to create overlay quad VBO.");
        return 0;
    }

    bi.usage = SDL_GPU_BUFFERUSAGE_INDEX;
    bi.size = sizeof(quadIdx);
    hVideo->overlayQuadIbo = SDL3_CreateGPUBuffer(hVideo->device, &bi);
    if (!hVideo->overlayQuadIbo) {
        BR_FATAL("SDL3GPU: Failed to create overlay quad IBO.");
        return 0;
    }

    /* Static resources upload through the staging path (fills slot 0; its
     * fence is waited in the first SDL3GPUREND_EnsureRecording). */
    if (SDL3GPUREND_UploadBufferToBuffer(hVideo, hVideo->overlayQuadVbo, quad, sizeof(quad)) != 0)
        return 0;
    if (SDL3GPUREND_UploadBufferToBuffer(hVideo, hVideo->overlayQuadIbo, quadIdx, sizeof(quadIdx)) != 0)
        return 0;

    return 1;
}

/*
 * Ensures the current frame slot has a mapped staging transfer buffer with at
 * least `need` free bytes. Grows (waits the slot's pending upload first) when
 * the current staging buffer is exhausted.
 */
static int EnsureStagingCapacity(HVIDEO hVideo, size_t need) {
    uint32_t f = hVideo->currentFrame;

    if (hVideo->stagingTransfer[f] &&
        hVideo->stagingMapped[f] &&
        hVideo->stagingOffset[f] + need <= hVideo->stagingSize) {
        return 1;
    }

    if (hVideo->uploadFence[f]) {
        WaitFence(hVideo->device, hVideo->uploadFence[f]);
        hVideo->uploadFence[f] = NULL;
    }
    if (hVideo->stagingMapped[f]) {
        SDL3_UnmapGPUTransferBuffer(hVideo->device, hVideo->stagingTransfer[f]);
        hVideo->stagingMapped[f] = NULL;
    }

    size_t newSize = hVideo->stagingSize ? hVideo->stagingSize * 2 : SDL3GPUREND_DEFAULT_STAGING_CAPACITY;
    while (newSize < need) newSize *= 2;

    SDL_GPUTransferBufferCreateInfo tci = {0};
    tci.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    tci.size = newSize;
    SDL_GPUTransferBuffer* tb = SDL3_CreateGPUTransferBuffer(hVideo->device, &tci);
    if (!tb) {
        BR_FATAL("SDL3GPU: Failed to create staging transfer buffer.");
        return 0;
    }

    if (hVideo->stagingTransfer[f])
        SDL3_ReleaseGPUTransferBuffer(hVideo->device, hVideo->stagingTransfer[f]);
    hVideo->stagingTransfer[f] = tb;
    hVideo->stagingSize = newSize;
    hVideo->stagingOffset[f] = 0;
    hVideo->stagingMapped[f] = SDL3_MapGPUTransferBuffer(hVideo->device, tb, false);
    if (!hVideo->stagingMapped[f]) {
        BR_FATAL("SDL3GPU: Failed to map staging transfer buffer.");
        return 0;
    }
    return 1;
}

static int EnsureStagingMapped(HVIDEO hVideo, uint32_t f) {
    if (hVideo->stagingMapped[f]) return 1;
    if (!hVideo->stagingTransfer[f])
        return EnsureStagingCapacity(hVideo, SDL3GPUREND_DEFAULT_STAGING_CAPACITY);
    hVideo->stagingMapped[f] = SDL3_MapGPUTransferBuffer(hVideo->device, hVideo->stagingTransfer[f], false);
    return hVideo->stagingMapped[f] != NULL;
}

/*
 * Uploads host data into a GPU buffer through the current frame slot's staging
 * transfer buffer. Same staging/fence discipline as SDL3GPUREND_UploadBufferToImage.
 * Returns 0 on success, nonzero on failure.
 */
int SDL3GPUREND_UploadBufferToBuffer(HVIDEO hVideo, SDL_GPUBuffer* buffer, const void* hostData, size_t size) {
    if (!hostData || size == 0) return -1;

    uint32_t f = hVideo->currentFrame;

    if (hVideo->uploadFence[f]) {
        WaitFence(hVideo->device, hVideo->uploadFence[f]);
        hVideo->uploadFence[f] = NULL;
    }
    if (!hVideo->stagingMapped[f]) {
        if (!EnsureStagingMapped(hVideo, f))
            return -1;
        hVideo->stagingOffset[f] = 0;
    }
    if (hVideo->stagingOffset[f] + size > hVideo->stagingSize) {
        if (!EnsureStagingCapacity(hVideo, size))
            return -1;
    }
    if (!hVideo->stagingMapped[f])
        return -1;

    memcpy((char*)hVideo->stagingMapped[f] + hVideo->stagingOffset[f], hostData, size);
    SDL3_UnmapGPUTransferBuffer(hVideo->device, hVideo->stagingTransfer[f]);
    hVideo->stagingMapped[f] = NULL;

    SDL_GPUCommandBuffer* cmd = SDL3_AcquireGPUCommandBuffer(hVideo->device);
    if (!cmd) {
        BR_FATAL("SDL3GPU: Failed to acquire upload command buffer.");
        return -1;
    }

    SDL_GPUCopyPass* copy = SDL3_BeginGPUCopyPass(cmd);
    SDL_GPUTransferBufferLocation src = { hVideo->stagingTransfer[f], (Uint32)hVideo->stagingOffset[f] };
    SDL_GPUBufferRegion dst = { buffer, 0, (Uint32)size };
    SDL3_UploadToGPUBuffer(copy, &src, &dst, false);
    SDL3_EndGPUCopyPass(copy);

    SDL_GPUFence* fence = SDL3_SubmitGPUCommandBufferAndAcquireFence(cmd);
    if (!fence) {
        BR_FATAL("SDL3GPU: Failed to submit upload command buffer.");
        return -1;
    }
    hVideo->uploadFence[f] = fence;
    hVideo->stagingOffset[f] += (Uint32)size;
    return 0;
}

static int CreateRings(HVIDEO hVideo) {
    hVideo->dynVboCapacity = SDL3GPUREND_DEFAULT_RING_VBO_CAPACITY;
    hVideo->dynIboCapacity = SDL3GPUREND_DEFAULT_RING_IBO_CAPACITY;

    for (uint32_t f = 0; f < MAX_FRAMES_IN_FLIGHT; f++) {
        SDL_GPUBufferCreateInfo bi = {0};
        bi.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
        bi.size = hVideo->dynVboCapacity;
        hVideo->dynVbo[f] = SDL3_CreateGPUBuffer(hVideo->device, &bi);
        if (!hVideo->dynVbo[f]) {
            BR_FATAL("SDL3GPU: Failed to create dynamic VBO.");
            return 0;
        }

        bi.usage = SDL_GPU_BUFFERUSAGE_INDEX;
        bi.size = hVideo->dynIboCapacity;
        hVideo->dynIbo[f] = SDL3_CreateGPUBuffer(hVideo->device, &bi);
        if (!hVideo->dynIbo[f]) {
            BR_FATAL("SDL3GPU: Failed to create dynamic IBO.");
            return 0;
        }

        SDL_GPUTransferBufferCreateInfo tci = {0};
        tci.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
        tci.size = hVideo->dynVboCapacity;
        hVideo->dynVboTransfer[f] = SDL3_CreateGPUTransferBuffer(hVideo->device, &tci);
        if (!hVideo->dynVboTransfer[f]) {
            BR_FATAL("SDL3GPU: Failed to create dynamic VBO transfer buffer.");
            return 0;
        }
        hVideo->dynVboMapped[f] = SDL3_MapGPUTransferBuffer(hVideo->device, hVideo->dynVboTransfer[f], false);
        if (!hVideo->dynVboMapped[f]) {
            BR_FATAL("SDL3GPU: Failed to map dynamic VBO transfer buffer.");
            return 0;
        }

        tci.size = hVideo->dynIboCapacity;
        hVideo->dynIboTransfer[f] = SDL3_CreateGPUTransferBuffer(hVideo->device, &tci);
        if (!hVideo->dynIboTransfer[f]) {
            BR_FATAL("SDL3GPU: Failed to create dynamic IBO transfer buffer.");
            return 0;
        }
        hVideo->dynIboMapped[f] = SDL3_MapGPUTransferBuffer(hVideo->device, hVideo->dynIboTransfer[f], false);
        if (!hVideo->dynIboMapped[f]) {
            BR_FATAL("SDL3GPU: Failed to map dynamic IBO transfer buffer.");
            return 0;
        }

        tci.size = SDL3GPUREND_DEFAULT_STAGING_CAPACITY;
        hVideo->stagingTransfer[f] = SDL3_CreateGPUTransferBuffer(hVideo->device, &tci);
        if (!hVideo->stagingTransfer[f]) {
            BR_FATAL("SDL3GPU: Failed to create staging transfer buffer.");
            return 0;
        }
        hVideo->stagingMapped[f] = SDL3_MapGPUTransferBuffer(hVideo->device, hVideo->stagingTransfer[f], false);
        if (!hVideo->stagingMapped[f]) {
            BR_FATAL("SDL3GPU: Failed to map staging transfer buffer.");
            return 0;
        }
        hVideo->stagingSize = SDL3GPUREND_DEFAULT_STAGING_CAPACITY;
    }
    return 1;
}

static void ReleaseRings(HVIDEO hVideo) {
    SDL_GPUDevice* device = hVideo->device;
    if (!device) return;
    for (uint32_t f = 0; f < MAX_FRAMES_IN_FLIGHT; f++) {
        if (hVideo->dynVboMapped[f]) { SDL3_UnmapGPUTransferBuffer(device, hVideo->dynVboTransfer[f]); hVideo->dynVboMapped[f] = NULL; }
        if (hVideo->dynIboMapped[f]) { SDL3_UnmapGPUTransferBuffer(device, hVideo->dynIboTransfer[f]); hVideo->dynIboMapped[f] = NULL; }
        if (hVideo->stagingMapped[f]) { SDL3_UnmapGPUTransferBuffer(device, hVideo->stagingTransfer[f]); hVideo->stagingMapped[f] = NULL; }
        if (hVideo->dynVboTransfer[f]) { SDL3_ReleaseGPUTransferBuffer(device, hVideo->dynVboTransfer[f]); hVideo->dynVboTransfer[f] = NULL; }
        if (hVideo->dynIboTransfer[f]) { SDL3_ReleaseGPUTransferBuffer(device, hVideo->dynIboTransfer[f]); hVideo->dynIboTransfer[f] = NULL; }
        if (hVideo->stagingTransfer[f]) { SDL3_ReleaseGPUTransferBuffer(device, hVideo->stagingTransfer[f]); hVideo->stagingTransfer[f] = NULL; }
        if (hVideo->dynVbo[f]) { SDL3_ReleaseGPUBuffer(device, hVideo->dynVbo[f]); hVideo->dynVbo[f] = NULL; }
        if (hVideo->dynIbo[f]) { SDL3_ReleaseGPUBuffer(device, hVideo->dynIbo[f]); hVideo->dynIbo[f] = NULL; }
    }
}

SDL_GPUShader* SDL3GPUREND_CreateShader(HVIDEO hVideo, const SDL3GPUREND_ShaderSource* source, SDL_GPUShaderStage stage,
    Uint32 fragUniformBuffers) {
    SDL_GPUShaderCreateInfo ci = {0};

    /* Select the source for the device's backend. The Metal backend always
     * takes MSL source (the format mask is MSL | METALLIB); the D3D12 and
     * Vulkan backends take DXIL and SPIR-V respectively. */
    if (hVideo->shaderFormat & SDL_GPU_SHADERFORMAT_SPIRV) {
        ci.format = SDL_GPU_SHADERFORMAT_SPIRV;
        ci.code = (const Uint8*)source->spirv;
        ci.code_size = source->spirv_size;
        ci.entrypoint = "main";
    } else if (hVideo->shaderFormat & SDL_GPU_SHADERFORMAT_MSL) {
        ci.format = SDL_GPU_SHADERFORMAT_MSL;
        ci.code = (const Uint8*)source->msl;
        ci.code_size = source->msl_size;
        ci.entrypoint = "main0";
    } else if (hVideo->shaderFormat & SDL_GPU_SHADERFORMAT_DXIL) {
        ci.format = SDL_GPU_SHADERFORMAT_DXIL;
        ci.code = (const Uint8*)source->dxil;
        ci.code_size = source->dxil_size;
        ci.entrypoint = "main";
    } else {
        BR_FATAL1("SDL3GPU: Unsupported device shader format 0x%x.", hVideo->shaderFormat);
        return NULL;
    }

    if (!ci.code || ci.code_size == 0) {
        BR_FATAL2("SDL3GPU: No %s shader source for device format 0x%x (not built into this binary).",
            stage == SDL_GPU_SHADERSTAGE_VERTEX ? "vertex" : "fragment", ci.format);
        return NULL;
    }

    ci.stage = stage;
    ci.num_samplers = 0;
    ci.num_storage_textures = 0;
    ci.num_storage_buffers = 0;
    ci.num_uniform_buffers = 0;

    if (stage == SDL_GPU_SHADERSTAGE_VERTEX) {
        /* set1: model + scene UBOs. */
        ci.num_uniform_buffers = 2;
    } else {
        /* set2: main_texture sampler; set3: model + scene UBOs (+ text colour
         * at slot 2 for the text shader). */
        ci.num_samplers = 1;
        ci.num_uniform_buffers = fragUniformBuffers;
    }

    SDL_GPUShader* shader = SDL3_CreateGPUShader(hVideo->device, &ci);
    if (!shader) {
        BR_FATAL("SDL3GPU: Failed to create shader.");
        return NULL;
    }
    return shader;
}

SDL_GPUGraphicsPipeline* SDL3GPUREND_CreateGraphicsPipeline(HVIDEO hVideo,
    SDL_GPUShader* vertModule, SDL_GPUShader* fragModule,
    const SDL_GPUVertexBufferDescription* bindingDesc,
    const SDL_GPUVertexAttribute* attrDescs, uint32_t attrCount,
    uint32_t width, uint32_t height, bool blendEnable,
    bool depthTestEnable, bool depthWriteEnable,
    SDL_GPUCompareOp compareOp) {

    (void)width;
    (void)height;

    SDL_GPUGraphicsPipelineCreateInfo ci = {0};
    ci.vertex_shader = vertModule;
    ci.fragment_shader = fragModule;
    ci.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;

    ci.vertex_input_state.num_vertex_buffers = bindingDesc ? 1 : 0;
    ci.vertex_input_state.vertex_buffer_descriptions = bindingDesc;
    ci.vertex_input_state.num_vertex_attributes = attrCount;
    ci.vertex_input_state.vertex_attributes = attrDescs;

    ci.rasterizer_state.fill_mode = SDL_GPU_FILLMODE_FILL;
    ci.rasterizer_state.cull_mode = SDL_GPU_CULLMODE_NONE;
    ci.rasterizer_state.front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE;
    ci.rasterizer_state.enable_depth_clip = true;
    ci.rasterizer_state.enable_depth_bias = false;
    ci.rasterizer_state.depth_bias_constant_factor = 0.0f;
    ci.rasterizer_state.depth_bias_clamp = 0.0f;
    ci.rasterizer_state.depth_bias_slope_factor = 0.0f;

    ci.multisample_state.sample_count = SDL_GPU_SAMPLECOUNT_1;
    ci.multisample_state.sample_mask = 0;
    ci.multisample_state.enable_mask = false;
    ci.multisample_state.enable_alpha_to_coverage = false;

    ci.depth_stencil_state.compare_op = compareOp;
    ci.depth_stencil_state.enable_depth_test = depthTestEnable;
    ci.depth_stencil_state.enable_depth_write = depthWriteEnable;
    ci.depth_stencil_state.enable_stencil_test = false;
    ci.depth_stencil_state.compare_mask = 0xFF;
    ci.depth_stencil_state.write_mask = 0xFF;

    SDL_GPUColorTargetBlendState blend = {0};
    blend.color_write_mask = SDL_GPU_COLORCOMPONENT_R | SDL_GPU_COLORCOMPONENT_G |
                             SDL_GPU_COLORCOMPONENT_B | SDL_GPU_COLORCOMPONENT_A;
    blend.enable_blend = blendEnable;
    blend.enable_color_write_mask = false;
    if (blendEnable) {
        blend.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
        blend.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
        blend.color_blend_op = SDL_GPU_BLENDOP_ADD;
        blend.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
        blend.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ZERO;
        blend.alpha_blend_op = SDL_GPU_BLENDOP_ADD;
    }

    /* Pipelines always declare the depth attachment: every render pass targets
     * transferTexture + depthTexture, and depth testing simply stays disabled
     * when the flags say so (mirrors the VK driver). */
    SDL_GPUColorTargetDescription target = {0};
    target.format = hVideo->swapchainTextureFormat;
    target.blend_state = blend;

    ci.target_info.color_target_descriptions = &target;
    ci.target_info.num_color_targets = 1;
    ci.target_info.depth_stencil_format = hVideo->depthFormat;
    ci.target_info.has_depth_stencil_target = true;

    SDL_GPUGraphicsPipeline* pipeline = SDL3_CreateGPUGraphicsPipeline(hVideo->device, &ci);
    if (!pipeline) {
        BR_FATAL("SDL3GPU: Failed to create graphics pipeline.");
        return NULL;
    }
    return pipeline;
}

static const char* SDL3GPUREND_ShaderFormatName(SDL_GPUShaderFormat format) {
    if (format & SDL_GPU_SHADERFORMAT_SPIRV) return "Vulkan (SPIR-V)";
    if (format & SDL_GPU_SHADERFORMAT_MSL) return "Metal (MSL)";
    if (format & SDL_GPU_SHADERFORMAT_DXIL) return "D3D12 (DXIL)";
    return "unknown";
}

HVIDEO SDL3GPUREND_VideoOpen(HVIDEO hVideo, void* parent,
    const SDL3GPUREND_ShaderSource brender[2],
    const SDL3GPUREND_ShaderSource overlay[2],
    const SDL3GPUREND_ShaderSource defaultShaders[2],
    br_device_sdl3gpu_callback_procs* callbacks, int width, int height,
    bool debug_mode) {

    if (hVideo == NULL) {
        BR_FATAL("VIDEO: Invalid handle.");
        return NULL;
    }

    memset(hVideo, 0, sizeof(VIDEO));
    hVideo->res = parent;

    /* Resolve the SDL3 symbol table before the first SDL3 call. Standalone
     * builds bind the pointers at compile time; dynamic builds (dethrace)
     * resolve them from the SDL3 library handle the host passed in the
     * callbacks structure (or load SDL3 themselves if none was given). */
    if (SDL3GPUREND_LoadSDLSymbols(callbacks ? callbacks->sdl3_handle : NULL) != 0) {
        return NULL;
    }

    if (callbacks) {
        hVideo->get_window_size = callbacks->get_window_size;
        hVideo->get_viewport = callbacks->get_viewport;
    }

    /* Fall back to the embedded sources (sdl3_shaders.c) when the caller does
     * not supply shaders. */
    if (!brender) brender = brender_shaders;
    if (!overlay) overlay = overlay_shaders;
    if (!defaultShaders) defaultShaders = brender_shaders;

    hVideo->window = callbacks && callbacks->get_window ? (SDL_Window*)callbacks->get_window() : NULL;
    if (hVideo->window == NULL) {
        BR_FATAL("SDL3GPU: No window provided (get_window callback missing).");
        return NULL;
    }

    /* Request every shader format this build embeds, so whichever backend SDL3
     * GPU picks, its shader format is supported. On Metal the returned mask is
     * MSL | METALLIB; we create MSL shaders (SDL3 accepts MSL source whenever
     * the MSL bit is set). */
    SDL_GPUShaderFormat shaderFormats = SDL_GPU_SHADERFORMAT_SPIRV;
#if SDL3GPUREND_SHADERFORMAT_MSL_AVAILABLE
    shaderFormats |= SDL_GPU_SHADERFORMAT_MSL;
#endif
#if SDL3GPUREND_SHADERFORMAT_DXIL_AVAILABLE
    shaderFormats |= SDL_GPU_SHADERFORMAT_DXIL;
#endif

    hVideo->device = SDL3_CreateGPUDevice(shaderFormats, debug_mode, NULL);
    if (!hVideo->device) {
        BR_FATAL("SDL3GPU: Failed to create GPU device.");
        return NULL;
    }
    if (debug_mode) {
        BrLogPrintf("SDL3GPU: device created with debug/validation enabled\n");
    }
    hVideo->shaderFormat = SDL3_GetGPUShaderFormats(hVideo->device);
    if ((hVideo->shaderFormat & SDL_GPU_SHADERFORMAT_SPIRV) == 0 &&
        (hVideo->shaderFormat & SDL_GPU_SHADERFORMAT_MSL) == 0 &&
        (hVideo->shaderFormat & SDL_GPU_SHADERFORMAT_DXIL) == 0) {
        BR_FATAL1("SDL3GPU: Device backend needs shader format 0x%x, which this build does not provide.",
            hVideo->shaderFormat);
        return NULL;
    }

    if (!SDL3_ClaimWindowForGPUDevice(hVideo->device, hVideo->window)) {
        BR_FATAL("SDL3GPU: Failed to claim window for GPU device.");
        return NULL;
    }

    hVideo->swapchainTextureFormat = SDL3_GetGPUSwapchainTextureFormat(hVideo->device, hVideo->window);
    hVideo->depthFormat = SDL_GPU_TEXTUREFORMAT_D32_FLOAT;

    if (width <= 0 || height <= 0) {
        SDL3_GetWindowSizeInPixels(hVideo->window, &width, &height);
    }
    hVideo->windowWidth = width;
    hVideo->windowHeight = height;

    if (!CreateOffscreenTargets(hVideo))
        goto cleanup;
    if (!CreateSamplers(hVideo))
        goto cleanup;
    if (!CreateRings(hVideo))
        goto cleanup;
    if (!CreateOverlayQuad(hVideo))
        goto cleanup;
    if (!CreateDefaultTexture(hVideo))
        goto cleanup;

    hVideo->brenderVertShader = SDL3GPUREND_CreateShader(hVideo, &brender[SDL3GPUREND_STAGE_VERTEX], SDL_GPU_SHADERSTAGE_VERTEX, 2);
    hVideo->brenderFragShader = SDL3GPUREND_CreateShader(hVideo, &brender[SDL3GPUREND_STAGE_FRAGMENT], SDL_GPU_SHADERSTAGE_FRAGMENT, 2);
    hVideo->overlayVertShader = SDL3GPUREND_CreateShader(hVideo, &overlay[SDL3GPUREND_STAGE_VERTEX], SDL_GPU_SHADERSTAGE_VERTEX, 2);
    hVideo->overlayFragShader = SDL3GPUREND_CreateShader(hVideo, &overlay[SDL3GPUREND_STAGE_FRAGMENT], SDL_GPU_SHADERSTAGE_FRAGMENT, 2);
    if (!hVideo->brenderVertShader || !hVideo->brenderFragShader ||
        !hVideo->overlayVertShader || !hVideo->overlayFragShader)
        goto cleanup_shaders;

    {
        SDL_GPUVertexBufferDescription bindingDesc = {0};
        bindingDesc.slot = 0;
        bindingDesc.pitch = sizeof(sdl3_vertex_f);
        bindingDesc.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
        bindingDesc.instance_step_rate = 0;

        SDL_GPUVertexAttribute attrDescs[4] = {0};
        attrDescs[0].location = 0;
        attrDescs[0].buffer_slot = 0;
        attrDescs[0].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3;
        attrDescs[0].offset = offsetof(sdl3_vertex_f, p);
        attrDescs[1].location = 1;
        attrDescs[1].buffer_slot = 0;
        attrDescs[1].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
        attrDescs[1].offset = offsetof(sdl3_vertex_f, map);
        attrDescs[2].location = 2;
        attrDescs[2].buffer_slot = 0;
        attrDescs[2].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3;
        attrDescs[2].offset = offsetof(sdl3_vertex_f, n);
        attrDescs[3].location = 3;
        attrDescs[3].buffer_slot = 0;
        attrDescs[3].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4;
        attrDescs[3].offset = offsetof(sdl3_vertex_f, c);

        hVideo->brenderPipeline = SDL3GPUREND_CreateGraphicsPipeline(hVideo,
            hVideo->brenderVertShader, hVideo->brenderFragShader,
            &bindingDesc, attrDescs, 4,
            hVideo->windowWidth, hVideo->windowHeight, false, true, true,
            SDL_GPU_COMPAREOP_LESS);
        if (!hVideo->brenderPipeline)
            goto cleanup_shaders;

        hVideo->brenderPipelineNoDepth = SDL3GPUREND_CreateGraphicsPipeline(hVideo,
            hVideo->brenderVertShader, hVideo->brenderFragShader,
            &bindingDesc, attrDescs, 4,
            hVideo->windowWidth, hVideo->windowHeight, false, false, false,
            SDL_GPU_COMPAREOP_LESS);
        if (!hVideo->brenderPipelineNoDepth)
            goto cleanup_shaders;

        /* BRender's default depth function is BR_LESS_OR_EQUAL (see
         * BRender's default renderer state); the GL driver has a matching
         * pipeline pair, so SDL3GPU needs one too or the shared commonrend
         * draw path has nothing to select. */
        hVideo->brenderPipelineLE = SDL3GPUREND_CreateGraphicsPipeline(hVideo,
            hVideo->brenderVertShader, hVideo->brenderFragShader,
            &bindingDesc, attrDescs, 4,
            hVideo->windowWidth, hVideo->windowHeight, false, true, true,
            SDL_GPU_COMPAREOP_LESS_OR_EQUAL);
        if (!hVideo->brenderPipelineLE)
            goto cleanup_shaders;

        hVideo->brenderBlendPipeline = SDL3GPUREND_CreateGraphicsPipeline(hVideo,
            hVideo->brenderVertShader, hVideo->brenderFragShader,
            &bindingDesc, attrDescs, 4,
            hVideo->windowWidth, hVideo->windowHeight, true, true, false,
            SDL_GPU_COMPAREOP_LESS);
        if (!hVideo->brenderBlendPipeline)
            goto cleanup_shaders;

        hVideo->brenderBlendPipelineNoDepth = SDL3GPUREND_CreateGraphicsPipeline(hVideo,
            hVideo->brenderVertShader, hVideo->brenderFragShader,
            &bindingDesc, attrDescs, 4,
            hVideo->windowWidth, hVideo->windowHeight, true, false, false,
            SDL_GPU_COMPAREOP_LESS);
        if (!hVideo->brenderBlendPipelineNoDepth)
            goto cleanup_shaders;

        hVideo->brenderBlendPipelineLE = SDL3GPUREND_CreateGraphicsPipeline(hVideo,
            hVideo->brenderVertShader, hVideo->brenderFragShader,
            &bindingDesc, attrDescs, 4,
            hVideo->windowWidth, hVideo->windowHeight, true, true, false,
            SDL_GPU_COMPAREOP_LESS_OR_EQUAL);
        if (!hVideo->brenderBlendPipelineLE)
            goto cleanup_shaders;
    }

    /* Default shaders/pipeline alias the brender ones unless the caller
     * supplied a distinct pair. */
    if (defaultShaders != brender_shaders) {
        hVideo->defaultVertShader = SDL3GPUREND_CreateShader(hVideo, &defaultShaders[SDL3GPUREND_STAGE_VERTEX], SDL_GPU_SHADERSTAGE_VERTEX, 2);
        hVideo->defaultFragShader = SDL3GPUREND_CreateShader(hVideo, &defaultShaders[SDL3GPUREND_STAGE_FRAGMENT], SDL_GPU_SHADERSTAGE_FRAGMENT, 2);
    }
    hVideo->defaultPipeline = hVideo->brenderPipeline;

    {
        SDL_GPUVertexBufferDescription bindingDesc = {0};
        bindingDesc.slot = 0;
        bindingDesc.pitch = 4 * sizeof(float);
        bindingDesc.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
        bindingDesc.instance_step_rate = 0;

        SDL_GPUVertexAttribute attrDescs[2] = {0};
        attrDescs[0].location = 0;
        attrDescs[0].buffer_slot = 0;
        attrDescs[0].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
        attrDescs[0].offset = 0;
        attrDescs[1].location = 1;
        attrDescs[1].buffer_slot = 0;
        attrDescs[1].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
        attrDescs[1].offset = 2 * sizeof(float);

        hVideo->overlayPipeline = SDL3GPUREND_CreateGraphicsPipeline(hVideo,
            hVideo->overlayVertShader, hVideo->overlayFragShader,
            &bindingDesc, attrDescs, 2,
            hVideo->windowWidth, hVideo->windowHeight, true, false, false,
            SDL_GPU_COMPAREOP_LESS);
        if (!hVideo->overlayPipeline)
            goto cleanup_shaders;
    }

    /* Text pipeline: glyph quads (same vertex layout as the overlay) sampled
     * through text.vert/text.frag. The text colour block lives at fragment
     * uniform slot 2, so the fragment shader needs 3 uniform slots. */
    hVideo->textVertShader = SDL3GPUREND_CreateShader(hVideo, &text_shaders[SDL3GPUREND_STAGE_VERTEX],
        SDL_GPU_SHADERSTAGE_VERTEX, 2);
    hVideo->textFragShader = SDL3GPUREND_CreateShader(hVideo, &text_shaders[SDL3GPUREND_STAGE_FRAGMENT],
        SDL_GPU_SHADERSTAGE_FRAGMENT, 3);
    if (!hVideo->textVertShader || !hVideo->textFragShader)
        goto cleanup_shaders;

    {
        SDL_GPUVertexBufferDescription bindingDesc = {0};
        bindingDesc.slot = 0;
        bindingDesc.pitch = 4 * sizeof(float);
        bindingDesc.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
        bindingDesc.instance_step_rate = 0;

        SDL_GPUVertexAttribute attrDescs[2] = {0};
        attrDescs[0].location = 0;
        attrDescs[0].buffer_slot = 0;
        attrDescs[0].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
        attrDescs[0].offset = 0;
        attrDescs[1].location = 1;
        attrDescs[1].buffer_slot = 0;
        attrDescs[1].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
        attrDescs[1].offset = 2 * sizeof(float);

        hVideo->textPipeline = SDL3GPUREND_CreateGraphicsPipeline(hVideo,
            hVideo->textVertShader, hVideo->textFragShader,
            &bindingDesc, attrDescs, 2,
            hVideo->windowWidth, hVideo->windowHeight, true, false, false,
            SDL_GPU_COMPAREOP_LESS);
        if (!hVideo->textPipeline)
            goto cleanup_shaders;
    }

    BrLogPrintf("SDL3GPU: GPU device initialized (%s, framebuffer %dx%d)\n",
        SDL3GPUREND_ShaderFormatName(hVideo->shaderFormat),
        hVideo->windowWidth, hVideo->windowHeight);

    g_sdl3gpurend_video = hVideo;
    return hVideo;

cleanup_shaders:
    if (hVideo->textPipeline) { SDL3_ReleaseGPUGraphicsPipeline(hVideo->device, hVideo->textPipeline); hVideo->textPipeline = NULL; }
    if (hVideo->textFragShader) { SDL3_ReleaseGPUShader(hVideo->device, hVideo->textFragShader); hVideo->textFragShader = NULL; }
    if (hVideo->textVertShader) { SDL3_ReleaseGPUShader(hVideo->device, hVideo->textVertShader); hVideo->textVertShader = NULL; }
    if (hVideo->overlayVertShader) SDL3_ReleaseGPUShader(hVideo->device, hVideo->overlayVertShader);
    if (hVideo->overlayFragShader) SDL3_ReleaseGPUShader(hVideo->device, hVideo->overlayFragShader);
    if (hVideo->brenderFragShader) SDL3_ReleaseGPUShader(hVideo->device, hVideo->brenderFragShader);
    if (hVideo->brenderVertShader) SDL3_ReleaseGPUShader(hVideo->device, hVideo->brenderVertShader);
cleanup:
    ReleaseRings(hVideo);
    if (hVideo->overlayQuadIbo) { SDL3_ReleaseGPUBuffer(hVideo->device, hVideo->overlayQuadIbo); hVideo->overlayQuadIbo = NULL; }
    if (hVideo->overlayQuadVbo) { SDL3_ReleaseGPUBuffer(hVideo->device, hVideo->overlayQuadVbo); hVideo->overlayQuadVbo = NULL; }
    if (hVideo->samplerNearest) { SDL3_ReleaseGPUSampler(hVideo->device, hVideo->samplerNearest); hVideo->samplerNearest = NULL; }
    if (hVideo->samplerLinear) { SDL3_ReleaseGPUSampler(hVideo->device, hVideo->samplerLinear); hVideo->samplerLinear = NULL; }
    if (hVideo->depthTexture) { SDL3_ReleaseGPUTexture(hVideo->device, hVideo->depthTexture); hVideo->depthTexture = NULL; }
    if (hVideo->transferTexture) { SDL3_ReleaseGPUTexture(hVideo->device, hVideo->transferTexture); hVideo->transferTexture = NULL; }
    if (hVideo->window) { SDL3_ReleaseWindowFromGPUDevice(hVideo->device, hVideo->window); }
    if (hVideo->device) { SDL3_DestroyGPUDevice(hVideo->device); hVideo->device = NULL; }
    hVideo->window = NULL;
    return NULL;
}

void SDL3GPUREND_VideoClose(HVIDEO hVideo) {
    if (!hVideo || !hVideo->device) return;

    for (uint32_t f = 0; f < MAX_FRAMES_IN_FLIGHT; f++) {
        WaitFence(hVideo->device, hVideo->frameFence[f]);
        hVideo->frameFence[f] = NULL;
        WaitFence(hVideo->device, hVideo->ringUploadFence[f]);
        hVideo->ringUploadFence[f] = NULL;
        WaitFence(hVideo->device, hVideo->uploadFence[f]);
        hVideo->uploadFence[f] = NULL;
    }

    /* Drain any submitted command buffer whose fence we may no longer hold
     * (e.g. an upload fence overwritten by a later upload) so every resource
     * is released before the device is destroyed. */
    SDL3_WaitForGPUIdle(hVideo->device);

    ReleaseRings(hVideo);

    if (hVideo->defaultFragShader && hVideo->defaultFragShader != hVideo->brenderFragShader)
        SDL3_ReleaseGPUShader(hVideo->device, hVideo->defaultFragShader);
    if (hVideo->defaultVertShader && hVideo->defaultVertShader != hVideo->brenderVertShader)
        SDL3_ReleaseGPUShader(hVideo->device, hVideo->defaultVertShader);
    if (hVideo->overlayPipeline) { SDL3_ReleaseGPUGraphicsPipeline(hVideo->device, hVideo->overlayPipeline); hVideo->overlayPipeline = NULL; }
    if (hVideo->brenderBlendPipelineLE) { SDL3_ReleaseGPUGraphicsPipeline(hVideo->device, hVideo->brenderBlendPipelineLE); hVideo->brenderBlendPipelineLE = NULL; }
    if (hVideo->brenderBlendPipelineNoDepth) { SDL3_ReleaseGPUGraphicsPipeline(hVideo->device, hVideo->brenderBlendPipelineNoDepth); hVideo->brenderBlendPipelineNoDepth = NULL; }
    if (hVideo->brenderBlendPipeline) { SDL3_ReleaseGPUGraphicsPipeline(hVideo->device, hVideo->brenderBlendPipeline); hVideo->brenderBlendPipeline = NULL; }
    if (hVideo->brenderPipelineLE) { SDL3_ReleaseGPUGraphicsPipeline(hVideo->device, hVideo->brenderPipelineLE); hVideo->brenderPipelineLE = NULL; }
    if (hVideo->brenderPipelineNoDepth) { SDL3_ReleaseGPUGraphicsPipeline(hVideo->device, hVideo->brenderPipelineNoDepth); hVideo->brenderPipelineNoDepth = NULL; }
    if (hVideo->brenderPipeline) { SDL3_ReleaseGPUGraphicsPipeline(hVideo->device, hVideo->brenderPipeline); hVideo->brenderPipeline = NULL; }
    hVideo->defaultPipeline = NULL;
    if (hVideo->textPipeline) { SDL3_ReleaseGPUGraphicsPipeline(hVideo->device, hVideo->textPipeline); hVideo->textPipeline = NULL; }
    if (hVideo->textFragShader) { SDL3_ReleaseGPUShader(hVideo->device, hVideo->textFragShader); hVideo->textFragShader = NULL; }
    if (hVideo->textVertShader) { SDL3_ReleaseGPUShader(hVideo->device, hVideo->textVertShader); hVideo->textVertShader = NULL; }
    if (hVideo->overlayFragShader) { SDL3_ReleaseGPUShader(hVideo->device, hVideo->overlayFragShader); hVideo->overlayFragShader = NULL; }
    if (hVideo->overlayVertShader) { SDL3_ReleaseGPUShader(hVideo->device, hVideo->overlayVertShader); hVideo->overlayVertShader = NULL; }
    if (hVideo->brenderFragShader) { SDL3_ReleaseGPUShader(hVideo->device, hVideo->brenderFragShader); hVideo->brenderFragShader = NULL; }
    if (hVideo->brenderVertShader) { SDL3_ReleaseGPUShader(hVideo->device, hVideo->brenderVertShader); hVideo->brenderVertShader = NULL; }
    if (hVideo->overlayTexture) { SDL3_ReleaseGPUTexture(hVideo->device, hVideo->overlayTexture); hVideo->overlayTexture = NULL; }
    for (int i = 0; i < SDL3GPUREND_BG_POOL; i++) {
        if (hVideo->bgTexture[i]) { SDL3_ReleaseGPUTexture(hVideo->device, hVideo->bgTexture[i]); hVideo->bgTexture[i] = NULL; }
    }
    for (int i = 0; i < TEXT_ATLAS_CACHE_MAX; i++) {
        if (hVideo->textAtlas[i].texture) { SDL3_ReleaseGPUTexture(hVideo->device, hVideo->textAtlas[i].texture); hVideo->textAtlas[i].texture = NULL; }
    }
    if (hVideo->defaultTexture) { SDL3_ReleaseGPUTexture(hVideo->device, hVideo->defaultTexture); hVideo->defaultTexture = NULL; }
    if (hVideo->overlayQuadIbo) { SDL3_ReleaseGPUBuffer(hVideo->device, hVideo->overlayQuadIbo); hVideo->overlayQuadIbo = NULL; }
    if (hVideo->overlayQuadVbo) { SDL3_ReleaseGPUBuffer(hVideo->device, hVideo->overlayQuadVbo); hVideo->overlayQuadVbo = NULL; }
    if (hVideo->samplerNearest) { SDL3_ReleaseGPUSampler(hVideo->device, hVideo->samplerNearest); hVideo->samplerNearest = NULL; }
    if (hVideo->samplerLinear) { SDL3_ReleaseGPUSampler(hVideo->device, hVideo->samplerLinear); hVideo->samplerLinear = NULL; }
    if (hVideo->depthTexture) { SDL3_ReleaseGPUTexture(hVideo->device, hVideo->depthTexture); hVideo->depthTexture = NULL; }
    if (hVideo->transferTexture) { SDL3_ReleaseGPUTexture(hVideo->device, hVideo->transferTexture); hVideo->transferTexture = NULL; }

    if (hVideo->window) SDL3_ReleaseWindowFromGPUDevice(hVideo->device, hVideo->window);
    SDL3_DestroyGPUDevice(hVideo->device);
    hVideo->device = NULL;
    hVideo->window = NULL;

    if (g_sdl3gpurend_video == hVideo) g_sdl3gpurend_video = NULL;
}

void SDL3GPUREND_VideoResize(HVIDEO hVideo) {
    int w = hVideo->windowWidth, h = hVideo->windowHeight;
    if (hVideo->get_window_size) {
        hVideo->get_window_size(&w, &h);
    } else if (hVideo->window) {
        SDL3_GetWindowSizeInPixels(hVideo->window, &w, &h);
    }
    if (w <= 0 || h <= 0) return;

    if (hVideo->transferTexture) { SDL3_ReleaseGPUTexture(hVideo->device, hVideo->transferTexture); hVideo->transferTexture = NULL; }
    if (hVideo->depthTexture) { SDL3_ReleaseGPUTexture(hVideo->device, hVideo->depthTexture); hVideo->depthTexture = NULL; }

    hVideo->windowWidth = w;
    hVideo->windowHeight = h;

    if (!CreateOffscreenTargets(hVideo)) {
        BR_FATAL("SDL3GPU: Failed to recreate offscreen targets after resize.");
    }
}

void SDL3GPUREND_UpdateScene(HVIDEO hVideo, void* data, size_t size) {
    if (size > sizeof(hVideo->sceneData)) size = sizeof(hVideo->sceneData);
    if (data) memcpy(&hVideo->sceneData, data, size);
}

void SDL3GPUREND_SceneBegin(HVIDEO hVideo) {
    SDL3GPUREND_EnsureRecording(hVideo);
    if (!hVideo->commandBuffer) return;
    SDL3_PushGPUVertexUniformData(hVideo->commandBuffer, SDL3GPUREND_SCENE_UNIFORM_SLOT,
        &hVideo->sceneData, (Uint32)sizeof(hVideo->sceneData));
    SDL3_PushGPUFragmentUniformData(hVideo->commandBuffer, SDL3GPUREND_SCENE_UNIFORM_SLOT,
        &hVideo->sceneData, (Uint32)sizeof(hVideo->sceneData));
}

void SDL3GPUREND_PushModel(HVIDEO hVideo, const void* data, size_t size) {
    if (!data || size == 0) return;
    if (size > sizeof(hVideo->modelData)) size = sizeof(hVideo->modelData);
    SDL3GPUREND_EnsureRecording(hVideo);
    if (!hVideo->commandBuffer) return;
    SDL3_PushGPUVertexUniformData(hVideo->commandBuffer, SDL3GPUREND_MODEL_UNIFORM_SLOT,
        data, (Uint32)size);
    SDL3_PushGPUFragmentUniformData(hVideo->commandBuffer, SDL3GPUREND_MODEL_UNIFORM_SLOT,
        data, (Uint32)size);
}

void SDL3GPUREND_EnsureRecording(HVIDEO hVideo) {
    if (hVideo->isRecording) return;

    uint32_t f = hVideo->currentFrame;

    if (hVideo->frameFence[f]) { WaitFence(hVideo->device, hVideo->frameFence[f]); hVideo->frameFence[f] = NULL; }
    if (hVideo->ringUploadFence[f]) { WaitFence(hVideo->device, hVideo->ringUploadFence[f]); hVideo->ringUploadFence[f] = NULL; }
    if (hVideo->uploadFence[f]) { WaitFence(hVideo->device, hVideo->uploadFence[f]); hVideo->uploadFence[f] = NULL; }

    /* Re-map the transfer buffers for this frame slot. */
    if (hVideo->dynVboTransfer[f] && !hVideo->dynVboMapped[f])
        hVideo->dynVboMapped[f] = SDL3_MapGPUTransferBuffer(hVideo->device, hVideo->dynVboTransfer[f], false);
    if (hVideo->dynIboTransfer[f] && !hVideo->dynIboMapped[f])
        hVideo->dynIboMapped[f] = SDL3_MapGPUTransferBuffer(hVideo->device, hVideo->dynIboTransfer[f], false);
    if (hVideo->stagingTransfer[f] && !hVideo->stagingMapped[f])
        hVideo->stagingMapped[f] = SDL3_MapGPUTransferBuffer(hVideo->device, hVideo->stagingTransfer[f], false);

    hVideo->dynVboOffset[f] = 0;
    hVideo->dynIboOffset[f] = 0;
    hVideo->dynVboWritten[f] = 0;
    hVideo->dynIboWritten[f] = 0;
    hVideo->stagingOffset[f] = 0;

    hVideo->frameEpoch++;

    hVideo->clearAreaCount = 0;
    hVideo->pratcamAreaCount = 0;
    hVideo->bgSceneIndex = 0;
    hVideo->pendingMainPurge = 0;

    /* Resize detection. */
    {
        int w = hVideo->windowWidth, h = hVideo->windowHeight;
        SDL3GPUREND_GetWindowSize(hVideo, &w, &h);
        if (w > 0 && h > 0 && (w != hVideo->windowWidth || h != hVideo->windowHeight)) {
            SDL3GPUREND_VideoResize(hVideo);
            hVideo->mainViewportW = 0;
        }
    }

    hVideo->commandBuffer = SDL3_AcquireGPUCommandBuffer(hVideo->device);
    if (!hVideo->commandBuffer) {
        BR_FATAL("SDL3GPU: Failed to acquire command buffer.");
        return;
    }
    hVideo->isRecording = 1;
    hVideo->renderPassActive = 0;
    hVideo->currentPass = NULL;
}

void SDL3GPUREND_BeginRenderPass(HVIDEO hVideo) {
    if (hVideo->renderPassActive) return;
    SDL3GPUREND_EnsureRecording(hVideo);
    if (!hVideo->commandBuffer) return;

    SDL_GPUColorTargetInfo color = {0};
    color.texture = hVideo->transferTexture;
    color.clear_color = (SDL_FColor){0.0f, 0.0f, 0.0f, 1.0f};
    color.load_op = SDL_GPU_LOADOP_CLEAR;
    color.store_op = SDL_GPU_STOREOP_STORE;

    SDL_GPUDepthStencilTargetInfo depth = {0};
    depth.texture = hVideo->depthTexture;
    depth.clear_depth = 1.0f;
    depth.load_op = SDL_GPU_LOADOP_CLEAR;
    depth.store_op = SDL_GPU_STOREOP_DONT_CARE;
    depth.stencil_load_op = SDL_GPU_LOADOP_DONT_CARE;
    depth.stencil_store_op = SDL_GPU_STOREOP_DONT_CARE;

    hVideo->currentPass = SDL3_BeginGPURenderPass(hVideo->commandBuffer, &color, 1, &depth);
    if (!hVideo->currentPass) {
        BR_FATAL("SDL3GPU: Failed to begin render pass.");
        return;
    }
    hVideo->renderPassActive = 1;

    /* SDL3 GPU resets all render state at every pass start. */
    hVideo->lastPipeline = NULL;
    hVideo->lastVbo = NULL;
    hVideo->lastIbo = NULL;
    hVideo->lastVboOffset = 0;
    hVideo->lastIboOffset = 0;
    hVideo->lastTexture = NULL;
    hVideo->lastSampler = NULL;
}

void SDL3GPUREND_EndRenderPass(HVIDEO hVideo) {
    if (!hVideo->renderPassActive || !hVideo->currentPass) return;
    SDL3_EndGPURenderPass(hVideo->currentPass);
    hVideo->currentPass = NULL;
    hVideo->renderPassActive = 0;
}

void SDL3GPUREND_ClearDepthAttachment(HVIDEO hVideo) {
    if (!hVideo->renderPassActive || !hVideo->currentPass || !hVideo->commandBuffer)
        return;

    SDL3GPUREND_EndRenderPass(hVideo);

    SDL_GPUColorTargetInfo color = {0};
    color.texture = hVideo->transferTexture;
    color.load_op = SDL_GPU_LOADOP_LOAD; /* preserve the frame so far */
    color.store_op = SDL_GPU_STOREOP_STORE;

    SDL_GPUDepthStencilTargetInfo depth = {0};
    depth.texture = hVideo->depthTexture;
    depth.clear_depth = 1.0f;
    depth.load_op = SDL_GPU_LOADOP_CLEAR;
    depth.store_op = SDL_GPU_STOREOP_DONT_CARE;
    depth.stencil_load_op = SDL_GPU_LOADOP_DONT_CARE;
    depth.stencil_store_op = SDL_GPU_STOREOP_DONT_CARE;

    hVideo->currentPass = SDL3_BeginGPURenderPass(hVideo->commandBuffer, &color, 1, &depth);
    if (!hVideo->currentPass) {
        BR_FATAL("SDL3GPU: Failed to begin render pass after depth clear.");
        return;
    }
    hVideo->renderPassActive = 1;

    /* SDL3 GPU resets all render state at every pass start. */
    hVideo->lastPipeline = NULL;
    hVideo->lastVbo = NULL;
    hVideo->lastIbo = NULL;
    hVideo->lastVboOffset = 0;
    hVideo->lastIboOffset = 0;
    hVideo->lastTexture = NULL;
    hVideo->lastSampler = NULL;
}

int SDL3GPUREND_Present(HVIDEO hVideo) {
    uint32_t f = hVideo->currentFrame;
    SDL_GPUDevice* device = hVideo->device;

    if (hVideo->renderPassActive)
        SDL3GPUREND_EndRenderPass(hVideo);

    if (!hVideo->commandBuffer || !hVideo->isRecording) {
        hVideo->currentFrame = (f + 1) % MAX_FRAMES_IN_FLIGHT;
        return 0;
    }

    /* 1. Ring upload: unmap the transfer buffers and copy the written region
     * into the GPU ring buffers. Submitted before the main submit; the queue
     * is FIFO, so every draw in the main buffer sees this frame's ring data. */
    if (hVideo->dynVboMapped[f]) {
        SDL3_UnmapGPUTransferBuffer(device, hVideo->dynVboTransfer[f]);
        hVideo->dynVboMapped[f] = NULL;
    }
    if (hVideo->dynIboMapped[f]) {
        SDL3_UnmapGPUTransferBuffer(device, hVideo->dynIboTransfer[f]);
        hVideo->dynIboMapped[f] = NULL;
    }

    if (hVideo->dynVboWritten[f] > 0 || hVideo->dynIboWritten[f] > 0) {
        SDL_GPUCommandBuffer* upCmd = SDL3_AcquireGPUCommandBuffer(device);
        if (upCmd) {
            SDL_GPUCopyPass* copy = SDL3_BeginGPUCopyPass(upCmd);
            if (hVideo->dynVboWritten[f] > 0) {
                SDL_GPUTransferBufferLocation src = { hVideo->dynVboTransfer[f], 0 };
                SDL_GPUBufferRegion dst = { hVideo->dynVbo[f], 0, (Uint32)hVideo->dynVboWritten[f] };
                SDL3_UploadToGPUBuffer(copy, &src, &dst, false);
            }
            if (hVideo->dynIboWritten[f] > 0) {
                SDL_GPUTransferBufferLocation src = { hVideo->dynIboTransfer[f], 0 };
                SDL_GPUBufferRegion dst = { hVideo->dynIbo[f], 0, (Uint32)hVideo->dynIboWritten[f] };
                SDL3_UploadToGPUBuffer(copy, &src, &dst, false);
            }
            SDL3_EndGPUCopyPass(copy);
            hVideo->ringUploadFence[f] = SDL3_SubmitGPUCommandBufferAndAcquireFence(upCmd);
            if (!hVideo->ringUploadFence[f])
                BR_FATAL("SDL3GPU: Failed to submit ring upload.");
        }
    }

    /* 2. Blit the offscreen frame into the swapchain and submit the main
     * command buffer. */
    SDL_GPUTexture* swapchainTexture = NULL;
    Uint32 sw = 0, sh = 0;
    int acq = SDL3_AcquireGPUSwapchainTexture(hVideo->commandBuffer, hVideo->window, &swapchainTexture, &sw, &sh);
    if (acq) {
        if (swapchainTexture) {
            SDL_GPUBlitInfo blit = {0};
            blit.source.texture = hVideo->transferTexture;
            blit.source.w = hVideo->windowWidth;
            blit.source.h = hVideo->windowHeight;
            blit.destination.texture = swapchainTexture;
            blit.destination.w = sw;
            blit.destination.h = sh;
            blit.load_op = SDL_GPU_LOADOP_DONT_CARE;
            blit.filter = SDL_GPU_FILTER_LINEAR;
            /* BRender's projection is GL-style (NDC y-up); SDL3-GPU NDC
             * is y-down, so the rendered transfer texture is vertically
             * mirrored. Flip it at present time to match the screen. */
            blit.flip_mode = SDL_FLIP_VERTICAL;
            SDL3_BlitGPUTexture(hVideo->commandBuffer, &blit);
        }
    } else {
        BrLogPrintf("SDL3GPU: AcquireGPUSwapchainTexture failed: %s\n", SDL3_GetError());
    }

    if (g_sdl3gpurend_external_cb)
        g_sdl3gpurend_external_cb(hVideo->commandBuffer, g_sdl3gpurend_external_ud);

    hVideo->frameFence[f] = SDL3_SubmitGPUCommandBufferAndAcquireFence(hVideo->commandBuffer);
    if (!hVideo->frameFence[f])
        BR_FATAL("SDL3GPU: Failed to submit frame command buffer.");

    hVideo->commandBuffer = NULL;
    hVideo->isRecording = 0;
    hVideo->renderPassActive = 0;
    hVideo->currentPass = NULL;

    hVideo->currentFrame = (f + 1) % MAX_FRAMES_IN_FLIGHT;
    return 0;
}

/* Letterbox: centre pm_w x pm_h in win_w x win_h, returning the scaled,
 * centered rect and pm->window scale factors (rx, ry). Shared by sceneBegin
 * and the overlay; any output may be NULL (full window, scale 1 if no size). */
void SDL3GPUREND_LetterboxViewport(int win_w, int win_h, int pm_w, int pm_h,
    int* vp_x, int* vp_y, int* vp_w, int* vp_h, float* rx, float* ry) {
    int vp_width = win_w, vp_height = win_h;
    if (pm_w > 0 && pm_h > 0 && win_h > 0) {
        float aspect = (float)win_w / (float)win_h;
        float target = (float)pm_w / (float)pm_h;
        if (aspect > target) {
            vp_width = (int)((float)win_h * target + 0.5f);
        } else {
            vp_height = (int)((float)win_w / target + 0.5f);
        }
    }
    if (vp_x) *vp_x = (win_w - vp_width) / 2;
    if (vp_y) *vp_y = (win_h - vp_height) / 2;
    if (vp_w) *vp_w = vp_width;
    if (vp_h) *vp_h = vp_height;
    if (rx) *rx = (pm_w > 0 && vp_width > 0) ? (float)vp_width / (float)pm_w : 1.0f;
    if (ry) *ry = (pm_h > 0 && vp_height > 0) ? (float)vp_height / (float)pm_h : 1.0f;
}

void SDL3GPUREND_ViewportTransform(HVIDEO hVideo, int win_w, int win_h,
    int pm_w, int pm_h, int* x, int* y, float* rx, float* ry) {
    if (hVideo->get_viewport != NULL) {
        hVideo->get_viewport(x, y, rx, ry);
        return;
    }
    SDL3GPUREND_LetterboxViewport(win_w, win_h, pm_w, pm_h, x, y, NULL, NULL, rx, ry);
}

void SDL3GPUREND_OverlayDraw(HVIDEO hVideo) {
    if (!hVideo->renderPassActive || !hVideo->currentPass) return;
    if (!hVideo->overlayDirty) return;
    if (!hVideo->overlayTexture || !hVideo->overlayPipeline) return;

    SDL_GPURenderPass* pass = hVideo->currentPass;

    SDL_GPUViewport viewport = {0};
    viewport.max_depth = 1.0f;
    SDL_Rect scissor = {0, 0, hVideo->windowWidth, hVideo->windowHeight};
    {
        /* Same letterbox as the scene viewport: centre the 4:3 overlay in the
         * window, matching glrend. Overlay texture is game-screen sized. */
        int vp_x, vp_y, vp_width, vp_height;
        SDL3GPUREND_LetterboxViewport(hVideo->windowWidth, hVideo->windowHeight,
            hVideo->pm_width, hVideo->pm_height,
            &vp_x, &vp_y, &vp_width, &vp_height, NULL, NULL);
        viewport.x = (float)vp_x;
        viewport.y = (float)vp_y;
        viewport.w = (float)vp_width;
        viewport.h = (float)vp_height;
        scissor.x = vp_x;
        scissor.y = vp_y;
        scissor.w = vp_width;
        scissor.h = vp_height;
    }
    SDL3_SetGPUViewport(pass, &viewport);
    SDL3_SetGPUScissor(pass, &scissor);

    if (hVideo->lastPipeline != hVideo->overlayPipeline) {
        SDL3_BindGPUGraphicsPipeline(pass, hVideo->overlayPipeline);
        hVideo->lastPipeline = hVideo->overlayPipeline;
    }

    SDL_GPUBufferBinding vbo = { hVideo->overlayQuadVbo, 0 };
    if (hVideo->lastVbo != hVideo->overlayQuadVbo || hVideo->lastVboOffset != 0) {
        SDL3_BindGPUVertexBuffers(pass, 0, &vbo, 1);
        hVideo->lastVbo = hVideo->overlayQuadVbo;
        hVideo->lastVboOffset = 0;
    }

    SDL_GPUBufferBinding ibo = { hVideo->overlayQuadIbo, 0 };
    if (hVideo->lastIbo != hVideo->overlayQuadIbo || hVideo->lastIboOffset != 0) {
        SDL3_BindGPUIndexBuffer(pass, &ibo, SDL_GPU_INDEXELEMENTSIZE_16BIT);
        hVideo->lastIbo = hVideo->overlayQuadIbo;
        hVideo->lastIboOffset = 0;
    }

    if (hVideo->lastTexture != hVideo->overlayTexture || hVideo->lastSampler != hVideo->overlaySampler) {
        SDL_GPUTextureSamplerBinding tsb = { hVideo->overlayTexture, hVideo->overlaySampler };
        SDL3_BindGPUFragmentSamplers(pass, SDL3GPUREND_FRAGMENT_SAMPLER_SLOT, &tsb, 1);
        hVideo->lastTexture = hVideo->overlayTexture;
        hVideo->lastSampler = hVideo->overlaySampler;
    }

    SDL3_DrawGPUIndexedPrimitives(pass, SDL3GPUREND_OVERLAY_QUAD_INDICES, 1, 0, 0, 0);
    hVideo->overlayDirty = 0;
}

void SDL3GPUREND_DrawSceneBackground(HVIDEO hVideo, int gx, int gy, int gw, int gh) {
    if (!hVideo->renderPassActive || !hVideo->currentPass) return;
    if (!hVideo->lockedPixels || gw <= 0 || gh <= 0) return;
    if (!hVideo->overlayPipeline || !hVideo->overlaySampler) return;
    if (hVideo->pm_width <= 0 || hVideo->pm_height <= 0) return;
    if (hVideo->windowWidth <= 0 || hVideo->windowHeight <= 0) return;

    int bpp = (hVideo->pm_type == BR_PMT_RGB_565 || hVideo->pm_type == BR_PMT_RGB_555) ? 2 : 4;

    int slot = hVideo->bgSceneIndex;
    hVideo->bgSceneIndex = (hVideo->bgSceneIndex + 1) % SDL3GPUREND_BG_POOL;

    SDL_GPUTexture* tex = hVideo->bgTexture[slot];
    if (!tex) {
        SDL_GPUTextureCreateInfo ti = {0};
        ti.type = SDL_GPU_TEXTURETYPE_2D;
        ti.format = SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM;
        ti.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER;
        ti.width = (Uint32)hVideo->pm_width;
        ti.height = (Uint32)hVideo->pm_height;
        ti.layer_count_or_depth = 1;
        ti.num_levels = 1;
        ti.sample_count = SDL_GPU_SAMPLECOUNT_1;
        hVideo->bgTexture[slot] = SDL3_CreateGPUTexture(hVideo->device, &ti);
        if (!hVideo->bgTexture[slot])
            return;
        tex = hVideo->bgTexture[slot];
    }

    /* Snapshot the scene rect's CPU pre-scene content as BGRA8888 (the magenta
     * sentinel becomes transparent so the blended quad does not tint it). This
     * runs at sceneBegin, before the 3D draws; the sceneEnd purge later wipes
     * the rect from lockedPixels so the composite will not re-draw it on top. */
    br_uint_32* bgra = BrScratchAllocate((size_t)gw * gh * 4);
    if (!bgra)
        return;

    if (bpp == 2) {
        int rShift = (hVideo->pm_type == BR_PMT_RGB_565) ? 11 : 10;
        int gShift = (hVideo->pm_type == BR_PMT_RGB_565) ? 5 : 5;
        int gMask = (hVideo->pm_type == BR_PMT_RGB_565) ? 0x3F : 0x1F;
        int gDiv = (hVideo->pm_type == BR_PMT_RGB_565) ? 63 : 31;
        int row_w = hVideo->pm_row_bytes / 2;
        const br_uint_16* src = (const br_uint_16*)hVideo->lockedPixels;
        for (int y = 0; y < gh; y++) {
            for (int x = 0; x < gw; x++) {
                br_uint_16 p = src[(gy + y) * row_w + (gx + x)];
                if (p == BR_COLOUR_565(31, 0, 31)) {
                    bgra[y * gw + x] = 0;
                } else {
                    int r5 = (p >> rShift) & 0x1F;
                    int g = (p >> gShift) & gMask;
                    int b5 = p & 0x1F;
                    bgra[y * gw + x] = (b5 * 255 / 31)
                        | ((g * 255 / gDiv) << 8)
                        | ((r5 * 255 / 31) << 16)
                        | (0xFF << 24);
                }
            }
        }
    } else {
        /* 4 bpp: raw copy; the magenta sentinel samples as rgb==(1,0,1) and is
         * discarded by overlay.frag. */
        int row_w = hVideo->pm_row_bytes / 4;
        const br_uint_32* src = (const br_uint_32*)hVideo->lockedPixels;
        for (int y = 0; y < gh; y++)
            memcpy(&bgra[y * gw], &src[(gy + y) * row_w + gx], (size_t)gw * 4);
    }

    if (SDL3GPUREND_UploadBufferToImage(hVideo, tex, (uint32_t)gw, (uint32_t)gh,
            (uint32_t)gx, (uint32_t)gy, bgra, (size_t)gw * gh * 4) != 0) {
        BrScratchFree(bgra);
        return;
    }
    BrScratchFree(bgra);

    /* The quad fills the whole scene viewport with NDC +-1: the viewport set
     * in sceneBegin already confines it to the scene rect's on-screen area
     * (and the scissor clips to it), so this matches the full-screen overlay
     * quad. overlay.vert flips v (1.0 - v), so the viewport TOP vertex (NDC
     * +1) must carry uv.y = 1 - v1 (rect bottom row) and the BOTTOM vertex
     * (NDC -1) uv.y = 1 - v0 (rect top row) — the rect then lands upright at
     * its on-screen position after the present flip, exactly like the 3D of
     * the same scene and the full-screen overlay quad. */
    float u0 = (float)gx / (float)hVideo->pm_width;
    float u1 = (float)(gx + gw) / (float)hVideo->pm_width;
    float v0 = (float)gy / (float)hVideo->pm_height;
    float v1 = (float)(gy + gh) / (float)hVideo->pm_height;

    float quad[4][4] = {
        { -1.0f,  1.0f, u0, 1.0f - v1 },
        { -1.0f, -1.0f, u0, 1.0f - v0 },
        {  1.0f, -1.0f, u1, 1.0f - v0 },
        {  1.0f,  1.0f, u1, 1.0f - v1 },
    };

    /* Sub-allocate the 4 verts from the ring VBO (same pattern as small model
     * rebuilds); the ring upload at present makes them visible to this draw. */
    int f = hVideo->currentFrame;
    size_t size = sizeof(quad);
    size_t offset = hVideo->dynVboOffset[f];
    offset = (offset + 15) & ~(size_t)15;
    if (!hVideo->isRecording || hVideo->dynVboMapped[f] == NULL ||
        offset + size > hVideo->dynVboCapacity)
        return;
    memcpy((char*)hVideo->dynVboMapped[f] + offset, quad, size);
    hVideo->dynVboOffset[f] = offset + size;
    hVideo->dynVboWritten[f] = hVideo->dynVboOffset[f];

    SDL_GPURenderPass* pass = hVideo->currentPass;

    if (hVideo->lastPipeline != hVideo->overlayPipeline) {
        SDL3_BindGPUGraphicsPipeline(pass, hVideo->overlayPipeline);
        hVideo->lastPipeline = hVideo->overlayPipeline;
    }

    SDL_GPUBufferBinding vbo = { hVideo->dynVbo[f], offset };
    if (hVideo->lastVbo != hVideo->dynVbo[f] || hVideo->lastVboOffset != offset) {
        SDL3_BindGPUVertexBuffers(pass, 0, &vbo, 1);
        hVideo->lastVbo = hVideo->dynVbo[f];
        hVideo->lastVboOffset = offset;
    }

    SDL_GPUBufferBinding ibo = { hVideo->overlayQuadIbo, 0 };
    if (hVideo->lastIbo != hVideo->overlayQuadIbo || hVideo->lastIboOffset != 0) {
        SDL3_BindGPUIndexBuffer(pass, &ibo, SDL_GPU_INDEXELEMENTSIZE_16BIT);
        hVideo->lastIbo = hVideo->overlayQuadIbo;
        hVideo->lastIboOffset = 0;
    }

    if (hVideo->lastTexture != tex || hVideo->lastSampler != hVideo->overlaySampler) {
        SDL_GPUTextureSamplerBinding tsb = { tex, hVideo->overlaySampler };
        SDL3_BindGPUFragmentSamplers(pass, SDL3GPUREND_FRAGMENT_SAMPLER_SLOT, &tsb, 1);
        hVideo->lastTexture = tex;
        hVideo->lastSampler = hVideo->overlaySampler;
    }

    SDL3_DrawGPUIndexedPrimitives(pass, SDL3GPUREND_OVERLAY_QUAD_INDICES, 1, 0, 0, 0);
}

int SDL3GPUREND_UploadBufferToImage(HVIDEO hVideo, SDL_GPUTexture* texture,
    uint32_t width, uint32_t height, uint32_t dstX, uint32_t dstY,
    const void* hostData, size_t hostDataSize) {

    if (!hostData || hostDataSize == 0) return -1;

    uint32_t f = hVideo->currentFrame;

    /* The slot is never reused while a copy it fed is still pending. */
    if (hVideo->uploadFence[f]) {
        WaitFence(hVideo->device, hVideo->uploadFence[f]);
        hVideo->uploadFence[f] = NULL;
    }

    if (!hVideo->stagingMapped[f]) {
        if (!EnsureStagingMapped(hVideo, f))
            return -1;
        hVideo->stagingOffset[f] = 0;
    }

    if (hVideo->stagingOffset[f] + hostDataSize > hVideo->stagingSize) {
        if (!EnsureStagingCapacity(hVideo, hostDataSize))
            return -1;
    }
    if (!hVideo->stagingMapped[f])
        return -1;

    memcpy((char*)hVideo->stagingMapped[f] + hVideo->stagingOffset[f], hostData, hostDataSize);
    SDL3_UnmapGPUTransferBuffer(hVideo->device, hVideo->stagingTransfer[f]);
    hVideo->stagingMapped[f] = NULL;

    SDL_GPUCommandBuffer* cmd = SDL3_AcquireGPUCommandBuffer(hVideo->device);
    if (!cmd) {
        BR_FATAL("SDL3GPU: Failed to acquire upload command buffer.");
        return -1;
    }

    SDL_GPUCopyPass* copy = SDL3_BeginGPUCopyPass(cmd);
    SDL_GPUTextureTransferInfo src = {0};
    src.transfer_buffer = hVideo->stagingTransfer[f];
    src.offset = (Uint32)hVideo->stagingOffset[f];
    SDL_GPUTextureRegion dst = {0};
    dst.texture = texture;
    dst.w = width;
    dst.h = height;
    dst.d = 1;
    dst.x = dstX;
    dst.y = dstY;
    SDL3_UploadToGPUTexture(copy, &src, &dst, false);
    SDL3_EndGPUCopyPass(copy);

    SDL_GPUFence* fence = SDL3_SubmitGPUCommandBufferAndAcquireFence(cmd);
    if (!fence) {
        BR_FATAL("SDL3GPU: Failed to submit upload command buffer.");
        return -1;
    }
    hVideo->uploadFence[f] = fence;
    hVideo->stagingOffset[f] += (Uint32)hostDataSize;
    return 0;
}

void SDL3GPUREND_DeferFreeImage(HVIDEO hVideo, SDL_GPUTexture* texture, SDL_GPUSampler* sampler) {
    if (texture) SDL3_ReleaseGPUTexture(hVideo->device, texture);
    if (sampler) SDL3_ReleaseGPUSampler(hVideo->device, sampler);
}

void SDL3GPUREND_DeferFreeBuffer(HVIDEO hVideo, SDL_GPUBuffer* buffer) {
    if (buffer) SDL3_ReleaseGPUBuffer(hVideo->device, buffer);
}

void SDL3GPUREND_GetDeviceInfo(SDL3GPUREND_DeviceInfo* info) {
    if (!info) return;
    memset(info, 0, sizeof(*info));
    if (!g_sdl3gpurend_video) return;
    info->gpu_device = g_sdl3gpurend_video->device;
    info->window = g_sdl3gpurend_video->window;
    info->swapchain_texture_format = (uint32_t)g_sdl3gpurend_video->swapchainTextureFormat;
}

void SDL3GPUREND_SetExternalRenderCallback(void (*cb)(void* cmd, void* ud), void* ud) {
    g_sdl3gpurend_external_cb = cb;
    g_sdl3gpurend_external_ud = ud;
}
