#include "frontend_network.h"

#include "frontend.h"
#include "frontend_main.h"
#include "frontend_network_options.h"
#include "frontend_options.h"
#include "frontend_quit.h"
#include "globvars.h"
#include "globvrpb.h"
#include "graphics.h"
#include "init.h"
#include "input.h"
#include "intrface.h"
#include "loading.h"
#include "loadsave.h"
#include "main.h"
#include "newgame.h"

#include "structur.h"

#include "network.h"
#include "utility.h"
#include "platform.h"

#include "world.h"

#include "c2_string.h"

#include "carpocalypse2_macros.h"
// GLOBAL: CARMA2_HW 0x00763920
tNet_game_type gFrontend_game_type;

// GLOBAL: CARMA2_HW 0x00763b20
tNet_game_options gFrontend_net_options;

// GLOBAL: CARMA2_HW 0x00688aa8
tU32 gFrontend_net_current_roll;

// GLOBAL: CARMA2_HW 0x00686efc
tU32 gFrontend_net_last_roll;

// GLOBAL: CARMA2_HW 0x0059c820
tNet_mode gFrontend_net_mode = eNet_mode_thinking_about_it;

// GLOBAL: CARMA2_HW 0x00763714
tNet_game_details* gGame_to_join;

// GLOBAL: CARMA2_HW 0x007638e0
int gFrontend_net_car_index;

// GLOBAL: CARMA2_HW 0x00763600
char gFrontend_host_join_buffer[256];

// GLOBAL: CARMA2_HW 0x007638a8
const char* gJoined_game_notification;


// GLOBAL: CARMA2_HW 0x0068703c
int gINT_0068703c;

void C2_HOOK_FASTCALL SaveSinglePlayerState(void) {

    if (!gValid_stashed_save_game) {
        MakeSavedGame(&gStashed_save_game);
    }
    gValid_stashed_save_game = 1;
}


void C2_HOOK_FASTCALL NetworkJoinSetup(void) {
    gINT_0068703c = 0;
    SetAlwaysTyping();
    InitGamesToJoin();
    NetStartProducingJoinList(AddToJoinList);
}

#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern int C2_HOOK_FASTCALL DoLoadGame2(tSave_game* pSave_game);
extern void C2_HOOK_FASTCALL FuckWithWidths(tFrontend_spec* pFrontend);
extern const char* C2_HOOK_FASTCALL GetMiscString(int pIndex);
extern void C2_HOOK_FASTCALL InitGame(int pStart_race);
extern void C2_HOOK_FASTCALL LoadMenuSettings(tFrontend_spec* pFrontend);
extern void C2_HOOK_FASTCALL LoadRaces(tRace_list_spec* pRace_list, int* pCount, int pRace_type_index);
extern void C2_HOOK_FASTCALL MakeSavedGame(tSave_game* pSave_game);
extern void C2_HOOK_FASTCALL MungeButtonModels(tFrontend_spec* pFrontend, int pButton_index);
extern void C2_HOOK_FASTCALL NetDisposeGameDetails(tNet_game_details* pDetails);
extern void C2_HOOK_FASTCALL NetEndJoinList(void);
extern void C2_HOOK_FASTCALL NetStartProducingJoinList(tAddToJoinListProc *pAdd_proc);
extern tU32 C2_HOOK_FASTCALL PDGetTotalTime(void);
extern int C2_HOOK_FASTCALL PickNetRace(int pCurrent_race, tNet_sequence_type pNet_race_sequence);
extern void C2_HOOK_FASTCALL SetAlwaysTyping(void);
extern void C2_HOOK_FASTCALL SetOptions(tNet_game_type pGame_type, tNet_game_options* pGame_options);
extern void C2_HOOK_FASTCALL ShutdownNetIfRequired(void);
extern tU32 gFrontend_last_scroll;
extern tU32 gFrontend_time_last_input;
extern tJoinable_game gGames_to_join[6];
extern int gINT_00688748;
extern int gLast_graph_sel;
extern tNet_mode gNet_mode;
extern int gNumber_of_races;
extern tProgram_state gProgram_state;
extern int gRace_index;
extern tRace_list_spec gRace_list[100];
extern tSave_game gStashed_save_game;
extern int gValid_stashed_save_game;
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern int C2_HOOK_FASTCALL ChooseNetCar(tNet_game_details* pNet_game, tNet_game_options* pOptions, int* pCar_index, int pIm_the_host_so_fuck_off);
extern void C2_HOOK_FASTCALL ClearAlwaysTyping(void);
extern int C2_HOOK_FASTCALL DRStricmp(const char* p1, const char* p2);
extern void C2_HOOK_FASTCALL DisposeStorageSpace(tBrender_storage* pStorage);
extern void C2_HOOK_FASTCALL EdgeTriggerModeOff(void);
extern void C2_HOOK_FASTCALL EdgeTriggerModeOn(void);
extern int C2_HOOK_FASTCALL EitherMouseButtonDown(void);
extern int C2_HOOK_FASTCALL FRONTEND_CreateMenu(tFrontend_spec* pFrontend_spec);
extern int C2_HOOK_FASTCALL FRONTEND_DestroyMenu(tFrontend_spec* pFrontend);
extern int C2_HOOK_FASTCALL FindPrevActiveItem(tFrontend_spec* pFrontend, int pStart_index);
extern int C2_HOOK_FASTCALL GetItemAtMousePos(tFrontend_spec *pFrontend, int pX, int pY);
extern void C2_HOOK_FASTCALL GetMousePosition(int *pX, int *pY);
extern const char* C2_HOOK_FASTCALL IString_Get(int pIndex);
extern void C2_HOOK_FASTCALL InitialiseStorageSpace(int pUnknown, tBrender_storage* pStorage_space, int pMax_pixelmaps, int pMax_shade_tables, int pMax_materials, int pMax_models, int pMax_sounds);
extern void C2_HOOK_FASTCALL KillSplashScreen(void);
extern void C2_HOOK_FASTCALL Morph_Initialise(tFrontend_spec* pCurrent, tFrontend_spec* pNext);
extern void C2_HOOK_FASTCALL MungeMetaCharactersChar(char* pText, char pMeta, char pChar);
extern void C2_HOOK_FASTCALL MungeMetaCharactersNum(char* pText, char pMeta, int pNum);
extern tNet_game_details* C2_HOOK_FASTCALL NetHostGame(tNet_game_type pNet_type, tNet_game_options* pOptions, int pRace_index, const char* pHost_name, int pCar_index);
extern int C2_HOOK_FASTCALL NetJoinGame(tNet_game_details *pGame_details,char *pPlayer_name,int pCar_index);
extern void C2_HOOK_FASTCALL NetLeaveGame(tNet_game_details* pNet_game);
extern void C2_HOOK_FASTCALL NetSendHeadupToAllPlayers(char* pMessage);
extern int C2_HOOK_FASTCALL PDAnyKeyDown(void);
extern int C2_HOOK_FASTCALL PDKeyDown(int pKey_index);
extern void C2_HOOK_FASTCALL PollKeys(void);
extern void C2_HOOK_FASTCALL PossibleService(void);
extern int C2_HOOK_FASTCALL ProcessInputString(void);
extern void C2_HOOK_FASTCALL ReenableNetService(void);
extern int C2_HOOK_FASTCALL SaveOptions(void);
extern void C2_HOOK_FASTCALL ServiceGame(void);
extern void C2_HOOK_FASTCALL SolidPolyFontText(const char* pText, int pX, int pY, int pFont, tJustification pJust, int pRender);
extern void C2_HOOK_FASTCALL ToggleSelection(tFrontend_spec* pFrontend);
extern int C2_HOOK_FASTCALL ToggleTyping(tFrontend_spec* pFrontend);
extern void C2_HOOK_FASTCALL UpdateScrollPositions(tFrontend_spec* pFrontend);
extern char gCamera_incident_direction[];
extern tRace_info gCurrent_race;
extern tFrontend_spec gFrontend_QUIT;
extern int gFrontend_leave_current_menu;
extern int gFrontend_selected_item_index;
extern int gMouse_in_use;
extern tBrender_storage gNet_cars_storage_space;
extern int gNet_join_host_result;
extern int gNet_storage_space_initialized;
extern int gTyping;
extern int gTyping_slot;
#endif
// FUNCTION: CARMA2_HW 0x00467ef0
int C2_NAKED C2_HOOK_FASTCALL NetworkJoinMenuInfunc(tFrontend_spec* pFrontend) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;

    if (!pFrontend->loaded) {
        LoadMenuSettings(pFrontend);
    }
    SaveSinglePlayerState();
    pFrontend->items[30].text[0] = '\0';
    ReadNetGameChoices(&gFrontend_game_type, &gFrontend_net_options, &gRace_index);
    LoadRaces(gRace_list, &gNumber_of_races, gFrontend_game_type);
    strcpy(pFrontend->items[26].text, gProgram_state.player_name);
    FuckWithWidths(pFrontend);
    ResetInterfaceTimeout();

    for (i = 0; i < pFrontend->count_items; i++) {
        switch (i) {
        case 19:
        case 24:
            pFrontend->items[i].visible = 0;
            pFrontend->items[i].enabled = kFrontendItemEnabled_default;
            break;
        default:
            pFrontend->items[i].visible = 1;
            break;
        }
    }
    UpdateNetTrackScroller(pFrontend);
    UpdateNetGameTypeScroller(pFrontend);
    NetworkJoinSetup();
    return 0;
#else
    __asm {
            sub esp, 8
            push ebx
            push ebp
            mov ebp, ecx
            xor ebx, ebx
            push esi
            push edi
            cmp dword ptr [ebp + 0xad1c], ebx
            jne L00467f08
            call LoadMenuSettings
    L00467f08:
            cmp dword ptr [gValid_stashed_save_game], ebx
            jne L00467f1a
            mov ecx, offset gStashed_save_game
            call MakeSavedGame
    L00467f1a:
            mov dword ptr [gValid_stashed_save_game], 1
            mov byte ptr [ebp + 0x29d9], 0
            mov ecx, dword ptr [gFrontend_game_type]
            mov edx, offset gFrontend_net_options
            call SetOptions
            mov edx, dword ptr [gFrontend_net_options+0x28]
            or ecx, 0xffffffff
            call PickNetRace
            mov dword ptr [gRace_index], eax
            mov eax, dword ptr [gFrontend_game_type]
            push eax
            mov edx, offset gNumber_of_races
            mov ecx, offset gRace_list
            call LoadRaces
            mov edi, offset gProgram_state+0x1a10
            or ecx, 0xffffffff
            xor eax, eax
            lea edx, [ebp + 0x2479]
            repne scasb
            not ecx
            sub edi, ecx
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            shr ecx, 2
            rep movsd
            mov ecx, eax
            and ecx, 3
            rep movsb
            mov ecx, ebp
            call FuckWithWidths
            call PDGetTotalTime
            mov dword ptr [gFrontend_time_last_input], eax
            mov ecx, dword ptr [ebp + 0x104]
            xor eax, eax
            cmp ecx, ebx
            jle L00467fd4
            lea ecx, [ebp + 0x15c]
    L00467fae:
            cmp eax, 0x13
            mov dword ptr [ecx], 1
            je L00467fbe
            cmp eax, 0x18
            jne L00467fc3
    L00467fbe:
            mov dword ptr [ecx], ebx
            mov dword ptr [ecx - 4], ebx
    L00467fc3:
            mov edx, dword ptr [ebp + 0x104]
            inc eax
            add ecx, 0x158
            cmp eax, edx
            jl L00467fae
    L00467fd4:
            mov eax, dword ptr [ebp + 0x87d8]
            mov ecx, dword ptr [ebp + 0x87d4]
            mov edi, dword ptr [ebp + 0x87d0]
            sub ecx, eax
            mov edx, eax
            add eax, edi
            cmp edx, eax
            jge L0046805f
            mov eax, edx
            shl eax, 3
            sub eax, edx
            lea eax, [eax + eax*2]
            lea eax, [edx + eax*2]
            lea eax, [ebp + eax*8 + 0x189]
            mov dword ptr [esp + 0x10], eax
            lea eax, [ecx + ecx*8]
            shl eax, 4
            add eax, ecx
            lea ebx, [eax*4 + gRace_list]
    L00468017:
            mov edi, ebx
            or ecx, 0xffffffff
            xor eax, eax
            add ebx, 0x244
            repne scasb
            not ecx
            sub edi, ecx
            mov eax, ecx
            mov esi, edi
            mov edi, dword ptr [esp + 0x10]
            shr ecx, 2
            rep movsd
            mov ecx, eax
            and ecx, 3
            inc edx
            rep movsb
            mov ecx, dword ptr [ebp + 0x87d0]
            mov esi, dword ptr [ebp + 0x87d8]
            mov edi, dword ptr [esp + 0x10]
            add ecx, esi
            add edi, 0x158
            cmp edx, ecx
            mov dword ptr [esp + 0x10], edi
            jl L00468017
    L0046805f:
            mov ecx, dword ptr [ebp + 0x87a4]
            mov eax, dword ptr [ebp + 0x87a0]
            mov esi, dword ptr [ebp + 0x879c]
            sub eax, ecx
            mov ebx, ecx
            add ecx, esi
            cmp ebx, ecx
            mov dword ptr [esp + 0x10], eax
            jge L004680f3
            mov ecx, ebx
            shl ecx, 3
            sub ecx, ebx
            lea edx, [ecx + ecx*2]
            lea ecx, [ebx + edx*2]
            lea edx, [ebp + ecx*8 + 0x189]
            mov dword ptr [esp + 0x14], edx
            jmp L0046809d
    L00468099:
            mov eax, dword ptr [esp + 0x10]
    L0046809d:
            cmp eax, 2
            jl L004680a3
            inc eax
    L004680a3:
            lea ecx, [eax + 0x3b]
            call GetMiscString
            mov edi, eax
            or ecx, 0xffffffff
            xor eax, eax
            repne scasb
            mov eax, dword ptr [esp + 0x14]
            not ecx
            sub edi, ecx
            mov edx, ecx
            mov esi, edi
            mov edi, eax
            add eax, 0x158
            shr ecx, 2
            rep movsd
            mov ecx, edx
            mov dword ptr [esp + 0x14], eax
            and ecx, 3
            rep movsb
            mov ecx, dword ptr [esp + 0x10]
            mov eax, dword ptr [ebp + 0x879c]
            mov edx, dword ptr [ebp + 0x87a4]
            inc ecx
            inc ebx
            add eax, edx
            cmp ebx, eax
            mov dword ptr [esp + 0x10], ecx
            jl L00468099
    L004680f3:
            mov ecx, ebp
            call FuckWithWidths
            mov dword ptr [gINT_0068703c], 0
            call SetAlwaysTyping
            mov eax, offset gGames_to_join
    L0046810e:
            mov dword ptr [eax], 0
            add eax, 8
            cmp eax, offset gINT_00688748
            jl L0046810e
            mov ecx, offset AddToJoinList
            call NetStartProducingJoinList
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 8
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

// FUNCTION: CARMA2_HW 0x00468260
int C2_NAKED C2_HOOK_FASTCALL NetworkJoinMenuOutfunc(tFrontend_spec* pFrontend) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;
    int bound;
    (void)pFrontend;

    NetEndJoinList();
    bound = gLast_graph_sel;
    for (i = 0; i < bound; i++) {
        if (gGames_to_join[i].game != NULL) {
            NetDisposeGameDetails(gGames_to_join[i].game);
            gGames_to_join[i].game = NULL;
        }
    }
    return 0;
#else
    __asm {
            push ebx
            push esi
            push edi
            call NetEndJoinList
            mov ebx, dword ptr [gLast_graph_sel]
            xor edi, edi
            mov esi, offset gGames_to_join
    L00468275:
            cmp edi, ebx
            je L0046828a
            mov ecx, dword ptr [esi]
            test ecx, ecx
            je L0046828a
            call NetDisposeGameDetails
            mov dword ptr [esi], 0
    L0046828a:
            add esi, 8
            inc edi
            cmp esi, offset gINT_00688748
            jl L00468275
            pop edi
            pop esi
            xor eax, eax
            pop ebx
            ret
            nop
            nop
            nop
            nop
    }
#endif
}

void C2_HOOK_FASTCALL NetworkJoinUpdateScroller(tFrontend_spec* pFrontend) {
    int have_game;
    int i;

    have_game = 0;
    PossibleService();
    for (i = 0; i < CARPOCALYPSE2_ASIZE(gGames_to_join); i++) {
        if (gGames_to_join[i].game != NULL) {
            have_game = 1;
        }
    }
    if (have_game) {
        int game_i;
        for (i = pFrontend->scrollers[2].indexFirstScrollableItem, game_i = 0; i <= pFrontend->scrollers[2].indexLastScrollableItem; i++, game_i++) {
            if (gGames_to_join[game_i].time != 0 && gGames_to_join[game_i].game != NULL) {
                /* FIXME: game name type is wrong!!! */
                sprintf(pFrontend->items[i].text, "%s", &gGames_to_join[game_i].game->pd_net_info.addr[0xe]);
                pFrontend->items[i].enabled = kFrontendItemEnabled_enabled;
            } else {
                sprintf(pFrontend->items[i].text, "%s", " ");
                pFrontend->items[i].enabled = kFrontendItemEnabled_disabled;
            }
        }
        pFrontend->items[29].enabled = kFrontendItemEnabled_enabled;
    } else {
        sprintf(pFrontend->items[20].text, "%s", IString_Get(253));
        strcpy(pFrontend->items[21].text, " ");
        strcpy(pFrontend->items[22].text, " ");
        strcpy(pFrontend->items[23].text, " ");
        pFrontend->items[20].enabled = kFrontendItemEnabled_disabled;
        pFrontend->items[21].enabled = kFrontendItemEnabled_disabled;
        pFrontend->items[22].enabled = kFrontendItemEnabled_disabled;
        pFrontend->items[23].enabled = kFrontendItemEnabled_disabled;
        pFrontend->items[29].enabled = kFrontendItemEnabled_disabled;
    }
    FuckWithWidths(pFrontend);
}

// FUNCTION: CARMA2_HW 0x00469320
int C2_NAKED C2_HOOK_FASTCALL FRONTEND_NetworkJoinMenuHandler(tFrontend_spec* pFrontend) {
#ifndef CARPOCALYPSE2_MATCHING
    int original_item_index;

    original_item_index = gFrontend_selected_item_index;
    gTyping_slot = -1;
    EdgeTriggerModeOn();
    KillSplashScreen();
    if (gTyping) {
        int processed = ProcessInputString();
        FuckWithWidths(pFrontend);
        if (processed < 0) {
            NetGameToggleTyping(pFrontend);
        }
    } else {
        int key_down;
        int mouse_down;
        int mouse_item;

        PollKeys();
        EdgeTriggerModeOff();
        key_down = PDAnyKeyDown();
        if (key_down == -1 && key_down != 4) {
            gMouse_in_use = 0;
            ResetInterfaceTimeout();
        }
        EdgeTriggerModeOn();
        mouse_down = 0;
        mouse_item = original_item_index;
        if (gMouse_in_use) {
            int mouse_x, mouse_y;

            ResetInterfaceTimeout();
            GetMousePosition(&mouse_x, &mouse_y);
            mouse_down = EitherMouseButtonDown();
            mouse_item = GetItemAtMousePos(gCurrent_frontend_spec, mouse_x, mouse_y);
            gFrontend_selected_item_index = mouse_item;
            if (gFrontend_selected_item_index == -1) {
                gFrontend_selected_item_index = 21;
            }
        }
        if (PDKeyDown(72) || PDKeyDown(89)) {
             gFrontend_selected_item_index = FindPrevActiveItem(gCurrent_frontend_spec, gFrontend_selected_item_index);
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
            if (gFrontend_selected_item_index < pFrontend->count_items - 1) {
                gFrontend_selected_item_index = FindNextActiveItem(gCurrent_frontend_spec, gFrontend_selected_item_index);
            } else {
                gFrontend_selected_item_index = FindNextActiveItem(gCurrent_frontend_spec, -1);
            }
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
            int i;

            ShutdownNetIfRequired();
            MaybeRestoreSavedGame();
            gCurrent_frontend_spec->default_item = gFrontend_selected_item_index;
            FRONTEND_DestroyMenu(pFrontend);
            gCurrent_frontend_spec = pFrontend->previous;
            FRONTEND_CreateMenu(gCurrent_frontend_spec);
            Morph_Initialise(pFrontend, gCurrent_frontend_spec);
            gFrontend_selected_item_index = gCurrent_frontend_spec->default_item;
            for (i = 0; i < gCurrent_frontend_spec->count_scrollers; i++) {

                gCurrent_frontend_spec->scrollers[i].indexTopItem = gCurrent_frontend_spec->scrollers[i].indexOfItemAtTop;
            }
            UpdateScrollPositions(gCurrent_frontend_spec);
            return 0;
        }
        if (PDKeyDown(51) || PDKeyDown(52) || (mouse_down == 1 && mouse_item != -1)) {
            int result;

            ToggleSelection(pFrontend);
            if (!gMouse_in_use
                    && gFrontend_selected_item_index >= gCurrent_frontend_spec->scrollers[0].indexFirstScrollableItem
                    && gFrontend_selected_item_index <= gCurrent_frontend_spec->scrollers[0].indexLastScrollableItem) {

                gFrontend_selected_item_index = 21;
            }
            switch (pFrontend->items[gFrontend_selected_item_index].field_0xc) {
            case 2:
                return 2;
            case 1:
                gFrontend_leave_current_menu = 1;
                break;
            }
            if (pFrontend->items[gFrontend_selected_item_index].action != NULL) {
                result = pFrontend->items[gFrontend_selected_item_index].action(pFrontend);
            } else {
                result = pFrontend->items[gFrontend_selected_item_index].field_0xc;
            }
            if (pFrontend->items[(gFrontend_selected_item_index)].menuInfo != NULL) {
                int i;

                for (i = 0; i < gCurrent_frontend_spec->count_scrollers; i++) {

                    gCurrent_frontend_spec->scrollers[i].indexOfItemAtTop = gCurrent_frontend_spec->scrollers[i].indexTopItem;
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

                    gCurrent_frontend_spec->scrollers[i].indexOfItemAtTop = gCurrent_frontend_spec->scrollers[i].indexTopItem;
                }
                UpdateScrollPositions(gCurrent_frontend_spec);
            }
            return result;
        }
    }
    ServiceGame();
    NetworkJoinUpdateScroller(pFrontend);
    if (gFrontend_leave_current_menu) {
        return 1;
    } else {
        return 0;
    }
#else
    __asm {
            sub esp, 0x10
            mov eax, dword ptr [gFrontend_selected_item_index]
            push ebx
            push ebp
            push esi
            mov ebx, ecx
            push edi
            mov dword ptr [esp + 0x10], ebx
            mov dword ptr [esp + 0x1c], eax
            mov dword ptr [gTyping_slot], 0xffffffff
            call EdgeTriggerModeOn
            call KillSplashScreen
            mov eax, dword ptr [gTyping]
            test eax, eax
            je L004693d7
            call ProcessInputString
            mov ecx, ebx
            mov esi, eax
            call FuckWithWidths
            test esi, esi
            jge L00469710
            mov ecx, ebx
            call ToggleTyping
            mov eax, dword ptr [gTyping]
            test eax, eax
            jne L00469710
            mov ecx, dword ptr [gFrontend_selected_item_index]
            mov eax, ecx
            shl eax, 3
            sub eax, ecx
            lea edx, [eax + eax*2]
            lea eax, [ecx + edx*2]
            or ecx, 0xffffffff
            lea edx, [ebx + eax*8 + 0x189]
            xor eax, eax
            mov edi, edx
            repne scasb
            not ecx
            dec ecx
            je L00469710
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
            jmp L00469710
    L004693d7:
            call PollKeys
            call EdgeTriggerModeOff
            call PDAnyKeyDown
            cmp eax, -1
            je L00469404
            cmp eax, 4
            je L00469404
            mov dword ptr [gMouse_in_use], 0
            call PDGetTotalTime
            mov dword ptr [gFrontend_time_last_input], eax
    L00469404:
            call EdgeTriggerModeOn
            mov eax, dword ptr [gMouse_in_use]
            xor ebp, ebp
            test eax, eax
            je L00469461
            call PDGetTotalTime
            lea edx, [esp + 0x14]
            lea ecx, [esp + 0x18]
            mov dword ptr [gFrontend_time_last_input], eax
            call GetMousePosition
            call EitherMouseButtonDown
            mov edx, dword ptr [esp + 0x18]
            mov ecx, dword ptr [gCurrent_frontend_spec]
            mov ebp, eax
            mov eax, dword ptr [esp + 0x14]
            push eax
            call GetItemAtMousePos
            mov edi, eax
            cmp edi, -1
            je L00469455
            mov dword ptr [gFrontend_selected_item_index], edi
            jmp L00469465
    L00469455:
            mov dword ptr [gFrontend_selected_item_index], 0x15
            jmp L00469465
    L00469461:
            mov edi, dword ptr [esp + 0x1c]
    L00469465:
            mov ecx, 0x48
            call PDKeyDown
            test eax, eax
            jne L00469485
            mov ecx, 0x59
            call PDKeyDown
            test eax, eax
            je L0046951c
    L00469485:
            mov ecx, dword ptr [gFrontend_selected_item_index]
            lea eax, [ecx - 1]
            mov ecx, dword ptr [gCurrent_frontend_spec]
            test eax, eax
            jl L004694c3
            mov edx, eax
            shl edx, 3
            sub edx, eax
            lea edx, [edx + edx*2]
            lea edx, [eax + edx*2]
            lea edx, [ecx + edx*8 + 0x15c]
    L004694ac:
            mov esi, dword ptr [edx - 4]
            test esi, esi
            jle L004694b8
            cmp dword ptr [edx], 0
            jne L004694d4
    L004694b8:
            dec eax
            sub edx, 0x158
            test eax, eax
            jge L004694ac
    L004694c3:
            mov edx, dword ptr [ecx + 0x104]
            call FindPrevActiveItem
            mov ecx, dword ptr [gCurrent_frontend_spec]
    L004694d4:
            mov edx, dword ptr [gMouse_in_use]
            mov dword ptr [gFrontend_selected_item_index], eax
            test edx, edx
            jne L0046951c
            cmp eax, dword ptr [ecx + 0x87a4]
            jl L0046951c
            cmp eax, dword ptr [ecx + 0x87a8]
            jg L0046951c
            mov ecx, ebx
            call ToggleSelection
            mov ecx, dword ptr [gFrontend_selected_item_index]
            mov eax, ecx
            shl eax, 3
            sub eax, ecx
            lea eax, [eax + eax*2]
            lea ecx, [ecx + eax*2]
            mov eax, dword ptr [ebx + ecx*8 + 0x134]
            test eax, eax
            je L0046951c
            mov ecx, ebx
            call eax
    L0046951c:
            mov ecx, 0x49
            call PDKeyDown
            test eax, eax
            jne L0046953c
            mov ecx, 0x53
            call PDKeyDown
            test eax, eax
            je L00469614
    L0046953c:
            mov edx, dword ptr [ebx + 0x104]
            mov eax, dword ptr [gFrontend_selected_item_index]
            dec edx
            cmp eax, edx
            jge L0046958e
            mov esi, dword ptr [gCurrent_frontend_spec]
            inc eax
            mov ecx, dword ptr [esi + 0x104]
            lea edx, [ecx - 1]
            cmp eax, edx
            jg L0046958a
            mov ecx, eax
            shl ecx, 3
            sub ecx, eax
            lea ecx, [ecx + ecx*2]
            lea ecx, [eax + ecx*2]
            lea ecx, [esi + ecx*8 + 0x15c]
    L00469574:
            cmp dword ptr [ecx - 4], 0
            jle L0046957f
            cmp dword ptr [ecx], 0
            jne L004695cc
    L0046957f:
            inc eax
            add ecx, 0x158
            cmp eax, edx
            jle L00469574
    L0046958a:
            xor eax, eax
            jmp L004695cc
    L0046958e:
            mov esi, dword ptr [gCurrent_frontend_spec]
            mov dword ptr [gFrontend_selected_item_index], 0xffffffff
            xor ecx, ecx
            mov eax, dword ptr [esi + 0x104]
            lea edx, [eax - 1]
            test edx, edx
            jl L004695c8
            lea eax, [esi + 0x15c]
    L004695b3:
            cmp dword ptr [eax - 4], 0
            jle L004695be
            cmp dword ptr [eax], 0
            jne L004695ca
    L004695be:
            inc ecx
            add eax, 0x158
            cmp ecx, edx
            jle L004695b3
    L004695c8:
            xor ecx, ecx
    L004695ca:
            mov eax, ecx
    L004695cc:
            mov ecx, dword ptr [gMouse_in_use]
            mov dword ptr [gFrontend_selected_item_index], eax
            test ecx, ecx
            jne L00469614
            cmp eax, dword ptr [esi + 0x87a4]
            jl L00469614
            cmp eax, dword ptr [esi + 0x87a8]
            jg L00469614
            mov ecx, ebx
            call ToggleSelection
            mov ecx, dword ptr [gFrontend_selected_item_index]
            mov eax, ecx
            shl eax, 3
            sub eax, ecx
            lea edx, [eax + eax*2]
            lea eax, [ecx + edx*2]
            mov eax, dword ptr [ebx + eax*8 + 0x134]
            test eax, eax
            je L00469614
            mov ecx, ebx
            call eax
    L00469614:
            mov ecx, 0x3f
            call PDKeyDown
            test eax, eax
            je L004696de
            call ShutdownNetIfRequired
            mov eax, dword ptr [gValid_stashed_save_game]
            test eax, eax
            je L00469659
            push -1
            mov edx, offset gNumber_of_races
            mov ecx, offset gRace_list
            call LoadRaces
            mov ecx, offset gStashed_save_game
            call DoLoadGame2
            mov dword ptr [gValid_stashed_save_game], 0
    L00469659:
            mov ecx, dword ptr [gCurrent_frontend_spec]
            mov edx, dword ptr [gFrontend_selected_item_index]
            mov dword ptr [ecx + 0x118], edx
            mov ecx, ebx
            call FRONTEND_DestroyMenu
            mov ecx, dword ptr [ebx + 0x114]
            mov dword ptr [gCurrent_frontend_spec], ecx
            call FRONTEND_CreateMenu
            mov edx, dword ptr [gCurrent_frontend_spec]
            mov ecx, ebx
            call Morph_Initialise
            mov ecx, dword ptr [gCurrent_frontend_spec]
            xor edx, edx
            mov eax, dword ptr [ecx + 0x118]
            mov dword ptr [gFrontend_selected_item_index], eax
            mov eax, dword ptr [ecx + 0x8790]
            test eax, eax
            jle L004696cf
            xor eax, eax
    L004696af:
            mov esi, dword ptr [ecx + eax + 0x87c0]
            inc edx
            mov dword ptr [ecx + eax + 0x87a0], esi
            mov ecx, dword ptr [gCurrent_frontend_spec]
            add eax, 0x34
            cmp edx, dword ptr [ecx + 0x8790]
            jl L004696af
    L004696cf:
            call UpdateScrollPositions
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x10
            ret
    L004696de:
            mov ecx, 0x33
            call PDKeyDown
            test eax, eax
            jne L00469880
            mov ecx, 0x34
            call PDKeyDown
            test eax, eax
            jne L00469880
            cmp ebp, 1
            jne L00469710
            cmp edi, -1
            jne L00469880
    L00469710:
            call ServiceGame
            xor esi, esi
            call PossibleService
            mov eax, offset gGames_to_join
    L00469721:
            cmp dword ptr [eax], 0
            je L0046972b
            mov esi, 1
    L0046972b:
            add eax, 8
            cmp eax, offset gINT_00688748
            jl L00469721
            test esi, esi
            je L004697fb
            mov ebp, dword ptr [ebx + 0x880c]
            mov eax, dword ptr [ebx + 0x8810]
            cmp ebp, eax
            jg L004697eb
            lea ecx, [ebp + 1]
            lea edi, [ebp*8]
            mov eax, ecx
            lea ebx, [ebp*8]
            shl eax, 3
            sub eax, ecx
            lea edx, [eax + eax*2]
            lea eax, [ecx + edx*2]
            mov ecx, dword ptr [esp + 0x10]
            lea esi, [ecx + eax*8]
    L00469776:
            mov eax, edi
            sub eax, ebx
            mov ecx, dword ptr [eax + gGames_to_join+0x4]
            test ecx, ecx
            je L004697b9
            mov eax, edi
            sub eax, ebx
            mov ecx, dword ptr [eax + gGames_to_join]
            test ecx, ecx
            je L004697b9
            mov eax, edi
            sub eax, ebx
            mov edx, dword ptr [eax + gGames_to_join]
            lea eax, [esi + 0x31]
            add edx, 0xe
            push edx
            push offset gFrontend_QUIT+0xb90c
            push eax
            call sprintf
            add esp, 0xc
            mov dword ptr [esi], 1
            jmp L004697d5
    L004697b9:
            push offset gCamera_incident_direction+0xc
            lea ecx, [esi + 0x31]
            push offset gFrontend_QUIT+0xb90c
            push ecx
            call sprintf
            add esp, 0xc
            mov dword ptr [esi], 0xffffffff
    L004697d5:
            mov edx, dword ptr [esp + 0x10]
            inc ebp
            add edi, 8
            add esi, 0x158
            cmp ebp, dword ptr [edx + 0x8810]
            jle L00469776
    L004697eb:
            mov ebx, dword ptr [esp + 0x10]
            mov dword ptr [ebx + 0x2850], 1
            jmp L00469864
    L004697fb:
            mov ecx, 0xfd
            call IString_Get
            push eax
            lea eax, [ebx + 0x1c69]
            push offset gFrontend_QUIT+0xb90c
            push eax
            call sprintf
            mov cx, word ptr [gCamera_incident_direction+0xc]
            add esp, 0xc
            mov word ptr [ebx + 0x1dc1], cx
            mov dx, word ptr [gCamera_incident_direction+0xc]
            mov word ptr [ebx + 0x1f19], dx
            mov ax, word ptr [gCamera_incident_direction+0xc]
            mov word ptr [ebx + 0x2071], ax
            or eax, 0xffffffff
            mov dword ptr [ebx + 0x2850], eax
            mov dword ptr [ebx + 0x1c38], eax
            mov dword ptr [ebx + 0x1d90], eax
            mov dword ptr [ebx + 0x1ee8], eax
            mov dword ptr [ebx + 0x2040], eax
    L00469864:
            mov ecx, ebx
            call FuckWithWidths
            mov ecx, dword ptr [gFrontend_leave_current_menu]
            xor eax, eax
            test ecx, ecx
            setne al
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x10
            ret
    L00469880:
            mov ecx, ebx
            call ToggleSelection
            mov eax, dword ptr [gMouse_in_use]
            mov esi, dword ptr [gCurrent_frontend_spec]
            test eax, eax
            mov eax, dword ptr [gFrontend_selected_item_index]
            jne L004698b5
            cmp eax, dword ptr [esi + 0x87a4]
            jl L004698b5
            cmp eax, dword ptr [esi + 0x87a8]
            jg L004698b5
            mov eax, 0x15
            mov dword ptr [gFrontend_selected_item_index], eax
    L004698b5:
            mov ecx, eax
            shl ecx, 3
            sub ecx, eax
            lea ecx, [ecx + ecx*2]
            lea edx, [eax + ecx*2]
            mov ecx, dword ptr [ebx + edx*8 + 0x13c]
            cmp ecx, 2
            lea edx, [ebx + edx*8]
            jne L004698db
            mov eax, ecx
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x10
            ret
    L004698db:
            cmp ecx, 1
            jne L004698e6
            mov dword ptr [gFrontend_leave_current_menu], ecx
    L004698e6:
            mov edx, dword ptr [edx + 0x134]
            test edx, edx
            je L00469903
            mov ecx, ebx
            call edx
            mov esi, dword ptr [gCurrent_frontend_spec]
            mov edi, eax
            mov eax, dword ptr [gFrontend_selected_item_index]
            jmp L0046991b
    L00469903:
            mov edx, dword ptr [esp + 0x1c]
            mov ecx, edx
            shl ecx, 3
            sub ecx, edx
            lea ecx, [ecx + ecx*2]
            lea edx, [edx + ecx*2]
            mov edi, dword ptr [ebx + edx*8 + 0x13c]
    L0046991b:
            mov ecx, eax
            shl ecx, 3
            sub ecx, eax
            lea ecx, [ecx + ecx*2]
            lea edx, [eax + ecx*2]
            mov ecx, dword ptr [ebx + edx*8 + 0x138]
            test ecx, ecx
            je L00469a17
            mov dword ptr [esi + 0x118], eax
            mov eax, dword ptr [gCurrent_frontend_spec]
            xor edx, edx
            mov ecx, dword ptr [eax + 0x8790]
            test ecx, ecx
            jle L0046996f
            xor ecx, ecx
    L00469950:
            mov esi, dword ptr [eax + ecx + 0x87a0]
            inc edx
            mov dword ptr [eax + ecx + 0x87c0], esi
            mov eax, dword ptr [gCurrent_frontend_spec]
            add ecx, 0x34
            cmp edx, dword ptr [eax + 0x8790]
            jl L00469950
    L0046996f:
            mov ecx, ebx
            call FRONTEND_DestroyMenu
            mov ecx, dword ptr [gFrontend_selected_item_index]
            mov eax, ecx
            shl eax, 3
            sub eax, ecx
            lea eax, [eax + eax*2]
            lea ecx, [ecx + eax*2]
            mov ecx, dword ptr [ebx + ecx*8 + 0x138]
            mov dword ptr [gCurrent_frontend_spec], ecx
            call FRONTEND_CreateMenu
            mov edx, dword ptr [gCurrent_frontend_spec]
            mov eax, dword ptr [ebx + 0x114]
            cmp edx, eax
            je L004699b7
            mov dword ptr [edx + 0x114], ebx
            mov edx, dword ptr [gCurrent_frontend_spec]
    L004699b7:
            mov ecx, ebx
            call Morph_Initialise
            mov ecx, dword ptr [gCurrent_frontend_spec]
            cmp ecx, offset gFrontend_QUIT
            jne L004699d8
            mov dword ptr [gFrontend_selected_item_index], 0
            jmp L004699e4
    L004699d8:
            mov edx, dword ptr [ecx + 0x118]
            mov dword ptr [gFrontend_selected_item_index], edx
    L004699e4:
            mov eax, dword ptr [ecx + 0x8790]
            xor edx, edx
            test eax, eax
            jle L00469a12
            xor eax, eax
    L004699f2:
            mov esi, dword ptr [ecx + eax + 0x87c0]
            inc edx
            mov dword ptr [ecx + eax + 0x87a0], esi
            mov ecx, dword ptr [gCurrent_frontend_spec]
            add eax, 0x34
            cmp edx, dword ptr [ecx + 0x8790]
            jl L004699f2
    L00469a12:
            call UpdateScrollPositions
    L00469a17:
            mov eax, edi
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x10
            ret
    }
#endif
}

void C2_HOOK_FASTCALL UpdateNetTrackScroller(tFrontend_spec* pFrontend) {
    int i;
    tFrontend_scroller_spec* scroller = &pFrontend->scrollers[1];

    for (i = scroller->indexFirstScrollableItem; i < scroller->indexFirstScrollableItem + scroller->nbDisplayedAtOnce; i++) {

        strcpy(pFrontend->items[i].text, gRace_list[i - scroller->indexFirstScrollableItem].name);
    }
}

void C2_HOOK_FASTCALL UpdateNetGameTypeScroller(tFrontend_spec* pFrontend) {
    tFrontend_scroller_spec* scroller;
    int i;

    scroller = &pFrontend->scrollers[0];
    for (i = scroller->indexFirstScrollableItem; i < scroller->indexFirstScrollableItem + scroller->nbDisplayedAtOnce; i++) {
        int text_idx;

        text_idx = i - scroller->indexTopItem;
        if (text_idx >= 2) {
            /* Skip "smashy things" network type */
            text_idx += 1;
        }
        strcpy(pFrontend->items[i].text, GetMiscString(eMiscString_network_type_start + text_idx));
    }
    FuckWithWidths(pFrontend);
}

// FUNCTION: CARMA2_HW 0x00467ce0
int C2_NAKED C2_HOOK_FASTCALL NetGameTypeUp(tFrontend_spec* pFrontend) {
#ifndef CARPOCALYPSE2_MATCHING

    if (PDGetTotalTime() - gFrontend_last_scroll > 400) {
        gFrontend_last_scroll = PDGetTotalTime();
        ScrollUp(pFrontend, 0);
    }
    UpdateNetGameTypeScroller(pFrontend);
    return 0;
#else
    __asm {
            sub esp, 8
            push ebx
            push ebp
            push esi
            push edi
            mov ebx, ecx
            call PDGetTotalTime
            sub eax, dword ptr [gFrontend_last_scroll]
            cmp eax, 0x190
            jbe L00467d1a
            call PDGetTotalTime
            mov dword ptr [gFrontend_last_scroll], eax
            mov eax, dword ptr [ebx + 0x87a0]
            cmp eax, dword ptr [ebx + 0x87a4]
            jle L00467d1a
            dec eax
            mov dword ptr [ebx + 0x87a0], eax
    L00467d1a:
            mov ebp, dword ptr [ebx + 0x87a4]
            mov ecx, dword ptr [ebx + 0x879c]
            mov eax, dword ptr [ebx + 0x87a0]
            add ecx, ebp
            sub eax, ebp
            cmp ebp, ecx
            mov dword ptr [esp + 0x10], eax
            jge L00467dad
            mov ecx, ebp
            shl ecx, 3
            sub ecx, ebp
            lea edx, [ecx + ecx*2]
            lea ecx, [ebp + edx*2]
            lea edx, [ebx + ecx*8 + 0x189]
            mov dword ptr [esp + 0x14], edx
            jmp L00467d57
    L00467d53:
            mov eax, dword ptr [esp + 0x10]
    L00467d57:
            cmp eax, 2
            jl L00467d5d
            inc eax
    L00467d5d:
            lea ecx, [eax + 0x3b]
            call GetMiscString
            mov edi, eax
            or ecx, 0xffffffff
            xor eax, eax
            repne scasb
            mov eax, dword ptr [esp + 0x14]
            not ecx
            sub edi, ecx
            mov edx, ecx
            mov esi, edi
            mov edi, eax
            add eax, 0x158
            shr ecx, 2
            rep movsd
            mov ecx, edx
            mov dword ptr [esp + 0x14], eax
            and ecx, 3
            rep movsb
            mov ecx, dword ptr [esp + 0x10]
            mov eax, dword ptr [ebx + 0x879c]
            mov edx, dword ptr [ebx + 0x87a4]
            inc ecx
            inc ebp
            add eax, edx
            cmp ebp, eax
            mov dword ptr [esp + 0x10], ecx
            jl L00467d53
    L00467dad:
            mov ecx, ebx
            call FuckWithWidths
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 8
            ret
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00468790
int C2_HOOK_FASTCALL NetSetRaceType(tFrontend_spec* pFrontend) {

    gFrontend_game_type = pFrontend->scrollers[0].indexTopItem - pFrontend->scrollers[0].indexFirstScrollableItem;
    switch (gFrontend_selected_item_index) {
    case 4:
        gFrontend_game_type += 1;
        break;
    case 5:
        gFrontend_game_type += 2;
        break;
    case 6:
        gFrontend_game_type += 3;
        break;
    }

    if (gFrontend_game_type >= 2) {
        gFrontend_game_type += 1;
    }
    pFrontend->items[17].enabled = kFrontendItemEnabled_enabled;
    pFrontend->items[29].enabled = kFrontendItemEnabled_disabled;
    gNet_last_game_type = gFrontend_game_type;
    SaveOptions();
    ReadNetGameChoices(&gFrontend_game_type, &gFrontend_net_options, &gRace_index);
    LoadRaces(gRace_list, &gNumber_of_races, gFrontend_game_type);
    sprintf(pFrontend->items[30].text, "%s", GetMiscString(eMiscString_network_type_description_start + gFrontend_game_type));
    MungeMetaCharactersChar(pFrontend->items[30].text, 'R', '\r');
    pFrontend->scrollers[1].indexTopItem = pFrontend->scrollers[1].indexFirstScrollableItem;
    pFrontend->scrollers[1].count = gNumber_of_races;
    UpdateNetTrackScroller(pFrontend);
    UpdateNetGameTypeScroller(pFrontend);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00467dc0
int C2_NAKED C2_HOOK_FASTCALL NetGameTypeDn(tFrontend_spec* pFrontend) {
#ifndef CARPOCALYPSE2_MATCHING

    if (PDGetTotalTime() - gFrontend_last_scroll > 400) {
        gFrontend_last_scroll = PDGetTotalTime();
        ScrollDn(pFrontend, 0);
    }
    UpdateNetGameTypeScroller(pFrontend);
    return 0;
#else
    __asm {
            sub esp, 8
            push ebx
            push ebp
            push esi
            push edi
            mov ebx, ecx
            call PDGetTotalTime
            sub eax, dword ptr [gFrontend_last_scroll]
            cmp eax, 0x190
            jbe L00467e0c
            call PDGetTotalTime
            mov dword ptr [gFrontend_last_scroll], eax
            mov eax, dword ptr [ebx + 0x87a0]
            mov ecx, dword ptr [ebx + 0x8798]
            mov ebp, dword ptr [ebx + 0x87a4]
            mov edx, dword ptr [ebx + 0x879c]
            add ecx, ebp
            add edx, eax
            cmp edx, ecx
            jge L00467e0c
            inc eax
            mov dword ptr [ebx + 0x87a0], eax
    L00467e0c:
            mov ebp, dword ptr [ebx + 0x87a4]
            mov ecx, dword ptr [ebx + 0x879c]
            mov eax, dword ptr [ebx + 0x87a0]
            add ecx, ebp
            sub eax, ebp
            cmp ebp, ecx
            mov dword ptr [esp + 0x10], eax
            jge L00467e9f
            mov ecx, ebp
            shl ecx, 3
            sub ecx, ebp
            lea edx, [ecx + ecx*2]
            lea ecx, [ebp + edx*2]
            lea edx, [ebx + ecx*8 + 0x189]
            mov dword ptr [esp + 0x14], edx
            jmp L00467e49
    L00467e45:
            mov eax, dword ptr [esp + 0x10]
    L00467e49:
            cmp eax, 2
            jl L00467e4f
            inc eax
    L00467e4f:
            lea ecx, [eax + 0x3b]
            call GetMiscString
            mov edi, eax
            or ecx, 0xffffffff
            xor eax, eax
            repne scasb
            mov eax, dword ptr [esp + 0x14]
            not ecx
            sub edi, ecx
            mov edx, ecx
            mov esi, edi
            mov edi, eax
            add eax, 0x158
            shr ecx, 2
            rep movsd
            mov ecx, edx
            mov dword ptr [esp + 0x14], eax
            and ecx, 3
            rep movsb
            mov ecx, dword ptr [esp + 0x10]
            mov eax, dword ptr [ebx + 0x87a4]
            mov edx, dword ptr [ebx + 0x879c]
            inc ecx
            inc ebp
            add eax, edx
            cmp ebp, eax
            mov dword ptr [esp + 0x10], ecx
            jl L00467e45
    L00467e9f:
            mov ecx, ebx
            call FuckWithWidths
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 8
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00466c00
int C2_NAKED C2_HOOK_FASTCALL NetRaceUp(tFrontend_spec* pFrontend) {
#ifndef CARPOCALYPSE2_MATCHING

    gFrontend_net_current_roll = PDGetTotalTime();
    if (gFrontend_net_current_roll - gFrontend_net_last_roll < 300) {
        return 0;
    }
    gFrontend_net_last_roll = gFrontend_net_current_roll;
    ScrollUp(pFrontend, 1);
    RefreshNetRacesScroller(pFrontend);
    return 0;
#else
    __asm {
            push ecx
            push ebp
            mov ebp, ecx
            push esi
            mov dword ptr [esp + 8], ebp
            call PDGetTotalTime
            mov esi, dword ptr [gFrontend_net_last_roll]
            mov ecx, eax
            sub ecx, esi
            mov dword ptr [gFrontend_net_current_roll], eax
            cmp ecx, 0x12c
            jge L00466c2b
            xor eax, eax
            pop esi
            pop ebp
            pop ecx
            ret
    L00466c2b:
            mov dword ptr [gFrontend_net_last_roll], eax
            mov eax, dword ptr [ebp + 0x87d4]
            mov ecx, dword ptr [ebp + 0x87d8]
            push edi
            cmp eax, ecx
            push ebx
            jle L00466c49
            dec eax
            mov dword ptr [ebp + 0x87d4], eax
    L00466c49:
            mov edx, dword ptr [ebp + 0x87d8]
            mov eax, dword ptr [ebp + 0x87d4]
            mov ecx, dword ptr [ebp + 0x87dc]
            sub eax, edx
            cmp edx, ecx
            jg L00466cbe
            lea ecx, [eax + eax*8]
            shl ecx, 4
            add ecx, eax
            mov eax, edx
            shl eax, 3
            sub eax, edx
            lea ebx, [ecx*4 + gRace_list]
            lea eax, [eax + eax*2]
            lea ecx, [edx + eax*2]
            lea ebp, [ebp + ecx*8 + 0x189]
    L00466c84:
            mov edi, ebx
            or ecx, 0xffffffff
            xor eax, eax
            add ebx, 0x244
            repne scasb
            not ecx
            sub edi, ecx
            mov eax, ecx
            mov esi, edi
            mov edi, ebp
            add ebp, 0x158
            shr ecx, 2
            rep movsd
            mov ecx, eax
            and ecx, 3
            inc edx
            rep movsb
            mov ecx, dword ptr [esp + 0x10]
            cmp edx, dword ptr [ecx + 0x87dc]
            jle L00466c84
            mov ebp, ecx
    L00466cbe:
            mov ecx, ebp
            call FuckWithWidths
            xor edx, edx
            mov ecx, ebp
            call MungeButtonModels
            xor eax, eax
            pop ebx
            pop edi
            pop esi
            pop ebp
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
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004691a0
int C2_NAKED C2_HOOK_FASTCALL NetHostChooseThisRace(tFrontend_spec* pFrontend) {
#ifndef CARPOCALYPSE2_MATCHING

    pFrontend->items[17].enabled = kFrontendItemEnabled_enabled;
    pFrontend->items[29].enabled = kFrontendItemEnabled_disabled;
    gProgram_state.current_race_index = RaceIndex(pFrontend->items[gFrontend_selected_item_index].text);
    FillInRaceDescription(pFrontend->items[30].text, gProgram_state.current_race_index);
    FuckWithWidths(pFrontend);
    return 0;
#else
    __asm {
            push ecx
            push ebx
            push ebp
            mov ebp, ecx
            push esi
            xor esi, esi
            push edi
            mov dword ptr [ebp + 0x1830], 1
            mov dword ptr [ebp + 0x2850], 0xffffffff
            mov ecx, dword ptr [gFrontend_selected_item_index]
            mov dword ptr [esp + 0x10], ebp
            mov eax, ecx
            shl eax, 3
            sub eax, ecx
            lea eax, [eax + eax*2]
            lea ecx, [ecx + eax*2]
            mov eax, dword ptr [gNumber_of_races]
            test eax, eax
            lea ebx, [ebp + ecx*8 + 0x189]
            jle L00469206
            mov edi, offset gRace_list
    L004691e9:
            mov edx, edi
            mov ecx, ebx
            call DRStricmp
            test eax, eax
            je L0046920a
            mov eax, dword ptr [gNumber_of_races]
            inc esi
            add edi, 0x244
            cmp esi, eax
            jl L004691e9
    L00469206:
            mov esi, dword ptr [esp + 0x10]
    L0046920a:
            lea ebx, [esi + esi*8]
            or ecx, 0xffffffff
            shl ebx, 4
            add ebx, esi
            xor eax, eax
            shl ebx, 2
            add ebp, 0x29d9
            mov dword ptr [gProgram_state+0x78], esi
            lea edi, [ebx + gRace_list+0x80]
            push 0xd
            repne scasb
            not ecx
            sub edi, ecx
            mov edx, ecx
            mov esi, edi
            mov edi, ebp
            shr ecx, 2
            rep movsd
            mov ecx, edx
            mov dl, 0x52
            and ecx, 3
            rep movsb
            mov ecx, ebp
            call MungeMetaCharactersChar
            mov eax, dword ptr [ebx + gRace_list+0x194]
            mov dl, 0x4f
            push eax
            mov ecx, ebp
            call MungeMetaCharactersNum
            mov ecx, dword ptr [ebx + gRace_list+0x1dc]
            mov dl, 0x4c
            push ecx
            mov ecx, ebp
            call MungeMetaCharactersNum
            mov ecx, dword ptr [esp + 0x10]
            call FuckWithWidths
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            pop ecx
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00466da0
int C2_NAKED C2_HOOK_FASTCALL NetRaceDn(tFrontend_spec* pFrontend) {
#ifndef CARPOCALYPSE2_MATCHING

    gFrontend_net_current_roll = PDGetTotalTime();
    if (gFrontend_net_current_roll - gFrontend_net_last_roll < 300) {
        return 0;
    }
    gFrontend_net_last_roll = gFrontend_net_current_roll;
    ScrollDn(pFrontend, 1);
    RefreshNetRacesScroller(pFrontend);
    return 0;
#else
    __asm {
            push ecx
            push ebp
            mov ebp, ecx
            push esi
            mov dword ptr [esp + 8], ebp
            call PDGetTotalTime
            mov esi, dword ptr [gFrontend_net_last_roll]
            mov ecx, eax
            sub ecx, esi
            mov dword ptr [gFrontend_net_current_roll], eax
            cmp ecx, 0x12c
            jge L00466dcb
            xor eax, eax
            pop esi
            pop ebp
            pop ecx
            ret
    L00466dcb:
            push edi
            mov dword ptr [gFrontend_net_last_roll], eax
            mov eax, dword ptr [ebp + 0x87d4]
            mov edx, dword ptr [ebp + 0x87d0]
            mov ecx, dword ptr [ebp + 0x87cc]
            mov edi, dword ptr [ebp + 0x87d8]
            add edx, eax
            add ecx, edi
            cmp edx, ecx
            push ebx
            jge L00466df9
            inc eax
            mov dword ptr [ebp + 0x87d4], eax
    L00466df9:
            mov edx, dword ptr [ebp + 0x87d8]
            mov eax, dword ptr [ebp + 0x87d4]
            mov ecx, dword ptr [ebp + 0x87dc]
            sub eax, edx
            cmp edx, ecx
            jg L00466e6e
            lea ecx, [eax + eax*8]
            shl ecx, 4
            add ecx, eax
            mov eax, edx
            shl eax, 3
            sub eax, edx
            lea ebx, [ecx*4 + gRace_list]
            lea eax, [eax + eax*2]
            lea ecx, [edx + eax*2]
            lea ebp, [ebp + ecx*8 + 0x189]
    L00466e34:
            mov edi, ebx
            or ecx, 0xffffffff
            xor eax, eax
            add ebx, 0x244
            repne scasb
            not ecx
            sub edi, ecx
            mov eax, ecx
            mov esi, edi
            mov edi, ebp
            add ebp, 0x158
            shr ecx, 2
            rep movsd
            mov ecx, eax
            and ecx, 3
            inc edx
            rep movsb
            mov ecx, dword ptr [esp + 0x10]
            cmp edx, dword ptr [ecx + 0x87dc]
            jle L00466e34
            mov ebp, ecx
    L00466e6e:
            mov ecx, ebp
            call FuckWithWidths
            xor edx, edx
            mov ecx, ebp
            call MungeButtonModels
            xor eax, eax
            pop ebx
            pop edi
            pop esi
            pop ebp
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
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00469280
int C2_NAKED C2_HOOK_FASTCALL NetCancel(tFrontend_spec* pFrontend) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;

    NetEndJoinList();
    for (i = 0; i < gLast_graph_sel; i++) {
        tNet_game_details* game = gGames_to_join[i].game;

        if (game != NULL) {
            NetDisposeGameDetails(game);
            gGames_to_join[i].game = NULL;
        }
    }
    ShutdownNetIfRequired();
    gNet_mode = eNet_mode_none;
    LoadRaces(gRace_list, &gNumber_of_races, -1);
    InitGame(gDev_initial_race);
    MaybeRestoreSavedGame();
    return 0;
#else
    __asm {
            push ebx
            push esi
            push edi
            call NetEndJoinList
            mov ebx, dword ptr [gLast_graph_sel]
            xor edi, edi
            mov esi, offset gGames_to_join
    L00469295:
            cmp edi, ebx
            je L004692aa
            mov ecx, dword ptr [esi]
            test ecx, ecx
            je L004692aa
            call NetDisposeGameDetails
            mov dword ptr [esi], 0
    L004692aa:
            add esi, 8
            inc edi
            cmp esi, offset gINT_00688748
            jl L00469295
            call ShutdownNetIfRequired
            mov edx, offset gNumber_of_races
            mov ecx, offset gRace_list
            push -1
            mov dword ptr [gNet_mode], 0
            call LoadRaces
            mov ecx, dword ptr [gDev_initial_race]
            call InitGame
            mov eax, dword ptr [gValid_stashed_save_game]
            test eax, eax
            je L0046930f
            push -1
            mov edx, offset gNumber_of_races
            mov ecx, offset gRace_list
            call LoadRaces
            mov ecx, offset gStashed_save_game
            call DoLoadGame2
            mov dword ptr [gValid_stashed_save_game], 0
    L0046930f:
            pop edi
            pop esi
            xor eax, eax
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

int C2_HOOK_FASTCALL DoMultiplayerStartStuff(tNet_mode pNet_mode) {

    if (pNet_mode == eNet_mode_thinking_about_it) {
        if (gGame_to_join == NULL) {
            return 0;
        }
        SolidPolyFontText("Joining a game", 200, 100, kPolyfont_hand_green_15pt_lit, eJust_left, 1);
        gFrontend_net_car_index = -1;

        do {
            gNet_join_host_result = 0;

            if (ChooseNetCar(gCurrent_net_game, &gFrontend_net_options, &gFrontend_net_car_index, 0)) {
                InitNetStorageSpace();
                gNet_join_host_result = NetJoinGame(gGame_to_join, gProgram_state.player_name, gFrontend_net_car_index);
                if (gNet_join_host_result) {
                    DisposeNetStorageSpace();
                } else {
                    LoadRaces(gRace_list, &gNumber_of_races, gCurrent_net_game->type);
                    SetUpOtherNetThings(gGame_to_join);
                    ReenableNetService();
                    strcpy(gFrontend_host_join_buffer, gProgram_state.player_name);
                    strcat(gFrontend_host_join_buffer, " ");
                    strcat(gFrontend_host_join_buffer, GetMiscString(eMiscString_has_joined_the_game));
                    NetSendHeadupToAllPlayers(gFrontend_host_join_buffer);
                    gNet_join_host_result = 1;
                }
            }
        } while (gNet_join_host_result == -4);
        if (gNet_join_host_result > 0) {
            return 1;
        }
        NetDisposeGameDetails(gGame_to_join);
    } else {
        gFrontend_net_car_index = -1;
        gRace_index = gProgram_state.current_race_index;
        if (ChooseNetCar(gCurrent_net_game, &gFrontend_net_options, &gFrontend_net_car_index, 1)) {
            InitNetStorageSpace();
            if (NetHostGame(gFrontend_game_type, &gFrontend_net_options, gRace_index,
                             gProgram_state.player_name, gFrontend_net_car_index)) {

                SetUpOtherNetThings(gCurrent_net_game);
                ReenableNetService();
                return 1;
            }
            DisposeNetStorageSpace();
            ReenableNetService();
            NetLeaveGame(gCurrent_net_game);
        }
    }
    return 0;
}

int C2_HOOK_FASTCALL NetworkJoinGoAhead(tFrontend_spec* pFrontend) {
    int i;

    ClearAlwaysTyping();
    if (gLast_graph_sel < 0) {
        return 0;
    }
    for (i = 0; i < gLast_graph_sel; i++) {
        tNet_game_details* game = gGames_to_join[i].game;

        if (game != NULL) {
            NetDisposeGameDetails(game);
            gGames_to_join[i].game = NULL;
        }
    }
    SaveOptions();
    gGame_to_join = gGames_to_join[gLast_graph_sel].game;
    if (DoMultiplayerStartStuff(gFrontend_net_mode) != 0) {
        return 1;
    }
    InitGamesToJoin();
    gLast_graph_sel = -1;
    NetStartProducingJoinList(AddToJoinList);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00468460
int C2_HOOK_FASTCALL StartNetworkGame(tFrontend_spec* pFrontend) {
    int i;
    int selection;

    (void)pFrontend;
    ClearAlwaysTyping();
    if (gLast_graph_sel < 0) {
        return 0;
    }
    NetEndJoinList();
    selection = gLast_graph_sel;
    {
        tJoinable_game* game;

        for (i = 0, game = gGames_to_join; (int)game < (int)&gGames_to_join[CARPOCALYPSE2_ASIZE(gGames_to_join)]; i++, game++) {
            if (i != selection && game->game != NULL) {
                NetDisposeGameDetails(game->game);
                game->game = NULL;
            }
        }
    }
    SaveOptions();
    gGame_to_join = gGames_to_join[gLast_graph_sel].game;
    if (gFrontend_net_mode == eNet_mode_thinking_about_it) {
        if (gGame_to_join != NULL) {
            SolidPolyFontText("Joining a game", 200, 100, kPolyfont_hand_green_15pt_lit, eJust_left, 1);
            gFrontend_net_car_index = -1;
            do {
                int i;

                gNet_join_host_result = 0;
                for (i = 0; i < kMax_netplayers; i++) {
                    gCurrent_race.opponent_list[i].car_spec = NULL;
                }
                gNet_storage_space_initialized = 1;
                InitialiseStorageSpace(0, &gNet_cars_storage_space, 2000, 50, 2000, 2000, 50);
                gNet_join_host_result = NetJoinGame(gGame_to_join, gProgram_state.player_name,
                                                    gFrontend_net_car_index);
                if (gNet_join_host_result == 0) {
                    tNet_game_details* game_to_join;

                    LoadRaces(gRace_list, &gNumber_of_races, gCurrent_net_game->type);
                    game_to_join = gGame_to_join;
                    gProgram_state.skill_level = 1;
                    InitGame(game_to_join->start_race);
                    ReenableNetService();
                    gJoined_game_notification = GetMiscString(eMiscString_has_joined_the_game);
                    strcpy(gFrontend_host_join_buffer, gProgram_state.player_name);
                    strcat(gFrontend_host_join_buffer, " ");
                    strcat(gFrontend_host_join_buffer, gJoined_game_notification);
                    NetSendHeadupToAllPlayers(gFrontend_host_join_buffer);
                    gNet_join_host_result = 1;
                } else {
                    if (gNet_storage_space_initialized != 0) {
                        DisposeStorageSpace(&gNet_cars_storage_space);
                    }
                    gNet_storage_space_initialized = 0;
                }
            } while (gNet_join_host_result == -4);
            if (gNet_join_host_result > 0) {
                goto success;
            }
            NetDisposeGameDetails(gGame_to_join);
        }
    } else {
        gFrontend_net_car_index = -1;
        gRace_index = gProgram_state.current_race_index;
        gChosen_net_game_options = &gFrontend_net_options;
        SetNetAvailability(&gFrontend_net_options);
        gFrontend_net_car_index = PickARandomCar();
        if (gFrontend_net_car_index >= 0) {
            gCar_details[gFrontend_net_car_index].ownership = eCar_owner_someone;
        }
        InitNetStorageSpace();
        if (NetHostGame(gFrontend_game_type, &gFrontend_net_options, gRace_index,
                        gProgram_state.player_name, gFrontend_net_car_index) != NULL) {
            gProgram_state.skill_level = 1;
            {
                tNet_game_details* current_game;

                current_game = gCurrent_net_game;
                InitGame(current_game->start_race);
            }
            ReenableNetService();
            goto success;
        }
        if (gNet_storage_space_initialized != 0) {
            DisposeStorageSpace(&gNet_cars_storage_space);
        }
        gNet_storage_space_initialized = 0;
        ReenableNetService();
        NetLeaveGame(gCurrent_net_game);
    }
    {
        tJoinable_game* game;

        for (game = gGames_to_join; (int)game < (int)&gGames_to_join[CARPOCALYPSE2_ASIZE(gGames_to_join)]; game++) {
            game->game = NULL;
        }
    }
    gLast_graph_sel = -1;
    NetStartProducingJoinList(AddToJoinList);
    return 0;

success:
    return 1;
}

// FUNCTION: CARMA2_HW 0x00468e80
int C2_NAKED C2_HOOK_FASTCALL NetworkStartHost(tFrontend_spec* pFrontend) {
#ifndef CARPOCALYPSE2_MATCHING

    gFrontend_net_mode = eNet_mode_host;
    return NetworkJoinGoAhead(pFrontend);
#else
    __asm {
            push ebx
            push ebp
            push esi
            push edi
            mov dword ptr [gFrontend_net_mode], 2
            call ClearAlwaysTyping
            mov eax, dword ptr [gLast_graph_sel]
            xor ebp, ebp
            cmp eax, ebp
            jl L0046918b
            call NetEndJoinList
            mov ebx, dword ptr [gLast_graph_sel]
            xor edi, edi
            mov esi, offset gGames_to_join
    L00468eb4:
            cmp edi, ebx
            je L00468ec5
            mov ecx, dword ptr [esi]
            cmp ecx, ebp
            je L00468ec5
            call NetDisposeGameDetails
            mov dword ptr [esi], ebp
    L00468ec5:
            add esi, 8
            inc edi
            cmp esi, offset gINT_00688748
            jl L00468eb4
            call SaveOptions
            mov eax, dword ptr [gLast_graph_sel]
            mov ecx, dword ptr [gFrontend_net_mode]
            mov ebx, 1
            mov eax, dword ptr [eax*8 + gGames_to_join]
            cmp ecx, ebx
            mov dword ptr [gGame_to_join], eax
            jne L00469090
            cmp eax, ebp
            je L00469166
            push ebx
            push ebp
            push ebx
            push 0x64
            mov edx, 0xc8
            mov ecx, offset gFrontend_QUIT+0xb94c
            call SolidPolyFontText
            mov dword ptr [gFrontend_net_car_index], 0xffffffff
    L00468f20:
            mov dword ptr [gNet_join_host_result], ebp
            mov eax, offset gCurrent_race+0xcf0
    L00468f2b:
            mov dword ptr [eax], ebp
            add eax, 0x10
            cmp eax, offset gCurrent_race+0xdb0
            jl L00468f2b
            push 0x32
            push 0x7d0
            push 0x7d0
            push 0x32
            push 0x7d0
            mov edx, offset gNet_cars_storage_space
            xor ecx, ecx
            mov dword ptr [gNet_storage_space_initialized], ebx
            call InitialiseStorageSpace
            mov ecx, dword ptr [gFrontend_net_car_index]
            mov edx, offset gProgram_state+0x1a10
            push ecx
            mov ecx, dword ptr [gGame_to_join]
            call NetJoinGame
            cmp eax, ebp
            mov dword ptr [gNet_join_host_result], eax
            jne L00469052
            mov edx, dword ptr [gCurrent_net_game]
            mov ecx, offset gRace_list
            mov eax, dword ptr [edx + 0x74]
            mov edx, offset gNumber_of_races
            push eax
            call LoadRaces
            mov eax, dword ptr [gGame_to_join]
            mov dword ptr [gProgram_state+0x20], ebx
            mov ecx, dword ptr [eax + 0x38]
            call InitGame
            call ReenableNetService
            mov ecx, 0x3a
            call GetMiscString
            mov edx, eax
            or ecx, 0xffffffff
            mov edi, offset gProgram_state+0x1a10
            xor eax, eax
            repne scasb
            not ecx
            sub edi, ecx
            mov dword ptr [gJoined_game_notification], edx
            mov eax, ecx
            mov esi, edi
            shr ecx, 2
            mov edi, offset gFrontend_host_join_buffer
            rep movsd
            mov ecx, eax
            xor eax, eax
            and ecx, 3
            rep movsb
            mov edi, offset gCamera_incident_direction+0xc
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            mov esi, edi
            mov ebp, ecx
            mov edi, offset gFrontend_host_join_buffer
            or ecx, 0xffffffff
            repne scasb
            mov ecx, ebp
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, ebp
            and ecx, 3
            rep movsb
            mov edi, edx
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            mov esi, edi
            mov edx, ecx
            mov edi, offset gFrontend_host_join_buffer
            or ecx, 0xffffffff
            repne scasb
            mov ecx, edx
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, edx
            and ecx, 3
            rep movsb
            mov ecx, offset gFrontend_host_join_buffer
            call NetSendHeadupToAllPlayers
            mov eax, ebx
            xor ebp, ebp
            mov dword ptr [gNet_join_host_result], eax
            jmp L0046906f
    L00469052:
            cmp dword ptr [gNet_storage_space_initialized], ebp
            je L00469069
            mov ecx, offset gNet_cars_storage_space
            call DisposeStorageSpace
            mov eax, dword ptr [gNet_join_host_result]
    L00469069:
            mov dword ptr [gNet_storage_space_initialized], ebp
    L0046906f:
            cmp eax, -4
            je L00468f20
            cmp eax, ebp
            jg L00469137
            mov ecx, dword ptr [gGame_to_join]
            call NetDisposeGameDetails
            jmp L00469166
    L00469090:
            mov eax, dword ptr [gProgram_state+0x78]
            mov ecx, dword ptr [gCurrent_net_game]
            push ebx
            push offset gFrontend_net_car_index
            mov edx, offset gFrontend_net_options
            mov dword ptr [gFrontend_net_car_index], 0xffffffff
            mov dword ptr [gRace_index], eax
            call ChooseNetCar
            test eax, eax
            je L00469166
            mov eax, offset gCurrent_race+0xcf0
    L004690c7:
            mov dword ptr [eax], ebp
            add eax, 0x10
            cmp eax, offset gCurrent_race+0xdb0
            jl L004690c7
            push 0x32
            push 0x7d0
            push 0x7d0
            push 0x32
            push 0x7d0
            mov edx, offset gNet_cars_storage_space
            xor ecx, ecx
            mov dword ptr [gNet_storage_space_initialized], ebx
            call InitialiseStorageSpace
            mov ecx, dword ptr [gFrontend_net_car_index]
            mov edx, dword ptr [gRace_index]
            push ecx
            mov ecx, dword ptr [gFrontend_game_type]
            push offset gProgram_state+0x1a10
            push edx
            mov edx, offset gFrontend_net_options
            call NetHostGame
            test eax, eax
            je L0046913e
            mov eax, dword ptr [gCurrent_net_game]
            mov dword ptr [gProgram_state+0x20], ebx
            mov ecx, dword ptr [eax + 0x38]
            call InitGame
            call ReenableNetService
    L00469137:
            mov eax, ebx
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret
    L0046913e:
            cmp dword ptr [gNet_storage_space_initialized], ebp
            je L00469150
            mov ecx, offset gNet_cars_storage_space
            call DisposeStorageSpace
    L00469150:
            mov dword ptr [gNet_storage_space_initialized], ebp
            call ReenableNetService
            mov ecx, dword ptr [gCurrent_net_game]
            call NetLeaveGame
    L00469166:
            mov eax, offset gGames_to_join
    L0046916b:
            mov dword ptr [eax], ebp
            add eax, 8
            cmp eax, offset gINT_00688748
            jl L0046916b
            mov ecx, offset AddToJoinList
            mov dword ptr [gLast_graph_sel], 0xffffffff
            call NetStartProducingJoinList
    L0046918b:
            pop edi
            pop esi
            pop ebp
            xor eax, eax
            pop ebx
            ret
    }
#endif
}

void C2_HOOK_FASTCALL NetworkUpdateSelectedGameInfo(tFrontend_spec* pFrontend) {
    int netgame_idx;

    netgame_idx = 0;
    if (gFrontend_selected_item_index >= pFrontend->scrollers[2].indexFirstScrollableItem
            && (gFrontend_selected_item_index <= pFrontend->scrollers[2].indexLastScrollableItem)) {
        netgame_idx = gFrontend_selected_item_index - pFrontend->scrollers[2].indexFirstScrollableItem;
    }
    if (gGames_to_join[netgame_idx].game != NULL
            && gGames_to_join[netgame_idx].game->type >= 0
            && gGames_to_join[netgame_idx].game->type < 8) {

        size_t len1;
        size_t len2;
        size_t len3;
        char s[256];

        if (PDGetTotalTime() - gGames_to_join[netgame_idx].time >= 15000) {
            DisposeJoinableGame(netgame_idx);
            return;
        }
        len1 = sprintf(s, "Game: %s @R", gGames_to_join[netgame_idx].game->pd_net_info.addr + 14);
        len2 = sprintf(s + len1, "Type: %s @R", GetMiscString(eMiscString_network_type_start + gGames_to_join[netgame_idx].game->type));
        len3 = sprintf(s + len1 + len2, "Players: %d @R", gGames_to_join[netgame_idx].game->num_players);
        sprintf(s + len1 + len2 + len3, "Status: %s, %s ",
                GetMiscString(eMiscString_netgame_stage_start + gGames_to_join[netgame_idx].game->status.stage),
                GetMiscString(gGames_to_join[netgame_idx].game->options.open_game ? eMiscString_open : eMiscString_closed));
        sprintf(pFrontend->items[30].text, "%s", s);
        MungeMetaCharactersChar(pFrontend->items[30].text, 'R', '\r');
    }
}

// FUNCTION: CARMA2_HW 0x004682c0
int C2_NAKED C2_HOOK_FASTCALL NetJoinChooseThisGame(tFrontend_spec* pFrontend) {
#ifndef CARPOCALYPSE2_MATCHING
    tJoinable_game* jgame;

    gLast_graph_sel = gFrontend_selected_item_index - pFrontend->scrollers[2].indexFirstScrollableItem;
    jgame = &gGames_to_join[gLast_graph_sel];
    if (gLast_graph_sel < 0 || jgame->game == NULL || (!jgame->game->options.open_game && !jgame->game->no_races_yet) || jgame->game->num_players >= kMax_netplayers) {
        gLast_graph_sel = -1;
    }
    pFrontend->items[17].enabled = kFrontendItemEnabled_disabled;
    NetworkUpdateSelectedGameInfo(pFrontend);
    return 0;
#else
    __asm {
            mov eax, dword ptr [gFrontend_selected_item_index]
            sub esp, 0x100
            mov edx, 0xffffffff
            push ebx
            mov ebx, ecx
            push ebp
            push esi
            mov ecx, dword ptr [ebx + 0x880c]
            push edi
            sub eax, ecx
            js L00468306
            mov ecx, dword ptr [eax*8 + gGames_to_join]
            test ecx, ecx
            je L00468306
            mov esi, dword ptr [ecx + 0x54]
            test esi, esi
            jne L004682f9
            mov esi, dword ptr [ecx + 0x3c]
            test esi, esi
            je L00468306
    L004682f9:
            mov esi, dword ptr [ecx + 0x34]
            mov dword ptr [gLast_graph_sel], eax
            cmp esi, 0xc
            jl L0046830c
    L00468306:
            mov dword ptr [gLast_graph_sel], edx
    L0046830c:
            mov ecx, dword ptr [ebx + 0x880c]
            mov dword ptr [ebx + 0x1830], edx
            mov eax, dword ptr [gFrontend_selected_item_index]
            xor esi, esi
            cmp eax, ecx
            jl L0046832f
            cmp eax, dword ptr [ebx + 0x8810]
            jg L0046832f
            sub eax, ecx
            mov esi, eax
    L0046832f:
            mov eax, dword ptr [esi*8 + gGames_to_join]
            test eax, eax
            je L0046844c
            mov eax, dword ptr [eax + 0x74]
            test eax, eax
            jl L0046844c
            cmp eax, 7
            jg L0046844c
            call PDGetTotalTime
            sub eax, dword ptr [esi*8 + gGames_to_join+0x4]
            cmp eax, 0x3a98
            jb L00468389
            mov ecx, dword ptr [esi*8 + gGames_to_join]
            call NetDisposeGameDetails
            mov dword ptr [esi*8 + gGames_to_join], 0
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x100
            ret
    L00468389:
            mov eax, dword ptr [esi*8 + gGames_to_join]
            lea ecx, [esp + 0x10]
            add eax, 0xe
            push eax
            push offset gFrontend_QUIT+0xb940
            push ecx
            call sprintf
            mov edx, dword ptr [esi*8 + gGames_to_join]
            add esp, 0xc
            mov edi, eax
            mov ecx, dword ptr [edx + 0x74]
            add ecx, 0x3b
            call GetMiscString
            push eax
            lea eax, [esp + edi + 0x14]
            push offset gFrontend_QUIT+0xb934
            push eax
            call sprintf
            mov ecx, dword ptr [esi*8 + gGames_to_join]
            add esp, 0xc
            add edi, eax
            mov edx, dword ptr [ecx + 0x34]
            push edx
            lea eax, [esp + edi + 0x14]
            push offset gFrontend_QUIT+0xb924
            push eax
            call sprintf
            mov ecx, dword ptr [esi*8 + gGames_to_join]
            add esp, 0xc
            add edi, eax
            mov ecx, dword ptr [ecx + 0x40]
            add ecx, 0x44
            call GetMiscString
            mov edx, dword ptr [esi*8 + gGames_to_join]
            mov ebp, eax
            mov ecx, dword ptr [edx + 0x54]
            add ecx, 0x47
            call GetMiscString
            push eax
            push ebp
            lea eax, [esp + edi + 0x18]
            push offset gFrontend_QUIT+0xb910
            push eax
            call sprintf
            add esp, 0x10
            lea ecx, [esp + 0x10]
            lea esi, [ebx + 0x29d9]
            push ecx
            push offset gFrontend_QUIT+0xb90c
            push esi
            call sprintf
            add esp, 0xc
            mov dl, 0x52
            mov ecx, esi
            push 0xd
            call MungeMetaCharactersChar
    L0046844c:
            pop edi
            pop esi
            pop ebp
            xor eax, eax
            pop ebx
            add esp, 0x100
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004670c0
int C2_HOOK_FASTCALL NetGameToggleTyping(tFrontend_spec* pFrontend) {
    ToggleTyping(pFrontend);
    if (!gTyping && strlen(pFrontend->items[gFrontend_selected_item_index].text) != 0) {

        strcpy(gProgram_state.player_name, pFrontend->items[gFrontend_selected_item_index].text);
        SaveOptions();
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00468a50
int C2_NAKED C2_HOOK_FASTCALL NetworkStartJoin(tFrontend_spec* pFrontend) {
#ifndef CARPOCALYPSE2_MATCHING

    gFrontend_net_mode = eNet_mode_thinking_about_it;
    return NetworkJoinGoAhead(pFrontend);
#else
    __asm {
            push ebx
            push ebp
            push esi
            mov ebp, 1
            push edi
            mov dword ptr [gFrontend_net_mode], ebp
            call ClearAlwaysTyping
            mov eax, dword ptr [gLast_graph_sel]
            test eax, eax
            jl L00468d66
            call NetEndJoinList
            mov ebx, dword ptr [gLast_graph_sel]
            xor edi, edi
            mov esi, offset gGames_to_join
    L00468a83:
            cmp edi, ebx
            je L00468a98
            mov ecx, dword ptr [esi]
            test ecx, ecx
            je L00468a98
            call NetDisposeGameDetails
            mov dword ptr [esi], 0
    L00468a98:
            add esi, 8
            inc edi
            cmp esi, offset gINT_00688748
            jl L00468a83
            call SaveOptions
            mov eax, dword ptr [gLast_graph_sel]
            mov ecx, dword ptr [gFrontend_net_mode]
            cmp ecx, ebp
            mov eax, dword ptr [eax*8 + gGames_to_join]
            mov dword ptr [gGame_to_join], eax
            jne L00468c60
            xor esi, esi
            cmp eax, esi
            je L00468d41
            push ebp
            push esi
            push ebp
            push 0x64
            mov edx, 0xc8
            mov ecx, offset gFrontend_QUIT+0xb94c
            call SolidPolyFontText
            mov dword ptr [gFrontend_net_car_index], 0xffffffff
    L00468af0:
            mov dword ptr [gNet_join_host_result], esi
            mov eax, offset gCurrent_race+0xcf0
    L00468afb:
            mov dword ptr [eax], esi
            add eax, 0x10
            cmp eax, offset gCurrent_race+0xdb0
            jl L00468afb
            push 0x32
            push 0x7d0
            push 0x7d0
            push 0x32
            push 0x7d0
            mov edx, offset gNet_cars_storage_space
            xor ecx, ecx
            mov dword ptr [gNet_storage_space_initialized], ebp
            call InitialiseStorageSpace
            mov ecx, dword ptr [gFrontend_net_car_index]
            mov edx, offset gProgram_state+0x1a10
            push ecx
            mov ecx, dword ptr [gGame_to_join]
            call NetJoinGame
            cmp eax, esi
            mov dword ptr [gNet_join_host_result], eax
            jne L00468c22
            mov edx, dword ptr [gCurrent_net_game]
            mov ecx, offset gRace_list
            mov eax, dword ptr [edx + 0x74]
            mov edx, offset gNumber_of_races
            push eax
            call LoadRaces
            mov eax, dword ptr [gGame_to_join]
            mov dword ptr [gProgram_state+0x20], ebp
            mov ecx, dword ptr [eax + 0x38]
            call InitGame
            call ReenableNetService
            mov ecx, 0x3a
            call GetMiscString
            mov edx, eax
            or ecx, 0xffffffff
            mov edi, offset gProgram_state+0x1a10
            xor eax, eax
            repne scasb
            not ecx
            sub edi, ecx
            mov dword ptr [gJoined_game_notification], edx
            mov eax, ecx
            mov esi, edi
            shr ecx, 2
            mov edi, offset gFrontend_host_join_buffer
            rep movsd
            mov ecx, eax
            xor eax, eax
            and ecx, 3
            rep movsb
            mov edi, offset gCamera_incident_direction+0xc
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            mov esi, edi
            mov ebx, ecx
            mov edi, offset gFrontend_host_join_buffer
            or ecx, 0xffffffff
            repne scasb
            mov ecx, ebx
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, ebx
            and ecx, 3
            rep movsb
            mov edi, edx
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            mov esi, edi
            mov edx, ecx
            mov edi, offset gFrontend_host_join_buffer
            or ecx, 0xffffffff
            repne scasb
            mov ecx, edx
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, edx
            and ecx, 3
            rep movsb
            mov ecx, offset gFrontend_host_join_buffer
            call NetSendHeadupToAllPlayers
            mov eax, ebp
            xor esi, esi
            mov dword ptr [gNet_join_host_result], eax
            jmp L00468c3f
    L00468c22:
            cmp dword ptr [gNet_storage_space_initialized], esi
            je L00468c39
            mov ecx, offset gNet_cars_storage_space
            call DisposeStorageSpace
            mov eax, dword ptr [gNet_join_host_result]
    L00468c39:
            mov dword ptr [gNet_storage_space_initialized], esi
    L00468c3f:
            cmp eax, -4
            je L00468af0
            cmp eax, esi
            jg L00468d0b
            mov ecx, dword ptr [gGame_to_join]
            call NetDisposeGameDetails
            jmp L00468d41
    L00468c60:
            mov eax, dword ptr [gProgram_state+0x78]
            mov ecx, dword ptr [gCurrent_net_game]
            push ebp
            push offset gFrontend_net_car_index
            mov edx, offset gFrontend_net_options
            mov dword ptr [gFrontend_net_car_index], 0xffffffff
            mov dword ptr [gRace_index], eax
            call ChooseNetCar
            test eax, eax
            je L00468d3f
            mov eax, offset gCurrent_race+0xcf0
    L00468c97:
            mov dword ptr [eax], 0
            add eax, 0x10
            cmp eax, offset gCurrent_race+0xdb0
            jl L00468c97
            push 0x32
            push 0x7d0
            push 0x7d0
            push 0x32
            push 0x7d0
            mov edx, offset gNet_cars_storage_space
            xor ecx, ecx
            mov dword ptr [gNet_storage_space_initialized], ebp
            call InitialiseStorageSpace
            mov ecx, dword ptr [gFrontend_net_car_index]
            mov edx, dword ptr [gRace_index]
            push ecx
            mov ecx, dword ptr [gFrontend_game_type]
            push offset gProgram_state+0x1a10
            push edx
            mov edx, offset gFrontend_net_options
            call NetHostGame
            test eax, eax
            je L00468d12
            mov eax, dword ptr [gCurrent_net_game]
            mov dword ptr [gProgram_state+0x20], ebp
            mov ecx, dword ptr [eax + 0x38]
            call InitGame
            call ReenableNetService
    L00468d0b:
            mov eax, ebp
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret
    L00468d12:
            mov eax, dword ptr [gNet_storage_space_initialized]
            test eax, eax
            je L00468d25
            mov ecx, offset gNet_cars_storage_space
            call DisposeStorageSpace
    L00468d25:
            mov dword ptr [gNet_storage_space_initialized], 0
            call ReenableNetService
            mov ecx, dword ptr [gCurrent_net_game]
            call NetLeaveGame
    L00468d3f:
            xor esi, esi
    L00468d41:
            mov eax, offset gGames_to_join
    L00468d46:
            mov dword ptr [eax], esi
            add eax, 8
            cmp eax, offset gINT_00688748
            jl L00468d46
            mov ecx, offset AddToJoinList
            mov dword ptr [gLast_graph_sel], 0xffffffff
            call NetStartProducingJoinList
    L00468d66:
            pop edi
            pop esi
            pop ebp
            xor eax, eax
            pop ebx
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004682a0
int C2_HOOK_FASTCALL NetToggleMode(tFrontend_spec* pFrontend) {
    (void)pFrontend;
    if (gFrontend_net_mode == eNet_mode_thinking_about_it) {
        gFrontend_net_mode = eNet_mode_host;
    } else {
        gFrontend_net_mode = eNet_mode_thinking_about_it;
    }
    WaitFor(6000);
    return 0;
}
