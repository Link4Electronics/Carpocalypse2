#ifndef _DEVPIXMP_H_
#define _DEVPIXMP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>

typedef struct {
    float x, y, z;
    float r, g, b;
    float u, v;
} br_device_pixelmap_sdl3_tri;

typedef struct br_device_pixelmap_sdl3_quad {
    br_device_pixelmap_sdl3_tri tris[4];
    SDL_GPUGraphicsPipeline* pipeline;
    SDL_GPUBuffer* buffer;
} br_device_pixelmap_sdl3_quad;

#ifdef BR_DEVICE_PIXELMAP_PRIVATE

#include "devpixmp_base.h"

typedef struct br_device_pixelmap {
    BR_DEVICE_PIXELMAP_BASE;

    union {
        struct {
            br_device_sdl3gpu_callback_procs callbacks;
            VIDEO video;
            void* sdl3_context;
            const char* sdl3_version;
            const char* sdl3_vendor;
            const char* sdl3_renderer;
            SDL_GPUDevice* sdl3_device;
            SDL_Window* sdl3_window;
            br_int_32 num_refs;
        } asFront;
        struct {
            struct br_device_pixelmap* depthbuffer;
            void* lockedPixels;
            int possiblyDirty;
            int locked;
        } asBack;
        struct {
            struct br_device_pixelmap* backbuffer;
        } asDepth;
    };
    struct br_device_clut* clut;
} br_device_pixelmap;

BR_DEVICE_PIXELMAP_COMMON_DECLS;

#endif

#ifdef __cplusplus
};
#endif
#endif
