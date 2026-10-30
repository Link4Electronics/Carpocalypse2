#ifndef _BRSDL3GPUREND_H_
#define _BRSDL3GPUREND_H_

#ifndef _BRENDER_H_
#error Please include brender.h first
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Platform callbacks handed to the front screen pixelmap via
 * BRT_SDL3GPU_CALLBACKS_P (the platform layer owns the window and the GPU
 * device). Mirrors the glrend callback procs.
 */
typedef void BR_CALLBACK br_device_sdl3gpu_swapbuffers_cbfn(br_pixelmap* pm);
typedef void* BR_CALLBACK br_device_sdl3gpu_getprocaddress_cbfn(const char* name);
typedef void BR_CALLBACK br_device_sdl3gpu_free_cbfn(br_pixelmap* pm, void* user);
typedef void BR_CALLBACK br_device_sdl3gpu_getviewport_cbfn(int* x, int* y, float* width_multiplier, float* height_multiplier);
typedef void BR_CALLBACK br_device_sdl3gpu_get_window_size_cbfn(int* width, int* height);
typedef void* BR_CALLBACK br_device_sdl3gpu_get_window_cbfn(void);

typedef struct br_device_sdl3gpu_callback_procs {
    br_device_sdl3gpu_getprocaddress_cbfn* get_proc_address;
    br_device_sdl3gpu_getviewport_cbfn* get_viewport;
    br_device_sdl3gpu_swapbuffers_cbfn* swap_buffers;
    br_device_sdl3gpu_free_cbfn* free;
    br_device_sdl3gpu_get_window_size_cbfn* get_window_size;
    br_device_sdl3gpu_get_window_cbfn* get_window;
    /* Handle to the already-loaded SDL3 library, used by the sdl3gpurend
     * driver to resolve its SDL3 function pointers at runtime. May be NULL;
     * linked builds ignore it. */
    void* sdl3_handle;
} br_device_sdl3gpu_callback_procs;

/*
 * SDL3 GPU device info. Pointers refer to SDL3 objects:
 *   gpu_device               -> SDL_GPUDevice*
 *   window                   -> SDL_Window*
 *   swapchain_texture_format -> SDL_GPUTextureFormat (swapchain format)
 */
typedef struct SDL3GPUREND_DeviceInfo {
    void* gpu_device;
    void* window;
    uint32_t swapchain_texture_format;
} SDL3GPUREND_DeviceInfo;

void SDL3GPUREND_GetDeviceInfo(SDL3GPUREND_DeviceInfo* info);

/*
 * External render callback, invoked inside the driver's frame while a
 * command buffer is active. The first argument is the active
 * SDL_GPUCommandBuffer*; the second is the opaque user pointer.
 */
void SDL3GPUREND_SetExternalRenderCallback(void (*cb)(void* cmd, void* ud), void* ud);

#ifdef __cplusplus
}
#endif

/*
 * Main entry point for device.
 */
#ifndef _NO_PROTOTYPES
struct br_device *BR_EXPORT BrDrv1SDL3GPURENDBegin(const char *arguments);
#endif /* _NO_PROTOTYPES */

#endif /* _BRSDL3GPUREND_H_ */
