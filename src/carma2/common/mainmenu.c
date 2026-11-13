#include "mainmenu.h"

#include "frontend.h"
#include "flicplay.h"
#include "globvars.h"
#include "graphics.h"
#include "globvrpb.h"
#include "network.h"
#include "sound.h"
#include "mainmenu.h"

#include "interface.h"
#include "interface-main.h"
#include "globvars.h"
#include <string.h>
#include "platform.h"

#ifdef CARPOCALYPSE2_MATCHING
#include "c2_hooks.h"
#endif

// QuitVerifyStart

// QuitVerifyDone

#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_FASTCALL ClearEntireScreen(void);
extern int C2_HOOK_FASTCALL DRS3StopOutletSound(tS3_outlet* pOutlet);
extern int C2_HOOK_FASTCALL DoInterfaceScreen(const tInterface_spec* pSpec, int pOptions, int pCurrent_choice);
extern void C2_HOOK_FASTCALL EndMouseCursor(void);
extern void C2_HOOK_FASTCALL FadePaletteDown(void);
extern void C2_HOOK_FASTCALL GoingBackToRaceFromInterface(void);
extern void C2_HOOK_FASTCALL GoingToInterfaceFromRace(void);
extern int C2_HOOK_FASTCALL IRandomBetween(int pA, int pB);
extern void C2_HOOK_FASTCALL LoadInterfaceStuff(int pWithin_race);
extern void C2_HOOK_FASTCALL RemoveTransientBitmaps(int pGraphically_remove_them);
extern void C2_HOOK_FASTCALL StartMouseCursor(void);
extern void C2_HOOK_FASTCALL TurnOffPanelFlics(void);
extern int gAusterity_mode;
extern tS3_outlet* gEffects_outlet;
extern int gMouse_started;
extern tProgram_state gProgram_state;
extern char g_source_exists[];
#endif
// FUNCTION: CARMA2_HW 0x00494570
void C2_HOOK_FASTCALL DoOptionsMenu(void) {
    tPlayer_status player_status;

#ifdef CARPOCALYPSE2_FIX_BUGS
    player_status = ePlayer_status_unknown;
#endif

    if (gNet_mode != eNet_mode_none) {
        player_status = gNet_players[gThis_net_player_index].player_status;
        NetPlayerStatusChanged(ePlayer_status_main_menu);
    }
    if (FrontEndProcessMenus(kFrontend_menu_options) == 0) {
        gProgram_state.prog_status = eProg_quit;
    }
    if (gNet_mode != eNet_mode_none && gNet_players[gThis_net_player_index].player_status == ePlayer_status_main_menu) {
        NetPlayerStatusChanged(player_status);
    }
}
// GLOBAL: CARMA2_HW 0x0068c850
int gMouse_was_started__mainmenu;

// GLOBAL: CARMA2_HW 0x0068c854
int gReplace_background;

// FUNCTION: CARMA2_HW 0x00494400
int C2_HOOK_FASTCALL QuitVerifyDone(int pCurrent_choice, int pCurrent_mode, int pGo_ahead, int pEscaped, int pTimed_out) {
    (void)pCurrent_mode;
    (void)pGo_ahead;
    (void)pEscaped;
    (void)pTimed_out;

    FadePaletteDown();
    TurnOnPanelFlics();
    if (gMouse_was_started__mainmenu != 0) {
        RemoveTransientBitmaps(1);
    }
    if (gReplace_background != 0) {
        if (pCurrent_choice == 0) {
            ClearEntireScreen();
        }
        PDScreenBufferSwap(0);
        FadePaletteUp();
    }
    return pCurrent_choice == 0;
}

// FUNCTION: CARMA2_HW 0x00494450
int C2_NAKED C2_HOOK_FASTCALL DoVerifyQuit(int pReplace_background) {
#ifndef CARPOCALYPSE2_MATCHING
    NOT_IMPLEMENTED();
    return 1;
#else
    __asm {
            mov eax, dword ptr [gAusterity_mode]
            push esi
            xor esi, esi
            push edi
            test eax, eax
            mov edi, ecx
            je L00494467
            mov eax, 1
            pop edi
            pop esi
            ret
    L00494467:
            mov eax, dword ptr [gProgram_state+0x28]
            test eax, eax
            je L0049448e
            call FadePaletteDown
            call ClearEntireScreen
            call GoingToInterfaceFromRace
            mov esi, 1
            mov dword ptr [gProgram_state+0x28], 0
    L0049448e:
            mov edx, 3
            mov ecx, 1
            call IRandomBetween
            dec eax
            je L004944be
            dec eax
            je L004944b2
            dec eax
            jne L004944c8
            mov dword ptr [g_source_exists+0xc60], 0x84
            jmp L004944c8
    L004944b2:
            mov dword ptr [g_source_exists+0xc60], 0x83
            jmp L004944c8
    L004944be:
            mov dword ptr [g_source_exists+0xc60], 0x82
    L004944c8:
            mov eax, dword ptr [gMouse_started]
            test eax, eax
            mov dword ptr [gMouse_was_started__mainmenu], eax
            je L004944e5
            mov ecx, 1
            call RemoveTransientBitmaps
            call EndMouseCursor
    L004944e5:
            call TurnOffPanelFlics
            mov ecx, dword ptr [gEffects_outlet]
            mov dword ptr [gReplace_background], edi
            call DRS3StopOutletSound
            mov ecx, esi
            call LoadInterfaceStuff
            xor edx, edx
            mov ecx, offset g_source_exists+0xc5c
            push 0
            call DoInterfaceScreen
            mov edi, eax
            mov eax, dword ptr [gMouse_was_started__mainmenu]
            test eax, eax
            je L00494527
            mov ecx, esi
            call LoadInterfaceStuff
            call StartMouseCursor
    L00494527:
            test esi, esi
            je L0049453a
            call GoingBackToRaceFromInterface
            mov dword ptr [gProgram_state+0x28], 1
    L0049453a:
            mov eax, edi
            pop edi
            pop esi
            ret
    }
#endif
}

#ifdef CARPOCALYPSE2_MATCHING
// FUNCTION: CARMA2_HW 0x00494540
int C2_HOOK_FASTCALL DoMainScreen(void) {
    int result;

    StartMusicTrack(0x270e);

    result = FrontEndProcessMenus(kFrontend_menu_main);

    switch (result) {
    case 0:
        gProgram_state.prog_status = eProg_quit;
        break;
    case 1:
        gProgram_state.prog_status = eProg_game_starting;
        break;
    }
}
#else
int C2_HOOK_FASTCALL DoMainScreen(void) {
    int result;

    result = FrontEndProcessMenus(kFrontend_menu_main);

    if (result < 0) {
        gProgram_state.prog_status = eProg_quit;
        return 0;
    }
    if (result > 0) {
        gProgram_state.prog_status = eProg_game_starting;
    }
    return 1;
}
#endif

// DoOptionsMenu