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
void C2_HOOK_FASTCALL WorkerPedDismember(int field_0x0, int field_0x4, int field_0x7) {
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

// C2PORT-NONMATCHING: best 33.84% | difftest PASS 2000 trials (spec difftest/specs/004eb8f0_WorkerShrapnelShower.json) | reason stack-layout/register-allocation
// FUNCTION: CARMA2_HW 0x004eb8f0
void C2_HOOK_FASTCALL WorkerShrapnelShower(int pOwner, int* pShrapnel_spec, tU32 pSeed, undefined4 pArg4, undefined4 pArg5, undefined4 pArg6, undefined4 pArg7, int pTime, undefined4 pArg9, br_matrix34* pTransform, undefined4 pFragment_owner, br_vector3* pPosition, float pDistance, undefined4 pArg14, undefined4 pArg15) {
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
