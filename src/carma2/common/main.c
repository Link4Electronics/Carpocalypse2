#include "main.h"

#include "utility.h"

#include "carpocalypse2_types.h"

#include "controls.h"
#include "drmem.h"
#include "errors.h"
#include "globvars.h"
#include "globvrpb.h"
#include "graphics.h"
#include "init.h"
#include "input.h"
#include "loading.h"
#include "loadsave.h"
#include "network.h"
#include "physics.h"
#include "replay.h"
#include "sound.h"
#include "structur.h"
#include "timers.h"
#include "utility.h"
#include "video.h"
#include "world.h"

#include "platform.h"

#include <s3/s3.h>

#include "c2_stdlib.h"
#include "c2_string.h"
#include "main.h"

#include "globvars.h"
#include "globvrpb.h"
#include "platform.h"
#include "carpocalypse2_types.h"
#include "network.h"
#include "loading1.h"
#include "utility.h"
#include "errors.h"
#include "sound.h"
#include "init.h"
#include "structur.h"
#include "loading2.h"
#include "loadsave.h"
#include "movie.h"
#include "phil.h"
#include "world3.h"
#include "world1.h"
#include "graphics2.h"

#include <string.h>
#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_FASTCALL AutoSaveAdditionalStuff(void);
extern void C2_HOOK_FASTCALL CheckKevKeys(void);
extern void C2_HOOK_FASTCALL CheckMemory(void);
extern void C2_HOOK_FASTCALL ClearEntireScreen(void);
extern void C2_HOOK_FASTCALL CloseDiagnostics(void);
extern br_error C2_HOOK_FASTCALL DRBrEnd(void);
extern void C2_HOOK_FASTCALL DRS3ShutDown(void);
extern void C2_HOOK_FASTCALL DoSaveGame(void);
extern int C2_HOOK_FASTCALL MovieStopRecordingIfNecessary(void);
extern void C2_HOOK_FASTCALL NetLeaveGame(tNet_game_details* pNet_game);
extern void C2_HOOK_FASTCALL NetService(int pIn_race);
extern int C2_HOOK_FASTCALL PDGetMicroseconds(void);
extern void C2_HOOK_FASTCALL PDRevertPalette(void);
extern int C2_HOOK_FASTCALL PDServiceSystem(tU32 pTime_since_last_call);
extern C2_NORETURN void C2_HOOK_FASTCALL PDShutdownSystem(void);
extern void C2_HOOK_FASTCALL PHILDisable(void);
extern void C2_HOOK_FASTCALL PollKeys(void);
extern int C2_HOOK_FASTCALL SaveOptions(void);
extern void C2_HOOK_FASTCALL ShutdownNetIfRequired(void);
extern void C2_HOOK_FASTCALL StopMusic(void);
extern void C2_HOOK_CDECL dr_dprintf();
extern br_actor* gAdditional_actors;
extern int gBr_initialized;
extern char gCurrent_dropped_actor_index[];
extern tNet_game_details* gCurrent_net_game;
extern tU32 gFrame_period;
extern tNet_mode gNet_mode;
extern tProgram_state gProgram_state;
extern int gSave_game_out_of_sync;
extern int gSound_available;
extern tTimer gTimers[19];
extern int gTimers_max_index;
extern int gTimers_stack[19];
extern int gTimers_stack_size;
extern char g_source_exists[];
#endif
// FUNCTION: CARMA2_HW 0x00492050
void C2_HOOK_FASTCALL ServiceTheGame(int pRacing) {

    CheckMemory();
    if (!pRacing) {
        CyclePollKeys();
    }
    PollKeys();
    rand();
    if (PDServiceSystem(gFrame_period)) {
        QuitGame();
    }
    if (!pRacing) {
        CheckSystemKeys(0);
    }
    if (!pRacing && gSound_enabled) {
        SoundService();
    }
    NetService(pRacing);
}

// FUNCTION: CARMA2_HW 0x004922a0
void C2_NAKED C2_HOOK_FASTCALL ServiceGameInRace(void) {
#ifndef CARPOCALYPSE2_MATCHING

    Timers_Push(TIMER_SER);
    ServiceTheGame(1);
    CheckKevKeys();
    Timers_Pop(TIMER_SER);
#else
    __asm {
            mov eax, dword ptr [gTimers_stack_size]
            push esi
            test eax, eax
            push edi
            mov dword ptr [eax*4 + gTimers_stack], 0x11
            je L004922da
            mov eax, dword ptr [eax*4 + gCurrent_dropped_actor_index+0x18]
            lea esi, [eax*8]
            lea edx, [eax*8]
            sub esi, eax
            shl esi, 2
            xor ecx, ecx
            mov cl, byte ptr [esi + gTimers+0x8]
            jmp L004922f6
    L004922da:
            mov eax, dword ptr [gTimers_max_index]
            xor ecx, ecx
            mov esi, eax
            mov edx, eax
            shl esi, 3
            sub esi, eax
            shl esi, 2
            shl edx, 3
            mov cl, byte ptr [esi + gTimers+0x8]
    L004922f6:
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
            mov edx, dword ptr [gTimers_stack_size]
            mov dword ptr [gTimers+0x1e8], eax
            inc edx
            mov dword ptr [gTimers_stack_size], edx
            call CheckMemory
            call PollKeys
            call rand
            mov ecx, dword ptr [gFrame_period]
            call PDServiceSystem
            test eax, eax
            je L0049241f
            call SaveOptions
            mov eax, dword ptr [gSave_game_out_of_sync]
            test eax, eax
            je L00492360
            call DoSaveGame
    L00492360:
            call MovieStopRecordingIfNecessary
            call PHILDisable
            push offset g_source_exists+0x198
            call dr_dprintf
            mov eax, dword ptr [gProgram_state+0x28]
            add esp, 4
            test eax, eax
            je L004923af
            mov eax, dword ptr [gAdditional_actors]
            test eax, eax
            je L004923af
            mov ecx, dword ptr [eax + 8]
            test ecx, ecx
            je L004923af
            push offset g_source_exists+0x180
            call dr_dprintf
            add esp, 4
            call AutoSaveAdditionalStuff
            push offset g_source_exists+0x168
            call dr_dprintf
            add esp, 4
    L004923af:
            mov dword ptr [gProgram_state+0x28], 0
            call SaveOptions
            mov eax, dword ptr [gNet_mode]
            test eax, eax
            je L004923d2
            mov ecx, dword ptr [gCurrent_net_game]
            call NetLeaveGame
    L004923d2:
            call ShutdownNetIfRequired
            mov eax, dword ptr [gSound_available]
            test eax, eax
            je L004923e5
            call DRS3ShutDown
    L004923e5:
            mov eax, dword ptr [gBr_initialized]
            test eax, eax
            je L004923f3
            call ClearEntireScreen
    L004923f3:
            call PDRevertPalette
            call StopMusic
            mov eax, dword ptr [gBr_initialized]
            test eax, eax
            je L0049240b
            call DRBrEnd
    L0049240b:
            call PDShutdownSystem
            call CloseDiagnostics
            push 0
            call exit
            add esp, 4
    L0049241f:
            mov ecx, 1
            call NetService
            call CheckKevKeys
            mov eax, dword ptr [gTimers+0x1e4]
            and eax, 0xff
            lea esi, [eax*4 + gTimers+0x1e0]
            call PDGetMicroseconds
            mov edx, dword ptr [gTimers+0x1e8]
            mov ecx, dword ptr [esi]
            sub eax, edx
            add ecx, eax
            mov eax, dword ptr [gTimers_stack_size]
            cmp eax, 1
            mov dword ptr [esi], ecx
            jle L00492470
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_stack_size]
            mov ecx, dword ptr [ecx*4 + gCurrent_dropped_actor_index+0x18]
            jmp L0049247b
    L00492470:
            call PDGetMicroseconds
            mov ecx, dword ptr [gTimers_max_index]
    L0049247b:
            mov edx, ecx
            pop edi
            shl edx, 3
            sub edx, ecx
            pop esi
            mov dword ptr [edx*4 + gTimers+0xc], eax
            mov eax, dword ptr [gTimers_stack_size]
            dec eax
            mov dword ptr [gTimers_stack_size], eax
            ret
    }
#endif
}
// FUNCTION: CARMA2_HW 0x00491f70
C2_NORETURN void C2_HOOK_FASTCALL QuitGame(void) {
    SaveOptions();
    if (gSave_game_out_of_sync) {
        DoSaveGame();
    }
    MovieStopRecordingIfNecessary();
    PHILDisable();
    dr_dprintf("QuitGame() - Point 1");
    if (gProgram_state.racing && gAdditional_actors != NULL && gAdditional_actors->children != NULL) {
        dr_dprintf("QuitGame() - Point 2");
        AutoSaveAdditionalStuff();
        dr_dprintf("QuitGame() - Point 3");
    }
    gProgram_state.racing = 0;
    SaveOptions();
    if (gNet_mode != eNet_mode_none) {
        NetLeaveGame(gCurrent_net_game);
    }
    ShutdownNetIfRequired();
    if (gSound_available) {
        DRS3ShutDown();
    }
    if (gBr_initialized) {
        ClearEntireScreen();
    }
    PDRevertPalette();
    StopMusic();
    if (gBr_initialized) {
        DRBrEnd();
    }
    PDShutdownSystem();
    CloseDiagnostics();
    exit(0);
}

// ServiceTheGame

// FUNCTION: CARMA2_HW 0x00492180
void C2_HOOK_FASTCALL ServiceGame(void) {

    ServiceTheGame(0);
}

// ServiceGameInRace

// FUNCTION: CARMA2_HW 0x004924a0
void C2_HOOK_FASTCALL GameMain(int pArgc, const char** pArgv) {
    tPath_name location;

    PDSetFileVariables();
    PDBuildAppPath(gApplication_path);
    OpenDiagnostics();

    strcat(gApplication_path, "DATA");
    UsePathFileToDetermineIfFullInstallation();
    if (!gCD_fully_installed) {
        if (GetRegisterSourceLocation(location)) {
            if (!PDCheckDriveExists(location)) {
                PDInitialiseSystem();
                PDFatalError("Can't find the Carmageddon CD\n");
            }
        }
    }
    InitialiseDeathRace(pArgc, pArgv);
    DoProgram();
    QuitGame();
}