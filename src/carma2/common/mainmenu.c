#include "mainmenu.h"

#include "frontend.h"
#include "globvars.h"
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

// STUB: CARMA2_HW 0x00494450
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
    if (FRONTEND_Main(kFrontend_menu_options) == 0) {
        gProgram_state.prog_status = eProg_quit;
    }
    if (gNet_mode != eNet_mode_none && gNet_players[gThis_net_player_index].player_status == ePlayer_status_main_menu) {
        NetPlayerStatusChanged(player_status);
    }
}
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

    result = FRONTEND_Main(kFrontend_menu_main);

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