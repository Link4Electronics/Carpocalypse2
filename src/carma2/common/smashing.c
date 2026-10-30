#include "smashing.h"

#include "brucetrk.h"
#include "car.h"
#include "crush.h"

#include "displays.h"

#include "explosions.h"

#include "finteray.h"

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

#include "racemem.h"

#include "replay.h"
#include "sound.h"

#include "structur.h"

#include "utility.h"
#include "world.h"

#include <brender/brender.h>

#include "c2_string.h"

#include "carpocalypse2_macros.h"
#include <math.h>

#include <string.h>

extern const br_scalar gFloat_0058aec0;
extern int gCount_shrapnel_shower_calls;
extern int gUnknown_006b9c60[100];
extern const double gDouble_0058af28;
extern const br_scalar gFloat_0058af30;
extern const br_scalar gFloat_0058af34;
extern const tU32 gSplit_triangle_vertex_orders[6][3];

// GLOBAL: CARMA2_HW 0x0058aec4
const br_scalar gFloat_0058aec4 = 2.384185791015625e-7f;

// GLOBAL: CARMA2_HW 0x0058aec8
const double gDouble_0058aec8 = 1.0;

// GLOBAL: CARMA2_HW 0x0058af38
const br_scalar gFloat_0058af38 = 0.3333333432674408f;

// GLOBAL: CARMA2_HW 0x0058aed8
const double gDouble_0058aed8 = 0.9;

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

// GLOBAL: CARMA2_HW 0x0058af40
const double gDOUBLE_0058af40 = 6.9;


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

// GLOBAL: CARMA2_HW 0x0065ff08
const tU32 gSplit_triangle_vertex_orders[6][3] = {
    { 0, 1, 2 },
    { 0, 2, 1 },
    { 1, 0, 2 },
    { 1, 2, 0 },
    { 2, 0, 1 },
    { 2, 1, 0 },
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

// C2PORT-NONMATCHING: best 20.27% | difftest PASS 2000 trials (spec difftest/specs/004edbe0_FUN_004edbe0.json) | reason stack-layout-x87-order
// FUNCTION: CARMA2_HW 0x004edbe0
void C2_HOOK_FASTCALL FUN_004edbe0(undefined4 param_1, int* param_2, int* param_3, undefined4 param_4, float param_5) {
  typedef unsigned char byte;
  typedef unsigned short ushort;
  typedef unsigned int uint;

  float fVar1;
  byte bVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  int iVar6;
  byte *pbVar7;
  int *piVar8;
  int iVar9;
  float *pfVar10;
  int iVar11;
  int iVar12;
  byte *pbVar13;
  undefined2 *puVar14;
  int *piVar15;
  float *pfVar16;
  byte *pbVar17;
  int *piVar18;
  int bVar19;
  float local_7e08;
  float local_7e04;
  float local_7e00;
  int local_7dfc;
  float local_7df8;
  float local_7df4;
  float local_7df0;
  int local_7de8;
  int local_7de0;
  int local_7dd8 [3];
  float local_7dcc;
  float local_7dc8;
  float local_7dc4;
  int local_7dc0;
  float local_7dbc;
  float local_7db8;
  float local_7db4;
  int local_7da4;
  int local_7da0 [20];
  int local_7d50;
  float local_7d4c;
  int *local_7d48;
  tBounds local_bounds;
  tFace_ref face_list[400];

  iVar6 = GetTotalTime();
  local_7de8 = 0;
  puVar14 = (undefined2 *)param_3[1];
  if (0 < *param_3) {
    do {
      if (*(int *)(puVar14 + 2) == 1) {
        memcpy(&local_bounds.original_bounds.min, puVar14 + 0x16, sizeof(br_bounds3));
      }
      else {
        BrMatrix34ApplyP(&local_bounds.original_bounds.min, (br_vector3 *)(puVar14 + 0x16),
                         (br_matrix34 *)((tU8 *)(uintptr_t)param_1 + 0x8c));
        BrMatrix34ApplyP(&local_bounds.original_bounds.max, (br_vector3 *)(puVar14 + 0x1c),
                         (br_matrix34 *)((tU8 *)(uintptr_t)param_1 + 0x8c));
        pfVar10 = (float *)&local_bounds.original_bounds.max;
        iVar9 = 3;
        do {
          if (*pfVar10 < pfVar10[-3]) {
            fVar1 = pfVar10[-3];
            pfVar10[-3] = *pfVar10;
            *pfVar10 = fVar1;
          }
          pfVar10 = pfVar10 + 1;
          iVar9 = iVar9 + -1;
        } while (iVar9 != 0);
      }
      local_bounds.mat = &gTrack_actor->t.t.mat;
      local_7de0 = FindFacesInBox(&local_bounds,face_list,400,NULL);
      if (*(int *)(puVar14 + 0x22) == 0) {
        local_7e08 = local_7e08 / param_5;
        local_7e04 = local_7e04 / param_5;
        local_7e00 = local_7e00 / param_5;
      }
      if (0 < local_7de0) {
        piVar15 = (int *)&face_list[0].a;
        do {
          iVar9 = -1;
          local_7dfc = 0;
          if ((piVar15[-0x12] != 0) &&
             (pbVar13 = *(byte **)(piVar15[-0x12] + 4), pbVar13 != (byte *)0x0)) {
            iVar11 = -1;
            pbVar17 = pbVar13;
            do {
              if (iVar11 == 0) break;
              iVar11 = iVar11 + -1;
              bVar2 = *pbVar17;
              pbVar17 = pbVar17 + 1;
            } while (bVar2 != 0);
            if ((iVar11 != -0xd) || (pbVar13[5] != 0x7c)) goto LAB_004ede0f;
            iVar9 = *(int *)(puVar14 + 0x14);
            if ((iVar9 == 0) || (iVar9 == 1)) {
LAB_004edd98:
              local_7df8 = ((float)piVar15[-0x11] + (float)piVar15[-0xe] + (float)piVar15[-0xb]) *
                           gFloat_0058af38;
              local_7df4 = ((float)piVar15[-0x10] + (float)piVar15[-0xd] + (float)piVar15[-10]) *
                           gFloat_0058af38;
              local_7df0 = ((float)piVar15[-0xf] + (float)piVar15[-9] + (float)piVar15[-0xc]) *
                           gFloat_0058af38;
              iVar9 = 0;
              local_7dfc = (int)(uintptr_t)&gTrack_smashable_environment_specs[pbVar13[6] - 1];
              goto LAB_004edf55;
            }
            if (iVar9 == 4) {
              pbVar17 = (byte *)(puVar14 + 4);
              pbVar7 = pbVar13;
              do {
                bVar2 = *pbVar7;
                bVar19 = bVar2 < *pbVar17;
                if (bVar2 != *pbVar17) {
LAB_004edd8b:
                  iVar9 = (1 - (uint)bVar19) - (uint)(bVar19 != 0);
                  goto LAB_004edd90;
                }
                if (bVar2 == 0) break;
                bVar2 = pbVar7[1];
                bVar19 = bVar2 < pbVar17[1];
                if (bVar2 != pbVar17[1]) goto LAB_004edd8b;
                pbVar7 = pbVar7 + 2;
                pbVar17 = pbVar17 + 2;
              } while (bVar2 != 0);
              iVar9 = 0;
LAB_004edd90:
              if (iVar9 == 0) goto LAB_004edd98;
            }
            goto LAB_004ee209;
          }
LAB_004ede0f:
          iVar11 = *piVar15;
          if (iVar11 == 0) {
LAB_004edf55:
            if (iVar9 != -1) {
              piVar8 = (int *)&gSmash_explosions[0].what.smash.field_0x80;
              iVar12 = 0;
              do {
                if (((piVar8[-0x21] != 0) && (piVar8[-0x22] == 1)) &&
                   (((iVar9 == 1 && (*piVar8 == *piVar15)) ||
                    ((((iVar9 == 0 && (*piVar8 == *piVar15)) &&
                      ((short)piVar8[1] == (short)piVar15[1])) &&
                     (*(short *)((int)piVar8 + 6) == *(short *)((int)piVar15 + 6))))))) {
                  local_7dfc = 0;
                  break;
                }
                piVar8 = piVar8 + 0x32;
                iVar12++;
              } while (iVar12 < CARPOCALYPSE2_ASIZE(gSmash_explosions));
              if ((local_7dfc != 0) && (*piVar15 != *param_2)) {
                local_7dd8[0] = param_2[0x1a];
                local_7dd8[1] = param_2[0x1b];
                local_7dd8[2] = param_2[0x1c];
                iVar11 = IRandomBetween(*puVar14,puVar14[1]);
                local_7d50 = iVar9;
                local_7dc0 = iVar11 + iVar6;
                if (*(int *)(puVar14 + 0x22) == 1) {
                  fVar5 = (float)param_2[0x2c] - local_7df8;
                  fVar1 = (float)param_2[0x2d] - local_7df4;
                  fVar4 = (float)param_2[0x2e] - local_7df0;
                  fVar3 = fVar4 * fVar4 + fVar1 * fVar1 + fVar5 * fVar5;
                }
                else {
                  if (*(int *)(puVar14 + 0x22) != 2) goto LAB_004ee0fc;
                  fVar5 = local_7df8 - (float)param_2[0x2c];
                  fVar1 = local_7df4 - (float)param_2[0x2d];
                  fVar4 = local_7df0 - (float)param_2[0x2e];
                  fVar3 = fVar4 * fVar4 + fVar1 * fVar1 + fVar5 * fVar5;
                }
                if (sqrt((double)fVar3) <= gFloat_0058aec4) {
                  local_7e08 = 1.0;
                  local_7e04 = 0.0;
                  local_7e00 = 0.0;
                }
                else {
                  fVar3 = (float)((double)gDouble_0058aec8 / sqrt((double)fVar3));
                  local_7e08 = fVar3 * fVar5;
                  local_7e04 = fVar3 * fVar1;
                  local_7e00 = fVar3 * fVar4;
                }
LAB_004ee0fc:
                if (*(float *)(puVar14 + 0x24) < gFloat_0058aec0) {
                  local_7d4c = -(param_5 * *(float *)(puVar14 + 0x24));
                  local_7dbc = local_7e08 * local_7d4c;
                  local_7db8 = local_7e04 * local_7d4c;
                  local_7db4 = local_7e00 * local_7d4c;
                }
                else {
                  local_7dbc = *(float *)(puVar14 + 0x24) * local_7e08;
                  local_7db8 = *(float *)(puVar14 + 0x24) * local_7e04;
                  local_7db4 = *(float *)(puVar14 + 0x24) * local_7e00;
                  local_7d4c = *(float *)(puVar14 + 0x24);
                }
                local_7dc8 = local_7df4;
                local_7dcc = local_7df8;
                local_7dc4 = local_7df0;
                piVar8 = piVar15 + -0x12;
                piVar18 = local_7da0;
                for (iVar9 = 0x14; iVar9 != 0; iVar9 = iVar9 + -1) {
                  *piVar18 = *piVar8;
                  piVar8 = piVar8 + 1;
                  piVar18 = piVar18 + 1;
                }
                local_7da4 = local_7dfc;
                local_7d48 = param_2;
                iVar9 = 0;
                piVar8 = (int *)&gSmash_explosions[0].active;
LAB_004ee1c9:
                if (*piVar8 != 0) goto code_r0x004ee1ce;
                gSmash_explosions[iVar9].type = 1;
                gSmash_explosions[iVar9].active = (int)(uintptr_t)param_3;
                piVar8 = local_7dd8;
                piVar18 = (int *)&gSmash_explosions[iVar9].what;
                for (iVar11 = 0x25; iVar11 != 0; iVar11 = iVar11 + -1) {
                  *piVar18 = *piVar8;
                  piVar8 = piVar8 + 1;
                  piVar18 = piVar18 + 1;
                }
              }
            }
          }
          else {
            pbVar13 = *(byte **)(iVar11 + 0x14);
            if (pbVar13 == (byte *)0x0) {
LAB_004ede3d:
              if (((iVar11 == 0) || (pbVar13 = *(byte **)(iVar11 + 0x14), pbVar13 == (byte *)0x0))
                 || ((*pbVar13 != 0x26 ||
                     (gUnknown_006b9c60[(int)(char)pbVar13[2] + (char)pbVar13[1] * 10]
                      == 0)))) goto LAB_004edf55;
            }
            else {
              iVar12 = -1;
              pbVar17 = pbVar13;
              do {
                if (iVar12 == 0) break;
                iVar12 = iVar12 + -1;
                bVar2 = *pbVar17;
                pbVar17 = pbVar17 + 1;
              } while (bVar2 != 0);
              if (((iVar12 != -0xd) || (pbVar13[5] != 0x7c)) && (pbVar13[1] != 0xa3))
              goto LAB_004ede3d;
            }
            if (((*pbVar13 == 0x26) && (bVar2 = pbVar13[1], '/' < (char)bVar2)) &&
               ((char)bVar2 < ':')) {
              if ((pbVar13[3] == 0x21) ||
                 (((iVar9 = *(int *)(puVar14 + 0x14), iVar9 != 0 && (iVar9 != 3)) &&
                  ((iVar9 != 6 ||
                   ((bVar2 != *(byte *)((int)puVar14 + 9) || (pbVar13[2] != *(byte *)(puVar14 + 5)))
                   )))))) goto LAB_004ee209;
LAB_004edf18:
              iVar11 = *piVar15;
              iVar9 = 1;
              local_7df8 = *(float *)(iVar11 + 0x50);
              local_7df4 = *(float *)(iVar11 + 0x54);
              local_7df0 = *(float *)(iVar11 + 0x58);
              local_7dfc = (int)(uintptr_t)&gTrack_smashable_environment_specs[pbVar13[6] - 1];
              goto LAB_004edf55;
            }
            iVar9 = *(int *)(puVar14 + 0x14);
            if ((iVar9 == 0) || (iVar9 == 2)) goto LAB_004edf18;
            if (iVar9 == 5) {
              pbVar17 = (byte *)(puVar14 + 4);
              pbVar7 = pbVar13;
              do {
                bVar2 = *pbVar7;
                bVar19 = bVar2 < *pbVar17;
                if (bVar2 != *pbVar17) {
LAB_004edf0b:
                  iVar9 = (1 - (uint)bVar19) - (uint)(bVar19 != 0);
                  goto LAB_004edf10;
                }
                if (bVar2 == 0) break;
                bVar2 = pbVar7[1];
                bVar19 = bVar2 < pbVar17[1];
                if (bVar2 != pbVar17[1]) goto LAB_004edf0b;
                pbVar7 = pbVar7 + 2;
                pbVar17 = pbVar17 + 2;
              } while (bVar2 != 0);
              iVar9 = 0;
LAB_004edf10:
              if (iVar9 == 0) goto LAB_004edf18;
            }
          }
LAB_004ee209:
          piVar15 = piVar15 + 0x14;
          local_7de0 = local_7de0 + -1;
        } while (local_7de0 != 0);
      }
      local_7de8 = local_7de8 + 1;
      puVar14 = puVar14 + 0x26;
    } while (local_7de8 < *param_3);
  }
  return;
code_r0x004ee1ce:
  piVar8 = piVar8 + 0x32;
  iVar9 = iVar9 + 1;
  if (piVar8 > (int *)((tU8 *)&gSmash_explosions[0].active + sizeof(gSmash_explosions) - 1)) goto LAB_004ee209;
  goto LAB_004ee1c9;
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

// C2PORT-NONMATCHING: best 98.73% | difftest PASS 6000 trials (spec difftest/specs/004ed490_SmashItIntoVerySmallPiecesIndeed.json) | reason award_given store sits before the credits compare instead of between fcomp and fnstsw
// FUNCTION: CARMA2_HW 0x004ed490
void C2_HOOK_FASTCALL SmashItIntoVerySmallPiecesIndeed(tPhysics_object* pObject, tSmashable_item_spec* pSpec, undefined4 pArg3, tConnotations* pConnotations, tU32 pSeed, undefined4 pArg6, const br_vector3* pPoint, const br_vector3* pSound_position, br_actor* pActor, tU32 pTime, undefined4 pArg11, br_matrix34* pTransform, br_bounds3* pBounds, br_vector3* pPosition, float pDistance, undefined4 pArg16, int pFlag) {
    int i;
    tCar_spec* owner;
    tSide_effects* side_effects;
    tSpecial_effects_spec* special_effects;
    tSmash_run_time_variable_change* variable_change;
    br_actor* actor;
    int value;

    if (pConnotations->count_sounds != 0) {
        DRS3StartSound3D(gCar_outlet,
            pConnotations->sounds[IRandomBetween(0, pConnotations->count_sounds - 1)],
            pSound_position, &gZero_v__car, 1, 255, IRandomBetween(0xe000, 0x12000), 0x10000);
    }
    if (pFlag == 0) {
        side_effects = &pConnotations->side_effects;
        if (side_effects->activations.count_activations != 0) {
            QueueNonCarCuboidActivation(pActor, &side_effects->activations, pObject);
        }
        if (side_effects->side_effects.count_side_effects != 0) {
            FUN_004edbe0((undefined4)(uintptr_t)pSpec, (int*)pObject, (int*)&side_effects->side_effects, pArg16, pDistance);
        }
        if (side_effects->room_turn_on_code != 0
                && pObject != NULL
                && pObject->owner != NULL
                && pObject->flags_0x238 == 1
                && ((tCar_spec*)pObject->owner)->driver == eDriver_local_human) {
            if (BR_SQR3((pObject->actor->t.t.translate.t.v[0] - pPoint->v[0]), (pObject->actor->t.t.translate.t.v[1] - pPoint->v[1]), (pObject->actor->t.t.translate.t.v[2] - pPoint->v[2])) < gDouble_0058af28) {
                FUN_004f1030(pObject, side_effects->room_turn_on_code, NULL);
            }
        }
    }
    for (i = 0; i < pConnotations->count_shrapnel; i++) {
        WorkerShrapnelShower((int)pArg3, (int*)&pConnotations->shrapnel[i], pSeed, pArg6, (undefined4)(uintptr_t)pPoint, (undefined4)(uintptr_t)pSound_position, (undefined4)(uintptr_t)pActor, (int)pTime, pArg11, pTransform, (undefined4)(uintptr_t)pBounds, pPosition, pDistance, (undefined4)(uintptr_t)&pObject->transform_matrix.m[3], pFlag);
    }
    if (pFlag == 0) {
        owner = (pObject != NULL && pObject->owner != NULL && pObject->flags_0x238 == 1) ? (tCar_spec*)pObject->owner : NULL;
        if ((owner != NULL
                    && (pConnotations->award.frequency == kRepeatability_Repeated
                        || (pConnotations->award.frequency == kRepeatability_SingleShot && pConnotations->award_given == 0))
                    && owner->driver == eDriver_local_human)
                || pConnotations->award.frequency == kRepeatability_DoItRegardless) {
            pConnotations->award_given = 1;
            if (pConnotations->award.field_0x4 > gFloat_0058aec0) {
                EarnCredits((int)pConnotations->award.field_0x4);
            } else {
                float credits = -(pDistance * pConnotations->award.field_0x4);
                if (credits >= gFloat_0058af30) {
                    credits = gFloat_0058af30;
                }
                EarnCredits100((int)credits);
            }
            if (pConnotations->award.field_0x8 > gFloat_0058aec0) {
                AwardTime((int)pConnotations->award.field_0x8);
            } else {
                float time = -(pDistance * pConnotations->award.field_0x8);
                if (time >= gFloat_0058af34) {
                    time = gFloat_0058af34;
                }
                AwardTime5((int)time);
            }
            if (pConnotations->award.field_0xc != 0) {
                NewTextHeadupSlot2(4, 0, 3000, -4, GetMiscString(pConnotations->award.field_0xc), 0);
            }
            if (pConnotations->award.field_0x10 != 0) {
                DoFancyHeadup(pConnotations->award.field_0x10);
            }
        }
    }
    special_effects = &pConnotations->special_effects;
    pDistance = gYon_squared;
    if (gAction_replay_mode != 0 || !((int (C2_HOOK_FASTCALL*)(const br_vector3*, br_scalar))PointOutOfSight)(&pActor->t.t.translate.t, pDistance)) {
        actor = pActor;
        if (special_effects->explosion_animation.count_groups != 0) {
            Explode(&special_effects->explosion_animation, actor, pBounds, NULL);
        }
    } else {
        actor = pActor;
    }
    if (special_effects->slick.material != NULL) {
        DoSpillage((tSlick_spec*)&special_effects->slick, actor);
    }
    variable_change = pConnotations->variable_changes.runtime_changes;
    for (i = 0; i < pConnotations->variable_changes.count; i++, variable_change++) {
        value = variable_change->field_0x0;
        if (value != 9999) {
            AccumulateRuntimeVariable((tU16)variable_change->field_0x2, value);
        } else {
            ToggleRuntimeVariable((tU16)variable_change->field_0x2);
        }
    }
    if (gCurrent_race.race_spec->race_type >= kRaceType_Smash
            && GetRuntimeVariable(gCurrent_race.race_spec->options.smash.var_smash_number) >= gCurrent_race.race_spec->options.smash.var_smash_target) {
        RaceCompleted(eRace_over_3);
    }
}

extern const double gDouble_0058aeb8;

// GLOBAL: CARMA2_HW 0x0058af18
const double gDouble_0058af18 = 0.09587379924285257;

// C2PORT-NONMATCHING: best 42.86% | difftest PASS 2000 trials (spec difftest/specs/004ecfb0_FlushSmashQueue.json) | reason stack-layout,x87-order,register-allocation
// FUNCTION: CARMA2_HW 0x004ecfb0
void C2_HOOK_FASTCALL FlushSmashQueue(int pFlush_powerups) {
    int i;
    int j;
    int k;
    tQueued_smash* queued_smash;
    tCar_crush_smashable_part* smashable;
    br_actor* actor;
    br_matrix34 actor_to_world;
    tConnotations* connotations;
    tU32 time;
    tU32 seed;
    int level;
    int target_level;
    int count_shrapnel_calls;
    char* identifier;
    br_model* model;
    int* smash_flag;
    int* next_actor;
    int* queue_matrix;
    int outer_count;

    C2_HOOK_BUG_ON(sizeof(tQueued_smash) != 0x88);

    queue_matrix = (int*)&gQueued_smashes[0].field_0x4c;
    for (i = 0; i < gCount_queued_smashes; i++, queue_matrix += 0x22) {
        queued_smash = (tQueued_smash*)((char*)queue_matrix - 0x4c);
        actor = (br_actor*)queue_matrix[-0x11];
        if (actor->model != NULL) {
            if (queue_matrix[-3] != 0) {
                gMatrix34_006b78a0 = *(br_matrix34*)queue_matrix;
                gVector3_006a4688 = *(br_vector3*)&queue_matrix[12];
                gINT_006a3334 = 1;
            } else {
                gINT_006a3334 = 0;
            }
            smashable = (tCar_crush_smashable_part*)queued_smash->smashable;
            target_level = queued_smash->field_0x38;
            time = GetTotalTime();
            level = smashable->field_0x4c;
            if (level < smashable->count_smashable_levels - 1) {
                if (target_level >= smashable->count_smashable_levels) {
                    target_level = smashable->count_smashable_levels - 1;
                }
                seed = (tU32)(uintptr_t)smashable ^ time;
                BrActorToActorMatrix34(&actor_to_world, actor, gUniverse_actor);
                ActuallySmashCarPart(queued_smash->car, smashable, target_level);
                count_shrapnel_calls = gCount_shrapnel_shower_calls;
                connotations = (tConnotations*)&smashable->levels[target_level].count_sounds;
                for (k = level + 1; k <= target_level; k++) {
                    if ((smashable->levels[k].flags & 1) == 0 || target_level - level == 1) {
                        SmashItIntoVerySmallPiecesIndeed((tPhysics_object*)(uintptr_t)queued_smash->field_0x4,
                            (tSmashable_item_spec*)(uintptr_t)queued_smash->field_0x48,
                            (undefined4)(uintptr_t)smashable, connotations, seed,
                            (undefined4)(uintptr_t)&queued_smash->field_0x14,
                            &queued_smash->field_0x2c, &queued_smash->field_0x2c, actor, time,
                            count_shrapnel_calls, &actor_to_world, NULL, NULL,
                            (float)sqrt(BR_SQR(queued_smash->field_0x20.v[1])
                                + BR_SQR(queued_smash->field_0x20.v[2])
                                + BR_SQR(queued_smash->field_0x20.v[0])),
                            (undefined4)(uintptr_t)&queued_smash->field_0x20, queued_smash->field_0x3c);
                    }
                }
                if (smashable->funk_material != NULL) {
                    identifier = smashable->funk_material->identifier;
                    if (identifier != NULL) {
                        switch (smashable->levels[smashable->field_0x4c].collision_type) {
                        case kTextureLevelCollisionChange_Solid:
                            *identifier = 'X';
                            break;
                        case kTextureLevelCollisionChange_PassThrough:
                            *identifier = '>';
                            break;
                        case kTextureLevelCollisionChange_Edges:
                            *identifier = '?';
                            break;
                        }
#ifndef CARPOCALYPSE2_MATCHING
                        if (getenv("C2_OBJLOG") != NULL) {
                            dr_dprintf("SMASHLOG actor=%p model=%p material=%p car=%p from=%d to=%d count=%d collision=%d name=%.48s", actor,
                                actor->model, smashable->funk_material, queued_smash->car, level, smashable->field_0x4c,
                                smashable->count_smashable_levels, smashable->levels[smashable->field_0x4c].collision_type, identifier);
                        }
#endif
                    }
                }
            }
        }
    }
    smash_flag = &gQueued_smashes[0].field_0x44;
    outer_count = gCount_queued_smashes;
    i = 1;
    if (0 < outer_count) {
        do {
            if (*smash_flag != 0) {
                model = ((br_actor*)smash_flag[-0xf])->model;
                DRModelUpdateAndKevificateMaterials(model, BR_MODU_ALL);
                outer_count = gCount_queued_smashes;
                if (i < gCount_queued_smashes) {
                    next_actor = smash_flag + 0x13;
                    j = gCount_queued_smashes - i;
                    do {
                        if (model == ((br_actor*)*next_actor)->model) {
                            *smash_flag = 0;
                        }
                        next_actor += 0x22;
                        j--;
                    } while (j != 0);
                }
            }
            smash_flag += 0x22;
            i++;
        } while (i <= outer_count);
    }
    if (!pFlush_powerups) {
        FlushPowerupQueue();
    }
    gCount_queued_smashes = 0;
}

// FUNCTION: CARMA2_HW 0x004ed2b0
void C2_HOOK_FASTCALL ActuallySmashCarPart(tCar_spec* pCar_spec, tCar_crush_smashable_part* pSmashable, int pLevel) {
    br_pixelmap* texture;
    br_pixelmap** pixelmaps;
    int i;
    unsigned int flags;
    float fVar1;
    tU32 uVar5;

    texture = NULL;
    if (pCar_spec != NULL) {
        for (i = 0; i < CARPOCALYPSE2_ASIZE(gRepair_animations); i++) {
            if (gRepair_animations[i].field_0x0 == pSmashable) {
                gRepair_animations[i].field_0x0 = NULL;
                break;
            }
        }
    }
    i = pSmashable->levels[pLevel].count_pixelmaps;
    if (i != 0) {
        i = IRandomBetween(0, i - 1);
        pixelmaps = (br_pixelmap**)pSmashable->levels;
        texture = pixelmaps[pLevel * 0xb1 + i];
    }
    PipeSingleSmashTextureChange(pCar_spec, pSmashable->funk_material, texture);
    pSmashable->funk_material->user = texture;
    if (pCar_spec == NULL || pCar_spec->field_0x1960 == NULL) {
        pSmashable->funk_material->colour_map = texture;
    }
    pSmashable->field_0x4c = pLevel;
    flags = pSmashable->levels[pLevel].flags;
    if (pCar_spec == NULL || (flags & 0x1c) != 0) {
        if ((flags & 4) != 0 && PercentageChance(0x32)) {
            pSmashable->funk_material->map_transform.m[0][0] *= 0.8f;
        }
        if ((flags & 8) != 0 && PercentageChance(0x32)) {
            pSmashable->funk_material->map_transform.m[1][1] *= 0.8f;
        }
        if ((flags & 0x10) != 0) {
            i = IRandomBetween(0, 3);
            if (i == 0) {
                goto random_scale;
            }
            i--;
            if (i == 0) {
                goto random_rotate_1;
            }
            i--;
            if (i != 0) {
                goto no_transform;
            }
            fVar1 = pSmashable->funk_material->map_transform.m[0][0];
            pSmashable->funk_material->map_transform.m[0][0] = -pSmashable->funk_material->map_transform.m[0][1];
            pSmashable->funk_material->map_transform.m[0][1] = -fVar1;
            fVar1 = pSmashable->funk_material->map_transform.m[1][0];
            *(tU32*)&pSmashable->funk_material->map_transform.m[1][0] = *(tU32*)&pSmashable->funk_material->map_transform.m[0][1];
            goto set_m11;
random_rotate_1:
            uVar5 = *(tU32*)&pSmashable->funk_material->map_transform.m[0][0];
            *(tU32*)&pSmashable->funk_material->map_transform.m[0][0] = *(tU32*)&pSmashable->funk_material->map_transform.m[0][1];
            *(tU32*)&pSmashable->funk_material->map_transform.m[0][1] = uVar5;
            fVar1 = pSmashable->funk_material->map_transform.m[1][0];
            pSmashable->funk_material->map_transform.m[1][0] = -pSmashable->funk_material->map_transform.m[0][1];
            fVar1 = -fVar1;
            goto set_m11;
random_scale:
            pSmashable->funk_material->map_transform.m[0][0] *= 0.8f;
            fVar1 = pSmashable->funk_material->map_transform.m[1][1] * 0.8f;
set_m11:
            pSmashable->funk_material->map_transform.m[1][1] = fVar1;
        }
no_transform:
        ;
    } else {
        BrMatrix23Identity(&pSmashable->funk_material->map_transform);
    }
    BrMaterialUpdate(pSmashable->funk_material, BR_MATU_ALL);
    if (pCar_spec == &gProgram_state.current_car && gAction_replay_camera_mode == 8) {
        MungeCarMaterials(pCar_spec, 1);
    }
    if (pSmashable->funk >= 0) {
        DisableFunkotronic(pSmashable->funk);
    }
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

// GLOBAL: CARMA2_HW 0x0058af28
const double gDouble_0058af28 = 2.0;

// GLOBAL: CARMA2_HW 0x0058af30
const br_scalar gFloat_0058af30 = 20000.0f;

// GLOBAL: CARMA2_HW 0x0058af34
const br_scalar gFloat_0058af34 = 60.0f;

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

#ifndef CARPOCALYPSE2_MATCHING
static void LogGlassFragmentSource(br_actor* actor, int faces, int vertices, const tU16* indices) {
    br_model* model;
    int i, minimum = 65535, maximum = 0;
    if (getenv("C2_OBJLOG") == NULL) return;
    model = actor->model;
    for (i = 0; i < vertices && i < 200; i++) {
        if (indices[i] < minimum) minimum = indices[i];
        if (indices[i] > maximum) maximum = indices[i];
    }
    dr_dprintf("GLASSFRAG actor=%p user=%p model=%p faces=%d vertices=%d indices=%p range=%d..%d",
        actor, actor->user, model, faces, vertices, indices, minimum, maximum);
    dr_dprintf("GLASSFRAG model_name=%.48s actor_name=%.48s",
        model->identifier != NULL ? model->identifier : "<NULL>",
        actor->identifier != NULL ? actor->identifier : "<NULL>");
    dr_dprintf("GLASSFRAG model_nv=%u nf=%u flags=%04x source=%p prepared=%p stored=%p",
        model->nvertices, model->nfaces, model->flags, model->vertices, model->prepared, model->stored);
    for (i = 0; i < vertices && i < 200; i++) {
        dr_dprintf("GLASSFRAG index[%d]=%u%s", i, indices[i], indices[i] >= model->nvertices ? " OUTSIDE_MODEL" : "");
    }
}
#endif

// C2PORT-NONMATCHING: best 18.17% | difftest PASS 2000 trials (spec difftest/specs/004ec370_CreateSmashedGlassFragments.json; output fixture PASS 2000) | reason VC5 stack layout and x87 scheduling
// FUNCTION: CARMA2_HW 0x004ec370
void C2_HOOK_FASTCALL CreateSmashedGlassFragments(tU32 pSeed, br_vector3* pDirection,
        br_actor* pSource_actor, tU32 pTime, int pFace_count, const tU16* pSource_faces,
        int pVertex_count, const tU16* pSource_vertices, tU32 pOwner, tU32 pFlags,
        float pLongest_edge_squared, tSmashable_initial_speed_spec* pInitial_speed,
        const int* pLifetime_range, tU32 pFragment_owner, br_matrix34* pSource_transform) {
    float vertex_data[2000];
    tU16 face_data[4000];
    int saved_random_seed;
    int face_count;
    int vertex_count;
    int face_index;
    int i;

    if (pFace_count == 0 || pSource_faces == NULL || pVertex_count == 0 || pSource_vertices == NULL) {
        return;
    }

#ifndef CARPOCALYPSE2_MATCHING
    LogGlassFragmentSource(pSource_actor, pFace_count, pVertex_count, pSource_vertices);
#endif
    saved_random_seed = rand();
    srand(pSeed);

    for (i = 0; i < 2000; i++) {
        vertex_data[i] = 0.f;
    }
    for (i = 0; i < pVertex_count; i++) {
        const float* source_vertex;
        int component;

        source_vertex = (const float*)&pSource_actor->model->vertices[pSource_vertices[i]];
        for (component = 0; component < 10; component++) {
            vertex_data[i * 10 + component] = source_vertex[component];
        }
    }

    for (i = 0; i < 4000; i++) {
        face_data[i] = 0;
    }
    for (i = 0; i < pFace_count; i++) {
        face_data[i * 20 + 0] = pSource_faces[i * 3 + 0];
        face_data[i * 20 + 1] = pSource_faces[i * 3 + 1];
        face_data[i * 20 + 2] = pSource_faces[i * 3 + 2];
        *(tU32*)&face_data[i * 20 + 4] = pOwner;
        face_data[i * 20 + 8] = 0;
    }

    face_count = pFace_count;
    vertex_count = pVertex_count;
    for (face_index = 0; face_index < face_count; face_index++) {
        int subdivision;

        for (subdivision = 0; subdivision < 4; subdivision++) {
            tU16* face;
            float* v0;
            float* v1;
            float* v2;
            float edge_02_squared;
            float edge_01_squared;
            float edge_12_squared;
            const tU32* order;
            int order_index;
            int first_midpoint;
            int second_midpoint;
            int component;

            if (face_count > 198 || vertex_count > 198) {
                break;
            }

            face = &face_data[face_index * 20];
            v0 = &vertex_data[face[0] * 10];
            v1 = &vertex_data[face[1] * 10];
            v2 = &vertex_data[face[2] * 10];

            edge_02_squared = (v0[0] - v2[0]) * (v0[0] - v2[0])
                    + (v0[2] - v2[2]) * (v0[2] - v2[2])
                    + (v0[1] - v2[1]) * (v0[1] - v2[1]);
            edge_01_squared = (v0[0] - v1[0]) * (v0[0] - v1[0])
                    + (v0[2] - v1[2]) * (v0[2] - v1[2])
                    + (v0[1] - v1[1]) * (v0[1] - v1[1]);
            edge_12_squared = (v1[0] - v2[0]) * (v1[0] - v2[0])
                    + (v1[2] - v2[2]) * (v1[2] - v2[2])
                    + (v1[1] - v2[1]) * (v1[1] - v2[1]);

            if (edge_01_squared < pLongest_edge_squared
                    && edge_02_squared < pLongest_edge_squared
                    && edge_12_squared < pLongest_edge_squared
                    && PercentageChance(0) == 0) {
                break;
            }

            if (edge_02_squared <= edge_01_squared || edge_12_squared <= edge_01_squared) {
                if (edge_02_squared <= edge_12_squared || edge_01_squared <= edge_12_squared) {
                    order_index = 2 + IRandomBetween(0, 1);
                } else {
                    order_index = IRandomBetween(0, 1);
                }
            } else {
                order_index = 4 + IRandomBetween(0, 1);
            }
            order = gSplit_triangle_vertex_orders[order_index];

            first_midpoint = vertex_count;
            second_midpoint = vertex_count + 1;
            {
                float fraction;
                float* point;

                fraction = FRandomBetween(0.2f, 0.8f);
                point = &vertex_data[first_midpoint * 10];
                for (component = 0; component < 5; component++) {
                point[component] = (vertex_data[face[order[1]] * 10 + component]
                        - vertex_data[face[order[0]] * 10 + component]) * fraction
                        + vertex_data[face[order[0]] * 10 + component];
                }
            }
            {
                float fraction;
                float* point;

                fraction = FRandomBetween(0.2f, 0.8f);
                point = &vertex_data[second_midpoint * 10];
                for (component = 0; component < 5; component++) {
                point[component] = (vertex_data[face[order[2]] * 10 + component]
                        - vertex_data[face[order[0]] * 10 + component]) * fraction
                        + vertex_data[face[order[0]] * 10 + component];
                }
            }

            face_data[face_count * 20 + 0] = face[order[1]];
            face_data[face_count * 20 + 1] = (tU16)first_midpoint;
            face_data[face_count * 20 + 2] = (tU16)second_midpoint;
            *(tU32*)&face_data[face_count * 20 + 4] = pOwner;
            face_data[face_count * 20 + 8] = 0;

            face_data[(face_count + 1) * 20 + 0] = face[order[1]];
            face_data[(face_count + 1) * 20 + 1] = face[order[2]];
            face_data[(face_count + 1) * 20 + 2] = (tU16)second_midpoint;
            *(tU32*)&face_data[(face_count + 1) * 20 + 4] = pOwner;
            face_data[(face_count + 1) * 20 + 8] = 0;

            face[order[1]] = (tU16)first_midpoint;
            face[order[2]] = (tU16)second_midpoint;
            vertex_count += 2;
            face_count += 2;
        }
    }

    {
        br_vector3 centre;
        int vertex_index;

        centre.v[0] = 0.f;
        centre.v[1] = 0.f;
        centre.v[2] = 0.f;
        for (vertex_index = 0; vertex_index < vertex_count; vertex_index++) {
            br_vector3 transformed;

            BrMatrix34ApplyP(&transformed, (br_vector3*)&vertex_data[vertex_index * 10], pSource_transform);
            vertex_data[vertex_index * 10 + 0] = transformed.v[0];
            vertex_data[vertex_index * 10 + 1] = transformed.v[1];
            vertex_data[vertex_index * 10 + 2] = transformed.v[2];
            if ((pFlags & 1) != 0) {
                centre.v[0] += transformed.v[0];
                centre.v[1] += transformed.v[1];
                centre.v[2] += transformed.v[2];
            }
        }
        centre.v[0] /= vertex_count;
        centre.v[1] /= vertex_count;
        centre.v[2] /= vertex_count;

        for (face_index = 0; face_index < face_count; face_index++) {
            tSmash_vertex* fragment;
            br_model* model;
            br_vector3 fragment_centre;
            float vertex_count_float;
            float speed_scale;
            double model_extent;
            int vertex;

            fragment = FindShrapnelFragment(NULL, pFragment_owner, 3);
            if (fragment == NULL) {
                break;
            }
            model = fragment->actor->model;
            for (vertex = 0; vertex < 3; vertex++) {
                tU16 index;
                float* source;

                index = face_data[face_index * 20 + vertex];
                source = &vertex_data[index * 10];
                model->vertices[vertex].p.v[0] = source[0];
                model->vertices[vertex].p.v[1] = source[1];
                model->vertices[vertex].p.v[2] = source[2];
                model->vertices[vertex].map.v[0] = source[3];
                model->vertices[vertex].map.v[1] = source[4];
            }
            model->faces[0].material = (br_material*)pOwner;

            vertex_count_float = (float)model->nvertices;
            fragment_centre.v[0] = (model->vertices[0].p.v[0] + model->vertices[1].p.v[0] + model->vertices[2].p.v[0]) / vertex_count_float;
            fragment_centre.v[1] = (model->vertices[0].p.v[1] + model->vertices[1].p.v[1] + model->vertices[2].p.v[1]) / vertex_count_float;
            fragment_centre.v[2] = (model->vertices[0].p.v[2] + model->vertices[1].p.v[2] + model->vertices[2].p.v[2]) / vertex_count_float;
            for (vertex = 0; vertex < 3; vertex++) {
                model->vertices[vertex].p.v[0] -= fragment_centre.v[0];
                model->vertices[vertex].p.v[1] -= fragment_centre.v[1];
                model->vertices[vertex].p.v[2] -= fragment_centre.v[2];
            }

            BrMatrix34Identity(&fragment->actor->t.t.mat);
            for (vertex = 0; vertex < 3; vertex++) {
                float spread;

                spread = (pFlags & 1) != 0 ? 0.04f : 0.015f;
                fragment->actor->t.t.translate.t.v[vertex] = FRandomPosNeg(spread)
                        + ((pFlags & 1) != 0 ? centre.v[vertex] : fragment_centre.v[vertex]);
            }
            BrModelUpdate(model, 0x15);

            model_extent = (double)model->bounds.max.v[0] - model->bounds.min.v[0]
                    + model->bounds.max.v[1] - model->bounds.min.v[1]
                    + model->bounds.max.v[2] - model->bounds.min.v[2];
            if (model_extent < 0.1) {
                speed_scale = (float)((0.2 - model_extent) * 5.0);
            } else {
                speed_scale = 0.5f;
            }
            ApplyInitialMovement((undefined4*)pInitial_speed, pDirection, &fragment->v, &fragment->omega,
                    speed_scale, (br_vector3*)&model->prepared->groups[0].faces[0].eqn, NULL, NULL);
            fragment->field_0xc = pTime;
            fragment->time_last_move = pTime;
            fragment->end_time = pTime + pLifetime_range[1];
            fragment->field_0x10 = pTime + IRandomBetween(pLifetime_range[0], pLifetime_range[1]);
        }
    }

    srand(saved_random_seed);
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
float C2_HOOK_FASTCALL PositionFragment(br_actor* pActor, tSmashable_initial_position_spec* pInitial_position, br_actor* pSource_actor, br_vector3* pPosition4, br_vector3* pPosition5, br_actor* pRotation_actor, float* pRandom_ranges, br_matrix34* pPost_matrix) {
    int i;
    float local_1c = 1.f;
    float local_18;
    float local_14;
    float local_10;
    br_vector3 local_c;
    br_vector3 transformed_vector;

    local_1c = 1.f;

    switch (pInitial_position->type) {
        case kInitialSmashablePosition_SphereClumped:
            FUN_004eb6a0(&pActor->t.t.mat);
            if (pInitial_position->position.sphere.where != 0) {
                pPosition5 = pPosition4;
            }
            pActor->t.t.translate.t = *pPosition5;
            local_c.v[0] = FRandomPosNeg(1.f);
            local_c.v[1] = FRandomPosNeg(1.f);
            local_c.v[2] = FRandomPosNeg(1.f);
            local_18 = FRandomBetween(0.f, pInitial_position->position.sphere.radius);
            local_1c = local_18 / pInitial_position->position.sphere.radius;
            local_14 = (float)sqrt((double)(local_c.v[2] * local_c.v[2] + local_c.v[1] * local_c.v[1] + local_c.v[0] * local_c.v[0]));
            local_10 = local_18 / local_14;
            pActor->t.t.translate.t.v[0] = local_10 * local_c.v[0] + pPosition5->v[0];
            pActor->t.t.translate.t.v[1] = local_10 * local_c.v[1] + pPosition5->v[1];
            pActor->t.t.translate.t.v[2] = local_10 * local_c.v[2] + pPosition5->v[2];
            break;

        case kInitialSmashablePosition_BoxClumped:
            FUN_004eb6a0(&pActor->t.t.mat);
            for (i = 0; i < 3; i++) {
                if (pRandom_ranges != NULL) {
                    local_c.v[i] = pInitial_position->position.box.v[i]
                        * FRandomBetween(pRandom_ranges[i], pRandom_ranges[i + 3]);
                } else {
                    local_c.v[i] = pInitial_position->position.box.v[i] * gFloat_0058aec0;
                }
            }
            BrMatrix34ApplyV(&transformed_vector, &local_c, &pRotation_actor->t.t.mat);
            pActor->t.t.translate.t.v[0] = transformed_vector.v[0] + pPosition4->v[0];
            pActor->t.t.translate.t.v[1] = transformed_vector.v[1] + pPosition4->v[1];
            pActor->t.t.translate.t.v[2] = transformed_vector.v[2] + pPosition4->v[2];
            break;

        case kInitialSmashablePosition_ActorBased:
            if (pPost_matrix != NULL) {
                pActor->t.t.mat = pSource_actor->t.t.mat;
                BrMatrix34Post(&pActor->t.t.mat, pPost_matrix);
            } else {
                pActor->t.t.mat = pSource_actor->t.t.mat;
                BrMatrix34Post(&pActor->t.t.mat, &pRotation_actor->t.t.mat);
            }
            break;
    }
    return local_1c;
}

// FUNCTION: CARMA2_HW 0x004eb120
void C2_HOOK_FASTCALL InitFragmentSpeed(tSmash_vertex* pFragment, tSmashable_initial_speed_spec* pSpeed, br_vector3* pArg3, float pArg4, int* pArg5, int pArg6, br_vector3* pArg7, int pArg8, int pArg9) {

    ApplyInitialMovement((undefined4*)pSpeed, pArg3, &pFragment->v, &pFragment->omega, pArg4, pArg7, (br_vector3*)(uintptr_t)pArg8, (br_vector3*)(uintptr_t)pArg9);
    pFragment->field_0xc = pArg6;
    pFragment->time_last_move = pArg6;
    pFragment->end_time = pArg6 + pArg5[1];
    pFragment->field_0x10 = IRandomBetween(pArg5[0], pArg5[1]) + pArg6;
}

// FUNCTION: CARMA2_HW 0x004f0380
void C2_HOOK_FAKE_THISCALL FUN_004f0380(tU32 pCurrent_time, int pArg2, int* pArg3, tU32 pArg4, undefined4 pArg5, undefined4 pArg6, undefined4 pArg7, undefined4 pArg8, tU32 pStart_time, tU32 pOwner, undefined4 pArg11, undefined4 pArg12, undefined* pArg13, float pArg14, undefined4 pArg15) {
    int i;
    tSmash_vertex* fragment;
    float time_since;

    WorkerShrapnelShower(pArg2, pArg3, pArg4, pArg5, pArg6, pArg7, pArg8, pStart_time, pOwner, (br_matrix34*)pArg11, pArg12, (br_vector3*)pArg13, pArg14, pArg15, 0);
    time_since = (float)(pCurrent_time - pStart_time);
    fragment = gSmash_glass_fragments;
    i = CARPOCALYPSE2_ASIZE(gSmash_glass_fragments);
    do {
        if (fragment->end_time != 0 && fragment->field_0x18 == pOwner) {
            fragment->field_0x34 = 0;
            FUN_004eff30(fragment, GetTotalTime(), time_since);
            fragment->field_0x34 = 1;
        }
        fragment++;
        i--;
    } while (i != 0);
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
