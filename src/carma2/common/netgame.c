#include "netgame.h"

extern tU8 gNon_car_spec_indices[100];

extern br_quat* C2_HOOK_CDECL BrQuatNormalise(br_quat* q, const br_quat* qq);
extern br_matrix34* C2_HOOK_CDECL BrQuatToMatrix34(br_matrix34* mat, const br_quat* q);

#include "brucetrk.h"

#include "flap.h"

#include "displays.h"
#include "globvars.h"

#include "globvrkm.h"

#include "globvrpb.h"
#include "network.h"

#include "newgame.h"

#include "phil.h"
#include "structur.h"
#include "world.h"
#include "opponent.h"
#include "physics.h"
#include "platform.h"
#include "utility.h"
#include "car.h"
#include "racestrt.h"
#include "controls.h"
#include "crush.h"
#include "crush2.h"
#include "powerups.h"
#include "spark.h"

#include "graphics.h"

#include "racemem.h"
#include "smashing.h"
#include "errors.h"
#include "carpocalypse2_macros.h"
#include "c2_stdlib.h"

#include <brender/brender.h>
#include "netgame.h"

#include "network.h"
#include "loading3.h"
#include "globvars.h"
// GLOBAL: CARMA2_HW 0x0074a738
int gInitialised_grid;

// GLOBAL: CARMA2_HW 0x0074a734
int gIt_or_fox;

// GLOBAL: CARMA2_HW 0x0068d924
tNet_game_player_info* gLast_lepper;

// GLOBAL: CARMA2_HW 0x0068d928
tU32 gLast_it_change;

// GLOBAL: CARMA2_HW 0x0068d934
int gNot_shown_race_type_headup;

// GLOBAL: CARMA2_HW 0x0068d938
int gWinner_declared;

// GLOBAL: CARMA2_HW 0x0068d92c
tU32 gTime_for_punishment;

// GLOBAL: CARMA2_HW 0x0068d920
int gINT_0068d920;

// GLOBAL: CARMA2_HW 0x0075b8f8
br_pixelmap* gDigits_pix;

// GLOBAL: CARMA2_HW 0x0058a3f8
const br_scalar gFLOAT_0058a3f8 = 1.0f;

// GLOBAL: CARMA2_HW 0x0058a3fc
const br_scalar gFLOAT_0058a3fc = -1.0f;

// GLOBAL: CARMA2_HW 0x0058a400
const br_scalar gFLOAT_0058a400 = 0.5f;

// GLOBAL: CARMA2_HW 0x0058a3d0
const br_scalar gFLOAT_0058a3d0 = 32767.0f;

// GLOBAL: CARMA2_HW 0x0058a3d4
const br_scalar gFLOAT_0058a3d4 = 255.0f;

// GLOBAL: CARMA2_HW 0x0058a3d8
const br_scalar gFLOAT_0058a3d8 = 40.0f;

// GLOBAL: CARMA2_HW 0x0058a3f0
const double gDOUBLE_0058a3f0 = 4.0;

// GLOBAL: CARMA2_HW 0x0058a3e8
const br_scalar gNon_car_pull_actor_distance_squared = 900.0f;

// GLOBAL: CARMA2_HW 0x0058a404
const br_scalar gFLOAT_0058a404 = 65534.0f;

// GLOBAL: CARMA2_HW 0x0058a408
const br_scalar gFLOAT_0058a408 = -0.5f;

// GLOBAL: CARMA2_HW 0x0058a40c
const br_scalar gFLOAT_0058a40c = 0.000015259254723787308f;

// GLOBAL: CARMA2_HW 0x0068d954
int gUNK_0068d954;

#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern br_actor* C2_HOOK_CDECL BrActorAdd();
extern br_actor* C2_HOOK_CDECL BrActorRemove();
extern void C2_HOOK_CDECL BrMatrix34Copy();
extern void C2_HOOK_CDECL BrMatrix34RotateY();
extern float C2_HOOK_FASTCALL ComputeCarImpact(tCar_spec* pCar, int pDelta, float* pOut);
extern void C2_HOOK_FASTCALL CrushCarByNetwork(tCar_spec* pCar);
extern void C2_HOOK_FASTCALL DisposeKevStuffCar(tCar_spec* pCar);
extern tNon_car_spec* C2_HOOK_FASTCALL DoPullActorFromWorld(br_actor* actor);
extern void C2_HOOK_FASTCALL ExpandVector3(br_vector3* pDest, const tCompressed_vector3 *pSrc, float pMin, float pMax);
extern void C2_HOOK_FASTCALL FUN_0042da20(undefined4* pRecipient);
extern void C2_HOOK_FASTCALL FUN_0043d7d0(tNet_game_player_info* pRecipient);
extern void C2_HOOK_FASTCALL FUN_00498ab0(void* pArg1, tU8* pSrc);
extern void C2_HOOK_FASTCALL FUN_004f0f80(tNet_game_player_info* pRecipient);
extern int C2_HOOK_FASTCALL GetCompressedCrushVertexDataSize(tU8* pData);
extern int C2_HOOK_FASTCALL GetNetworkDataSize(tU8* pNet_data);
extern int C2_HOOK_FASTCALL GetSingleMatrixFromNetworkData(tU8* pNet_data, br_matrix34* pMatrix);
extern tNet_message_chunk* C2_HOOK_FASTCALL NetGetToPlayerContents(tNet_message_type pType, tU32 pSize_decider, tPlayer_ID pPlayer_id);
extern void C2_HOOK_FASTCALL NetSendMessageStacks(void);
extern void C2_HOOK_FASTCALL PlayerIsInfected(tNet_game_player_info* pPlayer);
extern void C2_HOOK_FASTCALL ReallyViewOpponent(void);
extern int C2_HOOK_FASTCALL SelectCrushNetwork(undefined4* pField, tPhysics_object* pObject, int pMode);
extern void C2_HOOK_FASTCALL SendDetachedNonCarDataToPlayer(tNet_game_player_info* pRecipient);
extern void C2_HOOK_FASTCALL SetAllShapeStatusFlags(tCar_crush_network_shapes* pShapes);
extern void C2_HOOK_FASTCALL SetSmokeLastDamageLevel(tCar_spec* pCar);
extern void C2_HOOK_FASTCALL SortOutSmoke(tCar_spec* pCar);
extern void C2_HOOK_FASTCALL StopCarBeingIt(tCar_spec* pCar);
extern void C2_HOOK_FASTCALL StopCarSmoking(tCar_spec* pCar);
extern void C2_HOOK_FASTCALL TotallyRepairACar(tCar_spec* pCar_spec);
extern void C2_HOOK_FASTCALL UpdateCrushVertices(tCar_spec* pCar, tU8* pField);
extern void C2_HOOK_FASTCALL WeldCar(tCar_spec* pCar_spec);
extern void C2_HOOK_FASTCALL WeldCarByNetwork(tCar_spec* pCar);
extern void C2_HOOK_FASTCALL WeldCarPartial(tCar_spec* pCar);
extern void C2_HOOK_FASTCALL XZToColumnXZ(tU8* pColumn_x, tU8* pColumn_z, br_scalar pX, br_scalar pZ, tTrack_spec* pTrack_spec);
extern br_uint_8 bl;
extern tU32 gCar_to_view_id;
extern tNet_game_details* gCurrent_net_game;
extern int gFace_num__car;
extern tNet_mode gNet_mode;
extern tNet_game_player_info gNet_players[12];
extern int gNumber_of_net_players;
extern tProgram_state gProgram_state;
extern int gThis_net_player_index;
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern int C2_HOOK_FASTCALL CarShapeNeedsSendingToPlayer(tCar_crush_network_shapes* pShapes, int pPlayer_id);
extern void C2_HOOK_FASTCALL ClearShapeStatusFlag(tCar_crush_network_shapes* pShapes, int pPlayer_id);
extern void C2_HOOK_FASTCALL CompressCarDataChunk(tU8* pChunk, int pPlayer_index);
extern void C2_HOOK_FASTCALL CompressPlane(tU16 pDest[4], const br_vector4* pSrc);
extern void C2_HOOK_FASTCALL DoNetScores2(int pOnly_sort_scores);
extern void C2_HOOK_FASTCALL EncodeNetShapeData(tU8* pData, tCar_spec* pCar);
extern int C2_HOOK_FASTCALL FUN_0043d900(tCar_spec* pCar);
extern void C2_NORETURN C2_HOOK_CDECL FatalError();
extern int C2_HOOK_FASTCALL GetHierarchyNetworkSize(tPhysics_object* pObject);
extern int C2_HOOK_FASTCALL GetHierarchyNetworkStuff(tPhysics_object* pObject, tU8* pNet_data, int pRemaining);
extern const char* C2_HOOK_FASTCALL GetMiscString(int pIndex);
extern void C2_HOOK_FASTCALL NetBroadcastContents(tNet_message_chunk* pMessage);
extern tNet_message* C2_HOOK_FASTCALL NetBuildGuaranteedMessage(tU8 pNet_message_type, int pOption);
extern tNet_message_chunk* C2_HOOK_FASTCALL NetGetToHostContents(tNet_message_type pType, tU32 pSize_decider);
extern int C2_HOOK_FASTCALL NetGuaranteedSendMessageToAllPlayers(tNet_game_details* pDetails, tNet_message* pMessage, void* pNotifyFail);
extern int C2_HOOK_FASTCALL NetGuaranteedSendMessageToPlayer(tNet_game_details* pNet_game, tNet_message* pMessage, undefined4 pPlayer, undefined4 pArg4);
extern void C2_HOOK_FASTCALL NetSendHeadupToPlayer(const char* pMessage, tPlayer_ID pPlayer_id);
extern tNet_message_chunk* C2_HOOK_FASTCALL NetStartBroadcastContents(tNet_message_type pType, tU32 pSize_decider);
extern void C2_HOOK_FASTCALL SendDetachedNonCarData(tNon_car_spec* pNon_car, undefined4 pArg2, tNet_game_player_info* pRecipient);
extern void C2_HOOK_FASTCALL SendLocalCarData(tU32 pNext_frame_time);
extern void C2_HOOK_FASTCALL SendNonCar(tNon_car_spec* pNon_car, tU32 pPlayer_id);
extern void C2_HOOK_FASTCALL SendSplitHalfNonCarData(tNon_car_spec* pNon_car, undefined4 pArg2, tNet_game_player_info* pRecipient);
extern void __cdecl _ftol();
extern tNon_car_spec* gActive_non_car_list[99];
extern char gCar_map_colours[];
extern int gNum_active_non_cars;
extern tU32 gRace_start;
extern int gReceived_game_scores;
extern int gScore_winner;
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern tU32 C2_HOOK_FASTCALL GetTotalTime(void);
#endif
#ifdef CARPOCALYPSE2_MATCHING
#endif
// FUNCTION: CARMA2_HW 0x00499260

void C2_HOOK_FASTCALL DisableCar(tCar_spec* pCar) {

    if (pCar->driver_name[0] != '\0') {
        if (!pCar->disabled) {
            pCar->disabled = 1;
            ForceRebuildActiveCarList();
        }
        if (pCar->car_master_actor->t.t.mat.m[3][0] < 500.0f) {
            BrVector3Accumulate(&pCar->car_master_actor->t.t.translate.t, &gInitial_position);
            BrVector3Copy((br_vector3*)pCar->old_frame_mat.m[3], &pCar->car_master_actor->t.t.translate.t);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004992e0
void C2_HOOK_FASTCALL EnableCar(tCar_spec* pCar) {

    if (pCar->driver_name[0] != '\0') {
        if (pCar->disabled) {
            pCar->disabled = 0;
            ForceRebuildActiveCarList();
        }
        if (pCar->car_master_actor->t.t.mat.m[3][0] > 500.0f) {
            BrVector3Sub(&pCar->car_master_actor->t.t.translate.t, &pCar->car_master_actor->t.t.translate.t, &gInitial_position);
            BrVector3Copy((br_vector3*)pCar->old_frame_mat.m[3], &pCar->car_master_actor->t.t.translate.t);
        }
    }
}

// FUNCTION: CARMA2_HW 0x0049b690
void C2_HOOK_FASTCALL InitialisePlayerScore(tNet_game_player_info* pPlayer) {

    C2_HOOK_BUG_ON(sizeof(tNet_game_details) != 0x78);
    C2_HOOK_BUG_ON(sizeof(tNet_game_options) != 0x30);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_game_details, field_0x10, 0x10);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_game_details, field_0x14, 0x14);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_game_details, start_race, 0x38);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_game_details, options, 0x44);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_game_details, type, 0x74);
    C2_HOOK_BUG_ON(sizeof(tNet_game_player_info) != 0xd4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_game_player_info, reposition_time, 0x18);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_game_player_info, score2, 0x64);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_game_player_info, credits, 0x68);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_game_player_info, wasted, 0x6c);

    PossibleService();
    switch (gCurrent_net_game->type) {
    case eNet_game_type_fight_to_death:
        pPlayer->score2 = 100;
        break;
    case eNet_game_type_1:
    case eNet_game_type_2:
    case eNet_game_type_4:
    case eNet_game_type_5:
    case eNet_game_type_6:
        pPlayer->score2 = 0;
        break;
    case eNet_game_type_checkpoint:
        pPlayer->score2 = 0xffff;
        break;
    case eNet_game_type_foxy:
        pPlayer->score2 = 0;
        break;
    }
    pPlayer->credits = gCurrent_net_game->options.starting_credits;
    pPlayer->wasted = 0;
    pPlayer->reposition_time = 0;
}

// FUNCTION: CARMA2_HW 0x0049b720
void C2_HOOK_FASTCALL InitPlayers(void) {
    int i;

    for (i = 0; i < gNumber_of_net_players; i++) {
        InitialisePlayerScore(&gNet_players[i]);
    }
    if (gNet_mode == eNet_mode_host) {
        gLast_it_change = 0;
        gLast_lepper = NULL;
    }
    gTime_for_punishment = 0;
    gNot_shown_race_type_headup = 1;
    gIt_or_fox = -1;
    gWinner_declared = 0;
}

// FUNCTION: CARMA2_HW 0x0049b7e0
void C2_NAKED C2_HOOK_FASTCALL FUN_0049b7e0(tU32 pPlayer_id) {
#ifndef CARPOCALYPSE2_MATCHING
    int game_type;
    int i;
    int player_count;
    tNet_game_player_info* player;

    game_type = gCurrent_net_game->type;
    game_type -= 2;
    if (game_type != 0) {
        game_type -= 4;
        if (game_type != 0) {
            if (--game_type != 0) {
                return;
            }
            if (gNet_players[gIt_or_fox].ID != pPlayer_id) {
                i = 0;
                if (0 < gNumber_of_net_players) {
                    player = gNet_players;
                    do {
                        StopCarBeingIt(player->car);
                        i++;
                        player++;
                    } while (i < gNumber_of_net_players);
                }
                i = 0;
                if (0 < gNumber_of_net_players) {
                    player = gNet_players;
                    player_count = gNumber_of_net_players;
                    do {
                        if (player->ID == pPlayer_id) {
                            DisposeKevStuffCar(player->car);
                            player_count = gNumber_of_net_players;
                            gIt_or_fox = i;
                        }
                        i++;
                        player++;
                    } while (i < player_count);
                }
            }
        } else {
            i = 0;
            if (0 < gNumber_of_net_players) {
                player = gNet_players;
                do {
                    if (player->field_0x80 == 0 && (pPlayer_id & (1u << i)) != 0) {
                        PlayerIsInfected(player);
                    }
                    i++;
                    player++;
                } while (i < gNumber_of_net_players);
            }
        }
        return;
    }
    gINT_0068d920 = pPlayer_id;
#else
    __asm {
            mov eax, dword ptr [gCurrent_net_game]
            push ebx
            push esi
            push edi
            mov eax, dword ptr [eax + 0x74]
            mov ebx, ecx
            sub eax, 2
            je L0049b8bb
            sub eax, 4
            je L0049b879
            dec eax
            jne L0049b8c1
            mov eax, dword ptr [gIt_or_fox]
            lea ecx, [eax + eax*2]
            lea ecx, [ecx + ecx*8]
            shl ecx, 1
            sub ecx, eax
            cmp dword ptr [ecx*4 + gNet_players+0x24], ebx
            je L0049b8c1
            mov eax, dword ptr [gNumber_of_net_players]
            xor esi, esi
            test eax, eax
            jle L0049b845
            mov edi, offset gNet_players+0xc8
    L0049b82e:
            mov ecx, dword ptr [edi]
            call StopCarBeingIt
            mov eax, dword ptr [gNumber_of_net_players]
            inc esi
            add edi, 0xd4
            cmp esi, eax
            jl L0049b82e
    L0049b845:
            xor esi, esi
            test eax, eax
            jle L0049b8c1
            mov edi, offset gNet_players+0xc8
    L0049b850:
            cmp dword ptr [edi - 0xa4], ebx
            jne L0049b86a
            mov ecx, dword ptr [edi]
            call DisposeKevStuffCar
            mov eax, dword ptr [gNumber_of_net_players]
            mov dword ptr [gIt_or_fox], esi
    L0049b86a:
            inc esi
            add edi, 0xd4
            cmp esi, eax
            jl L0049b850
            pop edi
            pop esi
            pop ebx
            ret
    L0049b879:
            mov eax, dword ptr [gNumber_of_net_players]
            xor esi, esi
            test eax, eax
            jle L0049b8c1
            mov edi, offset gNet_players
    L0049b889:
            mov eax, dword ptr [edi + 0x80]
            test eax, eax
            jne L0049b8a7
            mov edx, 1
            mov ecx, esi
            shl edx, cl
            test edx, ebx
            je L0049b8a7
            mov ecx, edi
            call PlayerIsInfected
    L0049b8a7:
            mov eax, dword ptr [gNumber_of_net_players]
            inc esi
            add edi, 0xd4
            cmp esi, eax
            jl L0049b889
            pop edi
            pop esi
            pop ebx
            ret
    L0049b8bb:
            mov dword ptr [gINT_0068d920], ebx
    L0049b8c1:
            pop edi
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
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0049bd00
void C2_HOOK_FASTCALL InitNetGameplayStuff(void) {
}

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x0049a760
void C2_NAKED C2_HOOK_FASTCALL DeclareWinner(int pWinner_index) {
#ifndef CARPOCALYPSE2_MATCHING

    NOT_IMPLEMENTED();
#else
    __asm {
            sub esp, 0x108
            mov eax, dword ptr [gWinner_declared]
            push ebx
            mov ebx, ecx
            test eax, eax
            mov dword ptr [esp + 8], ebx
            jne L0049a934
            push edi
            push esi
            mov ecx, 1
            push ebp
            mov dword ptr [gWinner_declared], ecx
            call DoNetScores2
            xor edx, edx
            mov cl, 0xa
            call NetBuildGuaranteedMessage
            lea ecx, [ebx + ebx*2]
            mov dword ptr [eax + 0x1c], 8
            push 0
            mov edx, eax
            lea esi, [ecx + ecx*8]
            shl esi, 1
            sub esi, ebx
            shl esi, 2
            mov ecx, dword ptr [esi + gNet_players+0x24]
            push ecx
            mov ecx, dword ptr [gCurrent_net_game]
            call NetGuaranteedSendMessageToPlayer
            mov edx, dword ptr [esi + gNet_players+0x88]
            inc edx
            mov dword ptr [esi + gNet_players+0x88], edx
            mov edx, dword ptr [gCurrent_net_game]
            mov eax, dword ptr [edx + 0x74]
            cmp eax, 4
            je L0049a7df
            test eax, eax
            jne L0049a7f2
    L0049a7df:
            mov eax, dword ptr [gScore_winner]
            mov ecx, dword ptr [esi + gNet_players+0x90]
            add ecx, eax
            mov dword ptr [esi + gNet_players+0x90], ecx
    L0049a7f2:
            mov ecx, 0xbd
            call GetMiscString
            lea ecx, [esi + gNet_players+0x28]
            lea edx, [esp + 0x18]
            push eax
            push ecx
            push offset gCar_map_colours+0x10
            push edx
            call sprintf
            mov ebp, dword ptr [gNumber_of_net_players]
            add esp, 0x10
            xor edi, edi
            test ebp, ebp
            mov dword ptr [esp + 0x10], edi
            jle L0049a8d7
            mov esi, offset gNet_players+0x90
    L0049a82f:
            mov eax, dword ptr [gCurrent_net_game]
            mov eax, dword ptr [eax + 0x74]
            cmp eax, 4
            je L0049a884
            test eax, eax
            je L0049a884
            mov edx, dword ptr [esi + 4]
            test ebp, ebp
            jle L0049a86e
            mov ebx, dword ptr [esi - 0x2c]
            mov eax, offset gNet_players+0x94
            mov edi, ebp
    L0049a851:
            cmp dword ptr [eax - 0x30], ebx
            jne L0049a85e
            mov ecx, dword ptr [eax]
            cmp ecx, edx
            jge L0049a85e
            mov edx, ecx
    L0049a85e:
            add eax, 0xd4
            dec edi
            jne L0049a851
            mov edi, dword ptr [esp + 0x10]
            mov ebx, dword ptr [esp + 0x14]
    L0049a86e:
            lea ecx, [edx*4]
            mov edx, offset gScore_winner
            sub edx, ecx
            mov ecx, dword ptr [esi]
            mov eax, dword ptr [edx]
            add ecx, eax
            mov dword ptr [esi], ecx
    L0049a884:
            mov ecx, dword ptr [esi - 0x14]
            inc ecx
            cmp edi, ebx
            mov dword ptr [esi - 0x14], ecx
            je L0049a8c4
            xor edx, edx
            mov cl, 0xa
            call NetBuildGuaranteedMessage
            mov dword ptr [eax + 0x1c], 9
            mov ecx, dword ptr [esi - 0x6c]
            push 0
            push ecx
            mov ecx, dword ptr [gCurrent_net_game]
            mov edx, eax
            call NetGuaranteedSendMessageToPlayer
            mov edx, dword ptr [esi - 0x6c]
            lea ecx, [esp + 0x18]
            call NetSendHeadupToPlayer
            mov ebp, dword ptr [gNumber_of_net_players]
    L0049a8c4:
            inc edi
            add esi, 0xd4
            cmp edi, ebp
            mov dword ptr [esp + 0x10], edi
            jl L0049a82f
    L0049a8d7:
            xor edx, edx
            mov cl, 0x1e
            mov dword ptr [gReceived_game_scores], 1
            call NetBuildGuaranteedMessage
            mov ecx, dword ptr [gNumber_of_net_players]
            xor edi, edi
            test ecx, ecx
            jle L0049a922
            mov esi, offset gNet_players+0x88
            lea ecx, [eax + 0x20]
    L0049a8fe:
            mov edx, dword ptr [esi - 0xc]
            inc edi
            mov dword ptr [ecx - 4], edx
            mov edx, dword ptr [esi]
            mov dword ptr [ecx], edx
            mov edx, dword ptr [esi + 8]
            mov dword ptr [ecx + 4], edx
            mov edx, dword ptr [gNumber_of_net_players]
            add ecx, 0xc
            add esi, 0xd4
            cmp edi, edx
            jl L0049a8fe
    L0049a922:
            mov ecx, dword ptr [gCurrent_net_game]
            push 0
            mov edx, eax
            call NetGuaranteedSendMessageToAllPlayers
            pop ebp
            pop esi
            pop edi
    L0049a934:
            pop ebx
            add esp, 0x108
            ret
    }
#endif
}
#pragma auto_inline(on)

// FUNCTION: CARMA2_HW 0x0049bc20
void C2_HOOK_FASTCALL SendGameplay(tPlayer_ID pPlayer, tNet_gameplay_mess pMess, int pParam_1, int pParam_2, int pParam_3, int pParam_4) {
    tNet_message* message = NetBuildGuaranteedMessage(0x1b, 0);

    *(int *)((tU8 *)message + 0x20) = pParam_1;
    *(int *)((tU8 *)message + 0x24) = pParam_2;
    *(int *)((tU8 *)message + 0x1c) = pMess;
    *(int *)((tU8 *)message + 0x28) = pParam_3;
    *(int *)((tU8 *)message + 0x2c) = pParam_4;
    NetGuaranteedSendMessageToPlayer(gCurrent_net_game, message, pPlayer, 0);
}

// FUNCTION: CARMA2_HW 0x0049bc70
void C2_HOOK_FASTCALL SendGameplayToAllPlayers(tNet_gameplay_mess pMess, int pParam_1, int pParam_2, int pParam_3, int pParam_4) {
    tNet_message* message = NetBuildGuaranteedMessage(0x1b, 0);

    *(int *)((tU8 *)message + 0x24) = pParam_2;
    *(int *)((tU8 *)message + 0x1c) = pMess;
    *(int *)((tU8 *)message + 0x20) = pParam_1;
    *(int *)((tU8 *)message + 0x28) = pParam_3;
    *(int *)((tU8 *)message + 0x2c) = pParam_4;
    NetGuaranteedSendMessageToAllPlayers(gCurrent_net_game, message, 0);
}

void C2_HOOK_FASTCALL PlayerIsInfected(tNet_game_player_info* pPlayer);

void C2_HOOK_FASTCALL LoseAllLocalPowerups(tCar_spec* pCar);

// FUNCTION: CARMA2_HW 0x0049a9e0
void C2_HOOK_FASTCALL PlayerIsIt(tNet_game_player_info* pPlayer) {
    int i;
    char s[256];

    if (gIt_or_fox == pPlayer - gNet_players) {
        return;
    }
    for (i = 0; i < gNumber_of_net_players; i++) {
        StopCarBeingIt(gNet_players[i].car);
    }
    if (gCurrent_net_game->type == eNet_game_type_foxy) {
        pPlayer->car->power_up_levels[1] = 0;
    }
    DisposeKevStuffCar(pPlayer->car);
    gIt_or_fox = pPlayer - gNet_players;
    sprintf(s, "%s", GetMiscString(0xbc));
    NetSendHeadupToPlayer(s, pPlayer->ID);
    for (i = 0; i < gNumber_of_net_players; i++) {
        if (&gNet_players[i] != pPlayer) {
            sprintf(s, "%s %s", pPlayer->player_name, GetMiscString(0xbb));
            NetSendHeadupToPlayer(s, gNet_players[i].ID);
        }
    }
}

// FUNCTION: CARMA2_HW 0x0049abf0
void C2_HOOK_FASTCALL CalcPlayerScores(void) {
    int i;
    int j;
    int knock_out_bit;
    int cars_left;
    int car_left;
    int flags;
    int score;
    int highest;
    int next_highest;
    int lowest_score;
    tNet_game_player_info* lowest_score_player;
    int player_left;
    tCar_spec* car;
    tNet_message* message;
    tS32 time;
    char s[256];

    time = GetTotalTime();

    switch (gCurrent_net_game->type) {
    case eNet_game_type_2:
        highest = 0;
        next_highest = 0;
        for (i = 0; i < gNumber_of_net_players; i++) {
            if (gNet_players[i].score2 > highest) {
                next_highest = highest;
                highest = gNet_players[i].score2;
            } else if (gNet_players[i].score2 > next_highest) {
                next_highest = gNet_players[i].score2;
            }
        }
        gINT_0068d920 = (next_highest + highest + gCount_smashable_race_targets - GetRuntimeVariable(1)) / 2 + 1;
        if (gINT_0068d920 > gCurrent_net_game->options.starting_target) {
            gINT_0068d920 = gCurrent_net_game->options.starting_target;
        }
        break;
    case eNet_game_type_foxy:
        if (gIt_or_fox < 0) {
            gLast_lepper = NULL;
            for (i = 0; i < gNumber_of_net_players; i++) {
                if (i != -1 && gRace_board[i].meter == gNumber_of_net_players - 1) {
                    goto select_initial_it;
                }
            }
            do {
                i = IRandomBetween(0, gNumber_of_net_players - 1);
            } while (i == -1 && !gRace_board[i].car->knackered);
select_initial_it:
            PlayerIsIt(&gNet_players[i]);
        }
        if (gLast_lepper != NULL && gIt_or_fox >= 0) {
            br_vector3 difference;

            BrVector3Sub(&difference, &gLast_lepper->car->pos, &gRace_board[gIt_or_fox].car->pos);
            if (BrVector3LengthSquared(&difference) >= 4.0f) {
                gLast_lepper = NULL;
            }
        }
        break;
    default:
        break;
    }

    lowest_score = 9999;
    lowest_score_player = NULL;
    cars_left = 0;
    for (i = 0; i < gNumber_of_net_players; i++) {
        car = gRace_board[i].car;
        if (gNet_players[i].reposition_time != 0
                && time >= (int)gNet_players[i].reposition_time
                && (!gRace_finished || gRace_over_reason == eRace_not_over_yet)) {
            gNet_players[i].wasted = 0;
            gNet_players[i].reposition_time = 0;
            if (gCurrent_net_game->type == eNet_game_type_2
                    || gCurrent_net_game->type == eNet_game_type_checkpoint
                    || gCurrent_net_game->type == eNet_game_type_4) {
                BrMatrix34Copy(&car->car_master_actor->t.t.mat, (br_matrix34*)gRace_board[i].field_0x04);
            } else {
                SetInitialPosition(&gCurrent_race, gNet_players[i].opponent_list_index, gNet_players[i].grid_index);
            }
            StopCarSmokingInstantly(car);
            LoseAllLocalPowerups(car);
            InitialiseCar(car);
            TotallyRepairACar(car);
            if (car->driver == eDriver_local_human) {
                gTime_for_punishment = PDGetTotalTime() + 2000;
                EmptyInventory();
            }
            SetFlipUpCar(car);
            car->collision_info->field_0x49c = GetRaceTime() + 100;
            gSynch_race_start = 0;
            gINT_0074b780 = 1;
            message = NetBuildGuaranteedMessage(0x32, 0);
            *(int *)((tU8 *)message + 0x1c) = gProgram_state.racing;
            gNet_players[i].last_waste_message = 0;
            gNet_players[i].wasteage_attributed = 0;
            ((tU8 *)message)[0x24] = (tU8)i;
            CompressMatrix34((tCompressed_matrix3 *)((tU8 *)message + 0x26), (int *)((tU8 *)message + 0x20),
                &gCurrent_race.opponent_list[gNet_players[i].opponent_list_index].car_spec->car_master_actor->t.t.mat);
            NetGuaranteedSendMessageToAllPlayers(gCurrent_net_game, message, 0);
            if (gProgram_state.racing) {
                SendCurrentPowerups();
            }
        }
        if (gNet_players[i].last_waste_message != 0
                && !gNet_players[i].wasteage_attributed
                && time - gNet_players[i].last_waste_message > 500) {
            sprintf(s, "%s %s", gNet_players[i].player_name, GetMiscString(0xac));
            gNet_players[i].last_waste_message = 0;
            gNet_players[i].wasteage_attributed = 0;
            if (gCurrent_net_game->type == eNet_game_type_1) {
                gNet_players[i].score2--;
            }
            NetSendHeadupToEverybody(s);
        }
        SetKnackeredFlag(car);
        if (car->knackered && !gNet_players[i].wasted) {
            gNet_players[i].wasted = 1;
            message = NetBuildGuaranteedMessage(0x19, 0);
            *(int *)((tU8 *)message + 0x20) = -1;
            *(int *)((tU8 *)message + 0x1c) = gNet_players[i].ID;
            NetGuaranteedSendMessageToEverybody(gCurrent_net_game, message, 0);
            switch (gCurrent_net_game->type) {
            case eNet_game_type_fight_to_death:
                cars_left = 0;
                for (j = 0; j < gNumber_of_net_players; j++) {
                    if (!gNet_players[j].wasted) {
                        cars_left++;
                        car_left = j;
                    }
                }
                gNet_players[i].score += (&gScore_winner)[-cars_left];
                if (cars_left == 1) {
                    DeclareWinner(car_left);
                } else if (cars_left <= 0) {
                    for (j = 0; j < gNumber_of_net_players; j++) {
                        gNet_players[j].field_0x7c++;
                        message = NetBuildGuaranteedMessage(0xa, 0);
                        *(int *)((tU8 *)message + 0x1c) = 9;
                        NetGuaranteedSendMessageToPlayer(gCurrent_net_game, message, gNet_players[j].ID, 0);
                    }
                }
                break;
            case eNet_game_type_1:
            case eNet_game_type_4:
            case eNet_game_type_5:
                gNet_players[i].reposition_time = GetTotalTime() + 5000;
                break;
            case eNet_game_type_2:
                gNet_players[i].reposition_time = GetTotalTime() + 5000;
                gNet_players[i].score2 /= 2;
                break;
            case eNet_game_type_checkpoint:
                if ((gNet_players[i].score2 >> 16) != gCurrent_race.check_point_count) {
                    knock_out_bit = IRandomBetween(0, gCurrent_race.check_point_count - 1);
                    while ((gNet_players[i].score2 & (1 << knock_out_bit)) != 0) {
                        knock_out_bit++;
                        if (knock_out_bit >= gCurrent_race.check_point_count) {
                            knock_out_bit = 0;
                        }
                    }
                    gNet_players[i].score2 |= 1 << knock_out_bit;
                }
                gNet_players[i].reposition_time = GetTotalTime() + 5000;
                break;
            case eNet_game_type_6:
                gNet_players[i].reposition_time = GetTotalTime() + 5000;
                if (gNet_players[i].field_0x80 == 0) {
                    PlayerIsInfected(&gNet_players[i]);
                }
                break;
            case eNet_game_type_foxy:
                gNet_players[i].reposition_time = GetTotalTime() + 5000;
                gNet_players[i].score2 /= 2;
                if (gNumber_of_net_players > 1 && gIt_or_fox == i) {
                    gLast_lepper = NULL;
                    for (j = 0; j < gNumber_of_net_players; j++) {
                        if (j != i && gRace_board[j].meter == gNumber_of_net_players - 1) {
                            goto select_wasted_it;
                        }
                    }
                    do {
                        j = IRandomBetween(0, gNumber_of_net_players - 1);
                    } while (j == i && !gRace_board[j].car->knackered);
select_wasted_it:
                    PlayerIsIt(&gNet_players[j]);
                }
                break;
            }
        }
        switch (gCurrent_net_game->type) {
        case eNet_game_type_fight_to_death:
            if (car->knackered) {
                gNet_players[i].score2 = -1000000;
            } else {
                gNet_players[i].score2 = GetAverageCrushLevel(car);
            }
            break;
        case eNet_game_type_2:
            if (gNet_players[i].score2 >= gINT_0068d920 && !gRace_finished) {
                DeclareWinner(i);
            }
            break;
        case eNet_game_type_checkpoint:
            score = 0;
            gNet_players[i].score2 = gNet_players[i].score2 & 0xffff;
            flags = gNet_players[i].score2;
            for (j = 0; j < gCurrent_race.check_point_count; j++) {
                if ((flags & 1) != 0) {
                    score++;
                }
                flags >>= 1;
            }
            gNet_players[i].score2 |= score << 16;
            if (!score && !gRace_finished) {
                DeclareWinner(i);
            }
            break;
        case eNet_game_type_4:
            if (gNet_players[i].score2 >= 0) {
                if (gNet_players[i].score2 < lowest_score) {
                    lowest_score = gNet_players[i].score2;
                    lowest_score_player = &gNet_players[i];
                } else if (gNet_players[i].score2 == lowest_score) {
                    lowest_score_player = NULL;
                }
            }
            break;
        case eNet_game_type_foxy:
            if (i == gIt_or_fox && !gCountdown && !gRace_finished) {
                gNet_players[i].score2 += gFrame_period;
                if (gNet_players[i].score2 >= gCurrent_net_game->options.starting_target) {
                    DeclareWinner(i);
                }
            }
            break;
        }
    }
    if (gCurrent_net_game->type == eNet_game_type_4
            && lowest_score_player != NULL
            && lowest_score_player->score2 >= 0) {
        player_left = -1;
        cars_left = 0;
        for (i = 0; i < gNumber_of_net_players; i++) {
            if (gNet_players[i].score2 >= 0 && &gNet_players[i] != lowest_score_player) {
                cars_left++;
                if (player_left == -1) {
                    player_left = i;
                } else {
                    player_left = -2;
                }
            }
        }
        if (cars_left) {
            lowest_score_player->car->knackered = 1;
            lowest_score_player->wasted = 1;
            lowest_score_player->score2 += (&gScore_winner)[-cars_left];
            lowest_score_player->score2 = -1;
            if (player_left == -1) {
                for (i = 0; i < gNumber_of_net_players; i++) {
                    gNet_players[i].field_0x7c++;
                    message = NetBuildGuaranteedMessage(0xa, 0);
                    *(int *)((tU8 *)message + 0x1c) = 9;
                    NetGuaranteedSendMessageToPlayer(gCurrent_net_game, message, gNet_players[i].ID, 0);
                }
            } else if (player_left >= 0) {
                DeclareWinner(player_left);
            } else {
                SendGameplay(lowest_score_player->ID, (tNet_gameplay_mess)2, 0, 0, 0, 0);
                sprintf(s, "%s %s", lowest_score_player->player_name, GetMiscString(0xb8));
                NetSendHeadupToEverybody(s);
            }
        }
    }
}

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x0049b540
void C2_NAKED C2_HOOK_FASTCALL PlayerIsInfected(tNet_game_player_info* pPlayer) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;
    int offset;
    int infected_count;
    int winner_index;
    tPlayer_ID* player_id;
    int* player_infected;
    char message[256];

    if (pPlayer->field_0x80 == 0) {
        DisposeKevStuffCar(pPlayer->car);
        *(int*)((tU8*)pPlayer->car + 0x1bc) = 1;
        *(int*)((tU8*)pPlayer->car + 0x1b4) = 1;
        *(int*)((tU8*)pPlayer->car + 0x1b8) = 1;
        pPlayer->field_0x80 = 1;
        offset = 0x18a8;
        do {
            offset += 4;
            *(tU32*)(offset - 4 + (tU8*)pPlayer->car) = *(tU32*)(offset + 8 + (tU8*)pPlayer->car);
        } while (offset < 0x18b4);
        if (gNet_mode == eNet_mode_host) {
            sprintf(message, "%s", GetMiscString(0xba));
            NetSendHeadupToPlayer(message, pPlayer->ID);
            i = 0;
            if (0 < gNumber_of_net_players) {
                player_id = &gNet_players[0].ID;
                do {
                    if ((tNet_game_player_info*)(player_id - 9) != pPlayer) {
                        sprintf(message, "%s %s", pPlayer->player_name, GetMiscString(0xb9));
                        NetSendHeadupToPlayer(message, *player_id);
                    }
                    i++;
                    player_id += 0x35;
                } while (i < gNumber_of_net_players);
            }
        }
        pPlayer->score2 = -1 - GetTotalTime();
        infected_count = 0;
        i = gProgram_state.racing;
        winner_index = 0;
        if (i != 0 && gNet_mode == eNet_mode_host) {
            i = 0;
            if (0 < gNumber_of_net_players) {
                player_infected = &gNet_players[0].field_0x80;
                do {
                    if (*player_infected == 0) {
                        infected_count++;
                        winner_index = i;
                    }
                    i++;
                    player_infected += 0x35;
                } while (i < gNumber_of_net_players);
            }
            if (infected_count <= 1) {
                DeclareWinner(winner_index);
            }
        }
    }
#else
    __asm {
            sub esp, 0x100
            push ebx
            push esi
            mov esi, ecx
            push edi
            mov eax, dword ptr [esi + 0x80]
            test eax, eax
            jne L0049b686
            mov ecx, dword ptr [esi + 0xc8]
            call DisposeKevStuffCar
            mov ecx, dword ptr [esi + 0xc8]
            mov eax, 1
            mov dword ptr [ecx + 0x1bc], eax
            mov edx, dword ptr [esi + 0xc8]
            mov dword ptr [edx + 0x1b4], eax
            mov ecx, dword ptr [esi + 0xc8]
            mov dword ptr [ecx + 0x1b8], eax
            mov dword ptr [esi + 0x80], eax
            mov eax, 0x18a8
    L0049b598:
            mov ecx, dword ptr [esi + 0xc8]
            add eax, 4
            cmp eax, 0x18b4
            mov edx, dword ptr [eax + ecx + 8]
            mov dword ptr [eax + ecx - 4], edx
            jl L0049b598
            cmp dword ptr [gNet_mode], 2
            jne L0049b635
            mov ecx, 0xba
            call GetMiscString
            push eax
            lea eax, [esp + 0x10]
            push offset gCancel_recover+0xbc
            push eax
            call sprintf
            mov edx, dword ptr [esi + 0x24]
            add esp, 0xc
            lea ecx, [esp + 0xc]
            call NetSendHeadupToPlayer
            mov eax, dword ptr [gNumber_of_net_players]
            xor ebx, ebx
            test eax, eax
            jle L0049b635
            mov edi, offset gNet_players+0x24
    L0049b5f2:
            lea ecx, [edi - 0x24]
            cmp ecx, esi
            je L0049b625
            mov ecx, 0xb9
            call GetMiscString
            lea edx, [esi + 0x28]
            push eax
            push edx
            lea eax, [esp + 0x14]
            push offset gCar_map_colours+0x10
            push eax
            call sprintf
            mov edx, dword ptr [edi]
            add esp, 0x10
            lea ecx, [esp + 0xc]
            call NetSendHeadupToPlayer
    L0049b625:
            mov eax, dword ptr [gNumber_of_net_players]
            inc ebx
            add edi, 0xd4
            cmp ebx, eax
            jl L0049b5f2
    L0049b635:
            call GetTotalTime
            or ecx, 0xffffffff
            xor edi, edi
            sub ecx, eax
            mov dword ptr [esi + 0x64], ecx
            mov eax, dword ptr [gProgram_state+0x28]
            xor ecx, ecx
            test eax, eax
            je L0049b686
            cmp dword ptr [gNet_mode], 2
            jne L0049b686
            mov esi, dword ptr [gNumber_of_net_players]
            xor eax, eax
            test esi, esi
            jle L0049b67c
            mov edx, offset gNet_players+0x80
    L0049b669:
            cmp dword ptr [edx], 0
            jne L0049b671
            inc edi
            mov ecx, eax
    L0049b671:
            inc eax
            add edx, 0xd4
            cmp eax, esi
            jl L0049b669
    L0049b67c:
            cmp edi, 1
            jg L0049b686
            call DeclareWinner
    L0049b686:
            pop edi
            pop esi
            pop ebx
            add esp, 0x100
            ret
    }
#endif
}
#pragma auto_inline(on)

void C2_HOOK_FASTCALL LoseAllLocalPowerups(tCar_spec* pCar) {

    NOT_IMPLEMENTED();
}

__inline void C2_HOOK_FASTCALL SendPlayerScores(void) {
    tNet_message_chunk* the_contents;
    int i;

    the_contents = NetStartBroadcastContents(24, 0);
    if (the_contents == NULL) {
        return;
    }
    switch (gCurrent_net_game->type) {
    case eNet_game_type_2:
        the_contents->scores.general_score = gINT_0068d920;
        break;
    case eNet_game_type_6:
        the_contents->scores.general_score = 0;
        for (i = 0; i < gNumber_of_net_players; i++) {
            if (gNet_players[i].field_0x80) {
                the_contents->scores.general_score |= 1 << i;
            }
        }
        break;
    case eNet_game_type_foxy:
        the_contents->scores.general_score = gNet_players[gIt_or_fox].ID;
        break;
    default:
        break;
    }
    for (i = 0; i < gNumber_of_net_players; i++) {
        the_contents->scores.scores[i] = gNet_players[i].score2;
    }
    NetBroadcastContents(the_contents);
}

// FUNCTION: CARMA2_HW 0x0049ab00
void C2_HOOK_FASTCALL DoNetGameManagement(void) {
    // GLOBAL: CARMA2_HW 0x00659c34
    static tU32 last_playerscores_sent = -1;

    if (gNet_mode == eNet_mode_host) {
        CalcPlayerScores();
        if (gPHIL_last_physics_tick <= last_playerscores_sent || gPHIL_last_physics_tick >= last_playerscores_sent + 80) {
            last_playerscores_sent = gPHIL_last_physics_tick;
            SendPlayerScores();
        }
    }
}

// FUNCTION: CARMA2_HW 0x00499000
void C2_HOOK_FASTCALL SetUpNetCarPositions(void) {
    int i;
    int j;
    int k;
    int grid_index;
    int racer_count;

    DisableNetService();
    if (!gInitialised_grid) {
        for (i = 0; i < gNumber_of_net_players; i++) {
            gNet_players[i].grid_position_set = 0;
            gNet_players[i].last_waste_message = 0;
            gNet_players[i].wasteage_attributed = 0;
        }
    }
    for (i = 0; i < gNumber_of_net_players; i++) {
        gCurrent_race.opponent_list[i].index = -1;
        gCurrent_race.opponent_list[i].ranking = IRandomBetween(0, 99);
        gCurrent_race.opponent_list[i].car_spec = gNet_players[i].car;
        gCurrent_race.opponent_list[i].net_player_index = i;
        gNet_players[i].opponent_list_index = i;
    }
    racer_count = gNumber_of_net_players;
    if (!gInitialised_grid && gCurrent_net_game->options.grid_start) {
        qsort(gCurrent_race.opponent_list, gNumber_of_net_players, sizeof(tOpp_spec), SortGridFunction);
    }
    gCurrent_race.number_of_racers = 0;
    for (i = 0; i < gNumber_of_net_players; i++) {
        gNet_players[gCurrent_race.opponent_list[i].net_player_index].opponent_list_index = i;
    }
    for (i = 0; i < racer_count; i++) {
        if ((gCurrent_race.opponent_list[i].car_spec != NULL
                && gCurrent_race.opponent_list[i].car_spec->driver == eDriver_oppo
                && !gInitialised_grid)
            || (gCurrent_race.opponent_list[i].car_spec != NULL
                && gCurrent_race.opponent_list[i].car_spec->driver >= eDriver_net_human
                && !gNet_players[gCurrent_race.opponent_list[i].net_player_index].grid_position_set)) {
            grid_index = -1;
            for (j = 0; j < (int)CARPOCALYPSE2_ASIZE(gNet_players) && grid_index < 0; j++) {
                grid_index = j;
                for (k = 0; k < racer_count; k++) {
                    if (k != i
                        && gNet_players[gCurrent_race.opponent_list[k].net_player_index].grid_position_set
                        && gNet_players[gCurrent_race.opponent_list[k].net_player_index].grid_index == j) {
                        grid_index = -1;
                        break;
                    }
                }
            }
            if (grid_index < 0) {
                FatalError(kFatalError_NetworkCodeSelfCheck);
            }
            SetInitialPosition(&gCurrent_race, i, grid_index);
            gNet_players[gCurrent_race.opponent_list[i].net_player_index].grid_index = grid_index;
            if (!gInitialised_grid) {
                gCurrent_race.number_of_racers = i + 1;
            } else {
                InitialiseCar2(gCurrent_race.opponent_list[i].car_spec, 0);
            }
            gNet_players[gCurrent_race.opponent_list[i].net_player_index].grid_position_set = 1;
        }
    }
    gCurrent_race.number_of_racers = racer_count;
    gInitialised_grid = 1;
    ReenableNetService();
}

// FUNCTION: CARMA2_HW 0x004985f0
void C2_NAKED C2_HOOK_FASTCALL ReceivedCarData(tNet_message_chunk_car_data* pContents) {
#ifndef CARPOCALYPSE2_MATCHING
    tCar_spec* car;
    int player_index;
    int i;
    int input_axis;
    int axis_value;
    tU32 repair_delta;
    float local_4;

    car = NULL;
    for (player_index = 0; player_index < gNumber_of_net_players; player_index++) {
        if (gNet_players[player_index].ID == pContents->player_id) {
            car = gNet_players[player_index].car;
            break;
        }
    }
    if (car == NULL) {
        return;
    }

    if (car->collision_info->message_time > pContents->next_frame_time) {
        return;
    }

    if (car->disabled != 0) {
        EnableCar(car);
        SelectCrushNetwork((undefined4*)pContents->object_hierarchy, car->collision_info, 2);
        car->collision_info->box_face_ref = gFace_num__car - 2;
        car->collision_info->message_time = pContents->next_frame_time;
        car->collision_info->field_0x261 = 0xff;
        for (i = 0; i < 4; i++) {
            car->collision_info->field_0x26c[i] = pContents->suspension_compression[i];
        }
        for (i = 0; i < 12; i++) {
            car->collision_info->field_0x280[i] = pContents->damage_levels[i];
        }
        DisableCar(car);
        return;
    }

    if (gNet_mode == eNet_mode_host) {
        if (player_index == gThis_net_player_index) {
            return;
        }
        if (pContents->hierarchy_flags >= (tU32)car->collision_info->field_0x49c) {
            FUN_00498ab0(car, (tU8*)pContents);
            if (pContents->crush_state != car->car_crush_spec->field_0x24) {
                SetAllShapeStatusFlags(car->car_crush_spec->network_stuff);
                if (car->car_crush_spec->network_stuff == NULL) {
                    car->car_crush_spec->field_0x26 = pContents->crush_state;
                } else {
                    for (i = 0; i < 12; i++) {
                        if (car->car_crush_spec->network_stuff->items[i].network_id == (int)pContents->player_id) {
                            car->car_crush_spec->network_stuff->items[i].field_0x4 = 0;
                            break;
                        }
                    }
                    if (i == 12) {
                        car->car_crush_spec->field_0x26 = pContents->crush_state;
                    }
                }
            }
        } else {
            if (pContents->hierarchy_flags == 0) {
                car->collision_info->field_0x274 = 0;
                car->collision_info->field_0x261 = 0;
            }
            if (pContents->crush_state == car->car_crush_spec->field_0x24) {
                if (car->car_crush_spec->network_stuff != NULL) {
                    for (i = 0; i < 12; i++) {
                        if (car->car_crush_spec->network_stuff->items[i].network_id == (int)pContents->player_id) {
                            car->car_crush_spec->network_stuff->items[i].field_0x4 = 0;
                            break;
                        }
                    }
                }
            }
        }
        car->power_up_levels[0] = pContents->power_up_levels[0];
        car->power_up_levels[1] = pContents->power_up_levels[1];
        car->power_up_levels[2] = pContents->power_up_levels[2];
        car->keys = *(tCar_controls*)&pContents->controls;
        axis_value = (int)(pContents->controls << 24);
        input_axis = -1;
        if (axis_value >= 0) {
            input_axis = axis_value >> 15;
        }
        car->joystick.acc = input_axis;
        axis_value = (int)((pContents->controls & 0xffffff00U) << 16);
        if (axis_value >= 0) {
            input_axis = axis_value >> 15;
        } else {
            input_axis = -1;
        }
        car->joystick.dec = input_axis;
        return;
    }

    if (gNet_mode != eNet_mode_client) {
        return;
    }
    if (gCar_to_view_id == gNet_players[player_index].ID) {
        ReallyViewOpponent();
    }
    if (player_index == gThis_net_player_index) {
        car->car_crush_spec->field_0x26 = pContents->crush_state;
        if (pContents->hierarchy_flags <= (tU32)car->collision_info->field_0x49c) {
            return;
        }
        FUN_00498ab0(car, (tU8*)pContents);
        return;
    }

    FUN_00498ab0(car, (tU8*)pContents);
    car->power_up_levels[0] = pContents->power_up_levels[0];
    car->power_up_levels[1] = pContents->power_up_levels[1];
    car->power_up_levels[2] = pContents->power_up_levels[2];
    car->keys = *(tCar_controls*)&car->collision_info->field_0x26c[4];
    axis_value = (int)(*(tU32*)&car->collision_info->field_0x26c[4] << 24);
    input_axis = -1;
    if (axis_value >= 0) {
        input_axis = axis_value >> 15;
    }
    car->joystick.acc = input_axis;
    axis_value = (int)((*(tU32*)&car->collision_info->field_0x26c[4] & 0xffffff00U) << 16);
    if (axis_value >= 0) {
        input_axis = axis_value >> 15;
    } else {
        input_axis = -1;
    }
    car->joystick.dec = input_axis;
    if (car->active != 0) {
        return;
    }

    SelectCrushNetwork((undefined4*)pContents->object_hierarchy, car->collision_info, 2);
    car->pos.v[0] = car->collision_info->transform_matrix.m[3][0];
    car->pos.v[1] = car->collision_info->transform_matrix.m[3][1];
    car->pos.v[2] = car->collision_info->transform_matrix.m[3][2];
    car->collision_info->pos.v[0] = car->collision_info->transform_matrix.m[3][0];
    car->collision_info->pos.v[1] = car->collision_info->transform_matrix.m[3][1];
    car->collision_info->pos.v[2] = car->collision_info->transform_matrix.m[3][2];

    if (car->driver >= eDriver_net_human && car->collision_info->field_0x27c > car->repair_time) {
        repair_delta = car->collision_info->field_0x27c - car->repair_time;
        if (repair_delta >= 100000) {
            TotallyRepairACar(car);
            car->repair_time = car->collision_info->field_0x27c;
        } else {
            ComputeCarImpact(car, repair_delta, &local_4);
        }
        for (i = 0; i < 12; i++) {
            car->damage_units[i].damage_level = car->collision_info->field_0x280[i];
        }
        SetSmokeLastDamageLevel(car);
        StopCarSmoking(car);
    } else {
        for (i = 0; i < 12; i++) {
            car->damage_units[i].damage_level = car->collision_info->field_0x280[i];
        }
        SortOutSmoke(car);
    }

    car->collision_info->box_face_ref = gFace_num__car - 2;
    if ((car->collision_info->field_0x292 & 1) != 0
            && car->collision_info->field_0x290 != car->car_crush_spec->field_0x24) {
        if ((car->collision_info->field_0x292 & 2) != 0) {
            WeldCarByNetwork(car);
        } else if (car->car_crush_spec->field_0x144 != 0) {
            CrushCarByNetwork(car);
        }
        if ((car->collision_info->field_0x292 & 4) != 0) {
            WeldCarPartial(car);
        } else if (car->car_crush_spec->field_0x4b8 != 0) {
            WeldCar(car);
        }
        UpdateCrushVertices(car, (tU8*)&car->collision_info->field_0x2fc);
    }
#else
    __asm {
            push ecx
            push ebx
            push ebp
            push esi
            push edi
            mov edi, ecx
            xor ebx, ebx
            mov ecx, dword ptr [gNumber_of_net_players]
            xor esi, esi
            xor ebp, ebp
            cmp ecx, ebx
            jle L00498631
            mov edx, dword ptr [edi + 4]
            mov eax, offset gNet_players+0x24
    L0049860f:
            cmp dword ptr [eax], edx
            je L0049861f
            inc ebp
            add eax, 0xd4
            cmp ebp, ecx
            jl L0049860f
            jmp L00498631
    L0049861f:
            lea eax, [ebp + ebp*2]
            lea eax, [eax + eax*8]
            shl eax, 1
            sub eax, ebp
            mov esi, dword ptr [eax*4 + gNet_players+0xc8]
    L00498631:
            cmp esi, ebx
            je L00498a9d
            mov eax, dword ptr [esi + 8]
            mov edx, dword ptr [edi + 8]
            mov ecx, dword ptr [eax + 0x268]
            cmp ecx, edx
            ja L00498a9d
            cmp dword ptr [esi + 4], ebx
            je L004986c8
            mov ecx, esi
            call EnableCar
            mov edx, dword ptr [esi + 8]
            lea ecx, [edi + 0x34]
            push 2
            call SelectCrushNetwork
            mov edx, dword ptr [gFace_num__car]
            mov eax, dword ptr [esi + 8]
            add edx, -2
            mov word ptr [eax + 0x178], dx
            mov ecx, dword ptr [esi + 8]
            mov edx, dword ptr [edi + 8]
            mov dword ptr [ecx + 0x268], edx
            mov eax, dword ptr [esi + 8]
            mov byte ptr [eax + 0x261], 0xff
            xor eax, eax
    L00498691:
            mov ecx, dword ptr [esi + 8]
            mov dl, byte ptr [eax + edi + 0xc]
            mov byte ptr [eax + ecx + 0x26c], dl
            inc eax
            cmp eax, 4
            jl L00498691
            xor eax, eax
    L004986a7:
            mov ecx, dword ptr [esi + 8]
            mov dl, byte ptr [eax + edi + 0x20]
            mov byte ptr [eax + ecx + 0x280], dl
            inc eax
            cmp eax, 0xc
            jl L004986a7
            mov ecx, esi
            call DisableCar
            pop edi
            pop esi
            pop ebp
            pop ebx
            pop ecx
            ret
    L004986c8:
            mov ecx, dword ptr [gNet_mode]
            cmp ecx, 2
            jne L0049880f
            cmp ebp, dword ptr [gThis_net_player_index]
            je L00498a9d
            mov ecx, dword ptr [edi + 0x14]
            mov edx, dword ptr [eax + 0x49c]
            cmp edx, ecx
            ja L0049875a
            mov edx, edi
            mov ecx, esi
            call FUN_00498ab0
            mov eax, dword ptr [esi + 0x18d4]
            mov cx, word ptr [edi + 0x30]
            cmp cx, word ptr [eax + 0x24]
            je L0049879e
            mov ecx, dword ptr [eax + 0x20]
            call SetAllShapeStatusFlags
            mov edx, dword ptr [esi + 0x18d4]
            mov ebp, dword ptr [edi + 4]
            mov edx, dword ptr [edx + 0x20]
            cmp edx, ebx
            je L0049874a
            xor eax, eax
            mov ecx, edx
    L00498729:
            cmp dword ptr [ecx], ebp
            je L00498746
            inc eax
            add ecx, 8
            cmp eax, 0xc
            jl L00498729
            mov eax, dword ptr [esi + 0x18d4]
            mov cx, word ptr [edi + 0x30]
            mov word ptr [eax + 0x26], cx
            jmp L0049879e
    L00498746:
            mov byte ptr [edx + eax*8 + 4], bl
    L0049874a:
            mov eax, dword ptr [esi + 0x18d4]
            mov cx, word ptr [edi + 0x30]
            mov word ptr [eax + 0x26], cx
            jmp L0049879e
    L0049875a:
            cmp ecx, ebx
            jne L0049876d
            mov dword ptr [eax + 0x274], ebx
            mov edx, dword ptr [esi + 8]
            mov byte ptr [edx + 0x261], bl
    L0049876d:
            mov eax, dword ptr [esi + 0x18d4]
            mov cx, word ptr [edi + 0x30]
            cmp cx, word ptr [eax + 0x24]
            jne L0049879e
            mov edx, dword ptr [eax + 0x20]
            mov ebp, dword ptr [edi + 4]
            cmp edx, ebx
            je L0049879e
            xor eax, eax
            mov ecx, edx
    L0049878b:
            cmp dword ptr [ecx], ebp
            je L0049879a
            inc eax
            add ecx, 8
            cmp eax, 0xc
            jl L0049878b
            jmp L0049879e
    L0049879a:
            mov byte ptr [edx + eax*8 + 4], bl
    L0049879e:
            xor edx, edx
            xor eax, eax
            mov dl, byte ptr [edi + 0x2c]
            xor ecx, ecx
            mov dword ptr [esi + 0x18a8], edx
            mov al, byte ptr [edi + 0x2d]
            mov dword ptr [esi + 0x18ac], eax
            mov cl, byte ptr [edi + 0x2e]
            mov dword ptr [esi + 0x18b0], ecx
            mov edx, dword ptr [edi + 0x10]
            mov dword ptr [esi + 0x12d0], edx
            mov eax, dword ptr [edi + 0x10]
            shl eax, 0x18
            mov ecx, 0xffffffff
            js L004987e0
            sar eax, 0xf
            mov dword ptr [esi + 0x12dc], eax
            jmp L004987e6
    L004987e0:
            mov dword ptr [esi + 0x12dc], ecx
    L004987e6:
            mov edi, dword ptr [edi + 0x10]
            and edi, 0xffffff00
            shl edi, 0x10
            js L00498803
            sar edi, 0xf
            mov dword ptr [esi + 0x12e0], edi
            pop edi
            pop esi
            pop ebp
            pop ebx
            pop ecx
            ret
    L00498803:
            mov dword ptr [esi + 0x12e0], ecx
            pop edi
            pop esi
            pop ebp
            pop ebx
            pop ecx
            ret
    L0049880f:
            cmp ecx, 3
            jne L00498a9d
            lea eax, [ebp + ebp*2]
            mov ecx, dword ptr [gCar_to_view_id]
            lea eax, [eax + eax*8]
            shl eax, 1
            sub eax, ebp
            cmp ecx, dword ptr [eax*4 + gNet_players+0x24]
            jne L00498837
            call ReallyViewOpponent
    L00498837:
            cmp ebp, dword ptr [gThis_net_player_index]
            je L00498a76
            mov edx, edi
            mov ecx, esi
            call FUN_00498ab0
            xor edx, edx
            xor eax, eax
            mov dl, byte ptr [edi + 0x2c]
            xor ecx, ecx
            mov dword ptr [esi + 0x18a8], edx
            mov al, byte ptr [edi + 0x2d]
            mov dword ptr [esi + 0x18ac], eax
            mov cl, byte ptr [edi + 0x2e]
            mov eax, dword ptr [esi + 8]
            mov dword ptr [esi + 0x18b0], ecx
            mov ecx, 0xffffffff
            mov edx, dword ptr [eax + 0x270]
            mov dword ptr [esi + 0x12d0], edx
            mov eax, dword ptr [eax + 0x270]
            shl eax, 0x18
            js L004988a3
            mov edx, dword ptr [esi + 8]
            mov eax, dword ptr [edx + 0x270]
            shl eax, 0x18
            sar eax, 0xf
            mov dword ptr [esi + 0x12dc], eax
            jmp L004988a9
    L004988a3:
            mov dword ptr [esi + 0x12dc], ecx
    L004988a9:
            mov edx, dword ptr [esi + 8]
            mov eax, dword ptr [edx + 0x270]
            and al, 0
            shl eax, 0x10
            js L004988d2
            mov ecx, edx
            mov edx, dword ptr [ecx + 0x270]
            and dl, 0
            shl edx, 0x10
            sar edx, 0xf
            mov dword ptr [esi + 0x12e0], edx
            jmp L004988d8
    L004988d2:
            mov dword ptr [esi + 0x12e0], ecx
    L004988d8:
            cmp dword ptr [esi + 0x1d0], ebx
            jne L00498a9d
            mov edx, dword ptr [esi + 8]
            push 2
            lea ecx, [edi + 0x34]
            call SelectCrushNetwork
            mov eax, dword ptr [esi + 8]
            mov ecx, dword ptr [eax + 0xb0]
            mov dword ptr [esi + 0x4c], ecx
            mov edx, dword ptr [eax + 0xb4]
            mov dword ptr [esi + 0x50], edx
            mov eax, dword ptr [eax + 0xb8]
            mov dword ptr [esi + 0x54], eax
            mov eax, dword ptr [esi + 8]
            mov ecx, dword ptr [eax + 0xb0]
            mov dword ptr [eax + 0x80], ecx
            mov eax, dword ptr [esi + 8]
            mov edx, dword ptr [eax + 0xb4]
            mov dword ptr [eax + 0x84], edx
            mov eax, dword ptr [esi + 8]
            mov ecx, dword ptr [eax + 0xb8]
            mov dword ptr [eax + 0x88], ecx
            mov eax, dword ptr [esi + 0xc]
            cmp eax, 7
            jl L004989b7
            mov edx, dword ptr [esi + 8]
            mov eax, dword ptr [esi + 0x18a4]
            mov edx, dword ptr [edx + 0x27c]
            cmp edx, eax
            jbe L004989b7
            sub edx, eax
            cmp edx, 0x186a0
            jb L00498979
            mov ecx, esi
            call TotallyRepairACar
            mov eax, dword ptr [esi + 8]
            mov ecx, dword ptr [eax + 0x27c]
            mov dword ptr [esi + 0x18a4], ecx
            jmp L00498987
    L00498979:
            lea eax, [esp + 0x10]
            mov ecx, esi
            push eax
            call ComputeCarImpact
            fstp st(0)
    L00498987:
            xor eax, eax
            lea ecx, [esi + 0x4e0]
    L0049898f:
            mov edi, dword ptr [esi + 8]
            xor edx, edx
            add ecx, 0x2c
            mov dl, byte ptr [eax + edi + 0x280]
            inc eax
            mov dword ptr [ecx - 0x2c], edx
            cmp eax, 0xc
            jl L0049898f
            mov ecx, esi
            call SetSmokeLastDamageLevel
            mov ecx, esi
            call StopCarSmoking
            jmp L004989de
    L004989b7:
            xor eax, eax
            lea ecx, [esi + 0x4e0]
    L004989bf:
            mov edi, dword ptr [esi + 8]
            xor edx, edx
            add ecx, 0x2c
            mov dl, byte ptr [eax + edi + 0x280]
            inc eax
            mov dword ptr [ecx - 0x2c], edx
            cmp eax, 0xc
            jl L004989bf
            mov ecx, esi
            call SortOutSmoke
    L004989de:
            mov eax, dword ptr [gFace_num__car]
            mov ecx, dword ptr [esi + 8]
            add eax, -2
            mov word ptr [ecx + 0x178], ax
            mov eax, dword ptr [esi + 8]
            mov cl, byte ptr [eax + 0x292]
            test cl, 1
            je L00498a9d
            mov edx, dword ptr [esi + 0x18d4]
            mov ax, word ptr [eax + 0x290]
            cmp ax, word ptr [edx + 0x24]
            je L00498a9d
            test cl, 2
            je L00498a27
            mov ecx, esi
            call WeldCarByNetwork
            jmp L00498a36
    L00498a27:
            cmp byte ptr [edx + 0x144], bl
            je L00498a36
            mov ecx, esi
            call CrushCarByNetwork
    L00498a36:
            mov ecx, dword ptr [esi + 8]
            test byte ptr [ecx + 0x292], 4
            je L00498a4b
            mov ecx, esi
            call WeldCarPartial
            jmp L00498a60
    L00498a4b:
            mov edx, dword ptr [esi + 0x18d4]
            cmp byte ptr [edx + 0x4b8], bl
            je L00498a60
            mov ecx, esi
            call WeldCar
    L00498a60:
            mov edx, dword ptr [esi + 8]
            mov ecx, esi
            add edx, 0x2fc
            call UpdateCrushVertices
            pop edi
            pop esi
            pop ebp
            pop ebx
            pop ecx
            ret
    L00498a76:
            mov eax, dword ptr [esi + 0x18d4]
            mov cx, word ptr [edi + 0x30]
            mov word ptr [eax + 0x26], cx
            mov edx, dword ptr [esi + 8]
            mov ecx, dword ptr [edi + 0x14]
            mov eax, dword ptr [edx + 0x49c]
            cmp eax, ecx
            jae L00498a9d
            mov edx, edi
            mov ecx, esi
            call FUN_00498ab0
    L00498a9d:
            pop edi
            pop esi
            pop ebp
            pop ebx
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
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00498ab0
void C2_NAKED C2_HOOK_FASTCALL FUN_00498ab0(void* pArg1, tU8* pSrc) {
#ifndef CARPOCALYPSE2_MATCHING
    tU8* data;
    tU8* base;
    int size;
    int i;

    memcpy(*(tU8**)((tU8*)pArg1 + 8) + 0x260, pSrc, 0x34);
    data = pSrc + 0x34;
    size = GetNetworkDataSize(data);
    memcpy(*(tU8**)((tU8*)pArg1 + 8) + 0x294, data, size);
    data += size;
    if ((*(tU8*)(*(tU8**)((tU8*)pArg1 + 8) + 0x292) & 8) != 0) {
        for (i = 0x48c; i < 0x49c; i += 4) {
            base = *(tU8**)((tU8*)pArg1 + 8);
            *(tU32*)(base + i) = *(tU32*)data;
            data += 4;
        }
    } else {
        for (i = 0x48c; i < 0x49c; i += 4) {
            base = *(tU8**)((tU8*)pArg1 + 8);
            *(tU32*)(base + i) = 0;
        }
    }
    if ((*(tU8*)(*(tU8**)((tU8*)pArg1 + 8) + 0x292) & 1) != 0) {
        size = GetCompressedCrushVertexDataSize(data);
        memcpy(*(tU8**)((tU8*)pArg1 + 8) + 0x2fc, data, size);
        data += size;
    }
    if ((*(tU8*)(*(tU8**)((tU8*)pArg1 + 8) + 0x292) & 2) != 0) {
        *(tU8*)(*(tU8**)((tU8*)pArg1 + 8) + 0x481) = *data;
        data += 1;
        *(tU16*)(*(tU8**)((tU8*)pArg1 + 8) + 0x482) = *(tU16*)data;
        data += 2;
    }
    base = *(tU8**)((tU8*)pArg1 + 8);
    if ((*(tU8*)(base + 0x292) & 4) != 0) {
        *(tU32*)(base + 0x484) = *(tU32*)data;
        *(tU32*)(base + 0x488) = *(tU32*)(data + 4);
    }
#else
    __asm {
            push ebx
            push ebp
            mov ebp, ecx
            push esi
            push edi
            mov ecx, 0xd
            mov edi, dword ptr [ebp + 8]
            mov esi, edx
            add edi, 0x260
            lea ebx, [edx + 0x34]
            rep movsd
            mov ecx, ebx
            call GetNetworkDataSize
            mov edi, dword ptr [ebp + 8]
            mov ecx, eax
            mov edx, ecx
            mov esi, ebx
            add edi, 0x294
            add ebx, eax
            shr ecx, 2
            rep movsd
            mov ecx, edx
            and ecx, 3
            rep movsb
            mov eax, dword ptr [ebp + 8]
            mov cl, byte ptr [eax + 0x292]
            mov eax, 0x48c
            test cl, 8
            je L00498b19
    L00498b02:
            mov ecx, dword ptr [ebp + 8]
            mov edx, dword ptr [ebx]
            add ebx, 4
            mov dword ptr [ecx + eax], edx
            add eax, 4
            cmp eax, 0x49c
            jl L00498b02
            jmp L00498b2e
    L00498b19:
            mov ecx, dword ptr [ebp + 8]
            add eax, 4
            cmp eax, 0x49c
            mov dword ptr [ecx + eax - 4], 0
            jl L00498b19
    L00498b2e:
            mov edx, dword ptr [ebp + 8]
            test byte ptr [edx + 0x292], 1
            je L00498b5e
            mov ecx, ebx
            call GetCompressedCrushVertexDataSize
            mov edi, dword ptr [ebp + 8]
            mov ecx, eax
            mov edx, ecx
            mov esi, ebx
            add edi, 0x2fc
            shr ecx, 2
            rep movsd
            mov ecx, edx
            and ecx, 3
            add ebx, eax
            rep movsb
    L00498b5e:
            mov eax, dword ptr [ebp + 8]
            test byte ptr [eax + 0x292], 2
            je L00498b83
            mov cl, byte ptr [ebx]
            inc ebx
            mov byte ptr [eax + 0x481], cl
            mov edx, dword ptr [ebp + 8]
            mov ax, word ptr [ebx]
            add ebx, 2
            mov word ptr [edx + 0x482], ax
    L00498b83:
            mov ebp, dword ptr [ebp + 8]
            test byte ptr [ebp + 0x292], 4
            je L00498ba0
            mov ecx, dword ptr [ebx]
            mov dword ptr [ebp + 0x484], ecx
            mov edx, dword ptr [ebx + 4]
            mov dword ptr [ebp + 0x488], edx
    L00498ba0:
            pop edi
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

// FUNCTION: CARMA2_HW 0x00498e70
void C2_HOOK_FASTCALL SignalToStartRace(void) {
    tNet_message* the_message;
    int inactive;
    int i;
    int j;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message, contents, 0x18);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_start_race, car_count, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_start_race, racing, 0x3);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_start_race, next_race, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_start_race, car_list, 0x6);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tNet_message_chunk_start_race, car_list, 0xf0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_start_race_car, next_car_index, 0x1);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_start_race_car, matrix, 0x2);
    C2_HOOK_BUG_ON(sizeof(tNet_start_race_car) != 0x14);

    gCurrent_net_game->no_races_yet = 0;
    gSynch_race_start = 0;
    gINT_0074b780 = 1;
    the_message = NetBuildGuaranteedMessage(eNet_message_chunk_type_start_race, 0);
    the_message->contents.start_race.racing = (tU8)gProgram_state.racing;
    the_message->contents.start_race.car_count = (tU8)gCurrent_race.number_of_racers;
    for (i = 0; i < gCurrent_race.number_of_racers; i++) {
        CompressMatrix34(&the_message->contents.start_race.car_list[i].matrix, &inactive,
            &gCurrent_race.opponent_list[i].car_spec->car_master_actor->t.t.mat);
        for (j = 0; j < gNumber_of_net_players; j++) {
            if (gCurrent_race.opponent_list[i].car_spec == gNet_players[j].car) {
                the_message->contents.start_race.car_list[i].index = (tU8)j;
                if (inactive) {
                    the_message->contents.start_race.car_list[i].index |= 0x80;
                }
                break;
            }
        }
        if (gCurrent_net_game->options.car_choice == eNet_car_all
                || gCurrent_net_game->options.car_choice == eNet_car_novelty) {
            if (gNet_players[the_message->contents.start_race.car_list[i].index & 0x7f].next_car_index < 0) {
                gNet_players[the_message->contents.start_race.car_list[i].index & 0x7f].next_car_index = PickARandomCar();
                gCar_details[gNet_players[the_message->contents.start_race.car_list[i].index & 0x7f].next_car_index].ownership = eCar_owner_someone;
            }
            the_message->contents.start_race.car_list[i].next_car_index = (tU8)gNet_players[the_message->contents.start_race.car_list[i].index & 0x7f].next_car_index;
        }
    }
    if (gPending_race < 0) {
        gPending_race = PickNetRace(gProgram_state.current_race_index,
            gCurrent_net_game->options.race_sequence_type);
    }
    the_message->contents.start_race.next_race = (tU8)gPending_race;
    NetGuaranteedSendMessageToAllPlayers(gCurrent_net_game, the_message, NULL);
    if (gProgram_state.racing) {
        SendCurrentPowerups();
    }
}

// FUNCTION: CARMA2_HW 0x00499360
void C2_HOOK_FASTCALL DoNetworkHeadups(int pCredits) {
    char s[256];
    char s2[256];
    static tU32 last_flash;
    static int flash_state;

    if (gNot_shown_race_type_headup) {
        gNot_shown_race_type_headup = 0;
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(gCurrent_net_game->type + 59));
    }
    if (gTime_for_punishment != 0 && PDGetTotalTime() >= gTime_for_punishment) {
        gTime_for_punishment = 0;
        switch (gCurrent_net_game->type) {
        case 2:
            NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(219));
            break;
        case 3:
            NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(220));
            break;
        case 4:
            NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(221));
            break;
        case 7:
            NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(222));
            break;
        default:
            break;
        }
    }
    if (gNet_mode == eNet_mode_none
        || (float)gProgram_state.credits >= gRecovery_cost.initial_network[gCurrent_net_game->type]
        || Flash(200, &last_flash, &flash_state)) {
        sprintf(s, "\xf8%d\xfa %s", pCredits, GetMiscString(94));
        ChangeHeadupText(gNet_cash_headup, s);
    } else {
        ChangeHeadupText(gNet_cash_headup, "");
    }
    switch (gCurrent_net_game->type) {
    case 1:
        sprintf(s, "\xf8%d\xfa %s", gCurrent_net_game->options.starting_target, GetMiscString(241));
        break;
    case 2:
        sprintf(s, "%s \xf8%d\xfa", GetMiscString(180), gINT_0068d920);
        break;
    case 5:
        sprintf(s, "%s \xf8%d\xfa", GetMiscString(181), gCurrent_net_game->options.starting_target);
        break;
    case 7:
        TimerString(gCurrent_net_game->options.starting_target, s2, 1, 1, 0);
        sprintf(s, "%s \xf8%s\xfa", GetMiscString(182), s2);
        break;
    default:
        s[0] = '\0';
        break;
    }
    ChangeHeadupText(gNet_ped_headup, s);
    switch (gHeadup_detail_level) {
    case 0:
    case 3:
        MoveHeadupTo(gNet_cash_headup, 0x50, 0x19);
        MoveHeadupTo(gNet_ped_headup, 0x50, 0);
        break;
    case 1:
    case 4:
        MoveHeadupTo(gNet_cash_headup, 0x1e0, 0x19);
        MoveHeadupTo(gNet_ped_headup, 0x1e0, 0);
        break;
    case 2:
    case 5:
        MoveHeadupTo(gNet_cash_headup, 0x1ae, 0x3c);
        MoveHeadupTo(gNet_ped_headup, 0x1ae, 0x14);
        break;
    default:
        break;
    }
}

// FUNCTION: CARMA2_HW 0x00497900
void C2_NAKED C2_HOOK_FASTCALL SendCarData(tU32 pNext_frame_time) {
#ifndef CARPOCALYPSE2_MATCHING

    if (gNet_mode == eNet_mode_none) {
        return;
    }
    NOT_IMPLEMENTED();
#else
    __asm {
            sub esp, 0x268
            mov eax, dword ptr [gNet_mode]
            push ebx
            push ebp
            push esi
            mov esi, ecx
            push edi
            test eax, eax
            mov dword ptr [esp + 0x20], esi
            je L00497ee0
            cmp eax, 2
            jne L00497941
            xor edx, edx
            mov cl, 0x12
            call NetStartBroadcastContents
            test eax, eax
            je L00497953
            mov ecx, dword ptr [gRace_start]
            mov dword ptr [eax + 4], ecx
            mov ecx, eax
            call NetBroadcastContents
            jmp L00497953
    L00497941:
            xor edx, edx
            mov cl, 0x12
            call NetGetToHostContents
            mov edx, dword ptr [gRace_start]
            mov dword ptr [eax + 4], edx
    L00497953:
            mov eax, dword ptr [gNet_mode]
            cmp eax, 2
            jne L00497ed4
            mov eax, dword ptr [gNumber_of_net_players]
            mov dword ptr [esp + 0x1c], 0
            test eax, eax
            jle L00497e73
            mov edi, offset gNet_players+0xc8
            mov dword ptr [esp + 0x18], edi
    L0049797f:
            mov ebp, dword ptr [edi]
            mov dword ptr [esp + 0x28], ebp
            mov eax, dword ptr [ebp + 4]
            test eax, eax
            jne L00497e4e
            mov ecx, dword ptr [ebp + 8]
            mov byte ptr [esp + 0x13], 0
            call GetHierarchyNetworkSize
            mov edx, dword ptr [ebp + 0x640]
            mov byte ptr [esp + 0x2c], al
            mov ebx, dword ptr [esp + 0x2c]
            mov ecx, 0x1e
            and ebx, 0xff
            cmp edx, ecx
            lea eax, [ebx + 0x34]
            jg L004979d5
            cmp dword ptr [ebp + 0x66c], ecx
            jg L004979d5
            cmp dword ptr [ebp + 0x698], ecx
            jg L004979d5
            cmp dword ptr [ebp + 0x6c4], ecx
            jle L004979dd
    L004979d5:
            mov byte ptr [esp + 0x13], 8
            add eax, 0x10
    L004979dd:
            mov byte ptr [esp + 0x3c], al
            mov eax, dword ptr [edi - 0xa4]
            mov byte ptr [esp + 0x3d], 0x10
            mov dword ptr [esp + 0x40], eax
            mov dword ptr [esp + 0x44], esi
            fld dword ptr [ebp + 0x1254]
            fdiv dword ptr [ebp + 0x1258]
            fmul dword ptr [gFLOAT_0058a3d0]
            call _ftol
            mov word ptr [esp + 0x54], ax
            mov eax, dword ptr [ebp + 0x12d0]
            mov dword ptr [esp + 0x4c], eax
            mov ecx, dword ptr [ebp + 0x12dc]
            sar ecx, 9
            and al, 0
            and ecx, 0xff
            or eax, ecx
            mov dword ptr [esp + 0x4c], eax
            mov edx, dword ptr [ebp + 0x12e0]
            sar edx, 1
            and edx, 0xff00
            and ah, 0
            or edx, eax
            mov dword ptr [esp + 0x4c], edx
            fld dword ptr [ebp + 0x1340]
            call _ftol
            mov word ptr [esp + 0x56], ax
            xor esi, esi
            lea edi, [ebp + 0x1264]
    L00497a60:
            _emit 0xd9
        _emit 0x07
            mov eax, esi
            sar eax, 1
            fdiv dword ptr [ebp + eax*4 + 0x1218]
            fmul dword ptr [gFLOAT_0058a3d4]
            call _ftol
            mov byte ptr [esp + esi + 0x48], al
            inc esi
            add edi, 4
            cmp esi, 4
            jl L00497a60
            xor eax, eax
            lea ecx, [ebp + 0x4e0]
    L00497a8d:
            mov dl, byte ptr [ecx]
            add ecx, 0x2c
            mov byte ptr [esp + eax + 0x5c], dl
            inc eax
            cmp eax, 0xc
            jl L00497a8d
            mov al, byte ptr [ebp + 0x18a8]
            push 0x68
            mov byte ptr [esp + 0x6c], al
            mov cl, byte ptr [ebp + 0x18ac]
            mov byte ptr [esp + 0x6d], cl
            mov dl, byte ptr [ebp + 0x18b0]
            mov byte ptr [esp + 0x6e], dl
            mov eax, dword ptr [ebp + 0x18a4]
            mov dword ptr [esp + 0x5c], eax
            mov ecx, dword ptr [ebp + 8]
            mov edx, dword ptr [ecx + 0x49c]
            mov dword ptr [esp + 0x54], edx
            mov eax, dword ptr [ebp + 0x18d4]
            mov dl, byte ptr [esp + 0x17]
            mov cx, word ptr [eax + 0x24]
            mov byte ptr [esp + 0x72], dl
            mov word ptr [esp + 0x70], cx
            mov ecx, dword ptr [ebp + 8]
            lea edx, [esp + 0x74]
            call GetHierarchyNetworkStuff
            cmp eax, ebx
            je L00497b09
            push offset gScore_winner+0xc
            push 0x72
            call FatalError
            add esp, 8
    L00497b09:
            mov al, byte ptr [esp + 0x13]
            lea ecx, [esp + ebx + 0x70]
            test al, 8
            je L00497b2d
            lea eax, [ebp + 0x132c]
            mov edx, 4
    L00497b20:
            mov esi, dword ptr [eax]
            add eax, 4
            mov dword ptr [ecx], esi
            add ecx, 4
            dec edx
            jne L00497b20
    L00497b2d:
            mov ecx, dword ptr [gNumber_of_net_players]
            mov al, byte ptr [esp + 0x3c]
            mov edi, 1
            mov byte ptr [esp + 0x13], al
            cmp ecx, edi
            mov dword ptr [esp + 0x24], edi
            jle L00497e09
            mov dword ptr [esp + 0x14], 0x74bdb8
    L00497b54:
            mov ebx, dword ptr [esp + 0x1c]
            cmp ebx, edi
            jne L00497b73
            mov eax, dword ptr [ebp + 8]
            mov edx, dword ptr [eax + 0x49c]
            mov esi, dword ptr [eax + 0x274]
            cmp edx, esi
            je L00497de8
    L00497b73:
            mov eax, dword ptr [esp + 0x14]
            mov edx, dword ptr [eax + 0xac]
            test edx, edx
            je L00497b9d
            xor esi, esi
            dec ecx
            test ecx, ecx
            jle L00497b9f
            mov eax, offset gNet_players+0x24
    L00497b8d:
            cmp dword ptr [eax], edx
            je L00497b9f
            inc esi
            add eax, 0xd4
            cmp esi, ecx
            jl L00497b8d
            jmp L00497b9f
    L00497b9d:
            mov esi, edi
    L00497b9f:
            cmp ebx, edi
            mov ebx, dword ptr [esp + 0x18]
            je L00497c2d
            mov eax, dword ptr [ebx]
            lea ecx, [esi + esi*2]
            mov eax, dword ptr [eax + 0x10]
            lea ecx, [ecx + ecx*8]
            shl ecx, 1
            fld dword ptr [eax + 0x50]
            fld dword ptr [eax + 0x54]
            fxch st(1)
            sub ecx, esi
            mov ecx, dword ptr [ecx*4 + gNet_players+0xc8]
            mov ecx, dword ptr [ecx + 0x10]
            fsub dword ptr [ecx + 0x50]
            fxch st(1)
            fsub dword ptr [ecx + 0x54]
            fld dword ptr [eax + 0x58]
            fsub dword ptr [ecx + 0x58]
            fxch st(2)
            fst dword ptr [esp + 0x30]
            fxch st(1)
            fst dword ptr [esp + 0x34]
            fxch st(1)
            fmul dword ptr [esp + 0x30]
            fxch st(1)
            fmul dword ptr [esp + 0x34]
            fld st(2)
            fmul st(0), st(3)
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fsqrt
            fstp st(1)
            fcomp dword ptr [gFLOAT_0058a3d8]
            fnstsw ax
            test ah, 1
            jne L00497c2d
            mov edx, dword ptr [esp + 0x14]
            mov cl, 0x2d
            mov eax, dword ptr [edx]
            xor edx, edx
            push eax
            call NetGetToPlayerContents
            mov ecx, eax
            mov edx, dword ptr [esp + 0x1c]
            call CompressCarDataChunk
            jmp L00497de8
    L00497c2d:
            mov dl, byte ptr [esp + 0x6e]
            mov cl, byte ptr [esp + 0x13]
            mov edi, dword ptr [ebx]
            and dl, 0xf8
            mov byte ptr [esp + 0x3c], cl
            mov byte ptr [esp + 0x6e], dl
            mov eax, dword ptr [edi + 0x18d4]
            mov ecx, dword ptr [eax + 0x1c]
            test ecx, ecx
            je L00497d9f
            mov ecx, dword ptr [eax + 0x28]
            test ecx, ecx
            je L00497c9f
            mov cx, word ptr [eax + 0x24]
            mov dl, cl
            and dl, 1
            neg dl
            sbb edx, edx
            neg edx
            inc edx
            add edx, ecx
            mov word ptr [eax + 0x24], dx
            mov eax, dword ptr [edi + 0x18d4]
            mov ecx, dword ptr [eax + 0x20]
            call SetAllShapeStatusFlags
            mov ecx, dword ptr [edi + 0x18d4]
            mov edx, dword ptr [ebx - 0xa4]
            mov ecx, dword ptr [ecx + 0x20]
            call ClearShapeStatusFlag
            mov edx, dword ptr [edi + 0x18d4]
            mov dword ptr [edx + 0x28], 0
    L00497c9f:
            mov ecx, dword ptr [edi + 0x18d4]
            lea eax, [esi + esi*2]
            mov ecx, dword ptr [ecx + 0x20]
            lea eax, [eax + eax*8]
            shl eax, 1
            sub eax, esi
            mov edx, dword ptr [eax*4 + gNet_players+0x24]
            call CarShapeNeedsSendingToPlayer
            test eax, eax
            je L00497d9f
            mov ecx, edi
            mov bl, 1
            call FUN_0043d900
            mov ecx, dword ptr [edi + 0x18d4]
            mov ebp, eax
            mov dl, byte ptr [ecx + 0x144]
            test dl, dl
            je L00497ce6
            mov bl, 3
            lea eax, [ebp + 3]
    L00497ce6:
            mov dl, byte ptr [ecx + 0x4b8]
            test dl, dl
            je L00497cf6
            or bl, 4
            add eax, 8
    L00497cf6:
            mov edx, dword ptr [esp + 0x3c]
            mov cl, byte ptr [esp + 0x6e]
            and edx, 0xff
            or cl, bl
            mov byte ptr [esp + 0x6e], cl
            lea esi, [esp + edx + 0x3c]
            mov dl, byte ptr [esp + 0x3c]
            add dl, al
            test bl, 1
            mov byte ptr [esp + 0x3c], dl
            je L00497d28
            mov edx, edi
            mov ecx, esi
            call EncodeNetShapeData
            add esi, ebp
    L00497d28:
            test bl, 2
            je L00497d83
            mov ecx, dword ptr [edi + 0x18d4]
            xor dl, dl
            fld dword ptr [ecx + 0x174]
            fcomp dword ptr [ecx + 0x14c]
            fnstsw ax
            test ah, 0x40
            je L00497d4a
            mov dl, 1
    L00497d4a:
            mov ax, word ptr [ecx + 0x190]
            test ax, ax
            je L00497d5d
            or dl, 2
            mov ecx, eax
            jmp L00497d7a
    L00497d5d:
            mov ax, word ptr [ecx + 0x192]
            test ax, ax
            je L00497d70
            or dl, 4
            mov ecx, eax
            jmp L00497d7a
    L00497d70:
            mov cx, word ptr [ecx + 0x194]
            or dl, 8
    L00497d7a:
            mov byte ptr [esi], dl
            inc esi
            mov word ptr [esi], cx
            add esi, 2
    L00497d83:
            test bl, 4
            je L00497d9b
            mov edx, dword ptr [edi + 0x18d4]
            mov ecx, esi
            add edx, 0x548
            call CompressPlane
    L00497d9b:
            mov ebp, dword ptr [esp + 0x28]
    L00497d9f:
            mov al, byte ptr [esp + 0x3c]
            mov edx, dword ptr [esp + 0x14]
            mov cl, al
            and cl, 1
            add al, cl
            mov cl, 0x10
            mov byte ptr [esp + 0x3c], al
            mov eax, dword ptr [edx]
            mov edx, dword ptr [esp + 0x3c]
            push eax
            and edx, 0xff
            call NetGetToPlayerContents
            lea esi, [esp + 0x3c]
            mov edi, eax
            mov ecx, dword ptr [esp + 0x3c]
            and ecx, 0xff
            mov edx, ecx
            shr ecx, 2
            rep movsd
            mov ecx, edx
            and ecx, 3
            rep movsb
            mov edi, dword ptr [esp + 0x24]
    L00497de8:
            mov ecx, dword ptr [esp + 0x14]
            inc edi
            add ecx, 0xd4
            mov dword ptr [esp + 0x24], edi
            mov dword ptr [esp + 0x14], ecx
            mov ecx, dword ptr [gNumber_of_net_players]
            cmp edi, ecx
            jl L00497b54
    L00497e09:
            mov eax, dword ptr [ebp + 0x18a0]
            test eax, eax
            je L00497e4a
            mov ecx, dword ptr [esp + 0x20]
            add eax, 0xfffffe0c
            cmp eax, ecx
            jae L00497e4a
            xor edx, edx
            mov cl, 0x17
            call NetStartBroadcastContents
            test eax, eax
            je L00497e4a
            mov ecx, dword ptr [esp + 0x18]
            mov edx, dword ptr [ecx - 0xa4]
            mov dword ptr [eax + 4], edx
            mov ecx, dword ptr [ebp + 0x18a0]
            mov dword ptr [eax + 8], ecx
            mov ecx, eax
            call NetBroadcastContents
    L00497e4a:
            mov esi, dword ptr [esp + 0x20]
    L00497e4e:
            mov eax, dword ptr [esp + 0x1c]
            mov edi, dword ptr [esp + 0x18]
            mov ecx, dword ptr [gNumber_of_net_players]
            inc eax
            add edi, 0xd4
            cmp eax, ecx
            mov dword ptr [esp + 0x1c], eax
            mov dword ptr [esp + 0x18], edi
            jl L0049797f
    L00497e73:
            mov eax, dword ptr [gNum_active_non_cars]
            xor ebx, ebx
            test eax, eax
            jle L00497ee0
            mov edi, offset gActive_non_car_list
            mov ebp, 0x100000
    L00497e88:
            mov ecx, dword ptr [edi]
            test dword ptr [ecx + 0x100], ebp
            je L00497ebc
            mov eax, dword ptr [ecx + 0xc]
            cmp eax, 2
            jne L00497ea5
            push 0
            mov edx, esi
            call SendDetachedNonCarData
            jmp L00497ebc
    L00497ea5:
            cmp eax, 3
            jne L00497eb5
            push 0
            mov edx, esi
            call SendSplitHalfNonCarData
            jmp L00497ebc
    L00497eb5:
            mov edx, esi
            call SendNonCar
    L00497ebc:
            mov eax, dword ptr [gNum_active_non_cars]
            inc ebx
            add edi, 4
            cmp ebx, eax
            jl L00497e88
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x268
            ret
    L00497ed4:
            cmp eax, 3
            jne L00497ee0
            mov ecx, esi
            call SendLocalCarData
    L00497ee0:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x268
            ret
    }
#endif
}
// SendShapeNumbers

// FUNCTION: CARMA2_HW 0x00497ef0
void C2_HOOK_FASTCALL ClearShapeStatusFlag(tCar_crush_network_shapes* pShapes, int pPlayer_id) {
    int i;

    if (pShapes == NULL) {
        return;
    }
    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(pShapes->items); i++) {
        if (pShapes->items[i].network_id == pPlayer_id) {
            pShapes->items[i].field_0x4 = 0;
            return;
        }
    }
}

// FUNCTION: CARMA2_HW 0x00497f10
void C2_NAKED C2_HOOK_FASTCALL SendNonCar(tNon_car_spec* pNon_car, tU32 pPlayer_id) {
#ifndef CARPOCALYPSE2_MATCHING
    tNet_message_chunk* message;
    int hierarchy_size;
    int written_size;
    int raw_flags;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_non_car, non_car_id, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_non_car, player_id, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_non_car, flags, 0x10);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_non_car, object_hierarchy, 0x12);

    hierarchy_size = GetHierarchyNetworkSize(pNon_car->collision_info);
    message = NetStartBroadcastContents(eNet_message_chunk_type_non_car, hierarchy_size + 0x12);
    if (message != NULL) {
        message->non_car.non_car_id = (tU16)pNon_car->car_ID;
        message->non_car.player_id = pPlayer_id;
        raw_flags = pNon_car->flags;
        message->non_car.flags = (tU16)(((raw_flags & 0xff00) != 0) * 8
                | ((char)raw_flags != '\0') * 4
                | pNon_car->collision_info->disable_move_rotate * 2
                | (pNon_car->actor->identifier[3] == '!'));
        written_size = GetHierarchyNetworkStuff(pNon_car->collision_info, message->non_car.object_hierarchy, 0x48);
        if (written_size != hierarchy_size) {
            FatalError(kFatalError_NetContentsTooBig_S, "in non_car object data");
        }
        NetBroadcastContents(message);
    }
#else
    __asm {
            push ebx
            push ebp
            push esi
            mov esi, ecx
            push edi
            mov ebp, edx
            mov ecx, dword ptr [esi + 8]
            call GetHierarchyNetworkSize
            mov ebx, eax
            mov cl, 0x11
            lea edx, [ebx + 0x12]
            call NetStartBroadcastContents
            mov edi, eax
            test edi, edi
            je L00497fac
            xor eax, eax
            push 0x48
            mov ax, word ptr [esi + 0x80]
            mov dword ptr [edi + 8], ebp
            mov dword ptr [edi + 4], eax
            mov eax, dword ptr [esi + 0x100]
            mov ecx, eax
            mov edx, dword ptr [esi + 8]
            and ecx, 0xff00
            neg ecx
            sbb ecx, ecx
            neg ecx
            shl ecx, 1
            neg al
            sbb eax, eax
            neg eax
            or ecx, eax
            movzx ax, byte ptr [edx + 0xec]
            mov edx, dword ptr [esi + 0x10]
            shl ecx, 1
            or ecx, eax
            mov eax, dword ptr [edx + 0x14]
            shl ecx, 1
            xor edx, edx
            cmp byte ptr [eax + 3], 0x21
            sete dl
            or ecx, edx
            lea edx, [edi + 0x12]
            mov word ptr [edi + 0x10], cx
            mov ecx, dword ptr [esi + 8]
            call GetHierarchyNetworkStuff
            cmp eax, ebx
            je L00497fa5
            push offset gScore_winner+0x20
            push 0x72
            call FatalError
            add esp, 8
    L00497fa5:
            mov ecx, edi
            call NetBroadcastContents
    L00497fac:
            pop edi
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
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00497fc0
void C2_NAKED C2_HOOK_FASTCALL SendLocalCarData(tU32 pNext_frame_time) {
#ifndef CARPOCALYPSE2_MATCHING
    tNet_message_chunk_car_data* message;
    tNet_message_chunk_recover* recover_message;
    tNet_message_chunk_car_crush_sync* crush_sync_message;
    tU8 flags;
    tU8 hierarchy_size;
    int message_size;
    int crush_size;
    int i;
    tU8* pData;

    if (gProgram_state.current_car.disabled != 0) {
        return;
    }

    flags = 0;
    hierarchy_size = (tU8)GetHierarchyNetworkSize(gProgram_state.current_car.collision_info);
    message_size = hierarchy_size + 0x34;
    if (gProgram_state.current_car.damage_units[8].damage_level > 0x1e
            || gProgram_state.current_car.damage_units[9].damage_level > 0x1e
            || gProgram_state.current_car.damage_units[10].damage_level > 0x1e
            || gProgram_state.current_car.damage_units[11].damage_level > 0x1e) {
        flags = 8;
        message_size += 0x10;
    }

    if (gProgram_state.current_car.car_crush_spec->version_le_100 != 0) {
        if (gProgram_state.current_car.car_crush_spec->field_0x28 != 0) {
            gProgram_state.current_car.car_crush_spec->field_0x24 = (tU16)(gProgram_state.current_car.car_crush_spec->field_0x24 + 2 - ((gProgram_state.current_car.car_crush_spec->field_0x24 & 1) != 0));
            gProgram_state.current_car.car_crush_spec->field_0x28 = 0;
        }
        if (gProgram_state.current_car.car_crush_spec->field_0x24 != gProgram_state.current_car.car_crush_spec->field_0x26) {
            crush_size = FUN_0043d900(&gProgram_state.current_car);
            message_size += crush_size;
            flags |= 1;
            if (gProgram_state.current_car.car_crush_spec->field_0x144 != 0) {
                message_size += 3;
                flags |= 2;
            }
            if (gProgram_state.current_car.car_crush_spec->field_0x4b8 != 0) {
                message_size += 8;
                flags |= 4;
            }
        }
    }

    message = (tNet_message_chunk_car_data*)NetGetToHostContents(eNet_message_chunk_type_car_data, message_size);
    message->next_frame_time = pNext_frame_time;
    message->player_id = gNet_players[gThis_net_player_index].ID;
    message->curvature = (tU16)(gProgram_state.current_car.curvature / gProgram_state.current_car.maxcurve * gFLOAT_0058a3d0);
    message->controls = *(tU32*)&gProgram_state.current_car.keys;
    message->controls = (message->controls & 0xffffff00U) | ((gProgram_state.current_car.joystick.acc >> 9) & 0xff);
    message->controls = (message->controls & 0xffff00ffU) | ((gProgram_state.current_car.joystick.dec >> 1) & 0xff00);
    message->revs = (tU16)gProgram_state.current_car.revs;
    message->hierarchy_flags = gProgram_state.current_car.collision_info->field_0x49c;
    for (i = 0; i < 4; i++) {
        message->suspension_compression[i] = (tU8)(gProgram_state.current_car.oldd[i] / gProgram_state.current_car.susp_height[i >> 1] * gFLOAT_0058a3d4);
    }
    for (i = 0; i < 12; i++) {
        message->damage_levels[i] = (tU8)gProgram_state.current_car.damage_units[i].damage_level;
    }
    message->power_up_levels[0] = (tU8)gProgram_state.current_car.power_up_levels[0];
    message->power_up_levels[1] = (tU8)gProgram_state.current_car.power_up_levels[1];
    message->power_up_levels[2] = (tU8)gProgram_state.current_car.power_up_levels[2];
    message->repair_time = gProgram_state.current_car.repair_time;
    message->crush_state = gProgram_state.current_car.car_crush_spec->field_0x24;
    message->flags = flags;
    if (GetHierarchyNetworkStuff(gProgram_state.current_car.collision_info, message->object_hierarchy, 0x68) != hierarchy_size) {
        FatalError(0x72, "in car object data");
    }

    pData = message->object_hierarchy + hierarchy_size;
    if ((flags & 8) != 0) {
        for (i = 0; i < 4; i++) {
            *(float*)pData = gProgram_state.current_car.wheel_dam_offset[i];
            pData += sizeof(float);
        }
    }
    if ((flags & 1) != 0) {
        EncodeNetShapeData(pData, &gProgram_state.current_car);
        pData += crush_size;
    }
    if ((flags & 2) != 0) {
        tU8 deformation_flags;
        tU16 deformation_value;

        deformation_flags = 0;
        if (gProgram_state.current_car.car_crush_spec->field_0x174 == gProgram_state.current_car.car_crush_spec->bend_z_min) {
            deformation_flags |= 1;
        }
        if (gProgram_state.current_car.car_crush_spec->field_0x190 != 0) {
            deformation_flags |= 2;
            deformation_value = gProgram_state.current_car.car_crush_spec->field_0x190;
        } else if (gProgram_state.current_car.car_crush_spec->field_0x192 != 0) {
            deformation_flags |= 4;
            deformation_value = gProgram_state.current_car.car_crush_spec->field_0x192;
        } else {
            deformation_flags |= 8;
            deformation_value = gProgram_state.current_car.car_crush_spec->field_0x194;
        }
        *pData++ = deformation_flags;
        *(tU16*)pData = deformation_value;
        pData += sizeof(tU16);
    }
    if ((flags & 4) != 0) {
        CompressPlane((tU16*)pData, &gProgram_state.current_car.car_crush_spec->field_0x548.v[0]);
    }

    if (gProgram_state.current_car.time_to_recover != 0 && gProgram_state.current_car.time_to_recover - 500U < pNext_frame_time) {
        recover_message = (tNet_message_chunk_recover*)NetGetToHostContents(eNet_message_chunk_type_car_recover, 0);
        recover_message->player = gNet_players[gThis_net_player_index].ID;
        recover_message->time = gProgram_state.current_car.time_to_recover;
    }

    crush_sync_message = (tNet_message_chunk_car_crush_sync*)NetGetToHostContents(
        eNet_message_chunk_type_car_crush_sync, gNumber_of_net_players * 8 + 4);
    crush_sync_message->player_id = gNet_players[gThis_net_player_index].ID;
    crush_sync_message->count = (tU8)(gNumber_of_net_players - 1);
    for (i = 0; i < gNumber_of_net_players; i++) {
        if (i != gThis_net_player_index) {
            crush_sync_message->players[i].player = gNet_players[i].ID;
            crush_sync_message->players[i].crush_state = gNet_players[i].car->car_crush_spec->field_0x24;
        }
    }
#else
    __asm {
            sub esp, 0xc
            mov eax, dword ptr [gProgram_state+0xb0]
            push ebx
            push ebp
            push esi
            test eax, eax
            push edi
            mov dword ptr [esp + 0x14], ecx
            jne L00498360
            mov ecx, dword ptr [gProgram_state+0xb4]
            mov byte ptr [esp + 0x13], 0
            call GetHierarchyNetworkSize
            mov ecx, dword ptr [gProgram_state+0x6ec]
            mov byte ptr [esp + 0x18], al
            mov ebx, dword ptr [esp + 0x18]
            mov eax, 0x1e
            and ebx, 0xff
            cmp ecx, eax
            lea esi, [ebx + 0x34]
            jg L00498020
            cmp dword ptr [gProgram_state+0x718], eax
            jg L00498020
            cmp dword ptr [gProgram_state+0x744], eax
            jg L00498020
            cmp dword ptr [gProgram_state+0x770], eax
            jle L00498028
    L00498020:
            mov byte ptr [esp + 0x13], 8
            add esi, 0x10
    L00498028:
            mov eax, dword ptr [gProgram_state+0x1980]
            mov ecx, dword ptr [eax + 0x1c]
            test ecx, ecx
            je L004980bf
            mov ecx, dword ptr [eax + 0x28]
            test ecx, ecx
            je L00498066
            mov cx, word ptr [eax + 0x24]
            mov dl, cl
            and dl, 1
            neg dl
            sbb edx, edx
            add edx, 2
            add edx, ecx
            mov word ptr [eax + 0x24], dx
            mov eax, dword ptr [gProgram_state+0x1980]
            mov dword ptr [eax + 0x28], 0
            mov eax, dword ptr [gProgram_state+0x1980]
    L00498066:
            mov cx, word ptr [eax + 0x24]
            cmp cx, word ptr [eax + 0x26]
            je L004980bf
            mov al, byte ptr [esp + 0x13]
            mov ecx, offset gProgram_state+0xac
            or al, 1
            mov byte ptr [esp + 0x13], al
            call FUN_0043d900
            mov dword ptr [esp + 0x18], eax
            add esi, eax
            mov eax, dword ptr [gProgram_state+0x1980]
            mov cl, byte ptr [eax + 0x144]
            test cl, cl
            je L004980a7
            mov dl, byte ptr [esp + 0x13]
            or dl, 2
            add esi, 3
            mov byte ptr [esp + 0x13], dl
    L004980a7:
            mov cl, byte ptr [eax + 0x4b8]
            test cl, cl
            je L004980bf
            mov cl, byte ptr [esp + 0x13]
            or cl, 4
            add esi, 8
            mov byte ptr [esp + 0x13], cl
    L004980bf:
            mov edx, esi
            mov cl, 0x10
            call NetGetToHostContents
            mov esi, eax
            mov eax, dword ptr [gThis_net_player_index]
            lea ecx, [eax + eax*2]
            lea edx, [ecx + ecx*8]
            mov ecx, dword ptr [esp + 0x14]
            shl edx, 1
            sub edx, eax
            mov eax, dword ptr [edx*4 + gNet_players+0x24]
            mov dword ptr [esi + 8], ecx
            mov dword ptr [esi + 4], eax
            fld dword ptr [gProgram_state+0x1300]
            fdiv dword ptr [gProgram_state+0x1304]
            fmul dword ptr [gFLOAT_0058a3d0]
            call _ftol
            mov word ptr [esi + 0x18], ax
            mov ecx, dword ptr [gProgram_state+0x137c]
            mov dword ptr [esi + 0x10], ecx
            mov eax, dword ptr [gProgram_state+0x1388]
            sar eax, 9
            and eax, 0xff
            and cl, 0
            or eax, ecx
            mov dword ptr [esi + 0x10], eax
            mov edx, dword ptr [gProgram_state+0x138c]
            sar edx, 1
            and ah, 0
            and edx, 0xff00
            or eax, edx
            mov dword ptr [esi + 0x10], eax
            fld dword ptr [gProgram_state+0x13ec]
            call _ftol
            mov word ptr [esi + 0x1a], ax
            mov eax, dword ptr [gProgram_state+0xb4]
            xor edi, edi
            mov ebp, offset gProgram_state+0x1310
            mov ecx, dword ptr [eax + 0x49c]
            mov dword ptr [esi + 0x14], ecx
    L0049815d:
            _emit 0xd9
        _emit 0x45
        _emit 0x00
            mov edx, edi
            sar edx, 1
            fdiv dword ptr [edx*4 + gProgram_state+0x12c4]
            fmul dword ptr [gFLOAT_0058a3d4]
            call _ftol
            mov byte ptr [edi + esi + 0xc], al
            add ebp, 4
            inc edi
            cmp ebp, offset gProgram_state+0x1320
            jl L0049815d
            lea ecx, [esi + 0x20]
            mov eax, offset gProgram_state+0x58c
    L0049818e:
            mov dl, byte ptr [eax]
            add eax, 0x2c
            mov byte ptr [ecx], dl
            inc ecx
            cmp eax, offset gProgram_state+0x79c
            jl L0049818e
            mov al, byte ptr [gProgram_state+0x1954]
            push 0x68
            mov byte ptr [esi + 0x2c], al
            mov cl, byte ptr [gProgram_state+0x1958]
            mov byte ptr [esi + 0x2d], cl
            mov dl, byte ptr [gProgram_state+0x195c]
            mov byte ptr [esi + 0x2e], dl
            mov eax, dword ptr [gProgram_state+0x1950]
            mov dword ptr [esi + 0x1c], eax
            mov ecx, dword ptr [gProgram_state+0x1980]
            mov al, byte ptr [esp + 0x17]
            add esi, 0x34
            mov dx, word ptr [ecx + 0x24]
            mov byte ptr [esi - 2], al
            mov word ptr [esi - 4], dx
            mov ecx, dword ptr [gProgram_state+0xb4]
            mov edx, esi
            call GetHierarchyNetworkStuff
            cmp eax, ebx
            je L004981f9
            push offset gScore_winner+0xc
            push 0x72
            call FatalError
            add esp, 8
    L004981f9:
            add esi, ebx
            mov bl, byte ptr [esp + 0x13]
            test bl, 8
            je L0049821a
            mov eax, offset gProgram_state+0x13d8
    L00498209:
            mov ecx, dword ptr [eax]
            add eax, 4
            mov dword ptr [esi], ecx
            add esi, 4
            cmp eax, offset gProgram_state+0x13e8
            jl L00498209
    L0049821a:
            test bl, 1
            je L0049822f
            mov edx, offset gProgram_state+0xac
            mov ecx, esi
            call EncodeNetShapeData
            add esi, dword ptr [esp + 0x18]
    L0049822f:
            test bl, 2
            je L00498286
            mov edx, dword ptr [gProgram_state+0x1980]
            xor cl, cl
            fld dword ptr [edx + 0x174]
            fcomp dword ptr [edx + 0x14c]
            fnstsw ax
            test ah, 0x40
            je L00498251
            mov cl, 1
    L00498251:
            mov ax, word ptr [edx + 0x190]
            test ax, ax
            je L00498262
            or cl, 2
            jmp L0049827d
    L00498262:
            mov ax, word ptr [edx + 0x192]
            test ax, ax
            je L00498273
            or cl, 4
            jmp L0049827d
    L00498273:
            mov ax, word ptr [edx + 0x194]
            or cl, 8
    L0049827d:
            mov byte ptr [esi], cl
            inc esi
            mov word ptr [esi], ax
            add esi, 2
    L00498286:
            test bl, 4
            je L0049829e
            mov edx, dword ptr [gProgram_state+0x1980]
            mov ecx, esi
            add edx, 0x548
            call CompressPlane
    L0049829e:
            mov eax, dword ptr [gProgram_state+0x194c]
            test eax, eax
            je L004982e0
            mov ecx, dword ptr [esp + 0x14]
            add eax, 0xfffffe0c
            cmp eax, ecx
            jae L004982e0
            xor edx, edx
            mov cl, 0x17
            call NetGetToHostContents
            mov ecx, dword ptr [gThis_net_player_index]
            lea edx, [ecx + ecx*2]
            lea edx, [edx + edx*8]
            shl edx, 1
            sub edx, ecx
            mov ecx, dword ptr [edx*4 + gNet_players+0x24]
            mov dword ptr [eax + 4], ecx
            mov edx, dword ptr [gProgram_state+0x194c]
            mov dword ptr [eax + 8], edx
    L004982e0:
            mov eax, dword ptr [gNumber_of_net_players]
            mov cl, 0x2a
            lea edx, [eax*8 + 4]
            call NetGetToHostContents
            mov ecx, dword ptr [gThis_net_player_index]
            lea edx, [ecx + ecx*2]
            lea edx, [edx + edx*8]
            shl edx, 1
            sub edx, ecx
            mov ecx, dword ptr [edx*4 + gNet_players+0x24]
            mov dword ptr [eax + 4], ecx
            mov dl, byte ptr [gNumber_of_net_players]
            dec dl
            mov byte ptr [eax + 8], dl
            mov ecx, dword ptr [gNumber_of_net_players]
            xor edx, edx
            test ecx, ecx
            jle L00498360
            mov ecx, offset gNet_players+0xc8
            add eax, 0x10
    L0049832c:
            cmp edx, dword ptr [gThis_net_player_index]
            je L0049834c
            mov esi, dword ptr [ecx - 0xa4]
            mov dword ptr [eax - 4], esi
            mov esi, dword ptr [ecx]
            mov esi, dword ptr [esi + 0x18d4]
            mov si, word ptr [esi + 0x24]
            mov word ptr [eax], si
    L0049834c:
            mov esi, dword ptr [gNumber_of_net_players]
            inc edx
            add eax, 8
            add ecx, 0xd4
            cmp edx, esi
            jl L0049832c
    L00498360:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0xc
            ret
    }
#endif
}

// HostFillInCarContents

// ClientSendCarData

// DistanceBetweenCars

// FUNCTION: CARMA2_HW 0x00498370
int C2_HOOK_FASTCALL CarShapeNeedsSendingToPlayer(tCar_crush_network_shapes* pShapes, int pPlayer_id) {
    int i;

    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(pShapes->items); i++) {
        if (pShapes->items[i].network_id == pPlayer_id) {
            return (tS8)pShapes->items[i].field_0x4;
        }
    }
    return 0;
}

// AddShapeStuffToMechanicsMessage

// TimeToSendData

// SendCarData

// ReceivedRecover

// CopyMechanics

// ReceivedShapeNumbers

// FillInSimpleCarPos

// ReceivedSimpleCarPos

// ReceivedMechanics

// FUNCTION: CARMA2_HW 0x00498bb0
void C2_HOOK_FASTCALL ReceivedCopInfo(void) {
}

// SendAllNonCarPositions

// FUNCTION: CARMA2_HW 0x00498bc0
void C2_HOOK_FASTCALL ReceivedNonCarPosition(tNet_message_chunk_non_car_position* pContents) {
    br_actor* actor;

    actor = gProgram_state.track_spec.non_car_list[pContents->ID];
    if (actor != NULL && gNet_mode != eNet_mode_host) {
        BrMatrix34Copy(&actor->t.t.mat, &pContents->mat);
        if (pContents->flags != 0) {
            actor->identifier[3] = '!';
        }
    }
}

// FUNCTION: CARMA2_HW 0x00498c00
void C2_NAKED C2_HOOK_FASTCALL ReceivedNonCar(tNet_message_chunk_non_car* pContents) {
#ifndef CARPOCALYPSE2_MATCHING
    br_actor* actor;
    tNon_car_spec* non_car;
    tNon_car_spec* candidate_non_car;
    tTrack_square* column;
    int non_car_index;
    tS8 non_car_spec_index;
    tU8 column_x;
    tU8 column_z;
    float dx;
    float dy;
    float dz;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_non_car, non_car_id, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_non_car, player_id, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_non_car, flags, 0x10);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_non_car, object_hierarchy, 0x12);

    if (pContents->non_car_id >= (tU32)gProgram_state.track_spec.count_non_cars) {
        return;
    }
    actor = gProgram_state.track_spec.non_car_list[pContents->non_car_id];
    if (actor == NULL) {
        return;
    }

    non_car = (tNon_car_spec*)actor->type_data;
    if (non_car != NULL && (non_car->driver != eDriver_4 || (tU16)non_car->car_ID != pContents->non_car_id)) {
        non_car = NULL;
    }
    if ((pContents->flags & 1) != 0) {
        actor->identifier[3] = '!';
    } else {
        actor->identifier[3] = 'x';
    }
    if (non_car == NULL) {
        if (actor->identifier[1] >= '0' && actor->identifier[1] <= '9') {
            candidate_non_car = NULL;
            non_car_index = actor->identifier[1] * 10 + actor->identifier[2] - 0x210;
            non_car_spec_index = (tS8)gNon_car_spec_indices[non_car_index];
            if (non_car_spec_index != 0) {
                candidate_non_car = &gProgram_state.non_cars[non_car_spec_index + 0x27];
            }
            dx = gProgram_state.current_car.car_master_actor->t.t.translate.t.v[0] - actor->t.t.translate.t.v[0];
            dy = gProgram_state.current_car.car_master_actor->t.t.translate.t.v[1] - actor->t.t.translate.t.v[1];
            dz = gProgram_state.current_car.car_master_actor->t.t.translate.t.v[2] - actor->t.t.translate.t.v[2];
            if ((candidate_non_car != NULL && (candidate_non_car->flags & 0x10000) != 0)
                    || dx * dx + dy * dy + dz * dz < gNon_car_pull_actor_distance_squared) {
                DoPullActorFromWorld(actor);
                non_car = (tNon_car_spec*)actor->type_data;
            }
        }
        if (non_car == NULL) {
            GetSingleMatrixFromNetworkData(pContents->object_hierarchy, &actor->t.t.mat);
            XZToColumnXZ(&column_x, &column_z,
                    actor->t.t.translate.t.v[0], actor->t.t.translate.t.v[2], &gProgram_state.track_spec);
            column = gProgram_state.track_spec.columns[column_z];
            if (column[column_x].actor_0x0 == actor->parent) {
                return;
            }
            if (column[column_x].actor_0x0 == NULL) {
                return;
            }
            BrActorRemove(actor);
            BrActorAdd(gProgram_state.track_spec.columns[column_z][column_x].actor_0x0, actor);
            return;
        }
    }

    if ((pContents->flags & 4) != 0) {
        if ((tU8)non_car->flags == 0) {
            non_car->flags |= 1;
        }
    } else {
        non_car->flags &= ~0xff;
    }
    if ((pContents->flags & 8) != 0) {
        if ((non_car->flags & 0xff00) == 0) {
            non_car->flags |= 0x100;
        }
    } else {
        non_car->flags &= ~0xff00;
    }

    if ((pContents->flags & 2) != 0) {
        SelectCrushNetwork((undefined4*)pContents->object_hierarchy, non_car->collision_info, 2);
        non_car->collision_info->disable_move_rotate = 1;
    } else {
        memcpy(&non_car->collision_info->field_0x294, pContents->object_hierarchy,
                GetNetworkDataSize(pContents->object_hierarchy));
        non_car->collision_info->message_time = pContents->player_id;
        non_car->collision_info->field_0x261 = 0x11;
        non_car->collision_info->disable_move_rotate = 0;
    }
#else
    __asm {
            sub esp, 8
            push ebx
            mov ebx, ecx
            mov ecx, dword ptr [gProgram_state+0x1bbc]
            push ebp
            mov eax, dword ptr [ebx + 4]
            push esi
            cmp eax, ecx
            push edi
            jae L00498e68
            mov ecx, dword ptr [gProgram_state+0x1bc0]
            mov esi, dword ptr [ecx + eax*4]
            test esi, esi
            je L00498e68
            mov ebp, dword ptr [esi + 0x5c]
            test ebp, ebp
            je L00498c47
            cmp dword ptr [ebp + 0xc], 4
            jne L00498c45
            xor edx, edx
            mov dx, word ptr [ebp + 0x80]
            cmp edx, eax
            je L00498c47
    L00498c45:
            xor ebp, ebp
    L00498c47:
            test byte ptr [ebx + 0x10], 1
            je L00498c56
            mov eax, dword ptr [esi + 0x14]
            mov byte ptr [eax + 3], 0x21
            jmp L00498c5d
    L00498c56:
            mov ecx, dword ptr [esi + 0x14]
            mov byte ptr [ecx + 3], 0x78
    L00498c5d:
            test ebp, ebp
            jne L00498dc0
            mov ecx, dword ptr [esi + 0x14]
            mov al, byte ptr [ecx + 1]
            cmp al, 0x30
            jl L00498d22
            cmp al, 0x39
            jg L00498d22
            movsx eax, al
            lea edx, [eax + eax*4]
            movsx eax, byte ptr [ecx + 2]
            lea eax, [eax + edx*2 - 0x210]
            mov al, byte ptr [eax + gNon_car_spec_indices]
            test al, al
            je L00498cae
            movsx eax, al
            mov edx, dword ptr [gProgram_state+0x5ec8]
            add eax, 0x27
            mov ecx, eax
            shl ecx, 6
            add ecx, eax
            lea ecx, [edx + ecx*4]
            jmp L00498cb0
    L00498cae:
            xor ecx, ecx
    L00498cb0:
            mov eax, dword ptr [gProgram_state+0xbc]
            test ecx, ecx
            fld dword ptr [eax + 0x50]
            fld dword ptr [eax + 0x54]
            fld dword ptr [eax + 0x58]
            fxch st(2)
            fsub dword ptr [esi + 0x50]
            fxch st(1)
            fsub dword ptr [esi + 0x54]
            fxch st(2)
            fsub dword ptr [esi + 0x58]
            fxch st(1)
            fxch st(2)
            fxch st(1)
            je L00498ce3
            test dword ptr [ecx + 0x100], 0x10000
            jne L00498d12
    L00498ce3:
            fld st(2)
            fld st(2)
            fxch st(1)
            fmul st(0), st(4)
            fld st(2)
            fxch st(2)
            fmul st(0), st(4)
            fxch st(2)
            fmul st(0), st(3)
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fcomp dword ptr [gNon_car_pull_actor_distance_squared]
            fstp st(0)
            fnstsw ax
            fstp st(0)
            test ah, 1
            fstp st(0)
            je L00498d22
            jmp L00498d18
    L00498d12:
            fstp st(0)
            fstp st(0)
            fstp st(0)
    L00498d18:
            mov ecx, esi
            call DoPullActorFromWorld
            mov ebp, dword ptr [esi + 0x5c]
    L00498d22:
            test ebp, ebp
            jne L00498dc0
            lea edx, [esi + 0x2c]
            lea ecx, [ebx + 0x12]
            call GetSingleMatrixFromNetworkData
            mov eax, dword ptr [esi + 0x58]
            mov ecx, dword ptr [esi + 0x50]
            push offset gProgram_state+0x1ba0
            push eax
            push ecx
            lea edx, [esp + 0x1c]
            lea ecx, [esp + 0x20]
            call XZToColumnXZ
            mov ecx, dword ptr [gProgram_state+0x1bb8]
            mov edx, dword ptr [esp + 0x10]
            mov eax, dword ptr [esp + 0x14]
            and edx, 0xff
            and eax, 0xff
            mov edx, dword ptr [ecx + edx*4]
            mov ecx, dword ptr [esi + 0xc]
            lea eax, [eax + eax*2]
            mov eax, dword ptr [edx + eax*4]
            cmp eax, ecx
            je L00498e68
            test eax, eax
            je L00498e68
            push esi
            call BrActorRemove
            mov eax, dword ptr [esp + 0x18]
            mov ecx, dword ptr [esp + 0x14]
            and eax, 0xff
            and ecx, 0xff
            add esp, 4
            lea edx, [eax + eax*2]
            mov eax, dword ptr [gProgram_state+0x1bb8]
            push esi
            mov ecx, dword ptr [eax + ecx*4]
            mov edx, dword ptr [ecx + edx*4]
            push edx
            call BrActorAdd
            add esp, 8
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 8
            ret
    L00498dc0:
            test byte ptr [ebx + 0x10], 4
            mov eax, dword ptr [ebp + 0x100]
            je L00498dd6
            test al, al
            jne L00498dde
            and al, 1
            or al, 1
            jmp L00498dd8
    L00498dd6:
            and al, 0
    L00498dd8:
            mov dword ptr [ebp + 0x100], eax
    L00498dde:
            test byte ptr [ebx + 0x10], 8
            mov eax, dword ptr [ebp + 0x100]
            je L00498df7
            test ah, 0xff
            jne L00498e00
            and ah, 1
            or ah, 1
            jmp L00498dfa
    L00498df7:
            and ah, 0
    L00498dfa:
            mov dword ptr [ebp + 0x100], eax
    L00498e00:
            test byte ptr [ebx + 0x10], 2
            je L00498e25
            mov edx, dword ptr [ebp + 8]
            push 2
            lea ecx, [ebx + 0x12]
            call SelectCrushNetwork
            mov eax, dword ptr [ebp + 8]
            mov byte ptr [eax + 0xec], 1
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 8
            ret
    L00498e25:
            lea esi, [ebx + 0x12]
            mov ecx, esi
            call GetNetworkDataSize
            mov edi, dword ptr [ebp + 8]
            mov ecx, eax
            mov edx, ecx
            add edi, 0x294
            shr ecx, 2
            rep movsd
            mov ecx, edx
            and ecx, 3
            rep movsb
            mov eax, dword ptr [ebp + 8]
            mov ecx, dword ptr [ebx + 8]
            mov dword ptr [eax + 0x268], ecx
            mov edx, dword ptr [ebp + 8]
            mov byte ptr [edx + 0x261], 0x11
            mov eax, dword ptr [ebp + 8]
            mov byte ptr [eax + 0xec], 0
    L00498e68:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 8
            ret
    }
#endif
}

// SignalToStartRace

// SignalToRepositionCar

// SetUpNetCarPositions

// ReinitialiseCar

// RepositionPlayer

// DisableCar

// EnableCar

// DoNetworkHeadups

// SortNetHeadAscending

// SortNetHeadDescending

// ClipName

// CreateBoxes

// DrawScoreBoxes

// FUNCTION: CARMA2_HW 0x004999f0
void C2_HOOK_FASTCALL DoNetScores(void) {
    DoNetScores2(0);
}

// FUNCTION: CARMA2_HW 0x0049a710
void C2_HOOK_FASTCALL InitNetHeadups(void) {

    gDigits_pix = LoadPixelmap("HDIGITS.PIX");
    if (gDigits_pix != NULL) {
        BrMapAdd(gDigits_pix);
    }
}

// FUNCTION: CARMA2_HW 0x0049a730
void C2_HOOK_FASTCALL DisposeNetHeadups(void) {
    if (gDigits_pix != NULL) {
        BrMapRemove(gDigits_pix);
        BrPixelmapFree(gDigits_pix);
    }
}

// EverybodysLost

// DeclareWinner

// FUNCTION: CARMA2_HW 0x0049a990
void C2_HOOK_FASTCALL CarInContactWithFox(tNet_game_player_info* pPlayer) {
    if ((int)(PDGetTotalTime() - gLast_it_change) > 500) {
        if (gLast_lepper != pPlayer) {
            gLast_it_change = PDGetTotalTime();
            if (gIt_or_fox >= 0) {
                gLast_lepper = &gNet_players[gIt_or_fox];
            }
            PlayerIsIt(pPlayer);
        }
    }
}

// PlayerIsIt

// FUNCTION: CARMA2_HW 0x0049a940
void C2_HOOK_FASTCALL CheckForVampireWinner(void) {
    int i;
    int players_left;
    int winner_index;

    players_left = 0;
    winner_index = 0;
    if (gProgram_state.racing && gNet_mode == eNet_mode_host) {
        for (i = 0; i < gNumber_of_net_players; i++) {
            if (!gNet_players[i].field_0x80) {
                players_left++;
                winner_index = i;
            }
        }
        if (players_left <= 1) {
            DeclareWinner(winner_index);
        }
    }
}

// FarEnoughAway

// CarInContactWithFox

// SelectRandomFox

// CalcPlayerScores

// SendPlayerScores

// DoNetGameManagement

// InitialisePlayerScore

// InitPlayers

// UseGeneralScore

// FUNCTION: CARMA2_HW 0x0049b8d0
void C2_HOOK_FASTCALL NetSendEnvironmentChanges(tNet_game_player_info* pRecipient) {
    int i;
    char exclamation;
    br_actor** non_car_list;
    tNon_car_spec* non_car;
    tNet_message_chunk* message;

    exclamation = '!';
    non_car_list = gProgram_state.track_spec.non_car_list;
    for (i = 0; i < gProgram_state.track_spec.count_non_cars; i++, non_car_list++) {
        if ((*non_car_list)->identifier[3] != exclamation && (*non_car_list)->identifier[3] != '&') {
            continue;
        }
        non_car = (tNon_car_spec*)(*non_car_list)->type_data;
        if (non_car != NULL
                && non_car->driver != 0
                && (tU16)non_car->car_ID == i
                && (non_car->flags & 0x100000) != 0) {
            continue;
        }
        message = NetGetToPlayerContents(eNet_message_chunk_type_non_car_position, 0, pRecipient->ID);
        if (message != NULL) {
            BrMatrix34Copy(&message->non_car_position.mat, &(*non_car_list)->t.t.mat);
            message->non_car_position.ID = i;
            message->non_car_position.flags = (*non_car_list)->identifier[3] == exclamation;
        }
    }
    SendDetachedNonCarDataToPlayer(pRecipient);
    FUN_0043d7d0(pRecipient);
    FUN_0042da20((undefined4*)pRecipient);
    FUN_004f0f80(pRecipient);
    NetSendMessageStacks();
}

// FUNCTION: CARMA2_HW 0x0049b9a0
void C2_NAKED C2_HOOK_FASTCALL CheckForNeedyEnvironmentRecipients(void) {
#ifndef CARPOCALYPSE2_MATCHING
#ifndef CARPOCALYPSE2_MATCHING
    int i;

    if (gNet_mode != eNet_mode_host) {
        return;
    }
    for (i = 0; i < gNumber_of_net_players; i++) {
        if (i != gThis_net_player_index && gNet_players[i].player_status >= ePlayer_status_ready
                && gNet_players[i].player_status < ePlayer_status_racing) {
            SendCarData(PDGetTotalTime() + 1000);
        }
    }
#else
    int i;

    if (gNet_mode != eNet_mode_host) {
        return;
    }
    for (i = 0; i < gNumber_of_net_players; i++) {
        if (i != gThis_net_player_index && gNet_players[i].player_status >= ePlayer_status_ready
                && gNet_players[i].player_status < ePlayer_status_racing) {
            SendCarData(PDGetTotalTime() + 1000);
        }
    }
#endif
#else
    __asm {
            sub esp, 8
            mov ecx, dword ptr [gNumber_of_net_players]
            mov eax, 1
            cmp ecx, eax
            mov dword ptr [esp], eax
            jle L0049bac9
            push edi
            push esi
            mov esi, offset gNet_players+0x12c
            push ebp
            push ebx
            mov dword ptr [esp + 0x14], esi
    L0049b9c7:
            cmp dword ptr [esi], 0
            je L0049baa4
            mov eax, dword ptr [esi - 0x10]
            lea ebx, [esi - 0x58]
            cmp eax, 6
            jne L0049baa4
            mov eax, dword ptr [gProgram_state+0x1bbc]
            mov edi, dword ptr [gProgram_state+0x1bc0]
            xor ebp, ebp
            test eax, eax
            jle L0049ba78
    L0049b9f4:
            mov eax, dword ptr [edi]
            mov ecx, dword ptr [eax + 0x14]
            mov cl, byte ptr [ecx + 3]
            cmp cl, 0x21
            je L0049ba06
            cmp cl, 0x26
            jne L0049ba67
    L0049ba06:
            mov eax, dword ptr [eax + 0x5c]
            test eax, eax
            je L0049ba2d
            mov ecx, dword ptr [eax + 0xc]
            test ecx, ecx
            je L0049ba2d
            xor edx, edx
            mov dx, word ptr [eax + 0x80]
            cmp edx, ebp
            jne L0049ba2d
            test dword ptr [eax + 0x100], 0x100000
            jne L0049ba67
    L0049ba2d:
            mov eax, dword ptr [ebx + 0x24]
            xor edx, edx
            push eax
            mov cl, 0x1c
            call NetGetToPlayerContents
            mov esi, eax
            test esi, esi
            je L0049ba67
            mov ecx, dword ptr [edi]
            lea edx, [esi + 0xc]
            add ecx, 0x2c
            push ecx
            push edx
            call BrMatrix34Copy
            mov dword ptr [esi + 8], ebp
            mov eax, dword ptr [edi]
            add esp, 8
            xor edx, edx
            mov ecx, dword ptr [eax + 0x14]
            cmp byte ptr [ecx + 3], 0x21
            sete dl
            mov word ptr [esi + 0x3c], dx
    L0049ba67:
            mov eax, dword ptr [gProgram_state+0x1bbc]
            inc ebp
            add edi, 4
            cmp ebp, eax
            jl L0049b9f4
            mov esi, dword ptr [esp + 0x14]
    L0049ba78:
            mov ecx, ebx
            call SendDetachedNonCarDataToPlayer
            mov ecx, ebx
            call FUN_0043d7d0
            mov ecx, ebx
            call FUN_0042da20
            mov ecx, ebx
            call FUN_004f0f80
            call NetSendMessageStacks
            mov dword ptr [esi], 0
            call NetSendMessageStacks
    L0049baa4:
            mov eax, dword ptr [esp + 0x10]
            mov ecx, dword ptr [gNumber_of_net_players]
            inc eax
            add esi, 0xd4
            cmp eax, ecx
            mov dword ptr [esp + 0x10], eax
            mov dword ptr [esp + 0x14], esi
            jl L0049b9c7
            pop ebx
            pop ebp
            pop esi
            pop edi
    L0049bac9:
            add esp, 8
            ret
    }
#endif
}

// ReceivedGameplay

// SendGameplay

// SendGameplayToAllPlayers

// SendGameplayToHost

// InitNetGameplayStuff

// FUNCTION: CARMA2_HW 0x0049bd10
void C2_HOOK_FASTCALL DefaultNetName(void) {
    NetObtainSystemUserName(gProgram_state.player_name, 32);
}

// FUNCTION: CARMA2_HW 0x0049bde0
void C2_HOOK_FASTCALL AddPlayerToShapeStatusLists(int pPlayer_id, int pCount, tNet_game_player_info* pPlayers) {
    tCar_crush_network_shapes* shapes;
    int j;

    for (; pCount > 0; pCount--, pPlayers++) {
        if (pPlayers->car != NULL && pPlayers->car->car_crush_spec != NULL) {
            shapes = pPlayers->car->car_crush_spec->network_stuff;
            for (j = 0; j < (int)CARPOCALYPSE2_ASIZE(shapes->items); j++) {
                if (shapes->items[j].network_id == 0) {
                    shapes->items[j].network_id = pPlayer_id;
                    shapes->items[j].field_0x4 = 1;
                    break;
                }
            }
        }
    }
}

// RemovePlayerFromShapeStatusLists

// FUNCTION: CARMA2_HW 0x00498390
void C2_HOOK_FASTCALL ReceivedRecover(tNet_message_chunk_recover* pContents) {
    int i;

    if (gNet_players[gThis_net_player_index].ID != pContents->player) {
        for (i = 0; i < gNumber_of_net_players; i++) {
            if (gNet_players[i].ID == pContents->player) {
                gNet_players[i].car->time_to_recover = pContents->time;
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x004983f0
void C2_HOOK_FASTCALL ReceivedCrushSync(tNet_message_chunk_car_crush_sync* pContents) {
    int i;
    int j;
    int player_id;
    tCar_spec* car;
    tCar_crush_network_shapes* network_stuff;
    tU16* pCrush_state;
    int* network_id;

    i = 0;
    if (pContents->count > 0) {
        pCrush_state = &pContents->players[0].crush_state;
        do {
            car = NetCarFromPlayerID(*((tPlayer_ID*)pCrush_state - 1));
            if (car != NULL && *pCrush_state == car->car_crush_spec->field_0x24) {
                network_stuff = car->car_crush_spec->network_stuff;
                player_id = pContents->player_id;
                if (network_stuff != NULL) {
                    j = 0;
                    network_id = (int*)network_stuff;
                    while (j < 12) {
                        if (*network_id == player_id) {
                            goto clear_network_flag;
                        }
                        j++;
                        network_id += 2;
                    }
                    goto next_crush_state;
clear_network_flag:
                    network_stuff->items[j].field_0x4 = 0;
next_crush_state:
                    ;
                }
            }
            i++;
            pCrush_state += 4;
        } while (i < pContents->count);
    }
}

// FUNCTION: CARMA2_HW 0x004984c0
void C2_NAKED C2_HOOK_FASTCALL FUN_004984c0(tNet_message_chunk_car_reposition* pContents) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;
    tCar_spec* car;
    br_matrix34* car_matrix;
    tU32 repair_delta;
    float deflection;

    car = NULL;
    for (i = 0; i < gNumber_of_net_players; i++) {
        if (gNet_players[i].ID == pContents->player_id) {
            car = gNet_players[i].car;
            break;
        }
    }
    if (car == NULL) {
        return;
    }
    car->active = 0;
    car_matrix = &car->car_master_actor->t.t.mat;
    BrMatrix34RotateY(car_matrix, pContents->angle);
    ExpandVector3((br_vector3*)&car_matrix->m[3][0], &pContents->position, -500.f, 500.f);
    BrMatrix34Copy(&car->collision_info->transform_matrix, car_matrix);
    car->pos.v[0] = car->collision_info->transform_matrix.m[3][0];
    car->pos.v[1] = car->collision_info->transform_matrix.m[3][1];
    car->pos.v[2] = car->collision_info->transform_matrix.m[3][2];
    car->collision_info->pos.v[0] = car->collision_info->transform_matrix.m[3][0];
    car->collision_info->pos.v[1] = car->collision_info->transform_matrix.m[3][1];
    car->collision_info->pos.v[2] = car->collision_info->transform_matrix.m[3][2];
    if (car->driver >= eDriver_net_human) {
        repair_delta = pContents->repair_time;
        if (repair_delta > car->repair_time) {
            repair_delta -= car->repair_time;
            if (repair_delta >= 100000) {
                TotallyRepairACar(car);
                car->repair_time = car->collision_info->field_0x27c;
                return;
            }
            ComputeCarImpact(car, (int)repair_delta, &deflection);
        }
    }
#else
    __asm {
            push ecx
            mov edx, dword ptr [gNumber_of_net_players]
            push ebx
            push esi
            xor esi, esi
            xor eax, eax
            test edx, edx
            push edi
            mov ebx, ecx
            jle L004984fe
            mov edi, dword ptr [ebx + 4]
            mov ecx, offset gNet_players+0x24
    L004984dc:
            cmp dword ptr [ecx], edi
            je L004984ed
            inc eax
            add ecx, 0xd4
            cmp eax, edx
            jl L004984dc
            jmp L004984fe
    L004984ed:
            lea ecx, [eax + eax*2]
            lea ecx, [ecx + ecx*8]
            shl ecx, 1
            sub ecx, eax
            mov esi, dword ptr [ecx*4 + gNet_players+0xc8]
    L004984fe:
            test esi, esi
            je L004985de
            mov edi, dword ptr [esi + 0x10]
            mov dword ptr [esi + 0x1d0], 0
            mov dx, word ptr [ebx + 2]
            add edi, 0x2c
            push edx
            push edi
            call BrMatrix34RotateY
            add esp, 8
            lea edx, [ebx + 8]
            lea ecx, [edi + 0x24]
            push 0x43fa0000
            push 0xc3fa0000
            call ExpandVector3
            mov eax, dword ptr [esi + 8]
            add eax, 0x8c
            push edi
            push eax
            call BrMatrix34Copy
            mov eax, dword ptr [esi + 8]
            add esp, 8
            mov ecx, dword ptr [eax + 0xb0]
            mov dword ptr [esi + 0x4c], ecx
            mov edx, dword ptr [eax + 0xb4]
            mov dword ptr [esi + 0x50], edx
            mov eax, dword ptr [eax + 0xb8]
            mov dword ptr [esi + 0x54], eax
            mov eax, dword ptr [esi + 8]
            mov ecx, dword ptr [eax + 0xb0]
            mov dword ptr [eax + 0x80], ecx
            mov eax, dword ptr [esi + 8]
            mov edx, dword ptr [eax + 0xb4]
            mov dword ptr [eax + 0x84], edx
            mov eax, dword ptr [esi + 8]
            mov ecx, dword ptr [eax + 0xb8]
            mov dword ptr [eax + 0x88], ecx
            mov eax, dword ptr [esi + 0xc]
            cmp eax, 7
            jl L004985de
            mov edx, dword ptr [ebx + 0x10]
            mov eax, dword ptr [esi + 0x18a4]
            cmp edx, eax
            jbe L004985de
            sub edx, eax
            cmp edx, 0x186a0
            jb L004985d0
            mov ecx, esi
            call TotallyRepairACar
            mov edx, dword ptr [esi + 8]
            mov eax, dword ptr [edx + 0x27c]
            mov dword ptr [esi + 0x18a4], eax
            pop edi
            pop esi
            pop ebx
            pop ecx
            ret
    L004985d0:
            lea ecx, [esp + 0xc]
            push ecx
            mov ecx, esi
            call ComputeCarImpact
            fstp st(0)
    L004985de:
            pop edi
            pop esi
            pop ebx
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
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00498460
void C2_HOOK_FASTCALL CompressCarDataChunk(tU8* pChunk, int pPlayer_index) {
    tCar_spec* car;
    br_actor* actor;

    car = gNet_players[pPlayer_index].car;
    CompressVector3((tCompressed_vector3*)(pChunk + 8), &car->car_master_actor->t.t.translate.t, -500.0f, 500.0f);
    actor = car->car_master_actor;
    *(tS16*)(pChunk + 2) = (tS16)BrRadianToAngle(atan2(actor->t.t.mat.m[2][0], actor->t.t.mat.m[2][2]));
    *(int*)(pChunk + 4) = gNet_players[pPlayer_index].ID;
    *(int*)(pChunk + 0x10) = *(int*)((tU8*)car + 0x18a4);
}

// FUNCTION: CARMA2_HW 0x00499220
void C2_HOOK_FASTCALL ReinitialiseCar(tCar_spec* pCar) {
    StopCarSmokingInstantly(pCar);
    CloseDownPowerUpsForCar(pCar);
    InitialiseCar(pCar);
    TotallyRepairACar(pCar);
    if (pCar != NULL && pCar->driver == eDriver_local_human) {
        gTime_for_punishment = PDGetTotalTime() + 2000;
        EmptyInventory();
    }
}

// FUNCTION: CARMA2_HW 0x0049aaf0
void C2_HOOK_FASTCALL FUN_0049aaf0(void) {
}

// FUNCTION: CARMA2_HW 0x0049bad0
void C2_HOOK_FASTCALL FUN_0049bad0(void* pArg1, void* pArg2, int pArg3) {
    int iVar1;
    int iVar2;

    iVar1 = *(int*)((tU8*)pArg1 + 4);
    if (iVar1 == 4) {
        if (gUNK_0068d954 == 0) {
            gUNK_0068d954 = 1;
            NetFullScreenMessage(0xe4, 1);
            iVar1 = PermitNetServiceReentrancy();
            do {
                NetService(0);
                iVar2 = CheckQuit();
                if (iVar2 != 0) {
                    NetFullScreenMessage(0xe4, 1);
                }
            } while (((gWaiting_for_unpause != 0) && (gProgram_state.prog_status != eProg_idling)) &&
                     ((gRace_finished == 0 || (gRace_over_reason != 4))));
            if (iVar1 != 0) {
                HaltNetServiceReentrancy();
            }
            gWaiting_for_unpause = 1;
            FadePaletteDown();
            ClearEntireScreen();
            FadePaletteUp();
            gUNK_0068d954 = 0;
            return;
        }
    } else {
        if (iVar1 == 5) {
            gWaiting_for_unpause = 0;
            return;
        }
        if (iVar1 == 3) {
            KnackerThisCar(NetCarFromPlayerID(*(tPlayer_ID*)((tU8*)pArg2 + 8)));
            return;
        }
        if (*(int*)((tU8*)gCurrent_net_game + 0x74) >= 3 && *(int*)((tU8*)gCurrent_net_game + 0x74) <= 5) {
            switch (iVar1) {
            case 0:
                Checkpoint(*(int*)((tU8*)pArg1 + 8), 1);
                break;
            case 1:
                WrongCheckpoint(*(int*)((tU8*)pArg1 + 8));
                return;
            case 2:
                DoFancyHeadup(0x1a);
                ChangeAmbientPratcam(0x24);
                gRace_finished = 1;
                return;
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x0049bd20
void C2_HOOK_FASTCALL FUN_0049bd20(int* pDst, const int* pSrc) {
    pDst[0] = pSrc[0];
    pDst[1] = pSrc[1];
    pDst[2] = pSrc[2];
    pDst[3] = pSrc[3];
    pDst[4] = pSrc[4];
    pDst[5] = pSrc[5];
    pDst[6] = pSrc[9];
    pDst[7] = pSrc[10];
    pDst[8] = pSrc[11];
}

// FUNCTION: CARMA2_HW 0x0049be40
void C2_HOOK_FASTCALL RemovePlayerFromShapeStatusLists(tPlayer_ID pPlayer_id, int pPlayer_count, tNet_game_player_info* pPlayers) {
    int i;

    for (i = 0; i < pPlayer_count; i++) {
        tCar_spec* car = pPlayers[i].car;

        if (car != NULL && car->car_crush_spec != NULL) {
            tCar_crush_network_shapes* network_shapes = car->car_crush_spec->network_stuff;
            int j;

            for (j = 0; j < 12; j++) {
                if ((tU32)network_shapes->items[j].network_id == pPlayer_id) {
                    network_shapes->items[j].network_id = 0;
                    break;
                }
            }
        }
    }
}
