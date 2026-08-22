#include "44-mainmenu.h"

#include "72-interface.h"
#include "72-interface-main.h"
#include "globvars.h"
#include <string.h>
#include "platform.h"

#ifdef CARPOCALYPSE2_MATCHING
#include "c2_hooks.h"
#else
extern void carpocalypse2_PresentFrame(void);
extern int carpocalypse2_ShouldQuit(void);
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
    int result;

    strcpy(gFrontend_MAIN.backdrop_name, "MAIN.BKD");
    result = FRONTEND_Main(&gFrontend_MAIN);

    switch (result) {
    case 0: /* stay in menu */
        return 1;
    case 1: /* start game */
        gProgram_state.prog_status = eProg_game_starting;
        return 1;
    default:
        gProgram_state.prog_status = eProg_quit;
        return 0;
    }
}

// DoOptionsMenu

