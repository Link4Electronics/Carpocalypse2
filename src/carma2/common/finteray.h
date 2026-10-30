#ifndef GUARD_FINTERAY_H
#define GUARD_FINTERAY_H

#include "c2_hooks.h"
#include "carpocalypse2_types.h"

typedef struct {
    br_vector3* edge_record;
    br_vector3 actor_normal;
    br_vector3 object_normal;
    br_vector3 shape_normal;
    br_vector3 hit_position_and_corner[2];
} tFinteray_ray_cast_locals;

// BadDiv

// DRVector2AccumulateScale

// PickBoundsTestRay

// ActorRayPick2D

// DRSceneRayPick2D

// DRModelPick2D

// FindHighestPolyCallBack

// FindHighestCallBack

// ActorFindFace

// EnablePlingMaterials

// DisablePlingMaterials

// StationarySphereSphereCollide

// SphereSphereCollide

// StationarySphereEdgeCollide

// PointSphereCollision
void C2_HOOK_FASTCALL PointSphereCollision(br_vector3* pPoint, br_vector3* pSphere_center, br_scalar pRadius, br_vector3* pNormal, br_scalar* pCollision_t);
extern void C2_HOOK_FASTCALL StationarySphereEdgeCollide(br_vector3* pEdge_start, br_vector3* pEdge_end, br_vector3* pEdge_direction, br_scalar pEdge_length_squared, br_vector3* pSphere_end, tCollision_shape_sphere_data* pSphere, br_vector3* pPoint, br_vector3* pNormal, br_scalar* pT);


void C2_HOOK_FASTCALL FUN_0045e200(br_scalar* pClearance, tCollision_shape_polyhedron_data* pShape, const br_vector3* pStart, const br_vector3* pEnd, br_vector3* pHit_position, br_vector3* pNormal, br_scalar* pFactor);

extern void C2_HOOK_FASTCALL FUN_0045e020(br_vector3* pVertex, br_vector3* pSphere_start, br_vector3* pSphere_end, br_vector3* pSphere_direction, tCollision_shape_sphere_data* pSphere, br_vector3* pPoint, br_vector3* pNormal, br_scalar* pT);

void C2_HOOK_FASTCALL FUN_0045dea0(const br_vector3* pStart, const br_vector3* pEnd, const br_vector3* pSegment_delta, br_scalar pSegment_length_squared, const br_vector3* pEdge_end, const tCollision_shape_sphere_data* pSphere, br_vector3* pCollision_pos, br_vector3* pNormal, br_scalar* pT);
void C2_HOOK_FASTCALL FUN_0045d920(br_scalar* pStart, br_scalar* pEnd, br_scalar* pEdge_start, br_scalar* pEdge_end, br_scalar* pSphere, br_scalar* pCollision_pos, br_scalar* pNormal, br_scalar* pT, tU32 pStart_flags, tU32 pEnd_flags);
void C2_HOOK_FASTCALL FUN_0045e4e0(float* param_1, float* param_2, float* param_3, float* param_4, float* param_5, float* param_6, float* param_7);

// SphereEdgeCollide

// SpherePolyFaceCollide

// SphereBoxFaceCollide

// SphereFaceCollide

// CheckSingleFace

// MultiRayCheckSingleFace

// GetNewBoundingBox

// FillInBounds

// ActorBoxPick

// ModelPickBox

// ClipToPlaneGE

// ClipToPlaneLE

// BoundsOverlapTest

// BoundsTransformTest

// LineBoxColl

// SphereBoxIntersection

// LineBoxCollWithSphere

// LineSphereColl

// LineSphereCollWithSphere

// LinePolyColl

// SpherePolyIntersection

// LinePolyCollWithSphere

// ShapeRayCast

// ShapeRayCastWithSphere

// CastRayDownThruFaces

#define CARPOCALYPSE2_FINTERAY_H

#include "c2_hooks.h"

#include "carpocalypse2_types.h"

extern int gPling_materials;
extern void C2_HOOK_FASTCALL SphereFaceCollide(tFace_ref* pFace, br_vector3* pStart, br_vector3* pEnd, br_scalar pRadius, void* pUnused_point, br_vector3* pNormal, br_scalar* pT);


extern int C2_HOOK_FASTCALL LineBoxColl(br_vector3* pStart, br_vector3* pEnd, br_bounds* pBounds, br_vector3* pPoint, br_scalar* pT);
extern int C2_HOOK_FASTCALL SphereBoxIntersection(br_bounds* pBounds, br_vector3* pPoint, br_scalar pRadius_squared, br_vector3* pClosest);


extern int C2_HOOK_FASTCALL LinePolyCollWithSphere(br_vector3* pStart, br_vector3* pEnd, tCollision_shape_polyhedron* pShape, br_vector3* pPoint, br_vector3* pNormal, br_scalar* pT);

extern int C2_HOOK_FASTCALL ShapeRayCastWithSphere(br_vector3* pStart, br_vector3* pEnd, tPhysics_shape* pShape, br_vector3* pPoint, br_vector3* pNormal, br_scalar* pT);


extern int gTemp_group;

extern int gNearest_face;

extern br_model* gNearest_model;

extern br_actor* gNearest_actor;

extern br_scalar gNearest_T;

extern int gNearest_face_group;

extern br_matrix34 gPick_model_to_view__finteray;

extern tFace_ref* gPling_face;

extern int gActorBoxPick_StopGroovidelics;

void C2_HOOK_FASTCALL EnablePlingMaterials(void);

void C2_HOOK_FASTCALL DisablePlingMaterials(void);

void C2_HOOK_FASTCALL MultiRayCheckSingleFace(int pNum_rays, tFace_ref* pFace, br_vector3* ray_pos, br_vector3* ray_dir, br_vector3* normal, br_scalar* rt);

void C2_HOOK_FASTCALL GetNewBoundingBox(br_bounds* b2, br_bounds* b1, br_matrix34* m);

int C2_HOOK_FASTCALL PickBoundsTestRay__finteray(br_bounds* b, br_vector3* rp, br_vector3* rd, br_scalar t_near, br_scalar t_far, br_scalar* new_t_near, br_scalar* new_t_far);

int C2_HOOK_FASTCALL FUN_00460da0(br_bounds* pBounds, br_vector3* pPoint, br_scalar pRadius_squared, br_vector3* pClosestPoint);

tU32 C2_HOOK_FASTCALL FUN_00460e10(void* pUnused_ecx, br_vector3* pPoint, br_bounds* pBounds, br_vector3* pPosition, br_vector3* pNormal, float* pFactor);

int C2_HOOK_FASTCALL FUN_004611b0(const br_vector3* pRay_start, const br_vector3* pRay_end, const tCollision_shape_sphere_data* pSphere, br_vector3* pPosition, br_vector3* pNormal, float* pFactor);
int C2_HOOK_FASTCALL FUN_00425ca0(tPhysics_object* pObject1, tPhysics_object* pObject2, void* pCollision_contacts, int pCollision_count);

tU32 C2_HOOK_FASTCALL FUN_00461520(const br_vector3* pRay_start, const br_vector3* pRay_end, tCollision_shape_polyhedron* pShape, br_vector3* pPosition, br_vector3* pNormal, float* pFactor);

void C2_HOOK_FASTCALL DRVector2AccumulateScale__finteray(br_vector2* a, const br_vector2* b, br_scalar s);

int C2_HOOK_FASTCALL DRModelPick2D__finteray(br_model* model, br_material* material, br_actor* actor, br_vector3* ray_pos, br_vector3* ray_dir, br_scalar t_near, br_scalar t_far, dr_modelpick2d_cbfn* callback, void* arg);

int C2_HOOK_FASTCALL ActorRayPick2D(br_actor* ap, br_vector3* pPosition, br_vector3* pDir, br_model* model, br_material* material, dr_pick2d_cbfn* callback);

int C2_HOOK_CDECL FindHighestPolyCallBack__finteray(br_model* pModel, br_actor* pActor, br_material* pMaterial, br_vector3* pRay_pos, br_vector3* pRay_dir, float pT, int pF, int pE, int pV, br_vector3* pPoint, br_vector2* pMap, void* pData);

int C2_HOOK_CDECL FindHighestCallBack__finteray(br_actor* pActor, br_model* pModel, br_material* pMaterial, br_vector3* pRay_pos, br_vector3* pRay_dir, br_scalar pT_near, br_scalar pT_far, void* pArg);

int C2_HOOK_FASTCALL DRSceneRayPick2D(br_actor* pWorld, br_vector3* pPosition, br_vector3* pDir, dr_pick2d_cbfn* pCallback);

void C2_HOOK_FASTCALL ActorFindFace(br_vector3* pPosition, br_vector3* pDir, br_actor* pWorld, br_vector3* nor, br_scalar* t, br_material** material, br_actor** actor);

void C2_HOOK_FASTCALL FUN_0041e3f0(br_vector3* pRay_pos, br_vector3* pRay_dir, br_vector3* pHit_pos, br_scalar* pT, void* pRay_context);

void C2_HOOK_FASTCALL FUN_0041e360(br_vector3* pRay_pos, br_vector3* pRay_dir, br_vector3* pNor, br_scalar* pT);

void C2_HOOK_FASTCALL FindFace(br_vector3* pPosition, br_vector3* pDir, br_vector3* nor, br_scalar* t, br_material** material);

tFace_ref* C2_HOOK_FASTCALL FUN_004225f0(br_vector3* pRay_pos, br_vector3* pRay_dir, br_vector3* pHit_normal, br_scalar* pBest_t, br_vector3* pHit_pos, tFace_ref* pFaces, int pFace_count);
tFace_ref* C2_HOOK_FASTCALL FUN_004227a0(br_vector3* pRay_pos, br_vector3* pRay_dir, br_vector3* pHit_normal, br_scalar* pBest_t, br_vector3* pHit_pos, tFace_ref** pFaces, int pFace_count);

void C2_HOOK_FASTCALL CheckSingleFace(tFace_ref* pFace, br_vector3* ray_pos, br_vector3* ray_dir, br_vector3* normal, br_scalar* rt, br_vector3* coll_pos);

void C2_HOOK_FASTCALL FillInBounds(tBounds* bnds);

int C2_HOOK_FASTCALL FUN_004268a0(float* pRecords, br_vector3* pVector_a, float pT, br_vector3* pVector_b, br_vector3* pVector_c, int pCount, void* pContext_a, void* pContext_b, unsigned int pValue_a, br_vector3* pVector_d, float pValue_b);

int C2_HOOK_FASTCALL FUN_00427870(tPhysics_object* pObject1, tPhysics_object* pObject2, tPhysics_shape* pShape1, tPhysics_shape* pShape2, const br_vector3* pVertices1, const br_vector3* pVertices2, int pCount1, int pCount2, void* pRecords, int pRecord_count, undefined4 pArg11);

int C2_HOOK_FASTCALL FUN_004269c0(tPhysics_object* pObject1, tPhysics_object* pObject2, tPhysics_shape* pShape1, tPhysics_shape* pShape2, const br_vector3* pVertices1, const br_vector3* pVertices2, int pCount1, int pCount2, void* pRecords, int pRecord_count, int pRemaining);

void C2_HOOK_FASTCALL FUN_0045d5e0(tCollision_shape_sphere_data* pSphere1, tCollision_shape_sphere_data* pSphere2, br_vector3* pStart, br_vector3* pEnd, br_vector3* pNormal, float* pFactor);

extern const tPolyhedron_edge_indexes gBox_edges_0058f7f0[12];

int C2_HOOK_FASTCALL FUN_00427fd0(tPhysics_object* pObj1, tPhysics_object* pObj2, tPhysics_shape* pEdgeShape, tPhysics_shape* pRayShape, int pVertices, float* pRecords, int pRecordCount, undefined4 pUnused, const tU8* pEdge_visibility, int* pCollided, float* pNormal);

int C2_HOOK_FASTCALL FUN_004287a0(tPhysics_object* pObject1, tPhysics_object* pObject2, tPhysics_shape* pShape1, tPhysics_shape* pShape2, const br_vector3* pVertices1, const br_vector3* pVertices2, void* pRecords, int pRecord_count, undefined4 pArg11, const tU8* pVisibility1, const tU8* pVisibility2);

int C2_HOOK_FASTCALL FUN_00428530(tPhysics_object* pObject1, tPhysics_object* pObject2, tPhysics_shape* pCorner_shape, tPhysics_shape* pShape, float pT, int pEdge_count, float* pRecords, int pRecord_count, undefined4 pArg9, tU8* pEdge_visibility);

int C2_HOOK_FASTCALL BoundsOverlapTest__finteray(br_bounds* b1, br_bounds* b2);
int C2_HOOK_CDECL TestSegmentBox(const br_vector3* pA, const br_vector3* pB, const br_bounds3* pBox);


int C2_HOOK_FASTCALL ModelPickBox(br_actor* actor, tBounds* bnds, br_model* model, br_material* model_material, tFace_ref* face_list, int max_face, br_matrix34* pMat);

int C2_HOOK_FASTCALL ActorBoxPick(tBounds* bnds, br_actor* ap, br_model* model, br_material* material, tFace_ref* face_list, int max_face, br_matrix34* pMat, tWorld_callbacks* pWorld_callbacks);

int C2_HOOK_FASTCALL FindFacesInBox(tBounds* bnds, tFace_ref* face_list, int max_face, tWorld_callbacks* pWorld_callbacks);

tFace_ref* C2_HOOK_FASTCALL CastRayDownThruFaces(br_vector3* pPosition, tFace_ref* pFaces, int pCount, br_scalar pOffset, br_vector3* pCollision_position);


extern int C2_HOOK_FASTCALL LinePolyColl(br_vector3* pStart, br_vector3* pEnd, tCollision_shape_polyhedron_data* pPoly, br_vector3* pPoint, br_scalar pRadius, br_scalar* pT);

extern int C2_HOOK_FASTCALL LineSphereCollWithSphere(br_vector3* pStart, br_vector3* pEnd, tCollision_shape_sphere_data* pSphere, br_vector3* pPoint, br_vector3* pNormal, br_scalar* pT);

extern int C2_HOOK_FASTCALL LineSphereColl(br_vector3* pStart, br_vector3* pEnd, tCollision_shape_sphere_data* pSphere, br_vector3* pPoint, br_vector3* pNormal, br_scalar* pT);

extern int C2_HOOK_FASTCALL LineBoxCollWithSphere(br_vector3* pStart, br_vector3* pEnd, br_bounds* pBounds, br_vector3* pPoint, br_vector3* pNormal, br_scalar* pT);

#endif // GUARD_FINTERAY_H
