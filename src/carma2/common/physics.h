#ifndef GUARD_PHYSICS_H
#define GUARD_PHYSICS_H

#include "c2_hooks.h"
#include "carpocalypse2_types.h"

typedef struct tCollision_contact {
    float impulse;              /* 0x00: impulse magnitude */
    tPhysics_object* object1;   /* 0x04: first object */
    tPhysics_object* object2;   /* 0x08: second object (can be NULL) */
    void* field_0xc;            /* 0x0c: world face for world contacts (friction callback argument) */
    float field_0x10;           /* 0x10: some float */
    br_vector3 r1;              /* 0x14: contact point relative to object1 */
    br_vector3 r2;              /* 0x20: contact point relative to object2 */
    br_vector3 normal;          /* 0x2c: contact normal */
    br_vector3 field_0x38;      /* 0x38: another vector */
    br_vector3 impulse1;        /* 0x44: impulse applied to object1 */
    br_vector3 impulse2;        /* 0x50: impulse applied to object2 */
    int type;                   /* 0x5c: contact type (0xe = skip) */
} tCollision_contact;

typedef struct {
    float depth;                 // 0x00
    tPhysics_object* object1;    // 0x04
    tPhysics_object* object2;    // 0x08
    float field_0x0c;            // 0x0c
    float field_0x10;            // 0x10
    br_vector3 field_0x14;       // 0x14
    br_vector3 field_0x20;       // 0x20
    br_vector3 field_0x2c;       // 0x2c
    br_vector3 field_0x38;       // 0x38
    tU8 field_0x44[0x18];        // 0x44
    int type;                    // 0x5c
} tQuickHinge_record;

extern int gUNK_006793d0;

float C2_HOOK_FASTCALL PhysicsObjectWeightedSquareSum(tPhysics_object* pObject);
float C2_HOOK_FASTCALL PhysicsObjectsWeightedSquareSum(tPhysics_object* pObject);
void C2_HOOK_FASTCALL UpdateChildTransforms(tPhysics_object* pObject, br_matrix34* pParent_matrix);
void C2_HOOK_FASTCALL FUN_004c4850(float* pA, int pMatrix, int pFrom, int pTo, float* pB, int pPivot_indices);

int C2_HOOK_FASTCALL RayCastInBox(br_vector3* pRay_pos, br_vector3* pRay_dir, br_vector3* pHit_pos, br_scalar* pT, tPhysics_object* pObject);

void C2_HOOK_FASTCALL FUN_0041e3f0(br_vector3* pRay_pos, br_vector3* pRay_dir, br_vector3* pHit_pos, br_scalar* pT, void* pRay_context);

// count

// DrMatrix34ApplyLPInverse

// AddObjectWallForce

extern int C2_HOOK_FASTCALL AddObjectObjectForce(tPhysics_joint_force* pForces, const br_vector3* pPoint_a, br_scalar pT, const br_vector3* pPoint_b, const br_vector3* pNormal, int pCount, tPhysics_object* pObject_a, tPhysics_object* pObject_b, int pType, void* pUnused, br_scalar pValue);

extern tFace_ref* C2_HOOK_FASTCALL RayCastThruFaceList(br_vector3* pPosition, br_vector3* pDirection, br_vector3* pNormal, br_scalar* pT, br_vector3* pCollision_position, tFace_ref* pFaces, int pCount);



// RayCastInBox

// FaceOffCount2

// FaceOffCount

// FaceOnCount

// EliminateFaces

extern void C2_HOOK_FASTCALL GetShapePoint(br_vector3* pPoint, tPhysics_shape* pShape, tU32 pIndex);

// GetObjectObjectEdgeIntersections

// GetFaceEdgeIntersection

// GetSphereToFacePoints

// CheckSingleEdgeWithSphere

// GetSphereToEdgePoints

// GetCornerToFacePoints

// CylinderCheck

// CheckTwoEdges

// WorldPointCollision

// CheckSingleEdgeWithObjectEdges

// GetEdgeToEdgePoints

// SetUpWorkspace

// GetObjectWallForceInfo

// GetObjectCornerToObjectFacePoints

// BoundsOverlapTest

// GetObjectObjectEdgePoints

// GetSphereNonSphereForceInfo

// GetSphereSphereForceInfo

// GetSphereShapeForceInfo

// GetNonSphereNonSphereForceInfo

// GetNonSphereShapeForceInfo

// GetObjectObjectForceInfo

// AddOldForces

// TestShapeEdgeIntersection

// TestObjectOverlap

// TestObjectOverlapIgnoreCloseRelatives

// TestObjectOverlapRecurse

// TestForObjectInSensiblePlace

// HierarchyCalcEnergy

// CheckCollisionList

// MakeJointVelocitiesVector

// ApplySingleForce

// ApplyJointForces

// SkipForces

// ScaleUpObjectMI

// CheckForTwang

// CreateJointForce

// CreateJointTorque

// AllocateObjectWorkspace

// RedoQuickHingeVelocities

// GetJointForces

// GetUniversalRotationAngle

// GetObjectsParent

// GetJointLimits

// GetListJointLimits

// GetGroupJointLimits

// LimitOmega

// CapOmega

// ListLimitOmega

// GroupLimitOmega

// PositionWorldJointedObject

// TranslateAndRotatePartialObject

// TranslateAndRotateJointedObject

// CountJointForces

// MoveJointedObject

// TellPhysicsJointsHaveChanged

// CombineBounds

// GetWorldBoundingBoxForObjects

// GetWorldBoundingBoxForObject

// MatricesAreIdentical

// InternalPrepareObject

// PrepareObject

// PrepareForCollisions

// CheckPointVelocities

// WriteNewPositions

// SetListDoingNothingFlag

// SetGroupDoingNothingFlag

// SetHierarchyDoingNothingFlag

// CheckDoingNothingFlag

// SetGroup

// GroupTogether

// GetPreCalculatedJointForces

// MakeObjectListDoSomething

// CheckForObjectTouchingAnotherObject

// CheckForObjectListTouchingAnotherObject

// CheckForObjectHierachyTouchingAnotherObject

// CheckForObjectHierachyTouchingObjectList

// MakeObjectDoSomething

// GetReducedList

// ReduceReducedList

// SetObjectTrialPositionType

// FillInTrialWorkspace

// FlagsAreGo

// CollideObjectWithReducedObjectList

// AddObjectToReducedList

// ReducedFindObjectCollisions

// FindObjectCollisions

// DoRigidJoints

// DoInterGroupCollisions

// ReduceEffectOfForces

// DoCollisions

// AddDoubleTorqueToMatrix

// DRMatrix33Inverse

// SetUpQuickHingeData

// InitQuickHinge

// CalcQuickHingeData

// SetUpForce

// SetUpTorqueHingePartners

// SetUpHingePartners

// MakeMatrix

// RestoreListOmega

// RestoreGroupOmegas

// MakeVelocitiesVector2

// MakeVelocitiesVector

// NewAddFrictionObjectObject

// NewAddFriction

// ApplyForces

// DoJointFriction

// ListDoJointFriction

// PostAddFriction

// RotateObjectSecondOrder

// RotateObjectFirstOrder

// RotateObject

// TranslateObject

// GetFacesNearObject

// ResetObjectAndChildren

// ResetObjectList

// RepositionObjectGroup

// SimpleRotate

// InterpolateSingleObject

// InterpolateObjects

// ApplyGravitySingleObject

// ApplyGravity

// SetGravityDeltaV

// WriteNewPositions

// InternalPositionChildren

// PositionChildren

// ApplyTorque

// ApplyDiminishingTorque

// OrderObjectListOnX

// SwapPair

// CheckObjectsPostionInList

// TestForWrongAnswer

// RQIterativeImprovement

// RQExpand

// RQSingleExpand

void C2_HOOK_FASTCALL CapOmega(tPhysics_object* pObject);


int C2_HOOK_FASTCALL FlagsAreGo(tPhysics_object* pObj1, tPhysics_object* pObj2);
// FUNCTION: CARMA2_HW 0x004ba550
int C2_HOOK_FASTCALL ScaleCollisionImpulses(tCollision_contact* pContacts, float* pImpulses, int pCount, int* pIndices, int pNum_impacts, float pThreshold);



// MakeIdentity

// SparseRQDcmpV

// MakeMatrixFromIndices

// HasBeenDoneBefore

// RemoveRQSingularities

// AddToMatrix

// RQRemovedOneTooManyForces

// GetInverseM

// RQMatrixSolveNoNegatives

// RQMatrixSolveWithNegatives

extern int C2_HOOK_FASTCALL MatrixProcess(br_scalar* pR, br_scalar* pQ, int pDim);

// RqBackSub

extern void* C2_HOOK_FASTCALL GetPhysicsScratchPointer(int pSize);

extern void C2_HOOK_FASTCALL IncreasePhysicsScratchPointer(int pSize);

// ClearPhysicsScratchSpace

// AllocateShapeSphere

// AllocateShapePolyhedron

// AllocateShapeWireFrame

// AllocateShapeBox

// GetJointSize

// AllocatePhysicsJoint

// ClonePhysicsJoint

// FreeShape

extern void C2_HOOK_FASTCALL FreeShapeList(tPhysics_shape *pShape);

extern void C2_HOOK_FASTCALL FreePhysicsJoint(tPhysics_joint* pJoint);
extern void C2_HOOK_FASTCALL CreateJointForce(tPhysics_object* pObject, tPhysics_object* pPartner, tPhysics_joint_force* pForce, const br_vector3* pDirection);


// GetBoundsFromPointList

// FillInShape

// UpdateCollisionObject

// PhysicsError

// PhysicsWarning

// PhysicsSetErrorHandler

// RepositionAndOrientateChildren

// PhysicsAddObject

// PhysicsAddObjectList

// PhysicsRemoveObject

// PhysicsObjectRecurse

// PhysicsObjectRecurseChildren

// InitPhysicsWorkspace

// FindFacesInBox

// GetFrictionFromFace

// DoPhysicsError

// ResetMan2

// SetUpTestObjects

// ResetObjectList

// ResetMan

extern void C2_HOOK_FASTCALL InitPhysics(void);
extern void C2_HOOK_FASTCALL PhysicsError(tPhysicsError pError, const char* pMessage);



// PointOutOfSightNotAR

// CalcCollObjIDFromCarSpec

// CalcCollObjID

// GetCarSpecFromCollObjID

// GetCollObjFromID

#define CARPOCALYPSE2_PHYSICS_H

#include "carpocalypse2_types.h"

#include "c2_hooks.h"

extern tPhysicsError_cbfn* gPhysics_error_cb;
extern void* gPhysics_scratch_ptr;


extern void* gPhysics_buffer1_ptr;

extern void* gPhysics_buffer2_ptr;

extern void* gPhysics_buffer3_ptr;

extern int gPhysics_other_buffer_capacity;

extern void* gPhysics_other_buffer;
extern void* gPhysics_other_buffer_cursor;


extern tU8 gTemporary_physics_render_buffer[300000];

extern int gCollision_info_uid_counter;

extern int gPHIL_enabled;

extern int gFace_num__car;

extern int gCrush_deferred;

extern int gPHIL_object_added;

extern tU32 gPHIL_last_physics_tick;

extern int gPHIL_mechanics_time_sync;

extern tWorld_callback_active_passive_cbfn* gPHIL_original_activate_passive;

extern tPhysics_callbacks* gPHIL_callbacks;

extern int gPHIL_count_list_collision_infos;

extern tPhysics_object* gSensible_place_blocker;

extern tPhysics_object* gList_collision_infos;

extern int gPHIL_munging_objects;

extern int gPHIL_count_queued_objects_for_removal;

extern tPhysics_object* gPHIL_queued_objects_for_removal[];

extern tPHIL_queued_objects gPhil_queued_objects;

#define NET_OBJECT_HIERARCHY_DEFAULT_SIZE 72

void C2_HOOK_FASTCALL DoPhysicsError(tPhysicsError pError, const char* pMessage);

void C2_HOOK_FASTCALL SetUpTestObjects(void);

void C2_HOOK_FASTCALL PhysicsSetErrorHandler(tPhysicsError_cbfn* pError_cbfn);

void C2_HOOK_FASTCALL InitPhysicsWorkspace(tU8* pBuffer, int pSize);

int C2_HOOK_FASTCALL PHILInit(void);

void C2_HOOK_FASTCALL PHILDisable(void);

tCollision_shape_box* C2_HOOK_FASTCALL AllocateBoxCollisionShape(int pType);
tCollision_shape_polyhedron* C2_HOOK_FASTCALL AllocateShapePolyhedron(int pCount_points, int pType);


tCollision_shape_sphere* C2_HOOK_FASTCALL AllocateShapeSphere(int pType);


tCollision_shape_polyhedron* C2_HOOK_FASTCALL AllocateShapeWireframePolyhedron(int pCount_points, int pType);

tCollision_shape_wireframe* C2_HOOK_FASTCALL AllocateWireFrameCollisionShape(int pcount_points, int pCount_lines, br_uint_8 pType);

void C2_HOOK_FASTCALL ReadMechanicsShapes(tPhysics_shape** pShape, FILE* pF);
int C2_HOOK_FASTCALL Collinear(const br_vector3* pV1, const br_vector3* pV2, const br_vector3* pV3);


int C2_HOOK_FASTCALL ArePointsColinear(const br_vector3* pV1, const br_vector3* pV2, const br_vector3* pV3);

void C2_HOOK_FASTCALL UpdateCollisionObject(tPhysics_object* pCollision_info);

tPhysicsError C2_HOOK_FASTCALL ProcessTetrahedronPolyhedronCollisionShape(tCollision_shape_polyhedron_data* pPolyhedron, tPolyhedron_edge_indexes* pEdges);

int C2_HOOK_FASTCALL PolyhedronCollisionShape_AddPoint(tCollision_shape_polyhedron_data* pPolyhedron, br_vector3* pVertex, tU8 pVertex_index, tPolyhedron_edge_indexes* pEdge_to_plane_indices);
br_scalar C2_HOOK_FASTCALL SignedTwiceAreaOf3DTriangle(const br_vector3* pV1, const br_vector3* pV2);


br_scalar C2_HOOK_FASTCALL ComparePolyhedronPlaneToNormal(const br_vector3* pV1, const br_vector3* pV2);

int C2_HOOK_CDECL ComparePolyhedronPointIndicesToNormal(const int* pIndex1, const int* pIndex2);

tPhysicsError C2_HOOK_FASTCALL AddPolyhedronCollisionShapePlanes(tCollision_shape_polyhedron_data* pPolyhedron);

tPhysicsError C2_HOOK_FASTCALL ConvexHull3D(tCollision_shape_polyhedron_data* pPolyhedron);

int C2_HOOK_FASTCALL FUN_00422030(br_model* pModel, int pCount_vertices, int* pVertex_indices);

int C2_HOOK_FASTCALL FUN_004222c0(int* pOut_point_count, int* pVertex_indices, br_model* pModel, int pMin_point_count, int pMax_point_count);
tPhysicsError C2_HOOK_FASTCALL FUN_00422400(tCollision_shape_polyhedron_data* pPolyhedron, int* pVertex_indices, br_model* pModel, int pCount_vertices);
int C2_HOOK_FASTCALL FUN_00422500(tCollision_shape_polyhedron_data* pPolyhedron, br_model* pModel, int pMin_point_count, int pMax_point_count, int* pVertex_indices);

void C2_HOOK_FASTCALL GetBoundsFromPointList(const br_vector3* pVertices, int pCount_vertices, br_bounds3* pBounds);

void C2_HOOK_FASTCALL FillInShape(tPhysics_shape* pShape);

tPhysics_object* C2_HOOK_FAKE_THISCALL MungeSphereObject(br_model* pModel, undefined4 pArg2, float pWeight);

void C2_HOOK_FASTCALL WriteNewPositions(tPhysics_object* pObject);

void C2_HOOK_FASTCALL PositionChildren(tPhysics_object *pCollision_info);

void C2_HOOK_FASTCALL InternalPositionChildren(tPhysics_object *pParent, tPhysics_object *pRoot);

int C2_HOOK_CDECL PHILSetObjectProperty(tPhysics_object *pCollision_info, int pParam, ...);

float C2_HOOK_FASTCALL PHILGetObjectProperty(tPhysics_object *pCollision_info, int pParam);

void C2_HOOK_FASTCALL SetCollisionInfoDoNothingChain(tPhysics_object *pCollision_info, tU32 pDisable);

void C2_HOOK_FASTCALL SetCollisionInfoDoNothing(tPhysics_object *pCollision_info, tU32 pDisable);

void C2_HOOK_FASTCALL SetCollisionInfoChildsDoNothing(tPhysics_object *pCollision_info, tU32 pDisable);

int C2_HOOK_FASTCALL PHILRemoveObject(tPhysics_object* pObject);

void C2_HOOK_FASTCALL AddCollisionInfoChild(tPhysics_object* pParent, tPhysics_object* pChild);

int C2_HOOK_FASTCALL PhysicsObjectRecurse(tPhysics_object* pCollision_info, tEnumCollision_cbfn* pCallback, void* pUser_data);

typedef struct tShape_overlap_work {
    float factor;
    float relative[3];
    float transformed[3];
    br_bounds3 expanded_bounds;
    br_vector3 hit_position;
    br_vector3 vertices[64];
} tShape_overlap_work;

int C2_HOOK_FASTCALL TestObjectOverlapRecurse(tPhysics_object* pCollision_1, tPhysics_object* pCollision_2, tPhysics_shape* pShape_1, tPhysics_shape* pShape_2);

int C2_HOOK_FASTCALL TestForObjectInSensiblePlace(tPhysics_object* pCollision_info_1, tPhysics_object* pCollision_info_2, br_vector3* pVec3, tWorld_callbacks* pCar_callbacks);
int C2_HOOK_FASTCALL TestShapeOverlap(tPhysics_object* pObjA, tPhysics_object* pObjB, tPhysics_shape* pShapeA, tPhysics_shape* pShapeB);


int C2_HOOK_FASTCALL FUN_004c2d60(tPhysics_object* pObject, tWorld_callbacks* pCar_callbacks);

int C2_HOOK_FASTCALL FUN_004228b0(tPhysics_object* pObject, void* pCollision_buffer, int pIndex);

int C2_HOOK_FASTCALL FUN_004293c0(tPhysics_object* pObject1, tPhysics_object* pObject2);

void C2_HOOK_FASTCALL FUN_00429490(tPhysics_object* pObject);

int C2_HOOK_FASTCALL CanPhysicsObjectsCollide(volatile tPhysics_object* pObject1, volatile tPhysics_object* pObject2);


int C2_HOOK_FASTCALL FUN_00423320(tPhysics_object* pObject, tPhysics_shape* pShape, br_matrix34* pMatrix);

tPhysics_object* C2_HOOK_FAKE_THISCALL MungeBoxObject(br_model* pModel, undefined4 pArg2, float pMass);

tPhysics_joint* C2_HOOK_FASTCALL AllocatePhysicsJoint(int pCount_limits, int pType);

tPhysics_joint* C2_HOOK_FASTCALL ClonePhysicsJoint(const tPhysics_joint* pJoint, int pType);

int C2_HOOK_FASTCALL FUN_004c6200(tPhysics_object* pObject, tPhysics_object* pBuffer, int pRemaining);

// FIXME: unsure about these helpers' real names; addressed by CARMA2_HW offset.
void C2_HOOK_FASTCALL PHILRemoveFromParentChain(tPhysics_object* pObject);
void C2_HOOK_FASTCALL PHILDetachObjectState(tPhysics_object* pObject);
int C2_HOOK_FASTCALL FUN_004b9650(tPhysics_object* pObject, tPhysics_object* pParent_context);
int C2_HOOK_FASTCALL PHILQueueDetachBit(tPhysics_object* pObject, void* pA, const br_vector3* pB, const br_vector3* pC);
void C2_HOOK_FASTCALL PHILSendDetachBit(int pFlags, void* pBit);

void C2_HOOK_FASTCALL PhysicsAddObject(tPhysics_object* pParent, tPhysics_object* pChild);

tPhysics_object* C2_HOOK_FASTCALL PHILGetFirstObject(void);

tPhysics_object* C2_HOOK_FASTCALL PHILGetNextObject(tPhysics_object* pCollision_info);

void C2_HOOK_FASTCALL InternalPrepareObject(tPhysics_object* pObject);

void C2_HOOK_FASTCALL FUN_004b73a0(tPhysics_object* pObject, const br_matrix34* pParent_matrix, const br_matrix34* pRoot_matrix);

void C2_HOOK_FAKE_THISCALL MoveJointedObject(tPhysics_object* pObject, undefined4 pArg2, float pDelta_time);

void C2_HOOK_FAKE_THISCALL FUN_004b8fe0(tPhysics_object* pObject, undefined4 pArg2, float pDelta_time);

void C2_HOOK_FAKE_THISCALL FUN_004b9230(tPhysics_object* pObject, tPhysics_object** ppChanged_object, float pDelta_time);

void C2_HOOK_FAKE_THISCALL RotateObject(tPhysics_object* pObject, undefined4 pArg2, float pDelta_time);

void C2_HOOK_FAKE_THISCALL RotateObjectFirstOrderRigid(tPhysics_object* pObject, undefined4 pArg2, float pDelta_time);

void C2_HOOK_FASTCALL MergeBoundsIntoParent(tPhysics_object* pObject, br_bounds3* pParent);

void C2_HOOK_FASTCALL MakeObjectListDoSomething(tPhysics_object* pObject);

int C2_HOOK_FASTCALL CheckForObjectHierachyTouchingAnotherObject(tPhysics_object* pObject, tPhysics_object* pList, tPhysics_object* pList_original);

int C2_HOOK_FASTCALL PHILMakeObjectActive(tPhysics_object* pObject, const br_vector3* pVel, const br_vector3* pOmega, int pArg4);

void C2_HOOK_FASTCALL CheckForObjectHierachyTouchingObjectList(tPhysics_object* pObject, tPhysics_object* pList, tPhysics_object* pList_original);

int C2_HOOK_FASTCALL PHILAddActiveObject(tPhysics_object* pInfo, undefined4* pArg2, const br_vector3* pArg3, const br_vector3* pArg4);

void C2_HOOK_FASTCALL SwapPair(tPhysics_object* pObj1, tPhysics_object* pObj2);

void C2_HOOK_FASTCALL CheckObjectsPostionInList(tPhysics_object* pObject, tPhysics_object** pList);

void C2_HOOK_FASTCALL MakeObjectDoSomething(tPhysics_object* pObject, tPhysics_object* pList);

void C2_HOOK_FASTCALL FUN_004be0e0(tPhysics_object* pObject, tPhysics_object* pList);

void C2_HOOK_FASTCALL AddObjectToReducedList(tPhysics_object* pObject, tPhysics_object** pReduced_list);

void C2_HOOK_FASTCALL PrepareObject(tPhysics_object* pObject, tPhysics_object** pList);

int C2_HOOK_FASTCALL PHILAddObject(tPhysics_object* pObject);

void C2_HOOK_FASTCALL PHILDoPhysics(tPhysics_callbacks* pCallbacks, tU32 pLast_tick_time, tU32 pFrame_period);

int C2_HOOK_FASTCALL TimeToSendData(void);

void C2_HOOK_FASTCALL PHILActivatePassive(tPhysics_object* pObject, tPhysics_object* pActivator);

void C2_HOOK_FASTCALL TestAutoSpecialVolume(tPhysics_object* pObject);

void C2_HOOK_FASTCALL MungeSpecialVolume(tPhysics_object* pObject);

void C2_HOOK_FASTCALL ProcessGravity(tPHIL_queued_header* pObject_info, tPhysics_object* pObject, float pGravity);

void C2_HOOK_FASTCALL ProcessDrag2(tPHIL_queued_header* pObject_info, tPhysics_object* pObject, float pDrag, int pObject_info_flags, tSpecial_volume* pSpecial_volume);

void C2_HOOK_FASTCALL ProcessDrag(tPHIL_queued_header* pObject_info, tPhysics_object* pObject, float pDrag);

void C2_HOOK_FASTCALL ApplyWaterOmegaBrake(tPhysics_object* pObject, const br_vector3* pV);

void C2_HOOK_FASTCALL PhysicsObjectSetImpulse(tPhysics_object* pObject, br_vector3* pImpulse);

void C2_HOOK_FASTCALL PhysicsObjectMoveVelocity(tPhysics_object* pObject);

void C2_HOOK_FASTCALL PhysicsObjectApplyTorque(tPhysics_object* pObject, const br_vector3* pTorque);

void C2_HOOK_FASTCALL PhysicsObjectMoveVelocityList(tPhysics_object* pObject);

void C2_HOOK_FASTCALL LevelOutOnSurface(tPhysics_object *pObject);

void C2_HOOK_FASTCALL MarkObjectAndChildrenAsPassive(tPhysics_object* pObject);

void C2_HOOK_FASTCALL DoCollisions(tPhysics_object** pObject_list, tWorld_callbacks* volatile pWorld_callbacks);

void C2_HOOK_FASTCALL ClearPhysicsScratchSpace(void);
undefined4 C2_HOOK_FASTCALL FUN_004c5830(int pMatrix, int pSize, undefined4* pQMatrix);

void C2_HOOK_FASTCALL PhysicsWarning(const char* pMessage);


int C2_HOOK_FASTCALL AddDoubleTorqueToMatrix(tPhysics_object* pObject);

void C2_HOOK_FASTCALL DRMatrix33Inverse(tPhysics_object* pObject, int pFlag);

int C2_HOOK_FASTCALL SetUpQuickHingeData(tPhysics_object** pObject_list, tPhysics_object* pObject, void* pBuffer1, void* pBuffer2, int pArg, tWorld_callbacks* pWorld_callbacks);

int C2_HOOK_FASTCALL CopyPhysicsJointData(tPhysics_joint_data* pDestination, int pDestination_index, const tPhysics_joint_data* pSource, int pCount);

void C2_HOOK_FASTCALL FUN_004bc190(int* pObject_list, float* pVectors, int pCount, int pUnused);
int C2_HOOK_FASTCALL CollectJointLimitsIntoBuffer(tPhysics_object* pObject, tQuickHinge_record* pRecords, int pCount, int pMaxRecords);
void C2_HOOK_FASTCALL FUN_004bd4f0(const br_matrix34* pTransform, const br_matrix34* pInverse_transform, const br_vector3* pVector1, const br_vector3* pVector2, const br_vector3* pVector3, const br_vector3* pVector4, br_vector3* pVector5, br_vector3* pVector6);
int C2_HOOK_FASTCALL CollectJointLimitsOfTree(tPhysics_object* pObject, void* pCollision_data, int pCollision_count, int pCollision_capacity);

tPhysics_object** C2_HOOK_FASTCALL AppendOverlappingObjects(tPhysics_object* pObject, tPhysics_object* pList, tPhysics_object** pObjects);

void C2_HOOK_FASTCALL SetFieldF0Tree(tPhysics_object* pObject, int pValue);
void C2_HOOK_FASTCALL FUN_004bddd0(tPhysics_object* pObject1, tPhysics_object* pObject2);

void C2_HOOK_FASTCALL FUN_004be4b0(float* pNormal, float* pScaledVector, const float* pVector1, const float* pVector2, tPhysics_object* pObject);

tPhysics_object* C2_HOOK_FASTCALL FUN_004be550(br_vector3* pAngular_delta, br_vector3* pLinear_delta, br_vector3* pAngular_delta_other, br_vector3* pLinear_delta_other, const br_vector3* pImpulse, tPhysics_object* pObject);
tPhysics_object* C2_HOOK_FASTCALL FUN_004becf0(br_vector3* pAngular_delta, br_vector3* pLinear_delta, br_vector3* pAngular_delta_other, br_vector3* pLinear_delta_other, tPhysics_object* pObject);

void C2_HOOK_FAKE_THISCALL FUN_004b7960(tPhysics_joint_data* pJoint_data, float pDt);

void C2_HOOK_FASTCALL FUN_004b77b0(int* pDestination, int* pJoint_data, int pCount);
void C2_HOOK_FASTCALL FUN_004be350(float* pDestination, const float* pSource);
void C2_HOOK_FASTCALL FUN_004bf490(int* pDestination, int pCount, int* pJoint_data, int pJoint_count);
void C2_HOOK_FASTCALL FUN_004c53b0(float* pMatrix, float* pData, float* pJoint_data, float* pJoint_dt, int pCount);
void C2_HOOK_FASTCALL FUN_004b8cf0(tPhysics_object* pObject);
void C2_HOOK_FAKE_THISCALL FUN_004bc8e0(tPhysics_object* pObject, br_scalar pScale);
void C2_HOOK_FAKE_THISCALL FUN_004b8ee0(tPhysics_object* pObject, undefined4 pArg2, float pDelta_time);

void C2_HOOK_FASTCALL ResetOmegaTree(tPhysics_object* pObject);
int C2_HOOK_FASTCALL FUN_004c0430(float* pFactors, void* pContact_list, int pCapacity, int pCount);
int C2_HOOK_FASTCALL ClampObjectSpin(tPhysics_object* pObject);
void C2_HOOK_FASTCALL FUN_004c1840(volatile tPhysics_object* pObject);

void C2_HOOK_FASTCALL RotateObjectFirstOrder(void* pContact, tWorld_callbacks* pWorld_callbacks);

void C2_HOOK_FASTCALL TranslateObject(tPhysics_object* pObject);

int C2_HOOK_FASTCALL PHILSetPassiveObjectsMatrix(tPhysics_object* pObject, br_matrix34* pMatrix);

void C2_HOOK_FAKE_THISCALL RepositionObjectGroup(tPhysics_object* pObject, float pDt);

void C2_HOOK_FAKE_THISCALL InterpolateSingleObject(tPhysics_object* pObject, float pDt);

void C2_HOOK_FAKE_THISCALL FUN_004c28a0(tPhysics_object* pObject, float pDt);

void C2_HOOK_FAKE_THISCALL InterpolateObjects(tPhysics_object* pObject, float pDt);

void C2_HOOK_FASTCALL ChangedObjectsCallbacks(tPhysics_object* pObjects, tPhysics_callbacks* pCallbacks, tU32 pPeriod);

void C2_HOOK_FASTCALL ResetObjectList(tPhysics_object* pObjects);

void C2_HOOK_FASTCALL StopObjectList(tPhysics_object* pObjects);

int C2_HOOK_FASTCALL PhysicsObjectRecurseChildren(tPhysics_object* pObject, tEnumCollision_cbfn* pCallback, void* pContext);

int C2_HOOK_FASTCALL GetObjectNetworkStuff(tPhysics_object* pObject, tU8* pBuffer, int pSize);

int C2_HOOK_FASTCALL SetObjectNetworkStuff(const tU8* pBuffer, tPhysics_object* pObject, int pMode);

void C2_HOOK_FASTCALL FUN_0049cb00(tPhysics_object* pObject, br_matrix34* pMatrix, int pParent_matrix);

int C2_HOOK_FASTCALL GetHierarchyNetworkStuff(tPhysics_object* pObject, tU8* pNet_data, int pRemaining);

int C2_HOOK_FASTCALL SizeOfObjectNetworkStuff(tPhysics_joint_type pType);

int C2_HOOK_FASTCALL GetHierarchyNetworkSize(tPhysics_object* pObject);




int C2_HOOK_FASTCALL CollectPlacementFaces(tPhysics_object* pObject, tCollision_contact* pContacts, int pCount);

int C2_HOOK_FASTCALL CullPlacementFaces(tPhysics_object* pObject, tPhysics_shape* pShape, br_matrix34* pMat);

int C2_HOOK_FASTCALL TestPairOverlap(tPhysics_object* pObj1, tPhysics_object* pObj2);


int C2_HOOK_CDECL CompareAngle(const void* pIndex1, const void* pIndex2);




extern void* gPhysics_scratch_ptr;

extern char gPhysics_empty_message;

extern void C2_HOOK_FASTCALL CreateJointTorque(tPhysics_object* pObject, tPhysics_object* pPartner, tPhysics_joint_force* pForce, const br_vector3* pDirection);


#endif // GUARD_PHYSICS_H
