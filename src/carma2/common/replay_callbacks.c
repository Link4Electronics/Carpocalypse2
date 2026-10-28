/* port: retain double timer arithmetic and mapped replay-rate constants. */
#include "replay_callbacks.h"

#include "crush.h"
#include "flap.h"
#include "funks.h"
#include "font.h"
#include "grafdata.h"
#include "globvars.h"
#include "input.h"
#include "piping.h"
#include "powerups.h"
#include "replay.h"
#include "smashing.h"
#include "sound.h"
#include "spark.h"
#include "pedestrn.h"
#include "platform.h"
#include "graphics1.h"
#include "graphics.h"
#include "graphics2.h"
#include "utility.h"
#include "video.h"

#include "carpocalypse2_macros.h"
#include "world.h"
#include "c2_math.h"

// Unmapped replay pipe worker helpers (orig PDB has no symbols for these;
// they exist only so the wrappers can emit their call/jmp operands).
extern void C2_HOOK_FASTCALL WorkerPedPos(tPedestrian* pPed, br_vector3* pVec);
extern void C2_HOOK_FASTCALL WorkerNapalmBolt(int pBolt_index, tPedestrian* pPed, br_vector3* pPositions);
extern void C2_HOOK_FASTCALL WorkerPedPhysics(int field_0x0, br_matrix34* pMatrix);
extern void C2_HOOK_FASTCALL WorkerDroneCornerPos(int field_0x0, int field_0x4, tU16 field_0x6, tU16 field_0x8, tU16* pField_0xa);
extern void C2_HOOK_FASTCALL WorkerNonCarCreation(int field_0x0);
extern void C2_HOOK_FASTCALL WorkerNonCarCreationUndo(int field_0x0);
extern void C2_HOOK_FASTCALL WorkerSmashTextureChange(br_material* pMaterial, tCar_spec* pCar, br_pixelmap* pOld_texture, br_pixelmap* pNew_texture);
extern void C2_HOOK_FASTCALL WorkerPedDismember(int field_0x0, int field_0x4, int field_0x7);
extern void C2_HOOK_FASTCALL WorkerPedDismemberUndo(tPedestrian* pPed, int pBone, tS8 pField_0x6);
extern void C2_HOOK_FASTCALL WorkerPedModelChange(int field_0x0, int field_0x4, int field_0x6, int field_0x8);
extern void C2_HOOK_FASTCALL WorkerPedModelChangeUndo(int field_0x0, int field_0x4, int field_0xc, int field_0xa);
extern void C2_HOOK_FASTCALL WorkerDroneStraightPos(int field_0x0, int field_0x4, tU16 field_0x8, tU16 field_0xa);
extern void C2_HOOK_FASTCALL WorkerRepulseRay(int field_0x0, int field_0x4, int field_0x8, int field_0xc);
extern void C2_HOOK_FASTCALL WorkerSmashModelChangeBUndo(int field_0x0);
extern void C2_HOOK_FASTCALL WorkerCrush(int field_0x4, void* pField_0x8, void* pField_0xe);
extern void C2_HOOK_FASTCALL WorkerShrapnel(int field_0x0, void* pField_0x4, tU16 field_0x10, int field_0x14);
extern void C2_HOOK_FASTCALL WorkerPedMove(tPedestrian* pPed, int pMove_id, int pSpeed_bits, int pField_0x8, int pField_0x14, int pFrame, br_vector3* pField_0x24, int pAction, br_vector3* pField_0x38);
extern void C2_HOOK_FASTCALL WorkerPedMoveUndo(tPedestrian* pPed, int field_0x6, float field_0x10, int zero, int zero2, int field_0xa, br_vector3* pField_0x18, int field_0x30, br_vector3* pField_0x38);
extern void C2_HOOK_FASTCALL WorkerPedStatus(int field_0x0, tU8 field_0x4, tU8 field_0x5, tU8 field_0x6, tU8 field_0x7, tU8 field_0x9, tU8 field_0xb, br_vector3* p0xc, br_vector3* p0x18, br_vector3* p0x24);
extern void C2_HOOK_FASTCALL WorkerPedStatusUndo(int field_0x0, tU8 field_0x4, tU8 field_0x5, tU8 field_0x6, tU8 field_0x7, tU8 field_0x8, tU8 field_0xa, br_vector3* p0xc, br_vector3* p0x18, br_vector3* p0x24);
extern tCar_spec* C2_HOOK_FASTCALL GetCarSpec(tVehicle_type pCategory, int pIndex);
// FUN_004ca650 ignores its (fake-this)call edx argument and the original callers do not set it.
typedef void (C2_HOOK_FASTCALL *tPedFPChangeNoEdx)(tPedestrian* pPed, float pSpeed);
// Real (already implemented elsewhere) functions the replay wrappers tail-call.
extern void C2_HOOK_FASTCALL FUN_004df210(tPowerup* pPowerup, tCar_spec* pCar);
extern void C2_HOOK_FASTCALL ARUndoPowerupRespawn(br_actor* pActor, int pUnused);
extern void C2_HOOK_FASTCALL ARDoShitMine(br_actor* pActor, const br_vector3* pUnused);
extern void C2_HOOK_FASTCALL ARUndoShitMine(br_actor* pActor, const br_vector3* pUnused);
extern void C2_HOOK_FASTCALL CopyExplosionData(int pIndex, tExplosion* pExplosion);
extern void C2_HOOK_FASTCALL SmashRemoveSomeFaces(br_model* pModel, int pCount);
extern void C2_HOOK_FASTCALL TurnOffSmashModel(char* pArg);
extern void C2_HOOK_FASTCALL TurnOnSmashDecal(int pIndex);
extern void C2_HOOK_FASTCALL TurnOffSmashDecal(int pIndex);
extern void C2_HOOK_FASTCALL PHILSetObjectMatrix(tPhysics_object* pObject, br_matrix34* pMat);
extern void C2_HOOK_FASTCALL AdjustNonCar(br_actor* pActor, const br_matrix34* pMatrix);
extern void C2_HOOK_FASTCALL AdjustSmokeColumn(int pIndex, const tSmoke_column_core* pCore, int pVertex_index, int pColour);
extern void C2_HOOK_FASTCALL AddFacesToModel(br_model* pModel, int pFace_count, const tU16* pFace_data, br_material* pMaterial);
extern void C2_HOOK_FASTCALL RecordReplaySplash(void* pObject, int pType, tU32 pWater_density, const tU32* pNormal, const tU32* pBounds);
extern void C2_HOOK_FAKE_THISCALL ChangeSmashModel(void* pThis, undefined4 pArg2, undefined4 pValue);
extern void C2_HOOK_FASTCALL FUN_0043be50(br_model* pModel, const tCompressed_vector3* pVectors, const tU16* pDeltas);
extern void C2_HOOK_FASTCALL ARApplyModelMash(br_model* pModel, const tCompressed_vector3* pDeltas);
extern void C2_HOOK_FASTCALL ARUndoModelMash(br_model* pModel, const tCompressed_vector3* pOffsets);
extern void C2_HOOK_FAKE_THISCALL AdjustOilSpill(int pIndex, const br_matrix34* pMatrix, float pFull_size, float pGrow_rate, tU32 pSpill_time, tU32 pUnused, tCar_spec* pCar, const br_vector3* pPos, tU32 pUnused2);
extern void C2_HOOK_FASTCALL FUN_004ccae0(tU32 pFragment_owner, tU32 pTime, tU32 pSeed, float pForce, int pStarting_size, br_vector3* pInitial_position, br_vector3* pInitial_velocity, br_vector3* pDirection);
extern void C2_HOOK_FASTCALL FUN_004ccc40(int pCurrent_time, tU32 pOwner, int pSpurt_time, tU32 pSeed, float pForce, int pStarting_size, br_vector3* pInitial_position, br_vector3* pInitial_velocity, br_vector3* pDirection);
extern void C2_HOOK_FASTCALL ARUndoSplitWeld(tCar_spec* pCar_spec);
extern void C2_HOOK_FASTCALL DoInitialTail(void);
extern void C2_HOOK_FASTCALL UndoInitialTail(void);
extern void C2_HOOK_FASTCALL AdjustFunkEnable(int pIndex, tS16 pEnable);
extern void C2_HOOK_FASTCALL AdjustPedDiagnostics(tU32 pSubject_index, void* pUnused2, void* pUnused3, void* pUnused4);
extern void C2_HOOK_FASTCALL WorkerPowerupRespawn(int field_0x0, int field_0x4);
extern void C2_HOOK_FASTCALL WorkerShrapnelShower(int pOwner, int* pShrapnel_spec, tU32 pSeed, undefined4 pArg4, undefined4 pArg5, undefined4 pArg6, undefined4 pArg7, int pTime, undefined4 pArg9, br_matrix34* pTransform, undefined4 pFragment_owner, br_vector3* pPosition, float pDistance, undefined4 pArg14, undefined4 pArg15);
extern tNapalm_bolt gNapalm_bolts[5];
extern int gDisable_sound;
extern void C2_HOOK_FASTCALL WorkerFlame(int index, int flag, float f1, float f2, float f3, float f4);
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

    return ApplyTime(((tPipe_chunk_frame_bound*)*pChunk)->field_0x4);
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

    return FUN_0043be50((br_model*)chunk->field_0x4, (const tCompressed_vector3*)&chunk->field_0x8[0], (const tU16*)&chunk->field_0x8[6]);
}

// FUNCTION: CARMA2_HW 0x004c9490
void C2_HOOK_FASTCALL ApplyCar(tPipe_chunk** pChunk) {
    tU32 code = *(tU32*)*pChunk;
    tCar_spec* car;
    br_vector3 scaled;
    br_vector4 out4;

    if ((code & 0xffffff00u) == 0) {
        car = &gProgram_state.current_car;
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
        car = &gProgram_state.current_car;
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
        car = &gProgram_state.current_car;
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
        car = &gProgram_state.current_car;
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
            return DoInitialTail();
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
        return UndoInitialTail();
    }
}

// FUNCTION: CARMA2_HW 0x004c9140
void C2_HOOK_FASTCALL ApplySpark(tPipe_chunk** pChunk) {
    tPipe_chunk_spark_replay* chunk = (tPipe_chunk_spark_replay*)*pChunk;

    return CreateSingleSpark(chunk->field_0x0, (br_vector3*)&chunk->field_0x4[0], (br_vector3*)&chunk->field_0x4[0xc]);
}

// FUNCTION: CARMA2_HW 0x004f87b0
void C2_HOOK_FASTCALL ResetShrapnel(void) {
    int i;

    if (gShrapnel_flags == 0) {
        return;
    }
    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gShrapnel); i++) {
        if (((1 << i) & gShrapnel_flags) != 0 && gShrapnel[i].actor->parent != NULL) {
            BrActorRemove(gShrapnel[i].actor);
        }
    }
    gShrapnel_flags = 0;
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

    return SetGrooveInterrupt(chunk->field_0x0, (const br_matrix34*)&chunk->field_0x14, chunk->field_0x4, chunk->field_0x8, *(float*)&chunk->field_0xc, *(float*)&chunk->field_0x10);
}

// FUNCTION: CARMA2_HW 0x004c91b0
void C2_HOOK_FASTCALL ApplyNonCar(tPipe_chunk** pChunk) {
    tPipe_chunk_non_car_replay* chunk = (tPipe_chunk_non_car_replay*)*pChunk;

    return AdjustNonCar((br_actor*)chunk->field_0x4, (const br_matrix34*)&chunk->field_0x8);
}

// FUNCTION: CARMA2_HW 0x004fb510
void C2_HOOK_FASTCALL ResetSmoke(void) {

    gSmoke_flags = 0;
}

// GLOBAL: CARMA2_HW 0x0075bc3c
tCar_spec* gCurrent_car;

// C2PORT-NONMATCHING: best 100% | difftest PASS 2000 trials (spec 004c91c0_ApplySmoke.json) | reason verifies smoke fields and position arguments
// FUNCTION: CARMA2_HW 0x004c91c0
void C2_HOOK_FASTCALL ApplySmoke(tPipe_chunk** pChunk) {
    tU8* chunk = (tU8*)*pChunk;
    br_vector3 tmp;

    tmp.v[0] = (float)((double)(int)(tS16)*(tU16*)(chunk + 4) * 0.00125);
    tmp.v[1] = (float)((double)(int)(tS16)*(tU16*)(chunk + 6) * 0.00125);
    tmp.v[2] = (float)((double)(int)(tS16)*(tU16*)(chunk + 8) * 0.00125);
    tmp.v[0] += *(float*)((tU8*)gCurrent_car + 0x50);
    tmp.v[1] += *(float*)((tU8*)gCurrent_car + 0x54);
    tmp.v[2] += *(float*)((tU8*)gCurrent_car + 0x58);

    return AdjustSmoke(*(int*)(chunk + 0), *(tU8*)(chunk + 0xd), &tmp, (float)*(tU16*)(chunk + 0xa) * 0.0009765625f, (float)*(tU8*)(chunk + 0xc) * 0.00390625f);
}

// FUNCTION: CARMA2_HW 0x004c9330
void C2_HOOK_FASTCALL ApplyOilSpill(tPipe_chunk** pChunk) {
    tPipe_chunk_oil_spill* chunk = (tPipe_chunk_oil_spill*)*pChunk;

    return AdjustOilSpill(chunk->field_0x0, (const br_matrix34*)&chunk->field_0x4, *(float*)&chunk->field_0x34, *(float*)&chunk->field_0x38, chunk->field_0x3c, 0, (tCar_spec*)chunk->field_0x44, &chunk->field_0x48, chunk->field_0x54);
}

// FUNCTION: CARMA2_HW 0x004c9280
void C2_HOOK_FASTCALL ApplySmokeColumn(tPipe_chunk** pChunk) {
    tPipe_chunk_smoke_column_replay* chunk = (tPipe_chunk_smoke_column_replay*)*pChunk;

    return AdjustSmokeColumn(chunk->field_0x0 & 0x3fff, (const tSmoke_column_core*)chunk->pad, chunk->field_0x14, chunk->field_0x0 >> 14);
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

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x004d6180
void C2_HOOK_FASTCALL AdjustProxRay(int pRay_index, tU16 pCar_ID, tU16 pPed_index, tU32 pTime) {
    tCar_spec* car;

    if ((pCar_ID >> 8) == 0) {
        car = &gProgram_state.current_car;
    } else {
        car = GetCarSpec(pCar_ID >> 8, pCar_ID & 0xff);
    }
    gProximity_rays[pRay_index].car = car;
    gProximity_rays[pRay_index].ped = &gPedestrian_array[pPed_index];
    gProximity_rays[pRay_index].start_time = pTime;
}
#pragma auto_inline(on)

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

    return AdjustProxRay(chunk->field_0x0, chunk->field_0x6, chunk->field_0x4, chunk->field_0x8);
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

    return ARApplyModelMash((br_model*)chunk->field_0x4, (const tCompressed_vector3*)chunk->field_0x8);
}

// FUNCTION: CARMA2_HW 0x004c9f80
void C2_HOOK_FASTCALL UndoModelMash(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_model_mash* chunk = (tPipe_chunk_model_mash*)*pChunk;

    return ARUndoModelMash((br_model*)chunk->field_0x4, (const tCompressed_vector3*)chunk->field_0x8);
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
            return ARApplySplitWeld(&gProgram_state.current_car, (float*)&swGetChunk(pChunk)->field_0x4);
        }

        return ARApplySplitWeld(GetCarSpec(fn >> 8, fn & 0xffu), (float*)&swGetChunk(pChunk)->field_0x4);
    }

    if ((fn & 0xffffff00u) == 0) {
        return ARUndoSplitWeld(&gProgram_state.current_car);
    }

    return ARUndoSplitWeld(GetCarSpec(fn >> 8, fn & 0xffu));
}

// FUNCTION: CARMA2_HW 0x004c9fd0
void C2_HOOK_FASTCALL UndoSplitWeld(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tU32 fn = (*(tPipe_chunk_split_weld**)*pChunk)->field_0x0;

    if ((*(tPipe_chunk_split_weld**)*pChunk)->field_0x14) {
        if ((fn & 0xffffff00u) == 0) {
            return ARUndoSplitWeld(&gProgram_state.current_car);
        }

        return ARUndoSplitWeld(GetCarSpec(fn >> 8, fn & 0xffu));
    }

    if ((fn & 0xffffff00u) == 0) {
        return ARApplySplitWeld(&gProgram_state.current_car, (float*)&swGetChunk(pChunk)->field_0x4);
    }

    return ARApplySplitWeld(GetCarSpec(fn >> 8, fn & 0xffu), (float*)&swGetChunk(pChunk)->field_0x4);
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

    memcpy(&gMatrix34_006b78a0, &chunk->field_0xa4, sizeof(chunk->field_0xa4));

    return WorkerShrapnelShower(((tPipe_chunk_shrapnel_shower*)*pChunk)->field_0x4, (int*)((tPipe_chunk_shrapnel_shower*)*pChunk)->field_0x84, ((tPipe_chunk_shrapnel_shower*)*pChunk)->field_0x44, &((tPipe_chunk_shrapnel_shower*)*pChunk)->field_0x8, &((tPipe_chunk_shrapnel_shower*)*pChunk)->field_0x20, &((tPipe_chunk_shrapnel_shower*)*pChunk)->field_0x2c, ((tPipe_chunk_shrapnel_shower*)*pChunk)->field_0x50, ((tPipe_chunk_shrapnel_shower*)*pChunk)->field_0x48, ((tPipe_chunk_shrapnel_shower*)*pChunk)->field_0x0, &((tPipe_chunk_shrapnel_shower*)*pChunk)->field_0x54, &((tPipe_chunk_shrapnel_shower*)*pChunk)->field_0x88, &((tPipe_chunk_shrapnel_shower*)*pChunk)->field_0x14, *(float*)&((tPipe_chunk_shrapnel_shower*)*pChunk)->field_0xa0, &((tPipe_chunk_shrapnel_shower*)*pChunk)->field_0x38, 0);
}

// FUNCTION: CARMA2_HW 0x004ca070
void C2_HOOK_FASTCALL UndoShrapnelShower(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_shrapnel_shower* chunk = (tPipe_chunk_shrapnel_shower*)*pChunk;
    tPipe_chunk_shrapnel_shower* prev = (tPipe_chunk_shrapnel_shower*)pPrev_chunk;

    if (chunk->field_0x84 != 0 || prev == NULL || prev->field_0x4 == 0 || prev->field_0x84 == 0) {
        return;
    }

    memcpy(&gMatrix34_006b78a0, &prev->field_0xa4, sizeof(prev->field_0xa4));

    return FUN_004f0380(chunk->field_0x48, prev->field_0x4, (int*)prev->field_0x84, prev->field_0x44, &prev->field_0x8, &prev->field_0x20, &prev->field_0x2c, prev->field_0x50, prev->field_0x48, prev->field_0x0, &prev->field_0x54, &prev->field_0x88, &prev->field_0x14, *(float*)&prev->field_0xa0, &prev->field_0x38);
}

// FUNCTION: CARMA2_HW 0x004c99d0
void C2_HOOK_FASTCALL ApplyExplosion(tPipe_chunk** pChunk) {
    tPipe_chunk_explosion* chunk = (tPipe_chunk_explosion*)*pChunk;

    return CopyExplosionData(chunk->field_0x0, (tExplosion*)&chunk->field_0x4);
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
        return ChangeSmashModel((void*)chunk->field_0x0, chunk->field_0x4, chunk->field_0x8);
    }
    return TurnOffSmashModel((char*)chunk->field_0x0);
}

// FUNCTION: CARMA2_HW 0x004ca110
void C2_HOOK_FASTCALL UndoSmashModelChange(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_smash_model_change* chunk = (tPipe_chunk_smash_model_change*)*pChunk;

    if (chunk->field_0x8) {
        return ChangeSmashModel((void*)chunk->field_0x0, chunk->field_0x8, chunk->field_0x4);
    }
    return WorkerSmashModelChangeBUndo(chunk->field_0x0);
}

// FUNCTION: CARMA2_HW 0x004ca460
int C2_HOOK_FASTCALL CalcRemoveFacesLength(tPipe_chunk* pChunk) {

    return 8 + 8 * ((tPipe_chunk_remove_faces*)pChunk)->count;
}

// FUNCTION: CARMA2_HW 0x004ca470
int C2_HOOK_FASTCALL FUN_004ca470(tPedestrian* pPed) {

    if (pPed->field_0x0c != NULL && pPed->field_0x0c->field_0xb0 != NULL) {
        return pPed->field_0x0c->field_0xb0->field_0x4;
    }
    return 0;
}

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x004ca4d0
void C2_HOOK_FASTCALL FUN_004ca4d0(int pIndex, const br_vector3* pDir) {

    if (!gBOOL_00744804) {
        SetCharacterDirection(gPedestrian_array[pIndex & 0xffff].character, pDir, &y_unit_vector);
    }
}

// FUNCTION: CARMA2_HW 0x004ca650
void C2_HOOK_FAKE_THISCALL FUN_004ca650(tPedestrian* pPed, undefined4 pEdx, float pSpeed) {

    if (!gBOOL_00744804) {
        SetCharacterMove(pPed->character, -1, pSpeed, 0, 0, GetTotalTime());
    }
}
#pragma auto_inline(on)

// FUNCTION: CARMA2_HW 0x004c9a10
void C2_HOOK_FASTCALL ApplySmashRemoveFaces(tPipe_chunk** pChunk) {
    tPipe_chunk_remove_faces* chunk = (tPipe_chunk_remove_faces*)*pChunk;

    return SmashRemoveSomeFaces((br_model*)chunk->field_0x0, chunk->count);
}

// FUNCTION: CARMA2_HW 0x004ca130
void C2_HOOK_FASTCALL UndoSmashRemoveFaces(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_remove_faces_replay* chunk = (tPipe_chunk_remove_faces_replay*)*pChunk;

    return AddFacesToModel((br_model*)chunk->field_0x0, chunk->field_0x8, (const tU16*)&chunk->field_0xc, (br_material*)chunk->field_0x4);
}

// FUNCTION: CARMA2_HW 0x004c9a20
void C2_HOOK_FASTCALL ApplySmashDecal(tPipe_chunk** pChunk) {
    tPipe_chunk_smash_decal* chunk = (tPipe_chunk_smash_decal*)*pChunk;

    return TurnOnSmashDecal(chunk->field_0x0);
}

// FUNCTION: CARMA2_HW 0x004ca150
void C2_HOOK_FASTCALL UndoSmashDecal(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_smash_decal* chunk = (tPipe_chunk_smash_decal*)*pChunk;

    return TurnOffSmashDecal(chunk->field_0x0);
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

    return FUN_004ca4d0(chunk->field_0x0, &chunk->field_0x4);
}

// FUNCTION: CARMA2_HW 0x004c9ae0
void C2_HOOK_FASTCALL ApplyPedMove(tPipe_chunk** pChunk) {
    tPipe_chunk_ped_move* chunk = (tPipe_chunk_ped_move*)*pChunk;

    return WorkerPedMove((tPedestrian*)chunk->field_0x0, chunk->field_0x4, chunk->field_0xc, chunk->field_0x8 & 1, chunk->field_0x14, 0, &chunk->field_0x24, chunk->field_0x34, &chunk->field_0x38);
}

// FUNCTION: CARMA2_HW 0x004ca1e0
void C2_HOOK_FASTCALL UndoPedMove(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_ped_move* chunk = (tPipe_chunk_ped_move*)*pChunk;

    if (!chunk->field_0x14) {
        return WorkerPedMoveUndo((tPedestrian*)chunk->field_0x0, chunk->field_0x6, *(float*)&chunk->field_0x10, 0, 0, chunk->field_0xa, &chunk->field_0x18, chunk->field_0x30, &chunk->field_0x38);
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

    return ((tPedFPChangeNoEdx)FUN_004ca650)((tPedestrian*)chunk->field_0x0, *(float*)&chunk->field_0x8);
}

// FUNCTION: CARMA2_HW 0x004ca230
void C2_HOOK_FASTCALL UndoPedFPChange(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_ped_fp_change* chunk = (tPipe_chunk_ped_fp_change*)*pChunk;

    return ((tPedFPChangeNoEdx)FUN_004ca650)((tPedestrian*)chunk->field_0x0, *(float*)&chunk->field_0x4);
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

    return PHILSetObjectMatrix((tPhysics_object*)chunk->field_0x0, &chunk->field_0x4);
}

// FUNCTION: CARMA2_HW 0x004c9b90
void C2_HOOK_FASTCALL ApplyGibShower(tPipe_chunk** pChunk) {
    tPipe_chunk_gib_shower* chunk = (tPipe_chunk_gib_shower*)*pChunk;

    return FUN_004ccae0(chunk->field_0x0, chunk->field_0xc, chunk->field_0x8, *(float*)&chunk->field_0x10, chunk->field_0x4, &chunk->field_0x14, &chunk->field_0x20, &chunk->field_0x2c);
}

// FUNCTION: CARMA2_HW 0x004ca270
void C2_HOOK_FASTCALL UndoGibShower(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_gib_shower* prev = (tPipe_chunk_gib_shower*)pPrev_chunk;

    if (prev == NULL) {
        return;
    }

    return FUN_004ccc40(((tPipe_chunk_gib_shower*)*pChunk)->field_0xc, prev->field_0x0, prev->field_0xc, prev->field_0x8, *(float*)&prev->field_0x10, prev->field_0x4, &prev->field_0x14, &prev->field_0x20, &prev->field_0x2c);
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
    return DisableGroovidelic((br_actor*)chunk->field_0x0, chunk->field_0x8);
}

// FUNCTION: CARMA2_HW 0x004ca2d0
void C2_HOOK_FASTCALL UndoGrooveOnOff(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_groove_on_off* chunk = (tPipe_chunk_groove_on_off*)*pChunk;

    if (chunk->field_0x4 != 0) {
        return DisableGroovidelic((br_actor*)chunk->field_0x0, chunk->field_0x8);
    }
    return EnableGroovidelic((br_actor*)chunk->field_0x0, chunk->field_0x8);
}

// FUNCTION: CARMA2_HW 0x004c9c00
void C2_HOOK_FASTCALL ApplyPowerupGot(tPipe_chunk** pChunk) {
    tPipe_chunk_powerup* chunk = (tPipe_chunk_powerup*)*pChunk;

    return FUN_004df200((tPowerup*)chunk->field_0x0, (tCar_spec*)chunk->field_0x4);
}

// FUNCTION: CARMA2_HW 0x004ca2f0
void C2_HOOK_FASTCALL UndoPowerupGot(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_powerup* chunk = (tPipe_chunk_powerup*)*pChunk;

    return FUN_004df210((tPowerup*)chunk->field_0x0, (tCar_spec*)chunk->field_0x4);
}

// FUNCTION: CARMA2_HW 0x004c9c10
void C2_HOOK_FASTCALL ApplyPowerupLose(tPipe_chunk** pChunk) {
    tPipe_chunk_powerup* chunk = (tPipe_chunk_powerup*)*pChunk;

    return FUN_004df210((tPowerup*)chunk->field_0x0, (tCar_spec*)chunk->field_0x4);
}

// FUNCTION: CARMA2_HW 0x004ca300
void C2_HOOK_FASTCALL UndoPowerupLose(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_powerup* chunk = (tPipe_chunk_powerup*)*pChunk;

    return FUN_004df200((tPowerup*)chunk->field_0x0, (tCar_spec*)chunk->field_0x4);
}

// FUNCTION: CARMA2_HW 0x004c9c20
void C2_HOOK_FASTCALL ApplyFunkEnable(tPipe_chunk** pChunk) {
    tPipe_chunk_funk_enable* chunk = (tPipe_chunk_funk_enable*)*pChunk;

    return AdjustFunkEnable(chunk->field_0x0, chunk->field_0x4);
}

// FUNCTION: CARMA2_HW 0x004ca310
void C2_HOOK_FASTCALL UndoFunkEnable(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_funk_enable* chunk = (tPipe_chunk_funk_enable*)*pChunk;

    return AdjustFunkEnable(chunk->field_0x0, chunk->field_0x4 == 0);
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

// FUNCTION: CARMA2_HW 0x004ca340
void C2_HOOK_FASTCALL FUN_004ca340(tPipe_chunk** pChunk) {
    tPipe_chunk* chunk = *pChunk;

    if (*(tS16*)((tU8*)chunk + 4) != 0) {
        MakeModelMaterialsSingleSided((br_model*)*(void**)chunk);
        return;
    }
    MakeModelMaterialsDoubleSided((br_model*)*(void**)chunk);
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

    return AdjustPedDiagnostics(*(void**)chunk, chunk + 4, chunk + 0x58, chunk + 0x144);
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

    return RecordReplaySplash((void*)chunk->field_0x0, (tU8)chunk->field_0x4, chunk->field_0x8, (const tU32*)&chunk->field_0xc, (const tU32*)&chunk->field_0x18);
}

// FUNCTION: CARMA2_HW 0x004c9ee0
void C2_HOOK_FASTCALL UndoExtendedSplash(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    SetPipeUndoFlag();
    if (pPrev_chunk != NULL) {
        tPipe_chunk_extended_splash* chunk = (tPipe_chunk_extended_splash*)pPrev_chunk;

        RecordReplaySplash((void*)chunk->field_0x0, (tU8)chunk->field_0x4, chunk->field_0x8, (const tU32*)&chunk->field_0xc, (const tU32*)&chunk->field_0x18);
    } else {
        ClearSplashReplay((void*)((tPipe_chunk_extended_splash*)*pChunk)->field_0x0);
    }
    ClearPipeUndoFlag();
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
    int i;
    int j;

    for (i = 0; i < CARPOCALYPSE2_ASIZE(gNapalm_bolts); i++) {
        gNapalm_bolts[i].ped = NULL;

        for (j = 0; j < 7; j++) {
            gNapalm_bolts[i].actors[j]->render_style = BR_RSTYLE_NONE;
        }
    }
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

    return ARDoPowerupRespawn((br_actor*)chunk->field_0x0, chunk->field_0x4);
}

// FUNCTION: CARMA2_HW 0x004ca3b0
void C2_HOOK_FASTCALL UndoPowerupRespawn(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_powerup* chunk = (tPipe_chunk_powerup*)*pChunk;

    return ARUndoPowerupRespawn((br_actor*)chunk->field_0x0, chunk->field_0x4);
}

// FUNCTION: CARMA2_HW 0x004c9d50
void C2_HOOK_FASTCALL ApplyShitMine(tPipe_chunk** pChunk) {
    tPipe_chunk_shit_mine* chunk = (tPipe_chunk_shit_mine*)*pChunk;

    return ARDoShitMine((br_actor*)chunk->field_0x0, &chunk->field_0x4);
}

// FUNCTION: CARMA2_HW 0x004ca3c0
void C2_HOOK_FASTCALL UndoShitMine(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_shit_mine* chunk = (tPipe_chunk_shit_mine*)*pChunk;

    return ARUndoShitMine((br_actor*)chunk->field_0x0, &chunk->field_0x4);
}

// FUNCTION: CARMA2_HW 0x004c9d60
void C2_HOOK_FASTCALL ApplyEndShitMine(tPipe_chunk** pChunk) {
    tPipe_chunk_shit_mine* chunk = (tPipe_chunk_shit_mine*)*pChunk;

    return ARUndoShitMine((br_actor*)chunk->field_0x0, &chunk->field_0x4);
}

// FUNCTION: CARMA2_HW 0x004ca3d0
void C2_HOOK_FASTCALL UndoEndShitMine(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_shit_mine* chunk = (tPipe_chunk_shit_mine*)*pChunk;

    return ARDoShitMine((br_actor*)chunk->field_0x0, &chunk->field_0x4);
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

// FUNCTION: CARMA2_HW 0x004c9dc0
void C2_HOOK_FASTCALL ApplyReplayFrameTime(tPipe_chunk** pChunk) {
    return ApplyTime(((tPipe_chunk_frame_bound*)*pChunk)->field_0x4);
}
// FUNCTION: CARMA2_HW 0x004e6280
void C2_HOOK_FASTCALL ActualActionReplayHeadups(int pSpecial_zappy_bastard) {
    tU16 colour1;
    tU16 colour2;
    tU16 colour3;
    tU16 colour4;
    tU32 time;
    int elapsed;
    int total;
    int pos;
    int i;
    int showed_camera;
    int x;
    int text_y;

    colour3 = 4;
    colour2 = 2;
    colour4 = 0x52;
    colour1 = 0x51;
    if (gBack_screen->type != 3) {
        colour3 = PaletteEntry16Bit(gRender_palette, 4);
        colour2 = PaletteEntry16Bit(gRender_palette, 2);
        colour4 = PaletteEntry16Bit(gRender_palette, 0x52);
        colour1 = PaletteEntry16Bit(gRender_palette, 0x51);
    }
    time = PDGetTotalTime();
    if (PDKeyDown(0)) {
        return;
    }
    PossibleLock(1);
    DRPixelmapBlit(gCurrent_graf_data->field_0x300, gCurrent_graf_data->field_0x304,
        gReplay_headup_graphics.field_0x04);
    if (GetNextReplayRecordState()) {
        DRPixelmapBlit(gCurrent_graf_data->field_0x300 + gCurrent_graf_data->field_0x358,
            gCurrent_graf_data->field_0x304 + gCurrent_graf_data->field_0x35c,
            gReplay_headup_graphics.field_0x28);
    }
    if (pSpecial_zappy_bastard < 0) {
        DRPixelmapBlit(gCurrent_graf_data->field_0x310 + gCurrent_graf_data->field_0x300,
            gCurrent_graf_data->field_0x314 + gCurrent_graf_data->field_0x304,
            gReplay_headup_graphics.field_0x08);
    } else if (pSpecial_zappy_bastard > 0) {
        DRPixelmapBlit(gCurrent_graf_data->field_0x300 + gCurrent_graf_data->field_0x340,
            gCurrent_graf_data->field_0x304 + gCurrent_graf_data->field_0x344,
            gReplay_headup_graphics.field_0x20);
    } else if (ARGetReplayRate() < -1.0f) {
        DRPixelmapBlit(gCurrent_graf_data->field_0x318 + gCurrent_graf_data->field_0x300,
            gCurrent_graf_data->field_0x31c + gCurrent_graf_data->field_0x304,
            gReplay_headup_graphics.field_0x0c);
    } else if (ARGetReplayRate() > 1.0f) {
        DRPixelmapBlit(gCurrent_graf_data->field_0x300 + gCurrent_graf_data->field_0x338,
            gCurrent_graf_data->field_0x304 + gCurrent_graf_data->field_0x33c,
            gReplay_headup_graphics.field_0x1c);
    } else if (ARGetReplayRate() == 1.0f) {
        DRPixelmapBlit(gCurrent_graf_data->field_0x300 + gCurrent_graf_data->field_0x330,
            gCurrent_graf_data->field_0x304 + gCurrent_graf_data->field_0x334,
            gReplay_headup_graphics.field_0x18);
    } else if (ARGetReplayRate() == -1.0f) {
        DRPixelmapBlit(gCurrent_graf_data->field_0x300 + gCurrent_graf_data->field_0x320,
            gCurrent_graf_data->field_0x304 + gCurrent_graf_data->field_0x324,
            gReplay_headup_graphics.field_0x10);
    } else {
        DRPixelmapBlit(gCurrent_graf_data->field_0x300 + gCurrent_graf_data->field_0x328,
            gCurrent_graf_data->field_0x304 + gCurrent_graf_data->field_0x32c,
            gReplay_headup_graphics.field_0x14);
    }
    elapsed = gLast_replay_frame_time - gUnk_006a2360;
    total = gUnk_006a2364 - gUnk_006a2360;
    pos = (int)((float)elapsed / (float)total
        * (gReplay_timeline_x2[gReal_graf_data_index] - gReplay_timeline_x1[gReal_graf_data_index]))
        + gReplay_timeline_x1[gReal_graf_data_index];
    if (pos > gReplay_timeline_x1[gReal_graf_data_index]) {
        BrPixelmapLine(gBack_screen, gReplay_timeline_x1[gReal_graf_data_index],
            gReplay_timeline_y[gReal_graf_data_index], pos - 1,
            gReplay_timeline_y[gReal_graf_data_index], colour2);
        for (i = 1; i < 6; i++) {
            BrPixelmapLine(gBack_screen, gReplay_timeline_x1[gReal_graf_data_index],
                gReplay_timeline_y[gReal_graf_data_index] + i, pos - 1,
                gReplay_timeline_y[gReal_graf_data_index] + i, colour3);
        }
        BrPixelmapLine(gBack_screen, gReplay_timeline_x1[gReal_graf_data_index],
            gReplay_timeline_y[gReal_graf_data_index] + 6, pos - 1,
            gReplay_timeline_y[gReal_graf_data_index] + 6, colour2);
    }
    if (pos < gReplay_timeline_x2[gReal_graf_data_index]) {
        BrPixelmapLine(gBack_screen, pos, gReplay_timeline_y[gReal_graf_data_index],
            gReplay_timeline_x2[gReal_graf_data_index] - 1,
            gReplay_timeline_y[gReal_graf_data_index], colour1);
        for (i = 1; i < 6; i++) {
            BrPixelmapLine(gBack_screen, pos, gReplay_timeline_y[gReal_graf_data_index] + i,
                gReplay_timeline_x2[gReal_graf_data_index] - 1,
                gReplay_timeline_y[gReal_graf_data_index] + i, colour4);
        }
        BrPixelmapLine(gBack_screen, pos, gReplay_timeline_y[gReal_graf_data_index] + 6,
            gReplay_timeline_x2[gReal_graf_data_index] - 1,
            gReplay_timeline_y[gReal_graf_data_index] + 6, colour1);
    }
    BrPixelmapLine(gBack_screen, gReplay_timeline_x1[gReal_graf_data_index] - 1,
        gReplay_timeline_y[gReal_graf_data_index],
        gReplay_timeline_x1[gReal_graf_data_index] - 1,
        gReplay_timeline_y[gReal_graf_data_index] + 6, colour2);
    BrPixelmapLine(gBack_screen, gReplay_timeline_x2[gReal_graf_data_index],
        gReplay_timeline_y[gReal_graf_data_index],
        gReplay_timeline_x2[gReal_graf_data_index],
        gReplay_timeline_y[gReal_graf_data_index] + 6, colour1);
    if (gReplay_show_controls) {
        DRPixelmapBlit(gCurrent_graf_data->field_0x300 + gCurrent_graf_data->field_0x348,
            gCurrent_graf_data->field_0x304 + gCurrent_graf_data->field_0x34c,
            gReplay_headup_graphics.field_0x24);
    }
    PossibleUnlock(1);
    showed_camera = 0;
    if (time - gUnk_006a23c8 < 0x7d0) {
        showed_camera = 1;
        DRPixelmapText(gBack_screen, gCurrent_graf_data->field_0x350,
            gCurrent_graf_data->field_0x354, &gFonts[1],
            GetMiscString(eMiscString_standard_cam + gAction_replay_camera_mode),
            gCurrent_graf_data->field_0x350 * 2);
    }
    if (GetNextReplayRecordState()) {
        if (gShow_status_message) {
            if (gNext_replay_record_state == 4) {
                x = gCurrent_graf_data->field_0x350;
                text_y = gCurrent_graf_data->field_0x354;
                if (!showed_camera) {
                    DRPixelmapText(gBack_screen, x, text_y, &gFonts[1],
                        GetMiscString(gStatus_message_1), gBack_screen->width);
                }
            } else {
                x = gCurrent_graf_data->width / 2;
                text_y = gCurrent_graf_data->height / 2;
                DRPixelmapCentredText(gBack_screen, x, text_y - 0x40, &gFonts[4],
                    GetMiscString(gStatus_message_1));
                if (gStatus_message_2) {
                    DRPixelmapCentredText(gBack_screen, x, text_y - 0x20, &gFonts[6],
                        GetMiscString(gStatus_message_2));
                }
            }
        }
    } else if (((time / 400) & 1) != 0) {
        PossibleLock(1);
        DRPixelmapBlit(gCurrent_graf_data->field_0x2f8,
            gCurrent_graf_data->field_0x2fc,
            gReplay_headup_graphics.field_0x00);
        PossibleUnlock(1);
    }
    if (!gRecording_mouse_disabled && !GetNextReplayRecordState()) {
        PossibleLock(1);
        DoMouseCursor();
        PossibleUnlock(1);
    }
}

// Replay pipe worker helpers live in replay_workers.c so they are not inlined
// into the wrappers above; prototypes are at the top of this file.
