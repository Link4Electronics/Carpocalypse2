#include "24-loadsave.h"

#include "carpocalypse2_types.h"

// GLOBAL: CARMA2_HW 0x0068b8ec
int gSave_game_out_of_sync;

// GLOBAL: CARMA2_HW 0x0068c728
int gCount_saved_games;

// GLOBAL: CARMA2_HW 0x0068c72c
tSave_game* gSaved_games;

// Encryptificate

// MakeSavedGame

// STUB: CARMA2_HW 0x00491ac0
void C2_HOOK_FASTCALL DoSaveGame(void) {
#ifndef CARPOCALYPSE2_MATCHING
    /* stub: no-op for Linux boot */
#else
    NOT_IMPLEMENTED();
#endif
}

// StartSavedGamesList

// FUNCTION: CARMA2_HW 0x00491c90
void C2_HOOK_FASTCALL EndSavedGamesList(void) {

    if (gSaved_games != NULL) {
        BrMemFree(gSaved_games);
    }
    gSaved_games = NULL;
}

// FUNCTION: CARMA2_HW 0x00491cb0
tSave_game* C2_HOOK_FASTCALL GetNthSavedGame(int pN) {
    if (gSaved_games == NULL || pN >= gCount_saved_games) {
        return NULL;
    }
    return &gSaved_games[gCount_saved_games - pN - 1];
}

// DoLoadGame2

// STUB: CARMA2_HW 0x00491e20
int C2_HOOK_FASTCALL DoLoadGame(int pIndex) {
    NOT_IMPLEMENTED();
}

// DoLoadMostRecentGame

