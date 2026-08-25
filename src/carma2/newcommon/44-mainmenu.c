#include "44-mainmenu.h"

#include "72-interface.h"
#include "72-interface-main.h"
#include "globvars.h"
#include <string.h>
#include "platform.h"

#ifdef CARPOCALYPSE2_MATCHING
#include "c2_hooks.h"
#endif

// QuitVerifyStart

// QuitVerifyDone

// STUB: CARMA2_HW 0x00494450
int C2_HOOK_FASTCALL DoVerifyQuit(int pReplace_background) {
    NOT_IMPLEMENTED();
    return 1;
}

// FUNCTION: CARMA2_HW 0x00494540
int C2_HOOK_FASTCALL DoMainScreen(void) {
#ifdef CARPOCALYPSE2_MATCHING
    NOT_IMPLEMENTED();
    return 0;
#else
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
#endif
}

// DoOptionsMenu

