/*
 * Main menu spec + handlers (ported from frontend_main.c — real implementation).
 */
#include "interface-main.h"

#include "controls.h"
#include "interface.h"
#include "globvars.h"
#include "graphics2.h"
#include "input.h"
#include "loading1.h"
#include "loading2.h"
#include "main.h"
#include "network.h"
#include "platform.h"
#include "font.h"
#include "racestrt.h"
#include "utility.h"

#include "c2_string.h"


#include "controls.h"
#include "interface.h"
#include "globvars.h"
#include "globvrpb.h"
#include "graphics2.h"
#include "input.h"
#include "loading1.h"
#include "loading2.h"
#include "main.h"
#include "network.h"
#include "platform.h"
#include "font.h"
#include "racestrt.h"
#include "utility.h"

#include "c2_string.h"


/* forward declarations */
extern void C2_HOOK_FASTCALL MenuSetCarImage(int pCar_index, int pBrender_index);
extern void C2_HOOK_FASTCALL MenuSetDriverImage(int pOpponent_index, int pFrontend_index);
extern int C2_HOOK_FASTCALL RaceIndex(const char* pName);
extern int C2_HOOK_FASTCALL ProcessInputString(void);
extern int C2_HOOK_FASTCALL FindNextActiveItem(tFrontend_spec* pFrontend, int pStart_index);
extern void C2_HOOK_FASTCALL CheckForCheatingGits(void);
extern void C2_HOOK_FASTCALL AboutToLoadFirstCar(void);
extern void C2_HOOK_FASTCALL FillInRaceDescription(char* pDest, int pRace_index);
extern int C2_HOOK_CDECL sub_576c00(int pValue, int pDivisor);
extern tU32 C2_HOOK_FASTCALL FudgeBRenderIntoTheNinetiesWithSomeProperFuckingColourSupport(br_pixelmap*, tU32, tU32, tU32, tU32);
extern void C2_HOOK_FASTCALL FuckWithWidths(tFrontend_spec* pFrontend);
extern void C2_HOOK_FASTCALL ResetInterfaceTimeout(void);
extern void C2_HOOK_FASTCALL DefaultInfunc(tFrontend_spec* pFrontend);
int C2_HOOK_FASTCALL MainMenuInfunc(tFrontend_spec* pFrontend);
int C2_HOOK_FASTCALL MainMenuOutfunc(tFrontend_spec* pFrontend);
int C2_HOOK_FASTCALL FRONTEND_MainMenuHandler(tFrontend_spec* pFrontend);
int C2_HOOK_FASTCALL OnePlayerSetup(tFrontend_spec* pFrontend);
int C2_HOOK_FASTCALL MultiplayerSetup(tFrontend_spec* pFrontend);
int C2_HOOK_FASTCALL temp(tFrontend_spec* pFrontend);
int C2_HOOK_FASTCALL testUp(tFrontend_spec* pFrontend);
int C2_HOOK_FASTCALL testDn(tFrontend_spec* pFrontend);
int C2_HOOK_FASTCALL MainMenuSelectRace(tFrontend_spec* pFrontend);
int C2_HOOK_FASTCALL NewGameToggleTyping(tFrontend_spec* pFrontend);
int C2_HOOK_FASTCALL StartFudge(tFrontend_spec* pFrontend);
extern int gFrontend_leave_current_menu;
extern int gTyping_slot;
extern int gFrontend_suppress_mouse;
extern tInterface_spec* gCurrent_interface_spec;
extern tFrontendMenuType gFrontend_next_menu;
extern tFrontend_spec gFrontend_QUIT;
extern int gFrontend_change_car_selected_car;
extern int gFrontend_opponent_profile_pic_needs_update;
extern int gLast_host_query;
extern int gPending_race;
extern int gStart_race_sent;
extern tU32 gFrontend_last_scroll;
extern tU32 gFrontend_time_last_input;
extern tRace_group_spec* gRace_groups;
extern int gFrontend_scrollbars_updated;
extern tU32 gFrontend_time_last_input;
extern int gMouse_in_use;
extern br_actor* gFrontend_menu_camera;
extern br_actor* gFrontend_billboard_actors[8];
extern br_uint_32 gFrontend_APO_Colour_1;
extern br_uint_32 gFrontend_APO_Colour_2;
extern br_uint_32 gFrontend_APO_Colour_3;
extern tRace_group_spec* gCurrent_race_group;
extern tRace_list_spec gRace_list[100];
extern tProgram_state gProgram_state;
extern br_pixelmap* gFrontend_images[100];
extern tFrontend_brender_item gFrontend_brender_items[100];
extern int gFrontend_selected_item_index;
extern br_pixelmap* gRender_palette;
extern int gFrontend_count_brender_items;

extern tFrontend_spec gFrontend_OPTIONS;
extern tFrontend_spec gFrontend_NETWORK;
extern tFrontend_spec gFrontend_CHANGE_CAR;
extern tFrontend_spec gFrontend_NEWGAME;

// GLOBAL: CARMA2_HW 0x005a80f0
tFrontend_spec gFrontend_MAIN = {
    "Main",
    0,
    26,
    MainMenuInfunc,
    MainMenuOutfunc,
    FRONTEND_MainMenuHandler,
    &gFrontend_OPTIONS,
    0,
    0,
    0,
    2,
    2,
    1,
    {
        { 0x9,      OnePlayerSetup,      NULL,                          0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0xa,      MultiplayerSetup,    &gFrontend_NETWORK,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404,    temp,                NULL,                          0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x403,    testUp,              NULL,                          0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404,    MainMenuSelectRace,  NULL,                          0, 17, 18, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, },
        { 0x404,    MainMenuSelectRace,  NULL,                          0, 17, 18, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, },
        { 0x404,    MainMenuSelectRace,  NULL,                          0, 17, 18, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, },
        { 0x404,    MainMenuSelectRace,  NULL,                          0, 17, 18, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, },
        { 0x403,    testDn,              NULL,                          0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xf,      temp,                &gFrontend_OPTIONS,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x402,    temp,                &gFrontend_CHANGE_CAR,    0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x402,    temp,                &gFrontend_CHANGE_CAR,    0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x402,    temp,                &gFrontend_CHANGE_CAR,    0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x402,    temp,                &gFrontend_CHANGE_CAR,    0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x402,    temp,                &gFrontend_CHANGE_CAR,    0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x402,    temp,                &gFrontend_CHANGE_CAR,    0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xc,      temp,                NULL,                          0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404,    temp,                NULL,                          0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404,    NewGameToggleTyping, NULL,                          0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xec,     temp,                NULL,                          0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404,    temp,                NULL,                          0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0xe,      StartFudge,          &gFrontend_NEWGAME,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x401,    temp,                NULL,                          0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x401,    temp,                NULL,                          0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0xf7,     temp,                NULL,                          0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x401,    temp,                NULL,                          0, 17, 18, 0, 0, 0, 0, 0, 1, },
    },
    1,
    {
        { 1, 8, 6, 12, 12, 19 },
    },
};

// GLOBAL: CARMA2_HW 0x00687040
int gFrontend_car_image_outdated;

// GLOBAL: CARMA2_HW 0x0074c6b4
int gFrontend_net_initialized;

#ifdef CARPOCALYPSE2_MATCHING
extern br_actor* C2_HOOK_CDECL BrActorAllocate();
extern br_actor* C2_HOOK_FASTCALL CreateAPOactor(void);
extern int C2_HOOK_FASTCALL DRStricmp(const char* p1, const char* p2);
extern void C2_HOOK_FASTCALL EdgeTriggerModeOff(void);
extern void C2_HOOK_FASTCALL EdgeTriggerModeOn(void);
extern int C2_HOOK_FASTCALL EitherMouseButtonDown(void);
extern int C2_HOOK_FASTCALL FRONTEND_CreateMenu(tFrontend_spec* pFrontend_spec);
extern int C2_HOOK_FASTCALL FRONTEND_DestroyMenu(tFrontend_spec* pFrontend);
extern int C2_HOOK_FASTCALL FindPrevActiveItem(tFrontend_spec* pFrontend, int pStart_index);
extern int C2_HOOK_FASTCALL GetItemAtMousePos(tFrontend_spec *pFrontend, int pX, int pY);
extern void C2_HOOK_FASTCALL GetMousePosition(int *pX, int *pY);
extern const char* C2_HOOK_FASTCALL IString_Get(int pIndex);
extern void C2_HOOK_FASTCALL KillSplashScreen(void);
extern void C2_HOOK_FASTCALL LoadMenuSettings(tFrontend_spec* pFrontend);
extern void C2_HOOK_FASTCALL Morph_Initialise(tFrontend_spec* pCurrent, tFrontend_spec* pNext);
extern char* C2_HOOK_FASTCALL MungeCommas(int pValue);
extern void C2_HOOK_FASTCALL MungeMetaCharactersChar(char* pText, char pMeta, char pChar);
extern void C2_HOOK_FASTCALL MungeMetaCharactersNum(char* pText, char pMeta, int pNum);
extern int C2_HOOK_FASTCALL PDAnyKeyDown(void);
extern tU32 C2_HOOK_FASTCALL PDGetTotalTime(void);
extern int C2_HOOK_FASTCALL PDKeyDown(int pKey_index);
extern void C2_HOOK_FASTCALL PollKeys(void);
extern int C2_HOOK_FASTCALL PolyFontHeight(int pFont_index);
extern int C2_HOOK_FASTCALL PolyFontTextWidth(int pFont, const char* pText);
extern int C2_HOOK_FASTCALL SaveOptions(void);
extern void C2_HOOK_FASTCALL ServiceGame(void);
extern void C2_HOOK_FASTCALL SolidPolyFontText(const char* pText, int pX, int pY, int pFont, tJustification pJust, int pRender);
extern void C2_HOOK_FASTCALL ToggleSelection(tFrontend_spec* pFrontend);
extern int C2_HOOK_FASTCALL ToggleTyping(tFrontend_spec* pFrontend);
extern void C2_HOOK_FASTCALL TransparentPolyFontTextInABox(int pFont, const char* pText, int pLeft, int pTop, int pRight, int pBottom, tJustification pJust, undefined4 pUnknown, double pBlend);
extern void C2_HOOK_FASTCALL UpdateScrollPositions(tFrontend_spec* pFrontend);
extern void C2_HOOK_FASTCALL WaitForNoKeys(void);
extern br_pixelmap* gBack_screen;
extern char gFlic_bunch[];
extern int gIs_boundary_race;
extern tNet_mode gNet_mode;
extern char gNext_crush_actor_id[];
extern int gNumber_of_races;
extern tOpponent* gOpponents;
extern int gTyping;
#endif
// FUNCTION: CARMA2_HW 0x00576c00
int C2_NAKED C2_HOOK_CDECL sub_576c00(int pValue, int pDivisor) {
#ifndef CARPOCALYPSE2_MATCHING
    int quotient;
    int remainder;

    quotient = pValue / pDivisor;
    remainder = pValue % pDivisor;
    if (pValue < 0 && remainder > 0) {
        quotient += 1;
        remainder -= pDivisor;
    }
    return quotient;
#else
    __asm {
            mov ecx, dword ptr [esp + 4]
            push esi
            mov esi, dword ptr [esp + 0xc]
            mov eax, ecx
            cdq
            idiv esi
            push edi
            mov edi, eax
            mov eax, ecx
            cdq
            idiv esi
            test ecx, ecx
            jge L00576c21
            test edx, edx
            jle L00576c21
            inc edi
            sub edx, esi
    L00576c21:
            mov eax, edi
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
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00469a40
int C2_NAKED C2_HOOK_FASTCALL MainMenuInfunc(tFrontend_spec* pFrontend) {
#ifndef CARPOCALYPSE2_MATCHING
    int group;
    int race_index;
    int i;
    char group_name[12];
    br_camera* camera;

    if (pFrontend->loaded == 0) {
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
    gFrontend_time_last_input = PDGetTotalTime();
    group = (gCurrent_race_group - gRace_groups) % 10;
    sprintf(group_name, "%s %d", IString_Get(78), group + 1);
    strcpy(pFrontend->items[2].text, group_name);
    race_index = 4 * group;
    for (i = pFrontend->scrollers[0].indexFirstScrollableItem; i <= pFrontend->scrollers[0].indexLastScrollableItem; i++, race_index += 1) {
        tFrontend_item_spec* item = &pFrontend->items[i];

        strcpy(item->text, gRace_list[race_index].name);
        item->radioButton_selected = race_index == gProgram_state.current_race_index;
        if (gRace_list[race_index].is_boundary) {
            item->unlitFont = 2;
            item->highFont = 3;
        } else {
            item->unlitFont = 0;
            item->highFont = 1;
        }
    }
    if (gIs_boundary_race || gProgram_state.game_completed) {
        pFrontend->items[7].enabled = 1;
    } else {
        pFrontend->items[7].enabled = -1;
    }
    sub_576c00(gProgram_state.current_race_index, 4);
    strcpy(pFrontend->items[22].text, gRace_list[gProgram_state.current_race_index].description);
    MungeMetaCharactersChar(pFrontend->items[22].text, 'R', '\r');
    MungeMetaCharactersNum(pFrontend->items[22].text, 'O', gRace_list[gProgram_state.current_race_index].count_explicit_opponents);
    MungeMetaCharactersNum(pFrontend->items[22].text, 'L', gRace_list[gProgram_state.current_race_index].count_laps);
    strcpy(pFrontend->items[18].text, gProgram_state.player_name);
    strcpy(pFrontend->items[20].text, gOpponents[gProgram_state.current_car_index].car_name);
    strcpy(pFrontend->items[23].text, MungeCommas(gProgram_state.credits));
    gFrontend_car_image_outdated = 1;
    pFrontend->isPreviousSomeOtherMenu = 1;
    gMouse_in_use = 0;
    gFrontend_menu_camera = BrActorAllocate(BR_ACTOR_CAMERA, NULL);
    camera = gFrontend_menu_camera->type_data;
    camera->type = BR_CAMERA_PARALLEL;
    camera->field_of_view = BR_ANGLE_DEG(90);
    camera->hither_z = 1.f;
    camera->yon_z = 3.f;
    camera->width = 640.f;
    camera->height = 480.f;
    gFrontend_billboard_actors[0] = CreateAPOactor();
    gFrontend_APO_Colour_1 = FudgeBRenderIntoTheNinetiesWithSomeProperFuckingColourSupport(gFrontend_billboard_actors[0]->material->colour_map, 0xff, 0x80, 0x00, 0xff);
    gFrontend_APO_Colour_2 = FudgeBRenderIntoTheNinetiesWithSomeProperFuckingColourSupport(gFrontend_billboard_actors[0]->material->colour_map, 0x00, 0x00, 0x80, 0xff);
    gFrontend_APO_Colour_3 = FudgeBRenderIntoTheNinetiesWithSomeProperFuckingColourSupport(gFrontend_billboard_actors[0]->material->colour_map, 0x00, 0x00, 0xff, 0xff);
    FuckWithWidths(pFrontend);
    return 1;
#else
    __asm {
            sub esp, 0x18
            push ebx
            push ebp
            mov ebp, ecx
            push esi
            push edi
            mov eax, dword ptr [ebp + 0xad1c]
            test eax, eax
            jne L00469a73
            call LoadMenuSettings
            mov ecx, ebp
            call FuckWithWidths
            mov eax, dword ptr [ebp + 0x114]
            test eax, eax
            je L00469a73
            mov dword ptr [eax + 0xb8c0], 1
    L00469a73:
            mov dword ptr [gFrontend_scrollbars_updated], 0
            call EdgeTriggerModeOff
            call WaitForNoKeys
            call EdgeTriggerModeOn
            call PDGetTotalTime
            mov ecx, dword ptr [gCurrent_race_group]
            mov dword ptr [gFrontend_time_last_input], eax
            mov eax, dword ptr [gRace_groups]
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
            mov ecx, 0x4e
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
            lea ebx, [ebx*4 - 4]
            shr ecx, 2
            rep movsd
            mov ecx, eax
            mov dword ptr [esp + 0x10], ebx
            and ecx, 3
            rep movsb
            mov eax, dword ptr [ebp + 0x87a4]
            mov ecx, dword ptr [ebp + 0x87a8]
            cmp eax, ecx
            mov dword ptr [esp + 0x14], eax
            jg L00469bda
            lea ecx, [ebx + ebx*8]
            shl ecx, 4
            add ecx, ebx
            lea ebx, [ecx*4 + gRace_list+0x190]
            mov ecx, eax
            shl ecx, 3
            sub ecx, eax
            lea edx, [ecx + ecx*2]
            lea eax, [eax + edx*2]
            lea edx, [ebp + eax*8 + 0x16c]
    L00469b4b:
            lea edi, [ebx - 0x190]
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
            mov eax, dword ptr [ebx]
            test eax, eax
            je L00469ba0
            mov dword ptr [edx - 0x2c], 2
            mov dword ptr [edx - 0x28], 3
            jmp L00469bae
    L00469ba0:
            mov dword ptr [edx - 0x2c], 0
            mov dword ptr [edx - 0x28], 1
    L00469bae:
            mov ecx, dword ptr [esp + 0x10]
            mov eax, dword ptr [esp + 0x14]
            inc ecx
            add ebx, 0x244
            mov dword ptr [esp + 0x10], ecx
            mov ecx, dword ptr [ebp + 0x87a8]
            inc eax
            add edx, 0x158
            cmp eax, ecx
            mov dword ptr [esp + 0x14], eax
            jle L00469b4b
    L00469bda:
            mov ecx, dword ptr [gIs_boundary_race]
            mov eax, dword ptr [gProgram_state+0x78]
            test ecx, ecx
            jne L00469bff
            mov ecx, dword ptr [gProgram_state+0x84]
            test ecx, ecx
            jne L00469bff
            mov dword ptr [ebp + 0xac0], 0xffffffff
            jmp L00469c09
    L00469bff:
            mov dword ptr [ebp + 0xac0], 1
    L00469c09:
            push 4
            push eax
            call sub_576c00
            mov eax, dword ptr [gProgram_state+0x78]
            or ecx, 0xffffffff
            add esp, 8
            lea edx, [ebp + 0x1f19]
            lea ebx, [eax + eax*8]
            shl ebx, 4
            add ebx, eax
            xor eax, eax
            shl ebx, 2
            push 0xd
            lea edi, [ebx + gRace_list+0x80]
            repne scasb
            not ecx
            sub edi, ecx
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            mov dl, 0x52
            shr ecx, 2
            rep movsd
            mov ecx, eax
            and ecx, 3
            rep movsb
            lea esi, [ebp + 0x1f19]
            mov ecx, esi
            call MungeMetaCharactersChar
            mov ecx, dword ptr [ebx + gRace_list+0x194]
            mov dl, 0x4f
            push ecx
            mov ecx, esi
            call MungeMetaCharactersNum
            mov edx, dword ptr [ebx + gRace_list+0x1dc]
            mov ecx, esi
            push edx
            mov dl, 0x4c
            call MungeMetaCharactersNum
            mov edi, offset gProgram_state+0x1a10
            or ecx, 0xffffffff
            xor eax, eax
            lea edx, [ebp + 0x19b9]
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
            mov eax, dword ptr [gProgram_state+0x8c]
            mov edx, dword ptr [gOpponents]
            mov ecx, eax
            shl ecx, 3
            sub ecx, eax
            xor eax, eax
            shl ecx, 6
            lea edi, [ecx + edx + 0x90]
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            lea edx, [ebp + 0x1c69]
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            shr ecx, 2
            rep movsd
            mov ecx, eax
            and ecx, 3
            rep movsb
            mov ecx, dword ptr [gProgram_state]
            call MungeCommas
            mov edi, eax
            or ecx, 0xffffffff
            lea edx, [ebp + 0x2071]
            xor eax, eax
            repne scasb
            not ecx
            sub edi, ecx
            push 0
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            push 3
            shr ecx, 2
            rep movsd
            mov ecx, eax
            and ecx, 3
            rep movsb
            mov esi, 1
            mov dword ptr [gFrontend_car_image_outdated], esi
            mov dword ptr [ebp + 0xb8c0], esi
            mov dword ptr [gMouse_in_use], 0
            call BrActorAllocate
            mov dword ptr [gFrontend_menu_camera], eax
            mov eax, dword ptr [eax + 0x5c]
            add esp, 8
            mov byte ptr [eax + 4], 0
            mov word ptr [eax + 6], 0x3ffc
            mov dword ptr [eax + 8], 0x3f800000
            mov dword ptr [eax + 0xc], 0x40400000
            mov dword ptr [eax + 0x14], 0x44200000
            mov dword ptr [eax + 0x18], 0x43f00000
            call CreateAPOactor
            mov dword ptr [gFrontend_billboard_actors], eax
            mov ecx, dword ptr [eax + 0x1c]
            push 0xff
            push 0
            mov ecx, dword ptr [ecx + 0x40]
            push 0x80
            mov edx, 0xff
            call FudgeBRenderIntoTheNinetiesWithSomeProperFuckingColourSupport
            mov dword ptr [gFrontend_APO_Colour_1], eax
            mov eax, dword ptr [gFrontend_billboard_actors]
            push 0xff
            push 0x80
            mov ecx, dword ptr [eax + 0x1c]
            push 0
            xor edx, edx
            mov ecx, dword ptr [ecx + 0x40]
            call FudgeBRenderIntoTheNinetiesWithSomeProperFuckingColourSupport
            mov dword ptr [gFrontend_APO_Colour_2], eax
            mov eax, dword ptr [gFrontend_billboard_actors]
            push 0xff
            push 0xff
            mov ecx, dword ptr [eax + 0x1c]
            push 0
            xor edx, edx
            mov ecx, dword ptr [ecx + 0x40]
            call FudgeBRenderIntoTheNinetiesWithSomeProperFuckingColourSupport
            mov ecx, ebp
            mov dword ptr [gFrontend_APO_Colour_3], eax
            call FuckWithWidths
            mov eax, esi
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
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00469df0
int C2_HOOK_FASTCALL MainMenuOutfunc(tFrontend_spec* pFrontend) {
    br_actor* actor;

    actor = gFrontend_billboard_actors[0];
    if (actor != NULL) {
        if (actor->material->colour_map != NULL) {
            BrMapRemove(actor->material->colour_map);
            BrPixelmapFree(actor->material->colour_map);
        }
        if (actor->material != NULL) {
            BrMaterialRemove(actor->material);
            BrMaterialFree(actor->material);
        }
        if (actor->model != NULL) {
            BrModelRemove(actor->model);
            BrModelFree(actor->model);
        }
        if (actor->parent != NULL) {
            BrActorRemove(actor);
        }
        BrActorFree(actor);
    }
    BrActorFree(gFrontend_menu_camera);
    gFrontend_menu_camera = NULL;
    SaveOptions();
    return 1;
}

// FUNCTION: CARMA2_HW 0x00467050
int C2_HOOK_FASTCALL NewGameToggleTyping(tFrontend_spec* pFrontend) {
    ToggleTyping(pFrontend);
    if (!gTyping && strlen(pFrontend->items[gFrontend_selected_item_index].text) != 0) {

        strcpy(gProgram_state.player_name, pFrontend->items[gFrontend_selected_item_index].text);
        SaveOptions();
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0046a0e0
int C2_NAKED C2_HOOK_FASTCALL FRONTEND_MainMenuHandler(tFrontend_spec* pFrontend) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;

    if (gProgram_state.game_completed) {
        pFrontend->items[3].enabled = kFrontendItemEnabled_enabled;
        pFrontend->items[3].visible = 1;
        pFrontend->items[8].enabled = kFrontendItemEnabled_enabled;
        pFrontend->items[8].visible = 1;
    } else {
        pFrontend->items[3].enabled = kFrontendItemEnabled_default;
        pFrontend->items[3].visible = 1;
        pFrontend->items[8].enabled = kFrontendItemEnabled_default;
        pFrontend->items[8].visible = 1;
    }
    if (gIs_boundary_race || gProgram_state.game_completed) {
        pFrontend->items[7].enabled = kFrontendItemEnabled_enabled;
    } else {
        pFrontend->items[7].enabled = kFrontendItemEnabled_disabled;
    }

    TransparentPolyFontTextInABox(kPolyfont_hand_green_10pt_lit, "v1.02a",
        gBack_screen->width - PolyFontTextWidth(kPolyfont_hand_green_10pt_lit, "v1.02a") - 4,
        gBack_screen->height - PolyFontHeight(kPolyfont_hand_green_10pt_lit) - 4,
        gBack_screen->width,
        gBack_screen->height,
        eJust_left, 1, 150.);
    if (gFrontend_selected_item_index == 0) {
        pFrontend->default_item = 21;
        gFrontend_selected_item_index = 21;
    }
    if (gFrontend_car_image_outdated) {
        MenuSetCarImage(gProgram_state.current_car_index, 10);
        gFrontend_car_image_outdated = 0;
    }
    for (i = gCurrent_frontend_spec->scrollers[0].indexFirstScrollableItem; i <= gCurrent_frontend_spec->scrollers[0].indexLastScrollableItem; i++) {
        tFrontend_item_spec* item;
        int race_index;

        item = &pFrontend->items[i];
        race_index = RaceIndex(item->text);
        if (gRace_list[race_index].count_opponents != 0) {
            char buf[2];

            sprintf(buf, "%c", '\x1f');
            SolidPolyFontText(buf, item->x - 10, item->y, kPolyfont_hand_green_15pt_lit, eJust_centre, 1);
        }
    }
    gTyping_slot = -1;
    EdgeTriggerModeOn();
    KillSplashScreen();
    if (gTyping) {
        int input;

        input = ProcessInputString();
        FuckWithWidths(pFrontend);
        if (input < 0) {
            NewGameToggleTyping(pFrontend);
        }
        FuckWithWidths(pFrontend);
    } else {
        int key;
        int mouse_button;
        int item_at_mouse_pos = -1;

        PollKeys();
        EdgeTriggerModeOff();
        key = PDAnyKeyDown();
        if (key != -1 && key != 4) {
            gMouse_in_use = 0;
            ResetInterfaceTimeout();
        }
        EdgeTriggerModeOn();

        mouse_button = 0;
        if (gMouse_in_use) {
            int mouse_x;
            int mouse_y;

            gFrontend_selected_item_index = 0;
            ResetInterfaceTimeout();
            GetMousePosition(&mouse_x, &mouse_y);
            mouse_button = EitherMouseButtonDown();
            item_at_mouse_pos = GetItemAtMousePos(gCurrent_frontend_spec, mouse_x, mouse_y);
            gFrontend_selected_item_index = item_at_mouse_pos;
            if (gFrontend_selected_item_index == -1) {
                gFrontend_selected_item_index = 99;
            }
            if (gFrontend_selected_item_index >= 4 && gFrontend_selected_item_index <= 7
                    && pFrontend->items[gFrontend_selected_item_index].enabled > 0) {
                int race_index;

                race_index = RaceIndex(pFrontend->items[gFrontend_selected_item_index].text);
                FillInRaceDescription(pFrontend->items[22].text, race_index);
            }
        }
        if (PDKeyDown(72) || PDKeyDown(89)) {
            if (gFrontend_selected_item_index == 15) {
                gFrontend_selected_item_index = 10;
            }
            gFrontend_selected_item_index = FindPrevActiveItem(gCurrent_frontend_spec, gFrontend_selected_item_index);

            if (!gMouse_in_use) {
                if (gCurrent_frontend_spec->scrollers[0].indexFirstScrollableItem <= gFrontend_selected_item_index &&
                        gFrontend_selected_item_index <= gCurrent_frontend_spec->scrollers[0].indexLastScrollableItem) {

                    ToggleSelection(pFrontend);
                    if (pFrontend->items[gFrontend_selected_item_index].action != NULL) {
                        pFrontend->items[gFrontend_selected_item_index].action(pFrontend);
                    }
                }
            }
        }
        if (PDKeyDown(73) || PDKeyDown(83)) {

            if (gFrontend_selected_item_index < 21) {
                if (gFrontend_selected_item_index == 10) {
                    gFrontend_selected_item_index = 15;
                }
                gFrontend_selected_item_index = FindNextActiveItem(gCurrent_frontend_spec, gFrontend_selected_item_index);
            } else {
                gFrontend_selected_item_index = FindNextActiveItem(gCurrent_frontend_spec, -1);
            }
            if (!gMouse_in_use) {
                if (gFrontend_selected_item_index >= gCurrent_frontend_spec->scrollers[0].indexFirstScrollableItem &&
                        gFrontend_selected_item_index <= gCurrent_frontend_spec->scrollers[0].indexLastScrollableItem) {

                    ToggleSelection(pFrontend);
                    if (pFrontend->items[gFrontend_selected_item_index].action != NULL) {
                        pFrontend->items[gFrontend_selected_item_index].action(pFrontend);
                    }
                }
            }
        }
        if (PDKeyDown(63)) {
            gCurrent_frontend_spec->default_item = gFrontend_selected_item_index;

            for (i = 0; i < gCurrent_frontend_spec->count_scrollers; i++) {

                gCurrent_frontend_spec->scrollers[i].indexOfItemAtTop =
                    gCurrent_frontend_spec->scrollers[i].indexTopItem;
            }
            FRONTEND_DestroyMenu(pFrontend);
            gCurrent_frontend_spec = &gFrontend_OPTIONS;
            FRONTEND_CreateMenu(&gFrontend_OPTIONS);
            if (gCurrent_frontend_spec != pFrontend->previous) {
                gCurrent_frontend_spec->previous = pFrontend;
            }
            Morph_Initialise(pFrontend, gCurrent_frontend_spec);
            for (i = 0; i < gCurrent_frontend_spec->count_scrollers; i++) {

                gCurrent_frontend_spec->scrollers[i].indexTopItem = gCurrent_frontend_spec->scrollers[i].indexOfItemAtTop;
            }
            UpdateScrollPositions(gCurrent_frontend_spec);
            gFrontend_selected_item_index = gCurrent_frontend_spec->default_item;
            if (!gProgram_state.racing) {
                if (!(gCurrent_frontend_spec->default_item < 3 || gCurrent_frontend_spec->default_item > 30)) {
                    gFrontend_selected_item_index = 3;
                }
            } else {
                if (gNet_mode != eNet_mode_none && gCurrent_frontend_spec->default_item == 5) {
                    gFrontend_selected_item_index = 3;
                }
            }
        }
        if (PDKeyDown(51) || PDKeyDown(52) || (mouse_button != 0 && item_at_mouse_pos != -1)) {
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
                if (mouse_button == 2 && gFrontend_selected_item_index >= 4 && gFrontend_selected_item_index <= 7) {
                    if (gProgram_state.field_0x2c != 0) {
                        return 1;
                    }
                    gFrontend_next_menu = kFrontend_menu_newgame;
                    return 3;
                }
            } else {
                result = pFrontend->items[gFrontend_selected_item_index].field_0xc;
            }
            if (result == 5) {
                return 1;
            }
            if (pFrontend->items[gFrontend_selected_item_index].menuInfo == NULL) {
                return result;
            }
            gCurrent_frontend_spec->default_item = gFrontend_selected_item_index;
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

            if (gCurrent_frontend_spec == &gFrontend_OPTIONS) {
                if (!gProgram_state.racing) {
                    if (gFrontend_OPTIONS.default_item < 3) {
                        gFrontend_selected_item_index = 3;
                    }
                } else if (gNet_mode != eNet_mode_none && gFrontend_OPTIONS.default_item == 5) {
                    gFrontend_selected_item_index = 3;
                }
            }
            for (i = 0; i < gCurrent_frontend_spec->count_scrollers; i++) {
                gCurrent_frontend_spec->scrollers[i].indexTopItem = gCurrent_frontend_spec->scrollers[i].indexOfItemAtTop;
            }
            UpdateScrollPositions(gCurrent_frontend_spec);
            return result;
        }
    }
    ServiceGame();
    CheckForCheatingGits();
    return gFrontend_leave_current_menu != 0;
#else
    __asm {
            sub esp, 0x18
            push ebx
            push ebp
            mov ebp, ecx
            push esi
            mov ecx, dword ptr [gProgram_state+0x84]
            xor esi, esi
            cmp ecx, esi
            push edi
            mov dword ptr [esp + 0x14], ebp
            mov eax, 1
            jne L0046a118
            mov dword ptr [ebp + 0x560], esi
            mov dword ptr [ebp + 0x564], esi
            mov dword ptr [ebp + 0xc18], esi
            mov dword ptr [ebp + 0xc1c], esi
            jmp L0046a130
    L0046a118:
            mov dword ptr [ebp + 0x560], eax
            mov dword ptr [ebp + 0x564], eax
            mov dword ptr [ebp + 0xc18], eax
            mov dword ptr [ebp + 0xc1c], eax
    L0046a130:
            cmp dword ptr [gIs_boundary_race], esi
            jne L0046a14c
            cmp dword ptr [gProgram_state+0x84], esi
            jne L0046a14c
            mov dword ptr [ebp + 0xac0], 0xffffffff
            jmp L0046a152
    L0046a14c:
            mov dword ptr [ebp + 0xac0], eax
    L0046a152:
            push 0x4062c000
            push esi
            push eax
            mov eax, dword ptr [gBack_screen]
            xor ecx, ecx
            xor edx, edx
            mov cx, word ptr [eax + 0x36]
            mov dx, word ptr [eax + 0x34]
            push esi
            push ecx
            push edx
            mov ecx, 0xb
            call PolyFontHeight
            mov edx, dword ptr [gBack_screen]
            xor ecx, ecx
            mov cx, word ptr [edx + 0x36]
            mov edx, offset gFrontend_QUIT+0xb95c
            sub ecx, eax
            sub ecx, 4
            push ecx
            mov ecx, 0xb
            call PolyFontTextWidth
            mov edx, dword ptr [gBack_screen]
            xor ecx, ecx
            mov cx, word ptr [edx + 0x34]
            mov edx, offset gFrontend_QUIT+0xb95c
            sub ecx, eax
            sub ecx, 4
            push ecx
            mov ecx, 0xb
            call TransparentPolyFontTextInABox
            mov ecx, dword ptr [gFrontend_selected_item_index]
            mov eax, 0x15
            cmp ecx, esi
            jne L0046a1d3
            mov dword ptr [ebp + 0x118], eax
            mov dword ptr [gFrontend_selected_item_index], eax
    L0046a1d3:
            cmp dword ptr [gFrontend_car_image_outdated], esi
            je L0046a1f1
            mov ecx, dword ptr [gProgram_state+0x8c]
            mov edx, 0xa
            call MenuSetCarImage
            mov dword ptr [gFrontend_car_image_outdated], esi
    L0046a1f1:
            mov eax, dword ptr [gCurrent_frontend_spec]
            mov ecx, dword ptr [eax + 0x87a4]
            mov edx, dword ptr [eax + 0x87a8]
            cmp ecx, edx
            mov dword ptr [esp + 0x1c], ecx
            jg L0046a2c6
            mov eax, ecx
            mov edi, dword ptr [esp + 0x18]
            shl eax, 3
            sub eax, ecx
            lea eax, [eax + eax*2]
            lea ecx, [ecx + eax*2]
            lea ebx, [ebp + ecx*8 + 0x14c]
    L0046a226:
            mov eax, dword ptr [gNumber_of_races]
            xor esi, esi
            test eax, eax
            jle L0046a264
            lea ebp, [ebx + 0x3d]
            mov edi, offset gRace_list
    L0046a239:
            mov edx, edi
            mov ecx, ebp
            call DRStricmp
            test eax, eax
            je L0046a258
            mov eax, dword ptr [gNumber_of_races]
            inc esi
            add edi, 0x244
            cmp esi, eax
            jl L0046a239
            jmp L0046a25c
    L0046a258:
            mov dword ptr [esp + 0x18], esi
    L0046a25c:
            mov ebp, dword ptr [esp + 0x14]
            mov edi, dword ptr [esp + 0x18]
    L0046a264:
            lea edx, [edi + edi*8]
            shl edx, 4
            add edx, edi
            mov eax, dword ptr [edx*4 + gRace_list+0x184]
            test eax, eax
            je L0046a2a3
            push 0x1f
            lea eax, [esp + 0x16]
            push offset gFlic_bunch+0x10f8
            push eax
            call sprintf
            mov ecx, dword ptr [ebx]
            mov edx, dword ptr [ebx - 4]
            add esp, 0xc
            sub edx, 0xa
            push 1
            push 2
            push 3
            push ecx
            lea ecx, [esp + 0x22]
            call SolidPolyFontText
    L0046a2a3:
            mov edx, dword ptr [gCurrent_frontend_spec]
            mov eax, dword ptr [esp + 0x1c]
            inc eax
            add ebx, 0x158
            mov ecx, dword ptr [edx + 0x87a8]
            mov dword ptr [esp + 0x1c], eax
            cmp eax, ecx
            jle L0046a226
    L0046a2c6:
            mov dword ptr [gTyping_slot], 0xffffffff
            call EdgeTriggerModeOn
            call KillSplashScreen
            mov eax, dword ptr [gTyping]
            xor esi, esi
            cmp eax, esi
            je L0046a360
            call ProcessInputString
            mov ecx, ebp
            mov esi, eax
            call FuckWithWidths
            test esi, esi
            jge L0046a354
            mov ecx, ebp
            call ToggleTyping
            mov eax, dword ptr [gTyping]
            test eax, eax
            jne L0046a354
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
            je L0046a354
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
    L0046a354:
            mov ecx, ebp
            call FuckWithWidths
            jmp L0046a7e8
    L0046a360:
            call PollKeys
            call EdgeTriggerModeOff
            call PDAnyKeyDown
            cmp eax, -1
            je L0046a389
            cmp eax, 4
            je L0046a389
            mov dword ptr [gMouse_in_use], esi
            call PDGetTotalTime
            mov dword ptr [gFrontend_time_last_input], eax
    L0046a389:
            call EdgeTriggerModeOn
            mov eax, dword ptr [gMouse_in_use]
            mov dword ptr [esp + 0x18], esi
            cmp eax, esi
            je L0046a4d1
            mov dword ptr [gFrontend_selected_item_index], esi
            call PDGetTotalTime
            lea edx, [esp + 0x20]
            lea ecx, [esp + 0x24]
            mov dword ptr [gFrontend_time_last_input], eax
            call GetMousePosition
            call EitherMouseButtonDown
            mov edx, dword ptr [esp + 0x24]
            mov ecx, dword ptr [gCurrent_frontend_spec]
            mov dword ptr [esp + 0x18], eax
            mov eax, dword ptr [esp + 0x20]
            push eax
            call GetItemAtMousePos
            mov ebx, eax
            cmp ebx, -1
            mov dword ptr [esp + 0x1c], ebx
            mov ecx, ebx
            jne L0046a3eb
            mov ecx, 0x63
    L0046a3eb:
            cmp ecx, 3
            mov dword ptr [gFrontend_selected_item_index], ecx
            jle L0046a4d5
            cmp ecx, 8
            jge L0046a4d5
            lea edx, [ecx + 1]
            mov eax, edx
            shl eax, 3
            sub eax, edx
            lea eax, [eax + eax*2]
            lea edx, [edx + eax*2]
            cmp dword ptr [ebp + edx*8], esi
            jle L0046a4d5
            mov eax, ecx
            xor esi, esi
            shl eax, 3
            sub eax, ecx
            lea eax, [eax + eax*2]
            lea ecx, [ecx + eax*2]
            mov eax, dword ptr [gNumber_of_races]
            test eax, eax
            lea ebx, [ebp + ecx*8 + 0x189]
            jle L0046a45e
            mov edi, offset gRace_list
    L0046a441:
            mov edx, edi
            mov ecx, ebx
            call DRStricmp
            test eax, eax
            je L0046a462
            mov eax, dword ptr [gNumber_of_races]
            inc esi
            add edi, 0x244
            cmp esi, eax
            jl L0046a441
    L0046a45e:
            mov esi, dword ptr [esp + 0x24]
    L0046a462:
            lea ebx, [esi + esi*8]
            or ecx, 0xffffffff
            shl ebx, 4
            add ebx, esi
            xor eax, eax
            shl ebx, 2
            mov edx, dword ptr [esp + 0x14]
            push 0xd
            lea edi, [ebx + gRace_list+0x80]
            repne scasb
            not ecx
            sub edi, ecx
            lea ebp, [edx + 0x1f19]
            mov eax, ecx
            mov esi, edi
            mov edi, ebp
            mov dl, 0x52
            shr ecx, 2
            rep movsd
            mov ecx, eax
            and ecx, 3
            rep movsb
            mov ecx, ebp
            call MungeMetaCharactersChar
            mov ecx, dword ptr [ebx + gRace_list+0x194]
            mov dl, 0x4f
            push ecx
            mov ecx, ebp
            call MungeMetaCharactersNum
            mov edx, dword ptr [ebx + gRace_list+0x1dc]
            mov ecx, ebp
            push edx
            mov dl, 0x4c
            call MungeMetaCharactersNum
            xor esi, esi
            mov ebx, dword ptr [esp + 0x1c]
            mov ebp, dword ptr [esp + 0x14]
            jmp L0046a4d5
    L0046a4d1:
            mov ebx, dword ptr [esp + 0x1c]
    L0046a4d5:
            mov ecx, 0x48
            call PDKeyDown
            test eax, eax
            jne L0046a4f5
            mov ecx, 0x59
            call PDKeyDown
            test eax, eax
            je L0046a595
    L0046a4f5:
            mov eax, dword ptr [gFrontend_selected_item_index]
            cmp eax, 0xf
            jne L0046a509
            mov eax, 0xa
            mov dword ptr [gFrontend_selected_item_index], eax
    L0046a509:
            mov ecx, dword ptr [gCurrent_frontend_spec]
            dec eax
            cmp eax, esi
            jl L0046a53c
            mov edx, eax
            shl edx, 3
            sub edx, eax
            lea edx, [edx + edx*2]
            lea edx, [eax + edx*2]
            lea edx, [ecx + edx*8 + 0x15c]
    L0046a528:
            cmp dword ptr [edx - 4], esi
            jle L0046a531
            cmp dword ptr [edx], esi
            jne L0046a54d
    L0046a531:
            dec eax
            sub edx, 0x158
            cmp eax, esi
            jge L0046a528
    L0046a53c:
            mov edx, dword ptr [ecx + 0x104]
            call FindPrevActiveItem
            mov ecx, dword ptr [gCurrent_frontend_spec]
    L0046a54d:
            mov edx, dword ptr [gMouse_in_use]
            mov dword ptr [gFrontend_selected_item_index], eax
            cmp edx, esi
            jne L0046a595
            cmp eax, dword ptr [ecx + 0x87a4]
            jl L0046a595
            cmp eax, dword ptr [ecx + 0x87a8]
            jg L0046a595
            mov ecx, ebp
            call ToggleSelection
            mov ecx, dword ptr [gFrontend_selected_item_index]
            mov eax, ecx
            shl eax, 3
            sub eax, ecx
            lea eax, [eax + eax*2]
            lea ecx, [ecx + eax*2]
            mov eax, dword ptr [ebp + ecx*8 + 0x134]
            cmp eax, esi
            je L0046a595
            mov ecx, ebp
            call eax
    L0046a595:
            mov ecx, 0x49
            call PDKeyDown
            test eax, eax
            jne L0046a5b5
            mov ecx, 0x53
            call PDKeyDown
            test eax, eax
            je L0046a694
    L0046a5b5:
            mov eax, dword ptr [gFrontend_selected_item_index]
            cmp eax, 0x15
            jge L0046a60f
            cmp eax, 0xa
            jne L0046a5ce
            mov eax, 0xf
            mov dword ptr [gFrontend_selected_item_index], eax
    L0046a5ce:
            mov esi, dword ptr [gCurrent_frontend_spec]
            inc eax
            mov edx, dword ptr [esi + 0x104]
            dec edx
            cmp eax, edx
            jg L0046a60b
            mov ecx, eax
            shl ecx, 3
            sub ecx, eax
            lea ecx, [ecx + ecx*2]
            lea ecx, [eax + ecx*2]
            lea ecx, [esi + ecx*8 + 0x15c]
    L0046a5f4:
            mov edi, dword ptr [ecx - 4]
            test edi, edi
            jle L0046a600
            cmp dword ptr [ecx], 0
            jne L0046a64c
    L0046a600:
            inc eax
            add ecx, 0x158
            cmp eax, edx
            jle L0046a5f4
    L0046a60b:
            xor eax, eax
            jmp L0046a64c
    L0046a60f:
            mov esi, dword ptr [gCurrent_frontend_spec]
            mov dword ptr [gFrontend_selected_item_index], 0xffffffff
            xor ecx, ecx
            mov edx, dword ptr [esi + 0x104]
            dec edx
            test edx, edx
            jl L0046a648
            lea eax, [esi + 0x15c]
    L0046a632:
            mov edi, dword ptr [eax - 4]
            test edi, edi
            jle L0046a63e
            cmp dword ptr [eax], 0
            jne L0046a64a
    L0046a63e:
            inc ecx
            add eax, 0x158
            cmp ecx, edx
            jle L0046a632
    L0046a648:
            xor ecx, ecx
    L0046a64a:
            mov eax, ecx
    L0046a64c:
            mov ecx, dword ptr [gMouse_in_use]
            mov dword ptr [gFrontend_selected_item_index], eax
            test ecx, ecx
            jne L0046a694
            cmp eax, dword ptr [esi + 0x87a4]
            jl L0046a694
            cmp eax, dword ptr [esi + 0x87a8]
            jg L0046a694
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
            je L0046a694
            mov ecx, ebp
            call eax
    L0046a694:
            mov ecx, 0x3f
            call PDKeyDown
            test eax, eax
            je L0046a7ba
            mov ecx, dword ptr [gCurrent_frontend_spec]
            mov edx, dword ptr [gFrontend_selected_item_index]
            mov dword ptr [ecx + 0x118], edx
            mov ecx, dword ptr [gCurrent_frontend_spec]
            xor edx, edx
            mov eax, dword ptr [ecx + 0x8790]
            test eax, eax
            jle L0046a6ec
            xor eax, eax
    L0046a6cc:
            mov esi, dword ptr [ecx + eax + 0x87a0]
            inc edx
            mov dword ptr [ecx + eax + 0x87c0], esi
            mov ecx, dword ptr [gCurrent_frontend_spec]
            add eax, 0x34
            cmp edx, dword ptr [ecx + 0x8790]
            jl L0046a6cc
    L0046a6ec:
            mov ecx, ebp
            call FRONTEND_DestroyMenu
            mov ecx, offset gFrontend_OPTIONS
            mov dword ptr [gCurrent_frontend_spec], 0x632c60
            call FRONTEND_CreateMenu
            mov edx, dword ptr [gCurrent_frontend_spec]
            mov eax, dword ptr [ebp + 0x114]
            cmp edx, eax
            je L0046a723
            mov dword ptr [edx + 0x114], ebp
            mov edx, dword ptr [gCurrent_frontend_spec]
    L0046a723:
            mov ecx, ebp
            call Morph_Initialise
            mov ecx, dword ptr [gCurrent_frontend_spec]
            xor edx, edx
            mov eax, dword ptr [ecx + 0x8790]
            test eax, eax
            jle L0046a75e
            xor eax, eax
    L0046a73e:
            mov esi, dword ptr [ecx + eax + 0x87c0]
            inc edx
            mov dword ptr [ecx + eax + 0x87a0], esi
            mov ecx, dword ptr [gCurrent_frontend_spec]
            add eax, 0x34
            cmp edx, dword ptr [ecx + 0x8790]
            jl L0046a73e
    L0046a75e:
            call UpdateScrollPositions
            mov eax, dword ptr [gCurrent_frontend_spec]
            mov ecx, dword ptr [eax + 0x118]
            mov dword ptr [gFrontend_selected_item_index], ecx
            mov ecx, dword ptr [gProgram_state+0x28]
            test ecx, ecx
            je L0046a79e
            mov ecx, dword ptr [gNet_mode]
            test ecx, ecx
            je L0046a7ba
            cmp dword ptr [eax + 0x118], 5
            jne L0046a7ba
            mov edi, 3
            mov dword ptr [gFrontend_selected_item_index], edi
            jmp L0046a7bf
    L0046a79e:
            mov eax, dword ptr [eax + 0x118]
            mov edi, 3
            cmp eax, edi
            jl L0046a7b2
            cmp eax, 0x1e
            jle L0046a7bf
    L0046a7b2:
            mov dword ptr [gFrontend_selected_item_index], edi
            jmp L0046a7bf
    L0046a7ba:
            mov edi, 3
    L0046a7bf:
            mov ecx, 0x33
            call PDKeyDown
            test eax, eax
            jne L0046a807
            mov ecx, 0x34
            call PDKeyDown
            test eax, eax
            jne L0046a807
            mov eax, dword ptr [esp + 0x18]
            test eax, eax
            je L0046a7e8
            cmp ebx, -1
            jne L0046a807
    L0046a7e8:
            call ServiceGame
            call CheckForCheatingGits
            mov ecx, dword ptr [gFrontend_leave_current_menu]
            xor eax, eax
            test ecx, ecx
            setne al
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x18
            ret
    L0046a807:
            mov ecx, ebp
            call ToggleSelection
            mov eax, dword ptr [gMouse_in_use]
            mov ecx, dword ptr [gCurrent_frontend_spec]
            test eax, eax
            mov eax, dword ptr [gFrontend_selected_item_index]
            jne L0046a83c
            cmp eax, dword ptr [ecx + 0x87a4]
            jl L0046a83c
            cmp eax, dword ptr [ecx + 0x87a8]
            jg L0046a83c
            mov eax, 0x15
            mov dword ptr [gFrontend_selected_item_index], eax
    L0046a83c:
            mov ecx, eax
            shl ecx, 3
            sub ecx, eax
            lea edx, [ecx + ecx*2]
            lea ecx, [eax + edx*2]
            mov edx, dword ptr [ebp + ecx*8 + 0x13c]
            cmp edx, 2
            lea ecx, [ebp + ecx*8]
            jne L0046a863
            mov eax, edx
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x18
            ret
    L0046a863:
            cmp edx, 1
            jne L0046a86e
            mov dword ptr [gFrontend_leave_current_menu], edx
    L0046a86e:
            mov edx, dword ptr [ecx + 0x134]
            test edx, edx
            je L0046a8c0
            mov ecx, ebp
            call edx
            mov esi, eax
            mov eax, dword ptr [esp + 0x18]
            cmp eax, 2
            mov eax, dword ptr [gFrontend_selected_item_index]
            jne L0046a8c6
            cmp eax, 4
            jl L0046a8c6
            cmp eax, 8
            jge L0046a8c6
            mov eax, dword ptr [gProgram_state+0x2c]
            test eax, eax
            je L0046a8ac
            mov eax, 1
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x18
            ret
    L0046a8ac:
            mov dword ptr [gFrontend_next_menu], 6
            mov eax, edi
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x18
            ret
    L0046a8c0:
            mov esi, dword ptr [ecx + 0x13c]
    L0046a8c6:
            cmp esi, 5
            jne L0046a8da
            mov esi, 1
            mov eax, esi
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x18
            ret
    L0046a8da:
            mov ecx, eax
            shl ecx, 3
            sub ecx, eax
            lea edx, [ecx + ecx*2]
            lea ecx, [eax + edx*2]
            mov edx, dword ptr [ebp + ecx*8 + 0x138]
            test edx, edx
            je L0046aa0f
            mov edx, dword ptr [gCurrent_frontend_spec]
            mov dword ptr [edx + 0x118], eax
            mov eax, dword ptr [gCurrent_frontend_spec]
            xor edx, edx
            mov ecx, dword ptr [eax + 0x8790]
            test ecx, ecx
            jle L0046a934
            xor ecx, ecx
    L0046a915:
            mov ebx, dword ptr [eax + ecx + 0x87a0]
            inc edx
            mov dword ptr [eax + ecx + 0x87c0], ebx
            mov eax, dword ptr [gCurrent_frontend_spec]
            add ecx, 0x34
            cmp edx, dword ptr [eax + 0x8790]
            jl L0046a915
    L0046a934:
            mov ecx, ebp
            call FRONTEND_DestroyMenu
            mov ecx, dword ptr [gFrontend_selected_item_index]
            mov eax, ecx
            shl eax, 3
            sub eax, ecx
            lea eax, [eax + eax*2]
            lea ecx, [ecx + eax*2]
            mov ecx, dword ptr [ebp + ecx*8 + 0x138]
            mov dword ptr [gCurrent_frontend_spec], ecx
            call FRONTEND_CreateMenu
            mov edx, dword ptr [gCurrent_frontend_spec]
            mov eax, dword ptr [ebp + 0x114]
            cmp edx, eax
            je L0046a97c
            mov dword ptr [edx + 0x114], ebp
            mov edx, dword ptr [gCurrent_frontend_spec]
    L0046a97c:
            mov ecx, ebp
            call Morph_Initialise
            mov ecx, dword ptr [gCurrent_frontend_spec]
            cmp ecx, offset gFrontend_QUIT
            jne L0046a99d
            mov dword ptr [gFrontend_selected_item_index], 0
            jmp L0046a9a9
    L0046a99d:
            mov edx, dword ptr [ecx + 0x118]
            mov dword ptr [gFrontend_selected_item_index], edx
    L0046a9a9:
            cmp ecx, offset gFrontend_OPTIONS
            jne L0046a9dc
            mov eax, dword ptr [gProgram_state+0x28]
            test eax, eax
            je L0046a9ce
            mov eax, dword ptr [gNet_mode]
            test eax, eax
            je L0046a9dc
            cmp dword ptr [ecx + 0x118], 5
            jne L0046a9dc
            jmp L0046a9d6
    L0046a9ce:
            cmp dword ptr [ecx + 0x118], edi
            jge L0046a9dc
    L0046a9d6:
            mov dword ptr [gFrontend_selected_item_index], edi
    L0046a9dc:
            mov eax, dword ptr [ecx + 0x8790]
            xor edx, edx
            test eax, eax
            jle L0046aa0a
            xor eax, eax
    L0046a9ea:
            mov edi, dword ptr [ecx + eax + 0x87c0]
            inc edx
            mov dword ptr [ecx + eax + 0x87a0], edi
            mov ecx, dword ptr [gCurrent_frontend_spec]
            add eax, 0x34
            cmp edx, dword ptr [ecx + 0x8790]
            jl L0046a9ea
    L0046aa0a:
            call UpdateScrollPositions
    L0046aa0f:
            mov eax, esi
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x18
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0046b6a0
void C2_HOOK_FASTCALL UpdateCarInfo(tFrontend_spec *pFrontend) {

    strcpy(pFrontend->items[2].text, gOpponents[gProgram_state.cars_available[gFrontend_change_car_selected_car]].car_name);
    strcpy(pFrontend->items[3].text, gOpponents[gProgram_state.cars_available[gFrontend_change_car_selected_car]].name);
    strcpy(pFrontend->items[5].text, gOpponents[gProgram_state.cars_available[gFrontend_change_car_selected_car]].line4_description);
    sprintf(pFrontend->items[4].text, "%s @R%s @R%s",
        gOpponents[gProgram_state.cars_available[gFrontend_change_car_selected_car]].line1_topspeed,
        gOpponents[gProgram_state.cars_available[gFrontend_change_car_selected_car]].line2_weight,
        gOpponents[gProgram_state.cars_available[gFrontend_change_car_selected_car]].line3_acceleration);
    MungeMetaCharactersChar(pFrontend->items[4].text, 'R', '\r');
    FuckWithWidths(pFrontend);
    if (gFrontend_car_image_outdated && gFrontend_opponent_profile_pic_needs_update) {
        MenuSetDriverImage(gProgram_state.cars_available[gFrontend_change_car_selected_car], 12);
        MenuSetCarImage(gProgram_state.cars_available[gFrontend_change_car_selected_car], 6);
        gFrontend_opponent_profile_pic_needs_update = 0;
        gFrontend_car_image_outdated = 0;
    }
}

// FUNCTION: CARMA2_HW 0x00469a30
int C2_HOOK_FASTCALL OnePlayerSetup(tFrontend_spec* pFrontend) {
    return 0;
}

// FUNCTION: CARMA2_HW 0x00467eb0
int C2_HOOK_FASTCALL MultiplayerSetup(tFrontend_spec* pFrontend) {
    gFrontend_net_initialized = !NetInitialise();
    if (gFrontend_net_initialized) {

        gPending_race = -1;
        gStart_race_sent = 0;
        gCurrent_race.number_of_racers = 0;
        gLast_host_query = 0;
        AboutToLoadFirstCar();
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0046a010
int C2_HOOK_FASTCALL testUp(tFrontend_spec *pFrontend) {
    if (PDGetTotalTime() - gFrontend_last_scroll > 400) {
        gFrontend_last_scroll = PDGetTotalTime();
        if ((gCurrent_race_group - gRace_groups) % 10 > 0) {
            gCurrent_race_group -= 1;
        }
        RefreshRacesScroller(pFrontend);
        FuckWithWidths(pFrontend);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0046a080
int C2_HOOK_FASTCALL testDn(tFrontend_spec *pFrontend) {
    if (PDGetTotalTime() - gFrontend_last_scroll > 400) {
        gFrontend_last_scroll = PDGetTotalTime();
        if ((gCurrent_race_group - gRace_groups) % 10 < 9) {
            gCurrent_race_group += 1;
        }
        RefreshRacesScroller(pFrontend);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00469ea0
int C2_NAKED C2_HOOK_FASTCALL MainMenuSelectRace(tFrontend_spec* pFrontend) {
#ifndef CARPOCALYPSE2_MATCHING
    char group_name[12];
    int race_index;
    int i;

    race_index = gFrontend_selected_item_index;
    for (i = 0; i < gNumber_of_races; i++) {
        if (DRStricmp(pFrontend->items[gFrontend_selected_item_index].text, gRace_list[i].name) == 0) {
            race_index = i;
            break;
        }
    }
    gProgram_state.current_race_index = race_index;
    pFrontend->items[22].highFont = 11;
    pFrontend->items[22].unlitFont = 11;
    if (!gMouse_in_use) {
        char* str;

        strcpy(pFrontend->items[22].text, gRace_list[race_index].name);
        str = pFrontend->items[22].text;
        MungeMetaCharactersChar(str, 'R', '\r');
        MungeMetaCharactersNum(str, gRace_list[race_index].count_explicit_opponents, 'O');
        MungeMetaCharactersNum(str, gRace_list[race_index].count_laps, 'L');
    }
    gCurrent_race_group = gRace_list[race_index].group;
    sprintf(group_name, "%s %d", IString_Get(78), (gCurrent_race_group - gRace_groups) % 10 + 1);
    strcpy(pFrontend->items[2].text, group_name);
    FuckWithWidths(pFrontend);
    return 0;
#else
    __asm {
            sub esp, 0x10
            push ebx
            push ebp
            mov ebp, ecx
            push esi
            mov ecx, dword ptr [gFrontend_selected_item_index]
            xor esi, esi
            mov eax, ecx
            push edi
            shl eax, 3
            sub eax, ecx
            lea eax, [eax + eax*2]
            lea ecx, [ecx + eax*2]
            mov eax, dword ptr [gNumber_of_races]
            test eax, eax
            lea ebx, [ebp + ecx*8 + 0x189]
            jle L00469ef0
            mov edi, offset gRace_list
    L00469ed3:
            mov edx, edi
            mov ecx, ebx
            call DRStricmp
            test eax, eax
            je L00469ef4
            mov eax, dword ptr [gNumber_of_races]
            inc esi
            add edi, 0x244
            cmp esi, eax
            jl L00469ed3
    L00469ef0:
            mov esi, dword ptr [esp + 0x10]
    L00469ef4:
            mov eax, 0xb
            mov dword ptr [gProgram_state+0x78], esi
            mov dword ptr [ebp + 0x1ed4], eax
            mov dword ptr [ebp + 0x1ed0], eax
            mov eax, dword ptr [gMouse_in_use]
            test eax, eax
            jne L00469f79
            lea ebx, [esi + esi*8]
            or ecx, 0xffffffff
            shl ebx, 4
            add ebx, esi
            xor eax, eax
            shl ebx, 2
            lea edx, [ebp + 0x1f19]
            push 0xd
            lea edi, [ebx + gRace_list+0x80]
            repne scasb
            not ecx
            sub edi, ecx
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            mov dl, 0x52
            shr ecx, 2
            rep movsd
            mov ecx, eax
            and ecx, 3
            rep movsb
            lea esi, [ebp + 0x1f19]
            mov ecx, esi
            call MungeMetaCharactersChar
            mov ecx, dword ptr [ebx + gRace_list+0x194]
            mov dl, 0x4f
            push ecx
            mov ecx, esi
            call MungeMetaCharactersNum
            mov edx, dword ptr [ebx + gRace_list+0x1dc]
            mov ecx, esi
            push edx
            mov dl, 0x4c
            call MungeMetaCharactersNum
    L00469f79:
            mov eax, dword ptr [gProgram_state+0x78]
            mov ebx, dword ptr [gRace_groups]
            lea ecx, [eax + eax*8]
            shl ecx, 4
            add ecx, eax
            mov eax, dword ptr [ecx*4 + gRace_list+0x240]
            mov dword ptr [gCurrent_race_group], eax
            sub eax, ebx
            mov ecx, eax
            mov eax, 0x2aaaaaab
            imul ecx
            mov eax, edx
            mov ecx, 0xa
            sar eax, 1
            mov edx, eax
            shr edx, 0x1f
            add eax, edx
            cdq
            idiv ecx
            mov ecx, 0x4e
            inc edx
            push edx
            call IString_Get
            lea edx, [esp + 0x18]
            push eax
            push offset gNext_crush_actor_id+0x50c
            push edx
            call sprintf
            lea edi, [esp + 0x24]
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
            and ecx, 3
            rep movsb
            mov ecx, ebp
            call FuckWithWidths
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x10
            ret
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00467270
int C2_HOOK_FASTCALL StartFudge(tFrontend_spec* pFrontend) {
    if (gProgram_state.field_0x2c) {
        return 5;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00577190
int C2_HOOK_CDECL FUN_00577190(int pArg) {
    return pArg - 0x20;
}
// FUNCTION: CARMA2_HW 0x00576eb0
int C2_HOOK_CDECL FUN_00576eb0(FILE* pFile) {
    return fgetc(pFile);
}
