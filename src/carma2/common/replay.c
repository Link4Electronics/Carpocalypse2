#include "replay.h"

#include "car.h"
#include "controls.h"
#include "displays.h"
#include "drone.h"
#include "finteray.h"
#include "globvars.h"
#include "globvrkm.h"
#include "globvrpb.h"
#include "graphics.h"
#include "input.h"
#include "loading.h"
#include "main.h"
#include "mainloop.h"
#include "netgame.h"
#include "oil.h"
#include "pedestrn.h"
#include "piping.h"
#include "platform.h"
#include "powerups.h"
#include "replay_callbacks.h"
#include "utility.h"

#include "c2_string.h"

#include "carpocalypse2_macros.h"
#include "replay.h"
// GLOBAL: CARMA2_HW 0x00679278
int gAction_replay_manual_camera_target_type;

// GLOBAL: CARMA2_HW 0x0067927c
br_vector3 gAction_replay_manual_camera_target_position;

// GLOBAL: CARMA2_HW 0x00679288
tCar_spec* gAction_replay_manual_camera_target_car;

// GLOBAL: CARMA2_HW 0x0067928c
tPedestrian* gAction_replay_manual_camera_target_pedestrian;

// GLOBAL: CARMA2_HW 0x00679290
br_actor* gAction_replay_manual_camera_target_character_actor;

// GLOBAL: CARMA2_HW 0x0058f600
tU8 gCamera_type_allowed_replay[9] = {
    1, 1, 1, 1, 1, 0, 0, 1, 1,
};

// GLOBAL: CARMA2_HW 0x0058f610
tU8 gCamera_type_allowed_gameplay[9] = {
    1, 0, 0, 0, 1, 0, 0, 1, 1,
};

// GLOBAL: CARMA2_HW 0x006a23c4
int gSingle_frame_mode;

extern int gDisable_sound;

void C2_HOOK_FASTCALL FUN_00403cc0(int pUnused, tARDoActionReplay_step_callback* pStep_callback, tARDoActionReplay_frame_callback* pFrame_start_callback, tARDoActionReplay_frame_callback* pFrame_end_callback);

// GLOBAL: CARMA2_HW 0x00589330
const br_scalar gFLOAT_00589330 = 0.0f;

// GLOBAL: CARMA2_HW 0x006a23d4
tU32 gLast_synch_time;

// GLOBAL: CARMA2_HW 0x006a2374
int gKey_down;

// GLOBAL: CARMA2_HW 0x006a2360
tU32 gUnk_006a2360;

// GLOBAL: CARMA2_HW 0x006a2364
tU32 gUnk_006a2364;

// GLOBAL: CARMA2_HW 0x006a2368
int gReplay_show_controls;

// GLOBAL: CARMA2_HW 0x0065fbe0
int gReplay_timeline_x1[2] = {240, 443};

// GLOBAL: CARMA2_HW 0x0065fbe8
int gReplay_timeline_x2[2] = {306, 612};

// GLOBAL: CARMA2_HW 0x0065fbf0
int gReplay_timeline_y[2] = {184, 446};

// GLOBAL: CARMA2_HW 0x006a2388
tReplay_headup_graphics gReplay_headup_graphics;

// GLOBAL: CARMA2_HW 0x006a2378
int gUnk_006a2378;

// GLOBAL: CARMA2_HW 0x006a237c
tU32 gUnk_006a237c;

// GLOBAL: CARMA2_HW 0x006a23c8
tU32 gUnk_006a23c8;

// GLOBAL: CARMA2_HW 0x006a2380
tU32 gTime_of_last_zappy_headups;

// GLOBAL: CARMA2_HW 0x006a2388
br_pixelmap* gReplay_pixelmaps[11];

// GLOBAL: CARMA2_HW 0x0065fbb0
const char* gReplay_pixie_names[11] = {
    "REPLAY.PIX",
    "RBUTTONS.PIX",
    "REWSTART.PIX",
    "REW.PIX",
    "REVPLAY.PIX",
    "PAUSE.PIX",
    "PLAY.PIX",
    "FFWD.PIX",
    "FWDEND.PIX",
    "CAMERA.PIX",
    "RECORD.PIX",
};

// GLOBAL: CARMA2_HW 0x0058f620
int gAR_camera_type = 2;

// GLOBAL: CARMA2_HW 0x0058f6a4
int gCar_to_car_collisions = 1;

// SetCameraType

int C2_HOOK_FASTCALL IsCameraTypeAllowed(tActionReplayCameraMode pCamera_type) {

    if (gAction_replay_mode && !gCamera_type_allowed_replay[pCamera_type]) {
        return 0;
    }
    if (!gAction_replay_mode && !gCamera_type_allowed_gameplay[pCamera_type]) {
        return 0;
    }
    if (pCamera_type > 8) {
        return 0;
    }
    if (pCamera_type == kActionReplayCameraMode_Drone && !OKToViewDrones()) {
        return 0;
    }
    if (pCamera_type == kActionReplayCameraMode_Peds && gPed_count == 0) {
        return 0;
    }
    return 1;
}

#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_CDECL ARDoSingleVariedSession();
extern void C2_HOOK_FASTCALL BRPM_convert(br_pixelmap* pMap, int pPixel_type);
extern br_pixelmap* C2_HOOK_CDECL BrMapAdd();
extern br_pixelmap* C2_HOOK_FASTCALL LoadPixelmap(const char* pPath_name);
extern br_pixelmap* gBack_screen;
extern volatile br_scalar gUNK_0069410c;
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern int C2_HOOK_FASTCALL ARGetReplayDirection(void);
extern float C2_HOOK_FASTCALL ARGetReplayRate(void);
extern int C2_HOOK_FASTCALL ARGoBackwards(void);
extern int C2_HOOK_FASTCALL ARGoForwards(void);
extern int C2_HOOK_FASTCALL ARReplayPauseState(void);
extern void C2_HOOK_FASTCALL ARSetPause(int pPause);
extern void C2_HOOK_FASTCALL ARSetReplayDirection(float pDirection);
extern void C2_HOOK_FASTCALL ARSetReplayRate(float rate);
extern int C2_HOOK_FASTCALL ARToggleReplay(tTurn_on_AR_callback* pTurn_on_AR_cb, tTurn_off_AR_callback* pTurn_off_AR_cb, tAfter_AR_callback* pAfter_AR_cb, tZappy_AR_callback* pZappy_AR_cb, tPreprocess_AR_callback* pPreprocess_cb, tPostProcess_AR_callback* pPostProcess_cb, int* pArg7, int* pArg8);
extern void C2_HOOK_FASTCALL AfterActionReplay(void);
extern br_pixelmap* C2_HOOK_CDECL BrMapRemove();
extern void C2_HOOK_CDECL BrPixelmapFree();
extern void C2_HOOK_FASTCALL ChangeCameraType(void);
extern int C2_HOOK_FASTCALL DRS3StopAllOutletSoundsExceptCDA(void);
extern void C2_HOOK_FASTCALL EdgeTriggerModeOff(void);
extern int C2_HOOK_FASTCALL EitherMouseButtonDown(void);
extern void C2_HOOK_FASTCALL FUN_00403210(tARDoActionReplay_step_callback* pStep_callback, tARDoActionReplay_frame_callback* pFrame_start_callback, tARDoActionReplay_frame_callback* pFrame_end_callback);
extern void C2_HOOK_FASTCALL FUN_00403c70(tARDoActionReplay_step_callback* pStep_callback, tARDoActionReplay_frame_callback* pFrame_start_callback, tARDoActionReplay_frame_callback* pFrame_end_callback);
extern const char* C2_HOOK_FASTCALL GetMiscString(int pIndex);
extern void C2_HOOK_FASTCALL GetMousePosition(int *pX, int *pY);
extern int C2_HOOK_FASTCALL GetNextReplayRecordState(void);
extern void C2_HOOK_FASTCALL GrooveThoseDelics(void);
extern int C2_HOOK_FASTCALL KeyIsDown(int pKey_index);
extern void C2_HOOK_FASTCALL MovieRecordButtonPressed(void);
extern void C2_HOOK_FASTCALL MungeCarGraphics(tU32 pFrame_period);
extern int C2_HOOK_FASTCALL NewTextHeadupSlot(int pSlot_index, int pFlash_rate, int pLifetime, int pFont_index, const char* pText);
extern int C2_HOOK_FASTCALL PDAnyKeyDown(void);
extern tU32 C2_HOOK_FASTCALL PDGetTotalTime(void);
extern void C2_HOOK_FASTCALL PickActionReplayManualCameraTarget(int pCursor_x, int pCursor_y, float pArg3);
extern void C2_HOOK_FASTCALL RenderAFrame(int pDepth_mask_on);
extern void C2_HOOK_FASTCALL ServiceGameInRace(void);
extern void C2_HOOK_FASTCALL TurnOffActionReplay(void);
extern void C2_HOOK_FASTCALL TurnOnActionReplay(void);
extern void __cdecl _ftol();
extern char gConst_kangeroo_force_factor[];
extern tU32 gFrame_period;
extern int gMouse_in_use;
extern int gNoTransients;
extern int gPed_676914;
extern int gReal_graf_data_index;
extern int gRecording_mouse_disabled;
#endif
// FUNCTION: CARMA2_HW 0x0040ea30
void C2_HOOK_FASTCALL PositionExternalCamera(tCar_spec* pCar_spec, tU32 pTime) {
    br_camera* camera;
    // GLOBAL: CARMA2_HW 0x0058f62c
    static tActionReplayCameraMode old_camera_mode = kActionReplayCameraMode_Invalid;

    camera = gCamera->type_data;
    CheckCameraHither();
    if (!IsCameraTypeAllowed(gAction_replay_camera_mode)) {
        ChangeCameraType();
    }
    AmIGettingBoredWatchingCameraSpin();
    if (!gProgram_state.cockpit_on) {
        tCar_spec* c;

        if (gOpponent_viewing_mode && gAction_replay_mode) {
            c = &gProgram_state.current_car;
        } else {
            c = gCar_to_view;
        }
        if (c->car_master_actor->t.t.translate.t.v[0] <= 500.f) {
            if (old_camera_mode != gAction_replay_camera_mode) {
                camera->field_of_view = BR_ANGLE_DEG(gCamera_angle);
            }
            camera->hither_z = gCamera_hither;
            switch (gAction_replay_camera_mode) {
            case kActionReplayCameraMode_Standard:
            case kActionReplayCameraMode_Rigid:
            case kActionReplayCameraMode_Reversing:
                NormalPositionExternalCamera(c, pTime);
                break;
            case kActionReplayCameraMode_Panning:
                CheckDisablePlingMaterials(c);
                SetPanningFieldOfView();
                if (old_camera_mode != kActionReplayCameraMode_Panning) {
                    SetUpPanningCamera(c);
                }
                PanningExternalCamera(c, pTime);
                EnablePlingMaterials();
                break;
            case kActionReplayCameraMode_ActionTracking:
                CheckDisablePlingMaterials(c);
                SetPanningFieldOfView();
                if (!IncidentCam(c, pTime)) {
                    if (old_camera_mode != gAction_replay_camera_mode) {
                        SetUpPanningCamera(c);
                    }
                    PanningExternalCamera(c, pTime);
                    EnablePlingMaterials();
                }
                break;
            case kActionReplayCameraMode_Manual:
                FrozenCamera(c, pTime);
                break;
            case kActionReplayCameraMode_Peds:
                PositionPedCam(GetTestPed(), pTime);
                break;
            case kActionReplayCameraMode_Drone:
                PositionDroneCam(pTime);
                break;
            case kActionReplayCameraMode_Internal:
                camera->hither_z = gCamera_cockpit_hither;
                PositionCarMountedCamera(c, pTime);
                break;
#ifdef CARPOCALYPSE2_FIX_BUGS
            default:
                abort();
#endif
            }
            old_camera_mode = gAction_replay_camera_mode;
        }
    }
}

// FUNCTION: CARMA2_HW 0x00403d40
void C2_HOOK_FASTCALL ARMainLoopStart(void) {
    gAction_replay_mode = 0;
    gLast_replay_frame_time = PDGetTotalTime();
}

// FUNCTION: CARMA2_HW 0x00403d30
void C2_HOOK_FASTCALL ARService(void) {
}

// FUNCTION: CARMA2_HW 0x004e71c0
void C2_HOOK_FASTCALL TurnOnActionReplay(void) {
#pragma inline_depth(0)

    SavePedStuff();
    if (gMap_view == 2) {
        ToggleMap();
    }
    if (gNet_mode == eNet_mode_host) {
        SendGameplayToAllPlayers(eNet_gameplay_host_paused, 0, 0, 0, 0);
    }
    gUnk_006a237c = PDGetTotalTime();
    gUnk_006a2360 = ARGetStartTime();
    LoadInterfaceStuff(1);
    StartMouseCursor();
    gKey_down = 0x34;
    gUnk_006a2378 = 0;
    gUnk_006a23c8 = PDGetTotalTime();
    gUnk_006a2364 = GetTotalTime();
    if (gRace_finished == 0) {
        SaveCameraPosition(0);
    }
    gCamera_type = gAction_replay_camera_mode;
    SetCameraType(gAR_camera_type);
    InitReplaySplashes();
#pragma inline_depth()
}

// FUNCTION: CARMA2_HW 0x004e7270
void C2_HOOK_FASTCALL TurnOffActionReplay(void) {
    UnlockInterfaceStuff();
    AssertThisTimeAsCurrentTime();
    if (gRace_finished == 0) {
        RestoreCameraPosition(0);
    }
    if (gNet_mode == eNet_mode_host) {
        SendGameplayToAllPlayers(eNet_gameplay_host_unpaused, 0, 0, 0, 0);
    }
    gAR_camera_type = gAction_replay_camera_mode;
    SetCameraType(gCamera_type);
    ComparePedStuff();
    EndMouseCursor();
}

// FUNCTION: CARMA2_HW 0x004e72d0
void C2_HOOK_FASTCALL AfterActionReplay(void) {
    ForceRebuildActiveCarList();
}

// FUNCTION: CARMA2_HW 0x004e6900
void C2_HOOK_FASTCALL DoZappyActionReplayHeadups(int pFrame_number) {
    tU32 time = PDGetTotalTime();

    if (abs(pFrame_number) > 0x2710 &&
        time - gTime_of_last_zappy_headups > 0x32) {
        ActualActionReplayHeadups(pFrame_number);
        gTime_of_last_zappy_headups = time;
        PDScreenBufferSwap(0);
        RemoveTransientBitmaps(1);
    }
}

// FUNCTION: CARMA2_HW 0x004e6950
void C2_HOOK_FASTCALL PreProcess(int pFrame_period) {
    UpdateFramePeriod(&gCamera_period);
    if (ARGetReplayRate() < 0.0f) {
        ((void (C2_HOOK_FASTCALL *)(int))MungePedestrians)(pFrame_period);
    }
}

// FUNCTION: CARMA2_HW 0x004e6980
void C2_HOOK_FASTCALL PostProcess(int pFrame_period) {
    ProcessOilSpills((tU32)pFrame_period);
    if (ARGetReplayRate() >= 0.0f) {
        ((void (C2_HOOK_FASTCALL *)(int))MungePedestrians)(pFrame_period);
    }
    MungePowerupStuff(pFrame_period);
}

// FUNCTION: CARMA2_HW 0x004e72e0
void C2_HOOK_FASTCALL ToggleReplay(int* pArg1, int* pArg2) {
    if (gPed_676914) {
        RenderAFrame(1);
        RenderAFrame(1);
    }
    if (!ARToggleReplay(TurnOnActionReplay, TurnOffActionReplay, AfterActionReplay,
            DoZappyActionReplayHeadups, PreProcess, PostProcess, pArg1, pArg2)) {
        NewTextHeadupSlot(4, 0, 1000, -4, GetMiscString(eMiscString_action_replay_unavailable));
    }
}

// FUNCTION: CARMA2_HW 0x004c6bf0
void C2_HOOK_FASTCALL InitialiseActionReplay(void) {
    C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(gReplay_callbacks) != 70);

    ARInitialise(!gAusterity_mode && gNet_mode == eNet_mode_none, CARPOCALYPSE2_ASIZE(gReplay_callbacks), gReplay_callbacks);
    if (!gAusterity_mode && gNet_mode == eNet_mode_none) {
        gCrush_space = BrMemAllocate(0x4000, kMem_pipe_model_geometry);
        gSmudge_space = (tPipe_smudge_data*)gCrush_space;
        return;
    }
    gCrush_space = NULL;
    gSmudge_space = NULL;
}

// FUNCTION: CARMA2_HW 0x004c6c60
void C2_HOOK_FASTCALL DisposeActionReplay(void) {
#ifndef CARPOCALYPSE2_MATCHING
    if (gLocal_buffer != NULL) {
        BrMemFree(gLocal_buffer);
        gLocal_buffer = NULL;
    }
    PDDisposeActionReplayBuffer();
    gPipe_buffer_start = NULL;
    gPipe_buffer_phys_end = NULL;
    gPipe_buffer_working_end = NULL;
    gPipe_buffer_oldest = NULL;
    gPipe_record_ptr = NULL;
    gPipe_buffer_size = 0;
#else
    DisposePiping();
    if (gCrush_space != NULL) {
        BrMemFree(gCrush_space);
        gCrush_space = NULL;
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004e6ff0
void C2_HOOK_FASTCALL CheckReplayTurnOn(void) {
    if (!gAction_replay_mode) {
        if (!KeyIsDown(58) || gEntering_message) {
            gKey_down = -1;
        } else if (gKey_down == -1) {
            ToggleReplay(NULL, NULL);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004e68e0
void C2_HOOK_FASTCALL DoActionReplay(tU32 pFrame_period) {
    gTime_of_last_zappy_headups = 0;
    FUN_00403cc0(pFrame_period, DoZappyActionReplayHeadups, PreProcess, PostProcess);
}

// FUNCTION: CARMA2_HW 0x004e69b0
void C2_NAKED C2_HOOK_FASTCALL PollActionReplayControls(tU32 *pFrame_period, tU32* pAverage_frame_period) {
#ifndef CARPOCALYPSE2_MATCHING

    NOT_IMPLEMENTED();
#else
    __asm {
            sub esp, 0x18
            push ebx
            push ebp
            push esi
            push edi
            mov esi, edx
            mov edi, ecx
            call EdgeTriggerModeOff
            call PDGetTotalTime
            mov dword ptr [esp + 0x14], eax
            call ARGetReplayRate
            fstp dword ptr [esp + 0x10]
            mov ebx, dword ptr [gKey_down]
            call PDAnyKeyDown
            cmp ebx, -1
            mov dword ptr [gKey_down], eax
            je L004e69f0
            cmp ebx, 4
            jne L004e6a77
    L004e69f0:
            mov ecx, 0x3a
            call KeyIsDown
            test eax, eax
            je L004e6a72
            call DRS3StopAllOutletSoundsExceptCDA
            mov eax, dword ptr [gPed_676914]
            test eax, eax
            je L004e6a1f
            mov ebp, 1
            mov ecx, ebp
            call RenderAFrame
            mov ecx, ebp
            call RenderAFrame
    L004e6a1f:
            push esi
            push edi
            push offset PostProcess
            push offset PreProcess
            push offset DoZappyActionReplayHeadups
            push offset AfterActionReplay
            mov edx, offset TurnOffActionReplay
            mov ecx, offset TurnOnActionReplay
            call ARToggleReplay
            test eax, eax
            jne L004e6fd9
            mov ecx, 0x25
            call GetMiscString
            xor edx, edx
            mov ecx, 4
            push eax
            push -4
            push 0x3e8
            call NewTextHeadupSlot
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x18
            ret
    L004e6a72:
            mov eax, dword ptr [gKey_down]
    L004e6a77:
            cmp eax, -1
            mov ebp, 1
            je L004e6a8e
            xor edi, edi
            mov dword ptr [gMouse_in_use], edi
            jmp L004e6b39
    L004e6a8e:
            cmp ebx, -1
            je L004e6aab
            cmp ebx, 4
            je L004e6aab
            cmp ebx, 0x55
            je L004e6aab
            cmp ebx, 0x57
            je L004e6aab
            cmp ebx, 0x4c
            jne L004e6b31
    L004e6aab:
            call EitherMouseButtonDown
            test eax, eax
            je L004e6b2c
            lea edx, [esp + 0x18]
            lea ecx, [esp + 0x1c]
            call GetMousePosition
            mov eax, dword ptr [gReal_graf_data_index]
            mov edx, dword ptr [esp + 0x18]
            mov ecx, dword ptr [esp + 0x1c]
            xor edi, edi
            lea eax, [eax + eax*8]
            xor esi, esi
            shl eax, 4
            add eax, offset gReplay_timeline_y+0x34
    L004e6add:
            cmp ecx, dword ptr [eax - 4]
            jl L004e6af0
            cmp edx, dword ptr [eax]
            jl L004e6af0
            cmp ecx, dword ptr [eax + 4]
            jge L004e6af0
            cmp edx, dword ptr [eax + 8]
            jl L004e6b00
    L004e6af0:
            inc esi
            add eax, 0x10
            cmp esi, 9
            jl L004e6add
            mov eax, dword ptr [gKey_down]
            jmp L004e6b0c
    L004e6b00:
            mov eax, dword ptr [esi*4 + gReplay_timeline_y+0x8]
            mov dword ptr [gKey_down], eax
    L004e6b0c:
            cmp esi, 9
            jne L004e6b24
            cmp dword ptr [gUnk_006a23c8+0x4], edi
            jne L004e6b24
            push edi
            call PickActionReplayManualCameraTarget
            mov eax, dword ptr [gKey_down]
    L004e6b24:
            mov dword ptr [gUnk_006a23c8+0x4], ebp
            jmp L004e6b39
    L004e6b2c:
            mov eax, dword ptr [gKey_down]
    L004e6b31:
            xor edi, edi
            mov dword ptr [gUnk_006a23c8+0x4], edi
    L004e6b39:
            cmp eax, 0x4b
            jne L004e6b54
            cmp ebx, -1
            je L004e6b48
            cmp ebx, 4
            jne L004e6b54
    L004e6b48:
            call MovieRecordButtonPressed
            mov eax, dword ptr [gKey_down]
            jmp L004e6b7f
    L004e6b54:
            cmp eax, 0x4c
            jne L004e6b79
            cmp ebx, eax
            mov dword ptr [gReplay_show_controls], ebp
            je L004e6b7f
            call ChangeCameraType
            call PDGetTotalTime
            mov dword ptr [gUnk_006a23c8], eax
            mov eax, dword ptr [gKey_down]
            jmp L004e6b7f
    L004e6b79:
            mov dword ptr [gReplay_show_controls], edi
    L004e6b7f:
            cmp eax, 0x56
            je L004e6b89
            cmp eax, 0x6a
            jne L004e6b93
    L004e6b89:
            cmp ebx, -1
            je L004e6bcc
            cmp ebx, 4
            je L004e6bcc
    L004e6b93:
            cmp eax, 0x51
            je L004e6b9d
            cmp eax, 0x32
            jne L004e6be2
    L004e6b9d:
            cmp ebx, -1
            je L004e6ba7
            cmp ebx, 4
            jne L004e6be2
    L004e6ba7:
            call ARGetReplayDirection
            neg eax
            mov dword ptr [esp + 0x20], eax
            push ecx
            fild dword ptr [esp + 0x24]
            _emit 0xd9
        _emit 0x1c
        _emit 0x24
            call ARSetReplayDirection
            call ARReplayPauseState
            test eax, eax
            je L004e6bdd
            xor ecx, ecx
            jmp L004e6bd8
    L004e6bcc:
            call ARReplayPauseState
            mov ecx, eax
            neg ecx
            sbb ecx, ecx
            inc ecx
    L004e6bd8:
            call ARSetPause
    L004e6bdd:
            mov eax, dword ptr [gKey_down]
    L004e6be2:
            cmp eax, 0x52
            jne L004e6c10
            cmp ebx, -1
            je L004e6bf1
            cmp ebx, 4
            jne L004e6c10
    L004e6bf1:
            push 0xbf800000
            call ARSetReplayRate
            push 0xbf800000
            call ARSetReplayDirection
            mov dword ptr [gSingle_frame_mode], ebp
            jmp L004e6da2
    L004e6c10:
            cmp eax, 0x54
            jne L004e6c3e
            cmp ebx, -1
            je L004e6c1f
            cmp ebx, 4
            jne L004e6c3e
    L004e6c1f:
            push 0x3f800000
            call ARSetReplayRate
            push 0x3f800000
            call ARSetReplayDirection
            mov dword ptr [gSingle_frame_mode], ebp
            jmp L004e6da2
    L004e6c3e:
            cmp eax, 0x55
            je L004e6d34
            cmp eax, 0x44
            je L004e6d34
            cmp eax, 0x57
            je L004e6cc9
            cmp eax, 0x45
            je L004e6cc9
            cmp eax, 0x3b
            jne L004e6c7f
            push 0xbf800000
            call ARSetReplayRate
            push 0xbf800000
            call ARSetReplayDirection
            xor ecx, ecx
            call ARSetPause
            jmp L004e6da2
    L004e6c7f:
            cmp eax, 0x3a
            jne L004e6ca4
            push 0x3f800000
            call ARSetReplayRate
            push 0x3f800000
            call ARSetReplayDirection
            xor ecx, ecx
            call ARSetPause
            jmp L004e6da2
    L004e6ca4:
            call ARReplayPauseState
            test eax, eax
            je L004e6cb3
            push edi
            jmp L004e6d9d
    L004e6cb3:
            call ARGetReplayDirection
            mov dword ptr [esp + 0x20], eax
            fild dword ptr [esp + 0x20]
            push ecx
            _emit 0xd9
        _emit 0x1c
        _emit 0x24
            jmp L004e6d9d
    L004e6cc9:
            call ARGetReplayRate
            fcomp qword ptr [gConst_kangeroo_force_factor+0x138]
            fnstsw ax
            test ah, 1
            je L004e6ce5
            push 0x3f99999a
            call ARSetReplayRate
    L004e6ce5:
            cmp dword ptr [gUnk_006a23c8+0x8], edi
            je L004e6d1b
            call ARGetReplayRate
            mov eax, dword ptr [esp + 0x14]
            mov edx, dword ptr [gUnk_006a23c8+0x8]
            sub eax, edx
            mov dword ptr [esp + 0x24], edi
            mov dword ptr [esp + 0x20], eax
            push ecx
            fild qword ptr [esp + 0x24]
            fmul qword ptr [gConst_kangeroo_force_factor+0x140]
            fsubp st(1), st(0)
            _emit 0xd9
        _emit 0x1c
        _emit 0x24
            call ARSetReplayRate
    L004e6d1b:
            call ARGetReplayRate
            fcomp dword ptr [gConst_kangeroo_force_factor+0x148]
            fnstsw ax
            test ah, 0x41
            jne L004e6da2
            push 0x41000000
            jmp L004e6d9d
    L004e6d34:
            call ARGetReplayRate
            fcomp qword ptr [gConst_kangeroo_force_factor+0x150]
            fnstsw ax
            test ah, 0x41
            jne L004e6d50
            push 0xbf99999a
            call ARSetReplayRate
    L004e6d50:
            cmp dword ptr [gUnk_006a23c8+0x8], edi
            je L004e6d86
            call ARGetReplayRate
            mov ecx, dword ptr [esp + 0x14]
            mov edx, dword ptr [gUnk_006a23c8+0x8]
            sub ecx, edx
            mov dword ptr [esp + 0x24], edi
            mov dword ptr [esp + 0x20], ecx
            push ecx
            fild qword ptr [esp + 0x24]
            fmul qword ptr [gConst_kangeroo_force_factor+0x158]
            fsubp st(1), st(0)
            _emit 0xd9
        _emit 0x1c
        _emit 0x24
            call ARSetReplayRate
    L004e6d86:
            call ARGetReplayRate
            fcomp dword ptr [gConst_kangeroo_force_factor+0x160]
            fnstsw ax
            test ah, 1
            je L004e6da2
            push 0xc1000000
    L004e6d9d:
            call ARSetReplayRate
    L004e6da2:
            mov eax, dword ptr [gKey_down]
            cmp eax, 0x58
            je L004e6db1
            cmp eax, 0x42
            jne L004e6dc3
    L004e6db1:
            cmp ebx, -1
            je L004e6e54
            cmp ebx, 4
            je L004e6e54
    L004e6dc3:
            cmp eax, 0x5a
            je L004e6dd1
            cmp eax, 0x43
            jne L004e6eca
    L004e6dd1:
            cmp ebx, -1
            je L004e6ddf
            cmp ebx, 4
            jne L004e6eca
    L004e6ddf:
            mov eax, dword ptr [gNoTransients]
            mov dword ptr [gTime_of_last_zappy_headups], edi
            cmp eax, edi
            je L004e6df4
            mov dword ptr [gRecording_mouse_disabled], ebp
    L004e6df4:
            mov ecx, ebp
            call RenderAFrame
            mov ecx, ebp
            call RenderAFrame
            mov edx, offset PreProcess
            mov ecx, offset DoZappyActionReplayHeadups
            push offset PostProcess
            call FUN_00403210
            cmp dword ptr [gNoTransients], edi
            je L004e6e2d
            call GetNextReplayRecordState
            test eax, eax
            jne L004e6e2d
            mov dword ptr [gRecording_mouse_disabled], edi
    L004e6e2d:
            push 0xbf800000
            call ARSetReplayRate
            mov ecx, dword ptr [gFrame_period]
            call MungeCarGraphics
            call GrooveThoseDelics
            push edi
            call ARSetReplayRate
            push 0xbf800000
            jmp L004e6ebe
    L004e6e54:
            mov eax, dword ptr [gNoTransients]
            mov dword ptr [gTime_of_last_zappy_headups], edi
            cmp eax, edi
            je L004e6e69
            mov dword ptr [gRecording_mouse_disabled], ebp
    L004e6e69:
            mov ecx, ebp
            call RenderAFrame
            mov ecx, ebp
            call RenderAFrame
            mov edx, offset PreProcess
            mov ecx, offset DoZappyActionReplayHeadups
            push offset PostProcess
            call FUN_00403c70
            cmp dword ptr [gNoTransients], edi
            je L004e6e99
            mov dword ptr [gRecording_mouse_disabled], edi
    L004e6e99:
            push 0x3f800000
            call ARSetReplayRate
            mov ecx, dword ptr [gFrame_period]
            call MungeCarGraphics
            call GrooveThoseDelics
            push edi
            call ARSetReplayRate
            push 0x3f800000
    L004e6ebe:
            call ARSetReplayDirection
            mov ecx, ebp
            call ARSetPause
    L004e6eca:
            fld dword ptr [gUnk_006a2378]
            fcomp dword ptr [gConst_kangeroo_force_factor+0x130]
            fnstsw ax
            test ah, 0x40
            jne L004e6ef3
            mov edx, dword ptr [gUnk_006a2378]
            push edx
            call ARSetReplayRate
            mov dword ptr [gUnk_006a2378], 0
    L004e6ef3:
            call ARGetReplayRate
            fmul dword ptr [esp + 0x10]
            fcomp dword ptr [gConst_kangeroo_force_factor+0x130]
            fnstsw ax
            test ah, 1
            je L004e6f1c
            call ARGetReplayRate
            fstp dword ptr [gUnk_006a2378]
            push edi
            call ARSetReplayRate
            jmp L004e6f26
    L004e6f1c:
            mov dword ptr [gUnk_006a2378], 0
    L004e6f26:
            fld dword ptr [esp + 0x10]
            fcomp dword ptr [gConst_kangeroo_force_factor+0x130]
            fnstsw ax
            test ah, 0x40
            jne L004e6f5b
            call ARGetReplayRate
            fdiv dword ptr [esp + 0x10]
            mov eax, dword ptr [gFrame_period]
            mov dword ptr [esp + 0x24], edi
            mov dword ptr [esp + 0x20], eax
            fimul dword ptr [esp + 0x20]
            call _ftol
            mov dword ptr [gFrame_period], eax
    L004e6f5b:
            call ARGetReplayRate
            fabs
            fcomp qword ptr [gConst_kangeroo_force_factor+0x138]
            fnstsw ax
            test ah, 1
            je L004e6f77
            mov dword ptr [gUnk_006a23c8+0x8], edi
            jmp L004e6f81
    L004e6f77:
            mov ecx, dword ptr [esp + 0x14]
            mov dword ptr [gUnk_006a23c8+0x8], ecx
    L004e6f81:
            fld dword ptr [esp + 0x10]
            fcomp dword ptr [gConst_kangeroo_force_factor+0x130]
            fnstsw ax
            test ah, 0x41
            je L004e6fb1
            call ARGetReplayRate
            fcomp dword ptr [gConst_kangeroo_force_factor+0x130]
            fnstsw ax
            test ah, 0x41
            jne L004e6fb1
            call ARGoForwards
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x18
            ret
    L004e6fb1:
            fld dword ptr [esp + 0x10]
            fcomp dword ptr [gConst_kangeroo_force_factor+0x130]
            fnstsw ax
            test ah, 1
            jne L004e6fd9
            call ARGetReplayRate
            fcomp dword ptr [gConst_kangeroo_force_factor+0x130]
            fnstsw ax
            test ah, 1
            je L004e6fd9
            call ARGoBackwards
    L004e6fd9:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x18
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00402360
int C2_HOOK_FASTCALL ARReplayIsReallyPaused(void) {
    return gReplay_rate == gFLOAT_00589330;
}

// FUNCTION: CARMA2_HW 0x004c80b0
void C2_NAKED C2_HOOK_FASTCALL PipeSingleGrooveStop(int pGroove_index, br_matrix34* pMatrix, int pPath_interrupt, int pObject_interrupt, float pPath_resumption, float pObject_resumption) {
#ifndef CARPOCALYPSE2_MATCHING

    gUNK_0069410c = *(tU32*)&pPath_resumption;
    ARDoSingleVariedSession(0xd, pGroove_index, 5,
        4, 0, pObject_interrupt,
        4, 4, pObject_interrupt,
        4, 8, *(tU32*)&pPath_resumption,
        4, 0xc, *(tU32*)&pPath_resumption,
        0x30, 0x10, pMatrix);
#else
    __asm {
            mov eax, dword ptr [esp + 0xc]
            push edx
            push 0x10
            push 0x30
            push eax
            push 0xc
            push 4
            mov dword ptr [gUNK_0069410c], eax
            push eax
            mov eax, dword ptr [esp + 0x24]
            push 8
            push 4
            push eax
            push 4
            push 4
            push eax
            push 0
            push 4
            push 5
            push ecx
            push 0xd
            call ARDoSingleVariedSession
            add esp, 0x48
            ret 0x10
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

// ActualActionReplayHeadups

// FUNCTION: CARMA2_HW 0x004e6270
void C2_HOOK_FASTCALL DoActionReplayHeadups(void) {
    ActualActionReplayHeadups(0);
}

// FUNCTION: CARMA2_HW 0x004023a0
void C2_HOOK_FASTCALL ARSetReplayRate(float rate) {
    gReplay_rate = rate;
}

// FUNCTION: CARMA2_HW 0x004e7110
void C2_NAKED C2_HOOK_FASTCALL SynchronizeActionReplay(void) {
#ifndef CARPOCALYPSE2_MATCHING

    while (ARGetReplayRate() != 0.f && (float)(PDGetTotalTime() - gLast_synch_time) < (float)gFrame_period / fabsf(ARGetReplayRate())) {
        ServiceGameInRace();
    }
    gLast_synch_time = PDGetTotalTime();
    if (gSingle_frame_mode) {
        ARSetReplayRate(0.f);
        gSingle_frame_mode = 0;
    }
#else
    __asm {
            push ebp
            mov ebp, esp
            and esp, 0xfffffff8
            sub esp, 0x24
            push esi
            call ARGetReplayRate
            fcomp dword ptr [gConst_kangeroo_force_factor+0x130]
            xor esi, esi
            fnstsw ax
            test ah, 0x40
            jne L004e718d
    L004e712e:
            mov eax, dword ptr [gFrame_period]
            mov dword ptr [esp + 0xc], esi
            mov dword ptr [esp + 8], eax
            fild qword ptr [esp + 8]
            fstp qword ptr [esp + 0x18]
            call PDGetTotalTime
            mov ecx, dword ptr [gLast_synch_time]
            mov dword ptr [esp + 0x14], esi
            sub eax, ecx
            mov dword ptr [esp + 0x10], eax
            fild qword ptr [esp + 0x10]
            fstp qword ptr [esp + 0x20]
            call ARGetReplayRate
            fabs
            fdivr qword ptr [esp + 0x18]
            fcomp qword ptr [esp + 0x20]
            fnstsw ax
            test ah, 0x41
            jne L004e718d
            call ServiceGameInRace
            call ARGetReplayRate
            fcomp dword ptr [gConst_kangeroo_force_factor+0x130]
            fnstsw ax
            test ah, 0x40
            je L004e712e
    L004e718d:
            call PDGetTotalTime
            mov dword ptr [gLast_synch_time], eax
            mov eax, dword ptr [gSingle_frame_mode]
            cmp eax, esi
            je L004e71ac
            push esi
            call ARSetReplayRate
            mov dword ptr [gSingle_frame_mode], esi
    L004e71ac:
            pop esi
            mov esp, ebp
            pop ebp
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004e6260
void C2_HOOK_FASTCALL DoActionReplayPostSwap(void) {
    RemoveTransientBitmaps(1);
}
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

// FUNCTION: CARMA2_HW 0x004022e0
void C2_HOOK_FASTCALL ApplyTime(tU32 pTime) {
    gLast_replay_frame_time = pTime;
}

// FUNCTION: CARMA2_HW 0x004022f0
void C2_HOOK_FASTCALL ARSetPause(int pPause) {
    gReplay_pause_state = pPause;
}

// FUNCTION: CARMA2_HW 0x004e7090
void C2_NAKED C2_HOOK_FASTCALL InitializeActionReplayPix(void) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;

    for (i = 0; i < 11; i++) {
        gReplay_pixelmaps[i] = LoadPixelmap(gReplay_pixie_names[i]);
        BRPM_convert(gReplay_pixelmaps[i], gBack_screen->type);
        BrMapAdd(gReplay_pixelmaps[i]);
    }
#else
    __asm {
            push esi
            xor esi, esi
    L004e7093:
            mov ecx, dword ptr [esi + gReplay_pixie_names]
            call LoadPixelmap
            mov ecx, dword ptr [gBack_screen]
            xor edx, edx
            mov dword ptr [esi + gReplay_headup_graphics], eax
            mov dl, byte ptr [ecx + 0x2c]
            mov ecx, eax
            call BRPM_convert
            mov edx, dword ptr [esi + gReplay_headup_graphics]
            push edx
            call BrMapAdd
            add esi, 4
            add esp, 4
            cmp esi, 0x2c
            jl L004e7093
            pop esi
            ret
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004e70d0
void C2_NAKED C2_HOOK_FASTCALL DisposeReplayPixelmaps(void) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;

    for (i = 0; i < 11; i++) {
        if (gReplay_pixelmaps[i] != NULL) {
            BrMapRemove(gReplay_pixelmaps[i]);
            BrPixelmapFree(gReplay_pixelmaps[i]);
            gReplay_pixelmaps[i] = NULL;
        }
    }
#else
    __asm {
            push esi
            mov esi, offset gReplay_headup_graphics
    L004e70d6:
            mov eax, dword ptr [esi]
            test eax, eax
            je L004e70f6
            push eax
            call BrMapRemove
            mov eax, dword ptr [esi]
            add esp, 4
            push eax
            call BrPixelmapFree
            add esp, 4
            mov dword ptr [esi], 0
    L004e70f6:
            add esi, 4
            cmp esi, offset gReplay_headup_graphics+0x2c
            jl L004e70d6
            pop esi
            ret
    }
#endif
}

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x00403cc0
void C2_HOOK_FASTCALL FUN_00403cc0(int pUnused, tARDoActionReplay_step_callback* pStep_callback, tARDoActionReplay_frame_callback* pFrame_start_callback, tARDoActionReplay_frame_callback* pFrame_end_callback) {
    if (gReplay_rate != gFLOAT_00589330) {
        ARDoActionReplay((int)gReplay_rate, pStep_callback, pFrame_start_callback, pFrame_end_callback);
    }
}
#pragma auto_inline(on)

// FUNCTION: CARMA2_HW 0x00403c70
void C2_HOOK_FASTCALL FUN_00403c70(tARDoActionReplay_step_callback* pStep_callback, tARDoActionReplay_frame_callback* pFrame_start_callback, tARDoActionReplay_frame_callback* pFrame_end_callback) {
    float saved_replay_rate;

    saved_replay_rate = gReplay_rate;
    gDisable_sound = 1;
    gReplay_rate = -100.0f;
    ARDoActionReplay(-2147483647, pStep_callback, pFrame_start_callback, pFrame_end_callback);
    gDisable_sound = 0;
    gReplay_rate = saved_replay_rate;
}