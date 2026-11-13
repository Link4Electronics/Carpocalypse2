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
// FUNCTION: CARMA2_HW 0x00401170
void C2_HOOK_FASTCALL MungeAIWorld(tU32 pFrame_period) {

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
            fcomp qword ptr [gDouble_0058a068+0x2d0]
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
tRace_result C2_HOOK_FASTCALL MainGameLoop(void) {
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