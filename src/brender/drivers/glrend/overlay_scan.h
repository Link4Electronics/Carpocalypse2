#ifndef GL_OVERLAY_SCAN_H
#define GL_OVERLAY_SCAN_H
#include <stddef.h>
#include <stdint.h>
#ifdef __ARM_NEON
#include <arm_neon.h>
/* Use32bit lanes: this ARM compiler spills64bit lane extraction to the stack. */
static inline int GLOverlayAnyDifference(uint16x8_t difference) {
    uint32x4_t words = vreinterpretq_u32_u16(difference);
    uint32x2_t pair = vorr_u32(vget_low_u32(words), vget_high_u32(words));
    return (vget_lane_u32(pair, 0) | vget_lane_u32(pair, 1)) != 0;
}
#endif

/* Always examine the mapped pixels: writes need not set a dirty flag. Use
 * complete cache-line-sized checks on Vita rather than one branch per pixel. */
static inline int GLOverlayAllKeyScalar(const uint16_t* pixels, size_t count, uint16_t key) {
    while (count--) if (*pixels++ != key) return 0;
    return 1;
}
static inline int GLOverlayAllKey(const uint16_t* pixels, size_t count, uint16_t key) {
#ifdef __ARM_NEON
    const uint16x8_t keys = vdupq_n_u16(key);
    /* Keep reductions in NEON across512 bytes. Reading lanes into ARM registers
     * after every64 bytes serializes the Cortex-A9 pipelines. All loads remain
     * inside the mapped buffer; the smaller loop below handles the tail. */
    while (count >= 256) {
        uint16x8_t a = vdupq_n_u16(0), b = a, c = a, d = a;
        for (unsigned block = 0; block < 8; ++block) {
            /* one 32-byte A9 cache line per prefetch, ~512 bytes ahead (PLD never faults) */
            __builtin_prefetch(pixels + 256, 0, 0);
            __builtin_prefetch(pixels + 272, 0, 0);
            a = vorrq_u16(a, veorq_u16(vld1q_u16(pixels), keys));
            b = vorrq_u16(b, veorq_u16(vld1q_u16(pixels + 8), keys));
            c = vorrq_u16(c, veorq_u16(vld1q_u16(pixels + 16), keys));
            d = vorrq_u16(d, veorq_u16(vld1q_u16(pixels + 24), keys));
            pixels += 32;
        }
        if (GLOverlayAnyDifference(vorrq_u16(vorrq_u16(a,b), vorrq_u16(c,d)))) return 0;
        count -= 256;
    }
    while (count >= 32) {
        uint16x8_t difference = vorrq_u16(veorq_u16(vld1q_u16(pixels), keys),
            veorq_u16(vld1q_u16(pixels + 8), keys));
        difference = vorrq_u16(difference, vorrq_u16(veorq_u16(vld1q_u16(pixels + 16), keys),
            veorq_u16(vld1q_u16(pixels + 24), keys)));
        if (GLOverlayAnyDifference(difference)) return 0;
        pixels += 32;
        count -= 32;
    }
#endif
    return GLOverlayAllKeyScalar(pixels, count, key);
}
static inline void GLOverlayFillKey(uint16_t* pixels, size_t count, uint16_t key) {
#ifdef __ARM_NEON
    const uint16x8_t keys = vdupq_n_u16(key);
    while (count >= 32) {
        vst1q_u16(pixels, keys); vst1q_u16(pixels + 8, keys);
        vst1q_u16(pixels + 16, keys); vst1q_u16(pixels + 24, keys);
        pixels += 32;
        count -= 32;
    }
#endif
    while (count--) *pixels++ = key;
}
#endif
