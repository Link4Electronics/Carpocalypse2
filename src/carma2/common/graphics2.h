#ifndef GUARD_GRAPHICS2_H
#define GUARD_GRAPHICS2_H

#include "c2_hooks.h"
#include "brender/brender.h"

extern int gNoTransients;
extern br_pixelmap* gRender_palette;
extern int gDim_amount;
extern int gPalette_changed;
extern br_pixelmap* gCurrent_palette;

void C2_HOOK_FASTCALL DRSetPaletteEntries(br_pixelmap* pPalette, int pFirst_colour, int pCount);

extern void C2_HOOK_FASTCALL DRSetPalette3(br_pixelmap* pThe_palette, int pSet_current_palette);

extern void C2_HOOK_FASTCALL DRSetPalette2(br_pixelmap* pThe_palette, int pSet_current_palette);

extern void C2_HOOK_FASTCALL DRSetPalette(br_pixelmap* pThe_palette);

extern void C2_HOOK_FASTCALL DRSetPaletteByName(const char* pPalette_name);

extern void C2_HOOK_FASTCALL InitializePalettes(void);

extern void C2_HOOK_FASTCALL InitPaletteAnimate(void);

// RevertPalette

// MungePalette

// ResetPalette

// Darken

// SetFadedPalette

// FadePaletteDown

// FadePaletteUp

// EnsurePaletteUp

// EnsureRenderPalette

extern void C2_HOOK_FASTCALL InitTransientBitmaps(void);

// SetAllShapeStatusFlags

// DeallocateAllTransientBitmaps

extern void C2_HOOK_FASTCALL RemoveTransientBitmaps(int pGraphically_remove_them);

// SaveTransient

extern int C2_HOOK_FASTCALL DoMouseCursor(void);

// AllocateCursorTransient

// InitMouseTargetLines

extern void C2_HOOK_FASTCALL StartMouseCursor(void);

extern void C2_HOOK_FASTCALL EndMouseCursor(void);

// FUNCTION: CARMA2_HW 0x0047b450
extern void C2_HOOK_FASTCALL DRDrawLine(br_pixelmap* pDestn, int pX1, int pY1, int pX2, int pY2, int pColour);

extern void C2_HOOK_FASTCALL ClearEntireScreen(void);

// DistanceFromPlane

extern void C2_HOOK_FASTCALL KillSplashScreen(void);

// FUNCTION: CARMA2_HW 0x0047b990
extern void C2_HOOK_FASTCALL SplashScreenWith(const char* pPixmap_name);
extern void C2_HOOK_FASTCALL DRPixelmapBlit(br_int_16 pX, br_int_16 pY, const br_pixelmap* pSource);


extern void C2_HOOK_FASTCALL DRPixelmapRectangleMaskedCopy(br_pixelmap* pDest, br_int_16 pDest_x, br_int_16 pDest_y, const br_pixelmap* pSource, br_int_16 pSource_x, br_int_16 pSource_y, br_int_16 pWidth, br_int_16 pHeight);


extern void C2_HOOK_FASTCALL DRMaskedStamp(br_int_16 pDest_x, br_int_16 pDest_y, const br_pixelmap* pSource);

void C2_HOOK_FASTCALL FUN_0047bc80(br_pixelmap* pDest, br_int_16 pDest_x, br_int_16 pDest_y, const br_pixelmap* pSource, br_int_16 pSource_x, br_int_16 pSource_y, br_int_16 pWidth, br_int_16 pHeight);

void C2_HOOK_FASTCALL FUN_0047d700(br_pixelmap* pDest, const br_pixelmap* pSource, unsigned int pArg3, unsigned int pDest_y, unsigned int pWidth, unsigned int pHeight);


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

extern void C2_HOOK_FASTCALL EnableLights(void);

extern void C2_HOOK_FASTCALL DRPixelmapBlend75Copy(br_pixelmap* pDest, br_int_16 pDest_x, br_int_16 pDest_y, br_pixelmap* pSource);

#endif // GUARD_GRAPHICS2_H
