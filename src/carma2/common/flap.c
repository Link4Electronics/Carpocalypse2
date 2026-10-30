#include "flap.h"

#include "car.h"
#include "compress.h"
#include "crush.h"
#include "crush1.h"

#include "depth.h"

#include "globvars.h"

#include "globvrkm.h"

#include "globvrpb.h"
#include "network.h"
#include "physics.h"
#include "platform.h"
#include "piping.h"

#include "powerups.h"

#include "raycast.h"

#include "sound.h"
#include "utility.h"

#include "world.h"

#include "c2_stdlib.h"
#include "c2_math.h"
#include "carpocalypse2_macros.h"

extern br_scalar gCrushZero;
extern br_scalar gTorque_to_snap_per_tonne;
extern br_scalar gCrushSqrtHalf;
extern tU16 gNext_crush_actor_id;
extern const br_scalar gFLOAT_00589a54;
extern const double gDOUBLE_00589ad0;
extern br_scalar gFlap_inertia_fudge_biscuit;
extern br_scalar gFLOAT_0058a768;
extern br_scalar gMin_force_to_split_XZ_per_tonne;
extern br_scalar gMin_force_to_split_Y_per_tonne;
extern int gINT_00692dc8;

// GLOBAL: CARMA2_HW 0x0067a3b0
tU16 gSplit_polyhedron_edge_map[2560];

// GLOBAL: CARMA2_HW 0x00679444
float* gSplit_polyhedron_source_vertices;

// GLOBAL: CARMA2_HW 0x006796c0
float* gSplit_polyhedron_positive_vertices;

// GLOBAL: CARMA2_HW 0x006796c4
float* gSplit_polyhedron_negative_vertices;

// GLOBAL: CARMA2_HW 0x0067be70
br_face* gSplit_polyhedron_source_faces;

// GLOBAL: CARMA2_HW 0x00589ae0
const br_scalar gFloat_00589ae0 = 0.0833333358f;

// GLOBAL: CARMA2_HW 0x0067be8c
int gINT_0067be8c;

// GLOBAL: CARMA2_HW 0x00589a98
const br_scalar gFLOAT_00589a98 = 0.0001f;

// GLOBAL: CARMA2_HW 0x00589a9c
const br_scalar gFLOAT_00589a9c = 0.9999f;

// GLOBAL: CARMA2_HW 0x00589a2c
const br_scalar gFLOAT_00589a2c = 0.5f;

// GLOBAL: CARMA2_HW 0x00589a28
const br_scalar gFLOAT_00589a28 = 0.25f;

// GLOBAL: CARMA2_HW 0x00589a7c
const br_scalar gFLOAT_00589a7c = 0.001f;

// GLOBAL: CARMA2_HW 0x00589aa8
const br_scalar gFLOAT_00589aa8 = -0.05797101557254791f;

// GLOBAL: CARMA2_HW 0x00589aac
const br_scalar gFLOAT_00589aac = 0.05797101557254791f;

// GLOBAL: CARMA2_HW 0x00589a60
const double gDOUBLE_00589a60 = 1.0;

// GLOBAL: CARMA2_HW 0x00589a54
const br_scalar gFLOAT_00589a54 = 1.0f;

// GLOBAL: CARMA2_HW 0x00589a58
const br_scalar gFLOAT_00589a58 = 0.01f;

// GLOBAL: CARMA2_HW 0x00589ab4
const br_scalar gFLOAT_00589ab4 = 1.1920928955078125e-7f;

// GLOBAL: CARMA2_HW 0x00589ab8
const double gDOUBLE_00589ab8 = 0.1744903177022934;

// GLOBAL: CARMA2_HW 0x00589a5c
const br_scalar gFLOAT_00589a5c = 2.384185791015625e-7f;

// GLOBAL: CARMA2_HW 0x00589aa0
const br_scalar gFLOAT_00589aa0 = 0.007246376946568489f;

// GLOBAL: CARMA2_HW 0x00589aa4
const br_scalar gFLOAT_00589aa4 = -0.007246376946568489f;

// GLOBAL: CARMA2_HW 0x00589b2c
const br_scalar gFLOAT_00589b2c = -1.0f;

// GLOBAL: CARMA2_HW 0x00589b30
const br_scalar gFLOAT_00589b30 = 0.0f;

// FUNCTION: CARMA2_HW 0x00433350
tPhysics_object* C2_HOOK_FASTCALL DetachBit(tCar_spec* pCar, br_actor* pActor, br_bounds3* pBounds) {

    NOT_IMPLEMENTED();
    return NULL;
}

extern void (C2_HOOK_CDECL BrVector3Sub)(br_vector3*, const br_vector3*, const br_vector3*);

// C2PORT-NONMATCHING: best 38.04% | difftest PASS 2000 trials (spec difftest/specs/004336f0_InitDetachedBitPhysicsObject.json) | reason x87-regalloc
// FUNCTION: CARMA2_HW 0x004336f0
void C2_HOOK_FASTCALL InitDetachedBitPhysicsObject(tPhysics_object* pObject, br_actor* pActor, br_model* pModel, tCar_crush_detach_data* pDetach_data, tCar_crush_spec* pCar_crush, const br_bounds3* pBounds, tPhysics_object* pParent_collision_info) {
    tCar_crush_shape_info* shape_info;
    tUser_crush_data* actor_user;
    tCar_crush_buffer_entry* crush_data;
    tPhysics_shape* shape;
    tPhysics_object* root_object;
    float* local_20[2];
    br_vector3 temporary;
    br_vector3 velocity_offset;
    float local_c[3];
    float fVar1;
    float fVar2;
    float fVar3;
    float fVar4;
    float best_distance;
    int iVar5;
    unsigned int uVar6;

    pObject->actor = pActor;
    if (pBounds != NULL) {
        pObject->shape->common.type = kCollisionShapeType_Box;
        DRBoundsCopy(&pObject->shape->common.bb, pBounds);
    } else if (pDetach_data->type == 4) {
        shape_info = &pCar_crush->field_0x4[pDetach_data->field_0x78];
        shape = pObject->shape;
        pObject->shape = (tPhysics_shape*)shape_info->field_0x0;
        shape_info->field_0x0 = (tCollision_shape_polyhedron*)shape;
        shape_info->field_0x8 = 1;
        pCar_crush->expand_bounding_box |= 1;
    } else if (pDetach_data->count_shape_points < 1) {
        pObject->shape->common.type = kCollisionShapeType_Box;
        DRBoundsCopy(&pObject->shape->common.bb, &pModel->bounds);
    } else {
        iVar5 = FUN_00422500(&pObject->shape->polyhedron.polyhedron, pModel, 8,
            pDetach_data->count_shape_points, pDetach_data->shape_points);
        if (iVar5 == 0) {
            pObject->shape->common.type = kCollisionShapeType_Polyhedron;
            GetBoundsFromPointList(pObject->shape->polyhedron.polyhedron.points,
                pObject->shape->polyhedron.polyhedron.count_points, &pObject->shape->common.bb);
        } else if (iVar5 == 2) {
            pObject->shape->common.type = kCollisionShapeType_Wireframe_Polyhedron;
            GetBoundsFromPointList(pObject->shape->polyhedron.polyhedron.points,
                pObject->shape->polyhedron.polyhedron.count_points, &pObject->shape->common.bb);
        } else {
            pObject->shape->common.type = kCollisionShapeType_Box;
            DRBoundsCopy(&pObject->shape->common.bb, &pModel->bounds);
        }
    }
    DRBoundsCopy(&pObject->bb1, &pObject->shape->common.bb);
    DRBoundsCopy(&pObject->bb2, &pObject->shape->common.bb);

    actor_user = pActor->user;
    crush_data = actor_user->crush_data;
    pObject->M = crush_data->field_0x24;
    local_20[0] = pParent_collision_info->bb1.min.v;
    local_20[1] = pParent_collision_info->bb1.max.v;
    temporary.v[0] = pObject->bb1.min.v[0] * gFLOAT_00589a2c;
    temporary.v[1] = pObject->bb1.min.v[1] * gFLOAT_00589a2c;
    temporary.v[2] = pObject->bb1.min.v[2] * gFLOAT_00589a2c;
    local_c[0] = pObject->bb1.max.v[0] * gFLOAT_00589a2c;
    local_c[1] = pObject->bb1.max.v[1] * gFLOAT_00589a2c;
    local_c[2] = pObject->bb1.max.v[2] * gFLOAT_00589a2c;
    pObject->cmpos.v[0] = local_c[0] + temporary.v[0];
    pObject->cmpos.v[1] = local_c[1] + temporary.v[1];
    pObject->cmpos.v[2] = local_c[2] + temporary.v[2];

    BrMatrix34ApplyP(&temporary, &pObject->cmpos, &pActor->t.t.mat);
    DrMatrix34ApplyLPInverse((br_vector3*)local_c, &temporary,
        &pParent_collision_info->actor->t.t.mat);
    uVar6 = 0;
    iVar5 = 0;
    best_distance = 3.402823466e+38f;
    do {
        if (uVar6 != 2) {
            fVar1 = (float)fabs(local_c[uVar6 >> 1] - local_20[uVar6 & 1][uVar6 >> 1]);
            if (fVar1 < best_distance) {
                best_distance = fVar1;
                iVar5 = uVar6;
            }
        }
        uVar6++;
    } while (uVar6 < 6);
    uVar6 = iVar5 >> 1;
    if ((iVar5 & 1) == 0) {
        fVar1 = pObject->bb1.min.v[uVar6];
    } else {
        fVar1 = pObject->bb1.max.v[uVar6];
    }
    pObject->cmpos.v[uVar6] = (fVar1 + pObject->cmpos.v[uVar6]) * gFLOAT_00589a2c;

    fVar1 = pObject->bb1.max.v[0] - pObject->bb1.min.v[0];
    fVar2 = pObject->bb1.max.v[1] - pObject->bb1.min.v[1];
    fVar2 = fVar2 * fVar2;
    fVar1 = fVar1 * fVar1;
    fVar3 = pObject->bb1.max.v[2] - pObject->bb1.min.v[2];
    fVar3 = fVar3 * fVar3;
    fVar4 = (fVar3 + fVar1) * pObject->M * gFloat_00589ae0;
    fVar3 = pObject->M * (fVar3 + fVar2) * gFloat_00589ae0;
    pObject->I.v[0] = fVar3;
    pObject->I.v[1] = fVar4;
    pObject->I.v[2] = pObject->M * (fVar2 + fVar1) * gFloat_00589ae0;

    BrMatrix34ApplyV(&temporary, &pParent_collision_info->omega,
        &pParent_collision_info->actor->t.t.mat);
    BrMatrix34TApplyV(&pObject->omega, &temporary, &pActor->t.t.mat);
    BrMatrix34ApplyP(&temporary, &pParent_collision_info->cmpos,
        &pParent_collision_info->actor->t.t.mat);
    DrMatrix34ApplyLPInverse((br_vector3*)local_c, &temporary, &pActor->t.t.mat);
    /* Use the out-of-line subtraction to retain retail's scalar stores before
     * the cross product; VC5 otherwise reuses an unrounded x87 component. */
    (BrVector3Sub)(&velocity_offset, &pObject->cmpos, (br_vector3*)local_c);
    pObject->v.v[0] = (br_scalar)((double)pObject->omega.v[1] * velocity_offset.v[2] - (double)pObject->omega.v[2] * velocity_offset.v[1]);
    pObject->v.v[1] = (br_scalar)((double)pObject->omega.v[2] * velocity_offset.v[0] - (double)pObject->omega.v[0] * velocity_offset.v[2]);
    pObject->v.v[2] = (br_scalar)((double)pObject->omega.v[0] * velocity_offset.v[1] - (double)pObject->omega.v[1] * velocity_offset.v[0]);
    BrMatrix34ApplyV(&temporary, &pObject->v, &pActor->t.t.mat);
    pObject->v.v[0] = pParent_collision_info->v.v[0] + temporary.v[0];
    pObject->v.v[1] = pParent_collision_info->v.v[1] + temporary.v[1];
    pObject->v.v[2] = pParent_collision_info->v.v[2] + temporary.v[2];
    BrVector3SetFloat(&pObject->field_0x54, 0.f, gFLOAT_00589aa8, 0.f);
    pObject->box_face_ref = gFace_num__car - 2;

    root_object = pObject;
    while (root_object->parent != NULL) {
        root_object = root_object->parent;
    }
    SetCollisionInfoChildsDoNothing(root_object, 0);
    pObject->field_0x261 = 0;
    pObject->flags |= 0x100;
    pObject->message_time = 0;
    pObject->field_0x49c = 1;
    pObject->uid = crush_data->id;
    pObject->water_d = pParent_collision_info->water_d;
    pObject->last_special_volume = pParent_collision_info->last_special_volume;
    pObject->auto_special_volume = pParent_collision_info->auto_special_volume;
}

// FUNCTION: CARMA2_HW 0x00436d60
tPhysics_object* C2_HOOK_FASTCALL SemiDetachBit(tCar_spec* pCar, br_actor* pActor, float pArg3, int* pArg4, br_vector3* pArg5, br_bounds3* pArg6, br_vector3* pArg7, br_vector3* pArg8) {

    NOT_IMPLEMENTED();
    return NULL;
}

// FUNCTION: CARMA2_HW 0x0042dbd0
int C2_HOOK_FASTCALL GetSDBJointPosAndBounds(br_vector3* pP1, br_vector3* pP2, br_vector3* pP33, br_bounds3* pBounds, br_actor* pActor) {
    tUser_crush_data* user;
    br_model* model;
    tCar_crush_buffer_entry* crush_data;
    tCar_crush_detach_data* detach_data;
    tCar_crush_flap_data* flap_data;

    user = pActor->user;
    if (user == NULL) {
        return 1;
    }
    model = user->models[0];
    if (model == NULL) {
        return 1;
    }
    crush_data = user->crush_data;
    if (crush_data == NULL) {
        return 1;
    }
    detach_data = crush_data->detach_data;
    if (detach_data == NULL) {
        return 1;
    }
    DRBoundsCopy(pBounds, &model->bounds);
    flap_data = crush_data->flap_data;
    if (flap_data != NULL) {
        pP1->v[0] = model->vertices[flap_data->hinge0].p.v[0];
        pP1->v[1] = model->vertices[flap_data->hinge0].p.v[1];
        pP1->v[2] = model->vertices[flap_data->hinge0].p.v[2];
        pP2->v[0] = model->vertices[flap_data->hinge1].p.v[0];
        pP2->v[1] = model->vertices[flap_data->hinge1].p.v[1];
        pP2->v[2] = model->vertices[flap_data->hinge1].p.v[2];
        pP33->v[0] = model->vertices[flap_data->hinge2].p.v[0];
        pP33->v[1] = model->vertices[flap_data->hinge2].p.v[1];
        pP33->v[2] = model->vertices[flap_data->hinge2].p.v[2];
    } else {
        pP1->v[0] = model->vertices[detach_data->field_0x30].p.v[0];
        pP1->v[1] = model->vertices[detach_data->field_0x30].p.v[1];
        pP1->v[2] = model->vertices[detach_data->field_0x30].p.v[2];
        pP2->v[0] = 0.f;
        pP2->v[1] = 0.f;
        pP2->v[2] = 0.f;
        pP33->v[0] = 0.f;
        pP33->v[1] = 0.f;
        pP33->v[2] = 0.f;
    }
    return 0;
}

typedef struct tNet_message_chunk_semi_detach_bit {
    int ID;
    tU8 field_0x4;
    undefined field_0x5[3];
    tU32 field_0x8;
    tCompressed_vector3 field_0xc;
    tCompressed_vector3 field_0x12;
    tCompressed_vector3 field_0x18;
    tCompressed_vector3 field_0x1e;
    tCompressed_vector3 field_0x24;
    tU16 field_0x2a;
} tNet_message_chunk_semi_detach_bit;

// FUNCTION: CARMA2_HW 0x0042dab0
void C2_HOOK_FASTCALL SendSemiDetachBit(tCar_spec* pCar, br_actor* pActor, float pArg3, undefined4* pArg4) {
    tNet_message* message;
    br_vector3 p1;
    br_vector3 p2;
    br_vector3 p3;
    br_bounds3 bnds;
    tNet_message_chunk_semi_detach_bit* semi;
    tUser_crush_data* user;

    message = NetBuildGuaranteedMessage(0x24, 0);
    semi = (tNet_message_chunk_semi_detach_bit*)&message->guaranteed.contents.detach_bit;
    semi->ID = NetPlayerFromCar(pCar)->ID;
    user = pActor->user;
    semi->field_0x4 = user->crush_data->id;
    semi->field_0x8 = gPHIL_last_physics_tick + 120;
    if (!GetSDBJointPosAndBounds(&p1, &p2, &p3, &bnds, pActor)) {
        CompressVector3(&semi->field_0xc, &p1, -10.f, 10.f);
        CompressVector3(&semi->field_0x12, &p2, -10.f, 10.f);
        CompressVector3(&semi->field_0x18, &p3, -10.f, 10.f);
        CompressVector3(&semi->field_0x1e, &bnds.min, -10.f, 10.f);
        CompressVector3(&semi->field_0x24, &bnds.max, -10.f, 10.f);
        semi->field_0x2a = DRScalarToU16(pArg3, 0.f, 40.f);
        if (pArg4) {
            SendGuaranteedMessageToPlayer(gCurrent_net_game, message, pArg4[0x9], 0);
        } else {
            NetGuaranteedSendMessageToEverybody(gCurrent_net_game, message, 0);
        }
    }
}

int C2_HOOK_FASTCALL BitIsInBentPartOfCar(br_actor* pActor, float pArg2, float pArg3) {
    tCar_spec* car;
    float delta;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, old_frame_mat, 0x1c);

    if (pActor->user == NULL) {
        return 1;
    }
    /* FIXME: what type is stored in pActor->user? */
    car = *(tCar_spec**)pActor->user;
    if (car == NULL) {
        return 1;
    }
    delta = .4f * (pArg2 - pArg3);
    if (delta < 0.f) {
        return pArg3 + delta > car->old_frame_mat.m[2][2];
    } else {
        return pArg3 + delta < car->old_frame_mat.m[3][2];
    }
}

void C2_HOOK_FASTCALL SendDetachBit(tCar_spec* pCar, br_actor* pActor) {
    tNet_message* message;
    br_vector3 p1;
    br_vector3 p2;
    br_vector3 p3;
    br_bounds3 bnds;

    NOT_IMPLEMENTED(); /* FIXME: what type is pActor->user? */

#ifndef CARPOCALYPSE2_MATCHING
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message, guaranteed.contents.detach_bit.ID, 0x1c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message, guaranteed.contents.detach_bit.field_0x4, 0x20);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message, guaranteed.contents.detach_bit.field_0x8, 0x24);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message, guaranteed.contents.detach_bit.bounds_min, 0x28);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message, guaranteed.contents.detach_bit.bounds_max, 0x2e);
#endif

    message = NetBuildGuaranteedMessage(51, 0);
    message->guaranteed.contents.detach_bit.ID = NetPlayerFromCar(pCar)->ID;
    message->guaranteed.contents.detach_bit.field_0x4 = ((undefined**)pActor->user)[0x8][0x20];
    message->guaranteed.contents.detach_bit.field_0x8 = gPHIL_last_physics_tick + 120;
    if (!GetSDBJointPosAndBounds(&p1, &p2, &p3, &bnds, pActor)) {
        CompressVector3(&message->guaranteed.contents.detach_bit.bounds_min, &bnds.min, -10.f, 10.f);
        CompressVector3(&message->guaranteed.contents.detach_bit.bounds_max, &bnds.max, -10.f, 10.f);
        NetGuaranteedSendMessageToEverybody(gCurrent_net_game, message, 0);
    }
}

// FUNCTION: CARMA2_HW 0x00436ad0
void C2_HOOK_FASTCALL DoDetaching(void) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_crush_spec, field_0x174, 0x174);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_crush_spec, field_0x180, 0x180);

    if (gNet_mode == eNet_mode_none || gNet_mode == eNet_mode_host) {
        int new_count_crush_detach_list;
        int i;

        new_count_crush_detach_list = 0;
        for (i = 0; i < gCount_crush_detach_list; i++) {
            tCrush_detach_list_item* detach_list_item;
            int keep;
            int synced;

            detach_list_item = &gCrush_detach_list[i];
            keep = 1;
            synced = 0;
            if (gPHIL_last_physics_tick >= detach_list_item->time) {
                keep = 0;

                if (!((detach_list_item->car->car_crush_spec->field_0x144
                            && BitIsInBentPartOfCar(detach_list_item->actor,
                                   detach_list_item->car->car_crush_spec->field_0x174,
                                   detach_list_item->car->car_crush_spec->field_0x180))
                        || detach_list_item->car->car_crush_spec->field_0x4b8)) {

                    if (gNet_mode == eNet_mode_none) {
                        if (detach_list_item->field_0x10) {

                            DetachBit(detach_list_item->car, detach_list_item->actor, NULL);
                        } else {
                            SemiDetachBit(detach_list_item->car, detach_list_item->actor, detach_list_item->field_0x8, &keep, NULL, NULL, NULL, NULL);
                        }
                    } else {
                        if (detach_list_item->field_0x10) {
                            int j;

                            for (j = 0; j < gCount_net_crush_detach_list; j++) {
                                if (gNet_crush_detach_list[j].actor == detach_list_item->actor) {
                                    synced = 1;
                                    break;
                                }
                            }
                            if (!synced) {
                                SendDetachBit(detach_list_item->car, detach_list_item->actor);
                            }
                        } else {
                            int j;

                            for (j = 0; j < gCount_net_crush_semi_detach_bit_list; j++) {
                                if (gNet_crush_semi_detach_bit_list[j].actor == detach_list_item->actor) {
                                    synced = 1;
                                    break;
                                }
                            }
                            if (!synced) {
                                SendSemiDetachBit(detach_list_item->car, detach_list_item->actor, detach_list_item->field_0x8, NULL);
                            }
                        }
                    }
                }
            }
            if (keep && i != new_count_crush_detach_list) {
                gCrush_detach_list[new_count_crush_detach_list] = gCrush_detach_list[i];
                new_count_crush_detach_list += 1;
            }
        }
        gCount_crush_detach_list = new_count_crush_detach_list;
    }
}

// FUNCTION: CARMA2_HW 0x0042d7e0
void C2_HOOK_FASTCALL SetBitForDetachment(br_actor* pActor, tCar_spec* pCar, float pArg3, int pArg4) {
    int i;
    tUser_crush_data* user;
    tCar_crush_buffer_entry* crush_data;
    tCar_crush_detach_data* detach_data;
    tCar_crush_spec* car_crush_spec;

    if (gCount_crush_detach_list == CARPOCALYPSE2_ASIZE(gCrush_detach_list)) {
        return;
    }
    if (gNet_mode == eNet_mode_none || gNet_mode == eNet_mode_host) {
        if (pActor == NULL) {
            return;
        }
        user = pActor->user;
        if (user == NULL) {
            return;
        }
        crush_data = user->crush_data;
        if (crush_data == NULL) {
            return;
        }
        detach_data = crush_data->detach_data;
        if (detach_data == NULL) {
            return;
        }
        for (i = 0; i < gCount_crush_detach_list; i++) {
            if (gCrush_detach_list[i].actor == pActor) {
                return;
            }
        }
        car_crush_spec = pCar->car_crush_spec;
        for (i = 0; i < car_crush_spec->field_0x270; i++) {
            if (car_crush_spec->field_0x274[i].field_0x0 == pActor) {
                return;
            }
        }
        gCrush_detach_list[gCount_crush_detach_list].actor = pActor;
        gCrush_detach_list[gCount_crush_detach_list].car = pCar;
        gCrush_detach_list[gCount_crush_detach_list].field_0x8 = pArg3;
        if (pArg4) {
            gCrush_detach_list[gCount_crush_detach_list].time = gPHIL_last_physics_tick;
        } else {
            gCrush_detach_list[gCount_crush_detach_list].time = gPHIL_last_physics_tick > gINT_0067be84 + 0x3e8 ? gPHIL_last_physics_tick : gINT_0067be84 + 0x3e8;
        }
        if (gCrush_detach_list[gCount_crush_detach_list].time > gINT_0067be84) {
            gINT_0067be84 = gCrush_detach_list[gCount_crush_detach_list].time;
        }
        gCrush_detach_list[gCount_crush_detach_list].field_0x10 = (detach_data->type == eDetachType_fully_detachable);
        gCount_crush_detach_list = gCount_crush_detach_list + 1;
    }
}

// FUNCTION: CARMA2_HW 0x0042d950
void C2_HOOK_FASTCALL MakeModelMaterialsDoubleSided(br_model* pModel) {
    int g;

    if (pModel == NULL) {
        return;
    }
    for (g = 0; g < pModel->prepared->ngroups; g++) {
        br_material* material;

        material = pModel->faces[*pModel->prepared->groups[g].face_user].material;
        if (material != NULL && !(material->flags & BR_MATF_TWO_SIDED)) {
            material->flags |= BR_MATF_TWO_SIDED;
            BrMaterialUpdate(material, BR_MATU_RENDERING);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004384c0
intptr_t C2_HOOK_FASTCALL FlapBit(br_actor* pActor, br_matrix34* pMat, void* pContext) {

    NOT_IMPLEMENTED();
    return 0;
}

static __inline void C2_HOOK_FASTCALL StartFlapping(br_actor* pActor, tCar_spec* pCar, tU8 pBits, float pArg4) {
    tUser_crush_data* user_crush;

    user_crush = pActor->user;
    if (user_crush != NULL && user_crush->crush_data != NULL && user_crush->crush_data->flap_data != NULL) {
        tCar_crush_flap_data* flap_data = user_crush->crush_data->flap_data;
        if (flap_data->kev_o_flap) {
            SetBitForDetachment(pActor, pCar, pArg4, 1);
        } else {
            MakeModelMaterialsDoubleSided(user_crush->models[0]);
            PipeSingleDSModel(1, user_crush->models[0]);
            pCar->car_crush_spec->field_0x134 += 1;
            flap_data->field_0x0 = 1;
        }
        pCar->use_shell_model = 0;
    }
}

void C2_HOOK_FASTCALL FlapBits(tCar_spec* pCar) {
    // GLOBAL: CARMA2_HW 0x0067be88
    static tU32 last_flap_bits;
    tCar_crush_spec* car_crush_spec;

    car_crush_spec = pCar->car_crush_spec;
    if (car_crush_spec != NULL) {
        if (car_crush_spec->field_0x134) {
            float dt;
            br_vector3 force;
            tFlapBit_arg flapbit_arg;

            dt = (float)(gPHIL_last_physics_tick - last_flap_bits) * 0.001f;
            BrVector3Sub(&force, &pCar->collision_info->v, &car_crush_spec->field_0x138);
            BrVector3InvScale(&force, &force, dt);
            BrVector3Add(&force, &gBatty_gravity, &force);
            BrMatrix34TApplyV(&flapbit_arg.force, &force, &pCar->car_master_actor->t.t.mat);
            flapbit_arg.car = pCar;
            ARStartPipingSession(ePipe_chunk_flap_bits);
            DRActorEnumRecurseWithSnart(pCar->car_model_actor, NULL, FlapBit, &flapbit_arg);
            AREndPipingSession();
        }
        car_crush_spec->field_0x138 = pCar->collision_info->v;
        last_flap_bits = gPHIL_last_physics_tick;
    }
}

// FUNCTION: CARMA2_HW 0x004381b0
void C2_HOOK_FASTCALL DoFlapping(void) {
    int i;

    for (i = 0; i < gCount_toggled_doors; i++) {
        tToggled_door *toggle_door = &gToggled_doors[i];
        if (toggle_door->actor != NULL) {
            tUser_crush_data *user_crush_data = toggle_door->actor->user;
            if (user_crush_data != NULL && user_crush_data->crush_data != NULL &&
                user_crush_data->crush_data->flap_data != NULL) {
                tCar_crush_flap_data *flap_data = user_crush_data->crush_data->flap_data;
                if (!flap_data->field_0x0 && (toggle_door->field_0x8 & 0x1)) {
                    StartFlapping(toggle_door->actor, toggle_door->car, flap_data->field_0x0, toggle_door->field_0xc);
                } else if (flap_data->field_0x0 && !(toggle_door->field_0x8 & 0x1)) {
                    StopFlapping(toggle_door->actor, toggle_door->car);
                }
            }
        }
    }
    gCount_toggled_doors = 0;
    for (i = 0; i < gNum_active_cars; i++) {
        tCar_spec* car;

        car = gActive_car_list[i];
        if (car != NULL && car->driver >= eDriver_oppo) {
            FlapBits(car);
        }
    }
}

// FUNCTION: CARMA2_HW 0x00434910
int C2_HOOK_FASTCALL BitObjectIsSufficientlyOutsideCarObjectToDetach(tPhysics_object* pObject) {
    int i;

    for (i = 0; i < pObject->physics_joint1->count_limits; i++) {
        br_vector3 tv;
        br_vector3 tv2;

        BrMatrix34ApplyV(&tv, &pObject->physics_joint1->limits[i].child, &pObject->actor->t.t.mat);
        BrMatrix34TApplyV(&tv2, &tv, &pObject->parent->actor->t.t.mat);
        if (BrVector3Dot(&pObject->physics_joint1->limits[i].parent, &tv2) > gCrushSqrtHalf) {
            return 1;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0042f4e0
void C2_HOOK_FASTCALL FullyDetachBit(tCar_spec* pCar, tPhysics_object* pObject) {
    void* bit;
    void* b2;
    void* b3;
    void* state;

    bit = pObject->actor;
    if (bit == NULL) {
        return;
    }
    b2 = *(void**)((tU8*)bit + 0x60);
    if (b2 == NULL) {
        return;
    }
    b3 = *(void**)((tU8*)b2 + 0x8);
    if (b3 == NULL) {
        return;
    }
    state = *(void**)((tU8*)b3 + 0x34);
    if (state == NULL) {
        return;
    }
    if (*(int*)((tU8*)state + 0x28) != 1) {
        return;
    }

    PHILRemoveFromParentChain(pObject);
    if (pObject->physics_joint1 != NULL) {
        *(int*)pObject->physics_joint1 = 0;
        pObject->physics_joint1 = NULL;
    }
    if (gCrush_deferred != 0) {
        PHILDetachObjectState(pCar->collision_info);
    }
    pObject->flags |= 0x100;

    b2 = *(void**)((tU8*)bit + 0x5c);
    if (gNum_active_non_cars != 0x63) {
        *(int*)((tU8*)b2 + 0xc) = gDetached_bit_driver;
        gActive_non_car_list[gNum_active_non_cars] = (tNon_car_spec*)b2;
        gNum_active_non_cars += 1;
        PHILQueueDetachBit(*(void**)((tU8*)b2 + 0x8), 0, 0, 0);
    }
    PHILSendDetachBit(*(tU16*)((tU8*)b2 + 0x80), bit);
    *(tU8*)((tU8*)(*(void**)((tU8*)bit + 0x14)) + 0x3) = 0x21;
    *(int*)((tU8*)state + 0x28) = 2;
    pCar->use_shell_model = 0;
}

#define RECORD_SPLIT_EDGE(pVertex_a, pVertex_b) \
    do { \
        tU16* edge_slot = gSplit_polyhedron_edge_map; \
        while (edge_slot[2] != 0 && \
                (edge_slot[0] != (pVertex_a) || edge_slot[1] != (pVertex_b))) { \
            edge_slot += 5; \
        } \
        if (edge_slot[2] == 0) { \
            edge_slot[0] = (pVertex_a); \
            edge_slot[1] = (pVertex_b); \
            edge_slot[2] = positive_vertex_count; \
            edge_slot[3] = negative_vertex_count; \
            *(tU8*)(edge_slot + 4) = 1; \
            positive_vertex_count++; \
            negative_vertex_count++; \
        } \
    } while (0)

#define WRITE_SPLIT_FACE(pOutput_face, pInput_face, pA, pB, pC) \
    do { \
        br_face* split_output_face = (pOutput_face); \
        split_output_face->vertices[0] = (pA); \
        split_output_face->vertices[1] = (pB); \
        split_output_face->vertices[2] = (pC); \
        split_output_face->smoothing = (pInput_face)->smoothing; \
        split_output_face->material = (pInput_face)->material; \
    } while (0)

// C2PORT-NONMATCHING: best 26.99% | difftest PASS 2000 trials (spec difftest/specs/0042f5d0_FUN_0042f5d0.json) | reason x87-order and stack layout
// FUNCTION: CARMA2_HW 0x0042f5d0
void C2_HOOK_FASTCALL FUN_0042f5d0(br_model* pSource_model, br_model** pModel_out1, br_model** pModel_out2, float* pPlane, int pArg5) {
    br_vertex* source_vertices;
    br_face* source_faces;
    br_model* positive_model;
    br_model* negative_model;
    br_face* positive_faces;
    br_face* negative_faces;
    br_face* input_face;
    tU16 positive_vertex_count;
    tU16 negative_vertex_count;
    tU16 positive_vertex_index;
    tU16 negative_vertex_index;
    tS16 positive_face_count;
    tS16 negative_face_count;
    tU16 positive_face_index;
    tU16 negative_face_index;
    tU16 nvertices;
    tU16 nfaces;
    tU16 i;
    tU16 v0;
    tU16 v1;
    tU16 v2;
    tU16 positive01;
    tU16 negative01;
    tU16 positive02;
    tU16 negative02;
    tU16 positive12;
    tU16 negative12;
    tU8 side;
    tU8 vertex_sides[1024];
    float distances[1024];
    tS16 output_vertex_indices[1024];
    tU8 face_sides[1020];

    if (pSource_model->nvertices > 0x400) {
        PDFatalError("By the death spanners of Mars, there's a lot of vertices on that model.");
    }

    source_vertices = pSource_model->vertices;
    source_faces = pSource_model->faces;
    nvertices = pSource_model->nvertices;
    nfaces = pSource_model->nfaces;
    gSplit_polyhedron_source_vertices = (float*)source_vertices;
    gSplit_polyhedron_source_faces = source_faces;

    positive_vertex_count = 0;
    negative_vertex_count = 0;
    for (i = 0; i < nvertices; i++) {
        br_vertex* vertex = &source_vertices[i];
        float distance;

        distance = vertex->p.v[1] * pPlane[1];
        distance += vertex->p.v[2] * pPlane[2];
        distance += vertex->p.v[0] * pPlane[0];
        distance -= pPlane[3];
        distances[i] = distance;
        if (distance <= gCrushZero) {
            vertex_sides[i] = 1;
            negative_vertex_count++;
        } else {
            vertex_sides[i] = 0;
            positive_vertex_count++;
        }
    }

    for (i = 0; i < 512; i++) {
        gSplit_polyhedron_edge_map[i * 5 + 2] = 0;
    }

    positive_face_count = 0;
    negative_face_count = 0;
    for (i = 0; i < nfaces; i++) {
        input_face = &source_faces[i];
        v0 = input_face->vertices[0];
        v1 = input_face->vertices[1];
        v2 = input_face->vertices[2];
        side = (tU8)((vertex_sides[v2] << 2) | (vertex_sides[v1] << 1) | vertex_sides[v0]);
        face_sides[i] = side;
        switch (side) {
        case 0:
            positive_face_count++;
            break;
        case 1:
            RECORD_SPLIT_EDGE(v1, v0);
            RECORD_SPLIT_EDGE(v2, v0);
            positive_face_count += 2;
            negative_face_count++;
            break;
        case 2:
            RECORD_SPLIT_EDGE(v0, v1);
            RECORD_SPLIT_EDGE(v2, v1);
            positive_face_count += 2;
            negative_face_count++;
            break;
        case 3:
            RECORD_SPLIT_EDGE(v2, v0);
            RECORD_SPLIT_EDGE(v2, v1);
            positive_face_count++;
            negative_face_count += 2;
            break;
        case 4:
            RECORD_SPLIT_EDGE(v0, v2);
            RECORD_SPLIT_EDGE(v1, v2);
            positive_face_count += 2;
            negative_face_count++;
            break;
        case 5:
            RECORD_SPLIT_EDGE(v1, v0);
            RECORD_SPLIT_EDGE(v1, v2);
            positive_face_count++;
            negative_face_count += 2;
            break;
        case 6:
            RECORD_SPLIT_EDGE(v0, v1);
            RECORD_SPLIT_EDGE(v0, v2);
            positive_face_count++;
            negative_face_count += 2;
            break;
        case 7:
            negative_face_count++;
            break;
        }
        gSplit_polyhedron_source_faces++;
    }

    *pModel_out1 = BrModelAllocate(pSource_model->identifier, positive_vertex_count, positive_face_count);
    *pModel_out2 = BrModelAllocate(pSource_model->identifier, negative_vertex_count, negative_face_count);
    positive_model = *pModel_out1;
    negative_model = *pModel_out2;
    positive_model->flags |= 0x880;
    negative_model->flags |= 0x880;
    gSplit_polyhedron_positive_vertices = (float*)positive_model->vertices;
    gSplit_polyhedron_negative_vertices = (float*)negative_model->vertices;

    positive_vertex_index = 0;
    negative_vertex_index = 0;
    for (i = 0; i < nvertices; i++) {
        if (vertex_sides[i] == 0) {
            output_vertex_indices[i] = positive_vertex_index;
            positive_model->vertices[positive_vertex_index].p = source_vertices[i].p;
            positive_model->vertices[positive_vertex_index].map = source_vertices[i].map;
            positive_vertex_index++;
        } else {
            output_vertex_indices[i] = negative_vertex_index;
            negative_model->vertices[negative_vertex_index].p = source_vertices[i].p;
            negative_model->vertices[negative_vertex_index].map = source_vertices[i].map;
            negative_vertex_index++;
        }
    }

    positive_faces = positive_model->faces;
    negative_faces = negative_model->faces;
    positive_face_index = 0;
    negative_face_index = 0;
    gSplit_polyhedron_source_faces = source_faces;
    for (i = 0; i < nfaces; i++) {
        input_face = gSplit_polyhedron_source_faces;
        v0 = input_face->vertices[0];
        v1 = input_face->vertices[1];
        v2 = input_face->vertices[2];
        switch (face_sides[i]) {
        case 0:
            WRITE_SPLIT_FACE(&positive_faces[positive_face_index++], input_face, output_vertex_indices[v0], output_vertex_indices[v1], output_vertex_indices[v2]);
            break;
        case 1:
            FUN_00430150(v1, v0, &positive01, &negative01, distances);
            FUN_00430150(v2, v0, &positive02, &negative02, distances);
            WRITE_SPLIT_FACE(&negative_faces[negative_face_index++], input_face, output_vertex_indices[v0], negative01, negative02);
            WRITE_SPLIT_FACE(&positive_faces[positive_face_index++], input_face, output_vertex_indices[v1], output_vertex_indices[v2], positive01);
            WRITE_SPLIT_FACE(&positive_faces[positive_face_index++], input_face, output_vertex_indices[v2], positive02, positive01);
            break;
        case 2:
            FUN_00430150(v0, v1, &positive01, &negative01, distances);
            FUN_00430150(v2, v1, &positive12, &negative12, distances);
            WRITE_SPLIT_FACE(&positive_faces[positive_face_index++], input_face, output_vertex_indices[v0], positive01, positive12);
            WRITE_SPLIT_FACE(&negative_faces[negative_face_index++], input_face, output_vertex_indices[v1], negative12, negative01);
            WRITE_SPLIT_FACE(&positive_faces[positive_face_index++], input_face, output_vertex_indices[v2], output_vertex_indices[v0], positive12);
            break;
        case 3:
            FUN_00430150(v2, v0, &positive02, &negative02, distances);
            FUN_00430150(v2, v1, &positive12, &negative12, distances);
            WRITE_SPLIT_FACE(&positive_faces[positive_face_index++], input_face, output_vertex_indices[v2], positive02, positive12);
            WRITE_SPLIT_FACE(&negative_faces[negative_face_index++], input_face, output_vertex_indices[v0], negative12, negative02);
            WRITE_SPLIT_FACE(&negative_faces[negative_face_index++], input_face, output_vertex_indices[v1], negative12, output_vertex_indices[v0]);
            break;
        case 4:
            FUN_00430150(v0, v2, &positive02, &negative02, distances);
            FUN_00430150(v1, v2, &positive12, &negative12, distances);
            WRITE_SPLIT_FACE(&positive_faces[positive_face_index++], input_face, output_vertex_indices[v0], positive12, positive02);
            WRITE_SPLIT_FACE(&positive_faces[positive_face_index++], input_face, output_vertex_indices[v1], positive12, output_vertex_indices[v0]);
            WRITE_SPLIT_FACE(&negative_faces[negative_face_index++], input_face, output_vertex_indices[v2], negative02, negative12);
            break;
        case 5:
            FUN_00430150(v1, v0, &positive01, &negative01, distances);
            FUN_00430150(v1, v2, &positive12, &negative12, distances);
            WRITE_SPLIT_FACE(&negative_faces[negative_face_index++], input_face, output_vertex_indices[v0], negative01, negative12);
            WRITE_SPLIT_FACE(&positive_faces[positive_face_index++], input_face, output_vertex_indices[v1], positive12, positive01);
            WRITE_SPLIT_FACE(&negative_faces[negative_face_index++], input_face, output_vertex_indices[v2], output_vertex_indices[v0], negative12);
            break;
        case 6:
            FUN_00430150(v0, v1, &positive01, &negative01, distances);
            FUN_00430150(v0, v2, &positive02, &negative02, distances);
            WRITE_SPLIT_FACE(&positive_faces[positive_face_index++], input_face, output_vertex_indices[v0], positive01, positive02);
            WRITE_SPLIT_FACE(&negative_faces[negative_face_index++], input_face, output_vertex_indices[v1], output_vertex_indices[v2], negative01);
            WRITE_SPLIT_FACE(&negative_faces[negative_face_index++], input_face, output_vertex_indices[v2], negative02, negative01);
            break;
        case 7:
            WRITE_SPLIT_FACE(&negative_faces[negative_face_index++], input_face, output_vertex_indices[v0], output_vertex_indices[v1], output_vertex_indices[v2]);
            break;
        }
        gSplit_polyhedron_source_faces++;
    }

    WhitenVertexRGB(pModel_out1, 1);
    BrModelAdd(*pModel_out1);
    if (pArg5 != 0) {
        BrModelFree(*pModel_out2);
        return;
    }
    WhitenVertexRGB(pModel_out2, 1);
    BrModelAdd(*pModel_out2);
}

#undef WRITE_SPLIT_FACE
#undef RECORD_SPLIT_EDGE

// FUNCTION: CARMA2_HW 0x004314b0
void C2_HOOK_FASTCALL SendFullyDetachBit(tCar_spec* pCar, tPhysics_object* pObject) {
    tNet_message* message;
    br_vector3 p1;
    br_vector3 p2;
    br_vector3 p3;
    br_bounds3 bnds;
    tNet_message_chunk_semi_detach_bit* chunk;

    message = NetBuildGuaranteedMessage(0x25, 0);
    chunk = (tNet_message_chunk_semi_detach_bit*)&message->guaranteed.contents.detach_bit;
    chunk->ID = NetPlayerFromCar(pCar)->ID;
    chunk->field_0x4 = pObject->uid;
    chunk->field_0x8 = gPHIL_last_physics_tick + 120;
    if (!GetSDBJointPosAndBounds(&p1, &p2, &p3, &bnds, pObject->actor)) {
        CompressVector3(&chunk->field_0xc, &p1, -10.f, 10.f);
        CompressVector3(&chunk->field_0x12, &p2, -10.f, 10.f);
        CompressVector3(&chunk->field_0x18, &p3, -10.f, 10.f);
        CompressVector3(&chunk->field_0x1e, &bnds.min, -10.f, 10.f);
        CompressVector3(&chunk->field_0x24, &bnds.max, -10.f, 10.f);
        NetGuaranteedSendMessageToEverybody(gCurrent_net_game, message, 0);
    }
}

// FUNCTION: CARMA2_HW 0x00431310
int C2_HOOK_FASTCALL FullyDetachSnappedBits(tCollision_contact* pContacts, float* pImpulses, int pCount) {
    tPhysics_object* object1;
    tPhysics_object* parent;
    tCar_spec* car;
    tUser_crush_data* user;
    float snap_torque;
    int index;
    int i;

    for (i = 0; i < pCount; i++) {
        if (pContacts[i].type == 0xb
                && (object1 = pContacts[i].object1) != NULL
                && object1->owner != NULL
                && object1->flags_0x238 == 1
                && (parent = object1->parent) != NULL
                && parent->owner != NULL
                && parent->flags_0x238 == 1
                && (car = (tCar_spec*)parent->owner) != NULL
                && car->driver > 5
                && car->invulnerable_no_crushage == 0
                && (user = object1->actor->user) != NULL
                && user->crush_data != NULL
                && user->crush_data->flap_data != NULL
                && user->crush_data->flap_data->kev_o_flap != 0) {
            snap_torque = object1->M * gTorque_to_snap_per_tonne;
            if (pImpulses[i] > snap_torque) {
                if (BrVector3Dot(&pContacts[i].normal, &object1->physics_joint1->hinge_axis) < gCrushZero) {
                    if (gNet_mode == 0) {
                        index = i;
                        ScaleCollisionImpulses(pContacts, pImpulses, pCount, &index, 1, snap_torque);
                        FullyDetachBit(car, object1);
                        MungeDetachLists(car->car_crush_spec);
                        return 1;
                    } else if (gNet_mode == 2) {
                        SendFullyDetachBit(car, object1);
                    }
                }
            }
        }
    }
    return 0;
}

// C2PORT-NONMATCHING: best 44.99% | difftest PASS 2000 trials (spec difftest/specs/0042dfd0_TrySplitCarOnVerticalForce.json) | reason x87-order/stack-layout
// FUNCTION: CARMA2_HW 0x0042dfd0
int C2_HOOK_FASTCALL TrySplitCarOnVerticalForce(tPhysics_joint_data* pJoint_data, float* pForces, int pCount) {
    int force_processed[40];
    float split_plane[4];
    int split_joint_indices[40];
    int force_index;
    int scan_index;
    int split_joint_count;
    int can_split;
    float split_y;
    float total_force;
    float force_threshold;
    tPhysics_joint_data* volatile joint_base;
    tPhysics_joint_data* joint_data;
    tPhysics_joint_data* scan_joint_data;
    tPhysics_object* object;
    tCar_spec* car;
    tCar_crush_spec* crush_spec;

    joint_base = pJoint_data;
    if (FUN_00430e30(pJoint_data, pForces, pCount) != 0) {
        return 1;
    }
    if (pCount > 40) {
        pCount = 40;
    }
    for (force_index = 0; force_index < pCount; force_index++) {
        force_processed[force_index] = 0;
    }
    force_index = 0;
    if (pCount < 1) {
        return 0;
    }
    joint_data = joint_base;
    do {
        object = joint_data->object1;
        if (pForces[force_index] >= gFLOAT_00589a58
                && force_processed[force_index] == 0
                && joint_data->object2 == NULL
                && object != NULL
                && object->flags_0x238 == 1
                && (car = (tCar_spec*)object->owner) != NULL
                && car->driver > 5
                && (crush_spec = car->car_crush_spec) != NULL
                && crush_spec->field_0x4b8 == 0
                && *(int*)((tU8*)car + 0x1b4) == 0) {
            split_y = crush_spec->split_y_pos - car->collision_info->cmpos.v[1];
            can_split = 1;
            if (joint_data->anchor1.v[1] < split_y) {
                can_split = 0;
            }
            if (can_split) {
                split_joint_indices[0] = force_index;
                split_joint_count = 1;
                total_force = pForces[force_index];
            }
            force_processed[force_index] = 1;
            scan_index = force_index + 1;
            scan_joint_data = joint_data + 1;
            while (scan_index < pCount) {
                object = scan_joint_data->object1;
                if (pForces[scan_index] >= gFLOAT_00589a58
                        && force_processed[scan_index] == 0
                        && scan_joint_data->object2 == NULL
                        && object != NULL
                        && object->owner == car) {
                    if (can_split) {
                        if (scan_joint_data->anchor1.v[1] >= split_y) {
                            split_joint_indices[split_joint_count] = scan_index;
                            split_joint_count++;
                            total_force += pForces[scan_index];
                        } else {
                            can_split = 0;
                        }
                    }
                    force_processed[scan_index] = 1;
                }
                scan_index++;
                scan_joint_data++;
            }
            if (can_split) {
                force_threshold = (car->collision_info->M * gMin_force_to_split_Y_per_tonne)
                    / (crush_spec->snappability_factor * *(float*)((tU8*)car + 0x18d0));
                if (force_threshold < total_force
                        && SplitCheck(car, 1, NULL, crush_spec->split_y_pos, split_plane) != 0) {
                    ScaleCollisionImpulses((int)joint_base, pForces, pCount, split_joint_indices, split_joint_count, force_threshold);
                    FUN_0042e2e0(car, split_plane, 1);
                    return 1;
                }
            }
        }
        force_index++;
        joint_data++;
    } while (force_index < pCount);
    return 0;
}

// C2PORT-NONMATCHING: best 26.83% | difftest PASS 2000 trials (spec difftest/specs/00430e30_FUN_00430e30.json) | reason VC5 register and stack layout
// FUNCTION: CARMA2_HW 0x00430e30
int C2_HOOK_FASTCALL FUN_00430e30(tPhysics_joint_data* pJoint_data, float* pForces, int pCount) {
    register int face_index;
    register int edge_index;
    int next_edge_index;
    register int hit_count;
    int joint_index;
    int hit_faces[2];
    register tPhysics_object* object;
    register tPhysics_object* car_physics;
    tCar_spec* car;
    register tCar_crush_spec* crush_spec;
    register tFace_ref* face;
    br_vector3 edge;
    br_vector3 transformed_edge;
    br_vector3 split_normal;
    float split_input[3];
    float split_plane[4];
    float magnitude;
    float force_threshold;
    float component;
    float centre_x;
    float centre_z;
    float horizontal_length;
    float horizontal_x;
    float horizontal_z;
    tPhysics_joint_data* joint_data;
    register float* face_vertices;
    if (pCount > 0x28) {
        pCount = 0x28;
    }
    if (pCount < 1) {
        return 0;
    }

    joint_index = 0;
    joint_data = pJoint_data;
    do {
        object = joint_data->object1;
        if (gFLOAT_00589a58 <= pForces[joint_index]
                && joint_data->object2 == NULL
                && joint_data->field_0x5c == 0
                && object != NULL
                && object->flags_0x238 == 1
                && (car = (tCar_spec*)object->owner) != NULL
                && car->driver > 5 && car->car_crush_spec != NULL
                    && car->car_crush_spec->field_0x4b8 == 0
                    && *(int*)((tU8*)car + 0x1b4) == 0) {
                crush_spec = car->car_crush_spec;
                car_physics = car->collision_info;
                force_threshold = (car_physics->M * gMin_force_to_split_XZ_per_tonne)
                    / (crush_spec->snappability_factor * *(float*)((tU8*)car + 0x18d0));
                if (force_threshold <= pForces[joint_index]) {
                    hit_count = 0;
                    face_index = car_physics->box_face_start;
                    if (face_index < car_physics->box_face_end) {
                        do {
                            if (hit_count > 1) {
                                goto found_two_faces;
                            }
                            face = &gFace_list__car[face_index];
                            face_vertices = (float*)((tU8*)face + 4);
                            for (edge_index = 0; edge_index < 3; edge_index++) {
                                next_edge_index = (edge_index + 1) % 3;
                                if (PointEdgeDistSq(&joint_data->anchor2, &face->v[edge_index], &face->v[next_edge_index])
                                        < gFLOAT_00589ab4) {
                                    hit_faces[hit_count] = face_index;
                                    hit_count++;
                                    edge.v[0] = face_vertices[edge_index * 3 + 1]
                                        - face_vertices[next_edge_index * 3 + 1];
                                    edge.v[1] = face_vertices[edge_index * 3 + 2]
                                        - face_vertices[next_edge_index * 3 + 2];
                                    edge.v[2] = face_vertices[(edge_index + 1) * 3]
                                        - face_vertices[(next_edge_index + 1) * 3];
                                    break;
                                }
                            }
                            face_index++;
                        } while (face_index < car_physics->box_face_end);
                    }
                    if (hit_count > 1) {
found_two_faces:
                        if (gFace_list__car[hit_faces[0]].normal.v[2] * gFace_list__car[hit_faces[1]].normal.v[2]
                                + gFace_list__car[hit_faces[0]].normal.v[1] * gFace_list__car[hit_faces[1]].normal.v[1]
                                + gFace_list__car[hit_faces[0]].normal.v[0] * gFace_list__car[hit_faces[1]].normal.v[0]
                                <= cos(gDOUBLE_00589ab8)) {
                            BrMatrix34TApplyV(&transformed_edge, &edge, &car_physics->actor->t.t.mat);
                            split_normal.v[0] = joint_data->axis1.v[2] * transformed_edge.v[0]
                                - joint_data->axis1.v[1] * transformed_edge.v[2];
                            split_normal.v[1] = joint_data->axis1.v[0] * transformed_edge.v[2]
                                - joint_data->axis1.v[2] * transformed_edge.v[1];
                            split_normal.v[2] = joint_data->axis1.v[1] * transformed_edge.v[1]
                                - joint_data->axis1.v[0] * transformed_edge.v[0];
                            magnitude = (float)sqrt((double)(split_normal.v[0] * split_normal.v[0]
                                + split_normal.v[1] * split_normal.v[1]
                                + split_normal.v[2] * split_normal.v[2]));
                            if (magnitude <= gFLOAT_00589a5c) {
                                split_normal.v[0] = 1.0f;
                                split_normal.v[1] = 0.0f;
                                split_normal.v[2] = 0.0f;
                            } else {
                                component = (float)gDOUBLE_00589a60 / magnitude;
                                split_normal.v[0] *= component;
                                split_normal.v[1] *= component;
                                split_normal.v[2] *= component;
                            }
                            if (gCrushSqrtHalf < (float)fabs(split_normal.v[0])
                                    || gCrushSqrtHalf < (float)fabs(split_normal.v[2])) {
                                centre_x = joint_data->anchor1.v[0] + car_physics->cmpos.v[0];
                                centre_z = joint_data->anchor1.v[2] + car_physics->cmpos.v[2];
                                horizontal_length = (float)sqrt((double)(split_normal.v[0] * split_normal.v[0]
                                    + split_normal.v[2] * split_normal.v[2]));
                                if (horizontal_length <= gFLOAT_00589a5c) {
                                    horizontal_x = 1.0f;
                                    horizontal_z = 0.0f;
                                } else {
                                    horizontal_length = (float)gDOUBLE_00589a60 / horizontal_length;
                                    horizontal_x = horizontal_length * split_normal.v[0];
                                    horizontal_z = horizontal_length * split_normal.v[2];
                                }
                                split_input[0] = horizontal_x;
                                split_input[1] = horizontal_z;
                                split_input[2] = horizontal_z * centre_z + horizontal_x * centre_x;
                                if (SplitCheck(car, 0, split_input, 0.0f, split_plane) != 0) {
                                    ScaleCollisionImpulses((int)pJoint_data, pForces, pCount, &joint_index, 1, force_threshold);
                                    FUN_0042e2e0(car, split_plane, 1);
                                    return 1;
                                }
                            }
                        }
                    }
                }
        }
        joint_index++;
        joint_data++;
    } while (joint_index < pCount);
    return 0;
}

// FUNCTION: CARMA2_HW 0x0042f3d0
void C2_HOOK_FASTCALL MungeDetachLists(tCar_crush_spec* pCar_crush) {
    int i;
    int new_count;

    new_count = 0;
    for (i = 0; i < pCar_crush->field_0x270; i++) {
        tCar_bit_spec* car_bit = &pCar_crush->field_0x274[i];

        /* FIXME: what is type ot tCar_bit_spec::field_0xc? */
        if (car_bit->field_0xc->field_0x28 == 1) {
            if (i != new_count) {
                pCar_crush->field_0x274[new_count] = *car_bit;
                new_count += 1;
            }
        } else if (car_bit->field_0xc->field_0x28 == 2) {
            pCar_crush->field_0x2b4[pCar_crush->field_0x2b0] = *car_bit;
            pCar_crush->field_0x2b0 += 1;
        }
    }
    pCar_crush->field_0x270 = new_count;

    new_count = 0;
    for (i = 0; i < pCar_crush->field_0x2b0; i++) {
        tCar_bit_spec* car_bit = &pCar_crush->field_0x2b4[i];

        /* FIXME: what is type ot tCar_bit_spec::field_0xc? */
        if (car_bit->field_0xc->field_0x28 == 1) {
            pCar_crush->field_0x274[pCar_crush->field_0x270] = *car_bit;
            pCar_crush->field_0x270 += 1;
        } else if (car_bit->field_0xc->field_0x28 == 2) {
            if (i != new_count) {
                pCar_crush->field_0x2b4[new_count] = *car_bit;
                new_count += 1;
            }
        }
    }
    pCar_crush->field_0x2b0 = new_count;
}

// FUNCTION: CARMA2_HW 0x00438900
void C2_HOOK_FASTCALL DoFullyDetaching(void) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, car_crush_spec, 0x18d4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_crush_spec, field_0x270, 0x270);

    if (gNet_mode == eNet_mode_none || gNet_mode == eNet_mode_host) {
        int i;

        for (i = 0; i < gNum_active_cars; i++) {
            tCar_spec* car;

            car = gActive_car_list[i];
            if (car != NULL && car->driver > 5 && car->car_crush_spec != NULL) {
                tCar_crush_spec* car_crush;
                int j;

                car_crush = car->car_crush_spec;
                for (j = 0; j < car_crush->field_0x270; j++) {
                    int detach;
                    tCar_bit_spec* bit_car = &car_crush->field_0x274[j];
                    int dt;

                    /* field_0xc type unresolved; index 9 matches jump table (cases 0,1,2,4) */
                    switch (bit_car->field_0xc->type) {
                    case 2:
                    case 4:
                        detach = 1;
                        break;
                    case 0:
                    case 1:
                        detach = BitObjectIsSufficientlyOutsideCarObjectToDetach(bit_car->field_0x8);
                        dt = PDGetTotalTime() - bit_car->field_0x10;
                        if (dt < gMax_detach_time_ms) {
                            detach &= PercentageChance((int)((float)(100 * dt) / (float)gMax_detach_time_ms));
                        }
                        break;
                    default:
                        detach = 0;
                        break;
                    }
                    if (detach) {
                        if (gNet_mode != eNet_mode_none) {
                            int bit_synced;
                            int k;

                            bit_synced = 0;
                            for (k = 0; k < gCount_net_crush_full_detach_bit_list; k++) {
                                if (gNet_crush_full_detach_bit_list[k].object == bit_car->field_0x8) {
                                    bit_synced = 1;
                                    break;
                                }
                            }
                            if (!bit_synced) {
                                SendFullyDetachBit(car, bit_car->field_0x8);
                            }
                        } else {
                            FullyDetachBit(car, bit_car->field_0x8);
                        }
                    }
                }
                MungeDetachLists(car_crush);
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x00433c70
void C2_HOOK_FASTCALL BendCar(tCar_spec* pCar, br_angle pAngle_x, br_angle pAngle_y, br_angle pAngle_z, const br_vector3* pArg5, float pArg6, int pArg7) {

    NOT_IMPLEMENTED();
}

br_angle C2_HOOK_FASTCALL QuantizeAngle(br_angle pAngle, br_angle pMin, br_angle pMax, br_angle pQuant) {
    if (pAngle < BrDegreeToAngle(179.96f) && pAngle > pMax) {
        return pMax;
    } else if (pAngle > BrDegreeToAngle(179.96f) && pAngle < pMin) {
         return pMin;
    } else {
        return pQuant * (pAngle / pQuant);
    }
}

int C2_HOOK_FASTCALL BendCarOneForce(tCar_spec* pCar, tCar_crush* pCar_crush) {
    tPhysics_object* object;
    int bend_max;
    float bb_z;
    br_vector3 bend_v;
    br_angle angle_x;
    br_angle angle_y;

    object = pCar->collision_info;

    BrVector3Set(&bend_v, 0.f, 0.f, 0.f);
    if (pCar_crush->field_0x34.v[2] > pCar->car_crush_spec->bend_z_max) {
        bend_max = 1;
        bb_z = object->bb1.min.v[2];
        bend_v.v[2] = pCar->car_crush_spec->bend_z_max;
    } else if (pCar_crush->field_0x34.v[2] < pCar->car_crush_spec->bend_z_min) {
        bend_max = 0;
        bb_z = object->bb1.max.v[2];
        bend_v.v[2] = pCar->car_crush_spec->bend_z_min;
    } else {
        return 0;
    }
    angle_x = 0;
    angle_y = 0;

    if (fabsf(pCar_crush->field_0x44.v[2]) > sqrtf(.5f)) {
        float f = fabsf(pCar_crush->field_0x44.v[2]) * pCar_crush->field_0x40;

        srand(pCar_crush->field_0x50);
        if (PercentageChance(50)) {
            bend_v.v[1] = object->bb1.max.v[1];
            if (bend_max) {
                angle_x = BrRadianToAngle(atan2f(f, gMin_bend_force));
            } else {
                angle_x = BrRadianToAngle(atan2f(-f, gMin_bend_force));
            }
        } else {
            bend_v.v[1] = object->bb1.min.v[1];
            if (bend_max) {
                angle_x = BrRadianToAngle(atan2f(-f, gMin_bend_force));
            } else {
                angle_x = BrRadianToAngle(atan2f(f, gMin_bend_force));
            }
        }
    } else if (fabsf(pCar_crush->field_0x44.v[1]) > sqrtf(.5f)) {
        float f = fabsf(pCar_crush->field_0x44.v[1]) * pCar_crush->field_0x40;
        if (pCar_crush->field_0x44.v[0] < 0.f) {
            bend_v.v[0] = object->bb1.max.v[0];
            if (bend_max) {
                angle_y = BrRadianToAngle(atan2f(-f, gMin_bend_force));
            } else {
                angle_y = BrRadianToAngle(atan2f(f, gMin_bend_force));
            }
        } else {
            bend_v.v[0] = object->bb1.min.v[0];
            if (bend_max) {
                angle_y = BrRadianToAngle(atan2f(f, gMin_bend_force));
            } else {
                angle_y = BrRadianToAngle(atan2f(-f, gMin_bend_force));
            }
        }
    } else {
        float f = fabsf(pCar_crush->field_0x44.v[1]) * pCar_crush->field_0x40;
        if (pCar_crush->field_0x44.v[1] < 0.f) {
            bend_v.v[1] = object->bb1.max.v[1];
            if (bend_max) {
                angle_x = BrRadianToAngle(atan2f(f, gMin_bend_force));
            } else {
                angle_x = BrRadianToAngle(atan2f(-f, gMin_bend_force));
            }
        } else {
            bend_v.v[1] = object->bb1.min.v[1];
            if (bend_max) {
                angle_x = BrRadianToAngle(atan2f(-f, gMin_bend_force));
            } else {
                angle_x = BrRadianToAngle(atan2f(f, gMin_bend_force));
            }
        }
    }
    BendCar(pCar,
        QuantizeAngle(angle_x, BrDegreeToAngle(290), BrDegreeToAngle(70), BrDegreeToAngle(20)),
        QuantizeAngle(angle_y, BrDegreeToAngle(310), BrDegreeToAngle(50), BrDegreeToAngle(20)),
        0, &bend_v, bb_z, 1);
    DRS3StartSound3D(gCar_outlet, eSoundId_BendCar, &pCar->collision_info->actor->t.t.translate.t, &gZero_v__car, 1, 255, IRandomBetween(0xe000, 0x12000), 0x10000);
    return 1;
}

// FUNCTION: CARMA2_HW 0x00438a90
void C2_HOOK_FASTCALL DoBending(void) {
    int i;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, car_crush_spec, 0x18d4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_crush_spec, field_0x4b8, 0x4b8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_crush_spec, field_0x144, 0x144);

    for (i = 0; gCrush_lists[i].car_spec != NULL; i++) {

        tCar_crush_spec* ptVar2 = gCrush_lists[i].car_spec->car_crush_spec;
        if (!ptVar2->field_0x4b8 && !ptVar2->field_0x144) {
            int j;

            for (j = 0; j < gCrush_lists[i].count; j++) {
                if (gCrush_lists[i].items[j].field_0x40 < 0.f) {
                    gCrush_lists[i].items[j].field_0x40 = -gCrush_lists[i].items[j].field_0x40;
                    if (BendCarOneForce(gCrush_lists[i].car_spec, &gCrush_lists[i].items[j])) {
                        break;
                    }
                }
            }
        }
        gCrush_lists[i].car_spec = NULL;
        i = i + 1;
    }
}

// C2PORT-NONMATCHING: best 49.32% | difftest PASS 2000 trials (spec difftest/specs/00438e50_FUN_00438e50.json) | reason stack-layout
// FUNCTION: CARMA2_HW 0x00438e50
void C2_HOOK_FASTCALL FUN_00438e50(void) {
    tCar_spec* car;
    tPhysics_object* collision_info;
    tCar_crush_spec* crush_spec;
    br_vector3 bend_position;

    switch (gINT_0067be8c % 5) {
    case 0:
        car = gCar_to_view;
        collision_info = car->collision_info;
        crush_spec = car->car_crush_spec;
        bend_position.v[0] = 0.f;
        bend_position.v[1] = collision_info->bb1.max.v[1];
        bend_position.v[2] = *(br_scalar*)((tU8*)crush_spec + 0x14c);
        BendCar(car, 0xe002, 0, 0, (br_vector3*)&bend_position, collision_info->bb1.max.v[2], 0);
        break;
    case 1:
        car = gCar_to_view;
        collision_info = car->collision_info;
        crush_spec = car->car_crush_spec;
        bend_position.v[0] = 0.f;
        bend_position.v[1] = collision_info->bb1.min.v[1];
        bend_position.v[2] = *(br_scalar*)((tU8*)crush_spec + 0x14c);
        BendCar(car, 0x1ffe, 0, 0, (br_vector3*)&bend_position, collision_info->bb1.max.v[2], 0);
        break;
    case 2:
        car = gCar_to_view;
        collision_info = car->collision_info;
        crush_spec = car->car_crush_spec;
        bend_position.v[0] = collision_info->bb1.min.v[0];
        bend_position.v[1] = 0.f;
        bend_position.v[2] = *(br_scalar*)((tU8*)crush_spec + 0x14c);
        BendCar(car, 0, 0xe002, 0, (br_vector3*)&bend_position, collision_info->bb1.max.v[2], 0);
        break;
    case 3:
        car = gCar_to_view;
        collision_info = car->collision_info;
        crush_spec = car->car_crush_spec;
        bend_position.v[0] = collision_info->bb1.max.v[0];
        bend_position.v[1] = 0.f;
        bend_position.v[2] = *(br_scalar*)((tU8*)crush_spec + 0x14c);
        BendCar(car, 0, 0x1ffe, 0, (br_vector3*)&bend_position, collision_info->bb1.max.v[2], 0);
        break;
    case 4:
        car = gCar_to_view;
        collision_info = car->collision_info;
        crush_spec = car->car_crush_spec;
        bend_position.v[0] = (collision_info->bb1.max.v[0] + collision_info->bb1.min.v[0]) * gFLOAT_00589a2c;
        bend_position.v[1] = (collision_info->bb1.max.v[1] + collision_info->bb1.min.v[1]) * gFLOAT_00589a2c;
        bend_position.v[2] = *(br_scalar*)((tU8*)crush_spec + 0x150);
        BendCar(car, 0, 0, 0x1ffe, (br_vector3*)&bend_position, collision_info->bb1.min.v[2], 0);
        break;
    default:
        goto increment_counter;
    }
increment_counter:
    gINT_0067be8c++;
}

// C2PORT-NONMATCHING: best 37.28% | difftest PASS 2000 trials (spec difftest/specs/00430150_FUN_00430150.json) | reason x87-order and register allocation
// FUNCTION: CARMA2_HW 0x00430150
void C2_HOOK_FASTCALL FUN_00430150(tU16 pVertex_a, tU16 pVertex_b, tU16* pOutput_a, tU16* pOutput_b, float* pDistances) {
    tU16* puVar1;
    float* pfVar2;
    float fVar3;
    float fVar4;
    float fVar5;
    float fVar6;
    tU32 uVar7;
    tU16* puVar8;
    int iVar9;
    int iVar10;
    double fVar11;
    double fVar12;

    puVar8 = gSplit_polyhedron_edge_map;
    if (gSplit_polyhedron_edge_map[2] != 0) {
        while ((*puVar8 != pVertex_a) || (puVar8[1] != pVertex_b)) {
            puVar1 = puVar8 + 7;
            puVar8 = puVar8 + 5;
            if (*puVar1 == 0) {
                return;
            }
        }
        *pOutput_a = puVar8[2];
        *pOutput_b = puVar8[3];
        if ((char)puVar8[4] != '\0') {
            uVar7 = (tU32)pVertex_b;
            pfVar2 = gSplit_polyhedron_positive_vertices + (tU32)*pOutput_a * 10;
            fVar11 = SRandomBetween(-0.028985507786273956f, 0.028985507786273956f);
            fVar11 = fabs((pDistances[uVar7] + fVar11) / (pDistances[(tU32)pVertex_a] - pDistances[uVar7]));
            if (gCrushZero < fVar11) {
                if (gFLOAT_00589a54 <= fVar11) {
                    fVar11 = gFLOAT_00589a9c;
                }
            } else {
                fVar11 = gFLOAT_00589a98;
            }
            iVar9 = uVar7 * 0x28;
            fVar12 = gFLOAT_00589a54 - fVar11;
            fVar3 = *(float*)((tU8*)gSplit_polyhedron_source_vertices + 8 + iVar9);
            iVar10 = (tU32)pVertex_a * 0x28;
            fVar4 = *(float*)((tU8*)gSplit_polyhedron_source_vertices + 8 + iVar10);
            fVar5 = *(float*)((tU8*)gSplit_polyhedron_source_vertices + iVar10);
            fVar6 = *(float*)((tU8*)gSplit_polyhedron_source_vertices + iVar9);
            pfVar2[1] = (float)(*(float*)((tU8*)gSplit_polyhedron_source_vertices + 4 + iVar9) * fVar12
                    + (float)(*(float*)((tU8*)gSplit_polyhedron_source_vertices + 4 + iVar10) * fVar11));
            pfVar2[2] = (float)(fVar3 * fVar12 + (float)(fVar4 * fVar11));
            pfVar2[0] = (float)(fVar6 * fVar12 + fVar5 * fVar11);
            fVar3 = *(float*)((tU8*)gSplit_polyhedron_source_vertices + 0xc + iVar9);
            fVar4 = *(float*)((tU8*)gSplit_polyhedron_source_vertices + 0xc + iVar10);
            pfVar2[4] = (float)(*(float*)((tU8*)gSplit_polyhedron_source_vertices + 0x10 + iVar9) * fVar12
                    + (float)(*(float*)((tU8*)gSplit_polyhedron_source_vertices + 0x10 + iVar10) * fVar11));
            pfVar2[3] = (float)(fVar4 * fVar11 + fVar3 * fVar12);
            *(tU32*)((tU8*)gSplit_polyhedron_negative_vertices + (tU32)*pOutput_b * 0x28)
                    = *(tU32*)((tU8*)gSplit_polyhedron_positive_vertices + (tU32)*pOutput_a * 0x28);
            *(tU32*)((tU8*)gSplit_polyhedron_negative_vertices + 4 + (tU32)*pOutput_b * 0x28)
                    = *(tU32*)((tU8*)gSplit_polyhedron_positive_vertices + 4 + (tU32)*pOutput_a * 0x28);
            *(tU32*)((tU8*)gSplit_polyhedron_negative_vertices + 8 + (tU32)*pOutput_b * 0x28)
                    = *(tU32*)((tU8*)gSplit_polyhedron_positive_vertices + 8 + (tU32)*pOutput_a * 0x28);
            *(tU32*)((tU8*)gSplit_polyhedron_negative_vertices + 0xc + (tU32)*pOutput_b * 0x28)
                    = *(tU32*)((tU8*)gSplit_polyhedron_positive_vertices + 0xc + (tU32)*pOutput_a * 0x28);
            *(tU32*)((tU8*)gSplit_polyhedron_negative_vertices + 0x10 + (tU32)*pOutput_b * 0x28)
                    = *(tU32*)((tU8*)gSplit_polyhedron_positive_vertices + 0x10 + (tU32)*pOutput_a * 0x28);
            ((tU8*)puVar8)[8] = 0;
        }
    }
}

// FUNCTION: CARMA2_HW 0x00430600
void C2_HOOK_FASTCALL ComputeSplitEdgeIntersections(br_vector3* pPositive_intersection, br_vector3* pNegative_intersection, const br_vector3* pPositive_point, const br_vector3* pNegative_point, float pPositive_distance, float pNegative_distance) {
    float fVar1;
    float fVar2;
    float fVar3;
    float fVar4;
    volatile float fVar5;
    float fVar6;
    float fVar7;
    float fVar8;
    fVar7 = pPositive_distance - gFLOAT_00589aa4;
    fVar5 = -((pPositive_distance - gFLOAT_00589aa0) / (pNegative_distance - pPositive_distance));
    fVar8 = gFLOAT_00589a54 - fVar5;
    fVar1 = pNegative_point->v[2];
    fVar2 = pPositive_point->v[2];
    fVar3 = pPositive_point->v[0];
    fVar4 = pNegative_point->v[0];
    pPositive_intersection->v[1] = pNegative_point->v[1] * fVar5 + pPositive_point->v[1] * fVar8;
    pPositive_intersection->v[2] = fVar1 * fVar5 + fVar2 * fVar8;
    pPositive_intersection->v[0] = fVar5 * fVar4 + fVar3 * fVar8;
    fVar1 = pPositive_point->v[2];
    fVar5 = -(fVar7 / (pNegative_distance - pPositive_distance));
    fVar6 = gFLOAT_00589a54 - fVar5;
    fVar2 = pNegative_point->v[2];
    fVar3 = pPositive_point->v[0];
    fVar4 = pNegative_point->v[0];
    pNegative_intersection->v[1] = pNegative_point->v[1] * fVar5 + pPositive_point->v[1] * fVar6;
    pNegative_intersection->v[2] = fVar2 * fVar5 + fVar1 * fVar6;
    pNegative_intersection->v[0] = fVar5 * fVar4 + fVar3 * fVar6;
}

// C2PORT-NONMATCHING: best 26.13% | difftest PASS 2000 trials (spec difftest/specs/004303c0_FUN_004303c0.json) | reason x87-order and register allocation
// FUNCTION: CARMA2_HW 0x004303c0
void C2_HOOK_FASTCALL SplitPolyhedronByPlane(int* pSource, int pPositive_output, int pNegative_output, float* pPlane) {
    float afStack_80[16];
    int aiStack_40[16];
    int iVar1;
    undefined4 uVar2;
    tU32 uVar3;
    tU32 uVar4;
    int iVar5;
    tU32 uVar6;
    undefined4* puVar7;
    undefined4* puVar8;
    undefined4* puVar9;
    float fVar10;
    float fVar11;
    int local_90;
    int local_8c;
    int local_88;

    if (*pSource != 1) {
        PDFatalError("Oooh no. I can't split that sort of shape, Mrs Jabberwok.");
    }

    puVar9 = *(undefined4**)(pPositive_output + 0x44);
    iVar5 = 0;
    puVar7 = *(undefined4**)(pNegative_output + 0x44);
    local_88 = 0;
    local_8c = 0;
    local_90 = 0;
    while (iVar5 < 0xc0 && local_90 < pSource[0xe]) {
        iVar1 = pSource[0x11];
        uVar2 = *(undefined4*)(iVar1 + iVar5);
        fVar11 = (*(float*)(iVar1 + 4 + iVar5) * pPlane[1]
                + *(float*)(iVar1 + 8 + iVar5) * pPlane[2]
                + *(float*)(iVar1 + iVar5) * *pPlane) - pPlane[3];
        afStack_80[local_90] = fVar11;
        if (fVar11 <= gCrushZero) {
            *puVar7 = uVar2;
            puVar8 = puVar7 + 3;
            puVar7[1] = *(undefined4*)(iVar5 + 4 + pSource[0x11]);
            puVar7[2] = *(undefined4*)(iVar5 + 8 + pSource[0x11]);
            local_8c = local_8c + 1;
            aiStack_40[local_90] = 2;
        } else {
            *puVar9 = uVar2;
            puVar9[1] = *(undefined4*)(iVar5 + 4 + pSource[0x11]);
            puVar9[2] = *(undefined4*)(iVar5 + 8 + pSource[0x11]);
            local_88 = local_88 + 1;
            aiStack_40[local_90] = 1;
            puVar8 = puVar7;
            puVar9 = puVar9 + 3;
        }
        local_90 = local_90 + 1;
        iVar5 = iVar5 + 0xc;
        puVar7 = puVar8;
    }

    local_90 = 0;
    if (0 < pSource[0xf]) {
        do {
            if ((0xf < local_88) || (0xf < local_8c)) {
                break;
            }
            uVar4 = (tU32)*(tU8*)(pSource[0x12] + local_90 * 2);
            uVar6 = (tU32)*(tU8*)(pSource[0x12] + local_90 * 2 + 1);
            if (aiStack_40[uVar4] != aiStack_40[uVar6]) {
                if (aiStack_40[uVar4] == 1) {
                    fVar11 = afStack_80[uVar6];
                    fVar10 = afStack_80[uVar4];
                    uVar3 = uVar4;
                    uVar4 = uVar6;
                } else {
                    fVar11 = afStack_80[uVar4];
                    fVar10 = afStack_80[uVar6];
                    uVar3 = uVar6;
                }
                ComputeSplitEdgeIntersections((br_vector3*)(*(float**)(pPositive_output + 0x44) + local_88 * 3),
                        (br_vector3*)(*(float**)(pNegative_output + 0x44) + local_8c * 3),
                        (br_vector3*)((float*)pSource[0x11] + uVar3 * 3),
                        (br_vector3*)((float*)pSource[0x11] + uVar4 * 3),
                        fVar10, fVar11);
                local_88 = local_88 + 1;
                local_8c = local_8c + 1;
            }
            local_90 = local_90 + 1;
        } while (local_90 < pSource[0xf]);
    }

    *(int*)(pPositive_output + 0x38) = local_88;
    *(int*)(pNegative_output + 0x38) = local_8c;
    if (local_88 < 4) {
        *(int*)(pPositive_output + 0x38) = 0;
        *(int*)(pPositive_output + 0x3c) = 0;
        *(int*)(pPositive_output + 0x40) = 0;
    } else {
        FillInShape((tPhysics_shape*)pPositive_output);
    }
    if (local_8c < 4) {
        *(int*)(pNegative_output + 0x38) = 0;
        *(int*)(pNegative_output + 0x3c) = 0;
        *(int*)(pNegative_output + 0x40) = 0;
        return;
    }
    FillInShape((tPhysics_shape*)pNegative_output);
}

// FUNCTION: CARMA2_HW 0x0042da20
void C2_HOOK_FASTCALL FUN_0042da20(undefined4* pRecipient) {
    int i;
    tCar_spec** pActiveCar;

    i = 0;
    if (0 < gNum_active_cars) {
        pActiveCar = gActive_car_list;
        do {
            if (*pActiveCar != NULL && (*pActiveCar)->driver > 5 && (*pActiveCar)->car_crush_spec != NULL) {
                tCar_crush_spec* car_crush;
                int j;

                car_crush = (*pActiveCar)->car_crush_spec;
                j = 0;
                if (0 < car_crush->field_0x270) {
                    do {
                        SendSemiDetachBit(*pActiveCar, car_crush->field_0x274[j].field_0x0, 0.f, pRecipient);
                        j = j + 1;
                    } while (j < car_crush->field_0x270);
                }
            }
            i = i + 1;
            pActiveCar = pActiveCar + 1;
        } while (i < gNum_active_cars);
    }
}

// FUNCTION: CARMA2_HW 0x00433b90
void C2_HOOK_FASTCALL FUN_00433b90(br_model* pModel, const br_vector3* pOffset) {
    volatile br_scalar center_x;
    volatile br_scalar center_y;
    volatile br_scalar center_z;
    volatile br_vector3 offset;
    volatile br_scalar delta_x;
    volatile br_scalar delta_y;
    volatile br_scalar delta_z;
    tU16 i;

    center_x = (pModel->bounds.max.v[0] + pModel->bounds.min.v[0]) * gFLOAT_00589a2c;
    center_y = (pModel->bounds.max.v[1] + pModel->bounds.min.v[1]) * gFLOAT_00589a2c;
    center_z = (pModel->bounds.max.v[2] + pModel->bounds.min.v[2]) * gFLOAT_00589a2c;
    offset.v[0] = pOffset->v[0];
    offset.v[1] = pOffset->v[1];
    offset.v[2] = pOffset->v[2];
    delta_x = offset.v[0] - center_x;
    delta_y = offset.v[1] - center_y;
    delta_z = offset.v[2] - center_z;
    for (i = 0; i < pModel->nvertices; i++) {
        pModel->vertices[i].p.v[0] = delta_x + pModel->vertices[i].p.v[0];
        pModel->vertices[i].p.v[1] = delta_y + pModel->vertices[i].p.v[1];
        pModel->vertices[i].p.v[2] = delta_z + pModel->vertices[i].p.v[2];
    }
    if ((pModel->flags & BR_MODF_CUSTOM) != 0 && pModel->user != NULL) {
        *(int*)pModel->user = 1;
        return;
    }
    pModel->custom = JitModelUpdate;
    pModel->flags |= BR_MODF_CUSTOM;
}

// C2PORT-NONMATCHING: best 37.21% | difftest PASS 2000 trials (spec difftest/specs/00437480_FUN_00437480.json) | reason x87-order and register allocation
// FUNCTION: CARMA2_HW 0x00437480
void C2_HOOK_FASTCALL FUN_00437480(undefined4* param_1, int param_2, int param_3, int param_4, undefined4 param_5, undefined4* param_6) {
    tU8 bVar1;
    tU8 bVar2;
    float fVar3;
    float fVar4;
    float fVar5;
    undefined4* puVar6;
    undefined4 uVar7;
    int iVar8;
    tU32 uVar9;
    tU32 uVar10;
    br_vector3 local_0;

    param_1[0] = 3;
    if (param_6 == NULL) {
        if (param_3 != 0) {
            param_1[2] = *(undefined4*)&(*(br_vertex**)(param_4 + 8))[*(tU16*)((char*)param_3 + 0xa)].p.v[0];
            param_1[3] = *(undefined4*)&(*(br_vertex**)(param_4 + 8))[*(tU16*)((char*)param_3 + 0xa)].p.v[1];
            param_1[4] = *(undefined4*)&(*(br_vertex**)(param_4 + 8))[*(tU16*)((char*)param_3 + 0xa)].p.v[2];
            goto LAB_0043752a;
        }
        param_1[2] = *(undefined4*)&(*(br_vertex**)(param_4 + 8))[*(tU16*)((char*)param_2 + 0x30)].p.v[0];
        param_1[3] = *(undefined4*)&(*(br_vertex**)(param_4 + 8))[*(tU16*)((char*)param_2 + 0x30)].p.v[1];
        uVar7 = *(undefined4*)&(*(br_vertex**)(param_4 + 8))[*(tU16*)((char*)param_2 + 0x30)].p.v[2];
    } else {
        param_1[2] = *(undefined4*)param_6;
        param_1[3] = *((undefined4*)param_6 + 1);
        uVar7 = *((undefined4*)param_6 + 2);
    }
    param_1[4] = uVar7;
LAB_0043752a:
    BrMatrix34ApplyP(&local_0, (br_vector3*)&param_1[2], &((tPhysics_object*)param_5)->actor->t.t.mat);
    DrMatrix34ApplyLPInverse((br_vector3*)&param_1[5], &local_0, &((tPhysics_object*)param_5)->parent->actor->t.t.mat);
    param_1[0x14] = 2;
    iVar8 = 0;
    puVar6 = param_1 + 0x16;
    do {
        puVar6[-1] = 0;
        *puVar6 = 0;
        iVar8 = iVar8 + 1;
        puVar6 = puVar6 + 8;
    } while (iVar8 < (int)param_1[0x14]);
    if (param_3 != 0) {
        ((float*)param_1)[0x17] = (*(br_vertex**)(param_4 + 8))[*(tU16*)((char*)param_3 + 0xe)].p.v[0]
                - (*(br_vertex**)(param_4 + 8))[*(tU16*)((char*)param_3 + 0xa)].p.v[0];
        ((float*)param_1)[0x18] = (*(br_vertex**)(param_4 + 8))[*(tU16*)((char*)param_3 + 0xe)].p.v[1]
                - (*(br_vertex**)(param_4 + 8))[*(tU16*)((char*)param_3 + 0xa)].p.v[1];
        fVar3 = (*(br_vertex**)(param_4 + 8))[*(tU16*)((char*)param_3 + 0xe)].p.v[2]
                - (*(br_vertex**)(param_4 + 8))[*(tU16*)((char*)param_3 + 0xa)].p.v[2];
        ((float*)param_1)[0x19] = fVar3;
        fVar3 = sqrtf(fVar3 * fVar3
                + ((float*)param_1)[0x18] * ((float*)param_1)[0x18]
                + ((float*)param_1)[0x17] * ((float*)param_1)[0x17]);
        if (fVar3 <= gFLOAT_00589a5c) {
            ((float*)param_1)[0x17] = 1.0f;
            ((float*)param_1)[0x18] = 0.0f;
            ((float*)param_1)[0x19] = 0.0f;
        } else {
            fVar3 = (float)gDOUBLE_00589a60 / fVar3;
            ((float*)param_1)[0x17] = ((float*)param_1)[0x17] * fVar3;
            ((float*)param_1)[0x18] = ((float*)param_1)[0x18] * fVar3;
            ((float*)param_1)[0x19] = ((float*)param_1)[0x19] * fVar3;
        }
        ((float*)param_1)[0x1f] = (*(br_vertex**)(param_4 + 8))[*(tU16*)((char*)param_3 + 0x10)].p.v[0]
                - (*(br_vertex**)(param_4 + 8))[*(tU16*)((char*)param_3 + 0xa)].p.v[0];
        ((float*)param_1)[0x20] = (*(br_vertex**)(param_4 + 8))[*(tU16*)((char*)param_3 + 0x10)].p.v[1]
                - (*(br_vertex**)(param_4 + 8))[*(tU16*)((char*)param_3 + 0xa)].p.v[1];
        fVar3 = (*(br_vertex**)(param_4 + 8))[*(tU16*)((char*)param_3 + 0x10)].p.v[2]
                - (*(br_vertex**)(param_4 + 8))[*(tU16*)((char*)param_3 + 0xa)].p.v[2];
        ((float*)param_1)[0x21] = fVar3;
        fVar3 = sqrtf(fVar3 * fVar3
                + ((float*)param_1)[0x20] * ((float*)param_1)[0x20]
                + ((float*)param_1)[0x1f] * ((float*)param_1)[0x1f]);
        if (fVar3 <= gFLOAT_00589a5c) {
            ((float*)param_1)[0x1f] = 1.0f;
            ((float*)param_1)[0x20] = 0.0f;
            ((float*)param_1)[0x21] = 0.0f;
        } else {
            fVar3 = (float)gDOUBLE_00589a60 / fVar3;
            ((float*)param_1)[0x1f] = ((float*)param_1)[0x1f] * fVar3;
            ((float*)param_1)[0x20] = ((float*)param_1)[0x20] * fVar3;
            ((float*)param_1)[0x21] = ((float*)param_1)[0x21] * fVar3;
        }
        ((float*)param_1)[0x1a] = ((float*)param_1)[0x21] * ((float*)param_1)[0x18]
                - ((float*)param_1)[0x20] * ((float*)param_1)[0x19];
        fVar3 = ((float*)param_1)[0x19] * ((float*)param_1)[0x1f]
                - ((float*)param_1)[0x17] * ((float*)param_1)[0x21];
        ((float*)param_1)[0x1b] = fVar3;
        fVar4 = ((float*)param_1)[0x17] * gFLOAT_00589a58;
        ((float*)param_1)[0x1c] = ((float*)param_1)[0x19] * gFLOAT_00589a58
                + (((float*)param_1)[0x17] * ((float*)param_1)[0x20] - ((float*)param_1)[0x18] * ((float*)param_1)[0x1f]);
        fVar4 = ((float*)param_1)[0x1f] * gFLOAT_00589a58 + fVar4 + ((float*)param_1)[0x1a];
        fVar3 = ((float*)param_1)[0x20] * gFLOAT_00589a58
                + ((float*)param_1)[0x18] * gFLOAT_00589a58 + fVar3;
        fVar5 = ((float*)param_1)[0x21] * gFLOAT_00589a58 + ((float*)param_1)[0x1c];
        ((float*)param_1)[0x1a] = fVar4;
        ((float*)param_1)[0x1b] = fVar3;
        ((float*)param_1)[0x1c] = fVar5;
        fVar5 = sqrtf(fVar5 * fVar5 + fVar3 * fVar3 + fVar4 * fVar4);
        if (fVar5 <= gFLOAT_00589a5c) {
            ((float*)param_1)[0x1a] = 1.0f;
            ((float*)param_1)[0x1b] = 0.0f;
            ((float*)param_1)[0x1c] = 0.0f;
        } else {
            fVar5 = (float)gDOUBLE_00589a60 / fVar5;
            ((float*)param_1)[0x1a] = fVar5 * fVar4;
            ((float*)param_1)[0x1b] = fVar5 * fVar3;
            ((float*)param_1)[0x1c] = ((float*)param_1)[0x1c] * fVar5;
        }
        ((float*)param_1)[0x22] = ((float*)param_1)[0x1a];
        ((float*)param_1)[0x23] = ((float*)param_1)[0x1b];
        ((float*)param_1)[0x24] = ((float*)param_1)[0x1c];
        return;
    }
    bVar1 = *((tU8*)param_2 + 0x33);
    bVar2 = *((tU8*)param_2 + 0x32);
    iVar8 = (int)(tU32)bVar1 >> 1;
    ((float*)param_1)[0x1a] = 0.0f;
    ((float*)param_1)[0x1b] = 0.0f;
    ((float*)param_1)[0x1c] = 0.0f;
    uVar7 = *(undefined4*)&gFLOAT_00589b2c;
    if ((bVar1 & 1) != 0) {
        uVar7 = *(undefined4*)&gFLOAT_00589a54;
    }
    param_1[iVar8 + 0x1a] = uVar7;
    param_1[0x24] = param_1[0x1c];
    param_1[0x23] = param_1[0x1b];
    param_1[0x22] = param_1[0x1a];
    uVar9 = (iVar8 + 1U) % 3;
    uVar7 = *(undefined4*)&gFLOAT_00589a54;
    if (((int)(tU32)bVar2 >> (char)uVar9 & 1U) != 0) {
        uVar7 = *(undefined4*)&gFLOAT_00589b2c;
    }
    param_1[uVar9 + 0x17] = uVar7;
    uVar10 = (iVar8 + 2U) % 3;
    param_1[uVar10 + 0x17] = 0;
    fVar3 = gFLOAT_00589b30;
    if ((bVar1 & 1) != 0) {
        fVar3 = gFLOAT_00589a58;
    }
    ((float*)param_1)[iVar8 + 0x17] = fVar3;
    fVar3 = sqrtf(((float*)param_1)[0x19] * ((float*)param_1)[0x19]
            + ((float*)param_1)[0x18] * ((float*)param_1)[0x18]
            + ((float*)param_1)[0x17] * ((float*)param_1)[0x17]);
    if (fVar3 <= gFLOAT_00589a5c) {
        ((float*)param_1)[0x17] = 1.0f;
        ((float*)param_1)[0x18] = 0.0f;
        ((float*)param_1)[0x19] = 0.0f;
    } else {
        fVar3 = (float)gDOUBLE_00589a60 / fVar3;
        ((float*)param_1)[0x17] = ((float*)param_1)[0x17] * fVar3;
        ((float*)param_1)[0x18] = ((float*)param_1)[0x18] * fVar3;
        ((float*)param_1)[0x19] = ((float*)param_1)[0x19] * fVar3;
    }
    param_1[uVar9 + 0x1f] = 0;
    uVar7 = *(undefined4*)&gFLOAT_00589a54;
    if (((int)(tU32)bVar2 >> (char)uVar10 & 1U) != 0) {
        uVar7 = *(undefined4*)&gFLOAT_00589b2c;
    }
    param_1[uVar10 + 0x1f] = uVar7;
    fVar3 = gFLOAT_00589b30;
    if ((bVar1 & 1) != 0) {
        fVar3 = gFLOAT_00589a58;
    }
    ((float*)param_1)[iVar8 + 0x1f] = fVar3;
    fVar3 = sqrtf(((float*)param_1)[0x21] * ((float*)param_1)[0x21]
            + ((float*)param_1)[0x20] * ((float*)param_1)[0x20]
            + ((float*)param_1)[0x1f] * ((float*)param_1)[0x1f]);
    if (fVar3 <= gFLOAT_00589a5c) {
        ((float*)param_1)[0x1f] = 1.0f;
        ((float*)param_1)[0x20] = 0.0f;
        ((float*)param_1)[0x21] = 0.0f;
        return;
    }
    fVar3 = (float)gDOUBLE_00589a60 / fVar3;
    ((float*)param_1)[0x1f] = ((float*)param_1)[0x1f] * fVar3;
    ((float*)param_1)[0x20] = ((float*)param_1)[0x20] * fVar3;
    ((float*)param_1)[0x21] = ((float*)param_1)[0x21] * fVar3;
}

// C2PORT-NONMATCHING: best 46.95% | difftest PASS 2000 trials (spec difftest/specs/00437ac0_SetUpFlapJoint.json) | reason x87-order
// FUNCTION: CARMA2_HW 0x00437ac0
void C2_HOOK_FASTCALL SetUpFlapJoint(tPhysics_joint* pJoint, tCar_crush_flap_data* pFlap_data, br_model* pModel, tPhysics_object* pObject, br_vector3* pPoint1, br_vector3* pPoint2, br_vector3* pPoint3) {
    float* pfVar1;
    float fVar2;
    float fVar3;
    int bVar4;
    float fVar5;
    float fVar6;
    float fVar7;
    float* pfVar8;
    float* joint;
    float* point_1;
    float* point_2;
    float* point_3;
    br_vector3 local_point;
    br_matrix34 rotation;

    joint = (float*)pJoint;
    point_1 = (float*)pPoint1;
    point_2 = (float*)pPoint2;
    point_3 = (float*)pPoint3;
    *(int *)joint = 4;
    if (point_1 == (float *)0x0) {
        point_1 = (float *)&pModel->vertices[pFlap_data->hinge0].p;
    }
    pfVar8 = point_2;
    if (point_2 == (float *)0x0) {
        pfVar8 = (float *)&pModel->vertices[pFlap_data->hinge1].p;
    }
    if (point_3 == (float *)0x0) {
        point_2 = (float *)&pModel->vertices[pFlap_data->hinge2].p;
    }
    else {
        point_2 = point_3;
    }
    joint[2] = *point_1;
    joint[3] = point_1[1];
    joint[4] = point_1[2];
    BrMatrix34ApplyP(&local_point, (br_vector3 *)&joint[2], &pObject->actor->t.t.mat);
    DrMatrix34ApplyLPInverse((br_vector3 *)&joint[5], &local_point, &pObject->parent->actor->t.t.mat);
    pfVar1 = (float *)(joint + 0xe);
    *pfVar1 = *pfVar8 - *point_1;
    joint[0xf] = pfVar8[1] - point_1[1];
    fVar2 = pfVar8[2] - point_1[2];
    joint[0x10] = fVar2;
    fVar2 = sqrtf(fVar2 * fVar2 + (float)joint[0xf] * (float)joint[0xf] + *pfVar1 * *pfVar1);
    if (gFLOAT_00589a5c < fVar2) {
        fVar2 = (float)gDOUBLE_00589a60 / fVar2;
        *pfVar1 = *pfVar1 * fVar2;
        joint[0xf] = (float)joint[0xf] * fVar2;
        joint[0x10] = (float)joint[0x10] * fVar2;
    } else {
        *pfVar1 = 1.0;
        joint[0xf] = 0;
        joint[0x10] = 0;
    }
    bVar4 = *pfVar1 == gCrushZero;
    joint[0x11] = *pfVar1;
    joint[0x12] = joint[0xf];
    joint[0x13] = joint[0x10];
    if ((bVar4) && ((float)joint[0x10] == gCrushZero)) {
        joint[8] = 1.0f;
        joint[9] = 0;
        joint[10] = 0;
    }
    else {
        joint[8] = -(float)joint[0x10];
        joint[10] = *pfVar1;
        joint[9] = 0;
    }
    fVar2 = sqrtf((float)joint[10] * (float)joint[10] + (float)joint[8] * (float)joint[8]);
    if (gFLOAT_00589a5c < fVar2) {
        fVar2 = (float)gDOUBLE_00589a60 / fVar2;
        joint[9] = 0;
        joint[8] = (float)joint[8] * fVar2;
        joint[10] = (float)joint[10] * fVar2;
    } else {
        joint[8] = 1.0f;
        joint[9] = 0;
        joint[10] = 0;
    }
    joint[0xb] = (float)joint[0xf] * (float)joint[10] - (float)joint[0x10] * (float)joint[9]
    ;
    joint[0xc] = (float)joint[8] * (float)joint[0x10] - *pfVar1 * (float)joint[10];
    fVar3 = *pfVar1 * (float)joint[9] - (float)joint[8] * (float)joint[0xf];
    joint[0xd] = fVar3;
    fVar2 = sqrtf(fVar3 * fVar3 +
                              (float)joint[0xc] * (float)joint[0xc] + (float)joint[0xb] * (float)joint[0xb]
                            );
    if (gFLOAT_00589a5c < fVar2) {
        fVar2 = (float)gDOUBLE_00589a60 / fVar2;
        joint[0xb] = (float)joint[0xb] * fVar2;
        joint[0xc] = (float)joint[0xc] * fVar2;
        joint[0xd] = fVar2 * fVar3;
    } else {
        joint[0xb] = 1.0f;
        joint[0xc] = 0;
        joint[0xd] = 0;
    }
    ((tPhysics_joint *)joint)->count_limits = 2;
    joint[0x17] = *point_2 - *point_1;
    joint[0x18] = point_2[1] - point_1[1];
    fVar2 = point_2[2];
    fVar3 = point_1[2];
    joint[0x19] = fVar2 - fVar3;
    fVar2 = (float)joint[0x17] -
                    *pfVar1 * (*pfVar1 * (float)joint[0x17] + (float)joint[0x10] * (fVar2 - fVar3) +
                                        (float)joint[0xf] * (float)joint[0x18]);
    fVar3 = (float)joint[0x18] -
                    ((float)joint[0x10] * (float)joint[0x19] + (float)joint[0xf] * (float)joint[0x18]
                    + (float)joint[0x17] * *pfVar1) * (float)joint[0xf];
    fVar6 = (float)joint[0x19] -
                    (float)joint[0x10] *
                    ((float)joint[0x18] * (float)joint[0xf] + (float)joint[0x10] * (float)joint[0x19]
                    + (float)joint[0x17] * *pfVar1);
    joint[0x17] = fVar2;
    fVar7 = gFLOAT_00589a5c;
    fVar5 = sqrtf(fVar6 * fVar6 + fVar3 * fVar3 + fVar2 * fVar2);
    joint[0x18] = fVar3;
    joint[0x19] = fVar6;
    if (fVar7 < fVar5) {
        fVar5 = (float)gDOUBLE_00589a60 / fVar5;
        joint[0x17] = fVar5 * fVar2;
        joint[0x18] = fVar5 * fVar3;
        joint[0x19] = (float)joint[0x19] * fVar5;
    } else {
        joint[0x17] = 1.0f;
        joint[0x18] = 0;
        joint[0x19] = 0;
    }
    joint[0x1a] =
              (float)joint[0xf] * (float)joint[0x19] - (float)joint[0x10] * (float)joint[0x18];
    pfVar8 = (float *)(joint + 0x1f);
    joint[0x1b] = (float)joint[0x10] * (float)joint[0x17] - *pfVar1 * (float)joint[0x19];
    *pfVar8 = (float)joint[0x17];
    fVar3 = *pfVar1 * (float)joint[0x18] - (float)joint[0x17] * (float)joint[0xf];
    joint[0x20] = joint[0x18];
    joint[0x21] = joint[0x19];
    joint[0x1c] = fVar3;
    fVar2 = sqrtf(fVar3 * fVar3 +
                              (float)joint[0x1b] * (float)joint[0x1b] +
                              (float)joint[0x1a] * (float)joint[0x1a]);
    if (gFLOAT_00589a5c < fVar2) {
        fVar2 = (float)gDOUBLE_00589a60 / fVar2;
        joint[0x1a] = (float)joint[0x1a] * fVar2;
        joint[0x1b] = (float)joint[0x1b] * fVar2;
        joint[0x1c] = fVar2 * fVar3;
    } else {
        joint[0x1a] = 1.0f;
        joint[0x1b] = 0;
        joint[0x1c] = 0;
    }
    BrMatrix34Rotate(&rotation, (br_angle)(pFlap_data->field_0x6 - 0x3ffc), (br_vector3 *)pfVar1);
    BrMatrix34ApplyV((br_vector3 *)&joint[0x22], (br_vector3 *)&joint[0x1f], &rotation);
    fVar7 = gFLOAT_00589a5c;
    fVar2 = (float)joint[0x1a] * gFLOAT_00589a58 + (float)joint[0x17];
    fVar6 = (float)joint[0x1b] * gFLOAT_00589a58 + (float)joint[0x18];
    fVar5 = (float)joint[0x1c] * gFLOAT_00589a58 + (float)joint[0x19];
    fVar3 = sqrtf(fVar5 * fVar5 + fVar6 * fVar6 + fVar2 * fVar2);
    joint[0x17] = fVar2;
    joint[0x18] = fVar6;
    joint[0x19] = fVar5;
    if (fVar7 < fVar3) {
        fVar3 = (float)gDOUBLE_00589a60 / fVar3;
        joint[0x17] = fVar3 * fVar2;
        joint[0x18] = fVar3 * fVar6;
        joint[0x19] = (float)joint[0x19] * fVar3;
    } else {
        joint[0x17] = 1.0f;
        joint[0x18] = 0;
        joint[0x19] = 0;
    }
    fVar2 = (float)joint[0x22] * gFLOAT_00589a58 + *pfVar8;
    fVar6 = (float)joint[0x23] * gFLOAT_00589a58 + (float)joint[0x20];
    fVar5 = (float)joint[0x24] * gFLOAT_00589a58 + (float)joint[0x21];
    *pfVar8 = fVar2;
    fVar7 = gFLOAT_00589a5c;
    fVar3 = sqrtf(fVar5 * fVar5 + fVar6 * fVar6 + fVar2 * fVar2);
    joint[0x20] = fVar6;
    joint[0x21] = fVar5;
    if (fVar7 < fVar3) {
        fVar3 = (float)gDOUBLE_00589a60 / fVar3;
        *pfVar8 = fVar3 * fVar2;
        joint[0x20] = fVar3 * fVar6;
        joint[0x21] = fVar3 * fVar5;
        return;
    }
    *pfVar8 = 1.0;
    joint[0x20] = 0;
    joint[0x21] = 0;
    return;
}

// FUNCTION: CARMA2_HW 0x0042d8f0
void C2_HOOK_FASTCALL AddToggledDoor(br_actor* pActor, tCar_spec* pCar, int pFlag, float pDelay) {
    int i;

    if (gCount_toggled_doors != 16) {
        for (i = 0; i < gCount_toggled_doors; i++) {
            if (gToggled_doors[i].actor == pActor) {
                return;
            }
        }
        gToggled_doors[gCount_toggled_doors].actor = pActor;
        gToggled_doors[gCount_toggled_doors].car = pCar;
        gToggled_doors[gCount_toggled_doors].field_0x8 = pFlag;
        gToggled_doors[gCount_toggled_doors].field_0xc = pDelay;
        gCount_toggled_doors = gCount_toggled_doors + 1;
    }
}
