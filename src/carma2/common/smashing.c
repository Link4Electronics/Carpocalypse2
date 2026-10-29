#include "smashing.h"

#include "brucetrk.h"
#include "car.h"
#include "crush.h"
#include "explosions.h"
#include "globvars.h"
#include "globvrpb.h"
#include "graphics.h"
#include "loading.h"
#include "oil.h"
#include "pedestrn.h"
#include "physics.h"
#include "piping.h"
#include "platform.h"
#include "powerups.h"
#include "replay.h"
#include "sound.h"
#include "utility.h"
#include "world.h"

#include <brender/brender.h>

#include "c2_string.h"

#include "carpocalypse2_macros.h"
#include <math.h>

extern const br_scalar gFloat_0058aec0;

// GLOBAL: CARMA2_HW 0x0058af5c
const br_scalar gFLOAT_0058af5c = 0.001f;

// GLOBAL: CARMA2_HW 0x0058af60
const double gDOUBLE_0058af60 = 4.9;

// GLOBAL: CARMA2_HW 0x0058af68
const double gDOUBLE_0058af68 = -0.14492753623188406;

// GLOBAL: CARMA2_HW 0x0058af70
const double gDOUBLE_0058af70 = 9.8;

// GLOBAL: CARMA2_HW 0x0058af78
const double gDOUBLE_0058af78 = 0.0001;

// GLOBAL: CARMA2_HW 0x0058af80
const double gDOUBLE_0058af80 = 10430.378350470453;

// GLOBAL: CARMA2_HW 0x006b78e0
tSmash_vertex gSmash_glass_fragments[200];

// GLOBAL: CARMA2_HW 0x006abee8
br_vector3 gZero_vector__smash;


// GLOBAL: CARMA2_HW 0x0065fed0
const char* gInitial_smashable_position_type_names[3] = {
    "sphereclumped",
    "boxclumped",
    "actorbased",
};


// GLOBAL: CARMA2_HW 0x0065fee0
const char* gInitial_position_sphere_where[2] = {
    "impact",
    "model",
};

// GLOBAL: CARMA2_HW 0x0068c898
tSmashable_race_target gSmashable_race_targets[300];

// GLOBAL: CARMA2_HW 0x0074abe0
int gCount_smashable_race_targets;

// GLOBAL: CARMA2_HW 0x006a828c
int gCount_queued_smashes;

// GLOBAL: CARMA2_HW 0x006a82a4
tU32 gLast_munge_smash_edge_triggers;

// GLOBAL: CARMA2_HW 0x006a4430
tPowerup_queue_item gPowerup_queue[50];

// GLOBAL: CARMA2_HW 0x006a4698
tQueued_smash gQueued_smashes[20];

// GLOBAL: CARMA2_HW 0x006b78a0
br_matrix34 gMatrix34_006b78a0;

// GLOBAL: CARMA2_HW 0x006a4688
br_vector3 gVector3_006a4688;

// GLOBAL: CARMA2_HW 0x006a3334
int gINT_006a3334;

// InitGlassFragments

// InitDecals

// FUNCTION: CARMA2_HW 0x004ee550
void C2_HOOK_FASTCALL ReadSmashableInitialPosition(FILE* pFile, tSmashable_initial_position_spec*  pInitial_pos) {

    /* Initial position type */
    pInitial_pos->type = GetALineAndInterpretCommand(pFile, gInitial_smashable_position_type_names, CARPOCALYPSE2_ASIZE(gInitial_smashable_position_type_names));

    switch (pInitial_pos->type) {
        case kInitialSmashablePosition_SphereClumped:
            /* Sphere radius */
            pInitial_pos->position.sphere.radius = GetAScalar(pFile);
            /* Sphere centre */
            pInitial_pos->position.sphere.where = GetALineAndInterpretCommand(pFile, gInitial_position_sphere_where, CARPOCALYPSE2_ASIZE(gInitial_position_sphere_where));
            break;
        case kInitialSmashablePosition_BoxClumped:
            GetThreeFloats(pFile, &pInitial_pos->position.box.v[0], &pInitial_pos->position.box.v[1], &pInitial_pos->position.box.v[2]);
            break;
        default:
            break;
    }
}

// FUNCTION: CARMA2_HW 0x004ee500
void C2_HOOK_FASTCALL ReadSmashableInitialSpeed(FILE* pFile, tSmashable_initial_speed_spec* pInitial_speed) {

    C2_HOOK_BUG_ON(sizeof(tSmashable_initial_speed_spec) != 28);

    /* Min,max speed towards car */
    GetPairOfFloats(pFile, &pInitial_speed->min_speed, &pInitial_speed->max_speed);
    /* Impactee velocity factor */
    pInitial_speed->impactee_velocity_factor = GetAScalar(pFile);
    /* Random velocity (max) */
    pInitial_speed->random_velocity_max = GetAScalar(pFile);
    /* Random up velocity (max) */
    pInitial_speed->random_up_velocity_max = GetAScalar(pFile);
    /* Random normal velocity (max) */
    pInitial_speed->random_normal_velocity_max = GetAScalar(pFile);
    /* Random spin rate (max) */
    pInitial_speed->random_spin_rate_max = GetAScalar(pFile) * 0.1f;
}

// FUNCTION: CARMA2_HW 0x004ee5a0
void C2_HOOK_FASTCALL ReadMinMaxTimeInMilliseconds(FILE* pFile, int* pTimes) {
    float f1;
    float f2;

    /* Min time, Max time */
    GetPairOfFloats(pFile, &f1, &f2);
    pTimes[0] = (int)(1000.0 * f1);
    pTimes[1] = (int)(1000.0 * f2);
}

// FUNCTION: CARMA2_HW 0x004977e0
void C2_HOOK_FASTCALL InitSmashTargets(void) {

    gCount_smashable_race_targets = 0;
}

// FUNCTION: CARMA2_HW 0x004977f0
void C2_HOOK_FASTCALL AddSmashableRaceTarget(br_model* pModel, br_actor* pActor, int pUnknown) {

    C2_HOOK_BUG_ON(sizeof(tSmashable_race_target) != 12);
    if (gCount_smashable_race_targets < (int)CARPOCALYPSE2_ASIZE(gSmashable_race_targets)) {
        gSmashable_race_targets[gCount_smashable_race_targets].model = pModel;
        gSmashable_race_targets[gCount_smashable_race_targets].actor = pActor;
        gSmashable_race_targets[gCount_smashable_race_targets].field_0x8 = pUnknown;
        gCount_smashable_race_targets++;
    }
}

void C2_HOOK_FASTCALL SplondificatalizeIdentifier(br_material* pMaterial, char** ppIdentifier, int pIndex) {
    int len;
    char original_suffix[64];
    char new_identifier[64];
    char* suffix_ptr;

    strcpy(new_identifier, *ppIdentifier);
    len = strlen(new_identifier);
    if (len < 4) {
        sprintf(new_identifier, "Smash material %s has a name that is less than 4 characters long", *ppIdentifier);
        PDFatalError(new_identifier);
    }
    suffix_ptr = new_identifier + len - 4;
    strcpy(original_suffix, suffix_ptr);
    sprintf(suffix_ptr, "        ");
    sprintf(&new_identifier[5], "%cx", '|');
    new_identifier[6] = (char)(pIndex + 1);
    strcat(new_identifier, original_suffix);
    if (strlen(*ppIdentifier) < 12) {
        BrResFree(*ppIdentifier);
        *ppIdentifier = BrResStrDup(pMaterial, new_identifier);
    } else {
        strcpy(*ppIdentifier, new_identifier);
    }
}

// FUNCTION: CARMA2_HW 0x004f0bc0
void C2_HOOK_FASTCALL MungeSmashMaterialNames(void) {
    int i;

    C2_HOOK_BUG_ON(sizeof(tSmashable_item_spec) != 0x2e0);

    for (i = 0; i < gCount_track_smashable_environment_specs; i++) {
        tSmashable_item_spec* item_spec = &gTrack_smashable_environment_specs[i];

        if (item_spec->trigger_type == kSmashableTrigger_Material) {
            SplondificatalizeIdentifier(
                item_spec->trigger_object.material,
                &item_spec->trigger_object.material->identifier,
                i);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004eff30
void C2_HOOK_FASTCALL FUN_004eff30(tSmash_vertex* pFragment, tU32 pTime, float pDelta_time) {
    float dt;
    float gravity_dt;
    float omega;
    br_actor* actor;
    br_vector3 axis;
    br_vector3 pos;

    dt = pDelta_time * gFLOAT_0058af5c;
    if (dt != gFloat_0058aec0 && !pFragment->field_0x34) {
        gravity_dt = gGravity_multiplier * dt;
        pFragment->actor->t.t.translate.t.v[0] -= pFragment->v.v[0] * dt * (float)gDOUBLE_0058af68;
        pFragment->actor->t.t.translate.t.v[1] -= (pFragment->v.v[1] * dt - gravity_dt * dt * (float)gDOUBLE_0058af60) * (float)gDOUBLE_0058af68;
        pFragment->actor->t.t.translate.t.v[2] -= pFragment->v.v[2] * dt * (float)gDOUBLE_0058af68;
        pFragment->v.v[1] -= dt * gGravity_multiplier * (float)gDOUBLE_0058af70;
        omega = BrVector3Length(&pFragment->omega);
        BrVector3InvScale(&axis, &pFragment->omega, omega);
        if (gDOUBLE_0058af78 <= fabs(dt * omega)) {
            actor = pFragment->actor;
            pos.v[0] = actor->t.t.translate.t.v[0];
            pos.v[1] = actor->t.t.translate.t.v[1];
            pos.v[2] = actor->t.t.translate.t.v[2];
            actor->t.t.translate.t.v[0] = 0.f;
            actor->t.t.translate.t.v[1] = 0.f;
            actor->t.t.translate.t.v[2] = 0.f;
            BrMatrix34PostRotate(&actor->t.t.mat, (br_angle)(long)(omega * dt * gDOUBLE_0058af80), &axis);
            actor->t.t.translate.t.v[0] = pos.v[0];
            actor->t.t.translate.t.v[1] = pos.v[1];
            actor->t.t.translate.t.v[2] = pos.v[2];
        }
    }
    if (pTime < pFragment->field_0x10) {
        pFragment->actor->render_style = BR_RSTYLE_FACES;
    } else {
        pFragment->actor->render_style = BR_RSTYLE_NONE;
    }
}

// FUNCTION: CARMA2_HW 0x004f00ca
void C2_HOOK_FASTCALL DoFragMovement(tSmash_vertex* pFragment, tU32  pTime, float pDelta_time) {
    float f_dt;
    float omega;
    br_vector3 pos;
    br_vector3 axis;
    br_actor* actor;

    f_dt = pDelta_time / 1000.f;
    if (f_dt != 0.f && !pFragment->field_0x34) {

        pFragment->actor->t.t.translate.t.v[0] += pFragment->v.v[0] * f_dt / WORLD_SCALE;
        pFragment->actor->t.t.translate.t.v[1] += (pFragment->v.v[1] * f_dt - 9.8f / 2.f * gGravity_multiplier * CARPOCALYPSE2_SQR(f_dt)) / WORLD_SCALE;
        pFragment->actor->t.t.translate.t.v[2] += pFragment->v.v[2] * f_dt / WORLD_SCALE;
        pFragment->v.v[1] -= f_dt * gGravity_multiplier / WORLD_SCALE;
        omega = BrVector3Length(&pFragment->omega);
        BrVector3InvScale(&axis, &pFragment->omega, omega);
        if (fabsf(f_dt * omega) >= 0.0001) {
            actor = pFragment->actor;
            BrVector3Copy(&pos, &actor->t.t.translate.t);
            BrVector3Set(&actor->t.t.translate.t, 0.f, 0.f, 0.f);
            BrMatrix34PostRotate(&actor->t.t.mat, BrRadianToAngle(omega * f_dt), &axis);
            BrVector3Copy(&actor->t.t.translate.t, &pos);
        }
    }
    if (pTime < pFragment->field_0x10) {
        pFragment->actor->render_style = BR_RSTYLE_FACES;
    } else {
        pFragment->actor->render_style = BR_RSTYLE_NONE;
    }
}

void C2_HOOK_FASTCALL KillFragment(tSmash_vertex* pFragment) {

    BrActorRemove(pFragment->actor);
    pFragment->end_time = 0;
}

// FUNCTION: CARMA2_HW 0x004f0150
void C2_HOOK_FASTCALL MungeGlassFragments2(int pEnd_race) {
    // GLOBAL: CARMA2_HW 0x006a7fc0
    static tU32 prev_glass_munge;
    tU32 the_time;
    float f_prev_glass_munge;
    tSmash_vertex* fragment;
    tU32 field_0x18;
    int i;
    int j;

    C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(gSmash_glass_fragments) != 200);
    C2_HOOK_BUG_ON(sizeof(tSmash_vertex) != 0x38);

    the_time = GetTotalTime();
    if (prev_glass_munge != 0) {
        f_prev_glass_munge = (float)the_time - prev_glass_munge;
        for (i = 0; i < CARPOCALYPSE2_ASIZE(gSmash_glass_fragments); i++) {
            fragment = &gSmash_glass_fragments[i];
            if (fragment->end_time != 0) {
                if (!pEnd_race && (the_time < fragment->end_time || (gAction_replay_mode && ARGetReplayRate() <= 0.f)) && the_time >= fragment->field_0xc) {
                    DoFragMovement(fragment, the_time, f_prev_glass_munge);
                    if (fragment->field_0x34) {
                        fragment->field_0x34 = 0;
                    }
                    fragment->time_last_move = the_time;
                } else {
                    field_0x18 = fragment->field_0x18;

                    if (the_time >= fragment->end_time) {
                        if (!(field_0x18 & 0x80000000)) {
                            PipeSingleShrapnelShower(field_0x18, fragment->time_last_move, 0, 0, NULL, NULL, 0.f, NULL, NULL, 0, NULL, 0, NULL, NULL, NULL);
                        } else if (field_0x18 & 0x40000000) {
                            PipeSingleGibShower(field_0x18, fragment->time_last_move, 0, 0, 0, NULL, NULL, NULL);
                        } else {
                            PipeSingleBloodSpurt(field_0x18, fragment->time_last_move, 0, NULL, NULL, NULL);
                        }
                    }
                    for (j = 0; j < CARPOCALYPSE2_ASIZE(gSmash_glass_fragments); j++) {
                        if (gSmash_glass_fragments[j].end_time != 0 &&
                                gSmash_glass_fragments[j].field_0x18 == field_0x18) {
                            KillFragment(&gSmash_glass_fragments[j]);
                        }
                    }
                }
            }
        }
    }
    prev_glass_munge = the_time;
}

void C2_HOOK_FASTCALL MungeAnimationRepairs(void) {
    tU32 the_time;
    tRepair_animation* animation;
    int i;

    the_time = GetTotalTime();
    for (i = 0; i < CARPOCALYPSE2_ASIZE(gRepair_animations); i++) {
        animation = &gRepair_animations[i];
        if (animation->field_0x0 != NULL && the_time - animation->field_0x4 >= 150) {
            tCar_crush_smashable_part* smashable = (tCar_crush_smashable_part*)animation->field_0x0;
            if (smashable->field_0x4c != 0) {
                ActuallyRepairSmash((tCar_spec*)animation->field_0x8, smashable, smashable->field_0x4c - 1);
                animation->field_0x4 += 150;
            }
            if (smashable->field_0x4c == 0) {
                animation->field_0x0 = NULL;
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x004f00f0
void C2_HOOK_FASTCALL MungeGlassFragments(void) {
#ifdef CARPOCALYPSE2_MATCHING
    tU32 the_time;
    tRepair_animation* animation;
    int i;

    MungeGlassFragments2(0);
    the_time = GetTotalTime();
    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gRepair_animations); i++) {
        animation = &gRepair_animations[i];
        if (animation->field_0x0 != NULL && the_time - animation->field_0x4 >= 150) {
            tCar_crush_smashable_part* smashable = (tCar_crush_smashable_part*)animation->field_0x0;
            if (smashable->field_0x4c != 0) {
                ActuallyRepairSmash((tCar_spec*)animation->field_0x8, smashable, smashable->field_0x4c - 1);
                animation->field_0x4 += 150;
            }
            if (smashable->field_0x4c == 0) {
                animation->field_0x0 = NULL;
            }
        }
    }
#else
    MungeGlassFragments2(0);
    MungeAnimationRepairs();
#endif
}

void C2_HOOK_FASTCALL FlushPowerupQueue(void) {
    int i;
    C2_HOOK_BUG_ON(sizeof(tPowerup_queue_item) != 0xc);

    for (i = 0; i < gSize_powerup_queue; i++) {
        GotPowerup(gPowerup_queue[i].car, gPowerup_queue[i].powerup_index);
        MayQueuePowerupRespawn(gPowerup_queue[i].powerup_index, gPowerup_queue[i].actor);
    }
    gSize_powerup_queue = 0;
}

// FUNCTION: CARMA2_HW 0x004ed860
void C2_HOOK_FASTCALL QueueNonCarCuboidActivation(br_actor* pActor, tNon_car_cuboid_activations* pActivations, void* pContext) {
    tU32 time;
    br_matrix34* matrix;
    int i;
    tNon_car_cuboid_activation* activation;
    tSmash_actor_search_context search;
    br_bounds3 bounds;
    tU32 min_column_x;
    tU32 max_column_x;
    tU32 min_column_z;
    tU32 max_column_z;
    volatile int* active;
    tU8* active_end;

    C2_HOOK_BUG_ON(sizeof(tNon_car_cuboid_activation) != 0x40);
    C2_HOOK_BUG_ON(sizeof(tDelayed_non_car) != 0xc0);

    time = GetTotalTime();
    if (pActor == NULL || pActor->identifier == NULL || pActor->identifier[0] != '&') {
        matrix = (br_matrix34*)((tU8*)pContext + 0x8c);
    } else {
        pActor = (br_actor*)((tU8*)pActor + 0x2c);
        matrix = (br_matrix34*)pActor;
    }
    activation = pActivations->activations;
    for (i = 0; i < pActivations->count_activations; i++, activation++) {
        int column_x;
        int column_z;
        tDelayed_non_car delayed;

        search.count = 0;
        if (activation->field_0x4 == 1) {
            bounds = activation->bounds;
        } else {
            int axis;

            BrMatrix34ApplyP(&bounds.min, &activation->bounds.min, matrix);
            BrMatrix34ApplyP(&bounds.max, &activation->bounds.max, matrix);
            for (axis = 0; axis < 3; axis++) {
                if (bounds.min.v[axis] > bounds.max.v[axis]) {
                    br_scalar value = bounds.min.v[axis];
                    bounds.min.v[axis] = bounds.max.v[axis];
                    bounds.max.v[axis] = value;
                }
            }
        }
        search.bounds = &bounds;
        search.field_0x4 = activation->field_0x8;
        XZToColumnXZ((tU8*)&min_column_x, (tU8*)&min_column_z, bounds.min.v[0], bounds.min.v[2], &gProgram_state.track_spec);
        XZToColumnXZ((tU8*)&max_column_x, (tU8*)&max_column_z, bounds.max.v[0], bounds.max.v[2], &gProgram_state.track_spec);
        column_x = (int)(min_column_x & 0xff);
        if (column_x <= (int)(max_column_x & 0xff)) {
            int column_offset;

            column_offset = column_x * sizeof(tTrack_square);
            do {
                column_z = (int)(min_column_z & 0xff);
                if (column_z <= (int)(max_column_z & 0xff)) {
                    do {
                        tTrack_square* square = (tTrack_square*)((tU8*)gProgram_state.track_spec.columns[column_z] + column_offset);
                        br_actor* column_actor = square->actor_0x0;

                        if (column_actor != NULL && DRActorEnumRecurse(column_actor, FUN_004edb20, &search) != 0) {
                            break;
                        }
                        column_z++;
                    } while (column_z <= (int)(max_column_z & 0xff));
                }
                column_x++;
                column_offset += sizeof(tTrack_square);
            } while (column_x <= (int)(max_column_x & 0xff));
        }
        delayed.field_0x0 = *(br_vector3*)((tU8*)pContext + 0x68);
        delayed.field_0xc = *(br_vector3*)((tU8*)matrix + 0x24);
        delayed.field_0x18 = (undefined4*)((tU8*)activation + 0x24);
        delayed.count_actions = search.count;
        for (column_x = 0; column_x < delayed.count_actions; column_x++) {
            delayed.actor_time_actions[column_x].actor = search.actors[column_x];
            delayed.actor_time_actions[column_x].time = time + IRandomBetween((tU16)activation->field_0x0, (tU16)activation->field_0x2);
        }
        active = &gSmash_explosions[0].active;
        active_end = (tU8*)gSmash_explosions + sizeof(gSmash_explosions) + sizeof(int);
        while (*active != 0) {
            active = (int*)((tU8*)active + sizeof(tSmash_explosion));
            if ((tU8*)active >= active_end) {
                break;
            }
        }
        if ((tU8*)active < active_end) {
            tSmash_explosion* queued;

            queued = (tSmash_explosion*)((tU8*)active - sizeof(int));
            queued->type = 0;
            queued->active = (int)(intptr_t)pContext;
            queued->what.non_car = delayed;
        }
    }
}

// FUNCTION: CARMA2_HW 0x004edb20
int C2_HOOK_CDECL FUN_004edb20(br_actor* pActor, tSmash_actor_search_context* pContext) {
    char* identifier;
    char first_digit;
    br_vector3* position;

    C2_HOOK_BUG_ON(sizeof(*pContext) != 0x58);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSmash_actor_search_context, actors, 0x8);

    identifier = pActor->identifier;
    if (identifier != NULL && identifier[0] == '&') {
        position = &pActor->t.t.translate.t;
        if (position->v[0] < pContext->bounds->min.v[0]) {
            return 0;
        }
        if (position->v[1] < pContext->bounds->min.v[1]) {
            return 0;
        }
        if (position->v[2] < pContext->bounds->min.v[2]) {
            return 0;
        }
        if (position->v[0] > pContext->bounds->max.v[0]) {
            return 0;
        }
        if (position->v[1] > pContext->bounds->max.v[1]) {
            return 0;
        }
        if (position->v[2] > pContext->bounds->max.v[2]) {
            return 0;
        }
        first_digit = identifier[1];
        if (first_digit < '0' || first_digit > '9') {
            return 0;
        }
        if (pContext->field_0x4 < 0 || pContext->field_0x4 == identifier[2] - 0x210 + first_digit * 10) {
            if (pContext->count >= 20) {
                return -1;
            }
            pContext->actors[pContext->count] = pActor;
            pContext->count++;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004f1030
void C2_HOOK_FASTCALL FUN_004f1030(tPhysics_object* pObject, int pMode, br_actor* pActor) {
    tU8* info;
    int type;

    info = (tU8*)pObject->owner;
    if (pMode == 0xe) {
        if (pObject != NULL && info != NULL && pObject->flags_0x238 == 1) {
            type = *(int*)(info + 0xc);
            if ((type == 0 || (type != 2 && type != 3)) && gSize_powerup_queue < 0x32 && type > 5) {
                gPowerup_queue[gSize_powerup_queue].actor = pActor;
                gPowerup_queue[gSize_powerup_queue].car = (tCar_spec*)info;
                gPowerup_queue[gSize_powerup_queue].powerup_index = 0xe;
                gSize_powerup_queue = gSize_powerup_queue + 1;
                return;
            }
        }
        FUN_004d9d00(pObject, 0xe);
        return;
    }
    if (pObject != NULL && info != NULL && pObject->flags_0x238 == 1) {
        type = *(int*)(info + 0xc);
        if ((type == 0 || (type != 2 && type != 3)) && gSize_powerup_queue < 0x32 && type == 8) {
            goto enqueue;
        }
    }
    if (pObject->flags_0x238 == 0x20) {
        if (gMutant_tail_state == 2 || GetTotalTime() - gINT_00705b70 < 0x1d4c) {
            goto enqueue;
        }
    }
    return;
enqueue:
    gPowerup_queue[gSize_powerup_queue].actor = pActor;
    gPowerup_queue[gSize_powerup_queue].car = &gProgram_state.current_car;
    gPowerup_queue[gSize_powerup_queue].powerup_index = pMode;
    gSize_powerup_queue = gSize_powerup_queue + 1;
}

void C2_HOOK_FASTCALL SmashItIntoVerySmallPiecesIndeed(tCar_spec* pCar, undefined4 pArg2, br_actor* pActor, undefined4 pArg4, undefined4 pArg5, void* pArg6, void* pArg7, undefined4 pArg8) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004ecfb0
void C2_HOOK_FASTCALL FlushSmashQueue(int pFlush_powerups) {
    int i;
    int j;
    tQueued_smash* queued_smash;

    C2_HOOK_BUG_ON(sizeof(tQueued_smash) != 0x88);

    for (i = 0; i < gCount_queued_smashes; i++) {
        queued_smash = &gQueued_smashes[i];
        if (queued_smash->actor->model != NULL) {
            if (queued_smash->field_0x40 != 0) {
                gMatrix34_006b78a0 = queued_smash->field_0x4c;
                BrVector3Copy(&gVector3_006a4688, &queued_smash->field_0x7c);
            }
            gINT_006a3334 = queued_smash->field_0x40 != 0;
            SmashItIntoVerySmallPiecesIndeed(
                    queued_smash->car, queued_smash->field_0x4, queued_smash->actor,
                    queued_smash->field_0x48, queued_smash->field_0x10, &queued_smash->field_0x14,
                    &queued_smash->field_0x2c, queued_smash->field_0x38);
        }
    }
    for (i = 0; i < gCount_queued_smashes; i++) {
        queued_smash = &gQueued_smashes[i];

        if (queued_smash->field_0x44) {
            DRModelUpdateAndKevificateMaterials(queued_smash->actor->model, BR_MODU_ALL);
            for (j = i + 1; j < gCount_queued_smashes; j++) {
                if (queued_smash->actor->model == gQueued_smashes[j].actor->model) {
                    queued_smash->field_0x44 = 0;
                }
            }
        }
    }
    if (!pFlush_powerups) {
        FlushPowerupQueue();
    }
    gCount_queued_smashes = 0;
}

void C2_HOOK_FASTCALL DoDelayedNonCar(tU32 pTime, tDelayed_non_car* pDelayed_non_car) {
    int i;
    int number_started;
    tDelayed_non_car_actor_item* read_action;
    tDelayed_non_car_actor_item* write_action;

    read_action = pDelayed_non_car->actor_time_actions;
    write_action = read_action;
    number_started = 0;

    for (i = 0; i < pDelayed_non_car->count_actions; read_action++, i++) {
        if (pTime < read_action->time) {
            *write_action++ = *read_action;
        } else if (read_action->actor->type_data == NULL) {
            tNon_car_spec *non_car;

            non_car = DoPullActorFromWorld(read_action->actor);
            if (non_car != NULL) {
                if (non_car->flags & 0x10000) {
                    MakeLiftGoUp(non_car);
                } else {
                    br_vector3 speed;
                    br_vector3 omega;

#ifdef CARPOCALYPSE2_FIX_BUGS
                    BrVector3Set(&speed, 0.f, 0.f, 0.f);
                    BrVector3Set(&omega, 0.f, 0.f, 0.f);
#endif

                    ApplyInitialMovement(pDelayed_non_car->field_0x18, &pDelayed_non_car->field_0x0, &speed, &omega, 1.f, &gZero_vector__smash,
                             &pDelayed_non_car->field_0xc, &read_action->actor->t.t.translate.t);
                    BrVector3InvScale(&non_car->collision_info->v, &speed, WORLD_SCALE);
                    BrVector3Scale(&non_car->collision_info->omega, &omega, .0958738f);
                }
                non_car->collision_info->disable_move_rotate = 0;
            }
            number_started += 1;
        }
    }
    pDelayed_non_car->count_actions -= number_started;
}

void C2_HOOK_FASTCALL DoDelayedSmash(tDelayed_smash* pDelayed_smash) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tDelayed_smash, field_0xc, 0xc);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tDelayed_smash, field_0x1c, 0x1c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tDelayed_smash, field_0x38, 0x38);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tDelayed_smash, field_0x80, 0x80);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tDelayed_smash, field_0x8c, 0x8c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tDelayed_smash, field_0x90, 0x90);

    if (pDelayed_smash->field_0x88 == 1) {
        if (pDelayed_smash->field_0x80->identifier[0] == '&' && (pDelayed_smash->field_0x80->identifier[1] >= '0' && pDelayed_smash->field_0x80->identifier[1] <= '9')) {
            DoPullActorFromWorld(pDelayed_smash->field_0x80);
            pDelayed_smash->field_0x80->identifier[3] = '!';
            BashObject(
                pDelayed_smash->field_0x90, pDelayed_smash->field_0x80,
                pDelayed_smash->field_0x8c, &pDelayed_smash->field_0xc,
                &pDelayed_smash->field_0x0, &pDelayed_smash->field_0x1c,
                1, 0);
            return;
        }
    }
    SmashEnvironment(
        pDelayed_smash->field_0x90, &pDelayed_smash->field_0x38,
        pDelayed_smash->field_0x8c, &pDelayed_smash->field_0xc,
        &pDelayed_smash->field_0x0, &pDelayed_smash->field_0x1c,
        1, 0);
}

// FUNCTION: CARMA2_HW 0x004ecc80
void C2_HOOK_FASTCALL MungeDelayedSideEffects(void) {
    int i;
    tU32 the_time;

#ifndef CARPOCALYPSE2_MATCHING
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSmash_explosion, what.non_car, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSmash_explosion, what.non_car.count_actions, 0x24);
#endif

    the_time = PDGetTotalTime();
    for (i = 0; i < CARPOCALYPSE2_ASIZE(gSmash_explosions); i++) {
        tSmash_explosion* delayed_effect = &gSmash_explosions[i];

        if (delayed_effect->active) {
            switch (delayed_effect->type) {
            case 0:
                DoDelayedNonCar(the_time, &delayed_effect->what.non_car);
                if (delayed_effect->what.non_car.count_actions == 0) {
                    delayed_effect->active = 0;
                }
                break;
            case 1:
                if (the_time >= delayed_effect->what.smash.field_0x18) {
                    DoDelayedSmash(&delayed_effect->what.smash);
                    delayed_effect->active = 0;
                }
                break;
            }
        }

    }
}

// GLOBAL: CARMA2_HW 0x006a7cf8
int gPowerup_respawn_sound_count;

// GLOBAL: CARMA2_HW 0x006a7cfc
int gPowerup_respawn_sound_ids[16];

// GLOBAL: CARMA2_HW 0x006a7f1c
tExplosion_animation gPowerup_respawn_explosion;

// GLOBAL: CARMA2_HW 0x006a7f24
int gPowerup_respawn_spill_time_offset;

// GLOBAL: CARMA2_HW 0x006a7f28
br_scalar gPowerup_respawn_full_size_min;

// GLOBAL: CARMA2_HW 0x006a7f2c
br_scalar gPowerup_respawn_full_size_max;

// GLOBAL: CARMA2_HW 0x006a7f30
br_scalar gPowerup_respawn_grow_size_min;

// GLOBAL: CARMA2_HW 0x006a7f34
br_scalar gPowerup_respawn_grow_size_max;

// GLOBAL: CARMA2_HW 0x006a7f38
br_scalar gPowerup_respawn_size;

// GLOBAL: CARMA2_HW 0x006a7f3c
br_scalar gPowerup_respawn_field_0x48;

// GLOBAL: CARMA2_HW 0x006a7f40
br_scalar gPowerup_respawn_field_0x4c;

// GLOBAL: CARMA2_HW 0x006a7f44
br_scalar gPowerup_respawn_field_0x40;

// GLOBAL: CARMA2_HW 0x006a7f48
br_scalar gPowerup_respawn_field_0x44;

// GLOBAL: CARMA2_HW 0x006a7f4c
br_material* gPowerup_respawn_material;

// GLOBAL: CARMA2_HW 0x006a7f50
int gPowerup_respawn_field_0x50;

// FUNCTION: CARMA2_HW 0x004ecea0
void C2_HOOK_FASTCALL DoPowerupRespawnSmash(br_actor *pActor) {
    br_bounds3* pBounds;

    if (gPowerup_respawn_sound_count != 0) {
        DRS3StartSound3D(gXXX_outlet, gPowerup_respawn_sound_ids[IRandomBetween(0, gPowerup_respawn_sound_count - 1)],
            (br_vector3*)((tU8*)pActor + 0x50), &gZero_v__car,
            1, 0xff, IRandomBetween(0xe000, 0x12000), 0x10000);
    }
    pBounds = &pActor->model->bounds;
    if (gPed_676914 == 0) {
        if (PointOutOfSight((br_vector3*)((tU8*)pActor + 0x50), *(undefined4*)&gYon_squared, gYon_squared) != 0) {
            goto check_spill;
        }
    }
    if (*(int*)&gPowerup_respawn_explosion != 0) {
        Explode(&gPowerup_respawn_explosion, pActor, pBounds, NULL);
    }
check_spill:
    if (gPowerup_respawn_material != NULL) {
        QueueOilSpill2(NULL, pActor, pActor->model,
            gPowerup_respawn_spill_time_offset,
            gPowerup_respawn_full_size_min,
            gPowerup_respawn_full_size_max,
            gPowerup_respawn_grow_size_min,
            gPowerup_respawn_grow_size_max,
            gPowerup_respawn_size,
            gPowerup_respawn_field_0x48,
            gPowerup_respawn_field_0x4c,
            gPowerup_respawn_field_0x40,
            gPowerup_respawn_field_0x44,
            gPowerup_respawn_material,
            gPowerup_respawn_field_0x50);
    }
}

// FUNCTION: CARMA2_HW 0x004f64d0
void C2_NAKED C2_HOOK_FASTCALL MungeSmashEdgeTriggers(tU32 pTime) {
#ifndef CARPOCALYPSE2_MATCHING
    if (gLast_munge_smash_edge_triggers != gPHIL_last_physics_tick) {
        tSmashable_item_spec* item;
        int i;

        item = gTrack_smashable_environment_specs;
        for (i = 0; i < gCount_track_smashable_environment_specs; i++, item++) {
            if (item->field_0x10 != 0 && item->field_0x10 != pTime) {
                item->field_0x10 = 0;
            }
        }
    }
    gLast_munge_smash_edge_triggers = gPHIL_last_physics_tick;
#else
    __asm {
        mov eax, dword ptr [gLast_munge_smash_edge_triggers]
        mov edx, dword ptr [gPHIL_last_physics_tick]
        cmp eax, edx
        push esi
        je munge_smash_edge_triggers_done
        mov eax, dword ptr [gCount_track_smashable_environment_specs]
        xor esi, esi
        test eax, eax
        jle munge_smash_edge_triggers_done
        mov edx, dword ptr [gTrack_smashable_environment_specs]
        lea eax, [edx + 0x10]

    munge_smash_edge_triggers_loop:
        mov edx, dword ptr [eax]
        test edx, edx
        je munge_smash_edge_triggers_next
        cmp edx, ecx
        je munge_smash_edge_triggers_next
        mov dword ptr [eax], 0

    munge_smash_edge_triggers_next:
        mov edx, dword ptr [gCount_track_smashable_environment_specs]
        inc esi
        add eax, 0x2e0
        cmp esi, edx
        jl munge_smash_edge_triggers_loop

    munge_smash_edge_triggers_done:
        mov eax, dword ptr [gPHIL_last_physics_tick]
        pop esi
        mov dword ptr [gLast_munge_smash_edge_triggers], eax
        ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004ecfa0
void C2_HOOK_FASTCALL InitSmashQueue(void) {

    gCount_queued_smashes = 0;
}

void C2_HOOK_FASTCALL MungeInternalCarGlass(tCar_spec* pCar_spec) {

    if (pCar_spec == &gProgram_state.current_car && gAction_replay_camera_mode == kActionReplayCameraMode_Internal) {
        MungeCarMaterials(pCar_spec, 1);
    }
}

// FUNCTION: CARMA2_HW 0x004ef840
void C2_HOOK_FASTCALL ActuallyRepairSmash(tCar_spec* pCar_spec, tCar_crush_smashable_part* pSmashable, int pLevel) {
    br_pixelmap* texture;

    pSmashable->field_0x4c = pLevel;
    texture = pSmashable->levels[pLevel].pixelmaps[IRandomBetween(0, pSmashable->levels[pLevel].count_pixelmaps - 1)];
    PipeSingleSmashTextureChange(pCar_spec, pSmashable->funk_material, texture);
    pSmashable->funk_material->user = texture;
    if (pCar_spec->field_0x1960 == NULL) {
        pSmashable->funk_material->colour_map = texture;
    }
    BrMaterialUpdate(pSmashable->funk_material, BR_MATU_COLOURMAP);
    MungeInternalCarGlass(pCar_spec);
    if (pLevel == 0 && pSmashable->funk >= 0) {
        EnableFunkotronic(pSmashable->funk);
    }
    if (pCar_spec != NULL && pCar_spec->driver == eDriver_local_human && gProgram_state.racing) {
        DRS3StartSound(gXXX_outlet, eSoundId_SmashRepair);
    }
}

// FUNCTION: CARMA2_HW 0x004ef9c0
void C2_HOOK_FASTCALL TotallyRepairSmash(tCar_spec *pCar_Spec, tCar_crush_buffer_entry *pSmash_data) {
    tCar_crush_smashable_part* smashable = pSmash_data->smashables;
    int i;

    for (i = 0; i < pSmash_data->count_smashables; i++, smashable++) {
        if (smashable->field_0x4c != 0) {
            ActuallyRepairSmash(pCar_Spec, smashable, 0);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004eb180
void C2_HOOK_FASTCALL ApplyInitialMovement(undefined4* pArg1, br_vector3* pArg2, br_vector3* pArg3, br_vector3* pArg4, float pArg5, br_vector3* pArg6, br_vector3* pArg7, br_vector3* pArg8) {
    float* f = (float*)pArg1;
    br_vector3 unit;
    float w = pArg5;
    float len;
    int i;

    pArg3->v[0] = f[2] * pArg2->v[0];
    pArg3->v[1] = f[2] * pArg2->v[1];
    pArg3->v[2] = f[2] * pArg2->v[2];

    if (pArg7 != NULL && pArg8 != NULL) {
        if (!f[0] && !f[4]) {
            w = (float)sqrt((pArg7->v[0] - pArg8->v[0]) * (pArg7->v[0] - pArg8->v[0]) + (pArg7->v[1] - pArg8->v[1]) * (pArg7->v[1] - pArg8->v[1]) + (pArg7->v[2] - pArg8->v[2]) * (pArg7->v[2] - pArg8->v[2]));
            unit.v[0] = FRandomBetween(f[0], f[4]) / f[4] * unit.v[0];
            unit.v[1] = FRandomBetween(f[0], f[4]) / f[4] * unit.v[1];
            unit.v[2] = FRandomBetween(f[0], f[4]) / f[4] * unit.v[2];
            pArg3->v[0] += unit.v[0];
            pArg3->v[1] += unit.v[1];
            pArg3->v[2] += unit.v[2];
        }
    }

    for (i = 0; i < 3; i++) {
        pArg3->v[i] = (FRandomPosNeg(f[0xc]) + pArg3->v[i]) * w;
        pArg4->v[i] = FRandomBetween(0.0f, 1.0f);
    }

    pArg3->v[1] += FRandomBetween(0.0f, f[0x10]);

    len = (float)sqrt(pArg6->v[0] * pArg6->v[0] + pArg6->v[1] * pArg6->v[1] + pArg6->v[2] * pArg6->v[2]);
    if (len > 2.3841858e-07f) {
        float inv = (float)(1.0 / (double)len);
        unit.v[0] = pArg6->v[0] * inv;
        unit.v[1] = pArg6->v[1] * inv;
        unit.v[2] = pArg6->v[2] * inv;
    } else {
        unit.v[0] = 1.0f;
        unit.v[1] = 0.0f;
        unit.v[2] = 0.0f;
    }
    unit.v[0] = FRandomBetween(0.0f, f[0x14]) * unit.v[0];
    unit.v[1] = FRandomBetween(0.0f, f[0x14]) * unit.v[1];
    unit.v[2] = FRandomBetween(0.0f, f[0x14]) * unit.v[2];
    pArg3->v[0] += unit.v[0];
    pArg3->v[1] += unit.v[1];
    pArg3->v[2] += unit.v[2];

    len = (float)sqrt(pArg4->v[0] * pArg4->v[0] + pArg4->v[1] * pArg4->v[1] + pArg4->v[2] * pArg4->v[2]);
    if (len > 2.3841858e-07f) {
        float inv = (float)(1.0 / (double)len);
        pArg4->v[0] = pArg4->v[0] * inv;
        pArg4->v[1] = pArg4->v[1] * inv;
        pArg4->v[2] = pArg4->v[2] * inv;
    } else {
        pArg4->v[0] = 1.0f;
        pArg4->v[1] = 0.0f;
        pArg4->v[2] = 0.0f;
    }

    pArg4->v[0] *= FRandomPosNeg(w * f[0x18]);
    pArg4->v[1] *= FRandomPosNeg(w * f[0x18]);
    pArg4->v[2] *= FRandomPosNeg(w * f[0x18]);
}

// FUNCTION: CARMA2_HW 0x004eb070
tSmash_vertex* C2_HOOK_FASTCALL FindShrapnelFragment(br_model* pModel, tU32 pId, int pPriority) {
    int i;
    int lowest_priority;
    tSmash_vertex* fragment;
    tSmash_vertex* lowest_priority_fragment;

    C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(gSmash_glass_fragments) != 200);

    lowest_priority = 10000;
    for (i = 0, fragment = gSmash_glass_fragments; i < (int)CARPOCALYPSE2_ASIZE(gSmash_glass_fragments); i++, fragment++) {
        if (fragment->end_time == 0) {
            i = -1;
            break;
        }
        if (fragment->priority < lowest_priority) {
            lowest_priority = fragment->priority;
            lowest_priority_fragment = fragment;
        }
    }
    if (i >= 0) {
        if (lowest_priority < pPriority) {
            fragment = lowest_priority_fragment;
        } else {
            return NULL;
        }
    }
    if (pModel != NULL) {
        fragment->actor->model = pModel;
    } else {
        fragment->actor->model = fragment->model;
    }
    if (fragment->actor->parent == NULL) {
        BrActorAdd(gNon_track_actor, fragment->actor);
    }
    fragment->priority = (tU8)pPriority;
    fragment->field_0x18 = pId;
    fragment->field_0xc = GetTotalTime();
    fragment->field_0x34 = 1;
    return fragment;
}

#pragma inline_depth(0)
// FUNCTION: CARMA2_HW 0x004eb6a0
void C2_HOOK_FASTCALL FUN_004eb6a0(br_matrix34* pMat) {
    float len;
    float inv;
    float x0;
    float y0;
    float z0;

    pMat->m[3][0] = 0;
    pMat->m[3][1] = 0;
    pMat->m[3][2] = 0;
    pMat->m[1][0] = FRandomPosNeg(1.0f);
    pMat->m[1][1] = FRandomPosNeg(1.0f);
    pMat->m[1][2] = FRandomPosNeg(1.0f);
    pMat->m[2][0] = FRandomPosNeg(1.0f);
    pMat->m[2][1] = FRandomPosNeg(1.0f);
    z0 = FRandomPosNeg(1.0f);
    len = sqrtf(pMat->m[2][0] * pMat->m[2][0] + pMat->m[2][1] * pMat->m[2][1] + z0 * z0);
    pMat->m[2][2] = z0;
    if (len > 2.3841858e-7f) {
        inv = 1.0 / len;
        pMat->m[2][0] = pMat->m[2][0] * inv;
        pMat->m[2][1] = pMat->m[2][1] * inv;
        pMat->m[2][2] = z0 * inv;
    } else {
        pMat->m[2][0] = 1.0f;
        pMat->m[2][1] = 0;
        pMat->m[2][2] = 0;
    }
    x0 = pMat->m[1][1] * pMat->m[2][2] - pMat->m[2][1] * pMat->m[1][2];
    y0 = pMat->m[1][2] * pMat->m[2][0] - pMat->m[1][0] * pMat->m[2][2];
    z0 = pMat->m[1][0] * pMat->m[2][1] - pMat->m[2][0] * pMat->m[1][1];
    pMat->m[0][0] = x0;
    pMat->m[0][1] = y0;
    pMat->m[0][2] = z0;
    len = sqrtf(x0 * x0 + y0 * y0 + z0 * z0);
    if (len > 2.3841858e-7f) {
        inv = 1.0 / len;
        pMat->m[0][0] = x0 * inv;
        pMat->m[0][1] = y0 * inv;
        pMat->m[0][2] = z0 * inv;
    } else {
        pMat->m[0][0] = 1.0f;
        pMat->m[0][1] = 0;
        pMat->m[0][2] = 0;
    }
    x0 = pMat->m[2][1] * pMat->m[0][2] - pMat->m[2][2] * pMat->m[0][1];
    y0 = pMat->m[2][2] * pMat->m[0][0] - pMat->m[2][0] * pMat->m[0][2];
    z0 = pMat->m[2][0] * pMat->m[0][1] - pMat->m[2][1] * pMat->m[0][0];
    pMat->m[1][0] = x0;
    pMat->m[1][1] = y0;
    pMat->m[1][2] = z0;
    len = x0 * x0 + y0 * y0 + z0 * z0;
    if (len < 0.9) {
        BrMatrix34Identity(pMat);
    }
}
#pragma inline_depth()

// FUNCTION: CARMA2_HW 0x004eb4a0
float C2_HOOK_FASTCALL PositionFragment(br_actor* pActor, tSmashable_initial_position_spec* pPosition, int pArg3, int pArg4, int pArg5, br_vector3* pArg6, br_vector3* pArg7, int pArg8) {
    br_matrix34 tmp;
    br_vector3 dir;
    br_vector3 pos;
    float scale;
    int i;

    scale = 1.0f;
    switch (pPosition->type) {
        case 0: {
            br_vector3 cross;
            br_vector3 n;

            FUN_004eb6a0(&tmp);
            BrVector3Set(&pos, FRandomPosNeg(1.f), FRandomPosNeg(1.f), FRandomPosNeg(1.f));
            cross.v[0] = tmp.m[0][1] * pos.v[2] - tmp.m[0][2] * pos.v[1];
            cross.v[1] = tmp.m[0][2] * pos.v[0] - tmp.m[0][0] * pos.v[2];
            cross.v[2] = tmp.m[0][0] * pos.v[1] - tmp.m[0][1] * pos.v[0];
            BrVector3Normalise(&n, &cross);
            scale = BrVector3Length(&n);
            return scale;
        }
        case 1: {
            BrMatrix34Post(&pActor->t.t.mat, pPosition);
            dir.v[0] = (float)FRandomBetween((int)pArg6->v[0], (int)pArg6->v[1]);
            for (i = 0; i < 3; i++) {
                pActor->t.t.mat.m[3][i] += dir.v[i] * pArg6->v[2];
            }
            return 1.0f;
        }
        case 2:
            if (pArg8 != 0) {
                memcpy(pActor->t.t.mat.m[0], pArg7, sizeof(br_matrix34));
            } else {
                memcpy(pActor->t.t.mat.m[0], pArg6, sizeof(br_matrix34));
            }
            return 1.0f;
        default:
            return scale;
    }
}

// FUNCTION: CARMA2_HW 0x004eb120
void C2_HOOK_FASTCALL InitFragmentSpeed(tSmash_vertex* pFragment, tSmashable_initial_speed_spec* pSpeed, br_vector3* pArg3, float pArg4, int* pArg5, int pArg6, br_vector3* pArg7, int pArg8, int pArg9) {

    ApplyInitialMovement((undefined4*)pSpeed, pArg3, &pFragment->v, &pFragment->omega, pArg4, pArg7, (br_vector3*)(uintptr_t)pArg8, (br_vector3*)(uintptr_t)pArg9);
    pFragment->field_0xc = pArg6;
    pFragment->time_last_move = pArg6;
    pFragment->end_time = pArg6 + pArg5[1];
    pFragment->field_0x10 = IRandomBetween(pArg5[0], pArg5[1]) + pArg6;
}

// STUB: CARMA2_HW 0x004f0380
void C2_HOOK_FAKE_THISCALL FUN_004f0380(tU32 pCurrent_time, int pArg2, int* pArg3, tU32 pArg4, undefined4 pArg5, undefined4 pArg6, undefined4 pArg7, undefined4 pArg8, tU32 pStart_time, tU32 pOwner, undefined4 pArg11, undefined4 pArg12, undefined* pArg13, float pArg14, undefined4 pArg15) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x00497860
void C2_HOOK_FASTCALL FUN_00497860(tNet_message* pMessage, void* pSender_address) {
    int number_of_players;
    int i;
    tPlayer_ID sender_id;
    tPlayer_ID message_id;
    tNet_game_player_info* player;

    if (gNet_mode != eNet_mode_none) {
        number_of_players = gNumber_of_net_players;
        i = 0;
        sender_id = *(tPlayer_ID*)((tU8*)pSender_address + 8);
        if (number_of_players <= 0) {
            return;
        }
        for (; i < number_of_players; i++) {
            if (gNet_players[i].ID == sender_id) {
                goto found_player;
            }
        }
        return;

found_player:
        player = &gNet_players[i];
        message_id = pMessage->header.field_0x4;
        *(volatile tPlayer_ID*)&player->field_0xd0 = message_id;
        if (player->ID == message_id) {
            player->car->collision_info->field_0x49c += 0x28;
        }
    }
}

// FUNCTION: CARMA2_HW 0x004ef830
void C2_HOOK_FASTCALL ActuallyRepairSmashThunk(tCar_spec* pCar_spec, tCar_crush_smashable_part* pSmashable, int pUnused, int pLevel) {

    ActuallyRepairSmash(pCar_spec, pSmashable, pLevel);
}
