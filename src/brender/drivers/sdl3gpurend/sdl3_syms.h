#ifndef sdl3gpurend_syms_h
#define sdl3gpurend_syms_h

/* Symbol table for the SDL3 GPU API as used by the sdl3gpurend driver.
 *
 * This list is expanded with the X macro (see sdl3_dyn.h / sdl3_dyn.c) into:
 *   - a function-pointer typedef per SDL3 function,
 *   - an extern function pointer (SDL3_<name>) per SDL3 function.
 *
 * Every SDL3 call in the driver goes through these pointers (SDL3_<name>), so
 * the driver works both linked against SDL3 (standalone BRender builds) and
 * with SDL3 resolved at runtime (dethrace single-executable builds). Keep the
 * signatures in sync with the SDL3 headers (SDL3/SDL_gpu.h, SDL3/SDL_video.h,
 * SDL3/SDL_error.h).
 *
 * X(name, return-type, (parameter-list))
 */

#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>

#define FOREACH_SDL3GPUREND_SYM(X)                                             \
    X(AcquireGPUCommandBuffer, SDL_GPUCommandBuffer*,                       \
      (SDL_GPUDevice* device))                                              \
    X(AcquireGPUSwapchainTexture, bool,                                     \
      (SDL_GPUCommandBuffer* command_buffer, SDL_Window* window,            \
       SDL_GPUTexture** swapchain_texture,                                  \
       Uint32* swapchain_texture_width,                                     \
       Uint32* swapchain_texture_height))                                   \
    X(BeginGPUCopyPass, SDL_GPUCopyPass*,                                   \
      (SDL_GPUCommandBuffer* command_buffer))                               \
    X(BeginGPURenderPass, SDL_GPURenderPass*,                               \
      (SDL_GPUCommandBuffer* command_buffer,                                \
       const SDL_GPUColorTargetInfo* color_target_infos,                    \
       Uint32 num_color_targets,                                            \
       const SDL_GPUDepthStencilTargetInfo* depth_stencil_target_info))     \
    X(BindGPUFragmentSamplers, void,                                        \
      (SDL_GPURenderPass* render_pass, Uint32 first_slot,                   \
       const SDL_GPUTextureSamplerBinding* texture_sampler_bindings,        \
       Uint32 num_bindings))                                                \
    X(BindGPUGraphicsPipeline, void,                                        \
      (SDL_GPURenderPass* render_pass, SDL_GPUGraphicsPipeline* graphics_pipeline)) \
    X(BindGPUIndexBuffer, void,                                             \
      (SDL_GPURenderPass* render_pass, const SDL_GPUBufferBinding* binding, \
       SDL_GPUIndexElementSize index_element_size))                         \
    X(BindGPUVertexBuffers, void,                                           \
      (SDL_GPURenderPass* render_pass, Uint32 first_slot,                   \
       const SDL_GPUBufferBinding* bindings, Uint32 num_bindings))          \
    X(BlitGPUTexture, void,                                                 \
      (SDL_GPUCommandBuffer* command_buffer, const SDL_GPUBlitInfo* info))  \
    X(ClaimWindowForGPUDevice, bool,                                        \
      (SDL_GPUDevice* device, SDL_Window* window))                          \
    X(CreateGPUBuffer, SDL_GPUBuffer*,                                      \
      (SDL_GPUDevice* device, const SDL_GPUBufferCreateInfo* createinfo))   \
    X(CreateGPUDevice, SDL_GPUDevice*,                                      \
      (SDL_GPUShaderFormat format_flags, bool debug_mode, const char* name))\
    X(CreateGPUGraphicsPipeline, SDL_GPUGraphicsPipeline*,                  \
      (SDL_GPUDevice* device, const SDL_GPUGraphicsPipelineCreateInfo* createinfo)) \
    X(CreateGPUSampler, SDL_GPUSampler*,                                    \
      (SDL_GPUDevice* device, const SDL_GPUSamplerCreateInfo* createinfo))  \
    X(CreateGPUShader, SDL_GPUShader*,                                      \
      (SDL_GPUDevice* device, const SDL_GPUShaderCreateInfo* createinfo))   \
    X(CreateGPUTexture, SDL_GPUTexture*,                                    \
      (SDL_GPUDevice* device, const SDL_GPUTextureCreateInfo* createinfo))  \
    X(CreateGPUTransferBuffer, SDL_GPUTransferBuffer*,                      \
      (SDL_GPUDevice* device, const SDL_GPUTransferBufferCreateInfo* createinfo)) \
    X(DestroyGPUDevice, void, (SDL_GPUDevice* device))                      \
    X(DrawGPUIndexedPrimitives, void,                                       \
      (SDL_GPURenderPass* render_pass, Uint32 num_indices,                  \
       Uint32 num_instances, Uint32 first_index, Sint32 vertex_offset,      \
       Uint32 first_instance))                                              \
    X(EndGPUCopyPass, void, (SDL_GPUCopyPass* copy_pass))                   \
    X(EndGPURenderPass, void, (SDL_GPURenderPass* render_pass))             \
    X(GetError, const char*, (void))                                        \
    X(GetGPUShaderFormats, SDL_GPUShaderFormat, (SDL_GPUDevice* device))    \
    X(GetGPUSwapchainTextureFormat, SDL_GPUTextureFormat,                   \
      (SDL_GPUDevice* device, SDL_Window* window))                          \
    X(GetWindowSizeInPixels, bool, (SDL_Window* window, int* w, int* h))    \
    X(MapGPUTransferBuffer, void*,                                          \
      (SDL_GPUDevice* device, SDL_GPUTransferBuffer* transfer_buffer,       \
       bool cycle))                                                         \
    X(PushGPUFragmentUniformData, void,                                     \
      (SDL_GPUCommandBuffer* command_buffer, Uint32 slot_index,             \
       const void* data, Uint32 length))                                    \
    X(PushGPUVertexUniformData, void,                                       \
      (SDL_GPUCommandBuffer* command_buffer, Uint32 slot_index,             \
       const void* data, Uint32 length))                                    \
    X(ReleaseGPUBuffer, void,                                               \
      (SDL_GPUDevice* device, SDL_GPUBuffer* buffer))                       \
    X(ReleaseGPUFence, void,                                                \
      (SDL_GPUDevice* device, SDL_GPUFence* fence))                         \
    X(ReleaseGPUGraphicsPipeline, void,                                     \
      (SDL_GPUDevice* device, SDL_GPUGraphicsPipeline* graphics_pipeline))  \
    X(ReleaseGPUSampler, void,                                              \
      (SDL_GPUDevice* device, SDL_GPUSampler* sampler))                     \
    X(ReleaseGPUShader, void,                                               \
      (SDL_GPUDevice* device, SDL_GPUShader* shader))                       \
    X(ReleaseGPUTexture, void,                                              \
      (SDL_GPUDevice* device, SDL_GPUTexture* texture))                     \
    X(ReleaseGPUTransferBuffer, void,                                       \
      (SDL_GPUDevice* device, SDL_GPUTransferBuffer* transfer_buffer))      \
    X(ReleaseWindowFromGPUDevice, void,                                     \
      (SDL_GPUDevice* device, SDL_Window* window))                          \
    X(SetGPUScissor, void,                                                  \
      (SDL_GPURenderPass* render_pass, const SDL_Rect* rect))               \
    X(SetGPUViewport, void,                                                 \
      (SDL_GPURenderPass* render_pass, const SDL_GPUViewport* viewport))    \
    X(SubmitGPUCommandBufferAndAcquireFence, SDL_GPUFence*,                 \
      (SDL_GPUCommandBuffer* command_buffer))                               \
    X(UnmapGPUTransferBuffer, void,                                         \
      (SDL_GPUDevice* device, SDL_GPUTransferBuffer* transfer_buffer))      \
    X(UploadToGPUBuffer, void,                                              \
      (SDL_GPUCopyPass* copy_pass, const SDL_GPUTransferBufferLocation* source, \
       const SDL_GPUBufferRegion* destination, bool cycle))                 \
    X(UploadToGPUTexture, void,                                             \
      (SDL_GPUCopyPass* copy_pass, const SDL_GPUTextureTransferInfo* source, \
       const SDL_GPUTextureRegion* destination, bool cycle))                \
    X(WaitForGPUFences, bool,                                               \
      (SDL_GPUDevice* device, bool wait_all, SDL_GPUFence* const* fences,   \
       Uint32 num_fences))                                                  \
    X(WaitForGPUIdle, bool,                                                 \
      (SDL_GPUDevice* device))

#endif /* sdl3gpurend_syms_h */
