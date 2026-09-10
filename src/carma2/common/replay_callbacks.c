#include "replay_callbacks.h"

#include "crush.h"
#include "flap.h"
#include "globvars.h"
#include "piping.h"
#include "powerups.h"
#include "replay.h"
#include "sound.h"
#include "spark.h"
#include "pedestrn.h"
#include "graphics1.h"
#include "graphics.h"
#include "utility.h"

#include "carpocalypse2_macros.h"
#include "world.h"
#include "c2_math.h"

// Unmapped replay pipe worker helpers (orig PDB has no symbols for these;
// they exist only so the wrappers can emit their call/jmp operands).
extern void C2_HOOK_FASTCALL WorkerFrameBoundary(int field_0x4);
extern void C2_HOOK_FASTCALL WorkerGrooveOff(br_actor* pActor, tU32 pBlock_flags);
extern void C2_HOOK_FASTCALL WorkerFunkEnable(int field_0x0, tU16 field_0x4);
extern void C2_HOOK_FASTCALL WorkerPowerupGot(int field_0x0, int field_0x4);
extern void C2_HOOK_FASTCALL WorkerPowerupLose(int field_0x0, int field_0x4);
extern void C2_HOOK_FASTCALL WorkerPowerupRespawn(int field_0x0, int field_0x4);
extern void C2_HOOK_FASTCALL WorkerPowerupRespawnUndo(int field_0x0, int field_0x4);
extern void C2_HOOK_FASTCALL WorkerShitMine(int field_0x0, br_vector3* pPos);
extern void C2_HOOK_FASTCALL WorkerEndShitMine(int field_0x0, br_vector3* pPos);
extern void C2_HOOK_FASTCALL WorkerPedPos(int field_0x0, br_vector3* pVec);
extern void C2_HOOK_FASTCALL WorkerNapalmBolt(int field_0x0, int field_0x4, br_vector3* pVec);
extern void C2_HOOK_FASTCALL WorkerPedDir(int field_0x0, br_vector3* pDir);
extern void C2_HOOK_FASTCALL WorkerPedPhysics(int field_0x0, br_matrix34* pMatrix);
extern void C2_HOOK_FASTCALL WorkerDroneCornerPos(int field_0x0, int field_0x4, tU16 field_0x6, tU16 field_0x8, tU16* pField_0xa);
extern void C2_HOOK_FASTCALL WorkerFlap(int field_0x4, tS16 field_0x8);
extern void C2_HOOK_FASTCALL WorkerModelMash(int field_0x4, tU8* pField_0x8);
extern void C2_HOOK_FASTCALL WorkerModelMashUndo(int field_0x4, tU8* pField_0x8);
extern void C2_HOOK_FASTCALL WorkerExplosion(int field_0x0, br_vector3* pField_0x4);
extern void C2_HOOK_FASTCALL WorkerNonCarCreation(int field_0x0);
extern void C2_HOOK_FASTCALL WorkerNonCarCreationUndo(int field_0x0);
extern void C2_HOOK_FASTCALL WorkerSmashRemoveFaces(int field_0x0, int count);
extern void C2_HOOK_FASTCALL WorkerSmashDecal(int field_0x0);
extern void C2_HOOK_FASTCALL WorkerPHILObject(int field_0x0, br_matrix34* pField_0x4);
extern void C2_HOOK_FASTCALL WorkerNonCar(int field_0x4, br_vector3* pField_0x8);
extern void C2_HOOK_FASTCALL WorkerSmokeColumn(int column_flags, br_vector3* pPos, tU16 vertex, int pFlags_hi);
extern void C2_HOOK_FASTCALL WorkerSmashTextureChange(int field_0x0, int field_0x4, int field_0xc, int field_0x10);
extern void C2_HOOK_FASTCALL WorkerUndoSmashRemoveFaces(int field_0x0, int field_0x8, void* pField_0xc, int field_0x4);
extern void C2_HOOK_FASTCALL WorkerSmashDecalUndo(int field_0x0);
extern void C2_HOOK_FASTCALL WorkerExtendedSplash(int field_0x0, tU8 field_0x4, int field_0x8, br_vector3* pField_0xc, br_vector3* pField_0x18);
extern void C2_HOOK_FASTCALL WorkerPedDismember(int field_0x0, int field_0x4, int field_0x7);
extern void C2_HOOK_FASTCALL WorkerPedDismemberUndo(int field_0x0, int field_0x4, int field_0x6);
extern void C2_HOOK_FASTCALL WorkerPedFPChange(int field_0x0, tPipe_stack_word field_0x4);
extern void C2_HOOK_FASTCALL WorkerPedModelChange(int field_0x0, int field_0x4, int field_0x6, int field_0x8);
extern void C2_HOOK_FASTCALL WorkerPedModelChangeUndo(int field_0x0, int field_0x4, int field_0xc, int field_0xa);
extern void C2_HOOK_FASTCALL WorkerDroneStraightPos(int field_0x0, int field_0x4, tU16 field_0x8, tU16 field_0xa);
extern void C2_HOOK_FASTCALL WorkerRepulseRay(int field_0x0, int field_0x4, int field_0x8, int field_0xc);
extern void C2_HOOK_FASTCALL WorkerSmashModelChangeApply(int field_0x0, int field_0x4, int field_0x8);
extern void C2_HOOK_FASTCALL WorkerSmashModelChangeB(int field_0x0);
extern void C2_HOOK_FASTCALL WorkerSmashModelChangeBUndo(int field_0x0);
extern void C2_HOOK_FASTCALL WorkerCrush(int field_0x4, void* pField_0x8, void* pField_0xe);
extern void C2_HOOK_FASTCALL WorkerCrushUndo(int field_0x4, void* pField_0x8, void* pField_0xe);
extern void C2_HOOK_FASTCALL WorkerSpark(int field_0x0, void* pField_0x4, void* pField_0x10);
extern void C2_HOOK_FASTCALL WorkerShrapnel(int field_0x0, void* pField_0x4, tU16 field_0x10, int field_0x14);
extern void C2_HOOK_FASTCALL WorkerProxRay(int field_0x0, tU16 field_0x6, tU16 field_0x4, int field_0x8);
extern void C2_HOOK_FASTCALL WorkerGrooveStop(int field_0x0, br_vector3* pField_0x14, int field_0x4, int field_0x8, int field_0xc, int field_0x10);
extern void C2_HOOK_FASTCALL WorkerOilSpill(int field_0x0, br_vector3* pField_0x4, int field_0x34, int field_0x38, int field_0x3c, int zero, int field_0x44, br_vector3* pField_0x48, int field_0x54);
extern void C2_HOOK_FASTCALL WorkerGibShower(int field_0x0, int field_0xc, int field_0x8, int field_0x10, int field_0x4, br_vector3* pField_0x14, br_vector3* pField_0x20, br_vector3* pField_0x2c);
extern void C2_HOOK_FASTCALL WorkerPedMove(int field_0x0, int field_0x4, int field_0xc, int field_0x8_1, int field_0x14, int zero, br_vector3* pField_0x24, int field_0x34, br_vector3* pField_0x38);
extern void C2_HOOK_FASTCALL WorkerPedMoveUndo(int field_0x0, int field_0x6, int field_0x10, int zero, int zero2, int field_0xa, br_vector3* pField_0x18, int field_0x30, br_vector3* pField_0x38);
extern void C2_HOOK_FASTCALL WorkerSpecial(void);
extern void C2_HOOK_FASTCALL WorkerSpecialUndo(void);
extern void C2_HOOK_FASTCALL WorkerPedStatus(int field_0x0, tU8 field_0x4, tU8 field_0x5, tU8 field_0x6, tU8 field_0x7, tU8 field_0x9, tU8 field_0xb, br_vector3* p0xc, br_vector3* p0x18, br_vector3* p0x24);
extern void C2_HOOK_FASTCALL WorkerPedStatusUndo(int field_0x0, tU8 field_0x4, tU8 field_0x5, tU8 field_0x6, tU8 field_0x7, tU8 field_0x8, tU8 field_0xa, br_vector3* p0xc, br_vector3* p0x18, br_vector3* p0x24);
extern void C2_HOOK_FASTCALL WorkerGibShowerUndo(int field_0xc, int prev_0x0, int prev_0xc, int prev_0x8, int prev_0x10, int prev_0x4, br_vector3* prev_0x14, br_vector3* prev_0x20, br_vector3* prev_0x2c);
extern void C2_HOOK_FASTCALL WorkerSplitWeldApply(void* roadMachine, br_vector3* pVector);
extern void C2_HOOK_FASTCALL WorkerSplitWeldUndo(void* roadMachine);
extern tCar_spec* C2_HOOK_FASTCALL GetCarSpec(tVehicle_type pCategory, int pIndex);
extern void C2_HOOK_FASTCALL WorkerShrapnelShower(tU32 field_0x4, tU32 field_0x84, tU32 field_0x84b, tU32 field_0x44, br_vector3* field_0x8, br_vector3* field_0x20, br_vector3* field_0x2c, tU32 field_0x50, tU32 field_0x48, tU32 field_0x0, br_vector3* field_0x54, br_vector3* field_0x88, br_vector3* field_0x14, tU32 field_0xa0, br_vector3* field_0x38, tU32 field_0);
extern void C2_HOOK_FASTCALL WorkerShrapnelShowerUndo(tU32 chunk_0x48, tU32 prev_0x4, tU32 prev_0x84, tU32 prev_0x44, br_vector3* prev_0x8, br_vector3* prev_0x20, br_vector3* prev_0x2c, tU32 prev_0x50, tU32 prev_0x48, tU32 prev_0x0, br_vector3* prev_0x54, br_vector3* prev_0x88, br_vector3* prev_0x14, tU32 prev_0xa0, br_vector3* prev_0x38);
extern tNapalm_bolt gNapalm_bolts[5];
extern void C2_HOOK_FASTCALL WorkerSmoke(int index, tU8 flag, float f1, float f2, br_vector3* pVec);
extern void C2_HOOK_FASTCALL WorkerFlame(int index, int flag, float f1, float f2, float f3, float f4);
extern void C2_HOOK_FASTCALL WorkerPedDiagnostics(void* pOpponent, void* pField_0x4, void* pField_0x58, void* pField_0x144);
extern void C2_HOOK_FASTCALL WorkerSmudge(tPipe_chunk** pChunk, int pScale);
extern int gShrapnel_flags;
extern int gSmoke_flags;
extern tShrapnel gShrapnel[30];
extern tSmoke_column gSmoke_column[10];
extern br_vector3 gZero_vector__smash;
// GLOBAL: CARMA2_HW 0x0065d0c8
const tReplay_callback gReplay_callbacks[70] = {
    {
        NULL,
        0x4,
        NULL,
        ApplyFrameBoundary,
        0x0,
        NULL
    },
    {
        NULL,
        0x14,
        NULL,
        NULL,
        0x0,
        NULL
    },
    {
        NULL,
        0x14,
        NULL,
        ApplySound,
        0x0,
        NULL
    },
    {
        NULL,
        0x0,
        CalcCrushLength,
        ApplyCrush,
        0x5,
        UndoCrush
    },
    {
        NULL,
        0x0,
        NULL,
        NULL,
        0x1,
        NULL
    },
    {
        NULL,
        0x44,
        NULL,
        ApplyCar,
        0x3,
        NULL
    },
    {
        NULL,
        0x6,
        NULL,
        ApplyGWS,
        0x3,
        NULL
    },
    {
        NULL,
        0xC,
        NULL,
        ApplyDamage,
        0x4,
        UndoDamage
    },
    {
        NULL,
        0x2,
        NULL,
        ApplySpecial,
        0x4,
        UndoSpecial
    },
    {
        NULL,
        0x0,
        NULL,
        NULL,
        0x2,
        NULL
    },
    {
        ResetSparks,
        0x18,
        NULL,
        ApplySpark,
        0x2,
        NULL
    },
    {
        ResetShrapnel,
        0x0,
        CalcShrapnelLength,
        ApplyShrapnel,
        0x2,
        NULL
    },
    {
        ResetScreenWobble,
        0x2,
        NULL,
        ApplyScreenWobble,
        0x4,
        UndoScreenWobble
    },
    {
        NULL,
        0x40,
        NULL,
        ApplyGrooveStop,
        0x2,
        NULL
    },
    {
        NULL,
        0x34,
        NULL,
        ApplyNonCar,
        0x3,
        NULL
    },
    {
        ResetSmoke,
        0xA,
        NULL,
        ApplySmoke,
        0x2,
        NULL
    },
    {
        NULL,
        0x54,
        NULL,
        ApplyOilSpill,
        0x2,
        NULL
    },
    {
        NULL,
        0x14,
        NULL,
        ApplySmokeColumn,
        0x1,
        NULL
    },
    {
        NULL,
        0x14,
        NULL,
        ApplyFlame,
        0x1,
        NULL
    },
    {
        NULL,
        0x0,
        CalcSmudgeLength,
        ApplySmudge,
        0x4,
        UndoSmudge
    },
    {
        NULL,
        0x10,
        NULL,
        ApplySplash,
        0x4,
        UndoSplash
    },
    {
        ResetProxRay,
        0x8,
        NULL,
        ApplyProxRay,
        0x1,
        NULL
    },
    {
        NULL,
        0x34,
        NULL,
        ApplySkidAdjustment,
        0x4,
        UndoSkidAdjustment
    },
    {
        NULL,
        0x8,
        NULL,
        ApplyFlap,
        0x5,
        UndoFlap
    },
    {
        NULL,
        0x0,
        CalcModelMashLength,
        ApplyModelMash,
        0x5,
        UndoModelMash
    },
    {
        NULL,
        0xC,
        NULL,
        ApplyRelink,
        0x5,
        UndoRelink
    },
    {
        NULL,
        0x4,
        NULL,
        ApplyIdentity,
        0x1,
        NULL
    },
    {
        NULL,
        0x14,
        NULL,
        ApplySplitWeld,
        0x4,
        UndoSplitWeld
    },
    {
        NULL,
        0x18,
        NULL,
        ApplyBend,
        0x5,
        UndoBend
    },
    {
        NULL,
        0x6,
        NULL,
        ApplyUnBend,
        0x5,
        UndoUnBend
    },
    {
        NULL,
        0xD0,
        NULL,
        ApplyShrapnelShower,
        0x4,
        UndoShrapnelShower
    },
    {
        NULL,
        0x78,
        NULL,
        ApplyExplosion,
        0x2,
        NULL
    },
    {
        NULL,
        0x4,
        NULL,
        ApplyNonCarCreation,
        0x5,
        UndoNonCarCreation
    },
    {
        NULL,
        0x8,
        NULL,
        ApplySmashModelChange,
        0x5,
        UndoSmashModelChange
    },
    {
        NULL,
        0x0,
        CalcRemoveFacesLength,
        ApplySmashRemoveFaces,
        0x5,
        UndoSmashRemoveFaces
    },
    {
        NULL,
        0x4,
        NULL,
        ApplySmashDecal,
        0x5,
        UndoSmashDecal
    },
    {
        NULL,
        0x10,
        NULL,
        ApplySmashTextureChange,
        0x5,
        UndoSmashTextureChange
    },
    {
        ResetRepulseRay,
        0xC,
        NULL,
        ApplyRepulseRay,
        0x2,
        NULL
    },
    {
        NULL,
        0x0,
        CalcEndMyBendLength,
        ApplyEndMyBend,
        0x1,
        NULL
    },
    {
        NULL,
        0x10,
        NULL,
        ApplyActorTrans,
        0x5,
        UndoActorTrans
    },
    {
        NULL,
        0x50,
        NULL,
        ApplyPedStatus,
        0x5,
        UndoPedStatus
    },
    {
        NULL,
        0xC,
        NULL,
        ApplyPedDir,
        0x2,
        NULL
    },
    {
        NULL,
        0x64,
        NULL,
        ApplyPedMove,
        0x5,
        UndoPedMove
    },
    {
        NULL,
        0x30,
        NULL,
        ApplyPedPhysics,
        0x2,
        NULL
    },
    {
        NULL,
        0x4,
        NULL,
        ApplyPedDismember,
        0x5,
        UndoPedDismember
    },
    {
        NULL,
        0x8,
        NULL,
        ApplyPedFPChange,
        0x5,
        UndoPedFPChange
    },
    {
        NULL,
        0x18,
        NULL,
        ApplyPedPos,
        0x5,
        UndoPedPos
    },
    {
        NULL,
        0x18,
        NULL,
        ApplyPedModelChange,
        0x5,
        UndoPedModelChange
    },
    {
        NULL,
        0x30,
        NULL,
        ApplyPHILObject,
        0x2,
        NULL
    },
    {
        NULL,
        0x34,
        NULL,
        ApplyGibShower,
        0x4,
        UndoGibShower
    },
    {
        NULL,
        0x2C,
        NULL,
        ApplyBloodSpurt,
        0x4,
        UndoBloodSpurt
    },
    {
        NULL,
        0x8,
        NULL,
        ApplyGrooveOnOff,
        0x5,
        UndoGrooveOnOff
    },
    {
        NULL,
        0x0,
        NULL,
        ApplyPowerupGot,
        0x5,
        UndoPowerupGot
    },
    {
        NULL,
        0x0,
        NULL,
        ApplyPowerupLose,
        0x5,
        UndoPowerupLose
    },
    {
        NULL,
        0x2,
        NULL,
        ApplyFunkEnable,
        0x5,
        UndoFunkEnable
    },
    {
        NULL,
        0x8,
        NULL,
        ApplyVanishDismembered,
        0x5,
        UndoVanishDismembered
    },
    {
        NULL,
        0x2,
        NULL,
        ApplyDSModel,
        0x1,
        NULL
    },
    {
        NULL,
        0x26C,
        NULL,
        ApplyPedDiagnostics,
        0x2,
        NULL
    },
    {
        NULL,
        0xC,
        NULL,
        ApplyVector3,
        0x5,
        UndoVector3
    },
    {
        NULL,
        0x2C,
        NULL,
        ApplyExtendedSplash,
        0x4,
        UndoExtendedSplash
    },
    {
        NULL,
        0x2,
        NULL,
        ApplyDroneRender,
        0x1,
        NULL
    },
    {
        NULL,
        0x1E,
        NULL,
        ApplyDroneCornerPos,
        0x1,
        NULL
    },
    {
        NULL,
        0x8,
        NULL,
        ApplyDroneStraightPos,
        0x1,
        NULL
    },
    {
        NULL,
        0x4,
        NULL,
        ApplyDroneUnused,
        0x1,
        NULL
    },
    {
        ARResetNapalmBolts,
        0x58,
        NULL,
        ApplyNapalmBolt,
        0x4,
        UndoNapalmBolt
    },
    {
        NULL,
        0x4,
        NULL,
        ApplyPowerupRespawn,
        0x5,
        UndoPowerupRespawn
    },
    {
        NULL,
        0x30,
        NULL,
        ApplyShitMine,
        0x5,
        UndoShitMine
    },
    {
        NULL,
        0x30,
        NULL,
        ApplyEndShitMine,
        0x5,
        UndoEndShitMine
    },
    {
        NULL,
        0x2,
        NULL,
        ApplyTransformType,
        0x5,
        UndoTransformType
    },
    {
        NULL,
        0x2,
        NULL,
        ApplyOppoRenderage,
        0x1,
        NULL
    }
};

// FUNCTION: CARMA2_HW 0x004c9360
void C2_HOOK_FASTCALL ApplyFrameBoundary(tPipe_chunk** pChunk) {

    return WorkerFrameBoundary(((tPipe_chunk_frame_bound*)*pChunk)->field_0x4);
}

// FUNCTION: CARMA2_HW 0x004c9370
void C2_HOOK_FASTCALL ApplySound(tPipe_chunk** pChunk) {
    tPipe_chunk_sound* chunk;

    if (*(tU32*)0x676918 != 0) return;

    chunk = (tPipe_chunk_sound*)*pChunk;
    if (chunk->field_0x16 == 0) {
        DRS3StartSound2(
            GetOutletFromIndex(chunk->field_0x14),
            chunk->field_0x0,
            1, -1, -1,
            (int)(fabs(ARGetReplayRate()) * 65536.0),
            0x10000);
        return;
    }

    {
        float dot = chunk->field_0x8 * chunk->field_0x8
            + chunk->field_0xc * chunk->field_0xc
            + chunk->field_0x10 * chunk->field_0x10;

        if (dot >= 0.0f) {
            DRS3StartSound2(
                GetOutletFromIndex(((tPipe_chunk_sound*)*pChunk)->field_0x14),
                ((tPipe_chunk_sound*)*pChunk)->field_0x0,
                1,
                ((tPipe_chunk_sound*)*pChunk)->field_0x16 & 0xff,
                (((tPipe_chunk_sound*)*pChunk)->field_0x16 >> 8) & 0xff,
                (int)(fabs(ARGetReplayRate()) * (double)((tPipe_chunk_sound*)*pChunk)->field_0x4),
                0x10000);
        } else {
            DRS3StartSound3D(
                GetOutletFromIndex(((tPipe_chunk_sound*)*pChunk)->field_0x14),
                ((tPipe_chunk_sound*)*pChunk)->field_0x0,
                (br_vector3*)((tU8*)&((tPipe_chunk_sound*)*pChunk)->field_0x8),
                &gZero_vector__smash,
                1,
                ((tPipe_chunk_sound*)*pChunk)->field_0x16,
                (int)(fabs(ARGetReplayRate()) * (double)((tPipe_chunk_sound*)*pChunk)->field_0x4),
                0x10000);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004ca3f0
int C2_HOOK_FASTCALL CalcCrushLength(tPipe_chunk* pChunk) {
    br_model* model = *(br_model**)((tU8*)pChunk + 4);
    return 0xa + 2 * model->nvertices;
}

// FUNCTION: CARMA2_HW 0x004c9040
void C2_HOOK_FASTCALL ApplyCrush(tPipe_chunk** pChunk) {
    tPipe_chunk_crush* chunk = (tPipe_chunk_crush*)*pChunk;

    return WorkerCrush(chunk->field_0x4, &chunk->field_0x8[0], &chunk->field_0x8[6]);
}

// FUNCTION: CARMA2_HW 0x004c9d90
void C2_HOOK_FASTCALL UndoCrush(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_crush* chunk = (tPipe_chunk_crush*)*pChunk;

    return WorkerCrushUndo(chunk->field_0x4, &chunk->field_0x8[0], &chunk->field_0x8[6]);
}

// FUNCTION: CARMA2_HW 0x004c9490
void C2_HOOK_FASTCALL ApplyCar(tPipe_chunk** pChunk) {
    tU32 code = *(tU32*)*pChunk;
    tCar_spec* car;
    br_vector3 scaled;
    br_vector4 out4;

    if ((code & 0xffffff00u) == 0) {
        car = (tCar_spec*)0x75bc2c;
    } else {
        car = GetCarSpec(code >> 8, code & 0xffu);
    }

    *(br_matrix34*)((tU8*)car->car_master_actor + 0x2c) = *(br_matrix34*)((tU8*)*pChunk + 4);
    memcpy((tU8*)car->collision_info + 0x68, (tU8*)*pChunk + 0x34, 12);
    BrMatrix34TApplyV(&car->collision_info->velocity_car_space, &car->collision_info->v, &car->car_master_actor->t.t.mat);

    if (car->collision_info->velocity_car_space.v[0] * car->collision_info->velocity_car_space.v[0]
            + car->collision_info->velocity_car_space.v[1] * car->collision_info->velocity_car_space.v[1]
            + car->collision_info->velocity_car_space.v[2] * car->collision_info->velocity_car_space.v[2] < 0.01) {
        car->direction.v[0] = -*(float*)((tU8*)car->car_master_actor + 0x44);
        car->direction.v[1] = -*(float*)((tU8*)car->car_master_actor + 0x48);
        car->direction.v[2] = -*(float*)((tU8*)car->car_master_actor + 0x4c);
    } else {
        float len;
        float inv;
        len = (float)sqrt(car->collision_info->v.v[0] * car->collision_info->v.v[0]
                        + car->collision_info->v.v[1] * car->collision_info->v.v[1]
                        + car->collision_info->v.v[2] * car->collision_info->v.v[2]);
        if (len > 2.3841858e-07f) {
            car->direction.v[0] = *(volatile float*)((tU8*)car->collision_info + 0x68) * (float)(1.0 / (double)len);
            car->direction.v[1] = *(volatile float*)((tU8*)car->collision_info + 0x6c) * (float)(1.0 / (double)len);
            car->direction.v[2] = *(volatile float*)((tU8*)car->collision_info + 0x70) * (float)(1.0 / (double)len);
        } else {
            car->direction.v[0] = 1.0f;
            car->direction.v[1] = 0.0f;
            car->direction.v[2] = 0.0f;
        }
    }

    car->pos = *(br_vector3*)((tU8*)car->car_master_actor + 0x50);
    scaled.v[0] = car->centre_of_mass_world_scale.v[0] * 0.1449275f;
    scaled.v[1] = car->centre_of_mass_world_scale.v[1] * 0.1449275f;
    scaled.v[2] = car->centre_of_mass_world_scale.v[2] * 0.1449275f;
    BrMatrix34ApplyV(&out4, &scaled, &car->car_master_actor->t.t.mat);
    car->pos.v[0] += out4.v[3];
    car->pos.v[1] += out4.v[0];
    car->pos.v[2] += out4.v[0];

    *(float*)((tU8*)car + 0x4a4) = (float)((double)(int)*(tS16*)((tU8*)*pChunk + 0x40) * 2.1362956633198035e-06);
    *(float*)((tU8*)car + 0x1340) = (float)((int)(*(tU16*)((tU8*)*pChunk + 0x42) & 0x7ff) * 10);
    *(int*)((tU8*)car + 0x135c) = (*(tU16*)((tU8*)*pChunk + 0x42) >> 12) - 1;
    car->frame_collision_flag = *(tU16*)((tU8*)*pChunk + 0x42) >> 11 & 1;
    car->field_0x18cc = *(int*)((tU8*)*pChunk + 0x44);
}

// FUNCTION: CARMA2_HW 0x004c96c0
void C2_HOOK_FASTCALL ApplyGWS(tPipe_chunk** pChunk) {
    tCar_spec* car;
    tU32 code = ((tPipe_chunk_graphpical_wheel_stuff*)*pChunk)->field_0x0;

    if ((code & 0xffffff00u) == 0) {
        car = (tCar_spec*)0x75bc2c;
    } else {
        car = GetCarSpec(code >> 8, code & 0xffu);
    }

    *(float*)((tU8*)car + 0x4a8) = (float)((double)(int)((tPipe_chunk_graphpical_wheel_stuff*)*pChunk)->field_0x6 * 0.001181102362);
    *(float*)((tU8*)car + 0x4ac) = (float)((double)(int)((tPipe_chunk_graphpical_wheel_stuff*)*pChunk)->field_0x7 * 0.001181102362);
    *(float*)((tU8*)car + 0x4b0) = (float)((double)(int)((tPipe_chunk_graphpical_wheel_stuff*)*pChunk)->field_0x8 * 0.001181102362);
    *(float*)((tU8*)car + 0x4b4) = (float)((double)(int)((tPipe_chunk_graphpical_wheel_stuff*)*pChunk)->field_0x9 * 0.001181102362);
    *(float*)((tU8*)car + 0x4a0) = (float)((double)(int)((tPipe_chunk_graphpical_wheel_stuff*)*pChunk)->field_0x4 * 0.001831110569);
}

// FUNCTION: CARMA2_HW 0x004c9770
void C2_HOOK_FASTCALL ApplyDamage(tPipe_chunk** pChunk) {
    tCar_spec* car;
    tU32 code = ((tPipe_chunk_damage*)*pChunk)->field_0x0;
    int* p;
    int i;

    if ((code & 0xffffff00u) == 0) {
        car = (tCar_spec*)0x75bc2c;
    } else {
        car = GetCarSpec(code >> 8, code & 0xffu);
    }

    p = (int*)((tU8*)car + 0x4e0);

    for (i = 0; i < 12; i++) {
        *p += ((tPipe_chunk_damage*)*pChunk)->field_0x4[i];
        p = (int*)((tU8*)p + 0x2c);
    }
}

// FUNCTION: CARMA2_HW 0x004c9dd0
void C2_HOOK_FASTCALL UndoDamage(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tCar_spec* car;
    tU32 code = ((tPipe_chunk_damage*)*pChunk)->field_0x0;
    int* p;
    int i;

    if ((code & 0xffffff00u) == 0) {
        car = (tCar_spec*)0x75bc2c;
    } else {
        car = GetCarSpec(code >> 8, code & 0xffu);
    }

    p = (int*)((tU8*)car + 0x4e0);

    for (i = 0; i < 12; i++) {
        *p -= ((tPipe_chunk_damage*)*pChunk)->field_0x4[i];
        p = (int*)((tU8*)p + 0x2c);
    }
}

// FUNCTION: CARMA2_HW 0x004c97c0
void C2_HOOK_FASTCALL ApplySpecial(tPipe_chunk** pChunk) {
    tU32 type = *(tU32*)*pChunk;

    switch (type) {
        case 0:
            if (fabs(ARGetReplayRate()) <= 1.0) {
                return FadePaletteDown();
            }
            return;
        case 1:
            return WorkerSpecial();
    }
}

// FUNCTION: CARMA2_HW 0x004c9e20
void C2_HOOK_FASTCALL UndoSpecial(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tU32 type = *(tU32*)*pChunk;

    if (type == 0) {
        if (fabs(ARGetReplayRate()) <= 1.0) {
            return FadePaletteDown();
        }
    } else {
        if (--type) {
            return;
        }
        return WorkerSpecialUndo();
    }
}

// FUNCTION: CARMA2_HW 0x004c9140
void C2_HOOK_FASTCALL ApplySpark(tPipe_chunk** pChunk) {
    tPipe_chunk_spark_replay* chunk = (tPipe_chunk_spark_replay*)*pChunk;

    return WorkerSpark(chunk->field_0x0, &chunk->field_0x4[0], &chunk->field_0x4[0xc]);
}

// FUNCTION: CARMA2_HW 0x004f87b0
void C2_HOOK_FASTCALL ResetShrapnel(void) {
    tShrapnel* rec;
    tU32 i = 0;

    if (gShrapnel_flags != 0) {
        rec = gShrapnel;
        do {
            if ((1 << i) & gShrapnel_flags) {
                br_actor* elem = rec->actor;
                if (*(void**)((tU8*)elem + 0xc) != 0) {
                    BrActorRemove(elem);
                }
            }
            rec++;
            i++;
        } while ((tS32)(tU32)(tU8*)rec < (tS32)(tU32)(tU8*)gSmoke_column);
        gShrapnel_flags = 0;
    }
}

// FUNCTION: CARMA2_HW 0x004c9160
void C2_HOOK_FASTCALL ApplyShrapnel(tPipe_chunk** pChunk) {
    tPipe_chunk_shrapnel_replay* chunk = (tPipe_chunk_shrapnel_replay*)*pChunk;

    return WorkerShrapnel(chunk->field_0x0, &chunk->field_0x4[0], chunk->field_0x10, chunk->field_0x14);
}

// FUNCTION: CARMA2_HW 0x004ca400
int C2_HOOK_FASTCALL CalcShrapnelLength(tPipe_chunk* pChunk) {
    return (*(tU32*)pChunk & 0x8000) ? 0x14 : 0xc;
}

// FUNCTION: CARMA2_HW 0x004e4e30
void C2_HOOK_FASTCALL ResetScreenWobble(void) {
    gScreen_wobble_x = 0;
    gScreen_wobble_y = 0;
}

// FUNCTION: CARMA2_HW 0x004c9180
void C2_HOOK_FASTCALL ApplyScreenWobble(tPipe_chunk** pChunk) {

    SetScreenWobble(((tPipe_chunk_screen_wobble*)((tU8*)*pChunk + 4))->wobble_x,
                    ((tPipe_chunk_screen_wobble*)((tU8*)*pChunk + 4))->wobble_y);
}

// FUNCTION: CARMA2_HW 0x004c9e50
void C2_HOOK_FASTCALL UndoScreenWobble(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

    SetPipeUndoFlag();
    if (pPrev_chunk != NULL) {
        tPipe_chunk_screen_wobble* wobble = (tPipe_chunk_screen_wobble*)((tU8*)pPrev_chunk + 4);
        SetScreenWobble(wobble->wobble_x, wobble->wobble_y);
    } else {
        SetScreenWobble(0, 0);
    }
    ClearPipeUndoFlag();
}

// FUNCTION: CARMA2_HW 0x004c9190
void C2_HOOK_FASTCALL ApplyGrooveStop(tPipe_chunk** pChunk) {
    tPipe_chunk_groove_stop* chunk = (tPipe_chunk_groove_stop*)*pChunk;

    return WorkerGrooveStop(chunk->field_0x0, &chunk->field_0x14, chunk->field_0x4, chunk->field_0x8, chunk->field_0xc, chunk->field_0x10);
}

// FUNCTION: CARMA2_HW 0x004c91b0
void C2_HOOK_FASTCALL ApplyNonCar(tPipe_chunk** pChunk) {
    tPipe_chunk_non_car_replay* chunk = (tPipe_chunk_non_car_replay*)*pChunk;

    return WorkerNonCar(chunk->field_0x4, &chunk->field_0x8);
}

// FUNCTION: CARMA2_HW 0x004fb510
void C2_HOOK_FASTCALL ResetSmoke(void) {

    gSmoke_flags = 0;
}

// FUNCTION: CARMA2_HW 0x004c91c0
void C2_HOOK_FASTCALL ApplySmoke(tPipe_chunk** pChunk) {
    tU8* chunk = (tU8*)*pChunk;
    tCar_spec* car = *(tCar_spec**)0x75bc3c;
    float tmp[3];

    tmp[0] = (float)((double)(int)(tS16)*(tU16*)(chunk + 4) * 0.00125);
    tmp[1] = (float)((double)(int)(tS16)*(tU16*)(chunk + 6) * 0.00125);
    tmp[2] = (float)((double)(int)(tS16)*(tU16*)(chunk + 8) * 0.00125);
    tmp[0] += *(float*)((tU8*)car + 0x50);
    tmp[1] += *(float*)((tU8*)car + 0x54);
    tmp[2] += *(float*)((tU8*)car + 0x58);

    return WorkerSmoke(*(int*)(chunk + 0), *(tU8*)(chunk + 0xc), (float)*(tU16*)(chunk + 0xa) * 0.00390625f, (float)*(tU8*)(chunk + 0xc) * 0.0009765625f, tmp);
}

// FUNCTION: CARMA2_HW 0x004c9330
void C2_HOOK_FASTCALL ApplyOilSpill(tPipe_chunk** pChunk) {
    tPipe_chunk_oil_spill* chunk = (tPipe_chunk_oil_spill*)*pChunk;

    return WorkerOilSpill(chunk->field_0x0, &chunk->field_0x4, chunk->field_0x34, chunk->field_0x38, chunk->field_0x3c, 0, chunk->field_0x44, &chunk->field_0x48, chunk->field_0x54);
}

// FUNCTION: CARMA2_HW 0x004c9280
void C2_HOOK_FASTCALL ApplySmokeColumn(tPipe_chunk** pChunk) {
    tPipe_chunk_smoke_column_replay* chunk = (tPipe_chunk_smoke_column_replay*)*pChunk;

    return WorkerSmokeColumn(chunk->field_0x0 & 0x3fff, (br_vector3*)chunk->pad, chunk->field_0x14, chunk->field_0x0 >> 14);
}

// FUNCTION: CARMA2_HW 0x004c92a0
void C2_HOOK_FASTCALL ApplyFlame(tPipe_chunk** pChunk) {
    tU8* chunk = (tU8*)*pChunk;

    return WorkerFlame(*(int*)(chunk + 0), (int)(tS16)*(tU16*)(chunk + 4), *(float*)(chunk + 8), *(float*)(chunk + 0xc), *(float*)(chunk + 0x10), *(float*)(chunk + 0x10));
}

// FUNCTION: CARMA2_HW 0x004ca420
int C2_HOOK_FASTCALL CalcSmudgeLength(tPipe_chunk* pChunk) {
    tU16 vertex_count = ((tPipe_chunk_smudge*)pChunk)->smudge.vertex_count;
    return 8 + vertex_count * 4;
}

// FUNCTION: CARMA2_HW 0x004c9060
void C2_HOOK_FASTCALL ApplySmudge(tPipe_chunk** pChunk) {

    return WorkerSmudge(pChunk, 1);
}

// FUNCTION: CARMA2_HW 0x004c9db0
void C2_HOOK_FASTCALL UndoSmudge(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

    return WorkerSmudge(pChunk, -1);
}

// FUNCTION: CARMA2_HW 0x004c92d0
void C2_HOOK_FASTCALL ApplySplash(tPipe_chunk** pChunk) {
    ((tReplay_splash_effect*)((tPipe_chunk_splash*)*pChunk)->pOwner)->size = ((tPipe_chunk_splash*)*pChunk)->v[0];
    ((tReplay_splash_effect*)((tPipe_chunk_splash*)*pChunk)->pOwner)->x = ((tPipe_chunk_splash*)*pChunk)->v[1];
    ((tReplay_splash_effect*)((tPipe_chunk_splash*)*pChunk)->pOwner)->y = ((tPipe_chunk_splash*)*pChunk)->v[2];
    ((tReplay_splash_effect*)((tPipe_chunk_splash*)*pChunk)->pOwner)->z = ((tPipe_chunk_splash*)*pChunk)->v[3];
}

// FUNCTION: CARMA2_HW 0x004c9e80
void C2_HOOK_FASTCALL UndoSplash(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    SetPipeUndoFlag();
    if (pPrev_chunk != NULL) {
        tPipe_chunk_splash* chunk = (tPipe_chunk_splash*)pPrev_chunk;
        ((tReplay_splash_effect*)chunk->pOwner)->size = ((tPipe_chunk_splash*)pPrev_chunk)->v[0];
        ((tReplay_splash_effect*)chunk->pOwner)->x = ((tPipe_chunk_splash*)pPrev_chunk)->v[1];
        ((tReplay_splash_effect*)chunk->pOwner)->y = ((tPipe_chunk_splash*)pPrev_chunk)->v[2];
        ((tReplay_splash_effect*)chunk->pOwner)->z = ((tPipe_chunk_splash*)pPrev_chunk)->v[3];
    } else {
        ((tReplay_splash_effect*)((tPipe_chunk_splash*)*pChunk)->pOwner)->size = 10000.0f;
    }
    ClearPipeUndoFlag();
}

// FUNCTION: CARMA2_HW 0x004d61f0
void C2_HOOK_FASTCALL ResetProxRay(void) {
    tProximity_ray* ray = gProximity_rays;
    do {
        ray->car = NULL;
        ray++;
    } while ((int)ray < (int)&gProximity_rays[CARPOCALYPSE2_ASIZE(gProximity_rays)]);
}

// FUNCTION: CARMA2_HW 0x004c97f0
void C2_HOOK_FASTCALL ApplyProxRay(tPipe_chunk** pChunk) {
    tPipe_chunk_prox_ray_replay* chunk = (tPipe_chunk_prox_ray_replay*)*pChunk;

    return WorkerProxRay(chunk->field_0x0, chunk->field_0x6, chunk->field_0x4, chunk->field_0x8);
}

// FUNCTION: CARMA2_HW 0x004c9810
void C2_HOOK_FASTCALL ApplySkidAdjustment(tPipe_chunk** pChunk) {
    tPipe_chunk_skid_adjust* chunk = (tPipe_chunk_skid_adjust*)*pChunk;
    AdjustSkid(chunk->skid_num, &chunk->matrix, chunk->material);
}

// FUNCTION: CARMA2_HW 0x004c9f20
void C2_HOOK_FASTCALL UndoSkidAdjustment(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    SetPipeUndoFlag();
    if (pPrev_chunk != NULL) {
        tPipe_chunk_skid_adjust* chunk = (tPipe_chunk_skid_adjust*)pPrev_chunk;
        AdjustSkid(chunk->skid_num, &chunk->matrix, chunk->material);
    } else {
        HideSkid(((tPipe_chunk_skid_adjust*)*pChunk)->skid_num);
    }
    ClearPipeUndoFlag();
}

// FUNCTION: CARMA2_HW 0x004c9830
void C2_HOOK_FASTCALL ApplyFlap(tPipe_chunk** pChunk) {
    tPipe_chunk_flap* chunk = (tPipe_chunk_flap*)*pChunk;

    return WorkerFlap(chunk->field_0x4, chunk->field_0x8);
}

// FUNCTION: CARMA2_HW 0x004c9f60
void C2_HOOK_FASTCALL UndoFlap(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_flap* chunk = (tPipe_chunk_flap*)*pChunk;

    return WorkerFlap(chunk->field_0x4, (tS16)-chunk->field_0x8);
}

// FUNCTION: CARMA2_HW 0x004ca430
int C2_HOOK_FASTCALL CalcModelMashLength(tPipe_chunk* pChunk) {
    br_model* model = *(br_model**)((tU8*)pChunk + 4);
    return 4 + 6 * model->nvertices;
}

// FUNCTION: CARMA2_HW 0x004c9840
void C2_HOOK_FASTCALL ApplyModelMash(tPipe_chunk** pChunk) {
    tPipe_chunk_model_mash* chunk = (tPipe_chunk_model_mash*)*pChunk;

    return WorkerModelMash(chunk->field_0x4, chunk->field_0x8);
}

// FUNCTION: CARMA2_HW 0x004c9f80
void C2_HOOK_FASTCALL UndoModelMash(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_model_mash* chunk = (tPipe_chunk_model_mash*)*pChunk;

    return WorkerModelMashUndo(chunk->field_0x4, chunk->field_0x8);
}

// FUNCTION: CARMA2_HW 0x004c9850
void C2_HOOK_FASTCALL ApplyRelink(tPipe_chunk** pChunk) {
    tPipe_chunk_relink* chunk = (tPipe_chunk_relink*)*pChunk;
    br_actor* actor = chunk->actor;
    br_actor* parent = chunk->field_0xc;
    if (actor->parent != NULL) {
        if (parent != NULL) {
            BrActorRelink(parent, actor);
        } else {
            BrActorRemove(actor);
        }
    } else {
        if (parent != NULL) {
            BrActorAdd(parent, actor);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004c9f90
void C2_HOOK_FASTCALL UndoRelink(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_relink* chunk = (tPipe_chunk_relink*)*pChunk;
    br_actor* actor = chunk->actor;
    br_actor* parent = (br_actor*)chunk->field_0x8;
    if (actor->parent != NULL) {
        if (parent != NULL) {
            BrActorRelink(parent, actor);
        } else {
            BrActorRemove(actor);
        }
    } else {
        if (parent != NULL) {
            BrActorAdd(parent, actor);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004c9890
void C2_HOOK_FASTCALL ApplyIdentity(tPipe_chunk** pChunk) {
    BrMatrix34Identity((br_matrix34*)((tU8*)((tPipe_chunk_actor_trans*)*pChunk)->field_0x4 + 0x2c));
}

static tPipe_chunk_split_weld* swGetChunk(tPipe_chunk** pp) {
    return (tPipe_chunk_split_weld*)*(tPipe_chunk** volatile)pp;
}

// FUNCTION: CARMA2_HW 0x004c98b0
void C2_HOOK_FASTCALL ApplySplitWeld(tPipe_chunk** pChunk) {
    tU32 fn = (*(tPipe_chunk_split_weld**)*pChunk)->field_0x0;

    if ((*(tPipe_chunk_split_weld**)*pChunk)->field_0x14) {
        if ((fn & 0xffffff00u) == 0) {
            return WorkerSplitWeldApply((void*)0x75bc2c, &swGetChunk(pChunk)->field_0x4);
        }

        return WorkerSplitWeldApply(GetCarSpec(fn >> 8, fn & 0xffu), &swGetChunk(pChunk)->field_0x4);
    }

    if ((fn & 0xffffff00u) == 0) {
        return WorkerSplitWeldUndo((void*)0x75bc2c);
    }

    return WorkerSplitWeldUndo(GetCarSpec(fn >> 8, fn & 0xffu));
}

// FUNCTION: CARMA2_HW 0x004c9fd0
void C2_HOOK_FASTCALL UndoSplitWeld(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tU32 fn = (*(tPipe_chunk_split_weld**)*pChunk)->field_0x0;

    if ((*(tPipe_chunk_split_weld**)*pChunk)->field_0x14) {
        if ((fn & 0xffffff00u) == 0) {
            return WorkerSplitWeldUndo((void*)0x75bc2c);
        }

        return WorkerSplitWeldUndo(GetCarSpec(fn >> 8, fn & 0xffu));
    }

    if ((fn & 0xffffff00u) == 0) {
        return WorkerSplitWeldApply((void*)0x75bc2c, &swGetChunk(pChunk)->field_0x4);
    }

    return WorkerSplitWeldApply(GetCarSpec(fn >> 8, fn & 0xffu), &swGetChunk(pChunk)->field_0x4);
}

// FUNCTION: CARMA2_HW 0x004c9930
void C2_HOOK_FASTCALL ApplyBend(tPipe_chunk** pChunk) {
}

// FUNCTION: CARMA2_HW 0x004ca050
void C2_HOOK_FASTCALL UndoBend(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
}

// FUNCTION: CARMA2_HW 0x004c9940
void C2_HOOK_FASTCALL ApplyUnBend(tPipe_chunk** pChunk) {
}

// FUNCTION: CARMA2_HW 0x004ca060
void C2_HOOK_FASTCALL UndoUnBend(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
}

// FUNCTION: CARMA2_HW 0x004c9960
void C2_HOOK_FASTCALL ApplyShrapnelShower(tPipe_chunk** pChunk) {
    tPipe_chunk_shrapnel_shower* chunk = (tPipe_chunk_shrapnel_shower*)*pChunk;

    if (chunk->field_0x84 == 0) {
        return;
    }

    memcpy((void*)0x6b78a0, &chunk->field_0xa4, sizeof(chunk->field_0xa4));

    return WorkerShrapnelShower(((tPipe_chunk_shrapnel_shower*)*pChunk)->field_0x4, ((tPipe_chunk_shrapnel_shower*)*pChunk)->field_0x84, ((tPipe_chunk_shrapnel_shower*)*pChunk)->field_0x84, ((tPipe_chunk_shrapnel_shower*)*pChunk)->field_0x44, &((tPipe_chunk_shrapnel_shower*)*pChunk)->field_0x8, &((tPipe_chunk_shrapnel_shower*)*pChunk)->field_0x20, &((tPipe_chunk_shrapnel_shower*)*pChunk)->field_0x2c, ((tPipe_chunk_shrapnel_shower*)*pChunk)->field_0x50, ((tPipe_chunk_shrapnel_shower*)*pChunk)->field_0x48, ((tPipe_chunk_shrapnel_shower*)*pChunk)->field_0x0, &((tPipe_chunk_shrapnel_shower*)*pChunk)->field_0x54, &((tPipe_chunk_shrapnel_shower*)*pChunk)->field_0x88, &((tPipe_chunk_shrapnel_shower*)*pChunk)->field_0x14, ((tPipe_chunk_shrapnel_shower*)*pChunk)->field_0xa0, &((tPipe_chunk_shrapnel_shower*)*pChunk)->field_0x38, 0);
}

// FUNCTION: CARMA2_HW 0x004ca070
void C2_HOOK_FASTCALL UndoShrapnelShower(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_shrapnel_shower* chunk = (tPipe_chunk_shrapnel_shower*)*pChunk;
    tPipe_chunk_shrapnel_shower* prev = (tPipe_chunk_shrapnel_shower*)pPrev_chunk;

    if (chunk->field_0x84 != 0 || prev == NULL || prev->field_0x4 == 0 || prev->field_0x84 == 0) {
        return;
    }

    memcpy((void*)0x6b78a0, &prev->field_0xa4, sizeof(prev->field_0xa4));

    return WorkerShrapnelShowerUndo(chunk->field_0x48, prev->field_0x4, prev->field_0x84, prev->field_0x44, &prev->field_0x8, &prev->field_0x20, &prev->field_0x2c, prev->field_0x50, prev->field_0x48, prev->field_0x0, &prev->field_0x54, &prev->field_0x88, &prev->field_0x14, prev->field_0xa0, &prev->field_0x38);
}

// FUNCTION: CARMA2_HW 0x004c99d0
void C2_HOOK_FASTCALL ApplyExplosion(tPipe_chunk** pChunk) {
    tPipe_chunk_explosion* chunk = (tPipe_chunk_explosion*)*pChunk;

    return WorkerExplosion(chunk->field_0x0, &chunk->field_0x4);
}

// FUNCTION: CARMA2_HW 0x004c99e0
void C2_HOOK_FASTCALL ApplyNonCarCreation(tPipe_chunk** pChunk) {
    tPipe_chunk_non_car_creation* chunk = (tPipe_chunk_non_car_creation*)*pChunk;

    return WorkerNonCarCreation(chunk->field_0x0);
}

// FUNCTION: CARMA2_HW 0x004ca100
void C2_HOOK_FASTCALL UndoNonCarCreation(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_non_car_creation* chunk = (tPipe_chunk_non_car_creation*)*pChunk;

    return WorkerNonCarCreationUndo(chunk->field_0x0);
}

// FUNCTION: CARMA2_HW 0x004c99f0
void C2_HOOK_FASTCALL ApplySmashModelChange(tPipe_chunk** pChunk) {
    tPipe_chunk_smash_model_change* chunk = (tPipe_chunk_smash_model_change*)*pChunk;

    if (chunk->field_0x8) {
        return WorkerSmashModelChangeApply(chunk->field_0x0, chunk->field_0x4, chunk->field_0x8);
    }
    return WorkerSmashModelChangeB(chunk->field_0x0);
}

// FUNCTION: CARMA2_HW 0x004ca110
void C2_HOOK_FASTCALL UndoSmashModelChange(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_smash_model_change* chunk = (tPipe_chunk_smash_model_change*)*pChunk;

    if (chunk->field_0x8) {
        return WorkerSmashModelChangeApply(chunk->field_0x0, chunk->field_0x8, chunk->field_0x4);
    }
    return WorkerSmashModelChangeBUndo(chunk->field_0x0);
}

// FUNCTION: CARMA2_HW 0x004ca460
int C2_HOOK_FASTCALL CalcRemoveFacesLength(tPipe_chunk* pChunk) {

    return 8 + 8 * ((tPipe_chunk_remove_faces*)pChunk)->count;
}

// FUNCTION: CARMA2_HW 0x004c9a10
void C2_HOOK_FASTCALL ApplySmashRemoveFaces(tPipe_chunk** pChunk) {
    tPipe_chunk_remove_faces* chunk = (tPipe_chunk_remove_faces*)*pChunk;

    return WorkerSmashRemoveFaces(chunk->field_0x0, (int)chunk->count);
}

// FUNCTION: CARMA2_HW 0x004ca130
void C2_HOOK_FASTCALL UndoSmashRemoveFaces(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_remove_faces_replay* chunk = (tPipe_chunk_remove_faces_replay*)*pChunk;

    return WorkerUndoSmashRemoveFaces(chunk->field_0x0, chunk->field_0x8, &chunk->field_0xc, chunk->field_0x4);
}

// FUNCTION: CARMA2_HW 0x004c9a20
void C2_HOOK_FASTCALL ApplySmashDecal(tPipe_chunk** pChunk) {
    tPipe_chunk_smash_decal* chunk = (tPipe_chunk_smash_decal*)*pChunk;

    return WorkerSmashDecal(chunk->field_0x0);
}

// FUNCTION: CARMA2_HW 0x004ca150
void C2_HOOK_FASTCALL UndoSmashDecal(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_smash_decal* chunk = (tPipe_chunk_smash_decal*)*pChunk;

    return WorkerSmashDecalUndo(chunk->field_0x0);
}

// FUNCTION: CARMA2_HW 0x004c9a30
void C2_HOOK_FASTCALL ApplySmashTextureChange(tPipe_chunk** pChunk) {
    tPipe_chunk_smash_texture_change_replay* chunk = (tPipe_chunk_smash_texture_change_replay*)*pChunk;

    return WorkerSmashTextureChange(chunk->field_0x0, chunk->field_0x4, chunk->field_0xc, chunk->field_0x10);
}

// FUNCTION: CARMA2_HW 0x004ca160
void C2_HOOK_FASTCALL UndoSmashTextureChange(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_smash_texture_change_replay* chunk = (tPipe_chunk_smash_texture_change_replay*)*pChunk;

    return WorkerSmashTextureChange(chunk->field_0x0, chunk->field_0x4, chunk->field_0x10, chunk->field_0xc);
}

// FUNCTION: CARMA2_HW 0x004da990
void C2_HOOK_FASTCALL ResetRepulseRay(void) {
    int i = 0;

    do {
        tRepulse_link* link = &gRepulse_links[i];
        if (link->actor->parent == gNon_track_actor) {
            BrActorRemove(link->actor);
        }
        link->time = 0;
        i++;
    } while ((int)&gRepulse_links[i].actor < (int)&gINT_006a0a5c);
}

// FUNCTION: CARMA2_HW 0x004c9a50
void C2_HOOK_FASTCALL ApplyRepulseRay(tPipe_chunk** pChunk) {
    tPipe_chunk_repulse_ray* chunk = (tPipe_chunk_repulse_ray*)*pChunk;

    return WorkerRepulseRay((int)chunk->origin, (int)chunk->victim, (int)chunk->time, (int)chunk->field_0xc);
}

// FUNCTION: CARMA2_HW 0x004ca450
int C2_HOOK_FASTCALL CalcEndMyBendLength(tPipe_chunk* pChunk) {

    return ((tPipe_chunk_end_my_bend*)pChunk)->length;
}

// FUNCTION: CARMA2_HW 0x004c9950
void C2_HOOK_FASTCALL ApplyEndMyBend(tPipe_chunk** pChunk) {
}

// FUNCTION: CARMA2_HW 0x004c9a70
void C2_HOOK_FASTCALL ApplyActorTrans(tPipe_chunk** pChunk) {
    ((tReplay_actor_trans_effect*)((tPipe_chunk_actor_trans*)*pChunk)->field_0x4)->x += ((tPipe_chunk_actor_trans*)*pChunk)->delta.v[0];
    ((tReplay_actor_trans_effect*)((tPipe_chunk_actor_trans*)*pChunk)->field_0x4)->y += ((tPipe_chunk_actor_trans*)*pChunk)->delta.v[1];
    ((tReplay_actor_trans_effect*)((tPipe_chunk_actor_trans*)*pChunk)->field_0x4)->z += ((tPipe_chunk_actor_trans*)*pChunk)->delta.v[2];
}

// FUNCTION: CARMA2_HW 0x004ca180
void C2_HOOK_FASTCALL UndoActorTrans(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    ((tReplay_actor_trans_effect*)((tPipe_chunk_actor_trans*)*pChunk)->field_0x4)->x -= ((tPipe_chunk_actor_trans*)*pChunk)->delta.v[0];
    ((tReplay_actor_trans_effect*)((tPipe_chunk_actor_trans*)*pChunk)->field_0x4)->y -= ((tPipe_chunk_actor_trans*)*pChunk)->delta.v[1];
    ((tReplay_actor_trans_effect*)((tPipe_chunk_actor_trans*)*pChunk)->field_0x4)->z -= ((tPipe_chunk_actor_trans*)*pChunk)->delta.v[2];
}

// FUNCTION: CARMA2_HW 0x004c9aa0
void C2_HOOK_FASTCALL ApplyPedStatus(tPipe_chunk** pChunk) {
    tPipe_chunk_ped_status* chunk = (tPipe_chunk_ped_status*)*pChunk;

    return WorkerPedStatus(chunk->field_0x0, chunk->field_0x4[0], chunk->field_0x4[1], chunk->field_0x4[2], chunk->field_0x4[3], chunk->field_0x4[5], chunk->field_0x4[7], &chunk->field_0xc, &chunk->field_0x18, &chunk->field_0x24);
}

// FUNCTION: CARMA2_HW 0x004ca1b0
void C2_HOOK_FASTCALL UndoPedStatus(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_ped_status* chunk = (tPipe_chunk_ped_status*)*pChunk;

    return WorkerPedStatusUndo(chunk->field_0x0, chunk->field_0x4[0], chunk->field_0x4[1], chunk->field_0x4[2], chunk->field_0x4[3], chunk->field_0x4[4], chunk->field_0x4[6], &chunk->field_0xc, &chunk->field_0x18, &chunk->field_0x24);
}

// FUNCTION: CARMA2_HW 0x004c9ad0
void C2_HOOK_FASTCALL ApplyPedDir(tPipe_chunk** pChunk) {
    tPipe_chunk_ped_dir* chunk = (tPipe_chunk_ped_dir*)*pChunk;

    return WorkerPedDir(chunk->field_0x0, &chunk->field_0x4);
}

// FUNCTION: CARMA2_HW 0x004c9ae0
void C2_HOOK_FASTCALL ApplyPedMove(tPipe_chunk** pChunk) {
    tPipe_chunk_ped_move* chunk = (tPipe_chunk_ped_move*)*pChunk;

    return WorkerPedMove(chunk->field_0x0, chunk->field_0x4, chunk->field_0xc, chunk->field_0x8 & 1, chunk->field_0x14, 0, &chunk->field_0x24, chunk->field_0x34, &chunk->field_0x38);
}

// FUNCTION: CARMA2_HW 0x004ca1e0
void C2_HOOK_FASTCALL UndoPedMove(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_ped_move* chunk = (tPipe_chunk_ped_move*)*pChunk;

    if (!chunk->field_0x14) {
        return WorkerPedMoveUndo(chunk->field_0x0, chunk->field_0x6, chunk->field_0x10, 0, 0, chunk->field_0xa, &chunk->field_0x18, chunk->field_0x30, &chunk->field_0x38);
    }
    return;
}

// FUNCTION: CARMA2_HW 0x004c9b10
void C2_HOOK_FASTCALL ApplyPedPhysics(tPipe_chunk** pChunk) {
    tPipe_chunk_ped_physics* chunk = (tPipe_chunk_ped_physics*)*pChunk;

    return WorkerPedPhysics(chunk->field_0x0, &chunk->field_0x4);
}

// FUNCTION: CARMA2_HW 0x004c9b20
void C2_HOOK_FASTCALL ApplyPedDismember(tPipe_chunk** pChunk) {
    tPipe_chunk_ped_dismember* chunk = (tPipe_chunk_ped_dismember*)*pChunk;

    return WorkerPedDismember(chunk->field_0x0, (int)chunk->field_0x4, (int)chunk->field_0x7);
}

// FUNCTION: CARMA2_HW 0x004ca210
void C2_HOOK_FASTCALL UndoPedDismember(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_ped_dismember* chunk = (tPipe_chunk_ped_dismember*)*pChunk;

    return WorkerPedDismemberUndo(chunk->field_0x0, (int)chunk->field_0x4, (int)(*(tS8*)((tU8*)chunk + 6)));
}

// FUNCTION: CARMA2_HW 0x004c9b40
void C2_HOOK_FASTCALL ApplyPedFPChange(tPipe_chunk** pChunk) {
    tPipe_chunk_ped_fp_change* chunk = (tPipe_chunk_ped_fp_change*)*pChunk;

    return WorkerPedFPChange(chunk->field_0x0, *(tPipe_stack_word*)&chunk->field_0x8);
}

// FUNCTION: CARMA2_HW 0x004ca230
void C2_HOOK_FASTCALL UndoPedFPChange(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_ped_fp_change* chunk = (tPipe_chunk_ped_fp_change*)*pChunk;

    return WorkerPedFPChange(chunk->field_0x0, *(tPipe_stack_word*)&chunk->field_0x4);
}

// FUNCTION: CARMA2_HW 0x004c9b50
void C2_HOOK_FASTCALL ApplyPedPos(tPipe_chunk** pChunk) {
    tPipe_chunk_ped_pos* chunk = (tPipe_chunk_ped_pos*)*pChunk;

    return WorkerPedPos(chunk->field_0x0, &chunk->field_0x10);
}

// FUNCTION: CARMA2_HW 0x004ca240
void C2_HOOK_FASTCALL UndoPedPos(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_ped_pos* chunk = (tPipe_chunk_ped_pos*)*pChunk;

    return WorkerPedPos(chunk->field_0x0, &chunk->field_0x4);
}

// FUNCTION: CARMA2_HW 0x004c9b60
void C2_HOOK_FASTCALL ApplyPedModelChange(tPipe_chunk** pChunk) {
    tPipe_chunk_ped_model_change* chunk = (tPipe_chunk_ped_model_change*)*pChunk;

    return WorkerPedModelChange(chunk->field_0x0, (int)chunk->field_0x4, (int)chunk->field_0x6, (int)chunk->field_0x8);
}

// FUNCTION: CARMA2_HW 0x004ca250
void C2_HOOK_FASTCALL UndoPedModelChange(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_ped_model_change* chunk = (tPipe_chunk_ped_model_change*)*pChunk;

    return WorkerPedModelChangeUndo(chunk->field_0x0, (int)chunk->field_0x4, chunk->field_0xc, (int)chunk->field_0xa);
}

// FUNCTION: CARMA2_HW 0x004c9b80
void C2_HOOK_FASTCALL ApplyPHILObject(tPipe_chunk** pChunk) {
    tPipe_chunk_phil_object* chunk = (tPipe_chunk_phil_object*)*pChunk;

    return WorkerPHILObject(chunk->field_0x0, &chunk->field_0x4);
}

// FUNCTION: CARMA2_HW 0x004c9b90
void C2_HOOK_FASTCALL ApplyGibShower(tPipe_chunk** pChunk) {
    tPipe_chunk_gib_shower* chunk = (tPipe_chunk_gib_shower*)*pChunk;

    return WorkerGibShower(chunk->field_0x0, chunk->field_0xc, chunk->field_0x8, chunk->field_0x10, chunk->field_0x4, &chunk->field_0x14, &chunk->field_0x20, &chunk->field_0x2c);
}

// FUNCTION: CARMA2_HW 0x004ca270
void C2_HOOK_FASTCALL UndoGibShower(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_gib_shower* prev = (tPipe_chunk_gib_shower*)pPrev_chunk;

    if (prev == NULL) {
        return;
    }

    return WorkerGibShowerUndo(((tPipe_chunk_gib_shower*)*pChunk)->field_0xc, prev->field_0x0, prev->field_0xc, prev->field_0x8, prev->field_0x10, prev->field_0x4, &prev->field_0x14, &prev->field_0x20, &prev->field_0x2c);
}

// FUNCTION: CARMA2_HW 0x004c9bc0
void C2_HOOK_FASTCALL ApplyBloodSpurt(tPipe_chunk** pChunk) {
    tPipe_chunk_blood_spurt* chunk = (tPipe_chunk_blood_spurt*)*pChunk;

    DoBloodSpurt(chunk->pOwner, chunk->field_0x8, chunk->field_0x4, &chunk->field_0xc, &chunk->field_0x18, &chunk->field_0x24);
}

// FUNCTION: CARMA2_HW 0x004ca2a0
void C2_HOOK_FASTCALL UndoBloodSpurt(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

    if (pPrev_chunk != NULL) {
        tPipe_chunk_blood_spurt* chunk = (tPipe_chunk_blood_spurt*)*pChunk;
        tPipe_chunk_blood_spurt* prev = (tPipe_chunk_blood_spurt*)pPrev_chunk;

        DoBloodSpurtUndo(chunk->field_0x8, prev->pOwner, prev->field_0x8, prev->field_0x4, &prev->field_0xc, &prev->field_0x18, &prev->field_0x24);
    }
}

// FUNCTION: CARMA2_HW 0x004c9be0
void C2_HOOK_FASTCALL ApplyGrooveOnOff(tPipe_chunk** pChunk) {
    tPipe_chunk_groove_on_off* chunk = (tPipe_chunk_groove_on_off*)*pChunk;

    if (chunk->field_0x4 != 0) {
        return EnableGroovidelic((br_actor*)chunk->field_0x0, chunk->field_0x8);
    }
    return WorkerGrooveOff((br_actor*)chunk->field_0x0, chunk->field_0x8);
}

// FUNCTION: CARMA2_HW 0x004ca2d0
void C2_HOOK_FASTCALL UndoGrooveOnOff(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_groove_on_off* chunk = (tPipe_chunk_groove_on_off*)*pChunk;

    if (chunk->field_0x4 != 0) {
        return WorkerGrooveOff((br_actor*)chunk->field_0x0, chunk->field_0x8);
    }
    return EnableGroovidelic((br_actor*)chunk->field_0x0, chunk->field_0x8);
}

// FUNCTION: CARMA2_HW 0x004c9c00
void C2_HOOK_FASTCALL ApplyPowerupGot(tPipe_chunk** pChunk) {
    tPipe_chunk_powerup* chunk = (tPipe_chunk_powerup*)*pChunk;

    return WorkerPowerupGot(chunk->field_0x0, chunk->field_0x4);
}

// FUNCTION: CARMA2_HW 0x004ca2f0
void C2_HOOK_FASTCALL UndoPowerupGot(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_powerup* chunk = (tPipe_chunk_powerup*)*pChunk;

    return WorkerPowerupLose(chunk->field_0x0, chunk->field_0x4);
}

// FUNCTION: CARMA2_HW 0x004c9c10
void C2_HOOK_FASTCALL ApplyPowerupLose(tPipe_chunk** pChunk) {
    tPipe_chunk_powerup* chunk = (tPipe_chunk_powerup*)*pChunk;

    return WorkerPowerupLose(chunk->field_0x0, chunk->field_0x4);
}

// FUNCTION: CARMA2_HW 0x004ca300
void C2_HOOK_FASTCALL UndoPowerupLose(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_powerup* chunk = (tPipe_chunk_powerup*)*pChunk;

    return WorkerPowerupGot(chunk->field_0x0, chunk->field_0x4);
}

// FUNCTION: CARMA2_HW 0x004c9c20
void C2_HOOK_FASTCALL ApplyFunkEnable(tPipe_chunk** pChunk) {
    tPipe_chunk_funk_enable* chunk = (tPipe_chunk_funk_enable*)*pChunk;

    return WorkerFunkEnable(chunk->field_0x0, chunk->field_0x4);
}

// FUNCTION: CARMA2_HW 0x004ca310
void C2_HOOK_FASTCALL UndoFunkEnable(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_funk_enable* chunk = (tPipe_chunk_funk_enable*)*pChunk;

    return WorkerFunkEnable(chunk->field_0x0, chunk->field_0x4 == 0);
}

// FUNCTION: CARMA2_HW 0x004c9c30
void C2_HOOK_FASTCALL ApplyVanishDismembered(tPipe_chunk** pChunk) {
    tPipe_chunk_vanish* chunk = (tPipe_chunk_vanish*)*pChunk;

    DoVanishDismembered(chunk->field_0x0, chunk->field_0x8);
}

// FUNCTION: CARMA2_HW 0x004ca330
void C2_HOOK_FASTCALL UndoVanishDismembered(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_vanish* chunk = (tPipe_chunk_vanish*)*pChunk;

    DoVanishDismembered(chunk->field_0x0, chunk->field_0x4);
}

// FUNCTION: CARMA2_HW 0x004c9c40
void C2_HOOK_FASTCALL ApplyDSModel(tPipe_chunk** pChunk) {
    tPipe_chunk* chunk = *pChunk;

    if (*(tS16*)((tU8*)chunk + 4) != 0) {
        MakeModelMaterialsDoubleSided((br_model*)*(void**)chunk);
    } else {
        DRActorEnumRecurse(((tCar_spec*)*(void**)chunk)->car_model_actor, (br_actor_enum_cbfn*)MakeCarModelsMaterialsSingleSided, *(void**)chunk);
    }
}

// FUNCTION: CARMA2_HW 0x004c9c70
void C2_HOOK_FASTCALL ApplyPedDiagnostics(tPipe_chunk** pChunk) {
    tU8* chunk = (tU8*)*pChunk;

    return WorkerPedDiagnostics(*(void**)chunk, chunk + 4, chunk + 0x58, chunk + 0x144);
}

// FUNCTION: CARMA2_HW 0x004c9c90
void C2_HOOK_FASTCALL ApplyVector3(tPipe_chunk** pChunk) {
    ((tPipe_chunk_vector3*)*pChunk)->p_vector->v[0] += ((tPipe_chunk_vector3*)*pChunk)->delta.v[0];
    ((tPipe_chunk_vector3*)*pChunk)->p_vector->v[1] += ((tPipe_chunk_vector3*)*pChunk)->delta.v[1];
    ((tPipe_chunk_vector3*)*pChunk)->p_vector->v[2] += ((tPipe_chunk_vector3*)*pChunk)->delta.v[2];
}

// FUNCTION: CARMA2_HW 0x004ca360
void C2_HOOK_FASTCALL UndoVector3(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    ((tPipe_chunk_vector3*)*pChunk)->p_vector->v[0] -= ((tPipe_chunk_vector3*)*pChunk)->delta.v[0];
    ((tPipe_chunk_vector3*)*pChunk)->p_vector->v[1] -= ((tPipe_chunk_vector3*)*pChunk)->delta.v[1];
    ((tPipe_chunk_vector3*)*pChunk)->p_vector->v[2] -= ((tPipe_chunk_vector3*)*pChunk)->delta.v[2];
}

// FUNCTION: CARMA2_HW 0x004c9310
void C2_HOOK_FASTCALL ApplyExtendedSplash(tPipe_chunk** pChunk) {
    tPipe_chunk_extended_splash* chunk = (tPipe_chunk_extended_splash*)*pChunk;

    return WorkerExtendedSplash(chunk->field_0x0, (tU8)chunk->field_0x4, chunk->field_0x8, &chunk->field_0xc, &chunk->field_0x18);
}

// FUNCTION: CARMA2_HW 0x004c9ee0
void C2_HOOK_FASTCALL UndoExtendedSplash(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    SetPipeUndoFlag();
    if (pPrev_chunk != NULL) {
        tPipe_chunk_extended_splash* chunk = (tPipe_chunk_extended_splash*)pPrev_chunk;

        WorkerExtendedSplash(chunk->field_0x0, (tU8)chunk->field_0x4, chunk->field_0x8, &chunk->field_0xc, &chunk->field_0x18);
        ClearPipeUndoFlag();
    }
}

// FUNCTION: CARMA2_HW 0x004c9cc0
void C2_HOOK_FASTCALL ApplyDroneRender(tPipe_chunk** pChunk) {
    SetDroneRender(*(tU32*)*pChunk, *(tU16*)((tU8*)*pChunk + 4));
}

// FUNCTION: CARMA2_HW 0x004c9cd0
void C2_HOOK_FASTCALL ApplyDroneCornerPos(tPipe_chunk** pChunk) {
    tPipe_chunk_drone_corner_pos* chunk = (tPipe_chunk_drone_corner_pos*)*pChunk;

    return WorkerDroneCornerPos(chunk->field_0x0, chunk->field_0x4, chunk->field_0x6, chunk->field_0x8, &chunk->field_0xa);
}

// FUNCTION: CARMA2_HW 0x004c9cf0
void C2_HOOK_FASTCALL ApplyDroneStraightPos(tPipe_chunk** pChunk) {
    tPipe_chunk_drone_straight_pos* chunk = (tPipe_chunk_drone_straight_pos*)*pChunk;

    return WorkerDroneStraightPos(chunk->field_0x0, chunk->field_0x4, chunk->field_0x8, chunk->field_0xa);
}

// FUNCTION: CARMA2_HW 0x004c9d10
void C2_HOOK_FASTCALL ApplyDroneUnused(tPipe_chunk** pChunk) {
}

// FUNCTION: CARMA2_HW 0x004d5b10
void C2_HOOK_FASTCALL ARResetNapalmBolts(void) {
    int field = 0x69b9f4;
    int i;

    do {
        *(int*)(field - 4) = 0;

        for (i = 0; i < 7; i++) {
            *(tU8*)(*(int*)(field + i * 4) + 0x20) = 1;
        }

        field += 0x6c;
    } while (field < 0x69bc10);
}

// FUNCTION: CARMA2_HW 0x004c9d20
void C2_HOOK_FASTCALL ApplyNapalmBolt(tPipe_chunk** pChunk) {
    tPipe_chunk_napalm_bolt* chunk = (tPipe_chunk_napalm_bolt*)*pChunk;

    return WorkerNapalmBolt(chunk->field_0x0, chunk->field_0x4, &chunk->field_0x8);
}

// FUNCTION: CARMA2_HW 0x004ca390
void C2_HOOK_FASTCALL UndoNapalmBolt(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

    if (pPrev_chunk != NULL) {
        tPipe_chunk_napalm_bolt* chunk = (tPipe_chunk_napalm_bolt*)*pChunk;
        tPipe_chunk_napalm_bolt* prev = (tPipe_chunk_napalm_bolt*)pPrev_chunk;

        if (prev->field_0x4 == chunk->field_0x4) {
            return WorkerNapalmBolt(prev->field_0x0, prev->field_0x4, &prev->field_0x8);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004c9d40
void C2_HOOK_FASTCALL ApplyPowerupRespawn(tPipe_chunk** pChunk) {
    tPipe_chunk_powerup* chunk = (tPipe_chunk_powerup*)*pChunk;

    return WorkerPowerupRespawn(chunk->field_0x0, chunk->field_0x4);
}

// FUNCTION: CARMA2_HW 0x004ca3b0
void C2_HOOK_FASTCALL UndoPowerupRespawn(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_powerup* chunk = (tPipe_chunk_powerup*)*pChunk;

    return WorkerPowerupRespawnUndo(chunk->field_0x0, chunk->field_0x4);
}

// FUNCTION: CARMA2_HW 0x004c9d50
void C2_HOOK_FASTCALL ApplyShitMine(tPipe_chunk** pChunk) {
    tPipe_chunk_shit_mine* chunk = (tPipe_chunk_shit_mine*)*pChunk;

    return WorkerShitMine(chunk->field_0x0, &chunk->field_0x4);
}

// FUNCTION: CARMA2_HW 0x004ca3c0
void C2_HOOK_FASTCALL UndoShitMine(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_shit_mine* chunk = (tPipe_chunk_shit_mine*)*pChunk;

    return WorkerEndShitMine(chunk->field_0x0, &chunk->field_0x4);
}

// FUNCTION: CARMA2_HW 0x004c9d60
void C2_HOOK_FASTCALL ApplyEndShitMine(tPipe_chunk** pChunk) {
    tPipe_chunk_shit_mine* chunk = (tPipe_chunk_shit_mine*)*pChunk;

    return WorkerEndShitMine(chunk->field_0x0, &chunk->field_0x4);
}

// FUNCTION: CARMA2_HW 0x004ca3d0
void C2_HOOK_FASTCALL UndoEndShitMine(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_shit_mine* chunk = (tPipe_chunk_shit_mine*)*pChunk;

    return WorkerShitMine(chunk->field_0x0, &chunk->field_0x4);
}

// FUNCTION: CARMA2_HW 0x004c9d70
void C2_HOOK_FASTCALL ApplyTransformType(tPipe_chunk** pChunk) {
    ((br_actor*)((tPipe_chunk_transform_type*)*pChunk)->pOwner)->t.type = ((tPipe_chunk_transform_type*)*pChunk)->type_to;
}

// FUNCTION: CARMA2_HW 0x004ca3e0
void C2_HOOK_FASTCALL UndoTransformType(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    ((br_actor*)((tPipe_chunk_transform_type*)*pChunk)->pOwner)->t.type = ((tPipe_chunk_transform_type*)*pChunk)->type_from;
}

// FUNCTION: CARMA2_HW 0x004c9d80
void C2_HOOK_FASTCALL ApplyOppoRenderage(tPipe_chunk** pChunk) {
    SetOppoRender((tOpponent_spec*)*(tU32*)*pChunk, *(tU16*)((tU8*)*pChunk + 4));
}
// FUNCTION: CARMA2_HW 0x004e6280
void C2_HOOK_FASTCALL ActualActionReplayHeadups(int pSpecial_zappy_bastard) {

    NOT_IMPLEMENTED();
}

// Replay pipe worker helpers live in replay_workers.c so they are not inlined
// into the wrappers above; prototypes are at the top of this file.
