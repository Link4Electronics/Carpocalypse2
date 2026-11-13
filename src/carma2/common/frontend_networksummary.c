#include "frontend_networksummary.h"

#include "font.h"
#include "frontend.h"
#include "frontend_main.h"
#include "frontend_quit.h"
#include "globvars.h"
#include "globvrpb.h"
#include "globvrbm.h"
#include "graphics.h"
#include "network.h"
#include "polyfont.h"
#include "utility.h"

#include "c2_stdlib.h"
#include "c2_string.h"
// GLOBAL: CARMA2_HW 0x00686ec0
int gPlayer_lookup_netsummary[14];

// GLOBAL: CARMA2_HW 0x006864fc
int gFrontend_netsummary_first_iteration;


#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern tNet_game_player_info gNet_players[12];
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_FASTCALL EdgeTriggerModeOff(void);
extern void C2_HOOK_FASTCALL EdgeTriggerModeOn(void);
extern void C2_HOOK_FASTCALL FuckWithWidths(tFrontend_spec* pFrontend);
extern const char* C2_HOOK_FASTCALL GetMiscString(int pIndex);
extern const char* C2_HOOK_FASTCALL IString_Get(int pIndex);
extern void C2_HOOK_FASTCALL LoadMenuSettings(tFrontend_spec* pFrontend);
extern void C2_HOOK_FASTCALL WaitForNoKeys(void);
extern void __cdecl _ftol();
extern tConnected_items* gConnected_items;
extern tFrontend_slider* gCurrent_frontend_scrollbars;
extern tNet_game_details* gCurrent_net_game;
extern char gDouble_0058a068[];
extern tFrontend_spec gFrontend_QUIT;
extern int gFrontend_scrollbars_updated;
extern int gMouse_in_use;
extern int gNumber_of_net_players;
extern tStruct_00686508* gPTR_00686508;
extern char gPed_flags_0058f2e4[];
extern int gScore_winner;
#endif
// FUNCTION: CARMA2_HW 0x004743c0
static int C2_NAKED C2_HOOK_CDECL SortScores(const void* pArg1, const void* pArg2) {
#ifndef CARPOCALYPSE2_MATCHING

    return gNet_players[*(int*)pArg1].score - gNet_players[*(int*)pArg2].score;
#else
    __asm {
            mov eax, dword ptr [esp + 4]
            mov eax, dword ptr [eax]
            lea ecx, [eax + eax*2]
            lea edx, [ecx + ecx*8]
            shl edx, 1
            sub edx, eax
            mov eax, dword ptr [esp + 8]
            mov eax, dword ptr [eax]
            lea ecx, [eax + eax*2]
            lea ecx, [ecx + ecx*8]
            shl ecx, 1
            sub ecx, eax
            mov eax, dword ptr [ecx*4 + gNet_players+0x90]
            mov ecx, dword ptr [edx*4 + gNet_players+0x90]
            sub eax, ecx
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

static void C2_HOOK_FASTCALL SortGameScores(void) {

    qsort(gPlayer_lookup_netsummary, gNumber_of_net_players, sizeof(int), SortScores);
}

void C2_HOOK_FASTCALL NetworkSummarySetup(tFrontend_spec* pFrontend) {
    int i;

    strcpy(pFrontend->items[60].text, GetMiscString(eMiscString_player));
    strcpy(pFrontend->items[61].text, GetMiscString(eMiscString_played));
    strcpy(pFrontend->items[62].text, GetMiscString(eMiscString_won));
    strcpy(pFrontend->items[63].text, GetMiscString(eMiscString_score));
    strcpy(pFrontend->items[64].text, IString_Get(182));

    for (i = 0; i < gNumber_of_net_players; i++) {
        gPlayer_lookup_netsummary[i] = i;
    }
    SortGameScores();
    for (i = 0; i < gNumber_of_net_players; i++) {

        pFrontend->items[ 0 + i].visible = 1;
        pFrontend->items[12 + i].visible = 1;
        pFrontend->items[24 + i].visible = 1;
        pFrontend->items[36 + i].visible = 1;
        pFrontend->items[48 + i].visible = 1;
        pFrontend->items[65 + i].visible = 1;
        strcpy(pFrontend->items[0 + i].text, gNet_players[i].player_name);
        if (gNet_players[i].host) {
            strcat(pFrontend->items[0 + i].text, " (");
            strcat(pFrontend->items[0 + i].text, GetMiscString(eMiscString_host));
            strcat(pFrontend->items[0 + i].text, ")");
        }
        sprintf(pFrontend->items[12 + i].text, "%d", gNet_players[i].field_0x7c);
        sprintf(pFrontend->items[24 + i].text, "%d", gNet_players[i].field_0x88);
        sprintf(pFrontend->items[36 + i].text, "%d", gNet_players[i].score);
        sprintf(pFrontend->items[48 + i].text, "%d%%",
                   (int)((float)gNet_players[i].score / (float)(gNet_players[i].field_0x7c) / gScore_winner * 100.f));
    }
    for (i = gNumber_of_net_players; i < kMax_netplayers; i++) {
        pFrontend->items[ 0 + i].visible = 0;
        pFrontend->items[12 + i].visible = 0;
        pFrontend->items[24 + i].visible = 0;
        pFrontend->items[36 + i].visible = 0;
        pFrontend->items[48 + i].visible = 0;
        pFrontend->items[65 + i].visible = 0;

        pFrontend->items[ 0 + i].text[0] = '\0';
        pFrontend->items[12 + i].text[0] = '\0';
        pFrontend->items[24 + i].text[0] = '\0';
        pFrontend->items[36 + i].text[0] = '\0';
        pFrontend->items[48 + i].text[0] = '\0';
    }

    if (gCurrent_net_game->options.open_game) {
        sprintf(pFrontend->items[78].text, "%s: %s (%s)",
            GetMiscString(eMiscString_open),
            GetMiscString(eMiscString_network_type_start + gCurrent_net_game->type),
            GetMiscString(eMiscString_game_type));
    } else {
        sprintf(pFrontend->items[78].text, "%s: %s (%s)",
            GetMiscString(eMiscString_closed),
            GetMiscString(eMiscString_network_type_start + gCurrent_net_game->type),
            GetMiscString(eMiscString_game_type));
    }
}

// FUNCTION: CARMA2_HW 0x00473f40
int C2_NAKED C2_HOOK_FASTCALL NetSummary_Infunc(tFrontend_spec* pFrontend) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;

    Generic_Infunc(pFrontend);
    NetworkSummarySetup(pFrontend);
    for (i = 0; i < pFrontend->count_items; i++) {

        pFrontend->items[i].visible = 1;
    }
    FuckWithWidths(pFrontend);
    gMouse_in_use = 0;
    gFrontend_netsummary_first_iteration = 1;
    return 1;
#else
    __asm {
            sub esp, 0x10
            push ebx
            push ebp
            mov ebp, ecx
            xor ebx, ebx
            push esi
            push edi
            mov eax, dword ptr [ebp + 0xad1c]
            mov dword ptr [esp + 0x1c], ebp
            cmp eax, ebx
            jne L00473f79
            call LoadMenuSettings
            mov ecx, ebp
            call FuckWithWidths
            mov eax, dword ptr [ebp + 0x114]
            cmp eax, ebx
            je L00473f79
            mov dword ptr [eax + 0xb8c0], 1
    L00473f79:
            mov dword ptr [gFrontend_scrollbars_updated], ebx
            call EdgeTriggerModeOff
            call WaitForNoKeys
            call EdgeTriggerModeOn
            mov ecx, 0xd2
            mov dword ptr [gCurrent_frontend_scrollbars], ebx
            mov dword ptr [gConnected_items], ebx
            mov dword ptr [gPTR_00686508], ebx
            call GetMiscString
            mov edi, eax
            or ecx, 0xffffffff
            xor eax, eax
            lea edx, [ebp + 0x5229]
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
            mov ecx, 0xd3
            call GetMiscString
            mov edi, eax
            or ecx, 0xffffffff
            xor eax, eax
            lea edx, [ebp + 0x5381]
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
            mov ecx, 0xd4
            call GetMiscString
            mov edi, eax
            or ecx, 0xffffffff
            xor eax, eax
            lea edx, [ebp + 0x54d9]
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
            mov ecx, 0xd5
            call GetMiscString
            mov edi, eax
            or ecx, 0xffffffff
            xor eax, eax
            lea edx, [ebp + 0x5631]
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
            mov ecx, 0xb6
            call IString_Get
            mov edi, eax
            or ecx, 0xffffffff
            lea edx, [ebp + 0x5789]
            xor eax, eax
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
            mov edx, dword ptr [gNumber_of_net_players]
            cmp edx, ebx
            jle L004740a6
            mov ecx, offset gPlayer_lookup_netsummary
    L0047409c:
            mov dword ptr [ecx], eax
            inc eax
            add ecx, 4
            cmp eax, edx
            jl L0047409c
    L004740a6:
            push offset SortScores
            push 4
            push edx
            push offset gPlayer_lookup_netsummary
            call qsort
            mov ecx, dword ptr [gNumber_of_net_players]
            add esp, 0x10
            cmp ecx, ebx
            mov dword ptr [esp + 0x14], ebx
            jle L00474288
            mov dword ptr [esp + 0x10], 0x686ec0
            add ebp, 0x117c
    L004740db:
            mov ecx, dword ptr [esp + 0x10]
            mov eax, dword ptr [ecx]
            lea ecx, [eax + eax*2]
            lea edx, [ecx + ecx*8]
            or ecx, 0xffffffff
            shl edx, 1
            sub edx, eax
            mov eax, 1
            mov dword ptr [ebp - 0x1020], eax
            mov dword ptr [ebp], eax
            mov dword ptr [ebp + 0x1020], eax
            lea ebx, [edx*4 + gNet_players]
            mov dword ptr [ebp + 0x2040], eax
            mov dword ptr [ebp + 0x3060], eax
            mov dword ptr [ebp + 0x4738], eax
            lea edi, [ebx + 0x28]
            xor eax, eax
            lea edx, [ebp - 0xff3]
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
            mov eax, dword ptr [ebx + 0x20]
            test eax, eax
            je L004741de
            mov edi, offset gFrontend_QUIT+0xbd58
            or ecx, 0xffffffff
            xor eax, eax
            repne scasb
            not ecx
            sub edi, ecx
            mov eax, ecx
            mov esi, edi
            mov dword ptr [esp + 0x18], eax
            mov edi, edx
            mov edx, dword ptr [esp + 0x18]
            or ecx, 0xffffffff
            xor eax, eax
            repne scasb
            mov ecx, edx
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, edx
            and ecx, 3
            rep movsb
            mov ecx, 0x58
            call GetMiscString
            mov edi, eax
            or ecx, 0xffffffff
            xor eax, eax
            repne scasb
            not ecx
            sub edi, ecx
            mov esi, edi
            mov edx, ecx
            lea edi, [ebp - 0xff3]
            or ecx, 0xffffffff
            repne scasb
            mov ecx, edx
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, edx
            and ecx, 3
            rep movsb
            mov edi, offset gFrontend_QUIT+0xbd54
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            mov esi, edi
            mov edx, ecx
            lea edi, [ebp - 0xff3]
            or ecx, 0xffffffff
            repne scasb
            mov ecx, edx
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, edx
            and ecx, 3
            rep movsb
    L004741de:
            mov eax, dword ptr [ebx + 0x7c]
            lea ecx, [ebp + 0x2d]
            push eax
            push offset gPed_flags_0058f2e4+0x134
            push ecx
            call sprintf
            mov edx, dword ptr [ebx + 0x88]
            add esp, 0xc
            lea eax, [ebp + 0x104d]
            push edx
            push offset gPed_flags_0058f2e4+0x134
            push eax
            call sprintf
            mov ecx, dword ptr [ebx + 0x90]
            add esp, 0xc
            lea edx, [ebp + 0x206d]
            push ecx
            push offset gPed_flags_0058f2e4+0x134
            push edx
            call sprintf
            fild dword ptr [ebx + 0x90]
            fild dword ptr [ebx + 0x7c]
            add esp, 0xc
            fdivp st(1), st(0)
            fidiv dword ptr [gScore_winner]
            fmul qword ptr [gDouble_0058a068+0x90]
            call _ftol
            push eax
            lea eax, [ebp + 0x308d]
            push offset gFrontend_QUIT+0xbd4c
            push eax
            call sprintf
            mov eax, dword ptr [esp + 0x20]
            mov edi, dword ptr [esp + 0x1c]
            mov ecx, dword ptr [gNumber_of_net_players]
            add esp, 0xc
            inc eax
            add edi, 4
            add ebp, 0x158
            cmp eax, ecx
            mov dword ptr [esp + 0x14], eax
            mov dword ptr [esp + 0x10], edi
            jl L004740db
            mov ebp, dword ptr [esp + 0x1c]
            xor ebx, ebx
    L00474288:
            cmp ecx, 0xc
            jge L004742f0
            mov eax, ecx
            shl eax, 3
            sub eax, ecx
            lea edx, [eax + eax*2]
            lea eax, [ecx + edx*2]
            mov edx, 0xc
            sub edx, ecx
            lea eax, [ebp + eax*8 + 0x11a9]
    L004742a8:
            mov byte ptr [eax - 0x1020], 0
            mov byte ptr [eax], 0
            mov byte ptr [eax + 0x1020], 0
            mov byte ptr [eax + 0x2040], 0
            mov byte ptr [eax + 0x3060], 0
            mov dword ptr [eax - 0x104d], ebx
            mov dword ptr [eax - 0x2d], ebx
            mov dword ptr [eax + 0xff3], ebx
            mov dword ptr [eax + 0x2013], ebx
            mov dword ptr [eax + 0x3033], ebx
            mov dword ptr [eax + 0x470b], ebx
            add eax, 0x158
            dec edx
            jne L004742a8
    L004742f0:
            mov ecx, dword ptr [gCurrent_net_game]
            cmp dword ptr [ecx + 0x54], ebx
            je L00474331
            mov ecx, 0x48
            call GetMiscString
            mov edx, dword ptr [gCurrent_net_game]
            push eax
            mov ecx, dword ptr [edx + 0x74]
            add ecx, 0x3b
            call GetMiscString
            mov ecx, 0xcf
            push eax
            call GetMiscString
            push eax
            lea eax, [ebp + 0x6a59]
            push offset gFrontend_QUIT+0xbd2c
            push eax
            jmp L00474365
    L00474331:
            mov ecx, 0x47
            call GetMiscString
            mov ecx, dword ptr [gCurrent_net_game]
            push eax
            mov ecx, dword ptr [ecx + 0x74]
            add ecx, 0x3b
            call GetMiscString
            mov ecx, 0xcf
            push eax
            call GetMiscString
            lea edx, [ebp + 0x6a59]
            push eax
            push offset gFrontend_QUIT+0xbd2c
            push edx
    L00474365:
            call sprintf
            mov ecx, dword ptr [ebp + 0x104]
            add esp, 0x14
            xor eax, eax
            cmp ecx, ebx
            jle L00474396
            lea ecx, [ebp + 0x15c]
    L0047437f:
            mov dword ptr [ecx], 1
            mov edx, dword ptr [ebp + 0x104]
            inc eax
            add ecx, 0x158
            cmp eax, edx
            jl L0047437f
    L00474396:
            mov ecx, ebp
            call FuckWithWidths
            mov eax, 1
            mov dword ptr [gMouse_in_use], ebx
            pop edi
            pop esi
            pop ebp
            mov dword ptr [gFrontend_netsummary_first_iteration], eax
            pop ebx
            add esp, 0x10
            ret
    }
#endif
}

#pragma inline_depth(0)
// FUNCTION: CARMA2_HW 0x0044bac0
void C2_HOOK_FASTCALL DrawThisCarIconNow(int pCar_index, int pX, int pY) {
    gCar_icons_model_actor->material->colour_map = gTexture_maps[gCar_icons[pCar_index].index];
    BrMaterialUpdate(gCar_icons_model_actor->material, BR_MATU_COLOURMAP);
    gCar_icons_model_actor->model = gCar_icons[pCar_index].model;
    BrMatrix34Translate(&gCar_icons_model_actor->t.t.mat, (float)pX, (float)-pY, 0.f);
    BrActorAdd(g2d_camera, gCar_icons_model_actor);
    BrZbsSceneRender(g2d_camera, g2d_camera, gBack_screen, gDepth_buffer);
    BrActorRemove(gCar_icons_model_actor);
}
#pragma inline_depth()

#pragma inline_depth(0)
// FUNCTION: CARMA2_HW 0x00474400
int C2_HOOK_FASTCALL NetSummary_MenuHandler(tFrontend_spec* pFrontend) {
    int result;
    int i;
    int player_index;

    if (gFrontend_netsummary_first_iteration) {

        gFrontend_selected_item_index = 79;
        gFrontend_netsummary_first_iteration = 0;
    }
    if (gProgram_state.prog_status == eProg_idling) {
        return 1;
    }
    result = Generic_MenuHandler(pFrontend);
    for (i = 0; i < gNumber_of_net_players; i++) {

        player_index = gPlayer_lookup_netsummary[i];
        pFrontend->items[player_index].visible = 1;
        pFrontend->items[12 + player_index].visible = 1;
        pFrontend->items[24 + player_index].visible = 1;
        pFrontend->items[36 + player_index].visible = 1;
        pFrontend->items[48 + player_index].visible = 1;
        pFrontend->items[65 + player_index].visible = 1;
        DrawThisCarIconNow(gNet_players[player_index].car_index, 88, 112 + 24 * i);
    }
    for (i = gNumber_of_net_players; i < kMax_netplayers; i++) {

        pFrontend->items[0 + i].visible = 0;
        pFrontend->items[12 + i].visible = 0;
        pFrontend->items[24 + i].visible = 0;
        pFrontend->items[36 + i].visible = 0;
        pFrontend->items[48 + i].visible = 0;
        pFrontend->items[65 + i].visible = 0;
    }
    return result;
}
#pragma inline_depth()