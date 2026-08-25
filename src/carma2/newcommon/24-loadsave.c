#include "24-loadsave.h"

#include "platform.h"
#include "globvars.h"
#include "08-loading1.h"
#include "70-packfile.h"
#include "41-utility.h"
extern void C2_HOOK_FASTCALL SplungeSomeData(void* pData, size_t size);
#include "carpocalypse2_types.h"

// GLOBAL: CARMA2_HW 0x0068b8ec
int gSave_game_out_of_sync;

// GLOBAL: CARMA2_HW 0x0068c728
int gCount_saved_games;

int gFrontend_count_saved_games;

// GLOBAL: CARMA2_HW 0x0068c72c
tSave_game* gSaved_games;

// FUNCTION: CARMA2_HW 0x00500090
void C2_HOOK_FASTCALL Encryptificate(tSave_game* pSave_games, int pCount) {

    C2_HOOK_BUG_ON(sizeof(tSave_game) != 0x328);
    SplungeSomeData(pSave_games, pCount * sizeof(tSave_game));
}


// MakeSavedGame

// STUB: CARMA2_HW 0x00491ac0
void C2_HOOK_FASTCALL DoSaveGame(void) {
#ifndef CARPOCALYPSE2_MATCHING
    /* stub: no-op for Linux boot */
#else
    NOT_IMPLEMENTED();
#endif
}

// FUNCTION: CARMA2_HW 0x00491bb0
int C2_HOOK_FASTCALL StartSavedGamesList(void) {
    tPath_name path;
    FILE* f;
    int filesize;

    C2_HOOK_BUG_ON(sizeof(tSave_game) != 0x328);

    if (gSaved_games != NULL) {
        PDFatalError("Saved games list already started");
    }
    PathCat(path, gApplication_path, "SAVEDGAMES.ARS");
    f = DRfopen(path, "rb");
    if (f == NULL) {
        return 0;
    }
    PFfseek(f, 0, SEEK_END);
    filesize = PFftell(f);
    PFrewind(f);
    gCount_saved_games = filesize / sizeof(tSave_game);
    gSaved_games = BrMemAllocate(filesize, kMem_misc);
    PFfread(gSaved_games, 1, filesize, f);
    Encryptificate(gSaved_games, gCount_saved_games);
    PFfclose(f);
    return gCount_saved_games;
}

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

