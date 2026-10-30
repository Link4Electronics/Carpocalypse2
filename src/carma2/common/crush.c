#include "crush.h"
#include "crush1.h"

#include "animation.h"
#include "car.h"
#include "compress.h"
#include "controls.h"
#include "crush2.h"
#include "depth.h"

#include "drone.h"

#include "displays.h"
#include "errors.h"

#include "explosions.h"

#include "finteray.h"
#include "flap.h"
#include "funks.h"

#include "graphics1.h"

#include "globvars.h"
#include "globvrkm.h"
#include "globvrpb.h"
#include "loading.h"
#include "network.h"
#include "oil.h"
#include "opponent.h"
#include "piping.h"
#include "phil.h"
#include "platform.h"
#include "powerups.h"
#include "pratcam.h"
#include "physics.h"

#include "raycast.h"

#include "replay.h"
#include "shrapnel.h"
#include "skidmark.h"
#include "smash.h"
#include "smashing.h"
#include "sound.h"
#include "spark.h"

#include "tinted.h"

#include "structur.h"
#include "utility.h"
#include "world.h"

#include <brender/brender.h>

#include "carpocalypse2_macros.h"

void C2_HOOK_FASTCALL DeleteFarCrushes(tCar_spec* pCar);

extern br_scalar gFLOAT_00589a5c;
extern const br_scalar gFLOAT_00589a54;
extern const double gDOUBLE_00589a60;
extern const double gDOUBLE_0058af40;
extern const br_scalar gFLOAT_00589a28;
extern const br_scalar gFLOAT_00589a2c;
extern const double gDOUBLE_00589a38;
extern const double gDOUBLE_00589af8;
extern const double gDOUBLE_00589b00;
extern const br_scalar gFLOAT_00589b08;
extern const br_scalar gFLOAT_00589b0c;
extern const br_scalar gFLOAT_00589b10;
extern float gUNK_00589a44;
extern tU16 gCrush_vertex_compression_index;
extern int gMin_split_damage;
extern int gMax_split_damage;
extern br_actor* gSmashable_noncar_shrapnel_actors[100];
extern intptr_t C2_HOOK_CDECL MakeCarModelsMaterialsDoubleSided(br_actor* pActor, void* pArg);
int C2_HOOK_FAKE_THISCALL FUN_004358a0(br_actor* pActor, int pArg2, tCar_spec** ppCar);
int C2_HOOK_CDECL PredicateSpamPanel(br_actor* pActor, tCar_spec* pCar);

// GLOBAL: CARMA2_HW 0x006796bc
int gCount_queued_drone_crushes;

// GLOBAL: CARMA2_HW 0x0067bad8
tQueued_drone_crush gQueued_drone_crushes[4];

// GLOBAL: CARMA2_HW 0x0067baa0
int gCrush_panel_ids[10];

/* Not ported yet: minimal stubs so callers keep their direct call targets. */
#pragma auto_inline(off)
// GLOBAL: CARMA2_HW 0x00589ae4
const br_scalar gFloat_00589ae4 = 0.4f;

// GLOBAL: CARMA2_HW 0x00589ae8
const br_scalar gFloat_00589ae8 = 0.15f;

// GLOBAL: CARMA2_HW 0x00589a68
const br_scalar gCrushReattachDistanceSquared = 0.000210039914236404f;

// GLOBAL: CARMA2_HW 0x00589b48
const br_scalar gCrushRepairStepFactor = 0.00005f;

// GLOBAL: CARMA2_HW 0x00589b58
const br_scalar gFloat_00589b58 = 0.2f;

// GLOBAL: CARMA2_HW 0x00589b5c
const br_scalar gFloat_00589b5c = 0.015f;

// GLOBAL: CARMA2_HW 0x00589b60
const br_scalar gFloat_00589b60 = 0.001f;

// GLOBAL: CARMA2_HW 0x00589af0
const double gCrushShapeOutsideTolerance = -1e-5;

// GLOBAL: CARMA2_HW 0x00589a48
float gUNK_00589a48 = -0.028985507786273956f;

// C2PORT-NONMATCHING: best 27.76% | difftest PASS 2000 trials (spec difftest/specs/00435110_FUN_00435110.json) | reason x87-fp-codegen
// FUNCTION: CARMA2_HW 0x00435110
void C2_HOOK_FASTCALL FUN_00435110(int* param_1, int param_2) {
    float* pfVar1;
    float* pfVar2;
    volatile float fVar3;
    volatile float fVar4;
    volatile float fVar5;
    volatile float fVar6;
    float fVar8;
    int iVar7;
    int iVar10;
    int iVar12;
    int iVar13;
    int iVar15;
    int bVar9;
    tU8* pbVar14;
    float* pfVar11;
    float* pfVar16;
    int local_74;
    float local_5c_buf[25];
    float* local_5c = local_5c_buf + 1;

    iVar13 = param_1[4];
    if (param_1[3] != iVar13) {
        iVar10 = param_1[0];
        iVar15 = 0;
        fVar6 = *(float*)(*(int*)(param_2 + 0x34) + 0x20);
        local_74 = 0;
        iVar12 = *(int*)(iVar10 + 0x3c);
        if (0 < iVar12) {
            iVar7 = *(int*)(iVar10 + 0x44);
            pbVar14 = *(tU8**)(iVar10 + 0x48);
            pfVar11 = local_5c;
            do {
                pfVar1 = (float*)(iVar7 + (tU32)*pbVar14 * 0xc);
                pfVar2 = (float*)(iVar7 + (tU32)pbVar14[1] * 0xc);
                if (fVar6 <= *(float*)(iVar7 + 8 + (tU32)pbVar14[1] * 0xc)) {
                    if (fVar6 <= pfVar1[2]) {
                        goto LAB_004351ac;
                    }
                    bVar9 = 1;
                } else if (pfVar1[2] <= fVar6) {
LAB_004351ac:
                    bVar9 = 0;
                } else {
                    bVar9 = 1;
                }
                pfVar16 = pfVar11;
                if (bVar9 && gUNK_00589a44 <= (pfVar1[2] - fVar6 < 0 ? fVar6 - pfVar1[2] : pfVar1[2] - fVar6) &&
                    gUNK_00589a44 <= (pfVar2[2] - fVar6 < 0 ? fVar6 - pfVar2[2] : pfVar2[2] - fVar6)) {
                    fVar8 = (fVar6 - pfVar1[2]) / (pfVar1[2] - pfVar2[2]);
                    iVar15++;
                    pfVar16 = pfVar11 + 3;
                    fVar3 = pfVar1[1];
                    fVar4 = pfVar2[1];
                    fVar5 = pfVar1[1];
                    pfVar11[-1] = *pfVar1 + (*pfVar1 - *pfVar2) * fVar8;
                    pfVar11[0] = fVar5 + (fVar3 - fVar4) * fVar8;
                    pfVar11[1] = fVar6;
                    if (iVar15 == 8) {
                        break;
                    }
                }
                local_74++;
                pbVar14 += 2;
                pfVar11 = pfVar16;
            } while (local_74 < iVar12);
        }
        if (iVar13 < param_1[3] + iVar15) {
            iVar15 = iVar13 - param_1[3];
        }
        if (0 < iVar15) {
            iVar10 = param_1[3] * 0x38;
            pfVar11 = local_5c;
            iVar13 = iVar15;
            do {
                *(float*)(iVar10 + 0x18 + param_1[6]) = pfVar11[-1];
                *(float*)(iVar10 + 0x1c + param_1[6]) = pfVar11[0];
                *(float*)(iVar10 + 0x20 + param_1[6]) = pfVar11[1];
                iVar10 += 0x38;
                iVar13--;
                pfVar11 += 3;
            } while (iVar13 != 0);
        }
        iVar13 = 0;
        param_1[5] = param_1[3];
        iVar15 = param_1[3] + iVar15;
        param_1[3] = iVar15;
        if (0 < iVar15) {
            iVar10 = 0;
            do {
                *(int*)(param_1[6] + iVar10 + 0x2c) = *(int*)(param_1[6] + iVar10 + 0x18);
                *(int*)(param_1[6] + iVar10 + 0x30) = *(int*)(param_1[6] + iVar10 + 0x1c);
                iVar12 = param_1[6] + iVar10;
                iVar13++;
                iVar10 += 0x38;
                *(int*)(iVar12 + 0x34) = *(int*)(iVar12 + 0x20);
            } while (iVar13 < param_1[3]);
        }
        iVar13 = 0;
        if (0 < param_1[3]) {
            do {
                FUN_00434f60((br_vector3*)((tU8*)param_1[6] + iVar13 * 0x38 + 0x18), (void*)param_2);
                iVar13++;
            } while (iVar13 < param_1[3]);
        }
    }
}

// C2PORT-NONMATCHING: best 55.73% | difftest PASS 2000 trials (spec difftest/specs/00435310_FUN_00435310.json) | reason VC5 x87 scheduling, register allocation, and stack layout
// FUNCTION: CARMA2_HW 0x00435310
void C2_HOOK_FASTCALL FUN_00435310(tCar_spec* pCar, void* pTransform_context) {
    tGroovidelic_spec* groove;
    br_actor* wheel_actor;
    br_vector3* centre;
    br_vector3 delta;
    br_vector3 vector;
    float fVar2;
    float fVar3;
    float fVar4;
    float angle;
    int i;

    for (i = 0; i < 6; i++) {
        wheel_actor = pCar->pivot_actors[i];
        if (wheel_actor == NULL) {
            wheel_actor = pCar->wheel_actors[i];
        }
        if (wheel_actor != NULL) {
            groove = ActorsGroove(wheel_actor);
            if (groove != NULL && groove->path_type == eGroove_path_straight) {
                centre = &groove->path_data.straight_info.centre;
                pCar->car_crush_spec->field_0x1ec[i].v[0] = centre->v[0];
                pCar->car_crush_spec->field_0x1ec[i].v[1] = centre->v[1];
                pCar->car_crush_spec->field_0x1ec[i].v[2] = centre->v[2];
                delta.v[0] = -centre->v[0];
                delta.v[1] = -centre->v[1];
                delta.v[2] = -centre->v[2];
                FUN_00434f60(centre, pTransform_context);
                centre->v[1] = pCar->car_crush_spec->field_0x1ec[i].v[1];
                delta.v[0] += centre->v[0];
                delta.v[1] += centre->v[1];
                delta.v[2] += centre->v[2];
                PipeSingleVector3(centre, &delta);
            }
        }
    }

    for (i = 0; i < 4; i++) {
        fVar2 = pCar->wpos[i].v[0];
        fVar3 = pCar->wpos[i].v[1];
        fVar4 = pCar->wpos[i].v[2];
        pCar->car_crush_spec->field_0x234[i].v[0] = fVar2;
        pCar->car_crush_spec->field_0x234[i].v[1] = fVar3;
        pCar->car_crush_spec->field_0x234[i].v[2] = fVar4;
        vector.v[0] = (br_scalar)(fVar2 * gDOUBLE_00589a38);
        vector.v[1] = (br_scalar)(fVar3 * gDOUBLE_00589a38);
        vector.v[2] = (br_scalar)(fVar4 * gDOUBLE_00589a38);
        FUN_00434f60(&vector, pTransform_context);
        pCar->wpos[i].v[0] = (br_scalar)(vector.v[0] * gDOUBLE_00589af8);
        pCar->wpos[i].v[1] = (br_scalar)(vector.v[1] * gDOUBLE_00589af8);
        pCar->wpos[i].v[2] = (br_scalar)(vector.v[2] * gDOUBLE_00589af8);
    }

    SetCarSuspGiveAndHeight(pCar, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f);
    if (*(float*)(*(int*)((tU8*)pTransform_context + 0x34) + 0x20) == pCar->car_crush_spec->bend_z_min) {
        angle = (float)*(tU16*)((tU8*)*(int*)((tU8*)pTransform_context + 0x34) + 0x3e) * (float)gDOUBLE_00589b00;
        if (!(angle < gFLOAT_00589b08)) {
            angle -= gFLOAT_00589b0c;
        }
        pCar->field_0x1260 = angle * gFLOAT_00589b10;
    }
    CalcGraphicalWheelStuff(pCar);
}

// C2PORT-NONMATCHING: best 51.04% | difftest PASS 2000 trials (spec difftest/specs/00435550_FUN_00435550.json) | reason x87-order-stack-layout
// FUNCTION: CARMA2_HW 0x00435550
void C2_HOOK_FASTCALL FUN_00435550(tCar_spec* pCar, br_actor* pActor, br_angle pAngle_x, br_angle pAngle_y, br_angle pAngle_z, const br_vector3* pArg6, float pArg7, float* pArg8) {
    struct {
        tCar_spec* car;
        br_matrix34 matrix;
        float* bounds;
    } context;
    br_vector3 transformed;
    float* pBounds;

    BrMatrix34Translate(&context.matrix, -pArg6->v[0], -pArg6->v[1], -pArg6->v[2]);
    if (pAngle_y != 0) {
        BrMatrix34PostRotateY(&context.matrix, pAngle_y);
    }
    if (pAngle_x != 0) {
        BrMatrix34PostRotateX(&context.matrix, pAngle_x);
    }
    if (pAngle_z != 0) {
        BrMatrix34PostRotateZ(&context.matrix, pAngle_z);
    }
    BrMatrix34PostTranslate(&context.matrix, pArg6->v[0], pArg6->v[1], pArg6->v[2]);

    BrActorToBounds((br_bounds*)pArg8, pActor);
    pBounds = pArg8;
    pBounds[0x11] = pBounds[3];
    pBounds[0x12] = pBounds[4];
    pBounds[0x13] = pArg6->v[2];
    pBounds[0x14] = pBounds[3];
    pBounds[0x15] = pBounds[1];
    pBounds[0x16] = pArg6->v[2];
    pBounds[0x17] = pBounds[0];
    pBounds[0x18] = pBounds[4];
    pBounds[0x19] = pArg6->v[2];
    pBounds[0x1a] = pBounds[0];
    pBounds[0x1b] = pBounds[1];
    pBounds[0x1c] = pArg6->v[2];
    pBounds[9] = pBounds[0];
    pBounds[10] = pBounds[1];
    pBounds[0xb] = pArg7;
    pBounds[0xc] = pBounds[0x11] - pBounds[0];
    pBounds[0xd] = pBounds[0x12] - pBounds[1];
    pBounds[0xe] = pBounds[0x13] - pArg7;

    BrMatrix34ApplyP(&transformed, (br_vector3*)&pBounds[0x11], &context.matrix);
    pBounds[0x11] = transformed.v[0] - pBounds[0x11];
    pBounds[0x12] = transformed.v[1] - pBounds[0x12];
    pBounds[0x13] = transformed.v[2] - pBounds[0x13];
    BrMatrix34ApplyP(&transformed, (br_vector3*)&pBounds[0x14], &context.matrix);
    pBounds[0x14] = transformed.v[0] - pBounds[0x14];
    pBounds[0x15] = transformed.v[1] - pBounds[0x15];
    pBounds[0x16] = transformed.v[2] - pBounds[0x16];
    BrMatrix34ApplyP(&transformed, (br_vector3*)&pBounds[0x17], &context.matrix);
    pBounds[0x17] = transformed.v[0] - pBounds[0x17];
    pBounds[0x18] = transformed.v[1] - pBounds[0x18];
    pBounds[0x19] = transformed.v[2] - pBounds[0x19];
    BrMatrix34ApplyP(&transformed, (br_vector3*)&pBounds[0x1a], &context.matrix);
    pBounds[0x1a] = transformed.v[0] - pBounds[0x1a];
    pBounds[0x1b] = transformed.v[1] - pBounds[0x1b];
    pBounds[0x1c] = transformed.v[2] - pBounds[0x1c];

    pBounds[0x11] = ((pBounds[0x11] - pBounds[0x14]) - pBounds[0x17]) + pBounds[0x1a];
    pBounds[0x12] = ((pBounds[0x12] - pBounds[0x15]) - pBounds[0x18]) + pBounds[0x1b];
    pBounds[0x13] = ((pBounds[0x13] - pBounds[0x16]) - pBounds[0x19]) + pBounds[0x1c];
    pBounds[0x14] -= pBounds[0x1a];
    pBounds[0x15] -= pBounds[0x1b];
    pBounds[0x16] -= pBounds[0x1c];
    pBounds[0x17] -= pBounds[0x1a];
    pBounds[0x18] -= pBounds[0x1b];
    pBounds[0x19] -= pBounds[0x1c];
    *(int*)&pBounds[0x1d] = pArg6->v[2] < pArg7;
    pBounds[6] = pArg6->v[0];
    pBounds[7] = pArg6->v[1];
    pBounds[8] = pArg6->v[2];

    gCrush_vertex_compression_index = 0;
    context.car = pCar;
    context.bounds = pBounds;
    ARStartPipingSession(ePipe_model_mash);
    DRActorEnumRecurseWithTranslation(pActor, NULL, (tDRActorEnumRecurseWithTranslation_cbfn*)FUN_004358a0, &context.car);
    AREndPipingSession();
    *(br_angle*)((tU8*)pBounds + 0x3c) = pAngle_x;
    *(br_angle*)((tU8*)pBounds + 0x3e) = pAngle_y;
    *(br_angle*)((tU8*)pBounds + 0x40) = pAngle_z;
}

// GLOBAL: CARMA2_HW 0x00589acc
const br_scalar gFLOAT_00589acc = 0.0014492754125967622f;

// GLOBAL: CARMA2_HW 0x00589ac0
const br_scalar gFLOAT_00589ac0 = 1.1594202518463135f;

extern const br_scalar gFLOAT_00589a54;
extern const br_scalar gFLOAT_00589a7c;
extern const double gDOUBLE_00589a38;
extern const double gDOUBLE_00589af8;
extern const double gDOUBLE_00589b00;
extern const br_scalar gFLOAT_00589b08;
extern const br_scalar gFLOAT_00589b0c;
extern const br_scalar gFLOAT_00589b10;

// GLOBAL: CARMA2_HW 0x00589a80
const br_scalar gFloat_00589a80 = 10.0f;

// GLOBAL: CARMA2_HW 0x00589a88
const double gDOUBLE_00589a88 = 0.005;

extern const double gDOUBLE_00589a60;

// GLOBAL: CARMA2_HW 0x00589ad0
const double gDOUBLE_00589ad0 = 10430.378350470453;

// GLOBAL: CARMA2_HW 0x00589ad8
const br_scalar gFLOAT_00589ad8 = 0.2f;

// GLOBAL: CARMA2_HW 0x00589b40
const double gDouble_00589b40 = 0.0000152587890625;

// GLOBAL: CARMA2_HW 0x00589b64
const br_scalar gFloat_00589b64 = 0.0f;

// GLOBAL: CARMA2_HW 0x00589b68
const double gDouble_00589b68 = 1.4285714285714286;

// GLOBAL: CARMA2_HW 0x00589b70
const br_scalar gFloat_00589b70 = 0.5f;

// GLOBAL: CARMA2_HW 0x00589b74
const br_scalar gFloat_00589b74 = 15.0f;

// GLOBAL: CARMA2_HW 0x00589b78
const br_scalar gFloat_00589b78 = 1500.0f;

// GLOBAL: CARMA2_HW 0x00589b7c
const br_scalar gFloat_00589b7c = 30.0f;

// GLOBAL: CARMA2_HW 0x00589b80
const double gDouble_00589b80 = 1.0;

// GLOBAL: CARMA2_HW 0x00589b88
const double gDouble_00589b88 = 99.0;

// GLOBAL: CARMA2_HW 0x00589b90
const double gDouble_00589b90 = -0.05;

// GLOBAL: CARMA2_HW 0x00589b98
const br_scalar gFloat_00589b98 = 10.0f;

// GLOBAL: CARMA2_HW 0x00589b9c
const br_scalar gFloat_00589b9c = 7.0f;

// GLOBAL: CARMA2_HW 0x00589ba0
const br_scalar gFloat_00589ba0 = 4.0f;

// GLOBAL: CARMA2_HW 0x00589ba4
const br_scalar gFloat_00589ba4 = 20.0f;

// GLOBAL: CARMA2_HW 0x00589ba8
const br_scalar gFloat_00589ba8 = 5.0f;

// GLOBAL: CARMA2_HW 0x00589bac
const br_scalar gFloat_00589bac = 100.0f;

// GLOBAL: CARMA2_HW 0x00589bb0
const br_scalar gFloat_00589bb0 = 50.0f;

// GLOBAL: CARMA2_HW 0x00589bb4
const br_scalar gFloat_00589bb4 = 1.0f;

// GLOBAL: CARMA2_HW 0x00589bb8
const br_scalar gFloat_00589bb8 = 60.0f;

// GLOBAL: CARMA2_HW 0x00589bbc
const br_scalar gFloat_00589bbc = 25.0f;

// GLOBAL: CARMA2_HW 0x00589bc0
const br_scalar gFloat_00589bc0 = 150.0f;

// GLOBAL: CARMA2_HW 0x00589bc4
const br_scalar gFloat_00589bc4 = -50.0f;

// GLOBAL: CARMA2_HW 0x0058afb4
const br_scalar gFloat_0058afb4 = -10.0f;

// GLOBAL: CARMA2_HW 0x0058af88
const double gDOUBLE_0058af88 = 0.5;

// GLOBAL: CARMA2_HW 0x0058af90
const double gDOUBLE_0058af90 = 2.384185791015625e-7;

// GLOBAL: CARMA2_HW 0x0058af98
const double gDOUBLE_0058af98 = -0.0001;

// GLOBAL: CARMA2_HW 0x0058afa0
const double gDOUBLE_0058afa0 = 1.0001;

// GLOBAL: CARMA2_HW 0x0058f860
tU16 gNext_crush_actor_id = 10000;

// GLOBAL: CARMA2_HW 0x0067b83c
tU16 gCrush_vertex_compression_index;

// GLOBAL: CARMA2_HW 0x00589a44
float gUNK_00589a44 = 0.028985507786273956f;

// GLOBAL: CARMA2_HW 0x006b9c60
int gUnknown_006b9c60[100];

// GLOBAL: CARMA2_HW 0x0058afb0
float gFloat_0058afb0 = 0.25f;

// GLOBAL: CARMA2_HW 0x006a8290
int gINT_006a8290;

// GLOBAL: CARMA2_HW 0x0058af20
const br_scalar gFLOAT_0058af20 = -1.0f;

// GLOBAL: CARMA2_HW 0x0058aef8
const double gDOUBLE_0058aef8 = 0.1;

// GLOBAL: CARMA2_HW 0x0058afa8
const double gDOUBLE_0058afa8 = 0.01;

extern int gCapacity_track_smashables;
extern void C2_HOOK_CDECL BrMatrix23ApplyP(br_vector2* A, const br_vector2* B, const br_matrix23* C);
extern const double gDouble_0058aee8;

// GLOBAL: CARMA2_HW 0x00589a90
const br_scalar gFLOAT_00589a90 = 0.014492753893136978f;

// GLOBAL: CARMA2_HW 0x00589a94
const br_scalar gFLOAT_00589a94 = -0.014492753893136978f;

// GLOBAL: CARMA2_HW 0x00679698
extern br_scalar gDistortion_factor;

// GLOBAL: CARMA2_HW 0x006796b8
extern br_scalar gMin_crush_force;

// GLOBAL: CARMA2_HW 0x0067bdec
extern br_scalar gMax_crush_force;

// GLOBAL: CARMA2_HW 0x006796b0
extern br_scalar gForce_to_movement_factor;

// GLOBAL: CARMA2_HW 0x0067bacc
extern br_scalar gMax_crush_dist_sq;

// GLOBAL: CARMA2_HW 0x00589a40
br_scalar gCrushZero = 0.0f;

// GLOBAL: CARMA2_HW 0x00589adc
br_scalar gCrushEps = 9.999999747378752e-06f;

// GLOBAL: CARMA2_HW 0x00589a74
br_scalar gCrushSqrtHalf = 0.7071070075035095f;

// GLOBAL: CARMA2_HW 0x0067bac8
extern br_scalar gMin_force_to_split_XZ_per_tonne;

// GLOBAL: CARMA2_HW 0x0067bd5c
extern br_scalar gMin_force_to_split_Y_per_tonne;

// GLOBAL: CARMA2_HW 0x0067bd64
extern int gMin_split_damage;

// GLOBAL: CARMA2_HW 0x0067b7d0
extern int gMax_split_damage;

// GLOBAL: CARMA2_HW 0x0067be04
extern br_scalar gChance_of_inverse_buckle;

// GLOBAL: CARMA2_HW 0x0067b7b4
extern br_scalar gFlap_inertia_fudge_biscuit;

// GLOBAL: CARMA2_HW 0x0067bdf8
extern br_scalar gTorque_to_snap_per_tonne;

// GLOBAL: CARMA2_HW 0x0067944c
extern br_scalar gNormal_force_to_detach;

// GLOBAL: CARMA2_HW 0x0067be00
extern br_scalar gChance_of_bending;

// GLOBAL: CARMA2_HW 0x0067a18c
extern br_scalar gMin_bend_angle;

// GLOBAL: CARMA2_HW 0x0067b7b0
extern int gMin_bend_damage;

// GLOBAL: CARMA2_HW 0x0067bdf4
extern int gMax_bend_damage;

// GLOBAL: CARMA2_HW 0x0067be08
tU8 gCrush_data_entry_counter;

// GLOBAL: CARMA2_HW 0x0058f808
const char* gSoftness_names[6] = {
    "very soft",
    "soft",
    "normal",
    "hard",
    "very hard",
    "uncrushable",
};

// GLOBAL: CARMA2_HW 0x0058f848
const char* gCrush_type_names[3] = {
    "boring",
    "flap",
    "detach",
};

// GLOBAL: CARMA2_HW 0x0058f820
const char* gEase_of_detachment_names[5] = {
    "very easy",
    "easy",
    "normal",
    "difficult",
    "very difficult",
};

// GLOBAL: CARMA2_HW 0x0058f858
const char* gCar_crush_shape_names[2] = {
    "box",
    "poly",
};

// GLOBAL: CARMA2_HW 0x0058f838
const char* gDetach_type_names[] = {
    "normal",
    "stubborn",
    "fully_detach",
    "joint_index",
};

// GLOBAL: CARMA2_HW 0x0065ff00
const char* gPosition_type_names[2] = {
    "relative",
    "absolute",
};

// GLOBAL: CARMA2_HW 0x0065fea0
const char* gSmashable_collision_type_names[3] = {
    "solid",
    "passthrough",
    "edges",
};

// GLOBAL: CARMA2_HW 0x0067a190
undefined4 gCrush_array_0067a190[10];

// GLOBAL: CARMA2_HW 0x00679440
undefined4 gDAT_00679440;

// GLOBAL: CARMA2_HW 0x006796a0
tCrush_info_buffer gDetached_bit_crush_info_buffer;

// GLOBAL: CARMA2_HW 0x00679448
tPhysics_object* gDetached_bit_collision_infos;

// GLOBAL: CARMA2_HW 0x006796ac
tDriver gDetached_bit_driver;

// GLOBAL: CARMA2_HW 0x0058aec0
const br_scalar gFloat_0058aec0 = 0.0f;

// GLOBAL: CARMA2_HW 0x0067bd68
tPhysics_joint* gTrack_crush_joints[32];

// GLOBAL: CARMA2_HW 0x0067b7b8
tCrush_info_buffer gSplit_car_crush_info_buffer;

// GLOBAL: CARMA2_HW 0x0067a188
tPhysics_object* gSplit_car_collision_infos;

// GLOBAL: CARMA2_HW 0x0067b7c4
tDriver gSplit_car_driver;

// GLOBAL: CARMA2_HW 0x006796b4
tPhysics_shape* gGonad_sphere_collision_shape;

// GLOBAL: CARMA2_HW 0x006796c8
tCrush_list_item gCrush_lists[8];

// GLOBAL: CARMA2_HW 0x006796bc
int gCount_queued_drone_crushes;

// GLOBAL: CARMA2_HW 0x0067bad0
int gCount_crush_detach_list;

// GLOBAL: CARMA2_HW 0x0067be84
tU32 gINT_0067be84;

// GLOBAL: CARMA2_HW 0x0067bdfc
int gINT_0067bdfc;

// GLOBAL: CARMA2_HW 0x0067bdf0
int gCount_net_crush_detach_list;

// GLOBAL: CARMA2_HW 0x0067b838
int gCount_net_crush_semi_detach_bit_list;

// GLOBAL: CARMA2_HW 0x00589c40
const float gFloat_00589c40 = -0.5f;

// GLOBAL: CARMA2_HW 0x0067bd60
int gCount_net_crush_full_detach_bit_list;

// GLOBAL: CARMA2_HW 0x0067b7c8
int gCount_net_crush_reattach_bit_list;

// GLOBAL: CARMA2_HW 0x0067b7cc
int gCount_car_damage_crush_list;

// GLOBAL: CARMA2_HW 0x0067bd58
int gCount_toggled_doors;

// GLOBAL: CARMA2_HW 0x00679450
tToggled_door gToggled_doors[16];

// GLOBAL: CARMA2_HW 0x0067a290
tCrush_net_detach_list_item gNet_crush_detach_list[8];

// GLOBAL: CARMA2_HW 0x0067b840
tCrush_net_semi_detach_bit_list_item gNet_crush_semi_detach_bit_list[8];

// GLOBAL: CARMA2_HW 0x0067b7d8
tCrush_net_full_detach_bit_list_item gNet_crush_full_detach_bit_list[8];

// GLOBAL: CARMA2_HW 0x0067be10
tCrush_net_reattach_bit_list_item gNet_crush_reattach_bit_list[8];

// GLOBAL: CARMA2_HW 0x00679558
tCrush_detach_list_item gCrush_detach_list[16];

// GLOBAL: CARMA2_HW 0x0067bd18
tCar_damge_crush_list_item gCar_damage_crush_list[8];

// GLOBAL: CARMA2_HW 0x0067bad8
tQueued_drone_crush gQueued_drone_crushes[4];

// GLOBAL: CARMA2_HW 0x00590020
float gWobble_spam_y[8] = { 0.0f, -0.15f, 0.4f, 0.15f, -0.4f, 0.25f, 0.0f, -0.25f };

// GLOBAL: CARMA2_HW 0x00590040
float gWobble_spam_z[8] = { 0.4f, -0.25f, 0.0f, 0.25f, 0.0f, 0.15f, -0.4f, -0.15f };

// GLOBAL: CARMA2_HW 0x00590060
float gWheel_circ_to_width = .16f;

// GLOBAL: CARMA2_HW 0x0067baa0
int gCrush_panel_ids[10];

// GLOBAL: CARMA2_HW 0x0068b85c
int gCrush_spam_gate;

// GLOBAL: CARMA2_HW 0x0067c3b4
int gLast_credit_award_time = 0;

// GLOBAL: CARMA2_HW 0x00590068
unsigned char gSmoke_part_table[12] = {
    0x14, 0x14, 0x00,
    0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10
};

// C2PORT-NONMATCHING: best 29.32% | difftest PASS 2000 trials (spec difftest/specs/004ee290_FUN_004ee290.json) | reason VC5 register/stack allocation and branch layout

// C2PORT-NONMATCHING: best 79.85% | difftest PASS 2000 trials (spec difftest/specs/0042d1e0_FUN_0042d1e0.json) | reason x87-order, register-allocation, stack-layout
// FUNCTION: CARMA2_HW 0x0042d1e0
void C2_HOOK_FASTCALL FUN_0042d1e0(tPhysics_object* pObject, int pCount, br_vector3* pArg3, br_vector3* pArg4) {
    float fVar5;
    br_vector3 normalized;
    int iVar8;
    int iVar6;
    float* pfVar10;

    fVar5 = (float)sqrt(pArg4->v[0] * pArg4->v[0] + pArg4->v[2] * pArg4->v[2] + pArg4->v[1] * pArg4->v[1]);
    normalized.v[0] = pArg4->v[0] / fVar5;
    normalized.v[1] = pArg4->v[1] / fVar5;
    normalized.v[2] = pArg4->v[2] / fVar5;
    for (iVar8 = 0; iVar8 < gCount_queued_drone_crushes; iVar8++) {
        if (gQueued_drone_crushes[iVar8].field_0x0 == pObject) {
            for (iVar6 = 0; iVar6 < gQueued_drone_crushes[iVar8].count; iVar6++) {
                pfVar10 = &gQueued_drone_crushes[iVar8].items[iVar6].field_0x38.v[1];
                if (gCrushSqrtHalf <
                    pfVar10[0] * normalized.v[1] + pfVar10[-1] * normalized.v[0] + pfVar10[1] * normalized.v[2]) {
                    gQueued_drone_crushes[iVar8].items[iVar6].field_0x34 += fVar5;
                    return;
                }
            }
            if (gQueued_drone_crushes[iVar8].count < 2) {
                AddDroneCrush(&gQueued_drone_crushes[iVar8].items[gQueued_drone_crushes[iVar8].count], &normalized, fVar5, pCount, pArg3);
                gQueued_drone_crushes[iVar8].count++;
            }
        }
    }
    if (gCount_queued_drone_crushes < 4) {
        gQueued_drone_crushes[gCount_queued_drone_crushes].field_0x0 = pObject;
        gQueued_drone_crushes[gCount_queued_drone_crushes].count = 0;
        AddDroneCrush(&gQueued_drone_crushes[gCount_queued_drone_crushes].items[0], &normalized, fVar5, pCount, pArg3);
        gCount_queued_drone_crushes++;
    }
}

// FUNCTION: CARMA2_HW 0x004ee290
tSmashable_level* C2_HOOK_FAKE_THISCALL FUN_004ee290(tCar_spec* pCar, tPhysics_object* pCollision_info, br_vector3* pPosition, br_vector3* pOptional_position, br_actor* pActor, undefined4 pArg6, tCar_crush_smashable_part* pSmashable, float pArg8, undefined4 pArg9, undefined4 pArg10) {
    int queued_count;
    int level_index;

    int lower_level;
    int queue_index;
    tSmashable_level* result;
    tQueued_smash* queued_smash;
    float* trigger_threshold;

    result = NULL;
    lower_level = pSmashable->field_0x4c;
    level_index = pSmashable->count_smashable_levels - 1;
    if (level_index <= lower_level) {
        goto final_failure;
    }
    queued_count = gCount_queued_smashes;
    if (queued_count >= CARPOCALYPSE2_ASIZE(gQueued_smashes)) {
        goto final_failure;
    }
    if (pActor->model == NULL) {
        goto final_failure;
    }
    if (level_index <= lower_level) {
        goto final_failure;
    }
#ifndef CARPOCALYPSE2_MATCHING
    if (getenv("C2_OBJLOG") != NULL) {
        int log_level;
        dr_dprintf("SMASHINPUT part=%p material=%p actor=%p input=%.7g current=%d count=%d",
            pSmashable, pSmashable->funk_material, pActor, pArg8, lower_level, pSmashable->count_smashable_levels);
        for (log_level = 1; log_level < pSmashable->count_smashable_levels && log_level <= 3; ++log_level) {
            dr_dprintf("SMASHTHRESH part=%p level=%d threshold=%.7g collision=%d", pSmashable, log_level,
                pSmashable->levels[log_level].trigger_threshold, pSmashable->levels[log_level].collision_type);
        }
    }
#endif
    trigger_threshold = (float*)((tU8*)pSmashable->levels + pSmashable->count_smashable_levels * sizeof(tSmashable_level) - 0x2b4);
    while (pArg8 < *trigger_threshold) {
        level_index--;
        trigger_threshold -= 0xb1;
        if (level_index <= lower_level) {
            goto level_failure;
        }
    }
    goto queue_search;
level_failure:
    return NULL;
queue_search:
    queue_index = 0;
    queued_smash = gQueued_smashes;
    if (queued_count > 0) {
        do {
            if (queued_smash->smashable == pSmashable) {
                if (level_index <= queued_smash->field_0x38) {
                    goto final_failure;
                }
                queued_smash->field_0x38 = level_index;
                result = &pSmashable->levels[level_index];
                break;
            }
            queue_index++;
            queued_smash++;
        } while (queue_index < queued_count);
    }
    if (result == NULL) {
        queued_smash = &gQueued_smashes[queued_count];
        queued_smash->car = pCar;
        queued_smash->field_0x4 = (undefined4)(uintptr_t)pCollision_info;
        queued_smash->actor = pActor;
        queued_smash->smashable = pSmashable;
        queued_smash->field_0x48 = pArg6;
        queued_smash->field_0x38 = level_index;
        queued_smash->field_0x2c.v[0] = pPosition->v[0];
        queued_smash->field_0x2c.v[1] = pPosition->v[1];
        queued_smash->field_0x2c.v[2] = pPosition->v[2];
        queued_smash->field_0x44 = pArg9;
        if (gINT_006a3334 != 0) {
            queued_smash->field_0x40 = 1;
            queued_smash->field_0x4c = gMatrix34_006b78a0;
            queued_smash->field_0x7c.v[0] = gVector3_006a4688.v[0];
            queued_smash->field_0x7c.v[1] = gVector3_006a4688.v[1];
            queued_smash->field_0x7c.v[2] = gVector3_006a4688.v[2];
        } else {
            queued_smash->field_0x40 = 0;
        }
        if (pOptional_position == NULL) {
            queued_smash->field_0x20.v[0] = gZero_vector__smash.v[0];
            queued_smash->field_0x20.v[1] = gZero_vector__smash.v[1];
            queued_smash->field_0x20.v[2] = gZero_vector__smash.v[2];
        } else {
            queued_smash->field_0x20.v[0] = pOptional_position->v[0];
            queued_smash->field_0x20.v[1] = pOptional_position->v[1];
            queued_smash->field_0x20.v[2] = pOptional_position->v[2];
        }
        queued_smash->field_0x3c = pArg10;
        if (pCollision_info == NULL || pCollision_info->owner == NULL || pCollision_info->flags_0x238 != 1) {
            queued_smash->field_0x14.v[0] = 0.f;
            queued_smash->field_0x14.v[1] = 0.f;
            queued_smash->field_0x14.v[2] = 0.f;
        } else {
            queued_smash->field_0x14.v[0] = (br_scalar)(pCollision_info->field_0x1b4.v[0] * gDOUBLE_0058af40);
            queued_smash->field_0x14.v[1] = (br_scalar)(pCollision_info->field_0x1b4.v[1] * gDOUBLE_0058af40);
            queued_smash->field_0x14.v[2] = (br_scalar)(pCollision_info->field_0x1b4.v[2] * gDOUBLE_0058af40);
        }
        gCount_queued_smashes = queued_count + 1;
        result = &pSmashable->levels[level_index];
    }
    return result;
final_failure:
    return NULL;
}

// C2PORT-NONMATCHING: best 47.71% | difftest PASS 2000 trials (spec difftest/specs/0043b510_CrushModelVertices.json) | reason x87 order and stack layout
// FUNCTION: CARMA2_HW 0x0043b510
int C2_HOOK_FASTCALL CrushModelVertices(br_model* pModel, tModel_mash_context* pContext) {
    tModel_detail_vertex_data* original_vertices;
    br_vertex* vertex;
    float work[3075];
    float displacement;
    int i;

    if (pModel == NULL || pModel->user == NULL) {
        return 0;
    }
    original_vertices = ((tUser_detail_level_model*)pModel->user)->field_0x4;
    if (original_vertices == NULL) {
        return 0;
    }
    if (pModel->nvertices > 0x400) {
        PDFatalError("Dog gone it, there's a lot of vertices on that model.");
    }

    vertex = pModel->vertices;
    displacement = gCrushZero;
    for (i = 0; i < pModel->nvertices; i++) {
        br_vector3 expanded;
        int axis;

        ExpandVector3(&expanded, &original_vertices[i].p, -10.f, 10.f);
        work[0] = vertex->p.v[0];
        work[1] = vertex->p.v[1];
        work[2] = vertex->p.v[2];
        for (axis = 0; axis < 3; axis++) {
            float delta;

            delta = fabsf(expanded.v[axis] - work[axis]);
            if (delta <= gCrushZero) {
                work[i * 3 + axis + 3] = 0.f;
            } else {
                float signed_delta;

                *pContext->total_displacement += delta;
                if (expanded.v[axis] < work[axis]) {
                    vertex->p.v[axis] -= pContext->step;
                    if (vertex->p.v[axis] < expanded.v[axis]) {
                        vertex->p.v[axis] = expanded.v[axis];
                    }
                    signed_delta = vertex->p.v[axis] - work[axis];
                    displacement -= signed_delta;
                } else {
                    vertex->p.v[axis] += pContext->step;
                    if (expanded.v[axis] < vertex->p.v[axis]) {
                        vertex->p.v[axis] = expanded.v[axis];
                    }
                    signed_delta = vertex->p.v[axis] - work[axis];
                    displacement += signed_delta;
                }
                work[i * 3 + axis + 3] = signed_delta;
                original_vertices[i].flags &= ~4;
            }
        }
        vertex++;
    }

    if (displacement == gCrushZero) {
        return 1;
    }
    if ((pModel->flags & BR_MODF_CUSTOM) != 0 && pModel->user != NULL) {
        *(tU32*)pModel->user = 1;
    } else {
        pModel->custom = JitModelUpdate;
        pModel->flags |= BR_MODF_CUSTOM;
    }
    AddModelMashToPipingSession(pModel, (br_vector3*)&work[3]);
    pContext->displacement += displacement;
    return 0;
}

// C2PORT-NONMATCHING: best 73.58% | difftest PASS 4000 trials (spec difftest/specs/00434320_FUN_00434320.json) | reason x87 scheduling (cross/hinge vectors) and prologue push placement; spec keeps hinge1 vertex unflagged (degenerate atan2) and stubs PipeSingleFlap
// FUNCTION: CARMA2_HW 0x00434320
int C2_HOOK_CDECL FUN_00434320(br_actor* pActor, int pArg2) {
    tUser_crush_data* user;
    tCar_crush_buffer_entry* crush_data;
    tModel_detail_vertex_data* detail_vertices;
    tCar_crush_flap_data* flap;
    br_model* model;
    br_model* parent_model;
    br_vertex* vertices;
    br_vertex* hinge_vertex;
    br_vector3 edge1;
    br_vector3 edge2;
    br_vector3 rotation_axis;
    br_vector3 hinge_axis;
    float length;
    float dot;
    int local_angle;
    tU16 min_angle;
    tU16 max_angle;
    tU16 vertex_angle;
    tU16 new_min_angle;
    tU16 new_max_angle;
    tU16 current_flap_angle;
    tU16 i;

    user = pActor->user;
    if (user != NULL
            && (crush_data = user->crush_data) != NULL
            && (flap = crush_data->flap_data) != NULL
            && flap->kev_o_flap == 0
            && flap->field_0x0 != 0
            && flap->field_0x15 != 0
            && (model = user->models[0]) != NULL
            && model->user != NULL
            && (detail_vertices = ((tUser_detail_level_model*)model->user)->field_0x4) != NULL) {
        vertices = model->vertices;
        hinge_vertex = &vertices[flap->hinge0];
        edge1.v[0] = vertices[flap->hinge1].p.v[0] - hinge_vertex->p.v[0];
        edge1.v[1] = vertices[flap->hinge1].p.v[1] - hinge_vertex->p.v[1];
        edge1.v[2] = vertices[flap->hinge1].p.v[2] - hinge_vertex->p.v[2];
        length = (float)sqrt((double)(edge1.v[0] * edge1.v[0] + edge1.v[1] * edge1.v[1] + edge1.v[2] * edge1.v[2]));
        if (length > gFLOAT_00589a5c) {
            length = (float)gDOUBLE_00589a60 / length;
            edge1.v[0] *= length;
            edge1.v[1] *= length;
            edge1.v[2] *= length;
        } else {
            edge1.v[0] = 1.f;
            edge1.v[1] = 0.f;
            edge1.v[2] = 0.f;
        }

        edge2.v[0] = vertices[flap->hinge2].p.v[0] - hinge_vertex->p.v[0];
        edge2.v[1] = vertices[flap->hinge2].p.v[1] - hinge_vertex->p.v[1];
        edge2.v[2] = vertices[flap->hinge2].p.v[2] - hinge_vertex->p.v[2];
        length = (float)sqrt((double)(edge2.v[0] * edge2.v[0] + edge2.v[1] * edge2.v[1] + edge2.v[2] * edge2.v[2]));
        if (length > gFLOAT_00589a5c) {
            length = (float)gDOUBLE_00589a60 / length;
            edge2.v[0] *= length;
            edge2.v[1] *= length;
            edge2.v[2] *= length;
        } else {
            edge2.v[0] = 1.f;
            edge2.v[1] = 0.f;
            edge2.v[2] = 0.f;
        }

        rotation_axis.v[0] = edge2.v[2] * edge1.v[1] - edge2.v[1] * edge1.v[2];
        rotation_axis.v[1] = edge2.v[0] * edge1.v[2] - edge2.v[2] * edge1.v[0];
        rotation_axis.v[2] = edge2.v[1] * edge1.v[0] - edge2.v[0] * edge1.v[1];
        length = (float)sqrt((double)(rotation_axis.v[0] * rotation_axis.v[0] + rotation_axis.v[1] * rotation_axis.v[1] + rotation_axis.v[2] * rotation_axis.v[2]));
        if (length > gFLOAT_00589a5c) {
            length = (float)gDOUBLE_00589a60 / length;
            rotation_axis.v[0] *= length;
            rotation_axis.v[1] *= length;
            rotation_axis.v[2] *= length;
        } else {
            rotation_axis.v[0] = 1.f;
            rotation_axis.v[1] = 0.f;
            rotation_axis.v[2] = 0.f;

        }

        dot = edge2.v[0] * edge1.v[0] + edge2.v[1] * edge1.v[1] + edge2.v[2] * edge1.v[2];
        hinge_axis.v[0] = edge2.v[0] - edge1.v[0] * dot;
        hinge_axis.v[1] = edge2.v[1] - edge1.v[1] * dot;
        hinge_axis.v[2] = edge2.v[2] - edge1.v[2] * dot;
        length = (float)sqrt((double)(hinge_axis.v[0] * hinge_axis.v[0] + hinge_axis.v[1] * hinge_axis.v[1] + hinge_axis.v[2] * hinge_axis.v[2]));
        if (length > gFLOAT_00589a5c) {
            length = (float)gDOUBLE_00589a60 / length;
            hinge_axis.v[0] *= length;
            hinge_axis.v[1] *= length;
            hinge_axis.v[2] *= length;
        } else {
            hinge_axis.v[0] = 1.f;
            hinge_axis.v[1] = 0.f;
            hinge_axis.v[2] = 0.f;
        }

        parent_model = ((tUser_crush_data*)pActor->parent->user)->models[0];
        DrMatrix34ApplyLPInverse(&edge2, &parent_model->vertices[flap->field_0x12].p, &pActor->t.t.mat);
        edge2.v[0] = edge2.v[0] - hinge_vertex->p.v[0];
        edge2.v[1] = edge2.v[1] - hinge_vertex->p.v[1];
        edge2.v[2] = edge2.v[2] - hinge_vertex->p.v[2];
        dot = edge2.v[0] * hinge_axis.v[0] + edge2.v[1] * hinge_axis.v[1] + edge2.v[2] * hinge_axis.v[2];
        if (dot == gCrushZero) {
            local_angle = 0;
        } else {
            local_angle = (int)(atan2((double)(edge2.v[0] * rotation_axis.v[0] + edge2.v[1] * rotation_axis.v[1] + edge2.v[2] * rotation_axis.v[2]), (double)dot) * gDOUBLE_00589ad0);
        }

        max_angle = 0x3ffc;
        min_angle = -0x3ffc;
        for (i = 0; i < model->nvertices; i++) {
            if ((detail_vertices[i].flags & 2) != 0) {
                edge2.v[0] = vertices[i].p.v[0] - hinge_vertex->p.v[0];
                edge2.v[1] = vertices[i].p.v[1] - hinge_vertex->p.v[1];
                edge2.v[2] = vertices[i].p.v[2] - hinge_vertex->p.v[2];
                dot = edge2.v[0] * hinge_axis.v[0] + edge2.v[1] * hinge_axis.v[1] + edge2.v[2] * hinge_axis.v[2];
                if (dot != gCrushZero) {
                    vertex_angle = (int)(atan2((double)(edge2.v[0] * rotation_axis.v[0] + edge2.v[1] * rotation_axis.v[1] + edge2.v[2] * rotation_axis.v[2]), (double)dot) * gDOUBLE_00589ad0);
                    if ((tU16)(vertex_angle - min_angle) < 0x7ff8) {
                        min_angle = vertex_angle;
                    }
                    if ((tU16)(max_angle - vertex_angle) < 0x7ff8) {
                        max_angle = vertex_angle;
                    }
                }
            }
        }

        current_flap_angle = flap->field_0x4;
        new_min_angle = local_angle + current_flap_angle - max_angle;
        new_max_angle = local_angle + current_flap_angle - min_angle + 0x3ffc;
        flap->field_0x2 = (tU16)new_min_angle;
        flap->field_0x6 = (tU16)new_max_angle;
        flap->field_0x8 = (tU16)(new_max_angle - new_min_angle);
        if ((tU16)(current_flap_angle - new_min_angle) > (tU16)(new_max_angle - new_min_angle)) {
            if ((tU16)(current_flap_angle - new_max_angle) < 0x7ff8) {
                PipeSingleFlap((uintptr_t)pActor, (tU16)(new_max_angle - current_flap_angle));
                flap->field_0x4 = flap->field_0x6;
            } else {
                PipeSingleFlap((uintptr_t)pActor, (tU16)(new_min_angle - current_flap_angle));
                flap->field_0x4 = flap->field_0x2;
            }
        }
        BrMatrix34Translate(&pActor->t.t.mat,
            -hinge_vertex->p.v[0], -hinge_vertex->p.v[1], -hinge_vertex->p.v[2]);
        BrMatrix34PostRotate(&pActor->t.t.mat, flap->field_0x4, &edge1);
        parent_model = ((tUser_crush_data*)pActor->parent->user)->models[0];
        BrMatrix34PostTranslate(&pActor->t.t.mat,
            parent_model->vertices[flap->field_0xc].p.v[0],
            parent_model->vertices[flap->field_0xc].p.v[1],
            parent_model->vertices[flap->field_0xc].p.v[2]);
    }
    return 0;
}

// C2PORT-NONMATCHING: best 66.33% | difftest PASS 2000 trials (spec difftest/specs/0043d690_SendSplitHalfNonCarData.json) | reason register-allocation
// FUNCTION: CARMA2_HW 0x0043d690
void C2_HOOK_FASTCALL SendSplitHalfNonCarData(tNon_car_spec* pNon_car, undefined4 pArg2, tNet_game_player_info* pRecipient) {
    tU8* message;
    int hierarchy_size;
    int contents_size;
    int written_size;
    tCar_spec* car;
    tNet_game_player_info* player;
    tNet_message* guaranteed_message;
    br_vector3* crush_vector;
    register undefined4 local_arg2;

    local_arg2 = pArg2;
    guaranteed_message = NULL;
    car = NULL;
    hierarchy_size = GetHierarchyNetworkSize(pNon_car->collision_info);
    contents_size = hierarchy_size + 0x16;
    if (pRecipient == NULL) {
        if (pNon_car->collision_info->disable_move_rotate == 0 || (pNon_car->flags & 0x100000) == 0) {
            message = (tU8*)NetStartBroadcastContents(0x29, contents_size);
        } else {
            guaranteed_message = NetBuildGuaranteedMessage(0x29, contents_size);
            message = (tU8*)guaranteed_message + 0x18;
            pNon_car->flags &= ~0x100000;
        }
    } else {
        message = (tU8*)NetGetToPlayerContents(0x29, contents_size, pRecipient->ID);
    }
    if (message != NULL) {
        if (pNon_car->actor != NULL) {
            car = (tCar_spec*)pNon_car->actor->user;
        }
        player = NetPlayerFromCar(car);
        *(undefined4*)(message + 8) = local_arg2;
        *(tPlayer_ID*)(message + 4) = player->ID;
        *(tU16*)(message + 0xc) = (tU16)(pNon_car->collision_info->disable_move_rotate << 1);
        crush_vector = &car->car_crush_spec->field_0x548;
        CompressVector3((tCompressed_vector3*)(message + 0xe), crush_vector, -1.0f, 1.0f);
        *(tU16*)(message + 0x14) = DRScalarToU16(*(float*)((tU8*)crush_vector + 0xc), -2.5f, 2.5f);
        written_size = GetHierarchyNetworkStuff(pNon_car->collision_info, message + 0x16, 0x48);
        if (written_size != hierarchy_size) {
            FatalError(kFatalError_NetContentsTooBig_S, "in split half non_car object data");
        }
        if (pRecipient == NULL) {
            if (guaranteed_message == NULL) {
                NetBroadcastContents((tNet_message_chunk*)message);
                return;
            }
            NetGuaranteedSendMessageToEverybody(gCurrent_net_game, guaranteed_message, 0);
        }
    }
}

// C2PORT-NONMATCHING: best 34.52% | difftest PASS 2000 trials (spec difftest/specs/0042e2e0_FUN_0042e2e0.json) | reason x87-scheduling-stack-layout
// FUNCTION: CARMA2_HW 0x0042e2e0
void C2_HOOK_FASTCALL FUN_0042e2e0(tCar_spec* pCar, float* pPlane, int pFlag) {
    char* car_bytes = (char*)pCar;
    int iVar1;
    float fVar2;
    float fVar3;
    float fVar4;
    float fVar5;
    int iVar6;
    undefined4 uVar7;
    undefined4 uVar8;
    int bVar9;
    int bVar10;
    float fVar11;
    float fVar12;
    undefined4 *puVar13;
    float *pfVar14;
    int *piVar15;
    int *piVar16;
    int iVar17;
    undefined4 *puVar18;
    int iVar19;
    int iVar20;
    int *piVar21;
    int iVar22;
    int uVar23;
    int uVar24;
    int iVar25;
    int iVar26;
    float local_50;
    int local_44;
    int local_38;
    int local_2c;
    undefined local_20 [4];
    float local_1c [3];
    br_vector4 split_plane;

    if (((*(int *)(car_bytes + 0x1b4) == 0) || (*(int *)(car_bytes + 0x1d4) != 0)) &&
          (*(float *)(car_bytes + 0x4c8) == gDOUBLE_00589a60)) {
        iVar26 = *(int *)(car_bytes + 8);
        iVar6 = *(int *)(car_bytes + 0x18d4);
        pfVar14 = (float *)(iVar26 + 0x24);
        local_1c[0] = (*(float *)(iVar26 + 0x30) - *pfVar14) * gFLOAT_00589a28;
        local_1c[1] = (*(float *)(iVar26 + 0x34) - *(float *)(iVar26 + 0x28)) * gFLOAT_00589a28;
        local_1c[2] = (*(float *)(iVar26 + 0x38) - *(float *)(iVar26 + 0x2c)) * gFLOAT_00589a28;
        if (fabsf(*pPlane) <= gCrushSqrtHalf) {
            iVar19 = 1;
            if (fabsf(pPlane[1]) <= gCrushSqrtHalf) {
                iVar19 = 2;
            }
        }
        else {
            iVar19 = 0;
        }
        fVar2 = pPlane[1];
        uVar23 = (iVar19 + 1) % 3;
        fVar3 = pPlane[2];
        fVar4 = pPlane[iVar19];
        fVar5 = pPlane[iVar19];
        local_1c[iVar19] = 0.0;
        bVar9 = gCrushZero <= fVar5;
        uVar24 = (iVar19 + 2) % 3;
        fVar5 = pfVar14[uVar24];
        local_1c[uVar23] = (*(float *)(iVar26 + 0x30 + uVar23 * 4) + pfVar14[uVar23]) * gFLOAT_00589a2c;
        local_1c[uVar24] = (fVar5 + *(float *)(iVar26 + 0x30 + uVar24 * 4)) * gFLOAT_00589a2c;
        fVar2 = -(((local_1c[0] * *pPlane + fVar3 * local_1c[2] + fVar2 * local_1c[1]) - pPlane[3]) /
                          fVar4);
        DRActorEnumRecurse((br_actor*)*(intptr_t*)(car_bytes + 0xe0c), MakeCarModelsMaterialsDoubleSided, (void*)car_bytes);
        split_plane.v[0] = *pPlane;
        split_plane.v[1] = pPlane[1];
        split_plane.v[2] = pPlane[2];
        iVar26 = *(int *)(car_bytes + 0xe0c);
        iVar22 = *(int *)(iVar6 + 0x4d0);
        uVar7 = *(undefined4 *)(iVar6 + 0x4cc);
        piVar21 = *(int **)(iVar26 + 0x60);
        puVar18 = (undefined4 *)(iVar6 + 0x4c4);
        split_plane.v[3] = pPlane[3] -
                            (*(float *)(iVar26 + 0x58) * split_plane.v[2] +
                            *(float *)(iVar26 + 0x54) * split_plane.v[1] + *(float *)(iVar26 + 0x50) * split_plane.v[0]);
        FUN_0042f5d0((br_model*)*piVar21, (br_model**)(iVar6 + 0x4c4), (br_model**)(iVar22 + 0x18), split_plane.v, 0);
        iVar26 = 1;
        piVar16 = piVar21;
        if (1 < *(int *)(car_bytes + 0xe14)) {
            do {
                if (piVar16[1] != 0) {
                    FUN_0042f5d0((br_model*)piVar16[1], (br_model**)&piVar16[1], (br_model**)local_20, split_plane.v, 1);
                }
                iVar26 = iVar26 + 1;
                piVar16 = piVar16 + 1;
            } while (iVar26 < *(int *)(car_bytes + 0xe14));
        }
        iVar26 = (int)FUN_0042f170(&gSplit_car_crush_info_buffer, (br_actor*)uVar7, pCar);
        if (iVar26 == 0) {
            iVar26 = 0;
            if (0 < *(int *)(car_bytes + 0xe14)) {
                do {
                    BrModelFree(BrModelRemove((br_model*)*puVar18));
                    iVar26 = iVar26 + 1;
                    puVar18 = puVar18 + 1;
                } while (iVar26 < *(int *)(car_bytes + 0xe14));
            }
            BrModelFree((br_model*)*(int*)(iVar22 + 0x18));
            DRActorEnumRecurse((br_actor*)*(intptr_t*)(car_bytes + 0xe0c), MakeCarModelsMaterialsSingleSided, (void*)car_bytes)
            ;
            return;
        }
        CloseDownPowerUpsForCar(pCar);
        piVar15 = (int *)(*(int *)(iVar26 + 8) + 0x20);
        piVar16 = (int *)*piVar15;
        iVar25 = *piVar16;
        while (iVar25 == 2) {
            *piVar15 = piVar16[0xd];
            piVar15 = (int *)(*(int *)(iVar26 + 8) + 0x20);
            piVar16 = (int *)*piVar15;
            iVar25 = *piVar16;
        }
        iVar25 = 0;
        if (0 < *(int *)(car_bytes + 0xe14)) {
            iVar20 = (int)piVar21 - (int)puVar18;
            do {
                uVar8 = *(undefined4 *)(iVar20 + (int)puVar18);
                *(undefined4 *)(iVar20 + (int)puVar18) = *puVar18;
                *puVar18 = uVar8;
                iVar25 = iVar25 + 1;
                puVar18 = puVar18 + 1;
            } while (iVar25 < *(int *)(car_bytes + 0xe14));
        }
        *(int *)(*(int *)(car_bytes + 0xe0c) + 0x18) = *piVar21;
        FUN_004f5a60(pCar);
        if (gNet_mode != 0) {
            iVar25 = 0;
            if (0 < *(int *)(iVar6 + 0x270)) {
                do {
                    FullyDetachBit(pCar, (tPhysics_object*)*(int*)(iVar6 + 0x27c + iVar25 * 0x14));
                    iVar25 = iVar25 + 1;
                } while (iVar25 < *(int *)(iVar6 + 0x270));
            }
            MungeDetachLists((tCar_crush_spec*)iVar6);
        }
        FUN_004c86b0((tU32)gNon_track_actor, (br_actor*)uVar7);
        BrActorAdd(gNon_track_actor, (br_actor*)uVar7);
        BrMatrix34Copy((br_matrix34*)((char*)uVar7 + 0x2c), (br_matrix34*)((char*)pCar + 0x3c));
        PHILSendDetachBit(*(tU16*)((char*)pCar + 0x80), (void*)uVar7);
        puVar18 = *(undefined4 **)(*(int *)(car_bytes + 0xe0c) + 8);
        while (puVar13 = puVar18, puVar13 != (undefined4 *)0x0) {
            puVar18 = (undefined4 *)*puVar13;
            piVar21 = (int *)puVar13[0x18];
            if ((piVar21 == (int *)0x0) || (piVar21[3] == 0)) {
                iVar25 = 0;
                piVar16 = (int *)(car_bytes + 0x12ec);
                do {
                    if ((puVar13 == (undefined4 *)piVar16[6]) || (puVar13 == (undefined4 *)*piVar16))
                    goto LAB_0042f10b;
                    iVar25 = iVar25 + 1;
                    piVar16 = piVar16 + 1;
                } while (iVar25 < 6);
                if ((piVar21 != (int *)0x0) && (iVar25 = *piVar21, iVar25 != 0)) {
                    bVar10 = gCrushZero <
                                      (*(float *)(iVar25 + 0x34) * split_plane.v[0] +
                                      *(float *)(iVar25 + 0x3c) * split_plane.v[2] + *(float *)(iVar25 + 0x38) * split_plane.v[1]) -
                                      split_plane.v[3];
                    if (bVar10 == gCrushZero <
                                                (*(float *)(iVar25 + 0x44) * split_plane.v[1] +
                                                *(float *)(iVar25 + 0x40) * split_plane.v[0] +
                                                *(float *)(iVar25 + 0x48) * split_plane.v[2]) - split_plane.v[3]) {
                        if (!bVar10) {
                            FUN_004c86b0(0, (br_actor*)puVar13);
                            BrActorRemove((br_actor*)puVar13);
                            FUN_004c86b0((tU32)(iVar6 + 0x4c4), (br_actor*)puVar13);
                            BrActorAdd((br_actor*)iVar22, (br_actor*)puVar13);
                            puVar13[6] = iVar25;
                        }
                    }
                    else if (*(int *)(*(int *)(car_bytes + 0x18d4) + 0x494) != 8) {
                        FUN_004c86b0(0, (br_actor*)puVar13);
                        BrActorRemove((br_actor*)puVar13);
                        *(undefined4 **)
                          (*(int *)(car_bytes + 0x18d4) + 0x498 + *(int *)(*(int *)(car_bytes + 0x18d4) + 0x494) * 4)
                                  = puVar13;
                        piVar21 = (int *)(*(int *)(car_bytes + 0x18d4) + 0x494);
                        *piVar21 = *piVar21 + 1;
                    }
                }
            }
            else {
LAB_0042f10b:
                if ((((float *)puVar13)[0x14] * *pPlane +
                        ((float *)puVar13)[0x16] * pPlane[2] + pPlane[1] * ((float *)puVar13)[0x15]) - pPlane[3] <
                        gCrushZero) {
                    FUN_004c86b0(0, (br_actor*)puVar13);
                    BrActorRemove((br_actor*)puVar13);
                    FUN_004c86b0((tU32)(iVar6 + 0x4c4), (br_actor*)puVar13);
                    BrActorAdd((br_actor*)iVar22, (br_actor*)puVar13);
                }
            }
        }
        gCount_crush_detach_list = 0;
        puVar18 = *(undefined4 **)(car_bytes + 8);
        *(undefined4 *)(iVar6 + 0x4d4) = *puVar18;
        *(float *)(iVar6 + 0x4d8) = ((float *)puVar18)[1] / *(float *)(car_bytes + 0x4c8);
        *(float *)(iVar6 + 0x4dc) = ((float *)puVar18)[2] / *(float *)(car_bytes + 0x4c8);
        *(float *)(iVar6 + 0x4e0) = ((float *)puVar18)[3] / *(float *)(car_bytes + 0x4c8);
        *(float *)(iVar6 + 0x4e4) = ((float *)puVar18)[4] / *(float *)(car_bytes + 0x4c8);
        *(undefined4 *)(iVar6 + 0x4e8) = puVar18[5];
        *(undefined4 *)(iVar6 + 0x4ec) = puVar18[6];
        *(undefined4 *)(iVar6 + 0x4f0) = puVar18[7];
        DRBoundsCopy((br_bounds3*)(iVar6 + 0x4f4), (br_bounds3*)(*(int*)(car_bytes + 8) + 0x24));
        DRBoundsCopy((br_bounds3*)(iVar6 + 0x50c), (br_bounds3*)(*(int*)(car_bytes + 8) + 0x3c));
        *(undefined4 *)(iVar6 + 0x524) = puVar18[0x15];
        *(undefined4 *)(iVar6 + 0x528) = puVar18[0x16];
        *(undefined4 *)(iVar6 + 0x52c) = puVar18[0x17];
        *(undefined4 *)(iVar6 + 0x530) = puVar18[0x1a];
        *(undefined4 *)(iVar6 + 0x534) = puVar18[0x1b];
        *(undefined4 *)(iVar6 + 0x538) = puVar18[0x1c];
        *(undefined4 *)(iVar6 + 0x53c) = puVar18[0x1d];
        *(undefined4 *)(iVar6 + 0x540) = puVar18[0x1e];
        *(undefined4 *)(iVar6 + 0x544) = puVar18[0x1f];
        BrMatrix34Copy((br_matrix34*)(*(int*)(iVar26 + 8) + 0x8c), (br_matrix34*)(*(int*)(car_bytes + 8) + 0x8c));
        *(undefined4 *)(*(int *)(iVar26 + 8) + 600) = *(undefined4 *)(*(int *)(car_bytes + 8) + 600);
        *(undefined4 *)(*(int *)(iVar26 + 8) + 0x25c) = *(undefined4 *)(*(int *)(car_bytes + 8) + 0x25c);
        *(undefined4 *)(*(int *)(iVar26 + 8) + 0x244) = *(undefined4 *)(*(int *)(car_bytes + 8) + 0x244);
        *(undefined4 *)(*(int *)(iVar26 + 8) + 0x248) = *(undefined4 *)(*(int *)(car_bytes + 8) + 0x248);
        *(undefined4 *)(*(int *)(iVar26 + 8) + 0x24c) = *(undefined4 *)(*(int *)(car_bytes + 8) + 0x24c);
        *(undefined4 *)(*(int *)(iVar26 + 8) + 0x250) = *(undefined4 *)(*(int *)(car_bytes + 8) + 0x250);
        *(undefined4 *)(*(int *)(iVar26 + 8) + 0x254) = *(undefined4 *)(*(int *)(car_bytes + 8) + 0x254);
        iVar22 = *(int *)(car_bytes + 8);
        fVar4 = *(float *)(iVar6 + 0x500 + iVar19 * 4) - fVar2;
        fVar5 = fVar2 - *(float *)(iVar6 + 0x4f4 + iVar19 * 4);
        fVar11 = *(float *)(iVar6 + 0x4e8 + iVar19 * 4) - *(float *)(iVar6 + 0x4f4 + iVar19 * 4);
        iVar25 = *(int *)(iVar26 + 8);
        local_50 = (fVar11 * fVar5) / (fVar4 + fVar5);
        fVar3 = (fVar5 - local_50) * fVar5;
        fVar12 = (*(float *)(iVar6 + 0x4d8) * fVar3) / (local_50 * fVar4 + fVar3);
        fVar3 = *(float *)(iVar6 + 0x4d8);
        *(undefined4 *)(iVar22 + 0x14) = *(undefined4 *)(iVar6 + 0x4e8);
        fVar3 = fVar3 - fVar12;
        *(undefined4 *)(iVar22 + 0x18) = *(undefined4 *)(iVar6 + 0x4ec);
        *(undefined4 *)(iVar22 + 0x1c) = *(undefined4 *)(iVar6 + 0x4f0);
        *(undefined4 *)(iVar25 + 0x14) = *(undefined4 *)(iVar6 + 0x4e8);
        *(undefined4 *)(iVar25 + 0x18) = *(undefined4 *)(iVar6 + 0x4ec);
        *(undefined4 *)(iVar25 + 0x1c) = *(undefined4 *)(iVar6 + 0x4f0);
        local_50 = *(float *)(iVar6 + 0x4f4 + iVar19 * 4) + local_50;
        fVar4 = (fVar4 * fVar11) / (fVar4 + fVar5) + fVar5 + *(float *)(iVar6 + 0x4f4 + iVar19 * 4);
        if (fVar2 - gFLOAT_00589a90 < local_50) {
            local_50 = fVar2 - gFLOAT_00589a90;
        }
        if (fVar4 < fVar2 - gFLOAT_00589a94) {
            fVar4 = fVar2 - gFLOAT_00589a94;
        }
        if (bVar9) {
            *(float *)(iVar22 + 4) = fVar3;
            *(float *)(iVar25 + 4) = fVar12;
            *(float *)(iVar22 + 0x14 + iVar19 * 4) = fVar4;
        }
        else {
            *(float *)(iVar22 + 4) = fVar12;
            *(float *)(iVar25 + 4) = fVar3;
            *(float *)(iVar22 + 0x14 + iVar19 * 4) = local_50;
            local_50 = fVar4;
        }
        *(float *)(iVar25 + 0x14 + iVar19 * 4) = local_50;
        *(undefined4 *)(iVar22 + 0x74) = *(undefined4 *)(iVar6 + 0x53c);
        *(undefined4 *)(iVar22 + 0x78) = *(undefined4 *)(iVar6 + 0x540);
        *(undefined4 *)(iVar22 + 0x7c) = *(undefined4 *)(iVar6 + 0x544);
        *(undefined4 *)(iVar25 + 0x74) = *(undefined4 *)(iVar6 + 0x53c);
        *(undefined4 *)(iVar25 + 0x78) = *(undefined4 *)(iVar6 + 0x540);
        *(undefined4 *)(iVar25 + 0x7c) = *(undefined4 *)(iVar6 + 0x544);
        local_1c[1] = *(float *)(iVar22 + 0x18) - *(float *)(iVar6 + 0x4ec);
        local_1c[2] = *(float *)(iVar22 + 0x1c) - *(float *)(iVar6 + 0x4f0);
        local_1c[0] = *(float *)(iVar22 + 0x14) - *(float *)(iVar6 + 0x4e8);
        *(float *)(iVar22 + 0x68) =
                  *(float *)(iVar6 + 0x540) * local_1c[2] - *(float *)(iVar6 + 0x544) * local_1c[1];
        *(float *)(iVar22 + 0x6c) =
                  *(float *)(iVar6 + 0x544) * local_1c[0] - *(float *)(iVar6 + 0x53c) * local_1c[2];
        *(float *)(iVar22 + 0x70) =
                  *(float *)(iVar6 + 0x53c) * local_1c[1] - *(float *)(iVar6 + 0x540) * local_1c[0];
        BrMatrix34ApplyV((br_vector3*)local_1c, (br_vector3*)(iVar22 + 0x68), (br_matrix34*)(*(int*)(iVar6 + 0x4d4) + 0x2c));
        *(float *)(iVar22 + 0x68) = local_1c[0] + *(float *)(iVar6 + 0x530);
        *(float *)(iVar22 + 0x6c) = local_1c[1] + *(float *)(iVar6 + 0x534);
        *(float *)(iVar22 + 0x70) = local_1c[2] + *(float *)(iVar6 + 0x538);
        BrMatrix34TApplyV((br_vector3*)(iVar22 + 0x1a8), (br_vector3*)(iVar22 + 0x68), (br_matrix34*)(*(int*)iVar22 + 0x2c));
        local_1c[1] = *(float *)(iVar25 + 0x18) - *(float *)(iVar6 + 0x4ec);
        local_1c[2] = *(float *)(iVar25 + 0x1c) - *(float *)(iVar6 + 0x4f0);
        local_1c[0] = *(float *)(iVar25 + 0x14) - *(float *)(iVar6 + 0x4e8);
        *(float *)(iVar25 + 0x68) =
                  *(float *)(iVar6 + 0x540) * local_1c[2] - *(float *)(iVar6 + 0x544) * local_1c[1];
        *(float *)(iVar25 + 0x6c) =
                  *(float *)(iVar6 + 0x544) * local_1c[0] - *(float *)(iVar6 + 0x53c) * local_1c[2];
        *(float *)(iVar25 + 0x70) =
                  *(float *)(iVar6 + 0x53c) * local_1c[1] - *(float *)(iVar6 + 0x540) * local_1c[0];
        BrMatrix34ApplyV((br_vector3*)local_1c, (br_vector3*)(iVar25 + 0x68), (br_matrix34*)(*(int*)(iVar6 + 0x4d4) + 0x2c));
        *(float *)(iVar25 + 0x68) = local_1c[0] + *(float *)(iVar6 + 0x530);
        *(float *)(iVar25 + 0x6c) = local_1c[1] + *(float *)(iVar6 + 0x534);
        *(float *)(iVar25 + 0x70) = local_1c[2] + *(float *)(iVar6 + 0x538);
        BrMatrix34TApplyV((br_vector3*)(iVar25 + 0x1a8), (br_vector3*)(iVar25 + 0x68), (br_matrix34*)(*(int*)iVar25 + 0x2c));
        iVar17 = (iVar19 + 2) * 4;
        iVar20 = uVar23 * 4 + 8;
        iVar1 = uVar24 * 4 + 8;
        *(float *)(iVar22 + iVar17) =
                  (*(float *)(iVar6 + 0x4d4 + iVar17) * *(float *)(iVar22 + 4)) / *(float *)(iVar6 + 0x4d8);
        *(float *)(iVar25 + iVar17) =
                  (*(float *)(iVar25 + 4) * *(float *)(iVar6 + 0x4d4 + iVar17)) / *(float *)(iVar6 + 0x4d8);
        fVar2 = *(float *)(iVar25 + 0x14 + iVar19 * 4) - *(float *)(iVar6 + 0x4e8 + iVar19 * 4);
        fVar3 = *(float *)(iVar22 + 0x14 + iVar19 * 4) - *(float *)(iVar6 + 0x4e8 + iVar19 * 4);
        fVar5 = *(float *)(iVar25 + 4) * fVar2 * fVar2 + *(float *)(iVar22 + 4) * fVar3 * fVar3;
        fVar2 = *(float *)(iVar22 + 4);
        fVar11 = *(float *)(iVar6 + 0x4d4 + iVar20) - fVar5;
        fVar3 = *(float *)(iVar6 + 0x4d8);
        local_1c[uVar23] = fVar11;
        fVar4 = local_1c[uVar23];
        *(float *)(iVar22 + iVar20) = (fVar2 * fVar11) / fVar3;
        *(float *)(iVar25 + iVar20) = (*(float *)(iVar25 + 4) * fVar4) / *(float *)(iVar6 + 0x4d8);
        fVar5 = *(float *)(iVar6 + 0x4d4 + iVar1) - fVar5;
        fVar2 = *(float *)(iVar22 + 4);
        local_44 = 0;
        local_38 = 0;
        fVar3 = *(float *)(iVar6 + 0x4d8);
        local_1c[uVar24] = fVar5;
        fVar4 = local_1c[uVar24];
        *(float *)(iVar22 + iVar1) = (fVar2 * fVar5) / fVar3;
        *(float *)(iVar25 + iVar1) = (*(float *)(iVar25 + 4) * fVar4) / *(float *)(iVar6 + 0x4d8);
        *(undefined *)(iVar22 + 0xed) = 1;
        *(undefined *)(iVar25 + 0xed) = 1;
        *(undefined *)(iVar22 + 0xec) = 0;
        *(undefined *)(iVar25 + 0xec) = 0;
        piVar21 = *(int **)(car_bytes + 0x18d4);
        iVar22 = *(int *)(iVar26 + 8);
        iVar25 = *(int *)(iVar22 + 0x20);
        if (0 < *piVar21) {
            local_50 = 0.0;
            do {
                if (*(int *)(piVar21[1] + (int)local_50 + 8) == 0) {
                    SplitPolyhedronByPlane((int*)*(int*)(piVar21[1] + (int)local_50), *(int*)(piVar21[1] + (int)local_50 + 4), iVar25, pPlane);
                    iVar25 = *(int *)(iVar25 + 0x34);
                    if (local_44 == 0) {
                        *(undefined4 *)(*(int *)(car_bytes + 8) + 0x20) =
                                  *(undefined4 *)((int)local_50 + 4 + piVar21[1]);
                    }
                    else {
                        *(undefined4 *)(local_44 + 0x34) = *(undefined4 *)((int)local_50 + 4 + piVar21[1]);
                    }
                    local_44 = *(int *)((int)local_50 + 4 + piVar21[1]);
                }
                local_38 = local_38 + 1;
                local_50 = (float)((int)local_50 + 0x1c);
            } while (local_38 < *piVar21);
        }
        if (local_44 != 0) {
            *(undefined4 *)(local_44 + 0x34) = 0;
        }
        if (local_38 < 4) {
            local_38 = 4 - local_38;
            do {
                *(undefined4 *)(iVar25 + 0x38) = 0;
                *(undefined4 *)(iVar25 + 0x3c) = 0;
                *(undefined4 *)(iVar25 + 0x40) = 0;
                iVar25 = *(int *)(iVar25 + 0x34);
                local_38 = local_38 + -1;
            } while (local_38 != 0);
        }
        piVar21 = piVar21 + 0x159;
        local_2c = 4;
        do {
            iVar25 = *piVar21;
            pfVar14 = *(float **)(iVar25 + 0x40);
            if ((pfVar14[1] * pPlane[1] + pPlane[2] * pfVar14[2] + *pfVar14 * *pPlane) - pPlane[3] <=
                    gCrushZero) {
                uVar7 = *(undefined4 *)(iVar22 + 0x20);
                *(int *)(iVar22 + 0x20) = iVar25;
                *(undefined4 *)(*piVar21 + 0x34) = uVar7;
            }
            else {
                uVar7 = *(undefined4 *)(*(int *)(car_bytes + 8) + 0x20);
                *(int *)(*(int *)(car_bytes + 8) + 0x20) = iVar25;
                *(undefined4 *)(*piVar21 + 0x34) = uVar7;
            }
            piVar21 = piVar21 + 1;
            local_2c = local_2c + -1;
        } while (local_2c != 0);
        *(undefined4 *)(iVar6 + 0x28) = 1;
        puVar18 = (undefined4 *)(car_bytes + 0x1264);
        iVar22 = 4;
        do {
            *puVar18 = *(undefined4 *)(car_bytes + 0x1220);
            puVar18 = puVar18 + 1;
            iVar22 = iVar22 + -1;
        } while (iVar22 != 0);
        UpdateCollisionObject((tPhysics_object*)*(int*)(iVar26 + 8));
        UpdateCollisionObject(pCar->collision_info);
        *(float *)(*(int *)(car_bytes + 8) + 0x40) = GetCarOverallBoundsMinY(pCar);
        *(undefined4 *)(car_bytes + 0x12e8) = 0;
        iVar22 = *(int *)(*(int *)(car_bytes + 8) + 0x224);
        while (iVar22 != 0) {
            iVar25 = *(int *)(iVar22 + 0x188);
            if (gCrushZero <=
                    (*(float *)(iVar25 + 0x18) * pPlane[1] +
                    pPlane[2] * *(float *)(iVar25 + 0x1c) + *(float *)(iVar25 + 0x14) * *pPlane) -
                    pPlane[3]) {
                iVar22 = *(int *)(iVar22 + 0x220);
            }
            else {
                iVar1 = iVar22;
                PHILSendDetachBit(*(tU16*)(*(int*)(*(int*)iVar1 + 0x5c) + 0x80), (void*)*(int*)iVar1);
                iVar22 = *(int *)(iVar1 + 0x220);
                PHILRemoveFromParentChain((tPhysics_object*)iVar1);
                PHILDetachObjectState(pCar->collision_info);
                PhysicsAddObject((tPhysics_object*)*(int*)(iVar26 + 8), (tPhysics_object*)iVar1);
                PHILDetachObjectState((tPhysics_object*)*(int*)(iVar26 + 8));
            }
        }
        if (gNum_active_non_cars != 99) {
            *(undefined4 *)(iVar26 + 0xc) = gSplit_car_driver;
            gActive_non_car_list[gNum_active_non_cars] = (tNon_car_spec*)iVar26;
            gNum_active_non_cars = gNum_active_non_cars + 1;
            PHILQueueDetachBit((tPhysics_object*)*(int*)(iVar26 + 8), NULL, NULL, NULL);
        }
        *(undefined *)(iVar6 + 0x4b8) = 1;
        *(undefined4 *)(car_bytes + 0xe24) = 0;
        *(float *)(iVar6 + 0x548) = *pPlane;
        *(float *)(iVar6 + 0x54c) = pPlane[1];
        *(float *)(iVar6 + 0x550) = pPlane[2];
        *(float *)(iVar6 + 0x554) = pPlane[3];
        if (iVar19 != 1) {
            *(undefined *)(car_bytes + 0x18c8) = 1;
            *(undefined4 *)(car_bytes + 0x1340) = 0;
            if (((car_bytes != 0) && (*(int *)(car_bytes + 0xc) == 6)) || (gNet_mode == 2)) {
                KnackerThisCar(pCar);
            }
        }
        if (pFlag != 0) {
            ApplyRandomDamageToCar(pCar, gMin_split_damage, gMax_split_damage);
        }
        FUN_004c86f0(*(tU16*)((char*)pCar + 0x80), pPlane, 1);
    }
    return;
}

// C2PORT-NONMATCHING: best 60.47% | difftest PASS 2000 trials (spec difftest/specs/004f3740_FUN_004f3740.json) | reason x87-order
// FUNCTION: CARMA2_HW 0x004f3740
void C2_HOOK_FASTCALL FUN_004f3740(const int* pIndices, const float* pTransform, const float* pPoint, float* pResult) {
    pResult[pIndices[0]] = (((pPoint[0] - pTransform[3]) + pTransform[7] * pTransform[pIndices[0]]) * pTransform[10]
        - ((pPoint[1] - pTransform[4]) + pTransform[9] * pTransform[pIndices[0]]) * pTransform[8])
        / (pTransform[10] * pTransform[7] - pTransform[9] * pTransform[8]);
    pResult[pIndices[1]] = (((pPoint[0] - pTransform[3]) + pTransform[pIndices[1]] * pTransform[8]) * pTransform[9]
        - ((pPoint[1] - pTransform[4]) + pTransform[10] * pTransform[pIndices[1]]) * pTransform[7])
        / (pTransform[9] * pTransform[8] - pTransform[10] * pTransform[7]);
    pResult[pIndices[2]] = (*(const float*)((const tU8*)pIndices + 0x18)
            - ((const float*)((const tU8*)pIndices + 0xc))[pIndices[0]] * pResult[pIndices[0]]
            - ((const float*)((const tU8*)pIndices + 0xc))[pIndices[1]] * pResult[pIndices[1]])
        / ((const float*)((const tU8*)pIndices + 0xc))[pIndices[2]];
}

#pragma auto_inline(on)

#include <ctype.h>
#include "c2_stdlib.h"
#include "c2_string.h"

#include "c2_math.h"

extern int gCount_shrapnel_shower_calls;

void C2_HOOK_FASTCALL ClearCrushLists(void) {
    int i;

    C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(gCrush_lists) != 8);
    C2_HOOK_BUG_ON(sizeof(gCrush_lists[0]) != 0x158);

    for (i = 0; gCrush_lists[i].car_spec != NULL; i++) {
        gCrush_lists[i].car_spec = NULL;
    }
}

// FUNCTION: CARMA2_HW 0x0042a190
void C2_HOOK_FASTCALL ResetCrushSystems(void) {
    int i;

    C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(gTrack_crush_joints) != 32);

    for (i = 0; i < CARPOCALYPSE2_ASIZE(gTrack_crush_joints); i++) {
        gTrack_crush_joints[i]->type = eJoint_none;
    }
    gDetached_bit_crush_info_buffer.field_0x8 = 0;
    gSplit_car_crush_info_buffer.field_0x8 = 0;
    ClearCrushLists();
    gCount_queued_drone_crushes = 0;
    gCount_crush_detach_list = 0;
    gINT_0067bdfc = 0;
    gCount_toggled_doors = 0;
    gCrush_data_entry_counter = 0;
    gCount_net_crush_detach_list = 0;
    gCount_net_crush_semi_detach_bit_list = 0;
    gCount_net_crush_full_detach_bit_list = 0;
    gCount_net_crush_reattach_bit_list = 0;
    gCount_car_damage_crush_list = 0;
}

// C2PORT-NONMATCHING: best 76.53% | difftest PASS 2000 trials (spec difftest/specs/0042f170_FUN_0042f170.json) | reason register allocation
// FUNCTION: CARMA2_HW 0x0042f170
tCrush_info* C2_HOOK_FASTCALL FUN_0042f170(tCrush_info_buffer* pInfo_buffer, br_actor* pActor, tCar_spec* pCar) {
    int i;
    int count;
    int start_index;
    int j;
    int zero;
    tCrush_info* info;
    tCrush_info* candidate;

    count = pInfo_buffer->capacity;
    for (i = 0; i < count; i++) {
        if (pInfo_buffer->crush_infos[i].driver == 0) {
            break;
        }
    }
    if (i == count) {
        tPhysics_object* object;

        if (gCrush_deferred) {
            return NULL;
        }

        start_index = pInfo_buffer->field_0x8;
        for (i = 0; i < count; i++) {
            candidate = &pInfo_buffer->crush_infos[start_index];
            if (candidate->driver != 1) {
                break;
            }
            start_index = (start_index + 1) % count;
        }
        if (i == count) {
            return NULL;
        }
        info = &pInfo_buffer->crush_infos[start_index];
        pInfo_buffer->field_0x8 = (pInfo_buffer->field_0x8 + 1) % count;
        zero = 0;
        info->actor->type_data = (void*)zero;
        PHILSendDetachBit(info->car_ID, info->actor);
        FUN_004c86b0(0, info->actor);
        BrActorRemove(info->actor);

        object = info->collision_object->child;
        while (object != (tPhysics_object*)zero) {
            tPhysics_object* next;
            tCrush_info* child_info;
            br_actor* child_actor;
            tCar_spec* child_car;
            tCar_crush_spec* crush_spec;

            next = object->next;
            child_actor = object->actor;
            child_info = child_actor->type_data;
            child_actor->type_data = (void*)zero;

            child_car = *(tCar_spec**)(*(char**)(*(char**)((char*)child_actor->user + 8) + 0x34) + 0x2c);
            crush_spec = child_car->car_crush_spec;
            if (crush_spec->field_0x494 != 8) {
                FUN_004c86b0(0, child_actor);
                BrActorRemove(child_actor);
                crush_spec->field_0x498[crush_spec->field_0x494] = child_actor;
                crush_spec->field_0x494++;
            }
            child_info->driver = zero;
            PHILRemoveFromParentChain(object);
            object = next;
        }

        i = 0;
        if (gNum_active_non_cars > zero) {
            for (i = 0; i < gNum_active_non_cars; i++) {
                if ((void*)gActive_non_car_list[i] == info) {
                    break;
                }
            }
        }
        if (i != gNum_active_non_cars) {
            gNum_active_non_cars--;
            if (i < gNum_active_non_cars) {
                for (j = i; j < gNum_active_non_cars; j++) {
                    gActive_non_car_list[j] = gActive_non_car_list[j + 1];
                }
            }
        }

        info->driver = zero;
        info->actor->type_data = (void*)zero;
        PHILRemoveObject(info->collision_object);
    } else {
        info = &pInfo_buffer->crush_infos[i];
    }

    zero = 0;
    pActor->type_data = info;
    info->driver = *(tDriver*)((char*)pInfo_buffer + 0xc);
    info->collision_object->actor = pActor;
    info->actor = pActor;
    info->field_0x4 = zero;
    info->field_0xc4 = -1.0f;
    if (gNet_mode == eNet_mode_none) {
        if (gNext_crush_actor_id == 20000) {
            gNext_crush_actor_id = 10000;
        }
        info->car_ID = gNext_crush_actor_id++;
    } else {
        info->car_ID = pCar->car_ID;
    }
    info->car = pCar;
    info->collision_object->owner = info;
    info->collision_object->flags_0x238 = 1;
    info->collision_object->field_0x1a0 = 0xffff;
    info->collision_object->field_0x1a4 = zero;
    return info;
}

// FUNCTION: CARMA2_HW 0x004ef460
void C2_HOOK_FASTCALL LoadMinMax(FILE* pF, br_bounds3* pBounds) {
    float x1, x2;
    float y1, y2;
    float z1, z2;

    GetThreeFloats(pF, &x1, &y1, &z1);
    GetThreeFloats(pF, &x2, &y2, &z2);
    pBounds->min.v[0] = MIN(x1, x2);
    pBounds->min.v[1] = MIN(y1, y2);
    pBounds->min.v[2] = MIN(z1, z2);
    pBounds->max.v[0] = MAX(x1, x2);
    pBounds->max.v[1] = MAX(y1, y2);
    pBounds->max.v[2] = MAX(z1, z2);
}

void C2_HOOK_FASTCALL LoadNoncarActivation(FILE* pF, tNoncar_activation** pNoncar_activations, int* pCount_noncar_activations) {
    int i;
    int v1, v2;
    tNoncar_activation* activation;

    /* Number of non-cars activated */
    *pCount_noncar_activations = GetAnInt(pF);
    C2_HOOK_BUG_ON(sizeof(tNoncar_activation) != 64);
    (*pNoncar_activations) = BrMemAllocate(*pCount_noncar_activations * sizeof(tNoncar_activation), kMem_smash_side_effects);
    for (i = 0; i < *pCount_noncar_activations; i++) {
        activation = &(*pNoncar_activations)[i];
        GetPairOfInts(pF, &v1, &v2);
        activation->time_0x0 = 1000 * v1;
        activation->time_0x2 = 1000 * v2;
        activation->location_type = GetALineAndInterpretCommand(pF, gPosition_type_names, CARPOCALYPSE2_ASIZE(gPosition_type_names));
        activation->field_0x8 = GetAnInt(pF);
        LoadMinMax(pF, &activation->bounds);
        ReadSmashableInitialSpeed(pF, &activation->speed_info);
    }
}

void C2_HOOK_FASTCALL ReadAward(FILE* pF, tAward_info* pAward_info) {

    pAward_info->frequency = GetALineAndInterpretCommand(pF, gRepeatability_names, CARPOCALYPSE2_ASIZE(gRepeatability_names));
    if (pAward_info->frequency != kRepeatability_None) {
        pAward_info->field_0x4 = GetAScalar(pF);
        pAward_info->field_0x8 = GetAScalar(pF);
        pAward_info->field_0xc = GetAnInt(pF);
        pAward_info->field_0x10 = GetAnInt(pF);
    }
}

// FUNCTION: CARMA2_HW 0x004eea20
void C2_HOOK_FASTCALL LoadSmashableLevels(FILE* pF, tSmashable_level** pSmashable_levels, int* pCount_smashable_levels, int pIs_texture_change, tBrender_storage* pBrender_storage) {
    char s[256];
    int i;
    int j;
    int sound;
    int v1;
    int v2;
    tSmashable_level* level;

    /* pixelmap to use when intact */
    GetAString(pF, s);
    /* Number of levels */
    *pCount_smashable_levels = GetAnInt(pF) + 1;
    C2_HOOK_BUG_ON(sizeof(tSmashable_level) != 708);
    *pSmashable_levels = BrMemAllocate(*pCount_smashable_levels * sizeof(tSmashable_level), kMem_smash_levels);

    (*pSmashable_levels)[0].pixelmaps[0] = BrMapFind(s);
    if ((*pSmashable_levels)[0].pixelmaps[0] == NULL) {
        FatalError(kFatalError_CantLoadSmashPix_S, s);
    }
    (*pSmashable_levels)[0].count_shrapnels = 0;
    (*pSmashable_levels)[0].trigger_threshold = 0;
    for (i = 1; i < *pCount_smashable_levels; i++) {
        level = &(*pSmashable_levels)[i];
        /* trigger threshold (default if zero) */
        level->trigger_threshold = GetAScalar(pF);
        /* flags */
        level->flags = GetAnInt(pF);
        if (pIs_texture_change) {
            level->collision_type = GetALineAndInterpretCommand(pF, gSmashable_collision_type_names, CARPOCALYPSE2_ASIZE(gSmashable_collision_type_names));
        }
        /* number of possible sounds */
        level->count_sounds = GetAnInt(pF);
        for (j = 0; j < level->count_sounds; j++) {
            /* sound ID */
            sound = GetAnInt(pF);
            level->sounds[j] = LoadSingleSound(pBrender_storage, sound);
        }
        ReadShrapnelSpec(pF, level->shrapnels, &level->count_shrapnels);
        ReadExplosion(pF, &level->animation);
        ReadSpillData(pF, &level->slick);
        LoadNoncarActivation(pF, &level->noncar_activations, &level->count_noncar_activations);
        ReadShrapnelSideEffects(pF, &level->side_effects);
        /*Extensions flags */
        level->extension_flags = GetAnInt(pF);
        if (level->extension_flags & 0x1) {
            level->field_0x280 = GetAnInt(pF);
        }
        /* Room turn on code */
        level->room_turn_on_code = GetAnInt(pF);
        ReadAward(pF, &level->award);
        /* run-time variable changes */
        level->count_runtime_variable_changes = GetAnInt(pF);
        for (j = 0; j < level->count_runtime_variable_changes; j++) {
            GetPairOfInts(pF, &v1, &v2);
            level->runtime_variable_changes[j].field_0x0 = v2;
            level->runtime_variable_changes[j].field_0x2 = v1;
        }
        /* Number of pixelmaps */
        level->count_pixelmaps = GetAnInt(pF);
        for (j = 0; j < level->count_pixelmaps; j++) {
            GetAString(pF, s);
            level->pixelmaps[j] = BrMapFind(s);
            if (level->pixelmaps[j] == NULL) {
                FatalError(kFatalError_CantLoadSmashPix_S, s);
            }
        }
    }
}

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x004ee250
void C2_HOOK_FASTCALL FUN_004ee250(tCar_spec* pCar, tPhysics_object* pCollision_info, br_vector3* pPosition, br_vector3* pOptional_position, br_actor* pActor, undefined4 pArg6, tCar_crush_smashable_part* pSmashable, float pArg8, undefined4 pArg9) {
    gINT_006a3334 = 0;
    FUN_004ee290(pCar, pCollision_info, pPosition, pOptional_position, pActor, pArg6, pSmashable, pArg8, pArg9, 0);
}
#pragma auto_inline(on)

// FUNCTION: CARMA2_HW 0x004ee990
void C2_HOOK_FASTCALL LoadCarCrushSmashDataEntries(FILE* pF, tCar_crush_buffer_entry* pCar_crush_buffer_entry, tBrender_storage* pBrender_storage) {
    int i;

    /* Number of 'Smash data' entries */
    pCar_crush_buffer_entry->count_smashables = GetAnInt(pF);

    C2_HOOK_BUG_ON(sizeof(tCar_crush_smashable_part) != 124);
    pCar_crush_buffer_entry->smashables = BrMemAllocate(pCar_crush_buffer_entry->count_smashables * sizeof(tCar_crush_smashable_part), kMem_smashable_info);
    for (i = 0; i < pCar_crush_buffer_entry->count_smashables; i++) {
        /* name of material */
        GetAString(pF, pCar_crush_buffer_entry->smashables[i].material_name);
        pCar_crush_buffer_entry->smashables[i].funk = -1;
        LoadSmashableLevels(pF, &pCar_crush_buffer_entry->smashables[i].levels, &pCar_crush_buffer_entry->smashables[i].count_smashable_levels, 0, pBrender_storage);
    }
}

// FUNCTION: CARMA2_HW 0x0042a550
void C2_HOOK_FASTCALL LoadCarCrushDataEntry(FILE* pF, tCar_crush_buffer_entry* pCar_crush_buffer_entry,tBrender_storage *pBrender_storage) {
    int i;

    /* Actor */
    GetAString(pF, pCar_crush_buffer_entry->actor_name);
    if (strlen(pCar_crush_buffer_entry->actor_name) < 4) {
        PDFatalError("Less than 4 chars in car actor identifier");
    }
    StringTransformToLower(pCar_crush_buffer_entry->actor_name);
    /* Softness */
    switch (GetALineAndInterpretCommand(pF, gSoftness_names, CARPOCALYPSE2_ASIZE(gSoftness_names))) {
    case eSoftness_VerySoft:
        pCar_crush_buffer_entry->softness_factor = 4.f;
        break;
    case eSoftness_Soft:
        pCar_crush_buffer_entry->softness_factor = 2.f;
        break;
    default:
        pCar_crush_buffer_entry->softness_factor = 1.f;
        break;
    case eSoftness_Hard:
        pCar_crush_buffer_entry->softness_factor = 0.5f;
        break;
    case eSoftness_VeryHard:
        pCar_crush_buffer_entry->softness_factor = 0.25f;
        break;
    case eSoftness_Uncrushable:
        pCar_crush_buffer_entry->softness_factor = 0.f;
        break;
    }
    pCar_crush_buffer_entry->id = gCrush_data_entry_counter;
    gCrush_data_entry_counter++;

    /* Crush type */
    switch (GetALineAndInterpretCommand(pF, gCrush_type_names, CARPOCALYPSE2_ASIZE(gCrush_type_names))) {
    case eCrushType_Flap:
        C2_HOOK_BUG_ON(sizeof(tCar_crush_flap_data) != 44);
        pCar_crush_buffer_entry->flap_data = BrMemAllocate(sizeof(tCar_crush_flap_data), kMem_crush_data);
        /* Hinge point 0 */
        pCar_crush_buffer_entry->flap_data->hinge0 = GetAnInt(pF);
        /* Hinge point 1 */
        pCar_crush_buffer_entry->flap_data->hinge1 = GetAnInt(pF);
        /* Hinge point 2 */
        pCar_crush_buffer_entry->flap_data->hinge2 = GetAnInt(pF);
        pCar_crush_buffer_entry->flap_data->field_0x4 = 0;
        pCar_crush_buffer_entry->flap_data->field_0x2 = 0;
        pCar_crush_buffer_entry->flap_data->field_0x6 = 0x3ffc;
        pCar_crush_buffer_entry->flap_data->field_0x8 = 0x3ffc;
        /* Kev-o-flap? */
        pCar_crush_buffer_entry->flap_data->kev_o_flap = GetAnInt(pF);
        pCar_crush_buffer_entry->flap_data->field_0x14 = 1;
        pCar_crush_buffer_entry->flap_data->field_0x15 = 0;
        pCar_crush_buffer_entry->flap_data->field_0x18 = 0;
        pCar_crush_buffer_entry->flap_data->is_door = strstr(pCar_crush_buffer_entry->actor_name, "door") != NULL || strstr(pCar_crush_buffer_entry->actor_name, "dor") != NULL;
        C2_HOOK_BUG_ON(sizeof(tCar_crush_detach_data) != 136);
        pCar_crush_buffer_entry->detach_data = BrMemAllocate(sizeof(tCar_crush_detach_data), kMem_crush_data);
        pCar_crush_buffer_entry->detach_data->field_0x0 = 0.f;
        /* Ease of flap */
        switch (GetALineAndInterpretCommand(pF, gEase_of_detachment_names, CARPOCALYPSE2_ASIZE(gEase_of_detachment_names))) {
        case eEaseOfDetachment_VeryEasy:
            pCar_crush_buffer_entry->detach_data->force = gNormal_force_to_detach * 0.25f;
            break;
        case eEaseOfDetachment_Easy:
            pCar_crush_buffer_entry->detach_data->force = gNormal_force_to_detach * 0.5f;
            break;
        case eEaseOfDetachment_Normal:
        default:
            pCar_crush_buffer_entry->detach_data->force = gNormal_force_to_detach;
            break;
        case eEaseOfDetachment_Hard:
            pCar_crush_buffer_entry->detach_data->force = gNormal_force_to_detach * 2.f;
            break;
        case eEaseOfDetachment_VeryHard:
            pCar_crush_buffer_entry->detach_data->force = gNormal_force_to_detach * 4.f;
            break;
        }
        if (pCar_crush_buffer_entry->flap_data->kev_o_flap) {
            pCar_crush_buffer_entry->detach_data->type = eDetachType_kev_o_flap;
        } else {
            pCar_crush_buffer_entry->detach_data->type = eDetachType_fully_detachable;
        }
        /* shape */
        switch (GetALineAndInterpretCommand(pF, gCar_crush_shape_names, CARPOCALYPSE2_ASIZE(gCar_crush_shape_names))) {
        case eCarCrushShape_poly:
            pCar_crush_buffer_entry->detach_data->count_shape_points = GetAnInt(pF);
            for (i = 0; i < pCar_crush_buffer_entry->detach_data->count_shape_points; i++) {
                pCar_crush_buffer_entry->detach_data->shape_points[i] = GetAnInt(pF);
            }
            break;
        case eCarCrushShape_box:
        default:
            pCar_crush_buffer_entry->detach_data->count_shape_points = 0;
            break;
        }
        pCar_crush_buffer_entry->detach_data->field_0x28 = 0;
        break;
    case eCrushType_Detach:
        C2_HOOK_BUG_ON(sizeof(tCar_crush_detach_data) != 136);
        pCar_crush_buffer_entry->detach_data = BrMemAllocate(sizeof(tCar_crush_detach_data), kMem_crush_data);
        pCar_crush_buffer_entry->detach_data->field_0x0 = 0.f;
        /* Ease of detachment */
        switch (GetALineAndInterpretCommand(pF, gEase_of_detachment_names, CARPOCALYPSE2_ASIZE(gEase_of_detachment_names))) {
        case eEaseOfDetachment_VeryEasy:
            pCar_crush_buffer_entry->detach_data->force = gNormal_force_to_detach * .25f;
            break;
        case eEaseOfDetachment_Easy:
            pCar_crush_buffer_entry->detach_data->force = gNormal_force_to_detach * .5f;
            break;
        case eEaseOfDetachment_Normal:
        default:
            pCar_crush_buffer_entry->detach_data->force = gNormal_force_to_detach;
            break;
        case eEaseOfDetachment_Hard:
            pCar_crush_buffer_entry->detach_data->force = gNormal_force_to_detach * 2.f;
            break;
        case eEaseOfDetachment_VeryHard:
            pCar_crush_buffer_entry->detach_data->force = gNormal_force_to_detach * 4.f;
            break;
        }
        /* Type */
        pCar_crush_buffer_entry->detach_data->type = GetALineAndInterpretCommand(pF, gDetach_type_names, CARPOCALYPSE2_ASIZE(gDetach_type_names));
        if (pCar_crush_buffer_entry->detach_data->type == eDetachType_joint_index) {
            /* FIXME: not used in C2 game data */
            pCar_crush_buffer_entry->detach_data->field_0x30 = GetAnInt(pF);
        }
        switch (GetALineAndInterpretCommand(pF, gCar_crush_shape_names, CARPOCALYPSE2_ASIZE(gCar_crush_shape_names))) {
        case eCarCrushShape_poly:
            pCar_crush_buffer_entry->detach_data->count_shape_points = GetAnInt(pF);
            if (pCar_crush_buffer_entry->detach_data->count_shape_points > 16) {
                PDFatalError("Too many sample points in crush data");
            }
            for (i = 0; i < pCar_crush_buffer_entry->detach_data->count_shape_points; i++) {
                pCar_crush_buffer_entry->detach_data->shape_points[i] = GetAnInt(pF);
            }
            break;
        case eCarCrushShape_box:
        default:
            pCar_crush_buffer_entry->detach_data->count_shape_points = 0;
            break;
        }
        pCar_crush_buffer_entry->detach_data->field_0x28 = 0;
        break;
    }
    if (gNet_mode != eNet_mode_none && pCar_crush_buffer_entry->detach_data != NULL) {
        pCar_crush_buffer_entry->detach_data->count_shape_points = 0;
    }
    pCar_crush_buffer_entry->field_0x2c = 0;
    LoadCarCrushSmashDataEntries(pF, pCar_crush_buffer_entry, pBrender_storage);
}

// FUNCTION: CARMA2_HW 0x0042a280
int C2_HOOK_FASTCALL LoadCarCrush(tCar_crush_buffer* pCar_crush_buffer, const char* pPath, tBrender_storage* pBrender_storage, tCar_crush_spec** pCar_crush_spec) {
    int i;
    int j;
    int k;
    FILE* f;
    char s[256];
    int count;
    int version;
    tCar_crush_spec* car_crush;
    int version_le_100;

    C2_HOOK_BUG_ON(sizeof(tCar_crush_buffer) != 260);

    *pCar_crush_spec = NULL;
    pCar_crush_buffer->count_entries = 0;
    f = DRfopen(pPath, "rt");
    if (f == NULL) {
        return 1;
    }
    /* VERSION %d */
    GetALineAndDontArgue(f, s);
    count = sscanf(s, "VERSION %d", &version);
    if (count != 1) {
        FatalError(kFatalError_WrongCrushDataFileVersion_SDD, pPath, 0, 4);
    }
    version_le_100 = version <= 100;
    if (!version_le_100) {
        version -= 100;
    }
    if (version != 4) {
        FatalError(kFatalError_WrongCrushDataFileVersion_SDD, pPath, version, 4);
    }

    C2_HOOK_BUG_ON(sizeof(tCar_crush_spec) != 0x57c);
    car_crush = BrMemAllocate(sizeof(tCar_crush_spec), kMem_crush_data);
    *pCar_crush_spec = car_crush;

    /* Sub member: Master-crush data */
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 2; j++) {
            /* [0] Number of 'X/Y/Z mins' entries.
             * [1] Number of 'X/Y/Z maxs' entries. */
            count = GetAnInt(f);
            car_crush->count_limits.counts[i][j] = count;
            if (count == 0) {
                continue;
            }
            if (count > 2) {
                FatalError(kFatalError_TooManyCrushLimits);
            }
            for (k = 0; k < count; k++) {
                car_crush->limits.limits[i][j].values[k] = GetAScalar(f);
            }
        }
    }
    car_crush->field_0x134 = 0;
    car_crush->field_0x144 = 0;
    /* Bendability factor */
    car_crush->bendability_factor = GetAScalar(f);
    /* Bend point Z min */
    car_crush->bend_z_min = GetAScalar(f);
    /* Bend point Z max */
    car_crush->bend_z_max = GetAScalar(f);
    car_crush->field_0x270 = 0;
    car_crush->field_0x2b0 = 0;
    car_crush->field_0x4b8 = 0;
    /* Snappability factor */
    car_crush->snappability_factor = GetAScalar(f);
    /* Y split position */
    car_crush->split_y_pos = GetAScalar(f);
    /* Driver position */
    GetThreeFloats(f, &car_crush->driver_position.v[0], &car_crush->driver_position.v[1], &car_crush->driver_position.v[2]);
    car_crush->model_actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    car_crush->actor = BrActorAllocate(BR_ACTOR_NONE, NULL);
    BrActorAdd(car_crush->actor, car_crush->model_actor);
    car_crush->actor->identifier = BrResStrDup(car_crush->actor, "Sx?!");
    car_crush->version_le_100 = version_le_100;
    /* Number of 'Crush data' entries. */
    pCar_crush_buffer->count_entries = car_crush->nb_entries = GetAnInt(f);
    if (pCar_crush_buffer->count_entries > kMax_crush_datas) {
        FatalError(kFatalError_TooManyCrushDatas);
    }
    gCrush_data_entry_counter = 1;
    for (i = 0; i < pCar_crush_buffer->count_entries; i++) {
        pCar_crush_buffer->entries[i] = BrMemAllocate(sizeof(tCar_crush_buffer_entry), kMem_crush_data);
        LoadCarCrushDataEntry(f, pCar_crush_buffer->entries[i], pBrender_storage);
    }
    PFfclose(f);
    return 0;
}

// FUNCTION: CARMA2_HW 0x0048bf20
int C2_HOOK_CDECL LinkCrushData(br_actor* pActor, void* pData) {

    C2_HOOK_BUG_ON(sizeof(tUser_crush_data) != 16);
    C2_HOOK_BUG_ON(sizeof(tGroovidelic_spec) != 132);
    if (pActor->type == BR_ACTOR_MODEL) {
        pActor->user = BrMemAllocate(sizeof(tUser_crush_data), kMem_crush_data);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0042a940
void C2_HOOK_FASTCALL LinkCarCrushDatas(br_actor *pActor, tCar_crush_buffer *pBuffer) {

    DRActorEnumRecurse(pActor, LinkCarCrushData, pBuffer);
}

// FUNCTION: CARMA2_HW 0x0042a950
int C2_HOOK_CDECL LinkCarCrushData(br_actor* pActor, void* pData) {
    int i;
    tUser_crush_data* user_crush;
    tCar_crush_buffer* car_crush_buffer;

    car_crush_buffer = pData;
    user_crush = pActor->user;
    if (user_crush == NULL) {
        return 0;
    }
    user_crush->crush_data = NULL;
    for (i = 0; i < car_crush_buffer->count_entries; i++) {
        if (car_crush_buffer->entries[i] != NULL && DRStricmp(car_crush_buffer->entries[i]->actor_name, pActor->identifier) == 0) {
            user_crush->crush_data = car_crush_buffer->entries[i];
            return 0;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0048bf50
int C2_HOOK_CDECL AllocateUserDetailLevel(br_actor* pActor, void* pData) {
    tUser_crush_data* user_crush;
    tCar_spec* car_spec;
    tUser_detail_level_model* model_data;
    int i;

    C2_HOOK_BUG_ON(sizeof(tUser_detail_level_model) != 0xc);

    car_spec = pData;
    user_crush = pActor->user;
    if (user_crush != NULL) {
        for (i = 0; i < car_spec->count_detail_levels; i++) {
            if (user_crush->models[i] != NULL) {
                model_data = BrMemAllocate(sizeof(tUser_detail_level_model), kMem_crush_data);
                user_crush->models[i]->user = model_data;
            }
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0048bfa0
int C2_HOOK_CDECL LinkCrushModel(br_actor* pActor, void* pData) {
    tCrush_model_pool* pool = pData;
    tUser_crush_data* user_crush_data = pData;
    int len_actor_identifier;
    int len_model_identifier;
    int i;
    int crush_idx;
    const char *dot;

    if (pActor == NULL || pActor->identifier == NULL) {
        return 0;
    }
    user_crush_data = pActor->user;
    if (user_crush_data == NULL) {
        return 0;
    }

    dot = strrchr(pActor->identifier, '.');
    if (dot == NULL) {
        len_actor_identifier = strlen(pActor->identifier);
    } else {
        len_actor_identifier = dot - pActor->identifier;
    }
    for (i = 0; i < pool->model_count; i++) {
        br_model* model = pool->models[i];
        if (model == NULL || model->identifier == NULL) {
            continue;
        }
        dot = strrchr(model->identifier, '.');
        if (dot == NULL) {
            len_model_identifier = strlen(model->identifier);
        } else {
            len_model_identifier = dot - model->identifier;
        }
        if (len_actor_identifier != len_model_identifier || strncmp(model->identifier, pActor->identifier, len_actor_identifier) != 0) {
            continue;
        }
        if (dot != NULL && isdigit(dot[1])) {
            crush_idx = dot[1] - '0';
        } else {
            crush_idx  = -1;
        }
        if (crush_idx < 0) {
            for (crush_idx = 0; crush_idx < pool->count_detail_levels; crush_idx++) {
                if (user_crush_data->models[crush_idx] == NULL || user_crush_data->models[crush_idx]->nvertices < model->nvertices) {
                    break;
                }
            }
        }
        if (crush_idx < CARPOCALYPSE2_ASIZE(user_crush_data->models)) {
            if (user_crush_data->models[crush_idx] != NULL && crush_idx < CARPOCALYPSE2_ASIZE(user_crush_data->models) - 1) {
                memmove(&user_crush_data->models[crush_idx + 1],
                    &user_crush_data->models[crush_idx],
                    (CARPOCALYPSE2_ASIZE(user_crush_data->models) - crush_idx - 1) * sizeof(br_model*));
            }
            user_crush_data->models[crush_idx] = model;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0042ac70
intptr_t C2_HOOK_CDECL AccumulateSquashVertices(br_actor* actor, void* pData) {
    tAccumulateSquashVertices_UserData* context = pData;
    tUser_crush_data* crush_data = actor->user;

    if (crush_data != NULL && crush_data->crush_data != NULL) {
        int i;

        for (i = 0; i < CARPOCALYPSE2_ASIZE(crush_data->models); i++) {
            if (crush_data->models[i] != NULL) {
                context->count_vertices += crush_data->models[i]->nvertices;
            }
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0042acb0
int C2_HOOK_CDECL IncreasingCompare(const void* pValue1, const void* pValue2) {

    if (*(const float*)pValue1 < *(const float*)pValue2) {
        return -1;
    }
    if (*(const float*)pValue1 == *(const float*)pValue2) {
        return 0;
    }
    return 1;
}

// FUNCTION: CARMA2_HW 0x0042ace0
int C2_HOOK_CDECL DecreasingCompare(const void* pValue1, const void* pValue2) {

    if (*(const float*)pValue1 > *(const float*)pValue2) {
        return -1;
    }
    if (*(const float*)pValue1 == *(const float*)pValue2) {
        return 0;
    }
    return 1;
}

// FUNCTION: CARMA2_HW 0x0042b4c0
tU16 C2_HOOK_FASTCALL CrushLimitNumber(const br_vector3* pPoint, const tCar_crush_limits* pLimits, const tCar_crush_count_limits* pCount_limits, int* pInvalid) {
    int number;
    int i;
    int mi;
    int ma;

    *pInvalid = 1;
    number = 0;
    for (i = CARPOCALYPSE2_ASIZE(pPoint->v) - 1; i >= 0; i--) {
        for (mi = 0; mi < pCount_limits->counts[i][1] - 1; mi++) {
            if (pPoint->v[i] < pLimits->limits[i][1].values[mi]) {
                *pInvalid = 0;
                break;
            }
        }
        number = (number << 2) | mi;
        for (ma = 0; ma < pCount_limits->counts[i][0] - 1; ma++) {
            if (pLimits->limits[i][0].values[ma] < pPoint->v[i]) {
                *pInvalid = 0;
                break;
            }
        }
        number = (number << 2) | ma;
    }
    return (tU16)number;
}

void C2_HOOK_FASTCALL InitModelMasterCrushData(tCar_spec* pCar_spec) {
    tCar_crush_spec* car_crush;

    C2_HOOK_BUG_ON(sizeof(tCar_crush_vertex_data) != 0x6);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_crush_spec, field_0x1cc, 0x1cc);

    car_crush = pCar_spec->car_crush_spec;
    if (car_crush != NULL) {
        tAccumulateSquashVertices_UserData data;
        data.count_vertices = 0;
        data.bens_z_min = car_crush->bend_z_min;
        data.bens_z_max = car_crush->bend_z_max;
        DRActorEnumRecurse(pCar_spec->car_model_actor, AccumulateSquashVertices, &data);
        car_crush->field_0x1cc = BrMemAllocate(data.count_vertices * sizeof(tCar_crush_vertex_data), kMem_crush_data);
        BrMatrix34Copy(&car_crush->model_actor->t.t.mat ,&pCar_spec->car_model_actor->t.t.mat);
    }
}

void C2_HOOK_FASTCALL CheckHingePointOrder(tCar_crush_flap_data* pHinge, br_model* pModel, br_vector3* pPos) {
    const br_vertex* vertices = pModel->vertices;
    br_vector3 hinge01;
    br_vector3 hinge02;
    br_vector3 normal;
    br_vector3 center_offset;

    BrVector3Sub(&hinge01, &vertices[pHinge->hinge1].p, &vertices[pHinge->hinge0].p);
    BrVector3Sub(&hinge02, &vertices[pHinge->hinge2].p, &vertices[pHinge->hinge0].p);
    BrVector3Cross(&normal, &hinge02, &hinge01);
    Vector3Average(&center_offset, &pModel->bounds.min, &pModel->bounds.max);
    BrVector3Sub(&center_offset, &center_offset, pPos);
    if (BrVector3Dot(&center_offset, &normal) > 0.0f) {
        tU16 hinge1 = pHinge->hinge1;

        pHinge->hinge1 = pHinge->hinge0;
        pHinge->hinge0 = hinge1;
    }
}

// FUNCTION: CARMA2_HW 0x0042bb00
tU16 C2_HOOK_FASTCALL FindNearestVertex(br_vector3* pPoint, const br_model* pModel) {
    tU16 closest_vertex = 0;
    br_scalar closest_dist = BR_SCALAR_MAX;
    tU16 i;

    for (i = 0; i < pModel->nvertices; i++) {
        br_scalar d = Vector3DistanceSquared(pPoint, &pModel->vertices[i].p);
        if (d >= closest_dist) {
            goto next_vertex;
        }
        closest_dist = d;
        closest_vertex = i;
next_vertex:;
    }
    return closest_vertex;
}

tU16 C2_HOOK_FASTCALL FindNearestParentVertex(br_actor* pActor, br_vector3* pPos) {
    br_actor* parent = pActor->parent;
    tUser_crush_data* parent_user;
    br_model* parent_model = NULL;
    volatile br_vector3 point;

    if (parent != NULL) {
        parent_user = parent->user;
        if (parent_user != NULL) {
            parent_model = parent_user->models[0];
        }
    }
    if (parent_model == NULL) {
        PDFatalError("I can't cope flapping bit doesn't have a valid parent");
    }
    BrMatrix34ApplyP(&point, pPos, &pActor->t.t.mat);
    return FindNearestVertex(&point, parent_model);
}

// FUNCTION: CARMA2_HW 0x0042bb90
void C2_HOOK_FASTCALL SetUpSemiDetachJointStuff(tCar_crush_detach_data* pDetach_data, const br_model* pModel, const br_bounds3* pBounds) {
    const br_vector3* model_bounds[2];
    int closest_corner_index = 0;

    model_bounds[0] = &pModel->bounds.min;
    model_bounds[1] = &pModel->bounds.max;
    if (pDetach_data->type == eDetachType_joint_index) {
        float closest_corner_distance = BR_SCALAR_MAX;
        br_vector3 vp;
        int i;

        BrVector3Copy(&vp, &pModel->vertices[pDetach_data->field_0x30].p);
        for (i = 0; i < 8; i++) {
            br_vector3 corner;
            float dist_bb;

            BrVector3Set(&corner,
                model_bounds[i & 1]->v[0],
                model_bounds[(i >> 1) & 1]->v[1],
                model_bounds[(i >> 2) & 1]->v[2]);
            dist_bb = Vector3DistanceSquared(&corner, &vp);
            if (!(dist_bb >= closest_corner_distance)) {
                closest_corner_distance = dist_bb;
                closest_corner_index = i;
            }
        }
    } else {
        float closest_distance = BR_SCALAR_MAX;
        br_vector3 closest_corner;
        int i;

        BrVector3Copy(&closest_corner, &pModel->bounds.min);
        for (i = 0; i < 8; i++) {
            br_vector3 corner;
            float dist_closest_side;

            BrVector3Set(&corner,
                model_bounds[i & 1]->v[0],
                model_bounds[(i >> 1) & 1]->v[1],
                model_bounds[(i >> 2) & 1]->v[2]);
            dist_closest_side = DistanceToBounds(&corner, pBounds);
            if (!(dist_closest_side >= closest_distance)) {
                closest_distance = dist_closest_side;
                BrVector3Copy(&closest_corner, &corner);
                closest_corner_index = i;
            }
        }
        pDetach_data->field_0x30 = FindNearestVertex(&closest_corner, pModel);
    }
    pDetach_data->field_0x32 = (tU8)closest_corner_index;
    {
        br_vector3 center;
        const br_vector3* bounds_points[2];
        float closest = BR_SCALAR_MAX;
        int closest_side = 0;
        double d;
        int i;

        bounds_points[0] = &pBounds->min;
        bounds_points[1] = &pBounds->max;
        Vector3Average(&center, &pModel->bounds.min, &pModel->bounds.max);
        for (i = 0; i < 6; i++) {
            if (i != 2) {
                d = fabs((double)center.v[i / 2] - bounds_points[i & 1]->v[i / 2]);
                if (!(d >= closest)) {
                    closest = (float)d;
                    closest_side = i;
                }
            }
        }
        pDetach_data->field_0x33 = (tU8)closest_side;
    }
}

// FUNCTION: CARMA2_HW 0x0042b7f0
intptr_t C2_HOOK_CDECL InitPhysCrushDataCB(br_actor* actor, void* data) {
    tInitPhysCrushDataCB_Data* context = data;
    tUser_crush_data* user_crush_data;
    tCar_crush_buffer_entry* crush_data;
    br_model* model;
    tCar_crush_flap_data* flap_data;
    tCar_crush_detach_data* detach_data;
    double volume;

    user_crush_data = actor->user;
    if (user_crush_data == NULL) {
        return 0;
    }
    crush_data = user_crush_data->crush_data;
    if (crush_data == NULL) {
        return 0;
    }
    model = user_crush_data->models[0];
    if (model == NULL) {
        return 0;
    }
    volume = ((double)model->bounds.max.v[0] - model->bounds.min.v[0])
           * ((double)model->bounds.max.v[1] - model->bounds.min.v[1])
           * ((double)model->bounds.max.v[2] - model->bounds.min.v[2])
           * context->value;
    crush_data->field_0x24 = (float)volume;
    if (!(volume >= 0.01f)) {
        crush_data->field_0x24 = 0.01f;
    }
    flap_data = crush_data->flap_data;
    if (flap_data != NULL) {
        br_vector3 tv;
        const br_vertex* vertices;
        tCar_crush_flap_data* current_flap;
        tU16 parent_vertex;

        if ((flap_data->kev_o_flap & 0x2) == 0) {
            CheckHingePointOrder(flap_data, model, &context->car->collision_info->cmpos);
        }
        parent_vertex = FindNearestParentVertex(actor, &model->vertices[crush_data->flap_data->hinge0].p);
        crush_data->flap_data->field_0xc = parent_vertex;
        parent_vertex = FindNearestParentVertex(actor, &model->vertices[crush_data->flap_data->hinge2].p);
        crush_data->flap_data->field_0x12 = parent_vertex;
        vertices = model->vertices;
        BrVector3Sub(&tv, &vertices[flap_data->hinge2].p, &vertices[flap_data->hinge1].p);
        current_flap = crush_data->flap_data;
        current_flap->field_0x1c = BrVector3Length(&tv) * 0.5f;
    }
    detach_data = crush_data->detach_data;
    if (detach_data != NULL) {
        detach_data->field_0x2c = context->car;
        SetUpSemiDetachJointStuff(detach_data, model, &context->car->collision_info->bb1);
    }
    return 0;
}

void C2_HOOK_FASTCALL InitPhysCrushData(tCar_spec* pCar_spec) {
    tPhysics_object* collision_info = pCar_spec->collision_info;
    tInitPhysCrushDataCB_Data data;

    data.car = pCar_spec;
    data.value = collision_info->M /
                 ((collision_info->bb2.max.v[0] - collision_info->bb2.min.v[0]) *
                  (collision_info->bb2.max.v[1] - collision_info->bb2.min.v[1]) *
                  (collision_info->bb2.max.v[2] - collision_info->bb2.min.v[2]));
    DRActorEnumRecurse(pCar_spec->car_model_actor, InitPhysCrushDataCB, &data);
}

void C2_HOOK_FASTCALL CalculateReferencePoints(br_model* pModel, br_model* pParent_model, tCar_crush_detach_data* pDetach_data, br_actor* pActor) {
    int i;
    float d_min, d_max;
    br_vector3 min_v, max_v;

    if (pModel == NULL && pParent_model == NULL) {
        return;
    }
    d_max = d_min = BR_SCALAR_MAX;
    BrMatrix34ApplyP(&min_v, &pModel->bounds.min, &pActor->t.t.mat);
    BrMatrix34ApplyP(&max_v, &pModel->bounds.max, &pActor->t.t.mat);
    for (i = 0; i < pParent_model->nvertices; i++) {
        float d;

        d = Vector3DistanceSquared(&min_v, &pParent_model->vertices[i].p);
        if (!(d >= d_min)) {
            d_min = d;
            pDetach_data->min_vertex = (tU16)i;
        }
        d = Vector3DistanceSquared(&max_v, &pParent_model->vertices[i].p);
        if (!(d >= d_max)) {
            d_max = d;
            pDetach_data->max_vertex = (tU16)i;
        }
    }
}

// FUNCTION: CARMA2_HW 0x0042b590
intptr_t C2_HOOK_FASTCALL SoftnessOfNearestPointCB(br_actor* pActor, br_vector3* pPoint, void* pData) {
    tUser_crush_data* user_crush_data = pActor->user;
    tCar_crush_buffer_entry* crush_data;
    br_model* model;
    tSoftnessOfNearestPointCB_Context* context;
    const br_vector3* point;
    br_vector3 delta_point;
    float dist_bb_min;
    float dist_bb_max;
    float dist_bb_diagonal;
    float dist_p;
    int i;

    if (user_crush_data == NULL) {
        return 0;
    }
    crush_data = user_crush_data->crush_data;
    if (crush_data == NULL) {
        return 0;
    }
    model = user_crush_data->models[0];
    if (model == NULL) {
        return 0;
    }
    context = pData;
    point = context->point;
    BrVector3Sub(&delta_point, point, pPoint);
    dist_bb_diagonal = Vector3DistanceSquared(&model->bounds.max, &model->bounds.min);
    dist_bb_min = Vector3DistanceSquared(&delta_point, &model->bounds.min);
    dist_bb_max = Vector3DistanceSquared(&delta_point, &model->bounds.max);
    dist_p = dist_bb_min > dist_bb_max ? dist_bb_min : dist_bb_max;
    if (!((double)dist_p - dist_bb_diagonal > context->distance)) {
        for (i = 0; i < model->nvertices; i++) {
            float d;

            d = Vector3DistanceSquared(&model->vertices[i].p, &delta_point);
            if (!(d >= context->distance)) {
                context->distance = d;
                context->softness_factor = crush_data->softness_factor;
            }
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0042b760
intptr_t C2_HOOK_FASTCALL DRActorEnumRecurseWithTranslation(br_actor* pActor, br_vector3* pDelta, tDRActorEnumRecurseWithTranslation_cbfn* cbfn, void* pContext) {
    br_vector3 delta;
    intptr_t result;
    br_actor* a;

    if (pDelta == NULL) {
        BrVector3Set(&delta, 0.0f, 0.0f, 0.0f);
    } else {
        delta.v[0] = pActor->t.t.mat.m[3][0] + pDelta->v[0];
        delta.v[1] = pActor->t.t.mat.m[3][1] + pDelta->v[1];
        delta.v[2] = pActor->t.t.mat.m[3][2] + pDelta->v[2];
    }
    result = cbfn(pActor, &delta, pContext);
    if (result != 0) {
        return result;
    }
    for (a = pActor->children; a != NULL; a = a->next) {
        result = DRActorEnumRecurseWithTranslation(a, &delta, cbfn, pContext);
        if (result != 0) {
            return result;
        }
    }
    return 0;
}

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x005148c0
intptr_t C2_HOOK_FASTCALL DRActorEnumRecurseWithSnart(br_actor* pActor, const br_matrix34 *pMat, tDRActorEnumRecurseWithSnart_cbfn* pCallback, void* pContext) {
    br_actor* child;
    br_matrix34 mat;
    intptr_t result;

    if (pMat != NULL) {
        BrMatrix34Mul(&mat, &pActor->t.t.mat, pMat);
    } else {
        BrMatrix34Identity(&mat);
    }
    result = pCallback(pActor, &mat, pContext);
    if (result != 0) {
        return result;
    }
    for (child = pActor->children; child != NULL; child = child->next) {

        result = DRActorEnumRecurseWithSnart(child, &mat, pCallback, pContext);
        if (result != 0) {
            return result;
        }
    }
    return 0;
}
#pragma auto_inline(on)

float C2_HOOK_FASTCALL SoftnessOfNearestPoint(tCar_spec* pCar_spec, br_vector3* pPoint) {
    tSoftnessOfNearestPointCB_Context softcb_data;

    softcb_data.point = pPoint;
    softcb_data.distance = BR_SCALAR_MAX;
    softcb_data.softness_factor = 1.f;

    DRActorEnumRecurseWithTranslation(pCar_spec->car_model_actor, NULL, SoftnessOfNearestPointCB, &softcb_data);
    return softcb_data.softness_factor;
}

// FUNCTION: CARMA2_HW 0x0042b0e0
void C2_HOOK_FASTCALL SetUpShapeLimitingStuff(tCar_crush_spec* pCar_crush, tCar_spec* pCar_spec) {
    br_bounds3 wheel_all_bounds;
    const double inverse_world_scale = 1.0 / 6.9;
    int i;

    BrVector3Copy(&wheel_all_bounds.min, &pCar_spec->wpos[0]);
    BrVector3Copy(&wheel_all_bounds.max, &wheel_all_bounds.min);
    for (i = 1; i < 4; i++) {
        int j;

        for (j = 0; j < 3; j++) {
            if (!(pCar_spec->wpos[i].v[j] >= wheel_all_bounds.min.v[j])) {
                wheel_all_bounds.min.v[j] = pCar_spec->wpos[i].v[j];
            } else if (pCar_spec->wpos[i].v[j] > wheel_all_bounds.max.v[j]) {
                wheel_all_bounds.max.v[j] = pCar_spec->wpos[i].v[j];
            }
        }
    }
    for (i = 0; i < 3; i++) {
        wheel_all_bounds.min.v[i] = (float)(wheel_all_bounds.min.v[i] * inverse_world_scale);
        wheel_all_bounds.max.v[i] = (float)(wheel_all_bounds.max.v[i] * inverse_world_scale);
    }
    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(pCar_spec->wheel_actors); i++) {
        br_actor* wheel_actor = pCar_spec->wheel_actors[i];
        tUser_crush_data* user_crush_data;
        br_model* wheel_model;
        br_matrix34 wheel_to_car;
        br_bounds3 wheel_bounds;
        int j;

        if (wheel_actor == NULL) {
            continue;
        }
        user_crush_data = wheel_actor->user;
        if (user_crush_data == NULL) {
            continue;
        }
        wheel_model = user_crush_data->models[0];
        if (wheel_model == NULL) {
            continue;
        }
        BrActorToActorMatrix34(&wheel_to_car, wheel_actor, pCar_spec->car_model_actor);
        for (j = 0; j < 3; j++) {
            wheel_bounds.min.v[j] = wheel_model->bounds.min.v[j] + wheel_to_car.m[3][j];
            wheel_bounds.max.v[j] = wheel_model->bounds.max.v[j] + wheel_to_car.m[3][j];
        }
        for (j = 0; j < 3; j++) {
            if (!(wheel_bounds.min.v[j] >= wheel_all_bounds.min.v[j])) {
                wheel_all_bounds.min.v[j] = wheel_bounds.min.v[j];
            } else if (wheel_bounds.max.v[j] > wheel_all_bounds.max.v[j]) {
                wheel_all_bounds.max.v[j] = wheel_bounds.max.v[j];
            }
        }
    }
    for (i = 0; i < 3; i++) {
        int j;

        for (j = 0; j < pCar_crush->count_limits.counts[i][0]; j++) {
            pCar_crush->field_0xbc.limits[i][0].values[j] = pCar_crush->limits.limits[i][0].values[j];
        }
        pCar_crush->field_0xbc.limits[i][0].values[j] = wheel_all_bounds.max.v[i];
        pCar_crush->field_0xa4.counts[i][0] = j + 1;
        for (j = 0; j < pCar_crush->count_limits.counts[i][1]; j++) {
            pCar_crush->field_0xbc.limits[i][1].values[j] = pCar_crush->limits.limits[i][1].values[j];
        }
        pCar_crush->field_0xbc.limits[i][1].values[j] = wheel_all_bounds.min.v[i];
        pCar_crush->field_0xa4.counts[i][1] = j + 1;
    }
    for (i = 0; i < 3; i++) {
        qsort(pCar_crush->field_0xbc.limits[i][0].values, pCar_crush->field_0xa4.counts[i][0], sizeof(float), DecreasingCompare);
        qsort(pCar_crush->field_0xbc.limits[i][1].values, pCar_crush->field_0xa4.counts[i][1], sizeof(float), IncreasingCompare);
    }
    for (i = 0; i < pCar_crush->count_shapes; i++) {
        int j;
        tCar_crush_shape_info* shape_info = &pCar_crush->field_0x4[i];

        for (j = 0; j < shape_info->count_points; j++) {
            int failed;
            tU16 number;
            float softness;

            number = CrushLimitNumber(&shape_info->field_0x18[j].field_0x0, &pCar_crush->field_0xbc, &pCar_crush->field_0xa4, &failed);
            shape_info->field_0x18[j].field_0x28 = number;
            if (failed == 0) {
                softness = SoftnessOfNearestPoint(pCar_spec, &shape_info->field_0x18[j].field_0x0);
                shape_info->field_0x18[j].field_0x24 = softness;
            } else {
                shape_info->field_0x18[j].field_0x24 = 0.0f;
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x0042be20
intptr_t C2_HOOK_CDECL InitModelCrushDataCB(br_actor* actor, void* data) {
    tUser_crush_data* user_crush_data;
    tCar_crush_detach_data* detach_data;

    user_crush_data = actor->user;
    if (user_crush_data != NULL && user_crush_data->models[0] != NULL
            && user_crush_data->crush_data != NULL && (detach_data = user_crush_data->crush_data->detach_data) != NULL) {
        tUser_crush_data* parent_user_crush_data;
        br_model* model;
        br_model* parent_model;
        tU16 i;
        float d_min, d_max;
        br_vector3 min_v, max_v;

        model = user_crush_data->models[0];
        parent_user_crush_data = actor->parent->user;
        parent_model = parent_user_crush_data->models[0];
        d_max = d_min = BR_SCALAR_MAX;
        BrMatrix34ApplyP(&min_v, &model->bounds.min, &actor->t.t.mat);
        BrMatrix34ApplyP(&max_v, &model->bounds.max, &actor->t.t.mat);

        for (i = 0; i < parent_model->nvertices; i++) {
            float d;
            br_vector3 delta;

            BrVector3Sub(&delta, &min_v, &parent_model->vertices[i].p);
            d = BrVector3LengthSquared(&delta);
            if (d < d_min) {
                detach_data->min_vertex = i;
                d_min = d;
            }
            BrVector3Sub(&delta, &max_v, &parent_model->vertices[i].p);
            d = BrVector3LengthSquared(&delta);
            if (d < d_max) {
                detach_data->max_vertex = i;
                d_max = d;
            }
        }
    }
    return 0;
}

// InitModelCrushData

// FUNCTION: CARMA2_HW 0x0043b740
void C2_HOOK_FASTCALL SmoothCarCrushShapePoints(tCar_spec* pCar, float pStep) {
    tCar_crush_spec* pCrush;
    tCar_crush_shape_info* pShape;
    br_vector3* pCurrent_vector;
    float* pCurrent;
    float value;
    int shape_index;
    int shape_offset;
    int point_index;
    int point_offset;
    int axis_count;
    int point_data;
    int target_offset;

    pCrush = pCar->car_crush_spec;
    shape_index = 0;
    point_offset = 0;
    if (pCrush->count_shapes > 0) {
        shape_offset = 0;
        do {
            pShape = (tCar_crush_shape_info*)((tU8*)pCrush->field_0x4 + shape_offset);
            point_index = 0;
            if (pShape->count_points > 0) {
                do {
                    point_data = (int)pShape->field_0x18 + point_offset;
                    pCurrent_vector = (br_vector3*)(point_data + 0x18);
                    pCurrent = pCurrent_vector->v;
                    target_offset = point_data - (int)pCurrent;
                    axis_count = 3;
                    do {
                        if (*pCurrent > *(float*)(target_offset + (int)pCurrent)) {
                            value = *pCurrent - pStep;
                            *pCurrent = value;
                            if (!(value < *(float*)(target_offset + (int)pCurrent))) {
                                goto next_axis;
                            }
                            goto clamp_to_target;
                        } else if (*pCurrent < *(float*)(target_offset + (int)pCurrent)) {
                            value = *pCurrent;
                            *pCurrent = value + pStep;
                            if (*(float*)(target_offset + (int)pCurrent) < value + pStep) {
                                goto clamp_to_target;
                            }
                        }
                        goto next_axis;
clamp_to_target:
                        *pCurrent = *(float*)(target_offset + (int)pCurrent);
next_axis:
                        pCurrent++;
                        axis_count--;
                    } while (axis_count != 0);
                    if (DRVector3TestForNan(pCurrent_vector)) {
                        PDEnterDebugger("NaN");
                    }
                    point_index++;
                    point_offset += sizeof(tCar_crush_reordered_shape_info);
                } while (point_index < pShape->count_points);
            }
            point_offset = 0;
            pCrush = pCar->car_crush_spec;
            shape_index++;
            shape_offset += sizeof(tCar_crush_shape_info);
        } while (shape_index < pCrush->count_shapes);
    }
    pCrush = pCar->car_crush_spec;
    pCrush->expand_bounding_box = 3;
}

void C2_HOOK_FASTCALL InitModelCrushData(tCar_spec* pCar_spec) {
    DRActorEnumRecurse(pCar_spec->car_model_actor, InitModelCrushDataCB, pCar_spec);
}

void C2_HOOK_FASTCALL CopyShapePolyhedron(tCollision_shape_polyhedron* pDest, const tCollision_shape_polyhedron* pSrc) {
    int i;

    pDest->common.type = kCollisionShapeType_Polyhedron;
    pDest->polyhedron.count_points = pSrc->polyhedron.count_points;
    for (i = 0; i < pSrc->polyhedron.count_points; i++) {
        BrVector3Copy(&pDest->polyhedron.points[i], &pSrc->polyhedron.points[i]);
    }
    FillInShape((tPhysics_shape*)pDest);
}

// FUNCTION: CARMA2_HW 0x0042a9b0
void C2_HOOK_FASTCALL InitNetworkShapesStuff(tCar_crush_spec* pCar_crush) {
    int i;

    C2_HOOK_BUG_ON(sizeof(tCar_crush_network_shapes) != 0x60);

    pCar_crush->field_0x24 = 0;
    pCar_crush->field_0x26 = 0;

    if (gNet_mode == eNet_mode_host) {
        pCar_crush->network_stuff = BrMemAllocate(sizeof(tCar_crush_network_shapes), kMem_crush_data);
        for (i = 1; i < gNumber_of_net_players; i++) {
            pCar_crush->network_stuff->items[i - 1].network_id = gNet_players[i].ID;
            pCar_crush->network_stuff->items[i - 1].field_0x4 = 0;
        }
    }
}

// C2PORT-NONMATCHING: best 55.22% | difftest PASS 2000 trials (spec difftest/specs/00434f60_FUN_00434f60.json) | reason VC5 x87 scheduling and stack layout
// FUNCTION: CARMA2_HW 0x00434f60
void C2_HOOK_FASTCALL FUN_00434f60(br_vector3* pVec, void* pArg2) {
    tU8* curve;
    int mode;
    float z_delta;
    float x_norm;
    float y_norm;
    float z_norm;
    float cross_norm;
    float x_correction;
    float y_correction;
    float z_correction;
    br_vector3 transformed;

    curve = *(tU8**)((tU8*)pArg2 + 0x34);
    mode = *(int*)(curve + 0x74);
    z_delta = pVec->v[2] - *(float*)(curve + 0x20);
    if ((mode != 0 && z_delta <= gCrushZero) || (mode == 0 && z_delta >= gCrushZero)) {
        BrMatrix34ApplyP(&transformed, pVec, (br_matrix34*)((tU8*)pArg2 + 4));
        pVec->v[0] = transformed.v[0];
        pVec->v[1] = transformed.v[1];
        pVec->v[2] = transformed.v[2];
        return;
    }
    z_delta = pVec->v[2] - *(float*)(curve + 0x2c);
    if (!((mode != 0 && z_delta <= gCrushZero) || (mode == 0 && z_delta >= gCrushZero))) {
        return;
    }
    x_norm = (pVec->v[0] - *(float*)(curve + 0x24)) / *(float*)(curve + 0x30);
    y_norm = (pVec->v[1] - *(float*)(curve + 0x28)) / *(float*)(curve + 0x34);
    z_norm = z_delta / *(float*)(curve + 0x38);
    cross_norm = y_norm * x_norm;
    x_correction = *(float*)(curve + 0x44) * cross_norm;
    x_correction += *(float*)(curve + 0x50) * x_norm;
    x_correction += *(float*)(curve + 0x5c) * y_norm;
    x_correction += *(float*)(curve + 0x68);
    y_correction = *(float*)(curve + 0x48) * cross_norm;
    y_correction += *(float*)(curve + 0x54) * x_norm;
    y_correction += *(float*)(curve + 0x60) * y_norm;
    y_correction += *(float*)(curve + 0x6c);
    z_correction = *(float*)(curve + 0x4c) * cross_norm;
    z_correction += *(float*)(curve + 0x58) * x_norm;
    z_correction += *(float*)(curve + 0x64) * y_norm;
    z_correction += *(float*)(curve + 0x70);
    pVec->v[0] += x_correction * z_norm;
    pVec->v[1] += y_correction * z_norm;
    pVec->v[2] += z_correction * z_norm;
}


void C2_HOOK_FASTCALL InitShapeStuff(tCar_crush_spec* pCar_crush, tPhysics_object* pCollision_info, tCar_spec* pCar_spec) {
    tPhysics_shape* shape;
    int i;

    C2_HOOK_BUG_ON(sizeof(tCar_crush_shape_info) != 0x1c);
    C2_HOOK_BUG_ON(sizeof(tCar_crush_reordered_shape_info) != 0x38);

    pCar_crush->count_shapes = 0;
    for (shape = pCollision_info->shape; shape != NULL; shape = shape->common.next) {
        pCar_crush->count_shapes += 1;
    }

    pCar_crush->field_0x4 = BrMemAllocate(pCar_crush->count_shapes * sizeof(tCar_crush_shape_info), kMem_crush_data);
    for (i = 0, shape = pCollision_info->shape; shape != NULL; shape = shape->common.next, i++) {
        int j;
        tCar_crush_shape_info* crush_shape_info = &pCar_crush->field_0x4[i];

        crush_shape_info->count_points = shape->polyhedron.polyhedron.count_points;
        crush_shape_info->count_points_add_8 = crush_shape_info->count_points + 8;
        crush_shape_info->field_0x0 = AllocateShapePolyhedron(crush_shape_info->count_points_add_8, kMem_crush_data);
        CopyShapePolyhedron(crush_shape_info->field_0x0, (tCollision_shape_polyhedron*)shape);
        crush_shape_info->field_0x4 = AllocateShapePolyhedron(crush_shape_info->count_points_add_8, kMem_crush_data);
        crush_shape_info->field_0x18 = BrMemAllocate(crush_shape_info->count_points_add_8 * sizeof(tCar_crush_reordered_shape_info), kMem_crush_data);
        for (j = 0; j < crush_shape_info->count_points; j++) {
            BrVector3Copy(&crush_shape_info->field_0x18[j].field_0x0, &shape->polyhedron.polyhedron.points[j]);
            BrVector3Copy(&crush_shape_info->field_0x18[j].field_0xc, &crush_shape_info->field_0x18[j].field_0x0);
            BrVector3Copy(&crush_shape_info->field_0x18[j].field_0x18, &crush_shape_info->field_0x18[j].field_0x0);
        }
        crush_shape_info->field_0x8 = 0;
    }
    pCar_crush->field_0xc = pCollision_info->shape;
    pCollision_info->shape = (tPhysics_shape*)pCar_crush->field_0x4->field_0x0;
    for (i = 0; i < pCar_crush->count_shapes - 1; i++) {
        pCar_crush->field_0x4[i].field_0x0->common.next = (tPhysics_shape*)pCar_crush->field_0x4[i + 1].field_0x0;
    }
    pCar_crush->field_0x4[pCar_crush->count_shapes - 1].field_0x0->common.next = NULL;

    pCar_crush->expand_bounding_box = 0;
    SetUpShapeLimitingStuff(pCar_crush, pCar_spec);
    if (gNet_mode != eNet_mode_none) {
        InitNetworkShapesStuff(pCar_crush);
    }
}

// FUNCTION: CARMA2_HW 0x0042ad10
void C2_HOOK_FASTCALL InitPhysMasterCrushData(tCar_spec* pCar_spec) {
    tCar_crush_spec* car_crush;
    tPhysics_object* collision_info;
    int i;

    car_crush = pCar_spec->car_crush_spec;
    collision_info = pCar_spec->collision_info;
    if (car_crush == NULL || collision_info == NULL) {
        return;
    }
    for (i = 0; i < 3; i++) {
        car_crush->limits.limits[i][0].values[car_crush->count_limits.counts[i][0]] = collision_info->bb1.min.v[i] - 0.02898551f;
        car_crush->limits.limits[i][1].values[car_crush->count_limits.counts[i][1]] = collision_info->bb1.max.v[i] + 0.02898551f;
        car_crush->count_limits.counts[i][0] += 1;
        car_crush->count_limits.counts[i][1] += 1;
    }
    InitShapeStuff(car_crush, collision_info, pCar_spec);
    for (i = 0; i < 4; i++) {
        car_crush->field_0x564[i] = AllocateWireFrameCollisionShape(2, 1, kMem_crush_data);
        BrVector3InvScale(&car_crush->field_0x564[i]->wireframe.points[0], &pCar_spec->wpos[i], WORLD_SCALE);
        BrVector3InvScale(&car_crush->field_0x564[i]->wireframe.points[1], &pCar_spec->wpos[i], WORLD_SCALE);
        car_crush->field_0x564[i]->wireframe.points[1].v[1] = 0.f;
        car_crush->field_0x564[i]->wireframe.lines[0].index1 = 0;
        car_crush->field_0x564[i]->wireframe.lines[0].index2 = 1;

        FillInShape((tPhysics_shape*)car_crush->field_0x564[i]);
    }
    car_crush->actor->user = pCar_spec;
    car_crush->field_0x574 = 0;
    for (i = 0; ; i++) {
        int c = tolower(pCar_spec->name[i]);
        if (c == 0) {
            break;
        }
        car_crush->field_0x574 *= 2;
        car_crush->field_0x574 += c;
        car_crush->field_0x574 &= 0xffff;
    }
}

// FUNCTION: CARMA2_HW 0x0042c300
float C2_HOOK_FASTCALL PointEdgeDistSq(const br_vector3* pP, const br_vector3* pA, const br_vector3* pB) {
    br_vector3 pAB;
    br_vector3 pAP;
    br_vector3 pProj;
    float proj;

    BrVector3Sub(&pAB, pB, pA);
    BrVector3Sub(&pAP, pP, pA);
    proj = BrVector3Dot(&pAP, &pAB) / BrVector3Dot(&pAB, &pAB);
    if (!(proj >= 0.0f)) {
        proj = 0.0f;
    } else if (proj > 1.0f) {
        proj = 1.0f;
    }
    BrVector3Scale(&pProj, &pAB, proj);
    BrVector3Add(&pProj, &pProj, pA);
    BrVector3Sub(&pProj, pP, &pProj);
    return BrVector3LengthSquared(&pProj);
}

// FUNCTION: CARMA2_HW 0x0042c0e0
void C2_HOOK_FASTCALL SetFlapCheckVertices(tCar_crush_flap_data* pFlap_data, const br_model* pModel, tModel_detail_vertex_data* pVertex_data) {
    const br_vector3 *p0, *p1, *p2;
    br_vector3 hinge01;
    br_vector3 hinge02;
    br_vector3 hinge_normal;
    br_scalar hinge_dot;
    br_scalar distance;
    br_scalar plane;
    int i;

    p0 = &pModel->vertices[pFlap_data->hinge0].p;
    p1 = &pModel->vertices[pFlap_data->hinge1].p;
    p2 = &pModel->vertices[pFlap_data->hinge2].p;
    BrVector3Sub(&hinge02, p2, p0);
    BrVector3Sub(&hinge01, p1, p0);
    BrVector3Cross(&hinge_normal, &hinge01, &hinge02);
    BrVector3Normalise(&hinge_normal, &hinge_normal);
    hinge_dot = BrVector3Dot(&hinge_normal, p0);

    for (i = 0; i < pModel->nvertices; i++) {
        distance = PointEdgeDistSq(&pModel->vertices[i].p, &pModel->vertices[pFlap_data->hinge0].p, &pModel->vertices[pFlap_data->hinge1].p);
        plane = BrVector3Dot(&hinge_normal, &pModel->vertices[i].p) - hinge_dot;
        if (distance > 0.00021003991f && plane > -0.0014492754f) {
            pVertex_data[i].flags |= 0x2;
        } else {
            pVertex_data[i].flags &= ~0x2;
        }
    }
}

// C2PORT-NONMATCHING: best 66.85% | difftest PASS 2000 trials (spec difftest/specs/0043d340_ReceivedDetachedNonCarData.json) | reason VC5 register allocation and loop codegen
// FUNCTION: CARMA2_HW 0x0043d340
void C2_HOOK_FASTCALL ReceivedDetachedNonCarData(tU8* pMessage) {
    register tU8* message;
    register tCar_spec* car;
    tCar_spec* volatile saved_car;
    register tCar_crush_spec* crush;
    register tPhysics_object* object;
    register br_actor* actor;
    tFind_crush_actor_arg find_arg;
    br_bounds3 bounds;
    tCar_crush_detach_data* detach_data;
    tUser_crush_data* user_crush_data;
    tNon_car_spec* crush_info;
    register tPhysics_object* collision_info;
    int i;
    int count;
    int changed;
    int hierarchy_size;
    tCar_bit_spec* bit;

    message = pMessage;
    car = NetCarFromPlayerID(*(tPlayer_ID*)(message + 4));
    saved_car = car;
    if (car == NULL) {
        return;
    }

    crush = car->car_crush_spec;
    object = NULL;
    if (crush != NULL) {
        count = crush->field_0x2b0;
        i = 0;
        bit = &crush->field_0x2b4[0];
        if (count > 0) {
            do {
                if (bit->field_0x8->uid == message[8]) {
                    object = *(tPhysics_object**)&bit->field_0x0;
                    goto selected_bit;
                }
                i++;
                bit++;
            } while (i < count);
        }
    }
selected_bit:
    if (object == NULL && crush != NULL) {
        count = crush->field_0x270;
        i = 0;
        bit = &crush->field_0x274[0];
        if (count > 0) {
            do {
                if (bit->field_0x8->uid == message[8]) {
                    object = *(tPhysics_object**)&bit->field_0x0;
                    break;
                }
                i++;
                bit++;
            } while (i < count);
        }
    }

    if (object != NULL) {
        FullyDetachBit(saved_car, object);
        MungeDetachLists(saved_car->car_crush_spec);
        actor = object->actor;
    } else {
        find_arg.id = message[8];
        find_arg.actor = NULL;
        if (!DRActorEnumRecurse(saved_car->car_model_actor, FindActorByCrushEntryID, &find_arg)) {
            return;
        }
        actor = find_arg.actor;
        ExpandVector3(&bounds.min, (tCompressed_vector3*)(message + 0xa), -10.f, 10.f);
        ExpandVector3(&bounds.max, (tCompressed_vector3*)(message + 0x10), -10.f, 10.f);
        if (DetachBit(saved_car, actor, &bounds) == NULL) {
            return;
        }
    }

    crush_info = actor->type_data;
    if (crush_info == NULL) {
        return;
    }
    collision_info = crush_info->collision_info;
    user_crush_data = actor->user;
    if (user_crush_data == NULL || user_crush_data->crush_data == NULL) {
        return;
    }
    detach_data = user_crush_data->crush_data->detach_data;
    if (detach_data == NULL) {
        return;
    }

    changed = 0;
    for (i = 0; i < 3; i++) {
        if (((tU16*)(message + 0xa))[i] != detach_data->field_0x7c.v[i]) {
            collision_info->shape->common.bb.min.v[i] = DRU16ToScalar(((tU16*)(message + 0xa))[i], -10.f, 10.f);
            changed = 1;
        }
        if (((tU16*)(message + 0x10))[i] != detach_data->field_0x82.v[i]) {
            collision_info->shape->common.bb.max.v[i] = DRU16ToScalar(((tU16*)(message + 0x10))[i], -10.f, 10.f);
            changed = 1;
        }
    }
    if (changed) {
        UpdateCollisionObject(collision_info);
    }

    if ((message[0x20] & 2) == 0 && *(int*)(message + 0x18) != -1) {
        hierarchy_size = GetNetworkDataSize(message + 0x22);
        memcpy((tU8*)crush_info->collision_info + 0x294, message + 0x22, hierarchy_size);
        crush_info->collision_info->message_time = *(tU32*)(message + 0x18);
        crush_info->collision_info->field_0x261 = 0x27;
        crush_info->collision_info->disable_move_rotate = 0;
        return;
    }

    SelectCrushNetwork((undefined4*)(message + 0x22), crush_info->collision_info, 2);
    crush_info->collision_info->disable_move_rotate = 1;
}


void C2_HOOK_FASTCALL InitModelVertexData(tModel_detail_vertex_data* pVertex_data, br_model* pModel, int pDetail_level, tCar_crush_buffer_entry* pCrush_data, tCar_crush_spec* pCar_crush_spec) {
    int i;

    for (i = 0; i < pModel->nvertices; i++) {
        int invalid;
        tU16 number;

        CompressVector3(&pVertex_data[i].p, &pModel->vertices[i].p, -10.0f, 10.0f);
        number = CrushLimitNumber(&pModel->vertices[i].p, &pCar_crush_spec->limits, &pCar_crush_spec->count_limits, &invalid);
        pVertex_data[i].limit_number = (tS16)number;
    }
    if (pDetail_level == 0 && pCrush_data != NULL && pCrush_data->flap_data != NULL) {
        SetFlapCheckVertices(pCrush_data->flap_data, pModel, pVertex_data);
    }
}

// FUNCTION: CARMA2_HW 0x0042bf80
intptr_t C2_HOOK_CDECL InitVertexDataCB(br_actor* pActor, void* data) {
    tUser_crush_data* user_crush_data;
    tCar_spec* car_spec;
    int i;

    C2_HOOK_BUG_ON(sizeof(tModel_detail_vertex_data) != 10);

    user_crush_data = pActor->user;
    car_spec = data;
    if (user_crush_data == NULL || user_crush_data->crush_data == NULL || car_spec->car_crush_spec == NULL) {
        return 0;
    }
    for (i = 0; i < car_spec->count_detail_levels; i++) {
        br_model* model = user_crush_data->models[i];
        if (model != NULL && model->nvertices != 0 && model->user != NULL) {
            tUser_detail_level_model* model_user_data = model->user;
            model_user_data->field_0x4 = BrMemAllocate(model->nvertices * sizeof(tModel_detail_vertex_data), kMem_crush_data);
            InitModelVertexData(model_user_data->field_0x4, model, i, user_crush_data->crush_data, car_spec->car_crush_spec);
            if (i == 0) {
                LinkSmashies(pActor, user_crush_data->crush_data, model_user_data->field_0x4);
            }
        }
    }
    return 0;
}

void C2_HOOK_FASTCALL InitVertexData(tCar_spec* pCar_spec) {
    tCar_crush_spec* car_crush_spec;
    int i;

    car_crush_spec = pCar_spec->car_crush_spec;
    if (car_crush_spec == NULL) {
        return;
    }
    for (i = 0; i < 3; i++) {
        qsort(car_crush_spec->limits.limits[i][0].values, car_crush_spec->count_limits.counts[i][0], sizeof(float), DecreasingCompare);
        qsort(car_crush_spec->limits.limits[i][1].values, car_crush_spec->count_limits.counts[i][1], sizeof(float), IncreasingCompare);
    }
    DRActorEnumRecurse(pCar_spec->car_model_actor, InitVertexDataCB, pCar_spec);
}

void C2_HOOK_FASTCALL CheckWheelPositions(const tCar_spec* pCar_spec) {
    int wheel_i;

    for (wheel_i = 0; wheel_i < 4; wheel_i++) {
        const br_vector3* wpos = &pCar_spec->wpos[wheel_i];
        const tPhysics_shape* shape;
        int wheel_outside_all_shapes = 1;

        for (shape = pCar_spec->collision_info->shape; shape != NULL; shape = shape->common.next) {
            int wheel_outside_shape = 1;
            if (shape->common.type == kCollisionShapeType_Polyhedron) {
                int plane_i;

                for (plane_i = 0; plane_i < shape->polyhedron.polyhedron.count_planes; plane_i++) {
                    const br_vector4* plane = &shape->polyhedron.polyhedron.planes[plane_i];
                    if (BrVector3Dot(plane, wpos) / WORLD_SCALE + plane->v[3] < 0.f) {
                        wheel_outside_shape = 0;
                        break;
                    }
                }
            }
            if (wheel_outside_shape) {
                wheel_outside_all_shapes = 0;
                break;
            }
        }
        if (wheel_outside_all_shapes) {
            PDFatalError("Wheels outside all shapes");
        }
    }
}

// FUNCTION: CARMA2_HW 0x0042aa20
void C2_HOOK_FASTCALL InitPhysModCrushData(tCar_spec* pCar_spec) {

    CheckWheelPositions(pCar_spec);
    if (pCar_spec->car_crush_spec != NULL) {
        SwitchCarModels(pCar_spec, 0);
        InitModelMasterCrushData(pCar_spec);
        InitModelCrushData(pCar_spec);
        InitPhysMasterCrushData(pCar_spec);
        InitPhysCrushData(pCar_spec);
        InitVertexData(pCar_spec);
    }
}

// FUNCTION: CARMA2_HW 0x0042d9b0
void C2_HOOK_FASTCALL MakeModelMaterialsSingleSided(br_model* pModel) {
    int g;

    if (pModel == NULL) {
        return;
    }
    for (g = 0; g < pModel->prepared->ngroups; g++) {
        br_material* mat = pModel->faces[*pModel->prepared->groups[g].face_user].material;

        if (mat != NULL && (mat->flags & BR_MATF_TWO_SIDED) && mat->identifier != NULL && mat->identifier[0] == 'S') {
            mat->flags &= ~BR_MATF_TWO_SIDED;
            BrMaterialUpdate(mat, BR_MATU_RENDERING);
        }
    }
}

// FUNCTION: CARMA2_HW 0x0042deb0
intptr_t C2_HOOK_CDECL MakeCarModelsMaterialsSingleSided(br_actor* pActor, void* pArg) {
    tUser_crush_data* user_crush = (tUser_crush_data*)pActor->user;
    tCar_spec *car_spec = (tCar_spec*)pArg;
    if (user_crush != NULL) {
        int i;

        for (i = 0; i < car_spec->count_detail_levels; i++) {
            if (user_crush->models[i] != NULL) {
                MakeModelMaterialsSingleSided(user_crush->models[i]);
            }
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00431590
void C2_HOOK_CDECL JitModelUpdate(br_actor* pActor, br_model* pModel, br_material* pMaterial, void* pOrder_table, br_uint_8 pStyle, int pOn_screen) {

    BrModelUpdate(pModel, BR_MODU_VERTEX_POSITIONS);
    pModel->flags &= ~BR_MODF_CUSTOM;
    BrZsModelRender(pActor, pModel, pMaterial, pOrder_table, pStyle, BrOnScreenCheck(&pModel->bounds), 0);
}

// FUNCTION: CARMA2_HW 0x004315e0
void C2_HOOK_FASTCALL SetModelForUpdate(br_model* pModel) {
    tUser_detail_level_model* user_detail_model;

    if ((pModel->flags & BR_MODF_CUSTOM) != 0 && pModel->user != NULL) {
        user_detail_model = pModel->user;
        *(int*)user_detail_model->field_0x0 = 1;
    } else {
        pModel->custom = JitModelUpdate;
        pModel->flags |= BR_MODF_CUSTOM;
    }
}

void C2_HOOK_FASTCALL TotallyRepairModel(br_model* pModel) {
    tUser_detail_level_model* user_detail_model;
    tModel_detail_vertex_data* vertex_detail_data;
    int i;
    br_vector3 vector_mash[1024];

    if (pModel == NULL) {
        return;
    }
    user_detail_model = pModel->user;
    if (user_detail_model == NULL) {
        return;
    }
    vertex_detail_data = user_detail_model->field_0x4;
    if (vertex_detail_data == NULL) {
        return;
    }
    if (pModel->nvertices > CARPOCALYPSE2_ASIZE(vector_mash)) {
        PDFatalError("Holy polygons Batman! There's a lot of vertices on that model.");
    }
    for (i = 0; i < pModel->nvertices; i++) {
        br_vector3 pos;

        ExpandVector3(&pos, &vertex_detail_data[i].p, -10.f, 10.f);
        BrVector3Sub(&vector_mash[i], &pos, &pModel->vertices[i].p);
        BrVector3Copy(&pModel->vertices[i].p, &pos);
        vertex_detail_data[i].flags &= ~0x4;
    }
    AddModelMashToPipingSession(pModel, vector_mash);
    SetModelForUpdate(pModel);
}

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x00439500
void C2_HOOK_FASTCALL FUN_00439500(void) {

    DeleteFarCrushes(gCar_to_view);
}
#pragma auto_inline(on)

// FUNCTION: CARMA2_HW 0x00439a20
intptr_t C2_HOOK_CDECL TotallyRepairModels(br_actor* pActor, void* pUser) {

    tCar_spec* car_spec;
    tUser_crush_data* user_crush;
    tCar_crush_buffer_entry* crush_data;
    int i;

    car_spec = pUser;
    user_crush = pActor->user;
    if (user_crush == NULL) {

        return 0;
    }

    for (i = 0; i < car_spec->count_detail_levels; i++) {
        TotallyRepairModel(user_crush->models[i]);
    }

    crush_data = user_crush->crush_data;
    if (crush_data != NULL) {
        TotallyRepairSmash(car_spec, crush_data);
        crush_data->field_0x2c = 0;
        if (crush_data->flap_data != NULL && !crush_data->flap_data->kev_o_flap) {
            crush_data->flap_data->field_0x15 = 0;
            crush_data->flap_data->field_0x14 = 1;
            crush_data->flap_data->field_0x18 = 0.f;
        }
        if (crush_data->detach_data != NULL) {
            crush_data->detach_data->field_0x0 = 0.f;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00439a10
int C2_HOOK_FASTCALL TotallyRepairObject(tPhysics_object* pCollision_info, void* pUser_data) {

    DRActorEnumRecurse(pCollision_info->actor, TotallyRepairModels, pUser_data);
    return 0;
}

// FUNCTION: CARMA2_HW 0x004398d0
intptr_t C2_HOOK_CDECL BattenDownTheHatches(br_actor* pActor, void* pUser) {
    tUser_crush_data* user_crush_data = pActor->user;
    tCar_crush_buffer_entry* crush_data;
    tCar_spec* car_spec = pUser;

    if (user_crush_data == NULL) {
        return 0;
    }
    crush_data = user_crush_data->crush_data;
    if (crush_data == NULL) {
        return 0;
    }
    if (crush_data->flap_data == NULL) {
        return 0;
    }
    if (crush_data->flap_data->field_0x0 && !crush_data->flap_data->kev_o_flap) {
        StopFlapping(pActor, car_spec);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004398c0
int C2_HOOK_FASTCALL BattenDownTheObjects(tPhysics_object* pCollision_info, void* pUser_data) {

    DRActorEnumRecurse(pCollision_info->actor, BattenDownTheHatches, pUser_data);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00439960
void C2_HOOK_FASTCALL TotallyRepairCarCollisionShapes(tCar_spec *pCar_spec) {
    int i;

    for (i = 0; i < pCar_spec->car_crush_spec->count_shapes; i++) {

        tCar_crush_shape_info *shape_info;
        int j;

        shape_info = &pCar_spec->car_crush_spec->field_0x4[i];
        for (j = 0; j < shape_info->count_points; j++) {

            BrVector3Copy(&shape_info->field_0x18[j].field_0x18,
                          &shape_info->field_0x18[j].field_0x0);
            if (DRVector3TestForNan(&shape_info->field_0x18[j].field_0x18)) {
                PDEnterDebugger("NaN");
            }
        }
    }
    pCar_spec->car_crush_spec->expand_bounding_box = 3;
}

// FUNCTION: CARMA2_HW 0x00439fd0
void C2_HOOK_FASTCALL CompletelyUnBendCollisionShape(tCar_crush_shape_info *pShape) {
    int i;

    pShape->count_points = pShape->original_count_points;
    for (i = 0; i < pShape->count_points; i++) {
        BrVector3Copy(
            &pShape->field_0x18[i].field_0x18,
            &pShape->field_0x18[i].field_0x2c);
    }
}

void C2_HOOK_FASTCALL CompletelyUnBendCollisionShapes(tCar_spec* pCar_spec) {
    int i;

    for (i = 0; i < pCar_spec->car_crush_spec->count_shapes; i++) {
        CompletelyUnBendCollisionShape(&pCar_spec->car_crush_spec->field_0x4[i]);
    }
    pCar_spec->car_crush_spec->expand_bounding_box = 3;
}

// FUNCTION: CARMA2_HW 0x0043a010
void C2_HOOK_FASTCALL CompletelyUnBendWheels(tCar_spec* pCar_spec) {
    int i;

    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(pCar_spec->wheel_actors); i++) {
        br_actor *actor;
        tGroovidelic_spec *groove;

        actor = pCar_spec->pivot_actors[i];
        if (actor == NULL) {
            actor = pCar_spec->wheel_actors[i];
            if (actor == NULL) {
                continue;
            }
        }
        groove = ActorsGroove(actor);
        if (groove != NULL && groove->path_type == eGroove_path_straight) {
            br_vector3 original_centre;
            br_vector3 delta;

            BrVector3Negate(&original_centre, &groove->path_data.straight_info.centre);
            BrVector3Copy(&groove->path_data.straight_info.centre,
                &pCar_spec->car_crush_spec->field_0x1ec[i]);
            BrVector3Add(&delta, &groove->path_data.straight_info.centre, &original_centre);
            PipeSingleVector3(&groove->path_data.straight_info.centre, &delta);
        }
    }

    for (i = 0; i < CARPOCALYPSE2_ASIZE(pCar_spec->wpos); i++) {
        BrVector3Copy(&pCar_spec->wpos[i], &pCar_spec->car_crush_spec->field_0x234[i]);
    }
    SetCarSuspGiveAndHeight(pCar_spec, 1.f, 1.f, 1.f, 0.f, 0.f);
    pCar_spec->field_0x1260 = 0.f;
}

void C2_HOOK_FASTCALL CompletelyUnBendOtherThings(tCar_spec* pCar_spec) {

    CompletelyUnBendCollisionShapes(pCar_spec);
    CompletelyUnBendWheels(pCar_spec);
    BrVector3Copy(
        &pCar_spec->collision_info->cmpos,
        &pCar_spec->car_crush_spec->field_0x264);
}

// FUNCTION: CARMA2_HW 0x00439910
intptr_t C2_HOOK_CDECL EnableGroovers(br_actor *pActor, void* pData) {
    tUser_crush_data *user_crush;

    user_crush = pActor->user;
    if (user_crush != NULL && user_crush->groove != NULL) {
        PipeSingleGrooveOnOff(1, pActor, 3);
        EnableGroovidelic(pActor, 3);
        PipeSingleTransformType(pActor, BR_TRANSFORM_MATRIX34_LP);
        pActor->t.type = BR_TRANSFORM_MATRIX34_LP;
    }
    return 0;
}

void C2_HOOK_FASTCALL ClipPointToPolyhedron(br_vector3* pPoint, tCollision_shape_polyhedron_data* pPolyhedron) {
    int i;
    for (i = 0; i < pPolyhedron->count_planes; i++) {
        br_vector4* plane = &pPolyhedron->planes[i];
        br_scalar proj_factor;

        proj_factor = BrVector3Dot(plane, pPoint) + plane->v[3];
        if (proj_factor < 0.f) {
            br_vector3 proj;

            BrVector3Scale(&proj, plane, -proj_factor);
            BrVector3Accumulate(pPoint, &proj);
        }
    }
}

int C2_HOOK_FASTCALL PointIsOutsideShape(br_vector3* pPoint, tCollision_shape_polyhedron_data* pPolyhedron) {
    int i;

    for (i = 0; i < pPolyhedron->count_planes; i++) {
        br_vector4* plane = &pPolyhedron->planes[i];

        if (BrVector3Dot(plane, pPoint) + plane->v[3] < -1e-5f) {
            return 1;
        }
    }
    return 0;
}

int C2_HOOK_FASTCALL PointIsOutsideLimits(tCar_crush_shape_info* pShape, int pPoint_index, tCar_crush_limits* pLimits) {
    int i;
    tU16 flags;

    C2_HOOK_BUG_ON(sizeof(pLimits->limits) != 0x60);
    C2_HOOK_BUG_ON(sizeof(pLimits->limits[0]) != 0x20);
    C2_HOOK_BUG_ON(sizeof(pLimits->limits[0][0]) != 0x10);
    C2_HOOK_BUG_ON(sizeof(pLimits->limits[0][0].values[0]) != 0x4);

    flags = pShape->field_0x18[pPoint_index].field_0x28;
    for (i = 0; i < 3; i++) {
        br_scalar v = pShape->field_0x18[pPoint_index].field_0x18.v[i];

        if (v < pLimits->limits[i][0].values[(flags >> 0) & 0x3]) {
            return 1;
        }
        if (v > pLimits->limits[i][1].values[(flags >> 2) & 0x3]) {
            return 1;
        }
        flags >>= 4;
    }
    return 0;
}

void C2_HOOK_FASTCALL QuantizeVector3(br_vector3* pV) {
    tCompressed_vector3 cv3;

    CompressVector3(&cv3, pV, -2.5f, 2.5f);
    ExpandVector3(pV, &cv3, -2.5f, 2.5f);
}

void C2_HOOK_FASTCALL SwapCarShapes(tCar_spec* pCar_spec, tPhysics_shape* pCollision_shape) {
    pCar_spec->collision_info->shape = pCollision_shape;
    UpdateCollisionObject(pCar_spec->collision_info);
    pCar_spec->collision_info->bb2.min.v[1] = GetCarOverallBoundsMinY(pCar_spec);
}

int C2_HOOK_FASTCALL MoveCarToSensiblePlace(tCar_spec* pCar_spec) {
    br_vector3 original_pos;
    float factor;
    int i;
    br_vector3 prev_delta;

    factor = 0.007246377f;
    BrVector3Set(&prev_delta, 0.f, 0.f, 0.f);
    BrVector3Copy(&original_pos, (br_vector3*)&pCar_spec->collision_info->transform_matrix.m[3]);
    for (i = 0; i < 10; i++) {
        br_vector3 delta;

        if (TestForCarInSensiblePlace(pCar_spec, &delta)) {
            return 1;
        }
        if (BrVector3Dot(&prev_delta, &delta) <= 0.707107f) {
            BrVector3Copy(&prev_delta, &delta);
            factor = 0.007246377f;
        } else {
            factor *= 2.f;
        }
        BrVector3Scale(&delta, &delta, factor);
        BrVector3Accumulate((br_vector3*)&pCar_spec->collision_info->transform_matrix.m[3], &delta);
    }
    BrVector3Copy((br_vector3*)&pCar_spec->collision_info->transform_matrix.m[3], &original_pos);
    return 0;
}

// FUNCTION: CARMA2_HW 0x004349c0
int C2_HOOK_FASTCALL SwapShapesIfPossible(tCar_spec *pCar_spec) {
    tCar_crush_spec* car_crush = pCar_spec->car_crush_spec;
    tCollision_shape_polyhedron* first_poly;
    tCollision_shape_polyhedron* last_poly;
    tPhysics_shape* original_shape;
    int i;

    if (car_crush == NULL) {
        return 0;
    }
    if (car_crush->count_shapes == 0) {
        return 0;
    }
    if (car_crush->field_0x4 == NULL) {
        return 0;
    }
    if (!car_crush->version_le_100) {
        car_crush->expand_bounding_box = 0;
        return 0;
    }
    first_poly = NULL;
    last_poly = NULL;
    for (i = 0; i < car_crush->count_shapes; i++) {
        tCar_crush_shape_info* shape = &car_crush->field_0x4[i];
        if (!shape->field_0x8) {
            if (car_crush->expand_bounding_box & 0x4) {
                int j;

                for (j = 0; j < shape->count_points; j++) {
                    tCar_crush_reordered_shape_info* shape_field_0x18 = &shape->field_0x18[j];

                    ClipPointToPolyhedron(&shape_field_0x18->field_0x18, &shape->field_0x0->polyhedron);
                    if (PointIsOutsideShape(&shape_field_0x18->field_0x18, &shape->field_0x0->polyhedron)
                            || PointIsOutsideLimits(shape, j, &car_crush->field_0xbc)) {
                        BrVector3Copy(&shape_field_0x18->field_0x18, &shape_field_0x18->field_0xc);
                    }
                    if (DRVector3TestForNan(&shape_field_0x18->field_0x18)) {
                        PDEnterDebugger("NaN");
                    }
                }
            }
            if (car_crush->expand_bounding_box & 0x2) {
                int j;

                shape->field_0x4->polyhedron.count_points = shape->count_points;
                for (j = 0; j < shape->count_points; j++) {
                    tCar_crush_reordered_shape_info* shape_field_0x18 = &shape->field_0x18[j];

                    if (gNet_mode != eNet_mode_none) {
                        QuantizeVector3(&shape_field_0x18->field_0x18);
                    }
                    BrVector3Copy(&shape->field_0x4->polyhedron.points[j],
                        &shape_field_0x18->field_0x18);
                }
                FillInShape((tPhysics_shape*)shape->field_0x4);
            }
            if (first_poly == NULL) {
                first_poly = shape->field_0x4;
            }
            if (last_poly != NULL) {
                last_poly->common.next = (tPhysics_shape*)shape->field_0x4;
            }
            last_poly = shape->field_0x4;
        }
    }
    if (first_poly == NULL) {
        return 0;
    }
    original_shape = pCar_spec->collision_info->shape;
    SwapCarShapes(pCar_spec, (tPhysics_shape*)first_poly);
    if (!(car_crush->expand_bounding_box & 0x4)) {
        if (!MoveCarToSensiblePlace(pCar_spec)) {
            SwapCarShapes(pCar_spec, original_shape);
            return 1;
        }
    }
    if (car_crush->expand_bounding_box & 0x8) {
        BrVector3Copy(&pCar_spec->collision_info->cmpos,
            &car_crush->field_0x10);
    }
    car_crush->expand_bounding_box = 0;
    for (i = 0; i < car_crush->count_shapes; i++) {
        tCar_crush_shape_info* shape = &car_crush->field_0x4[i];
        if (!shape->field_0x8) {
            tCollision_shape_polyhedron* poly;
            int j;

            poly = shape->field_0x0;
            shape->field_0x0 = shape->field_0x4;
            shape->field_0x4 = poly;
            for (j = 0; j < shape->count_points; j++) {
                BrVector3Copy(&shape->field_0x18->field_0xc,
                    &shape->field_0x18->field_0x18);
            }
        }
    }
    return 0;
}

void C2_HOOK_FASTCALL DoCompletelyUnBentThings(tCar_spec* pCar_spec) {

    pCar_spec->car_crush_spec->field_0x144 = 0;
    CompletelyUnBendOtherThings(pCar_spec);
    DRActorEnumRecurse(pCar_spec->car_model_actor, EnableGroovers, NULL);
}

extern int gCrush_spam_gate;
// FUNCTION: CARMA2_HW 0x00439bf0
void C2_HOOK_FASTCALL TotallyRepairCar(void) {

    if (!gCrush_spam_gate) {
        TotallyRepairACar(&gProgram_state.current_car);
    }
}

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x0043b250
void C2_HOOK_FASTCALL FUN_0043b250(tCar_spec* pCar, tPhysics_object* pObject) {
    tNet_message* message;
    tNet_game_player_info* player;

    message = NetBuildGuaranteedMessage(0x26, 0);
    player = NetPlayerFromCar(pCar);
    *(int*)((tU8*)message + 0x1c) = player->ID;
    *(tU8*)((tU8*)message + 0x20) = *(tU8*)((tU8*)pObject + 0x190);
    *(tU32*)((tU8*)message + 0x24) = gPHIL_last_physics_tick + 120;
    NetGuaranteedSendMessageToEverybody(gCurrent_net_game, message, 0);
}
#pragma auto_inline(on)

#pragma auto_inline(off)

// FUNCTION: CARMA2_HW 0x004358a0
int C2_HOOK_FAKE_THISCALL FUN_004358a0(br_actor* pActor, int pArg2, tCar_spec** ppCar) {
    tUser_crush_data* user_crush_data;
    br_actor** piVar2;
    int i;

    user_crush_data = pActor->user;
    if (user_crush_data != NULL) {
        piVar2 = (*ppCar)->wheel_actors;
        for (i = 0; i < 6; i++) {
            if (pActor == piVar2[6]) {
                return 0;
            }
            if (pActor == piVar2[0]) {
                return 0;
            }
            piVar2++;
        }
        if (user_crush_data->groove != NULL) {
            PipeSingleGrooveOnOff(0, pActor, 3);
            DisableGroovidelic(pActor, 3);
            PipeSingleTransformType(pActor, 5);
            *(tU16*)((tU8*)pActor + 0x28) = 5;
        }
        if (user_crush_data->crush_data != NULL) {
            i = 0;
            if (0 < (*ppCar)->count_detail_levels) {
                do {
                    FUN_00435960(((void**)user_crush_data)[i], (const br_vector3*)pArg2, ppCar, pActor);
                    i++;
                } while (i < (*ppCar)->count_detail_levels);
            }
            user_crush_data->crush_data->field_0x2c = 1;
        }
    }
    return 0;
}

// C2PORT-NONMATCHING: best 40.55% | difftest PASS 2000 trials (spec difftest/specs/00435960_FUN_00435960.json) | reason x87-order-stack-layout
// FUNCTION: CARMA2_HW 0x00435960
void C2_HOOK_FASTCALL FUN_00435960(br_model* pModel, const br_vector3* pOffset, tCar_spec** ppCar, br_actor* pActor) {
    tUser_detail_level_model* model_user_data;
    tModel_detail_vertex_data* original_vertices;
    tU8* curve;
    tU16 vertex_index;
    int changed;
    float work[3074];
    float* mash;

    (void)pActor;
    if (pModel == NULL || pModel->user == NULL) {
        return;
    }
    model_user_data = pModel->user;
    original_vertices = model_user_data->field_0x4;
    if (original_vertices == NULL) {
        return;
    }
    if (pModel->nvertices > 0x400) {
        PDFatalError("Alas, there's a lot of vertices on that model.");
    }

    curve = *(tU8**)((tU8*)ppCar + 0x34);
    *(tU16*)model_user_data->field_0x8 = gCrush_vertex_compression_index;
    mash = work + 2;
    changed = 0;
    for (vertex_index = 0; vertex_index < pModel->nvertices; vertex_index++) {
        br_vertex* vertex;
        br_vector3 position;
        br_vector3 transformed;
        tU8* vertex_flags;
        int mode;
        float z_delta;

        vertex = &pModel->vertices[vertex_index];
        position.v[0] = vertex->p.v[0] + pOffset->v[0];
        position.v[1] = vertex->p.v[1] + pOffset->v[1];
        position.v[2] = vertex->p.v[2] + pOffset->v[2];
        mode = *(int*)(curve + 0x74);
        z_delta = position.v[2] - *(float*)(curve + 0x20);
        vertex_flags = (tU8*)original_vertices + vertex_index * 10 + 6;
        if ((mode != 0 && z_delta <= gCrushZero) || (mode == 0 && z_delta >= gCrushZero)) {
            BrMatrix34ApplyP(&transformed, &position, (br_matrix34*)((tU8*)ppCar + 4));
            mash[vertex_index * 3] = -vertex->p.v[0];
            mash[vertex_index * 3 + 1] = -vertex->p.v[1];
            mash[vertex_index * 3 + 2] = -vertex->p.v[2];
            vertex->p.v[0] = transformed.v[0] - pOffset->v[0];
            vertex->p.v[1] = transformed.v[1] - pOffset->v[1];
            vertex->p.v[2] = transformed.v[2] - pOffset->v[2];
            mash[vertex_index * 3] += vertex->p.v[0];
            mash[vertex_index * 3 + 1] += vertex->p.v[1];
            mash[vertex_index * 3 + 2] += vertex->p.v[2];
            *vertex_flags |= 0x18;
            changed = 1;
        } else {
            float x_delta;
            float y_delta;

            x_delta = position.v[0] - *(float*)(curve + 0x24);
            y_delta = position.v[1] - *(float*)(curve + 0x28);
            z_delta = position.v[2] - *(float*)(curve + 0x2c);
            if ((mode != 0 && z_delta <= gCrushZero) || (mode == 0 && z_delta >= gCrushZero)) {
                float x_norm;
                float y_norm;
                float z_norm;
                float cross_norm;
                float x_correction;
                float y_correction;
                float z_correction;
                tCompressed_vector3* compressed_position;
                int compression_index;

                *vertex_flags = (*vertex_flags & 0xef) | 8;
                compression_index = gCrush_vertex_compression_index;
                compressed_position = *(tCompressed_vector3**)(curve + 0x78) + (compression_index & 0xffff);
                CompressVector3(compressed_position, &position, -10.f, 10.f);
                gCrush_vertex_compression_index++;
                x_norm = x_delta / *(float*)(curve + 0x30);
                y_norm = y_delta / *(float*)(curve + 0x34);
                z_norm = z_delta / *(float*)(curve + 0x38);
                cross_norm = y_norm * x_norm;
                x_correction = *(float*)(curve + 0x44) * cross_norm;
                x_correction += *(float*)(curve + 0x50) * x_norm;
                x_correction += *(float*)(curve + 0x5c) * y_norm;
                x_correction += *(float*)(curve + 0x68);
                y_correction = *(float*)(curve + 0x48) * cross_norm;
                y_correction += *(float*)(curve + 0x54) * x_norm;
                y_correction += *(float*)(curve + 0x60) * y_norm;
                y_correction += *(float*)(curve + 0x6c);
                z_correction = *(float*)(curve + 0x4c) * cross_norm;
                z_correction += *(float*)(curve + 0x58) * x_norm;
                z_correction += *(float*)(curve + 0x64) * y_norm;
                z_correction += *(float*)(curve + 0x70);
                mash[vertex_index * 3] = x_correction * z_norm;
                mash[vertex_index * 3 + 1] = y_correction * z_norm;
                mash[vertex_index * 3 + 2] = z_correction * z_norm;
                vertex->p.v[0] = (position.v[0] + x_correction * z_norm) - pOffset->v[0];
                vertex->p.v[1] = (position.v[1] + y_correction * z_norm) - pOffset->v[1];
                vertex->p.v[2] = (position.v[2] + z_correction * z_norm) - pOffset->v[2];
                changed = 1;
            } else {
                mash[vertex_index * 3] = 0.f;
                mash[vertex_index * 3 + 1] = 0.f;
                mash[vertex_index * 3 + 2] = 0.f;
                *vertex_flags &= 0xf7;
            }
        }
    }

    if (changed) {
        if ((pModel->flags & BR_MODF_CUSTOM) == 0 || pModel->user == NULL) {
            pModel->custom = JitModelUpdate;
            pModel->flags |= BR_MODF_CUSTOM;
        } else {
            *(tU32*)pModel->user = 1;
        }
        AddModelMashToPipingSession(pModel, (br_vector3*)mash);
    }
}

// C2PORT-NONMATCHING: best 50.15% | difftest PASS 2000 trials (spec difftest/specs/00435fa0_FUN_00435fa0.json) | reason VC5 stack layout and x87/register allocation
// FUNCTION: CARMA2_HW 0x00435fa0
void C2_HOOK_FASTCALL FUN_00435fa0(tCar_spec* pCar) {
    int* pPanel_id;
    br_actor* pModel_actor;
    br_actor* pActor;
    br_actor* pNext_actor;
    int found;
    tPhysics_object* pDetached;
    float fVar3;
    float fVar4;
    float fVar5;
    float fVar6;
    float fVar7;
    float fVar8;
    float fVar9;
    br_vector3 world_position;
    br_vector3 detached_position;

    pPanel_id = gCrush_panel_ids;
    do {
        *pPanel_id = IRandomBetween(1, pCar->car_crush_spec->nb_entries);
        pPanel_id++;
    } while (pPanel_id < &gCrush_panel_ids[CARPOCALYPSE2_ASIZE(gCrush_panel_ids)]);

    pModel_actor = pCar->car_model_actor;
    pActor = *(br_actor**)((tU8*)pModel_actor + 8);
    while (pActor != NULL) {
        pNext_actor = *(br_actor**)pActor;
        if (DRActorRecurseWithPredicate(pActor, (void*)PredicateSpamPanel, pCar) != 0) {
            return;
        }
        pActor = pNext_actor;
    }

    pModel_actor = *(br_actor**)((tU8*)pModel_actor + 0x60);
    if (pModel_actor == NULL) {
        return;
    }
    pActor = *(br_actor**)((tU8*)pModel_actor + 8);
    if (pActor == NULL) {
        return;
    }
    if (*(void**)((tU8*)pActor + 0x34) == NULL) {
        return;
    }

    found = 0;
    pPanel_id = gCrush_panel_ids;
    do {
        if (*pPanel_id == (int)*(tU8*)((tU8*)pActor + 0x20)) {
            found = 1;
            break;
        }
        pPanel_id++;
    } while (pPanel_id < &gCrush_panel_ids[CARPOCALYPSE2_ASIZE(gCrush_panel_ids)]);
    if (found && (pDetached = DetachBit(pCar, pModel_actor, NULL)) != NULL) {
        BrMatrix34ApplyP(&world_position, (br_vector3*)(*(tU8**)((tU8*)pCar + 8) + 0x14),
            (br_matrix34*)(*(tU8**)(*(tU8**)((tU8*)pCar + 8)) + 0x2c));
        BrMatrix34ApplyP(&detached_position, (br_vector3*)((tU8*)pDetached + 0x14),
            (br_matrix34*)(*(tU8**)pDetached + 0x2c));

        fVar8 = detached_position.v[0] - world_position.v[0];
        fVar3 = detached_position.v[1] - world_position.v[1];
        fVar7 = detached_position.v[2] - world_position.v[2];
        fVar5 = (float)sqrt((double)(fVar7 * fVar7 + fVar3 * fVar3 + fVar8 * fVar8));
        fVar4 = gCrushZero;
        fVar6 = gCrushZero;
        fVar9 = gFLOAT_00589a54;
        if (gFLOAT_00589a5c < fVar5) {
            fVar5 = (float)gDOUBLE_00589a60 / fVar5;
            fVar4 = fVar5 * fVar3;
            fVar6 = fVar5 * fVar7;
            fVar9 = fVar5 * fVar8;
        }
        fVar5 = fVar9 * gFLOAT_00589ac0 + pDetached->v.v[0];
        fVar3 = fVar4 * gFLOAT_00589ac0 + pDetached->v.v[1];
        fVar4 = fVar6 * gFLOAT_00589ac0 + pDetached->v.v[2];
        pDetached->v.v[0] = fVar5;
        pDetached->v.v[1] = fVar3;
        pDetached->v.v[2] = fVar4;
        pDetached->v.v[0] = *(float*)(*(tU8**)((tU8*)pCar + 8) + 0x68) + fVar5;
        pDetached->v.v[1] = *(float*)(*(tU8**)((tU8*)pCar + 8) + 0x6c) + fVar3;
        pDetached->v.v[2] = *(float*)(*(tU8**)((tU8*)pCar + 8) + 0x70) + fVar4;
    }
}
// FUNCTION: CARMA2_HW 0x0043b2a0
void C2_HOOK_FASTCALL FUN_0043b2a0(tCar_spec* pCar) {
    tNet_message* message;

    message = NetBuildGuaranteedMessage(0x2b, 0);
    *(int*)((tU8*)message + 0x1c) = NetPlayerFromCar(pCar)->ID;
    *(tU32*)((tU8*)message + 0x20) = gPHIL_last_physics_tick + 120;
    NetGuaranteedSendMessageToEverybody(gCurrent_net_game, message, 0);
}
#pragma auto_inline(on)

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x0043b2e0
intptr_t C2_HOOK_CDECL FUN_0043b2e0(br_actor* pActor, tCrush_displacement_context* pContext) {
    tUser_crush_data* pUser_crush_data;
    br_vector3 original_position;
    float displacement;
    int model_index;
    int vertex_index;
    br_model** pModel_cursor;

    pUser_crush_data = pActor->user;
    if (pUser_crush_data == NULL || pUser_crush_data->crush_data == NULL
            || pUser_crush_data->crush_data->field_0x2c == 0) {
        return 0;
    }
    model_index = 0;
    if (model_index < pContext->car->count_detail_levels) {
        pModel_cursor = (br_model**)pUser_crush_data;
        do {
            br_model* pModel;
            float* pTotal_displacement;

            pTotal_displacement = pContext->total_displacement;
            pModel = *pModel_cursor;
            if (pModel != NULL && pModel->user != NULL
                    && ((tUser_detail_level_model*)pModel->user)->field_0x4 != NULL) {
                tModel_detail_vertex_data* pOriginal_vertices;

                pOriginal_vertices = ((tUser_detail_level_model*)pModel->user)->field_0x4;
                displacement = gCrushZero;
                for (vertex_index = 0; vertex_index < (int)pModel->nvertices; vertex_index++) {
                    int axis;

                    ExpandVector3(&original_position, &pOriginal_vertices[vertex_index].p, -10.f, 10.f);
                    for (axis = 0; axis < 3; axis++) {
                        displacement = fabsf(original_position.v[axis] - pModel->vertices[vertex_index].p.v[axis]) + displacement;
                    }
                }
                displacement += *pTotal_displacement;
                *pTotal_displacement = displacement;
            }
            model_index++;
            pModel_cursor++;
        } while (model_index < pContext->car->count_detail_levels);
    }
    return 0;
}
#pragma auto_inline(on)

// C2PORT-NONMATCHING: best 92.56% | reason VC5 branch displacement layout
// FUNCTION: CARMA2_HW 0x0043b470
intptr_t C2_HOOK_CDECL FUN_0043b470(br_actor* pActor, tModel_mash_context* pContext) {
    tUser_crush_data* user_crush_data;
    tCar_crush_buffer_entry* crush_data;
    int model_index;

    user_crush_data = pActor->user;
    if (user_crush_data == NULL) {
        return 0;
    }
    crush_data = user_crush_data->crush_data;
    if (crush_data == NULL) {
        return 0;
    }
    if (CrushModelVertices(user_crush_data->models[0], pContext) != 0) {
        AddRepairAnimationToQueue(pContext->car, crush_data);
        crush_data->field_0x2c = 0;
        if (crush_data->flap_data != NULL && crush_data->flap_data->kev_o_flap == 0) {
            crush_data->flap_data->field_0x15 = 0;
            crush_data->flap_data->field_0x14 = 1;
            crush_data->flap_data->field_0x18 = 0;
        }
        if (crush_data->detach_data != NULL) {
            crush_data->detach_data->field_0x0 = 0.0f;
        }
    } else {
        for (model_index = 1; model_index < pContext->car->count_detail_levels; model_index++) {
            CrushModelVertices(user_crush_data->models[model_index], pContext);
        }
    }
    return 0;
}

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x0043b400
int C2_HOOK_FASTCALL FUN_0043b400(tPhysics_object* pObject, tCrush_displacement_context* pContext) {
    tCrush_displacement_context* context;
    tModel_mash_context model_context;

    context = pContext;
    model_context.car = context->car;
    model_context.step = context->step;
    model_context.displacement = 0.0f;
    model_context.total_displacement = context->total_displacement;

    ARStartPipingSession(0x18);
    DRActorEnumRecurse(pObject->actor, (br_actor_enum_cbfn*)FUN_0043b470, &model_context);
    AREndPipingSession();
    DRActorEnumRecurse(pObject->actor, (br_actor_enum_cbfn*)FUN_00434320, NULL);

    context->displacement += model_context.displacement;
    return 0;
}
#pragma auto_inline(on)

// C2PORT-NONMATCHING: best 62.85% | difftest PASS 2000 trials (spec difftest/specs/0043b8a0_WeldCar.json) | reason register-allocation/loop-codegen
// FUNCTION: CARMA2_HW 0x0043b8a0
void C2_HOOK_FASTCALL WeldCar(tCar_spec* pCar_spec) {
    tPhysics_object* object;

    register tCar_crush_spec* crush_spec;
    tNon_car_spec* non_car_info;
    tPhysics_object* collision_info;
    tPhysics_object* next_object;
    tCollision_shape_polyhedron* shape;
    br_actor* actor;
    br_actor** actor_ptr;
    tNon_car_spec** active_ptr;
    int remaining;
    br_model** local_108;
    br_model** model_ptr;
    br_model** saved_models;
    int i;
    int index;
    char local_100[256];

    crush_spec = pCar_spec->car_crush_spec;
    if (crush_spec->field_0x4b8 == 0) {
        return;
    }

    CloseDownPowerUpsForCar(pCar_spec);
    non_car_info = crush_spec->actor->type_data;
    if (non_car_info != NULL) {
        collision_info = non_car_info->collision_info;
        while (collision_info->shape->common.type == kCollisionShapeType_Wireframe) {
            collision_info->shape = collision_info->shape->common.next;
        }

        object = collision_info->child;
        while (object != NULL) {
            PHILSendDetachBit((tU16)((tNon_car_spec*)object->actor->type_data)->car_ID, object->actor);
            next_object = object->next;
            PHILRemoveFromParentChain(object);
            PhysicsAddObject(pCar_spec->collision_info, object);
            object = next_object;
        }

        PHILSendDetachBit((tU16)non_car_info->car_ID, non_car_info->actor);
        index = 0;
        if (gNum_active_non_cars > 0) {
            active_ptr = gActive_non_car_list;
            do {
                if (*active_ptr == non_car_info) {
                    break;
                }
                index++;
                active_ptr++;
            } while (index < gNum_active_non_cars);
        }
        if (index != gNum_active_non_cars) {
            gNum_active_non_cars--;
            if (index < gNum_active_non_cars) {
                active_ptr = &gActive_non_car_list[index];
                remaining = gNum_active_non_cars - index;
                do {
                    *active_ptr = active_ptr[1];
                    active_ptr++;
                    remaining--;
                } while (remaining != 0);
            }
        }

        non_car_info->driver = 0;
        non_car_info->actor->type_data = NULL;
        PHILRemoveObject(non_car_info->collision_info);
    }

    for (actor = crush_spec->model_actor->children; actor != NULL; actor = crush_spec->model_actor->children) {
        FUN_004c86b0(0, actor);
        BrActorRemove(actor);
        FUN_004c86b0(0, actor);
        BrActorAdd(pCar_spec->car_model_actor, actor);
    }

    actor_ptr = crush_spec->field_0x498;
    for (i = 0; i < crush_spec->field_0x494; i++, actor_ptr++) {
        actor = *actor_ptr;
        FUN_004c86b0((tU32)pCar_spec->car_model_actor, actor);
        BrActorAdd(pCar_spec->car_model_actor, actor);
    }
    crush_spec->field_0x494 = 0;

    local_108 = (br_model**)pCar_spec->car_model_actor->user;
    model_ptr = local_108;
    saved_models = crush_spec->field_0x4c4;
    for (i = 0; i < pCar_spec->count_detail_levels; i++) {
        if (*model_ptr != NULL) {
            BrModelFree(BrModelRemove(*model_ptr));
            *model_ptr = *saved_models;
        }
        model_ptr++;
        saved_models++;
    }
    pCar_spec->car_model_actor->model = local_108[0];
    BrModelFree(BrModelRemove(crush_spec->model_actor->model));
    FUN_004c86b0(0, crush_spec->actor);
    BrActorRemove(crush_spec->actor);
    DRActorEnumRecurse(pCar_spec->car_model_actor, MakeCarModelsMaterialsSingleSided, pCar_spec);

    shape = NULL;
    for (i = 0; i < crush_spec->count_shapes; i++) {
        tCar_crush_shape_info* shape_info;

        shape_info = &crush_spec->field_0x4[i];
        if (shape_info->field_0x8 == 0) {
            if (shape == NULL) {
                pCar_spec->collision_info->shape = (tPhysics_shape*)shape_info->field_0x0;
            } else {
                shape->common.next = (tPhysics_shape*)shape_info->field_0x0;
            }
            shape = shape_info->field_0x0;
        }
    }
    if (shape != NULL) {
        shape->common.next = NULL;
    }

    UpdateCollisionObject(pCar_spec->collision_info);
    pCar_spec->collision_info->bb2.min.v[1] = GetCarOverallBoundsMinY(pCar_spec);
    pCar_spec->number_of_wheels_on_ground = 0;
    pCar_spec->collision_info->actor = crush_spec->saved_collision_actor;
    pCar_spec->collision_info->M = crush_spec->saved_collision_mass;
    pCar_spec->collision_info->I.v[0] = crush_spec->saved_collision_inertia.v[0];
    pCar_spec->collision_info->I.v[1] = crush_spec->saved_collision_inertia.v[1];
    pCar_spec->collision_info->I.v[2] = crush_spec->saved_collision_inertia.v[2];
    pCar_spec->collision_info->cmpos.v[0] = crush_spec->saved_collision_cmpos.v[0];
    pCar_spec->collision_info->cmpos.v[1] = crush_spec->saved_collision_cmpos.v[1];
    pCar_spec->collision_info->cmpos.v[2] = crush_spec->saved_collision_cmpos.v[2];
    pCar_spec->collision_info->field_0x54.v[0] = crush_spec->saved_collision_field_0x54.v[0];
    pCar_spec->collision_info->field_0x54.v[1] = crush_spec->saved_collision_field_0x54.v[1];
    pCar_spec->collision_info->field_0x54.v[2] = crush_spec->saved_collision_field_0x54.v[2];
    pCar_spec->collision_info->box_face_ref = gFace_num__car - 2;
    SetCarSuspGiveAndHeight(pCar_spec, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f);
    crush_spec->field_0x28 = 1;
    pCar_spec->field_0x18c8 = 0;
    crush_spec->field_0x4b8 = 0;

    if (gProgram_state.racing != 0) {
        DRS3StartSound3D(gCar_outlet, 0x1484, &pCar_spec->collision_info->actor->t.t.translate.t,
                &gZero_v__car, 1, 0xff, IRandomBetween(0xe000, 0x12000), 0x10000);
    }

    if (pCar_spec != NULL && pCar_spec->driver == eDriver_local_human) {
        SpendCredits(2000);
        sprintf(local_100, "%s %d", "REPAIR COST", 2000);
        NewTextHeadupSlot(4, 0, 0x5dc, -4, local_100);
    }

    FUN_004c86f0((tU16)pCar_spec->car_ID, &crush_spec->field_0x548, 0);
}

// FUNCTION: CARMA2_HW 0x00434010
void C2_HOOK_FASTCALL ReAttachBit(tCar_spec* pCar, br_actor* pParent_actor, br_actor* pActor, tPhysics_object* pObject) {
    tCar_crush_detach_data* detach_data;
    tUser_crush_data* user_crush_data;
    tCar_crush_buffer_entry* crush_data;
    register tCar_crush_spec* crush_spec;
    register tNon_car_spec* non_car;
    tPhysics_shape* physics_shape;
    tCar_crush_flap_data* flap_data;
    tNon_car_spec** active_non_car;
    br_model* model;
    tUser_crush_data* parent_user_crush_data;
    br_model* parent_model;
    br_vertex* vertex;
    register int zero;
    int index;
    int remaining;

    zero = 0;
    user_crush_data = pActor->user;
    if (user_crush_data == (tUser_crush_data*)zero) {
        return;
    }
    crush_data = user_crush_data->crush_data;
    if (crush_data == (tCar_crush_buffer_entry*)zero) {
        return;
    }
    detach_data = crush_data->detach_data;
    if (detach_data == (tCar_crush_detach_data*)zero || detach_data->field_0x28 == zero) {
        return;
    }

    crush_spec = pCar->car_crush_spec;
    non_car = pActor->type_data;
    if (non_car != (tNon_car_spec*)zero) {
        pActor->type_data = (void*)zero;
        non_car->driver = zero;
        if (pObject != (tPhysics_object*)zero) {
            PHILRemoveFromParentChain(pObject);
            if (pObject->physics_joint1 != (tPhysics_joint*)zero) {
                pObject->physics_joint1->type = zero;
                pObject->physics_joint1 = (tPhysics_joint*)zero;
            }
            if (detach_data->type == (tDetach_type)4) {
                physics_shape = pObject->shape;
                pObject->shape = (tPhysics_shape*)crush_spec->field_0x4[detach_data->field_0x78].field_0x0;
                crush_spec->field_0x4[detach_data->field_0x78].field_0x0 = (tCollision_shape_polyhedron*)physics_shape;
                crush_spec->field_0x4[detach_data->field_0x78].field_0x8 = zero;
                crush_spec->expand_bounding_box |= 1;
            }
        } else {
            PHILSendDetachBit((tU16)non_car->car_ID, pActor);
            index = 0;
            while (index < gNum_active_non_cars && gActive_non_car_list[index] != non_car) {
                index++;
            }
            if (index != gNum_active_non_cars) {
                gNum_active_non_cars--;
                if (index < gNum_active_non_cars) {
                    active_non_car = &gActive_non_car_list[index];
                    remaining = gNum_active_non_cars - index;
                    do {
                        *active_non_car = active_non_car[1];
                        active_non_car++;
                        remaining--;
                    } while (remaining != zero);
                }
            }
            non_car->driver = zero;
            non_car->actor->type_data = (void*)zero;
            PHILRemoveObject(non_car->collision_info);
        }
    }

    FUN_004c86b0((tU32)pParent_actor, pActor);
    if (pActor->parent != (br_actor*)zero) {
        BrActorRelink(pParent_actor, pActor);
    } else {
        BrActorAdd(pParent_actor, pActor);
    }

    flap_data = crush_data->flap_data;
    if (flap_data == (tCar_crush_flap_data*)zero || flap_data->kev_o_flap != zero) {
        BrMatrix34Identity(&pActor->t.t.mat);
        PipeSingleIdentity(pActor);
        if (flap_data != (tCar_crush_flap_data*)zero) {
            flap_data->field_0x0 = (tU8)zero;
        }
    } else {
        model = user_crush_data->models[0];
        vertex = &model->vertices[flap_data->hinge0];
        BrMatrix34Translate(&pActor->t.t.mat,
            -vertex->p.v[0], -vertex->p.v[1], -vertex->p.v[2]);
        parent_user_crush_data = pParent_actor->user;
        parent_model = parent_user_crush_data->models[0];
        vertex = &parent_model->vertices[flap_data->field_0xc];
        BrMatrix34PostTranslate(&pActor->t.t.mat,
            vertex->p.v[0], vertex->p.v[1], vertex->p.v[2]);
        PipeSingleFlap((uintptr_t)pActor, (tU16)-flap_data->field_0x4);
        flap_data->field_0x4 = (tU16)zero;

        if (gCount_toggled_doors != 16) {
            index = 0;
            while (index < gCount_toggled_doors && gToggled_doors[index].actor != pActor) {
                index++;
            }
            if (index == gCount_toggled_doors) {
                gToggled_doors[index].actor = pActor;
                gToggled_doors[index].car = pCar;
                gToggled_doors[index].field_0x8 = 1;
                gToggled_doors[index].field_0xc = 0;
                gCount_toggled_doors++;
            }
        }
        FUN_00434320(pActor, zero);
    }

    PipeSingleDSModel(1, user_crush_data->models[0]);
    pActor->model = user_crush_data->models[0];
    if (gProgram_state.racing != zero && pObject == (tPhysics_object*)zero) {
        DRS3StartSound3D(gXXX_outlet, 0x1483,
            &pCar->collision_info->actor->t.t.translate.t,
            &gZero_v__car, 1, 0xff, IRandomBetween(0xe000, 0x12000), 0x10000);
    }
    if (user_crush_data->groove != (tGroovidelic_spec*)zero) {
        PipeSingleGrooveOnOff(1, pActor, 3);
        EnableGroovidelic(pActor, 3);
        PipeSingleTransformType(pActor, 1);
        pActor->t.type = 1;
    }
    detach_data->field_0x28 = zero;
}

extern int gCount_car_damage_crush_list;

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x0043cac0
int C2_HOOK_CDECL FindActorByCrushEntryID(br_actor* pActor, void* pArg) {
    tFind_crush_actor_arg* arg = pArg;
    tUser_crush_data* user_crush_data;

    user_crush_data = pActor->user;
    if (user_crush_data != NULL && user_crush_data->crush_data != NULL && user_crush_data->crush_data->id == arg->id) {
        arg->actor = pActor;
        return 1;
    }
    return 0;
}
#pragma auto_inline(on)

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x0043ce60
void C2_HOOK_FASTCALL ReceivedCrushReattachBit(tNet_message_chunk_crush_reattach_bit* pContents) {
    tCar_spec* car;
    int item_offset;
    int count;
    tU32 bit_index;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_crush_reattach_bit, player_id, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_crush_reattach_bit, field_0x8, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCrush_net_reattach_bit_list_item, car, 0x4);

    if (gCount_net_crush_reattach_bit_list != 8) {
        car = NetCarFromPlayerID(pContents->player_id);
        if (car != NULL) {
            count = gCount_net_crush_reattach_bit_list;
            item_offset = count * 3;
            item_offset <<= 2;
            count++;
            *(tCar_spec**)((tU8*)&gNet_crush_reattach_bit_list[0].car + item_offset) = car;
            *(tU32*)((tU8*)&gNet_crush_reattach_bit_list[0].field_0x0 + item_offset) = 0;
            bit_index = pContents->field_0x8;
            gCount_net_crush_reattach_bit_list = count;
            *(tU32*)((tU8*)&gNet_crush_reattach_bit_list[0].field_0x8 + item_offset) = bit_index;
        }
    }
}
#pragma auto_inline(on)

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x0043d1d0
void C2_HOOK_FASTCALL SendDetachedNonCarData(tNon_car_spec* pNon_car, undefined4 pArg2, tNet_game_player_info* pRecipient) {
    int hierarchy_size;
    int contents_size;
    int message;
    int written_size;
    undefined4 car;
    tNet_message* guaranteed_message;

    guaranteed_message = NULL;
    hierarchy_size = GetHierarchyNetworkSize(pNon_car->collision_info);
    contents_size = hierarchy_size + 0x22;
    if (pRecipient == NULL) {
        if (pNon_car->collision_info->disable_move_rotate == 0 || (pNon_car->flags & 0x100000) == 0) {
            message = (int)NetStartBroadcastContents(0x27, contents_size);
        } else {
            guaranteed_message = NetBuildGuaranteedMessage(0x27, contents_size);
            message = (int)guaranteed_message + 0x18;
            pNon_car->flags &= ~0x100000;
        }
    } else {
        message = (int)NetGetToPlayerContents(0x27, contents_size, pRecipient->ID);
    }
    if (message != 0) {
        *(undefined4*)(message + 0x18) = pArg2;
        *(tU16*)(message + 0x20) = (tU16)((pNon_car->actor->identifier[3] == '!') | (pNon_car->collision_info->disable_move_rotate << 1));
        written_size = GetHierarchyNetworkStuff(pNon_car->collision_info, (tU8*)(message + 0x22), 0x48);
        if (written_size != hierarchy_size) {
            FatalError(kFatalError_NetContentsTooBig_S, "in detached bit non_car object data");
        }
        if (pNon_car->driver == 2
                && pNon_car->actor != NULL
                && (car = *(undefined4*)((tU8*)pNon_car->actor + 0x60)) != 0
                && (car = *(undefined4*)((tU8*)car + 8)) != 0
                && (car = *(undefined4*)((tU8*)car + 0x34)) != 0) {
            car = *(undefined4*)((tU8*)car + 0x2c);
        } else {
            car = 0;
        }
        *(tPlayer_ID*)(message + 4) = NetPlayerFromCar((tCar_spec*)car)->ID;
        *(tU8*)(message + 8) = *(tU8*)((tU8*)*(void**)((tU8*)*(void**)((tU8*)pNon_car->actor + 0x60) + 8) + 0x20);
        CompressVector3((tCompressed_vector3*)(message + 0xa), (br_vector3*)((tU8*)pNon_car->collision_info + 0x24), -10.f, 10.f);
        CompressVector3((tCompressed_vector3*)(message + 0x10), (br_vector3*)((tU8*)pNon_car->collision_info + 0x30), -10.f, 10.f);
        if (pRecipient == NULL) {
            if (guaranteed_message == NULL) {
                NetBroadcastContents((tNet_message_chunk*)message);
                return;
            }
            NetGuaranteedSendMessageToEverybody(gCurrent_net_game, guaranteed_message, 0);
        }
    }
}
#pragma auto_inline(on)

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x0043d5b0
void C2_HOOK_FASTCALL SendDetachedNonCarDataToPlayer(tNet_game_player_info* pRecipient) {
    int i;
    tNon_car_spec** pNon_car;

    i = 0;
    if (gNum_active_non_cars > 0) {
        pNon_car = gActive_non_car_list;
        do {
            if ((*pNon_car)->driver == 2) {
                SendDetachedNonCarData(*pNon_car, -1, pRecipient);
            }
            i++;
            pNon_car++;
        } while (i < gNum_active_non_cars);
    }
}
#pragma auto_inline(on)

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x0043d5f0
void C2_HOOK_FASTCALL FUN_0043d5f0(const tU8* pMessage) {
    int crush;
    int count;
    int i;
    volatile int* actor_field;

    crush = (int)NetCarFromPlayerID(*(const tPlayer_ID*)(pMessage + 4));
    if (crush != 0) {
        crush = *(int*)(crush + 0x18d4);
        if (crush != 0) {
            count = *(int*)(crush + 0x2b0);
            i = 0;
            if (0 < count) {
                actor_field = (int*)(crush + 0x2bc);
                while (i < count) {
                    if (*(tU8*)(*actor_field + 0x190) == pMessage[8]) {
                        crush = *(int*)(crush + 0x2b4 + i * 0x14);
                        goto found_actor;
                    }
                    i++;
                    actor_field += 5;
                }
            }
        }
        crush = 0;
found_actor:
        if (crush != 0 && gNet_mode != 2) {
            BrMatrix34Copy((br_matrix34*)(crush + 0x2c), (br_matrix34*)(pMessage + 0xc));
        }
    }
}
#pragma auto_inline(on)

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x0043d670
void C2_HOOK_FASTCALL ToggleFlapsFromNetwork(tNet_message_chunk* pChunk) {
    tCar_spec* car;

    car = NetCarFromPlayerID(pChunk->toggle_doors.id);
    if (car != NULL) {
        PhysicsObjectRecurse(car->collision_info, ToggleFlapsCB, car);
    }
}
#pragma auto_inline(on)

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x0043d7d0
void C2_HOOK_FASTCALL FUN_0043d7d0(tNet_game_player_info* pRecipient) {
    int i;
    tNon_car_spec** pNon_car;

    i = 0;
    if (gNum_active_non_cars > 0) {
        pNon_car = gActive_non_car_list;
        do {
            if ((*pNon_car)->driver == 3) {
                SendSplitHalfNonCarData(*pNon_car, -1, pRecipient);
            }
            i++;
            pNon_car++;
        } while (i < gNum_active_non_cars);
    }
}
#pragma auto_inline(on)

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x0043d810
void C2_HOOK_FASTCALL FUN_0043d810(tU8* pMessage) {
    tCar_spec* car;
    tU8* crush_info;
    tU8* compressed_vector;
    float local_plane[4];
    int hierarchy_size;

    car = NetCarFromPlayerID(*(tPlayer_ID*)(pMessage + 4));
    if (car != NULL) {
        if (car->car_crush_spec->field_0x4b8 == 0) {
            compressed_vector = pMessage + 0xe;
            ExpandVector3((br_vector3*)local_plane, (tCompressed_vector3*)compressed_vector, -1.0f, 1.0f);
            local_plane[3] = DRU16ToScalar(*(tU16*)(compressed_vector + 6), -2.5f, 2.5f);
            FUN_0042e2e0(car, local_plane, 0);
        }
        crush_info = *(tU8**)(*(tU8**)((tU8*)car->car_crush_spec + 0x4cc) + 0x5c);
        if (crush_info != NULL) {
            if ((pMessage[0xc] & 2) == 0 && *(int*)(pMessage + 8) != -1) {
                hierarchy_size = GetNetworkDataSize(pMessage + 0x16);
                memcpy(*(tU8**)(crush_info + 8) + 0x294, pMessage + 0x16, hierarchy_size);
                *(int*)(*(tU8**)(crush_info + 8) + 0x268) = *(int*)(pMessage + 8);
                *(tU8*)(*(tU8**)(crush_info + 8) + 0x261) = 0x29;
                *(tU8*)(*(tU8**)(crush_info + 8) + 0xec) = 0;
                return;
            }
            SelectCrushNetwork((undefined4*)(pMessage + 0x16), *(tPhysics_object**)(crush_info + 8), 2);
            *(tU8*)(*(tU8**)(crush_info + 8) + 0xec) = 1;
        }
    }
}
#pragma auto_inline(on)

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x0043d900
int C2_HOOK_FASTCALL FUN_0043d900(tCar_spec* pCar) {
    tCar_crush_spec* crush;
    int* node;
    int i;
    int total;
    int result;

    crush = pCar->car_crush_spec;
    if (crush->field_0x4b8 != 0) {
        result = 1;
        node = *(int**)(*(int*)((tU8*)pCar + 8) + 0x20);
        if (node != NULL) {
            do {
                if (node[0] == 1 || node[0] == 3) {
                    result = result + 1 + node[0xe] * 6;
                }
                node = (int*)node[0xd];
            } while (node != NULL);
        }
        return result;
    }
    total = 0;
    for (i = 0; i < crush->count_shapes; i++) {
        total += crush->field_0x4[i].count_points;
    }
    return total * 6 + 1;
}
#pragma auto_inline(on)

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x0043d960
int C2_HOOK_FASTCALL GetCompressedCrushVertexDataSize(tU8* pData) {
    tU8 count_or_points;
    int count;
    int size;

    count_or_points = *pData++;
    if (count_or_points <= 4) {
        count = count_or_points;
        size = 1;
        if (count > 0) {
            do {
                count_or_points = *pData;
                size = size + 1 + count_or_points * 6;
                pData = pData + count_or_points * 6 + 1;
                count--;
            } while (count != 0);
        }
        return size;
    }
    return count_or_points * 6 + 1;
}
#pragma auto_inline(on)

// FUNCTION: CARMA2_HW 0x0043ceb0
void C2_HOOK_FASTCALL UpdateCrushPainList(void) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004f62e0
void C2_HOOK_FASTCALL UpdateCrushTimers(void) {
    tQueued_net_smash* pSmash;
    int count;

    if (gNet_mode == eNet_mode_none) {
        return;
    }
    pSmash = gQueued_net_smashes;
    count = CARPOCALYPSE2_ASIZE(gQueued_net_smashes);
    while (count-- != 0) {
        if (pSmash->field_0x4 != 0 && gPHIL_last_physics_tick >= pSmash->field_0x4) {
            pSmash->field_0x4 = 0;
            DoNetSmash(pSmash);
        }
        pSmash++;
    }
}

// FUNCTION: CARMA2_HW 0x004f6320
void C2_HOOK_FASTCALL FUN_004f6320(tSmashable_noncar_action* pAction) {
    int i;
    int smoke_i;
    br_actor* pActor;
    tNon_car_spec* pNon_car_spec;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSmashable_noncar_action, random_seed, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSmashable_noncar_action, non_car_index, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSmashable_noncar_action, velocity, 0xc);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSmashable_noncar_action, transform, 0x24);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSmashable_noncar_action, smoke_chance, 0x54);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSmashable_noncar_action, smoke_max, 0x60);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_actor, type_data, 0x5c);

    i = 0;
    if (gCount_smashable_noncar_shrapnel_actors > 0) {
        do {
            pActor = gSmashable_noncar_shrapnel_actors[i];
            if ((tU8)pActor->identifier[1] == pAction->shrapnel_actor_identifier[0]
                    && (tU8)pActor->identifier[2] == pAction->shrapnel_actor_identifier[1]) {
                pActor = gSmashable_noncar_shrapnel_actors[i];
                goto found_shrapnel_actor;
            }
            i++;
        } while (i < gCount_smashable_noncar_shrapnel_actors);
        pActor = NULL;
    } else {
        pActor = NULL;
    }
found_shrapnel_actor:
    if (pActor != NULL && gProgram_state.track_spec.non_car_list[pAction->non_car_index] == NULL) {
        pActor = CloneActor(pActor);
        sprintf(pActor->identifier + 4, "%04d.ACT", (int)pAction->non_car_index);
        gProgram_state.track_spec.non_car_list[pAction->non_car_index] = pActor;
        BrActorAdd(gDont_render_actor, pActor);
        BrMatrix34Copy(&pActor->t.t.mat, &pAction->transform);
        if (pActor->type_data == NULL) {
            pNon_car_spec = DoPullActorFromWorld(pActor);
            if (pNon_car_spec != NULL) {
                pNon_car_spec->collision_info->v.v[0] = pAction->velocity.v[0];
                pNon_car_spec->collision_info->v.v[1] = pAction->velocity.v[1];
                pNon_car_spec->collision_info->v.v[2] = pAction->velocity.v[2];
                pNon_car_spec->collision_info->omega.v[0] = pAction->angular_velocity.v[0];
                pNon_car_spec->collision_info->omega.v[1] = pAction->angular_velocity.v[1];
                pNon_car_spec->collision_info->omega.v[2] = pAction->angular_velocity.v[2];
                srand(pAction->random_seed);
                if (PercentageChance(pAction->smoke_chance) && pAction->smoke_count > 0) {
                    for (smoke_i = 0; smoke_i < pAction->smoke_count; smoke_i++) {
                        CreateSmokeColumn2(2, pActor, (tCar_spec*)pNon_car_spec, NULL,
                            IRandomBetween(pAction->smoke_min, pAction->smoke_max),
                            IRandomBetween(0, pActor->model->prepared->groups->nvertices), 10000000);
                    }
                }
                if (gProgram_state.track_spec.count_non_cars < pAction->non_car_index + 1) {
                    gProgram_state.track_spec.count_non_cars = pAction->non_car_index + 1;
                }
                return;
            }
        }
        BrActorRemove(pActor);
        BrActorFree(pActor);
        gProgram_state.track_spec.non_car_list[pAction->non_car_index] = NULL;
    }
}

int C2_HOOK_FASTCALL GetAverageCrushLevel(tCar_spec* pCar);
void C2_HOOK_FASTCALL DeleteFarCrushes(tCar_spec* pCar);
int C2_HOOK_FASTCALL DRActorRecurseWithPredicate(br_actor* pActor, void* pPredicate, tCar_spec* pCar);
int C2_HOOK_CDECL PredicateSpamPanel(br_actor* pActor, tCar_spec* pCar);
void C2_HOOK_FASTCALL InitialiseSpamCrush(tCar_spec* pCar, void* pContext);
void C2_HOOK_FASTCALL CrushPanelWithForce(tCar_spec* pCar, tU32 pFlag_a, tU32 pFlag_b, tU32 pFlag_c, br_vector3* pVec, tU32 pFlag_d, tU32 pFlag_e);
intptr_t C2_HOOK_FASTCALL CrushCarActorCbfn(br_actor* pActor, br_matrix34* pMat, void* pContext);
intptr_t C2_HOOK_FASTCALL CrushCarModelCbfn(br_actor* pActor, br_matrix34* pMat, void* pContext);

// FUNCTION: CARMA2_HW 0x00431610
void C2_HOOK_FASTCALL TotallySpamTheModel(tCar_spec* pCar_spec, float pDamage) {
    tCar_crush_spec* spec;
    br_actor* model_actor;
    tPhysics_object* panel_actor;
    void* pMorph_data;
    float damage2;
    volatile float rate;
    float vdim[6];
    float vlow[3];
    float vhigh[3];
    br_vector3 vec_dir;
    struct {
        int count;
        br_vector3 v;
    } spam_arg;
    struct {
        tCar_spec* car;
        float* p_rate;
    } context;
    int pCount;
    int i;
    int j;
    int rnd;
    int flip;
    int axis;
    int found;
    int count;

    if (gCrush_spam_gate) {
        return;
    }
    if (gCrush_deferred) {
        count = gCount_car_damage_crush_list;
        if (count == 8) {
            return;
        }
        for (i = 0; i < count; i++) {
            if (gCar_damage_crush_list[i].car == pCar_spec) {
                if (pDamage > gCar_damage_crush_list[i].damage) {
                    gCar_damage_crush_list[i].damage = pDamage;
                }
                return;
            }
        }
        gCar_damage_crush_list[count].car = pCar_spec;
        gCar_damage_crush_list[count].damage = pDamage;
        gCount_car_damage_crush_list = count + 1;
        return;
    }

    spec = pCar_spec->car_crush_spec;
    if (spec == NULL) {

        return;
    }
    if (*(int*)((tU8*)pCar_spec + 0x1b4) != 0) {
        return;
    }
    if (spec->field_0x4b8 != 0) {
        return;
    }
    if (*(tU8*)&spec->field_0x144 != 0) {
        return;
    }
    if (gNet_mode) {
        srand(spec->field_0x574);
        spec->field_0x574 = rand();
    }
    for (i = 0; i < 10; i++) {
        gCrush_panel_ids[i] = IRandomBetween(1, spec->nb_entries);
    }

    model_actor = pCar_spec->car_model_actor;
    {
        br_actor* pNode = *(br_actor**)((tU8*)model_actor + 0x8);
        while (pNode != NULL) {
            if (DRActorRecurseWithPredicate(pNode, (void*)PredicateSpamPanel, pCar_spec)) {
                goto paint;
            }
            pNode = *(br_actor**)pNode;
        }
    }

    if (*(void**)((tU8*)model_actor + 0x60) != NULL) {
        br_actor* pShell = *(br_actor**)((tU8*)model_actor + 0x60);
        pShell = *(br_actor**)((tU8*)pShell + 0x8);
        if (pShell != NULL) {
            if (*(void**)((tU8*)pShell + 0x34) != NULL) {
                rnd = *(tU8*)((tU8*)pShell + 0x20);
                found = 0;
                for (i = 0; i < 10; i++) {
                    if (gCrush_panel_ids[i] == rnd) {
                        found = 1;
                        break;
                    }
                }
                if (found != 0) {
                    panel_actor = DetachBit(pCar_spec, model_actor, NULL);
                    if (panel_actor != NULL) {
                        br_vector3 tmp1;
                        br_vector3 tmp2;

                        double dx;
                        double dy;
                        double dz;
                        double len;
                        double inv_len;

                        BrMatrix34ApplyP(&tmp1, &pCar_spec->collision_info->cmpos, (br_matrix34*)((tU8*)pCar_spec->collision_info->actor + 0x2c));
                        BrMatrix34ApplyP(&tmp2, &panel_actor->cmpos, (br_matrix34*)((tU8*)panel_actor->actor + 0x2c));
                        dx = tmp2.v[0] - tmp1.v[0];
                        dy = tmp2.v[1] - tmp1.v[1];
                        dz = tmp2.v[2] - tmp1.v[2];
                        len = sqrt(dx * dx + dy * dy + dz * dz);
                        if (len > 2.38419e-07f) {
                            inv_len = 1.0 / len;
                            panel_actor->v.v[0] = pCar_spec->collision_info->v.v[0] + (float)(dx * inv_len * 1.15942f);
                            panel_actor->v.v[1] = pCar_spec->collision_info->v.v[1] + (float)(dy * inv_len * 1.15942f);
                            panel_actor->v.v[2] = pCar_spec->collision_info->v.v[2] + (float)(dz * inv_len * 1.15942f);
                        } else {
                            panel_actor->v.v[0] = pCar_spec->collision_info->v.v[0] + 1.15942f;
                            panel_actor->v.v[1] = pCar_spec->collision_info->v.v[1];
                            panel_actor->v.v[2] = pCar_spec->collision_info->v.v[2];
                        }
                    }
                }
            }
        }
    }

paint:
    damage2 = pDamage * 1.5f;
    pMorph_data = *(void**)((tU8*)pCar_spec + 0x8);
    pCount = 1;
    {
        float* pBBox = (float*)((tU8*)pMorph_data + 0x24);
        vdim[0] = pBBox[0] - 0.0289855f;
        vdim[1] = pBBox[1] - 0.0289855f;
        vdim[2] = pBBox[2] - 0.0289855f;
        vdim[3] = pBBox[3] - (-0.0289855f);
        vdim[4] = pBBox[4] - (-0.0289855f);
        vdim[5] = pBBox[5] - (-0.0289855f);
    }

    i = 10;
    while (i != 0) {
        rnd = IRandomBetween(0, 5);
        axis = rnd >> 1;
        flip = rnd & 1;
        vlow[axis] = flip ? vdim[3 + axis] : vdim[axis];
        vhigh[axis] = flip ? vdim[axis] : vdim[3 + axis];
        j = (axis + 1) % 3;
        vlow[j] = SRandomBetween(vdim[3 + j], vdim[j]);
        vhigh[j] = SRandomBetween(vdim[3 + j], vdim[j]);
        j = (j + 1) % 3;
        vlow[j] = SRandomBetween(vdim[3 + j], vdim[j]);
        vhigh[j] = SRandomBetween(vdim[3 + j], vdim[j]);

        vhigh[0] = vhigh[0] - vlow[0];
        vhigh[1] = vhigh[1] - vlow[1];
        vhigh[2] = vhigh[2] - vlow[2];
        {
            float len = (float)sqrt((double)(vhigh[0] * vhigh[0] + vhigh[1] * vhigh[1] + vhigh[2] * vhigh[2]));
            if (len > 2.38419e-07f) {
                float scale = (float)(1.0 / len);
                vhigh[0] = vhigh[0] * scale;
                vhigh[1] = vhigh[1] * scale;
                vhigh[2] = vhigh[2] * scale;
            } else {
                vhigh[0] = 1.0f;
                vhigh[1] = 0.0f;
                vhigh[2] = 0.0f;
            }
        }
        memcpy(&vec_dir, &vlow, sizeof(vec_dir));

        spec = pCar_spec->car_crush_spec;
        if (spec != NULL) {
            if (*(int*)((tU8*)pCar_spec + 0x1b4) == 0) {
                if (spec->field_0x4b8 == 0) {
                    float x = damage2;
                    if (x > gMax_crush_force) {
                        x = gMax_crush_force;
                    }
                    context.car = pCar_spec;
                    context.p_rate = (float*)&pCount;
                    rate = (float)(x / (double)pCount);
                    if (*(tU8*)&spec->field_0x144 != 0) {
                        ARStartPipingSession(3);
                        DRActorEnumRecurseWithSnart(pCar_spec->car_model_actor, NULL, CrushCarModelCbfn, &context);
                        AREndPipingSession();
                    } else {
                        InitialiseSpamCrush(pCar_spec, &context);
                        ARStartPipingSession(3);
                        DRActorEnumRecurseWithSnart(pCar_spec->car_master_actor, NULL, CrushCarActorCbfn, &context);
                        AREndPipingSession();
                        pCar_spec->use_shell_model = 0;
                    }
                }
            }
        }
        i--;
    }

    if (*(tU8*)&pCar_spec->car_crush_spec->field_0x144 != 0) {
        return;
    }
    if (GetAverageCrushLevel(pCar_spec) >= 0x4b) {
        return;
    }
    if (PercentageChance((int)(*(float*)((tU8*)pCar_spec->car_crush_spec + 0x4bc) * 25.0f))) {
        DeleteFarCrushes(pCar_spec);
        return;
    }
    if (*(float*)((tU8*)pCar_spec->car_crush_spec + 0x148) == gCrushZero) {
        return;
    }
    rnd = IRandomBetween(1, 8) - 1;
    if ((tU32)rnd > 7) {
        return;
    }
    switch (rnd) {
    case 0:
    {
        tCar_crush_spec* pSpec_a = pCar_spec->car_crush_spec;
        void* pMorph_a = *(void**)((tU8*)pCar_spec + 0x8);
        spam_arg.count = 0;
        spam_arg.v.v[1] = *(float*)((tU8*)pMorph_a + 0x34);
        spam_arg.v.v[2] = *(float*)((tU8*)pSpec_a + 0x14c);
        CrushPanelWithForce(pCar_spec, 0xe002, 0, 0, &spam_arg.v, *(tU32*)((tU8*)pMorph_a + 0x38), 0);
        return;
    }

    case 1:
    {
        tCar_crush_spec* pSpec_b = pCar_spec->car_crush_spec;
        void* pMorph_b = *(void**)((tU8*)pCar_spec + 0x8);
        spam_arg.count = 0;
        spam_arg.v.v[1] = *(float*)((tU8*)pMorph_b + 0x28);
        spam_arg.v.v[2] = *(float*)((tU8*)pSpec_b + 0x14c);
        CrushPanelWithForce(pCar_spec, 0x1ffe, 0, 0, &spam_arg.v, *(tU32*)((tU8*)pMorph_b + 0x38), 0);
        return;
    }

    case 2:
    {
        tCar_crush_spec* pSpec_c = pCar_spec->car_crush_spec;
        void* pMorph_c = *(void**)((tU8*)pCar_spec + 0x8);
        spam_arg.v.v[0] = *(float*)((tU8*)pMorph_c + 0x24);
        spam_arg.v.v[1] = 0.0f;
        spam_arg.v.v[2] = *(float*)((tU8*)pSpec_c + 0x14c);
        CrushPanelWithForce(pCar_spec, 0, 0xe002, 0, &spam_arg.v, *(tU32*)((tU8*)pMorph_c + 0x38), 0);
        return;
    }

    case 3:
    {
        tCar_crush_spec* pSpec_d = pCar_spec->car_crush_spec;
        void* pMorph_d = *(void**)((tU8*)pCar_spec + 0x8);
        spam_arg.v.v[0] = *(float*)((tU8*)pMorph_d + 0x30);
        spam_arg.v.v[1] = 0.0f;
        spam_arg.v.v[2] = *(float*)((tU8*)pSpec_d + 0x14c);
        CrushPanelWithForce(pCar_spec, 0, 0x1ffe, 0, &spam_arg.v, *(tU32*)((tU8*)pMorph_d + 0x38), 0);
        return;
    }

    case 4:
    case 5:
    case 6:
    case 7:
    {
        tCar_crush_spec* pSpec_e = pCar_spec->car_crush_spec;
        void* pMorph_e = *(void**)((tU8*)pCar_spec + 0x8);
        spam_arg.v.v[0] = (*(float*)((tU8*)pMorph_e + 0x24) + *(float*)((tU8*)pMorph_e + 0x30)) * 0.5f;
        spam_arg.v.v[1] = (*(float*)((tU8*)pMorph_e + 0x28) + *(float*)((tU8*)pMorph_e + 0x34)) * 0.5f;
        spam_arg.v.v[2] = *(float*)((tU8*)pSpec_e + 0x150);
        CrushPanelWithForce(pCar_spec, 0, 0x1ffe, 0, &spam_arg.v, *(tU32*)((tU8*)pMorph_e + 0x2c), 0);
        return;
    }
    }
}

#pragma auto_inline(off)

// FUNCTION: CARMA2_HW 0x00432e70
void C2_HOOK_FASTCALL InitialiseSpamCrush(tCar_spec* pCar, void* pContext) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x00435dd0
int C2_HOOK_FASTCALL DRActorRecurseWithPredicate(br_actor* pActor, void* pPredicate, tCar_spec* pCar) {
    int result;
    br_actor* actor;
    br_actor* next;
    int(C2_HOOK_CDECL* pred)(void*, void*) = (int(C2_HOOK_CDECL*)(void*, void*))pPredicate;

    actor = pActor;
    for (pActor = pActor->children; pActor != NULL; pActor = next) {
        next = pActor->next;
        result = DRActorRecurseWithPredicate(pActor, pred, pCar);
        if (result != 0) {
            return result;
        }
    }
    return pred(actor, pCar);
}

// FUNCTION: CARMA2_HW 0x00435e10
int C2_HOOK_CDECL PredicateSpamPanel(br_actor* pActor, tCar_spec* pCar) {
    tUser_crush_data* user;
    tCar_crush_buffer_entry* crush_data;
    int* pPanel_id;
    tPhysics_object* detached;
    br_vector3 tmp1;
    br_vector3 tmp2;
    double dx;
    double dy;
    double dz;
    double len;
    double inv_len;

    user = pActor->user;
    if (user == NULL) {
        goto fail;
    }
    crush_data = user->crush_data;
    if (crush_data == NULL) {
        goto fail;
    }
    if (crush_data->detach_data == NULL) {
        goto fail;
    }

    pPanel_id = gCrush_panel_ids;
    while (pPanel_id < &gCrush_panel_ids[CARPOCALYPSE2_ASIZE(gCrush_panel_ids)]) {
        if (*pPanel_id == crush_data->id) {
            break;
        }
        pPanel_id++;
    }
    if (pPanel_id >= &gCrush_panel_ids[CARPOCALYPSE2_ASIZE(gCrush_panel_ids)]) {
        goto fail;
    }
    detached = DetachBit(pCar, pActor, NULL);
    if (detached == NULL) {
        goto fail;
    }
    BrMatrix34ApplyP(&tmp1, &pCar->collision_info->cmpos, (br_matrix34*)((tU8*)pCar->collision_info->actor + 0x2c));
    BrMatrix34ApplyP(&tmp2, &detached->cmpos, (br_matrix34*)((tU8*)detached->actor + 0x2c));
    dx = tmp1.v[0] - tmp2.v[0];
    dy = tmp1.v[1] - tmp2.v[1];
    dz = tmp1.v[2] - tmp2.v[2];
    len = sqrt(dx * dx + dy * dy + dz * dz);
    if (len > 2.38419e-07f) {
        inv_len = 1.0 / len;
        detached->v.v[0] += (float)(dx * inv_len * 1.15942f);
        detached->v.v[1] += (float)(dy * inv_len * 1.15942f);
        detached->v.v[2] += (float)(dz * inv_len * 1.15942f);
    } else {
        detached->v.v[0] += 1.15942f;
    }
    detached->v.v[0] += pCar->collision_info->v.v[0];
    detached->v.v[1] += pCar->collision_info->v.v[1];
    detached->v.v[2] += pCar->collision_info->v.v[2];
#ifdef CARPOCALYPSE2_MATCHING
    /* Original falls through with whatever value happens to be in eax
     * (non-zero in practice); no explicit return to reproduce its codegen. */
    goto done;
fail:
    return 0;
done:;
#else
    return 1;
fail:
    return 0;
#endif
}

// FUNCTION: CARMA2_HW 0x00435d40
void C2_HOOK_FASTCALL DeleteFarCrushes(tCar_spec* pCar) {

    NOT_IMPLEMENTED();
}

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x00441420
void C2_HOOK_FASTCALL ApplyRandomDamageToCar(tCar_spec* pCar, int pMinDamage, int pMaxDamage) {
    int damage_type;
    int damage_count;

    damage_type = 0;
    damage_count = 0;
    for (;;) {
        DoDamage(pCar, (tDamage_type)damage_type, IRandomBetween(pMinDamage, pMaxDamage));
        if (damage_type == eDamage_engine && pCar->damage_units[eDamage_engine].damage_level < 0x50) {
            damage_count++;
            if (damage_count < 0x1e) {
                continue;
            }
        }
        damage_type++;
        if (damage_type >= 0xc) {
            pCar->damage_magnitude_accumulator -= gFloat_00589c40;
            return;
        }
    }
}
#pragma auto_inline(on)

// FUNCTION: CARMA2_HW 0x00441350
int C2_HOOK_FASTCALL GetAverageCrushLevel(tCar_spec* pCar) {
    int wheels_avg;
    int engine;
    int trans;
    int driver;
    int avg;
    int result;

    if (pCar->knackered) {
        return 0;
    }
    wheels_avg = pCar->damage_units[eDamage_lr_wheel].damage_level
        + pCar->damage_units[eDamage_rr_wheel].damage_level
        + pCar->damage_units[eDamage_rf_wheel].damage_level
        + pCar->damage_units[eDamage_lf_wheel].damage_level;
    engine = pCar->damage_units[eDamage_engine].damage_level;
    trans = pCar->damage_units[eDamage_transmission].damage_level;
    driver = pCar->damage_units[eDamage_driver].damage_level;
    wheels_avg /= 4;
    if (engine > 75 || trans > 75 || driver > 75 || wheels_avg > 75) {
        if (engine > trans && engine > driver && engine > wheels_avg) {
            return 100 - engine;
        }
        if (trans > driver && trans > wheels_avg) {
            return 100 - trans;
        }
        if (driver > wheels_avg) {
            return 100 - driver;
        }
        return 100 - wheels_avg;
    }
    avg = driver + wheels_avg;
    avg += trans;
    avg += engine;
    avg /= 4;
    result = 100 - avg;
    if (result >= 25) {
        return result;
    }
    return 25;
}

void C2_HOOK_FASTCALL CrushPanelWithForce(tCar_spec* pCar, tU32 pFlag_a, tU32 pFlag_b, tU32 pFlag_c, br_vector3* pVec, tU32 pFlag_d, tU32 pFlag_e) {

    NOT_IMPLEMENTED();
}

#pragma auto_inline(on)

// FUNCTION: CARMA2_HW 0x0043f930
void C2_HOOK_FASTCALL DoDamage(tCar_spec *pCar, tDamage_type pDamage_type, int pMagnitude) {
    tDamage_unit* damage_unit;
    int old_level;
    int new_level;

    if (pCar != NULL && pCar->driver >= eDriver_net_human && pDamage_type == eDamage_driver) {
        return;
    }
    if (pCar->invulnerable_no_damage || pMagnitude < 5) {
        return;
    }
    damage_unit = &pCar->damage_units[pDamage_type];
    old_level = damage_unit->damage_level;
    new_level = pMagnitude + old_level;
    damage_unit->damage_level = new_level;
    if (new_level >= 99) {
        if (pMagnitude < 10) {
            damage_unit->damage_level = old_level;
        } else {
            damage_unit->damage_level = 99;
        }
    }
    if (SetKnackeredFlag(pCar)) {
        return;
    }
    if (pCar == NULL || pCar->driver != eDriver_oppo) {
        if (gNet_mode == eNet_mode_none
            && ((pCar->damage_units[eDamage_engine].damage_level >= 99
                    && pCar->damage_units[eDamage_engine].last_level < 99
                    && pCar->damage_units[eDamage_transmission].last_level < 99)
                || (pCar->damage_units[eDamage_transmission].damage_level >= 99
                    && pCar->damage_units[eDamage_engine].last_level < 99
                    && pCar->damage_units[eDamage_transmission].last_level < 99))) {
            QueueOilSpill(pCar);
        }
    }
}

// C2PORT-NONMATCHING: best 52.75% | difftest PASS 2000 trials (spec difftest/specs/0043fb70_DamageCarInDirection.json) | reason compiler x87 scheduling, register allocation, and stack layout
// FUNCTION: CARMA2_HW 0x0043fb70
void C2_HOOK_FASTCALL DamageCarInDirection(tCar_spec* pCar, float* pPosition, float* pImpulse, int pCollision_with_car) {
    br_bounds3 bounds;
    float impact_strength;
    float x_from_min;
    float x_from_max;
    float y_from_min;
    float y_from_max;
    float z_from_min;
    float z_from_max;
    float nearest_x;
    float nearest_y;
    float nearest_z;
    float normalized_x;
    float normalized_y;
    float normalized_z;
    int impact_location;
    int clause_index;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, softness_factor, 0x18d0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, last_impact_location, 0x6f4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, damage_programs, 0x6f8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, last_impact_position, 0x13d8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, bb1, 0x24);

    impact_strength = (float)sqrt(pImpulse[0] * pImpulse[0]
        + pImpulse[2] * pImpulse[2]
        + pImpulse[1] * pImpulse[1]);
    if (impact_strength == gFloat_00589b64 && pCollision_with_car == 0) {
        return;
    }

    bounds = pCar->collision_info->bb1;
    x_from_min = pPosition[0] - bounds.min.v[0];
    x_from_max = bounds.max.v[0] - pPosition[0];
    impact_strength = (float)(pCar->softness_factor * impact_strength * gDouble_00589b68);
    nearest_x = x_from_min;
    if (x_from_min >= x_from_max) {
        nearest_x = x_from_max;
    }

    y_from_min = pPosition[1] - bounds.min.v[1];
    y_from_max = bounds.max.v[1] - pPosition[1];
    nearest_y = y_from_min;
    if (y_from_min >= y_from_max) {
        nearest_y = y_from_max;
    }

    z_from_min = pPosition[2] - bounds.min.v[2];
    z_from_max = bounds.max.v[2] - pPosition[2];
    nearest_z = z_from_max;
    if (z_from_min < z_from_max) {
        nearest_z = z_from_min;
    }

    if (nearest_x < nearest_z || nearest_y < nearest_z) {
        if (nearest_y < nearest_x || nearest_z < nearest_x) {
            impact_location = y_from_min < y_from_max ? eImpact_bottom : eImpact_top;
            normalized_x = x_from_min / (bounds.max.v[0] - bounds.min.v[0]);
            normalized_y = y_from_min;
        } else {
            impact_location = x_from_max <= x_from_min ? eImpact_right : eImpact_left;
            normalized_x = nearest_y;
            normalized_y = y_from_min / (bounds.max.v[1] - bounds.min.v[1]);
        }
        normalized_z = z_from_min / (bounds.max.v[2] - bounds.min.v[2]);
    } else {
        impact_location = z_from_max <= z_from_min ? eImpact_back : eImpact_front;
        normalized_x = x_from_min / (bounds.max.v[0] - bounds.min.v[0]);
        normalized_y = y_from_min / (bounds.max.v[1] - bounds.min.v[1]);
        normalized_z = x_from_min;
    }
    if (pCollision_with_car != 0 && pCar->last_impact_location == eImpact_unknown) {
        pCar->last_impact_position.v[0] = normalized_x;
        pCar->last_impact_position.v[1] = normalized_y;
        pCar->last_impact_position.v[2] = normalized_z;
        pCar->last_impact_location = (tImpact_location)impact_location;
    }
    if (impact_strength == gFloat_00589b64 || pCar->invulnerable_no_damage != 0) {
        return;
    }
    if (pCollision_with_car == 0 && impact_location == eImpact_bottom) {
        impact_strength *= gFloat_00589b70;
    }

    for (clause_index = 0; clause_index < pCar->damage_programs[impact_location].clause_count; clause_index++) {
        tDamage_clause* clause = &pCar->damage_programs[impact_location].clauses[clause_index];
        int condition_index;
        int conditions_pass = 1;

        for (condition_index = 0; condition_index < clause->condition_count && conditions_pass; condition_index++) {
            tDamage_condition* condition = &clause->conditions[condition_index];
            float position_component;

            if (condition->axis_comp == eAxis_x) {
                position_component = normalized_x;
            } else if (condition->axis_comp == eAxis_y) {
                position_component = normalized_y;
            } else if (condition->axis_comp == eAxis_z) {
                position_component = normalized_z;
            } else {
                continue;
            }
            if (condition->condition_operator == eCondition_less_than) {
                conditions_pass = position_component < condition->comparitor;
            } else if (condition->condition_operator == eCondition_greater_than) {
                conditions_pass = position_component > condition->comparitor;
            }
        }
        if (conditions_pass) {
            int effect_index;

            for (effect_index = 0; effect_index < clause->effect_count; effect_index++) {
                tDamage_effect* effect = &clause->effects[effect_index];
                float damage_amount;

                if (pCar == NULL || pCar->driver < eDriver_net_human) {
                    damage_amount = effect->weakness_factor * impact_strength;
                    damage_amount *= gFloat_00589b98;
                    damage_amount *= (float)(gDouble_00589b80
                        - ((gDouble_00589b80 - (double)gCurrent_race.index / gNumber_of_races)
                            * gDouble_00589b88 * gDouble_00589b90));
                    DoDamage(pCar, effect->type, (int)damage_amount);
                } else if (gNet_mode != eNet_mode_none) {
                    damage_amount = effect->weakness_factor * impact_strength;
                    damage_amount *= gFloat_00589b74;
                    DoDamage(pCar, effect->type, (int)damage_amount);
                } else {
                    damage_amount = effect->weakness_factor * impact_strength;
                    if (PercentageChance((int)(damage_amount * gFloat_00589b78))) {
                        DoDamage(pCar, effect->type, (int)(damage_amount * gFloat_00589b7c));
                    }
                }
            }
        }
    }

    if (pCar == NULL || pCar->driver != eDriver_local_human) {
        return;
    }
    switch (impact_location) {
    case eImpact_top:
    case eImpact_bottom:
        NewScreenWobble(
            FRandomBetween(impact_strength * gFloat_00589ba8, impact_strength * gFloat_00589ba4),
            FRandomBetween(impact_strength * gFloat_00589b7c, impact_strength * gFloat_00589bb8),
            FRandomBetween(gFloat_00589bb4 / impact_strength, gFloat_00589ba8 / impact_strength));
        return;
    case eImpact_left:
        NewScreenWobble(
            FRandomBetween(impact_strength * gFloat_00589bb0, impact_strength * gFloat_00589bac),
            FRandomBetween(impact_strength * gFloat_00589ba8, impact_strength * gFloat_00589ba4),
            FRandomBetween(gFloat_00589ba0 / impact_strength, gFloat_00589b9c / impact_strength));
        return;
    case eImpact_right:
        NewScreenWobble(
            -FRandomBetween(impact_strength * gFloat_00589bb0, impact_strength * gFloat_00589bac),
            FRandomBetween(impact_strength * gFloat_00589ba8, impact_strength * gFloat_00589ba4),
            FRandomBetween(gFloat_00589ba0 / impact_strength, gFloat_00589b9c / impact_strength));
        return;
    case eImpact_front:
        NewScreenWobble(
            FRandomBetween(impact_strength * gFloat_00589ba8, impact_strength * gFloat_00589ba4),
            FRandomBetween(impact_strength * gFloat_00589bb0, impact_strength * gFloat_00589bc0),
            FRandomBetween(gFloat_00589b9c / impact_strength, gFloat_00589bbc / impact_strength));
        return;
    case eImpact_back:
        NewScreenWobble(
            FRandomBetween(impact_strength * gFloat_00589ba8, impact_strength * gFloat_00589ba4),
            FRandomBetween(impact_strength * gFloat_00589bc4, impact_strength * gFloat_00589bc0),
            FRandomBetween(gFloat_00589b9c / impact_strength, gFloat_00589bbc / impact_strength));
        return;
    }
}

#define gCurrent_credit_car_index (*(int*)((char*)&gProgram_state + 0x20))

#define DO_CRASH_EARNINGS_I(pCar, pOffset)   (*(int*)((char*)(pCar) + (pOffset)))
#define DO_CRASH_EARNINGS_F(pCar, pOffset)   (*(float*)((char*)(pCar) + (pOffset)))

// FUNCTION: CARMA2_HW 0x00440660
int C2_HOOK_FASTCALL DoCrashEarnings(tCar_spec* pCar1, tCar_spec* pCar2) {
    tCar_spec* pVictim;
    tCar_spec* pAttacker;
    tCar_spec* pCar2next;
    tU32 time;
    int i;
    tCar_spec* pCar1next;
    int flag;
    int classification1;
    int classification2;
    int flag34;
    int pass;
    int flag3c;
    int multiplier;
    int flag44;
    int qualify1b;
    int qualify2b;
    int l50;
    int qualify1;
    int qualify2;
    int damage;
    int limit;
    int face1;
    int face2;
    int credit;
    float fpa;
    float fpb;
    float fpc;
    float dot;

    pCar2next = pCar2;
    pCar1next = pCar1;
    pVictim = NULL;
    pAttacker = NULL;
    flag3c = 0;
    i = 1;
    qualify1b = 0;
    qualify2b = 0;
    flag34 = 0;
    time = PDGetTotalTime();
    flag = 0;

    if (pCar1 != NULL && pCar1->driver > 5) {
        l50 = (int)pCar1->damage_magnitude_accumulator;
    } else {
        l50 = (int)0.0f;
    }

    flag44 = 0;
    if (pCar2 != NULL) {
        if (pCar2->driver > 5) {
            fpa = pCar2->damage_magnitude_accumulator;
        } else {
            fpa = 0.0f;
        }
        if (fpa != 0.0f) {
            flag44 = 1;
        }
    }

    if (pCar1 == NULL || pCar1->driver <= 5) {
        if (pCar2 == NULL || pCar2->driver <= 5) {
            return 0;
        }

        if (pCar1 != NULL && pCar1->driver != 0
            && (pCar1->driver == 4 || pCar1->driver == 2 || pCar1->driver == 3)
            && pCar1->field_0xc8 != NULL
            && ((tCar_spec*)pCar1->field_0xc8)->driver > 5
            && (tCar_spec*)pCar1->field_0xc8 != pCar2) {
            pCar2->time_last_hit = time;
            pCar2->last_hit_by = (tCar_spec*)pCar1->field_0xc8;
        }

        pCar1next = pCar2;
        pCar2next = NULL;
    } else if (pCar2 != NULL && pCar2->driver <= 5) {
        if (pCar2->driver != 0
            && (pCar2->driver == 4 || pCar2->driver == 2 || pCar2->driver == 3)
            && pCar2->field_0xc8 != NULL
            && ((tCar_spec*)pCar2->field_0xc8)->driver > 5
            && (tCar_spec*)pCar2->field_0xc8 != pCar1) {
            pCar1->time_last_hit = time;
            pCar1->last_hit_by = (tCar_spec*)pCar2->field_0xc8;
        }
        pCar2next = NULL;
    }

    pCar1 = pCar1next;
    pCar2 = pCar2next;

    if (DO_CRASH_EARNINGS_I(pCar1, 0x1d8) != 0) {
        goto return_check;
    }
    if (pCar2 != NULL) {
        if (DO_CRASH_EARNINGS_I(pCar2, 0x1d8) != 0) {
            goto return_check;
        }
        if (pCar2->damage_magnitude_accumulator > 0.00005f) {
            goto sequence;
        }
        if (pCar1->damage_magnitude_accumulator <= 0.00005f) {
            goto return_check;
        }
    }
sequence:

    classification1 = DO_CRASH_EARNINGS_I(pCar1, 0x6f4);
    if (classification1 == 2 || classification1 == 3 || classification1 == 0 || classification1 == 1) {
        if (pCar1->field_0x13e0 < 0.25) {
            classification1 = 4;
        } else if (pCar1->field_0x13e0 > 0.75) {
            classification1 = 5;
        }
    }

    fpa = fabsf(pCar1->pre_car_col_velocity_car_space.v[0]);
    fpb = fabsf(pCar1->pre_car_col_velocity_car_space.v[1]);
    fpc = fabsf(pCar1->pre_car_col_velocity_car_space.v[2]);
    if (fpa > fpb && fpa > fpc) {
        face1 = pCar1->pre_car_col_velocity_car_space.v[0] < 0.0f ? 2 : 3;
    } else if (fpb > fpa && fpb > fpc) {
        face1 = pCar1->pre_car_col_velocity_car_space.v[1] < 0.0f ? 1 : 0;
    } else {
        face1 = pCar1->pre_car_col_velocity_car_space.v[2] < 0.0f ? 4 : 5;
    }
    qualify1 = (classification1 == face1);

    if (pCar2 == NULL) {
        qualify2 = l50;
        goto one_car_only;
    }

    classification2 = DO_CRASH_EARNINGS_I(pCar2, 0x6f4);
    if (classification2 == 2 || classification2 == 3 || classification2 == 0 || classification2 == 1) {
        if (pCar2->field_0x13e0 < 0.25) {
            classification2 = 4;
        } else if (pCar2->field_0x13e0 > 0.75) {
            classification2 = 5;
        }
    }

    fpa = fabsf(pCar2->pre_car_col_velocity_car_space.v[0]);
    fpb = fabsf(pCar2->pre_car_col_velocity_car_space.v[1]);
    fpc = fabsf(pCar2->pre_car_col_velocity_car_space.v[2]);
    if (fpa > fpb && fpa > fpc) {
        face2 = pCar2->pre_car_col_velocity_car_space.v[0] < 0.0f ? 2 : 3;
    } else if (fpb > fpa && fpb > fpc) {
        face2 = pCar2->pre_car_col_velocity_car_space.v[1] < 0.0f ? 1 : 0;
    } else {
        face2 = pCar2->pre_car_col_velocity_car_space.v[2] < 0.0f ? 4 : 5;
    }
    qualify2 = (classification2 == face2);

    if (pCar1 == NULL || pCar1->driver < 7) {
        goto record_opponent;
    }

    if (pCar2 == NULL) {
        goto one_car_only;
    }

    if (qualify1
        && pCar1->pre_car_col_velocity_car_space.v[2] != 0.0f
        && ((DO_CRASH_EARNINGS_I(pCar1, 0x135c) > 0) ^ (pCar1->pre_car_col_velocity_car_space.v[2] > 0.0f))
        && (DO_CRASH_EARNINGS_I(pCar1, 0x12d0) & 0x40000 || DO_CRASH_EARNINGS_I(pCar1, 0x12dc) > 0x8000)) {
        pCar2->last_hit_by = pCar1;
        pCar2->time_last_hit = time;
    }
    goto after_record;

record_opponent:
    if (pCar2 == NULL) {
        goto one_car_only;
    }
    if (pCar2->driver < 7) {
        goto after_record;
    }
    if (qualify2
        && pCar2->pre_car_col_velocity_car_space.v[2] != 0.0f
        && ((DO_CRASH_EARNINGS_I(pCar2, 0x135c) > 0) ^ (pCar2->pre_car_col_velocity_car_space.v[2] > 0.0f))
        && (DO_CRASH_EARNINGS_I(pCar2, 0x12d0) & 0x40000 || DO_CRASH_EARNINGS_I(pCar2, 0x12dc) > 0x8000)) {
        pCar1->last_hit_by = pCar2;
        pCar1->time_last_hit = time;
    }

after_record:
    if (pCar2 != NULL) {
        goto followed_cars;
    }

one_car_only:
    if ((int)(time - pCar1->time_last_hit) >= 5000) {
        return 1;
    }
    pVictim = pCar1->last_hit_by;
    pAttacker = pCar1;
    i = 1;
    flag = 1;
    goto crash_sequence;

followed_cars:
    if (qualify1
        && pCar1->pre_car_col_speed * 5.0f > pCar2->pre_car_col_speed
        && pCar1->pre_car_col_speed > 0.0005f
        && (pCar1 == NULL || pCar1->driver < 7
            || (pCar1->pre_car_col_velocity_car_space.v[2] != 0.0f
                && ((DO_CRASH_EARNINGS_I(pCar1, 0x135c) > 0) ^ (pCar1->pre_car_col_velocity_car_space.v[2] > 0.0f))
                && (DO_CRASH_EARNINGS_I(pCar1, 0x12d0) & 0x40000 || DO_CRASH_EARNINGS_I(pCar1, 0x12dc) > 0x8000)))) {
        qualify1b = 1;
    }

    if (qualify2
        && pCar2->pre_car_col_speed * 5.0f > pCar1->pre_car_col_speed
        && pCar2->pre_car_col_speed > 0.0005f
        && (pCar2->pre_car_col_velocity_car_space.v[2] != 0.0f
            && ((DO_CRASH_EARNINGS_I(pCar2, 0x135c) > 0) ^ (pCar2->pre_car_col_velocity_car_space.v[2] > 0.0f))
            && (DO_CRASH_EARNINGS_I(pCar2, 0x12d0) & 0x40000 || DO_CRASH_EARNINGS_I(pCar2, 0x12dc) > 0x8000))) {
        qualify2b = 1;
    }

    if (gNet_mode == eNet_mode_none || qualify1b == 0 || qualify2b == 0) {
        if (qualify2b == 0 || pCar2->driver != 8) {
            goto dot_check;
        }
        qualify1b = 0;
        goto dot_check;
    }
    flag34 = 1;
    goto crash_sequence;

dot_check:
    if (qualify1b != 0) {
        pVictim = pCar1;

        pAttacker = pCar2;
    } else if (qualify2b != 0) {
        pVictim = pCar2;
        pAttacker = pCar1;
    } else {
        goto crash_sequence;
    }

    dot = pCar2->pre_car_col_direction.v[0] * pCar1->pre_car_col_direction.v[0]
        + pCar2->pre_car_col_direction.v[1] * pCar1->pre_car_col_direction.v[1]
        + pCar2->pre_car_col_direction.v[2] * pCar1->pre_car_col_direction.v[2];

    if (classification1 == 4
        && classification2 == 4
        && pCar1->pre_car_col_speed > 0.001f
        && pCar2->pre_car_col_speed > 0.001f
        && dot < -0.7f) {
        flag3c = 1;
        i = 2;
    } else {
        i = 1;
    }

crash_sequence:
    if (flag34 == 0) {
        if (pAttacker != NULL && pVictim != NULL && pVictim->driver >= 7) {
            goto record_twattage;
        }
        if (pCar2 != NULL) {
            if (pCar2->field_0x13f0 == (void*)pCar1
                && (int)(time - pCar2->time_last_victim) < 750) {
                pVictim = pCar1;
                pAttacker = pCar2;
                flag = 1;
                goto record_twattage;
            }
            if (pCar1->field_0x13f0 == (void*)pCar2) {
                if ((int)(time - pCar1->time_last_victim) < 750) {
                    pVictim = pCar2;
                    pAttacker = pCar1;
                    flag = 1;
                    goto record_twattage;
                }
                goto record_twattage;
            }
        }
        if ((int)(time - pCar1->time_last_victim) < 750) {
            pVictim = (tCar_spec*)pCar1->field_0x13f0;
            pAttacker = pCar1;
            flag = 1;
        }
    }

record_twattage:
    if (pVictim == NULL || pAttacker == NULL) {
        goto credits_loop_setup;
    }

    RecordOpponentTwattageOccurrence(pVictim, pAttacker);

    damage = 0;
    {
        int u;

        for (u = 0; u < 12; u++) {
            int new_level;

            if (pAttacker->damage_units[u].last_level < pAttacker->damage_units[u].damage_level) {
                new_level = (int)((double)pAttacker->damage_units[u].last_level
                    - (double)(pAttacker->damage_units[u].damage_level - pAttacker->damage_units[u].last_level) * -2.0);
                if (new_level > 99) {
                    new_level = 99;
                }
                pAttacker->damage_units[u].damage_level = new_level;
                damage += new_level - pAttacker->damage_units[u].last_level;
            }

            if (pVictim->damage_units[u].last_level < pVictim->damage_units[u].damage_level) {
                new_level = (int)((double)pVictim->damage_units[u].last_level
                    - (double)(pVictim->damage_units[u].damage_level - pVictim->damage_units[u].last_level) * -0.1);
                if (new_level < 0) {
                    new_level = 0;
                }
                pVictim->damage_units[u].damage_level = new_level;
            }
        }
    }

credits_loop_setup:
    limit = flag34 != 0 ? 2 : 1;
    pass = 0;
    goto credits_loop_test;

credits_pass_loop:
    if (flag34 != 0) {
        if (pass != 0) {
            pVictim = pCar1;
            pAttacker = pCar2;
        } else {
            pVictim = pCar2;
            pAttacker = pCar1;
        }
    }

    if (pVictim == NULL) {
        goto credits_pass_next;
    }
    if (pVictim->driver != 8 && gNet_mode == eNet_mode_none) {
        goto credits_pass_next;
    }

    if (pAttacker == NULL) {
        goto credits_pass_next;
    }

    SetKnackeredFlag(pAttacker);

    if (DO_CRASH_EARNINGS_I(pAttacker, 0x1d4) != 0 && DO_CRASH_EARNINGS_I(pAttacker, 0x1d8) == 0) {
        tNet_message* pMsg;

        DO_CRASH_EARNINGS_I(pAttacker, 0x1d8) = 1;

        if (gNet_mode == eNet_mode_none) {
            PratcamEvent(0x20);
            DoFancyHeadup(0x14);
            EarnCredits((int)((sqr(0.7 / (double)DO_CRASH_EARNINGS_F(pAttacker, 0x18d0))
                * (double)gCredits_wasting_car[gCurrent_credit_car_index] + 50.0) * 0.01) * 100);
            AwardTime(gTime_wasting_car[gCurrent_credit_car_index]);
        } else {
            pMsg = NetBuildGuaranteedMessage(0, 0x19);
            DO_CRASH_EARNINGS_I(pMsg, 0x1c) = DO_CRASH_EARNINGS_I(NetPlayerFromCar(pAttacker), 0x24);
            if (NetPlayerFromCar(pVictim) != NULL) {
                DO_CRASH_EARNINGS_I(pMsg, 0x20) = DO_CRASH_EARNINGS_I(NetPlayerFromCar(pVictim), 0x24);
            } else {
                DO_CRASH_EARNINGS_I(pMsg, 0x20) = 0xfffffffe;
            }
            NetGuaranteedSendMessageToEverybody(gCurrent_net_game, pMsg, 0);
        }
    }

    pAttacker->time_last_hit = time;
    pAttacker->last_hit_by = pVictim;

    if (flag == 0) {
        pAttacker->time_last_victim = time;
        pAttacker->field_0x13f0 = (void*)pVictim;
    }

    if (fabsf(pAttacker->collision_info->omega.v[0]) > 4.0f
        || fabsf(pAttacker->collision_info->omega.v[1]) > 6.0f
        || fabsf(pAttacker->collision_info->omega.v[2]) > 4.0f) {
        i += i;
    }

    if (DO_CRASH_EARNINGS_I(pCar1, 0x12e8) == 0) {
        volatile br_vector3 point;

        BrMatrix34ApplyP(&point, &pCar1->collision_info->cmpos, &pCar1->collision_info->transform_matrix);
        point.v[0] -= -0.15f;
    }
    if (pCar2 != NULL && DO_CRASH_EARNINGS_I(pCar2, 0x12e8) == 0) {
        volatile br_vector3 point;

        BrMatrix34ApplyP(&point, &pCar2->collision_info->cmpos, &pCar2->collision_info->transform_matrix);
        point.v[0] -= -0.15f;
    }

    damage = (int)((float)(0.7 / (double)DO_CRASH_EARNINGS_F(pAttacker, 0x18d0)) * damage);

    if (DO_CRASH_EARNINGS_I(pAttacker, 0x1cc) != 0) {
        goto credits_damage_shift;
    }
    if (gNet_mode != eNet_mode_none) {
        goto credits_damage_shift;
    }

    multiplier = i <= 8 ? i : 8;
    credit = (int)(((gCredits_per_unit_car_damage[gCurrent_credit_car_index] * (float)damage) * multiplier + 50.0f) * 0.01f) * 100;
    if (credit == 0 && DO_CRASH_EARNINGS_I(pAttacker, 0x1d4) == 0) {
        goto credits_damage_shift;
    }
    if (DO_CRASH_EARNINGS_I(pAttacker, 0x1d4) != 0) {
        goto credits_damage_shift;
    }

    if (credit >= 2000) {
        credit = 2000;
    }
    EarnCredits(credit);
    gLast_credit_award_time = time;

    credit = (int)((gSeconds_per_unit_car_damage[gCurrent_credit_car_index] * (float)damage + 2.5) * 0.2);
    credit *= 5;
    if (credit >= 90) {
        credit = 90;
    }
    AwardTime((tU32)credit);

    if (pCar2 != NULL) {
        if (flag3c != 0) {
            DoFancyHeadup(0x13);
        } else if (i > 2) {
            DoFancyHeadup(3);
        } else if (i > 1) {
            DoFancyHeadup(2);
        }
    }

credits_damage_shift:
    {
        int u;

        for (u = 0; u < 12; u++) {
            pAttacker->damage_units[u].last_level = pAttacker->damage_units[u].damage_level;
        }
    }

credits_pass_next:
    pass++;
    goto credits_loop_test;

credits_loop_test:
    if (pass < limit) {
        goto credits_pass_loop;
    }

    pCar1->damage_magnitude_accumulator = 0.0f;
    if (pCar2 != NULL) {
        pCar2->damage_magnitude_accumulator = 0.0f;
    }

return_check:
    if (l50 != 0 || (pCar2 != NULL && flag44 != 0)) {
        return 1;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00440230
void C2_HOOK_FASTCALL SortOutSmoke(tCar_spec* pCar) {
    int i;
    int flag;

    flag = 0;

    if (pCar == NULL || pCar->driver <= 5) {
        return;
    }

    for (i = 0; i < 12; i++) {
        int mult;
        int level;

        mult = gSmoke_part_table[i];
        if (mult == 0) {
            continue;
        }
        level = (99 - pCar->damage_units[i].damage_level) / mult;
        if (level <= 2) {
            flag = 1;
        }
        if (pCar->damage_units[i].damage_level != pCar->damage_units[i].smoke_last_level
            && mult
            && pCar->damage_units[i].damage_level > pCar->damage_units[i].smoke_last_level
            && mult > 0
            && ((99 - pCar->damage_units[i].smoke_last_level) / mult) != level
            && level <= 2) {
            ConditionalSmokeColumn(pCar, i, level);
        }
    }

    if (!flag && IsCarSmoking(pCar)) {
        StopCarSmoking(pCar);
    }

    for (i = 0; i < CARPOCALYPSE2_ASIZE(pCar->damage_units); i++) {
        pCar->damage_units[i].smoke_last_level = pCar->damage_units[i].damage_level;
    }
}

// C2PORT-NONMATCHING: best 54.07% | difftest PASS 2000 trials (spec difftest/specs/004f0d50_FUN_004f0d50.json) | reason x87-order-stack-layout
// FUNCTION: CARMA2_HW 0x004f0d50
int C2_HOOK_FASTCALL FUN_004f0d50(const float* pPoint, const float* pNormal, float pPlane_distance, const float* pVertex0, const float* pVertex1, const float* pVertex2) {
    float fVar1, fVar2, fVar3, fVar4, fVar5, fVar6, fVar7, fVar10;
    int iVar8, iVar11;
    unsigned int uVar9;
    volatile float local_c[3];

    pPlane_distance = pNormal[1] * pPoint[1] + (pNormal[2] * pPoint[2] + pNormal[0] * pPoint[0]) - pPlane_distance;
    if (fabsf(pPlane_distance) > gDOUBLE_0058af88) {
        return 0;
    }
    pPlane_distance = -pPlane_distance;
    local_c[0] = pNormal[0] * pPlane_distance + pPoint[0];
    local_c[1] = pNormal[1] * pPlane_distance + pPoint[1];
    local_c[2] = pNormal[2] * pPlane_distance + pPoint[2];
    uVar9 = (unsigned int)(fabsf(pNormal[0]) < fabsf(pNormal[1]));
    fVar7 = fabsf(pNormal[uVar9]);
    if (fVar7 < fabsf(pNormal[2])) {
        uVar9 = 2;
    }
    switch (uVar9) {
    case 0:
        iVar8 = 1;
        iVar11 = 2;
        break;
    case 1:
        iVar8 = 0;
        iVar11 = 2;
        break;
    case 2:
        iVar8 = 0;
        iVar11 = 1;
        break;
    }
    fVar1 = pVertex0[iVar8];
    fVar6 = pVertex1[iVar8] - fVar1;
    fVar2 = pVertex0[iVar11];
    fVar3 = pVertex1[iVar11] - fVar2;
    fVar5 = pVertex2[iVar11] - fVar2;
    fVar4 = pVertex2[iVar8] - fVar1;
    fVar1 = local_c[iVar8] - fVar1;
    fVar10 = local_c[iVar11] - fVar2;
    if (fabsf(fVar6) > gDOUBLE_0058af90) {
        fVar7 = fVar5 * fVar6 - fVar3 * fVar4;
        if (fVar7 == gFloat_0058aec0) {
            return 0;
        }
        fVar7 = (fVar10 * fVar6 - fVar3 * fVar1) / fVar7;
        fVar6 = (fVar1 - fVar7 * fVar4) / fVar6;
    } else {
        fVar7 = fVar1 / fVar4;
        fVar6 = (fVar10 - fVar7 * fVar5) / fVar3;
    }
    if (gDOUBLE_0058af98 <= fVar6 && gDOUBLE_0058af98 <= fVar7 && fVar7 + fVar6 <= gDOUBLE_0058afa0) {
        return 1;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004f3820
br_actor* C2_HOOK_FASTCALL FUN_004f3820(const void* pArg1, const br_vertex* pVertices, br_material* pMaterial, br_uint_16* pFace_indices, br_uint_16* pVertex_indices) {
    tU32 selected_time;
    tDecal* pDecal;
    tDecal* pOldest_decal;
    br_model* pModel;
    int vertex_offset;
    int i;
    int updated;
    br_face* pFace;
    br_uint_16 vertex;

    updated = 0;
    selected_time = GetTotalTime();
    pDecal = gDecals;
    pOldest_decal = gDecals;
    i = 0;
    do {
        if (pDecal->time == 0) {
            break;
        }
        if (pDecal->time < selected_time) {
            selected_time = pDecal->time;
            pOldest_decal = pDecal;
        }
        i++;
        pDecal++;
    } while (i < 50);
    if (i >= 50) {
        pDecal = pOldest_decal;
    }

    pModel = pDecal->actor->model;
    pModel->faces[0].material = pMaterial;
    pModel->faces[1].material = pMaterial;
    vertex_offset = 0;
    do {
        *(br_vertex*)((char*)pModel->vertices + vertex_offset) = *(const br_vertex*)((const char*)pVertices + vertex_offset);
        *(float*)((char*)pModel->vertices + vertex_offset + 4) -= gFloat_0058afb4;
        vertex_offset += 0x28;
    } while (vertex_offset < 0xa0);
    BrModelUpdate(pModel, 0x7fff);
    BrActorAdd(gOther_selfs[0], pDecal->actor);

    vertex_offset = 0;
    do {
        pFace = (br_face*)((char*)pModel->faces + vertex_offset);
        if (*(const float*)((const char*)pArg1 + 0x3c) * pFace->n.v[2]
                + pFace->n.v[0] * *(const float*)((const char*)pArg1 + 0x34)
                + *(const float*)((const char*)pArg1 + 0x38) * pFace->n.v[1] <= gFloat_0058aec0) {
            vertex = pFace->vertices[0];
            pFace->vertices[0] = pFace->vertices[1];
            pFace->vertices[1] = vertex;
            updated = 1;
        }
        vertex_offset += 0x28;
    } while (vertex_offset < 0x50);
    if (updated) {
        BrModelUpdate(pModel, 0x7fff);
    }

    pDecal->time = GetTotalTime();
    for (i = 0; i < 4; i++) {
        pVertex_indices[i] = (br_uint_16)i;
    }
    pFace_indices[2] = 3;
    pFace_indices[5] = 3;
    pFace_indices[0] = 0;
    pFace_indices[1] = 1;
    pFace_indices[3] = 0;
    pFace_indices[4] = 2;
    FUN_004c89f0(2);
    return pDecal->actor;
}

// C2PORT-NONMATCHING: best 69.33% | difftest PASS 2000 trials (spec difftest/specs/004f4c70_FUN_004f4c70.json) | reason struct-chain-codegen
// FUNCTION: CARMA2_HW 0x004f4c70
int C2_HOOK_FAKE_THISCALL FUN_004f4c70(void* pThis, void* pArg2, undefined4* pOut, int pFlag) {
    tU8* p1;
    tU8* yNode;
    tU8* zNode;
    tU8* wNode;
    tU16* puVar7;
    int iVar2;
    int iVar4;
    tU32 uVar5;
    tU32 uVar1;
    tU32 uVar9;
    int iVar3;

    p1 = (tU8*)pThis;
    memcpy(pOut, p1, 0x50);
    yNode = *(tU8**)(*(tU8**)(p1 + 0x48) + 0x18);
    if (*(short*)(p1 + 0x4c) < 0) {
        iVar2 = *(int*)(yNode + 8);
        puVar7 = *(tU16**)(yNode + 0xc);
        uVar5 = 0;
        uVar1 = *(tU16*)(yNode + 0x12);
        if (uVar1 != 0) {
            while ((pFlag == 0 && uVar5 == *(tU16*)(p1 + 0x4e)) ||
                   (iVar4 = FUN_004f0d50(pArg2, (const float*)((tU8*)puVar7 + 0x18), *(br_scalar*)((tU8*)puVar7 + 0x24),
                                         (const float*)(iVar2 + (tU32)puVar7[0] * 0x28),
                                         (const float*)(iVar2 + (tU32)puVar7[1] * 0x28),
                                         (const float*)(iVar2 + (tU32)puVar7[2] * 0x28)),
                    iVar4 == 0)) {
                uVar5++;
                puVar7 += 0x14;
                if ((int)uVar1 <= (int)uVar5) {
                    return 0;
                }
            }
            *(short*)((tU8*)pOut + 0x4e) = (short)uVar5;
            pOut[0xd] = *(undefined4*)(puVar7 + 0xc);
            pOut[0xe] = *(undefined4*)(puVar7 + 0xe);
            pOut[0xf] = *(undefined4*)(puVar7 + 0x10);
            pOut[0x11] = *(undefined4*)(puVar7 + 0x12);
            pOut[0] = *(undefined4*)(puVar7 + 4);
            return 1;
        }
    } else {
        iVar3 = (int)*(short*)(p1 + 0x4c);
        zNode = *(tU8**)(yNode + 0x4c);
        wNode = *(tU8**)(zNode + 0x18);
        uVar9 = 0;
        uVar5 = *(tU16*)(wNode + 0x1c + iVar3 * 0x24);
        iVar2 = *(int*)(wNode + 0x10 + iVar3 * 0x24);
        puVar7 = *(tU16**)(wNode + 4 + iVar3 * 0x24);
        if (uVar5 != 0) {
            while (uVar9 == *(tU16*)(p1 + 0x4e) ||
                   (iVar4 = FUN_004f0d50(pArg2, (const float*)((tU8*)puVar7 + 0xc), *(br_scalar*)((tU8*)puVar7 + 0x18),
                                         (const float*)((tU32)puVar7[0] * 0x20 + iVar2),
                                         (const float*)((tU32)puVar7[1] * 0x20 + iVar2),
                                         (const float*)((tU32)puVar7[2] * 0x20 + iVar2)),
                    iVar4 == 0)) {
                uVar9++;
                puVar7 += 0xe;
                if ((int)uVar5 <= (int)uVar9) {
                    return 0;
                }
            }
            *(short*)((tU8*)pOut + 0x4e) = (short)uVar9;
            pOut[0xd] = *(undefined4*)(puVar7 + 6);
            pOut[0xe] = *(undefined4*)(puVar7 + 8);
            pOut[0xf] = *(undefined4*)(puVar7 + 10);
            pOut[0x11] = *(undefined4*)(puVar7 + 0xc);
            return 1;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00440640
void C2_HOOK_FASTCALL CrashEarnings(tCar_spec* pCar1, tCar_spec* pCar2) {

    if (DoCrashEarnings(pCar1, pCar2)) {
        SortOutSmoke(pCar1);
        SortOutSmoke(pCar2);
    }
}

// FUNCTION: CARMA2_HW 0x00440350
void C2_HOOK_FASTCALL DoWheelDamage(tU32 pFrame_period) {
    int i;

    if (gAction_replay_mode && ARReplayIsReallyPaused()) {
        return;
    }

    for (i = 0; i < gNum_active_cars; i++) {
        tCar_spec* car;

        car = gActive_car_list[i];
        if ((car != NULL && car->driver > 5) && !(car->car_crush_spec != NULL && car->car_crush_spec->field_0x144)) {
            int j;

            for (j = 0; j < CARPOCALYPSE2_ASIZE(car->wheel_dam_offset); j++) {
                int damage;
                br_scalar y_amount;
                br_scalar z_amount;
                br_scalar wheel_circum;

                if (car->wheel_actors[j] == NULL) {
                    continue;
                }
                damage = car->damage_units[8 + j].damage_level;
                if (damage <= 30 || gRace_finished) {
                    car->wheel_dam_offset[j] = 0.f;
                    continue;
                }
                if (PointOutOfSight(&car->pos CARPOCALYPSE2_THISCALL_EDX, 32.f)) {
                    break;
                }
                y_amount = (damage - 30) * gWobble_spam_y[damage % 8];
                z_amount = (damage - 30) * gWobble_spam_z[damage % 8];
                BrMatrix34PreRotateY(&car->wheel_actors[j]->t.t.mat, BrDegreeToAngle(y_amount < 0 ? y_amount + 360.f : y_amount));
                BrMatrix34PreRotateZ(&car->wheel_actors[j]->t.t.mat, BrDegreeToAngle(z_amount < 0 ? z_amount + 360.f : z_amount));
                if (j < 2 && car->wheel_actors[4 + j] != NULL) {
                    BrMatrix34PreRotateY(&car->wheel_actors[4 + j]->t.t.mat, BrDegreeToAngle(y_amount < 0 ? y_amount + 360.f : y_amount));
                    BrMatrix34PreRotateZ(&car->wheel_actors[4 + j]->t.t.mat, BrDegreeToAngle(z_amount < 0 ? z_amount + 360.f : z_amount));
                }
                switch (j) {
                case 0:
                    if (car->driven_wheels_spin_ref_1 < 0) {
                        wheel_circum = car->non_driven_wheels_circum;
                    } else {
                        wheel_circum = car->driven_wheels_circum;
                    }
                    break;
                case 1:
                    if (car->driven_wheels_spin_ref_2 < 0) {
                        wheel_circum = car->non_driven_wheels_circum;
                    } else {
                        wheel_circum = car->driven_wheels_circum;
                    }
                    break;
                case 2:
                    if (car->driven_wheels_spin_ref_3 < 0) {
                        wheel_circum = car->non_driven_wheels_circum;
                    } else {
                        wheel_circum = car->driven_wheels_circum;
                    }
                    break;
                case 3:
                    if (car->driven_wheels_spin_ref_4 < 0) {
                        wheel_circum = car->non_driven_wheels_circum;
                    } else {
                        wheel_circum = car->driven_wheels_circum;
                    }
                    break;
                default:
                    abort();
                    break;
                }
                if (gNet_mode == eNet_mode_none || car->driver == eDriver_local_human) {
                    br_vector3 temp_vector;
                    br_vector3 wonky_vector;

                    BrVector3Set(&temp_vector, wheel_circum * gWheel_circ_to_width, 0.f, 0.f);
                    BrMatrix34ApplyV(&wonky_vector, &temp_vector, &car->wheel_actors[j]->t.t.mat);
                    car->wheel_dam_offset[j] = fabsf(wonky_vector.v[1]);
                }
            }
        }
    }
}

/* Per-triangle uv -> position mapping, as read by FUN_004f3740 / FUN_004f36c0 (size 0x30). */
typedef struct {
    float p0[3];            /* 0x00 position of the first vertex */
    float uv0[2];           /* 0x0c uv of the first vertex */
    float uv_apex[2];       /* 0x14 uv of the third vertex */
    float m[4];             /* 0x1c position->uv matrix (2x2) */
    br_face* face;          /* 0x2c */
} tSmash_uv_triangle;

/* Mapping info of a quad made of two triangles sharing the longest edge (size 0x84). */
typedef struct {
    int axis[3];                    /* 0x00 projection axes (a, b) and the dropped axis (c) */
    float plane[4];                 /* 0x0c face normal and plane distance */
    float uv_delta1[2];             /* 0x1c */
    tSmash_uv_triangle tri[2];      /* 0x24 */
} tSmash_uv_map;

/* The expected sizes are the 32-bit retail ones; skip them on 64-bit hosts. */
#if defined(_M_IX86) || defined(__i386__)
typedef char tSmash_uv_triangle_size_check[(sizeof(tSmash_uv_triangle) == 0x30) ? 1 : -1];
typedef char tSmash_uv_map_size_check[(sizeof(tSmash_uv_map) == 0x84) ? 1 : -1];
typedef char tSmashable_item_spec_size_check[(sizeof(tSmashable_item_spec) == 0x2e0) ? 1 : -1];
typedef char tSmashable_level_size_check[(sizeof(tSmashable_level) == 0x2c4) ? 1 : -1];
typedef char br_vertex_size_check[(sizeof(br_vertex) == 0x28) ? 1 : -1];
typedef char br_face_size_check[(sizeof(br_face) == 0x28) ? 1 : -1];
typedef char tFace_ref_size_check[(sizeof(tFace_ref) == 0x50) ? 1 : -1];
#endif

/* Fields of the smashable environment spec that are not (yet) named in tSmashable_item_spec. */
#define SMASH_SPEC_FACE_A(pSpec)    (*(br_face**)((tU8*)(pSpec) + 0x98))
#define SMASH_SPEC_FACE_B(pSpec)    (*(br_face**)((tU8*)(pSpec) + 0x9c))
#define SMASH_SPEC_FLAGS_A(pSpec)   (*((tU8*)(pSpec) + 0xa0))
#define SMASH_SPEC_FLAGS_B(pSpec)   (*((tU8*)(pSpec) + 0xa1))

/* Remember which edge of which face was cut (a spec keeps up to two faces). */

static void SmashSpecRecordEdgeFace(tSmashable_item_spec* pSpec, br_face* pFace, tU8 pEdge_flag) {

    if (SMASH_SPEC_FACE_A(pSpec) == pFace) {
        SMASH_SPEC_FLAGS_A(pSpec) |= pEdge_flag;
    } else if (SMASH_SPEC_FACE_B(pSpec) == pFace) {
        SMASH_SPEC_FLAGS_B(pSpec) |= pEdge_flag;
    }
    if (SMASH_SPEC_FACE_A(pSpec) == NULL) {
        SMASH_SPEC_FLAGS_A(pSpec) |= pEdge_flag;
        SMASH_SPEC_FACE_A(pSpec) = pFace;
    } else {
        SMASH_SPEC_FLAGS_B(pSpec) |= pEdge_flag;
        SMASH_SPEC_FACE_B(pSpec) = pFace;
    }
}

/* Mark an edge of a face: directly when the level already exists, otherwise remember it in the spec. */

static void SmashSpecMarkEdgeFace(tSmashable_item_spec* pSpec, int pAlready_marked, int pRecord, br_face* pFace, tU8 pEdge_flag) {

    if (pAlready_marked) {
        if (pFace != NULL) {
            pFace->flags |= pEdge_flag;
        }
    } else if (pRecord) {
        SmashSpecRecordEdgeFace(pSpec, pFace, pEdge_flag);
    }
}

/* Apply the edge flags remembered in the spec to the faces. Returns 0 when nothing was remembered. */

static int SmashSpecApplyRecordedEdgeFlags(tSmashable_item_spec* pSpec) {
    br_face* face_a;
    br_face* face_b;

    face_a = SMASH_SPEC_FACE_A(pSpec);
    face_b = SMASH_SPEC_FACE_B(pSpec);
    if (face_a == NULL && face_b == NULL) {
        return 0;
    }
    if (face_a != NULL) {
        face_a->flags |= SMASH_SPEC_FLAGS_A(pSpec);
    }
    face_b = SMASH_SPEC_FACE_B(pSpec);
    if (face_b != NULL) {
        face_b->flags |= SMASH_SPEC_FLAGS_B(pSpec);
    }
    return 1;
}

/* Which edge flag to set for the cut edge (0 = left, 1 = bottom, 2 = right, 3 = top). */

static void SmashSpecMarkCutEdge(tSmashable_item_spec* pSpec, int pAlready_marked, int pRecord, int pEdge, br_face* pCurrent_face, br_face* pAdjacent_face) {

    switch (pEdge) {
    case 0:
        SmashSpecMarkEdgeFace(pSpec, pAlready_marked, pRecord, pAdjacent_face, 4);
        break;
    case 1:
        SmashSpecMarkEdgeFace(pSpec, pAlready_marked, pRecord, pCurrent_face, 1);
        break;
    case 2:
        SmashSpecMarkEdgeFace(pSpec, pAlready_marked, pRecord, pCurrent_face, 2);
        break;
    case 3:
        SmashSpecMarkEdgeFace(pSpec, pAlready_marked, pRecord, pAdjacent_face, 2);
        break;
    }
}

/* Reorder a triangle so that its two first vertices form the longest edge. */

static void SmashOrderTriangleByLongestEdge(const tU8** pCorner) {
    const float* v0;
    const float* v1;
    const float* v2;
    const tU8* swap;
    float d01;
    float d02;
    float d12;

    v0 = (const float*)pCorner[0];
    v1 = (const float*)pCorner[1];
    v2 = (const float*)pCorner[2];
    d01 = (v0[0] - v1[0]) * (v0[0] - v1[0]) + (v0[2] - v1[2]) * (v0[2] - v1[2]) + (v0[1] - v1[1]) * (v0[1] - v1[1]);
    d02 = (v0[0] - v2[0]) * (v0[0] - v2[0]) + (v0[2] - v2[2]) * (v0[2] - v2[2]) + (v0[1] - v2[1]) * (v0[1] - v2[1]);
    d12 = (v1[0] - v2[0]) * (v1[0] - v2[0]) + (v1[2] - v2[2]) * (v1[2] - v2[2]) + (v1[1] - v2[1]) * (v1[1] - v2[1]);
    if (d02 > d01 && d02 > d12) {
        swap = pCorner[1];
        pCorner[1] = pCorner[2];
        pCorner[2] = swap;
    } else if (d12 > d01 && d12 > d02) {
        swap = pCorner[0];
        pCorner[0] = pCorner[2];
        pCorner[2] = swap;
    }
}

/* If the triangle contains both vertices of the edge (pIndex0, pIndex1), return its third vertex index, else -1. */

static int SmashFindTriangleApex(const br_uint_16* pVertices, int pIndex0, int pIndex1) {
    int a;
    int b;
    int c;

    a = pVertices[0];
    b = pVertices[1];
    c = pVertices[2];
    if ((a == pIndex0 && b == pIndex1) || (b == pIndex0 && a == pIndex1)) {
        return c;
    }
    if ((a == pIndex0 && c == pIndex1) || (c == pIndex0 && a == pIndex1)) {
        return b;
    }
    if ((b == pIndex0 && c == pIndex1) || (c == pIndex0 && b == pIndex1)) {
        return a;
    }
    return -1;
}


// C2PORT-NONMATCHING: best 20.49% | difftest PASS 2000 trials (spec difftest/specs/004f18d0_FUN_004f18d0.json) | reason unknown-source-shape, x87-order, stack-layout
// FUNCTION: CARMA2_HW 0x004f18d0
float C2_HOOK_FASTCALL FUN_004f18d0(tPhysics_object* pObject, void* pArg2_root, br_actor* pActor, tSmashable_item_spec* pSpec1, tSmashable_item_spec* pSpec2, undefined4* pArg2, float pArg3, br_vector3* pArg4, br_vector3* pArg5, br_vector3* pArg6, int pArg11, int pArg12, int* pFlag, int pArg14) {
    static const int kEdge_vertex_a[4] = { 0, 0, 1, 2 };
    static const int kEdge_vertex_b[4] = { 2, 1, 3, 3 };
    tFace_ref* face_ref;
    tFace_ref replacement_face_ref;
    tFace_ref neighbour_face_ref;
    tSmashable_item_spec* spec;
    tSmashable_item_spec* candidate;
    tSmashable_level* level;
    br_actor* level_actor;
    br_actor* child_actor;
    br_model* model;
    const br_vertex* child_vertices;
    const tU8* vertex_base;
    const tU8* corner[4];
    const float* p0;
    const float* p1;
    const float* p2;
    const float* p3;
    br_face* faces;
    br_face* current_face;
    br_face* adjacent_face;
    br_vector2 uv[4];
    br_vertex quad[4];
    tSmash_uv_map mapinfo;
    br_vector3 contact;
    br_vector3 nudged;
    float plane[4];
    float e1[3];
    float e2[3];
    float e3[3];
    float result;
    float dist;
    float abs_x;
    float abs_y;
    float abs_z;
    float du1;
    float dv1;
    float du2;
    float dv2;
    float du3;
    float dv3;
    float det;
    float neg_det;
    float delta_a;
    float delta_b;
    float v_hit;
    float u_floor;
    float v_floor;
    float u_ceil;
    float v_ceil;
    float u_min;
    float v_min;
    float u_max;
    float v_max;
    float u_center;
    float v_center;
    float mid[3];
    float opposite[3];
    double u_hit;
    tU8 edge_cut[4];
    tU8 edge_at_bounds[4];
    int depth;
    int already_marked;
    int use_bounds;
    int vertex_stride;
    int num_faces;
    int idx0;
    int idx1;
    int apex;
    int axis_a;
    int axis_b;
    int axis_c;
    int found;
    int opposite_edge;
    int a;
    int b;
    int n;
    int k;
    int i;
    int dummy_flag;

    face_ref = (tFace_ref*)pArg2;
    result = -1.0f;
    already_marked = 0;
    *pFlag = 0;
    depth = gINT_006a8290;
    gINT_006a8290 = depth + 1;
    if (depth >= 0x10
            || (pSpec2->mode == kSmashableMode_Decal) == ((pActor->model->flags >> 7) & 1)
            || gCount_track_smashable_environment_specs >= gCapacity_track_smashables
            || pSpec2->mode_data.texture_change.count_levels <= 1
            || (double)pArg3 < gDOUBLE_0058aef8
            || pSpec2->mode_data.texture_change.count_levels - 1 <= (int)pSpec2->mode_data.texture_change.field_0x4c) {
        return result;
    }

    dist = DistanceFromFace(pArg4, face_ref);
    contact.v[0] = face_ref->normal.v[0] * -dist + pArg4->v[0];
    contact.v[1] = face_ref->normal.v[1] * -dist + pArg4->v[1];
    contact.v[2] = face_ref->normal.v[2] * -dist + pArg4->v[2];

    if (pArg11 < 0 && pSpec1->field_0x2d8 != 0 && pSpec1->mode == kSmashableMode_TextureChange) {
        if (!FUN_004f4c70(face_ref, &contact, (undefined4*)&replacement_face_ref, 1)) {
            return gFLOAT_0058af20;
        }
        pArg2_root = replacement_face_ref.material;
        face_ref = &replacement_face_ref;
        pSpec1 = &gTrack_smashable_environment_specs[(tU8)replacement_face_ref.material->identifier[6] - 1];
        pSpec2 = pSpec1;
        pArg12 = (pSpec2->field_0x2dc == 0);
    }

    use_bounds = ((tSmashable_level*)pSpec2->mode_data.texture_change.levels)[1].flags & 2;
    model = face_ref->a->model;
    current_face = NULL;
    adjacent_face = NULL;

    /* Find the triangle that was hit and the neighbouring triangle sharing its longest edge. */
    if (face_ref->g < 0) {
        faces = model->faces;
        vertex_base = (const tU8*)model->vertices;
        vertex_stride = sizeof(br_vertex);
        current_face = &faces[(tU16)face_ref->f];
        num_faces = model->nfaces;
        for (k = 0; k < 3; k++) {
            corner[k] = vertex_base + current_face->vertices[k] * vertex_stride;
        }
        SmashOrderTriangleByLongestEdge(corner);
        idx0 = (int)(corner[0] - vertex_base) / vertex_stride;
        idx1 = (int)(corner[1] - vertex_base) / vertex_stride;
        apex = -1;
        for (n = 0; n < num_faces; n++) {
            br_face* candidate_face;

            candidate_face = &faces[n];
            if (candidate_face == current_face) {
                continue;
            }
            if (current_face->material != candidate_face->material) {
                continue;
            }
            apex = SmashFindTriangleApex(candidate_face->vertices, idx0, idx1);
            if (apex >= 0) {
                adjacent_face = candidate_face;
                break;
            }
        }
        if (n >= num_faces) {
            return gFLOAT_0058af20;
        }
        corner[3] = vertex_base + apex * vertex_stride;
        plane[0] = current_face->n.v[0];
        plane[1] = current_face->n.v[1];
        plane[2] = current_face->n.v[2];
        plane[3] = current_face->d;
    } else {
        v11group* group;
        v11face* group_faces;
        v11face* current_group_face;

        group = &model->prepared->groups[face_ref->g];
        vertex_base = (const tU8*)group->vertices;
        vertex_stride = sizeof(fmt_vertex);
        group_faces = group->faces;
        current_group_face = &group_faces[(tU16)face_ref->f];
        num_faces = group->nfaces;
        for (k = 0; k < 3; k++) {
            corner[k] = vertex_base + current_group_face->vertices[k] * vertex_stride;
        }
        SmashOrderTriangleByLongestEdge(corner);
        idx0 = (int)(corner[0] - vertex_base) / vertex_stride;
        idx1 = (int)(corner[1] - vertex_base) / vertex_stride;
        apex = -1;
        for (n = 0; n < num_faces; n++) {
            if (&group_faces[n] == current_group_face) {
                continue;
            }
            apex = SmashFindTriangleApex(group_faces[n].vertices, idx0, idx1);
            if (apex >= 0) {
                break;
            }
        }
        if (n >= num_faces) {
            return gFLOAT_0058af20;
        }
        corner[3] = vertex_base + apex * vertex_stride;
        plane[0] = current_group_face->eqn.v[0];
        plane[1] = current_group_face->eqn.v[1];
        plane[2] = current_group_face->eqn.v[2];
        plane[3] = current_group_face->eqn.v[3];
    }

    for (k = 0; k < 4; k++) {
        BrMatrix23ApplyP(&uv[k], (const br_vector2*)(corner[k] + 0xc), &face_ref->material->map_transform);
    }

    /* Project along the dominant axis of the face normal. */
    abs_x = fabsf(face_ref->normal.v[0]);
    abs_y = fabsf(face_ref->normal.v[1]);
    abs_z = fabsf(face_ref->normal.v[2]);
    if (abs_x > abs_y && abs_x > abs_z) {
        axis_a = 2;
        axis_b = 1;
        axis_c = 0;
    } else if (abs_y > abs_x && abs_y > abs_z) {
        axis_a = 0;
        axis_b = 2;
        axis_c = 1;
    } else {
        axis_a = 0;
        axis_b = 1;
        axis_c = 2;
    }

    p0 = (const float*)corner[0];
    p1 = (const float*)corner[1];
    p2 = (const float*)corner[2];
    p3 = (const float*)corner[3];
    e1[0] = p1[0] - p0[0];
    e1[1] = p1[1] - p0[1];
    e1[2] = p1[2] - p0[2];
    e2[0] = p2[0] - p0[0];
    e2[1] = p2[1] - p0[1];
    e2[2] = p2[2] - p0[2];
    du1 = uv[1].v[0] - uv[0].v[0];
    dv1 = uv[1].v[1] - uv[0].v[1];
    du2 = uv[2].v[0] - uv[0].v[0];
    dv2 = uv[2].v[1] - uv[0].v[1];

    mapinfo.axis[0] = axis_a;
    mapinfo.axis[1] = axis_b;
    mapinfo.axis[2] = axis_c;
    mapinfo.plane[0] = plane[0];
    mapinfo.plane[1] = plane[1];
    mapinfo.plane[2] = plane[2];
    mapinfo.plane[3] = plane[3];
    mapinfo.uv_delta1[0] = du1;
    mapinfo.uv_delta1[1] = dv1;

    det = e1[axis_a] * e2[axis_b] - e1[axis_b] * e2[axis_a];
    neg_det = e1[axis_b] * e2[axis_a] - e1[axis_a] * e2[axis_b];
    mapinfo.tri[0].p0[0] = p0[0];
    mapinfo.tri[0].p0[1] = p0[1];
    mapinfo.tri[0].p0[2] = p0[2];
    mapinfo.tri[0].uv0[0] = uv[0].v[0];
    mapinfo.tri[0].uv0[1] = uv[0].v[1];
    mapinfo.tri[0].uv_apex[0] = uv[2].v[0];
    mapinfo.tri[0].uv_apex[1] = uv[2].v[1];
    mapinfo.tri[0].m[0] = (e2[axis_b] * du1 - e1[axis_b] * du2) / det;
    mapinfo.tri[0].m[1] = (e2[axis_a] * du1 - e1[axis_a] * du2) / neg_det;
    mapinfo.tri[0].m[2] = (e2[axis_b] * dv1 - e1[axis_b] * dv2) / det;
    mapinfo.tri[0].m[3] = (e2[axis_a] * dv1 - e1[axis_a] * dv2) / neg_det;
    mapinfo.tri[0].face = current_face;

    e3[0] = p3[0] - p0[0];
    e3[1] = p3[1] - p0[1];
    e3[2] = p3[2] - p0[2];
    du3 = uv[3].v[0] - uv[0].v[0];
    dv3 = uv[3].v[1] - uv[0].v[1];
    det = e1[axis_a] * e3[axis_b] - e1[axis_b] * e3[axis_a];
    neg_det = e1[axis_b] * e3[axis_a] - e1[axis_a] * e3[axis_b];
    mapinfo.tri[1].p0[0] = p0[0];
    mapinfo.tri[1].p0[1] = p0[1];
    mapinfo.tri[1].p0[2] = p0[2];
    mapinfo.tri[1].uv0[0] = uv[0].v[0];
    mapinfo.tri[1].uv0[1] = uv[0].v[1];
    mapinfo.tri[1].uv_apex[0] = uv[3].v[0];
    mapinfo.tri[1].uv_apex[1] = uv[3].v[1];
    mapinfo.tri[1].m[0] = (e3[axis_b] * du1 - e1[axis_b] * du3) / det;
    mapinfo.tri[1].m[1] = (e3[axis_a] * du1 - e1[axis_a] * du3) / neg_det;
    mapinfo.tri[1].m[2] = (e3[axis_b] * dv1 - e1[axis_b] * dv3) / det;
    mapinfo.tri[1].m[3] = (e3[axis_a] * dv1 - e1[axis_a] * dv3) / neg_det;
    mapinfo.tri[1].face = adjacent_face;

    if (uv[3].v[0] == uv[2].v[0] && uv[3].v[1] == uv[2].v[1]) {
        return gFLOAT_0058af20;
    }

    /* Where in uv space was the surface hit, and which texel cell contains it? */
    delta_a = contact.v[axis_a] - mapinfo.tri[0].p0[axis_a];
    delta_b = contact.v[axis_b] - mapinfo.tri[0].p0[axis_b];
    u_hit = mapinfo.tri[0].m[0] * delta_a + mapinfo.tri[0].m[1] * delta_b + uv[0].v[0];
    v_hit = mapinfo.tri[0].m[2] * delta_a + mapinfo.tri[0].m[3] * delta_b + uv[0].v[1];
    u_floor = (float)floor(u_hit);
    v_floor = (float)floor((double)v_hit);
    u_ceil = (float)ceil(u_hit);
    v_ceil = (float)ceil((double)v_hit);

    u_min = uv[0].v[0];
    v_min = uv[0].v[1];
    u_max = uv[0].v[0];
    v_max = uv[0].v[1];
    for (k = 1; k < 4; k++) {
        if (uv[k].v[0] < u_min) {
            u_min = uv[k].v[0];
        }
        if (uv[k].v[1] < v_min) {
            v_min = uv[k].v[1];
        }
        if (uv[k].v[0] > u_max) {
            u_max = uv[k].v[0];
        }
        if (v_max < uv[k].v[1]) {
            v_max = uv[k].v[1];
        }
    }
    if (use_bounds) {
        u_floor = u_min;
        v_floor = v_min;
        u_ceil = u_max;
        v_ceil = v_max;
    }

    /* Clamp the cell to the uv range of the face. A cut edge is one that ends inside the face. */
    edge_cut[0] = 0;
    edge_cut[1] = 0;
    edge_cut[2] = 0;
    edge_cut[3] = 0;
    edge_at_bounds[0] = 0;
    edge_at_bounds[1] = 0;
    edge_at_bounds[2] = 0;
    edge_at_bounds[3] = 0;
    if (fabsf(u_floor - u_min) < gDOUBLE_0058afa8) {
        edge_at_bounds[0] = 1;
        u_floor = u_min;
    } else if (u_floor < u_min) {
        edge_at_bounds[0] = 1;
        u_floor = u_min;
        edge_cut[0] = 1;
    }
    if (fabsf(v_floor - v_min) < gDOUBLE_0058afa8) {
        edge_at_bounds[1] = 1;
        v_floor = v_min;
    } else if (v_floor < v_min) {
        edge_at_bounds[1] = 1;
        v_floor = v_min;
        edge_cut[1] = 1;
    }
    if (fabsf(u_ceil - u_max) < gDOUBLE_0058afa8) {
        edge_at_bounds[2] = 1;
        u_ceil = u_max;
    } else if (u_ceil > u_max) {
        edge_at_bounds[2] = 1;
        u_ceil = u_max;
        edge_cut[2] = 1;
    }
    if (fabsf(v_ceil - v_max) < gDOUBLE_0058afa8) {
        edge_at_bounds[3] = 1;
        v_ceil = v_max;
    } else if (v_ceil > v_max) {
        edge_at_bounds[3] = 1;
        v_ceil = v_max;
        edge_cut[3] = 1;
    }
    if (u_floor == u_ceil || v_floor == v_ceil) {
        return gFLOAT_0058af20;
    }

    /* Build the four corners of the cell: 0 = (min,min), 1 = (max,min), 2 = (min,max), 3 = (max,max). */
    quad[0].map.v[0] = u_floor;
    quad[0].map.v[1] = v_floor;
    quad[1].map.v[0] = u_ceil;
    quad[1].map.v[1] = v_floor;
    quad[2].map.v[0] = u_floor;
    quad[2].map.v[1] = v_ceil;
    quad[3].map.v[0] = u_ceil;
    quad[3].map.v[1] = v_ceil;
    for (k = 0; k < 4; k++) {
        quad[k].n.v[0] = face_ref->normal.v[0];
        quad[k].n.v[1] = face_ref->normal.v[1];
        quad[k].n.v[2] = face_ref->normal.v[2];
        quad[k].red = 0xff;
        quad[k].grn = 0xff;
        quad[k].blu = 0xff;
    }
    for (k = 0; k < 4; k++) {
        FUN_004f36c0((const int*)&mapinfo, &quad[k].map.v[0], &quad[k].p.v[0]);
    }

    u_center = (quad[3].map.v[0] + quad[2].map.v[0] + quad[1].map.v[0] + quad[0].map.v[0]) * gFloat_0058afb0;
    v_center = (quad[3].map.v[1] + quad[2].map.v[1] + quad[1].map.v[1] + quad[0].map.v[1]) * gFloat_0058afb0;
    for (k = 0; k < 4; k++) {
        i = (u_center <= quad[k].map.v[0]);
        if (v_center < quad[k].map.v[1]) {
            i |= 2;
        }
        gMatrix34_006b78a0.m[i][0] = quad[k].p.v[0];
        gMatrix34_006b78a0.m[i][1] = quad[k].p.v[1];
        gMatrix34_006b78a0.m[i][2] = quad[k].p.v[2];
    }
    gVector3_006a4688.v[0] = mapinfo.plane[0];
    gVector3_006a4688.v[1] = mapinfo.plane[1];
    gVector3_006a4688.v[2] = mapinfo.plane[2];
    gINT_006a3334 = 1;

    spec = pSpec2;
    level_actor = pActor;
    if (pSpec2->field_0x2dc == 0) {
        /* Reuse the spec of a previously created piece with the same geometry. */
        found = 0;
        for (candidate = (tSmashable_item_spec*)pSpec1->field_0x2d8; candidate != NULL && !found; candidate = (tSmashable_item_spec*)candidate->field_0x2d8) {
            child_actor = (br_actor*)candidate->field_0x2d4;
            if (child_actor != NULL) {
                child_vertices = child_actor->model->vertices;
                for (k = 0; k < 4; k++) {
                    if (fabsf(child_vertices[k].p.v[0] - quad[k].p.v[0]) >= gDOUBLE_0058afa8
                            || fabsf(child_vertices[k].p.v[1] - quad[k].p.v[1]) >= gDOUBLE_0058afa8
                            || fabsf(child_vertices[k].p.v[2] - quad[k].p.v[2]) >= gDOUBLE_0058afa8) {
                        break;
                    }
                }
                if (k == 4) {
                    pSpec2 = candidate;
                    level_actor = child_actor;
                    found = 1;
                }
            }
        }
        spec = pSpec2;

        if (!found && (spec->flags & 0x80000000) == 0) {
            /* Create a new spec for this piece, derived from the spec of the whole surface. */
            tSmashable_item_spec* clone;
            tSmashable_level* levels;

            clone = &gTrack_smashable_environment_specs[gCount_track_smashable_environment_specs];
            memcpy(clone, spec, sizeof(tSmashable_item_spec));
            if (spec->trigger_type != (tSmashable_trigger_type)-1
                    && (spec->mode == kSmashableMode_Decal || spec->mode == kSmashableMode_TextureChange)
                    && spec->mode_data.texture_change.levels != NULL) {
                int levels_size;

                levels_size = spec->mode_data.texture_change.count_levels * sizeof(tSmashable_level);
                levels = BrMemAllocate(levels_size, kMem_cloned_block);
                memcpy(levels, spec->mode_data.texture_change.levels, levels_size);
                clone->mode_data.texture_change.levels = levels;
            }
            if (clone->trigger_object.material != NULL) {
                clone->trigger_object.material = DRMaterialClone(clone->trigger_object.material, 0);
                clone->trigger_object.material->identifier = BrResStrDup(clone->trigger_object.material, spec->trigger_object.material->identifier);
                clone->mode_data.texture_change.trigger.material = clone->trigger_object.material;
                clone->flags |= 0x80000000;
                MungeSmashMaterialName(clone->trigger_object.material, &clone->trigger_object.material->identifier, (char)(clone - gTrack_smashable_environment_specs));
            }

            gCount_track_smashable_environment_specs++;
            clone->mode_data.texture_change.field_0x74 = 2;
            clone->mode_data.texture_change.field_0x78 = (undefined4)(uintptr_t)BrMemAllocate(12, kMem_smash_poly_array);
            clone->mode_data.texture_change.field_0x6c = 4;
            clone->mode_data.texture_change.field_0x70 = (undefined4)(uintptr_t)BrMemAllocate(8, kMem_smash_vertex_array);
            SMASH_SPEC_FACE_A(clone) = NULL;
            SMASH_SPEC_FACE_B(clone) = NULL;
            clone->field_0x2dc = 1;
            FogAccordingToGPSCDE(clone->trigger_object.material);

            if (clone->mode == kSmashableMode_Decal) {
                level_actor = FUN_004f3820(face_ref, quad, clone->trigger_object.material,
                    (br_uint_16*)clone->mode_data.texture_change.field_0x78,
                    (br_uint_16*)clone->mode_data.texture_change.field_0x70);
                clone->field_0x2d4 = (undefined4)(uintptr_t)level_actor;
            } else {
                if (!FUN_004f39a0((tU8*)&mapinfo, (tU8*)face_ref->a, (float*)quad, edge_at_bounds,
                        (tU32*)corner, (tU32)(uintptr_t)clone->trigger_object.material,
                        (tU16*)clone->mode_data.texture_change.field_0x78,
                        (tS16*)clone->mode_data.texture_change.field_0x70)) {
                    FreeEnvSmash(clone);
                    gCount_track_smashable_environment_specs--;
                    return gFLOAT_0058af20;
                }
                pObject->box_face_ref = (tU16)(gFace_num__car - 2);
                pObject->box_face_start = 0;
                pObject->box_face_end = 0;
            }
            clone->field_0x2d8 = spec->field_0x2d8;
            spec->field_0x2d8 = (undefined4)(uintptr_t)clone;
            spec = clone;
        }
    }

    /* Which level of the spec applies to the hit, and how should the edges of the cell be treated? */
    level = FUN_004ee290(NULL, pObject, &contact, pArg6, level_actor, (undefined4)(uintptr_t)pSpec1,
        (tCar_crush_smashable_part*)&spec->mode_data, pArg3, spec->mode != kSmashableMode_Decal, pArg14);
    already_marked = (level != NULL && level->collision_type == kTextureLevelCollisionChange_Edges);
    if (level != NULL && (level->collision_type == kTextureLevelCollisionChange_PassThrough
            || level->collision_type == kTextureLevelCollisionChange_Edges)) {
        result = level->trigger_threshold;
    }
    *pFlag = 1;

    if (pArg11 >= 0) {
        if (already_marked && SmashSpecApplyRecordedEdgeFlags(spec)) {
            /* flags were applied from the spec */
        } else if (pArg11 <= 3) {
            SmashSpecMarkCutEdge(spec, already_marked, pArg12, pArg11, current_face, adjacent_face);
        }
    }

    /* Continue on the other side of every cut edge (except the one we came through). */
    for (i = 0; i < 4; i++) {
        if (i == pArg11 || !edge_cut[i]) {
            continue;
        }
        a = kEdge_vertex_a[i];
        b = kEdge_vertex_b[i];
        mid[0] = (float)((quad[a].p.v[0] + quad[b].p.v[0]) * gDOUBLE_0058af88);
        mid[1] = (float)((quad[a].p.v[1] + quad[b].p.v[1]) * gDOUBLE_0058af88);
        mid[2] = (float)((quad[a].p.v[2] + quad[b].p.v[2]) * gDOUBLE_0058af88);
        opposite_edge = (i + 2) & 3;
        a = kEdge_vertex_a[opposite_edge];
        b = kEdge_vertex_b[opposite_edge];
        opposite[0] = (float)((quad[a].p.v[0] + quad[b].p.v[0]) * gDOUBLE_0058af88);
        opposite[1] = (float)((quad[a].p.v[1] + quad[b].p.v[1]) * gDOUBLE_0058af88);
        opposite[2] = (float)((quad[a].p.v[2] + quad[b].p.v[2]) * gDOUBLE_0058af88);
        nudged.v[0] = (float)((mid[0] - opposite[0]) * gDouble_0058aee8 + mid[0]);
        nudged.v[1] = (float)((mid[1] - opposite[1]) * gDouble_0058aee8 + mid[1]);
        nudged.v[2] = (float)((mid[2] - opposite[2]) * gDouble_0058aee8 + mid[2]);

        if (!FUN_004f4c70(face_ref, &nudged, (undefined4*)&neighbour_face_ref, 0)) {
            continue;
        }
        if (already_marked) {
            if (!SmashSpecApplyRecordedEdgeFlags(spec)) {
                SmashSpecMarkCutEdge(spec, already_marked, pArg12, i, current_face, adjacent_face);
            }
        } else {
            SmashSpecMarkCutEdge(spec, already_marked, pArg12, i, current_face, adjacent_face);
        }
        if (pSpec2->field_0x2dc != 0) {
            for (n = 0; n < gCount_track_smashable_environment_specs; n++) {
                candidate = &gTrack_smashable_environment_specs[n];
                if (candidate->trigger_type == kSmashableTrigger_Material
                        && candidate->trigger_object.material == neighbour_face_ref.material
                        && (candidate->mode == kSmashableMode_TextureChange || candidate->mode == kSmashableMode_Decal)) {
                    pSpec2 = candidate;
                    break;
                }
            }
        }
        FUN_004f18d0(pObject, pArg2_root, pActor, pSpec1, pSpec2, (undefined4*)&neighbour_face_ref,
            pArg3, &nudged, pArg5, pArg6, opposite_edge, pArg12, &dummy_flag, pArg14);
    }
    return result;
}

// C2PORT-NONMATCHING: best 16.83% | difftest PASS 2000 trials (spec difftest/specs/004f39a0_FUN_004f39a0.json) | reason VC5 stack and register codegen
// FUNCTION: CARMA2_HW 0x004f39a0
int C2_HOOK_FASTCALL FUN_004f39a0(tU8* pContext, tU8* pShape_context, float* pVertex_data, tU8* pClip_flags, tU32* pCorner_vertex_addresses, tU32 pSurface_token, tU16* pQuad_indices, tS16* pOutput_indices) {
  short *psVar1;
  short *psVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  float fVar8;
  float fVar9;
  float fVar10;
  float fVar11;
  float fVar12;
  float fVar13;
  float fVar14;
  tU16 uVar15;
  tU32 uVar16;
  tU32 uVar17;
  tU32 uVar18;
  float *pfVar19;
  int iVar20;
  tU32 uVar21;
  int iVar22;
  float **ppfVar23;
  int iVar24;
  float *pfVar25;
  tU32 *puVar26;
  int iVar27;
  float *pfVar28;
  short *psVar29;
  float local_58 [10];
  float *local_30 [4];
  float *local_20 [4];
  float *local_10;
  float *local_c;
  float *local_8;
  float *local_4;

  iVar22 = *(int *)(pShape_context + 0x18);
  *pQuad_indices = 0;
  pQuad_indices[1] = 1;
  pQuad_indices[2] = 3;
  pQuad_indices[3] = 0;
  pQuad_indices[4] = 3;
  pQuad_indices[5] = 2;
  ppfVar23 = local_30;
  for (iVar20 = 4; iVar20 != 0; iVar20 = iVar20 + -1) {
    *ppfVar23 = (float *)0x0;
    ppfVar23 = ppfVar23 + 1;
  }
  uVar17 = BrResSize(*(void **)(iVar22 + 8));
  uVar17 = uVar17 / 0x28;
  uVar18 = BrResSize(*(void **)(iVar22 + 0xc));
  uVar18 = uVar18 / 0x28;
  fVar3 = *(float *)(pCorner_vertex_addresses[1] + 0xc);
  fVar4 = *(float *)(pCorner_vertex_addresses[3] + 0xc);
  fVar5 = *(float *)(pCorner_vertex_addresses[2] + 0xc);
  fVar6 = *(float *)(*pCorner_vertex_addresses + 0xc);
  fVar7 = *(float *)(pCorner_vertex_addresses[2] + 0x10);
  fVar8 = *(float *)(pCorner_vertex_addresses[3] + 0x10);
  iVar20 = 4;
  fVar9 = *(float *)(pCorner_vertex_addresses[1] + 0x10);
  fVar10 = *(float *)(*pCorner_vertex_addresses + 0x10);
  do {
    pfVar19 = (float *)*pCorner_vertex_addresses;
    uVar21 = (tU32)((fVar3 + fVar4 + fVar5 + fVar6) * gFloat_0058afb0 <= pfVar19[3]);
    if ((fVar7 + fVar8 + fVar9 + fVar10) * gFloat_0058afb0 < pfVar19[4]) {
      uVar21 = uVar21 | 2;
    }
    pCorner_vertex_addresses = pCorner_vertex_addresses + 1;
    iVar20 = iVar20 + -1;
    local_30[uVar21] = pfVar19;
    pfVar19 = local_30[0];
  } while (iVar20 != 0);
  iVar20 = 0;
  ppfVar23 = local_30;
  do {
    if (*ppfVar23 == (float *)0x0) {
      return 0;
    }
    iVar20 = iVar20 + 1;
    ppfVar23 = ppfVar23 + 1;
  } while (iVar20 < 4);
  if ((pClip_flags[1] & pClip_flags[3] & pClip_flags[2] & *pClip_flags) == 0) {
    if ((*pClip_flags == 0) || (pClip_flags[1] == 0)) {
      uVar15 = *(tU16 *)(iVar22 + 0x10);
      if (uVar17 <= uVar15) {
        return 0;
      }
      local_20[0] = (float *)(*(int *)(iVar22 + 8) + (tU32)uVar15 * 0x28);
      *(tU16 *)(iVar22 + 0x10) = uVar15 + 1;
      pfVar25 = pVertex_data;
      pfVar28 = local_20[0];
      for (iVar20 = 10; iVar20 != 0; iVar20 = iVar20 + -1) {
        *pfVar28 = *pfVar25;
        pfVar25 = pfVar25 + 1;
        pfVar28 = pfVar28 + 1;
      }
    }
    else {
      local_20[0] = local_30[0];
    }
    if ((pClip_flags[2] == 0) || (pClip_flags[1] == 0)) {
      uVar15 = *(tU16 *)(iVar22 + 0x10);
      if (uVar17 <= uVar15) {
        return 0;
      }
      local_20[1] = (float *)(*(int *)(iVar22 + 8) + (tU32)uVar15 * 0x28);
      *(tU16 *)(iVar22 + 0x10) = uVar15 + 1;
      pfVar25 = pVertex_data + 10;
      pfVar28 = local_20[1];
      for (iVar20 = 10; iVar20 != 0; iVar20 = iVar20 + -1) {
        *pfVar28 = *pfVar25;
        pfVar25 = pfVar25 + 1;
        pfVar28 = pfVar28 + 1;
      }
    }
    else {
      local_20[1] = local_30[1];
    }
    if ((pClip_flags[3] == 0) || (*pClip_flags == 0)) {
      uVar15 = *(tU16 *)(iVar22 + 0x10);
      if (uVar17 <= uVar15) {
        return 0;
      }
      local_20[2] = (float *)(*(int *)(iVar22 + 8) + (tU32)uVar15 * 0x28);
      *(tU16 *)(iVar22 + 0x10) = uVar15 + 1;
      pfVar25 = pVertex_data + 0x14;
      pfVar28 = local_20[2];
      for (iVar20 = 10; iVar20 != 0; iVar20 = iVar20 + -1) {
        *pfVar28 = *pfVar25;
        pfVar25 = pfVar25 + 1;
        pfVar28 = pfVar28 + 1;
      }
    }
    else {
      local_20[2] = local_30[2];
    }
    if ((pClip_flags[2] == 0) || (pClip_flags[3] == 0)) {
      uVar15 = *(tU16 *)(iVar22 + 0x10);
      if (uVar17 <= uVar15) {
        return 0;
      }
      local_20[3] = (float *)(*(int *)(iVar22 + 8) + (tU32)uVar15 * 0x28);
      *(tU16 *)(iVar22 + 0x10) = uVar15 + 1;
      pfVar25 = pVertex_data + 0x1e;
      pfVar28 = local_20[3];
      for (iVar20 = 10; iVar20 != 0; iVar20 = iVar20 + -1) {
        *pfVar28 = *pfVar25;
        pfVar25 = pfVar25 + 1;
        pfVar28 = pfVar28 + 1;
      }
    }
    else {
      local_20[3] = local_30[3];
    }
    if (pClip_flags[1] == 0) {
      pfVar25 = local_20[1];
      pfVar28 = local_58;
      for (iVar20 = 10; iVar20 != 0; iVar20 = iVar20 + -1) {
        *pfVar28 = *pfVar25;
        pfVar25 = pfVar25 + 1;
        pfVar28 = pfVar28 + 1;
      }
      local_58[4] = (local_30[1][4] - pfVar19[4]) *
                    ((local_58[3] - pfVar19[3]) / (local_30[1][3] - pfVar19[3])) + pfVar19[4];
      FUN_004f36c0((const int*)pContext, &local_58[3], local_58);
      uVar15 = *(tU16 *)(iVar22 + 0x10);
      if (uVar17 <= uVar15) {
        return 0;
      }
      local_10 = (float *)(*(int *)(iVar22 + 8) + (tU32)uVar15 * 0x28);
      *(tU16 *)(iVar22 + 0x10) = uVar15 + 1;
      pfVar19 = local_58;
      pfVar25 = local_10;
      for (iVar20 = 10; iVar20 != 0; iVar20 = iVar20 + -1) {
        *pfVar25 = *pfVar19;
        pfVar19 = pfVar19 + 1;
        pfVar25 = pfVar25 + 1;
      }
    }
    else {
      local_10 = local_20[1];
    }
    pfVar19 = local_30[1];
    if (pClip_flags[2] == 0) {
      pfVar25 = local_20[3];
      pfVar28 = local_58;
      for (iVar20 = 10; iVar20 != 0; iVar20 = iVar20 + -1) {
        *pfVar28 = *pfVar25;
        pfVar25 = pfVar25 + 1;
        pfVar28 = pfVar28 + 1;
      }
      local_58[3] = (local_30[3][3] - pfVar19[3]) *
                    ((local_58[4] - pfVar19[4]) / (local_30[3][4] - pfVar19[4])) + pfVar19[3];
      FUN_004f36c0((const int*)pContext, &local_58[3], local_58);
      uVar15 = *(tU16 *)(iVar22 + 0x10);
      if (uVar17 <= uVar15) {
        return 0;
      }
      local_c = (float *)(*(int *)(iVar22 + 8) + (tU32)uVar15 * 0x28);
      *(tU16 *)(iVar22 + 0x10) = uVar15 + 1;
      pfVar19 = local_58;
      pfVar25 = local_c;
      for (iVar20 = 10; iVar20 != 0; iVar20 = iVar20 + -1) {
        *pfVar25 = *pfVar19;
        pfVar19 = pfVar19 + 1;
        pfVar25 = pfVar25 + 1;
      }
    }
    else {
      local_c = local_20[3];
    }
    pfVar19 = local_30[3];
    if (pClip_flags[3] == 0) {
      pfVar25 = local_20[2];
      pfVar28 = local_58;
      for (iVar20 = 10; iVar20 != 0; iVar20 = iVar20 + -1) {
        *pfVar28 = *pfVar25;
        pfVar25 = pfVar25 + 1;
        pfVar28 = pfVar28 + 1;
      }
      local_58[4] = (local_30[2][4] - pfVar19[4]) *
                    ((local_58[3] - pfVar19[3]) / (local_30[2][3] - pfVar19[3])) + pfVar19[4];
      FUN_004f36c0((const int*)pContext, &local_58[3], local_58);
      uVar15 = *(tU16 *)(iVar22 + 0x10);
      if (uVar17 <= uVar15) {
        return 0;
      }
      local_8 = (float *)(*(int *)(iVar22 + 8) + (tU32)uVar15 * 0x28);
      *(tU16 *)(iVar22 + 0x10) = uVar15 + 1;
      pfVar19 = local_58;
      pfVar25 = local_8;
      for (iVar20 = 10; iVar20 != 0; iVar20 = iVar20 + -1) {
        *pfVar25 = *pfVar19;
        pfVar19 = pfVar19 + 1;
        pfVar25 = pfVar25 + 1;
      }
    }
    else {
      local_8 = local_20[2];
    }
    pfVar19 = local_30[2];
    if (*pClip_flags == 0) {
      pfVar25 = local_20[0];
      pfVar28 = local_58;
      for (iVar20 = 10; iVar20 != 0; iVar20 = iVar20 + -1) {
        *pfVar28 = *pfVar25;
        pfVar25 = pfVar25 + 1;
        pfVar28 = pfVar28 + 1;
      }
      local_58[3] = (local_30[0][3] - pfVar19[3]) *
                    ((local_58[4] - pfVar19[4]) / (local_30[0][4] - pfVar19[4])) + pfVar19[3];
      FUN_004f36c0((const int*)pContext, &local_58[3], local_58);
      uVar15 = *(tU16 *)(iVar22 + 0x10);
      if (uVar17 <= uVar15) {
        return 0;
      }
      local_4 = (float *)(*(int *)(iVar22 + 8) + (tU32)uVar15 * 0x28);
      *(tU16 *)(iVar22 + 0x10) = uVar15 + 1;
      pfVar19 = local_58;
      pfVar25 = local_4;
      for (iVar20 = 10; iVar20 != 0; iVar20 = iVar20 + -1) {
        *pfVar25 = *pfVar19;
        pfVar19 = pfVar19 + 1;
        pfVar25 = pfVar25 + 1;
      }
    }
    else {
      local_4 = local_20[0];
    }
    uVar16 = *(tU32 *)(*(int *)(pContext + 0x50) + 8);
    *(tU32 *)(*(int *)(pContext + 0x50) + 8) = pSurface_token;
    *(tU8 *)(*(int *)(pContext + 0x50) + 0x10) = 0;
    iVar20 = (int)local_20[0] - *(int *)(iVar22 + 8);
    **(short **)(pContext + 0x50) =
         (short)(iVar20 / 0x28);
    iVar20 = (int)local_20[1] - *(int *)(iVar22 + 8);
    *(short *)(*(int *)(pContext + 0x50) + 2) =
         (short)(iVar20 / 0x28);
    iVar20 = (int)local_20[3] - *(int *)(iVar22 + 8);
    *(short *)(*(int *)(pContext + 0x50) + 4) =
         (short)(iVar20 / 0x28);
    *(tU32 *)(*(int *)(pContext + 0x80) + 8) = pSurface_token;
    *(tU8 *)(*(int *)(pContext + 0x80) + 0x10) = 0;
    iVar20 = (int)local_20[0] - *(int *)(iVar22 + 8);
    **(short **)(pContext + 0x80) =
         (short)(iVar20 / 0x28);
    iVar20 = (int)local_20[3] - *(int *)(iVar22 + 8);
    *(short *)(*(int *)(pContext + 0x80) + 2) =
         (short)(iVar20 / 0x28);
    iVar20 = (int)local_20[2] - *(int *)(iVar22 + 8);
    *(short *)(*(int *)(pContext + 0x80) + 4) =
         (short)(iVar20 / 0x28);
    *(tU8 *)(*(int *)(pContext + 0x50) + 0x10) = 4;
    *(tU8 *)(*(int *)(pContext + 0x80) + 0x10) = 1;
    ppfVar23 = local_20;
    pfVar19 = (float *)&gMatrix34_006b78a0;
    do {
      pfVar25 = *ppfVar23;
      pfVar28 = pfVar19 + 3;
      ppfVar23 = ppfVar23 + 1;
      *pfVar19 = *pfVar25;
      pfVar19[1] = pfVar25[1];
      pfVar19[2] = pfVar25[2];
      pfVar19 = pfVar28;
    } while ((tU32)pfVar28 < (tU32)((tU8*)&gMatrix34_006b78a0 + sizeof(gMatrix34_006b78a0)));
    gVector3_006a4688.v[0] = *(float *)(pContext + 0xc);
    gVector3_006a4688.v[1] = *(float *)(pContext + 0x10);
    gVector3_006a4688.v[2] = *(float *)(pContext + 0x14);
    gINT_006a3334 = 1;
    if (*pClip_flags == 0) {
      uVar15 = *(tU16 *)(iVar22 + 0x12);
      if (uVar18 <= uVar15 + 1) {
        return 0;
      }
      puVar26 = *(tU32 **)(iVar22 + 0xc);
      *(tU16 *)(iVar22 + 0x12) = uVar15 + 2;
      psVar1 = (short *)(puVar26 + (tU32)uVar15 * 10);
      psVar2 = (short *)(puVar26 + (tU32)(tU16)(uVar15 + 1) * 10);
      psVar29 = psVar1;
      for (iVar20 = 10; iVar20 != 0; iVar20 = iVar20 + -1) {
        *(tU32 *)psVar29 = *puVar26;
        puVar26 = puVar26 + 1;
        psVar29 = psVar29 + 2;
      }
      puVar26 = *(tU32 **)(iVar22 + 0xc);
      psVar29 = psVar2;
      for (iVar20 = 10; iVar20 != 0; iVar20 = iVar20 + -1) {
        *(tU32 *)psVar29 = *puVar26;
        puVar26 = puVar26 + 1;
        psVar29 = psVar29 + 2;
      }
      fVar3 = local_4[1];
      fVar4 = local_4[2];
      fVar5 = local_20[0][1];
      fVar6 = local_20[0][2];
      fVar7 = local_8[1];
      fVar8 = local_8[2];
      fVar9 = local_30[2][1];
      fVar10 = local_30[2][2];
      fVar11 = *local_4;
      fVar12 = *local_8;
      fVar13 = *local_20[0];
      fVar14 = *local_30[2];
      *(tU8 *)(psVar1 + 8) = 0;
      *(tU32 *)(psVar1 + 4) = uVar16;
      if ((fVar11 - fVar12) * (fVar11 - fVar12) +
          (fVar4 - fVar8) * (fVar4 - fVar8) + (fVar3 - fVar7) * (fVar3 - fVar7) <=
          (fVar13 - fVar14) * (fVar13 - fVar14) +
          (fVar6 - fVar10) * (fVar6 - fVar10) + (fVar5 - fVar9) * (fVar5 - fVar9)) {
        iVar20 = (int)local_20[0] - *(int *)(iVar22 + 8);
        *psVar1 = (short)(iVar20 / 0x28);
        iVar20 = (int)local_8 - *(int *)(iVar22 + 8);
        psVar1[1] = (short)(iVar20 / 0x28);
        iVar20 = (int)local_30[2] - *(int *)(iVar22 + 8);
        psVar1[2] = (short)(iVar20 / 0x28);
        *(tU32 *)(psVar2 + 4) = uVar16;
        *(tU8 *)(psVar2 + 8) = 0;
        iVar20 = (int)local_20[0] - *(int *)(iVar22 + 8);
        *psVar2 = (short)(iVar20 / 0x28);
        iVar20 = (int)local_30[2] - *(int *)(iVar22 + 8);
        psVar2[1] = (short)(iVar20 / 0x28);
        iVar20 = (int)local_4 - *(int *)(iVar22 + 8);
        psVar2[2] = (short)(iVar20 / 0x28);
      }
      else {
        iVar20 = (int)local_4 - *(int *)(iVar22 + 8);
        *psVar1 = (short)(iVar20 / 0x28);
        iVar20 = (int)local_20[0] - *(int *)(iVar22 + 8);
        psVar1[1] = (short)(iVar20 / 0x28);
        iVar20 = (int)local_8 - *(int *)(iVar22 + 8);
        psVar1[2] = (short)(iVar20 / 0x28);
        *(tU32 *)(psVar2 + 4) = uVar16;
        *(tU8 *)(psVar2 + 8) = 0;
        iVar20 = (int)local_4 - *(int *)(iVar22 + 8);
        *psVar2 = (short)(iVar20 / 0x28);
        iVar20 = (int)local_8 - *(int *)(iVar22 + 8);
        psVar2[1] = (short)(iVar20 / 0x28);
        iVar20 = (int)local_30[2] - *(int *)(iVar22 + 8);
        psVar2[2] = (short)(iVar20 / 0x28);
      }
    }
    if (pClip_flags[1] == 0) {
      uVar15 = *(tU16 *)(iVar22 + 0x12);
      if (uVar18 <= uVar15 + 1) {
        return 0;
      }
      puVar26 = *(tU32 **)(iVar22 + 0xc);
      *(tU16 *)(iVar22 + 0x12) = uVar15 + 2;
      psVar1 = (short *)(puVar26 + (tU32)uVar15 * 10);
      psVar2 = (short *)(puVar26 + (tU32)(tU16)(uVar15 + 1) * 10);
      psVar29 = psVar1;
      for (iVar20 = 10; iVar20 != 0; iVar20 = iVar20 + -1) {
        *(tU32 *)psVar29 = *puVar26;
        puVar26 = puVar26 + 1;
        psVar29 = psVar29 + 2;
      }
      puVar26 = *(tU32 **)(iVar22 + 0xc);
      psVar29 = psVar2;
      for (iVar20 = 10; iVar20 != 0; iVar20 = iVar20 + -1) {
        *(tU32 *)psVar29 = *puVar26;
        puVar26 = puVar26 + 1;
        psVar29 = psVar29 + 2;
      }
      fVar3 = local_30[0][1];
      fVar4 = local_30[0][2];
      fVar5 = local_10[1];
      fVar6 = local_10[2];
      fVar7 = local_20[1][1];
      fVar8 = local_20[1][2];
      fVar9 = local_4[1];
      fVar10 = local_4[2];
      fVar11 = *local_30[0];
      fVar12 = *local_20[1];
      fVar13 = *local_10;
      fVar14 = *local_4;
      *(tU8 *)(psVar1 + 8) = 0;
      *(tU32 *)(psVar1 + 4) = uVar16;
      if ((fVar11 - fVar12) * (fVar11 - fVar12) +
          (fVar4 - fVar8) * (fVar4 - fVar8) + (fVar3 - fVar7) * (fVar3 - fVar7) <=
          (fVar13 - fVar14) * (fVar13 - fVar14) +
          (fVar6 - fVar10) * (fVar6 - fVar10) + (fVar5 - fVar9) * (fVar5 - fVar9)) {
        iVar20 = (int)local_10 - *(int *)(iVar22 + 8);
        *psVar1 = (short)(iVar20 / 0x28);
        iVar20 = (int)local_20[1] - *(int *)(iVar22 + 8);
        psVar1[1] = (short)(iVar20 / 0x28);
        iVar20 = (int)local_4 - *(int *)(iVar22 + 8);
        psVar1[2] = (short)(iVar20 / 0x28);
        *(tU32 *)(psVar2 + 4) = uVar16;
        *(tU8 *)(psVar2 + 8) = 0;
        iVar20 = (int)local_10 - *(int *)(iVar22 + 8);
        *psVar2 = (short)(iVar20 / 0x28);
        iVar20 = (int)local_4 - *(int *)(iVar22 + 8);
        psVar2[1] = (short)(iVar20 / 0x28);
        iVar20 = (int)local_30[0] - *(int *)(iVar22 + 8);
        psVar2[2] = (short)(iVar20 / 0x28);
      }
      else {
        iVar20 = (int)local_30[0] - *(int *)(iVar22 + 8);
        *psVar1 = (short)(iVar20 / 0x28);
        iVar20 = (int)local_10 - *(int *)(iVar22 + 8);
        psVar1[1] = (short)(iVar20 / 0x28);
        iVar20 = (int)local_20[1] - *(int *)(iVar22 + 8);
        psVar1[2] = (short)(iVar20 / 0x28);
        *(tU32 *)(psVar2 + 4) = uVar16;
        *(tU8 *)(psVar2 + 8) = 0;
        iVar20 = (int)local_30[0] - *(int *)(iVar22 + 8);
        *psVar2 = (short)(iVar20 / 0x28);
        iVar20 = (int)local_20[1] - *(int *)(iVar22 + 8);
        psVar2[1] = (short)(iVar20 / 0x28);
        iVar20 = (int)local_4 - *(int *)(iVar22 + 8);
        psVar2[2] = (short)(iVar20 / 0x28);
      }
    }
    if (pClip_flags[2] == 0) {
      uVar15 = *(tU16 *)(iVar22 + 0x12);
      if (uVar18 <= uVar15 + 1) {
        return 0;
      }
      puVar26 = *(tU32 **)(iVar22 + 0xc);
      psVar1 = (short *)(puVar26 + (tU32)uVar15 * 10);
      *(tU16 *)(iVar22 + 0x12) = uVar15 + 2;
      psVar2 = (short *)(puVar26 + (tU32)(tU16)(uVar15 + 1) * 10);
      psVar29 = psVar1;
      for (iVar20 = 10; iVar20 != 0; iVar20 = iVar20 + -1) {
        *(tU32 *)psVar29 = *puVar26;
        puVar26 = puVar26 + 1;
        psVar29 = psVar29 + 2;
      }
      puVar26 = *(tU32 **)(iVar22 + 0xc);
      psVar29 = psVar2;
      for (iVar20 = 10; iVar20 != 0; iVar20 = iVar20 + -1) {
        *(tU32 *)psVar29 = *puVar26;
        puVar26 = puVar26 + 1;
        psVar29 = psVar29 + 2;
      }
      fVar3 = local_10[1];
      fVar4 = local_10[2];
      fVar5 = local_30[1][1];
      fVar6 = local_30[1][2];
      fVar7 = local_c[1];
      fVar8 = local_c[2];
      fVar9 = local_20[3][1];
      fVar10 = local_20[3][2];
      fVar11 = *local_10;
      fVar12 = *local_c;
      fVar13 = *local_30[1];
      fVar14 = *local_20[3];
      *(tU8 *)(psVar1 + 8) = 0;
      *(tU32 *)(psVar1 + 4) = uVar16;
      if ((fVar11 - fVar12) * (fVar11 - fVar12) +
          (fVar4 - fVar8) * (fVar4 - fVar8) + (fVar3 - fVar7) * (fVar3 - fVar7) <=
          (fVar13 - fVar14) * (fVar13 - fVar14) +
          (fVar6 - fVar10) * (fVar6 - fVar10) + (fVar5 - fVar9) * (fVar5 - fVar9)) {
        iVar20 = (int)local_30[1] - *(int *)(iVar22 + 8);
        *psVar1 = (short)(iVar20 / 0x28);
        iVar20 = (int)local_c - *(int *)(iVar22 + 8);
        psVar1[1] = (short)(iVar20 / 0x28);
        iVar20 = (int)local_20[3] - *(int *)(iVar22 + 8);
        psVar1[2] = (short)(iVar20 / 0x28);
        *(tU32 *)(psVar2 + 4) = uVar16;
        *(tU8 *)(psVar2 + 8) = 0;
        iVar20 = (int)local_30[1] - *(int *)(iVar22 + 8);
        *psVar2 = (short)(iVar20 / 0x28);
        iVar20 = (int)local_20[3] - *(int *)(iVar22 + 8);
        psVar2[1] = (short)(iVar20 / 0x28);
        iVar20 = (int)local_10 - *(int *)(iVar22 + 8);
        psVar2[2] = (short)(iVar20 / 0x28);
      }
      else {
        iVar20 = (int)local_10 - *(int *)(iVar22 + 8);
        *psVar1 = (short)(iVar20 / 0x28);
        iVar20 = (int)local_30[1] - *(int *)(iVar22 + 8);
        psVar1[1] = (short)(iVar20 / 0x28);
        iVar20 = (int)local_c - *(int *)(iVar22 + 8);
        psVar1[2] = (short)(iVar20 / 0x28);
        *(tU32 *)(psVar2 + 4) = uVar16;
        *(tU8 *)(psVar2 + 8) = 0;
        iVar20 = (int)local_10 - *(int *)(iVar22 + 8);
        *psVar2 = (short)(iVar20 / 0x28);
        iVar20 = (int)local_c - *(int *)(iVar22 + 8);
        psVar2[1] = (short)(iVar20 / 0x28);
        iVar20 = (int)local_20[3] - *(int *)(iVar22 + 8);
        psVar2[2] = (short)(iVar20 / 0x28);
      }
    }
    if (pClip_flags[3] == 0) {
      uVar15 = *(tU16 *)(iVar22 + 0x12);
      if (uVar18 <= uVar15 + 1) {
        return 0;
      }
      puVar26 = *(tU32 **)(iVar22 + 0xc);
      *(tU16 *)(iVar22 + 0x12) = uVar15 + 2;
      psVar1 = (short *)(puVar26 + (tU32)uVar15 * 10);
      psVar2 = (short *)(puVar26 + (tU32)(tU16)(uVar15 + 1) * 10);
      psVar29 = psVar1;
      for (iVar20 = 10; iVar20 != 0; iVar20 = iVar20 + -1) {
        *(tU32 *)psVar29 = *puVar26;
        puVar26 = puVar26 + 1;
        psVar29 = psVar29 + 2;
      }
      puVar26 = *(tU32 **)(iVar22 + 0xc);
      psVar29 = psVar2;
      for (iVar20 = 10; iVar20 != 0; iVar20 = iVar20 + -1) {
        *(tU32 *)psVar29 = *puVar26;
        puVar26 = puVar26 + 1;
        psVar29 = psVar29 + 2;
      }
      fVar3 = local_20[2][1];
      fVar4 = local_20[2][2];
      fVar5 = local_c[1];
      fVar6 = local_c[2];
      fVar7 = local_30[3][1];
      fVar8 = local_30[3][2];
      fVar9 = local_8[1];
      fVar10 = local_8[2];
      fVar11 = *local_20[2];
      fVar12 = *local_30[3];
      fVar13 = *local_c;
      fVar14 = *local_8;
      *(tU8 *)(psVar1 + 8) = 0;
      *(tU32 *)(psVar1 + 4) = uVar16;
      if ((fVar11 - fVar12) * (fVar11 - fVar12) +
          (fVar4 - fVar8) * (fVar4 - fVar8) + (fVar3 - fVar7) * (fVar3 - fVar7) <=
          (fVar13 - fVar14) * (fVar13 - fVar14) +
          (fVar6 - fVar10) * (fVar6 - fVar10) + (fVar5 - fVar9) * (fVar5 - fVar9)) {
        iVar20 = (int)local_c - *(int *)(iVar22 + 8);
        *psVar1 = (short)(iVar20 / 0x28);
        iVar20 = (int)local_30[3] - *(int *)(iVar22 + 8);
        psVar1[1] = (short)(iVar20 / 0x28);
        iVar20 = (int)local_8 - *(int *)(iVar22 + 8);
        psVar1[2] = (short)(iVar20 / 0x28);
        *(tU32 *)(psVar2 + 4) = uVar16;
        *(tU8 *)(psVar2 + 8) = 0;
        iVar20 = (int)local_c - *(int *)(iVar22 + 8);
        *psVar2 = (short)(iVar20 / 0x28);
        iVar20 = (int)local_8 - *(int *)(iVar22 + 8);
        psVar2[1] = (short)(iVar20 / 0x28);
        iVar20 = (int)local_20[2] - *(int *)(iVar22 + 8);
        psVar2[2] = (short)(iVar20 / 0x28);
      }
      else {
        iVar20 = (int)local_20[2] - *(int *)(iVar22 + 8);
        *psVar1 = (short)(iVar20 / 0x28);
        iVar20 = (int)local_c - *(int *)(iVar22 + 8);
        psVar1[1] = (short)(iVar20 / 0x28);
        iVar20 = (int)local_30[3] - *(int *)(iVar22 + 8);
        psVar1[2] = (short)(iVar20 / 0x28);
        *(tU32 *)(psVar2 + 4) = uVar16;
        *(tU8 *)(psVar2 + 8) = 0;
        iVar20 = (int)local_20[2] - *(int *)(iVar22 + 8);
        *psVar2 = (short)(iVar20 / 0x28);
        iVar20 = (int)local_30[3] - *(int *)(iVar22 + 8);
        psVar2[1] = (short)(iVar20 / 0x28);
        iVar20 = (int)local_8 - *(int *)(iVar22 + 8);
        psVar2[2] = (short)(iVar20 / 0x28);
      }
    }
    ppfVar23 = local_20;
    iVar20 = 8;
    do {
      iVar24 = (int)*ppfVar23 - *(int *)(iVar22 + 8);
      iVar27 = iVar20 + 2;
      ppfVar23 = ppfVar23 + 1;
      *(short *)((int)pOutput_indices + iVar20 + -8) =
           (short)(iVar24 / 0x28);
      iVar20 = iVar27;
    } while (iVar27 < 0x10);
    return 1;
  }
  ppfVar23 = local_30;
  pfVar19 = (float *)&gMatrix34_006b78a0;
  do {
    pfVar25 = *ppfVar23;
    pfVar28 = pfVar19 + 3;
    ppfVar23 = ppfVar23 + 1;
    *pfVar19 = *pfVar25;
    pfVar19[1] = pfVar25[1];
    pfVar19[2] = pfVar25[2];
    pfVar19 = pfVar28;
  } while ((tU32)pfVar28 < (tU32)((tU8*)&gMatrix34_006b78a0 + sizeof(gMatrix34_006b78a0)));
  ppfVar23 = local_30;
  pClip_flags = (tU8 *)0x4;
  gVector3_006a4688.v[0] = *(float *)(pContext + 0xc);
  gVector3_006a4688.v[1] = *(float *)(pContext + 0x10);
  gVector3_006a4688.v[2] = *(float *)(pContext + 0x14);
  gINT_006a3334 = 1;
  do {
    iVar20 = (int)*ppfVar23 - *(int *)(iVar22 + 8);
    ppfVar23 = ppfVar23 + 1;
    *pOutput_indices = (short)(iVar20 / 0x28);
    pOutput_indices = pOutput_indices + 1;
    pClip_flags = (tU8 *)((int)pClip_flags + -1);
  } while (pClip_flags != (tU8 *)0x0);
  *(tU32 *)(*(int *)(pContext + 0x50) + 8) = pSurface_token;
  *(tU8 *)(*(int *)(pContext + 0x50) + 0x10) = 0;
  iVar20 = (int)local_30[0] - *(int *)(iVar22 + 8);
  **(short **)(pContext + 0x50) =
       (short)(iVar20 / 0x28);
  iVar20 = (int)local_30[1] - *(int *)(iVar22 + 8);
  *(short *)(*(int *)(pContext + 0x50) + 2) =
       (short)(iVar20 / 0x28);
  iVar20 = (int)local_30[3] - *(int *)(iVar22 + 8);
  *(short *)(*(int *)(pContext + 0x50) + 4) =
       (short)(iVar20 / 0x28);
  *(tU32 *)(*(int *)(pContext + 0x80) + 8) = pSurface_token;
  *(tU8 *)(*(int *)(pContext + 0x80) + 0x10) = 0;
  iVar20 = (int)local_30[0] - *(int *)(iVar22 + 8);
  **(short **)(pContext + 0x80) =
       (short)(iVar20 / 0x28);
  iVar20 = (int)local_30[3] - *(int *)(iVar22 + 8);
  *(short *)(*(int *)(pContext + 0x80) + 2) =
       (short)(iVar20 / 0x28);
  iVar22 = (int)local_30[2] - *(int *)(iVar22 + 8);
  *(short *)(*(int *)(pContext + 0x80) + 4) =
       (short)(iVar22 / 0x28);
  *(tU8 *)(*(int *)(pContext + 0x50) + 0x10) = 4;
  *(tU8 *)(*(int *)(pContext + 0x80) + 0x10) = 1;
  return 1;
}


#pragma auto_inline(off)
// C2PORT-NONMATCHING: best 45.38% | difftest PASS 2000 trials (spec difftest/specs/00431e20_CrushCarActorCbfn.json) | reason register-allocation/stack-layout
// FUNCTION: CARMA2_HW 0x00431e20
intptr_t C2_HOOK_FASTCALL CrushCarActorCbfn(br_actor* pActor, br_matrix34* pMat, void* pContext) {
    register void** context;
    tCar_spec* car;
    tUser_crush_data* actor_user;
    tCar_crush_buffer_entry* entry;
    tCar_crush_spec* crush_spec;
    tCar_crush_smashable_part* smashable;
    float* crush_center;
    float force;
    float max_distance;
    float max_bend;
    volatile tU32 smashable_count;
    tU32 max_vertex;
    float dist_sq;
    int axis;
    int flip;
    int limit_count;
    int found;
    int i;
    float local_48[6];
    br_vector3 local_30[4];

    if (pActor->type != 1) {
        return 0;
    }
    actor_user = pActor->user;
    if (actor_user == NULL || actor_user->crush_data == NULL) {
        return 0;
    }

    entry = actor_user->crush_data;
    context = (void**)pContext;
    if (((tCar_crush*)context[1])->count > 4) {
        ((tCar_crush*)context[1])->count = 4;
    }

    local_48[0] = 0.f;
    local_48[1] = 0.f;
    local_48[2] = 0.f;
    for (i = 0; i < ((tCar_crush*)context[1])->count; i++) {
        DrMatrix34ApplyLPInverse(&local_30[i],
            (br_vector3*)((tU8*)context[1] + i * sizeof(br_vector3) + 4), pMat);
        local_48[0] += local_30[i].v[0];
        local_48[1] += local_30[i].v[1];
        local_48[2] += local_30[i].v[2];
    }
    DrMatrix34ApplyLPInverse((br_vector3*)local_48,
        (br_vector3*)((tU8*)context[1] + 0x34), pMat);
    BrMatrix34TApplyV((br_vector3*)(local_48 + 3),
        (br_vector3*)((tU8*)context[1] + 0x44), pMat);

    force = entry->softness_factor * ((float*)context)[2];
    if (TooFarToCrush(pActor->model, ((tCar_crush*)context[1])->count, local_30, gMax_crush_dist_sq) != 0) {
        return 0;
    }

    if ((float)fabs(local_48[3]) > gCrushSqrtHalf) {
        axis = 0;
    } else if ((float)fabs(local_48[4]) > gCrushSqrtHalf) {
        axis = 1;
    } else {
        axis = 2;
    }
    flip = gCrushZero < local_48[axis + 3];
    car = (tCar_spec*)context[0];
    crush_spec = car->car_crush_spec;
    limit_count = crush_spec->count_limits.counts[axis][flip];
    found = 0;
    for (i = 0; i < limit_count; i++) {
        if (flip == 0) {
            if (crush_spec->limits.limits[axis][0].values[i] < local_48[axis]) {
                found = i;
                break;
            }
        } else if (local_48[axis] < crush_spec->limits.limits[axis][1].values[i]) {
            found = i;
            break;
        }
    }

    FUN_00432b80(pActor->model, ((tCar_crush*)context[1])->count, (float*)local_30,
        (br_vector3*)(local_48 + 3), force, (const float*)&crush_spec->limits, axis, found,
        &max_bend, &max_distance, (tU16*)&max_vertex);

    crush_center = (float*)((tU8*)context[1] + 0x34);
    smashable_count = entry->count_smashables;
    if (smashable_count != 0) {
        smashable = entry->smashables;
        do {
            dist_sq = (smashable->field_0x54.v[0] - crush_center[0])
                    * (smashable->field_0x54.v[0] - crush_center[0])
                    + (smashable->field_0x54.v[2] - crush_center[2])
                    * (smashable->field_0x54.v[2] - crush_center[2])
                    + (smashable->field_0x54.v[1] - crush_center[1])
                    * (smashable->field_0x54.v[1] - crush_center[1]);
            if (dist_sq < gFLOAT_00589acc) {
                dist_sq = gFLOAT_00589acc;
            }
            FUN_004ee250(car, car->collision_info, (br_vector3*)((tU8*)car + 0x4c), NULL, pActor, 0,
                smashable, (force * gDistortion_factor) / (float)sqrt((double)dist_sq), 0);
            smashable++;
            smashable_count--;
        } while (smashable_count != 0);
    }

    if (entry->detach_data != NULL) {
        entry->detach_data->field_0x0 += ((float*)context)[2] * max_bend;
        if (entry->detach_data->field_0x0 > entry->detach_data->force) {
            SetBitForDetachment(pActor, car, 0.f, 0);
        }
    }
    if (entry->flap_data != NULL && entry->flap_data->kev_o_flap == 0 && gCrushZero < max_distance) {
        FUN_004321d0(car, pActor, (br_vector3*)(local_48 + 3), max_distance, max_vertex,
            *(tU32*)((tU8*)context[1] + 0x50));
    }
    if (max_bend != gCrushZero) {
        entry->field_0x2c = 1;
    }

    for (i = 1; i < car->count_detail_levels; i++) {
        FUN_00432b80(((tUser_crush_data*)pActor->user)->models[i], ((tCar_crush*)context[1])->count,
            (float*)local_30, (br_vector3*)(local_48 + 3), force, (const float*)&crush_spec->limits, axis, found,
            &max_bend, &max_distance, (tU16*)&max_vertex);
    }
    return 0;
}

// C2PORT-NONMATCHING: best 0.00% | difftest PASS 2000 trials (spec difftest/specs/004321d0_FUN_004321d0.json) | reason reccmp-unindexed/x87-angle-rounding
// FUNCTION: CARMA2_HW 0x004321d0
void C2_HOOK_FASTCALL FUN_004321d0(tCar_spec* pCar, br_actor* pActor, const br_vector3* pImpact_direction, br_scalar pImpact_distance, tU32 pVertex_index, tU32 pRandom_seed) {
    tUser_crush_data* user;
    tUser_crush_data* parent_user;
    tUser_detail_level_model* detail_model;
    tModel_detail_vertex_data* detail_vertices;
    tCar_crush_buffer_entry* crush_data;
    tCar_crush_flap_data* flap;
    br_model* model;
    br_model* parent_model;
    br_vertex* vertices;
    br_vertex* hinge_vertex;
    br_vertex* impact_vertex;
    br_vector3 edge1;
    br_vector3 edge2;
    br_vector3 hinge_axis;
    br_vector3 rotation_axis;
    br_vector3 local_parent_vertex;
    br_vector3 from_hinge;
    float moved[1024];
    float edge1_length;
    float edge2_length;
    float axis_length;
    float dot;
    float plane_distance;
    float distance_to_impact;
    float delta_angle;
    float random_distance;
    tS16 local_angle;
    tS16 current_flap_angle;
    tS16 vertex_angle;
    float min_angle;
    float max_angle;
    float new_min_angle;
    float new_max_angle;
    float excess;
    int i;

    user = pActor->user;
    if (user == NULL || user->models[0] == NULL) {
        return;
    }
    model = user->models[0];
    detail_model = model->user;
    if (detail_model == NULL || detail_model->field_0x4 == NULL || user->crush_data == NULL) {
        return;
    }
    detail_vertices = detail_model->field_0x4;
    crush_data = user->crush_data;
    flap = crush_data->flap_data;
    if (flap == NULL || !flap->field_0x14) {
        return;
    }
    if (model->nvertices > 0x400) {
        PDFatalError("Oh my goodness, there's a lot of vertices on that model.");
    }

    vertices = model->vertices;
    hinge_vertex = &vertices[flap->hinge0];
    edge1.v[0] = vertices[flap->hinge1].p.v[0] - hinge_vertex->p.v[0];
    edge1.v[1] = vertices[flap->hinge1].p.v[1] - hinge_vertex->p.v[1];
    edge1.v[2] = vertices[flap->hinge1].p.v[2] - hinge_vertex->p.v[2];
    edge1_length = (float)sqrt((double)(edge1.v[2] * edge1.v[2] + edge1.v[1] * edge1.v[1] + edge1.v[0] * edge1.v[0]));
    if (edge1_length <= gFLOAT_00589a5c) {
        edge1.v[0] = 1.f;
        edge1.v[1] = 0.f;
        edge1.v[2] = 0.f;
    } else {
        edge1_length = (float)gDOUBLE_00589a60 / edge1_length;
        edge1.v[0] *= edge1_length;
        edge1.v[1] *= edge1_length;
        edge1.v[2] *= edge1_length;
    }

    edge2.v[0] = vertices[flap->hinge2].p.v[0] - hinge_vertex->p.v[0];
    edge2.v[1] = vertices[flap->hinge2].p.v[1] - hinge_vertex->p.v[1];
    edge2.v[2] = vertices[flap->hinge2].p.v[2] - hinge_vertex->p.v[2];
    edge2_length = (float)sqrt((double)(edge2.v[2] * edge2.v[2] + edge2.v[1] * edge2.v[1] + edge2.v[0] * edge2.v[0]));
    if (edge2_length <= gFLOAT_00589a5c) {
        edge2.v[0] = 1.f;
        edge2.v[1] = 0.f;
        edge2.v[2] = 0.f;
    } else {
        edge2_length = (float)gDOUBLE_00589a60 / edge2_length;
        edge2.v[0] *= edge2_length;
        edge2.v[1] *= edge2_length;
        edge2.v[2] *= edge2_length;
    }

    rotation_axis.v[0] = edge2.v[2] * edge1.v[1] - edge2.v[1] * edge1.v[2];
    rotation_axis.v[1] = edge2.v[0] * edge1.v[2] - edge2.v[2] * edge1.v[0];
    rotation_axis.v[2] = edge2.v[1] * edge1.v[0] - edge2.v[0] * edge1.v[1];
    axis_length = (float)sqrt((double)(rotation_axis.v[2] * rotation_axis.v[2] + rotation_axis.v[1] * rotation_axis.v[1] + rotation_axis.v[0] * rotation_axis.v[0]));
    if (axis_length <= gFLOAT_00589a5c) {
        rotation_axis.v[0] = 1.f;
        rotation_axis.v[1] = 0.f;
        rotation_axis.v[2] = 0.f;
    } else {
        axis_length = (float)gDOUBLE_00589a60 / axis_length;
        rotation_axis.v[0] *= axis_length;
        rotation_axis.v[1] *= axis_length;
        rotation_axis.v[2] *= axis_length;
    }

    dot = edge2.v[0] * edge1.v[0] + edge2.v[1] * edge1.v[1] + edge2.v[2] * edge1.v[2];
    hinge_axis.v[0] = edge1.v[0] * dot - edge2.v[0];
    hinge_axis.v[1] = edge1.v[1] * dot - edge2.v[1];
    hinge_axis.v[2] = edge1.v[2] * dot - edge2.v[2];
    axis_length = (float)sqrt((double)(hinge_axis.v[2] * hinge_axis.v[2] + hinge_axis.v[1] * hinge_axis.v[1] + hinge_axis.v[0] * hinge_axis.v[0]));
    if (axis_length <= gFLOAT_00589a5c) {
        hinge_axis.v[0] = 1.f;
        hinge_axis.v[1] = 0.f;
        hinge_axis.v[2] = 0.f;
    } else {
        axis_length = (float)gDOUBLE_00589a60 / axis_length;
        hinge_axis.v[0] *= axis_length;
        hinge_axis.v[1] *= axis_length;
        hinge_axis.v[2] *= axis_length;
    }

    parent_user = pActor->parent->user;
    parent_model = parent_user->models[0];
    DrMatrix34ApplyLPInverse(&local_parent_vertex,
        &parent_model->vertices[flap->field_0x12].p, &pActor->t.t.mat);
    from_hinge.v[0] = local_parent_vertex.v[0] - hinge_vertex->p.v[0];
    from_hinge.v[1] = local_parent_vertex.v[1] - hinge_vertex->p.v[1];
    from_hinge.v[2] = local_parent_vertex.v[2] - hinge_vertex->p.v[2];
    dot = rotation_axis.v[0] * from_hinge.v[0] + rotation_axis.v[1] * from_hinge.v[1] + rotation_axis.v[2] * from_hinge.v[2];
    if (-(hinge_axis.v[0] * from_hinge.v[0] + hinge_axis.v[1] * from_hinge.v[1] + hinge_axis.v[2] * from_hinge.v[2]) == gCrushZero) {
        local_angle = 0;
    } else {
        local_angle = (tS16)(atan2((double)dot,
            (double)-(hinge_axis.v[0] * from_hinge.v[0] + hinge_axis.v[1] * from_hinge.v[1] + hinge_axis.v[2] * from_hinge.v[2])) * gDOUBLE_00589ad0);
    }

    plane_distance = pImpact_direction->v[0] * hinge_axis.v[0]
        + pImpact_direction->v[2] * hinge_axis.v[2]
        + pImpact_direction->v[1] * hinge_axis.v[1];
    if (gFLOAT_00589ad8 < plane_distance) {
        impact_vertex = &vertices[pVertex_index & 0xffff];
        distance_to_impact = (hinge_axis.v[2] * (hinge_vertex->p.v[2] - impact_vertex->p.v[2])
            + hinge_axis.v[1] * (hinge_vertex->p.v[1] - impact_vertex->p.v[1])
            + hinge_axis.v[0] * (hinge_vertex->p.v[0] - impact_vertex->p.v[0])) / plane_distance;
        if (gCrushZero < distance_to_impact && gCrushZero < pImpact_distance) {
            delta_angle = (float)sqrt((double)((distance_to_impact + pImpact_distance) * (distance_to_impact + pImpact_distance)
                - distance_to_impact * distance_to_impact)) * gFLOAT_00589a2c;
            srand(pRandom_seed);
            if (PercentageChance((int)gChance_of_inverse_buckle)) {
                delta_angle = -delta_angle;
            }
            excess = (float)fabs((double)(flap->field_0x18 + delta_angle)) - flap->field_0x1c;
            flap->field_0x18 += delta_angle;
            if (gCrushZero < excess) {
                flap->field_0x14 = 0;
                if (gCrushZero < delta_angle) {
                    delta_angle -= excess;
                } else {
                    delta_angle += excess;
                }
            }

            random_distance = SRandomBetween(0.3f, 0.7f) * distance_to_impact;
            min_angle = -16380.f;
            max_angle = 16380.f;
            for (i = 0; i < model->nvertices; i++) {
                br_vertex* vertex = &vertices[i];
                float vertex_distance;
                float remaining_distance;

                vertex_distance = pImpact_direction->v[0] * (vertex->p.v[0] - impact_vertex->p.v[0])
                    + pImpact_direction->v[1] * (vertex->p.v[1] - impact_vertex->p.v[1])
                    + pImpact_direction->v[2] * (vertex->p.v[2] - impact_vertex->p.v[2]);
                if (vertex_distance <= gCrushZero) {
                    moved[i] = 0.f;
                } else {
                    remaining_distance = vertex_distance - random_distance;
                    if (remaining_distance <= gCrushZero) {
                        moved[i] = (vertex_distance * delta_angle) / random_distance;
                    } else {
                        float hinge_distance;
                        float total_distance;

                        hinge_distance = (hinge_axis.v[2] * (hinge_vertex->p.v[2] - vertex->p.v[2])
                            + hinge_axis.v[1] * (hinge_vertex->p.v[1] - vertex->p.v[1])
                            + hinge_axis.v[0] * (hinge_vertex->p.v[0] - vertex->p.v[0])) / plane_distance;
                        if (hinge_distance <= gCrushZero) {
                            moved[i] = 0.f;
                        } else {
                            total_distance = hinge_distance + remaining_distance;
                            if (total_distance == gCrushZero) {
                                moved[i] = 0.f;
                            } else {
                                moved[i] = (hinge_distance * delta_angle) / total_distance;
                            }
                        }
                    }
                    vertex->p.v[0] += moved[i] * rotation_axis.v[0];
                    vertex->p.v[1] += moved[i] * rotation_axis.v[1];
                    vertex->p.v[2] += moved[i] * rotation_axis.v[2];
                }
                if ((detail_vertices[i].flags & 2) != 0) {
                    float axis_dot;

                    from_hinge.v[0] = vertex->p.v[0] - hinge_vertex->p.v[0];
                    from_hinge.v[1] = vertex->p.v[1] - hinge_vertex->p.v[1];
                    from_hinge.v[2] = vertex->p.v[2] - hinge_vertex->p.v[2];
                    axis_dot = -(hinge_axis.v[2] * from_hinge.v[2]
                        + hinge_axis.v[1] * from_hinge.v[1]
                        + hinge_axis.v[0] * from_hinge.v[0]);
                    if (axis_dot != gCrushZero) {
                        vertex_angle = (tS16)(atan2((double)(rotation_axis.v[2] * from_hinge.v[2]
                                + rotation_axis.v[1] * from_hinge.v[1]
                                + rotation_axis.v[0] * from_hinge.v[0]), (double)axis_dot)
                            * gDOUBLE_00589ad0);
                        if ((tU16)(vertex_angle - (tS16)min_angle) < 0x7ff8) {
                            min_angle = vertex_angle;
                        }
                        if ((tU16)((tS16)max_angle - vertex_angle) < 0x7ff8) {
                            max_angle = vertex_angle;
                        }
                    }
                }
            }

            current_flap_angle = flap->field_0x4;
            new_max_angle = (tS16)(local_angle - (tS16)min_angle + 0x3ffc + current_flap_angle);
            new_min_angle = (tS16)(local_angle - (tS16)max_angle + current_flap_angle);
            flap->field_0x2 = (tS16)new_min_angle;
            flap->field_0x6 = (tS16)new_max_angle;
            flap->field_0x8 = (tS16)(new_max_angle - new_min_angle);
            if ((tU16)(flap->field_0x8) < (tU16)(current_flap_angle - (tS16)new_min_angle)) {
                if ((tU16)(current_flap_angle - (tS16)new_max_angle) < 0x7ff8) {
                    PipeSingleFlap((uintptr_t)pActor, (tU16)((tS16)new_max_angle - current_flap_angle));
                    flap->field_0x4 = (tS16)new_max_angle;
                } else {
                    PipeSingleFlap((uintptr_t)pActor, (tU16)((tS16)new_min_angle - current_flap_angle));
                    flap->field_0x4 = (tS16)new_min_angle;
                }
                BrMatrix34Translate(&pActor->t.t.mat,
                    -hinge_vertex->p.v[0], -hinge_vertex->p.v[1], -hinge_vertex->p.v[2]);
                BrMatrix34PostRotate(&pActor->t.t.mat, flap->field_0x4, &edge1);
                parent_model = parent_user->models[0];
                BrMatrix34PostTranslate(&pActor->t.t.mat,
                    parent_model->vertices[flap->field_0xc].p.v[0],
                    parent_model->vertices[flap->field_0xc].p.v[1],
                    parent_model->vertices[flap->field_0xc].p.v[2]);
            }

            flap->field_0x15 = 1;
            if (flap->field_0x0 == 0 && gCount_toggled_doors != 16) {
                for (i = 0; i < gCount_toggled_doors; i++) {
                    if (gToggled_doors[i].actor == pActor) {
                        break;
                    }
                }
                if (i == gCount_toggled_doors) {
                    gToggled_doors[i].actor = pActor;
                    gToggled_doors[i].car = pCar;
                    gToggled_doors[i].field_0x8 = 1;
                    gToggled_doors[i].field_0xc = 0.f;
                    gCount_toggled_doors++;
                }
            }
            FUN_004c6e40(model, &hinge_axis, moved);
        }
    }
}

// C2PORT-NONMATCHING: best 38.03% | difftest PASS 2000 trials (spec difftest/specs/00432b80_FUN_00432b80.json) | reason regalloc-stack-x87
// FUNCTION: CARMA2_HW 0x00432b80
void C2_HOOK_FASTCALL FUN_00432b80(br_model* pModel, int pCrush_count, float* pCrush_points, const br_vector3* pDirection, float pForce, const float* pLimits, int pAxis, int pLimit, float* pMax_bend, float* pMax_distance, tU16* pMax_vertex) {
    tUser_detail_level_model* detail_model;
    tModel_detail_vertex_data* vertex_data;
    register float* crush_points;
    float bend_storage[1025];
    float* bend;
    volatile int crush_count_for_distance;
    volatile float max_dist_sq;
    volatile float force_scaled;
    float vertex_distance;
    float direction_y;
    float direction_z;
    float bend_amount;
    unsigned int limit_index;
    tU16 vertex_index;

    crush_count_for_distance = pCrush_count;
    crush_points = pCrush_points;
    bend = bend_storage + 1;
    *pMax_bend = 0.f;
    *pMax_distance = 0.f;
    *pMax_vertex = 0;
    if (pModel == NULL || pModel->user == NULL) {
        return;
    }
    detail_model = pModel->user;
    vertex_data = detail_model->field_0x4;
    if (vertex_data == NULL) {
        return;
    }
    if (pModel->nvertices > 0x400) {
        PDFatalError("Well stone the crows, there's a lot of vertices on that model.");
    }
    for (vertex_index = 0; vertex_index < pModel->nvertices; vertex_index++) {
        float dist_sq;

        max_dist_sq = gMax_crush_dist_sq;
        dist_sq = CrushShapeDistSq((float*)&pModel->vertices[vertex_index].p, crush_count_for_distance, crush_points);
        if (dist_sq <= gMax_crush_dist_sq) {
            bend[vertex_index] = gFLOAT_00589a54 - (float)sqrt(dist_sq / max_dist_sq);
        } else {
            bend[vertex_index] = gCrushZero;
        }
        if (*pMax_bend < bend[vertex_index]) {
            *pMax_bend = bend[vertex_index];
            *pMax_vertex = vertex_index;
        }
    }
    if (pForce != gCrushZero && *pMax_bend != gCrushZero) {
        force_scaled = pForce * gForce_to_movement_factor;
        for (vertex_index = 0; vertex_index < pModel->nvertices; vertex_index++) {
            if (bend[vertex_index] != gCrushZero) {
                bend[vertex_index] *= force_scaled;
                limit_index = ((tU16)vertex_data[vertex_index].limit_number >> (((gCrushZero < pDirection->v[pAxis]) + pAxis * 2) * 2)) & 3;
                if (pLimit < (int)limit_index) {
                    bend[vertex_index] = 0.f;
                } else {
                    vertex_distance = (pLimits[limit_index + ((gCrushZero < pDirection->v[pAxis]) + pAxis * 2) * 4] - pModel->vertices[vertex_index].p.v[pAxis]) / pDirection->v[pAxis];

                    if (gCrushZero <= vertex_distance) {
                        if (vertex_distance < bend[vertex_index]) {
                            bend[vertex_index] = vertex_distance;
                        }
                    } else {
                        bend[vertex_index] = 0.f;
                    }
                }
                vertex_distance = bend[vertex_index];
                direction_y = pDirection->v[1];
                direction_z = pDirection->v[2];
                bend_amount = bend[vertex_index];
                pModel->vertices[vertex_index].p.v[0] = bend_amount * pDirection->v[0] + pModel->vertices[vertex_index].p.v[0];
                pModel->vertices[vertex_index].p.v[1] = direction_y * bend_amount + pModel->vertices[vertex_index].p.v[1];
                pModel->vertices[vertex_index].p.v[2] = vertex_distance * direction_z + pModel->vertices[vertex_index].p.v[2];
            }
        }
        *pMax_distance = bend[*pMax_vertex];
        FUN_004c6e40(pModel, (br_vector3*)pDirection, bend);
        if ((pModel->flags & 0x20) != 0 && pModel->user != NULL) {
            *(int*)pModel->user = 1;
            return;
        }
        pModel->custom = JitModelUpdate;
        pModel->flags |= 0x20;
    }
}

// FUNCTION: CARMA2_HW 0x00433260
intptr_t C2_HOOK_FASTCALL CrushCarModelCbfn(br_actor* pActor, br_matrix34* pMat, void* pContext) {
    struct {
        tCar_spec* car;
        tCar_crush* crush;
        float force;
    }* context;
    tUser_crush_data* user;
    tCar_crush_buffer_entry* crush_data;
    br_vector3 points[4];
    br_vector3 direction;
    float scale;
    int i;

    context = pContext;
    if (*(tU8*)((tU8*)pActor + 0x12) == 1 && *(tUser_crush_data**)((tU8*)pActor + 0x60) != NULL) {
        user = *(tUser_crush_data**)((tU8*)pActor + 0x60);
        crush_data = user->crush_data;
        if (crush_data != NULL) {
            for (i = 0; i < context->crush->count; i++) {
                DrMatrix34ApplyLPInverse(&points[i], &context->crush->vectors[i], pMat);
            }
            BrMatrix34TApplyV(&direction, &context->crush->field_0x44, pMat);
            scale = context->force * crush_data->softness_factor;
            direction.v[0] = direction.v[0] * scale;
            direction.v[1] = direction.v[1] * scale;
            direction.v[2] = direction.v[2] * scale;
            for (i = 0; i < context->car->count_detail_levels; i++) {
                SimpleCrushModel(user->models[i], context->crush->count, points[0].v, direction.v,
                    (float*)&context->car->car_crush_spec->field_0x11c);
            }
        }
    }
    return 0;
}

// CrushShapeDistSq
float C2_HOOK_FASTCALL CrushShapeDistSq(br_vector3* pPoint, int pCount, br_vector3* pVectors) {

    NOT_IMPLEMENTED();
    return 0.0f;
}

// FUNCTION: CARMA2_HW 0x00436850
float C2_HOOK_FASTCALL CrushShapeBendFactor(br_vector3* pVertexPos, int pCount, br_vector3* pVectors, br_scalar pMaxDistSq) {
    float dist_sq;

    dist_sq = DistSq(pVertexPos, pCount, pVectors);
    if (dist_sq > gMax_crush_dist_sq) {
        return gCrushZero;
    }
    return 1.0f - (float)sqrt(dist_sq / pMaxDistSq);
}

// C2PORT-NONMATCHING: best 50.52% | difftest PASS 2000 trials (spec difftest/specs/0043c7a0_FUN_0043c7a0.json) | reason x87-scheduling-and-stack-layout
// FUNCTION: CARMA2_HW 0x0043c7a0
void C2_HOOK_FASTCALL FUN_0043c7a0(br_vector3* pVertex, int pCount, float* pVectors, br_vector3* pDirection, br_scalar pForce, tU32 pFlags, const tCar_crush_limits* pLimits, int pFound) {
    float fVar1;
    int iVar3;
    tU32 uVar4;
    double fVar5;
    float fVar6;
    br_vector3 displacement;

    fVar1 = gMax_crush_dist_sq;
    fVar5 = CrushShapeDistSq((float*)pVertex, pCount, pVectors);
    if (fVar5 > fVar1) {
        fVar5 = gCrushZero;
    } else {
        fVar5 = gFLOAT_00589a54 - (float)sqrt(fVar5 / fVar1);
    }
    if (fVar5 != gCrushZero) {
        fVar5 = pForce * gForce_to_movement_factor * fVar5;
        if (fabsf(pDirection->v[0]) <= gCrushSqrtHalf) {
            iVar3 = 1;
            if (fabsf(pDirection->v[1]) <= gCrushSqrtHalf) {
                iVar3 = 2;
            }
        } else {
            iVar3 = 0;
        }
        fVar1 = pDirection->v[iVar3];
        uVar4 = (pFlags & 0xffff) >> ((iVar3 * 2 + (gCrushZero < fVar1)) * 2) & 3;
        if (pFound < (int)uVar4) {
            fVar5 = gCrushZero;
        } else {
            fVar6 = (pLimits->limits[iVar3][gCrushZero < fVar1].values[uVar4] - pVertex->v[iVar3]) / fVar1;
            if (gCrushZero <= fVar6) {
                if (fVar6 < fVar5) {
                    fVar5 = fVar6;
                }
            } else {
                fVar5 = gCrushZero;
            }
        }
        if (fVar5 != gCrushZero) {
            BrVector3Scale(&displacement, pDirection, (br_scalar)fVar5);
            BrVector3Accumulate(pVertex, &displacement);
        }
    }
    if (DRVector3TestForNan(pVertex) != 0) {
        PDEnterDebugger("NaN");
    }
}

// FUNCTION: CARMA2_HW 0x00436890
int C2_HOOK_FASTCALL CrushShapeFindAxis(br_vector3* pDirection) {

    if (fabsf(pDirection->v[0]) > gCrushSqrtHalf) {
        return 0;
    }
    if (fabsf(pDirection->v[1]) > gCrushSqrtHalf) {
        return 1;
    }
    return 2;
}

// FUNCTION: CARMA2_HW 0x004367a0
void C2_HOOK_FASTCALL CrushShapeMorphVertex(float* pOut, int pAxis, float pVertexComponent, float pDirectionComponent, tU32 pFlags, tCar_crush_limits* pLimits, int pFound) {
    int index;
    float t;

    index = ((pFlags & 0xffff) >> ((pAxis * 2 + (pDirectionComponent > gCrushZero)) * 2)) & 3;
    if (index > pFound) {
        *pOut = 0.0f;
        return;
    }
    t = (pLimits->limits[pAxis][pDirectionComponent > gCrushZero].values[index] - pVertexComponent) / pDirectionComponent;
    if (t < gCrushZero) {
        *pOut = 0.0f;
    } else if (t < *pOut) {
        *pOut = t;
    }
}

// FUNCTION: CARMA2_HW 0x004368d0
void C2_HOOK_FASTCALL CrushShapeFinishVertex(br_vector3* pVertexPos, tU16 pFlags, tCar_crush_limits* pLimits) {
    int i;

    for (i = 0; i < 3; i++) {
        if (pVertexPos->v[i] < pLimits->limits[i][0].values[pFlags & 3]) {
            pVertexPos->v[i] = pLimits->limits[i][0].values[pFlags & 3];
            pFlags >>= 4;
        } else {
            pFlags >>= 2;
            if (pVertexPos->v[i] > pLimits->limits[i][1].values[pFlags & 3]) {
                pVertexPos->v[i] = pLimits->limits[i][1].values[pFlags & 3];
            }
            pFlags >>= 2;
        }
    }
}

// C2PORT-NONMATCHING: best 36.56% | difftest PASS 2000 trials (spec difftest/specs/0043c480_ReceivedCrush.json) | reason stack-layout
// FUNCTION: CARMA2_HW 0x0043c480
void C2_HOOK_FASTCALL ReceivedCrush(tU8* pContents) {
    tCar_crush crush;
    struct {
        tCar_spec* car;
        tCar_crush* crush;
        float force;
    } context;
    br_actor* master_actor;
    tCar_spec* car;
    tCar_crush_spec* crush_spec;
    br_vector3 direction;
    float crush_y;
    float crush_z;
    float magnitude;
    int axis;
    int sign;
    int found;
    int count;
    int i;
    int j;
    tCar_crush_shape_info* shape;
    tCar_crush_reordered_shape_info* entry;

    car = NetCarFromPlayerID(*(tPlayer_ID*)(pContents + 4));
    if (car != NULL && gNet_mode != 2 && *(int*)((tU8*)car + 0x1d0) == 0 && gCrush_spam_gate == 0) {
        ExpandCrush(&crush, (tCompressed_car_crush*)(pContents + 8));
        master_actor = car->car_master_actor;
        crush_spec = car->car_crush_spec;
        if (crush_spec != NULL && *(int*)((tU8*)car + 0x1b4) == 0 && crush_spec->field_0x4b8 == 0) {
            context.force = crush.field_0x40;
            if (gMax_crush_force < context.force) {
                context.force = gMax_crush_force;
            }
            context.force = context.force / (float)crush.count;
            context.car = car;
            context.crush = &crush;

            if (crush_spec->field_0x144 != 0) {
                ARStartPipingSession(3);
                DRActorEnumRecurseWithSnart(car->car_model_actor, NULL, CrushCarModelCbfn, &context);
                AREndPipingSession();
                return;
            }

            crush_y = context.force * crush.field_0x44.v[1];
            crush_z = context.force * crush.field_0x44.v[2];
            if (crush_y > gCrushZero) {
                crush_y = gCrushZero;
            }
            magnitude = (float)sqrt(crush_y * crush_y + crush_z * crush_z);
            if (magnitude >= gCrushEps) {
                direction.v[0] = gCrushZero / magnitude;
                direction.v[1] = crush_y / magnitude;
                direction.v[2] = crush_z / magnitude;
                if (fabsf(direction.v[0]) <= gCrushSqrtHalf) {
                    axis = 1;
                    if (fabsf(direction.v[1]) <= gCrushSqrtHalf) {
                        axis = 2;
                    }
                } else {
                    axis = 0;
                }
                sign = (gCrushZero < direction.v[axis]) ? 1 : 0;
                found = 0;
                count = crush_spec->field_0xa4.counts[axis][sign];
                if (count > 0) {
                    i = 0;
                    do {
                        if (sign == 0) {
                            if (crush_spec->field_0xbc.limits[axis][0].values[i] < crush.field_0x34.v[axis]) {
                                found = i;
                                break;
                            }
                        } else {
                            if (crush.field_0x34.v[axis] < crush_spec->field_0xbc.limits[axis][1].values[i]) {
                                found = i;
                                break;
                            }
                        }
                        i++;
                    } while (i < count);
                }

                if (crush_spec->count_shapes > 0) {
                    i = 0;
                    do {
                        shape = &crush_spec->field_0x4[i];
                        if (shape->field_0x8 == 0) {
                            for (j = 0; j < shape->count_points; j++) {
                                entry = &shape->field_0x18[j];
                                FUN_0043c7a0(&entry->field_0x18, crush.count, crush.vectors[0].v, &direction,
                                    entry->field_0x24 * magnitude, entry->field_0x28, &crush_spec->field_0xbc, found);
                                CrushShapeFinishVertex(&entry->field_0x18, entry->field_0x28, &crush_spec->field_0xbc);
                            }
                        }
                        i++;
                    } while (i < crush_spec->count_shapes);
                }
                crush_spec->expand_bounding_box = 7;
            }

            ARStartPipingSession(3);
            DRActorEnumRecurseWithSnart(master_actor, NULL, CrushCarActorCbfn, &context);
            AREndPipingSession();
            *(int*)((tU8*)car + 0xe24) = 0;
        }
    }
}

// C2PORT-NONMATCHING: best 48.13% | difftest PASS 2000 trials (spec difftest/specs/0043cd60_FUN_0043cd60.json) | reason regalloc-loop
// FUNCTION: CARMA2_HW 0x0043cd60
void C2_HOOK_FASTCALL FUN_0043cd60(void* pParam) {
    tCar_crush_spec* crush;
    int found;
    int i;
    int* piVar4;
    int count;
    tCar_spec* car;

    if (gCount_net_crush_reattach_bit_list != 8) {
        car = NetCarFromPlayerID(*(tPlayer_ID*)((tU8*)pParam + 4));
        if (car != NULL) {
            crush = car->car_crush_spec;
            found = 0;
            if (crush != NULL) {
                i = 0;
                if (0 < crush->field_0x2b0) {
                    piVar4 = (int*)&crush->field_0x2b4[0].field_0x8;
                    do {
                        if (*(char*)((tU8*)(*piVar4) + 0x190) == *(char*)((tU8*)pParam + 8)) {
                            found = (int)crush->field_0x2b4[i].field_0x0;
                            goto found_first;
                        }
                        i++;
                        piVar4 = piVar4 + 5;
                    } while (i < crush->field_0x2b0);
                }
            }
            found = 0;
found_first:
            if (found == 0) {
                found = 0;
                if (crush != NULL) {
                    i = 0;
                    if (0 < crush->field_0x270) {
                        piVar4 = (int*)&crush->field_0x274[0].field_0x8;
                        do {
                            if (*(char*)((tU8*)(*piVar4) + 0x190) == *(char*)((tU8*)pParam + 8)) {
                                found = (int)crush->field_0x274[i].field_0x0;
                                goto found_second;
                            }
                            i++;
                            piVar4 = piVar4 + 5;
                        } while (i < crush->field_0x270);
                    }
                }
                found = 0;
found_second:
                if (found == 0) {
                    return;
                }
            }
            i = 0;
            if (0 < gCount_net_crush_reattach_bit_list) {
                piVar4 = (int*)gNet_crush_reattach_bit_list;
                do {
                    if (*piVar4 == found) {
                        return;
                    }
                    i++;
                    piVar4 = piVar4 + 3;
                } while (i < gCount_net_crush_reattach_bit_list);
            }
            count = gCount_net_crush_reattach_bit_list;
            gNet_crush_reattach_bit_list[count].car = car;
            gNet_crush_reattach_bit_list[count].field_0x0 = found;
            gCount_net_crush_reattach_bit_list = count + 1;
            gNet_crush_reattach_bit_list[count].field_0x8 = *(tU32*)((tU8*)pParam + 0xc);
        }
    }
}
#pragma auto_inline(on)

// FUNCTION: CARMA2_HW 0x004363f0
void C2_HOOK_FASTCALL CrushCar(tCar_spec* pCar, br_actor* pActor, tCar_crush* pCrush) {
    tCar_crush_spec* spec;
    tCar_crush_limits* crush_limits;
    struct {
        tCar_spec* car;
        tCar_crush* crush;
    } context;
    float rate;
    float crush_len;
    br_vector3 crush;
    int axis;
    int flip;
    int limit_count;
    int found;
    int shape_index;
    int vertex_count;
    float val;
    float bend;
    float aux;
    int new_axis;
    int k;

    spec = pCar->car_crush_spec;
    if (spec == NULL) {
        return;
    }
    if (*(int*)((tU8*)pCar + 0x1b4) != 0) {
        return;
    }
    if (spec->field_0x4b8 != 0) {
        return;
    }

    context.car = pCar;
    context.crush = pCrush;

    {
        float x = pCrush->field_0x40;
        if (x > gMax_crush_force) {
            x = gMax_crush_force;
        }
        rate = (float)(x / (double)pCrush->count);
    }

    if (spec->field_0x144 != 0) {
        ARStartPipingSession(3);
        DRActorEnumRecurseWithSnart(pCar->car_model_actor, NULL, CrushCarModelCbfn, &context);
        AREndPipingSession();
        return;
    }

    if (pActor != pCar->car_master_actor) {
        goto tail;
    }

    crush.v[0] = pCrush->field_0x44.v[0] * rate;
    crush.v[1] = pCrush->field_0x44.v[1] * rate;
    if (crush.v[1] > gCrushZero) {
        crush.v[1] = 0.0f;
    }
    crush.v[2] = pCrush->field_0x44.v[2] * rate;
    crush.v[0] = gCrushZero;
    crush_len = (float)sqrt((double)(crush.v[1] * crush.v[1] + crush.v[2] * crush.v[2]));
    if (crush_len < gCrushEps) {
        goto tail;
    }

    crush.v[0] = gCrushZero / crush_len;
    crush.v[1] = crush.v[1] / crush_len;
    crush.v[2] = crush.v[2] / crush_len;

    if (fabsf(crush.v[0]) > gCrushSqrtHalf) {
        axis = 0;
    } else {
        axis = 1;
        if (fabsf(crush.v[1]) > gCrushSqrtHalf) {
            axis = 2;
        }
    }

    flip = (crush.v[axis] > gCrushZero) ? 1 : 0;
    limit_count = spec->field_0xa4.counts[axis][flip];
    crush_limits = &spec->field_0xbc;
    found = 0;
    for (k = 0; k < limit_count; k++) {
        if (flip != 0) {
            if (crush_limits->limits[axis][1].values[k] > pCrush->field_0x34.v[axis]) {
                found = k;
                break;
            }
        } else {
            if (crush_limits->limits[axis][0].values[k] < pCrush->field_0x34.v[axis]) {
                found = k;
                break;
            }
        }
    }

    for (shape_index = 0; shape_index < spec->count_shapes; shape_index++) {
        tCar_crush_shape_info* shape = &spec->field_0x4[shape_index];
        if (shape->field_0x8 != 0 || shape->count_points <= 0) {
            continue;
        }

        for (vertex_count = 0; vertex_count < shape->count_points; vertex_count++) {
            tCar_crush_reordered_shape_info* vertex_info = &shape->field_0x18[vertex_count];

            val = vertex_info->field_0x24 * crush_len;
            bend = CrushShapeBendFactor(&vertex_info->field_0x18, pCrush->count, pCrush->vectors, gMax_crush_dist_sq);
            if (bend == gCrushZero) {
                aux = gForce_to_movement_factor * val * bend;
                new_axis = CrushShapeFindAxis(&crush);
                CrushShapeMorphVertex(&aux, new_axis, vertex_info->field_0x18.v[new_axis], crush.v[new_axis + 1], (tU32)vertex_info->field_0x28, crush_limits, found);
                if (aux == gCrushZero) {
                    vertex_info->field_0x18.v[1] += crush.v[1] * aux;
                    vertex_info->field_0x18.v[2] += crush.v[2] * aux;
                    vertex_info->field_0x18.v[0] += crush.v[0] * aux;
                }
            }

            if (DRVector3TestForNan(&vertex_info->field_0x18)) {
                PDEnterDebugger("NaN");
            }
            CrushShapeFinishVertex(&vertex_info->field_0x18, vertex_info->field_0x28, crush_limits);
        }
    }
    spec->expand_bounding_box = 7;

tail:
    ARStartPipingSession(3);
    DRActorEnumRecurseWithSnart(pActor, NULL, CrushCarActorCbfn, &context);
    AREndPipingSession();
    pCar->use_shell_model = 0;
}

void C2_HOOK_FASTCALL DoSpams(void) {
    int i;

    for (i = 0; i < gCount_car_damage_crush_list; i++) {
        TotallySpamTheModel(gCar_damage_crush_list[i].car, gCar_damage_crush_list[i].damage);
    }
    gCount_car_damage_crush_list = 0;
}

// FUNCTION: CARMA2_HW 0x00436940
void C2_HOOK_FASTCALL CompressCrush(tCompressed_car_crush* pDest, const tCar_crush* pSrc) {
    int i;

    pDest->count = pSrc->count;
    for (i = 0; i < pSrc->count; i++) {
        CompressVector3(&pDest->vectors[i], &pSrc->vectors[i], -2.5f, 2.5f);
    }
    pDest->field_0x1a = DRScalarToU16(pSrc->field_0x40, 0.f, 10.f);
    CompressVector3(&pDest->field_0x1c, &pSrc->field_0x44, -1.f, 1.f);
    pDest->field_0x24 = pSrc->field_0x50;
}

// FUNCTION: CARMA2_HW 0x004369c0
void C2_HOOK_FASTCALL ExpandCrush(tCar_crush* pDest, const tCompressed_car_crush* pSrc) {
    int i;

    pDest->count = pSrc->count;
    ExpandVector3(&pDest->vectors[0], &pSrc->vectors[0], -2.5f, 2.5f);
    BrVector3Copy(&pDest->field_0x34, &pSrc->vectors[0]);
    for (i = 1; i < pSrc->count; i++) {
        ExpandVector3(&pDest->vectors[i], &pSrc->vectors[i], -2.5f, 2.5f);
        BrVector3Accumulate(&pDest->field_0x34, &pDest->vectors[i]);
    }
    BrVector3InvScale(&pDest->field_0x34, &pDest->field_0x34, (float)pDest->count);
    pDest->field_0x40 = DRU16ToScalar(pSrc->field_0x1a, -1.f, 1.f);
    ExpandVector3(&pDest->field_0x44, &pSrc->field_0x1c, -1.f, 1.f);
    pDest->field_0x50 = pSrc->field_0x24;
}

void C2_HOOK_FASTCALL SendCrush(tCar_spec* pCar, tCompressed_car_crush* pCrush) {
    tNet_message_chunk* message;

    message = NetStartBroadcastContents(eNet_message_chunk_type_car_crush, 0);
    if (message != NULL) {
        message->car_crush.car_id = NetPlayerFromCar(pCar)->ID;
        memcpy(&message->car_crush.car_crush, pCrush, sizeof(tCompressed_car_crush));
        NetBroadcastContents(message);
    }
}

void C2_HOOK_FASTCALL DoCrushing(void) {
    int list_i;

    C2_HOOK_BUG_ON(sizeof(tCar_crush) != 0x54);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, car_crush_spec, 0x18d4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_crush_spec, bendability_factor, 0x148);

    for (list_i = 0; gCrush_lists[list_i].car_spec != NULL; list_i++) {
        int i;
        tCar_spec* car = gCrush_lists[list_i].car_spec;

        for (i = 0; i < gCrush_lists[list_i].count; i++) {
            tCar_crush* crush;
            float amount;

            crush = &gCrush_lists[list_i].items[i];
            amount = car->car_crush_spec->bendability_factor * crush->field_0x40;
            if (!(car != NULL && car->driver == eDriver_local_human)) {
                amount *= 1.f - (1.f - ((float)gCurrent_race.index / (float)gNumber_of_races) * 99.f / 20.f);
            }
            if (amount > gMin_bend_force) {
                if (PercentageChance((int)gChance_of_bending)) {
                    crush->field_0x40 = -crush->field_0x40;
                } else {
                    if (gNet_mode != eNet_mode_none) {
                        tCompressed_car_crush compressed_crush;

                        CompressCrush(&compressed_crush, crush);
                        SendCrush(car, &compressed_crush);
                        ExpandCrush(crush, &compressed_crush);
                    }
                    CrushCar(car, car->car_master_actor, crush);
                }
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x0042d180
void C2_HOOK_FASTCALL CrushDroneObject(undefined4* pArg1, int pArg2, undefined* pArg3, br_vector3* pArg4) {

    NOT_IMPLEMENTED();
}

// C2PORT-NONMATCHING: best 48.97% | difftest PASS 2000 trials (spec difftest/specs/0042d400_FUN_0042d400.json) | reason x87-order/stack-layout
// FUNCTION: CARMA2_HW 0x0042d400
void C2_HOOK_FASTCALL FUN_0042d400(tCar_spec* pCar, int pPoint_count, const br_vector3* pPoints, const br_vector3* pForce, undefined4 pArg5) {
    int point_count;
    int unique_count;
    int point_i;
    int list_i;
    int crush_i;
    int i;
    float force_length;
    float force_magnitude;
    float scale;
    int is_unique;
    br_vector3 unique_points[4];
    br_vector3 average;
    br_vector3 scaled_force;
    union {
        float f;
        int i;
    } force_magnitude_arg;

    if (pCar->car_crush_spec == NULL || *(int*)((tU8*)pCar + 0x1b4) != 0 || pCar->car_crush_spec->field_0x4b8 != 0) {
        return;
    }

    force_length = (float)sqrt((double)(pForce->v[0] * pForce->v[0] + pForce->v[1] * pForce->v[1] + pForce->v[2] * pForce->v[2]));
    force_magnitude = pCar->softness_factor * force_length;
    if (!(force_magnitude > gMin_crush_force)) {
        return;
    }
    if (force_magnitude > gFloat_00589a80) {
        force_magnitude = 10.0f;
    }

    scale = (float)(gDOUBLE_00589a60 / force_length);
    scaled_force.v[0] = pForce->v[0] * scale;
    scaled_force.v[1] = pForce->v[1] * scale;
    scaled_force.v[2] = pForce->v[2] * scale;

    unique_count = 1;
    unique_points[0] = pPoints[0];
    average = pPoints[0];
    point_count = pPoint_count;
    if (point_count > 4) {
        point_count = 4;
    }
    for (point_i = 1; point_i < point_count; point_i++) {
        is_unique = 1;
        for (i = 0; i < unique_count; i++) {
            if (fabs((double)pPoints[point_i].v[0] - (double)unique_points[i].v[0]) < gDOUBLE_00589a88 &&
                fabs((double)pPoints[point_i].v[1] - (double)unique_points[i].v[1]) < gDOUBLE_00589a88 &&
                fabs((double)pPoints[point_i].v[2] - (double)unique_points[i].v[2]) < gDOUBLE_00589a88) {
                is_unique = 0;
                break;
            }
        }
        if (is_unique) {
            unique_points[unique_count] = pPoints[point_i];
            average.v[0] += pPoints[point_i].v[0];
            average.v[1] += pPoints[point_i].v[1];
            average.v[2] += pPoints[point_i].v[2];
            unique_count++;
        }
    }
    average.v[0] /= (float)unique_count;
    average.v[1] /= (float)unique_count;
    average.v[2] /= (float)unique_count;

    for (list_i = 0; list_i < 8; list_i++) {
        if (gCrush_lists[list_i].car_spec == pCar || gCrush_lists[list_i].car_spec == NULL) {
            break;
        }
    }
    if (list_i >= 8) {
        return;
    }

    force_magnitude_arg.f = force_magnitude;
    if (gCrush_lists[list_i].car_spec == NULL) {
        gCrush_lists[list_i].car_spec = pCar;
        gCrush_lists[list_i].count = 1;
        crush_i = 0;
    } else if (gCrush_lists[list_i].count < 4) {
        crush_i = gCrush_lists[list_i].count;
        gCrush_lists[list_i].count++;
    } else {
        float weakest_force;

        weakest_force = gCrush_lists[list_i].items[0].field_0x40;
        crush_i = 0;
        for (i = 1; i < gCrush_lists[list_i].count; i++) {
            if (gCrush_lists[list_i].items[i].field_0x40 < weakest_force) {
                weakest_force = gCrush_lists[list_i].items[i].field_0x40;
                crush_i = i;
            }
        }
        if (!(weakest_force < force_magnitude)) {
            return;
        }
    }

    SetCrush((int*)&gCrush_lists[list_i].items[crush_i], unique_count, (int*)unique_points, (int*)&average, (int*)&scaled_force, force_magnitude_arg.i, pArg5);
}

void C2_HOOK_FASTCALL DoDroneCrushing(void) {
    int i;

    C2_HOOK_BUG_ON(sizeof(tQueued_drone_crush) != 0x90);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tQueued_drone_crush, field_0x0, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tQueued_drone_crush, count, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tQueued_drone_crush, items, 0x8);
    C2_HOOK_BUG_ON(sizeof(tQueued_drone_crush_item) != 0x44);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tQueued_drone_crush_item, count, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tQueued_drone_crush_item, points, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tQueued_drone_crush_item, field_0x34, 0x34);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tQueued_drone_crush_item, field_0x38, 0x38);

    for (i = 0; i < gCount_queued_drone_crushes; i++) {
        tQueued_drone_crush* queued_drone_crush;
        int j;

        queued_drone_crush = &gQueued_drone_crushes[i];
        for (j = 0; j < queued_drone_crush->count; j++) {
            br_vector3 tv;

            if (queued_drone_crush->items[j].field_0x34 > 5.f) {
                queued_drone_crush->items[j].field_0x34 = 5.f;
            }
            BrVector3Scale(&tv, &queued_drone_crush->items[j].field_0x38, queued_drone_crush->items[j].field_0x34);
            CrushDroneObject(queued_drone_crush->field_0x0, queued_drone_crush->items[j].count, queued_drone_crush->items[j].points, &tv);
        }
    }
    gCount_queued_drone_crushes = 0;
}

void C2_HOOK_FASTCALL DoShapeSwapping(void) {
    int i;

    for (i = 0; i < gNum_active_cars; i++) {
        tCar_spec* car;

        car = gActive_car_list[i];
        if (car != NULL && car->driver >= eDriver_oppo && car->car_crush_spec != NULL && (car->car_crush_spec->expand_bounding_box & 0x1)) {
            SwapShapesIfPossible(car);
        }
    }
}

// FUNCTION: CARMA2_HW 0x00436170
void C2_HOOK_FASTCALL CrushBendFlapRend(void) {

    DoSpams();
    DoCrushing();
    DoDroneCrushing();
    DoShapeSwapping();
    DoFlapping();
    DoDetaching();
    DoFullyDetaching();
    DoBending();
    ClearCrushLists();
}

#pragma inline_depth(0)
// FUNCTION: CARMA2_HW 0x004387d0
void C2_HOOK_FASTCALL StopFlapping(br_actor* pActor, tCar_spec* pCar_spec) {
    tUser_crush_data* user;
    tCar_crush_buffer_entry* crush_data;
    tCar_crush_flap_data* flap_data;

    user = pActor->user;
    if (user == NULL) {
        return;
    }
    crush_data = user->crush_data;
    if (crush_data == NULL) {
        return;
    }
    flap_data = crush_data->flap_data;
    if (flap_data == NULL) {
        return;
    }
    if (flap_data->kev_o_flap) {
        if (gNet_mode != eNet_mode_none) {
            if (gNet_mode == eNet_mode_host) {
                tPhysics_object* object;
                tNet_message* message;

                object = flap_data->physics_object;
                message = NetBuildGuaranteedMessage(0x26, 0);
                message->guaranteed.contents.detach_bit.ID = NetPlayerFromCar(pCar_spec)->ID;
                message->guaranteed.contents.detach_bit.field_0x4 = object->uid;
                message->guaranteed.contents.detach_bit.field_0x8 = gPHIL_last_physics_tick + 120;
                NetGuaranteedSendMessageToEverybody(gCurrent_net_game, message, 0);
            }
        } else {
            ReAttachBit(pCar_spec, pCar_spec->car_model_actor, pActor, flap_data->physics_object);
            MungeDetachLists(pCar_spec->car_crush_spec);
        }
    } else {
        pCar_spec->car_crush_spec->field_0x134 -= 1;
        BrMatrix34Identity(&pActor->t.t.mat);
        PipeSingleFlap(pActor, -flap_data->field_0x4);
        flap_data->field_0x4 = 0;
        flap_data->field_0x2 = 0;
        flap_data->field_0x6 = 0x3ffc;
        flap_data->field_0x8 = 0x3ffc;
        flap_data->field_0x0 = 0;
    }
    DRS3StartSound3D(gXXX_outlet, 0x1482, &pCar_spec->car_master_actor->t.t.translate.t, &gZero_v__car, 1, 255, IRandomBetween(0xe000, 0x12000), 0x10000);
}
#pragma inline_depth()

// FUNCTION: CARMA2_HW 0x004f55d0
intptr_t C2_HOOK_CDECL MungeMaterialCB(br_actor* pActor, void* pData) {
    tMungeMaterialCB_Context* context;
    br_face* face;
    int face_i;
    int i;
    int j;
    int found;

    context = pData;
    if (pActor->model != NULL) {
        for (face_i = 0, face = pActor->model->faces; face_i < pActor->model->nfaces; face_i++, face++) {
            if (face->material == context->material_0x0) {
                face->material = context->material_0x4;
                if (context->indices != NULL && context->count_indices < context->capacity_indices) {
                    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(face->vertices); i++) {
                        found = 0;
                        for (j = 0; j < context->count_indices; j++) {
                            if (face->vertices[i] == context->indices[j]) {
                                if (context->vertices != NULL && context->count_vertices < context->capacity_vertices) {
                                    context->vertices[context->count_vertices].field_0x0[i] = (tU16)j;
                                }
                                found = 1;
                                break;
                            }
                        }
                        if (!found) {
                            if (context->count_indices != 0) {
                                BrVector3Accumulate(&context->field_0x20, &face->n);
                            } else {
                                BrVector3Copy(&context->field_0x20, &face->n);
                            }
                            if (context->vertices != NULL && context->count_vertices < context->capacity_vertices) {
                                context->vertices[context->count_vertices].field_0x0[i] = (tU16)context->count_indices;
                            }
                            context->indices[context->count_indices] = face->vertices[i];
                            context->count_indices++;
                        }
                    }
                }
                if (context->vertices != NULL && context->count_vertices < context->capacity_vertices) {
                    context->count_vertices++;
                }
            }
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004f54d0
void C2_HOOK_FASTCALL MungeMaterial(br_actor* pActor, int pRecursive, br_material* pMaterial_1, br_material* pMaterial_2, int pCapacity_indices, tU16* pIndices, int* pCount_indices, int pCapacity_vertices, tCar_crush_vertex_data* pVertices, int* pCount_vertices, br_vector3* pNormal) {
    tMungeMaterialCB_Context munge_material_cb_ctx;

    munge_material_cb_ctx.material_0x0 = pMaterial_1;
    munge_material_cb_ctx.material_0x4 = pMaterial_2;
    munge_material_cb_ctx.capacity_indices = pCapacity_indices;
    munge_material_cb_ctx.indices = pIndices;
    munge_material_cb_ctx.count_indices = 0;
    munge_material_cb_ctx.capacity_vertices = pCapacity_vertices;
    munge_material_cb_ctx.vertices = pVertices;
    munge_material_cb_ctx.count_vertices = 0;

    if (pRecursive) {
        DRActorEnumRecurse(pActor, MungeMaterialCB, &munge_material_cb_ctx);
    } else {
        MungeMaterialCB(pActor, &munge_material_cb_ctx);
    }
    if (munge_material_cb_ctx.count_indices != 0) {
        BrVector3Normalise(pNormal, &munge_material_cb_ctx.field_0x20);
    }
    *pCount_indices = munge_material_cb_ctx.count_indices;
    *pCount_vertices = munge_material_cb_ctx.count_vertices;
}

// FUNCTION: CARMA2_HW 0x004f57c0
void C2_HOOK_FASTCALL LinkSmashies(br_actor* pActor, tCar_crush_buffer_entry* pCrush_data, tModel_detail_vertex_data* pVertex_data) {

    int i;

    for (i = 0; i < pCrush_data->count_smashables; i++) {
        tCar_crush_smashable_part* smashable;
        int j;
        tU16 indices[50];
        int count_indices;
        tCar_crush_vertex_data vertices[50];
        int count_vertices;

        C2_HOOK_BUG_ON(sizeof(vertices[0]) != 6);
        C2_HOOK_BUG_ON(sizeof(vertices[0].field_0x0[0]) != 2);

        smashable = &pCrush_data->smashables[i];
        smashable->funk_material = FindMaterial(smashable->material_name, pActor, 0);
        if (smashable->funk_material == NULL) {
            FatalError(kFatalError_CannotFindSmashMaterial_S, smashable->material_name);
        }
        MungeMaterial(pActor, 0, smashable->funk_material, smashable->funk_material,
            CARPOCALYPSE2_ASIZE(indices), indices, &count_indices,
            CARPOCALYPSE2_ASIZE(vertices), vertices, &count_vertices, &smashable->field_0x60);
        for (j = 0; j < count_indices; j++) {
            if (j == 0) {
                BrVector3Copy(&smashable->field_0x54, &pActor->model->vertices[indices[j]].p);
            } else {
                BrVector3Accumulate(&smashable->field_0x54, &pActor->model->vertices[indices[j]].p);
            }
        }
        if (count_indices != 0) {
            BrVector3InvScale(&smashable->field_0x54, &smashable->field_0x54, (float)count_indices);
        }
        smashable->field_0x6c = count_indices;
        if (count_indices != 0) {
            smashable->field_0x70 = BrMemAllocate(count_indices * sizeof(tU16), kMem_smash_poly_array);
            memcpy(smashable->field_0x70, indices, count_indices * sizeof(tU16));
        } else {
            smashable->field_0x70 = NULL;
        }
        smashable->field_0x74 = count_vertices;
        if (count_vertices != 0) {
            smashable->field_0x78 = BrMemAllocate(count_vertices * sizeof(tCar_crush_vertex_data), kMem_smash_vertex_array);
            memcpy(smashable->field_0x78, vertices, count_vertices * sizeof(tCar_crush_vertex_data));
        } else {
            smashable->field_0x78 = NULL;
        }
        BrResFree(smashable->funk_material->identifier);
        smashable->funk_material->identifier = NULL;
        smashable->field_0x4c = 0;
        smashable->funk_material->colour_map = smashable->levels->pixelmaps[0];
        BrMaterialUpdate(smashable->funk_material, BR_MATU_COLOURMAP);
    }
}

// FUNCTION: CARMA2_HW 0x00429d90
void C2_HOOK_FASTCALL RemoveCarFromCrushLists(tCar_spec* pCar_spec) {
    int i;
    int new_count;

    C2_HOOK_BUG_ON(sizeof(tCrush_net_detach_list_item) != 0x24);
    C2_HOOK_BUG_ON(sizeof(tCrush_net_semi_detach_bit_list_item) != 0x4c);
    C2_HOOK_BUG_ON(sizeof(tCrush_net_full_detach_bit_list_item) != 0xc);
    C2_HOOK_BUG_ON(sizeof(tCrush_net_reattach_bit_list_item) != 0xc);
    C2_HOOK_BUG_ON(sizeof(tCrush_detach_list_item) != 0x14);
    C2_HOOK_BUG_ON(sizeof(tToggled_door) != 0x10);
    C2_HOOK_BUG_ON(sizeof(tCar_damge_crush_list_item) != 0x8);

    new_count = 0;
    for (i = 0; i < gCount_net_crush_detach_list; i++) {
        if (gNet_crush_detach_list[i].car != pCar_spec) {
            gNet_crush_detach_list[new_count] = gNet_crush_detach_list[i];
            new_count += 1;
        }
    }
    gCount_net_crush_detach_list = new_count;

    new_count = 0;
    for (i = 0; i < gCount_net_crush_semi_detach_bit_list; i++) {
        if (gNet_crush_semi_detach_bit_list[i].car != pCar_spec) {
            gNet_crush_semi_detach_bit_list[new_count] = gNet_crush_semi_detach_bit_list[i];
            new_count += 1;
        }
    }
    gCount_net_crush_semi_detach_bit_list = new_count;

    new_count = 0;
    for (i = 0; i < gCount_net_crush_full_detach_bit_list; i++) {
        if (gNet_crush_full_detach_bit_list[i].car != pCar_spec) {
            gNet_crush_full_detach_bit_list[new_count] = gNet_crush_full_detach_bit_list[i];
            new_count += 1;
        }
    }
    gCount_net_crush_full_detach_bit_list = new_count;

    new_count = 0;
    for (i = 0; i < gCount_net_crush_reattach_bit_list; i++) {
        if (gNet_crush_reattach_bit_list[i].car != pCar_spec) {
            gNet_crush_reattach_bit_list[new_count] = gNet_crush_reattach_bit_list[i];
            new_count += 1;
        }
    }
    gCount_net_crush_reattach_bit_list = new_count;

    new_count = 0;
    for (i = 0; i < gCount_crush_detach_list; i++) {
        if (gCrush_detach_list[i].car != pCar_spec) {
            gCrush_detach_list[new_count] = gCrush_detach_list[i];
            new_count += 1;
        }
    }
    gCount_crush_detach_list = new_count;

    new_count = 0;
    for (i = 0; i < gCount_toggled_doors; i++) {
        if (gToggled_doors[i].car != pCar_spec) {
            gToggled_doors[new_count] = gToggled_doors[i];
            new_count += 1;
        }
    }
    gCount_toggled_doors = new_count;

    new_count = 0;
    for (i = 0; i < gCount_car_damage_crush_list; i++) {
        if (gCar_damage_crush_list[i].car != pCar_spec) {
            gCar_damage_crush_list[new_count] = gCar_damage_crush_list[i];
            new_count += 1;
        }
    }
    gCount_car_damage_crush_list = new_count;
}

extern const float gUNK_00589fc4;
extern const double gDouble_00589fe8;
extern const double gDouble_00589fd8;

// GLOBAL: CARMA2_HW 0x00589fb0
float gUNK_00589fb0 = 1.0f;

// GLOBAL: CARMA2_HW 0x00589fb8
float gUNK_00589fb8 = -1.0f;

// FUNCTION: CARMA2_HW 0x00461740
int C2_HOOK_FASTCALL ShapeRayCast(const br_vector3* p1, const br_vector3* p2, const tPhysics_shape* pShape, br_vector3* pPos, float* pFactor, br_vector3* pNormal) {
    int index;
    int result;

    switch (pShape->common.type) {
    case kCollisionShapeType_Box:
        result = LineBoxColl((br_vector3*)p1, (br_vector3*)p2, (br_bounds*)&pShape->box.common.bb, pPos, pFactor);
        if (pNormal != NULL && result > 0) {
            index = (result - 1) & 3;
            pNormal->v[0] = 0.0f;
            pNormal->v[1] = 0.0f;
            pNormal->v[2] = 0.0f;
            pNormal->v[index] = (result & 4) ? 1.0f : -1.0f;
        }
        return result;
    case kCollisionShapeType_Polyhedron:
    case kCollisionShapeType_Wireframe_Polyhedron:
        result = LinePolyColl((br_vector3*)p1, (br_vector3*)p2, (tCollision_shape_polyhedron_data*)&pShape->polyhedron.polyhedron, pPos, 0, pFactor);
        if (pNormal != NULL && result > 0) {
            index = (result & 0x3f) - 1;
            pNormal->v[0] = pShape->polyhedron.polyhedron.planes[index].v[0];
            pNormal->v[1] = pShape->polyhedron.polyhedron.planes[index].v[1];
            pNormal->v[2] = pShape->polyhedron.polyhedron.planes[index].v[2];
        }
        return result;
    case kCollisionShapeType_Sphere:
        return LineSphereColl((br_vector3*)p1, (br_vector3*)p2, (tCollision_shape_sphere_data*)&pShape->sphere.sphere, pPos, pNormal, pFactor);
    default:
        PhysicsError(ePhysicsError_UnknownShapeType, "");
    case kCollisionShapeType_Wireframe:
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x0043f5f0
void C2_HOOK_FASTCALL KnackerThisCar(tCar_spec* pCar) {

    if (pCar->invulnerable_no_wastage) {
        return;
    }
    if (gNet_mode == eNet_mode_client) {
        return;
    }
    if (gNet_mode == eNet_mode_none && pCar != NULL && pCar->driver == eDriver_local_human) {
        RaceCompleted(eRace_over_6);
    }
    pCar->knackered = 1;
    QueueWastedMessage(pCar->index);
    if (gNet_mode == eNet_mode_none) {
        if (GetCarCount(eVehicle_opponent) != 0 && NumberOfOpponentsLeft() == 0) {
            if (gCurrent_race.race_spec->race_type == kRaceType_Carma1
                || gCurrent_race.race_spec->race_type == kRaceType_Cars) {
                if (gCurrent_race.race_spec->race_type == kRaceType_Carma1) {
                    NewTextHeadupSlot(4, 0, 5000, -4, GetMiscString(eMiscString_every_opponent_wasted));
                }
                RaceCompleted(eRace_over_2);
            }
        }
    }
    QueueOilSpill(pCar);
    if (gNet_mode == eNet_mode_none) {
        KillGroovadelic(pCar->index);
        KillFunkotronic(pCar->index);
    }
}

// FUNCTION: CARMA2_HW 0x0043f9f0
void C2_HOOK_FASTCALL RecordLastDamage(tCar_spec* pCar) {
    int i;

    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(pCar->damage_units); i++) {
        pCar->damage_units[i].last_level = pCar->damage_units[i].damage_level;
    }
    pCar->damage_magnitude_accumulator = 0.0f;
    pCar->last_impact_location = eImpact_unknown;
    pCar->pre_car_col_mat = pCar->car_master_actor->t.t.mat;
    pCar->pre_car_col_speed = pCar->speed;
    pCar->pre_car_col_knackered = pCar->knackered;
    pCar->pre_car_col_direction = pCar->direction;
    pCar->pre_car_col_velocity = pCar->collision_info->v;
    pCar->pre_car_col_velocity_car_space = pCar->collision_info->velocity_car_space;
}

// FUNCTION: CARMA2_HW 0x00440210
void C2_HOOK_FASTCALL SetSmokeLastDamageLevel(tCar_spec* pCar) {
    int i;

    for (i = 0; i < CARPOCALYPSE2_ASIZE(pCar->damage_units); i++) {
        pCar->damage_units[i].smoke_last_level = pCar->damage_units[i].damage_level;
    }
}

// C2PORT-NONMATCHING: best 14.86% | difftest PASS 2000 trials (spec difftest/specs/0043a5b0_FUN_0043a5b0.json) | reason x87-order
// FUNCTION: CARMA2_HW 0x0043a5b0
void C2_HOOK_FASTCALL FUN_0043a5b0(void* pElement, void* pArg2, void* pArg3) {
    br_model* pModel = (br_model*)pElement;
    br_vector3* pOffset = (br_vector3*)pArg2;
    tU8* pFrame = (tU8*)pArg3;
    tU8* pLattice;
    tCompressed_vector3* pCompressed;
    tModel_detail_vertex_data* pDetail;
    int vector_index;
    int dirty = 0;
    int i;
    float mash[1024 * 3];

    if (pModel == NULL || pModel->user == NULL) {
        return;
    }
    pDetail = ((tUser_detail_level_model*)pModel->user)->field_0x4;
    if (pDetail == NULL) {
        return;
    }
    if (pModel->nvertices > 0x400) {
        PDFatalError("My Lord! There's a lot of vertices on that model.");
    }
    pLattice = *(tU8**)(pFrame + 0x60);
    vector_index = *(tU16*)((tU8*)pModel->user + 8);
    pCompressed = *(tCompressed_vector3**)(pLattice + 0x78);

    for (i = 0; i < pModel->nvertices; i++) {
        tU8 flags = pDetail[i].flags;
        br_vector3* pVert = &pModel->vertices[i].p;

        if ((flags & 0x8) == 0) {
            mash[i * 3 + 0] = 0.f;
            mash[i * 3 + 1] = 0.f;
            mash[i * 3 + 2] = 0.f;
            continue;
        }
        dirty = 1;
        if ((flags & 0x10) == 0) {
            br_vector3 e;
            float s;
            float t;
            float u;
            float st;
            float oldx;
            float oldy;
            float oldz;
            float dx;
            float dy;
            float dz;
            float nx;
            float ny;
            float nz;
            float fx;
            float fy;
            float fz;

            ExpandVector3(&e, &pCompressed[vector_index], -10.f, 10.f);
            vector_index++;
            s = (e.v[0] - *(float*)(pLattice + 0x24)) / *(float*)(pLattice + 0x30);
            t = (e.v[1] - *(float*)(pLattice + 0x28)) / *(float*)(pLattice + 0x34);
            u = (e.v[2] - *(float*)(pLattice + 0x2c)) / *(float*)(pLattice + 0x38);
            st = s * t;
            oldx = pVert->v[0];
            oldy = pVert->v[1];
            oldz = pVert->v[2];
            mash[i * 3 + 0] = -oldx;
            mash[i * 3 + 1] = -oldy;
            mash[i * 3 + 2] = -oldz;
            dx = e.v[0] - pOffset->v[0];
            dy = e.v[1] - pOffset->v[1];
            dz = e.v[2] - pOffset->v[2];
            nx = dx + (*(float*)(pFrame + 0x54) + *(float*)(pFrame + 0x48) * t + *(float*)(pFrame + 0x3c) * s + *(float*)(pFrame + 0x30) * st) * u;
            pVert->v[0] = nx;
            ny = dy + (*(float*)(pFrame + 0x58) + *(float*)(pFrame + 0x4c) * t + *(float*)(pFrame + 0x40) * s + *(float*)(pFrame + 0x34) * st) * u;
            pVert->v[1] = ny;
            fx = ((oldx - dx) - (*(float*)(pLattice + 0x68) + *(float*)(pLattice + 0x5c) * t + *(float*)(pLattice + 0x50) * s + *(float*)(pLattice + 0x44) * st) * u) + pVert->v[0];
            nz = dz + (*(float*)(pFrame + 0x5c) + *(float*)(pFrame + 0x50) * t + *(float*)(pFrame + 0x44) * s + *(float*)(pFrame + 0x38) * st) * u;
            pVert->v[2] = nz;
            pVert->v[0] = fx;
            fy = ((oldy - dy) - (*(float*)(pLattice + 0x6c) + *(float*)(pLattice + 0x60) * t + *(float*)(pLattice + 0x54) * s + *(float*)(pLattice + 0x48) * st) * u) + pVert->v[1];
            fz = ((oldz - dz) - (*(float*)(pLattice + 0x70) + *(float*)(pLattice + 0x64) * t + *(float*)(pLattice + 0x58) * s + *(float*)(pLattice + 0x4c) * st) * u) + pVert->v[2];
            pVert->v[1] = fy;
            pVert->v[2] = fz;
            mash[i * 3 + 0] = fx + mash[i * 3 + 0];
            mash[i * 3 + 1] = mash[i * 3 + 1] + fy;
            mash[i * 3 + 2] = mash[i * 3 + 2] + fz;
        } else {
            br_vector3 b;
            br_vector3 out;

            mash[i * 3 + 0] = -pVert->v[0];
            mash[i * 3 + 1] = -pVert->v[1];
            mash[i * 3 + 2] = -pVert->v[2];
            b.v[0] = pVert->v[0] + pOffset->v[0];
            b.v[1] = pVert->v[1] + pOffset->v[1];
            b.v[2] = pVert->v[2] + pOffset->v[2];
            BrMatrix34ApplyP(&out, &b, (br_matrix34*)pFrame);
            pVert->v[0] = out.v[0] - pOffset->v[0];
            pVert->v[1] = out.v[1] - pOffset->v[1];
            pVert->v[2] = out.v[2] - pOffset->v[2];
            mash[i * 3 + 0] = mash[i * 3 + 0] + pVert->v[0];
            mash[i * 3 + 1] = mash[i * 3 + 1] + pVert->v[1];
            mash[i * 3 + 2] = mash[i * 3 + 2] + pVert->v[2];
        }
    }

    if (dirty) {
        if ((pModel->flags & BR_MODF_CUSTOM) == 0 || pModel->user == NULL) {
            pModel->custom = JitModelUpdate;
            pModel->flags |= BR_MODF_CUSTOM;
        } else {
            *(tU32*)pModel->user = 1;
        }
        AddModelMashToPipingSession(pModel, (br_vector3*)mash);
    }
}

// FUNCTION: CARMA2_HW 0x0043a560
int C2_HOOK_FASTCALL FUN_0043a560(void* pThis, void* pArg2, void* pArg3) {
    int i;
    int count;
    int** elem;

    elem = *(int***)((tU8*)pThis + 0x60);
    if (elem == NULL) {
        return 0;
    }
    if ((int)elem[2] != 0) {
        i = 0;
        if (0 < *(int*)(*(int*)((tU8*)pArg3 + 0x64) + 0xe14)) {
            do {
                FUN_0043a5b0(*elem, pArg2, pArg3);
                i++;
                elem++;
                count = *(int*)(*(int*)((tU8*)pArg3 + 0x64) + 0xe14);
            } while (i < count);
        }
    }
    return 0;
}

#pragma auto_inline(off)

typedef struct {
    tCar_spec* car_spec;
    float step;
    float repaired;
    float deflection;
} tRepairCarContext;

static intptr_t C2_HOOK_CDECL RepairCarCB(br_actor* pActor, void* pData) {
    tRepairCarContext* ctx = pData;
    tUser_crush_data* user_crush_data;
    int i;

    user_crush_data = pActor->user;
    if (user_crush_data == NULL || user_crush_data->crush_data == NULL) {
        return 0;
    }
    for (i = 0; i < ctx->car_spec->count_detail_levels; i++) {
        br_model* model = user_crush_data->models[i];
        int j;

        if (model == NULL || model->user == NULL || model->user == (void*)(uintptr_t)1 || model->nvertices == 0) {
            continue;
        }
        /* tUser_detail_level_model with original (undamaged) vertices in field_0x4. */
        {
            tModel_detail_vertex_data* vertex_data = ((tUser_detail_level_model*)model->user)->field_0x4;
            int touched;

            if (vertex_data == NULL) {
                continue;
            }
            touched = 0;
            for (j = 0; j < model->nvertices; j++) {
                br_vector3 undamaged;
                int k;

                ExpandVector3(&undamaged, &vertex_data[j].p, -10.f, 10.f);
                for (k = 0; k < 3; k++) {
                    float deviation = fabsf(undamaged.v[k] - model->vertices[j].p.v[k]);
                    float before;

                    ctx->deflection += deviation;
                    if (undamaged.v[k] == model->vertices[j].p.v[k]) {
                        continue;
                    }
                    before = model->vertices[j].p.v[k];
                    if (undamaged.v[k] < model->vertices[j].p.v[k]) {
                        model->vertices[j].p.v[k] -= ctx->step;
                        if (model->vertices[j].p.v[k] < undamaged.v[k]) {
                            model->vertices[j].p.v[k] = undamaged.v[k];
                        }
                        ctx->repaired += before - model->vertices[j].p.v[k];
                    } else {
                        model->vertices[j].p.v[k] += ctx->step;
                        if (model->vertices[j].p.v[k] > undamaged.v[k]) {
                            model->vertices[j].p.v[k] = undamaged.v[k];
                        }
                        ctx->repaired += model->vertices[j].p.v[k] - before;
                    }
                    touched = 1;
                }
                if (model->vertices[j].p.v[0] == undamaged.v[0]
                    && model->vertices[j].p.v[1] == undamaged.v[1]
                    && model->vertices[j].p.v[2] == undamaged.v[2]) {
                    vertex_data[j].flags &= ~0x4;
                }
            }
            if (touched) {
                SetModelForUpdate(model);
            }
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0043ace0
float C2_HOOK_FASTCALL ComputeCarImpact(tCar_spec* pCar, int pDelta, float* pOut) {
    tRepairCarContext ctx;

    if (pCar == NULL || pCar->car_model_actor == NULL) {
        if (pOut != NULL) {
            *pOut = 0.f;
        }
        return 0.f;
    }
    ctx.car_spec = pCar;
    ctx.step = (float)pDelta * 0.00005f;
    ctx.repaired = 0.f;
    ctx.deflection = 0.f;
    DRActorEnumRecurse(pCar->car_model_actor, RepairCarCB, &ctx);
    if (pOut != NULL) {
        *pOut = ctx.deflection;
    }
    return ctx.repaired;
}

// C2PORT-NONMATCHING: best 86.09% | difftest PASS 2000 trials (spec difftest/specs/0049cd90_SelectCrushNetwork.json) | reason VC5 register and loop layout
// FUNCTION: CARMA2_HW 0x0049cd90
int C2_HOOK_FASTCALL SelectCrushNetwork(undefined4* pField, tPhysics_object* pObject, int pMode) {
    int total_size;
    tU8* current_data;
    tPhysics_object* matching_child;
    tPhysics_object* child;
    tPhysics_object* recursive_child;

    if (pObject != NULL && ((tU8*)pField)[1] != *(tU8*)((tU8*)pObject + 0x190)) {
        PhysicsError(9, "netphys got messed up");
    }
    total_size = SetObjectNetworkStuff((tU8*)pField, pObject, pMode);
    if ((((tU8*)pField)[0] & 0x80) != 0) {
        do {
            current_data = (tU8*)pField + total_size;
            if (pObject != NULL) {
                child = pObject->child;
                while (child != NULL && *(tU8*)((tU8*)child + 0x190) != current_data[1]) {
                    child = child->next;
                }
                matching_child = child;
            } else {
                matching_child = NULL;
            }
            if (pObject != NULL) {
                child = pObject->child;
                while (child != NULL && *(tU8*)((tU8*)child + 0x190) != current_data[1]) {
                    child = child->next;
                }
                recursive_child = child;
            } else {
                recursive_child = NULL;
            }
            total_size += SelectCrushNetwork((undefined4*)current_data, recursive_child, pMode);
            if (matching_child != NULL) {
                matching_child->field_0xf0 = pObject->field_0xf0;
            }
        } while ((current_data[0] & 0x40) != 0);
    }
    return total_size;
}

// C2PORT-NONMATCHING: best 54.75% | difftest PASS 2000 trials (spec difftest/specs/0043a150_FUN_0043a150.json) | reason x87-order
// FUNCTION: CARMA2_HW 0x0043a150
void C2_HOOK_FASTCALL FUN_0043a150(br_actor* pActor, float* pCrush_data, short pAngle_x, short pAngle_y, short pAngle_z, tCar_spec* pCar) {
    br_vector3 local_74;
    tCrushNetworkTransformContext local_68;
    *(short*)((tU8*)pCrush_data + 0x3c) += pAngle_x;
    *(short*)((tU8*)pCrush_data + 0x3e) += pAngle_y;
    *(short*)((tU8*)pCrush_data + 0x40) += pAngle_z;
    local_68.crush_data = pCrush_data;
    local_68.car = pCar;
    BrMatrix34Translate(&local_68.matrix, -pCrush_data[6], -pCrush_data[7], -pCrush_data[8]);
    if (*(short*)((tU8*)pCrush_data + 0x3e) != 0) {
        BrMatrix34PostRotateY(&local_68.matrix, *(short*)((tU8*)pCrush_data + 0x3e));
    }
    if (*(short*)((tU8*)pCrush_data + 0x3c) != 0) {
        BrMatrix34PostRotateX(&local_68.matrix, *(short*)((tU8*)pCrush_data + 0x3c));
    }
    if (*(short*)((tU8*)pCrush_data + 0x40) != 0) {
        BrMatrix34PostRotateZ(&local_68.matrix, *(short*)((tU8*)pCrush_data + 0x40));
    }
    BrMatrix34PostTranslate(&local_68.matrix, pCrush_data[6], pCrush_data[7], pCrush_data[8]);

    local_68.corner_0_x = pCrush_data[3];
    local_68.corner_0_y = pCrush_data[4];
    local_68.corner_0_z = pCrush_data[8];
    local_68.corner_1_x = pCrush_data[3];
    local_68.corner_1_y = pCrush_data[1];
    local_68.corner_1_z = pCrush_data[8];
    local_68.corner_2_x = pCrush_data[0];
    local_68.corner_2_y = pCrush_data[4];
    local_68.corner_2_z = pCrush_data[8];
    local_68.corner_3_x = pCrush_data[0];
    local_68.corner_3_y = pCrush_data[1];
    local_68.corner_3_z = pCrush_data[8];
    BrMatrix34ApplyP(&local_74, (br_vector3*)&local_68.corner_0_x, &local_68.matrix);
    local_68.corner_0_x = local_74.v[0] - local_68.corner_0_x;
    local_68.corner_0_y = local_74.v[1] - local_68.corner_0_y;
    local_68.corner_0_z = local_74.v[2] - local_68.corner_0_z;
    BrMatrix34ApplyP(&local_74, (br_vector3*)&local_68.corner_1_x, &local_68.matrix);
    local_68.corner_1_x = local_74.v[0] - local_68.corner_1_x;
    local_68.corner_1_y = local_74.v[1] - local_68.corner_1_y;
    local_68.corner_1_z = local_74.v[2] - local_68.corner_1_z;
    BrMatrix34ApplyP(&local_74, (br_vector3*)&local_68.corner_2_x, &local_68.matrix);
    local_68.corner_2_x = local_74.v[0] - local_68.corner_2_x;
    local_68.corner_2_y = local_74.v[1] - local_68.corner_2_y;
    local_68.corner_2_z = local_74.v[2] - local_68.corner_2_z;
    BrMatrix34ApplyP(&local_74, (br_vector3*)&local_68.corner_3_x, &local_68.matrix);
    local_68.corner_3_x = local_74.v[0] - local_68.corner_3_x;
    local_68.corner_3_y = local_74.v[1] - local_68.corner_3_y;
    local_68.corner_3_z = local_74.v[2] - local_68.corner_3_z;

    local_68.corner_0_x = ((local_68.corner_0_x - local_68.corner_1_x) - local_68.corner_2_x) + local_68.corner_3_x;
    local_68.corner_0_y = ((local_68.corner_0_y - local_68.corner_1_y) - local_68.corner_2_y) + local_68.corner_3_y;
    local_68.corner_0_z = ((local_68.corner_0_z - local_68.corner_1_z) - local_68.corner_2_z) + local_68.corner_3_z;
    local_68.corner_1_x = local_68.corner_1_x - local_68.corner_3_x;
    local_68.corner_1_y = local_68.corner_1_y - local_68.corner_3_y;
    local_68.corner_1_z = local_68.corner_1_z - local_68.corner_3_z;
    local_68.corner_2_x = local_68.corner_2_x - local_68.corner_3_x;
    local_68.corner_2_y = local_68.corner_2_y - local_68.corner_3_y;
    local_68.corner_2_z = local_68.corner_2_z - local_68.corner_3_z;

    BrMatrix34Translate(&local_68.matrix, -pCrush_data[6], -pCrush_data[7], -pCrush_data[8]);
    if (pAngle_y != 0) {
        BrMatrix34PostRotateY(&local_68.matrix, pAngle_y);
    }
    if (pAngle_x != 0) {
        BrMatrix34PostRotateX(&local_68.matrix, pAngle_x);
    }
    if (pAngle_z != 0) {
        BrMatrix34PostRotateZ(&local_68.matrix, pAngle_z);
    }
    BrMatrix34PostTranslate(&local_68.matrix, pCrush_data[6], pCrush_data[7], pCrush_data[8]);
    ARStartPipingSession(ePipe_model_mash);
    DRActorEnumRecurseWithTranslation(pActor, NULL, (tDRActorEnumRecurseWithTranslation_cbfn*)FUN_0043a560, &local_68);
    AREndPipingSession();

    pCrush_data[0x11] = local_68.corner_0_x;
    pCrush_data[0x12] = local_68.corner_0_y;
    pCrush_data[0x13] = local_68.corner_0_z;
    pCrush_data[0x14] = local_68.corner_1_x;
    pCrush_data[0x15] = local_68.corner_1_y;
    pCrush_data[0x16] = local_68.corner_1_z;
    pCrush_data[0x17] = local_68.corner_2_x;
    pCrush_data[0x18] = local_68.corner_2_y;
    pCrush_data[0x19] = local_68.corner_2_z;
    pCrush_data[0x1a] = local_68.corner_3_x;
    pCrush_data[0x1b] = local_68.corner_3_y;
    pCrush_data[0x1c] = local_68.corner_3_z;
}

// C2PORT-NONMATCHING: best 25.83% | difftest PASS 2000 trials (spec difftest/specs/00439c30_DoCrushNetwork.json) | reason VC5 register and stack layout
// FUNCTION: CARMA2_HW 0x00439c30
float C2_HOOK_FASTCALL DoCrushNetwork(tCar_spec* pCar, int pAmount, float* pOut) {
    tU16 value_x;
    tU16 value_y;
    tU16 value_z;
    float total;
    float delta_y_value;
    float delta_z_value;
    tU16 amount;
    tU16 converted_x;
    tU16 converted_y;
    tU16 converted_z;
    tU16 normalized_x;
    int delta_x;
    int delta_y;
    int delta_z;
    int delta_amount;
    int i;
    tCar_crush_spec* pCrush;
    float* pCrush_data;
    br_matrix34 matrix;

    pCrush = pCar->car_crush_spec;
    pCrush_data = (float*)((tU8*)pCrush + 0x154);
    delta_amount = pAmount * 8;
    amount = (tU16)delta_amount;
    value_x = pCrush->field_0x190;
    value_y = pCrush->field_0x192;
    value_z = pCrush->field_0x194;

    if (value_x < 0x3ffc) {
        if (value_x < amount) {
            delta_x = -(int)value_x;
        } else {
            delta_x = -delta_amount;
        }
    } else {
        delta_x = -(int)value_x;
        if ((tU16)delta_x >= amount) {
            delta_x = delta_amount;
        }
    }
    if (value_y < 0x3ffc) {
        if (value_y < amount) {
            delta_y = -(int)value_y;
        } else {
            delta_y = -delta_amount;
        }
    } else {
        delta_y = -(int)value_y;
        if ((tU16)delta_y >= amount) {
            delta_y = delta_amount;
        }
    }
    if (value_z < 0x3ffc) {
        if (value_z < amount) {
            delta_z = -(int)value_z;
        } else {
            delta_z = -delta_amount;
        }
    } else {
        delta_z = -(int)value_z;
        if ((tU16)delta_z >= amount) {
            delta_z = delta_amount;
        }
    }

    converted_x = value_x < 0x7ff8 ? value_x : (tU16)-value_x;
    converted_y = value_y < 0x7ff8 ? value_y : (tU16)-value_y;
    converted_z = value_z < 0x7ff8 ? value_z : (tU16)-value_z;
    total = (float)converted_x * gDouble_00589b40;
    total += (float)converted_y * gDouble_00589b40;
    total += (float)converted_z * gDouble_00589b40;
    delta_y_value = NormalizedAngle((tU16)delta_y);
    delta_z_value = NormalizedAngle((tU16)delta_z);
    *pOut = total + total;
    FUN_0043a150(pCar->car_master_actor, pCrush_data, (short)delta_x, (short)delta_y, (short)delta_z, pCar);

    BrMatrix34Translate(&matrix, -pCrush_data[6], -pCrush_data[7], -pCrush_data[8]);
    if (pCrush->field_0x192 != 0) {
        BrMatrix34PostRotateY(&matrix, pCrush->field_0x192);
    }
    if (pCrush->field_0x190 != 0) {
        BrMatrix34PostRotateX(&matrix, pCrush->field_0x190);
    }
    if (pCrush->field_0x194 != 0) {
        BrMatrix34PostRotateZ(&matrix, pCrush->field_0x194);
    }
    BrMatrix34PostTranslate(&matrix, pCrush_data[6], pCrush_data[7], pCrush_data[8]);

    for (i = 0; i < pCrush->count_shapes; i++) {
        FUN_0043aa20(&pCrush->field_0x4[i], &matrix);
    }
    pCrush->expand_bounding_box = 3;
    FUN_0043aa80(pCar, &matrix);
    BrVector3Copy(&pCar->collision_info->cmpos, &pCrush->field_0x264);
    FUN_00434f60(&pCar->collision_info->cmpos, &matrix);

    if (pCrush->field_0x190 == 0 && pCrush->field_0x192 == 0 && pCrush->field_0x194 == 0) {
        pCrush->field_0x144 = 0;
        for (i = 0; i < pCrush->count_shapes; i++) {
            CompletelyUnBendCollisionShape(&pCrush->field_0x4[i]);
        }
        pCrush->expand_bounding_box = 3;
        CompletelyUnBendWheels(pCar);
        BrVector3Copy(&pCar->collision_info->cmpos, &pCrush->field_0x264);
        DRActorEnumRecurse(pCar->car_model_actor, EnableGroovers, NULL);
    }

    normalized_x = (tU16)delta_x;
    if (normalized_x >= 0x7ff8) {
        normalized_x = (tU16)-normalized_x;
    }
    return delta_z_value + delta_y_value + (float)normalized_x * (float)gDouble_00589b40;
}

// FUNCTION: CARMA2_HW 0x0043aa20
void C2_HOOK_FASTCALL FUN_0043aa20(void* pThis, void* pArg2) {
    int i;
    int offset;

    i = 0;
    if (0 < *(int*)((tU8*)pThis + 0xc)) {
        offset = 0;
        do {
            *(int*)(*(int*)((tU8*)pThis + 0x18) + offset + 0x18) = *(int*)(*(int*)((tU8*)pThis + 0x18) + offset + 0x2c);
            *(int*)(*(int*)((tU8*)pThis + 0x18) + offset + 0x1c) = *(int*)(*(int*)((tU8*)pThis + 0x18) + 0x30 + offset);
            *(int*)(*(int*)((tU8*)pThis + 0x18) + offset + 0x20) = *(int*)(*(int*)((tU8*)pThis + 0x18) + 0x34 + offset);
            FUN_00434f60((void*)(*(int*)((tU8*)pThis + 0x18) + offset + 0x18), pArg2);
            i++;
            offset += 0x38;
        } while (i < *(int*)((tU8*)pThis + 0xc));
    }
}

// FUNCTION: CARMA2_HW 0x00439c10
void C2_HOOK_FASTCALL CrushCarByNetwork(tCar_spec* pCar) {
    float temp;

    DoCrushNetwork(pCar, 0x7d0, &temp);
}

// FUNCTION: CARMA2_HW 0x0043dce0
void C2_HOOK_FASTCALL WeldCarByNetwork(tCar_spec* pCar) {
    tPhysics_object* info;
    tCar_crush_spec* spec;
    float temp;
    tU8 f;
    int flag;
    tU32 id_a;
    tU32 id_b;
    tU32 id_c;
    br_vector3 v;

    spec = pCar->car_crush_spec;
    if (spec->field_0x144 != 0) {
        tPhysics_object* binfo;

        binfo = pCar->collision_info;
        f = binfo->field_0x481;
        {
            int c1;
            int c2;
            if (f & 1) {
                c1 = (spec->field_0x174 == spec->bend_z_min);
            } else {
                c1 = (spec->field_0x174 == spec->bend_z_max);
            }
            c2 = ((f & 2) != 0 && spec->field_0x190 == binfo->field_0x482)
                    || ((f & 4) != 0 && spec->field_0x192 == binfo->field_0x482)
                    || ((f & 8) != 0 && spec->field_0x194 == binfo->field_0x482);
            if (c1 & c2) {
                return;
            }
        }
        DoCrushNetwork(pCar, 0x7d0, &temp);
    }

    info = pCar->collision_info;
    id_a = 0;
    id_b = 0;
    id_c = 0;
    v.v[0] = 0.f;
    v.v[1] = 0.f;
    v.v[2] = 0.f;
    if ((info->field_0x481 & 1) != 0) {
        v.v[2] = spec->bend_z_min;
        temp = info->bb1.max.v[2];
        flag = 1;
    } else {
        v.v[2] = spec->bend_z_max;
        temp = info->bb1.min.v[2];
        flag = 0;
    }
    switch (info->field_0x481 >> 1) {
    case 1:
        id_a = info->field_0x482;
        if ((id_a < 0x7ff8 && flag) || (id_a > 0x7ff8 && !flag)) {
            v.v[1] = info->bb1.min.v[1];
        } else {
            v.v[1] = info->bb1.max.v[1];
        }
        break;
    case 2:
        id_b = info->field_0x482;
        if ((id_b > 0x7ff8 && flag) || (id_b < 0x7ff8 && !flag)) {
            v.v[0] = info->bb1.min.v[0];
        } else {
            v.v[0] = info->bb1.max.v[0];
        }
        break;
    case 4:
        id_c = info->field_0x482;
        v.v[0] = (info->bb1.max.v[0] + info->bb1.min.v[0]) * 0.5f;
        v.v[1] = (info->bb1.min.v[1] + info->bb1.max.v[1]) * 0.5f;
        break;
    }
    CrushPanelWithForce(pCar, id_a, id_b, id_c, &v, *(tU32*)&temp, 0);
}

// FUNCTION: CARMA2_HW 0x0043df10
void C2_HOOK_FASTCALL WeldCarPartial(tCar_spec* pCar) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x0043db00
void C2_HOOK_FASTCALL UpdateCrushVertices(tCar_spec* pCar, tU8* pField) {
    int i;
    int j;

    if (pCar->car_crush_spec->field_0x4b8 != 0) {
        tPhysics_shape* shape;
        int count;

        shape = pCar->collision_info->shape;
        count = *pField++;
        while (shape != NULL && count > 0) {
            if (shape->common.type == kCollisionShapeType_Polyhedron
                    || shape->common.type == kCollisionShapeType_Wireframe_Polyhedron) {

                shape->polyhedron.polyhedron.count_points = *pField++;
                for (i = 0; i < shape->polyhedron.polyhedron.count_points; i++) {
                    ExpandVector3(&shape->polyhedron.polyhedron.points[i],
                            (const tCompressed_vector3*)pField, -2.5f, 2.5f);
                    pField += 6;
                }
                count--;
            }
            shape = shape->common.next;
        }
    } else {
        pField++;
        for (i = 0; i < pCar->car_crush_spec->count_shapes; i++) {
            tCar_crush_shape_info* elem = &pCar->car_crush_spec->field_0x4[i];

            for (j = 0; j < elem->count_points; j++) {

                ExpandVector3(&elem->field_0x18[j].field_0x18, (const tCompressed_vector3*)pField, -2.5f, 2.5f);
                pField += 6;
                BrVector3Copy(&elem->field_0x18[j].field_0xc, &elem->field_0x18[j].field_0x18);
                BrVector3Copy(&elem->field_0x0->polyhedron.points[j], &elem->field_0x18[j].field_0xc);
            }
            FillInShape((tPhysics_shape*)elem->field_0x0);
        }
    }

    UpdateCollisionObject(pCar->collision_info);
    pCar->collision_info->bb2.min.v[1] = GetCarOverallBoundsMinY(pCar);
    pCar->car_crush_spec->field_0x26 = pCar->collision_info->field_0x290;
    pCar->car_crush_spec->field_0x24 = pCar->car_crush_spec->field_0x26;
    pCar->car_crush_spec->expand_bounding_box = 0;
}

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x004f3560
void C2_HOOK_FASTCALL MungeSmashMaterialName(void* pResource, char** ppName, char pIndex) {
    char name[64];
    char suffix[64];
    int name_length;

    strcpy(name, *ppName);
    name_length = strlen(name);
    if (name_length < 4) {
        sprintf(name, "Smash material %s has a name that is less than 4 characters long", *ppName);
        PDFatalError(name);
    }
    strcpy(suffix, name + name_length - 4);
    sprintf(name + name_length - 4, "        ");
    sprintf(name + 5, "%cx", '|');
    name[6] = pIndex + 1;
    strcat(name, suffix);
    name[11] = 0;

    if (strlen(*ppName) < 12) {
        BrResFree(*ppName);
        *ppName = BrResStrDup(pResource, name);
        return;
    }
    strcpy(*ppName, name);
}
#pragma auto_inline(on)

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x004f36c0
void C2_HOOK_FASTCALL FUN_004f36c0(const int* pIndices, const float* pPoint, float* pResult) {
    const float* pData;
    float dy1;
    float dx1;
    float dy0;
    float dx0;
    float distance1;
    float distance0;

    pData = (const float*)pIndices;
    dy1 = pData[27] - pPoint[1];
    dx1 = pData[26] - pPoint[0];
    dy0 = pData[15] - pPoint[1];
    dx0 = pData[14] - pPoint[0];
    distance1 = dy1 * dy1 + dx1 * dx1;
    distance0 = dy0 * dy0 + dx0 * dx0;
    if (distance1 < distance0) {
        FUN_004f3740(pIndices, (const float*)((const tU8*)pIndices + 0x54), pPoint, pResult);
    } else {
        FUN_004f3740(pIndices, (const float*)((const tU8*)pIndices + 0x24), pPoint, pResult);
    }
}
#pragma auto_inline(on)

// FUNCTION: CARMA2_HW 0x004f4e20
float C2_HOOK_FASTCALL ApplySmashTriggerEffect(tSmashable_item_spec* pSpec, tPhysics_object* pObject, br_actor* pActor, float pArg3, br_vector3* pArg4, br_vector3* pArg5, br_vector3* pArg6, int* pFlag, int pArg8) {

    NOT_IMPLEMENTED();
    return 0.f;
}

// FUNCTION: CARMA2_HW 0x004f5110
void C2_HOOK_FASTCALL ApplySmashTriggerNumber(tPhysics_object* pObject, tFace_ref* pFace, br_scalar pSpeed, br_vector3* pPoint, br_vector3* pVelocity, br_vector3* pNormal, int pArg7) {
    tCar_spec* car;
    void* owner;
    int driver;
    tNet_message* message;
    tStored_smash* chunk;
    tStored_smash* stored;
    br_vector3 point;
    float d;
    int id;

    if (gNet_mode == eNet_mode_none) {
        return;
    }
    if (pObject != NULL && pObject->owner != NULL && pObject->flags_0x238 == 1) {
        owner = pObject->owner;
        driver = *(int*)((char*)owner + 0xc);
        if (driver > 5) {
            if (owner == NULL || driver != eDriver_local_human) {
                return;
            }
        }
    }
    if (pFace != NULL) {
        d = DistanceFromFace(pPoint, pFace);
        BrVector3Scale(&point, &pFace->normal, -d);
        BrVector3Add(&point, &point, pPoint);
    } else {
        BrVector3Copy(&point, pPoint);
    }
    message = NetBuildGuaranteedMessage(0x21, 0);
    chunk = (tStored_smash*)&message->guaranteed;
    chunk->field_0x4 = gPHIL_last_physics_tick + 40;
    if (pObject != NULL && pObject->owner != NULL && pObject->flags_0x238 == 1) {
        car = (tCar_spec*)pObject->owner;
        if (car != NULL && car->driver > 5) {
            if (gNet_mode != eNet_mode_none) {
                id = NetPlayerFromCar(car) - gNet_players;
            } else {
                id = *(tU16*)((tU8*)car + 0x80);
            }
        } else {
            id = -1;
        }
    } else {
        id = -1;
    }
    chunk->player = id;
    chunk->field_0xc = pSpeed;
    BrVector3Copy(&chunk->field_0x10, &point);
    chunk->field_0x1c = *pVelocity;
    chunk->field_0x28 = *pNormal;
    if (pArg7 != 0) {
        chunk->field_0x34 = 2;
    } else {
        chunk->field_0x34 = 0;
    }
    NetGuaranteedSendMessageToAllPlayers(gCurrent_net_game, message, 0);
    if (gNet_mode == eNet_mode_host && gCount_host_smashes < 1000) {
        stored = &gNet_host_smashes[gCount_host_smashes];
        gCount_host_smashes++;
        *stored = *chunk;
    }
}

#pragma auto_inline(on)

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x004f5430
int C2_HOOK_FASTCALL FUN_004f5430(br_material* pMaterial) {
    int i;
    int count;

    count = gCount_track_smashable_environment_specs;
    for (i = 0; i < count; i++) {
        tSmashable_item_spec* spec = &gTrack_smashable_environment_specs[i];

        if (spec->trigger_type == kSmashableTrigger_Material && spec->trigger_object.material == pMaterial) {
            return 1;
        }
    }
    return 0;
}
#pragma auto_inline(on)

// C2PORT-NONMATCHING: best 52.75% | reason regalloc-loop
// FUNCTION: CARMA2_HW 0x004f5a70
intptr_t C2_HOOK_CDECL FUN_004f5a70(br_actor* pActor, void* pContext) {
    tUser_crush_data* user_crush_data;
    tCar_crush_buffer_entry* crush_data;
    int i;
    int offset;

    if (pActor->user != NULL && (user_crush_data = pActor->user, (crush_data = user_crush_data->crush_data) != NULL) &&
        (i = 0, crush_data->count_smashables != 0)) {
        offset = 0;
        do {
            gINT_006a3334 = 0;
            FUN_004ee290((tCar_spec*)pContext, *(tPhysics_object**)((char*)pContext + 8), (br_vector3*)((char*)pContext + 0x4c), 0, pActor, 0,
                         (tCar_crush_smashable_part*)((char*)crush_data->smashables + offset), 1000.0f, 0, 0);
            i++;
            offset += 0x7c;
        } while (i < crush_data->count_smashables);
    }
    return 0;
}

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x004f5a60
void C2_HOOK_FASTCALL FUN_004f5a60(tCar_spec* pCar) {
    DRActorEnumRecurse(pCar->car_master_actor, FUN_004f5a70, pCar);
}
#pragma auto_inline(on)

// FUNCTION: CARMA2_HW 0x004f52f0
float C2_HOOK_FASTCALL BashObject(tPhysics_object* pObject, br_actor* pActor, float pArg3, br_vector3 *pArg4, br_vector3* pArg5, br_vector3* pArg6, int pArg7, int pArg8) {
    float result = -1.f;
    int flag = 0;
    char* identifier;
    tSmashable_item_spec* trigger;

    if (pActor != NULL) {
        identifier = pActor->identifier;
        if (identifier != NULL && identifier[0] == '&') {
            trigger = gSmashable_track_indexable_triggers[10 * (identifier[1] - '0') + (identifier[2] - '0')];
            if (trigger != NULL) {
                if ((pArg7 == 0 && (trigger->trigger_object.number.field_0x1 & 1) && identifier[3] != '!')
                        || (pArg7 == 0 && (trigger->trigger_object.number.field_0x1 & 4) && identifier[3] == '!')
                        || (pArg7 != 0 && (trigger->trigger_object.number.field_0x1 & 2))) {
                    gINT_006a3334 = 0;
                    result = ApplySmashTriggerEffect(trigger, pObject, pActor, pArg3, pArg4, pArg5, pArg6, &flag, pArg8);
                    if (flag != 0 && pArg8 == 0) {
                        ApplySmashTriggerNumber(pObject, 0, pArg3, pArg4, pArg5, pArg6, 1);
                    }
                }
            }
        }
    }
    return result;
}

// FUNCTION: CARMA2_HW 0x004f53e0
void C2_HOOK_FASTCALL PushObject(tPhysics_object* pObject, br_vector3* pPos, br_vector3* pV, br_vector3* pUp) {

    BashObject(pObject, pObject->actor, BrVector3Length(pUp), pPos, pV, pUp, 1, 0);
}

// FUNCTION: CARMA2_HW 0x004f1140
float C2_HOOK_FASTCALL SmashEnvironment(tPhysics_object* pObject, undefined4* pArg2, float pArg3, br_vector3* pArg4, br_vector3* pArg5, br_vector3* pArg6, int pArg7, int pArg8) {

    NOT_IMPLEMENTED();
    return 0.f;
}

void C2_HOOK_FASTCALL SphericizeModel(br_model* pModel, const br_vector3* pCenter, br_scalar pRadius) {
    int i;
    br_vector3 tv;

    for (i = 0; i < pModel->nvertices; i++) {
        BrVector3Sub(&tv, &pModel->vertices[i].p, pCenter);
        BrVector3Normalise(&tv, &tv);
        BrVector3Scale(&pModel->vertices[i].p, &tv, pRadius);
    }
    BrModelUpdate(pModel, BR_MODU_VERTEX_POSITIONS);
}

void C2_HOOK_FASTCALL SphericizePhysics(tCar_spec* pCar, const br_vector3* pCenter, br_scalar pRadius) {
    br_vector3 delta;

    gGonad_sphere_collision_shape->sphere.sphere.radius = pRadius;
    BrVector3Copy(&gGonad_sphere_collision_shape->sphere.sphere.center, pCenter);
    gGonad_sphere_collision_shape->common.next = NULL;
    FillInShape(gGonad_sphere_collision_shape);
    pCar->collision_info->shape = gGonad_sphere_collision_shape;
    UpdateCollisionObject(pCar->collision_info);
    BrVector3Copy(&pCar->collision_info->cmpos, pCenter);
    pCar->collision_info->M = 50.f;
    BrVector3Set(&pCar->collision_info->I,
        CARPOCALYPSE2_SQR(pCar->collision_info->shape->sphere.sphere.radius) * pCar->collision_info->M / WORLD_SCALE,
        CARPOCALYPSE2_SQR(pCar->collision_info->shape->sphere.sphere.radius) * pCar->collision_info->M / WORLD_SCALE,
        CARPOCALYPSE2_SQR(pCar->collision_info->shape->sphere.sphere.radius) * pCar->collision_info->M / WORLD_SCALE);
    pCar->collision_info->world_friction = 16.f;
    gGonad_sphere_collision_shape->common.type = kCollisionShapeType_Box;
    while (!TestForCarInSensiblePlace(pCar, &delta)) {
        BrVector3InvScale(&delta, &delta, 2.f * WORLD_SCALE);
        BrVector3Accumulate((br_vector3*)&pCar->collision_info->transform_matrix.m[3], &delta);
    }
    gGonad_sphere_collision_shape->common.type = kCollisionShapeType_Sphere;
    pCar->field_0x18c8 = 2;
}
// FUNCTION: CARMA2_HW 0x004391d0
void C2_HOOK_FASTCALL SphericizeCar(tCar_spec* pCar) {
    br_vector3 center;
    br_scalar radius;
    br_model* shell_model;

    shell_model = pCar->shell_model;
    if (shell_model != NULL) {
        Vector3Average(&center, &shell_model->bounds.min, &shell_model->bounds.max);
        radius = Vector3Distance(&center, &shell_model->bounds.min);
        SphericizeModel(shell_model, &center, radius);
        pCar->car_model_actor->children = NULL;
        SphericizePhysics(pCar, &center, radius);
        pCar->invulnerable_no_crushage = 1;
        pCar->invulnerable_no_damage = 1;
        pCar->use_shell_model = 1;
    }
}

// C2PORT-NONMATCHING: best 58.74% | difftest PASS 2000 trials (spec difftest/specs/0043be50_FUN_0043be50.json) | reason x87-scheduling
// FUNCTION: CARMA2_HW 0x0043be50
void C2_HOOK_FASTCALL FUN_0043be50(br_model* pModel, const tCompressed_vector3* pVectors, const tU16* pDeltas) {
    br_model* model;
    br_vector3 direction;
    br_vector3 scaled_direction;
    const tU16* delta;
    int i;
    int vertex_offset;
    float scale;
    br_vertex* vertex;

    model = pModel;
    ExpandVector3(&direction, pVectors, -1.0f, 1.0f);
    i = 0;
    if (model->nvertices > 0) {
        delta = pDeltas;
        vertex_offset = 0;
        do {
            if (*delta != 0) {
                scale = -DRU16ToScalar(*delta, 0.0f, 10.0f);
                vertex = (br_vertex*)((tU8*)model->vertices + vertex_offset);
                BrVector3Scale(&scaled_direction, &direction, scale);
                BrVector3Accumulate(&vertex->p, &scaled_direction);
            }
            i++;
            delta++;
            vertex_offset += 0x28;
        } while (i < model->nvertices);
    }
    if ((model->flags & 0x20) != 0 && model->user != NULL) {
        *(tU32*)model->user = 1;
        return;
    }
    model->custom = JitModelUpdate;
    model->flags |= 0x20;
}

// FUNCTION: CARMA2_HW 0x00440300
void C2_HOOK_FASTCALL MakeCarAvailable(tCar_spec* pCar) {
    pCar->has_been_stolen = 1;
    gProgram_state.cars_available[gProgram_state.number_of_cars] = pCar->index;
    gProgram_state.number_of_cars += 1;
    gOpponents[pCar->index].dead = 1;
}

// FUNCTION: CARMA2_HW 0x0043c940
void C2_HOOK_FASTCALL FUN_0043c940(void* pParam) {
    tCar_spec* car;
    tFind_crush_actor_arg find_arg;
    br_actor* actor;
    int i;
    int count;
    tCrush_net_semi_detach_bit_list_item* entry;

    if (gCount_net_crush_semi_detach_bit_list != 8) {
        car = NetCarFromPlayerID(*(tPlayer_ID*)((tU8*)pParam + 4));
        if (car != NULL) {
            find_arg.id = ((tU8*)pParam)[8];
            find_arg.actor = NULL;
            actor = DRActorEnumRecurse(car->car_model_actor, FindActorByCrushEntryID, &find_arg)
                ? find_arg.actor
                : NULL;
            if (actor != NULL) {
                count = gCount_net_crush_semi_detach_bit_list;
                for (i = 0; i < count; i++) {
                    if (gNet_crush_semi_detach_bit_list[i].actor == actor) {
                        return;
                    }
                }
                entry = &gNet_crush_semi_detach_bit_list[count];
                entry->actor = actor;
                entry->car = car;
                entry->field_0x8 = *(tU32*)((tU8*)pParam + 0xc);
                ExpandVector3(&gNet_crush_semi_detach_bit_list[gCount_net_crush_semi_detach_bit_list].field_0xc,
                    (tCompressed_vector3*)((tU8*)pParam + 0x10), -10.f, 10.f);
                ExpandVector3(&gNet_crush_semi_detach_bit_list[gCount_net_crush_semi_detach_bit_list].field_0x30,
                    (tCompressed_vector3*)((tU8*)pParam + 0x22), -10.f, 10.f);
                ExpandVector3(&gNet_crush_semi_detach_bit_list[gCount_net_crush_semi_detach_bit_list].field_0x3c,
                    (tCompressed_vector3*)((tU8*)pParam + 0x28), -10.f, 10.f);
                ExpandVector3(&gNet_crush_semi_detach_bit_list[gCount_net_crush_semi_detach_bit_list].field_0x18,
                    (tCompressed_vector3*)((tU8*)pParam + 0x16), -10.f, 10.f);
                ExpandVector3(&gNet_crush_semi_detach_bit_list[gCount_net_crush_semi_detach_bit_list].field_0x24,
                    (tCompressed_vector3*)((tU8*)pParam + 0x1c), -10.f, 10.f);
                gNet_crush_semi_detach_bit_list[gCount_net_crush_semi_detach_bit_list].field_0x48 =
                    DRU16ToScalar(*(tU16*)((tU8*)pParam + 0x2e), 0.f, 40.f);
                gCount_net_crush_semi_detach_bit_list++;
            }
        }
    }
}
