#ifndef GUARD_LOADSAVE_H
#define GUARD_LOADSAVE_H

#include "c2_hooks.h"
#include "carpocalypse2_types.h"

extern int gSave_game_out_of_sync;

// Encryptificate

// MakeSavedGame

extern void C2_HOOK_FASTCALL DoSaveGame(void);

// StartSavedGamesList

extern void C2_HOOK_FASTCALL EndSavedGamesList(void);

extern tSave_game* C2_HOOK_FASTCALL GetNthSavedGame(int pN);

// DoLoadGame2

extern int C2_HOOK_FASTCALL DoLoadGame(int pIndex);

// DoLoadMostRecentGame

#define CARPOCALYPSE2_LOADSAVE_H

#include "c2_hooks.h"

#include "carpocalypse2_types.h"


extern int gValid_stashed_save_game;

extern tSave_game gStashed_save_game;

extern int gCount_saved_games;

extern tSave_game* gSaved_games;

void C2_HOOK_FASTCALL RestoreSinglePlayerState(void);

void C2_HOOK_FASTCALL MaybeRestoreSavedGame(void);

int C2_HOOK_FASTCALL DoLoadGame2(tSave_game* pSave_game);

void C2_HOOK_FASTCALL DoLoadMostRecentGame(void);

void C2_HOOK_FASTCALL SplungeSomeData(void* pData, br_size_t size);

void C2_HOOK_FASTCALL Encryptificate(tSave_game* pSave_games, int pCount);

int C2_HOOK_FASTCALL StartSavedGamesList(void);

void C2_HOOK_FASTCALL MakeSavedGame(tSave_game* pSave_game);

#endif // GUARD_LOADSAVE_H
