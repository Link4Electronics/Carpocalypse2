#ifndef GUARD_RAYCAST_H
#define GUARD_RAYCAST_H

#include "c2_hooks.h"

#include "carpocalypse2_types.h"

// DRActorToRoot

extern void C2_HOOK_FASTCALL InitRayCasting(void);

// BadDiv

// DRVector2AccumulateScale

// PickBoundsTestRay

// ActorPick2D

// DRScenePick2DXY

// DRScenePick2D

// DRModelPick2D

// FindHighestPolyCallBack

// FindHighestCallBack

// FindBestY

// FindYVerticallyBelowPolyCallBack

// FindYVerticallyBelowCallBack

// FindYVerticallyBelow

// FindYVerticallyBelow2

#define CARPOCALYPSE2_RAYCAST_H

#include "c2_hooks.h"

#include "carpocalypse2_types.h"

extern br_actor* gY_picking_camera;

extern br_scalar gLowest_y_above;

extern br_scalar gHighest_y_below;

extern br_scalar gCurrent_y;

extern br_model* gAbove_model;

extern br_model* gBelow_model;

extern int gAbove_face_index;

extern int gBelow_face_index;

extern br_material* gMaterial_below;

int C2_HOOK_FASTCALL PickBoundsTestRay__raycast(br_bounds* b, br_vector3* rp, br_vector3* rd, br_scalar t_near, br_scalar t_far, br_scalar* new_t_near, br_scalar* new_t_far);

int C2_HOOK_FASTCALL ActorPick2D(br_actor* ap, br_model* model, br_material* material, dr_pick2d_cbfn* callback, void* arg);

int C2_HOOK_FASTCALL DRActorToRoot(br_actor* a, br_actor* world, br_matrix34* m);

int C2_HOOK_FASTCALL DRScenePick2DXY(br_actor* world, br_actor* camera, br_pixelmap* viewport, int pick_x, int pick_y, dr_pick2d_cbfn* callback, void* arg);

int C2_HOOK_CDECL FindYVerticallyBelowPolyCallBack(br_model* pModel, br_material* pMaterial, br_vector3* pRay_pos, br_vector3* pRay_dir, float pT, int pF, int pE, int pV, br_vector3* pPoint, br_vector2* pMap, void* pContext);

int C2_HOOK_FASTCALL BadDiv_raycast(br_scalar a, br_scalar b);

void C2_HOOK_FASTCALL DRVector2AccumulateScale_raycast(br_vector2* a, br_vector2* b, br_scalar s);

int C2_HOOK_FASTCALL DRModelPick2D_raycast(br_model* model, br_material* material, br_vector3* ray_pos, br_vector3* ray_dir, br_scalar t_near, br_scalar t_far, dr_modelpick2d_raycast_cbfn* callback, void* arg);

int C2_HOOK_CDECL FindYVerticallyBelowCallBack(br_actor* pActor, br_model* pModel, br_material* pMaterial, br_vector3* pRay_pos, br_vector3* pRay_dir, float pT_near, float pT_far, void* pArg);

int C2_HOOK_FASTCALL DRScenePick2D(br_actor* world, br_actor* camera, dr_pick2d_cbfn* callback, void* arg);

br_scalar C2_HOOK_FASTCALL FindYVerticallyBelow(br_vector3* pPosition);

br_scalar C2_HOOK_FASTCALL FindYVerticallyBelow2(br_vector3* pCast_point);

int C2_HOOK_CDECL FindHighestPolyCallBack__raycast(br_model* pModel, br_material* pMaterial, br_vector3* pRay_pos, br_vector3* pRay_dir, br_scalar pT, int pF, int pE, int pV, br_vector3* pPoint, br_vector2* pMap, void* pArg);

int C2_HOOK_CDECL FindHighestCallBack__raycast(br_actor* pActor, br_model* pModel, br_material* pMaterial, br_vector3* pRay_pos, br_vector3* pRay_dir, br_scalar pT_near, br_scalar pT_far, void* pArg);

void C2_HOOK_FASTCALL FindBestY(br_vector3* pPosition, br_actor* pWorld, br_scalar pStarting_height, br_scalar* pNearest_y_above, br_scalar* pNearest_y_below, br_model** pNearest_above_model, br_model** pNearest_below_model, int* pNearest_above_face_index, int* pNearest_below_face_index);

void C2_HOOK_FASTCALL DrMatrix34ApplyLPInverse(br_vector3* pDest, const br_vector3* pOrigin, const br_matrix34* pMatrix);

tFace_ref* C2_HOOK_FASTCALL FUN_004226d0(const br_vector3* pRay_pos, const br_vector3* pRay_dir, br_vector3* pHit_normal, br_scalar* pDistance, br_vector3* pHit_position, tFace_ref* pFaces, int pFace_count);

int C2_HOOK_FASTCALL FUN_004228b0(tPhysics_object* pObject, void* pCollision_buffer, int pIndex);

int C2_HOOK_FASTCALL FUN_00423320(tPhysics_object* pObject, tPhysics_shape* pShape, br_matrix34* pMatrix);

int C2_HOOK_FASTCALL FUN_004236a0(tPhysics_object* pObject, tPhysics_shape* pShape, void* pCollision_slot, int pCollision_count, int pMax_collisions, tCollision_shape_test_data* pShape_data, void* pScratch, int* pHas_collision);

int C2_HOOK_FASTCALL FUN_00423ed0(tPhysics_object* pObject, tPhysics_shape* pShape, void* pCollision_slot, int pCollision_count, int pMax_collisions, tCollision_shape_test_data* pShape_data, void* pScratch);
int C2_HOOK_FASTCALL FUN_00423c00(tPhysics_object* pOwner, br_scalar* pSphere, void* pCollision_list, int pCollision_count, int pUnused5, br_vector3* pEdge_start, br_vector3* pEdge_end, void* pUnused8);


void C2_HOOK_FASTCALL FUN_0045e850(tFace_ref* pFace, br_vector3* pSphere_center, br_vector3* pSphere_vertex, br_scalar pRadius, br_vector3* pNormal, br_vector3* pClosest_point, br_scalar* pDistance);

int C2_HOOK_FASTCALL FUN_00424270(float* pStart, float* pEnd, int* volatile param_3, int param_4, undefined4 param_5, int param_6, int param_7, int param_8);

int C2_HOOK_FASTCALL FUN_00424990(float* p1, float* p2, float* p3, float* p4, float* p5, float* p6, float* p7, undefined4 p8, int p9, int p10);

int C2_HOOK_FASTCALL FUN_00422fe0(float* pCollision_list, float* pContact_pos, float pT, float* pNormal, undefined4 pUnk5, int pCollision_count, tPhysics_object* pOwner, int pType, void* pCollision_data, float* pOptional_normal);

int C2_HOOK_FASTCALL FUN_00423ce0(br_vector3* pStart, br_vector3* pEnd, tPhysics_object* pOwner, br_scalar* pSphere, void* pCollision_list, int pCollision_count, br_vector3* pEdge_start, br_vector3* pEdge_end, void* pCollision_data);

int C2_HOOK_FASTCALL FUN_00424140(tPhysics_object* pObject, tPhysics_shape* pBounds_shape, tPhysics_shape* pRay_shape, int pCollision_count, undefined4 pArg5, tU8* pPolyhedron_data, undefined4 pArg7);

int C2_HOOK_FASTCALL FUN_00425630(const br_vector3* param_1, const br_vector3* param_2, const br_vector3* param_3, const br_vector3* param_4, const br_vector3* param_5, const br_vector3* param_6, const br_vector3* param_7, const br_vector3* param_8, float* param_9, undefined4 param_10, const tPhysics_object* param_11, const tPhysics_object* param_12);

extern int gSensible_place_collect_active;

extern int gSensible_place_collect_active; // also written by TestForObjectInSensiblePlace (physics.c)

#endif // GUARD_RAYCAST_H
