#include "72-interface.h"

#include "05-drmem.h"
#include "08-loading1.h"
#include "10-loading2.h"
#include "15-displays.h"
#include "18-graphics2.h"
#include "19-font.h"
#include "24-loadsave.h"
#include "35-intrface.h"
#include "40-main.h"
#include "41-utility.h"
#include "42-input.h"
#include "52-errors.h"
#include "69-sound.h"
#include "70-packfile.h"
#include "72-interface-main.h"
#include "72-interface-network.h"
#include "01-network.h"
#include "10-loading2.h"
#include "72-interface-quit.h"
#include "72-interface-options.h"
#include "72-interface-newgame.h"
#include "72-interface-wrecks.h"
#include "72-interface-netsync.h"
#include "72-interface-networksummary.h"
#include "72-interface-startgame.h"
#include "63-loading3.h"
#include "66-tintedpoly.h"
#include "joystick.h"
#include "globvars.h"
#include "platform.h"
#include "carpocalypse2_macros.h"
#include "carpocalypse2_types.h"

#include "c2_string.h"
#include <ctype.h>

extern void C2_HOOK_FASTCALL DisplayJoystickSettings(tFrontend_spec *pFrontend);

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
extern int C2_HOOK_FASTCALL NetInitialise(void);
extern void C2_HOOK_FASTCALL AboutToLoadFirstCar(void);
extern int gFrontend_controls_indices[15];
extern void C2_HOOK_FASTCALL CheckForCheatingGits(void);

#define COUNT_FRONTEND_INTERPOLATE_STEPS 16

/* ==== forward declarations for the frontend flow ==== */
extern void C2_HOOK_FASTCALL FRONTEND_DrawMenu(tFrontend_spec* pFrontend);
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
extern void C2_HOOK_FASTCALL PrepareSliders(tFrontend_spec* pFrontend);
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
extern float C2_HOOK_FASTCALL GetJoystickFBBGain(void);
extern void C2_HOOK_FASTCALL SetJoystickX(float v);
extern void C2_HOOK_FASTCALL SetJoystickY(float v);
extern void C2_HOOK_FASTCALL SetJoystickFFBGain(float v);

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

// FUNCTION: CARMA2_HW 0x00466450
int C2_HOOK_FASTCALL temp(tFrontend_spec* pFrontend) {

    return 0;
}

// int C2_HOOK_FASTCALL GetItemAtMousePos(tFrontend_spec *pFrontend, int pX, int pY); // Moved for inlining purposes

// FUNCTION: CARMA2_HW 0x00466ce0
void C2_HOOK_FASTCALL MungeButtonModels(tFrontend_spec* pFrontend, int pButton_index) {
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
void C2_HOOK_FASTCALL ToggleSelection(tFrontend_spec* pFrontend) {
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


// FUNCTION: CARMA2_HW 0x00467b30
void C2_HOOK_FASTCALL RefreshRacesScroller(tFrontend_spec* pFrontend) {
    char group_text[12];
    int group;
    int i;

    group = (gCurrent_race_group - gRace_groups) % 10;
    sprintf(group_text, "%s %d", IString_Get(78), group + 1);
    strcpy(pFrontend->items[2].text, group_text);

    for (i = pFrontend->scrollers[0].indexFirstScrollableItem; i <= pFrontend->scrollers[0].indexLastScrollableItem; i++) {
        tFrontend_item_spec* item = &pFrontend->items[i];
        int race_i = 4 * group + i;

        strcpy(item->text, gRace_list[race_i].name);
        if (race_i == gProgram_state.current_race_index) {
            item->radioButton_selected = 1;
        } else {
            item->radioButton_selected = 0;

        }
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

// StopAllThatJoinyStuffThisInstant

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
        BrPixelmapFill(pFrontend_brender_item->field_0x10, BR_COLOUR_RGBA(0, 0, 0, 0x80));
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
    tS8 i;

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

// FRONTEND_Redraw

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

void C2_HOOK_FASTCALL FRONTEND_Setup2D(void) {
    br_camera* camera;

    gFrontend_actor = BrActorAllocate(BR_ACTOR_NONE, NULL);
    gFrontend_camera = BrActorAllocate(BR_ACTOR_CAMERA, NULL);
    camera = gFrontend_camera->type_data;
    camera->type = BR_CAMERA_PARALLEL;
    camera->field_of_view = BrDegreeToAngle(45); /* 0x1ffe */
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
void C2_HOOK_FASTCALL ScrollCredits(void) {
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
int C2_HOOK_FASTCALL LoadGameOutFunc(tFrontend_spec* pFrontend) {
    int i;

    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gFrontend_billboard_actors); i++) {
        KillAPOactor(gFrontend_billboard_actors[i]);
    }
    BrActorFree(gFrontend_menu_camera);
    gFrontend_menu_camera = NULL;
    EndSavedGamesList();
    DRS3StartSound(gEffects_outlet, eSoundId_Swingout);
    return 0;
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

// FUNCTION: CARMA2_HW 0x0046fb80
int C2_HOOK_FASTCALL LoadGameScrollDownLoadGameScrollDown(tFrontend_spec* pFrontend) {

    if (gFrontend_load_game_index_top + 8 < gFrontend_count_saved_games && gFrontend_scroll_time_left == 0) {
        gFrontend_load_game_index_top += 1;
        DRS3StartSound(gEffects_outlet, eSoundId_LeftButton);
        gFrontend_scroll_time_left += gFrontend_scroll_time_increment;
    }
    return 0;
}

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
int C2_HOOK_FASTCALL LoadGameUpdateFunc(tFrontend_spec* pFrontend) {
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

// GLOBAL: CARMA2_HW 0x0068c230
extern char* gInterface_strings[300];
// GLOBAL: CARMA2_HW 0x0059b0d0
int gFrontend_stuff_not_loaded = 1;
// GLOBAL: CARMA2_HW 0x00686f10
br_material* gFrontend_backdrop_materials[3];
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
void C2_HOOK_FASTCALL DisplayJoystickSettings(tFrontend_spec *pFrontend) {

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
}

/* ==== REAL frontend flow ported from common/ ==== */

void C2_HOOK_FASTCALL LoadMenuImages(void) {
    int i;
    char s[256];
    char s2[256];
    FILE* f;
    int count;

    /* 1 to 6 are car images, 7 is driver image */
    for (i = 1; i < 8; i++) { /* FIXME: magic number */
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

// FUNCTION: CARMA2_HW 0x0046cf10

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

// FUNCTION: CARMA2_HW 0x0046abf0

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


int C2_HOOK_FASTCALL FRONTEND_Redraw(void) {
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

// FUNCTION: CARMA2_HW 0x0046f630

int C2_HOOK_FASTCALL FRONTEND_Main(tFrontendMenuType pFrontendType) {
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
            DRPixelmapRectangleCopy(gBack_screen, 0, 0,
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
        if (gFrontend_interpolate_steps_left != 0) {
            gFrontend_interpolate_steps_left -= 1;
        } else {
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

// FUNCTION: CARMA2_HW 0x00467a30

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

int C2_HOOK_FASTCALL FRONTEND_GenericMenuHandler(tFrontend_spec* pFrontend) {
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
}

// FUNCTION: CARMA2_HW 0x00466450

int C2_HOOK_FASTCALL Ians_GetItemAtMousePos(tFrontend_spec* pFrontend, int pX, int pY) {
    int i;
    tFrontend_slider* slider;

    for (i = 0; i < pFrontend->count_items; i++) {
        tFrontend_item_spec* item = &pFrontend->items[i];
        br_model* model = gFrontend_brender_items[i].model;
        if (pX >= model->vertices[1].p.v[0] && pX <= model->vertices[3].p.v[0]
                && pY >= -model->vertices[0].p.v[1] && pY <= -model->vertices[1].p.v[1]
                && item->visible
                && item->enabled > 0) {

            return i;
        }
    }
    for (slider = gCurrent_frontend_scrollbars; slider != NULL; slider = slider->next) {
        for (i = 0; i < 3; i++) {
            br_model* model = gFrontend_brender_items[slider->itemid_start + i].model;
            if (pX >= model->vertices[1].p.v[0] && pX <= model->vertices[3].p.v[0]
                    && pY >= -model->vertices[0].p.v[1] && pY <= -model->vertices[1].p.v[1]) {

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

        if (gFrontend_selected_item_index >= slider->itemid_left_reference && gFrontend_selected_item_index <= slider->itemid_left_reference + 2) {

            return slider;
        }
        if (gFrontend_selected_item_index >= slider->itemid_start && gFrontend_selected_item_index <= slider->itemid_start + 2) {

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

// FUNCTION: CARMA2_HW 0x004720e0

void C2_HOOK_FASTCALL RefreshScrollSet(tFrontend_spec* pFrontend) {
    int i;

    for (i = 0; i < gControls_scroller.range_length; i++) {

        strcpy(pFrontend->items[43 + i].text, GetMiscString(140 + gControls_scroller.field_0x8 + i));
        strcpy(pFrontend->items[53 + i].text, gKey_names[gKey_mapping[gControls_frontend_to_key_mapping_lut[i + gControls_scroller.field_0x8]] + 2]);
    }
    pFrontend->items[63].visible = gControls_scroller.field_0x8 != 0;
    pFrontend->items[64].visible = gControls_scroller.field_0x8 != gControls_scroller.field_0x0 - gControls_scroller.range_length;
    FuckWithWidths(pFrontend);
}


int C2_HOOK_FASTCALL DetermineKeyArrayIndex(void) {
    int start;

    start = gFrontend_selected_item_index;
    if (gFrontend_selected_item_index > 52) {
        start = gFrontend_selected_item_index - 10;
    }
    return start + (gControls_scroller.field_0x8 - 43);
}

// FUNCTION: CARMA2_HW 0x00471dd0

int C2_HOOK_FASTCALL Generic_FindNextActiveItem(tFrontend_spec* pFrontend, int pItem) {
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


void C2_HOOK_FASTCALL PrepareSliders(tFrontend_spec* pFrontend) {
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

int C2_HOOK_FASTCALL Generic_MenuHandler(tFrontend_spec* pFrontend) {
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
            NewGameToggleTyping(pFrontend);
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

void C2_HOOK_FASTCALL FRONTEND_DrawMenu(tFrontend_spec* pFrontend) {
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
}



void C2_HOOK_FASTCALL FillInRaceDescription(char *pDest, int pRace_index) {

    strcpy(pDest, gRace_list[pRace_index].description);
    MungeMetaCharactersChar(pDest, 'R', '\r');
    MungeMetaCharactersNum(pDest, 'O', gRace_list[pRace_index].count_explicit_opponents);
    MungeMetaCharactersNum(pDest, 'L', gRace_list[pRace_index].count_laps);
}

// FUNCTION: CARMA2_HW 0x0046aa20

void C2_HOOK_FASTCALL MenuSetCarImage(int pCar_index, int pBrender_index) {
    tPath_name pack_path;
    char pack_filename[64];
    char* pos_dot;
    tTWTVFS twt;
    int i;

    pos_dot = strrchr(gOpponents[pCar_index].car_file_name, '.');
    PathCat(pack_path, gApplication_path, "INTRFACE");
    PathCat(pack_path, pack_path, "CARIMAGE");

    strcpy(pack_filename, gOpponents[pCar_index].car_file_name);
    pack_filename[strlen(pack_filename) - 4] = '\0';
    strcat(pack_filename, "CI");
    PathCat(pack_path, pack_path, pack_filename);
    twt = OpenPackFileAndSetTiffLoading(pack_path);
    for (i = 0; i < 6; i++) {
        char pm_name[20];

        sprintf(pm_name, "%.*s%c.TIF", pos_dot - gOpponents[pCar_index].car_file_name, gOpponents[pCar_index].car_file_name, 'A' + i);
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
    char* pos_dot;
    tPath_name pack_path;
    char pack_filename[40];
    char pm_name[20];
    tTWTVFS twt;

    pos_dot = strrchr(gOpponents[pOpponent_index].abbrev_name, '.');
    PathCat(pack_path, gApplication_path, "INTRFACE");
    PathCat(pack_path, pack_path, "CARIMAGE");
    strcpy(pack_filename, gOpponents[pOpponent_index].car_file_name);
    pack_filename[strlen(pack_filename) - 4] = '\0';
    strcat(pack_filename, "CI");
    PathCat(pack_path, pack_path, pack_filename);
    twt = OpenPackFileAndSetTiffLoading(pack_path);

    sprintf(pm_name, "%.*s%c%c.TIF", pos_dot - gOpponents[pOpponent_index].abbrev_name, gOpponents[pOpponent_index].abbrev_name, '6', '4');
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

    gFrontend_brender_items[pFrontend_index].model->vertices[0].p.v[2] = -1.f;
    gFrontend_brender_items[pFrontend_index].model->vertices[1].p.v[2] = -1.f;
    gFrontend_brender_items[pFrontend_index].model->vertices[2].p.v[2] = -1.f;
    gFrontend_brender_items[pFrontend_index].model->vertices[3].p.v[2] = -1.f;
    BrModelUpdate(gFrontend_brender_items[pFrontend_index].model, BR_MODU_VERTEX_POSITIONS);
}

// FUNCTION: CARMA2_HW 0x0046e470

int C2_HOOK_FASTCALL ProcessInputString(void) {
    int len;
    int key;
    int int_ch;

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
    int_ch = PDGetKeyboardCharacter();
    if (int_ch != 0) {
        char ch;

        dr_dprintf("FRONTEND: Got char %d", int_ch);
        ch = PDConvertToASCIILessThan128((char)int_ch);
        dr_dprintf("FRONTEND: Char converted to %d", ch);
        if (ch >= 0x20 && ch != 0x7f) {
            gFrontend_current_input[len + 0] = ch;
            gFrontend_current_input[len + 1] = '\0';
            dr_dprintf("FRONTEND: Adding char %d to string. String now '%s'", int_ch, gFrontend_current_input);
            return 1;
        }
        dr_dprintf("FRONTEND: Invalid Char", int_ch);
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












int C2_HOOK_FASTCALL CreditsScreenInfunc(tFrontend_spec* pFrontend) {
    tPath_name interface_path;
    tPath_name credits_path;
    FILE* f;
    int i;

    if (!pFrontend->loaded) {
        LoadMenuSettings(pFrontend);
    }
    FuckWithWidths(pFrontend);
    PathCat(interface_path, gApplication_path, "INTRFACE");
    PathCat(credits_path, interface_path, "creditslist.txt");
    f = DRfopen(credits_path, "rt");
    if (f == NULL) {
        FatalError(kFatalError_Mysterious_SS, interface_path, interface_path);
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
    for (i = 0; i < gCredits_line_count; i++) {
        char s[256];
        char small_buf[11];

        GetALineAndDontArgue(f, s);
        if (strncmp(s, "FONT", 4) == 0) {
            strncpy(small_buf, &s[5], sizeof(small_buf) - 1);
#ifdef CARPOCALYPSE2_FIX_BUGS
            small_buf[sizeof(small_buf) - 1] = '\0';
#endif
            gCredits_fonts[i] = atoi(small_buf);
        } else if (strncmp(s, "SPACE", 5) == 0) {
            double space_height;
            strncpy(small_buf, &s[6], sizeof(small_buf) - 1);
#ifdef CARPOCALYPSE2_FIX_BUGS
            small_buf[sizeof(small_buf) - 1] = '\0';
#endif
            space_height = (double)PolyFontHeight(gCredits_fonts[i]) * atof(small_buf);
            gCredits_heights[i] += (int)space_height;
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
        }
        if (i + 1 < gCredits_line_count) {
            gCredits_fonts[i + 1] = gCredits_fonts[i];
            gCredits_throbs[i + 1] = gCredits_throbs[i];
            gCredits_heights[i + 1] = 0;
        }
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

int C2_HOOK_FASTCALL LoadGameInFunc(tFrontend_spec* pFrontend) {
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
    camera->field_of_view = BrDegreeToAngle(90);
    camera->hither_z = 1.f;
    camera->yon_z = 3.f;
    camera->width = 640.f;
    camera->height = 480.f;

    for (i = 0; i < CARPOCALYPSE2_ASIZE(gFrontend_billboard_actors); i++) {
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
}

// FUNCTION: CARMA2_HW 0x0046fa60

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

    SetJoystickFFBGain((int)(100.f * pSlider->value));
}

// FUNCTION: CARMA2_HW 0x004725f0

int C2_HOOK_FASTCALL Controls_Infunc(tFrontend_spec* pFrontend) {

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
}

// FUNCTION: CARMA2_HW 0x00472a30

int C2_HOOK_FASTCALL Controls_Outfunc(tFrontend_spec* pFrontend) {

    Generic_Outfunc(pFrontend);
    DisposeKeyNames();
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

// FUNCTION: CARMA2_HW 0x0045bd90

void C2_HOOK_FASTCALL SaveAllJoystickData(void) {

    NOT_IMPLEMENTED();
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

int C2_HOOK_FASTCALL Controls_SlotActivated(tFrontend_spec* pFrontend) {
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

        FRONTEND_Redraw();

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

int C2_HOOK_FASTCALL Controls_Ok(tFrontend_spec* pFrontend) {
    int i;
    int prev_item;

    gINT_00688748 = -1;
    WaitForNoKeys();

    for (i = 28; i < CARPOCALYPSE2_ASIZE(gKey_mapping); i++) {

        if (gKey_mapping[i] == -2) {
            gINT_00688748 = i;
            break;
        }
    }
    if (gJoystick_index == -1) {

        for (i = 28; i < CARPOCALYPSE2_ASIZE(gKey_mapping); i++) {

            if (gKey_mapping[i] >= 107 && gKey_mapping[i] <= 142) {
                gINT_00688748 = i;
            }
        }
    }
    if (gINT_00688748 == -1) {
        SaveAllJoystickData();
        return 0;
    }
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
        gFrontend_selected_item_index = i - gControls_scroller.field_0x8 + gControls_scroller.range_starts[0];
    }
    if (gFrontend_selected_item_index == -1) {
        PDFatalError("Screwed up in Controls_Ok().");
    }
    Controls_SlotActivated(pFrontend);
    gFrontend_selected_item_index = prev_item;
    return 4;
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

// FUNCTION: CARMA2_HW 0x00482160
void C2_HOOK_FASTCALL CyclePollKeys(void) {
    int i;

    for (i = 0; i < CARPOCALYPSE2_ASIZE(gKey_array); i++) {
        if (gKey_array[i] > gKey_poll_counter) {
            gKey_array[i] = 0;
            if (i > 143) {
                gJoy_array[i - 143] = -1; // yes this is a little weird I know...
            }
        }
    }
    gKey_poll_counter = 0;
}

// GLOBAL: CARMA2_HW 0x00604900
int gFrontend_controls_indices[15] = {
    12, 13, 17, 18, 22, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35
};
