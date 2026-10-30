#ifndef GUARD_TINTEDPOLY_H
#define GUARD_TINTEDPOLY_H

#include "c2_hooks.h"

extern void C2_HOOK_FASTCALL InitTintedPolyStuff(void);

void C2_HOOK_FASTCALL ApplyTintedPolyPalette(int pTintedIndex);

// BuildTintedPolyModel

// BuildTintedPolyMaterial

// GetRangeOfValuesFromPixelmap

extern int C2_HOOK_FASTCALL CreateTintedPoly(int x0, int y0, int width, int height, int class, int arg1, int arg2, int arg3);

// UpdateTintedPolyByMaterial

// UpdatePolyVertexColours

// TurnTintedPolyOn

// TurnTintedPolyOff

// RenderTintedPolys

// SetTintedPolyColour

// SetTintedPolyRefMaterial

// InWater

// ClearTheFlame

// GetUniqueFileName

// UpdateFirePoly

// UpdatePulsePoly

// ProcessTintedPoly

// UpdateTintedPolys

// SetTintedPolySize

// TintedPolyIsOn

// UpdatePulsePoly
void C2_HOOK_FASTCALL UpdatePulsePoly(int pIndex);
extern void C2_HOOK_FASTCALL UpdatePolyVertexColours(int pTintedIndex);

// UpdateFirePoly
void C2_HOOK_FASTCALL UpdateFirePoly(int pIndex);

void C2_HOOK_FASTCALL UpdateTintedPolyByMaterial(int pTintedIndex);


#endif // GUARD_TINTEDPOLY_H
