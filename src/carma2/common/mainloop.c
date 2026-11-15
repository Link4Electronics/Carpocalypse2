#include "mainloop.h"

#include "car.h"
#include "controls.h"
#include "crush.h"
#include "depth.h"
#include "displays.h"
#include "drmem.h"
#include "drone.h"
#include "explosions.h"
#include "flicplay.h"
#include "globvars.h"
#include "globvrkm.h"
#include "graphics.h"
#include "globvrpb.h"
#include "input.h"
#include "joystick.h"
#include "loading.h"
#include "main.h"
#include "mainmenu.h"
#include "netgame.h"
#include "network.h"
#include "oil.h"
#include "opponent.h"
#include "pedestrn.h"
#include "physics.h"
#include "piping.h"
#include "powerups.h"
#include "racemem.h"
#include "replay.h"
#include "skidmark.h"
#include "smashing.h"
#include "sound.h"
#include "spark.h"
#include "structur.h"
#include "timers.h"
#include "trig.h"
#include "utility.h"
#include "world.h"

#ifdef CARPOCALYPSE2_MATCHING
#include <windows.h>

/* Defined in drmem.c; also referenced from inline asm in MainGameLoop. */
extern br_size_t gPage_size;
extern SYSTEM_INFO gSystem_info;
#endif

#include "carpocalypse2_macros.h"
#include "mainloop.h"
// GLOBAL: CARMA2_HW 0x0068c814
tU32 gOld_camera_time;

// GLOBAL: CARMA2_HW 0x0068c820
tU32 gActual_last_tick_count;

// GLOBAL: CARMA2_HW 0x0068c824
tU32 gAverage_frame_period;

// GLOBAL: CARMA2_HW 0x0068c770
tU32 gLast_net_auto_increase_credits;

// GLOBAL: CARMA2_HW 0x0068c764
tU32 gLast_credit_headup__mainloop;

// GLOBAL: CARMA2_HW 0x0068c76c
int gLast_time_headup;

// GLOBAL: CARMA2_HW 0x0068c808
int gTime_bonus_headup;

// GLOBAL: CARMA2_HW 0x0068c810
int gRace_bonus_headup;

// GLOBAL: CARMA2_HW 0x0068c788
tU32 gTime_bonus_start;

// GLOBAL: CARMA2_HW 0x0068c784
tTime_bonus_state gTime_bonus_state;

// GLOBAL: CARMA2_HW 0x0068c768
int gWasted_flash_state;

// GLOBAL: CARMA2_HW 0x0068c730
tU32 gWasted_last_flash;

// GLOBAL: CARMA2_HW 0x0068c778
float gFLOAT_0068c778;

// GLOBAL: CARMA2_HW 0x0068c80c
int gCredits_post_race;

// GLOBAL: CARMA2_HW 0x0068c738
br_vector3 gPrev_camera_position;

// GLOBAL: CARMA2_HW 0x0074abf0
tU32 gCamera_period;

// GLOBAL: CARMA2_HW 0x0068c834
tU32 gPrev_task_switched_message;

// GLOBAL: CARMA2_HW 0x0068c838
int gWindow_inactive;

// GLOBAL: CARMA2_HW 0x0068c774
int gTotal_bonus_post_race;

// GLOBAL: CARMA2_HW 0x0068c760
int gQueued_wasted_messages_count;

// GLOBAL: CARMA2_HW 0x0068c77c
tU32 gLast_wasted_message_start;

// GLOBAL: CARMA2_HW 0x0068c748
int gQueued_wasted_messages[5];

// GLOBAL: CARMA2_HW 0x0068c75c
int gCompletion_bonus_post_race;

// GLOBAL: CARMA2_HW 0x0068c780
int gTime_bonus;


// GLOBAL: CARMA2_HW 0x00761ef0
int gCredits_per_second_time_bonus[3];  /* FIXME: rename to gPoints_per_second */

// GLOBAL: CARMA2_HW 0x0074b740
int gAuto_increase_credits_dt[3];

// GLOBAL: CARMA2_HW 0x0074b720
tU32 gNet_auto_increase_credits_dt[8];

// GLOBAL: CARMA2_HW 0x0079eb00
char gHeadup_oppo_ped_text[64];

#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern tU32 C2_HOOK_FASTCALL GetTotalTime(void);
extern tU32 C2_HOOK_FASTCALL PDGetTotalTime(void);
extern tU32 gLast_replay_frame_time;
extern tU32 gLast_tick_count;
extern tNet_mode gNet_mode;
extern int gPed_676914;
extern tProgram_state gProgram_state;
extern tU32 gRace_start;
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern br_uint_16 C2_HOOK_CDECL BrActorToActorMatrix34();
extern br_scalar C2_HOOK_STDCALL FastScalarArcTan2(br_scalar pY, br_scalar pX);
extern void C2_HOOK_FASTCALL PositionExternalCamera(tCar_spec* pCar_spec, tU32 pTime);
extern void C2_HOOK_FASTCALL ResetPedNearness(void);
extern br_actor* gCamera;
extern float gCamera_to_horiz_angle;
extern br_matrix34 gCamera_to_world;
extern char gDouble_0058a068[];
extern br_vector3* gOur_pos;
extern br_actor* gPlayer_car_master_actor;
extern br_actor* gRearview_camera;
extern br_matrix34 gRearview_camera_to_world;
extern br_actor* gUniverse_actor;
extern br_scalar gYon_multiplier;
extern float gYon_squared;
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_FASTCALL ARMainLoopStart(void);
extern void C2_HOOK_FASTCALL ARService(void);
extern void C2_HOOK_FASTCALL AddLostTime(tU32 pLost_time);
extern void C2_HOOK_FASTCALL AnimateSky(void);
extern void C2_HOOK_FASTCALL ApplyPhysicsToCars(tU32 pLast_tick_time, tU32 pFrame_period);
extern void C2_HOOK_FASTCALL AutoSaveAdditionalStuff(void);
extern void C2_HOOK_FASTCALL CameraBugFix(tCar_spec* c, tU32 pTime);
extern void C2_HOOK_FASTCALL ChangeDepthEffect(void);
extern void C2_HOOK_FASTCALL ChangeHeadupText(int pHeadup_index, char* pNew_text);
extern void C2_HOOK_FASTCALL ChangingView(void);
extern void C2_HOOK_FASTCALL CheckCheckpoints(void);
extern void C2_HOOK_FASTCALL CheckRecoveryOfCars(tU32 pEndFrameTime);
extern void C2_HOOK_FASTCALL CheckReplayTurnOn(void);
extern void C2_HOOK_FASTCALL CheckSystemKeys(int pRacing);
extern void C2_HOOK_FASTCALL ClearEntireScreen(void);
extern void C2_HOOK_FASTCALL ClearHeadupSlot(int pSlot_index);
extern void C2_HOOK_FASTCALL ClearQueuedHeadups(void);
extern void C2_HOOK_FASTCALL ControlOurCar(tU32 pTime_difference);
extern void C2_HOOK_FASTCALL CrushBendFlapRend(void);
extern void C2_HOOK_FASTCALL CyclePollKeys(void);
extern tS3_sound_tag C2_HOOK_FASTCALL DRS3StartSound(tS3_outlet* pOutlet, tS3_sound_id pSound);
extern int C2_HOOK_FASTCALL DRS3StopAllOutletSoundsExceptCDA(void);
extern void C2_HOOK_FASTCALL DoActionReplay(tU32 pFrame_period);
extern void C2_HOOK_FASTCALL DoComplexCarModels(void);
extern void C2_HOOK_FASTCALL DoDronePerGameFrameStuff(void);
extern void C2_HOOK_FASTCALL DoLODCarModels(void);
extern void C2_HOOK_FASTCALL DoNetGameManagement(void);
extern void C2_HOOK_FASTCALL DoNetworkHeadups(int pCredits);
extern void C2_HOOK_FASTCALL DoOptionsMenu(void);
extern void C2_HOOK_FASTCALL DoPowerupPeriodics(tU32 pFrame_period);
extern void C2_HOOK_FASTCALL DoWheelDamage(tU32 pFrame_period);
extern void C2_HOOK_FASTCALL EdgeTriggerModeOff(void);
extern void C2_HOOK_FASTCALL EnsurePaletteUp(void);
extern void C2_HOOK_FASTCALL EnsureRenderPalette(void);
extern void C2_HOOK_FASTCALL EnsureSpecialVolumesHidden(void);
extern void C2_HOOK_FASTCALL EnterUserMessage(void);
extern void C2_HOOK_FASTCALL FadePaletteDown(void);
extern int C2_HOOK_FASTCALL Flash(tU32 pPeriod, tU32* pLast_change, int* pCurrent_state);
extern void C2_HOOK_FASTCALL FlushSmashQueue(int pFlush_powerups);
extern void C2_HOOK_FASTCALL ForceRebuildActiveCarList(void);
extern void C2_HOOK_FASTCALL FunkThoseTronics(void);
extern void C2_HOOK_FASTCALL GetAverageGridPosition(tRace_info* pThe_race);
extern int C2_HOOK_FASTCALL GetCarCount(tVehicle_type pCategory);
extern const char* C2_HOOK_FASTCALL GetMiscString(int pIndex);
extern tU32 C2_HOOK_FASTCALL GetRaceTime(void);
extern int C2_HOOK_FASTCALL GetRuntimeVariable(int pAddress);
extern void C2_HOOK_FASTCALL GoingBackToRaceFromInterface(void);
extern void C2_HOOK_FASTCALL GoingToInterfaceFromRace(void);
extern void C2_HOOK_FASTCALL GrooveThoseDelics(void);
extern void C2_HOOK_FASTCALL InitSmashQueue(void);
extern void C2_HOOK_FASTCALL InitialiseActionReplay(void);
extern void C2_HOOK_FASTCALL InitialiseExtraRenders(void);
extern int C2_HOOK_FASTCALL KeyIsDown(int pKey_index);
extern void C2_HOOK_FASTCALL LastChanceForPedEffects(void);
extern void C2_HOOK_FASTCALL MakeFlagWavingBastardWaveHisFlagWhichIsTheProbablyTheLastThingHeWillEverDo(void);
extern void C2_HOOK_FASTCALL MungeCarGraphics(tU32 pFrame_period);
extern void C2_HOOK_FASTCALL MungeDelayedSideEffects(void);
extern void C2_HOOK_FASTCALL MungeEngineNoise(void);
extern void C2_HOOK_FASTCALL MungeEnvironmentalSound(void);
extern void C2_HOOK_FASTCALL MungeExplosions(void);
extern void C2_HOOK_FASTCALL MungeGlassFragments(void);
extern void C2_HOOK_FASTCALL MungeJoystickHeadups(void);
extern void C2_HOOK_FASTCALL MungeLightingEffects(void);
extern void C2_HOOK_FASTCALL MungeNapalm(void);
extern void C2_HOOK_FASTCALL MungeOpponents(void);
extern void C2_HOOK_FASTCALL MungePalette(void);
extern void C2_HOOK_FASTCALL MungePedestrians(void);
extern void C2_HOOK_FASTCALL MungePowerupStuff(undefined4 pArg1);
extern void C2_HOOK_FASTCALL MungeShrapnel(tU32 pTime);
extern void C2_HOOK_FASTCALL MungeSmashEdgeTriggers(tU32 pTime);
extern void C2_HOOK_FASTCALL MungeSomeOtherCarGraphics(void);
extern void C2_HOOK_FASTCALL NetFinishRace(tNet_game_details* pDetails, tRace_over_reason pReason);
extern void C2_HOOK_FASTCALL NetPlayerStatusChanged(tPlayer_status pNew_status);
extern void C2_HOOK_FASTCALL NetReceiveAndProcessMessages(void);
extern void C2_HOOK_FASTCALL NetSendMessageStacks(void);
extern int C2_HOOK_FASTCALL NewImageHeadupSlot(int pSlot_index, int pFlash_rate, int pLifetime, int pImage_index);
extern int C2_HOOK_FASTCALL NewTextHeadupSlot(int pSlot_index, int pFlash_rate, int pLifetime, int pFont_index, const char* pText);
extern int C2_HOOK_FASTCALL NumberOfOpponentsLeft(void);
extern int C2_HOOK_FASTCALL PDGetMicroseconds(void);
extern int C2_HOOK_FASTCALL PDIsWindowInactive(void);
extern int C2_HOOK_FASTCALL PDKeyDown(int pKey_index);
extern void C2_HOOK_FASTCALL PHILDisable(void);
extern void C2_HOOK_FASTCALL PipeCarPositions(void);
extern void C2_HOOK_FASTCALL PollActionReplayControls(tU32 *pFrame_period, tU32* pAverage_frame_period);
extern void C2_HOOK_FASTCALL PollCameraControls(tU32 pCamera_period);
extern void C2_HOOK_FASTCALL PollCarControls(tU32 pTime_difference);
extern void C2_HOOK_FASTCALL PrintMemoryDump(int pFlags, char* pTitle);
extern void C2_HOOK_FASTCALL ProcessOilSpills(tU32 pFrame_period);
extern void C2_HOOK_FASTCALL ProcessShitMines(tU32 pTime);
extern void C2_HOOK_FASTCALL RaceCompleted(tRace_over_reason pReason);
extern void C2_HOOK_FASTCALL RenderAFrame(int pDepth_mask_on);
extern void C2_HOOK_FASTCALL ResetCarScreens(void);
extern void C2_HOOK_FASTCALL ResetGrooveFlags(void);
extern void C2_HOOK_FASTCALL ResetLollipopQueue(void);
extern void C2_HOOK_FASTCALL ResetPalette(void);
extern void C2_HOOK_FASTCALL RevertPalette(void);
extern void C2_HOOK_FASTCALL ServiceGameInRace(void);
extern void C2_HOOK_FASTCALL ShowSpecialVolumesIfRequ(void);
extern void C2_HOOK_FASTCALL SkidsPerFrame(void);
extern void C2_HOOK_FASTCALL StopCarBeingIt(tCar_spec* pCar);
extern void C2_HOOK_FASTCALL SuspendPendingFlic(void);
extern void C2_HOOK_FASTCALL TimerString(tU32 pTime, char* pStr, undefined4 pArg3, int pFudge_colon, int pFloat);
extern void C2_HOOK_FASTCALL Timers_EndFrame(void);
extern void C2_HOOK_FASTCALL Timers_Init(void);
extern void C2_HOOK_FASTCALL Timers_StartFrame(void);
extern void C2_HOOK_FASTCALL ToggleMap(void);
extern void C2_HOOK_FASTCALL ToggleReplay(int* pArg1, int* pArg2);
extern void C2_HOOK_FASTCALL UpdateFramePeriod(tU32* pCamera_period);
extern void C2_HOOK_FASTCALL WaitForNoKeys(void);
extern void __cdecl _ftol();
extern int gAbandon_game;
extern tCar_spec* gActive_car_list[139];
extern br_actor* gAdditional_actors;
extern int gCar_kill_count_headup;
extern tCar_spec* gCar_to_view;
extern tU32 gCar_to_view_id;
extern int gCheckpoint;
extern int gCheckpoint_count;
extern int gCount_killed_peds;
extern int gCount_opponents;
extern int gCountdown;
extern int gCredits_won_headup;
extern tNet_game_details* gCurrent_net_game;
extern tRace_info gCurrent_race;
extern float gDrone_delta_time;
extern int gEntering_message;
extern int gFirst_drone_processing;
extern tU32 gFrame_period_for_this_munging;
extern int gFrame_rate;
extern int gFreeze_timer;
extern int gHeadup_detail_level;
extern int gHost_abandon_game;
extern int gInitialised_grid;
extern int gIt_or_fox;
extern tU32 gKnobbled_frame_period;
extern int gLap;
extern int gLaps_headup;
extern int gMap_view;
extern int gNo_credits_APO_restore;
extern int gNo_races_yet;
extern int gNum_active_cars;
extern int gNumber_of_net_players;
extern tOpponent* gOpponents;
extern tU32 gPalette_fade_time;
extern int gPed_count;
extern int gPed_kill_count_headup;
extern tS3_outlet* gPedestrians_outlet;
extern tU32 gRace_finished;
extern tRace_over_reason gRace_over_reason;
extern int gReceived_game_scores;
extern int gRecover_car;
extern tFloat_bunch_info gRecovery_cost;
extern int gStart_race_sent;
extern int gThis_net_player_index;
extern tU32 gTimer;
extern int gTimer_headup;
extern int gTimers_max_index;
extern int gTimers_stack[19];
extern int gTimers_stack_size;
extern int gTotal_count_smash_peds;
extern int gTotal_laps;
extern tU32 gWait_for_it;
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern char gData_00658944[];
extern char gData_0065896c[];
extern char gData_00658980[];
extern char gData_00658990[];
extern char gData_0068c804[];
extern char gData_006b75dc[];
extern char gDouble_0058a2e8[];
extern char gDouble_0058a2f0[];
extern char gDouble_0058a2f8[];
extern char gDouble_0058a300[];
extern char gDouble_0058a310[];
extern char gDouble_0058a318[];
extern char gDouble_0058a320[];
extern char gDouble_0058a328[];
extern char gDouble_0058a338[];
extern char gFloat_0058930c[];
extern char gFloat_0058a308[];
extern char gFloat_0058a330[];
extern char gFloat_006588f0[];
extern char gFloat_00658908[];
extern char gFloat_0065892c[];
extern char gFloat_00658978[];
extern char gFloat_00658998[];
extern char gString_0058fd6c[];
extern char gString_00658960[];
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern char gMatchData_00658944[];
extern char gMatchData_0065896c[];
extern char gMatchData_00658980[];
extern char gMatchData_00658990[];
extern char gMatchData_0068c804[];
extern char gMatchData_006b75dc[];
extern char gMatchDouble_0058a2e8[];
extern char gMatchDouble_0058a2f0[];
extern char gMatchDouble_0058a2f8[];
extern char gMatchDouble_0058a300[];
extern char gMatchDouble_0058a310[];
extern char gMatchDouble_0058a318[];
extern char gMatchDouble_0058a320[];
extern char gMatchDouble_0058a328[];
extern char gMatchDouble_0058a338[];
extern char gMatchFloat_0058930c[];
extern char gMatchFloat_0058a308[];
extern char gMatchFloat_0058a330[];
extern char gMatchFloat_006588f0[];
extern char gMatchFloat_00658908[];
extern char gMatchFloat_0065892c[];
extern char gMatchFloat_00658978[];
extern char gMatchFloat_00658998[];
extern char gMatchString_0058fd6c[];
extern char gMatchString_00658960[];
#endif
// FUNCTION: CARMA2_HW 0x00401170
void C2_NAKED C2_HOOK_FASTCALL MungeAIWorld(tU32 pFrame_period) {
#ifndef CARPOCALYPSE2_MATCHING

    gTime_stamp_for_this_munging = GetTotalTime();
    if (gTime_stamp_for_this_munging == 0) {
        gTime_stamp_for_this_munging = PDGetTotalTime();
    }
    gDrone_delta_time = (float)pFrame_period / 1000.f;
    gFrame_period_for_this_munging = pFrame_period;
    Timers_Push(TIMER_DRN);
    DoDronePerGameFrameStuff();
    Timers_Pop(TIMER_DRN);
    Timers_Push(TIMER_OPP);
    MungeOpponents();
    Timers_Pop(TIMER_OPP);
    gFirst_drone_processing = 0;
    ProcessShitMines(gTime_stamp_for_this_munging);
#else
    __asm {
            sub esp, 8
            push ebx
            push esi
            push edi
            mov ebx, ecx
            call GetTotalTime
            test eax, eax
            mov dword ptr [gTime_stamp_for_this_munging], eax
            jne L00401190
            call PDGetTotalTime
            mov dword ptr [gTime_stamp_for_this_munging], eax
    L00401190:
            mov dword ptr [esp + 0xc], ebx
            mov dword ptr [esp + 0x10], 0
            fild qword ptr [esp + 0xc]
            mov eax, dword ptr [gTimers_stack_size]
            mov dword ptr [gFrame_period_for_this_munging], ebx
            test eax, eax
            fmul dword ptr [gMatchFloat_0058930c]
            mov dword ptr [eax*4 + gTimers_stack], 6
            fstp dword ptr [gDrone_delta_time]
            je L004011ea
            mov eax, dword ptr [eax*4 + gMatchData_006b75dc]
            lea esi, [eax*8]
            lea edx, [eax*8]
            sub esi, eax
            shl esi, 2
            xor ecx, ecx
            mov cl, byte ptr [esi + gTimers+0x8]
            jmp L00401206
    L004011ea:
            mov eax, dword ptr [gTimers_max_index]
            xor ecx, ecx
            mov esi, eax
            mov edx, eax
            shl esi, 3
            sub esi, eax
            shl esi, 2
            shl edx, 3
            mov cl, byte ptr [esi + gTimers+0x8]
    L00401206:
            sub edx, eax
            add ecx, edx
            lea edi, [ecx*4 + gTimers+0x4]
            call PDGetMicroseconds
            mov edx, dword ptr [esi + gTimers+0xc]
            mov ecx, dword ptr [edi]
            sub eax, edx
            add ecx, eax
            mov dword ptr [edi], ecx
            call PDGetMicroseconds
            mov edi, dword ptr [gTimers_stack_size]
            mov dword ptr [gTimers+0xb4], eax
            inc edi
            mov dword ptr [gTimers_stack_size], edi
            call DoDronePerGameFrameStuff
            mov eax, dword ptr [gTimers+0xb0]
            and eax, 0xff
            lea esi, [eax*4 + gTimers+0xac]
            call PDGetMicroseconds
            mov edx, dword ptr [gTimers+0xb4]
            mov ecx, dword ptr [esi]
            sub eax, edx
            add ecx, eax
            mov eax, dword ptr [gTimers_stack_size]
            cmp eax, 1
            mov dword ptr [esi], ecx
            jle L00401296
            call PDGetMicroseconds
            mov edx, dword ptr [gTimers_stack_size]
            mov ecx, dword ptr [edx*4 + gMatchData_006b75dc]
            mov esi, ecx
            shl esi, 3
            sub esi, ecx
            mov ecx, dword ptr [gTimers_max_index]
            mov dword ptr [esi*4 + gTimers+0xc], eax
            jmp L004012b5
    L00401296:
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_max_index]
            mov edx, ecx
            shl edx, 3
            sub edx, ecx
            mov dword ptr [edx*4 + gTimers+0xc], eax
            mov edx, dword ptr [gTimers_stack_size]
    L004012b5:
            dec edx
            mov dword ptr [gTimers_stack_size], edx
            mov dword ptr [edx*4 + gTimers_stack], 7
            je L004012f8
            mov eax, dword ptr [edx*4 + gMatchData_006b75dc]
            xor ecx, ecx
            lea esi, [eax*8]
            lea edx, [eax*8]
            sub esi, eax
            sub edx, eax
            shl esi, 2
            mov cl, byte ptr [esi + gTimers+0x8]
            add ecx, edx
            lea edi, [ecx*4 + gTimers+0x4]
            jmp L0040131a
    L004012f8:
            mov esi, ecx
            mov edx, ecx
            shl esi, 3
            sub esi, ecx
            xor eax, eax
            shl esi, 2
            shl edx, 3
            mov al, byte ptr [esi + gTimers+0x8]
            sub edx, ecx
            add eax, edx
            lea edi, [eax*4 + gTimers+0x4]
    L0040131a:
            call PDGetMicroseconds
            mov edx, dword ptr [esi + gTimers+0xc]
            mov ecx, dword ptr [edi]
            sub eax, edx
            add ecx, eax
            mov dword ptr [edi], ecx
            call PDGetMicroseconds
            mov edi, dword ptr [gTimers_stack_size]
            mov ecx, ebx
            inc edi
            mov dword ptr [gTimers+0xd0], eax
            mov dword ptr [gTimers_stack_size], edi
            call MungeOpponents
            mov eax, dword ptr [gTimers+0xcc]
            and eax, 0xff
            lea esi, [eax*4 + gTimers+0xc8]
            call PDGetMicroseconds
            mov edx, dword ptr [gTimers+0xd0]
            mov ecx, dword ptr [esi]
            sub eax, edx
            add ecx, eax
            mov eax, dword ptr [gTimers_stack_size]
            cmp eax, 1
            mov dword ptr [esi], ecx
            jle L0040138d
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            mov ecx, dword ptr [ecx*4 + gMatchData_006b75dc]
            jmp L00401398
    L0040138d:
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_max_index]
    L00401398:
            mov edx, ecx
            mov dword ptr [gFirst_drone_processing], 0
            shl edx, 3
            sub edx, ecx
            mov ecx, dword ptr [gTime_stamp_for_this_munging]
            mov dword ptr [edx*4 + gTimers+0xc], eax
            mov eax, dword ptr [gTimers_stack_size]
            dec eax
            mov dword ptr [gTimers_stack_size], eax
            call ProcessShitMines
            pop edi
            pop esi
            pop ebx
            add esp, 8
            ret
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004940e0
void C2_NAKED C2_HOOK_FASTCALL CalculateCameraStuff(tU32 pCamera_period) {
#ifndef CARPOCALYPSE2_MATCHING
    br_camera* camera;
    br_vector3 delta_camera;

    gOur_pos = &gPlayer_car_master_actor->t.t.translate.t;
    PositionExternalCamera(&gProgram_state.current_car, pCamera_period);
    BrActorToActorMatrix34(&gCamera_to_world, gCamera, gUniverse_actor);
    BrActorToActorMatrix34(&gRearview_camera_to_world, gRearview_camera, gUniverse_actor);
    gCamera_to_horiz_angle = FastScalarArcTan2(gCamera_to_world.m[2][1], gCamera_to_world.m[1][1]);

    camera = gCamera->type_data;
    gYon_squared = gYon_multiplier * gYon_multiplier * camera->yon_z * camera->yon_z;
    BrVector3Sub(&delta_camera, (br_vector3*)gCamera_to_world.m[3], &gPrev_camera_position);
    if (BrVector3LengthSquared(&delta_camera) > 400.0) {
        ResetPedNearness();
    }
    BrVector3Copy(&gPrev_camera_position, (br_vector3*)gCamera_to_world.m[3]);
#else
    __asm {
            sub esp, 8
            mov eax, dword ptr [gPlayer_car_master_actor]
            mov edx, ecx
            add eax, 0x50
            mov ecx, offset gProgram_state+0xac
            mov dword ptr [gOur_pos], eax
            call PositionExternalCamera
            mov ecx, dword ptr [gUniverse_actor]
            mov edx, dword ptr [gCamera]
            push ecx
            push edx
            push offset gCamera_to_world
            call BrActorToActorMatrix34
            mov eax, dword ptr [gUniverse_actor]
            mov ecx, dword ptr [gRearview_camera]
            add esp, 0xc
            push eax
            push ecx
            push offset gRearview_camera_to_world
            call BrActorToActorMatrix34
            mov edx, dword ptr [gCamera_to_world+0x10]
            mov eax, dword ptr [gCamera_to_world+0x1c]
            add esp, 0xc
            push edx
            push eax
            call FastScalarArcTan2
            fld dword ptr [gCamera_to_world+0x28]
            fld dword ptr [gCamera_to_world+0x24]
            fsub dword ptr [gPrev_camera_position]
            fxch st(1)
            fsub dword ptr [gPrev_camera_position+0x4]
            fld dword ptr [gCamera_to_world+0x2c]
            fsub dword ptr [gPrev_camera_position+0x8]
            fxch st(2)
            _emit 0xd9
        _emit 0x54
        _emit 0x24
        _emit 0x00
            fxch st(1)
            fst dword ptr [esp + 4]
            fxch st(1)
            _emit 0xd8
        _emit 0x4c
        _emit 0x24
        _emit 0x00
            fxch st(1)
            fmul dword ptr [esp + 4]
            fxch st(2)
            fst dword ptr [esp + 4]
            fmul dword ptr [esp + 4]
            fld dword ptr [gYon_multiplier]
            fxch st(4)
            fstp dword ptr [gCamera_to_horiz_angle]
            mov ecx, dword ptr [gCamera]
            fxch st(2)
            faddp st(1), st(0)
            fxch st(2)
            fmul dword ptr [gYon_multiplier]
            fxch st(1)
            faddp st(2), st(0)
            mov edx, dword ptr [ecx + 0x5c]
            mov eax, dword ptr [edx + 0xc]
            mov dword ptr [esp + 4], eax
            fmul dword ptr [esp + 4]
            fxch st(1)
            fcomp qword ptr [gMatchDouble_0058a338]
            fmul dword ptr [esp + 4]
            fnstsw ax
            test ah, 0x41
            fstp dword ptr [gYon_squared]
            jne L004941db
            call ResetPedNearness
    L004941db:
            mov ecx, dword ptr [gCamera_to_world+0x24]
            mov edx, dword ptr [gCamera_to_world+0x28]
            mov eax, dword ptr [gCamera_to_world+0x2c]
            mov dword ptr [gPrev_camera_position], ecx
            mov dword ptr [gPrev_camera_position+0x4], edx
            mov dword ptr [gPrev_camera_position+0x8], eax
            add esp, 8
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
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00494210
int C2_HOOK_FAKE_THISCALL FUN_00494210(int pStatus, undefined4 pArg2, undefined4 pArg3, undefined4 pArg4, int pFlag) {
    (void)pArg2;
    (void)pArg3;
    (void)pArg4;
    if (pFlag != 0) {
        return -1;
    }
    switch (pStatus) {
    case 1:
        PreloadBunchOfFlics(4);
        return pStatus;
    case 2:
        PreloadBunchOfFlics(5);
        return pStatus;
    case 3:
        PreloadBunchOfFlics(1);
        return pStatus;
    case 4:
        PreloadBunchOfFlics(3);
        return pStatus;
    case 5:
        PreloadBunchOfFlics(2);
        return pStatus;
    case 7:
        PreloadBunchOfFlics(7);
    }
    return pStatus;
}

void C2_HOOK_FASTCALL CheckActiveCarList(void) {
    int i;
    int new_count = 0;

    for (i = 0; i < gNum_active_cars; i++) {
        tCar_spec *c;

        c = gActive_car_list[i];
        if (c != NULL && c->driver > 5) {
            if (i != new_count) {
                gActive_car_list[new_count] = c;
            }
            new_count += 1;
        }
    }
    gNum_active_cars = new_count;
}

void C2_HOOK_FASTCALL CalculateFrameRate(void) {
    // GLOBAL: CARMA2_HW 0x0068c734
    static  tU32 last_time;
    tU32 new_time;
    // GLOBAL: CARMA2_HW 0x0068c790
    static int last_rates[30];
    int i;
    int new_rate;

    new_time = PDGetTotalTime();
    if (new_time != last_time) {
        new_rate = 10000 / (new_time - last_time);
        last_rates[CARPOCALYPSE2_ASIZE(last_rates) - 1] = new_rate;
        gFrame_rate = new_rate;
        for (i = 0; i < CARPOCALYPSE2_ASIZE(last_rates); i++) {
            gFrame_rate += last_rates[i];
        }
        gFrame_rate /= CARPOCALYPSE2_ASIZE(last_rates) + 1;
        for (i = 0; i < CARPOCALYPSE2_ASIZE(last_rates) - 1; i++) {
            last_rates[i] = last_rates[i + 1];
        }
        last_time = new_time;
    }
}

// FUNCTION: CARMA2_HW 0x00492840
void C2_HOOK_FASTCALL FUN_00492840(void* pMemory, br_size_t pSize) {
#ifdef CARPOCALYPSE2_MATCHING
    extern br_size_t gPage_size;
    extern SYSTEM_INFO gSystem_info;
    br_size_t offset;

    if (gPage_size == 0) {
        GetSystemInfo(&gSystem_info);
        gPage_size = gSystem_info.dwOemId;
    }
    for (offset = gPage_size << 4; offset < pSize; offset += gPage_size) {
        *((volatile br_uint_8*)pMemory + (offset - 16 * gPage_size)) = *((volatile br_uint_8*)pMemory + (offset - 16 * gPage_size));
        *((volatile br_uint_8*)pMemory + offset) = *((volatile br_uint_8*)pMemory + offset);
    }
#else
    (void)pMemory;
    (void)pSize;
#endif
}

// FUNCTION: CARMA2_HW 0x00492980
tRace_result C2_NAKED C2_HOOK_FASTCALL MainGameLoop(void) {
#ifndef CARPOCALYPSE2_MATCHING
    int tried_to_allocate_AR;
    tRace_result result;

    tried_to_allocate_AR = 0;
    gCar_to_view = &gProgram_state.current_car;
    gCar_to_view_id = 0;
    gOld_camera_time = 0;
    gLast_net_auto_increase_credits = 0;
    gLast_credit_headup__mainloop = 0;
    gLast_time_headup = gTimer;
    gRace_bonus_headup = -1;
    gTime_bonus_headup = -1;
    gTime_bonus_start = 0;
    gTime_bonus_state = 0;
    ARMainLoopStart();
    gActual_last_tick_count = PDGetTotalTime();
    gFrame_period = 100;
    gQueued_wasted_messages_count = 0;
    gWasted_flash_state = 0;
    gWasted_last_flash = 0;
    gLast_tick_count = gActual_last_tick_count;
    RevertPalette();
    gHost_abandon_game = 0;
    gNo_races_yet = 0;
    gRace_over_reason = eRace_not_over_yet;
    gFLOAT_0068c778 = 0.f;
    gNo_credits_APO_restore = 0;
    gReceived_game_scores = 0;
    if (gNet_mode == eNet_mode_none) {
        gProgram_state.field_0x2c = 1;
    }
    gCredits_post_race = -1;
    ShowSpecialVolumesIfRequ();
    gProgram_state.racing = 1;
    if (gNet_mode == eNet_mode_host) {
        gCurrent_net_game->status.stage = eNet_game_playing;
    }
    NetPlayerStatusChanged(ePlayer_status_racing);
    GetAverageGridPosition(&gCurrent_race);
    ForceRebuildActiveCarList();
    PrintMemoryDump(0, "ABOUT TO ENTER MAINLOOP");
    UpdateFramePeriod(&gCamera_period);
    BrVector3Set(&gPrev_camera_position, 1000.f, 1000.f, 1000.f);
    CalculateCameraStuff(gCamera_period);

    /* Only present in win32 version */
    PDPageInProcessMemory();

    gRace_start = PDGetTotalTime();
    Timers_Init();

    if (gWait_for_it != 0) {
        gWait_for_it += GetTotalTime();
    }
    ClearEntireScreen();
    ResetPalette();
    for (;;) {
        int window_inactive;
        tU32 frame_start_time;

        Timers_StartFrame();
        EdgeTriggerModeOff();
        if (!gPed_676914) {
            UpdateFramePeriod(&gCamera_period);
        }
        frame_start_time = GetTotalTime();
        CyclePollKeys();
        InitSmashQueue();
        CheckSystemKeys(1);
        InitialiseExtraRenders();
        NetReceiveAndProcessMessages();

        if (gHost_abandon_game || gProgram_state.prog_status == eProg_idling) {
            break;
        }
        if (gNet_mode != eNet_mode_none && gCurrent_net_game->type == eNet_game_type_foxy && gThis_net_player_index == gIt_or_fox && gMap_view == 2) {
            ToggleMap();
        }
        ResetGrooveFlags();
        ServiceGameInRace();
        EnterUserMessage();
        ARService();
        ResetLollipopQueue();
        MungePalette();

        window_inactive = PDIsWindowInactive();
        if (window_inactive) {

            if ((tU32)PDGetTotalTime() > gPrev_task_switched_message + 1000) {
                gPrev_task_switched_message = GetTotalTime();
                NewTextHeadupSlot(4, 0, 500, -4, GetMiscString(eMiscString_task_switched));
            }
        } else {
            if (gWindow_inactive) {
                ClearHeadupSlot(4);
                ClearQueuedHeadups();
            }
        }
        gWindow_inactive = window_inactive;

        if (!gPed_676914) {
            MungeAIWorld(gFrame_period);
            PollCarControls(gFrame_period);
        }

        if (!gPed_676914) {
            CheckActiveCarList();

            Timers_Push(TIMER_PED);
            MungePedestrians();
            Timers_Pop(TIMER_PED);

            ControlOurCar(gFrame_period);

            Timers_Push(TIMER_PHS);
            ApplyPhysicsToCars(gLast_tick_count - gRace_start, gFrame_period);
            Timers_Pop(TIMER_PHS);

            MungeSomeOtherCarGraphics();
            PipeCarPositions();

            Timers_Push(TIMER_CRS);
            CrushBendFlapRend();
            Timers_Pop(TIMER_CRS);

            DoNetGameManagement();
            CheckRecoveryOfCars(gFrame_period + gLast_tick_count - gRace_start);
            NetSendMessageStacks();
        } else {
            DoActionReplay(gFrame_period);
        }
        PollCameraControls(gCamera_period);
        MungeEngineNoise();
        CalculateCameraStuff(gCamera_period);
        MungeNapalm();
        if (!gPed_676914) {
            DoPowerupPeriodics(gFrame_period);
        }
        if (!gPed_676914) {
            CheckCheckpoints();
            MungeEnvironmentalSound();
        }
        ChangingView();
        MungeCarGraphics(gFrame_period);
        FunkThoseTronics();
        GrooveThoseDelics();
        DoWheelDamage(gFrame_period);
        CalculateFrameRate();
        CameraBugFix(&gProgram_state.current_car, gCamera_period);
        if (!gPed_676914) {
            /* MungeHeadups() inlined to match retail codegen */
            {
            char the_text[256];
            int net_credits;
            int effective_timer;
            int bonus;
            tU32 the_time;
            // GLOBAL: CARMA2_HW 0x0068c818
            static tU32 last_rattle_time;

            ClearHeadupSlot(3);
            MungeJoystickHeadups();
            net_credits = gProgram_state.credits;
            if (fabsf((float)gProgram_state.credits - (float)gLast_credit_headup__mainloop) / (float)gFrame_rate > 1.2f) {
                if (gProgram_state.credits - gLast_credit_headup__mainloop > 0) {
                    net_credits = (int)((float)gLast_credit_headup__mainloop + ((float)gProgram_state.credits - (float)gLast_credit_headup__mainloop + 1000.f) * (float)gFrame_period * 1.2f / 1000.f);
                } else {
                    net_credits = (int)((float)gLast_credit_headup__mainloop - ((float)gLast_credit_headup__mainloop - (float)gProgram_state.credits + 1000.f) * (float)gFrame_period * 1.2f / 1000.f);
                }
            }

            if (gNet_mode != eNet_mode_none
                    && gCountdown == 0
                    && gNet_auto_increase_credits_dt[gCurrent_net_game->type] != 0
                    && net_credits < gRecovery_cost.initial_network[gCurrent_net_game->type]
                    && PDGetTotalTime() - gLast_net_auto_increase_credits >= gNet_auto_increase_credits_dt[gCurrent_net_game->type]) {

                gLast_net_auto_increase_credits = PDGetTotalTime();
                gProgram_state.credits += 50;

        #define GET_RECOVERY_COST() ((gNet_mode == eNet_mode_none) ? \
            gRecovery_cost.initial[gProgram_state.skill_level] : \
            gRecovery_cost.initial_network[gCurrent_net_game->type])

                if (net_credits + 50 > GET_RECOVERY_COST()) {
                    gProgram_state.credits += (int)(GET_RECOVERY_COST() - net_credits - 50.f);
                    net_credits = (int)GET_RECOVERY_COST();
                } else {
                    net_credits += 50;
                }
            }
            gLast_credit_headup__mainloop = net_credits;

            if (gCountdown != 0) {
                int new_countdown;

                new_countdown = (int)(7.5 - GetRaceTime() / 1000.0);
                if (new_countdown < 0) {
                    new_countdown = 0;
                }
                if (gCountdown != new_countdown && new_countdown <= 5) {
                    gCountdown = new_countdown;
                    NewImageHeadupSlot(5, 0, 800, 4 + new_countdown);
                    DRS3StartSound(gPedestrians_outlet, eSoundId_Countdown_Start + gCountdown);
                    if (new_countdown == 0) {
                        MakeFlagWavingBastardWaveHisFlagWhichIsTheProbablyTheLastThingHeWillEverDo();
                    }
                }
            }
            if (fabsf((float)gTimer - (float)gLast_time_headup) / (float)gFrame_period <= 10.0) {
                effective_timer = gTimer;
            } else if ((int)gTimer < gLast_time_headup) {
                effective_timer = gTimer;
            } else {
                effective_timer = (int)((float)gLast_time_headup + 10.f * (float)gFrame_period);
            }
            gLast_time_headup = effective_timer;

            if (gNet_mode == eNet_mode_none) {
                if (net_credits < 0) {
                    sprintf(the_text, "\xf8%d\xfa %s", -net_credits, GetMiscString(eMiscString_credits_lost));
                    ChangeHeadupText(gCredits_won_headup, the_text);
                } else {
                    sprintf(the_text, "\xf8%d\xfa %s", net_credits, GetMiscString(net_credits >= 100000 ? eMiscString_credits_abbrev : eMiscString_credits_gained));
                    ChangeHeadupText(gCredits_won_headup, the_text);
                }
                if (gCurrent_race.race_spec->race_type == kRaceType_Carma1) {
                    sprintf(the_text, "\xf8%d\xfa/\xf8%d\xfa %s", gCount_killed_peds, gPed_count, GetMiscString(eMiscString_kills));
                    ChangeHeadupText(gPed_kill_count_headup, the_text);
                } else if (gCurrent_race.race_spec->race_type == kRaceType_Peds) {
                    sprintf(the_text, "\xf8%d\xfa/\xf8%d\xfa %s", gCount_killed_peds, gTotal_count_smash_peds, GetMiscString(eMiscString_kills));
                    ChangeHeadupText(gPed_kill_count_headup, the_text);
                }
                if (gCurrent_race.race_spec->race_type == kRaceType_Carma1
                        || gCurrent_race.race_spec->race_type == kRaceType_Cars) {

                    if (gQueued_wasted_messages_count != 0 && GetTotalTime() > gLast_wasted_message_start + 5000) {
                        LoseOldestWastedMessage();
                    }

                    if (gQueued_wasted_messages_count == 0) {
                        int count_oppos;

                        if (gCurrent_race.race_spec->race_type == kRaceType_Carma1) {
                            count_oppos = GetCarCount(eVehicle_opponent);
                        } else {
                            count_oppos = gCount_opponents;
                        }
                        sprintf(the_text, "%s \xf8%d\xfa/\xf8%d",
                            GetMiscString(eMiscString_wasted),
                            count_oppos - NumberOfOpponentsLeft(),
                            count_oppos);
                    } else {
                        if (Flash(150, &gWasted_last_flash, &gWasted_flash_state)) {
                            sprintf(the_text, "\xf9%s %s",
                                gOpponents[gQueued_wasted_messages[0]].abbrev_name,
                                GetMiscString(eMiscString_wasted));
                        } else {
                            sprintf(the_text, " ");
                        }
                    }
                    ChangeHeadupText(gCar_kill_count_headup, the_text);
                }

                TimerString(effective_timer, the_text, 1, 0, 0);
                ChangeHeadupText(gTimer_headup, the_text);

                if (gHeadup_detail_level == 1 || gHeadup_detail_level == 4) {
                    sprintf(gHeadup_oppo_ped_text, "%i  %i  %i",
                        GetCarCount(eVehicle_opponent) - NumberOfOpponentsLeft(),
                        net_credits,
                        gCount_killed_peds);
                }
                if (gCurrent_race.race_spec->race_type == kRaceType_Carma1
                        || gCurrent_race.race_spec->race_type == kRaceType_Checkpoints) {

                    sprintf(the_text, "%s \xf8%d\xfa/\xf8%d \xfa%s \xf8%d\xfa/\xf8%d",
                        GetMiscString(eMiscString_cp), gCheckpoint, gCheckpoint_count,
                        GetMiscString(eMiscString_lap), gLap,gTotal_laps);
                    ChangeHeadupText(gLaps_headup, the_text);
                }
                if (gCurrent_race.race_spec->race_type > 3) {
 /* kRaceType_Smash or kRaceType_SmashNPed */
                    sprintf(the_text, "%s \xf8%d\xfa/\xf8%d",
                        GetMiscString(eMiscString_targets),
                        GetRuntimeVariable(gCurrent_race.race_spec->options.smash.var_smash_number),
                        gCurrent_race.race_spec->options.smash.var_smash_target);
                    ChangeHeadupText(gLaps_headup, the_text);
                }

                the_time = GetTotalTime() - gTime_bonus_state;

                switch (gTime_bonus_state) {
                case eTime_bonus_initial_pause:
                    if (the_time >= 500) {
                        bonus = gCompletion_bonus_post_race;
                        sprintf(the_text, "%s %d", GetMiscString(eMiscString_completion_bonus_colon), bonus);
                        DRS3StartSound(gPedestrians_outlet, eSoundId_Clap);
                        ChangeHeadupText(gRace_bonus_headup, the_text);
                        gProgram_state.credits += bonus;
                        gTime_bonus_state = eTime_bonus_race_bonus;
                        gTime_bonus_start = GetTotalTime();
                    }
                    gRace_finished = 10000;
                    break;
                case eTime_bonus_race_bonus:
                    if (the_time >= 2000) {
                        gTime_bonus_state = eTime_bonus_tb_up;
                        gTime_bonus_start = GetTotalTime();
                        last_rattle_time = 0;
                    }
                    gRace_finished = 10000;
                    break;
                case eTime_bonus_tb_up:
                    if (gTimer != 0) {
                        if (the_time - last_rattle_time > 15) {
                            int previous_gtimer;

                            previous_gtimer = gTimer;
                            gTimer -= 1000 * ((the_time - last_rattle_time) / 15);
                            if (gTimer < 0) {
                                gTimer = 0;
                            }
                            gTime_bonus += (previous_gtimer - gTimer) / 1000 * gCredits_per_second_time_bonus[gProgram_state.skill_level];
                            last_rattle_time += 15 * ((the_time - last_rattle_time) / 15);
                        }
                        sprintf(the_text, "%s %d", GetMiscString(eMiscString_time_bonus_colon), gTime_bonus);
                        ChangeHeadupText(gTime_bonus_headup, the_text);
                    } else {
                        gTime_bonus_state = eTime_bonus_tb_pause;
                        gTime_bonus_start = GetTotalTime();
                        last_rattle_time = 0;
                    }
                    gRace_finished = 10000;
                    break;
                case eTime_bonus_tb_pause:
                    if (the_time >= 1000) {
                        gTime_bonus_state = eTime_bonus_tb_down;
                        gTime_bonus_start = GetTotalTime();
                        last_rattle_time = 0;
                    }
                    gRace_finished = 10000;
                    break;
                case eTime_bonus_tb_down:
                    if (gTime_bonus != 0) {
                        if (the_time - last_rattle_time > 15) {
                            bonus = gTime_bonus;
                            gTime_bonus -= (the_time - last_rattle_time) * gCredits_per_second_time_bonus[gProgram_state.skill_level] / 15;
                            if (gTime_bonus < 0) {
                                gTime_bonus = 0;
                            }
                            gProgram_state.credits += bonus - gTime_bonus;
                            last_rattle_time += 15 * ((the_time - last_rattle_time) / 15);
                        }
                        sprintf(the_text, "%s %d", GetMiscString(eMiscString_time_bonus_colon), gTime_bonus);
                        ChangeHeadupText(gTime_bonus_headup, the_text);
                    } else {
                        gTime_bonus_state = eTime_bonus_end_pause;
                        gTime_bonus_start = GetTotalTime();
                    }
                    gRace_finished = 10000;
                    break;
                case eTime_bonus_end_pause:
                    if (the_time >= 2000 && gRace_finished > 1) {
                        gRace_finished = 1;
                    }
                    break;
                default:
                    break;
                }
            } else {
                DoNetworkHeadups(net_credits);
            }
        }
            ProcessOilSpills(gFrame_period);
            MungeSmashEdgeTriggers(GetTotalTime());
        }
        MungeDelayedSideEffects();
        MungeShrapnel(gFrame_period);
        if (!gPed_676914) {
            MungePowerupStuff(gFrame_period);
        }
        ChangeDepthEffect();
        ServiceGameInRace();
        EnterUserMessage();
        SkidsPerFrame();
        FlushSmashQueue(0);
        MungeGlassFragments();
        AnimateSky();
        MungeExplosions();
        LastChanceForPedEffects();
        DoLODCarModels();
        MungeLightingEffects();

        if (gWait_for_it != 0) {
            if (GetTotalTime() >= gWait_for_it) {
                gWait_for_it = 0;
            }
        }
        if (gWait_for_it == 0) {
            Timers_Force(TIMER_RQQ);
            RenderAFrame(1);
            Timers_Force(TIMER_OQQ);
        }
        DoComplexCarModels();
        CheckReplayTurnOn();
        if (!gRecover_car
                && gProgram_state.prog_status == eProg_game_ongoing
                && gPalette_fade_time == 0
                && (gNet_mode == eNet_mode_none
                        || !gPed_676914
                        || gProgram_state.current_car.car_master_actor->t.t.translate.t.v[0] < 500.f)) {

            EnsureRenderPalette();
            EnsurePaletteUp();
        }

        if (KeyIsDown(0) && !gEntering_message) {

            WaitForNoKeys();
            if (!gPed_676914) {
                tU32 start_menu_time;

                if (gMap_view == 2) {
                    ToggleMap();
                }
                start_menu_time = PDGetTotalTime();
                FadePaletteDown();
                ClearEntireScreen();
                GoingToInterfaceFromRace();
                DoOptionsMenu();
                GoingBackToRaceFromInterface();
                AddLostTime(PDGetTotalTime() - start_menu_time);
            } else {
                ToggleReplay(NULL, NULL);
            }
        }
        if (!gPed_676914) {
            CheckTimer();
        } else {
            PollActionReplayControls(&gFrame_period, &gAverage_frame_period);
        }
        if (!gPed_676914 && gKnobbled_frame_period != 0) {
            while (GetTotalTime() - frame_start_time < gKnobbled_frame_period) {
                /* brr */
                /* FIXME: replace with udelay */
            }
        }
        if (!tried_to_allocate_AR) {
            tried_to_allocate_AR = 1;
            PrintMemoryDump(0, "JUST RENDERED 1ST STUFF");
            InitialiseActionReplay();
            PrintMemoryDump(0, "JUST ALLOCATED ACTION REPLAY BUFFER");
        }
        if (gNet_mode == eNet_mode_client && gAbandon_game) {
            gProgram_state.prog_status = eProg_idling;
            gAbandon_game = 0;
        }
        Timers_EndFrame();

        if (gProgram_state.prog_status != eProg_game_ongoing) {
            break;
        }
        if (MungeRaceFinished()) {
            break;
        }
        if (gAbandon_game || gHost_abandon_game) {
            break;
        }
    }
    if (gCredits_post_race >= 0) {
        gProgram_state.credits = gCredits_post_race + gTotal_bonus_post_race;
    }
    PHILDisable();
    PrintMemoryDump(0, "JUST EXITED MAINLOOP");
    FadePaletteDown();
    ClearEntireScreen();
    SuspendPendingFlic();
    RevertPalette();
    if (gMap_view == 2) {
        ToggleMap();
    }
    EnsureSpecialVolumesHidden();
    FadePaletteDown();
    NetPlayerStatusChanged(ePlayer_status_loading);
    if (gAdditional_actors != NULL && gAdditional_actors->children != NULL) {
        AutoSaveAdditionalStuff();
    }
    if (gProgram_state.prog_status == eProg_game_ongoing) {
        ResetCarScreens();
    }
    if (gAbandon_game && gNet_mode == eNet_mode_host && gRace_over_reason < 8 ) {
 /* FIXME: 8 == eRace_over_count */
        NetFinishRace(gCurrent_net_game, eRace_over_abandoned);
    }
    if (gPed_676914) {
        ToggleReplay(NULL, NULL);
    }
    if (gHost_abandon_game || gProgram_state.prog_status != eProg_game_ongoing) {
        result = eRace_game_abandonned;
    } else if (gAbandon_game == 1) {
        result = eRace_aborted;
    } else if (gRace_over_reason == eRace_over_out_of_time || gRace_over_reason == eRace_over_6 || gRace_over_reason == eRace_over_7) {
        result = eRace_timed_out;
    } else {
        result = eRace_completed;
    }
    if (result < eRace_completed) {
        gProgram_state.redo_race_index = gProgram_state.current_race_index;
    } else {
        gProgram_state.redo_race_index = -1;
    }
    if (gNet_mode != eNet_mode_none
            && result == eRace_game_abandonned
            && !gAbandon_game
            && gProgram_state.prog_status != eProg_idling) {

        gProgram_state.prog_status = eProg_game_starting;
    }
    gAbandon_game = 0;
    gStart_race_sent = 0;
    gInitialised_grid = 0;
    gHost_abandon_game = 0;
    DRS3StopAllOutletSoundsExceptCDA();
    if (gNet_mode != eNet_mode_none) {
        int i;

        for (i = 0; i < gNumber_of_net_players; i++) {
            StopCarBeingIt(gNet_players[i].car);
        }
    }
    gProgram_state.racing = 0;
    if (gNet_mode == eNet_mode_host) {
        gCurrent_net_game->status.stage = eNet_game_stage0;
    }
    WaitForNoKeys();
    return result;
#else
    __asm {
            sub esp, 0x128
            mov eax, dword ptr [gTimer]
            push ebx
            push ebp
            push esi
            push edi
            xor edi, edi
            or esi, 0xffffffff
            mov dword ptr [esp + 0x14], edi
            mov dword ptr [gCar_to_view], 0x75bc2c
            mov dword ptr [gCar_to_view_id], edi
            mov dword ptr [gOld_camera_time], edi
            mov dword ptr [gLast_net_auto_increase_credits], edi
            mov dword ptr [gLast_credit_headup__mainloop], edi
            mov dword ptr [gLast_time_headup], eax
            mov dword ptr [gRace_bonus_headup], esi
            mov dword ptr [gTime_bonus_headup], esi
            mov dword ptr [gTime_bonus_start], edi
            mov dword ptr [gTime_bonus_state], edi
            call ARMainLoopStart
            call PDGetTotalTime
            mov dword ptr [gLast_tick_count], eax
            mov dword ptr [gActual_last_tick_count], eax
            mov dword ptr [gFrame_period], 0x64
            mov dword ptr [gQueued_wasted_messages_count], edi
            mov dword ptr [gWasted_flash_state], edi
            mov dword ptr [gWasted_last_flash], edi
            call RevertPalette
            mov eax, dword ptr [gNet_mode]
            mov dword ptr [gHost_abandon_game], edi
            cmp eax, edi
            mov dword ptr [gNo_races_yet], edi
            mov dword ptr [gRace_over_reason], esi
            mov dword ptr [gFLOAT_0068c778], edi
            mov dword ptr [gNo_credits_APO_restore], edi
            mov dword ptr [gReceived_game_scores], edi
            mov ebx, 1
            jne L00492a44
            mov dword ptr [gProgram_state+0x2c], ebx
    L00492a44:
            mov dword ptr [gCredits_post_race], esi
            call ShowSpecialVolumesIfRequ
            mov ecx, dword ptr [gNet_mode]
            mov eax, 2
            cmp ecx, eax
            mov dword ptr [gProgram_state+0x28], ebx
            jne L00492a6d
            mov ecx, dword ptr [gCurrent_net_game]
            mov dword ptr [ecx + 0x40], eax
    L00492a6d:
            mov ecx, 6
            call NetPlayerStatusChanged
            mov ecx, offset gCurrent_race
            call GetAverageGridPosition
            call ForceRebuildActiveCarList
            mov edx, offset gMatchFloat_00658998
            xor ecx, ecx
            call PrintMemoryDump
            mov ecx, offset gCamera_period
            call UpdateFramePeriod
            mov ecx, dword ptr [gCamera_period]
            mov dword ptr [gPrev_camera_position], 0x447a0000
            mov dword ptr [gPrev_camera_position+0x4], 0x447a0000
            mov dword ptr [gPrev_camera_position+0x8], 0x447a0000
            call CalculateCameraStuff
            _emit 0xff
        _emit 0x15
        _emit 0xa0
        _emit 0x90
        _emit 0x58
        _emit 0x00
            mov ebx, eax
            push 0x74
            push ebx
            _emit 0xff
        _emit 0x15
        _emit 0x78
        _emit 0x90
        _emit 0x58
        _emit 0x00
            test eax, eax
            jne L00492b58
            cmp word ptr [ebx + 0x72], 0x4948
            jne L00492b58
            cmp ebx, edi
            je L00492b58
            mov ecx, dword ptr [gPage_size]
    L00492aec:
            lea esi, [ebx + 0x74]
    L00492aef:
            mov eax, dword ptr [esi]
            mov ebp, eax
            and ebp, 0x5ffffffc
            je L00492b51
            test al, 1
            jne L00492b4b
            test ecx, ecx
            lea edi, [ebp - 4]
            jne L00492b1d
            push offset gSystem_info
            _emit 0xff
        _emit 0x15
        _emit 0x50
        _emit 0x91
        _emit 0x58
        _emit 0x00
            mov ecx, dword ptr [gSystem_info]
            mov dword ptr [gPage_size], ecx
    L00492b1d:
            mov eax, ecx
            shl eax, 4
            cmp eax, edi
            jae L00492b4b
    L00492b26:
            shl ecx, 4
            mov edx, eax
            sub edx, ecx
            lea ecx, [edx + esi + 0x10]
            mov dl, byte ptr [edx + esi + 0x10]
            mov byte ptr [ecx], dl
            mov cl, byte ptr [eax + esi + 0x10]
            mov byte ptr [eax + esi + 0x10], cl
            mov ecx, dword ptr [gPage_size]
            add eax, ecx
            cmp eax, edi
            jb L00492b26
    L00492b4b:
            add esi, ebp
            xor edi, edi
            jmp L00492aef
    L00492b51:
            mov ebx, dword ptr [ebx + 0x48]
            cmp ebx, edi
            jne L00492aec
    L00492b58:
            push 0x5dc
            _emit 0xff
        _emit 0x15
        _emit 0x94
        _emit 0x90
        _emit 0x58
        _emit 0x00
            call PDGetTotalTime
            mov dword ptr [gRace_start], eax
            call Timers_Init
            cmp dword ptr [gWait_for_it], edi
            je L00492b85
            call GetTotalTime
            add dword ptr [gWait_for_it], eax
    L00492b85:
            call ClearEntireScreen
            call ResetPalette
    L00492b8f:
            call Timers_StartFrame
            call EdgeTriggerModeOff
            mov eax, dword ptr [gPed_676914]
            test eax, eax
            jne L00492bac
            mov ecx, offset gCamera_period
            call UpdateFramePeriod
    L00492bac:
            call GetTotalTime
            mov dword ptr [esp + 0x18], eax
            call CyclePollKeys
            call InitSmashQueue
            mov ecx, 1
            call CheckSystemKeys
            call InitialiseExtraRenders
            call NetReceiveAndProcessMessages
            mov eax, dword ptr [gHost_abandon_game]
            mov esi, 2
            test eax, eax
            jne L00493f04
            cmp dword ptr [gProgram_state+0xa4], esi
            je L00493f04
            mov eax, dword ptr [gNet_mode]
            test eax, eax
            je L00492c22
            mov edx, dword ptr [gCurrent_net_game]
            cmp dword ptr [edx + 0x74], 7
            jne L00492c22
            mov eax, dword ptr [gThis_net_player_index]
            mov ecx, dword ptr [gIt_or_fox]
            cmp eax, ecx
            jne L00492c22
            cmp dword ptr [gMap_view], esi
            jne L00492c22
            call ToggleMap
    L00492c22:
            call ResetGrooveFlags
            call ServiceGameInRace
            call EnterUserMessage
            call ARService
            call ResetLollipopQueue
            call MungePalette
            call PDIsWindowInactive
            mov esi, eax
            test esi, esi
            je L00492c8a
            call PDGetTotalTime
            mov ecx, dword ptr [gPrev_task_switched_message]
            add ecx, 0x3e8
            cmp eax, ecx
            jbe L00492ca2
            call GetTotalTime
            mov ecx, 0x8b
            mov dword ptr [gPrev_task_switched_message], eax
            call GetMiscString
            xor edx, edx
            mov ecx, 4
            push eax
            push -4
            push 0x1f4
            call NewTextHeadupSlot
            jmp L00492ca2
    L00492c8a:
            mov eax, dword ptr [gWindow_inactive]
            test eax, eax
            je L00492ca2
            mov ecx, 4
            call ClearHeadupSlot
            call ClearQueuedHeadups
    L00492ca2:
            mov eax, dword ptr [gPed_676914]
            mov dword ptr [gWindow_inactive], esi
            test eax, eax
            jne L0049305b
            mov ecx, dword ptr [gFrame_period]
            call MungeAIWorld
            mov ecx, dword ptr [gFrame_period]
            call PollCarControls
            mov eax, dword ptr [gPed_676914]
            test eax, eax
            jne L0049305b
            mov ebp, dword ptr [gNum_active_cars]
            xor ecx, ecx
            xor esi, esi
            test ebp, ebp
            jle L00492d11
            mov edi, offset gActive_car_list
            mov ebx, 5
            mov edx, edi
    L00492cf2:
            mov eax, dword ptr [edx]
            test eax, eax
            je L00492d07
            cmp dword ptr [eax + 0xc], ebx
            jle L00492d07
            cmp esi, ecx
            je L00492d03
            mov dword ptr [edi], eax
    L00492d03:
            inc esi
            add edi, 4
    L00492d07:
            inc ecx
            add edx, 4
            cmp ecx, ebp
            jl L00492cf2
            jmp L00492d16
    L00492d11:
            mov ebx, 5
    L00492d16:
            mov eax, dword ptr [gTimers_stack_size]
            mov dword ptr [gNum_active_cars], esi
            test eax, eax
            mov dword ptr [eax*4 + gTimers_stack], ebx
            je L00492d50
            mov eax, dword ptr [eax*4 + gMatchData_006b75dc]
            lea esi, [eax*8]
            lea ecx, [eax*8]
            sub esi, eax
            shl esi, 2
            xor edx, edx
            mov dl, byte ptr [esi + gTimers+0x8]
            jmp L00492d6c
    L00492d50:
            mov eax, dword ptr [gTimers_max_index]
            xor edx, edx
            mov esi, eax
            mov ecx, eax
            shl esi, 3
            sub esi, eax
            shl esi, 2
            shl ecx, 3
            mov dl, byte ptr [esi + gTimers+0x8]
    L00492d6c:
            sub ecx, eax
            add edx, ecx
            lea edi, [edx*4 + gTimers+0x4]
            call PDGetMicroseconds
            mov edx, dword ptr [esi + gTimers+0xc]
            mov ecx, dword ptr [edi]
            sub eax, edx
            add ecx, eax
            mov dword ptr [edi], ecx
            call PDGetMicroseconds
            mov edi, dword ptr [gTimers_stack_size]
            mov ecx, dword ptr [gFrame_period]
            inc edi
            mov dword ptr [gTimers+0x98], eax
            mov dword ptr [gTimers_stack_size], edi
            call MungePedestrians
            mov edx, dword ptr [gTimers+0x94]
            and edx, 0xff
            lea esi, [edx*4 + gTimers+0x90]
            call PDGetMicroseconds
            mov edx, dword ptr [gTimers+0x98]
            mov ecx, dword ptr [esi]
            sub eax, edx
            add ecx, eax
            mov eax, dword ptr [gTimers_stack_size]
            cmp eax, 1
            mov dword ptr [esi], ecx
            jle L00492df0
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            mov ecx, dword ptr [ecx*4 + gMatchData_006b75dc]
            jmp L00492dfb
    L00492df0:
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_max_index]
    L00492dfb:
            mov edx, ecx
            shl edx, 3
            sub edx, ecx
            mov ecx, dword ptr [gFrame_period]
            mov dword ptr [edx*4 + gTimers+0xc], eax
            mov edx, dword ptr [gTimers_stack_size]
            dec edx
            mov dword ptr [gTimers_stack_size], edx
            call ControlOurCar
            mov eax, dword ptr [gTimers_stack_size]
            xor ebx, ebx
            cmp eax, ebx
            mov dword ptr [eax*4 + gTimers_stack], 4
            je L00492e5b
            mov eax, dword ptr [eax*4 + gMatchData_006b75dc]
            lea esi, [eax*8]
            lea edx, [eax*8]
            sub esi, eax
            shl esi, 2
            xor ecx, ecx
            mov cl, byte ptr [esi + gTimers+0x8]
            jmp L00492e77
    L00492e5b:
            mov eax, dword ptr [gTimers_max_index]
            xor ecx, ecx
            mov esi, eax
            mov edx, eax
            shl esi, 3
            sub esi, eax
            shl esi, 2
            shl edx, 3
            mov cl, byte ptr [esi + gTimers+0x8]
    L00492e77:
            sub edx, eax
            add ecx, edx
            lea edi, [ecx*4 + gTimers+0x4]
            call PDGetMicroseconds
            mov edx, dword ptr [esi + gTimers+0xc]
            mov ecx, dword ptr [edi]
            sub eax, edx
            add ecx, eax
            mov dword ptr [edi], ecx
            call PDGetMicroseconds
            mov ebp, dword ptr [gTimers_stack_size]
            mov ecx, dword ptr [gLast_tick_count]
            mov edi, dword ptr [gRace_start]
            mov edx, dword ptr [gFrame_period]
            inc ebp
            sub ecx, edi
            mov dword ptr [gTimers+0x7c], eax
            mov dword ptr [gTimers_stack_size], ebp
            call ApplyPhysicsToCars
            mov eax, dword ptr [gTimers+0x78]
            and eax, 0xff
            lea esi, [eax*4 + gTimers+0x74]
            call PDGetMicroseconds
            mov edx, dword ptr [gTimers+0x7c]
            mov ecx, dword ptr [esi]
            sub eax, edx
            mov ebp, 1
            add ecx, eax
            mov eax, dword ptr [gTimers_stack_size]
            cmp eax, ebp
            mov dword ptr [esi], ecx
            jle L00492f0b
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            mov ecx, dword ptr [ecx*4 + gMatchData_006b75dc]
            jmp L00492f16
    L00492f0b:
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_max_index]
    L00492f16:
            mov edx, ecx
            shl edx, 3
            sub edx, ecx
            mov ecx, dword ptr [gFrame_period]
            mov dword ptr [edx*4 + gTimers+0xc], eax
            mov edx, dword ptr [gTimers_stack_size]
            dec edx
            mov dword ptr [gTimers_stack_size], edx
            call MungeSomeOtherCarGraphics
            call PipeCarPositions
            mov eax, dword ptr [gTimers_stack_size]
            cmp eax, ebx
            mov dword ptr [eax*4 + gTimers_stack], 8
            je L00492f79
            mov eax, dword ptr [eax*4 + gMatchData_006b75dc]
            lea esi, [eax*8]
            lea edx, [eax*8]
            sub esi, eax
            shl esi, 2
            xor ecx, ecx
            mov cl, byte ptr [esi + gTimers+0x8]
            jmp L00492f95
    L00492f79:
            mov eax, dword ptr [gTimers_max_index]
            xor ecx, ecx
            mov esi, eax
            mov edx, eax
            shl esi, 3
            sub esi, eax
            shl esi, 2
            shl edx, 3
            mov cl, byte ptr [esi + gTimers+0x8]
    L00492f95:
            sub edx, eax
            add ecx, edx
            lea edi, [ecx*4 + gTimers+0x4]
            call PDGetMicroseconds
            mov edx, dword ptr [esi + gTimers+0xc]
            mov ecx, dword ptr [edi]
            sub eax, edx
            add ecx, eax
            mov dword ptr [edi], ecx
            call PDGetMicroseconds
            mov edi, dword ptr [gTimers_stack_size]
            mov dword ptr [gTimers+0xec], eax
            inc edi
            mov dword ptr [gTimers_stack_size], edi
            call CrushBendFlapRend
            mov eax, dword ptr [gTimers+0xe8]
            and eax, 0xff
            lea esi, [eax*4 + gTimers+0xe4]
            call PDGetMicroseconds
            mov edx, dword ptr [gTimers+0xec]
            mov ecx, dword ptr [esi]
            sub eax, edx
            add ecx, eax
            mov eax, dword ptr [gTimers_stack_size]
            cmp eax, ebp
            mov dword ptr [esi], ecx
            jle L00493010
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            mov ecx, dword ptr [ecx*4 + gMatchData_006b75dc]
            jmp L0049301b
    L00493010:
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_max_index]
    L0049301b:
            mov edx, ecx
            shl edx, 3
            sub edx, ecx
            mov ecx, dword ptr [gTimers_stack_size]
            dec ecx
            mov dword ptr [edx*4 + gTimers+0xc], eax
            mov dword ptr [gTimers_stack_size], ecx
            call DoNetGameManagement
            mov eax, dword ptr [gLast_tick_count]
            mov ecx, dword ptr [gFrame_period]
            add ecx, eax
            mov eax, dword ptr [gRace_start]
            sub ecx, eax
            call CheckRecoveryOfCars
            call NetSendMessageStacks
            jmp L0049306d
    L0049305b:
            mov ecx, dword ptr [gFrame_period]
            call DoActionReplay
            xor ebx, ebx
            mov ebp, 1
    L0049306d:
            mov ecx, dword ptr [gCamera_period]
            call PollCameraControls
            call MungeEngineNoise
            mov ecx, dword ptr [gCamera_period]
            call CalculateCameraStuff
            call MungeNapalm
            cmp dword ptr [gPed_676914], ebx
            jne L004930b2
            mov ecx, dword ptr [gFrame_period]
            call DoPowerupPeriodics
            cmp dword ptr [gPed_676914], ebx
            jne L004930b2
            call CheckCheckpoints
            call MungeEnvironmentalSound
    L004930b2:
            call ChangingView
            mov ecx, dword ptr [gFrame_period]
            call MungeCarGraphics
            call FunkThoseTronics
            call GrooveThoseDelics
            mov ecx, dword ptr [gFrame_period]
            call DoWheelDamage
            call PDGetTotalTime
            mov edi, eax
            _emit 0xa1
        _emit 0x34
        _emit 0xc7
        _emit 0x68
        _emit 0x00
            cmp edi, eax
            je L00493144
            mov ecx, edi
            xor edx, edx
            sub ecx, eax
            mov eax, 0x2710
            div ecx
            mov esi, eax
            mov eax, 0x68c790
            mov ecx, esi
    L004930fd:
            mov edx, dword ptr [eax]
            add eax, 4
            add ecx, edx
            cmp eax, offset gTime_bonus_headup
            jl L004930fd
            mov eax, 0x84210843
            imul ecx
            add edx, ecx
            sar edx, 4
            mov eax, edx
            shr eax, 0x1f
            add edx, eax
            mov eax, 0x68c790
            mov dword ptr [gFrame_rate], edx
    L00493129:
            mov ecx, dword ptr [eax + 4]
            mov dword ptr [eax], ecx
            add eax, 4
            cmp eax, offset gMatchData_0068c804
            jl L00493129
            mov dword ptr [gMatchData_0068c804], esi
            _emit 0x89
        _emit 0x3d
        _emit 0x34
        _emit 0xc7
        _emit 0x68
        _emit 0x00
    L00493144:
            mov edx, dword ptr [gCamera_period]
            mov ecx, offset gProgram_state+0xac
            call CameraBugFix
            cmp dword ptr [gPed_676914], ebx
            jne L00493a06
            mov ecx, 3
            call ClearHeadupSlot
            call MungeJoystickHeadups
            mov edx, dword ptr [gFrame_period]
            mov ebp, dword ptr [gProgram_state]
            mov dword ptr [esp + 0x30], edx
            mov dword ptr [esp + 0x34], ebx
            fild qword ptr [esp + 0x30]
            mov dword ptr [esp + 0x10], ebp
            fild dword ptr [esp + 0x10]
            fisub dword ptr [gLast_credit_headup__mainloop]
            fabs
            fdiv st(0), st(1)
            fcomp qword ptr [gMatchDouble_0058a2e8]
            fnstsw ax
            test ah, 0x41
            jne L00493210
            mov ecx, dword ptr [gLast_credit_headup__mainloop]
            mov eax, ebp
            sub eax, ecx
            cmp eax, ebx
            mov dword ptr [esp + 0x10], eax
            jle L004931e1
            fild dword ptr [gLast_credit_headup__mainloop]
            fild dword ptr [esp + 0x10]
            fsub qword ptr [gMatchDouble_0058a2f0]
            fmul st(0), st(2)
            fmul qword ptr [gMatchDouble_0058a2f8]
            fsubp st(1), st(0)
            call _ftol
            mov ebp, eax
            fstp st(0)
            mov dword ptr [esp + 0x10], ebp
            jmp L00493212
    L004931e1:
            fild dword ptr [gLast_credit_headup__mainloop]
            sub ecx, ebp
            mov dword ptr [esp + 0x10], ecx
            fild dword ptr [esp + 0x10]
            fsub qword ptr [gMatchDouble_0058a2f0]
            fmul st(0), st(2)
            fmul qword ptr [gMatchDouble_0058a300]
            fsubp st(1), st(0)
            call _ftol
            mov ebp, eax
            fstp st(0)
            mov dword ptr [esp + 0x10], ebp
            jmp L00493212
    L00493210:
            fstp st(0)
    L00493212:
            cmp dword ptr [gNet_mode], ebx
            je L00493340
            cmp dword ptr [gCountdown], ebx
            jne L00493340
            mov eax, dword ptr [gCurrent_net_game]
            mov eax, dword ptr [eax + 0x74]
            shl eax, 2
            cmp dword ptr [eax + gNet_auto_increase_credits_dt], ebx
            je L00493340
            fild dword ptr [esp + 0x10]
            fst dword ptr [esp + 0x1c]
            fcomp dword ptr [eax + gRecovery_cost+0xc]
            fnstsw ax
            test ah, 1
            je L00493340
            call PDGetTotalTime
            mov edx, dword ptr [gLast_net_auto_increase_credits]
            mov ecx, dword ptr [gCurrent_net_game]
            sub eax, edx
            mov edx, dword ptr [ecx + 0x74]
            cmp eax, dword ptr [edx*4 + gNet_auto_increase_credits_dt]
            jb L00493340
            call PDGetTotalTime
            mov ecx, dword ptr [gProgram_state]
            mov esi, dword ptr [gProgram_state+0x20]
            mov edi, dword ptr [gCurrent_net_game]
            mov dword ptr [gLast_net_auto_increase_credits], eax
            mov eax, dword ptr [gNet_mode]
            add ecx, 0x32
            cmp eax, ebx
            mov dword ptr [gProgram_state], ecx
            je L004932b7
            mov eax, dword ptr [edi + 0x74]
            fld dword ptr [eax*4 + gRecovery_cost+0xc]
            jmp L004932be
    L004932b7:
            fld dword ptr [esi*4 + gRecovery_cost]
    L004932be:
            lea ecx, [ebp + 0x32]
            mov dword ptr [esp + 0x10], ecx
            fild dword ptr [esp + 0x10]
            fcomp st(1)
            fnstsw ax
            test ah, 0x41
            fstp st(0)
            jne L0049333e
            cmp dword ptr [gNet_mode], ebx
            je L004932e8
            mov ecx, dword ptr [edi + 0x74]
            fld dword ptr [ecx*4 + gRecovery_cost+0xc]
            jmp L004932ef
    L004932e8:
            fld dword ptr [esi*4 + gRecovery_cost]
    L004932ef:
            fld dword ptr [esp + 0x1c]
            fsub st(0), st(1)
            fild dword ptr [gProgram_state]
            fxch st(1)
            fsub dword ptr [gMatchFloat_0058a308]
            fsubp st(1), st(0)
            call _ftol
            mov ecx, dword ptr [gNet_mode]
            mov dword ptr [gProgram_state], eax
            cmp ecx, ebx
            fstp st(0)
            je L0049332e
            mov edx, dword ptr [edi + 0x74]
            fld dword ptr [edx*4 + gRecovery_cost+0xc]
            call _ftol
            mov ebp, eax
            jmp L00493340
    L0049332e:
            fld dword ptr [esi*4 + gRecovery_cost]
            call _ftol
            mov ebp, eax
            jmp L00493340
    L0049333e:
            mov ebp, ecx
    L00493340:
            mov eax, dword ptr [gCountdown]
            mov dword ptr [gLast_credit_headup__mainloop], ebp
            cmp eax, ebx
            je L004933c1
            call GetRaceTime
            mov dword ptr [esp + 0x20], eax
            mov dword ptr [esp + 0x24], ebx
            fild qword ptr [esp + 0x20]
            fmul qword ptr [gMatchDouble_0058a310]
            fsubr qword ptr [gMatchDouble_0058a318]
            call _ftol
            mov esi, eax
            cmp esi, ebx
            jge L00493379
            xor esi, esi
    L00493379:
            cmp esi, dword ptr [gCountdown]
            je L004933c1
            cmp esi, 5
            jg L004933c1
            lea eax, [esi + 4]
            xor edx, edx
            push eax
            push 0x320
            mov ecx, 5
            mov dword ptr [gCountdown], esi
            call NewImageHeadupSlot
            mov ecx, dword ptr [gCountdown]
            lea edx, [ecx + 0x1f40]
            mov ecx, dword ptr [gPedestrians_outlet]
            call DRS3StartSound
            cmp esi, ebx
            jne L004933c1
            call MakeFlagWavingBastardWaveHisFlagWhichIsTheProbablyTheLastThingHeWillEverDo
    L004933c1:
            fild dword ptr [gTimer]
            mov edx, dword ptr [gFrame_period]
            mov dword ptr [esp + 0x2c], ebx
            fild dword ptr [gLast_time_headup]
            mov dword ptr [esp + 0x28], edx
            fild qword ptr [esp + 0x28]
            fxch st(1)
            fsubp st(2), st(0)
            fxch st(1)
            fabs
            fdiv st(0), st(1)
            fcomp qword ptr [gMatchDouble_0058a320]
            fnstsw ax
            test ah, 0x41
            jne L00493428
            mov eax, dword ptr [gTimer]
            mov esi, dword ptr [gLast_time_headup]
            mov ecx, eax
            sub ecx, esi
            test ecx, ecx
            jle L00493422
            fmul qword ptr [gMatchDouble_0058a328]
            fild dword ptr [gLast_time_headup]
            fxch st(1)
            fsubp st(1), st(0)
            call _ftol
            mov edi, eax
            jmp L00493430
    L00493422:
            fstp st(0)
            mov edi, eax
            jmp L00493430
    L00493428:
            mov edi, dword ptr [gTimer]
            fstp st(0)
    L00493430:
            mov eax, dword ptr [gNet_mode]
            mov dword ptr [gLast_time_headup], edi
            cmp eax, ebx
            je L0049344b
            mov ecx, ebp
            call DoNetworkHeadups
            jmp L004939ea
    L0049344b:
            cmp ebp, ebx
            jl L00493474
            xor ecx, ecx
            cmp ebp, 0x186a0
            setl cl
            dec ecx
            and ecx, 0x1f
            add ecx, 0x10
            call GetMiscString
            lea edx, [esp + 0x38]
            push eax
            push ebp
            push offset gMatchData_00658990
            push edx
            jmp L0049348e
    L00493474:
            mov ecx, 0x11
            call GetMiscString
            lea ecx, [esp + 0x38]
            push eax
            mov eax, ebp
            neg eax
            push eax
            push offset gMatchData_00658990
            push ecx
    L0049348e:
            call sprintf
            mov ecx, dword ptr [gCredits_won_headup]
            add esp, 0x10
            lea edx, [esp + 0x38]
            call ChangeHeadupText
            mov edx, dword ptr [gCurrent_race+0x98]
            mov eax, dword ptr [edx + 0x210]
            cmp eax, ebx
            jne L004934c7
            mov ecx, 0x12
            call GetMiscString
            push eax
            mov eax, dword ptr [gPed_count]
            jmp L004934dc
    L004934c7:
            cmp eax, 2
            jne L00493505
            mov ecx, 0x12
            call GetMiscString
            push eax
            mov eax, dword ptr [gTotal_count_smash_peds]
    L004934dc:
            mov ecx, dword ptr [gCount_killed_peds]
            push eax
            push ecx
            lea edx, [esp + 0x44]
            push offset gMatchData_00658980
            push edx
            call sprintf
            mov ecx, dword ptr [gPed_kill_count_headup]
            add esp, 0x14
            lea edx, [esp + 0x38]
            call ChangeHeadupText
    L00493505:
            mov eax, dword ptr [gCurrent_race+0x98]
            mov eax, dword ptr [eax + 0x210]
            cmp eax, ebx
            je L0049351d
            cmp eax, 1
            jne L00493630
    L0049351d:
            cmp dword ptr [gQueued_wasted_messages_count], ebx
            je L004935d9
            call GetTotalTime
            mov ecx, dword ptr [gLast_wasted_message_start]
            add ecx, 0x1388
            cmp eax, ecx
            jbe L0049356d
            mov edx, dword ptr [gQueued_wasted_messages_count]
            cmp edx, 1
            jle L0049355c
            mov eax, offset gQueued_wasted_messages+0x4
            lea ecx, [edx - 1]
    L00493551:
            mov esi, dword ptr [eax]
            mov dword ptr [eax - 4], esi
            add eax, 4
            dec ecx
            jne L00493551
    L0049355c:
            dec edx
            mov dword ptr [gQueued_wasted_messages_count], edx
            call GetTotalTime
            mov dword ptr [gLast_wasted_message_start], eax
    L0049356d:
            cmp dword ptr [gQueued_wasted_messages_count], ebx
            je L004935d9
            push offset gWasted_flash_state
            mov edx, offset gWasted_last_flash
            mov ecx, 0x96
            call Flash
            test eax, eax
            je L004935c5
            mov ecx, 0x2e
            call GetMiscString
            push eax
            mov eax, dword ptr [gQueued_wasted_messages]
            mov edx, eax
            shl edx, 3
            sub edx, eax
            mov eax, dword ptr [gOpponents]
            shl edx, 6
            lea ecx, [edx + eax + 0x18]
            lea edx, [esp + 0x3c]
            push ecx
            push offset gMatchFloat_00658978
            push edx
            call sprintf
            add esp, 0x10
            jmp L00493621
    L004935c5:
            lea eax, [esp + 0x38]
            push 0x58f640
            push eax
            call sprintf
            add esp, 8
            jmp L00493621
    L004935d9:
            mov ecx, dword ptr [gCurrent_race+0x98]
            cmp dword ptr [ecx + 0x210], ebx
            jne L004935f5
            mov ecx, 2
            call GetCarCount
            mov esi, eax
            jmp L004935fb
    L004935f5:
            mov esi, dword ptr [gCount_opponents]
    L004935fb:
            push esi
            call NumberOfOpponentsLeft
            sub esi, eax
            mov ecx, 0x13
            push esi
            call GetMiscString
            lea edx, [esp + 0x40]
            push eax
            push offset gMatchData_0065896c
            push edx
            call sprintf
            add esp, 0x14
    L00493621:
            mov ecx, dword ptr [gCar_kill_count_headup]
            lea edx, [esp + 0x38]
            call ChangeHeadupText
    L00493630:
            push ebx
            mov esi, 1
            push ebx
            push esi
            lea edx, [esp + 0x44]
            mov ecx, edi
            call TimerString
            mov ecx, dword ptr [gTimer_headup]
            lea edx, [esp + 0x38]
            call ChangeHeadupText
            mov eax, dword ptr [gHeadup_detail_level]
            cmp eax, esi
            je L00493660
            cmp eax, 4
            jne L0049368d
    L00493660:
            mov eax, dword ptr [gCount_killed_peds]
            push eax
            push ebp
            call NumberOfOpponentsLeft
            mov ecx, 2
            mov edi, eax
            call GetCarCount
            sub eax, edi
            push eax
            push offset gMatchString_00658960
            push offset gHeadup_oppo_ped_text
            call sprintf
            add esp, 0x14
    L0049368d:
            mov eax, dword ptr [gCurrent_race+0x98]
            mov ecx, dword ptr [eax + 0x210]
            cmp ecx, ebx
            je L004936a1
            cmp ecx, 3
            jne L004936f8
    L004936a1:
            mov ecx, dword ptr [gTotal_laps]
            mov edx, dword ptr [gLap]
            push ecx
            push edx
            mov ecx, 0x14
            call GetMiscString
            mov ecx, dword ptr [gCheckpoint]
            push eax
            mov eax, dword ptr [gCheckpoint_count]
            push eax
            push ecx
            mov ecx, 0x15
            call GetMiscString
            lea edx, [esp + 0x4c]
            push eax
            push offset gMatchData_00658944
            push edx
            call sprintf
            mov ecx, dword ptr [gLaps_headup]
            add esp, 0x20
            lea edx, [esp + 0x38]
            call ChangeHeadupText
            mov eax, dword ptr [gCurrent_race+0x98]
    L004936f8:
            cmp dword ptr [eax + 0x210], 4
            jl L00493740
            mov ecx, dword ptr [eax + 0x218]
            push ecx
            mov ecx, dword ptr [eax + 0x214]
            call GetRuntimeVariable
            mov ecx, 0x114
            push eax
            call GetMiscString
            lea edx, [esp + 0x40]
            push eax
            push offset gMatchData_0065896c
            push edx
            call sprintf
            mov ecx, dword ptr [gLaps_headup]
            add esp, 0x14
            lea edx, [esp + 0x38]
            call ChangeHeadupText
    L00493740:
            call GetTotalTime
            mov edi, dword ptr [gTime_bonus_start]
            mov ecx, dword ptr [gTime_bonus_state]
            sub eax, edi
            cmp ecx, 6
            ja L004939ea
            jmp dword ptr [ecx*4 + 0x4940bc]
            cmp eax, 0x1f4
            jb L004939c9
            mov eax, dword ptr [gCompletion_bonus_post_race]
            mov ecx, 0x16
            push eax
            call GetMiscString
            lea ecx, [esp + 0x3c]
            push eax
            push offset gMatchString_0058fd6c
            push ecx
            call sprintf
            mov ecx, dword ptr [gPedestrians_outlet]
            add esp, 0x10
            mov edx, 0x1f4f
            call DRS3StartSound
            mov ecx, dword ptr [gRace_bonus_headup]
            lea edx, [esp + 0x38]
            call ChangeHeadupText
            mov edx, dword ptr [gCompletion_bonus_post_race]
            mov eax, dword ptr [gProgram_state]
            add eax, edx
            mov dword ptr [gTime_bonus_state], 2
            mov dword ptr [gProgram_state], eax
            call GetTotalTime
            mov dword ptr [gTime_bonus_start], eax
            mov dword ptr [gRace_finished], 0x2710
            jmp L004939ea
            cmp eax, 0x7d0
            jb L004939c9
            mov dword ptr [gTime_bonus_state], 3
            jmp L004938ea
            mov esi, dword ptr [gTimer]
            cmp esi, ebx
            je L004938c9
            _emit 0x8b
        _emit 0x3d
        _emit 0x18
        _emit 0xc8
        _emit 0x68
        _emit 0x00
            mov ecx, eax
            sub ecx, edi
            cmp ecx, 0xf
            jbe L00493888
            mov eax, 0x88888889
            mul ecx
            mov ecx, edx
            shr ecx, 3
            mov eax, ecx
            neg eax
            shl eax, 2
            sub eax, ecx
            lea eax, [eax + eax*4]
            lea eax, [eax + eax*4]
            lea eax, [esi + eax*8]
            cmp eax, ebx
            mov dword ptr [gTimer], eax
            jge L0049384a
            xor eax, eax
            mov dword ptr [gTimer], eax
    L0049384a:
            sub esi, eax
            mov eax, 0x10624dd3
            imul esi
            sar edx, 6
            mov eax, edx
            mov esi, dword ptr [gTime_bonus]
            shr eax, 0x1f
            add edx, eax
            mov eax, dword ptr [gProgram_state+0x20]
            imul edx, dword ptr [eax*4 + gCredits_per_second_time_bonus]
            lea eax, [ecx + ecx*2]
            add esi, edx
            mov ecx, eax
            mov dword ptr [gTime_bonus], esi
            add ecx, edi
            lea edx, [ecx + eax*4]
            _emit 0x89
        _emit 0x15
        _emit 0x18
        _emit 0xc8
        _emit 0x68
        _emit 0x00
    L00493888:
            mov eax, dword ptr [gTime_bonus]
            mov ecx, 0x17
            push eax
            call GetMiscString
            lea ecx, [esp + 0x3c]
            push eax
            push offset gMatchString_0058fd6c
            push ecx
            call sprintf
            mov ecx, dword ptr [gTime_bonus_headup]
            add esp, 0x10
            lea edx, [esp + 0x38]
            call ChangeHeadupText
            mov dword ptr [gRace_finished], 0x2710
            jmp L004939ea
    L004938c9:
            mov dword ptr [gTime_bonus_state], 4
            jmp L004938ea
            cmp eax, 0x3e8
            jb L004939c9
            mov dword ptr [gTime_bonus_state], 5
    L004938ea:
            call GetTotalTime
            mov dword ptr [gTime_bonus_start], eax
            _emit 0x89
        _emit 0x1d
        _emit 0x18
        _emit 0xc8
        _emit 0x68
        _emit 0x00
            mov dword ptr [gRace_finished], 0x2710
            jmp L004939ea
            mov edi, dword ptr [gTime_bonus]
            cmp edi, ebx
            je L004939b5
            _emit 0x8b
        _emit 0x2d
        _emit 0x18
        _emit 0xc8
        _emit 0x68
        _emit 0x00
            mov esi, eax
            sub esi, ebp
            cmp esi, 0xf
            jbe L0049397c
            mov edx, dword ptr [gProgram_state+0x20]
            mov eax, 0x88888889
            mov ecx, edi
            mov edx, dword ptr [edx*4 + gCredits_per_second_time_bonus]
            imul edx, esi
            mul edx
            shr edx, 3
            sub edi, edx
            mov dword ptr [gTime_bonus], edi
            jns L00493954
            xor edi, edi
            mov dword ptr [gTime_bonus], edi
    L00493954:
            mov eax, dword ptr [gProgram_state]
            sub ecx, edi
            add eax, ecx
            mov dword ptr [gProgram_state], eax
            mov eax, 0x88888889
            mul esi
            shr edx, 3
            lea eax, [edx + edx*2]
            mov ecx, eax
            add ecx, ebp
            lea edx, [ecx + eax*4]
            _emit 0x89
        _emit 0x15
        _emit 0x18
        _emit 0xc8
        _emit 0x68
        _emit 0x00
    L0049397c:
            push edi
            mov ecx, 0x17
            call GetMiscString
            push eax
            lea eax, [esp + 0x40]
            push offset gMatchString_0058fd6c
            push eax
            call sprintf
            mov ecx, dword ptr [gTime_bonus_headup]
            add esp, 0x10
            lea edx, [esp + 0x38]
            call ChangeHeadupText
            mov dword ptr [gRace_finished], 0x2710
            jmp L004939ea
    L004939b5:
            mov dword ptr [gTime_bonus_state], 6
            call GetTotalTime
            mov dword ptr [gTime_bonus_start], eax
    L004939c9:
            mov dword ptr [gRace_finished], 0x2710
            jmp L004939ea
            cmp eax, 0x7d0
            jb L004939ea
            cmp dword ptr [gRace_finished], esi
            jbe L004939ea
            mov dword ptr [gRace_finished], esi
    L004939ea:
            mov ecx, dword ptr [gFrame_period]
            call ProcessOilSpills
            call GetTotalTime
            mov ecx, eax
            call MungeSmashEdgeTriggers
            mov ebp, 1
    L00493a06:
            call MungeDelayedSideEffects
            mov ecx, dword ptr [gFrame_period]
            call MungeShrapnel
            cmp dword ptr [gPed_676914], ebx
            jne L00493a29
            mov ecx, dword ptr [gFrame_period]
            call MungePowerupStuff
    L00493a29:
            call ChangeDepthEffect
            call ServiceGameInRace
            call EnterUserMessage
            call SkidsPerFrame
            xor ecx, ecx
            call FlushSmashQueue
            call MungeGlassFragments
            call AnimateSky
            call MungeExplosions
            call LastChanceForPedEffects
            call DoLODCarModels
            call MungeLightingEffects
            mov ecx, dword ptr [gWait_for_it]
            cmp ecx, ebx
            je L00493a8b
            call GetTotalTime
            mov ecx, dword ptr [gWait_for_it]
            cmp eax, ecx
            jb L00493a83
            xor ecx, ecx
            mov dword ptr [gWait_for_it], ecx
    L00493a83:
            cmp ecx, ebx
            jne L00493b4c
    L00493a8b:
            mov eax, dword ptr [gTimers_max_index]
            xor ecx, ecx
            mov esi, eax
            mov edx, eax
            shl esi, 3
            sub esi, eax
            shl esi, 2
            shl edx, 3
            mov cl, byte ptr [esi + gTimers+0x8]
            sub edx, eax
            add ecx, edx
            lea edi, [ecx*4 + gTimers+0x4]
            call PDGetMicroseconds
            mov edx, dword ptr [esi + gTimers+0xc]
            mov ecx, dword ptr [edi]
            sub eax, edx
            mov dword ptr [gTimers_max_index], 3
            add ecx, eax
            mov dword ptr [edi], ecx
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_max_index]
            mov edx, ecx
            shl edx, 3
            sub edx, ecx
            mov ecx, ebp
            mov dword ptr [edx*4 + gTimers+0xc], eax
            call RenderAFrame
            mov eax, dword ptr [gTimers_max_index]
            xor ecx, ecx
            mov esi, eax
            mov edx, eax
            shl esi, 3
            sub esi, eax
            shl esi, 2
            shl edx, 3
            mov cl, byte ptr [esi + gTimers+0x8]
            sub edx, eax
            add ecx, edx
            lea edi, [ecx*4 + gTimers+0x4]
            call PDGetMicroseconds
            mov edx, dword ptr [esi + gTimers+0xc]
            mov ecx, dword ptr [edi]
            sub eax, edx
            mov dword ptr [gTimers_max_index], 0x12
            add ecx, eax
            mov dword ptr [edi], ecx
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_max_index]
            mov edx, ecx
            shl edx, 3
            sub edx, ecx
            mov dword ptr [edx*4 + gTimers+0xc], eax
    L00493b4c:
            call DoComplexCarModels
            call CheckReplayTurnOn
            cmp dword ptr [gRecover_car], ebx
            jne L00493b9e
            cmp dword ptr [gProgram_state+0xa4], 5
            jne L00493b9e
            cmp dword ptr [gPalette_fade_time], ebx
            jne L00493b9e
            cmp dword ptr [gNet_mode], ebx
            je L00493b94
            cmp dword ptr [gPed_676914], ebx
            je L00493b94
            mov eax, dword ptr [gProgram_state+0xbc]
            fld dword ptr [eax + 0x50]
            fcomp dword ptr [gMatchFloat_0058a330]
            fnstsw ax
            test ah, 1
            je L00493b9e
    L00493b94:
            call EnsureRenderPalette
            call EnsurePaletteUp
    L00493b9e:
            xor ecx, ecx
            call KeyIsDown
            test eax, eax
            je L00493c05
            cmp dword ptr [gEntering_message], ebx
            jne L00493c05
            call WaitForNoKeys
            cmp dword ptr [gPed_676914], ebx
            je L00493bc9
            xor edx, edx
            xor ecx, ecx
            call ToggleReplay
            jmp L00493c05
    L00493bc9:
            cmp dword ptr [gMap_view], 2
            jne L00493bd7
            call ToggleMap
    L00493bd7:
            call PDGetTotalTime
            mov esi, eax
            call FadePaletteDown
            call ClearEntireScreen
            call GoingToInterfaceFromRace
            call DoOptionsMenu
            call GoingBackToRaceFromInterface
            call PDGetTotalTime
            mov ecx, eax
            sub ecx, esi
            call AddLostTime
    L00493c05:
            cmp dword ptr [gPed_676914], ebx
            jne L00493ca0
            cmp dword ptr [gFreeze_timer], ebx
            jne L00493caf
            cmp dword ptr [gCountdown], ebx
            jne L00493caf
            cmp dword ptr [gRace_finished], ebx
            jne L00493caf
            mov eax, dword ptr [gTimer]
            mov ecx, dword ptr [gFrame_period]
            cmp eax, ecx
            ja L00493c52
            mov ecx, 5
            mov dword ptr [gTimer], ebx
            call RaceCompleted
            jmp L00493caf
    L00493c52:
            cmp dword ptr [gNet_mode], ebx
            jne L00493c61
            sub eax, ecx
            mov dword ptr [gTimer], eax
    L00493c61:
            lea ecx, [eax + 0x1f4]
            mov eax, 0x10624dd3
            imul ecx
            sar edx, 6
            _emit 0xa1
        _emit 0x28
        _emit 0xc8
        _emit 0x68
        _emit 0x00
            mov ecx, edx
            shr ecx, 0x1f
            add edx, ecx
            mov esi, edx
            cmp esi, eax
            je L00493c98
            cmp esi, 0xa
            jg L00493c98
            mov ecx, dword ptr [gPedestrians_outlet]
            mov edx, 0x3e9
            call DRS3StartSound
    L00493c98:
            _emit 0x89
        _emit 0x35
        _emit 0x28
        _emit 0xc8
        _emit 0x68
        _emit 0x00
            jmp L00493caf
    L00493ca0:
            mov edx, offset gAverage_frame_period
            mov ecx, offset gFrame_period
            call PollActionReplayControls
    L00493caf:
            cmp dword ptr [gPed_676914], ebx
            jne L00493ce5
            cmp dword ptr [gKnobbled_frame_period], ebx
            je L00493ce5
            call GetTotalTime
            mov esi, dword ptr [esp + 0x18]
            mov ecx, dword ptr [gKnobbled_frame_period]
            sub eax, esi
            cmp eax, ecx
            jae L00493ce5
    L00493cd4:
            call GetTotalTime
            mov ecx, dword ptr [gKnobbled_frame_period]
            sub eax, esi
            cmp eax, ecx
            jb L00493cd4
    L00493ce5:
            cmp dword ptr [esp + 0x14], ebx
            jne L00493d0c
            mov edx, offset gMatchFloat_0065892c
            xor ecx, ecx
            mov dword ptr [esp + 0x14], ebp
            call PrintMemoryDump
            call InitialiseActionReplay
            mov edx, offset gMatchFloat_00658908
            xor ecx, ecx
            call PrintMemoryDump
    L00493d0c:
            cmp dword ptr [gNet_mode], 3
            jne L00493d2d
            cmp dword ptr [gAbandon_game], ebx
            je L00493d2d
            mov dword ptr [gProgram_state+0xa4], 2
            mov dword ptr [gAbandon_game], ebx
    L00493d2d:
            call Timers_EndFrame
            cmp dword ptr [gProgram_state+0xa4], 5
            jne L00493eff
            cmp dword ptr [gRace_finished], ebx
            je L00493ee5
            cmp dword ptr [gPed_676914], ebx
            jne L00493ee5
            mov eax, dword ptr [gNet_mode]
            mov ecx, dword ptr [gRace_over_reason]
            cmp eax, ebx
            je L00493d6f
            cmp ecx, -1
            je L00493ee5
    L00493d6f:
            cmp eax, ebx
            jne L00493e1e
            cmp ecx, 3
            jg L00493e1e
            cmp dword ptr [gCredits_post_race], ebx
            jge L00493e1e
            mov edx, dword ptr [gProgram_state]
            mov eax, dword ptr [gCurrent_race+0x98]
            mov dword ptr [gCredits_post_race], edx
            mov eax, dword ptr [eax + 0x210]
            neg eax
            sbb eax, eax
            not eax
            and eax, ecx
            mov ecx, dword ptr [gProgram_state+0x20]
            lea edx, [ecx + eax*2]
            mov ecx, dword ptr [ecx*4 + gCredits_per_second_time_bonus]
            add eax, edx
            mov edx, dword ptr [gTimer]
            mov esi, dword ptr [eax*4 + gCurrent_race+0x18]
            mov eax, 0x10624dd3
            imul edx
            sar edx, 6
            mov eax, edx
            mov dword ptr [gCompletion_bonus_post_race], esi
            shr eax, 0x1f
            add edx, eax
            imul ecx, edx
            add ecx, esi
            mov dword ptr [gTotal_bonus_post_race], ecx
            mov ecx, 0x63
            call GetRuntimeVariable
            test eax, eax
            jne L00493e18
            mov ecx, 0x122
            call GetMiscString
            xor edx, edx
            mov ecx, 4
            push eax
            push -4
            push 0xbb8
            call NewTextHeadupSlot
    L00493e18:
            mov ecx, dword ptr [gRace_over_reason]
    L00493e1e:
            mov eax, dword ptr [gFrame_period]
            mov edx, dword ptr [gRace_finished]
            cmp edx, eax
            ja L00493ebf
            cmp dword ptr [gTimer], ebx
            je L00493eb5
            cmp dword ptr [gNet_mode], ebx
            jne L00493eb5
            cmp ecx, 6
            je L00493eb5
            mov ecx, 0x63
            call GetRuntimeVariable
            test eax, eax
            jne L00493eb5
            mov edx, dword ptr [gTimer]
            push 0x676920
            push -4
            push ebx
            lea eax, [edx + 0x12c]
            xor edx, edx
            mov ecx, 9
            lea eax, [eax + eax*2]
            lea eax, [eax + eax*4]
            mov dword ptr [gRace_finished], eax
            call NewTextHeadupSlot
            xor edx, edx
            mov ecx, 0xa
            push 0x676920
            push -4
            push ebx
            mov dword ptr [gRace_bonus_headup], eax
            call NewTextHeadupSlot
            mov dword ptr [gTime_bonus_headup], eax
            mov dword ptr [gTime_bonus], ebx
            call GetTotalTime
            mov dword ptr [gTime_bonus_start], eax
            mov dword ptr [gTime_bonus_state], ebp
            jmp L00493ec5
    L00493eb5:
            mov dword ptr [gRace_finished], ebx
            mov eax, ebp
            jmp L00493ee7
    L00493ebf:
            sub dword ptr [gRace_finished], eax
    L00493ec5:
            mov ecx, 0x33
            call PDKeyDown
            test eax, eax
            jne L00493ee1
            mov ecx, 0x34
            call PDKeyDown
            test eax, eax
            je L00493ee5
    L00493ee1:
            mov eax, ebp
            jmp L00493ee7
    L00493ee5:
            xor eax, eax
    L00493ee7:
            cmp eax, ebx
            jne L00493eff
            cmp dword ptr [gAbandon_game], ebx
            jne L00493eff
            cmp dword ptr [gHost_abandon_game], ebx
            je L00492b8f
    L00493eff:
            mov esi, 2
    L00493f04:
            mov eax, dword ptr [gCredits_post_race]
            xor edi, edi
            cmp eax, edi
            jl L00493f1d
            mov ecx, dword ptr [gTotal_bonus_post_race]
            add ecx, eax
            mov dword ptr [gProgram_state], ecx
    L00493f1d:
            call PHILDisable
            mov edx, offset gMatchFloat_006588f0
            xor ecx, ecx
            call PrintMemoryDump
            call FadePaletteDown
            call ClearEntireScreen
            call SuspendPendingFlic
            call RevertPalette
            cmp dword ptr [gMap_view], esi
            jne L00493f4f
            call ToggleMap
    L00493f4f:
            call EnsureSpecialVolumesHidden
            call FadePaletteDown
            mov ecx, esi
            call NetPlayerStatusChanged
            mov eax, dword ptr [gAdditional_actors]
            cmp eax, edi
            je L00493f73
            cmp dword ptr [eax + 8], edi
            je L00493f73
            call AutoSaveAdditionalStuff
    L00493f73:
            mov eax, dword ptr [gProgram_state+0xa4]
            mov ebx, 5
            cmp eax, ebx
            jne L00493f86
            call ResetCarScreens
    L00493f86:
            cmp dword ptr [gAbandon_game], edi
            je L00493faf
            cmp dword ptr [gNet_mode], esi
            jne L00493faf
            cmp dword ptr [gRace_over_reason], 8
            jge L00493faf
            mov ecx, dword ptr [gCurrent_net_game]
            mov edx, 4
            call NetFinishRace
    L00493faf:
            cmp dword ptr [gPed_676914], edi
            je L00493fc0
            xor edx, edx
            xor ecx, ecx
            call ToggleReplay
    L00493fc0:
            cmp dword ptr [gHost_abandon_game], edi
            jne L00493ffc
            cmp dword ptr [gProgram_state+0xa4], ebx
            jne L00493ffc
            mov eax, dword ptr [gAbandon_game]
            mov ebp, 1
            cmp eax, ebp
            je L00493ffe
            mov eax, dword ptr [gRace_over_reason]
            cmp eax, ebx
            je L00493ff8
            cmp eax, 6
            je L00493ff8
            cmp eax, 7
            je L00493ff8
            mov ebp, 3
            jmp L00493ffe
    L00493ff8:
            mov ebp, esi
            jmp L00493ffe
    L00493ffc:
            xor ebp, ebp
    L00493ffe:
            cmp ebp, 3
            jge L00494011
            mov edx, dword ptr [gProgram_state+0x78]
            mov dword ptr [gProgram_state+0x7c], edx
            jmp L0049401b
    L00494011:
            mov dword ptr [gProgram_state+0x7c], 0xffffffff
    L0049401b:
            cmp dword ptr [gNet_mode], edi
            je L00494041
            cmp ebp, edi
            jne L00494041
            cmp dword ptr [gAbandon_game], edi
            jne L00494041
            cmp dword ptr [gProgram_state+0xa4], esi
            je L00494041
            mov dword ptr [gProgram_state+0xa4], 4
    L00494041:
            mov dword ptr [gAbandon_game], edi
            mov dword ptr [gStart_race_sent], edi
            mov dword ptr [gInitialised_grid], edi
            mov dword ptr [gHost_abandon_game], edi
            call DRS3StopAllOutletSoundsExceptCDA
            cmp dword ptr [gNet_mode], edi
            je L0049408d
            mov eax, dword ptr [gNumber_of_net_players]
            xor esi, esi
            test eax, eax
            jle L0049408d
            mov edi, offset gNet_players+0xc8
    L00494076:
            mov ecx, dword ptr [edi]
            call StopCarBeingIt
            mov eax, dword ptr [gNumber_of_net_players]
            inc esi
            add edi, 0xd4
            cmp esi, eax
            jl L00494076
    L0049408d:
            mov ecx, dword ptr [gNet_mode]
            xor eax, eax
            cmp ecx, 2
            mov dword ptr [gProgram_state+0x28], eax
            jne L004940a8
            mov ecx, dword ptr [gCurrent_net_game]
            mov dword ptr [ecx + 0x40], eax
    L004940a8:
            call WaitForNoKeys
            mov eax, ebp
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x128
            ret
            mov edi, edi
            _emit 0xea
            _emit 0x39
            _emit 0x49
            _emit 0x00
            _emit 0x63
            _emit 0x37
            _emit 0x49
            _emit 0x00
            _emit 0xe5
            _emit 0x37
            _emit 0x49
            _emit 0x00
            _emit 0xff
            _emit 0x37
            _emit 0x49
            _emit 0x00
            _emit 0xd5
            _emit 0x38
            _emit 0x49
            _emit 0x00
            _emit 0x09
            _emit 0x39
            _emit 0x49
            _emit 0x00
            _emit 0xd5
            _emit 0x39
            _emit 0x49
            _emit 0x00
            _emit 0x90
            _emit 0x90
            _emit 0x90
            _emit 0x90
            _emit 0x90
            _emit 0x90
            _emit 0x90
            _emit 0x90
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00492830
tU32 C2_HOOK_FASTCALL GetLastTickCount(void) {
    return gLast_tick_count;
}

// FUNCTION: CARMA2_HW 0x00492950
tRace_result C2_HOOK_FASTCALL DoRace(void) {
    tRace_result result;

    if (PercentageChance(50)) {
        StartMusicTrack(9998);
    } else {
        StartMusicTrack(9997);
    }
    gRace_start = PDGetTotalTime();
    result = MainGameLoop();
    return result;
}

// FUNCTION: CARMA2_HW 0x00492680
void C2_NAKED C2_HOOK_FASTCALL UpdateFramePeriod(tU32* pCamera_period) {
#ifndef CARPOCALYPSE2_MATCHING
    tU32 new_tick_count;
    tU32 new_camera_tick_count;
    int error;

    if (gPed_676914) {
        tU32 last_tick_count;
        tU32 camera_period;

        last_tick_count = gLast_tick_count;
        gFrame_period = abs((int)(gLast_replay_frame_time - last_tick_count));
        gLast_tick_count = gLast_replay_frame_time;
        new_camera_tick_count = PDGetTotalTime();
        new_tick_count = GetTotalTime();
        if (gOld_camera_time != 0) {
            camera_period = new_camera_tick_count - gOld_camera_time;
        } else {
            camera_period = 0;
        }
        *pCamera_period = camera_period;
        gOld_camera_time = new_camera_tick_count;
        if (gFrame_period != 0) {
            *pCamera_period = gFrame_period;
        }
    } else {
        int new_period;

        new_tick_count = PDGetTotalTime();
        gLast_tick_count += gFrame_period;
        error = new_tick_count - gLast_tick_count;
        gFrame_period = new_tick_count - gActual_last_tick_count;
        if (gFrame_period > 500 && new_tick_count - gRace_start > 2000) {
            gFrame_period = gAverage_frame_period / 10;
            gLast_tick_count = new_tick_count;
            if (gNet_mode != eNet_mode_none) {
                switch (gNet_mode) {
                case eNet_mode_client:
                    gProgram_state.current_car.collision_info->field_0x49c = 0;
                    break;
                case eNet_mode_host:
                    break;
                default:
                    break;
                }
            }
        }
        gAverage_frame_period = 9 * gAverage_frame_period / 10 + gFrame_period;
        if (new_tick_count - gRace_start > 2000) {
            gFrame_period = gAverage_frame_period / 10;
        }
        new_period = gFrame_period + error;
        if (new_period <= 0) {
            gLast_tick_count = new_tick_count;
        } else {
            gFrame_period = new_period;
        }
        *pCamera_period = gFrame_period;
        gActual_last_tick_count = new_tick_count;
    }
    if (gFrame_period < 10) {
        gFrame_period = 10;
        gLast_tick_count = new_tick_count;
    } else if (gFrame_period > 1000) {
        gFrame_period = 1000;
        gLast_tick_count = new_tick_count;
    }
    if (*pCamera_period < 10) {
        *pCamera_period = 10;
    } else if (*pCamera_period > 1000) {
        *pCamera_period = 1000;
    }
#else
    __asm {
            mov eax, dword ptr [gPed_676914]
            push ebx
            push ebp
            push esi
            test eax, eax
            push edi
            mov ebx, ecx
            je L004926eb
            mov ecx, dword ptr [gLast_replay_frame_time]
            mov edx, dword ptr [gLast_tick_count]
            mov eax, ecx
            mov dword ptr [gLast_tick_count], ecx
            sub eax, edx
            cdq
            xor eax, edx
            sub eax, edx
            mov dword ptr [gFrame_period], eax
            call PDGetTotalTime
            mov edi, eax
            call GetTotalTime
            mov ecx, dword ptr [gOld_camera_time]
            mov esi, eax
            test ecx, ecx
            je L004926cd
            mov eax, edi
            sub eax, ecx
            jmp L004926cf
    L004926cd:
            xor eax, eax
    L004926cf:
            mov dword ptr [ebx], eax
            mov eax, dword ptr [gFrame_period]
            test eax, eax
            mov dword ptr [gOld_camera_time], edi
            je L004927d5
            mov dword ptr [ebx], eax
            jmp L004927d0
    L004926eb:
            call PDGetTotalTime
            mov edx, dword ptr [gFrame_period]
            mov esi, eax
            mov eax, dword ptr [gLast_tick_count]
            mov ecx, esi
            add eax, edx
            mov edx, dword ptr [gActual_last_tick_count]
            mov edi, esi
            sub ecx, edx
            sub edi, eax
            cmp ecx, 0x1f4
            mov dword ptr [gLast_tick_count], eax
            mov dword ptr [gFrame_period], ecx
            jbe L00492774
            mov edx, dword ptr [gRace_start]
            mov eax, esi
            sub eax, edx
            cmp eax, 0x7d0
            jbe L00492774
            mov eax, 0xcccccccd
            mov dword ptr [gLast_tick_count], esi
            mul dword ptr [gAverage_frame_period]
            mov eax, dword ptr [gNet_mode]
            mov ecx, edx
            shr ecx, 3
            test eax, eax
            mov dword ptr [gFrame_period], ecx
            je L00492774
            sub eax, 2
            je L00492774
            dec eax
            jne L00492774
            mov ecx, dword ptr [gProgram_state+0xb4]
            mov dword ptr [ecx + 0x49c], 0
            mov ecx, dword ptr [gFrame_period]
    L00492774:
            mov eax, dword ptr [gAverage_frame_period]
            mov ebp, dword ptr [gRace_start]
            lea edx, [eax + eax*8]
            mov eax, 0xcccccccd
            mul edx
            mov eax, esi
            shr edx, 3
            sub eax, ebp
            add edx, ecx
            cmp eax, 0x7d0
            mov dword ptr [gAverage_frame_period], edx
            jbe L004927b1
            mov eax, 0xcccccccd
            mul edx
            mov ecx, edx
            shr ecx, 3
            mov dword ptr [gFrame_period], ecx
    L004927b1:
            lea eax, [edi + ecx]
            test eax, eax
            jg L004927c0
            mov dword ptr [gLast_tick_count], esi
            jmp L004927c8
    L004927c0:
            mov ecx, eax
            mov dword ptr [gFrame_period], ecx
    L004927c8:
            mov dword ptr [ebx], ecx
            mov dword ptr [gActual_last_tick_count], esi
    L004927d0:
            mov eax, dword ptr [gFrame_period]
    L004927d5:
            cmp eax, 0xa
            jae L004927e6
            mov dword ptr [gFrame_period], 0xa
            jmp L004927f7
    L004927e6:
            cmp eax, 0x3e8
            jbe L004927fd
            mov dword ptr [gFrame_period], 0x3e8
    L004927f7:
            mov dword ptr [gLast_tick_count], esi
    L004927fd:
            mov eax, dword ptr [ebx]
            cmp eax, 0xa
            jae L0049280f
            mov dword ptr [ebx], 0xa
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret
    L0049280f:
            cmp eax, 0x3e8
            jbe L0049281c
            mov dword ptr [ebx], 0x3e8
    L0049281c:
            pop edi
            pop esi
            pop ebp
            pop ebx
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
            nop
    }
#endif
}


void C2_HOOK_FASTCALL CheckTimer(void) {
    tS32 time_in_seconds;
    tS32 time_left;
    // GLOBAL: CARMA2_HW 0x0068c828
    static tU32 last_time_in_seconds;

    if (!gFreeze_timer && !gCountdown && !gRace_finished) {
        if (gFrame_period < gTimer) {
            if (gNet_mode == eNet_mode_none) {
                gTimer -= gFrame_period;
            }
            time_left = gTimer + 500;
            time_in_seconds = time_left / 1000;
            if (time_in_seconds != last_time_in_seconds && time_in_seconds <= 10) {
                DRS3StartSound(gIndexed_outlets[0], eSoundId_warning_buzzer_noise);
            }
            last_time_in_seconds = time_in_seconds;
        } else {
            gTimer = 0;
            RaceCompleted(eRace_over_out_of_time);
        }
    }
}

int C2_HOOK_FASTCALL MungeRaceFinished(void) {

    if (!gRace_finished || gAction_replay_mode || (gNet_mode != eNet_mode_none && gRace_over_reason == eRace_not_over_yet)) {
        return 0;
    }
    if (gNet_mode == eNet_mode_none && gRace_over_reason < 4 && gCredits_post_race < 0) {
        int completion_bonus;

        gCredits_post_race = gProgram_state.credits;
        if (gCurrent_race.race_spec->race_type == kRaceType_Carma1) {
            completion_bonus = gCurrent_race.completion_bonus[gProgram_state.skill_level];
        } else {
            /* FIXME: is it int completion_bonus[4][3] instead?*/
            switch (gRace_over_reason) {
            case eRace_over_0:
                completion_bonus = gCurrent_race.completion_bonus[gProgram_state.skill_level];
                break;
            case eRace_over_1:
                completion_bonus = gCurrent_race.completion_bonus_peds[gProgram_state.skill_level];
                break;
            case eRace_over_2:
                completion_bonus = gCurrent_race.completion_bonus_opponents[gProgram_state.skill_level];
                break;
            case eRace_over_3:
                /* FIXME: what completion_bonus type is this? */
                completion_bonus = gCurrent_race.completion_bonus_field_0x3c[gProgram_state.skill_level];
                break;
            default:
                CARPOCALYPSE2_UNREACHABLE();
                break;
            }
        }
        gCompletion_bonus_post_race = completion_bonus;
        gTotal_bonus_post_race = gCredits_per_second_time_bonus[gProgram_state.skill_level] * (gTimer / 1000)
                + gCompletion_bonus_post_race;
        if (!GetRuntimeVariable(99)) {
            NewTextHeadupSlot(4, 0, 3000, -4, GetMiscString(eMiscString_game_auto_saved));
        }
    }

    if (gRace_finished > gFrame_period) {
        gRace_finished -= gFrame_period;
    } else {
        if (gTimer == 0 || gNet_mode != eNet_mode_none || gRace_over_reason == eRace_over_6 || GetRuntimeVariable(99) != 0) {
            gRace_finished = 0;
            return 1;
        }
        gRace_finished = 15 * (gTimer + 300);
        gRace_bonus_headup = NewTextHeadupSlot(9, 0, 0, -4, "");
        gTime_bonus_headup = NewTextHeadupSlot(10, 0, 0, -4, "");
        gTime_bonus = 0;
        gTime_bonus_start = GetTotalTime();
        gTime_bonus_state = eTime_bonus_initial_pause;
    }
    return PDKeyDown(51) || PDKeyDown(52);
}

void C2_HOOK_FASTCALL LoseOldestWastedMessage(void) {
    int i;

    for (i = 1; i < gQueued_wasted_messages_count; i++) {
        gQueued_wasted_messages[i - 1] = gQueued_wasted_messages[i];
    }
    gQueued_wasted_messages_count -= 1;
    gLast_wasted_message_start = GetTotalTime();
}
// CalculateFrameRate

// LoseOldestWastedMassage

// FUNCTION: CARMA2_HW 0x00492620
void C2_NAKED C2_HOOK_FASTCALL QueueWastedMessage(int pIndex) {
#ifndef CARPOCALYPSE2_MATCHING

    if (gQueued_wasted_messages_count == CARPOCALYPSE2_ASIZE(gQueued_wasted_messages)) {
        int i;

        for (i = 1; i < gQueued_wasted_messages_count; i++) {
            gQueued_wasted_messages[i - 1] = gQueued_wasted_messages[i];
        }
        gQueued_wasted_messages_count = CARPOCALYPSE2_ASIZE(gQueued_wasted_messages) - 1;
        gLast_wasted_message_start = GetTotalTime();
    }
    if (gQueued_wasted_messages_count == 0) {
        gLast_wasted_message_start = GetTotalTime();
    }
    gQueued_wasted_messages[gQueued_wasted_messages_count] = pIndex;
    gQueued_wasted_messages_count++;
#else
    __asm {
            mov eax, dword ptr [gQueued_wasted_messages_count]
            push esi
            cmp eax, 5
            mov esi, ecx
            jne L00492655
            mov eax, offset gQueued_wasted_messages+0x4
    L00492632:
            mov ecx, dword ptr [eax]
            mov dword ptr [eax - 4], ecx
            add eax, 4
            cmp eax, offset gCompletion_bonus_post_race
            jl L00492632
            mov dword ptr [gQueued_wasted_messages_count], 4
            call GetTotalTime
            mov dword ptr [gLast_wasted_message_start], eax
    L00492655:
            mov eax, dword ptr [gQueued_wasted_messages_count]
            test eax, eax
            jne L00492668
            call GetTotalTime
            mov dword ptr [gLast_wasted_message_start], eax
    L00492668:
            mov eax, dword ptr [gQueued_wasted_messages_count]
            mov dword ptr [eax*4 + gQueued_wasted_messages], esi
            inc eax
            mov dword ptr [gQueued_wasted_messages_count], eax
            pop esi
            ret
            nop
            nop
            nop
            nop
    }
#endif
}

// MungeHeadups

// UpdateFramePeriod

// CheckTimer

// MungeRaceFinished

// CalculateCameraStuff

// CheckActiveCarList

// MainGameLoop

// DoRace