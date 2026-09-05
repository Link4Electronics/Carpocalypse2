#include "oil.h"

#include "finteray.h"
#include "globvars.h"
#include "globvrpb.h"
#include "graphics.h"
#include "loading.h"
#include "network.h"
#include "piping.h"
#include "utility.h"

#include <brender/brender.h>

#include "carpocalypse2_macros.h"
#include "oil.h"

// STUB: CARMA2_HW 0x004a6a10
// GLOBAL: CARMA2_HW 0x0065a398
const char* gOil_pixie_names[1] = {
    "OIL.PIX",
};

// GLOBAL: CARMA2_HW 0x00690c88
br_pixelmap* gOil_pixies[1];

// GLOBAL: CARMA2_HW 0x00690c90
tOil_spill_info gOily_spills[32];

// GLOBAL: CARMA2_HW 0x0074a68c
br_material* oily_material;

// GLOBAL: CARMA2_HW 0x00691710
int gNext_oil_pixie;

// FUNCTION: CARMA2_HW 0x004a6c50
void C2_HOOK_FASTCALL ResetOilSpills(void) {
    int i;

    for (i = 0; i < CARPOCALYPSE2_ASIZE(gOily_spills); i++) {
        gOily_spills[i].actor->render_style = BR_RSTYLE_NONE;
        gOily_spills[i].car = NULL;
        gOily_spills[i].car_actor = NULL;
        gOily_spills[i].stop_time = 0;
    }
}

// FUNCTION: CARMA2_HW 0x004a6c80
void C2_HOOK_FASTCALL QueueOilSpill2(
    tCar_spec* pCar,
    br_actor* pActor,
    br_model* pModel,
    int pSpill_time_offset,
    float pFull_size_min,
    float pFull_size_max,
    float pGrow_size_min,
    float pGrow_size_max,
    float pSize,
    float pField_0x48,
    float pField_0x4c,
    float pField_0x40,
    float pField_0x44,
    br_material* pMaterial,
    int pField_0x50)
{
    int i;
    int slot;
    int oldest;
    tU32 time_low;
    tU32 time_high;
    tU32 smallest_spill_time;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tOil_spill_info, car, 0xc);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tOil_spill_info, spill_time, 0x10);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tOil_spill_info, current_size, 0x1c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tOil_spill_info, field_0x20, 0x20);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tOil_spill_info, grow_rate, 0x24);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tOil_spill_info, field_0x40, 0x40);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tOil_spill_info, field_0x44, 0x44);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tOil_spill_info, field_0x48, 0x48);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tOil_spill_info, field_0x4c, 0x4c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tOil_spill_info, field_0x50, 0x50);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_actor, material, 0x1c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_actor, render_style, 0x20);

    time_low = GetTotalTime();
    time_high = GetTotalTime();

    if (pCar != NULL) {
        for (i = 0; i < CARPOCALYPSE2_ASIZE(gOily_spills); i++) {
            if (gOily_spills[i].car_actor == pActor
                    && time_low < gOily_spills[i].spill_time + 5000) {
                return;
            }
        }
    }

    slot = -1;
    oldest = 0;
    smallest_spill_time = time_high;
    for (i = 0; i < CARPOCALYPSE2_ASIZE(gOily_spills); i++) {
        if (gOily_spills[i].car_actor == NULL) {
            slot = i;
            break;
        }
        if (gOily_spills[i].spill_time < smallest_spill_time) {
            smallest_spill_time = gOily_spills[i].spill_time;
            oldest = i;
        }
    }
    if (slot < 0) {
        slot = oldest;
    }

    gOily_spills[slot].actor->material = pMaterial;
    gOily_spills[slot].car = pCar;
    gOily_spills[slot].car_actor = pActor;
    gOily_spills[slot].car_model = pModel;
    gOily_spills[slot].spill_time = time_low + pSpill_time_offset;
    gOily_spills[slot].field_0x20 = pSize;
    gOily_spills[slot].full_size = SRandomBetween(pFull_size_min, pFull_size_max);
    gOily_spills[slot].grow_rate = SRandomBetween(pGrow_size_min, pGrow_size_max);
    gOily_spills[slot].current_size = pSize;
    gOily_spills[slot].field_0x40 = pField_0x40;
    gOily_spills[slot].field_0x44 = pField_0x44;
    gOily_spills[slot].field_0x48 = pField_0x48;
    gOily_spills[slot].field_0x4c = pField_0x4c;
    gOily_spills[slot].field_0x50 = pField_0x50;
    gOily_spills[slot].actor->render_style = BR_RSTYLE_FACES;
}

// FUNCTION: CARMA2_HW 0x004a6dd0
void C2_HOOK_FASTCALL QueueOilSpill(tCar_spec* pCar) {
    QueueOilSpill2(
        pCar,
        pCar->car_master_actor,
        pCar->car_actor->model,
        500,
        0.35f,
        0.6f,
        3.0e-5f,
        0.0001f,
        0.1f,
        1.5f,
        2.5f,
        0.01f,
        0.15f,
        oily_material,
        0);
}

// FUNCTION: CARMA2_HW 0x004a7460
void C2_HOOK_FASTCALL  SetInitialOilStuff(tOil_spill_info* pOil, br_model* pModel) {

    BrVector2Set(&pModel->vertices[0].p, -pOil->field_0x20, -pOil->field_0x20);
    BrVector2Set(&pModel->vertices[1].p,  pOil->field_0x20, -pOil->field_0x20);
    BrVector2Set(&pModel->vertices[2].p,  pOil->field_0x20,  pOil->field_0x20);
    BrVector2Set(&pModel->vertices[3].p, -pOil->field_0x20,  pOil->field_0x20);
    pOil->actor->render_style = BR_RSTYLE_FACES;
    BrMaterialUpdate(pOil->actor->material, BR_MATU_ALL);
    BrModelUpdate(pModel, BR_MODU_ALL);
}

void C2_HOOK_FASTCALL MungeOilsHeightAeGround(tOil_spill_info* pThe_spill) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tOil_spill_info, actor, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tOil_spill_info, position, 0x34);

    BrVector3Copy(&pThe_spill->actor->t.t.translate.t, &pThe_spill->position);
}

int C2_HOOK_FASTCALL OKToSpillOil(tOil_spill_info* pOil) {
    br_scalar distance;
    br_scalar mr_dotty;
    br_vector3 pos;
    br_vector3 v;
    br_vector3 ray_pos;
    br_vector3 ray_dir;
    br_vector3 normal;
    tCar_spec* car;
    int i;
    int face_count;
    int found_one;
    br_angle angle_to_rotate_by;
    tBounds kev_bounds;
    tFace_ref the_list[10];
    tFace_ref* face_ref;

    car = pOil->car;
    if (car != NULL && car->driver >= 7 && car->damage_units[eDamage_engine].damage_level < 99 && car->damage_units[eDamage_transmission].damage_level < 99) {
        return 0;
    }
    angle_to_rotate_by = IRandomBetween(0, 0xffff);
    BrVector3Set(&kev_bounds.original_bounds.min,
        -pOil->full_size,
        pOil->car_model->bounds.min.v[1] - 1.f / WORLD_SCALE,
        -pOil->full_size);
    BrVector3Set(&kev_bounds.original_bounds.max,
        pOil->full_size,
        pOil->car_model->bounds.max.v[1],
        pOil->full_size);
    BrMatrix34PreRotateY(&pOil->actor->t.t.mat, angle_to_rotate_by);
    kev_bounds.mat = &pOil->car_actor->t.t.mat;
    face_count = FindFacesInBox(&kev_bounds, the_list, CARPOCALYPSE2_ASIZE(the_list), NULL);
    BrVector3Set(&v, .0f, .2f, .0f);
    BrVector3Add(&ray_pos, &pOil->car_actor->t.t.translate.t, &v);
    if (ray_dir.v[1] * ray_dir.v[1] <= 2 * BR_SCALAR_EPSILON) {
        BrVector3Set(&ray_dir, 1.f, 0.f, 0.f);
    } else {
        BrVector3Set(&ray_dir, 0.f, 1.f, 0.f);
    }
    if (face_count == 0) {
        return 0;
    }
    found_one = 0;
    for (i = 0; i < face_count; i++) {
        face_ref = &the_list[i];
        if (!found_one) {
            CheckSingleFace(face_ref, &ray_pos, &ray_dir, &normal, &distance, &pos);
            if (distance < 100.f) {
                found_one = 1;
                BrVector3Copy(&pOil->actor->t.t.look_up.up, &normal);
                BrVector3Set(&v, 0.f, 0.f, 1.f);
                BrVector3Cross(&pOil->actor->t.t.look_up.look, &normal, &v);
                BrVector3Set(&v, -1.f, 0.f, 0.f);
                BrVector3Cross((br_vector3*)pOil->actor->t.t.mat.m[2], &normal, &v);
                BrVector3Copy(&pOil->position, &pos);
                BrMatrix34PreRotateY(&pOil->actor->t.t.mat, angle_to_rotate_by);
            }
        }
    }
    if (!found_one || normal.v[1] < .97f) {
        return 0;
    }
    for (i = 0; i < face_count; i++) {
        face_ref = &the_list[i];
        mr_dotty = BrVector3Dot(&face_ref->normal, &normal);
        if (mr_dotty < .98f && (mr_dotty > .8f || !NormalSideOfPlane(&pOil->actor->t.t.translate.t, &face_ref->normal, face_ref->d))) {
            return 0;
        }
    }
    return 1;
}

void C2_HOOK_FASTCALL MungeOilsHeightAboveGround(tOil_spill_info* pOil) {

    BrVector3Copy(&pOil->actor->t.t.translate.t, &pOil->position);
}

// FUNCTION: CARMA2_HW 0x004a6e50
void C2_HOOK_FASTCALL ProcessOilSpills(tU32 pFrame_period) {
    int i;
    tU32 time;
    br_scalar grow_amount;
    br_scalar this_size;
    br_vector3 v;
    tNet_message* message;

#ifndef CARPOCALYPSE2_MATCHING
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tOil_spill_info, position, 0x34);
    /* FIXME: unconditional assert */
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message, contents.oil_spill.player, 0x1c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message, contents.oil_spill.full_size, 0x20);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message, contents.oil_spill.grow_rate, 0x24);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message, contents.oil_spill.current_size, 0x28);
#endif

    time = GetTotalTime();
    for (i = 0; i < CARPOCALYPSE2_ASIZE(gOily_spills); i++) {
        br_model* the_model;
        tOil_spill_info* oil = &gOily_spills[i];

        if (oil->car_actor == NULL) {
            oil->actor->render_style = BR_RSTYLE_NONE;
        } else {
            the_model = oil->actor->model;
            if (oil->actor->render_style == BR_RSTYLE_NONE
                    && time >= oil->spill_time
                    && (oil->car == NULL || (fabsf(oil->car->collision_info->v.v[0]) < 1 / WORLD_SCALE / 100.f
                                          && fabsf(oil->car->collision_info->v.v[1]) < 1 / WORLD_SCALE / 100.f
                                          && fabsf(oil->car->collision_info->v.v[2]) < 1 / WORLD_SCALE / 100.f))) {
                if (gAction_replay_mode) {
                    SetInitialOilStuff(oil, the_model);
                } else {
                    if (!OKToSpillOil(oil)) {
                        oil->car = NULL;
                        oil->car_actor = NULL;
                    } else {
                        oil->spill_time = time;
                        if (gNext_oil_pixie > 0) { /* CARPOCALYPSE2_ASIZE(gOil_pixies) */
                            gNext_oil_pixie = 0;
                        }
                        if (oil->car == NULL) {
                            BrVector3Copy(&oil->original_pos, &oil->car_actor->t.t.translate.t);
                        } else {
                            BrVector3Copy(&oil->original_pos, &oil->car->pos);
                        }
                        PipeSingleOilSpill(i,
                            &oil->actor->t.t.mat,
                            oil->full_size,
                            oil->grow_rate,
                            oil->spill_time,
                            oil->stop_time,
                            oil->car,
                            &oil->original_pos,
                            oil->actor->material->colour_map);
                        oil->stop_time = 0;
                        SetInitialOilStuff(oil, the_model);
                        if (gNet_mode == eNet_mode_host && oil->car != NULL) {
                            message = NetBuildGuaranteedMessage(31, 0);
                            message->contents.oil_spill.player = NetPlayerFromCar(oil->car)->ID;
                            message->contents.oil_spill.full_size = oil->full_size;
                            message->contents.oil_spill.grow_rate = oil->grow_rate;
                            message->contents.oil_spill.current_size = oil->current_size;
                            NetGuaranteedSendMessageToAllPlayers(gCurrent_net_game, message, NULL);
                        }
                    }
                }
            } else {
                if (oil->actor->render_style == BR_RSTYLE_FACES
                        && (oil->stop_time == 0 || time < oil->stop_time)) {

                    int grow = 1;
                    if (oil->car != NULL) {
                        BrVector3Sub(&v, &oil->original_pos, &oil->car->pos);
                        grow_amount = BrVector3LengthSquared(&v);
                        grow = oil->stop_time == 0 || grow_amount <= 0.2f;
                    }
                    if (grow) {
                        this_size = oil->field_0x20 + (time - oil->spill_time) * oil->grow_rate;
                        if (this_size >= oil->field_0x20) {
                            oil->actor->render_style = BR_RSTYLE_FACES;
                            if (this_size <= oil->full_size) {
                                the_model->vertices[0].p.v[0] = -this_size;
                                the_model->vertices[0].p.v[2] = -this_size;
                                the_model->vertices[1].p.v[0] =  this_size;
                                the_model->vertices[1].p.v[2] = -this_size;
                                the_model->vertices[2].p.v[0] =  this_size;
                                the_model->vertices[2].p.v[2] =  this_size;
                                the_model->vertices[3].p.v[0] = -this_size;
                                the_model->vertices[3].p.v[2] =  this_size;
                                oil->current_size = this_size;
                            } else {
                                the_model->vertices[0].p.v[0] = -oil->full_size;
                                the_model->vertices[0].p.v[2] = -oil->full_size;
                                the_model->vertices[1].p.v[0] =  oil->full_size;
                                the_model->vertices[1].p.v[2] = -oil->full_size;
                                the_model->vertices[2].p.v[0] =  oil->full_size;
                                the_model->vertices[2].p.v[2] =  oil->full_size;
                                the_model->vertices[3].p.v[0] = -oil->full_size;
                                the_model->vertices[3].p.v[2] =  oil->full_size;
                                oil->current_size = oil->full_size;
                            }
                            BrModelUpdate(the_model, BR_MODU_ALL);
                        } else {
                            oil->actor->render_style = BR_RSTYLE_NONE;
                        }
                    } else {
                        oil->stop_time = time;
                        continue;
                    }
                }
            }
        }
        if (oil->actor->render_style == BR_RSTYLE_FACES) {
            MungeOilsHeightAboveGround(oil);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004a74e0
int C2_HOOK_FASTCALL GetOilSpillCount(void) {

    return CARPOCALYPSE2_ASIZE(gOily_spills);
}

// FUNCTION: CARMA2_HW 0x004a74f0
void C2_HOOK_FASTCALL GetOilSpillDetails(int pIndex, br_actor** pActor, br_scalar* pSize) {

    if (gOily_spills[pIndex].car != NULL) {
        *pActor = gOily_spills[pIndex].actor;
        *pSize = gOily_spills[pIndex].full_size;
    } else {
        *pActor = NULL;
    }
}

// FUNCTION: CARMA2_HW 0x004a6e20
void C2_HOOK_FASTCALL MungeIndexedOilsHeightAboveGround(int pIndex) {

    MungeOilsHeightAboveGround(&gOily_spills[pIndex]);
}
// FUNCTION: CARMA2_HW 0x004a6a10
void C2_HOOK_FASTCALL InitOilSpills(void) {
#ifndef CARPOCALYPSE2_MATCHING
    /* stub: no-op for Linux boot */
#else
    NOT_IMPLEMENTED();
#endif
}

// ResetOilSpills

// QueueOilSpill2

// QueueOilSpill

// OKToSpillOil

// MungeOilsHeightAboveGround

// MungeIndexedOilsHeightAboveGround

// SetInitialOilStuff

// ProcessOilSpills

// GetOilSpillCount

// GetOilSpillDetails

// PointInSpill

// GetOilFrictionFactors

// AdjustOilSpill

// ReceivedOilSpill

// RemoveCarsOilSlicks