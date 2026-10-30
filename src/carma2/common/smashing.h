#ifndef CARPOCALYPSE2_SMASHING_H
#define CARPOCALYPSE2_SMASHING_H

#include "carpocalypse2_types.h"

#include "c2_hooks.h"

#include "c2_stdio.h"

typedef struct tSmash_actor_search_context {
    const br_bounds3* bounds;
    tS8 field_0x4;
    tS8 count;
    tU8 field_0x6[2];
    br_actor* actors[20];
} tSmash_actor_search_context;

extern tSmash_vertex gSmash_glass_fragments[200];
extern tSmashable_race_target gSmashable_race_targets[300];
extern int gCount_smashable_race_targets;
extern int gCount_queued_smashes;
extern tU32 gLast_munge_smash_edge_triggers;
extern br_vector3 gZero_vector__smash;
extern const double gDOUBLE_0058af40;
extern const char* gInitial_smashable_position_type_names[3];
extern const char* gInitial_position_sphere_where[2];

extern tPowerup_queue_item gPowerup_queue[50];
extern tQueued_smash gQueued_smashes[20];
extern br_matrix34 gMatrix34_006b78a0;
extern br_vector3 gVector3_006a4688;
extern int gINT_006a3334;

void C2_HOOK_FASTCALL SmashItIntoVerySmallPiecesIndeed(tPhysics_object* pObject, tSmashable_item_spec* pSpec, undefined4 pArg3, tConnotations* pConnotations, tU32 pSeed, undefined4 pArg6, const br_vector3* pPoint, const br_vector3* pSound_position, br_actor* pActor, tU32 pTime, undefined4 pArg11, br_matrix34* pTransform, br_bounds3* pBounds, br_vector3* pPosition, float pDistance, undefined4 pArg16, int pFlag);
void C2_HOOK_FASTCALL FUN_004edbe0(undefined4 pArg1, int* pArg2, int* pArg3, undefined4 pArg4, float pArg5);

void C2_HOOK_FASTCALL InitGlassFragments(void);

tSmash_vertex* C2_HOOK_FASTCALL FindShrapnelFragment(br_model* pModel, tU32 pField_0x18, int pPriority);

void C2_HOOK_FASTCALL FUN_004eff30(tSmash_vertex* pFragment, tU32 pTime, float pDelta_time);

float C2_HOOK_FASTCALL PositionFragment(br_actor* pActor, tSmashable_initial_position_spec* pInitial_position, br_actor* pSource_actor, br_vector3* pPosition4, br_vector3* pPosition5, br_actor* pRotation_actor, float* pRandom_ranges, br_matrix34* pPost_matrix);

void C2_HOOK_FASTCALL GenerateRandomRotationMatrix34(br_matrix34* pMatrix);

void C2_HOOK_FASTCALL WorkerShrapnelShower(int pOwner, int* pShrapnel_spec, tU32 pSeed, undefined4 pArg4, undefined4 pArg5, undefined4 pArg6, undefined4 pArg7, int pTime, undefined4 pArg9, br_matrix34* pTransform, undefined4 pFragment_owner, br_vector3* pPosition, float pDistance, undefined4 pArg14, undefined4 pArg15);

void C2_HOOK_FAKE_THISCALL FUN_004f0380(tU32 pCurrent_time, int pArg2, int* pArg3, tU32 pArg4, undefined4 pArg5, undefined4 pArg6, undefined4 pArg7, undefined4 pArg8, tU32 pStart_time, tU32 pOwner, undefined4 pArg11, undefined4 pArg12, undefined* pArg13, float pArg14, undefined4 pArg15);

void C2_HOOK_FASTCALL InitSmashing(void);

void C2_HOOK_FASTCALL ReadSmashableInitialPosition(FILE* pFile, tSmashable_initial_position_spec*  pInitial_pos);

void C2_HOOK_FASTCALL ReadSmashableInitialSpeed(FILE* pFile, tSmashable_initial_speed_spec* pInitial_speed);

void C2_HOOK_FASTCALL ReadMinMaxTimeInMilliseconds(FILE* pFile, int* pTimes);

void C2_HOOK_FASTCALL InitSmashTargets(void);

void C2_HOOK_FASTCALL AddSmashableRaceTarget(br_model* pModel, br_actor* pActor, int pUnknown);

void C2_HOOK_FASTCALL FUN_00497860(tNet_message* pMessage, void* pSender_address);

void C2_HOOK_FASTCALL SplondificatalizeIdentifier(br_material* pMaterial, char** ppIdentifier, int pIndex);

void C2_HOOK_FASTCALL MungeSmashMaterialNames(void);

void C2_HOOK_FASTCALL FUN_004f1030(tPhysics_object* pObject, int pMode, br_actor* pActor);

void C2_HOOK_FASTCALL CleanUpSmashStuff(void);

void C2_HOOK_FASTCALL DoFragMovement(tSmash_vertex* pFragment, tU32  pTime, float pDelta_time);


void C2_HOOK_FASTCALL KillFragment(tSmash_vertex* pFragment);

void C2_HOOK_FASTCALL MungeGlassFragments2(int pEnd_race);


void C2_HOOK_FASTCALL MungeAnimationRepairs(void);

void C2_HOOK_FASTCALL MungeGlassFragments(void);

void C2_HOOK_FASTCALL ActuallySmashCarPart(tCar_spec* pCar_spec, tCar_crush_smashable_part* pSmashable, int pLevel);

void C2_HOOK_FASTCALL FlushSmashQueue(int pFlush_powerups);

void C2_HOOK_FASTCALL FlushPowerupQueue(void);


void C2_HOOK_FASTCALL QueueNonCarCuboidActivation(br_actor* pActor, tNon_car_cuboid_activations* pActivations, void* pContext);

int C2_HOOK_CDECL FUN_004edb20(br_actor* pActor, tSmash_actor_search_context* pContext);

typedef struct tQueueOilSpill2Settings tQueueOilSpill2Settings;
void C2_HOOK_FASTCALL DoSpillage(tSlick_spec* pSlick, br_actor* pActor);

void C2_HOOK_FASTCALL MungeDelayedSideEffects(void);

void C2_HOOK_FASTCALL DoPowerupRespawnSmash(br_actor *pActor);

void C2_HOOK_FASTCALL MungeSmashEdgeTriggers(tU32 pTime);

void C2_HOOK_FASTCALL InitSmashQueue(void);

void C2_HOOK_FASTCALL MungeInternalCarGlass(tCar_spec* pCar_spec);

void C2_HOOK_FASTCALL ActuallyRepairSmash(tCar_spec* pCar_spec, tCar_crush_smashable_part* pSmashable, int pLevel);
void C2_HOOK_FASTCALL ActuallyRepairSmashThunk(tCar_spec* pCar_spec, tCar_crush_smashable_part* pSmashable, int pUnused, int pLevel);

void C2_HOOK_FASTCALL ApplyInitialMovement(undefined4* pArg1, br_vector3* pArg2, br_vector3* pArg3, br_vector3* pArg4, float pArg5, br_vector3* pArg6, br_vector3* pArg7, br_vector3* pArg8);

void C2_HOOK_FASTCALL CreateSmashedGlassFragments(tU32 pSeed, br_vector3* pDirection, br_actor* pSource_actor, tU32 pTime, int pFace_count, const tU16* pSource_faces, int pVertex_count, const tU16* pSource_vertices, tU32 pOwner, tU32 pFlags, float pLongest_edge_squared, tSmashable_initial_speed_spec* pInitial_speed, const int* pLifetime_range, tU32 pFragment_owner, br_matrix34* pSource_transform);


// UNLABELED: CARMA2_HW 0x004eb120
void C2_HOOK_FASTCALL InitFragmentSpeed(tSmash_vertex* pFragment, tSmashable_initial_speed_spec* pSpeed, br_vector3* pArg3, float pArg4, int* pArg5, int pArg6, br_vector3* pArg7, int pArg8, int pArg9);

void C2_HOOK_FASTCALL TotallyRepairSmash(tCar_spec *pCar_Spec, tCar_crush_buffer_entry *pSmash_data);

#endif //CARPOCALYPSE2_SMASHING_H
