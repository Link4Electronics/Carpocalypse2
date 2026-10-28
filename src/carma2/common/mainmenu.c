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
int C2_HOOK_FASTCALL DoVerifyQuit(int pReplace_background) {
    NOT_IMPLEMENTED();
    return 1;
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