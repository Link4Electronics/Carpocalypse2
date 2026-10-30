#ifndef CARPOCALYPSE2_EXPLOSIONS_H
#define CARPOCALYPSE2_EXPLOSIONS_H

#include "carpocalypse2_types.h"

#include "c2_hooks.h"
#include "c2_stdio.h"

extern tSmash_explosion gSmash_explosions[20];

void C2_HOOK_FASTCALL InitExplosions(void);

void C2_HOOK_FASTCALL ResetExplosions(void);

void C2_HOOK_FASTCALL ReadExplosionInfo(FILE* pF, int* pChance_explosion, int* pExplosion_sound_id, tExplosion_animation* pExplosion_groups);

int C2_HOOK_FAKE_THISCALL PointOutOfSightNotAR(const br_vector3* pPoint, undefined4 pArg2, float pMax_distance);

void C2_HOOK_FASTCALL MungeExplosions(void);

void C2_HOOK_FASTCALL Explode(tExplosion_animation* pExplosion_animation, br_actor* pActor, br_bounds3* pBounds, br_vector3* pPos);

void C2_HOOK_FASTCALL FUN_004c89f0(uintptr_t pOwner);

void C2_HOOK_FASTCALL FUN_004eb120(tSmash_vertex* pFragment, tSmashable_initial_speed_spec* pInitial_speed, br_vector3* pDirection, float pScale, int* pTime_range, tU32 pTime, br_vector3* pRandom_direction, br_vector3* pArg7, br_vector3* pArg8);

void C2_HOOK_FASTCALL RemoveExplosions(tCar_spec* pCar);

void C2_HOOK_FASTCALL AddFacesToModel(br_model* pModel, int pFace_count, const tU16* pFace_data, br_material* pMaterial);

void C2_HOOK_FASTCALL FUN_004c8980(uintptr_t pOwner, tU32 pCount, const void* pData, undefined4 pValue);

#endif // CARPOCALYPSE2_EXPLOSIONS_H
