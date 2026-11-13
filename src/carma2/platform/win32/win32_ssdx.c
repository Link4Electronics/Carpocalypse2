#include "win32_ssdx.h"

#include "win32.h"
#include "win32_sound.h"

#include "errors.h"
#include "globvars.h"
#include "graphics1.h"

#include "platform.h"

#include <windows.h>
#include <ddraw.h>
#include <dsound.h>

#include "c2_stdlib.h"
#include "c2_string.h"

#include "carpocalypse2_macros.h"

// GLOBAL: CARMA2_HW 0x006aa9e0
int gWindowed;

// GLOBAL: CARMA2_HW 0x006aaa08
HWND gHWnd_SSDX;

// GLOBAL: CARMA2_HW 0x006aa9d8
int gEnumerate_DirectX_surfaces;


// GLOBAL: CARMA2_HW 0x006aa5c8
RECT gSSDXRect;

// GLOBAL: CARMA2_HW 0x006aa9f8
LPDIRECTDRAWSURFACE gPrimary_surface;

// GLOBAL: CARMA2_HW 0x006aa9fc
LPDIRECTDRAWSURFACE gAttached_surface;

// GLOBAL: CARMA2_HW 0x006aaa00
LPDIRECTDRAWCLIPPER gDirectDrawClipper;

// GLOBAL: CARMA2_HW 0x006aaa14
LPDIRECTDRAW gDirectDraw;

// GLOBAL: CARMA2_HW 0x006aaa18
LPDIRECTDRAW2 gDirectDraw2;

// GLOBAL: CARMA2_HW 0x006aaa04
LPDIRECTDRAWPALETTE gDirectDrawPalette;

// GLOBAL: CARMA2_HW 0x006aa5d8
PALETTEENTRY gSSDX_system_palette[256];

// GLOBAL: CARMA2_HW 0x006aa9dc
LPRECT gSSDXLockedRect;

// GLOBAL: CARMA2_HW 0x006aaa0c
int gAttached_surface_locked;

// GLOBAL: CARMA2_HW 0x006aa9e8
RECT gSSDXWindowPos;

// GLOBAL: CARMA2_HW 0x007a0540
MCI_OPEN_PARMS mci_open_params;

// GLOBAL: CARMA2_HW 0x007a0530
MCI_PLAY_PARMS mci_play_parms;

// GLOBAL: CARMA2_HW 0x007a04e0
MCI_PLAY_PARMS gPDS3_cda_play_parms;

// GLOBAL: CARMA2_HW 0x006aaa10
int gSSDXPitch;


#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_CDECL dr_dprintf();
extern char gSoundType_choices[];
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern void* C2_HOOK_FASTCALL SSDXLockAttachedSurface(void);
extern void C2_HOOK_FASTCALL SSDXLogError(HRESULT hRes);
#endif
// FUNCTION: CARMA2_HW 0x00500070
HRESULT CALLBACK LocalEnumAttachedSurfacesCallback(LPDIRECTDRAWSURFACE lpSurface, LPDDSURFACEDESC lpSurfaceDesc, LPVOID lpContext) {
    int* counter = lpContext;
    dr_dprintf("LocalEnumAttachedSurfacesCallback(): Enum-ing attached surface #%d, address 0x%p", *counter, lpSurface);
    gAttached_surface = lpSurface;
    return DDENUMRET_OK;
}

// FUNCTION: CARMA2_HW 0x005000a0
void C2_NAKED C2_HOOK_FASTCALL LocalWindowedDDSetup(int pWidth, int pHeight, int* pPitch) {
#ifndef CARPOCALYPSE2_MATCHING
    DDSURFACEDESC desc;
    HRESULT res;
    HDC hdc;
    int i;

    SetRect(&gSSDXRect, 0, 0, pWidth, pHeight);

    memset(&desc, 0, sizeof(desc));
    C2_HOOK_BUG_ON(sizeof(desc) != 0x6c);
    desc.dwSize = sizeof(desc);
    desc.dwFlags = DDSD_CAPS;
    desc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
    dr_dprintf("LocalWindowedDDSetup(): Calling CreateSurface() for primary surface...");
    res = IDirectDraw2_CreateSurface(gDirectDraw, &desc, &gPrimary_surface, NULL);
    if (res != DD_OK) {
        SSDXLogError(res);
        goto post_creation;
    }

    memset(&desc, 0, sizeof(desc));
    desc.dwSize = sizeof(desc);
    desc.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
    desc.dwHeight = pHeight;
    desc.dwWidth = pWidth;
    desc.ddsCaps.dwCaps = DDSCAPS_3DDEVICE | DDSCAPS_PALETTE | DDSCAPS_FLIP | DDSCAPS_ALPHA;
    dr_dprintf("LocalWindowedDDSetup(): Calling CreateSurface() for attached surface...");
    res = IDirectDraw_CreateSurface(gDirectDraw, &desc, &gAttached_surface, NULL);
    if (res != DD_OK) {
        SSDXLogError(res);
        goto post_creation;
    }

    res = IDirectDraw_CreateClipper(gDirectDraw, 0, &gDirectDrawClipper, NULL);
    if (res != DD_OK) {
        SSDXLogError(res);
        goto post_creation;
    }

    res = IDirectDrawClipper_SetHWnd(gDirectDrawClipper, 0, gHWnd_SSDX);
    if (res != DD_OK) {
        SSDXLogError(res);
        goto post_creation;
    }

    hdc = GetDC(gHWnd_SSDX);
    GetSystemPaletteEntries(hdc, 0, CARPOCALYPSE2_ASIZE(gSSDX_system_palette), gSSDX_system_palette);
    ReleaseDC(gHWnd_SSDX, hdc);
    for (i = 0; i < CARPOCALYPSE2_ASIZE(gSSDX_system_palette); i++) {
        if (i > 10 && i < 245) {
            gSSDX_system_palette[i].peFlags = 0;
        } else {
            gSSDX_system_palette[i].peFlags = PC_NOCOLLAPSE;
        }
    }
    dr_dprintf("LocalWindowedDDSetup(): Calling CreatePalette()...");
    res = IDirectDraw_CreatePalette(gDirectDraw, DDPCAPS_ALLOW256 | DDPCAPS_8BIT, gSSDX_system_palette, &gDirectDrawPalette, NULL);
    if (res != DD_OK) {
        SSDXLogError(res);
        goto post_creation;
    }

    dr_dprintf("LocalWindowedDDSetup(): Calling SetPalette()...");
    res = IDirectDrawSurface_SetPalette(gPrimary_surface, gDirectDrawPalette);
    if (res != DD_OK) {
        SSDXLogError(res);
        goto post_creation;
    }
post_creation:
    SSDXLockAttachedSurface();
    if (gAttached_surface_locked) {
        res = IDirectDrawSurface_Unlock(gAttached_surface, gSSDXLockedRect);
        if (res == DD_OK) {
            gAttached_surface_locked = 0;
        } else {
            dr_dprintf("SSDXUnlockAttachedSurface(): Problem while unlocking attached surface...");
            SSDXLogError(res);
        }
    }
    *pPitch = gSSDXPitch;
#else
    __asm {
            sub esp, 0x6c
            push ebx
            push ebp
            push esi
            mov ebx, edx
            push edi
            mov ebp, ecx
            push ebx
            push ebp
            push 0
            push 0
            push offset gSSDXRect
            call dword ptr ds:[0x5891cc]
            mov ecx, 0x1b
            xor eax, eax
            lea edi, [esp + 0x10]
            push 0x660624
            rep stosd
            mov dword ptr [esp + 0x14], 0x6c
            mov dword ptr [esp + 0x18], 1
            mov dword ptr [esp + 0x7c], 0x200
            call dr_dprintf
            mov eax, dword ptr ds:[gDirectDraw]
            add esp, 4
            lea edx, [esp + 0x10]
            mov ecx, dword ptr [eax]
            push 0
            push offset gPrimary_surface
            push edx
            push eax
            call dword ptr [ecx + 0x18]
            mov esi, eax
            test esi, esi
            jne L00500261
            mov ecx, 0x1b
            lea edi, [esp + 0x10]
            rep stosd
            push 0x6605dc
            mov dword ptr [esp + 0x14], 0x6c
            mov dword ptr [esp + 0x18], 7
            mov dword ptr [esp + 0x7c], 0x840
            mov dword ptr [esp + 0x20], ebp
            mov dword ptr [esp + 0x1c], ebx
            call dr_dprintf
            mov eax, dword ptr ds:[gDirectDraw]
            add esp, 4
            lea edx, [esp + 0x10]
            mov ecx, dword ptr [eax]
            push esi
            push offset gAttached_surface
            push edx
            push eax
            call dword ptr [ecx + 0x18]
            mov esi, eax
            test esi, esi
            jne L00500261
            mov eax, dword ptr ds:[gDirectDraw]
            push esi
            push offset gDirectDrawClipper
            push esi
            mov ecx, dword ptr [eax]
            push eax
            call dword ptr [ecx + 0x10]
            mov esi, eax
            test esi, esi
            jne L00500261
            mov eax, dword ptr ds:[gDirectDrawClipper]
            mov ecx, dword ptr ds:[gHWnd_SSDX]
            push ecx
            push esi
            mov edx, dword ptr [eax]
            push eax
            call dword ptr [edx + 0x20]
            mov esi, eax
            test esi, esi
            jne L00500261
            mov eax, dword ptr ds:[gPrimary_surface]
            mov ecx, dword ptr ds:[gDirectDrawClipper]
            push ecx
            push eax
            mov edx, dword ptr [eax]
            call dword ptr [edx + 0x70]
            mov esi, eax
            test esi, esi
            jne L00500261
            mov edx, dword ptr ds:[gHWnd_SSDX]
            push edx
            call dword ptr ds:[0x5891c0]
            mov esi, eax
            push offset gSSDX_system_palette
            push 0x100
            push 0
            push esi
            call dword ptr ds:[0x589024]
            mov eax, dword ptr ds:[gHWnd_SSDX]
            push esi
            push eax
            call dword ptr ds:[0x5891b4]
            mov eax, 0x6aa5db
    L005001f0:
            cmp eax, 0x6aa603
            jl L00500203
            cmp eax, 0x6aa9af
            jg L00500203
            mov byte ptr [eax], 4
            jmp L00500206
    L00500203:
            mov byte ptr [eax], 0
    L00500206:
            add eax, 4
            cmp eax, 0x6aa9db
            jl L005001f0
            push 0x6605a8
            call dr_dprintf
            mov eax, dword ptr ds:[gDirectDraw]
            add esp, 4
            mov ecx, dword ptr [eax]
            push 0
            push offset gDirectDrawPalette
            push offset gSSDX_system_palette
            push 0x44
            push eax
            call dword ptr [ecx + 0x14]
            mov esi, eax
            test esi, esi
            jne L00500261
            push 0x660578
            call dr_dprintf
            mov eax, dword ptr ds:[gPrimary_surface]
            mov ecx, dword ptr ds:[gDirectDrawPalette]
            add esp, 4
            mov edx, dword ptr [eax]
            push ecx
            push eax
            call dword ptr [edx + 0x7c]
            mov esi, eax
            test esi, esi
            je L00500268
    L00500261:
            mov ecx, esi
            call SSDXLogError
    L00500268:
            call SSDXLockAttachedSurface
            mov eax, dword ptr ds:[gAttached_surface_locked]
            test eax, eax
            je L005002ac
            mov eax, dword ptr ds:[gAttached_surface]
            mov ecx, dword ptr ds:[gSSDXLockedRect]
            push ecx
            push eax
            mov edx, dword ptr [eax]
            call dword ptr [edx + 0x80]
            mov edi, eax
            test edi, edi
            jne L00500298
            mov dword ptr ds:[gAttached_surface_locked], eax
            jmp L005002ac
    L00500298:
            push 0x66052c
            call dr_dprintf
            add esp, 4
            mov ecx, edi
            call SSDXLogError
    L005002ac:
            mov edx, dword ptr [esp + 0x80]
            mov eax, dword ptr ds:[gSSDXPitch]
            pop edi
            mov dword ptr [edx], eax
            mov eax, esi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x6c
            ret 4
    }
#endif
}

// FUNCTION: CARMA2_HW 0x005002d0
void C2_NAKED C2_HOOK_FASTCALL SSDXLogError(HRESULT hRes) {
#ifndef CARPOCALYPSE2_MATCHING

#define LOG_CASE_DDERR(V) case V: dr_dprintf("%s (%x)", #V, hRes); return
    switch (hRes) {
    LOG_CASE_DDERR(DDERR_UNSUPPORTED);
    LOG_CASE_DDERR(DDERR_GENERIC);
    LOG_CASE_DDERR(DDERR_OUTOFMEMORY);
    LOG_CASE_DDERR(DDERR_INVALIDPARAMS);
    LOG_CASE_DDERR(DDERR_INCOMPATIBLEPRIMARY);
    LOG_CASE_DDERR(DDERR_INVALIDCAPS);
    LOG_CASE_DDERR(DDERR_INVALIDMODE);
    LOG_CASE_DDERR(DDERR_INVALIDOBJECT);
    LOG_CASE_DDERR(DDERR_INVALIDPIXELFORMAT);
    LOG_CASE_DDERR(DDERR_INVALIDRECT);
    LOG_CASE_DDERR(DDERR_NOEXCLUSIVEMODE);
    LOG_CASE_DDERR(DDERR_SURFACEBUSY);
    LOG_CASE_DDERR(DDERR_SURFACELOST);
    LOG_CASE_DDERR(DDERR_WASSTILLDRAWING);
    default: dr_dprintf("UNKNOWN DD ERROR (%x) N.B. Look for this number in 'ddraw.h': %d", hRes, HRESULT_CODE(hRes)); return;
    LOG_CASE_DDERR(DD_OK);
    LOG_CASE_DDERR(DDERR_IMPLICITLYCREATED);
    LOG_CASE_DDERR(DDERR_WRONGMODE);
    }
#undef LOG_CASE_DDERR
#else
    __asm {
            cmp ecx, 0x80004005
            jg L00500310
            je L005002fb
            cmp ecx, 0x80004001
            jne L0050049c
            mov eax, 0x660800
            push ecx
            push eax
            push 0x6607f8
            call dr_dprintf
            add esp, 0xc
            ret
    L005002fb:
            mov eax, 0x6607e8
            push ecx
            push eax
            push 0x6607f8
            call dr_dprintf
            add esp, 0xc
            ret
    L00500310:
            cmp ecx, 0x80070057
            jg L00500350
            je L0050033b
            cmp ecx, 0x8007000e
            jne L0050049c
            mov eax, 0x6607d4
            push ecx
            push eax
            push 0x6607f8
            call dr_dprintf
            add esp, 0xc
            ret
    L0050033b:
            mov eax, 0x6607c0
            push ecx
            push eax
            push 0x6607f8
            call dr_dprintf
            add esp, 0xc
            ret
    L00500350:
            cmp ecx, 0x88760064
            jg L00500390
            je L0050037b
            cmp ecx, 0x8876005f
            jne L0050049c
            mov eax, 0x6607a4
            push ecx
            push eax
            push 0x6607f8
            call dr_dprintf
            add esp, 0xc
            ret
    L0050037b:
            mov eax, 0x660790
            push ecx
            push eax
            push 0x6607f8
            call dr_dprintf
            add esp, 0xc
            ret
    L00500390:
            cmp ecx, 0x88760082
            jg L005003d0
            je L005003bb
            cmp ecx, 0x88760078
            jne L0050049c
            mov eax, 0x66077c
            push ecx
            push eax
            push 0x6607f8
            call dr_dprintf
            add esp, 0xc
            ret
    L005003bb:
            mov eax, 0x660768
            push ecx
            push eax
            push 0x6607f8
            call dr_dprintf
            add esp, 0xc
            ret
    L005003d0:
            cmp ecx, 0x88760096
            jg L00500410
            je L005003fb
            cmp ecx, 0x88760091
            jne L0050049c
            mov eax, 0x66074c
            push ecx
            push eax
            push 0x6607f8
            call dr_dprintf
            add esp, 0xc
            ret
    L005003fb:
            mov eax, 0x660738
            push ecx
            push eax
            push 0x6607f8
            call dr_dprintf
            add esp, 0xc
            ret
    L00500410:
            cmp ecx, 0x887601ae
            jg L0050044c
            je L00500437
            cmp ecx, 0x887600e1
            jne L0050049c
            mov eax, 0x660720
            push ecx
            push eax
            push 0x6607f8
            call dr_dprintf
            add esp, 0xc
            ret
    L00500437:
            mov eax, 0x66070c
            push ecx
            push eax
            push 0x6607f8
            call dr_dprintf
            add esp, 0xc
            ret
    L0050044c:
            cmp ecx, 0x8876021c
            jg L00500488
            je L00500473
            cmp ecx, 0x887601c2
            jne L0050049c
            mov eax, 0x6606f8
            push ecx
            push eax
            push 0x6607f8
            call dr_dprintf
            add esp, 0xc
            ret
    L00500473:
            mov eax, 0x6606e0
            push ecx
            push eax
            push 0x6607f8
            call dr_dprintf
            add esp, 0xc
            ret
    L00500488:
            cmp ecx, 0x8876024b
            je L005004dd
            cmp ecx, 0x8876024c
            je L005004c8
            test ecx, ecx
            je L005004b3
    L0050049c:
            mov eax, ecx
            and eax, 0xffff
            push eax
            push ecx
            push 0x66069c
            call dr_dprintf
            add esp, 0xc
            ret
    L005004b3:
            mov eax, 0x660694
            push ecx
            push eax
            push 0x6607f8
            call dr_dprintf
            add esp, 0xc
            ret
    L005004c8:
            mov eax, 0x66067c
            push ecx
            push eax
            push 0x6607f8
            call dr_dprintf
            add esp, 0xc
            ret
    L005004dd:
            mov eax, 0x66066c
            push ecx
            push eax
            push 0x6607f8
            call dr_dprintf
            add esp, 0xc
            ret
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00500500
int C2_HOOK_FASTCALL SSDXStart(void *p_hWnd, int p_DirectDraw, int p_EnumerateDDX, int p_flags) {
    HRESULT hRes;

    hRes = 0;
    dr_dprintf("SSDXStart(): START...");
    gWindowed = p_DirectDraw;
    if (p_EnumerateDDX) {
        gEnumerate_DirectX_surfaces = 1;
        if (p_DirectDraw) {
            gEnumerate_DirectX_surfaces = 0;
        }
    } else {
        gEnumerate_DirectX_surfaces = 0;
    }
    gHWnd_SSDX = p_hWnd;
    if (p_flags & 0x2) {
        dr_dprintf("SSDXStart(): Calling DirectSoundCreate()...");
#ifdef DSOUND_ENABLED
        hRes = DirectSoundCreate(0, &gDirectSound, NULL);
#else
        hRes = DSERR_GENERIC;
#endif
    }
    if (hRes != DS_OK) {
        SSDXLogError(hRes);
    }
    dr_dprintf("SSDXStart(): END.");
    return (int)hRes;
}

// FUNCTION: CARMA2_HW 0x00500590
void C2_HOOK_FASTCALL SSDXStop(void) {

    SSDXUnlockAttachedSurface();

    if (gDirectDrawPalette != NULL) {

        dr_dprintf("SSDXStop(): Releasing palette...");
        IDirectDrawPalette_Release(gDirectDrawPalette);
        gDirectDrawPalette = NULL;
    }

    if (gDirectDrawClipper != NULL) {

        dr_dprintf("SSDXStop(): Releasing clipper...");
        IDirectDrawClipper_Release(gDirectDrawClipper);
        gDirectDrawClipper = NULL;
    }

    if (gWindowed) {

        if (gAttached_surface != NULL) {
            dr_dprintf("SSDXStop(): Releasing attached surface...");
            IDirectDrawSurface_Release(gAttached_surface);
            gAttached_surface = NULL;
        }
        if (gPrimary_surface != NULL) {
            dr_dprintf("SSDXStop(): Releasing primary surface...");
            IDirectDrawSurface_Release(gPrimary_surface);
            gPrimary_surface = NULL;
        }
    } else {
        dr_dprintf("SSDXStop(): Restoring display mode");
        if (gDirectDraw2 != NULL) {
            IDirectDraw2_RestoreDisplayMode(gDirectDraw2);
        }
        if (gPrimary_surface != NULL) {
            dr_dprintf("SSDXStop(): Releasing primary surface...");
            IDirectDrawSurface_Release(gPrimary_surface);
            gAttached_surface = NULL;
            gPrimary_surface = NULL;
        }
    }
}

// FUNCTION: CARMA2_HW 0x005006d0
void C2_HOOK_FASTCALL SSDXRelease(void) {

    if (gDirectDraw2 != NULL) {
        dr_dprintf("SSDXStop(): Releasing DDraw2 interface...");
        IDirectDraw2_Release(gDirectDraw2);
        gDirectDraw2 = NULL;
    }
    if (gDirectDraw != NULL) {
        dr_dprintf("SSDXStop(): Releasing DDraw object...");
        IDirectDraw_Release(gDirectDraw);
        gDirectDraw = NULL;
    }
    if (gDirectSound != NULL) {
        dr_dprintf("SSDXStop(): Releasing DSound object...");
        IDirectSound_Release(gDirectSound);
        gDirectSound = NULL;
    }
}

// FUNCTION: CARMA2_HW 0x00500760
void C2_NAKED C2_HOOK_FASTCALL SSDXDirectDrawSetup(int pWidth, int pHeight, int pBits, int* pPitch) {
#ifndef CARPOCALYPSE2_MATCHING
    HRESULT result;

    if (gWindowed) {
        LocalWindowedDDSetup(pWidth, pHeight, pPitch);
    }
    dr_dprintf("SSDXDirectDrawSetup(): Calling SetDisplayMode( %dx%dx%d )...", pWidth, pHeight, pBits);
    result = IDirectDraw2_SetDisplayMode(gDirectDraw2, pWidth, pHeight, pBits, 0, !gEnumerate_DirectX_surfaces && pWidth == 320 ? DDSDM_STANDARDVGAMODE : 0);
    SetRect(&gSSDXWindowPos, 0, 0, pWidth, pHeight);
    if (result == DD_OK) {
        DDSURFACEDESC surfaceDesc;

        C2_HOOK_BUG_ON(sizeof(surfaceDesc) != 0x6c);
        memset(&surfaceDesc, 0, sizeof(surfaceDesc));
        surfaceDesc.dwSize = sizeof(surfaceDesc);
        surfaceDesc.dwFlags = DDSD_CAPS | (gEnumerate_DirectX_surfaces ? DDSD_BACKBUFFERCOUNT : 0);
        surfaceDesc.dwBackBufferCount = gEnumerate_DirectX_surfaces ? 1 : 0;
        surfaceDesc.ddsCaps.dwCaps = DDSCAPS_SYSTEMMEMORY | DDSCAPS_PRIMARYSURFACE | (gEnumerate_DirectX_surfaces ? DDSCAPS_FLIP | DDSCAPS_COMPLEX : 0);
        dr_dprintf("SSDXDirectDrawSetup(): Calling CreateSurface()...");
        result = IDirectDraw_CreateSurface(gDirectDraw, &surfaceDesc, &gPrimary_surface, NULL);
        if (result == DD_OK) {

            if (gEnumerate_DirectX_surfaces) {
                int enum_counter;

                dr_dprintf("SSDXDirectDrawSetup(): Calling EnumAttachedSurfaces()...");
                result = IDirectDrawSurface_EnumAttachedSurfaces(gPrimary_surface, &enum_counter, LocalEnumAttachedSurfacesCallback);
                dr_dprintf("SSDXDirectDrawSetup(): enum_counter is %d", enum_counter);
            } else {

                C2_HOOK_BUG_ON(sizeof(surfaceDesc) != 0x6c);
                SetRect(&gSSDXRect, 0, 0, pWidth, pHeight);
                memset(&surfaceDesc, 0, sizeof(surfaceDesc));
                surfaceDesc.dwSize = sizeof(surfaceDesc);
                surfaceDesc.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
                surfaceDesc.ddsCaps.dwCaps = DDSCAPS_SYSTEMMEMORY | DDSCAPS_OFFSCREENPLAIN;
                surfaceDesc.dwHeight = pHeight;
                surfaceDesc.dwWidth = pWidth;

                dr_dprintf("SSDXDirectDrawSetup(): Calling CreateSurface() for attached surface...");
                result = IDirectDraw_CreateSurface(gDirectDraw, &surfaceDesc, &gAttached_surface, NULL);
            }
            if (result == DD_OK) {
                PALETTEENTRY palette[256];

                memset(palette, 0, sizeof(palette));
                result = IDirectDraw_CreatePalette(gDirectDraw, DDPCAPS_ALLOW256 | DDPCAPS_8BIT, palette, &gDirectDrawPalette, NULL);
                if (result == DD_OK) {
                    dr_dprintf("SSDXDirectDrawSetup(): Calling SetPalette");
                    result = IDirectDrawSurface_SetPalette(gPrimary_surface, gDirectDrawPalette);
                }
            }
        }
    }
    SSDXLockAttachedSurface();
    SSDXUnlockAttachedSurface();
    *pPitch = gSSDXPitch;
    if (result == DD_OK) {
        dr_dprintf("SSDXDirectDrawSetup(): Directdraw initialised: surface pith %d", gSSDXPitch);
    } else {
        dr_dprintf("WARNING: Problems during SSDXDirectDrawSetup()");
        SSDXLogError(result);
    }
#else
    __asm {
            sub esp, 0x470
            mov eax, dword ptr ds:[gWindowed]
            push ebx
            push ebp
            mov ebx, edx
            test eax, eax
            mov ebp, ecx
            mov dword ptr [esp + 8], 0
            je L00500795
            mov eax, dword ptr [esp + 0x480]
            push eax
            call LocalWindowedDDSetup
            pop ebp
            pop ebx
            add esp, 0x470
            ret 8
    L00500795:
            push edi
            push esi
            mov esi, dword ptr [esp + 0x484]
            push esi
            push ebx
            push ebp
            push 0x660b1c
            call dr_dprintf
            mov eax, dword ptr ds:[gEnumerate_DirectX_surfaces]
            add esp, 0x10
            test eax, eax
            jne L005007c6
            cmp ebp, 0x140
            jne L005007c6
            mov ecx, 1
            jmp L005007c8
    L005007c6:
            xor ecx, ecx
    L005007c8:
            mov eax, dword ptr ds:[gDirectDraw2]
            push ecx
            push 0
            push esi
            mov edx, dword ptr [eax]
            push ebx
            push ebp
            push eax
            call dword ptr [edx + 0x54]
            mov esi, eax
            push ebx
            push ebp
            push 0
            push 0
            push offset gSSDXWindowPos
            call dword ptr ds:[0x5891cc]
            test esi, esi
            jne L00500984
            mov ecx, 0x1b
            xor eax, eax
            lea edi, [esp + 0x14]
            push 0x660ae8
            rep stosd
            mov eax, dword ptr ds:[gEnumerate_DirectX_surfaces]
            mov dword ptr [esp + 0x18], 0x6c
            mov ecx, eax
            neg ecx
            sbb ecx, ecx
            xor edx, edx
            and ecx, 0x20
            or ecx, 1
            test eax, eax
            setne dl
            neg eax
            sbb eax, eax
            mov dword ptr [esp + 0x1c], ecx
            and eax, 0x18
            mov dword ptr [esp + 0x2c], edx
            or ah, 0xa
            mov dword ptr [esp + 0x80], eax
            call dr_dprintf
            mov eax, dword ptr ds:[gDirectDraw]
            add esp, 4
            lea edx, [esp + 0x14]
            mov ecx, dword ptr [eax]
            push esi
            push offset gPrimary_surface
            push edx
            push eax
            call dword ptr [ecx + 0x18]
            mov esi, eax
            test esi, esi
            jne L00500984
            mov eax, dword ptr ds:[gEnumerate_DirectX_surfaces]
            test eax, eax
            je L005008a8
            push 0x660aac
            call dr_dprintf
            mov eax, dword ptr ds:[gPrimary_surface]
            add esp, 4
            lea edx, [esp + 0x10]
            mov ecx, dword ptr [eax]
            push offset LocalEnumAttachedSurfacesCallback
            push edx
            push eax
            call dword ptr [ecx + 0x24]
            mov esi, eax
            mov eax, dword ptr [esp + 0x10]
            push eax
            push 0x660a80
            call dr_dprintf
            add esp, 8
            jmp L0050090f
    L005008a8:
            push ebx
            push ebp
            push 0
            push 0
            push offset gSSDXRect
            call dword ptr ds:[0x5891cc]
            mov ecx, 0x1b
            xor eax, eax
            lea edi, [esp + 0x14]
            push 0x6605dc
            rep stosd
            mov dword ptr [esp + 0x18], 0x6c
            mov dword ptr [esp + 0x1c], 7
            mov dword ptr [esp + 0x80], 0x840
            mov dword ptr [esp + 0x24], ebp
            mov dword ptr [esp + 0x20], ebx
            call dr_dprintf
            mov eax, dword ptr ds:[gDirectDraw]
            add esp, 4
            lea edx, [esp + 0x14]
            mov ecx, dword ptr [eax]
            push 0
            push offset gAttached_surface
            push edx
            push eax
            call dword ptr [ecx + 0x18]
            mov esi, eax
    L0050090f:
            test esi, esi
            jne L00500984
            lea eax, [esp + 0x82]
            mov ecx, 0x100
    L0050091f:
            mov byte ptr [eax + 1], 0
            mov byte ptr [eax], 0
            mov byte ptr [eax - 1], 0
            mov byte ptr [eax - 2], 0
            add eax, 4
            dec ecx
            jne L0050091f
            push 0x660a4c
            call dr_dprintf
            mov eax, dword ptr ds:[gDirectDraw]
            add esp, 4
            lea edx, [esp + 0x80]
            mov ecx, dword ptr [eax]
            push 0
            push offset gDirectDrawPalette
            push edx
            push 0x44
            push eax
            call dword ptr [ecx + 0x14]
            mov esi, eax
            test esi, esi
            jne L00500984
            push 0x660a1c
            call dr_dprintf
            mov eax, dword ptr ds:[gPrimary_surface]
            mov edx, dword ptr ds:[gDirectDrawPalette]
            add esp, 4
            mov ecx, dword ptr [eax]
            push edx
            push eax
            call dword ptr [ecx + 0x7c]
            mov esi, eax
    L00500984:
            call SSDXLockAttachedSurface
            mov eax, dword ptr ds:[gAttached_surface_locked]
            test eax, eax
            je L005009c8
            mov eax, dword ptr ds:[gAttached_surface]
            mov edx, dword ptr ds:[gSSDXLockedRect]
            push edx
            push eax
            mov ecx, dword ptr [eax]
            call dword ptr [ecx + 0x80]
            mov edi, eax
            test edi, edi
            jne L005009b4
            mov dword ptr ds:[gAttached_surface_locked], eax
            jmp L005009c8
    L005009b4:
            push 0x66052c
            call dr_dprintf
            add esp, 4
            mov ecx, edi
            call SSDXLogError
    L005009c8:
            mov eax, dword ptr [esp + 0x488]
            mov ecx, dword ptr ds:[gSSDXPitch]
            test esi, esi
            mov dword ptr [eax], ecx
            je L005009fe
            push 0x6609ec
            call dr_dprintf
            add esp, 4
            mov ecx, esi
            call SSDXLogError
            mov eax, esi
            pop esi
            pop edi
            pop ebp
            pop ebx
            add esp, 0x470
            ret 8
    L005009fe:
            mov edx, dword ptr ds:[gSSDXPitch]
            push edx
            push 0x6609ac
            call dr_dprintf
            add esp, 8
            mov eax, esi
            pop esi
            pop edi
            pop ebp
            pop ebx
            add esp, 0x470
            ret 8
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00500a30
void* C2_HOOK_FASTCALL SSDXLockAttachedSurface(void) {
    HRESULT result;
    DDSURFACEDESC surfaceDesc;

    if (!gAttached_surface_locked) {
        if (gAttached_surface != NULL) {
            result = IDirectDrawSurface_IsLost(gPrimary_surface);
            if (result == DDERR_SURFACELOST) {
                dr_dprintf("SSDXLockAttachedSurface(): Primary surface is lost, restoring...");
                result = IDirectDrawSurface_Restore(gPrimary_surface);
            }
            if (result == DD_OK) {
                result = IDirectDrawSurface_IsLost(gAttached_surface);
                if (result == DDERR_SURFACELOST) {
                    dr_dprintf("SSDXLockAttachedSurface(): Attached surface is lost, restoring...");
                    result = IDirectDrawSurface_Restore(gAttached_surface);
                }
                if (result == DD_OK) {
                    memset(&surfaceDesc, 0, sizeof(surfaceDesc));
                    surfaceDesc.dwSize = sizeof(surfaceDesc);
                    result = IDirectDrawSurface_Lock(gAttached_surface, NULL, &surfaceDesc, DDLOCK_WAIT, NULL);
                    if (result == DD_OK) {
                        gAttached_surface_locked = 1;
                        gSSDXLockedRect = surfaceDesc.lpSurface;
                        gSSDXPitch = surfaceDesc.lPitch;
                        return surfaceDesc.lpSurface;
                    }
                }
            }
            SSDXLogError(result);
        }
    }
    if (gAttached_surface_locked) {
        dr_dprintf("WARNING: SSDXLockAttachedSurface() called but surface already locked");
    }
    if (gAttached_surface == NULL) {
        dr_dprintf("WARNING: SSDXLockAttachedSurface() called but no attached surface available");
    }
    return NULL;
}

// FUNCTION: CARMA2_HW 0x00500b40
void C2_HOOK_FASTCALL SSDXUnlockAttachedSurface(void) {

    if (gAttached_surface_locked) {
        HRESULT result = IDirectDrawSurface_Unlock(gAttached_surface, gSSDXLockedRect);
        if (result == DD_OK) {
            gAttached_surface_locked = 0;
        } else {
            dr_dprintf("SSDXUnlockAttachedSurface(): Problem while unlocking attached surface...");
            SSDXLogError(result);
        }
    }
}

// FUNCTION: CARMA2_HW 0x00500b90
void C2_NAKED C2_HOOK_FASTCALL SSDXBlitOrFlip(void) {
#ifndef CARPOCALYPSE2_MATCHING
    if (gWindowed) {
        SSDXBlit();
    } else {
        IDirectDrawSurface_Flip(gPrimary_surface, NULL, DDFLIP_WAIT);
    }
#else
    __asm {
            mov eax, dword ptr [gWindowed]
            test eax, eax
            mov eax, dword ptr [gPrimary_surface]
            mov ecx, dword ptr [eax]
            je L00500bbd
            mov edx, dword ptr [gAttached_surface]
            push 0
            push 0x1000000
            push offset gSSDXRect
            push edx
            push offset gSSDXWindowPos
            push eax
            call dword ptr [ecx + 0x14]
            ret
    L00500bbd:
            push 1
            push 0
            push eax
            call dword ptr [ecx + 0x2c]
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00500bd0
void C2_NAKED C2_HOOK_FASTCALL SSDXBlit(void) {
#ifndef CARPOCALYPSE2_MATCHING

    IDirectDrawSurface_Blt(gPrimary_surface, &gSSDXWindowPos, gAttached_surface, &gSSDXRect, DDBLT_WAIT, NULL);
#else
    __asm {
            mov eax, dword ptr [gWindowed]
            mov edx, dword ptr [gAttached_surface]
            test eax, eax
            mov eax, dword ptr [gPrimary_surface]
            push 0
            push 0x1000000
            push offset gSSDXRect
            mov ecx, dword ptr [eax]
            push edx
            push offset gSSDXWindowPos
            push eax
            call dword ptr [ecx + 0x14]
            ret
            nop
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00500c00
void C2_HOOK_FASTCALL SSDXSetPaletteEntries(PALETTEENTRY* pPalette, int pStart, int pCount) {
    if (gWindowed && gDirectDrawPalette != NULL) {
        int i;

        for (i = MAX(10, pStart); i < MIN(0xf6, pStart + pCount - 1); i++) {

            gSSDX_system_palette[i].peRed = pPalette[i].peRed;
            gSSDX_system_palette[i].peGreen = pPalette[i].peGreen;
            gSSDX_system_palette[i].peBlue = pPalette[i].peBlue;
        }
        IDirectDrawPalette_SetEntries(gDirectDrawPalette, 0, pStart, pCount, gSSDX_system_palette);
    } else {
        IDirectDrawPalette_SetEntries(gDirectDrawPalette, 0, pStart, pCount, pPalette);
    }
}

// FUNCTION: CARMA2_HW 0x00500ca0
void C2_HOOK_FASTCALL SSDXSetPrimaryPalette(void) {
    HRESULT result;

    if (!gWindowed) {
        return;
    }
    if (gDirectDrawPalette == NULL) {
        return;
    }
    if (gPrimary_surface == NULL) {
        return;
    }
    result = IDirectDrawSurface_SetPalette(gPrimary_surface, gDirectDrawPalette);
    if (result == DDERR_SURFACELOST) {
        IDirectDrawSurface_Restore(gPrimary_surface);
        IDirectDrawSurface_SetPalette(gPrimary_surface, gDirectDrawPalette);
    }
}

// FUNCTION: CARMA2_HW 0x00500cf0
void C2_HOOK_FASTCALL SSDXWindowPosOrSizeChanged(HWND pHwnd) {

    if (gWindowed) {
        GetClientRect(pHwnd, &gSSDXWindowPos);
        ClientToScreen(pHwnd, (POINT*)&gSSDXWindowPos.left);
        ClientToScreen(pHwnd, (POINT*)&gSSDXWindowPos.right);
        dr_dprintf("SSDXWindowPosOrSizeChanged(): New window rect: (%d,%d)(%d,%d)",
            gSSDXWindowPos.left, gSSDXWindowPos.top, gSSDXWindowPos.right, gSSDXWindowPos.bottom);
    }
}
