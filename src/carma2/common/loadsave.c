#include "loadsave.h"

#include "globvars.h"
#include "globvrpb.h"
#include "loading.h"
#include "platform.h"
#include "utility.h"

#include "carpocalypse2_macros.h"

#include "c2_string.h"
#include "loadsave.h"

#include "platform.h"
#include "globvars.h"
#include "loading1.h"
#include "packfile.h"
#include "utility.h"
extern void C2_HOOK_FASTCALL SplungeSomeData(void* pData, size_t size);
// GLOBAL: CARMA2_HW 0x00688780
tSave_game gStashed_save_game;

// GLOBAL: CARMA2_HW 0x0068b8ec
int gSave_game_out_of_sync;

// GLOBAL: CARMA2_HW 0x0068c728
int gCount_saved_games;

int gFrontend_count_saved_games;

// GLOBAL: CARMA2_HW 0x0068c72c
tSave_game* gSaved_games;

void C2_HOOK_FASTCALL RestoreSinglePlayerState(void) {

    LoadRaces(gRace_list, &gNumber_of_races, -1);
    DoLoadGame2(&gStashed_save_game);
    gValid_stashed_save_game = 0;
}

#ifdef CARPOCALYPSE2_MATCHING
extern void* C2_HOOK_CDECL BrMemAllocate();
extern FILE* C2_HOOK_FASTCALL DRfopen(const char* pFilename, const char* pMode);
extern void C2_NORETURN C2_HOOK_FASTCALL PDFatalError(const char* pThe_str);
extern void C2_HOOK_FASTCALL PDGetCurrentDate(char* pTimeStr);
extern void C2_HOOK_FASTCALL PDGetCurrentTime(char* pTimeStr);
extern int C2_HOOK_FASTCALL PFfclose(FILE* pFile);
extern br_size_t C2_HOOK_FASTCALL PFfread(void* buf, br_size_t size, unsigned int n, void* f);
extern int C2_HOOK_FASTCALL PFfseek(FILE* pF, int offset, int whence);
extern int C2_HOOK_FASTCALL PFftell(FILE* pF);
extern void C2_HOOK_FASTCALL PFrewind(FILE* pF);
extern void C2_HOOK_FASTCALL PathCat(char* pDestn_str, const char* pStr_1, const char* pStr_2);
extern tPath_name gApplication_path;
extern br_matrix34 gCamera_to_world;
extern int gCancel_recover;
extern int gCurrent_APO_levels[3];
extern int gIs_boundary_race;
extern int gNumber_of_races;
extern tProgram_state gProgram_state;
extern tRace_list_spec gRace_list[100];
extern char g_source_exists[];
#endif
// FUNCTION: CARMA2_HW 0x00466e90
void C2_HOOK_FASTCALL MaybeRestoreSavedGame(void) {

    if (gValid_stashed_save_game) {
        RestoreSinglePlayerState();
    }
}

// FUNCTION: CARMA2_HW 0x00491ce0
int C2_HOOK_FASTCALL DoLoadGame2(tSave_game* pSave_game) {
    int i;

    C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(gProgram_state.cars_available) != 60);
    C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(gCurrent_APO_levels) != 3);

    if (pSave_game->magic != 0x12345678) {
        return 0;
    }
    gProgram_state.skill_level = pSave_game->skill_level;
    gProgram_state.game_completed = pSave_game->game_completed;
    gProgram_state.current_race_index = pSave_game->current_race_index;
    gCurrent_race_group = gRace_list[gProgram_state.current_race_index].group;
    gIs_boundary_race = pSave_game->is_boundary_race;
    gProgram_state.credits = pSave_game->credits;
    strcpy(gProgram_state.car_name, pSave_game->car_name);
    strcpy(gProgram_state.player_name, pSave_game->player_name);
    for (i = 0; i < gNumber_of_races; i++) {
        gRace_list[i].count_opponents = pSave_game->races_finished[i];
    }
    gProgram_state.number_of_cars = pSave_game->number_of_cars;
    gProgram_state.current_car_index = pSave_game->current_car_index;
    memcpy(gProgram_state.cars_available, pSave_game->cars, sizeof(gProgram_state.cars_available));
    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gCurrent_APO_levels); i++) {
        gCurrent_APO_levels[i] = gProgram_state.current_car.power_up_levels[i] = pSave_game->apo_levels[i];
        gCurrent_APO_potential_levels[i] = gProgram_state.current_car.power_up_slots[i] = pSave_game->apo_potential[i];
    }
    gProgram_state.field_0x2c = 1;
    return 1;
}

// FUNCTION: CARMA2_HW 0x00491e50
void C2_HOOK_FASTCALL DoLoadMostRecentGame(void) {

    if (StartSavedGamesList()) {
        DoLoadGame(0);
        EndSavedGamesList();
    }
}

// FUNCTION: CARMA2_HW 0x004919a0
void C2_NAKED C2_HOOK_FASTCALL MakeSavedGame(tSave_game* pSave_game) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;

    C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(gProgram_state.cars_available) != 60);

    pSave_game->magic = 0x12345678;
    pSave_game->skill_level = gProgram_state.skill_level;
    pSave_game->game_completed = gProgram_state.game_completed;
    pSave_game->current_race_index = gProgram_state.current_race_index;
    pSave_game->is_boundary_race = gIs_boundary_race;
    pSave_game->credits = gProgram_state.credits;
    strcpy(pSave_game->car_name, gProgram_state.car_name);
    strcpy(pSave_game->player_name, gProgram_state.player_name);
    PDGetCurrentTime(pSave_game->time);
    PDGetCurrentDate(pSave_game->date);
    for (i = 0; i < gNumber_of_races; i++) {
        pSave_game->races_finished[i] = gRace_list[i].count_opponents;
    }
    pSave_game->number_of_cars = gProgram_state.number_of_cars;
    pSave_game->current_car_index = gProgram_state.current_car_index;
    for (i = 0; i < CARPOCALYPSE2_ASIZE(gProgram_state.cars_available); i++) {
        pSave_game->cars[i] = gProgram_state.cars_available[i];
    }
    for (i = 0; i < 3; i++) {
        pSave_game->apo_levels[i] = gCurrent_APO_levels[i];
        pSave_game->apo_potential[i] = gCurrent_APO_potential_levels[i];
    }
#else
    __asm {
            mov eax, dword ptr [gProgram_state+0x20]
            push ebx
            mov ebx, ecx
            push esi
            push edi
            mov edi, offset gProgram_state+0x1a5e
            mov dword ptr [ebx + 0x208], eax
            mov dword ptr [ebx], 0x12345678
            mov ecx, dword ptr [gProgram_state+0x84]
            mov dword ptr [ebx + 0x20c], ecx
            mov edx, dword ptr [gProgram_state+0x78]
            mov dword ptr [ebx + 0x308], edx
            mov eax, dword ptr [gIs_boundary_race]
            mov dword ptr [ebx + 0x30c], eax
            mov ecx, dword ptr [gProgram_state]
            mov dword ptr [ebx + 0x204], ecx
            or ecx, 0xffffffff
            xor eax, eax
            lea edx, [ebx + 0x12]
            repne scasb
            not ecx
            sub edi, ecx
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            lea edx, [ebx + 4]
            shr ecx, 2
            rep movsd
            mov ecx, eax
            xor eax, eax
            and ecx, 3
            rep movsb
            mov edi, offset gProgram_state+0x1a10
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            shr ecx, 2
            rep movsd
            mov ecx, eax
            and ecx, 3
            rep movsb
            lea ecx, [ebx + 0x52]
            call PDGetCurrentTime
            lea ecx, [ebx + 0x32]
            call PDGetCurrentDate
            mov ecx, dword ptr [gNumber_of_races]
            xor eax, eax
            test ecx, ecx
            jle L00491a6b
            mov edx, offset gRace_list+0x184
            lea ecx, [ebx + 0x74]
    L00491a53:
            mov esi, dword ptr [edx]
            inc eax
            mov dword ptr [ecx], esi
            mov esi, dword ptr [gNumber_of_races]
            add ecx, 4
            add edx, 0x244
            cmp eax, esi
            jl L00491a53
    L00491a6b:
            mov ecx, dword ptr [gProgram_state+0x88]
            lea edi, [ebx + 0x214]
            mov dword ptr [ebx + 0x210], ecx
            mov edx, dword ptr [gProgram_state+0x8c]
            mov ecx, 0x3c
            mov esi, offset gProgram_state+0x1aa0
            mov dword ptr [ebx + 0x304], edx
            xor eax, eax
            rep movsd
            lea ecx, [ebx + 0x31c]
    L00491a9d:
            mov edx, dword ptr [eax + gCurrent_APO_levels]
            add eax, 4
            mov dword ptr [ecx - 0xc], edx
            mov edx, dword ptr [eax + gCamera_to_world+0x2c]
            mov dword ptr [ecx], edx
            add ecx, 4
            cmp eax, 0xc
            jl L00491a9d
            pop edi
            pop esi
            pop ebx
            ret
            nop
            nop
            nop
    }
#endif
}
#include "carpocalypse2_types.h"

static __inline void C2_HOOK_FASTCALL Encryptificate(tSave_game* pSave_games, int pCount) {

    C2_HOOK_BUG_ON(sizeof(tSave_game) != 0x328);
    SplungeSomeData(pSave_games, pCount * sizeof(tSave_game));
}


// MakeSavedGame

// FUNCTION: CARMA2_HW 0x00491ac0
void C2_HOOK_FASTCALL DoSaveGame(void) {
    tPath_name the_path;
    tSave_game save_game;
    FILE* f;
    int i;

    C2_HOOK_BUG_ON(sizeof(tSave_game) != 0x328);

    if (gNet_mode != eNet_mode_none) {
        return;
    }
    PathCat(the_path, gApplication_path, "SAVEDGAMES.ARS");
    PDFileUnlock(the_path);
    f = DRfopen(the_path, "ab+");
    if (f == NULL) {
        return;
    }
    PFfseek(f, 0, SEEK_END);
    for (i = 0; i < 3; i++) {
        gCurrent_APO_potential_levels[i] = gProgram_state.current_car.power_up_slots[i];
    }
    for (i = 0; i < 3; i++) {
        gCurrent_APO_levels[i] = gProgram_state.current_car.power_up_levels[i];
    }
    MakeSavedGame(&save_game);
    Encryptificate(&save_game, 1);
    PFfwrite(&save_game, 1, sizeof(save_game), f);
    PFfclose(f);
    gSave_game_out_of_sync = 0;
}

// FUNCTION: CARMA2_HW 0x00491bb0
int C2_NAKED C2_HOOK_FASTCALL StartSavedGamesList(void) {
#ifndef CARPOCALYPSE2_MATCHING
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
#else
    __asm {
            mov eax, dword ptr [gSaved_games]
            sub esp, 0x100
            test eax, eax
            push esi
            push edi
            je L00491bcb
            mov ecx, offset g_source_exists+0x144
            call PDFatalError
    L00491bcb:
            push offset g_source_exists+0x134
            mov edx, offset gApplication_path
            lea ecx, [esp + 0xc]
            call PathCat
            mov edx, offset gCancel_recover+0x1a4
            lea ecx, [esp + 8]
            call DRfopen
            mov esi, eax
            test esi, esi
            je L00491c7a
            push 2
            xor edx, edx
            mov ecx, esi
            call PFfseek
            mov ecx, esi
            call PFftell
            mov ecx, esi
            mov edi, eax
            call PFrewind
            mov eax, 0x446f8657
            mul edi
            mov eax, edi
            push 0xfd
            sub eax, edx
            push edi
            shr eax, 1
            add eax, edx
            shr eax, 9
            mov dword ptr [gCount_saved_games], eax
            call BrMemAllocate
            add esp, 8
            mov edx, 1
            mov ecx, eax
            mov dword ptr [gSaved_games], eax
            push esi
            push edi
            call PFfread
            mov eax, dword ptr [gCount_saved_games]
            lea ecx, [eax + eax*4]
            lea ecx, [ecx + ecx*4]
            lea edx, [eax + ecx*4]
            mov ecx, dword ptr [gSaved_games]
            shl edx, 3
            call SplungeSomeData
            mov ecx, esi
            call PFfclose
            mov eax, dword ptr [gCount_saved_games]
            pop edi
            pop esi
            add esp, 0x100
            ret
    L00491c7a:
            pop edi
            xor eax, eax
            pop esi
            add esp, 0x100
            ret
    }
#endif
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

// FUNCTION: CARMA2_HW 0x00491e20
int C2_HOOK_FASTCALL DoLoadGame(int pIndex) {
    if (gSaved_games != NULL && pIndex >= 0 && pIndex < gCount_saved_games) {
        return DoLoadGame2(&gSaved_games[gCount_saved_games - pIndex - 1]);
    }
    return 0;
}

// DoLoadMostRecentGame
