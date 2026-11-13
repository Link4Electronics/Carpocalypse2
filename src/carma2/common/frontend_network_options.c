#include "frontend_network_options.h"

#include "frontend.h"
#include "frontend_main.h"
#include "frontend_network.h"
#include "frontend_quit.h"
#include "globvars.h"
#include "input.h"
#include "interface.h"
#include "loading.h"
#include "newgame.h"
#include "platform.h"

#include "c2_string.h"
// GLOBAL: CARMA2_HW 0x006864a0
tNet_game_options gFrontend_backup_net_options;

// GLOBAL: CARMA2_HW 0x006886e8
tStruct_00686508 gUNK_006886e8;

// GLOBAL: CARMA2_HW 0x006883b0
tStruct_00686508 gUNK_006883b0;

static __inline void C2_HOOK_FASTCALL NetOptions_GenericInfunc(tFrontend_spec* pFrontend) {

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
    gCurrent_frontend_scrollbars = NULL;
    gConnected_items = NULL;
    gPTR_00686508 = NULL;
}

__inline void C2_HOOK_FASTCALL DisplayNetworkOptions(tFrontend_spec* pFrontend) {

    if (gFrontend_net_options.show_players_on_map) {
        SelectThisItemIn(pFrontend, 1, 2);
    } else {
        SelectThisItemIn(pFrontend, 1, 3);
    }
    if (gFrontend_net_options.powerup_respawn) {
        SelectThisItemIn(pFrontend, 2, 5);
    } else {
        SelectThisItemIn(pFrontend, 2, 6);
    }
    if (gFrontend_net_options.open_game) {
        SelectThisItemIn(pFrontend, 3, 8);
    } else {
        SelectThisItemIn(pFrontend, 3, 9);
    }
    if (gFrontend_net_options.grid_start) {
        SelectThisItemIn(pFrontend, 4, 11);
    } else {
        SelectThisItemIn(pFrontend, 4, 12);
    }
    switch (gFrontend_net_options.race_sequence_type) {
    case 0:
        SelectThisItemIn(pFrontend, 5, 14);
        break;
    case 1:
        SelectThisItemIn(pFrontend, 5, 15);
        break;
    }
    if (gFrontend_net_options.random_car_choice) {
        SelectThisItemIn(pFrontend, 6, 17);
    } else {
        SelectThisItemIn(pFrontend, 6, 18);
    }
    switch (gFrontend_net_options.car_choice) {
    case 0:
        SelectThisItemIn(pFrontend, 7, 20);
        break;
    case 1:
        SelectThisItemIn(pFrontend, 7, 21);
        break;
    case 2:
        SelectThisItemIn(pFrontend, 7, 22);
        break;
    }
    sprintf(pFrontend->items[24].text, "%i", gFrontend_net_options.starting_credits);
    switch (gFrontend_game_type) {
    case eNet_game_type_1:
        pFrontend->items[25].stringId = 233;
        break;
    case eNet_game_type_5:
        pFrontend->items[25].stringId = 234;
        break;
    case eNet_game_type_foxy:
        pFrontend->items[25].stringId = 235;
        break;
    case eNet_game_type_fight_to_death:
    case eNet_game_type_2:
    case eNet_game_type_checkpoint:
    case eNet_game_type_4:
    case eNet_game_type_6:
        pFrontend->items[25].stringId = 232;
        break;
    }
    pFrontend->items[25].enabled = kFrontendItemEnabled_enabled;
    pFrontend->items[26].enabled = kFrontendItemEnabled_enabled;
    pFrontend->items[30].enabled = kFrontendItemEnabled_enabled;
    pFrontend->items[29].enabled = kFrontendItemEnabled_enabled;
    switch (gFrontend_game_type) {
    case eNet_game_type_1:
    case eNet_game_type_5:
        sprintf(pFrontend->items[26].text, "%i", gFrontend_net_options.starting_target);
        break;
    case eNet_game_type_foxy:
        {
            int seconds = gFrontend_net_options.starting_target / 1000;
            int hours = seconds / 3600;
            int minutes = (seconds / 60) % 60;

            seconds %= 60;
            if (hours != 0) {
                sprintf(pFrontend->items[26].text, "%02i:%02i:%02i", hours, minutes, seconds);
            } else {
                sprintf(pFrontend->items[26].text, "%02i:%02i", minutes, seconds);
            }
        }
        break;
    case eNet_game_type_fight_to_death:
    case eNet_game_type_2:
    case eNet_game_type_checkpoint:
    case eNet_game_type_4:
    case eNet_game_type_6:
        pFrontend->items[25].enabled = kFrontendItemEnabled_disabled;
        pFrontend->items[26].enabled = kFrontendItemEnabled_disabled;
        pFrontend->items[30].enabled = kFrontendItemEnabled_disabled;
        pFrontend->items[29].enabled = kFrontendItemEnabled_disabled;
        strcpy(pFrontend->items[26].text, "----");
        break;
    }
    FuckWithWidths(pFrontend);
}

void C2_HOOK_FASTCALL BackupNetworkOptions(void) {

    C2_HOOK_BUG_ON(sizeof(gFrontend_net_options) != 0x30);
    C2_HOOK_BUG_ON(sizeof(gFrontend_backup_net_options) != 0x30);
    gFrontend_backup_net_options = gFrontend_net_options;
}

#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern int C2_HOOK_FASTCALL SaveOptions(void);
extern int C2_HOOK_FASTCALL WhichItemIsSelectedIn(tFrontend_spec* pFrontend, int pGroup);
extern tNet_game_options gFrontend_net_options;
extern int gNet_last_game_type;
extern tNet_game_options gNet_settings[9];
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_FASTCALL FuckWithWidths(tFrontend_spec* pFrontend);
extern tU32 C2_HOOK_FASTCALL PDGetTotalTime(void);
extern tNet_game_type gFrontend_game_type;
extern tU32 gFrontend_net_current_roll;
extern tU32 gFrontend_net_last_roll;
extern int gFrontend_selected_item_index;
#endif
// FUNCTION: CARMA2_HW 0x00472ed0
int C2_HOOK_FASTCALL NetOptions_Infunc(tFrontend_spec* pFrontend) {

    C2_HOOK_BUG_ON(sizeof(tNet_game_options) != 0x30);

    NetOptions_GenericInfunc(pFrontend);
    DisplayNetworkOptions(pFrontend);
    BackupNetworkOptions();
    gUNK_006886e8.field_0x0 = 24;
    gUNK_006886e8.field_0x4 = 28;
    gUNK_006886e8.field_0x8 = 27;
    gUNK_006883b0.field_0x0 = 26;
    gUNK_006883b0.field_0x4 = 30;
    gUNK_006883b0.field_0x8 = 29;
    gUNK_006886e8.next = &gUNK_006883b0;
    gUNK_006883b0.next = NULL;
    gPTR_00686508 = &gUNK_006886e8;
    return 0;
}

// FUNCTION: CARMA2_HW 0x00473210
int C2_HOOK_FASTCALL NetOptions_Outfunc(tFrontend_spec* pFrontend) {
    return 0;
}

// FUNCTION: CARMA2_HW 0x00473220
int C2_HOOK_FASTCALL NetOptions_CreditsRoller(tFrontend_spec* pFrontend) {

    gFrontend_net_current_roll = PDGetTotalTime();
    if ((int)(gFrontend_net_current_roll - gFrontend_net_last_roll) >= 300) {
        gFrontend_net_last_roll = gFrontend_net_current_roll;
        if (gFrontend_selected_item_index == 28) {
            gFrontend_net_options.starting_credits += 1000;
        } else if (gFrontend_selected_item_index == 27) {
            gFrontend_net_options.starting_credits -= 1000;
        }
        if (gFrontend_net_options.starting_credits < 0) {
            gFrontend_net_options.starting_credits = 0;
        } else if (gFrontend_net_options.starting_credits > 500000) {
            gFrontend_net_options.starting_credits = 500000;
        }
        sprintf(pFrontend->items[24].text, "%i", gFrontend_net_options.starting_credits);
        FuckWithWidths(pFrontend);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004732b0
int C2_NAKED C2_HOOK_FASTCALL NetOptions_TargetRoller(tFrontend_spec* pFrontend) {
#ifndef CARPOCALYPSE2_MATCHING
    int increment;
    int minimum;
    int maximum;

    gFrontend_net_current_roll = PDGetTotalTime();
    if (gFrontend_net_current_roll - gFrontend_net_last_roll < 300) {
        return 0;
    }
    switch (gFrontend_game_type) {
    case eNet_game_type_1:
        increment = 1;
        minimum = 1;
        maximum = 64;
        break;
    case eNet_game_type_5:
        increment = 1;
        minimum = 1;
        maximum = 100;
        break;
    case eNet_game_type_foxy:
        increment = 30 * 1000;
        minimum = 30 * 1000;
        maximum = 2 * 60 * 60 * 1000;
        break;
    default:
        gFrontend_net_last_roll = gFrontend_net_current_roll;
        return 0;
    }
    if (gFrontend_selected_item_index == 29) {
        gFrontend_net_options.starting_target -= increment;
    } else if ((gFrontend_selected_item_index == 30)) {
        gFrontend_net_options.starting_target += increment;
    }
    if (gFrontend_net_options.starting_target < minimum) {
        gFrontend_net_options.starting_target = minimum;
    } else if (gFrontend_net_options.starting_target > maximum) {
        gFrontend_net_options.starting_target = maximum;
    }
    gFrontend_net_last_roll = gFrontend_net_current_roll;
    switch (gFrontend_game_type) {
    case eNet_game_type_fight_to_death:
    case eNet_game_type_2:
    case eNet_game_type_checkpoint:
    case eNet_game_type_4:
    case eNet_game_type_6:
        strcpy(pFrontend->items[26].text, "----");
        break;
    case eNet_game_type_1:
    case eNet_game_type_5:
        sprintf(pFrontend->items[26].text, "%i", gFrontend_net_options.starting_target);
        break;
    case eNet_game_type_foxy:
        {
            int seconds = gFrontend_net_options.starting_target / 1000;
            int minutes = (seconds / 60) % 60;
            int hours = seconds / 3600;
            if (hours == 0) {
                sprintf(pFrontend->items[26].text, "%02i:%02i", minutes, seconds % 60);
            } else {
                sprintf(pFrontend->items[26].text, "%02i:%02i:%02i", hours, minutes, seconds % 60);
            }
        }
        break;
    default:
        break;
    }
    FuckWithWidths(pFrontend);
    return 0;
#else
    __asm {
            push ebx
            push esi
            push edi
            mov ebx, ecx
            call PDGetTotalTime
            mov esi, dword ptr ds:[gFrontend_net_last_roll]
            mov ecx, eax
            sub ecx, esi
            mov dword ptr ds:[gFrontend_net_current_roll], eax
            cmp ecx, 0x12c
            jl L00473444
            mov edi, dword ptr ds:[gFrontend_game_type]
            mov dword ptr ds:[gFrontend_net_last_roll], eax
            mov eax, edi
            dec eax
            je L0047330f
            sub eax, 4
            je L00473301
            sub eax, 2
            jne L00473444
            mov eax, 0x7530
            mov esi, 0x6ddd00
            mov edx, eax
            jmp L0047331b
    L00473301:
            mov eax, 1
            mov esi, 0x64
            mov edx, eax
            jmp L0047331b
    L0047330f:
            mov eax, 1
            mov esi, 0x40
            mov edx, eax
    L0047331b:
            mov ecx, dword ptr ds:[gFrontend_selected_item_index]
            cmp ecx, 0x1e
            jne L00473330
            mov ecx, dword ptr ds:[0x763b3c]
            add ecx, eax
            jmp L0047333d
    L00473330:
            cmp ecx, 0x1d
            mov ecx, dword ptr ds:[0x763b3c]
            jne L00473343
            sub ecx, eax
    L0047333d:
            mov dword ptr ds:[0x763b3c], ecx
    L00473343:
            cmp ecx, edx
            jge L0047334b
            mov ecx, edx
            jmp L00473351
    L0047334b:
            cmp ecx, esi
            jle L00473357
            mov ecx, esi
    L00473351:
            mov dword ptr ds:[0x763b3c], ecx
    L00473357:
            cmp edi, 7
            ja L0047343d
            jmp dword ptr [edi*4 + 0x47344c]
            push ecx
            lea edx, [ebx + 0x2479]
            push 0x655aa0
            push edx
            call sprintf
            add esp, 0xc
            mov ecx, ebx
            call FuckWithWidths
            xor eax, eax
            pop edi
            pop esi
            pop ebx
            ret
            mov eax, 0x10624dd3
            mov edi, 0x3c
            imul ecx
            sar edx, 6
            mov eax, edx
            shr eax, 0x1f
            add edx, eax
            mov eax, 0x91a2b3c5
            mov ecx, edx
            imul ecx
            add edx, ecx
            sar edx, 0xb
            mov eax, edx
            shr eax, 0x1f
            add edx, eax
            mov eax, 0x88888889
            mov esi, edx
            imul ecx
            mov eax, edx
            add eax, ecx
            sar eax, 5
            mov edx, eax
            shr edx, 0x1f
            add eax, edx
            cdq
            idiv edi
            mov eax, ecx
            mov ecx, 0x3c
            mov edi, edx
            cdq
            idiv ecx
            test esi, esi
            push edx
            push edi
            je L00473402
            push esi
            lea edx, [ebx + 0x2479]
            push 0x655b0c
            push edx
            call sprintf
            add esp, 0x14
            mov ecx, ebx
            call FuckWithWidths
            xor eax, eax
            pop edi
            pop esi
            pop ebx
            ret
    L00473402:
            lea eax, [ebx + 0x2479]
            push 0x655b00
            push eax
            call sprintf
            add esp, 0x10
            mov ecx, ebx
            call FuckWithWidths
            xor eax, eax
            pop edi
            pop esi
            pop ebx
            ret
            mov edx, dword ptr ds:[0x655af8]
            lea ecx, [ebx + 0x2479]
            mov dword ptr [ebx + 0x2479], edx
            mov al, byte ptr ds:[0x655afc]
            mov byte ptr [ecx + 4], al
    L0047343d:
            mov ecx, ebx
            call FuckWithWidths
    L00473444:
            pop edi
            pop esi
            xor eax, eax
            pop ebx
            ret
            mov edi, edi
            _emit 0x23
            _emit 0x34
            _emit 0x47
            _emit 0x00
            _emit 0x67
            _emit 0x33
            _emit 0x47
            _emit 0x00
            _emit 0x23
            _emit 0x34
            _emit 0x47
            _emit 0x00
            _emit 0x23
            _emit 0x34
            _emit 0x47
            _emit 0x00
            _emit 0x23
            _emit 0x34
            _emit 0x47
            _emit 0x00
            _emit 0x67
            _emit 0x33
            _emit 0x47
            _emit 0x00
            _emit 0x23
            _emit 0x34
            _emit 0x47
            _emit 0x00
            _emit 0x89
            _emit 0x33
            _emit 0x47
            _emit 0x00
            _emit 0x90
            _emit 0x90
            _emit 0x90
            _emit 0x90
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00473470
int C2_NAKED C2_HOOK_FASTCALL NetOptions_Ok(tFrontend_spec* pFrontend) {
#ifndef CARPOCALYPSE2_MATCHING

    if (WhichItemIsSelectedIn(pFrontend, 1) == 2) {
        gFrontend_net_options.show_players_on_map = 1;
    } else {
        gFrontend_net_options.show_players_on_map = 0;
    }
    if (WhichItemIsSelectedIn(pFrontend, 2) == 5) {
        gFrontend_net_options.powerup_respawn = 1;
    } else {
        gFrontend_net_options.powerup_respawn = 0;
    }
    if (WhichItemIsSelectedIn(pFrontend, 3) == 8) {
        gFrontend_net_options.open_game = 1;
    } else {
        gFrontend_net_options.open_game = 0;
    }
    if (WhichItemIsSelectedIn(pFrontend, 4) == 11) {
        gFrontend_net_options.grid_start = 1;
    } else {
        gFrontend_net_options.grid_start = 0;
    }
    if (WhichItemIsSelectedIn(pFrontend, 6) == 17) {
        gFrontend_net_options.random_car_choice = 1;
    } else {
        gFrontend_net_options.random_car_choice = 0;
    }
    gFrontend_net_options.race_sequence_type = WhichItemIsSelectedIn(pFrontend, 5) - 14;
    gFrontend_net_options.car_choice = WhichItemIsSelectedIn(pFrontend, 7) - 20;

    C2_HOOK_BUG_ON(sizeof(gNet_settings[0]) != 0x30);
    C2_HOOK_BUG_ON(sizeof(gFrontend_net_options) != 0x30);
    memcpy(&gNet_settings[0], &gFrontend_net_options, sizeof(gFrontend_net_options));
    memcpy(&gNet_settings[gNet_last_game_type], &gFrontend_net_options, sizeof(gFrontend_net_options));
    SaveOptions();
    return 0;
#else
    __asm {
            push esi
            push edi
            mov esi, ecx
            mov edx, 1
            call WhichItemIsSelectedIn
            sub eax, 2
            mov edx, 2
            neg eax
            sbb eax, eax
            mov ecx, esi
            inc eax
            mov dword ptr [gFrontend_net_options], eax
            call WhichItemIsSelectedIn
            sub eax, 5
            mov edx, 3
            neg eax
            sbb eax, eax
            mov ecx, esi
            inc eax
            mov dword ptr [gFrontend_net_options+0xc], eax
            call WhichItemIsSelectedIn
            sub eax, 8
            mov edx, 4
            neg eax
            sbb eax, eax
            mov ecx, esi
            inc eax
            mov dword ptr [gFrontend_net_options+0x10], eax
            call WhichItemIsSelectedIn
            sub eax, 0xb
            mov edx, 6
            neg eax
            sbb eax, eax
            mov ecx, esi
            inc eax
            mov dword ptr [gFrontend_net_options+0x18], eax
            call WhichItemIsSelectedIn
            sub eax, 0x11
            mov edx, 5
            neg eax
            sbb eax, eax
            mov ecx, esi
            inc eax
            mov dword ptr [gFrontend_net_options+0x24], eax
            call WhichItemIsSelectedIn
            sub eax, 0xe
            mov edx, 7
            mov ecx, esi
            mov dword ptr [gFrontend_net_options+0x28], eax
            call WhichItemIsSelectedIn
            sub eax, 0x14
            mov ecx, 0xc
            mov dword ptr [gFrontend_net_options+0x2c], eax
            mov eax, dword ptr [gNet_last_game_type]
            mov esi, offset gFrontend_net_options
            mov edi, offset gNet_settings
            rep movsd
            lea edi, [eax + eax*2]
            mov ecx, 0xc
            shl edi, 4
            add edi, offset gNet_settings+0x30
            mov esi, offset gFrontend_net_options
            rep movsd
            call SaveOptions
            xor eax, eax
            pop edi
            pop esi
            ret
            nop
    }
#endif
}

void C2_HOOK_FASTCALL RestoreNetworkOptions(void) {

    C2_HOOK_BUG_ON(sizeof(gFrontend_backup_net_options) != 0x30);
    memmove(&gFrontend_net_options, &gFrontend_backup_net_options, sizeof(gFrontend_backup_net_options));
}

// FUNCTION: CARMA2_HW 0x00473550
int C2_NAKED C2_HOOK_FASTCALL NetOptions_Cancel(tFrontend_spec* pFrontend) {
#ifndef CARPOCALYPSE2_MATCHING

    RestoreNetworkOptions();
    return 0;
#else
    __asm {
            push esi
            push edi
            mov ecx, 0xc
            mov esi, offset gFrontend_backup_net_options
            mov edi, offset gFrontend_net_options
            xor eax, eax
            rep movsd
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
    }
#endif
}