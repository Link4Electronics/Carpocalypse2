/*
 * Shared br_pixelmap -> RGBA8888 CPU conversion (glrend/sdl3gpurend).
 *
 * The SDL3 backend feeds the fragment sampler, which samples R8G8B8A8_UNORM,
 * so every source format is expanded here. The GL backend keeps its native
 * format uploads (VIDEOI_BrPixelmapGetTypeDetails) and only uses this for the
 * paletted (INDEX_8) just-in-time texture path, which already expanded to
 * RGBA8888.
 *
 * The RGB_565/555 expansion matches the GL shader sampling of those formats
 * (R5G6B5/R5G5B5 layout + 8-bit colour components). The palette path writes
 * R,G,B,A bytes from a 32-bit BR_COLOUR entry with alpha forced to 0xFF.
 */
#include <string.h>

#include "brassert.h"
#include "drv.h"
#include "commonrend.h"
#include "pixconv.h"

br_error BREND_FN(Pixelmap, ExpandToRGBA8888)(const void* src, int width, int height,
    int src_row_bytes, br_uint_8 type, br_uint_32* dst, const br_uint_32* palette) {
    if (src == NULL || dst == NULL || width <= 0 || height <= 0)
        return BRE_FAIL;

    const char* base = (const char*)src;

    if (type == BR_PMT_INDEX_8) {
        if (palette == NULL)
            return BRE_FAIL;

        for (int y = 0; y < height; y++) {
            const br_uint_8* row = (const br_uint_8*)(base + y * src_row_bytes);
            for (int x = 0; x < width; x++) {
                br_uint_32 entry = palette[row[x]];
                uint8_t* d = (uint8_t*)&dst[y * width + x];
                d[0] = (uint8_t)BR_RED(entry);
                d[1] = (uint8_t)BR_GRN(entry);
                d[2] = (uint8_t)BR_BLU(entry);
                d[3] = 0xFF;
            }
        }
        return BRE_OK;
    }

    if (type == BR_PMT_RGB_565 || type == BR_PMT_RGB_555) {
        for (int y = 0; y < height; y++) {
            const br_uint_16* row = (const br_uint_16*)(base + y * src_row_bytes);
            for (int x = 0; x < width; x++) {
                br_uint_16 p = row[x];
                uint8_t* d = (uint8_t*)&dst[y * width + x];
                if (type == BR_PMT_RGB_565) {
                    d[0] = (uint8_t)(((p >> 11) & 0x1F) << 3);
                    d[1] = (uint8_t)(((p >> 5) & 0x3F) << 2);
                    d[2] = (uint8_t)((p & 0x1F) << 3);
                } else {
                    d[0] = (uint8_t)(((p >> 10) & 0x1F) << 3);
                    d[1] = (uint8_t)(((p >> 5) & 0x1F) << 3);
                    d[2] = (uint8_t)((p & 0x1F) << 3);
                }
                d[3] = 0xFF;
            }
        }
        return BRE_OK;
    }

    if (type == BR_PMT_RGB_888) {
        for (int y = 0; y < height; y++) {
            const char* row = base + y * src_row_bytes;
            uint8_t* d = (uint8_t*)dst + (size_t)y * width * 4;
            for (int x = 0; x < width; x++) {
                d[x * 4 + 0] = (uint8_t)row[x * 3 + 0];
                d[x * 4 + 1] = (uint8_t)row[x * 3 + 1];
                d[x * 4 + 2] = (uint8_t)row[x * 3 + 2];
                d[x * 4 + 3] = 0xFF;
            }
        }
        return BRE_OK;
    }

    if (type == BR_PMT_RGBA_8888 || type == BR_PMT_RGBX_888) {
        for (int y = 0; y < height; y++) {
            const char* row = base + y * src_row_bytes;
            memcpy((uint8_t*)dst + (size_t)y * width * 4, row, (size_t)width * 4);
        }
        return BRE_OK;
    }

    return BRE_FAIL;
}
