#include "c2_hooks.h"
#include "carpocalypse2_types.h"
#include "camera.h"
#include "world.h"
#include "opponent.h"

#include "replay.h"

#include "globvars.h"
#include "pedestrn.h"
#include "utility.h"
#include "drone.h"

#include "brucetrk.h"

#include "car.h"

#include "graphics.h"

#include "globvrpb.h"

#include "network.h"

#include "piping.h"

#include "smashing.h"

extern tNapalm_bolt gNapalm_bolts[5];
extern float gConst_replay_rate_zero;
extern br_scalar gFLOAT_00589a5c;
extern const double gDOUBLE_00589a60;
extern int gMax_count_non_cars;
extern const br_scalar gFloat_0058aec0;
extern const br_scalar gFloat_0058aec4;
extern const double gDouble_0058aec8;
extern const br_vector3 gVector3_00661678;

// GLOBAL: CARMA2_HW 0x006a829c
int gCount_shrapnel_shower_calls;

// GLOBAL: CARMA2_HW 0x0058aee0
const br_scalar gFloat_0058aee0 = 10.0f;

// GLOBAL: CARMA2_HW 0x0058aeb8
const double gDouble_0058aeb8 = 0.14492753623188406;

// GLOBAL: CARMA2_HW 0x0058aee8
const double gDouble_0058aee8 = 0.001;

// GLOBAL: CARMA2_HW 0x0058aef0
const double gDouble_0058aef0 = 6.283185307179586;
extern void C2_HOOK_FASTCALL FUN_004f8b50(const br_vector3* pPosition, const br_vector3* pVelocity, const br_vector3* pDirection, br_scalar pStrength, int pMaterialCount, br_material** pMaterials);
extern void C2_HOOK_FASTCALL FUN_004c8940(uintptr_t pOwner);
typedef void (C2_HOOK_FASTCALL *tCreateSmokeColumn2Worker)(int, br_actor*, int, int, int);

#include "powerups.h"
#include "spark.h"

#include "compress.h"

#include "crush.h"

// Replay pipe worker helpers (no orig PDB symbols; the Apply/Undo wrappers in
// replay_callbacks.c forward onto them). Kept in their own TU so MSVC does not
// inline the stub bodies into the wrappers, letting the wrappers emit a tail
// call/jmp to this external symbol.

void C2_HOOK_FASTCALL WorkerFrameBoundary(int field_0x4) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL WorkerGrooveOff(br_actor* pActor, tU32 pBlock_flags) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL WorkerFunkEnable(int field_0x0, tU16 field_0x4) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL WorkerPowerupGot(int field_0x0, int field_0x4) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL WorkerPowerupLose(int field_0x0, int field_0x4) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL WorkerPowerupRespawn(int field_0x0, int field_0x4) {

    *(char*)(field_0x0 + 0x20) = 4;
}

void C2_HOOK_FASTCALL WorkerPowerupRespawnUndo(int field_0x0, int field_0x4) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL WorkerShitMine(int field_0x0, br_vector3* pPos) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL WorkerEndShitMine(int field_0x0, br_vector3* pPos) {

    NOT_IMPLEMENTED();
}

#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_CDECL BrMatrix34Copy();
extern undefined4 C2_HOOK_FASTCALL CharacterNoLongerRenderable(tPed_character_instance* pCharacter);
extern br_scalar C2_HOOK_FASTCALL DRU16ToScalar(tU16 pValue, float pMin, float pMax);
extern void C2_HOOK_FASTCALL DoDroneFunkyGroovyThings(tDrone_spec *pDrone);
extern void C2_HOOK_FASTCALL ExpandVector3(br_vector3* pDest, const tCompressed_vector3 *pSrc, float pMin, float pMax);
extern undefined4 C2_HOOK_FASTCALL MakeCharacterRenderable2(tPed_character_instance* pCharacter, int pIndex);
extern void C2_HOOK_FASTCALL PointActorAlongThisBloodyVector(br_actor* pThe_actor, br_vector3* pThe_vector);
extern void C2_HOOK_FASTCALL RecalculateOrientationOfRoot(tPed_character_instance* pPed);
extern void C2_HOOK_FASTCALL SetCharacterBonePositions(tPed_character_instance* pPed, undefined4 pArg2, undefined4 pArg3);
extern void C2_HOOK_FASTCALL SetCharacterDirection(tPed_character_instance* pPed, const br_vector3* pDir, const br_vector3* pUp);
extern void C2_HOOK_FASTCALL SetCharacterPosition(tPed_character_instance* pCharacter, br_vector3* pPos, int pArg3);
extern br_uint_8 bl;
extern int gBOOL_00744804;
extern tDrone_path_node* gDrone_path_nodes;
extern tDrone_spec* gDrone_specs;
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_FASTCALL ApplyInitialMovement(undefined4* pArg1, br_vector3* pArg2, br_vector3* pArg3, br_vector3* pArg4, float pArg5, br_vector3* pArg6, br_vector3* pArg7, br_vector3* pArg8);
extern br_actor* C2_HOOK_CDECL BrActorAdd();
extern void C2_HOOK_CDECL BrActorFree();
extern br_actor* C2_HOOK_CDECL BrActorRemove();
extern void C2_HOOK_CDECL BrMatrix34Mul();
extern void C2_HOOK_CDECL BrMatrix34Scale();
extern br_actor* C2_HOOK_FASTCALL CloneActor(br_actor* pActor);
extern void C2_HOOK_FASTCALL CreateSmashedGlassFragments(tU32 pSeed, br_vector3* pDirection, br_actor* pSource_actor, tU32 pTime, int pFace_count, const tU16* pSource_faces, int pVertex_count, const tU16* pSource_vertices, tU32 pOwner, tU32 pFlags, float pLongest_edge_squared, tSmashable_initial_speed_spec* pInitial_speed, const int* pLifetime_range, tU32 pFragment_owner, br_matrix34* pSource_transform);
extern void C2_HOOK_FASTCALL CreateSmokeColumn2(int pType, br_actor* pActor, tCar_spec* pCar, tPhysics_object* pObject, int pColour, int pVertex, tU32 pLifetime);
extern tNon_car_spec* C2_HOOK_FASTCALL DoPullActorFromWorld(br_actor* actor);
extern tSmash_vertex* C2_HOOK_FASTCALL FindShrapnelFragment(br_model* pModel, tU32 pField_0x18, int pPriority);
extern int C2_HOOK_FASTCALL IRandomBetween(int pA, int pB);
extern tNet_message* C2_HOOK_FASTCALL NetBuildGuaranteedMessage(tU8 pNet_message_type, int pOption);
extern int C2_HOOK_FASTCALL NetGuaranteedSendMessageToAllPlayers(tNet_game_details* pDetails, tNet_message* pMessage, void* pNotifyFail);
extern int C2_HOOK_FASTCALL PercentageChance(int pC);
extern void C2_HOOK_FASTCALL PipeSingleShrapnelShower(uintptr_t pId, undefined4 pArg2, undefined4 pArg3, undefined4 pArg4, br_vector3* pArg5, br_vector3* pArg6, float pArg7, br_vector3* pArg8, br_vector3* pArg9, undefined4 pArg10, br_bounds3* pArg11, undefined4 pArg12, br_matrix34* pArg13, br_vector3* pArg14, void* pArg15);
extern int C2_HOOK_FASTCALL PointOutOfSight(const br_vector3* pPoint, undefined4 pArg2, br_scalar pMax_distance);
extern float C2_HOOK_FASTCALL PositionFragment(br_actor* pActor, tSmashable_initial_position_spec* pInitial_position, br_actor* pSource_actor, br_vector3* pPosition4, br_vector3* pPosition5, br_actor* pRotation_actor, float* pRandom_ranges, br_matrix34* pPost_matrix);
extern tNet_game_details* gCurrent_net_game;
extern br_actor* gDont_render_actor;
extern int gINT_006a3334;
extern int gINT_006a8294;
extern br_matrix34 gMatrix34_006b78a0;
extern tNet_mode gNet_mode;
extern int gPed_676914;
extern br_vector3 gVector3_006a4688;
extern float gYon_squared;
extern br_vector3 gZero_vector__smash;
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern char gString_00660084[];
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern char gMatchString_00660084[];
#endif
// FUNCTION: CARMA2_HW 0x004ca830
void C2_NAKED C2_HOOK_FASTCALL WorkerPedPos(tPedestrian* pPed, br_vector3* pVec) {
#ifndef CARPOCALYPSE2_MATCHING

    if (gBOOL_00744804 != 0) {
        return;
    }
    if (pVec->v[0] == gConst_replay_rate_zero
            && pVec->v[1] == gConst_replay_rate_zero
            && pVec->v[2] == gConst_replay_rate_zero) {
        pPed->flags |= 0x80;
        return;
    }
    SetCharacterPosition(pPed->character, pVec, -1);
    if ((pPed->flags & 0x80) != 0) {
        pPed->flags &= ~0x80;
    }
#else
    __asm {
            mov eax, dword ptr [gBOOL_00744804]
            push esi
            test eax, eax
            mov esi, ecx
            jne L004ca888
            fld dword ptr [edx]
            fcomp dword ptr [gConst_replay_rate_zero]
            fnstsw ax
            test ah, 0x40
            je L004ca871
            fld dword ptr [edx + 4]
            fcomp dword ptr [gConst_replay_rate_zero]
            fnstsw ax
            test ah, 0x40
            je L004ca871
            fld dword ptr [edx + 8]
            fcomp dword ptr [gConst_replay_rate_zero]
            fnstsw ax
            test ah, 0x40
            je L004ca871
            or byte ptr [esi + 8], 0x80
            pop esi
            ret
    L004ca871:
            mov ecx, dword ptr [esi]
            push -1
            call SetCharacterPosition
            mov ax, word ptr [esi + 8]
            test al, 0x80
            je L004ca888
            and al, 0x7f
            mov word ptr [esi + 8], ax
    L004ca888:
            pop esi
            ret
            nop
            nop
            nop
            nop
            nop
            nop
    }
#endif
}

void C2_HOOK_FASTCALL WorkerNapalmBolt(int field_0x0, int field_0x4, br_vector3* pVec) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL WorkerPedDir(int field_0x0, br_vector3* pDir) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004ca7d0
void C2_HOOK_FASTCALL WorkerPedPhysics(int pPedestrian_id, br_matrix34* pMatrix) {
    tPedestrian* ped;

    ped = &gPedestrian_array[pPedestrian_id & 0xffff];
    BrMatrix34Copy(GetCharacterMatrixPtr(ped->character), pMatrix);
    ped->pos.v[0] = pMatrix->m[3][0];
    ped->pos.v[1] = pMatrix->m[3][1];
    ped->pos.v[2] = pMatrix->m[3][2];
    RecalculateOrientationOfRoot(ped->character);
    SetCharacterBonePositions(ped->character, 3, 0);
}

// FUNCTION: CARMA2_HW 0x004529d0
void C2_HOOK_FASTCALL WorkerDroneCornerPos(int pDrone_index, tU16 pSpeed, tU16 pControl_a, tU16 pControl_b, const tU16* pTransform) {
    tDrone_spec* drone;

    drone = &gDrone_specs[pDrone_index];
    drone->field_0xe4 = DRU16ToScalar(pSpeed, -500.f, 500.f);
    drone->field_0xe0 = DRU16ToScalar(pControl_a, 0.f, 1.f);
    drone->field_0xe8 = DRU16ToScalar(pControl_b, 0.f, 100.f);

    drone->actor->t.t.mat.m[0][0] = DRU16ToScalar(pTransform[0], -1.f, 1.f);
    drone->actor->t.t.mat.m[0][1] = DRU16ToScalar(pTransform[1], -1.f, 1.f);
    drone->actor->t.t.mat.m[0][2] = DRU16ToScalar(pTransform[2], -1.f, 1.f);
    drone->actor->t.t.mat.m[1][0] = DRU16ToScalar(pTransform[3], -1.f, 1.f);
    drone->actor->t.t.mat.m[1][1] = DRU16ToScalar(pTransform[4], -1.f, 1.f);
    drone->actor->t.t.mat.m[1][2] = DRU16ToScalar(pTransform[5], -1.f, 1.f);
    drone->actor->t.t.mat.m[2][0] = DRU16ToScalar(pTransform[6], -1.f, 1.f);
    drone->actor->t.t.mat.m[2][1] = DRU16ToScalar(pTransform[7], -1.f, 1.f);
    drone->actor->t.t.mat.m[2][2] = DRU16ToScalar(pTransform[8], -1.f, 1.f);
    drone->actor->t.t.mat.m[3][0] = DRU16ToScalar(pTransform[9], -1000.f, 1000.f);
    drone->actor->t.t.mat.m[3][1] = DRU16ToScalar(pTransform[10], -1000.f, 1000.f);
    drone->actor->t.t.mat.m[3][2] = DRU16ToScalar(pTransform[11], -1000.f, 1000.f);
    DoDroneFunkyGroovyThings(drone);
}

void C2_HOOK_FASTCALL WorkerFlap(br_actor* pActor, tS16 pDelta) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL WorkerModelMash(int field_0x4, tU8* pField_0x8) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL WorkerModelMashUndo(int field_0x4, tU8* pField_0x8) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL WorkerExplosion(int field_0x0, br_vector3* pField_0x4) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL WorkerNonCarCreation(int field_0x0) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL WorkerNonCarCreationUndo(int field_0x0) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL WorkerSmashRemoveFaces(int field_0x0, int count) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL WorkerSmashDecal(int field_0x0) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL WorkerPHILObject(int field_0x0, br_matrix34* pField_0x4) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL WorkerNonCar(int field_0x4, br_vector3* pField_0x8) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL WorkerSmokeColumn(int column_flags, br_vector3* pPos, tU16 vertex, int pFlags_hi) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004f6060
void C2_HOOK_FASTCALL WorkerSmashTextureChange(br_material* pMaterial, tCar_spec* pCar, int field_0xc, br_pixelmap* pColour_map) {
    if (pCar != NULL && pCar->field_0x1960 != NULL) {
        pMaterial->user = pColour_map;
    } else {
        pMaterial->colour_map = pColour_map;
    }
    BrMaterialUpdate(pMaterial, BR_MATU_COLOURMAP);
    if (pCar == &gProgram_state.current_car && gAction_replay_camera_mode == 8) {
        MungeCarMaterials(pCar, 1);
    }
}

void C2_HOOK_FASTCALL WorkerUndoSmashRemoveFaces(int field_0x0, int field_0x8, void* pField_0xc, int field_0x4) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL WorkerSmashDecalUndo(int field_0x0) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL WorkerExtendedSplash(int field_0x0, tU8 field_0x4, int field_0x8, br_vector3* pField_0xc, br_vector3* pField_0x18) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004ca900
void C2_NAKED C2_HOOK_FASTCALL WorkerPedDismember(int field_0x0, int field_0x4, int field_0x7) {
#ifndef CARPOCALYPSE2_MATCHING
    tPedestrian* ped;
    tPed_character_instance* character;
    br_actor* actor;

    ped = (tPedestrian*)field_0x0;
    ped->character->field_0xc |= gPow2_array[field_0x4];
    ped->character->field_0x6 = field_0x7;
    MakeCharacterPhysicsForBone(ped->character, field_0x4, 1, 0);
    character = ped->character;
    actor = character->personality->form->actor_sets[(tS8)character->field_0x4].actors[field_0x4];
    if (actor->parent == NULL && (field_0x4 == 0 || (gPow2_array[field_0x4] & character->field_0xc) != 0)) {
        BrActorAdd(gNon_track_actor, actor);
    }
#else
    __asm {
            push ebx
            push esi
            push edi
            mov edi, edx
            mov esi, ecx
            push 0
            lea ebx, [edi*4]
            push 1
            mov eax, dword ptr [esi]
            mov ecx, dword ptr [ebx + gPow2_array]
            mov edx, dword ptr [eax + 0xc]
            or edx, ecx
            mov dword ptr [eax + 0xc], edx
            mov edx, dword ptr [esi]
            mov al, byte ptr [esp + 0x18]
            mov byte ptr [edx + 6], al
            mov ecx, dword ptr [esi]
            mov edx, edi
            call MakeCharacterPhysicsForBone
            mov esi, dword ptr [esi]
            mov edx, dword ptr [esi]
            movsx ecx, byte ptr [esi + 4]
            mov eax, dword ptr [edx + 0x28]
            mov edx, dword ptr [eax + 0x30]
            mov eax, dword ptr [edx + ecx*8 + 4]
            mov eax, dword ptr [eax + ebx]
            mov ecx, dword ptr [eax + 0xc]
            test ecx, ecx
            jne L004ca971
            test edi, edi
            je L004ca961
            mov ecx, dword ptr [esi + 0xc]
            mov edx, dword ptr [ebx + gPow2_array]
            test ecx, edx
            je L004ca971
    L004ca961:
            mov edx, dword ptr [gNon_track_actor]
            push eax
            push edx
            call BrActorAdd
            add esp, 8
    L004ca971:
            pop edi
            pop esi
            pop ebx
            ret 4
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

// FUNCTION: CARMA2_HW 0x004ca980
void C2_HOOK_FASTCALL WorkerPedDismemberUndo(tPedestrian* pPed, int pBone, tS8 pField_0x6) {
    tPed_character_instance* character;
    tPed_form_actor_set* actor_set;
    br_actor* actor;

    actor_set = &pPed->character->personality->form->actor_sets[(tS8)pPed->character->field_0x4];
    pPed->character->field_0xc &= ~gPow2_array[pBone];
    pPed->character->field_0x10 &= ~gPow2_array[pBone];
    pPed->character->field_0x6 = pField_0x6;
    character = pPed->character;
    SetCharacterBonePositions(character, 3, 0);
    actor = actor_set->actors[pBone];
    if (actor->parent != NULL) {
        BrActorRemove(actor);
    }
}

void C2_HOOK_FASTCALL WorkerPedFPChange(int field_0x0, tPipe_stack_word field_0x4) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004ca8d0
void C2_HOOK_FASTCALL WorkerPedModelChange(tPedestrian* pPed, int pBone, int pModel, int pSet) {
    if (pBone < 0) {
        SetCharacterAllBonesModel(pPed->character, pModel, pSet);
    } else {
        SetCharacterBoneModel(pPed->character, pBone, pModel, pSet);
    }
}

// FUNCTION: CARMA2_HW 0x004ca8a0
void C2_HOOK_FASTCALL WorkerPedModelChangeUndo(int field_0x0, int field_0x4, int field_0xc, int field_0xa) {
    tPedestrian* ped = (tPedestrian*)field_0x0;

    if (field_0x4 < 0) {
        SetCharacterAllBonesModel(ped->character, -1, field_0xa);
    } else {
        FUN_0040b500(ped->character, field_0x4, (br_model*)field_0xc, field_0xa);
    }
}

// FUNCTION: CARMA2_HW 0x00452880
void C2_NAKED C2_HOOK_FASTCALL WorkerDroneStraightPos(int pDrone_index, tDrone_path_node_section* pSection, tU16 pSpeed, tU16 pProgress) {
#ifndef CARPOCALYPSE2_MATCHING
    tDrone_spec* drone;
    br_vector3 direction;

    drone = &gDrone_specs[pDrone_index];
    drone->field_0xe4 = DRU16ToScalar(pSpeed, -500.f, 500.f);
    drone->field_0xe0 = 0.f;

    direction.v[0] = (pSection->field_0x1c - DRU16ToScalar(pProgress, 0.f, 255.f)) * pSection->field_0x10.v[0];
    direction.v[1] = (pSection->field_0x1c - DRU16ToScalar(pProgress, 0.f, 255.f)) * pSection->field_0x10.v[1];
    direction.v[2] = (pSection->field_0x1c - DRU16ToScalar(pProgress, 0.f, 255.f)) * pSection->field_0x10.v[2];
    direction.v[0] = -direction.v[0];
    direction.v[1] = -direction.v[1];
    direction.v[2] = -direction.v[2];
    drone->actor->t.t.translate.t.v[0] = gDrone_path_nodes[pSection->node1].position.v[0] + direction.v[0];
    drone->actor->t.t.translate.t.v[1] = gDrone_path_nodes[pSection->node1].position.v[1] + direction.v[1];
    drone->actor->t.t.translate.t.v[2] = gDrone_path_nodes[pSection->node1].position.v[2] + direction.v[2];

    direction.v[0] = pSection->field_0x10.v[0];
    direction.v[1] = pSection->field_0x10.v[1];
    direction.v[2] = pSection->field_0x10.v[2];
    if (drone->form->flags & 0x10) {
        direction.v[1] = 0.f;
    }
    PointActorAlongThisBloodyVector(drone->actor, &direction);
    DoDroneFunkyGroovyThings(drone);
#else
    __asm {
            sub esp, 0xc
            mov eax, ecx
            push ebx
            shl eax, 5
            sub eax, ecx
            push esi
            mov esi, edx
            push edi
            mov edx, dword ptr [gDrone_specs]
            lea eax, [eax + eax*2]
            push 0x43fa0000
            push 0xc3fa0000
            lea ecx, [ecx + eax*2]
            lea edi, [edx + ecx*8]
            mov ecx, dword ptr [esp + 0x24]
            call DRU16ToScalar
            fstp dword ptr [edi + 0xe4]
            mov ebx, dword ptr [esp + 0x20]
            push 0x437f0000
            push 0
            mov ecx, ebx
            mov dword ptr [edi + 0xe0], 0
            call DRU16ToScalar
            fsubr dword ptr [esi + 0x1c]
            push 0x437f0000
            push 0
            mov ecx, ebx
            fmul dword ptr [esi + 0x10]
            fstp dword ptr [esp + 0x14]
            call DRU16ToScalar
            fsubr dword ptr [esi + 0x1c]
            push 0x437f0000
            push 0
            mov ecx, ebx
            fmul dword ptr [esi + 0x14]
            fstp dword ptr [esp + 0x18]
            call DRU16ToScalar
            fsubr dword ptr [esi + 0x1c]
            movsx eax, word ptr [esi]
            fmul dword ptr [esi + 0x18]
            fld dword ptr [esp + 0xc]
            fchs
            fstp dword ptr [esp + 0xc]
            fld dword ptr [esp + 0x10]
            fchs
            fstp dword ptr [esp + 0x10]
            lea ecx, [eax + eax*8]
            fchs
            fstp dword ptr [esp + 0x14]
            lea edx, [eax + ecx*2]
            mov ecx, dword ptr [gDrone_path_nodes]
            lea eax, [eax + edx*4]
            mov edx, dword ptr [edi + 0xec]
            fld dword ptr [ecx + eax*4]
            fadd dword ptr [esp + 0xc]
            fstp dword ptr [edx + 0x50]
            movsx eax, word ptr [esi]
            lea ecx, [eax + eax*8]
            lea edx, [eax + ecx*2]
            mov ecx, dword ptr [gDrone_path_nodes]
            lea eax, [eax + edx*4]
            mov edx, dword ptr [edi + 0xec]
            fld dword ptr [ecx + eax*4 + 4]
            fadd dword ptr [esp + 0x10]
            fstp dword ptr [edx + 0x54]
            movsx eax, word ptr [esi]
            lea ecx, [eax + eax*8]
            lea edx, [eax + ecx*2]
            mov ecx, dword ptr [gDrone_path_nodes]
            lea eax, [eax + edx*4]
            fld dword ptr [ecx + eax*4 + 8]
            fadd dword ptr [esp + 0x14]
            mov edx, dword ptr [edi + 0xec]
            fstp dword ptr [edx + 0x58]
            mov eax, dword ptr [esi + 0x10]
            mov ecx, dword ptr [esi + 0x14]
            mov edx, dword ptr [esi + 0x18]
            mov dword ptr [esp + 0xc], eax
            mov dword ptr [esp + 0x10], ecx
            mov dword ptr [esp + 0x14], edx
            mov eax, dword ptr [edi + 4]
            test byte ptr [eax + 0x2c], 0x10
            je L004529b1
            mov dword ptr [esp + 0x10], 0
    L004529b1:
            mov ecx, dword ptr [edi + 0xec]
            lea edx, [esp + 0xc]
            call PointActorAlongThisBloodyVector
            mov ecx, edi
            call DoDroneFunkyGroovyThings
            pop edi
            pop esi
            pop ebx
            add esp, 0xc
            ret 8
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004da960
void C2_HOOK_FASTCALL WorkerRepulseRay(int pIndex, br_vector3* pOrigin, br_vector3* pVictim, tU32 pTime) {
    gRepulse_links[pIndex].pos_origin = pOrigin;
    gRepulse_links[pIndex].pos_victim = pVictim;
    gRepulse_links[pIndex].time = pTime;
}

void C2_HOOK_FASTCALL WorkerSmashModelChangeApply(int field_0x0, int field_0x4, int field_0x8) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL WorkerSmashModelChangeB(int field_0x0) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL WorkerSmashModelChangeBUndo(int field_0x0) {

    NOT_IMPLEMENTED();
}
// FUNCTION: CARMA2_HW 0x0043bd70
void C2_NAKED C2_HOOK_FASTCALL WorkerCrush(int field_0x4, void* pField_0x8, void* pField_0xe) {
#ifndef CARPOCALYPSE2_MATCHING
    br_model* pModel;
    br_vector3 offset;
    br_vector3 scaled_offset;
    tU16* pWeights;
    float scale;
    int i;
    int vertex_offset;

    pModel = (br_model*)field_0x4;
    ExpandVector3(&offset, (const tCompressed_vector3*)pField_0x8, -1.f, 1.f);
    pWeights = (tU16*)pField_0xe;
    i = 0;
    if (pModel->nvertices > 0) {
        vertex_offset = 0;
        do {
            if (*pWeights != 0) {
                scale = DRU16ToScalar(*pWeights, 0.f, 10.f);
                scaled_offset.v[0] = scale * offset.v[0];
                scaled_offset.v[1] = scale * offset.v[1];
                scaled_offset.v[2] = scale * offset.v[2];
                *(float*)((tU8*)pModel->vertices + vertex_offset) = scaled_offset.v[0] + *(float*)((tU8*)pModel->vertices + vertex_offset);
                *(float*)((tU8*)pModel->vertices + vertex_offset + 4) = scaled_offset.v[1] + *(float*)((tU8*)pModel->vertices + vertex_offset + 4);
                *(float*)((tU8*)pModel->vertices + vertex_offset + 8) = scaled_offset.v[2] + *(float*)((tU8*)pModel->vertices + vertex_offset + 8);
            }
            i++;
            pWeights++;
            vertex_offset += sizeof(br_vertex);
        } while (i < pModel->nvertices);
    }
    if ((pModel->flags & BR_MODF_CUSTOM) != 0 && pModel->user != NULL) {
        *(int*)pModel->user = 1;
        return;
    }
    pModel->custom = JitModelUpdate;
    pModel->flags |= BR_MODF_CUSTOM;
#else
    __asm {
            sub esp, 0x18
            push ebx
            push ebp
            push esi
            push edi
            mov esi, ecx
            push 0x3f800000
            push 0xbf800000
            lea ecx, [esp + 0x18]
            call ExpandVector3
            xor ebx, ebx
            cmp word ptr [esi + 0x10], bx
            jbe L0043be0b
            mov ebp, dword ptr [esp + 0x2c]
            xor edi, edi
    L0043bd9a:
            mov cx, word ptr [ebp]
            test cx, cx
            je L0043bdfa
            push 0x41200000
            push 0
            call DRU16ToScalar
            mov eax, dword ptr [esi + 8]
            fld st(0)
            fmul dword ptr [esp + 0x10]
            fld st(1)
            fmul dword ptr [esp + 0x14]
            fxch st(1)
            fadd dword ptr [eax + edi]
            fxch st(2)
            fmul dword ptr [esp + 0x18]
            fxch st(1)
            fxch st(2)
            fxch st(1)
            fxch st(1)
            fstp dword ptr [eax + edi]
            add eax, edi
            mov ecx, dword ptr [esi + 8]
            fxch st(1)
            fadd dword ptr [ecx + edi + 4]
            fxch st(1)
            fst dword ptr [esp + 0x24]
            lea eax, [ecx + edi + 4]
            fxch st(1)
            _emit 0xd9
        _emit 0x18
            mov edx, dword ptr [esi + 8]
            fadd dword ptr [edx + edi + 8]
            lea eax, [edx + edi + 8]
            _emit 0xd9
        _emit 0x18
    L0043bdfa:
            xor eax, eax
            inc ebx
            mov ax, word ptr [esi + 0x10]
            add ebp, 2
            add edi, 0x28
            cmp ebx, eax
            jl L0043bd9a
    L0043be0b:
            mov ax, word ptr [esi + 0x20]
            test al, 0x20
            je L0043be2a
            mov ecx, dword ptr [esi + 0x28]
            test ecx, ecx
            je L0043be2a
            mov dword ptr [ecx], 1
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x18
            ret 4
    L0043be2a:
            or al, 0x20
            mov dword ptr [esi + 0x24], 0x431590
            mov word ptr [esi + 0x20], ax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x18
            ret 4
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

void C2_HOOK_FASTCALL WorkerCrushUndo(int field_0x4, void* pField_0x8, void* pField_0xe) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL WorkerSpark(int field_0x0, void* pField_0x4, void* pField_0x10) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004f86f0
void C2_HOOK_FASTCALL WorkerShrapnel(int pShrapnel_num, br_vector3* pPos, tU16 pAge, br_material* pMaterial) {
    int i;

    i = pShrapnel_num & 0x7fff;
    if ((gShrapnel_flags & (1 << i)) == 0) {
        BrActorAdd(gNon_track_actor, gShrapnel[i].actor);
    }
    gShrapnel_flags |= 1 << i;
    gShrapnel[i].actor->t.t.translate.t.v[0] = pPos->v[0];
    gShrapnel[i].actor->t.t.translate.t.v[1] = pPos->v[1];
    gShrapnel[i].actor->t.t.translate.t.v[2] = pPos->v[2];
    if ((pShrapnel_num & 0x8000) != 0) {
        gShrapnel[i].age = pAge;
        gShrapnel[i].actor->material = pMaterial;
    }
}

void C2_HOOK_FASTCALL WorkerProxRay(int field_0x0, tU16 field_0x6, tU16 field_0x4, int field_0x8) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL WorkerGrooveStop(int field_0x0, br_vector3* pField_0x14, int field_0x4, int field_0x8, int field_0xc, int field_0x10) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL WorkerOilSpill(int field_0x0, br_vector3* pField_0x4, int field_0x34, int field_0x38, int field_0x3c, int zero, int field_0x44, br_vector3* pField_0x48, int field_0x54) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL WorkerGibShower(int field_0x0, int field_0xc, int field_0x8, int field_0x10, int field_0x4, br_vector3* pField_0x14, br_vector3* pField_0x20, br_vector3* pField_0x2c) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004ca500
void C2_HOOK_FASTCALL WorkerPedMove(tPedestrian* pPed, int pMove_id, int pSpeed_bits, int pField_0x8, int pField_0x14, int pFrame, br_vector3* pField_0x24, int pAction, br_vector3* pField_0x38) {
    int current_move_id;

    if (!gBOOL_00744804) {
        if (pMove_id != -1) {
            current_move_id = pPed->character->personality->form->moves[pPed->character->field_0x7].id;
            BrMatrix34Copy(&pPed->character->field_0x8c, (br_matrix34*)pField_0x38);
            SetCharacterMove(pPed->character, pMove_id, *(float*)&pSpeed_bits, pField_0x14, pField_0x8, GetTotalTime());
            if (!(pPed->character->field_0x14 & 4)) {
                SetCharacterPosition(pPed->character, pField_0x24, -1);
            }
            if (current_move_id != pMove_id) {
                pPed->character->field_0x1c = (tU16)pFrame;
                SetCharacterBonePositions(pPed->character, 0, 0);
            }
        }
        if (pAction != -1) {
            pPed->action = (tPed_action)pAction;
        }
    }
}

// FUNCTION: CARMA2_HW 0x004ca5a0
void C2_HOOK_FASTCALL WorkerPedMoveUndo(tPedestrian* pPed, int pMove_id, float field_0x10, int zero, int zero2, int field_0xa, br_vector3* pField_0x18, int field_0x30, br_vector3* pField_0x38) {
    int old_move_id;

    if (gBOOL_00744804 != 0) {
        return;
    }
    if (pMove_id != -1) {
        old_move_id = pPed->character->personality->form->moves[pPed->character->field_0x7].id;
        BrMatrix34Copy(&pPed->character->field_0x8c, (br_matrix34*)pField_0x38);
        SetCharacterMove(pPed->character, pMove_id, field_0x10, zero2, zero, GetTotalTime());
        if ((pPed->character->field_0x14 & 4) == 0) {
            SetCharacterPosition(pPed->character, pField_0x18, -1);
        }
        if (old_move_id != pMove_id) {
            pPed->character->field_0x1c = field_0xa;
            SetCharacterBonePositions(pPed->character, 0, 0);
            pPed->character->field_0x24 = GetTotalTime();
        }
    }
    if (field_0x30 != -1) {
        pPed->action = field_0x30;
    }
}

void C2_HOOK_FASTCALL WorkerSpecial(void) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL WorkerSpecialUndo(void) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004ca680
void C2_NAKED C2_HOOK_FASTCALL WorkerPedStatus(int field_0x0, tU8 field_0x4, tU8 field_0x5, tU8 field_0x6, tU8 field_0x7, tU8 field_0x9, tU8 field_0xb, br_vector3* p0xc, br_vector3* p0x18, br_vector3* p0x24) {
#ifndef CARPOCALYPSE2_MATCHING
    tPedestrian* ped = (tPedestrian*)field_0x0;

    if (field_0x5 != 0xff) {
        if (field_0x5 != 0) {
            ped->flags |= 1;
            MakeCharacterRenderable2(ped->character, (int)(tS8)field_0x9);
        } else {
            ped->flags &= ~1;
            CharacterNoLongerRenderable(ped->character);
        }
    }
    if (field_0x7 != 0xff) {
        ped->character->field_0x14 = (int)(tS8)field_0x7;
        ped->character->field_0x5 = field_0xb;
        if ((field_0x6 & 4) != 0 && (field_0x7 & 4) == 0) {
            RecalculateOrientationOfRoot(ped->character);
        }
    }
#else
    __asm {
            mov al, byte ptr [esp + 4]
            push esi
            cmp al, 0xff
            mov esi, ecx
            je L004ca6ac
            test al, al
            je L004ca6a1
            movsx edx, byte ptr [esp + 0x14]
            mov ecx, dword ptr [esi]
            or byte ptr [esi + 8], 1
            call MakeCharacterRenderable2
            jmp L004ca6ac
    L004ca6a1:
            mov ecx, dword ptr [esi]
            and byte ptr [esi + 8], 0xfe
            call CharacterNoLongerRenderable
    L004ca6ac:
            mov al, byte ptr [esp + 0x10]
            cmp al, 0xff
            je L004ca6d9
            mov edx, dword ptr [esi]
            movsx ecx, al
            mov dword ptr [edx + 0x14], ecx
            mov ecx, dword ptr [esi]
            mov dl, byte ptr [esp + 0x18]
            mov byte ptr [ecx + 5], dl
            mov cl, byte ptr [esp + 0xc]
            test cl, 4
            je L004ca6d9
            test al, 4
            jne L004ca6d9
            mov ecx, dword ptr [esi]
            call RecalculateOrientationOfRoot
    L004ca6d9:
            pop esi
            ret 0x20
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004ca6e0
void C2_NAKED C2_HOOK_FASTCALL WorkerPedStatusUndo(int field_0x0, tU8 field_0x4, tU8 field_0x5, tU8 field_0x6, tU8 field_0x7, tU8 field_0x8, tU8 field_0xa, br_vector3* p0xc, br_vector3* p0x18, br_vector3* p0x24) {
#ifndef CARPOCALYPSE2_MATCHING
    tPedestrian* ped;

    ped = (tPedestrian*)field_0x0;
    if (field_0x4 != 0xff) {
        if (field_0x4 != 0) {
            ped->flags |= 1;
            MakeCharacterRenderable2(ped->character, (int)(tS8)field_0x8);
        } else {
            *(tU8*)&ped->flags &= ~1;
            CharacterNoLongerRenderable(ped->character);
        }
    }
    if (field_0x6 == 0xff) {
        return;
    }
    if ((field_0x6 & 4) == 0) {
        if ((field_0x7 & 4) != 0) {
            ped->character->field_0x14 = (int)(tS8)field_0x6;
            ped->character->field_0x5 = field_0xa;
            SetCharacterDirection(ped->character, p0xc, p0x18);
            BrMatrix34Copy(&ped->character->field_0x8c, (br_matrix34*)p0x24);
            SetCharacterBonePositions(ped->character, 0, 0);
            return;
        }
    }
    if ((field_0x7 & 4) == 0 && (field_0x6 & 4) != 0) {
        BrMatrix34Copy(&ped->character->field_0x8c, (br_matrix34*)p0x24);
        SetCharacterBonePositions(ped->character, 0, 0);
        ped->character->field_0x14 = (int)(tS8)field_0x6;
        ped->character->field_0x5 = field_0xa;
        return;
    }
    ped->character->field_0x14 = (int)(tS8)field_0x6;
    ped->character->field_0x5 = field_0xa;
#else
    __asm {
            push ebx
            push esi
            cmp dl, 0xff
            mov esi, ecx
            je L004ca70a
            test dl, dl
            je L004ca6ff
            movsx edx, byte ptr [esp + 0x18]
            mov ecx, dword ptr [esi]
            or byte ptr [esi + 8], 1
            call MakeCharacterRenderable2
            jmp L004ca70a
    L004ca6ff:
            mov ecx, dword ptr [esi]
            and byte ptr [esi + 8], 0xfe
            call CharacterNoLongerRenderable
    L004ca70a:
            mov bl, byte ptr [esp + 0x10]
            cmp bl, 0xff
            je L004ca7bd
            mov cl, byte ptr [esp + 0x14]
            mov al, bl
            and al, 4
            jne L004ca76c
            test cl, 4
            je L004ca771
            mov ecx, dword ptr [esi]
            movsx eax, bl
            mov dword ptr [ecx + 0x14], eax
            mov edx, dword ptr [esi]
            mov al, byte ptr [esp + 0x1c]
            mov ecx, dword ptr [esp + 0x24]
            mov byte ptr [edx + 5], al
            mov edx, dword ptr [esp + 0x20]
            push ecx
            mov ecx, dword ptr [esi]
            call SetCharacterDirection
            mov eax, dword ptr [esi]
            mov edx, dword ptr [esp + 0x28]
            add eax, 0x8c
            push edx
            push eax
            call BrMatrix34Copy
            mov ecx, dword ptr [esi]
            add esp, 8
            xor edx, edx
            push 0
            call SetCharacterBonePositions
            pop esi
            pop ebx
            ret 0x20
    L004ca76c:
            test cl, 4
            jne L004ca7ac
    L004ca771:
            test al, al
            je L004ca7ac
            mov edx, dword ptr [esi]
            mov ecx, dword ptr [esp + 0x28]
            add edx, 0x8c
            push ecx
            push edx
            call BrMatrix34Copy
            mov ecx, dword ptr [esi]
            add esp, 8
            xor edx, edx
            push 0
            call SetCharacterBonePositions
            mov ecx, dword ptr [esi]
            movsx eax, bl
            mov dword ptr [ecx + 0x14], eax
            mov edx, dword ptr [esi]
            mov al, byte ptr [esp + 0x1c]
            mov byte ptr [edx + 5], al
            pop esi
            pop ebx
            ret 0x20
    L004ca7ac:
            mov edx, dword ptr [esi]
            movsx ecx, bl
            mov dword ptr [edx + 0x14], ecx
            mov eax, dword ptr [esi]
            mov cl, byte ptr [esp + 0x1c]
            mov byte ptr [eax + 5], cl
    L004ca7bd:
            pop esi
            pop ebx
            ret 0x20
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

void C2_HOOK_FASTCALL WorkerGibShowerUndo(int field_0xc, int prev_0x0, int prev_0xc, int prev_0x8, int prev_0x10, int prev_0x4, br_vector3* prev_0x14, br_vector3* prev_0x20, br_vector3* prev_0x2c) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL WorkerSplitWeldApply(void* roadMachine, br_vector3* pVector) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL WorkerSplitWeldUndo(void* roadMachine) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004eb8f0
void C2_NAKED C2_HOOK_FASTCALL WorkerShrapnelShower(int pOwner, int* pShrapnel_spec, tU32 pSeed, undefined4 pArg4, undefined4 pArg5, undefined4 pArg6, undefined4 pArg7, int pTime, undefined4 pArg9, br_matrix34* pTransform, undefined4 pFragment_owner, br_vector3* pPosition, float pDistance, undefined4 pArg14, undefined4 pArg15) {
#ifndef CARPOCALYPSE2_MATCHING
    float fVar1;
    undefined4 uVar2;
    int iVar3;
    int iVar4;
    tU32 uVar5;
    int iVar6;
    int iVar7;
    int iVar8;
    undefined4 uVar9;
    undefined4 uVar10;
    undefined4 uVar11;
    int bVar12;
    tU8* local_c8;
    int local_c4;
    int local_c0;
    tU32 local_70;
    br_matrix34 basis;
    br_matrix34 scale_matrix;
    br_matrix34 emission_transform;
    br_vector3 initial_velocity;
    br_vector3 initial_omega;
    float width;
    float height;
    if (gAction_replay_mode != 0) {
        BrMatrix34Copy(&basis, pTransform);
    } else     if ((((*pShrapnel_spec == 2) || (*pShrapnel_spec == 3))
                && (pShrapnel_spec[10] == 2)) && (gINT_006a3334 != 0)) {
            basis.m[0][0] = gMatrix34_006b78a0.m[1][0] - gMatrix34_006b78a0.m[0][0];
            basis.m[0][1] = gMatrix34_006b78a0.m[1][1] - gMatrix34_006b78a0.m[0][1];
            fVar1 = gMatrix34_006b78a0.m[1][2] - gMatrix34_006b78a0.m[0][2];
            basis.m[0][2] = (float)sqrt(fVar1 * fVar1 + basis.m[0][1] * basis.m[0][1] + basis.m[0][0] * basis.m[0][0]);
            if (basis.m[0][2] <= gFloat_0058aec4) {
                basis.m[0][0] = 1.0f;
                basis.m[0][1] = 0.0f;
                basis.m[0][2] = 0.0f;
            } else {
                basis.m[0][2] = (float)gDouble_0058aec8 / basis.m[0][2];
                basis.m[0][0] = basis.m[0][2] * basis.m[0][0];
                basis.m[0][1] = basis.m[0][2] * basis.m[0][1];
                basis.m[0][2] = basis.m[0][2] * fVar1;
            }
            basis.m[1][0] = gMatrix34_006b78a0.m[2][0] - gMatrix34_006b78a0.m[0][0];
            basis.m[1][1] = gMatrix34_006b78a0.m[2][1] - gMatrix34_006b78a0.m[0][1];
            fVar1 = gMatrix34_006b78a0.m[2][2] - gMatrix34_006b78a0.m[0][2];
            basis.m[1][2] = (float)sqrt(fVar1 * fVar1 + basis.m[1][1] * basis.m[1][1] + basis.m[1][0] * basis.m[1][0]);
            if (basis.m[1][2] <= gFloat_0058aec4) {
                basis.m[1][0] = 1.0f;
                basis.m[1][1] = 0.0f;
                basis.m[1][2] = 0.0f;
            } else {
                basis.m[1][2] = (float)gDouble_0058aec8 / basis.m[1][2];
                basis.m[1][0] = basis.m[1][2] * basis.m[1][0];
                basis.m[1][1] = basis.m[1][2] * basis.m[1][1];
                basis.m[1][2] = basis.m[1][2] * fVar1;
            }
            basis.m[2][2] = (float)sqrt(gVector3_006a4688.v[2] * gVector3_006a4688.v[2]
                    + gVector3_006a4688.v[1] * gVector3_006a4688.v[1]
                    + gVector3_006a4688.v[0] * gVector3_006a4688.v[0]);
            if (basis.m[2][2] <= gFloat_0058aec4) {
                basis.m[2][0] = 1.0f;
                basis.m[2][1] = 0.0f;
                basis.m[2][2] = 0.0f;
            } else {
                basis.m[2][2] = (float)gDouble_0058aec8 / basis.m[2][2];
                basis.m[2][0] = basis.m[2][2] * gVector3_006a4688.v[0];
                basis.m[2][1] = basis.m[2][2] * gVector3_006a4688.v[1];
                basis.m[2][2] = basis.m[2][2] * gVector3_006a4688.v[2];
            }
            basis.m[3][0] = gMatrix34_006b78a0.m[1][0];
            basis.m[3][1] = gMatrix34_006b78a0.m[1][1];
            basis.m[3][2] = gMatrix34_006b78a0.m[1][2];
    }

    switch (*pShrapnel_spec) {
    case 0:
        if (gAction_replay_mode != 0 || PointOutOfSight((br_vector3*)pArg6, gYon_squared, gYon_squared) == 0) {
            if (pPosition == NULL) {
                pPosition = (br_vector3*)&gVector3_00661678;
            }
            FUN_004f8b50((br_vector3*)pArg6, &gZero_vector__smash, pPosition,
                    (float)sqrt(pDistance) * gFloat_0058aee0,
                    pShrapnel_spec[0xe], (br_material**)(pShrapnel_spec + 0xf));
        }
        break;
    case 1:
        if (gAction_replay_mode != 0 || PointOutOfSight((br_vector3*)pArg6, 0, gYon_squared) == 0) {
            if (pOwner == 0) {
                uVar9 = 0;
                uVar11 = 0;
                uVar10 = 0;
                uVar2 = 0;
            } else {
                uVar9 = *(undefined4*)(pOwner + 0x70);
                uVar11 = *(undefined4*)(pOwner + 0x6c);
                uVar10 = *(undefined4*)(pOwner + 0x78);
                uVar2 = *(undefined4*)(pOwner + 0x74);
            }
            CreateSmashedGlassFragments(pSeed, (br_vector3*)pArg4, (br_actor*)pArg7,
                    (tU32)pTime, (int)uVar2, (const tU16*)uVar10, (int)uVar11,
                    (const tU16*)uVar9, pShrapnel_spec[0xe], pShrapnel_spec[0x10],
                    *(float*)(pShrapnel_spec + 0xf), (tSmashable_initial_speed_spec*)(pShrapnel_spec + 1),
                    (const int*)(pShrapnel_spec + 8), (tU32)pArg9, (br_matrix34*)pTransform);
        }
        break;
    case 2:
        if (gAction_replay_mode != 0 || PointOutOfSight((br_vector3*)pArg6, 0, gYon_squared) == 0) {
            local_c8 = gINT_006a3334 != 0 ? (tU8*)&basis : NULL;
            uVar5 = rand();
            srand(pSeed);
            iVar3 = pShrapnel_spec[0xe];
            if (iVar3 >= 0) {
                iVar4 = IRandomBetween(iVar3, pShrapnel_spec[0xf]);
            } else {
                iVar4 = pShrapnel_spec[0x10];
            }
            if (local_c8 != NULL && *(float*)&pShrapnel_spec[0x11] != gFloat_0058aec0
                    && *(float*)&pShrapnel_spec[0x12] != gFloat_0058aec0) {
                width = (float)sqrt(
                    (gMatrix34_006b78a0.m[1][0] - gMatrix34_006b78a0.m[0][0]) * (gMatrix34_006b78a0.m[1][0] - gMatrix34_006b78a0.m[0][0]) +
                    (gMatrix34_006b78a0.m[1][1] - gMatrix34_006b78a0.m[0][1]) * (gMatrix34_006b78a0.m[1][1] - gMatrix34_006b78a0.m[0][1]) +
                    (gMatrix34_006b78a0.m[1][2] - gMatrix34_006b78a0.m[0][2]) * (gMatrix34_006b78a0.m[1][2] - gMatrix34_006b78a0.m[0][2]));
                height = (float)sqrt(
                    (gMatrix34_006b78a0.m[2][0] - gMatrix34_006b78a0.m[0][0]) * (gMatrix34_006b78a0.m[2][0] - gMatrix34_006b78a0.m[0][0]) +
                    (gMatrix34_006b78a0.m[2][1] - gMatrix34_006b78a0.m[0][1]) * (gMatrix34_006b78a0.m[2][1] - gMatrix34_006b78a0.m[0][1]) +
                    (gMatrix34_006b78a0.m[2][2] - gMatrix34_006b78a0.m[0][2]) * (gMatrix34_006b78a0.m[2][2] - gMatrix34_006b78a0.m[0][2]));
                if (width < gDouble_0058aee8 || height < gDouble_0058aee8) {
                    break;
                }
                BrMatrix34Scale(&scale_matrix, width / *(float*)&pShrapnel_spec[0x11], height / *(float*)&pShrapnel_spec[0x12], 1.0f);
                BrMatrix34Mul(&emission_transform, &scale_matrix, &basis);
                local_c8 = (tU8*)&emission_transform;
            }

            iVar6 = 0;
            if (iVar4 > 0) {
                do {
                    if (iVar3 >= 0) {
                        iVar7 = IRandomBetween(0, pShrapnel_spec[0x10] - 1);
                        local_c4 = *(int*)((tU8*)pShrapnel_spec[0x13] + iVar7 * 4);
                    } else {
                        local_c4 = *(int*)((tU8*)pShrapnel_spec[0x13] + iVar6 * 4);
                    }
                    iVar7 = (int)FindShrapnelFragment(((br_actor*)local_c4)->model, (tU32)pArg9, 4);
                    if (iVar7 == 0) {
                        break;
                    }
                    fVar1 = PositionFragment(((tSmash_vertex*)iVar7)->actor,
                            (tSmashable_initial_position_spec*)(pShrapnel_spec + 10),
                            local_c4, (br_vector3*)pArg5, (br_vector3*)pArg6,
                            pArg7, (undefined4*)pFragment_owner, (int)local_c8);
                    ApplyInitialMovement((undefined4*)(pShrapnel_spec + 1),
                            (br_vector3*)pArg4, &((tSmash_vertex*)iVar7)->v, &((tSmash_vertex*)iVar7)->omega,
                            fVar1, &gZero_vector__smash, (br_vector3*)pArg14,
                            &((tSmash_vertex*)iVar7)->actor->t.t.translate.t);
                    *(int*)(iVar7 + 0xc) = pTime;
                    *(int*)(iVar7 + 0x14) = pTime;
                    *(int*)(iVar7 + 8) = pTime + pShrapnel_spec[9];
                    iVar8 = IRandomBetween(pShrapnel_spec[8], pShrapnel_spec[9]);
                    iVar6++;
                    *(int*)(iVar7 + 0x10) = iVar8 + pTime;
                } while (iVar6 < iVar4);
            }
            srand(uVar5);
        }
        break;
    case 3:
        bVar12 = gINT_006a3334 != 0;
        local_c0 = 0;
        if (gNet_mode != 3 || gINT_006a8294 != 0) {
            iVar3 = pShrapnel_spec[0xe];
            if (iVar3 >= 0) {
                iVar4 = IRandomBetween(iVar3, pShrapnel_spec[0xf]);
            } else {
                iVar4 = pShrapnel_spec[0x10];
            }
            local_c4 = 0;
            if (iVar4 > 0) {
                do {
                    uVar5 = rand();
                    local_70 = uVar5;
                    if (gMax_count_non_cars <= gProgram_state.track_spec.count_non_cars) {
                        break;
                    }
                    if (gNet_mode == 2) {
                        local_c0 = (int)NetBuildGuaranteedMessage(0x22, 0);
                        *(tU32*)(local_c0 + 0x1c) = uVar5;
                        *(tU16*)(local_c0 + 0x20) = (tU16)gProgram_state.track_spec.count_non_cars;
                    }
                    if (iVar3 >= 0) {
                        iVar6 = IRandomBetween(0, pShrapnel_spec[0x10] - 1);
                        iVar6 = *(int*)((tU8*)pShrapnel_spec[0x11] + iVar6 * 4);
                    } else {
                        iVar6 = *(int*)((tU8*)pShrapnel_spec[0x11] + local_c4 * 4);
                    }
                    if (gNet_mode == 2) {
                        *(tU16*)(local_c0 + 0x22) = *(tU16*)(*(int*)(iVar6 + 0x14) + 1);
                    }
                    iVar6 = (int)CloneActor((br_actor*)iVar6);
                    sprintf((char*)(*(int*)(iVar6 + 0x14) + 4), "%04d.ACT", gProgram_state.track_spec.count_non_cars);
                    gProgram_state.track_spec.non_car_list[gProgram_state.track_spec.count_non_cars] = (br_actor*)iVar6;
                    gProgram_state.track_spec.count_non_cars++;
                    BrActorAdd(gDont_render_actor, (br_actor*)iVar6);
                    fVar1 = PositionFragment((br_actor*)iVar6,
                            (tSmashable_initial_position_spec*)(pShrapnel_spec + 10), iVar6,
                            (br_vector3*)pArg5, (br_vector3*)pArg6, pArg7,
                            (undefined4*)pFragment_owner, (int)(uintptr_t)&basis);
                    if (*(int*)(iVar6 + 0x5c) != 0 || (iVar7 = (int)DoPullActorFromWorld((br_actor*)iVar6)) == 0) {
                        BrActorRemove((br_actor*)iVar6);
                        BrActorFree((br_actor*)iVar6);
                        gProgram_state.track_spec.count_non_cars--;
                        break;
                    }
                    uVar5 = rand();
                    ApplyInitialMovement((undefined4*)(pShrapnel_spec + 1),
                            (br_vector3*)pArg4, &initial_velocity, &initial_omega,
                            fVar1, &gZero_vector__smash, (br_vector3*)pArg14,
                            &((br_actor*)iVar6)->t.t.translate.t);
                    *(float*)(*(int*)(iVar7 + 8) + 0x68) = initial_velocity.v[0] * gDouble_0058aeb8;
                    *(float*)(*(int*)(iVar7 + 8) + 0x6c) = initial_velocity.v[1] * gDouble_0058aeb8;
                    *(float*)(*(int*)(iVar7 + 8) + 0x70) = initial_velocity.v[2] * gDouble_0058aeb8;
                    *(float*)(*(int*)(iVar7 + 8) + 0x74) = initial_omega.v[0] * gDouble_0058aef0;
                    *(float*)(*(int*)(iVar7 + 8) + 0x78) = initial_omega.v[1] * gDouble_0058aef0;
                    *(float*)(*(int*)(iVar7 + 8) + 0x7c) = initial_omega.v[2] * gDouble_0058aef0;
                    *(tU8*)(*(int*)(iVar7 + 8) + 0xec) = 0;
                    srand(local_70);
                    iVar8 = PercentageChance(pShrapnel_spec[0x12]);
                    if (iVar8 != 0 && pShrapnel_spec[0x13] > 0) {
                        iVar8 = 0;
                        do {
                            uVar9 = IRandomBetween(0, *(tU16*)(*(int*)(*(int*)(*(int*)(iVar6 + 0x18) + 0x4c) + 0x18) + 0x1e));
                            IRandomBetween(pShrapnel_spec[0x14], pShrapnel_spec[0x15]);
                            ((tCreateSmokeColumn2Worker)CreateSmokeColumn2)(2, (br_actor*)iVar6,
                                    iVar7, 0, (int)uVar9);
                            iVar8++;
                        } while (iVar8 < pShrapnel_spec[0x13]);
                    }
                    if (gNet_mode == 2) {
                        BrMatrix34Copy(&basis, &gMatrix34_006b78a0);
                        *(undefined4*)(local_c0 + 0x24) = *(undefined4*)(*(int*)(iVar7 + 8) + 0x68);
                        *(undefined4*)(local_c0 + 0x28) = *(undefined4*)(*(int*)(iVar7 + 8) + 0x6c);
                        *(undefined4*)(local_c0 + 0x2c) = *(undefined4*)(*(int*)(iVar7 + 8) + 0x70);
                        *(undefined4*)(local_c0 + 0x30) = *(undefined4*)(*(int*)(iVar7 + 8) + 0x74);
                        *(undefined4*)(local_c0 + 0x34) = *(undefined4*)(*(int*)(iVar7 + 8) + 0x78);
                        *(undefined4*)(local_c0 + 0x38) = *(undefined4*)(*(int*)(iVar7 + 8) + 0x7c);
                        *(int*)(local_c0 + 0x6c) = pShrapnel_spec[0x12];
                        *(int*)(local_c0 + 0x70) = pShrapnel_spec[0x13];
                        *(int*)(local_c0 + 0x74) = pShrapnel_spec[0x14];
                        *(int*)(local_c0 + 0x78) = pShrapnel_spec[0x15];
                    }
                    FUN_004c8940(gProgram_state.track_spec.count_non_cars - 1);
                    if (gNet_mode == 2) {
                        NetGuaranteedSendMessageToAllPlayers(gCurrent_net_game, (tNet_message*)local_c0, 0);
                    }
                    srand(uVar5);
                    local_c4++;
                } while (local_c4 < iVar4);
            }
        }
        break;
    }
    if (*pShrapnel_spec == 2 || *pShrapnel_spec == 1) {
        PipeSingleShrapnelShower(pArg9, pTime, pSeed, pOwner, (br_vector3*)pArg4,
                pPosition, pDistance, (br_vector3*)pArg5, (br_vector3*)pArg6,
                (undefined4)pShrapnel_spec, (br_bounds3*)pFragment_owner, pArg7,
                *pShrapnel_spec == 2 ? &basis : pTransform,
                (br_vector3*)pArg14, &gMatrix34_006b78a0);
        gCount_shrapnel_shower_calls++;
    }
#else
    __asm {
            sub esp, 0xc8
            push ebx
            push ebp
            push esi
            push edi
            mov edi, edx
            mov ebp, ecx
            mov edx, dword ptr [gPed_676914]
            mov dword ptr [esp + 0x1c], ebp
            test edx, edx
            je L004ec153
            mov eax, dword ptr [esp + 0xf8]
            lea ecx, [esp + 0x38]
            push eax
            push ecx
            call BrMatrix34Copy
            mov edx, dword ptr [gPed_676914]
            add esp, 8
    L004eb92b:
            mov ecx, dword ptr [gINT_006a3334]
    L004eb931:
            mov eax, dword ptr [edi]
            mov ebx, dword ptr [esp + 0xf0]
            cmp eax, 3
            ja L004ec079
            jmp dword ptr [eax*4 + 0x4ec35c]
            mov ecx, dword ptr [gYon_squared]
            test edx, edx
            mov dword ptr [esp + 0x10], ecx
            jne L004eb96f
            mov edx, ecx
            mov ecx, dword ptr [esp + 0xe8]
            push edx
            call PointOutOfSight
            test eax, eax
            jne L004ec079
    L004eb96f:
            mov eax, dword ptr [esp + 0x100]
            test eax, eax
            jne L004eb97f
            mov eax, offset gVector3_00661678
    L004eb97f:
            fld dword ptr [esp + 0x104]
            fsqrt
            mov edx, dword ptr [edi + 0x38]
            lea ecx, [edi + 0x3c]
            push ecx
            push edx
            push ecx
            mov ecx, dword ptr [esp + 0xf4]
            mov edx, offset gZero_vector__smash
            fmul dword ptr [gFloat_0058aee0]
            _emit 0xd9
        _emit 0x1c
        _emit 0x24
            push eax
            call FUN_004f8b50
            jmp L004ec079
            mov eax, dword ptr [gYon_squared]
            test edx, edx
            mov dword ptr [esp + 0x10], eax
            jne L004eb9d5
            mov ecx, eax
            push ecx
            mov ecx, dword ptr [esp + 0xec]
            call PointOutOfSight
            test eax, eax
            jne L004ec079
    L004eb9d5:
            test ebp, ebp
            je L004eb9de
            mov esi, dword ptr [ebp + 0x70]
            jmp L004eb9e0
    L004eb9de:
            xor esi, esi
    L004eb9e0:
            test ebp, ebp
            je L004eb9e9
            mov edx, dword ptr [ebp + 0x6c]
            jmp L004eb9eb
    L004eb9e9:
            xor edx, edx
    L004eb9eb:
            test ebp, ebp
            je L004eb9f4
            mov ecx, dword ptr [ebp + 0x78]
            jmp L004eb9f6
    L004eb9f4:
            xor ecx, ecx
    L004eb9f6:
            test ebp, ebp
            je L004eb9ff
            mov eax, dword ptr [ebp + 0x74]
            jmp L004eba01
    L004eb9ff:
            xor eax, eax
    L004eba01:
            mov ebp, dword ptr [esp + 0xf8]
            push ebp
            mov ebp, dword ptr [esp + 0xf8]
            push ebp
            lea ebp, [edi + 0x20]
            push ebp
            lea ebp, [edi + 4]
            push ebp
            mov ebp, dword ptr [edi + 0x3c]
            push ebp
            mov ebp, dword ptr [edi + 0x40]
            push ebp
            mov ebp, dword ptr [edi + 0x38]
            push ebp
            push esi
            push edx
            mov edx, dword ptr [esp + 0x110]
            push ecx
            mov ecx, dword ptr [esp + 0x104]
            push eax
            push ebx
            push edx
            mov edx, dword ptr [esp + 0x114]
            call CreateSmashedGlassFragments
            jmp L004ec075
            mov eax, dword ptr [gYon_squared]
            test edx, edx
            mov dword ptr [esp + 0x10], eax
            jne L004eba74
            mov ecx, eax
            push ecx
            mov ecx, dword ptr [esp + 0xec]
            call PointOutOfSight
            test eax, eax
            jne L004ec079
            mov ecx, dword ptr [gINT_006a3334]
    L004eba74:
            neg ecx
            sbb ecx, ecx
            lea edx, [esp + 0x38]
            and ecx, edx
            mov ebp, ecx
            mov dword ptr [esp + 0x10], ebp
            call rand
            mov dword ptr [esp + 0x30], eax
            mov eax, dword ptr [esp + 0xdc]
            push eax
            call srand
            mov ecx, dword ptr [edi + 0x38]
            add esp, 4
            xor edx, edx
            test ecx, ecx
            setl dl
            mov esi, edx
            test esi, esi
            mov dword ptr [esp + 0x34], esi
            je L004ebab6
            mov eax, dword ptr [edi + 0x40]
            jmp L004ebabe
    L004ebab6:
            mov edx, dword ptr [edi + 0x3c]
            call IRandomBetween
    L004ebabe:
            test ebp, ebp
            mov dword ptr [esp + 0x18], eax
            je L004ebc18
            fld dword ptr [edi + 0x44]
            fcomp dword ptr [gFloat_0058aec0]
            fnstsw ax
            test ah, 0x40
            jne L004ebc18
            fld dword ptr [edi + 0x48]
            fcomp dword ptr [gFloat_0058aec0]
            fnstsw ax
            test ah, 0x40
            jne L004ebc18
            fld dword ptr [gMatrix34_006b78a0+0x10]
            fld dword ptr [gMatrix34_006b78a0+0xc]
            fsub dword ptr [gMatrix34_006b78a0]
            fxch st(1)
            fsub dword ptr [gMatrix34_006b78a0+0x4]
            fld dword ptr [gMatrix34_006b78a0+0x14]
            fsub dword ptr [gMatrix34_006b78a0+0x8]
            fxch st(2)
            fst dword ptr [esp + 0x20]
            fld st(1)
            fxch st(1)
            fmul dword ptr [esp + 0x20]
            fld dword ptr [gMatrix34_006b78a0+0x18]
            fxch st(2)
            fmul st(0), st(3)
            fld dword ptr [gMatrix34_006b78a0+0x1c]
            fxch st(5)
            fst dword ptr [esp + 0x28]
            fxch st(3)
            fsub dword ptr [gMatrix34_006b78a0]
            fxch st(5)
            fsub dword ptr [gMatrix34_006b78a0+0x4]
            fxch st(3)
            fmul dword ptr [esp + 0x28]
            fld dword ptr [gMatrix34_006b78a0+0x20]
            fxch st(6)
            fst dword ptr [esp + 0x20]
            fxch st(4)
            fst dword ptr [esp + 0x24]
            fxch st(6)
            fsub dword ptr [gMatrix34_006b78a0+0x8]
            fst dword ptr [esp + 0x28]
            fxch st(2)
            faddp st(3), st(0)
            fxch st(3)
            fmul dword ptr [esp + 0x20]
            fxch st(3)
            faddp st(2), st(0)
            fxch st(4)
            fmul dword ptr [esp + 0x24]
            fxch st(1)
            fsqrt
            fxch st(1)
            faddp st(2), st(0)
            fst dword ptr [esp + 0x10]
            fst dword ptr [esp + 0x10]
            fcomp qword ptr [gDouble_0058aee8]
            fxch st(2)
            fmul dword ptr [esp + 0x28]
            faddp st(2), st(0)
            fxch st(1)
            fnstsw ax
            fsqrt
            test ah, 1
            fstp st(1)
            fstp dword ptr [esp + 0x14]
            jne L004ec075
            fld dword ptr [esp + 0x14]
            fcomp qword ptr [gDouble_0058aee8]
            fnstsw ax
            test ah, 1
            jne L004ec075
            fld dword ptr [esp + 0x14]
            fdiv dword ptr [edi + 0x48]
            push 0x3f800000
            push ecx
            _emit 0xd9
        _emit 0x1c
        _emit 0x24
            fld dword ptr [esp + 0x18]
            fdiv dword ptr [edi + 0x44]
            push ecx
            lea ecx, [esp + 0x84]
            _emit 0xd9
        _emit 0x1c
        _emit 0x24
            push ecx
            call BrMatrix34Scale
            add esp, 0x10
            lea edx, [esp + 0x78]
            lea eax, [esp + 0xa8]
            push ebp
            push edx
            push eax
            call BrMatrix34Mul
            lea ecx, [esp + 0xb4]
            add esp, 0xc
            mov dword ptr [esp + 0x10], ecx
    L004ebc18:
            mov eax, dword ptr [esp + 0x18]
            xor ebp, ebp
            test eax, eax
            jle L004ebd04
            jmp L004ebc2c
    L004ebc28:
            mov esi, dword ptr [esp + 0x34]
    L004ebc2c:
            test esi, esi
            je L004ebc3c
            mov edx, dword ptr [edi + 0x4c]
            mov eax, dword ptr [edx + ebp*4]
            mov dword ptr [esp + 0x14], eax
            jmp L004ebc51
    L004ebc3c:
            mov edx, dword ptr [edi + 0x40]
            xor ecx, ecx
            dec edx
            call IRandomBetween
            mov ecx, dword ptr [edi + 0x4c]
            mov edx, dword ptr [ecx + eax*4]
            mov dword ptr [esp + 0x14], edx
    L004ebc51:
            mov eax, dword ptr [esp + 0x14]
            mov edx, dword ptr [esp + 0xf4]
            push 4
            mov ecx, dword ptr [eax + 0x18]
            call FindShrapnelFragment
            mov esi, eax
            test esi, esi
            je L004ebd04
            mov ecx, dword ptr [esp + 0x10]
            mov edx, dword ptr [esp + 0xfc]
            mov eax, dword ptr [esp + 0xec]
            push ecx
            mov ecx, dword ptr [esp + 0xec]
            push edx
            mov edx, dword ptr [esp + 0xec]
            push eax
            mov eax, dword ptr [esp + 0x20]
            push ecx
            mov ecx, dword ptr [esi]
            push edx
            push eax
            lea edx, [edi + 0x28]
            call PositionFragment
            mov ecx, dword ptr [esi]
            fstp dword ptr [esp + 0x14]
            mov edx, dword ptr [esp + 0x108]
            mov eax, dword ptr [esp + 0x14]
            add ecx, 0x50
            push ecx
            push edx
            push offset gZero_vector__smash
            lea ecx, [esi + 0x28]
            push eax
            lea edx, [esi + 0x1c]
            push ecx
            push edx
            mov edx, dword ptr [esp + 0xf8]
            lea ecx, [edi + 4]
            call ApplyInitialMovement
            mov dword ptr [esi + 0xc], ebx
            mov dword ptr [esi + 0x14], ebx
            mov ecx, dword ptr [edi + 0x24]
            mov eax, ebx
            add eax, ecx
            mov dword ptr [esi + 8], eax
            mov edx, dword ptr [edi + 0x24]
            mov ecx, dword ptr [edi + 0x20]
            call IRandomBetween
            add eax, ebx
            inc ebp
            mov dword ptr [esi + 0x10], eax
            mov eax, dword ptr [esp + 0x18]
            cmp ebp, eax
            jl L004ebc28
    L004ebd04:
            mov ecx, dword ptr [esp + 0x30]
            push ecx
            call srand
            add esp, 4
            jmp L004ec075
            mov eax, dword ptr [gNet_mode]
            lea edx, [esp + 0x38]
            neg ecx
            sbb ecx, ecx
            xor esi, esi
            and ecx, edx
            cmp eax, 3
            mov dword ptr [esp + 0x34], ecx
            mov dword ptr [esp + 0x18], esi
            jne L004ebd41
            mov eax, dword ptr [gINT_006a8294]
            test eax, eax
            je L004ec079
    L004ebd41:
            mov ecx, dword ptr [edi + 0x38]
            xor eax, eax
            test ecx, ecx
            setl al
            test eax, eax
            mov dword ptr [esp + 0x30], eax
            je L004ebd58
            mov eax, dword ptr [edi + 0x40]
            jmp L004ebd60
    L004ebd58:
            mov edx, dword ptr [edi + 0x3c]
            call IRandomBetween
    L004ebd60:
            test eax, eax
            mov dword ptr [esp + 0x10], eax
            mov dword ptr [esp + 0x14], 0
            jle L004ec079
    L004ebd74:
            call rand
            mov ecx, dword ptr [gMax_count_non_cars]
            mov ebp, eax
            mov eax, dword ptr [gProgram_state+0x1bbc]
            mov dword ptr [esp + 0x68], ebp
            cmp eax, ecx
            jge L004ec075
            cmp dword ptr [gNet_mode], 2
            jne L004ebdb8
            xor edx, edx
            mov cl, 0x22
            call NetBuildGuaranteedMessage
            mov esi, eax
            mov dword ptr [esp + 0x18], esi
            mov dword ptr [esi + 0x1c], ebp
            mov cx, word ptr [gProgram_state+0x1bbc]
            mov word ptr [esi + 0x20], cx
    L004ebdb8:
            mov eax, dword ptr [esp + 0x30]
            test eax, eax
            je L004ebdcc
            mov edx, dword ptr [edi + 0x44]
            mov eax, dword ptr [esp + 0x14]
            mov eax, dword ptr [edx + eax*4]
            jmp L004ebddd
    L004ebdcc:
            mov edx, dword ptr [edi + 0x40]
            xor ecx, ecx
            dec edx
            call IRandomBetween
            mov ecx, dword ptr [edi + 0x44]
            mov eax, dword ptr [ecx + eax*4]
    L004ebddd:
            cmp dword ptr [gNet_mode], 2
            jne L004ebdf1
            mov edx, dword ptr [eax + 0x14]
            mov cx, word ptr [edx + 1]
            mov word ptr [esi + 0x22], cx
    L004ebdf1:
            mov ecx, eax
            call CloneActor
            mov edx, dword ptr [gProgram_state+0x1bbc]
            mov esi, eax
            push edx
            push offset gMatchString_00660084
            mov eax, dword ptr [esi + 0x14]
            add eax, 4
            push eax
            call sprintf
            mov ecx, dword ptr [gProgram_state+0x1bbc]
            mov edx, dword ptr [gProgram_state+0x1bc0]
            add esp, 0xc
            mov dword ptr [edx + ecx*4], esi
            mov eax, dword ptr [gProgram_state+0x1bbc]
            inc eax
            push esi
            mov dword ptr [gProgram_state+0x1bbc], eax
            mov eax, dword ptr [gDont_render_actor]
            push eax
            call BrActorAdd
            mov ecx, dword ptr [esp + 0x3c]
            mov edx, dword ptr [esp + 0x104]
            mov eax, dword ptr [esp + 0xf4]
            add esp, 8
            push ecx
            mov ecx, dword ptr [esp + 0xec]
            push edx
            mov edx, dword ptr [esp + 0xec]
            push eax
            push ecx
            push edx
            push esi
            lea edx, [edi + 0x28]
            mov ecx, esi
            call PositionFragment
            mov eax, dword ptr [esi + 0x5c]
            fstp dword ptr [esp + 0x2c]
            test eax, eax
            jne L004ec058
            mov ecx, esi
            call DoPullActorFromWorld
            mov ebp, eax
            test ebp, ebp
            je L004ec058
            call rand
            mov ecx, dword ptr [esp + 0x2c]
            lea edx, [esi + 0x50]
            push edx
            mov edx, dword ptr [esp + 0x10c]
            push edx
            push offset gZero_vector__smash
            lea edx, [esp + 0x78]
            push ecx
            lea ebx, [esp + 0x30]
            push edx
            mov edx, dword ptr [esp + 0xf4]
            lea ecx, [edi + 4]
            push ebx
            mov dword ptr [esp + 0x44], eax
            call ApplyInitialMovement
            fld dword ptr [esp + 0x20]
            fmul qword ptr [gDouble_0058aeb8]
            mov ecx, dword ptr [ebp + 8]
            mov eax, dword ptr [esp + 0x68]
            push eax
            fstp dword ptr [ecx + 0x68]
            fld dword ptr [esp + 0x28]
            fmul qword ptr [gDouble_0058aeb8]
            mov edx, dword ptr [ebp + 8]
            fstp dword ptr [edx + 0x6c]
            fld dword ptr [esp + 0x2c]
            fmul qword ptr [gDouble_0058aeb8]
            mov eax, dword ptr [ebp + 8]
            fstp dword ptr [eax + 0x70]
            fld dword ptr [esp + 0x70]
            fmul qword ptr [gDouble_0058aef0]
            mov ecx, dword ptr [ebp + 8]
            fstp dword ptr [ecx + 0x74]
            fld dword ptr [esp + 0x74]
            fmul qword ptr [gDouble_0058aef0]
            mov edx, dword ptr [ebp + 8]
            fstp dword ptr [edx + 0x78]
            fld dword ptr [esp + 0x78]
            fmul qword ptr [gDouble_0058aef0]
            mov eax, dword ptr [ebp + 8]
            fstp dword ptr [eax + 0x7c]
            mov ecx, dword ptr [ebp + 8]
            mov byte ptr [ecx + 0xec], 0
            call srand
            mov ecx, dword ptr [edi + 0x48]
            add esp, 4
            call PercentageChance
            test eax, eax
            je L004ebf91
            mov eax, dword ptr [edi + 0x4c]
            xor ebx, ebx
            test eax, eax
            jle L004ebf91
    L004ebf52:
            mov edx, dword ptr [esi + 0x18]
            push 0x989680
            mov eax, dword ptr [edx + 0x4c]
            xor edx, edx
            mov ecx, dword ptr [eax + 0x18]
            mov dx, word ptr [ecx + 0x1e]
            xor ecx, ecx
            call IRandomBetween
            mov edx, dword ptr [edi + 0x54]
            mov ecx, dword ptr [edi + 0x50]
            push eax
            call IRandomBetween
            mov edx, esi
            mov ecx, 2
            push eax
            push 0
            push ebp
            call CreateSmokeColumn2
            mov eax, dword ptr [edi + 0x4c]
            inc ebx
            cmp ebx, eax
            jl L004ebf52
    L004ebf91:
            mov eax, dword ptr [gNet_mode]
            mov ebx, 2
            cmp eax, ebx
            jne L004ec005
            add esi, 0x2c
            push esi
            mov esi, dword ptr [esp + 0x1c]
            lea edx, [esi + 0x3c]
            push edx
            call BrMatrix34Copy
            mov eax, dword ptr [ebp + 8]
            add esp, 8
            mov ecx, dword ptr [eax + 0x68]
            mov dword ptr [esi + 0x24], ecx
            mov edx, dword ptr [ebp + 8]
            mov eax, dword ptr [edx + 0x6c]
            mov dword ptr [esi + 0x28], eax
            mov ecx, dword ptr [ebp + 8]
            mov edx, dword ptr [ecx + 0x70]
            mov dword ptr [esi + 0x2c], edx
            mov eax, dword ptr [ebp + 8]
            mov ecx, dword ptr [eax + 0x74]
            mov dword ptr [esi + 0x30], ecx
            mov edx, dword ptr [ebp + 8]
            mov eax, dword ptr [edx + 0x78]
            mov dword ptr [esi + 0x34], eax
            mov ecx, dword ptr [ebp + 8]
            lea eax, [esi + 0x6c]
            mov edx, dword ptr [ecx + 0x7c]
            mov dword ptr [esi + 0x38], edx
            mov ecx, dword ptr [edi + 0x48]
            mov dword ptr [eax], ecx
            mov edx, dword ptr [edi + 0x4c]
            mov dword ptr [eax + 4], edx
            mov ecx, dword ptr [edi + 0x50]
            mov dword ptr [eax + 8], ecx
            mov edx, dword ptr [edi + 0x54]
            mov dword ptr [eax + 0xc], edx
            jmp L004ec009
    L004ec005:
            mov esi, dword ptr [esp + 0x18]
    L004ec009:
            mov eax, dword ptr [gProgram_state+0x1bbc]
            lea ecx, [eax - 1]
            call FUN_004c8940
            cmp dword ptr [gNet_mode], ebx
            jne L004ec02d
            mov ecx, dword ptr [gCurrent_net_game]
            push 0
            mov edx, esi
            call NetGuaranteedSendMessageToAllPlayers
    L004ec02d:
            mov ecx, dword ptr [esp + 0x2c]
            push ecx
            call srand
            mov eax, dword ptr [esp + 0x18]
            mov ecx, dword ptr [esp + 0x14]
            mov ebx, dword ptr [esp + 0xf4]
            add esp, 4
            inc eax
            cmp eax, ecx
            mov dword ptr [esp + 0x14], eax
            jl L004ebd74
            jmp L004ec075
    L004ec058:
            push esi
            call BrActorRemove
            add esp, 4
            push esi
            call BrActorFree
            mov eax, dword ptr [gProgram_state+0x1bbc]
            add esp, 4
            dec eax
            mov dword ptr [gProgram_state+0x1bbc], eax
    L004ec075:
            mov ebp, dword ptr [esp + 0x1c]
    L004ec079:
            mov eax, dword ptr [edi]
            cmp eax, 2
            jne L004ec0d6
            mov edx, dword ptr [esp + 0x108]
            mov ecx, dword ptr [esp + 0xec]
            push offset gMatrix34_006b78a0
            lea eax, [esp + 0x3c]
            push edx
            mov edx, dword ptr [esp + 0x104]
            push eax
            mov eax, dword ptr [esp + 0xf4]
            push ecx
            mov ecx, dword ptr [esp + 0xf4]
            push edx
            mov edx, dword ptr [esp + 0x118]
            push edi
            push eax
            mov eax, dword ptr [esp + 0x11c]
            push ecx
            mov ecx, dword ptr [esp + 0x100]
            push edx
            mov edx, dword ptr [esp + 0x100]
            push eax
            push ecx
            push ebp
            push edx
            jmp L004ec132
    L004ec0d6:
            cmp eax, 1
            jne L004ec146
            mov eax, dword ptr [esp + 0x108]
            mov ecx, dword ptr [esp + 0xf8]
            mov edx, dword ptr [esp + 0xec]
            push offset gMatrix34_006b78a0
            push eax
            mov eax, dword ptr [esp + 0x104]
            push ecx
            mov ecx, dword ptr [esp + 0xf4]
            push edx
            mov edx, dword ptr [esp + 0xf4]
            push eax
            mov eax, dword ptr [esp + 0x118]
            push edi
            push ecx
            mov ecx, dword ptr [esp + 0x11c]
            push edx
            mov edx, dword ptr [esp + 0x100]
            push eax
            mov eax, dword ptr [esp + 0x100]
            push ecx
            push edx
            push ebp
            push eax
    L004ec132:
            mov ecx, dword ptr [esp + 0x128]
            mov edx, ebx
            call PipeSingleShrapnelShower
            inc dword ptr [gCount_shrapnel_shower_calls]
    L004ec146:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0xc8
            ret 0x34
    L004ec153:
            mov eax, dword ptr [edi]
            cmp eax, 2
            je L004ec163
            cmp eax, 3
            jne L004eb92b
    L004ec163:
            cmp dword ptr [edi + 0x28], 2
            jne L004eb92b
            mov ecx, dword ptr [gINT_006a3334]
            test ecx, ecx
            je L004eb931
            fld dword ptr [gMatrix34_006b78a0+0x10]
            fld dword ptr [gMatrix34_006b78a0+0xc]
            fsub dword ptr [gMatrix34_006b78a0]
            fxch st(1)
            fsub dword ptr [gMatrix34_006b78a0+0x4]
            fld dword ptr [gMatrix34_006b78a0+0x14]
            fsub dword ptr [gMatrix34_006b78a0+0x8]
            fld st(2)
            fld st(2)
            fxch st(1)
            fmul st(0), st(4)
            fxch st(1)
            fmul st(0), st(3)
            fld st(2)
            fmul st(0), st(3)
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fsqrt
            fcom dword ptr [gFloat_0058aec4]
            fnstsw ax
            test ah, 0x41
            jne L004ec1f4
            fdivr qword ptr [gDouble_0058aec8]
            fld st(0)
            fld st(1)
            fxch st(1)
            fmul st(0), st(5)
            fxch st(1)
            fmul st(0), st(4)
            fxch st(2)
            fmul st(0), st(3)
            fxch st(1)
            fstp dword ptr [esp + 0x38]
            fxch st(1)
            fstp dword ptr [esp + 0x3c]
            fstp dword ptr [esp + 0x40]
            fstp st(0)
            fstp st(0)
            fstp st(0)
            jmp L004ec214
    L004ec1f4:
            fstp st(0)
            fstp st(0)
            fstp st(0)
            fstp st(0)
            mov dword ptr [esp + 0x38], 0x3f800000
            mov dword ptr [esp + 0x3c], 0
            mov dword ptr [esp + 0x40], 0
    L004ec214:
            fld dword ptr [gMatrix34_006b78a0+0x1c]
            fld dword ptr [gMatrix34_006b78a0+0x18]
            fsub dword ptr [gMatrix34_006b78a0]
            fxch st(1)
            fsub dword ptr [gMatrix34_006b78a0+0x4]
            fld dword ptr [gMatrix34_006b78a0+0x20]
            fsub dword ptr [gMatrix34_006b78a0+0x8]
            fld st(2)
            fld st(2)
            fxch st(1)
            fmul st(0), st(4)
            fxch st(1)
            fmul st(0), st(3)
            fld st(2)
            fmul st(0), st(3)
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fsqrt
            fcom dword ptr [gFloat_0058aec4]
            fnstsw ax
            test ah, 0x41
            jne L004ec28d
            fdivr qword ptr [gDouble_0058aec8]
            fld st(0)
            fld st(1)
            fxch st(1)
            fmul st(0), st(5)
            fxch st(1)
            fmul st(0), st(4)
            fxch st(2)
            fmul st(0), st(3)
            fxch st(1)
            fstp dword ptr [esp + 0x44]
            fxch st(1)
            fstp dword ptr [esp + 0x48]
            fstp dword ptr [esp + 0x4c]
            fstp st(0)
            fstp st(0)
            fstp st(0)
            jmp L004ec2ad
    L004ec28d:
            fstp st(0)
            fstp st(0)
            fstp st(0)
            fstp st(0)
            mov dword ptr [esp + 0x44], 0x3f800000
            mov dword ptr [esp + 0x48], 0
            mov dword ptr [esp + 0x4c], 0
    L004ec2ad:
            fld dword ptr [gVector3_006a4688+0x4]
            fld dword ptr [gVector3_006a4688]
            fmul dword ptr [gVector3_006a4688]
            fld dword ptr [gVector3_006a4688+0x8]
            fxch st(2)
            fmul dword ptr [gVector3_006a4688+0x4]
            fxch st(2)
            fmul dword ptr [gVector3_006a4688+0x8]
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fsqrt
            fcom dword ptr [gFloat_0058aec4]
            fnstsw ax
            test ah, 0x41
            jne L004ec320
            fdivr qword ptr [gDouble_0058aec8]
            fld st(0)
            fld st(1)
            fxch st(1)
            fmul dword ptr [gVector3_006a4688]
            fxch st(1)
            fmul dword ptr [gVector3_006a4688+0x4]
            fxch st(2)
            fmul dword ptr [gVector3_006a4688+0x8]
            fxch st(1)
            fstp dword ptr [esp + 0x50]
            fxch st(1)
            fstp dword ptr [esp + 0x54]
            fstp dword ptr [esp + 0x58]
            jmp L004ec33a
    L004ec320:
            fstp st(0)
            mov dword ptr [esp + 0x50], 0x3f800000
            mov dword ptr [esp + 0x54], 0
            mov dword ptr [esp + 0x58], 0
    L004ec33a:
            mov eax, dword ptr [gMatrix34_006b78a0+0xc]
            mov dword ptr [esp + 0x5c], eax
            mov eax, dword ptr [gMatrix34_006b78a0+0x10]
            mov dword ptr [esp + 0x60], eax
            mov eax, dword ptr [gMatrix34_006b78a0+0x14]
            mov dword ptr [esp + 0x64], eax
            jmp L004eb931
            mov edi, edi
            _emit 0x4a
            _emit 0xb9
            _emit 0x4e
            _emit 0x00
            _emit 0xb1
            _emit 0xb9
            _emit 0x4e
            _emit 0x00
            _emit 0x4a
            _emit 0xba
            _emit 0x4e
            _emit 0x00
            _emit 0x16
            _emit 0xbd
            _emit 0x4e
            _emit 0x00
            _emit 0x90
            _emit 0x90
            _emit 0x90
            _emit 0x90
    }
#endif
}

void C2_HOOK_FASTCALL WorkerShrapnelShowerUndo(tU32 chunk_0x48, tU32 prev_0x4, tU32 prev_0x84, tU32 prev_0x44, br_vector3* prev_0x8, br_vector3* prev_0x20, br_vector3* prev_0x2c, tU32 prev_0x50, tU32 prev_0x48, tU32 prev_0x0, br_vector3* prev_0x54, br_vector3* prev_0x88, br_vector3* prev_0x14, tU32 prev_0xa0, br_vector3* prev_0x38) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL WorkerSmoke(int index, tU8 flag, float f1, float f2, br_vector3* pVec) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL WorkerFlame(int index, int flag, float f1, float f2, float f3, float f4) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL WorkerPedDiagnostics(void* pOpponent, void* pField_0x4, void* pField_0x58, void* pField_0x144) {

    NOT_IMPLEMENTED();
}

typedef struct {
    tU32 subject_index;
    union {
        tPipe_smudge_data smudge;
    } data;
} tPipe_chunk_smudge_view;

// FUNCTION: CARMA2_HW 0x004c9070
void C2_HOOK_FASTCALL WorkerSmudge(tPipe_chunk** pChunk, int pScale) {
    int i;
    int v;
    tU8 inc;
    br_model* model_ptr;
    tCar_spec* car;

#define chunk (*(tPipe_chunk_smudge_view**)pChunk)

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_smudge_view, data, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_smudge_data, vertex_changes, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSmudged_vertex, light_index, 0x2);
    C2_HOOK_BUG_ON(sizeof(tSmudged_vertex) != 0x4);

    if ((chunk->subject_index >> 8) == eVehicle_self) {
        car = &gProgram_state.current_car;
    } else {
        car = GetCarSpec(chunk->subject_index >> 8, chunk->subject_index & 0xff);
    }
    model_ptr = car->car_actor->model;
    for (i = 0; i < chunk->data.smudge.vertex_count; i++) {
        v = chunk->data.smudge.vertex_changes[i].vertex_index;
        inc = chunk->data.smudge.vertex_changes[i].light_index * pScale;
        model_ptr->prepared->groups->vertex_colours[v] = ((model_ptr->prepared->groups->vertex_colours[v] & 0xff000000u) + ((br_colour)inc << 24)) & 0xff000000u;
        if (model_ptr->flags & BR_MODF_UPDATEABLE) {
            model_ptr->vertices[model_ptr->prepared->groups->vertex_user[v]].index = BR_ALPHA(model_ptr->prepared->groups->vertex_colours[v]) + inc;
        }
    }

#undef chunk
}
