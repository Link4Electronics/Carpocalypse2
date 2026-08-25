#include "18-graphics2.h"

#include <string.h>
#include <stdio.h>

#include "42-input.h"
#include "25-grafdata.h"
#include "66-tintedpoly.h"
#include "41-utility.h"
#include "52-errors.h"
#include "globvars.h"
#include "platform.h"
#include "carpocalypse2_types.h"
#include "carpocalypse2_macros.h"

extern br_pixelmap* gBack_screen;
extern void C2_HOOK_FASTCALL FadePaletteUp(void);
extern void C2_HOOK_FASTCALL DeallocateTransientBitmap(int pIndex);
extern void C2_HOOK_FASTCALL SetTintedPolySize(int pIndex, int pX, int pY, int pW, int pH);
extern void C2_HOOK_FASTCALL TurnTintedPolyOn(int pIndex);
extern void C2_HOOK_FASTCALL TurnTintedPolyOff(int pIndex);
extern void C2_HOOK_FASTCALL ProcessTintedPoly(int pIndex);
extern void C2_HOOK_FASTCALL RenderTintedPolys(void);
extern tU32 C2_HOOK_FASTCALL PDGetTotalTime(void);
extern int gMouse_in_use;
extern tTintedPoly gTintedPolys[10];
extern tGraf_spec gGraf_specs[2];
extern int gNoTransients;

int gMouse_started;
tMouse_coord gMouse_last_pos;
int gNext_transient;
int gTransient_bitmap_index;
int gCurrent_cursor_index;
int gCursor_line_width = 5;
// GLOBAL: CARMA2_HW 0x0074ca28
int gNoTransients;
// GLOBAL: CARMA2_HW 0x0074cf04
// GLOBAL: CARMA2_HW 0x0074a680
char* gCurrent_palette_pixels;
// GLOBAL: CARMA2_HW 0x006923c0
// GLOBAL: CARMA2_HW 0x006a27a8
int gSaved_table_count;
int gPalette_munged;
tU32 gLast_palette_change;
int gPalette_index;
tSaved_table gSaved_shade_tables[100];
int gPalette_changed;
// GLOBAL: CARMA2_HW 0x0074a674
br_pixelmap* gRender_palette;
br_pixelmap* gOrig_render_palette;
br_pixelmap* gFlic_palette;
// GLOBAL: CARMA2_HW 0x0074a678
br_pixelmap* gCurrent_palette;
// GLOBAL: CARMA2_HW 0x006923b8
br_colour* gScratch_pixels;
// GLOBAL: CARMA2_HW 0x006923a8
br_pixelmap* gScratch_palette;
// GLOBAL: CARMA2_HW 0x006923c8
int gFaded_palette;
// GLOBAL: CARMA2_HW 0x0079ec14
int gDim_amount;
// GLOBAL: CARMA2_HW 0x006923b4
int gPalette_index;
tTransient_bm gTransient_bitmaps[50];
int gCursor_tinted_top = -1;
int gCursor_tinted_left = -1;
int gCursor_tinted_bottom = -1;
int gCursor_tinted_right = -1;
int gCursor_tinted_center = -1;

// FUNCTION: CARMA2_HW 0x0043dff0
void C2_HOOK_FASTCALL DeallocateAllTransientBitmaps(void) {
    int i;

    for (i = 0; i < CARPOCALYPSE2_ASIZE(gTransient_bitmaps); i++) {
        DeallocateTransientBitmap(i);
    }
}

// FUNCTION: CARMA2_HW 0x0043e010
void C2_HOOK_FASTCALL RemoveTransientBitmaps(int pGraphically_remove_them) {
    int i;
    int order_number;

    if (gNoTransients && pGraphically_remove_them) {
        for (order_number = gNext_transient - 1; order_number >= 0; order_number--) {
            for (i = 0; i < CARPOCALYPSE2_ASIZE(gTransient_bitmaps); i++) {
                if (gTransient_bitmaps[i].pixmap != NULL && gTransient_bitmaps[i].order_number == order_number) {
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

#include "41-utility.h"
#include "52-errors.h"
#include "globvars.h"
#include "platform.h"
#include "carpocalypse2_types.h"




// FUNCTION: CARMA2_HW 0x004b55f0
void C2_HOOK_FASTCALL Darken(tU8* pPtr, unsigned int pDarken_amount) {

    *pPtr = (pDarken_amount * *pPtr) / 256;
}

void C2_HOOK_FASTCALL SetFadedPalette(int pDegree) {
    int j;

    memcpy(gScratch_pixels, gCurrent_palette->pixels, 4 * 256);
    for (j = 0; j < 256; j++) {
        Darken((tU8*)&gScratch_pixels[4 * j + 0], pDegree);
        Darken((tU8*)&gScratch_pixels[4 * j + 1], pDegree);
        Darken((tU8*)&gScratch_pixels[4 * j + 2], pDegree);
        Darken((tU8*)&gScratch_pixels[4 * j + 3], pDegree);
    }
    DRSetPalette2(gScratch_palette, 0);
}
// FUNCTION: CARMA2_HW 0x004b5470
void C2_HOOK_FASTCALL FadePaletteUp(void) {
    int i;
    int start_time;
    int the_time;

    if (gFaded_palette) {
        gFaded_palette = 0;
        start_time = PDGetTotalTime();
        while (1) {
            the_time = PDGetTotalTime() - start_time;
            if (the_time >= 500) {
                break;
            }
            i = (the_time * 256) / 500;
            SetFadedPalette(i);
        }
        DRSetPalette(gCurrent_palette);
    }
}

void C2_HOOK_FASTCALL EnsurePaletteUp(void) {

    if (gFaded_palette) {
        FadePaletteUp();
    }
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

    mse_cross = BrMapFind("mse_corn");
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
        BrMatrix23Rotate(&(gTintedPolys[gCursor_tinted_top].material)->map_transform, BrDegreeToAngle(90));
        BrMaterialUpdate(gTintedPolys[gCursor_tinted_top].material, BR_MATU_ALL);
        (gTintedPolys[gCursor_tinted_bottom].material)->colour_map = mse_line;
        BrMatrix23Rotate(&(gTintedPolys[gCursor_tinted_bottom].material)->map_transform, BrDegreeToAngle(90));
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

// DRDrawLine

// STUB: CARMA2_HW 0x0047b880
void C2_HOOK_FASTCALL ClearEntireScreen(void) {
#ifndef CARPOCALYPSE2_MATCHING
    /* stub: no-op for Linux boot */
#else
    NOT_IMPLEMENTED();
#endif
}

// DistanceFromPlane

// STUB: CARMA2_HW 0x0047b960
void C2_HOOK_FASTCALL KillSplashScreen(void) {
#ifndef CARPOCALYPSE2_MATCHING
    /* stub: no-op for Linux boot */
#else
    NOT_IMPLEMENTED();
#endif
}

// SplashScreenWith

// FUNCTION: CARMA2_HW 0x0047ba80
void C2_HOOK_FASTCALL DRPixelmapRectangleMaskedCopy(br_pixelmap* pDest, br_int_16 pDest_x, br_int_16 pDest_y, const br_pixelmap* pSource, br_int_16 pSource_x, br_int_16 pSource_y, br_int_16 pWidth, br_int_16 pHeight) {
    int y;
    int x;
    int dest_row_wrap;
    int source_row_wrap;
    tU16 the_byte;
    tU16* source_ptr;
    tU16* dest_ptr;
    int dest_x;
    int dest_y;

    source_ptr = (tU16*)pSource->pixels + pSource->row_bytes * pSource_y / sizeof(tU16) + pSource_x;
    dest_x = pDest_x;
    dest_y = pDest_y;
    dest_ptr = (tU16*)pDest->pixels + dest_y * pDest->row_bytes / sizeof(tU16) + pDest->base_x / sizeof(tU16) + dest_x;
    source_row_wrap = pSource->row_bytes / sizeof(tU16) - pWidth;
    dest_row_wrap = pDest->row_bytes / sizeof(tU16) - pWidth;

    if (dest_y < 0) {
        pHeight += dest_y;
        if (pHeight <= 0) {
            return;
        }
        source_ptr -= dest_y * pSource->row_bytes / sizeof(tU16);
        dest_ptr -= dest_y * pDest->row_bytes / sizeof(tU16);
        dest_y = 0;
    }
    if (dest_y < pDest->height) {
        if (pDest->height < dest_y + pHeight) {
            pHeight = pDest->height - dest_y;
        }
        if (dest_x < 0) {
            pWidth += dest_x;
            if (pWidth <= 0) {
                return;
            }
            dest_x = 0;
            source_ptr -= dest_x;
            dest_ptr -= dest_x;
            source_row_wrap -= dest_x;
            dest_row_wrap -= dest_x;
        }
        if (dest_x < pDest->width) {
            if (dest_x + pWidth > pDest->width) {
                pWidth -= pWidth + dest_x - pDest->width;
                source_row_wrap += pWidth + dest_x - pDest->width;
                dest_row_wrap += pWidth + dest_x - pDest->width;
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
    if (pThe_palette != gRender_palette) {
        gPalette_munged |= 0x1;
    }
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
    if (pThe_palette != gRender_palette) {
        gPalette_munged |= 0x1;
    }
}

// FUNCTION: CARMA2_HW 0x004b5030
void C2_HOOK_FASTCALL DRSetPalette(br_pixelmap* pThe_palette) {

    DRSetPalette2(pThe_palette, 1);
}


// FUNCTION: CARMA2_HW 0x0043dff0
void C2_HOOK_FASTCALL InitTransientBitmaps(void) {
    int i;

    C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(gTransient_bitmaps) != 50);
    C2_HOOK_BUG_ON(sizeof(tTransient_bm) != 0x18);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tTransient_bm, in_use, 4);

    for (i = 0; i < CARPOCALYPSE2_ASIZE(gTransient_bitmaps); i++) {
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


// FUNCTION: CARMA2_HW 0x0043dfd0
void C2_HOOK_FASTCALL DeallocateTransientBitmap(int pIndex) {

    if (gTransient_bitmaps[pIndex].pixmap != NULL) {
        BrPixelmapFree(gTransient_bitmaps[pIndex].pixmap);
        gTransient_bitmaps[pIndex].pixmap = NULL;
        gTransient_bitmaps[pIndex].in_use = 0;
    }
}
