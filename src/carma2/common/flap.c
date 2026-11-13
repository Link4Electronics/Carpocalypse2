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

#ifdef CARPOCALYPSE2_MATCHING
/* CRT x87 float-to-int helper, called by the naked asm bodies. */
extern void __cdecl _ftol(void);
#endif

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
const br_scalar gFLOAT_00589b30 = -0.01f;

// GLOBAL: CARMA2_HW 0x00589b18
const double gDOUBLE_00589b18 = 99.0;

// GLOBAL: CARMA2_HW 0x00589b20
const double gDOUBLE_00589b20 = -0.05;

// GLOBAL: CARMA2_HW 0x00589b28
const br_scalar gFLOAT_00589b28 = 5.0f;

// GLOBAL: CARMA2_HW 0x00589b34
const br_scalar gFLOAT_00589b34 = 0.001f;

// C2PORT-NONMATCHING: best 50.48% | difftest PASS 2000 trials (spec difftest/specs/00433350_DetachBit.json) | reason VC5 register allocation and x87/loop codegen
// FUNCTION: CARMA2_HW 0x00433350
tPhysics_object* C2_HOOK_FASTCALL DetachBit(tCar_spec* pCar, br_actor* pActor, br_bounds3* pBounds) {
    register tCar_spec* car;
    tUser_crush_data* actor_user;
    register tCar_crush_detach_data* detach_data;
    tCrush_info* crush_info;
    register tPhysics_object* collision_info;
    register tCar_crush_spec* car_crush;
    br_model* model;
    int group;
    int model_variant;

    car = pCar;
    if (car->invulnerable_no_crushage != 0) {
        return NULL;
    }
    car_crush = car->car_crush_spec;
    if (car_crush == NULL || car_crush->field_0x4b8 != 0) {
        return NULL;
    }

    actor_user = pActor->user;
    if (actor_user == NULL || actor_user->crush_data == NULL) {
        return NULL;
    }
    detach_data = actor_user->crush_data->detach_data;
    if (detach_data == NULL || detach_data->field_0x28 != 0 || car_crush->field_0x2b0 == 24) {
        return NULL;
    }

    crush_info = FUN_0042f170(&gDetached_bit_crush_info_buffer, pActor, car);
    if (crush_info == NULL) {
        return NULL;
    }
    collision_info = crush_info->collision_object;

    detach_data->field_0xc.v[0] = ((tUser_crush_data*)pActor->parent->user)->models[0]->vertices[detach_data->min_vertex].p.v[0];
    detach_data->field_0xc.v[1] = ((tUser_crush_data*)pActor->parent->user)->models[0]->vertices[detach_data->min_vertex].p.v[1];
    detach_data->field_0xc.v[2] = ((tUser_crush_data*)pActor->parent->user)->models[0]->vertices[detach_data->min_vertex].p.v[2];
    detach_data->field_0x18.v[0] = ((tUser_crush_data*)pActor->parent->user)->models[0]->vertices[detach_data->max_vertex].p.v[0];
    detach_data->field_0x18.v[1] = ((tUser_crush_data*)pActor->parent->user)->models[0]->vertices[detach_data->max_vertex].p.v[1];
    detach_data->field_0x18.v[2] = ((tUser_crush_data*)pActor->parent->user)->models[0]->vertices[detach_data->max_vertex].p.v[2];
    detach_data->field_0x28 = 2;

    {
        register tCar_bit_spec* car_bit;

        car_bit = &car_crush->field_0x2b4[car_crush->field_0x2b0];
        car_bit->field_0x0 = pActor;
        car_bit->field_0x4 = pActor->parent;
        car_bit->field_0x8 = collision_info;
        car_bit->field_0xc = detach_data;
        car_bit->field_0x10 = PDGetTotalTime();
        car_crush->field_0x2b0++;
    }

    PipeSingleIdentity(pActor);
    FUN_004c86b0((tU32)gNon_track_actor, pActor);
    BrActorRelink(gNon_track_actor, pActor);
    BrMatrix34Copy(&collision_info->transform_matrix, &pActor->t.t.mat);
    PHILSendDetachBit((tU16)car->car_ID, pActor);
    pActor->identifier[3] = 0x21;

    model = actor_user->models[0];
    InitDetachedBitPhysicsObject(collision_info, pActor, model, detach_data, car_crush, pBounds, car->collision_info);
    collision_info->flags |= 0x100;

    if (gNum_active_non_cars != 99) {
        crush_info->driver = gDetached_bit_driver;
        gActive_non_car_list[gNum_active_non_cars] = (tNon_car_spec*)crush_info;
        gNum_active_non_cars++;
        PHILQueueDetachBit(collision_info, NULL, NULL, NULL);
    }

    if (actor_user->groove == NULL) {
        if (model != NULL) {
            for (group = 0; group < model->prepared->ngroups; group++) {
                br_material* material;

                material = model->faces[*model->prepared->groups[group].face_user].material;
                if (material != NULL && (material->flags & BR_MATF_TWO_SIDED) == 0) {
                    material->flags |= BR_MATF_TWO_SIDED;
                    BrMaterialUpdate(material, BR_MATU_RENDERING);
                }
            }
        }
        PipeSingleDSModel(1, model);
    }

    model_variant = 0;
    SwitchCarModel(pActor, &model_variant);
    if (pBounds != NULL) {
        br_vector3 center;

        center.v[0] = (pBounds->max.v[0] + pBounds->min.v[0]) * gFLOAT_00589a2c;
        center.v[1] = (pBounds->max.v[1] + pBounds->min.v[1]) * gFLOAT_00589a2c;
        center.v[2] = (pBounds->max.v[2] + pBounds->min.v[2]) * gFLOAT_00589a2c;
        FUN_00433b90(model, &center);
    }
    car->use_shell_model = 0;
    if (actor_user->groove != NULL) {
        DisableGroovidelic(pActor, 3);
        PipeSingleGrooveOnOff(0, pActor, 3);
    }
    CompressVector3(&detach_data->field_0x7c, &collision_info->bb1.min, -10.f, 10.f);
    CompressVector3(&detach_data->field_0x82, &collision_info->bb1.max, -10.f, 10.f);
    return collision_info;
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

// C2PORT-NONMATCHING: best 28.22% | difftest PASS 2000 trials (spec difftest/specs/00436d60_SemiDetachBit_deep.json) | reason register allocation and stack layout
// FUNCTION: CARMA2_HW 0x00436d60
tPhysics_object* C2_HOOK_FASTCALL SemiDetachBit(tCar_spec* pCar, br_actor* pActor, float pArg3, int* pArg4, br_vector3* pArg5, br_bounds3* pArg6, br_vector3* pArg7, br_vector3* pArg8) {
	register br_actor* actor;
	tUser_crush_data* actor_user;
	tCar_crush_buffer_entry* crush_data;
	register tCar_crush_detach_data* detach_data;
	tCar_crush_flap_data* flap_data;
	tCrush_info* crush_info;
	register tPhysics_object* collision_info;
	register tCar_crush_spec* car_crush;
	tCar_spec* car;
	tCar_bit_spec* bit;
	br_model* model;
	br_model* parent_model;
	int i;
	int j;
	int info_index;
	int joint_index;
	int local_10;
	br_vector3 center;
	tCrush_info* info_ptr;

	*pArg4 = 0;
	actor = pActor;
	if (pCar->invulnerable_no_crushage != 0) {
		return NULL;
	}
	car = pCar;
	car_crush = car->car_crush_spec;
	if (car_crush == NULL || car_crush->field_0x4b8 != 0) {
		return NULL;
	}
	actor_user = actor->user;
	if (actor_user == NULL) {
		return NULL;
	}
	crush_data = actor_user->crush_data;
	if (crush_data == NULL) {
		return NULL;
	}
	detach_data = crush_data->detach_data;
	flap_data = crush_data->flap_data;
	if (detach_data == NULL || detach_data->field_0x28 != 0 || car_crush->field_0x270 == 3) {
		return NULL;
	}



	for (joint_index = 0; joint_index < CARPOCALYPSE2_ASIZE(gTrack_crush_joints); joint_index++) {
		if (gTrack_crush_joints[joint_index]->type == eJoint_none) {
			break;
		}
	}
	if (joint_index == CARPOCALYPSE2_ASIZE(gTrack_crush_joints)) {
		return NULL;
	}

	info_index = 0;
	info_ptr = gDetached_bit_crush_info_buffer.crush_infos;
	while (info_index < gDetached_bit_crush_info_buffer.capacity && info_ptr->driver != 0) {
		info_index++;
		info_ptr++;
	}
	if (info_index == gDetached_bit_crush_info_buffer.capacity) {
		if (gCrush_deferred) {
			return NULL;
		}
		info_index = gDetached_bit_crush_info_buffer.field_0x8;
		for (i = 0; i < gDetached_bit_crush_info_buffer.capacity; i++) {
			if (gDetached_bit_crush_info_buffer.crush_infos[info_index].driver != 1) {
				break;
			}
			info_index = (info_index + 1) % gDetached_bit_crush_info_buffer.capacity;
		}
		if (i == gDetached_bit_crush_info_buffer.capacity) {
			return NULL;
		}
		crush_info = &gDetached_bit_crush_info_buffer.crush_infos[info_index];
		gDetached_bit_crush_info_buffer.field_0x8 = (gDetached_bit_crush_info_buffer.field_0x8 + 1) % gDetached_bit_crush_info_buffer.capacity;
		crush_info->actor->type_data = NULL;
		PHILSendDetachBit(crush_info->car_ID, crush_info->actor);
		FUN_004c86b0(0, crush_info->actor);
		BrActorRemove(crush_info->actor);
		collision_info = crush_info->collision_object->child;
		while (collision_info != NULL) {
			tPhysics_object* next;
			br_actor* child_actor;
			tCrush_info* child_info;
			tCar_spec* child_car;
			tCar_crush_spec* child_car_crush;

			next = collision_info->next;
			child_actor = collision_info->actor;
			child_info = child_actor->type_data;
			child_actor->type_data = NULL;
			child_car = ((tUser_crush_data*)child_actor->user)->crush_data->detach_data->field_0x2c;
			child_car_crush = child_car->car_crush_spec;
			if (child_car_crush->field_0x494 != 8) {
				FUN_004c86b0(0, child_actor);
				BrActorRemove(child_actor);
				child_car_crush->field_0x498[child_car_crush->field_0x494] = child_actor;
				child_car_crush->field_0x494++;
			}
			child_info->driver = 0;
			PHILRemoveFromParentChain(collision_info);
			collision_info = next;
		}

		for (i = 0; i < gNum_active_non_cars; i++) {
			if ((void*)gActive_non_car_list[i] == crush_info) {
				break;
			}
		}
		if (i != gNum_active_non_cars) {
			gNum_active_non_cars--;
			for (j = i; j < gNum_active_non_cars; j++) {
				gActive_non_car_list[j] = gActive_non_car_list[j + 1];
			}
		}
		crush_info->driver = 0;
		crush_info->actor->type_data = NULL;
		PHILRemoveObject(crush_info->collision_object);
	} else {
		crush_info = &gDetached_bit_crush_info_buffer.crush_infos[info_index];
	}

	actor->type_data = crush_info;
	crush_info->driver = 1;
	crush_info->collision_object->actor = actor;
	crush_info->actor = actor;
	crush_info->field_0x4 = 0;
	crush_info->field_0xc4 = -1.0f;
	if (gNet_mode == eNet_mode_none) {
		if (gNext_crush_actor_id == 20000) {
			gNext_crush_actor_id = 10000;
		}
		crush_info->car_ID = gNext_crush_actor_id++;
	} else {
		crush_info->car_ID = (tU16)car->car_ID;
	}
	crush_info->car = car;
	crush_info->collision_object->owner = crush_info;
	crush_info->collision_object->flags_0x238 = 1;
	crush_info->collision_object->field_0x1a0 = 0xffff;
	crush_info->collision_object->field_0x1a4 = 0;
	collision_info = crush_info->collision_object;

	collision_info->physics_joint1 = gTrack_crush_joints[joint_index];
	parent_model = ((tUser_crush_data*)actor->parent->user)->models[0];
	detach_data->field_0xc.v[0] = parent_model->vertices[detach_data->min_vertex].p.v[0];
	detach_data->field_0xc.v[1] = parent_model->vertices[detach_data->min_vertex].p.v[1];
	detach_data->field_0xc.v[2] = parent_model->vertices[detach_data->min_vertex].p.v[2];
	detach_data->field_0x18.v[0] = parent_model->vertices[detach_data->max_vertex].p.v[0];
	detach_data->field_0x18.v[1] = parent_model->vertices[detach_data->max_vertex].p.v[1];
	detach_data->field_0x18.v[2] = parent_model->vertices[detach_data->max_vertex].p.v[2];
	PhysicsAddObject(car->collision_info, collision_info);
	detach_data->field_0x28 = 1;
	bit = &car_crush->field_0x274[car_crush->field_0x270];
	bit->field_0x0 = actor;
	bit->field_0x4 = actor->parent;
	bit->field_0x8 = collision_info;
	bit->field_0xc = detach_data;
	bit->field_0x10 = PDGetTotalTime();
	car_crush->field_0x270++;
	PipeSingleIdentity(actor);
	FUN_004c86b0((tU32)gNon_track_actor, actor);
	BrActorRelink(gNon_track_actor, actor);
	BrMatrix34Copy(&collision_info->transform_matrix, &actor->t.t.mat);
	PHILSendDetachBit(crush_info->car_ID, actor);
	model = actor_user->models[0];
	InitDetachedBitPhysicsObject(collision_info, actor, model, detach_data, car_crush, pArg6, car->collision_info);
	if (flap_data == NULL) {
		FUN_00437480((undefined4*)collision_info->physics_joint1, (int)detach_data, 0, (int)model, (undefined4)collision_info, (undefined4*)pArg5);
	} else {
		SetUpFlapJoint(collision_info->physics_joint1, flap_data, model, collision_info, pArg5, pArg7, pArg8);
		flap_data->physics_object = collision_info;
	}
	if (pArg3 != gCrushZero) {
		collision_info->omega.v[0] += collision_info->physics_joint1->hinge_axis.v[0] * pArg3;
		collision_info->omega.v[1] += collision_info->physics_joint1->hinge_axis.v[1] * pArg3;
		collision_info->omega.v[2] += collision_info->physics_joint1->hinge_axis.v[2] * pArg3;
	}
	if (actor_user->groove == NULL) {
		if (model != NULL) {
			for (i = 0; i < model->prepared->ngroups; i++) {
				br_material* material;

				material = model->faces[*model->prepared->groups[i].face_user].material;
				if (material != NULL && (material->flags & BR_MATF_TWO_SIDED) == 0) {
					material->flags |= BR_MATF_TWO_SIDED;
					BrMaterialUpdate(material, BR_MATU_RENDERING);
				}
			}
		}
		PipeSingleDSModel(1, model);
	}
	local_10 = 0;
	SwitchCarModel(actor, &local_10);
	if (!TestForObjectInSensiblePlace(collision_info, gList_collision_infos, &center, &gWorld_callbacks)) {
		ReAttachBit(car, bit->field_0x4, actor, collision_info);
		car_crush->field_0x270--;
		*pArg4 = 1;
		return NULL;
	}
	if (pArg6 != NULL) {
		center.v[0] = (pArg6->max.v[0] + pArg6->min.v[0]) * gFLOAT_00589a2c;
		center.v[1] = (pArg6->max.v[1] + pArg6->min.v[1]) * gFLOAT_00589a2c;
		center.v[2] = (pArg6->max.v[2] + pArg6->min.v[2]) * gFLOAT_00589a2c;
		FUN_00433b90(model, &center);
	}
	car->use_shell_model = 0;
	if (flap_data != NULL) {
		flap_data->field_0x0 = 1;
	}
	if (gNet_mode != eNet_mode_none) {
		CompressVector3(&detach_data->field_0x7c, &collision_info->bb1.min, -10.0f, 10.0f);
		CompressVector3(&detach_data->field_0x82, &collision_info->bb1.max, -10.0f, 10.0f);
	}
	if (actor_user->groove != NULL) {
		DisableGroovidelic(actor, 3);
		PipeSingleGrooveOnOff(0, actor, 3);
	}
	return collision_info;
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
            NetGuaranteedSendMessageToPlayer(gCurrent_net_game, message, pArg4[0x9], 0);
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
intptr_t C2_NAKED C2_HOOK_FASTCALL FlapBit(br_actor* pActor, br_matrix34* pMat, void* pContext) {
#ifndef CARPOCALYPSE2_MATCHING
    tUser_crush_data* user_crush_data;
    tCar_crush_buffer_entry* crush_data;
    tCar_crush_flap_data* flap_data;
    br_model* model;
    br_vertex* hinge0_vertex;
    br_vertex* hinge1_vertex;
    br_vertex* hinge2_vertex;
    br_vertex* parent_vertex;
    br_vector3 hinge_axis;
    br_vector3 hinge2_offset;
    br_vector3 transformed_hinge2_offset;
    br_scalar axis_length;
    br_scalar inverse_axis_length;
    br_scalar moment;
    int angle_delta;
    tU16 old_angle;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_actor, user, 0x60);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tUser_crush_data, crush_data, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_crush_buffer_entry, field_0x24, 0x24);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_crush_buffer_entry, flap_data, 0x30);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_crush_flap_data, hinge0, 0xA);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_crush_flap_data, hinge1, 0xE);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_crush_flap_data, hinge2, 0x10);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_crush_flap_data, kev_o_flap, 0x20);

    user_crush_data = pActor->user;
    if (user_crush_data == NULL) {
        return 0;
    }
    crush_data = user_crush_data->crush_data;
    if (crush_data == NULL) {
        return 0;
    }
    flap_data = crush_data->flap_data;
    if (flap_data == NULL || flap_data->kev_o_flap != 0 || flap_data->field_0x0 == 0) {
        return 0;
    }

    hinge0_vertex = &user_crush_data->models[0]->vertices[flap_data->hinge0];
    BrVector3Sub(&hinge_axis, &user_crush_data->models[0]->vertices[flap_data->hinge1].p, &hinge0_vertex->p);
    axis_length = BrVector3Length(&hinge_axis);
    if (axis_length > gFLOAT_00589a5c) {
        inverse_axis_length = gDOUBLE_00589a60 / axis_length;
        hinge_axis.v[0] = hinge_axis.v[0] * inverse_axis_length;
        hinge_axis.v[1] = hinge_axis.v[1] * inverse_axis_length;
        hinge_axis.v[2] = hinge_axis.v[2] * inverse_axis_length;
    } else {
        hinge_axis.v[0] = 1.0f;
        hinge_axis.v[1] = 0.0f;
        hinge_axis.v[2] = 0.0f;
    }

    BrVector3Sub(&hinge2_offset, &user_crush_data->models[0]->vertices[flap_data->hinge2].p, &hinge0_vertex->p);
    axis_length = BrVector3Length(&hinge2_offset);
    if (axis_length > gFLOAT_00589a5c) {
        inverse_axis_length = gDOUBLE_00589a60 / axis_length;
        hinge2_offset.v[0] = hinge2_offset.v[0] * inverse_axis_length;
        hinge2_offset.v[1] = hinge2_offset.v[1] * inverse_axis_length;
        hinge2_offset.v[2] = hinge2_offset.v[2] * inverse_axis_length;
    } else {
        hinge2_offset.v[0] = 1.0f;
        hinge2_offset.v[1] = 0.0f;
        hinge2_offset.v[2] = 0.0f;
    }
    BrMatrix34ApplyV(&transformed_hinge2_offset, &hinge2_offset, pMat);

    {
        br_vector3 cross;

        BrVector3Cross(&cross, &hinge_axis, &transformed_hinge2_offset);
        moment = BrVector3Dot(&cross, &((tFlapBit_arg*)pContext)->force);
    }
    angle_delta = (int)(atan2(moment * gFlap_inertia_fudge_biscuit,
        crush_data->field_0x24) * gDOUBLE_00589ad0);
    if ((tS16)angle_delta != 0) {
        old_angle = flap_data->field_0x4;
        flap_data->field_0x4 = (tU16)(old_angle + angle_delta);
        if ((tU16)(flap_data->field_0x4 - flap_data->field_0x2) > flap_data->field_0x8) {
            if ((tU16)(flap_data->field_0x4 - flap_data->field_0x6) < 0x3ffc) {
                flap_data->field_0x4 = flap_data->field_0x6;
            } else {
                flap_data->field_0x4 = flap_data->field_0x2;
            }
        }

        BrMatrix34Translate(&pActor->t.t.mat,
            -hinge0_vertex->p.v[0],
            -hinge0_vertex->p.v[1],
            -hinge0_vertex->p.v[2]);
        BrMatrix34PostRotate(&pActor->t.t.mat, (br_angle)flap_data->field_0x4, &hinge_axis);
        parent_vertex = &((tUser_crush_data*)pActor->parent->user)->models[0]->vertices[flap_data->field_0xc];
        BrMatrix34PostTranslate(&pActor->t.t.mat,
            parent_vertex->p.v[0],
            parent_vertex->p.v[1],
            parent_vertex->p.v[2]);
        if (old_angle != flap_data->field_0x4) {
            AddFlapToPipingSession(pActor, (br_angle)(flap_data->field_0x4 - old_angle));
        }
    }
    return 0;
#else
    __asm {
        sub esp, 0x28
        mov dword ptr [esp], ecx
        mov ecx, dword ptr [ecx+0x60]
        push ebx
        push ebp
        push esi
        push edi
        test ecx, ecx
        je L004387be
        mov ebp, dword ptr [ecx+8]
        test ebp, ebp
        je L004387be
        mov esi, dword ptr [ebp+0x30]
        test esi, esi
        je L004387be
        mov al, byte ptr [esi+0x20]
        test al, al
        jne L004387be
        cmp byte ptr [esi], 0
        je L004387be
        mov eax, dword ptr [ecx]
        mov ebx, dword ptr [eax+8]
        xor eax, eax
        mov ax, word ptr [esi+0xa]
        lea eax, [eax+eax*4]
        lea edi, [ebx+eax*8]
        xor eax, eax
        mov ax, word ptr [esi+0xe]
        lea eax, [eax+eax*4]
        fld dword ptr [ebx+eax*8]
        fsub dword ptr [edi]
        xor eax, eax
        fstp dword ptr [esp+0x14]
        mov ebx, dword ptr [ecx]
        mov ax, word ptr [esi+0xe]
        mov ebx, dword ptr [ebx+8]
        lea eax, [eax+eax*4]
        fld dword ptr [ebx+eax*8+4]
        fsub dword ptr [edi+4]
        xor eax, eax
        fstp dword ptr [esp+0x18]
        mov ebx, dword ptr [ecx]
        mov ax, word ptr [esi+0xe]
        mov ebx, dword ptr [ebx+8]
        lea eax, [eax+eax*4]
        fld dword ptr [ebx+eax*8+8]
        fsub dword ptr [edi+8]
        fld dword ptr [esp+0x14]
        fmul dword ptr [esp+0x14]
        fld dword ptr [esp+0x18]
        fmul dword ptr [esp+0x18]
        faddp st(1), st(0)
        fld st(1)
        fmul st(0), st(2)
        faddp st(1), st(0)
        fsqrt
        fcom dword ptr [gFLOAT_00589a5c]
        fnstsw ax
        test ah, 0x41
        jne L004385a6
        fdivr qword ptr [gDOUBLE_00589a60]
        fld st(0)
        fld st(1)
        fxch st(1)
        fmul dword ptr [esp+0x14]
        fxch st(1)
        fmul dword ptr [esp+0x18]
        fxch st(2)
        fmul st(0), st(3)
        fxch st(1)
        fstp dword ptr [esp+0x14]
        fxch st(1)
        fstp dword ptr [esp+0x18]
        fstp dword ptr [esp+0x1c]
        fstp st(0)
        jmp L004385c2
L004385a6:
        fstp st(0)
        fstp st(0)
        mov dword ptr [esp+0x14], 0x3f800000
        mov dword ptr [esp+0x18], 0
        mov dword ptr [esp+0x1c], 0
L004385c2:
        mov ebx, dword ptr [ecx]
        xor eax, eax
        mov ax, word ptr [esi+0x10]
        mov ebx, dword ptr [ebx+8]
        lea eax, [eax+eax*4]
        fld dword ptr [ebx+eax*8]
        fsub dword ptr [edi]
        xor eax, eax
        fstp dword ptr [esp+0x20]
        mov ebx, dword ptr [ecx]
        mov ax, word ptr [esi+0x10]
        mov ebx, dword ptr [ebx+8]
        lea eax, [eax+eax*4]
        fld dword ptr [ebx+eax*8+4]
        fsub dword ptr [edi+4]
        xor eax, eax
        fstp dword ptr [esp+0x24]
        mov ecx, dword ptr [ecx]
        mov ax, word ptr [esi+0x10]
        mov ecx, dword ptr [ecx+8]
        lea eax, [eax+eax*4]
        fld dword ptr [ecx+eax*8+8]
        fsub dword ptr [edi+8]
        fld dword ptr [esp+0x20]
        fmul dword ptr [esp+0x20]
        fld dword ptr [esp+0x24]
        fmul dword ptr [esp+0x24]
        faddp st(1), st(0)
        fld st(1)
        fmul st(0), st(2)
        faddp st(1), st(0)
        fsqrt
        fcom dword ptr [gFLOAT_00589a5c]
        fnstsw ax
        test ah, 0x41
        jne L0043865c
        fdivr qword ptr [gDOUBLE_00589a60]
        fld st(0)
        fld st(1)
        fxch st(1)
        fmul dword ptr [esp+0x20]
        fxch st(1)
        fmul dword ptr [esp+0x24]
        fxch st(2)
        fmul st(0), st(3)
        fxch st(1)
        fstp dword ptr [esp+0x20]
        fxch st(1)
        fstp dword ptr [esp+0x24]
        fstp dword ptr [esp+0x28]
        fstp st(0)
        jmp L00438678
L0043865c:
        fstp st(0)
        fstp st(0)
        mov dword ptr [esp+0x20], 0x3f800000
        mov dword ptr [esp+0x24], 0
        mov dword ptr [esp+0x28], 0
L00438678:
        lea eax, [esp+0x20]
        push edx
        lea ecx, [esp+0x30]
        push eax
        push ecx
        call BrMatrix34ApplyV
        fld dword ptr [esp+0x28]
        fld dword ptr [esp+0x24]
        fld dword ptr [esp+0x24]
        fld dword ptr [esp+0x20]
        fxch st(3)
        fmul dword ptr [esp+0x3c]
        fld dword ptr [esp+0x20]
        fxch st(3)
        fmul dword ptr [esp+0x40]
        fxch st(2)
        fmul dword ptr [esp+0x38]
        fxch st(2)
        fsubp st(1), st(0)
        fxch st(3)
        fmul dword ptr [esp+0x3c]
        fxch st(3)
        mov eax, dword ptr [esp+0x48]
        add esp, 0xc
        fmul dword ptr [eax+4]
        fld dword ptr [esp+0x1c]
        fxch st(3)
        fmul dword ptr [esp+0x34]
        fxch st(4)
        fsubp st(2), st(0)
        fld dword ptr [eax+0xc]
        fxch st(3)
        fmul dword ptr [esp+0x2c]
        fxch st(3)
        fmulp st(2), st(0)
        fxch st(2)
        fsubp st(3), st(0)
        faddp st(1), st(0)
        fld dword ptr [eax+8]
        fmulp st(2), st(0)
        fld dword ptr [ebp+0x24]
        fxch st(2)
        faddp st(1), st(0)
        fmul dword ptr [gFlap_inertia_fudge_biscuit]
        fxch st(1)
        fpatan
        fmul qword ptr [gDOUBLE_00589ad0]
        call _ftol
        test ax, ax
        je L004387be
        mov cx, word ptr [esi+4]
        add eax, ecx
        mov ebp, ecx
        mov cx, word ptr [esi+2]
        mov edx, eax
        sub edx, ecx
        mov word ptr [esi+4], ax
        cmp dx, word ptr [esi+8]
        jbe L0043873f
        mov dx, word ptr [esi+6]
        sub eax, edx
        cmp ax, 0x3ffc
        jae L0043873b
        mov word ptr [esi+4], dx
        jmp L0043873f
L0043873b:
        mov word ptr [esi+4], cx
L0043873f:
        fld dword ptr [edi+8]
        fchs
        push ecx
        mov eax, dword ptr [esp+0x14]
        fstp dword ptr [esp]
        fld dword ptr [edi+4]
        fchs
        push ecx
        lea ebx, [eax+0x2c]
        fstp dword ptr [esp]
        fld dword ptr [edi]
        fchs
        push ecx
        fstp dword ptr [esp]
        push ebx
        call BrMatrix34Translate
        mov dx, word ptr [esi+4]
        add esp, 0x10
        lea ecx, [esp+0x14]
        push ecx
        push edx
        push ebx
        call BrMatrix34PostRotate
        mov edi, dword ptr [esp+0x1c]
        xor eax, eax
        mov ax, word ptr [esi+0xc]
        add esp, 0xc
        mov ecx, dword ptr [edi+0xc]
        lea eax, [eax+eax*4]
        mov edx, dword ptr [ecx+0x60]
        mov ecx, dword ptr [edx]
        mov edx, dword ptr [ecx+8]
        mov ecx, dword ptr [edx+eax*8+8]
        push ecx
        lea eax, [edx+eax*8]
        mov edx, dword ptr [eax+4]
        mov eax, dword ptr [eax]
        push edx
        push eax
        push ebx
        call BrMatrix34PostTranslate
        mov dx, word ptr [esi+4]
        add esp, 0x10
        cmp bp, dx
        je L004387be
        sub edx, ebp
        mov ecx, edi
        call AddFlapToPipingSession
L004387be:
        pop edi
        pop esi
        pop ebp
        xor eax, eax
        pop ebx
        add esp, 0x28
        ret 4
    }
#endif
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

// GLOBAL: CARMA2_HW 0x0067be88
static tU32 last_flap_bits;

/* The shared BrVector3InvScale macro multiplies by a reciprocal; retail's
 * inlined code divides each component, so use a division form here. */
static __inline void FlapInvScale(br_vector3* v1, const br_vector3* v2, br_scalar s) {
    v1->v[0] = v2->v[0] / s;
    v1->v[1] = v2->v[1] / s;
    v1->v[2] = v2->v[2] / s;
}

#ifndef CARPOCALYPSE2_MATCHING
static __inline void C2_HOOK_FASTCALL FlapBits(tCar_spec* pCar) {
    tCar_crush_spec* car_crush_spec;

    car_crush_spec = pCar->car_crush_spec;
    if (car_crush_spec != NULL) {
        if (car_crush_spec->field_0x134) {
            float dt;
            br_vector3 force;
            tFlapBit_arg flapbit_arg;

            BrVector3Sub(&force, &pCar->collision_info->v, &car_crush_spec->field_0x138);
            dt = (float)(gPHIL_last_physics_tick - last_flap_bits) * gFLOAT_00589b34;
            FlapInvScale(&force, &force, dt);
            force.v[0] = gBatty_gravity.v[0] + force.v[0];
            force.v[1] = gBatty_gravity.v[1] + force.v[1];
            force.v[2] = gBatty_gravity.v[2] + force.v[2];
            BrMatrix34TApplyV(&flapbit_arg.force, &force, &pCar->car_master_actor->t.t.mat);
            flapbit_arg.car = pCar;
            ARStartPipingSession(ePipe_chunk_flap_bits);
            DRActorEnumRecurseWithSnart(pCar->car_model_actor, NULL, FlapBit, &flapbit_arg);
            AREndPipingSession();
        }
        BrVector3Copy(&car_crush_spec->field_0x138, &pCar->collision_info->v);
        last_flap_bits = gPHIL_last_physics_tick;
    }
}
#endif

// FUNCTION: CARMA2_HW 0x004381b0
void C2_NAKED C2_HOOK_FASTCALL DoFlapping(void) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;

    for (i = 0; i < gCount_toggled_doors; i++) {
        tToggled_door *toggle_door = &gToggled_doors[i];
        br_actor *actor = toggle_door->actor;
        if (actor != NULL) {
            tUser_crush_data *user_crush_data = actor->user;
            if (user_crush_data != NULL) {
                tCar_crush_buffer_entry *crush_data = user_crush_data->crush_data;
                if (crush_data != NULL) {
                    tCar_crush_flap_data *flap_data = crush_data->flap_data;
                    if (flap_data != NULL) {
                        if (!flap_data->field_0x0 && (toggle_door->field_0x8 & 0x1)) {
                            StartFlapping(actor, toggle_door->car, flap_data->field_0x0, toggle_door->field_0xc);
                        } else if (flap_data->field_0x0 && !(toggle_door->field_0x8 & 0x1)) {
                            StopFlapping(actor, toggle_door->car);
                        }
                    }
                }
            }
        }
    }
    gCount_toggled_doors = 0;
    for (i = 0; i < gNum_active_cars; i++) {
        tCar_spec* car;

        car = gActive_car_list[i];
        if (car != NULL && car->driver > eDriver_oppo - 1) {
#ifdef CARPOCALYPSE2_MATCHING
            {
                tCar_crush_spec* car_crush_spec;
                float dt;
                br_vector3 force;
                tFlapBit_arg flapbit_arg;

                car_crush_spec = car->car_crush_spec;
                if (car_crush_spec != NULL) {
                    if (car_crush_spec->field_0x134) {
                        BrVector3Sub(&force, &car->collision_info->v, &car_crush_spec->field_0x138);
                        dt = (float)(gPHIL_last_physics_tick - last_flap_bits) * gFLOAT_00589b34;
                        FlapInvScale(&force, &force, dt);
                        force.v[0] = gBatty_gravity.v[0] + force.v[0];
                        force.v[1] = gBatty_gravity.v[1] + force.v[1];
                        force.v[2] = gBatty_gravity.v[2] + force.v[2];
                        BrMatrix34TApplyV(&flapbit_arg.force, &force, &car->car_master_actor->t.t.mat);
                        flapbit_arg.car = car;
                        ARStartPipingSession(ePipe_chunk_flap_bits);
                        DRActorEnumRecurseWithSnart(car->car_model_actor, NULL, FlapBit, &flapbit_arg);
                        AREndPipingSession();
                    }
                    BrVector3Copy(&car_crush_spec->field_0x138, &car->collision_info->v);
                    last_flap_bits = gPHIL_last_physics_tick;
                }
            }
#else
            FlapBits(car);
#endif
        }
    }
#else
    __asm {
        sub esp, 0x34
        mov eax, dword ptr [gCount_toggled_doors]
        push ebx
        push ebp
        push esi
        test eax, eax
        push edi
        mov dword ptr [esp+0x14], 0
        jle L00438329
        mov esi, offset gToggled_doors+0x8
        mov dword ptr [esp+0x18], esi
L004381d5:
        mov ecx, dword ptr [esi - 8]
        test ecx, ecx
        je L0043830b
        mov ebx, dword ptr [ecx+0x60]
        test ebx, ebx
        je L0043830b
        mov eax, dword ptr [ebx+8]
        test eax, eax
        je L0043830b
        mov eax, dword ptr [eax+0x30]
        test eax, eax
        je L0043830b
        mov al, byte ptr [eax]
        test al, al
        jne L004382fe
        test byte ptr [esi], 1
        je L004382fa
        mov eax, dword ptr [esi+4]
        mov ebp, dword ptr [esi - 4]
        test ebx, ebx
        mov dword ptr [esp+0x1c], eax
        mov dword ptr [esp+0x10], ebp
        je L0043830b
        mov eax, dword ptr [ebx+8]
        test eax, eax
        je L0043830b
        mov edi, dword ptr [eax+0x30]
        test edi, edi
        mov dword ptr [esp+0x20], edi
        je L0043830b
        mov al, byte ptr [edi+0x20]
        test al, al
        je L00438268
        mov edx, dword ptr [esp+0x1c]
        push 1
        push edx
        mov edx, ebp
        call SetBitForDetachment
        mov dword ptr [ebp+0xe24], 0
        jmp L0043830b
L00438268:
        mov esi, dword ptr [ebx]
        test esi, esi
        je L004382c7
        mov eax, dword ptr [esi+0x4c]
        xor ebp, ebp
        cmp word ptr [eax+8], bp
        jbe L004382c3
        xor edi, edi
L0043827b:
        mov eax, dword ptr [eax+0x18]
        mov ecx, dword ptr [eax+edi+0xc]
        xor eax, eax
        mov ax, word ptr [ecx]
        lea edx, [eax+eax*4]
        mov eax, dword ptr [esi+0xc]
        mov eax, dword ptr [eax+edx*8+8]
        test eax, eax
        je L004382ae
        mov ecx, dword ptr [eax+0x20]
        test ch, 0x10
        jne L004382ae
        or ch, 0x10
        push 2
        push eax
        mov dword ptr [eax+0x20], ecx
        call BrMaterialUpdate
        add esp, 8
L004382ae:
        mov eax, dword ptr [esi+0x4c]
        xor ecx, ecx
        inc ebp
        add edi, 0x24
        mov cx, word ptr [eax+8]
        cmp ebp, ecx
        jl L0043827b
        mov edi, dword ptr [esp+0x20]
L004382c3:
        mov ebp, dword ptr [esp+0x10]
L004382c7:
        mov edx, dword ptr [ebx]
        mov ecx, 1
        call PipeSingleDSModel
        mov eax, dword ptr [ebp+0x18d4]
        mov esi, dword ptr [esp+0x18]
        mov cl, byte ptr [eax+0x134]
        inc cl
        mov byte ptr [eax+0x134], cl
        mov byte ptr [edi], 1
        mov dword ptr [ebp+0xe24], 0
        jmp L0043830b
L004382fa:
        test al, al
        je L0043830b
L004382fe:
        test byte ptr [esi], 1
        jne L0043830b
        mov edx, dword ptr [esi - 4]
        call StopFlapping
L0043830b:
        mov eax, dword ptr [esp+0x14]
        mov ecx, dword ptr [gCount_toggled_doors]
        inc eax
        add esi, 0x10
        cmp eax, ecx
        mov dword ptr [esp+0x14], eax
        mov dword ptr [esp+0x18], esi
        jl L004381d5
L00438329:
        mov eax, dword ptr [gNum_active_cars]
        xor ebp, ebp
        cmp eax, ebp
        mov dword ptr [gCount_toggled_doors], ebp
        jle L004384b1
        mov ecx, dword ptr [gPHIL_last_physics_tick]
        mov edx, dword ptr [last_flap_bits]
        mov ebx, offset gActive_car_list
L0043834f:
        mov esi, dword ptr [ebx]
        test esi, esi
        je L004384a0
        cmp dword ptr [esi+0xc], 5
        jle L004384a0
        mov edi, dword ptr [esi+0x18d4]
        test edi, edi
        je L004384a0
        mov al, byte ptr [edi+0x134]
        test al, al
        je L0043846e
        mov eax, dword ptr [esi+8]
        sub ecx, edx
        mov dword ptr [esp+0x24], 0
        mov dword ptr [esp+0x20], ecx
        fld dword ptr [eax+0x68]
        fsub dword ptr [edi+0x138]
        fild qword ptr [esp+0x20]
        fxch st(1)
        fstp dword ptr [esp+0x28]
        fld dword ptr [eax+0x6c]
        fsub dword ptr [edi+0x13c]
        fxch st(1)
        fmul dword ptr [gFLOAT_00589b34]
        fxch st(1)
        fstp dword ptr [esp+0x2c]
        fld dword ptr [eax+0x70]
        fsub dword ptr [edi+0x140]
        fld dword ptr [esp+0x28]
        fld dword ptr [esp+0x2c]
        fld st(2)
        fxch st(4)
        fstp dword ptr [esp+0x10]
        fxch st(1)
        fdiv dword ptr [esp+0x10]
        fxch st(1)
        mov edx, dword ptr [esi+0x10]
        lea eax, [esp+0x28]
        add edx, 0x2c
        lea ecx, [esp+0x38]
        push edx
        push eax
        push ecx
        fdiv dword ptr [esp+0x1c]
        fxch st(3)
        fdiv dword ptr [esp+0x1c]
        fld dword ptr [gBatty_gravity]
        fld dword ptr [gBatty_gravity+0x4]
        fld dword ptr [gBatty_gravity+0x8]
        fxch st(4)
        fstp dword ptr [esp+0x34]
        fxch st(1)
        fadd dword ptr [esp+0x34]
        fxch st(5)
        fstp dword ptr [esp+0x38]
        fadd dword ptr [esp+0x38]
        fxch st(1)
        fstp dword ptr [esp+0x3c]
        fxch st(1)
        fadd dword ptr [esp+0x3c]
        fxch st(3)
        fstp dword ptr [esp+0x34]
        fstp dword ptr [esp+0x38]
        fxch st(1)
        fstp dword ptr [esp+0x3c]
        fstp st(0)
        call BrMatrix34TApplyV
        add esp, 0xc
        mov ecx, 0x17
        mov dword ptr [esp+0x34], esi
        call ARStartPipingSession
        mov ecx, dword ptr [esi+0xe0c]
        lea edx, [esp+0x34]
        push edx
        push offset FlapBit
        xor edx, edx
        call DRActorEnumRecurseWithSnart
        call AREndPipingSession
L0043846e:
        mov eax, dword ptr [esi+8]
        mov ecx, dword ptr [eax+0x68]
        mov dword ptr [edi+0x138], ecx
        mov edx, dword ptr [esi+8]
        mov eax, dword ptr [edx+0x6c]
        mov dword ptr [edi+0x13c], eax
        mov ecx, dword ptr [esi+8]
        mov edx, dword ptr [ecx+0x70]
        mov dword ptr [edi+0x140], edx
        mov ecx, dword ptr [gPHIL_last_physics_tick]
        mov edx, ecx
        mov dword ptr [last_flap_bits], edx
L004384a0:
        mov eax, dword ptr [gNum_active_cars]
        inc ebp
        add ebx, 4
        cmp ebp, eax
        jl L0043834f
L004384b1:
        pop edi
        pop esi
        pop ebp
        pop ebx
        add esp, 0x34
        ret
    }
#endif
}
// FUNCTION: CARMA2_HW 0x00434910
int C2_HOOK_FASTCALL BitObjectIsSufficientlyOutsideCarObjectToDetach(tPhysics_object* pObject) {
    int i;

    for (i = 0; i < pObject->physics_joint1->count_limits; i++) {
        br_vector3 tv;
        br_vector3 tv2;

        BrMatrix34ApplyV(&tv, &pObject->physics_joint1->limits[i].child, &pObject->actor->t.t.mat);
        BrMatrix34TApplyV(&tv2, &tv, &pObject->parent->actor->t.t.mat);
        if (BrVector3Dot(&pObject->physics_joint1->limits[i].parent, &tv2) > 0.707107f) {
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
                && (car = (tCar_spec*)object->owner) != NULL
                && object->flags_0x238 == 1
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

// C2PORT-NONMATCHING: best 35.76% | difftest PASS 2000 trials (spec difftest/specs/00433c70_BendCar.json) | reason VC5 x87 scheduling, register allocation, and stack layout
// FUNCTION: CARMA2_HW 0x00433c70
void C2_HOOK_FASTCALL BendCar(tCar_spec* pCar, br_angle pAngle_x, br_angle pAngle_y, br_angle pAngle_z, const br_vector3* pArg5, float pArg6, int pArg7) {
    tCar_crush_spec* crush_spec;
    undefined4* puVar7;
    int i;
    float fVar1;
    float fVar2;
    float fVar3;
    float fVar4;
    float matrix_storage[17];
    br_matrix34* matrix;
    float* crush_values;

    crush_spec = pCar->car_crush_spec;
    if (crush_spec == NULL || *(int*)((tU8*)pCar + 0x1b4) != 0 || crush_spec->field_0x144 != 0) {
        return;
    }
    matrix = (br_matrix34*)&matrix_storage[3];

    i = 0;
    fVar2 = pArg5->v[2];
    if (0 < crush_spec->field_0x270) {
        puVar7 = (undefined4*)&crush_spec->field_0x274[0].field_0x8;
        do {
            void** bit_object_ptr;

            bit_object_ptr = *(void***)((tU8*)puVar7[-2] + 0x60);
            if (bit_object_ptr == NULL || *bit_object_ptr == NULL) {
                goto LAB_00433d12;
            }
            fVar1 = (fVar2 - pArg6) * gFloat_00589ae4;
            fVar3 = fVar1 + pArg6;
            if (fVar1 < gCrushZero) {
                if (*(float*)((tU8*)*bit_object_ptr + 0x3c) < fVar3) {
                    goto LAB_00433d12;
                }
            } else if (fVar3 < *(float*)((tU8*)*bit_object_ptr + 0x48)) {
                goto LAB_00433d12;
            }
            goto LAB_00433d39;

        LAB_00433d12:
            if (BitObjectIsSufficientlyOutsideCarObjectToDetach((tPhysics_object*)*puVar7)) {
                FullyDetachBit(pCar, (tPhysics_object*)*puVar7);
            } else {
                ReAttachBit(pCar, (br_actor*)puVar7[-1], (br_actor*)puVar7[-2], (tPhysics_object*)*puVar7);
            }

        LAB_00433d39:
            i++;
            puVar7 += 5;
        } while (i < crush_spec->field_0x270);
    }
    MungeDetachLists(crush_spec);

    FUN_00435550(pCar, pCar->car_master_actor, pAngle_x, pAngle_y, pAngle_z, pArg5, pArg6,
            (float*)&crush_spec->field_0x154[0]);
    crush_values = (float*)&crush_spec->field_0x154[0];
    /* Retail keeps crush_values in the local right after the matrix (0x433d93); FUN_00435110
     * reads it back as *(matrix + 0x34) to get the pivot z (crush_values[8]). */
    *(float**)&matrix_storage[16] = crush_values;

    BrMatrix34Translate(matrix,
            -crush_values[6],
            -crush_values[7],
            -crush_values[8]);
    if (crush_spec->field_0x192 != 0) {
        BrMatrix34PostRotateY(matrix, crush_spec->field_0x192);
    }
    if (crush_spec->field_0x190 != 0) {
        BrMatrix34PostRotateX(matrix, crush_spec->field_0x190);
    }
    if (crush_spec->field_0x194 != 0) {
        BrMatrix34PostRotateZ(matrix, crush_spec->field_0x194);
    }
    BrMatrix34PostTranslate(matrix,
            crush_values[6],
            crush_values[7],
            crush_values[8]);
    LoseCarPowerups(pCar);
    if (crush_spec->expand_bounding_box != 0) {
        SwapShapesIfPossible(pCar);
    }
    i = 0;
    if (0 < crush_spec->count_shapes) {
        do {
            FUN_00435110((int*)&crush_spec->field_0x4[i], (int)matrix);
            i++;
        } while (i < crush_spec->count_shapes);
    }

    crush_spec->field_0x264.v[0] = *(float*)((tU8*)pCar->collision_info + 0x14);
    crush_spec->field_0x264.v[1] = *(float*)((tU8*)pCar->collision_info + 0x18);
    crush_spec->field_0x264.v[2] = *(float*)((tU8*)pCar->collision_info + 0x1c);
    crush_spec->field_0x10.v[0] = *(float*)((tU8*)pCar->collision_info + 0x14);
    crush_spec->field_0x10.v[1] = *(float*)((tU8*)pCar->collision_info + 0x18);
    crush_spec->field_0x10.v[2] = *(float*)((tU8*)pCar->collision_info + 0x1c);
    FUN_00434f60(&crush_spec->field_0x10, matrix);
    crush_spec->expand_bounding_box = 0xb;
    SwapShapesIfPossible(pCar);
    FUN_00435310(pCar, matrix);
    FUN_004f5a60(pCar);
    if (pArg7 != 0) {
        ApplyRandomDamageToCar(pCar, gMin_bend_damage, gMax_bend_damage);
    }

    *(int*)((tU8*)pCar + 0xe24) = 0;
    fVar3 = (pCar->collision_info->bb1.max.v[0] - pCar->collision_info->bb1.min.v[0]) * gFloat_00589ae8;
    fVar2 = pCar->collision_info->bb1.max.v[2];
    fVar4 = (pCar->collision_info->bb1.max.v[1] - pCar->collision_info->bb1.min.v[1]) * gFloat_00589ae8;
    fVar1 = pCar->collision_info->bb1.min.v[2];
    crush_spec->field_0x11c.min.v[0] = fVar3 + pCar->collision_info->bb1.min.v[0];
    matrix_storage[1] = pCar->collision_info->bb1.min.v[1];
    matrix_storage[2] = pCar->collision_info->bb1.min.v[2];
    fVar2 = (fVar2 - fVar1) * gFloat_00589ae8;
    crush_spec->field_0x11c.min.v[1] = matrix_storage[1] + fVar4;
    crush_spec->field_0x11c.min.v[2] = matrix_storage[2] + fVar2;
    crush_spec->field_0x11c.max.v[0] = pCar->collision_info->bb1.max.v[0] - fVar3;
    crush_spec->field_0x11c.max.v[1] = pCar->collision_info->bb1.max.v[1] - fVar4;
    crush_spec->field_0x11c.max.v[2] = pCar->collision_info->bb1.max.v[2] - fVar2;
    crush_spec->field_0x1d0 = crush_spec->field_0x2b0;
    crush_spec->field_0x144 = 1;
    CrashEarnings(pCar, NULL);

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
    DRS3StartSound3D(gXXX_outlet, eSoundId_BendCar, &pCar->collision_info->actor->t.t.translate.t, &gZero_v__car, 1, 255, IRandomBetween(0xe000, 0x12000), 0x10000);
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

// FUNCTION: CARMA2_HW 0x00437480
void C2_NAKED C2_HOOK_FASTCALL FUN_00437480(tPhysics_joint* pJoint, tCar_crush_detach_data* pDetach_data, tCar_crush_flap_data* pFlap_data, br_model* pModel, tPhysics_object* pObject, br_vector3* pPoint) {
#ifndef CARPOCALYPSE2_MATCHING
    br_vector3 local_point;
    br_scalar length;
    br_scalar inverse_length;
    br_scalar fVar1;
    br_scalar fVar2;
    br_scalar fVar3;
    int axis_bits;
    int sign_bits;
    int axis;
    int i;
    int j;

    pJoint->type = eJoint_ball_n_socket;
    if (pPoint != NULL) {
        BrVector3Copy(&pJoint->field_0x08, pPoint);
    } else if (pFlap_data != NULL) {
        BrVector3Copy(&pJoint->field_0x08, &pModel->vertices[pFlap_data->hinge0].p);
    } else {
        BrVector3Copy(&pJoint->field_0x08, &pModel->vertices[pDetach_data->field_0x30].p);
    }
    BrMatrix34ApplyP(&local_point, &pJoint->field_0x08, &pObject->actor->t.t.mat);
    DrMatrix34ApplyLPInverse(&pJoint->field_0x14, &local_point, &pObject->parent->actor->t.t.mat);
    pJoint->count_limits = 2;
    for (i = 0; i < pJoint->count_limits; i++) {
        pJoint->limits[i].type = eJoint_limit_plane;
        pJoint->limits[i].value = 0.0f;
    }
    if (pFlap_data != NULL) {
        BrVector3Sub(&pJoint->limits[0].child, &pModel->vertices[pFlap_data->hinge1].p, &pModel->vertices[pFlap_data->hinge0].p);
        length = BrVector3Length(&pJoint->limits[0].child);
        if (length > gFLOAT_00589a5c) {
            inverse_length = gDOUBLE_00589a60 / length;
            pJoint->limits[0].child.v[0] = inverse_length * pJoint->limits[0].child.v[0];
            pJoint->limits[0].child.v[1] = pJoint->limits[0].child.v[1] * inverse_length;
            pJoint->limits[0].child.v[2] = pJoint->limits[0].child.v[2] * inverse_length;
        } else {
            pJoint->limits[0].child.v[0] = 1.0f;
            pJoint->limits[0].child.v[1] = 0.0f;
            pJoint->limits[0].child.v[2] = 0.0f;
        }
        BrVector3Sub(&pJoint->limits[1].child, &pModel->vertices[pFlap_data->hinge2].p, &pModel->vertices[pFlap_data->hinge0].p);
        length = BrVector3Length(&pJoint->limits[1].child);
        if (length > gFLOAT_00589a5c) {
            inverse_length = gDOUBLE_00589a60 / length;
            pJoint->limits[1].child.v[0] = pJoint->limits[1].child.v[0] * inverse_length;
            pJoint->limits[1].child.v[1] = pJoint->limits[1].child.v[1] * inverse_length;
            pJoint->limits[1].child.v[2] = pJoint->limits[1].child.v[2] * inverse_length;
        } else {
            pJoint->limits[1].child.v[0] = 1.0f;
            pJoint->limits[1].child.v[1] = 0.0f;
            pJoint->limits[1].child.v[2] = 0.0f;
        }
        pJoint->limits[0].parent.v[0] = pJoint->limits[1].child.v[2] * pJoint->limits[0].child.v[1]
            - pJoint->limits[1].child.v[1] * pJoint->limits[0].child.v[2];
        pJoint->limits[0].parent.v[1] = pJoint->limits[0].child.v[2] * pJoint->limits[1].child.v[0]
            - pJoint->limits[0].child.v[0] * pJoint->limits[1].child.v[2];
        fVar1 = pJoint->limits[0].child.v[0] * gFLOAT_00589a58;
        pJoint->limits[0].parent.v[2] = pJoint->limits[0].child.v[2] * gFLOAT_00589a58
            + (pJoint->limits[0].child.v[0] * pJoint->limits[1].child.v[1] - pJoint->limits[0].child.v[1] * pJoint->limits[1].child.v[0]);
        fVar1 = pJoint->limits[1].child.v[0] * gFLOAT_00589a58 + fVar1 + pJoint->limits[0].parent.v[0];
        fVar2 = pJoint->limits[1].child.v[1] * gFLOAT_00589a58 + pJoint->limits[0].child.v[1] * gFLOAT_00589a58 + pJoint->limits[0].parent.v[1];
        fVar3 = pJoint->limits[1].child.v[2] * gFLOAT_00589a58 + pJoint->limits[0].parent.v[2];
        pJoint->limits[0].parent.v[0] = fVar1;
        pJoint->limits[0].parent.v[1] = fVar2;
        pJoint->limits[0].parent.v[2] = fVar3;
        length = BrVector3Length(&pJoint->limits[0].parent);
        if (length > gFLOAT_00589a5c) {
            inverse_length = gDOUBLE_00589a60 / length;
            pJoint->limits[0].parent.v[0] = pJoint->limits[0].parent.v[0] * inverse_length;
            pJoint->limits[0].parent.v[1] = pJoint->limits[0].parent.v[1] * inverse_length;
            pJoint->limits[0].parent.v[2] = pJoint->limits[0].parent.v[2] * inverse_length;
        } else {
            pJoint->limits[0].parent.v[0] = 1.0f;
            pJoint->limits[0].parent.v[1] = 0.0f;
            pJoint->limits[0].parent.v[2] = 0.0f;
        }
        BrVector3Copy(&pJoint->limits[1].parent, &pJoint->limits[0].parent);
        return;
    }
    axis_bits = pDetach_data->field_0x33;
    sign_bits = pDetach_data->field_0x32;
    axis = axis_bits >> 1;
    pJoint->limits[0].parent.v[0] = 0.0f;
    pJoint->limits[0].parent.v[1] = 0.0f;
    pJoint->limits[0].parent.v[2] = 0.0f;
    pJoint->limits[0].parent.v[axis] = (axis_bits & 1) ? gFLOAT_00589a54 : gFLOAT_00589b2c;
    BrVector3Copy(&pJoint->limits[1].parent, &pJoint->limits[0].parent);
    i = (axis + 1) % 3;
    pJoint->limits[0].child.v[i] = ((sign_bits >> i) & 1) ? gFLOAT_00589b2c : gFLOAT_00589a54;
    j = (axis + 2) % 3;
    pJoint->limits[0].child.v[j] = 0.0f;
    pJoint->limits[0].child.v[axis] = (axis_bits & 1) ? gFLOAT_00589a58 : gFLOAT_00589b30;
    length = BrVector3Length(&pJoint->limits[0].child);
    if (length > gFLOAT_00589a5c) {
        inverse_length = gDOUBLE_00589a60 / length;
        pJoint->limits[0].child.v[0] = inverse_length * pJoint->limits[0].child.v[0];
        pJoint->limits[0].child.v[1] = pJoint->limits[0].child.v[1] * inverse_length;
        pJoint->limits[0].child.v[2] = pJoint->limits[0].child.v[2] * inverse_length;
    } else {
        pJoint->limits[0].child.v[0] = 1.0f;
        pJoint->limits[0].child.v[1] = 0.0f;
        pJoint->limits[0].child.v[2] = 0.0f;
    }
    pJoint->limits[1].child.v[i] = 0.0f;
    pJoint->limits[1].child.v[j] = ((sign_bits >> j) & 1) ? gFLOAT_00589b2c : gFLOAT_00589a54;
    pJoint->limits[1].child.v[axis] = (axis_bits & 1) ? gFLOAT_00589a58 : gFLOAT_00589b30;
    length = BrVector3Length(&pJoint->limits[1].child);
    if (length > gFLOAT_00589a5c) {
        inverse_length = gDOUBLE_00589a60 / length;
        pJoint->limits[1].child.v[0] = pJoint->limits[1].child.v[0] * inverse_length;
        pJoint->limits[1].child.v[1] = pJoint->limits[1].child.v[1] * inverse_length;
        pJoint->limits[1].child.v[2] = pJoint->limits[1].child.v[2] * inverse_length;
    } else {
        pJoint->limits[1].child.v[0] = 1.0f;
        pJoint->limits[1].child.v[1] = 0.0f;
        pJoint->limits[1].child.v[2] = 0.0f;
    }
#else
    __asm {
        sub esp, 0xc
        push ebx
        mov ebx, dword ptr [esp+0x18]
        push ebp
        push esi
        mov esi, ecx
        push edi
        mov ecx, dword ptr [esp+0x2c]
        mov edi, dword ptr [esp+0x20]
        test ecx, ecx
        mov ebp, edx
        mov dword ptr [esi], 3
        je L004374b3
        mov edx, dword ptr [ecx]
        lea eax, [esi+8]
        mov dword ptr [eax], edx
        mov edx, dword ptr [ecx+4]
        mov dword ptr [esi+0xc], edx
        mov ecx, dword ptr [ecx+8]
        jmp L00437527
L004374b3:
        test edi, edi
        lea eax, [esi+8]
        je L004374f3
        xor ecx, ecx
        mov cx, word ptr [edi+0xa]
        lea edx, [ecx+ecx*4]
        mov ecx, dword ptr [ebx+8]
        mov edx, dword ptr [ecx+edx*8]
        xor ecx, ecx
        mov dword ptr [eax], edx
        mov cx, word ptr [edi+0xa]
        mov edx, dword ptr [ebx+8]
        lea ecx, [ecx+ecx*4]
        mov ecx, dword ptr [edx+ecx*8+4]
        mov dword ptr [esi+0xc], ecx
        xor ecx, ecx
        mov cx, word ptr [edi+0xa]
        lea edx, [ecx+ecx*4]
        mov ecx, dword ptr [ebx+8]
        mov edx, dword ptr [ecx+edx*8+8]
        mov dword ptr [esi+0x10], edx
        jmp L0043752a
L004374f3:
        mov edx, dword ptr [ebx+8]
        xor ecx, ecx
        mov cx, word ptr [ebp+0x30]
        lea ecx, [ecx+ecx*4]
        mov ecx, dword ptr [edx+ecx*8]
        mov dword ptr [eax], ecx
        xor ecx, ecx
        mov cx, word ptr [ebp+0x30]
        lea edx, [ecx+ecx*4]
        mov ecx, dword ptr [ebx+8]
        mov edx, dword ptr [ecx+edx*8+4]
        xor ecx, ecx
        mov dword ptr [esi+0xc], edx
        mov cx, word ptr [ebp+0x30]
        mov edx, dword ptr [ebx+8]
        lea ecx, [ecx+ecx*4]
        mov ecx, dword ptr [edx+ecx*8+8]
L00437527:
        mov dword ptr [esi+0x10], ecx
L0043752a:
        mov edx, dword ptr [esp+0x28]
        mov ecx, dword ptr [edx]
        lea edx, [esp+0x10]
        add ecx, 0x2c
        push ecx
        push eax
        push edx
        call BrMatrix34ApplyP
        mov eax, dword ptr [esp+0x34]
        add esp, 0xc
        mov ecx, dword ptr [eax+0x228]
        mov edx, dword ptr [ecx]
        lea ecx, [esi+0x14]
        add edx, 0x2c
        push edx
        lea edx, [esp+0x14]
        call DrMatrix34ApplyLPInverse
        xor edx, edx
        mov dword ptr [esi+0x50], 2
        xor ecx, ecx
        lea eax, [esi+0x58]
L0043756c:
        mov dword ptr [eax - 4], edx
        mov dword ptr [eax], edx
        inc ecx
        add eax, 0x20
        cmp ecx, dword ptr [esi+0x50]
        jl L0043756c
        cmp edi, edx
        je L004378c2
        xor eax, eax
        mov ecx, dword ptr [ebx+8]
        mov ax, word ptr [edi+0xe]
        lea ebp, [eax+eax*4]
        xor eax, eax
        mov ax, word ptr [edi+0xa]
        fld dword ptr [ecx+ebp*8]
        lea eax, [eax+eax*4]
        fsub dword ptr [ecx+eax*8]
        xor eax, eax
        fstp dword ptr [esi+0x5c]
        mov ax, word ptr [edi+0xe]
        mov ecx, dword ptr [ebx+8]
        lea ebp, [eax+eax*4]
        xor eax, eax
        mov ax, word ptr [edi+0xa]
        fld dword ptr [ecx+ebp*8+4]
        lea eax, [eax+eax*4]
        fsub dword ptr [ecx+eax*8+4]
        xor eax, eax
        fstp dword ptr [esi+0x60]
        mov ax, word ptr [edi+0xe]
        mov ecx, dword ptr [ebx+8]
        lea ebp, [eax+eax*4]
        xor eax, eax
        mov ax, word ptr [edi+0xa]
        fld dword ptr [ecx+ebp*8+8]
        lea eax, [eax+eax*4]
        fsub dword ptr [ecx+eax*8+8]
        fst dword ptr [esi+0x64]
        fld dword ptr [esi+0x5c]
        fstp dword ptr [esp+0x28]
        fld dword ptr [esi+0x60]
        fstp dword ptr [esp+0x2c]
        fld dword ptr [esp+0x28]
        fmul dword ptr [esp+0x28]
        fld dword ptr [esp+0x2c]
        fmul dword ptr [esp+0x2c]
        faddp st(1), st(0)
        fld st(1)
        fmul st(0), st(2)
        faddp st(1), st(0)
        fsqrt
        fstp st(1)
        fcom dword ptr [gFLOAT_00589a5c]
        fnstsw ax
        test ah, 0x41
        jne L00437647
        fdivr qword ptr [gDOUBLE_00589a60]
        fld dword ptr [esi+0x5c]
        fld dword ptr [esi+0x60]
        fld dword ptr [esi+0x64]
        fxch st(2)
        fxch st(1)
        fxch st(1)
        fmul st(0), st(3)
        fxch st(1)
        fmul st(0), st(3)
        fxch st(2)
        fmul st(0), st(3)
        fxch st(1)
        fstp dword ptr [esi+0x5c]
        fxch st(1)
        fstp dword ptr [esi+0x60]
        fstp dword ptr [esi+0x64]
        fstp st(0)
        jmp L00437656
L00437647:
        fstp st(0)
        mov dword ptr [esi+0x5c], 0x3f800000
        mov dword ptr [esi+0x60], edx
        mov dword ptr [esi+0x64], edx
L00437656:
        xor eax, eax
        mov ecx, dword ptr [ebx+8]
        mov ax, word ptr [edi+0x10]
        lea ebp, [eax+eax*4]
        xor eax, eax
        mov ax, word ptr [edi+0xa]
        fld dword ptr [ecx+ebp*8]
        lea eax, [eax+eax*4]
        fsub dword ptr [ecx+eax*8]
        xor eax, eax
        fstp dword ptr [esi+0x7c]
        mov ax, word ptr [edi+0x10]
        mov ecx, dword ptr [ebx+8]
        lea ebp, [eax+eax*4]
        xor eax, eax
        mov ax, word ptr [edi+0xa]
        fld dword ptr [ecx+ebp*8+4]
        lea eax, [eax+eax*4]
        fsub dword ptr [ecx+eax*8+4]
        xor eax, eax
        fstp dword ptr [esi+0x80]
        mov ax, word ptr [edi+0x10]
        mov ebx, dword ptr [ebx+8]
        lea ecx, [eax+eax*4]
        xor eax, eax
        mov ax, word ptr [edi+0xa]
        fld dword ptr [ebx+ecx*8+8]
        lea eax, [eax+eax*4]
        fsub dword ptr [ebx+eax*8+8]
        fst dword ptr [esi+0x84]
        fld dword ptr [esi+0x7c]
        fstp dword ptr [esp+0x28]
        fld dword ptr [esi+0x80]
        fstp dword ptr [esp+0x2c]
        fld dword ptr [esp+0x28]
        fmul dword ptr [esp+0x28]
        fld dword ptr [esp+0x2c]
        fmul dword ptr [esp+0x2c]
        faddp st(1), st(0)
        fld st(1)
        fmul st(0), st(2)
        faddp st(1), st(0)
        fsqrt
        fstp st(1)
        fcom dword ptr [gFLOAT_00589a5c]
        fnstsw ax
        test ah, 0x41
        jne L00437730
        fdivr qword ptr [gDOUBLE_00589a60]
        fld dword ptr [esi+0x7c]
        fld dword ptr [esi+0x80]
        fld dword ptr [esi+0x84]
        fxch st(2)
        fxch st(1)
        fxch st(1)
        fmul st(0), st(3)
        fxch st(1)
        fmul st(0), st(3)
        fxch st(2)
        fmul st(0), st(3)
        fxch st(1)
        fstp dword ptr [esi+0x7c]
        fxch st(1)
        fstp dword ptr [esi+0x80]
        fstp dword ptr [esi+0x84]
        fstp st(0)
        jmp L00437745
L00437730:
        fstp st(0)
        mov dword ptr [esi+0x7c], 0x3f800000
        mov dword ptr [esi+0x80], edx
        mov dword ptr [esi+0x84], edx
L00437745:
        fld dword ptr [esi+0x7c]
        fld dword ptr [esi+0x64]
        fld dword ptr [esi+0x84]
        fld dword ptr [esi+0x5c]
        fld dword ptr [esi+0x84]
        fmul dword ptr [esi+0x60]
        fld dword ptr [esi+0x80]
        fmul dword ptr [esi+0x64]
        fsubp st(1), st(0)
        fstp dword ptr [esi+0x68]
        fld dword ptr [esi+0x80]
        fld dword ptr [esi+0x5c]
        fxch st(4)
        fmulp st(5), st(0)
        fld dword ptr [esi+0x7c]
        fxch st(2)
        fmulp st(3), st(0)
        fxch st(2)
        fsubp st(4), st(0)
        fld st(3)
        fxch st(3)
        fmulp st(2), st(0)
        fxch st(2)
        fstp dword ptr [esi+0x6c]
        fld dword ptr [esi+0x60]
        fld dword ptr [esi+0x5c]
        fld dword ptr [esi+0x7c]
        fxch st(2)
        fmulp st(4), st(0)
        fld dword ptr [esi+0x64]
        fxch st(4)
        fsubp st(3), st(0)
        fld dword ptr [esi+0x60]
        fxch st(4)
        fmul dword ptr [gFLOAT_00589a58]
        faddp st(3), st(0)
        fmul dword ptr [gFLOAT_00589a58]
        fxch st(2)
        fstp dword ptr [esi+0x70]
        fmul dword ptr [gFLOAT_00589a58]
        fxch st(2)
        fmul dword ptr [gFLOAT_00589a58]
        fxch st(2)
        faddp st(1), st(0)
        fld dword ptr [esi+0x80]
        fmul dword ptr [gFLOAT_00589a58]
        fxch st(1)
        fadd dword ptr [esi+0x68]
        fld dword ptr [esi+0x84]
        fxch st(2)
        faddp st(3), st(0)
        fxch st(1)
        fmul dword ptr [gFLOAT_00589a58]
        fxch st(2)
        fadd st(0), st(3)
        fxch st(1)
        fst dword ptr [esp+0x28]
        fxch st(2)
        fadd dword ptr [esi+0x70]
        fxch st(1)
        fst dword ptr [esp+0x2c]
        fxch st(2)
        fstp dword ptr [esi+0x68]
        fxch st(1)
        fstp dword ptr [esi+0x6c]
        fstp dword ptr [esp+0x20]
        fstp st(0)
        fld dword ptr [esp+0x28]
        fld dword ptr [esp+0x2c]
        fld dword ptr [esp+0x20]
        fxch st(2)
        fst dword ptr [esp+0x24]
        fld st(1)
        fxch st(1)
        fmul dword ptr [esp+0x24]
        fxch st(1)
        fmul st(0), st(2)
        fxch st(3)
        fst dword ptr [esp+0x24]
        fmul dword ptr [esp+0x24]
        fxch st(3)
        faddp st(1), st(0)
        fld dword ptr [esp+0x20]
        fxch st(3)
        faddp st(1), st(0)
        fxch st(2)
        fstp dword ptr [esi+0x70]
        fxch st(1)
        fsqrt
        fstp st(1)
        fcom dword ptr [gFLOAT_00589a5c]
        fnstsw ax
        test ah, 0x41
        jne L0043788e
        fdivr qword ptr [gDOUBLE_00589a60]
        fld dword ptr [esi+0x70]
        fld st(1)
        fld st(2)
        fxch st(1)
        fmul dword ptr [esp+0x28]
        fxch st(1)
        fmul dword ptr [esp+0x2c]
        fxch st(2)
        fmul st(0), st(3)
        fxch st(1)
        fstp dword ptr [esi+0x68]
        fxch st(1)
        fstp dword ptr [esi+0x6c]
        fstp dword ptr [esi+0x70]
        fstp st(0)
        jmp L0043789d
L0043788e:
        fstp st(0)
        mov dword ptr [esi+0x68], 0x3f800000
        mov dword ptr [esi+0x6c], edx
        mov dword ptr [esi+0x70], edx
L0043789d:
        mov ecx, dword ptr [esi+0x68]
        mov edx, dword ptr [esi+0x6c]
        mov eax, dword ptr [esi+0x70]
        mov dword ptr [esi+0x88], ecx
        mov dword ptr [esi+0x8c], edx
        mov dword ptr [esi+0x90], eax
        pop edi
        pop esi
        pop ebp
        pop ebx
        add esp, 0xc
        ret 0x10
L004378c2:
        xor eax, eax
        xor ebx, ebx
        mov al, byte ptr [ebp+0x33]
        mov bl, byte ptr [ebp+0x32]
        mov edi, eax
        mov ebp, eax
        sar edi, 1
        and ebp, 1
        mov dword ptr [esp+0x2c], ebx
        mov dword ptr [esi+0x68], edx
        mov dword ptr [esi+0x6c], edx
        mov dword ptr [esi+0x70], edx
        je L004378ec
        fld dword ptr [gFLOAT_00589a54]
        jmp L004378f2
L004378ec:
        fld dword ptr [gFLOAT_00589b2c]
L004378f2:
        fstp dword ptr [esi+edi*4+0x68]
        mov eax, dword ptr [esi+0x70]
        mov edx, dword ptr [esi+0x6c]
        mov ecx, dword ptr [esi+0x68]
        mov dword ptr [esi+0x90], eax
        lea eax, [edi+1]
        mov dword ptr [esi+0x8c], edx
        mov dword ptr [esi+0x88], ecx
        mov ecx, 3
        cdq
        idiv ecx
        mov ecx, edx
        sar ebx, cl
        test bl, 1
        je L0043792d
        fld dword ptr [gFLOAT_00589b2c]
        jmp L00437933
L0043792d:
        fld dword ptr [gFLOAT_00589a54]
L00437933:
        lea eax, [edi+2]
        mov ebx, 3
        cdq
        fstp dword ptr [esi+ecx*4+0x5c]
        idiv ebx
        xor ebx, ebx
        cmp ebp, ebx
        mov dword ptr [esi+edx*4+0x5c], ebx
        je L00437954
        fld dword ptr [gFLOAT_00589a58]
        jmp L0043795a
L00437954:
        fld dword ptr [gFLOAT_00589b30]
L0043795a:
        fstp dword ptr [esi+edi*4+0x5c]
        fld dword ptr [esi+0x5c]
        fld dword ptr [esi+0x60]
        fld dword ptr [esi+0x64]
        fxch st(2)
        fst dword ptr [esp+0x28]
        fld st(1)
        fxch st(1)
        fmul dword ptr [esp+0x28]
        fxch st(1)
        fmul st(0), st(2)
        fxch st(3)
        fst dword ptr [esp+0x28]
        fmul dword ptr [esp+0x28]
        fxch st(3)
        faddp st(1), st(0)
        fxch st(2)
        faddp st(2), st(0)
        fxch st(1)
        fsqrt
        fstp st(1)
        fcom dword ptr [gFLOAT_00589a5c]
        fnstsw ax
        test ah, 0x41
        jne L004379ce
        fdivr qword ptr [gDOUBLE_00589a60]
        fld dword ptr [esi+0x5c]
        fld dword ptr [esi+0x60]
        fld dword ptr [esi+0x64]
        fxch st(2)
        fxch st(1)
        fxch st(1)
        fmul st(0), st(3)
        fxch st(1)
        fmul st(0), st(3)
        fxch st(2)
        fmul st(0), st(3)
        fxch st(1)
        fstp dword ptr [esi+0x5c]
        fxch st(1)
        fstp dword ptr [esi+0x60]
        fstp dword ptr [esi+0x64]
        fstp st(0)
        jmp L004379dd
L004379ce:
        fstp st(0)
        mov dword ptr [esi+0x5c], 0x3f800000
        mov dword ptr [esi+0x60], ebx
        mov dword ptr [esi+0x64], ebx
L004379dd:
        mov eax, dword ptr [esp+0x2c]
        mov dword ptr [esi+ecx*4+0x7c], ebx
        mov ecx, edx
        sar eax, cl
        test al, 1
        je L004379f5
        fld dword ptr [gFLOAT_00589b2c]
        jmp L004379fb
L004379f5:
        fld dword ptr [gFLOAT_00589a54]
L004379fb:
        fstp dword ptr [esi+edx*4+0x7c]
        cmp ebp, ebx
        je L00437a0b
        fld dword ptr [gFLOAT_00589a58]
        jmp L00437a11
L00437a0b:
        fld dword ptr [gFLOAT_00589b30]
L00437a11:
        fstp dword ptr [esi+edi*4+0x7c]
        fld dword ptr [esi+0x7c]
        fld dword ptr [esi+0x80]
        fld dword ptr [esi+0x84]
        fxch st(2)
        fst dword ptr [esp+0x28]
        fld st(1)
        fxch st(1)
        fmul dword ptr [esp+0x28]
        fxch st(1)
        fmul st(0), st(2)
        fxch st(3)
        fst dword ptr [esp+0x28]
        fmul dword ptr [esp+0x28]
        fxch st(3)
        faddp st(1), st(0)
        fxch st(2)
        faddp st(2), st(0)
        fxch st(1)
        fsqrt
        fstp st(1)
        fcom dword ptr [gFLOAT_00589a5c]
        fnstsw ax
        test ah, 0x41
        jne L00437a9f
        fdivr qword ptr [gDOUBLE_00589a60]
        fld dword ptr [esi+0x7c]
        fld dword ptr [esi+0x80]
        fld dword ptr [esi+0x84]
        fxch st(2)
        fxch st(1)
        fxch st(1)
        fmul st(0), st(3)
        fxch st(1)
        fmul st(0), st(3)
        fxch st(2)
        fmul st(0), st(3)
        fxch st(1)
        fstp dword ptr [esi+0x7c]
        fxch st(1)
        fstp dword ptr [esi+0x80]
        fstp dword ptr [esi+0x84]
        fstp st(0)
        pop edi
        pop esi
        pop ebp
        pop ebx
        add esp, 0xc
        ret 0x10
L00437a9f:
        mov dword ptr [esi+0x7c], 0x3f800000
        mov dword ptr [esi+0x80], ebx
        mov dword ptr [esi+0x84], ebx
        pop edi
        pop esi
        pop ebp
        fstp st(0)
        pop ebx
        add esp, 0xc
        ret 0x10
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00437ac0
void C2_NAKED C2_HOOK_FASTCALL SetUpFlapJoint(tPhysics_joint* pJoint, tCar_crush_flap_data* pFlap_data, br_model* pModel, tPhysics_object* pObject, br_vector3* pPoint1, br_vector3* pPoint2, br_vector3* pPoint3) {
#ifndef CARPOCALYPSE2_MATCHING
    br_vector3* point3;
    br_vector3 local_point;
    br_vector3 scaled;
    br_matrix34 rotation;
    br_scalar length;
    br_scalar inverse_length;
    br_scalar dot;

    pJoint->type = eJoint_quick_hinge;
    if (pPoint1 == NULL) {
        pPoint1 = &pModel->vertices[pFlap_data->hinge0].p;
    }
    if (pPoint2 == NULL) {
        pPoint2 = &pModel->vertices[pFlap_data->hinge1].p;
    }
    if (pPoint3 != NULL) {
        point3 = pPoint3;
    } else {
        point3 = &pModel->vertices[pFlap_data->hinge2].p;
    }
    BrVector3Copy(&pJoint->field_0x08, pPoint1);
    BrMatrix34ApplyP(&local_point, &pJoint->field_0x08, &pObject->actor->t.t.mat);
    DrMatrix34ApplyLPInverse(&pJoint->field_0x14, &local_point, &pObject->parent->actor->t.t.mat);

    BrVector3Sub(&pJoint->hinge_axis, pPoint2, pPoint1);
    length = BrVector3Length(&pJoint->hinge_axis);
    if (length > gFLOAT_00589a5c) {
        inverse_length = gDOUBLE_00589a60 / length;
        pJoint->hinge_axis.v[0] = inverse_length * pJoint->hinge_axis.v[0];
        pJoint->hinge_axis.v[1] = inverse_length * pJoint->hinge_axis.v[1];
        pJoint->hinge_axis.v[2] = inverse_length * pJoint->hinge_axis.v[2];
    } else {
        pJoint->hinge_axis.v[0] = 1.0f;
        pJoint->hinge_axis.v[1] = 0.0f;
        pJoint->hinge_axis.v[2] = 0.0f;
    }
    BrVector3Copy(&pJoint->parent_bone_axis, &pJoint->hinge_axis);
    if (pJoint->hinge_axis.v[0] == gCrushZero && pJoint->hinge_axis.v[2] == gCrushZero) {
        pJoint->hinge_axis2.v[0] = 1.0f;
        pJoint->hinge_axis2.v[1] = 0.0f;
        pJoint->hinge_axis2.v[2] = 0.0f;
    } else {
        pJoint->hinge_axis2.v[0] = -pJoint->hinge_axis.v[2];
        pJoint->hinge_axis2.v[1] = 0.0f;
        pJoint->hinge_axis2.v[2] = pJoint->hinge_axis.v[0];
    }
    length = sqrtf(pJoint->hinge_axis2.v[0] * pJoint->hinge_axis2.v[0] + pJoint->hinge_axis2.v[2] * pJoint->hinge_axis2.v[2]);
    if (length > gFLOAT_00589a5c) {
        inverse_length = gDOUBLE_00589a60 / length;
        pJoint->hinge_axis2.v[0] = pJoint->hinge_axis2.v[0] * inverse_length;
        pJoint->hinge_axis2.v[1] = 0.0f;
        pJoint->hinge_axis2.v[2] = pJoint->hinge_axis2.v[2] * inverse_length;
    } else {
        pJoint->hinge_axis2.v[0] = 1.0f;
        pJoint->hinge_axis2.v[1] = 0.0f;
        pJoint->hinge_axis2.v[2] = 0.0f;
    }
    BrVector3Cross(&pJoint->hinge_axis3, &pJoint->hinge_axis, &pJoint->hinge_axis2);
    length = BrVector3Length(&pJoint->hinge_axis3);
    if (length > gFLOAT_00589a5c) {
        inverse_length = gDOUBLE_00589a60 / length;
        pJoint->hinge_axis3.v[0] = pJoint->hinge_axis3.v[0] * inverse_length;
        pJoint->hinge_axis3.v[1] = pJoint->hinge_axis3.v[1] * inverse_length;
        pJoint->hinge_axis3.v[2] = pJoint->hinge_axis3.v[2] * inverse_length;
    } else {
        pJoint->hinge_axis3.v[0] = 1.0f;
        pJoint->hinge_axis3.v[1] = 0.0f;
        pJoint->hinge_axis3.v[2] = 0.0f;
    }

    pJoint->count_limits = 2;
    BrVector3Sub(&pJoint->limits[0].child, point3, pPoint1);
    dot = BrVector3Dot(&pJoint->hinge_axis, &pJoint->limits[0].child);
    pJoint->limits[0].child.v[0] = pJoint->limits[0].child.v[0] - pJoint->hinge_axis.v[0] * dot;
    pJoint->limits[0].child.v[1] = pJoint->limits[0].child.v[1] - pJoint->hinge_axis.v[1] * dot;
    pJoint->limits[0].child.v[2] = pJoint->limits[0].child.v[2] - pJoint->hinge_axis.v[2] * dot;
    length = BrVector3Length(&pJoint->limits[0].child);
    if (length > gFLOAT_00589a5c) {
        inverse_length = gDOUBLE_00589a60 / length;
        pJoint->limits[0].child.v[0] = pJoint->limits[0].child.v[0] * inverse_length;
        pJoint->limits[0].child.v[1] = pJoint->limits[0].child.v[1] * inverse_length;
        pJoint->limits[0].child.v[2] = pJoint->limits[0].child.v[2] * inverse_length;
    } else {
        pJoint->limits[0].child.v[0] = 1.0f;
        pJoint->limits[0].child.v[1] = 0.0f;
        pJoint->limits[0].child.v[2] = 0.0f;
    }
    BrVector3Cross(&pJoint->limits[0].parent, &pJoint->hinge_axis, &pJoint->limits[0].child);
    length = BrVector3Length(&pJoint->limits[0].parent);
    if (length > gFLOAT_00589a5c) {
        inverse_length = gDOUBLE_00589a60 / length;
        pJoint->limits[0].parent.v[0] = pJoint->limits[0].parent.v[0] * inverse_length;
        pJoint->limits[0].parent.v[1] = pJoint->limits[0].parent.v[1] * inverse_length;
        pJoint->limits[0].parent.v[2] = pJoint->limits[0].parent.v[2] * inverse_length;
    } else {
        pJoint->limits[0].parent.v[0] = 1.0f;
        pJoint->limits[0].parent.v[1] = 0.0f;
        pJoint->limits[0].parent.v[2] = 0.0f;
    }
    BrVector3Copy(&pJoint->limits[1].child, &pJoint->limits[0].child);
    BrMatrix34Rotate(&rotation, (br_angle)(pFlap_data->field_0x6 - 0x3ffc), &pJoint->hinge_axis);
    BrMatrix34ApplyV(&pJoint->limits[1].parent, &pJoint->limits[1].child, &rotation);
    BrVector3Scale(&scaled, &pJoint->limits[0].parent, gFLOAT_00589a58);
    BrVector3Add(&pJoint->limits[0].child, &pJoint->limits[0].child, &scaled);
    length = BrVector3Length(&pJoint->limits[0].child);
    if (length > gFLOAT_00589a5c) {
        inverse_length = gDOUBLE_00589a60 / length;
        pJoint->limits[0].child.v[0] = pJoint->limits[0].child.v[0] * inverse_length;
        pJoint->limits[0].child.v[1] = pJoint->limits[0].child.v[1] * inverse_length;
        pJoint->limits[0].child.v[2] = pJoint->limits[0].child.v[2] * inverse_length;
    } else {
        pJoint->limits[0].child.v[0] = 1.0f;
        pJoint->limits[0].child.v[1] = 0.0f;
        pJoint->limits[0].child.v[2] = 0.0f;
    }
    BrVector3Scale(&scaled, &pJoint->limits[1].parent, gFLOAT_00589a58);
    BrVector3Add(&pJoint->limits[1].child, &pJoint->limits[1].child, &scaled);
    length = BrVector3Length(&pJoint->limits[1].child);
    if (length > gFLOAT_00589a5c) {
        inverse_length = gDOUBLE_00589a60 / length;
        pJoint->limits[1].child.v[0] = pJoint->limits[1].child.v[0] * inverse_length;
        pJoint->limits[1].child.v[1] = pJoint->limits[1].child.v[1] * inverse_length;
        pJoint->limits[1].child.v[2] = pJoint->limits[1].child.v[2] * inverse_length;
    } else {
        pJoint->limits[1].child.v[0] = 1.0f;
        pJoint->limits[1].child.v[1] = 0.0f;
        pJoint->limits[1].child.v[2] = 0.0f;
    }
#else
    __asm {
        sub esp, 0x40
        push ebx
        push ebp
        push esi
        push edi
        mov edi, dword ptr [esp+0x5c]
        mov esi, ecx
        mov ecx, dword ptr [esp+0x54]
        mov dword ptr [esp+0x10], edx
        test edi, edi
        mov dword ptr [esi], 4
        jne L00437aee
        mov edi, dword ptr [ecx+8]
        xor eax, eax
        mov ax, word ptr [edx+0xa]
        lea eax, [eax+eax*4]
        lea edi, [edi+eax*8]
L00437aee:
        mov ebx, dword ptr [esp+0x60]
        test ebx, ebx
        jne L00437b05
        mov ebx, dword ptr [ecx+8]
        xor eax, eax
        mov ax, word ptr [edx+0xe]
        lea eax, [eax+eax*4]
        lea ebx, [ebx+eax*8]
L00437b05:
        mov eax, dword ptr [esp+0x64]
        test eax, eax
        je L00437b13
        mov dword ptr [esp+0x60], eax
        jmp L00437b26
L00437b13:
        xor eax, eax
        mov ax, word ptr [edx+0x10]
        lea edx, [eax+eax*4]
        mov eax, dword ptr [ecx+8]
        lea ecx, [eax+edx*8]
        mov dword ptr [esp+0x60], ecx
L00437b26:
        mov edx, dword ptr [edi]
        mov ebp, dword ptr [esp+0x58]
        lea eax, [esi+8]
        mov dword ptr [eax], edx
        mov ecx, dword ptr [edi+4]
        mov dword ptr [esi+0xc], ecx
        mov edx, dword ptr [edi+8]
        mov dword ptr [esi+0x10], edx
        mov ecx, dword ptr [ebp]
        add ecx, 0x2c
        lea edx, [esp+0x14]
        push ecx
        push eax
        push edx
        call BrMatrix34ApplyP
        mov eax, dword ptr [ebp+0x228]
        add esp, 0xc
        lea ecx, [esi+0x14]
        lea edx, [esp+0x14]
        mov eax, dword ptr [eax]
        add eax, 0x2c
        push eax
        call DrMatrix34ApplyLPInverse
        fld dword ptr [ebx]
        fsub dword ptr [edi]
        lea ecx, [esi+0x38]
        fstp dword ptr [ecx]
        fld dword ptr [ebx+4]
        fsub dword ptr [edi+4]
        fld dword ptr [ecx]
        fstp dword ptr [esp+0x5c]
        fstp dword ptr [esi+0x3c]
        fld dword ptr [ebx+8]
        fsub dword ptr [edi+8]
        fld dword ptr [esi+0x3c]
        fld dword ptr [esp+0x5c]
        fxch st(2)
        fst dword ptr [esp+0x54]
        fld st(1)
        fxch st(3)
        fmul dword ptr [esp+0x5c]
        fxch st(3)
        fmul st(0), st(2)
        fxch st(1)
        fst dword ptr [esp+0x5c]
        fmul dword ptr [esp+0x5c]
        fxch st(1)
        faddp st(3), st(0)
        fld dword ptr [esp+0x54]
        fxch st(1)
        faddp st(3), st(0)
        fstp dword ptr [esi+0x40]
        fxch st(1)
        fsqrt
        fstp st(1)
        fcom dword ptr [gFLOAT_00589a5c]
        fnstsw ax
        test ah, 0x41
        jne L00437c02
        fdivr qword ptr [gDOUBLE_00589a60]
        xor ebx, ebx
        mov edx, 0x3f800000
        fld dword ptr [ecx]
        fld dword ptr [esi+0x3c]
        fld dword ptr [esi+0x40]
        fxch st(2)
        fxch st(1)
        fxch st(1)
        fmul st(0), st(3)
        fxch st(1)
        fmul st(0), st(3)
        fxch st(2)
        fmul st(0), st(3)
        fxch st(1)
        fstp dword ptr [ecx]
        fxch st(1)
        fstp dword ptr [esi+0x3c]
        fstp dword ptr [esi+0x40]
        fstp st(0)
        jmp L00437c13
L00437c02:
        mov edx, 0x3f800000
        xor ebx, ebx
        fstp st(0)
        mov dword ptr [ecx], edx
        mov dword ptr [esi+0x3c], ebx
        mov dword ptr [esi+0x40], ebx
L00437c13:
        mov eax, dword ptr [ecx]
        fld dword ptr [ecx]
        fcomp dword ptr [gCrushZero]
        mov dword ptr [esi+0x44], eax
        mov eax, dword ptr [esi+0x3c]
        mov dword ptr [esi+0x48], eax
        mov eax, dword ptr [esi+0x40]
        mov dword ptr [esi+0x4c], eax
        fnstsw ax
        test ah, 0x40
        je L00437c4e
        fld dword ptr [esi+0x40]
        fcomp dword ptr [gCrushZero]
        fnstsw ax
        test ah, 0x40
        je L00437c4e
        mov dword ptr [esi+0x20], edx
        mov dword ptr [esi+0x24], ebx
        mov dword ptr [esi+0x28], ebx
        jmp L00437c5e
L00437c4e:
        fld dword ptr [esi+0x40]
        fchs
        fstp dword ptr [esi+0x20]
        fld dword ptr [ecx]
        fstp dword ptr [esi+0x28]
        mov dword ptr [esi+0x24], ebx
L00437c5e:
        fld dword ptr [esi+0x28]
        fld dword ptr [esi+0x20]
        fst dword ptr [esp+0x5c]
        fld st(1)
        fxch st(1)
        fmul dword ptr [esp+0x5c]
        fxch st(1)
        fmul st(0), st(2)
        faddp st(1), st(0)
        fsqrt
        fstp st(1)
        fcom dword ptr [gFLOAT_00589a5c]
        fnstsw ax
        test ah, 0x41
        jne L00437ca8
        fdivr qword ptr [gDOUBLE_00589a60]
        mov dword ptr [esi+0x24], ebx
        fld dword ptr [esi+0x28]
        fld dword ptr [esi+0x20]
        fmul st(0), st(2)
        fxch st(1)
        fmul st(0), st(2)
        fxch st(1)
        fstp dword ptr [esi+0x20]
        fstp dword ptr [esi+0x28]
        fstp st(0)
        jmp L00437cb3
L00437ca8:
        fstp st(0)
        mov dword ptr [esi+0x20], edx
        mov dword ptr [esi+0x24], ebx
        mov dword ptr [esi+0x28], ebx
L00437cb3:
        fld dword ptr [esi+0x28]
        fld dword ptr [esi+0x3c]
        fld dword ptr [esi+0x24]
        fld dword ptr [esi+0x40]
        fld dword ptr [esi+0x20]
        fxch st(3)
        fmulp st(4), st(0)
        fld dword ptr [esi+0x28]
        fxch st(1)
        fmulp st(2), st(0)
        fxch st(1)
        fsubp st(3), st(0)
        fxch st(1)
        fmul dword ptr [esi+0x40]
        fxch st(2)
        fstp dword ptr [esi+0x2c]
        fld dword ptr [esi+0x2c]
        fld st(0)
        fmulp st(1), st(0)
        fld dword ptr [ecx]
        fld dword ptr [esi+0x24]
        fld dword ptr [ecx]
        fxch st(2)
        fmulp st(4), st(0)
        fxch st(3)
        fsubp st(4), st(0)
        fmulp st(2), st(0)
        fxch st(2)
        fstp dword ptr [esi+0x30]
        fld dword ptr [esi+0x30]
        fst dword ptr [esp+0x5c]
        fmul dword ptr [esp+0x5c]
        fld dword ptr [esi+0x20]
        fxch st(1)
        faddp st(3), st(0)
        fmul dword ptr [esi+0x3c]
        fsubp st(1), st(0)
        fst dword ptr [esp+0x5c]
        fst dword ptr [esp+0x54]
        fmul dword ptr [esp+0x54]
        fld dword ptr [esp+0x5c]
        fxch st(1)
        faddp st(2), st(0)
        fstp dword ptr [esi+0x34]
        fsqrt
        fcom dword ptr [gFLOAT_00589a5c]
        fnstsw ax
        test ah, 0x41
        jne L00437d60
        fdivr qword ptr [gDOUBLE_00589a60]
        fld dword ptr [esi+0x30]
        fld dword ptr [esi+0x2c]
        fld st(2)
        fmul dword ptr [esp+0x5c]
        fxch st(1)
        fmul st(0), st(3)
        fxch st(2)
        fmul st(0), st(3)
        fxch st(2)
        fstp dword ptr [esi+0x2c]
        fxch st(1)
        fstp dword ptr [esi+0x30]
        fstp dword ptr [esi+0x34]
        fstp st(0)
        jmp L00437d6b
L00437d60:
        fstp st(0)
        mov dword ptr [esi+0x2c], edx
        mov dword ptr [esi+0x30], ebx
        mov dword ptr [esi+0x34], ebx
L00437d6b:
        mov eax, dword ptr [esp+0x60]
        mov dword ptr [esi+0x50], 2
        fld dword ptr [eax]
        fsub dword ptr [edi]
        fld dword ptr [esi+0x40]
        fld dword ptr [ecx]
        fxch st(2)
        fstp dword ptr [esi+0x5c]
        fld dword ptr [eax+4]
        fsub dword ptr [edi+4]
        fld dword ptr [esi+0x5c]
        fld dword ptr [esi+0x3c]
        fxch st(2)
        fstp dword ptr [esi+0x60]
        fld dword ptr [eax+8]
        fsub dword ptr [edi+8]
        fst dword ptr [esp+0x54]
        fst dword ptr [esi+0x64]
        fld dword ptr [esi+0x64]
        fld dword ptr [esi+0x60]
        fxch st(4)
        fmulp st(4), st(0)
        fxch st(3)
        fstp dword ptr [esp+0x5c]
        fxch st(3)
        fmul dword ptr [esp+0x54]
        fld dword ptr [esi+0x40]
        fxch st(5)
        fmulp st(2), st(0)
        fld dword ptr [ecx]
        fxch st(2)
        faddp st(1), st(0)
        fld dword ptr [esi+0x5c]
        fxch st(1)
        fadd dword ptr [esp+0x5c]
        fxch st(2)
        fmulp st(2), st(0)
        fxch st(1)
        fstp dword ptr [esp+0x14]
        fld dword ptr [esi+0x64]
        fxch st(1)
        fmul dword ptr [ecx]
        fxch st(4)
        fmulp st(2), st(0)
        fxch st(3)
        fstp dword ptr [esp+0x54]
        fadd dword ptr [esp+0x5c]
        fadd dword ptr [esp+0x54]
        fld dword ptr [esi+0x40]
        fmulp st(3), st(0)
        fmul dword ptr [esi+0x3c]
        fld dword ptr [esi+0x60]
        fxch st(1)
        fstp dword ptr [esp+0x18]
        fmul dword ptr [esi+0x3c]
        fld dword ptr [esi+0x40]
        fld dword ptr [esi+0x5c]
        fxch st(2)
        faddp st(4), st(0)
        fxch st(1)
        fsub dword ptr [esp+0x14]
        fxch st(3)
        fadd dword ptr [esp+0x54]
        fxch st(3)
        fstp dword ptr [esp+0x5c]
        fmulp st(2), st(0)
        fxch st(1)
        fstp dword ptr [esp+0x1c]
        fstp st(0)
        fld dword ptr [esi+0x60]
        fsub dword ptr [esp+0x18]
        fld dword ptr [esi+0x64]
        fsub dword ptr [esp+0x1c]
        fld dword ptr [esp+0x5c]
        fxch st(2)
        fst dword ptr [esp+0x54]
        fxch st(1)
        fst dword ptr [esp+0x58]
        fxch st(2)
        fst dword ptr [esp+0x60]
        fxch st(1)
        fst dword ptr [esp+0x64]
        fxch st(1)
        fmul dword ptr [esp+0x60]
        fxch st(1)
        fmul dword ptr [esp+0x64]
        fxch st(2)
        fst dword ptr [esp+0x60]
        fmul dword ptr [esp+0x60]
        fxch st(2)
        faddp st(1), st(0)
        fld dword ptr [esp+0x5c]
        fld dword ptr [esp+0x54]
        fxch st(3)
        faddp st(2), st(0)
        fld dword ptr [esp+0x58]
        fxch st(1)
        fstp dword ptr [esi+0x5c]
        fxch st(1)
        fsqrt
        fst dword ptr [esp+0x60]
        fcom dword ptr [gFLOAT_00589a5c]
        fxch st(2)
        fstp dword ptr [esi+0x60]
        fstp dword ptr [esi+0x64]
        fnstsw ax
        test ah, 0x41
        jne L00437ecf
        fdivr qword ptr [gDOUBLE_00589a60]
        fld dword ptr [esi+0x64]
        fld st(1)
        fld st(2)
        fxch st(1)
        fmul dword ptr [esp+0x5c]
        fxch st(1)
        fmul dword ptr [esp+0x54]
        fxch st(2)
        fmul st(0), st(3)
        fxch st(1)
        fstp dword ptr [esi+0x5c]
        fxch st(1)
        fstp dword ptr [esi+0x60]
        fstp dword ptr [esi+0x64]
        fstp st(0)
        jmp L00437eda
L00437ecf:
        fstp st(0)
        mov dword ptr [esi+0x5c], edx
        mov dword ptr [esi+0x60], ebx
        mov dword ptr [esi+0x64], ebx
L00437eda:
        fld dword ptr [esi+0x64]
        fld dword ptr [esi+0x3c]
        fld dword ptr [esi+0x60]
        fld dword ptr [esi+0x40]
        fld dword ptr [esi+0x5c]
        fld dword ptr [esi+0x40]
        fxch st(4)
        fmulp st(5), st(0)
        fld dword ptr [esi+0x64]
        fxch st(2)
        fmulp st(3), st(0)
        fxch st(2)
        fsubp st(4), st(0)
        fxch st(3)
        fstp dword ptr [esi+0x68]
        fld dword ptr [esi+0x68]
        fld st(0)
        fmulp st(1), st(0)
        fld dword ptr [ecx]
        fxch st(3)
        fmulp st(2), st(0)
        fxch st(2)
        fmulp st(3), st(0)
        fld dword ptr [esi+0x60]
        fxch st(3)
        fsubp st(1), st(0)
        fld dword ptr [ecx]
        fmulp st(3), st(0)
        mov eax, dword ptr [esi+0x5c]
        lea edi, [esi+0x7c]
        fstp dword ptr [esi+0x6c]
        fld dword ptr [esi+0x6c]
        fst dword ptr [esp+0x5c]
        fmul dword ptr [esp+0x5c]
        fld dword ptr [esi+0x5c]
        fxch st(1)
        faddp st(2), st(0)
        fmul dword ptr [esi+0x3c]
        mov dword ptr [edi], eax
        mov eax, dword ptr [esi+0x60]
        fsubp st(2), st(0)
        fxch st(1)
        fst dword ptr [esp+0x5c]
        fst dword ptr [esp+0x54]
        fmul dword ptr [esp+0x54]
        fld dword ptr [esp+0x5c]
        fxch st(1)
        faddp st(2), st(0)
        mov dword ptr [esi+0x80], eax
        mov eax, dword ptr [esi+0x64]
        mov dword ptr [esi+0x84], eax
        fstp dword ptr [esi+0x70]
        fsqrt
        fcom dword ptr [gFLOAT_00589a5c]
        fnstsw ax
        test ah, 0x41
        jne L00437fa3
        fdivr qword ptr [gDOUBLE_00589a60]
        fld dword ptr [esi+0x6c]
        fld dword ptr [esi+0x68]
        fld st(2)
        fmul dword ptr [esp+0x5c]
        fxch st(1)
        fmul st(0), st(3)
        fxch st(2)
        fmul st(0), st(3)
        fxch st(2)
        fstp dword ptr [esi+0x68]
        fxch st(1)
        fstp dword ptr [esi+0x6c]
        fstp dword ptr [esi+0x70]
        fstp st(0)
        jmp L00437fae
L00437fa3:
        fstp st(0)
        mov dword ptr [esi+0x68], edx
        mov dword ptr [esi+0x6c], ebx
        mov dword ptr [esi+0x70], ebx
L00437fae:
        mov edx, dword ptr [esp+0x10]
        push ecx
        lea ecx, [esp+0x24]
        mov ax, word ptr [edx+6]
        sub ax, 0x3ffc
        push eax
        push ecx
        call BrMatrix34Rotate
        add esp, 0xc
        lea edx, [esp+0x20]
        lea ebp, [esi+0x88]
        push edx
        push edi
        push ebp
        call BrMatrix34ApplyV
        fld dword ptr [esi+0x68]
        fld dword ptr [esi+0x6c]
        fld dword ptr [esi+0x70]
        fxch st(2)
        fmul dword ptr [gFLOAT_00589a58]
        fxch st(1)
        fmul dword ptr [gFLOAT_00589a58]
        fxch st(2)
        fmul dword ptr [gFLOAT_00589a58]
        fxch st(1)
        fadd dword ptr [esi+0x5c]
        fxch st(2)
        fadd dword ptr [esi+0x60]
        fxch st(1)
        fadd dword ptr [esi+0x64]
        fxch st(2)
        fst dword ptr [esp+0x68]
        fxch st(1)
        fst dword ptr [esp+0x60]
        fxch st(2)
        fst dword ptr [esp+0x64]
        fxch st(1)
        fst dword ptr [esp+0x6c]
        fxch st(2)
        fst dword ptr [esp+0x70]
        fxch st(2)
        fmul dword ptr [esp+0x6c]
        fxch st(2)
        fmul dword ptr [esp+0x70]
        fxch st(1)
        fst dword ptr [esp+0x6c]
        fmul dword ptr [esp+0x6c]
        fxch st(1)
        faddp st(2), st(0)
        fld dword ptr [esp+0x68]
        fld dword ptr [esp+0x60]
        fxch st(2)
        faddp st(3), st(0)
        fld dword ptr [esp+0x64]
        fxch st(3)
        fsqrt
        add esp, 0xc
        fst dword ptr [esp+0x60]
        fcom dword ptr [gFLOAT_00589a5c]
        fxch st(1)
        fstp dword ptr [esi+0x5c]
        fxch st(1)
        fstp dword ptr [esi+0x60]
        fnstsw ax
        fxch st(1)
        fstp dword ptr [esi+0x64]
        test ah, 0x41
        jne L004380a8
        fdivr qword ptr [gDOUBLE_00589a60]
        fld dword ptr [esi+0x64]
        fld st(1)
        fld st(2)
        fxch st(1)
        fmul dword ptr [esp+0x5c]
        fxch st(1)
        fmul dword ptr [esp+0x54]
        fxch st(2)
        fmul st(0), st(3)
        fxch st(1)
        fstp dword ptr [esi+0x5c]
        fxch st(1)
        fstp dword ptr [esi+0x60]
        fstp dword ptr [esi+0x64]
        fstp st(0)
        jmp L004380b7
L004380a8:
        fstp st(0)
        mov dword ptr [esi+0x5c], 0x3f800000
        mov dword ptr [esi+0x60], ebx
        mov dword ptr [esi+0x64], ebx
L004380b7:
        fld dword ptr [ebp]
        fld dword ptr [esi+0x8c]
        fld dword ptr [esi+0x90]
        fxch st(2)
        fmul dword ptr [gFLOAT_00589a58]
        fxch st(1)
        fmul dword ptr [gFLOAT_00589a58]
        fxch st(2)
        fmul dword ptr [gFLOAT_00589a58]
        fxch st(1)
        fadd dword ptr [edi]
        fxch st(2)
        fadd dword ptr [esi+0x80]
        fxch st(1)
        fadd dword ptr [esi+0x84]
        fld st(2)
        fld st(2)
        fld st(2)
        fxch st(2)
        fst dword ptr [esp+0x5c]
        fxch st(1)
        fst dword ptr [esp+0x54]
        fxch st(1)
        fmul dword ptr [esp+0x5c]
        fxch st(1)
        fmul dword ptr [esp+0x54]
        fxch st(2)
        fst dword ptr [esp+0x5c]
        fmul dword ptr [esp+0x5c]
        fxch st(2)
        faddp st(1), st(0)
        fld st(4)
        fld st(4)
        fxch st(3)
        faddp st(2), st(0)
        fld st(3)
        fxch st(1)
        fstp dword ptr [edi]
        fxch st(1)
        fsqrt
        fst dword ptr [esp+0x5c]
        fcom dword ptr [gFLOAT_00589a5c]
        fxch st(2)
        fstp dword ptr [esi+0x80]
        fstp dword ptr [esi+0x84]
        fnstsw ax
        test ah, 0x41
        jne L00438187
        fdivr qword ptr [gDOUBLE_00589a60]
        fld st(0)
        fld st(1)
        fxch st(1)
        fmul st(0), st(5)
        fxch st(1)
        fmul st(0), st(4)
        fxch st(2)
        fmul st(0), st(3)
        fxch st(1)
        fstp dword ptr [edi]
        fxch st(1)
        fstp dword ptr [esi+0x80]
        fstp dword ptr [esi+0x84]
        fstp st(0)
        fstp st(0)
        fstp st(0)
        pop edi
        pop esi
        pop ebp
        pop ebx
        add esp, 0x40
        ret 0x14
L00438187:
        fstp st(0)
        fstp st(0)
        mov dword ptr [edi], 0x3f800000
        mov dword ptr [esi+0x80], ebx
        mov dword ptr [esi+0x84], ebx
        pop edi
        fstp st(0)
        pop esi
        pop ebp
        fstp st(0)
        pop ebx
        add esp, 0x40
        ret 0x14
    }
#endif
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
