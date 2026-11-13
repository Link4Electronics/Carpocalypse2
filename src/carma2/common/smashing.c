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

#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern int C2_HOOK_FASTCALL AccumulateRuntimeVariable(int pAddress, int pAmount);
extern void C2_HOOK_FASTCALL ActuallyRepairSmash(tCar_spec* pCar_spec, tCar_crush_smashable_part* pSmashable, int pLevel);
extern void C2_HOOK_FASTCALL ActuallySmashCarPart(tCar_spec* pCar_spec, tCar_crush_smashable_part* pSmashable, int pLevel);
extern void C2_HOOK_FASTCALL AwardTime(tU32 pTime);
extern void C2_HOOK_FASTCALL AwardTime5(tU32 pTime);
extern br_uint_16 C2_HOOK_CDECL BrActorToActorMatrix34();
extern void C2_HOOK_CDECL BrMatrix34ApplyV();
extern void C2_HOOK_CDECL BrMatrix34Identity();
extern void C2_HOOK_CDECL BrMatrix34Post();
extern void C2_HOOK_CDECL BrMatrix34PostRotate();
extern intptr_t C2_HOOK_FASTCALL DRActorEnumRecurse(br_actor* pActor, br_actor_enum_cbfn* callback, void* arg);
extern void C2_HOOK_FASTCALL DRModelUpdateAndKevificateMaterials(br_model* pModel, br_uint_16 pFlags);
extern int C2_HOOK_FASTCALL DRS3StartSound3D(tS3_outlet* pOutlet, int pSound_id, const br_vector3* pInitial_position, const br_vector3* pInitial_velocity, int pRepeats, int pVolume, int pPitch, int pSpeed);
extern void C2_HOOK_FASTCALL DoFancyHeadup(int pIndex);
extern void C2_HOOK_FASTCALL DoFragMovement(tSmash_vertex* pFragment, tU32  pTime, float pDelta_time);
extern void C2_HOOK_FASTCALL EarnCredits(int pAmount);
extern void C2_HOOK_FASTCALL EarnCredits100(int pAmount);
extern void C2_HOOK_FASTCALL Explode(tExplosion_animation* pExplosion_animation, br_actor* pActor, br_bounds3* pBounds, br_vector3* pPos);
extern float C2_HOOK_STDCALL FRandomBetween(float pA, float pB);
extern float C2_HOOK_STDCALL FRandomPosNeg(float pN);
extern void C2_HOOK_FASTCALL FUN_004d9d00(tPhysics_object* pObject, int pMode);
extern int C2_HOOK_CDECL FUN_004edb20();
extern const char* C2_HOOK_FASTCALL GetMiscString(int pIndex);
extern int C2_HOOK_FASTCALL GetRuntimeVariable(int pAddress);
extern tU32 C2_HOOK_FASTCALL GetTotalTime(void);
extern int C2_HOOK_FASTCALL GotPowerup(tCar_spec* pCar, int pIndex);
extern int C2_HOOK_FASTCALL IRandomBetween(int pA, int pB);
extern void C2_HOOK_FASTCALL MayQueuePowerupRespawn(int pPowerup_index, br_actor* actor);
extern int C2_HOOK_FASTCALL NewTextHeadupSlot2(int pSlot_index, int pFlash_rate, int pLifetime, int pFont_index, const char* pText, int pQueue_it);
extern int C2_HOOK_FASTCALL PointOutOfSight(const br_vector3* pPoint, undefined4 pArg2, br_scalar pMax_distance);
extern void QueueOilSpill2();
extern void C2_HOOK_FASTCALL RaceCompleted(tRace_over_reason pReason);
extern int C2_HOOK_FASTCALL ToggleRuntimeVariable(int pAddress);
extern void C2_HOOK_FASTCALL WorkerShrapnelShower(int pOwner, int* pShrapnel_spec, tU32 pSeed, undefined4 pArg4, undefined4 pArg5, undefined4 pArg6, undefined4 pArg7, int pTime, undefined4 pArg9, br_matrix34* pTransform, undefined4 pFragment_owner, br_vector3* pPosition, float pDistance, undefined4 pArg14, undefined4 pArg15);
extern void C2_HOOK_FASTCALL XZToColumnXZ(tU8* pColumn_x, tU8* pColumn_z, br_scalar pX, br_scalar pZ, tTrack_spec* pTrack_spec);
extern void __cdecl _ftol();
extern int gCount_track_smashable_environment_specs;
extern tRace_info gCurrent_race;
extern tDecal gDecals[50];
extern float gGravity_multiplier;
extern tU32 gINT_00705b70;
extern int gMutant_tail_state;
extern tNet_mode gNet_mode;
extern tNet_game_player_info gNet_players[12];
extern int gNumber_of_net_players;
extern tU32 gPHIL_last_physics_tick;
extern int gPed_676914;
extern tSmashable_item_spec gPowerup_respawn_smashable;
extern tProgram_state gProgram_state;
extern tRepair_animation gRepair_animations[25];
extern int gSize_powerup_queue;
extern tSmash_explosion gSmash_explosions[20];
extern tSmashable_item_spec* gTrack_smashable_environment_specs;
extern br_actor* gUniverse_actor;
extern tS3_outlet* gXXX_outlet;
extern float gYon_squared;
extern br_vector3 gZero_v__car;
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_CDECL BrMatrix34ApplyP();
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern int gPowerup_respawn_sound_ids[16];
extern void C2_HOOK_FASTCALL ApplyInitialMovement(undefined4* pArg1, br_vector3* pArg2, br_vector3* pArg3, br_vector3* pArg4, float pArg5, br_vector3* pArg6, br_vector3* pArg7, br_vector3* pArg8);
extern float C2_HOOK_FASTCALL BashObject(tPhysics_object* pObject, br_actor* pActor, float pArg3, br_vector3 *pArg4, br_vector3* pArg5, br_vector3* pArg6, int pArg7, int pArg8);
extern void C2_HOOK_CDECL BrMaterialUpdate();
extern void C2_HOOK_CDECL BrMatrix23Identity();
extern void C2_HOOK_CDECL BrModelUpdate();
extern void C2_HOOK_FASTCALL DisableFunkotronic(int pIndex);
extern tNon_car_spec* C2_HOOK_FASTCALL DoPullActorFromWorld(br_actor* actor);
extern int C2_HOOK_FASTCALL FindFacesInBox(tBounds* bnds, tFace_ref* face_list, int max_face, tWorld_callbacks* pWorld_callbacks);
extern tSmash_vertex* C2_HOOK_FASTCALL FindShrapnelFragment(br_model* pModel, tU32 pField_0x18, int pPriority);
extern void C2_HOOK_FASTCALL MakeLiftGoUp(tNon_car_spec* pNon_car);
extern void C2_HOOK_FASTCALL MungeCarMaterials(tCar_spec* pCar, int pInternal_cam);
extern int C2_HOOK_FASTCALL PercentageChance(int pC);
extern void C2_HOOK_FASTCALL PipeSingleSmashTextureChange(tCar_spec* pCar_spec, br_material* pMaterial, br_pixelmap* pTexture);
extern float C2_HOOK_FASTCALL SmashEnvironment(tPhysics_object* pObject, undefined4* pArg2, float pArg3, br_vector3* pArg4, br_vector3* pArg5, br_vector3* pArg6, int pArg7, int pArg8);
extern void __cdecl _chkstk();
extern tActionReplayCameraMode gAction_replay_camera_mode;
extern char gDOUBLE_0058aef8[];
extern char gFLOAT_0058af20[];
extern char gSmashable_triggers_by_identifier[];
extern br_actor* gTrack_actor;
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern int gPowerup_respawn_field_0x50;
extern float C2_HOOK_FASTCALL ARGetReplayRate(void);
extern br_actor* C2_HOOK_CDECL BrActorRemove();
extern void C2_HOOK_CDECL BrResFree();
extern char* C2_HOOK_CDECL BrResStrDup();
extern void C2_NORETURN C2_HOOK_FASTCALL PDFatalError(const char* pThe_str);
extern void C2_HOOK_FASTCALL PipeSingleBloodSpurt(uintptr_t pId, undefined4 pArg2, undefined4 pArg3, br_vector3* pArg4, br_vector3* pArg5, br_vector3* pArg6);
extern void C2_HOOK_FASTCALL PipeSingleGibShower(uintptr_t pId, undefined4 pArg2, undefined4 pArg3, undefined4 pArg4, undefined4 pArg5, br_vector3* pArg6, br_vector3* pArg7, br_vector3* pArg8);
extern void C2_HOOK_FASTCALL PipeSingleShrapnelShower(uintptr_t pId, undefined4 pArg2, undefined4 pArg3, undefined4 pArg4, br_vector3* pArg5, br_vector3* pArg6, float pArg7, br_vector3* pArg8, br_vector3* pArg9, undefined4 pArg10, br_bounds3* pArg11, undefined4 pArg12, br_matrix34* pArg13, br_vector3* pArg14, void* pArg15);
#endif
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
void C2_NAKED C2_HOOK_FASTCALL MungeSmashMaterialNames(void) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;
    tSmashable_item_spec* item_spec;

    C2_HOOK_BUG_ON(sizeof(tSmashable_item_spec) != 0x2e0);

    item_spec = gTrack_smashable_environment_specs;
    for (i = 0; i < gCount_track_smashable_environment_specs; i++, item_spec++) {
        if (item_spec->trigger_type == kSmashableTrigger_Material) {
            SplondificatalizeIdentifier(
                item_spec->trigger_object.material,
                &item_spec->trigger_object.material->identifier,
                i);
        }
    }
#else
    __asm {
            mov eax, dword ptr [gCount_track_smashable_environment_specs]
            sub esp, 0x88
            push ebx
            xor ebx, ebx
            test eax, eax
            jle L004f0d45
            mov eax, dword ptr [gTrack_smashable_environment_specs]
            push edi
            add eax, 4
            push esi
            push ebp
            mov dword ptr [esp + 0x14], eax
    L004f0be5:
            mov ecx, dword ptr [eax + 4]
            test ecx, ecx
            jne L004f0d26
            mov ebp, dword ptr [eax]
            or ecx, 0xffffffff
            xor eax, eax
            lea esi, [esp + 0x18]
            mov edx, dword ptr [ebp + 4]
            mov dword ptr [esp + 0x10], esi
            mov edi, edx
            repne scasb
            not ecx
            sub edi, ecx
            mov eax, ecx
            mov esi, edi
            mov edi, dword ptr [esp + 0x10]
            shr ecx, 2
            rep movsd
            mov ecx, eax
            xor eax, eax
            and ecx, 3
            rep movsb
            lea edi, [esp + 0x18]
            or ecx, 0xffffffff
            repne scasb
            not ecx
            dec ecx
            mov esi, ecx
            cmp esi, 4
            jge L004f0c4f
            push edx
            lea ecx, [esp + 0x1c]
            push offset gSplit_triangle_vertex_orders+0x1bc
            push ecx
            call sprintf
            add esp, 0xc
            lea ecx, [esp + 0x18]
            call PDFatalError
    L004f0c4f:
            lea edx, [esp + esi + 0x14]
            or ecx, 0xffffffff
            mov edi, edx
            xor eax, eax
            repne scasb
            not ecx
            lea esi, [esp + 0x58]
            sub edi, ecx
            mov eax, ecx
            mov dword ptr [esp + 0x10], esi
            mov esi, edi
            mov edi, dword ptr [esp + 0x10]
            shr ecx, 2
            rep movsd
            mov ecx, eax
            push offset gSplit_triangle_vertex_orders+0x1b0
            and ecx, 3
            push edx
            rep movsb
            call sprintf
            add esp, 8
            lea ecx, [esp + 0x1d]
            push 0x7c
            push offset gSplit_triangle_vertex_orders+0x1ac
            push ecx
            call sprintf
            mov dl, bl
            add esp, 0xc
            lea edi, [esp + 0x58]
            or ecx, 0xffffffff
            xor eax, eax
            inc dl
            repne scasb
            not ecx
            mov byte ptr [esp + 0x1e], dl
            sub edi, ecx
            lea edx, [esp + 0x18]
            mov esi, edi
            mov edi, edx
            mov edx, ecx
            or ecx, 0xffffffff
            repne scasb
            mov ecx, edx
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, edx
            mov edx, dword ptr [ebp + 4]
            and ecx, 3
            rep movsb
            mov edi, edx
            or ecx, 0xffffffff
            mov byte ptr [esp + 0x23], al
            repne scasb
            not ecx
            dec ecx
            cmp ecx, 0xc
            jae L004f0d05
            push edx
            call BrResFree
            add esp, 4
            lea eax, [esp + 0x18]
            push eax
            push ebp
            call BrResStrDup
            add esp, 8
            mov dword ptr [ebp + 4], eax
            jmp L004f0d26
    L004f0d05:
            lea edi, [esp + 0x18]
            or ecx, 0xffffffff
            xor eax, eax
            repne scasb
            not ecx
            sub edi, ecx
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            shr ecx, 2
            rep movsd
            mov ecx, eax
            and ecx, 3
            rep movsb
    L004f0d26:
            mov eax, dword ptr [esp + 0x14]
            mov ecx, dword ptr [gCount_track_smashable_environment_specs]
            inc ebx
            add eax, 0x2e0
            cmp ebx, ecx
            mov dword ptr [esp + 0x14], eax
            jl L004f0be5
            pop ebp
            pop esi
            pop edi
    L004f0d45:
            pop ebx
            add esp, 0x88
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004eff30
void C2_NAKED C2_HOOK_FASTCALL FUN_004eff30(tSmash_vertex* pFragment, tU32 pTime, float pDelta_time) {
#ifndef CARPOCALYPSE2_MATCHING
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
#else
    __asm {
            sub esp, 0x20
            fld dword ptr [esp + 0x24]
            fmul dword ptr [gFLOAT_0058af5c]
            push ebx
            push esi
            push edi
            mov ebx, edx
            mov esi, ecx
            fst dword ptr [esp + 0x30]
            fcomp dword ptr [gFloat_0058aec0]
            fnstsw ax
            test ah, 0x40
            jne DoFragMovement
            mov al, byte ptr [esi + 0x34]
            xor ecx, ecx
            cmp al, cl
            jne DoFragMovement
            fld dword ptr [esp + 0x30]
            fld dword ptr [esi + 0x1c]
            fld dword ptr [gGravity_multiplier]
            fmul st(0), st(2)
            fld dword ptr [esi + 0x20]
            fxch st(1)
            fmul st(0), st(3)
            fxch st(2)
            fmul dword ptr [esp + 0x30]
            fxch st(1)
            fmul dword ptr [esp + 0x30]
            fxch st(2)
            fmul qword ptr [gDOUBLE_0058af60]
            fxch st(1)
            fmul qword ptr [gDOUBLE_0058af68]
            mov eax, dword ptr [esi]
            fld dword ptr [eax + 0x50]
            fld dword ptr [esi + 0x24]
            fmul dword ptr [esp + 0x30]
            fst dword ptr [esp + 0x1c]
            fxch st(3)
            fsubp st(4), st(0)
            fxch st(1)
            fsubp st(1), st(0)
            fxch st(1)
            fmul qword ptr [gDOUBLE_0058af68]
            fxch st(2)
            fst dword ptr [esp + 0x18]
            fxch st(1)
            fstp dword ptr [eax + 0x50]
            mov eax, dword ptr [esi]
            fmul qword ptr [gDOUBLE_0058af68]
            fld dword ptr [eax + 0x54]
            fxch st(1)
            fsubp st(1), st(0)
            fld dword ptr [esp + 0x30]
            fxch st(1)
            fstp dword ptr [eax + 0x54]
            mov eax, dword ptr [esi]
            fld dword ptr [eax + 0x58]
            fxch st(2)
            fsubp st(2), st(0)
            fxch st(1)
            fstp dword ptr [eax + 0x58]
            mov edi, dword ptr [esi]
            fmul dword ptr [gGravity_multiplier]
            fld dword ptr [esi + 0x20]
            fxch st(1)
            fmul qword ptr [gDOUBLE_0058af70]
            add edi, 0x2c
            fsubp st(1), st(0)
            fstp dword ptr [esi + 0x20]
            fstp st(0)
            fld dword ptr [esi + 0x2c]
            fld dword ptr [esi + 0x30]
            fld dword ptr [esi + 0x28]
            fxch st(2)
            fst dword ptr [esp + 0xc]
            fxch st(1)
            fst dword ptr [esp + 0x10]
            fxch st(1)
            fmul dword ptr [esp + 0xc]
            fxch st(1)
            fmul dword ptr [esp + 0x10]
            fld st(2)
            fmul st(0), st(3)
            fxch st(1)
            faddp st(2), st(0)
            fld dword ptr [esi + 0x28]
            fxch st(1)
            faddp st(2), st(0)
            fxch st(1)
            fsqrt
            fst dword ptr [esp + 0x10]
            fstp st(2)
            fdiv st(0), st(1)
            fstp dword ptr [esp + 0x20]
            fld dword ptr [esi + 0x30]
            fld dword ptr [esi + 0x2c]
            fdiv st(0), st(2)
            fxch st(1)
            fdiv st(0), st(2)
            fld dword ptr [esp + 0x30]
            fmulp st(3), st(0)
            fxch st(1)
            fstp dword ptr [esp + 0x24]
            fld st(1)
            fabs
            fst dword ptr [esp + 0x30]
            fcomp qword ptr [gDOUBLE_0058af78]
            fstp dword ptr [esp + 0x28]
            fnstsw ax
            test ah, 1
            jne L004f00c8
            mov eax, dword ptr [edi + 0x24]
            mov edx, dword ptr [edi + 0x28]
            fmul qword ptr [gDOUBLE_0058af80]
            mov dword ptr [esp + 0x14], eax
            mov dword ptr [esp + 0x18], edx
            mov edx, dword ptr [edi + 0x2c]
            lea eax, [esp + 0x20]
            mov dword ptr [esp + 0x1c], edx
            mov dword ptr [edi + 0x24], ecx
            mov dword ptr [edi + 0x28], ecx
            mov dword ptr [edi + 0x2c], ecx
            push eax
            call _ftol
            push eax
            push edi
            call BrMatrix34PostRotate
            fld dword ptr [esp + 0x20]
            fld dword ptr [esp + 0x24]
            fld dword ptr [esp + 0x28]
            fxch st(2)
            fstp dword ptr [edi + 0x24]
            add esp, 0xc
            fstp dword ptr [edi + 0x28]
            fstp dword ptr [edi + 0x2c]
            jmp DoFragMovement
    L004f00c8:
            fstp st(0)
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004f00ca
void C2_NAKED C2_HOOK_FASTCALL DoFragMovement(tSmash_vertex* pFragment, tU32  pTime, float pDelta_time) {
#ifndef CARPOCALYPSE2_MATCHING
    if (pTime < pFragment->field_0x10) {
        pFragment->actor->render_style = BR_RSTYLE_FACES;
    } else {
        pFragment->actor->render_style = BR_RSTYLE_NONE;
    }
#else
    __asm {
            cmp ebx, dword ptr [esi + 0x10]
            jb L004f00de
            mov eax, dword ptr [esi]
            mov byte ptr [eax + 0x20], 1
            pop edi
            pop esi
            pop ebx
            add esp, 0x20
            ret 4
    L004f00de:
            mov ecx, dword ptr [esi]
            pop edi
            pop esi
            pop ebx
            mov byte ptr [ecx + 0x20], 4
            add esp, 0x20
            ret 4
            nop
            nop
            nop
    }
#endif
}

void C2_HOOK_FASTCALL KillFragment(tSmash_vertex* pFragment) {

    BrActorRemove(pFragment->actor);
    pFragment->end_time = 0;
}

// FUNCTION: CARMA2_HW 0x004f0150
void C2_NAKED C2_HOOK_FASTCALL MungeGlassFragments2(int pEnd_race) {
#ifndef CARPOCALYPSE2_MATCHING
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
                    FUN_004eff30(fragment, the_time, f_prev_glass_munge);
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
#else
    __asm {
            sub esp, 0x14
            mov dword ptr [esp + 8], ecx
            call GetTotalTime
            mov ecx, eax
            mov eax, dword ptr [gPowerup_respawn_field_0x50+0x70]
            test eax, eax
            mov dword ptr [esp], ecx
            je L004f02a0
            mov dword ptr [esp + 0xc], ecx
            mov dword ptr [esp + 0x10], 0
            fild qword ptr [esp + 0xc]
            push edi
            push esi
            push ebp
            push ebx
            fisub dword ptr [gPowerup_respawn_field_0x50+0x70]
            mov edi, offset gSmash_glass_fragments+0x34
            mov dword ptr [esp + 0x14], 0xc8
            fstp dword ptr [esp + 0x1c]
    L004f019a:
            mov eax, dword ptr [edi - 0x2c]
            test eax, eax
            je L004f0286
            mov edx, dword ptr [esp + 0x18]
            test edx, edx
            jne L004f01f8
            cmp ecx, eax
            jb L004f01cc
            mov eax, dword ptr [gPed_676914]
            test eax, eax
            je L004f01f8
            call ARGetReplayRate
            fcomp dword ptr [gFloat_0058aec0]
            fnstsw ax
            test ah, 0x41
            je L004f01f8
    L004f01cc:
            mov edx, dword ptr [esp + 0x10]
            mov eax, dword ptr [edi - 0x28]
            cmp edx, eax
            jb L004f01f8
            mov eax, dword ptr [esp + 0x1c]
            lea ecx, [edi - 0x34]
            push eax
            call FUN_004eff30
            cmp byte ptr [edi], 0
            je L004f01ec
            mov byte ptr [edi], 0
    L004f01ec:
            mov ecx, dword ptr [esp + 0x10]
            mov dword ptr [edi - 0x20], ecx
            jmp L004f0286
    L004f01f8:
            mov edx, dword ptr [esp + 0x10]
            mov eax, dword ptr [edi - 0x2c]
            mov ebx, dword ptr [edi - 0x1c]
            cmp edx, eax
            jb L004f025a
            test ebx, ebx
            jl L004f0230
            push 0
            push 0
            push 0
            push 0
            push 0
            mov edx, dword ptr [edi - 0x20]
            push 0
            push 0
            push 0
            push 0
            push 0
            push 0
            push 0
            push 0
            mov ecx, ebx
            call PipeSingleShrapnelShower
            jmp L004f025a
    L004f0230:
            push 0
            push 0
            test ebx, 0x40000000
            push 0
            push 0
            je L004f0250
            mov edx, dword ptr [edi - 0x20]
            push 0
            push 0
            mov ecx, ebx
            call PipeSingleGibShower
            jmp L004f025a
    L004f0250:
            mov edx, dword ptr [edi - 0x20]
            mov ecx, ebx
            call PipeSingleBloodSpurt
    L004f025a:
            mov esi, offset gSmash_glass_fragments+0x8
            mov ebp, 0xc8
    L004f0264:
            cmp dword ptr [esi], 0
            je L004f0280
            cmp dword ptr [esi + 0x10], ebx
            jne L004f0280
            mov eax, dword ptr [esi - 8]
            push eax
            call BrActorRemove
            add esp, 4
            mov dword ptr [esi], 0
    L004f0280:
            add esi, 0x38
            dec ebp
            jne L004f0264
    L004f0286:
            mov eax, dword ptr [esp + 0x14]
            mov ecx, dword ptr [esp + 0x10]
            add edi, 0x38
            dec eax
            mov dword ptr [esp + 0x14], eax
            jne L004f019a
            pop ebx
            pop ebp
            pop esi
            pop edi
    L004f02a0:
            mov dword ptr [gPowerup_respawn_field_0x50+0x70], ecx
            add esp, 0x14
            ret
    }
#endif
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
void C2_NAKED C2_HOOK_FASTCALL MungeGlassFragments(void) {
#ifndef CARPOCALYPSE2_MATCHING
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
#else
    __asm {
            push ebx
            push ebp
            push esi
            push edi
            xor ecx, ecx
            call MungeGlassFragments2
            call GetTotalTime
            mov ebp, eax
            mov esi, offset gRepair_animations+0x4
            xor ebx, ebx
    L004f0109:
            mov edi, dword ptr [esi - 4]
            cmp edi, ebx
            je L004f013e
            mov edx, dword ptr [esi]
            mov eax, ebp
            sub eax, edx
            cmp eax, 0x96
            jb L004f013e
            mov eax, dword ptr [edi + 0x4c]
            cmp eax, ebx
            je L004f0136
            mov ecx, dword ptr [esi + 4]
            dec eax
            push eax
            mov edx, edi
            call ActuallyRepairSmash
            add dword ptr [esi], 0x96
    L004f0136:
            cmp dword ptr [edi + 0x4c], ebx
            jne L004f013e
            mov dword ptr [esi - 4], ebx
    L004f013e:
            add esi, 0xc
            cmp esi, offset gDecals
            jl L004f0109
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret
            nop
            nop
    }
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
void C2_NAKED C2_HOOK_FASTCALL QueueNonCarCuboidActivation(br_actor* pActor, tNon_car_cuboid_activations* pActivations, void* pContext) {
#ifndef CARPOCALYPSE2_MATCHING
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
#else
    __asm {
            sub esp, 0x154
            push ebx
            push ebp
            push esi
            push edi
            mov edi, edx
            mov esi, ecx
            mov dword ptr [esp + 0x28], edi
            call GetTotalTime
            test esi, esi
            mov dword ptr [esp + 0x2c], eax
            je L004ed894
            mov eax, dword ptr [esi + 0x14]
            test eax, eax
            je L004ed894
            cmp byte ptr [eax], 0x26
            jne L004ed894
            add esi, 0x2c
            mov dword ptr [esp + 0x10], esi
            jmp L004ed8a6
    L004ed894:
            mov eax, dword ptr [esp + 0x168]
            add eax, 0x8c
            mov dword ptr [esp + 0x10], eax
            mov esi, eax
    L004ed8a6:
            mov eax, dword ptr [edi]
            mov ebx, dword ptr [edi + 4]
            test eax, eax
            mov dword ptr [esp + 0x20], 0
            mov dword ptr [esp + 0x14], ebx
            jle L004edb07
            add ebx, 0xc
            jmp L004ed8c8
    L004ed8c4:
            mov esi, dword ptr [esp + 0x10]
    L004ed8c8:
            mov byte ptr [esp + 0x51], 0
            mov eax, dword ptr [ebx - 8]
            cmp eax, 1
            jne L004ed8e4
            mov ecx, 6
            mov esi, ebx
            lea edi, [esp + 0x34]
            rep movsd
            jmp L004ed92a
    L004ed8e4:
            push esi
            lea ecx, [esp + 0x38]
            push ebx
            push ecx
            call BrMatrix34ApplyP
            add esp, 0xc
            lea edx, [ebx + 0xc]
            lea eax, [esp + 0x40]
            push esi
            push edx
            push eax
            call BrMatrix34ApplyP
            add esp, 0xc
            lea ecx, [esp + 0x40]
            mov edx, 3
    L004ed90e:
            fld dword ptr [ecx - 0xc]
            _emit 0xd8
        _emit 0x19
            fnstsw ax
            test ah, 0x41
            jne L004ed924
            fld dword ptr [ecx - 0xc]
            mov eax, dword ptr [ecx]
            mov dword ptr [ecx - 0xc], eax
            _emit 0xd9
        _emit 0x19
    L004ed924:
            add ecx, 4
            dec edx
            jne L004ed90e
    L004ed92a:
            mov eax, dword ptr [esp + 0x3c]
            lea ecx, [esp + 0x34]
            mov dword ptr [esp + 0x4c], ecx
            mov dl, byte ptr [ebx - 4]
            mov ecx, dword ptr [esp + 0x34]
            push offset gProgram_state+0x1ba0
            push eax
            mov byte ptr [esp + 0x58], dl
            push ecx
            lea edx, [esp + 0x3c]
            lea ecx, [esp + 0x30]
            call XZToColumnXZ
            lea ecx, [esp + 0x1c]
            mov edx, dword ptr [esp + 0x48]
            mov eax, dword ptr [esp + 0x40]
            push offset gProgram_state+0x1ba0
            push edx
            push eax
            lea edx, [esp + 0x24]
            call XZToColumnXZ
            mov ebp, dword ptr [esp + 0x24]
            mov ecx, dword ptr [esp + 0x1c]
            and ebp, 0xff
            and ecx, 0xff
            cmp ebp, ecx
            jg L004ed9ed
            lea edi, [ebp + ebp*2]
            shl edi, 2
    L004ed990:
            mov esi, dword ptr [esp + 0x30]
            mov edx, dword ptr [esp + 0x18]
            and esi, 0xff
            and edx, 0xff
            cmp esi, edx
            jg L004ed9db
    L004ed9a8:
            mov eax, dword ptr [gProgram_state+0x1bb8]
            mov ecx, dword ptr [eax + esi*4]
            lea eax, [ecx + edi]
            mov ecx, dword ptr [ecx + edi]
            test ecx, ecx
            je L004ed9cd
            lea edx, [esp + 0x4c]
            push edx
            mov edx, offset FUN_004edb20
            call DRActorEnumRecurse
            test eax, eax
            jne L004ed9db
    L004ed9cd:
            mov eax, dword ptr [esp + 0x18]
            inc esi
            and eax, 0xff
            cmp esi, eax
            jle L004ed9a8
    L004ed9db:
            mov ecx, dword ptr [esp + 0x1c]
            inc ebp
            and ecx, 0xff
            add edi, 0xc
            cmp ebp, ecx
            jle L004ed990
    L004ed9ed:
            mov edx, dword ptr [esp + 0x168]
            xor edi, edi
            add edx, 0x68
            mov eax, dword ptr [edx]
            mov dword ptr [esp + 0xa4], eax
            mov eax, dword ptr [esp + 0x10]
            mov ecx, dword ptr [edx + 4]
            add eax, 0x24
            mov dword ptr [esp + 0xa8], ecx
            mov edx, dword ptr [edx + 8]
            mov ecx, dword ptr [eax]
            mov dword ptr [esp + 0xac], edx
            mov dword ptr [esp + 0xb0], ecx
            mov edx, dword ptr [eax + 4]
            lea ecx, [ebx + 0x18]
            mov dword ptr [esp + 0xb4], edx
            mov dword ptr [esp + 0xbc], ecx
            mov eax, dword ptr [eax + 8]
            mov dword ptr [esp + 0xb8], eax
            movsx eax, byte ptr [esp + 0x51]
            test eax, eax
            mov dword ptr [esp + 0xc0], eax
            jle L004eda93
            lea esi, [esp + 0xc8]
            lea ebp, [esp + 0x54]
    L004eda5f:
            mov edx, dword ptr [ebp]
            mov eax, dword ptr [esp + 0x14]
            mov dword ptr [esi - 4], edx
            xor edx, edx
            mov dx, word ptr [ebx - 0xa]
            xor ecx, ecx
            mov cx, word ptr [eax]
            call IRandomBetween
            mov ecx, dword ptr [esp + 0x2c]
            add ebp, 4
            add eax, ecx
            inc edi
            mov dword ptr [esi], eax
            mov eax, dword ptr [esp + 0xc0]
            add esi, 8
            cmp edi, eax
            jl L004eda5f
    L004eda93:
            xor ecx, ecx
            mov eax, offset gSmash_explosions+0x4
    L004eda9a:
            cmp dword ptr [eax], 0
            je L004edaae
            add eax, 0xc8
            inc ecx
            cmp eax, offset gPowerup_respawn_smashable+0x4
            jl L004eda9a
            jmp L004edae2
    L004edaae:
            lea eax, [ecx + ecx*4]
            mov ecx, dword ptr [esp + 0x168]
            lea esi, [esp + 0xa4]
            lea eax, [eax + eax*4]
            shl eax, 3
            mov dword ptr [eax + gSmash_explosions], 0
            mov dword ptr [eax + gSmash_explosions+0x4], ecx
            mov ecx, 0x30
            lea edi, [eax + gSmash_explosions+0x8]
            rep movsd
    L004edae2:
            mov edx, dword ptr [esp + 0x28]
            mov eax, dword ptr [esp + 0x20]
            mov esi, dword ptr [esp + 0x14]
            inc eax
            mov ecx, dword ptr [edx]
            add esi, 0x40
            add ebx, 0x40
            cmp eax, ecx
            mov dword ptr [esp + 0x20], eax
            mov dword ptr [esp + 0x14], esi
            jl L004ed8c4
    L004edb07:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x154
            ret 4
    }
#endif
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

// FUNCTION: CARMA2_HW 0x004edbe0
void C2_NAKED C2_HOOK_FASTCALL FUN_004edbe0(undefined4 param_1, int* param_2, int* param_3, undefined4 param_4, float param_5) {
#ifndef CARPOCALYPSE2_MATCHING
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
#else
    __asm {
            mov eax, 0x7e0c
            call _chkstk
            mov dword ptr [esp + 0x20], edx
            push ebx
            push ebp
            call GetTotalTime
            mov ebp, dword ptr [esp + 0x7e18]
            mov dword ptr [esp + 0x38], eax
            mov dword ptr [esp + 0x2c], 0
            mov eax, dword ptr [ebp]
            mov ebx, dword ptr [ebp + 4]
            test eax, eax
            mov dword ptr [esp + 8], ebx
            jle L004ee243
            push edi
            push esi
    L004edc1c:
            mov eax, dword ptr [ebx + 4]
            lea edi, [ebx + 0x2c]
            cmp eax, 1
            mov dword ptr [esp + 0x38], edi
            jne L004edc3d
            mov esi, edi
            mov ecx, 6
            lea edi, [esp + 0xdc]
            rep movsd
            jmp L004edc96
    L004edc3d:
            mov eax, dword ptr [esp + 0x30]
            lea ecx, [esp + 0xdc]
            lea esi, [eax + 0x8c]
            push esi
            push edi
            push ecx
            call BrMatrix34ApplyP
            add esp, 0xc
            add edi, 0xc
            lea edx, [esp + 0xe8]
            push esi
            push edi
            push edx
            call BrMatrix34ApplyP
            add esp, 0xc
            lea ecx, [esp + 0xe8]
            mov edx, 3
    L004edc7a:
            fld dword ptr [ecx - 0xc]
            _emit 0xd8
        _emit 0x19
            fnstsw ax
            test ah, 0x41
            jne L004edc90
            fld dword ptr [ecx - 0xc]
            mov eax, dword ptr [ecx]
            mov dword ptr [ecx - 0xc], eax
            _emit 0xd9
        _emit 0x19
    L004edc90:
            add ecx, 4
            dec edx
            jne L004edc7a
    L004edc96:
            mov ecx, dword ptr [gTrack_actor]
            push 0
            add ecx, 0x2c
            push 0x190
            mov dword ptr [esp + 0xe0], ecx
            lea edx, [esp + 0x124]
            lea ecx, [esp + 0xe0]
            call FindFacesInBox
            mov ecx, dword ptr [ebx + 0x44]
            test ecx, ecx
            jne L004edcf4
            fld dword ptr [esp + 0x14]
            fdiv dword ptr [esp + 0x7e28]
            fstp dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x18]
            fdiv dword ptr [esp + 0x7e28]
            fstp dword ptr [esp + 0x18]
            fld dword ptr [esp + 0x1c]
            fdiv dword ptr [esp + 0x7e28]
            fstp dword ptr [esp + 0x1c]
    L004edcf4:
            test eax, eax
            jle L004ee226
            lea ebx, [esp + 0x164]
            mov dword ptr [esp + 0x3c], eax
    L004edd07:
            mov eax, dword ptr [ebx - 0x48]
            or esi, 0xffffffff
            test eax, eax
            mov dword ptr [esp + 0x20], 0
            je L004ede0f
            mov ebp, dword ptr [eax + 4]
            test ebp, ebp
            je L004ede0f
            mov edi, ebp
            or ecx, 0xffffffff
            xor eax, eax
            repne scasb
            not ecx
            dec ecx
            cmp ecx, 0xb
            jne L004ede0f
            cmp byte ptr [ebp + 5], 0x7c
            jne L004ede0f
            mov edx, dword ptr [esp + 0x10]
            mov eax, dword ptr [edx + 0x28]
            test eax, eax
            je L004edd98
            cmp eax, 1
            je L004edd98
            cmp eax, 4
            jne L004ee209
            mov eax, edx
            lea edi, [eax + 8]
            mov eax, ebp
    L004edd67:
            mov dl, byte ptr [eax]
            mov cl, dl
            cmp dl, byte ptr [edi]
            jne L004edd8b
            test cl, cl
            je L004edd87
            mov dl, byte ptr [eax + 1]
            mov cl, dl
            cmp dl, byte ptr [edi + 1]
            jne L004edd8b
            add eax, 2
            add edi, 2
            test cl, cl
            jne L004edd67
    L004edd87:
            xor eax, eax
            jmp L004edd90
    L004edd8b:
            sbb eax, eax
            sbb eax, -1
    L004edd90:
            test eax, eax
            jne L004ee209
    L004edd98:
            fld dword ptr [ebx - 0x44]
            fld dword ptr [ebx - 0x40]
            fld dword ptr [ebx - 0x3c]
            fxch st(2)
            fadd dword ptr [ebx - 0x38]
            fxch st(1)
            fadd dword ptr [ebx - 0x34]
            fxch st(2)
            fadd dword ptr [ebx - 0x24]
            fxch st(1)
            fadd dword ptr [ebx - 0x2c]
            fxch st(2)
            fadd dword ptr [ebx - 0x28]
            fxch st(1)
            fadd dword ptr [ebx - 0x30]
            fxch st(2)
            fmul dword ptr [gFloat_0058af38]
            fxch st(1)
            fmul dword ptr [gFloat_0058af38]
            fxch st(2)
            fmul dword ptr [gFloat_0058af38]
            fxch st(1)
            fstp dword ptr [esp + 0x24]
            xor eax, eax
            mov edx, dword ptr [gTrack_smashable_environment_specs]
            mov al, byte ptr [ebp + 6]
            xor esi, esi
            fxch st(1)
            fstp dword ptr [esp + 0x28]
            lea ecx, [eax + eax*2]
            shl ecx, 3
            sub ecx, eax
            shl ecx, 5
            fstp dword ptr [esp + 0x2c]
            lea eax, [ecx + edx - 0x2e0]
            mov dword ptr [esp + 0x20], eax
            jmp L004edf55
    L004ede0f:
            mov ebp, dword ptr [ebx]
            test ebp, ebp
            je L004edf55
            mov edx, dword ptr [ebp + 0x14]
            test edx, edx
            je L004ede3d
            mov edi, edx
            or ecx, 0xffffffff
            xor eax, eax
            repne scasb
            not ecx
            dec ecx
            cmp ecx, 0xb
            jne L004ede37
            cmp byte ptr [edx + 5], 0x7c
            je L004ede76
    L004ede37:
            cmp byte ptr [edx + 1], 0xa3
            je L004ede76
    L004ede3d:
            test ebp, ebp
            je L004edf55
            mov edx, dword ptr [ebp + 0x14]
            test edx, edx
            je L004edf55
            cmp byte ptr [edx], 0x26
            jne L004edf55
            movsx eax, byte ptr [edx + 1]
            movsx ecx, byte ptr [edx + 2]
            lea eax, [eax + eax*4]
            lea ecx, [ecx + eax*2]
            mov eax, dword ptr [ecx*4 + gSmashable_triggers_by_identifier]
            test eax, eax
            je L004edf55
    L004ede76:
            cmp byte ptr [edx], 0x26
            jne L004edec6
            mov cl, byte ptr [edx + 1]
            cmp cl, 0x30
            jl L004edec6
            cmp cl, 0x39
            jg L004edec6
            cmp byte ptr [edx + 3], 0x21
            je L004ee209
            mov eax, dword ptr [esp + 0x10]
            mov eax, dword ptr [eax + 0x28]
            test eax, eax
            je L004edf18
            cmp eax, 3
            je L004edf18
            cmp eax, 6
            jne L004ee209
            mov eax, dword ptr [esp + 0x38]
            cmp cl, byte ptr [eax - 0x23]
            jne L004ee209
            mov cl, byte ptr [edx + 2]
            cmp cl, byte ptr [eax - 0x22]
            jne L004ee209
            jmp L004edf18
    L004edec6:
            mov ecx, dword ptr [esp + 0x10]
            mov eax, dword ptr [ecx + 0x28]
            test eax, eax
            je L004edf18
            cmp eax, 2
            je L004edf18
            cmp eax, 5
            jne L004ee209
            mov eax, ecx
            mov edi, edx
            lea ebp, [eax + 8]
    L004edee6:
            mov cl, byte ptr [edi]
            mov al, cl
            cmp cl, byte ptr [ebp]
            jne L004edf0b
            test al, al
            je L004edf07
            mov cl, byte ptr [edi + 1]
            mov al, cl
            cmp cl, byte ptr [ebp + 1]
            jne L004edf0b
            add edi, 2
            add ebp, 2
            test al, al
            jne L004edee6
    L004edf07:
            xor eax, eax
            jmp L004edf10
    L004edf0b:
            sbb eax, eax
            sbb eax, -1
    L004edf10:
            test eax, eax
            jne L004ee209
    L004edf18:
            mov eax, dword ptr [ebx]
            mov esi, 1
            mov ecx, dword ptr [eax + 0x50]
            mov dword ptr [esp + 0x24], ecx
            mov ecx, dword ptr [eax + 0x54]
            mov eax, dword ptr [eax + 0x58]
            mov dword ptr [esp + 0x28], ecx
            mov dword ptr [esp + 0x2c], eax
            xor eax, eax
            mov al, byte ptr [edx + 6]
            mov edx, dword ptr [gTrack_smashable_environment_specs]
            lea ecx, [eax + eax*2]
            shl ecx, 3
            sub ecx, eax
            shl ecx, 5
            lea eax, [ecx + edx - 0x2e0]
            mov dword ptr [esp + 0x20], eax
    L004edf55:
            cmp esi, -1
            je L004ee209
            mov eax, offset gSmash_explosions+0x88
            mov ecx, 1
    L004edf68:
            mov edx, dword ptr [eax - 0x84]
            test edx, edx
            je L004edfa6
            cmp dword ptr [eax - 0x88], ecx
            jne L004edfa6
            cmp esi, ecx
            jne L004edf86
            mov edx, dword ptr [eax]
            mov edi, dword ptr [ebx]
            cmp edx, edi
            je L004edfb4
    L004edf86:
            test esi, esi
            jne L004edfa6
            mov edx, dword ptr [eax]
            mov edi, dword ptr [ebx]
            cmp edx, edi
            jne L004edfa6
            mov dx, word ptr [eax + 4]
            cmp dx, word ptr [ebx + 4]
            jne L004edfa6
            mov dx, word ptr [eax + 6]
            cmp dx, word ptr [ebx + 6]
            je L004edfb4
    L004edfa6:
            add eax, 0xc8
            cmp eax, offset gPowerup_respawn_sound_ids+0x6c
            jl L004edf68
            jmp L004edfbc
    L004edfb4:
            mov dword ptr [esp + 0x20], 0
    L004edfbc:
            mov eax, dword ptr [esp + 0x20]
            test eax, eax
            je L004ee209
            mov ecx, dword ptr [esp + 0x30]
            mov eax, dword ptr [ebx]
            cmp eax, dword ptr [ecx]
            je L004ee209
            mov ebp, ecx
            mov edi, dword ptr [esp + 0x10]
            lea edx, [ebp + 0x68]
            mov eax, dword ptr [ebp + 0x68]
            mov dword ptr [esp + 0x44], eax
            mov ecx, dword ptr [edx + 4]
            mov dword ptr [esp + 0x48], ecx
            xor ecx, ecx
            mov edx, dword ptr [edx + 8]
            mov cx, word ptr [edi]
            mov dword ptr [esp + 0x4c], edx
            xor edx, edx
            mov dx, word ptr [edi + 2]
            call IRandomBetween
            mov edx, dword ptr [esp + 0x40]
            mov dword ptr [esp + 0xcc], esi
            add eax, edx
            mov dword ptr [esp + 0x5c], eax
            mov eax, dword ptr [edi + 0x44]
            dec eax
            je L004ee069
            dec eax
            jne L004ee0fc
            fld dword ptr [esp + 0x28]
            fld dword ptr [esp + 0x24]
            fsub dword ptr [ebp + 0xb0]
            fxch st(1)
            fsub dword ptr [ebp + 0xb4]
            fld dword ptr [esp + 0x2c]
            fsub dword ptr [ebp + 0xb8]
            fld st(2)
            fld st(2)
            fxch st(1)
            fmul st(0), st(4)
            fxch st(1)
            fmul st(0), st(3)
            fld st(2)
            fmul st(0), st(3)
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fsqrt
            fcom dword ptr [gFloat_0058aec4]
            fnstsw ax
            test ah, 0x41
            jne L004ee0dc
            jmp L004ee0ae
    L004ee069:
            fld dword ptr [ebp + 0xb4]
            fld dword ptr [ebp + 0xb0]
            fsub dword ptr [esp + 0x24]
            fxch st(1)
            fsub dword ptr [esp + 0x28]
            fld dword ptr [ebp + 0xb8]
            fsub dword ptr [esp + 0x2c]
            fld st(2)
            fld st(2)
            fxch st(1)
            fmul st(0), st(4)
            fxch st(1)
            fmul st(0), st(3)
            fld st(2)
            fmul st(0), st(3)
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fsqrt
            fcom dword ptr [gFloat_0058aec4]
            fnstsw ax
            test ah, 0x41
            jne L004ee0dc
    L004ee0ae:
            fdivr qword ptr [gDouble_0058aec8]
            fld st(0)
            fld st(1)
            fxch st(1)
            fmul st(0), st(5)
            fxch st(1)
            fmul st(0), st(4)
            fxch st(2)
            fmul st(0), st(3)
            fxch st(1)
            fstp dword ptr [esp + 0x14]
            fxch st(1)
            fstp dword ptr [esp + 0x18]
            fstp dword ptr [esp + 0x1c]
            fstp st(0)
            fstp st(0)
            fstp st(0)
            jmp L004ee0fc
    L004ee0dc:
            fstp st(0)
            fstp st(0)
            fstp st(0)
            fstp st(0)
            mov dword ptr [esp + 0x14], 0x3f800000
            mov dword ptr [esp + 0x18], 0
            mov dword ptr [esp + 0x1c], 0
    L004ee0fc:
            fld dword ptr [edi + 0x48]
            fcomp dword ptr [gFloat_0058aec0]
            fnstsw ax
            test ah, 1
            jne L004ee141
            fld dword ptr [edi + 0x48]
            fld dword ptr [edi + 0x48]
            fld dword ptr [edi + 0x48]
            fld dword ptr [edi + 0x48]
            fxch st(3)
            fmul dword ptr [esp + 0x14]
            fxch st(2)
            fmul dword ptr [esp + 0x18]
            fxch st(1)
            fmul dword ptr [esp + 0x1c]
            fxch st(2)
            fstp dword ptr [esp + 0x60]
            fstp dword ptr [esp + 0x64]
            fstp dword ptr [esp + 0x68]
            fstp dword ptr [esp + 0xd0]
            jmp L004ee18d
    L004ee141:
            fld dword ptr [esp + 0x7e28]
            fmul dword ptr [edi + 0x48]
            fld dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x18]
            fld dword ptr [esp + 0x1c]
            fxch st(3)
            fchs
            fstp dword ptr [esp + 0xd0]
            fxch st(1)
            fmul dword ptr [esp + 0xd0]
            fxch st(1)
            fmul dword ptr [esp + 0xd0]
            fxch st(2)
            fmul dword ptr [esp + 0xd0]
            fxch st(1)
            fstp dword ptr [esp + 0x60]
            fxch st(1)
            fstp dword ptr [esp + 0x64]
            fstp dword ptr [esp + 0x68]
    L004ee18d:
            mov ecx, dword ptr [esp + 0x28]
            mov eax, dword ptr [esp + 0x24]
            mov edx, dword ptr [esp + 0x2c]
            mov dword ptr [esp + 0x54], ecx
            mov dword ptr [esp + 0x50], eax
            mov eax, dword ptr [esp + 0x20]
            mov ecx, 0x14
            lea esi, [ebx - 0x48]
            lea edi, [esp + 0x7c]
            mov dword ptr [esp + 0x58], edx
            rep movsd
            mov dword ptr [esp + 0x78], eax
            mov dword ptr [esp + 0xd4], ebp
            xor eax, eax
            mov ecx, offset gSmash_explosions+0x4
    L004ee1c9:
            cmp dword ptr [ecx], 0
            je L004ee1df
            add ecx, 0xc8
            inc eax
            cmp ecx, offset gPowerup_respawn_smashable+0x4
            jl L004ee1c9
            jmp L004ee209
    L004ee1df:
            lea eax, [eax + eax*4]
            mov ecx, 0x25
            lea esi, [esp + 0x44]
            lea eax, [eax + eax*4]
            shl eax, 3
            mov dword ptr [eax + gSmash_explosions], 1
            lea edi, [eax + gSmash_explosions+0x8]
            mov dword ptr [eax + gSmash_explosions+0x4], ebp
            rep movsd
    L004ee209:
            mov eax, dword ptr [esp + 0x3c]
            add ebx, 0x50
            dec eax
            mov dword ptr [esp + 0x3c], eax
            jne L004edd07
            mov ebp, dword ptr [esp + 0x7e20]
            mov ebx, dword ptr [esp + 0x10]
    L004ee226:
            mov eax, dword ptr [esp + 0x34]
            mov ecx, dword ptr [ebp]
            inc eax
            add ebx, 0x4c
            cmp eax, ecx
            mov dword ptr [esp + 0x34], eax
            mov dword ptr [esp + 0x10], ebx
            jl L004edc1c
            pop esi
            pop edi
    L004ee243:
            pop ebp
            pop ebx
            add esp, 0x7e0c
            ret 0xc
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004f1030
void C2_NAKED C2_HOOK_FASTCALL FUN_004f1030(tPhysics_object* pObject, int pMode, br_actor* pActor) {
#ifndef CARPOCALYPSE2_MATCHING
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
#else
    __asm {
            push esi
            mov esi, dword ptr [ecx + 0x23c]
            push edi
            mov edi, edx
            cmp edi, 0xe
            jne L004f10b3
            test ecx, ecx
            je L004f10a4
            test esi, esi
            je L004f10a4
            cmp byte ptr [ecx + 0x238], 1
            jne L004f10a4
            mov eax, dword ptr [esi + 0xc]
            test eax, eax
            je L004f1061
            cmp eax, 2
            je L004f10a4
            cmp eax, 3
            je L004f10a4
    L004f1061:
            mov edx, dword ptr [gSize_powerup_queue]
            cmp edx, 0x32
            jge L004f10a4
            cmp dword ptr [esi + 0xc], 5
            jle L004f10a4
            mov esi, dword ptr [esp + 0xc]
            lea eax, [edx + edx*2]
            shl eax, 2
            inc edx
            mov dword ptr [eax + gPowerup_queue+0x8], esi
            mov ecx, dword ptr [ecx + 0x23c]
            mov dword ptr [eax + gPowerup_queue], ecx
            mov dword ptr [eax + gPowerup_queue+0x4], 0xe
            mov dword ptr [gSize_powerup_queue], edx
            pop edi
            pop esi
            ret 4
    L004f10a4:
            mov edx, 0xe
            call FUN_004d9d00
            pop edi
            pop esi
            ret 4
    L004f10b3:
            mov edx, dword ptr [gSize_powerup_queue]
            test ecx, ecx
            je L004f10e6
            test esi, esi
            je L004f10e6
            cmp byte ptr [ecx + 0x238], 1
            jne L004f10e6
            mov eax, dword ptr [esi + 0xc]
            test eax, eax
            je L004f10db
            cmp eax, 2
            je L004f10e6
            cmp eax, 3
            je L004f10e6
    L004f10db:
            cmp edx, 0x32
            jge L004f10e6
            cmp dword ptr [esi + 0xc], 8
            je L004f1110
    L004f10e6:
            cmp byte ptr [ecx + 0x238], 0x20
            jne L004f1137
            cmp dword ptr [gMutant_tail_state], 2
            je L004f1110
            call GetTotalTime
            sub eax, dword ptr [gINT_00705b70]
            cmp eax, 0x1d4c
            jae L004f1137
            mov edx, dword ptr [gSize_powerup_queue]
    L004f1110:
            mov ecx, dword ptr [esp + 0xc]
            lea eax, [edx + edx*2]
            shl eax, 2
            inc edx
            mov dword ptr [eax + gPowerup_queue+0x8], ecx
            mov dword ptr [eax + gPowerup_queue], 0x75bc2c
            mov dword ptr [eax + gPowerup_queue+0x4], edi
            mov dword ptr [gSize_powerup_queue], edx
    L004f1137:
            pop edi
            pop esi
            ret 4
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004ed490
void C2_NAKED C2_HOOK_FASTCALL SmashItIntoVerySmallPiecesIndeed(tPhysics_object* pObject, tSmashable_item_spec* pSpec, undefined4 pArg3, tConnotations* pConnotations, tU32 pSeed, undefined4 pArg6, const br_vector3* pPoint, const br_vector3* pSound_position, br_actor* pActor, tU32 pTime, undefined4 pArg11, br_matrix34* pTransform, br_bounds3* pBounds, br_vector3* pPosition, float pDistance, undefined4 pArg16, int pFlag) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;
    tCar_spec* owner;
    tSide_effects* side_effects;
    tSpecial_effects_spec* special_effects;
    tSmash_run_time_variable_change* variable_change;
    br_actor* actor;
    int value;

    if (pConnotations->count_sounds != 0) {
        DRS3StartSound3D(gXXX_outlet,
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
#else
    __asm {
            push ecx
            push ebx
            push ebp
            mov ebp, dword ptr [esp + 0x14]
            push esi
            mov esi, ecx
            push edi
            mov eax, dword ptr [ebp]
            mov ebx, edx
            test eax, eax
            mov dword ptr [esp + 0x10], esi
            je L004ed4e8
            push 0x10000
            mov edx, 0x12000
            mov ecx, 0xe000
            call IRandomBetween
            mov edx, dword ptr [ebp]
            xor ecx, ecx
            push eax
            mov eax, dword ptr [esp + 0x34]
            push 0xff
            push 1
            push offset gZero_v__car
            push eax
            dec edx
            call IRandomBetween
            mov edx, dword ptr [ebp + eax*4 + 4]
            mov ecx, dword ptr [gXXX_outlet]
            call DRS3StartSound3D
    L004ed4e8:
            mov eax, dword ptr [esp + 0x50]
            test eax, eax
            jne L004ed5a9
            mov eax, dword ptr [ebp + 0x25c]
            lea edi, [ebp + 0x25c]
            test eax, eax
            je L004ed510
            mov ecx, dword ptr [esp + 0x30]
            push esi
            mov edx, edi
            call QueueNonCarCuboidActivation
    L004ed510:
            mov ecx, dword ptr [edi + 8]
            lea eax, [edi + 8]
            test ecx, ecx
            je L004ed52e
            mov ecx, dword ptr [esp + 0x48]
            mov edx, dword ptr [esp + 0x4c]
            push ecx
            push edx
            push eax
            mov edx, esi
            mov ecx, ebx
            call FUN_004edbe0
    L004ed52e:
            mov edx, dword ptr [edi + 0x18]
            test edx, edx
            je L004ed5a9
            test esi, esi
            je L004ed5a9
            mov eax, dword ptr [esi + 0x23c]
            test eax, eax
            je L004ed5a9
            cmp byte ptr [esi + 0x238], 1
            jne L004ed5a9
            cmp dword ptr [eax + 0xc], 8
            jne L004ed5a9
            mov eax, dword ptr [esi]
            mov ecx, dword ptr [esp + 0x28]
            fld dword ptr [eax + 0x58]
            fld dword ptr [eax + 0x54]
            fsub dword ptr [ecx + 4]
            fxch st(1)
            fsub dword ptr [ecx + 8]
            fld dword ptr [eax + 0x50]
            _emit 0xd8
        _emit 0x21
            fxch st(2)
            fst dword ptr [esp + 0x1c]
            fld st(1)
            fxch st(1)
            fmul dword ptr [esp + 0x1c]
            fxch st(1)
            fmul st(0), st(2)
            fxch st(3)
            fst dword ptr [esp + 0x1c]
            fmul dword ptr [esp + 0x1c]
            fxch st(3)
            faddp st(1), st(0)
            fxch st(2)
            faddp st(2), st(0)
            fxch st(1)
            fcomp qword ptr [gDouble_0058af28]
            fnstsw ax
            test ah, 1
            fstp st(0)
            je L004ed5a9
            push 0
            mov ecx, esi
            call FUN_004f1030
    L004ed5a9:
            mov eax, dword ptr [ebp + 0x10]
            xor edi, edi
            test eax, eax
            jle L004ed61a
            mov ebx, dword ptr [esp + 0x44]
            add esi, 0xb0
            mov dword ptr [esp + 0x1c], esi
            lea esi, [ebp + 0x14]
    L004ed5c3:
            mov eax, dword ptr [esp + 0x50]
            mov ecx, dword ptr [esp + 0x1c]
            mov edx, dword ptr [esp + 0x48]
            push eax
            mov eax, dword ptr [esp + 0x44]
            push ecx
            mov ecx, dword ptr [esp + 0x44]
            push edx
            mov edx, dword ptr [esp + 0x44]
            push ebx
            push eax
            mov eax, dword ptr [esp + 0x48]
            push ecx
            mov ecx, dword ptr [esp + 0x48]
            push edx
            mov edx, dword ptr [esp + 0x48]
            push eax
            mov eax, dword ptr [esp + 0x48]
            push ecx
            mov ecx, dword ptr [esp + 0x48]
            push edx
            mov edx, dword ptr [esp + 0x48]
            push eax
            push ecx
            mov ecx, dword ptr [esp + 0x48]
            push edx
            mov edx, esi
            call WorkerShrapnelShower
            mov eax, dword ptr [ebp + 0x10]
            inc edi
            add esi, 0x58
            cmp edi, eax
            jl L004ed5c3
            mov esi, dword ptr [esp + 0x10]
    L004ed61a:
            mov eax, dword ptr [esp + 0x50]
            test eax, eax
            jne L004ed756
            test esi, esi
            je L004ed63d
            mov eax, dword ptr [esi + 0x23c]
            test eax, eax
            je L004ed63d
            cmp byte ptr [esi + 0x238], 1
            je L004ed63f
    L004ed63d:
            xor eax, eax
    L004ed63f:
            test eax, eax
            je L004ed663
            mov ecx, dword ptr [ebp + 0x278]
            cmp ecx, 2
            je L004ed65d
            cmp ecx, 1
            jne L004ed663
            mov ecx, dword ptr [ebp + 0x28c]
            test ecx, ecx
            jne L004ed663
    L004ed65d:
            cmp dword ptr [eax + 0xc], 8
            je L004ed670
    L004ed663:
            cmp dword ptr [ebp + 0x278], 3
            jne L004ed756
    L004ed670:
            fld dword ptr [ebp + 0x27c]
            fcomp dword ptr [gFloat_0058aec0]
            mov dword ptr [ebp + 0x28c], 1
            fnstsw ax
            test ah, 0x41
            jne L004ed6a1
            fld dword ptr [ebp + 0x27c]
            call _ftol
            mov ecx, eax
            call EarnCredits
            jmp L004ed6ce
    L004ed6a1:
            fld dword ptr [esp + 0x48]
            fmul dword ptr [ebp + 0x27c]
            fchs
            fcom dword ptr [gFloat_0058af30]
            fnstsw ax
            test ah, 1
            jne L004ed6c2
            fstp st(0)
            fld dword ptr [gFloat_0058af30]
    L004ed6c2:
            call _ftol
            mov ecx, eax
            call EarnCredits100
    L004ed6ce:
            fld dword ptr [ebp + 0x280]
            fcomp dword ptr [gFloat_0058aec0]
            fnstsw ax
            test ah, 0x41
            jne L004ed6f5
            fld dword ptr [ebp + 0x280]
            call _ftol
            mov ecx, eax
            call AwardTime
            jmp L004ed722
    L004ed6f5:
            fld dword ptr [esp + 0x48]
            fmul dword ptr [ebp + 0x280]
            fchs
            fcom dword ptr [gFloat_0058af34]
            fnstsw ax
            test ah, 1
            jne L004ed716
            fstp st(0)
            fld dword ptr [gFloat_0058af34]
    L004ed716:
            call _ftol
            mov ecx, eax
            call AwardTime5
    L004ed722:
            mov ecx, dword ptr [ebp + 0x284]
            test ecx, ecx
            je L004ed747
            push 0
            call GetMiscString
            xor edx, edx
            mov ecx, 4
            push eax
            push -4
            push 0xbb8
            call NewTextHeadupSlot2
    L004ed747:
            mov ecx, dword ptr [ebp + 0x288]
            test ecx, ecx
            je L004ed756
            call DoFancyHeadup
    L004ed756:
            mov eax, dword ptr [gYon_squared]
            lea esi, [ebp + 0x224]
            mov dword ptr [esp + 0x48], eax
            mov eax, dword ptr [gPed_676914]
            test eax, eax
            jne L004ed785
            mov ecx, dword ptr [esp + 0x48]
            mov edi, dword ptr [esp + 0x30]
            push ecx
            lea ecx, [edi + 0x50]
            call PointOutOfSight
            test eax, eax
            jne L004ed79e
            jmp L004ed789
    L004ed785:
            mov edi, dword ptr [esp + 0x30]
    L004ed789:
            cmp dword ptr [esi], 0
            je L004ed79e
            mov edx, dword ptr [esp + 0x40]
            push 0
            push edx
            mov edx, edi
            mov ecx, esi
            call Explode
    L004ed79e:
            mov eax, dword ptr [esi + 0x30]
            test eax, eax
            je L004ed7e2
            mov eax, dword ptr [esi + 0x34]
            mov ecx, dword ptr [esi + 0x30]
            mov edx, dword ptr [esi + 0x24]
            push eax
            mov eax, dword ptr [esi + 0x20]
            push ecx
            mov ecx, dword ptr [esi + 0x2c]
            push edx
            mov edx, dword ptr [esi + 0x28]
            push eax
            mov eax, dword ptr [esi + 0x1c]
            push ecx
            mov ecx, dword ptr [esi + 0x18]
            push edx
            mov edx, dword ptr [esi + 0x14]
            push eax
            mov eax, dword ptr [esi + 0x10]
            push ecx
            mov ecx, dword ptr [esi + 0xc]
            push edx
            mov edx, dword ptr [esi + 8]
            push eax
            mov eax, dword ptr [edi + 0x18]
            push ecx
            push edx
            push eax
            mov edx, edi
            xor ecx, ecx
            call QueueOilSpill2
    L004ed7e2:
            mov al, byte ptr [ebp + 0x290]
            xor edi, edi
            test al, al
            lea esi, [ebp + 0x292]
            jbe L004ed827
    L004ed7f4:
            movsx edx, word ptr [esi]
            cmp edx, 0x270f
            je L004ed80c
            xor ecx, ecx
            mov cx, word ptr [esi + 2]
            call AccumulateRuntimeVariable
            jmp L004ed817
    L004ed80c:
            xor ecx, ecx
            mov cx, word ptr [esi + 2]
            call ToggleRuntimeVariable
    L004ed817:
            xor ecx, ecx
            inc edi
            mov cl, byte ptr [ebp + 0x290]
            add esi, 4
            cmp edi, ecx
            jl L004ed7f4
    L004ed827:
            mov eax, dword ptr [gCurrent_race+0x98]
            cmp dword ptr [eax + 0x210], 4
            jl L004ed858
            mov ecx, dword ptr [eax + 0x214]
            call GetRuntimeVariable
            mov edx, dword ptr [gCurrent_race+0x98]
            cmp eax, dword ptr [edx + 0x218]
            jl L004ed858
            mov ecx, 3
            call RaceCompleted
    L004ed858:
            pop edi
            pop esi
            pop ebp
            pop ebx
            pop ecx
            ret 0x3c
    }
#endif
}

extern const double gDouble_0058aeb8;

// GLOBAL: CARMA2_HW 0x0058af18
const double gDouble_0058af18 = 0.09587379924285257;

// FUNCTION: CARMA2_HW 0x004ecfb0
void C2_NAKED C2_HOOK_FASTCALL FlushSmashQueue(int pFlush_powerups) {
#ifndef CARPOCALYPSE2_MATCHING
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
#else
    __asm {
            sub esp, 0x70
            mov edx, dword ptr [gCount_queued_smashes]
            push ebx
            push ebp
            push esi
            test edx, edx
            push edi
            mov dword ptr [esp + 0x4c], ecx
            mov dword ptr [esp + 0x30], 0
            jle L004ed25e
            mov ebp, offset gQueued_smashes+0x4c
    L004ecfd6:
            mov eax, dword ptr [ebp - 0x44]
            mov dword ptr [esp + 0x24], eax
            mov ecx, dword ptr [eax + 0x18]
            test ecx, ecx
            je L004ed1e7
            mov eax, dword ptr [ebp - 0xc]
            test eax, eax
            je L004ed025
            mov ecx, 0xc
            mov esi, ebp
            mov edi, offset gMatrix34_006b78a0
            lea eax, [ebp + 0x30]
            rep movsd
            mov ecx, dword ptr [eax]
            mov dword ptr [gINT_006a3334], 1
            mov dword ptr [gVector3_006a4688], ecx
            mov edx, dword ptr [eax + 4]
            mov dword ptr [gVector3_006a4688+0x4], edx
            mov eax, dword ptr [eax + 8]
            mov dword ptr [gVector3_006a4688+0x8], eax
            jmp L004ed02f
    L004ed025:
            mov dword ptr [gINT_006a3334], 0
    L004ed02f:
            mov ecx, dword ptr [ebp - 0x10]
            mov ebx, dword ptr [ebp - 0x14]
            mov edx, dword ptr [ebp - 4]
            mov eax, dword ptr [ebp - 0x48]
            mov edi, dword ptr [ebp - 0x3c]
            mov dword ptr [esp + 0x34], ecx
            mov ecx, dword ptr [ebp - 0x4c]
            lea esi, [ebp - 0x2c]
            mov dword ptr [esp + 0x28], ebx
            mov dword ptr [esp + 0x38], edx
            mov dword ptr [esp + 0x3c], eax
            mov dword ptr [esp + 0x2c], ecx
            call GetTotalTime
            mov ecx, dword ptr [edi + 0x48]
            mov edx, dword ptr [edi + 0x4c]
            mov dword ptr [esp + 0x20], eax
            mov dword ptr [esp + 0x14], edx
            lea eax, [ecx - 1]
            cmp edx, eax
            jge L004ed1e7
            cmp ebx, ecx
            jl L004ed080
            mov ebx, eax
            mov dword ptr [esp + 0x28], ebx
    L004ed080:
            mov edx, dword ptr [esp + 0x20]
            mov eax, dword ptr [esp + 0x24]
            mov ecx, edi
            xor ecx, edx
            mov edx, dword ptr [gUniverse_actor]
            mov dword ptr [esp + 0x44], ecx
            push edx
            lea ecx, [esp + 0x54]
            push eax
            push ecx
            call BrActorToActorMatrix34
            mov ecx, dword ptr [esp + 0x38]
            add esp, 0xc
            mov edx, edi
            push ebx
            call ActuallySmashCarPart
            mov edx, dword ptr [gCount_shrapnel_shower_calls]
            mov ecx, dword ptr [edi + 0x50]
            lea eax, [ebx + ebx*4]
            mov dword ptr [esp + 0x40], edx
            lea edx, [ebx + eax*2]
            shl edx, 4
            add edx, ebx
            lea eax, [ecx + edx*4 + 0x14]
            mov edx, dword ptr [esp + 0x14]
            mov dword ptr [esp + 0x48], eax
            lea eax, [edx + 1]
            lea edx, [eax + eax*4]
            lea edx, [eax + edx*2]
            shl edx, 4
            add edx, eax
            cmp eax, ebx
            lea ecx, [ecx + edx*4]
            jg L004ed1a9
            add ecx, 0x2bc
            mov dword ptr [esp + 0x10], ecx
            mov ecx, ebx
            sub ecx, eax
            inc ecx
            mov dword ptr [esp + 0x2c], ecx
    L004ed101:
            mov eax, dword ptr [esp + 0x10]
            test byte ptr [eax], 1
            je L004ed117
            mov eax, dword ptr [esp + 0x14]
            mov edx, ebx
            sub edx, eax
            cmp edx, 1
            jne L004ed190
    L004ed117:
            fld dword ptr [esi + 4]
            fld dword ptr [esi + 8]
            fld dword ptr [esi]
            fxch st(2)
            fst dword ptr [esp + 0x18]
            fxch st(1)
            fst dword ptr [esp + 0x1c]
            fxch st(1)
            fmul dword ptr [esp + 0x18]
            fxch st(1)
            fmul dword ptr [esp + 0x1c]
            fld st(2)
            fmul st(0), st(3)
            fxch st(1)
            faddp st(2), st(0)
            mov eax, dword ptr [esp + 0x34]
            mov edx, dword ptr [esp + 0x38]
            push eax
            push esi
            faddp st(1), st(0)
            push ecx
            lea eax, [esp + 0x5c]
            mov ecx, dword ptr [esp + 0x48]
            lea ebx, [ebp - 0x38]
            fsqrt
            fstp st(1)
            fstp dword ptr [esp]
            push 0
            push 0
            push eax
            mov eax, dword ptr [esp + 0x58]
            push eax
            mov eax, dword ptr [esp + 0x3c]
            push eax
            mov eax, dword ptr [esp + 0x44]
            push eax
            lea eax, [ebp - 0x20]
            push eax
            push eax
            mov eax, dword ptr [esp + 0x70]
            push ebx
            push eax
            mov eax, dword ptr [esp + 0x7c]
            push eax
            push edi
            call SmashItIntoVerySmallPiecesIndeed
            mov ecx, dword ptr [esp + 0x2c]
            mov ebx, dword ptr [esp + 0x28]
    L004ed190:
            mov edx, dword ptr [esp + 0x10]
            add edx, 0x2c4
            dec ecx
            mov dword ptr [esp + 0x10], edx
            mov dword ptr [esp + 0x2c], ecx
            jne L004ed101
    L004ed1a9:
            mov eax, dword ptr [edi + 0x44]
            test eax, eax
            je L004ed1e7
            mov ecx, dword ptr [eax + 4]
            test ecx, ecx
            je L004ed1e7
            mov eax, dword ptr [edi + 0x4c]
            lea edx, [eax + eax*4]
            lea edx, [eax + edx*2]
            shl edx, 4
            add edx, eax
            mov eax, dword ptr [edi + 0x50]
            mov eax, dword ptr [eax + edx*4 + 0x2c0]
            sub eax, 0
            je L004ed1e4
            dec eax
            je L004ed1df
            dec eax
            jne L004ed1e7
            mov byte ptr [ecx], 0x3f
            jmp L004ed1e7
    L004ed1df:
            mov byte ptr [ecx], 0x3e
            jmp L004ed1e7
    L004ed1e4:
            mov byte ptr [ecx], 0x58
    L004ed1e7:
            mov eax, dword ptr [esp + 0x30]
            mov edx, dword ptr [gCount_queued_smashes]
            inc eax
            add ebp, 0x88
            cmp eax, edx
            mov dword ptr [esp + 0x30], eax
            jl L004ecfd6
            test edx, edx
            jle L004ed25e
            mov edi, 1
            mov esi, offset gQueued_smashes+0x44
    L004ed212:
            cmp dword ptr [esi], 0
            je L004ed250
            mov ecx, dword ptr [esi - 0x3c]
            mov edx, 0x7fff
            mov ebx, dword ptr [ecx + 0x18]
            mov ecx, ebx
            call DRModelUpdateAndKevificateMaterials
            mov edx, dword ptr [gCount_queued_smashes]
            cmp edi, edx
            jge L004ed250
            mov eax, edx
            lea ecx, [esi + 0x4c]
            sub eax, edi
    L004ed23a:
            mov ebp, dword ptr [ecx]
            cmp ebx, dword ptr [ebp + 0x18]
            jne L004ed247
            mov dword ptr [esi], 0
    L004ed247:
            add ecx, 0x88
            dec eax
            jne L004ed23a
    L004ed250:
            inc edi
            add esi, 0x88
            lea eax, [edi - 1]
            cmp eax, edx
            jl L004ed212
    L004ed25e:
            mov eax, dword ptr [esp + 0x4c]
            xor ebx, ebx
            cmp eax, ebx
            jne L004ed29f
            mov eax, dword ptr [gSize_powerup_queue]
            xor edi, edi
            cmp eax, ebx
            jle L004ed299
            mov esi, offset gPowerup_queue+0x4
    L004ed278:
            mov edx, dword ptr [esi]
            mov ecx, dword ptr [esi - 4]
            call GotPowerup
            mov edx, dword ptr [esi + 4]
            mov ecx, dword ptr [esi]
            call MayQueuePowerupRespawn
            mov eax, dword ptr [gSize_powerup_queue]
            inc edi
            add esi, 0xc
            cmp edi, eax
            jl L004ed278
    L004ed299:
            mov dword ptr [gSize_powerup_queue], ebx
    L004ed29f:
            pop edi
            pop esi
            mov dword ptr [gCount_queued_smashes], ebx
            pop ebp
            pop ebx
            add esp, 0x70
            ret
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004ed2b0
void C2_NAKED C2_HOOK_FASTCALL ActuallySmashCarPart(tCar_spec* pCar_spec, tCar_crush_smashable_part* pSmashable, int pLevel) {
#ifndef CARPOCALYPSE2_MATCHING
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
#else
    __asm {
            push ecx
            push ebx
            push ebp
            xor ebp, ebp
            push esi
            cmp ecx, ebp
            push edi
            mov esi, edx
            mov dword ptr [esp + 0x10], ecx
            je L004ed2e3
            xor ecx, ecx
            mov eax, offset gRepair_animations
    L004ed2c8:
            cmp dword ptr [eax], esi
            je L004ed2d9
            add eax, 0xc
            inc ecx
            cmp eax, offset gRepair_animations+0x12c
            jl L004ed2c8
            jmp L004ed2e3
    L004ed2d9:
            lea eax, [ecx + ecx*2]
            mov dword ptr [eax*4 + gRepair_animations], ebp
    L004ed2e3:
            mov edi, dword ptr [esp + 0x18]
            mov edx, dword ptr [esi + 0x50]
            lea ecx, [edi + edi*4]
            lea ebx, [edi + ecx*2]
            shl ebx, 4
            add ebx, edi
            shl ebx, 2
            mov eax, dword ptr [ebx + edx + 0xc]
            cmp eax, ebp
            je L004ed31d
            lea edx, [eax - 1]
            xor ecx, ecx
            call IRandomBetween
            lea ecx, [edi + edi*4]
            lea edx, [edi + ecx*2]
            mov ecx, dword ptr [esi + 0x50]
            shl edx, 4
            add edx, edi
            add eax, edx
            mov ebp, dword ptr [ecx + eax*4]
    L004ed31d:
            mov edx, dword ptr [esi + 0x44]
            mov ecx, dword ptr [esp + 0x10]
            push ebp
            call PipeSingleSmashTextureChange
            mov edx, dword ptr [esi + 0x44]
            mov eax, dword ptr [esp + 0x10]
            test eax, eax
            mov dword ptr [edx + 0x94], ebp
            je L004ed345
            mov ecx, dword ptr [eax + 0x1960]
            test ecx, ecx
            jne L004ed34b
    L004ed345:
            mov ecx, dword ptr [esi + 0x44]
            mov dword ptr [ecx + 0x40], ebp
    L004ed34b:
            mov edx, dword ptr [esi + 0x50]
            mov dword ptr [esi + 0x4c], edi
            test eax, eax
            mov ebx, dword ptr [ebx + edx + 0x2bc]
            je L004ed375
            test bl, 0x1c
            jne L004ed375
            mov eax, dword ptr [esi + 0x44]
            add eax, 0x24
            push eax
            call BrMatrix23Identity
            add esp, 4
            jmp L004ed445
    L004ed375:
            test bl, 4
            je L004ed397
            mov ecx, 0x32
            call PercentageChance
            test eax, eax
            je L004ed397
            mov eax, dword ptr [esi + 0x44]
            fld dword ptr [eax + 0x24]
            fmul dword ptr [gFLOAT_0058af20]
            fstp dword ptr [eax + 0x24]
    L004ed397:
            test bl, 8
            je L004ed3b9
            mov ecx, 0x32
            call PercentageChance
            test eax, eax
            je L004ed3b9
            mov eax, dword ptr [esi + 0x44]
            fld dword ptr [eax + 0x30]
            fmul dword ptr [gFLOAT_0058af20]
            fstp dword ptr [eax + 0x30]
    L004ed3b9:
            test bl, 0x10
            je L004ed445
            mov edx, 3
            xor ecx, ecx
            call IRandomBetween
            sub eax, 0
            je L004ed427
            dec eax
            je L004ed400
            dec eax
            jne L004ed445
            mov eax, dword ptr [esi + 0x44]
            fld dword ptr [eax + 0x24]
            fld dword ptr [eax + 0x28]
            fchs
            fstp dword ptr [eax + 0x24]
            mov ecx, dword ptr [esi + 0x44]
            fchs
            fstp dword ptr [ecx + 0x28]
            mov eax, dword ptr [esi + 0x44]
            mov edx, dword ptr [eax + 0x28]
            fld dword ptr [eax + 0x2c]
            mov dword ptr [eax + 0x2c], edx
            mov eax, dword ptr [esi + 0x44]
            jmp L004ed442
    L004ed400:
            mov eax, dword ptr [esi + 0x44]
            mov ecx, dword ptr [eax + 0x28]
            fld dword ptr [eax + 0x24]
            mov dword ptr [eax + 0x24], ecx
            mov edx, dword ptr [esi + 0x44]
            fstp dword ptr [edx + 0x28]
            mov eax, dword ptr [esi + 0x44]
            fld dword ptr [eax + 0x2c]
            fld dword ptr [eax + 0x28]
            fchs
            fstp dword ptr [eax + 0x2c]
            mov eax, dword ptr [esi + 0x44]
            fchs
            jmp L004ed442
    L004ed427:
            mov eax, dword ptr [esi + 0x44]
            fld dword ptr [eax + 0x24]
            fmul dword ptr [gFLOAT_0058af20]
            fstp dword ptr [eax + 0x24]
            mov eax, dword ptr [esi + 0x44]
            fld dword ptr [eax + 0x30]
            fmul dword ptr [gFLOAT_0058af20]
    L004ed442:
            fstp dword ptr [eax + 0x30]
    L004ed445:
            mov ecx, dword ptr [esi + 0x44]
            push 0x7fff
            push ecx
            call BrMaterialUpdate
            mov ecx, dword ptr [esp + 0x18]
            add esp, 8
            cmp ecx, offset gProgram_state+0xac
            jne L004ed475
            cmp dword ptr [gAction_replay_camera_mode], 8
            jne L004ed475
            mov edx, 1
            call MungeCarMaterials
    L004ed475:
            mov ecx, dword ptr [esi + 0x40]
            test ecx, ecx
            jl L004ed481
            call DisableFunkotronic
    L004ed481:
            pop edi
            pop esi
            pop ebp
            pop ebx
            pop ecx
            ret 4
    }
#endif
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
void C2_NAKED C2_HOOK_FASTCALL MungeDelayedSideEffects(void) {
#ifndef CARPOCALYPSE2_MATCHING
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
#else
    __asm {
            sub esp, 0x28
            push ebx
            push ebp
            push esi
            push edi
            call GetTotalTime
            mov esi, eax
            mov edi, offset gSmash_explosions+0x88
            mov dword ptr [esp + 0x1c], esi
    L004ecc97:
            mov eax, dword ptr [edi - 0x84]
            xor edx, edx
            cmp eax, edx
            je L004ece81
            mov eax, dword ptr [edi - 0x88]
            sub eax, edx
            je L004ecd38
            dec eax
            jne L004ece81
            cmp esi, dword ptr [edi - 0x68]
            jb L004ece81
            cmp dword ptr [edi + 8], 1
            jne L004ecd13
            mov ecx, dword ptr [edi]
            mov eax, dword ptr [ecx + 0x14]
            cmp byte ptr [eax], 0x26
            jne L004ecd13
            mov al, byte ptr [eax + 1]
            cmp al, 0x30
            jl L004ecd13
            cmp al, 0x39
            jg L004ecd13
            call DoPullActorFromWorld
            mov eax, dword ptr [edi]
            lea edx, [edi - 0x64]
            push 0
            push 1
            mov ecx, dword ptr [eax + 0x14]
            lea eax, [edi - 0x80]
            push edx
            push eax
            mov byte ptr [ecx + 3], 0x21
            mov edx, dword ptr [edi + 0xc]
            lea ecx, [edi - 0x74]
            push ecx
            mov ecx, dword ptr [edi + 0x10]
            push edx
            mov edx, dword ptr [edi]
            call BashObject
            fstp st(0)
            jmp L004ece77
    L004ecd13:
            push edx
            lea eax, [edi - 0x64]
            push 1
            lea ecx, [edi - 0x80]
            push eax
            mov eax, dword ptr [edi + 0xc]
            lea edx, [edi - 0x74]
            push ecx
            mov ecx, dword ptr [edi + 0x10]
            push edx
            push eax
            lea edx, [edi - 0x48]
            call SmashEnvironment
            fstp st(0)
            jmp L004ece77
    L004ecd38:
            mov ecx, dword ptr [edi - 0x64]
            lea eax, [edi - 0x60]
            cmp ecx, edx
            lea ebx, [edi - 0x80]
            mov dword ptr [esp + 0x14], edx
            mov dword ptr [esp + 0x10], eax
            mov dword ptr [esp + 0x18], edx
            mov ebp, eax
            jle L004ece64
    L004ecd57:
            cmp esi, dword ptr [ebp + 4]
            jb L004ece3b
            mov ecx, dword ptr [ebp]
            mov edx, dword ptr [ecx + 0x5c]
            test edx, edx
            jne L004ece2c
            call DoPullActorFromWorld
            mov esi, eax
            test esi, esi
            je L004ece28
            test dword ptr [esi + 0x100], 0x10000
            je L004ecd95
            mov ecx, esi
            call MakeLiftGoUp
            jmp L004ece1e
    L004ecd95:
            mov edx, dword ptr [ebp]
            mov ecx, dword ptr [ebx + 0x18]
            add edx, 0x50
            lea eax, [ebx + 0xc]
            push edx
            push eax
            push offset gZero_vector__smash
            lea edx, [esp + 0x38]
            push 0x3f800000
            lea eax, [esp + 0x30]
            push edx
            push eax
            mov edx, ebx
            call ApplyInitialMovement
            fld dword ptr [esp + 0x20]
            fmul qword ptr [gDouble_0058aeb8]
            mov ecx, dword ptr [esi + 8]
            fstp dword ptr [ecx + 0x68]
            fld dword ptr [esp + 0x24]
            fmul qword ptr [gDouble_0058aeb8]
            mov edx, dword ptr [esi + 8]
            fstp dword ptr [edx + 0x6c]
            fld dword ptr [esp + 0x28]
            fmul qword ptr [gDouble_0058aeb8]
            mov eax, dword ptr [esi + 8]
            fstp dword ptr [eax + 0x70]
            fld dword ptr [esp + 0x2c]
            fmul qword ptr [gDouble_0058af18]
            mov ecx, dword ptr [esi + 8]
            fstp dword ptr [ecx + 0x74]
            fld dword ptr [esp + 0x30]
            fmul qword ptr [gDouble_0058af18]
            mov edx, dword ptr [esi + 8]
            fstp dword ptr [edx + 0x78]
            fld dword ptr [esp + 0x34]
            fmul qword ptr [gDouble_0058af18]
            mov eax, dword ptr [esi + 8]
            fstp dword ptr [eax + 0x7c]
    L004ece1e:
            mov ecx, dword ptr [esi + 8]
            mov byte ptr [ecx + 0xec], 0
    L004ece28:
            mov eax, dword ptr [esp + 0x10]
    L004ece2c:
            mov ecx, dword ptr [esp + 0x14]
            mov esi, dword ptr [esp + 0x1c]
            inc ecx
            mov dword ptr [esp + 0x14], ecx
            jmp L004ece4d
    L004ece3b:
            mov edx, dword ptr [ebp]
            mov dword ptr [eax], edx
            mov ecx, dword ptr [ebp + 4]
            mov dword ptr [eax + 4], ecx
            add eax, 8
            mov dword ptr [esp + 0x10], eax
    L004ece4d:
            mov ecx, dword ptr [esp + 0x18]
            mov edx, dword ptr [edi - 0x64]
            inc ecx
            add ebp, 8
            cmp ecx, edx
            mov dword ptr [esp + 0x18], ecx
            jl L004ecd57
    L004ece64:
            mov edx, dword ptr [esp + 0x14]
            mov ecx, dword ptr [ebx + 0x1c]
            sub ecx, edx
            mov dword ptr [ebx + 0x1c], ecx
            mov eax, dword ptr [edi - 0x64]
            test eax, eax
            jne L004ece81
    L004ece77:
            mov dword ptr [edi - 0x84], 0
    L004ece81:
            add edi, 0xc8
            cmp edi, offset gPowerup_respawn_sound_ids+0x6c
            jl L004ecc97
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x28
            ret
    }
#endif
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
void C2_NAKED C2_HOOK_FASTCALL DoPowerupRespawnSmash(br_actor *pActor) {
#ifndef CARPOCALYPSE2_MATCHING
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
#else
    __asm {
            push ecx
            mov eax, dword ptr [gPowerup_respawn_sound_count]
            push esi
            test eax, eax
            push edi
            mov esi, ecx
            je L004ecef5
            push 0x10000
            mov edx, 0x12000
            mov ecx, 0xe000
            call IRandomBetween
            mov ecx, dword ptr [gPowerup_respawn_sound_count]
            push eax
            push 0xff
            push 1
            lea eax, [esi + 0x50]
            push offset gZero_v__car
            lea edx, [ecx - 1]
            push eax
            xor ecx, ecx
            call IRandomBetween
            mov edx, dword ptr [eax*4 + gPowerup_respawn_sound_ids]
            mov ecx, dword ptr [gXXX_outlet]
            call DRS3StartSound3D
    L004ecef5:
            mov edi, dword ptr [esi + 0x18]
            mov eax, dword ptr [gPed_676914]
            mov edx, dword ptr [gYon_squared]
            add edi, 0x34
            test eax, eax
            mov dword ptr [esp + 8], edx
            jne L004ecf1d
            mov eax, edx
            lea ecx, [esi + 0x50]
            push eax
            call PointOutOfSight
            test eax, eax
            jne L004ecf35
    L004ecf1d:
            mov eax, dword ptr [gPowerup_respawn_explosion]
            test eax, eax
            je L004ecf35
            push 0
            push edi
            mov edx, esi
            mov ecx, offset gPowerup_respawn_explosion
            call Explode
    L004ecf35:
            mov eax, dword ptr [gPowerup_respawn_material]
            test eax, eax
            je L004ecf96
            mov ecx, dword ptr [gPowerup_respawn_field_0x50]
            mov edx, dword ptr [gPowerup_respawn_field_0x4c]
            push ecx
            mov ecx, dword ptr [gPowerup_respawn_field_0x44]
            push eax
            mov eax, dword ptr [gPowerup_respawn_field_0x48]
            push edx
            mov edx, dword ptr [gPowerup_respawn_field_0x40]
            push eax
            mov eax, dword ptr [gPowerup_respawn_size]
            push ecx
            mov ecx, dword ptr [gPowerup_respawn_grow_size_max]
            push edx
            mov edx, dword ptr [gPowerup_respawn_grow_size_min]
            push eax
            mov eax, dword ptr [gPowerup_respawn_full_size_max]
            push ecx
            mov ecx, dword ptr [gPowerup_respawn_full_size_min]
            push edx
            mov edx, dword ptr [gPowerup_respawn_spill_time_offset]
            push eax
            mov eax, dword ptr [esi + 0x18]
            push ecx
            push edx
            push eax
            mov edx, esi
            xor ecx, ecx
            call QueueOilSpill2
    L004ecf96:
            pop edi
            pop esi
            pop ecx
            ret
            nop
            nop
            nop
            nop
            nop
            nop
    }
#endif
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
            je L004f6514
            mov eax, dword ptr [gCount_track_smashable_environment_specs]
            xor esi, esi
            test eax, eax
            jle L004f6514
            mov edx, dword ptr [gTrack_smashable_environment_specs]
            lea eax, [edx + 0x10]
    L004f64f4:
            mov edx, dword ptr [eax]
            test edx, edx
            je L004f6504
            cmp edx, ecx
            je L004f6504
            mov dword ptr [eax], 0
    L004f6504:
            mov edx, dword ptr [gCount_track_smashable_environment_specs]
            inc esi
            add eax, 0x2e0
            cmp esi, edx
            jl L004f64f4
    L004f6514:
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
void C2_NAKED C2_HOOK_FASTCALL ApplyInitialMovement(undefined4* pArg1, br_vector3* pArg2, br_vector3* pArg3, br_vector3* pArg4, float pArg5, br_vector3* pArg6, br_vector3* pArg7, br_vector3* pArg8) {
#ifndef CARPOCALYPSE2_MATCHING
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
#else
    __asm {
            sub esp, 0xc
            push ebx
            push ebp
            push esi
            push edi
            mov edi, ecx
            mov ebx, dword ptr [esp + 0x20]
            mov ecx, dword ptr [esp + 0x30]
            fld dword ptr [edi + 8]
            fmul dword ptr [edx]
            test ecx, ecx
            fstp dword ptr [ebx]
            fld dword ptr [edx + 4]
            fmul dword ptr [edi + 8]
            fstp dword ptr [ebx + 4]
            fld dword ptr [edx + 8]
            fmul dword ptr [edi + 8]
            fstp dword ptr [ebx + 8]
            je L004eb29c
            mov edx, dword ptr [esp + 0x34]
            test edx, edx
            je L004eb29c
            fld dword ptr [edi]
            fst dword ptr [esp + 0x30]
            fcomp dword ptr [gFloat_0058aec0]
            fnstsw ax
            test ah, 0x40
            jne L004eb29c
            fld dword ptr [edi + 4]
            fst dword ptr [esp + 0x20]
            fcomp dword ptr [gFloat_0058aec0]
            fnstsw ax
            test ah, 0x40
            jne L004eb29c
            fld dword ptr [ecx + 4]
            fld dword ptr [ecx]
            fsub dword ptr [edx]
            fxch st(1)
            fsub dword ptr [edx + 4]
            fld dword ptr [ecx + 8]
            fsub dword ptr [edx + 8]
            fxch st(2)
            fst dword ptr [esp + 0x10]
            fxch st(1)
            fst dword ptr [esp + 0x14]
            fxch st(1)
            fmul dword ptr [esp + 0x10]
            fxch st(2)
            fstp dword ptr [esp + 0x18]
            mov eax, dword ptr [esp + 0x20]
            mov ecx, dword ptr [esp + 0x30]
            fmul dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x18]
            fmul dword ptr [esp + 0x18]
            fxch st(1)
            faddp st(2), st(0)
            push eax
            push ecx
            faddp st(1), st(0)
            fsqrt
            fstp dword ptr [esp + 0x28]
            call FRandomBetween
            mov edx, dword ptr [edi + 4]
            mov eax, dword ptr [edi]
            fdiv dword ptr [esp + 0x20]
            push edx
            push eax
            fmul dword ptr [esp + 0x18]
            fstp dword ptr [esp + 0x18]
            call FRandomBetween
            mov ecx, dword ptr [edi + 4]
            mov edx, dword ptr [edi]
            fdiv dword ptr [esp + 0x20]
            push ecx
            push edx
            fmul dword ptr [esp + 0x1c]
            fstp dword ptr [esp + 0x1c]
            call FRandomBetween
            fdiv dword ptr [esp + 0x20]
            fmul dword ptr [esp + 0x18]
            fld dword ptr [ebx]
            fld dword ptr [esp + 0x14]
            fxch st(2)
            fst dword ptr [esp + 0x18]
            fxch st(1)
            fadd dword ptr [esp + 0x10]
            fxch st(2)
            fadd dword ptr [ebx + 4]
            fxch st(1)
            fadd dword ptr [ebx + 8]
            fxch st(2)
            fstp dword ptr [ebx]
            fstp dword ptr [ebx + 4]
            fstp dword ptr [ebx + 8]
    L004eb29c:
            mov ebp, dword ptr [esp + 0x24]
            mov esi, ebx
            sub ebp, ebx
            mov dword ptr [esp + 0x20], 3
    L004eb2ac:
            mov eax, dword ptr [edi + 0xc]
            push eax
            call FRandomPosNeg
            fadd dword ptr [esi]
            push 0x3f800000
            push 0
            fmul dword ptr [esp + 0x30]
            fstp dword ptr [esi]
            call FRandomBetween
            fstp dword ptr [ebp + esi]
            mov eax, dword ptr [esp + 0x20]
            add esi, 4
            dec eax
            mov dword ptr [esp + 0x20], eax
            jne L004eb2ac
            mov ecx, dword ptr [edi + 0x10]
            xor ebp, ebp
            push ecx
            push ebp
            call FRandomBetween
            fadd dword ptr [ebx + 4]
            mov ecx, dword ptr [esp + 0x2c]
            fstp dword ptr [ebx + 4]
            fld dword ptr [ecx + 4]
            fld dword ptr [ecx + 8]
            fld dword ptr [ecx]
            fxch st(2)
            fst dword ptr [esp + 0x20]
            fxch st(1)
            fst dword ptr [esp + 0x30]
            fxch st(1)
            fmul dword ptr [esp + 0x20]
            fxch st(1)
            fmul dword ptr [esp + 0x30]
            fld st(2)
            fmul st(0), st(3)
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fsqrt
            fstp st(1)
            fcom dword ptr [gFloat_0058aec4]
            fnstsw ax
            test ah, 0x41
            jne L004eb35e
            fdivr qword ptr [gDouble_0058aec8]
            fld dword ptr [ecx + 4]
            fld dword ptr [ecx + 8]
            fld dword ptr [ecx]
            fxch st(2)
            fxch st(1)
            fxch st(2)
            fmul st(0), st(3)
            fxch st(1)
            fmul st(0), st(3)
            fxch st(2)
            fmul st(0), st(3)
            fxch st(1)
            fstp dword ptr [esp + 0x10]
            fxch st(1)
            fstp dword ptr [esp + 0x14]
            fstp dword ptr [esp + 0x18]
            fstp st(0)
            jmp L004eb378
    L004eb35e:
            fstp st(0)
            mov dword ptr [esp + 0x10], 0x3f800000
            mov dword ptr [esp + 0x14], 0
            mov dword ptr [esp + 0x18], 0
    L004eb378:
            mov edx, dword ptr [edi + 0x14]
            push edx
            push ebp
            call FRandomBetween
            mov eax, dword ptr [edi + 0x14]
            fmul dword ptr [esp + 0x10]
            push eax
            push ebp
            fstp dword ptr [esp + 0x18]
            call FRandomBetween
            mov ecx, dword ptr [edi + 0x14]
            fmul dword ptr [esp + 0x14]
            push ecx
            push ebp
            fstp dword ptr [esp + 0x1c]
            call FRandomBetween
            fmul dword ptr [esp + 0x18]
            fld dword ptr [ebx]
            fld dword ptr [esp + 0x14]
            fxch st(2)
            fst dword ptr [esp + 0x18]
            fxch st(1)
            fadd dword ptr [esp + 0x10]
            fxch st(2)
            fadd dword ptr [ebx + 4]
            fxch st(1)
            fadd dword ptr [ebx + 8]
            fxch st(2)
            fstp dword ptr [ebx]
            mov esi, dword ptr [esp + 0x24]
            fstp dword ptr [ebx + 4]
            fstp dword ptr [ebx + 8]
            fld dword ptr [esi + 4]
            fld dword ptr [esi + 8]
            fld dword ptr [esi]
            fxch st(2)
            fst dword ptr [esp + 0x24]
            fxch st(1)
            fst dword ptr [esp + 0x20]
            fxch st(1)
            fmul dword ptr [esp + 0x24]
            fxch st(1)
            fmul dword ptr [esp + 0x20]
            fld st(2)
            fmul st(0), st(3)
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fsqrt
            fstp st(1)
            fcom dword ptr [gFloat_0058aec4]
            fnstsw ax
            test ah, 0x41
            jne L004eb43d
            fdivr qword ptr [gDouble_0058aec8]
            fld dword ptr [esi + 4]
            fld dword ptr [esi + 8]
            fld dword ptr [esi]
            fxch st(2)
            fxch st(1)
            fxch st(2)
            fmul st(0), st(3)
            fxch st(1)
            fmul st(0), st(3)
            fxch st(2)
            fmul st(0), st(3)
            fxch st(1)
            fstp dword ptr [esi]
            fxch st(1)
            fstp dword ptr [esi + 4]
            fstp dword ptr [esi + 8]
            fstp st(0)
            jmp L004eb44b
    L004eb43d:
            fstp st(0)
            mov dword ptr [esi], 0x3f800000
            mov dword ptr [esi + 4], ebp
            mov dword ptr [esi + 8], ebp
    L004eb44b:
            fld dword ptr [esp + 0x28]
            fmul dword ptr [edi + 0x18]
            push ecx
            fstp dword ptr [esp]
            call FRandomPosNeg
            fmul dword ptr [esi]
            fld dword ptr [esp + 0x28]
            fxch st(1)
            fstp dword ptr [esi]
            push ecx
            fmul dword ptr [edi + 0x18]
            fstp dword ptr [esp]
            call FRandomPosNeg
            fmul dword ptr [esi + 4]
            fld dword ptr [esp + 0x28]
            fxch st(1)
            fstp dword ptr [esi + 4]
            push ecx
            fmul dword ptr [edi + 0x18]
            fstp dword ptr [esp]
            call FRandomPosNeg
            fmul dword ptr [esi + 8]
            pop edi
            fstp dword ptr [esi + 8]
            pop esi
            pop ebp
            pop ebx
            add esp, 0xc
            ret 0x18
            nop
            nop
            nop
            nop
            nop
            nop
            nop
    }
#endif
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

// FUNCTION: CARMA2_HW 0x004ec370
void C2_HOOK_FASTCALL CreateSmashedGlassFragments(tU32 pSeed, br_vector3* pDirection,
        br_actor* pSource_actor, tU32 pTime, int pFace_count, const tU16* pSource_faces,
        int pVertex_count, const tU16* pSource_vertices, tU32 pOwner, tU32 pFlags,
        float pLongest_edge_squared, tSmashable_initial_speed_spec* pInitial_speed,
C2_NAKED         const int* pLifetime_range, tU32 pFragment_owner, br_matrix34* pSource_transform) {
#ifndef CARPOCALYPSE2_MATCHING
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
#else
    __asm {
            mov eax, 0x3ed8
            call _chkstk
            mov dword ptr [esp + 0x50], edx
            push ebp
            push esi
            mov esi, dword ptr [esp + 0x3eec]
            push edi
            test esi, esi
            mov edi, ecx
            je L004ecc2e
            mov eax, dword ptr [esp + 0x3ef4]
            test eax, eax
            je L004ecc2e
            mov ebp, dword ptr [esp + 0x3ef8]
            test ebp, ebp
            je L004ecc2e
            mov eax, dword ptr [esp + 0x3efc]
            test eax, eax
            je L004ecc2e
            push ebx
            call rand
            push edi
            mov dword ptr [esp + 0x68], eax
            call srand
            add esp, 4
            mov ecx, 0x7d0
            xor eax, eax
            lea edi, [esp + 0x68]
            test ebp, ebp
            rep stosd
            jle L004ec42e
            mov eax, dword ptr [esp + 0x3eec]
            mov edx, dword ptr [esp + 0x3f00]
            lea ebx, [esp + 0x68]
            mov ecx, dword ptr [eax + 0x18]
            mov dword ptr [esp + 0x1c], ecx
    L004ec3fc:
            mov ecx, dword ptr [esp + 0x1c]
            xor eax, eax
            mov ax, word ptr [edx]
            mov edi, ebx
            mov ecx, dword ptr [ecx + 8]
            add edx, 2
            lea eax, [eax + eax*4]
            add ebx, 0x28
            dec ebp
            lea esi, [ecx + eax*8]
            mov ecx, 0xa
            rep movsd
            jne L004ec3fc
            mov esi, dword ptr [esp + 0x3ef4]
            mov ebp, dword ptr [esp + 0x3efc]
    L004ec42e:
            mov ecx, 0x7d0
            xor eax, eax
            lea edi, [esp + 0x1fa8]
            mov ebx, ebp
            rep stosd
            mov ecx, dword ptr [esp + 0x3ef8]
            mov dword ptr [esp + 0x34], ebx
            test esi, esi
            jle L004ec487
            lea eax, [esp + 0x1fb8]
            mov edx, esi
    L004ec458:
            mov esi, ecx
            lea edi, [eax - 0x10]
            add eax, 0x28
            add ecx, 6
            mov ebp, dword ptr [esi]
            dec edx
            mov dword ptr [edi], ebp
            mov si, word ptr [esi + 4]
            mov word ptr [edi + 4], si
            mov esi, dword ptr [esp + 0x3f04]
            mov dword ptr [eax - 0x30], esi
            mov byte ptr [eax - 0x28], 0
            jne L004ec458
            mov esi, dword ptr [esp + 0x3ef4]
    L004ec487:
            mov eax, esi
            lea edx, [esp + 0x1fa8]
            test esi, esi
            mov dword ptr [esp + 0x20], eax
            mov dword ptr [esp + 0x30], 0
            mov dword ptr [esp + 0x50], edx
            jle L004ec833
    L004ec4a8:
            cmp eax, 0xc7
            jge L004ec833
            cmp ebx, 0xc7
            jge L004ec833
            lea ecx, [eax + eax*4]
            mov dword ptr [esp + 0x1c], 0
            lea edx, [esp + ecx*8 + 0x1fa8]
            lea ecx, [ebx + ebx*4]
            mov dword ptr [esp + 0x4c], edx
            lea edi, [esp + ecx*8 + 0x68]
            jmp L004ec4ea
    L004ec4de:
            mov edi, dword ptr [esp + 0x10]
            mov ebx, dword ptr [esp + 0x34]
            mov eax, dword ptr [esp + 0x20]
    L004ec4ea:
            cmp eax, 0xc7
            jge L004ec817
            cmp ebx, 0xc7
            jge L004ec817
            mov ecx, dword ptr [esp + 0x50]
            lea edx, [esp + 0x38]
            mov esi, 3
    L004ec50e:
            xor eax, eax
            add edx, 4
            mov ax, word ptr [ecx]
            add ecx, 2
            dec esi
            lea eax, [eax + eax*4]
            lea eax, [esp + eax*8 + 0x68]
            mov dword ptr [edx - 4], eax
            jne L004ec50e
            mov eax, dword ptr [esp + 0x38]
            mov ecx, dword ptr [esp + 0x3c]
            mov edx, dword ptr [esp + 0x40]
            fld dword ptr [eax + 8]
            fld dword ptr [eax + 4]
            fsub dword ptr [ecx + 4]
            fxch st(1)
            fsub dword ptr [ecx + 8]
            _emit 0xd9
        _emit 0x00
            _emit 0xd8
        _emit 0x21
            fxch st(2)
            fst dword ptr [esp + 0x10]
            fld st(1)
            fxch st(1)
            fmul dword ptr [esp + 0x10]
            fld dword ptr [eax + 4]
            fxch st(2)
            fmul st(0), st(3)
            fld dword ptr [eax + 8]
            fxch st(5)
            fst dword ptr [esp + 0x10]
            fmul dword ptr [esp + 0x10]
            fld dword ptr [edx + 4]
            fxch st(2)
            faddp st(3), st(0)
            fxch st(3)
            fsub dword ptr [edx + 4]
            fxch st(5)
            fsub dword ptr [edx + 8]
            fxch st(3)
            faddp st(2), st(0)
            fxch st(4)
            fst dword ptr [esp + 0x10]
            fxch st(2)
            fst dword ptr [esp + 0x18]
            fxch st(2)
            fmul dword ptr [esp + 0x10]
            fxch st(2)
            fmul dword ptr [esp + 0x18]
            fxch st(1)
            fst dword ptr [esp + 0x18]
            fcomp dword ptr [esp + 0x3f0c]
            faddp st(1), st(0)
            _emit 0xd9
        _emit 0x00
            _emit 0xd8
        _emit 0x22
            fnstsw ax
            fst dword ptr [esp + 0x10]
            fmul dword ptr [esp + 0x10]
            fxch st(3)
            fsub dword ptr [ecx + 4]
            fxch st(3)
            faddp st(1), st(0)
            fld dword ptr [edx + 8]
            fsub dword ptr [ecx + 8]
            fxch st(3)
            fst dword ptr [esp + 0x10]
            fxch st(3)
            fst dword ptr [esp + 0x14]
            fxch st(3)
            fmul dword ptr [esp + 0x10]
            fxch st(3)
            fmul dword ptr [esp + 0x14]
            fxch st(1)
            fstp dword ptr [esp + 0x10]
            _emit 0xd9
        _emit 0x02
            _emit 0xd8
        _emit 0x21
            fxch st(1)
            faddp st(3), st(0)
            test ah, 1
            fst dword ptr [esp + 0x14]
            fmul dword ptr [esp + 0x14]
            faddp st(2), st(0)
            fxch st(1)
            fstp dword ptr [esp + 0x14]
            fstp st(0)
            je L004ec62f
            fld dword ptr [esp + 0x10]
            fcomp dword ptr [esp + 0x3f0c]
            fnstsw ax
            test ah, 1
            je L004ec62f
            fld dword ptr [esp + 0x14]
            fcomp dword ptr [esp + 0x3f0c]
            fnstsw ax
            test ah, 1
            je L004ec62f
            xor ecx, ecx
            call PercentageChance
            test eax, eax
            je L004ec813
    L004ec62f:
            fld dword ptr [esp + 0x18]
            fcomp dword ptr [esp + 0x10]
            fnstsw ax
            test ah, 1
            je L004ec665
            fld dword ptr [esp + 0x18]
            fcomp dword ptr [esp + 0x14]
            fnstsw ax
            test ah, 1
            je L004ec665
            mov edx, 1
            xor ecx, ecx
            call IRandomBetween
            lea ecx, [eax + eax*2]
            lea esi, [ecx*4 + gSplit_triangle_vertex_orders+0x30]
            jmp L004ec6b1
    L004ec665:
            fld dword ptr [esp + 0x14]
            fcomp dword ptr [esp + 0x10]
            fnstsw ax
            test ah, 1
            je L004ec69b
            fld dword ptr [esp + 0x14]
            fcomp dword ptr [esp + 0x18]
            fnstsw ax
            test ah, 1
            je L004ec69b
            mov edx, 1
            xor ecx, ecx
            call IRandomBetween
            lea edx, [eax + eax*2]
            lea esi, [edx*4 + gSplit_triangle_vertex_orders]
            jmp L004ec6b1
    L004ec69b:
            mov edx, 1
            xor ecx, ecx
            call IRandomBetween
            lea eax, [eax + eax*2]
            lea esi, [eax*4 + gSplit_triangle_vertex_orders+0x18]
    L004ec6b1:
            mov ecx, dword ptr [esi + 4]
            mov edx, dword ptr [esi]
            mov ebp, edi
            add edi, 0x28
            inc ebx
            mov dword ptr [esp + 0x48], edi
            inc ebx
            add edi, 0x28
            push 0x3f4ccccd
            mov dword ptr [esp + 0x48], ebp
            mov dword ptr [esp + 0x38], ebx
            mov ebx, dword ptr [esp + ecx*4 + 0x3c]
            mov dword ptr [esp + 0x14], edi
            mov edi, dword ptr [esp + edx*4 + 0x3c]
            push 0x3e4ccccd
            call FRandomBetween
            mov ecx, ebx
            mov edx, ebp
            sub ecx, edi
            mov eax, edi
            sub edx, edi
            mov ebx, 3
    L004ec6f6:
            fld dword ptr [ecx + eax]
            _emit 0xd8
        _emit 0x20
            add eax, 4
            dec ebx
            fmul st(0), st(1)
            fadd dword ptr [eax - 4]
            fstp dword ptr [edx + eax - 4]
            jne L004ec6f6
            lea eax, [edi + 0xc]
            mov edi, 2
    L004ec712:
            fld dword ptr [eax + ecx]
            _emit 0xd8
        _emit 0x20
            add eax, 4
            dec edi
            fmul st(0), st(1)
            fadd dword ptr [eax - 4]
            fstp dword ptr [eax + edx - 4]
            jne L004ec712
            mov eax, dword ptr [esi + 8]
            mov ecx, dword ptr [esi]
            push 0x3f4ccccd
            push 0x3e4ccccd
            mov ebx, dword ptr [esp + eax*4 + 0x40]
            mov edi, dword ptr [esp + ecx*4 + 0x40]
            fstp st(0)
            call FRandomBetween
            mov ecx, ebx
            mov eax, edi
            mov edx, dword ptr [esp + 0x48]
            sub ecx, edi
            sub edx, edi
            mov ebx, 3
    L004ec755:
            fld dword ptr [ecx + eax]
            _emit 0xd8
        _emit 0x20
            add eax, 4
            dec ebx
            fmul st(0), st(1)
            fadd dword ptr [eax - 4]
            fstp dword ptr [edx + eax - 4]
            jne L004ec755
            lea eax, [edi + 0xc]
            mov edi, 2
    L004ec771:
            fld dword ptr [eax + ecx]
            _emit 0xd8
        _emit 0x20
            add eax, 4
            dec edi
            fmul st(0), st(1)
            fadd dword ptr [eax - 4]
            fstp dword ptr [eax + edx - 4]
            jne L004ec771
            mov edx, dword ptr [esp + 0x4c]
            mov ebp, dword ptr [esp + 0x3f04]
            mov ebx, dword ptr [esp + 0x50]
            mov eax, edx
            mov edi, dword ptr [esp + 0x20]
            add edx, 0x28
            mov ecx, edx
            add edx, 0x28
            mov dword ptr [esp + 0x4c], edx
            mov dword ptr [eax + 8], ebp
            mov byte ptr [eax + 0x10], 0
            mov edx, dword ptr [esi + 4]
            inc edi
            mov dx, word ptr [ebx + edx*2]
            inc edi
            mov word ptr [eax], dx
            mov edx, dword ptr [esp + 0x34]
            mov dword ptr [esp + 0x20], edi
            lea edi, [edx - 2]
            dec edx
            mov word ptr [eax + 2], di
            mov word ptr [eax + 4], dx
            mov dword ptr [ecx + 8], ebp
            mov byte ptr [ecx + 0x10], 0
            mov eax, dword ptr [esi + 4]
            fstp st(0)
            mov ax, word ptr [ebx + eax*2]
            mov word ptr [ecx], ax
            mov eax, dword ptr [esi + 8]
            mov ax, word ptr [ebx + eax*2]
            mov word ptr [ecx + 4], dx
            mov word ptr [ecx + 2], ax
            mov ecx, dword ptr [esi + 4]
            mov word ptr [ebx + ecx*2], di
            mov eax, dword ptr [esi + 8]
            mov word ptr [ebx + eax*2], dx
            mov eax, dword ptr [esp + 0x1c]
            inc eax
            cmp eax, 4
            mov dword ptr [esp + 0x1c], eax
            jl L004ec4de
            mov ebx, dword ptr [esp + 0x34]
    L004ec813:
            mov eax, dword ptr [esp + 0x20]
    L004ec817:
            mov ecx, dword ptr [esp + 0x30]
            mov esi, dword ptr [esp + 0x50]
            inc ecx
            add esi, 0x28
            cmp ecx, eax
            mov dword ptr [esp + 0x30], ecx
            mov dword ptr [esp + 0x50], esi
            jl L004ec4a8
    L004ec833:
            test ebx, ebx
            mov dword ptr [esp + 0x38], 0
            mov dword ptr [esp + 0x3c], 0
            mov dword ptr [esp + 0x40], 0
            lea esi, [esp + 0x68]
            jle L004ec8c7
            mov edi, dword ptr [esp + 0x3f08]
            mov ebp, ebx
            mov ebx, dword ptr [esp + 0x3f1c]
            and edi, 1
    L004ec866:
            push ebx
            lea ecx, [esp + 0x58]
            push esi
            push ecx
            call BrMatrix34ApplyP
            mov eax, dword ptr [esp + 0x60]
            mov ecx, dword ptr [esp + 0x64]
            mov edx, esi
            add esp, 0xc
            test edi, edi
            mov dword ptr [edx], eax
            mov eax, dword ptr [esp + 0x5c]
            mov dword ptr [edx + 4], ecx
            mov dword ptr [edx + 8], eax
            je L004ec8bd
            fld dword ptr [esp + 0x38]
            fld dword ptr [esp + 0x3c]
            fld dword ptr [esp + 0x40]
            fxch st(2)
            fadd dword ptr [esp + 0x54]
            fxch st(1)
            fadd dword ptr [esp + 0x58]
            fxch st(2)
            fadd dword ptr [esp + 0x5c]
            fxch st(1)
            fstp dword ptr [esp + 0x38]
            fxch st(1)
            fstp dword ptr [esp + 0x3c]
            fstp dword ptr [esp + 0x40]
    L004ec8bd:
            add esi, 0x28
            dec ebp
            jne L004ec866
            mov eax, dword ptr [esp + 0x20]
    L004ec8c7:
            fild dword ptr [esp + 0x34]
            fld dword ptr [esp + 0x38]
            fld dword ptr [esp + 0x3c]
            fld dword ptr [esp + 0x40]
            fxch st(2)
            fdiv st(0), st(3)
            fxch st(1)
            test eax, eax
            mov dword ptr [esp + 0x30], 0
            fdiv st(0), st(3)
            fxch st(2)
            fdiv st(0), st(3)
            fxch st(1)
            fstp dword ptr [esp + 0x38]
            fxch st(1)
            fstp dword ptr [esp + 0x3c]
            fstp dword ptr [esp + 0x40]
            fstp st(0)
            jle L004ecc20
            lea ebx, [esp + 0x1faa]
    L004ec90c:
            mov edx, dword ptr [esp + 0x3f18]
            push 3
            xor ecx, ecx
            call FindShrapnelFragment
            mov esi, eax
            test esi, esi
            je L004ecc20
            mov ecx, dword ptr [esi]
            xor eax, eax
            mov ax, word ptr [ebx - 2]
            mov edx, dword ptr [ecx + 0x18]
            lea eax, [eax + eax*4]
            mov ecx, dword ptr [edx + 8]
            shl eax, 3
            mov edx, dword ptr [esp + eax + 0x68]
            mov dword ptr [ecx], edx
            mov ecx, dword ptr [esi]
            mov edx, dword ptr [ecx + 0x18]
            mov ecx, dword ptr [edx + 8]
            mov edx, dword ptr [esp + eax + 0x6c]
            mov dword ptr [ecx + 4], edx
            mov ecx, dword ptr [esi]
            mov edx, dword ptr [ecx + 0x18]
            mov ecx, dword ptr [edx + 8]
            mov edx, dword ptr [esp + eax + 0x70]
            mov dword ptr [ecx + 8], edx
            mov ecx, dword ptr [esi]
            mov edx, dword ptr [ecx + 0x18]
            mov ecx, dword ptr [edx + 8]
            mov edx, dword ptr [ecx + 4]
            _emit 0xd9
        _emit 0x01
            mov dword ptr [esp + 0x28], edx
            mov edx, dword ptr [ecx + 8]
            mov dword ptr [esp + 0x2c], edx
            mov edx, dword ptr [esp + eax + 0x74]
            mov dword ptr [ecx + 0xc], edx
            mov ecx, dword ptr [esi]
            mov edx, dword ptr [ecx + 0x18]
            mov ecx, dword ptr [edx + 8]
            mov edx, dword ptr [esp + eax + 0x78]
            mov dword ptr [ecx + 0x10], edx
            mov eax, dword ptr [esi]
            mov ecx, dword ptr [eax + 0x18]
            mov eax, dword ptr [ebx + 6]
            mov edx, dword ptr [ecx + 0xc]
            mov dword ptr [edx + 8], eax
            mov ecx, dword ptr [esi]
            xor eax, eax
            mov ax, word ptr [ebx]
            mov edx, dword ptr [ecx + 0x18]
            mov ecx, dword ptr [edx + 8]
            lea eax, [eax + eax*4]
            shl eax, 3
            mov edx, dword ptr [esp + eax + 0x68]
            mov dword ptr [ecx + 0x28], edx
            mov ecx, dword ptr [esi]
            mov edx, dword ptr [ecx + 0x18]
            mov ecx, dword ptr [edx + 8]
            mov edx, dword ptr [esp + eax + 0x6c]
            mov dword ptr [ecx + 0x2c], edx
            mov ecx, dword ptr [esi]
            mov edx, dword ptr [ecx + 0x18]
            mov ecx, dword ptr [edx + 8]
            mov edx, dword ptr [esp + eax + 0x70]
            mov dword ptr [ecx + 0x30], edx
            mov ecx, dword ptr [esi]
            mov edx, dword ptr [ecx + 0x18]
            mov ecx, dword ptr [edx + 8]
            mov edx, dword ptr [esp + eax + 0x74]
            fadd dword ptr [ecx + 0x28]
            fld dword ptr [ecx + 0x2c]
            fadd dword ptr [esp + 0x28]
            fstp dword ptr [esp + 0x28]
            fld dword ptr [ecx + 0x30]
            mov dword ptr [ecx + 0x34], edx
            mov ecx, dword ptr [esi]
            fadd dword ptr [esp + 0x2c]
            mov edx, dword ptr [ecx + 0x18]
            mov ecx, dword ptr [edx + 8]
            mov edx, dword ptr [esp + eax + 0x78]
            fstp dword ptr [esp + 0x2c]
            xor eax, eax
            mov dword ptr [ecx + 0x38], edx
            mov ax, word ptr [ebx + 2]
            mov ecx, dword ptr [esi]
            mov edx, dword ptr [ecx + 0x18]
            lea eax, [eax + eax*4]
            shl eax, 3
            mov ecx, dword ptr [edx + 8]
            mov edx, dword ptr [esp + eax + 0x68]
            mov dword ptr [ecx + 0x50], edx
            mov ecx, dword ptr [esi]
            mov edx, dword ptr [ecx + 0x18]
            mov ecx, dword ptr [edx + 8]
            mov edx, dword ptr [esp + eax + 0x6c]
            mov dword ptr [ecx + 0x54], edx
            mov ecx, dword ptr [esi]
            mov edx, dword ptr [ecx + 0x18]
            mov ecx, dword ptr [edx + 8]
            mov edx, dword ptr [esp + eax + 0x70]
            mov dword ptr [ecx + 0x58], edx
            mov ecx, dword ptr [esi]
            mov edx, dword ptr [ecx + 0x18]
            mov ecx, dword ptr [edx + 8]
            mov edx, dword ptr [esp + eax + 0x74]
            fadd dword ptr [ecx + 0x50]
            fld dword ptr [ecx + 0x54]
            fadd dword ptr [esp + 0x28]
            fstp dword ptr [esp + 0x28]
            fld dword ptr [ecx + 0x58]
            mov dword ptr [ecx + 0x5c], edx
            mov ecx, dword ptr [esi]
            fadd dword ptr [esp + 0x2c]
            mov edx, dword ptr [ecx + 0x18]
            mov ecx, dword ptr [edx + 8]
            mov edx, dword ptr [esp + eax + 0x78]
            fstp dword ptr [esp + 0x2c]
            mov dword ptr [ecx + 0x60], edx
            mov eax, dword ptr [esi]
            xor ecx, ecx
            xor edx, edx
            mov eax, dword ptr [eax + 0x18]
            mov cx, word ptr [eax + 0x10]
            mov dword ptr [esp + 0x4c], ecx
            fild dword ptr [esp + 0x4c]
            test ecx, ecx
            fstp dword ptr [esp + 0x10]
            fdiv dword ptr [esp + 0x10]
            fstp dword ptr [esp + 0x24]
            fld dword ptr [esp + 0x28]
            fdiv dword ptr [esp + 0x10]
            fstp dword ptr [esp + 0x28]
            fld dword ptr [esp + 0x2c]
            fdiv dword ptr [esp + 0x10]
            fstp dword ptr [esp + 0x2c]
            jle L004ecb04
            xor ecx, ecx
    L004ecab8:
            mov eax, dword ptr [eax + 8]
            xor edi, edi
            add eax, ecx
            inc edx
            _emit 0xd9
        _emit 0x00
            fsub dword ptr [esp + 0x24]
            _emit 0xd9
        _emit 0x18
            mov eax, dword ptr [esi]
            mov eax, dword ptr [eax + 0x18]
            mov eax, dword ptr [eax + 8]
            fld dword ptr [eax + ecx + 4]
            fsub dword ptr [esp + 0x28]
            lea eax, [eax + ecx + 4]
            _emit 0xd9
        _emit 0x18
            mov eax, dword ptr [esi]
            mov eax, dword ptr [eax + 0x18]
            mov eax, dword ptr [eax + 8]
            fld dword ptr [eax + ecx + 8]
            fsub dword ptr [esp + 0x2c]
            lea eax, [eax + ecx + 8]
            add ecx, 0x28
            _emit 0xd9
        _emit 0x18
            mov eax, dword ptr [esi]
            mov eax, dword ptr [eax + 0x18]
            mov di, word ptr [eax + 0x10]
            cmp edx, edi
            jl L004ecab8
    L004ecb04:
            mov ecx, dword ptr [esi]
            add ecx, 0x2c
            push ecx
            call BrMatrix34Identity
            mov ebp, dword ptr [esp + 0x3f0c]
            add esp, 4
            and ebp, 1
            xor edi, edi
    L004ecb1e:
            test ebp, ebp
            je L004ecb38
            push 0x3d23d70a
            call FRandomPosNeg
            fadd dword ptr [esp + edi + 0x38]
            mov edx, dword ptr [esi]
            fstp dword ptr [edx + edi + 0x50]
            jmp L004ecb4c
    L004ecb38:
            push 0x3c75c28f
            call FRandomPosNeg
            fadd dword ptr [esp + edi + 0x24]
            mov eax, dword ptr [esi]
            fstp dword ptr [eax + edi + 0x50]
    L004ecb4c:
            add edi, 4
            cmp edi, 0xc
            jl L004ecb1e
            mov ecx, dword ptr [esi]
            push 0x15
            mov edx, dword ptr [ecx + 0x18]
            push edx
            call BrModelUpdate
            mov eax, dword ptr [esi]
            add esp, 8
            fld dword ptr [gFloat_0058aec0]
            mov ecx, dword ptr [eax + 0x18]
            mov edx, 3
            lea eax, [ecx + 0x34]
    L004ecb77:
            fld dword ptr [eax + 0xc]
            _emit 0xd8
        _emit 0x20
            add eax, 4
            dec edx
            faddp st(1), st(0)
            jne L004ecb77
            fcom qword ptr [gDOUBLE_0058aef8]
            fnstsw ax
            test ah, 1
            je L004ecba3
            fsubr qword ptr [gDOUBLE_0058aef8+0x8]
            fmul qword ptr [gDOUBLE_0058aef8+0x10]
            fstp dword ptr [esp + 0x1c]
            jmp L004ecbad
    L004ecba3:
            fstp st(0)
            mov dword ptr [esp + 0x1c], 0x3f000000
    L004ecbad:
            mov ecx, dword ptr [ecx + 0x4c]
            push 0
            push 0
            mov edx, dword ptr [ecx + 0x18]
            mov ecx, dword ptr [esp + 0x24]
            mov eax, dword ptr [edx + 4]
            lea edx, [esi + 0x28]
            add eax, 0xc
            push eax
            push ecx
            mov ecx, dword ptr [esp + 0x3f20]
            lea eax, [esi + 0x1c]
            push edx
            mov edx, dword ptr [esp + 0x74]
            push eax
            call ApplyInitialMovement
            mov edi, dword ptr [esp + 0x3ef0]
            mov eax, dword ptr [esp + 0x3f14]
            mov dword ptr [esi + 0xc], edi
            mov dword ptr [esi + 0x14], edi
            mov edx, dword ptr [eax + 4]
            mov ecx, edi
            add ecx, edx
            mov dword ptr [esi + 8], ecx
            mov edx, dword ptr [eax + 4]
            mov ecx, dword ptr [eax]
            call IRandomBetween
            mov ecx, dword ptr [esp + 0x20]
            add eax, edi
            mov dword ptr [esi + 0x10], eax
            mov eax, dword ptr [esp + 0x30]
            inc eax
            add ebx, 0x28
            cmp eax, ecx
            mov dword ptr [esp + 0x30], eax
            jl L004ec90c
    L004ecc20:
            mov edx, dword ptr [esp + 0x64]
            push edx
            call srand
            add esp, 4
            pop ebx
    L004ecc2e:
            pop edi
            pop esi
            pop ebp
            add esp, 0x3ed8
            ret 0x34
    }
#endif
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
void C2_NAKED C2_HOOK_FASTCALL FUN_004eb6a0(br_matrix34* pMat) {
#ifndef CARPOCALYPSE2_MATCHING
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
#else
    __asm {
            sub esp, 0x10
            push esi
            push edi
            mov esi, ecx
            xor edi, edi
            push 0x3f800000
            mov dword ptr [esi + 0x24], edi
            mov dword ptr [esi + 0x28], edi
            mov dword ptr [esi + 0x2c], edi
            call FRandomPosNeg
            fstp dword ptr [esi + 0xc]
            push 0x3f800000
            call FRandomPosNeg
            fstp dword ptr [esi + 0x10]
            push 0x3f800000
            call FRandomPosNeg
            fstp dword ptr [esi + 0x14]
            push 0x3f800000
            call FRandomPosNeg
            fstp dword ptr [esi + 0x18]
            push 0x3f800000
            call FRandomPosNeg
            fstp dword ptr [esi + 0x1c]
            push 0x3f800000
            call FRandomPosNeg
            fld dword ptr [esi + 0x18]
            fld dword ptr [esi + 0x1c]
            fxch st(2)
            fst dword ptr [esp + 8]
            fxch st(1)
            fst dword ptr [esp + 0x10]
            fxch st(2)
            fst dword ptr [esp + 0xc]
            fxch st(2)
            fmul dword ptr [esp + 0x10]
            fxch st(2)
            fmul dword ptr [esp + 0xc]
            fxch st(1)
            fst dword ptr [esp + 0x10]
            fmul dword ptr [esp + 0x10]
            fxch st(1)
            faddp st(2), st(0)
            fld dword ptr [esp + 8]
            fxch st(1)
            faddp st(2), st(0)
            fstp dword ptr [esi + 0x20]
            fsqrt
            fcom dword ptr [gFloat_0058aec4]
            fnstsw ax
            test ah, 0x41
            jne L004eb77a
            fdivr qword ptr [gDouble_0058aec8]
            fld dword ptr [esi + 0x18]
            fld dword ptr [esi + 0x1c]
            fld dword ptr [esp + 8]
            fxch st(2)
            fxch st(1)
            fxch st(1)
            fmul st(0), st(3)
            fxch st(1)
            fmul st(0), st(3)
            fxch st(2)
            fmul st(0), st(3)
            fxch st(1)
            fstp dword ptr [esi + 0x18]
            fxch st(1)
            fstp dword ptr [esi + 0x1c]
            fstp dword ptr [esi + 0x20]
            fstp st(0)
            jmp L004eb789
    L004eb77a:
            fstp st(0)
            mov dword ptr [esi + 0x18], 0x3f800000
            mov dword ptr [esi + 0x1c], edi
            mov dword ptr [esi + 0x20], edi
    L004eb789:
            fld dword ptr [esi + 0x18]
            fld dword ptr [esi + 0x14]
            fld dword ptr [esi + 0xc]
            fld dword ptr [esi + 0x20]
            fld dword ptr [esi + 0xc]
            fld dword ptr [esi + 0x10]
            fmul dword ptr [esi + 0x20]
            fld dword ptr [esi + 0x1c]
            fmul dword ptr [esi + 0x14]
            fsubp st(1), st(0)
            fstp dword ptr [esp + 8]
            fld dword ptr [esi + 0x1c]
            fxch st(4)
            fmulp st(5), st(0)
            fld dword ptr [esi + 0x18]
            fxch st(2)
            fmulp st(3), st(0)
            fxch st(2)
            fsubp st(4), st(0)
            fxch st(3)
            fst dword ptr [esp + 0xc]
            fst dword ptr [esp + 0x10]
            fmul dword ptr [esp + 0x10]
            fld dword ptr [esi + 0x10]
            fxch st(3)
            fmulp st(2), st(0)
            fxch st(2)
            fmulp st(3), st(0)
            fld dword ptr [esp + 8]
            fxch st(3)
            fsubp st(1), st(0)
            fxch st(2)
            fmul dword ptr [esp + 8]
            fxch st(2)
            fst dword ptr [esp + 0x10]
            fst dword ptr [esp + 0x14]
            fmul dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x10]
            fxch st(1)
            faddp st(2), st(0)
            mov eax, dword ptr [esp + 8]
            mov ecx, dword ptr [esp + 0xc]
            mov dword ptr [esi], eax
            mov dword ptr [esi + 4], ecx
            fstp dword ptr [esi + 8]
            fxch st(1)
            faddp st(1), st(0)
            fsqrt
            fcom dword ptr [gFloat_0058aec4]
            fnstsw ax
            test ah, 0x41
            jne L004eb846
            fdivr qword ptr [gDouble_0058aec8]
            fld st(0)
            fld st(1)
            fxch st(1)
            fmul dword ptr [esp + 8]
            fxch st(1)
            fmul dword ptr [esp + 0xc]
            fxch st(2)
            fmul dword ptr [esp + 0x10]
            fxch st(1)
            fstp dword ptr [esi]
            fxch st(1)
            fstp dword ptr [esi + 4]
            fstp dword ptr [esi + 8]
            jmp L004eb854
    L004eb846:
            fstp st(0)
            mov dword ptr [esi], 0x3f800000
            mov dword ptr [esi + 4], edi
            mov dword ptr [esi + 8], edi
    L004eb854:
            fld dword ptr [esi + 8]
            fld dword ptr [esi + 0x1c]
            fld dword ptr [esi + 4]
            fld dword ptr [esi + 0x20]
            fld dword ptr [esi + 0x20]
            fld dword ptr [esi + 8]
            fxch st(4)
            fmulp st(5), st(0)
            fld dword ptr [esi + 0x18]
            fxch st(2)
            fmulp st(3), st(0)
            fxch st(2)
            fsubp st(4), st(0)
            fxch st(1)
            fmul dword ptr [esi]
            fxch st(3)
            fstp dword ptr [esi + 0xc]
            fld dword ptr [esi + 0xc]
            fld st(0)
            fmul st(0), st(1)
            fld dword ptr [esi + 4]
            fld dword ptr [esi + 0x18]
            fxch st(4)
            fmulp st(5), st(0)
            fxch st(4)
            fsubp st(5), st(0)
            fxch st(2)
            fmulp st(3), st(0)
            fxch st(3)
            fstp dword ptr [esi + 0x10]
            fld dword ptr [esi + 0x10]
            fst dword ptr [esp + 0x14]
            fmul dword ptr [esp + 0x14]
            fld dword ptr [esi + 0x1c]
            fmul dword ptr [esi]
            fxch st(1)
            faddp st(2), st(0)
            fsubp st(2), st(0)
            fxch st(1)
            fst dword ptr [esp + 0x10]
            fst dword ptr [esp + 0x14]
            fmul dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x10]
            fxch st(1)
            faddp st(2), st(0)
            fstp dword ptr [esi + 0x14]
            fcomp qword ptr [gDouble_0058aed8]
            fnstsw ax
            test ah, 1
            fstp st(0)
            je L004eb8e3
            push esi
            call BrMatrix34Identity
            add esp, 4
    L004eb8e3:
            pop edi
            pop esi
            add esp, 0x10
            ret
            nop
            nop
            nop
            nop
            nop
            nop
            nop
    }
#endif
}
#pragma inline_depth()

// FUNCTION: CARMA2_HW 0x004eb4a0
float C2_NAKED C2_HOOK_FASTCALL PositionFragment(br_actor* pActor, tSmashable_initial_position_spec* pInitial_position, br_actor* pSource_actor, br_vector3* pPosition4, br_vector3* pPosition5, br_actor* pRotation_actor, float* pRandom_ranges, br_matrix34* pPost_matrix) {
#ifndef CARPOCALYPSE2_MATCHING
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
#else
    __asm {
            sub esp, 0x1c
            push ebx
            push ebp
            push esi
            push edi
            mov edi, edx
            mov ebx, ecx
            mov dword ptr [esp + 0x10], 0x3f800000
            mov eax, dword ptr [edi]
            sub eax, 0
            je L004eb5c9
            dec eax
            je L004eb52a
            dec eax
            jne L004eb690
            mov edx, dword ptr [esp + 0x44]
            lea eax, [ebx + 0x2c]
            test edx, edx
            je L004eb4fb
            mov ecx, dword ptr [esp + 0x30]
            mov edi, eax
            push edx
            push eax
            lea esi, [ecx + 0x2c]
            mov ecx, 0xc
            rep movsd
            call BrMatrix34Post
            fld dword ptr [esp + 0x18]
            add esp, 8
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x1c
            ret 0x18
    L004eb4fb:
            mov edx, dword ptr [esp + 0x30]
            mov ecx, 0xc
            mov edi, eax
            lea esi, [edx + 0x2c]
            rep movsd
            mov ecx, dword ptr [esp + 0x3c]
            add ecx, 0x2c
            push ecx
            push eax
            call BrMatrix34Post
            fld dword ptr [esp + 0x18]
            add esp, 8
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x1c
            ret 0x18
    L004eb52a:
            lea ecx, [ebx + 0x2c]
            call FUN_004eb6a0
            mov eax, dword ptr [esp + 0x40]
            lea ebp, [esp + 0x20]
            add edi, 4
            mov esi, eax
            sub ebp, eax
            mov dword ptr [esp + 0x30], 3
    L004eb549:
            test eax, eax
            je L004eb55f
            mov edx, dword ptr [esi + 0xc]
            mov eax, dword ptr [esi]
            push edx
            push eax
            call FRandomBetween
            mov eax, dword ptr [esp + 0x40]
            jmp L004eb565
    L004eb55f:
            fld dword ptr [gFloat_0058aec0]
    L004eb565:
            fld dword ptr [edi]
            mov ecx, dword ptr [esp + 0x30]
            add esi, 4
            fmul st(0), st(1)
            add edi, 4
            dec ecx
            mov dword ptr [esp + 0x30], ecx
            fstp dword ptr [ebp + esi - 4]
            fstp st(0)
            jne L004eb549
            mov ecx, dword ptr [esp + 0x3c]
            lea edx, [esp + 0x20]
            add ecx, 0x2c
            lea eax, [esp + 0x14]
            push ecx
            push edx
            push eax
            call BrMatrix34ApplyV
            mov eax, dword ptr [esp + 0x40]
            add esp, 0xc
            fld dword ptr [esp + 0x14]
            fadd dword ptr [eax]
            fstp dword ptr [ebx + 0x50]
            fld dword ptr [eax + 4]
            fadd dword ptr [esp + 0x18]
            fstp dword ptr [ebx + 0x54]
            fld dword ptr [eax + 8]
            fadd dword ptr [esp + 0x1c]
            fstp dword ptr [ebx + 0x58]
            fld dword ptr [esp + 0x10]
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x1c
            ret 0x18
    L004eb5c9:
            lea ecx, [ebx + 0x2c]
            call FUN_004eb6a0
            mov eax, dword ptr [edi + 8]
            mov esi, dword ptr [esp + 0x38]
            test eax, eax
            je L004eb5e0
            mov esi, dword ptr [esp + 0x34]
    L004eb5e0:
            lea ebp, [ebx + 0x50]
            mov ecx, esi
            mov edx, ebp
            push 0x3f800000
            mov eax, dword ptr [ecx]
            mov dword ptr [edx], eax
            mov eax, dword ptr [ecx + 4]
            mov dword ptr [edx + 4], eax
            mov ecx, dword ptr [ecx + 8]
            mov dword ptr [edx + 8], ecx
            call FRandomPosNeg
            fstp dword ptr [esp + 0x30]
            push 0x3f800000
            call FRandomPosNeg
            fstp dword ptr [esp + 0x40]
            push 0x3f800000
            call FRandomPosNeg
            mov edx, dword ptr [edi + 4]
            fstp dword ptr [esp + 0x3c]
            push edx
            push 0
            call FRandomBetween
            fld dword ptr [esp + 0x40]
            fld dword ptr [esp + 0x30]
            fmul dword ptr [esp + 0x30]
            fld dword ptr [esp + 0x3c]
            fxch st(2)
            fmul dword ptr [esp + 0x40]
            fxch st(3)
            fstp dword ptr [esp + 0x34]
            fxch st(1)
            fmul dword ptr [esp + 0x3c]
            fxch st(2)
            faddp st(1), st(0)
            fld dword ptr [esp + 0x34]
            fdiv dword ptr [edi + 4]
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            fstp dword ptr [esp + 0x10]
            fsqrt
            fdivr dword ptr [esp + 0x34]
            fld st(0)
            fmul dword ptr [esp + 0x30]
            fld st(1)
            fmul dword ptr [esp + 0x40]
            fxch st(1)
            fadd dword ptr [esi]
            fxch st(2)
            fmul dword ptr [esp + 0x3c]
            fxch st(2)
            fstp dword ptr [ebp]
            fadd dword ptr [esi + 4]
            fstp dword ptr [ebx + 0x54]
            fadd dword ptr [esi + 8]
            fstp dword ptr [ebx + 0x58]
    L004eb690:
            fld dword ptr [esp + 0x10]
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x1c
            ret 0x18
            nop
            nop
    }
#endif
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
void C2_NAKED C2_HOOK_FASTCALL FUN_00497860(tNet_message* pMessage, void* pSender_address) {
#ifndef CARPOCALYPSE2_MATCHING
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
#else
    __asm {
            mov eax, dword ptr [gNet_mode]
            push esi
            push edi
            test eax, eax
            je L004978c1
            mov esi, dword ptr [gNumber_of_net_players]
            xor eax, eax
            test esi, esi
            jle L004978c1
            mov edi, dword ptr [edx + 8]
            mov edx, offset gNet_players+0x24
    L0049787f:
            cmp dword ptr [edx], edi
            je L00497891
            inc eax
            add edx, 0xd4
            cmp eax, esi
            jl L0049787f
            pop edi
            pop esi
            ret
    L00497891:
            lea edx, [eax + eax*2]
            lea edx, [edx + edx*8]
            shl edx, 1
            sub edx, eax
            mov eax, dword ptr [ecx + 4]
            shl edx, 2
            mov dword ptr [edx + gNet_players+0xd0], eax
            mov ecx, dword ptr [edx + gNet_players+0x24]
            cmp ecx, eax
            jne L004978c1
            mov eax, dword ptr [edx + gNet_players+0xc8]
            mov eax, dword ptr [eax + 8]
            add dword ptr [eax + 0x49c], 0x28
    L004978c1:
            pop edi
            pop esi
            ret
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004ef830
void C2_HOOK_FASTCALL ActuallyRepairSmashThunk(tCar_spec* pCar_spec, tCar_crush_smashable_part* pSmashable, int pUnused, int pLevel) {
    ActuallyRepairSmash(pCar_spec, pSmashable, pLevel);
}
