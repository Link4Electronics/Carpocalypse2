#include "c2_hooks.h"
#include "carpocalypse2_types.h"
#include "world.h"
#include "opponent.h"

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

    NOT_IMPLEMENTED();
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

void C2_HOOK_FASTCALL WorkerPedPos(int field_0x0, br_vector3* pVec) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL WorkerNapalmBolt(int field_0x0, int field_0x4, br_vector3* pVec) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL WorkerPedDir(int field_0x0, br_vector3* pDir) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL WorkerPedPhysics(int field_0x0, br_matrix34* pMatrix) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL WorkerDroneCornerPos(int field_0x0, int field_0x4, tU16 field_0x6, tU16 field_0x8, tU16* pField_0xa) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL WorkerFlap(int field_0x4, tS16 field_0x8) {

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

void C2_HOOK_FASTCALL WorkerSmashTextureChange(int field_0x0, int field_0x4, int field_0xc, int field_0x10) {

    NOT_IMPLEMENTED();
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

void C2_HOOK_FASTCALL WorkerPedDismember(int field_0x0, tS16 field_0x4, tS8 field_0x7) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL WorkerPedDismemberUndo(int field_0x0, tS16 field_0x4, tS8 field_0x6) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL WorkerPedFPChange(int field_0x0, tPipe_stack_word field_0x4) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL WorkerPedModelChange(int field_0x0, tS16 field_0x4, tS16 field_0x6, tS16 field_0x8) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL WorkerPedModelChangeUndo(int field_0x0, tS16 field_0x4, int field_0xc, tS16 field_0xa) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL WorkerDroneStraightPos(int field_0x0, int field_0x4, tU16 field_0x8, tU16 field_0xa) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL WorkerRepulseRay(int field_0x0, int field_0x4, int field_0x8, int field_0xc) {

    NOT_IMPLEMENTED();
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
void C2_HOOK_FASTCALL WorkerCrush(int field_0x4, void* pField_0x8, void* pField_0xe) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL WorkerCrushUndo(int field_0x4, void* pField_0x8, void* pField_0xe) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL WorkerSpark(int field_0x0, void* pField_0x4, void* pField_0x10) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL WorkerShrapnel(int field_0x0, void* pField_0x4, tU16 field_0x10, int field_0x14) {

    NOT_IMPLEMENTED();
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

void C2_HOOK_FASTCALL WorkerPedMove(int field_0x0, int field_0x4, int field_0xc, int field_0x8_1, int field_0x14, int zero, br_vector3* pField_0x24, int field_0x34, br_vector3* pField_0x38) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL WorkerPedMoveUndo(int field_0x0, int field_0x6, int field_0x10, int zero, int zero2, int field_0xa, br_vector3* pField_0x18, int field_0x30, br_vector3* pField_0x38) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL WorkerSpecial(void) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL WorkerSpecialUndo(void) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL WorkerPedStatus(int field_0x0, tU8 field_0x4, tU8 field_0x5, tU8 field_0x6, tU8 field_0x7, tU8 field_0x9, tU8 field_0xb, br_vector3* p0xc, br_vector3* p0x18, br_vector3* p0x24) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL WorkerPedStatusUndo(int field_0x0, tU8 field_0x4, tU8 field_0x5, tU8 field_0x6, tU8 field_0x7, tU8 field_0x8, tU8 field_0xa, br_vector3* p0xc, br_vector3* p0x18, br_vector3* p0x24) {

    NOT_IMPLEMENTED();
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

void C2_HOOK_FASTCALL WorkerShrapnelShower(tU32 field_0x4, tU32 field_0x84, tU32 field_0x84b, tU32 field_0x44, br_vector3* field_0x8, br_vector3* field_0x20, br_vector3* field_0x2c, tU32 field_0x50, tU32 field_0x48, tU32 field_0x0, br_vector3* field_0x54, br_vector3* field_0x88, br_vector3* field_0x14, tU32 field_0xa0, br_vector3* field_0x38, tU32 field_0) {

    NOT_IMPLEMENTED();
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

// FUNCTION: CARMA2_HW 0x004c9070
void C2_HOOK_FASTCALL WorkerSmudge(tPipe_chunk** pChunk, int pScale) {
    tCar_spec* car;
    tU8* a;
    tU8* esi;
    unsigned int code;
    int i;

    code = *(unsigned int*)*pChunk;
    if ((code & 0xffffff00u) == 0) car = (tCar_spec*)0x75bc2c; else car = GetCarSpec(code >> 8, code & 0xffu);
    a = *(tU8**)((tU8*)car + 0xe10);
    i = 0;
    if (*(tU16*)((tU8*)*pChunk + 4) <= 0) return;
    esi = *(tU8**)(a + 0x18);
    do {
        tU32* colors = *(tU32**)(*(tU8**)(*(tU8**)(esi + 0x4c) + 0x18) + 0x14);
        tU16 idx = *(tU16*)((tU8*)*pChunk + i * 4 + 0xc);
        tS8 bright = (tS8)((tS8)*(tU8*)((tU8*)*pChunk + i * 4 + 0xe) * (tS8)pScale);
        tU32 alpha24 = ((tU32)(tU8)bright) << 24;
        tU32 c = colors[idx];

        colors[idx] = (c & 0xff000000) + alpha24 & 0xff000000;
        if (*(tU8*)(esi + 0x20) & 0x80) {
            tU8* q = *(tU8**)(esi + 0x4c);
            tU8* w = *(tU8**)(q + 0x18);
            tU32* colors2 = (tU32*)(*(tU8**)(w + 0x14));
            tU16* idx2 = (tU16*)(*(tU8**)(w + 0x18));
            tU8 dl = (tU8)(colors2[idx] >> 24) + (tU8)bright;
            tU8* arr = *(tU8**)(esi + 8);
            tU16 ix = idx2[idx];

            arr[(tU32)ix * 40 + 0x14] = dl;
        }
        i++;
    } while (i < *(tU16*)((tU8*)*pChunk + 4));
}
