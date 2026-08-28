#ifndef GUARD_16_GRAPHICS1_H
#define GUARD_16_GRAPHICS1_H

#include "c2_hooks.h"
#include "carpocalypse2_types.h"

extern int gRender_indent;
extern int gWidth;
extern int gHeight;

// MungeClipPlane

// TryThisEdge

// ProcessShadow

// RenderShadows

extern void C2_HOOK_FASTCALL SetShadowLevel(tShadow_level pLevel);

// GetShadowLevel

// ToggleShadow

extern void C2_HOOK_FASTCALL InitShadow(void);

// SaveShadeTable

// SaveShadeTables

extern void C2_HOOK_FASTCALL DisposeSavedShadeTables(void);

// ShadowMode

// LollipopizeActor

// ResetLollipopQueue

// RenderLollipops

// SetupDepthBuffer

extern void C2_HOOK_FASTCALL SetBRenderScreenAndBuffers(int pX_offset, int pY_offset, int pWidth, int pHeight);

// AdjustRenderScreenSize

// ScreenSmaller

// ScreenLarger

// ClearWobbles

extern void C2_HOOK_FASTCALL InitWobbleStuff(void);

// NewScreenWobble

// SetScreenWobble

// ResetScreenWobble

// CalculateWobblitude

// ProcessNonTrackActors

// ConditionallyFillWithSky

// FoxyStuff

// DoARenderPass

// DoACompleteRenderPass

// RenderAFrame

extern int gScreen_wobble_x;
extern int gScreen_wobble_y;
extern int gHud_actor_storage_size;
extern br_actor* gHud_actor_storage[128];
extern float gCosine_array[64];

extern void C2_HOOK_FASTCALL StartRenderingHeadups(void);

extern void C2_HOOK_FASTCALL RenderThisHeadup(br_actor* pActor);

extern void C2_HOOK_FASTCALL StopRenderingHeadups(void);

extern void C2_HOOK_FASTCALL CleanPolyFontDanglers(void);

extern void C2_HOOK_FASTCALL DoTestHeadup(void);

#endif // GUARD_16_GRAPHICS1_H
