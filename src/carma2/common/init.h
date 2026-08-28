#ifndef GUARD_INIT_H
#define GUARD_INIT_H

#include "c2_hooks.h"
#include "carpocalypse2_types.h"

#include "brender/brender.h"

extern int gBr_initialized;
extern float gCamera_angle;
extern int gInitial_rank;
extern int gStarting_money[3];
extern int gNet_starting_money[8];
extern int gCredits_per_rank[3];
extern tSlot_info gInitial_APO[3];
extern tSlot_info gInitial_APO_potential[3];
extern tSlot_info gMax_APO[3];

// Modify2DCopyPixelmaps

// SetupFFBValues

extern void C2_HOOK_FASTCALL AllocateActors(void);

extern void C2_HOOK_FASTCALL AllocateCamera(void);

// ReinitialiseForwardCamera

// AllocateRearviewPixelmap

// ReinitialiseRearviewCamera

// ReinitialiseRenderStuff

extern void C2_HOOK_FASTCALL InstallFindFailedHooks(void);

extern void C2_HOOK_FASTCALL AllocateStandardLamp(void);

// extern void C2_HOOK_FASTCALL InitializeBRenderEnvironment(void);

// InitBRFonts

extern void C2_HOOK_FASTCALL Init2DStuffForPolyFonts(void);

extern void C2_HOOK_FASTCALL InitLineStuff(void);

extern void C2_HOOK_FASTCALL Init2DStuff(void);

extern void C2_HOOK_FASTCALL CheckTimedDemo(void);

// extern void C2_HOOK_FASTCALL InitialiseApplication(int pArgc, const char **pArgv);

extern void C2_HOOK_FASTCALL DoDevelopmentThings(void);

extern void C2_HOOK_FASTCALL InitialiseDeathRace(int pArgc, const char** pArgv);


extern void C2_HOOK_FASTCALL InitGameAccordingToSkillLevel(void);

extern void C2_HOOK_FASTCALL InitGame(int pStart_race);

// LoadInTrack

// InitRace

extern int C2_HOOK_FASTCALL GetScreenSize(void);

extern void C2_HOOK_FASTCALL SetScreenSize(int pNew_size);

#define CARPOCALYPSE2_INIT_H

#include "brender/brender.h"

#include "c2_hooks.h"


extern br_material* gDefault_track_material;

extern int gRender_indent;

extern int gBrZb_initialized;

extern undefined4 gDAT_0074c6d0;

extern int gInitial_powerup_slots[3];

void C2_HOOK_FASTCALL InitialiseApplication(int pArgc, const char **pArgv);

void C2_HOOK_FASTCALL InitLightingEffects(void);

void C2_HOOK_FASTCALL KillLightingEffects(void);

void C2_HOOK_FASTCALL InitializeBRenderEnvironment(void);

void C2_HOOK_FASTCALL InitSmokeStuff(void);

int C2_HOOK_FASTCALL HighResHeadupWidth(int pWidth);

int C2_HOOK_FASTCALL HighResHeadupHeight(int pHeight);

void C2_HOOK_FASTCALL DisposeGameIfNecessary(void);

void C2_HOOK_FASTCALL ReinitialiseRenderStuff(void);

void C2_HOOK_FASTCALL ReinitialiseForwardCamera(void);

void C2_HOOK_FASTCALL AllocateRearviewPixelmap(void);

void C2_HOOK_FASTCALL ReinitialiseRearviewCamera(void);

void C2_HOOK_FASTCALL Modify2DCopyPixelmaps(void);

void C2_HOOK_FASTCALL InitRace(void);

void C2_HOOK_FASTCALL DisposeRace(void);

void C2_HOOK_FASTCALL LoadInTrack(void);

void C2_HOOK_FASTCALL DisposeTrack(void);

#endif // GUARD_INIT_H
