#ifndef CARPOCALYPSE2_SDL3_SSDX_H
#define CARPOCALYPSE2_SDL3_SSDX_H

#include <s3/s3.h>

enum tSSDXFlags {
    SSDX_InitDirectSound = 0x2,
};

extern int SSDXStart(void *phWnd, int pDirectDraw, int pEnumerateDDX, int pflags);

extern void SSDXStop(void);

extern void SSDXRelease(void);

#endif /* CARPOCALYPSE2_SDL3_SSDX_H */
