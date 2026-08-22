#ifndef _PIXCONV_H_
#define _PIXCONV_H_

#include "commonrend.h"

#ifndef NO_PROTOTYPES

#ifdef __cplusplus
extern "C" {
#endif

/* Expands a memory pixelmap row layout to RGBA8888 on the CPU.
 * src:   pixel data, rows at src_row_bytes stride
 * dst:   width*height pixels, written as 4 bytes each (R,G,B,A)
 * palette: BR_PMT_INDEX_8 lookup table (must be set for that type)
 * Returns BRE_OK, or BRE_FAIL for an unsupported type or NULL palette. */
br_error BREND_FN(Pixelmap, ExpandToRGBA8888)(const void* src, int width, int height,
    int src_row_bytes, br_uint_8 type, br_uint_32* dst, const br_uint_32* palette);

#ifdef __cplusplus
};
#endif

#endif
#endif /* _PIXCONV_H_ */
