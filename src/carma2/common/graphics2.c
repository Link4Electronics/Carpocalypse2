#include "graphics2.h"

#include <string.h>
#include <stdio.h>

#include "input.h"
#include "grafdata.h"
#include "tintedpoly.h"
#include "utility.h"
#include "errors.h"
#include "globvars.h"
#include "platform.h"
#include "carpocalypse2_types.h"
#include "carpocalypse2_macros.h"
extern void C2_HOOK_FASTCALL FadePaletteUp(void);
extern void C2_HOOK_FASTCALL DeallocateTransientBitmap(int pIndex);
extern void C2_HOOK_FASTCALL SetTintedPolySize(int pIndex, int pX, int pY, int pW, int pH);
extern void C2_HOOK_FASTCALL TurnTintedPolyOn(int pIndex);
extern void C2_HOOK_FASTCALL TurnTintedPolyOff(int pIndex);
extern void C2_HOOK_FASTCALL ProcessTintedPoly(int pIndex);
extern void C2_HOOK_FASTCALL RenderTintedPolys(void);
extern tU32 C2_HOOK_FASTCALL PDGetTotalTime(void);
extern br_pixelmap* C2_HOOK_FASTCALL LoadPixelmap(const char* pPath_name);
extern void C2_HOOK_FASTCALL SetFadedPalette(unsigned int pDegree);
extern br_pixelmap* gBack_screen;
extern br_pixelmap* gPalette_0074a5fc;
extern int gMouse_in_use;
extern tTintedPoly gTintedPolys[10];
extern tGraf_spec gGraf_specs[2];
extern int gNoTransients;

// GLOBAL: CARMA2_HW 0x0068d918
tU16 gPixelmap_copy_buffer[640];

// GLOBAL: CARMA2_HW 0x0067c390
int gMouse_started;
// GLOBAL: CARMA2_HW 0x0079ecb0
tMouse_coord gMouse_last_pos;
// GLOBAL: CARMA2_HW 0x0067be94
int gTransient_bitmap_index;
// GLOBAL: CARMA2_HW 0x0067be90
int gCurrent_cursor_index;
int gCursor_line_width = 5;
// The original lays these out contiguously at the end of the transient
// bitmap array: gNext_transient at +0x4b0, gNoTransients at +0x4b4 and
// gDim_amount at +0x4c0.  The loops' end-of-array compares resolve to
// these symbols, so keep them explicitly initialized (which places them
// in .data in declaration order) and adjacent.
// GLOBAL: CARMA2_HW 0x0067be98
tTransient_bm gTransient_bitmaps[50] = { 0 };
// GLOBAL: CARMA2_HW 0x0067c348
int gNext_transient = 0;
// GLOBAL: CARMA2_HW 0x0074ca28
int gNoTransients = 0;
int gTransient_pad_0 = 0;
int gTransient_pad_1 = 0;
// GLOBAL: CARMA2_HW 0x0067c358
int gDim_amount = 0;
// GLOBAL: CARMA2_HW 0x0074a680
char* gCurrent_palette_pixels;
// GLOBAL: CARMA2_HW 0x006a27a8
int gSaved_table_count;
// GLOBAL: CARMA2_HW 0x006923c0
int gPalette_munged;
// GLOBAL: CARMA2_HW 0x006923a4
tU32 gLast_palette_change;
// GLOBAL: CARMA2_HW 0x006923b4
int gPalette_index;
// GLOBAL: CARMA2_HW 0x006a2488
extern tSaved_table gSaved_shade_tables[100];
// GLOBAL: CARMA2_HW 0x0074cf04
int gPalette_changed;
// GLOBAL: CARMA2_HW 0x0074a674
br_pixelmap* gRender_palette;
// GLOBAL: CARMA2_HW 0x006923ac
br_pixelmap* gOrig_render_palette;
// GLOBAL: CARMA2_HW 0x0074a67c
br_pixelmap* gFlic_palette;
// GLOBAL: CARMA2_HW 0x0074a678
br_pixelmap* gCurrent_palette;
// GLOBAL: CARMA2_HW 0x006923b8
br_colour* gScratch_pixels;
// GLOBAL: CARMA2_HW 0x006923a8
br_pixelmap* gScratch_palette;
// GLOBAL: CARMA2_HW 0x006923c8
int gFaded_palette;
// GLOBAL: CARMA2_HW 0x0058fdc8
int gCursor_tinted_top = -1;
// GLOBAL: CARMA2_HW 0x0058fdd0
int gCursor_tinted_left = -1;
// GLOBAL: CARMA2_HW 0x0058fdcc
int gCursor_tinted_bottom = -1;
// GLOBAL: CARMA2_HW 0x0058fdd4
int gCursor_tinted_right = -1;
// GLOBAL: CARMA2_HW 0x0058fdd8
int gCursor_tinted_center = -1;

// GLOBAL: CARMA2_HW 0x0068be20
br_pixelmap* gCurrent_splash;

void C2_HOOK_FASTCALL DeallocateAllTransientBitmaps(void) {
    int i;

    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gTransient_bitmaps); i++) {
        DeallocateTransientBitmap(i);
    }
}

#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_FASTCALL BRPM_convert(br_pixelmap* pMap, int pPixel_type);
extern br_pixelmap* C2_HOOK_CDECL BrMapAdd();
extern br_pixelmap* C2_HOOK_CDECL BrMapFind();
extern br_pixelmap* C2_HOOK_CDECL BrMapRemove();
extern void C2_HOOK_CDECL BrPixelmapFree();
extern void C2_HOOK_CDECL BrPixelmapRectangleCopy();
extern void C2_HOOK_CDECL BrPixelmapRectangleFill();
extern int C2_HOOK_FASTCALL FRONTEND_Redraw(br_pixelmap* dst, br_int_16 dx, br_int_16 dy, br_pixelmap* src, br_int_16 sx, br_int_16 sy, br_int_16 w, br_int_16 h);
extern void C2_HOOK_FASTCALL PDScreenBufferSwap(int pRendering_area_only);
extern void C2_HOOK_FASTCALL PDSetPalette(br_pixelmap *pixelmap);
extern void C2_HOOK_FASTCALL PixelmapSwapByteOrder(br_pixelmap* pMap);
extern br_uint_8 bl;
extern tPixelFlags gPixelFlags;
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern tNet_mode gNet_mode;
#endif
// FUNCTION: CARMA2_HW 0x0043e010
void C2_HOOK_FASTCALL RemoveTransientBitmaps(int pGraphically_remove_them) {
    int i;
    int order_number;

    if (!gNoTransients && pGraphically_remove_them) {
        for (order_number = gNext_transient - 1; order_number >= 0; order_number--) {
            for (i = 0; i < 50; i++) {
                if (gTransient_bitmaps[i].pixmap != NULL && order_number == gTransient_bitmaps[i].order_number) {
                    if (gTransient_bitmaps[i].in_use) {
                        BrPixelmapRectangleCopy(gBack_screen,
                                                gTransient_bitmaps[i].x_coord,
                                                gTransient_bitmaps[i].y_coord,
                                                gTransient_bitmaps[i].pixmap,
                                                0,
                                                0,
                                                gTransient_bitmaps[i].pixmap->width,
                                                gTransient_bitmaps[i].pixmap->height);
                    }
                    break;
                }
            }
        }
    }
    gNext_transient = 0;
}

#include "utility.h"
#include "errors.h"
#include "globvars.h"
#include "platform.h"
#include "carpocalypse2_types.h"


void C2_HOOK_FASTCALL Darken(tU8* pPtr, tU32 pDarken_amount) {
    *pPtr = (pDarken_amount * *pPtr) / 256;
}

void C2_HOOK_FASTCALL DRSetPalette2(br_pixelmap* pThe_palette, int pSet_current_palette) {
    ((br_int_32*)pThe_palette->pixels)[0] = 0;
    if (pSet_current_palette) {
        memcpy(gCurrent_palette_pixels, pThe_palette->pixels, 256 * sizeof(br_colour));
    }
    gPalette_changed = 0;
    if (!gFaded_palette) {
        PDSetPalette(pThe_palette);
    }
    gPalette_munged |= (pThe_palette != gRender_palette);
}

// FUNCTION: CARMA2_HW 0x004b5390
void C2_NAKED C2_HOOK_FASTCALL SetFadedPalette(tU32 pDegree) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;

    memcpy(gScratch_pixels, gCurrent_palette->pixels, 256 * sizeof(br_colour));
    for (i = 0; i < 256; i++) {
        Darken(&((tU8*)gScratch_pixels)[4 * i + 0], pDegree);
        Darken(&((tU8*)gScratch_pixels)[4 * i + 1], pDegree);
        Darken(&((tU8*)gScratch_pixels)[4 * i + 2], pDegree);
        Darken(&((tU8*)gScratch_pixels)[4 * i + 3], pDegree);
    }
    DRSetPalette2(gScratch_palette, 0);
#else
    __asm {
            mov eax, dword ptr [gCurrent_palette]
            push esi
            mov edx, ecx
            push edi
            mov esi, dword ptr [eax + 8]
            mov edi, dword ptr [gScratch_pixels]
            mov ecx, 0x100
            xor eax, eax
            rep movsd
    L004b53ab:
            mov esi, dword ptr [gScratch_pixels]
            xor ecx, ecx
            mov cl, byte ptr [esi + eax]
            add eax, 4
            imul ecx, edx
            shr ecx, 8
            mov byte ptr [esi + eax - 4], cl
            mov esi, dword ptr [gScratch_pixels]
            xor ecx, ecx
            mov cl, byte ptr [esi + eax - 3]
            imul ecx, edx
            shr ecx, 8
            mov byte ptr [esi + eax - 3], cl
            mov esi, dword ptr [gScratch_pixels]
            xor ecx, ecx
            mov cl, byte ptr [esi + eax - 2]
            imul ecx, edx
            shr ecx, 8
            mov byte ptr [esi + eax - 2], cl
            mov esi, dword ptr [gScratch_pixels]
            xor ecx, ecx
            mov cl, byte ptr [esi + eax - 1]
            imul ecx, edx
            shr ecx, 8
            mov byte ptr [esi + eax - 1], cl
            cmp eax, 0x400
            jl L004b53ab
            mov eax, dword ptr [gScratch_palette]
            mov esi, eax
            mov edx, dword ptr [eax + 8]
            mov dword ptr [edx], 0
            mov eax, dword ptr [gFaded_palette]
            test eax, eax
            mov dword ptr [gPalette_changed], 0
            jne L004b5436
            mov ecx, esi
            call PDSetPalette
    L004b5436:
            mov edx, dword ptr [gRender_palette]
            mov ecx, dword ptr [gPalette_munged]
            xor eax, eax
            cmp esi, edx
            setne al
            or ecx, eax
            pop edi
            mov dword ptr [gPalette_munged], ecx
            pop esi
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
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004b5470
void C2_NAKED C2_HOOK_FASTCALL FadePaletteUp(void) {
#ifndef CARPOCALYPSE2_MATCHING
    int start_time;
    int the_time;

    if (!gFaded_palette) {
        return;
    }

    gFaded_palette = 0;
    start_time = PDGetTotalTime();
    while ((the_time = PDGetTotalTime() - start_time) < 500) {
        SetFadedPalette((the_time * 256) / 500);
    }
    DRSetPalette(gCurrent_palette);
#else
    __asm {
            mov eax, dword ptr [gFaded_palette]
            test eax, eax
            je L004b55e6
            push edi
            push esi
            push ebp
            push ebx
            mov dword ptr [gFaded_palette], 0
            call PDGetTotalTime
            mov ebp, eax
            call PDGetTotalTime
            sub eax, ebp
            cmp eax, 0x1f4
            jge L004b5590
    L004b54a4:
            shl eax, 8
            mov ecx, eax
            mov eax, 0x10624dd3
            imul ecx
            sar edx, 5
            mov eax, edx
            mov edi, dword ptr [gScratch_pixels]
            shr eax, 0x1f
            add edx, eax
            mov eax, dword ptr [gCurrent_palette]
            mov ecx, 0x100
            mov esi, dword ptr [eax + 8]
            xor eax, eax
            rep movsd
    L004b54d1:
            mov esi, dword ptr [gScratch_pixels]
            xor ecx, ecx
            mov ebx, edx
            mov cl, byte ptr [esi + eax]
            add eax, 4
            imul ebx, ecx
            shr ebx, 8
            mov byte ptr [esi + eax - 4], bl
            mov esi, dword ptr [gScratch_pixels]
            xor ecx, ecx
            mov ebx, edx
            mov cl, byte ptr [esi + eax - 3]
            imul ebx, ecx
            shr ebx, 8
            mov byte ptr [esi + eax - 3], bl
            mov esi, dword ptr [gScratch_pixels]
            xor ecx, ecx
            mov ebx, edx
            mov cl, byte ptr [esi + eax - 2]
            imul ebx, ecx
            shr ebx, 8
            mov byte ptr [esi + eax - 2], bl
            mov esi, dword ptr [gScratch_pixels]
            xor ecx, ecx
            mov ebx, edx
            mov cl, byte ptr [esi + eax - 1]
            imul ebx, ecx
            shr ebx, 8
            mov byte ptr [esi + eax - 1], bl
            cmp eax, 0x400
            jl L004b54d1
            mov esi, dword ptr [gScratch_palette]
            mov edx, dword ptr [esi + 8]
            mov dword ptr [edx], 0
            mov eax, dword ptr [gFaded_palette]
            test eax, eax
            mov dword ptr [gPalette_changed], 0
            jne L004b5563
            mov ecx, esi
            call PDSetPalette
    L004b5563:
            mov edi, dword ptr [gRender_palette]
            xor eax, eax
            cmp esi, edi
            mov esi, dword ptr [gPalette_munged]
            setne al
            or esi, eax
            mov dword ptr [gPalette_munged], esi
            call PDGetTotalTime
            sub eax, ebp
            cmp eax, 0x1f4
            jl L004b54a4
    L004b5590:
            mov ebx, dword ptr [gCurrent_palette]
            mov ecx, dword ptr [ebx + 8]
            mov dword ptr [ecx], 0
            mov esi, dword ptr [ebx + 8]
            mov edi, dword ptr [gCurrent_palette_pixels]
            mov ecx, 0x100
            rep movsd
            mov eax, dword ptr [gFaded_palette]
            mov dword ptr [gPalette_changed], 0
            test eax, eax
            jne L004b55c9
            mov ecx, ebx
            call PDSetPalette
    L004b55c9:
            mov ecx, dword ptr [gRender_palette]
            mov eax, dword ptr [gPalette_munged]
            xor edx, edx
            cmp ebx, ecx
            setne dl
            pop ebx
            or eax, edx
            pop ebp
            pop esi
            mov dword ptr [gPalette_munged], eax
            pop edi
    L004b55e6:
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
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004b55f0
void C2_NAKED C2_HOOK_FASTCALL EnsurePaletteUp(void) {
#ifndef CARPOCALYPSE2_MATCHING
    if (gFaded_palette) {
        FadePaletteUp();
    }
#else
    __asm {
            mov eax, dword ptr [gFaded_palette]
            test eax, eax
            je L004b5766
            push edi
            push esi
            push ebp
            push ebx
            mov dword ptr [gFaded_palette], 0
            call PDGetTotalTime
            mov ebp, eax
            call PDGetTotalTime
            sub eax, ebp
            cmp eax, 0x1f4
            jge L004b5710
    L004b5624:
            shl eax, 8
            mov ecx, eax
            mov eax, 0x10624dd3
            imul ecx
            sar edx, 5
            mov eax, edx
            mov edi, dword ptr [gScratch_pixels]
            shr eax, 0x1f
            add edx, eax
            mov eax, dword ptr [gCurrent_palette]
            mov ecx, 0x100
            mov esi, dword ptr [eax + 8]
            xor eax, eax
            rep movsd
    L004b5651:
            mov esi, dword ptr [gScratch_pixels]
            xor ecx, ecx
            mov ebx, edx
            mov cl, byte ptr [esi + eax]
            add eax, 4
            imul ebx, ecx
            shr ebx, 8
            mov byte ptr [esi + eax - 4], bl
            mov esi, dword ptr [gScratch_pixels]
            xor ecx, ecx
            mov ebx, edx
            mov cl, byte ptr [esi + eax - 3]
            imul ebx, ecx
            shr ebx, 8
            mov byte ptr [esi + eax - 3], bl
            mov esi, dword ptr [gScratch_pixels]
            xor ecx, ecx
            mov ebx, edx
            mov cl, byte ptr [esi + eax - 2]
            imul ebx, ecx
            shr ebx, 8
            mov byte ptr [esi + eax - 2], bl
            mov esi, dword ptr [gScratch_pixels]
            xor ecx, ecx
            mov ebx, edx
            mov cl, byte ptr [esi + eax - 1]
            imul ebx, ecx
            shr ebx, 8
            mov byte ptr [esi + eax - 1], bl
            cmp eax, 0x400
            jl L004b5651
            mov esi, dword ptr [gScratch_palette]
            mov edx, dword ptr [esi + 8]
            mov dword ptr [edx], 0
            mov eax, dword ptr [gFaded_palette]
            test eax, eax
            mov dword ptr [gPalette_changed], 0
            jne L004b56e3
            mov ecx, esi
            call PDSetPalette
    L004b56e3:
            mov edi, dword ptr [gRender_palette]
            xor eax, eax
            cmp esi, edi
            mov esi, dword ptr [gPalette_munged]
            setne al
            or esi, eax
            mov dword ptr [gPalette_munged], esi
            call PDGetTotalTime
            sub eax, ebp
            cmp eax, 0x1f4
            jl L004b5624
    L004b5710:
            mov ebx, dword ptr [gCurrent_palette]
            mov ecx, dword ptr [ebx + 8]
            mov dword ptr [ecx], 0
            mov esi, dword ptr [ebx + 8]
            mov edi, dword ptr [gCurrent_palette_pixels]
            mov ecx, 0x100
            rep movsd
            mov eax, dword ptr [gFaded_palette]
            mov dword ptr [gPalette_changed], 0
            test eax, eax
            jne L004b5749
            mov ecx, ebx
            call PDSetPalette
    L004b5749:
            mov ecx, dword ptr [gRender_palette]
            mov eax, dword ptr [gPalette_munged]
            xor edx, edx
            cmp ebx, ecx
            setne dl
            pop ebx
            or eax, edx
            pop ebp
            pop esi
            mov dword ptr [gPalette_munged], eax
            pop edi
    L004b5766:
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0043e0a0
int C2_HOOK_FASTCALL DoMouseCursor(void) {

    int delta_time;
    int mouse_moved;
    int button_down;
    int new_required;
    tU32 time_now;
    int pos_x, pos_y;

    // GLOBAL: CARMA2_HW 0x0067c398
    static tU32 last_call_time;

    // GLOBAL: CARMA2_HW 0x0067c39c
    static tU32 last_required_change;
    // GLOBAL: CARMA2_HW 0x0067c3a0
    static int delta_x;
    // GLOBAL: CARMA2_HW 0x0067c3a4
    static int required_cursor;
    // GLOBAL: CARMA2_HW 0x0067c3a8
    static int zero_count;
    // GLOBAL: CARMA2_HW 0x0067c3ac
    static int button_was_down;
    // GLOBAL: CARMA2_HW 0x0058fddc
    static int draw_cursor = 1;

    (void)button_was_down;

    do {
        time_now = PDGetTotalTime();
        if (last_call_time == 0) {
            delta_time = 1000;
        } else {
            delta_time = time_now - last_call_time;
        }
    } while (delta_time <= 20);
    GetMousePosition(&pos_x, &pos_y);
    mouse_moved = pos_x != gMouse_last_pos.x || pos_y != gMouse_last_pos.y;
    button_down = EitherMouseButtonDown();
    if (gMouse_in_use || mouse_moved) {
        gMouse_in_use = 1;
        if (pos_x == gMouse_last_pos.x) {
            if (zero_count > 4) {
                delta_x = 0;
            }
            zero_count += 1;
        } else {
            zero_count = 0;
            delta_x = 1000 * (pos_x - gMouse_last_pos.x) / delta_time;
        }
        if (delta_x < -10) {
            new_required = 0;
        } else if (delta_x > 10) {
            new_required = 3;
        } else {
            new_required = 2;
        }
        if (new_required != required_cursor && time_now - last_required_change >= 200) {
            last_required_change = time_now;
            required_cursor = new_required;
        }
        gCurrent_cursor_index = 2;
        if (!gNoTransients) {
            br_pixelmap *map;
            int idx = gTransient_bitmap_index;

            gTransient_bitmaps[idx].x_coord = (pos_x - 7) & ~3;
            gTransient_bitmaps[idx].y_coord = pos_y - 7;
            gTransient_bitmaps[idx].in_use = 1;
            gTransient_bitmaps[idx].order_number = gNext_transient;
            gNext_transient += 1;
            map = gTransient_bitmaps[idx].pixmap;
            BrPixelmapRectangleCopy(map,
                0, 0,
                gBack_screen,
                gTransient_bitmaps[idx].x_coord,
                gTransient_bitmaps[idx].y_coord,
                map->width,
                map->height);
        }
        if (draw_cursor && draw_cursor == 1) {
            PossibleUnlock(1);
            SetTintedPolySize(gCursor_tinted_top,
                pos_x - 1, 0,
                gCursor_line_width, pos_y - 9);
            SetTintedPolySize(gCursor_tinted_left,
                0, pos_y - 1,
                pos_x - 9, gCursor_line_width);
            SetTintedPolySize(gCursor_tinted_bottom,
                pos_x - 1, pos_y + 5 + gCursor_line_width,
                gCursor_line_width, gCurrent_graf_data->height - gCursor_line_width - pos_y + 9);
            SetTintedPolySize(gCursor_tinted_right,
                pos_x + 5 + gCursor_line_width, pos_y - 1,
                gCurrent_graf_data->width - gCursor_line_width - pos_x + 9, gCursor_line_width);
            SetTintedPolySize(gCursor_tinted_center,
                pos_x - 7, pos_y - 7,
                16, 16);
            TurnTintedPolyOn(gCursor_tinted_top);
            TurnTintedPolyOn(gCursor_tinted_left);
            TurnTintedPolyOn(gCursor_tinted_bottom);
            TurnTintedPolyOn(gCursor_tinted_right);
            TurnTintedPolyOn(gCursor_tinted_center);
            ProcessTintedPoly(gCursor_tinted_top);
            ProcessTintedPoly(gCursor_tinted_left);
            ProcessTintedPoly(gCursor_tinted_bottom);
            ProcessTintedPoly(gCursor_tinted_right);
            ProcessTintedPoly(gCursor_tinted_center);
            RenderTintedPolys();
            TurnTintedPolyOff(gCursor_tinted_top);
            TurnTintedPolyOff(gCursor_tinted_left);
            TurnTintedPolyOff(gCursor_tinted_bottom);
            TurnTintedPolyOff(gCursor_tinted_right);
            TurnTintedPolyOff(gCursor_tinted_center);
        }
    }
    last_call_time = time_now;
    gMouse_last_pos.y = pos_y;
    gMouse_last_pos.x = pos_x;
    button_was_down = button_down;
    return mouse_moved;
}

// AllocateCursorTransient

// InitMouseTargetLines

// FUNCTION: CARMA2_HW 0x0043e3f0
void C2_HOOK_FASTCALL AllocateCursorActors(void) {
    br_pixelmap *mse_cross;
    br_pixelmap *mse_line;

    BrMapFind("mse_corn");
    mse_cross = BrMapFind("mse_cros");
    mse_line = BrMapFind("mse_line");
    gCursor_tinted_top = CreateTintedPoly(0, 0, gCursor_line_width, gCurrent_graf_data->height, 1, 127, 192, 0);
    gCursor_tinted_left = CreateTintedPoly(0, 0, gCurrent_graf_data->width, gCursor_line_width, 1, 127, 192, 0);
    gCursor_tinted_bottom = CreateTintedPoly(0, 0, gCursor_line_width, gCurrent_graf_data->height, 1, 127, 192, 0);
    gCursor_tinted_right = CreateTintedPoly(0, 0, gCurrent_graf_data->width, gCursor_line_width, 1, 127, 192, 0);
    gCursor_tinted_center = CreateTintedPoly(0, 0, 16, 16, 1, 127, 192, 0);
    if (mse_line != NULL) {
        gTintedPolys[gCursor_tinted_left].material->colour_map = mse_line;
        gTintedPolys[gCursor_tinted_left].material->ka = 1.0f;
        BrMaterialUpdate(gTintedPolys[gCursor_tinted_left].material, BR_MATU_ALL);
        gTintedPolys[gCursor_tinted_right].material->colour_map = mse_line;
        BrMaterialUpdate(gTintedPolys[gCursor_tinted_right].material, BR_MATU_ALL);
        gTintedPolys[gCursor_tinted_top].material->colour_map = mse_line;
        BrMatrix23Rotate(&(gTintedPolys[gCursor_tinted_top].material)->map_transform, BR_ANGLE_DEG(90));
        BrMaterialUpdate(gTintedPolys[gCursor_tinted_top].material, BR_MATU_ALL);
        (gTintedPolys[gCursor_tinted_bottom].material)->colour_map = mse_line;
        BrMatrix23Rotate(&(gTintedPolys[gCursor_tinted_bottom].material)->map_transform, BR_ANGLE_DEG(90));
        BrMaterialUpdate(gTintedPolys[gCursor_tinted_bottom].material, BR_MATU_ALL);
    }
    if (mse_cross != NULL) {
        gTintedPolys[gCursor_tinted_center].material->colour_map = mse_cross;
        BrMaterialUpdate(gTintedPolys[gCursor_tinted_center].material, BR_MATU_ALL);
    }
}

// FUNCTION: CARMA2_HW 0x0043e6c0
void C2_HOOK_FASTCALL StartMouseCursor(void) {
    if (gCursor_tinted_top == -1) {
        AllocateCursorActors();
    }
    gNext_transient = 0;
    gTransient_bitmap_index = 0;
    GetMousePosition(&gMouse_last_pos.x, &gMouse_last_pos.y);
    gMouse_in_use = 0;
    gCurrent_cursor_index = 2;
    gMouse_started = 1;
}

// FUNCTION: CARMA2_HW 0x0043e710
void C2_HOOK_FASTCALL EndMouseCursor(void) {
    RemoveTransientBitmaps(1);
    DeallocateAllTransientBitmaps();
    gMouse_started = 0;
}

// FUNCTION: CARMA2_HW 0x0047b450
void C2_HOOK_FASTCALL DRDrawLine(br_pixelmap* pDestn, int pX1, int pY1, int pX2, int pY2, int pColour) {
    if (gBack_screen->type != BR_PMT_INDEX_8) {
        pColour = PaletteEntry16Bit(gRender_palette, pColour);
    }
    BrPixelmapLine(pDestn, pX1, pY1, pX2, pY2, pColour);
}

// FUNCTION: CARMA2_HW 0x0047b880
void C2_HOOK_FASTCALL ClearEntireScreen(void) {
    if (gScreen != NULL) {
        BrPixelmapFill(gScreen, gGraf_specs[gGraf_spec_index].black_value);
    }
    BrPixelmapFill(gBack_screen, gGraf_specs[gGraf_spec_index].black_value);
    BrPixelmapFill(gDepth_buffer, 0xffffffff);
    PDScreenBufferSwap(0);
    BrPixelmapFill(gBack_screen, gGraf_specs[gGraf_spec_index].black_value);
}

// DistanceFromPlane

// FUNCTION: CARMA2_HW 0x0047b960
void C2_HOOK_FASTCALL KillSplashScreen(void) {
    if (gCurrent_splash != NULL) {
        BrMapRemove(gCurrent_splash);
        BrPixelmapFree(gCurrent_splash);
        gCurrent_splash = NULL;
    }
}

// FUNCTION: CARMA2_HW 0x0047b990
void C2_NAKED C2_HOOK_FASTCALL SplashScreenWith(const char* pPixmap_name) {
#ifndef CARPOCALYPSE2_MATCHING
    br_pixelmap* map;
    tPixelFlags flags;

    map = BrMapFind(pPixmap_name);
    if (gCurrent_splash != NULL && map == gCurrent_splash) {
        return;
    }
    if (gCurrent_splash != NULL) {
        KillSplashScreen();
    }
    flags = gPixelFlags;
    gCurrent_splash = map;
    if (map == NULL) {
        gCurrent_splash = LoadPixelmap(pPixmap_name);
        BRPM_convert(gCurrent_splash, gBack_screen->type);
        if (gCurrent_splash != NULL) {
            BrMapAdd(gCurrent_splash);
        }
    }
    gPixelFlags = flags;
    if (gCurrent_splash != NULL) {
        PixelmapSwapByteOrder(gCurrent_splash);
        FRONTEND_Redraw(gBack_screen, 0, 0, gCurrent_splash, 0, 0,
            (br_int_16)gCurrent_splash->width, (br_int_16)gCurrent_splash->height);
        PDScreenBufferSwap(0);
        BrMapRemove(gCurrent_splash);
        BrPixelmapFree(gCurrent_splash);
        gCurrent_splash = NULL;
    }
#else
    __asm {
            push ebx
            push esi
            push edi
            mov edi, ecx
            push edi
            call BrMapFind
            mov esi, eax
            mov eax, dword ptr [gCurrent_splash]
            add esp, 4
            test eax, eax
            je L0047b9cc
            cmp esi, eax
            je L0047ba75
            test eax, eax
            je L0047b9cc
            push eax
            call BrMapRemove
            mov eax, dword ptr [gCurrent_splash]
            add esp, 4
            push eax
            call BrPixelmapFree
            add esp, 4
    L0047b9cc:
            mov ebx, dword ptr [gPixelFlags]
            mov ecx, esi
            test esi, esi
            mov dword ptr [gCurrent_splash], ecx
            jne L0047ba15
            mov ecx, edi
            call LoadPixelmap
            mov ecx, dword ptr [gBack_screen]
            xor edx, edx
            mov dword ptr [gCurrent_splash], eax
            mov dl, byte ptr [ecx + 0x2c]
            mov ecx, eax
            call BRPM_convert
            mov ecx, dword ptr [gCurrent_splash]
            test ecx, ecx
            je L0047ba15
            push ecx
            call BrMapAdd
            mov ecx, dword ptr [gCurrent_splash]
            add esp, 4
    L0047ba15:
            test ecx, ecx
            mov dword ptr [gPixelFlags], ebx
            je L0047ba75
            call PixelmapSwapByteOrder
            mov eax, dword ptr [gCurrent_splash]
            mov dx, word ptr [eax + 0x36]
            mov cx, word ptr [eax + 0x34]
            push edx
            push ecx
            mov ecx, dword ptr [gBack_screen]
            push 0
            push 0
            push eax
            push 0
            xor edx, edx
            call FRONTEND_Redraw
            xor ecx, ecx
            call PDScreenBufferSwap
            mov edx, dword ptr [gCurrent_splash]
            push edx
            call BrMapRemove
            mov eax, dword ptr [gCurrent_splash]
            add esp, 4
            push eax
            call BrPixelmapFree
            add esp, 4
            mov dword ptr [gCurrent_splash], 0
    L0047ba75:
            pop edi
            pop esi
            pop ebx
            ret
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

// FUNCTION: CARMA2_HW 0x0047ba80
void C2_NAKED C2_HOOK_FASTCALL DRPixelmapRectangleMaskedCopy(br_pixelmap* pDest, br_int_16 pDest_x, br_int_16 pDest_y, const br_pixelmap* pSource, br_int_16 pSource_x, br_int_16 pSource_y, br_int_16 pWidth, br_int_16 pHeight) {
#ifndef CARPOCALYPSE2_MATCHING
    int y;
    int x;
    int dest_row_wrap;
    int source_row_wrap;
    tU16 the_byte;
    tU16* source_ptr;
    tU16* dest_ptr;
    int x_delta;

    source_ptr = (tU16*)pSource->pixels + pSource->row_bytes * pSource_y / 2 + pSource_x;
    dest_ptr = (tU16*)pDest->pixels + pDest_y * pDest->row_bytes / 2 + pDest->base_x / 2 + pDest_x;
    source_row_wrap = pSource->row_bytes / 2 - pWidth;
    dest_row_wrap = pDest->row_bytes / 2 - pWidth;

    if (pDest_y < 0) {
        pHeight += pDest_y;
        if (pHeight <= 0) {
            return;
        }
        source_ptr -= pDest_y * pSource->row_bytes / 2;
        dest_ptr -= pDest_y * pDest->row_bytes / 2;
        pDest_y = 0;
    }
    if (pDest_y < pDest->height) {
        if (pDest_y + pHeight > pDest->height) {
            pHeight = pDest->height - pDest_y;
        }
        if (pDest_x < 0) {
            pWidth += pDest_x;
            if (pWidth <= 0) {
                return;
            }
            source_ptr -= pDest_x;
            dest_ptr -= pDest_x;
            source_row_wrap -= pDest_x;
            dest_row_wrap -= pDest_x;
            pDest_x = 0;
        }
        if (pDest_x < pDest->width) {
            if (pDest_x + pWidth > pDest->width) {
                x_delta = pDest_x + pWidth - pDest->width;
                pWidth -= x_delta;
                source_row_wrap += x_delta;
                dest_row_wrap += x_delta;
            }
            for (y = 0; y < pHeight; y++) {
                for (x = 0; x < pWidth; x++) {
                    the_byte = *source_ptr;
                    if (the_byte != 0) {
                        *dest_ptr = the_byte;
                    }
                    source_ptr++;
                    dest_ptr++;
                }
                source_ptr += source_row_wrap;
                dest_ptr += dest_row_wrap;
            }
        }
    }
#else
    __asm {
            sub esp, 8
            movsx eax, word ptr [esp + 0x18]
            push ebx
            push ebp
            push esi
            push edi
            mov edi, dword ptr [esp + 0x20]
            mov si, dx
            mov word ptr [esp + 0x10], si
            movsx ebp, word ptr [edi + 0x28]
            imul eax, ebp
            cdq
            sub eax, edx
            movsx edx, word ptr [esp + 0x24]
            sar eax, 1
            add eax, edx
            mov edx, dword ptr [edi + 8]
            lea edi, [edx + eax*2]
            movsx eax, word ptr [esp + 0x1c]
            movsx edx, word ptr [ecx + 0x28]
            mov dword ptr [esp + 0x20], eax
            imul eax, edx
            cdq
            sub eax, edx
            xor edx, edx
            mov dx, word ptr [ecx + 0x30]
            mov ebx, eax
            movsx eax, si
            shr edx, 1
            sar ebx, 1
            add edx, eax
            mov dword ptr [esp + 0x14], eax
            mov eax, dword ptr [ecx + 8]
            add edx, ebx
            lea esi, [eax + edx*2]
            mov eax, ebp
            movsx edx, word ptr [esp + 0x2c]
            mov dword ptr [esp + 0x24], edx
            cdq
            sub eax, edx
            mov edx, dword ptr [esp + 0x24]
            sar eax, 1
            sub eax, edx
            mov dword ptr [esp + 0x28], eax
            movsx eax, word ptr [ecx + 0x28]
            cdq
            sub eax, edx
            mov edx, eax
            mov eax, dword ptr [esp + 0x24]
            sar edx, 1
            sub edx, eax
            mov eax, dword ptr [esp + 0x1c]
            test ax, ax
            mov dword ptr [esp + 0x24], edx
            jge L0047bb52
            mov edx, dword ptr [esp + 0x30]
            add edx, eax
            test dx, dx
            mov dword ptr [esp + 0x30], edx
            jle L0047bc32
            mov eax, dword ptr [esp + 0x20]
            mov dword ptr [esp + 0x1c], 0
            imul eax, ebp
            cdq
            sub eax, edx
            mov edx, dword ptr [esp + 0x24]
            sar eax, 1
            neg eax
            neg ebx
            lea edi, [edi + eax*2]
            mov eax, dword ptr [esp + 0x1c]
            lea esi, [esi + ebx*2]
    L0047bb52:
            mov bx, word ptr [ecx + 0x36]
            movsx ebp, ax
            mov dword ptr [esp + 0x1c], ebx
            and ebx, 0xffff
            cmp ebp, ebx
            mov dword ptr [esp + 0x24], ebx
            jge L0047bc32
            movsx ebx, word ptr [esp + 0x30]
            add ebx, ebp
            mov ebp, dword ptr [esp + 0x24]
            cmp ebx, ebp
            jle L0047bb88
            mov ebx, dword ptr [esp + 0x1c]
            sub ebx, eax
            mov dword ptr [esp + 0x30], ebx
    L0047bb88:
            cmp word ptr [esp + 0x10], 0
            mov ebp, dword ptr [esp + 0x2c]
            jge L0047bbc4
            add ebp, dword ptr [esp + 0x10]
            test bp, bp
            mov dword ptr [esp + 0x2c], ebp
            jle L0047bc32
            mov ebx, dword ptr [esp + 0x14]
            mov dword ptr [esp + 0x10], 0
            lea eax, [ebx + ebx]
            sub edi, eax
            sub esi, eax
            mov eax, dword ptr [esp + 0x28]
            sub eax, ebx
            sub edx, ebx
            mov dword ptr [esp + 0x28], eax
    L0047bbc4:
            xor ebx, ebx
            mov bx, word ptr [ecx + 0x34]
            movsx ecx, word ptr [esp + 0x10]
            cmp ecx, ebx
            jge L0047bc32
            movsx eax, bp
            lea ebp, [eax + ecx]
            cmp ebp, ebx
            jle L0047bbf5
            mov ebp, dword ptr [esp + 0x2c]
            add eax, ecx
            sub eax, ebx
            sub ebp, eax
            mov dword ptr [esp + 0x2c], ebp
            mov ebp, dword ptr [esp + 0x28]
            add ebp, eax
            add edx, eax
            jmp L0047bbf9
    L0047bbf5:
            mov ebp, dword ptr [esp + 0x28]
    L0047bbf9:
            movsx eax, word ptr [esp + 0x30]
            test eax, eax
            jle L0047bc32
            movsx ecx, word ptr [esp + 0x2c]
            mov dword ptr [esp + 0x24], ecx
            mov ebx, eax
    L0047bc0d:
            test ecx, ecx
            jle L0047bc29
    L0047bc11:
            mov ax, word ptr [edi]
            add edi, 2
            test ax, ax
            je L0047bc1f
            mov word ptr [esi], ax
    L0047bc1f:
            add esi, 2
            dec ecx
            jne L0047bc11
            mov ecx, dword ptr [esp + 0x24]
    L0047bc29:
            dec ebx
            lea esi, [esi + edx*2]
            lea edi, [edi + ebp*2]
            jne L0047bc0d
    L0047bc32:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 8
            ret 0x18
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0047bc40
void C2_HOOK_FASTCALL DRPixelmapBlit(br_int_16 pX, br_int_16 pY, const br_pixelmap* pSource) {
    DRPixelmapRectangleMaskedCopy(gBack_screen, pX, pY, pSource, 0, 0, pSource->width, pSource->height);
}

// FUNCTION: CARMA2_HW 0x0047bc80
void C2_NAKED C2_HOOK_FASTCALL FUN_0047bc80(br_pixelmap* pDest, br_int_16 pDest_x, br_int_16 pDest_y, const br_pixelmap* pSource, br_int_16 pSource_x, br_int_16 pSource_y, br_int_16 pWidth, br_int_16 pHeight) {
#ifndef CARPOCALYPSE2_MATCHING
    int x;
    int y;
    int dest_row_wrap;
    int source_row_wrap;
    int row_bytes;
    tS16 pixel;
    tS16* source_ptr;
    tS16* dest_ptr;

    row_bytes = (br_int_16)pSource->row_bytes;
    source_ptr = (tS16*)pSource->pixels + (pSource_y * row_bytes / 2 + pSource_x);
    row_bytes = (br_int_16)pDest->row_bytes;
    dest_ptr = (tS16*)pDest->pixels + (pDest_y * row_bytes / 2 + pDest_x);
    source_row_wrap = ((br_int_16)pSource->row_bytes) / 2 - pWidth;
    dest_row_wrap = ((br_int_16)pDest->row_bytes) / 2 - pWidth;

    for (y = 0; y < pHeight; y++) {
        for (x = 0; x < pWidth; x++) {
            pixel = *source_ptr++;
            if (pixel != 0) {
                *dest_ptr = pixel;
            }
            dest_ptr++;
        }
        dest_ptr += dest_row_wrap;
        source_ptr += source_row_wrap;
    }
#else
    __asm {
            movsx eax, word ptr [esp + 0x10]
            push ebx
            push ebp
            push esi
            push edi
            mov edi, dword ptr [esp + 0x18]
            mov si, dx
            movsx ebp, word ptr [ecx + 0x28]
            movsx ebx, word ptr [edi + 0x28]
            imul eax, ebx
            cdq
            sub eax, edx
            mov ecx, dword ptr [ecx + 8]
            movsx edx, word ptr [esp + 0x1c]
            sar eax, 1
            add eax, edx
            mov edx, dword ptr [edi + 8]
            lea edi, [edx + eax*2]
            movsx eax, word ptr [esp + 0x14]
            imul eax, ebp
            cdq
            sub eax, edx
            movsx edx, si
            sar eax, 1
            add eax, edx
            lea esi, [ecx + eax*2]
            mov eax, ebx
            cdq
            movsx ecx, word ptr [esp + 0x24]
            sub eax, edx
            mov ebx, eax
            mov eax, ebp
            cdq
            sub eax, edx
            sar ebx, 1
            sar eax, 1
            sub ebx, ecx
            sub eax, ecx
            movsx ecx, word ptr [esp + 0x28]
            test ecx, ecx
            jle L0047bd11
            mov ebp, ecx
    L0047bce9:
            movsx ecx, word ptr [esp + 0x24]
            test ecx, ecx
            jle L0047bd08
            mov edx, ecx
    L0047bcf4:
            mov cx, word ptr [edi]
            add edi, 2
            test cx, cx
            je L0047bd02
            mov word ptr [esi], cx
    L0047bd02:
            add esi, 2
            dec edx
            jne L0047bcf4
    L0047bd08:
            dec ebp
            lea esi, [esi + eax*2]
            lea edi, [edi + ebx*2]
            jne L0047bce9
    L0047bd11:
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 0x18
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            sub esp, 0x14
            push ebx
            push ebp
            push esi
            mov esi, ecx
            mov ecx, dword ptr [esp + 0x28]
            xor eax, eax
            mov dword ptr [esp + 0x10], eax
            mov dword ptr [esp + 0x18], eax
            movsx ebx, word ptr [ecx + 0x28]
            movsx eax, word ptr [esp + 0x30]
            imul eax, ebx
            mov word ptr [esp + 0xc], dx
            mov ecx, dword ptr [ecx + 8]
            cdq
            sub eax, edx
            push edi
            movsx edx, word ptr [esp + 0x30]
            sar eax, 1
            add eax, edx
            mov dword ptr [esp + 0x18], esi
            mov dword ptr [esp + 0x2c], ebx
            lea edi, [ecx + eax*2]
            movsx eax, word ptr [esp + 0x28]
            movsx ecx, word ptr [esi + 0x28]
            mov dword ptr [esp + 0x30], eax
            imul eax, ecx
            cdq
            sub eax, edx
            mov edx, dword ptr [esi + 8]
            mov ebp, eax
            movsx eax, word ptr [esp + 0x10]
            sar ebp, 1
            mov dword ptr [esp + 0x20], eax
            add eax, ebp
            lea esi, [edx + eax*2]
            movsx eax, word ptr [esp + 0x38]
            mov dword ptr [esp + 0x34], eax
            mov eax, ebx
            cdq
            sub eax, edx
            mov edx, dword ptr [esp + 0x34]
            mov ebx, eax
            mov eax, ecx
            sar ebx, 1
            sub ebx, edx
            cdq
            sub eax, edx
            mov ecx, eax
            mov eax, dword ptr [esp + 0x34]
            sar ecx, 1
            sub ecx, eax
            mov eax, dword ptr [esp + 0x28]
            test ax, ax
            jge L0047bdf1
            mov edx, dword ptr [esp + 0x3c]
            add edx, eax
            test dx, dx
            mov dword ptr [esp + 0x3c], edx
            jle L0047bf7b
            mov eax, dword ptr [esp + 0x30]
            mov dword ptr [esp + 0x28], 0
            imul eax, dword ptr [esp + 0x2c]
            cdq
            sub eax, edx
            sar eax, 1
            neg eax
            neg ebp
            lea edi, [edi + eax*2]
            mov eax, dword ptr [esp + 0x28]
            lea esi, [esi + ebp*2]
    L0047bdf1:
            mov edx, dword ptr [esp + 0x18]
            movsx ebp, ax
            mov dx, word ptr [edx + 0x36]
            mov dword ptr [esp + 0x30], edx
            and edx, 0xffff
            cmp ebp, edx
            mov dword ptr [esp + 0x34], edx
            jge L0047bf7b
            movsx edx, word ptr [esp + 0x3c]
            add edx, ebp
            mov ebp, dword ptr [esp + 0x34]
            cmp edx, ebp
            jle L0047be2b
            mov edx, dword ptr [esp + 0x30]
            sub edx, eax
            mov dword ptr [esp + 0x3c], edx
    L0047be2b:
            cmp word ptr [esp + 0x10], 0
            mov ebp, dword ptr [esp + 0x38]
            jge L0047be5f
            add ebp, dword ptr [esp + 0x10]
            test bp, bp
            mov dword ptr [esp + 0x38], ebp
            jle L0047bf7b
            mov edx, dword ptr [esp + 0x20]
            mov dword ptr [esp + 0x10], 0
            sub ebx, edx
            lea eax, [edx + edx]
            sub edi, eax
            sub esi, eax
            sub ecx, edx
    L0047be5f:
            mov eax, dword ptr [esp + 0x18]
            xor edx, edx
            mov dx, word ptr [eax + 0x34]
            movsx eax, word ptr [esp + 0x10]
            cmp eax, edx
            jge L0047bf7b
            movsx ebp, bp
            add ebp, eax
            cmp ebp, edx
            jle L0047be96
            movsx ebp, word ptr [esp + 0x38]
            add eax, ebp
            sub eax, edx
            mov edx, dword ptr [esp + 0x38]
            sub edx, eax
            add ebx, eax
            mov dword ptr [esp + 0x38], edx
            add ecx, eax
    L0047be96:
            movsx eax, word ptr [esp + 0x3c]
            test eax, eax
            mov dword ptr [esp + 0x34], 0
            mov dword ptr [esp + 0x30], eax
            jle L0047bf7b
    L0047beaf:
            movsx eax, word ptr [esp + 0x38]
            test eax, eax
            jle L0047bece
            mov edx, eax
    L0047beba:
            mov ax, word ptr [edi]
            add edi, 2
            test ax, ax
            je L0047bec8
            mov word ptr [esi], ax
    L0047bec8:
            add esi, 2
            dec edx
            jne L0047beba
    L0047bece:
            mov eax, dword ptr [esp + 0x14]
            mov ebp, dword ptr [esp + 0x1c]
            sar eax, 0x10
            mov edx, eax
            mov dword ptr [esp + 0x1c], eax
            mov eax, dword ptr [esp + 0x40]
            sub edx, ebp
            lea edi, [edi + ebx*2]
            lea ebp, [edx + ecx]
            lea esi, [esi + ebp*2]
            mov ebp, dword ptr [esp + 0x14]
            add ebp, eax
            mov dword ptr [esp + 0x14], ebp
            mov ebp, dword ptr [esp + 0x10]
            add ebp, edx
            mov dword ptr [esp + 0x10], ebp
            mov ax, word ptr [esp + 0x10]
            test ax, ax
            jge L0047bf33
            movsx eax, ax
            mov edx, ebp
            mov ebp, dword ptr [esp + 0x38]
            add ebp, edx
            lea edx, [eax + eax]
            sub edi, edx
            sub esi, edx
            sub ebx, eax
            mov dword ptr [esp + 0x10], 0
            sub ecx, eax
            mov ax, word ptr [esp + 0x10]
            mov dword ptr [esp + 0x38], ebp
    L0047bf33:
            mov ebp, dword ptr [esp + 0x18]
            xor edx, edx
            mov dx, word ptr [ebp + 0x34]
            movsx ebp, ax
            cmp ebp, edx
            jge L0047bf7b
            movsx eax, word ptr [esp + 0x38]
            add eax, ebp
            cmp eax, edx
            jle L0047bf66
            movsx eax, word ptr [esp + 0x38]
            add eax, ebp
            sub eax, edx
            mov edx, dword ptr [esp + 0x38]
            sub edx, eax
            add ebx, eax
            mov dword ptr [esp + 0x38], edx
            add ecx, eax
    L0047bf66:
            mov eax, dword ptr [esp + 0x34]
            mov edx, dword ptr [esp + 0x30]
            inc eax
            cmp eax, edx
            mov dword ptr [esp + 0x34], eax
            jl L0047beaf
    L0047bf7b:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x14
            ret 0x1c
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
            cmp word ptr [esp + 0x18], 0
            push ebx
            push ebp
            push esi
            push edi
            mov di, dx
            je L0047c083
            mov esi, dword ptr [esp + 0x18]
            movsx eax, word ptr [esp + 0x20]
            movsx ebp, word ptr [esi + 0x28]
            imul eax, ebp
            cdq
            sub eax, edx
            movsx edx, word ptr [esp + 0x1c]
            movsx ebx, word ptr [ecx + 0x28]
            sar eax, 1
            add eax, edx
            mov edx, dword ptr [esi + 8]
            mov ecx, dword ptr [ecx + 8]
            lea esi, [edx + eax*2]
            movsx eax, word ptr [esp + 0x14]
            imul eax, ebx
            cdq
            sub eax, edx
            movsx edx, di
            sar eax, 1
            add eax, edx
            movsx edi, word ptr [esp + 0x24]
            lea ecx, [ecx + eax*2]
            mov eax, ebp
            cdq
            sub eax, edx
            mov ebp, eax
            mov eax, ebx
            cdq
            sub eax, edx
            mov edx, dword ptr [esp + 0x18]
            sar eax, 1
            sub eax, edi
            movsx ebx, word ptr [esp + 0x28]
            mov dword ptr [esp + 0x1c], eax
            xor eax, eax
            mov ax, word ptr [edx + 0x36]
            shl eax, 0x10
            cdq
            idiv ebx
            sar ebp, 1
            sub ebp, edi
            xor edi, edi
            mov edx, eax
            sub edx, 0x10000
            test ebx, ebx
            mov dword ptr [esp + 0x20], edx
            jle L0047c083
            mov dword ptr [esp + 0x28], ebx
            jmp L0047c02f
    L0047c02b:
            mov edx, dword ptr [esp + 0x20]
    L0047c02f:
            movsx eax, word ptr [esp + 0x24]
            test eax, eax
            jle L0047c04e
            mov ebx, eax
    L0047c03a:
            mov ax, word ptr [esi]
            add esi, 2
            test ax, ax
            je L0047c048
            mov word ptr [ecx], ax
    L0047c048:
            add ecx, 2
            dec ebx
            jne L0047c03a
    L0047c04e:
            mov eax, dword ptr [esp + 0x1c]
            lea ecx, [ecx + eax*2]
            mov eax, edi
            add edi, edx
            mov edx, dword ptr [esp + 0x18]
            mov ebx, edi
            shr ebx, 0x10
            shr eax, 0x10
            sub ebx, eax
            movsx eax, word ptr [edx + 0x28]
            cdq
            sub eax, edx
            sar eax, 1
            imul ebx, eax
            mov eax, dword ptr [esp + 0x28]
            add ebx, ebp
            dec eax
            lea esi, [esi + ebx*2]
            mov dword ptr [esp + 0x28], eax
            jne L0047c02b
    L0047c083:
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 0x18
            nop
            nop
            nop
            nop
            nop
            nop
            sub esp, 0x14
            push ebx
            mov ebx, dword ptr [esp + 0x20]
            push ebp
            mov ebp, dword ptr [esp + 0x20]
            push esi
            mov esi, ecx
            xor eax, eax
            sub ebp, ebx
            mov ax, word ptr [esi + 0x36]
            push edi
            add ebp, eax
            mov dword ptr [esp + 0x10], edx
            mov dword ptr [esp + 0x1c], ebp
            call PDGetTotalTime
            mov edi, eax
            mov dword ptr [esp + 0x2c], edi
            call PDGetTotalTime
            lea ecx, [edi + 0x64]
            cmp eax, ecx
            mov dword ptr [esp + 0x20], ecx
            jge L0047c19b
            mov ecx, dword ptr [esp + 0x30]
            sub ecx, ebx
            mov dword ptr [esp + 0x14], ecx
            jmp L0047c0e2
    L0047c0de:
            mov edi, dword ptr [esp + 0x2c]
    L0047c0e2:
            sub eax, edi
            push 0
            sub eax, 0x64
            imul eax, ebp
            mov ecx, eax
            mov eax, 0x51eb851f
            imul ecx
            sar edx, 5
            mov ecx, edx
            xor eax, eax
            mov ax, word ptr [esi + 0x34]
            shr ecx, 0x1f
            add edx, ecx
            mov ecx, dword ptr [esp + 0x14]
            mov edi, edx
            mov edx, dword ptr [esp + 0x18]
            push edx
            mov edx, dword ptr [gBack_screen]
            push eax
            push ebx
            push ecx
            push edx
            call BrPixelmapRectangleFill
            add esp, 0x18
            cmp edi, 0x3e8
            je L0047c18c
            mov eax, dword ptr [esp + 0x28]
            xor ecx, ecx
            mov cx, word ptr [esi + 0x36]
            mov dword ptr [esp + 0x18], 0
            lea edx, [edi + eax]
            mov eax, ebx
            sub eax, edx
            mov ebp, ecx
            test eax, eax
            jle L0047c151
            sub ebp, eax
            mov edi, eax
            add edx, eax
            jmp L0047c155
    L0047c151:
            mov edi, dword ptr [esp + 0x18]
    L0047c155:
            mov eax, dword ptr [esp + 0x30]
            sub eax, edx
            sub eax, ecx
            jns L0047c161
            add ebp, eax
    L0047c161:
            xor ecx, ecx
            mov eax, dword ptr [gBack_screen]
            mov cx, word ptr [esi + 0x34]
            push ebp
            push ecx
            push edi
            push 0
            push esi
            push edx
            mov edx, dword ptr [esp + 0x28]
            push edx
            push eax
            call BrPixelmapRectangleCopy
            add esp, 0x20
            xor ecx, ecx
            call PDScreenBufferSwap
            mov ebp, dword ptr [esp + 0x1c]
    L0047c18c:
            call PDGetTotalTime
            cmp eax, dword ptr [esp + 0x20]
            jl L0047c0de
    L0047c19b:
            mov edi, dword ptr [esp + 0x30]
            mov eax, dword ptr [esp + 0x10]
            mov ecx, edi
            xor edx, edx
            mov dx, word ptr [esi + 0x34]
            sub ecx, ebx
            push 0
            push ecx
            mov ecx, dword ptr [gBack_screen]
            push edx
            push ebx
            push eax
            push ecx
            call BrPixelmapRectangleFill
            mov ecx, dword ptr [esp + 0x40]
            xor eax, eax
            mov ax, word ptr [esi + 0x36]
            sub ebx, ecx
            add esp, 0x18
            xor ebp, ebp
            test ebx, ebx
            mov edx, eax
            jle L0047c1dc
            sub edx, ebx
            mov ebp, ebx
            add ecx, ebx
    L0047c1dc:
            sub edi, ecx
            sub edi, eax
            jns L0047c1e4
            add edx, edi
    L0047c1e4:
            push edx
            xor edx, edx
            mov dx, word ptr [esi + 0x34]
            mov eax, dword ptr [esp + 0x14]
            push edx
            push ebp
            push 0
            push esi
            push ecx
            mov ecx, dword ptr [gBack_screen]
            push eax
            push ecx
            call BrPixelmapRectangleCopy
            add esp, 0x20
            xor ecx, ecx
            call PDScreenBufferSwap
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x14
            ret 0xc
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
            sub esp, 0x14
            push ebx
            push ebp
            mov ebp, dword ptr [esp + 0x28]
            push esi
            mov esi, dword ptr [esp + 0x24]
            mov ebx, ebp
            push edi
            sub ebx, esi
            mov dword ptr [esp + 0x10], edx
            mov edi, ecx
            mov dword ptr [esp + 0x1c], ebx
            call PDGetTotalTime
            mov esi, eax
            mov dword ptr [esp + 0x14], esi
            call PDGetTotalTime
            lea ecx, [esi + 0x64]
            cmp eax, ecx
            mov dword ptr [esp + 0x20], ecx
            jge L0047c320
            mov ecx, ebp
            mov ebp, dword ptr [esp + 0x2c]
            sub ecx, ebp
            mov dword ptr [esp + 0x18], ecx
            jmp L0047c26e
    L0047c26a:
            mov esi, dword ptr [esp + 0x14]
    L0047c26e:
            sub eax, esi
            push 0
            imul eax, ebx
            mov ecx, eax
            mov eax, 0x51eb851f
            imul ecx
            sar edx, 5
            mov eax, edx
            mov ecx, dword ptr [esp + 0x1c]
            shr eax, 0x1f
            add edx, eax
            mov eax, dword ptr [esp + 0x14]
            mov esi, edx
            xor edx, edx
            mov dx, word ptr [edi + 0x34]
            push ecx
            mov ecx, dword ptr [gBack_screen]
            push edx
            push ebp
            push eax
            push ecx
            call BrPixelmapRectangleFill
            add esp, 0x18
            cmp esi, 0x3e8
            je L0047c30f
            mov edx, dword ptr [esp + 0x28]
            mov eax, dword ptr [esp + 0x2c]
            add edx, esi
            xor ecx, ecx
            mov cx, word ptr [edi + 0x36]
            sub eax, edx
            xor ebp, ebp
            mov ebx, ecx
            test eax, eax
            jle L0047c2d3
            sub ebx, eax
            mov ebp, eax
            add edx, eax
    L0047c2d3:
            mov eax, dword ptr [esp + 0x30]
            sub eax, edx
            sub eax, ecx
            jns L0047c2df
            add ebx, eax
    L0047c2df:
            xor eax, eax
            mov ecx, dword ptr [esp + 0x10]
            mov ax, word ptr [edi + 0x34]
            push ebx
            push eax
            push ebp
            push 0
            push edi
            push edx
            mov edx, dword ptr [gBack_screen]
            push ecx
            push edx
            call BrPixelmapRectangleCopy
            add esp, 0x20
            xor ecx, ecx
            call PDScreenBufferSwap
            mov ebp, dword ptr [esp + 0x2c]
            mov ebx, dword ptr [esp + 0x1c]
    L0047c30f:
            call PDGetTotalTime
            cmp eax, dword ptr [esp + 0x20]
            jl L0047c26a
            jmp L0047c324
    L0047c320:
            mov ebp, dword ptr [esp + 0x2c]
    L0047c324:
            mov eax, dword ptr [esp + 0x30]
            mov ecx, dword ptr [esp + 0x10]
            sub eax, ebp
            mov edx, dword ptr [gBack_screen]
            push 0
            push eax
            xor eax, eax
            mov ax, word ptr [edi + 0x34]
            push eax
            push ebp
            push ecx
            push edx
            call BrPixelmapRectangleFill
            add esp, 0x18
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x14
            ret 0xc
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
            sub esp, 0x14
            push ebx
            push ebp
            mov ebp, dword ptr [esp + 0x28]
            push esi
            push edi
            mov edi, dword ptr [esp + 0x28]
            sub ebp, edi
            mov dword ptr [esp + 0x10], edx
            mov esi, ecx
            mov dword ptr [esp + 0x1c], ebp
            call PDGetTotalTime
            mov edi, eax
            mov dword ptr [esp + 0x14], edi
            call PDGetTotalTime
            mov ebx, dword ptr [esp + 0x2c]
            lea ecx, [edi + 0x64]
            cmp eax, ecx
            mov dword ptr [esp + 0x20], ecx
            jge L0047c467
            mov ecx, dword ptr [esp + 0x30]
            sub ecx, ebx
            mov dword ptr [esp + 0x2c], ecx
            jmp L0047c3ae
    L0047c3aa:
            mov edi, dword ptr [esp + 0x14]
    L0047c3ae:
            mov ecx, edi
            push 0
            sub ecx, eax
            mov eax, 0x51eb851f
            add ecx, 0x64
            imul ecx, ebp
            imul ecx
            sar edx, 5
            mov eax, edx
            mov ecx, dword ptr [esp + 0x30]
            shr eax, 0x1f
            add edx, eax
            mov eax, dword ptr [esp + 0x14]
            mov edi, edx
            xor edx, edx
            mov dx, word ptr [esi + 0x34]
            push ecx
            mov ecx, dword ptr [gBack_screen]
            push edx
            push ebx
            push eax
            push ecx
            call BrPixelmapRectangleFill
            add esp, 0x18
            cmp edi, 0x3e8
            je L0047c458
            mov edx, dword ptr [esp + 0x28]
            mov eax, ebx
            add edx, edi
            xor ecx, ecx
            mov cx, word ptr [esi + 0x36]
            sub eax, edx
            test eax, eax
            mov dword ptr [esp + 0x18], 0
            mov ebp, ecx
            jle L0047c41c
            sub ebp, eax
            mov edi, eax
            add edx, eax
            jmp L0047c420
    L0047c41c:
            mov edi, dword ptr [esp + 0x18]
    L0047c420:
            mov eax, dword ptr [esp + 0x30]
            sub eax, edx
            sub eax, ecx
            jns L0047c42c
            add ebp, eax
    L0047c42c:
            xor eax, eax
            mov ecx, dword ptr [esp + 0x10]
            mov ax, word ptr [esi + 0x34]
            push ebp
            push eax
            push edi
            push 0
            push esi
            push edx
            mov edx, dword ptr [gBack_screen]
            push ecx
            push edx
            call BrPixelmapRectangleCopy
            add esp, 0x20
            xor ecx, ecx
            call PDScreenBufferSwap
            mov ebp, dword ptr [esp + 0x1c]
    L0047c458:
            call PDGetTotalTime
            cmp eax, dword ptr [esp + 0x20]
            jl L0047c3aa
    L0047c467:
            mov edi, dword ptr [esp + 0x30]
            mov edx, dword ptr [esp + 0x10]
            mov eax, edi
            xor ecx, ecx
            mov cx, word ptr [esi + 0x34]
            sub eax, ebx
            push 0
            push eax
            mov eax, dword ptr [gBack_screen]
            push ecx
            push ebx
            push edx
            push eax
            call BrPixelmapRectangleFill
            mov ecx, dword ptr [esp + 0x40]
            xor eax, eax
            mov ax, word ptr [esi + 0x36]
            sub ebx, ecx
            add esp, 0x18
            xor ebp, ebp
            test ebx, ebx
            mov edx, eax
            jle L0047c4a7
            sub edx, ebx
            mov ebp, ebx
            add ecx, ebx
    L0047c4a7:
            sub edi, ecx
            sub edi, eax
            jns L0047c4af
            add edx, edi
    L0047c4af:
            push edx
            xor edx, edx
            mov dx, word ptr [esi + 0x34]
            mov eax, dword ptr [esp + 0x14]
            push edx
            push ebp
            push 0
            push esi
            push ecx
            mov ecx, dword ptr [gBack_screen]
            push eax
            push ecx
            call BrPixelmapRectangleCopy
            add esp, 0x20
            xor ecx, ecx
            call PDScreenBufferSwap
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x14
            ret 0xc
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
            nop
            sub esp, 0x14
            push ebx
            mov ebx, dword ptr [esp + 0x1c]
            push ebp
            mov ebp, dword ptr [esp + 0x24]
            push esi
            push edi
            mov edi, ecx
            xor eax, eax
            sub ebx, ebp
            mov dword ptr [esp + 0x10], edx
            mov ax, word ptr [edi + 0x36]
            add ebx, eax
            mov dword ptr [esp + 0x1c], ebx
            call PDGetTotalTime
            mov esi, eax
            mov dword ptr [esp + 0x2c], esi
            call PDGetTotalTime
            lea ecx, [esi + 0x64]
            cmp eax, ecx
            mov dword ptr [esp + 0x20], ecx
            jge L0047c5f8
            mov ecx, dword ptr [esp + 0x30]
            sub ecx, ebp
            mov dword ptr [esp + 0x14], ecx
            jmp L0047c542
    L0047c53e:
            mov esi, dword ptr [esp + 0x2c]
    L0047c542:
            mov ecx, esi
            push 0
            sub ecx, eax
            mov eax, 0x51eb851f
            imul ecx, ebx
            imul ecx
            sar edx, 5
            mov ecx, edx
            xor eax, eax
            mov ax, word ptr [edi + 0x34]
            shr ecx, 0x1f
            add edx, ecx
            mov ecx, dword ptr [esp + 0x14]
            mov esi, edx
            mov edx, dword ptr [esp + 0x18]
            push edx
            mov edx, dword ptr [gBack_screen]
            push eax
            push ebp
            push ecx
            push edx
            call BrPixelmapRectangleFill
            add esp, 0x18
            cmp esi, 0x3e8
            je L0047c5e9
            mov eax, dword ptr [esp + 0x28]
            xor ecx, ecx
            mov cx, word ptr [edi + 0x36]
            mov dword ptr [esp + 0x18], 0
            lea edx, [esi + eax]
            mov eax, ebp
            sub eax, edx
            mov ebx, ecx
            test eax, eax
            jle L0047c5ae
            sub ebx, eax
            mov esi, eax
            add edx, eax
            jmp L0047c5b2
    L0047c5ae:
            mov esi, dword ptr [esp + 0x18]
    L0047c5b2:
            mov eax, dword ptr [esp + 0x30]
            sub eax, edx
            sub eax, ecx
            jns L0047c5be
            add ebx, eax
    L0047c5be:
            xor ecx, ecx
            mov eax, dword ptr [gBack_screen]
            mov cx, word ptr [edi + 0x34]
            push ebx
            push ecx
            push esi
            push 0
            push edi
            push edx
            mov edx, dword ptr [esp + 0x28]
            push edx
            push eax
            call BrPixelmapRectangleCopy
            add esp, 0x20
            xor ecx, ecx
            call PDScreenBufferSwap
            mov ebx, dword ptr [esp + 0x1c]
    L0047c5e9:
            call PDGetTotalTime
            cmp eax, dword ptr [esp + 0x20]
            jl L0047c53e
    L0047c5f8:
            mov eax, dword ptr [esp + 0x30]
            mov edx, dword ptr [esp + 0x10]
            xor ecx, ecx
            sub eax, ebp
            mov cx, word ptr [edi + 0x34]
            push 0
            push eax
            mov eax, dword ptr [gBack_screen]
            push ecx
            push ebp
            push edx
            push eax
            call BrPixelmapRectangleFill
            add esp, 0x18
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x14
            ret 0xc
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
            ret 4
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
            ret 4
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

// DRMaskedStamp

// SwitchToHiresMode

// SwitchToLoresMode

// PoshDrawLine

// DeviouslyDimRectangle

// DimRectangle

// DimRectangleClipped

// DR8BitPixelmapRotatedAndFeatheredCopy

// DR16BitPixelmapRotatedAndFeatheredCopy

// DRPixelmapRotatedAndFeatheredCopy

// DRPixelmapBlendedLine

// DR8BitFancyDrawLine

// FancyDrawLine

// DRPixelmapCopyMapBlack8Bit

// DRPixelmapCopyMapBlack

// DisableLights

// FUNCTION: CARMA2_HW 0x0047d6d0
void C2_HOOK_FASTCALL EnableLights(void) {
    int i;

    for (i = 0; i < gNumber_of_lights; i++) {
        BrLightEnable(gLight_array[i]);
    }
}

// FUNCTION: CARMA2_HW 0x0047d700
void C2_NAKED C2_HOOK_FASTCALL FUN_0047d700(br_pixelmap* pDest, const br_pixelmap* pSource, unsigned int pArg3, unsigned int pDest_y, unsigned int pWidth, unsigned int pHeight) {
#ifndef CARPOCALYPSE2_MATCHING
    const tU16* source_ptr;
    tU16* dest_ptr;
    unsigned int x;
    unsigned int y;

    y = 0;
    if ((pHeight & 0xffff) != 0) {
        pDest_y &= 0xffff;
        pWidth &= 0xffff;
        pHeight &= 0xffff;
        do {
            source_ptr = (const tU16*)((const tU8*)pSource->pixels + y * pSource->row_bytes);
            dest_ptr = (tU16*)((tU8*)pDest->pixels + pDest_y * pDest->row_bytes);
            if (pWidth != 0) {
                for (x = 0; x < pWidth; x++) {
                    gPixelmap_copy_buffer[x] = source_ptr[x];
                }
            }
            memcpy(dest_ptr, gPixelmap_copy_buffer, pWidth * sizeof(tU16));
            y++;
            pDest_y++;
        } while (y < pHeight);
    }
    (void)pArg3;
#else
    __asm {
            sub esp, 0xc
            mov eax, dword ptr [esp + 0x1c]
            push ebp
            push esi
            xor ebp, ebp
            mov esi, edx
            and eax, 0xffff
            mov dword ptr [esp + 8], esi
            mov dword ptr [esp + 0xc], ecx
            mov dword ptr [esp + 0x10], eax
            jle L0047d7a6
            mov edx, dword ptr [esp + 0x1c]
            push edi
            push ebx
            mov ebx, dword ptr [esp + 0x28]
            and ebx, 0xffff
            and edx, 0xffff
            mov dword ptr [esp + 0x2c], edx
            jmp L0047d748
    L0047d740:
            mov esi, dword ptr [esp + 0x10]
            mov ecx, dword ptr [esp + 0x14]
    L0047d748:
            movsx edi, word ptr [esi + 0x28]
            mov eax, ebp
            imul eax, edi
            mov edi, dword ptr [esi + 8]
            movsx esi, word ptr [ecx + 0x28]
            add eax, edi
            mov edi, edx
            imul edi, esi
            add edi, dword ptr [ecx + 8]
            test ebx, ebx
            jle L0047d780
            mov ecx, offset gNet_mode+0x8
            mov esi, ebx
            sub eax, ecx
    L0047d76f:
            mov dx, word ptr [eax + ecx]
            mov word ptr [ecx], dx
            add ecx, 2
            dec esi
            jne L0047d76f
            mov edx, dword ptr [esp + 0x2c]
    L0047d780:
            lea ecx, [ebx + ebx]
            mov esi, offset gNet_mode+0x8
            mov eax, ecx
            shr ecx, 2
            rep movsd
            mov ecx, eax
            mov eax, dword ptr [esp + 0x18]
            and ecx, 3
            inc ebp
            inc edx
            cmp ebp, eax
            rep movsb
            mov dword ptr [esp + 0x2c], edx
            jl L0047d740
            pop ebx
            pop edi
    L0047d7a6:
            pop esi
            pop ebp
            add esp, 0xc
            ret 0x10
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0047d7b0
void C2_NAKED C2_HOOK_FASTCALL DRPixelmapBlend75Copy(br_pixelmap* pDest, br_int_16 pDest_x, br_int_16 pDest_y, br_pixelmap* pSource) {
#ifndef CARPOCALYPSE2_MATCHING
    int x;
    int y;
    tU8* dest_ptr;

    for (y = 0; y < pSource->height; y++) {
        dest_ptr = (tU8*)pDest->pixels + (pDest_y + y) * pDest->row_bytes + pDest_x;
        for (x = 0; x < pSource->width; x++) {
            *dest_ptr = ((tU8*)gPalette_0074a5fc->pixels)[*dest_ptr + 256 * ((tU8*)pSource->pixels)[y * pSource->row_bytes + x]];
            dest_ptr++;
        }
    }
#else
    __asm {
            push ecx
            push esi
            mov esi, dword ptr [esp + 0x10]
            push edi
            xor edi, edi
            cmp word ptr [esi + 0x36], di
            mov dword ptr [esp + 8], ecx
            jbe L0047d840
            movsx eax, dx
            mov dword ptr [esp + 0x14], eax
            push ebp
            movsx eax, word ptr [esp + 0x14]
            push ebx
            mov dword ptr [esp + 0x18], eax
    L0047d7d5:
            movsx edx, word ptr [ecx + 0x28]
            imul eax, edx
            mov edx, dword ptr [ecx + 8]
            mov ebp, dword ptr [esp + 0x1c]
            add eax, edx
            xor edx, edx
            add eax, ebp
            cmp word ptr [esi + 0x34], dx
            jbe L0047d82a
    L0047d7ef:
            movsx ecx, word ptr [esi + 0x28]
            mov ebx, edi
            mov ebp, dword ptr [esi + 8]
            imul ebx, ecx
            add ebx, edx
            xor ecx, ecx
            mov cl, byte ptr [ebx + ebp]
            mov ebx, dword ptr [gPalette_0074a5fc]
            shl ecx, 8
            mov ebp, dword ptr [ebx + 8]
            xor ebx, ebx
            mov bl, byte ptr [eax]
            add ecx, ebp
            inc edx
            inc eax
            mov cl, byte ptr [ecx + ebx]
            mov byte ptr [eax - 1], cl
            xor ecx, ecx
            mov cx, word ptr [esi + 0x34]
            cmp edx, ecx
            jl L0047d7ef
            mov ecx, dword ptr [esp + 0x10]
    L0047d82a:
            mov eax, dword ptr [esp + 0x18]
            xor edx, edx
            mov dx, word ptr [esi + 0x36]
            inc edi
            inc eax
            cmp edi, edx
            mov dword ptr [esp + 0x18], eax
            jl L0047d7d5
            pop ebx
            pop ebp
    L0047d840:
            pop edi
            pop esi
            pop ecx
            ret 8
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

// FUNCTION: CARMA2_HW 0x0047c650
int C2_HOOK_FASTCALL SwitchToHiresMode(void) {
    return 0;
}

// FUNCTION: CARMA2_HW 0x004b4fd0
void C2_HOOK_FASTCALL DRSetPaletteEntries(br_pixelmap* pPalette, int pFirst_colour, int pCount) {
    if (!pFirst_colour) {
        ((br_int_32*)pPalette->pixels)[0] = 0;
    }
    memcpy(gCurrent_palette_pixels + 4 * pFirst_colour, (char*)pPalette->pixels + 4 * pFirst_colour, 4 * pCount);
    gPalette_changed = 0;
    if (!gFaded_palette) {
        PDSetPaletteEntries(pPalette, pFirst_colour, pCount);
    }
    gPalette_munged = 1;
}

void C2_HOOK_FASTCALL DRSetPalette3(br_pixelmap* pThe_palette, int pSet_current_palette) {

    if (pSet_current_palette) {
        memcpy(gCurrent_palette_pixels, pThe_palette->pixels, 256 * sizeof(br_colour));
    }
    gPalette_changed = 0;
    if (!gFaded_palette) {
        PDSetPalette(pThe_palette);
    }
    gPalette_munged |= (pThe_palette != gRender_palette);
}

// FUNCTION: CARMA2_HW 0x004b5030
void C2_HOOK_FASTCALL DRSetPalette(br_pixelmap* pThe_palette) {
    ((br_int_32*)pThe_palette->pixels)[0] = 0;
    memcpy(gCurrent_palette_pixels, pThe_palette->pixels, 4 * 256);
    gPalette_changed = 0;
    if (!gFaded_palette) {
        PDSetPalette(pThe_palette);
    }
    gPalette_munged |= (pThe_palette != gRender_palette);
}


// FUNCTION: CARMA2_HW 0x0043dff0
void C2_HOOK_FASTCALL InitTransientBitmaps(void) {
    int i;

    C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(gTransient_bitmaps) != 50);
    C2_HOOK_BUG_ON(sizeof(tTransient_bm) != 0x18);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tTransient_bm, in_use, 4);

    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gTransient_bitmaps); i++) {
        gTransient_bitmaps[i].pixmap = NULL;
        gTransient_bitmaps[i].in_use = 0;
    }
}

// FUNCTION: CARMA2_HW 0x004e9b10
br_uint_32 C2_HOOK_CDECL SaveShadeTable(br_pixelmap* pTable, void* pArg) {
    br_pixelmap* copy;

    C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(gSaved_shade_tables) != 100);
    if (gSaved_table_count == CARPOCALYPSE2_ASIZE(gSaved_shade_tables)) {
        return 1;
    }
    gSaved_shade_tables[gSaved_table_count].original = pTable;
    C2_HOOK_BUG_ON(sizeof(br_pixelmap) != 0x44);
    copy = BrMemAllocate(sizeof(br_pixelmap), kMem_misc);
    gSaved_shade_tables[gSaved_table_count].copy = copy;
    gSaved_table_count += 1;
    memcpy(copy, pTable, sizeof(br_pixelmap));
    return 0;
}

// FUNCTION: CARMA2_HW 0x004e9ae0
void C2_HOOK_FASTCALL SaveShadeTables(void) {
    PossibleService();
    gSaved_table_count = 0;
    BrTableEnum("*", (br_table_enum_cbfn*)SaveShadeTable, NULL);
}

// FUNCTION: CARMA2_HW 0x004b52a0
void C2_HOOK_FASTCALL InitPaletteAnimate(void) {
    gLast_palette_change = 0;
    gPalette_index = 0;
}


void C2_HOOK_FASTCALL DeallocateTransientBitmap(int pIndex) {

    if (gTransient_bitmaps[pIndex].pixmap != NULL) {
        BrPixelmapFree(gTransient_bitmaps[pIndex].pixmap);
        gTransient_bitmaps[pIndex].pixmap = NULL;
        gTransient_bitmaps[pIndex].in_use = 0;
    }
}

// FUNCTION: CARMA2_HW 0x004b5230
void C2_HOOK_FASTCALL DRSetPaletteByName(const char* pPalette_name) {
    br_pixelmap* palette;

    palette = BrTableFind(pPalette_name);
    if (palette != NULL) {
        ((br_int_32*)palette->pixels)[0] = 0;
        memcpy(gCurrent_palette_pixels, palette->pixels, 256 * sizeof(br_colour));
        gPalette_changed = 0;
        if (!gFaded_palette) {
            PDSetPalette(palette);
        }
        gPalette_munged |= (palette != gRender_palette);
    }
}
