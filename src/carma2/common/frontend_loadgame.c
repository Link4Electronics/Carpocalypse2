#include "frontend_loadgame.h"

#include "frontend.h"
#include "frontend_main.h"
#include "frontend_network.h"
#include "frontend_options.h"
#include "frontend_quit.h"
#include "globvars.h"
#include "graphics.h"
#include "input.h"
#include "loading.h"
#include "loadsave.h"
#include "main.h"
#include "platform.h"
#include "polyfont.h"
#include "sound.h"
#include "utility.h"

#include "carpocalypse2_macros.h"

#include <ctype.h>
#include "c2_string.h"
static int LoadSlotN(tFrontend_spec* pFrontend, int pN) {

    if (gFrontend_load_game_index_top + pN + 1 > gFrontend_count_saved_games) {
        pFrontend->items[pN + 1].menuInfo = NULL;
        return 0;
    }
    TryToLoadGame(pN);
    if (gProgram_state.racing) {
        return 1;
    }
    gFrontend_next_menu = kFrontend_menu_main;
    return 3;
}