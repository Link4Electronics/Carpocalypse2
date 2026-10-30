/*
 * Stored buffer methods (SDL3 GPU)
 *
 * Textures are always uploaded as RGBA8888: the SDL3 GPU backend only needs to
 * feed the fragment sampler, and the shared GLSL samples R8G8B8A8_UNORM, so
 * every source format (INDEX_8, RGB_565/555/888, RGBA/RGBX_8888) is converted
 * to RGBA8888 on the CPU before SDL3GPUREND_UploadBufferToImage.
 *
 * Offscreen device pixelmaps have no GPU image of their own — their pixels live
 * in the shared CPU locked buffer (hVideo->lockedPixels, see devpixmp.c). The
 * texture is created lazily from that region on first use (BufferStoredSDL3GPUREND
 * Reupload), which is what supports the pratcam quad sampling an offscreen
 * render target.
 */
#include <stddef.h>
#include <string.h>

#include "brassert.h"
#include "drv.h"
#include "pixconv.h"

/* Backend-specific init for the fields written by BufferStoredSDL3GPURENDUpdate
 * (see commonrend/sbuffer_common.c allocate). */
void BufferStoredSDL3GPURENDInitFields(struct br_buffer_stored* self) {
    self->image = NULL;
    self->sampler = NULL;
    self->width = 0;
    self->height = 0;
    self->pixel_type = 0;
    self->blended = BR_FALSE;
}

/* Expands the region of the shared CPU locked buffer that backs an offscreen
 * device pixelmap into RGBA8888. The write path (devpixmp.c rectangleCopyFrom
 * and the renderer flush) lays the pixels out at (pm_base_x, pm_base_y) using
 * the offscreen's own pm_row_bytes as the row stride, so the reads mirror that
 * exactly. */
static void lockedRegionToRGBA(br_device_pixelmap* pm, const void* lockedPixels, uint32_t* dst) {
    int w = pm->pm_width;
    int h = pm->pm_height;
    int bpp;
    switch (pm->pm_type) {
    case BR_PMT_RGB_565:
    case BR_PMT_RGB_555:
    case BR_PMT_RGBA_4444:
        bpp = 2;
        break;
    default:
        bpp = 4;
        break;
    }

    const char* base = (const char*)lockedPixels + pm->pm_base_y * pm->pm_row_bytes + pm->pm_base_x * bpp;

    br_uint_8 type = (bpp == 2) ? pm->pm_type : BR_PMT_RGBA_8888;
    BREND_FN(Pixelmap, ExpandToRGBA8888)(base, w, h, pm->pm_row_bytes, type, dst, NULL);
}

static HVIDEO BufferStoredVideo(struct br_buffer_stored* self) {
    br_device_pixelmap* screen = (br_device_pixelmap*)self->renderer->pixelmap->screen;
    return &screen->asFront.video;
}

/* Mirrors glrend's `blended` classification: an RGBA texture whose alpha is
 * not fully opaque must go through the blended pipeline. Without this the
 * car glass (ARGB4444) is drawn with the opaque pipeline and appears solid. */
static br_boolean rgbaHasAlpha(const br_uint_32* rgba, size_t count) {
    const uint8_t* p = (const uint8_t*)rgba;
    size_t i;

    for (i = 0; i < count; i++) {
        if (p[i * 4 + 3] != 0xFF)
            return BR_TRUE;
    }
    return BR_FALSE;
}

static br_boolean pixelTypeHasAlpha(br_uint_8 type) {
    return type == BR_PMT_RGBA_4444 || type == BR_PMT_RGBA_8888 || type == BR_PMT_ARGB_4444;
}

/* (Re)creates the GPU texture + sampler for `rgba` and uploads it. */
static br_error uploadRGBA(struct br_buffer_stored* self, HVIDEO hVideo, const void* rgba, int w, int h) {
    if (self->image) {
        SDL3GPUREND_DeferFreeImage(hVideo, self->image, self->sampler);
        self->image = NULL;
        self->sampler = NULL;
    }

    SDL_GPUTextureCreateInfo ti = {0};
    ti.type = SDL_GPU_TEXTURETYPE_2D;
    ti.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
    ti.width = w;
    ti.height = h;
    ti.layer_count_or_depth = 1;
    ti.num_levels = 1;
    ti.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER;

    self->image = SDL3_CreateGPUTexture(hVideo->device, &ti);
    if (!self->image)
        return BRE_FAIL;

    if (SDL3GPUREND_UploadBufferToImage(hVideo, self->image, w, h, 0, 0, rgba, (size_t)w * h * 4) != 0)
        return BRE_FAIL;

    SDL_GPUSamplerCreateInfo si = {0};
    si.min_filter = SDL_GPU_FILTER_LINEAR;
    si.mag_filter = SDL_GPU_FILTER_LINEAR;
    si.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR;
    si.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
    si.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
    si.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;

    self->sampler = SDL3_CreateGPUSampler(hVideo->device, &si);
    if (!self->sampler)
        return BRE_FAIL;

    self->width = w;
    self->height = h;
    return BRE_OK;
}

static br_error updateMemory(struct br_buffer_stored* self, br_pixelmap* pm) {
    HVIDEO hVideo = BufferStoredVideo(self);
    int w = pm->width;
    int h = pm->height;
    size_t rgbaSize = (size_t)w * h * 4;

    if ((pm->flags & BR_PMF_NO_ACCESS) || pm->pixels == NULL)
        return BRE_FAIL;

    br_uint_32* rgba = BrScratchAllocate(rgbaSize);

    br_uint_32* palette = NULL;
    if (pm->type == BR_PMT_INDEX_8) {
        br_device* dev_obj = ObjectDevice(self);
        if (dev_obj && dev_obj->clut) {
            self->palette_pointer = dev_obj->clut;
            self->palette_revision = dev_obj->clut->revision;
        } else {
            self->palette_pointer = NULL;
            self->palette_revision = 0;
        }

        if (dev_obj && dev_obj->clut && dev_obj->clut->entries)
            palette = dev_obj->clut->entries;
        else if (pm->map && pm->map->pixels)
            palette = (br_uint_32*)pm->map->pixels;
    }

    if (BREND_FN(Pixelmap, ExpandToRGBA8888)(pm->pixels, w, h, pm->row_bytes, pm->type, rgba, palette) != BRE_OK) {
        BrScratchFree(rgba);
        return BRE_FAIL;
    }

    br_error r = uploadRGBA(self, hVideo, rgba, w, h);

    if (r == BRE_OK)
        self->blended = rgbaHasAlpha((const br_uint_32*)rgba, (size_t)w * h);

    BrScratchFree(rgba);

    if (r != BRE_OK)
        return r;

    self->source = pm;
    self->source_flags = pm->flags;
    self->pixel_type = pm->type;
    self->paletted_source_dirty = BR_FALSE;
    if (pm->type == BR_PMT_INDEX_8)
        self->paletted_source_dirty = BR_TRUE;

    return BRE_OK;
}

br_error BufferStoredSDL3GPURENDUpdate(struct br_buffer_stored* self, struct br_device_pixelmap* pm, br_token_value* tv) {
    br_device* pm_device;
    (void)tv;

    if (!pm)
        return BRE_FAIL;

    if (pm->pm_type != BR_PMT_INDEX_8 && pm->pm_type != BR_PMT_RGB_565 && pm->pm_type != BR_PMT_RGB_555 &&
        pm->pm_type != BR_PMT_RGB_888 && pm->pm_type != BR_PMT_RGBA_8888 && pm->pm_type != BR_PMT_RGBX_888 &&
        pm->pm_type != BR_PMT_RGBA_4444)
        return BRE_FAIL;

    /*
     * Find out where the pixelmap comes from
     */
    pm_device = ObjectDevice(pm);
    if (pm_device == NULL) {
        return updateMemory(self, (br_pixelmap*)pm);
    } else if (pm_device == self->device) {
        /*
         * Device pixelmap (offscreen render target): keep the source. The GPU
         * texture is created lazily from hVideo->lockedPixels on first use.
         */
        ASSERT(self->source == NULL || self->source == (br_pixelmap*)pm);
        self->source = (br_pixelmap*)pm;
        self->source_flags = pm->pm_flags;
        self->width = pm->pm_width;
        self->height = pm->pm_height;
        self->pixel_type = pm->pm_type;
        self->blended = pixelTypeHasAlpha(pm->pm_type);
        return BRE_OK;
    } else {
        /*
         * The pixelmap is from another device, we can't use it
         */
        return BRE_FAIL;
    }
}

br_boolean BufferStoredSDL3GPURENDReupload(struct br_buffer_stored* self) {
    if (!self || !self->source)
        return BR_FALSE;

    br_pixelmap* pm = self->source;
    br_device* dev_obj = ObjectDevice(self);

    /* Device pixelmap source: pixels live in the shared CPU locked buffer. */
    if (ObjectDevice(pm) == self->device) {
        br_device_pixelmap* dpm = (br_device_pixelmap*)pm;
        if (dpm->use_type == BRT_OFFSCREEN) {
            HVIDEO hVideo = &dpm->screen->asFront.video;
            if (!hVideo->lockedPixels)
                return BR_TRUE;
            if (self->width <= 0 || self->height <= 0)
                return BR_TRUE;

            br_uint_32* rgba = BrScratchAllocate((size_t)self->width * self->height * 4);
            lockedRegionToRGBA(dpm, hVideo->lockedPixels, rgba);
            uploadRGBA(self, hVideo, rgba, self->width, self->height);
            BrScratchFree(rgba);

            self->paletted_source_dirty = BR_FALSE;
            return BR_TRUE;
        }
        return BR_TRUE;
    }

    if (pm->type != BR_PMT_INDEX_8)
        return BR_FALSE;

    if (dev_obj && dev_obj->clut && self->palette_pointer && self->palette_revision != dev_obj->clut->revision)
        self->paletted_source_dirty = BR_TRUE;

    if (self->paletted_source_dirty != BR_TRUE)
        return BR_TRUE;

    HVIDEO hVideo = BufferStoredVideo(self);

    br_uint_32* palette = NULL;
    if (dev_obj && dev_obj->clut && dev_obj->clut->entries)
        palette = dev_obj->clut->entries;
    else if (pm->map && pm->map->pixels)
        palette = (br_uint_32*)pm->map->pixels;
    if (!palette)
        return BR_FALSE;

    int w = self->width;
    int h = self->height;

    br_uint_32* rgba = BrScratchAllocate((size_t)w * h * 4);
    if (BREND_FN(Pixelmap, ExpandToRGBA8888)(pm->pixels, w, h, pm->row_bytes, pm->type, rgba, palette) != BRE_OK) {
        BrScratchFree(rgba);
        return BR_FALSE;
    }

    br_error r = uploadRGBA(self, hVideo, rgba, w, h);

    BrScratchFree(rgba);

    if (r != BRE_OK)
        return BR_FALSE;

    self->paletted_source_dirty = BR_FALSE;
    if (dev_obj && dev_obj->clut)
        self->palette_revision = dev_obj->clut->revision;

    return BR_TRUE;
}

br_error BREND_CMETHOD_DECL(BREND_CLASS(br_buffer_stored_), update)(struct br_buffer_stored* self,
    struct br_device_pixelmap* pm, br_token_value* tv) {
    return BufferStoredSDL3GPURENDUpdate(self, pm, tv);
}

void BREND_CMETHOD_DECL(BREND_CLASS(br_buffer_stored_), free)(br_object* _self) {
    br_buffer_stored* self = (br_buffer_stored*)_self;

    ObjectContainerRemove(self->renderer, (br_object*)self);

    if (self->image) {
        SDL3GPUREND_DeferFreeImage(BufferStoredVideo(self), self->image, self->sampler);
        self->image = NULL;
        self->sampler = NULL;
    }

    BrResFreeNoCallback(self);
}

