#include "frontend.h"

#include "drmem.h"
#include "errors.h"
#include "font.h"
#include "frontend_controls.h"
#include "frontend_credits.h"
#include "frontend_loadgame.h"
#include "frontend_main.h"
#include "frontend_netsync.h"
#include "frontend_network.h"
#include "frontend_networksummary.h"
#include "frontend_newgame.h"
#include "frontend_options.h"
#include "frontend_quit.h"
#include "frontend_startgame.h"
#include "frontend_wrecks.h"
#include "globvars.h"
#include "graphics.h"
#include "init.h"
#include "input.h"
#include "intrface.h"
#include "loading.h"
#include "main.h"
#include "options.h"
#include "polyfont.h"
#include "platform.h"
#include "sound.h"
#include "tinted.h"
#include "utility.h"

#include <brender/brender.h>

#include "carpocalypse2_macros.h"
#include "c2_string.h"
#include "brender/br_types.h"
#define COUNT_FRONTEND_INTERPOLATE_STEPS 16
void C2_HOOK_FASTCALL LoadMenuModels(void);
int C2_HOOK_FASTCALL Frontend_FindNextVisibleItem(tFrontend_spec* pFrontend, int pIndex);
// GLOBAL: CARMA2_HW 0x0068c6e0
extern int gCount_interface_strings;
// GLOBAL: CARMA2_HW 0x0068c230
extern char* gInterface_strings[300];
// GLOBAL: CARMA2_HW 0x00686850
extern br_pixelmap* gFrontend_images[100];
// GLOBAL: CARMA2_HW 0x006870b8
extern tFrontend_model gFrontend_A_models[13];
// GLOBAL: CARMA2_HW 0x00687188
extern tFrontend_model gFrontend_B_models[11];
// GLOBAL: CARMA2_HW 0x00687058
extern tFrontend_model gFrontend_C_models[6];
// GLOBAL: CARMA2_HW 0x0059b0d0
extern int gFrontend_stuff_not_loaded;
// GLOBAL: CARMA2_HW 0x00688378
extern br_model* gFrontend_A_model_from;
// GLOBAL: CARMA2_HW 0x0068844c
extern br_model* gFrontend_A_model_to;
// GLOBAL: CARMA2_HW 0x00688768
extern br_model* gFrontend_B_model_from;
// GLOBAL: CARMA2_HW 0x006886e0
extern br_model* gFrontend_B_model_to;
// GLOBAL: CARMA2_HW 0x00686504
extern br_model* gFrontend_C_model_from;
// GLOBAL: CARMA2_HW 0x006886bc
extern br_model* gFrontend_C_model_to;
// GLOBAL: CARMA2_HW 0x006864e0
extern int gFrontend_backdrop0_opacity_mode;
// GLOBAL: CARMA2_HW 0x006864dc
extern int gFrontend_backdrop1_opacity_mode;
// GLOBAL: CARMA2_HW 0x006864d4
extern int gFrontend_backdrop2_opacity_mode;
// GLOBAL: CARMA2_HW 0x00686ef8
extern int gFrontend_interpolate_steps_left;
// GLOBAL: CARMA2_HW 0x0068683c
extern int gFrontend_count_brender_items;
// GLOBAL: CARMA2_HW 0x00764ee0
extern int gFrontend_remove_current_backdrop;
// GLOBAL: CARMA2_HW 0x00686f8c
extern br_pixelmap* gFrontend_backdrop;
// GLOBAL: CARMA2_HW 0x00688ae8
extern br_colour gFrontend_some_color;
// GLOBAL: CARMA2_HW 0x0068650c
extern br_actor* gFrontend_actor;
// GLOBAL: CARMA2_HW 0x00686f94
extern br_actor* gFrontend_camera;
// GLOBAL: CARMA2_HW 0x00686f50
extern br_token_value gFrontend_backdrop0_material_prims[3];
// GLOBAL: CARMA2_HW 0x00686f20
extern br_token_value gFrontend_backdrop1_material_prims[3];
// GLOBAL: CARMA2_HW 0x00686f38
extern br_token_value gFrontend_backdrop2_material_prims[3];
// GLOBAL: CARMA2_HW 0x00686f10
extern br_material* gFrontend_backdrop_materials[3];
// GLOBAL: CARMA2_HW 0x00686f08
extern int gCount_connected_items_indices;
// GLOBAL: CARMA2_HW 0x00687018
extern int gConnected_items_indices[6];
// FUNCTION: CARMA2_HW 0x004850d0
void C2_HOOK_FASTCALL FreeInterfaceStrings(void) {
    int i;

    for (i = 0; i < gCount_interface_strings; i++) {
        BrMemFree(gInterface_strings[i]);;
    }
    gCount_interface_strings = 0;
}