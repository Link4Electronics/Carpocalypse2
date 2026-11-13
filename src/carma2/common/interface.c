#include "interface.h"

#include "drmem.h"
#include "loading1.h"
#include "loading2.h"
#include "displays.h"
#include "graphics2.h"
#include "font.h"
#include "loadsave.h"
#include "intrface.h"
#include "main.h"
#include "utility.h"
#include "input.h"
#include "errors.h"
#include "sound.h"
#include "packfile.h"
#include "interface-main.h"
#include "interface-network.h"
#include "network.h"
#include "loading2.h"
#include "interface-quit.h"
#include "interface-options.h"
#include "interface-newgame.h"
#include "interface-wrecks.h"
#include "interface-netsync.h"
#include "interface-networksummary.h"
#include "interface-startgame.h"
#include "loading3.h"
#include "tintedpoly.h"
#include "joystick.h"
#include "globvars.h"
#include "platform.h"
#include "carpocalypse2_macros.h"
#include "carpocalypse2_types.h"

#include "c2_string.h"

#include "c2_stdio.h"

#include "c2_stdlib.h"

#include <ctype.h>

#include <math.h>

extern void C2_HOOK_FASTCALL DisplayJoystickSettings(tFrontend_spec *pFrontend);
extern int C2_HOOK_FASTCALL NetInitialise(void);
extern void C2_HOOK_FASTCALL AboutToLoadFirstCar(void);
extern void C2_HOOK_FASTCALL CheckForCheatingGits(void);

#define COUNT_FRONTEND_INTERPOLATE_STEPS 16

/* ==== forward declarations for the frontend flow ==== */
extern void C2_HOOK_FASTCALL FRONTEND_DrawMenu(tFrontend_spec* pFrontend);
extern int C2_HOOK_FASTCALL FRONTEND_Redraw(br_pixelmap* dst, br_int_16 dx, br_int_16 dy, br_pixelmap* src, br_int_16 sx, br_int_16 sy, br_int_16 w, br_int_16 h);
extern int C2_HOOK_FASTCALL FRONTEND_GenericMenuHandler(tFrontend_spec* pFrontend);
extern int C2_HOOK_FASTCALL Generic_MenuHandler(tFrontend_spec* pFrontend);
extern int C2_HOOK_FASTCALL Generic_Infunc(tFrontend_spec* pFrontend);
extern int C2_HOOK_FASTCALL Generic_Outfunc(tFrontend_spec* pFrontend);
extern void C2_HOOK_FASTCALL Generic_MungeActiveItems(tFrontend_spec* pFrontend);
extern void C2_HOOK_FASTCALL Generic_UnMungeActiveItems(tFrontend_spec* pFrontend);
extern int C2_HOOK_FASTCALL CreditsScreenInfunc(tFrontend_spec* pFrontend);
extern int C2_HOOK_FASTCALL CreditsScreenOutfunc(tFrontend_spec* pFrontend);
extern int C2_HOOK_FASTCALL LoadGameInFunc(tFrontend_spec* pFrontend);
extern int C2_HOOK_FASTCALL LoadGameScrollDown(tFrontend_spec* pFrontend);
extern int C2_HOOK_FASTCALL NewGameToggleTyping(tFrontend_spec* pFrontend);
extern int C2_HOOK_FASTCALL Controls_Infunc(tFrontend_spec* pFrontend);
extern int C2_HOOK_FASTCALL Controls_Outfunc(tFrontend_spec* pFrontend);
extern int C2_HOOK_FASTCALL Controls_Ok(tFrontend_spec* pFrontend);
extern int C2_HOOK_FASTCALL Controls_JoystickToggle(tFrontend_spec* pFrontend);
extern int C2_HOOK_FASTCALL Controls_JoystickDpadToggle(tFrontend_spec* pFrontend);
extern int C2_HOOK_FASTCALL Controls_SwitchKeymapSet(tFrontend_spec* pFrontend);
extern int C2_HOOK_FASTCALL Controls_SlotActivated(tFrontend_spec* pFrontend);
extern int C2_HOOK_FASTCALL Controls_KeyUp(tFrontend_spec* pFrontend);
extern int C2_HOOK_FASTCALL Controls_KeyDown(tFrontend_spec* pFrontend);
extern void C2_HOOK_FASTCALL RefreshScrollSet(tFrontend_spec* pFrontend);
extern void C2_HOOK_FASTCALL ScrollSet_DisplayEntry(tConnected_items* pScroll_set, int pItem);
extern int C2_HOOK_FASTCALL ScrollSet_TranslateItemToIndex(tConnected_items* pConnected, int pItem);
extern tStruct_00686508* C2_HOOK_FASTCALL GetUpDown(int pItem);
extern int C2_HOOK_FASTCALL DetermineKeyArrayIndex(void);
extern int C2_HOOK_FASTCALL ProcessInputString(void);
extern int C2_HOOK_FASTCALL RaceIndex(const char* pName);
extern tFrontend_slider* C2_HOOK_FASTCALL GetAnyActiveSlider(void);
extern tFrontend_slider* C2_HOOK_FASTCALL GetActiveSlider(void);
extern tConnected_items* C2_HOOK_FASTCALL GetScrollSet(int pItem);
extern int C2_HOOK_FASTCALL TranslateSliderItem(tFrontend_slider* pScroller, int pIndex);
extern void C2_HOOK_FASTCALL Generic_EventEffect(void);
extern int C2_HOOK_FASTCALL FindNextActiveItem(tFrontend_spec* pFrontend, int pStart_index);
extern int C2_HOOK_FASTCALL Frontend_FindNextVisibleItem(tFrontend_spec* pFrontend, int pIndex);
extern int C2_HOOK_FASTCALL Ians_GetItemAtMousePos(tFrontend_spec* pFrontend, int pX, int pY);
extern void C2_HOOK_FASTCALL FillInRaceDescription(char* pDest, int pRace_index);
extern void C2_HOOK_FASTCALL MenuSetCarImage(int pCar_index, int pBrender_index);
extern void C2_HOOK_FASTCALL MenuSetDriverImage(int pOpponent_index, int pFrontend_index);
extern void C2_HOOK_FASTCALL SwitchToHiresMode(void);
extern void C2_HOOK_FASTCALL TurnTintedPolyOff(int pTintedIndex);
extern void C2_HOOK_FASTCALL StartMusicTrack(int pIndex);
extern int C2_HOOK_FASTCALL StartSavedGamesList(void);
extern tU32 C2_HOOK_FASTCALL FudgeBRenderIntoTheNinetiesWithSomeProperFuckingColourSupport(br_pixelmap* pm, tU32 red, tU32 grn, tU32 blu, tU32 alp);
extern br_pixelmap* C2_HOOK_FASTCALL LoadPixelmap(const char* pName);
extern void C2_HOOK_FASTCALL LoadKeyNames(void);
extern void C2_HOOK_FASTCALL BackupKeyMappings(void);
extern void C2_HOOK_FASTCALL RestoreKeyMappings(void);
extern void C2_HOOK_FASTCALL DisposeKeyNames(void);
extern void C2_HOOK_FASTCALL SaveKeyMapping(void);
extern void C2_HOOK_FASTCALL LoadKeyMapping(void);
extern void C2_HOOK_FASTCALL ChangeKeyMapIndex(int pIndex);
extern void C2_HOOK_FASTCALL CyclePollKeys(void);
extern void C2_HOOK_FASTCALL Joystick_BackupSettings(void);
extern void C2_HOOK_FASTCALL SetJoystickDPadEnabled(int pEnabled);
extern void C2_HOOK_FASTCALL DisableJoysticks(void);
extern void C2_HOOK_FASTCALL EnableJoysticks(void);
extern float C2_HOOK_FASTCALL GetJoystickX(void);
extern float C2_HOOK_FASTCALL GetJoystickY(void);
extern int C2_HOOK_FASTCALL GetJoystickFBBGain(void);
extern void C2_HOOK_FASTCALL SetJoystickX(float v);
extern void C2_HOOK_FASTCALL SetJoystickY(float v);
extern void C2_HOOK_FASTCALL SetJoystickFFBGain(int pValue);

#define LOAD_SLOT_N(FRONTEND, N) \
    if (gFrontend_load_game_index_top + (N) + 1 > gFrontend_count_saved_games) { \
        pFrontend->items[N + 1].menuInfo = NULL; \
        return 0; \
    } \
    TryToLoadGame(N); \
    if (gProgram_state.racing) { \
        return 1; \
    } \
    gFrontend_next_menu = kFrontend_menu_main; \
    return 3;
extern tFrontend_spec gFrontend_CREDITS;
br_material* gFrontend_backdrop_materials[3];
br_actor* gFrontend_backdrop_actors[3];
extern tFrontend_spec gFrontend_LOAD_GAME;
extern tFrontend_spec gFrontend_CONTROLS;
extern int gNet_mode;
extern int gPending_race;
extern int gStart_race_sent;
extern int gLast_host_query;
extern int gFrontend_change_car_selected_car;
extern int gFrontend_opponent_profile_pic_needs_update;
extern int gFrontend_controls_indices[15];
extern int gHud_tinted1;
extern int gHud_tinted2;
extern int gHud_tinted3;
extern char* gKey_names[153];
extern tJoy_array gJoy_array;
extern tKey_array gKey_array;
extern int gKey_poll_counter;
extern int gTyping_slot;
extern int gKey_map_index;
extern int gKey_mapping[77];
extern tSave_game* gSaved_games;
extern int gINT_0068b8e4;
extern int gINT_0068b8e8;
extern int gRace_head_ups[22];
extern int gOrig_joystick_index;
extern br_actor* gPolyfont_glyph_actors[256];
extern tRace_group_spec* gRace_groups;

// GLOBAL: CARMA2_HW 0x00764eec
tFrontendMenuType gFrontend_next_menu;

// GLOBAL: CARMA2_HW 0x00688b20
int gFrontend_suppress_mouse;

// GLOBAL: CARMA2_HW 0x00688ab0
tConnected_items* gConnected_items;

// GLOBAL: CARMA2_HW 0x00686508
tStruct_00686508* gPTR_00686508;

// GLOBAL: CARMA2_HW 0x00686820
tFrontend_slider* gCurrent_frontend_scrollbars;

// GLOBAL: CARMA2_HW 0x00604730
tFrontend_item_spec gDefault_last_interface_item = {
    0x401, temp, NULL, 0, 0, 0, 0, 0, 0, 0, 0, 1,
};

// GLOBAL: CARMA2_HW 0x006864f4
int gFrontend_scrollbars_updated;

// GLOBAL: CARMA2_HW 0x00686850
br_pixelmap* gFrontend_images[100];

// GLOBAL: CARMA2_HW 0x00687248
tFrontend_brender_item gFrontend_brender_items[100]; /* FIXME: parametrize size + index of last item */

// GLOBAL: CARMA2_HW 0x00688770
int gFrontend_selected_item_index;

// GLOBAL: CARMA2_HW 0x0068b8a0
tRace_group_spec* gRace_groups;

// GLOBAL: CARMA2_HW 0x00688af4
char* gFrontend_current_input;

// GLOBAL: CARMA2_HW 0x0068723c
int gFrontend_text_input_item_index;

// GLOBAL: CARMA2_HW 0x00763900
char gFrontend_original_player_name[32];

// GLOBAL: CARMA2_HW 0x0059b0d8
int gINT_0059b0d8 = -1;

// GLOBAL: CARMA2_HW 0x0059b0d4
int gFrontend_maximum_input_length = 9;

// GLOBAL: CARMA2_HW 0x00763924
int gHierarchy_has_actor;

// GLOBAL: CARMA2_HW 0x00686834
tU32 gCredits_scroll_start;

// GLOBAL: CARMA2_HW 0x006883c0
int gCredits_line_count;

// GLOBAL: CARMA2_HW 0x00686f04
int* gCredits_heights;

// GLOBAL: CARMA2_HW 0x00687238
int* gCredits_throbs;

// GLOBAL: CARMA2_HW 0x00688448
char** gCredits_texts;

// GLOBAL: CARMA2_HW 0x0068682c
int* gCredits_fonts;

// GLOBAL: CARMA2_HW 0x00686f90
int gCredits_total_height;

// GLOBAL: CARMA2_HW 0x006886d0
double gFrontend_throb_factor;

// GLOBAL: CARMA2_HW 0x00763940
br_actor* gFrontend_billboard_actors[8];

// GLOBAL: CARMA2_HW 0x00688b10
br_actor* gFrontend_menu_camera;

// GLOBAL: CARMA2_HW 0x007638ac
int gFrontend_load_game_index_top;

// GLOBAL: CARMA2_HW 0x00763898
int gFrontend_scroll_time_left;

// GLOBAL: CARMA2_HW 0x0076389c
int gFrontend_scroll_time_increment;

// GLOBAL: CARMA2_HW 0x00764e9c
extern int gFrontend_count_saved_games;

// GLOBAL: CARMA2_HW 0x00763894
tU32 gFrontend_scroll_last_update;

// GLOBAL: CARMA2_HW 0x00688b04
int gFrontend_mouse_down;

// GLOBAL: CARMA2_HW 0x0068875c
tU32 gFrontend_time_last_input;

// GLOBAL: CARMA2_HW 0x00764ee4
int gNet_join_host_result;  // FIXME: use union instead?

// GLOBAL: CARMA2_HW 0x00688b08
int gPrev_frontend_mouse_down;

// GLOBAL: CARMA2_HW 0x006883a8
int gFrontend_leave_current_menu;

// GLOBAL: CARMA2_HW 0x00688abc
tFrontend_spec* gCurrent_frontend_spec;

// GLOBAL: CARMA2_HW 0x007635f0
br_uint_32 gFrontend_APO_Colour_1;

// GLOBAL: CARMA2_HW 0x007635e0
br_uint_32 gFrontend_APO_Colour_2;

// GLOBAL: CARMA2_HW 0x00763700
br_uint_32 gFrontend_APO_Colour_3;

// GLOBAL: CARMA2_HW 0x0068650c
br_actor* gFrontend_actor;
// GLOBAL: CARMA2_HW 0x00686f94
br_actor* gFrontend_camera;

// GLOBAL: CARMA2_HW 0x00686f50
br_token_value gFrontend_backdrop0_material_prims[3];

// GLOBAL: CARMA2_HW 0x00686f20
br_token_value gFrontend_backdrop1_material_prims[3];

// GLOBAL: CARMA2_HW 0x00686f38
br_token_value gFrontend_backdrop2_material_prims[3];

// GLOBAL: CARMA2_HW 0x0068683c
int gFrontend_count_brender_items;

// GLOBAL: CARMA2_HW 0x00764ee0
int gFrontend_remove_current_backdrop;

// GLOBAL: CARMA2_HW 0x00686f8c
br_pixelmap* gFrontend_backdrop;

// GLOBAL: CARMA2_HW 0x00688ae8
br_colour gFrontend_some_color;

// GLOBAL: CARMA2_HW 0x006870b8
tFrontend_model gFrontend_A_models[13];

// GLOBAL: CARMA2_HW 0x00687188
tFrontend_model gFrontend_B_models[11];

// GLOBAL: CARMA2_HW 0x00687058
tFrontend_model gFrontend_C_models[6];

// GLOBAL: CARMA2_HW 0x00688378
br_model* gFrontend_A_model_from;

// GLOBAL: CARMA2_HW 0x0068844c
br_model* gFrontend_A_model_to;

// GLOBAL: CARMA2_HW 0x00688768
br_model* gFrontend_B_model_from;

// GLOBAL: CARMA2_HW 0x006886e0
br_model* gFrontend_B_model_to;

// GLOBAL: CARMA2_HW 0x00686504
br_model* gFrontend_C_model_from;

// GLOBAL: CARMA2_HW 0x006886bc
br_model* gFrontend_C_model_to;

// GLOBAL: CARMA2_HW 0x006864e0
int gFrontend_backdrop0_opacity_mode;

// GLOBAL: CARMA2_HW 0x006864dc
int gFrontend_backdrop1_opacity_mode;

// GLOBAL: CARMA2_HW 0x006864d4
int gFrontend_backdrop2_opacity_mode;

// GLOBAL: CARMA2_HW 0x00686ef8
int gFrontend_interpolate_steps_left;

// GLOBAL: CARMA2_HW 0x0068c230
extern char* gInterface_strings[300];
// GLOBAL: CARMA2_HW 0x0059b0d0
int gFrontend_stuff_not_loaded = 1;
// GLOBAL: CARMA2_HW 0x00686f10
br_material* gFrontend_backdrop_materials[3];
// GLOBAL: CARMA2_HW 0x00687030
br_actor* gFrontend_backdrop_actors[3];
// GLOBAL: CARMA2_HW 0x0075b8fc
extern int gTyping_slot;
// GLOBAL: CARMA2_HW 0x00688aec
br_actor* gFrontend_wrecks_actor;
// GLOBAL: CARMA2_HW 0x00688af0
br_actor* gFrontend_wrecks_camera;
// GLOBAL: CARMA2_HW 0x007635e4
br_pixelmap* gFrontend_wrecks_pixelmap;
// GLOBAL: CARMA2_HW 0x00686f08
int gCount_connected_items_indices;
// GLOBAL: CARMA2_HW 0x00687018
int gConnected_items_indices[6];
// GLOBAL: CARMA2_HW 0x0068843c
tU32 gFrontend_last_scroll;
// GLOBAL: CARMA2_HW 0x00688444
int gINT_00688444;
// GLOBAL: CARMA2_HW 0x00688408
tConnected_items gControls_scroller;
// GLOBAL: CARMA2_HW 0x00604888
int gControls_frontend_to_key_mapping_lut[29] = {
    49, 50, 47, 48, 54, 45, 60, 58, 56, 46,
    57, 67, 68, 69, 71, 61, 62, 63, 64, 74,
    59, 70, 72, 73, 75, 76, 35, 65, 66,
};

extern int gFrontend_controls_count_keys;
extern int gFrontend_car_image_outdated;
extern tFrontend_slider gControls_slider_1;
extern tFrontend_slider gControls_slider_2;
extern tFrontend_slider gControls_slider_3;

// GLOBAL: CARMA2_HW 0x006864f8
int gFrontend_controls_count_keys;

// GLOBAL: CARMA2_HW 0x00686f68
tFrontend_slider gControls_slider_1;

// GLOBAL: CARMA2_HW 0x006883c8
tFrontend_slider gControls_slider_2;

// GLOBAL: CARMA2_HW 0x00688ac0
tFrontend_slider gControls_slider_3;

// GLOBAL: CARMA2_HW 0x00687040
extern int gFrontend_car_image_outdated;

// GLOBAL: CARMA2_HW 0x0074c6b4
extern int gFrontend_net_initialized;

// GLOBAL: CARMA2_HW 0x005cab48
tFrontend_spec gFrontend_CREDITS = {
    "Credits",
    0,
    1,
    CreditsScreenInfunc,
    CreditsScreenOutfunc,
    NULL,
    &gFrontend_MAIN,
    0,
    0,
    0,
    11,
    0,
    0,
    {
        { 0x401, temp, &gFrontend_MAIN, 0, 17, 18, 0, 0, 0, 0, 1, 1 },
    },
};

// GLOBAL: CARMA2_HW 0x005ed5a0
tFrontend_spec gFrontend_LOAD_GAME = {
    "LoadGame",
    0,
    19,
    LoadGameInFunc,
    LoadGameOutFunc,
    LoadGameUpdateFunc,
    &gFrontend_MAIN,
    0,
    0,
    0,
    0,
    9,
    0,
    {
        { 0x18,     temp,               NULL,                       0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x401,    LoadSlot1,          (void*)1,                   0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x401,    LoadSlot2,          (void*)1,                   0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x401,    LoadSlot3,          (void*)1,                   0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x401,    LoadSlot4,          (void*)1,                   0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x401,    LoadSlot5,          (void*)1,                   0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x401,    LoadSlot6,          (void*)1,                   0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x401,    LoadSlot7,          (void*)1,                   0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x401,    LoadSlot8,          (void*)1,                   0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x4b,     temp,               NULL,                       0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0xff,     temp,               NULL,                       0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x4e,     temp,               NULL,                       0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x4f,     temp,               NULL,                       0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x100,    temp,               NULL,                       0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0xfe,     temp,               NULL,                       0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x401,    temp,               NULL,                       0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x8,      temp,               &gFrontend_OPTIONS,    0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x401,    LoadGameScrollUp,   NULL,                       0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x401,    LoadGameScrollDown, NULL,                       0, 0, 0, 0, 0, 0, 0, 1, 1, },
    },
};

// GLOBAL: CARMA2_HW 0x00604940
tFrontend_spec gFrontend_CONTROLS = {
    "Controls",
    0,
    68,
    Controls_Infunc,
    Controls_Outfunc,
    Generic_MenuHandler,
    &gFrontend_OPTIONS,
    0,
    0,
    0,
    7,
    0,
    0,
    {
        { 0x402, temp,                          NULL,       0, 17, 18, 0, 0, 0, 0, 1, 0, },
        { 0x402, temp,                          NULL,       0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402, temp,                          NULL,       0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402, temp,                          NULL,       0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402, temp,                          NULL,       0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402, temp,                          NULL,       0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402, temp,                          NULL,       0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402, temp,                          NULL,       0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402, temp,                          NULL,       0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402, temp,                          NULL,       0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x56,  Controls_JoystickToggle,       NULL,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x57,  Controls_JoystickToggle,       NULL,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x59,  temp,                          NULL,       0, 0,  0,  0, 0, 0, 0, 0, 1, },
        { 0x5b,  temp,                          NULL,       0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402, temp,                          NULL,       0, 0,  0,  0, 0, 0, 0, 1, 1, 1, 0, 1, 1, },
        { 0x402, temp,                          NULL,       0, 0,  0,  0, 0, 0, 0, 1, 1, 1, 0, 0, 0, },
        { 0x402, temp,                          NULL,       0, 0,  0,  0, 0, 0, 0, 1, 1, 1, 0, 0, 0, },
        { 0x5a,  temp,                          NULL,       0, 0,  0,  0, 0, 0, 0, 0, 1, 0, 0, 0, 0, },
        { 0x5c,  temp,                          NULL,       0, 0,  0,  0, 0, 0, 0, 0, 1, 0, 0, 0, 0, },
        { 0x402, temp,                          NULL,       0, 0,  0,  0, 0, 0, 0, 1, 1, 1, 0, 1, 1, },
        { 0x402, temp,                          NULL,       0, 0,  0,  0, 0, 0, 0, 1, 1, 1, 0, 0, 0, },
        { 0x402, temp,                          NULL,       0, 0,  0,  0, 0, 0, 0, 1, 1, 1, 0, 0, 0, },
        { 0x63,  temp,                          NULL,       0, 0,  0,  0, 0, 0, 0, 0, 1, 0, 0, 0, 0, },
        { 0x402, temp,                          NULL,       0, 0,  0,  0, 0, 0, 0, 1, 1, 1, 0, 1, 1, },
        { 0x402, temp,                          NULL,       0, 0,  0,  0, 0, 0, 0, 1, 1, 1, 0, 0, 0, },
        { 0x402, temp,                          NULL,       0, 0,  0,  0, 0, 0, 0, 1, 1, 1, 0, 0, 0, },
        { 0x61,  temp,                          NULL,       0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x5f,  temp,                          NULL,       0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x62,  temp,                          NULL,       0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x61,  temp,                          NULL,       0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x5f,  temp,                          NULL,       0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x62,  temp,                          NULL,       0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x61,  temp,                          NULL,       0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x60,  temp,                          NULL,       0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x62,  temp,                          NULL,       0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0xef,  temp,                          NULL,       0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0xf1,  Controls_JoystickDpadToggle,   NULL,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xf2,  Controls_JoystickDpadToggle,   NULL,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x68,  temp,                          NULL,       0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0xf3,  Controls_SwitchKeymapSet,      NULL,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xf4,  Controls_SwitchKeymapSet,      NULL,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xf5,  Controls_SwitchKeymapSet,      NULL,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xf6,  Controls_SwitchKeymapSet,      NULL,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404, Controls_SlotActivated,        NULL,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404, Controls_SlotActivated,        NULL,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404, Controls_SlotActivated,        NULL,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404, Controls_SlotActivated,        NULL,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404, Controls_SlotActivated,        NULL,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404, Controls_SlotActivated,        NULL,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404, Controls_SlotActivated,        NULL,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404, Controls_SlotActivated,        NULL,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404, Controls_SlotActivated,        NULL,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404, Controls_SlotActivated,        NULL,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404, Controls_SlotActivated,        NULL,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404, Controls_SlotActivated,        NULL,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404, Controls_SlotActivated,        NULL,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404, Controls_SlotActivated,        NULL,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404, Controls_SlotActivated,        NULL,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404, Controls_SlotActivated,        NULL,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404, Controls_SlotActivated,        NULL,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404, Controls_SlotActivated,        NULL,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404, Controls_SlotActivated,        NULL,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404, Controls_SlotActivated,        NULL,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x403, Controls_KeyUp,                NULL,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x403, Controls_KeyDown,              NULL,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xee,  temp,                          NULL,       0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0xf0,  temp,                          NULL,       0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x7,   Controls_Ok,                   (void*)1,   0, 0,  0,  0, 0, 0, 0, 1, 1, },
    },
};

// GLOBAL: CARMA2_HW 0x00688748
int gINT_00688748;

// GLOBAL: CARMA2_HW 0x00604900
int gFrontend_controls_indices[15] = {
    12, 13, 17, 18, 22, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35
};

#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern br_actor* C2_HOOK_CDECL BrActorAdd();
extern br_actor* C2_HOOK_CDECL BrActorAllocate();
extern void C2_HOOK_CDECL BrActorFree();
extern br_actor* C2_HOOK_CDECL BrActorRemove();
extern br_bounds* C2_HOOK_CDECL BrActorToBounds();
extern void C2_HOOK_CDECL BrLightEnable();
extern br_pixelmap* C2_HOOK_CDECL BrMapRemove();
extern void C2_HOOK_CDECL BrMaterialFree();
extern br_material* C2_HOOK_CDECL BrMaterialRemove();
extern void C2_HOOK_CDECL BrMatrix34PostRotateX();
extern void C2_HOOK_CDECL BrMatrix34PostRotateY();
extern void C2_HOOK_CDECL BrMatrix34PreRotateX();
extern void C2_HOOK_CDECL BrMatrix34Translate();
extern void C2_HOOK_CDECL BrModelFree();
extern br_model* C2_HOOK_CDECL BrModelRemove();
extern void C2_HOOK_CDECL BrModelUpdate();
extern br_pixelmap* C2_HOOK_CDECL BrPixelmapAllocateSub();
extern void C2_HOOK_CDECL BrPixelmapCopy();
extern void C2_HOOK_CDECL BrPixelmapFill();
extern void C2_HOOK_CDECL BrPixelmapFree();
extern void C2_HOOK_CDECL BrZbsSceneRenderAdd();
extern void C2_HOOK_CDECL BrZbsSceneRenderBegin();
extern void C2_HOOK_CDECL BrZbsSceneRenderEnd();
extern br_actor* C2_HOOK_FASTCALL CreateAPOactor(void);
extern tS3_sound_tag C2_HOOK_FASTCALL DRS3StartSound(tS3_outlet* pOutlet, tS3_sound_id pSound);
extern void C2_HOOK_FASTCALL EdgeTriggerModeOff(void);
extern void C2_HOOK_FASTCALL EdgeTriggerModeOn(void);
extern int C2_HOOK_FASTCALL EitherMouseButtonDown(void);
extern void C2_HOOK_FASTCALL EndSavedGamesList(void);
extern int C2_HOOK_FASTCALL GetItemAtMousePos(tFrontend_spec *pFrontend, int pX, int pY);
extern void C2_HOOK_FASTCALL GetMousePosition(int *pX, int *pY);
extern void C2_HOOK_FASTCALL KillSplashScreen(void);
extern int C2_HOOK_FASTCALL PDAnyKeyDown(void);
extern tU32 C2_HOOK_FASTCALL PDGetTotalTime(void);
extern int C2_HOOK_FASTCALL PDKeyDown(int pKey_index);
extern void C2_HOOK_FASTCALL PollKeys(void);
extern void C2_HOOK_FASTCALL ServiceGame(void);
extern void C2_HOOK_FASTCALL UpdateScrollPositions(tFrontend_spec* pFrontend);
extern void C2_HOOK_FASTCALL WaitForNoKeys(void);
extern br_uint_8 bl;
extern br_pixelmap* gBack_screen;
extern br_scalar gCamera_yon;
extern br_pixelmap* gDepth_buffer;
extern tS3_outlet* gEffects_outlet;
extern tFrontend_spec gFrontend_QUIT;
extern int gMouse_in_use;
extern tNet_game_options gNet_settings[9];
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_CDECL BrMapUpdate();
extern void C2_HOOK_CDECL BrMaterialUpdate();
extern void* C2_HOOK_CDECL BrMemAllocate();
extern void C2_HOOK_CDECL BrPixelmapRectangleFill();
extern void C2_HOOK_CDECL BrZbsSceneRender();
extern FILE* C2_HOOK_FASTCALL DRfopen(const char* pFilename, const char* pMode);
extern int C2_HOOK_FASTCALL DoMouseCursor(void);
extern void C2_NORETURN C2_HOOK_CDECL FatalError();
extern tSave_game* C2_HOOK_FASTCALL GetNthSavedGame(int pN);
extern const char* C2_HOOK_FASTCALL IString_Get(int pIndex);
extern void C2_HOOK_FASTCALL Morph_Initialise(tFrontend_spec* pCurrent, tFrontend_spec* pNext);
extern char* C2_HOOK_FASTCALL MungeCommas(int pValue);
extern void C2_NORETURN C2_HOOK_FASTCALL PDFatalError(const char* pThe_str);
extern int C2_HOOK_FASTCALL PDIsJoystickDPadEnabled(void);
extern void C2_HOOK_FASTCALL PDScreenBufferSwap(int pRendering_area_only);
extern int C2_HOOK_FASTCALL PFfclose(FILE* pFile);
extern char* C2_HOOK_FASTCALL PFfgets(char* buffer, br_size_t size, FILE* pFile);
extern void C2_HOOK_FASTCALL PathCat(char* pDestn_str, const char* pStr_1, const char* pStr_2);
extern int C2_HOOK_FASTCALL PolyFontTextWidth(int pFont, const char* pText);
extern void C2_HOOK_FASTCALL PossibleLock(int pValue);
extern void C2_HOOK_FASTCALL PossibleService(void);
extern int C2_HOOK_FASTCALL PossibleUnlock(int pValue);
extern void C2_HOOK_FASTCALL PrintAPO(int pX, int pY, int pIndex, int pTex_index);
extern int C2_HOOK_FASTCALL SaveAllJoystickData(void);
extern int C2_HOOK_FASTCALL SaveOptions(void);
extern void C2_HOOK_FASTCALL SelectThisItemIn(tFrontend_spec* pFrontend, int pGroup, int pValue);
extern void C2_HOOK_FASTCALL SolidPolyFontText(const char* pText, int pX, int pY, int pFont, tJustification pJust, int pRender);
extern void C2_HOOK_FASTCALL SolidPolyFontTextInABox(int pFont, const char* pText, int pX, int pY, int pWidth, int pHeight, tJustification pJustification, undefined4 pParam_8);
extern void C2_HOOK_FASTCALL TransparentPolyFontText(const char *pText, int pX, int pY, int pFont, tJustification pJust, int pRender, double pOpacity_factor);
extern void C2_HOOK_FASTCALL TransparentPolyFontTextInABox(int pFont, const char* pText, int pLeft, int pTop, int pRight, int pBottom, tJustification pJust, undefined4 pUnknown, double pBlend);
extern void __cdecl _ftol();
extern tPath_name gApplication_path;
extern char gBackup_key_mappings[];
extern tRace_group_spec* gCurrent_race_group;
extern int gIs_boundary_race;
extern int gJoystick_index;
extern char gNext_crush_actor_id[];
extern int gOrig_key_map_index;
extern char gPed_flags_0058f2e4[];
extern tProgram_state gProgram_state;
extern char gQuickTime_call_component_with_storage[];
extern tRace_list_spec gRace_list[100];
extern int gTyping;
#endif
// FUNCTION: CARMA2_HW 0x00466450
int C2_HOOK_FASTCALL temp(tFrontend_spec* pFrontend) {
    return 0;
}

// GLOBAL: CARMA2_HW 0x0058a050
const double gDouble_0058a050 = 0.523470938205719;

// GLOBAL: CARMA2_HW 0x0058a058
const double gDouble_0058a058 = 1.2;

// GLOBAL: CARMA2_HW 0x0058a060
const double gDouble_0058a060 = 0.1;

// GLOBAL: CARMA2_HW 0x0058a068
const double gDouble_0058a068 = 0.577;

// FUNCTION: CARMA2_HW 0x00466460
void C2_NAKED C2_HOOK_FASTCALL FUN_00466460(br_actor* pActor, int pX, int pY) {
#ifndef CARPOCALYPSE2_MATCHING
    float local_18[6];
    float width;
    float height;
    float depth;
    br_actor* world;
    br_actor* camera_actor;
    br_actor* light_actor;
    br_pixelmap* pixelmap;
    volatile float extent;
    float distance_f;
    volatile float camera_y;
    double distance;

    if (pActor == NULL) {
        return;
    }
    BrActorToBounds((br_bounds*)local_18, pActor);
    width = local_18[3] - local_18[0];
    height = local_18[4] - local_18[1];
    depth = local_18[5] - local_18[2];
    BrMatrix34PostRotateY(&pActor->t.t.mat, 0x71c);

    world = BrActorAllocate(BR_ACTOR_NONE, NULL);
    camera_actor = BrActorAllocate(BR_ACTOR_CAMERA, NULL);
    light_actor = BrActorAllocate(BR_ACTOR_LIGHT, NULL);

    ((br_camera*)camera_actor->type_data)->type = BR_CAMERA_PERSPECTIVE_FOV_OLD;
    ((br_camera*)camera_actor->type_data)->field_of_view = 0x1554;
    ((br_camera*)camera_actor->type_data)->hither_z = 0.1f;
    ((br_camera*)camera_actor->type_data)->yon_z = gCamera_yon;
    ((br_light*)light_actor->type_data)->type = BR_LIGHT_DIRECT;

    BrMatrix34PostRotateX(&light_actor->t.t.mat, 0xfc72);
    BrMatrix34PostRotateY(&light_actor->t.t.mat, 0xc004);

    ((br_light*)light_actor->type_data)->colour = 0xffffff;
    ((br_light*)light_actor->type_data)->attenuation_c = 0.5f;

    if (width > height) {
        if (width > depth) {
            extent = width;
        } else {
            extent = depth;
        }
    } else {
        if (depth >= height) {
            extent = depth;
        } else {
            extent = height;
        }
    }
    distance = (extent / (sin(gDouble_0058a050) / cos(gDouble_0058a050))) * gDouble_0058a058;
    distance_f = (float)distance;
    if (distance_f <= gDouble_0058a060) {
        distance = gDouble_0058a060;
    } else {
        distance = distance_f;
    }
    camera_y = (float)(distance * gDouble_0058a068);
    BrMatrix34Translate(&camera_actor->t.t.mat, 0.f, camera_y, (float)distance);
    BrMatrix34PreRotateX(&camera_actor->t.t.mat, 0xeaac);

    BrActorAdd(world, camera_actor);
    BrActorAdd(world, light_actor);
    BrLightEnable(light_actor);

    pixelmap = BrPixelmapAllocateSub(gBack_screen, pX + 0x14, pY + 0x5a, 100, 0x3c);
    BrPixelmapFill(gDepth_buffer, (br_colour)-1);
    BrPixelmapFill(pixelmap, 0);
    BrZbsSceneRenderBegin(world, camera_actor, pixelmap, gDepth_buffer);
    BrZbsSceneRenderAdd(pActor);
    BrZbsSceneRenderEnd();

    BrActorRemove(camera_actor);
    BrActorFree(camera_actor);
    BrActorRemove(light_actor);
    BrActorFree(light_actor);
    BrActorFree(world);
#else
    __asm {
            sub esp, 0x30
            push esi
            mov esi, ecx
            test esi, esi
            mov dword ptr [esp + 0x14], edx
            mov dword ptr [esp + 0x18], esi
            je L004666de
            push edi
            push ebp
            push ebx
            lea eax, [esp + 0x28]
            push esi
            push eax
            call BrActorToBounds
            fld dword ptr [esp + 0x3c]
            fsub dword ptr [esp + 0x30]
            add esp, 8
            add esi, 0x2c
            push 0x71c
            push esi
            fstp dword ptr [esp + 0x1c]
            fld dword ptr [esp + 0x40]
            fsub dword ptr [esp + 0x34]
            fstp dword ptr [esp + 0x20]
            fld dword ptr [esp + 0x44]
            fsub dword ptr [esp + 0x38]
            fstp dword ptr [esp + 0x18]
            call BrMatrix34PostRotateY
            add esp, 8
            push 0
            push 0
            call BrActorAllocate
            add esp, 8
            mov ebx, eax
            push 0
            push 3
            call BrActorAllocate
            add esp, 8
            mov esi, eax
            push 0
            push 2
            call BrActorAllocate
            mov ecx, dword ptr [esi + 0x5c]
            mov edi, eax
            add esp, 8
            mov byte ptr [ecx + 4], 4
            mov edx, dword ptr [esi + 0x5c]
            lea ebp, [edi + 0x2c]
            push 0xfc72
            mov word ptr [edx + 6], 0x1554
            mov eax, dword ptr [esi + 0x5c]
            push ebp
            mov dword ptr [eax + 8], 0x3dcccccd
            mov ecx, dword ptr [esi + 0x5c]
            mov edx, dword ptr [gCamera_yon]
            mov dword ptr [ecx + 0xc], edx
            mov eax, dword ptr [edi + 0x5c]
            mov byte ptr [eax + 4], 1
            call BrMatrix34PostRotateX
            add esp, 8
            push 0xc004
            push ebp
            call BrMatrix34PostRotateY
            fld dword ptr [esp + 0x1c]
            fcomp dword ptr [esp + 0x20]
            mov ecx, dword ptr [edi + 0x5c]
            add esp, 8
            mov dword ptr [ecx + 8], 0xffffff
            mov edx, dword ptr [edi + 0x5c]
            fnstsw ax
            mov dword ptr [edx + 0xc], 0x3f000000
            test ah, 0x41
            jne L00466574
            fld dword ptr [esp + 0x14]
            fcomp dword ptr [esp + 0x10]
            fnstsw ax
            test ah, 0x41
            jne L0046656a
            mov eax, dword ptr [esp + 0x14]
            jmp L00466591
    L0046656a:
            mov ecx, dword ptr [esp + 0x10]
            mov dword ptr [esp + 0x10], ecx
            jmp L00466595
    L00466574:
            fld dword ptr [esp + 0x18]
            fcomp dword ptr [esp + 0x10]
            fnstsw ax
            test ah, 1
            je L0046658d
            mov edx, dword ptr [esp + 0x10]
            mov dword ptr [esp + 0x10], edx
            jmp L00466595
    L0046658d:
            mov eax, dword ptr [esp + 0x18]
    L00466591:
            mov dword ptr [esp + 0x10], eax
    L00466595:
            fld qword ptr [gDouble_0058a050]
            fld qword ptr [gDouble_0058a050]
            fsin
            fxch st(1)
            fcos
            fst dword ptr [esp + 0x18]
            fdivp st(1), st(0)
            fdivr dword ptr [esp + 0x10]
            fmul qword ptr [gDouble_0058a058]
            fst dword ptr [esp + 0x10]
            fcomp qword ptr [gDouble_0058a060]
            fnstsw ax
            test ah, 0x41
            jne L004665ce
            fld dword ptr [esp + 0x10]
            jmp L004665d4
    L004665ce:
            fld qword ptr [gDouble_0058a060]
    L004665d4:
            fld dword ptr [esp + 0x10]
            fcomp qword ptr [gDouble_0058a060]
            fnstsw ax
            test ah, 0x41
            jne L004665ef
            fld dword ptr [esp + 0x10]
            fstp qword ptr [esp + 0x18]
            jmp L004665ff
    L004665ef:
            mov dword ptr [esp + 0x18], 0x9999999a
            mov dword ptr [esp + 0x1c], 0x3fb99999
    L004665ff:
            push ecx
            lea ebp, [esi + 0x2c]
            fstp dword ptr [esp]
            fld qword ptr [esp + 0x1c]
            fmul qword ptr [gDouble_0058a068]
            push ecx
            fstp dword ptr [esp]
            push 0
            push ebp
            call BrMatrix34Translate
            add esp, 0x10
            push 0xeaac
            push ebp
            call BrMatrix34PreRotateX
            add esp, 8
            push esi
            push ebx
            call BrActorAdd
            add esp, 8
            push edi
            push ebx
            call BrActorAdd
            add esp, 8
            push edi
            call BrLightEnable
            mov ecx, dword ptr [esp + 0x48]
            mov edx, dword ptr [esp + 0x24]
            mov eax, dword ptr [gBack_screen]
            add esp, 4
            add ecx, 0x5a
            add edx, 0x14
            push 0x3c
            push 0x64
            push ecx
            push edx
            push eax
            call BrPixelmapAllocateSub
            mov ecx, dword ptr [gDepth_buffer]
            add esp, 0x14
            mov ebp, eax
            push -1
            push ecx
            call BrPixelmapFill
            add esp, 8
            push 0
            push ebp
            call BrPixelmapFill
            mov edx, dword ptr [gDepth_buffer]
            add esp, 8
            push edx
            push ebp
            push esi
            push ebx
            call BrZbsSceneRenderBegin
            mov eax, dword ptr [esp + 0x34]
            add esp, 0x10
            push eax
            call BrZbsSceneRenderAdd
            add esp, 4
            call BrZbsSceneRenderEnd
            push esi
            call BrActorRemove
            add esp, 4
            push esi
            call BrActorFree
            add esp, 4
            push edi
            call BrActorRemove
            add esp, 4
            push edi
            call BrActorFree
            add esp, 4
            push ebx
            call BrActorFree
            add esp, 4
            pop ebx
            pop ebp
            pop edi
    L004666de:
            pop esi
            add esp, 0x30
            ret 4
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
    }
#endif
}

// int C2_HOOK_FASTCALL GetItemAtMousePos(tFrontend_spec *pFrontend, int pX, int pY); // Moved for inlining purposes

// FUNCTION: CARMA2_HW 0x00466ce0
void C2_NAKED C2_HOOK_FASTCALL MungeButtonModels(tFrontend_spec* pFrontend, int pButton_index) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;
    int item_idx;

    tFrontend_scroller_spec* scroller = &pFrontend->scrollers[pButton_index];

    for (i = 0; i < scroller->nbDisplayedAtOnce; i++) {
        item_idx = scroller->indexFirstScrollableItem + i;
        gFrontend_brender_items[item_idx].model->vertices[3].p.v[0] = gFrontend_brender_items[item_idx].model->vertices[1].p.v[0] + pFrontend->items[item_idx].width;
        gFrontend_brender_items[item_idx].model->vertices[2].p.v[0] = gFrontend_brender_items[item_idx].model->vertices[3].p.v[0];
        gFrontend_brender_items[item_idx].model->vertices[2].p.v[1] = gFrontend_brender_items[item_idx].model->vertices[3].p.v[1] - pFrontend->items[item_idx].height;
        gFrontend_brender_items[item_idx].model->vertices[1].p.v[1] = gFrontend_brender_items[item_idx].model->vertices[2].p.v[1];
        BrModelUpdate(gFrontend_brender_items[item_idx].model, BR_MODU_VERTEX_POSITIONS);
    }
#else
    __asm {
            push ebx
            push ebp
            lea eax, [edx + edx*2]
            mov ebp, ecx
            push esi
            xor esi, esi
            lea ecx, [edx + eax*4]
            mov eax, dword ptr [ebp + ecx*4 + 0x879c]
            test eax, eax
            lea ebx, [ebp + ecx*4]
            jle L00466d91
            push edi
    L00466d01:
            mov ecx, dword ptr [ebx + 0x87a4]
            mov eax, esi
            add eax, ecx
            push 1
            lea edx, [eax + eax*4]
            lea ecx, [eax + edx*2]
            mov edx, dword ptr [ecx*4 + gFrontend_brender_items+0x4]
            mov edi, dword ptr [edx + 8]
            mov edx, eax
            shl edx, 3
            sub edx, eax
            lea edx, [edx + edx*2]
            lea eax, [eax + edx*2]
            fild dword ptr [ebp + eax*8 + 0x150]
            lea eax, [ebp + eax*8]
            fadd dword ptr [edi + 0x28]
            fstp dword ptr [edi + 0x78]
            mov edx, dword ptr [ecx*4 + gFrontend_brender_items+0x4]
            mov edx, dword ptr [edx + 8]
            mov edi, dword ptr [edx + 0x78]
            mov dword ptr [edx + 0x50], edi
            mov edx, dword ptr [ecx*4 + gFrontend_brender_items+0x4]
            fild dword ptr [eax + 0x154]
            mov edx, dword ptr [edx + 8]
            fsubr dword ptr [edx + 0x7c]
            fstp dword ptr [edx + 0x54]
            mov eax, dword ptr [ecx*4 + gFrontend_brender_items+0x4]
            mov eax, dword ptr [eax + 8]
            mov edx, dword ptr [eax + 0x54]
            mov dword ptr [eax + 0x2c], edx
            mov eax, dword ptr [ecx*4 + gFrontend_brender_items+0x4]
            push eax
            call BrModelUpdate
            mov eax, dword ptr [ebx + 0x879c]
            add esp, 8
            inc esi
            cmp esi, eax
            jl L00466d01
            pop edi
    L00466d91:
            pop esi
            pop ebp
            pop ebx
            ret
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
    }
#endif
}

// ScrollUp

// ScrollDn

static void C2_HOOK_FASTCALL SetLevelBar(tFrontend_spec* pFrontend, int pLevel_index, int pSelected_item) {
    int i;

    for (i = pFrontend->levels[pLevel_index - 1].first_item_id; i <= pFrontend->levels[pLevel_index - 1].last_item_id; i++) {

        if (i <= pSelected_item) {
            BrPixelmapCopy(gFrontend_brender_items[i].field_0xc, gFrontend_images[4]);
            BrPixelmapCopy(gFrontend_brender_items[i].field_0x10, gFrontend_images[4]);
        } else {
            BrPixelmapCopy(gFrontend_brender_items[i].field_0xc, gFrontend_images[5]);
            BrPixelmapCopy(gFrontend_brender_items[i].field_0x10, gFrontend_images[5]);
        }
        pFrontend->items[i].radioButton_selected = 0;
    }
    pFrontend->items[pSelected_item].radioButton_selected = 1;
}

// FUNCTION: CARMA2_HW 0x00467890
void C2_NAKED C2_HOOK_FASTCALL ToggleSelection(tFrontend_spec* pFrontend) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;

    if (pFrontend->items[gFrontend_selected_item_index].group != 0) {
        for (i = 0; i < pFrontend->count_items; i++) {
            if (pFrontend->items[i].group == pFrontend->items[gFrontend_selected_item_index].group && pFrontend->items[gFrontend_selected_item_index].selectable != kFrontendSelectableButton) {
                pFrontend->items[i].radioButton_selected = 0;
            }
        }
        if (pFrontend->items[gFrontend_selected_item_index].selectable != kFrontendSelectableButton) {
            pFrontend->items[gFrontend_selected_item_index].radioButton_selected = !pFrontend->items[gFrontend_selected_item_index].radioButton_selected;
            DRS3StartSound(gEffects_outlet, eSoundId_Done);
        }
    } else {
        if (pFrontend->items[gFrontend_selected_item_index].idLevelBar != 0) {
            SetLevelBar(pFrontend, pFrontend->items[gFrontend_selected_item_index].idLevelBar, gFrontend_selected_item_index);
        }
    }
#else
    __asm {
            sub esp, 8
            push ebx
            push ebp
            push esi
            push edi
            mov edi, ecx
            mov ecx, dword ptr [gFrontend_selected_item_index]
            mov dword ptr [esp + 0x14], edi
            mov eax, ecx
            shl eax, 3
            sub eax, ecx
            lea eax, [eax + eax*2]
            lea edx, [ecx + eax*2]
            mov ebx, dword ptr [edi + edx*8 + 0x170]
            lea eax, [edi + edx*8]
            test ebx, ebx
            je L00467958
            mov eax, dword ptr [edi + 0x104]
            xor esi, esi
            test eax, eax
            jle L0046790e
            lea edx, [edi + 0x16c]
    L004678d4:
            cmp dword ptr [edx + 4], ebx
            jne L004678fd
            mov eax, ecx
            shl eax, 3
            sub eax, ecx
            lea eax, [eax + eax*2]
            lea eax, [ecx + eax*2]
            mov ebp, dword ptr [edi + eax*8 + 0x168]
            test ebp, ebp
            je L004678fd
            mov dword ptr [edx], 0
            mov ecx, dword ptr [gFrontend_selected_item_index]
    L004678fd:
            mov eax, dword ptr [edi + 0x104]
            inc esi
            add edx, 0x158
            cmp esi, eax
            jl L004678d4
    L0046790e:
            mov eax, ecx
            shl eax, 3
            sub eax, ecx
            lea edx, [eax + eax*2]
            lea eax, [ecx + edx*2]
            mov ecx, dword ptr [edi + eax*8 + 0x168]
            test ecx, ecx
            lea eax, [edi + eax*8]
            je L00467a27
            mov edx, dword ptr [eax + 0x16c]
            xor ecx, ecx
            test edx, edx
            sete cl
            mov dword ptr [eax + 0x16c], ecx
            mov ecx, dword ptr [gEffects_outlet]
            mov edx, 0xbbc
            call DRS3StartSound
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 8
            ret
    L00467958:
            mov eax, dword ptr [eax + 0x174]
            test eax, eax
            je L00467a27
            lea edx, [eax + eax*2]
            mov eax, dword ptr [edi + edx*8 + 0xaf18]
            mov dword ptr [esp + 0x10], ecx
            mov esi, dword ptr [edi + edx*8 + 0xaf14]
            lea ebp, [edi + edx*8]
            cmp esi, eax
            jg L00467a0f
            lea eax, [esi + esi*4]
            lea edx, [esi + eax*2]
            mov eax, esi
            shl eax, 3
            sub eax, esi
            lea ebx, [edx*4 + gFrontend_brender_items+0x10]
            lea eax, [eax + eax*2]
            lea edx, [esi + eax*2]
            lea edi, [edi + edx*8 + 0x16c]
    L004679a7:
            cmp esi, ecx
            jg L004679c9
            mov eax, dword ptr [gFrontend_images+0x10]
            mov ecx, dword ptr [ebx - 4]
            push eax
            push ecx
            call BrPixelmapCopy
            mov edx, dword ptr [gFrontend_images+0x10]
            mov eax, dword ptr [ebx]
            add esp, 8
            push edx
            push eax
            jmp L004679e5
    L004679c9:
            mov ecx, dword ptr [gFrontend_images+0x14]
            mov edx, dword ptr [ebx - 4]
            push ecx
            push edx
            call BrPixelmapCopy
            mov eax, dword ptr [gFrontend_images+0x14]
            mov ecx, dword ptr [ebx]
            add esp, 8
            push eax
            push ecx
    L004679e5:
            call BrPixelmapCopy
            mov ecx, dword ptr [esp + 0x18]
            add esp, 8
            mov dword ptr [edi], 0
            mov eax, dword ptr [ebp + 0xaf18]
            inc esi
            add ebx, 0x2c
            add edi, 0x158
            cmp esi, eax
            jle L004679a7
            mov edi, dword ptr [esp + 0x14]
    L00467a0f:
            mov eax, ecx
            shl eax, 3
            sub eax, ecx
            lea edx, [eax + eax*2]
            lea eax, [ecx + edx*2]
            mov dword ptr [edi + eax*8 + 0x16c], 1
    L00467a27:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 8
            ret
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004666f0
void C2_HOOK_FASTCALL FuckWithWidths(tFrontend_spec* pFrontend) {
    int i;
    const char* text;

    // item = pFrontend->items;
    for (i = 0; i < pFrontend->count_items; i++) {
#ifdef CARPOCALYPSE2_FIX_BUGS
        text = NULL;
#endif

        if (pFrontend->items[i].stringId > 0x400) {
            if (pFrontend->items[i].stringId == 0x404) {
                text = pFrontend->items[i].text;
            } else {
                continue;
            }
        } else {
            text = IString_Get(pFrontend->items[i].stringId);
        }
        if (text != NULL && !pFrontend->items[i].wrapText) {
            pFrontend->items[i].width = PolyFontTextWidth(pFrontend->items[i].unlitFont, text);
            pFrontend->items[i].height = PolyFontHeight(pFrontend->items[i].unlitFont);
        }
    }
}

// FUNCTION: CARMA2_HW 0x0046abf0
br_pixelmap* C2_HOOK_FASTCALL GetThisFuckingPixelmapPleaseMrTwatter(const char* pFolder, const char* pName) {
    br_pixelmap* pixelmaps[1000];
    br_pixelmap* result;
    FILE* f;
    tPath_name the_path;
    char* str;
    size_t count;
    size_t i;

    result = NULL;
    PathCat(the_path, pFolder, "PIXIES.P16");
    f = PFfopen(the_path, "rb");
    if (f != NULL) {
        PFfclose(f);
        count = BrPixelmapLoadMany(the_path, pixelmaps, CARPOCALYPSE2_ASIZE(pixelmaps));
        strcpy(the_path, pName);
        str = strchr(the_path, '.');
        *str = '\0';
        for (i = 0; i < count; i++) {
            if (pixelmaps[i] != NULL) {
                if (DRStricmp(pixelmaps[i]->identifier, the_path) == 0) {
                    result = pixelmaps[i];
                } else {
                    BrPixelmapFree(pixelmaps[i]);
                    pixelmaps[i] = NULL;
                }
            }
        }
        EnsurePixelmapAllowed(result, 0);
        return result;
    } else {
        PathCat(the_path, pFolder, pName);
        result = DRImageLoad(the_path);
        EnsurePixelmapAllowed(result, 0);
        return result;
    }
}

// FUNCTION: CARMA2_HW 0x00466b60
int C2_HOOK_FASTCALL GameMenuInfunc(tFrontend_spec* pFrontend) {
    DefaultInfunc(pFrontend);
    return 1;
}

// FUNCTION: CARMA2_HW 0x00466760
void C2_HOOK_FASTCALL LoadMenuSettings(tFrontend_spec* pFrontend) {
    tPath_name path;
    char buffer[256];
    char* str;
    FILE* f;
    int count;
    int version;
    int item_count;
    int len_text;
    int i;
    int j;

    C2_HOOK_BUG_ON(sizeof(tFrontend_spec) != 0xb8c8);
    C2_HOOK_BUG_ON(sizeof(tFrontend_item_spec) != 0x158);
    C2_HOOK_BUG_ON(sizeof(tFrontend_scroller_spec) != 0x34);
    C2_HOOK_BUG_ON(sizeof(tFrontend_radio_spec) != 0x2c);
    C2_HOOK_BUG_ON(offsetof(tFrontend_spec, items) != 0x130);
    C2_HOOK_BUG_ON(offsetof(tFrontend_spec, scrollers) != 0x8794);
    C2_HOOK_BUG_ON(offsetof(tFrontend_spec, radios) != 0x9be8);
    C2_HOOK_BUG_ON(offsetof(tFrontend_spec, levels) != 0xaf24);

    PathCat(path, gApplication_path, "INTRFACE");
    PathCat(path, path, pFrontend->name);
    strcat(path, ".TXT");

    f = DRfopen(path, "rt");
    if (f == NULL) {
#ifdef CARPOCALYPSE2_FIX_BUGS
        FatalError(kFatalError_Mysterious_SS, pFrontend->name, path);
#else
        FatalError(kFatalError_Mysterious_SS);
#endif
    }
    pFrontend->loaded = 1;
    GetALineAndDontArgue(f, buffer);
    str = strtok(buffer, "\t ,/");
    if (DRStricmp(str, "VERSION") != 0) {
        FatalError(kFatalError_FileMustStartWith_SS, pFrontend->name, "VERSION");
    }
    str = strtok(NULL, "\t ,/");
    count = sscanf(str, "%d", &version);
    if (count == 0 || version != 3) {
        FatalError(kFatalError_CantCopeWithVersionFor_SS, str, pFrontend->name);
    }
    GetAString(f, pFrontend->name);
    item_count = GetAnInt(f);
    if (item_count != pFrontend->count_items) {
        BrFailure("Error - Menu item number mismatch ", pFrontend->name);
    }
    pFrontend->count_groups = GetAnInt(f);
    GetAString(f, pFrontend->backdrop_name);
    for (i = 0; i < item_count; i++) {
        GetAString(f, pFrontend->items[i].text);
        if (DRStricmp("XXX", pFrontend->items[i].text) == 0) {
            strcpy(pFrontend->items[i].text, "");
        }
        len_text = strlen(pFrontend->items[i].text);
        for (j = 0; j < len_text; j++) {
            if (pFrontend->items[i].text[j] == '_') {
                pFrontend->items[i].text[j] = ' ';
            }
        }
        GetPairOfInts(f, &pFrontend->items[i].x, &pFrontend->items[i].y);
        pFrontend->items[i].x2 = pFrontend->items[i].x;
        pFrontend->items[i].y2 = pFrontend->items[i].y;
        GetPairOfInts(f, &pFrontend->items[i].width, &pFrontend->items[i].height);
        GetPairOfInts(f, &pFrontend->items[i].unlitFont, &pFrontend->items[i].highFont);
        pFrontend->items[i].map_index = GetAnInt(f);
        pFrontend->items[i].greyedBackdrop = GetAnInt(f);
        pFrontend->items[i].selectable = GetAnInt(f);
        pFrontend->items[i].group = GetAnInt(f);
        pFrontend->items[i].idLevelBar = GetAnInt(f);
        pFrontend->items[i].wrapText = GetAnInt(f);
        pFrontend->items[i].glowDisabled = GetAnInt(f);
    }
    // Copy item 99 (=last index)
    // FIXME: stringid = 0x401 (==> find out function of this item)
    memcpy(&pFrontend->items[CARPOCALYPSE2_ASIZE(pFrontend->items) - 1], &gDefault_last_interface_item, sizeof(tFrontend_item_spec));

    pFrontend->count_scrollers = GetAnInt(f);
    if (pFrontend->count_scrollers != 0) {
        for (i = 0; i < pFrontend->count_scrollers; i++) {
            GetPairOfInts(f, &pFrontend->scrollers[i].id, &pFrontend->scrollers[i].count);
            GetPairOfInts(f, &pFrontend->scrollers[i].nbDisplayedAtOnce, &pFrontend->scrollers[i].indexTopItem);
            GetPairOfInts(f, &pFrontend->scrollers[i].indexFirstScrollableItem, &pFrontend->scrollers[i].indexLastScrollableItem);
            pFrontend->scrollers[i].greyedOutBackdrop = GetAnInt(f);
            GetPairOfInts(f, &pFrontend->scrollers[i].x_greyArea, &pFrontend->scrollers[i].y_greyArea);
            GetPairOfInts(f, &pFrontend->scrollers[i].width_greyArea, &pFrontend->scrollers[i].height_greyArea);
            count = pFrontend->scrollers[i].nbDisplayedAtOnce;
            for (j = pFrontend->scrollers[i].indexTopItem; j <= pFrontend->scrollers[i].indexLastScrollableItem; j++) {
                pFrontend->items[j].visible = count > 0;
                count --;
            }
            pFrontend->scrollers[i].indexOfItemAtTop = pFrontend->scrollers[i].indexTopItem;
        }
    }

    pFrontend->count_radios = GetAnInt(f);
    if (pFrontend->count_radios != 0) {
        for (i = 0; i < pFrontend->count_radios; i++) {
            GetPairOfInts(f, &pFrontend->radios[i].id, &pFrontend->radios[i].count);
            GetPairOfInts(f, &pFrontend->radios[i].indexFirstItem, &pFrontend->radios[i].indexLastItem);
            GetPairOfInts(f, &pFrontend->radios[i].indexSelected, &pFrontend->radios[i].greyboxRequested);
            GetPairOfInts(f, &pFrontend->radios[i].x_greybox, &pFrontend->radios[i].y_greybox);
            GetPairOfInts(f, &pFrontend->radios[i].width_greybox, &pFrontend->radios[i].height_greybox);
        }
    }

    pFrontend->count_levels = GetAnInt(f);
    if (pFrontend->count_levels != 0) {
        for (i = 0; i < pFrontend->count_levels; i++) {
            GetPairOfInts(f, &pFrontend->levels[i].field_0x00, &pFrontend->levels[i].field_0x04);
            GetPairOfInts(f, &pFrontend->levels[i].first_item_id, &pFrontend->levels[i].last_item_id);
        }
    }

#ifdef CARPOCALYPSE2_FIX_BUGS
    PFfclose(f);
#else
    fclose(f);
#endif
}

// LoadMenuImages

// LoadMenuModels

void C2_HOOK_FASTCALL DefaultInfunc(tFrontend_spec* pFrontend) {

    if (!pFrontend->loaded) {
        LoadMenuSettings(pFrontend);
        FuckWithWidths(pFrontend);
        if (pFrontend->previous != NULL) {
            pFrontend->previous->isPreviousSomeOtherMenu = 1;
        }
    }
    gFrontend_scrollbars_updated = 0;
    EdgeTriggerModeOff();
    WaitForNoKeys();
    EdgeTriggerModeOn();
}

// FindNextActiveItem

// FUNCTION: CARMA2_HW 0x00467a30
int C2_HOOK_FASTCALL FindPrevActiveItem(tFrontend_spec* pFrontend, int pStart_index) {
    int i;

    for (i = pStart_index - 1; i >= 0; i--) {
        tFrontend_item_spec *item = &pFrontend->items[i];

        if (item->enabled > 0 && item->visible) {
            return i;
        }
    }
    return FindPrevActiveItem(pFrontend, pFrontend->count_items);
}

// RaceIndex


extern int C2_HOOK_CDECL sub_576c00(int pValue, int pDivisor);

// FUNCTION: CARMA2_HW 0x00466bb0
int C2_HOOK_FASTCALL FuckingMakeTheFuckingRaceAndGroupsDisplayHaveTheRightCuntingStuffInIt(tFrontend_spec* pFrontend) {
    int race_index;

    race_index = gProgram_state.current_race_index;
    if (!gIs_boundary_race && !gProgram_state.game_completed) {
        pFrontend->items[7].enabled = -1;
    } else {
        pFrontend->items[7].enabled = 1;
    }
    sub_576c00(race_index, 4);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00467b30
void C2_NAKED C2_HOOK_FASTCALL RefreshRacesScroller(tFrontend_spec* pFrontend) {
#ifndef CARPOCALYPSE2_MATCHING
    char group_text[12];
    int group;
    int race_i;
    int i;

    group = (gCurrent_race_group - gRace_groups) % 10;
    sprintf(group_text, "%s %d", IString_Get(78), group + 1);
    strcpy(pFrontend->items[2].text, group_text);

    race_i = 4 * group;
    for (i = pFrontend->scrollers[0].indexFirstScrollableItem; i <= pFrontend->scrollers[0].indexLastScrollableItem; i++, race_i += 1) {
        tFrontend_item_spec* item = &pFrontend->items[i];

        strcpy(item->text, gRace_list[race_i].name);
        item->radioButton_selected = race_i == gProgram_state.current_race_index;
        if (gRace_list[race_i].is_boundary) {
            item->unlitFont = 2;
            item->highFont = 3;
        } else {
            item->unlitFont = 0;
            item->highFont = 1;
        }
    }
    FuckWithWidths(pFrontend);
    FuckingMakeTheFuckingRaceAndGroupsDisplayHaveTheRightCuntingStuffInIt(pFrontend);
    MungeButtonModels(pFrontend, 0);
#else
    __asm {
            sub esp, 0x18
            mov eax, dword ptr [gRace_groups]
            push ebx
            push ebp
            mov ebp, ecx
            mov ecx, dword ptr [gCurrent_race_group]
            push esi
            sub ecx, eax
            mov eax, 0x2aaaaaab
            imul ecx
            sar edx, 1
            mov ecx, edx
            mov eax, edx
            shr ecx, 0x1f
            add eax, ecx
            mov ecx, 0xa
            cdq
            idiv ecx
            push edi
            mov ecx, 0x4e
            mov dword ptr [esp + 0x14], ebp
            lea ebx, [edx + 1]
            push ebx
            call IString_Get
            lea edx, [esp + 0x20]
            push eax
            push offset gNext_crush_actor_id+0x50c
            push edx
            call sprintf
            lea edi, [esp + 0x2c]
            or ecx, 0xffffffff
            xor eax, eax
            add esp, 0x10
            repne scasb
            not ecx
            sub edi, ecx
            lea edx, [ebp + 0x439]
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            shr ecx, 2
            rep movsd
            mov ecx, eax
            lea eax, [ebx*4 - 4]
            and ecx, 3
            mov dword ptr [esp + 0x10], eax
            rep movsb
            mov ebx, dword ptr [ebp + 0x87a4]
            mov ecx, dword ptr [ebp + 0x87a8]
            cmp ebx, ecx
            jg L00467c80
            lea ecx, [eax + eax*8]
            shl ecx, 4
            add ecx, eax
            mov eax, ebx
            shl eax, 3
            sub eax, ebx
            lea ebp, [ecx*4 + gRace_list+0x190]
            mov ecx, dword ptr [esp + 0x14]
            lea edx, [eax + eax*2]
            lea eax, [ebx + edx*2]
            lea edx, [ecx + eax*8 + 0x16c]
    L00467bf2:
            lea edi, [ebp - 0x190]
            or ecx, 0xffffffff
            xor eax, eax
            lea esi, [edx + 0x1d]
            repne scasb
            not ecx
            sub edi, ecx
            mov dword ptr [esp + 0x18], esi
            mov eax, ecx
            mov esi, edi
            mov edi, dword ptr [esp + 0x18]
            shr ecx, 2
            rep movsd
            mov ecx, eax
            mov eax, dword ptr [esp + 0x10]
            and ecx, 3
            rep movsb
            mov esi, dword ptr [gProgram_state+0x78]
            xor ecx, ecx
            cmp eax, esi
            sete cl
            mov dword ptr [edx], ecx
            mov eax, dword ptr [ebp]
            test eax, eax
            je L00467c48
            mov dword ptr [edx - 0x2c], 2
            mov dword ptr [edx - 0x28], 3
            jmp L00467c56
    L00467c48:
            mov dword ptr [edx - 0x2c], 0
            mov dword ptr [edx - 0x28], 1
    L00467c56:
            mov eax, dword ptr [esp + 0x10]
            mov ecx, dword ptr [esp + 0x14]
            inc eax
            add ebp, 0x244
            mov dword ptr [esp + 0x10], eax
            mov eax, dword ptr [ecx + 0x87a8]
            inc ebx
            add edx, 0x158
            cmp ebx, eax
            jle L00467bf2
            mov ebp, ecx
    L00467c80:
            mov ecx, ebp
            call FuckWithWidths
            mov ecx, dword ptr [gIs_boundary_race]
            mov eax, dword ptr [gProgram_state+0x78]
            test ecx, ecx
            jne L00467cac
            mov ecx, dword ptr [gProgram_state+0x84]
            test ecx, ecx
            jne L00467cac
            mov dword ptr [ebp + 0xac0], 0xffffffff
            jmp L00467cb6
    L00467cac:
            mov dword ptr [ebp + 0xac0], 1
    L00467cb6:
            push 4
            push eax
            call sub_576c00
            add esp, 8
            xor edx, edx
            mov ecx, ebp
            call MungeButtonModels
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x18
            ret
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
    }
#endif
}

// NetworkUpdateSelectedGameInfo

// UpdateNetTrackScroller

// void C2_HOOK_FASTCALL RefreshNetRacesScroller(tFrontend_spec* pFrontend); // Moved for inlining purposes

// NetRaceUp

// NetRaceDn

// SaveSinglePlayerState

// RestoreSinglePlayerState

// MaybeRestoreSavedGame

// NetworkJoinUpdateScroller

// void C2_HOOK_FASTCALL UpdateScrollPositions(tFrontend_spec* pFrontend); // Moved for inlining purposes

// FUNCTION: CARMA2_HW 0x00466ec0
int C2_HOOK_FASTCALL ToggleTyping(tFrontend_spec* pFrontend) {
    if (gTyping) {
        EdgeTriggerModeOff();
        WaitForNoKeys();
        EdgeTriggerModeOn();
        FrontEndShowMouse();
        gTyping = 0;
        StopGettingInputString();
        pFrontend->items[gFrontend_text_input_item_index].unlitFont = 1;
        pFrontend->items[gFrontend_text_input_item_index].highFont = 1;
        gFrontend_original_player_name[0] = '\0';
        SaveOptions();
    } else {
        FrontEndHideMouse();
        gTyping = 1;
        gFrontend_text_input_item_index = gFrontend_selected_item_index;
        pFrontend->items[gFrontend_text_input_item_index].unlitFont = 2;
        pFrontend->items[gFrontend_text_input_item_index].highFont = 3;
        strcpy(pFrontend->items[gFrontend_text_input_item_index].text, gProgram_state.player_name);
        strcpy(gFrontend_original_player_name, gProgram_state.player_name);
        StartGettingInputString(pFrontend->items[gFrontend_text_input_item_index].text, 8);
    }
    return 0;
}

// NewGameToggleTyping

// NetGameToggleTyping

// ChooseSkill

// NewGameOutfunc

// NewGameInfunc

// StartFudge

// FRONTEND_NewGameMenuHandler

// UpdateNetGameTypeScroller

// NetGameTypeUp

// NetGameTypeDn

// MultiplayerSetup

// NetworkJoinSetup

// NetworkJoinMenuInfunc

// NetDisposeGameDetails

// NetworkJoinMenuOutfunc

// NetJoinChooseThisGame

// ChooseNetCar

// InitNetStorageSpace

// DisposeNetStorageSpace

// SetUpOtherNetThings

// DoMultiplayerStartStuff

// NetworkJoinGoAhead

// NetSetRaceType

// NetworkStartJoin

// NetworkStartHost

// NetHostChooseThisRace

// NetCancel

// FRONTEND_NetworkJoinMenuHandler

// OnePlayerSetup

// MenuSetCarImage

// MenuSetDriverImage

// MainMenuInfunc

// MainMenuOutfunc

// MainMenuSelectRace

// testUp

// testDn

// FRONTEND_MainMenuHandler

// UpdateCarInfo

// ChangeCarMenuInfunc

// ChangeCarCancel

// ChangeCarOK

// CarClickPrev

// CarClickNext

// ChangeCarMenuOutfunc

// FRONTEND_ChangeCarMenuHandler

// StartGameInfunc

// FRONTEND_StartGameMenuHandler

// CreditsScreenInfunc

// CreditsScreenOutfunc

// FRONTEND_GenericMenuHandler

// FUNCTION: CARMA2_HW 0x0046c5c0
void C2_HOOK_FASTCALL FRONTEND_CreateMenuButton(tFrontend_brender_item* pFrontend_brender_item, tS16 pX, tS16 pY, tS16 pWidth, tS16 pHeight, br_colour pColour, br_pixelmap* pMap, const char* pText) {
    float map_x;
    float map_y;

    C2_HOOK_BUG_ON(sizeof(tFrontend_brender_item) != 44);

    pFrontend_brender_item->actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    pFrontend_brender_item->model = BrModelAllocate("ButtonModel", 4, 2);
    pFrontend_brender_item->material = BrMaterialAllocate("ButtonMaterial");
    if (pFrontend_brender_item->model == NULL || pFrontend_brender_item->material == NULL || pFrontend_brender_item->actor == NULL) {
#ifdef CARPOCALYPSE2_FIX_BUGS
        FatalError(kFatalError_OOM_S, "");
#else
        FatalError(kFatalError_OOM_S);
#endif
    }
    if (pMap == NULL) {
        pFrontend_brender_item->field_0x10 = BrPixelmapAllocate(BR_PMT_RGBA_4444, 8, 8, NULL, 0);
        BrPixelmapFill(pFrontend_brender_item->field_0x10, 0x80);
        pFrontend_brender_item->field_0xc = BrPixelmapAllocate(BR_PMT_RGBA_4444, 8, 8, NULL, 0);
        BrPixelmapCopy(pFrontend_brender_item->field_0xc, pFrontend_brender_item->field_0x10);
        pFrontend_brender_item->field_0xc->identifier = BrResStrDup(pFrontend_brender_item->field_0xc, pText);
    } else {
        pFrontend_brender_item->field_0x10 = BrPixelmapAllocate(BR_PMT_RGBA_4444, pMap->width, pMap->height, NULL, 0);
        BrPixelmapCopy(pFrontend_brender_item->field_0x10, pMap);
        pFrontend_brender_item->field_0xc = BrPixelmapAllocate(BR_PMT_RGBA_4444, pMap->width, pMap->height, NULL, 0);
        BrPixelmapCopy(pFrontend_brender_item->field_0xc, pFrontend_brender_item->field_0x10);
    }
    BrMapAdd(pFrontend_brender_item->field_0xc);
    pFrontend_brender_item->prims[0].t = BRT_BLEND_B;
    pFrontend_brender_item->prims[0].v.b = 1;
    pFrontend_brender_item->prims[1].t = BRT_OPACITY_X;
    pFrontend_brender_item->prims[1].v.x = BR_FIXED_INT(0x80);
    pFrontend_brender_item->prims[2].t = BR_NULL_TOKEN;
    pFrontend_brender_item->prims[2].v.u32 = 0;
    pFrontend_brender_item->material->extra_prim = pFrontend_brender_item->prims;
    pFrontend_brender_item->actor->model = pFrontend_brender_item->model;
    pFrontend_brender_item->actor->material = pFrontend_brender_item->material;
    pFrontend_brender_item->actor->identifier = BrResStrDup(pFrontend_brender_item->actor, "Button");;
    BrMaterialAdd(pFrontend_brender_item->material);
    pFrontend_brender_item->material->colour = pColour;
    pFrontend_brender_item->material->flags = BR_MATF_ALWAYS_VISIBLE | BR_MATF_FORCE_FRONT;
    pFrontend_brender_item->material->colour_map = pFrontend_brender_item->field_0xc;
    BrMaterialUpdate(pFrontend_brender_item->material, BR_MATU_ALL);
    pFrontend_brender_item->model->flags |= BR_MODF_KEEP_ORIGINAL;

    pFrontend_brender_item->model->vertices[0].p.v[0] = pFrontend_brender_item->model->vertices[1].p.v[0] = (float)pX;
    pFrontend_brender_item->model->vertices[0].p.v[1] = pFrontend_brender_item->model->vertices[3].p.v[1] = -(float)pY;
    pFrontend_brender_item->model->vertices[2].p.v[0] = pFrontend_brender_item->model->vertices[3].p.v[0] = pFrontend_brender_item->model->vertices[1].p.v[0] + (float)pWidth;
    pFrontend_brender_item->model->vertices[1].p.v[1] = pFrontend_brender_item->model->vertices[2].p.v[1] = pFrontend_brender_item->model->vertices[3].p.v[1] - (float)pHeight;
    pFrontend_brender_item->model->vertices[0].p.v[2] = -1.1f;
    pFrontend_brender_item->model->vertices[1].p.v[2] = -1.1f;
    pFrontend_brender_item->model->vertices[2].p.v[2] = -1.1f;
    pFrontend_brender_item->model->vertices[3].p.v[2] = -1.1f;
    pFrontend_brender_item->model->faces[0].vertices[0] = 0;
    pFrontend_brender_item->model->faces[0].vertices[1] = 1;
    pFrontend_brender_item->model->faces[0].vertices[2] = 2;
    pFrontend_brender_item->model->faces[1].vertices[0] = 2;
    pFrontend_brender_item->model->faces[1].vertices[1] = 3;
    pFrontend_brender_item->model->faces[1].vertices[2] = 0;
    BrModelAdd(pFrontend_brender_item->model);
    if (pMap != NULL) {
        map_x = (float)pWidth / (float)HighResHeadupWidth(pMap->width);
        map_y = (float)pHeight / (float)HighResHeadupWidth(pMap->height);
    } else {
        map_x = (float)pWidth / (float)HighResHeadupWidth(8);
        map_y = (float)pHeight / (float)HighResHeadupWidth(8);
    }
    BrVector2Set(&pFrontend_brender_item->model->vertices[0].map, 0.0f, 0.0f);
    BrVector2Set(&pFrontend_brender_item->model->vertices[1].map, 0.0f, map_y);
    /* NOTE: retail emits u = (float)pHeight for the right edge here — a quirk
     * that only renders correctly under wrap-sampling; we keep the wrapped
     * equivalent (u = map_x) so our sampler produces the same texels */
    BrVector2Set(&pFrontend_brender_item->model->vertices[2].map, map_x, map_y);
    BrVector2Set(&pFrontend_brender_item->model->vertices[3].map, map_x, 0.0f);
    BrModelUpdate(pFrontend_brender_item->model, BR_MODU_ALL);
    BrActorAdd(gFrontend_actor, pFrontend_brender_item->actor);
    pFrontend_brender_item->actor->render_style = BR_RSTYLE_NONE;
}

// FUNCTION: CARMA2_HW 0x0046c970
int C2_HOOK_FASTCALL FRONTEND_CreateMenu(tFrontend_spec* pFrontend_spec) {
    char s[256];
    char s2[256];
    const char* name;
    tTWTVFS twt;
    int count_items;
    int i;

    sprintf(s, "START OF FRONTEND_CreateMenu for menu \'%s\'", pFrontend_spec->name);
    PrintMemoryDump(0, s);
    if (pFrontend_spec->create != NULL) {
        pFrontend_spec->create(pFrontend_spec);
    }
    name = pFrontend_spec->backdrop_name;
    gFrontend_count_brender_items = 0;
    count_items = pFrontend_spec->count_items;
    if (name != NULL && strlen(name) != 0) {
        if (!gFrontend_remove_current_backdrop) {

            PathCat(s2, gApplication_path, "INTRFACE");
            PathCat(s2, s2, "BACKDROP");
            strcpy(s, name);
            s[strlen(s) - 4] = '\0';
            PathCat(s2, s2, s);
            twt = OpenPackFileAndSetTiffLoading(s2);
            gFrontend_backdrop = GetThisFuckingPixelmapPleaseMrTwatter(s2, name);
            ClosePackFileAndSetTiffLoading(twt);

            if (gFrontend_backdrop == NULL) {
                BrFailure("FRONTEND: Error loading background graphics.", 0);
            }
            PixelmapSwapByteOrder(gFrontend_backdrop);
            BrMapAdd(gFrontend_backdrop);
        } else if (gFrontend_backdrop != NULL) {

            BrMapRemove(gFrontend_backdrop);
            BrPixelmapFree(gFrontend_backdrop);

            PathCat(s2, gApplication_path, "INTRFACE");
            PathCat(s2, s2, "BACKDROP");
            strcpy(s, name);
            s[strlen(s) - 4] = '\0';
            PathCat(s2, s2, s);
            twt = OpenPackFileAndSetTiffLoading(s2);
            gFrontend_backdrop = GetThisFuckingPixelmapPleaseMrTwatter(s2, name);
            ClosePackFileAndSetTiffLoading(twt);

            if (gFrontend_backdrop == NULL) {
                BrFailure("FRONTEND: Error loading background graphics.", 0);
            }
            PixelmapSwapByteOrder(gFrontend_backdrop);
            BrMapAdd(gFrontend_backdrop);
        }
    }
    StartMouseCursor();
    for (i = 0; i < count_items; i++) {
        br_pixelmap* map;

        if (pFrontend_spec->items[i].map_index != 0) {
            map = gFrontend_images[pFrontend_spec->items[i].map_index];
        } else {
            map = NULL;
        }
        if (i == gFrontend_selected_item_index) {
            FRONTEND_CreateMenuButton(
                &gFrontend_brender_items[gFrontend_count_brender_items],
                (tS16)pFrontend_spec->items[i].x,
                pFrontend_spec->items[i].y,
                pFrontend_spec->items[i].width,
                pFrontend_spec->items[i].height,
                gFrontend_some_color,
                map,
                pFrontend_spec->items[i].text);
        } else {
            FRONTEND_CreateMenuButton(
                &gFrontend_brender_items[gFrontend_count_brender_items],
                (tS16)pFrontend_spec->items[i].x,
                pFrontend_spec->items[i].y,
                pFrontend_spec->items[i].width,
                pFrontend_spec->items[i].height,
                gFrontend_some_color,
                map,
                pFrontend_spec->items[i].text);
        }
        gFrontend_count_brender_items++;
    }
    ResetInterfaceTimeout();
    FRONTEND_CreateMenuButton(&gFrontend_brender_items[99],
        0,
        0,
        0,
        0,
        gFrontend_some_color,
        NULL,
        "");
    if (gMouse_in_use) {
        gFrontend_selected_item_index = 99;
    } else {
        gFrontend_selected_item_index = 0;
    }
    sprintf(s, "END OF FRONTEND_CreateMenu for menu \'%s\'", pFrontend_spec->name);
    PrintMemoryDump(0, s);
    return 1;
}

// FUNCTION: CARMA2_HW 0x0046ccb0
int C2_HOOK_FASTCALL FRONTEND_DestroyMenu(tFrontend_spec* pFrontend) {
    char s[256];
    tU8 i;

    sprintf(s, "START OF FRONTEND_DestroyMenu for menu '%s'", pFrontend->name);
    PrintMemoryDump(0, s);
    if (pFrontend->destroy != NULL) {
        pFrontend->destroy(pFrontend);
    }
    while (gFrontend_actor->children != NULL && strcmp("Backdrop", gFrontend_actor->children->identifier) != 0) {
        BrActorRemove(gFrontend_actor->children);
    }
    for (i = 0; i < gFrontend_count_brender_items; i++) {
        BrMaterialRemove(gFrontend_brender_items[i].material);
        BrModelRemove(gFrontend_brender_items[i].model);
        gFrontend_brender_items[i].actor->render_style = BR_RSTYLE_NONE;
        BrMaterialFree(gFrontend_brender_items[i].material);
        gFrontend_brender_items[i].material = NULL;
        BrModelFree(gFrontend_brender_items[i].model);
        gFrontend_brender_items[i].model = NULL;
        BrActorFree(gFrontend_brender_items[i].actor);
        gFrontend_brender_items[i].actor = NULL;
        if (gFrontend_brender_items[i].field_0xc != NULL) {
            BrMapRemove(gFrontend_brender_items[i].field_0xc);
            BrPixelmapFree(gFrontend_brender_items[i].field_0xc);
            gFrontend_brender_items[i].field_0xc = NULL;
        }
        if (gFrontend_brender_items[i].field_0x10 != NULL) {
            BrPixelmapFree(gFrontend_brender_items[i].field_0x10);
            gFrontend_brender_items[i].field_0x10 = NULL;
        }
    }
    /* FIXME: parametrize last item */
    BrMaterialRemove(gFrontend_brender_items[99].material);
    BrModelRemove(gFrontend_brender_items[99].model);
    gFrontend_brender_items[99].actor->render_style = BR_RSTYLE_NONE;
    BrMaterialFree(gFrontend_brender_items[99].material);
    gFrontend_brender_items[99].material = NULL;
    BrModelFree(gFrontend_brender_items[99].model);
    gFrontend_brender_items[99].model = NULL;
    BrActorFree(gFrontend_brender_items[99].actor);
    gFrontend_brender_items[99].actor = NULL;
    EndMouseCursor();
    sprintf(s, "END OF FRONTEND_DestroyMenu for menu '%s'", pFrontend->name);
    PrintMemoryDump(0, s);
    return 1;
}

// FUNCTION: CARMA2_HW 0x0046cec0
void C2_HOOK_FASTCALL FRONTEND_PingPongFlash(void) {
    int time;

    time = (int)PDGetTotalTime() % 750;
    if (time < 375) {
        gFrontend_throb_factor = (double)time / (750.0 / 2.0);
    } else {
        gFrontend_throb_factor = (double)(750 - time) / (750.0 / 2.0);
    }
}

// FRONTEND_DrawMenu

// FRONTEND_Setup2D

// FRONTEND_Setup

// FUNCTION: CARMA2_HW 0x0046d8b0
void C2_HOOK_FASTCALL MaybeDoMouseCursor(void) {
    if (!gFrontend_suppress_mouse) {
        PossibleLock(1);
        DoMouseCursor();
        PossibleUnlock(1);
    }
}

void C2_HOOK_FASTCALL ResetInterfaceTimeout(void) {

    gFrontend_time_last_input = PDGetTotalTime();
}

// FUNCTION: CARMA2_HW 0x0046cf10
void C2_HOOK_FASTCALL FRONTEND_Setup2D(void) {
    br_camera* camera;

    gFrontend_actor = BrActorAllocate(BR_ACTOR_NONE, NULL);
    gFrontend_camera = BrActorAllocate(BR_ACTOR_CAMERA, NULL);
    camera = gFrontend_camera->type_data;
    camera->type = BR_CAMERA_PARALLEL;
    camera->field_of_view = 0x1ffe;
    camera->hither_z = 1.f;
    camera->yon_z = 3.f;
    camera->width = 640.f;
    camera->height = 480.f;
    BrActorAdd(gFrontend_actor, gFrontend_camera);
    gFrontend_backdrop0_material_prims[0].t = BRT_BLEND_B;
    gFrontend_backdrop0_material_prims[0].v.b = 1;
    gFrontend_backdrop0_material_prims[1].t = BRT_OPACITY_X;
    gFrontend_backdrop0_material_prims[1].v.x = 0xb00000;
    gFrontend_backdrop0_material_prims[2].t = BR_NULL_TOKEN;
    gFrontend_backdrop0_material_prims[2].v.u32 = 0;
    gFrontend_backdrop1_material_prims[0].t = BRT_BLEND_B;
    gFrontend_backdrop1_material_prims[0].v.b = 1;
    gFrontend_backdrop1_material_prims[1].t = BRT_OPACITY_X;
    gFrontend_backdrop1_material_prims[1].v.x = 0xb00000;
    gFrontend_backdrop1_material_prims[2].t = BR_NULL_TOKEN;
    gFrontend_backdrop1_material_prims[2].v.u32 = 0;
    gFrontend_backdrop2_material_prims[0].t = BRT_BLEND_B;
    gFrontend_backdrop2_material_prims[0].v.b = 1;
    gFrontend_backdrop2_material_prims[1].t = BRT_OPACITY_X;
    gFrontend_backdrop2_material_prims[1].v.x = 0xb00000;
    gFrontend_backdrop2_material_prims[2].t = BR_NULL_TOKEN;
    gFrontend_backdrop2_material_prims[2].v.u32 = 0;
    gFrontend_backdrop_materials[0] = BrMaterialAllocate("Backdrop_material1");
    gFrontend_backdrop_materials[1] = BrMaterialAllocate("Backdrop_material2");
    gFrontend_backdrop_materials[2] = BrMaterialAllocate("Backdrop_material3");
    gFrontend_backdrop_actors[0] = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    gFrontend_backdrop_actors[1] = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    gFrontend_backdrop_actors[2] = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    gFrontend_backdrop_materials[0]->colour = 0;
    gFrontend_backdrop_materials[1]->colour = 0;
    gFrontend_backdrop_materials[2]->colour = 0;
    gFrontend_backdrop_materials[0]->index_base = 0;
    gFrontend_backdrop_materials[1]->index_base = 0;
    gFrontend_backdrop_materials[2]->index_base = 0;
    gFrontend_backdrop_materials[0]->index_range = 1;
    gFrontend_backdrop_materials[1]->index_range = 1;
    gFrontend_backdrop_materials[2]->index_range = 1;
    gFrontend_backdrop_materials[0]->extra_prim = gFrontend_backdrop0_material_prims;
    gFrontend_backdrop_materials[1]->extra_prim = gFrontend_backdrop1_material_prims;
    gFrontend_backdrop_materials[2]->extra_prim = gFrontend_backdrop2_material_prims;
    BrMaterialAdd(gFrontend_backdrop_materials[0]);
    BrMaterialAdd(gFrontend_backdrop_materials[1]);
    BrMaterialAdd(gFrontend_backdrop_materials[2]);
    gFrontend_backdrop_actors[0]->material = gFrontend_backdrop_materials[0];
    gFrontend_backdrop_actors[0]->identifier = BrResStrDup(gFrontend_backdrop_actors[0], "Backdrop");;
    gFrontend_backdrop_actors[1]->material = gFrontend_backdrop_materials[1];
    gFrontend_backdrop_actors[1]->identifier = BrResStrDup(gFrontend_backdrop_actors[1], "Backdrop");
    gFrontend_backdrop_actors[2]->material = gFrontend_backdrop_materials[2];
    gFrontend_backdrop_actors[2]->identifier = BrResStrDup(gFrontend_backdrop_actors[2], "Backdrop");
    BrActorAdd(gFrontend_actor, gFrontend_backdrop_actors[0]);
    BrActorAdd(gFrontend_actor, gFrontend_backdrop_actors[1]);
    BrActorAdd(gFrontend_actor, gFrontend_backdrop_actors[2]);
}


// ProcessInputString

// FUNCTION: CARMA2_HW 0x0046e5c0
int C2_HOOK_FASTCALL StopGettingInputString(void) {
    gFrontend_current_input = NULL;
    return 0;
}

// FUNCTION: CARMA2_HW 0x0046e5d0
int C2_HOOK_FASTCALL StartGettingInputString(char* pBuffer, int pBuffer_size) {
    PDClearKeyboardBuffer();
    gINT_0059b0d8 = -1;
    gFrontend_maximum_input_length = pBuffer_size;
    gFrontend_current_input = pBuffer;
    DodgyPause(200);
    return 1;
}

// DisposeWrecksGallery

// ScrollToNextCar

// ScrollToPrevCar

// BuyCurrentCar

// WrecksInFunc

// KeepInRange

// WrecksOutFunc

// WrecksUpdateFunc

// GotItAlready

// GetCarSelectedByMouse

// WreckPick

// FUNCTION: CARMA2_HW 0x0046f560
intptr_t C2_HOOK_CDECL HeirarchyPick(br_actor* a, void* ref) {
    if (a == (br_actor*)ref) {
        gHierarchy_has_actor = 1;
    }
    BrActorEnum(a, HeirarchyPick, ref);
    return 0;
}

// FUNCTION: CARMA2_HW 0x0046f590
void C2_HOOK_FASTCALL DodgyPause(tU32 pTime) {
    tU32 start;

    start = PDGetTotalTime();
    while (PDGetTotalTime() < start + pTime) {
        /* brrrr */
    }
}

// FUNCTION: CARMA2_HW 0x0046f5b0
void C2_HOOK_FASTCALL MorphBlob(br_model* pModel_from, br_model* pModel_to, br_model* pModel, int pStep, int pCount_steps) {
    float t;
    int i;

    t = (float)pStep / (float)pCount_steps;
    for (i = 0; i < pModel->nvertices; i++) {

        pModel->vertices[i].p.v[0] = pModel_from->vertices[i].p.v[0] + (pModel_to->vertices[i].p.v[0] - pModel_from->vertices[i].p.v[0]) * t;
        pModel->vertices[i].p.v[1] = pModel_from->vertices[i].p.v[1] + (pModel_to->vertices[i].p.v[1] - pModel_from->vertices[i].p.v[1]) * t;
    }
    BrModelUpdate(pModel, BR_MODU_VERTEX_POSITIONS);
}

// FUNCTION: CARMA2_HW 0x0046f630
void C2_NAKED C2_HOOK_FASTCALL ScrollCredits(void) {
#ifndef CARPOCALYPSE2_MATCHING
    int y;
    int i;

    y = (int)(430.0 - (float)(PDGetTotalTime() - gCredits_scroll_start) * 0.03);
    for (i = 0; i < gCredits_line_count; i++) {
        y += gCredits_heights[i];
        if (y < 430 && y > 30) {
            if (gCredits_throbs[i]) {
                SolidPolyFontText(gCredits_texts[i], 320, y, gCredits_fonts[i] - 1, eJust_centre, 1);
                TransparentPolyFontText(gCredits_texts[i], 320, y, gCredits_fonts[i], eJust_centre, 1, gFrontend_throb_factor);
            } else {
                SolidPolyFontText(gCredits_texts[i], 320, y, gCredits_fonts[i], eJust_centre, 1);
            }
        }
    }
    if ((float)(PDGetTotalTime() - gCredits_scroll_start) * 0.03 > (float)gCredits_total_height) {
        gCredits_scroll_start = PDGetTotalTime();
    }
#else
    __asm {
            push ecx
            push esi
            push edi
            call PDGetTotalTime
            sub eax, dword ptr [gCredits_scroll_start]
            mov dword ptr [esp + 8], eax
            fild dword ptr [esp + 8]
            fmul qword ptr [gDouble_0058a068+0x60]
            fsubr qword ptr [gDouble_0058a068+0x68]
            call _ftol
            mov esi, eax
            mov eax, dword ptr [gCredits_line_count]
            xor edi, edi
            test eax, eax
            jle L0046f70b
    L0046f668:
            mov eax, dword ptr [gCredits_heights]
            add esi, dword ptr [eax + edi*4]
            cmp esi, 0x1ae
            jge L0046f6fd
            cmp esi, 0x1e
            jle L0046f6fd
            mov ecx, dword ptr [gCredits_throbs]
            push 1
            push 2
            cmp dword ptr [ecx + edi*4], 0
            je L0046f6e0
            mov edx, dword ptr [gCredits_fonts]
            mov ecx, dword ptr [gCredits_texts]
            mov eax, dword ptr [edx + edi*4]
            mov ecx, dword ptr [ecx + edi*4]
            dec eax
            mov edx, 0x140
            push eax
            push esi
            call SolidPolyFontText
            mov edx, dword ptr [gFrontend_throb_factor+0x4]
            mov eax, dword ptr [gFrontend_throb_factor]
            mov ecx, dword ptr [gCredits_fonts]
            push edx
            push eax
            mov eax, dword ptr [gCredits_texts]
            mov edx, dword ptr [ecx + edi*4]
            push 1
            mov ecx, dword ptr [eax + edi*4]
            push 2
            push edx
            push esi
            mov edx, 0x140
            call TransparentPolyFontText
            jmp L0046f6fd
    L0046f6e0:
            mov ecx, dword ptr [gCredits_fonts]
            mov eax, dword ptr [gCredits_texts]
            mov edx, dword ptr [ecx + edi*4]
            mov ecx, dword ptr [eax + edi*4]
            push edx
            push esi
            mov edx, 0x140
            call SolidPolyFontText
    L0046f6fd:
            mov eax, dword ptr [gCredits_line_count]
            inc edi
            cmp edi, eax
            jl L0046f668
    L0046f70b:
            call PDGetTotalTime
            sub eax, dword ptr [gCredits_scroll_start]
            mov dword ptr [esp + 8], eax
            fild dword ptr [esp + 8]
            fmul qword ptr [gDouble_0058a068+0x60]
            fild dword ptr [gCredits_total_height]
            fcompp
            fnstsw ax
            test ah, 1
            je L0046f73d
            call PDGetTotalTime
            mov dword ptr [gCredits_scroll_start], eax
    L0046f73d:
            pop edi
            pop esi
            pop ecx
            ret
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
    }
#endif
}

// LoadGameInFunc

// FUNCTION: CARMA2_HW 0x0046f8a0
br_actor* C2_HOOK_FASTCALL CreateAPOactor(void) {
    br_pixelmap* map;
    br_actor* actor;
    br_material* material;
    br_model* model;

    map = BrPixelmapAllocate(gBack_screen->type, 64, 64, NULL, 0);
    model = BrModelAllocate("Billboard Model", 4, 2);
    material = BrMaterialAllocate("Billboard Material");
    actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    actor->identifier = "Billboard Actor";
    actor->model = model;
    actor->material = material;
    actor->render_style = BR_RSTYLE_FACES;
    model->faces[0].vertices[0] = 0;
    model->faces[0].vertices[1] = 1;
    model->faces[0].vertices[2] = 2;
    model->faces[1].vertices[0] = 1;
    model->faces[1].vertices[1] = 3;
    model->faces[1].vertices[2] = 2;
    model->faces[0].material = NULL;
    model->faces[1].material = NULL;
    BrVector3Set(&model->vertices[0].p,  0.0f,   0.0f, -1.05f);
    BrVector3Set(&model->vertices[1].p, 40.0f,   0.0f, -1.05f);
    BrVector3Set(&model->vertices[2].p,  0.0f, -12.0f, -1.05f);
    BrVector3Set(&model->vertices[3].p, 40.0f, -12.0f, -1.05f);
    material->colour = 0;
    material->colour_map = map;
    material->flags = BR_MATF_ALWAYS_VISIBLE;
    model->flags |= BR_MODF_KEEP_ORIGINAL;
    BrMapAdd(map);
    BrMaterialAdd(material);
    BrModelAdd(model);
    return actor;
}

// FUNCTION: CARMA2_HW 0x0046f9e0
void C2_HOOK_FASTCALL KillAPOactor(br_actor* pActor) {
    if (pActor != NULL) {

        if (pActor->material->colour_map != NULL) {
            BrMapRemove(pActor->material->colour_map);
            BrPixelmapFree(pActor->material->colour_map);
        }
        if (pActor->material != NULL) {
            BrMaterialRemove(pActor->material);
            BrMaterialFree(pActor->material);
        }
        if (pActor->model != NULL) {
            BrModelRemove(pActor->model);
            BrModelFree(pActor->model);
        }
        if (pActor->parent != NULL) {
            BrActorRemove(pActor);
        }
        BrActorFree(pActor);
    }
}

// FUNCTION: CARMA2_HW 0x0046fa60
int C2_NAKED C2_HOOK_FASTCALL LoadGameOutFunc(tFrontend_spec* pFrontend) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;

    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gFrontend_billboard_actors); i++) {
        KillAPOactor(gFrontend_billboard_actors[i]);
    }
    BrActorFree(gFrontend_menu_camera);
    gFrontend_menu_camera = NULL;
    EndSavedGamesList();
    DRS3StartSound(gEffects_outlet, eSoundId_Swingout);
    return 0;
#else
    __asm {
            push esi
            push edi
            mov edi, offset gFrontend_billboard_actors
    L0046fa67:
            mov esi, dword ptr [edi]
            test esi, esi
            je L0046fae0
            mov eax, dword ptr [esi + 0x1c]
            mov eax, dword ptr [eax + 0x40]
            test eax, eax
            je L0046fa8f
            push eax
            call BrMapRemove
            mov ecx, dword ptr [esi + 0x1c]
            add esp, 4
            mov edx, dword ptr [ecx + 0x40]
            push edx
            call BrPixelmapFree
            add esp, 4
    L0046fa8f:
            mov eax, dword ptr [esi + 0x1c]
            test eax, eax
            je L0046faab
            push eax
            call BrMaterialRemove
            mov eax, dword ptr [esi + 0x1c]
            add esp, 4
            push eax
            call BrMaterialFree
            add esp, 4
    L0046faab:
            mov eax, dword ptr [esi + 0x18]
            test eax, eax
            je L0046fac7
            push eax
            call BrModelRemove
            mov ecx, dword ptr [esi + 0x18]
            add esp, 4
            push ecx
            call BrModelFree
            add esp, 4
    L0046fac7:
            mov eax, dword ptr [esi + 0xc]
            test eax, eax
            je L0046fad7
            push esi
            call BrActorRemove
            add esp, 4
    L0046fad7:
            push esi
            call BrActorFree
            add esp, 4
    L0046fae0:
            add edi, 4
            cmp edi, offset gNet_settings
            jl L0046fa67
            mov edx, dword ptr [gFrontend_menu_camera]
            push edx
            call BrActorFree
            add esp, 4
            mov dword ptr [gFrontend_menu_camera], 0
            call EndSavedGamesList
            mov ecx, dword ptr [gEffects_outlet]
            mov edx, 0xbbf
            call DRS3StartSound
            xor eax, eax
            pop edi
            pop esi
            ret
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0046fb30
int C2_HOOK_FASTCALL LoadGameScrollUp(tFrontend_spec* pFrontend) {
    if (gFrontend_load_game_index_top > 0 && gFrontend_scroll_time_left == 0) {
        gFrontend_load_game_index_top -= 1;
        DRS3StartSound(gEffects_outlet, eSoundId_LeftButton);
        gFrontend_scroll_time_left += gFrontend_scroll_time_increment;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0046fbd0
int C2_HOOK_FASTCALL LoadSlot1(tFrontend_spec* pFrontend) {
    LOAD_SLOT_N(pFrontend, 0);
}

// FUNCTION: CARMA2_HW 0x0046fc50
int C2_HOOK_FASTCALL LoadSlot2(tFrontend_spec* pFrontend) {
    LOAD_SLOT_N(pFrontend, 1);
}

// FUNCTION: CARMA2_HW 0x0046fcd0
int C2_HOOK_FASTCALL LoadSlot3(tFrontend_spec* pFrontend) {
    LOAD_SLOT_N(pFrontend, 2);
}

// FUNCTION: CARMA2_HW 0x0046fd50
int C2_HOOK_FASTCALL LoadSlot4(tFrontend_spec* pFrontend) {
    LOAD_SLOT_N(pFrontend, 3);
}

// FUNCTION: CARMA2_HW 0x0046fdd0
int C2_HOOK_FASTCALL LoadSlot5(tFrontend_spec* pFrontend) {
    LOAD_SLOT_N(pFrontend, 4);
}

// FUNCTION: CARMA2_HW 0x0046fe50
int C2_HOOK_FASTCALL LoadSlot6(tFrontend_spec* pFrontend) {
    LOAD_SLOT_N(pFrontend, 5);
}

// FUNCTION: CARMA2_HW 0x0046fed0
int C2_HOOK_FASTCALL LoadSlot7(tFrontend_spec* pFrontend) {
    LOAD_SLOT_N(pFrontend, 6);
}

// FUNCTION: CARMA2_HW 0x0046ff50
int C2_HOOK_FASTCALL LoadSlot8(tFrontend_spec* pFrontend) {
    LOAD_SLOT_N(pFrontend, 7);
}

#undef LOAD_SLOT_N

// FUNCTION: CARMA2_HW 0x0046ffd0
int C2_HOOK_FASTCALL TryToLoadGame(int pN) {
    if (!DoLoadGame(gFrontend_load_game_index_top + pN)) {
        DRS3StartSound(gEffects_outlet, eSoundId_CantAffordPart);
        return 0;
    } else {
        gAbandon_game = 1;
        gNo_credits_APO_restore = 1;
        DRS3StartSound(gEffects_outlet, eSoundId_Done);
        return 1;
    }
}

// FUNCTION: CARMA2_HW 0x00470020
int C2_NAKED C2_HOOK_FASTCALL LoadGameUpdateFunc(tFrontend_spec* pFrontend) {
#ifndef CARPOCALYPSE2_MATCHING
    int selected_item_index;
    tU32 prev;
    int mouse_x;
    int mouse_y;
    int i;

    ServiceGame();
    selected_item_index = gFrontend_selected_item_index;
    prev = gFrontend_scroll_last_update;
    gFrontend_scroll_last_update = PDGetTotalTime();
    gFrontend_scroll_time_left -= gFrontend_scroll_last_update - prev;
    if (gFrontend_scroll_time_left < 0) {
        gFrontend_scroll_time_left = 0;
    }
    gFrontend_mouse_down = 0;
    gFrontend_scroll_time_increment = 50;
    if (gMouse_in_use) {

        gFrontend_selected_item_index = 0;
        ResetInterfaceTimeout();
        GetMousePosition(&mouse_x, &mouse_y);
        gNet_join_host_result = GetItemAtMousePos(pFrontend, mouse_x, mouse_y);
        if (gNet_join_host_result != -1) {
            gFrontend_selected_item_index = gNet_join_host_result;
        }
        gPrev_frontend_mouse_down = gFrontend_mouse_down;
        gFrontend_mouse_down = EitherMouseButtonDown();
        if (gFrontend_mouse_down && !gPrev_frontend_mouse_down) {
            gFrontend_scroll_time_increment = 175;
        } else {
            gFrontend_scroll_time_increment = 25;
        }
    }
    if (gFrontend_load_game_index_top > 0) {
        pFrontend->items[17].enabled = kFrontendItemEnabled_enabled;
    } else {
        pFrontend->items[17].enabled = kFrontendItemEnabled_disabled;
    }
    if (gFrontend_load_game_index_top + 8 < gFrontend_count_saved_games) {
        pFrontend->items[18].enabled = kFrontendItemEnabled_enabled;
    } else {
        pFrontend->items[18].enabled = kFrontendItemEnabled_disabled;
    }
    for (i = 0; i < 8; i++) {
        int j;
        int font;
        tSave_game* save_game;
        char* text_ptr;
        char text[128];
        int y_text = 125 + i * 27;
        int y_apo = 128 + i * 27;
        char date_str[12];
        char time_str[16];
        int len_date;

        font = (i == gFrontend_selected_item_index - 1) ? kPolyfont_hand_green_15pt_lit : kPolyfont_hand_green_15pt_unlit;
        save_game = GetNthSavedGame(gFrontend_load_game_index_top + i);
        if (save_game == NULL) {
            continue;
        }
        memset(text, 0, sizeof(text));
        strcpy(date_str, save_game->date);
        strcpy(time_str, save_game->time);

        len_date = strlen(date_str);
        text_ptr = text;
        for (j = 0; j < len_date; j++) {

            if (isalnum(date_str[j])) {
                *text_ptr++ = date_str[j];
            } else if (date_str[j] == '/') {
                *text_ptr++ = '-';
            }
        }
        SolidPolyFontText(text, 42, y_text, font, eJust_left, 1);

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-truncation"
#endif

        memset(text, 0, sizeof(text));
        strncpy(text, time_str, 5);

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

        SolidPolyFontText(text, 120, y_text, font, eJust_left, 1);

        memset(text, 0, sizeof(text));
        strcpy(text, save_game->player_name);
        PolyClipName(text, font, 127);
        SolidPolyFontText(text, 190, y_text, font, 0, 1);

        if (save_game->game_completed) {
            sprintf(text, "! %i", 1 + (gRace_list[save_game->current_race_index].group - gRace_groups));
        } else if (save_game->is_boundary_race) {
            sprintf(text, "%c %i", '\x1f', 1 + (gRace_list[save_game->current_race_index].group - gRace_groups));
        } else {
            sprintf(text, "%i", 1 + (gRace_list[save_game->current_race_index].group - gRace_groups));
        }
        SolidPolyFontText(text, 325, y_text, font, eJust_right, 1);

        SolidPolyFontText(MungeCommas(save_game->credits), 366, y_text, font, eJust_left, 1);

        BuildAPO(save_game->apo_levels[0], save_game->apo_potential[0], i, 0);
        BuildAPO(save_game->apo_levels[1], save_game->apo_potential[1], i, 1);
        BuildAPO(save_game->apo_levels[2], save_game->apo_potential[2], i, 2);
        PrepareAPO(i);
        PrintAPO(450, y_apo, i, 0);
        PrintAPO(500, y_apo, i, 1);
        PrintAPO(550, y_apo, i, 2);
        sprintf(pFrontend->items[15].text, "%i-%i (%i)",
            gFrontend_load_game_index_top + 1,
            gFrontend_load_game_index_top + 8,
            gFrontend_count_saved_games);
    }

    if (PDKeyDown(51) || PDKeyDown(52) || (gFrontend_mouse_down && gNet_join_host_result != -1)) {
        int result;
        tFrontend_spec* next;
        int go_back;

        ToggleSelection(pFrontend);
        if (pFrontend->items[gFrontend_selected_item_index].field_0xc == 2) {
            return 2;
        } else if (pFrontend->items[gFrontend_selected_item_index].field_0xc == 1) {
            gFrontend_leave_current_menu = 1;
        }
        if (pFrontend->items[gFrontend_selected_item_index].action != NULL) {
            result = pFrontend->items[gFrontend_selected_item_index].action(pFrontend);
        } else {
            result = pFrontend->items[selected_item_index].field_0xc;
        }
        next = pFrontend->items[gFrontend_selected_item_index].menuInfo;
        go_back = 0;
        if (next == (tFrontend_spec*)(uintptr_t)0x1) {
            go_back = 1;
            next = pFrontend->previous;
        }
        if (pFrontend->items[gFrontend_selected_item_index].action != NULL) {
            if (next == NULL && pFrontend->items[gFrontend_selected_item_index].action != temp) {
                DRS3StartSound(gEffects_outlet, eSoundId_Done);
            }
            pFrontend->items[gFrontend_selected_item_index].action(pFrontend);
        }
        if (next != NULL) {
            if (!go_back) {
                Generic_LinkInEffect();
            } else {
                Generic_LinkOutEffect();
            }
            gCurrent_frontend_spec->default_item = gFrontend_selected_item_index;
            for (i = 0; i < gCurrent_frontend_spec->count_scrollers; i++) {
                gCurrent_frontend_spec->scrollers[i].indexOfItemAtTop = gCurrent_frontend_spec->scrollers[i].indexTopItem;
            }
            FRONTEND_DestroyMenu(pFrontend);
            gCurrent_frontend_spec = next;
            FRONTEND_CreateMenu(next);
            if (gCurrent_frontend_spec != pFrontend->previous) {
                gCurrent_frontend_spec->previous = pFrontend;
            }
            Morph_Initialise(pFrontend, gCurrent_frontend_spec);
            if (gCurrent_frontend_spec == &gFrontend_QUIT) {
                gFrontend_selected_item_index = 0;
            } else {
                gFrontend_selected_item_index = gCurrent_frontend_spec->default_item;
            }
            for (i = 0; i < gCurrent_frontend_spec->count_scrollers; i++) {
                gCurrent_frontend_spec->scrollers[i].indexOfItemAtTop = gCurrent_frontend_spec->scrollers[i].indexTopItem;
            }
            UpdateScrollPositions(gCurrent_frontend_spec);
            return result;
        } else {
            return go_back ? 1 : result;
        }
    } else {
        return gFrontend_leave_current_menu ? 1 : 0;
    }
#else
    __asm {
            sub esp, 0xc4
            push ebx
            push ebp
            push esi
            push edi
            mov edi, ecx
            mov dword ptr [esp + 0x20], edi
            call ServiceGame
            mov eax, dword ptr [gFrontend_selected_item_index]
            mov dword ptr [esp + 0x28], eax
            call PDGetTotalTime
            mov edx, dword ptr [gFrontend_scroll_last_update]
            mov ecx, eax
            mov dword ptr [gFrontend_scroll_last_update], eax
            mov eax, dword ptr [gFrontend_scroll_time_left]
            sub ecx, edx
            mov esi, 0
            sub eax, ecx
            mov dword ptr [gFrontend_scroll_time_left], eax
            jns L0047006b
            mov dword ptr [gFrontend_scroll_time_left], esi
    L0047006b:
            mov eax, dword ptr [gMouse_in_use]
            or ebx, 0xffffffff
            cmp eax, esi
            mov dword ptr [gFrontend_mouse_down], esi
            mov dword ptr [gFrontend_scroll_time_increment], 0x32
            je L004700f9
            mov dword ptr [gFrontend_selected_item_index], esi
            call PDGetTotalTime
            lea edx, [esp + 0x2c]
            lea ecx, [esp + 0x24]
            mov dword ptr [gFrontend_time_last_input], eax
            call GetMousePosition
            mov ecx, dword ptr [esp + 0x2c]
            mov edx, dword ptr [esp + 0x24]
            push ecx
            mov ecx, edi
            call GetItemAtMousePos
            cmp eax, ebx
            mov dword ptr [gNet_join_host_result], eax
            je L004700c2
            mov dword ptr [gFrontend_selected_item_index], eax
    L004700c2:
            mov edx, dword ptr [gFrontend_mouse_down]
            mov dword ptr [gPrev_frontend_mouse_down], edx
            call EitherMouseButtonDown
            cmp eax, esi
            mov dword ptr [gFrontend_mouse_down], eax
            je L004700ef
            mov eax, dword ptr [gPrev_frontend_mouse_down]
            mov dword ptr [gFrontend_scroll_time_increment], 0xaf
            cmp eax, esi
            je L004700f9
    L004700ef:
            mov dword ptr [gFrontend_scroll_time_increment], 0x19
    L004700f9:
            mov ecx, dword ptr [gFrontend_load_game_index_top]
            mov eax, 1
            cmp ecx, esi
            jle L00470110
            mov dword ptr [edi + 0x1830], eax
            jmp L00470116
    L00470110:
            mov dword ptr [edi + 0x1830], ebx
    L00470116:
            mov ecx, dword ptr [gFrontend_load_game_index_top]
            mov edx, dword ptr [gFrontend_count_saved_games]
            add ecx, 8
            cmp ecx, edx
            jge L00470131
            mov dword ptr [edi + 0x1988], eax
            jmp L00470137
    L00470131:
            mov dword ptr [edi + 0x1988], ebx
    L00470137:
            mov dword ptr [esp + 0x1c], esi
            mov dword ptr [esp + 0x14], 0x763940
            mov dword ptr [esp + 0x10], 0x7d
    L0047014b:
            mov edx, dword ptr [gFrontend_selected_item_index]
            mov eax, dword ptr [gFrontend_load_game_index_top]
            dec edx
            xor ebx, ebx
            cmp esi, edx
            lea ecx, [esi + eax]
            sete bl
            mov dword ptr [esp + 0x18], ebx
            call GetNthSavedGame
            mov ebp, eax
            test ebp, ebp
            je L00470600
            mov ecx, 0x20
            xor eax, eax
            lea edi, [esp + 0x44]
            lea edx, [esp + 0x34]
            rep stosd
            lea edi, [ebp + 0x32]
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            lea edx, [esp + 0xc4]
            shr ecx, 2
            rep movsd
            mov ecx, eax
            xor eax, eax
            and ecx, 3
            rep movsb
            lea edi, [ebp + 0x52]
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
            xor eax, eax
            and ecx, 3
            rep movsb
            lea edi, [esp + 0x34]
            or ecx, 0xffffffff
            repne scasb
            not ecx
            dec ecx
            xor esi, esi
            test ecx, ecx
            mov dword ptr [esp + 0x30], ecx
            jle L00470236
            lea edi, [esp + 0x44]
    L004701e6:
            mov eax, dword ptr [gQuickTime_call_component_with_storage+0x5a0]
            mov bl, byte ptr [esp + esi + 0x34]
            cmp eax, 1
            jle L0047020b
            movsx ecx, bl
            push 0x107
            push ecx
            call _isctype
            mov ecx, dword ptr [esp + 0x38]
            add esp, 8
            jmp L0047021c
    L0047020b:
            mov eax, dword ptr [gQuickTime_call_component_with_storage+0x394]
            movsx edx, bl
            mov ax, word ptr [eax + edx*2]
            and eax, 0x107
    L0047021c:
            test eax, eax
            je L00470224
            mov byte ptr [edi], bl
            jmp L0047022c
    L00470224:
            cmp bl, 0x2f
            jne L0047022d
            mov byte ptr [edi], 0x2d
    L0047022c:
            inc edi
    L0047022d:
            inc esi
            cmp esi, ecx
            jl L004701e6
            mov ebx, dword ptr [esp + 0x18]
    L00470236:
            mov esi, dword ptr [esp + 0x10]
            push 1
            push 0
            push ebx
            push esi
            mov edx, 0x2a
            lea ecx, [esp + 0x54]
            call SolidPolyFontText
            mov ecx, 0x20
            xor eax, eax
            lea edi, [esp + 0x44]
            push 5
            rep stosd
            lea ecx, [esp + 0xc8]
            lea edx, [esp + 0x48]
            push ecx
            push edx
            call strncpy
            add esp, 0xc
            mov edx, 0x78
            lea ecx, [esp + 0x44]
            push 1
            push 0
            push ebx
            push esi
            call SolidPolyFontText
            mov ecx, 0x20
            xor eax, eax
            lea edi, [esp + 0x44]
            lea edx, [esp + 0x44]
            rep stosd
            lea edi, [ebp + 4]
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            lea edx, [esp + 0x44]
            shr ecx, 2
            rep movsd
            mov ecx, eax
            and ecx, 3
            rep movsb
            mov ecx, ebx
            call PolyFontTextWidth
            cmp eax, 0x7f
            jle L004702e7
    L004702c5:
            lea edi, [esp + 0x44]
            or ecx, 0xffffffff
            xor eax, eax
            lea edx, [esp + 0x44]
            repne scasb
            not ecx
            dec ecx
            mov byte ptr [esp + ecx + 0x43], al
            mov ecx, ebx
            call PolyFontTextWidth
            cmp eax, 0x7f
            jg L004702c5
    L004702e7:
            mov ecx, dword ptr [esp + 0x10]
            push 1
            push 0
            push ebx
            push ecx
            mov edx, 0xbe
            lea ecx, [esp + 0x54]
            call SolidPolyFontText
            mov eax, dword ptr [ebp + 0x20c]
            test eax, eax
            je L00470340
            mov eax, dword ptr [ebp + 0x308]
            lea edx, [eax + eax*8]
            shl edx, 4
            add edx, eax
            mov eax, 0x2aaaaaab
            mov ecx, dword ptr [edx*4 + gRace_list+0x240]
            mov edx, dword ptr [gRace_groups]
            sub ecx, edx
            imul ecx
            sar edx, 1
            mov eax, edx
            shr eax, 0x1f
            lea ecx, [edx + eax + 1]
            push ecx
            push offset gFrontend_QUIT+0xbcbc
            jmp L004703bc
    L00470340:
            mov eax, dword ptr [ebp + 0x30c]
            test eax, eax
            mov eax, dword ptr [ebp + 0x308]
            lea ecx, [eax + eax*8]
            je L00470390
            mov edi, dword ptr [gRace_groups]
            shl ecx, 4
            add ecx, eax
            mov eax, 0x2aaaaaab
            mov ecx, dword ptr [ecx*4 + gRace_list+0x240]
            sub ecx, edi
            imul ecx
            sar edx, 1
            mov eax, edx
            shr eax, 0x1f
            lea ecx, [edx + eax + 1]
            lea edx, [esp + 0x44]
            push ecx
            push 0x1f
            push offset gFrontend_QUIT+0xbcb4
            push edx
            call sprintf
            add esp, 0x10
            jmp L004703c9
    L00470390:
            mov edx, dword ptr [gRace_groups]
            shl ecx, 4
            add ecx, eax
            mov eax, 0x2aaaaaab
            mov ecx, dword ptr [ecx*4 + gRace_list+0x240]
            sub ecx, edx
            imul ecx
            sar edx, 1
            mov eax, edx
            shr eax, 0x1f
            lea ecx, [edx + eax + 1]
            push ecx
            push offset gFrontend_QUIT+0xbcb0
    L004703bc:
            lea edx, [esp + 0x4c]
            push edx
            call sprintf
            add esp, 0xc
    L004703c9:
            mov esi, dword ptr [esp + 0x10]
            push 1
            push 1
            push ebx
            push esi
            mov edx, 0x145
            lea ecx, [esp + 0x54]
            call SolidPolyFontText
            mov ecx, dword ptr [ebp + 0x204]
            push 1
            push 0
            push ebx
            push esi
            call MungeCommas
            mov ecx, eax
            mov edx, 0x16e
            call SolidPolyFontText
            mov eax, dword ptr [ebp + 0x310]
            mov edi, dword ptr [ebp + 0x31c]
            mov ecx, dword ptr [esp + 0x14]
            mov dword ptr [esp + 0x18], eax
            push 0
            mov edx, dword ptr [ecx]
            mov eax, dword ptr [edx + 0x1c]
            mov ebx, dword ptr [eax + 0x40]
            push ebx
            call BrPixelmapFill
            add esp, 8
            xor esi, esi
    L00470427:
            mov eax, esi
            mov ecx, 0xa
            cdq
            idiv ecx
            mov eax, 0x66666667
            mov ecx, edx
            imul esi
            sar edx, 2
            mov eax, edx
            shr eax, 0x1f
            add edx, eax
            shl ecx, 2
            shl edx, 2
            cmp esi, edi
            jl L00470455
            mov eax, dword ptr [gFrontend_APO_Colour_2]
            jmp L00470467
    L00470455:
            mov eax, dword ptr [esp + 0x18]
            cmp esi, eax
            mov eax, dword ptr [gFrontend_APO_Colour_3]
            jge L00470467
            mov eax, dword ptr [gFrontend_APO_Colour_1]
    L00470467:
            push eax
            push 3
            push 3
            push edx
            push ecx
            push ebx
            call BrPixelmapRectangleFill
            add esp, 0x18
            inc esi
            cmp esi, 0x1e
            jl L00470427
            mov edx, dword ptr [esp + 0x14]
            mov ecx, dword ptr [ebp + 0x314]
            mov edi, dword ptr [ebp + 0x320]
            mov dword ptr [esp + 0x18], ecx
            mov eax, dword ptr [edx]
            xor esi, esi
            mov ecx, dword ptr [eax + 0x1c]
            mov ebx, dword ptr [ecx + 0x40]
    L0047049b:
            mov eax, esi
            mov ecx, 0xa
            cdq
            idiv ecx
            mov eax, 0x66666667
            mov ecx, edx
            imul esi
            sar edx, 2
            mov eax, edx
            shr eax, 0x1f
            add edx, eax
            shl ecx, 2
            shl edx, 2
            cmp esi, edi
            jl L004704c9
            mov eax, dword ptr [gFrontend_APO_Colour_2]
            jmp L004704db
    L004704c9:
            mov eax, dword ptr [esp + 0x18]
            cmp esi, eax
            mov eax, dword ptr [gFrontend_APO_Colour_3]
            jge L004704db
            mov eax, dword ptr [gFrontend_APO_Colour_1]
    L004704db:
            push eax
            push 3
            add edx, 0xc
            push 3
            push edx
            push ecx
            push ebx
            call BrPixelmapRectangleFill
            add esp, 0x18
            inc esi
            cmp esi, 0x1e
            jl L0047049b
            mov ecx, dword ptr [esp + 0x14]
            mov edi, dword ptr [ebp + 0x324]
            mov ebp, dword ptr [ebp + 0x318]
            xor esi, esi
            mov edx, dword ptr [ecx]
            mov eax, dword ptr [edx + 0x1c]
            mov ebx, dword ptr [eax + 0x40]
    L0047050e:
            mov eax, esi
            mov ecx, 0xa
            cdq
            idiv ecx
            mov eax, 0x66666667
            mov ecx, edx
            imul esi
            sar edx, 2
            mov eax, edx
            shr eax, 0x1f
            add edx, eax
            shl ecx, 2
            shl edx, 2
            cmp esi, edi
            jl L0047053c
            mov eax, dword ptr [gFrontend_APO_Colour_2]
            jmp L0047054a
    L0047053c:
            mov eax, dword ptr [gFrontend_APO_Colour_3]
            cmp esi, ebp
            jge L0047054a
            mov eax, dword ptr [gFrontend_APO_Colour_1]
    L0047054a:
            push eax
            push 3
            add edx, 0x18
            push 3
            push edx
            push ecx
            push ebx
            call BrPixelmapRectangleFill
            add esp, 0x18
            inc esi
            cmp esi, 0x1e
            jl L0047050e
            mov ecx, dword ptr [esp + 0x14]
            push 0xfff
            mov eax, dword ptr [ecx]
            mov esi, dword ptr [eax + 0x1c]
            mov ebx, dword ptr [eax + 0x18]
            mov edx, dword ptr [esi + 0x40]
            push edx
            call BrMapUpdate
            add esp, 8
            push 0x7fff
            push esi
            call BrMaterialUpdate
            add esp, 8
            push 0x40
            push ebx
            call BrModelUpdate
            mov eax, dword ptr [esp + 0x18]
            mov edi, dword ptr [esp + 0x24]
            add esp, 8
            mov ecx, 0x1c2
            lea esi, [eax + 3]
            push 0
            push edi
            mov edx, esi
            call PrintAPO
            mov edx, esi
            mov ecx, 0x1f4
            push 1
            push edi
            call PrintAPO
            mov edx, esi
            mov ecx, 0x226
            push 2
            push edi
            call PrintAPO
            mov eax, dword ptr [gFrontend_load_game_index_top]
            mov ecx, dword ptr [gFrontend_count_saved_games]
            push ecx
            lea edx, [eax + 8]
            inc eax
            push edx
            push eax
            mov eax, dword ptr [esp + 0x2c]
            push offset gFrontend_QUIT+0xbca4
            add eax, 0x15b1
            push eax
            call sprintf
            mov esi, edi
            mov edi, dword ptr [esp + 0x34]
            add esp, 0x14
    L00470600:
            mov eax, dword ptr [esp + 0x14]
            mov edx, dword ptr [esp + 0x10]
            add eax, 4
            inc esi
            add edx, 0x1b
            cmp eax, offset gNet_settings
            mov dword ptr [esp + 0x1c], esi
            mov dword ptr [esp + 0x14], eax
            mov dword ptr [esp + 0x10], edx
            jl L0047014b
            mov ecx, 0x33
            call PDKeyDown
            test eax, eax
            jne L0047066c
            mov ecx, 0x34
            call PDKeyDown
            test eax, eax
            jne L0047066c
            mov eax, dword ptr [gFrontend_mouse_down]
            test eax, eax
            je L00470654
            cmp dword ptr [gNet_join_host_result], -1
            jne L0047066c
    L00470654:
            mov ecx, dword ptr [gFrontend_leave_current_menu]
            xor eax, eax
            test ecx, ecx
            setne al
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0xc4
            ret
    L0047066c:
            mov ecx, edi
            call ToggleSelection
            mov ecx, dword ptr [gFrontend_selected_item_index]
            mov eax, ecx
            shl eax, 3
            sub eax, ecx
            lea edx, [eax + eax*2]
            lea eax, [ecx + edx*2]
            lea edx, [edi + eax*8]
            mov eax, dword ptr [edi + eax*8 + 0x13c]
            cmp eax, 2
            jne L004706a0
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0xc4
            ret
    L004706a0:
            cmp eax, 1
            jne L004706aa
            mov dword ptr [gFrontend_leave_current_menu], eax
    L004706aa:
            mov edx, dword ptr [edx + 0x134]
            test edx, edx
            je L004706c2
            mov ecx, edi
            call edx
            mov ecx, dword ptr [gFrontend_selected_item_index]
            mov ebp, eax
            jmp L004706da
    L004706c2:
            mov edx, dword ptr [esp + 0x28]
            mov eax, edx
            shl eax, 3
            sub eax, edx
            lea eax, [eax + eax*2]
            lea edx, [edx + eax*2]
            mov ebp, dword ptr [edi + edx*8 + 0x13c]
    L004706da:
            mov eax, ecx
            shl eax, 3
            sub eax, ecx
            lea eax, [eax + eax*2]
            lea edx, [ecx + eax*2]
            mov eax, dword ptr [edi + edx*8 + 0x138]
            cmp eax, 1
            lea edx, [edi + edx*8]
            jne L00470700
            mov ebx, dword ptr [edi + 0x114]
            mov esi, eax
            jmp L00470704
    L00470700:
            mov ebx, eax
            xor esi, esi
    L00470704:
            mov edx, dword ptr [edx + 0x134]
            test edx, edx
            je L00470746
            test ebx, ebx
            jne L00470730
            cmp edx, offset temp
            je L00470730
            mov ecx, dword ptr [gEffects_outlet]
            mov edx, 0xbbc
            call DRS3StartSound
            mov ecx, dword ptr [gFrontend_selected_item_index]
    L00470730:
            mov eax, ecx
            shl eax, 3
            sub eax, ecx
            lea eax, [eax + eax*2]
            lea edx, [ecx + eax*2]
            mov ecx, edi
            call dword ptr [edi + edx*8 + 0x134]
    L00470746:
            test ebx, ebx
            je L00470849
            test esi, esi
            mov edx, 0xbbe
            je L0047075c
            mov edx, 0xbbf
    L0047075c:
            mov ecx, dword ptr [gEffects_outlet]
            call DRS3StartSound
            mov eax, dword ptr [gCurrent_frontend_spec]
            mov ecx, dword ptr [gFrontend_selected_item_index]
            xor edx, edx
            mov dword ptr [eax + 0x118], ecx
            mov ecx, dword ptr [gCurrent_frontend_spec]
            mov eax, dword ptr [ecx + 0x8790]
            test eax, eax
            jle L004707ac
            xor eax, eax
    L0047078c:
            mov esi, dword ptr [ecx + eax + 0x87a0]
            inc edx
            mov dword ptr [ecx + eax + 0x87c0], esi
            mov ecx, dword ptr [gCurrent_frontend_spec]
            add eax, 0x34
            cmp edx, dword ptr [ecx + 0x8790]
            jl L0047078c
    L004707ac:
            mov ecx, edi
            call FRONTEND_DestroyMenu
            mov ecx, ebx
            mov dword ptr [gCurrent_frontend_spec], ebx
            call FRONTEND_CreateMenu
            mov edx, dword ptr [gCurrent_frontend_spec]
            mov eax, dword ptr [edi + 0x114]
            cmp edx, eax
            je L004707dc
            mov dword ptr [edx + 0x114], edi
            mov edx, dword ptr [gCurrent_frontend_spec]
    L004707dc:
            mov ecx, edi
            call Morph_Initialise
            mov ecx, dword ptr [gCurrent_frontend_spec]
            cmp ecx, offset gFrontend_QUIT
            jne L004707fd
            mov dword ptr [gFrontend_selected_item_index], 0
            jmp L00470809
    L004707fd:
            mov edx, dword ptr [ecx + 0x118]
            mov dword ptr [gFrontend_selected_item_index], edx
    L00470809:
            mov eax, dword ptr [ecx + 0x8790]
            xor edx, edx
            test eax, eax
            jle L00470837
            xor eax, eax
    L00470817:
            mov esi, dword ptr [ecx + eax + 0x87c0]
            inc edx
            mov dword ptr [ecx + eax + 0x87a0], esi
            mov ecx, dword ptr [gCurrent_frontend_spec]
            add eax, 0x34
            cmp edx, dword ptr [ecx + 0x8790]
            jl L00470817
    L00470837:
            call UpdateScrollPositions
            mov eax, ebp
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0xc4
            ret
    L00470849:
            test esi, esi
            mov eax, 1
            jne L00470854
            mov eax, ebp
    L00470854:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0xc4
            ret
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00470860
void C2_HOOK_FASTCALL PrepareAPO(int pActor_index) {
    br_material* material;
    br_model* model;

    material = gFrontend_billboard_actors[pActor_index]->material;
    model = gFrontend_billboard_actors[pActor_index]->model;
    BrMapUpdate(material->colour_map, BR_MAPU_ALL);
    BrMaterialUpdate(material, BR_MATU_ALL);
    BrModelUpdate(model, BR_MODU_VERTICES);
}

// FUNCTION: CARMA2_HW 0x004708a0
void C2_HOOK_FASTCALL PrintAPO(int pX, int pY, int pIndex, int pTex_index) {
    br_model *model;
    float map_left;
    float map_right;

    model = gFrontend_billboard_actors[pIndex]->model;
#if defined(_MSC_VER) && _MSC_VER >= 1400
#pragma warning(push)
#pragma warning(disable : 4244) // '=' : conversion from 'double ' to 'float ', possible loss of data
#endif
    map_left = (float)(pTex_index + 0) * 3.0 / 16.0;
    map_right = (float)(pTex_index + 1) * 3.0 / 16.0;
#if defined(_MSC_VER) && _MSC_VER >= 1400
#pragma warning(pop)
#endif
    BrVector2Set(&model->vertices[0].map, 0.0f,   map_left);
    BrVector2Set(&model->vertices[1].map, 10.0f / 16.0f, map_left);
    BrVector2Set(&model->vertices[2].map, 0.0f, map_right);
    BrVector2Set(&model->vertices[3].map, 10.0f / 16.0f, map_right);
    BrModelUpdate(model, BR_MODU_VERTEX_MAPPING);
    BrVector3Set(&gFrontend_billboard_actors[pIndex]->t.t.translate.t, (float)pX, (float)-pY, 0.0f);
    BrActorAdd(gFrontend_menu_camera, gFrontend_billboard_actors[pIndex]);
    BrZbSceneRender(gFrontend_menu_camera, gFrontend_menu_camera, gBack_screen, gDepth_buffer);
    BrActorRemove(gFrontend_billboard_actors[pIndex]);
}

// FUNCTION: CARMA2_HW 0x004709b0
void C2_HOOK_FASTCALL BuildAPO(int pCurrent, int pPotential, int pActor_index, int pAPO) {
    br_pixelmap* map;
    int x;
    int y;
    br_colour c;
    int i;

    map = gFrontend_billboard_actors[pActor_index]->material->colour_map;

    if (pAPO == 0) {
        BrPixelmapFill(map, 0);
    }

    for (i = 0; i < 3 * 10; i++) {

        x = 4 * (i % 10);
        y = 0 + 4 * (i / 10);

        if (i >= pPotential) {
            c = gFrontend_APO_Colour_2;
        } else if (i >= pCurrent) {
            c = gFrontend_APO_Colour_3;
        } else {
            c = gFrontend_APO_Colour_1;
        }

        BrPixelmapRectangleFill(map, x, y + 3 * 4 * pAPO, 3, 3, c);
    }
}

// FUNCTION: CARMA2_HW 0x00470a50
void C2_HOOK_FASTCALL PolyClipName(char *pText, int pFont, int pWidth) {
    while (PolyFontTextWidth(pFont, pText) > pWidth) {
        pText[strlen(pText) + -1] = '\0';
    }
}

// Ians_GetItemAtMousePos

// TranslateSliderItem

// GetActiveSlider

// GetAnyActiveSlider

// PrepareSliders

// FUNCTION: CARMA2_HW 0x00470a90
void C2_HOOK_FASTCALL Morph_Initialise(tFrontend_spec* pCurrent, tFrontend_spec* pNext) {
    gFrontend_A_model_from = gFrontend_A_models[pCurrent->model_A_index].model;
    gFrontend_A_model_to = gFrontend_A_models[pNext->model_A_index].model;
    if (pCurrent->model_A_index == 0) {
        gFrontend_backdrop0_opacity_mode = pNext->model_A_index != 0 ? 1 : -2;
    } else {
        gFrontend_backdrop0_opacity_mode = pNext->model_A_index != 0 ? 0 : -1;
    }

    gFrontend_B_model_from = gFrontend_B_models[pCurrent->model_B_index].model;
    gFrontend_B_model_to = gFrontend_B_models[pNext->model_B_index].model;
    if (pCurrent->model_B_index == 0) {
        gFrontend_backdrop1_opacity_mode = pNext->model_B_index != 0 ? 1 : -2;
    } else {
        gFrontend_backdrop1_opacity_mode = pNext->model_B_index != 0 ? 0 : -1;
    }

    gFrontend_C_model_from = gFrontend_C_models[pCurrent->model_C_index].model;
    gFrontend_C_model_to = gFrontend_C_models[pNext->model_C_index].model;
    if (pCurrent->model_C_index == 0) {
        gFrontend_backdrop2_opacity_mode = pNext->model_C_index != 0 ? 1 : -2;
    } else {
        gFrontend_backdrop2_opacity_mode = pNext->model_C_index != 0 ? 0 : -1;
    }

    gFrontend_interpolate_steps_left = 16;
}

// FUNCTION: CARMA2_HW 0x00470bb0
int C2_HOOK_FASTCALL Generic_Infunc(tFrontend_spec* pFrontend) {
    DefaultInfunc(pFrontend);
    gCurrent_frontend_scrollbars = NULL;
    gConnected_items = NULL;
    gPTR_00686508 = NULL;
    return 0;
}

// FUNCTION: CARMA2_HW 0x00470c10
int C2_HOOK_FASTCALL Generic_Outfunc(tFrontend_spec* pFrontend) {
    return 0;
}

// Generic_EventEffect

void C2_HOOK_FASTCALL Generic_LinkInEffect(void) {

    DRS3StartSound(gEffects_outlet, eSoundId_Swingin);
}

void C2_HOOK_FASTCALL Generic_LinkOutEffect(void) {

    DRS3StartSound(gEffects_outlet, eSoundId_Swingout);
}

// GetScrollSet

// ScrollSet_DisplayEntry

// ScrollSet_GetItem

// GetUpdown

// ScrollSet_TranslateItemToIndex

// Generic_FindNextActiveItem

// Generic_FindPrevActiveItem

// Generic_MungeActiveItems

// Generic_UnMungeActiveItems

// Generic_MenuHandler

// StripControls

// BackupKeyMappings

// LoadKeyNames

// DisposeKeyNames

// RefreshScrollSet

// DetermineKeyArrayIndex

// Joystick_BackupSettings

// Controls_JoystickToggle

// Controls_JoystickDpadToggle

// Slider_XProc

// Slider_YProc

// Slider_ForceProc

// DisplayJoystickSettings

// SaveAllJoystickData

// Controls_SwitchKeymapSet

// Controls_KeyUp

// Controls_KeyDown

// Controls_Infunc

// Controls_Outfunc

// Controls_SlotActivated

// Controls_Ok

// DisplayNetworkOptions

// BackupNetworkOptions

// RestoreNetworkOptions

// NetOptions_Infunc

// NetOptions_Outfunc

// NetOptions_CreditsRoller

// NetOptions_TargetRoller

// NetOptions_Ok

// NetOptions_Cancel

// CheckPlayersAreResponding

// MungePlayers

// NetSynchRaceStart2

// NetSync_Draw

// NetSync_Start

// NetSync_Abort

// NetSync_Infunc

// NetSync_MenuHandler

// NetSummary_Draw

// SortScores

// SortGameScores

// NetworkSummarySetup

// NetSummary_Infunc

// NetSummary_MenuHandler

// DisplayVolumeSettings

// Slider_EffectsProc

// Options_Infunc

// Options_Outfunc

// Options_MusicOn

// Options_MusicOff

// Options_Ok

// Options_AbortRace

// Options_AbortGame

// Options_Quit

// Graphics_Infunc

// Graphics_Outfunc

// FUNCTION: CARMA2_HW 0x00474860
void C2_HOOK_FASTCALL FrontEndShowMouse(void) {
    gFrontend_suppress_mouse = 0;
}

// FUNCTION: CARMA2_HW 0x00474870
void C2_HOOK_FASTCALL FrontEndHideMouse(void) {
    gFrontend_suppress_mouse = 1;
}

// FUNCTION: CARMA2_HW 0x004721e0
int C2_HOOK_FASTCALL Controls_JoystickToggle(tFrontend_spec* pFrontend) {
    if (gFrontend_selected_item_index == 10) {
        EnableJoysticks();
    } else {
        DisableJoysticks();
    }
    DisplayJoystickSettings(pFrontend);
    return 0;
}


// FUNCTION: CARMA2_HW 0x00472210
void C2_NAKED C2_HOOK_FASTCALL DisplayJoystickSettings(tFrontend_spec *pFrontend) {
#ifndef CARPOCALYPSE2_MATCHING

    C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(gFrontend_controls_indices) != 15);

    if (gJoystick_index != -1) {
        int i;

        pFrontend->items[10].unlitFont = 1;
        pFrontend->items[10].highFont = 1;
        pFrontend->items[11].unlitFont = 0;
        pFrontend->items[11].highFont = 0;
        gControls_slider_1.flags = 0;
        gControls_slider_2.flags = 0;
        gControls_slider_3.flags = 0;

        for (i = 0; i < 3; i++) {
            pFrontend->items[ 1 + i].enabled = kFrontendItemEnabled_enabled;
            pFrontend->items[ 4 + i].enabled = kFrontendItemEnabled_enabled;
            pFrontend->items[ 7 + i].enabled = kFrontendItemEnabled_enabled;
            pFrontend->items[14 + i].enabled = kFrontendItemEnabled_enabled;
            pFrontend->items[19 + i].enabled = kFrontendItemEnabled_enabled;
            pFrontend->items[23 + i].enabled = kFrontendItemEnabled_enabled;
        }
        for (i = 0; i < CARPOCALYPSE2_ASIZE(gFrontend_controls_indices); i++) {

            pFrontend->items[gFrontend_controls_indices[i]].enabled = kFrontendItemEnabled_enabled;
        }
        pFrontend->items[36].enabled = kFrontendItemEnabled_enabled;
        pFrontend->items[37].enabled = kFrontendItemEnabled_enabled;
    } else {
        int i;

        pFrontend->items[10].unlitFont = 0;
        pFrontend->items[10].highFont = 0;
        pFrontend->items[11].unlitFont = 1;
        pFrontend->items[11].highFont = 1;
        gControls_slider_1.flags = 1;
        gControls_slider_2.flags = 1;
        gControls_slider_3.flags = 1;

        for (i = 0; i < 3; i++) {
            pFrontend->items[ 1 + i].enabled = kFrontendItemEnabled_disabled;
            pFrontend->items[ 4 + i].enabled = kFrontendItemEnabled_disabled;
            pFrontend->items[ 7 + i].enabled = kFrontendItemEnabled_disabled;
            pFrontend->items[14 + i].enabled = kFrontendItemEnabled_disabled;
            pFrontend->items[19 + i].enabled = kFrontendItemEnabled_disabled;
            pFrontend->items[23 + i].enabled = kFrontendItemEnabled_disabled;
        }
        for (i = 0; i < CARPOCALYPSE2_ASIZE(gFrontend_controls_indices); i++) {

            pFrontend->items[gFrontend_controls_indices[i]].enabled = kFrontendItemEnabled_disabled;
        }
        pFrontend->items[36].enabled = kFrontendItemEnabled_disabled;
        pFrontend->items[37].enabled = kFrontendItemEnabled_disabled;
    }

    gControls_slider_1.value = gControls_slider_1.field_0x14 + GetJoystickX() / (gControls_slider_1.field_0x18 - gControls_slider_1.field_0x14);
    gControls_slider_2.value = gControls_slider_2.field_0x14 + GetJoystickY() / (gControls_slider_2.field_0x18 - gControls_slider_2.field_0x14);
    gControls_slider_3.value = 0.01f * (float)GetJoystickFBBGain();
    if (PDIsJoystickDPadEnabled()) {
        pFrontend->items[36].unlitFont = 1;
        pFrontend->items[36].highFont = 1;
        pFrontend->items[37].unlitFont = 0;
        pFrontend->items[37].highFont = 0;
    } else {
        pFrontend->items[36].unlitFont = 0;
        pFrontend->items[36].highFont = 0;
        pFrontend->items[37].unlitFont = 1;
        pFrontend->items[37].highFont = 1;
    }
#else
    __asm {
            push ecx
            mov eax, dword ptr [gJoystick_index]
            push ebx
            push ebp
            push esi
            push edi
            or edi, 0xffffffff
            cmp eax, edi
            mov esi, ecx
            jne L004722bf
            xor ebp, ebp
            mov ebx, 1
            mov dword ptr [esi + 0xeb0], ebp
            mov dword ptr [esi + 0xeb4], ebp
            mov dword ptr [esi + 0x1008], ebx
            mov dword ptr [esi + 0x100c], ebx
            mov byte ptr [gControls_slider_1], bl
            mov byte ptr [gControls_slider_2], bl
            mov byte ptr [gControls_slider_3], bl
            lea eax, [esi + 0x6b8]
            mov ecx, 3
    L00472263:
            mov dword ptr [eax - 0x408], edi
            mov dword ptr [eax], edi
            mov dword ptr [eax + 0x408], edi
            mov dword ptr [eax + 0xd70], edi
            mov dword ptr [eax + 0x1428], edi
            mov dword ptr [eax + 0x1988], edi
            add eax, 0x158
            dec ecx
            jne L00472263
            mov edx, offset gFrontend_controls_indices
    L00472290:
            mov eax, dword ptr [edx]
            add edx, 4
            inc eax
            mov ecx, eax
            shl ecx, 3
            sub ecx, eax
            cmp edx, offset gFrontend_controls_indices+0x3c
            lea ecx, [ecx + ecx*2]
            lea eax, [eax + ecx*2]
            mov dword ptr [esi + eax*8], edi
            jl L00472290
            mov dword ptr [esi + 0x31b8], edi
            mov dword ptr [esi + 0x3310], edi
            jmp L00472351
    L004722bf:
            mov ebx, 1
            xor eax, eax
            mov dword ptr [esi + 0xeb0], ebx
            mov dword ptr [esi + 0xeb4], ebx
            mov dword ptr [esi + 0x1008], eax
            mov dword ptr [esi + 0x100c], eax
            mov byte ptr [gControls_slider_1], al
            mov byte ptr [gControls_slider_2], al
            mov byte ptr [gControls_slider_3], al
            lea eax, [esi + 0x6b8]
            mov ecx, 3
    L004722f8:
            mov dword ptr [eax - 0x408], ebx
            mov dword ptr [eax], ebx
            mov dword ptr [eax + 0x408], ebx
            mov dword ptr [eax + 0xd70], ebx
            mov dword ptr [eax + 0x1428], ebx
            mov dword ptr [eax + 0x1988], ebx
            add eax, 0x158
            dec ecx
            jne L004722f8
            mov edx, offset gFrontend_controls_indices
    L00472325:
            mov eax, dword ptr [edx]
            add edx, 4
            inc eax
            mov ecx, eax
            shl ecx, 3
            sub ecx, eax
            cmp edx, offset gFrontend_controls_indices+0x3c
            lea ecx, [ecx + ecx*2]
            lea eax, [eax + ecx*2]
            mov dword ptr [esi + eax*8], ebx
            jl L00472325
            mov dword ptr [esi + 0x31b8], ebx
            mov dword ptr [esi + 0x3310], ebx
            xor ebp, ebp
    L00472351:
            call GetJoystickX
            fld dword ptr [gControls_slider_1+0x18]
            fsub dword ptr [gControls_slider_1+0x14]
            fxch st(1)
            fstp dword ptr [esp + 0x10]
            fdivr dword ptr [esp + 0x10]
            fadd dword ptr [gControls_slider_1+0x14]
            fstp dword ptr [gControls_slider_1+0x10]
            call GetJoystickY
            fld dword ptr [gControls_slider_2+0x18]
            fsub dword ptr [gControls_slider_2+0x14]
            fxch st(1)
            fstp dword ptr [esp + 0x10]
            fdivr dword ptr [esp + 0x10]
            fadd dword ptr [gControls_slider_2+0x14]
            fstp dword ptr [gControls_slider_2+0x10]
            call GetJoystickFBBGain
            mov dword ptr [esp + 0x10], eax
            fild dword ptr [esp + 0x10]
            fmul qword ptr [gDouble_0058a068+0x80]
            fstp dword ptr [gControls_slider_3+0x10]
            call PDIsJoystickDPadEnabled
            test eax, eax
            je L004723df
            mov dword ptr [esi + 0x31a0], ebx
            mov dword ptr [esi + 0x31a4], ebx
            mov dword ptr [esi + 0x32f8], ebp
            mov dword ptr [esi + 0x32fc], ebp
            pop edi
            pop esi
            pop ebp
            pop ebx
            pop ecx
            ret
    L004723df:
            mov dword ptr [esi + 0x31a0], ebp
            mov dword ptr [esi + 0x31a4], ebp
            mov dword ptr [esi + 0x32f8], ebx
            mov dword ptr [esi + 0x32fc], ebx
            pop edi
            pop esi
            pop ebp
            pop ebx
            pop ecx
            ret
    }
#endif
}

/* ==== REAL frontend flow ported from common/ ==== */

void C2_HOOK_FASTCALL LoadMenuImages(void) {
    int i;
    char s[256];
    char s2[256];
    FILE* f;
    int count;

    /* 1 to 6 are car images, 7 is driver image */
    for (i = 1; i < 8; i++) {
 /* FIXME: magic number */
        gFrontend_images[i] = LoadPixelmap("64by64.tif");
    }
    PathCat(s, gApplication_path, "INTRFACE");
    PathCat(s, s, "MENUIMAGES.TXT");
    /* menuimages.txt -  Menu button images, indexed from 1 */
    f = DRfopen(s, "rt");
    if (f == NULL) {
        FatalError(kFatalError_Mysterious_SS, "", "");
    }
    /* number of images */
    count = GetAnInt(f);
    for (i = 0; i < count; i++) {
        GetAString(f, s2);
        strcpy(s, s2);
        strcat(s, ".TIF");
        gFrontend_images[i + 8] = LoadPixelmap(s); /* FIXME: magic number */
    }

#if defined(CARPOCALYPSE2_FIX_BUGS)
    PFfclose(f);
#endif
}

static void C2_HOOK_FASTCALL LoadMenuModels(void) {
    char s[256];
    char s2[256];
    char s3[256];
    FILE* f;
    int count;
    int i;

    PathCat(s, gApplication_path, "INTRFACE");
    PathCat(s2, s, "MENUMODELS.TXT");
    f = DRfopen(s2, "rt");
    if (f == NULL) {
        FatalError(kFatalError_Mysterious_SS, "", "");
    }

    C2_HOOK_BUG_ON(sizeof(tFrontend_model) != 16);
    PathCat(s, gApplication_path, "INTRFACE");
    /* number of A models */
    count = GetAnInt(f);
    for (i = 0; i < count; i++) {
        GetAString(f, s3);
        PathCat(s2, s, s3);
        strcat(s2, ".DAT");
        gFrontend_A_models[i].model = BrModelLoad(s2);
        gFrontend_A_models[i].model->flags |= BR_MODF_KEEP_ORIGINAL;
        BrModelAdd(gFrontend_A_models[i].model);
        if (i == 0) {
            gFrontend_backdrop_actors[0]->model = BrModelLoad(s2);
            gFrontend_backdrop_actors[0]->model->flags |= BR_MODF_KEEP_ORIGINAL;
            BrModelAdd(gFrontend_backdrop_actors[0]->model);
            BrMatrix34Translate(&gFrontend_backdrop_actors[0]->t.t.mat, 300.f, -250.f, -2.5f);
            BrMatrix34PreScale(&gFrontend_backdrop_actors[0]->t.t.mat, 1.25f, 1.25f, 1.0f);
            gFrontend_backdrop_actors[0]->render_style = BR_RSTYLE_FACES;
        }
    }
    /* number of B models */
    count = GetAnInt(f);
    for (i = 0; i < count; i++) {
        GetAString(f, s3);
        PathCat(s2, s, s3);
        strcat(s2, ".DAT");
        gFrontend_B_models[i].model = BrModelLoad(s2);
        gFrontend_B_models[i].model->flags |= BR_MODF_KEEP_ORIGINAL;
        BrModelAdd(gFrontend_B_models[i].model);
        if (i == 0) {
            gFrontend_backdrop_actors[1]->model = BrModelLoad(s2);
            gFrontend_backdrop_actors[1]->model->flags |= BR_MODF_KEEP_ORIGINAL;
            BrModelAdd(gFrontend_backdrop_actors[1]->model);
            BrMatrix34Translate(&gFrontend_backdrop_actors[1]->t.t.mat, 300.f, -250.f, -2.5f);
            BrMatrix34PreScale(&gFrontend_backdrop_actors[1]->t.t.mat, 1.25f, 1.25f, 1.0f);
            gFrontend_backdrop_actors[1]->render_style = BR_RSTYLE_FACES;
        }
    }
    /* number of C models */
    count = GetAnInt(f);
    for (i = 0; i < count; i++) {
        GetAString(f, s3);
        PathCat(s2, s, s3);
        strcat(s2, ".DAT");
        gFrontend_C_models[i].model = BrModelLoad(s2);
        gFrontend_C_models[i].model->flags |= BR_MODF_KEEP_ORIGINAL;
        BrModelAdd(gFrontend_C_models[i].model);
        if (i == 0) {
            gFrontend_backdrop_actors[2]->model = BrModelLoad(s2);
            gFrontend_backdrop_actors[2]->model->flags |= BR_MODF_KEEP_ORIGINAL;
            BrModelAdd(gFrontend_backdrop_actors[2]->model);
            BrMatrix34Translate(&gFrontend_backdrop_actors[2]->t.t.mat, 300.f, -250.f, -2.5f);
            BrMatrix34PreScale(&gFrontend_backdrop_actors[2]->t.t.mat, 1.25f, 1.25f, 1.0f);
            gFrontend_backdrop_actors[2]->render_style = BR_RSTYLE_FACES;
        }
    }

#if defined(CARPOCALYPSE2_FIX_BUGS)
    PFfclose(f);
#endif
}

// FUNCTION: CARMA2_HW 0x0046d1c0
void C2_HOOK_FASTCALL FRONTEND_Setup(tFrontendMenuType pType) {

    PrintMemoryDump(0, "START OF FRONTEND_Setup");
    LoadInterfaceStuff(gProgram_state.racing);
    if (gFrontend_stuff_not_loaded) {
        IString_Load();
        InitPolyFonts();
        LoadMenuImages();
        PrintMemoryDump(0, "AFTER LoadMenuImages");
        FRONTEND_Setup2D();
        LoadMenuModels();
        PrintMemoryDump(0, "AFTER LoadMenuModels");
        gFrontend_stuff_not_loaded = 0;
    }
    switch (pType) {
    case kFrontend_menu_main:
        FRONTEND_CreateMenu(&gFrontend_MAIN);
        gCurrent_frontend_spec = &gFrontend_MAIN;
        break;
    case kFrontend_menu_options:
        FRONTEND_CreateMenu(&gFrontend_OPTIONS);
        gCurrent_frontend_spec = &gFrontend_OPTIONS;
        break;
    case kFrontend_menu_wrecks:
        FRONTEND_CreateMenu(&gFrontend_WRECKS);
        gCurrent_frontend_spec = &gFrontend_WRECKS;
        break;
    case kFrontend_menu_netsync:
        FRONTEND_CreateMenu(&gFrontend_NETSYNC);
        gCurrent_frontend_spec = &gFrontend_NETSYNC;
        break;
    case kFrontend_menu_networksummary:
        FRONTEND_CreateMenu(&gFrontend_NETWORK_SUMMARY);
        gCurrent_frontend_spec = &gFrontend_NETWORK_SUMMARY;
        break;
    case kFrontend_menu_credits:
        FRONTEND_CreateMenu(&gFrontend_CREDITS);
        gCurrent_frontend_spec = &gFrontend_CREDITS;
        break;
    case kFrontend_menu_newgame:
        FRONTEND_CreateMenu(&gFrontend_NEWGAME);
        gCurrent_frontend_spec = &gFrontend_NEWGAME;
        break;
    }
    PrintMemoryDump(0, "AFTER FRONTEND_CreateMenu");

    gCurrent_frontend_spec->previous = NULL;
    gFrontend_A_model_from = gFrontend_A_models[0].model;
    gFrontend_A_model_to = gFrontend_A_models[gCurrent_frontend_spec->model_A_index].model;
    gFrontend_B_model_from = gFrontend_B_models[0].model;
    gFrontend_B_model_to = gFrontend_B_models[gCurrent_frontend_spec->model_B_index].model;
    gFrontend_C_model_from = gFrontend_C_models[0].model;
    gFrontend_C_model_to = gFrontend_C_models[gCurrent_frontend_spec->model_C_index].model;
    gFrontend_backdrop0_opacity_mode = gCurrent_frontend_spec->model_A_index != 0 ? 1 : -2;
    gFrontend_backdrop1_opacity_mode = gCurrent_frontend_spec->model_B_index != 0 ? 1 : -2;
    gFrontend_backdrop2_opacity_mode = gCurrent_frontend_spec->model_C_index != 0 ? 1 : -1;
    gFrontend_interpolate_steps_left = COUNT_FRONTEND_INTERPOLATE_STEPS;
    PrintMemoryDump(0, "END OF FRONTEND_Setup");
}

void C2_HOOK_FASTCALL FrontendRedrawMenu(void) {
    int i;

    gBack_screen->origin_x = 0;
    gBack_screen->origin_y = 0;
    if (gFrontend_backdrop != NULL) {

        DrPixelmapRectangleCopyPossibleLock(gBack_screen, 0, 0,
            gFrontend_backdrop, 0, 0, gFrontend_backdrop->width, gFrontend_backdrop->height);
    }
    BrPixelmapFill(gDepth_buffer, 0xffffffff);
    for (i = 0; i < gFrontend_count_brender_items; i++) {
        br_actor* actor;

        actor = gFrontend_brender_items[i].actor;
        if (gCurrent_frontend_spec->items[i].visible) {
            actor->render_style = BR_RSTYLE_FACES;
        } else {
            actor->render_style = BR_RSTYLE_NONE;
        }
    }
    BrZbsSceneRender(gFrontend_actor, gFrontend_camera, gBack_screen, gDepth_buffer);
    FRONTEND_DrawMenu(gCurrent_frontend_spec);
    MaybeDoMouseCursor();
    PDScreenBufferSwap(0);
    return 0;
}

// FUNCTION: CARMA2_HW 0x0046d8e0
int C2_HOOK_FASTCALL FrontEndProcessMenus(tFrontendMenuType pFrontendType) {
    // GLOBAL: CARMA2_HW 0x0076370c
    static int back_screen_base_x;
    // GLOBAL: CARMA2_HW 0x00763710
    static int back_screen_base_y;
    // GLOBAL: CARMA2_HW 0x00763704
    static int back_screen_origin_x;
    // GLOBAL: CARMA2_HW 0x00763708
    static int back_screen_origin_y;

    if (gSound_enabled) {
        DRS3StopAllOutletSoundsExceptCDA();
    }
    SwitchToHiresMode();
    WaitForNoKeys();
    gFrontend_remove_current_backdrop = 0;
    TurnTintedPolyOff(gHud_tinted1);
    TurnTintedPolyOff(gHud_tinted2);
    TurnTintedPolyOff(gHud_tinted3);
    FRONTEND_Setup(pFrontendType);
    gFrontend_remove_current_backdrop = 1;
    gFrontend_leave_current_menu = 0;
    ResetInterfaceTimeout();
    Generic_LinkInEffect();
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFrontend_spec, unknownLastInt, 0xb8c4);
    gCurrent_frontend_spec->unknownLastInt = 0;

    for (;;) {
        gBack_screen->origin_x = 0;
        gBack_screen->origin_y = 0;
        if (gFrontend_backdrop != NULL) {
            FRONTEND_Redraw(gBack_screen, 0, 0,
                gFrontend_backdrop, 0, 0, gFrontend_backdrop->width, gFrontend_backdrop->height);
        }
        if (gCurrent_frontend_spec != &gFrontend_LOAD_GAME &&
              gCurrent_frontend_spec != &gFrontend_NETSYNC &&
              gCurrent_frontend_spec != &gFrontend_NETWORK_SUMMARY &&
              gFrontend_interpolate_steps_left == 0) {
            gCurrent_frontend_spec->unknownLastInt = FRONTEND_GenericMenuHandler(gCurrent_frontend_spec);
        }
        BrPixelmapFill(gDepth_buffer, 0xffffffff);
        gBack_screen->origin_x = 0;
        gBack_screen->origin_y = 0;

        if (gFrontend_interpolate_steps_left > 0) {
            int step = 16 - gFrontend_interpolate_steps_left + 1;

            MorphBlob(gFrontend_A_model_from, gFrontend_A_model_to, gFrontend_backdrop_actors[0]->model, step, 16);
            if (gFrontend_backdrop0_opacity_mode == -1) {
                gFrontend_backdrop0_material_prims[1].v.x = BR_FIXED_INT(176 * (16 - step)  / 16);
            } else if (gFrontend_backdrop0_opacity_mode == 1) {
                gFrontend_backdrop0_material_prims[1].v.x = BR_FIXED_INT(176 * step / 16);
            } else if (gFrontend_backdrop0_opacity_mode == -2) {
                gFrontend_backdrop0_material_prims[1].v.x = BR_FIXED_INT(0);
            }
            BrMaterialUpdate(gFrontend_backdrop_materials[0], BR_MATU_ALL);

            MorphBlob(gFrontend_B_model_from, gFrontend_B_model_to, gFrontend_backdrop_actors[1]->model, step, 16);
            if (gFrontend_backdrop1_opacity_mode == -1) {
                gFrontend_backdrop1_material_prims[1].v.x = BR_FIXED_INT(176 * (16 - step)  / 16);
            } else if (gFrontend_backdrop1_opacity_mode == 1) {
                gFrontend_backdrop1_material_prims[1].v.x = BR_FIXED_INT(176 * step / 16);
            } else if (gFrontend_backdrop1_opacity_mode == -2) {
                gFrontend_backdrop1_material_prims[1].v.x = BR_FIXED_INT(0);
            }
            BrMaterialUpdate(gFrontend_backdrop_materials[1], BR_MATU_ALL);

            MorphBlob(gFrontend_C_model_from, gFrontend_C_model_to, gFrontend_backdrop_actors[2]->model, step, 16);
            if (gFrontend_backdrop2_opacity_mode == -1) {
                gFrontend_backdrop2_material_prims[1].v.x = BR_FIXED_INT(176 * (16 - step)  / 16);
            } else if (gFrontend_backdrop2_opacity_mode == 1) {
                gFrontend_backdrop2_material_prims[1].v.x = BR_FIXED_INT(176 * step / 16);
            } else if (gFrontend_backdrop2_opacity_mode == -2) {
                gFrontend_backdrop2_material_prims[1].v.x = BR_FIXED_INT(0);
            }
            BrMaterialUpdate(gFrontend_backdrop_materials[2], BR_MATU_ALL);
        }
        BrZbsSceneRender(gFrontend_actor, gFrontend_camera, gBack_screen, gDepth_buffer);

        if (gCurrent_frontend_spec == &gFrontend_LOAD_GAME ||
                gCurrent_frontend_spec == &gFrontend_NETSYNC ||
                gCurrent_frontend_spec == &gFrontend_NETWORK_SUMMARY) {
            if (gFrontend_interpolate_steps_left == 0) {
                gCurrent_frontend_spec->unknownLastInt = FRONTEND_GenericMenuHandler(gCurrent_frontend_spec);
            }
        }
        if (gFrontend_interpolate_steps_left == 0) {
            if (gCurrent_frontend_spec == &gFrontend_WRECKS && gFrontend_wrecks_actor != NULL) {
                back_screen_base_x = gBack_screen->base_x;
                back_screen_base_y = gBack_screen->base_y;
                back_screen_origin_x = gBack_screen->origin_x;
                back_screen_origin_y = gBack_screen->origin_y;
                gBack_screen->base_x = 0;
                gBack_screen->base_y = 0;
                gBack_screen->origin_x = 320;
                gBack_screen->origin_y = 120;
                BrZbsSceneRender(gFrontend_wrecks_actor, gFrontend_wrecks_camera, gFrontend_wrecks_pixelmap, gDepth_buffer);
                gBack_screen->base_x = back_screen_base_x;
                gBack_screen->base_y = back_screen_base_y;
                gBack_screen->origin_x = back_screen_origin_x;
                gBack_screen->origin_y = back_screen_origin_y;
            }
            if (gCurrent_frontend_spec == &gFrontend_MAIN && gFrontend_menu_camera != NULL) {
                BuildAPO(gCurrent_APO_levels[0], gCurrent_APO_potential_levels[0], 0, 0);
                BuildAPO(gCurrent_APO_levels[1], gCurrent_APO_potential_levels[1], 0, 1);
                BuildAPO(gCurrent_APO_levels[2], gCurrent_APO_potential_levels[2], 0, 2);

                PrepareAPO(0);
                PrintAPO( 95, 348, 0, 0);
                PrintAPO(146, 348, 0, 1);
                PrintAPO(195, 348, 0, 2);
            }
            if (gCurrent_frontend_spec == &gFrontend_CREDITS) {
                ScrollCredits();
            }
            FRONTEND_DrawMenu(gCurrent_frontend_spec);
            if (gCurrent_frontend_spec->unknownLastInt == 0) {
                MaybeDoMouseCursor();
            }
        } else {
            gFrontend_interpolate_steps_left -= 1;
        }
        PDScreenBufferSwap(0);
        if (gCurrent_frontend_spec->unknownLastInt == 1 && gCurrent_frontend_spec != &gFrontend_START_GAME) {
            FRONTEND_DestroyMenu(gCurrent_frontend_spec);
            strcpy(gFrontend_START_GAME.backdrop_name, gCurrent_frontend_spec->backdrop_name);
            Morph_Initialise(gCurrent_frontend_spec, &gFrontend_START_GAME);
            gCurrent_frontend_spec = &gFrontend_START_GAME;
            FRONTEND_CreateMenu(&gFrontend_START_GAME);
        }
        if (gCurrent_frontend_spec->unknownLastInt == 3) {
            FRONTEND_DestroyMenu(gCurrent_frontend_spec);
            FRONTEND_Setup(gFrontend_next_menu);
            gCurrent_frontend_spec->unknownLastInt = 0;
        }
        if (gCurrent_frontend_spec->unknownLastInt != 0) {
            FRONTEND_DestroyMenu(gCurrent_frontend_spec);
            if (gFrontend_backdrop != NULL) {
                BrMapRemove(gFrontend_backdrop);
                BrPixelmapFree(gFrontend_backdrop);
            }
            DisposeInterfaceFonts();
            return gCurrent_frontend_spec->unknownLastInt != 2;
        }
    }
}


int C2_HOOK_FASTCALL FindNextActiveItem(tFrontend_spec* pFrontend, int pStart_index) {
    int i;

    for (i = pStart_index + 1; i < pFrontend->count_items; i++) {
        tFrontend_item_spec *item = &pFrontend->items[i];

        if (item->enabled > 0 && item->visible) {
            return i;
        }
    }
    return 0;
}

int C2_HOOK_FASTCALL Frontend_FindNextVisibleItem(tFrontend_spec* pFrontend, int pIndex) {
    int i;

    if (pIndex < pFrontend->count_items - 1) {
        for (i = pIndex + 1; pIndex < pFrontend->count_items; i++) {
            if (pFrontend->items[i].enabled > 0 && pFrontend->items[i].visible) {
                return i;
            }
        }
    } else {
        for (i = 0; i < pFrontend->count_items; i++) {
            if (pFrontend->items[i].enabled > 0 && pFrontend->items[i].visible) {
                return i;
            }
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0046c0d0

int C2_NAKED C2_HOOK_FASTCALL FRONTEND_GenericMenuHandler(tFrontend_spec* pFrontend) {
#ifndef CARPOCALYPSE2_MATCHING
    int original_selected_index;

    original_selected_index = gFrontend_selected_item_index;
    gTyping_slot = -1;
    EdgeTriggerModeOn();
    KillSplashScreen();
    if (pFrontend->tick != NULL) {
        int ret = pFrontend->tick(pFrontend);
        if (ret != 0) {
            return ret;
        }
    } else {
        int the_key;
        int item_under_mouse;
        int mouse_button;

        PollKeys();
        EdgeTriggerModeOff();
        the_key = PDAnyKeyDown();
        if (the_key != -1 && the_key != 4) {
            gMouse_in_use = 0;
            ResetInterfaceTimeout();
        }
        EdgeTriggerModeOn();
        mouse_button = 0;
        item_under_mouse = -1;
        if (gMouse_in_use) {
            int x;
            int y;

            ResetInterfaceTimeout();
            GetMousePosition(&x, &y);
            mouse_button = EitherMouseButtonDown();
            item_under_mouse = GetItemAtMousePos(gCurrent_frontend_spec, x, y);
            if (item_under_mouse != -1) {
                gFrontend_selected_item_index = item_under_mouse;
            } else {
                gFrontend_selected_item_index = 99;
            }
        }

        if (PDKeyDown(72) || PDKeyDown(89)) {
            gFrontend_selected_item_index = FindPrevActiveItem(pFrontend, gFrontend_selected_item_index);
            if (!gMouse_in_use
                    && gFrontend_selected_item_index >= gCurrent_frontend_spec->scrollers[0].indexFirstScrollableItem
                    && gFrontend_selected_item_index <= gCurrent_frontend_spec->scrollers[0].indexLastScrollableItem) {
                ToggleSelection(pFrontend);
                if (pFrontend->items[gFrontend_selected_item_index].action != NULL) {
                    pFrontend->items[gFrontend_selected_item_index].action(pFrontend);
                }
            }
        }

        if (PDKeyDown(73) || PDKeyDown(83)) {
            gFrontend_selected_item_index = Frontend_FindNextVisibleItem(pFrontend, gFrontend_selected_item_index);
            if (!gMouse_in_use
                && gFrontend_selected_item_index >= gCurrent_frontend_spec->scrollers[0].indexFirstScrollableItem
                && gFrontend_selected_item_index <= gCurrent_frontend_spec->scrollers[0].indexLastScrollableItem) {
                ToggleSelection(pFrontend);
                if (pFrontend->items[gFrontend_selected_item_index].action != NULL) {
                    pFrontend->items[gFrontend_selected_item_index].action(pFrontend);
                }
            }
        }

        if (PDKeyDown(63)) {
            if (pFrontend->previous == NULL) {
                return 2;
            } else {
                int i;

                gCurrent_frontend_spec->default_item = gFrontend_selected_item_index;
                FRONTEND_DestroyMenu(pFrontend);
                gCurrent_frontend_spec = pFrontend->previous;
                FRONTEND_CreateMenu(gCurrent_frontend_spec);
                Morph_Initialise(pFrontend, gCurrent_frontend_spec);
                for (i = 0; i < gCurrent_frontend_spec->count_scrollers; i++) {
                    gCurrent_frontend_spec->scrollers[i].indexTopItem = gCurrent_frontend_spec->scrollers[i].indexOfItemAtTop;
                }
                return 0;
            }
        }

        if (PDKeyDown(51) || PDKeyDown(52) || (mouse_button == 1 && item_under_mouse != -1)) {
            ToggleSelection(pFrontend);
            if (pFrontend->items[gFrontend_selected_item_index].field_0xc == 2) {
                return pFrontend->items[gFrontend_selected_item_index].field_0xc;
            }
            if (pFrontend->items[gFrontend_selected_item_index].field_0xc == 1) {
                gFrontend_leave_current_menu = 1;
            }
            if (pFrontend->items[gFrontend_selected_item_index].action != NULL) {
                pFrontend->items[gFrontend_selected_item_index].action(pFrontend);
            }
            if (pFrontend->items[gFrontend_selected_item_index].menuInfo != NULL) {
                int i;

                gCurrent_frontend_spec->default_item = gFrontend_selected_item_index;
                for (i = 0; i < gCurrent_frontend_spec->count_scrollers; i++) {
                    gCurrent_frontend_spec->scrollers[i].indexTopItem = gCurrent_frontend_spec->scrollers[i].indexOfItemAtTop;
                }
                FRONTEND_DestroyMenu(pFrontend);
                gCurrent_frontend_spec = pFrontend->items[gFrontend_selected_item_index].menuInfo;
                FRONTEND_CreateMenu(gCurrent_frontend_spec);
                if (gCurrent_frontend_spec != pFrontend->previous) {
                    gCurrent_frontend_spec->previous = pFrontend;
                }
                Morph_Initialise(pFrontend, gCurrent_frontend_spec);
                if (gCurrent_frontend_spec == &gFrontend_QUIT) {
                    gFrontend_selected_item_index = 0;
                } else {
                    gFrontend_selected_item_index = gCurrent_frontend_spec->default_item;
                }
                for (i = 0; i < gCurrent_frontend_spec->count_scrollers; i++) {
                    gCurrent_frontend_spec->scrollers[i].indexTopItem = gCurrent_frontend_spec->scrollers[i].indexOfItemAtTop;
                }
                RefreshRacesScroller(gCurrent_frontend_spec);
            }
            return pFrontend->items[original_selected_index].field_0xc;
        }
    }
    ServiceGame();
    return gFrontend_leave_current_menu != 0;
#else
    __asm {
            sub esp, 0x10
            push ebx
            push ebp
            push esi
            push edi
            mov edi, dword ptr [gFrontend_selected_item_index]
            mov esi, ecx
            mov dword ptr [esp + 0x10], edi
            mov dword ptr [gTyping_slot], 0xffffffff
            call EdgeTriggerModeOn
            call KillSplashScreen
            mov eax, dword ptr [esi + 0x110]
            xor ebx, ebx
            cmp eax, ebx
            je L0046c117
            mov ecx, esi
            call eax
            cmp eax, ebx
            je L0046c3eb
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x10
            ret
    L0046c117:
            call PollKeys
            call EdgeTriggerModeOff
            call PDAnyKeyDown
            cmp eax, -1
            je L0046c140
            cmp eax, 4
            je L0046c140
            mov dword ptr [gMouse_in_use], ebx
            call PDGetTotalTime
            mov dword ptr [gFrontend_time_last_input], eax
    L0046c140:
            call EdgeTriggerModeOn
            mov eax, dword ptr [gMouse_in_use]
            mov dword ptr [esp + 0x14], ebx
            cmp eax, ebx
            je L0046c1a1
            call PDGetTotalTime
            lea edx, [esp + 0x18]
            lea ecx, [esp + 0x1c]
            mov dword ptr [gFrontend_time_last_input], eax
            call GetMousePosition
            call EitherMouseButtonDown
            mov edx, dword ptr [esp + 0x1c]
            mov ecx, dword ptr [gCurrent_frontend_spec]
            mov dword ptr [esp + 0x14], eax
            mov eax, dword ptr [esp + 0x18]
            push eax
            call GetItemAtMousePos
            mov ebp, eax
            cmp ebp, -1
            je L0046c195
            mov dword ptr [gFrontend_selected_item_index], ebp
            jmp L0046c1a5
    L0046c195:
            mov dword ptr [gFrontend_selected_item_index], 0x63
            jmp L0046c1a5
    L0046c1a1:
            mov ebp, dword ptr [esp + 0x1c]
    L0046c1a5:
            mov ecx, 0x48
            call PDKeyDown
            test eax, eax
            jne L0046c1c5
            mov ecx, 0x59
            call PDKeyDown
            test eax, eax
            je L0046c259
    L0046c1c5:
            mov ecx, dword ptr [gFrontend_selected_item_index]
            lea eax, [ecx - 1]
            mov ecx, dword ptr [gCurrent_frontend_spec]
            cmp eax, ebx
            jl L0046c200
            mov edx, eax
            shl edx, 3
            sub edx, eax
            lea edx, [edx + edx*2]
            lea edx, [eax + edx*2]
            lea edx, [ecx + edx*8 + 0x15c]
    L0046c1ec:
            cmp dword ptr [edx - 4], ebx
            jle L0046c1f5
            cmp dword ptr [edx], ebx
            jne L0046c211
    L0046c1f5:
            dec eax
            sub edx, 0x158
            cmp eax, ebx
            jge L0046c1ec
    L0046c200:
            mov edx, dword ptr [ecx + 0x104]
            call FindPrevActiveItem
            mov ecx, dword ptr [gCurrent_frontend_spec]
    L0046c211:
            mov edx, dword ptr [gMouse_in_use]
            mov dword ptr [gFrontend_selected_item_index], eax
            cmp edx, ebx
            jne L0046c259
            cmp eax, dword ptr [ecx + 0x87a4]
            jl L0046c259
            cmp eax, dword ptr [ecx + 0x87a8]
            jg L0046c259
            mov ecx, esi
            call ToggleSelection
            mov ecx, dword ptr [gFrontend_selected_item_index]
            mov eax, ecx
            shl eax, 3
            sub eax, ecx
            lea eax, [eax + eax*2]
            lea ecx, [ecx + eax*2]
            mov eax, dword ptr [esi + ecx*8 + 0x134]
            cmp eax, ebx
            je L0046c259
            mov ecx, esi
            call eax
    L0046c259:
            mov ecx, 0x49
            call PDKeyDown
            test eax, eax
            jne L0046c279
            mov ecx, 0x53
            call PDKeyDown
            test eax, eax
            je L0046c317
    L0046c279:
            mov edx, dword ptr [esi + 0x104]
            mov eax, dword ptr [gFrontend_selected_item_index]
            dec edx
            cmp eax, edx
            jge L0046c56a
            mov edi, dword ptr [gCurrent_frontend_spec]
            inc eax
            mov ecx, dword ptr [edi + 0x104]
            lea edx, [ecx - 1]
            cmp eax, edx
            jg L0046c2c9
            mov ecx, eax
            shl ecx, 3
            sub ecx, eax
            lea ecx, [ecx + ecx*2]
            lea ecx, [eax + ecx*2]
            lea ecx, [edi + ecx*8 + 0x15c]
    L0046c2b5:
            cmp dword ptr [ecx - 4], ebx
            jle L0046c2be
            cmp dword ptr [ecx], ebx
            jne L0046c2cb
    L0046c2be:
            inc eax
            add ecx, 0x158
            cmp eax, edx
            jle L0046c2b5
    L0046c2c9:
            xor eax, eax
    L0046c2cb:
            mov ecx, dword ptr [gMouse_in_use]
            mov dword ptr [gFrontend_selected_item_index], eax
            cmp ecx, ebx
            jne L0046c313
            cmp eax, dword ptr [edi + 0x87a4]
            jl L0046c313
            cmp eax, dword ptr [edi + 0x87a8]
            jg L0046c313
            mov ecx, esi
            call ToggleSelection
            mov ecx, dword ptr [gFrontend_selected_item_index]
            mov eax, ecx
            shl eax, 3
            sub eax, ecx
            lea edx, [eax + eax*2]
            lea eax, [ecx + edx*2]
            mov eax, dword ptr [esi + eax*8 + 0x134]
            cmp eax, ebx
            je L0046c313
            mov ecx, esi
            call eax
    L0046c313:
            mov edi, dword ptr [esp + 0x10]
    L0046c317:
            mov ecx, 0x3f
            call PDKeyDown
            test eax, eax
            je L0046c3c3
            cmp dword ptr [esi + 0x114], ebx
            jne L0046c33e
            mov eax, 2
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x10
            ret
    L0046c33e:
            mov ecx, dword ptr [gCurrent_frontend_spec]
            mov edx, dword ptr [gFrontend_selected_item_index]
            mov dword ptr [ecx + 0x118], edx
            mov ecx, esi
            call FRONTEND_DestroyMenu
            mov ecx, dword ptr [esi + 0x114]
            mov dword ptr [gCurrent_frontend_spec], ecx
            call FRONTEND_CreateMenu
            mov edx, dword ptr [gCurrent_frontend_spec]
            mov ecx, esi
            call Morph_Initialise
            mov ecx, dword ptr [gCurrent_frontend_spec]
            xor edx, edx
            mov eax, dword ptr [ecx + 0x118]
            mov dword ptr [gFrontend_selected_item_index], eax
            mov eax, dword ptr [ecx + 0x8790]
            cmp eax, ebx
            jle L0046c3b4
            xor eax, eax
    L0046c394:
            mov esi, dword ptr [ecx + eax + 0x87c0]
            inc edx
            mov dword ptr [ecx + eax + 0x87a0], esi
            mov ecx, dword ptr [gCurrent_frontend_spec]
            add eax, 0x34
            cmp edx, dword ptr [ecx + 0x8790]
            jl L0046c394
    L0046c3b4:
            call UpdateScrollPositions
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x10
            ret
    L0046c3c3:
            mov ecx, 0x33
            call PDKeyDown
            test eax, eax
            jne L0046c405
            mov ecx, 0x34
            call PDKeyDown
            test eax, eax
            jne L0046c405
            cmp dword ptr [esp + 0x14], 1
            jne L0046c3eb
            cmp ebp, -1
            jne L0046c405
    L0046c3eb:
            call ServiceGame
            mov ecx, dword ptr [gFrontend_leave_current_menu]
            xor eax, eax
            cmp ecx, ebx
            setne al
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x10
            ret
    L0046c405:
            mov ecx, esi
            call ToggleSelection
            mov ecx, dword ptr [gFrontend_selected_item_index]
            mov eax, ecx
            shl eax, 3
            sub eax, ecx
            lea edx, [eax + eax*2]
            lea eax, [ecx + edx*2]
            lea edx, [esi + eax*8]
            mov eax, dword ptr [esi + eax*8 + 0x13c]
            cmp eax, 2
            jne L0046c436
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x10
            ret
    L0046c436:
            cmp eax, 1
            jne L0046c440
            mov dword ptr [gFrontend_leave_current_menu], eax
    L0046c440:
            mov edx, dword ptr [edx + 0x134]
            cmp edx, ebx
            je L0046c454
            mov ecx, esi
            call edx
            mov ecx, dword ptr [gFrontend_selected_item_index]
    L0046c454:
            mov eax, ecx
            shl eax, 3
            sub eax, ecx
            lea edx, [eax + eax*2]
            lea eax, [ecx + edx*2]
            cmp dword ptr [esi + eax*8 + 0x138], ebx
            je L0046c54e
            mov edx, dword ptr [gCurrent_frontend_spec]
            mov dword ptr [edx + 0x118], ecx
            mov eax, dword ptr [gCurrent_frontend_spec]
            xor edx, edx
            cmp dword ptr [eax + 0x8790], ebx
            jle L0046c4aa
            xor ecx, ecx
    L0046c48b:
            mov ebp, dword ptr [eax + ecx + 0x87a0]
            inc edx
            mov dword ptr [eax + ecx + 0x87c0], ebp
            mov eax, dword ptr [gCurrent_frontend_spec]
            add ecx, 0x34
            cmp edx, dword ptr [eax + 0x8790]
            jl L0046c48b
    L0046c4aa:
            mov ecx, esi
            call FRONTEND_DestroyMenu
            mov ecx, dword ptr [gFrontend_selected_item_index]
            mov eax, ecx
            shl eax, 3
            sub eax, ecx
            lea eax, [eax + eax*2]
            lea ecx, [ecx + eax*2]
            mov ecx, dword ptr [esi + ecx*8 + 0x138]
            mov dword ptr [gCurrent_frontend_spec], ecx
            call FRONTEND_CreateMenu
            mov edx, dword ptr [gCurrent_frontend_spec]
            mov eax, dword ptr [esi + 0x114]
            cmp edx, eax
            je L0046c4f2
            mov dword ptr [edx + 0x114], esi
            mov edx, dword ptr [gCurrent_frontend_spec]
    L0046c4f2:
            mov ecx, esi
            call Morph_Initialise
            mov ecx, dword ptr [gCurrent_frontend_spec]
            cmp ecx, offset gFrontend_QUIT
            jne L0046c50f
            mov dword ptr [gFrontend_selected_item_index], ebx
            jmp L0046c51b
    L0046c50f:
            mov edx, dword ptr [ecx + 0x118]
            mov dword ptr [gFrontend_selected_item_index], edx
    L0046c51b:
            mov eax, dword ptr [ecx + 0x8790]
            xor edx, edx
            cmp eax, ebx
            jle L0046c549
            xor eax, eax
    L0046c529:
            mov ebx, dword ptr [ecx + eax + 0x87c0]
            inc edx
            mov dword ptr [ecx + eax + 0x87a0], ebx
            mov ecx, dword ptr [gCurrent_frontend_spec]
            add eax, 0x34
            cmp edx, dword ptr [ecx + 0x8790]
            jl L0046c529
    L0046c549:
            call UpdateScrollPositions
    L0046c54e:
            mov eax, edi
            shl eax, 3
            sub eax, edi
            lea eax, [eax + eax*2]
            lea ecx, [edi + eax*2]
            pop edi
            mov eax, dword ptr [esi + ecx*8 + 0x13c]
            pop esi
            pop ebp
            pop ebx
            add esp, 0x10
            ret
    L0046c56a:
            mov edi, dword ptr [gCurrent_frontend_spec]
            mov dword ptr [gFrontend_selected_item_index], 0xffffffff
            xor ecx, ecx
            mov eax, dword ptr [edi + 0x104]
            lea edx, [eax - 1]
            cmp edx, ebx
            jl L0046c2c9
            lea eax, [edi + 0x15c]
    L0046c593:
            cmp dword ptr [eax - 4], ebx
            jle L0046c59c
            cmp dword ptr [eax], ebx
            jne L0046c5ab
    L0046c59c:
            inc ecx
            add eax, 0x158
            cmp ecx, edx
            jle L0046c593
            jmp L0046c2c9
    L0046c5ab:
            mov eax, ecx
            jmp L0046c2cb
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00471ba0
int C2_HOOK_FASTCALL Ians_GetItemAtMousePos(tFrontend_spec* pFrontend, int pX, int pY) {
    tFrontend_slider* slider;
    br_vertex* vertices;
    tFrontend_item_spec* item;
    tFrontend_brender_item* brender_item;
    int i;

    item = gCurrent_frontend_spec->items;
    brender_item = gFrontend_brender_items;
    for (i = 0; i < pFrontend->count_items; i++, brender_item++, item++) {
        vertices = brender_item->model->vertices;
        if (pX >= vertices[1].p.v[0] && pX <= vertices[3].p.v[0]
                && pY >= -vertices[0].p.v[1] && pY <= -vertices[1].p.v[1]
                && item->visible
                && item->enabled >= 1) {
            return i;
        }
    }
    for (slider = gCurrent_frontend_scrollbars; slider != NULL; slider = slider->next) {
        for (i = 0; i < 3; i++) {
            vertices = gFrontend_brender_items[slider->itemid_start + i].model->vertices;
            if (pX >= vertices[1].p.v[0] && pX <= vertices[3].p.v[0]
                    && pY >= -vertices[0].p.v[1] && pY <= -vertices[1].p.v[1]) {
                return slider->itemid_start + i;
            }
        }
    }
    return -1;
}

// FUNCTION: CARMA2_HW 0x00471cf0

tFrontend_slider* C2_HOOK_FASTCALL GetAnyActiveSlider(void) {
    tFrontend_slider* slider;

    for (slider = gCurrent_frontend_scrollbars; slider != NULL; slider = slider->next) {
        if (gFrontend_selected_item_index >= slider->itemid_left_reference
                && gFrontend_selected_item_index < slider->itemid_left_reference + 3) {
            return slider;
        }
        if (gFrontend_selected_item_index >= slider->itemid_start
                && gFrontend_selected_item_index < slider->itemid_start + 3) {
            return slider;
        }
    }
    return NULL;
}

// FUNCTION: CARMA2_HW 0x00471d70

int C2_HOOK_FASTCALL ScrollSet_TranslateItemToIndex(tConnected_items* pConnected, int pItem) {
    int i;
    int offset;

    offset = -1;
    for (i = 0; i < pConnected->count_ranges; i++) {

        if (pItem >= pConnected->range_starts[i] && pItem < pConnected->range_starts[i] + pConnected->range_length) {

            offset = pItem - pConnected->range_starts[i];
            break;
        }
    }
    if (offset == -1) {
        return -1;
    }
    return pConnected->field_0x8 + offset;
}

// FUNCTION: CARMA2_HW 0x00471d30

tConnected_items* C2_HOOK_FASTCALL GetScrollSet(int pItem) {
    tConnected_items* scrollset;

    scrollset = gConnected_items;
    while (scrollset != NULL) {
        int i;

        for (i = 0; i < scrollset->count_ranges; i++) {
            if (pItem >= scrollset->range_starts[i] && pItem < scrollset->range_starts[i] + scrollset->range_length) {
                return scrollset;
            }
        }
        scrollset = scrollset->next;
    }
    return NULL;
}


void C2_HOOK_FASTCALL ScrollSet_DisplayEntry(tConnected_items* pScroll_set, int pItem) {

    if (pItem < pScroll_set->field_0x8 || pItem >= pScroll_set->field_0x8 + pScroll_set->range_length) {
        if (pItem < pScroll_set->field_0x0 - pScroll_set->range_length) {
            pScroll_set->field_0x8 = pItem;
        } else {
            pScroll_set->field_0x8 = pScroll_set->field_0x0 - pScroll_set->range_length;
        }
    }
}

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x004720e0
void C2_HOOK_FASTCALL RefreshScrollSet(tFrontend_spec* pFrontend) {
    int i;

    for (i = 0; i < gControls_scroller.range_length; i++) {

        strcpy(pFrontend->items[43 + i].text, GetMiscString(i + 140 + gControls_scroller.field_0x8));
        strcpy(pFrontend->items[53 + i].text, gKey_names[gKey_mapping[gControls_frontend_to_key_mapping_lut[i + gControls_scroller.field_0x8]] + 2]);
    }
    if (gControls_scroller.field_0x8) {
        pFrontend->items[63].visible = 1;
    } else {
        pFrontend->items[63].visible = 0;
    }
    if (gControls_scroller.field_0x8 == gControls_scroller.field_0x0 - gControls_scroller.range_length) {
        pFrontend->items[64].visible = 0;
    } else {
        pFrontend->items[64].visible = 1;
    }

    FuckWithWidths(pFrontend);
}
#pragma auto_inline(on)


int C2_HOOK_FASTCALL DetermineKeyArrayIndex(void) {
    int start;

    start = gFrontend_selected_item_index;
    if (gFrontend_selected_item_index > 52) {
        start = gFrontend_selected_item_index - 10;
    }
    return start + (gControls_scroller.field_0x8 - 43);
}

// FUNCTION: CARMA2_HW 0x00471dd0
int C2_NAKED C2_HOOK_FASTCALL Generic_FindNextActiveItem(tFrontend_spec* pFrontend, int pItem) {
#ifndef CARPOCALYPSE2_MATCHING
    int start_item_group;
    tConnected_items* connected;
    tFrontend_slider* start_active_slider;
    tStruct_00686508* start_up_down;
    int i;
    int original_selected_item_index;

    original_selected_item_index = gFrontend_selected_item_index;
    gFrontend_selected_item_index = pItem;
    start_active_slider = GetAnyActiveSlider();
    start_item_group = pFrontend->items[gFrontend_selected_item_index].group;
    start_up_down = GetUpDown(pItem);

    for (connected = gConnected_items; connected != NULL; connected = connected->next) {
        int i;

        for (i = 0; i < connected->count_ranges; i++) {

            if (pItem == connected->range_starts[i] + connected->range_length - 1) {
                break;
            }
        }
        if (i < connected->count_ranges) {
            break;
        }
    }
    if (connected != NULL) {
        int item = ScrollSet_TranslateItemToIndex(connected, pItem);

        if (item == connected->field_0x8 + connected->range_length - 1 && connected->field_0x0 > connected->field_0x8 + connected->range_length) {
            connected->field_0x8 += 1;
            RefreshScrollSet(pFrontend);
            gFrontend_selected_item_index = item;
            return pItem;
        }
    }

    for (i = 1; i < pFrontend->count_items; i++) {
        int next_item;
        tFrontend_slider* next_active_slider;
        tStruct_00686508* next_up_down;

        next_item = (pItem + i) % pFrontend->count_items;
        if (pFrontend->items[next_item].enabled <= 0) {
            continue;
        }
        if (!pFrontend->items[next_item].visible) {
            continue;
        }
        gFrontend_selected_item_index = pItem;

        next_active_slider = GetAnyActiveSlider();
        if (start_active_slider != NULL && next_active_slider == start_active_slider) {
            continue;
        }
        if (next_active_slider != NULL && (next_active_slider->flags & 0x1)) {
            continue;
        }
        if (pFrontend->count_groups != 0 && start_item_group != 0 && start_item_group == pFrontend->items[next_item].group) {
            continue;
        }
        if (connected != NULL && GetScrollSet(next_item) == connected) {
            continue;
        }
        if (start_up_down == NULL) {
            return next_item;
        }
        next_up_down = GetUpDown(next_item);
        if (start_up_down != next_up_down) {
            if (next_up_down == NULL) {
                return next_item;
            }
            if (next_up_down->field_0x0 == next_item) {
                return next_item;
            }
        }
    }
    gFrontend_selected_item_index = original_selected_item_index;
    return 0;
#else
    __asm {
            sub esp, 0x20
            mov eax, dword ptr [gFrontend_selected_item_index]
            push ebx
            push ebp
            mov ebp, edx
            mov dword ptr [esp + 0x20], eax
            mov eax, dword ptr [gCurrent_frontend_scrollbars]
            mov edx, ecx
            push esi
            test eax, eax
            push edi
            mov dword ptr [esp + 0x1c], ebp
            mov dword ptr [esp + 0x14], edx
            mov dword ptr [gFrontend_selected_item_index], ebp
            je L00471e26
    L00471dfb:
            mov ecx, dword ptr [eax + 4]
            cmp ebp, ecx
            jl L00471e0d
            add ecx, 3
            cmp ebp, ecx
            jl L004720cb
    L00471e0d:
            mov ecx, dword ptr [eax + 8]
            cmp ebp, ecx
            jl L00471e1f
            add ecx, 3
            cmp ebp, ecx
            jl L004720cb
    L00471e1f:
            mov eax, dword ptr [eax + 0x20]
            test eax, eax
            jne L00471dfb
    L00471e26:
            mov dword ptr [esp + 0x18], 0
    L00471e2e:
            mov eax, ebp
            shl eax, 3
            sub eax, ebp
            lea ecx, [eax + eax*2]
            lea eax, [ebp + ecx*2]
            mov ecx, dword ptr [edx + eax*8 + 0x170]
            mov eax, dword ptr [gPTR_00686508]
            test eax, eax
            mov dword ptr [esp + 0x2c], ecx
            je L00471e6d
    L00471e50:
            cmp ebp, dword ptr [eax]
            je L00471e67
            cmp ebp, dword ptr [eax + 4]
            je L00471e67
            cmp ebp, dword ptr [eax + 8]
            je L00471e67
            mov eax, dword ptr [eax + 0xc]
            test eax, eax
            jne L00471e50
            jmp L00471e6d
    L00471e67:
            mov dword ptr [esp + 0x20], eax
            jmp L00471e75
    L00471e6d:
            mov dword ptr [esp + 0x20], 0
    L00471e75:
            mov ebx, dword ptr [gConnected_items]
            test ebx, ebx
            mov dword ptr [esp + 0x10], ebx
            je L00471f29
    L00471e87:
            mov edx, dword ptr [ebx + 0xc]
            xor eax, eax
            test edx, edx
            jle L00471ea8
            mov esi, dword ptr [ebx + 4]
            lea ecx, [ebx + 0x10]
    L00471e96:
            mov edi, dword ptr [ecx]
            lea edi, [edi + esi - 1]
            cmp ebp, edi
            je L00471ea8
            inc eax
            add ecx, 4
            cmp eax, edx
            jl L00471e96
    L00471ea8:
            cmp eax, edx
            jl L00471eb3
            mov ebx, dword ptr [ebx + 0x2c]
            test ebx, ebx
            jne L00471e87
    L00471eb3:
            test ebx, ebx
            mov dword ptr [esp + 0x10], ebx
            je L00471f29
            mov esi, dword ptr [ebx + 0xc]
            or edi, 0xffffffff
            xor ecx, ecx
            test esi, esi
            jle L00471ef1
            lea edx, [ebx + 0x10]
    L00471eca:
            mov eax, dword ptr [edx]
            cmp ebp, eax
            jl L00471edd
            mov ebx, dword ptr [ebx + 4]
            add ebx, eax
            cmp ebp, ebx
            jl L00471ee7
            mov ebx, dword ptr [esp + 0x10]
    L00471edd:
            inc ecx
            add edx, 4
            cmp ecx, esi
            jl L00471eca
            jmp L00471ef1
    L00471ee7:
            mov ebx, dword ptr [esp + 0x10]
            mov edi, ebp
            sub edi, dword ptr [ebx + ecx*4 + 0x10]
    L00471ef1:
            cmp edi, -1
            jne L00471efa
            or eax, edi
            jmp L00471eff
    L00471efa:
            mov eax, dword ptr [ebx + 8]
            add eax, edi
    L00471eff:
            mov ecx, dword ptr [ebx + 8]
            mov edx, dword ptr [ebx + 4]
            add edx, ecx
            lea esi, [edx - 1]
            cmp eax, esi
            jne L00471f29
            cmp edx, dword ptr [ebx]
            jge L00471f29
            inc ecx
            mov dword ptr [ebx + 8], ecx
            mov ecx, dword ptr [esp + 0x14]
            call RefreshScrollSet
            mov eax, ebp
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x20
            ret
    L00471f29:
            mov ecx, dword ptr [esp + 0x14]
            mov eax, 1
            mov dword ptr [esp + 0x24], eax
            mov ecx, dword ptr [ecx + 0x104]
            cmp ecx, eax
            jle L004720b8
    L00471f44:
            add eax, ebp
            cdq
            idiv ecx
            mov eax, edx
            lea esi, [eax + 1]
            mov edx, esi
            shl edx, 3
            sub edx, esi
            lea edx, [edx + edx*2]
            lea ecx, [esi + edx*2]
            mov edx, dword ptr [esp + 0x14]
            cmp dword ptr [edx + ecx*8], 1
            jl L00472078
            mov edx, eax
            shl edx, 3
            sub edx, eax
            lea ecx, [edx + edx*2]
            lea edx, [eax + ecx*2]
            mov ecx, dword ptr [esp + 0x14]
            lea esi, [ecx + edx*8]
            mov ecx, dword ptr [ecx + edx*8 + 0x15c]
            test ecx, ecx
            je L00472078
            mov edx, dword ptr [gCurrent_frontend_scrollbars]
            mov dword ptr [gFrontend_selected_item_index], eax
            test edx, edx
            je L00471fbe
    L00471f9b:
            mov ecx, dword ptr [edx + 4]
            cmp eax, ecx
            jl L00471fa9
            add ecx, 3
            cmp eax, ecx
            jl L00471fc0
    L00471fa9:
            mov ecx, dword ptr [edx + 8]
            cmp eax, ecx
            jl L00471fb7
            add ecx, 3
            cmp eax, ecx
            jl L00471fc0
    L00471fb7:
            mov edx, dword ptr [edx + 0x20]
            test edx, edx
            jne L00471f9b
    L00471fbe:
            xor edx, edx
    L00471fc0:
            mov ecx, dword ptr [esp + 0x18]
            test ecx, ecx
            je L00471fd0
            cmp edx, ecx
            je L00472078
    L00471fd0:
            test edx, edx
            je L00471fdd
            test byte ptr [edx], 1
            jne L00472078
    L00471fdd:
            mov edx, dword ptr [esp + 0x14]
            mov ecx, dword ptr [edx + 0xad18]
            test ecx, ecx
            je L00471ffb
            mov ecx, dword ptr [esp + 0x2c]
            test ecx, ecx
            je L00471ffb
            cmp ecx, dword ptr [esi + 0x170]
            je L00472078
    L00471ffb:
            test ebx, ebx
            je L00472043
            mov esi, dword ptr [gConnected_items]
            test esi, esi
            je L00472037
    L00472009:
            mov ecx, dword ptr [esi + 0xc]
            xor edi, edi
            test ecx, ecx
            jle L00472030
            lea ebx, [esi + 0x10]
    L00472015:
            mov edx, dword ptr [ebx]
            cmp eax, edx
            jl L00472028
            mov ebp, dword ptr [esi + 4]
            add ebp, edx
            cmp eax, ebp
            mov ebp, dword ptr [esp + 0x1c]
            jl L00472039
    L00472028:
            inc edi
            add ebx, 4
            cmp edi, ecx
            jl L00472015
    L00472030:
            mov esi, dword ptr [esi + 0x2c]
            test esi, esi
            jne L00472009
    L00472037:
            xor esi, esi
    L00472039:
            mov ecx, dword ptr [esp + 0x10]
            cmp esi, ecx
            mov ebx, ecx
            je L00472078
    L00472043:
            mov ecx, dword ptr [esp + 0x20]
            test ecx, ecx
            je L004720a6
            mov edx, dword ptr [gPTR_00686508]
            test edx, edx
            je L0047206a
    L00472055:
            cmp eax, dword ptr [edx]
            je L0047206c
            cmp eax, dword ptr [edx + 4]
            je L0047206c
            cmp eax, dword ptr [edx + 8]
            je L0047206c
            mov edx, dword ptr [edx + 0xc]
            test edx, edx
            jne L00472055
    L0047206a:
            xor edx, edx
    L0047206c:
            cmp ecx, edx
            je L00472078
            test edx, edx
            je L004720a6
            cmp dword ptr [edx], eax
            je L004720a6
    L00472078:
            mov ecx, dword ptr [esp + 0x14]
            mov eax, dword ptr [esp + 0x24]
            inc eax
            mov ecx, dword ptr [ecx + 0x104]
            mov dword ptr [esp + 0x24], eax
            cmp eax, ecx
            jl L00471f44
            mov eax, dword ptr [esp + 0x28]
            mov dword ptr [gFrontend_selected_item_index], eax
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x20
            ret
    L004720a6:
            mov edx, dword ptr [esp + 0x28]
            mov dword ptr [gFrontend_selected_item_index], edx
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x20
            ret
    L004720b8:
            mov eax, dword ptr [esp + 0x28]
            pop edi
            pop esi
            mov dword ptr [gFrontend_selected_item_index], eax
            pop ebp
            xor eax, eax
            pop ebx
            add esp, 0x20
            ret
    L004720cb:
            mov dword ptr [esp + 0x18], eax
            jmp L00471e2e
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
    }
#endif
}


int C2_HOOK_FASTCALL Generic_FindPrevActiveItem(tFrontend_spec* pFrontend, int pItem) {
    int start_selected_item_index;
    int start_item_group;
    tConnected_items* connected;
    tFrontend_slider* start_active_slider;
    tStruct_00686508* start_up_down;
    int i;

    start_selected_item_index = gFrontend_selected_item_index;
    gFrontend_selected_item_index = pItem;
    start_active_slider = GetAnyActiveSlider();
    start_item_group = pFrontend->items[pItem].group;
    start_up_down = GetUpDown(pItem);

    for (connected = gConnected_items; connected != NULL; connected = connected->next) {
        int i;

        for (i = 0; i < connected->count_ranges; i++) {

            if (gFrontend_selected_item_index == connected->range_starts[i]) {
                break;
            }
        }
        if (i < connected->count_ranges) {
            break;
        }
    }
    if (connected != NULL) {
        int item = ScrollSet_TranslateItemToIndex(connected, pItem);

        if (item == connected->field_0x8 && connected->field_0x8 > 0) {
            connected->field_0x8 -= 1;
            RefreshScrollSet(pFrontend);
            return pItem;
        }
    }

    for (i = 1; i < pFrontend->count_items; i++) {
        int prev_item;
        int prev_group;
        tFrontend_slider* prev_active_slider;
        tStruct_00686508* prev_up_down;

        prev_item = pItem - i;
        if (prev_item < 0) {
            prev_item += pFrontend->count_items;
        }
        if (pFrontend->items[prev_item].enabled <= 0) {
            continue;
        }
        if (!pFrontend->items[prev_item].visible) {
            continue;
        }

        gFrontend_selected_item_index = prev_item;
        prev_active_slider = GetAnyActiveSlider();
        if (start_active_slider != NULL && prev_active_slider == start_active_slider) {
            continue;
        }
        if (prev_active_slider != NULL && (prev_active_slider->flags & 0x1)) {
            continue;
        }
        if (pFrontend->count_groups != 0 && start_item_group != 0) {
            int prev_group;

            prev_group = pFrontend->items[prev_item].group;
            if (prev_group == start_item_group) {
                continue;
            }
            if (prev_group != 0) {
                int prev_prev_item;

                prev_prev_item = prev_item - 1;
                if (prev_prev_item < 0) {
                    prev_prev_item = pFrontend->count_items - 1;
                }
                while (pFrontend->items[prev_prev_item].group == prev_group) {
                    prev_item = prev_prev_item;
                    prev_prev_item -= 1;
                    if (prev_prev_item < 0) {
                        prev_prev_item = pFrontend->count_items - 1;
                    }
                }
            }
        } else if (pFrontend->count_groups != 0) {
            int prev_group;

            prev_group = pFrontend->items[prev_item].group;
            if (prev_group != 0) {
                int prev_prev_item;

                prev_prev_item = prev_item - 1;
                if (prev_prev_item < 0) {
                    prev_prev_item = pFrontend->count_items - 1;
                }
                while (pFrontend->items[prev_prev_item].group == prev_group) {
                    prev_item = prev_prev_item;
                    prev_prev_item -= 1;
                    if (prev_prev_item < 0) {
                        prev_prev_item = pFrontend->count_items - 1;
                    }
                }
            }
        }

        if (connected != NULL && GetScrollSet(prev_item) == connected) {
            continue;
        }
        prev_up_down = GetUpDown(prev_item);
        if (start_up_down == NULL) {
            if (prev_up_down == NULL) {
                gFrontend_selected_item_index = start_selected_item_index;
                return prev_item;
            }
            prev_group = prev_up_down->field_0x0;
        } else {
            if (start_up_down == prev_up_down) {
                continue;
            }
            if (prev_up_down == NULL) {
                gFrontend_selected_item_index = start_selected_item_index;
                return prev_item;
            }
            prev_group = prev_up_down->field_0x0;
        }
        if (prev_group == prev_item) {
            gFrontend_selected_item_index = start_selected_item_index;
            return prev_item;
        }
    }
    gFrontend_selected_item_index = start_selected_item_index;
    return 0;
}


int C2_HOOK_FASTCALL TranslateSliderItem(tFrontend_slider* pScroller, int pIndex) {

    if (pIndex >= pScroller->itemid_left_reference && pIndex <= pScroller->itemid_left_reference + 2) {
        return pIndex;
    } else {
        return pScroller->itemid_left_reference + 2;
    }
}


static __inline void PrepareSliders(tFrontend_spec* pFrontend) {
    tFrontend_slider* slider;

    for (slider = gCurrent_frontend_scrollbars; slider != NULL; slider = slider->next) {
        float v;
        br_model* model;

        pFrontend->items[slider->itemid_start + 0].x = pFrontend->items[slider->itemid_left_reference].x;
        pFrontend->items[slider->itemid_start + 0].width = 8;
        pFrontend->items[slider->itemid_start + 1].x = pFrontend->items[slider->itemid_left_reference].x + 8;
        pFrontend->items[slider->itemid_start + 1].width = slider->width;
        pFrontend->items[slider->itemid_start + 2].x = pFrontend->items[slider->itemid_left_reference].x + 8 + slider->width;
        pFrontend->items[slider->itemid_start + 2].width = 8;
        if (slider->value < 0.f) {
            slider->value = 0.f;
        } else if (slider->value > 1.f) {
            slider->value = 1.f;
        }

        model = gFrontend_brender_items[slider->itemid_left_reference + 1].model;
        v = model->vertices[1].p.v[0] + slider->value * slider->width;
        model->vertices[2].p.v[0] = model->vertices[3].p.v[0] = v;
        BrModelUpdate(model, BR_MODU_VERTEX_POSITIONS);

        model = gFrontend_brender_items[slider->itemid_left_reference + 2].model;
        model->vertices[0].p.v[0] = model->vertices[1].p.v[0] = v;
        model->vertices[2].p.v[0] = model->vertices[3].p.v[0] = v + 8.f;
        BrModelUpdate(model, BR_MODU_VERTEX_POSITIONS);
    }
}


tFrontend_slider* C2_HOOK_FASTCALL GetActiveSlider(void) {
    tFrontend_slider* slider;

    for (slider = gCurrent_frontend_scrollbars; slider != NULL; slider = slider->next) {

        if (slider->flags & 0x1) {
            continue;
        }
        if ((gFrontend_selected_item_index >= slider->itemid_left_reference && gFrontend_selected_item_index <= slider->itemid_left_reference + 2)
                || (gFrontend_selected_item_index >= slider->itemid_start && gFrontend_selected_item_index <= slider->itemid_start + 2)) {

            return slider;
        }
    }
    return slider;
}


void C2_HOOK_FASTCALL Generic_EventEffect(void) {

    DRS3StartSound(gEffects_outlet, eSoundId_Done);
}


tStruct_00686508* C2_HOOK_FASTCALL GetUpDown(int pItem) {
    tStruct_00686508 *up_down;

    for (up_down = gPTR_00686508; up_down != NULL; up_down = up_down->next) {
        if (pItem == up_down->field_0x0
                || pItem == up_down->field_0x4
                || pItem == up_down->field_0x8) {

            return up_down;
        }
    }
    return NULL;
}

// FUNCTION: CARMA2_HW 0x00470c20

int C2_NAKED C2_HOOK_FASTCALL Generic_MenuHandler(tFrontend_spec* pFrontend) {
#ifndef CARPOCALYPSE2_MATCHING
    int timeout;
    int original_item;
    int selected_item;
    int item_mouse;
    int button_down;
    int key;
    int mouse_x;
    tFrontend_slider* slider;

    item_mouse = 0; /* Added by carpocalypse2 */

    if (gTyping) {
        int input;

        input = ProcessInputString();
        FuckWithWidths(pFrontend);
        if (input < 0) {
            ToggleTyping(pFrontend);
            if (!gTyping && strlen(pFrontend->items[gFrontend_selected_item_index].text) != 0) {

                strcpy(gProgram_state.player_name, pFrontend->items[gFrontend_selected_item_index].text);
                SaveOptions();
            }
        }
        FuckWithWidths(pFrontend);
        ServiceGame();
        return gFrontend_leave_current_menu ? 1 : 0;
    }

    timeout = pFrontend->timeout != 0 && PDGetTotalTime() >= gFrontend_time_last_input + pFrontend->timeout;

    if (!gFrontend_scrollbars_updated) {
        gFrontend_scrollbars_updated = 1;
        PrepareSliders(pFrontend);
    }
    original_item = gFrontend_selected_item_index;
    button_down = 0;
    gTyping_slot = -1;
    EdgeTriggerModeOn();
    KillSplashScreen();
    PollKeys();
    EdgeTriggerModeOff();
    key = PDAnyKeyDown();
    if (key != -1 && key != 4) {
        gMouse_in_use = 0;
        ResetInterfaceTimeout();
    }
    EdgeTriggerModeOn();
    selected_item = gFrontend_selected_item_index;
    if (gINT_00688444) {
        int mouse_y;

        GetMousePosition(&mouse_x, &mouse_y);
        if (EitherMouseButtonDown()) {

            item_mouse = 1;
        } else {

            gINT_00688444 = 0;
            if (gMouse_in_use) {
                int item;

                ResetInterfaceTimeout();
                button_down = EitherMouseButtonDown();
                gFrontend_selected_item_index = Ians_GetItemAtMousePos(gCurrent_frontend_spec, mouse_x, mouse_y);
                if (gFrontend_selected_item_index == -1) {
                    gFrontend_selected_item_index = 99;
                }
                item = GetItemAtMousePos(gCurrent_frontend_spec, mouse_x, mouse_y);
                selected_item = 99;
                item_mouse = button_down != 0;
                if (item != -1) {
                    selected_item = item;
                }
            }
        }
    } else {

        if (gMouse_in_use) {
            int mouse_y;
            int item;

            GetMousePosition(&mouse_x, &mouse_y);
            button_down = EitherMouseButtonDown();
            if (button_down) {
                ResetInterfaceTimeout();
            }
            /* ??? */
            if (original_item != mouse_x || original_item != mouse_y) {
                ResetInterfaceTimeout();
            }
            gFrontend_selected_item_index = Ians_GetItemAtMousePos(gCurrent_frontend_spec, mouse_x, mouse_y);
            if (gFrontend_selected_item_index == -1) {
                gFrontend_selected_item_index = 99;
            }
            item = GetItemAtMousePos(gCurrent_frontend_spec, mouse_x, mouse_y);
            selected_item = 99;
            item_mouse = button_down != 0;
            if (item != -1) {
                selected_item = item;
            }
        }
    }

    slider = GetActiveSlider();

    if (slider != NULL) {
        int new_pos;
        int value_changed;

        if (item_mouse == 1) {
            value_changed = 1;
            new_pos = mouse_x - gCurrent_frontend_spec->items[slider->itemid_left_reference + 1].x;
        } else if (PDKeyDown(70)) {
            value_changed = 1;
            new_pos = (int)((float)slider->width * slider->value - (float)slider->width / 10);
        } else if (PDKeyDown(71)) {
            value_changed = 1;
            new_pos = (int)((float)slider->width * slider->value + (float)slider->width / 10);
        } else {
            value_changed = 0;
        }
        if (value_changed) {
            br_model* model;
            float new_vertex_pos;

            gINT_00688444 = 1;
            if (new_pos < 0) {
                new_pos = 0;
            } else if (new_pos > slider->width) {
                new_pos = slider->width;
            }

            model = gFrontend_brender_items[slider->itemid_left_reference + 1].model;
            new_vertex_pos = model->vertices[1].p.v[0] + new_pos;
            model->vertices[2].p.v[0] = model->vertices[3].p.v[0] = new_vertex_pos;
            BrModelUpdate(model, BR_MODU_VERTEX_POSITIONS);

            model = gFrontend_brender_items[slider->itemid_left_reference + 2].model;
            model->vertices[0].p.v[0] = model->vertices[1].p.v[0] = new_vertex_pos;
            model->vertices[2].p.v[0] = model->vertices[3].p.v[0] = new_vertex_pos + 8.f;
            BrModelUpdate(model, BR_MODU_VERTEX_POSITIONS);

            slider->value = (float)new_pos / (float)slider->width;
            if (slider->callback != NULL) {
                slider->callback(slider);
            }
        }
        selected_item = TranslateSliderItem(slider, gFrontend_selected_item_index);
    }
    gFrontend_selected_item_index = selected_item;

    if (PDKeyDown(72) || PDKeyDown(89)) {

        Generic_EventEffect();
        gFrontend_selected_item_index = Generic_FindPrevActiveItem(gCurrent_frontend_spec, gFrontend_selected_item_index);

        if (!gMouse_in_use
                && gFrontend_selected_item_index >= gCurrent_frontend_spec->scrollers[0].indexFirstScrollableItem
                && gFrontend_selected_item_index <= gCurrent_frontend_spec->scrollers[0].indexLastScrollableItem) {

            ToggleSelection(pFrontend);
            if (pFrontend->items[gFrontend_selected_item_index].action != NULL) {
                pFrontend->items[gFrontend_selected_item_index].action(pFrontend);
            }
        }
        slider = GetActiveSlider();
        if (slider != NULL) {
            gFrontend_selected_item_index = TranslateSliderItem(slider, gFrontend_selected_item_index);
        }
    }
    if (PDKeyDown(73) || PDKeyDown(83)) {

        Generic_EventEffect();
        if (gFrontend_selected_item_index >= pFrontend->count_items - 1) {
            gFrontend_selected_item_index = -1;
        }
        gFrontend_selected_item_index = Generic_FindNextActiveItem(gCurrent_frontend_spec, gFrontend_selected_item_index);
        if (!gMouse_in_use
                && gFrontend_selected_item_index >= gCurrent_frontend_spec->scrollers[0].indexFirstScrollableItem
                && gFrontend_selected_item_index <= gCurrent_frontend_spec->scrollers[0].indexLastScrollableItem) {

            ToggleSelection(pFrontend);
            if (pFrontend->items[gFrontend_selected_item_index].action != NULL) {
                pFrontend->items[gFrontend_selected_item_index].action(pFrontend);
            }
        }
        slider = GetActiveSlider();
        if (slider != NULL) {

            gFrontend_selected_item_index = TranslateSliderItem(slider, gFrontend_selected_item_index);
        }
    } else if (PDKeyDown(70) || PDKeyDown(85)) {
        int item;
        tStruct_00686508* up_down;
        int group;

        item = gFrontend_selected_item_index;
        up_down = GetUpDown(gFrontend_selected_item_index);
        if (up_down != NULL) {

            gFrontend_selected_item_index = up_down->field_0x8;
            if (pFrontend->items[gFrontend_selected_item_index].action != NULL) {
                pFrontend->items[gFrontend_selected_item_index].action(pFrontend);
            }
        }
        gFrontend_selected_item_index = item;
        group = pFrontend->items[gFrontend_selected_item_index].group;
        if (group != 0) {
            if (gFrontend_selected_item_index == pFrontend->radios[group - 1].indexFirstItem) {
                gFrontend_selected_item_index = pFrontend->radios[group - 1].indexLastItem;
            } else {
                gFrontend_selected_item_index = item - 1;
            }
        }
    } else if (PDKeyDown(71) || PDKeyDown(87)) {
        int item;
        tStruct_00686508* up_down;
        int group;

        up_down = GetUpDown(gFrontend_selected_item_index);
        item = gFrontend_selected_item_index;
        if (up_down != NULL) {
            gFrontend_selected_item_index = up_down->field_0x4;
            if (pFrontend->items[gFrontend_selected_item_index].action != NULL) {
                pFrontend->items[gFrontend_selected_item_index].action(pFrontend);
            }
        }
        gFrontend_selected_item_index = item;
        group = pFrontend->items[gFrontend_selected_item_index].group;
        if (group != 0) {
            if (item == pFrontend->radios[group - 1].indexLastItem) {
                gFrontend_selected_item_index = pFrontend->radios[group - 1].indexFirstItem;
            } else {
                gFrontend_selected_item_index = item + 1;
            }
        }
    }
    if (timeout || PDKeyDown(63)) {
        int i;

        if (pFrontend == &gFrontend_CONTROLS && Controls_Ok(&gFrontend_CONTROLS) == 4) {
            return 0;
        }
        Generic_LinkOutEffect();
        if (pFrontend->previous == NULL) {
            return 1;
        }
        gCurrent_frontend_spec->default_item = gFrontend_selected_item_index;
        FRONTEND_DestroyMenu(pFrontend);
        gCurrent_frontend_spec = pFrontend->previous;
        FRONTEND_CreateMenu(gCurrent_frontend_spec);
        gFrontend_selected_item_index = gCurrent_frontend_spec->default_item;

        for (i = 0; i < gCurrent_frontend_spec->count_scrollers; i++) {

            gCurrent_frontend_spec->scrollers[i].indexTopItem = gCurrent_frontend_spec->scrollers[i].indexOfItemAtTop;
        }
        UpdateScrollPositions(gCurrent_frontend_spec);
        Morph_Initialise(pFrontend, gCurrent_frontend_spec);
        return 0;
    } else if (PDKeyDown(51) || PDKeyDown(52) || (button_down == 1 && item_mouse != -1)) {
        int ret;
        tFrontend_spec* next_menu;
        int going_back;

        ToggleSelection(pFrontend);
        if (pFrontend->items[gFrontend_selected_item_index].field_0xc == 1) {
            gFrontend_leave_current_menu = 1;
        }
        next_menu = pFrontend->items[gFrontend_selected_item_index].menuInfo;
        going_back = next_menu == (tFrontend_spec*)0x1;
        if (going_back) {
            next_menu = pFrontend->previous;
        }
        if (pFrontend->items[gFrontend_selected_item_index].action == NULL) {
            ret = pFrontend->items[gFrontend_selected_item_index].field_0xc;
        } else {
            if (next_menu == NULL && pFrontend->items[gFrontend_selected_item_index].action != temp) {
                Generic_EventEffect();
            }
            ret = pFrontend->items[gFrontend_selected_item_index].action(pFrontend);
        }
        if (ret == 4) {
            return 0;
        }
        if (next_menu != NULL) {
            int i;

            if (going_back) {
                Generic_LinkOutEffect();
            } else {
                Generic_LinkInEffect();
            }
            gCurrent_frontend_spec->default_item = gFrontend_selected_item_index;
            for (i = 0; i < gCurrent_frontend_spec->count_scrollers; i++) {

                gCurrent_frontend_spec->scrollers[i].indexOfItemAtTop = gCurrent_frontend_spec->scrollers[i].indexTopItem;
            }
            FRONTEND_DestroyMenu(pFrontend);
            gCurrent_frontend_spec = next_menu;
            FRONTEND_CreateMenu(next_menu);
            if (gCurrent_frontend_spec != pFrontend->previous) {
                gCurrent_frontend_spec->previous = pFrontend;
            }
            Morph_Initialise(pFrontend, gCurrent_frontend_spec);
            if (gCurrent_frontend_spec == &gFrontend_QUIT) {
                gFrontend_selected_item_index = 0;
            } else {
                gFrontend_selected_item_index = gCurrent_frontend_spec->default_item;
            }
            for (i = 0; i < gCurrent_frontend_spec->count_scrollers; i++) {

                gCurrent_frontend_spec->scrollers[i].indexTopItem = gCurrent_frontend_spec->scrollers[i].indexOfItemAtTop;
            }
            UpdateScrollPositions(gCurrent_frontend_spec);
            return ret;
        }
        if (!going_back) {
            return ret;
        }
        return 1;
    } else {
        ServiceGame();
        return gFrontend_leave_current_menu ? 1 : 0;
    }
#else
    __asm {
            sub esp, 0x30
            mov eax, dword ptr [gTyping]
            push ebx
            push ebp
            push esi
            test eax, eax
            push edi
            mov ebp, ecx
            mov dword ptr [esp + 0x30], 0
            je L00470cb5
            call ProcessInputString
            mov ecx, ebp
            mov esi, eax
            call FuckWithWidths
            test esi, esi
            jge L00470ca9
            mov ecx, ebp
            call ToggleTyping
            mov eax, dword ptr [gTyping]
            test eax, eax
            jne L00470ca9
            mov ecx, dword ptr [gFrontend_selected_item_index]
            mov eax, ecx
            shl eax, 3
            sub eax, ecx
            lea eax, [eax + eax*2]
            lea ecx, [ecx + eax*2]
            xor eax, eax
            lea edx, [ebp + ecx*8 + 0x189]
            or ecx, 0xffffffff
            mov edi, edx
            repne scasb
            not ecx
            dec ecx
            je L00470ca9
            mov edi, edx
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            mov edx, ecx
            mov esi, edi
            mov edi, offset gProgram_state+0x1a10
            shr ecx, 2
            rep movsd
            mov ecx, edx
            and ecx, 3
            rep movsb
            call SaveOptions
    L00470ca9:
            mov ecx, ebp
            call FuckWithWidths
            jmp L004718de
    L00470cb5:
            mov eax, dword ptr [ebp + 0x100]
            test eax, eax
            je L00470cde
            call PDGetTotalTime
            mov ecx, dword ptr [gFrontend_time_last_input]
            mov esi, dword ptr [ebp + 0x100]
            add ecx, esi
            cmp eax, ecx
            jb L00470cde
            mov dword ptr [esp + 0x30], 1
    L00470cde:
            mov eax, dword ptr [gFrontend_scrollbars_updated]
            test eax, eax
            jne L00470ead
            mov esi, dword ptr [gCurrent_frontend_scrollbars]
            mov dword ptr [gFrontend_scrollbars_updated], 1
            test esi, esi
            je L00470ead
    L00470d03:
            mov ecx, dword ptr [esi + 4]
            mov eax, ecx
            shl eax, 3
            sub eax, ecx
            lea edx, [eax + eax*2]
            lea edx, [ecx + edx*2]
            mov ecx, dword ptr [esi + 8]
            mov eax, ecx
            shl eax, 3
            mov edx, dword ptr [ebp + edx*8 + 0x148]
            sub eax, ecx
            lea eax, [eax + eax*2]
            lea ecx, [ecx + eax*2]
            mov dword ptr [ebp + ecx*8 + 0x148], edx
            mov ecx, dword ptr [esi + 8]
            mov eax, ecx
            mov edx, 8
            shl eax, 3
            sub eax, ecx
            lea eax, [eax + eax*2]
            lea ecx, [ecx + eax*2]
            mov dword ptr [ebp + ecx*8 + 0x150], edx
            mov ecx, dword ptr [esi + 4]
            mov eax, ecx
            shl eax, 3
            sub eax, ecx
            lea eax, [eax + eax*2]
            lea ecx, [ecx + eax*2]
            mov edi, dword ptr [ebp + ecx*8 + 0x148]
            mov ecx, dword ptr [esi + 8]
            mov eax, ecx
            add edi, edx
            shl eax, 3
            sub eax, ecx
            lea eax, [eax + eax*2]
            lea ecx, [ecx + eax*2]
            mov dword ptr [ebp + ecx*8 + 0x2a0], edi
            mov ecx, dword ptr [esi + 8]
            mov eax, ecx
            shl eax, 3
            sub eax, ecx
            lea eax, [eax + eax*2]
            lea ecx, [ecx + eax*2]
            mov eax, dword ptr [esi + 0xc]
            mov dword ptr [ebp + ecx*8 + 0x2a8], eax
            mov ecx, dword ptr [esi + 4]
            mov eax, ecx
            shl eax, 3
            sub eax, ecx
            lea eax, [eax + eax*2]
            lea ecx, [ecx + eax*2]
            mov eax, dword ptr [ebp + ecx*8 + 0x148]
            mov ecx, dword ptr [esi + 0xc]
            lea edi, [eax + ecx + 8]
            mov ecx, dword ptr [esi + 8]
            mov eax, ecx
            shl eax, 3
            sub eax, ecx
            lea eax, [eax + eax*2]
            lea ecx, [ecx + eax*2]
            mov dword ptr [ebp + ecx*8 + 0x3f8], edi
            mov ecx, dword ptr [esi + 8]
            mov eax, ecx
            shl eax, 3
            sub eax, ecx
            lea eax, [eax + eax*2]
            lea ecx, [ecx + eax*2]
            mov dword ptr [ebp + ecx*8 + 0x400], edx
            fld dword ptr [esi + 0x10]
            fcomp dword ptr [gDouble_0058a068+0x30]
            fnstsw ax
            test ah, 1
            je L00470dfc
            mov dword ptr [esi + 0x10], 0
            jmp L00470e13
    L00470dfc:
            fld dword ptr [esi + 0x10]
            fcomp qword ptr [gDouble_0058a068+0x78]
            fnstsw ax
            test ah, 0x41
            jne L00470e13
            mov dword ptr [esi + 0x10], 0x3f800000
    L00470e13:
            mov eax, dword ptr [esi + 4]
            fild dword ptr [esi + 0xc]
            lea edx, [eax + eax*4]
            fmul dword ptr [esi + 0x10]
            lea eax, [eax + edx*2]
            mov edi, dword ptr [eax*4 + gFrontend_brender_items+0x30]
            mov ebx, dword ptr [edi + 8]
            fadd dword ptr [ebx + 0x28]
            call _ftol
            mov dword ptr [esp + 0x24], eax
            push 1
            fild dword ptr [esp + 0x28]
            push edi
            fst dword ptr [esp + 0x3c]
            fstp dword ptr [ebx + 0x78]
            mov eax, dword ptr [edi + 8]
            mov ecx, dword ptr [eax + 0x78]
            mov dword ptr [eax + 0x50], ecx
            call BrModelUpdate
            mov eax, dword ptr [esi + 4]
            add esp, 8
            lea edx, [eax + eax*4]
            push 1
            lea eax, [eax + edx*2]
            mov edx, dword ptr [esp + 0x38]
            mov eax, dword ptr [eax*4 + gFrontend_brender_items+0x5c]
            push eax
            mov ecx, dword ptr [eax + 8]
            mov dword ptr [ecx + 0x28], edx
            mov ecx, dword ptr [eax + 8]
            mov edx, dword ptr [ecx + 0x28]
            mov dword ptr [ecx], edx
            mov ecx, dword ptr [esp + 0x2c]
            mov edx, dword ptr [eax + 8]
            add ecx, 8
            mov dword ptr [esp + 0x3c], ecx
            fild dword ptr [esp + 0x3c]
            fstp dword ptr [edx + 0x78]
            mov ecx, dword ptr [eax + 8]
            mov edx, dword ptr [ecx + 0x78]
            mov dword ptr [ecx + 0x50], edx
            call BrModelUpdate
            mov esi, dword ptr [esi + 0x20]
            add esp, 8
            test esi, esi
            jne L00470d03
    L00470ead:
            mov eax, dword ptr [gFrontend_selected_item_index]
            xor ebx, ebx
            mov dword ptr [esp + 0x3c], eax
            mov dword ptr [esp + 0x24], ebx
            mov dword ptr [gTyping_slot], 0xffffffff
            call EdgeTriggerModeOn
            call KillSplashScreen
            call PollKeys
            call EdgeTriggerModeOff
            call PDAnyKeyDown
            cmp eax, -1
            je L00470ef9
            cmp eax, 4
            je L00470ef9
            mov dword ptr [gMouse_in_use], ebx
            call PDGetTotalTime
            mov dword ptr [gFrontend_time_last_input], eax
    L00470ef9:
            call EdgeTriggerModeOn
            mov eax, dword ptr [gINT_00688444]
            mov ecx, dword ptr [gFrontend_selected_item_index]
            cmp eax, ebx
            mov edi, ecx
            je L00470f95
            lea edx, [esp + 0x18]
            lea ecx, [esp + 0x14]
            call GetMousePosition
            call EitherMouseButtonDown
            test eax, eax
            jne L00470f8b
            mov eax, dword ptr [gMouse_in_use]
            mov dword ptr [gINT_00688444], ebx
            cmp eax, ebx
            je L00471042
            call PDGetTotalTime
            mov dword ptr [gFrontend_time_last_input], eax
            call EitherMouseButtonDown
            mov edx, dword ptr [esp + 0x18]
            xor ecx, ecx
            cmp eax, ebx
            push edx
            mov edx, dword ptr [esp + 0x18]
            mov dword ptr [esp + 0x28], eax
            setne cl
            mov ebx, ecx
            mov ecx, dword ptr [gCurrent_frontend_spec]
            call Ians_GetItemAtMousePos
            cmp eax, -1
            mov edi, 0x63
            mov dword ptr [gFrontend_selected_item_index], eax
            jne L00470f81
            mov dword ptr [gFrontend_selected_item_index], edi
    L00470f81:
            mov eax, dword ptr [esp + 0x18]
            push eax
            jmp L00471020
    L00470f8b:
            mov ebx, 1
            jmp L00471046
    L00470f95:
            cmp dword ptr [gMouse_in_use], ebx
            je L0047103c
            lea edx, [esp + 0x18]
            lea ecx, [esp + 0x14]
            call GetMousePosition
            call EitherMouseButtonDown
            mov esi, eax
            cmp esi, ebx
            mov dword ptr [esp + 0x24], esi
            je L00470fc7
            call PDGetTotalTime
            mov dword ptr [gFrontend_time_last_input], eax
    L00470fc7:
            mov ecx, dword ptr [esp + 0x3c]
            mov eax, dword ptr [esp + 0x14]
            cmp ecx, eax
            jne L00470fdf
            mov edx, dword ptr [esp + 0x3c]
            mov eax, dword ptr [esp + 0x18]
            cmp edx, eax
            je L00470fe9
    L00470fdf:
            call PDGetTotalTime
            mov dword ptr [gFrontend_time_last_input], eax
    L00470fe9:
            mov ecx, dword ptr [esp + 0x18]
            mov edx, dword ptr [esp + 0x14]
            xor eax, eax
            cmp esi, ebx
            setne al
            push ecx
            mov ecx, dword ptr [gCurrent_frontend_spec]
            mov ebx, eax
            call Ians_GetItemAtMousePos
            cmp eax, -1
            mov edi, 0x63
            mov dword ptr [gFrontend_selected_item_index], eax
            jne L0047101b
            mov dword ptr [gFrontend_selected_item_index], edi
    L0047101b:
            mov edx, dword ptr [esp + 0x18]
            push edx
    L00471020:
            mov edx, dword ptr [esp + 0x18]
            mov ecx, dword ptr [gCurrent_frontend_spec]
            call GetItemAtMousePos
            cmp eax, -1
            mov dword ptr [esp + 0x34], eax
            je L00471046
            mov edi, eax
            jmp L00471046
    L0047103c:
            mov ebx, dword ptr [esp + 0x34]
            jmp L0047104c
    L00471042:
            mov ebx, dword ptr [esp + 0x34]
    L00471046:
            mov ecx, dword ptr [gFrontend_selected_item_index]
    L0047104c:
            mov esi, dword ptr [gCurrent_frontend_scrollbars]
            test esi, esi
            je L0047107e
    L00471056:
            test byte ptr [esi], 1
            jne L00471077
            mov eax, dword ptr [esi + 4]
            cmp ecx, eax
            jl L00471069
            add eax, 3
            cmp ecx, eax
            jl L00471080
    L00471069:
            mov eax, dword ptr [esi + 8]
            cmp ecx, eax
            jl L00471077
            add eax, 3
            cmp ecx, eax
            jl L00471080
    L00471077:
            mov esi, dword ptr [esi + 0x20]
            test esi, esi
            jne L00471056
    L0047107e:
            xor esi, esi
    L00471080:
            test esi, esi
            je L00471242
            cmp ebx, 1
            jne L004710be
            mov edx, dword ptr [esi + 4]
            mov edi, dword ptr [gCurrent_frontend_spec]
            mov eax, edx
            shl eax, 3
            sub eax, edx
            lea eax, [eax + eax*2]
            lea edx, [edx + eax*2]
            mov eax, dword ptr [esp + 0x14]
            mov ebx, dword ptr [edi + edx*8 + 0x2a0]
            mov edi, 1
            sub eax, ebx
            mov dword ptr [esp + 0x10], eax
            jmp L00471154
    L004710be:
            mov ecx, 0x46
            call PDKeyDown
            test eax, eax
            je L00471103
            mov ecx, dword ptr [esi + 0xc]
            mov eax, 0x66666667
            imul ecx
            sar edx, 2
            mov eax, edx
            mov dword ptr [esp + 0x1c], ecx
            fild dword ptr [esp + 0x1c]
            shr eax, 0x1f
            add edx, eax
            mov edi, 1
            fmul dword ptr [esi + 0x10]
            mov dword ptr [esp + 0x2c], edx
            fisub dword ptr [esp + 0x2c]
            call _ftol
            mov dword ptr [esp + 0x10], eax
            jmp L0047114e
    L00471103:
            mov ecx, 0x47
            call PDKeyDown
            test eax, eax
            je L00471148
            mov ecx, dword ptr [esi + 0xc]
            mov eax, 0x66666667
            imul ecx
            sar edx, 2
            mov dword ptr [esp + 0x1c], ecx
            mov ecx, edx
            fild dword ptr [esp + 0x1c]
            shr ecx, 0x1f
            add edx, ecx
            mov edi, 1
            fmul dword ptr [esi + 0x10]
            mov dword ptr [esp + 0x2c], edx
            fiadd dword ptr [esp + 0x2c]
            call _ftol
            mov dword ptr [esp + 0x10], eax
            jmp L0047114e
    L00471148:
            mov eax, dword ptr [esp + 0x10]
            xor edi, edi
    L0047114e:
            mov ecx, dword ptr [gFrontend_selected_item_index]
    L00471154:
            test edi, edi
            je L0047122f
            test eax, eax
            mov dword ptr [gINT_00688444], 1
            jge L00471174
            mov dword ptr [esp + 0x10], 0
            jmp L0047117f
    L00471174:
            mov ecx, dword ptr [esi + 0xc]
            cmp eax, ecx
            jle L0047117f
            mov dword ptr [esp + 0x10], ecx
    L0047117f:
            mov eax, dword ptr [esi + 4]
            fild dword ptr [esp + 0x10]
            lea edx, [eax + eax*4]
            fstp dword ptr [esp + 0x1c]
            lea eax, [eax + edx*2]
            mov edi, dword ptr [eax*4 + gFrontend_brender_items+0x30]
            mov ebx, dword ptr [edi + 8]
            fld dword ptr [ebx + 0x28]
            fadd dword ptr [esp + 0x1c]
            call _ftol
            mov dword ptr [esp + 0x20], eax
            push 1
            fild dword ptr [esp + 0x24]
            push edi
            fst dword ptr [esp + 0x34]
            fstp dword ptr [ebx + 0x78]
            mov eax, dword ptr [edi + 8]
            mov ecx, dword ptr [eax + 0x78]
            mov dword ptr [eax + 0x50], ecx
            call BrModelUpdate
            mov eax, dword ptr [esi + 4]
            add esp, 8
            lea edx, [eax + eax*4]
            push 1
            lea eax, [eax + edx*2]
            mov edx, dword ptr [esp + 0x30]
            mov eax, dword ptr [eax*4 + gFrontend_brender_items+0x5c]
            push eax
            mov ecx, dword ptr [eax + 8]
            mov dword ptr [ecx + 0x28], edx
            mov ecx, dword ptr [eax + 8]
            mov edx, dword ptr [ecx + 0x28]
            mov dword ptr [ecx], edx
            mov ecx, dword ptr [esp + 0x28]
            mov edx, dword ptr [eax + 8]
            add ecx, 8
            mov dword ptr [esp + 0x34], ecx
            fild dword ptr [esp + 0x34]
            fstp dword ptr [edx + 0x78]
            mov ecx, dword ptr [eax + 8]
            mov edx, dword ptr [ecx + 0x78]
            mov dword ptr [ecx + 0x50], edx
            call BrModelUpdate
            fild dword ptr [esi + 0xc]
            mov eax, dword ptr [esi + 0x1c]
            add esp, 8
            test eax, eax
            fdivr dword ptr [esp + 0x1c]
            fstp dword ptr [esi + 0x10]
            je L00471229
            mov ecx, esi
            call eax
    L00471229:
            mov ecx, dword ptr [gFrontend_selected_item_index]
    L0047122f:
            mov esi, dword ptr [esi + 4]
            cmp ecx, esi
            jl L0047123f
            lea eax, [esi + 3]
            mov edi, ecx
            cmp ecx, eax
            jl L00471242
    L0047123f:
            lea edi, [esi + 2]
    L00471242:
            mov ecx, 0x48
            mov dword ptr [gFrontend_selected_item_index], edi
            call PDKeyDown
            test eax, eax
            jne L00471268
            mov ecx, 0x59
            call PDKeyDown
            test eax, eax
            je L004715d7
    L00471268:
            mov ecx, dword ptr [gEffects_outlet]
            mov edx, 0xbbc
            call DRS3StartSound
            mov eax, dword ptr [gCurrent_frontend_scrollbars]
            mov edx, dword ptr [gFrontend_selected_item_index]
            mov ebx, dword ptr [gCurrent_frontend_spec]
            xor esi, esi
            cmp eax, esi
            mov edi, edx
            je L004712bc
    L00471291:
            mov ecx, dword ptr [eax + 4]
            cmp edx, ecx
            jl L004712a3
            add ecx, 3
            cmp edx, ecx
            jl L00471b90
    L004712a3:
            mov ecx, dword ptr [eax + 8]
            cmp edx, ecx
            jl L004712b5
            add ecx, 3
            cmp edx, ecx
            jl L00471b90
    L004712b5:
            mov eax, dword ptr [eax + 0x20]
            cmp eax, esi
            jne L00471291
    L004712bc:
            mov dword ptr [esp + 0x1c], esi
    L004712c0:
            mov eax, edx
            shl eax, 3
            sub eax, edx
            lea ecx, [eax + eax*2]
            lea eax, [edx + ecx*2]
            mov ecx, dword ptr [ebx + eax*8 + 0x170]
            mov eax, dword ptr [gPTR_00686508]
            cmp eax, esi
            mov dword ptr [esp + 0x38], ecx
            je L004712fe
    L004712e1:
            cmp edx, dword ptr [eax]
            je L004712f8
            cmp edx, dword ptr [eax + 4]
            je L004712f8
            cmp edx, dword ptr [eax + 8]
            je L004712f8
            mov eax, dword ptr [eax + 0xc]
            cmp eax, esi
            jne L004712e1
            jmp L004712fe
    L004712f8:
            mov dword ptr [esp + 0x28], eax
            jmp L00471302
    L004712fe:
            mov dword ptr [esp + 0x28], esi
    L00471302:
            mov eax, dword ptr [gConnected_items]
            cmp eax, esi
            mov dword ptr [esp + 0x10], eax
            je L00471342
    L0047130f:
            mov ecx, dword ptr [esp + 0x10]
            xor eax, eax
            mov ecx, dword ptr [ecx + 0xc]
            test ecx, ecx
            jle L0047132f
            mov esi, dword ptr [esp + 0x10]
            add esi, 0x10
    L00471323:
            cmp edx, dword ptr [esi]
            je L0047132f
            inc eax
            add esi, 4
            cmp eax, ecx
            jl L00471323
    L0047132f:
            cmp eax, ecx
            jl L00471342
            mov eax, dword ptr [esp + 0x10]
            mov eax, dword ptr [eax + 0x2c]
            test eax, eax
            mov dword ptr [esp + 0x10], eax
            jne L0047130f
    L00471342:
            mov esi, dword ptr [esp + 0x10]
            test esi, esi
            je L00471370
            mov ecx, esi
            call ScrollSet_TranslateItemToIndex
            mov ecx, dword ptr [esi + 8]
            cmp eax, ecx
            jne L00471370
            test ecx, ecx
            jle L00471370
            mov edx, esi
            dec ecx
            mov dword ptr [edx + 8], ecx
            mov ecx, ebx
            call RefreshScrollSet
            mov esi, edi
            jmp L00471534
    L00471370:
            mov eax, dword ptr [ebx + 0x104]
            mov dword ptr [esp + 0x2c], 1
            cmp eax, 1
            jle L00471532
            dec edi
            mov dword ptr [esp + 0x20], edi
    L0047138c:
            mov esi, dword ptr [esp + 0x20]
            test esi, esi
            jge L00471396
            add esi, eax
    L00471396:
            lea ecx, [esi + 1]
            mov eax, ecx
            shl eax, 3
            sub eax, ecx
            lea eax, [eax + eax*2]
            lea ecx, [ecx + eax*2]
            cmp dword ptr [ebx + ecx*8], 1
            jl L00471512
            mov eax, esi
            shl eax, 3
            sub eax, esi
            lea edx, [eax + eax*2]
            lea eax, [esi + edx*2]
            lea edi, [ebx + eax*8]
            mov eax, dword ptr [ebx + eax*8 + 0x15c]
            test eax, eax
            je L00471512
            mov dword ptr [gFrontend_selected_item_index], esi
            call GetAnyActiveSlider
            mov ecx, dword ptr [esp + 0x1c]
            test ecx, ecx
            je L004713ea
            cmp eax, ecx
            je L00471512
    L004713ea:
            test eax, eax
            je L004713f7
            test byte ptr [eax], 1
            jne L00471512
    L004713f7:
            mov eax, dword ptr [ebx + 0xad18]
            test eax, eax
            je L004714bf
            mov ecx, dword ptr [esp + 0x38]
            test ecx, ecx
            je L0047146b
            mov edx, dword ptr [edi + 0x170]
            cmp ecx, edx
            je L00471512
            test edx, edx
            je L004714bf
            lea eax, [esi - 1]
            test eax, eax
            jge L00471431
            mov eax, dword ptr [ebx + 0x104]
            dec eax
    L00471431:
            mov ecx, eax
            shl ecx, 3
            sub ecx, eax
            lea ecx, [ecx + ecx*2]
            lea ecx, [eax + ecx*2]
            cmp dword ptr [ebx + ecx*8 + 0x170], edx
            jne L004714bf
    L00471447:
            mov esi, eax
            dec eax
            jns L00471453
            mov eax, dword ptr [ebx + 0x104]
            dec eax
    L00471453:
            mov ecx, eax
            shl ecx, 3
            sub ecx, eax
            lea ecx, [ecx + ecx*2]
            lea ecx, [eax + ecx*2]
            cmp dword ptr [ebx + ecx*8 + 0x170], edx
            je L00471447
            jmp L004714bf
    L0047146b:
            test eax, eax
            je L004714bf
            mov edx, dword ptr [edi + 0x170]
            test edx, edx
            je L004714bf
            lea eax, [esi - 1]
            test eax, eax
            jge L00471487
            mov eax, dword ptr [ebx + 0x104]
            dec eax
    L00471487:
            mov ecx, eax
            shl ecx, 3
            sub ecx, eax
            lea ecx, [ecx + ecx*2]
            lea ecx, [eax + ecx*2]
            cmp dword ptr [ebx + ecx*8 + 0x170], edx
            jne L004714bf
    L0047149d:
            mov esi, eax
            dec eax
            jns L004714a9
            mov eax, dword ptr [ebx + 0x104]
            dec eax
    L004714a9:
            mov ecx, eax
            shl ecx, 3
            sub ecx, eax
            lea ecx, [ecx + ecx*2]
            lea ecx, [eax + ecx*2]
            cmp dword ptr [ebx + ecx*8 + 0x170], edx
            je L0047149d
    L004714bf:
            mov eax, dword ptr [esp + 0x10]
            test eax, eax
            je L004714d4
            mov ecx, esi
            call GetScrollSet
            cmp eax, dword ptr [esp + 0x10]
            je L00471512
    L004714d4:
            mov eax, dword ptr [gPTR_00686508]
            test eax, eax
            je L004714f2
    L004714dd:
            cmp esi, dword ptr [eax]
            je L004714f4
            cmp esi, dword ptr [eax + 4]
            je L004714f4
            cmp esi, dword ptr [eax + 8]
            je L004714f4
            mov eax, dword ptr [eax + 0xc]
            test eax, eax
            jne L004714dd
    L004714f2:
            xor eax, eax
    L004714f4:
            mov ecx, dword ptr [esp + 0x28]
            test ecx, ecx
            je L0047150a
            cmp ecx, eax
            je L00471512
            test eax, eax
            je L00471534
            cmp dword ptr [eax], esi
            je L00471534
            jmp L00471512
    L0047150a:
            test eax, eax
            je L00471534
            cmp dword ptr [eax], esi
            je L00471534
    L00471512:
            mov ecx, dword ptr [esp + 0x2c]
            mov esi, dword ptr [esp + 0x20]
            mov eax, dword ptr [ebx + 0x104]
            inc ecx
            dec esi
            cmp ecx, eax
            mov dword ptr [esp + 0x2c], ecx
            mov dword ptr [esp + 0x20], esi
            jl L0047138c
    L00471532:
            xor esi, esi
    L00471534:
            mov eax, dword ptr [gMouse_in_use]
            mov ecx, esi
            test eax, eax
            mov dword ptr [gFrontend_selected_item_index], ecx
            jne L00471589
            mov eax, dword ptr [gCurrent_frontend_spec]
            cmp esi, dword ptr [eax + 0x87a4]
            jl L00471589
            cmp esi, dword ptr [eax + 0x87a8]
            jg L00471589
            mov ecx, ebp
            call ToggleSelection
            mov ecx, dword ptr [gFrontend_selected_item_index]
            mov eax, ecx
            shl eax, 3
            sub eax, ecx
            lea edx, [eax + eax*2]
            lea eax, [ecx + edx*2]
            mov eax, dword ptr [ebp + eax*8 + 0x134]
            test eax, eax
            je L00471589
            mov ecx, ebp
            call eax
            mov ecx, dword ptr [gFrontend_selected_item_index]
    L00471589:
            mov eax, dword ptr [gCurrent_frontend_scrollbars]
            test eax, eax
            je L004715ba
    L00471592:
            test byte ptr [eax], 1
            jne L004715b3
            mov edx, dword ptr [eax + 4]
            cmp ecx, edx
            jl L004715a5
            add edx, 3
            cmp ecx, edx
            jl L004715bc
    L004715a5:
            mov edx, dword ptr [eax + 8]
            cmp ecx, edx
            jl L004715b3
            add edx, 3
            cmp ecx, edx
            jl L004715bc
    L004715b3:
            mov eax, dword ptr [eax + 0x20]
            test eax, eax
            jne L00471592
    L004715ba:
            xor eax, eax
    L004715bc:
            test eax, eax
            je L004715d7
            mov eax, dword ptr [eax + 4]
            cmp ecx, eax
            jl L004715ce
            lea edx, [eax + 3]
            cmp ecx, edx
            jl L004715d1
    L004715ce:
            lea ecx, [eax + 2]
    L004715d1:
            mov dword ptr [gFrontend_selected_item_index], ecx
    L004715d7:
            mov ecx, 0x49
            call PDKeyDown
            test eax, eax
            jne L004717bb
            mov ecx, 0x53
            call PDKeyDown
            test eax, eax
            jne L004717bb
            mov ecx, 0x46
            call PDKeyDown
            test eax, eax
            jne L004716fd
            mov ecx, 0x55
            call PDKeyDown
            test eax, eax
            jne L004716fd
            mov ecx, 0x47
            call PDKeyDown
            test eax, eax
            jne L0047163f
            mov ecx, 0x57
            call PDKeyDown
            test eax, eax
            je L00471890
    L0047163f:
            mov eax, dword ptr [gPTR_00686508]
            mov ecx, dword ptr [gFrontend_selected_item_index]
            test eax, eax
            je L00471663
    L0047164e:
            cmp ecx, dword ptr [eax]
            je L00471665
            cmp ecx, dword ptr [eax + 4]
            je L00471665
            cmp ecx, dword ptr [eax + 8]
            je L00471665
            mov eax, dword ptr [eax + 0xc]
            test eax, eax
            jne L0047164e
    L00471663:
            xor eax, eax
    L00471665:
            test eax, eax
            je L004716af
            mov edx, dword ptr [eax + 4]
            lea esi, [edx*8]
            sub esi, edx
            lea esi, [esi + esi*2]
            lea esi, [edx + esi*2]
            mov edi, dword ptr [ebp + esi*8 + 0x134]
            test edi, edi
            je L004716af
            mov esi, ecx
            mov dword ptr [gFrontend_selected_item_index], edx
            mov ecx, dword ptr [eax + 4]
            mov eax, ecx
            shl eax, 3
            sub eax, ecx
            lea eax, [eax + eax*2]
            lea edx, [ecx + eax*2]
            mov ecx, ebp
            call dword ptr [ebp + edx*8 + 0x134]
            mov ecx, esi
            mov dword ptr [gFrontend_selected_item_index], ecx
    L004716af:
            mov eax, ecx
            shl eax, 3
            sub eax, ecx
            lea eax, [eax + eax*2]
            lea edx, [ecx + eax*2]
            mov eax, dword ptr [ebp + edx*8 + 0x170]
            test eax, eax
            je L00471890
            lea edx, [eax + eax*4]
            lea eax, [eax + edx*2]
            mov edx, dword ptr [ebp + eax*4 + 0x9bc8]
            cmp ecx, edx
            lea eax, [ebp + eax*4]
            jne L004716f1
            mov ecx, dword ptr [eax + 0x9bc4]
            mov dword ptr [gFrontend_selected_item_index], ecx
            jmp L00471890
    L004716f1:
            inc ecx
            mov dword ptr [gFrontend_selected_item_index], ecx
            jmp L00471890
    L004716fd:
            mov eax, dword ptr [gPTR_00686508]
            mov ecx, dword ptr [gFrontend_selected_item_index]
            test eax, eax
            je L00471721
    L0047170c:
            cmp ecx, dword ptr [eax]
            je L00471723
            cmp ecx, dword ptr [eax + 4]
            je L00471723
            cmp ecx, dword ptr [eax + 8]
            je L00471723
            mov eax, dword ptr [eax + 0xc]
            test eax, eax
            jne L0047170c
    L00471721:
            xor eax, eax
    L00471723:
            test eax, eax
            je L0047176d
            mov edx, dword ptr [eax + 8]
            lea esi, [edx*8]
            sub esi, edx
            lea esi, [esi + esi*2]
            lea esi, [edx + esi*2]
            mov edi, dword ptr [ebp + esi*8 + 0x134]
            test edi, edi
            je L0047176d
            mov esi, ecx
            mov dword ptr [gFrontend_selected_item_index], edx
            mov ecx, dword ptr [eax + 8]
            mov eax, ecx
            shl eax, 3
            sub eax, ecx
            lea edx, [eax + eax*2]
            lea eax, [ecx + edx*2]
            mov ecx, ebp
            call dword ptr [ebp + eax*8 + 0x134]
            mov ecx, esi
            mov dword ptr [gFrontend_selected_item_index], ecx
    L0047176d:
            mov eax, ecx
            shl eax, 3
            sub eax, ecx
            lea edx, [eax + eax*2]
            lea eax, [ecx + edx*2]
            mov eax, dword ptr [ebp + eax*8 + 0x170]
            test eax, eax
            je L00471890
            lea edx, [eax + eax*4]
            lea eax, [eax + edx*2]
            mov edx, dword ptr [ebp + eax*4 + 0x9bc4]
            cmp ecx, edx
            lea eax, [ebp + eax*4]
            jne L004717af
            mov ecx, dword ptr [eax + 0x9bc8]
            mov dword ptr [gFrontend_selected_item_index], ecx
            jmp L00471890
    L004717af:
            dec ecx
            mov dword ptr [gFrontend_selected_item_index], ecx
            jmp L00471890
    L004717bb:
            mov ecx, dword ptr [gEffects_outlet]
            mov edx, 0xbbc
            call DRS3StartSound
            mov eax, dword ptr [ebp + 0x104]
            mov edx, dword ptr [gFrontend_selected_item_index]
            dec eax
            cmp edx, eax
            jl L004717e5
            or edx, 0xffffffff
            mov dword ptr [gFrontend_selected_item_index], edx
    L004717e5:
            mov ecx, dword ptr [gCurrent_frontend_spec]
            call Generic_FindNextActiveItem
            mov ecx, dword ptr [gMouse_in_use]
            mov dword ptr [gFrontend_selected_item_index], eax
            test ecx, ecx
            jne L00471842
            mov ecx, dword ptr [gCurrent_frontend_spec]
            cmp eax, dword ptr [ecx + 0x87a4]
            jl L00471842
            cmp eax, dword ptr [ecx + 0x87a8]
            jg L00471842
            mov ecx, ebp
            call ToggleSelection
            mov eax, dword ptr [gFrontend_selected_item_index]
            mov ecx, eax
            shl ecx, 3
            sub ecx, eax
            lea ecx, [ecx + ecx*2]
            lea edx, [eax + ecx*2]
            mov edx, dword ptr [ebp + edx*8 + 0x134]
            test edx, edx
            je L00471842
            mov ecx, ebp
            call edx
            mov eax, dword ptr [gFrontend_selected_item_index]
    L00471842:
            mov ecx, dword ptr [gCurrent_frontend_scrollbars]
            test ecx, ecx
            je L00471874
    L0047184c:
            test byte ptr [ecx], 1
            jne L0047186d
            mov edx, dword ptr [ecx + 4]
            cmp eax, edx
            jl L0047185f
            add edx, 3
            cmp eax, edx
            jl L00471876
    L0047185f:
            mov edx, dword ptr [ecx + 8]
            cmp eax, edx
            jl L0047186d
            add edx, 3
            cmp eax, edx
            jl L00471876
    L0047186d:
            mov ecx, dword ptr [ecx + 0x20]
            test ecx, ecx
            jne L0047184c
    L00471874:
            xor ecx, ecx
    L00471876:
            test ecx, ecx
            je L00471890
            mov ecx, dword ptr [ecx + 4]
            cmp eax, ecx
            jl L00471888
            lea edx, [ecx + 3]
            cmp eax, edx
            jl L0047188b
    L00471888:
            lea eax, [ecx + 2]
    L0047188b:
            mov dword ptr [gFrontend_selected_item_index], eax
    L00471890:
            mov eax, dword ptr [esp + 0x30]
            test eax, eax
            jne L00471ac2
            mov ecx, 0x3f
            call PDKeyDown
            test eax, eax
            jne L00471ac2
            mov ecx, 0x33
            call PDKeyDown
            test eax, eax
            jne L004718f8
            mov ecx, 0x34
            call PDKeyDown
            test eax, eax
            jne L004718f8
            mov eax, dword ptr [esp + 0x24]
            mov esi, 1
            cmp eax, esi
            jne L004718de
            cmp dword ptr [esp + 0x34], -1
            jne L004718fd
    L004718de:
            call ServiceGame
            mov ecx, dword ptr [gFrontend_leave_current_menu]
            xor eax, eax
            test ecx, ecx
            setne al
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x30
            ret
    L004718f8:
            mov esi, 1
    L004718fd:
            mov ecx, ebp
            call ToggleSelection
            mov ecx, dword ptr [gFrontend_selected_item_index]
            mov eax, ecx
            shl eax, 3
            sub eax, ecx
            lea eax, [eax + eax*2]
            lea edx, [ecx + eax*2]
            lea eax, [ebp + edx*8]
            mov edx, dword ptr [ebp + edx*8 + 0x13c]
            cmp edx, esi
            jne L0047192c
            mov dword ptr [gFrontend_leave_current_menu], esi
    L0047192c:
            mov edx, dword ptr [eax + 0x138]
            cmp edx, esi
            jne L0047193e
            mov edi, dword ptr [ebp + 0x114]
            jmp L00471942
    L0047193e:
            mov edi, edx
            xor esi, esi
    L00471942:
            mov eax, dword ptr [eax + 0x134]
            test eax, eax
            je L00471987
            test edi, edi
            jne L0047196d
            cmp eax, offset temp
            je L0047196d
            mov ecx, dword ptr [gEffects_outlet]
            mov edx, 0xbbc
            call DRS3StartSound
            mov ecx, dword ptr [gFrontend_selected_item_index]
    L0047196d:
            mov eax, ecx
            shl eax, 3
            sub eax, ecx
            lea eax, [eax + eax*2]
            lea edx, [ecx + eax*2]
            mov ecx, ebp
            call dword ptr [ebp + edx*8 + 0x134]
            mov ebx, eax
            jmp L0047199f
    L00471987:
            mov ecx, dword ptr [esp + 0x3c]
            mov eax, ecx
            shl eax, 3
            sub eax, ecx
            lea eax, [eax + eax*2]
            lea ecx, [ecx + eax*2]
            mov ebx, dword ptr [ebp + ecx*8 + 0x13c]
    L0047199f:
            cmp ebx, 4
            jne L004719b0
            xor ebx, ebx
            mov eax, ebx
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x30
            ret
    L004719b0:
            test edi, edi
            je L00471ab0
            test esi, esi
            mov edx, 0xbbe
            je L004719c6
            mov edx, 0xbbf
    L004719c6:
            mov ecx, dword ptr [gEffects_outlet]
            call DRS3StartSound
            mov edx, dword ptr [gCurrent_frontend_spec]
            mov eax, dword ptr [gFrontend_selected_item_index]
            mov dword ptr [edx + 0x118], eax
            mov ecx, dword ptr [gCurrent_frontend_spec]
            xor edx, edx
            mov eax, dword ptr [ecx + 0x8790]
            test eax, eax
            jle L00471a16
            xor eax, eax
    L004719f6:
            mov esi, dword ptr [ecx + eax + 0x87a0]
            inc edx
            mov dword ptr [ecx + eax + 0x87c0], esi
            mov ecx, dword ptr [gCurrent_frontend_spec]
            add eax, 0x34
            cmp edx, dword ptr [ecx + 0x8790]
            jl L004719f6
    L00471a16:
            mov ecx, ebp
            call FRONTEND_DestroyMenu
            mov ecx, edi
            mov dword ptr [gCurrent_frontend_spec], edi
            call FRONTEND_CreateMenu
            mov edx, dword ptr [gCurrent_frontend_spec]
            mov eax, dword ptr [ebp + 0x114]
            cmp edx, eax
            je L00471a46
            mov dword ptr [edx + 0x114], ebp
            mov edx, dword ptr [gCurrent_frontend_spec]
    L00471a46:
            mov ecx, ebp
            call Morph_Initialise
            mov ecx, dword ptr [gCurrent_frontend_spec]
            cmp ecx, offset gFrontend_QUIT
            jne L00471a67
            mov dword ptr [gFrontend_selected_item_index], 0
            jmp L00471a73
    L00471a67:
            mov edx, dword ptr [ecx + 0x118]
            mov dword ptr [gFrontend_selected_item_index], edx
    L00471a73:
            mov eax, dword ptr [ecx + 0x8790]
            xor edx, edx
            test eax, eax
            jle L00471aa1
            xor eax, eax
    L00471a81:
            mov esi, dword ptr [ecx + eax + 0x87c0]
            inc edx
            mov dword ptr [ecx + eax + 0x87a0], esi
            mov ecx, dword ptr [gCurrent_frontend_spec]
            add eax, 0x34
            cmp edx, dword ptr [ecx + 0x8790]
            jl L00471a81
    L00471aa1:
            call UpdateScrollPositions
            mov eax, ebx
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x30
            ret
    L00471ab0:
            test esi, esi
            jne L00471b83
            mov eax, ebx
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x30
            ret
    L00471ac2:
            cmp ebp, offset gFrontend_CONTROLS
            jne L00471ae0
            mov ecx, ebp
            call Controls_Ok
            cmp eax, 4
            jne L00471ae0
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x30
            ret
    L00471ae0:
            mov ecx, dword ptr [gEffects_outlet]
            mov edx, 0xbbf
            call DRS3StartSound
            mov eax, dword ptr [ebp + 0x114]
            test eax, eax
            je L00471b83
            mov eax, dword ptr [gCurrent_frontend_spec]
            mov ecx, dword ptr [gFrontend_selected_item_index]
            mov dword ptr [eax + 0x118], ecx
            mov ecx, ebp
            call FRONTEND_DestroyMenu
            mov ecx, dword ptr [ebp + 0x114]
            mov dword ptr [gCurrent_frontend_spec], ecx
            call FRONTEND_CreateMenu
            mov ecx, dword ptr [gCurrent_frontend_spec]
            mov edx, dword ptr [ecx + 0x118]
            mov dword ptr [gFrontend_selected_item_index], edx
            mov eax, dword ptr [ecx + 0x8790]
            xor edx, edx
            test eax, eax
            jle L00471b67
            xor eax, eax
    L00471b47:
            mov esi, dword ptr [ecx + eax + 0x87c0]
            inc edx
            mov dword ptr [ecx + eax + 0x87a0], esi
            mov ecx, dword ptr [gCurrent_frontend_spec]
            add eax, 0x34
            cmp edx, dword ptr [ecx + 0x8790]
            jl L00471b47
    L00471b67:
            call UpdateScrollPositions
            mov edx, dword ptr [gCurrent_frontend_spec]
            mov ecx, ebp
            call Morph_Initialise
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x30
            ret
    L00471b83:
            pop edi
            pop esi
            pop ebp
            mov eax, 1
            pop ebx
            add esp, 0x30
            ret
    L00471b90:
            mov dword ptr [esp + 0x1c], eax
            jmp L004712c0
    }
#endif
}


void C2_HOOK_FASTCALL Generic_MungeActiveItems(tFrontend_spec* pFrontend) {
    int i;

    gCount_connected_items_indices = 0;
    if (gFrontend_selected_item_index != -1) {
        tConnected_items* connected_items = gConnected_items;
        int scroll_area_containing_selected_item = 0;

        for (; connected_items != NULL; connected_items = connected_items->next) {
            for (i = 0; i < connected_items->count_ranges; i++) {
                if (connected_items->range_starts[i] <= gFrontend_selected_item_index
                        && gFrontend_selected_item_index < connected_items->range_starts[i] + connected_items->range_length) {
                    scroll_area_containing_selected_item = gFrontend_selected_item_index - connected_items->range_starts[i] + 1;
                    break;
                }
            }
            if (scroll_area_containing_selected_item != 0) {
                break;
            }
        }
        if (scroll_area_containing_selected_item == 0) {
            gCount_connected_items_indices = 1;
            gConnected_items_indices[0] = gFrontend_selected_item_index;
            pFrontend->items[gFrontend_selected_item_index].flags |= 0x1;
        } else {
            for (i = 0; i < connected_items->count_ranges; i++) {
                gConnected_items_indices[gCount_connected_items_indices] = scroll_area_containing_selected_item + connected_items->range_starts[i] - 1;
                pFrontend->items[scroll_area_containing_selected_item + connected_items->range_starts[i] - 1].flags |= 0x1;
                gCount_connected_items_indices += 1;
            }
        }
    }
}


void C2_HOOK_FASTCALL Generic_UnMungeActiveItems(tFrontend_spec* pFrontend) {
    int i;

    for (i = 0; i < gCount_connected_items_indices; i++) {
        pFrontend->items[gConnected_items_indices[i]].flags &= ~0x1;
    }
    gCount_connected_items_indices = 0;
}

// FUNCTION: CARMA2_HW 0x0046e020

void C2_NAKED C2_HOOK_FASTCALL FRONTEND_DrawMenu(tFrontend_spec* pFrontend) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;
    br_fixed_ls blend_x;

    if (gFrontend_leave_current_menu || pFrontend->count_items <= 0) {
        return;
    }
    gCount_connected_items_indices = 0;
    if (gFrontend_selected_item_index != -1) {
        tConnected_items* connected_items = gConnected_items;
        int scroll_area_containing_selected_item = 0;
        for (; connected_items != NULL; connected_items = connected_items->next) {
            for (i = 0; i < connected_items->count_ranges; i++) {
                if (connected_items->range_starts[i] <= gFrontend_selected_item_index
                        && gFrontend_selected_item_index < connected_items->range_starts[i] + connected_items->range_length) {
                    scroll_area_containing_selected_item = gFrontend_selected_item_index - connected_items->range_starts[i] + 1;
                    break;
                }
            }
            if (scroll_area_containing_selected_item != 0) {
                break;
            }
        }
        if (scroll_area_containing_selected_item == 0) {
            gCount_connected_items_indices = 1;
            gConnected_items_indices[0] = gFrontend_selected_item_index;
            pFrontend->items[gFrontend_selected_item_index].flags |= 0x1;
        } else {
            for (i = 0; i < connected_items->count_ranges; i++) {
                gConnected_items_indices[gCount_connected_items_indices] = scroll_area_containing_selected_item + connected_items->range_starts[i] - 1;
                pFrontend->items[scroll_area_containing_selected_item + connected_items->range_starts[i] - 1].flags |= 0x1;
                gCount_connected_items_indices += 1;
            }
        }
    }
    for (i = 0; i < gFrontend_count_brender_items; i++) {
        tFrontend_brender_item* brender_item = &gFrontend_brender_items[i];
        tFrontend_item_spec* item = &pFrontend->items[i];
        const char* text;

        if (!item->visible) {
            brender_item->actor->render_style = BR_RSTYLE_NONE;
            continue;
        }

        brender_item->actor->render_style = BR_RSTYLE_FACES;
        text = item->stringId >= 0x400 ? item->text : IString_Get(item->stringId);
        if (item->flags & 0x1) {
            if (text != NULL) {
                TransparentPolyFontTextInABox(item->unlitFont, text,
                    item->x, item->y, item->x + item->width, item->y + item->height, eJust_left, 1, 0.6);
            }
            TransparentPolyFontTextInABox(item->highFont, text,
                item->x, item->y, item->x + item->width, item->y + item->height, eJust_left, 1,
                item->radioButton_selected ? 0.3 + 0.7 * gFrontend_throb_factor : 0.6 * gFrontend_throb_factor);
        } else if (item->enabled >= 0) {
            if (item->radioButton_selected) {
                if (text != NULL) {
                    SolidPolyFontTextInABox(item->highFont, text,
                        item->x, item->y, item->x + item->width, item->y + item->height, eJust_centre, 1);
                }
            } else {
                if (text != NULL) {
                    SolidPolyFontTextInABox(item->unlitFont, text,
                        item->x, item->y, item->x + item->width, item->y + item->height, eJust_left, 1);
                }
            }
            brender_item->prims[1].v.x = 0x800000;
            BrMaterialUpdate(brender_item->material, BR_MATU_EXTRA_PRIM);
        } else {
            if (item->map_index && brender_item->prims[1].v.x != 0x400000) {
                brender_item->prims[1].v.x = 0x400000;
                BrMaterialUpdate(brender_item->material, BR_MATU_EXTRA_PRIM);
            }
            if (text != NULL) {
                TransparentPolyFontTextInABox(item->unlitFont, text,
                    item->x, item->y, item->x + item->width, item->y + item->height, eJust_left, 1, 0.4);
            }
        }
    }
    blend_x = BR_FIXED_INT((int)(255. * (0.3 + 0.7 * gFrontend_throb_factor)));
    gFrontend_brender_items[gFrontend_selected_item_index].prims[1].v.x = blend_x;
    BrMaterialUpdate(gFrontend_brender_items[gFrontend_selected_item_index].material, BR_MATU_EXTRA_PRIM);
    if (pFrontend->items[gFrontend_selected_item_index].glowDisabled != 0) {
        for (i = 0; i < gFrontend_count_brender_items; i++) {
            if (pFrontend->items[i].glowDisabled == pFrontend->items[gFrontend_selected_item_index].glowDisabled
                    && gFrontend_brender_items[i].prims[1].v.x != blend_x) {
                gFrontend_brender_items[i].prims[1].v.x = blend_x;
                BrMaterialUpdate(gFrontend_brender_items[i].material, BR_MATU_EXTRA_PRIM);
            }
        }
    }
    FRONTEND_PingPongFlash();
    PossibleService();
    for (i = 0; i < gCount_connected_items_indices; i++) {
        pFrontend->items[gConnected_items_indices[i]].flags &= ~0x1;
    }
    gCount_connected_items_indices = 0;
#else
    __asm {
            push ebp
            mov ebp, esp
            and esp, 0xfffffff8
            sub esp, 8
            mov eax, dword ptr [ecx + 0x104]
            mov edx, dword ptr [gFrontend_leave_current_menu]
            push ebx
            push ebp
            inc eax
            push esi
            test edx, edx
            push edi
            mov dword ptr [esp + 0x10], ecx
            jne L0046e45c
            cmp eax, 1
            jle L0046e45c
            mov ebx, dword ptr [gFrontend_selected_item_index]
            dec eax
            mov dword ptr [esp + 0x14], eax
            mov eax, dword ptr [ecx + 0x11c]
            and eax, 2
            xor esi, esi
            cmp ebx, -1
            mov dword ptr [gCount_connected_items_indices], esi
            je L0046e152
            mov edi, dword ptr [gConnected_items]
            test edi, edi
            je L0046e0be
    L0046e07e:
            mov eax, dword ptr [edi + 0xc]
            xor ecx, ecx
            test eax, eax
            jle L0046e0af
            lea edx, [edi + 0x10]
    L0046e08a:
            mov eax, dword ptr [edx]
            cmp ebx, eax
            jl L0046e099
            mov ebp, dword ptr [edi + 4]
            add ebp, eax
            cmp ebx, ebp
            jl L0046e0a6
    L0046e099:
            mov eax, dword ptr [edi + 0xc]
            inc ecx
            add edx, 4
            cmp ecx, eax
            jl L0046e08a
            jmp L0046e0af
    L0046e0a6:
            mov edx, dword ptr [edi + ecx*4 + 0x10]
            mov esi, ebx
            sub esi, edx
            inc esi
    L0046e0af:
            test esi, esi
            jne L0046e0ba
            mov edi, dword ptr [edi + 0x2c]
            test edi, edi
            jne L0046e07e
    L0046e0ba:
            mov ecx, dword ptr [esp + 0x10]
    L0046e0be:
            mov eax, esi
            dec esi
            test eax, eax
            je L0046e125
            mov eax, dword ptr [edi + 0xc]
            xor ebp, ebp
            test eax, eax
            jle L0046e152
            lea edx, [edi + 0x10]
    L0046e0d5:
            mov ebx, dword ptr [edx]
            mov eax, dword ptr [gCount_connected_items_indices]
            mov ecx, esi
            add edx, 4
            add ecx, ebx
            mov dword ptr [eax*4 + gConnected_items_indices], ecx
            mov ecx, dword ptr [edx - 4]
            mov eax, esi
            add eax, ecx
            mov ecx, eax
            shl ecx, 3
            sub ecx, eax
            lea ecx, [ecx + ecx*2]
            lea eax, [eax + ecx*2]
            mov ecx, dword ptr [esp + 0x10]
            lea eax, [ecx + eax*8 + 0x188]
            mov cl, byte ptr [eax]
            or cl, 1
            mov byte ptr [eax], cl
            mov eax, dword ptr [gCount_connected_items_indices]
            inc eax
            inc ebp
            mov dword ptr [gCount_connected_items_indices], eax
            mov eax, dword ptr [edi + 0xc]
            cmp ebp, eax
            jl L0046e0d5
            jmp L0046e14c
    L0046e125:
            mov eax, ebx
            mov dword ptr [gCount_connected_items_indices], 1
            shl eax, 3
            sub eax, ebx
            mov dword ptr [gConnected_items_indices], ebx
            lea edx, [eax + eax*2]
            lea eax, [ebx + edx*2]
            lea eax, [ecx + eax*8 + 0x188]
            or byte ptr [eax], 1
    L0046e14c:
            mov ebx, dword ptr [gFrontend_selected_item_index]
    L0046e152:
            mov eax, dword ptr [gFrontend_count_brender_items]
            xor ebp, ebp
            test eax, eax
            jle L0046e30a
            mov ecx, dword ptr [esp + 0x10]
            mov ebx, offset gFrontend_brender_items+0x20
            lea esi, [ecx + 0x189]
    L0046e170:
            mov eax, dword ptr [esi - 0x2d]
            test eax, eax
            je L0046e2e6
            mov edx, dword ptr [ebx - 0x20]
            mov byte ptr [edx + 0x20], 4
            mov ecx, dword ptr [esi - 0x59]
            cmp ecx, 0x400
            jle L0046e195
            test esi, esi
            mov edi, esi
            jne L0046e19c
            jmp L0046e19c
    L0046e195:
            call IString_Get
            mov edi, eax
    L0046e19c:
            test byte ptr [esi - 1], 1
            je L0046e220
            test edi, edi
            je L0046e1d2
            mov eax, dword ptr [esi - 0x3d]
            mov edx, dword ptr [esi - 0x35]
            mov ecx, dword ptr [esi - 0x41]
            push 0x3fe33333
            push 0x33333333
            push 1
            add edx, eax
            push 0
            push edx
            mov edx, dword ptr [esi - 0x39]
            add edx, ecx
            push edx
            push eax
            push ecx
            mov ecx, dword ptr [esi - 0x49]
            mov edx, edi
            call TransparentPolyFontTextInABox
    L0046e1d2:
            mov eax, dword ptr [esi - 0x1d]
            mov ecx, dword ptr [esi - 0x41]
            fld qword ptr [gFrontend_throb_factor]
            test eax, eax
            mov eax, dword ptr [esi - 0x3d]
            je L0046e1f3
            fmul qword ptr [gDouble_0058a068+0x10]
            fsubr qword ptr [gDouble_0058a068+0x18]
            jmp L0046e1f9
    L0046e1f3:
            fmul qword ptr [gDouble_0058a068+0x20]
    L0046e1f9:
            mov edx, dword ptr [esi - 0x35]
            sub esp, 8
            add edx, eax
            _emit 0xdd
        _emit 0x1c
        _emit 0x24
            push 1
            push 0
            push edx
            mov edx, dword ptr [esi - 0x39]
            add edx, ecx
            push edx
            push eax
            push ecx
            mov ecx, dword ptr [esi - 0x45]
            mov edx, edi
            call TransparentPolyFontTextInABox
            jmp L0046e2ed
    L0046e220:
            mov eax, dword ptr [esi - 0x31]
            test eax, eax
            jge L0046e280
            mov eax, dword ptr [esi - 0x29]
            test eax, eax
            je L0046e24a
            cmp dword ptr [ebx], 0x400000
            je L0046e24a
            mov dword ptr [ebx], 0x400000
            mov eax, dword ptr [ebx - 0x18]
            push 0x40
            push eax
            call BrMaterialUpdate
            add esp, 8
    L0046e24a:
            test edi, edi
            je L0046e2ed
            mov eax, dword ptr [esi - 0x3d]
            mov edx, dword ptr [esi - 0x35]
            mov ecx, dword ptr [esi - 0x41]
            push 0x3fd99999
            push 0x9999999a
            push 1
            add edx, eax
            push 0
            push edx
            mov edx, dword ptr [esi - 0x39]
            add edx, ecx
            push edx
            push eax
            push ecx
            mov ecx, dword ptr [esi - 0x49]
            mov edx, edi
            call TransparentPolyFontTextInABox
            jmp L0046e2ed
    L0046e280:
            mov eax, dword ptr [esi - 0x1d]
            test eax, eax
            je L0046e2aa
            test edi, edi
            je L0046e2d0
            mov eax, dword ptr [esi - 0x3d]
            mov edx, dword ptr [esi - 0x35]
            mov ecx, dword ptr [esi - 0x41]
            push 1
            add edx, eax
            push 2
            push edx
            mov edx, dword ptr [esi - 0x39]
            add edx, ecx
            push edx
            push eax
            push ecx
            mov ecx, dword ptr [esi - 0x45]
            mov edx, edi
            jmp L0046e2cb
    L0046e2aa:
            test edi, edi
            je L0046e2d0
            mov eax, dword ptr [esi - 0x3d]
            mov edx, dword ptr [esi - 0x35]
            mov ecx, dword ptr [esi - 0x41]
            push 1
            add edx, eax
            push 0
            push edx
            mov edx, dword ptr [esi - 0x39]
            add edx, ecx
            push edx
            push eax
            push ecx
            mov ecx, dword ptr [esi - 0x49]
            mov edx, edi
    L0046e2cb:
            call SolidPolyFontTextInABox
    L0046e2d0:
            mov dword ptr [ebx], 0x800000
            mov eax, dword ptr [ebx - 0x18]
            push 0x40
            push eax
            call BrMaterialUpdate
            add esp, 8
            jmp L0046e2ed
    L0046e2e6:
            mov ecx, dword ptr [ebx - 0x20]
            mov byte ptr [ecx + 0x20], 1
    L0046e2ed:
            mov eax, dword ptr [gFrontend_count_brender_items]
            inc ebp
            add esi, 0x158
            add ebx, 0x2c
            cmp ebp, eax
            jl L0046e170
            mov ebx, dword ptr [gFrontend_selected_item_index]
    L0046e30a:
            fld qword ptr [gFrontend_throb_factor]
            fmul qword ptr [gDouble_0058a068+0x10]
            fsubr qword ptr [gDouble_0058a068+0x18]
            fmul qword ptr [gDouble_0058a068+0x28]
            call _ftol
            lea edx, [ebx + ebx*4]
            push 0x7fff
            shl eax, 0x10
            lea ebx, [ebx + edx*2]
            mov dword ptr [esp + 0x18], eax
            shl ebx, 2
            mov dword ptr [ebx + gFrontend_brender_items+0x20], eax
            mov eax, dword ptr [ebx + gFrontend_brender_items+0x8]
            push eax
            call BrMaterialUpdate
            mov ecx, dword ptr [gFrontend_selected_item_index]
            mov ebp, dword ptr [esp + 0x18]
            mov eax, ecx
            add esp, 8
            shl eax, 3
            sub eax, ecx
            lea edx, [eax + eax*2]
            lea eax, [ecx + edx*2]
            mov edx, dword ptr [ebp + eax*8 + 0x17c]
            test edx, edx
            je L0046e3d4
            mov eax, dword ptr [gFrontend_count_brender_items]
            xor ebx, ebx
            test eax, eax
            jle L0046e3d4
            mov esi, offset gFrontend_brender_items+0x20
            lea edi, [ebp + 0x17c]
    L0046e389:
            mov eax, ecx
            shl eax, 3
            sub eax, ecx
            lea edx, [eax + eax*2]
            lea eax, [ecx + edx*2]
            mov edx, dword ptr [edi]
            cmp edx, dword ptr [ebp + eax*8 + 0x17c]
            jne L0046e3c1
            mov eax, dword ptr [esp + 0x14]
            mov edx, dword ptr [esi]
            cmp edx, eax
            je L0046e3c1
            mov dword ptr [esi], eax
            mov eax, dword ptr [esi - 0x18]
            push 0x40
            push eax
            call BrMaterialUpdate
            mov ecx, dword ptr [gFrontend_selected_item_index]
            add esp, 8
    L0046e3c1:
            mov eax, dword ptr [gFrontend_count_brender_items]
            inc ebx
            add edi, 0x158
            add esi, 0x2c
            cmp ebx, eax
            jl L0046e389
    L0046e3d4:
            call PDGetTotalTime
            cdq
            mov ecx, 0x2ee
            idiv ecx
            cmp edx, 0x177
            mov dword ptr [esp + 0x14], edx
            jge L0046e3f3
            fild dword ptr [esp + 0x14]
            jmp L0046e402
    L0046e3f3:
            mov eax, 0x2ee
            sub eax, edx
            mov dword ptr [esp + 0x14], eax
            fild dword ptr [esp + 0x14]
    L0046e402:
            fmul qword ptr [gDouble_0058a068+0x8]
            fstp qword ptr [gFrontend_throb_factor]
            call PossibleService
            mov eax, dword ptr [gCount_connected_items_indices]
            xor edx, edx
            test eax, eax
            jle L0046e452
            mov esi, offset gConnected_items_indices
    L0046e423:
            mov ecx, dword ptr [esi]
            add esi, 4
            mov eax, ecx
            shl eax, 3
            sub eax, ecx
            lea eax, [eax + eax*2]
            lea ecx, [ecx + eax*2]
            mov bl, byte ptr [ebp + ecx*8 + 0x188]
            lea eax, [ebp + ecx*8 + 0x188]
            and bl, 0xfe
            inc edx
            mov byte ptr [eax], bl
            mov eax, dword ptr [gCount_connected_items_indices]
            cmp edx, eax
            jl L0046e423
    L0046e452:
            mov dword ptr [gCount_connected_items_indices], 0
    L0046e45c:
            pop edi
            pop esi
            pop ebp
            pop ebx
            mov esp, ebp
            pop ebp
            ret
    }
#endif
}


void C2_HOOK_FASTCALL FillInRaceDescription(char *pDest, int pRace_index) {

    strcpy(pDest, gRace_list[pRace_index].description);
    MungeMetaCharactersChar(pDest, 'R', '\r');
    MungeMetaCharactersNum(pDest, 'O', gRace_list[pRace_index].count_explicit_opponents);
    MungeMetaCharactersNum(pDest, 'L', gRace_list[pRace_index].count_laps);
}

// FUNCTION: CARMA2_HW 0x0046aa20

void C2_HOOK_FASTCALL MenuSetCarImage(int pCar_index, int pBrender_index) {
    char* car_file_name;
    char* pos_dot;
    char pack_filename[40];
    char pm_name[20];
    tPath_name pack_path;
    tTWTVFS twt;
    int i;

    car_file_name = gOpponents[pCar_index].car_file_name;
    pos_dot = strrchr(car_file_name, '.');
    PathCat(pack_path, gApplication_path, "INTRFACE");
    PathCat(pack_path, pack_path, "CARIMAGE");
    strcpy(pack_filename, gOpponents[pCar_index].car_file_name);
    pack_filename[strlen(pack_filename) - 4] = '\0';
    strcat(pack_filename, "CI");
    PathCat(pack_path, pack_path, pack_filename);
    twt = OpenPackFileAndSetTiffLoading(pack_path);
    for (i = 0; i < 6; i++) {
        sprintf(pm_name, "%.*s%c.TIF", (int)(pos_dot - car_file_name), car_file_name, 'A' + i);
        BrMapRemove(gFrontend_images[i + 1]);
        BrPixelmapFree(gFrontend_images[i + 1]);
        gFrontend_images[i + 1] = GetThisFuckingPixelmapPleaseMrTwatter(pack_path, pm_name);
        if (gFrontend_images[i + 1] == NULL) {
            gFrontend_images[i + 1] = LoadPixelmap("64by64.tif");
        }
        BrPixelmapCopy(gFrontend_brender_items[pBrender_index + i].field_0xc, gFrontend_images[i + 1]);
        BrPixelmapCopy(gFrontend_brender_items[pBrender_index + i].field_0x10, gFrontend_brender_items[pBrender_index + i].field_0xc);
        BrMapUpdate(gFrontend_brender_items[pBrender_index + i].field_0xc, BR_MAPU_ALL);
        BrMaterialUpdate(gFrontend_brender_items[pBrender_index + i].material, BR_MATU_COLOURMAP);
    }
    ClosePackFileAndSetTiffLoading(twt);
}

// FUNCTION: CARMA2_HW 0x0046b820

void C2_HOOK_FASTCALL MenuSetDriverImage(int pOpponent_index, int pFrontend_index) {
    char* abbrev_name;
    char* pos_dot;
    char pack_filename[40];
    char pm_name[20];
    tPath_name pack_path;
    tTWTVFS twt;

    abbrev_name = gOpponents[pOpponent_index].abbrev_name;
    pos_dot = strrchr(abbrev_name, '.');
    PathCat(pack_path, gApplication_path, "INTRFACE");
    PathCat(pack_path, pack_path, "CARIMAGE");
    strcpy(pack_filename, gOpponents[pOpponent_index].car_file_name);
    pack_filename[strlen(pack_filename) - 4] = '\0';
    strcat(pack_filename, "CI");
    PathCat(pack_path, pack_path, pack_filename);
    twt = OpenPackFileAndSetTiffLoading(pack_path);
    sprintf(pm_name, "%.*s%c%c.TIF", (int)(pos_dot - abbrev_name), abbrev_name, '6', '4');
    BrMapRemove(gFrontend_images[7]);
    BrPixelmapFree(gFrontend_images[7]);
    gFrontend_images[7] = GetThisFuckingPixelmapPleaseMrTwatter(pack_path, pm_name);
    if (gFrontend_images[7] == NULL) {
        gFrontend_images[7] = LoadPixelmap("64by64.tif");
    }
    BrPixelmapCopy(gFrontend_brender_items[pFrontend_index].field_0xc, gFrontend_images[7]);
    BrPixelmapCopy(gFrontend_brender_items[pFrontend_index].field_0x10, gFrontend_brender_items[pFrontend_index].field_0xc);
    BrMapUpdate(gFrontend_brender_items[pFrontend_index].field_0xc, BR_MAPU_ALL);
    ClosePackFileAndSetTiffLoading(twt);
    gFrontend_brender_items[pFrontend_index].model->vertices[0].p.v[2] = -1.0f;
    gFrontend_brender_items[pFrontend_index].model->vertices[1].p.v[2] = -1.0f;
    gFrontend_brender_items[pFrontend_index].model->vertices[2].p.v[2] = -1.0f;
    gFrontend_brender_items[pFrontend_index].model->vertices[3].p.v[2] = -1.0f;
    BrModelUpdate(gFrontend_brender_items[pFrontend_index].model, BR_MODU_VERTEX_POSITIONS);
}

// FUNCTION: CARMA2_HW 0x0046e470

int C2_HOOK_FASTCALL ProcessInputString(void) {
    int len;
    int key;
    int ch;

    if (gFrontend_current_input == NULL) {
        return 0;
    }

    len = strlen(gFrontend_current_input);
    if (len > gFrontend_maximum_input_length) {
        gFrontend_current_input[len - 1] = '\0';
        return -1;
    }
    PollKeys();
    key = PDAnyKeyDown();
    if (key == 50) {
        if (len > 0) {
            gFrontend_current_input[len - 1] = '\0';
        }
        return 1;
    }
    if (key == 63) {
        strcpy(gFrontend_current_input, gFrontend_original_player_name);
        return -2;
    }
    if (key == 51) {
        if (len == 0) {
            strcpy(gFrontend_current_input, gFrontend_original_player_name);
        }
        return -1;
    }
    ch = PDGetKeyboardCharacter();
    if (ch != 0) {
        dr_dprintf("FRONTEND: Got char %d", ch);
        ch = (tU8)PDConvertToASCIILessThan128(ch);
        dr_dprintf("FRONTEND: Char converted to %d", ch);
        if (ch > 0x1f && ch != 0x7f) {
            gFrontend_current_input[len + 0] = ch;
            gFrontend_current_input[len + 1] = '\0';
            dr_dprintf("FRONTEND: Adding char %d to string. String now '%s'", ch, gFrontend_current_input);
            return 1;
        }
        dr_dprintf("FRONTEND: Invalid Char", ch);
    }
    return 0;
}


int C2_HOOK_FASTCALL RaceIndex(const char* pName) {
    int i;

    for (i = 0; i < gNumber_of_races; i++) {

        if (DRStricmp(pName, gRace_list[i].name) == 0) {
            return i;
        }
    }
    return gNumber_of_races;
}


void C2_HOOK_FASTCALL ScrollUp(tFrontend_spec* pFrontend, int pScroller) {
    tFrontend_scroller_spec* scroller;

    scroller = &pFrontend->scrollers[pScroller];

    if (scroller->indexTopItem > scroller->indexFirstScrollableItem) {
        scroller->indexTopItem -= 1;
    }
}


void C2_HOOK_FASTCALL ScrollDn(tFrontend_spec* pFrontend, int pScroller) {
    tFrontend_scroller_spec* scroller;

    scroller = &pFrontend->scrollers[pScroller];

    if (scroller->indexTopItem + scroller->nbDisplayedAtOnce < scroller->indexFirstScrollableItem + scroller->count) {
        scroller->indexTopItem += 1;
    }
}


// FUNCTION: CARMA2_HW 0x0046bd60
int C2_HOOK_FASTCALL CreditsScreenInfunc(tFrontend_spec* pFrontend) {
    tPath_name interface_path;
    tPath_name credits_path;
    FILE* f;
    char s[100];
    char small_buf[11];
    int i;

    if (!pFrontend->loaded) {
        LoadMenuSettings(pFrontend);
    }
    FuckWithWidths(pFrontend);
    PathCat(interface_path, gApplication_path, "INTRFACE");
    PathCat(credits_path, interface_path, "creditslist.txt");
    f = DRfopen(credits_path, "rt");
    if (f == NULL) {
        FatalError(kFatalError_Mysterious_SS);
    }

    /* Number of lines */
    gCredits_line_count = GetAnInt(f);

    gCredits_texts = malloc(sizeof(char*) * gCredits_line_count);
    gCredits_fonts = malloc(sizeof(int) * gCredits_line_count);
    gCredits_heights = malloc(sizeof(int) * gCredits_line_count);
    gCredits_throbs = malloc(sizeof(int) * gCredits_line_count);

    gCredits_throbs[0] = 0;
    gCredits_fonts[0] = kPolyfont_hand_red_15pt_lit;
    gCredits_heights[0] = 0;
    gCredits_total_height = 0;
    i = 0;
    GetALineAndDontArgue(f, s);
    while (i < gCredits_line_count) {
        if (strncmp(s, "FONT", 4) == 0) {
            strncpy(small_buf, &s[5], sizeof(small_buf) - 1);
#ifdef CARPOCALYPSE2_FIX_BUGS
            small_buf[sizeof(small_buf) - 1] = '\0';
#endif
            gCredits_fonts[i] = atoi(small_buf);
        } else if (strncmp(s, "SPACE", 5) == 0) {
            strncpy(small_buf, &s[6], sizeof(small_buf) - 1);
#ifdef CARPOCALYPSE2_FIX_BUGS
            small_buf[sizeof(small_buf) - 1] = '\0';
#endif
            gCredits_heights[i] += (int)((double)PolyFontHeight(gCredits_fonts[i]) * atof(small_buf));
        } else if (strncmp(s, "THROB", 5) == 0) {
            strncpy(small_buf, &s[6], sizeof(small_buf) - 1);
#ifdef CARPOCALYPSE2_FIX_BUGS
            small_buf[sizeof(small_buf) - 1] = '\0';
#endif
            if (strncmp(small_buf, "ON", 2) == 0) {
                gCredits_throbs[i] = 1;
            } else {
                gCredits_throbs[i] = 0;
            }
        } else {
            gCredits_texts[i] = malloc(strlen(s) + 1);
            strcpy(gCredits_texts[i], s);
            gCredits_heights[i] += PolyFontHeight(gCredits_fonts[i]);
            gCredits_total_height += gCredits_heights[i];
            i++;
            if (i < gCredits_line_count) {
                gCredits_fonts[i] = gCredits_fonts[i - 1];
                gCredits_throbs[i] = gCredits_throbs[i - 1];
                gCredits_heights[i] = 0;
            }
        }
        GetALineAndDontArgue(f, s);
    }
#ifdef CARPOCALYPSE2_FIX_BUGS
    PFfclose(f);
#endif
    gCredits_total_height += 480;
    gCredits_scroll_start = PDGetTotalTime();
    StartMusicTrack(9998);
    gFrontend_selected_item_index = 0;
    return 1;
}

// FUNCTION: CARMA2_HW 0x0046c090

int C2_HOOK_FASTCALL CreditsScreenOutfunc(tFrontend_spec* pFrontend) {
    int i;

    for (i = 0; i < gCredits_line_count; i++) {
        free(gCredits_texts[i]);
    }
    free(gCredits_texts);
    return 1;
}

// FUNCTION: CARMA2_HW 0x0046f750
int C2_NAKED C2_HOOK_FASTCALL LoadGameInFunc(tFrontend_spec* pFrontend) {
#ifndef CARPOCALYPSE2_MATCHING
    br_camera* camera;
    int i;

    DefaultInfunc(pFrontend);
    ResetInterfaceTimeout();

    gFrontend_count_saved_games = StartSavedGamesList();
    gFrontend_load_game_index_top = 0;

    gFrontend_scroll_time_left = 0;
    gFrontend_scroll_time_increment = 25;
    gFrontend_scroll_last_update = PDGetTotalTime();

    gFrontend_menu_camera = BrActorAllocate(BR_ACTOR_CAMERA, NULL);
    camera = gFrontend_menu_camera->type_data;
    camera->type = BR_CAMERA_PARALLEL;
    camera->field_of_view = BR_ANGLE_DEG(90);
    camera->hither_z = 1.f;
    camera->yon_z = 3.f;
    camera->width = 640.f;
    camera->height = 480.f;

    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gFrontend_billboard_actors); i++) {
        gFrontend_billboard_actors[i] = CreateAPOactor();
    }
    gFrontend_APO_Colour_1 = FudgeBRenderIntoTheNinetiesWithSomeProperFuckingColourSupport(
        gFrontend_billboard_actors[0]->material->colour_map,
        0xff, 0x80, 0x00, 0xff);
    gFrontend_APO_Colour_2 = FudgeBRenderIntoTheNinetiesWithSomeProperFuckingColourSupport(
        gFrontend_billboard_actors[0]->material->colour_map,
        0x00, 0x00, 0x80, 0xff);
    gFrontend_APO_Colour_3 = FudgeBRenderIntoTheNinetiesWithSomeProperFuckingColourSupport(
        gFrontend_billboard_actors[0]->material->colour_map,
        0x00, 0x00, 0xff, 0xff);
    DRS3StartSound(gEffects_outlet, eSoundId_Swingin);
    return 0;
#else
    __asm {
            push ebx
            push esi
            mov esi, ecx
            xor ebx, ebx
            cmp dword ptr [esi + 0xad1c], ebx
            jne L0046f77e
            call LoadMenuSettings
            mov ecx, esi
            call FuckWithWidths
            mov esi, dword ptr [esi + 0x114]
            cmp esi, ebx
            je L0046f77e
            mov dword ptr [esi + 0xb8c0], 1
    L0046f77e:
            mov dword ptr [gFrontend_scrollbars_updated], ebx
            call EdgeTriggerModeOff
            call WaitForNoKeys
            call EdgeTriggerModeOn
            call PDGetTotalTime
            mov dword ptr [gFrontend_time_last_input], eax
            call StartSavedGamesList
            mov dword ptr [gFrontend_count_saved_games], eax
            mov dword ptr [gFrontend_load_game_index_top], ebx
            mov dword ptr [gFrontend_scroll_time_left], ebx
            mov dword ptr [gFrontend_scroll_time_increment], 0x19
            call PDGetTotalTime
            mov dword ptr [gFrontend_scroll_last_update], eax
            push ebx
            push 3
            call BrActorAllocate
            mov dword ptr [gFrontend_menu_camera], eax
            mov eax, dword ptr [eax + 0x5c]
            add esp, 8
            mov esi, offset gFrontend_billboard_actors
            mov byte ptr [eax + 4], bl
            mov word ptr [eax + 6], 0x3ffc
            mov dword ptr [eax + 8], 0x3f800000
            mov dword ptr [eax + 0xc], 0x40400000
            mov dword ptr [eax + 0x14], 0x44200000
            mov dword ptr [eax + 0x18], 0x43f00000
    L0046f804:
            call CreateAPOactor
            mov dword ptr [esi], eax
            add esi, 4
            cmp esi, offset gNet_settings
            jl L0046f804
            mov eax, dword ptr [gFrontend_billboard_actors]
            push 0xff
            push ebx
            push 0x80
            mov ecx, dword ptr [eax + 0x1c]
            mov edx, 0xff
            mov ecx, dword ptr [ecx + 0x40]
            call FudgeBRenderIntoTheNinetiesWithSomeProperFuckingColourSupport
            mov dword ptr [gFrontend_APO_Colour_1], eax
            mov eax, dword ptr [gFrontend_billboard_actors]
            push 0xff
            push 0x80
            mov ecx, dword ptr [eax + 0x1c]
            push ebx
            xor edx, edx
            mov ecx, dword ptr [ecx + 0x40]
            call FudgeBRenderIntoTheNinetiesWithSomeProperFuckingColourSupport
            mov dword ptr [gFrontend_APO_Colour_2], eax
            mov eax, dword ptr [gFrontend_billboard_actors]
            push 0xff
            push 0xff
            mov ecx, dword ptr [eax + 0x1c]
            push ebx
            xor edx, edx
            mov ecx, dword ptr [ecx + 0x40]
            call FudgeBRenderIntoTheNinetiesWithSomeProperFuckingColourSupport
            mov ecx, dword ptr [gEffects_outlet]
            mov edx, 0xbbe
            mov dword ptr [gFrontend_APO_Colour_3], eax
            call DRS3StartSound
            xor eax, eax
            pop esi
            pop ebx
            ret
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0046fb80
int C2_HOOK_FASTCALL LoadGameScrollDown(tFrontend_spec* pFrontend) {
    if (gFrontend_load_game_index_top + 8 < gFrontend_count_saved_games && gFrontend_scroll_time_left == 0) {
        gFrontend_load_game_index_top += 1;
        DRS3StartSound(gEffects_outlet, eSoundId_LeftButton);
        gFrontend_scroll_time_left += gFrontend_scroll_time_increment;
    }
    return 0;
}


// FUNCTION: CARMA2_HW 0x004729d0

void C2_HOOK_FASTCALL Slider_XProc(tFrontend_slider* pSlider) {
    SetJoystickX((pSlider->field_0x18 - pSlider->field_0x14) * pSlider->value);
}

// FUNCTION: CARMA2_HW 0x004729f0

void C2_HOOK_FASTCALL Slider_YProc(tFrontend_slider* pSlider) {
    SetJoystickY((pSlider->field_0x18 - pSlider->field_0x14) * pSlider->value);
}

// FUNCTION: CARMA2_HW 0x00472a10

void C2_HOOK_FASTCALL Slider_ForceProc(tFrontend_slider* pSlider) {

    SetJoystickFFBGain((int)(pSlider->value * 100.0));
}

// FUNCTION: CARMA2_HW 0x004725f0

int C2_NAKED C2_HOOK_FASTCALL Controls_Infunc(tFrontend_spec* pFrontend) {
#ifndef CARPOCALYPSE2_MATCHING

    Generic_Infunc(pFrontend);
    LoadKeyNames();
    gFrontend_controls_count_keys = 29;
    BackupKeyMappings();
    Joystick_BackupSettings();

    gJoystick_index = 0;

    SelectThisItemIn(pFrontend, 1, 39 + gKey_map_index);

    gControls_scroller.field_0x0 = gFrontend_controls_count_keys;
    gControls_scroller.range_length = 10;
    gControls_scroller.field_0x8 = 0;
    gControls_scroller.count_ranges = 2;
    gControls_scroller.range_starts[0] = 43;
    gControls_scroller.range_starts[1] = 53;
    gControls_scroller.next = NULL;
    gConnected_items = &gControls_scroller;

    gControls_slider_1.flags = 0;
    gControls_slider_1.itemid_left_reference = 14;
    gControls_slider_1.itemid_start = 1;
    gControls_slider_1.width = 236;
    gControls_slider_1.callback = Slider_XProc;
    gControls_slider_1.field_0x14 = 0.f;
    gControls_slider_1.field_0x18 = 2.f;
    gControls_slider_1.next = &gControls_slider_2;

    gControls_slider_2.flags = 0;
    gControls_slider_2.itemid_left_reference = 19;
    gControls_slider_2.itemid_start = 4;
    gControls_slider_2.width = 236;
    gControls_slider_2.callback = Slider_YProc;
    gControls_slider_2.field_0x14 = 0.f;
    gControls_slider_2.field_0x18 = 2.f;
    gControls_slider_2.next = &gControls_slider_3;

    gControls_slider_3.flags = 0;
    gControls_slider_3.itemid_left_reference = 23;
    gControls_slider_3.itemid_start = 7;
    gControls_slider_3.width = 236;
    gControls_slider_3.callback = Slider_ForceProc;
    gControls_slider_3.field_0x14 = 0.f;
    gControls_slider_3.field_0x18 = 1.f;
    gControls_slider_3.next = NULL;

    DisplayJoystickSettings(pFrontend);
    RefreshScrollSet(pFrontend);
    return 1;
#else
    __asm {
            sub esp, 0x208
            push ebx
            push ebp
            push esi
            mov esi, ecx
            push edi
            xor edi, edi
            mov eax, dword ptr [esi + 0xad1c]
            mov dword ptr [esp + 0x14], esi
            cmp eax, edi
            jne L0047262c
            call LoadMenuSettings
            mov ecx, esi
            call FuckWithWidths
            mov esi, dword ptr [esi + 0x114]
            cmp esi, edi
            je L0047262c
            mov dword ptr [esi + 0xb8c0], 1
    L0047262c:
            mov dword ptr [gFrontend_scrollbars_updated], edi
            call EdgeTriggerModeOff
            call WaitForNoKeys
            call EdgeTriggerModeOn
            mov edx, offset gApplication_path
            lea ecx, [esp + 0x118]
            push offset gFrontend_QUIT+0xbcd8
            mov dword ptr [gCurrent_frontend_scrollbars], edi
            mov dword ptr [gConnected_items], edi
            mov dword ptr [gPTR_00686508], edi
            call PathCat
            mov edx, offset gNext_crush_actor_id+0x180
            lea ecx, [esp + 0x118]
            call DRfopen
            mov ebx, eax
            cmp ebx, edi
            mov dword ptr [esp + 0x10], ebx
            jne L0047268e
            push 0x6a
            call FatalError
            add esp, 4
    L0047268e:
            mov ebp, offset gKey_names
    L00472693:
            push ebx
            mov edx, 0x100
            lea ecx, [esp + 0x1c]
            call PFfgets
            lea edi, [esp + 0x18]
            or ecx, 0xffffffff
            xor eax, eax
            xor edx, edx
            repne scasb
            not ecx
            dec ecx
            mov eax, ecx
            test eax, eax
            jle L004726e4
    L004726b8:
            mov cl, byte ptr [esp + edx + 0x18]
            lea edi, [esp + edx + 0x18]
            cmp cl, 0x20
            jae L004726db
            mov ecx, eax
            lea esi, [edi + 1]
            sub ecx, edx
            mov ebx, ecx
            shr ecx, 2
            rep movsd
            mov ecx, ebx
            and ecx, 3
            dec eax
            rep movsb
    L004726db:
            inc edx
            cmp edx, eax
            jl L004726b8
            mov ebx, dword ptr [esp + 0x10]
    L004726e4:
            lea edi, [esp + 0x18]
            or ecx, 0xffffffff
            xor eax, eax
            push 0xfd
            repne scasb
            not ecx
            push ecx
            call BrMemAllocate
            mov edx, eax
            lea edi, [esp + 0x20]
            or ecx, 0xffffffff
            xor eax, eax
            add esp, 8
            mov dword ptr [ebp], edx
            repne scasb
            not ecx
            sub edi, ecx
            add ebp, 4
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            shr ecx, 2
            rep movsd
            mov ecx, eax
            and ecx, 3
            cmp ebp, offset gFrontend_C_model_to
            rep movsb
            jl L00472693
            mov ecx, ebx
            call PFfclose
            mov ecx, dword ptr [gKey_map_index]
            lea eax, [esp + 0x18]
            xor ebx, ebx
            sub eax, 5
            mov dword ptr [gFrontend_controls_count_keys], 0x1d
            mov dword ptr [gOrig_key_map_index], ecx
            mov dword ptr [esp + 0x10], eax
            mov ebp, offset gBackup_key_mappings
    L00472763:
            push offset gFrontend_QUIT+0xbcc8
            mov edx, offset gApplication_path
            lea ecx, [esp + 0x1c]
            mov dword ptr [gKey_map_index], ebx
            call PathCat
            mov dl, byte ptr [gKey_map_index]
            lea edi, [esp + 0x18]
            or ecx, 0xffffffff
            xor eax, eax
            add dl, 0x30
            repne scasb
            mov eax, dword ptr [esp + 0x10]
            not ecx
            dec ecx
            mov byte ptr [ecx + eax], dl
            mov edx, offset gNext_crush_actor_id+0x180
            lea ecx, [esp + 0x18]
            call DRfopen
            mov esi, eax
            test esi, esi
            jne L004727b8
            push 9
            call FatalError
            add esp, 4
    L004727b8:
            mov edi, offset gKey_mapping
    L004727bd:
            push edi
            push offset gPed_flags_0058f2e4+0x134
            push esi
            call fscanf
            add edi, 4
            add esp, 0xc
            cmp edi, offset gKey_mapping+0x134
            jl L004727bd
            mov ecx, esi
            call PFfclose
            mov edi, ebp
            add ebp, 0x134
            mov ecx, 0x4d
            mov esi, offset gKey_mapping
            inc ebx
            cmp ebp, offset gBackup_key_mappings+0x4d0
            rep movsd
            jl L00472763
            mov eax, dword ptr [gOrig_key_map_index]
            mov edi, offset gKey_mapping
            mov dword ptr [gKey_map_index], eax
            lea ecx, [eax + eax*8]
            lea edx, [eax + ecx*2]
            mov ecx, 0x4d
            lea eax, [eax + edx*4]
            lea esi, [eax*4 + gBackup_key_mappings]
            rep movsd
            mov ecx, dword ptr [gJoystick_index]
            xor edi, edi
            mov dword ptr [gOrig_joystick_index], ecx
            mov dword ptr [gJoystick_index], edi
            call GetJoystickX
            fstp dword ptr [gOrig_joystick_index+0x4]
            call GetJoystickY
            fstp dword ptr [gOrig_joystick_index+0x8]
            call GetJoystickFBBGain
            mov dword ptr [gOrig_joystick_index+0xc], eax
            call PDIsJoystickDPadEnabled
            mov edx, dword ptr [gOrig_joystick_index]
            mov esi, dword ptr [esp + 0x14]
            mov dword ptr [gOrig_joystick_index+0x10], eax
            mov eax, dword ptr [gKey_map_index]
            add eax, 0x27
            mov dword ptr [gJoystick_index], edx
            push eax
            mov edx, 1
            mov ecx, esi
            call SelectThisItemIn
            mov ecx, dword ptr [gFrontend_controls_count_keys]
            mov eax, 0xec
            mov dword ptr [gControls_scroller], ecx
            mov ecx, esi
            mov dword ptr [gControls_scroller+0x4], 0xa
            mov dword ptr [gControls_scroller+0x8], edi
            mov dword ptr [gControls_scroller+0xc], 2
            mov dword ptr [gControls_scroller+0x10], 0x2b
            mov dword ptr [gControls_scroller+0x14], 0x35
            mov dword ptr [gControls_scroller+0x2c], edi
            mov dword ptr [gConnected_items], 0x688408
            mov byte ptr [gControls_slider_1], 0
            mov dword ptr [gControls_slider_1+0x4], 0xe
            mov dword ptr [gControls_slider_1+0x8], 1
            mov dword ptr [gControls_slider_1+0xc], eax
            mov dword ptr [gControls_slider_1+0x1c], 0x4729d0
            mov dword ptr [gControls_slider_1+0x14], edi
            mov dword ptr [gControls_slider_1+0x18], 0x40000000
            mov byte ptr [gControls_slider_2], 0
            mov dword ptr [gControls_slider_2+0x4], 0x13
            mov dword ptr [gControls_slider_2+0x8], 4
            mov dword ptr [gControls_slider_2+0xc], eax
            mov dword ptr [gControls_slider_2+0x1c], 0x4729f0
            mov dword ptr [gControls_slider_2+0x14], edi
            mov dword ptr [gControls_slider_2+0x18], 0x40000000
            mov byte ptr [gControls_slider_3], 0
            mov dword ptr [gControls_slider_3+0x4], 0x17
            mov dword ptr [gControls_slider_3+0x8], 7
            mov dword ptr [gControls_slider_3+0xc], eax
            mov dword ptr [gControls_slider_3+0x1c], 0x472a10
            mov dword ptr [gControls_slider_3+0x14], edi
            mov dword ptr [gControls_slider_3+0x18], 0x3f800000
            mov dword ptr [gControls_slider_1+0x20], 0x6883c8
            mov dword ptr [gControls_slider_2+0x20], 0x688ac0
            mov dword ptr [gControls_slider_3+0x20], edi
            mov dword ptr [gCurrent_frontend_scrollbars], 0x686f68
            call DisplayJoystickSettings
            mov ecx, esi
            call RefreshScrollSet
            mov eax, 1
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x208
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00472a30

int C2_HOOK_FASTCALL Controls_Outfunc(tFrontend_spec* pFrontend) {
    int i;

    Generic_Outfunc(pFrontend);
    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gKey_names); i++) {
        BrMemFree(gKey_names[i]);
    }
    SaveKeyMapping();
    return 1;
}

// FUNCTION: CARMA2_HW 0x00472400

int C2_HOOK_FASTCALL Controls_JoystickDpadToggle(tFrontend_spec* pFrontend) {
    if (gFrontend_selected_item_index == 36) {
        SetJoystickDPadEnabled(1);
    } else {
        SetJoystickDPadEnabled(0);
    }
    DisplayJoystickSettings(pFrontend);
    return 0;
}


// FUNCTION: CARMA2_HW 0x00472440

int C2_HOOK_FASTCALL Controls_SwitchKeymapSet(tFrontend_spec* pFrontend) {

    ChangeKeyMapIndex(gFrontend_selected_item_index - 39);
    SelectThisItemIn(pFrontend, 2, gKey_map_index + 39);
    RefreshScrollSet(pFrontend);
    FuckWithWidths(pFrontend);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00472b00

int C2_NAKED C2_HOOK_FASTCALL Controls_SlotActivated(tFrontend_spec* pFrontend) {
#ifndef CARPOCALYPSE2_MATCHING
    int key_array_index;
    float dy;
    br_model* model;
    int match;

    key_array_index = DetermineKeyArrayIndex();
    CyclePollKeys();
    PollKeys();
    WaitForNoKeys();
    pFrontend->items[0].visible = 1;
    model = gFrontend_brender_items[0].model;
    dy = (float)(17 * (key_array_index - gControls_scroller.field_0x8));
    model->vertices[0].p.v[1] = model->vertices[3].p.v[1] = -(pFrontend->items[0].y + dy);
    model->vertices[1].p.v[1] = model->vertices[2].p.v[1] = -(pFrontend->items[0].y + dy + 20);
    BrModelUpdate(model, BR_MODU_VERTEX_POSITIONS);
    match = -1;
    for (;;) {
        int key;

        FrontendRedrawMenu();

        key = PDAnyKeyDown();
        if (key != -1 && key != 63) {
            int i;

            for (i = 28; i < CARPOCALYPSE2_ASIZE(gKey_mapping); i++) {

                if (gKey_mapping[i] == key && i != gControls_frontend_to_key_mapping_lut[key_array_index]) {
                    int j;

                    for (j = 0; j < gFrontend_controls_count_keys; j++) {

                        if (i == gControls_frontend_to_key_mapping_lut[j]) {
                            gKey_mapping[i] = -2;
                            match = j;
                            break;
                        }
                    }
                    if (match < 0) {
                        DRS3StartSound(gEffects_outlet, eSoundId_CantAffordPart);
                        key = -1;
                        break;
                    }
                }
            }
        }
        CyclePollKeys();
        PollKeys();
        ServiceGame();
        if (key != -1 || EitherMouseButtonDown()) {
            pFrontend->items[0].visible = 0;
            DRS3StartSound(gEffects_outlet, eSoundId_Done);
            WaitForNoKeys();
            if (key != 63 && key != -1) {
                gKey_mapping[gControls_frontend_to_key_mapping_lut[key_array_index]] = key;
            }
            RefreshScrollSet(pFrontend);
            return 0;
        }
    }
#else
    __asm {
            sub esp, 0xc
            mov eax, dword ptr [gFrontend_selected_item_index]
            push ebx
            push ebp
            push esi
            mov esi, ecx
            cmp eax, 0x35
            push edi
            mov dword ptr [esp + 0x18], esi
            jl L00472b1a
            add eax, -0xa
    L00472b1a:
            mov ecx, dword ptr [gControls_scroller+0x8]
            lea edi, [ecx + eax - 0x2b]
            mov dword ptr [esp + 0x10], edi
            call CyclePollKeys
            call PollKeys
            call WaitForNoKeys
            mov dword ptr [esi + 0x15c], 1
            mov eax, edi
            mov edi, dword ptr [gControls_scroller+0x8]
            mov ecx, dword ptr [gFrontend_brender_items+0x4]
            sub eax, edi
            mov edx, ecx
            mov edi, eax
            push 1
            shl edi, 4
            add edi, eax
            lea eax, [ecx + 8]
            mov dword ptr [esp + 0x18], edi
            push edx
            fild dword ptr [esp + 0x1c]
            fild dword ptr [esi + 0x14c]
            mov ecx, dword ptr [eax]
            fadd st(0), st(1)
            fchs
            fstp dword ptr [ecx + 0x7c]
            mov ecx, dword ptr [eax]
            mov edi, dword ptr [ecx + 0x7c]
            mov dword ptr [ecx + 4], edi
            mov ecx, dword ptr [eax]
            fild dword ptr [esi + 0x14c]
            fadd st(0), st(1)
            fsub dword ptr [gDouble_0058a068+0x98]
            fchs
            fstp dword ptr [ecx + 0x54]
            mov eax, dword ptr [eax]
            fstp st(0)
            mov ecx, dword ptr [eax + 0x54]
            mov dword ptr [eax + 0x2c], ecx
            call BrModelUpdate
            add esp, 8
            or ebx, 0xffffffff
    L00472ba9:
            mov edx, dword ptr [gBack_screen]
            xor esi, esi
            mov word ptr [edx + 0x38], si
            mov eax, dword ptr [gBack_screen]
            mov word ptr [eax + 0x3a], si
            mov eax, dword ptr [gFrontend_backdrop]
            cmp eax, esi
            je L00472be2
            mov cx, word ptr [eax + 0x36]
            mov dx, word ptr [eax + 0x34]
            push ecx
            mov ecx, dword ptr [gBack_screen]
            push edx
            push esi
            push esi
            push eax
            push esi
            xor edx, edx
            call FRONTEND_Redraw
    L00472be2:
            mov eax, dword ptr [gDepth_buffer]
            push -1
            push eax
            call BrPixelmapFill
            mov eax, dword ptr [gFrontend_count_brender_items]
            add esp, 8
            xor ecx, ecx
            cmp eax, esi
            jle L00472c35
            xor edx, edx
            mov eax, offset gFrontend_brender_items
    L00472c04:
            mov edi, dword ptr [gCurrent_frontend_spec]
            mov ebp, dword ptr [edi + edx + 0x15c]
            mov edi, dword ptr [eax]
            cmp ebp, esi
            je L00472c1d
            mov byte ptr [edi + 0x20], 4
            jmp L00472c21
    L00472c1d:
            mov byte ptr [edi + 0x20], 1
    L00472c21:
            mov edi, dword ptr [gFrontend_count_brender_items]
            inc ecx
            add edx, 0x158
            add eax, 0x2c
            cmp ecx, edi
            jl L00472c04
    L00472c35:
            mov ecx, dword ptr [gDepth_buffer]
            mov edx, dword ptr [gBack_screen]
            mov eax, dword ptr [gFrontend_camera]
            push ecx
            mov ecx, dword ptr [gFrontend_actor]
            push edx
            push eax
            push ecx
            call BrZbsSceneRender
            mov ecx, dword ptr [gCurrent_frontend_spec]
            add esp, 0x10
            call FRONTEND_DrawMenu
            cmp dword ptr [gFrontend_suppress_mouse], esi
            jne L00472c84
            mov ecx, 1
            call PossibleLock
            call DoMouseCursor
            mov ecx, 1
            call PossibleUnlock
    L00472c84:
            xor ecx, ecx
            call PDScreenBufferSwap
            call PDAnyKeyDown
            mov ebp, eax
            cmp ebp, -1
            je L00472d03
            cmp ebp, 0x3f
            je L00472d03
            mov edi, dword ptr [gFrontend_controls_count_keys]
            mov esi, 0x1c
            mov edx, offset gKey_mapping+0x70
    L00472cac:
            cmp dword ptr [edx], ebp
            jne L00472ce2
            mov eax, dword ptr [esp + 0x10]
            cmp esi, dword ptr [eax*4 + gControls_frontend_to_key_mapping_lut]
            je L00472ce2
            xor eax, eax
            test edi, edi
            jle L00472cde
            mov ecx, offset gControls_frontend_to_key_mapping_lut
    L00472cc8:
            cmp dword ptr [ecx], esi
            je L00472cd6
            inc eax
            add ecx, 4
            cmp eax, edi
            jl L00472cc8
            jmp L00472cde
    L00472cd6:
            mov dword ptr [edx], 0xfffffffe
            mov ebx, eax
    L00472cde:
            test ebx, ebx
            jl L00472cf0
    L00472ce2:
            add edx, 4
            inc esi
            cmp edx, offset gKey_mapping+0x130
            jle L00472cac
            jmp L00472d03
    L00472cf0:
            mov ecx, dword ptr [gEffects_outlet]
            mov edx, 0xc1c
            call DRS3StartSound
            or ebp, 0xffffffff
    L00472d03:
            call CyclePollKeys
            call PollKeys
            call ServiceGame
            cmp ebp, -1
            jne L00472d24
            call EitherMouseButtonDown
            test eax, eax
            je L00472ba9
    L00472d24:
            mov esi, dword ptr [esp + 0x18]
            mov edx, 0xbbc
            mov dword ptr [esi + 0x15c], 0
            mov ecx, dword ptr [gEffects_outlet]
            call DRS3StartSound
            call WaitForNoKeys
            cmp ebp, 0x3f
            je L00472d63
            cmp ebp, -1
            je L00472d63
            mov ecx, dword ptr [esp + 0x10]
            mov edx, dword ptr [ecx*4 + gControls_frontend_to_key_mapping_lut]
            mov dword ptr [edx*4 + gKey_mapping], ebp
    L00472d63:
            mov ecx, esi
            call RefreshScrollSet
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0xc
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004725a0

int C2_HOOK_FASTCALL Controls_KeyUp(tFrontend_spec* pFrontend) {
    if (gControls_scroller.field_0x8 != 0) {
        gControls_scroller.field_0x8 -= 1;
        RefreshScrollSet(pFrontend);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004725c0

int C2_HOOK_FASTCALL Controls_KeyDown(tFrontend_spec* pFrontend) {
    if (gControls_scroller.field_0x8 < gControls_scroller.field_0x0 - gControls_scroller.range_length) {
        gControls_scroller.field_0x8 += 1;
        RefreshScrollSet(pFrontend);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00472d80

int C2_NAKED C2_HOOK_FASTCALL Controls_Ok(tFrontend_spec* pFrontend) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;
    int prev_item;

    gINT_00688748 = -1;
    WaitForNoKeys();

    for (i = 28; i <= (int)CARPOCALYPSE2_ASIZE(gKey_mapping) - 1; i++) {
        if (gKey_mapping[i] == -2) {
            gINT_00688748 = i;
            break;
        }
    }
    if (gJoystick_index == -1) {
        for (i = 28; i <= (int)CARPOCALYPSE2_ASIZE(gKey_mapping) - 1; i++) {
            if (gKey_mapping[i] >= 107 && gKey_mapping[i] < 139) {
                gINT_00688748 = i;
            } else if (gKey_mapping[i] >= 139 && gKey_mapping[i] < 143) {
                gINT_00688748 = i;
            }
        }
    }
    if (gINT_00688748 != -1) {
        for (i = 0; i < gFrontend_controls_count_keys; i++) {
            if (gControls_frontend_to_key_mapping_lut[i] == gINT_00688748) {
                break;
            }
        }
        ScrollSet_DisplayEntry(&gControls_scroller, i);
        RefreshScrollSet(pFrontend);
        prev_item = gFrontend_selected_item_index;
        if (i < gControls_scroller.field_0x8 || i > gControls_scroller.field_0x8 + gControls_scroller.range_length) {
            gFrontend_selected_item_index = -1;
        } else {
            i -= gControls_scroller.field_0x8;
            i += gControls_scroller.range_starts[0];
            gFrontend_selected_item_index = i;
        }
        if (gFrontend_selected_item_index == -1) {
            PDFatalError("Screwed up in Controls_Ok().");
            return 0;
        }
        Controls_SlotActivated(pFrontend);
        gFrontend_selected_item_index = prev_item;
        return 4;
    }
    SaveAllJoystickData();
    return 0;
#else
    __asm {
            push ebx
            push esi
            push edi
            mov ebx, ecx
            mov dword ptr [gINT_00688748], 0xffffffff
            call WaitForNoKeys
            mov ecx, 0x1c
            mov eax, offset gKey_mapping+0x70
    L00472d9e:
            cmp dword ptr [eax], -2
            je L00472db6
            add eax, 4
            inc ecx
            cmp eax, offset gKey_mapping+0x130
            jle L00472d9e
            mov edi, dword ptr [gINT_00688748]
            jmp L00472dbe
    L00472db6:
            mov edi, ecx
            mov dword ptr [gINT_00688748], edi
    L00472dbe:
            cmp dword ptr [gJoystick_index], -1
            jne L00472e03
            mov edx, 0x1c
            mov ecx, offset gKey_mapping+0x70
    L00472dd1:
            mov eax, dword ptr [ecx]
            cmp eax, 0x6b
            jl L00472de1
            cmp eax, 0x8b
            jge L00472de8
            jmp L00472def
    L00472de1:
            cmp eax, 0x8b
            jl L00472df1
    L00472de8:
            cmp eax, 0x8f
            jge L00472df1
    L00472def:
            mov edi, edx
    L00472df1:
            add ecx, 4
            inc edx
            cmp ecx, offset gKey_mapping+0x130
            jle L00472dd1
            mov dword ptr [gINT_00688748], edi
    L00472e03:
            cmp edi, -1
            je L00472eb6
            mov ecx, dword ptr [gFrontend_controls_count_keys]
            xor esi, esi
            test ecx, ecx
            jle L00472e29
            mov eax, offset gControls_frontend_to_key_mapping_lut
    L00472e1d:
            cmp dword ptr [eax], edi
            je L00472e29
            inc esi
            add eax, 4
            cmp esi, ecx
            jl L00472e1d
    L00472e29:
            mov eax, dword ptr [gControls_scroller+0x8]
            mov ecx, dword ptr [gControls_scroller+0x4]
            cmp esi, eax
            jl L00472e3e
            add eax, ecx
            cmp esi, eax
            jl L00472e54
    L00472e3e:
            mov eax, dword ptr [gControls_scroller]
            mov dword ptr [gControls_scroller+0x8], esi
            sub eax, ecx
            cmp esi, eax
            jl L00472e54
            mov dword ptr [gControls_scroller+0x8], eax
    L00472e54:
            mov ecx, ebx
            call RefreshScrollSet
            mov eax, dword ptr [gControls_scroller+0x8]
            mov edi, dword ptr [gFrontend_selected_item_index]
            cmp esi, eax
            jl L00472e82
            mov ecx, dword ptr [gControls_scroller+0x4]
            lea edx, [eax + ecx]
            cmp esi, edx
            jg L00472e82
            sub esi, eax
            mov eax, dword ptr [gControls_scroller+0x10]
            add esi, eax
            jmp L00472e85
    L00472e82:
            or esi, 0xffffffff
    L00472e85:
            cmp esi, -1
            mov dword ptr [gFrontend_selected_item_index], esi
            jne L00472ea0
            mov ecx, offset gFrontend_QUIT+0xbce8
            call PDFatalError
            xor eax, eax
            pop edi
            pop esi
            pop ebx
            ret
    L00472ea0:
            mov ecx, ebx
            call Controls_SlotActivated
            mov dword ptr [gFrontend_selected_item_index], edi
            mov eax, 4
            pop edi
            pop esi
            pop ebx
            ret
    L00472eb6:
            call SaveAllJoystickData
            xor eax, eax
            pop edi
            pop esi
            pop ebx
            ret
    }
#endif
}


// FUNCTION: CARMA2_HW 0x00487e10
void C2_HOOK_FASTCALL LoadKeyMapping(void) {
    FILE* f;
    tPath_name the_path;
    int i;

    PathCat(the_path, gApplication_path, "KEYMAP_X.TXT");
    the_path[strlen(the_path) - 5] = '0' + gKey_map_index;
    f = DRfopen(the_path, "rt");
    if (f == NULL) {
        FatalError(kFatalError_CouldNotOpenKeyMapFile);
    }

    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gKey_mapping); i++) {
        fscanf((FILE*)f, "%d", &gKey_mapping[i]);
    }
    PFfclose(f);
}

void C2_HOOK_FASTCALL SaveKeyMapping(void) {
    FILE* f;
    tPath_name the_path;
    int i;

    PathCat(the_path, gApplication_path, "KEYMAP_X.TXT");
    the_path[strlen(the_path) - 5] = '0' + gKey_map_index;
    PDFileUnlock(the_path);
    f = DRfopen(the_path, "wb");
    if (f == NULL) {
        FatalError(kFatalError_CouldNotOpenKeyMapFile);
    }
    for (i = 0; i < CARPOCALYPSE2_ASIZE(gKey_mapping); i++) {
        fprintf(f, "%d", gKey_mapping[i]);
        fputc('\r', f);
        fputc('\n', f);
    }
    PFfclose(f);
}

void C2_HOOK_FASTCALL ChangeKeyMapIndex(int pKey_map_index) {

    SaveKeyMapping();
    gKey_map_index = pKey_map_index;
    LoadKeyMapping();
}

// FUNCTION: CARMA2_HW 0x0046ad00
int C2_HOOK_FASTCALL InterfaceMenuInfunc(tFrontend_spec* pFrontend) {
    if (!pFrontend->loaded) {
        LoadMenuSettings(pFrontend);
        FuckWithWidths(pFrontend);
        if (pFrontend->previous != NULL) {
            pFrontend->previous->isPreviousSomeOtherMenu = 1;
        }
    }
    gFrontend_scrollbars_updated = 0;
    EdgeTriggerModeOff();
    WaitForNoKeys();
    EdgeTriggerModeOn();
    if (gProgram_state.racing != 0) {
        pFrontend->items[0].enabled = kFrontendItemEnabled_enabled;
        pFrontend->items[1].enabled = kFrontendItemEnabled_enabled;
        if (gNet_mode == eNet_mode_none) {
            pFrontend->items[5].enabled = kFrontendItemEnabled_disabled;
        } else {
            pFrontend->items[5].enabled = kFrontendItemEnabled_enabled;
        }
        return 1;
    }
    pFrontend->items[0].enabled = kFrontendItemEnabled_disabled;
    pFrontend->items[1].enabled = kFrontendItemEnabled_disabled;
    return 1;
}
// FUNCTION: CARMA2_HW 0x0046ada0
int C2_HOOK_FASTCALL InterfaceReturnTrue(void) {
    return 1;
}
