#include "racesumm.h"

#include "frontend.h"
#include "globvrpb.h"
#include "graphics.h"
#include "intrface.h"
#include "network.h"
#include "opponent.h"
#include "platform.h"
#include "sound.h"

#include "c2_stdlib.h"

// GLOBAL: CARMA2_HW 0x007051a0
int gPlayer_lookup_racesumm[14];

#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern tNet_game_player_info gNet_players[12];
#endif
// FUNCTION: CARMA2_HW 0x004e33a0
static int C2_NAKED C2_HOOK_CDECL SortScores(const void* pArg1, const void* pArg2) {
#ifndef CARPOCALYPSE2_MATCHING

    return gNet_players[*(int*)pArg2].score - gNet_players[*(int*)pArg1].score;
#else
    __asm {
            mov eax, dword ptr [esp + 8]
            mov eax, dword ptr [eax]
            lea ecx, [eax + eax*2]
            lea edx, [ecx + ecx*8]
            shl edx, 1
            sub edx, eax
            mov eax, dword ptr [esp + 4]
            mov eax, dword ptr [eax]
            lea ecx, [eax + eax*2]
            lea ecx, [ecx + ecx*8]
            shl ecx, 1
            sub ecx, eax
            mov eax, dword ptr [edx*4 + gNet_players+0x90]
            sub eax, dword ptr [ecx*4 + gNet_players+0x90]
            ret
            nop
    }
#endif
}

// SortGameScores

// DoNetRaceSummary

// FUNCTION: CARMA2_HW 0x004e32b0
void C2_HOOK_FASTCALL DoEndRaceSummary(tRace_result pRace_result, int pGame_completed) {
    int start_time;
    int i;
    int count;

    OriginalResetInterfaceTimeout();
    if (gNet_mode != eNet_mode_none) {
        NetPlayerStatusChanged(ePlayer_status_summary);
        start_time = PDGetTotalTime();
        while (!gReceived_game_scores && (int)PDGetTotalTime() - start_time < 20000) {
            NetService(0);
        }
        if (!gReceived_game_scores) {
            return;
        }
        for (i = 0; i < gNumber_of_net_players; i++) {
            gPlayer_lookup_racesumm[i] = i;
        }
        qsort(gPlayer_lookup_racesumm, gNumber_of_net_players, sizeof(int), SortScores);
        FrontEndProcessMenus(kFrontend_menu_networksummary);
        NetPlayerStatusChanged(ePlayer_status_loading);
        FadePaletteDown();
        return;
    }

    if (pGame_completed) {
        return;
    }

    count = GetCarCount(eVehicle_opponent);
    for (i = 0; i < count; i++) {
        if (GetCarSpec(eVehicle_opponent, i) != NULL) {
            StartMusicTrack(0x270e);
            FrontEndProcessMenus(kFrontend_menu_wrecks);
            return;
        }
    }
}

// FUNCTION: CARMA2_HW 0x004e33d0
int C2_HOOK_FASTCALL AnyBuyableCars(void) {
    int count;
    int i;

    count = GetCarCount(eVehicle_opponent);
    for (i = 0; i < count; i++) {
        if (GetCarSpec(eVehicle_opponent, i) != NULL) {
            return 1;
        }
    }
    return 0;
}

// RecordAPOAndCredits

// PickRaceWhenGameCompleted

// MungeRaceProgression
