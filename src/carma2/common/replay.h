#ifndef GUARD_REPLAY_H
#define GUARD_REPLAY_H

#include "c2_hooks.h"
#include "carpocalypse2_types.h"

typedef struct tReplayControlButtonBounds {
    int left;
    int top;
    int right;
    int bottom;
} tReplayControlButtonBounds;

extern int gAR_camera_type;
extern int gCar_car_collisions;
extern tU32 gTime_of_last_zappy_headups;
extern br_pixelmap* gReplay_pixelmaps[11];
extern int gReplay_progress_bar_left[2];
extern int gReplay_progress_bar_right[2];
extern int gReplay_progress_bar_y[2];
extern int gReplay_pause_state;
extern tU32 gReplay_progress_start_time;
extern tU32 gReplay_progress_end_time;
extern int gReplay_show_camera_icon;
extern tU32 gReplay_camera_mode_change_time;
extern const float gReplay_slow_rate_limit;
extern const float gReplay_fast_rate_limit;

// ActualActionReplayHeadups

// DoActionReplayPostSwap

// DoZappyActionReplayHeadups

// DoActionReplayHeadups

// PreProcess

// PostProcess

// DoActionReplay

// MoveToEndOfReplay

// MoveToStartOfReplay

// PollActionReplayControls

// CheckReplayTurnOn

// InitializeActionReplayPix

// FreeActionReplayPix

// SynchronizeActionReplay

// TurnOnActionReplay

// TurnOffActionReplay

// AfterActionReplay

// ToggleReplay

// DisplayMovieStatusPanel

// ApplyTime

// ARSetPause

#define CARPOCALYPSE2_REPLAY_H

#include "c2_hooks.h"

#include "carpocalypse2_types.h"

extern tActionReplayCameraMode gAction_replay_camera_mode;
extern void C2_HOOK_FASTCALL ApplyTime(tU32 pTime);



extern int gAction_replay_manual_camera_target_type;
extern br_vector3 gAction_replay_manual_camera_target_position;
extern tCar_spec* gAction_replay_manual_camera_target_car;
extern tPedestrian* gAction_replay_manual_camera_target_pedestrian;
extern br_actor* gAction_replay_manual_camera_target_character_actor;

extern tU8 gCamera_type_allowed_replay[9];
extern tU8 gCamera_type_allowed_gameplay[9];

extern tU32 gLast_synch_time;

extern int gSingle_frame_mode;

extern int gKey_down;
extern tU32 gUnk_006a2360;
extern tU32 gUnk_006a2364;
extern tU32 gUnk_006a23c8;


void C2_HOOK_FASTCALL SetQuickTimeDefaults(void);

void C2_HOOK_FASTCALL MungeCarMaterials(tCar_spec* pCar, int pInternal_cam);

void C2_HOOK_FASTCALL SetCameraType(tActionReplayCameraMode pCamPos);

void C2_HOOK_FASTCALL PositionExternalCamera(tCar_spec* pCar_spec, tU32 pTime);

void C2_HOOK_FASTCALL ARMainLoopStart(void);

void C2_HOOK_FASTCALL ARService(void);

int C2_HOOK_FASTCALL FUN_00403d00(void);

void C2_HOOK_FASTCALL thunk_FUN_00564d4a(int pUnused, int pTag);

int C2_HOOK_FASTCALL FUN_00403d20(void);

void C2_HOOK_FASTCALL TurnOnActionReplay(void);

void C2_HOOK_FASTCALL TurnOffActionReplay(void);

void C2_HOOK_FASTCALL AfterActionReplay(void);

void C2_HOOK_FASTCALL DoZappyActionReplayHeadups(int pFrame_number);

void C2_HOOK_FASTCALL PreProcess(int pFrame_period);

void C2_HOOK_FASTCALL PostProcess(int pFrame_period);

void C2_HOOK_FASTCALL ToggleReplay(int* pArg1, int* pArg2);

void C2_HOOK_FASTCALL InitialiseActionReplay(void);

void C2_HOOK_FASTCALL CheckReplayTurnOn(void);

void C2_HOOK_FASTCALL DoActionReplay(tU32 pFrame_period);

void C2_HOOK_FASTCALL PollActionReplayControls(tU32 *pFrame_period, tU32* pAverage_frame_period);

int C2_HOOK_FASTCALL ARReplayIsReallyPaused(void);

float C2_HOOK_FASTCALL ARGetReplayRate(void);

void C2_HOOK_FASTCALL PipeSingleGrooveStop(int pGroove_index, br_matrix34* pMatrix, int pPath_interrupt, int pObject_interrupt, float pPath_resumption, float pObject_resumption);

void C2_HOOK_FASTCALL ActualActionReplayHeadups(int pSpecial_zappy_bastard);

void C2_HOOK_FASTCALL DoActionReplayHeadups(void);

void C2_HOOK_FASTCALL ARSetReplayRate(float rate);

void C2_HOOK_FASTCALL SynchronizeActionReplay(void);

void C2_HOOK_FASTCALL DoActionReplayPostSwap(void);

int C2_HOOK_FASTCALL ARGetReplayPlayFlag(void);

void C2_HOOK_FASTCALL ARSetLastReplayFrameTime(tU32 pTime);

void C2_HOOK_FASTCALL ARSetReplayPlayFlag(int pFlag);

void C2_HOOK_FASTCALL InitializeActionReplayPix(void);

void C2_HOOK_FASTCALL DisposeReplayPixelmaps(void);

extern int gReplay_show_controls;
extern int gReplay_timeline_x1[2];
extern int gReplay_timeline_x2[2];
extern int gReplay_timeline_y[2];
extern tReplay_headup_graphics gReplay_headup_graphics;

extern tU32 gUnk_006a2360;
extern tU32 gUnk_006a2364;
extern tU32 gUnk_006a23c8;

extern void C2_HOOK_FASTCALL ARSetPause(int pPause);

#endif // GUARD_REPLAY_H
