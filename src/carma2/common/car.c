#include "car.h"

#include "brucetrk.h"
#include "compress.h"
#include "controls.h"
#include "crush.h"
#include "depth.h"
#include "displays.h"
#include "drone.h"
#include "finteray.h"
#include "globvars.h"
#include "globvrkm.h"
#include "globvrpb.h"
#include "graphics.h"
#include "loading.h"
#include "netgame.h"
#include "network.h"
#include "oil.h"
#include "opponent.h"
#include "pedestrn.h"
#include "physics.h"
#include "piping.h"
#include "platform.h"
#include "powerup.h"
#include "powerups.h"
#include "pratcam.h"
#include "racemem.h"
#include "raycast.h"
#include "replay.h"
#include "skidmark.h"
#include "sound.h"
#include "spark.h"
#include "structur.h"
#include "trig.h"
#include "utility.h"
#include "world.h"

#include "brender/brender.h"
#include "brender/br_inline_funcs.h"
#include "carpocalypse2_macros.h"
#include "carpocalypse2_types.h"

#include "c2_stdlib.h"
#include "c2_string.h"

#include "c2_math.h"
#include "car.h"

// GLOBAL: CARMA2_HW 0x005895f0
double gZero = 1.0;

// GLOBAL: CARMA2_HW 0x005896a8
float gF1000 = 1000.0f;

// GLOBAL: CARMA2_HW 0x00589694
float gFneg1000 = -1000.0f;

// GLOBAL: CARMA2_HW 0x0079ecc0
tPhysics_object* gDrivable_on_list[50];

// GLOBAL: CARMA2_HW 0x0079ed88
tU32 gDrivable_on_count;

typedef void (C2_HOOK_FAKE_THISCALL * tControl_car_fn)(tCar_spec* pCar_spec, undefined4 pArg2, float pT);

// GLOBAL: CARMA2_HW 0x0074a5f4
int gOver_shoot;

// GLOBAL: CARMA2_HW 0x0074c7c0
tCar_spec* gActive_car_list[139]; /* FIXME: uncertain about array length */

// GLOBAL: CARMA2_HW 0x0079ef2c
int gNum_cars_and_non_cars;

// GLOBAL: CARMA2_HW 0x0074c9ec
int gNum_active_cars;

// GLOBAL: CARMA2_HW 0x006793f4
static tU32 last_frame_start;

// GLOBAL: CARMA2_HW 0x006793a0
int gFreeze_mechanics;

// GLOBAL: CARMA2_HW 0x0079eda0
tNon_car_spec* gActive_non_car_list[99];

// GLOBAL: CARMA2_HW 0x006793dc
int gNum_active_non_cars;

// GLOBAL: CARMA2_HW 0x00679360
br_scalar gMin_world_y;

// GLOBAL: CARMA2_HW 0x006793e4
tPhysics_object* gUnknown_car_collision_info;

// GLOBAL: CARMA2_HW 0x00679268
br_vector3 gAverage_grid_position;


// GLOBAL: CARMA2_HW 0x0067939c
int gTesting_car_for_sensible_place;

// GLOBAL: CARMA2_HW 0x0065cf78
tWorld_callbacks gWorld_callbacks = {
    ProcessForcesCallback,
    ProcessJointForcesCallback,
    NewFacesListCallback,
    FindFacesInBox,
    PullActorFromWorld,
    StopGroovidelic,
    GetFrictionFromFace,
    NULL,
};


// GLOBAL: CARMA2_HW 0x0058f6e0
tPhysics_callbacks gCar_physics_callbacks = {
    &gWorld_callbacks,
    APTCPreCollision,
    APTCPostCollision,
    APTCChangedObjects,
    APTCActiveHalted,
    APTCPassiveActivated,
    NULL,
};

// GLOBAL: CARMA2_HW 0x006940b0
int gFace_count;

// GLOBAL: CARMA2_HW 0x00744820
tFace_ref gFace_list__car[300];

// GLOBAL: CARMA2_HW 0x00679294
int gCamera_mode;

// GLOBAL: CARMA2_HW 0x006792b8
int gCamera_frozen;

// GLOBAL: CARMA2_HW 0x006792bc
int gOpponent_viewing_mode;

// GLOBAL: CARMA2_HW 0x0058f5fc
int gNet_player_to_view_index = -1;

// GLOBAL: CARMA2_HW 0x00679304
br_angle gPanning_camera_angle;

// GLOBAL: CARMA2_HW 0x006792c8
tSave_camera gSave_camera[2];

// GLOBAL: CARMA2_HW 0x00655f40
br_scalar gCamera_zoom = 0.2f;

// GLOBAL: CARMA2_HW 0x0079efa4
tCamera_key_flags gCamera_key_flags;

// GLOBAL: CARMA2_HW 0x006792f4
undefined2 gUNK_006792f4;

// GLOBAL: CARMA2_HW 0x006792f8
int gUNK_006792f8;

// GLOBAL: CARMA2_HW 0x006792e8
br_vector3 gCamera_pos_before_collide;

// GLOBAL: CARMA2_HW 0x006792e0
br_angle gOld_yaw__car;

// GLOBAL: CARMA2_HW 0x006793e0
int gCamera_has_collided;

// GLOBAL: CARMA2_HW 0x006792e4
br_angle gOld_zoom;

// GLOBAL: CARMA2_HW 0x00679364
int gInTheSea;

// GLOBAL: CARMA2_HW 0x006793d4
int gDouble_pling_water;

// GLOBAL: CARMA2_HW 0x006793a8
tU32 gWild_start;

// GLOBAL: CARMA2_HW 0x006793ac
tU32 gQuite_wild_end;

// GLOBAL: CARMA2_HW 0x006793b0
tU32 gQuite_wild_start;

// GLOBAL: CARMA2_HW 0x006793b4
tU32 gOn_me_wheels_start;

// GLOBAL: CARMA2_HW 0x006793b8
tU32 gWoz_upside_down_at_all;

// GLOBAL: CARMA2_HW 0x00676854
int gStop_opponents_moving = 0;

// GLOBAL: CARMA2_HW 0x006793c0
int gSkid_tag[2];

// GLOBAL: CARMA2_HW 0x006793c8
tCar_spec* gLast_car_to_skid[2];

// GLOBAL: CARMA2_HW 0x00679390
br_vector3 gCar_to_view_original_v;

// GLOBAL: CARMA2_HW 0x006793a4
tU32 gLast_cunning_stunt;

// GLOBAL: CARMA2_HW 0x006792b4
br_actor* gPed_actor;

// GLOBAL: CARMA2_HW 0x0058f5e8
int gCamera_incident_flag;

// GLOBAL: CARMA2_HW 0x0058f630
int gCamera_incident_mode;

// GLOBAL: CARMA2_HW 0x0058f634
int gCamera_incident_direction;

// GLOBAL: CARMA2_HW 0x006792a0
tU32 gIncident_car_data[4];

// GLOBAL: CARMA2_HW 0x006792b0
br_scalar gIncident_towards;

// GLOBAL: CARMA2_HW 0x0067932c
tS32 gIncident_time;

// GLOBAL: CARMA2_HW 0x00679330
tS32 gIncident_camera_stack;

// GLOBAL: CARMA2_HW 0x006792c0
tU32 gSwitch_time;

// GLOBAL: CARMA2_HW 0x0058f6b0
const float gCar_simplification_factor[2][5] = {
    { 20.0f, 3.0f, 1.5f, 0.75f, 0.0f },
    { 50.0f, 5.0f, 2.5f, 1.50f, 0.0f }
};

// GLOBAL: CARMA2_HW 0x0058f660
tControl_car_fn ControlCar[6] = {
    ControlCar1,
    ControlCar2,
    ControlCar3,
    ControlCar4,
    ControlCar5,
    NULL,
};

// GLOBAL: CARMA2_HW 0x0058f678
int gControl__car = 3;

// GLOBAL: CARMA2_HW 0x00763500
int gCunning_stunt_bonus[3];

// GLOBAL: CARMA2_HW 0x0074d1c0
float gArmour_starting_value[100];

// GLOBAL: CARMA2_HW 0x00761f60
float gPower_starting_value[100];

// GLOBAL: CARMA2_HW 0x00761d40
float gOffensive_starting_value[100];

// GLOBAL: CARMA2_HW 0x0075ba20
tFloat_bunch_info gCar_softness;

// GLOBAL: CARMA2_HW 0x0074d600
tFloat_bunch_info gCar_car_damage_multiplier;

// GLOBAL: CARMA2_HW 0x0068b8d0
br_vector3 gZero_v__car;  // FIXME: make const?

static int CheckForWall(br_vector3* start, br_vector3* end) {
    br_vector3 dir;
    br_material* material;
    br_vector3 normal;
    br_scalar d;

    BrVector3Sub(&dir, end, start);
    FindFace(start, &dir, &normal, &d, &material);
    return d <= 1.f;
}

// FUNCTION: CARMA2_HW 0x004105f0
void C2_HOOK_FASTCALL SetUpPanningCamera(tCar_spec* c) {
    br_vector3 pos;
    br_vector3 perp;
    br_vector3 dir;
    br_vector3 normal;
    br_scalar d;
    br_material* material;
    br_vector3 tv;
    br_vector3 tv2;
    br_vector3 left;
    br_vector3 right;
    br_vector3 car_centre;
    br_scalar ts;
    tU32 time;
    tU32 time_step;
    tU32 t;
    tU32 t2;
    br_matrix34* m1;

    m1 = &c->car_master_actor->t.t.mat;
    gCamera_incident_flag = 0;
    ScanCarsPositions(c, &c->pos, 411.678222f, -1, 5000, &pos, &time);
    BrVector3Sub(&dir, &pos, &c->pos);
    if (GetTotalTime() < time) {
        time_step = time - GetTotalTime();
    } else {
        time_step = GetTotalTime() - time;
    }
    time_step *= SRandomBetween(0.8f, 1.5f);
    if (BrVector3LengthSquared(&dir) < 0.01 || time == 0) {
        BrVector3Negate(&dir, (br_vector3*)&m1->m[2][0]);
        BrVector3Copy(&car_centre, &c->pos);
        time_step = 0;
    } else {
        ScanCarsPositions(c, &c->pos, 102.919556f, -1, time_step / 2, &car_centre, &t);
        if (t == 0) {
            BrVector3Copy(&car_centre, &c->pos);
        }
    }
    BrVector3SetFloat(&tv, 0.f, 1.f, 0.f);
    BrVector3Cross(&perp, &tv, &dir);
    ts = BrVector3Length(&perp);
    if (ts < 10.f) {
        return;
    }
    ts = 2.f / ts;
    ts *= SRandomBetween(0.33333334f, 1.f);
    BrVector3Scale(&perp, &perp, ts);
    ts = SRandomBetween(0.33333334f, 1.f) * 2.f;
    BrVector3Set(&tv, 0.f, ts, 0.f);
    BrVector3Add(&tv, &car_centre, &tv);
    BrVector3Add(&left, &tv, &perp);
    BrVector3Sub(&right, &tv, &perp);
    CollideCamera2(&car_centre, &left, NULL, 1, NULL);
    CollideCamera2(&car_centre, &right, NULL, 1, NULL);
    BrVector3Sub(&tv, &left, &car_centre);
    BrVector3Sub(&tv2, &right, &car_centre);
    if (BrVector3LengthSquared(&tv) + SRandomPosNeg(.01f) > BrVector3LengthSquared(&tv2)) {
        BrVector3Copy(&gCamera->t.t.translate.t, &left);
    } else {
        BrVector3Copy(&gCamera->t.t.translate.t, &right);
    }
    if (time != 0) {
        if (CheckForWall(&c->pos, &gCamera->t.t.translate.t)) {
            ScanCarsPositions(c, &c->pos, 10000.f, -1, 1000, &tv, &t2);
            CollideCamera2(&tv, &gCamera->t.t.translate.t, NULL, 1, NULL);
        }
    }
    if (time != 0) {
        if (CheckForWall(&pos, &gCamera->t.t.translate.t)) {
            time_step = time_step / 16;
            BrVector3Copy(&tv, &car_centre);
            do {
                ScanCarsPositions(c, &tv, 10000.f, abs((tS32)t - (tS32)GetTotalTime()), time_step, &tv2, &t2);
                t += (tS32)(ARGetReplayDirection() ? 1 : -1) * time_step;
                BrVector3Copy(&tv, &tv2);
            } while (!CheckForWall(&tv, &gCamera->t.t.translate.t) && t < GetTotalTime() + 5000);
            gSwitch_time = t;
            return;
        }
    }
    if (time == 0) {
        time = 5000;
    }
    gSwitch_time = time;
}

// FUNCTION: CARMA2_HW 0x004122b0
int C2_HOOK_FASTCALL CollideCamera2(br_vector3* car_pos, br_vector3* cam_pos, br_vector3* old_camera_pos, int manual_move, tPhysics_object *collision_info) {
    int i;
    br_vector3 cam_hither_pos;
    br_vector3 car_coll_pos;
    br_vector3 dir;
    br_scalar ts;
    br_scalar dist;
    br_scalar hither;
    tBounds bnds;
    br_actor* actor;
    br_matrix34 mat;
    br_vector3 tv;
    tFace_ref face_list[6];
    br_scalar face_dots[6];
    int count_min_dots;
    int index_min_dots[3];
    int loop_done;
    int count;
    br_vector3 nor;
    br_vector3 delta;

    hither = ((br_camera*)gCamera->type_data)->hither_z * 3.0f;
    gCamera_has_collided = 0;
    loop_done = 0;
    for (;;) {
        br_vector3 new_cam_pos;
        br_vector3 tv2;
        br_vector3 p1;
        br_vector3 p2;
        br_vector3 nor_mod;
        br_material* material;
        br_scalar cos_coll;
        br_scalar delta_hither;
        br_scalar nor_factor;
        int i;

        BrVector3Sub(&delta, cam_pos, car_pos);
        dist = BrVector3Length(&delta);
        BrVector3Scale(&delta, &delta, 1.25f);
        ActorFindFace(car_pos, &delta, gTrack_actor, &nor, &ts, &material, &actor);
        if (ts >= 1.0) {
            BrVector3Copy(&new_cam_pos, cam_pos);
        } else {
            gCamera_has_collided = 1;
            BrVector3Scale(&tv, &delta, ts);
            BrVector3Add(&new_cam_pos, car_pos, &tv);
        }
        BrVector3Normalise(&dir, &delta);

        BrVector3Scale(&tv2, &dir, -hither);
        BrVector3Add(&cam_hither_pos, &new_cam_pos, &tv2);
        BrVector3Add(&car_coll_pos, car_pos, &delta);

        for (i = 0; i < gNum_active_non_cars; i++) {
            tNon_car_spec* non_car = gActive_non_car_list[i];

            if (non_car->flags & 0x80000) {
                br_scalar factor;
                br_vector3 temp_normal;

                DrMatrix34ApplyLPInverse(&p1, &cam_hither_pos, &non_car->collision_info->actor->t.t.mat);
                DrMatrix34ApplyLPInverse(&p2, &car_coll_pos, &non_car->collision_info->actor->t.t.mat);
                if (ShapeRayCast(&p1, &p2, non_car->collision_info->shape, &new_cam_pos, &factor, &temp_normal)) {
                    gCamera_has_collided = 1;
                    DrMatrix34ApplyLPInverse(&p1, car_pos, &non_car->collision_info->actor->t.t.mat);
                    DrMatrix34ApplyLPInverse(&p2, &car_coll_pos, &non_car->collision_info->actor->t.t.mat);
                    if (ShapeRayCast(&p1, &p2, non_car->collision_info->shape, &new_cam_pos, &factor, &temp_normal) && factor < ts) {
                        BrVector3Copy(&nor, &temp_normal);
                        actor = NULL;
                        BrMatrix34ApplyP(&tv, &new_cam_pos, &non_car->collision_info->actor->t.t.mat);
                        BrVector3Copy(&new_cam_pos, &tv);
                    }
                }
            }
        }
        if (ts > 1.f) {
            break;
        }
        cos_coll = BrVector3Dot(&nor, &delta);
        if (cos_coll <= 0.f) {
            cos_coll = -cos_coll;
        } else {
            BrVector3Negate(&nor, &nor);
        }
        if (loop_done) {
            break;
        }
        delta_hither = hither + cos_coll / 1.25f - ts * cos_coll;
        if (delta_hither <= 0.f) {
            break;
        }
        BrVector3Copy(&nor_mod, &nor);
        nor_mod.v[1] += nor.v[1] >= 0.7f ? 3.f : -3.f;
        nor_factor = fabsf(nor.v[1]) * delta_hither / BrVector3Length(&nor_mod);
        BrVector3Scale(&tv, &nor_mod, nor_factor);
        BrVector3Accumulate(cam_pos, &tv);

        BrVector3Sub(&delta, cam_pos, car_pos);
        BrVector3Normalise(&delta, &delta);
        BrVector3Scale(&delta, &delta, dist);
        BrVector3Add(cam_pos, car_pos, &delta);
        loop_done = 1;
    }
    if (ts <= 1.f) {
        br_scalar dt;
        br_scalar l;

        gCamera_has_collided = 1;
        if (actor != NULL && actor->identifier != NULL && actor->identifier[0] == '&') {
            br_vector3 tv;

            BrMatrix34ApplyV(&tv, &nor, &actor->t.t.mat);
            BrVector3Copy(&nor, &tv);
        }
        dt = -(hither / BrVector3Dot(&nor, &delta));
        ts -= dt;
        if (ts * dist < 1.f) {
            ts += dt - 0.001f;
            if (ts >= 1.f / dist) {
                ts = 1.f / dist;
            }
        }
        if (ts > .8f) {
            ts = .8f;
        }
        BrVector3Scale(&delta, &delta, ts);
        l = BrVector3LengthSquared(&delta);
        BrVector3Add(cam_pos, car_pos, &delta);
        if ((float)sqrt(l) < gMin_camera_car_distance && !loop_done) {
            br_scalar a;
            br_scalar b;
            br_scalar discr;

            BrVector3Scale(&tv, &nor, -nor.v[1]);
            tv.v[1] += 1.f;
            if (gProgram_state.current_car.car_master_actor->t.t.mat.m[1][1] < 0.f) {
                BrVector3Negate(&tv, &tv);
            }
            a = BrVector3LengthSquared(&tv);
            b = BrVector3Dot(&tv, &delta);
            discr = CARPOCALYPSE2_SQR(b) - 4.f * a * (l - CARPOCALYPSE2_SQR(gMin_camera_car_distance));
            if (discr >= 0.f && a != 0.f) {
                br_material* material;
                br_vector3 tv2;
                br_vector3 tv3;
                br_scalar f;
                br_scalar dot;

                f = (sqrtf(discr) - b) / (2 * a);
                BrVector3Scale(&tv, &tv, f);
                FindFace(cam_pos, &tv, &nor, &ts, &material);
                if (ts < 1.f) {
                    BrVector3Scale(&tv, &tv, ts);
                }
                BrVector3Copy(&tv2, &delta);
                tv2.v[1] = 0.f;
                BrVector3Normalise(&tv3, &tv2);
                BrVector3Accumulate(&delta, &tv);
                dot = BrVector3Dot(&tv3, &delta);
                if (dot < .03f && !gAction_replay_mode) {
                    br_scalar dot2;

                    BrVector3Normalise(&tv, &tv);
                    dot2 = BrVector3Dot(&tv3, &tv);
                    if (dot2 < -.03f) {
                        a = (.03f - dot) / dot2;
                        BrVector3Scale(&tv, &tv, a);
                        BrVector3Accumulate(&delta, &tv);
                    }
                }
            }
            BrVector3Add(cam_pos, car_pos, &delta);
        }
    }
    bnds.mat = &mat;
    BrMatrix34Identity(&mat);
    BrVector3Set(&tv, hither, hither, hither);
    BrVector3Sub(&bnds.original_bounds.min, cam_pos, &tv);
    BrVector3Add(&bnds.original_bounds.min, cam_pos, &tv);
    count = FindFacesInBox(&bnds, face_list, CARPOCALYPSE2_ASIZE(face_list), NULL);
    for (i = 0; i < count; i++) {
        tFace_ref* fr = &face_list[i];
        if ((fr->material->flags & BR_MATF_TWO_SIDED) && BrVector3Dot(&fr->normal, &delta) > 0.f) {
            BrVector3Negate(&fr->normal, &fr->normal);
        }
        BrVector3Sub(&tv, cam_pos, &fr->v[0]);
        face_dots[i] = BrVector3Dot(&tv, &fr->normal);
    }

    for (i = 0; i < CARPOCALYPSE2_ASIZE(index_min_dots); i++) {
        int j;
        float min_dot = 100.f;
        int index_min_dot;

        for (j = 0; j < count; j++) {
            if (face_dots[j] > 0.f && face_dots[j] < min_dot) {
                min_dot = face_dots[j];
                index_min_dot = j;
            }
        }
        if (min_dot == 100.f) {
            break;
        }
        index_min_dots[i] = index_min_dot;
        face_dots[index_min_dot] = -100.f;
    }
    count_min_dots = i;
    if (count_min_dots >= 1) {
        br_scalar d;

        BrVector3Sub(&tv, cam_pos, &face_list[index_min_dots[0]].v[0]);
        d = BrVector3Dot(&face_list[index_min_dots[0]].normal, &tv);
        if (d < hither) {
            d = hither - d;
            BrVector3Scale(&tv, &face_list[index_min_dots[0]].normal, d);
            BrVector3Accumulate(cam_pos, &tv);
        }
        if (count_min_dots >= 2) {
            int o_i = 1;

            d = BrVector3Dot(&face_list[index_min_dots[0]].normal, &face_list[index_min_dots[1]].normal);
            if (d > .95f && count_min_dots >= 3) {
                o_i = 2;
                d = BrVector3Dot(&face_list[index_min_dots[0]].normal, &face_list[index_min_dots[2]].normal);
            }
            if (d <= .95f) {
                br_vector3 tv;

                BrVector3Sub(&tv, cam_pos, &face_list[index_min_dots[o_i]].v[0]);
                d = BrVector3Dot(&face_list[index_min_dots[o_i]].normal, &tv);
                if (d < hither) {
                    br_scalar a;

                    a = BrVector3Dot(&face_list[index_min_dots[0]].normal, &face_list[index_min_dots[o_i]].normal);
                    BrVector3Scale(&tv, &face_list[index_min_dots[0]].normal, a);
                    BrVector3Sub(&face_list[index_min_dots[o_i]].normal, &face_list[index_min_dots[o_i]].normal, &tv);
                    BrVector3Scale(&tv, &face_list[index_min_dots[0]].normal, hither - d);
                    BrVector3Accumulate(cam_pos, &tv);
                }
            }
            if (count_min_dots >= 3) {
                br_vector3 tv;
                br_scalar d;

                BrVector3Sub(&tv, cam_pos, &face_list[index_min_dots[o_i]].v[0]);
                d = BrVector3Dot(&face_list[index_min_dots[o_i]].normal, &tv);
                if (d < hither && d >= 0.f) {
                    BrVector3Cross(&tv, &face_list[index_min_dots[0]].normal, &face_list[index_min_dots[o_i]].normal);
                    if (BrVector3Dot(&tv, &face_list[index_min_dots[2]].normal) < 0.f) {
                        BrVector3Negate(&tv, &tv);
                    }
                    if (d > .5f) {
                        br_scalar a;

                        a = (hither - d) / (BrVector3Dot(&face_list[index_min_dots[2]].normal, &tv));
                        BrVector3Scale(&tv, &tv, a);
                        BrVector3Accumulate(cam_pos, &tv);
                    }
                }
            }
        }
        if (collision_info != NULL && gAction_replay_camera_mode != kActionReplayCameraMode_Manual) {
            for (;;) {
                br_vector3 p1;
                br_vector3 p2;
                br_vector3 new_cam_pos;
                br_vector3 temp_normal;
                br_vector3 pos_screen;
                int num_collisions;

                BrVector3Sub(&delta, cam_pos, car_pos);
                BrVector3Normalise(&delta, &delta);
                BrVector3Scale(&delta, &delta, hither);
                BrVector3Sub(&pos_screen, cam_pos, &delta);
                DrMatrix34ApplyLPInverse(&p1, cam_pos, &collision_info->actor->t.t.mat);
                DrMatrix34ApplyLPInverse(&p2, &pos_screen, &collision_info->actor->t.t.mat);
                num_collisions = ShapeRayCast(&p1, &p2, collision_info->shape, &new_cam_pos, &ts, &temp_normal);
                if (num_collisions == 0) {
                    break;
                }
                if (num_collisions > 0) {
                    BrMatrix34ApplyP(cam_pos, &new_cam_pos, &collision_info->actor->t.t.mat);
                }
                BrVector3Accumulate(cam_pos, &delta);
            }
        }
    }
    return loop_done;
}

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x0041f4f0
void C2_HOOK_FAKE_THISCALL FlyCar(tCar_spec* c, undefined4 pArg2, br_scalar dt) {

    CARPOCALYPSE2_THISCALL_UNUSED(pArg2);

    NOT_IMPLEMENTED();
}
#pragma auto_inline(on)

// FUNCTION: CARMA2_HW 0x0041fbe0
float C2_HOOK_FASTCALL GetCarOverallBoundsMinY(tCar_spec* pCar) {
    float rear_min;
    float front_min;
    float min_value;

    rear_min = (pCar->wpos[0].v[1] - pCar->susp_height[0]) / WORLD_SCALE;
    front_min = (pCar->wpos[2].v[1] - pCar->susp_height[1]) / WORLD_SCALE;
    min_value = pCar->collision_info->bb1.min.v[1];

    min_value = MIN(min_value, rear_min);
    min_value = MIN(min_value, front_min);
    return min_value;
}

// FUNCTION: CARMA2_HW 0x0041fc60
void C2_HOOK_FAKE_THISCALL SetCarSuspGiveAndHeight(tCar_spec* pCar, undefined4 pArg2, float pFront_give_factor, float pRear_give_factor, float pDamping_factor, float pExtra_front_height, float pExtra_rear_height) {
    float ratio;
    float front_give;
    float rear_give;
    float damping;

#define UNK_SUSPENION_FACTOR 5.0f

    front_give = pCar->susp_give * pFront_give_factor * WORLD_SCALE;
    rear_give = pCar->steerable_suspension_give * pRear_give_factor * WORLD_SCALE;
    damping = pCar->damping * pDamping_factor;
    ratio = fabsf((pCar->wpos[0].v[2] - pCar->centre_of_mass_world_scale.v[2]) / (pCar->wpos[2].v[2] - pCar->centre_of_mass_world_scale.v[2]));
    pCar->sk[0] = (pCar->collision_info->M / (ratio + 1.0f)) * UNK_SUSPENION_FACTOR / rear_give;
    pCar->sb[0] = (pCar->collision_info->M / (ratio + 1.0f)) * sqrtf(UNK_SUSPENION_FACTOR) / sqrtf(rear_give);
    ratio = 1.0f / ratio;
    pCar->sk[1] = (pCar->collision_info->M / (ratio + 1.0f)) * UNK_SUSPENION_FACTOR / front_give;
    pCar->sb[1] = (pCar->collision_info->M / (ratio + 1.0f)) * sqrtf(UNK_SUSPENION_FACTOR) / sqrtf(front_give);

    pCar->sb[0] *= damping;
    pCar->sb[1] *= damping;
    pCar->susp_height[0] = pCar->ride_height + rear_give + pExtra_rear_height;
    pCar->susp_height[1] = pCar->ride_height + front_give + pExtra_front_height;

    pCar->collision_info->bb2.min.v[1] = GetCarOverallBoundsMinY(pCar);

#undef UNK_SUSPENION_FACTOR
}

// FUNCTION: CARMA2_HW 0x0041fe50
int C2_HOOK_FASTCALL TestForCarInSensiblePlace(tCar_spec *pCar_spec, br_vector3 *pVec3) {
    int r;

    if (!gProgram_state.racing) {
        return 1;
    }
    gTesting_car_for_sensible_place = 1;
    r = TestForObjectInSensiblePlace(pCar_spec->collision_info,
        gList_collision_infos,
        pVec3,
        &gWorld_callbacks);
    gTesting_car_for_sensible_place = 0;
    return r;
}

// FUNCTION: CARMA2_HW 0x004104b0
void C2_HOOK_FASTCALL PanningExternalCamera(tCar_spec* c, tU32 pTime) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004121b0
void C2_HOOK_FASTCALL PangToCamera(br_vector3* pPos, br_matrix34* pMat)
{
    br_camera* camera;
    br_scalar dx, dz, d, nx, nz;
    br_angle pitch;

    camera = (br_camera*)gCamera->type_data;

    dx = pPos->v[0] - pMat->m[3][0];
    dz = pPos->v[2] - pMat->m[3][2];
    d = sqrt(dx * dx + dz * dz);

    if (d >= BR_SCALAR_EPSILON * 2)
    {
        nx = dx / d;
        nz = dz / d;
    }
    else
    {
        nx = 1.0f;
        nz = 0.0f;
    }

    pMat->m[0][0] = -nz;
    pMat->m[0][1] = 0.0f;
    pMat->m[0][2] = -nx;
    pMat->m[1][0] = 0.0f;
    pMat->m[1][1] = 1.0f;
    pMat->m[1][2] = 0.0f;
    pMat->m[2][0] = -nx;
    pMat->m[2][1] = 0.0f;
    pMat->m[2][2] = -nz;

    pitch = BR_ATAN2(pMat->m[3][1] - pPos->v[1],
        sqrt((pMat->m[3][0] - pPos->v[2]) * (pMat->m[3][0] - pPos->v[2]) +
             (pMat->m[3][2] - pPos->v[0]) * (pMat->m[3][2] - pPos->v[0])));

    BrMatrix34PostRotateY(pMat, (br_angle)(camera->field_of_view / 5) - pitch);
}

// FUNCTION: CARMA2_HW 0x0040f790
int C2_HOOK_FASTCALL IncidentCam(tCar_spec* c, tU32 pTime) {
    br_matrix34* m2;
    br_vector3 old_cam_pos;
    br_vector3 cam1;
    br_vector3 camP;
    br_vector3 tv;
    br_vector3 perp;
    br_vector3 vertical;
    br_vector3 scan_pos;
    br_vector3 left;
    br_vector3 right;
    br_vector3 scaled;
    br_vector3 dir;
    br_vector3 nor;
    br_material* material;
    br_scalar d;
    br_scalar ts;
    tU32 time_returned;
    tU32 scan_data[4];
    tS32 mode2;
    br_scalar towards2;
    tS32 time2;
    int scan_index = 0;
    tS32 scan_time;
    tCar_spec* car2;
    br_actor* ped_actor;
    br_actor* murderer;
    tU8 col_x;
    tU8 col_z;
    int removed = 0;

    m2 = &gCamera->t.t.mat;
    gPed_actor = NULL;
    if (gCamera_incident_mode == 3) {
        if (!GetPipeCarStateAtTime(-1, &gCamera_incident_mode, &gIncident_towards, gIncident_car_data, &gIncident_time)) {
            gCamera_incident_mode = 3;
        } else {
            if (gIncident_time < 0 && ARGetReplayDirection() >= 0) {
                gCamera_incident_mode = 3;
            } else {
                scan_time = abs(gIncident_time);
                if (scan_time > 2500) {
                    gCamera_incident_mode = 3;
                } else {
                    if (GetPipeCarStateAtTime(scan_time, &mode2, &towards2, scan_data, &time2)) {
                        do {
                            scan_index++;
                            if (scan_index > 10) {
                                break;
                            }
                            if (abs(time2) > 3500) {
                                break;
                            }
                            if (mode2 < gCamera_incident_mode
                                || (mode2 == gCamera_incident_mode && gIncident_towards <= towards2)) {
                                gCamera_incident_mode = mode2;
                                gIncident_car_data[0] = scan_data[0];
                                gIncident_car_data[1] = scan_data[1];
                                gIncident_car_data[2] = scan_data[2];
                                gIncident_car_data[3] = scan_data[3];
                                gIncident_towards = towards2;
                                gIncident_time = time2;
                            }
                        } while (GetPipeCarStateAtTime(abs(time2), &mode2, &towards2, scan_data, &time2));
                    }
                    if (abs(gIncident_time) > 2500) {
                        gCamera_incident_mode = 3;
                    } else if (gCamera_incident_mode == 2 && gIncident_towards < 0.1f) {
                        gCamera_incident_mode = 3;
                    } else {
                        if (gCamera_incident_mode == 2) {
                            ScanCarsPositions(c, &c->pos, 100000.f, -1, abs(time2), &scan_pos, &time_returned);
                            if (time_returned != 0 && Vector3DistanceSquared(&scan_pos, &c->pos) <= 102.91955471539592) {
                                BrVector3Sub(&tv, &scan_pos, (br_vector3*)gIncident_car_data);
                                tv.v[1] = scan_pos.v[1] + 2.f - ((br_vector3*)gIncident_car_data)->v[1];
                                BrVector3Normalise(&tv, &tv);
                                BrVector3Scale(&tv, &tv, 2.f);
                                BrVector3Set(&vertical, 0.f, 1.f, 0.f);
                                BrVector3Cross(&perp, &vertical, &tv);
                                BrVector3Add(&left, &scan_pos, &tv);
                                BrVector3Add(&left, &left, &perp);
                                left.v[1] += 2.f;
                                BrVector3Add(&right, &scan_pos, &tv);
                                BrVector3Sub(&right, &right, &perp);
                                right.v[1] += 2.f;
                                CollideCamera2(&scan_pos, &left, NULL, 1, NULL);
                                CollideCamera2(&scan_pos, &right, NULL, 1, NULL);
                                if (Vector3DistanceSquared(&left, &scan_pos) > Vector3DistanceSquared(&right, &scan_pos)) {
                                    BrVector3Copy(&gCamera->t.t.translate.t, &left);
                                } else {
                                    BrVector3Copy(&gCamera->t.t.translate.t, &right);
                                }
                            } else {
                                gCamera_incident_mode = 3;
                            }
                        }
                        gIncident_time += (tS32)GetTotalTime();
                    }
                }
            }
        }
    } else {
        gCamera_incident_flag = 1;
    }
    switch (gCamera_incident_mode) {
        case 0:
            BrVector3Copy(&old_cam_pos, &gCamera->t.t.translate.t);
            ped_actor = (br_actor*)gIncident_car_data[0];
            murderer = (br_actor*)gIncident_car_data[1];
            gPed_actor = ped_actor;
            XZToColumnXZ(&col_x, &col_z, murderer->t.t.translate.t.v[0], murderer->t.t.translate.t.v[2], &gProgram_state.track_spec);
            if (gProgram_state.track_spec.columns[col_z][col_x].actor_0x0 == murderer->parent) {
                BrActorRemove(murderer);
                removed = 1;
            }
            BrVector3Copy(&cam1, &ped_actor->t.t.translate.t);
            gIncident_car_data[1] = (tU32)c->car_master_actor;
            if (c->car_master_actor != NULL) {
                BrVector3Copy(&camP, &c->pos);
            } else if (c->car_master_actor->model != NULL) {
                BrVector3Sub(&scaled, &c->car_master_actor->model->bounds.max, &c->car_master_actor->model->bounds.min);
                BrVector3Scale(&scaled, &scaled, 0.5f);
                BrMatrix34ApplyP(&camP, &scaled, &c->car_master_actor->t.t.mat);
            } else {
                BrVector3Copy(&camP, &c->car_master_actor->t.t.translate.t);
            }
            if ((tU32)GetTotalTime() < (tU32)gIncident_time || ARGetReplayDirection() != 1) {
                BrVector3Sub(&tv, &cam1, &camP);
                tv.v[1] = 0.f;
                BrVector3Normalise(&tv, &tv);
                BrVector3Set(&vertical, 0.f, 0.4f, 0.f);
                BrVector3Cross(&perp, &tv, &vertical);
                if (gCamera_incident_direction) {
                    BrVector3Negate(&perp, &perp);
                }
                if (ARReplayPlaying()) {
                    BrVector3Accumulate(&perp, &tv);
                }
                BrVector3Add(&gCamera->t.t.translate.t, &cam1, &perp);
                gCamera->t.t.translate.t.v[1] += 0.25f;
                CollideCamera2(&cam1, &gCamera->t.t.translate.t, NULL, 1, NULL);
            }
            PangToCamera(&cam1, m2);
            ts = Vector3DistanceSquared(&gCamera->t.t.translate.t, &((br_actor*)gIncident_car_data[1])->t.t.translate.t);
            if (abs((tS32)GetTotalTime() - gIncident_time) > 2500) {
                gCamera_incident_mode = 3;
            }
            if (ARReplayPlaying() ? (tU32)(gIncident_time + 300) < GetTotalTime() : (tU32)GetTotalTime() < (tU32)(gIncident_time - 600)) {
                if (ts > 25.f) {
                    gCamera_incident_mode = 3;
                } else {
                    BrVector3Sub(&dir, &gCamera->t.t.translate.t, &((br_actor*)gIncident_car_data[1])->t.t.translate.t);
                    FindFace(&((br_actor*)gIncident_car_data[1])->t.t.translate.t, &dir, &nor, &d, &material);
                    if (d <= 1.f) {
                        gCamera_incident_mode = 3;
                    }
                }
            }
            if (removed) {
                XZToColumnXZ(&col_x, &col_z, ((br_actor*)gIncident_car_data[1])->t.t.translate.t.v[0], ((br_actor*)gIncident_car_data[1])->t.t.translate.t.v[2], &gProgram_state.track_spec);
                BrActorAdd(&gProgram_state.track_spec.columns[col_z][col_x].actor_0x0, (br_actor*)gIncident_car_data[1]);
            }
            BrVector3Sub(&tv, &cam1, &gCamera->t.t.translate.t);
            if (tv.v[0] * tv.v[0] + tv.v[2] * tv.v[2] < 0.0225) {
                BrVector3Copy(&gCamera->t.t.translate.t, &old_cam_pos);
                gPed_actor = NULL;
                return 0;
            }
            break;
        case 1:
            car2 = (tCar_spec*)gIncident_car_data[0];
            BrVector3Sub(&tv, &car2->pos, &c->pos);
            tv.v[1] = 0.f;
            BrVector3Normalise(&tv, &tv);
            BrVector3Scale(&tv, &tv, 2.f);
            BrVector3Add(&gCamera->t.t.translate.t, &car2->pos, &tv);
            gCamera->t.t.translate.t.v[1] += 1.f;
            CollideCamera2(&car2->pos, &gCamera->t.t.translate.t, NULL, 1, NULL);
            PangToCamera(&car2->pos, m2);
            ts = Vector3DistanceSquared(&gCamera->t.t.translate.t, &c->pos);
            if (fabsf((float)((tS32)GetTotalTime() - gIncident_time)) > 2500.f) {
                gCamera_incident_mode = 3;
            }
            if (ARReplayPlaying() ? (tU32)gIncident_time < GetTotalTime() : GetTotalTime() < (tU32)gIncident_time) {
                if (ts > 25.f) {
                    gCamera_incident_mode = 3;
                } else {
                    BrVector3Sub(&dir, &gCamera->t.t.translate.t, &c->pos);
                    FindFace(&c->pos, &dir, &nor, &d, &material);
                    if (d <= 1.f) {
                        gCamera_incident_mode = 3;
                    }
                }
            }
            break;
        case 2:
            PangToCamera(&c->pos, m2);
            ts = Vector3DistanceSquared(&gCamera->t.t.translate.t, &c->pos);
            if (fabsf((float)((tS32)GetTotalTime() - gIncident_time)) > 2500.f) {
                gCamera_incident_mode = 3;
            }
            if (ARReplayPlaying() ? (tU32)gIncident_time < GetTotalTime() : GetTotalTime() < (tU32)gIncident_time) {
                if (ts > 25.f) {
                    gCamera_incident_mode = 3;
                } else {
                    BrVector3Sub(&dir, &gCamera->t.t.translate.t, &c->pos);
                    FindFace(&c->pos, &dir, &nor, &d, &material);
                    if (d <= 1.f) {
                        gCamera_incident_mode = 3;
                    }
                }
            }
            break;
        default:
            gCamera_incident_mode = 3;
            break;
        }
        if (gCamera_incident_mode != 3) {
            return 1;
        }
        if (gIncident_camera_stack > 1) {
            SetUpPanningCamera(c);
            return 0;
        }
        gIncident_camera_stack++;
        if (IncidentCam(c, pTime)) {
            gIncident_camera_stack--;
            return 1;
        }
        gIncident_camera_stack--;
        return 0;
}

// FUNCTION: CARMA2_HW 0x004ff530
void C2_HOOK_FASTCALL ResetCarSpecialVolume(tPhysics_object* pCollision_info) {
    br_vector3 cast_v;
    br_vector3 norm;
    br_scalar t;
    int id_len;
    char* mat_id;
    tSpecial_volume* new_special_volume;
    br_material* material;

    new_special_volume = NULL;
    BrVector3Set(&cast_v, 0.f, 200.f, 0.f);
    DisablePlingMaterials();
    FindFace(&pCollision_info->actor->t.t.translate.t, &cast_v, &norm, &t, &material);
    EnablePlingMaterials();
    if (t < 100.0f && material != NULL) {
        mat_id = material->identifier;
        if (mat_id != NULL) {
            id_len = strlen(mat_id);
            if (id_len != 0 && (mat_id[0] == '!' || mat_id[0] == '#')) {
                new_special_volume = GetDefaultSpecialVolumeForWater();
            }
        }
    }
    pCollision_info->auto_special_volume = new_special_volume;
    pCollision_info->water_depth_factor = 1.0f;
}

// FUNCTION: CARMA2_HW 0x004175e0
void C2_HOOK_FAKE_THISCALL ControlCar4(tCar_spec* c, undefined4 pArg2, br_scalar dt) {

    CARPOCALYPSE2_THISCALL_UNUSED(pArg2);

    if (c->keys.left) {
        if (c->turn_speed < 0.f) {
            c->turn_speed = 0.f;
        }
        if (c->collision_info->velocity_car_space.v[2] > 0.f) {
            c->turn_speed += .25 * dt;
        } else if ((c->curvature >= 0.f && c->collision_info->omega.v[1] >= -.001) || c->turn_speed != 0.f) {
            c->turn_speed += 25.f * dt * (0.05 / (5. + WORLD_SCALE * BrVector3Length(&c->collision_info->v) )) * .25;
        } else {
            c->turn_speed = 25.f * dt * (.05 / (5. + WORLD_SCALE * BrVector3Length(&c->collision_info->v)));
            if (c->collision_info->omega.v[1] < -.01) {
                c->turn_speed -= .25 * dt * c->collision_info->omega.v[1];
            }
        }
    }
    if (c->keys.right) {
        if (c->turn_speed > 0.f) {
            c->turn_speed = 0.f;
        }
        if (c->collision_info->velocity_car_space.v[2] > 0.f) {
            c->turn_speed -= .25 * dt;
        } else if ((c->curvature <= 0.f && c->collision_info->omega.v[1] <= .001) || c->turn_speed != 0.f) {
            c->turn_speed -= 25.f * dt * (.05 / (5. + WORLD_SCALE * BrVector3Length(&c->collision_info->v))) * .25;
        } else {
            c->turn_speed = -25.f * dt * (.05 / (5. + WORLD_SCALE * BrVector3Length(&c->collision_info->v)));
            if (c->collision_info->omega.v[1] < -.01) {
                c->turn_speed -= .25 * dt * c->collision_info->omega.v[1];
            }
        }
    }
    if (!c->keys.left && !c->keys.right) {
        c->turn_speed = 0.f;
    } else if (fabsf(c->turn_speed) < fabsf(2.f * dt * c->curvature) && c->curvature * c->turn_speed < 0.f) {
        c->turn_speed = -(2.f * dt * c->curvature);
    }
    c->curvature += c->turn_speed;
    if (c->joystick.left > 0) {
        c->curvature = (float)c->joystick.left * (float)c->joystick.left / 4294967300.f * c->maxcurve;
    } else if (c->joystick.right >= 0) {
        c->curvature = -((float)c->joystick.right * (float)c->joystick.right / 4294967300.f) * c->maxcurve;
    }
    if (c->curvature > c->maxcurve) {
        c->curvature = c->maxcurve;
    }
    if (c->curvature < -c->maxcurve) {
        c->curvature = -c->maxcurve;
    }
}

__inline void C2_HOOK_FASTCALL RememberSafePosition(tCar_spec* car, tU32 pTime_difference) {
    // GLOBAL: CARMA2_HW 0x006793ec
    static tU32 time_count = 0;
    int i;
    br_vector3 r;

    time_count += pTime_difference;
    if (time_count < 5000) {
        return;
    }
    time_count = 4000;
    if (car->disabled) {
        return;
    }
    if (car->car_crush_spec != NULL && car->car_crush_spec->field_0x4b8) {
        return;
    }

    CARPOCALYPSE2_BUG_ON(CARPOCALYPSE2_ASIZE(car->oldd) != 4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, susp_height, 0x1218);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, oldd, 0x1264);

    for (i = 0; i < CARPOCALYPSE2_ASIZE(car->oldd); i++) {
        if (car->oldd[i] >= car->susp_height[i / 2]) {
            return;
        }
    }
    if ((car->collision_info->last_special_volume == NULL || car->collision_info->last_special_volume->gravity_multiplier == gZero)
        && gFriction_materials[car->material_index[0]].tyre_road_friction >= 0.1
        && gFriction_materials[car->material_index[1]].tyre_road_friction >= 0.1
        && gFriction_materials[car->material_index[2]].tyre_road_friction >= 0.1
        && gFriction_materials[car->material_index[3]].tyre_road_friction >= 0.1
        && !car->field_0x195c
        && car->car_master_actor->t.t.mat.m[1][1] >= 0.8f) {

        CARPOCALYPSE2_BUG_ON(CARPOCALYPSE2_ASIZE(car->last_safe_positions) != 20);
        /* Only check last 5 positions */
        for (i = 0; i < 4; i++) {
            BrVector3Sub(&r, &car->car_master_actor->t.t.translate.t, (br_vector3*)car->last_safe_positions[i].m[3]);

            if (BrVector3LengthSquared(&r) < 8.4015961f) {
                return;
            }
        }
        for (i = CARPOCALYPSE2_ASIZE(car->last_safe_positions) - 2; i > 0; i--) {
            BrMatrix34Copy(&car->last_safe_positions[i], &car->last_safe_positions[i - 1]);
        }
        BrMatrix34Copy(&car->last_safe_positions[0], &car->car_master_actor->t.t.mat);
        time_count = 0;
    }
}

// FUNCTION: CARMA2_HW 0x00414cb0
void C2_HOOK_FASTCALL ControlOurCar(tU32 pTime_difference) {
    // GLOBAL: CARMA2_HW 0x0058f6d8
    static int last_key_down = 1;
    tCar_spec* car;
    tU32 time;
    br_vector3 minus_k;
    br_vector3 tmp;
    br_vector3 delta_omega;

    car = &gProgram_state.current_car;

    if (car->keys.change_down) {
        if (!last_key_down) {
            last_key_down = 1;

            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, number_of_wheels_on_ground, 0x12e8);
            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, oldd, 0x1264);
            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, susp_height, 0x1218);
            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, frame_collision_flag, 0x64);
            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, disable_move_rotate, 0xec);
            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, omega, 0x74);

            if (car->number_of_wheels_on_ground <= 2
                    && BrVector3LengthSquared(&car->collision_info->v) < .04
                    && car->oldd[0] == car->susp_height[0]
                    && car->oldd[1] == car->susp_height[1]
                    && (car->frame_collision_flag || car->collision_info->disable_move_rotate)) {

                tmp.v[0] = -car->collision_info->transform_matrix.m[1][2];
                tmp.v[1] = 0.f;
                tmp.v[2] = car->collision_info->transform_matrix.m[1][0];
                BrVector3Normalise(&tmp, &tmp);
                BrMatrix34TApplyV(&delta_omega, &tmp, &car->collision_info->transform_matrix);
                BrVector3Accumulate(&car->collision_info->omega, &delta_omega);
                car->collision_info->disable_move_rotate = 0;
            }
        }
    } else {
        last_key_down = 0;
    }
    if (gNet_mode != eNet_mode_none) {
        int i;

        for (i = 0; i < gNumber_of_net_players; i++) {

            if (i != gThis_net_player_index) {
                ControlCar[gControl__car](gNet_players[i].car CARPOCALYPSE2_THISCALL_EDX, 0.001f * pTime_difference);
            }
        }
    }
    if (gCar_flying) {
        FlyCar(gCar_to_view CARPOCALYPSE2_THISCALL_EDX, pTime_difference / 1000.f);
        return;
    }

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, end_steering_damage_effect, 0x154c);

    time = GetTotalTime();
    if (car->damage_units[eDamage_steering].damage_level > 40) {
        if (car->end_steering_damage_effect != 0) {
            if (car->end_steering_damage_effect > time || car->damage_units[eDamage_steering].damage_level == 99) {
                car->keys.left = car->false_key_left;
                car->keys.right = car->false_key_right;
            } else {
                car->end_steering_damage_effect = 0;
            }
        } else {
            float ts;

            ts = pTime_difference * (car->damage_units[eDamage_steering].damage_level - 40) * 0.0045;
            if (PercentageChance((int)ts) && fabsf(car->collision_info->velocity_car_space.v[2]) > 1. / (10000. * WORLD_SCALE)) {
                if (car->keys.left || car->keys.right) {
                    car->false_key_left = !car->keys.left;
                    car->false_key_right = !car->keys.right;
                } else {
                    if (PercentageChance(50)) {
                        car->false_key_left = 1;
                    } else {
                        car->false_key_right = 1;
                    }
                }
                ts = (float)(25 * (car->damage_units[eDamage_steering].damage_level - 40));
                car->end_steering_damage_effect = (tU32)(time + FRandomBetween(0.0f, ts));
            }
        }
    }
    if (car->damage_units[eDamage_transmission].damage_level > 40) {
        if (car->end_trans_damage_effect != 0) {
            if (car->end_trans_damage_effect > time || car->damage_units[eDamage_transmission].damage_level == 99) {
                car->gear = 0;
                car->just_changed_gear = 1;
            } else {
                car->end_trans_damage_effect = 0;
            }
        } else {
            float ts;

            ts = pTime_difference * (car->damage_units[eDamage_transmission].damage_level - 40) * 0.006;
            if (PercentageChance((int)ts)) {
                ts = (float)(50 * (car->damage_units[eDamage_transmission].damage_level - 40));
                car->end_trans_damage_effect = (tU32)(time + FRandomBetween(0.f, ts));
            }
        }
    }
    ControlCar[gControl__car](car CARPOCALYPSE2_THISCALL_EDX, pTime_difference / 1000.0f);
    RememberSafePosition(car, pTime_difference);
    if (gCamera_reset) {
        BrVector3SetFloat(&minus_k, 0.0f, 0.0f, -1.0f);
        gCamera_sign = 0;
        BrMatrix34ApplyV(&car->direction, &minus_k, &car->car_master_actor->t.t.mat);
    }
}

// FUNCTION: CARMA2_HW 0x00414510
void C2_HOOK_FASTCALL SetInitialPosition(tRace_info* pThe_race, int pCar_index, int pGrid_index) {
    int place_on_grid;
    int i;
    int start_i;
    int j;
    br_actor* car_actor;
    br_angle initial_yaw;
    br_scalar nearest_y_above;
    br_scalar nearest_y_below;
    int below_face_index;
    int above_face_index;
    br_model* below_model;
    br_model* above_model;
    tCar_spec* car;
    br_vector3 grid_offset;
    br_vector3 dist;
    br_vector3 real_pos;
    br_matrix34 initial_yaw_matrix;

    initial_yaw = 0;
    car_actor = pThe_race->opponent_list[pCar_index].car_spec->car_master_actor;
    car = pThe_race->opponent_list[pCar_index].car_spec;
    BrMatrix34Identity(&car_actor->t.t.mat);
    place_on_grid = 1;
    if (gNet_mode != eNet_mode_none && !gCurrent_net_game->options.grid_start && pThe_race->count_network_start_points != 0) {
        start_i = i = IRandomBetween(0, pThe_race->count_network_start_points - 1);
        do {
            PossibleService();
            for (j = 0; j < gNumber_of_net_players; j++) {
                if (j != pCar_index) {
                    BrVector3Copy(&real_pos, &pThe_race->opponent_list[j].car_spec->car_master_actor->t.t.translate.t);
                    if (real_pos.v[0] > 500.f) {
                        BrVector3Sub(&real_pos, &real_pos, &gInitial_position);
                    }
                    BrVector3Sub(&dist, &real_pos, &pThe_race->net_starts[i].pos);
                    if (BrVector3LengthSquared(&dist) < 16.f) {
                        break;
                    }
                }
            }
            if (j == gNumber_of_net_players) {
                BrVector3Copy(&car_actor->t.t.translate.t, &pThe_race->net_starts[i].pos);
                initial_yaw = BrDegreeToAngle(pThe_race->net_starts[i].yaw);
                place_on_grid = 0;
            }
            i++;
            if (i == pThe_race->count_network_start_points) {
                i = 0;
            }
        } while (i != start_i);
    }
    if (gNet_mode == eNet_mode_none && pGrid_index < 0) {
        BrVector3Copy(&car_actor->t.t.translate.t, &pThe_race->net_starts[-pGrid_index].pos);
        initial_yaw = BrDegreeToAngle(pThe_race->net_starts[-pGrid_index].yaw);
        place_on_grid = 0;
    }
    if (place_on_grid) {
        initial_yaw = BrDegreeToAngle(pThe_race->initial_yaw);
        BrMatrix34RotateY(&initial_yaw_matrix, initial_yaw);
        grid_offset.v[0] = -(br_scalar)(pGrid_index % 2);
        grid_offset.v[1] = 0.0f;
        grid_offset.v[2] = 2.0f * (br_scalar)(pGrid_index / 2) + 0.4f * (br_scalar)(pGrid_index % 2);
        BrMatrix34ApplyV(&car_actor->t.t.translate.t, &grid_offset, &initial_yaw_matrix);
        BrVector3Accumulate(&car_actor->t.t.translate.t, &pThe_race->initial_position);
    }
    if (gTrack_actor != NULL) {
        FindBestY(
            &car_actor->t.t.translate.t,
            gTrack_actor,
            10.0f,
            &nearest_y_above,
            &nearest_y_below,
            &above_model,
            &below_model,
            &above_face_index,
            &below_face_index);
        if (nearest_y_above != 30000.0f) {
            car_actor->t.t.translate.t.v[1] = nearest_y_above;
        } else if (nearest_y_below != -30000.0f) {
            car_actor->t.t.translate.t.v[1] = nearest_y_below;
        } else {
            car_actor->t.t.translate.t.v[1] = 0.0f;
        }
    }
    BrMatrix34PreRotateY(&car_actor->t.t.mat, initial_yaw);
    if (gNet_mode != eNet_mode_none) {
        tCompressed_matrix3 compressed;
        int inactive;

        CompressMatrix34(&compressed, &inactive, &car_actor->t.t.mat);
        ExpandMatrix34(&car_actor->t.t.mat, &compressed, inactive);
        BrMatrix34Copy(
            &gNet_players[pThe_race->opponent_list[pCar_index].net_player_index].initial_position,
            &car->car_master_actor->t.t.mat);
    }
    if (gNet_mode != eNet_mode_none && car->disabled && car_actor->t.t.translate.t.v[0] < 500.0f) {
        DisableCar(car);
    }
}

// FUNCTION: CARMA2_HW 0x00413f70
void C2_HOOK_FASTCALL InitialiseCar2(tCar_spec* pCar, int pClear_disabled_flag) {
    int index;
    int j;
    br_actor* car_actor;
    br_matrix34 safe_position;
    tNet_game_player_info* net_player;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, field_0x261, 0x261);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, message_time, 0x268);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, field_0x49c, 0x49c);

    PossibleService();
    if (pCar->disabled && pClear_disabled_flag) {
        if (gNet_mode == eNet_mode_none) {
            EnableCar(pCar);
        } else {
            net_player = NetPlayerFromCar(pCar);
            if (net_player->player_status == ePlayer_status_ready || net_player->player_status == ePlayer_status_racing) {
                EnableCar(pCar);
            }
        }
    }
    car_actor = pCar->car_master_actor;
    InitCarSkidStuff(pCar);
    pCar->car_model_actor->render_style = BR_RSTYLE_DEFAULT;
    SwitchCarModels(pCar, 0);
    pCar->collision_info->last_special_volume = NULL;
    pCar->collision_info->auto_special_volume = NULL;
    pCar->field_0xc8 = NULL;
    if (pCar != NULL && pCar->driver == eDriver_local_human) {
        ResetRecoveryVouchers();
    }
    BrVector3SetFloat(&pCar->collision_info->v, 0.f, 0.f, 0.f);
    BrVector3SetFloat(&pCar->collision_info->omega, 0.f, 0.f, 0.f);
    pCar->curvature = 0.f;
    pCar->field_0x1260 = 0.f;
    BrMatrix34Copy(&safe_position, &car_actor->t.t.mat);
    if (safe_position.m[3][0] > 500.0f) {
        BrVector3Sub((br_vector3*)safe_position.m[3], (br_vector3*)safe_position.m[3], &gInitial_position);
    }
    BrMatrix34Copy(&pCar->old_frame_mat, &safe_position);
    BrMatrix34Copy(&pCar->collision_info->transform_matrix, &safe_position);
    BrMatrix34ApplyP(&pCar->pos, &pCar->collision_info->cmpos, &pCar->collision_info->transform_matrix);
    for (j = 0; j < CARPOCALYPSE2_ASIZE(pCar->oldd); j++) {
        pCar->oldd[j] = pCar->ride_height;
    }
    pCar->gear = 0;
    pCar->revs = 0.f;
    pCar->traction_control = 1;
    BrVector3Negate(&pCar->direction, (br_vector3*)car_actor->t.t.mat.m[2]);
    for (j = 0; j < CARPOCALYPSE2_ASIZE(pCar->last_safe_positions); j++) {
        BrMatrix34Copy(&pCar->last_safe_positions[j], &safe_position);
    }
    pCar->collision_info->field_0x261 = 0;
    pCar->collision_info->message_time = 0;
    pCar->dt = -1.f;
    pCar->collision_info->field_0x49c = 1;
    pCar->time_to_recover = 0;
    pCar->repair_time  = 0;
    pCar->collision_info->water_d  = 10000.f;

    switch (pCar->driver) {
    case eDriver_oppo:
        index = 0;
        for (j = 0; j < gCurrent_race.number_of_racers; j++) {
            if (gCurrent_race.opponent_list[j].car_spec != NULL
                    && gCurrent_race.opponent_list[j].car_spec->driver == eDriver_oppo) {
                if (gCurrent_race.opponent_list[j].car_spec == pCar) {
                    pCar->car_ID = 0x200 + index;
                }
                index++;
            }
        }
        break;
    case eDriver_net_human:
        index = 0;
        for (j = 0; j < gCurrent_race.number_of_racers; j++) {
            if (gCurrent_race.opponent_list[j].car_spec != NULL
                    && gCurrent_race.opponent_list[j].car_spec->driver == eDriver_net_human) {
                if (gCurrent_race.opponent_list[j].car_spec == pCar) {
                    pCar->car_ID = 0x100 + index;
                }
                index++;
            }
        }
        break;
    case eDriver_local_human:
        pCar->car_ID = 0;
        break;
    default:
        abort();
        break;
    }
    PossibleService();
    pCar->collision_info->box_face_ref = gFace_num__car - 2;
    pCar->collision_info->box_face_end = 0;
    pCar->collision_info->box_face_start = 0;
    pCar->collision_info->disable_move_rotate = 0;
    pCar->end_steering_damage_effect = 0;
    pCar->end_trans_damage_effect = 0;
    pCar->wheel_dam_offset[0] = 0.f;
    pCar->wheel_dam_offset[1] = 0.f;
    pCar->wheel_dam_offset[2] = 0.f;
    pCar->wheel_dam_offset[3] = 0.f;
    pCar->shadow_intersection_flags = 0;
    pCar->underwater_ability = 0;

    if (gNet_mode == eNet_mode_none) {
        net_player = NULL;
    } else {
        net_player = NetPlayerFromCar(pCar);
    }
    if (net_player != NULL && net_player->field_0x80) {
        pCar->invulnerable_no_crushage = 1;
        pCar->invulnerable_no_damage = 1;
        pCar->invulnerable_no_wastage = 1;
    } else {
        pCar->invulnerable_no_crushage = 0;
        pCar->invulnerable_no_damage = 0;
        pCar->invulnerable_no_wastage = 0;
    }
    pCar->wall_climber_mode = 0;
    pCar->grip_multiplier = 1.f;
    pCar->damage_multiplier = 1.f;
    pCar->field_0x4c8 = 1.f;
    pCar->field_0x4d4 = 1.f;
    pCar->bounce_rate = 0.f;
    pCar->bounce_amount = 0.f;
    pCar->knackered = 0;
    pCar->collision_info->last_special_volume = NULL;
    pCar->collision_info->auto_special_volume = NULL;
    RemoveFromCloakingList(pCar);
    TurnOffCloaking(NULL, pCar);
    if (pCar != NULL && pCar->driver != eDriver_local_human) {
        pCar->joystick.left = -1;
        pCar->joystick.right = -1;
    }
    TotallyRepairACar(pCar);
    SetCarSuspGiveAndHeight(pCar CARPOCALYPSE2_THISCALL_EDX, 1.f, 1.f, 1.f, 0.f, 0.f);
    for (j = 0; j < CARPOCALYPSE2_ASIZE(pCar->powerups); j++) {
        pCar->powerups[j] = 0;
    }
    if (gNet_mode != eNet_mode_none && (net_player == NULL || !net_player->field_0x80)) {
        for (j = 0; j < CARPOCALYPSE2_ASIZE(pCar->power_up_levels); j++) {
            if (gNet_mode == eNet_mode_none) {
                pCar->power_up_levels[j] = gInitial_APO[j].initial[gProgram_state.skill_level];
            } else {
                pCar->power_up_levels[j] = gInitial_APO[j].initial_network[gProgram_state.skill_level];
            }
            if (gNet_mode == eNet_mode_none) {
                pCar->power_up_slots[j] = gInitial_APO_potential[j].initial[gProgram_state.skill_level];
            } else {
                pCar->power_up_slots[j] = gInitial_APO_potential[j].initial_network[gProgram_state.skill_level];
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x00414400
void C2_HOOK_FASTCALL InitialiseCar(tCar_spec* pCar) {

    InitialiseCar2(pCar, 1);
}

// FUNCTION: CARMA2_HW 0x00414410
void C2_HOOK_FASTCALL InitialiseCarsEtc(tRace_info* pThe_race) {
    int i;
    int cat;
    int car_count;
    tCar_spec* car;
    br_bounds bnds;

    BrVector3Copy(&gProgram_state.initial_position, &pThe_race->initial_position);
    gProgram_state.initial_yaw = pThe_race->initial_yaw;
    BrActorToBounds(&bnds, gProgram_state.track_spec.the_actor);
    gMin_world_y = bnds.min.v[1];
    gNum_active_non_cars = 0;
    for (cat = eVehicle_self; cat <= eVehicle_not_really; cat++) {
        if (cat == eVehicle_self) {
            car_count = 1;
        } else {
            car_count = GetCarCount(cat);
        }
        for (i = 0; i < car_count; i++) {
            PossibleService();
            if (cat == eVehicle_self) {
                car = &gProgram_state.current_car;
            } else {
                car = GetCarSpec(cat, i);
            }
            if (cat != eVehicle_not_really) {
                InitialiseCar(car);
            }
        }
    }
    gCamera_yaw = 0;
    if (gAction_replay_camera_mode == kActionReplayCameraMode_Manual) {
        gCamera_type = 0;
        gAction_replay_camera_mode = kActionReplayCameraMode_Standard;
    }
    InitialiseExternalCamera();
    if (gUnknown_car_collision_info != NULL && gProgram_state.current_car.collision_info != gUnknown_car_collision_info->parent) {
        AddCollisionInfoChild(gProgram_state.current_car.collision_info, gUnknown_car_collision_info);
    }
}

// FUNCTION: CARMA2_HW 0x004148d0
void C2_HOOK_FASTCALL SetInitialPositions(tRace_info* pThe_race) {
    int i;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tRace_info, number_of_racers, 0x90);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tRace_info, opponent_list, 0xce4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tOpp_spec, ranking, 0x8);
    C2_HOOK_BUG_ON(sizeof(tOpp_spec) != 16);

    for (i = 0; i < pThe_race->number_of_racers; i++) {
        int grid_index = pThe_race->opponent_list[i].ranking;

        if (grid_index >= 0) {
            grid_index = i;
        }
        SetInitialPosition(pThe_race, i, grid_index);
    }
}


// FUNCTION: CARMA2_HW 0x00413580
void C2_HOOK_FASTCALL InitialiseExternalCamera(void) {
    br_scalar ts;
    tCar_spec* c;
    br_angle yaw;

    c = gCar_to_view;
    if (!gProgram_state.racing) {
        c = &gProgram_state.current_car;
    }
    gCamera_height = c->pos.v[1];
    BrVector3Set(&gView_direction, c->direction.v[0], 0.0f, c->direction.v[2]);
    BrVector3Normalise(&gView_direction, &gView_direction);
    ts = -BrVector3Dot(&gView_direction, (br_vector3*)c->car_master_actor->t.t.mat.m[2]);
    gCamera_sign = ts < 0;
    gCamera_mode = 0;
    if (gCamera_sign) {
        yaw = -gCamera_yaw;
    } else {
        yaw = gCamera_yaw;
    }
    DrVector3RotateY(&gView_direction, yaw);
    gMin_camera_car_distance = 0.6f;
    gCamera_frozen = 0;
    gCamera_mode = -2;
    if (gCountdown && (gNet_mode == eNet_mode_none || gCurrent_net_game->options.grid_start) && gCountdown > 4) {
        gCamera_height += 10.f;
    }
}

void C2_HOOK_FASTCALL SetAmbientPratCam(tCar_spec* pCar) {
    br_scalar vcs_x;
    br_scalar vcs_y;
    br_scalar vcs_z;
    br_scalar abs_vcs_x;
    br_scalar abs_vcs_y;
    br_scalar abs_vcs_z;
    br_scalar abs_omega_x;
    br_scalar abs_omega_y;
    br_scalar abs_omega_z;
    tU32 the_time;
    // GLOBAL: CARMA2_HW 0x00679370
    static tU32 last_time_on_ground;

    if (gRace_finished) {
        return;
    }
    the_time = GetTotalTime();
    if (pCar->number_of_wheels_on_ground != 0) {
        last_time_on_ground = the_time;
    }
    vcs_x = pCar->collision_info->velocity_car_space.v[0] / 1000.f;
    vcs_y = pCar->collision_info->velocity_car_space.v[1] / 1000.f;
    vcs_z = pCar->collision_info->velocity_car_space.v[2] / 1000.f;
    abs_vcs_x = fabsf(vcs_x);
    abs_vcs_y = fabsf(vcs_y);
    abs_vcs_z = fabsf(vcs_z);
    abs_omega_x = fabsf(pCar->collision_info->omega.v[0]);
    abs_omega_y = fabsf(pCar->collision_info->omega.v[1]);
    abs_omega_z = fabsf(pCar->collision_info->omega.v[2]);

    if (abs_omega_x > 4.5f || abs_omega_z > 4.5f) {
        ChangeAmbientPratcam(9);
    } else if (abs_omega_y > 4.5f) {
        ChangeAmbientPratcam(12);
    } else if (abs_omega_x > 3.f || abs_omega_z > 3.f) {
        ChangeAmbientPratcam(8);
    } else if (abs_omega_y > 3.f) {
        ChangeAmbientPratcam(11);
    } else if (pCar->car_master_actor->t.t.mat.m[1][1] < 0.1f) {
        ChangeAmbientPratcam(44);
    } else if (abs_vcs_y > abs_vcs_z && abs_vcs_y > abs_vcs_x && vcs_y < -.004f) {
        ChangeAmbientPratcam(6);
    } else if (the_time - last_time_on_ground > 500) {
        ChangeAmbientPratcam(5);
    } else if (abs_vcs_x > abs_vcs_z && vcs_x > .001f) {
        ChangeAmbientPratcam(26);
    } else if (abs_vcs_x > abs_vcs_z && vcs_x < -.001f) {
        ChangeAmbientPratcam(25);
    } else if (abs_omega_x > 1.5f || abs_omega_z > 1.5f) {
        ChangeAmbientPratcam(7);
    } else if (abs_omega_y > 1.5f) {
        ChangeAmbientPratcam(10);
    } else if (abs_vcs_z > .01f) {
        ChangeAmbientPratcam(3);
    } else if (abs_vcs_z > .004f) {
        ChangeAmbientPratcam(2);
    } else if (abs_vcs_z > .0015f) {
        ChangeAmbientPratcam(1);
    } else {
        ChangeAmbientPratcam(0);
    }
}

// FUNCTION: CARMA2_HW 0x0041e660
void C2_HOOK_FASTCALL MungeCarGraphics(tU32 pFrame_period) {
    tU32 the_time;
    int car;
    int cat;

    if (gNet_mode != eNet_mode_none
            && gCurrent_net_game->type == eNet_game_type_foxy
            && gThis_net_player_index == gIt_or_fox) {
        gProgram_state.current_car.power_up_levels[1] = 0;
    }
    SetAmbientPratCam(&gProgram_state.current_car);

    the_time = PDGetTotalTime();
    for (cat = eVehicle_self; cat <= eVehicle_net_player; cat++) {
        int car_count;

        if (cat == eVehicle_self) {
            car_count = 1;
        } else {
            car_count = GetCarCount(cat);
        }
        for (car = 0; car < car_count; car++) {
            tCar_spec* the_car;

            if (cat == eVehicle_self) {
                the_car = &gProgram_state.current_car;
            } else {
                the_car = GetCarSpec(cat, car);
            }
            if (!(the_car != NULL && the_car->driver == eDriver_local_human) && PointOutOfSight(&the_car->pos CARPOCALYPSE2_THISCALL_EDX, gYon_squared)) {
                the_car->car_master_actor->render_style = BR_RSTYLE_NONE;
            } else {
                the_car->car_master_actor->render_style = BR_RSTYLE_DEFAULT;
            }
        }
    }
    if (!(gCar_to_view != NULL && gCar_to_view->driver == eDriver_oppo)) {
        gCar_to_view->car_master_actor->render_style = BR_RSTYLE_DEFAULT;
    }

    for (car = 0; car < gNum_active_cars; car++) {
        tCar_spec* the_car;

        the_car = gActive_car_list[car];
        if (the_car->car_master_actor->render_style != BR_RSTYLE_NONE && the_car != NULL && the_car->driver >= eDriver_oppo) {
            br_scalar car_x;
            br_scalar car_z;
            int oily_count;
            int i;

            the_car->shadow_intersection_flags = 0;
            car_x = the_car->car_master_actor->t.t.translate.t.v[0];
            car_z = the_car->car_master_actor->t.t.translate.t.v[2];
            oily_count = GetOilSpillCount();

            for (i = 0; i < oily_count; i++) {
                br_actor* oily_actor;
                br_scalar oily_size;

                GetOilSpillDetails(i, &oily_actor, &oily_size);
                if (oily_actor != NULL) {
                    br_scalar car_radius;

                    car_radius = the_car->collision_info->shape->common.bb.max.v[2] / WORLD_SCALE * 1.5f;
                    if (oily_actor->t.t.translate.t.v[0] - oily_size < car_x + car_radius
                            && oily_actor->t.t.translate.t.v[0] + oily_size > car_x - car_radius
                            && oily_actor->t.t.translate.t.v[2] - oily_size < car_z + car_radius
                            && oily_actor->t.t.translate.t.v[2] + oily_size > car_z - car_radius) {
                        the_car->shadow_intersection_flags |= 1 << i;
                    }
                }
            }
            if (the_car->driver < eDriver_net_human && (!gAction_replay_mode || !ARReplayIsReallyPaused())) {
                if (gCountdown) {
                    float sine_angle;
                    float raw_revs;
                    float rev_reducer;

                    sine_angle = FRandomBetween(.4f, 1.6f) * ((float)GetTotalTime() / ((float)gCountdown * 100.f));
                    sine_angle = frac(sine_angle) * 360.0f;
                    sine_angle = FastScalarSin((int)sine_angle);
                    raw_revs = the_car->red_line * fabsf(sine_angle);
                    rev_reducer = (11.f - (float)(gCountdown)) / 10.f;
                    the_car->revs = rev_reducer * raw_revs;
                } else {
                    the_car->revs = (the_car->speedo_speed / 0.003f - (float)(int)(the_car->speedo_speed / 0.003))
                                    * (float)(the_car->red_line - 800)
                                    + 800.f;
                }
            }
            for (i = 0; i < the_car->number_of_steerable_wheels; i++) {
                ControlBoundFunkGroove(the_car->steering_ref[i] CARPOCALYPSE2_THISCALL_EDX, the_car->steering_angle);
            }
            for (i = 0; i < CARPOCALYPSE2_ASIZE(the_car->rf_sus_ref); i++) {
                ControlBoundFunkGroove(the_car->rf_sus_ref[i] CARPOCALYPSE2_THISCALL_EDX, the_car->rf_sus_position);
                if ((i & 1) != 0) {
                    ControlBoundFunkGroove(the_car->lf_sus_ref[i] CARPOCALYPSE2_THISCALL_EDX, -the_car->lf_sus_position);
                } else {
                    ControlBoundFunkGroove(the_car->lf_sus_ref[i] CARPOCALYPSE2_THISCALL_EDX, the_car->lf_sus_position);
                }
            }
            for (i = 0; i < CARPOCALYPSE2_ASIZE(the_car->rr_sus_ref); i++) {
                ControlBoundFunkGroove(the_car->rr_sus_ref[i] CARPOCALYPSE2_THISCALL_EDX, the_car->rr_sus_position);
                if ((i & 1) != 0) {
                    ControlBoundFunkGroove(the_car->lr_sus_ref[i] CARPOCALYPSE2_THISCALL_EDX, -the_car->lr_sus_position);
                } else {
                    ControlBoundFunkGroove(the_car->lr_sus_ref[i] CARPOCALYPSE2_THISCALL_EDX, the_car->lr_sus_position);
                }
            }
            if (!gAction_replay_mode || !ARReplayIsReallyPaused()) {
                float wheel_speed;

                wheel_speed = -(the_car->speedo_speed / the_car->non_driven_wheels_circum * (float)gFrame_period);
                if (gAction_replay_mode && ARGetReplayDirection() < 0) {
                    wheel_speed = -wheel_speed;
                }
                ControlBoundFunkGroovePlus(the_car->non_driven_wheels_spin_ref_1 CARPOCALYPSE2_THISCALL_EDX, wheel_speed);
                ControlBoundFunkGroovePlus(the_car->non_driven_wheels_spin_ref_2 CARPOCALYPSE2_THISCALL_EDX, wheel_speed);
                ControlBoundFunkGroovePlus(the_car->non_driven_wheels_spin_ref_3 CARPOCALYPSE2_THISCALL_EDX, wheel_speed);
                ControlBoundFunkGroovePlus(the_car->non_driven_wheels_spin_ref_4 CARPOCALYPSE2_THISCALL_EDX, wheel_speed);
                if (the_car->driver >= eDriver_net_human) {
                    if (the_car->gear != 0) {
                        wheel_speed = -(the_car->revs
                                        * the_car->speed_revs_ratio
                                        * (float)the_car->gear
                                        * (float)gFrame_period
                                        / (1000.f * WORLD_SCALE))
                                        / the_car->driven_wheels_circum;
                    } else if (the_car->keys.brake) {
                        wheel_speed = 0.0;
                    } else {
                        wheel_speed = -(the_car->speedo_speed / the_car->driven_wheels_circum * (float)gFrame_period);
                    }
                    if (gAction_replay_mode && ARGetReplayDirection() < 0) {
                        wheel_speed = -wheel_speed;
                    }
                }
                ControlBoundFunkGroovePlus(the_car->driven_wheels_spin_ref_1 CARPOCALYPSE2_THISCALL_EDX, wheel_speed);
                ControlBoundFunkGroovePlus(the_car->driven_wheels_spin_ref_2 CARPOCALYPSE2_THISCALL_EDX, wheel_speed);
                ControlBoundFunkGroovePlus(the_car->driven_wheels_spin_ref_3 CARPOCALYPSE2_THISCALL_EDX, wheel_speed);
                ControlBoundFunkGroovePlus(the_car->driven_wheels_spin_ref_4 CARPOCALYPSE2_THISCALL_EDX, wheel_speed);
            }
            if (gAction_replay_mode) {
                MungeSpecialVolume(the_car->collision_info);
            } else if (the_car->driver == eDriver_local_human && the_car->collision_info->M < 5.f) {
                br_scalar abs_omega_x;
                br_scalar abs_omega_y;
                br_scalar abs_omega_z;
                int spinning_wildly;

                abs_omega_x = (WORLD_SCALE * WORLD_SCALE * the_car->collision_info->I.v[0] + 3.30f) / 2.f * fabsf(the_car->collision_info->omega.v[0]);
                abs_omega_y = (WORLD_SCALE * WORLD_SCALE * the_car->collision_info->I.v[1] + 3.57f) / 2.f * fabsf(the_car->collision_info->omega.v[1]);
                abs_omega_z = (WORLD_SCALE * WORLD_SCALE * the_car->collision_info->I.v[2] + 0.44f) / 2.f * fabsf(the_car->collision_info->omega.v[2]);
                spinning_wildly = abs_omega_x > 26.4f || abs_omega_y > 49.98f || abs_omega_z > 3.52f;
                if (spinning_wildly && the_time - gLast_cunning_stunt > 10000) {
                    if (gWild_start == 0
                            || (the_car->collision_info->last_special_volume != NULL && the_car->collision_info->last_special_volume->gravity_multiplier != 1.f)) {
                        gWild_start = the_time;
                    } else if (the_time - gWild_start >= 500) {
                        DoFancyHeadup(18);
                        EarnCredits(gCunning_stunt_bonus[gProgram_state.skill_level]);
                        gLast_cunning_stunt = the_time;
                        gOn_me_wheels_start = 0;
                        gQuite_wild_end = 0;
                        gQuite_wild_start = 0;
                        gWoz_upside_down_at_all = 0;
                    }
                } else {
                    int spinning_mildly;

                    gWild_start = 0;
                    spinning_mildly = abs_omega_x > 1.65f || abs_omega_z > .22f;
                    if (the_car->number_of_wheels_on_ground < 4) {
                        gOn_me_wheels_start = 0;
                        if (the_car->number_of_wheels_on_ground == 0 && spinning_mildly) {
                            if (gQuite_wild_start == 0) {
                                gQuite_wild_start = the_time;
                            }
                            if (the_car->car_master_actor->t.t.mat.m[1][1] < -.8f) {
                                gWoz_upside_down_at_all = the_time;
                            }
                        } else {
                            gQuite_wild_end = the_time;
                        }
                    } else {
                        if (gQuite_wild_end == 0) {
                            gQuite_wild_end = the_time;
                        }
                        if (gQuite_wild_start == 0
                                || the_time - gLast_cunning_stunt <= 10000) {
                            gWild_start = 0;
                        } else if (gQuite_wild_end - gQuite_wild_start >= 2000
                                && gQuite_wild_start <= gWoz_upside_down_at_all
                                && gWoz_upside_down_at_all <= gQuite_wild_end) {

                            if (gOn_me_wheels_start != 0) {
                                if (the_time - gOn_me_wheels_start > 500
                                        && !(the_car->collision_info->last_special_volume != NULL && the_car->collision_info->last_special_volume->gravity_multiplier != 1.f)) {
                                    DoFancyHeadup(18);
                                    EarnCredits(gCunning_stunt_bonus[gProgram_state.skill_level]);
                                    gLast_cunning_stunt = PDGetTotalTime();
                                    gQuite_wild_start = 0;
                                    gQuite_wild_end = 0;
                                    gOn_me_wheels_start = 0;
                                    gWoz_upside_down_at_all = 0;
                                } else {
                                    gWild_start = 0;
                                }
                            } else if (the_time - gQuite_wild_end < 300) {
                                gOn_me_wheels_start = the_time;
                                gWild_start = 0;
                            }
                        }
                    }
                }
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x0041f280
intptr_t C2_HOOK_CDECL TurnOffNonGroovers(br_actor* pActor, void* pData) {
    tUser_crush_data *crush_data = pActor->user;

    if (crush_data != NULL && crush_data->groove == NULL) {
        pActor->type = BR_ACTOR_NONE;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0041f110
void C2_HOOK_FASTCALL DoLODCarModels(void) {
    int i;
    int j;
    int variant;
    tCar_spec* car;
    br_vector3 tv;
    float level;

    for (i = 0; i < gNum_active_cars; i++) {
        car = gActive_car_list[i];

        if (car != NULL && car->driver > 5 && car->car_master_actor->render_style != BR_RSTYLE_NONE) {
            BrVector3Sub(&tv, &car->car_master_actor->t.t.translate.t, (br_vector3*)gCamera_to_world.m[3]);
            level = gCar_simplification_factor[gGraf_spec_index][gCar_simplification_level] >= 0.001f ? BrVector3LengthSquared(&tv) / gCar_simplification_factor[gGraf_spec_index][gCar_simplification_level] : BR_SCALAR_MAX;

            for (j = car->count_detail_levels - 1; j > 0; j--) {
                if (car->detail_levels[j] < level) {
                    break;
                }
            }
            if (j > 0) {
                variant = j;
                DRActorEnumRecurse(car->car_model_actor, SwitchCarModel, &variant);
                car->field_0xe18 = variant;
            } else if (!gAction_replay_mode && car->use_shell_model && car->shell_model != NULL) {
                DRActorEnumRecurse(car->car_model_actor, TurnOffNonGroovers, NULL);
                car->car_model_actor->type = BR_ACTOR_MODEL;
                car->car_model_actor->model = car->shell_model;
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x0041f2a0
void C2_HOOK_FASTCALL DoComplexCarModels(void) {
    int i;
    tCar_spec* car;

    for (i = 0; i < gNum_active_cars; i++) {
        car = gActive_car_list[i];
        if (car != NULL && car->driver > 5) {
            SwitchCarModels(car, 0);
        }
    }
}

// FUNCTION: CARMA2_HW 0x0041f300
void C2_HOOK_FASTCALL ResetCarScreens(void) {
    int i;
    int j;

    for (i = 0; i <= 3; i++) {
        int count = (i == 0) ? 1 : GetCarCount(i);
        for (j = 0; j < count; j++) {
            tCar_spec* spec = (i == 0) ? &gProgram_state.current_car : GetCarSpec(i, j);
            spec->collision_info->last_special_volume = NULL;
        }
    }
    MungeCarGraphics(gFrame_period);
}

// FUNCTION: CARMA2_HW 0x0040f760
void C2_HOOK_FASTCALL CameraBugFix(tCar_spec* c, tU32 pTime) {

    if (gAction_replay_mode
            && (gAction_replay_camera_mode == kActionReplayCameraMode_ActionTracking || gAction_replay_camera_mode == kActionReplayCameraMode_Panning)
            && gPed_actor != NULL
            && !gProgram_state.cockpit_on) {
        IncidentCam(c, pTime);
    }
}

void C2_HOOK_FASTCALL SetTextureBits(tCar_spec* pCar) {

    pCar->field_0x18cc = 0;
    if (pCar->keys.brake || (pCar->brake_force != 0.f && fabsf(pCar->collision_info->velocity_car_space.v[2]) > 7.2463765e-05f)) {
        pCar->field_0x18cc |= 0x4;
    }
    if (pCar->gear < 0 || (!(pCar != NULL && pCar->driver == eDriver_local_human) && pCar->collision_info->velocity_car_space.v[2] > 0.f)) {
        pCar->field_0x18cc |= 0x8;
    }
}

// FUNCTION: CARMA2_HW 0x0041e5a0
void C2_HOOK_FASTCALL MungeSomeOtherCarGraphics(void) {
    int i;

    for (i = 0; i < gNum_active_cars; i++) {
        tCar_spec* car;

        car = gActive_car_list[i];
        if (car->car_master_actor->render_style != BR_RSTYLE_NONE && !gAction_replay_mode) {
            SetTextureBits(car);
        }
    }
}

// FUNCTION: CARMA2_HW 0x00413780
void C2_HOOK_FASTCALL GetAverageGridPosition(tRace_info* pThe_race) {

    BrVector3SetFloat(&gAverage_grid_position, 0.0f, 0.0f, 0.0f);
    if (pThe_race->number_of_racers <= 2) {
        BrVector3Copy(&gAverage_grid_position, &gProgram_state.current_car.pos);
    } else {
        int i;
        br_scalar total_cars;

        total_cars = 0.0f;
        for (i = 0; i < pThe_race->number_of_racers; i++) {
            tCar_spec* car;

            car = pThe_race->opponent_list[i].car_spec;
            BrVector3Accumulate(&gAverage_grid_position, &car->pos);
            total_cars += 1.0f;
        }
        BrVector3InvScale(&gAverage_grid_position, &gAverage_grid_position, total_cars);
    }
}

// FUNCTION: CARMA2_HW 0x00420880
int C2_HOOK_FASTCALL GetPrecalculatedFacesUnderCar(tCar_spec* pCar, tFace_ref** pFace_refs) {

    if (pCar->collision_info->box_face_ref == gFace_num__car
        || (pCar->collision_info->box_face_ref == gFace_num__car - 1 && gFace_count < pCar->collision_info->box_face_start)) {
        *pFace_refs = &gFace_list__car[pCar->collision_info->box_face_start];
        return pCar->collision_info->box_face_end - pCar->collision_info->box_face_start;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0041c1b0
int C2_HOOK_FASTCALL ProcessForcesCallback(void* arg1, float* arg2, int arg3) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x0041e310
int C2_HOOK_FASTCALL ProcessJointForcesCallback(undefined4 param_1, undefined4 param_2, undefined4 param_3) {

    return 0;
}

static br_scalar Dot3(const br_vector3* a, const br_vector3* b) {
    return a->v[0] * b->v[0] + a->v[1] * b->v[1] + a->v[2] * b->v[2];
}

/* declared locally so the shared headers stay unmodified (codegen stability) */
void C2_HOOK_FASTCALL FreezeCamera(void);
void C2_HOOK_FASTCALL AddSplashToPipingSession(tPhysics_object* pCollision);
void C2_HOOK_FASTCALL AddExtendedSplashToPipingSession(tPhysics_object* pCollision, void* pArg2);
void C2_HOOK_FASTCALL PedPreCollisionStuff(void);
#define ePipe_chunk_splash 20
#define ePipe_chunk_extended_splash 59

// FUNCTION: CARMA2_HW 0x00414910
void C2_HOOK_FASTCALL NewFacesListCallback(tPhysics_object* pCollision, undefined4* arg2) {
    tFace_ref* pFace;
    br_bounds3 current_bounds;
    br_scalar old_d;
    float prop7;

    pFace = (tFace_ref*)arg2;
    old_d = pCollision->water_d;
    if (pCollision->owner == NULL) {
        return;
    }
    prop7 = PHILGetObjectProperty(pCollision, 7);
    if (pCollision->flags_0x238 != 1 && prop7 == 0.f) {
        return;
    }
    if (pCollision != NULL
        && pCollision->owner != NULL
        && pCollision->flags_0x238 == 1
        && ((tCar_spec*)pCollision->owner)->driver == eDriver_local_human
        && pCollision->water_d != 10000.f
        && gDouble_pling_water
        && BrVector3Dot(&pCollision->water_normal, &pCollision->field_0xf4.max) - pCollision->water_d <= 0.f) {
        gInTheSea = 1;
        FreezeCamera();
    }
    if (pFace != NULL && fabsf(pFace->normal.v[1]) > 0.9f) {
        BrVector3Copy(&pCollision->water_normal, &pFace->normal);
        if (pCollision->water_normal.v[1] < 0.f) {
            BrVector3Negate(&pCollision->water_normal, &pCollision->water_normal);
        }
        pCollision->water_d = (((pFace->v[0].v[2]) * (pCollision->water_normal.v[2])) + ((pFace->v[0].v[1]) * (pCollision->water_normal.v[1]))) + ((pFace->v[0].v[0]) * (pCollision->water_normal.v[0]));
        if (pCollision != NULL
            && pCollision->owner != NULL
            && pCollision->flags_0x238 == 1
            && ((tCar_spec*)pCollision->owner)->driver == eDriver_local_human) {
            if (pFace->material->identifier[1] == '!') {
                if (BrVector3Dot(&pCollision->field_0xf4.min, &pCollision->water_normal) - pCollision->water_d < 0.f) {
                    GetNewBoundingBox(&current_bounds, &pCollision->bb1, &pCollision->actor->t.t.mat);
                    if ((((pCollision->water_normal.v[0]) * (current_bounds.min.v[0])) + ((pCollision->water_normal.v[1]) * (current_bounds.min.v[1]))) + ((pCollision->water_normal.v[2]) * (current_bounds.min.v[2])) - pCollision->water_d < 0.f) {
                        gInTheSea = 1;
                        FreezeCamera();
                    }
                }
                gDouble_pling_water = 1;
            } else {
                gDouble_pling_water = 0;
            }
        }
    } else {
        pCollision->water_d = 10000.f;
        if (pCollision != NULL
            && pCollision->owner != NULL
            && pCollision->flags_0x238 == 1
            && ((tCar_spec*)pCollision->owner)->driver == eDriver_local_human) {
            gInTheSea = gInTheSea == 1 ? 2 : 0;
        }
    }
    if (fabs(old_d - pCollision->water_d) > 1e-05) {
        if (PHILGetObjectProperty(pCollision, 6) != 0.f) {
            ARStartPipingSession(ePipe_chunk_extended_splash);
            AddExtendedSplashToPipingSession(pCollision, pCollision);
            AREndPipingSession();
        }
        if (pCollision != NULL && pCollision->owner != NULL && pCollision->flags_0x238 == 1) {
            tCar_spec* owner = (tCar_spec*)pCollision->owner;
            if (owner != NULL && owner->driver > 5) {
                ARStartPipingSession(ePipe_chunk_splash);
                AddSplashToPipingSession(pCollision);
                AREndPipingSession();
                return;
            }
            ARStartPipingSession(ePipe_chunk_extended_splash);
            AddExtendedSplashToPipingSession(pCollision, pCollision->actor);
            AREndPipingSession();
            return;
        }
        if (pCollision != NULL && pCollision->owner != NULL && pCollision->flags_0x238 >= 2 && pCollision->flags_0x238 <= 4) {
            ARStartPipingSession(ePipe_chunk_extended_splash);
            AddExtendedSplashToPipingSession(pCollision, pCollision->owner);
            AREndPipingSession();
        }
    }
}

// FUNCTION: CARMA2_HW 0x0041ff20
tNon_car_spec* C2_HOOK_FASTCALL DoPullActorFromWorld(br_actor* actor) {

    NOT_IMPLEMENTED();
    return NULL;
}

// FUNCTION: CARMA2_HW 0x0041ff00
tNon_car_spec* C2_HOOK_FASTCALL PullActorFromWorld(br_actor* actor) {

    if (!gCrush_deferred && !gTesting_car_for_sensible_place) {
        return NULL;
    }
    return DoPullActorFromWorld(actor);
}

// FUNCTION: CARMA2_HW 0x004b5970
float C2_HOOK_FASTCALL GetFrictionFromFace(void *arg1) {
    void *ptr = *(void **)arg1;
    char code = *(char *)*(void **)((char *)ptr + 4);
    int idx = code - 0x2f;
    if (idx < 0 || idx >= 11) {
        idx = 0;
    }
    return gFriction_materials[idx].car_wall_friction;
}

// FUNCTION: CARMA2_HW 0x00417de0
void C2_HOOK_FAKE_THISCALL ControlCar1(tCar_spec* c, undefined4 arg2, br_scalar dt) {
    br_scalar mag;

    CARPOCALYPSE2_THISCALL_UNUSED(arg2);

    c->acc_force = 0.0f;

    if (c->keys.acc) {
        c->acc_force = c->collision_info->M * 7.0f;
    }
    if (c->keys.dec) {
        c->acc_force = -(c->collision_info->M * 7.0f);
    }

    if (c->keys.left) {
        if (c->curvature < 0.0f) {
            c->curvature -= dt * -0.25;
        } else {
            mag = BrVector3Length(&c->collision_info->v);
            c->curvature += 25.f * dt * (0.05f / (mag + 5.0f));
        }
    }

    if (c->keys.right) {
        if (c->curvature > 0.0f) {
            c->curvature -= dt * 0.25;
        } else {
            mag = BrVector3Length(&c->collision_info->v);
            c->curvature -= 25.f * dt * (0.05f / (mag + 5.0f));
        }
    }

    if (c->curvature > c->maxcurve) {
        c->curvature = c->maxcurve;
    }
    if (c->curvature < -c->maxcurve) {
        c->curvature = -c->maxcurve;
    }
}

// FUNCTION: CARMA2_HW 0x00417180
void C2_HOOK_FAKE_THISCALL ControlCar2(tCar_spec* c, undefined4 arg2, br_scalar dt) {
    br_scalar mag;

    CARPOCALYPSE2_THISCALL_UNUSED(arg2);

    c->acc_force = 0.0f;

    if (c->keys.acc) {
        c->acc_force = c->collision_info->M * 7.0f;
    }
    if (c->keys.dec) {
        c->acc_force = -(c->collision_info->M * 7.0f);
    }

    if (c->keys.left) {
        if (c->turn_speed < 0.0f) {
            c->turn_speed = 0.0f;
        }
        if (c->curvature < 0.0f) {
            c->turn_speed -= dt * -0.125;
        } else {
            mag = BrVector3Length(&c->collision_info->v);
            c->turn_speed -= -0.5f * (25.f * dt * (0.05f / (mag + 5.0f)));
        }
    }

    if (c->keys.right) {
        if (c->turn_speed > 0.0f) {
            c->turn_speed = 0.0f;
        }
        if (c->curvature > 0.0f) {
            c->turn_speed -= dt * 0.125;
        } else {
            mag = BrVector3Length(&c->collision_info->v);
            c->turn_speed -= 0.5f * (25.f * dt * (0.05f / (mag + 5.0f)));
        }
    }

    if (!c->keys.left && !c->keys.right) {
        c->turn_speed = 0.0f;
    }

    c->curvature += c->turn_speed;
    if (c->curvature > c->maxcurve) {
        c->curvature = c->maxcurve;
    }
    if (c->curvature < -c->maxcurve) {
        c->curvature = -c->maxcurve;
    }
}

// FUNCTION: CARMA2_HW 0x004173b0
void C2_HOOK_FAKE_THISCALL ControlCar3(tCar_spec* c, undefined4 arg2, br_scalar dt) {
    br_scalar mag;

    CARPOCALYPSE2_THISCALL_UNUSED(arg2);

    if (c->keys.left) {
        if (c->turn_speed < 0.0f) {
            c->turn_speed = 0.0f;
        }
        if (c->curvature >= 0.0f && c->collision_info->omega.v[1] >= 0.0f) {
            mag = BrVector3Length(&c->collision_info->v);
            c->turn_speed -= 25.f * dt * (0.05f / (mag + 5.0f)) * -0.5f * 0.75;
        } else {
            c->turn_speed -= dt * -0.375;
        }
    }

    if (c->keys.right) {
        if (c->turn_speed > 0.0f) {
            c->turn_speed = 0.0f;
        }
        if (c->curvature <= 0.0f && c->collision_info->omega.v[1] <= 0.0f) {
            mag = BrVector3Length(&c->collision_info->v);
            c->turn_speed -= 25.f * dt * (0.05f / (mag + 5.0f)) * 0.5f * 0.75;
        } else {
            c->turn_speed -= dt * 0.375;
        }
    }

    if (!c->keys.left && !c->keys.right) {
        c->turn_speed = 0.0f;
    }

    c->curvature += c->turn_speed;
    if (c->curvature > c->maxcurve) {
        c->curvature = c->maxcurve;
    }
    if (c->curvature < -c->maxcurve) {
        c->curvature = -c->maxcurve;
    }
}

// FUNCTION: CARMA2_HW 0x00417a20
void C2_HOOK_FAKE_THISCALL ControlCar5(tCar_spec* c, undefined4 arg2, br_scalar dt) {
    unsigned int input;
    br_scalar mag;

    CARPOCALYPSE2_THISCALL_UNUSED(arg2);

    input = *(unsigned int*)&c->keys;
    c->acc_force = 0.0f;

    if (c->keys.acc) {
        c->acc_force = c->collision_info->M * 7.0f;
    }
    if (c->keys.dec) {
        c->acc_force = -(c->collision_info->M * 7.0f);
    }

    if (c->keys.left) {
        if (c->turn_speed < 0.0f) {
            c->turn_speed = 0.0f;
        }
        if (c->curvature < 0.0f) {
            c->turn_speed += dt * 0.0625;
        } else {
            mag = sqrtf(c->collision_info->v.v[0] * c->collision_info->v.v[0]
                      + c->collision_info->v.v[1] * c->collision_info->v.v[1]
                      + c->collision_info->v.v[2] * c->collision_info->v.v[2]);
            c->turn_speed += 25.0f * dt * (0.05f / (mag + 5.0f)) * 0.25f;
        }
    }

    if (c->keys.right) {
        if (c->turn_speed > 0.0f) {
            c->turn_speed = 0.0f;
        }
        if (c->curvature > 0.0f) {
            c->turn_speed -= dt * 0.0625;
        } else {
            mag = sqrtf(c->collision_info->v.v[0] * c->collision_info->v.v[0]
                      + c->collision_info->v.v[1] * c->collision_info->v.v[1]
                      + c->collision_info->v.v[2] * c->collision_info->v.v[2]);
            c->turn_speed -= 25.0f * dt * (0.05f / (mag + 5.0f)) * 0.25f;
        }
    }

    if (!c->keys.left && !c->keys.right) {
        c->turn_speed = 0.0f;
        if (c->curvature < 0.0f) {
            if (!c->keys.holdw) {
                mag = sqrtf(c->collision_info->v.v[0] * c->collision_info->v.v[0]
                          + c->collision_info->v.v[1] * c->collision_info->v.v[1]
                          + c->collision_info->v.v[2] * c->collision_info->v.v[2]);
                c->curvature += 25.0f * dt * (0.05f / (mag + 5.0f)) * 2.0f;
                if (c->curvature > 0.0f) {
                    c->curvature = 0.0f;
                }
            }
        } else {
            if (!c->keys.holdw) {
                mag = sqrtf(c->collision_info->v.v[0] * c->collision_info->v.v[0]
                          + c->collision_info->v.v[1] * c->collision_info->v.v[1]
                          + c->collision_info->v.v[2] * c->collision_info->v.v[2]);
                c->curvature -= 25.0f * dt * (0.05f / (mag + 5.0f)) * 2.0f;
                if (c->curvature < 0.0f) {
                    c->curvature = 0.0f;
                }
            }
        }
    }

    c->curvature += c->turn_speed;
    if (c->curvature > c->maxcurve) {
        c->curvature = c->maxcurve;
    }
    if (c->curvature < -c->maxcurve) {
        c->curvature = -c->maxcurve;
    }

    input |= 0x10000;
    *(unsigned int*)&c->keys = input;
}

void C2_HOOK_FASTCALL DrVector3RotateY(br_vector3* v, br_angle t) {
    br_scalar c;
    br_scalar s;
    br_scalar ts;

    c = cosf(BrAngleToRadian(t));
    s = sinf(BrAngleToRadian(t));
    ts = v->v[0] * c + v->v[2] * s;
    v->v[2] = v->v[2] * c - v->v[0] * s;
    v->v[0] = ts;
}

// FUNCTION: CARMA2_HW 0x004f8dc0
intptr_t C2_HOOK_CDECL ActorFunks(br_actor* pActor, void* pContext) {
    tFunk_index_cbfn* funk_index_callback;
    tUser_crush_data* user_crush_data;
    tCar_crush_buffer_entry* crush_data;
    int i;
    int funk_index;

    user_crush_data = pActor->user;
    funk_index_callback = pContext;

    if (user_crush_data != NULL) {
        crush_data = user_crush_data->crush_data;
        if (crush_data != NULL) {
            if (crush_data->smashables != NULL) {
                for (i = 0; i < crush_data->count_smashables; i++) {
                    funk_index = crush_data->smashables[i].funk;
                    if (funk_index >= 0) {
                        funk_index_callback(funk_index);
                    }
                }
            }
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004f9020
void C2_HOOK_FASTCALL RecalculateCarMassMomentOfInertia(tCar_spec* car) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x0047b2b0
void C2_HOOK_FASTCALL MasterDisableFunkotronic(int pFunk_index) {

    gFunkotronics_array[pFunk_index].flags |= 0x2;
}

// FUNCTION: CARMA2_HW 0x0047b2e0
void C2_HOOK_FASTCALL MasterEnableFunkotronic(int pFunk_index) {

    gFunkotronics_array[pFunk_index].flags &= ~0x2;
}

// FUNCTION: CARMA2_HW 0x004f9760
int C2_HOOK_FASTCALL RestorePixelmap(br_material* pMaterial) {

    if (pMaterial->colour_map != (br_pixelmap*)pMaterial->user) {
        pMaterial->colour_map = (br_pixelmap*)pMaterial->user;
        BrMaterialUpdate(pMaterial, BR_MATU_ALL);
        return 1;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0040e700
void C2_HOOK_FASTCALL MungeCarMaterials(tCar_spec* pCar, int pInternal_cam) {
    int i;
    tCarCockpitMaterial* cockpit_material;

    for (i = 0, cockpit_material = &pCar->window_materials[0]; i < pCar->count_window_materials; i++, cockpit_material++) {
        int two_sided_material = pInternal_cam;

        if (pInternal_cam) {
            int j;

            for (j = 0; j < cockpit_material->count_maps; j++) {
                if (cockpit_material->material->colour_map == cockpit_material->maps[j]) {
                    two_sided_material = 0;
                }
            }
        }
        if (two_sided_material) {
            cockpit_material->material->flags &= ~BR_MATF_TWO_SIDED;
        }
        if (!two_sided_material) {
            cockpit_material->material->flags |= BR_MATF_TWO_SIDED;
        }
        BrMaterialUpdate(cockpit_material->material, BR_MATU_RENDERING);
    }
}

// FUNCTION: CARMA2_HW 0x0040f510
void C2_HOOK_FASTCALL CheckDisablePlingMaterials(tCar_spec* pCar) {
    br_matrix34* mat;
    br_scalar height;
    int i;

    height = 0.f;
    if (pCar->collision_info->water_d != 10000.f) {
        mat = &pCar->car_master_actor->t.t.mat;
        for (i = 0; i < 3; i++) {
            if (mat->m[i][1] > 0.f) {
                height += pCar->collision_info->bb2.max.v[i] * mat->m[i][1];
            } else {
                height += pCar->collision_info->bb2.min.v[i] * mat->m[i][1];
            }
        }
        if (mat->m[3][1] / WORLD_SCALE + height < pCar->collision_info->water_d) {
            DisablePlingMaterials();
        }
    } else {
        DisablePlingMaterials();
    }
}

// FUNCTION: CARMA2_HW 0x0040f590
void C2_HOOK_FASTCALL PositionCarMountedCamera(tCar_spec* pCar, tU32 pTime) {

    NOT_IMPLEMENTED();
}

typedef struct {
    int meter;
    undefined4 field_0x04[12];
    tCar_spec* car;
    undefined4 field_0x38[39];
} tRace_board_entry;

// GLOBAL: CARMA2_HW 0x0074bd54
tRace_board_entry gRace_board[8];

// FUNCTION: CARMA2_HW 0x0040f4a0
tCar_spec* C2_HOOK_FASTCALL GetRaceLeader(void) {
    int i;
    int best;
    tCar_spec* leader;

    leader = gRace_board[0].car;
    best = gRace_board[0].meter;
    if (gCurrent_net_game->type == eNet_game_type_foxy) {
        i = gIt_or_fox;
        if (i >= 0 && i < gNumber_of_net_players) {
            return gRace_board[i].car;
        }
    }
    if (gNumber_of_net_players > 1) {
        for (i = 1; i < gNumber_of_net_players; i++) {
            if (gRace_board[i].meter < best) {
                best = gRace_board[i].meter;
                leader = gRace_board[i].car;
            }
        }
    }
    return leader;
}

void C2_HOOK_FASTCALL CheckCameraHither(void) {
    br_camera *cam;
    // GLOBAL: CARMA2_HW 0x0067931c
    static int old_hither;

    cam = gCamera->type_data;
    if (TestForNan(&cam->hither_z)) {
        cam->hither_z = (float)old_hither;
    }
    old_hither = (int)cam->hither_z;
}

void C2_HOOK_FASTCALL AmIGettingBoredWatchingCameraSpin(void) {
    // GLOBAL: CARMA2_HW 0x00679298
    static tU32 time_of_death;

    // GLOBAL: CARMA2_HW 0x00679320
    static tU32 headup_timer;
    char s[256];

    if (gNet_mode != eNet_mode_none
            && (gCurrent_net_game->type == eNet_game_type_4
                    || gCurrent_net_game->type == eNet_game_type_fight_to_death)) {
        if (!gRace_finished) {
            time_of_death = 0;
            gOpponent_viewing_mode = 0;
        } else if (time_of_death == 0) {
            time_of_death = GetRaceTime();
        } else if (GetRaceTime() >= time_of_death + 10000) {
            if (gOpponent_viewing_mode == 0) {
                gOpponent_viewing_mode = 1;
                gNet_player_to_view_index = -2;
                ViewNetPlayer();
            }
            if (gNet_player_to_view_index >= gNumber_of_net_players) {
                gNet_player_to_view_index = -2;
                ViewNetPlayer();
            }
            if (gNet_player_to_view_index < 0 && gCar_to_view != GetRaceLeader()) {
                gNet_player_to_view_index = -2;
                ViewNetPlayer();
            }
            if ((GetRaceTime() > headup_timer + 1000 || headup_timer > GetRaceTime()) && gRace_over_reason == eRace_not_over_yet) {
                strcpy(s, GetMiscString(eMiscString_watching));
                strcat(s, " ");
                if (gNet_player_to_view_index >= 0) {
                    strcat(s, gNet_players[gNet_player_to_view_index].player_name);
                } else {
                    strcat(s, GetMiscString(eMiscString_race_leader));
                }
                headup_timer = GetRaceTime();
                NewTextHeadupSlot(6, 0, 500, -4, s);
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x00410be0
void C2_HOOK_FASTCALL SaveCameraPosition(int i) {

    if (gSave_camera[i].saved != 1) {
        gSave_camera[i].zoom = gCamera_zoom;
        gSave_camera[i].yaw = gCamera_yaw;
        gSave_camera[i].saved = 1;
    }
}

// FUNCTION: CARMA2_HW 0x00410c20
void C2_HOOK_FASTCALL RestoreCameraPosition(int i) {

    if (gSave_camera[i].saved != 0) {
        gCamera_zoom = gSave_camera[i].zoom;
        gCamera_yaw = gSave_camera[i].yaw;
        gSave_camera[i].saved = 0;
    }
}

void C2_HOOK_FASTCALL DoCameraControls(tCamera_key_flags *pCamera_controls, tU32 pTime_difference) {
    int flag;
    int swirl_mode;
    int up_and_down_mode;
    int going_down;
    // GLOBAL: CARMA2_HW 0x0067930c
    static int last_swirl_mode;

    flag = 0;
    swirl_mode = !GetRuntimeVariable(99) && gRace_finished && !gAction_replay_mode && (gCar_to_view == &gProgram_state.current_car || gCar_to_view->knackered);
    up_and_down_mode = swirl_mode && !gCamera_has_collided;
    going_down = gCamera_zoom > 1.0;
    if (last_swirl_mode != swirl_mode) {
        if (swirl_mode) {
            SaveCameraPosition(0);
        } else {
            RestoreCameraPosition(0);
        }
        last_swirl_mode = swirl_mode;
    }
    if (gMap_view != 2 && !gProgram_state.cockpit_on && (!gAction_replay_mode || gAction_replay_camera_mode < kActionReplayCameraMode_Panning)) {
        if (pCamera_controls->field_0x0_bit2 || (up_and_down_mode && going_down)) {
            gCamera_zoom += (float)pTime_difference * 5.f / 10000.f / (float)(2 * swirl_mode + 1);
            if (gCamera_zoom > 2.f) {
                gCamera_zoom = 2.f;
            }
            if (up_and_down_mode && gCamera_zoom > 1.f) {
                gCamera_zoom = 1.f;
            }
        }
        if (pCamera_controls->field_0x0_bit1 || (up_and_down_mode && !going_down)) {
            float zoom;

            gCamera_zoom -= (float)pTime_difference * 5.f / 10000.f / (float)(2 * swirl_mode + 1);
            if (gAction_replay_camera_mode == kActionReplayCameraMode_Peds) {
                zoom = 0.001f;
            } else {
                zoom = 0.1f;
            }
            if (gCamera_zoom < zoom) {
                gCamera_zoom = zoom;
                if (up_and_down_mode) {
                    if (gCamera_zoom < 1.0f) {
                        gCamera_zoom = 1.0f;
                    }
                }
            }
        }
        if (swirl_mode && gProgram_state.current_car.speedo_speed < 0.001449275362318841) {
            pCamera_controls->field_0x0_bit4 = 0;
            pCamera_controls->field_0x0_bit3 = 1;
        }

        if (gCamera_sign ? pCamera_controls->field_0x0_bit3 : pCamera_controls->field_0x0_bit4) {
            if (!gCamera_reset) {
                gCamera_yaw += BrDegreeToAngle((float)pTime_difference * 0.05f);
            }
            flag = 1;
        }
        if (gCamera_sign ? pCamera_controls->field_0x0_bit3 : pCamera_controls->field_0x0_bit4) {
            if (!gCamera_reset) {
                gCamera_yaw -= BrDegreeToAngle((float)pTime_difference * 0.05f);
            }
            if (flag) {
                gCamera_yaw = 0;
                gCamera_reset = 1;
            }
        } else {
            if (!flag) {
                gCamera_reset = 0;
            }
        }
    }
}

void C2_HOOK_FASTCALL MoveWithWheels(tCar_spec* pCar, br_vector3* pDir, int pManual_swing) {
    br_angle yaw;
    br_angle theta;
    // GLOBAL: CARMA2_HW 0x0058f638
    static int move_with_wheels = 1;


    if (pCar != NULL && pCar->speed <= 0.0001f && !gCamera_mode) {
        if (pManual_swing) {
            if (gCamera_yaw > BrDegreeToAngle(180)) {
                yaw = gCamera_yaw - BrDegreeToAngle(180);
            } else {
                yaw = gCamera_yaw;
            }
            if (yaw > BrDegreeToAngle(45) && yaw < BrDegreeToAngle(135)) {
                if (move_with_wheels) {
                    theta = BrRadianToAngle(atan2f(pCar->wpos[0].v[2] * pCar->curvature, 1.f));
                    gCamera_yaw += (-2 * gCamera_sign + 1) * theta;
                    move_with_wheels = 0;
                }
            } else {
                if (!move_with_wheels) {
                    theta = BrRadianToAngle(atan2f(pCar->wpos[0].v[2] * pCar->curvature, 1.f));
                    gCamera_yaw -= (-2 * gCamera_sign + 1) * theta;
                    move_with_wheels = 1;
                }
            }
        }
        if (move_with_wheels) {
            if (!gCar_flying && gAction_replay_camera_mode != kActionReplayCameraMode_Rigid) {
                theta = BrRadianToAngle(atan2f(pCar->wpos[0].v[2] * pCar->curvature, 1.f));
                DrVector3RotateY(pDir, theta);
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x00410c60
void C2_HOOK_FASTCALL GeneralisedPositionExternalCamera(tCar_spec* pCar, br_matrix34* pMat, br_vector3* pPos, float pSpeed, float pSpeedo_speed, br_vector3* pDirection, br_vector3* pOmega, tU32 pTime_difference) {
    br_vector3 old_camera_pos;
    br_matrix34* m1;
    int swoop;

    m1 = &gCamera->t.t.mat;
    swoop = gCountdown && gCamera_height > pPos->v[1] + 0.001f;
    BrVector3Copy(&old_camera_pos, &gCamera->t.t.translate.t);
    if (!gProgram_state.cockpit_on) {
        int manual_swing;
        float d;
        float l;
        float height_inc;
        float time;

        DoCameraControls(&gCamera_key_flags, pTime_difference);
        manual_swing = gCamera_key_flags.field_0x0_bit4 || gCamera_key_flags.field_0x0_bit3 || swoop;
        if (swoop) {
            gCamera_yaw = 0;
        }
        if (fabsf(pSpeedo_speed) > 0.0006f && gCamera_mode > 0) {
            gCamera_mode = -1;
            if (gAction_replay_camera_mode == kActionReplayCameraMode_Standard) {
                gCamera_sign = 0;
            } else if (BrVector3Dot((br_vector3*)pMat->m[2], pDirection) <= 0.f) {
                gCamera_sign = 0;
            } else {
                gCamera_sign = 1;
            }
        }
        if (pCar != NULL && pCar->frame_collision_flag && gCamera_mode != -2
                && (!gAction_replay_mode || ARReplayForwards())) {
            gCamera_mode = 1;
        }
        if (gCar_flying || gCamera_reset
                || gCamera_mode == -2 || gAction_replay_camera_mode == kActionReplayCameraMode_Rigid) {
            gCamera_mode = 0;
        }
        d = sqrtf(gCamera_zoom) + 4.f / WORLD_SCALE;
        if (!gCamera_mode || gCamera_mode == -1) {
            br_vector3 vn;
            br_vector3 a;

            if (gAction_replay_camera_mode == kActionReplayCameraMode_Rigid) {
                BrVector3Negate(&vn, (br_vector3 *) pMat->m[2]);
            } else {
                BrVector3Copy(&vn, pDirection);
            }
            MoveWithWheels(pCar, &vn, manual_swing);
            vn.v[1] = 0.0f;
            BrVector3Normalise(&vn, &vn);
            vn.v[1] = 0.0f;
            if (gCar_flying || gAction_replay_camera_mode == kActionReplayCameraMode_Rigid) {
                gCamera_sign = 0;
            }
            if (!gAction_replay_mode || !ARReplayIsReallyPaused()) {
                SwingCamera(pMat, m1, &vn, pOmega, pSpeed, pSpeedo_speed, pTime_difference, pCar);
            } else {
                SwingCamera(pMat, m1, &vn, pOmega, pSpeed, pSpeedo_speed, 0, pCar);
            }
            BrVector3Scale(&a, &vn, d);
            BrVector3Sub(&gCamera->t.t.translate.t, pPos, &a);
            BrVector3Copy(&gView_direction, &vn);
        } else {
            gUNK_006792f4 = 0;
            gUNK_006792f8 = 0;
        }
        if (gCamera_mode == 1) {
            br_vector3 vn;
            br_vector3 a;
            br_scalar dist;
            br_scalar l;

            BrVector3Sub(&a, pPos, &old_camera_pos);
            BrVector3Copy(&old_camera_pos, &gCamera_pos_before_collide);
            a.v[1] = 0.0f;
            if (manual_swing) {
                DrVector3RotateY(&a, (gCamera_sign == 0 ? 1 : -1) * (gCamera_yaw - gOld_yaw__car));
                gCamera_yaw = gOld_yaw__car;
            }
            BrVector3Normalise(&vn, &a);
            if (gAction_replay_camera_mode != kActionReplayCameraMode_Standard || manual_swing
                    || BrVector3Dot((br_vector3*)pMat->m[2], &vn) < BrVector3Dot((br_vector3*)pMat->m[2], &gView_direction)) {
                BrVector3Copy(&gView_direction, &vn);
            }
            BrVector3Scale(&vn, &vn, -d);
            BrVector3Accumulate(&a, &vn);
            dist = BrVector3Length(&a);
            l = (float)pTime_difference / 1000.0f * (dist + 1.0f) / dist;
            if (gAction_replay_camera_mode != kActionReplayCameraMode_Standard
                    && l < 1.0f && BrVector3Dot(&a, &vn) > 0.0f) {
                BrVector3Scale(&a, &a, l - 1.f);
                BrVector3Accumulate(&vn, &a);
            }
            BrVector3Add(&gCamera->t.t.translate.t, pPos, &vn);
        }

        height_inc = gCamera_zoom * gCamera_zoom + 0.3f;
        time = pTime_difference / 1000.f;
        if (!gCamera_frozen && (!gAction_replay_mode || !ARReplayIsReallyPaused())) {
            if (pTime_difference >= 5000) {
                gCamera_height = pPos->v[1];
            } else if (swoop) {
                if (time > 0.2f) {
                    time = 0.2f;
                }
                gCamera_height -= 5.0f * time;
                if (gCamera_height < pPos->v[1]) {
                    gCamera_height = pPos->v[1];
                }
            } else {
                gCamera_height += 5.0f * time * pPos->v[1];
                gCamera_height /= 5.0f * time + 1.0f;
            }
        }
        l = pDirection->v[1] * d;
        if (l > 0) {
            br_scalar new_height = pPos->v[1] - l - height_inc / 2.0f;
            if (new_height > gCamera_height) {
                gCamera_height = new_height;
            }
        }

        gCamera->t.t.translate.t.v[1] = height_inc + gCamera_height;
        BrVector3Copy(&gCamera_pos_before_collide, &gCamera->t.t.translate.t);
        CollideCameraWithOtherCars(pPos, &gCamera->t.t.translate.t);
        CollideCamera2(pPos, &gCamera->t.t.translate.t, &old_camera_pos,
            manual_swing || gCamera_key_flags.field_0x0_bit1 || gCamera_key_flags.field_0x0_bit2, pCar != NULL ? pCar->collision_info : NULL);
        if (gCamera_has_collided && swoop) {
            gCamera_height = pPos->v[1];
        }
        PointCameraAtCar(pPos, m1, 1.f);
    }
    gOld_yaw__car = gCamera_yaw;
    gOld_zoom = (int)gCamera_zoom;
}

void C2_HOOK_FASTCALL NormalPositionExternalCamera(tCar_spec* pCar, tU32 pTime_difference) {

    GeneralisedPositionExternalCamera(pCar, &pCar->car_master_actor->t.t.mat,
        &pCar->pos, pCar->speed, pCar->speedo_speed, &pCar->direction, &pCar->collision_info->omega, pTime_difference);
}

void C2_HOOK_FASTCALL SetPanningFieldOfView(void) {
    br_camera* camera_ptr;

    camera_ptr = gCamera->type_data;
    if (gPanning_camera_angle == 0) {
        gPanning_camera_angle = BrDegreeToAngle(gCamera_angle * 0.7f);
    }
    camera_ptr->field_of_view = gPanning_camera_angle;
}

// FUNCTION: CARMA2_HW 0x0040ef90
void C2_HOOK_FASTCALL FrozenCamera(tCar_spec* pCar, tU32 pTime) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL PositionPedCam(tPed_character_instance* pPed_character, tU32 pTime) {

    if (pPed_character == NULL) {
        ChangeCameraType();
    } else {
        br_matrix34* mat;

        mat = GetCharacterMatrixPtr(pPed_character);
        GeneralisedPositionExternalCamera(NULL, mat, (br_vector3*)mat->m[3],
            0.f, 0.f, &pPed_character->field_0xc0, &gZero_v__car, pTime);
    }
}

void C2_HOOK_FASTCALL PositionDroneCam(tU32 pTime_difference) {

    if (OKToViewDrones()) {
        br_matrix34* mat;
        br_vector3* dir;

        mat = GetCurrentViewDroneMat();
        dir = GetCurrentViewDroneDirection();
        GeneralisedPositionExternalCamera(NULL, mat, (br_vector3*)mat->m[3], 0.f, 0.f, dir, &gZero_v__car, pTime_difference);
    }
}

// FUNCTION: CARMA2_HW 0x00411980
void C2_HOOK_FASTCALL SwingCamera(br_matrix34* pM1, br_matrix34* pM2, br_vector3* pVn, br_vector3* pOmega, float pSpeed, float pSpeedo_speed, tU32 pTime_difference, tCar_spec* pCar) {
    br_angle yaw;
    br_scalar cos_dtheta;
    br_scalar sign;
    int manual_swing;
    br_scalar v16;
    br_scalar v17;
    br_scalar v18;
    br_scalar abs_v18;
    br_angle v8;
    br_angle v9;
    // GLOBAL: CARMA2_HW 0x0058f63c
    static int elapsed_time = -1;
    // GLOBAL: CARMA2_HW 0x00679338
    static br_vector3 old_vn;

    manual_swing = gOld_yaw__car != gCamera_yaw;
    if (elapsed_time > 500) {
        elapsed_time = -1;
    }
    if (elapsed_time >= 0) {
        elapsed_time += pTime_difference;
    }
    sign = -BrVector3Dot((br_vector3*)pM1->m[2], pVn);

    if ((sign < 0.0f) == gCamera_sign) {
        elapsed_time = -1;
    } else if (BrVector3Dot(pVn, &old_vn) <= 0.0 || elapsed_time >= 0) {
        if (gAction_replay_camera_mode != kActionReplayCameraMode_Standard || gCamera_sign) {
            if (elapsed_time < 0) {
                elapsed_time = 0;
            }
            if (elapsed_time < 500 && sign <= 0.0f) {
                BrVector3Negate(pVn, pVn);
            } else {
                gCamera_sign = !gCamera_sign;
                gUNK_006792f4 = BrDegreeToAngle(200.f * (gUNK_006792f8 ? .06f : .04f));

                if (gCamera_yaw > BR_ANGLE_DEG(180)) {
                    yaw = gCamera_yaw - BR_ANGLE_DEG(180);
                } else {
                    yaw = gCamera_yaw;
                }
                if (gCamera_yaw + BR_ANGLE_DEG(90) > BR_ANGLE_DEG(180)) {
                    gCamera_yaw = BR_ANGLE_DEG(180) - gCamera_yaw;
                } else if (yaw > BR_ANGLE_DEG(45) && yaw < BR_ANGLE_DEG(135)) {
                    gCamera_yaw = BR_ANGLE_DEG(180) - gCamera_yaw;
                }
            }
        }
    } else {
        gCamera_sign = !gCamera_sign;
        if (gCamera_yaw > BR_ANGLE_DEG(180)) {
            yaw = gCamera_yaw - BR_ANGLE_DEG(180);
        } else {
            yaw = gCamera_yaw;
        }
        if (yaw > BR_ANGLE_DEG(45) && yaw < BR_ANGLE_DEG(135)) {
            gCamera_yaw = -gCamera_yaw;
        }
    }
    BrVector3Copy(&old_vn, pVn);
    if (gCamera_sign) {
        yaw = -gCamera_yaw;
    } else {
        yaw = gCamera_yaw;
    }
    if (!gCar_flying) {
        DrVector3RotateY(pVn, yaw);
    }
    v16 = pVn->v[0] * gView_direction.v[0] + pVn->v[2] * gView_direction.v[2];
    v17 = pVn->v[0] * gView_direction.v[2] - pVn->v[2] * gView_direction.v[0];

    if (v16 < 0.5f && gCamera_yaw == 0) {
        gUNK_006792f8 = 1;
    }

    v18 = pOmega->v[0] * pM1->m[0][1] + pOmega->v[1] * pM1->m[1][1] + pOmega->v[2] * pM1->m[2][1];
    abs_v18 = fabsf(v18);
    v8 = BrRadianToAngle((float)pTime_difference * (abs_v18 + CARPOCALYPSE2_PI_F / 36.f) / 1000.f);
    v9 = BrRadianToAngle((float)sqrt(fabsf(v17)));

    if (!(gUNK_006792f4 == 0 && v16 > 0.f && v9 < v8) && !gCar_flying && !manual_swing) {
        br_angle omega;

        if (gUNK_006792f4 == 0) {
            gUNK_006792f4 = BrDegreeToAngle((gUNK_006792f8 ? .06f : .04f) * 50.f);
        }
        omega = pTime_difference * gUNK_006792f4 / 100;
        if (omega < v8) {
            omega = v8;
        }
        cos_dtheta = BR_COS(omega);
        if (cos_dtheta > v16) {
            br_scalar ts;

            if (v16 < -.7f && abs_v18 > 0.8f && v18 * v17 > 0.f) {
                omega = -omega;
            }
            ts = BrAngleToRadian(omega);
            if (v17 > 0.f) {
                pVn->v[0] = sinf(ts) * gView_direction.v[2] + cosf(ts) * gView_direction.v[0];
                pVn->v[2] = cosf(ts) * gView_direction.v[2] - sinf(ts) * gView_direction.v[0];
            } else {
                pVn->v[0] = cosf(ts) * gView_direction.v[0] - sinf(ts) * gView_direction.v[2];
                pVn->v[2] = sinf(ts) * gView_direction.v[0] + cosf(ts) * gView_direction.v[2];
            }
            gUNK_006792f4 += BrDegreeToAngle(pTime_difference * (gUNK_006792f8 ? .06f : .04f));

            if (gUNK_006792f4 > BrDegreeToAngle(gUNK_006792f8 ? 40.f : 10.f)) {
                gUNK_006792f4 = BrDegreeToAngle(gUNK_006792f8 ? 40.f : 10.f);
            }
            return;
        }
    }
    gUNK_006792f8 = 0;
    gCamera_mode = 0;
    gUNK_006792f4 = 0;
}

// FUNCTION: CARMA2_HW 0x00413570
int C2_HOOK_FASTCALL CollideCameraWithOtherCars(br_vector3* pPos, br_vector3* pCamera_pos) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x00411fc0
void C2_HOOK_FASTCALL PointCameraAtCar(br_vector3* pPos, br_matrix34* pMat, float pFov_factor) {
    br_vector3 vn;
    br_vector3 tv;
    br_vector3 tv2;
    br_scalar dist;
    br_scalar frac;
    br_angle theta;
    br_vector3* pos;
    br_camera* camera_ptr;
    int swoop;

    camera_ptr = gCamera->type_data;
    theta = (br_angle)(pFov_factor * camera_ptr->field_of_view / 5.f);
    swoop = gCountdown && gCamera_height > pPos->v[1] + 0.01f;
    if (swoop) {
        BrVector3Sub(&tv, &gAverage_grid_position, pPos);
        frac = (gCamera_height - pPos->v[1]) / 10.0f;
        BrVector3Scale(&tv, &tv, frac);
        BrVector3Add(&tv, pPos, &tv);
        pos = &tv;
        theta = (br_angle)((1.0f - frac) * (float)theta);
    } else {
        pos = pPos;
    }
    BrVector3Sub(&vn, pPos, (br_vector3*)pMat->m[3]);
    vn.v[1] = 0.f;
    BrVector3Normalise(&vn, &vn);
    pMat->m[0][0] = -vn.v[2];
    pMat->m[0][1] = 0.0f;
    pMat->m[0][2] = vn.v[0];
    pMat->m[1][0] = 0.0f;
    pMat->m[1][1] = 1.0f;
    pMat->m[1][2] = 0.0f;
    pMat->m[2][0] = -vn.v[0];
    pMat->m[2][1] = 0.0f;
    pMat->m[2][2] = -vn.v[2];
    BrVector3Sub(&tv2, pos, (br_vector3*)pMat->m[3]);
    dist = BrVector3Dot(&tv2, &vn);
    BrMatrix34PreRotateX(pMat, theta - BrRadianToAngle(atan2f(pMat->m[3][1] - pos->v[1], dist)));
}

// FUNCTION: CARMA2_HW 0x00414ca0
int C2_HOOK_FASTCALL IsCarInTheSea(void) {

    return gInTheSea;
}

// FUNCTION: CARMA2_HW 0x0043b840
float C2_HOOK_FASTCALL RepairCar(tU16 pCar_ID, tU32 pFrame_period, br_scalar* pTotal_deflection) {

    NOT_IMPLEMENTED();
    return 0.f;
}

// FUNCTION: CARMA2_HW 0x0041e580
void C2_HOOK_FASTCALL CancelPendingCunningStunt(void) {

    gQuite_wild_end = 0;
    gQuite_wild_start = 0;
    gOn_me_wheels_start = 0;
    gWoz_upside_down_at_all = 0;
    gWild_start = 0;
}

// FUNCTION: CARMA2_HW 0x004157e0
void C2_HOOK_FASTCALL CalcGraphicalWheelStuff(tCar_spec* pCar) {

    pCar->steering_angle = BrRadianToDegree(atanf((pCar->wpos[0].v[2] - pCar->wpos[2].v[2]) * (pCar->field_0x1260 + pCar->curvature)));
    pCar->lr_sus_position = (pCar->wpos[0].v[1] - pCar->oldd[0]) / WORLD_SCALE;
    pCar->rr_sus_position = (pCar->wpos[1].v[1] - pCar->oldd[1]) / WORLD_SCALE;
    pCar->lf_sus_position = (pCar->wpos[2].v[1] - pCar->oldd[2]) / WORLD_SCALE;
    pCar->rf_sus_position = (pCar->wpos[3].v[1] - pCar->oldd[3]) / WORLD_SCALE;
    PipeSingleGraphicalWheelStuff(pCar);
}

// FUNCTION: CARMA2_HW 0x00416500
void C2_HOOK_FASTCALL FinishCars(tU32 pLast_frame_time, tU32 pTime) {
    int i;

    if (gCar_flying) {
        BrMatrix34Copy(&gCar_to_view->collision_info->actor->t.t.mat,
            &gCar_to_view->collision_info->transform_matrix);
    }
    for (i = 0; i < gNum_cars_and_non_cars; i++) {
        tCar_spec* car;
        float original_speed;
        br_vector3 minus_k;

        car = gActive_car_list[i];
        BrMatrix34ApplyP(&car->pos, &car->collision_info->cmpos, &car->car_master_actor->t.t.mat);
        original_speed = car->speed;
        car->speed = BR_LENGTH2(car->collision_info->v.v[0], car->collision_info->v.v[2]) / 1000.f;
        BrVector3Negate(&minus_k, (br_vector3*)&car->car_master_actor->t.t.mat.m[2]);
        if (original_speed < 0.0001f && car->speed > 0.0001f) {
            car->speed = .0001f;
        }
        if (car->speed > .0001f) {
            BrVector3Normalise(&car->direction, &car->collision_info->v);
        } else {
            float ts;
            if (BrVector3Dot(&car->direction, &minus_k) < 0.f ) {
                ts = 1.f;
            } else {
                ts = -1.f;
            }
            BrVector3SetFloat(&minus_k, 0.f, 0.f, ts);
            BrMatrix34ApplyV(&car->direction, &minus_k, &car->car_master_actor->t.t.mat);
        }
        if (car != NULL && car->driver > 5) {
            int wheel;

            car->speedo_speed = BrVector3Dot(&minus_k, &car->collision_info->v) / 1000.f;
            CalcGraphicalWheelStuff(car);

            for (wheel = 0; wheel < 4; wheel++) {
                if (car->oldd[wheel] < car->susp_height[wheel / 2] && gCurrent_race.material_modifiers[car->material_index[wheel]].smoke_type >= 2 && !car->collision_info->disable_move_rotate) {
                    GenerateContinuousSmoke(car, wheel, pTime);
                }
            }
        }
    }
    if (pLast_frame_time < gPHIL_last_physics_tick && gCar_to_view->speed > .0001f) {
        br_vector3 tv;
        br_scalar dt;

        dt = (gPHIL_last_physics_tick - pLast_frame_time) / 40.f;
        BrVector3Sub(&tv, &gCar_to_view_original_v, &gCar_to_view->collision_info->v);
        BrVector3Scale(&tv, &tv, dt);
        BrVector3Accumulate(&tv, &gCar_to_view->collision_info->v);
        BrVector3Normalise(&gCar_to_view->direction, &tv);
    }
}

// FUNCTION: CARMA2_HW 0x004207a0
int C2_HOOK_FASTCALL PipeNonCarObject(tPhysics_object* pCollision_info, void* pUser_data) {

    NOT_IMPLEMENTED();
    return 0;
}

void C2_HOOK_FASTCALL PipeNonCars(void) {
    int i;

    ARStartPipingSession(ePipe_chunk_non_car);
    for (i = 0; i < gNum_active_non_cars; i++) {
        tNon_car_spec* non_car;

        non_car = gActive_non_car_list[i];
        if (non_car->car_ID != -1) {
            PhysicsObjectRecurse(non_car->collision_info, PipeNonCarObject, NULL);
        }
    }
    AREndPipingSession();
}

// FUNCTION: CARMA2_HW 0x004203c0
void C2_HOOK_FASTCALL CheckForDeAttachmentOfNonCars(tU32 pTime) {
    // GLOBAL: CARMA2_HW 0x00679418
    static tU32 total_time;
    int new_count;

    new_count = 0;
    if (gNum_active_non_cars != 0) {
        PipeNonCars();
        total_time += pTime;
        if (total_time >= 1000) {
            int i;

            total_time = 0;
            for (i = 0; i < gNum_active_non_cars; i++) {
                tNon_car_spec* non_car;

                non_car = gActive_non_car_list[i];

                if (non_car->actor->t.t.translate.t.v[1] < gMin_world_y - 10.f) {
                    non_car->collision_info->disable_move_rotate = 1;
                }
                if (TestForNan(&non_car->actor->t.t.translate.t.v[1])) {
                    BrVector3Set(&non_car->collision_info->omega, 0.f, 0.f, 0.f);
                    BrMatrix34Identity(&non_car->actor->t.t.mat);
                    BrVector3Set(&non_car->actor->t.t.translate.t, 2000.f, 0.f, 0.f);
                    non_car->collision_info->disable_move_rotate = 1;
                }
                gActive_non_car_list[new_count] = non_car;
                if (non_car->collision_info->disable_move_rotate && non_car->driver == eDriver_4) {
                    int j;
                    int drop;
                    br_actor *non_car_actor;

                    drop = 1;
                    non_car_actor = non_car->actor;
                    for (j = 0; j < gNum_cars_and_non_cars; j++) {
                        tCar_spec *car;

                        car = gActive_car_list[j];
                        if (car != (tCar_spec*)non_car && !car->collision_info->disable_move_rotate) {
                            br_matrix34 mat;
                            br_bounds3 bb;

                            BrMatrix34Mul(&mat, &non_car_actor->t.t.mat, &car->collision_info->field_0x144);
                            GetNewBoundingBox(&bb, &non_car_actor->model->bounds, &mat);
                            if (!(bb.min.v[0] > car->collision_info->field_0x124.max.v[0]
                                  || bb.min.v[1] > car->collision_info->field_0x124.max.v[1]
                                  || bb.min.v[2] > car->collision_info->field_0x124.max.v[2]
                                  || car->collision_info->field_0x124.min.v[0] > bb.max.v[0]
                                  || car->collision_info->field_0x124.min.v[1] > bb.max.v[1]
                                  || car->collision_info->field_0x124.min.v[2] > bb.max.v[2])) {
                                drop = 0;
                                break;
                            }
                        }
                    }
                    if (drop) {
                        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, field_0x17c, 0x17c);

                        if (non_car->flags & 0x10000) {
                            if (Vector3DistanceSquared(&non_car->collision_info->field_0x17c,
                                    (br_vector3*)non_car->collision_info->transform_matrix.m[3]) > CARPOCALYPSE2_SQR(.005f)) {
                                drop = 0;
                            } else {
                                BrVector3Copy((br_vector3 *) &non_car->collision_info->transform_matrix.m[3],
                                    &non_car->collision_info->field_0x17c);
                            }
                        }
                        if ((tCar_spec*)non_car != gCar_to_view) {
                            BrActorRemove(non_car_actor);
                            ClearSplashes(non_car->collision_info);
                            new_count -= 1;
                            non_car->driver = eDriver_non_car_unused_slot;
                            if (non_car->car_ID != -1) {
                                tU8 col_x;
                                tU8 col_z;
                                br_actor *parent;

                                XZToColumnXZ(&col_x, &col_z, non_car_actor->t.t.mat.m[3][0],
                                    non_car_actor->t.t.mat.m[3][2], &gProgram_state.track_spec);
                                parent = gTrack_actor;
                                if (gProgram_state.track_spec.columns[col_z][col_x].actor_0x0 != NULL) {
                                    parent = gProgram_state.track_spec.columns[col_z][col_x].actor_0x0;
                                }
                                BrActorAdd(parent, non_car_actor);
                            } else if (gAdditional_actors != NULL) {
                                BrActorAdd(gAdditional_actors, non_car_actor);
                            }
                            non_car_actor->type_data = NULL;
                            PHILRemoveObject(non_car->collision_info);
                        }
                    }
                }
                new_count += 1;
            }
            gNum_active_non_cars = new_count;
        }
    }
}


__inline void C2_HOOK_FASTCALL StopSkid(tCar_spec* pC) {

    if (gLast_car_to_skid[0] == pC) {
        DRS3StopSound(gSkid_tag[0]);
    }
    if (gLast_car_to_skid[1] == pC) {
        DRS3StopSound(gSkid_tag[1]);
    }
}

// FUNCTION: CARMA2_HW 0x00415890
void C2_HOOK_FASTCALL APTCPreCollision(void) {
    tPhysics_object* obj;
    tCar_spec* car;
    tNon_car_spec* non_car;
    tPhysics_object* node;
    void** wp;
    int i;
    float x;
    float f;

    gDrivable_on_count = 0;
    for (obj = gList_collision_infos; obj != NULL && gDrivable_on_count < 50; obj = obj->next) {
        if (obj->drivable_on) {
            gDrivable_on_list[gDrivable_on_count] = obj;
            gDrivable_on_count += 1;
        }
    }

    gCar_to_view_original_v.v[0] = gCar_to_view->collision_info->v.v[0];
    gCar_to_view_original_v.v[1] = gCar_to_view->collision_info->v.v[1];
    gCar_to_view_original_v.v[2] = gCar_to_view->collision_info->v.v[2];

    i = 0;
    if (i < gNum_active_cars) {
        wp = (void**)gActive_car_list;
        do {
            if (*wp != NULL && ((tCar_spec*)*wp)->driver > 5) {
                car = (tCar_spec*)*wp;
                BrMatrix34Copy(&car->car_master_actor->t.t.mat,
                               &car->collision_info->transform_matrix);
                if (car->collision_info->field_0x261 == 0x10
                        && car->collision_info->message_time == gPHIL_last_physics_tick) {
                    if (car == NULL || car->driver != eDriver_local_human) {
                        car->curvature = (float)car->collision_info->field_0x278 * car->maxcurve * 0.000030518509f;
                    }
                    if (gNet_mode == eNet_mode_host) {
                        car->collision_info->field_0xf0 = 1;
                    }
                    SetCollisionFlagsAndStuff(car);
                }
                if (car->disabled == 0) {
                    if (!(car->collision_info->disable_move_rotate != 0
                            && gPalette_fade_time != 0
                            && car != NULL
                            && car->driver == eDriver_local_human)) {
                        if (car->collision_info->box_face_ref != gFace_num__car
                                && (car->collision_info->box_face_ref != gFace_num__car - 1
                                    || car->collision_info->box_face_start <= gFace_count)) {
                            GetFacesInBox(car->collision_info, &gWorld_callbacks);
                        }
                        if (car->dt != 0.f) {
                            MoveAndCollideCar(car, 0.04f);
                        }
                        if (car->collision_info->child != NULL) {
                            PhysicsObjectMoveVelocityList(car->collision_info->child);
                            for (node = car->collision_info->child; node != NULL; node = node->next) {
                                AddDrag(car, node, 0.04f);
                                if (node->child != NULL) {
                                    DragChildren(car, node->child);
                                }
                            }
                            PositionChildren(car->collision_info);
                        }
                    }
                }
            }
            i++;
            wp++;
        } while (i < gNum_active_cars);
    }

    i = 0;
    if (i < gNum_active_non_cars) {
        wp = (void**)gActive_non_car_list;
        do {
            non_car = (tNon_car_spec*)*wp;
            if (non_car->collision_info->field_0x261 != 0
                    && non_car->collision_info->message_time == gPHIL_last_physics_tick) {
                SetCollisionFlagsAndStuff((tCar_spec*)non_car);
            }
            if (non_car->flags & 0x10000) {
                if (non_car->field_0xf8 != 0 && non_car->field_0xf8 < GetRaceTime()) {
                    non_car->field_0xf8 = 0;
                    non_car->flags = (non_car->flags & 0xffff0100) | 0x100;
                    non_car->collision_info->disable_move_rotate = 0;
                }
            }
            if (non_car->collision_info->disable_move_rotate == 0
                    || (non_car->flags & 0xff) != 0
                    || (non_car->flags & 0xff00) != 0) {
                non_car->collision_info->disable_move_rotate = 0;
                if (non_car->dt != 0.f) {
                    MoveAndCollideNonCar(non_car, 0.04f);
                }
                if (non_car->collision_info->child != NULL) {
                    PhysicsObjectMoveVelocityList(non_car->collision_info->child);
                    PositionChildren(non_car->collision_info);
                }
            }
            i++;
            wp++;
        } while (i < gNum_active_non_cars);
    }

    i = 0;
    if (i < gNum_active_cars) {
        wp = (void**)gActive_car_list;
        do {
            car = (tCar_spec*)*wp;
            if (car->field_0x4c8 != gZero) {
                x = car->field_0x4c8;
                f = car->collision_info->M; f *= x; car->collision_info->M = f;
                f = car->collision_info->I.v[0]; f *= x; car->collision_info->I.v[0] = f;
                f = car->collision_info->I.v[1]; f *= x; car->collision_info->I.v[1] = f;
                f = car->collision_info->I.v[2]; f *= x; car->collision_info->I.v[2] = f;
            }
            i++;
            wp++;
        } while (i < gNum_active_cars);
    }

    if (gCar_flying) {
        tCar_spec* c = gCar_to_view;

        c->collision_info->transform_matrix.m[3][1] -= gFneg1000;
        BrMatrix34Copy(&c->car_master_actor->t.t.mat,
                       &c->collision_info->transform_matrix);
        PositionChildren(c->collision_info);
        SetCollisionInfoChildsDoNothing(c->collision_info, 1);
    }
    MinePreCollisionStuff();
    PedPreCollisionStuff();
    DronePreCollisionStuff();
}

#pragma auto_inline(off)

// FUNCTION: CARMA2_HW 0x00415c40
void C2_HOOK_FASTCALL SetCollisionFlagsAndStuff(tCar_spec* pCar) {
    float temp;
    int i;

    if (gNet_mode == eNet_mode_host
            && pCar->collision_info->field_0x49c > pCar->collision_info->field_0x274) {
        pCar->collision_info->field_0x261 = 0;
        pCar->dt = -1.f;
        return;
    }

    switch (pCar->collision_info->field_0xf0) {
    case 0:
        SelectCrushNetwork(&pCar->collision_info->field_0x294, pCar->collision_info, 2);
        break;
    case 1:
        SelectCrushNetwork(&pCar->collision_info->field_0x294, pCar->collision_info, 1);
        break;
    case 2:
        SelectCrushNetwork(&pCar->collision_info->field_0x294, pCar->collision_info, 1);
        break;
    }

    if (pCar != NULL && pCar->driver > 5) {
        for (i = 0; i < 4; i++) {
            pCar->oldd[i] = (float)pCar->collision_info->field_0x26c[i]
                    * pCar->susp_height[i >> 1] * (1.f / 255.f);
        }

        if (pCar->driver == eDriver_net_human) {
            pCar->revs = (float)pCar->collision_info->field_0x27a;
        }

        if (pCar->driver >= eDriver_net_human
                && pCar->collision_info->field_0x27c > pCar->repair_time) {
            if (pCar->collision_info->field_0x27c - pCar->repair_time >= 100000) {
                TotallyRepairACar(pCar);
                pCar->repair_time = pCar->collision_info->field_0x27c;
            } else {
                ComputeCarImpact(pCar, pCar->collision_info->field_0x27c - pCar->repair_time, &temp);
            }
            for (i = 0; i < 12; i++) {
                pCar->damage_units[i].damage_level = (int)pCar->collision_info->field_0x280[i];
            }
            SetSmokeLastDamageLevel(pCar);
            StopCarSmoking(pCar);
        } else {
            for (i = 0; i < 12; i++) {
                pCar->damage_units[i].damage_level = (int)pCar->collision_info->field_0x280[i];
            }
            SortOutSmoke(pCar);
        }

        if (pCar->collision_info->disable_move_rotate != 0) {
            if (BrVector3LengthSquared(&pCar->collision_info->v) != 0.f) {
                pCar->collision_info->disable_move_rotate = 0;
            }
        }

        if (pCar->driver >= eDriver_net_human
                && (pCar->collision_info->field_0x292 & 1) != 0
                && (pCar->collision_info->field_0x290 != pCar->car_crush_spec->field_0x24
                        || pCar->driver == eDriver_local_human)) {
            if ((pCar->collision_info->field_0x292 & 2) != 0) {
                WeldCarByNetwork(pCar);
            } else if (pCar->car_crush_spec->field_0x144 != 0) {
                CrushCarByNetwork(pCar);
            }

            if ((pCar->collision_info->field_0x292 & 4) != 0) {
                WeldCarPartial(pCar);
            } else if (pCar->car_crush_spec->field_0x4b8 != 0) {
                WeldCar(pCar);
            }

            UpdateCrushVertices(pCar, (tU8*)&pCar->collision_info->field_0x2fc);
        }

        if (pCar->driver != eDriver_local_human) {
            for (i = 0; i < 4; i++) {
                pCar->wheel_dam_offset[i] = pCar->collision_info->field_0x48c[i];
            }
        }

        GetFacesInBox(pCar->collision_info, &gWorld_callbacks);
    }

    pCar->collision_info->field_0x261 = 0;
    if (gNet_mode == eNet_mode_client) {
        pCar->collision_info->field_0x49c = pCar->collision_info->field_0x274;
    }
}

// FUNCTION: CARMA2_HW 0x00415ef0
void C2_HOOK_FASTCALL AddDrag(tCar_spec* pCar, tPhysics_object* pObject, br_scalar pDt) {
    br_scalar drag;
    br_scalar k;
    br_vector3 v;

    drag = pDt * 0.0005f;
    if (pObject->last_special_volume != NULL) {
        k = pObject->last_special_volume->viscosity_multiplier;
        if (pCar->underwater_ability != 0) {
            k *= 0.6;
        }
        drag = k * drag * pObject->water_depth_factor;
    }

    k = BrVector3Length(&pObject->v) * drag * 6.9 / pObject->M;
    v.v[0] = pObject->v.v[0] * k;
    v.v[1] = pObject->v.v[1] * k;
    v.v[2] = pObject->v.v[2] * k;
    DRVector3Diminish(&pObject->v, &v);

    k = BrVector3Length(&pObject->omega);
    v.v[0] = pObject->omega.v[0] * (k * drag);
    v.v[1] = pObject->omega.v[1] * (k * drag);
    v.v[2] = pObject->omega.v[2] * (k * drag);
    ApplyWaterOmegaBrake(pObject, &v);
}

// FUNCTION: CARMA2_HW 0x00416030
void C2_HOOK_FASTCALL DragChildren(tCar_spec* pCar, tPhysics_object* pChild) {

    while (pChild != NULL) {
        AddDrag(pCar, pChild, 0.04f);
        if (pChild->child != NULL) {
            DragChildren(pCar, pChild->child);
        }
        pChild = pChild->next;
    }
}

// FUNCTION: CARMA2_HW 0x004168c0
void C2_HOOK_FASTCALL MoveAndCollideCar(tCar_spec* pCar, br_scalar pDt) {
    br_scalar wheel_spin_force;
    br_scalar ts;
    int new_gear;
    br_vector3 v;
    br_scalar scale;
    br_scalar k;
    int i;

    if (pCar->dt >= 0.f) {
        pDt = pCar->dt;
    }
    if (pDt == 0.f) {
        return;
    }
    if (gCar_flying && pCar == gCar_to_view) {
        return;
    }

    pCar->new_skidding = 0;
    if (pCar->collision_info->water_d != 10000.f) {
        TestAutoSpecialVolume(pCar->collision_info);
    }
    MungeSpecialVolume(pCar->collision_info);
    BrVector3Scale((br_vector3*)&pCar->car_master_actor->t.t.mat.m[3],
        (br_vector3*)&pCar->car_master_actor->t.t.mat.m[3], WORLD_SCALE);

    if (pCar->field_0x18c8 != 0) {
        if (pCar->collision_info->disable_move_rotate == 0) {
            k = pDt * 10.f;
            if (pCar->collision_info->last_special_volume != NULL) {
                k *= pCar->collision_info->last_special_volume->gravity_multiplier;
            }
            pCar->collision_info->v.v[1] = pCar->collision_info->v.v[1] - k * (1.0 / 6.9);
        }
        if (pCar->field_0x18c8 == 2) {
            BrMatrix34TApplyV(&v, (br_vector3*)&gCamera->t.t.mat,
                &pCar->collision_info->actor->t.t.mat);
            scale = BR_LENGTH3(v.v[0], v.v[1], v.v[2]);
            if (scale > BR_SCALAR_EPSILON * 2) {
                scale = gZero / scale;
                v.v[0] = v.v[0] * scale;
                v.v[1] = v.v[1] * scale;
                v.v[2] = v.v[2] * scale;
            } else {
                v.v[0] = 1.f;
                v.v[1] = 0.f;
                v.v[2] = 0.f;
            }
            if (pCar->keys.dec) {
                pCar->collision_info->omega.v[0] += v.v[0];
                pCar->collision_info->omega.v[1] += v.v[1];
                pCar->collision_info->omega.v[2] += v.v[2];
            }
            if (pCar->keys.acc) {
                pCar->collision_info->omega.v[0] -= v.v[0];
                pCar->collision_info->omega.v[1] -= v.v[1];
                pCar->collision_info->omega.v[2] -= v.v[2];
            }
            scale = BR_LENGTH3(pCar->collision_info->velocity_car_space.v[0],
                pCar->collision_info->velocity_car_space.v[1],
                pCar->collision_info->velocity_car_space.v[2]);
            if (scale > BR_SCALAR_EPSILON * 2) {
                scale = gZero / scale;
                v.v[0] = pCar->collision_info->velocity_car_space.v[0] * scale;
                v.v[1] = pCar->collision_info->velocity_car_space.v[1] * scale;
                v.v[2] = pCar->collision_info->velocity_car_space.v[2] * scale;
            } else {
                v.v[0] = 1.f;
                v.v[1] = 0.f;
                v.v[2] = 0.f;
            }
            if (pCar->keys.right) {
                pCar->collision_info->omega.v[0] += v.v[0];
                pCar->collision_info->omega.v[1] += v.v[1];
                pCar->collision_info->omega.v[2] += v.v[2];
            }
            if (pCar->keys.left) {
                pCar->collision_info->omega.v[0] -= v.v[0];
                pCar->collision_info->omega.v[1] -= v.v[1];
                pCar->collision_info->omega.v[2] -= v.v[2];
            }
            if (pCar->keys.brake) {
                pCar->collision_info->omega.v[0] = 0.f;
                pCar->collision_info->omega.v[1] = 0.f;
                pCar->collision_info->omega.v[2] = 0.f;
            }
        }
    } else if (pCar == NULL || pCar->driver < eDriver_net_human) {
        CalcForce(pCar, pDt);
    } else {
        if (gCar_flying && pCar->driver == eDriver_local_human) {
            pCar->acc_force = 0.f;
        } else {
            CalcEngineForce(pCar, pDt);
        }
        CalcForce(pCar, pDt);

        ts = -BrVector3Dot((br_vector3*)&pCar->car_master_actor->t.t.mat.m[2],
            &pCar->collision_info->v) * 6.9;
        if (pCar->gear == 0) {
            pCar->target_revs = 0.f;
        } else {
            pCar->target_revs = ts / pCar->speed_revs_ratio / (double)pCar->gear;
        }
        if (pCar->target_revs < 0.f) {
            pCar->target_revs = 0.f;
            pCar->gear = 0;
        }
        if (!pCar->number_of_wheels_on_ground || ((pCar->wheel_slip & 2) + 1) != 0 || !pCar->gear) {
            if (!pCar->number_of_wheels_on_ground) {
                wheel_spin_force = pCar->torque * pCar->force_torque_ratio;
            } else {
                wheel_spin_force = pCar->torque * pCar->force_torque_ratio
                    - (double)pCar->gear * pCar->acc_force;
            }
            if (pCar->gear == 0) {
                if (pCar->revs > gF1000 && !pCar->keys.brake
                        && (pCar->keys.acc || pCar->joystick.acc > 0)
                        && gCountdown == 0) {
                    new_gear = pCar->keys.backwards ? -1 : 1;
                    pCar->gear = new_gear;
                    pCar->target_revs = ts / pCar->speed_revs_ratio / (double)new_gear;
                }
                wheel_spin_force = pCar->torque * pCar->force_torque_ratio;
            } else if (pCar->gear < 2 && (pCar->keys.dec || pCar->joystick.dec > 0)
                    && fabsf(ts) < 1.f && pCar->revs > gF1000) {
                pCar->gear = -pCar->gear;
            }
            pCar->revs = pCar->revs
                - wheel_spin_force / pCar->force_torque_ratio * pDt * -5000.;
            if (pCar->traction_control && wheel_spin_force > 0.f) {
                if (pCar->revs > pCar->target_revs && pCar->gear != 0
                        && pCar->target_revs > gF1000) {
                    pCar->revs = pCar->target_revs;
                }
            }
            if (pCar->revs <= 0.f) {
                pCar->revs = 0.f;
            }
        }
        while ((pCar->wheel_slip & 2) == 0 && pCar->target_revs > 6000.f
                && pCar->gear < pCar->max_gear && pCar->gear > 0
                && !pCar->just_changed_gear) {
            pCar->target_revs = pCar->target_revs * pCar->gear / (pCar->gear + 1);
            pCar->gear++;
        }
        while (pCar->gear > 1 && pCar->target_revs < 3000.f
                && !pCar->just_changed_gear) {
            pCar->target_revs = pCar->target_revs * pCar->gear / (pCar->gear - 1);
            pCar->gear--;
        }
        if (pCar->revs < 200.f && pCar->target_revs < 200.f && pCar->gear <= 1
                && !pCar->keys.acc && pCar->joystick.acc <= 0
                && !pCar->just_changed_gear) {
            pCar->gear = 0;
        }
        if (pCar->just_changed_gear && pCar->revs < 6000.f && pCar->revs > 200.f
                && (pCar->gear < 2 || pCar->revs >= 3000.f)) {
            pCar->just_changed_gear = 0;
        }
        if (pCar->revs >= 6000.f && (pCar->keys.acc || pCar->joystick.acc > 0)) {
            pCar->just_changed_gear = 0;
        }
    }

    BrVector3InvScale((br_vector3*)&pCar->car_master_actor->t.t.mat.m[3],
        (br_vector3*)&pCar->car_master_actor->t.t.mat.m[3], WORLD_SCALE);
    BrMatrix34ApplyP(&pCar->pos, &pCar->collision_info->cmpos,
        &pCar->collision_info->transform_matrix);
    for (i = 0; i < 4; i++) {
        SkidMark(pCar, i);
    }
}

// FUNCTION: CARMA2_HW 0x00418250
void C2_HOOK_FASTCALL MoveNonCar(tNon_car_spec* pNon_car, br_scalar pDt) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x00417030
void C2_HOOK_FASTCALL MoveAndCollideNonCar(tNon_car_spec* pNon_car, br_scalar pDt) {

    if (pNon_car->collision_info->water_d != 10000.f) {
        TestAutoSpecialVolume(pNon_car->collision_info);
    }
    MungeSpecialVolume(pNon_car->collision_info);
    if (pNon_car->dt >= 0.f) {
        pDt = pNon_car->dt;
    }
    MoveNonCar(pNon_car, pDt);
    BrMatrix34ApplyP(&pNon_car->pos, &pNon_car->centre_of_mass_world_scale, &pNon_car->actor->t.t.mat);
    pNon_car->pos.v[0] *= 1.f / WORLD_SCALE;
    pNon_car->pos.v[1] *= 1.f / WORLD_SCALE;
    pNon_car->pos.v[2] *= 1.f / WORLD_SCALE;
}

// FUNCTION: CARMA2_HW 0x004c2060
void C2_HOOK_FASTCALL GetFacesInBox(tPhysics_object* pCollision, tWorld_callbacks* pWorld_callbacks) {
    tBounds bnds;
    br_bounds xformed_bounds;
    br_bounds tmp_bounds;
    br_vector3 vel;
    br_matrix34 mat1;
    br_matrix34 mat2;
    br_matrix34 mat3;
    br_matrix34 mat4;
    int i;
    int flags;

    if (pCollision->flags & 0x40) {
        pCollision->box_face_start = pCollision->box_face_end;
        return;
    }
    if ((pCollision->flags & 0x2) && pCollision->parent != NULL) {
        pCollision->box_face_start = pCollision->parent->box_face_start;
        pCollision->box_face_end = pCollision->parent->box_face_end;
        pCollision->box_face_ref = pCollision->parent->box_face_ref;
        return;
    }
    if (pCollision->flags & 0x1) {
        if (pCollision->box_face_ref == gFace_num__car
                || (pCollision->box_face_ref == gFace_num__car - 1
                    && pCollision->box_face_start > gFace_count)) {
            if (pCollision->field_0x10c.min.v[0] > pCollision->field_0x124.min.v[0]
                    && pCollision->field_0x10c.min.v[1] > pCollision->field_0x124.min.v[1]
                    && pCollision->field_0x10c.min.v[2] > pCollision->field_0x124.min.v[2]
                    && pCollision->field_0x10c.max.v[0] < pCollision->field_0x124.max.v[0]
                    && pCollision->field_0x10c.max.v[1] < pCollision->field_0x124.max.v[1]
                    && pCollision->field_0x10c.max.v[2] < pCollision->field_0x124.max.v[2]) {
                return;
            }
        }
        BrVector3Scale(&vel, &pCollision->v, 0.12f);
        for (i = 0; i < 3; i++) {
            bnds.original_bounds.min.v[i] = pCollision->field_0x10c.min.v[i] - 0.15;
            bnds.original_bounds.max.v[i] = pCollision->field_0x10c.max.v[i] + 0.15;
            if (vel.v[i] < 0.f) {
                bnds.original_bounds.min.v[i] += vel.v[i];
            } else {
                bnds.original_bounds.max.v[i] += vel.v[i];
            }
            pCollision->field_0x124.min.v[i] = bnds.original_bounds.min.v[i] + 0.005;
            pCollision->field_0x124.max.v[i] = bnds.original_bounds.max.v[i] - 0.005;
        }
        bnds.mat = &mat3;
        BrMatrix34Identity(&mat3);
    } else {
        br_matrix34* actor_mat = &pCollision->actor->t.t.mat;
        br_matrix34* trans_mat = &pCollision->transform_matrix;

        if (pCollision->box_face_ref == gFace_num__car
                || (pCollision->box_face_ref == gFace_num__car - 1
                    && pCollision->box_face_start > gFace_count)) {
            BrMatrix34Mul(&mat1, actor_mat, &pCollision->field_0x144);
            GetNewBoundingBox(&xformed_bounds, &pCollision->bb2, &mat1);
            if (xformed_bounds.max.v[0] < pCollision->field_0x124.max.v[0]
                    && xformed_bounds.max.v[1] < pCollision->field_0x124.max.v[1]
                    && xformed_bounds.max.v[2] < pCollision->field_0x124.max.v[2]
                    && xformed_bounds.min.v[0] > pCollision->field_0x124.min.v[0]
                    && xformed_bounds.min.v[1] > pCollision->field_0x124.min.v[1]
                    && xformed_bounds.min.v[2] > pCollision->field_0x124.min.v[2]) {
                return;
            }
        }
        BrMatrix34LPInverse(&mat3, actor_mat);
        BrMatrix34Mul(&mat2, trans_mat, &mat3);
        GetNewBoundingBox(&bnds.original_bounds, &pCollision->bb2, &mat2);
        for (i = 0; i < 3; i++) {
            bnds.original_bounds.min.v[i] = bnds.original_bounds.min.v[i] < pCollision->bb2.min.v[i]
                ? bnds.original_bounds.min.v[i] : pCollision->bb2.min.v[i];
            bnds.original_bounds.max.v[i] = bnds.original_bounds.max.v[i] > pCollision->bb2.max.v[i]
                ? bnds.original_bounds.max.v[i] : pCollision->bb2.max.v[i];
            bnds.original_bounds.min.v[i] -= 0.0025f;
            bnds.original_bounds.max.v[i] += 0.0025f;
        }
        BrMatrix34Mul(&mat1, &mat2, &mat2);
        BrMatrix34Mul(&mat4, &mat1, &mat2);
        BrMatrix34LPInverse(&mat1, &mat4);
        GetNewBoundingBox(&tmp_bounds, &pCollision->bb2, &mat1);
        for (i = 0; i < 3; i++) {
            bnds.original_bounds.min.v[i] = bnds.original_bounds.min.v[i] < tmp_bounds.min.v[i]
                ? bnds.original_bounds.min.v[i] : tmp_bounds.min.v[i];
            bnds.original_bounds.max.v[i] = bnds.original_bounds.max.v[i] > tmp_bounds.max.v[i]
                ? bnds.original_bounds.max.v[i] : tmp_bounds.max.v[i];
            bnds.original_bounds.min.v[i] -= 0.02f;
            bnds.original_bounds.max.v[i] += 0.02f;
        }
        pCollision->field_0x124 = bnds.original_bounds;
        BrMatrix34Copy(&pCollision->field_0x144, &mat3);
        bnds.mat = actor_mat;
    }
    pCollision->box_face_start = gFace_count;
    gPling_face = NULL;
    flags = pCollision->flags;
    gActorBoxPick_StopGroovidelics = (tU8)(~flags) >> 7;
    if (pWorld_callbacks != NULL && pWorld_callbacks->find_faces_in_box != NULL) {
        gFace_count += pWorld_callbacks->find_faces_in_box(&bnds, &gFace_list__car[gFace_count], 300 - gFace_count, pWorld_callbacks);
        if (gFace_count >= 300) {
            pCollision->box_face_start = 0;
            gFace_count = pWorld_callbacks->find_faces_in_box(&bnds, gFace_list__car, 300, pWorld_callbacks);
            gFace_num__car++;
        }
    }
    pCollision->box_face_end = gFace_count;
    pCollision->box_face_ref = gFace_num__car;
    gActorBoxPick_StopGroovidelics = 1;
    if (pWorld_callbacks != NULL && pWorld_callbacks->new_face_list != NULL) {
        pWorld_callbacks->new_face_list(pCollision, gPling_face);
    }
    if (pCollision->flags & 0x80) {
        pCollision->box_face_end = pCollision->box_face_start;
    }
}

#pragma auto_inline(on)

// FUNCTION: CARMA2_HW 0x00416070
void C2_HOOK_FASTCALL APTCPostCollision(void) {
    void** wp;
    tCar_spec* car;
    int i;

    if (gCar_flying) {
        tCar_spec* c = gCar_to_view;
        c->collision_info->transform_matrix.m[3][1] -= gF1000;

        BrMatrix34Copy(&c->car_master_actor->t.t.mat,
                       &c->collision_info->transform_matrix);
        PositionChildren(c->collision_info);
        SetCollisionInfoChildsDoNothing(c->collision_info, 0);
    }

    i = 0;
    if (i < gNum_active_cars) {
        wp = (void**)gActive_car_list;
        do {
            float x;
            float fac;
            float f;

            car = (tCar_spec*)*wp;
            if (car->field_0x4c8 != gZero) {
                x = car->field_0x4c8;
                fac = (float)(gZero / x);
                f = car->collision_info->M; f *= fac; car->collision_info->M = f;
                f = car->collision_info->I.v[0]; f *= fac; car->collision_info->I.v[0] = f;
                f = car->collision_info->I.v[1]; f *= fac; car->collision_info->I.v[1] = f;
                f = car->collision_info->I.v[2]; f *= fac; car->collision_info->I.v[2] = f;
            }
            if (((tCar_spec*)*wp)->number_of_wheels_on_ground != 0) {
                SetCollisionInfoChildsDoNothing(((tCar_spec*)*wp)->collision_info, 0);
            }
            i++;
            wp++;
        } while (i < gNum_active_cars);
    }

    i = 0;
    if (i < gNum_cars_and_non_cars) {
        wp = (void**)gActive_car_list;
        do {
            car = (tCar_spec*)*wp;
            car->frame_collision_flag |= (int)(signed char)car->collision_info->collision_flag;
            i++;
            wp++;
        } while (i < gNum_cars_and_non_cars);
    }

    UpdateCrushTimers();
    if (gNet_mode) {
        UpdateCrushPainList();
    }
    APTCPostCollisionTree(gList_collision_infos);
}

// FUNCTION: CARMA2_HW 0x004161a0
void C2_HOOK_FASTCALL APTCPostCollisionTree(tPhysics_object* node) {
    tPhysics_object* child;
    void* owner;
    void* unk;

    while (node != NULL) {
        if (gNet_mode == eNet_mode_host) {
            int need = ((node->collision_flag & 2) != 0) || ((node->flags & 0x1000) != 0);

            for (child = node->child; child != NULL; child = child->next) {
                need |= child->collision_flag & 2;
            }
            if (need) {
                node->field_0x49c = gPHIL_last_physics_tick + 0x28;
                owner = node->owner;
                if (owner != NULL && node->flags_0x238 == 1 && *(int*)((char*)owner + 0xc) > 5) {
                    unk = *(void**)((char*)owner + 0x18d4);
                    if (unk != NULL) {
                        DeallocateTransientBitmap(*(int*)((char*)unk + 0x20));
                    }
                }
            }
        }
        node->disable_move_rotate &= 0xfd;
        APTCPostCollisionTree(node->child);
        node = node->next;
    }
}

// FUNCTION: CARMA2_HW 0x00416300
void C2_HOOK_FASTCALL APTCChangedObjects(tPhysics_object* pObject, undefined4 pArg2) {

    if (pObject != NULL && pObject->owner != NULL && pObject->flags_0x238 == 2) {
        BonerPedMovedByPhysics((tPed_character_instance*)pObject->owner, pArg2);
    }
    if (pObject != NULL && pObject->flags_0x238 == 0x10) {
        MoveMyDroneBaby(pObject, pArg2);
    }
}

// FUNCTION: CARMA2_HW 0x00416270
int C2_HOOK_FASTCALL APTCActiveHalted(tPhysics_object* pObject) {

    if (pObject != NULL && pObject->owner != NULL && pObject->flags_0x238 >= 2 && pObject->flags_0x238 <= 4) {
        return BonerActiveHalted((tPed_character_instance*)pObject->owner);
    }
    if (pObject != NULL && pObject->flags_0x238 == 0x10) {
        return MyDroneHathHalteth(pObject);
    }
    return 1;
}

// FUNCTION: CARMA2_HW 0x004162b0
int C2_HOOK_FASTCALL APTCPassiveActivated(tPhysics_object* pObject, undefined4 pArg2) {

    if (pObject != NULL && pObject->owner != NULL && pObject->flags_0x238 >= 2 && pObject->flags_0x238 <= 4) {
        return BonerPassiveCollision((tPed_character_instance*)pObject->owner, pArg2);
    }
    if (pObject != NULL && pObject->flags_0x238 == 0x10) {
        return MyDroneHathCollideth(pObject, (tPhysics_object*)pArg2);
    }
    return 1;
}

void C2_HOOK_FASTCALL GetNonCars(void) {
    int i;
    int j;

    gNum_cars_and_non_cars = gNum_active_non_cars + gNum_active_cars;
    for (i = gNum_active_cars, j = 0; i < gNum_cars_and_non_cars; i++, j++) {
        gActive_car_list[i] = (tCar_spec*)gActive_non_car_list[j];
    }
}

// FUNCTION: CARMA2_HW 0x00416340
void C2_HOOK_FASTCALL ApplyPhysicsToCars(tU32 pLast_tick_time, tU32 pFrame_period) {
    int i;

    if (gFreeze_mechanics) {
        return;
    }
    if (gNet_mode == eNet_mode_client) {
        ForceRebuildActiveCarList();
    }
    GetNonCars();

    last_frame_start = pLast_tick_time;
    for (i = 0; i < gNum_cars_and_non_cars; i++) {
        tCar_spec* car;

        car = gActive_car_list[i];
        car->frame_collision_flag = gOver_shoot && car->collision_info->collision_flag;
        if (car != NULL && car->driver > 5) {
            RecordLastDamage(car);
            if (car->driver == eDriver_oppo && gStop_opponents_moving) {
                car->acc_force = 0.f;
                car->brake_force = 0.f;
                car->keys.acc = 0;
                car->keys.dec = 0;
                car->joystick.acc = -1;
                car->joystick.dec = -1;
            }
            if (!car->wheel_slip) {
                StopSkid(car);
            }
            if (car->driver == eDriver_net_human && car->collision_info->message_time < pLast_tick_time - 1000) {
                car->keys.acc = 0;
                car->keys.dec = 0;
                car->joystick.acc = -1;
                car->joystick.dec = -1;
                car->keys.horn = 0;
            }
            SetSmokeLastDamageLevel(car);
        }
    }
    PHILDoPhysics(&gCar_physics_callbacks, pLast_tick_time, pFrame_period);
    if (TimeToSendData()) {
        SendCarData(gPHIL_last_physics_tick);
        SendMines(gPHIL_last_physics_tick);
    }
    FinishCars(pLast_tick_time + pFrame_period, pFrame_period);
    CheckForDeAttachmentOfNonCars(pFrame_period);
}

// FUNCTION: CARMA2_HW 0x00417fb0
void C2_HOOK_FASTCALL ApplyNonCarTumble(tPhysics_object* pObject, br_vector3* pV, br_scalar pTumbleFactor, br_scalar pTumbleThreshold) {
    br_vector3 cross;
    br_vector3 out;
    br_vector3 size;
    br_scalar len2;
    br_scalar speed;
    br_scalar over;
    br_scalar scale;
    br_scalar k;
    br_scalar nx;
    br_scalar ny;
    br_scalar nz;
    br_scalar new_len2;
    int i;

    if (pObject->collision_flag == 0 && pObject->disable_move_rotate == 0) {
        return;
    }

    len2 = BrVector3LengthSquared(pV);
    if (len2 < pTumbleThreshold * pTumbleThreshold) {
        return;
    }

    DRVector3SafeCross(&cross, &gFace_list__car[pObject->box_face_start].normal, pV);

    scale = BR_LENGTH3(cross.v[0], cross.v[1], cross.v[2]);
    if (scale > BR_SCALAR_EPSILON * 2) {
        scale = gZero / scale;
        cross.v[0] = cross.v[0] * scale;
        cross.v[1] = cross.v[1] * scale;
        cross.v[2] = cross.v[2] * scale;
    } else {
        cross.v[0] = 1.f;
        cross.v[1] = 0.f;
        cross.v[2] = 0.f;
    }

    BrMatrix34TApplyV(&out, &cross, &pObject->actor->t.t.mat);

    if (out.v[1] * pObject->omega.v[1] + out.v[2] * pObject->omega.v[2]
            + out.v[0] * pObject->omega.v[0] > 1.5) {
        return;
    }

    speed = sqrt(len2);
    BrVector3Sub(&size, &pObject->bb1.max, &pObject->bb1.min);
    over = speed - pTumbleThreshold;

    for (i = 0; i < 3; i++) {
        out.v[i] = out.v[i] * (0.5f * size.v[i]) / pObject->I.v[i];
    }

    k = 0.1f * pTumbleFactor * over * pObject->M;
    nx = out.v[0] * k + pObject->omega.v[0];
    ny = out.v[1] * k + pObject->omega.v[1];
    nz = out.v[2] * k + pObject->omega.v[2];

    new_len2 = nx * nx + ny * ny + nz * nz;
    pObject->omega.v[0] = nx;
    pObject->omega.v[1] = ny;
    pObject->omega.v[2] = nz;
    if (new_len2 > 400.f) {
        scale = sqrt(400.f / new_len2);
        pObject->omega.v[0] = nx * scale;
        pObject->omega.v[1] = ny * scale;
        pObject->omega.v[2] = nz * scale;
    }
}

// FUNCTION: CARMA2_HW 0x00418230
void C2_HOOK_FASTCALL MakeLiftGoUp(tNon_car_spec* pNon_car) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNon_car_spec, flags, 0x100);

    if (gNet_mode != eNet_mode_client) {
        pNon_car->flags &= ~0xff;
        pNon_car->flags |= 0x1;
    }
}
// DamageUnit

// SwitchCarModel

// FUNCTION: CARMA2_HW 0x00413f40
void C2_HOOK_FASTCALL SwitchCarModels(tCar_spec* pCar, int pIndex) {
#ifndef CARPOCALYPSE2_MATCHING
    DRActorEnumRecurse(pCar->car_model_actor, SwitchCarModel, &pIndex);
    pCar->field_0xe18 = pIndex;
#else
    DRActorEnumRecurse(pCar->car_model_actor, SwitchCarModel, &pIndex);
    pCar->field_0xe18 = pIndex;
#endif
}

// InitialiseCar2

// InitialiseCar

// InitialiseCarsEtc

// SetInitialPosition

// SetInitialPositions

// InitialiseNonCar

// NewFaceListCallBack

// IsCarInTheSea

// RememberSafePosition

// ControlNetCars

// ControlOurCar

// FUNCTION: CARMA2_HW 0x00415300
void C2_HOOK_FASTCALL CalcEngineForce(tCar_spec* pCar, br_scalar pDt) {
    br_scalar speed;
    br_scalar factor;
    tS32 tmp_joy;

    speed = -pCar->collision_info->velocity_car_space.v[2] * 6.9;
    pCar->acc_force = 0.f;
    if (pCar->revs == 0.f) {
        pCar->gear = 0;
    }

    if ((!pCar->keys.backwards && pCar->gear < 0) ||
        (pCar->gear == 0 && speed < -0.5 && !pCar->keys.backwards)) {
        pCar->keys.backwards = !pCar->keys.backwards;
        pCar->keys.acc ^= pCar->keys.dec;
        pCar->keys.dec ^= pCar->keys.acc;
        pCar->keys.acc ^= pCar->keys.dec;
        tmp_joy = pCar->joystick.acc;
        pCar->joystick.acc = pCar->joystick.dec;
        pCar->joystick.dec = tmp_joy;
    }
    if ((pCar->keys.backwards && pCar->gear > 0) ||
        (pCar->gear == 0 && speed > 0.5 && pCar->keys.backwards)) {
        pCar->keys.backwards = !pCar->keys.backwards;
        pCar->keys.acc ^= pCar->keys.dec;
        pCar->keys.dec ^= pCar->keys.acc;
        pCar->keys.acc ^= pCar->keys.dec;
        tmp_joy = pCar->joystick.acc;
        pCar->joystick.acc = pCar->joystick.dec;
        pCar->joystick.dec = tmp_joy;
    }
    if (pCar->gear == 0 && !pCar->keys.acc && pCar->joystick.acc <= 0 &&
        (pCar->keys.dec || pCar->joystick.dec > 0) && !pCar->keys.backwards &&
        fabsf(speed) < 1.f) {
        pCar->keys.backwards = 1;
        pCar->keys.acc = pCar->keys.dec;
        pCar->keys.dec = 0;
        tmp_joy = pCar->joystick.acc;
        pCar->joystick.acc = pCar->joystick.dec;
        pCar->joystick.dec = tmp_joy;
    }

    pCar->torque = pCar->revs * pCar->revs * -1e-08f - 0.2;
    if (pCar->keys.acc || pCar->joystick.acc >= 0) {
        if (fabsf(pCar->curvature) > pCar->maxcurve * 0.5f && pCar->gear < 2 &&
            pCar->gear != 0 && pCar->traction_control && pCar->joystick.acc < 0) {
            factor = 0.7f;
        } else if (pCar->joystick.acc >= 0) {
            factor = pCar->joystick.acc * (1.0 / 54613.0);
        } else {
            factor = 1.2f;
        }
        factor = factor * pCar->field_0x4d4;
        factor = factor * gPower_starting_value[pCar->power_up_levels[1]];
        if (pCar->damage_units[0].damage_level > 10) {
            factor = factor * (1.f - (pCar->damage_units[0].damage_level - 10) * 0.01f);
        }
        pCar->torque = pCar->torque + factor;
    } else {
        pCar->traction_control = 1;
    }

    if (pCar->keys.dec || (pCar->keys.acc && pCar->gear == 0) ||
        pCar->joystick.dec > 0 || (pCar->joystick.acc > 0 && pCar->gear == 0)) {
        if (pCar->joystick.dec > 0) {
            br_scalar bi = pCar->brake_increase;
            br_scalar q = (pCar->joystick.dec) / 65536;
            pCar->brake_force = pCar->initial_brake + bi * q;
        }
        if (pCar->brake_force == 0.f) {
            pCar->brake_force = pCar->initial_brake;
        } else {
            br_scalar bi = pCar->brake_increase;
            pCar->brake_force = pCar->brake_force + bi * pDt;
            if (pCar->brake_force > pCar->initial_brake + pCar->brake_increase) {
                pCar->brake_force = pCar->initial_brake + pCar->brake_increase;
            }
        }
    } else {
        pCar->brake_force = 0.f;
    }

    if (pCar->gear == 0) {
        return;
    }
    pCar->acc_force = pCar->force_torque_ratio * pCar->torque / (float)pCar->gear;
    if (pCar->brake_force != 0.f) {
        pCar->revs = pCar->target_revs;
    } else if (pCar->revs - 1.f > pCar->target_revs ||
               pCar->revs + 1.f < pCar->target_revs) {
        br_scalar lt = gZero / (pCar->speed_revs_ratio * pCar->collision_info->M) / (float)pCar->gear;
        pCar->acc_force = pCar->acc_force +
            (pCar->torque * pDt * 5000.0 + (pCar->revs - pCar->target_revs)) /
            ((lt +
              (float)pCar->gear * (gZero / (pCar->force_torque_ratio * 0.0002))) * pDt);
    }
}

// PrepareCars

// CalcGraphicalWheelStuff

// FinishCars

// GetNonCars

// GetNetPos

// MungeCarsMass

// AddDrag

// DragChildren

// DoCarStuff

// DoNonCarStuff

// RemoveFlyingCar

// RestoreFlyingCar

// GetDrivableOnList

// APTCPreCollision

// SetCollisionFlagsAndStuff

// APTCPostCollision

// APTCActiveHalted

// APTCPassiveActivated

// APTCChangedObjects

// ApplyPhysicsToCars

// MoveAndCollideCar

// MoveAndCollideNonCar

// ControlCar2

// ControlCar3

// ControlCar4

// ControlCar5

// ControlCar1

// SteeringSelfCentre

// NonCarSnapOff

// TestNonCarSnapOff

// TumbleObjectWithV

// TumbleObject

// MakeLiftGoUp

// NonCarCalcForce

// DoBumpiness

// SmashFacesWithWheels

// ConditionallyNoteSkid

// NudgeObject

// FUNCTION: CARMA2_HW 0x00418850
void C2_HOOK_FASTCALL CalcForce(tCar_spec* pCar, br_scalar pDt) {
    int i;
    int j;
    int normnum;
    int vol_mod;
    br_scalar force[4];
    br_scalar d[4];
    br_scalar dd[4];
    br_scalar rt[4];
    br_scalar wheelratio;
    br_scalar k;
    br_scalar ts;
    br_scalar ts2;
    br_scalar maxfl;
    br_scalar maxfr;
    br_scalar friction_number;
    br_scalar deltaomega;
    br_scalar v98;
    br_scalar v99;
    br_scalar v106;
    br_scalar v108;
    br_scalar v109;
    br_scalar v116;
    br_scalar v125;
    br_scalar v128;
    br_scalar v129;
    br_scalar v134;
    br_scalar v135;
    br_scalar pV;
    br_scalar fl_oil_factor;
    br_scalar fr_oil_factor;
    br_scalar rl_oil_factor;
    br_scalar rr_oil_factor;
    int delta;
    br_vector3 b;
    br_vector3 f;
    br_vector3 B;
    br_vector3 v;
    br_vector3 tv;
    br_vector3 tmp;
    br_vector3 ray_dir;
    br_vector3 normal;
    br_vector3 nor2;
    br_vector3 vplane;
    br_vector3 rightplane;
    br_vector3 a;
    br_vector3 v103;
    br_vector3 v136;
    br_vector3 v123;
    br_vector3 norm[4];
    br_vector3 ray_pos[4];
    br_vector3 wheel_pos[4];
    br_bounds bounds;
    tFace_ref* faces[4];
    tPhysics_object* objs[4];
    tPhysics_object* pObj;
    tPhysics_object** pList;
    tFace_ref* pFace;
    br_matrix34* mat;
    tSpecial_volume* vol;
    tMaterial_modifiers* mat_list;
    br_scalar* pd;
    static br_scalar stop_timer;
    static br_scalar slide_dist;

    pCar->curvature = pCar->curvature + pCar->field_0x1260;
    B.v[0] = 0.f;
    B.v[1] = 0.f;
    B.v[2] = 0.f;
    pCar->field_0x195c = 0;
    vol = pCar->collision_info->last_special_volume;
    wheelratio = (pCar->wpos[2].v[2] - pCar->centre_of_mass_world_scale.v[2])
        / (pCar->wpos[0].v[2] - pCar->centre_of_mass_world_scale.v[2]);
    faces[0] = NULL;
    faces[1] = NULL;
    faces[2] = NULL;
    faces[3] = NULL;
    normnum = 0;
    objs[0] = NULL;
    objs[1] = NULL;
    objs[2] = NULL;
    objs[3] = NULL;
    vol_mod = 0;
    f.v[0] = 0.f;
    f.v[1] = 0.f;
    f.v[2] = 0.f;
    mat = &pCar->car_master_actor->t.t.mat;
    b.v[0] = -mat->m[1][0];
    b.v[1] = -mat->m[1][1];
    b.v[2] = -mat->m[1][2];
    v.v[0] = pCar->collision_info->v.v[0] * 6.9;
    v.v[1] = pCar->collision_info->v.v[1] * 6.9;
    v.v[2] = pCar->collision_info->v.v[2] * 6.9;
    pCar->road_normal.v[0] = 0.f;
    pCar->road_normal.v[1] = 0.f;
    pCar->road_normal.v[2] = 0.f;
    for (i = 0; i < 4; i++) {
        BrMatrix34ApplyP(&wheel_pos[i], &pCar->wpos[i], mat);
    }
    k = (pCar->susp_height[0] > pCar->susp_height[1] ? pCar->susp_height[0] : pCar->susp_height[1]) - (-0.5);
    d[0] = 2.f;
    d[1] = 2.f;
    BrVector3Scale(&tmp, &b, k);
    d[2] = 2.f;
    d[3] = 2.f;
    for (i = 0; i < 4; i++) {
        BrVector3InvScale(&ray_pos[i], &wheel_pos[i], WORLD_SCALE);
    }
    BrVector3InvScale(&ray_dir, &tmp, WORLD_SCALE);
    for (i = pCar->collision_info->box_face_start, pFace = &gFace_list__car[i];
            i < pCar->collision_info->box_face_end;
            i++, pFace++) {
        if (gUNK_006793d0 != 0 && (pFace->flags & 0x80) != 0) {
            continue;
        }
        MultiRayCheckSingleFace(4, pFace, ray_pos, &ray_dir, &normal, rt);
        for (j = 0; j < 4; j++) {
            if (rt[j] < d[j]) {
                faces[j] = pFace;
                d[j] = rt[j];
                BrVector3Copy(&pCar->nor[j], &normal);
            }
        }
    }
    bounds.max = ray_pos[0];
    bounds.min = ray_pos[0];
    for (i = 1; i < 4; i++) {
        for (j = 0; j < 3; j++) {
            if (ray_pos[i].v[j] < bounds.min.v[j]) {
                bounds.min.v[j] = ray_pos[i].v[j];
            }
            if (bounds.max.v[j] < ray_pos[i].v[j]) {
                bounds.max.v[j] = ray_pos[i].v[j];
            }
        }
    }
    for (j = 0; j < 3; j++) {
        if (ray_dir.v[j] < 0.f) {
            bounds.min.v[j] = bounds.min.v[j] + ray_dir.v[j];
        } else {
            bounds.max.v[j] = bounds.max.v[j] + ray_dir.v[j];
        }
    }
    for (i = 0, pList = gDrivable_on_list; i < (int)gDrivable_on_count; i++, pList++) {
        pObj = *pList;
        if (!BoundsOverlapTest__finteray(&bounds, &pObj->field_0xf4)) {
            continue;
        }
        BrMatrix34TApplyV(&ray_dir, &ray_dir, &pObj->transform_matrix);
        if (pObj->shape == NULL) {
            continue;
        }
        for (;;) {
            BrMatrix34ApplyP(&pObj->cmpos, &pObj->cmpos, &pObj->transform_matrix);
            for (j = 0; j < 4; j++) {
                DrMatrix34ApplyLPInverse(&tmp, &ray_pos[j], &pObj->transform_matrix);
                ShapeRayCast(&tmp, &ray_dir, (const tPhysics_shape*)pObj->shape, &pObj->cmpos, &ts, &normal);
                if (ts < d[j]) {
                    normal.v[0] = -normal.v[0];
                    normal.v[1] = -normal.v[1];
                    normal.v[2] = -normal.v[2];
                    BrMatrix34ApplyV(&normal, &normal, &pObj->transform_matrix);
                    d[j] = ts;
                    faces[j] = NULL;
                    objs[j] = pObj;
                }
            }
            if (pObj->next == NULL) {
                break;
            }
            pObj = (tPhysics_object*)pObj->next;
        }
    }
    if (pCar->collision_info->last_special_volume != NULL
        && pCar->collision_info->last_special_volume->material_modifier_index != 0) {
        vol_mod = pCar->collision_info->last_special_volume->material_modifier_index;
    }
    mat_list = gFriction_materials;
    for (i = 0, j = 0; j < 0x30; i++, j += 0xc) {
        pd = &d[i];
        *pd = *pd * k;
        ts = *pd;
        if (faces[i] != NULL
                && faces[i]->material != NULL
                && faces[i]->material->identifier != NULL) {
            pCar->material_index[i] = vol_mod != 0
                ? vol_mod
                : (signed char)faces[i]->material->identifier[0] - '/';
            if (pCar->material_index[i] < 0 || pCar->material_index[i] > 0xb) {
                pCar->material_index[i] = 0;
            }
            if (strlen(faces[i]->material->identifier) == 0xb
                    && faces[i]->material->identifier[5] == '|') {
                pCar->oil_remaining[i] = 0.f;
            }
        } else {
            pCar->material_index[i] = 0;
        }
        BrMatrix34TApplyV(&norm[i], &pCar->nor[i], mat);
        if (mat_list[pCar->material_index[i]].bumpiness != 0.f) {
            BrVector3Scale(&tv, &pCar->nor[i], *pd);
            BrVector3Accumulate(&tv, &wheel_pos[i]);
            {
                int x = (int)(512.f * tv.v[0]);
                int y = (int)(512.f * tv.v[2]);

                if (x < 0) {
                    x = -x;
                }
                if (y < 0) {
                    y = -y;
                }
                x = x % 2048;
                y = y % 2048;
                if (x > 1024) {
                    x = 2048 - x;
                }
                if (y > 1024) {
                    y = 2048 - y;
                }
                if (x + y > 1024) {
                    delta = 2048 - x - y;
                } else {
                    delta = x + y;
                }
                delta = delta - 400;
                if (delta < 0) {
                    delta = 0;
                }
            }
            d[i] = delta * mat_list[pCar->material_index[i]].bumpiness / 42400.f
                * norm[i].v[1] + *pd;
        }
        if (*pd < -0.5f
                || pCar->wheel_dam_offset[i ^ 2] * 6.9 + pCar->susp_height[i / 2] < *pd) {
            force[i] = 0.f;
            d[i] = pCar->susp_height[i / 2];
        } else {
            BrVector3Accumulate(&pCar->road_normal, &norm[i]);
            normnum++;
            d[i] = d[i] - pCar->wheel_dam_offset[i ^ 2] * 6.9;
            force[i] = (pCar->susp_height[i / 2] - d[i]) * pCar->sk[i / 2];
            force[i] = force[i] - (d[i] - pCar->oldd[i]) / pDt * pCar->sb[i / 2];
            if (pCar->susp_height[i / 2] == pCar->oldd[i]
                && pCar->nor[i].v[0] * v.v[0] + pCar->nor[i].v[1] * v.v[1] + pCar->nor[i].v[2] * v.v[2] > -0.01f
                && pCar->collision_info->M * 5.f < force[i]) {
                d[i] = pCar->susp_height[i / 2];
                force[i] = pCar->collision_info->M * 5.f;
            }
            if (force[i] < 0.f) {
                force[i] = 0.f;
            }
            B.v[1] = force[i] + B.v[1];
            f.v[0] = f.v[0] - (pCar->wpos[i].v[2] - pCar->centre_of_mass_world_scale.v[2]) * force[i];
            f.v[2] = (pCar->wpos[i].v[0] - pCar->centre_of_mass_world_scale.v[0]) * force[i] + f.v[2];
        }
        dd[i] = pCar->oldd[i] - d[i];
        pCar->oldd[i] = d[i];
        if (objs[i] == NULL || force[i] == 0.f) {
            continue;
        }
        pCar->field_0x195c = (unsigned int)objs[i];
        if (objs[i]->disable_move_rotate != 0
                && objs[i]->owner != NULL
                && objs[i]->flags_0x238 == 1
                && ((tNon_car_spec*)objs[i]->owner)->driver <= 5
                && (((tNon_car_spec*)objs[i]->owner)->flags & 0x10000) != 0) {
            continue;
        }
        objs[i]->disable_move_rotate = 0;
        ts = pCar->collision_info->M * pDt / (force[i] / 6.9);
        tmp.v[0] = ts * b.v[0];
        tmp.v[1] = ts * b.v[1];
        tmp.v[2] = ts * b.v[2];
        objs[i]->v.v[0] = tmp.v[0] + objs[i]->v.v[0];
        objs[i]->v.v[1] = tmp.v[1] + objs[i]->v.v[1];
        objs[i]->v.v[2] = tmp.v[2] + objs[i]->v.v[2];
        if (gUNK_006793d0 == 2) {
            pCar->collision_info->field_0x49c = (int)&gZero;
        }
    }
    if (objs[0] != NULL
            && objs[0] == objs[1]
            && objs[0] == objs[2]
            && objs[0] == objs[3]
            && objs[0]->owner != NULL
            && objs[0]->flags_0x238 == 1
            && ((tNon_car_spec*)objs[0]->owner)->driver <= 5
            && (((tNon_car_spec*)objs[0]->owner)->flags & 0x10000) != 0
            && (((tNon_car_spec*)objs[0]->owner)->flags & 0x40000) != 0
            && (((tNon_car_spec*)objs[0]->owner)->flags & 0xff) == 0
            && (((tNon_car_spec*)objs[0]->owner)->flags & 0xff00) == 0
            && ((tNon_car_spec*)objs[0]->owner)->field_0xf8 == 0
            && gUNK_006793d0 != 3) {
        ((tNon_car_spec*)objs[0]->owner)->flags = (((tNon_car_spec*)objs[0]->owner)->flags & 1) | 1;
    }
    if (pCar->collision_info->disable_move_rotate != 0 && normnum == 0) {
        goto wall_climber;
    }
    if (pCar->collision_info->disable_move_rotate != 0) {
        /* TODO: sub_4b9e40(collision_info, 0) */
    }
wall_climber:
    if (pCar == NULL || pCar->driver <= 5 || pCar->wall_climber_mode == 0
            || (pCar->road_normal.v[0] == 0.f && pCar->road_normal.v[1] == 0.f && pCar->road_normal.v[2] == 0.f)) {
        friction_number = pCar->collision_info->M * 10.f;
        if (vol != NULL) {
            friction_number = (1.f - vol->gravity_multiplier) * pCar->collision_info->water_depth_factor;
            if (pCar->underwater_ability != 0) {
                friction_number = friction_number * 0.6f;
            }
            friction_number = (1.f - friction_number) * pCar->collision_info->M;
        } else {
            friction_number = pCar->collision_info->M;
        }
        friction_number = friction_number * gGravity_multiplier * 10.f;
        B.v[0] = B.v[0] - mat->m[0][1] * friction_number;
        B.v[1] = B.v[1] - mat->m[1][1] * friction_number;
        B.v[2] = B.v[2] - mat->m[2][1] * friction_number;
    } else {
        BrVector3Normalise(&tmp, &pCar->road_normal);
        BrVector3Scale(&tmp, &tmp, -(pCar->collision_info->M * 10.0f));
        BrVector3Accumulate(&B, &tmp);
    }
    if (normnum != 0) {
        BrVector3NormaliseQuick(&pCar->road_normal, &pCar->road_normal);
        friction_number = pCar->road_normal.v[1] * mat->m[1][1]
            + pCar->road_normal.v[2] * mat->m[2][1]
            + pCar->road_normal.v[0] * mat->m[0][1];
        if (pCar->driver > 5 && pCar->wall_climber_mode != 0) {
            friction_number = 1.f;
        }
        friction_number = mat_list[pCar->material_index[0]].down_force * friction_number;
        if (friction_number > 0.f) {
            friction_number = fabs(pCar->collision_info->velocity_car_space.v[2])
                * pCar->collision_info->M * 10.0 * friction_number / pCar->downforce_to_weight;
            if (pCar->collision_info->M * 10.0 < friction_number) {
                friction_number = pCar->collision_info->M * 10.0;
            }
            B.v[1] = B.v[1] - friction_number;
        }
        vplane.v[0] = BrVector3Dot(&pCar->collision_info->velocity_car_space, &pCar->road_normal) * pCar->road_normal.v[0];
        vplane.v[1] = BrVector3Dot(&pCar->collision_info->velocity_car_space, &pCar->road_normal) * pCar->road_normal.v[1];
        vplane.v[2] = BrVector3Dot(&pCar->collision_info->velocity_car_space, &pCar->road_normal) * pCar->road_normal.v[2];
        BrVector3Sub(&vplane, &pCar->collision_info->velocity_car_space, &vplane);
        if (vplane.v[2] < 0.f) {
            ts = 1.f;
        } else {
            ts = -1.f;
        }
        ts2 = BrVector3Length(&vplane);
        deltaomega = ts2 * pCar->curvature * ts;
        deltaomega = deltaomega - BrVector3Dot(&pCar->collision_info->omega, &pCar->road_normal);
        BrVector3Set(&v103, pCar->road_normal.v[1], -pCar->road_normal.v[0], 0.f);
        BrVector3Normalise(&v103, &v103);
        friction_number = pCar->collision_info->I.v[1] / pDt * deltaomega;
        ts = friction_number / (pCar->wpos[2].v[2] - pCar->wpos[0].v[2]);
        v108 = ts;
        v109 = -ts;
        BrVector3Set(&rightplane, 0.f, pCar->road_normal.v[2], -pCar->road_normal.v[1]);
        BrVector3Normalise(&rightplane, &rightplane);
        v99 = pCar->acc_force;
        friction_number = BrVector3Dot(&rightplane, &vplane);
        v108 = BrVector3Dot(&v103, &vplane);
        ts2 = fabs(v108);
        friction_number = (pCar->wpos[0].v[2] - pCar->centre_of_mass_world_scale.v[2]) * friction_number * fabs(pCar->curvature);
        if (pCar->curvature <= 0.f) {
            friction_number = v108 - friction_number;
        } else {
            friction_number = v108 + friction_number;
        }
        friction_number = -(pCar->collision_info->M / pDt * friction_number);
        friction_number = friction_number - BrVector3Dot(&B, &v103);
        friction_number = friction_number / (1.0 - wheelratio);
        v108 = friction_number + v108;
        v109 = -wheelratio * friction_number + v109;
        friction_number = (pCar->wpos[0].v[2] - pCar->wpos[2].v[2]) * v108;
        v98 = friction_number * pCar->curvature;
        friction_number = BrVector3Dot(&pCar->collision_info->velocity_car_space, &rightplane) * pCar->collision_info->M / pDt;
        v129 = BrVector3Dot(&rightplane, &B) + friction_number;
        v128 = pCar->mu.v[0] * pCar->brake_force / (pCar->mu.v[1] / pCar->traction_multiplier + pCar->mu.v[0]);
        v125 = pCar->brake_force - v128;
        ts = (pCar->damage_units[7].damage_level + pCar->damage_units[6].damage_level) / 2;
        if (ts > 20.f) {
            v128 = (1.f - (ts - 20.f) / 80.f) * (1.f - (ts - 20.f) / 80.f) * v128;
        }
        ts = (pCar->damage_units[5].damage_level + pCar->damage_units[4].damage_level) / 2;
        if (ts > 20.f) {
            v125 = (1.f - (ts - 20.f) / 80.f) * (1.f - (ts - 20.f) / 80.f) * v125;
        }
        ts2 = (force[1] + force[0]) * pCar->rolling_resistance + v128;
        v106 = (force[2] + force[3]) * pCar->steerable_rolling_resistance + v125;
        v128 = pCar->wpos[0].v[2] - pCar->wpos[2].v[2];
        v128 = sqrt(v128 * v128 * pCar->curvature * pCar->curvature + 1.0);
        v106 = v106 / v128;
        v134 = v106 + ts2;
        if (fabs(v129) < fabs(v134)) {
            ts2 = v129 / v134 * ts2;
            v106 = v129 / v134 * v106;
        }
        if ((v106 + ts2) * v129 < 0.f) {
            ts2 = -ts2;
            v106 = -v106;
        }
        v129 = v129 - (ts2 + v106);
        v99 = v99 - ts2;
        if (pCar->keys.brake
                && pCar->damage_units[7].damage_level < 60
                && pCar->damage_units[6].damage_level < 60) {
            v99 = v99 - v129;
            pCar->gear = 0;
        }
        v99 = v99 / pCar->traction_multiplier;
        v135 = sqrt(v99 * v99 + v109 * v109) / 2.0;
        /* TODO: GetOilFrictionFactors */
        fl_oil_factor = 1.f;
        fr_oil_factor = 1.f;
        rl_oil_factor = 1.f;
        rr_oil_factor = 1.f;
        if (pCar->driver <= 5) {
            v116 = 1.f;
        } else {
            v116 = pCar->grip_multiplier;
        }
        BrVector3Sub(&a, &pCar->wpos[0], &pCar->centre_of_mass_world_scale);
        BrVector3Cross(&a, &pCar->collision_info->omega, &a);
        BrVector3Accumulate(&a, &pCar->collision_info->velocity_car_space);
        if (pCar->driver >= 6
            && (((pCar->keys.left || pCar->joystick.left > 0x8000) && pCar->curvature > 0.f && deltaomega > 0.1 && a.v[0] > 0.f)
                || ((pCar->keys.right || pCar->joystick.right > 0x8000) && pCar->curvature < 0.f && deltaomega < 0.1 && a.v[0] < 0.f))
            && ts > 0.f) {
            friction_number = pCar->mu.v[0];
        } else {
            friction_number = pCar->mu.v[2];
            ts2 = BR_ABS(a.v[0]) / 10.f;
            if (ts2 > 1.f) {
                ts2 = 1.f;
            }
            friction_number = (pCar->mu.v[2] - pCar->mu.v[0]) * ts2 + friction_number;
        }
        maxfl = sqrt(force[0]) * friction_number * (rl_oil_factor * v116) * mat_list[pCar->material_index[0]].tyre_road_friction;
        maxfr = sqrt(force[1]) * friction_number * (rr_oil_factor * v116) * mat_list[pCar->material_index[1]].tyre_road_friction;
        pCar->max_force_rear = maxfr + maxfl;
        if (fabs(v109) > maxfr + maxfl && maxfr + maxfl > 0.1f) {
            v106 = (maxfr + maxfl) / fabs(v109) * pDt;
            v109 = v106 * v109;
            v99 = pCar->traction_multiplier * v106 * v99;
        }
        v98 = v98 - v106;
        v108 = (pCar->wpos[0].v[2] - pCar->wpos[2].v[2]) * pCar->curvature * v106 + v108;
        if (v135 > 0.0001f) {
            v109 = v109 / (v135 * 2.f);
            v99 = v99 / (v135 * 2.0);
        }
        v99 = pCar->traction_multiplier * v99;
        force[0] = v135;
        force[1] = v135;
        pCar->wheel_slip = 0;
        switch ((force[0] > maxfl) + 2 * (force[1] > maxfr)) {
        case 0:
            slide_dist = 0.f;
            break;
        case 1:
            force[0] = pCar->friction_slipping_reduction * maxfl;
            force[1] = v135 - force[0] + force[1];
            if (force[1] <= maxfr) {
                slide_dist = 0.f;
            } else {
                if (maxfr > 0.1f) {
                    pV = (force[1] - maxfr) / maxfr;
                    if (&gProgram_state.current_car == pCar) {
                        ts2 = 20.f;
                    } else {
                        ts2 = 60.f;
                    }
                    if (ts2 <= pV) {
                        pCar->new_skidding |= 2;
                    }
                    /* TODO: SkidNoise(pCar, 1, pV, pCar->material_index[1]); */
                }
                force[1] = pCar->friction_slipping_reduction * maxfr;
                pCar->wheel_slip |= 2;
            }
            break;
        case 2:
            force[1] = pCar->friction_slipping_reduction * maxfr;
            force[0] = v135 - force[1] + force[0];
            if (force[0] <= maxfl) {
                slide_dist = 0.f;
            } else {
                if (maxfl > 0.1f) {
                    pV = (force[0] - maxfl) / maxfl;
                    if (&gProgram_state.current_car == pCar) {
                        ts2 = 20.f;
                    } else {
                        ts2 = 60.f;
                    }
                    if (ts2 <= pV) {
                        pCar->new_skidding |= 1;
                    }
                    /* TODO: SkidNoise(pCar, 0, pV, pCar->material_index[0]); */
                }
                force[0] = pCar->friction_slipping_reduction * maxfl;
                pCar->wheel_slip |= 2;
            }
            break;
        case 3:
            force[0] = pCar->friction_slipping_reduction * maxfl;
            force[1] = pCar->friction_slipping_reduction * maxfr;
            pCar->wheel_slip |= 2;
            pV = (v135 * 2.0 - maxfl - maxfr) / (maxfr + maxfl);
            if (&gProgram_state.current_car == pCar) {
                ts2 = 20.f;
            } else {
                ts2 = 60.f;
            }
            if (ts2 <= pV) {
                if (maxfl > 0.1f) {
                    pCar->new_skidding |= 1;
                }
                if (maxfr > 0.1f) {
                    pCar->new_skidding |= 2;
                }
            }
            /* TODO: SkidNoise(pCar, IRandomBetween(0, 1), pV, ...) */
            break;
        }
        v135 = sqrt(v108 * v108 + v98 * v98) / 2.0;
        if (v135 > 0.0001f) {
            v108 = v108 / (v135 * 2.0);
            v98 = v98 / (v135 * 2.0);
        }
        maxfl = sqrt(force[2]) * pCar->mu.v[1] * (fl_oil_factor * v116) * mat_list[pCar->material_index[2]].tyre_road_friction;
        maxfr = sqrt(force[3]) * pCar->mu.v[1] * (fr_oil_factor * v116) * mat_list[pCar->material_index[3]].tyre_road_friction;
        pCar->max_force_front = maxfr + maxfl;
        force[2] = v135;
        force[3] = v135;
        v106 = (v135 > maxfl) + 2 * (v135 > maxfr);
        switch ((int)v106) {
        case 1:
            force[2] = pCar->friction_slipping_reduction * maxfl;
            force[3] = v135 - force[2] + force[3];
            if (force[3] > maxfr) {
                if (maxfr > 0.1f) {
                    pV = (force[3] - maxfr) / maxfr;
                    if (&gProgram_state.current_car == pCar) {
                        ts2 = 20.f;
                    } else {
                        ts2 = 60.f;
                    }
                    if (ts2 <= pV) {
                        pCar->new_skidding |= 8;
                    }
                    /* TODO: SkidNoise(pCar, 3, pV, pCar->material_index[3]); */
                }
                force[3] = pCar->friction_slipping_reduction * maxfr;
                pCar->wheel_slip |= 1;
            }
            break;
        case 2:
            force[3] = pCar->friction_slipping_reduction * maxfr;
            force[2] = v135 - force[3] + force[2];
            if (force[2] > maxfl) {
                if (maxfl > 0.1f) {
                    pV = (force[2] - maxfl) / maxfl;
                    if (&gProgram_state.current_car == pCar) {
                        ts2 = 20.f;
                    } else {
                        ts2 = 60.f;
                    }
                    if (ts2 <= pV) {
                        pCar->new_skidding |= 4;
                    }
                    /* TODO: SkidNoise(pCar, 2, pV, pCar->material_index[2]); */
                }
                force[2] = pCar->friction_slipping_reduction * maxfl;
                pCar->wheel_slip |= 1;
            }
            break;
        case 3:
            force[2] = pCar->friction_slipping_reduction * maxfl;
            force[3] = pCar->friction_slipping_reduction * maxfr;
            pCar->wheel_slip |= 1;
            pV = (v135 * 2.0 - maxfl - maxfr) / (maxfr + maxfl);
            if (&gProgram_state.current_car == pCar) {
                ts2 = 20.f;
            } else {
                ts2 = 60.f;
            }
            if (ts2 <= pV) {
                if (maxfl > 0.1f) {
                    pCar->new_skidding |= 4;
                }
                if (maxfr > 0.1f) {
                    pCar->new_skidding |= 8;
                }
            }
            /* TODO: SkidNoise */
            break;
        }
        BrVector3Scale(&v136, &rightplane, v99);
        BrVector3Scale(&a, &v103, v109);
        BrVector3Accumulate(&v136, &a);
        BrVector3Scale(&v123, &rightplane, v98);
        BrVector3Scale(&a, &v103, v108);
        BrVector3Accumulate(&v123, &a);
        for (i = 0; i < 4; i++) {
            rightplane = pCar->wpos[i];
            rightplane.v[1] = rightplane.v[1] - pCar->oldd[i];
            BrVector3Sub(&rightplane, &rightplane, &pCar->centre_of_mass_world_scale);
            if (i < 2) {
                BrVector3Scale(&b, &v136, force[i]);
            } else {
                BrVector3Scale(&b, &v123, force[i]);
            }
            BrVector3Accumulate(&B, &b);
            BrVector3Cross(&a, &rightplane, &b);
            BrVector3Accumulate(&f, &a);
        }
    } else {
        pCar->max_force_front = 0.f;
        pCar->max_force_rear = 0.f;
        StopSkid(pCar);
    }
    pCar->number_of_wheels_on_ground = normnum;
    BrMatrix34ApplyV(&b, &B, mat);
    BrVector3Scale(&rightplane, &f, pDt);
    BrVector3Scale(&rightplane, &b, pDt / pCar->collision_info->M);
    BrVector3Accumulate(&pCar->collision_info->v, &rightplane);
    if (pCar->speed < 0.0001f
        && ((!pCar->keys.acc && pCar->joystick.acc <= 0) || !pCar->gear)
        && !pCar->keys.dec
        && pCar->joystick.dec <= 0
        && pCar->bounce_rate == 0.f
        && BrVector3Length(&pCar->collision_info->omega) < 0.05f) {
        if (vol != NULL) {
            ts2 = pCar->driver > 5 && pCar->underwater_ability != 0
                ? 1.0 - (1.0 - vol->gravity_multiplier) * 0.6
                : vol->gravity_multiplier;
            friction_number = BrVector3Length(&b) / ts2 / gGravity_multiplier;
        } else {
            friction_number = BrVector3Length(&b);
        }
        if (pCar->collision_info->M > friction_number || (pCar->keys.brake && normnum >= 3)) {
            if (stop_timer == 100.f) {
                stop_timer = 0.f;
            }
            if (stop_timer > 0.5f) {
                BrVector3SetFloat(&pCar->collision_info->v, 0.f, 0.f, 0.f);
                BrVector3SetFloat(&pCar->collision_info->omega, 0.f, 0.f, 0.f);
                stop_timer = 0.5f;
            }
        }
    }
    stop_timer = pDt + stop_timer;
    if (stop_timer > 1.f) {
        stop_timer = 100.f;
    }
    AddDrag(pCar, (tPhysics_object*)pCar->collision_info, pDt);
    if (pCar->driver >= 6) {
        pCar->acc_force = -(v136.v[2] * force[0]) - v136.v[2] * force[1];
    }
}

// DoRevs

// ScrapeNoise

// SkidNoise

// StopSkid

// CrashNoise

// CrushAndDamageCar

// PointInFaceByQuiteABitActually

// DoEnvironmentSmashes

// ProcessForcesCallBack

// ProcessJointForcesCallBack

// MultiFindFloorInBoxM

// MultiRayCastOnObjects

// MultiFindFloorInBoxBU

// FindFace

// findfloor

// FindFloorInBoxBU

// CancelPendingCunningStunt

// SetAmbientPratCam

// SetTextureBits

// MungeSomeOtherCarGraphics

// MungeCarGraphics

// TurnOffNonGroovers

// DoLODCarModels

// DoComplexCarModels

// ResetCarScreens

// FlyCar

// GetCarOverallBoundsMinY

// SetCarSuspGiveAndHeight

// TestForCarInSensiblePlace

// PullActorFromWorld

// DoPullActorFromWorld

// PipeNonCarObject

// PipeNonCars

// CheckForDeAttachmentOfNonCars

// AdjustNonCar

// GetPrecalculatedFacesUnderCar

// TurnOnNonCar

// TurnOffNonCar