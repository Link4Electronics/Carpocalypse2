/*
 * Change-car menu (ported from frontend_changecar.c — real implementation).
 */
#include "interface-changecar.h"

#include "interface.h"
#include "interface-main.h"
#include "globvars.h"
#include "globvrpb.h"
#include "utility.h"

#include "c2_string.h"


#include "globvars.h"


/* forward declarations */
int C2_HOOK_FASTCALL ChangeCarMenuInfunc(tFrontend_spec* pFrontend);
int C2_HOOK_FASTCALL ChangeCarMenuOutfunc(tFrontend_spec* pFrontend);
int C2_HOOK_FASTCALL FRONTEND_ChangeCarMenuHandler(tFrontend_spec* pFrontend);
int C2_HOOK_FASTCALL CarClickPrev(tFrontend_spec* pFrontend);
int C2_HOOK_FASTCALL CarClickNext(tFrontend_spec* pFrontend);
int C2_HOOK_FASTCALL ChangeCarOK(tFrontend_spec* pFrontend);
int C2_HOOK_FASTCALL ChangeCarCancel(tFrontend_spec* pFrontend);
void C2_HOOK_FASTCALL UpdateCarInfo(tFrontend_spec* pFrontend);
extern tS3_outlet* gIndexed_outlets[7];
extern int C2_HOOK_FASTCALL PDGetTotalTime(void);
extern void C2_HOOK_FASTCALL PollKeys(void);
extern void C2_HOOK_FASTCALL EdgeTriggerModeOff(void);
extern void C2_HOOK_FASTCALL EdgeTriggerModeOn(void);
extern void C2_HOOK_FASTCALL WaitForNoKeys(void);
extern void C2_HOOK_FASTCALL GetMousePosition(int* pX, int* pY);
extern int C2_HOOK_FASTCALL EitherMouseButtonDown(void);
extern int C2_HOOK_FASTCALL PDKeyDown(int pKey_index);
extern void C2_HOOK_FASTCALL DRS3StartSound(int pOutlet, int pSound_index);
extern tFrontend_spec gFrontend_MAIN;
extern int gFrontend_car_image_outdated;
extern int gFrontend_opponent_profile_pic_needs_update;
extern tU32 gFrontend_time_last_input;
extern int gFrontend_scrollbars_updated;
extern int gFrontend_scroll_time_increment;
extern tU32 gFrontend_scroll_last_update;
extern int gFrontend_scroll_time_left;
extern int gPrev_frontend_mouse_down;
extern int gFrontend_mouse_down;
extern int gMouse_in_use;
extern int gFrontend_selected_item_index;
extern int gFrontend_leave_current_menu;
extern tFrontend_spec gFrontend_QUIT;
extern int gEffects_outlet;

// GLOBAL: CARMA2_HW 0x005d6410
tFrontend_spec gFrontend_CHANGE_CAR = {
    "ChangeCar",
    0,
    15,
    ChangeCarMenuInfunc,
    ChangeCarMenuOutfunc,
    FRONTEND_ChangeCarMenuHandler,
    &gFrontend_MAIN,
    0,
    0,
    0,
    5,
    0,
    4,
    {
        { 0x402, CarClickPrev,      NULL,                   0, 0, 0, 0, 0, 0, 0, 1, 1 },
        { 0x402, CarClickNext,      NULL,                   0, 0, 0, 0, 0, 0, 0, 1, 1 },
        { 0x404, temp,              NULL,                   0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 0x404, temp,              NULL,                   0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 0x404, temp,              NULL,                   0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 0x404, temp,              NULL,                   0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 0x402, temp,              NULL,                   0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 0x402, temp,              NULL,                   0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 0x402, temp,              NULL,                   0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 0x402, temp,              NULL,                   0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 0x402, temp,              NULL,                   0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 0x402, temp,              NULL,                   0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 0x402, temp,              NULL,                   0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 0x7,   ChangeCarOK,       &gFrontend_MAIN,   0, 0, 0, 0, 0, 0, 0, 1, 1 },
        { 0x8,   ChangeCarCancel,   &gFrontend_MAIN,   0, 0, 0, 0, 0, 0, 0, 1, 1 },
    },
};

// GLOBAL: CARMA2_HW 0x00764ee8
int gFrontend_change_car_selected_car;


#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern int C2_HOOK_FASTCALL FRONTEND_CreateMenu(tFrontend_spec* pFrontend_spec);
extern int C2_HOOK_FASTCALL FRONTEND_DestroyMenu(tFrontend_spec* pFrontend);
extern int C2_HOOK_FASTCALL GetItemAtMousePos(tFrontend_spec *pFrontend, int pX, int pY);
extern void C2_HOOK_FASTCALL Morph_Initialise(tFrontend_spec* pCurrent, tFrontend_spec* pNext);
extern void C2_HOOK_FASTCALL ToggleSelection(tFrontend_spec* pFrontend);
extern void C2_HOOK_FASTCALL UpdateScrollPositions(tFrontend_spec* pFrontend);
#endif
// FUNCTION: CARMA2_HW 0x0046b4e0
int C2_HOOK_FASTCALL ChangeCarMenuInfunc(tFrontend_spec* pFrontend) {
    int i;
    int count;

    gFrontend_change_car_selected_car = 0;
    if (gProgram_state.number_of_cars >= 2 || gProgram_state.game_completed) {
        pFrontend->items[0].enabled = kFrontendItemEnabled_enabled;
        pFrontend->items[1].enabled = kFrontendItemEnabled_enabled;
    } else {
        pFrontend->items[0].enabled = kFrontendItemEnabled_default;
        pFrontend->items[1].enabled = kFrontendItemEnabled_default;
    }
    /* DefaultInfunc is in another translation unit here; the original
       inlines it into this function. Expand it by hand so VC5 emits
       the same body. */
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
    gFrontend_time_last_input = PDGetTotalTime();
    gFrontend_car_image_outdated = 1;
    gFrontend_opponent_profile_pic_needs_update = 1;
    FuckWithWidths(pFrontend);
    for (i = 0; i < gProgram_state.number_of_cars; i++) {
        if (gProgram_state.cars_available[i] == gProgram_state.current_car_index) {
            gFrontend_change_car_selected_car = i;
            break;
        }
    }
    if (gProgram_state.game_completed) {
        count = 0;
        for (i = 0; i < gNumber_of_racers; i++) {
            if (gOpponents[i].network_availability != eNet_avail_never) {
                gProgram_state.cars_available[count] = i;
                count += 1;
            }
        }
        gProgram_state.number_of_cars = count;
    }
    gFrontend_scroll_time_increment = 50;
    gFrontend_scroll_last_update = PDGetTotalTime();
    gFrontend_scroll_time_left = 0;
    gPrev_frontend_mouse_down = 0;
    gFrontend_mouse_down = 0;
    return 1;
}

// FUNCTION: CARMA2_HW 0x0046ba70
int C2_HOOK_FASTCALL ChangeCarMenuOutfunc(tFrontend_spec* pFrontend) {
    return 1;
}

// FUNCTION: CARMA2_HW 0x0046ba80
int C2_NAKED C2_HOOK_FASTCALL FRONTEND_ChangeCarMenuHandler(tFrontend_spec* pFrontend) {
#ifndef CARPOCALYPSE2_MATCHING
    tU32 now;

    if (gFrontend_car_image_outdated || gFrontend_opponent_profile_pic_needs_update) {
        UpdateCarInfo(pFrontend);
    }
    PollKeys();

    now = PDGetTotalTime();
    gFrontend_scroll_time_left -= now - gFrontend_scroll_last_update;
    if (gFrontend_scroll_time_left < 0) {
        gFrontend_scroll_time_left = 0;
    }
    gFrontend_scroll_time_increment = 50;
    gFrontend_scroll_last_update = now;

    if (gMouse_in_use) {
        int mouse_x;
        int mouse_y;

        ResetInterfaceTimeout();
        GetMousePosition(&mouse_x, &mouse_y);
        gFrontend_selected_item_index = GetItemAtMousePos(pFrontend, mouse_x, mouse_y);
        if (gFrontend_selected_item_index == -1) {
            gFrontend_selected_item_index = 2;
        }
        gPrev_frontend_mouse_down = gFrontend_mouse_down;
        gFrontend_mouse_down = EitherMouseButtonDown();
        if (gFrontend_mouse_down && !gPrev_frontend_mouse_down) {
            gFrontend_scroll_time_increment = 300;
        } else {
            gFrontend_scroll_time_increment = 100;
        }
    }

    if (PDKeyDown(51) || PDKeyDown(52) || gFrontend_mouse_down) {

        ToggleSelection(pFrontend);
        switch (pFrontend->items[gFrontend_selected_item_index].field_0xc) {
        case 2:
            return 2;
        case 1:
            gFrontend_leave_current_menu = 1;
            break;
        }
        if (pFrontend->items[gFrontend_selected_item_index].action != NULL) {
            pFrontend->items[gFrontend_selected_item_index].action(pFrontend);
        }
        if (pFrontend->items[gFrontend_selected_item_index].menuInfo != NULL) {
            int i;

            gCurrent_frontend_spec->default_item = gFrontend_selected_item_index;
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
            UpdateScrollPositions(gCurrent_frontend_spec);
        }
    }
    if (gFrontend_leave_current_menu) {
        return 1;
    }
    else {
        return 0;
    }
#else
    __asm {
            mov eax, dword ptr [gFrontend_car_image_outdated]
            sub esp, 8
            test eax, eax
            push esi
            mov esi, ecx
            jne L0046ba98
            mov eax, dword ptr [gFrontend_opponent_profile_pic_needs_update]
            test eax, eax
            je L0046ba9f
    L0046ba98:
            mov ecx, esi
            call UpdateCarInfo
    L0046ba9f:
            call PollKeys
            call PDGetTotalTime
            mov edx, dword ptr [gFrontend_scroll_last_update]
            mov ecx, eax
            mov dword ptr [gFrontend_scroll_last_update], eax
            mov eax, dword ptr [gFrontend_scroll_time_left]
            sub ecx, edx
            sub eax, ecx
            mov dword ptr [gFrontend_scroll_time_left], eax
            jns L0046bad0
            mov dword ptr [gFrontend_scroll_time_left], 0
    L0046bad0:
            mov eax, dword ptr [gMouse_in_use]
            mov dword ptr [gFrontend_scroll_time_increment], 0x32
            test eax, eax
            je L0046bb55
            call PDGetTotalTime
            lea edx, [esp + 4]
            lea ecx, [esp + 8]
            mov dword ptr [gFrontend_time_last_input], eax
            call GetMousePosition
            mov eax, dword ptr [esp + 4]
            mov edx, dword ptr [esp + 8]
            push eax
            mov ecx, esi
            call GetItemAtMousePos
            cmp eax, -1
            mov dword ptr [gFrontend_selected_item_index], eax
            jne L0046bb1e
            mov dword ptr [gFrontend_selected_item_index], 2
    L0046bb1e:
            mov ecx, dword ptr [gFrontend_mouse_down]
            mov dword ptr [gPrev_frontend_mouse_down], ecx
            call EitherMouseButtonDown
            test eax, eax
            mov dword ptr [gFrontend_mouse_down], eax
            je L0046bb4b
            mov eax, dword ptr [gPrev_frontend_mouse_down]
            mov dword ptr [gFrontend_scroll_time_increment], 0x12c
            test eax, eax
            je L0046bb55
    L0046bb4b:
            mov dword ptr [gFrontend_scroll_time_increment], 0x64
    L0046bb55:
            mov ecx, 0x33
            call PDKeyDown
            test eax, eax
            jne L0046bb7e
            mov ecx, 0x34
            call PDKeyDown
            test eax, eax
            jne L0046bb7e
            mov eax, dword ptr [gFrontend_mouse_down]
            test eax, eax
            je L0046bc9a
    L0046bb7e:
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
            jne L0046bbac
            pop esi
            add esp, 8
            ret
    L0046bbac:
            cmp eax, 1
            jne L0046bbb6
            mov dword ptr [gFrontend_leave_current_menu], eax
    L0046bbb6:
            mov edx, dword ptr [edx + 0x134]
            test edx, edx
            je L0046bbca
            mov ecx, esi
            call edx
            mov ecx, dword ptr [gFrontend_selected_item_index]
    L0046bbca:
            mov eax, ecx
            shl eax, 3
            sub eax, ecx
            lea edx, [eax + eax*2]
            lea eax, [ecx + edx*2]
            mov edx, dword ptr [esi + eax*8 + 0x138]
            test edx, edx
            je L0046bc9a
            mov edx, dword ptr [gCurrent_frontend_spec]
            mov dword ptr [edx + 0x118], ecx
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
            je L0046bc3a
            mov dword ptr [edx + 0x114], esi
            mov edx, dword ptr [gCurrent_frontend_spec]
    L0046bc3a:
            mov ecx, esi
            call Morph_Initialise
            mov ecx, dword ptr [gCurrent_frontend_spec]
            cmp ecx, offset gFrontend_QUIT
            jne L0046bc5b
            mov dword ptr [gFrontend_selected_item_index], 0
            jmp L0046bc67
    L0046bc5b:
            mov edx, dword ptr [ecx + 0x118]
            mov dword ptr [gFrontend_selected_item_index], edx
    L0046bc67:
            mov eax, dword ptr [ecx + 0x8790]
            xor edx, edx
            test eax, eax
            jle L0046bc95
            xor eax, eax
    L0046bc75:
            mov esi, dword ptr [ecx + eax + 0x87c0]
            inc edx
            mov dword ptr [ecx + eax + 0x87a0], esi
            mov ecx, dword ptr [gCurrent_frontend_spec]
            add eax, 0x34
            cmp edx, dword ptr [ecx + 0x8790]
            jl L0046bc75
    L0046bc95:
            call UpdateScrollPositions
    L0046bc9a:
            mov ecx, dword ptr [gFrontend_leave_current_menu]
            xor eax, eax
            test ecx, ecx
            setne al
            pop esi
            add esp, 8
            ret
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0046b640
int C2_HOOK_FASTCALL CarClickPrev(tFrontend_spec* pFrontend) {
    if (gFrontend_scroll_time_left == 0) {
        gFrontend_change_car_selected_car -= 1;
        if (gFrontend_change_car_selected_car < 0) {
            gFrontend_change_car_selected_car = gProgram_state.number_of_cars - 1;
        }
        gFrontend_opponent_profile_pic_needs_update = 1;
        gFrontend_car_image_outdated = 1;
        DRS3StartSound(gEffects_outlet, eSoundId_LeftButton);
        UpdateCarInfo(pFrontend);
        gFrontend_scroll_time_left += gFrontend_scroll_time_increment;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0046ba00
int C2_HOOK_FASTCALL CarClickNext(tFrontend_spec* pFrontend) {
    if (gFrontend_scroll_time_left == 0) {
        gFrontend_change_car_selected_car += 1;
        if (gFrontend_change_car_selected_car >= gProgram_state.number_of_cars) {
            gFrontend_change_car_selected_car = 0;
        }
        gFrontend_opponent_profile_pic_needs_update = 1;
        gFrontend_car_image_outdated = 1;
        DRS3StartSound(gEffects_outlet, eSoundId_LeftButton);
        UpdateCarInfo(pFrontend);
        gFrontend_scroll_time_left += gFrontend_scroll_time_increment;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0046b620
int C2_HOOK_FASTCALL ChangeCarOK(tFrontend_spec* pFrontend) {
    gProgram_state.current_car_index = gProgram_state.cars_available[gFrontend_change_car_selected_car];
    return 0;
}

// FUNCTION: CARMA2_HW 0x0046b610
int C2_HOOK_FASTCALL ChangeCarCancel(tFrontend_spec* pFrontend) {
    return 0;
}