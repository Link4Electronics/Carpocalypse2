#include "structur.h"

#include "car.h"
#include "controls.h"
#include "crush.h"
#include "cutscene.h"
#include "displays.h"
#include "drmem.h"
#include "errors.h"
#include "finteray.h"
#include "flicplay.h"
#include "frontend.h"
#include "globvars.h"
#include "globvrkm.h"
#include "globvrpb.h"
#include "graphics.h"
#include "init.h"
#include "loading.h"
#include "loadsave.h"
#include "mainloop.h"
#include "mainmenu.h"
#include "netgame.h"
#include "network.h"
#include "opponent.h"
#include "piping.h"
#include "pratcam.h"
#include "racesumm.h"
#include "racestrt.h"
#include "sound.h"
#include "temp.h"
#include "tinted.h"
#include "utility.h"
#include "world.h"

#include "c2_stdlib.h"
#include "structur.h"

#include "globvars.h"
#include "globvrpb.h"
#include "platform.h"
#include "carpocalypse2_types.h"
#include "brender/brender.h"
#include "world3.h"
#include "input.h"
#include <stdlib.h>

#ifdef CARPOCALYPSE2_MATCHING
#include "c2_hooks.h"

// The retail compiler inlined these single-call-site helpers into DoProgram.
#if defined(_MSC_VER)
#define C2_MATCHING_INLINE __inline
#else
#define C2_MATCHING_INLINE inline
#endif
#else
#define C2_MATCHING_INLINE
#endif
#include "font.h"
#include "graphics2.h"
// GLOBAL: CARMA2_HW 0x00660ce0
int gMirror_on__structur = 1;

// GLOBAL: CARMA2_HW 0x00660ce8
int gLoad_last_save_game = 1;

// GLOBAL: CARMA2_HW 0x007051f8
int gMoney_pre_race;

// GLOBAL: CARMA2_HW 0x007051e0
int gAPO_pre_race[3];

// GLOBAL: CARMA2_HW 0x006b781c
tRace_over_reason gRace_over_reason;

// GLOBAL: CARMA2_HW 0x006aaa34
tU32 gLast_checkpoint_time;

// GLOBAL: CARMA2_HW 0x00660cdc
int gLast_wrong_checkpoint = -1;

// GLOBAL: CARMA2_HW 0x006aaa38
int gHud_tinted1_visible;

// GLOBAL: CARMA2_HW 0x006aaa3c
int gHud_tinted2_visible;

// GLOBAL: CARMA2_HW 0x00655e48
int gHud_tinted1 = -1;

// GLOBAL: CARMA2_HW 0x00655e4c
int gHud_tinted2 = -1;

// GLOBAL: CARMA2_HW 0x00655e50
int gHud_tinted3 = -1;

// GLOBAL: CARMA2_HW 0x00762140
int gCredits_checkpoint[3];

#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_FASTCALL CheckSingleFace(tFace_ref* pFace, br_vector3* ray_pos, br_vector3* ray_dir, br_vector3* normal, br_scalar* rt, br_vector3* coll_pos);
extern tS3_sound_tag C2_HOOK_FASTCALL DRS3StartSound(tS3_outlet* pOutlet, tS3_sound_id pSound);
extern void C2_HOOK_FASTCALL DeclareWinner(int pWinner_index);
extern void C2_HOOK_FASTCALL DoGameCompletedAnimation(void);
extern int C2_HOOK_FASTCALL FrontEndProcessMenus(tFrontendMenuType pFrontendType);
extern int C2_HOOK_FASTCALL GetCarCount(tVehicle_type pCategory);
extern tCar_spec* C2_HOOK_FASTCALL GetCarSpec(tVehicle_type pCategory, int pIndex);
extern const char* C2_HOOK_FASTCALL GetMiscString(int pIndex);
extern tU32 C2_HOOK_FASTCALL GetTotalTime(void);
extern int C2_HOOK_FASTCALL IRandomBetween(int pA, int pB);
extern void C2_HOOK_FASTCALL IncrementCheckpoint(void);
extern tNet_game_player_info* C2_HOOK_FASTCALL NetPlayerFromCar(tCar_spec *pCar);
extern int C2_HOOK_FASTCALL NewTextHeadupSlot(int pSlot_index, int pFlash_rate, int pLifetime, int pFont_index, const char* pText);
extern void C2_HOOK_FASTCALL SendGameplay(tPlayer_ID pPlayer, tNet_gameplay_mess pMess, int pParam_1, int pParam_2, int pParam_3, int pParam_4);
extern int gCheckpoint;
extern int gCountRaceGroups;
extern tNet_game_details* gCurrent_net_game;
extern tRace_info gCurrent_race;
extern tRace_group_spec* gCurrent_race_group;
extern char gFLOAT_0058b130[];
extern int gIs_boundary_race;
extern int gLap;
extern tNet_mode gNet_mode;
extern tNet_game_player_info gNet_players[12];
extern int gNo_credits_APO_restore;
extern int gNumber_of_races;
extern tS3_outlet* gPedestrians_outlet;
extern tProgram_state gProgram_state;
extern tU32 gRace_finished;
extern tRace_group_spec* gRace_groups;
extern tRace_list_spec gRace_list[100];
#endif
// FUNCTION: CARMA2_HW 0x004e3410
void C2_HOOK_FASTCALL RecordAPOAndCredits(void) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, power_up_levels, 0x18a8);

    gMoney_pre_race = gProgram_state.credits;
    gAPO_pre_race[0] = gProgram_state.current_car.power_up_levels[0];
    gAPO_pre_race[1] = gProgram_state.current_car.power_up_levels[1];
    gAPO_pre_race[2] = gProgram_state.current_car.power_up_levels[2];
}

// FUNCTION: CARMA2_HW 0x004e3440
int C2_NAKED C2_HOOK_FASTCALL DoPostRace(tRace_result pRace_result) {
#ifndef CARPOCALYPSE2_MATCHING

    NOT_IMPLEMENTED();
    return 0;
#else
    __asm {
            push ecx
            push ebx
            push ebp
            xor edx, edx
            push esi
            cmp ecx, 3
            push edi
            mov dword ptr [esp + 0x10], edx
            jne L004e3611
            mov eax, dword ptr [gCurrent_race+0x98]
            mov ecx, dword ptr [eax + 0x184]
            inc ecx
            mov dword ptr [eax + 0x184], ecx
            mov eax, dword ptr [gProgram_state+0x84]
            cmp eax, edx
            je L004e34a2
            mov eax, dword ptr [gNumber_of_races]
            xor ecx, ecx
            lea edx, [eax - 1]
            call IRandomBetween
            lea ecx, [eax + eax*8]
            mov dword ptr [gProgram_state+0x78], eax
            shl ecx, 4
            add ecx, eax
            mov eax, dword ptr [esp + 0x10]
            mov edx, dword ptr [ecx*4 + gRace_list+0x240]
            mov dword ptr [gCurrent_race_group], edx
            pop edi
            pop esi
            pop ebp
            pop ebx
            pop ecx
            ret
    L004e34a2:
            mov eax, dword ptr [gCurrent_race+0x98]
            cmp dword ptr [eax + 0x190], edx
            je L004e3564
            mov esi, dword ptr [gCurrent_race_group]
            mov eax, dword ptr [gRace_groups]
            add esi, 0xc
            mov dword ptr [gIs_boundary_race], edx
            mov ecx, esi
            mov dword ptr [gCurrent_race_group], esi
            sub ecx, eax
            mov eax, 0x2aaaaaab
            imul ecx
            mov eax, dword ptr [gCountRaceGroups]
            sar edx, 1
            mov ecx, edx
            shr ecx, 0x1f
            add edx, ecx
            cmp edx, eax
            jl L004e3538
            mov esi, 1
            mov dword ptr [gProgram_state+0x84], esi
            call DoGameCompletedAnimation
            mov ecx, 5
            call FrontEndProcessMenus
            mov edx, dword ptr [gNumber_of_races]
            xor ecx, ecx
            dec edx
            call IRandomBetween
            lea ecx, [eax + eax*8]
            mov dword ptr [gProgram_state+0x78], eax
            shl ecx, 4
            add ecx, eax
            mov dword ptr [esp + 0x10], esi
            mov eax, esi
            mov edx, dword ptr [ecx*4 + gRace_list+0x240]
            mov dword ptr [gCurrent_race_group], edx
            pop edi
            pop esi
            pop ebp
            pop ebx
            pop ecx
            ret
    L004e3538:
            mov ecx, dword ptr [esi + 4]
            mov eax, 0xe1fc780f
            sub ecx, offset gRace_list
            imul ecx
            add edx, ecx
            sar edx, 9
            mov eax, edx
            shr eax, 0x1f
            add edx, eax
            mov eax, dword ptr [esp + 0x10]
            mov dword ptr [gProgram_state+0x78], edx
            pop edi
            pop esi
            pop ebp
            pop ebx
            pop ecx
            ret
    L004e3564:
            mov ebp, dword ptr [gCurrent_race_group]
            xor eax, eax
            mov esi, 0x2710
            mov ebx, dword ptr [ebp]
            cmp ebx, edx
            jle L004e359c
            mov edx, dword ptr [ebp + 4]
            mov edi, dword ptr [esp + 0x10]
            add edx, 0x184
    L004e3585:
            mov ecx, dword ptr [edx]
            cmp ecx, esi
            jge L004e358f
            mov esi, ecx
            mov edi, eax
    L004e358f:
            inc eax
            add edx, 0x244
            cmp eax, ebx
            jl L004e3585
            jmp L004e35a0
    L004e359c:
            mov edi, dword ptr [esp + 0x10]
    L004e35a0:
            test esi, esi
            je L004e35da
            mov dword ptr [gIs_boundary_race], 1
            mov ecx, dword ptr [ebp + 8]
            sub ecx, offset gRace_list
            mov eax, 0xe1fc780f
            imul ecx
            add edx, ecx
            mov eax, dword ptr [esp + 0x10]
            sar edx, 9
            mov ecx, edx
            shr ecx, 0x1f
            add edx, ecx
            mov dword ptr [gProgram_state+0x78], edx
            pop edi
            pop esi
            pop ebp
            pop ebx
            pop ecx
            ret
    L004e35da:
            mov eax, dword ptr [ebp + 4]
            lea edx, [edi + edi*8]
            shl edx, 4
            add edx, edi
            lea ecx, [eax + edx*4]
            mov eax, 0xe1fc780f
            sub ecx, offset gRace_list
            imul ecx
            add edx, ecx
            mov eax, dword ptr [esp + 0x10]
            sar edx, 9
            mov ecx, edx
            shr ecx, 0x1f
            add edx, ecx
            mov dword ptr [gProgram_state+0x78], edx
            pop edi
            pop esi
            pop ebp
            pop ebx
            pop ecx
            ret
    L004e3611:
            cmp dword ptr [gNo_credits_APO_restore], edx
            jne L004e3647
            mov edx, dword ptr [gMoney_pre_race]
            mov eax, dword ptr [gAPO_pre_race]
            mov ecx, dword ptr [gAPO_pre_race+0x4]
            mov dword ptr [gProgram_state], edx
            mov edx, dword ptr [gAPO_pre_race+0x8]
            mov dword ptr [gProgram_state+0x1954], eax
            mov dword ptr [gProgram_state+0x1958], ecx
            mov dword ptr [gProgram_state+0x195c], edx
    L004e3647:
            mov eax, dword ptr [esp + 0x10]
            pop edi
            pop esi
            pop ebp
            pop ebx
            pop ecx
            ret
    }
#endif
}

void C2_HOOK_FASTCALL InitialiseProgramState(void) {

    gProgram_state.mirror_on = gMirror_on__structur;
    gProgram_state.prog_status = eProg_intro;
    gProgram_state.last_slot = 0;
    gProgram_state.view_type = eVT_Scene;
    gProgram_state.frank_or_anniness = eFrankie;
    gProgram_state.racing = 0;
    gProgram_state.cut_scene = 0;
    gProgram_state.saving = 0;
    gProgram_state.loading = 0;
    gProgram_state.dont_save_or_load = 0;
    gProgram_state.dont_load = 0;
    gProgram_state.field_0x2c = 0;
    if (gAusterity_mode) {
        gProgram_state.prat_cam_on = 0;
    } else {
        gProgram_state.prat_cam_on = gPratcam_on;
    }
    gPratcam_on = gProgram_state.prat_cam_on;
    gProgram_state.cockpit_on = gCockpit_on;
    gProgram_state.car_name[0] = '\0';
    SetSoundVolumes(0);
}

void C2_HOOK_FASTCALL DoLogos(void) {

    ClearEntireScreen();
    DoSCILogo();
    DoOpeningAnimation();
    DoStainlessLogo();
    gProgram_state.prog_status = eProg_opening;
}

void C2_HOOK_FASTCALL DoProgramDemo(void) {

    DoLogos();
    gProgram_state.prog_status = eProg_idling;
    DRS3StopOutletSound(gEffects_outlet);
}

void C2_HOOK_FASTCALL DoProgOpeningAnimation(void) {

    gProgram_state.prog_status = eProg_idling;
    DRS3StopOutletSound(gEffects_outlet);
}

C2_MATCHING_INLINE void C2_HOOK_FASTCALL SwapNetCarsLoad(void) {
    int i;

    DisableNetService();
    AboutToLoadFirstCar();
    for (i = 0; i < gNumber_of_net_players; i++) {
        if (gNet_players[i].next_car_index >= 0) {
            gNet_players[i].car_index = gNet_players[i].next_car_index;
        }
        gNet_players[i].next_car_index = -1;
        LoadCar(gOpponents[gNet_players[i].car_index].car_file_name,
            (gThis_net_player_index == i) ? eDriver_local_human : eDriver_net_human,
            gNet_players[i].car, gNet_players[i].car_index, gNet_players[i].player_name,
            &gNet_cars_storage_space);
    }
    ReenableNetService();
}

C2_MATCHING_INLINE void C2_HOOK_FASTCALL SwapNetCarsDispose(void) {
    int i;

    C2_HOOK_BUG_ON(sizeof(tCar_detail_info) != 20);

    DisableNetService();
    for (i = 0; i < gNumber_of_net_players; i++) {
        DisposeCar(gNet_players[i].car, gNet_players[i].car_index);
        if (gNet_players[i].car_index >= 0) {
            gCar_details[gNet_players[i].car_index].ownership = eCar_owner_none;
        }
    }
    ClearOutStorageSpace(&gOur_car_storage_space);
    ClearOutStorageSpace(&gNet_cars_storage_space);
    ReenableNetService();
}

C2_MATCHING_INLINE void C2_HOOK_FASTCALL DoGame(void) {
    tSO_result options_result;
    tRace_result race_result;
    int i;

    PrintMemoryDump(0,"START OF DoGame()");
    gAbandon_game = 0;
    gDisallow_abandon_race = 0;
    gCar_to_view = &gProgram_state.current_car;
    StartLoadingScreen();
    gProgram_state.prog_status = eProg_game_ongoing;
    if (gNet_mode != gNet_mode_of_last_game) {
        gProgram_state.prog_status = eProg_idling;
        return;
    }
    PrintMemoryDump(0, "BEFORE START RACE SCREEN");
    LoadRaceInfo(gProgram_state.current_race_index, &gCurrent_race);
    SelectOpponents(&gCurrent_race);
    if (gNet_mode == eNet_mode_none) {
        SelectOpponents(&gCurrent_race);
        LoadRaceInfo(gProgram_state.current_race_index, &gCurrent_race);
        FillInRaceInfo(&gCurrent_race);
    } else {
        FillInRaceInfo(&gCurrent_race);
        DisposeRaceInfo(&gCurrent_race);
    }
    PrintMemoryDump(0, "AFTER START RACE SCREEN");
    if (gNet_mode != eNet_mode_none) {
        if ((gCurrent_net_game->options.car_choice == eNet_car_novelty || gCurrent_net_game->options.car_choice == eNet_car_all)
                && !gNo_races_yet) {
            SwapNetCarsLoad();
        }
    }
    PrintMemoryDump(0, "AFTER LOADING OPPONENTS IN");
    InitRace();
    if (gNet_mode != gNet_mode_of_last_game) {
        gProgram_state.prog_status = eProg_idling;
        return;
    }
    if (gNet_mode == eNet_mode_none) {
        DoGridPosition();
        SetInitialPositions(&gCurrent_race);
    } else {
        FrontEndProcessMenus(kFrontend_menu_netsync);
        dr_dprintf("Returned from NetSync screen");
        SwitchToLoresMode();
    }
    if (gProgram_state.prog_status == eProg_idling) {
        dr_dprintf("Calling DisposeRace()");
        DisposeRace();
        dr_dprintf("Calling DisposeTrack()");
        DisposeTrack();
        dr_dprintf("Returning from DoGame()");
        return;
    }
    InitialiseAIWorldForRace(&gCurrent_race);
    InitialiseCarsEtc(&gCurrent_race);
    InitSoundSources();
    InitLastDamageArrayEtc();
    FogificateMaterials(kRendererShadingType_Default);
    EnableAutoFogification();
    RecordAPOAndCredits();
    race_result = DoRace();
    DisableAutoFogification();
    DisposeRace();
    PrintMemoryDump(0, "AFTER DISPOSING RACE");
    if (gNet_mode == eNet_mode_none) {
        options_result = DoPostRace(race_result);
        if (race_result == eRace_completed) {
            gSave_game_out_of_sync = 1;
            DoEndRaceAnimation();
            DoEndRaceSummary(eRace_completed, options_result);
            DoSaveGame();
        }
    } else {
        gProgram_state.current_race_index = gPending_race;
        gCurrent_net_game->start_race = gPending_race;
        gPending_race = -1;
        if (race_result != eRace_game_abandonned && race_result != eRace_aborted) {
            DoEndRaceSummary(race_result, 0);
        }
    }
    if (gNet_mode == eNet_mode_none) {
        TotallyRepairCar();
    } else {
        for (i = 0; i < gNumber_of_net_players; i++) {
            TotallyRepairACar(gNet_players[i].car);
        }
    }
    if (gNet_mode == eNet_mode_none) {
        DisposeOpponentsCars(&gCurrent_race);
    } else {
        if ((gCurrent_net_game->options.car_choice == eNet_car_all || gCurrent_net_game->options.car_choice == eNet_car_novelty)
                && !gNo_races_yet) {
            SwapNetCarsDispose();
        }
    }
    PrintMemoryDump(0, "AFTER DISPOSING CARS");
    DisposeTrack();
    PrintMemoryDump(0, "AFTER DISPOSING TRACK");
    if (gProgram_state.prog_status == eProg_game_ongoing) {
        if (gNet_mode == eNet_mode_none) {
            gProgram_state.prog_status = eProg_idling;
        } else {
            gProgram_state.prog_status = eProg_game_starting;
        }
    }
}

// FUNCTION: CARMA2_HW 0x00503b00
int C2_HOOK_FASTCALL ChooseOpponent(int pNastiness) {
    int i;
    int count;
    int temp_array[40];

    while (1) {
        count = 0;
        for (i = 0; i < gNumber_of_racers; i++) {
            if ((pNastiness == 0 || gOpponents[i].strength_rating == pNastiness)
                && gProgram_state.current_car_index != i
                && !gOpponents[i].picked
                && gOpponents[i].strength_rating <= 5
                && gOpponents[i].strength_rating >= 1) {
                temp_array[count++] = i;
            }
        }
        if (count != 0) {
            break;
        }
        if (pNastiness < 5) {
            pNastiness++;
        } else {
            pNastiness = 0;
        }
    }
    i = temp_array[IRandomBetween(0, count - 1)];
    gOpponents[i].picked = 1;
    return i;
}

int C2_HOOK_FASTCALL NumberOfOpponentsStillRunning(void) {
    int i;
    int car_count;
    int result;
    tCar_spec* the_car;

    result = 0;
    car_count = GetCarCount(eVehicle_opponent);
    for (i = 0; i < car_count; i++) {
        the_car = GetCarSpec(eVehicle_opponent, i);

        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, knackered, 0x1d4);

        if (!the_car->knackered) {
            result++;
        }
    }
    return result;
}

// FUNCTION: CARMA2_HW 0x005039e0
void C2_HOOK_FASTCALL SelectOpponents(tRace_info* pRace_info) {
    int i;

    if (gNet_mode == eNet_mode_none) {
        pRace_info->number_of_racers = pRace_info->race_spec->count_explicit_opponents;
        for (i = 0; i < gNumber_of_racers; i++) {
            gOpponents[i].picked = 0;
        }
        for (i = 0; i < pRace_info->race_spec->count_explicit_opponents; i++) {
            if (pRace_info->race_spec->explicit_opponents[i] >= 0) {
                gOpponents[pRace_info->race_spec->explicit_opponents[i]].picked = 1;
                pRace_info->opponent_list[i].index = pRace_info->race_spec->explicit_opponents[i];
                pRace_info->opponent_list[i].ranking = -(i + 1);
            }
        }
        for (i = 0; i < pRace_info->race_spec->count_explicit_opponents; i++) {
            if (pRace_info->race_spec->explicit_opponents[i] < 0) {
                pRace_info->opponent_list[i].index = ChooseOpponent(-pRace_info->race_spec->explicit_opponents[i]);
                pRace_info->opponent_list[i].ranking = IRandomBetween(0, 99);
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x00503050
int C2_HOOK_FASTCALL NumberOfOpponentsLeft(void) {
    int result;
    int count;
    int i;

    result = 0;
    count = GetCarCount(eVehicle_opponent);
    for (i = 0; i < count; i++) {
        tCar_spec* car_spec = GetCarSpec(eVehicle_opponent, i);

        if (!car_spec->knackered && (gCurrent_race.race_spec->race_type == kRaceType_Carma1 || (gCurrent_race.race_spec->race_type == kRaceType_Cars && car_spec->is_race_goal))) {
            result += 1;
        }
    }
    return result;
}

// FUNCTION: CARMA2_HW 0x005030b0
void C2_HOOK_FASTCALL RaceCompleted(tRace_over_reason pReason) {

    if (gRace_finished) {
        return;
    }
    if (gNet_mode == eNet_mode_host && pReason < eRace_over_8) {
        NetFinishRace(gCurrent_net_game, pReason);
    }
    if (pReason == eRace_over_out_of_time || pReason == eRace_over_7) {
        ChangeAmbientPratcam(35);
    } else if (pReason < eRace_over_abandoned) {
        ChangeAmbientPratcam(34);
    }
    gRace_over_reason = pReason;
    if (gMap_view == 2) {
        ToggleMap();
    }
    switch (gRace_over_reason) {
    case eRace_over_0:
    case eRace_over_1:
    case eRace_over_2:
    case eRace_over_3:
        ChangeAmbientPratcam(34);
        DoFancyHeadup(gCurrent_race.race_spec->is_boundary ? 34 : 23);
        DRS3StartSound(gPedestrians_outlet, eSoundId_RaceComplete);
        break;
    case eRace_over_abandoned:
        if (gNet_mode == eNet_mode_client) {
            gHost_abandon_game = 1;
            NetFullScreenMessage(eMiscString_host_abandoned_race, 0);
        }
        break;
    case eRace_over_out_of_time:
        ChangeAmbientPratcam(35);
        DoFancyHeadup(gCurrent_race.race_spec->is_boundary ? 33 : 22);
        DRS3StartSound(gPedestrians_outlet, eSoundId_OutOfTime2);
        break;
    case eRace_over_6:
        ChangeAmbientPratcam(36);
        DoFancyHeadup(gCurrent_race.race_spec->is_boundary ? 33 : 24);
        break;
    case eRace_over_7:
        ChangeAmbientPratcam(35);
        DoFancyHeadup(35);
        break;
    case eRace_over_8:
        ChangeAmbientPratcam(34);
        DoFancyHeadup(29);
        break;
    case eRace_over_9:
        ChangeAmbientPratcam(36);
        DoFancyHeadup(26);
        break;
    default:
        break;
    }
    if (gNet_mode == eNet_mode_none) {
        gRace_finished = 10000;
    } else {
        gRace_finished = 8000;
    }
}

// FUNCTION: CARMA2_HW 0x00503270
void C2_HOOK_FASTCALL Checkpoint(int pCheckpoint_index, int pDo_sound) {

    PratcamEvent(33);
    DoFancyHeadup(21);
    if (pDo_sound) {
        DRS3StartSound(gPedestrians_outlet, eSoundId_Checkpoint);
    }
}

// FUNCTION: CARMA2_HW 0x005032a0
void C2_HOOK_FASTCALL IncrementCheckpoint(void) {
    int done_voice;

    done_voice = 0;
    if (gRace_finished) {
        return;
    }
    if (gCurrent_race.race_spec->race_type != kRaceType_Carma1
            && gCurrent_race.race_spec->race_type != kRaceType_Checkpoints) {
        return;
    }
    gLast_checkpoint_time = GetTotalTime();
    if (gCheckpoint >= gCheckpoint_count) {
        gCheckpoint = 1;
        if (gLap++ >= gTotal_laps) {
            gLap = gTotal_laps;
            gCheckpoint = gCheckpoint_count;
            RaceCompleted(eRace_over_0);
        } else if (gLap == gTotal_laps) {
            PratcamEvent(33);
            NewTextHeadupSlot(4, 0, 1000, -4, GetMiscString(eMiscString_final_lap));
            DRS3StartSound(gPedestrians_outlet, eSoundId_FinalLap);
            done_voice = 1;
        }
    } else {
        gCheckpoint += 1;
    }
    if (!gRace_finished) {
        PratcamEvent(33);
        DoFancyHeadup(21);
        if (!done_voice) {
            DRS3StartSound(gPedestrians_outlet, eSoundId_Checkpoint);
        }
        if (gCredits_checkpoint[gProgram_state.skill_level] != 0) {
            EarnCredits(gCredits_checkpoint[gProgram_state.skill_level]);
        }
    }
}

// FUNCTION: CARMA2_HW 0x005033b0
void C2_HOOK_FASTCALL IncrementLap(void) {
    int i;

    for (i = gCheckpoint; i <= gCheckpoint_count; i++) {
        IncrementCheckpoint();
    }
}

// FUNCTION: CARMA2_HW 0x005033e0
void C2_HOOK_FASTCALL WrongCheckpoint(int pCheckpoint_index) {
    int next_checkpoint;

    if (gRace_finished) {
        return;
    }
    if (gCurrent_race.race_spec->race_type != kRaceType_Carma1
            && gCurrent_race.race_spec->race_type != kRaceType_Checkpoints) {
        return;
    }
    if ((gLast_wrong_checkpoint == pCheckpoint_index && GetTotalTime() - gLast_checkpoint_time > 20000)
            || (gLast_wrong_checkpoint != pCheckpoint_index && GetTotalTime() - gLast_checkpoint_time > 2000)) {
        if (gNet_mode == eNet_mode_none) {
            next_checkpoint = pCheckpoint_index + 2;
            if (next_checkpoint > gCurrent_race.check_point_count) {
                if (gLap == 1) {
                    next_checkpoint = -1;
                } else {
                    next_checkpoint = 1;
                }
            }
            if (next_checkpoint == gCheckpoint) {
                return;
            }
        }
        NewTextHeadupSlot(4, 0, 1000, -4, GetMiscString(eMiscString_wrong_checkpoint));
        DRS3StartSound(gPedestrians_outlet, eSoundId_WrongCheckpoint);
        gLast_checkpoint_time = GetTotalTime();
        gLast_wrong_checkpoint = pCheckpoint_index;
    }
}

int C2_HOOK_FASTCALL RayHitFace(br_vector3* pV0, br_vector3* pV1, br_vector3* pV2, br_vector3* pNormal, br_vector3* pStart, br_vector3* pDir) {
    tFace_ref the_face;
    br_scalar rt;
    br_vector3 rp;

    the_face.material = NULL;
    BrVector3Copy(&the_face.v[0], pV0);
    BrVector3Copy(&the_face.v[1], pV1);
    BrVector3Copy(&the_face.v[2], pV2);
    BrVector3Copy(&the_face.normal, pNormal);
    CheckSingleFace(&the_face, pStart, pDir, &the_face.normal, &rt, &rp);
    return rt >= 0.f && rt <= 1.f;
}

// FUNCTION: CARMA2_HW 0x005034b0
void C2_NAKED C2_HOOK_FASTCALL CheckCheckpoints(void) {
#ifndef CARPOCALYPSE2_MATCHING
    tCar_spec* car;
    br_vector3 orig;
    br_vector3 dir;
    int i;
    int j;
    int cat;
    int car_count;
    int car_index;
    tNet_game_player_info* net_player;

    if (gNet_mode == eNet_mode_client) {
        return;
    }
    if (gNet_mode == eNet_mode_host && gCurrent_net_game->type != eNet_game_type_5 && gCurrent_net_game->type != eNet_game_type_checkpoint && gCurrent_net_game->type != eNet_game_type_4) {
        return;
    }
    for (cat = eVehicle_self; cat <= gNet_mode; cat++) {
        if (cat == eVehicle_self) {
            car_count = 1;
        } else {
            car_count = GetCarCount(cat);
        }
        for (car_index = 0; car_index < car_count; car_index++) {
            if (cat == eVehicle_self) {
                car = &gProgram_state.current_car;
            } else {
                car = GetCarSpec(cat, car_index);
            }
            BrVector3Copy(&orig, (br_vector3*)car->old_frame_mat.m[3]);
            BrVector3Sub(&dir, &car->car_master_actor->t.t.translate.t, &orig);
            for (i = 0; i < gCurrent_race.check_point_count; i++) {
                for (j = 0; j < gCurrent_race.checkpoints[i].count_quads; j++) {
                    if (
                            RayHitFace(&gCurrent_race.checkpoints[i].quads[j].points[0],
                                       &gCurrent_race.checkpoints[i].quads[j].points[1],
                                       &gCurrent_race.checkpoints[i].quads[j].points[2],
                                       &gCurrent_race.checkpoints[i].normals[j],
                                       &orig, &dir)
                            || RayHitFace(&gCurrent_race.checkpoints[i].quads[j].points[0],
                                          &gCurrent_race.checkpoints[i].quads[j].points[2],
                                          &gCurrent_race.checkpoints[i].quads[j].points[3],
                                          &gCurrent_race.checkpoints[i].normals[j],
                                          &orig,
                                          &dir)) {
                        if (gNet_mode == eNet_mode_none) {
                            if (i + 1 == gCheckpoint) {
                                IncrementCheckpoint();
                            } else {
                                WrongCheckpoint(i);
                            }
                        } else {
                            net_player = NetPlayerFromCar(car);
                            if (gCurrent_net_game->type == eNet_game_type_checkpoint) {
                                if (net_player->score2 & (1 << i)) {
                                    net_player->score2 &= ~(1 << i);
                                    SendGameplay(net_player->ID, eNet_gameplay_checkpoint, i, 0, 0, 0);
                                } else {
                                    SendGameplay(net_player->ID, eNet_gameplay_wrong_checkpoint, i, 0, 0, 0);
                                }
                            } else if (gCurrent_net_game->type == eNet_game_type_5) {
                                if ((net_player->score2 % gCurrent_race.check_point_count) == i) {
                                    net_player->score2 += 1;
                                    SendGameplay(net_player->ID, eNet_gameplay_checkpoint, i, 0, 0, 0);
                                    gLast_checkpoint_time = GetTotalTime();
                                    if (net_player->score2 >= gCurrent_net_game->options.starting_target * gCurrent_race.check_point_count) {
                                        DeclareWinner(net_player - gNet_players);
                                    }
                                } else {
                                    SendGameplay(net_player->ID, eNet_gameplay_wrong_checkpoint, i, 0, 0, 0);
                                }
                            } else {
                                if ((net_player->score2 % gCurrent_race.check_point_count) == i) {
                                    net_player->score2 += 1;
                                    SendGameplay(net_player->ID, eNet_gameplay_checkpoint, i, 0, 0, 0);
                                    break;
                                } else {
                                    SendGameplay(net_player->ID, eNet_gameplay_wrong_checkpoint, i, 0, 0, 0);
                                }
                            }
                        }
                        break;
                    }
                }
            }
            BrMatrix34Copy(&car->old_frame_mat, &car->car_master_actor->t.t.mat);
        }
    }
#else
    __asm {
            mov ecx, dword ptr [gNet_mode]
            sub esp, 0xf0
            cmp ecx, 3
            push ebx
            push ebp
            push esi
            push edi
            je L005039a2
            cmp ecx, 2
            jne L005034e9
            mov eax, dword ptr [gCurrent_net_game]
            mov eax, dword ptr [eax + 0x74]
            cmp eax, 5
            je L005034e9
            cmp eax, 3
            je L005034e9
            cmp eax, 4
            jne L005039a2
    L005034e9:
            xor esi, esi
            test ecx, ecx
            mov dword ptr [esp + 0x1c], esi
            jl L005039a2
    L005034f7:
            test esi, esi
            jne L00503505
            mov dword ptr [esp + 0x18], 1
            jmp L00503510
    L00503505:
            mov ecx, esi
            call GetCarCount
            mov dword ptr [esp + 0x18], eax
    L00503510:
            mov eax, dword ptr [esp + 0x18]
            xor ebx, ebx
            test eax, eax
            mov dword ptr [esp + 0x28], ebx
            jle L00503990
    L00503522:
            test esi, esi
            jne L00503530
            mov dword ptr [esp + 0x10], 0x75bc2c
            jmp L0050353d
    L00503530:
            mov edx, ebx
            mov ecx, esi
            call GetCarSpec
            mov dword ptr [esp + 0x10], eax
    L0050353d:
            mov eax, dword ptr [esp + 0x10]
            xor ebp, ebp
            lea ecx, [eax + 0x40]
            mov edx, dword ptr [eax + 0x40]
            mov dword ptr [esp + 0x30], edx
            mov edx, dword ptr [ecx + 4]
            mov dword ptr [esp + 0x34], edx
            mov ecx, dword ptr [ecx + 8]
            mov dword ptr [esp + 0x38], ecx
            mov edx, dword ptr [eax + 0x10]
            fld dword ptr [edx + 0x50]
            fsub dword ptr [esp + 0x30]
            fstp dword ptr [esp + 0x3c]
            mov ecx, dword ptr [eax + 0x10]
            fld dword ptr [ecx + 0x54]
            fsub dword ptr [esp + 0x34]
            mov ecx, dword ptr [gCurrent_race+0x8]
            test ecx, ecx
            fstp dword ptr [esp + 0x40]
            mov edx, dword ptr [eax + 0x10]
            fld dword ptr [edx + 0x58]
            fsub dword ptr [esp + 0x38]
            fstp dword ptr [esp + 0x44]
            jle L0050396b
            mov esi, offset gCurrent_race+0x228
            mov dword ptr [esp + 0x14], esi
    L0050359c:
            mov eax, dword ptr [esi]
            mov dword ptr [esp + 0x20], 0
            test eax, eax
            jle L00503947
            add esi, 4
            lea ebx, [esi + 0xc0]
    L005035b7:
            mov eax, esi
            lea edi, [esi + 0x18]
            mov dword ptr [esp + 0x98], 0
            mov ecx, dword ptr [eax]
            mov dword ptr [esp + 0x9c], ecx
            lea ecx, [esi + 0xc]
            mov edx, dword ptr [eax + 4]
            mov dword ptr [esp + 0xa0], edx
            mov edx, dword ptr [ecx]
            mov eax, dword ptr [eax + 8]
            mov dword ptr [esp + 0xa8], edx
            mov dword ptr [esp + 0xa4], eax
            mov eax, dword ptr [ecx + 4]
            mov edx, edi
            mov dword ptr [esp + 0xac], eax
            mov ecx, dword ptr [ecx + 8]
            mov eax, dword ptr [edx]
            mov dword ptr [esp + 0xb0], ecx
            mov dword ptr [esp + 0xb4], eax
            mov eax, ebx
            mov ecx, dword ptr [edx + 4]
            mov dword ptr [esp + 0xb8], ecx
            mov ecx, dword ptr [eax]
            mov edx, dword ptr [edx + 8]
            mov dword ptr [esp + 0xcc], ecx
            mov dword ptr [esp + 0xbc], edx
            mov edx, dword ptr [eax + 4]
            mov dword ptr [esp + 0xd0], edx
            lea ecx, [esp + 0xf4]
            mov eax, dword ptr [eax + 8]
            lea edx, [esp + 0x24]
            mov dword ptr [esp + 0xd4], eax
            push ecx
            lea eax, [esp + 0xd0]
            push edx
            lea ecx, [esp + 0x44]
            push eax
            push ecx
            lea edx, [esp + 0x40]
            lea ecx, [esp + 0xa8]
            call CheckSingleFace
            fld dword ptr [esp + 0x24]
            fcomp dword ptr [gFLOAT_0058b130+0x3c]
            fnstsw ax
            test ah, 1
            jne L00503692
            fld dword ptr [esp + 0x24]
            fcomp dword ptr [gFLOAT_0058b130+0x40]
            fnstsw ax
            test ah, 0x41
            jne L00503762
    L00503692:
            mov edx, esi
            mov dword ptr [esp + 0x48], 0
            mov eax, dword ptr [edx]
            mov dword ptr [esp + 0x4c], eax
            mov eax, dword ptr [edi]
            mov ecx, dword ptr [edx + 4]
            mov dword ptr [esp + 0x58], eax
            mov dword ptr [esp + 0x50], ecx
            mov ecx, dword ptr [edi + 4]
            mov edx, dword ptr [edx + 8]
            lea eax, [esi + 0x24]
            mov dword ptr [esp + 0x54], edx
            mov edx, dword ptr [edi + 8]
            mov dword ptr [esp + 0x5c], ecx
            mov ecx, dword ptr [eax]
            mov dword ptr [esp + 0x60], edx
            mov dword ptr [esp + 0x64], ecx
            mov edx, dword ptr [eax + 4]
            mov ecx, ebx
            mov dword ptr [esp + 0x68], edx
            mov eax, dword ptr [eax + 8]
            mov edx, dword ptr [ecx]
            mov dword ptr [esp + 0x6c], eax
            mov dword ptr [esp + 0x7c], edx
            mov eax, dword ptr [ecx + 4]
            lea edx, [esp + 0xe8]
            mov dword ptr [esp + 0x80], eax
            lea eax, [esp + 0x2c]
            mov ecx, dword ptr [ecx + 8]
            push edx
            mov dword ptr [esp + 0x88], ecx
            lea ecx, [esp + 0x80]
            push eax
            lea edx, [esp + 0x44]
            push ecx
            push edx
            lea edx, [esp + 0x40]
            lea ecx, [esp + 0x58]
            call CheckSingleFace
            fld dword ptr [esp + 0x2c]
            fcomp dword ptr [gFLOAT_0058b130+0x3c]
            fnstsw ax
            test ah, 1
            jne L00503740
            fld dword ptr [esp + 0x2c]
            fcomp dword ptr [gFLOAT_0058b130+0x40]
            fnstsw ax
            test ah, 0x41
            jne L00503762
    L00503740:
            mov ecx, dword ptr [esp + 0x14]
            mov eax, dword ptr [esp + 0x20]
            inc eax
            add ebx, 0xc
            mov edx, dword ptr [ecx]
            add esi, 0x30
            cmp eax, edx
            mov dword ptr [esp + 0x20], eax
            jge L00503947
            jmp L005035b7
    L00503762:
            mov eax, dword ptr [gNet_mode]
            test eax, eax
            je L0050386b
            mov ecx, dword ptr [esp + 0x10]
            call NetPlayerFromCar
            mov edx, dword ptr [gCurrent_net_game]
            mov esi, eax
            mov eax, dword ptr [edx + 0x74]
            cmp eax, 3
            jne L005037b9
            mov eax, 1
            mov ecx, ebp
            shl eax, cl
            mov ecx, dword ptr [esi + 0x64]
            test ecx, eax
            je L00503852
            not eax
            and ecx, eax
            push 0
            push 0
            mov dword ptr [esi + 0x64], ecx
            mov ecx, dword ptr [esi + 0x24]
            push 0
            push ebp
            xor edx, edx
            call SendGameplay
            jmp L00503947
    L005037b9:
            mov ecx, dword ptr [esi + 0x64]
            cmp eax, 5
            mov eax, ecx
            cdq
            jne L0050382e
            idiv dword ptr [gCurrent_race+0x8]
            cmp edx, ebp
            jne L00503852
            inc ecx
            push 0
            push 0
            mov dword ptr [esi + 0x64], ecx
            mov ecx, dword ptr [esi + 0x24]
            push 0
            push ebp
            xor edx, edx
            call SendGameplay
            call GetTotalTime
            mov dword ptr [gLast_checkpoint_time], eax
            mov eax, dword ptr [gCurrent_net_game]
            mov ecx, dword ptr [eax + 0x60]
            mov eax, dword ptr [esi + 0x64]
            imul ecx, dword ptr [gCurrent_race+0x8]
            cmp eax, ecx
            jl L00503947
            sub esi, offset gNet_players
            mov eax, 0x4d4873ed
            imul esi
            sar edx, 6
            mov eax, edx
            mov ecx, edx
            shr eax, 0x1f
            add ecx, eax
            call DeclareWinner
            jmp L00503947
    L0050382e:
            idiv dword ptr [gCurrent_race+0x8]
            cmp edx, ebp
            jne L00503852
            inc ecx
            push 0
            push 0
            mov dword ptr [esi + 0x64], ecx
            mov ecx, dword ptr [esi + 0x24]
            push 0
            push ebp
            xor edx, edx
            call SendGameplay
            jmp L00503947
    L00503852:
            mov ecx, dword ptr [esi + 0x24]
            push 0
            push 0
            push 0
            push ebp
            mov edx, 1
            call SendGameplay
            jmp L00503947
    L0050386b:
            mov eax, dword ptr [gCheckpoint]
            lea esi, [ebp + 2]
            lea ecx, [esi - 1]
            cmp ecx, eax
            jne L00503884
            call IncrementCheckpoint
            jmp L00503947
    L00503884:
            mov eax, dword ptr [gRace_finished]
            test eax, eax
            jne L00503947
            mov edx, dword ptr [gCurrent_race+0x98]
            mov eax, dword ptr [edx + 0x210]
            test eax, eax
            je L005038aa
            cmp eax, 3
            jne L00503947
    L005038aa:
            cmp dword ptr [gLast_wrong_checkpoint], ebp
            jne L005038cc
            call GetTotalTime
            sub eax, dword ptr [gLast_checkpoint_time]
            cmp eax, 0x4e20
            ja L005038de
            cmp dword ptr [gLast_wrong_checkpoint], ebp
            je L00503947
    L005038cc:
            call GetTotalTime
            sub eax, dword ptr [gLast_checkpoint_time]
            cmp eax, 0x7d0
            jbe L00503947
    L005038de:
            mov eax, dword ptr [gNet_mode]
            test eax, eax
            jne L00503909
            cmp esi, dword ptr [gCurrent_race+0x8]
            jle L005038ff
            mov eax, dword ptr [gLap]
            dec eax
            neg eax
            sbb eax, eax
            and eax, 2
            dec eax
            jmp L00503901
    L005038ff:
            mov eax, esi
    L00503901:
            cmp eax, dword ptr [gCheckpoint]
            je L00503947
    L00503909:
            mov ecx, 0x2b
            call GetMiscString
            xor edx, edx
            mov ecx, 4
            push eax
            push -4
            push 0x3e8
            call NewTextHeadupSlot
            mov ecx, dword ptr [gPedestrians_outlet]
            mov edx, 0x1f4d
            call DRS3StartSound
            call GetTotalTime
            mov dword ptr [gLast_checkpoint_time], eax
            mov dword ptr [gLast_wrong_checkpoint], ebp
    L00503947:
            mov esi, dword ptr [esp + 0x14]
            mov eax, dword ptr [gCurrent_race+0x8]
            inc ebp
            add esi, 0x114
            cmp ebp, eax
            mov dword ptr [esp + 0x14], esi
            jl L0050359c
            mov ebx, dword ptr [esp + 0x28]
            mov eax, dword ptr [esp + 0x10]
    L0050396b:
            mov esi, dword ptr [eax + 0x10]
            lea edi, [eax + 0x1c]
            mov eax, dword ptr [esp + 0x18]
            add esi, 0x2c
            mov ecx, 0xc
            inc ebx
            rep movsd
            mov esi, dword ptr [esp + 0x1c]
            cmp ebx, eax
            mov dword ptr [esp + 0x28], ebx
            jl L00503522
    L00503990:
            mov eax, dword ptr [gNet_mode]
            inc esi
            cmp esi, eax
            mov dword ptr [esp + 0x1c], esi
            jle L005034f7
    L005039a2:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0xf0
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x005039b0
void C2_HOOK_FASTCALL TotalRepair(void) {

    TotallyRepairCar();
    NewTextHeadupSlot(4, 0, 1000, -4, GetMiscString(eMiscString_instant_repair));
}

// FUNCTION: CARMA2_HW 0x00503ba0
int C2_HOOK_FASTCALL PickNetRace(int pCurrent_race, tNet_sequence_type pNet_race_sequence) {
    int i;
    int new_index;
    int races_count;
    int most_seldom_seen;
    int races_to_pick_from[50];

    if (pNet_race_sequence == eNet_sequence_sequential) {
        pCurrent_race++;
        if (pCurrent_race >= gNumber_of_races) {
            pCurrent_race = 0;
        }
        return pCurrent_race;
    } else {
        most_seldom_seen = 10000;
        for (i = 0; i < gNumber_of_races; i++) {
            if (gRace_list[i].count_opponents < most_seldom_seen) {
                most_seldom_seen = gRace_list[i].count_opponents;
            }
        }
        races_count = 0;
        for (i = 0; i < gNumber_of_races; i++) {
            if (gRace_list[i].count_opponents == most_seldom_seen && i != pCurrent_race) {
                races_to_pick_from[races_count] = i;
                races_count++;
            }
        }
        new_index = races_to_pick_from[IRandomBetween(0, races_count - 1)];
        gRace_list[new_index].count_opponents++;
        return new_index;
    }
}

// FUNCTION: CARMA2_HW 0x00504170
void C2_HOOK_FASTCALL JumpTheStart(void) {
    char s[256];

    if (gNet_mode == eNet_mode_none || gProgram_state.credits >= gJump_start_fine[gProgram_state.skill_level]) {
        WakeUpOpponentsToTheFactThatTheStartHasBeenJumped(gCountdown);
        gCountdown = 0;
        DRS3StopOutletSound(gPedestrians_outlet);
        DRS3StartSound(gPedestrians_outlet, eSoundId_HeyYou);
        SpendCredits(gJump_start_fine[gProgram_state.skill_level]);
        sprintf(s, "%s %d %s",
            GetMiscString(eMiscString_bad_boy),
            gJump_start_fine[gProgram_state.skill_level],
            GetMiscString(eMiscString_credit_fine));
        NewTextHeadupSlot(4, 0, 1000, -4, s);
    }
}

// FUNCTION: CARMA2_HW 0x00504230
void C2_HOOK_FASTCALL GoingToInterfaceFromRace(void) {

    gInterface_within_race_mode = 1;
    gHud_tinted1_visible = !!TintedPolyIsOn(gHud_tinted1);
    gHud_tinted2_visible = !!TintedPolyIsOn(gHud_tinted2);
    TurnTintedPolyOff(gHud_tinted1);
    TurnTintedPolyOff(gHud_tinted2);
    PlayFlicsFromDisk();
    if (gNet_mode == eNet_mode_host) {
        SendGameplayToAllPlayers(eNet_gameplay_host_paused, 0, 0, 0, 0);
    }
}

// FUNCTION: CARMA2_HW 0x005042a0
void C2_HOOK_FASTCALL GoingBackToRaceFromInterface(void) {

    gInterface_within_race_mode = 0;
    if (gHud_tinted1_visible) {
        TurnTintedPolyOn(gHud_tinted1);
    }
    if (gHud_tinted2_visible) {
        TurnTintedPolyOn(gHud_tinted2);
    }
    SwitchToLoresMode();
    if (gNet_mode == eNet_mode_host) {
        SendGameplayToAllPlayers(eNet_gameplay_host_unpaused, 0, 0, 0, 0);
    }
}

// FUNCTION: CARMA2_HW 0x0049bcb0
void C2_HOOK_FASTCALL SendGameplayToHost(tNet_gameplay_mess pMess, int pParam_1, int pParam_2, int pParam_3, int pParam_4) {
    if (gNet_mode == 3) {
        tNet_message* message = NetBuildGuaranteedMessage(0x1b, 0);

        *(int *)((tU8 *)message + 0x24) = pParam_2;
        *(int *)((tU8 *)message + 0x1c) = pMess;
        *(int *)((tU8 *)message + 0x20) = pParam_1;
        *(int *)((tU8 *)message + 0x28) = pParam_3;
        *(int *)((tU8 *)message + 0x2c) = pParam_4;
        NetGuaranteedSendMessageToHost(gCurrent_net_game, message, 0);
    }
}
// NumberOfOpponentsLeft

// RaceCompleted

// Checkpoint

// IncrementCheckpoint

// IncrementLap

// RayHitFace

// WrongCheckpoint

// CheckCheckpoints

// TotalRepair

// DoLogos

// DoProgOpeningAnimation

// DoProgramDemo

// ChooseOpponent

// SelectOpponents

// PickNetRace

// SwapNetCarsLoad

// SwapNetCarsDispose

// DoGame

// InitialiseProgramState

void C2_HOOK_FASTCALL carpocalypse2_TestRenderTrack(void) {
    br_bounds bounds;
    br_vector3 center;
    br_vector3 eye;
    br_vector3 up;
    br_vector3 target;
    br_vector3 forward;
    br_vector3 right;
    br_vector3 temp;
    br_scalar half_width;
    br_scalar half_height;
    br_scalar half_depth;

    const char* tn = getenv("CARMA2_TRACK");
    if (!tn) tn = "NEWCITY2.TWT";
    LoadTrack(tn, &gProgram_state.track_spec, &gCurrent_race);

    BrActorToBounds(&bounds, gTrack_actor);

    center.v[0] = (bounds.min.v[0] + bounds.max.v[0]) * 0.5f;
    center.v[1] = (bounds.min.v[1] + bounds.max.v[1]) * 0.5f;
    center.v[2] = (bounds.min.v[2] + bounds.max.v[2]) * 0.5f;

    half_width = (bounds.max.v[0] - bounds.min.v[0]) * 0.5f;
    half_height = (bounds.max.v[1] - bounds.min.v[1]) * 0.5f;
    half_depth = (bounds.max.v[2] - bounds.min.v[2]) * 0.5f;

    if (half_width <= 0.5f && half_depth <= 0.5f && half_height <= 0.5f) {
        half_width = 2000.f;
        half_height = 2000.f;
        half_depth = 2000.f;
        center.v[0] = 0.f;
        center.v[1] = 0.f;
        center.v[2] = 0.f;
    }

    eye.v[0] = center.v[0];
    eye.v[1] = center.v[1] + half_height * 2.f + 200.f;
    eye.v[2] = center.v[2] + (half_depth * 2.f + half_width * 2.f) * 0.5f + 500.f;

    up.v[0] = 0.f;
    up.v[1] = 1.f;
    up.v[2] = 0.f;

    target.v[0] = center.v[0];
    target.v[1] = center.v[1] + half_height * 0.5f;
    target.v[2] = center.v[2];

    BrVector3Sub(&forward, &eye, &target);
    BrVector3Normalise(&forward, &forward);
    BrVector3Cross(&right, &up, &forward);
    BrVector3Normalise(&right, &right);
    BrVector3Cross(&temp, &forward, &right);

    gCamera->t.t.mat.m[0][0] = right.v[0];
    gCamera->t.t.mat.m[1][0] = right.v[1];
    gCamera->t.t.mat.m[2][0] = right.v[2];
    gCamera->t.t.mat.m[0][1] = temp.v[0];
    gCamera->t.t.mat.m[1][1] = temp.v[1];
    gCamera->t.t.mat.m[2][1] = temp.v[2];
    gCamera->t.t.mat.m[0][2] = forward.v[0];
    gCamera->t.t.mat.m[1][2] = forward.v[1];
    gCamera->t.t.mat.m[2][2] = forward.v[2];
    gCamera->t.t.translate.t.v[0] = eye.v[0];
    gCamera->t.t.translate.t.v[1] = eye.v[1];
    gCamera->t.t.translate.t.v[2] = eye.v[2];

    BrZbsSceneRender(gUniverse_actor, gCamera, gBack_screen, gDepth_buffer);
    PDScreenBufferSwap(0);

    BrActorRemove(gTrack_actor);
    ClearOutStorageSpace(&gTrack_storage_space);
    BrResFree(gTrack_actor);
}

void C2_HOOK_FASTCALL carpocalypse2_RaceSkeleton(void) {
    br_bounds bounds;
    br_vector3 center;
    br_vector3 eye;
    br_vector3 up;
    br_vector3 target;
    br_vector3 forward;
    br_vector3 right;
    br_vector3 temp;
    br_scalar half_width;
    br_scalar half_height;
    br_scalar half_depth;
    extern int carpocalypse2_MenuQuitRequested(void);

    const char* tn = getenv("CARMA2_TRACK");
    if (!tn) tn = "NEWCITY2.TWT";
    LoadTrack(tn, &gProgram_state.track_spec, &gCurrent_race);

    BrActorToBounds(&bounds, gTrack_actor);

    center.v[0] = (bounds.min.v[0] + bounds.max.v[0]) * 0.5f;
    center.v[1] = (bounds.min.v[1] + bounds.max.v[1]) * 0.5f;
    center.v[2] = (bounds.min.v[2] + bounds.max.v[2]) * 0.5f;

    half_width = (bounds.max.v[0] - bounds.min.v[0]) * 0.5f;
    half_height = (bounds.max.v[1] - bounds.min.v[1]) * 0.5f;
    half_depth = (bounds.max.v[2] - bounds.min.v[2]) * 0.5f;

    if (half_width <= 0.5f && half_depth <= 0.5f && half_height <= 0.5f) {
        half_width = 2000.f;
        half_height = 2000.f;
        half_depth = 2000.f;
        center.v[0] = 0.f;
        center.v[1] = 0.f;
        center.v[2] = 0.f;
    }

    eye.v[0] = center.v[0];
    eye.v[1] = center.v[1] + half_height * 2.f + 200.f;
    eye.v[2] = center.v[2] + (half_depth * 2.f + half_width * 2.f) * 0.5f + 500.f;

    up.v[0] = 0.f;
    up.v[1] = 1.f;
    up.v[2] = 0.f;

    target.v[0] = center.v[0];
    target.v[1] = center.v[1] + half_height * 0.5f;
    target.v[2] = center.v[2];

    BrVector3Sub(&forward, &eye, &target);
    BrVector3Normalise(&forward, &forward);
    BrVector3Cross(&right, &up, &forward);
    BrVector3Normalise(&right, &right);
    BrVector3Cross(&temp, &forward, &right);

    gCamera->t.t.mat.m[0][0] = right.v[0];
    gCamera->t.t.mat.m[1][0] = right.v[1];
    gCamera->t.t.mat.m[2][0] = right.v[2];
    gCamera->t.t.mat.m[0][1] = temp.v[0];
    gCamera->t.t.mat.m[1][1] = temp.v[1];
    gCamera->t.t.mat.m[2][1] = temp.v[2];
    gCamera->t.t.mat.m[0][2] = forward.v[0];
    gCamera->t.t.mat.m[1][2] = forward.v[1];
    gCamera->t.t.mat.m[2][2] = forward.v[2];
    gCamera->t.t.translate.t.v[0] = eye.v[0];
    gCamera->t.t.translate.t.v[1] = eye.v[1];
    gCamera->t.t.translate.t.v[2] = eye.v[2];

    {
        br_actor* direct;
        br_light* light;

        direct = BrActorAllocate(BR_ACTOR_LIGHT, NULL);
        light = direct->type_data;
        light->type = BR_LIGHT_DIRECT;
        light->colour = BR_COLOUR_RGB(255, 255, 255);
        light->attenuation_c = 1.0f;
        BrMatrix34RotateX(&direct->t.t.mat, BR_ANGLE_DEG(-60));
        BrMatrix34PostRotateY(&direct->t.t.mat, BR_ANGLE_DEG(30));
        BrActorAdd(gUniverse_actor, direct);
        BrLightEnable(direct);
    }

    LoadFont(1);

    gCar_to_view = &gProgram_state.current_car;
    gProgram_state.which_view = eView_forward;
    AboutToLoadFirstCar();
    LoadCar(gOpponents[0].car_file_name,
        eDriver_local_human,
        &gProgram_state.current_car,
        0,
        gProgram_state.player_name,
        &gOur_car_storage_space);
    SetCarStorageTexturingLevel(&gOur_car_storage_space, GetCarTexturingLevel(), eCTL_full);

    while (!carpocalypse2_MenuQuitRequested()) {
        PollKeys();
        if (PDAnyKeyDown() == 63) {
            break;
        }
        StartRenderingHeadups();
        gRender_screen->pixels = gBack_screen->pixels;
        BrPixelmapFill(gBack_screen, gGraf_specs[gGraf_spec_index].black_value);
        BrPixelmapFill(gDepth_buffer, 0xffffffff);
        BrZbsSceneRender(gUniverse_actor, gCamera, gBack_screen, gDepth_buffer);
        RenderTintedPolys();
        DimAFewBits();
        DoDamageScreen(GetTotalTime());
        DoHeadups(GetTotalTime());
        DoInstruments(GetTotalTime());
        DoTestHeadup();
        DoPSPowerupHeadups();
        StopRenderingHeadups();

        PDScreenBufferSwap(0);
    }

    BrActorRemove(gTrack_actor);
    ClearOutStorageSpace(&gTrack_storage_space);
    BrResFree(gTrack_actor);
}

// FUNCTION: CARMA2_HW 0x00503c50
void C2_HOOK_FASTCALL DoProgram(void) {
#ifdef CARPOCALYPSE2_MATCHING
    InitialiseProgramState();

    while (gProgram_state.prog_status != eProg_quit) {
        switch (gProgram_state.prog_status) {
        case eProg_intro:
            DisposeGameIfNecessary();
            DoLogos();
            break;
        case eProg_opening:
            DisposeGameIfNecessary();
            DoProgOpeningAnimation();
            break;
        case eProg_idling:
            DisposeGameIfNecessary();
            if (gNo_current_game) {
                LoadRaces(gRace_list, &gNumber_of_races, -1);
                InitGame(gDev_initial_race);
                MaybeRestoreSavedGame();
            }
            if (gAuto_load && gLoad_last_save_game) {
                DoLoadMostRecentGame();
                gLoad_last_save_game = 0;
            }
            DoMainScreen();
            break;
        case eProg_demo:
            DoProgramDemo();
            break;
        case eProg_game_starting:
            DoGame();
            break;
        default:
            break;
        }
    }
#else
    extern int DoMainScreen(void);

    if (getenv("CARMA2_TEST_TRACK")) {
        carpocalypse2_TestRenderTrack();
        gProgram_state.prog_status = eProg_quit;
        return;
    }

    /* InitialiseProgramState (inlined by retail compiler) */
    gProgram_state.prog_status = eProg_idling;
    gProgram_state.last_slot = 0;
    gProgram_state.racing = 0;
    gProgram_state.cut_scene = 0;
    gProgram_state.saving = 0;
    gProgram_state.loading = 0;
    gProgram_state.dont_save_or_load = 0;
    gProgram_state.dont_load = 0;
    gProgram_state.mirror_on = 1;
    gProgram_state.prat_cam_on = 1;
#ifndef CARPOCALYPSE2_MATCHING
    gProgram_state.cockpit_on = 0;
#else
    gProgram_state.cockpit_on = 1;
#endif
    gProgram_state.frank_or_anniness = eFrankie;

    do {
        switch (gProgram_state.prog_status) {
        case eProg_intro:
        case eProg_opening:
            gProgram_state.prog_status = eProg_idling;
            break;

        case eProg_idling:
            if (!DoMainScreen()) {
                gProgram_state.prog_status = eProg_quit;
            }
            break;

        case eProg_demo:
        case eProg_game_ongoing:
            gProgram_state.prog_status = eProg_quit;
            break;

        case eProg_game_starting:
            DoGame();
            break;

        default:
            break;
        }
    } while (gProgram_state.prog_status != eProg_quit);
#endif
}

// JumpTheStart

// GoingToInterfaceFromRace

// GoingBackToRaceFromInterface