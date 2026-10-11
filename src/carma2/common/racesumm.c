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

// FUNCTION: CARMA2_HW 0x004e33a0
static int C2_HOOK_CDECL SortScores(const void* pArg1, const void* pArg2) {

    return gNet_players[*(int*)pArg2].score - gNet_players[*(int*)pArg1].score;
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

// AnyBuyableCars

// RecordAPOAndCredits

// PickRaceWhenGameCompleted

// MungeRaceProgression
