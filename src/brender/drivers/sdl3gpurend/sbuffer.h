#ifndef _SBUFFER_H_
#define _SBUFFER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>

struct br_device_pixelmap;

br_error BufferStoredSDL3GPURENDUpdate(struct br_buffer_stored* self, struct br_device_pixelmap* pm, br_token_value* tv);
br_boolean BufferStoredSDL3GPURENDReupload(struct br_buffer_stored* self);

#ifdef BR_BUFFER_STORED_PRIVATE

typedef struct br_buffer_stored {
    struct br_buffer_stored_dispatch *dispatch;
    const char *identifier;

    br_device   *device;
    br_renderer *renderer;

    br_pixelmap *source;
    br_uint_16 source_flags;

    SDL_GPUTexture* image;
    SDL_GPUSampler* sampler;

    int width;
    int height;
    br_uint_8 pixel_type;

    struct br_device_clut* clut;
    br_uint_32 palette_checksum;

    br_boolean blended;

    struct br_tv_template *templates;

    br_boolean paletted_source_dirty;
    br_uint_32 palette_revision;
    struct br_device_clut *palette_pointer;

} br_buffer_stored;

#endif

#ifdef __cplusplus
};
#endif
#endif
