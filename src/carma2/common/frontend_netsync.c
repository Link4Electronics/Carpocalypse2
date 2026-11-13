#include "frontend_netsync.h"

#include "frontend.h"
#include "frontend_main.h"
#include "frontend_network.h"
#include "frontend_networksummary.h"
#include "frontend_quit.h"
#include "globvars.h"
#include "globvrpb.h"
#include "graphics.h"
#include "input.h"
#include "main.h"
#include "netgame.h"
#include "network.h"
#include "platform.h"
#include "racestrt.h"
#include "utility.h"

#include <stdio.h>
#include "c2_string.h"
// GLOBAL: CARMA2_HW 0x006864d0
tU32 gNet_synch_start;


#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_FASTCALL DrawThisCarIconNow(int pCar_index, int pX, int pY);
extern void C2_HOOK_FASTCALL EdgeTriggerModeOff(void);
extern void C2_HOOK_FASTCALL EdgeTriggerModeOn(void);
extern int C2_HOOK_FASTCALL EitherMouseButtonDown(void);
extern int C2_HOOK_FASTCALL FRONTEND_CreateMenu(tFrontend_spec* pFrontend_spec);
extern int C2_HOOK_FASTCALL FRONTEND_DestroyMenu(tFrontend_spec* pFrontend);
extern int C2_HOOK_FASTCALL FindPrevActiveItem(tFrontend_spec* pFrontend, int pStart_index);
extern void C2_HOOK_FASTCALL FuckWithWidths(tFrontend_spec* pFrontend);
extern int C2_HOOK_FASTCALL GetItemAtMousePos(tFrontend_spec *pFrontend, int pX, int pY);
extern const char* C2_HOOK_FASTCALL GetMiscString(int pIndex);
extern void C2_HOOK_FASTCALL GetMousePosition(int *pX, int *pY);
extern const char* C2_HOOK_FASTCALL IString_Get(int pIndex);
extern void C2_HOOK_FASTCALL KillSplashScreen(void);
extern void C2_HOOK_FASTCALL LoadMenuSettings(tFrontend_spec* pFrontend);
extern void C2_HOOK_FASTCALL Morph_Initialise(tFrontend_spec* pCurrent, tFrontend_spec* pNext);
extern tNet_message* C2_HOOK_FASTCALL NetBuildGuaranteedMessage(tU8 pNet_message_type, int pOption);
extern void C2_HOOK_FASTCALL NetGuaranteedSendMessageToHost(tNet_game_details* pDetails, tNet_message* pMessage, void* pNotifyFail);
extern void C2_HOOK_FASTCALL NetPlayerStatusChanged(tPlayer_status pNew_status);
extern int C2_HOOK_FASTCALL PDAnyKeyDown(void);
extern tU32 C2_HOOK_FASTCALL PDGetTotalTime(void);
extern int C2_HOOK_FASTCALL PDKeyDown(int pKey_index);
extern void C2_HOOK_FASTCALL PollKeys(void);
extern void C2_HOOK_FASTCALL ServiceGame(void);
extern void C2_HOOK_FASTCALL SetUpNetCarPositions(void);
extern void C2_HOOK_FASTCALL SignalToStartRace(void);
extern void C2_HOOK_FASTCALL SolidPolyFontText(const char* pText, int pX, int pY, int pFont, tJustification pJust, int pRender);
extern void C2_HOOK_FASTCALL ToggleSelection(tFrontend_spec* pFrontend);
extern void C2_HOOK_FASTCALL UpdateScrollPositions(tFrontend_spec* pFrontend);
extern void C2_HOOK_FASTCALL WaitForNoKeys(void);
extern tConnected_items* gConnected_items;
extern tFrontend_slider* gCurrent_frontend_scrollbars;
extern tNet_game_details* gCurrent_net_game;
extern tFrontend_spec gFrontend_QUIT;
extern int gFrontend_leave_current_menu;
extern tNet_mode gFrontend_net_mode;
extern int gFrontend_scrollbars_updated;
extern int gFrontend_selected_item_index;
extern tU32 gFrontend_time_last_input;
extern int gLast_host_query;
extern int gMouse_in_use;
extern tNet_mode gNet_mode;
extern tNet_game_player_info gNet_players[12];
extern int gNo_races_yet;
extern int gNumber_of_net_players;
extern tStruct_00686508* gPTR_00686508;
extern tProgram_state gProgram_state;
extern int gStart_race_sent;
extern int gThis_net_player_index;
extern int gTyping_slot;
#endif
// FUNCTION: CARMA2_HW 0x00473570
int C2_HOOK_FASTCALL NetSynchRaceStart2(tNet_synch_mode pMode) {

    if (pMode != eNet_synch_client) {
        if (gCurrent_net_game->status.stage == 0) {
            gCurrent_net_game->status.stage = 1;
        }
        SetUpNetCarPositions();
        gNet_synch_start = PDGetTotalTime();
    }
    return 3;
}

// FUNCTION: CARMA2_HW 0x00473610
int C2_NAKED C2_HOOK_FASTCALL NetSync_Infunc(tFrontend_spec* pFrontend) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;

    Generic_Infunc(pFrontend);
    CheckPlayersAreResponding();
    if (gFrontend_net_mode == eNet_mode_host) {
        if (gNo_races_yet) {
            NetSynchRaceStart2(eNet_synch_host_first);
        } else {
            NetSynchRaceStart2(eNet_synch_host_subsequent);
        }
    } else {
        NetSynchRaceStart2(eNet_synch_client);
    }
    for (i = 0; i < kMax_netplayers; i++) {

        pFrontend->items[2 + i].x = 160;
    }
    if (gCurrent_net_game->options.open_game) {
        sprintf(pFrontend->items[1].text, "%s: %s (%s)",
            GetMiscString(eMiscString_game_type),
            GetMiscString(eMiscString_network_type_start + gCurrent_net_game->type),
            GetMiscString(eMiscString_open));
    } else {
        sprintf(pFrontend->items[1].text, "%s: %s (%s)",
            GetMiscString(eMiscString_game_type),
            GetMiscString(eMiscString_network_type_start + gCurrent_net_game->type),
            GetMiscString(eMiscString_closed));
    }
    if (gNet_mode == eNet_mode_host) {
        if (gNo_races_yet) {
            pFrontend->items[26].enabled = kFrontendItemEnabled_enabled;
            pFrontend->items[26].visible = 1;
            pFrontend->items[26].stringId = 14;
            pFrontend->items[27].enabled = kFrontendItemEnabled_enabled;
            pFrontend->items[27].visible = 1;
            pFrontend->items[27].stringId = 21;
        } else {
            pFrontend->items[26].enabled = kFrontendItemEnabled_disabled;
            pFrontend->items[26].visible = 0;
            pFrontend->items[27].enabled = kFrontendItemEnabled_enabled;
            pFrontend->items[27].visible = 1;
            pFrontend->items[27].stringId = 21;
        }
    } else {
        pFrontend->items[26].enabled = kFrontendItemEnabled_disabled;
        pFrontend->items[26].visible = 0;
        pFrontend->items[27].enabled = kFrontendItemEnabled_enabled;
        pFrontend->items[27].visible = 1;
        pFrontend->items[27].stringId = 179;
    }
    FuckWithWidths(pFrontend);
    return 0;
#else
    __asm {
            push ebx
            push ebp
            push esi
            mov esi, ecx
            xor ebx, ebx
            push edi
            mov eax, dword ptr [esi + 0xad1c]
            mov ebp, 1
            cmp eax, ebx
            jne L00473643
            call LoadMenuSettings
            mov ecx, esi
            call FuckWithWidths
            mov eax, dword ptr [esi + 0x114]
            cmp eax, ebx
            je L00473643
            mov dword ptr [eax + 0xb8c0], ebp
    L00473643:
            mov dword ptr [gFrontend_scrollbars_updated], ebx
            call EdgeTriggerModeOff
            call WaitForNoKeys
            call EdgeTriggerModeOn
            mov dword ptr [gCurrent_frontend_scrollbars], ebx
            mov dword ptr [gConnected_items], ebx
            mov dword ptr [gPTR_00686508], ebx
            call PDGetTotalTime
            mov edx, dword ptr [gNumber_of_net_players]
            mov edi, eax
            xor eax, eax
            cmp edx, ebx
            jle L004736b1
            mov ebx, dword ptr [gThis_net_player_index]
            mov ecx, offset gNet_players+0x48
    L00473688:
            cmp eax, ebx
            je L004736a4
            mov ebp, edi
            sub ebp, dword ptr [ecx - 0x34]
            cmp ebp, 0xea60
            jbe L0047369f
            mov dword ptr [ecx], 5
    L0047369f:
            mov ebp, 1
    L004736a4:
            inc eax
            add ecx, 0xd4
            cmp eax, edx
            jl L00473688
            xor ebx, ebx
    L004736b1:
            cmp dword ptr [gNet_mode], 3
            jne L004736df
            cmp dword ptr [gLast_host_query], ebx
            jne L004736df
            xor edx, edx
            mov cl, 0xe
            call NetBuildGuaranteedMessage
            mov ecx, dword ptr [gCurrent_net_game]
            mov edx, eax
            push ebx
            call NetGuaranteedSendMessageToHost
            mov dword ptr [gLast_host_query], edi
    L004736df:
            mov eax, dword ptr [gFrontend_net_mode]
            mov edi, 2
            cmp eax, edi
            jne L0047371a
            mov eax, dword ptr [gNo_races_yet]
            cmp eax, ebx
            mov eax, dword ptr [gCurrent_net_game]
            mov ecx, dword ptr [eax + 0x40]
            je L00473704
            cmp ecx, ebx
            jne L0047370b
            jmp L00473708
    L00473704:
            cmp ecx, ebx
            jne L0047370b
    L00473708:
            mov dword ptr [eax + 0x40], ebp
    L0047370b:
            call SetUpNetCarPositions
            call PDGetTotalTime
            mov dword ptr [gNet_synch_start], eax
    L0047371a:
            lea eax, [esi + 0x3f8]
            mov ecx, 0xc
    L00473725:
            mov dword ptr [eax], 0xa0
            add eax, 0x158
            dec ecx
            jne L00473725
            mov eax, dword ptr [gCurrent_net_game]
            cmp dword ptr [eax + 0x54], ebx
            je L00473773
            mov ecx, 0x48
            call GetMiscString
            mov ecx, dword ptr [gCurrent_net_game]
            push eax
            mov ecx, dword ptr [ecx + 0x74]
            add ecx, 0x3b
            call GetMiscString
            mov ecx, 0xcf
            push eax
            call GetMiscString
            lea edx, [esi + 0x2e1]
            push eax
            push offset gFrontend_QUIT+0xbd2c
            push edx
            jmp L004737a6
    L00473773:
            mov ecx, 0x47
            call GetMiscString
            push eax
            mov eax, dword ptr [gCurrent_net_game]
            mov ecx, dword ptr [eax + 0x74]
            add ecx, 0x3b
            call GetMiscString
            mov ecx, 0xcf
            push eax
            call GetMiscString
            lea ecx, [esi + 0x2e1]
            push eax
            push offset gFrontend_QUIT+0xbd2c
            push ecx
    L004737a6:
            call sprintf
            mov eax, dword ptr [gNet_mode]
            add esp, 0x14
            cmp eax, edi
            jne L0047382d
            cmp dword ptr [gNo_races_yet], ebx
            je L004737f9
            mov ecx, esi
            mov dword ptr [esi + 0x2448], ebp
            mov dword ptr [esi + 0x244c], ebp
            mov dword ptr [esi + 0x25a0], ebp
            mov dword ptr [esi + 0x25a4], ebp
            mov dword ptr [esi + 0x2420], 0xe
            mov dword ptr [esi + 0x2578], 0x15
            call FuckWithWidths
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret
    L004737f9:
            mov ecx, esi
            mov dword ptr [esi + 0x2448], 0xffffffff
            mov dword ptr [esi + 0x244c], ebx
            mov dword ptr [esi + 0x25a0], ebp
            mov dword ptr [esi + 0x25a4], ebp
            mov dword ptr [esi + 0x2578], 0x15
            call FuckWithWidths
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret
    L0047382d:
            mov ecx, esi
            mov dword ptr [esi + 0x2448], 0xffffffff
            mov dword ptr [esi + 0x244c], ebx
            mov dword ptr [esi + 0x25a0], ebp
            mov dword ptr [esi + 0x25a4], ebp
            mov dword ptr [esi + 0x2578], 0xb3
            call FuckWithWidths
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret
    }
#endif
}

void C2_HOOK_FASTCALL MungePlayers(tFrontend_spec* pFrontend) {
    int count_ready = 0;
    int i;

    CheckPlayersAreResponding();
    NetPlayerStatusChanged(ePlayer_status_ready);
    for (i = 0; i < gNumber_of_net_players; i++) {

        pFrontend->items[ 2 + i].visible = 1;
        pFrontend->items[ 2 + i].enabled = kFrontendItemEnabled_default;
        pFrontend->items[14 + i].visible = 1;
        pFrontend->items[14 + i].enabled = kFrontendItemEnabled_default;
        if (gNet_players[i].host) {
            sprintf(pFrontend->items[ 2 + i].text, "%s (%s)",
                gNet_players[i].player_name, IString_Get(154));
        } else {
            strcpy(pFrontend->items[ 2 + i].text, gNet_players[i].player_name);
        }
        if (gNet_players[i].player_status == ePlayer_status_ready) {
            count_ready += 1;
        }
    }
    for (i = gNumber_of_net_players; i < kMax_netplayers; i++) {

        pFrontend->items[ 2 + i].visible = 0;
        pFrontend->items[ 2 + i].enabled = kFrontendItemEnabled_default;
        pFrontend->items[14 + i].visible = 0;
        pFrontend->items[14 + i].enabled = kFrontendItemEnabled_default;
    }
    if (gNet_mode == eNet_mode_host
            && count_ready == gNumber_of_net_players
            && gNumber_of_net_players > 1
            && (!gNo_races_yet || gNumber_of_net_players == kMax_netplayers)) {

        SignalToStartRace();
        gStart_race_sent = 1;
    }
}

void C2_HOOK_FASTCALL NetSync_Draw(tFrontend_spec* pFrontend) {
    int i;
    char s[128];

    for (i = 0; i < gNumber_of_net_players; i++) {

        DrawThisCarIconNow(gNet_players[i].car_index, 115, 99 + 26 * i);
        SolidPolyFontText(GetMiscString(eMiscString_player_status_start + gNet_players[i].player_status),
            520, 96 + 26 * i, pFrontend->items[2 + i].unlitFont, eJust_right, 1);
    }
    if (gNet_mode != eNet_mode_host) {
        sprintf(s, "...%s...", IString_Get(250));
        SolidPolyFontText(s, 320, 96 + 26 * gNumber_of_net_players,
            pFrontend->items[2].unlitFont, eJust_centre, 1);
    }
}

// FUNCTION: CARMA2_HW 0x00473870
int C2_NAKED C2_HOOK_FASTCALL NetSync_MenuHandler(tFrontend_spec* pFrontend) {
#ifndef CARPOCALYPSE2_MATCHING
    int key;
    int mouse_down;
    int mouse_item;
    int original;

    if (gProgram_state.prog_status == eProg_idling) {
        return 1;
    }
    original = gFrontend_selected_item_index;
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
    mouse_down = 0;
    mouse_item = -1;
    if (gMouse_in_use) {
        int mouse_x;
        int mouse_y;

        ResetInterfaceTimeout();
        GetMousePosition(&mouse_x, &mouse_y);
        mouse_down = EitherMouseButtonDown();
        mouse_item = GetItemAtMousePos(gCurrent_frontend_spec, mouse_x, mouse_y);
        gFrontend_selected_item_index = mouse_item;
        if (mouse_item == -1) {
            gFrontend_selected_item_index = 99;
        }
    }
    if (PDKeyDown(72) || PDKeyDown(89))  {
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
    if (PDKeyDown(51) || PDKeyDown(52) || (mouse_down == 1 && mouse_item != -1)) {
        int result;

        result = pFrontend->items[original].field_0xc;
        ToggleSelection(pFrontend);
        if (pFrontend->items[gFrontend_selected_item_index].field_0xc == 2) {
            return 2;
        }
        if (pFrontend->items[gFrontend_selected_item_index].action != NULL) {
            result = pFrontend->items[gFrontend_selected_item_index].action(pFrontend);
        }
        if (pFrontend->items[gFrontend_selected_item_index].menuInfo != NULL) {
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

    ServiceGame();
    MungePlayers(pFrontend);
    NetSync_Draw(pFrontend);
    FuckWithWidths(pFrontend);
    if (gFrontend_leave_current_menu || gStart_race_sent || gProgram_state.prog_status == eProg_game_starting) {
        return 1;
    } else {
        return 0;
    }
#else
    __asm {
            sub esp, 0x94
            mov eax, dword ptr [gProgram_state+0xa4]
            push ebx
            push ebp
            mov ebx, ecx
            push esi
            cmp eax, 2
            push edi
            mov dword ptr [esp + 0x18], ebx
            jne L0047389a
            mov eax, 1
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x94
            ret
    L0047389a:
            mov ebp, dword ptr [gFrontend_selected_item_index]
            mov dword ptr [gTyping_slot], 0xffffffff
            call EdgeTriggerModeOn
            call KillSplashScreen
            call PollKeys
            call EdgeTriggerModeOff
            call PDAnyKeyDown
            xor edi, edi
            cmp eax, -1
            je L004738df
            cmp eax, 4
            je L004738df
            mov dword ptr [gMouse_in_use], edi
            call PDGetTotalTime
            mov dword ptr [gFrontend_time_last_input], eax
    L004738df:
            call EdgeTriggerModeOn
            mov eax, dword ptr [gMouse_in_use]
            mov dword ptr [esp + 0x10], edi
            cmp eax, edi
            je L0047393d
            call PDGetTotalTime
            lea edx, [esp + 0x1c]
            lea ecx, [esp + 0x20]
            mov dword ptr [gFrontend_time_last_input], eax
            call GetMousePosition
            call EitherMouseButtonDown
            mov edx, dword ptr [esp + 0x20]
            mov ecx, dword ptr [gCurrent_frontend_spec]
            mov dword ptr [esp + 0x10], eax
            mov eax, dword ptr [esp + 0x1c]
            push eax
            call GetItemAtMousePos
            cmp eax, -1
            mov dword ptr [gFrontend_selected_item_index], eax
            mov dword ptr [esp + 0x14], eax
            jne L0047393d
            mov dword ptr [gFrontend_selected_item_index], 0x63
    L0047393d:
            mov ecx, 0x48
            call PDKeyDown
            test eax, eax
            jne L0047395d
            mov ecx, 0x59
            call PDKeyDown
            test eax, eax
            je L004739f1
    L0047395d:
            mov ecx, dword ptr [gFrontend_selected_item_index]
            lea eax, [ecx - 1]
            mov ecx, dword ptr [gCurrent_frontend_spec]
            cmp eax, edi
            jl L00473998
            mov edx, eax
            shl edx, 3
            sub edx, eax
            lea edx, [edx + edx*2]
            lea edx, [eax + edx*2]
            lea edx, [ecx + edx*8 + 0x15c]
    L00473984:
            cmp dword ptr [edx - 4], edi
            jle L0047398d
            cmp dword ptr [edx], edi
            jne L004739a9
    L0047398d:
            dec eax
            sub edx, 0x158
            cmp eax, edi
            jge L00473984
    L00473998:
            mov edx, dword ptr [ecx + 0x104]
            call FindPrevActiveItem
            mov ecx, dword ptr [gCurrent_frontend_spec]
    L004739a9:
            mov edx, dword ptr [gMouse_in_use]
            mov dword ptr [gFrontend_selected_item_index], eax
            cmp edx, edi
            jne L004739f1
            cmp eax, dword ptr [ecx + 0x87a4]
            jl L004739f1
            cmp eax, dword ptr [ecx + 0x87a8]
            jg L004739f1
            mov ecx, ebx
            call ToggleSelection
            mov ecx, dword ptr [gFrontend_selected_item_index]
            mov eax, ecx
            shl eax, 3
            sub eax, ecx
            lea eax, [eax + eax*2]
            lea ecx, [ecx + eax*2]
            mov eax, dword ptr [ebx + ecx*8 + 0x134]
            cmp eax, edi
            je L004739f1
            mov ecx, ebx
            call eax
    L004739f1:
            mov ecx, 0x49
            call PDKeyDown
            test eax, eax
            jne L00473a11
            mov ecx, 0x53
            call PDKeyDown
            test eax, eax
            je L00473aab
    L00473a11:
            mov edx, dword ptr [ebx + 0x104]
            mov eax, dword ptr [gFrontend_selected_item_index]
            dec edx
            cmp eax, edx
            jge L00473eee
            mov esi, dword ptr [gCurrent_frontend_spec]
            inc eax
            mov ecx, dword ptr [esi + 0x104]
            lea edx, [ecx - 1]
            cmp eax, edx
            jg L00473a61
            mov ecx, eax
            shl ecx, 3
            sub ecx, eax
            lea ecx, [ecx + ecx*2]
            lea ecx, [eax + ecx*2]
            lea ecx, [esi + ecx*8 + 0x15c]
    L00473a4d:
            cmp dword ptr [ecx - 4], edi
            jle L00473a56
            cmp dword ptr [ecx], edi
            jne L00473a63
    L00473a56:
            inc eax
            add ecx, 0x158
            cmp eax, edx
            jle L00473a4d
    L00473a61:
            xor eax, eax
    L00473a63:
            mov ecx, dword ptr [gMouse_in_use]
            mov dword ptr [gFrontend_selected_item_index], eax
            cmp ecx, edi
            jne L00473aab
            cmp eax, dword ptr [esi + 0x87a4]
            jl L00473aab
            cmp eax, dword ptr [esi + 0x87a8]
            jg L00473aab
            mov ecx, ebx
            call ToggleSelection
            mov ecx, dword ptr [gFrontend_selected_item_index]
            mov eax, ecx
            shl eax, 3
            sub eax, ecx
            lea edx, [eax + eax*2]
            lea eax, [ecx + edx*2]
            mov eax, dword ptr [ebx + eax*8 + 0x134]
            cmp eax, edi
            je L00473aab
            mov ecx, ebx
            call eax
    L00473aab:
            mov ecx, 0x33
            call PDKeyDown
            test eax, eax
            jne L00473d84
            mov ecx, 0x34
            call PDKeyDown
            test eax, eax
            jne L00473d84
            cmp dword ptr [esp + 0x10], 1
            jne L00473ae1
            cmp dword ptr [esp + 0x14], -1
            jne L00473d84
    L00473ae1:
            call ServiceGame
            mov dword ptr [esp + 0x10], edi
            call PDGetTotalTime
            mov edx, dword ptr [gNumber_of_net_players]
            mov esi, eax
            xor eax, eax
            cmp edx, edi
            jle L00473b2c
            mov ebp, dword ptr [gThis_net_player_index]
            mov ecx, offset gNet_players+0x48
    L00473b08:
            cmp eax, ebp
            je L00473b21
            mov edi, esi
            sub edi, dword ptr [ecx - 0x34]
            cmp edi, 0xea60
            jbe L00473b1f
            mov dword ptr [ecx], 5
    L00473b1f:
            xor edi, edi
    L00473b21:
            inc eax
            add ecx, 0xd4
            cmp eax, edx
            jl L00473b08
    L00473b2c:
            cmp dword ptr [gNet_mode], 3
            jne L00473b5a
            cmp dword ptr [gLast_host_query], edi
            jne L00473b5a
            xor edx, edx
            mov cl, 0xe
            call NetBuildGuaranteedMessage
            mov ecx, dword ptr [gCurrent_net_game]
            mov edx, eax
            push edi
            call NetGuaranteedSendMessageToHost
            mov dword ptr [gLast_host_query], esi
    L00473b5a:
            mov ecx, 1
            call NetPlayerStatusChanged
            mov ecx, dword ptr [gNumber_of_net_players]
            mov dword ptr [esp + 0x14], edi
            cmp ecx, edi
            jle L00473c1d
            mov ebp, offset gNet_players+0x28
            add ebx, 0x142c
            jmp L00473b85
    L00473b83:
            xor edi, edi
    L00473b85:
            mov dword ptr [ebx - 0x1020], 1
            mov dword ptr [ebx], 1
            mov dword ptr [ebx - 0x1024], edi
            mov dword ptr [ebx - 4], edi
            cmp dword ptr [ebp - 8], edi
            jne L00473bca
            mov edi, ebp
            or ecx, 0xffffffff
            xor eax, eax
            lea edx, [ebx - 0xff3]
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
            jmp L00473bea
    L00473bca:
            mov ecx, 0x9a
            call IString_Get
            lea ecx, [ebx - 0xff3]
            push eax
            push ebp
            push offset gFrontend_QUIT+0xbd44
            push ecx
            call sprintf
            add esp, 0x10
    L00473bea:
            cmp dword ptr [ebp + 0x20], 1
            jne L00473bf4
            inc dword ptr [esp + 0x10]
    L00473bf4:
            mov eax, dword ptr [esp + 0x14]
            mov ecx, dword ptr [gNumber_of_net_players]
            inc eax
            add ebx, 0x158
            add ebp, 0xd4
            cmp eax, ecx
            mov dword ptr [esp + 0x14], eax
            jl L00473b83
            mov ebx, dword ptr [esp + 0x18]
            xor edi, edi
    L00473c1d:
            cmp ecx, 0xc
            jge L00473c5c
            mov eax, ecx
            shl eax, 3
            sub eax, ecx
            lea edx, [eax + eax*2]
            lea eax, [ecx + edx*2]
            mov edx, 0xc
            sub edx, ecx
            lea eax, [ebx + eax*8 + 0x142c]
    L00473c3d:
            mov dword ptr [eax - 0x1020], edi
            mov dword ptr [eax], edi
            mov dword ptr [eax - 0x1024], edi
            mov dword ptr [eax - 4], edi
            add eax, 0x158
            dec edx
            jne L00473c3d
            mov ecx, dword ptr [gNumber_of_net_players]
    L00473c5c:
            cmp dword ptr [gNet_mode], 2
            jne L00473c92
            cmp dword ptr [esp + 0x10], ecx
            jne L00473c92
            cmp ecx, 1
            jle L00473c92
            cmp dword ptr [gNo_races_yet], edi
            je L00473c7d
            cmp ecx, 0xc
            jne L00473c92
    L00473c7d:
            call SignalToStartRace
            mov ecx, dword ptr [gNumber_of_net_players]
            mov dword ptr [gStart_race_sent], 1
    L00473c92:
            xor ebx, ebx
            cmp ecx, edi
            jle L00473cf5
            mov ecx, dword ptr [esp + 0x18]
            mov edi, offset gNet_players+0x48
            mov esi, 0x60
            lea ebp, [ecx + 0x3f0]
    L00473cac:
            mov ecx, dword ptr [edi + 4]
            lea edx, [esi + 3]
            push edx
            mov edx, 0x73
            call DrawThisCarIconNow
            mov eax, dword ptr [ebp]
            mov ecx, dword ptr [edi]
            push 1
            push 1
            push eax
            push esi
            add ecx, 0x4d
            call GetMiscString
            mov ecx, eax
            mov edx, 0x208
            call SolidPolyFontText
            mov eax, dword ptr [gNumber_of_net_players]
            inc ebx
            add ebp, 0x158
            add esi, 0x1a
            add edi, 0xd4
            cmp ebx, eax
            jl L00473cac
    L00473cf5:
            cmp dword ptr [gNet_mode], 2
            je L00473d43
            mov ecx, 0xfa
            call IString_Get
            lea ecx, [esp + 0x24]
            push eax
            push offset gFrontend_QUIT+0xbd38
            push ecx
            call sprintf
            mov edx, dword ptr [esp + 0x24]
            add esp, 0xc
            lea ecx, [ebx + ebx*2]
            mov eax, dword ptr [edx + 0x3f0]
            push 1
            lea edx, [ebx + ecx*4]
            push 2
            push eax
            lea ecx, [esp + 0x30]
            lea eax, [edx + edx + 0x60]
            mov edx, 0x140
            push eax
            call SolidPolyFontText
    L00473d43:
            mov ecx, dword ptr [esp + 0x18]
            call FuckWithWidths
            mov eax, dword ptr [gFrontend_leave_current_menu]
            test eax, eax
            jne L00473d74
            mov eax, dword ptr [gStart_race_sent]
            test eax, eax
            jne L00473d74
            cmp dword ptr [gProgram_state+0xa4], 4
            je L00473d74
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x94
            ret
    L00473d74:
            mov eax, 1
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x94
            ret
    L00473d84:
            mov eax, ebp
            shl eax, 3
            sub eax, ebp
            lea ecx, [eax + eax*2]
            lea edx, [ebp + ecx*2]
            mov ecx, ebx
            mov esi, dword ptr [ebx + edx*8 + 0x13c]
            call ToggleSelection
            mov ecx, dword ptr [gFrontend_selected_item_index]
            mov eax, ecx
            shl eax, 3
            sub eax, ecx
            lea eax, [eax + eax*2]
            lea edx, [ecx + eax*2]
            lea eax, [ebx + edx*8]
            mov edx, dword ptr [ebx + edx*8 + 0x13c]
            cmp edx, 2
            jne L00473dd2
            mov eax, 2
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x94
            ret
    L00473dd2:
            mov eax, dword ptr [eax + 0x134]
            cmp eax, edi
            je L00473de8
            mov ecx, ebx
            call eax
            mov ecx, dword ptr [gFrontend_selected_item_index]
            mov esi, eax
    L00473de8:
            mov eax, ecx
            shl eax, 3
            sub eax, ecx
            lea eax, [eax + eax*2]
            lea edx, [ecx + eax*2]
            cmp dword ptr [ebx + edx*8 + 0x138], edi
            je L00473ee1
            mov eax, dword ptr [gCurrent_frontend_spec]
            xor edx, edx
            mov dword ptr [eax + 0x118], ecx
            mov eax, dword ptr [gCurrent_frontend_spec]
            cmp dword ptr [eax + 0x8790], edi
            jle L00473e3d
            xor ecx, ecx
    L00473e1e:
            mov ebp, dword ptr [eax + ecx + 0x87a0]
            inc edx
            mov dword ptr [eax + ecx + 0x87c0], ebp
            mov eax, dword ptr [gCurrent_frontend_spec]
            add ecx, 0x34
            cmp edx, dword ptr [eax + 0x8790]
            jl L00473e1e
    L00473e3d:
            mov ecx, ebx
            call FRONTEND_DestroyMenu
            mov ecx, dword ptr [gFrontend_selected_item_index]
            mov eax, ecx
            shl eax, 3
            sub eax, ecx
            lea edx, [eax + eax*2]
            lea eax, [ecx + edx*2]
            mov ecx, dword ptr [ebx + eax*8 + 0x138]
            mov dword ptr [gCurrent_frontend_spec], ecx
            call FRONTEND_CreateMenu
            mov edx, dword ptr [gCurrent_frontend_spec]
            mov eax, dword ptr [ebx + 0x114]
            cmp edx, eax
            je L00473e85
            mov dword ptr [edx + 0x114], ebx
            mov edx, dword ptr [gCurrent_frontend_spec]
    L00473e85:
            mov ecx, ebx
            call Morph_Initialise
            mov ecx, dword ptr [gCurrent_frontend_spec]
            cmp ecx, offset gFrontend_QUIT
            jne L00473ea2
            mov dword ptr [gFrontend_selected_item_index], edi
            jmp L00473eae
    L00473ea2:
            mov edx, dword ptr [ecx + 0x118]
            mov dword ptr [gFrontend_selected_item_index], edx
    L00473eae:
            mov eax, dword ptr [ecx + 0x8790]
            xor edx, edx
            cmp eax, edi
            jle L00473edc
            xor eax, eax
    L00473ebc:
            mov edi, dword ptr [ecx + eax + 0x87c0]
            inc edx
            mov dword ptr [ecx + eax + 0x87a0], edi
            mov ecx, dword ptr [gCurrent_frontend_spec]
            add eax, 0x34
            cmp edx, dword ptr [ecx + 0x8790]
            jl L00473ebc
    L00473edc:
            call UpdateScrollPositions
    L00473ee1:
            mov eax, esi
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x94
            ret
    L00473eee:
            mov esi, dword ptr [gCurrent_frontend_spec]
            mov dword ptr [gFrontend_selected_item_index], 0xffffffff
            xor ecx, ecx
            mov eax, dword ptr [esi + 0x104]
            lea edx, [eax - 1]
            cmp edx, edi
            jl L00473a61
            lea eax, [esi + 0x15c]
    L00473f17:
            cmp dword ptr [eax - 4], edi
            jle L00473f20
            cmp dword ptr [eax], edi
            jne L00473f2f
    L00473f20:
            inc ecx
            add eax, 0x158
            cmp ecx, edx
            jle L00473f17
            jmp L00473a61
    L00473f2f:
            mov eax, ecx
            jmp L00473a63
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004735a0
int C2_HOOK_FASTCALL NetSync_Start(tFrontend_spec* pFrontend) {

    if (gNumber_of_net_players <= 1) {
        return 0;
    }
    SignalToStartRace();
    gStart_race_sent = 1;
    gNo_races_yet = 0;
    return 1;
}

// FUNCTION: CARMA2_HW 0x004735d0
int C2_HOOK_FASTCALL NetSync_Abort(tFrontend_spec* pFrontend) {

    if (gProgram_state.racing || gNet_mode != eNet_mode_none) {

        if (gProgram_state.racing) {
            gAbandon_game = 1;
        }
        gProgram_state.prog_status = eProg_idling;
        gNo_current_game = 1;
    }
    return 1;
}