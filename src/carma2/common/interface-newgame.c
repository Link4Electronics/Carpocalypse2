#include "interface-newgame.h"

#include "interface.h"
#include "init.h"
#include "loading2.h"
#include "graphics2.h"
#include "intrface.h"
#include "input.h"
#include "sound.h"
#include "globvars.h"

#include "c2_string.h"
extern void C2_HOOK_FASTCALL FuckWithWidths(tFrontend_spec* pFrontend);
extern int C2_HOOK_FASTCALL ProcessInputString(void);
extern int C2_HOOK_FASTCALL FindNextActiveItem(tFrontend_spec* pFrontend, int pStart_index);
extern void C2_HOOK_FASTCALL NewGameToggleTyping(tFrontend_spec* pFrontend);
extern void C2_HOOK_FASTCALL ServiceGame(void);
int C2_HOOK_FASTCALL NewGameInfunc(tFrontend_spec* pFrontend);
int C2_HOOK_FASTCALL NewGameOutfunc(tFrontend_spec* pFrontend);
int C2_HOOK_FASTCALL FRONTEND_NewGameMenuHandler(tFrontend_spec* pFrontend);
int C2_HOOK_FASTCALL ChooseSkill(tFrontend_spec* pFrontend);
extern int gFrontend_selected_item_index;
extern int gFrontend_leave_current_menu;
extern int gTyping_slot;
extern int gFrontend_scrollbars_updated;
extern tFrontend_spec gFrontend_MAIN;
extern tFrontend_spec gFrontend_QUIT;

// GLOBAL: CARMA2_HW 0x005bf280
tFrontend_spec gFrontend_NEWGAME = {
    "newgame",
    0,
    6,
    NewGameInfunc,
    NewGameOutfunc,
    FRONTEND_NewGameMenuHandler,
    &gFrontend_MAIN,
    0,
    0,
    0,
    10,
    0,
    0,
    {
        { 0, temp,          NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 1, temp,          NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 4, ChooseSkill,   NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 3, ChooseSkill,   NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 2, ChooseSkill,   NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 7, temp,          NULL, 1, 17, 18, 0, 0, 0, 0, 1, 1, },
    },
};

#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern tS3_sound_tag C2_HOOK_FASTCALL DRS3StartSound(tS3_outlet* pOutlet, tS3_sound_id pSound);
extern void C2_HOOK_FASTCALL EdgeTriggerModeOff(void);
extern void C2_HOOK_FASTCALL EdgeTriggerModeOn(void);
extern int C2_HOOK_FASTCALL EitherMouseButtonDown(void);
extern int C2_HOOK_FASTCALL FRONTEND_CreateMenu(tFrontend_spec* pFrontend_spec);
extern int C2_HOOK_FASTCALL FRONTEND_DestroyMenu(tFrontend_spec* pFrontend);
extern int C2_HOOK_FASTCALL FindPrevActiveItem(tFrontend_spec* pFrontend, int pStart_index);
extern int C2_HOOK_FASTCALL GetItemAtMousePos(tFrontend_spec *pFrontend, int pX, int pY);
extern void C2_HOOK_FASTCALL GetMousePosition(int *pX, int *pY);
extern void C2_HOOK_FASTCALL KillSplashScreen(void);
extern void C2_HOOK_FASTCALL Morph_Initialise(tFrontend_spec* pCurrent, tFrontend_spec* pNext);
extern int C2_HOOK_FASTCALL PDAnyKeyDown(void);
extern tU32 C2_HOOK_FASTCALL PDGetTotalTime(void);
extern int C2_HOOK_FASTCALL PDKeyDown(int pKey_index);
extern void C2_HOOK_FASTCALL PollKeys(void);
extern int C2_HOOK_FASTCALL SaveOptions(void);
extern void C2_HOOK_FASTCALL ToggleSelection(tFrontend_spec* pFrontend);
extern int C2_HOOK_FASTCALL ToggleTyping(tFrontend_spec* pFrontend);
extern void C2_HOOK_FASTCALL UpdateScrollPositions(tFrontend_spec* pFrontend);
extern tS3_outlet* gEffects_outlet;
extern tU32 gFrontend_time_last_input;
extern int gMouse_in_use;
extern tProgram_state gProgram_state;
extern int gTyping;
#endif
// FUNCTION: CARMA2_HW 0x004671a0
int C2_HOOK_FASTCALL NewGameInfunc(tFrontend_spec* pFrontend) {
    EdgeTriggerModeOff();
    WaitForNoKeys();
    EdgeTriggerModeOn();
    DRS3StartSound(gEffects_outlet, eSoundId_Swingin);
    switch (gProgram_state.skill_level) {
    case 0:
        pFrontend->items[2].enabled = 1;
        pFrontend->items[2].radioButton_selected = 1;
        break;
    case 1:
    default:
        pFrontend->items[3].enabled = 1;
        pFrontend->items[3].radioButton_selected = 1;
        gProgram_state.skill_level = 1;
        break;
    case 2:
        pFrontend->items[4].enabled = 1;
        pFrontend->items[4].radioButton_selected = 1;
        break;
    }
    pFrontend->items[5].enabled = 1;
    pFrontend->default_item = 5;
    gFrontend_selected_item_index = 5;
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
    gMouse_in_use = 0;
    return 0;
}

// FUNCTION: CARMA2_HW 0x00467180
int C2_HOOK_FASTCALL NewGameOutfunc(tFrontend_spec* pFrontend) {
    SaveOptions();
    InitGameAccordingToSkillLevel();
    DRS3StartSound(gEffects_outlet, eSoundId_Swingout);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00467280
int C2_NAKED C2_HOOK_FASTCALL FRONTEND_NewGameMenuHandler(tFrontend_spec* pFrontend) {
#ifndef CARPOCALYPSE2_MATCHING
    int orig_selected_item;

    orig_selected_item = gFrontend_selected_item_index;
    gTyping_slot = -1;
    EdgeTriggerModeOn();
    KillSplashScreen();
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
            SaveOptions();
        }
    } else {
        int key;
        int mouse_button;
        int item_at_mouse;

        PollKeys();
        EdgeTriggerModeOff();
        key = PDAnyKeyDown();
        if (key != -1 && key != 4) {
            gMouse_in_use = 0;
            ResetInterfaceTimeout();
        }
        EdgeTriggerModeOn();
        mouse_button = 0;
        item_at_mouse = -1;
        if (gMouse_in_use) {
            int mouse_x;
            int mouse_y;

            ResetInterfaceTimeout();
            GetMousePosition(&mouse_x, &mouse_y);
            mouse_button = EitherMouseButtonDown();
            item_at_mouse = GetItemAtMousePos(gCurrent_frontend_spec, mouse_x, mouse_y);
            gFrontend_selected_item_index = item_at_mouse;
            if (item_at_mouse == -1) {
                gFrontend_selected_item_index = 99;
            }
        }
        if (PDKeyDown(72) || PDKeyDown(89)) {

            gFrontend_selected_item_index = FindPrevActiveItem(gCurrent_frontend_spec, gFrontend_selected_item_index);
            if (!gMouse_in_use
                    && gFrontend_selected_item_index >= gCurrent_frontend_spec->scrollers[0].indexFirstScrollableItem
                    && gFrontend_selected_item_index <= gCurrent_frontend_spec->scrollers[0].indexLastScrollableItem) {

                ToggleSelection(pFrontend);
                if (pFrontend->items[gFrontend_selected_item_index].action != NULL) {
                    (*pFrontend->items[gFrontend_selected_item_index].action)(pFrontend);
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

            gCurrent_frontend_spec->default_item = gFrontend_selected_item_index;
            FRONTEND_DestroyMenu(pFrontend);
            gCurrent_frontend_spec = &gFrontend_MAIN;
            FRONTEND_CreateMenu(&gFrontend_MAIN);
            Morph_Initialise(pFrontend, gCurrent_frontend_spec);
            gFrontend_selected_item_index = gCurrent_frontend_spec->default_item;

            for (i = 0; i < gCurrent_frontend_spec->count_scrollers; i++) {

                gCurrent_frontend_spec->scrollers[i].indexTopItem = gCurrent_frontend_spec->scrollers[i].indexOfItemAtTop;
            }
            UpdateScrollPositions(gCurrent_frontend_spec);
            DRS3StartSound(gEffects_outlet, eSoundId_EscEsc);
            return 0;
        }

        if (PDKeyDown(51) || PDKeyDown(52) || (mouse_button != 0 && item_at_mouse != -1)) {

            ToggleSelection(pFrontend);
            if (pFrontend->items[gFrontend_selected_item_index].field_0xc == 2) {
                return 2;
            }
            if (pFrontend->items[gFrontend_selected_item_index].action != NULL) {

                pFrontend->items[gFrontend_selected_item_index].action(pFrontend);
                if (mouse_button == 2) {
                    return 1;
                }
            }
            if (pFrontend->items[gFrontend_selected_item_index].menuInfo != NULL) {
                int i;

                gCurrent_frontend_spec->default_item = gFrontend_selected_item_index;

                for (i = 0; i < gCurrent_frontend_spec->count_scrollers; i++) {

                    gCurrent_frontend_spec->scrollers[i].indexOfItemAtTop = gCurrent_frontend_spec->scrollers[i].indexTopItem;
                }
                FRONTEND_DestroyMenu(pFrontend);
                gCurrent_frontend_spec = pFrontend->items[gFrontend_selected_item_index].menuInfo;
                Morph_Initialise(pFrontend, gCurrent_frontend_spec);
                FRONTEND_CreateMenu(gCurrent_frontend_spec);
                if (gCurrent_frontend_spec == &gFrontend_QUIT) {
                    gFrontend_selected_item_index = 0;
                } else {
                    gFrontend_selected_item_index = gCurrent_frontend_spec->default_item;
                }
                for (i = 0; i < gCurrent_frontend_spec->count_scrollers; i++) {

                    gCurrent_frontend_spec->scrollers[i].indexTopItem = gCurrent_frontend_spec->scrollers[i].indexOfItemAtTop;
                }
                UpdateScrollPositions(gCurrent_frontend_spec);
            }
            return pFrontend->items[orig_selected_item].field_0xc;
        }
    }
    ServiceGame();
    return 0;
#else
    __asm {
            sub esp, 0x10
            push ebx
            push ebp
            push esi
            mov esi, dword ptr [gFrontend_selected_item_index]
            push edi
            mov edi, ecx
            mov dword ptr [esp + 0x14], esi
            mov dword ptr [gTyping_slot], 0xffffffff
            call EdgeTriggerModeOn
            call KillSplashScreen
            mov eax, dword ptr [gTyping]
            xor ebx, ebx
            cmp eax, ebx
            je L0046733c
            call ProcessInputString
            mov ecx, edi
            mov esi, eax
            call FuckWithWidths
            cmp esi, ebx
            jge L00467609
            mov ecx, edi
            call ToggleTyping
            cmp dword ptr [gTyping], ebx
            jne L00467328
            mov ecx, dword ptr [gFrontend_selected_item_index]
            mov eax, ecx
            shl eax, 3
            sub eax, ecx
            lea eax, [eax + eax*2]
            lea ecx, [ecx + eax*2]
            xor eax, eax
            lea edx, [edi + ecx*8 + 0x189]
            or ecx, 0xffffffff
            mov edi, edx
            repne scasb
            not ecx
            dec ecx
            je L00467328
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
    L00467328:
            call SaveOptions
            call ServiceGame
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x10
            ret
    L0046733c:
            call PollKeys
            call EdgeTriggerModeOff
            call PDAnyKeyDown
            cmp eax, -1
            je L00467365
            cmp eax, 4
            je L00467365
            mov dword ptr [gMouse_in_use], ebx
            call PDGetTotalTime
            mov dword ptr [gFrontend_time_last_input], eax
    L00467365:
            call EdgeTriggerModeOn
            mov eax, dword ptr [gMouse_in_use]
            mov dword ptr [esp + 0x10], ebx
            cmp eax, ebx
            je L004673c6
            call PDGetTotalTime
            lea edx, [esp + 0x18]
            lea ecx, [esp + 0x1c]
            mov dword ptr [gFrontend_time_last_input], eax
            call GetMousePosition
            call EitherMouseButtonDown
            mov edx, dword ptr [esp + 0x1c]
            mov ecx, dword ptr [gCurrent_frontend_spec]
            mov dword ptr [esp + 0x10], eax
            mov eax, dword ptr [esp + 0x18]
            push eax
            call GetItemAtMousePos
            mov ebp, eax
            cmp ebp, -1
            je L004673ba
            mov dword ptr [gFrontend_selected_item_index], ebp
            jmp L004673ca
    L004673ba:
            mov dword ptr [gFrontend_selected_item_index], 0x63
            jmp L004673ca
    L004673c6:
            mov ebp, dword ptr [esp + 0x1c]
    L004673ca:
            mov ecx, 0x48
            call PDKeyDown
            test eax, eax
            jne L004673ea
            mov ecx, 0x59
            call PDKeyDown
            test eax, eax
            je L0046747e
    L004673ea:
            mov ecx, dword ptr [gFrontend_selected_item_index]
            lea eax, [ecx - 1]
            mov ecx, dword ptr [gCurrent_frontend_spec]
            cmp eax, ebx
            jl L00467425
            mov edx, eax
            shl edx, 3
            sub edx, eax
            lea edx, [edx + edx*2]
            lea edx, [eax + edx*2]
            lea edx, [ecx + edx*8 + 0x15c]
    L00467411:
            cmp dword ptr [edx - 4], ebx
            jle L0046741a
            cmp dword ptr [edx], ebx
            jne L00467436
    L0046741a:
            dec eax
            sub edx, 0x158
            cmp eax, ebx
            jge L00467411
    L00467425:
            mov edx, dword ptr [ecx + 0x104]
            call FindPrevActiveItem
            mov ecx, dword ptr [gCurrent_frontend_spec]
    L00467436:
            mov edx, dword ptr [gMouse_in_use]
            mov dword ptr [gFrontend_selected_item_index], eax
            cmp edx, ebx
            jne L0046747e
            cmp eax, dword ptr [ecx + 0x87a4]
            jl L0046747e
            cmp eax, dword ptr [ecx + 0x87a8]
            jg L0046747e
            mov ecx, edi
            call ToggleSelection
            mov ecx, dword ptr [gFrontend_selected_item_index]
            mov eax, ecx
            shl eax, 3
            sub eax, ecx
            lea eax, [eax + eax*2]
            lea ecx, [ecx + eax*2]
            mov eax, dword ptr [edi + ecx*8 + 0x134]
            cmp eax, ebx
            je L0046747e
            mov ecx, edi
            call eax
    L0046747e:
            mov ecx, 0x49
            call PDKeyDown
            test eax, eax
            jne L0046749e
            mov ecx, 0x53
            call PDKeyDown
            test eax, eax
            je L0046753c
    L0046749e:
            mov edx, dword ptr [edi + 0x104]
            mov eax, dword ptr [gFrontend_selected_item_index]
            dec edx
            cmp eax, edx
            jge L00467786
            mov esi, dword ptr [gCurrent_frontend_spec]
            inc eax
            mov ecx, dword ptr [esi + 0x104]
            lea edx, [ecx - 1]
            cmp eax, edx
            jg L004674ee
            mov ecx, eax
            shl ecx, 3
            sub ecx, eax
            lea ecx, [ecx + ecx*2]
            lea ecx, [eax + ecx*2]
            lea ecx, [esi + ecx*8 + 0x15c]
    L004674da:
            cmp dword ptr [ecx - 4], ebx
            jle L004674e3
            cmp dword ptr [ecx], ebx
            jne L004674f0
    L004674e3:
            inc eax
            add ecx, 0x158
            cmp eax, edx
            jle L004674da
    L004674ee:
            xor eax, eax
    L004674f0:
            mov ecx, dword ptr [gMouse_in_use]
            mov dword ptr [gFrontend_selected_item_index], eax
            cmp ecx, ebx
            jne L00467538
            cmp eax, dword ptr [esi + 0x87a4]
            jl L00467538
            cmp eax, dword ptr [esi + 0x87a8]
            jg L00467538
            mov ecx, edi
            call ToggleSelection
            mov ecx, dword ptr [gFrontend_selected_item_index]
            mov eax, ecx
            shl eax, 3
            sub eax, ecx
            lea edx, [eax + eax*2]
            lea eax, [ecx + edx*2]
            mov eax, dword ptr [edi + eax*8 + 0x134]
            cmp eax, ebx
            je L00467538
            mov ecx, edi
            call eax
    L00467538:
            mov esi, dword ptr [esp + 0x14]
    L0046753c:
            mov ecx, 0x3f
            call PDKeyDown
            test eax, eax
            je L004675e2
            mov ecx, dword ptr [gCurrent_frontend_spec]
            mov edx, dword ptr [gFrontend_selected_item_index]
            mov dword ptr [ecx + 0x118], edx
            mov ecx, edi
            call FRONTEND_DestroyMenu
            mov ecx, offset gFrontend_MAIN
            mov dword ptr [gCurrent_frontend_spec], ecx
            call FRONTEND_CreateMenu
            mov edx, dword ptr [gCurrent_frontend_spec]
            mov ecx, edi
            call Morph_Initialise
            mov ecx, dword ptr [gCurrent_frontend_spec]
            xor edx, edx
            mov eax, dword ptr [ecx + 0x118]
            mov dword ptr [gFrontend_selected_item_index], eax
            mov eax, dword ptr [ecx + 0x8790]
            cmp eax, ebx
            jle L004675c3
            xor eax, eax
    L004675a3:
            mov esi, dword ptr [ecx + eax + 0x87c0]
            inc edx
            mov dword ptr [ecx + eax + 0x87a0], esi
            mov ecx, dword ptr [gCurrent_frontend_spec]
            add eax, 0x34
            cmp edx, dword ptr [ecx + 0x8790]
            jl L004675a3
    L004675c3:
            call UpdateScrollPositions
            mov ecx, dword ptr [gEffects_outlet]
            mov edx, 0xbbd
            call DRS3StartSound
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x10
            ret
    L004675e2:
            mov ecx, 0x33
            call PDKeyDown
            test eax, eax
            jne L00467618
            mov ecx, 0x34
            call PDKeyDown
            test eax, eax
            jne L00467618
            cmp dword ptr [esp + 0x10], ebx
            je L00467609
            cmp ebp, -1
            jne L00467618
    L00467609:
            call ServiceGame
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x10
            ret
    L00467618:
            mov ecx, edi
            call ToggleSelection
            mov ecx, dword ptr [gFrontend_selected_item_index]
            mov ebp, 2
            mov eax, ecx
            shl eax, 3
            sub eax, ecx
            lea edx, [eax + eax*2]
            lea eax, [ecx + edx*2]
            mov edx, dword ptr [edi + eax*8 + 0x13c]
            cmp edx, ebp
            lea eax, [edi + eax*8]
            jne L0046764f
            mov eax, ebp
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x10
            ret
    L0046764f:
            mov eax, dword ptr [eax + 0x134]
            cmp eax, ebx
            je L00467676
            mov ecx, edi
            call eax
            cmp dword ptr [esp + 0x10], ebp
            jne L00467670
            mov eax, 1
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x10
            ret
    L00467670:
            mov ecx, dword ptr [gFrontend_selected_item_index]
    L00467676:
            mov eax, ecx
            shl eax, 3
            sub eax, ecx
            lea edx, [eax + eax*2]
            lea eax, [ecx + edx*2]
            cmp dword ptr [edi + eax*8 + 0x138], ebx
            je L0046775a
            mov edx, dword ptr [gCurrent_frontend_spec]
            mov dword ptr [edx + 0x118], ecx
            mov eax, dword ptr [gCurrent_frontend_spec]
            xor edx, edx
            cmp dword ptr [eax + 0x8790], ebx
            jle L004676cc
            xor ecx, ecx
    L004676ad:
            mov ebp, dword ptr [eax + ecx + 0x87a0]
            inc edx
            mov dword ptr [eax + ecx + 0x87c0], ebp
            mov eax, dword ptr [gCurrent_frontend_spec]
            add ecx, 0x34
            cmp edx, dword ptr [eax + 0x8790]
            jl L004676ad
    L004676cc:
            mov ecx, edi
            call FRONTEND_DestroyMenu
            mov ecx, dword ptr [gFrontend_selected_item_index]
            mov eax, ecx
            shl eax, 3
            sub eax, ecx
            lea eax, [eax + eax*2]
            lea ecx, [ecx + eax*2]
            mov edx, dword ptr [edi + ecx*8 + 0x138]
            mov ecx, edi
            mov dword ptr [gCurrent_frontend_spec], edx
            call Morph_Initialise
            mov ecx, dword ptr [gCurrent_frontend_spec]
            call FRONTEND_CreateMenu
            mov ecx, dword ptr [gCurrent_frontend_spec]
            cmp ecx, offset gFrontend_QUIT
            jne L0046771b
            mov dword ptr [gFrontend_selected_item_index], ebx
            jmp L00467727
    L0046771b:
            mov edx, dword ptr [ecx + 0x118]
            mov dword ptr [gFrontend_selected_item_index], edx
    L00467727:
            mov eax, dword ptr [ecx + 0x8790]
            xor edx, edx
            cmp eax, ebx
            jle L00467755
            xor eax, eax
    L00467735:
            mov ebx, dword ptr [ecx + eax + 0x87c0]
            inc edx
            mov dword ptr [ecx + eax + 0x87a0], ebx
            mov ecx, dword ptr [gCurrent_frontend_spec]
            add eax, 0x34
            cmp edx, dword ptr [ecx + 0x8790]
            jl L00467735
    L00467755:
            call UpdateScrollPositions
    L0046775a:
            mov ecx, dword ptr [gEffects_outlet]
            mov edx, 0xbbc
            call DRS3StartSound
            mov eax, esi
            shl eax, 3
            sub eax, esi
            lea eax, [eax + eax*2]
            lea ecx, [esi + eax*2]
            mov eax, dword ptr [edi + ecx*8 + 0x13c]
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x10
            ret
    L00467786:
            mov esi, dword ptr [gCurrent_frontend_spec]
            mov dword ptr [gFrontend_selected_item_index], 0xffffffff
            xor ecx, ecx
            mov eax, dword ptr [esi + 0x104]
            lea edx, [eax - 1]
            cmp edx, ebx
            jl L004674ee
            lea eax, [esi + 0x15c]
    L004677af:
            cmp dword ptr [eax - 4], ebx
            jle L004677b8
            cmp dword ptr [eax], ebx
            jne L004677c7
    L004677b8:
            inc ecx
            add eax, 0x158
            cmp ecx, edx
            jle L004677af
            jmp L004674ee
    L004677c7:
            mov eax, ecx
            jmp L004674f0
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00467130
int C2_HOOK_FASTCALL ChooseSkill(tFrontend_spec* pFrontend) {
    switch (gFrontend_selected_item_index) {
    case 2:
        gProgram_state.skill_level = 0;
        InitGameAccordingToSkillLevel();
        break;
    case 3:
        gProgram_state.skill_level = 1;
        InitGameAccordingToSkillLevel();
        break;
    case 4:
        gProgram_state.skill_level = 2;
        InitGameAccordingToSkillLevel();
        break;
    }
    return 0;
}