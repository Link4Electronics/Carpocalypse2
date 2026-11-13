#include "powerup.h"

#include "car.h"
#include "displays.h"
#include "globvars.h"
#include "graphics1.h"
#include "pedestrn.h"
#include "physics.h"
#include "pratcam.h"
#include "spark.h"
#include "globvrpb.h"
#include "powerups.h"
#include "utility.h"
#include "carpocalypse2_macros.h"

#include "c2_string.h"
// GLOBAL: CARMA2_HW 0x006a0ad4
const char* gPowerup_txt_path;

// GLOBAL: CARMA2_HW 0x007059e0
tHeadup_icon gPickedup_powerups[20];

// GLOBAL: CARMA2_HW 0x00763480
tSlot_info gCost_APO;

// GLOBAL: CARMA2_HW 0x0075b900
tSlot_info gTrade_in_value_APO;

// GLOBAL: CARMA2_HW 0x0074d380
tSlot_info gSubstitution_value_APO;

// GLOBAL: CARMA2_HW 0x00761c60
tSlot_info gPotential_substitution_value_APO;

// GLOBAL: CARMA2_HW 0x0074d1a4
int gNet_powerup_time_replacement;

// GLOBAL: CARMA2_HW 0x007447d8
int gPickup_respawn_min_time_ms;

// GLOBAL: CARMA2_HW 0x007447e8
int gPickup_respawn_max_extra_time_ms;

// GLOBAL: CARMA2_HW 0x007059c0
int gCount_mutant_tail_parts;

// GLOBAL: CARMA2_HW 0x00705b70
tU32 gINT_00705b70;

// GLOBAL: CARMA2_HW 0x00705b78
float gMass_mutant_tail_link;

// GLOBAL: CARMA2_HW 0x00705b74
float gMass_mutant_tail_ball;

// GLOBAL: CARMA2_HW 0x0065ebbc
float gMass_mine = 1.0f;

// GLOBAL: CARMA2_HW 0x006a0454
int gCount_cloaked_cars;

// GLOBAL: CARMA2_HW 0x006a0910
tCar_spec* gCloaked_cars[12];

// GLOBAL: CARMA2_HW 0x006a0a54
tPowerup* gPowerup_array;

// GetNextGoodyTime

// DoPowerupTextHeadup

// QueueIconGoing

// QueueIconComing

// LosePowerupX

// LosePowerup

// LoseKeyboardPowerupN

// KeyboardPowerupFinished

// LoseAllSimilarPowerups (implemented in powerups.c)
extern int gNumber_of_powerups;

// ExcludedPowerup

#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_FASTCALL ARPowerupGot(tPowerup* pPowerup, tCar_spec* pCar);
extern void C2_HOOK_FASTCALL ARPowerupLose(tPowerup* pPowerup, tCar_spec* pCar);
extern br_actor* C2_HOOK_CDECL BrActorAllocate();
extern br_pixelmap* C2_HOOK_CDECL BrMapAdd();
extern br_material* C2_HOOK_CDECL BrMaterialAdd();
extern br_material* C2_HOOK_CDECL BrMaterialAllocate();
extern void C2_HOOK_CDECL BrMaterialUpdate();
extern void C2_HOOK_CDECL BrMatrix34Identity();
extern void C2_HOOK_CDECL BrMatrix34PostRotateZ();
extern void C2_HOOK_CDECL BrMatrix34Scale();
extern br_model* C2_HOOK_CDECL BrModelAdd();
extern br_model* C2_HOOK_CDECL BrModelAllocate();
extern int C2_HOOK_FASTCALL GetHowMuchBloodAndSnotToSmearAbout(void);
extern const char* C2_HOOK_FASTCALL GetMiscString(int pIndex);
extern int C2_HOOK_FASTCALL GetPedCount(void);
extern tU32 C2_HOOK_FASTCALL GetTotalTime(void);
extern int C2_HOOK_FASTCALL IsItOkayToFireHorribleBallsOfNastyNapalmDeathAtPerfectlyInnocentPassersByAndByInnocentIDoMeanInTheBiblicalSense(void);
extern int C2_HOOK_FASTCALL IsItReallyOKThatWeDontMakeAnyEffortToProtectAnySadFuckersOutThereThatDontWishToSeeInnocentPeopleBlownToBitsByHighExplosiveMinesAndShells(void);
extern tNet_message* C2_HOOK_FASTCALL NetBuildGuaranteedMessage(tU8 pNet_message_type, int pOption);
extern int C2_HOOK_FASTCALL NetGuaranteedSendMessageToAllPlayers(tNet_game_details* pDetails, tNet_message* pMessage, void* pNotifyFail);
extern int C2_HOOK_FASTCALL NewTextHeadupSlot(int pSlot_index, int pFlash_rate, int pLifetime, int pFont_index, const char* pText);
extern void C2_HOOK_FASTCALL PratcamEvent(int pIndex);
extern void C2_HOOK_FASTCALL QueueIconGoing(tPowerup* pPowerup, tHeadup_icon* pHeadup_icons, int* pCount_icons);
extern void C2_HOOK_FASTCALL RenderThisHeadup(br_actor* pActor);
extern void __cdecl _ftol();
extern char gCar_map_colours[];
extern char gConst_kangeroo_force_factor[];
extern tNet_game_details* gCurrent_net_game;
extern tRace_info gCurrent_race;
extern char gDefaultOpacity_TintedPoly[];
extern char gFLOAT_0058ac00[];
extern char gFLOAT_0058ac20[];
extern char gFizzle_in[];
extern int gFreeze_timer;
extern tFrontend_spec gFrontend_QUIT;
extern int gINT_006a0a5c;
extern int gINT_006a0a6c;
extern int gInventory[20];
extern int gInventory_selected;
extern tPlayer_ID gLocal_net_ID;
extern tNet_mode gNet_mode;
extern int gNumber_of_icons;
extern tProgram_state gProgram_state;
#endif
// FUNCTION: CARMA2_HW 0x004d9150
int C2_NAKED C2_HOOK_FASTCALL GotPowerupX(tCar_spec* pCar, int pIndex, int pArg3, int pMessage, tU32 pTime) {
#ifndef CARPOCALYPSE2_MATCHING
    tPowerup* the_powerup;
    int i;
    int ps_power;
    char s[256];
    char* s2;

    if (pIndex >= gNumber_of_powerups || pIndex < 0) {
        return -1;
    }
    the_powerup = &gPowerup_array[pIndex];
    if (the_powerup->type == ePowerup_dummy) {
        return -1;
    }
    if (the_powerup->got_proc == NULL) {
        return -1;
    }
    while (((gProgram_state.sausage_eater_mode || gCount_active_peds < 2)
            && (strstr(the_powerup->message, "Ped") != NULL
                || strstr(the_powerup->message, "ped") != NULL
                || strstr(the_powerup->message, "corpses") != NULL))
        || (gNet_mode != eNet_mode_none && the_powerup->net_type == eNet_powerup_inappropriate)) {
        pIndex = 0;
        the_powerup = &gPowerup_array[pIndex];
    }
    the_powerup->current_value = -1;
    LoseAllSimilarPowerups(the_powerup);
    ps_power = gNet_mode != eNet_mode_none && the_powerup->got_proc == GotTimeOrPower;
    if (the_powerup->message[0] != '\0' && pMessage && !ps_power) {
        s2 = s;
        strcpy(s2, the_powerup->message);
        if (the_powerup->got_proc == FreezeTimer) {
            s2 = strtok(s, "/");
            if (gFreeze_timer) {
                s2 = strtok(NULL, "/");
            }
        }
        NewTextHeadupSlot(4, 0, 3000, -4, s2);
    }
    the_powerup->car = pCar;
    if (the_powerup->got_proc != NULL) {
        pIndex = the_powerup->got_proc(the_powerup, pCar);
    }
    if (pCar->driver <= 5) {
        return pIndex;
    }
    switch (the_powerup->type) {
    case ePowerup_instantaneous:
        break;
    case ePowerup_timed:
        the_powerup->got_time = GetTotalTime();
        if (pArg3) {
            the_powerup->lose_time = the_powerup->got_time + the_powerup->duration;
        } else {
            the_powerup->lose_time = the_powerup->got_time + pTime;
        }
        gProgram_state.current_car.powerups[pIndex] = the_powerup->lose_time;
        break;
    case ePowerup_whole_race:
        the_powerup->got_time = GetTotalTime();
        gProgram_state.current_car.powerups[pIndex] = -1;
        break;
    default:
        break;
    }
    if (the_powerup->prat_cam_event >= 0) {
        PratcamEvent(the_powerup->prat_cam_event);
    }
    /* NOTE: retail also broadcasts a guaranteed "powerup gained" net message
     * here; omitted like the rest of the netcode (cf. CloseDownPowerUps). */
    if (the_powerup->type != ePowerup_instantaneous && the_powerup->icon != NULL) {
        for (i = 0; i < gNumber_of_icons; i++) {
            if (gPickedup_powerups[i].powerup == the_powerup) {
                gPickedup_powerups[i].fizzle_stage = 4;
                return pIndex;
            }
        }
        if (gNumber_of_icons == CARPOCALYPSE2_ASIZE(gPickedup_powerups)) {
            return pIndex;
        }
        gPickedup_powerups[gNumber_of_icons].powerup = the_powerup;
        gPickedup_powerups[gNumber_of_icons].fizzle_stage = 0;
        gPickedup_powerups[gNumber_of_icons].fizzle_direction = 1;
        gPickedup_powerups[gNumber_of_icons].fizzle_start = GetTotalTime();
        gNumber_of_icons++;
    }
    return pIndex;
#else
    __asm {
            sub esp, 0x114
            mov eax, dword ptr [gNumber_of_powerups]
            push ebx
            push ebp
            push esi
            push edi
            mov edi, edx
            cmp edi, eax
            mov dword ptr [esp + 0x10], edi
            mov dword ptr [esp + 0x1c], ecx
            jge L004d963b
            xor ebp, ebp
            cmp edi, ebp
            jl L004d963b
            mov eax, edi
            mov edx, dword ptr [gPowerup_array]
            shl eax, 3
            sub eax, edi
            lea eax, [eax + eax*2]
            lea ecx, [edi + eax*2]
            mov eax, dword ptr [edx + ecx*4]
            cmp eax, ebp
            lea ebx, [edx + ecx*4]
            mov dword ptr [esp + 0x14], ebx
            je L004d963b
            mov esi, dword ptr [gCurrent_race+0x98]
            call GetPedCount
            cmp eax, 1
            jge L004d91b6
            test byte ptr [ebx + 0x38], 0x40
            jne L004d9222
    L004d91b6:
            mov eax, dword ptr [gNet_mode]
            cmp eax, ebp
            je L004d91c9
            test byte ptr [ebx + 0x38], 2
            jne L004d9222
            cmp eax, ebp
            jne L004d91ce
    L004d91c9:
            cmp edi, 0x61
            je L004d9222
    L004d91ce:
            call IsItOkayToFireHorribleBallsOfNastyNapalmDeathAtPerfectlyInnocentPassersByAndByInnocentIDoMeanInTheBiblicalSense
            test eax, eax
            jne L004d91dc
            cmp edi, 0x5b
            je L004d9222
    L004d91dc:
            call IsItReallyOKThatWeDontMakeAnyEffortToProtectAnySadFuckersOutThereThatDontWishToSeeInnocentPeopleBlownToBitsByHighExplosiveMinesAndShells
            test eax, eax
            jne L004d91ef
            cmp edi, 0x34
            je L004d9222
            cmp edi, 0x60
            je L004d9222
    L004d91ef:
            call GetHowMuchBloodAndSnotToSmearAbout
            test eax, eax
            jle L004d9202
            cmp edi, 4
            je L004d9222
            cmp edi, 0x39
            je L004d9222
    L004d9202:
            mov edx, dword ptr [esi + 0x1d4]
            xor eax, eax
            cmp edx, ebp
            jle L004d922f
            mov ecx, dword ptr [esi + 0x1d8]
    L004d9214:
            cmp edi, dword ptr [ecx]
            je L004d9222
            inc eax
            add ecx, 4
            cmp eax, edx
            jl L004d9214
            jmp L004d922f
    L004d9222:
            mov eax, dword ptr [gPowerup_array]
            mov dword ptr [esp + 0x10], ebp
            mov dword ptr [esp + 0x14], eax
    L004d922f:
            mov edi, dword ptr [esp + 0x14]
            xor ebx, ebx
            mov dword ptr [edi + 0x28], 0xffffffff
            mov eax, dword ptr [gNumber_of_powerups]
            cmp eax, ebp
            jle L004d9339
            mov ecx, dword ptr [gPowerup_array]
            lea esi, [ecx + 0x40]
    L004d9252:
            lea edx, [esi - 0x40]
            cmp edi, edx
            je L004d9325
            cmp dword ptr [esi - 0x3c], ebp
            je L004d9325
            mov ax, word ptr [edi + 0x10]
            test word ptr [esi - 0x30], ax
            je L004d9325
            mov eax, dword ptr [esi]
            cmp eax, ebp
            je L004d9282
            mov edx, dword ptr [esi + 0xc]
            lea ecx, [esi - 0x40]
            call eax
    L004d9282:
            test byte ptr [esi - 8], 0x80
            je L004d9293
            mov edx, dword ptr [esi + 0xc]
            lea ecx, [esi - 0x40]
            call ARPowerupLose
    L004d9293:
            cmp dword ptr [gNet_mode], ebp
            je L004d92e8
            xor edx, edx
            mov cl, 0x16
            call NetBuildGuaranteedMessage
            mov edi, eax
            push ebp
            mov dword ptr [edi + 0x28], 2
            mov ecx, dword ptr [gLocal_net_ID]
            mov dword ptr [edi + 0x1c], ecx
            mov eax, dword ptr [gPowerup_array]
            mov ecx, esi
            sub ecx, eax
            mov eax, 0x2fa0be83
            sub ecx, 0x40
            imul ecx
            sar edx, 5
            mov eax, edx
            shr eax, 0x1f
            add edx, eax
            mov dword ptr [edi + 0x20], edx
            mov ecx, dword ptr [gCurrent_net_game]
            mov edx, edi
            call NetGuaranteedSendMessageToAllPlayers
            mov edi, dword ptr [esp + 0x14]
    L004d92e8:
            mov eax, dword ptr [gPowerup_array]
            lea ecx, [esi - 0x40]
            mov edx, ecx
            sub edx, eax
            mov eax, 0x2fa0be83
            imul edx
            sar edx, 5
            mov eax, edx
            shr eax, 0x1f
            add edx, eax
            mov dword ptr [edx*4 + gProgram_state+0x17bc], ebp
            mov eax, dword ptr [esi - 0xc]
            cmp eax, ebp
            jne L004d9322
            push offset gNumber_of_icons
            mov edx, offset gPickedup_powerups
            call QueueIconGoing
    L004d9322:
            mov dword ptr [esi - 0x3c], ebp
    L004d9325:
            mov eax, dword ptr [gNumber_of_powerups]
            inc ebx
            add esi, 0xac
            cmp ebx, eax
            jl L004d9252
    L004d9339:
            cmp dword ptr [gNet_mode], ebp
            je L004d9351
            cmp dword ptr [edi + 0x3c], 0x4dc470
            jne L004d9351
            mov ebx, 1
            jmp L004d9353
    L004d9351:
            xor ebx, ebx
    L004d9353:
            mov ecx, dword ptr [esp + 0x14]
            mov eax, dword ptr [esp + 0x10]
            mov edx, dword ptr [esp + 0x1c]
            mov dword ptr [esp + 0x18], ebx
            mov esi, dword ptr [ecx + 0x3c]
            mov dword ptr [esp + 0x20], eax
            cmp esi, ebp
            mov dword ptr [ecx + 0x4c], edx
            je L004d9377
            call esi
            mov dword ptr [esp + 0x10], eax
    L004d9377:
            cmp eax, ebp
            jl L004d963b
            mov ecx, dword ptr [esp + 0x10]
            mov eax, ecx
            shl eax, 3
            sub eax, ecx
            lea edx, [eax + eax*2]
            lea eax, [ecx + edx*2]
            mov edx, dword ptr [gPowerup_array]
            lea ebp, [edx + eax*4]
            mov al, byte ptr [ebp + 0x50]
            test al, al
            lea edi, [ebp + 0x50]
            je L004d9466
            mov eax, dword ptr [esp + 0x12c]
            test eax, eax
            je L004d9466
            mov edx, dword ptr [ebp + 0x34]
            test edx, edx
            jne L004d9466
            test ebx, ebx
            jne L004d9466
            cmp eax, 2
            lea ebx, [esp + 0x24]
            jne L004d93f2
            push edi
            mov ecx, 0xbe
            call GetMiscString
            push eax
            lea eax, [esp + 0x2c]
            push offset gCar_map_colours+0x10
            push eax
            call sprintf
            add esp, 0x10
            jmp L004d9413
    L004d93f2:
            or ecx, 0xffffffff
            xor eax, eax
            repne scasb
            not ecx
            sub edi, ecx
            lea edx, [esp + 0x24]
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            shr ecx, 2
            rep movsd
            mov ecx, eax
            and ecx, 3
            rep movsb
    L004d9413:
            cmp dword ptr [ebp + 0x3c], 0x4dc930
            jne L004d944a
            lea ecx, [esp + 0x24]
            push offset gMass_mine+0x30
            push ecx
            call strtok
            mov ebx, eax
            mov eax, dword ptr [gFreeze_timer]
            add esp, 8
            test eax, eax
            je L004d944a
            push offset gMass_mine+0x30
            push 0
            call strtok
            add esp, 8
            mov ebx, eax
    L004d944a:
            push ebx
            push -4
            push 0xbb8
            xor edx, edx
            mov ecx, 4
            call NewTextHeadupSlot
            mov ebx, dword ptr [esp + 0x18]
            mov ecx, dword ptr [esp + 0x10]
    L004d9466:
            test byte ptr [ebp + 0x38], 0x80
            je L004d9482
            mov eax, dword ptr [ecx*4 + gProgram_state+0x17bc]
            test eax, eax
            jne L004d9482
            mov edx, dword ptr [esp + 0x1c]
            mov ecx, ebp
            call ARPowerupGot
    L004d9482:
            mov eax, dword ptr [ebp]
            mov esi, dword ptr [esp + 0x128]
            dec eax
            je L004d94f4
            dec eax
            je L004d94ae
            dec eax
            jne L004d94f4
            call GetTotalTime
            mov edi, dword ptr [esp + 0x10]
            mov dword ptr [ebp + 4], eax
            mov dword ptr [edi*4 + gProgram_state+0x17bc], 0xffffffff
            jmp L004d94f8
    L004d94ae:
            call GetTotalTime
            test esi, esi
            mov dword ptr [ebp + 4], eax
            jne L004d94dd
            mov ecx, dword ptr [gNet_mode]
            test ecx, ecx
            je L004d94dd
            mov edx, dword ptr [esp + 0x130]
            mov edi, dword ptr [esp + 0x10]
            add edx, eax
            mov dword ptr [ebp + 0xc], edx
            mov dword ptr [edi*4 + gProgram_state+0x17bc], edx
            jmp L004d94f8
    L004d94dd:
            mov ecx, dword ptr [ebp + 8]
            mov edi, dword ptr [esp + 0x10]
            add ecx, eax
            mov edx, ecx
            mov dword ptr [ebp + 0xc], ecx
            mov dword ptr [edi*4 + gProgram_state+0x17bc], edx
            jmp L004d94f8
    L004d94f4:
            mov edi, dword ptr [esp + 0x10]
    L004d94f8:
            mov ecx, dword ptr [ebp + 0x2c]
            test ecx, ecx
            jl L004d9504
            call PratcamEvent
    L004d9504:
            mov eax, dword ptr [gNet_mode]
            test eax, eax
            je L004d955b
            test esi, esi
            je L004d955b
            cmp dword ptr [esp + 0x20], edi
            jne L004d955b
            test ebx, ebx
            jne L004d955b
            cmp dword ptr [ebp + 0x3c], 0x4de1e0
            je L004d955b
            xor edx, edx
            mov cl, 0x16
            call NetBuildGuaranteedMessage
            mov dword ptr [eax + 0x28], ebx
            mov ecx, dword ptr [gLocal_net_ID]
            mov dword ptr [eax + 0x1c], ecx
            mov dword ptr [eax + 0x20], edi
            cmp dword ptr [ebp], 2
            jne L004d9547
            mov ecx, dword ptr [ebp + 8]
            jmp L004d9549
    L004d9547:
            xor ecx, ecx
    L004d9549:
            mov dword ptr [eax + 0x24], ecx
            mov ecx, dword ptr [gCurrent_net_game]
            push 0
            mov edx, eax
            call NetGuaranteedSendMessageToAllPlayers
    L004d955b:
            cmp dword ptr [ebp], 1
            je L004d962c
            mov eax, dword ptr [ebp + 0x14]
            test eax, eax
            je L004d962c
            mov eax, dword ptr [ebp + 0x34]
            test eax, eax
            jne L004d962c
            mov eax, dword ptr [gNumber_of_icons]
            xor ecx, ecx
            test eax, eax
            jle L004d959b
            mov edx, offset gPickedup_powerups
    L004d958b:
            cmp ebp, dword ptr [edx]
            je L004d964b
            inc ecx
            add edx, 0x14
            cmp ecx, eax
            jl L004d958b
    L004d959b:
            cmp eax, 0x14
            je L004d962c
            lea ecx, [eax + eax*4]
            lea esi, [eax + eax*4]
            shl ecx, 2
            shl esi, 2
            mov dword ptr [ecx + gPickedup_powerups], ebp
            mov dword ptr [ecx + gPickedup_powerups+0x4], 0
            mov dword ptr [ecx + gPickedup_powerups+0x8], 1
            call GetTotalTime
            mov dword ptr [esi + gPickedup_powerups+0xc], eax
            mov ebx, dword ptr [ebp + 0x90]
            test ebx, ebx
            je L004d961d
            mov dword ptr [esi + gPickedup_powerups+0x10], ebx
            lea edi, [ebp + 0x94]
            mov ecx, 6
            mov esi, offset gDefaultOpacity_TintedPoly+0x84
            rep movsd
            mov eax, dword ptr [ebx + 0x1c]
            push 0x40
            mov ecx, dword ptr [eax + 0x58]
            mov dword ptr [ecx + 0xc], 0xff0000
            mov edx, dword ptr [ebx + 0x1c]
            push edx
            call BrMaterialUpdate
            add esp, 8
            mov ecx, ebx
            mov byte ptr [ebx + 0x20], 1
            call RenderThisHeadup
    L004d961d:
            mov eax, dword ptr [gNumber_of_icons]
            mov edi, dword ptr [esp + 0x10]
            inc eax
            mov dword ptr [gNumber_of_icons], eax
    L004d962c:
            mov eax, edi
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x114
            ret 0xc
    L004d963b:
            or eax, 0xffffffff
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x114
            ret 0xc
    L004d964b:
            mov eax, edi
            lea edx, [ecx + ecx*4]
            pop edi
            pop esi
            pop ebp
            mov dword ptr [edx*4 + gPickedup_powerups+0x4], 4
            pop ebx
            add esp, 0x114
            ret 0xc
    }
#endif
}

// GotPowerupXX

// GotPowerupEarwig

// FUNCTION: CARMA2_HW 0x004d8d40
void C2_NAKED C2_HOOK_FASTCALL GotPowerupEarwig(tCar_spec* pCar, int pIndex, int pMessage, int pSelect_inventory) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;

    GotPowerupX(pCar, pIndex, 0, pMessage, 0);
    if (pSelect_inventory) {
        for (i = 0; i < CARPOCALYPSE2_ASIZE(gInventory); i++) {
            if (gInventory[i] < 0) {
                break;
            }
        }
        if (i < CARPOCALYPSE2_ASIZE(gInventory)) {
            gInventory[i] = pIndex;
            gInventory_selected = i;
        }
    }
#else
    __asm {
            sub esp, 0x108
            mov eax, dword ptr [gNumber_of_powerups]
            push ebx
            push ebp
            mov ebx, edx
            push esi
            push edi
            cmp ebx, eax
            mov edi, ecx
            mov dword ptr [esp + 0x14], 0xffffffff
            mov eax, ebx
            jge L004d90e7
            shl eax, 3
            mov edx, dword ptr [gPowerup_array]
            sub eax, ebx
            xor ebp, ebp
            lea eax, [eax + eax*2]
            lea ecx, [ebx + eax*2]
            lea eax, [edx + ecx*4]
            mov dword ptr [esp + 0x10], eax
            cmp dword ptr [eax + 0x34], ebp
            je L004d90b9
            mov esi, dword ptr [gCurrent_race+0x98]
            call GetPedCount
            cmp eax, 1
            jge L004d8da6
            mov eax, dword ptr [esp + 0x10]
            test byte ptr [eax + 0x38], 0x40
            jne L004d90f4
    L004d8da6:
            mov eax, dword ptr [gNet_mode]
            cmp eax, ebp
            je L004d8dc1
            mov ecx, dword ptr [esp + 0x10]
            test byte ptr [ecx + 0x38], 2
            jne L004d90f9
            cmp eax, ebp
            jne L004d8dca
    L004d8dc1:
            cmp ebx, 0x61
            je L004d90f9
    L004d8dca:
            call IsItOkayToFireHorribleBallsOfNastyNapalmDeathAtPerfectlyInnocentPassersByAndByInnocentIDoMeanInTheBiblicalSense
            test eax, eax
            jne L004d8ddc
            cmp ebx, 0x5b
            je L004d90f4
    L004d8ddc:
            call IsItReallyOKThatWeDontMakeAnyEffortToProtectAnySadFuckersOutThereThatDontWishToSeeInnocentPeopleBlownToBitsByHighExplosiveMinesAndShells
            test eax, eax
            jne L004d8df7
            cmp ebx, 0x34
            je L004d90f4
            cmp ebx, 0x60
            je L004d90f4
    L004d8df7:
            call GetHowMuchBloodAndSnotToSmearAbout
            test eax, eax
            jle L004d8e12
            cmp ebx, 4
            je L004d90f4
            cmp ebx, 0x39
            je L004d90f4
    L004d8e12:
            mov edx, dword ptr [esi + 0x1d4]
            xor eax, eax
            cmp edx, ebp
            jle L004d8e34
            mov ecx, dword ptr [esi + 0x1d8]
    L004d8e24:
            cmp ebx, dword ptr [ecx]
            je L004d90f4
            inc eax
            add ecx, 4
            cmp eax, edx
            jl L004d8e24
    L004d8e34:
            mov eax, dword ptr [esp + 0x11c]
            cmp eax, ebp
            je L004d8eec
            cmp eax, 2
            lea ebp, [esp + 0x18]
            jne L004d8e73
            mov eax, dword ptr [esp + 0x10]
            mov ecx, 0xbe
            add eax, 0x50
            push eax
            call GetMiscString
            lea ecx, [esp + 0x1c]
            push eax
            push offset gCar_map_colours+0x10
            push ecx
            call sprintf
            add esp, 0x10
            jmp L004d8e9b
    L004d8e73:
            mov edx, dword ptr [esp + 0x10]
            or ecx, 0xffffffff
            xor eax, eax
            lea edi, [edx + 0x50]
            lea edx, [esp + 0x18]
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
    L004d8e9b:
            mov ecx, dword ptr [esp + 0x10]
            cmp dword ptr [ecx + 0x3c], 0x4dc930
            jne L004d8ed6
            lea edx, [esp + 0x18]
            push offset gMass_mine+0x30
            push edx
            call strtok
            mov ebp, eax
            mov eax, dword ptr [gFreeze_timer]
            add esp, 8
            test eax, eax
            je L004d8ed6
            push offset gMass_mine+0x30
            push 0
            call strtok
            add esp, 8
            mov ebp, eax
    L004d8ed6:
            push ebp
            push -4
            push 0xbb8
            xor edx, edx
            mov ecx, 4
            call NewTextHeadupSlot
            xor ebp, ebp
    L004d8eec:
            xor ecx, ecx
            mov eax, offset gInventory
    L004d8ef3:
            cmp dword ptr [eax], ebx
            je L004d8f04
            add eax, 4
            inc ecx
            cmp eax, offset gFizzle_in
            jl L004d8ef3
            jmp L004d8f08
    L004d8f04:
            mov dword ptr [esp + 0x14], ecx
    L004d8f08:
            mov edi, dword ptr [esp + 0x14]
            cmp edi, ebp
            jge L004d8f48
            xor ecx, ecx
            mov eax, offset gInventory
    L004d8f17:
            cmp dword ptr [eax], ebp
            jl L004d8f28
            add eax, 4
            inc ecx
            cmp eax, offset gFizzle_in
            jl L004d8f17
            jmp L004d8f35
    L004d8f28:
            mov eax, dword ptr [esp + 0x10]
            mov dword ptr [esp + 0x14], ecx
            mov edi, ecx
            mov dword ptr [eax + 0x30], ebp
    L004d8f35:
            cmp edi, ebp
            jge L004d8f48
            mov eax, ebx
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x108
            ret 8
    L004d8f48:
            mov edx, dword ptr [gInventory_selected]
            cmp edi, edx
            je L004d9083
            cmp edx, ebp
            jl L004d9083
            mov ecx, dword ptr [edx*4 + gInventory]
            mov eax, ecx
            shl eax, 3
            sub eax, ecx
            lea eax, [eax + eax*2]
            lea ecx, [ecx + eax*2]
            mov eax, dword ptr [gPowerup_array]
            lea esi, [eax + ecx*4]
            mov eax, dword ptr [eax + ecx*4 + 0x34]
            cmp eax, ebp
            jl L004d8f8e
            cmp dword ptr [gINT_006a0a6c], ebp
            je L004d9079
    L004d8f8e:
            cmp dword ptr [esi + 4], ebp
            je L004d8fa1
            mov eax, dword ptr [esi + 0x40]
            cmp eax, ebp
            je L004d8fa1
            mov edx, dword ptr [esi + 0x4c]
            mov ecx, esi
            call eax
    L004d8fa1:
            test byte ptr [esi + 0x38], 0x80
            je L004d8fb1
            mov edx, dword ptr [esi + 0x4c]
            mov ecx, esi
            call ARPowerupLose
    L004d8fb1:
            cmp dword ptr [gNet_mode], ebp
            je L004d8fff
            xor edx, edx
            mov cl, 0x16
            call NetBuildGuaranteedMessage
            mov ecx, eax
            push ebp
            mov dword ptr [ecx + 0x28], 2
            mov edx, dword ptr [gLocal_net_ID]
            mov dword ptr [ecx + 0x1c], edx
            mov eax, dword ptr [gPowerup_array]
            mov edx, esi
            sub edx, eax
            mov eax, 0x2fa0be83
            imul edx
            sar edx, 5
            mov eax, edx
            shr eax, 0x1f
            add edx, eax
            mov dword ptr [ecx + 0x20], edx
            mov edx, ecx
            mov ecx, dword ptr [gCurrent_net_game]
            call NetGuaranteedSendMessageToAllPlayers
    L004d8fff:
            mov edx, dword ptr [gPowerup_array]
            mov ecx, esi
            sub ecx, edx
            mov eax, 0x2fa0be83
            imul ecx
            sar edx, 5
            mov ecx, edx
            shr ecx, 0x1f
            add edx, ecx
            mov dword ptr [edx*4 + gProgram_state+0x17bc], ebp
            mov eax, dword ptr [esi + 0x34]
            cmp eax, ebp
            jne L004d9070
            mov edx, dword ptr [gNumber_of_icons]
            xor eax, eax
            cmp edx, ebp
            jle L004d9070
            mov ecx, offset gPickedup_powerups
    L004d9039:
            cmp esi, dword ptr [ecx]
            je L004d9047
            inc eax
            add ecx, 0x14
            cmp eax, edx
            jl L004d9039
            jmp L004d9070
    L004d9047:
            lea edi, [eax + eax*4]
            shl edi, 2
            mov dword ptr [edi + gPickedup_powerups+0x4], 3
            mov dword ptr [edi + gPickedup_powerups+0x8], 0xffffffff
            call GetTotalTime
            mov dword ptr [edi + gPickedup_powerups+0xc], eax
            mov edi, dword ptr [esp + 0x14]
    L004d9070:
            mov dword ptr [esi + 4], ebp
            mov edx, dword ptr [gInventory_selected]
    L004d9079:
            mov dword ptr [gINT_006a0a5c], 1
    L004d9083:
            cmp dword ptr [esp + 0x120], ebp
            jne L004d9090
            cmp edx, ebp
            jge L004d9096
    L004d9090:
            mov dword ptr [gInventory_selected], edi
    L004d9096:
            mov eax, dword ptr [esp + 0x10]
            mov edx, dword ptr [eax + 0x34]
            mov ecx, dword ptr [eax + 0x30]
            add ecx, edx
            mov dword ptr [eax + 0x30], ecx
            mov dword ptr [edi*4 + gInventory], ebx
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x108
            ret 8
    L004d90b9:
            cmp dword ptr [gNet_mode], ebp
            je L004d90d2
            cmp edi, ebp
            je L004d90d2
            cmp dword ptr [edi + 0xc], 8
            jne L004d90d2
            mov eax, 1
            jmp L004d90d4
    L004d90d2:
            xor eax, eax
    L004d90d4:
            mov ecx, dword ptr [esp + 0x11c]
            push ebp
            push ecx
            push eax
            mov edx, ebx
            mov ecx, edi
            call GotPowerupX
    L004d90e7:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x108
            ret 8
    L004d90f4:
            mov eax, dword ptr [gNet_mode]
    L004d90f9:
            cmp eax, ebp
            je L004d912c
            cmp edi, ebp
            je L004d912c
            cmp dword ptr [edi + 0xc], 8
            jne L004d912c
            mov edx, dword ptr [esp + 0x11c]
            mov eax, 1
            push ebp
            push edx
            push eax
            xor edx, edx
            mov ecx, edi
            call GotPowerupX
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x108
            ret 8
    L004d912c:
            mov edx, dword ptr [esp + 0x11c]
            xor eax, eax
            push ebp
            push edx
            push eax
            xor edx, edx
            mov ecx, edi
            call GotPowerupX
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x108
            ret 8
    }
#endif
}

// AutoGoody

// GotPowerup

// DRMapFindMeAPixie


// LoadPowerups (implemented in powerups.c)

// InitRepulseEffects

// MungeBoxObject

// MungeShereObject

// InitTail

// ThisObjectHitAMineSoSpamItUpUsingThisPowerupsSettings

// InitMineShit

// InitPowerups (implemented in powerups.c)

// RemoveRepulseEffect

// ResetRepulseEffects

// EmptyInventory

// ResetPowerups

// CloseDownPowerUps (implemented in powerups.c)

// NewRepulseRay

// RemoveRepulsiveReferences

// MungeRepulseRays

// AdjustRepulseRay

// ResetRepulseRay

// DrawSinglePowerupIcon

// DrawPowerupIcons2

// DrawKeyPowerups

// ToggleInventory

// RightInventory

// LeftInventory

// DrawPowerups

// MungeKeyboardPowerup

// DoPowerupPeriodics

// CheckRespawnQueue

// MungePowerupStuff

// GotPowerupN

// GotPowerup0

// GotPowerup1

// GotPowerup2

// GotPowerup3

// GotPowerup4

// GotPowerup5

// GotPowerup6

// GotPowerup7

// GotPowerup8

// GotPowerup9

// GotCredits

// GainAPO2

// GainAPOPot2

// GainAPO

// GotTimeOrPower

// GainAPOPotential

// SetPedSpeed

// SetPedSize

// SetPedHeadSize

// SetPedExplode

// SetPedBrittle

// SetInvulnerability

// ResetInvulnerability

// SetFreeRepairs

// ResetFreeRepairs

// SetStupidPedestrians

// ResetStupidPedestrians

// FreezeTimer

// UnfreezeTimer

// DoInstantRepair

// ResetPedSpeed

// ResetPedSize

// ResetPedHeadSize

// ResetPedExplode

// ResetPedBrittle

// SetEngineFactor

// SetUnderwater

// TrashBodywork

// TakeDrugs

// PaletteFuckedUpByDrugs

// TheEffectsOfDrugs

// SetOpponentsSpeed

// SetCopsSpeed

// SetGravity

// SetPinball

// SetWallclimb

// SetBouncey

// SetSuspension

// SetTyreGrip

// SetImmortalPeds

// ResetImmortalPeds

// SetSuicidalPeds

// ResetSuicidalPeds

// SetDamageMultiplier

// FillInMine

// SendMines

extern tCar_spec* gActive_car_list[139];
extern int gNum_active_cars;

// FUNCTION: CARMA2_HW 0x004dd1b0
void C2_HOOK_FASTCALL MinePreCollisionStuff(void) {
    int i;

    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gShit_mines); i++) {
        tPhysics_object* obj;

        if (!(gShit_mines[i].flags & 0x1)) {
            continue;
        }
        obj = gShit_mines[i].collision_info;
        if (obj->field_0x261 != 0x31 || obj->message_time != gPHIL_last_physics_tick) {
            continue;
        }
        obj->field_0xf0 = 1;
        SelectCrushNetwork(&obj->field_0x294, obj, 2);
        obj->field_0x261 = 0;
    }
}

// ActivateMine

// PositionMine

// MineSound

// ReceivedMine

// ShitMine

// MineExplode

// TiggerMineToExplode

// HitAShatOutMine

// ClearMineReferencesToCar

// MungeShatMines

// FUNCTION: CARMA2_HW 0x004de000
void C2_HOOK_FASTCALL ARDoShitMine(br_actor* pActor, const br_vector3* pUnused) {
    if (pActor->parent == NULL) {
        BrActorAdd(gNon_track_actor, pActor);
    }
    pActor->render_style = BR_RSTYLE_FACES;
}

// FUNCTION: CARMA2_HW 0x004de020
void C2_HOOK_FASTCALL ARUndoShitMine(br_actor* pActor, const br_vector3* pUnused) {
    if (pActor->parent != NULL) {
        BrActorRemove(pActor);
    }
    pActor->render_style = BR_RSTYLE_NONE;
}

// PissOutOil

// KangerooJump

// AnnihilatePeds

// NapalmPeds

// RepulseOpponents2

// RepulseOpponents

// RepulsePedestrians

// ReceivedRepulse

// ResetEngineFactor

// ResetUnderwater

// PukeDrugsBackUp

// ResetOpponentsSpeed

// ResetCopsSpeed

// ResetGravity

// ResetPinball

// ResetWallclimb

// ResetBouncey

// ResetSuspension

// ResetDamageMultiplier

// ResetTyreGrip

// PickAtRandom

// GotVouchers

// MungeVouchers

// SetInstantHandbrake

// ResetInstantHandbrake

// SetDancingPeds

// SetPanickingPeds

// SetLowGravityPeds

// SetPissedPeds

// SetGhostPeds

// ResetDancingPeds

// ResetPanickingPeds

// ResetLowGravityPeds

// ResetPissedPeds

// ResetGhostPeds

// DoBouncey

// HitMine

// SetMassMultiplier

// ResetMassMultiplier

// ShowPedestrians

// HidePedestrians

// SetElectroBastard

// ResetElectroBastard

// SetMutantCorpses

// ResetMutantCorpses

// ReceivedPowerup

// SendCurrentPowerups

// LoseAllLocalPowerups

// LoseDodgyPowerups

// GetPowerupCount

// GetPowerupPosition

// ARPowerupGot

// ARPowerupLose

// SetPissed

// ResetPissed

// PissedEffect

// RenderSpinningPowerup

// RenderChangingPowerup

// SetSpinningPowerup

// DuplicateModel

// SetChangingPowerup

// PedValium

// PowerupCancel

// GrowATail

// CutOffTail

// RemoveTail (implemented in powerups.c)

// FUNCTION: CARMA2_HW 0x004e0710
void C2_HOOK_FASTCALL DoInitialTail(void) {
    tPhysics_object* collision_info;

    for (collision_info = gMutant_tail_first_collision_info; collision_info != NULL; collision_info = collision_info->child) {
        collision_info->actor->render_style = BR_RSTYLE_FACES;
    }
}

// FUNCTION: CARMA2_HW 0x004e0730
void C2_HOOK_FASTCALL UndoInitialTail(void) {
    tPhysics_object* collision_info;

    for (collision_info = gMutant_tail_first_collision_info; collision_info != NULL; collision_info = collision_info->child) {
        collision_info->actor->render_style = BR_RSTYLE_NONE;
    }
}

// MayQueuePowerupRespawn

// FUNCTION: CARMA2_HW 0x004e07d0
void C2_HOOK_FASTCALL ARDoPowerupRespawn(br_actor* pActor, int pUnused) {
    pActor->render_style = BR_RSTYLE_FACES;
}

// FUNCTION: CARMA2_HW 0x004e07e0
void C2_HOOK_FASTCALL ARUndoPowerupRespawn(br_actor* pActor, int pUnused) {
    pActor->render_style = BR_RSTYLE_NONE;
}

// FUNCTION: CARMA2_HW 0x004e07f0
void C2_NAKED C2_HOOK_FASTCALL PrintPowerupIconIn3D(int pX, int pY, tHeadup_icon* pIcon, tPowerup* pPowerup, int pScale, tU32 pTime) {
#ifndef CARPOCALYPSE2_MATCHING
    float f;
    float scale;
    br_fixed_ls prev_opacity;

    if (pIcon == NULL) {
        pPowerup->icon_actor->render_style = BR_RSTYLE_FACES;
        BrVector3Set(&pIcon->icon_actor->t.t.translate.t, (float)pX + 16.0f, (float)-(pY + 16), 0.0f);
        RenderThisHeadup(pIcon->icon_actor);
        return;
    }
    if (pIcon->icon_actor != NULL) {
        pIcon->icon_actor->render_style = BR_RSTYLE_FACES;
        if (pScale != 0) {
            f = (float)(pTime - pIcon->fizzle_start) / 600.0f;
            if (pIcon->fizzle_direction > 0) {
                f = 1.0f - f;
                scale = 1.0f + 5.0f * f;
                BrMatrix34Scale(&pIcon->icon_actor->t.t.mat, scale, scale, 1.0f);
            } else {
                BrMatrix34Scale(&pIcon->icon_actor->t.t.mat, 1.01f - f, 1.01f - f, 1.0f);
            }
            BrMatrix34PostRotateZ(&pIcon->icon_actor->t.t.mat, (br_angle)((1.0f - f / 2.0f) * 65536.0f));
            pIcon->icon_actor->material->extra_prim[1].v.x = BR_FIXED_INT((1.0f - f) * 255.0f);
            BrMaterialUpdate(pIcon->icon_actor->material, BR_MATU_EXTRA_PRIM);
        } else {
            prev_opacity = pIcon->icon_actor->material->extra_prim[1].v.x;
            pIcon->icon_actor->material->extra_prim[1].v.x = BR_FIXED_INT(255);
            if (prev_opacity != pIcon->icon_actor->material->extra_prim[1].v.x) {
                BrMaterialUpdate(pIcon->icon_actor->material, BR_MATU_EXTRA_PRIM);
            }
            BrMatrix34Identity(&pIcon->icon_actor->t.t.mat);
        }
        BrVector3Set(&pIcon->icon_actor->t.t.translate.t, (float)pX + 16.0f, (float)-(pY + 16), 0.0f);
        RenderThisHeadup(pIcon->icon_actor);
    }
#else
    __asm {
            sub esp, 0xc
            push ebx
            push esi
            mov esi, dword ptr [esp + 0x18]
            push edi
            test esi, esi
            mov ebx, edx
            mov dword ptr [esp + 0xc], ecx
            jne L004e085a
            mov eax, dword ptr [esp + 0x20]
            fild dword ptr [esp + 0xc]
            mov ecx, dword ptr [eax + 0x90]
            fsub dword ptr [gConst_kangeroo_force_factor+0x40]
            mov byte ptr [ecx + 0x20], 4
            mov edx, dword ptr [eax + 0x90]
            mov ecx, 0xfffffff0
            sub ecx, ebx
            fstp dword ptr [edx + 0x50]
            mov edx, dword ptr [eax + 0x90]
            mov dword ptr [esp + 0x1c], ecx
            fild dword ptr [esp + 0x1c]
            fstp dword ptr [edx + 0x54]
            mov ecx, dword ptr [eax + 0x90]
            mov dword ptr [ecx + 0x58], esi
            mov ecx, dword ptr [eax + 0x90]
            call RenderThisHeadup
            pop edi
            pop esi
            pop ebx
            add esp, 0xc
            ret 0x10
    L004e085a:
            mov eax, dword ptr [esi + 0x10]
            test eax, eax
            je L004e09c1
            mov byte ptr [eax + 0x20], 4
            mov eax, dword ptr [esp + 0x24]
            test eax, eax
            je L004e0955
            mov edx, dword ptr [esp + 0x28]
            mov ecx, dword ptr [esi + 0xc]
            sub edx, ecx
            mov dword ptr [esp + 0x14], 0
            mov dword ptr [esp + 0x10], edx
            mov eax, dword ptr [esi + 8]
            fild qword ptr [esp + 0x10]
            test eax, eax
            fmul dword ptr [gConst_kangeroo_force_factor+0x44]
            fstp dword ptr [esp + 0x1c]
            jle L004e08d1
            fld dword ptr [gFLOAT_0058ac20]
            fsub dword ptr [esp + 0x1c]
            push 0x3f800000
            fst dword ptr [esp + 0x20]
            fmul dword ptr [gConst_kangeroo_force_factor+0x48]
            fsubr dword ptr [gFLOAT_0058ac20]
            fstp dword ptr [esp + 0x24]
            mov eax, dword ptr [esp + 0x24]
            push eax
            push eax
            mov eax, dword ptr [esi + 0x10]
            add eax, 0x2c
            push eax
            jmp L004e08f1
    L004e08d1:
            fld dword ptr [gConst_kangeroo_force_factor+0x4c]
            fsub dword ptr [esp + 0x1c]
            mov ecx, dword ptr [esi + 0x10]
            push 0x3f800000
            add ecx, 0x2c
            fstp dword ptr [esp + 0x24]
            mov eax, dword ptr [esp + 0x24]
            push eax
            push eax
            push ecx
    L004e08f1:
            call BrMatrix34Scale
            fld dword ptr [esp + 0x2c]
            fmul dword ptr [gFLOAT_0058ac00]
            add esp, 0x10
            fsubr dword ptr [gFLOAT_0058ac20]
            fmul dword ptr [gConst_kangeroo_force_factor+0x50]
            call _ftol
            mov edx, dword ptr [esi + 0x10]
            push eax
            add edx, 0x2c
            push edx
            call BrMatrix34PostRotateZ
            fld dword ptr [gFLOAT_0058ac20]
            fsub dword ptr [esp + 0x24]
            mov eax, dword ptr [esi + 0x10]
            add esp, 8
            mov edi, dword ptr [eax + 0x1c]
            fmul dword ptr [gConst_kangeroo_force_factor+0x54]
            call _ftol
            mov ecx, dword ptr [edi + 0x58]
            push 0x40
            shl eax, 0x10
            push edi
            mov dword ptr [ecx + 0xc], eax
            call BrMaterialUpdate
            add esp, 8
            jmp L004e098a
    L004e0955:
            mov edx, dword ptr [esi + 0x10]
            mov eax, dword ptr [edx + 0x1c]
            mov ecx, dword ptr [eax + 0x58]
            mov edx, dword ptr [ecx + 0xc]
            mov dword ptr [ecx + 0xc], 0xff0000
            mov ecx, dword ptr [eax + 0x58]
            cmp edx, dword ptr [ecx + 0xc]
            je L004e097b
            push 0x40
            push eax
            call BrMaterialUpdate
            add esp, 8
    L004e097b:
            mov edx, dword ptr [esi + 0x10]
            add edx, 0x2c
            push edx
            call BrMatrix34Identity
            add esp, 4
    L004e098a:
            fild dword ptr [esp + 0xc]
            mov eax, dword ptr [esi + 0x10]
            mov ecx, 0xfffffff0
            sub ecx, ebx
            fsub dword ptr [gConst_kangeroo_force_factor+0x40]
            mov dword ptr [esp + 0x1c], ecx
            fstp dword ptr [eax + 0x50]
            fild dword ptr [esp + 0x1c]
            mov edx, dword ptr [esi + 0x10]
            fstp dword ptr [edx + 0x54]
            mov eax, dword ptr [esi + 0x10]
            mov dword ptr [eax + 0x58], 0
            mov ecx, dword ptr [esi + 0x10]
            call RenderThisHeadup
    L004e09c1:
            pop edi
            pop esi
            pop ebx
            add esp, 0xc
            ret 0x10
            nop
            nop
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004e09d0
C2_NAKED br_actor* C2_HOOK_FASTCALL CreateBillBoard(br_pixelmap* pTexture) {
#ifndef CARPOCALYPSE2_MATCHING
    br_actor* actor;
    br_material* material;
    br_model* model;
    int width;
    int height;
    br_scalar left;
    br_scalar right;
    br_scalar top;
    br_scalar bottom;

    width = pTexture->width;
    height = pTexture->height;
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
    model->faces[1].vertices[0] = 2;
    model->faces[1].vertices[1] = 3;
    model->faces[1].vertices[2] = 0;
    model->faces[0].material = NULL;
    model->faces[1].material = NULL;
    left = (float)-(width / 2);
    top = (float)+(height / 2);
    BrVector3Set(&model->vertices[0].p, left,  top, -2.0f);
    bottom = (float)-(height / 2);
    BrVector3Set(&model->vertices[1].p, left, bottom, -2.0f);
    right = (float)+(width / 2);
    BrVector3Set(&model->vertices[2].p, right, bottom, -2.0f);
    BrVector3Set(&model->vertices[3].p, right,  top, -2.0f);
    BrVector2Set(&model->vertices[0].map, 0.0f, 0.0f);
    BrVector2Set(&model->vertices[1].map, 0.0f, 1.0f);
    BrVector2Set(&model->vertices[2].map, 1.0f, 1.0f);
    BrVector2Set(&model->vertices[3].map, 1.0f, 0.0f);
    material->colour = 0;
    material->colour_map = pTexture;
    material->flags = BR_MATF_ALWAYS_VISIBLE | BR_MATF_FORCE_FRONT;
    model->flags |= BR_MODF_KEEP_ORIGINAL;
    BrMapAdd(pTexture);
    BrMaterialAdd(material);
    BrModelAdd(model);
    return actor;
#else
    __asm {
            sub esp, 0x10
            push ebx
            push ebp
            mov ebp, ecx
            xor eax, eax
            push esi
            push edi
            mov ax, word ptr [ebp + 0x34]
            push 2
            mov dword ptr [esp + 0x18], eax
            xor eax, eax
            mov ax, word ptr [ebp + 0x36]
            push 4
            push offset gFrontend_QUIT+0xbc94
            mov dword ptr [esp + 0x1c], eax
            call BrModelAllocate
            add esp, 0xc
            mov esi, eax
            push offset gFrontend_QUIT+0xbc80
            call BrMaterialAllocate
            add esp, 4
            mov edi, eax
            push 0
            push 1
            call BrActorAllocate
            mov ebx, eax
            xor ecx, ecx
            add esp, 8
            mov dword ptr [ebx + 0x14], 0x655a60
            mov dword ptr [ebx + 0x18], esi
            mov dword ptr [ebx + 0x1c], edi
            mov byte ptr [ebx + 0x20], 4
            mov eax, dword ptr [esi + 0xc]
            mov word ptr [eax], cx
            mov edx, dword ptr [esi + 0xc]
            mov eax, 2
            mov word ptr [edx + 2], 1
            mov edx, dword ptr [esi + 0xc]
            mov word ptr [edx + 4], ax
            mov edx, dword ptr [esi + 0xc]
            mov word ptr [edx + 0x28], ax
            mov eax, dword ptr [esi + 0xc]
            mov word ptr [eax + 0x2a], 3
            mov edx, dword ptr [esi + 0xc]
            mov word ptr [edx + 0x2c], cx
            mov eax, dword ptr [esi + 0xc]
            mov dword ptr [eax + 8], ecx
            mov edx, dword ptr [esi + 0xc]
            mov eax, dword ptr [esp + 0x14]
            mov dword ptr [edx + 0x30], ecx
            cdq
            sub eax, edx
            sar eax, 1
            neg eax
            mov dword ptr [esp + 0x18], eax
            mov eax, dword ptr [esi + 8]
            fild dword ptr [esp + 0x18]
            _emit 0xd9
        _emit 0x10
            mov eax, dword ptr [esp + 0x10]
            cdq
            sub eax, edx
            mov edx, dword ptr [esi + 8]
            sar eax, 1
            mov dword ptr [esp + 0x18], eax
            fild dword ptr [esp + 0x18]
            fst dword ptr [esp + 0x1c]
            fstp dword ptr [edx + 4]
            mov eax, dword ptr [esi + 8]
            mov dword ptr [eax + 8], 0xc0000000
            mov edx, dword ptr [esi + 8]
            mov eax, dword ptr [esp + 0x10]
            fstp dword ptr [edx + 0x28]
            cdq
            sub eax, edx
            sar eax, 1
            neg eax
            mov dword ptr [esp + 0x18], eax
            mov eax, dword ptr [esi + 8]
            fild dword ptr [esp + 0x18]
            fst dword ptr [eax + 0x2c]
            mov edx, dword ptr [esi + 8]
            mov dword ptr [edx + 0x30], 0xc0000000
            mov eax, dword ptr [esp + 0x14]
            push ebp
            cdq
            sub eax, edx
            sar eax, 1
            mov dword ptr [esp + 0x1c], eax
            mov eax, dword ptr [esi + 8]
            fild dword ptr [esp + 0x1c]
            fst dword ptr [esp + 0x1c]
            fstp dword ptr [eax + 0x50]
            mov edx, dword ptr [esi + 8]
            fstp dword ptr [edx + 0x54]
            mov eax, dword ptr [esi + 8]
            mov dword ptr [eax + 0x58], 0xc0000000
            mov edx, dword ptr [esi + 8]
            mov eax, dword ptr [esp + 0x1c]
            mov dword ptr [edx + 0x78], eax
            mov edx, dword ptr [esi + 8]
            mov eax, dword ptr [esp + 0x20]
            mov dword ptr [edx + 0x7c], eax
            mov edx, dword ptr [esi + 8]
            mov dword ptr [edx + 0x80], 0xc0000000
            mov eax, dword ptr [esi + 8]
            mov dword ptr [eax + 0xc], ecx
            mov edx, dword ptr [esi + 8]
            mov dword ptr [edx + 0x10], ecx
            mov eax, dword ptr [esi + 8]
            mov dword ptr [eax + 0x34], ecx
            mov edx, dword ptr [esi + 8]
            mov eax, 0x3f800000
            mov dword ptr [edx + 0x38], eax
            mov edx, dword ptr [esi + 8]
            mov dword ptr [edx + 0x5c], eax
            mov edx, dword ptr [esi + 8]
            mov dword ptr [edx + 0x60], eax
            mov edx, dword ptr [esi + 8]
            mov dword ptr [edx + 0x84], eax
            mov eax, dword ptr [esi + 8]
            mov dword ptr [eax + 0x88], ecx
            mov dword ptr [edi + 8], ecx
            mov dword ptr [edi + 0x40], ebp
            mov dword ptr [edi + 0x20], 0x2800
            or byte ptr [esi + 0x20], 2
            call BrMapAdd
            add esp, 4
            push edi
            call BrMaterialAdd
            add esp, 4
            push esi
            call BrModelAdd
            add esp, 4
            mov eax, ebx
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x10
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

// FUNCTION: CARMA2_HW 0x004e0c00
void C2_HOOK_FASTCALL SetDefaultPowerupFilename(void) {
    gPowerup_txt_path = "POWERUP.TXT";
}

// FUNCTION: CARMA2_HW 0x004e0c40
int C2_HOOK_FASTCALL TurnOnCloaking(tPowerup* pPowerup, tCar_spec* pCar) {

    if (gNet_mode != eNet_mode_none && !IsCarCloaked(pCar)) {
        BlendifyCar(pCar);
        MasterDisableCarFunks(pCar);
        gCloaked_cars[gCount_cloaked_cars++] = pCar;
    }
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004e0cb0
void C2_HOOK_FASTCALL RemoveFromCloakingList(tCar_spec* pCar) {
    int i;

    if (gNet_mode != eNet_mode_none) {
        for (i = 0; i < gCount_cloaked_cars; i++) {

            if (gCloaked_cars[i] == pCar) {
#ifdef CARPOCALYPSE2_FIX_BUGS
                memmove(&gCloaked_cars[i], &gCloaked_cars[i + 1], (gCount_cloaked_cars - i - 1) * sizeof(tCar_spec*));
#else
                memcpy(&gCloaked_cars[i], &gCloaked_cars[i + 1], (CARPOCALYPSE2_ASIZE(gCloaked_cars) - i - 1) * sizeof(tCar_spec*));
#endif
                gCount_cloaked_cars -= 1;
                break;
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x004e0d10
int C2_HOOK_FASTCALL IsCarCloaked(tCar_spec* pCar) {
    int i;

    if (gNet_mode != eNet_mode_none) {
        for (i = 0; i < gCount_cloaked_cars; i++) {
            if (gCloaked_cars[i] == pCar) {
                return 1;
            }
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004e0d50
void C2_HOOK_FASTCALL TurnOffCloaking(tPowerup* pPowerup, tCar_spec* pCar) {

    if (gNet_mode != eNet_mode_none) {
        UnBlendifyCar(pCar);
        RemoveFromCloakingList(pCar);
        MasterEnableCarFunks(pCar);
        RestoreCarPixelmaps(pCar);
    }
}

// FUNCTION: CARMA2_HW 0x004df200
void C2_HOOK_FASTCALL FUN_004df200(tPowerup* pPowerup, tCar_spec* pCar) {

    if (pPowerup->got_proc != NULL) {
        ((void (C2_HOOK_FASTCALL *)(tPowerup*, tCar_spec*))pPowerup->got_proc)(pPowerup, pCar);
    }
}

// FUNCTION: CARMA2_HW 0x004df210
void C2_HOOK_FASTCALL FUN_004df210(tPowerup* pPowerup, tCar_spec* pCar) {
    if (pPowerup->lose_proc != NULL) {
        pPowerup->lose_proc(pPowerup, pCar);
    }
}

// FUNCTION: CARMA2_HW 0x004d9670
void C2_HOOK_FASTCALL QueueIconGoing(tPowerup* pPowerup, tHeadup_icon* pHeadup_icons, int* pCount_icons) {
    int i;

    for (i = 0; i < *pCount_icons; i++) {
        if (pPowerup == pHeadup_icons[i].powerup) {
            pHeadup_icons[i].fizzle_stage = 3;
            pHeadup_icons[i].fizzle_direction = -1;
            pHeadup_icons[i].fizzle_start = GetTotalTime();
            break;
        }
    }
}

// FUNCTION: CARMA2_HW 0x004e0ba0
void C2_HOOK_FASTCALL DisposeBillBoard(br_actor* pActor) {

    if (pActor != NULL) {
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

// FUNCTION: CARMA2_HW 0x004e0e20
void C2_HOOK_FASTCALL FUN_004e0e20(void) {

    // empty
}
