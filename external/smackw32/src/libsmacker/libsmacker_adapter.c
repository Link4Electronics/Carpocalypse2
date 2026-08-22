/*
 * smackw32 compatibility layer backed by libsmacker.
 *
 * The retail game links against RAD Game Tools' proprietary smackw32.dll to
 * play its .SMK cutscene videos. This module implements the same API surface
 * (see include/smack.h) on top of the LGPL-licensed libsmacker decoder, so the
 * intro/logos/opening animations can play on platforms where RAD's DLL cannot
 * be used.
 *
 * Semantics follow how the game actually drives the API (see the cutscene
 * module): SmackToBuffer() copies the current decoded frame into a caller
 * buffer, SmackDoFrame() finalizes palette bookkeeping for the current frame,
 * SmackNextFrame() advances, and SmackWait() returns the milliseconds
 * remaining until the next frame is due.
 */

#include "smack.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

#include "smacker.h"

#define SMACK_MAX_HANDLES 4

typedef struct {
    int active;
    int end_reached;
    unsigned long pending_advance;
    smk smk_file;
    unsigned long frame_index;
    unsigned long start_tick;
} smack_internal;

static struct _SMACK gSmacks[SMACK_MAX_HANDLES];
static smack_internal gSmackInternals[SMACK_MAX_HANDLES];

static unsigned long smack_ticks(void) {
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (unsigned long)((unsigned long)ts.tv_sec * 1000ul + (unsigned long)ts.tv_nsec / 1000000ul);
}

static smack_internal* smack_internal_from_handle(HSMACK handle) {
    int i;

    for (i = 0; i < SMACK_MAX_HANDLES; i++) {
        if (handle == &gSmacks[i] && gSmackInternals[i].active) {
            return &gSmackInternals[i];
        }
    }
    return NULL;
}

/* Refresh the RAD-visible struct from the decoder state. */
static void smack_sync_frame(struct _SMACK* handle, smack_internal* si) {
    const unsigned char* pal;

    pal = smk_get_palette(si->smk_file);
    if (pal != NULL && memcmp(handle->Palette, pal, 768) != 0) {
        memcpy(handle->Palette, pal, 768);
        handle->NewPalette = 1;
    }
}

int SMACK_STDCALL SmackSoundUseMSS(SMACK_HDIGDRIVER hDrv) {
    (void)hDrv;
    /* Audio does not go through Miles here; handled by the platform layer. */
    return -1;
}

int SMACK_STDCALL SmackSoundUseDirectSound(SMACK_HWND hWnd) {
    (void)hWnd;
    /* Ditto: no DirectSound on this platform. */
    return -1;
}

unsigned int SMACK_STDCALL SmackUseMMX(unsigned int flag) {
    (void)flag;
    return 1;
}

HSMACK SMACK_STDCALL SmackOpen(const char* path, unsigned int uFlags, unsigned int uExtraBuffers) {
    FILE* f;
    smk s;
    unsigned long width, height;
    unsigned long frame_number, frame_count;
    double us_per_frame;
    unsigned char mode;
    smack_internal* si;
    struct _SMACK* handle;
    int i;

    (void)uExtraBuffers;
    f = fopen(path, "rb");
    if (f == NULL) {
        return NULL;
    }
    /* NOTE: libsmacker takes ownership of the FILE*; smk_close() closes it
     * (immediately in memory mode, on close in disk mode). */
    mode = (uFlags & SMACKPRELOADALL) ? SMK_MODE_MEMORY : SMK_MODE_DISK;
    s = smk_open_filepointer(f, mode);
    if (s == NULL) {
        return NULL;
    }

    /* Video only for now: audio playback is not wired up yet. */
    smk_enable_all(s, SMK_VIDEO_TRACK);

    for (i = 0; i < SMACK_MAX_HANDLES; i++) {
        if (!gSmackInternals[i].active) {
            break;
        }
    }
    if (i == SMACK_MAX_HANDLES) {
        smk_close(s);
        return NULL;
    }

    width = 0;
    height = 0;
    frame_count = 0;
    us_per_frame = 0.0;
    frame_number = 0;
    if (!smk_info_all(s, &frame_number, &frame_count, &us_per_frame)) {
        smk_close(s);
        return NULL;
    }
    if (!smk_info_video(s, &width, &height, NULL)) {
        smk_close(s);
        return NULL;
    }
    /* Unpack the first frame so ToBuffer has data immediately. */
    if (!smk_first(s)) {
        smk_close(s);
        return NULL;
    }

    si = &gSmackInternals[i];
    memset(si, 0, sizeof(*si));
    si->active = 1;
    si->smk_file = s;
    si->frame_index = 0;
    si->start_tick = smack_ticks();

    handle = &gSmacks[i];
    memset(handle, 0, sizeof(*handle));
    handle->Version = 2;
    handle->Width = (unsigned int)width;
    handle->Height = (unsigned int)height;
    handle->Frames = (unsigned int)frame_count;
    handle->MSPerFrame = (us_per_frame > 0.0) ? (unsigned int)(us_per_frame / 1000.0) : 67u;
    if (handle->MSPerFrame == 0) {
        handle->MSPerFrame = 1;
    }
    handle->OpenFlags = uFlags;
    handle->FrameNum = 0;
    handle->NewPalette = 1;
    smack_sync_frame(handle, si);

    return handle;
}

void SMACK_STDCALL SmackClose(HSMACK handle) {
    smack_internal* si;

    si = smack_internal_from_handle(handle);
    if (si == NULL) {
        return;
    }
    smk_close(si->smk_file);
    si->active = 0;
    si->smk_file = NULL;
}

unsigned int SMACK_STDCALL SmackDoFrame(HSMACK handle) {
    smack_internal* si;

    si = smack_internal_from_handle(handle);
    if (si == NULL) {
        return 0;
    }
    if (si->pending_advance) {
        if (smk_next(si->smk_file)) {
            si->frame_index++;
            si->pending_advance = 0;
            handle->FrameNum = (unsigned int)si->frame_index;
            smack_sync_frame(handle, si);
        } else {
            si->end_reached = 1;
        }
    }
    return 0;
}

void SMACK_STDCALL SmackNextFrame(HSMACK handle) {
    smack_internal* si;

    si = smack_internal_from_handle(handle);
    if (si != NULL) {
        si->pending_advance = 1;
    }
}

unsigned int SMACK_STDCALL SmackWait(HSMACK handle) {
    smack_internal* si;
    unsigned long now;
    unsigned long due;

    si = smack_internal_from_handle(handle);
    if (si == NULL || si->end_reached) {
        return 0;
    }
    now = smack_ticks();
    due = si->start_tick + (si->frame_index + 1) * handle->MSPerFrame;
    if (now >= due) {
        return 0;
    }
    return due - now;
}

int SMACK_STDCALL SmackGoto(HSMACK handle, long frame) {
    smack_internal* si;

    si = smack_internal_from_handle(handle);
    if (si == NULL) {
        return -1;
    }
    if (!smk_seek_keyframe(si->smk_file, (unsigned long)frame)) {
        return -1;
    }
    si->frame_index = (unsigned long)frame;
    si->end_reached = 0;
    handle->FrameNum = (unsigned int)si->frame_index;
    smack_sync_frame(handle, si);
    return 0;
}

void SMACK_STDCALL SmackToBuffer(HSMACK handle, unsigned int uX, unsigned int uY, unsigned int uPitch, unsigned int uHeight, void* pBuffer, unsigned int uFlags) {
    smack_internal* si;
    const unsigned char* src;
    unsigned char* dst;
    unsigned int row;
    unsigned int width;
    unsigned int copy_width;

    (void)uFlags;
    si = smack_internal_from_handle(handle);
    if (si == NULL || pBuffer == NULL || handle->Width == 0) {
        return;
    }
    src = smk_get_video(si->smk_file);
    if (src == NULL) {
        return;
    }
    width = handle->Width;
    copy_width = width - uX;
    if (copy_width > width) {
        return;
    }
    dst = (unsigned char*)pBuffer;
    for (row = 0; row < uHeight; row++) {
        memcpy(dst + row * uPitch, src + (uY + row) * width + uX, copy_width);
    }
}

unsigned int SMACK_STDCALL SmackToBufferRect(HSMACK handle, unsigned int uSmackSurface) {
    (void)handle;
    (void)uSmackSurface;
    return 0;
}

HSMACKBLIT SMACK_STDCALL SmackBlitOpen(unsigned int uSurfaceFormat) {
    (void)uSurfaceFormat;
    return (HSMACKBLIT)1;
}

void SMACK_STDCALL SmackBlitSetPalette(HSMACKBLIT hBlit, void* pPalette, unsigned int uPalType) {
    (void)hBlit;
    (void)pPalette;
    (void)uPalType;
}

void SMACK_STDCALL SmackBlit(HSMACKBLIT hBlit, void* pDest, unsigned int uDestPitch, unsigned int uDestX, unsigned int uDestY, void* pSrc, unsigned int uSrcPitch, unsigned int uSrcX, unsigned int uSrcY, unsigned int uSrcZ, unsigned int uSrcW) {
    (void)hBlit;
    (void)pDest;
    (void)uDestPitch;
    (void)uDestX;
    (void)uDestY;
    (void)pSrc;
    (void)uSrcPitch;
    (void)uSrcX;
    (void)uSrcY;
    (void)uSrcZ;
    (void)uSrcW;
}

void SMACK_STDCALL SmackBlitClose(HSMACKBLIT hBlit) {
    (void)hBlit;
}

int SMACK_STDCALL SmackBlitClear(HSMACKBLIT a1, unsigned short* pFrameData, unsigned int uTargetSurfacePitch, unsigned int uOutX, unsigned int uOutY, unsigned int uOutZ, unsigned int uOutW, int a8) {
    (void)a1;
    (void)pFrameData;
    (void)uTargetSurfacePitch;
    (void)uOutX;
    (void)uOutY;
    (void)uOutZ;
    (void)uOutW;
    (void)a8;
    return 0;
}

int SMACK_STDCALL SmackBufferOpen(SMACK_HWND a1, long a2, long a3, long a4, long a5, long a6) {
    (void)a1;
    (void)a2;
    (void)a3;
    (void)a4;
    (void)a5;
    (void)a6;
    /* The game only checks for failure (NULL / 0). */
    return 1;
}

void SMACK_STDCALL SmackBufferClose(HSMACKBUF a1) {
    (void)a1;
}

void SMACK_STDCALL SmackBufferNewPalette(HSMACKBUF a1, void* a2, unsigned int a3) {
    (void)a1;
    (void)a2;
    (void)a3;
}

int SMACK_FASTCALL SmackVolumePan(HSMACK a3, long a4, long a5, long a6) {
    (void)a3;
    (void)a4;
    (void)a5;
    (void)a6;
    return 0;
}

unsigned int SMACK_STDCALL SmackSoundOnOff(HSMACK handle, unsigned int bOn) {
    smack_internal* si;

    si = smack_internal_from_handle(handle);
    if (si == NULL) {
        return 0;
    }
    /* Enable/disable audio tracks once audio playback exists. */
    smk_enable_audio(si->smk_file, 0, bOn ? 1 : 0);
    return bOn;
}

void SMACK_STDCALL SmackColorRemapWithTrans(HSMACK a1, void* a2, unsigned int a3, unsigned int a4, unsigned int a5) {
    (void)a1;
    (void)a2;
    (void)a3;
    (void)a4;
    (void)a5;
}
