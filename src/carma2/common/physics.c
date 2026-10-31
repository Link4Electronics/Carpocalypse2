#include "physics.h"

#include "pedestrn.h"

#include "crush.h"

#include "car.h"
#include "errors.h"
#include "globvars.h"
#include "globvrpb.h"
#include "loading.h"
#include "netgame.h"
#include "opponent.h"
#include "piping.h"
#include "platform.h"
#include "powerups.h"

#include "raycast.h"

#include "sound.h"
#include "spark.h"
#include "utility.h"
#include "world.h"
#include "finteray.h"

#include <brender/brender.h>

extern const br_scalar gFLOAT_005899a8;
extern const double gDouble_005899b0;

extern br_quat* C2_HOOK_CDECL BrQuatNormalise(br_quat* q, const br_quat* qq);
extern br_matrix34* C2_HOOK_CDECL BrQuatToMatrix34(br_matrix34* mat, const br_quat* q);

#include <stdarg.h>

extern const br_scalar gFLOAT_005899a0;
extern const br_scalar gFLOAT_005899a4;

#ifdef CARPOCALYPSE2_MATCHING
#if defined(_MSC_VER)
#define C2_MATCHING_INLINE __inline
#else
#define C2_MATCHING_INLINE inline
#endif
#else
#define C2_MATCHING_INLINE
#endif
#include "c2_stdlib.h"
#include "c2_string.h"

#include "carpocalypse2_macros.h"
#include "c2_math.h"

typedef struct {
    int count;
    tCollision_contact* data;
    undefined field_0x8[0x14 - 0x8];
    br_scalar* field_0x14;
    br_scalar* field_0x18;
} tPhysics_group_data;

extern const char gEmpty_string_00676920[];

extern const br_scalar gFLOAT_0058a3f8;
extern const br_scalar gFLOAT_0058a40c;
extern const br_scalar gFLOAT_0058a8b8;
extern const double gDOUBLE_0058a8c0;

// GLOBAL: CARMA2_HW 0x00589954
const br_scalar gFLOAT_00589954 = 0.0f;

// GLOBAL: CARMA2_HW 0x0058a414
const br_scalar gFLOAT_0058a414 = 39.2699f;

// GLOBAL: CARMA2_HW 0x0058a420
const br_scalar gFLOAT_0058a420 = 78.5398f;

#define PHYSICS_BUFFER_PART_SIZE 40000
#define PHYSICS_BUFFER_OTHER_SIZE 50000
#include "physics.h"

// count

// DrMatrix34ApplyLPInverse

// AddObjectWallForce

// FUNCTION: CARMA2_HW 0x004268a0
int C2_HOOK_FASTCALL AddObjectObjectForce(tPhysics_joint_force* pForces, const br_vector3* pPoint_a, br_scalar pT, const br_vector3* pPoint_b, const br_vector3* pNormal, int pCount, tPhysics_object* pObject_a, tPhysics_object* pObject_b, int pType, void* pUnused, br_scalar pValue) {
    int i;
    int j;
    tPhysics_joint_force* force;

    (void)pUnused;
    if (pCount < 4) {
        pForces[pCount].field_0x0 = 2.f;
        pCount++;
    }
    for (i = 0, force = pForces; i < pCount; i++, force++) {
        if (pT < force->field_0x0) {
            for (j = MIN(3, pCount); j >= i; j--) {
                pForces[j] = pForces[j - 1];
            }
            force->field_0x10 = pValue;
            force->object_a = pObject_a;
            force->object_b = pObject_b;
            BrVector3Sub(&force->point_a, pPoint_a, &pObject_a->cmpos);
            BrVector3Sub(&force->point_b, pPoint_b, &pObject_b->cmpos);
            BrMatrix34TApplyV(&force->direction_a, pNormal, &pObject_a->transform_matrix);
            BrMatrix34TApplyV(&force->direction_b, pNormal, &pObject_b->transform_matrix);
            force->field_0x0 = pT;
            force->type = pType;
            force->field_0xc = NULL;
            return pCount;
        }
    }
    return pCount;
}

// FUNCTION: CARMA2_HW 0x004225f0
tFace_ref* C2_HOOK_FASTCALL RayCastThruFaceList(br_vector3* pPosition, br_vector3* pDirection, br_vector3* pNormal, br_scalar* pT, br_vector3* pCollision_position, tFace_ref* pFaces, int pCount) {
    br_vector3 normal;
    br_vector3 collision_position;
    br_scalar t;
    tFace_ref* best;
    int i;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFace_ref, flags, 0x40);

    *pT = 2.f;
    for (i = 0; i < pCount; i++, pFaces++) {
        if ((pFaces->flags & 0x80) == 0) {
            CheckSingleFace(pFaces, pPosition, pDirection, &normal, &t, &collision_position);
            if (t < *pT) {
                *pT = t;
                BrVector3Copy(pNormal, &normal);
                best = pFaces;
                BrVector3Copy(pCollision_position, &collision_position);
            }
        }
    }
    if (*pT < 2.f) {
        return best;
    } else {
        return NULL;
    }
}

// FaceOffCount2

// FaceOffCount

// FaceOnCount

// EliminateFaces

// FUNCTION: CARMA2_HW 0x00428480
void C2_HOOK_FASTCALL GetShapePoint(br_vector3* pPoint, tPhysics_shape* pShape, tU32 pIndex) {
    switch (pShape->common.type) {
    case kCollisionShapeType_Box:
        BrVector3Set(pPoint,
            (pIndex & 0x2) ? pShape->common.bb.min.v[0] : pShape->common.bb.max.v[0],
            (pIndex & 0x1) ? pShape->common.bb.min.v[1] : pShape->common.bb.max.v[1],
            (pIndex & 0x4) ? pShape->common.bb.min.v[2] : pShape->common.bb.max.v[2]);
        break;
    case kCollisionShapeType_Wireframe:
        BrVector3Copy(pPoint, &pShape->wireframe.wireframe.points[pIndex]);
        return;
    case kCollisionShapeType_Polyhedron:
    case kCollisionShapeType_Wireframe_Polyhedron:
        BrVector3Copy(pPoint, &pShape->polyhedron.polyhedron.points[pIndex]);
        return;
    default:
        return;
    }
}

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

// FUNCTION: CARMA2_HW 0x004b8c20
void C2_HOOK_FASTCALL CreateJointForce(tPhysics_object* pObject, tPhysics_object* pPartner, tPhysics_joint_force* pForce, const br_vector3* pDirection) {
    br_vector3 direction;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint_force, field_0x0, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint_force, object_a, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint_force, object_b, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint_force, field_0xc, 0xc);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint_force, point_a, 0x14);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint_force, point_b, 0x20);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint_force, direction_a, 0x2c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint_force, direction_b, 0x38);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint_force, type, 0x5c);

    pForce->field_0x0 = 0.f;
    pForce->object_a = pObject;
    pForce->object_b = pPartner;
    pForce->field_0xc = NULL;
    BrVector3Sub(&pForce->point_a, &pObject->physics_joint1->field_0x08, &pObject->cmpos);
    BrVector3Sub(&pForce->point_b, &pObject->physics_joint1->field_0x14, &pPartner->cmpos);
    BrVector3Copy(&pForce->direction_a, pDirection);
    BrMatrix34ApplyV(&direction, pDirection, &pObject->transform_matrix);
    BrMatrix34TApplyV(&pForce->direction_b, &direction, &pPartner->transform_matrix);
    pForce->type = 10;
}

// FUNCTION: CARMA2_HW 0x004b8e10
void C2_HOOK_FASTCALL CreateJointTorque(tPhysics_object* pObject, tPhysics_object* pPartner, tPhysics_joint_force* pForce, const br_vector3* pDirection) {
    br_vector3 direction;

    pForce->field_0x0 = 0.f;
    pForce->object_a = pObject;
    pForce->object_b = pPartner;
    BrVector3Sub(&pForce->point_a, &pObject->physics_joint1->field_0x08, &pObject->cmpos);
    BrVector3Sub(&pForce->point_b, &pObject->physics_joint1->field_0x14, &pPartner->cmpos);
    BrVector3Copy(&pForce->direction_a, pDirection);
    BrMatrix34ApplyV(&direction, &pForce->direction_a, &pObject->transform_matrix);
    BrMatrix34TApplyV(&pForce->direction_b, &direction, &pPartner->transform_matrix);
    pForce->type = 11;
}

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

// FUNCTION: CARMA2_HW 0x004c2a10
void C2_HOOK_FASTCALL WriteNewPositions(tPhysics_object* pObject) {
    tPhysics_object* child;
    tPhysics_object* root;
    br_vector3 parent_point;
    br_vector3 child_point;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, field_0x1e8, 0x1e8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, field_0x230, 0x230);

    root = pObject;
    child = pObject->child;
    if (pObject->flags & 0x400) {
        root = pObject->field_0x230;
    }
    for (; child != NULL; child = child->next) {
        if (!(pObject->flags & 0x400)) {
            BrMatrix34ApplyP(&parent_point, &child->physics_joint1->field_0x14, &pObject->actor->t.t.mat);
            BrMatrix34ApplyP(&child_point, &child->physics_joint1->field_0x08, &child->actor->t.t.mat);
            BrVector3Sub(&parent_point, &parent_point, &child_point);
            BrVector3Accumulate((br_vector3*)child->actor->t.t.mat.m[3], &parent_point);
        } else {
            BrMatrix34Mul(&pObject->actor->t.t.mat, &pObject->field_0x1e8, &root->actor->t.t.mat);
        }
        WriteNewPositions(child);
    }
}

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

// FUNCTION: CARMA2_HW 0x004c1f10
void C2_HOOK_FAKE_THISCALL RotateObjectFirstOrderRigid(tPhysics_object* pObject, undefined4 pUnused, br_scalar pTime) {
    br_matrix34 rotation;
    br_vector3 axis;
    br_vector3 momentum;
    br_vector3 rotated;
    br_scalar length;
    br_scalar angle;

    CARPOCALYPSE2_THISCALL_UNUSED(pUnused);

    length = BrVector3Length(&pObject->omega);
    angle = length * pTime;
    if (angle < 0.0001) {
        return;
    }
    axis.v[0] = pObject->omega.v[0] / length;
    axis.v[1] = pObject->omega.v[1] / length;
    axis.v[2] = pObject->omega.v[2] / length;
    momentum.v[0] = pObject->I.v[0] * pObject->omega.v[0];
    momentum.v[1] = pObject->I.v[1] * pObject->omega.v[1];
    momentum.v[2] = pObject->I.v[2] * pObject->omega.v[2];
    BrMatrix34Rotate(&rotation, BrRadianToAngle(angle), &axis);
    BrMatrix34TApplyV(&rotated, &momentum, &rotation);
    BrMatrix34PreTranslate(&rotation, -pObject->cmpos.v[0], -pObject->cmpos.v[1], -pObject->cmpos.v[2]);
    BrMatrix34PostTranslate(&rotation, pObject->cmpos.v[0], pObject->cmpos.v[1], pObject->cmpos.v[2]);
    BrMatrix34Pre(&pObject->actor->t.t.mat, &rotation);
    pObject->omega.v[0] = rotated.v[0] / pObject->I.v[0];
    pObject->omega.v[1] = rotated.v[1] / pObject->I.v[1];
    pObject->omega.v[2] = rotated.v[2] / pObject->I.v[2];
}

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

// FUNCTION: CARMA2_HW 0x004c5b50
int C2_HOOK_FASTCALL MatrixProcess(br_scalar* pR, br_scalar* pQ, int pDim) {
    return FUN_004c5830((int)pR, pDim, (undefined4*)pQ);
}

// FUNCTION: CARMA2_HW 0x004c5b60
void C2_HOOK_FASTCALL RqBackSub(const br_scalar* pR, const br_scalar* pQ, const br_scalar* pRhs, br_scalar* pX, int pDim) {
    br_scalar intermediate[100];
    br_scalar sum;
    int i;
    int j;

    /* Solve Ax = b, with A represented by its RQ decomposition, where:
     * - R is a lower-diagonal matrix
     * - Q is an orthonormal matrix
     * with:
     * - all matrices (A, Q, R) are of size N x N (with N = pDim)
     * - vectors x, t are of size N x 1
     * A, Q, R matrices are of size [pDim x pDim]
     */

    C2_HOOK_ASSERT(pDim > 0 && pDim <= (int)CARPOCALYPSE2_ASIZE(intermediate));

    /* Use back substitution to solve Ry = b, with R a lower-diagonal matrix */
    intermediate[0] = pRhs[0] / pR[0 * pDim + 0];             /*  T[0] = B[0] / R[0, 0] */
    for (i = 1; i < pDim; i++) {
        sum = 0.0f;
        for (j = 0; j < i; j++) {
            sum += intermediate[j] * pR[i * pDim + j];        /*  sum += T[j] * R[i, j] */
        }
        intermediate[i] = (pRhs[i] - sum) / pR[i * pDim + i]; /*  T[i] = (B[i] - sum) / R[i, i] */
    }
    /* Solve Qx = T, by multiplying both sides with Q transposed */
    for (i = 0; i < pDim; i++) {
        pX[i] = pQ[0 * pDim + i] * intermediate[0];           /*  X[i]  = Q[0, i] * T[0] */
        for (j = 1; j < pDim; j++) {
            pX[i] += intermediate[j] * pQ[j * pDim + i];      /*  X[i] += Q[j, i] * T[j] */
        }
    }
}

// FUNCTION: CARMA2_HW 0x004c5c70
void* C2_HOOK_FASTCALL GetPhysicsScratchPointer(int pSize) {

    if ((tU8*)gPhysics_scratch_ptr + pSize > (tU8*)gPhysics_other_buffer + gPhysics_other_buffer_capacity) {
        PhysicsError(ePhysicsError_OutOfMemory, NULL);
    }
    return gPhysics_scratch_ptr;
}

// FUNCTION: CARMA2_HW 0x004c5cb0
void C2_HOOK_FASTCALL IncreasePhysicsScratchPointer(int pSize) {

    if ((tU8*)gPhysics_scratch_ptr + pSize > (tU8*)gPhysics_other_buffer + gPhysics_other_buffer_capacity) {
        PhysicsError(ePhysicsError_OutOfMemory, NULL);
    }
    gPhysics_scratch_ptr = (tU8*)gPhysics_scratch_ptr + pSize;
}

// C2PORT-NONMATCHING: best 14.29% | difftest PASS 2000 trials (spec difftest/specs/004c53b0_FUN_004c53b0.json) | reason original inlines the QR and solve loops
// FUNCTION: CARMA2_HW 0x004c53b0
void C2_HOOK_FASTCALL FUN_004c53b0(float* pMatrix, float* pData, float* pJoint_data, float* pJoint_dt, int pCount) {
    FUN_004c5830((int)pMatrix, pCount, (undefined4*)pData);
    if (pJoint_data == NULL || pJoint_dt == NULL) {
        return;
    }
    RqBackSub(pMatrix, pData, pJoint_data, pJoint_dt, pCount);
}

// ClearPhysicsScratchSpace

// AllocateShapeSphere

// AllocateShapePolyhedron

// AllocateShapeWireFrame

// AllocateShapeBox

// GetJointSize

// AllocatePhysicsJoint

// ClonePhysicsJoint

typedef enum {
    ePlaneSide_positive = 0,
    ePlaneSide_negative = 1,
    ePlaneSide_zero = 2,
} tPlane_side;

typedef int C2_HOOK_FASTCALL tPhysicsObject_cbfn(tPhysics_object* pObject, void* pContext);
// GLOBAL: CARMA2_HW 0x006940bc
void* gPhysics_scratch_ptr;

char gPhysics_empty_message;

// GLOBAL: CARMA2_HW 0x006940c0
tPhysicsError_cbfn* gPhysics_error_cb;

// GLOBAL: CARMA2_HW 0x00744818
void* gPhysics_buffer1_ptr;

// GLOBAL: CARMA2_HW 0x00744814
void* gPhysics_buffer2_ptr;

// GLOBAL: CARMA2_HW 0x0074a5e0
void* gPhysics_buffer3_ptr;

// GLOBAL: CARMA2_HW 0x006940bc
void* gPhysics_other_buffer_cursor;

// GLOBAL: CARMA2_HW 0x006940c8
int gPhysics_other_buffer_capacity;

// GLOBAL: CARMA2_HW 0x006940c4
void* gPhysics_other_buffer;

// GLOBAL: CARMA2_HW 0x006a0adc
int gCollision_info_uid_counter;

extern const float gFLOAT_0058ac00;

// GLOBAL: CARMA2_HW 0x0058ac04
const float gFLOAT_0058ac04 = 0.0833333358168602f;

// GLOBAL: CARMA2_HW 0x0065d010
int gFace_num__car = 1;

// GLOBAL: CARMA2_HW 0x00679438
int gSensible_place_collect_active;

// GLOBAL: CARMA2_HW 0x0079ecbc
tPhysics_object* gSensible_place_blocker;
// GLOBAL: CARMA2_HW 0x00679420
br_vector3 gPhysics_reference_normal_comparison;

// GLOBAL: CARMA2_HW 0x0067942c
tCollision_shape_polyhedron_data* gPolyhedron_to_sort;

// GLOBAL: CARMA2_HW 0x006923f0
tPHIL_queued_objects gPhil_queued_objects = { 0 };

// GLOBAL: CARMA2_HW 0x00692db8
tWorld_callback_active_passive_cbfn* gPHIL_original_activate_passive = NULL;

// GLOBAL: CARMA2_HW 0x006923e0
int gPHIL_count_list_collision_infos;

// GLOBAL: CARMA2_HW 0x00692dc0
int gPHIL_count_queued_objects;

// GLOBAL: CARMA2_HW 0x00692dbc
int gPHIL_count_queued_objects_for_removal;

// GLOBAL: CARMA2_HW 0x0074a5f0
tPhysics_object* gList_collision_infos;

// GLOBAL: CARMA2_HW 0x0074a5e8
int gPHIL_munging_objects;

// GLOBAL: CARMA2_HW 0x0074a5f8
int gCrush_deferred;

// GLOBAL: CARMA2_HW 0x006923e4
int gPHIL_object_added;

// GLOBAL: CARMA2_HW 0x0074a5ec
tU32 gPHIL_last_physics_tick;

// GLOBAL: CARMA2_HW 0x0074a5e4
int gPHIL_mechanics_time_sync;

// GLOBAL: CARMA2_HW 0x00692dcc
int gPrepared_objects;

// GLOBAL: CARMA2_HW 0x006940a0
int gINT_006940a0;

// GLOBAL: CARMA2_HW 0x006940a4
int gINT_006940a4;

// GLOBAL: CARMA2_HW 0x0065d008
br_scalar gFLOAT_0065d008 = 1.f;

// GLOBAL: CARMA2_HW 0x0065d00c
br_scalar gFLOAT_0065d00c = 1.f;

// GLOBAL: CARMA2_HW 0x0058a768
br_scalar gFLOAT_0058a768 = 0.0f;

// GLOBAL: CARMA2_HW 0x0058a780
const double gDOUBLE_0058a780 = 1.0;

// GLOBAL: CARMA2_HW 0x0058a7f0
const double gDOUBLE_0058a7f0 = 0.5;

// GLOBAL: CARMA2_HW 0x0058a7f8
const br_scalar gFLOAT_0058a7f8 = 0.04f;

// GLOBAL: CARMA2_HW 0x0058a800
const br_scalar gFLOAT_0058a800 = 1.0f;

// GLOBAL: CARMA2_HW 0x0058a804
const br_scalar gFLOAT_0058a804 = 0.1f;

// GLOBAL: CARMA2_HW 0x0058a7fc
br_scalar gFLOAT_0058a7fc = 2.384185791015625e-7f;

// GLOBAL: CARMA2_HW 0x00692dd0
br_scalar gFLOAT_00692dd0;

// GLOBAL: CARMA2_HW 0x00692dd4
br_scalar gFLOAT_00692dd4;

// GLOBAL: CARMA2_HW 0x0058a788
const double gDOUBLE_0058a788 = 0.0025;

// GLOBAL: CARMA2_HW 0x0058a790
const double gDOUBLE_0058a790 = -0.0025;

// GLOBAL: CARMA2_HW 0x0058a798
const double gDOUBLE_0058a798 = 0.01;

// GLOBAL: CARMA2_HW 0x0058a7c8
const br_scalar gFLOAT_0058a7c8 = 25.0f;

// GLOBAL: CARMA2_HW 0x0058a7d0
const double gDOUBLE_0058a7d0 = -0.5;

// GLOBAL: CARMA2_HW 0x0058a7e8
const br_scalar gFLOAT_0058a7e8 = 100.0f;

// GLOBAL: CARMA2_HW 0x0065d014
int gINT_0065d014 = 1;

// GLOBAL: CARMA2_HW 0x0058a7d8
const double gDOUBLE_0058a7d8 = -1.001;

// GLOBAL: CARMA2_HW 0x0058a7e0
const double gDOUBLE_0058a7e0 = 25.0;

// GLOBAL: CARMA2_HW 0x0058a830
const double gDOUBLE_0058a830 = 0.1;

// GLOBAL: CARMA2_HW 0x0058a838
const double gDOUBLE_0058a838 = -0.1;

// GLOBAL: CARMA2_HW 0x0058a840
const double gDOUBLE_0058a840 = 0.0099;

// GLOBAL: CARMA2_HW 0x0058a824
const br_scalar gFLOAT_0058a824 = 0.0016f;

// GLOBAL: CARMA2_HW 0x0058a828
const br_scalar gFLOAT_0058a828 = 2.4649f;

// GLOBAL: CARMA2_HW 0x0058a82c
const br_scalar gFLOAT_0058a82c = 1.27388525f;

// GLOBAL: CARMA2_HW 0x0058a85c
const br_scalar gFLOAT_0058a85c = 0.636942625f;

// GLOBAL: CARMA2_HW 0x0058a860
const double gDOUBLE_0058a860 = 0.0000001;

// GLOBAL: CARMA2_HW 0x0058a868
const br_scalar gFLOAT_0058a868 = 0.008f;

// GLOBAL: CARMA2_HW 0x0058a86c
const br_scalar gFLOAT_0058a86c = 31.2499981f;

// GLOBAL: CARMA2_HW 0x0058a770
const double gDOUBLE_0058a770 = 10430.378350470453;

// GLOBAL: CARMA2_HW 0x00692ddc
int gINT_00692ddc;

// GLOBAL: CARMA2_HW 0x00692de0
tCollision_contact gCollision_contacts[50];

// GLOBAL: CARMA2_HW 0x006940a8
tPhysics_object** gReduced_object_list;

// GLOBAL: CARMA2_HW 0x006940ac
int gINT_006940ac;

// GLOBAL: CARMA2_HW 0x006940b8
int gDAT_006940b8;

// GLOBAL: CARMA2_HW 0x00692dc8
int gINT_00692dc8;

// GLOBAL: CARMA2_HW 0x006923ec
tPhysics_object* gPHIL_queued_objects_for_removal[1];

// GLOBAL: CARMA2_HW 0x006923e8
tPhysics_callbacks* gPHIL_callbacks;

// FUNCTION: CARMA2_HW 0x004c6410
void C2_HOOK_FASTCALL PHILRemoveFromParentChain(tPhysics_object* pObject) {
    tPhysics_object* parent;
    tPhysics_object* walk;

    parent = pObject->parent;
    walk = parent->child;
    if (walk == pObject) {
        parent->child = pObject->next;
    } else {
        while (walk->next != pObject) {
            walk = walk->next;
        }
        walk->next = pObject->next;
    }
    pObject->parent = NULL;
    pObject->next = NULL;
}

// FUNCTION: CARMA2_HW 0x004b9750
void C2_HOOK_FASTCALL PHILDetachObjectState(tPhysics_object* pObject) {

    pObject->field_0x218 = 0;
    ((void (C2_HOOK_FASTCALL *)(tPhysics_object*, float))MoveJointedObject)(pObject, 0.f);
    gINT_00692dc8 = 1;
}

// FUNCTION: CARMA2_HW 0x004b62e0
int C2_HOOK_FASTCALL PHILQueueDetachBit(tPhysics_object* pObject, void* pA, const br_vector3* pB, const br_vector3* pC) {
    tQueued_object_info* object_info;
    int result;

    if (gPHIL_enabled) {
        return 0;
    }
    if (gPHIL_munging_objects) {
        gPHIL_object_added = 1;
    }
    result = PHILAddObject(pObject);
    if (result == 0) {
        if (pA != NULL && !gPHIL_enabled) {
            if (pObject->field_0x239 == 2) {
                tQueued_object_info* info = pObject->field_0x240;
                memcpy(&info->field_0x30, pA, sizeof(br_matrix34));
                info->flags |= 0x4;
            } else if (pObject->field_0x240 != NULL) {
                object_info = pObject->field_0x240;
                if (object_info->field_0x8 != 1) {
                    gPHIL_object_added = 0;
                    return 4;
                }
                memcpy(&object_info->object->actor->t.t.mat, pA, sizeof(br_matrix34));
            } else {
                gPHIL_object_added = 0;
                return 3;
            }
        }
        result = PHILMakeObjectActive(pObject, pB, pC, 0);
    }
    gPHIL_object_added = 0;
    return result;
}

// FUNCTION: CARMA2_HW 0x004c80f0
void C2_HOOK_FASTCALL PHILSendDetachBit(int pFlags, void* pBit) {

    ARDoSingleVariedSession(0xe, pFlags, 2, 4, 0, (uintptr_t)pBit, 0x30, 4, (uintptr_t)((tU8*)pBit + 0x2c));
}


// FUNCTION: CARMA2_HW 0x004b5990
void C2_HOOK_FASTCALL DoPhysicsError(tPhysicsError pError, const char* pMessage) {
    char s[256];

    switch(pError) {
    case ePhysicsError_UnknownShapeType:
        strcpy(s, "Unknown shape type");
        break;
    case ePhysicsError_WireFrameHasNoPoints:
        strcpy(s, "Wire frame shape has no points");
        break;
    case ePhysicsError_PolyhedronHasNoPoints:
        strcpy(s, "Polyhedron shape has no points");
        break;
    case ePhysicsError_UnknownHingeType:
        strcpy(s, "Unknown hinge type");
    default:
        sprintf(s, "%d ", pError);
        break;
    }
    FatalError(1000 + pError, s);
}

// FUNCTION: CARMA2_HW 0x004b5aa0
void C2_HOOK_FASTCALL SetUpTestObjects(void) {

}

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x004c61f0
void C2_HOOK_FASTCALL PhysicsSetErrorHandler(tPhysicsError_cbfn *pError_cbfn) {

    gPhysics_error_cb = pError_cbfn;
}
#pragma auto_inline(on)

// FUNCTION: CARMA2_HW 0x004c61c0
void C2_HOOK_FASTCALL PhysicsError(tPhysicsError pError, const char* pMessage) {

    if (gPhysics_error_cb != NULL) {
        gPhysics_error_cb(pError, pMessage);
    }

    exit(pError);
}

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x004c6510
void C2_HOOK_FASTCALL InitPhysicsWorkspace(tU8* pBuffer, int pSize) {

    gPhysics_buffer1_ptr = pBuffer;
    pSize -= 3 * PHYSICS_BUFFER_PART_SIZE;
    gPhysics_buffer2_ptr = pBuffer + PHYSICS_BUFFER_PART_SIZE;
    gPhysics_buffer3_ptr = pBuffer + 2 * PHYSICS_BUFFER_PART_SIZE;
    pBuffer += 3 * PHYSICS_BUFFER_PART_SIZE;
    if (pSize < PHYSICS_BUFFER_OTHER_SIZE) {
        PhysicsError(ePhysicsError_InsufficientSizedBuffer, "Physics workspace too small");
    }
    gPhysics_other_buffer_capacity = pSize;
    gPhysics_other_buffer = pBuffer;
}
#pragma auto_inline(on)

// FUNCTION: CARMA2_HW 0x004b5cc0
int C2_HOOK_FASTCALL PHILInit(void) {
    int i;

    C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(gPhil_queued_objects.headers) != 100);
    C2_HOOK_BUG_ON(sizeof(gPhil_queued_objects.headers[0]) != 0x18);

    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gPhil_queued_objects.headers); i++) {
        gPhil_queued_objects.headers[i].collision_info = NULL;
    }
    gPHIL_count_list_collision_infos = 0;
    gPHIL_count_queued_objects = 0;
    gPHIL_count_queued_objects_for_removal = 0;
    gList_collision_infos = NULL;
    gPHIL_munging_objects = 0;
    gCrush_deferred = 0;
    gPHIL_object_added = 0;
    gPHIL_last_physics_tick = 0;
    gPHIL_mechanics_time_sync = 1;
    gPHIL_enabled = 0;
    return 0;
}

// C2PORT-NONMATCHING: best 22.96% | difftest PASS 2000 trials (spec difftest/specs/004c4850_FUN_004c4850.json) | reason stack-layout-x87-order
// FUNCTION: CARMA2_HW 0x004c4850
void C2_HOOK_FASTCALL FUN_004c4850(float* param_1, int param_2, int param_3, int param_4, float* param_5, int param_6) {
    typedef unsigned char byte;
    typedef unsigned int uint;
  float local_190 [100];
  byte *pbVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  byte bVar8;
  int iVar9;
  float *pfVar10;
  float *pfVar11;
  float *pfVar12;
  float *pfVar13;
  float *pfVar14;
  int iVar15;
  int local_1bc;
  int local_1b8_offset;
  float *local_1b8;
  float local_1b4;
  float *local_1b0;
  float *local_1ac;
  int local_1a0;
  float *local_19c;
  float *local_198;

  local_1bc = param_3;
  if (param_3 < param_4) {
    local_1b8_offset = param_3 * 100;
    local_1ac = param_1 + param_3;
    do {
      bVar8 = *(byte *)(local_1bc + param_6);
      iVar9 = 0;
      if (0 < param_3) {
        local_1b0 = local_1ac;
        pfVar14 = local_190;
        do {
          pbVar1 = (byte *)(iVar9 + param_6);
          iVar9 = iVar9 + 1;
          fVar2 = *(float *)((uint)bVar8 * 400 + param_2 + (uint)*pbVar1 * 4);
          *pfVar14 = fVar2;
          *local_1b0 = fVar2;
          local_1b0 = local_1b0 + 100;
          pfVar14 = pfVar14 + 1;
        } while (iVar9 < param_3);
      }
      iVar9 = 0;
      pfVar14 = param_5;
      if (0 < param_3) {
        do {
          pfVar11 = local_190;
          pfVar13 = pfVar14;
          iVar15 = param_3;
          fVar2 = gFLOAT_0058a8b8;
          do {
            fVar3 = *pfVar11;
            fVar4 = *pfVar13;
            pfVar13 = pfVar13 + 1;
            pfVar11 = pfVar11 + 1;
            iVar15 = iVar15 + -1;
            fVar2 = fVar3 * fVar4 + fVar2;
          } while (iVar15 != 0);
          iVar15 = local_1b8_offset + iVar9;
          iVar9 = iVar9 + 1;
          param_1[iVar15] = fVar2;
          pfVar14 = pfVar14 + 100;
        } while (iVar9 < param_3);
      }
      local_1bc = local_1bc + 1;
      local_1ac = local_1ac + 1;
      local_1b8_offset = local_1b8_offset + 100;
    } while (local_1bc < param_4);
  }
  if (param_3 < param_4) {
    pfVar14 = param_5 + param_3 * 0x65;
    local_1b8 = param_5 + param_3;
    local_1ac = param_5 + param_3 * 100;
    iVar9 = param_3;
    local_198 = pfVar14;
    do {
      bVar8 = *(byte *)(iVar9 + param_6);
      pfVar11 = pfVar14;
      iVar15 = param_3;
      do {
        pbVar1 = (byte *)(iVar15 + param_6);
        iVar15 = iVar15 + 1;
        *(undefined4 *)((int)param_1 + (-4 - (int)param_5) + (int)(pfVar11 + 1)) =
             *(undefined4 *)((uint)bVar8 * 400 + param_2 + (uint)*pbVar1 * 4);
        *pfVar11 = 0.0;
        pfVar11 = pfVar11 + 1;
      } while (iVar15 < param_4);
      pfVar11 = local_1ac;
      pfVar13 = local_1b8;
      iVar15 = param_3;
      if (0 < param_3) {
        do {
          *pfVar11 = 0.0;
          *pfVar13 = 0.0;
          iVar15 = iVar15 + -1;
          pfVar11 = pfVar11 + 1;
          pfVar13 = pfVar13 + 100;
        } while (iVar15 != 0);
      }
      *local_198 = 1.0;
      iVar9 = iVar9 + 1;
      pfVar14 = pfVar14 + 100;
      local_1ac = local_1ac + 100;
      local_1b8 = local_1b8 + 1;
      local_198 = local_198 + 0x65;
    } while (iVar9 < param_4);
  }
  local_1ac = (float *)0x0;
  if (0 < param_3) {
    local_19c = param_1 + param_3 + 100;
    local_198 = (float *)0x64;
    pfVar14 = param_1;
    local_1b0 = param_1;
    do {
      fVar2 = pfVar14[(int)local_1ac];
      local_1b4 = 0.0;
      if (param_3 < param_4) {
        pfVar11 = pfVar14 + param_3;
        iVar9 = param_4 - param_3;
        do {
          fVar3 = fabsf(*pfVar11);
          if (fabsf(*pfVar11) < local_1b4) {
            fVar3 = local_1b4;
          }
          pfVar11 = pfVar11 + 1;
          iVar9 = iVar9 + -1;
          local_1b4 = fVar3;
        } while (iVar9 != 0);
      }
      if (local_1b4 != (float)gDOUBLE_0058a8c0) {
        fVar3 = fabsf(fVar2);
        if (fVar3 < local_1b4) {
          fVar3 = local_1b4;
        }
        if (param_3 < param_4) {
          iVar9 = param_4 - param_3;
          pfVar11 = pfVar14 + param_3;
          do {
            iVar9 = iVar9 + -1;
            *pfVar11 = *pfVar11 / fVar3;
            pfVar11 = pfVar11 + 1;
          } while (iVar9 != 0);
        }
        fVar2 = fVar2 / fVar3;
        fVar4 = gFLOAT_0058a8b8;
        if (param_3 < param_4) {
          pfVar11 = pfVar14 + param_3;
          iVar9 = param_4 - param_3;
          do {
            fVar5 = *pfVar11;
            pfVar11 = pfVar11 + 1;
            iVar9 = iVar9 + -1;
            fVar4 = fVar5 * fVar5 + fVar4;
          } while (iVar9 != 0);
        }
        fVar4 = fabsf(sqrtf(fVar2 * fVar2 + fVar4));
        if (fVar2 <= (float)gDOUBLE_0058a8c0) {
          fVar4 = -fVar4;
        }
        fVar2 = fVar2 + fVar4;
        iVar9 = 0;
        if (0 < param_4) {
          pfVar11 = (float *)(((int)param_5 - (int)param_1) + (int)pfVar14);
          do {
            fVar5 = fVar2 * *pfVar11;
            if (param_3 < param_4) {
              pfVar10 = pfVar14 + param_3;
              pfVar13 = param_5 + iVar9 + param_3 * 100;
              iVar15 = param_4 - param_3;
              do {
                fVar6 = *pfVar13;
                fVar7 = *pfVar10;
                pfVar13 = pfVar13 + 100;
                pfVar10 = pfVar10 + 1;
                iVar15 = iVar15 + -1;
                fVar5 = fVar6 * fVar7 + fVar5;
              } while (iVar15 != 0);
            }
            fVar5 = fVar5 / (fVar2 * fVar4);
            if (param_3 < param_4) {
              pfVar13 = pfVar14 + param_3;
              iVar15 = param_4 - param_3;
              pfVar10 = param_5 + iVar9 + param_3 * 100;
              do {
                fVar6 = *pfVar13;
                pfVar13 = pfVar13 + 1;
                iVar15 = iVar15 + -1;
                *pfVar10 = *pfVar10 - fVar5 * fVar6;
                pfVar10 = pfVar10 + 100;
              } while (iVar15 != 0);
            }
            iVar9 = iVar9 + 1;
            *pfVar11 = *pfVar11 - fVar2 * fVar5;
            pfVar11 = pfVar11 + 1;
          } while (iVar9 < param_4);
        }
        if ((int)local_1ac + 1 < param_4) {
          local_1a0 = param_4 - ((int)local_1ac + 1);
          pfVar11 = param_1 + (int)local_1ac + (int)local_198;
          pfVar13 = local_19c;
          do {
            fVar5 = fVar2 * *pfVar11;
            if (param_3 < param_4) {
              pfVar10 = pfVar14 + param_3;
              iVar9 = param_4 - param_3;
              pfVar12 = pfVar13;
              do {
                fVar6 = *pfVar12;
                fVar7 = *pfVar10;
                pfVar12 = pfVar12 + 1;
                pfVar10 = pfVar10 + 1;
                iVar9 = iVar9 + -1;
                fVar5 = fVar6 * fVar7 + fVar5;
              } while (iVar9 != 0);
            }
            fVar5 = fVar5 / (fVar2 * fVar4);
            if (param_3 < param_4) {
              pfVar10 = pfVar14 + param_3;
              iVar9 = param_4 - param_3;
              pfVar12 = pfVar13;
              do {
                fVar6 = *pfVar10;
                pfVar10 = pfVar10 + 1;
                iVar9 = iVar9 + -1;
                *pfVar12 = *pfVar12 - fVar5 * fVar6;
                pfVar12 = pfVar12 + 1;
              } while (iVar9 != 0);
            }
            pfVar13 = pfVar13 + 100;
            local_1a0 = local_1a0 + -1;
            *pfVar11 = *pfVar11 - fVar2 * fVar5;
            pfVar11 = pfVar11 + 100;
          } while (local_1a0 != 0);
        }
        *local_1b0 = -(fVar4 * fVar3);
        if (param_3 < param_4) {
          pfVar11 = pfVar14 + param_3;
          for (iVar9 = param_4 - param_3; iVar9 != 0; iVar9 = iVar9 + -1) {
            *pfVar11 = 0.0;
            pfVar11 = pfVar11 + 1;
          }
        }
      }
      local_1ac = (float *)((int)local_1ac + 1);
      pfVar14 = pfVar14 + 100;
      local_198 = (float *)((int)local_198 + 100);
      local_19c = local_19c + 100;
      local_1b0 = local_1b0 + 0x65;
    } while ((int)local_1ac < param_3);
  }
  if (param_3 < param_4) {
    local_1bc = param_3 * 100;
    pfVar14 = param_1 + param_3 * 0x65;
    do {
      local_1b4 = 0.0;
      if (param_3 < param_4) {
        iVar9 = param_4 - param_3;
        pfVar11 = pfVar14;
        do {
          fVar2 = fabsf(*pfVar11);
          if (fabsf(*pfVar11) < local_1b4) {
            fVar2 = local_1b4;
          }
          pfVar11 = pfVar11 + 1;
          iVar9 = iVar9 + -1;
          local_1b4 = fVar2;
        } while (iVar9 != 0);
      }
      if (local_1b4 != gFLOAT_0058a8b8) {
        if (param_3 < param_4) {
          iVar9 = param_4 - param_3;
          pfVar11 = pfVar14;
          do {
            iVar9 = iVar9 + -1;
            *pfVar11 = *pfVar11 / local_1b4;
            pfVar11 = pfVar11 + 1;
          } while (iVar9 != 0);
        }
        fVar2 = gFLOAT_0058a8b8;
        if (param_3 < param_4) {
          iVar9 = param_4 - param_3;
          pfVar11 = pfVar14;
          do {
            fVar3 = *pfVar11;
            pfVar11 = pfVar11 + 1;
            iVar9 = iVar9 + -1;
            fVar2 = fVar3 * fVar3 + fVar2;
          } while (iVar9 != 0);
        }
        fVar2 = fabsf(sqrtf(fVar2));
        if (*pfVar14 <= (float)gDOUBLE_0058a8c0) {
          fVar2 = -fVar2;
        }
        fVar3 = fVar2 * (fVar2 + *pfVar14);
        *pfVar14 = fVar2 + *pfVar14;
        iVar9 = 0;
        if (0 < param_4) {
          do {
            fVar4 = gFLOAT_0058a8b8;
            if (param_3 < param_4) {
              pfVar11 = param_5 + local_1bc + iVar9;
              iVar15 = param_4 - param_3;
              pfVar13 = pfVar14;
              do {
                fVar5 = *pfVar13;
                fVar6 = *pfVar11;
                pfVar13 = pfVar13 + 1;
                pfVar11 = pfVar11 + 100;
                iVar15 = iVar15 + -1;
                fVar4 = fVar5 * fVar6 + fVar4;
              } while (iVar15 != 0);
            }
            if (param_3 < param_4) {
              iVar15 = param_4 - param_3;
              pfVar11 = param_5 + iVar9 + local_1bc;
              pfVar13 = pfVar14;
              do {
                fVar5 = *pfVar13;
                pfVar13 = pfVar13 + 1;
                iVar15 = iVar15 + -1;
                *pfVar11 = *pfVar11 - (fVar4 / fVar3) * fVar5;
                pfVar11 = pfVar11 + 100;
              } while (iVar15 != 0);
            }
            iVar9 = iVar9 + 1;
          } while (iVar9 < param_4);
        }
        if (param_3 + 1 < param_4) {
          pfVar11 = param_1 + local_1bc + param_3 + 100;
          local_1a0 = param_4 - (param_3 + 1);
          do {
            fVar4 = gFLOAT_0058a8b8;
            if (param_3 < param_4) {
              iVar9 = param_4 - param_3;
              pfVar13 = pfVar14;
              pfVar10 = pfVar11;
              do {
                fVar5 = *pfVar10;
                fVar6 = *pfVar13;
                pfVar13 = pfVar13 + 1;
                pfVar10 = pfVar10 + 1;
                iVar9 = iVar9 + -1;
                fVar4 = fVar5 * fVar6 + fVar4;
              } while (iVar9 != 0);
            }
            if (param_3 < param_4) {
              iVar9 = param_4 - param_3;
              pfVar13 = pfVar11;
              pfVar10 = pfVar14;
              do {
                fVar5 = *pfVar10;
                pfVar10 = pfVar10 + 1;
                iVar9 = iVar9 + -1;
                *pfVar13 = *pfVar13 - (fVar4 / fVar3) * fVar5;
                pfVar13 = pfVar13 + 1;
              } while (iVar9 != 0);
            }
            pfVar11 = pfVar11 + 100;
            local_1a0 = local_1a0 + -1;
          } while (local_1a0 != 0);
        }
        iVar9 = param_3 + 1;
        *pfVar14 = -(fVar2 * local_1b4);
        if (iVar9 < param_4) {
          pfVar11 = param_1 + local_1bc + iVar9;
          for (iVar15 = param_4 - iVar9; iVar15 != 0; iVar15 = iVar15 + -1) {
            *pfVar11 = 0.0;
            pfVar11 = pfVar11 + 1;
          }
        }
      }
      param_3 = param_3 + 1;
      pfVar14 = pfVar14 + 0x65;
      local_1bc = local_1bc + 100;
    } while (param_3 < param_4);
  }
  return;
}

// C2PORT-NONMATCHING: best 25.67% | difftest PASS 2000 trials (spec difftest/specs/004c4f40_FUN_004c4f40.json) | reason x87-order/stack-layout
// FUNCTION: CARMA2_HW 0x004c4f40
void C2_HOOK_FASTCALL FUN_004c4f40(void* pA, undefined4 pArg1, int pCount, void* pB, void* pTableArg) {
    const tU8* pbVar1;
    float fVar2;
    float fVar3;
    float fVar4;
    float fVar7;
    float fVar8;
    float fVar9;
    float fVar10;
    float fVar11;
    tU8 bVar5;
    int bVar6;
    int iVar12;
    float* pfVar13;
    float* pfVar14;
    float* pfVar15;
    int iVar16;
    float* pfVar17;
    int iVar18;
    int iVar19;
    const float* pSource = (const float*)(uintptr_t)pArg1;
    const tU8* pTable = (const tU8*)pTableArg;
    int local_1b4;
    float* local_1b0;
    float* local_1a4;
    float local_190[101];

    iVar12 = 0;
    if (-1 < pCount) {
        bVar5 = pTable[pCount];
        pfVar13 = local_190;
        do {
            pbVar1 = pTable + iVar12;
            iVar12++;
            *pfVar13 = pSource[(tU32)*pbVar1 + (tU32)bVar5 * 100];
            pfVar13++;
        } while (iVar12 <= pCount);
    }
    iVar12 = 0;
    pfVar13 = (float*)pB;
    if (0 < pCount) {
        do {
            pfVar17 = local_190;
            pfVar14 = pfVar13;
            iVar16 = pCount;
            fVar8 = gFLOAT_0058a8b8;
            do {
                fVar2 = *pfVar14;
                fVar3 = *pfVar17;
                pfVar14++;
                pfVar17++;
                iVar16--;
                fVar8 = fVar2 * fVar3 + fVar8;
            } while (iVar16 != 0);
            iVar16 = iVar12 + pCount * 100;
            iVar12++;
            ((float*)pA)[iVar16] = fVar8;
            pfVar13 += 100;
        } while (iVar12 < pCount);
    }
    ((float*)pB)[pCount * 0x65] = 1.0f;
    if (0 < pCount) {
        pfVar13 = (float*)pB + pCount;
        pfVar17 = (float*)pB + pCount * 100;
        iVar12 = pCount;
        do {
            *pfVar13 = 0.0f;
            *pfVar17 = 0.0f;
            pfVar13 += 100;
            pfVar17++;
            iVar12--;
        } while (iVar12 != 0);
        if (0 < pCount) {
            local_1a4 = (float*)pB;
            iVar16 = (int)((float*)pB + pCount * 100) - (int)pB;
            local_1b0 = (float*)pA + pCount;
            pfVar13 = local_190;
            local_1b4 = 400;
            iVar12 = 1;
            pfVar17 = (float*)pA;
            do {
                fVar8 = fabsf(*pfVar13);
                *local_1b0 = 0.0f;
                if ((double)fVar8 != gDOUBLE_0058a8c0) {
                    fVar2 = fabsf(*pfVar17);
                    if (fVar2 < fVar8) {
                        fVar2 = fVar8;
                    }
                    fVar8 = *pfVar13 / fVar2;
                    fVar3 = *pfVar17 / fVar2;
                    *pfVar13 = fVar8;
                    fVar9 = fabsf(sqrtf(fVar3 * fVar3 + fVar8 * fVar8));
                    if ((double)fVar3 <= gDOUBLE_0058a8c0) {
                        fVar9 = -fVar9;
                    }
                    iVar19 = pCount + 1;
                    fVar3 = fVar3 + fVar9;
                    fVar10 = fVar8 / (fVar3 * fVar9);
                    fVar11 = fVar3 / (fVar3 * fVar9);
                    pfVar14 = local_1a4;
                    iVar18 = iVar19;
                    if (0 < iVar19) {
                        do {
                            fVar4 = fVar8 * *(float*)(iVar16 + (int)pfVar14) + fVar3 * *pfVar14;
                            iVar18--;
                            *(float*)(iVar16 - 4 + (int)(pfVar14 + 1)) = *(float*)(iVar16 + (int)pfVar14) - fVar10 * fVar4;
                            *pfVar14 = *pfVar14 - fVar11 * fVar4;
                            pfVar14++;
                        } while (iVar18 != 0);
                    }
                    if (iVar12 < iVar19) {
                        iVar19 -= iVar12;
                        pfVar15 = (float*)((int)pA + (local_1b4 - (int)local_190) + (int)pfVar13);
                        pfVar14 = pfVar13;
                        do {
                            pfVar14++;
                            fVar7 = fVar3 * *pfVar15 + fVar8 * *pfVar14;
                            iVar19--;
                            fVar4 = *pfVar15;
                            *pfVar14 = *pfVar14 - fVar10 * fVar7;
                            *pfVar15 = fVar4 - fVar11 * fVar7;
                            pfVar15 += 100;
                        } while (iVar19 != 0);
                    }
                    *pfVar17 = -(fVar9 * fVar2);
                }
                local_1b0 += 100;
                iVar16 -= 400;
                local_1b4 += 400;
                pfVar13++;
                local_1a4 += 100;
                pfVar17 += 0x65;
                bVar6 = iVar12 < pCount;
                iVar12++;
            } while (bVar6);
        }
    }
    ((float*)pA)[pCount * 0x65] = local_190[pCount];
}

// C2PORT-NONMATCHING: best 39.53% | difftest PASS 2000 trials (spec difftest/specs/004c5220_FUN_004c5220.json) | reason pointer-arith-codegen
// FUNCTION: CARMA2_HW 0x004c5220
void C2_HOOK_FASTCALL FUN_004c5220(int pParam1, int pParam2, undefined4 pArg1, undefined4* pArg2, int* pArg3, tU8* pArg4, int pArg5, int** pArg6, int pCount) {
    int iVar2;
    int iVar3;
    int iVar5;
    int iVar6;
    int iVar7;
    int local_14;
    int local_18;
    int local_c;
    int* param_9_val;
    int* param_8;
    int** param_9;
    undefined4* puVar4;
    undefined4* puVar8;
    undefined4* puVar9;
    undefined4* puVar10;

    iVar5 = 0;
    if (0 < pCount) {
        local_c = pCount;
        param_9 = pArg6;
        do {
            param_9_val = (int*)*param_9;
            iVar2 = *param_9_val;
            if (0 < iVar2) {
                param_8 = (int*)(pParam2 + iVar5 * 4);
                local_18 = 0;
                puVar4 = (undefined4*)(iVar5 * 400 + pParam2);
                puVar9 = (undefined4*)(pParam2 + iVar5 * 0x194);
                local_14 = iVar2;
                do {
                    iVar3 = pParam1 - pParam2;
                    iVar6 = iVar2;
                    iVar7 = local_18;
                    puVar8 = puVar9;
                    do {
                        iVar7 += 4;
                        iVar6--;
                        *(undefined4*)((char*)puVar8 + iVar3) = *(undefined4*)((char*)*(int*)((char*)param_9_val + 0x18) + iVar7 - 4);
                        *puVar8 = *(undefined4*)((char*)*(int*)((char*)param_9_val + 0x14) + iVar7 - 4);
                        puVar8++;
                    } while (iVar6 != 0);
                    iVar6 = iVar5;
                    puVar8 = puVar4;
                    puVar10 = (undefined4*)param_8;
                    if (0 < iVar5) {
                        do {
                            *(undefined4*)((char*)puVar10 + iVar3) = 0;
                            *(undefined4*)((char*)puVar8 + iVar3) = 0;
                            *puVar10 = 0;
                            *puVar8 = 0;
                            iVar6--;
                            puVar8++;
                            puVar10 += 100;
                        } while (iVar6 != 0);
                    }
                    param_8 = param_8 + 1;
                    local_18 = local_18 + iVar2 * 4;
                    puVar9 += 100;
                    puVar4 += 100;
                    local_14--;
                } while (local_14 != 0);
            }
            iVar5 += iVar2;
            param_9 = param_9 + 1;
            local_c--;
        } while (local_c != 0);
    }
    if (pArg5 - iVar5 == 1) {
        FUN_004c4f40(pParam1, pArg1, iVar5, (void*)pParam2, pArg4);
    } else if (iVar5 < pArg5) {
        FUN_004c4850((float*)(uintptr_t)pParam1, pArg1, iVar5, pArg5, (float*)pParam2, (int)(uintptr_t)pArg4);
    }
    iVar5 = 0;
    if (0 < pArg5) {
        do {
            *pArg2 = pArg3[pArg4[iVar5]];
            iVar5++;
            pArg2++;
        } while (iVar5 < pArg5);
    }
}

// FUNCTION: CARMA2_HW 0x004c5e00
tCollision_shape_box* C2_HOOK_FASTCALL AllocateBoxCollisionShape(int pType) {
    tCollision_shape_box* result;

    C2_HOOK_BUG_ON(sizeof(tCollision_shape_box) != 56);
    result = BrMemAllocate(sizeof(tCollision_shape_box), pType);
    result->common.type = kCollisionShapeType_Box;
    return result;
}

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x004c5d10
tCollision_shape_sphere* C2_HOOK_FASTCALL AllocateShapeSphere(int pType) {
    tCollision_shape_sphere* result;

    C2_HOOK_BUG_ON(sizeof(tCollision_shape_sphere) != 76);
    result = BrMemAllocate(sizeof(tCollision_shape_sphere), pType);
    result->common.type = kCollisionShapeType_Sphere;
    return result;
}
#pragma auto_inline(on)

// FUNCTION: CARMA2_HW 0x004c5d70
tCollision_shape_polyhedron* C2_HOOK_FASTCALL AllocateShapePolyhedron(int pCount_points, int pType) {
    tCollision_shape_polyhedron* result;
    tU8* raw_memory;
    int count_edges;

    C2_HOOK_BUG_ON(sizeof(tCollision_shape_polyhedron) != 80);
    C2_HOOK_BUG_ON(sizeof(tPolyhedron_edge_indexes) != 2);
    count_edges = (pCount_points - 2) * 3;
    result = BrMemAllocate(sizeof(tCollision_shape_polyhedron) + pCount_points * sizeof(br_vector3)
        + count_edges * sizeof(tPolyhedron_edge_indexes) + (pCount_points - 2) * 2 * sizeof(br_vector4), (br_uint_8)pType);
    raw_memory = (tU8*)result + sizeof(tCollision_shape_polyhedron);

    result->polyhedron.points = (br_vector3*)raw_memory;
    raw_memory += pCount_points * sizeof(br_vector3);

    result->polyhedron.edges = (tPolyhedron_edge_indexes*)raw_memory;
    raw_memory += count_edges * sizeof(tPolyhedron_edge_indexes);

    result->polyhedron.planes = (br_vector4*)raw_memory;

    result->polyhedron.count_points = 0;
    result->polyhedron.count_edges = 0;
    result->polyhedron.count_planes = 0;
    result->common.type = kCollisionShapeType_Polyhedron;
    return result;
}

// FUNCTION: CARMA2_HW 0x004c5dc0
tCollision_shape_wireframe* C2_HOOK_FASTCALL AllocateWireFrameCollisionShape(int pCount_points, int pCount_lines, br_uint_8 pType) {
    tCollision_shape_wireframe* result;
    tU8* raw_memory;

    C2_HOOK_BUG_ON(sizeof(tCollision_shape_wireframe) != 72);
    C2_HOOK_BUG_ON(sizeof(tPolyhedron_edge_indexes) != 2);
    result = BrMemAllocate(sizeof(tCollision_shape_wireframe) + pCount_points * sizeof(br_vector3) + pCount_lines * sizeof(tPolyhedron_edge_indexes), pType);
    raw_memory = (tU8*)result;

    raw_memory += sizeof(tCollision_shape_wireframe);

    result->wireframe.points = (br_vector3*)raw_memory;
    raw_memory += pCount_points * sizeof(br_vector3);

    result->wireframe.lines = (tPolyhedron_edge_indexes*)raw_memory;
    result->wireframe.count_lines = pCount_lines;
    result->wireframe.count_points = pCount_points;
    result->common.type = kCollisionShapeType_Wireframe;
    return result;
}

// FUNCTION: CARMA2_HW 0x00488b00
void C2_HOOK_FASTCALL ReadMechanicsShapes(tPhysics_shape** pShape, FILE* pF) {
    int i;
    int count;

    /* Number of 'Bounding shapes' entries. */
    count = GetAnInt(pF);

    for (i = 0; i < count; i++) {
        char s[256];
        tPhysics_shape* shape;

        GetAString(pF, s);
        if (DRStricmp(s, "box") == 0) {

            shape = (tPhysics_shape*)AllocateBoxCollisionShape(kMem_collision_shape);
            GetThreeFloats(pF,
                &shape->box.common.bb.min.v[0],
                &shape->box.common.bb.min.v[1],
                &shape->box.common.bb.min.v[2]);
            GetThreeFloats(pF,
                &shape->box.common.bb.max.v[0],
                &shape->box.common.bb.max.v[1],
                &shape->box.common.bb.max.v[2]);
        } else if (DRStricmp(s, "polyhedron") == 0) {
            int count_points;
            int j;

            /* number of points */
            count_points = GetAnInt(pF);
            if (count_points > 32) { /* FIXME: magic number */
                sprintf(s, "Ow! Physics shape is mad (having %d points is silly)", count_points);
                PDFatalError(s);
            }
            shape = (tPhysics_shape*)AllocateShapePolyhedron(count_points, kMem_collision_shape);

            shape->polyhedron.polyhedron.count_points = count_points;
            for (j = 0; j < count_points; j++) {
                GetThreeFloats(pF,
                    &shape->polyhedron.polyhedron.points[j].v[0],
                    &shape->polyhedron.polyhedron.points[j].v[1],
                    &shape->polyhedron.polyhedron.points[j].v[2]);
            }
        } else if (DRStricmp(s, "sphere") == 0) {

            shape = (tPhysics_shape*)AllocateShapeSphere(kMem_collision_shape);

            shape->sphere.sphere.radius = GetAScalar(pF);
            GetThreeFloats(pF,
                &shape->sphere.sphere.center.v[0],
                &shape->sphere.sphere.center.v[1],
                &shape->sphere.sphere.center.v[2]);
        } else if (DRStricmp(s, "wire") == 0) {
            br_vector3 points[64];
            int count_points;
            int count_lines;
            int j;

            count_points = GetAnInt(pF);
            if (count_points > CARPOCALYPSE2_ASIZE(points)) {
                FatalError(kFatalError_TooManyExtraPointsForCarIndex_S, " Too many points in wire frame shape ");
            }
            for (j = 0; j < count_points; j++) {
                GetThreeFloats(pF, &points[j].v[0], &points[j].v[1], &points[j].v[2]);
            }
            count_lines = GetAnInt(pF);
            shape = (tPhysics_shape*)AllocateWireFrameCollisionShape(count_points, count_lines, kMem_collision_shape);

            for (j = 0; j < count_points; j++) {
                BrVector3Copy(&shape->wireframe.wireframe.points[j], &points[j]);
            }
            for (j = 0; j < count_lines; j++) {
                int index1, index2;

                GetPairOfInts(pF, &index1, &index2);
                shape->wireframe.wireframe.lines[j].index1 = index1;
                shape->wireframe.wireframe.lines[j].index2 = index2;
            }
        } else {
            FatalError(kFatalError_ShapeDataIsWrong);
        }
        FillInShape(shape);
        *pShape = shape;
        pShape = &shape->common.next;
    }
}

// FUNCTION: CARMA2_HW 0x004c60a0
void C2_HOOK_FASTCALL UpdateCollisionObject(tPhysics_object* pCollision_info) {
    tPhysics_shape* shape;
    tPhysics_shape* current;

    shape = pCollision_info->shape;
    if (shape == NULL) {
        return;
    }
    pCollision_info->bb1 = shape->common.bb;

    for (current = shape->common.next; current != NULL; current = current->common.next) {
        int i;

        for (i = 0; i < 3; i++) {
            pCollision_info->bb1.min.v[i] = pCollision_info->bb1.min.v[i] < current->common.bb.min.v[i]
                ? pCollision_info->bb1.min.v[i] : current->common.bb.min.v[i];
            pCollision_info->bb1.max.v[i] = pCollision_info->bb1.max.v[i] > current->common.bb.max.v[i]
                ? pCollision_info->bb1.max.v[i] : current->common.bb.max.v[i];
        }
    }
    pCollision_info->bb2 = pCollision_info->bb1;

    pCollision_info->radius_squared = MAX(CARPOCALYPSE2_SQR(pCollision_info->bb2.min.v[0]), CARPOCALYPSE2_SQR(pCollision_info->bb2.max.v[0]))
            + MAX(CARPOCALYPSE2_SQR(pCollision_info->bb2.min.v[1]), CARPOCALYPSE2_SQR(pCollision_info->bb2.max.v[1]))
            + MAX(CARPOCALYPSE2_SQR(pCollision_info->bb2.min.v[2]), CARPOCALYPSE2_SQR(pCollision_info->bb2.max.v[2]));
    pCollision_info->radius = sqrtf(pCollision_info->radius_squared);
}

// Collinear

static br_scalar C2_HOOK_FASTCALL PlaneValue(const br_vector4* pPlane, const br_vector3* pP) {

    return BrVector3Dot(pPlane, pP) + pPlane->v[3];
}

// FUNCTION: CARMA2_HW 0x004211f0
tPhysicsError C2_HOOK_FASTCALL ProcessTetrahedronPolyhedronCollisionShape(tCollision_shape_polyhedron_data* pPolyhedron, tPolyhedron_edge_indexes* pEdges) {
    int first_non_colinear;
    int i;
    br_vector3 tv2;
    br_vector3 tv1;

    first_non_colinear = 0;
    while (Collinear(&pPolyhedron->points[first_non_colinear + 0], &pPolyhedron->points[first_non_colinear + 1], &pPolyhedron->points[first_non_colinear + 2])) {
        first_non_colinear++;
        if (first_non_colinear == pPolyhedron->count_points - 3) {
            return ePhysicsError_UnknownShapeType;
        }
    }

    if (first_non_colinear != 0) {
        BrVector3Copy(&pPolyhedron->points[0], &pPolyhedron->points[first_non_colinear + 0]);
        BrVector3Copy(&pPolyhedron->points[1], &pPolyhedron->points[first_non_colinear + 1]);
        BrVector3Copy(&pPolyhedron->points[2], &pPolyhedron->points[first_non_colinear + 2]);
    }
    pPolyhedron->edges[0].index1 = 0;
    pPolyhedron->edges[0].index2 = 1;
    pPolyhedron->edges[1].index1 = 1;
    pPolyhedron->edges[1].index2 = 2;
    pPolyhedron->edges[2].index1 = 0;
    pPolyhedron->edges[2].index2 = 2;

    /* PlaneEquation */
    BrVector3Sub(&tv1, &pPolyhedron->points[2], &pPolyhedron->points[1]);
    BrVector3Sub(&tv2, &pPolyhedron->points[1], &pPolyhedron->points[0]);
    BrVector3Cross(&pPolyhedron->planes[0], &tv2, &tv1);
    BrVector3Normalise(&pPolyhedron->planes[0], &pPolyhedron->planes[0]);
    pPolyhedron->planes[0].v[3] = -BrVector3Dot(&pPolyhedron->planes[0], &pPolyhedron->points[0]);

    pEdges[2].index1 = 0;
    pEdges[1].index1 = 0;
    pEdges[0].index1 = 0;

    for (i = 0; i < pPolyhedron->count_points; i++) {

        if (fabsf(PlaneValue(&pPolyhedron->planes[0], &pPolyhedron->points[i])) > 1e-6f) {
            break;
        }
    }
    if (i == pPolyhedron->count_points) {
        return 7;
    }
    BrVector3Copy(&pPolyhedron->points[3], &pPolyhedron->points[i]);
    /* Points inside the polyhedron must have positive plane value */
    if (PlaneValue(&pPolyhedron->planes[0], &pPolyhedron->points[i]) < 1e-6f) {
        BrVector4Negate(&pPolyhedron->planes[0], &pPolyhedron->planes[0]);
    }
    for (i = 0; i < 3; i++) {
        int o;
        int o_i;

        o = i + 1;
        o_i = o % 3;

        pPolyhedron->edges[3 + i].index1 = i;
        pPolyhedron->edges[3 + i].index2 = 3;

        /* PlaneEquation */
        BrVector3Sub(&tv1, &pPolyhedron->points[3], &pPolyhedron->points[i]);
        BrVector3Sub(&tv2, &pPolyhedron->points[o_i], &pPolyhedron->points[i]);
        BrVector3Cross(&pPolyhedron->planes[o], &tv1, &tv2);
        BrVector3Normalise(&pPolyhedron->planes[o], &pPolyhedron->planes[o]);
        pPolyhedron->planes[o].v[3] = -BrVector3Dot(&pPolyhedron->planes[o], &pPolyhedron->points[3]);

        pEdges[i].index2 = o;
        pEdges[3 + i].index1 = o;
        pEdges[3 + i].index2 = 1 + (o + 1) % 3;
    }

    for (i = 0; i < 3; i++) {

        if (PlaneValue(&pPolyhedron->planes[1 + i], &pPolyhedron->points[(2 + i) % 3]) < 1e-6f) {
            BrVector4Negate(&pPolyhedron->planes[1 + i], &pPolyhedron->planes[1 + i]);
        }
    }

    pPolyhedron->count_points = 4;
    pPolyhedron->count_planes = 4;
    pPolyhedron->count_edges = 6;
    return ePhysicsError_Ok;
}

static tPlane_side calculate_plane_side(br_vector4* pPlane, br_vector3* pPoint) {
    br_scalar s;

    s = PlaneValue(pPlane, pPoint);
    if (s < -1e-6f) {
        return ePlaneSide_negative;
    } else if (s >= 1e-6f) {
        return ePlaneSide_positive;
    } else {
        return ePlaneSide_zero;
    }
}

// FUNCTION: CARMA2_HW 0x00421790
int C2_HOOK_FASTCALL PolyhedronCollisionShape_AddPoint(tCollision_shape_polyhedron_data* pPolyhedron, br_vector3* pVertex, tU8 pVertex_index, tPolyhedron_edge_indexes* pEdge_to_plane_indices) {
    tPlane_side plane_side_buffer[596];
    tPolyhedron_edge_indexes new_edge_to_plane_indices[894];
    tPolyhedron_edge_indexes new_edges[894];
    br_vector4 new_planes[596];
    int positive_side_edges[596];
    int all_positive;
    int i;
    int j;
    int count_new_planes;
    int count_new_edges;
    int final_count_edges;
    int final_count_planes;

    all_positive = 1;
    for (i = 0; i < pPolyhedron->count_planes; i++) {
        plane_side_buffer[i] = calculate_plane_side(&pPolyhedron->planes[i], pVertex);
        all_positive &= plane_side_buffer[i] != ePlaneSide_negative;
    }
    /* If all positive ==> point is inside polyhedron */
    if (all_positive) {
        return 0;
    }

    count_new_planes = 0;
    count_new_edges = 0;

    for (i = 0; i < pPolyhedron->count_edges; i++) {
        tU8 plane1;
        tPlane_side side1;
        tU8 plane2;
        tPlane_side side2;

        plane1 = pEdge_to_plane_indices[i].index1;
        side1 = plane_side_buffer[plane1];
        plane2 = pEdge_to_plane_indices[i].index2;
        side2 = plane_side_buffer[plane2];
        positive_side_edges[i] = side1 == ePlaneSide_positive || side2 == ePlaneSide_positive;
        if ((side1 ^ side2) & ePlaneSide_negative) {
            /* Exactly one plane of edge has vertex on negative side */
            if (!(side1 & ePlaneSide_zero) && !(side2 & ePlaneSide_zero)) {
                br_vector3* v1,* v2;
                br_vector3 tv1, tv2;
                br_vector4 *new_plane;
                /* vertex does not lie on edge */
                v1 = &pPolyhedron->points[pPolyhedron->edges[i].index1];
                v2 = &pPolyhedron->points[pPolyhedron->edges[i].index2];
                BrVector3Sub(&tv1, pVertex, v1);
                BrVector3Sub(&tv2, v2, v1);
                new_plane = &new_planes[count_new_planes];
                BrVector3Cross(new_plane, &tv1, &tv2);
                BrVector3Normalise(new_plane, new_plane);
                if (BrVector3Dot(&pPolyhedron->planes[plane1], &pPolyhedron->planes[plane1]) * BrVector3Dot(new_plane, &pPolyhedron->planes[plane2])
                        - BrVector3Dot(&pPolyhedron->planes[plane1], &pPolyhedron->planes[plane2]) * BrVector3Dot(new_plane, &pPolyhedron->planes[plane1]) < 0.f) {
                    BrVector4Negate(new_plane, new_plane);
                }
                new_plane->v[3] = -BrVector3Dot(new_plane, pVertex);
                if (side1 == ePlaneSide_positive) {
                    pEdge_to_plane_indices[i].index2 = count_new_planes + pPolyhedron->count_planes;
                } else {
                    pEdge_to_plane_indices[i].index1 = count_new_planes + pPolyhedron->count_planes;
                }

                for (j = 0; j < count_new_edges; j++) {
                    if (new_edges[j].index1 == pPolyhedron->edges[i].index1 && new_edges[j].index2 == pVertex_index) {
                        new_edge_to_plane_indices[j].index2 = count_new_planes + pPolyhedron->count_planes;
                        break;
                    }
                }
                if (j >= count_new_edges) {
                    new_edges[count_new_edges].index1 = pPolyhedron->edges[i].index1;
                    new_edges[count_new_edges].index2 = pVertex_index;
                    new_edge_to_plane_indices[count_new_edges].index1 = count_new_planes + pPolyhedron->count_planes;
                    count_new_edges += 1;
                }

                for (j = 0; j < count_new_edges; j++) {
                    if (new_edges[j].index1 == pPolyhedron->edges[i].index2 && new_edges[j].index2 == pVertex_index) {
                        new_edge_to_plane_indices[j].index2 = count_new_planes + pPolyhedron->count_planes;
                        count_new_planes += 1;
                        break;
                    }
                }
                if (j >= count_new_edges) {
                    new_edges[count_new_edges].index1 = pPolyhedron->edges[i].index2;
                    new_edges[count_new_edges].index2 = pVertex_index;
                    new_edge_to_plane_indices[count_new_edges].index1 = count_new_planes + pPolyhedron->count_planes;
                    count_new_edges += 1;
                    count_new_planes += 1;
                }
            } else {
                if (!(side1 & ePlaneSide_zero)) {
                    plane1 = plane2;
                }

                for (j = 0; j < count_new_edges; j++) {
                    if (new_edges[j].index1 == pPolyhedron->edges[i].index1 && new_edges[j].index2 == pVertex_index) {
                        new_edge_to_plane_indices[j].index2 = plane1;
                        break;
                    }
                }
                if (j >= count_new_edges) {
                    new_edges[count_new_edges].index1 = pPolyhedron->edges[i].index1;
                    new_edges[count_new_edges].index2 = pVertex_index;
                    new_edge_to_plane_indices[count_new_edges].index1 = plane1;
                    count_new_edges += 1;
                }

                for (j = 0; j < count_new_edges; j++) {
                    if (new_edges[j].index1 == pPolyhedron->edges[i].index2 && new_edges[j].index2 == pVertex_index) {
                        new_edge_to_plane_indices[j].index2 = plane1;
                        break;
                    }
                }
                if (j >= count_new_edges) {
                    new_edges[count_new_edges].index1 = pPolyhedron->edges[i].index2;
                    new_edges[count_new_edges].index2 = pVertex_index;
                    new_edge_to_plane_indices[count_new_edges].index1 = plane1;
                    count_new_edges += 1;
                }

            }
        }
    }

    final_count_edges = 0;
    for (i = 0; i < pPolyhedron->count_edges; i++) {
        if (positive_side_edges[i]) {
            if (i != final_count_edges) {
                pPolyhedron->edges[final_count_edges] = pPolyhedron->edges[i];
                pEdge_to_plane_indices[final_count_edges] = pEdge_to_plane_indices[i];
            }
            final_count_edges += 1;
        }
    }
    for (i = 0; i < count_new_edges; i++) {
        pPolyhedron->edges[final_count_edges] = new_edges[i];
        pEdge_to_plane_indices[final_count_edges] = new_edge_to_plane_indices[i];
        final_count_edges += 1;
    }
    pPolyhedron->count_edges = final_count_edges;

    final_count_planes = 0;
    for (i = 0; i < pPolyhedron->count_planes; i++) {
        if (!(plane_side_buffer[i] & ePlaneSide_negative)) {
            if (i != final_count_planes) {
                BrVector4Copy(&pPolyhedron->planes[final_count_planes], &pPolyhedron->planes[i]);
                for (j = 0; j < pPolyhedron->count_edges; j++) {
                    if (pEdge_to_plane_indices[j].index1 == i) {
                        pEdge_to_plane_indices[j].index1 = final_count_planes;
                    } else if (pEdge_to_plane_indices[j].index2 == i) {
                        pEdge_to_plane_indices[j].index2 = final_count_planes;
                    }
                }
            }
            final_count_planes += 1;
        }
    }
    for (i = 0; i < count_new_planes; i++) {
        BrVector4Copy(&pPolyhedron->planes[final_count_planes], &new_planes[i]);
        for (j = 0; j < pPolyhedron->count_edges; j++) {
            if (pEdge_to_plane_indices[j].index1 == pPolyhedron->count_planes + i) {
                pEdge_to_plane_indices[j].index1 = final_count_planes;
            } else if (pEdge_to_plane_indices[j].index2 == pPolyhedron->count_planes + i) {
                pEdge_to_plane_indices[j].index2 = final_count_planes;
            }
        }
        final_count_planes += 1;
    }
    pPolyhedron->count_planes = final_count_planes;
    return 1;
}

// FUNCTION: CARMA2_HW 0x00420d60
br_scalar C2_HOOK_FASTCALL SignedTwiceAreaOf3DTriangle(const br_vector3* pV1, const br_vector3* pV2) {
    br_scalar x;
    br_scalar y;
    br_scalar z;

    y = pV1->v[2] * pV2->v[0] - pV1->v[0] * pV2->v[2];
    x = pV1->v[1] * pV2->v[2] - pV1->v[2] * pV2->v[1];
    z = pV1->v[0] * pV2->v[1] - pV1->v[1] * pV2->v[0];
    return x * gPhysics_reference_normal_comparison.v[0] + y * gPhysics_reference_normal_comparison.v[1] + z * gPhysics_reference_normal_comparison.v[2];
}

#pragma inline_depth(0)
// FUNCTION: CARMA2_HW 0x00420dc0
int C2_HOOK_CDECL CompareAngle(const void* pLhs, const void* pRhs) {
    tCollision_shape_polyhedron_data* polyhedron;
    br_scalar s1, s2;
    br_vector3 tv1, tv2;
    const int* const pIndex1 = pLhs;
    const int* const pIndex2 = pRhs;

    polyhedron = gPolyhedron_to_sort;
    BrVector3Sub(&tv1, &polyhedron->points[*pIndex1], &polyhedron->points[0]);
    BrVector3Sub(&tv2, &polyhedron->points[*pIndex2], &polyhedron->points[0]);
    s1 = SignedTwiceAreaOf3DTriangle(&tv1, &tv2);
    if (s1 > 1e-6f) {
        return -1;
    }
    if (s1 < -1e-6f) {
        return 1;
    }
    s1 = BrVector3Length(&tv1);
    s2 = BrVector3Length(&tv2);
    if (s1 < s2) {
        return -1;
    }
    if (s1 > s2) {
        return 1;
    }
    return 0;
}
#pragma inline_depth()

// FUNCTION: CARMA2_HW 0x00420910
tPhysicsError C2_HOOK_FASTCALL AddPolyhedronCollisionShapePlanes(tCollision_shape_polyhedron_data* pPolyhedron) {
    int first_vertex;
    br_vector3 tv1;
    br_vector3 tv2;
    br_vector3 tv3;
    int i;
    int indices_buffer[301];
    br_vector3 point_buffer[300];
    int index_min;
    int new_count_points;
    int read_idx;
    int write_idx;

    for (first_vertex = 0; Collinear(&pPolyhedron->points[first_vertex + 0], &pPolyhedron->points[first_vertex + 1], &pPolyhedron->points[first_vertex + 2]); first_vertex++) {
        if (first_vertex == pPolyhedron->count_points - 3) {
            return ePhysicsError_UnknownShapeType;
        }
    }
    BrVector3Sub(&tv1, &pPolyhedron->points[first_vertex + 1], &pPolyhedron->points[first_vertex + 0]);
    BrVector3Sub(&tv2, &pPolyhedron->points[first_vertex + 2], &pPolyhedron->points[first_vertex + 0]);
    BrVector3Cross(&tv3, &tv1, &tv2);
    BrVector3Normalise(&gPhysics_reference_normal_comparison, &tv3);

    index_min = 0;
    for (i = 1; i < pPolyhedron->count_points; i++) {

        if (pPolyhedron->points[i].v[0] < pPolyhedron->points[index_min].v[0]
                || (pPolyhedron->points[i].v[0] == pPolyhedron->points[index_min].v[0]
                        && (pPolyhedron->points[i].v[1] < pPolyhedron->points[index_min].v[1]
                            || (pPolyhedron->points[i].v[1] == pPolyhedron->points[index_min].v[1]
                               && pPolyhedron->points[i].v[2] < pPolyhedron->points[index_min].v[2])))) {
            index_min = i;
        }
    }

    BrVector3Copy(&tv1, &pPolyhedron->points[0]);
    BrVector3Copy(&pPolyhedron->points[0], &pPolyhedron->points[index_min]);
    BrVector3Copy(&pPolyhedron->points[index_min], &tv1);
    for (i = 0; i < pPolyhedron->count_points; i++) {
        indices_buffer[i + 1] = i;
    }
    gPolyhedron_to_sort = pPolyhedron;
    qsort(&indices_buffer[2], pPolyhedron->count_points - 1, sizeof(indices_buffer[1]), (int(C2_HOOK_CDECL*)(const void*, const void*))CompareAngle);
    indices_buffer[0] = indices_buffer[pPolyhedron->count_points];
    indices_buffer[1] = 0;

    new_count_points = 1;
    read_idx = 2;
    write_idx = 1;
    for (i = 1; i < pPolyhedron->count_points; ) {
        BrVector3Sub(&tv1, &pPolyhedron->points[indices_buffer[write_idx]], &pPolyhedron->points[indices_buffer[write_idx - 1]]);
        BrVector3Sub(&tv2, &pPolyhedron->points[indices_buffer[read_idx]], &pPolyhedron->points[indices_buffer[write_idx]]);

        if (SignedTwiceAreaOf3DTriangle(&tv1, &tv2) <= 1e-6f) {
            new_count_points -= 1;
            write_idx -= 1;
        } else {
            write_idx += 1;
            new_count_points += 1;
            indices_buffer[write_idx] = indices_buffer[read_idx];
            read_idx += 1;
            i++;
        }
    }
    pPolyhedron->count_points = new_count_points;
    pPolyhedron->count_edges = new_count_points;
    pPolyhedron->count_planes = 1;
    for (i = 0; i < new_count_points; i++) {
        BrVector3Copy(&point_buffer[i], &pPolyhedron->points[indices_buffer[i]]);
    }
    for (i = 0; i < new_count_points; i++) {
        BrVector3Copy(&pPolyhedron->points[i], &point_buffer[i]);
        pPolyhedron->edges[i].index1 = i + 0;
        pPolyhedron->edges[i].index2 = i + 1;
    }
    pPolyhedron->edges[0].index2 = 0;
    BrVector3Copy(&pPolyhedron->planes[0], &gPhysics_reference_normal_comparison);
    pPolyhedron->planes[0].v[3] = BrVector3Dot(&pPolyhedron->planes[0], &pPolyhedron->points[0]);
    return ePhysicsError_Ok;
}

// FUNCTION: CARMA2_HW 0x004c5eb0
void C2_HOOK_FASTCALL GetBoundsFromPointList(const br_vector3* pVertices, int pCount_vertices, br_bounds3* pBounds) {
    int i;

    BrVector3Copy(&pBounds->min, &pVertices[0]);
    BrVector3Copy(&pBounds->max, &pVertices[0]);
    for (i = 1; i < pCount_vertices; i++) {
        int j;

        for (j = 0; j < 3; j++) {
            if (pVertices[i].v[j] < pBounds->min.v[j]) {
                pBounds->min.v[j] = pVertices[i].v[j];
            } else if (pVertices[i].v[j] > pBounds->max.v[j]) {
                pBounds->max.v[j] = pVertices[i].v[j];
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x00420fb0
tPhysicsError C2_HOOK_FASTCALL ConvexHull3D(tCollision_shape_polyhedron_data* pPolyhedron) {
    int original_count_points;
    br_vector3 original_points[300];
    tPolyhedron_edge_indexes indices[894];

    if (pPolyhedron->count_points > CARPOCALYPSE2_ASIZE(original_points)) {
        pPolyhedron->count_points = CARPOCALYPSE2_ASIZE(original_points);
    }

    original_count_points = pPolyhedron->count_points;
    memcpy(original_points, pPolyhedron->points, sizeof(original_points));
    switch (ProcessTetrahedronPolyhedronCollisionShape(pPolyhedron, indices)) {
    case ePhysicsError_Ok: {
        int new_count_points;
        int i;
        int j;

        new_count_points = 4;
        for (i = 0; i < original_count_points; i++) {

            if (PolyhedronCollisionShape_AddPoint(pPolyhedron, &original_points[i], new_count_points, indices)) {
                if (i != new_count_points) {
                    BrVector3Copy(&pPolyhedron->points[new_count_points], &original_points[i]);
                }
                new_count_points++;
            }
        }
        pPolyhedron->count_points = new_count_points;

        new_count_points = 0;
        for (i = 0; i < pPolyhedron->count_points; i++) {

            for (j = 0; j < pPolyhedron->count_edges; j++) {
                if (i == pPolyhedron->edges[j].index1 || i == pPolyhedron->edges[j].index2) {
                    break;
                }
            }
            if (j >= pPolyhedron->count_edges) {
                continue;
            }
            if (i != new_count_points) {
                BrVector3Copy(&pPolyhedron->points[new_count_points], &pPolyhedron->points[i]);
                for (j = 0; j < pPolyhedron->count_edges; j++) {
                    if (pPolyhedron->edges[j].index1 == i) {
                        pPolyhedron->edges[j].index1 = new_count_points;
                    } else if (pPolyhedron->edges[j].index2 == i) {
                        pPolyhedron->edges[j].index2 = new_count_points;
                    }
                }
            }
            new_count_points++;
        }
        pPolyhedron->count_points = new_count_points;
        return ePhysicsError_Ok;
    }
    default: {
        tPhysicsError err;

        pPolyhedron->count_points = original_count_points;
        memcpy(pPolyhedron->points, original_points, sizeof(original_points));
        err = AddPolyhedronCollisionShapePlanes(pPolyhedron);
        if (err == ePhysicsError_Ok) {
            return ePhysicsError_WireFrameHasNoPoints;
        }
        return err;
    }
    case ePhysicsError_UnknownShapeType:
        return ePhysicsError_UnknownShapeType;
    }
}

// C2PORT-NONMATCHING: best 79.65% | difftest PASS 2000 trials (spec difftest/specs/00422030_FUN_00422030.json) | reason x87-order
// FUNCTION: CARMA2_HW 0x00422030
int C2_HOOK_FASTCALL FUN_00422030(br_model* pModel, int pCount_vertices, int* pVertex_indices) {
    float* distances;
    int removed[512];
    int count_vertices;
    int distance_offset;
    int remove_count;
    int i;

    count_vertices = pModel->nvertices;
    if (count_vertices > 512) {
        count_vertices = 512;
    }
    if (count_vertices <= pCount_vertices) {
        return 3;
    }
    distances = BrMemAllocate(count_vertices * count_vertices / 2 * sizeof(float), 0x80);
    if (distances == NULL) {
        return 4;
    }

    distance_offset = 0;
    i = 0;
    if (count_vertices != 0) {
        int vertex_offset;

        vertex_offset = 0;
        do {
            if (vertex_offset > 0) {
                float* row;
                int j;
                int inner_offset;

                row = distances + distance_offset;
                distance_offset += i;
                j = i;
                inner_offset = 0;
                do {
                    row[0] =
                        (*(float*)((char*)pModel->vertices + vertex_offset + 8) - *(float*)((char*)pModel->vertices + inner_offset + 8)) * (*(float*)((char*)pModel->vertices + vertex_offset + 8) - *(float*)((char*)pModel->vertices + inner_offset + 8))
                        + (*(float*)((char*)pModel->vertices + vertex_offset + 4) - *(float*)((char*)pModel->vertices + inner_offset + 4)) * (*(float*)((char*)pModel->vertices + vertex_offset + 4) - *(float*)((char*)pModel->vertices + inner_offset + 4))
                        + (*(float*)((char*)pModel->vertices + vertex_offset) - *(float*)((char*)pModel->vertices + inner_offset)) * (*(float*)((char*)pModel->vertices + vertex_offset) - *(float*)((char*)pModel->vertices + inner_offset));
                    row++;
                    inner_offset += 0x28;
                    j--;
                } while (j != 0);
            }
            i++;
            vertex_offset += 0x28;
        } while (i < count_vertices);
    }

    memset(removed, 0, count_vertices * sizeof(removed[0]));
    remove_count = count_vertices - pCount_vertices;
    do {
        int index1;
        int index2;
        float min_distance;
        float sum1;
        float sum2;

        min_distance = 3.4028235e+38f;
        for (i = 0; i < count_vertices; i++) {
            if (removed[i] == 0) {
                int j;

                for (j = 0; j < i; j++) {
                    if (removed[j] == 0) {
                        int distance_index;
                        float distance;

                        if (j < i) {
                            distance_index = ((i - 1) * i >> 1) + j;
                        } else {
                            distance_index = ((j - 1) * j >> 1) + i;
                        }
                        distance = distances[distance_index];
                        if (distance < min_distance) {
                            index1 = i;
                            index2 = j;
                            min_distance = distance;
                        }
                    }
                }
            }
        }

        sum1 = gFLOAT_00589954;
        sum2 = gFLOAT_00589954;
        for (i = 0; i < count_vertices; i++) {
            if (removed[i] == 0 && i != index1 && i != index2) {
                int distance_index;

                if (i < index1) {
                    distance_index = ((index1 - 1) * index1 >> 1) + i;
                } else {
                    distance_index = ((i - 1) * i >> 1) + index1;
                }
                sum1 = sqrtf(distances[distance_index]) + sum1;

                if (i < index2) {
                    distance_index = ((index2 - 1) * index2 >> 1) + i;
                } else {
                    distance_index = ((i - 1) * i >> 1) + index2;
                }
                sum2 = sqrtf(distances[distance_index]) + sum2;
            }
        }
        if (sum1 >= sum2) {
            removed[index2] = 1;
        } else {
            removed[index1] = 1;
        }
        remove_count--;
    } while (remove_count != 0);

    for (i = 0; i < count_vertices; i++) {
        if (removed[i] == 0) {
            *pVertex_indices = i;
            pVertex_indices++;
        }
    }
    BrMemFree(distances);
    return 0;
}

// C2PORT-NONMATCHING: best 82.35% | difftest PASS 2000 trials (spec difftest/specs/004222c0_FUN_004222c0.json) | reason allocation-init-regalloc
// FUNCTION: CARMA2_HW 0x004222c0
int C2_HOOK_FASTCALL FUN_004222c0(int* pOut_point_count, int* pVertex_indices, br_model* pModel, int pMin_point_count, int pMax_point_count) {
    tCollision_shape_polyhedron_data* allocation_result;
    int result;
    int point_count;
    int count_points;
    int edge_capacity;
    tCollision_shape_polyhedron_data* polyhedron;
    int count_minus_two;
    int count_plus_two;
    int plane_capacity;

    polyhedron = NULL;
    if (pModel == NULL) {
        return 5;
    }
    if (pMax_point_count < 3) {
        return 6;
    }
    if (pMax_point_count > (int)pModel->nvertices) {
        pMax_point_count = pModel->nvertices;
    }
    if (pMin_point_count < 3) {
        pMin_point_count = 3;
    } else if (pMin_point_count > pMax_point_count) {
        pMin_point_count = pMax_point_count;
    }

#ifdef __vita__
    /* The retail compiler inlined the same packed allocation here. */
    allocation_result = FUN_00421fd0(pMax_point_count);
    polyhedron = allocation_result;
#else
    count_minus_two = pMax_point_count - 2;
    count_plus_two = pMax_point_count + 2;
    edge_capacity = count_minus_two * 3;
    plane_capacity = count_plus_two * 3;
    allocation_result = BrMemAllocate(
            2 * (count_minus_two * 16 + edge_capacity + plane_capacity * 2),
            0x80);
    if (allocation_result != NULL) {
        polyhedron = allocation_result;
        polyhedron->points = (br_vector3*)(polyhedron + 1);
        polyhedron->count_points = 0;
        polyhedron->edges = (tPolyhedron_edge_indexes*)(polyhedron->points + pMax_point_count);
        polyhedron->count_edges = 0;
        polyhedron->count_planes = 0;
        polyhedron->planes = (br_vector4*)(polyhedron->edges + edge_capacity);
    }
#endif
    if (polyhedron != NULL) {
        *pOut_point_count = pMax_point_count;
        point_count = pMax_point_count;
        result = FUN_00422400(polyhedron, pVertex_indices, pModel, point_count);
        count_points = polyhedron->count_points;
        if (count_points != 0) {
            while (1) {
                if (result != 0 && result != 2) {
                    return result;
                }
                point_count = point_count * pMin_point_count / count_points;
                if (count_points == pMin_point_count || point_count >= pMax_point_count) {
                    break;
                }
                *pOut_point_count = point_count;
                result = FUN_00422400(polyhedron, pVertex_indices, pModel, point_count);
                count_points = polyhedron->count_points;
                if (count_points == 0) {
                    return result;
                }
            }
            BrMemFree(polyhedron);
        }
        return result;
    }
    return 4;
}

// FUNCTION: CARMA2_HW 0x004c5f20
void C2_HOOK_FASTCALL FillInShape(tPhysics_shape* pShape) {
    int i;
    tPhysicsError error;

    switch (pShape->common.type) {
    case kCollisionShapeType_Wireframe:
        if (pShape->wireframe.wireframe.count_points == 0) {
            PhysicsError(ePhysicsError_WireFrameHasNoPoints, NULL);
            return;
        }
        GetBoundsFromPointList(pShape->wireframe.wireframe.points, pShape->wireframe.wireframe.count_points, &pShape->common.bb);
        return;
    case kCollisionShapeType_Polyhedron:
        if (pShape->polyhedron.polyhedron.count_points < 4) {
            PhysicsError(ePhysicsError_PolyhedronHasNoPoints, NULL);
            return;
        }
        error = ConvexHull3D(&pShape->polyhedron.polyhedron);
        if (error != ePhysicsError_Ok) {
            if (error == ePhysicsError_WireFrameHasNoPoints) {
                pShape->common.type = kCollisionShapeType_Wireframe_Polyhedron;
            } else {
                PDFatalError("This polyhedron is too wierd for me");
            }
        }
        GetBoundsFromPointList(pShape->polyhedron.polyhedron.points, pShape->polyhedron.polyhedron.count_points, &pShape->common.bb);
        return;
    case kCollisionShapeType_Wireframe_Polyhedron:
        if (pShape->polyhedron.polyhedron.count_points < 3) {
            PhysicsError(4, NULL);
            return;
        }
        AddPolyhedronCollisionShapePlanes(&pShape->polyhedron.polyhedron);
        GetBoundsFromPointList(pShape->polyhedron.polyhedron.points, pShape->polyhedron.polyhedron.count_points, &pShape->common.bb);
        return;
    case kCollisionShapeType_Sphere:
        BrVector3Copy(&pShape->common.bb.min, &pShape->sphere.sphere.center);
        BrVector3Copy(&pShape->common.bb.max, &pShape->sphere.sphere.center);
        for (i = 0; i < 3; i++) {
            pShape->common.bb.min.v[i] -= pShape->sphere.sphere.radius;
            pShape->common.bb.max.v[i] += pShape->sphere.sphere.radius;
        }
        pShape->sphere.sphere.radius_squared = CARPOCALYPSE2_SQR(pShape->sphere.sphere.radius);
        return;
    case kCollisionShapeType_Box:
        return;
    default:
        PhysicsError(ePhysicsError_UnknownShapeType, NULL);
        return;
    }
}

// FUNCTION: CARMA2_HW 0x004da400
tPhysics_object* C2_HOOK_FAKE_THISCALL MungeSphereObject(br_model* pModel, undefined4 pArg2, float pWeight) {
    tPhysics_object* collision_info;

    CARPOCALYPSE2_THISCALL_UNUSED(pArg2);

    C2_HOOK_BUG_ON(sizeof(tPhysics_object) != 1240);

    collision_info = BrMemAllocate(sizeof(tPhysics_object), kMem_collision_object);
    collision_info->shape = (tPhysics_shape*)AllocateShapeSphere(kMem_collision_shape);
    BrVector3Set(&collision_info->shape->sphere.sphere.center, 0.f, 0.f, 0.f);
    collision_info->shape->sphere.sphere.radius = (pModel->bounds.max.v[0] - pModel->bounds.min.v[0] + pModel->bounds.max.v[1] - pModel->bounds.min.v[1] + pModel->bounds.max.v[2] - pModel->bounds.min.v[2]) / 6.f;
    FillInShape(collision_info->shape);
    collision_info->uid = gCollision_info_uid_counter;
    gCollision_info_uid_counter++;
    UpdateCollisionObject(collision_info);
    collision_info->M = pWeight;
    BrVector3Set(&collision_info->I,
        CARPOCALYPSE2_SQR(collision_info->shape->sphere.sphere.radius) * pWeight / 6.f,
        CARPOCALYPSE2_SQR(collision_info->shape->sphere.sphere.radius) * pWeight / 6.f,
        CARPOCALYPSE2_SQR(collision_info->shape->sphere.sphere.radius) * pWeight / 6.f);
    BrVector3SetFloat(&collision_info->field_0x54, 0.f, 1/-17.25f, 0.f);
    collision_info->actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    BrMatrix34Copy(&collision_info->actor->t.t.mat, &collision_info->transform_matrix);
    collision_info->actor->model = pModel;
    collision_info->box_face_ref = gFace_num__car - 2;
    return collision_info;
}

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x004c2b10
void C2_HOOK_FASTCALL PositionChildren(tPhysics_object *pCollision_info) {

    InternalPositionChildren(pCollision_info, pCollision_info);
}
#pragma auto_inline(on)

// FUNCTION: CARMA2_HW 0x004c2b20
void C2_HOOK_FASTCALL InternalPositionChildren(tPhysics_object *pParent, tPhysics_object *pRoot) {
    int flags = pParent->flags;
    tPhysics_object *child = pParent->child;

    if (!(flags & 0x400)) {
        pRoot = pParent;
    }
    for (; child != NULL; child = child->next) {
        if (!(pParent->flags & 0x400)) {
            br_vector3 tv1;
            br_vector3 tv2;
            BrMatrix34ApplyP(&tv1, &child->physics_joint1->field_0x14, &pParent->transform_matrix);
            BrMatrix34ApplyP(&tv2, &child->physics_joint1->field_0x08, &child->transform_matrix);
            BrVector3Sub(&tv1, &tv1, &tv2);
            BrVector3Accumulate((br_vector3*)&child->transform_matrix.m[3], &tv1);
        } else {
            BrMatrix34Mul(&pParent->transform_matrix, &pParent->field_0x1e8, &pRoot->transform_matrix);
        }
        InternalPositionChildren(child, pRoot);
    }
}

// FUNCTION: CARMA2_HW 0x004b63b0
int C2_HOOK_CDECL PHILSetObjectProperty(tPhysics_object *pCollision_info, int pParam, ...) {
    va_list va;
    tCollision_info_owner* owner;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCollision_info_owner, field_0x04, 0x04);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCollision_info_owner, field_0x08, 0x08);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCollision_info_owner, field_0x0c, 0x0c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCollision_info_owner, field_0x10, 0x10);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCollision_info_owner, field_0x14, 0x14);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, field_0x240, 0x240);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, box_face_ref, 0x178);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, flags, 0x19c);

    if (gPHIL_enabled) {
        return 0;
    }
    if (pCollision_info->field_0x239 != 2) {
    }
    owner = (tCollision_info_owner*)pCollision_info->field_0x240;
    if (owner == NULL) {
        return 3;
    }

    va_start(va, pParam);
    switch (pParam) {
        case 0:
            owner->field_0x10 = (float)va_arg(va, double);
            break;
        case 1: {
            int v = !!va_arg(va, int);
            if (v) {
                owner->field_0x04 |= 0x1;
                PositionChildren(pCollision_info);
            } else {
                owner->field_0x04 &= 0x1;
            }
            break;
        }
        case 2:
            owner->field_0x0c = (float)va_arg(va, double);
            break;
        case 3: {
            int v = va_arg(va, int) != 0;
            pCollision_info->flags = (pCollision_info->flags & ~(1 << 6)) | (v << 6);
            if (!v) {
                pCollision_info->box_face_ref = gFace_num__car - 2;
            }
            break;
        }
        case 4: {
            int v = va_arg(va, int) != 0;
            pCollision_info->flags = (pCollision_info->flags & ~(1 << 3)) | (v << 3);
            break;
        }
        case 5: {
            int v = va_arg(va, int) != 0;
            pCollision_info->flags = (pCollision_info->flags & ~(1 << 4)) | (v << 4);
            break;
        }
        case 6: {
            int v = va_arg(va, int) != 0;
            if (v) {
                owner->field_0x04 |= 0x4;
            } else {
                owner->field_0x04 &= 0x4;
            }
            break;
        }
        case 7: {
            int v = va_arg(va, int) != 0;
            if (v) {
                owner->field_0x04 |= 0x8;
                pCollision_info->water_d = 10000.f;
            } else {
                owner->field_0x04 &= 0x8;
            }
            break;
        }
        case 8:
            owner->field_0x14 = (float)va_arg(va, double);
            break;
        default:
            va_end(va);
            return 5;
    }
    va_end(va);
    return 0;
}

// FUNCTION: CARMA2_HW 0x004b6530
float C2_HOOK_FASTCALL PHILGetObjectProperty(tPhysics_object *pCollision_info, int pParam) {
    tCollision_info_owner* owner;

    owner = (tCollision_info_owner*)pCollision_info->field_0x240;
    if (gPHIL_enabled || owner == NULL) {
        return 0.f;
    }
    switch (pParam) {
    case 0:
        return owner->field_0x10;
    case 1:
        return (float)!!(owner->field_0x04 & 0x1);
    case 2:
        return owner->field_0x0c;
    case 3:
        return (float)!!(pCollision_info->flags & 0x40);
    case 4:
        return (float)!!(pCollision_info->flags & 0x8);
    case 5:
        return (float)!!(pCollision_info->flags & 0x10);
    case 6:
        return (float)!!(owner->field_0x04 & 0x4);
    case 7:
        return (float)!!(owner->field_0x04 & 0x8);
    case 8:
        return owner->field_0x14;
    default:
        return 5.f;
    }
}

// FUNCTION: CARMA2_HW 0x004b9ef0
void C2_HOOK_FASTCALL SetCollisionInfoChildsDoNothing(tPhysics_object *pCollision_info, tU32 pDisable) {
    tPhysics_object *sibling;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, field_0x1df, 0x1df);

    pCollision_info->disable_move_rotate = pDisable;
    pCollision_info->field_0x1df = 0;
    if (pCollision_info->child != NULL) {
        sibling = pCollision_info->child;
        while (sibling != NULL) {
            sibling->disable_move_rotate = pDisable;
            sibling->field_0x1df = 0;
            if (sibling->child != NULL) {
                SetCollisionInfoDoNothing(sibling->child, pDisable);
            }
            sibling = sibling->next;
        }
    }
}

// SetCollisionInfoDoNothing

// FUNCTION: CARMA2_HW 0x004c63d0
void C2_HOOK_FASTCALL AddCollisionInfoChild(tPhysics_object* pParent, tPhysics_object* pChild) {
    tPhysics_object* current;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, parent, 0x228);

    pChild->parent = pParent;
    current = pChild;
    while (current->next != NULL) {
        current = current->next;
        current->parent = pParent;
    }
    current->next = pParent->child;
    pParent->child = pChild;
}

// FUNCTION: CARMA2_HW 0x004c6470
int C2_HOOK_FASTCALL PhysicsObjectRecurse(tPhysics_object* pCollision_info, tEnumCollision_cbfn* pCallback, void* pUser_data) {
    int r;
    tPhysics_object* child;

    r = pCallback(pCollision_info, pUser_data);
    if (r != 0) {
        return r;
    }
    for (child = pCollision_info->child; child != NULL; child = child->next) {
        r = PhysicsObjectRecurse(child,pCallback, pUser_data);
        if (r != 0) {
            return r;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004ba4f0
int C2_HOOK_FASTCALL FlagsAreGo(tPhysics_object* pObj1, tPhysics_object* pObj2) {

    if (pObj2->field_0x1a4 & pObj1->field_0x1a0) {
        return 0;
    }
    if (pObj1->field_0x1a4 & pObj2->field_0x1a0) {
        return 0;
    }
    if (pObj1->flags & 4) {
        return 0;
    }
    if (pObj2->flags & 4) {
        return 0;
    }
    return 1;
}

// FUNCTION: CARMA2_HW 0x004ba550
int C2_HOOK_FASTCALL ScaleCollisionImpulses(tCollision_contact* pContacts, float* pImpulses, int pCount, int* pIndices, int pNum_impacts, float pThreshold) {
    int i;
    float total;
    float scale;
    float f;

    total = gFLOAT_0058a768;
    for (i = 0; i < pNum_impacts; i++) {
        total += pImpulses[pIndices[i]];
    }
    if (total <= pThreshold) {
        return 0;
    }
    scale = pThreshold / total;
    gINT_00692dc8 = 1;
    for (i = 0; i < pCount; i++) {
        pImpulses[i] = -pImpulses[i];
    }
    for (i = 0; i < pNum_impacts; i++) {
        f = pImpulses[pIndices[i]];
        pImpulses[pIndices[i]] = f * (0.f - scale);
        if (pContacts[pIndices[i]].type & 0x100) {
            f = pContacts[pIndices[i]].impulse1.v[0];
            pContacts[pIndices[i]].impulse1.v[0] = f * scale;
            f = pContacts[pIndices[i]].impulse1.v[1];
            pContacts[pIndices[i]].impulse1.v[1] = f * scale;
            f = pContacts[pIndices[i]].impulse1.v[2];
            pContacts[pIndices[i]].impulse1.v[2] = f * scale;
            if (pContacts[pIndices[i]].object2 != NULL) {
                f = pContacts[pIndices[i]].impulse1.v[0];
                pContacts[pIndices[i]].impulse1.v[0] = f * scale;
                f = pContacts[pIndices[i]].impulse1.v[1];
                pContacts[pIndices[i]].impulse1.v[1] = f * scale;
                f = pContacts[pIndices[i]].impulse1.v[2];
                pContacts[pIndices[i]].impulse1.v[2] = f * scale;
            }
        }
    }
    for (i = 0; i < pCount; i++) {
        if (pImpulses[i] < gFLOAT_0058a768) {
            pImpulses[i] = 0.f;
        }
        pContacts[i].type = 9;
    }
    return 1;
}

// Box edge table over corner indices (bit2 -> x, bit1 -> y, bit0 -> z).
static const tU8 sBox_shape_edges[12][2] = {
    { 0, 1 }, { 0, 2 }, { 0, 4 },
    { 1, 3 }, { 1, 5 }, { 2, 3 },
    { 2, 6 }, { 3, 7 }, { 4, 5 },
    { 4, 6 }, { 5, 7 }, { 6, 7 },
};

#define TEST_SHAPE_MAX_POINTS 512

// UNLABELED: CARMA2_HW 0x00428dc0
// One-direction convex-shape overlap worker: transforms shapeA's vertices
// into shapeB's frame and reports whether any shapeA edge touches shapeB's
// bounding box (expanded by 5mm). Callers invoke both directions.
// NOTE: for polyhedral shapeB the original runs segment-vs-face tests; this
// uses the bounding box instead (safe direction: overlapping rather than
// clear). The original also only tests the first shapeA in a chain; shape
// chains are single-element in practice.
int C2_HOOK_FASTCALL TestShapeOverlap(tPhysics_object* pObjA, tPhysics_object* pObjB, tPhysics_shape* pShapeA, tPhysics_shape* pShapeB) {
    br_vector3 points[TEST_SHAPE_MAX_POINTS];
    br_vector3 world;
    br_bounds3 expanded;
    tCollision_shape_polyhedron_data* poly;
    const tU8 (*edges)[2];
    int edge_count;
    int count;
    int i;
    int e;

    switch (pShapeA->common.type) {
    case kCollisionShapeType_Polyhedron:
    case kCollisionShapeType_Wireframe_Polyhedron:
        /* NB: wireframe shapes share the polyhedron data head layout. */
        poly = &pShapeA->polyhedron.polyhedron;
        count = poly->count_points;
        if (count > TEST_SHAPE_MAX_POINTS) {
            count = TEST_SHAPE_MAX_POINTS;
        }
        for (i = 0; i < count; i++) {
            BrMatrix34ApplyP(&world, &poly->points[i], &pObjA->transform_matrix);
            BrVector3Sub(&world, &world, (br_vector3*)&pObjB->transform_matrix.m[3]);
            BrMatrix34TApplyV(&points[i], &world, &pObjB->transform_matrix);
        }
        edges = (const tU8(*)[2])poly->edges;
        edge_count = poly->count_edges;
        break;
    case kCollisionShapeType_Box:
    case kCollisionShapeType_Wireframe:
    case kCollisionShapeType_Sphere:
        for (i = 0; i < 8; i++) {
            world.v[0] = (i & 4) ? pShapeA->common.bb.max.v[0] : pShapeA->common.bb.min.v[0];
            world.v[1] = (i & 2) ? pShapeA->common.bb.max.v[1] : pShapeA->common.bb.min.v[1];
            world.v[2] = (i & 1) ? pShapeA->common.bb.max.v[2] : pShapeA->common.bb.min.v[2];
            BrMatrix34ApplyP(&points[i], &world, &pObjA->transform_matrix);
            BrVector3Sub(&points[i], &points[i], (br_vector3*)&pObjB->transform_matrix.m[3]);
            BrMatrix34TApplyV(&world, &points[i], &pObjB->transform_matrix);
            points[i] = world;
        }
        count = 8;
        edges = sBox_shape_edges;
        edge_count = 12;
        break;
    default:
        return 0;
    }
    for (i = 0; i < 3; i++) {
        expanded.min.v[i] = pShapeB->common.bb.min.v[i] - 0.005f;
        expanded.max.v[i] = pShapeB->common.bb.max.v[i] + 0.005f;
    }
    for (e = 0; e < edge_count; e++) {
        if (edges[e][0] >= count || edges[e][1] >= count) {
            continue;
        }
        if (TestSegmentBox(&points[edges[e][0]], &points[edges[e][1]], &expanded)) {
            return 1;
        }
    }
    return 0;
}

// UNLABELED: CARMA2_HW 0x00423320
// Refreshes the shape bounds, then culls the object's track faces in its
// box-face range by plane distance. Faces whose material name starts with
// '!' or '>' are sticky-marked 0x80 (later raycasts skip marked faces);
// kept faces are counted. Returns the kept count.
int C2_HOOK_FASTCALL CullPlacementFaces(tPhysics_object* pObject, tPhysics_shape* pShape, br_matrix34* pMat) {
    tFace_ref* face;
    br_bounds3* bounds;
    int count;
    int i;
    int j;
    float d;

    GetNewBoundingBox(&pShape->common.field_0x1c, &pShape->common.bb, pMat);
    bounds = &pShape->common.field_0x1c;
    /* The refreshed bounds are actor-relative; shift to world frame like the
       original (which folds the actor translation in). */
    for (i = 0; i < 3; i++) {
        bounds->min.v[i] += pObject->actor->t.t.mat.m[3][i];
        bounds->max.v[i] += pObject->actor->t.t.mat.m[3][i];
    }
    count = 0;
    for (i = pObject->box_face_start; i < pObject->box_face_end; i++) {
        face = &gFace_list__car[i];
        /* Rough plane-slab reject first (generous: keep on doubt). */
        for (j = 0; j < 3; j++) {
            if (face->normal.v[j] > 0.f) {
                d = face->normal.v[j] * bounds->max.v[j];
            } else {
                d = face->normal.v[j] * bounds->min.v[j];
            }
            if (d < face->d - 0.005f) {
                break;
            }
            if (face->normal.v[j] > 0.f) {
                d = face->normal.v[j] * bounds->min.v[j];
            } else {
                d = face->normal.v[j] * bounds->max.v[j];
            }
            if (d > face->d + 0.005f) {
                break;
            }
        }
        if (j < 3) {
            continue;
        }
        if (face->material != NULL && face->material->identifier != NULL) {
            if (face->material->identifier[0] == '!') {
                face->flags |= 0x80;
                continue;
            }
            if (face->material->identifier[0] == '>') {
                face->flags |= 0x80;
                continue;
            }
        }
        count++;
    }
    return count;
}

// UNLABELED: CARMA2_HW 0x004228b0
// Object-vs-track-face contact generator. The retail function (0x4228b0) walks
// the shape chain and dispatches to per-shape helpers (0x4236a0/0x423ed0/
// 0x424140, and the type-4 face scan via 0x423c00) which append
// tCollision_contact records at pContacts + pCount and return how many were
// added. We reproduce the outer contract and record layout (contact plane test
// against gFace_list__car over [box_face_start, box_face_end), normal and r1 in
// transform_matrix space) with a single-box corner approximation:
//  - box_face_start == box_face_end -> 0,
//  - pCount > 0x24 -> 0, appends stop at 0x28 contacts (matches 0x422900/
//    0x422cec `0x28-count` limits),
//  - field_0xed receives the accumulated per-shape flags byte (0 here),
//  - omega is reloaded from rotate_omega when contacts were appended
//    (0x422fa1), impulsive response is left to the PFC callback.
int C2_HOOK_FASTCALL CollectPlacementFaces(tPhysics_object* pObject, tCollision_contact* pContacts, int pCount) {
    br_matrix34 inv_transform;
    br_vector3 corners[8];
    br_vector3 local;
    br_vector3 point;
    br_vector3 rel;
    tFace_ref* face;
    tCollision_contact* contact;
    float d;
    float deepest;
    int i;
    int j;
    int added;

    if (pObject->box_face_start == pObject->box_face_end) {
        return 0;
    }
    if (pCount > 0x24) {
        return 0;
    }
    BrMatrix34LPInverse(&inv_transform, &pObject->transform_matrix);
    for (i = 0; i < 8; i++) {
        local.v[0] = (i & 4) ? pObject->bb1.max.v[0] : pObject->bb1.min.v[0];
        local.v[1] = (i & 2) ? pObject->bb1.max.v[1] : pObject->bb1.min.v[1];
        local.v[2] = (i & 1) ? pObject->bb1.max.v[2] : pObject->bb1.min.v[2];
        BrMatrix34ApplyP(&corners[i], &local, &pObject->transform_matrix);
    }
    added = 0;
    for (i = pObject->box_face_start; i < pObject->box_face_end && pCount + added < 0x28; i++) {
        face = &gFace_list__car[i];
        if ((face->flags & 0x80) != 0) {
            continue;
        }
        deepest = 1.e30f;
        for (j = 0; j < 8; j++) {
            d = BrVector3Dot(&face->normal, &corners[j]) - face->d;
            if (d < deepest) {
                deepest = d;
                BrVector3Copy(&point, &corners[j]);
            }
        }
        if (deepest > 0.02f) {
            continue;
        }
        contact = &pContacts[pCount + added];
        contact->impulse = 0.f;
        contact->object1 = pObject;
        contact->object2 = NULL;
        contact->field_0xc = face;
        contact->field_0x10 = 0.f;
        BrMatrix34ApplyP(&rel, &point, &inv_transform);
        BrVector3Copy(&contact->r1, &rel);
        BrVector3SetFloat(&contact->r2, 0.f, 0.f, 0.f);
        BrMatrix34TApplyV(&contact->normal, &face->normal, &pObject->transform_matrix);
        BrVector3SetFloat(&contact->field_0x38, 0.f, 0.f, 0.f);
        BrVector3SetFloat(&contact->impulse1, 0.f, 0.f, 0.f);
        BrVector3SetFloat(&contact->impulse2, 0.f, 0.f, 0.f);
        contact->type = 0;
        added++;
    }
    pObject->field_0xed = 0;
    if (added == 0) {
        return 0;
    }
    BrVector3Copy(&pObject->omega, &pObject->rotate_omega);
    return added;
}

#pragma auto_inline(off)
#pragma inline_depth(0)
// Pair overlap test used by the sensible-place machinery: rejects trivially
// disjoint pairs (self, parent links, flags, bounds), runs the shape worker
// both directions, then recurses over p2's children. Returns 1 on overlap.
// FUNCTION: CARMA2_HW 0x004293c0
int C2_HOOK_FASTCALL TestPairOverlap(tPhysics_object* pObj1, tPhysics_object* pObj2) {
    tPhysics_shape* shape1;
    tPhysics_shape* shape2;
    tPhysics_object* child;

    if (pObj2 == pObj1) {
        return 0;
    }
    if (pObj1->parent == pObj2) {
        return 0;
    }
    if (pObj2->parent == pObj1) {
        return 0;
    }
    shape1 = pObj1->shape;
    shape2 = pObj2->shape;
    if (!FlagsAreGo(pObj1, pObj2)) {
        return 0;
    }
    if (!BoundsOverlapTest__finteray(&pObj1->field_0xf4, &pObj2->field_0xf4)) {
        return 0;
    }
    if (shape1 == NULL) {
        return 0;
    }
    /* NOTE: same degenerate chain walk as TestObjectOverlap (see above). */
    for (; shape1 != NULL; shape1 = shape1->common.next) {
        for (; shape2 != NULL; shape2 = shape2->common.next) {
            if (TestShapeOverlap(pObj1, pObj2, shape1, shape2)) {
                return 1;
            }
            if (TestShapeOverlap(pObj2, pObj1, shape2, shape1)) {
                return 1;
            }
        }
    }
    for (child = pObj2->child; child != NULL; child = child->next) {
        if (TestPairOverlap(pObj1, child)) {
            return 1;
        }
    }
    return 0;
}

// C2PORT-NONMATCHING: best 73.22% | difftest PASS 3000 trials (spec difftest/specs/00429490_FUN_00429490.json) | reason VC5 register allocation (loop vars/flags/spilled object pointers); stub arg4 &factor stack address compared only within tolerance, dead local count not observable
// FUNCTION: CARMA2_HW 0x00429490
void C2_HOOK_FASTCALL FUN_00429490(tPhysics_object* pObject) {
    tPhysics_object* object2;
    br_matrix34 relative_mat;
    br_matrix34 inverse_mat;
    br_vector3 corners1[8];
    br_vector3 corners2[8];
    br_vector3 point;
    br_vector3 delta;
    float factor;
    int count;
    int i;
    int bit_x;
    int bit_y;
    int bit_z;
    tCollision_shape_common* shape;
    int j;

    count = 0;
    for (; pObject != NULL; pObject = pObject->next) {
        for (object2 = pObject->next; object2 != NULL; object2 = object2->next) {
            BrMatrix34LPInverse(&inverse_mat, &object2->transform_matrix);
            BrMatrix34Mul(&relative_mat, &pObject->transform_matrix, &inverse_mat);
            if (pObject->shape->common.type != kCollisionShapeType_Box || object2->shape->common.type != kCollisionShapeType_Box) {
                continue;
            }
            for (i = 0; i < 8; i++) {
                shape = &pObject->shape->common;
                bit_x = i & 2;
                if (bit_x) {
                    point.v[0] = shape->bb.min.v[0];
                } else {
                    point.v[0] = shape->bb.max.v[0];
                }
                bit_y = i & 1;
                if (bit_y) {
                    point.v[1] = shape->bb.min.v[1];
                } else {
                    point.v[1] = shape->bb.max.v[1];
                }
                bit_z = i & 4;
                if (bit_z) {
                    point.v[2] = shape->bb.min.v[2];
                } else {
                    point.v[2] = shape->bb.max.v[2];
                }
                BrMatrix34ApplyP(&corners1[i], &point, &relative_mat);
                shape = &object2->shape->common;
                if (bit_x) {
                    point.v[0] = shape->bb.min.v[0];
                } else {
                    point.v[0] = shape->bb.max.v[0];
                }
                if (bit_y) {
                    point.v[1] = shape->bb.min.v[1];
                } else {
                    point.v[1] = shape->bb.max.v[1];
                }
                if (bit_z) {
                    point.v[2] = shape->bb.min.v[2];
                } else {
                    point.v[2] = shape->bb.max.v[2];
                }
                delta.v[0] = point.v[0] - relative_mat.m[3][0];
                delta.v[1] = point.v[1] - relative_mat.m[3][1];
                delta.v[2] = point.v[2] - relative_mat.m[3][2];
                BrMatrix34TApplyV(&corners2[i], &delta, &relative_mat);
            }
            for (j = 0; j < 12; j++) {
                if (LineBoxColl(&corners1[gBox_edges_0058f7f0[j].index1], &corners1[gBox_edges_0058f7f0[j].index2], &object2->shape->common.bb, &point, &factor)
                        || LineBoxColl(&corners2[gBox_edges_0058f7f0[j].index1], &corners2[gBox_edges_0058f7f0[j].index2], &pObject->shape->common.bb, &point, &factor)) {
                    count++;
                    break;
                }
            }
        }
    }
}

#pragma inline_depth()

#pragma auto_inline(off)
#pragma inline_depth(0)
// FUNCTION: CARMA2_HW 0x00429070
int C2_HOOK_FASTCALL TestForObjectInSensiblePlace(tPhysics_object* pCollision_info_1, tPhysics_object* pCollision_info_2, br_vector3* pVec3, tWorld_callbacks* pCar_callbacks) {
    tCollision_contact contacts[0x28];
    tPhysics_object* p1;
    tPhysics_object* p2;
    tPhysics_object* child;
    tPhysics_shape* shape1;
    tPhysics_shape* shape2;
    tPhysics_shape* s1;
    tPhysics_shape* s2;
    br_vector3 dir;
    float len;
    int count;
    int i;
    int found;

    gSensible_place_blocker = NULL;
    p1 = pCollision_info_1;
    p2 = pCollision_info_2;
    GetNewBoundingBox(&pCollision_info_1->field_0xf4, &pCollision_info_1->bb1, &pCollision_info_1->transform_matrix);
    GetFacesInBox(pCollision_info_1, pCar_callbacks);
    gSensible_place_collect_active = 1;
#if UINTPTR_MAX > 0xffffffffu
    /* The Vita-port collector (FUN_004228b0) hardcodes the original 32-bit
     * tCollision_contact stride (0x60) and object/material offsets, so it is
     * only valid for the matching build.  Keep the portable reconstruction on
     * 64-bit hosts. */
    count = CollectPlacementFaces(pCollision_info_1, contacts, 0);
#else
    count = FUN_004228b0(pCollision_info_1, contacts, 0);
#endif
    gSensible_place_collect_active = 0;
    if (count != 0) {
        float x = contacts[0].normal.v[0];
        float y = contacts[0].normal.v[1];
        float z = contacts[0].normal.v[2];
        if (count > 1) {
            i = 1;
            do {
                x += contacts[i].normal.v[0];
                y += contacts[i].normal.v[1];
                z += contacts[i].normal.v[2];
                i++;
            } while (i < count);
            len = sqrtf(x * x + y * y + z * z);
            if (len != 0.f) {
                len = 1.0 / len;
                x = x * len * pVec3->v[0];
                y = y * len * pVec3->v[1];
                z = z * len * pVec3->v[2];
            } else {
                x = 1.f;
                y = 0.f;
                z = 0.f;
            }
        }
        dir.v[0] = x;
        dir.v[1] = y;
        dir.v[2] = z;
        BrMatrix34ApplyV(pVec3, &dir, &pCollision_info_1->transform_matrix);
        return 0;
    }
    if (p2 == NULL) {
        return -1;
    } {
        do {
            if (p2 == p1) {
                continue;
            }
            if (p1->parent == p2) {
                continue;
            }
            if (p2->parent == p1) {
                continue;
            }
            if (!FlagsAreGo(p1, p2)) {
                continue;
            }
            if (!BoundsOverlapTest__finteray(&p1->field_0xf4, &p2->field_0xf4)) {
                continue;
            }
            shape1 = p1->shape;
            shape2 = p2->shape;
            found = 0;
            for (s2 = shape2; s2 != NULL; s2 = s2->common.next) {
                for (s1 = shape1; s1 != NULL; s1 = s1->common.next) {
                    if (TestShapeOverlap(p1, p2, s1, s2)
                        || TestShapeOverlap(p2, p1, s2, s1)) {
                        found = 1;
                        break;
                    }
                }
                if (found) {
                    break;
                }
            }
            if (!found) {
                for (child = p2->child; child != NULL; child = child->next) {
                    if (TestPairOverlap(p1, child)) {
                        found = 1;
                        break;
                    }
                }
            }
            if (!found) {
                continue;
            }
            pVec3->v[0] = p1->field_0x10c.min.v[0] + p1->field_0x10c.max.v[0];
            pVec3->v[1] = p1->field_0x10c.max.v[1] + p1->field_0x10c.min.v[1];
            pVec3->v[2] = p1->field_0x10c.max.v[2] + p1->field_0x10c.min.v[2];
            pVec3->v[0] -= p2->field_0x10c.min.v[0];
            pVec3->v[1] -= p2->field_0x10c.min.v[1];
            pVec3->v[2] -= p2->field_0x10c.min.v[2];
            pVec3->v[0] -= p2->field_0x10c.max.v[0];
            pVec3->v[1] -= p2->field_0x10c.max.v[1];
            pVec3->v[2] -= p2->field_0x10c.max.v[2];
            len = sqrtf(BrVector3Dot(pVec3, pVec3));
            if (len != 0.f) {
                float inv = 1.0 / len;
                pVec3->v[0] *= inv;
                pVec3->v[1] *= inv;
                pVec3->v[2] *= inv;
            } else {
                BrVector3Set(pVec3, 1.f, 0.f, 0.f);
            }
            gSensible_place_blocker = p2;
            return 0;
        } while ((p2 = p2->next) != NULL);
    }
    return -1;
}
#pragma inline_depth()
#pragma auto_inline(on)

#pragma inline_depth(0)
// FUNCTION: CARMA2_HW 0x004da290
tPhysics_object* C2_HOOK_FAKE_THISCALL MungeBoxObject(br_model* pModel, undefined4 pArg2, float pMass) {
    tPhysics_object* box;

    C2_HOOK_BUG_ON(sizeof(tPhysics_object) != 0x4d8);

    box = BrMemAllocate(sizeof(tPhysics_object), kMem_collision_object);
    box->shape = (tPhysics_shape*)AllocateBoxCollisionShape(kMem_collision_shape);
    box->shape->box.common.bb = pModel->bounds;
    FillInShape(box->shape);
    box->uid = gCollision_info_uid_counter;
    gCollision_info_uid_counter += 1;
    UpdateCollisionObject(box);
    box->M = pMass;
    BrVector3Set(&box->I,
        (CARPOCALYPSE2_SQR((pModel->bounds.max.v[1] - pModel->bounds.min.v[1]) * .5f) + CARPOCALYPSE2_SQR((pModel->bounds.max.v[2] - pModel->bounds.min.v[2]) * .5f)) * pMass * (1.f / 12.f),
        (CARPOCALYPSE2_SQR((pModel->bounds.max.v[0] - pModel->bounds.min.v[0]) * .5f) + CARPOCALYPSE2_SQR((pModel->bounds.max.v[2] - pModel->bounds.min.v[2]) * .5f)) * pMass * (1.f / 12.f),
        (CARPOCALYPSE2_SQR((pModel->bounds.max.v[0] - pModel->bounds.min.v[0]) * .5f) + CARPOCALYPSE2_SQR((pModel->bounds.max.v[1] - pModel->bounds.min.v[1]) * .5f)) * pMass * (1.f / 12.f));
    BrVector3SetFloat(&box->field_0x54, 0.f, 1/-17.25f, 0.f);
    box->actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    BrMatrix34Copy(&box->actor->t.t.mat, &box->transform_matrix);
    box->actor->model = pModel;
    box->box_face_ref = gFace_num__car - 2;
    return box;
}
#pragma inline_depth()

// FUNCTION: CARMA2_HW 0x004c5e20
tPhysics_joint* C2_HOOK_FASTCALL AllocatePhysicsJoint(int pCount_limits, int pType) {

    C2_HOOK_BUG_ON(offsetof(tPhysics_joint, limits) != 0x54);
    C2_HOOK_BUG_ON(sizeof(tPhysics_joint_limit) != 0x20);

    return BrMemAllocate(offsetof(tPhysics_joint, limits) + pCount_limits * sizeof(tPhysics_joint_limit), pType);
}

// FUNCTION: CARMA2_HW 0x004c5e40
tPhysics_joint* C2_HOOK_FASTCALL ClonePhysicsJoint(const tPhysics_joint* pJoint, int pType) {
    size_t size;
    tPhysics_joint* clone;

    size = offsetof(tPhysics_joint, limits) + pJoint->count_limits * sizeof(tPhysics_joint_limit);
    clone = BrMemAllocate(size, pType);
    memcpy(clone, pJoint, size);
    return clone;
}

// FUNCTION: CARMA2_HW 0x004c63b0
void C2_HOOK_FASTCALL PhysicsAddObject(tPhysics_object* pParent, tPhysics_object* pChild) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, next, 0x220);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, child, 0x224);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, parent, 0x228);

    pChild->next = pParent->child;
    pParent->child = pChild;
    pChild->parent = pParent;
}

// FUNCTION: CARMA2_HW 0x004b5fd0
tPhysics_object* C2_HOOK_FASTCALL PHILGetFirstObject(void) {
    if (gPHIL_enabled) {
        return NULL;
    }
    return gList_collision_infos;
}

// FUNCTION: CARMA2_HW 0x004b5ff0
tPhysics_object* C2_HOOK_FASTCALL PHILGetNextObject(tPhysics_object* pCollision_info) {
    if (gPHIL_enabled) {
        return NULL;
    }
    return pCollision_info->next;
}

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x004b7510
/* Non-matching body of MoveJointedObject (0x4b7510).
 * Retail entry does `xor edx, edx` immediately, so pArg2 is unused.
 * The leaf path (0x4b7743) does actor->t.t.mat.m[3] += v * pDt then calls the
 * omega integrator at 0x4c1be0; objects with children go through 0x4b8ee0,
 * which we approximate by applying the same integration to the whole subtree. */
void C2_HOOK_FAKE_THISCALL MoveJointedObject(tPhysics_object* pObject, undefined4 pArg2, float pDelta_time) {
    tPhysics_object* pChild;

    (void)pArg2;
    if (pDelta_time != 0.f) {
        pObject->actor->t.t.mat.m[3][0] += pObject->v.v[0] * pDelta_time;
        pObject->actor->t.t.mat.m[3][1] += pObject->v.v[1] * pDelta_time;
        pObject->actor->t.t.mat.m[3][2] += pObject->v.v[2] * pDelta_time;

        /* 0x4c1be0: rotate about the centre of mass by omega * pDt */
        {
            br_matrix34 rot;
            br_vector3 axis;
            br_scalar len2;

            len2 = pObject->omega.v[0] * pObject->omega.v[0]
                 + pObject->omega.v[1] * pObject->omega.v[1]
                 + pObject->omega.v[2] * pObject->omega.v[2];
            if (len2 > 1.e-7f) {
                br_scalar len;

                len = sqrtf(len2);
                axis.v[0] = pObject->omega.v[0] / len;
                axis.v[1] = pObject->omega.v[1] / len;
                axis.v[2] = pObject->omega.v[2] / len;
                BrMatrix34Rotate(&rot, BR_ANGLE_RAD(len * pDelta_time), &axis);
                BrMatrix34PreTranslate(&rot,
                    -pObject->cmpos.v[0], -pObject->cmpos.v[1], -pObject->cmpos.v[2]);
                BrMatrix34PostTranslate(&rot,
                    pObject->cmpos.v[0], pObject->cmpos.v[1], pObject->cmpos.v[2]);
                BrMatrix34Pre(&pObject->actor->t.t.mat, &rot);
            }
        }
    }
    for (pChild = pObject->child; pChild != NULL; pChild = pChild->next) {
        MoveJointedObject(pChild, 0, pDelta_time);
    }
}

// FUNCTION: CARMA2_HW 0x004b8ee0
void C2_HOOK_FAKE_THISCALL FUN_004b8ee0(tPhysics_object* pObject, undefined4 pArg2, float pDelta_time) {
    tPhysics_object* changed_object;
    tPhysics_object* child;
    tPhysics_object* matrix_parent;
    br_vector3 child_joint_pos;
    br_vector3 parent_joint_pos;

    (void)pArg2;
    changed_object = NULL;
    FUN_004b9230(pObject, &changed_object, pDelta_time);
    if (changed_object == NULL || changed_object->field_0x230 == NULL) {
        return;
    }
    child = changed_object;
    ((void (C2_HOOK_FAKE_THISCALL *)(tPhysics_object*, undefined4, float))FUN_004b8fe0)(child, *(undefined4*)&pDelta_time, pDelta_time);
    matrix_parent = child->field_0x230;
    do {
        BrMatrix34ApplyP(&child_joint_pos, &child->physics_joint1->field_0x08, &child->actor->t.t.mat);
        BrMatrix34ApplyP(&parent_joint_pos, &child->physics_joint1->field_0x14, &matrix_parent->actor->t.t.mat);
        child_joint_pos.v[0] -= parent_joint_pos.v[0];
        child_joint_pos.v[1] -= parent_joint_pos.v[1];
        child_joint_pos.v[2] -= parent_joint_pos.v[2];
        child->parent->actor->t.t.mat.m[3][0] += child_joint_pos.v[0];
        child->parent->actor->t.t.mat.m[3][1] += child_joint_pos.v[1];
        child->parent->actor->t.t.mat.m[3][2] += child_joint_pos.v[2];
        child = matrix_parent;
        matrix_parent = child->field_0x230;
    } while (matrix_parent != NULL);
    WriteNewPositions(pObject);
}

// C2PORT-NONMATCHING: best 47.77% | difftest PASS 2000 trials (spec difftest/specs/004b8fe0_FUN_004b8fe0.json) | reason x87 order and register/stack allocation
// FUNCTION: CARMA2_HW 0x004b8fe0
void C2_HOOK_FAKE_THISCALL FUN_004b8fe0(tPhysics_object* pObject, undefined4 pArg2, float pDelta_time) {
    br_vector3 joint_axis;
    br_vector3 joint_position;
    br_vector3 old_translation;
    float axis_offset;
    float axis_speed;

    (void)pArg2;
    if (pObject->physics_joint2->type == eJoint_translation) {
        BrMatrix34ApplyV(&joint_axis, &pObject->physics_joint2->hinge_axis, &pObject->transform_matrix);
        old_translation.v[0] = pObject->actor->t.t.mat.m[3][0];
        old_translation.v[1] = pObject->actor->t.t.mat.m[3][1];
        old_translation.v[2] = pObject->actor->t.t.mat.m[3][2];
        axis_offset = old_translation.v[0] * joint_axis.v[0]
                    + old_translation.v[1] * joint_axis.v[1]
                    + old_translation.v[2] * joint_axis.v[2];
        BrMatrix34ApplyP(&joint_position, &pObject->physics_joint2->field_0x08, &pObject->actor->t.t.mat);
        pObject->actor->t.t.mat.m[3][0] += pObject->field_0x17c.v[0] - joint_position.v[0];
        pObject->actor->t.t.mat.m[3][1] += pObject->field_0x17c.v[1] - joint_position.v[1];
        pObject->actor->t.t.mat.m[3][2] += pObject->field_0x17c.v[2] - joint_position.v[2];
        axis_offset -= pObject->actor->t.t.mat.m[3][0] * joint_axis.v[0]
                     + pObject->actor->t.t.mat.m[3][1] * joint_axis.v[1]
                     + pObject->actor->t.t.mat.m[3][2] * joint_axis.v[2];
        pObject->actor->t.t.mat.m[3][0] += axis_offset * joint_axis.v[0];
        pObject->actor->t.t.mat.m[3][1] += axis_offset * joint_axis.v[1];
        pObject->actor->t.t.mat.m[3][2] += axis_offset * joint_axis.v[2];
        axis_speed = pObject->v.v[0] * joint_axis.v[0]
                   + pObject->v.v[1] * joint_axis.v[1]
                   + pObject->v.v[2] * joint_axis.v[2];
        pObject->v.v[0] = axis_speed * joint_axis.v[0];
        pObject->v.v[1] = axis_speed * joint_axis.v[1];
        pObject->v.v[2] = axis_speed * joint_axis.v[2];
        pObject->actor->t.t.mat.m[3][0] += pDelta_time * axis_speed * joint_axis.v[0];
        pObject->actor->t.t.mat.m[3][1] += pDelta_time * axis_speed * joint_axis.v[1];
        pObject->actor->t.t.mat.m[3][2] += pDelta_time * axis_speed * joint_axis.v[2];
    } else {
        BrMatrix34ApplyP(&joint_position, &pObject->physics_joint2->field_0x08, &pObject->actor->t.t.mat);
        pObject->actor->t.t.mat.m[3][0] += pObject->field_0x17c.v[0] - joint_position.v[0];
        pObject->actor->t.t.mat.m[3][1] += pObject->field_0x17c.v[1] - joint_position.v[1];
        pObject->actor->t.t.mat.m[3][2] += pObject->field_0x17c.v[2] - joint_position.v[2];
    }
}

// C2PORT-NONMATCHING: best 51.06% | difftest PASS 2000 trials (spec difftest/specs/004b9230_FUN_004b9230.json) | reason x87-order/regalloc/stack-layout
// FUNCTION: CARMA2_HW 0x004b9230
void C2_HOOK_FAKE_THISCALL FUN_004b9230(tPhysics_object* pObject, tPhysics_object** ppChanged_object, float pDelta_time) {
    tPhysics_object* parent;
    tPhysics_object* child;
    br_matrix34 inverse_parent_matrix;
    br_matrix34 relative_matrix;
    br_scalar angle;
    union {
        struct {
            br_vector3 inverse_position;
            br_vector3 relative_position;
            br_vector3 angular_velocity;
        } parent_motion;
        struct {
            br_vector3 parent_omega_world;
            br_vector3 parent_omega_local;
            br_vector3 scratch;
        } hinge_motion;
        struct {
            br_vector3 child_joint_position;
            br_vector3 parent_joint_position;
            br_vector3 scratch;
        } joint_positions;
    } vectors;

    if (pObject->physics_joint1 == NULL || pObject->physics_joint1->type == eJoint_none) {
        if (pObject->physics_joint2 != NULL && pObject->physics_joint2->type != eJoint_none) {
            RotateObject(pObject, (undefined4)(uintptr_t)ppChanged_object, pDelta_time);
            FUN_004b8fe0(pObject, (undefined4)(uintptr_t)ppChanged_object, pDelta_time);
        } else {
            pObject->actor->t.t.mat.m[3][0] += pObject->v.v[0] * pDelta_time;
            pObject->actor->t.t.mat.m[3][1] += pObject->v.v[1] * pDelta_time;
            pObject->actor->t.t.mat.m[3][2] += pObject->v.v[2] * pDelta_time;
            RotateObject(pObject, (undefined4)(uintptr_t)ppChanged_object, pDelta_time);
        }
    } else {
        parent = pObject->field_0x230;
        if (parent == NULL) {
            PhysicsError(11, gEmpty_string_00676920);
        }
        if (pObject->flags & 0x400) {
            BrMatrix34Mul(&pObject->actor->t.t.mat, &pObject->field_0x1e8, &parent->actor->t.t.mat);
            BrMatrix34TApplyV(&pObject->omega, &parent->rotate_omega, &pObject->field_0x1e8);
            pObject->rotate_omega = pObject->omega;
            DrMatrix34ApplyLPInverse(&vectors.parent_motion.inverse_position, &parent->cmpos, &pObject->field_0x1e8);
            BrVector3Sub(&vectors.parent_motion.relative_position, &pObject->cmpos, &vectors.parent_motion.inverse_position);
            BrVector3Cross(&vectors.parent_motion.angular_velocity, &pObject->omega, &vectors.parent_motion.relative_position);
            BrMatrix34ApplyV(&pObject->v, &vectors.parent_motion.angular_velocity, &pObject->transform_matrix);
            pObject->v.v[0] += parent->v.v[0];
            pObject->v.v[1] += parent->v.v[1];
            pObject->v.v[2] += parent->v.v[2];
            BrMatrix34TApplyV(&pObject->velocity_car_space, &pObject->v, &pObject->transform_matrix);
        } else {
            switch (pObject->physics_joint1->type) {
            case eJoint_hinge:
            case eJoint_quick_hinge:
                BrMatrix34ApplyV(&vectors.hinge_motion.parent_omega_world, &parent->rotate_omega, &parent->transform_matrix);
                BrMatrix34TApplyV(&vectors.hinge_motion.parent_omega_local, &vectors.hinge_motion.parent_omega_world, &pObject->transform_matrix);
                angle = ((pObject->omega.v[0] - vectors.hinge_motion.parent_omega_local.v[0]) * pObject->physics_joint1->hinge_axis.v[0]
                       + (pObject->omega.v[1] - vectors.hinge_motion.parent_omega_local.v[1]) * pObject->physics_joint1->hinge_axis.v[1]
                       + (pObject->omega.v[2] - vectors.hinge_motion.parent_omega_local.v[2]) * pObject->physics_joint1->hinge_axis.v[2]) * pDelta_time;
                BrMatrix34LPInverse(&inverse_parent_matrix, &parent->transform_matrix);
                BrMatrix34Mul(&relative_matrix, &pObject->transform_matrix, &inverse_parent_matrix);
                BrMatrix34Mul(&pObject->actor->t.t.mat, &relative_matrix, &parent->actor->t.t.mat);
                BrMatrix34PreRotate(&pObject->actor->t.t.mat, (br_angle)(angle * gDOUBLE_0058a770), &pObject->physics_joint1->hinge_axis);
                pObject->rotate_omega = pObject->omega;
                BrMatrix34ApplyV(&vectors.hinge_motion.scratch, &pObject->omega, &pObject->transform_matrix);
                BrMatrix34TApplyV(&pObject->omega, &vectors.hinge_motion.scratch, &pObject->actor->t.t.mat);
                break;
            case eJoint_universal:
                RotateObject(pObject, (undefined4)(uintptr_t)ppChanged_object, pDelta_time);
                break;
            case eJoint_ball_n_socket:
                RotateObject(pObject, *(undefined4*)&pDelta_time, pDelta_time);
                break;
            default:
                PhysicsError(5, NULL);
            }
            BrMatrix34ApplyP(&vectors.joint_positions.child_joint_position, &pObject->physics_joint1->field_0x08, &pObject->actor->t.t.mat);
            BrMatrix34ApplyP(&vectors.joint_positions.parent_joint_position, &pObject->physics_joint1->field_0x14, &parent->actor->t.t.mat);
            pObject->actor->t.t.mat.m[3][0] += vectors.joint_positions.parent_joint_position.v[0] - vectors.joint_positions.child_joint_position.v[0];
            pObject->actor->t.t.mat.m[3][1] += vectors.joint_positions.parent_joint_position.v[1] - vectors.joint_positions.child_joint_position.v[1];
            pObject->actor->t.t.mat.m[3][2] += vectors.joint_positions.parent_joint_position.v[2] - vectors.joint_positions.child_joint_position.v[2];
        }
    }
    for (child = pObject->child; child != NULL; child = child->next) {
        FUN_004b9230(child, ppChanged_object, pDelta_time);
    }
    if (pObject->physics_joint2 != NULL) {
        *ppChanged_object = pObject;
    }
}

// FUNCTION: CARMA2_HW 0x004b99e0
void C2_HOOK_FASTCALL InternalPrepareObject(tPhysics_object* pObject) {
    tPhysics_object* child;
    tPhysics_object* parent;
    int i;
    int j;

    if (pObject->M == 0.f) {
        pObject->M = 0.01f;
        BrVector3SetFloat(&pObject->I, 0.01f, 0.01f, 0.01f);
    }
    {
        int f = pObject->flags;
        f &= ~0x1200;
        pObject->flags = f;
    }
    pObject->field_0x1e0[0] = pObject->disable_move_rotate;
    pObject->field_0x1de = 0;
    pObject->collision_flag = 0;
    pObject->field_0x1dc = 0;
    pObject->field_0x1e4 = 0;
    pObject->field_0x21c = 1.f;
    pObject->field183_0x1d8 = pObject;
    pObject->field_0x218 = 0;

    BrMatrix34TApplyV(&pObject->velocity_car_space, &pObject->v, &pObject->transform_matrix);
    parent = pObject->parent;
    pObject->field_0x1b4.v[0] = pObject->v.v[0];
    pObject->field_0x1b4.v[1] = pObject->v.v[1];
    pObject->field_0x1b4.v[2] = pObject->v.v[2];
    pObject->field_0x1cc.v[0] = pObject->omega.v[0];
    pObject->field_0x1cc.v[1] = pObject->omega.v[1];
    pObject->field_0x1cc.v[2] = pObject->omega.v[2];

    if (parent != NULL) {
        pObject->disable_move_rotate = parent->disable_move_rotate;
    }
    if (pObject->disable_move_rotate == 0) {
        if (pObject->field_0x1df != 0) {
            pObject->I.v[0] *= 50.f;
            pObject->I.v[1] *= 50.f;
            pObject->I.v[2] *= 50.f;
            pObject->field_0x21c *= 50.f;
        }
    }

    if (pObject->disable_move_rotate & (tU8)(pObject->field_0xf0 != 2)) {
        BrMatrix34Copy(&pObject->actor->t.t.mat, &pObject->transform_matrix);
        for (child = pObject->child; child != NULL; child = child->next) {
            InternalPrepareObject(child);
        }
        if (pObject->parent == NULL) {
            MergeBoundsIntoParent(pObject, NULL);
        }
        return;
    }

    if (pObject->field_0xf0 == 2) {
        float dx;
        float dy;
        float dz;
        float d2;

        pObject->field_0x230 = pObject->parent;
        dx = pObject->actor->t.t.mat.m[3][0] - pObject->transform_matrix.m[3][0];
        dy = pObject->actor->t.t.mat.m[3][1] - pObject->transform_matrix.m[3][1];
        dz = pObject->actor->t.t.mat.m[3][2] - pObject->transform_matrix.m[3][2];
        d2 = dx * dx + dy * dy + dz * dz;
        if (d2 > 1.0) {
            PhysicsWarning("Stupid trial position");
            BrMatrix34Copy(&pObject->transform_matrix, &pObject->actor->t.t.mat);
        }
        for (j = 3; j >= 0; j--) {
            for (i = 0; i < 3; i++) {
                if (pObject->transform_matrix.m[j][i] != pObject->actor->t.t.mat.m[j][i]) {
                    goto differ_found;
                }
            }
        }
        goto differ_done;
differ_found:
        pObject->disable_move_rotate = 0;
differ_done:
        for (child = pObject->child; child != NULL; child = child->next) {
            InternalPrepareObject(child);
        }
        MergeBoundsIntoParent(pObject, NULL);
        return;
    }

    if (pObject->field_0xf0 == 1) {
        BrMatrix34Copy((br_matrix34*)pObject->field_0xbc, &pObject->actor->t.t.mat);
    } else {
        BrMatrix34LPNormalise(&pObject->actor->t.t.mat, &pObject->transform_matrix);
    }
    for (child = pObject->child; child != NULL; child = child->next) {
        InternalPrepareObject(child);
    }
    if (pObject->parent != NULL) {
        return;
    }
    MoveJointedObject(pObject, 0, 0.04f);
    MergeBoundsIntoParent(pObject, NULL);
}

/* Transform bb1 by both object transforms, then merge the object and its
 * children into the parent bounds. */

/* 0x4b9ce0: transform bb1 by the object's transform matrix into field_0xf4,
 * then merge (grow) the actor-matrix box into it (padding by 0.0025); copy the
 * result to field_0x10c, recurse into children with the 0x10c box, and either
 * merge into the parent box or track the global x extents. */
// FUNCTION: CARMA2_HW 0x004b9ce0
void C2_HOOK_FASTCALL MergeBoundsIntoParent(tPhysics_object* pObject, br_bounds3* pBox) {
    tPhysics_object* child;
    br_bounds3 local_box;
    int i;

    GetNewBoundingBox(&pObject->field_0xf4, &pObject->bb1, &pObject->transform_matrix);

    GetNewBoundingBox(&local_box, &pObject->bb1, &pObject->actor->t.t.mat);
    for (i = 0; i < 3; i++) {
        if (pObject->field_0xf4.min.v[i] > local_box.min.v[i]) {
            pObject->field_0xf4.min.v[i] = local_box.min.v[i];
        }
        if (pObject->field_0xf4.max.v[i] < local_box.max.v[i]) {
            pObject->field_0xf4.max.v[i] = local_box.max.v[i];
        }
        pObject->field_0xf4.min.v[i] -= 0.0025;
        pObject->field_0xf4.max.v[i] += 0.0025;
    }

    pObject->field_0x10c = pObject->field_0xf4;

    for (child = pObject->child; child != NULL; child = child->next) {
        MergeBoundsIntoParent(child, &pObject->field_0x10c);
    }

    if (pBox != NULL) {
        for (i = 0; i < 3; i++) {
            if (pObject->field_0x10c.min.v[i] < pBox->min.v[i]) {
                pBox->min.v[i] = pObject->field_0x10c.min.v[i];
            }
            if (pObject->field_0x10c.max.v[i] > pBox->max.v[i]) {
                pBox->max.v[i] = pObject->field_0x10c.max.v[i];
            }
        }
    } else {
        if (gFLOAT_00692dd0 <= pObject->field_0x10c.max.v[0] - pObject->transform_matrix.m[3][0]) {
            gFLOAT_00692dd0 = pObject->field_0x10c.max.v[0] - pObject->transform_matrix.m[3][0];
        }
        if (gFLOAT_00692dd4 >= pObject->field_0x10c.min.v[0] - pObject->transform_matrix.m[3][0]) {
            gFLOAT_00692dd4 = pObject->field_0x10c.min.v[0] - pObject->transform_matrix.m[3][0];
        }
    }
}

// FUNCTION: CARMA2_HW 0x004b9f40
void C2_HOOK_FASTCALL MakeObjectListDoSomething(tPhysics_object* pObject) {

    while (pObject != NULL) {
        if (pObject->disable_move_rotate) {
            pObject->disable_move_rotate = 0;
            pObject->box_face_ref = gFace_num__car - 2;
        }
        if (pObject->child != NULL) {
            MakeObjectListDoSomething(pObject->child);
        }
        pObject = pObject->next;
    }
}

// FUNCTION: CARMA2_HW 0x004b9f90
int C2_HOOK_FASTCALL CheckForObjectHierachyTouchingAnotherObject(tPhysics_object* pObject, tPhysics_object* pList, tPhysics_object* pList_original) {
    tPhysics_object* obj;
    tPhysics_object* item;
    tPhysics_object* child;
    int face;

    if (pObject->field_0xf4.min.v[0] - pList->field_0xf4.max.v[0] > 0.01
        || pObject->field_0xf4.min.v[1] - pList->field_0xf4.max.v[1] > 0.01
        || pObject->field_0xf4.min.v[2] - pList->field_0xf4.max.v[2] > 0.01
        || pList->field_0xf4.min.v[0] - pObject->field_0xf4.max.v[0] > 0.01
        || pList->field_0xf4.min.v[1] - pObject->field_0xf4.max.v[1] > 0.01
        || pList->field_0xf4.min.v[2] - pObject->field_0xf4.max.v[2] > 0.01) {
        return 0;
    }
    obj = pList;
    if (obj->disable_move_rotate) {
        while (obj->parent != NULL) {
            if (obj->physics_joint1 == NULL) {
                break;
            }
            if (obj->physics_joint1->type == eJoint_none) {
                break;
            }
            obj = obj->parent;
        }
        face = gFace_num__car - 2;
        obj->disable_move_rotate = 0;
        obj->box_face_ref = face;
        if (obj->child != NULL || obj->physics_joint1 != NULL || obj->physics_joint2 != NULL) {
            if (obj->field_0x218 == 0) {
                MoveJointedObject(obj, (undefined4)pList, 0.f);
            }
            child = obj->child;
            if (child != NULL) {
                do {
                    if (child->disable_move_rotate) {
                        child->disable_move_rotate = 0;
                        child->box_face_ref = gFace_num__car - 2;
                    }
                    if (child->child != NULL) {
                        MakeObjectListDoSomething(child->child);
                    }
                    child = child->next;
                } while (child != NULL);
            }
        }
        if (pList_original == NULL) {
            return 1;
        }
        for (item = pList_original; item != NULL; item = item->next) {
            if (item->disable_move_rotate == 0) {
                continue;
            }
            CheckForObjectHierachyTouchingAnotherObject(obj, item, pList_original);
            for (child = obj->child; child != NULL; child = child->next) {
                if (item->disable_move_rotate == 0) {
                    break;
                }
                if (CheckForObjectHierachyTouchingAnotherObject(child, item, pList_original) != 0) {
                    break;
                }
            }
            if (item->child != NULL) {
                CheckForObjectHierachyTouchingObjectList(obj, item->child, pList_original);
            }
        }
    }
    return 1;
}

// FUNCTION: CARMA2_HW 0x004b6090
int C2_HOOK_FASTCALL PHILMakeObjectActive(tPhysics_object* pObject, const br_vector3* pVel, const br_vector3* pOmega, int pArg4) {
    tPHIL_queued_object_info* info;

    if (gPHIL_enabled) {
        return 0;
    }
    if (pObject->field_0x239 == 2) {
        info = pObject->field_0x240;
        info->field_0x8 = 2;
        if (pVel != NULL) {
            info->field_0x18 = *pVel;
            info->flags |= 0x1;
        }
        if (pOmega != NULL) {
            info->field_0x24 = *pOmega;
            info->flags |= 0x2;
        }
        if (pArg4) {
            info->field_0x4 |= 0x2;
        } else {
            info->field_0x4 &= ~0x2;
        }
        return 0;
    }
    info = pObject->field_0x240;
    if (info != NULL) {
        info->field_0x8 = 2;
        pObject->flags &= ~0x20;
        if (pArg4) {
            info->field_0x4 |= 0x2;
        } else {
            info->field_0x4 &= ~0x2;
        }
        if (pVel != NULL) {
            pObject->v = *pVel;
        }
        if (pOmega != NULL) {
            pObject->omega = *pOmega;
        }
        ResetCarSpecialVolume(pObject);
        return 0;
    } else {
        return 3;
    }
}


// FUNCTION: CARMA2_HW 0x004ba190
void C2_HOOK_FASTCALL CheckForObjectHierachyTouchingObjectList(tPhysics_object* pObject, tPhysics_object* pList, tPhysics_object* pList_original) {
    tPhysics_object* list_cur;
    tPhysics_object* obj;
    tPhysics_object* child;
    tPhysics_object* sib;
    int face;

    if (pList == NULL) {
        return;
    }
    for (list_cur = pList; list_cur != NULL; list_cur = list_cur->next) {
        if (list_cur->disable_move_rotate == 0) {
            continue;
        }
        if (!(pObject->field_0xf4.min.v[0] - list_cur->field_0xf4.max.v[0] > 0.01
            || pObject->field_0xf4.min.v[1] - list_cur->field_0xf4.max.v[1] > 0.01
            || pObject->field_0xf4.min.v[2] - list_cur->field_0xf4.max.v[2] > 0.01
            || list_cur->field_0xf4.min.v[0] - pObject->field_0xf4.max.v[0] > 0.01
            || list_cur->field_0xf4.min.v[1] - pObject->field_0xf4.max.v[1] > 0.01
            || list_cur->field_0xf4.min.v[2] - pObject->field_0xf4.max.v[2] > 0.01)) {
            obj = list_cur;
            while (obj->parent != NULL) {
                if (obj->physics_joint1 == NULL) {
                    break;
                }
                if (obj->physics_joint1->type == eJoint_none) {
                    break;
                }
                obj = obj->parent;
            }
            face = gFace_num__car - 2;
            obj->disable_move_rotate = 0;
            obj->box_face_ref = face;
            if (obj->child != NULL || obj->physics_joint1 != NULL || obj->physics_joint2 != NULL) {
                if (obj->field_0x218 == 0) {
                    MoveJointedObject(obj, (undefined4)list_cur, 0.f);
                }
                child = obj->child;
                if (child != NULL) {
                    do {
                        if (child->disable_move_rotate) {
                            child->disable_move_rotate = 0;
                            child->box_face_ref = gFace_num__car - 2;
                        }
                        if (child->child != NULL) {
                            MakeObjectListDoSomething(child->child);
                        }
                        child = child->next;
                    } while (child != NULL);
                }
            }
            CheckForObjectHierachyTouchingObjectList(obj, pList_original, pList_original);
        }
        sib = pObject->child;
        if (sib != NULL && list_cur->disable_move_rotate != 0) {
            do {
                if (sib->field_0xf4.min.v[0] - list_cur->field_0xf4.max.v[0] > 0.01
                    || sib->field_0xf4.min.v[1] - list_cur->field_0xf4.max.v[1] > 0.01
                    || sib->field_0xf4.min.v[2] - list_cur->field_0xf4.max.v[2] > 0.01
                    || list_cur->field_0xf4.min.v[0] - sib->field_0xf4.max.v[0] > 0.01
                    || list_cur->field_0xf4.min.v[1] - sib->field_0xf4.max.v[1] > 0.01
                    || list_cur->field_0xf4.min.v[2] - sib->field_0xf4.max.v[2] > 0.01) {
                    sib = sib->next;
                    continue;
                }
                obj = list_cur;
                while (obj->parent != NULL) {
                    if (obj->physics_joint1 == NULL) {
                        break;
                    }
                    if (obj->physics_joint1->type == eJoint_none) {
                        break;
                    }
                    obj = obj->parent;
                }
                face = gFace_num__car - 2;
                child = obj->child;
                obj->disable_move_rotate = 0;
                obj->box_face_ref = face;
                if (child != NULL || obj->physics_joint1 != NULL || obj->physics_joint2 != NULL) {
                    if (obj->field_0x218 == 0) {
                        MoveJointedObject(obj, (undefined4)list_cur, 0.f);
                    }
                    child = obj->child;
                    if (child != NULL) {
                        do {
                            if (child->disable_move_rotate) {
                                child->disable_move_rotate = 0;
                                child->box_face_ref = gFace_num__car - 2;
                            }
                            if (child->child != NULL) {
                                MakeObjectListDoSomething(child->child);
                            }
                            child = child->next;
                        } while (child != NULL);
                    }
                }
                CheckForObjectHierachyTouchingObjectList(obj, pList_original, pList_original);
                break;
            } while (sib != NULL);
        }
        if (list_cur->child != NULL) {
            CheckForObjectHierachyTouchingObjectList(pObject, list_cur->child, pList_original);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004b6210
int C2_HOOK_FASTCALL PHILAddActiveObject(tPhysics_object* pInfo, undefined4* pArg2, const br_vector3* pArg3, const br_vector3* pArg4) {
    int result;

    if (gPHIL_enabled) {
        return 0;
    }
    result = PHILAddObject(pInfo);
    if (result != 0) {
        return result;
    }
    if (pArg2 != NULL && !gPHIL_enabled) {
        if (pInfo->field_0x239 == 2) {
            tQueued_object_info* object_info = pInfo->field_0x240;
            object_info->field_0x30 = *(br_matrix34*)pArg2;
            object_info->flags |= 0x4;
        } else {
            tQueued_object_info* object_info = pInfo->field_0x240;
            if (object_info != NULL) {
                if (object_info->field_0x8 != 1) {
                    return 4;
                }
                object_info->object->actor->t.t.mat = *(br_matrix34*)pArg2;
            } else {
                return 3;
            }
        }
    }
    return PHILMakeObjectActive(pInfo, pArg3, pArg4, 0);
}

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x004b62b0
int C2_HOOK_FASTCALL PHILAddObjectWithFlag(tPhysics_object* pObject) {
    int result;

    if (gPHIL_enabled) {
        return 0;
    }
    if (gPHIL_munging_objects) {
        gPHIL_object_added = 1;
    }
    result = PHILAddObject(pObject);
    gPHIL_object_added = 0;
    return result;
}
#pragma auto_inline(on)

void C2_HOOK_FASTCALL SwapPair(tPhysics_object* pObj1, tPhysics_object* pObj2) {
    tPhysics_object* obj1_prev;
    tPhysics_object* obj2_next;

    obj1_prev = pObj1->prev;
    obj2_next = pObj2->next;
    if (obj1_prev != NULL) {
        obj1_prev->next = pObj2;
    }
    if (obj2_next != NULL) {
        obj2_next->prev = pObj1;
    }
    pObj1->prev = pObj2;
    pObj2->prev = obj1_prev;
    pObj1->next = obj2_next;
    pObj2->next = pObj1;
}

void C2_HOOK_FASTCALL CheckObjectsPostionInList(tPhysics_object* pObject, tPhysics_object** pList) {

    if (pObject->prev != NULL && pObject->transform_matrix.m[3][0] < pObject->prev->transform_matrix.m[3][0]) {
        do {
            SwapPair(pObject->prev, pObject);
        } while (pObject->prev != NULL && pObject->transform_matrix.m[3][0] < pObject->prev->transform_matrix.m[3][0]);
        if (pObject->prev == NULL) {
            *pList = pObject;
        }
    } else if (pObject->next != NULL && pObject->transform_matrix.m[3][0] > pObject->next->transform_matrix.m[3][0]) {
        do {
            SwapPair(pObject, pObject->next);
        } while (pObject->next != NULL && pObject->transform_matrix.m[3][0] > pObject->next->transform_matrix.m[3][0]);
    }
}

void C2_HOOK_FASTCALL MakeObjectDoSomething(tPhysics_object* pObject, tPhysics_object* pList) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, parent, 0x228);

    if (pObject->disable_move_rotate) {
        while (pObject->parent != NULL && pObject->parent->physics_joint1 != NULL && pObject->parent->physics_joint1->type != eJoint_none) {
            pObject = pObject->parent;
        }
        pObject->box_face_ref = gFace_num__car - 2;
        pObject->disable_move_rotate = 0;
        if (pObject->child != NULL || pObject->physics_joint1 != NULL || pObject->physics_joint2 != NULL) {
            if (pObject->field_0x218 == 0) {
                MoveJointedObject(pObject CARPOCALYPSE2_THISCALL_EDX, 0.f);
            }
            MakeObjectListDoSomething(pObject->child);
        }
        CheckForObjectHierachyTouchingObjectList(pObject, pList, pList);
    }
}

void C2_HOOK_FASTCALL AddObjectToReducedList(tPhysics_object* pObject, tPhysics_object** pReduced_list) {

    pObject->field_0x234 = *pReduced_list;
    pReduced_list[0] = pObject;
}

// FUNCTION: CARMA2_HW 0x004b9770
void C2_HOOK_FASTCALL PrepareObject(tPhysics_object* pObject, tPhysics_object** pList) {

    InternalPrepareObject(pObject);
    gPrepared_objects = 1;
    CheckObjectsPostionInList(pObject, pList);
    if (gINT_006940ac) {
        MakeObjectDoSomething(pObject, *pList);
    }
    if (gReduced_object_list != NULL) {
        AddObjectToReducedList(pObject, gReduced_object_list);
    }
}

// C2PORT-NONMATCHING: best 64.93% | difftest PASS 2000 trials (spec difftest/specs/004be0e0_FUN_004be0e0.json) | reason VC5 register allocation and stack layout
// FUNCTION: CARMA2_HW 0x004be0e0
void C2_HOOK_FASTCALL FUN_004be0e0(tPhysics_object* pObject, tPhysics_object* pList) {
    tPhysics_object* item;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, disable_move_rotate, 0xec);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, parent, 0x228);

    if (!pObject->disable_move_rotate) {
        return;
    }
    while (pObject->parent != NULL && pObject->physics_joint1 != NULL && pObject->physics_joint1->type != eJoint_none) {
        pObject = pObject->parent;
    }
    pObject->disable_move_rotate = 0;
    pObject->box_face_ref = gFace_num__car - 2;
    if (pObject->child != NULL || pObject->physics_joint1 != NULL || pObject->physics_joint2 != NULL) {
        if (pObject->field_0x218 == 0) {
            MoveJointedObject(pObject CARPOCALYPSE2_THISCALL_EDX, 0.f);
        }
        {
            tPhysics_object* child;
            for (child = pObject->child; child != NULL; child = child->next) {
                if (child->disable_move_rotate) {
                    child->disable_move_rotate = 0;
                    child->box_face_ref = gFace_num__car - 2;
                }
                if (child->child != NULL) {
                    MakeObjectListDoSomething(child->child);
                }
            }
        }
    }

    for (item = pList; item != NULL; item = item->next) {
        if (!item->disable_move_rotate) {
            continue;
        }
        if (pObject->field_0xf4.min.v[0] - item->field_0xf4.max.v[0] <= gDOUBLE_0058a798
                && pObject->field_0xf4.min.v[1] - item->field_0xf4.max.v[1] <= gDOUBLE_0058a798
                && pObject->field_0xf4.min.v[2] - item->field_0xf4.max.v[2] <= gDOUBLE_0058a798
                && item->field_0xf4.min.v[0] - pObject->field_0xf4.max.v[0] <= gDOUBLE_0058a798
                && item->field_0xf4.min.v[1] - pObject->field_0xf4.max.v[1] <= gDOUBLE_0058a798
                && item->field_0xf4.min.v[2] - pObject->field_0xf4.max.v[2] <= gDOUBLE_0058a798) {
            FUN_004be0e0(item, pList);
        }
        {
            tPhysics_object* child;
            child = pObject->child;
            if (child != NULL && item->disable_move_rotate) {
                for (; child != NULL; child = child->next) {
                    if (child->field_0xf4.min.v[0] - item->field_0xf4.max.v[0] <= gDOUBLE_0058a798
                            && child->field_0xf4.min.v[1] - item->field_0xf4.max.v[1] <= gDOUBLE_0058a798
                            && child->field_0xf4.min.v[2] - item->field_0xf4.max.v[2] <= gDOUBLE_0058a798
                            && item->field_0xf4.min.v[0] - child->field_0xf4.max.v[0] <= gDOUBLE_0058a798
                            && item->field_0xf4.min.v[1] - child->field_0xf4.max.v[1] <= gDOUBLE_0058a798
                            && item->field_0xf4.min.v[2] - child->field_0xf4.max.v[2] <= gDOUBLE_0058a798) {
                        FUN_004be0e0(item, pList);
                        break;
                    }
                }
            }
        }
        if (item->child != NULL) {
            CheckForObjectHierachyTouchingObjectList(pObject, item->child, pList);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004b5d40
int C2_HOOK_FASTCALL PHILAddObject(tPhysics_object* pObject) {
    int i;
    tPHIL_queued_header* object_info;

    if (gPHIL_enabled) {
        return 0;
    }
    if (gPHIL_munging_objects && !gPHIL_object_added) {
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tQueued_object_info, object, 0x0);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tQueued_object_info, field_0x8, 0x8);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tQueued_object_info, flags, 0x60);

        gPhil_queued_objects.objects[gPHIL_count_queued_objects].collision_info = pObject;
        gPhil_queued_objects.objects[gPHIL_count_queued_objects].field_0x8 = 1;
        gPhil_queued_objects.objects[gPHIL_count_queued_objects].flags = 0;
        pObject->field_0x240 = &gPhil_queued_objects.objects[gPHIL_count_queued_objects];
        pObject->field_0x239 = 2;
        gPHIL_count_queued_objects += 1;
        return 0;
    }
    object_info = NULL;
    for (i = 0; i < CARPOCALYPSE2_ASIZE(gPhil_queued_objects.headers); i++) {
        tPHIL_queued_header* current_object_info = &gPhil_queued_objects.headers[i];

        if (current_object_info->collision_info == NULL) {
            if (object_info == NULL) {
                object_info = current_object_info;
            }
        } else if (current_object_info->collision_info == pObject) {
            return 2;
        }
    }
    if (object_info == NULL) {
        return 1;
    }
    memset(object_info, 0, sizeof(*object_info));
    object_info->collision_info = pObject;
    object_info->field_0x8 = 1;
    pObject->field_0x240 = object_info;
    pObject->flags |= 0x20;
    pObject->field_0x239 = 1;
    gPHIL_count_list_collision_infos += 1;
    if (gList_collision_infos != NULL) {
        pObject->next = gList_collision_infos->next;
        gList_collision_infos->next = pObject;
        pObject->prev = gList_collision_infos;
        if (pObject->next != NULL) {
            pObject->next->prev = pObject;
        }
    } else {
        gList_collision_infos = pObject;
        pObject->next = NULL;
        pObject->prev = NULL;
    }
    if (gCrush_deferred) {
        PrepareObject(pObject, &gList_collision_infos);
    }
    return 0;
}

C2_MATCHING_INLINE void C2_HOOK_FASTCALL PHILMungeObjects(tPhysics_object* pObjects) {
    tPhysics_object* object;

    for (object = pObjects; object != NULL; object = object->next) {
        tPHIL_queued_header* object_info;
        tSpecial_volume* original_last_special_volume;
        float original_water_depth_factor;

#ifdef CARPOCALYPSE2_FIX_BUGS
        original_last_special_volume = NULL;
#endif

        object_info = object->field_0x240;
        if (!object->disable_move_rotate && object_info != NULL && object_info->field_0x8 == 2) {
            if (object_info->field_0x4 & 0x8) {
                original_water_depth_factor = object->water_depth_factor;
                original_last_special_volume = object->last_special_volume;
                if (object->water_d != 10000.f) {
                    TestAutoSpecialVolume(object);
                }
                MungeSpecialVolume(object);
            }
            if (object_info->field_0x10 != 0.f || (object->last_special_volume != NULL && (object_info->field_0x4 & 0x8))) {
                float gravity;

                gravity = object_info->field_0x10 != 0.f ? object_info->field_0x10 : 1.0;
                ProcessGravity(object_info, object, gravity);
            }
            if (object_info->field_0xc != 0.f || (object->last_special_volume != NULL && (object_info->field_0x4 & 0x8))) {
                float drag;

                drag = object_info->field_0xc != 0.f ? object_info->field_0xc : 1.0;
                ProcessDrag(object_info, object, drag);
            }
            if (object_info->field_0x4 & 0x8) {
                if (!(object->last_special_volume != NULL && object->last_special_volume->viscosity_multiplier > 2.0 && object->water_depth_factor == 1.0)
                        || (original_last_special_volume != NULL && original_last_special_volume->viscosity_multiplier > 2.0 && original_water_depth_factor >= 1.0)) {

                    if (!(object->last_special_volume != NULL && object->last_special_volume->viscosity_multiplier > 2.0)
                            && (original_last_special_volume != NULL && original_last_special_volume->viscosity_multiplier > 2.0)) {
                        ProcessDrag(object_info, object, gFLOAT_0065d00c);
                    }
                }
                else {
                    ProcessDrag(object_info, object, gFLOAT_0065d008);
                }
                if (object_info->field_0x14 != 0.f
                        && object->last_special_volume != NULL
                        && object->last_special_volume->viscosity_multiplier > 2.0
                        && object->water_depth_factor < 1.0) {
                    LevelOutOnSurface(object);
                }
            }
            if (object_info->field_0x4 & 0x1) {
                PositionChildren(object);
            }
        } else if (object_info != NULL && object_info->field_0x8 == 1) {
            MarkObjectAndChildrenAsPassive(object);
        }
    }
}

C2_MATCHING_INLINE void C2_HOOK_FASTCALL FlushQueuedAddsAndRemoves(void) {
    int i;

    for (i = 0; i < gPHIL_count_queued_objects; i++) {
        tPHIL_queued_object_info* queued_object_info;
        tPHIL_queued_header* object_info;

        queued_object_info = &gPhil_queued_objects.objects[i];
        if (queued_object_info->collision_info == NULL) {
            continue;
        }
        PHILAddObject(queued_object_info->collision_info);
        if (queued_object_info->flags & 0x4) {
            PHILSetPassiveObjectsMatrix(queued_object_info->collision_info, &queued_object_info->field_0x30);
        }
        if (queued_object_info->field_0x8 == 2) {
            br_vector3* omega;
            br_vector3* v;

            v = (queued_object_info->flags & 0x1) ? &queued_object_info->field_0x18 : NULL;
            omega = (queued_object_info->flags & 0x2) ? &queued_object_info->field_0x24 : NULL;
            PHILMakeObjectActive(queued_object_info->collision_info, v, omega, 0);
        }
        object_info = queued_object_info->collision_info->field_0x240;
        if (object_info != NULL) {
            object_info->field_0x4 &= ~0x2;
            object_info->field_0x4 |= queued_object_info->field_0x4 & ~0x2;
            object_info->field_0xc = queued_object_info->field_0xc;
            object_info->field_0x10 = queued_object_info->field_0x10;
            object_info->field_0x14 = queued_object_info->field_0x14;
        }
    }
    for (i = 0; i < gPHIL_count_queued_objects_for_removal; i++) {
        tPhysics_object* object;

        object = gPHIL_queued_objects_for_removal[i];
        if (object != NULL) {
            PHILRemoveObject(object);
        }
    }
}

C2_MATCHING_INLINE void C2_HOOK_FASTCALL PHILInterpolateObjects(tPhysics_object* pObjects, tU32 pTime) {
    tPhysics_object *object;
    float dt;

    dt = (gPHIL_last_physics_tick - pTime) * 0.001;
    if (!(dt <= .04 && dt >= .0f)) {
        dt = .0f;
    }
    gOver_shoot = dt > .0f;
    for (object = pObjects; object != NULL; object = object->next) {
        tPHIL_queued_object_info* object_info;

        object_info = object->field_0x240;
        if (object_info != NULL && object_info->field_0x8 == 2) {
            RepositionObjectGroup(object, -dt);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004b6630
void C2_HOOK_FASTCALL PHILDoPhysics(tPhysics_callbacks* pCallbacks, tU32 pLast_tick_time, tU32 pFrame_period) {
    tU32 now;
    int iteration;

    iteration = 0;
    if (gPHIL_enabled) {
        return;
    }
    now = pLast_tick_time + pFrame_period;
    gPHIL_original_activate_passive = pCallbacks->world_callbacks->activate_passive;
    pCallbacks->world_callbacks->activate_passive = PHILActivatePassive;
    gPHIL_callbacks = pCallbacks;
    if (gPHIL_last_physics_tick < pLast_tick_time) {
        gPHIL_last_physics_tick = 40 * (pLast_tick_time / 40);
    }
    if (now <= gPHIL_last_physics_tick) {
        ResetObjectList(gList_collision_infos);
        PHILInterpolateObjects(gList_collision_infos, now);
    } else {
        gCrush_deferred = 1;
        gPHIL_mechanics_time_sync = now - gPHIL_last_physics_tick;
        while (gPHIL_last_physics_tick < now && iteration < 5) {

            iteration += 1;
            if (pCallbacks->pre_collision != NULL) {
                pCallbacks->pre_collision();
            }
            PHILMungeObjects(gList_collision_infos);
            gPHIL_munging_objects = 1;
            if (gList_collision_infos != NULL) {
                DoCollisions(&gList_collision_infos, pCallbacks->world_callbacks);
            }
            gPHIL_munging_objects = 0;
            if (pCallbacks->post_collision != NULL) {
                pCallbacks->post_collision();
            }
            FlushQueuedAddsAndRemoves();
            gPHIL_last_physics_tick += 40;
            gPHIL_mechanics_time_sync -= 40;
            pLast_tick_time = gPHIL_mechanics_time_sync;
        }
        {
            tPhysics_object* object;
            float dt;

            dt = (gPHIL_last_physics_tick - now) * 0.001;
            if (!(dt <= .04 && dt >= .0f)) {
                dt = .0f;
            }
            gOver_shoot = dt > .0f;
            for (object = gList_collision_infos; object != NULL; object = object->next) {
                tPHIL_queued_object_info* object_info;

                object_info = object->field_0x240;
                if (object_info != NULL && object_info->field_0x8 == 2) {
                    RepositionObjectGroup(object, -dt);
                }
            }
        }
        ChangedObjectsCallbacks(gList_collision_infos, pCallbacks, pFrame_period);
        gPHIL_mechanics_time_sync = 1;
        gCrush_deferred = 0;
    }
    pCallbacks->world_callbacks->activate_passive = gPHIL_original_activate_passive;
}

// FUNCTION: CARMA2_HW 0x004978d0
int C2_HOOK_FASTCALL TimeToSendData(void) {
    // GLOBAL: CARMA2_HW 0x00659c30
    static tU32 last_physics_send_time = -1;

    if (gPHIL_last_physics_tick > last_physics_send_time && gPHIL_last_physics_tick < last_physics_send_time + 80) {
        return 0;
    }
    last_physics_send_time = gPHIL_last_physics_tick;
    return 1;
}

// FUNCTION: CARMA2_HW 0x004b71b0
void C2_HOOK_FASTCALL PHILActivatePassive(tPhysics_object* pObject, tPhysics_object* pActivator) {
    tCollision_info_owner* owner;

    owner = pObject->field_0x240;
    if (owner == NULL) {
        pObject->flags |= 0x1000;
        return;
    }
    if (owner->field_0x08 != 1) {
        pObject->flags |= 0x1000;
        return;
    }

    if (gPHIL_original_activate_passive != NULL) {
        gPHIL_original_activate_passive(pObject, pActivator);
    }

    if (gPHIL_callbacks->passive_activated != NULL) {
        if (gPHIL_callbacks->passive_activated(pObject, pActivator)) {
            PHILMakeObjectActive(pObject, NULL, NULL, 1);
        }
    } else {
        PHILMakeObjectActive(pObject, NULL, NULL, 1);
    }
}

// GLOBAL: CARMA2_HW 0x006793d0
int gUNK_006793d0;

/* Local declarations: physics.c does not include finteray.h so that header can
 * stay untouched. Note CheckSingleFace takes normal before rt (differs from
 * finteray.h). */
void C2_HOOK_FASTCALL CheckSingleFace(tFace_ref* pFace, br_vector3* ray_pos, br_vector3* ray_dir, br_vector3* normal, br_scalar* rt, br_vector3* coll_pos);
void C2_HOOK_FASTCALL EnablePlingMaterials(void);
void C2_HOOK_FASTCALL DisablePlingMaterials(void);

// GLOBAL: CARMA2_HW 0x005896d4 (defined in finteray.c)
extern br_scalar gFloat_005896d4;

#pragma auto_inline(off)

// FUNCTION: CARMA2_HW 0x0041e480
int C2_HOOK_FASTCALL RayCastInBox(br_vector3* pRay_pos, br_vector3* pRay_dir, br_vector3* pHit_pos, br_scalar* pT, tPhysics_object* pObject) {
    br_vector3 normal;
    br_vector3 coll_pos;
    br_scalar t;
    int i;
    int best;

    *pT = 2.f;
    for (i = pObject->box_face_start; i < pObject->box_face_end; i++) {
        if (gUNK_006793d0 != 0 && (gFace_list__car[i].flags & 0x80) != 0) {
            continue;
        }
        CheckSingleFace(&gFace_list__car[i], pRay_pos, pRay_dir, &normal, &t, &coll_pos);
        if (t < *pT) {
            *pT = t;
            best = i;
            *pHit_pos = normal;
        }
    }
    if (*pT < 2.f) {
        int c;

        c = gFace_list__car[best].material->identifier[0] - '/';
        if (c >= 0 && c < 11) {
            return c;
        }
    }
    return 0;
}

#pragma auto_inline(on)

// FUNCTION: CARMA2_HW 0x004ff5d0
void C2_HOOK_FASTCALL TestAutoSpecialVolume(tPhysics_object* pObject) {
    br_scalar d;
    br_scalar d_neg;
    br_scalar d_pos;
    br_scalar t1;
    br_scalar t2;
    tSpecial_volume* def_vol;
    br_vector3 hit;
    br_vector3 world_min;
    br_vector3 q;
    br_vector3 dir;
    int k;

    d = 0.f;
    for (k = 0; k < 3; k++) {
        d += BR_MAC3(pObject->transform_matrix.m[k][0], pObject->water_normal.v[0],
                     pObject->transform_matrix.m[k][1], pObject->water_normal.v[1],
                     pObject->transform_matrix.m[k][2], pObject->water_normal.v[2])
             * pObject->bb2.min.v[k];
    }
    d += BR_MAC3(pObject->transform_matrix.m[3][0], pObject->water_normal.v[0],
                 pObject->transform_matrix.m[3][1], pObject->water_normal.v[1],
                 pObject->transform_matrix.m[3][2], pObject->water_normal.v[2]);
    d_pos = d;
    d_neg = d;
    for (k = 0; k < 3; k++) {
        t1 = BR_MAC3(pObject->transform_matrix.m[k][0], pObject->water_normal.v[0],
                     pObject->transform_matrix.m[k][1], pObject->water_normal.v[1],
                     pObject->transform_matrix.m[k][2], pObject->water_normal.v[2])
             * (pObject->bb2.max.v[k] - pObject->bb2.min.v[k]);
        if (t1 < 0.f) {
            d_neg += t1;
        } else {
            d_pos += t1;
        }
    }
    if (d_neg >= pObject->water_d) {
        pObject->auto_special_volume = NULL;
        pObject->water_depth_factor = 1.f;
        return;
    }
    if (pObject->water_d >= d_pos - 0.001) {
        pObject->water_depth_factor = 1.f;
    } else {
        pObject->water_depth_factor = (pObject->water_d - d_neg) / (d_pos - d_neg);
    }
    if (pObject->auto_special_volume != NULL) {
        return;
    }
    def_vol = gDefault_water_spec_vol_real;
    if (def_vol != NULL) {
        BrMatrix34ApplyP(&world_min, &pObject->bb2.min, &pObject->transform_matrix);
        BrVector3Copy(&q, &world_min);
        for (k = 0; k < 3; k++) {
            hit.v[0] = (pObject->bb2.max.v[k] - pObject->bb2.min.v[k]) * pObject->transform_matrix.m[k][0];
            hit.v[1] = (pObject->bb2.max.v[k] - pObject->bb2.min.v[k]) * pObject->transform_matrix.m[k][1];
            hit.v[2] = (pObject->bb2.max.v[k] - pObject->bb2.min.v[k]) * pObject->transform_matrix.m[k][2];
            if (BrVector3Dot(&pObject->water_normal, &hit) > 0.f) {
                BrVector3Add(&q, &q, &hit);
            } else {
                BrVector3Add(&world_min, &world_min, &hit);
            }
        }
        BrVector3Sub(&dir, &world_min, &q);
        DisablePlingMaterials();
        RayCastInBox(&q, &dir, &hit, &t1, pObject);
        EnablePlingMaterials();
        RayCastInBox(&q, &dir, &hit, &t2, pObject);
        if (t1 < t2) {
            pObject->auto_special_volume = def_vol;
            return;
        }
    }
    pObject->water_depth_factor = 1.f;
    pObject->auto_special_volume = NULL;
}

// FUNCTION: CARMA2_HW 0x004ff410
void C2_HOOK_FASTCALL MungeSpecialVolume(tPhysics_object* pObject) {
    tSpecial_volume* new_vol;
    tCar_spec* sv;

    new_vol = FindSpecialVolume(&pObject->pos, pObject->last_special_volume, 0);

    sv = NULL;
    if (pObject != NULL && pObject->owner != NULL && pObject->flags_0x238 == 1) {
        sv = (tCar_spec*)pObject->owner;
    } else {
        sv = NULL;
    }

    if (pObject->auto_special_volume != NULL) {
        if (new_vol == NULL || new_vol->gravity_multiplier == 1.0f) {
            if (pObject->water_d == 10000.0f && pObject->water_depth_factor != 1.0f) {
                pObject->auto_special_volume = NULL;
            } else {
                new_vol = pObject->auto_special_volume;
            }
        }
    }

    if (new_vol != pObject->last_special_volume && sv != NULL && sv->driver == eDriver_local_human) {
        if (pObject->last_special_volume != NULL && pObject->last_special_volume->exit_noise >= 0
                && (new_vol == NULL || new_vol->exit_noise != pObject->last_special_volume->exit_noise)) {
            DRS3StartSound(gXXX_outlet, pObject->last_special_volume->exit_noise);
        }
        if (new_vol != NULL && new_vol->entry_noise >= 0
                && (pObject->last_special_volume == NULL || pObject->last_special_volume->entry_noise != new_vol->entry_noise)) {
            DRS3StartSound(gXXX_outlet, new_vol->entry_noise);
        }
    }

    pObject->last_special_volume = new_vol;

    if (sv != NULL && new_vol != NULL) {
        br_scalar gravity_multiplier = new_vol->gravity_multiplier;
        if (gravity_multiplier < 1.0f) {
            if (IsCarSmoking(sv) != 0) {
                StopCarSmoking(sv);
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x004b6ec0
void C2_HOOK_FASTCALL ProcessGravity(tPHIL_queued_header* pObject_info, tPhysics_object* pObject, float pGravity) {
    br_vector3 gravity;
    tSpecial_volume* pSpecial_volume;

    pSpecial_volume = NULL;
    if (pObject_info->field_0x4 & 0x8) {
        pSpecial_volume = pObject->last_special_volume;
    }
    if (pSpecial_volume != NULL) {
        if (pObject_info->field_0x14 == 0.0f || pSpecial_volume->viscosity_multiplier <= 2.0) {
            gravity.v[1] = pSpecial_volume->gravity_multiplier * pObject->water_depth_factor * pGravity * gGravity_multiplier * -0.05681159420289855;
            gravity.v[0] = 0.0f;
            gravity.v[2] = 0.0f;
        } else {
            gravity.v[0] = 0.0f;
            gravity.v[1] = ((1.0f - pSpecial_volume->gravity_multiplier) * pObject->water_depth_factor * pObject_info->field_0x14 - gGravity_multiplier * 1.4202898550724639 * pGravity) * 0.04;
            gravity.v[2] = 0.0f;
        }
    } else {
        gravity.v[0] = 0.0f;
        gravity.v[2] = 0.0f;
        gravity.v[1] = pGravity * gGravity_multiplier * -0.05681159420289855;
    }
    PhysicsObjectSetImpulse(pObject, &gravity);
    PhysicsObjectMoveVelocity(pObject);
}

// FUNCTION: CARMA2_HW 0x004c2c30
void C2_HOOK_FASTCALL PhysicsObjectApplyTorque(tPhysics_object* pObject, const br_vector3* pTorque) {
    float tx;
    float ty;
    float tz;

    tx = pTorque->v[1] * 0.0210039914f;
    ty = pTorque->v[2] * 0.0210039914f;
    tz = pTorque->v[0] * 0.0210039914f;
    pObject->omega.v[1] += tx / pObject->I.v[1];
    pObject->omega.v[2] += ty / pObject->I.v[2];
    pObject->omega.v[0] += tz / pObject->I.v[0];
}

// FUNCTION: CARMA2_HW 0x004c2c90
void C2_HOOK_FASTCALL ApplyWaterOmegaBrake(tPhysics_object* pObject, const br_vector3* pV) {
    br_scalar b0;
    br_scalar b1;
    br_scalar b2;
    br_vector3 om;

    b0 = (pV->v[0] * 0.0210039914f) / pObject->I.v[0];
    om = pObject->omega;
    b1 = (pV->v[1] * 0.0210039914f) / pObject->I.v[1];
    b2 = (pV->v[2] * 0.0210039914f) / pObject->I.v[2];

    pObject->omega.v[0] -= b0;
    pObject->omega.v[1] -= b1;
    pObject->omega.v[2] -= b2;

    if (om.v[0] * pObject->omega.v[0] <= 0.0f) {
        pObject->omega.v[0] = 0.0f;
    }
    if (om.v[1] * pObject->omega.v[1] <= 0.0f) {
        pObject->omega.v[1] = 0.0f;
    }
    if (om.v[2] * pObject->omega.v[2] <= 0.0f) {
        pObject->omega.v[2] = 0.0f;
    }
}

// FUNCTION: CARMA2_HW 0x004b6ce0
void C2_HOOK_FASTCALL ProcessDrag2(tPHIL_queued_header* pObject_info, tPhysics_object* pObject, float pDrag, int pObject_info_flags, tSpecial_volume* pSpecial_volume) {
    br_scalar drag;
    br_scalar s;
    tSpecial_volume* sp;
    br_vector3 v;

    drag = pDrag * 2e-05;
    sp = NULL;
    if (pObject_info_flags & 8) {
        sp = pSpecial_volume;
    }
    if (sp != NULL) {
        if (pObject_info->field_0x14 == 0.0f || sp->viscosity_multiplier <= 2.0) {
            drag = sp->viscosity_multiplier * pObject->water_depth_factor * drag;
        } else {
            drag = pObject->water_depth_factor * sp->viscosity_multiplier * drag * 0.5;
        }
    }

    s = BrVector3Length(&pObject->v);
    v.v[0] = pObject->v.v[0] * (s * drag * 6.9 / pObject->M);
    v.v[1] = pObject->v.v[1] * (s * drag * 6.9 / pObject->M);
    v.v[2] = pObject->v.v[2] * (s * drag * 6.9 / pObject->M);
    DRVector3Diminish(&pObject->v, &v);

    s = BrVector3Length(&pObject->omega);
    v.v[0] = pObject->omega.v[0] * (s * drag);
    v.v[1] = pObject->omega.v[1] * (s * drag);
    v.v[2] = pObject->omega.v[2] * (s * drag);
    ApplyWaterOmegaBrake(pObject, &v);

    for (pObject = pObject->child; pObject != NULL; pObject = pObject->next) {
        ProcessDrag2(pObject_info, pObject, pDrag, pObject_info_flags, pSpecial_volume);
    }
}

void C2_HOOK_FASTCALL ProcessDrag(tPHIL_queued_header* pObject_info, tPhysics_object* pObject, float pDrag) {

    ProcessDrag2(pObject_info, pObject, pDrag, pObject_info->field_0x4, pObject->last_special_volume);
}

// FUNCTION: CARMA2_HW 0x004b6fb0
void C2_HOOK_FASTCALL LevelOutOnSurface(tPhysics_object *pObject) {
    br_scalar v0;
    br_scalar v1;
    br_scalar v2;
    br_scalar dot;
    br_scalar p1;
    br_scalar tv;
    br_scalar px;
    br_scalar py;
    br_scalar pz;
    br_vector3 n;
    br_vector3 u;

    if (pObject->I.v[0] > pObject->I.v[1] && pObject->I.v[0] > pObject->I.v[2]) {
        v0 = pObject->actor->t.t.mat.m[0][0];
        v1 = pObject->actor->t.t.mat.m[0][1];
        v2 = pObject->actor->t.t.mat.m[0][2];
    } else if (pObject->I.v[1] > pObject->I.v[2] && pObject->I.v[1] > pObject->I.v[0]) {
        v0 = pObject->actor->t.t.mat.m[1][0];
        v1 = pObject->actor->t.t.mat.m[1][1];
        v2 = pObject->actor->t.t.mat.m[1][2];
    } else {
        v0 = pObject->actor->t.t.mat.m[2][0];
        v1 = pObject->actor->t.t.mat.m[2][1];
        v2 = pObject->actor->t.t.mat.m[2][2];
    }

    n.v[0] = -1.0f * v0;
    n.v[1] = -1.0f * v1;
    n.v[2] = -1.0f * v2;

    dot = n.v[2] * pObject->water_normal.v[2] + n.v[0] * pObject->water_normal.v[0] + n.v[1] * pObject->water_normal.v[1];

    if (dot > -0.99) {
        px = pObject->water_normal.v[1] * n.v[2] - pObject->water_normal.v[2] * n.v[1];
        py = pObject->water_normal.v[2] * n.v[0] - pObject->water_normal.v[0] * n.v[2];
        pz = pObject->water_normal.v[0] * n.v[1] - pObject->water_normal.v[1] * n.v[0];
        p1 = dot + 1.0f;
        tv = sqrtf(px * px + py * py + pz * pz);
        u.v[0] = px * (p1 / tv);
        tv = sqrtf(pz * pz + py * py + u.v[0] * u.v[0]);
        u.v[1] = py * (p1 / tv);
        tv = sqrtf(u.v[1] * u.v[1] + pz * pz + u.v[0] * u.v[0]);
        u.v[2] = pz * (p1 / tv);
        BrMatrix34TApplyV(&n, &u, &pObject->actor->t.t.mat);
        pObject->omega.v[0] += n.v[0];
        pObject->omega.v[1] += n.v[1];
        pObject->omega.v[2] += n.v[2];
    }
}

// FUNCTION: CARMA2_HW 0x004b6e90
void C2_HOOK_FASTCALL MarkObjectAndChildrenAsPassive(tPhysics_object* pObject) {
    tPhysics_object *child;

    pObject->field_0xf0 = 2;
    for (child = pObject->child; child != NULL; child = child->next) {
        MarkObjectAndChildrenAsPassive(child);
    }
}

// GLOBAL: CARMA2_HW 0x0058a8b8
const br_scalar gFLOAT_0058a8b8 = 0.0f;

// GLOBAL: CARMA2_HW 0x0058a8c0
const double gDOUBLE_0058a8c0 = 0.0;

// GLOBAL: CARMA2_HW 0x0058a8e0
const double gDOUBLE_0058a8e0 = 0.0001;

// GLOBAL: CARMA2_HW 0x0058a8e8
const float gFLOAT_0058a8e8 = 100000.0f;

// GLOBAL: CARMA2_HW 0x0058a8ec
const float gFLOAT_0058a8ec = -0.0001f;

// GLOBAL: CARMA2_HW 0x0058a8f0
const double gDOUBLE_0058a8f0 = -0.001;

// GLOBAL: CARMA2_HW 0x0058a8f8
const double gDOUBLE_0058a8f8 = 0.01;

// GLOBAL: CARMA2_HW 0x0058a900
const double gDOUBLE_0058a900 = -0.01;

// C2PORT-NONMATCHING: best 15.34% | difftest PASS 2000 trials (spec difftest/specs/004c3b60_FUN_004c3b60.json) | reason VC5 stack layout and register allocation
// FUNCTION: CARMA2_HW 0x004c3b60
undefined4 C2_HOOK_FASTCALL FUN_004c3b60(float* param_1, float* param_2, float* param_3, int param_4, undefined4 param_5, int param_6, int* param_7, int param_8, undefined4 param_9, undefined4 param_10, undefined4 param_11) {
  tU8 *pbVar1;
  tU32 *puVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  tU8 bVar7;
  tU8 uVar8;
  int bVar9;
  int iVar10;
  int iVar11;
  int iVar12;
  float *pfVar13;
  float *pfVar14;
  float *pfVar15;
  float *pfVar16;
  int iVar17;
  tU32 uVar18;
  tU32 *puVar19;
  tU32 *puVar20;
  int iVar21;
  int iVar22;
  float *pfVar23;
  tU32 *puVar24;
  tU8 *puVar25;
  int local_8e4;
  int local_8e0;
  int local_8d8;
  int local_8d4;
  int local_8c8;
  float local_8c4;
  int local_8c0;
  tU32 local_8b8 [2];
  float local_8b0;
  int local_8ac;
  float local_8a8;
  tU32 local_8a4 [8];
  tU8 local_884 [100];
  float local_820 [99];
  float afStack_694 [101];
  float local_500 [99];
  float afStack_374 [101];
  tU32 local_1e0 [115];

  local_8c0 = -1;
  *param_7 = 0;
  if (param_4 == 1) {
    if (gDOUBLE_0058a8e0 < *param_1) {
      *param_3 = *param_2 / *param_1;
      return 0;
    }
    *param_3 = 0.0;
    return 0;
  }
  iVar10 = 0;
  if (0 < param_4) {
    do {
      local_884[iVar10] = (tU8)iVar10;
      iVar10 = iVar10 + 1;
    } while (iVar10 < param_4);
  }
  iVar10 = 0;
  iVar17 = (param_4 - param_8) + -1;
  local_8e4 = param_8 + 1;
  *param_7 = iVar17;
  if (0 < iVar17) {
    do {
      *(char *)(iVar10 + param_6) = (char)iVar10 + (char)param_8 + '\x01';
      iVar10 = iVar10 + 1;
    } while (iVar10 < *param_7);
  }
  FUN_004c5220((int)gPhysics_buffer1_ptr, (int)gPhysics_buffer2_ptr, (undefined4)(tU32)param_1, (undefined4*)(afStack_694 + 1), (int*)param_2, local_884, local_8e4, (int**)param_10, (int)param_11);
  local_8b8[1] = 0;
  local_8b8[0] = 1;
  local_8d8 = 0;
  local_8d4 = local_8e4;
  do {
    fVar6 = gFLOAT_0058a8ec;
    if ((0x31 < local_8d8) && (local_8c8 == -1)) {
      PhysicsWarning("Solver exceeded iteration limit");
      return 0xffffffff;
    }
    puVar2 = local_1e0 + local_8d8 * 2;
    iVar10 = 1;
    *puVar2 = local_8b8[0];
    local_1e0[local_8d8 * 2 + 1] = local_8b8[1];
    local_820[0] = afStack_694[1] / *((float*)gPhysics_buffer1_ptr);
    if (1 < local_8e4) {
      iVar17 = 0;
      pfVar16 = ((float*)gPhysics_buffer1_ptr);
      pfVar13 = ((float*)gPhysics_buffer1_ptr);
      do {
        pfVar13 = pfVar13 + 100;
        pfVar16 = pfVar16 + 0x65;
        fVar5 = gFLOAT_0058a8b8;
        if (-4 < iVar17) {
          pfVar14 = local_820;
          pfVar15 = pfVar13;
          iVar22 = iVar10;
          do {
            fVar3 = *pfVar14;
            fVar4 = *pfVar15;
            pfVar15 = pfVar15 + 1;
            pfVar14 = pfVar14 + 1;
            iVar22 = iVar22 + -1;
            fVar5 = fVar3 * fVar4 + fVar5;
          } while (iVar22 != 0);
        }
        iVar10 = iVar10 + 1;
        *(float *)((int)local_820 + iVar17 + 4) =
             (*(float *)((int)afStack_694 + iVar17 + 8U) - fVar5) / *pfVar16;
        iVar17 = iVar17 + 4;
      } while (iVar10 < local_8e4);
    }
    iVar10 = local_8e4 + -1;
    iVar17 = 0;
    if (0 < iVar10) {
      pfVar16 = ((float*)gPhysics_buffer1_ptr) + iVar10 * 100;
      do {
        if (fabsf(((float*)gPhysics_buffer1_ptr)[iVar10 * 0x65] * gFLOAT_0058a8e8) < fabsf(*pfVar16)) break;
        iVar17 = iVar17 + 1;
        pfVar16 = pfVar16 + 1;
      } while (iVar17 < iVar10);
    }
    if (iVar17 == iVar10) {
      if (0 < local_8e4) {
        pfVar16 = afStack_374;
        iVar17 = local_8e4;
        do {
          pfVar16 = (float *)((int)pfVar16 + 4);
          *pfVar16 = 0.0;
          pfVar14 = local_820;
          fVar5 = *pfVar16;
          pfVar13 = (float *)(((int)gPhysics_buffer2_ptr - (int)(afStack_374 + 1)) + (int)pfVar16);
          iVar22 = local_8e4;
          do {
            fVar3 = *pfVar13;
            fVar4 = *pfVar14;
            pfVar13 = pfVar13 + 100;
            pfVar14 = pfVar14 + 1;
            iVar22 = iVar22 + -1;
            fVar5 = fVar3 * fVar4 + fVar5;
          } while (iVar22 != 0);
          *pfVar16 = fVar5;
          iVar17 = iVar17 + -1;
        } while (iVar17 != 0);
      }
      local_8c8 = -1;
      if (-1 < local_8c0) {
        local_8c0 = -1;
        bVar7 = *(tU8 *)(*param_7 + -1 + param_6);
        if (param_2[bVar7] == gFLOAT_0058a8b8) {
          param_2[bVar7] = -10000.0;
        }
      }
      if (param_8 < local_8e4) {
        pfVar16 = afStack_374 + param_8 + 1;
        iVar17 = param_8;
        do {
          if (*pfVar16 < fVar6) {
            fVar6 = *pfVar16;
            local_8c8 = iVar17;
          }
          iVar17 = iVar17 + 1;
          pfVar16 = pfVar16 + 1;
        } while (iVar17 < local_8e4);
      }
      if (-1 < local_8c8) {
        if (gFLOAT_0058a8b8 <= afStack_374[local_8e4]) {
          iVar17 = iVar10;
          if (local_8d4 <= iVar10) {
            pfVar16 = afStack_374 + local_8e4;
            do {
              if (gFLOAT_0058a8b8 < *pfVar16) {
                iVar17 = iVar17 + -1;
                break;
              }
              iVar17 = iVar17 + -1;
              pfVar16 = pfVar16 + -1;
            } while (local_8d4 <= iVar17);
          }
          iVar17 = iVar17 + 1;
        }
        else {
          iVar17 = 0;
        }
        if (local_8c8 < iVar17) {
          *(tU8 *)(*param_7 + param_6) = local_884[local_8c8];
          iVar17 = (tU32)local_884[local_8c8] - param_8;
          local_884[local_8c8] = 0xff;
          iVar10 = iVar17 >> 5;
          local_8b8[iVar10] = local_8b8[iVar10] & ~(1 << ((tU8)iVar17 & 0x1f));
          *param_7 = *param_7 + 1;
        }
        else {
          local_8b0 = 0.0;
          for (iVar17 = param_8; iVar22 = param_8, local_8c8 = param_8, iVar17 < local_8e4;
              iVar17 = iVar17 + 1) {
            if (iVar17 <= iVar10) {
              pfVar16 = ((float*)gPhysics_buffer2_ptr) + iVar17 + iVar10 * 100;
              iVar22 = iVar10;
              pfVar13 = local_500 + iVar10;
              pfVar14 = ((float*)gPhysics_buffer1_ptr) + iVar10 * 0x65;
              do {
                fVar6 = gFLOAT_0058a8b8;
                if (iVar22 < iVar10) {
                  pfVar23 = local_500 + iVar10;
                  pfVar15 = ((float*)gPhysics_buffer1_ptr) + iVar22 + iVar10 * 100;
                  iVar11 = iVar10 - iVar22;
                  do {
                    fVar5 = *pfVar23;
                    fVar3 = *pfVar15;
                    pfVar15 = pfVar15 + -100;
                    pfVar23 = pfVar23 + -1;
                    iVar11 = iVar11 + -1;
                    fVar6 = fVar5 * fVar3 + fVar6;
                  } while (iVar11 != 0);
                }
                fVar5 = *pfVar16;
                iVar22 = iVar22 + -1;
                pfVar16 = pfVar16 + -100;
                *pfVar13 = (fVar5 - fVar6) / *pfVar14;
                pfVar13 = pfVar13 + -1;
                pfVar14 = pfVar14 + -0x65;
              } while (iVar17 <= iVar22);
            }
            local_820[iVar17] = local_500[iVar17];
          }
          for (; iVar22 < local_8e4; iVar22 = iVar22 + 1) {
            if ((afStack_374[iVar22 + 1] / local_820[iVar22] < gFLOAT_0058a8b8) &&
               (local_8b0 < fabsf(local_820[iVar22]))) {
              local_8b0 = fabsf(local_820[iVar22]);
              local_8c8 = iVar22;
            }
          }
          *(tU8 *)(*param_7 + param_6) = local_884[local_8c8];
          iVar17 = (tU32)local_884[local_8c8] - param_8;
          local_884[local_8c8] = 0xff;
          iVar10 = iVar17 >> 5;
          local_8b8[iVar10] = local_8b8[iVar10] & ~(1 << ((tU8)iVar17 & 0x1f));
          *param_7 = *param_7 + 1;
        }
        iVar10 = 0;
        local_8d4 = 0;
        if (0 < local_8e4) {
          pfVar16 = afStack_694 + 1;
          pfVar13 = afStack_694;
          do {
            pfVar13 = (float *)((int)pfVar13 + 4);
            if (local_884[iVar10] != 0xff) {
              local_884[local_8d4] = local_884[iVar10];
              *pfVar16 = *pfVar13;
              local_8d4 = local_8d4 + 1;
              pfVar16 = pfVar16 + 1;
            }
            iVar10 = iVar10 + 1;
          } while (iVar10 < local_8e4);
        }
        goto LAB_004c4320;
      }
      iVar10 = param_4;
      pfVar16 = param_3;
      if (0 < param_4) {
        for (; iVar10 != 0; iVar10 = iVar10 + -1) {
          *pfVar16 = 0.0;
          pfVar16 = pfVar16 + 1;
        }
      }
      iVar10 = 0;
      if (0 < local_8e4) {
        pfVar16 = afStack_374;
        do {
          pfVar16 = (float *)((int)pfVar16 + 4);
          pbVar1 = local_884 + iVar10;
          iVar10 = iVar10 + 1;
          param_3[*pbVar1] = *pfVar16;
        } while (iVar10 < local_8e4);
      }
      iVar10 = 0;
      local_8a4[6] = *puVar2;
      local_8a4[7] = local_1e0[local_8d8 * 2 + 1];
      if (0 < local_8d8) {
        puVar24 = local_1e0;
        do {
          iVar17 = 0;
          puVar20 = local_8a4 + 6;
          puVar19 = puVar24;
          do {
            if (*puVar20 != *puVar19) break;
            iVar17 = iVar17 + 1;
            puVar19 = puVar19 + 1;
            puVar20 = puVar20 + 1;
          } while (iVar17 < 2);
          if (1 < iVar17) {
            local_8ac = 1;
            goto LAB_004c43d6;
          }
          iVar10 = iVar10 + 1;
          puVar24 = puVar24 + 2;
        } while (iVar10 < local_8d8);
      }
      local_8ac = 0;
LAB_004c43d6:
      iVar10 = -1;
      iVar22 = 0;
      local_8c4 = 0.0;
      iVar17 = *param_7;
      local_8e0 = -1;
      if (0 < iVar17) {
        do {
          fVar6 = gFLOAT_0058a8b8;
          if (0 < param_4) {
            pfVar13 = param_1 + (tU32)*(tU8 *)(iVar22 + param_6) * 100;
            pfVar16 = param_3;
            iVar17 = param_4;
            do {
              fVar5 = *pfVar13;
              fVar3 = *pfVar16;
              pfVar13 = pfVar13 + 1;
              pfVar16 = pfVar16 + 1;
              iVar17 = iVar17 + -1;
              fVar6 = fVar5 * fVar3 + fVar6;
            } while (iVar17 != 0);
          }
          uVar18 = (tU32)*(tU8 *)(iVar22 + param_6);
          fVar5 = (fVar6 - param_2[uVar18]) / param_1[uVar18 * 0x65];
          iVar17 = local_8e0;
          fVar3 = local_8c4;
          if (((fVar6 - param_2[uVar18] < gDOUBLE_0058a8f0) && (fVar5 < local_8c4)) &&
             (iVar10 = iVar22, iVar17 = iVar22, fVar3 = fVar5, local_8ac != 0)) {
            iVar11 = uVar18 - param_8;
            local_8a4[4] = *puVar2;
            local_8a4[5] = local_1e0[local_8d8 * 2 + 1];
            if (-1 < iVar11) {
              local_8a4[(iVar11 >> 5) + 4] =
                   local_8a4[(iVar11 >> 5) + 4] ^ 1 << ((tU8)iVar11 & 0x1f);
            }
            iVar11 = 0;
            if (0 < local_8d8) {
              puVar24 = local_1e0;
              do {
                iVar21 = 0;
                puVar20 = local_8a4 + 4;
                puVar19 = puVar24;
                do {
                  if (*puVar20 != *puVar19) break;
                  iVar21 = iVar21 + 1;
                  puVar19 = puVar19 + 1;
                  puVar20 = puVar20 + 1;
                } while (iVar21 < 2);
                if (1 < iVar21) {
                  iVar10 = local_8e0;
                  iVar17 = local_8e0;
                  fVar3 = local_8c4;
                  if (local_8e0 == -1) {
                    iVar10 = iVar22;
                    iVar17 = iVar22;
                  }
                  break;
                }
                iVar11 = iVar11 + 1;
                puVar24 = puVar24 + 2;
              } while (iVar11 < local_8d8);
            }
          }
          local_8c4 = fVar3;
          local_8e0 = iVar17;
          iVar22 = iVar22 + 1;
          iVar17 = *param_7;
        } while (iVar22 < iVar17);
      }
      if (iVar10 < 0) {
        bVar9 = 0;
        if (0 < param_4) {
          pfVar16 = local_500;
          iVar10 = param_4;
          pfVar13 = param_1;
          do {
            *pfVar16 = 0.0;
            fVar6 = *pfVar16;
            pfVar14 = pfVar13;
            pfVar15 = param_3;
            iVar17 = param_4;
            do {
              fVar5 = *pfVar14;
              fVar3 = *pfVar15;
              pfVar14 = pfVar14 + 1;
              pfVar15 = pfVar15 + 1;
              iVar17 = iVar17 + -1;
              fVar6 = fVar5 * fVar3 + fVar6;
            } while (iVar17 != 0);
            *pfVar16 = fVar6;
            if (((*(float *)(((int)param_3 - (int)local_500) + (int)pfVar16) != gFLOAT_0058a8b8) &&
                (gDOUBLE_0058a8f8 <
                 fabsf(*pfVar16 - *(float *)(((int)param_2 - (int)local_500) + (int)pfVar16)))) ||
               (*pfVar16 - *(float *)(((int)param_2 - (int)local_500) + (int)pfVar16) <
                gDOUBLE_0058a900)) {
              bVar9 = 1;
            }
            pfVar13 = pfVar13 + 100;
            pfVar16 = pfVar16 + 1;
            iVar10 = iVar10 + -1;
          } while (iVar10 != 0);
        }
        if (bVar9) {
          iVar10 = 0;
          if (0 < local_8e4) {
            do {
              iVar17 = 0;
              pfVar16 = afStack_374;
              fVar6 = -afStack_694[iVar10 + 1];
              do {
                pfVar16 = (float *)((int)pfVar16 + 4);
                pbVar1 = local_884 + iVar17;
                iVar17 = iVar17 + 1;
                fVar6 = param_1[(tU32)*pbVar1 + (tU32)local_884[iVar10] * 100] * *pfVar16 + fVar6;
              } while (iVar17 < local_8e4);
              local_500[iVar10] = fVar6;
              iVar10 = iVar10 + 1;
            } while (iVar10 < local_8e4);
          }
          iVar10 = 1;
          local_820[0] = local_500[0] / *((float*)gPhysics_buffer1_ptr);
          if (1 < local_8e4) {
            iVar17 = 0;
            pfVar16 = ((float*)gPhysics_buffer1_ptr);
            pfVar13 = ((float*)gPhysics_buffer1_ptr);
            do {
              pfVar13 = pfVar13 + 0x65;
              pfVar16 = pfVar16 + 100;
              fVar6 = gFLOAT_0058a8b8;
              if (-4 < iVar17) {
                pfVar14 = local_820;
                pfVar15 = pfVar16;
                iVar22 = iVar10;
                do {
                  fVar5 = *pfVar14;
                  fVar3 = *pfVar15;
                  pfVar15 = pfVar15 + 1;
                  pfVar14 = pfVar14 + 1;
                  iVar22 = iVar22 + -1;
                  fVar6 = fVar5 * fVar3 + fVar6;
                } while (iVar22 != 0);
              }
              iVar10 = iVar10 + 1;
              *(float *)((int)local_820 + iVar17 + 4) =
                   (*(float *)((int)local_500 + iVar17 + 4) - fVar6) / *pfVar13;
              iVar17 = iVar17 + 4;
            } while (iVar10 < local_8e4);
          }
          if (0 < local_8e4) {
            iVar17 = (int)afStack_374 + (4U - (int)gPhysics_buffer2_ptr);
            pfVar16 = ((float*)gPhysics_buffer2_ptr);
            iVar10 = local_8e4;
            do {
              pfVar13 = local_820;
              pfVar14 = pfVar16;
              iVar22 = local_8e4;
              do {
                fVar6 = *pfVar14;
                fVar5 = *pfVar13;
                pfVar14 = pfVar14 + 100;
                pfVar13 = pfVar13 + 1;
                iVar22 = iVar22 + -1;
                *(float *)(iVar17 + (int)pfVar16) =
                     fVar6 * fVar5 + *(float *)(iVar17 + (int)pfVar16);
              } while (iVar22 != 0);
              pfVar16 = pfVar16 + 1;
              iVar10 = iVar10 + -1;
            } while (iVar10 != 0);
          }
        }
        return 0;
      }
      uVar8 = *(tU8 *)(iVar10 + param_6);
      iVar10 = iVar10 + 1;
      if (iVar10 < iVar17) {
        puVar25 = (tU8 *)(iVar10 + param_6);
        do {
          iVar10 = iVar10 + 1;
          puVar25[-1] = *puVar25;
          puVar25 = puVar25 + 1;
        } while (iVar10 < *param_7);
      }
      iVar10 = *param_7;
      *param_7 = iVar10 + -1;
      *(tU8 *)(param_6 + iVar10 + -1) = uVar8;
      bVar7 = *(tU8 *)(*param_7 + param_6);
      local_884[local_8e4] = bVar7;
      iVar10 = (tU32)bVar7 - param_8;
      iVar17 = iVar10 >> 5;
      iVar22 = local_8e4 + 1;
      local_8b8[iVar17] = local_8b8[iVar17] | 1 << ((tU8)iVar10 & 0x1f);
      if (gDAT_006940b8 == 0) {
        FUN_004c4f40(((float*)gPhysics_buffer1_ptr), (undefined4)(tU32)param_1, local_8e4, ((float*)gPhysics_buffer2_ptr), local_884);
      }
      else {
            FUN_004c4850(((float*)gPhysics_buffer1_ptr), (int)(tU32)param_1, local_8e4, iVar22, ((float*)gPhysics_buffer2_ptr), (int)(uintptr_t)local_884);
      }
      afStack_694[iVar22] = param_2[local_884[local_8e4]];
      local_8e4 = iVar22;
    }
    else {
      local_8a8 = 0.0;
      iVar17 = iVar10;
      if (param_8 < iVar10) {
        pfVar16 = ((float*)gPhysics_buffer2_ptr) + param_8 + iVar10 * 100;
        iVar22 = param_8;
        do {
          if (local_8a8 < fabsf(*pfVar16)) {
            local_8a8 = fabsf(*pfVar16);
            iVar17 = iVar22;
          }
          iVar22 = iVar22 + 1;
          pfVar16 = pfVar16 + 1;
        } while (iVar22 < iVar10);
      }
      local_8c0 = param_8;
      if (iVar17 != iVar10) {
        iVar22 = (tU32)local_884[iVar17] - param_8;
        local_8a4[0] = *puVar2;
        local_8a4[1] = local_1e0[local_8d8 * 2 + 1];
        if (-1 < iVar22) {
          local_8a4[iVar22 >> 5] = local_8a4[iVar22 >> 5] ^ 1 << ((tU8)iVar22 & 0x1f);
        }
        iVar22 = 0;
        if (0 < local_8d8) {
          puVar24 = local_1e0;
LAB_004c3e12:
          iVar11 = 0;
          puVar20 = local_8a4;
          puVar19 = puVar24;
          do {
            if (*puVar20 != *puVar19) break;
            iVar11 = iVar11 + 1;
            puVar19 = puVar19 + 1;
            puVar20 = puVar20 + 1;
          } while (iVar11 < 2);
          if (iVar11 < 2) break;
          if (param_8 < iVar10) {
            pfVar16 = ((float*)gPhysics_buffer2_ptr) + param_8 + iVar10 * 100;
            iVar22 = param_8;
            local_8e4 = iVar17;
            fVar6 = gFLOAT_0058a8b8;
            do {
              fVar5 = *pfVar16;
              iVar11 = local_8e4;
              fVar3 = fVar6;
              if (fVar6 < fabsf(fVar5)) {
                iVar17 = (tU32)local_884[iVar22] - param_8;
                local_8a4[2] = *puVar2;
                local_8a4[3] = local_1e0[local_8d8 * 2 + 1];
                if (-1 < iVar17) {
                  local_8a4[(iVar17 >> 5) + 2] =
                       local_8a4[(iVar17 >> 5) + 2] ^ 1 << ((tU8)iVar17 & 0x1f);
                }
                iVar21 = 0;
                puVar24 = local_1e0;
                do {
                  iVar12 = 0;
                  puVar20 = local_8a4 + 2;
                  puVar19 = puVar24;
                  do {
                    if (*puVar20 != *puVar19) break;
                    iVar12 = iVar12 + 1;
                    puVar19 = puVar19 + 1;
                    puVar20 = puVar20 + 1;
                  } while (iVar12 < 2);
                  iVar17 = local_8e4;
                  iVar11 = local_8e4;
                  fVar3 = fVar6;
                  if (1 < iVar12) break;
                  iVar21 = iVar21 + 1;
                  puVar24 = puVar24 + 2;
                  iVar17 = iVar22;
                  iVar11 = iVar22;
                  fVar3 = fabsf(fVar5);
                } while (iVar21 < local_8d8);
              }
              fVar6 = fVar3;
              local_8e4 = iVar11;
              iVar22 = iVar22 + 1;
              pfVar16 = pfVar16 + 1;
            } while (iVar22 < iVar10);
          }
          if (iVar17 != iVar10) goto LAB_004c3f3b;
          goto LAB_004c3f48;
        }
LAB_004c3f3b:
        local_8c0 = iVar17;
      }
LAB_004c3f48:
      *(tU8 *)(*param_7 + param_6) = local_884[local_8c0];
      iVar22 = (int)((tU32)local_884[local_8c0] - param_8) >> 5;
      iVar17 = *param_7;
      local_8b8[iVar22] =
           local_8b8[iVar22] & ~(1 << ((tU8)((tU32)local_884[local_8c0] - param_8) & 0x1f));
      *param_7 = iVar17 + 1;
      for (iVar17 = local_8c0; local_8d4 = iVar10, iVar17 < iVar10; iVar17 = iVar17 + 1) {
        local_884[iVar17] = local_884[iVar17 + 1];
      }
LAB_004c4320:
      FUN_004c5220((int)gPhysics_buffer1_ptr, (int)gPhysics_buffer2_ptr, (undefined4)(tU32)param_1, (undefined4*)(afStack_694 + 1), (int*)param_2, local_884, local_8d4, (int**)param_10, (int)param_11);
      local_8e4 = local_8d4;
    }
    local_8d8 = local_8d8 + 1;
  } while (1);
  iVar22 = iVar22 + 1;
  puVar24 = puVar24 + 2;
  if (local_8d8 <= iVar22) goto LAB_004c3f3b;
  goto LAB_004c3e12;
}

// C2PORT-NONMATCHING: best 25.30% | difftest PASS 2000 trials (spec difftest/specs/004c5830_FUN_004c5830.json) | reason vc5-register-spills
// FUNCTION: CARMA2_HW 0x004c5830
undefined4 C2_HOOK_FASTCALL FUN_004c5830(int param_1, int param_2, undefined4* param_3) {
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float *pfVar7;
  int iVar8;
  register float *pfVar9;
  float *pfVar10;
  undefined4 *puVar11;
  register int iVar12;
  register int iVar13;
  register float *pfVar14;
  undefined4 *puVar15;
  float *pfVar16;
  undefined4 *puVar17;
  float local_38;
  int local_34;
  float *local_30;
  int local_1c;
  int local_18;
  int local_10;
  undefined4 local_c;

  local_c = 0;
  iVar12 = param_2;
  puVar17 = param_3;
  puVar15 = param_3;
  iVar13 = param_2;
  puVar11 = param_3;
  if (0 < param_2) {
    do {
      for (; iVar12 != 0; iVar12 = iVar12 - 1) {
        *puVar17 = 0;
        puVar17 = puVar17 + 1;
      }
      *puVar15 = 0x3f800000;
      iVar13 = iVar13 - 1;
      iVar12 = param_2;
      puVar17 = puVar11 + param_2;
      puVar15 = puVar15 + param_2 + 1;
      puVar11 = puVar11 + param_2;
    } while (iVar13 != 0);
  }
  iVar12 = 0;
  if (0 < param_2) {
    local_34 = 0;
    local_1c = 0;
    iVar13 = param_1;
    do {
      local_38 = 0.0f;
      if (iVar12 < param_2) {
        pfVar14 = (float*)(iVar13 + local_34);
        iVar8 = param_2 - iVar12;
        do {
          fVar5 = (float)fabs(*pfVar14);
          if ((float)fabs(*pfVar14) < local_38) {
            fVar5 = local_38;
          }
          pfVar14 = pfVar14 + 1;
          iVar8 = iVar8 - 1;
          local_38 = fVar5;
        } while (iVar8 != 0);
      }
      if (local_38 == (float)gDOUBLE_0058a8c0) {
        local_c = 1;
      }
      else {
        if (iVar12 < param_2) {
          iVar8 = param_2 - iVar12;
          pfVar14 = (float*)(iVar13 + local_34);
          do {
            iVar8 = iVar8 - 1;
            *pfVar14 = *pfVar14 / local_38;
            pfVar14 = pfVar14 + 1;
          } while (iVar8 != 0);
        }
        fVar5 = gFLOAT_0058a8b8;
        if (iVar12 < param_2) {
          pfVar14 = (float*)(iVar13 + local_34);
          iVar8 = param_2 - iVar12;
          do {
            fVar1 = *pfVar14;
            pfVar14 = pfVar14 + 1;
            iVar8 = iVar8 - 1;
            fVar5 = fVar1 * fVar1 + fVar5;
          } while (iVar8 != 0);
        }
        pfVar14 = (float*)(local_34 + iVar13);
        fVar5 = (float)fabs(sqrt(fVar5));
        if (*pfVar14 <= (float)gDOUBLE_0058a8c0) {
          fVar5 = -fVar5;
        }
        fVar1 = fVar5 * (fVar5 + *pfVar14);
        *pfVar14 = fVar5 + *pfVar14;
        if (-1 < local_34) {
          pfVar9 = (float*)((int)param_3 + (iVar13 - param_1));
          local_18 = iVar12 + 1;
          do {
            fVar6 = gFLOAT_0058a8b8;
            if (iVar12 < param_2) {
              iVar8 = param_2 - iVar12;
              pfVar7 = pfVar9;
              pfVar10 = pfVar14;
              do {
                fVar2 = *pfVar10;
                fVar3 = *pfVar7;
                pfVar10 = pfVar10 + 1;
                pfVar7 = pfVar7 + param_2;
                iVar8 = iVar8 - 1;
                fVar6 = fVar2 * fVar3 + fVar6;
              } while (iVar8 != 0);
            }
            if (iVar12 < param_2) {
              iVar8 = param_2 - iVar12;
              pfVar7 = pfVar9;
              pfVar10 = pfVar14;
              do {
                fVar2 = *pfVar10;
                pfVar10 = pfVar10 + 1;
                *pfVar7 = *pfVar7 - (fVar6 / fVar1) * fVar2;
                pfVar7 = pfVar7 + param_2;
                iVar8 = iVar8 - 1;
              } while (iVar8 != 0);
            }
            pfVar9 = pfVar9 + 1;
            local_18 = local_18 - 1;
          } while (local_18 != 0);
        }
        local_10 = iVar12 + 1;
        if (local_10 < param_2) {
          pfVar9 = (float*)(param_3 + local_1c + local_10);
          local_30 = (float*)(param_1 + (iVar12 + param_2 + local_1c) * 4);
          local_10 = param_2 - local_10;
          do {
            fVar6 = gFLOAT_0058a8b8;
            if (iVar12 < param_2) {
              iVar8 = param_2 - iVar12;
              pfVar7 = local_30;
              pfVar10 = pfVar14;
              do {
                fVar2 = *pfVar10;
                fVar3 = *pfVar7;
                pfVar7 = pfVar7 + 1;
                pfVar10 = pfVar10 + 1;
                iVar8 = iVar8 - 1;
                fVar6 = fVar2 * fVar3 + fVar6;
              } while (iVar8 != 0);
            }
            fVar2 = gFLOAT_0058a8b8;
            if (iVar12 < param_2) {
              pfVar7 = (float*)(param_1 + (param_2 + 1) * iVar12 * 4);
              iVar8 = param_2 - iVar12;
              pfVar10 = pfVar9;
              do {
                fVar3 = *pfVar10;
                fVar4 = *pfVar7;
                pfVar7 = pfVar7 + 1;
                pfVar10 = pfVar10 + param_2;
                iVar8 = iVar8 - 1;
                fVar2 = fVar3 * fVar4 + fVar2;
              } while (iVar8 != 0);
            }
            if (iVar12 < param_2) {
              iVar8 = param_2 - iVar12;
              pfVar7 = local_30;
              pfVar10 = pfVar14;
              pfVar16 = pfVar9;
              do {
                *pfVar7 = *pfVar7 - (fVar6 / fVar1) * *pfVar10;
                *pfVar16 = *pfVar16 - (fVar2 / fVar1) * *pfVar10;
                pfVar16 = pfVar16 + param_2;
                iVar8 = iVar8 - 1;
                pfVar7 = pfVar7 + 1;
                pfVar10 = pfVar10 + 1;
              } while (iVar8 != 0);
            }
            local_30 = local_30 + param_2;
            pfVar9 = pfVar9 + 1;
            local_10 = local_10 - 1;
          } while (local_10 != 0);
        }
        *pfVar14 = -(fVar5 * local_38);
        if (iVar12 + 1 < param_2) {
          puVar17 = (undefined4*)(iVar13 + 4 + local_34);
          for (iVar8 = param_2 - (iVar12 + 1); iVar8 != 0; iVar8 = iVar8 - 1) {
            *puVar17 = 0;
            puVar17 = puVar17 + 1;
          }
        }
      }
      iVar12 = iVar12 + 1;
      local_1c = local_1c + param_2;
      local_34 = local_34 + 4;
      iVar13 = iVar13 + param_2 * 4;
    } while (iVar12 < param_2);
  }
  return local_c;
}

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x004c5d00
void C2_HOOK_FASTCALL ClearPhysicsScratchSpace(void) {

    gPhysics_scratch_ptr = gPhysics_other_buffer;
}

// FUNCTION: CARMA2_HW 0x004c61e0
void C2_HOOK_FASTCALL PhysicsWarning(const char* pMessage) {
    (void)pMessage;
}

// GLOBAL: CARMA2_HW 0x0058a7a0
const double gDOUBLE_0058a7a0 = 0.001;

// GLOBAL: CARMA2_HW 0x0058a808
const double gDOUBLE_0058a808 = -0.001;

// GLOBAL: CARMA2_HW 0x0058a810
const double gDOUBLE_0058a810 = 0.000001;

// GLOBAL: CARMA2_HW 0x0058a818
const double gDOUBLE_0058a818 = 0.002;

// GLOBAL: CARMA2_HW 0x0058a7a8
const double gDOUBLE_0058a7a8 = 0.00009;

// GLOBAL: CARMA2_HW 0x0058a7b0
const br_scalar gFLOAT_0058a7b0 = 0.012f;

// GLOBAL: CARMA2_HW 0x0058a7b4
const br_scalar gFLOAT_0058a7b4 = 0.9f;

// GLOBAL: CARMA2_HW 0x0058a7b8
const br_scalar gFLOAT_0058a7b8 = 0.0004f;

// GLOBAL: CARMA2_HW 0x0058a7c0
const double gDOUBLE_0058a7c0 = 0.999;

// FUNCTION: CARMA2_HW 0x004baa00
int C2_HOOK_FASTCALL AddDoubleTorqueToMatrix(tPhysics_object* pObject) {
    tPhysics_object* obj;
    int acc;
    int child_acc;
    float X;
    float Y;
    float Z;
    float eAng;
    float eLin;
    float len;
    float nx;
    float ny;
    float nz;
    float d1;
    float d2;
    float bx;
    float by;
    float bz;
    float cx;
    float cy;
    float cz;
    float vx;
    float vy;
    float vz;
    int i;
    int new1df;

    obj = pObject;
    acc = 1;
    if (obj != NULL) {
    for (; obj != NULL; obj = obj->next) {
        if (obj->field_0x21c != 1.0f) {
            obj->I.v[0] /= obj->field_0x21c;
            obj->I.v[1] /= obj->field_0x21c;
            obj->I.v[2] /= obj->field_0x21c;
        }
        if (obj->field_0xf0 != 0) {
            obj->field_0xf0 = 0;
            obj->disable_move_rotate = (tU8)(((tU32)obj->flags >> 5) & 1);
        }
        {
            int f = obj->flags;
            f = (f & ~0x100) | ((f >> 1) & 0x100);
            obj->flags = f;
        }
        new1df = 0;
        if (obj->disable_move_rotate == 0) {
            if (obj->field_0x1df != 0 && obj->field183_0x1d8 == obj) {
                obj->field_0x1df = 0;
            }
            X = obj->field_0x1cc.v[0] + obj->omega.v[0];
            Y = obj->field_0x1cc.v[1] + obj->omega.v[1];
            Z = obj->field_0x1cc.v[2] + obj->omega.v[2];
            eAng = (obj->I.v[0] * X * X + obj->I.v[1] * Y * Y + obj->I.v[2] * Z * Z) / obj->M;
            if (obj->collision_flag != 0 || obj->field_0x1df == 0) {
                eLin = obj->v.v[0] * obj->v.v[0] + obj->v.v[1] * obj->v.v[1] + obj->v.v[2] * obj->v.v[2];
            } else {
                eLin = 0.0f;
            }
            if (eLin < 0.001 && eAng < 0.00009) {
                len = sqrtf(obj->field_0x54.v[0] * obj->field_0x54.v[0] + obj->field_0x54.v[1] * obj->field_0x54.v[1] + obj->field_0x54.v[2] * obj->field_0x54.v[2]);
                nx = obj->field_0x54.v[0] / len;
                ny = obj->field_0x54.v[1] / len;
                nz = obj->field_0x54.v[2] / len;
                d1 = -(obj->v.v[0] * nx + obj->v.v[1] * ny + obj->v.v[2] * nz);
                d2 = -(obj->field_0x1b4.v[0] * nx + obj->field_0x1b4.v[1] * ny + obj->field_0x1b4.v[2] * nz);
                if (obj->field_0x1df == 0) {
                    if (d1 < 0.012f && (d1 - d2) > 0.9f * len) {
                        bx = obj->omega.v[1] * obj->cmpos.v[2] - obj->omega.v[2] * obj->cmpos.v[1] + obj->velocity_car_space.v[0];
                        by = obj->omega.v[2] * obj->cmpos.v[0] - obj->omega.v[0] * obj->cmpos.v[2] + obj->velocity_car_space.v[1];
                        bz = obj->omega.v[0] * obj->cmpos.v[1] - obj->omega.v[1] * obj->cmpos.v[0] + obj->velocity_car_space.v[2];
                        for (i = 0; i < 8; i++) {
                            cx = (i & 2) ? obj->bb1.min.v[0] : obj->bb1.max.v[0];
                            cy = (i & 1) ? obj->bb1.min.v[1] : obj->bb1.max.v[1];
                            cz = (i & 4) ? obj->bb1.min.v[2] : obj->bb1.max.v[2];
                            vx = obj->omega.v[1] * cz - obj->omega.v[2] * cy + bx;
                            vy = obj->omega.v[2] * cx - obj->omega.v[0] * cz + by;
                            vz = obj->omega.v[0] * cy - obj->omega.v[1] * cx + bz;
                            if (vx * vx + vy * vy + vz * vz > 0.0004f) {
                                goto copy;
                            }
                        }
                    } else {
                        goto copy;
                    }
                }
                if (obj->physics_joint2 == NULL || obj->collision_flag != 0 || obj->transform_matrix.m[1][1] > 0.999) {
                    new1df = 1;
                    BrVector3SetFloat(&obj->v, 0.0f, 0.0f, 0.0f);
                    BrVector3SetFloat(&obj->omega, 0.0f, 0.0f, 0.0f);
                    BrVector3SetFloat(&obj->rotate_omega, 0.0f, 0.0f, 0.0f);
                }
            }
        }
copy:
        obj->field_0x1df = (tU8)new1df;
        BrMatrix34Copy(&obj->transform_matrix, &obj->actor->t.t.mat);
        BrMatrix34ApplyP(&obj->pos, &obj->cmpos, &obj->transform_matrix);
        if (obj->child != NULL) {
            child_acc = AddDoubleTorqueToMatrix(obj->child);
            if (obj->field_0x1df != 0) {
                obj->field_0x1df = (tU8)((child_acc << 1) + obj->field_0x1df);
            }
        } else {
            obj->field_0x1df = (tU8)(obj->field_0x1df * 3);
        }
        acc &= ((tU32)obj->field_0x1df >> 1) & 1;
    }
    }
    return acc;
}

// FUNCTION: CARMA2_HW 0x004bae80
void C2_HOOK_FASTCALL DRMatrix33Inverse(tPhysics_object* pObject, int pFlag) {
    tPhysics_object* sibling;

    pObject->field_0x1dc = (tU8)pFlag;
    for (sibling = pObject->child; sibling != NULL; sibling = sibling->next) {
        if (sibling->physics_joint1 != NULL && sibling->physics_joint1->type != 0) {
            DRMatrix33Inverse(sibling, pFlag);
        }
    }
}

static void ResetObjectAndChildren(tPhysics_object* pObject);
int C2_HOOK_FASTCALL ClampObjectSpinTree(tPhysics_object* pObject);

// UNLABELED DATA: CARMA2_HW 0x00692dd8
int gINT_00692dd8 = 0;

// UNLABELED: CARMA2_HW 0x004bd4f0
// Universal-joint axis builder. FASTCALL: ecx = mA, edx = mB, stack args
// (c0, p0, c1, p1, outA, outB), ret 0x18. Computes
// outB = BrVector3Normalise(mB^T*(mA*c0) + p0), falling back to (1,0,0)
// when |v| <= 2.3841858e-07f, then outA = mB^T*(mA*c1) reflected against
// outB and against p0.
extern void C2_HOOK_FASTCALL BuildUniversalJointAxes(br_matrix34* pMatA, br_matrix34* pMatB, br_vector3* pChild0, br_vector3* pParent0, br_vector3* pChild1, br_vector3* pParent1, br_vector3* pAxis, br_vector3* pNormal) {
    br_vector3 v;
    br_vector3 t;
    float d;

    BrMatrix34ApplyV(&v, pChild0, pMatA);
    BrMatrix34TApplyV(&t, &v, pMatB);
    pNormal->v[0] = t.v[0] + pParent0->v[0];
    pNormal->v[1] = t.v[1] + pParent0->v[1];
    pNormal->v[2] = t.v[2] + pParent0->v[2];
    d = sqrtf(pNormal->v[0] * pNormal->v[0] + pNormal->v[2] * pNormal->v[2] + pNormal->v[1] * pNormal->v[1]);
    if (d <= 2.3841858e-07f) {
        pNormal->v[0] = 1.0f;
        pNormal->v[1] = 0.0f;
        pNormal->v[2] = 0.0f;
    } else {
        d = 1.0f / d;
        pNormal->v[0] = pNormal->v[0] * d;
        pNormal->v[1] = pNormal->v[1] * d;
        pNormal->v[2] = pNormal->v[2] * d;
    }
    BrMatrix34ApplyV(&v, pChild1, pMatA);
    BrMatrix34TApplyV(pAxis, &v, pMatB);
    d = pAxis->v[1] * pNormal->v[1] + pNormal->v[2] * pAxis->v[2] + pNormal->v[0] * pAxis->v[0];
    d = d + d;
    t.v[1] = pAxis->v[1] - pNormal->v[1] * d;
    t.v[2] = pAxis->v[2] - pNormal->v[2] * d;
    pAxis->v[0] = pAxis->v[0] - d * pNormal->v[0];
    pAxis->v[1] = t.v[1];
    pAxis->v[2] = t.v[2];
    d = pParent0->v[2] * pAxis->v[2] + pParent0->v[1] * pAxis->v[1] + pAxis->v[0] * pParent0->v[0];
    d = d + d;
    pAxis->v[0] = pAxis->v[0] - d * pParent0->v[0];
    pAxis->v[1] = pAxis->v[1] - pParent0->v[1] * d;
    pAxis->v[2] = pAxis->v[2] - pParent0->v[2] * d;
}

// Joint-limit record emitter of the QuickHinge cluster. Signature:
// FASTCALL(pObject, pRecords) + stack (count, maxRecords), ret 8; the
// "0x28" passed by SetUpQuickHingeData is maxRecords (40), the record
// stride is 0x60. Walks both joints of pObject, emitting constraint
// records consumed by SetUpQuickHingeData's second phase (0x4bb22b):
//   pObject->physics_joint1 (0x188), limit types 0 (plane) and 1
//     (universal): emit record type 0xb with depth = dot1 - dot2 vs
//     limit value/thresholds; objB = pObject->field_0x230. Universal
//     limits consume two limit records and use 0x4bd4f0.
//   pObject->physics_joint2 (0x18c), limit types 0, 10 and 11:
//     type 0 (plane): objB = NULL, does not increment count (quirk);
//       type 10 (weld): only when pObject->field_0x1e0[0] != 0, sets
//       gINT_00692dd8 = 1 and rewrites/erases matching pending records
//       (type 0xe marking); type 11: record type 2 with normal
//       ±joint2->hinge_axis and interpolated ratio.
// Record layout (0x60 bytes): +0 depth/ratio, +4 objA, +8 objB,
//   +0x14/+0x20 joint->field_0x08 and joint->field_0x14 copies,
//   +0x2c vector, +0x38 normal, +0x5c type (0xb/2/0xe).
// Constants: 0.0f @0x58a768, 0.001 @0x58a7a0, 1e-6 @0x58a810,
//   0.002 @0x58a818, 1.0 @0x58a808, 2.3841858e-7f @0x58a7fc.
// FUNCTION: CARMA2_HW 0x004bca10
extern int C2_HOOK_FASTCALL CollectJointLimitsIntoBuffer(tPhysics_object* pObject, tQuickHinge_record* pRecords, int pCount, int pMaxRecords) {
    tPhysics_joint* joint;
    tPhysics_object* base;
    tQuickHinge_record* rec;
    br_vector3 vec;
    br_vector3 vec2;
    br_vector3 cross;
    br_vector3 outA1;
    br_vector3 outA2;
    br_vector3 outB1;
    float dot;
    float dot2;
    float len;
    int i;

    joint = pObject->physics_joint1;
    if (joint != NULL) {
        base = pObject->field_0x230;
        if (base == NULL) {
            PhysicsError((tPhysicsError)0xb, "");
        }
        if (joint->count_limits > 0) {
            i = 0;
            do {
                if (pMaxRecords <= pCount) {
                    break;
                }
                rec = &pRecords[pCount];
                switch (joint->limits[i].type) {
                case eJoint_limit_plane: {
                    BrMatrix34ApplyV(&vec, &joint->limits[i].child, &pObject->actor->t.t.mat);
                    BrMatrix34TApplyV(&vec2, &vec, &base->actor->t.t.mat);
                    dot = vec2.v[0] * joint->limits[i].parent.v[0] + vec2.v[1] * joint->limits[i].parent.v[1] + vec2.v[2] * joint->limits[i].parent.v[2];
                    if (dot < 0.001) {
                        BrMatrix34ApplyV(&vec, &joint->limits[i].child, &pObject->transform_matrix);
                        BrMatrix34TApplyV(&vec2, &vec, &base->transform_matrix);
                        dot2 = vec2.v[0] * joint->limits[i].parent.v[0] + vec2.v[1] * joint->limits[i].parent.v[1] + vec2.v[2] * joint->limits[i].parent.v[2];
                        if (0.0f <= dot2 && (0.001 <= dot2 || pObject->field_0xf0 != 0)) {
                            rec->depth = dot - dot2;
                            rec->type = 0xb;
                            rec->object1 = pObject;
                            rec->object2 = base;
                            cross.v[0] = vec2.v[1] * joint->limits[i].parent.v[2] - vec2.v[2] * joint->limits[i].parent.v[1];
                            cross.v[1] = vec2.v[2] * joint->limits[i].parent.v[0] - vec2.v[0] * joint->limits[i].parent.v[2];
                            cross.v[2] = vec2.v[0] * joint->limits[i].parent.v[1] - vec2.v[1] * joint->limits[i].parent.v[0];
                            len = sqrtf(cross.v[0] * cross.v[0] + cross.v[1] * cross.v[1] + cross.v[2] * cross.v[2]);
                            if (len <= 1e-6) {
                                BrMatrix34ApplyV(&vec, &pObject->omega, &pObject->transform_matrix);
                                BrMatrix34TApplyV(&rec->field_0x38, &vec, &base->transform_matrix);
                                rec->field_0x38.v[0] = rec->field_0x38.v[0] - base->omega.v[0];
                                rec->field_0x38.v[1] = rec->field_0x38.v[1] - base->omega.v[1];
                                rec->field_0x38.v[2] = rec->field_0x38.v[2] - base->omega.v[2];
                                len = sqrtf(rec->field_0x38.v[0] * rec->field_0x38.v[0] + rec->field_0x38.v[1] * rec->field_0x38.v[1] + rec->field_0x38.v[2] * rec->field_0x38.v[2]);
                                if (len <= 2.3841858e-07f) {
                                    rec->field_0x38.v[0] = 1.0f;
                                    rec->field_0x38.v[1] = 0.0f;
                                    rec->field_0x38.v[2] = 0.0f;
                                } else {
                                    len = 1.0 / len;
                                    rec->field_0x38.v[0] = rec->field_0x38.v[0] * len;
                                    rec->field_0x38.v[1] = rec->field_0x38.v[1] * len;
                                    rec->field_0x38.v[2] = rec->field_0x38.v[2] * len;
                                }
                            } else {
                                rec->field_0x38.v[0] = cross.v[0] / len;
                                rec->field_0x38.v[1] = cross.v[1] / len;
                                rec->field_0x38.v[2] = cross.v[2] / len;
                            }
                            BrMatrix34ApplyV(&vec, &rec->field_0x38, &base->transform_matrix);
                            BrMatrix34TApplyV(&rec->field_0x2c, &vec, &pObject->transform_matrix);
                            rec->field_0x14 = joint->field_0x08;
                            rec->field_0x20 = joint->field_0x14;
                            pCount++;
                        }
                    }
                    break;
                }
                case eJoint_limit_universal: {
                    BuildUniversalJointAxes(&pObject->actor->t.t.mat, &base->actor->t.t.mat,
                                            &joint->limits[i].child, &joint->limits[i].parent,
                                            &joint->limits[i + 1].child, &joint->limits[i + 1].parent,
                                            &outA1, &outB1);
                    dot = outA1.v[0] * joint->limits[i + 1].parent.v[0] + outA1.v[1] * joint->limits[i + 1].parent.v[1] + outA1.v[2] * joint->limits[i + 1].parent.v[2];
                    if (dot < joint->limits[i].value - 1.0) {
                        BuildUniversalJointAxes(&pObject->transform_matrix, &base->transform_matrix,
                                                &joint->limits[i].child, &joint->limits[i].parent,
                                                &joint->limits[i + 1].child, &joint->limits[i + 1].parent,
                                                &outA2, &rec->field_0x38);
                        dot2 = outA2.v[0] * joint->limits[i + 1].parent.v[0] + outA2.v[1] * joint->limits[i + 1].parent.v[1] + outA2.v[2] * joint->limits[i + 1].parent.v[2];
                        if (joint->limits[i].value < dot2) {
                            cross.v[0] = outA1.v[1] * outA2.v[2] - outA1.v[2] * outA2.v[1];
                            cross.v[1] = outA1.v[2] * outA2.v[0] - outA1.v[0] * outA2.v[2];
                            cross.v[2] = outA1.v[0] * outA2.v[1] - outA1.v[1] * outA2.v[0];
                            if (rec->field_0x38.v[0] * cross.v[0] + rec->field_0x38.v[1] * cross.v[1] + rec->field_0x38.v[2] * cross.v[2] < 0.0f) {
                                rec->field_0x38.v[0] = -rec->field_0x38.v[0];
                                rec->field_0x38.v[1] = -rec->field_0x38.v[1];
                                rec->field_0x38.v[2] = -rec->field_0x38.v[2];
                            }
                            BrMatrix34ApplyV(&vec, &rec->field_0x38, &base->transform_matrix);
                            BrMatrix34TApplyV(&rec->field_0x2c, &vec, &pObject->transform_matrix);
                            rec->depth = dot - dot2;
                            rec->type = 0xb;
                            rec->object1 = pObject;
                            rec->object2 = base;
                            pCount++;
                        }
                    }
                    i++;
                    break;
                }
                default:
                    PhysicsError((tPhysicsError)8, "");
                    break;
                }
                i++;
            } while (i < joint->count_limits);
        }
    }
    joint = pObject->physics_joint2;
    if (joint == NULL || joint->count_limits < 1) {
        return pCount;
    }
    for (i = 0; i < joint->count_limits; i++) {
        switch (joint->limits[i].type) {
        case eJoint_limit_plane: {
            BrMatrix34ApplyV(&vec, &joint->limits[i].child, &pObject->actor->t.t.mat);
            dot = joint->limits[i].parent.v[0] * vec.v[0] + joint->limits[i].parent.v[1] * vec.v[1] + joint->limits[i].parent.v[2] * vec.v[2];
            if (dot < 0.001) {
                if (pCount == pMaxRecords) {
                    return pCount;
                }
                pRecords[pCount].depth = dot;
                pRecords[pCount].type = 0xb;
                pRecords[pCount].object1 = pObject;
                pRecords[pCount].object2 = NULL;
                cross.v[0] = joint->limits[i].parent.v[2] * vec.v[1] - joint->limits[i].parent.v[1] * vec.v[2];
                cross.v[1] = joint->limits[i].parent.v[0] * vec.v[2] - joint->limits[i].parent.v[2] * vec.v[0];
                cross.v[2] = joint->limits[i].parent.v[1] * vec.v[0] - joint->limits[i].parent.v[0] * vec.v[1];
                len = sqrtf(cross.v[0] * cross.v[0] + cross.v[1] * cross.v[1] + cross.v[2] * cross.v[2]);
                if (len > 1e-6) {
                    pRecords[pCount].field_0x38.v[0] = cross.v[0] / len;
                    pRecords[pCount].field_0x38.v[1] = cross.v[1] / len;
                    pRecords[pCount].field_0x38.v[2] = cross.v[2] / len;
                }
            }
            break;
        }
        case eJoint_limit_10: {
            if (pObject->field_0x1e0[0] != 0) {
                int scan;
                int count;
                int n;
                tQuickHinge_record* r;
                tQuickHinge_record* d;
                gINT_00692dd8 = 1;
                scan = 0;
                count = pCount;
                r = pRecords;
                if (pCount > 0) {
                    do {
                        if (r->object1 == pObject) {
                            if (r->object2 == NULL) {
                                if (scan < count) {
                                    n = count - scan;
                                    d = r;
                                    do {
                                        *d = *(d + 1);
                                        d++;
                                        n--;
                                    } while (n != 0);
                                }
                                count--;
                                scan--;
                                r--;
                            } else {
                                float x;
                                float y;
                                float z;
                                x = r->field_0x2c.v[0];
                                y = r->field_0x2c.v[1];
                                z = r->field_0x2c.v[2];
                                r->field_0x2c.v[0] = -r->field_0x38.v[0];
                                r->field_0x2c.v[1] = -r->field_0x38.v[1];
                                r->field_0x2c.v[2] = -r->field_0x38.v[2];
                                r->field_0x38.v[0] = -x;
                                r->field_0x38.v[1] = -y;
                                r->field_0x38.v[2] = -z;
                                vec = r->field_0x14;
                                r->field_0x14 = r->field_0x20;
                                r->field_0x20 = vec;
                                r->object1 = r->object2;
                                r->object2 = pObject;
                                r->type = 0xe;
                            }
                        } else if (r->object2 == pObject) {
                            r->type = 0xe;
                        }
                        scan++;
                        r++;
                    } while (scan < count);
                }
                pCount = count;
            }
            break;
        }
        case eJoint_limit_11: {
            float proj_cm;
            float limit_plane;
            float actor_pos;
            float obj_pos;
            BrMatrix34ApplyV(&vec, &joint->hinge_axis, &pObject->transform_matrix);
            proj_cm = pObject->field_0x17c.v[0] * vec.v[0] + pObject->field_0x17c.v[1] * vec.v[1] + pObject->field_0x17c.v[2] * vec.v[2];
            limit_plane = joint->limits[i].value + proj_cm;
            actor_pos = pObject->actor->t.t.mat.m[3][0] * vec.v[0] + pObject->actor->t.t.mat.m[3][1] * vec.v[1] + pObject->actor->t.t.mat.m[3][2] * vec.v[2];
            obj_pos = pObject->transform_matrix.m[3][0] * vec.v[0] + pObject->transform_matrix.m[3][1] * vec.v[1] + pObject->transform_matrix.m[3][2] * vec.v[2];
            if (actor_pos <= limit_plane || limit_plane - 1.0 <= obj_pos) {
                if (actor_pos < proj_cm - 0.001 && proj_cm - 0.002 < obj_pos) {
                    if (pCount == pMaxRecords) {
                        return pCount;
                    }
                    rec = &pRecords[pCount];
                    rec->depth = (proj_cm - obj_pos) / (actor_pos - obj_pos);
                    rec->field_0x0c = 0.0f;
                    rec->type = 2;
                    rec->object1 = pObject;
                    rec->object2 = NULL;
                    rec->field_0x2c = joint->hinge_axis;
                    BrVector3SetFloat(&rec->field_0x14, 0, 0, 0);
                    pCount++;
                }
            } else {
                if (pCount == pMaxRecords) {
                    return pCount;
                }
                rec = &pRecords[pCount];
                rec->depth = (limit_plane - obj_pos) / (actor_pos - obj_pos);
                rec->field_0x0c = 0.0f;
                rec->type = 2;
                rec->object1 = pObject;
                rec->object2 = NULL;
                rec->field_0x2c.v[0] = -joint->hinge_axis.v[0];
                rec->field_0x2c.v[1] = -joint->hinge_axis.v[1];
                rec->field_0x2c.v[2] = -joint->hinge_axis.v[2];
                BrVector3SetFloat(&rec->field_0x14, 0, 0, 0);
                pCount++;
            }
            break;
        }
        default:
            PhysicsError((tPhysicsError)8, "");
            break;
        }
    }
    return pCount;
}

// C2PORT-NONMATCHING: best 72.22% | difftest PASS 2000 trials (spec difftest/specs/004bc8e0_FUN_004bc8e0.json) | reason VC5 x87 scheduling and stack layout
// FUNCTION: CARMA2_HW 0x004bc8e0
void C2_HOOK_FAKE_THISCALL FUN_004bc8e0(tPhysics_object* pObject, br_scalar pScale) {
    tPhysics_object* pCurrent;
    int* piVar1;
    int local_a0[40];
    int iVar2;
    int iVar4;
    int* piVar3;

    pObject->I.v[0] = pScale * pObject->I.v[0];
    pObject->I.v[1] = pScale * pObject->I.v[1];
    pObject->I.v[2] = pScale * pObject->I.v[2];
    pObject->field_0x21c = pScale * pObject->field_0x21c;
    if (pObject->field_0x1e4 != 0) {
        iVar4 = *(int*)(pObject->field_0x1e4 + 4);
        *(float*)(iVar4 + 8) = pScale * *(float*)(iVar4 + 8);
        *(float*)(iVar4 + 0xc) = pScale * *(float*)(iVar4 + 0xc);
        *(float*)(iVar4 + 0x10) = pScale * *(float*)(iVar4 + 0x10);
        *(float*)(iVar4 + 0x21c) = pScale * *(float*)(iVar4 + 0x21c);
        FUN_004b8cf0(pObject);
    }
    pCurrent = pObject;
    while (pCurrent->field_0x218 == 0 && pCurrent->parent != NULL) {
        pCurrent = pCurrent->parent;
    }
    piVar1 = (int*)(uintptr_t)pCurrent->field_0x218;
    if (piVar1 != NULL) {
        iVar4 = 0;
        if (0 < *piVar1) {
            iVar2 = 0;
            piVar3 = local_a0;
            do {
                iVar4++;
                *piVar3 = iVar2 + piVar1[1];
                iVar2 += 0x60;
                piVar3++;
            } while (iVar4 < *piVar1);
        }
        FUN_004bf490((int*)(uintptr_t)piVar1[6], *piVar1, local_a0, *piVar1);
        MatrixProcess((tU32)(uintptr_t)piVar1[6], (tU32)piVar1[5], (tU32)*piVar1);
    }
}

// UNLABELED: CARMA2_HW 0x004bc8e0
// Spin-damping application, FASTCALL(pObject, float factor) ret 4:
// scales pObject->I and pObject->field_0x21c by factor, and the same
// fields of the object linked through pObject->field_0x1e4 (+4). Then
// calls 0x4b8cf0(pObject) (unnamed, ~0x1f0, itself calling 0x4b8c20,
// 0x4b8e10, 0x4be350, 0x4b77b0, 0x4b7960, BrMatrix34ApplyV ...), then
// walks parents (0x228) up to the first object whose field_0x218 != 0,
// takes group = that field and, when non-NULL, builds an array of
// `group[0].count` pointers (base group[4], stride 0x60) and calls
// 0x4bf490(group[0x18], array, count) and 0x4c5b50(group[0x18],
// group[0x14], count). Only reached at extreme spin (see ClampObjectSpin).
// Body still to be ported.
extern void C2_HOOK_FASTCALL ApplySpinDampingFactor(tPhysics_object* pObject, float pFactor) {
    (void)pObject;
    (void)pFactor;
}

// Runs CollectJointLimitsIntoBuffer over pObject and its following siblings,
// recursing into the child chains first, and returns the accumulated record
// count.
// FUNCTION: CARMA2_HW 0x004bd6d0
extern int C2_HOOK_FASTCALL CollectJointLimitsOfTree(tPhysics_object* pObject, void* pBuf, int pCount, int pStride) {
    if (pObject != NULL) {
        do {
            pCount = CollectJointLimitsIntoBuffer(pObject, pBuf, pCount, pStride);
            if (pObject->child != NULL) {
                pCount = CollectJointLimitsOfTree(pObject->child, pBuf, pCount, pStride);
            }
            pObject = pObject->next;
        } while (pObject != NULL);
    }
    return pCount;
}

// Clamps runaway angular velocity: once omega^2 * 0.0016 exceeds 2.4649 the
// spin (also of the object linked through field_0x1e4) is scaled by 1/(2*a),
// a = 1.27388525*sqrt(omega^2*0.0016), and the damping is reported through
// ApplySpinDampingFactor plus the field_0x198 callback. Returns 1 when
// clamped, 0 when the spin was already small.
// FUNCTION: CARMA2_HW 0x004bd720
extern int C2_HOOK_FASTCALL ClampObjectSpin(tPhysics_object* pObject) {
    void* holder;
    tPhysics_object* partner;
    float spin2;
    float a;

    spin2 = pObject->omega.v[0] * pObject->omega.v[0]
        + pObject->omega.v[1] * pObject->omega.v[1]
        + pObject->omega.v[2] * pObject->omega.v[2];
    if (!(spin2 * 0.0016f > 2.4649f)) {
        return 0;
    }
    a = 1.27388525f * sqrtf(spin2 * 0.0016f);
    pObject->omega.v[0] /= 2.0f * a;
    pObject->omega.v[1] /= 2.0f * a;
    pObject->omega.v[2] /= 2.0f * a;
    holder = (void*)(uintptr_t)pObject->field_0x1e4;
    if (holder != NULL) {
        partner = *(tPhysics_object**)((tU8*)holder + 4);
        partner->omega.v[0] /= 2.0f * a;
        partner->omega.v[1] /= 2.0f * a;
        partner->omega.v[2] /= 2.0f * a;
    }
    ApplySpinDampingFactor(pObject, a * a);
    if (*(int*)&pObject->field_0x191[7] != 0) {
        ((void (C2_HOOK_FASTCALL *)(tPhysics_object*))*(int*)&pObject->field_0x191[7])(pObject);
    }
    return 1;
}

// Runs ClampObjectSpin over pObject, its following siblings and the whole
// child chains, returning the number of objects clamped.
// FUNCTION: CARMA2_HW 0x004bd7f0
extern int C2_HOOK_FASTCALL ClampObjectSpinTree(tPhysics_object* pObject) {
    int count;

    count = 0;
    for (; pObject != NULL; pObject = pObject->next) {
        count += ClampObjectSpin(pObject);
        if (pObject->child != NULL) {
            count += ClampObjectSpinTree(pObject->child);
        }
    }
    return count;
}

// FUNCTION: CARMA2_HW 0x004bd830
tPhysics_object** C2_HOOK_FASTCALL AppendOverlappingObjects(tPhysics_object* pObject, tPhysics_object* pList, tPhysics_object** pObjects) {
    tPhysics_object** pLink;
    tPhysics_object** pChild_link;
    tPhysics_object* pCurrent;
    tPhysics_object* child_head;
    float x;

    pLink = pObjects;
    if (pList->parent != NULL) {
        for (pCurrent = pList; pCurrent != NULL; pCurrent = pCurrent->next) {
            if ((pCurrent == pObject && (pObject->flags & 8) == 0)
                    || BoundsOverlapTest__finteray(&pObject->field_0x10c, &pCurrent->field_0x10c)) {
                *pLink = pCurrent;
                pLink = (tPhysics_object**)&pCurrent->field_0x234;
            }
        }
    } else {
        x = pObject->field_0x10c.min.v[0] - gFLOAT_00692dd0;
        for (pCurrent = pObject->prev; pCurrent != NULL && pCurrent->transform_matrix.m[3][0] >= x; pCurrent = pCurrent->prev) {
            if (BoundsOverlapTest__finteray(&pObject->field_0x10c, &pCurrent->field_0x10c)) {
                *pLink = pCurrent;
                pLink = (tPhysics_object**)&pCurrent->field_0x234;
            }
        }
        x = pObject->field_0x10c.max.v[0] - gFLOAT_00692dd4;
        for (pCurrent = pObject->next; pCurrent != NULL && pCurrent->transform_matrix.m[3][0] <= x; pCurrent = pCurrent->next) {
            if (BoundsOverlapTest__finteray(&pObject->field_0x10c, &pCurrent->field_0x10c)) {
                *pLink = pCurrent;
                pLink = (tPhysics_object**)&pCurrent->field_0x234;
            }
        }
        if ((pObject->flags & 8) == 0) {
            *pLink = pObject;
            pLink = (tPhysics_object**)&pObject->field_0x234;
        }
    }
    *pLink = NULL;

    child_head = NULL;
    pCurrent = *pObjects;
    pChild_link = &child_head;
    while (pCurrent != NULL) {
        if (pCurrent->child != NULL) {
            pChild_link = AppendOverlappingObjects(pObject, pCurrent->child, pChild_link);
        }
        pCurrent = (tPhysics_object*)pCurrent->field_0x234;
    }
    if (child_head != NULL) {
        *pChild_link = NULL;
        *pLink = child_head;
        return pChild_link;
    }
    return pLink;
}

static void WakeObjectOwner(tPhysics_object* pObject, tWorld_callbacks* pCallbacks) {
    tPhysics_object* owner;
    tPhysics_object* child;

    owner = pObject;
    while (owner->parent != NULL && owner->physics_joint1 != NULL && owner->physics_joint1->type != eJoint_none) {
        owner = owner->parent;
    }
    owner->field_0xf0 = 0;
    for (child = owner->child; child != NULL; child = child->next) {
        SetFieldF0Tree(child, 0);
    }
    if (pCallbacks != NULL && pCallbacks->activate_passive != NULL) {
        pCallbacks->activate_passive(owner, pObject);
    }
    if ((owner->child != NULL || owner->physics_joint2 != NULL) && owner->field_0x218 == 0) {
        MoveJointedObject(owner, 0, 0.f);
    }
}

// FUNCTION: CARMA2_HW 0x004bddd0
void C2_HOOK_FASTCALL FUN_004bddd0(tPhysics_object* pArg1, tPhysics_object* pArg2) {
    tPhysics_object* volatile pObject1 = pArg1;
    tPhysics_object* volatile pObject2 = pArg2;
    tPhysics_object* parent;
    register tPhysics_object* child;
    register tPhysics_object* group;
    char object1_mark;
    char mark;
    register int next_mark;

    parent = pObject1->parent;
    while (parent != NULL && pObject1->physics_joint1 != NULL && pObject1->physics_joint1->type != 0) {
        pObject1 = parent;
        parent = parent->parent;
    }
    parent = pObject2->parent;
    while (parent != NULL && pObject2->physics_joint1 != NULL && pObject2->physics_joint1->type != 0) {
        pObject2 = parent;
        parent = parent->parent;
    }

    if (pObject1 == pObject2) {
        return;
    }

    object1_mark = (char)pObject1->field_0x1dc;
    if (pObject2 == NULL) {
        if (object1_mark == 0) {
            next_mark = gINT_006940a0;
            pObject1->field_0x1dc = (tU8)next_mark;
            for (child = pObject1->child; child != NULL; child = child->next) {
                if (child->physics_joint1 != NULL && child->physics_joint1->type != 0) {
                    DRMatrix33Inverse(child, next_mark);
                }
            }
            pObject1->field183_0x1d8 = pObject1;
            gINT_006940a0++;
        }
        return;
    }

    if (object1_mark == 0) {
        if (pObject2->field_0x1dc == 0) {
            next_mark = gINT_006940a0;
            pObject1->field_0x1dc = (tU8)next_mark;
            for (child = pObject1->child; child != NULL; child = child->next) {
                if (child->physics_joint1 != NULL && child->physics_joint1->type != 0) {
                    DRMatrix33Inverse(child, next_mark);
                }
            }
            pObject2->field_0x1dc = (tU8)gINT_006940a0;
            for (child = pObject2->child; child != NULL; child = child->next) {
                if (child->physics_joint1 != NULL && child->physics_joint1->type != 0) {
                    DRMatrix33Inverse(child, gINT_006940a0);
                }
            }
            pObject1->field183_0x1d8 = pObject2;
            pObject2->field183_0x1d8 = pObject1;
            gINT_006940a0++;
            return;
        }

        mark = (char)pObject2->field_0x1dc;
        pObject1->field_0x1dc = (tU8)mark;
        for (child = pObject1->child; child != NULL; child = child->next) {
            if (child->physics_joint1 != NULL && child->physics_joint1->type != 0) {
                DRMatrix33Inverse(child, (int)mark);
            }
        }
        pObject1->field183_0x1d8 = pObject2->field183_0x1d8;
        pObject2->field183_0x1d8 = pObject1;
        return;
    }

    if (pObject2->field_0x1dc == 0) {
        pObject2->field_0x1dc = (tU8)object1_mark;
        for (child = pObject2->child; child != NULL; child = child->next) {
            if (child->physics_joint1 != NULL && child->physics_joint1->type != 0) {
                DRMatrix33Inverse(child, (int)object1_mark);
            }
        }
        pObject2->field183_0x1d8 = pObject1->field183_0x1d8;
        pObject1->field183_0x1d8 = pObject2;
        return;
    }

    if (object1_mark != (char)pObject2->field_0x1dc) {
        group = pObject2;
        while (group->field183_0x1d8 != pObject2) {
            group->field_0x1dc = (tU8)object1_mark;
            for (child = group->child; child != NULL; child = child->next) {
                if (child->physics_joint1 != NULL && child->physics_joint1->type != 0) {
                    DRMatrix33Inverse(child, (int)object1_mark);
                }
            }
            group = group->field183_0x1d8;
        }
        group->field_0x1dc = (tU8)object1_mark;
        for (child = group->child; child != NULL; child = child->next) {
            if (child->physics_joint1 != NULL && child->physics_joint1->type != 0) {
                DRMatrix33Inverse(child, (int)object1_mark);
            }
        }
        group->field183_0x1d8 = pObject1->field183_0x1d8;
        pObject1->field183_0x1d8 = pObject2;
    }
}

// APPROXIMATION: stands in for the object-vs-object contact generator at
// 0x425ca0 (the retail AddObjectObjectForce dispatch over the sphere/edge/
// corner helpers GetSphereSphereForceInfo / GetNonSphereNonSphereForceInfo /
// ...). Oriented-box SAT over the two objects' bb1 bounds; emits at most one
// contact on the minimum-overlap axis with the partner support point towards
// pObject as the contact position. `normal`/`field_0x38` are the same world
// direction (partner -> pObject) expressed in each object's transform space.
static int C2_HOOK_FASTCALL GenerateObjectObjectContacts(tPhysics_object* pObject, tPhysics_object* pPartner, tCollision_contact* pContacts, int pCount) {
    br_matrix34 inv1;
    br_matrix34 inv2;
    br_matrix34 to1;
    br_vector3 row2[3];
    br_vector3 e1;
    br_vector3 center1;
    br_vector3 center2;
    br_vector3 delta;
    br_vector3 normal;
    br_vector3 axis;
    br_vector3 world_normal;
    br_vector3 dir2;
    br_vector3 point1;
    br_vector3 point2;
    tCollision_contact* contact;
    float ext1[3];
    float ext2[3];
    float distance;
    float radius;
    float overlap;
    float best;
    float len;
    int i;
    int j;
    int a;
    int b;

    if (pCount > 0x24) {
        return 0;
    }
    if (!BoundsOverlapTest__finteray(&pObject->field_0xf4, &pPartner->field_0xf4)) {
        return 0;
    }
    for (i = 0; i < 3; i++) {
        ext1[i] = (pObject->bb1.max.v[i] - pObject->bb1.min.v[i]) * 0.5f;
        ext2[i] = (pPartner->bb1.max.v[i] - pPartner->bb1.min.v[i]) * 0.5f;
        center1.v[i] = (pObject->bb1.max.v[i] + pObject->bb1.min.v[i]) * 0.5f;
        center2.v[i] = (pPartner->bb1.max.v[i] + pPartner->bb1.min.v[i]) * 0.5f;
        if (ext1[i] <= 0.f || ext2[i] <= 0.f) {
            return 0;
        }
    }
    BrMatrix34LPInverse(&inv1, &pObject->transform_matrix);
    BrMatrix34LPInverse(&inv2, &pPartner->transform_matrix);
    /* BRender is row-vector: local1 = local2 * M2 * inv(M1); basis images are
       the matrix ROWS. */
    BrMatrix34Mul(&to1, &pPartner->transform_matrix, &inv1);
    BrMatrix34ApplyP(&delta, &center2, &to1);
    BrVector3Sub(&delta, &delta, &center1);
    for (i = 0; i < 3; i++) {
        row2[i].v[0] = to1.m[i][0];
        row2[i].v[1] = to1.m[i][1];
        row2[i].v[2] = to1.m[i][2];
    }
    best = 1.e30f;
    BrVector3SetFloat(&normal, 1.f, 0.f, 0.f);
    for (i = 0; i < 15; i++) {
        if (i < 3) {
            BrVector3SetFloat(&axis, 0.f, 0.f, 0.f);
            axis.v[i] = 1.f;
        } else if (i < 6) {
            BrVector3Copy(&axis, &row2[i - 3]);
        } else {
            a = (i - 6) / 3;
            b = (i - 6) % 3;
            BrVector3SetFloat(&e1, 0.f, 0.f, 0.f);
            e1.v[a] = 1.f;
            BrVector3Cross(&axis, &e1, &row2[b]);
        }
        len = axis.v[0] * axis.v[0] + axis.v[1] * axis.v[1] + axis.v[2] * axis.v[2];
        if (len < 1.e-12f) {
            continue;
        }
        len = 1.0f / sqrtf(len);
        axis.v[0] *= len;
        axis.v[1] *= len;
        axis.v[2] *= len;
        distance = axis.v[0] * delta.v[0] + axis.v[1] * delta.v[1] + axis.v[2] * delta.v[2];
        if (distance < 0.f) {
            distance = -distance;
        }
        radius = ext1[0] * fabsf(axis.v[0]) + ext1[1] * fabsf(axis.v[1]) + ext1[2] * fabsf(axis.v[2]);
        for (j = 0; j < 3; j++) {
            radius += ext2[j] * fabsf(axis.v[0] * row2[j].v[0] + axis.v[1] * row2[j].v[1] + axis.v[2] * row2[j].v[2]);
        }
        overlap = radius - distance;
        if (overlap < 0.f) {
            return 0;
        }
        if (overlap < best) {
            best = overlap;
            BrVector3Copy(&normal, &axis);
        }
    }
    if (best == 1.e30f) {
        return 0;
    }
    if (0.f < normal.v[0] * delta.v[0] + normal.v[1] * delta.v[1] + normal.v[2] * delta.v[2]) {
        BrVector3Negate(&normal, &normal);
    }
    BrMatrix34ApplyV(&world_normal, &normal, &pObject->transform_matrix);
    len = world_normal.v[0] * world_normal.v[0] + world_normal.v[1] * world_normal.v[1] + world_normal.v[2] * world_normal.v[2];
    if (len < 1.e-12f) {
        return 0;
    }
    len = 1.0f / sqrtf(len);
    world_normal.v[0] *= len;
    world_normal.v[1] *= len;
    world_normal.v[2] *= len;
    BrMatrix34ApplyV(&dir2, &world_normal, &inv2);
    for (i = 0; i < 3; i++) {
        point2.v[i] = center2.v[i] + (dir2.v[i] >= 0.f ? ext2[i] : -ext2[i]);
    }
    BrMatrix34ApplyP(&point1, &point2, &to1);
    contact = &pContacts[pCount];
    contact->impulse = 0.f;
    contact->object1 = pObject;
    contact->object2 = pPartner;
    contact->field_0xc = NULL;
    contact->field_0x10 = 0.f;
    BrVector3Sub(&contact->r1, &point1, &pObject->cmpos);
    BrVector3Sub(&contact->r2, &point2, &pPartner->cmpos);
    BrVector3Copy(&contact->normal, &normal);
    BrVector3Copy(&contact->field_0x38, &dir2);
    BrVector3SetFloat(&contact->impulse1, 0.f, 0.f, 0.f);
    BrVector3SetFloat(&contact->impulse2, 0.f, 0.f, 0.f);
    contact->type = 0;
    BrVector3Copy(&pObject->omega, &pObject->rotate_omega);
    BrVector3Copy(&pPartner->omega, &pPartner->rotate_omega);
    return 1;
}

// UNLABELED: CARMA2_HW 0x004bd9e0
// Contact-generation glue for one object: refreshes the faces in its box,
// appends track-face contacts (0x4228b0), then walks the overlap list built by
// QuickHingeCollect, appending object-object contacts (0x425ca0) and waking the
// jointed owner of either object on new contacts. Children with an active joint
// (0x188) recurse over the subset of the list overlapping their 0x10c box.
// gReduced_object_list/gINT_006940ac (0x6940a8/0x6940ac) are only live around
// GetFacesInBox. Returns the running contact count.
static int C2_HOOK_FASTCALL QuickHingeGen(tPhysics_object* pObject, tPhysics_object** ppPartner_list, tCollision_contact* pContacts, int pCount, tWorld_callbacks* pCallbacks, tPhysics_object* pHead) {
    tPhysics_object* partner;
    tPhysics_object* child;
    tPhysics_object* node;
    tPhysics_object** slot;
    tPhysics_object* moved;
    tPhysics_object** moved_tail;
    int added;

    gReduced_object_list = ppPartner_list;
    gINT_006940ac = (pObject->field_0x1b4.v[0] * pObject->field_0x1b4.v[0]
        + pObject->field_0x1b4.v[1] * pObject->field_0x1b4.v[1]
        + pObject->field_0x1b4.v[2] * pObject->field_0x1b4.v[2]) != 0.f;
    GetFacesInBox(pObject, pCallbacks);
    gReduced_object_list = NULL;
    gINT_006940ac = 0;
#if UINTPTR_MAX > 0xffffffffu
    /* See TestForObjectInSensiblePlace: the Vita-port collector assumes the
     * 32-bit tCollision_contact layout.  Use the portable path on 64-bit. */
    added = CollectPlacementFaces(pObject, pContacts, pCount);
#else
    added = FUN_004228b0(pObject, pContacts, pCount);
#endif
    if (added != 0) {
        if (pObject->field_0xf0 == 2) {
            WakeObjectOwner(pObject, pCallbacks);
        }
        pCount += added;
    }
    for (partner = *ppPartner_list; partner != NULL; partner = (tPhysics_object*)partner->field_0x234) {
        if (partner == pObject) {
            continue;
        }
        if (pObject->field_0x1dc != 0 && partner->field_0x1dc == pObject->field_0x1dc && (tU32)partner <= (tU32)pObject) {
            continue;
        }
        if (pObject->parent == partner || partner->parent == pObject) {
            continue;
        }
        if ((partner->field_0x1a4 & pObject->field_0x1a0) != 0
            || (partner->field_0x1a0 & pObject->field_0x1a4) != 0
            || (pObject->flags & 4) != 0
            || (partner->flags & 4) != 0) {
            continue;
        }
        /* APPROXIMATION: stands in for the 0x425ca0 object-vs-object contact
         * generator (retail AddObjectObjectForce dispatch). */
        added = GenerateObjectObjectContacts(pObject, partner, pContacts, pCount);
        if (added != 0) {
            if (pObject->field_0xf0 == 2) {
                WakeObjectOwner(pObject, pCallbacks);
            }
            if (partner->field_0xf0 == 2) {
                WakeObjectOwner(partner, pCallbacks);
            }
            /* NOT_IMPLEMENTED: CARMA2_HW 0x004be0e0: if (partner->field_0xec != 0)
             *   WakeAttachedObject(partner, pHead); */
            /* NOT_IMPLEMENTED: CARMA2_HW 0x004bddd0:
             *   RegisterCollisionPair(pObject, partner); */
            pCount += added;
        }
    }
    for (child = pObject->child; child != NULL; child = child->next) {
        if (child->physics_joint1 == NULL || child->physics_joint1->type == eJoint_none) {
            continue;
        }
        moved = NULL;
        moved_tail = &moved;
        slot = ppPartner_list;
        for (node = *slot; node != NULL; node = *slot) {
            if (node == child || BoundsOverlapTest__finteray(&child->field_0x10c, &node->field_0x10c)) {
                *slot = (tPhysics_object*)node->field_0x234;
                *moved_tail = node;
                moved_tail = (tPhysics_object**)&node->field_0x234;
            } else {
                slot = (tPhysics_object**)&node->field_0x234;
            }
        }
        *moved_tail = NULL;
        *slot = moved;
        pCount = QuickHingeGen(child, slot, pContacts, pCount, pCallbacks, pHead);
    }
    return pCount;
}

static void ResetObjectAndChildren(tPhysics_object* pObject);

// APPROXIMATION: stands in for the impulse array produced by the retail
// 0x4bf490/0x4c3b60 joint+impulse solver pass that SetUpQuickHingeData phase 2
// consumes (0x4bbdaf/0x4bbe08). Normal (approach-speed) impulse of one contact:
// the closing speed along the world-space contact normal times the effective
// mass (object mass for track contacts, reduced mass for pairs).
static float ComputeContactImpulse(tCollision_contact* pContact) {
    tPhysics_object* obj1;
    tPhysics_object* obj2;
    br_vector3 normal;
    float vn;

    obj1 = pContact->object1;
    obj2 = pContact->object2;
    BrMatrix34ApplyV(&normal, &pContact->normal, &obj1->transform_matrix);
    vn = normal.v[0] * obj1->v.v[0] + normal.v[1] * obj1->v.v[1] + normal.v[2] * obj1->v.v[2];
    if (obj2 != NULL) {
        vn -= normal.v[0] * obj2->v.v[0] + normal.v[1] * obj2->v.v[1] + normal.v[2] * obj2->v.v[2];
    }
    if (!(vn < 0.f)) {
        return 0.f;
    }
    if (obj2 != NULL && obj1->M != 0.f && obj2->M != 0.f) {
        return -vn / (1.f / obj1->M + 1.f / obj2->M);
    }
    return -vn * obj1->M;
}

// APPROXIMATION: applies one normal contact impulse to the involved objects.
// The retail impulse solver (0x4bf490/0x4c3b60) folds this into its velocity
// solve; nothing else in the reconstruction applies the normal component
// (RotateObjectFirstOrder is friction-only), which let cars sink through track
// faces and opponents. World push along the contact normal plus the COM torque
// from the lever arms, mirroring RotateObjectFirstOrder's frame conventions.
static void C2_HOOK_FASTCALL ApplyContactNormalImpulse(tCollision_contact* pContact, float pImpulse) {
    tPhysics_object* obj1;
    tPhysics_object* obj2;
    br_vector3 world_normal;
    float inv_m;

    obj1 = pContact->object1;
    obj2 = pContact->object2;
    BrMatrix34ApplyV(&world_normal, &pContact->normal, &obj1->transform_matrix);
    if (obj1->M != 0.f) {
        inv_m = pImpulse / obj1->M;
        obj1->v.v[0] += world_normal.v[0] * inv_m;
        obj1->v.v[1] += world_normal.v[1] * inv_m;
        obj1->v.v[2] += world_normal.v[2] * inv_m;
        obj1->velocity_car_space.v[0] += world_normal.v[0] * inv_m;
        obj1->velocity_car_space.v[1] += world_normal.v[1] * inv_m;
        obj1->velocity_car_space.v[2] += world_normal.v[2] * inv_m;
        if ((obj1->flags & 0x800) == 0) {
            obj1->omega.v[0] += pImpulse * (pContact->r1.v[1] * pContact->normal.v[2] - pContact->r1.v[2] * pContact->normal.v[1]) / obj1->I.v[0];
            obj1->omega.v[1] += pImpulse * (pContact->r1.v[2] * pContact->normal.v[0] - pContact->r1.v[0] * pContact->normal.v[2]) / obj1->I.v[1];
            obj1->omega.v[2] += pImpulse * (pContact->r1.v[0] * pContact->normal.v[1] - pContact->r1.v[1] * pContact->normal.v[0]) / obj1->I.v[2];
        }
    }
    if (obj2 != NULL && obj2->M != 0.f) {
        inv_m = pImpulse / obj2->M;
        obj2->v.v[0] -= world_normal.v[0] * inv_m;
        obj2->v.v[1] -= world_normal.v[1] * inv_m;
        obj2->v.v[2] -= world_normal.v[2] * inv_m;
        obj2->velocity_car_space.v[0] -= world_normal.v[0] * inv_m;
        obj2->velocity_car_space.v[1] -= world_normal.v[1] * inv_m;
        obj2->velocity_car_space.v[2] -= world_normal.v[2] * inv_m;
        if ((obj2->flags & 0x800) == 0) {
            obj2->omega.v[0] -= pImpulse * (pContact->r2.v[1] * pContact->field_0x38.v[2] - pContact->r2.v[2] * pContact->field_0x38.v[1]) / obj2->I.v[0];
            obj2->omega.v[1] -= pImpulse * (pContact->r2.v[2] * pContact->field_0x38.v[0] - pContact->r2.v[0] * pContact->field_0x38.v[2]) / obj2->I.v[1];
            obj2->omega.v[2] -= pImpulse * (pContact->r2.v[0] * pContact->field_0x38.v[1] - pContact->r2.v[1] * pContact->field_0x38.v[0]) / obj2->I.v[2];
        }
    }
}

// APPROXIMATION: sequential normal-impulse application over a contact range.
// Stands in for the retail 0x4bf490/0x4c3b60 solver passes; called from
// DoCollisions before the RotateObjectFirstOrder friction pass.
static void C2_HOOK_FASTCALL ApplyContactImpulses(tCollision_contact* pContacts, int pCount) {
    int i;

    for (i = 0; i < pCount; i++) {
        float impulse;

        impulse = ComputeContactImpulse(&pContacts[i]);
        if (impulse != 0.f) {
            ApplyContactNormalImpulse(&pContacts[i], impulse);
        }
    }
}

#if UINTPTR_MAX > 0xffffffffu
/* 64-bit portable contact collection.  The Vita-port SetUpQuickHingeData below
 * drives the 32-bit-only joint/impulse solver (FUN_004bf490/FUN_004c3b60 store
 * pointers as ints and use 0x60-byte record offsets), so keep the pre-port
 * approximation on 64-bit hosts.  The matching (32-bit) build still compiles
 * the ported body. */
static int C2_HOOK_FASTCALL SetUpQuickHingeDataPortable(tPhysics_object** pObject_list, tPhysics_object* pObject, void* pBuffer1, void* pBuffer2, int pArg, tWorld_callbacks* pWorld_callbacks) {
    tPhysics_object* head;
    tPhysics_object* cur;
    tPhysics_object* next;
    tPhysics_object* child;
    tPhysics_object* out;
    int cnt;
    int prev;
    float dx;
    float dy;
    float dz;
    float lv;
    float ld;

    (void)pBuffer2;
    (void)pArg;
    cnt = 0;
    head = *pObject_list;
    for (cur = pObject; ; cur = next) {
        next = cur->field183_0x1d8;
        prev = cnt;
        AppendOverlappingObjects(cur, head, &out);
        cnt = QuickHingeGen(cur, &out, (tCollision_contact*)pBuffer1, prev, pWorld_callbacks, head);
        if (gNet_mode == eNet_mode_host && cur->field_0xf0 == 1 && cnt > prev) {
            dx = cur->transform_matrix.m[3][0] - ((br_matrix34*)cur->field_0xbc)->m[3][0];
            dy = cur->transform_matrix.m[3][1] - ((br_matrix34*)cur->field_0xbc)->m[3][1];
            dz = cur->transform_matrix.m[3][2] - ((br_matrix34*)cur->field_0xbc)->m[3][2];
            lv = sqrtf(cur->v.v[0] * cur->v.v[0] + cur->v.v[1] * cur->v.v[1] + cur->v.v[2] * cur->v.v[2]);
            ld = sqrtf(dx * dx + dy * dy + dz * dz);
            if (lv + 0.5 < ld * 25.0f) {
                cur->flags |= 0x1000;
                cur->field_0xf0 = 0;
                for (cur = pObject; ; cur = cur->field183_0x1d8) {
                    if (cur->disable_move_rotate == 0) {
                        if (cur->field_0xf0 == 1) {
                            BrMatrix34Copy(&cur->actor->t.t.mat, (br_matrix34*)cur->field_0xbc);
                        } else {
                            BrMatrix34LPNormalise(&cur->actor->t.t.mat, &cur->transform_matrix);
                        }
                    }
                    for (child = cur->child; child != NULL; child = child->next) {
                        ResetObjectAndChildren(child);
                    }
                    MoveJointedObject(cur, 0, 0.04f);
                    MergeBoundsIntoParent(cur, NULL);
                    if (cur->field183_0x1d8 == pObject) {
                        return -1;
                    }
                }
            }
        }
        if (next == pObject) {
            break;
        }
    }
    if (cnt > 0) {
        tCollision_contact* contacts;
        tCollision_contact* dest;
        tPhysics_object* obj1;
        float impulses[0x28];
        int i;

        contacts = (tCollision_contact*)pBuffer1;
        for (i = 0; i < cnt; i++) {
            impulses[i] = ComputeContactImpulse(&contacts[i]);
        }
        /* 0x4bbe08: contacts the solver left unimpulsed are marked type 9. */
        for (i = 0; i < cnt; i++) {
            if (impulses[i] == 0.f) {
                contacts[i].type = 9;
            }
        }
        gINT_00692dc8 = 0;
        /* 0x4bbe34: car force callback (ecx = contacts, edx = impulses, push count). */
        if (pWorld_callbacks->process_forces != NULL) {
            pWorld_callbacks->process_forces(contacts, impulses, cnt);
        }
        /* 0x4bbe9a: impulse-carrying contacts are appended to gCollision_contacts. */
        for (i = 0; i < cnt; i++) {
            if (impulses[i] == 0.f) {
                continue;
            }
            if (contacts[i].type == 9) {
                continue;
            }
            if (contacts[i].type == 0xb) {
                continue;
            }
            obj1 = contacts[i].object1;
            if (contacts[i].object2 != NULL) {
                if ((obj1->collision_flag & 2) != 0 && (contacts[i].object2->collision_flag & 2) != 0) {
                    continue;
                }
            } else {
                if ((obj1->collision_flag & 1) != 0) {
                    continue;
                }
            }
            if (gINT_00692ddc >= 0x32) {
                continue;
            }
            dest = &gCollision_contacts[gINT_00692ddc];
            memcpy(dest, &contacts[i], sizeof(tCollision_contact));
            dest->impulse = impulses[i];
            gINT_00692ddc++;
        }
    }
    return 0;
}
#endif

// C2PORT-NONMATCHING: best 36.35% | difftest PASS 4000 trials (spec difftest/specs/004baec0_SetUpQuickHingeData.json, _legacy/_torque) | reason x87-order-and-stack-layout (legacy B-vector path unobservable under stubs)
// FUNCTION: CARMA2_HW 0x004baec0
int C2_HOOK_FASTCALL SetUpQuickHingeData(tPhysics_object** pObject_list, tPhysics_object* pObject, void* pBuffer1, void* pBuffer2, int pArg, tWorld_callbacks* pWorld_callbacks) {
#if UINTPTR_MAX > 0xffffffffu
    return SetUpQuickHingeDataPortable(pObject_list, pObject, pBuffer1, pBuffer2, pArg, pWorld_callbacks);
#else
    tCollision_contact* contacts;
    tPhysics_object* overlap_head;
    tPhysics_object* list_head;
    tPhysics_object* obj;
    tPhysics_object* next_obj;
    tPhysics_object* child;
    tPhysics_object* top;
    tPhysics_object* orig;
    tCollision_contact* rows[100];
    tCollision_contact** rows_c;
    tPhysics_group_data* groups[100];
    float solve_a[100];
    float solve_b[100];
    float* a_c;
    float* b_c;
    tU8 solve_flags[100];
    tU8 solve_other[4];
    tPhysics_group_data* group;
    tCollision_contact* group_rows[40];
    float group_b[100];
    float group_a[100];
    tPhysics_joint* joint;
    tCollision_contact* rec;
    tCollision_contact* p;
    tPhysics_object* o1;
    tPhysics_object* o2;
    br_vector3 tmp;
    br_vector3 tmp2;
    br_vector3 torque;
    br_vector3 cross;
    float thr;
    float lam;
    float value;
    float max_value;
    float step;
    int count;
    int old_count;
    int ring_length;
    int total;
    int contact_count;
    int joint_rows;
    int group_count;
    int nrows;
    int i;
    int k;
    int idx;
    int n;
    int any;
    int solve_result;
    int changed;
    int sum_children;
    int sum_all;
    int flags_acc;
    float dx;
    float dy;
    float dz;

    contacts = (tCollision_contact*)pBuffer1;
    count = 0;
    obj = pObject;
    do {
        next_obj = obj->field183_0x1d8;
        old_count = count;
        list_head = *pObject_list;
        AppendOverlappingObjects(obj, list_head, &overlap_head);
        count = QuickHingeGen(obj, &overlap_head, pBuffer1, count, pWorld_callbacks, list_head);
        if (gNet_mode == 2 && obj->field_0xf0 == 1 && count > old_count) {
            dx = obj->transform_matrix.m[3][0] - ((float*)&obj->field_0xbc[0x24])[0];
            dy = obj->transform_matrix.m[3][1] - ((float*)&obj->field_0xbc[0x24])[1];
            dz = obj->transform_matrix.m[3][2] - ((float*)&obj->field_0xbc[0x24])[2];
            if (sqrtf(obj->v.v[2] * obj->v.v[2] + obj->v.v[1] * obj->v.v[1] + obj->v.v[0] * obj->v.v[0]) - gDOUBLE_0058a7d0
                    < sqrtf(dz * dz + dy * dy + dx * dx) * gFLOAT_0058a7c8) {
                obj->flags |= 0x1000;
                obj->field_0xf0 = 0;
                obj = pObject;
                do {
                    if (obj->disable_move_rotate == 0) {
                        if (obj->field_0xf0 == 1) {
                            BrMatrix34Copy(&obj->actor->t.t.mat, (const br_matrix34*)&obj->field_0xbc);
                        } else {
                            BrMatrix34LPNormalise(&obj->actor->t.t.mat, &obj->transform_matrix);
                        }
                        for (child = obj->child; child != NULL; child = child->next) {
                            ResetObjectAndChildren(child);
                        }
                        FUN_004b8ee0(obj CARPOCALYPSE2_THISCALL_EDX, 0.04f);
                        MergeBoundsIntoParent(obj, NULL);
                        if (obj->field_0x194 != 0) {
                            ((int (C2_HOOK_FASTCALL *)(tPhysics_object*))(uintptr_t)obj->field_0x194)(obj);
                        }
                    }
                    obj = obj->field183_0x1d8;
                } while (obj != pObject);
                return -1;
            }
        }
        obj = next_obj;
    } while (next_obj != pObject);

    gINT_00692dd8 = 0;
    ring_length = 0;
    obj = pObject;
    do {
        ring_length++;
        if (ring_length > 100) {
            PhysicsError((tPhysicsError)10, "Corrupt group");
        }
        obj = obj->field183_0x1d8;
    } while (obj != pObject);

    obj = pObject;
    do {
        if ((obj->flags & 0x10) == 0) {
            old_count = count;
            count = CollectJointLimitsIntoBuffer(obj, (undefined4*)pBuffer1, count, 0x28);
            child = obj->child;
            if (child != NULL) {
                for (; child != NULL; child = child->next) {
                    count = CollectJointLimitsIntoBuffer(child, (undefined4*)pBuffer1, count, 0x28);
                    if (child->child != NULL) {
                        count = CollectJointLimitsOfTree(child->child, (undefined4*)pBuffer1, count, 0x28);
                    }
                }
            }
            if (old_count != count && obj->field_0xf0 == 2) {
                top = obj;
                while (top->parent != NULL && top->physics_joint1 != NULL && top->physics_joint1->type != 0) {
                    top = top->parent;
                }
                top->field_0xf0 = 0;
                for (child = top->child; child != NULL; child = child->next) {
                    SetFieldF0Tree(child, 0);
                }
                if (pWorld_callbacks != NULL && pWorld_callbacks->activate_passive != NULL) {
                    pWorld_callbacks->activate_passive(top, NULL);
                }
                if ((top->child != NULL || top->physics_joint2 != NULL) && top->field_0x218 == 0) {
                    MoveJointedObject(top CARPOCALYPSE2_THISCALL_EDX, 0.0f);

                }
            }
        }
        obj = obj->field183_0x1d8;
    } while (obj != pObject);

    if (count == 0) {
        return 0;
    }
    if (count > 0) {
        for (i = 0; i < count; i++) {
            rec = &contacts[i];
            orig = rec->object1;
            while ((rec->object1->flags & 0x400) != 0) {
                rec->object1 = rec->object1->parent;
            }
            if (orig != rec->object1) {
                rec->r1.v[0] = orig->cmpos.v[0] + rec->r1.v[0];
                rec->r1.v[1] = orig->cmpos.v[1] + rec->r1.v[1];
                rec->r1.v[2] = orig->cmpos.v[2] + rec->r1.v[2];
                BrMatrix34ApplyP(&tmp, &rec->r1, &orig->transform_matrix);
                DrMatrix34ApplyLPInverse(&rec->r1, &tmp, &rec->object1->transform_matrix);
                rec->r1.v[0] = rec->r1.v[0] - rec->object1->cmpos.v[0];
                rec->r1.v[1] = rec->r1.v[1] - rec->object1->cmpos.v[1];
                rec->r1.v[2] = rec->r1.v[2] - rec->object1->cmpos.v[2];
                BrMatrix34ApplyV(&tmp, &rec->normal, &orig->transform_matrix);
                BrMatrix34TApplyV(&rec->normal, &tmp, &rec->object1->transform_matrix);
            }
            orig = rec->object2;
            if (orig != NULL) {
                while ((rec->object2->flags & 0x400) != 0) {
                    rec->object2 = rec->object2->parent;
                }
                if (orig != rec->object1) {
                    rec->r2.v[0] = orig->cmpos.v[0] + rec->r2.v[0];
                    rec->r2.v[1] = orig->cmpos.v[1] + rec->r2.v[1];
                    rec->r2.v[2] = orig->cmpos.v[2] + rec->r2.v[2];
                    BrMatrix34ApplyP(&tmp, &rec->r2, &orig->transform_matrix);
                    DrMatrix34ApplyLPInverse(&rec->r2, &tmp, &rec->object2->transform_matrix);
                    rec->r2.v[0] = rec->r2.v[0] - rec->object2->cmpos.v[0];
                    rec->r2.v[1] = rec->r2.v[1] - rec->object2->cmpos.v[1];
                    rec->r2.v[2] = rec->r2.v[2] - rec->object2->cmpos.v[2];
                    BrMatrix34ApplyV(&tmp, &rec->field_0x38, &orig->transform_matrix);
                    BrMatrix34TApplyV(&rec->field_0x38, &tmp, &rec->object2->transform_matrix);
                }
            }
        }
    }

    contact_count = count;
    total = CopyPhysicsJointData((tPhysics_joint_data*)pBuffer1, count, (const tPhysics_joint_data*)pBuffer2, pArg);
    joint_rows = 0;
    group_count = 0;
    obj = pObject;
    do {
        group = (tPhysics_group_data*)(uintptr_t)obj->field_0x218;
        if (group != NULL) {
            groups[group_count] = group;
            group_count++;
            p = group->data;
            for (k = 0; k < group->count; k++) {
                rows[joint_rows] = p;
                joint_rows++;
                p++;
            }
        }
        obj = obj->field183_0x1d8;
    } while (obj != pObject);
    if (total > 0) {
        for (i = 0; i < total; i++) {
            rows[joint_rows + i] = &contacts[i];
        }
    }
    if (joint_rows + total > 100) {
        total = 100 - joint_rows;
    }
    if (contact_count > total) {
        contact_count = total;
    }
    nrows = joint_rows + total;
    a_c = solve_a + joint_rows;
    b_c = solve_b + joint_rows;
    rows_c = rows + joint_rows;
    FUN_004bf490((int*)gPhysics_buffer3_ptr, 100, (int*)rows, nrows);

    obj = pObject;
    do {
        obj->omega.v[0] = obj->rotate_omega.v[0];
        obj->omega.v[1] = obj->rotate_omega.v[1];
        obj->omega.v[2] = obj->rotate_omega.v[2];
        for (child = obj->child; child != NULL; child = child->next) {
            child->omega.v[0] = child->rotate_omega.v[0];
            child->omega.v[1] = child->rotate_omega.v[1];
            child->omega.v[2] = child->rotate_omega.v[2];
            if (child->child != NULL) {
                ResetOmegaTree(child->child);
            }
        }
        obj = obj->field183_0x1d8;
    } while (obj != pObject);

    any = 0;
    solve_result = 0;
    if (gINT_0065d014 != 0) {
        solve_result = FUN_004c0430(b_c, rows_c, total, contact_count);
    } else {
        if (contact_count < total) {
            for (i = contact_count; i < total; i++) {
                b_c[i] = 0.0f;
            }
        }
        if (contact_count > 0) {
            for (i = 0; i < contact_count; i++) {
                rec = rows_c[i];
                o1 = rec->object1;
                if (rec->type == 0xb) {
                    value = -(o1->omega.v[2] * rec->normal.v[2] + rec->normal.v[1] * o1->omega.v[1] + rec->normal.v[0] * o1->omega.v[0]);
                    b_c[i] = value;
                    o2 = rec->object2;
                    if (o2 != NULL) {
                        b_c[i] = o2->omega.v[0] * rec->field_0x38.v[0] + rec->field_0x38.v[1] * o2->omega.v[1] + rec->field_0x38.v[2] * o2->omega.v[2] + b_c[i];
                    }
                    if (b_c[i] < gDOUBLE_0058a7a0) {
                        b_c[i] = 0.001f;
                    } else {
                        any = 1;
                    }
                } else {
                    cross.v[0] = o1->omega.v[1] * rec->r1.v[2] - rec->r1.v[1] * o1->omega.v[2];
                    cross.v[1] = o1->omega.v[2] * rec->r1.v[0] - rec->r1.v[2] * o1->omega.v[0];
                    cross.v[2] = o1->omega.v[0] * rec->r1.v[1] - rec->r1.v[0] * o1->omega.v[1];
                    cross.v[0] = o1->velocity_car_space.v[0] + cross.v[0];
                    cross.v[1] = o1->velocity_car_space.v[1] + cross.v[1];
                    cross.v[2] = o1->velocity_car_space.v[2] + cross.v[2];
                    b_c[i] = -(rec->normal.v[0] * cross.v[0] + rec->normal.v[2] * cross.v[2] + rec->normal.v[1] * cross.v[1]);
                    o2 = rec->object2;
                    if (o2 != NULL) {
                        cross.v[0] = o2->omega.v[1] * rec->r2.v[2] - rec->r2.v[1] * o2->omega.v[2];
                        cross.v[1] = o2->omega.v[2] * rec->r2.v[0] - rec->r2.v[2] * o2->omega.v[0];
                        cross.v[2] = o2->omega.v[0] * rec->r2.v[1] - rec->r2.v[0] * o2->omega.v[1];
                        cross.v[0] = o2->velocity_car_space.v[0] + cross.v[0];
                        cross.v[1] = o2->velocity_car_space.v[1] + cross.v[1];
                        cross.v[2] = o2->velocity_car_space.v[2] + cross.v[2];
                        b_c[i] = rec->field_0x38.v[2] * cross.v[2] + rec->field_0x38.v[1] * cross.v[1] + rec->field_0x38.v[0] * cross.v[0] + b_c[i];
                    }
                    if (b_c[i] < gFLOAT_0058a768) {
                        b_c[i] = 0.0f;
                    } else if (b_c[i] > gDOUBLE_0058a798) {
                        any = 1;
                    }
                    b_c[i] = gDOUBLE_0058a798 - b_c[i] * gDOUBLE_0058a7d8;
                }
            }
        }
        if (any) {
            solve_result = 0;
        } else {
            if (contact_count > 0) {
                for (i = 0; i < contact_count; i++) {
                    rec = rows_c[i];
                    if (rec->type == 0xb) {
                        b_c[i] = (gDOUBLE_0058a7a0 - rec->impulse) * gDOUBLE_0058a7e0;
                    } else {
                        o1 = rec->object1;
                        o2 = rec->object2;
                        if (o2 != NULL) {
                            tmp.v[0] = o1->cmpos.v[0] + rec->r1.v[0];
                            tmp.v[1] = o1->cmpos.v[1] + rec->r1.v[1];
                            tmp.v[2] = o1->cmpos.v[2] + rec->r1.v[2];
                            BrMatrix34ApplyP(&cross, &tmp, &o1->transform_matrix);
                            DrMatrix34ApplyLPInverse(&tmp2, &cross, &o2->transform_matrix);
                            BrMatrix34ApplyP(&cross, &tmp, &o1->actor->t.t.mat);
                            DrMatrix34ApplyLPInverse(&torque, &cross, &o2->actor->t.t.mat);
                            tmp2.v[0] = torque.v[0] - tmp2.v[0];
                            tmp2.v[1] = torque.v[1] - tmp2.v[1];
                            tmp2.v[2] = torque.v[2] - tmp2.v[2];
                            if (rec->type == 5) {
                                BrMatrix34ApplyV(&cross, &rec->field_0x38, &o2->transform_matrix);
                                BrMatrix34TApplyV(&torque, &cross, &o2->actor->t.t.mat);
                                b_c[i] = -((torque.v[0] * tmp2.v[0] + torque.v[1] * tmp2.v[1] + torque.v[2] * tmp2.v[2]) * gFLOAT_0058a7c8);
                            } else {
                                b_c[i] = -((rec->field_0x38.v[0] * tmp2.v[0] + rec->field_0x38.v[1] * tmp2.v[1] + rec->field_0x38.v[2] * tmp2.v[2]) * gFLOAT_0058a7c8);
                            }
                        } else {
                            tmp.v[0] = o1->cmpos.v[0] + rec->r1.v[0];
                            tmp.v[1] = o1->cmpos.v[1] + rec->r1.v[1];
                            tmp.v[2] = o1->cmpos.v[2] + rec->r1.v[2];
                            BrMatrix34ApplyP(&cross, &tmp, &o1->actor->t.t.mat);
                            DrMatrix34ApplyLPInverse(&tmp2, &cross, &o1->transform_matrix);
                            tmp2.v[0] = tmp2.v[0] - tmp.v[0];
                            tmp2.v[1] = tmp2.v[1] - tmp.v[1];
                            tmp2.v[2] = tmp2.v[2] - tmp.v[2];
                            b_c[i] = gDOUBLE_0058a798 - (rec->normal.v[0] * tmp2.v[0] + rec->normal.v[2] * tmp2.v[2] + rec->normal.v[1] * tmp2.v[1]) * gFLOAT_0058a7c8;
                        }
                    }
                    if (b_c[i] < gDOUBLE_0058a798) {
                        b_c[i] = 0.01f;
                    } else {
                        any = 1;
                    }
                }
            }
            if (!any && contact_count > 0) {
                for (i = 0; i < contact_count; i++) {
                    if (rows_c[i]->type != 0xb) {
                        b_c[i] = 0.05f;
                    }
                }
            }
            solve_result = 1;
        }
    }

    FUN_004b77b0((int*)solve_b, (int*)rows, joint_rows);
    if (FUN_004c3b60((float*)gPhysics_buffer3_ptr, solve_b, solve_a, nrows, 100, (int)(uintptr_t)solve_flags, (int*)solve_other, joint_rows, contact_count, (undefined4)(uintptr_t)groups, group_count) != 0) {
        gINT_006940a4++;
    }

    if (gINT_00692dd8 != 0) {
        changed = 0;
        obj = pObject;
        do {
            joint = obj->physics_joint2;
            if (joint != NULL && obj->field_0x1e0[0] != 0) {
                n = joint->count_limits;
                idx = 0;
                if (n > 0) {
                    do {
                        if (joint->limits[idx].type == 10) {
                            break;
                        }
                        idx++;
                    } while (idx < n);
                }
                if (idx != n) {
                    thr = joint->limits[idx].value;
                    BrVector3SetFloat(&torque, 0.0f, 0.0f, 0.0f);
                    if (total > 0) {
                        for (i = 0; i < total; i++) {
                            rec = &contacts[i];
                            lam = a_c[i];
                            if (rec->object1 == obj) {
                                dx = rec->r1.v[0] - joint->field_0x08.v[0];
                                dy = rec->r1.v[1] - joint->field_0x08.v[1];
                                dz = rec->r1.v[2] - joint->field_0x08.v[2];
                                cross.v[0] = lam * (rec->normal.v[2] * dy - rec->normal.v[1] * dz);
                                cross.v[1] = lam * (rec->normal.v[0] * dz - rec->normal.v[2] * dx);
                                cross.v[2] = lam * (rec->normal.v[1] * dx - rec->normal.v[0] * dy);
                                torque.v[1] = cross.v[1] + torque.v[1];
                                torque.v[0] = cross.v[0] + torque.v[0];
                                torque.v[2] = cross.v[2] + torque.v[2];
                            } else if (rec->object2 == obj) {
                                dx = rec->r2.v[0] - joint->field_0x08.v[0];
                                dy = rec->r2.v[1] - joint->field_0x08.v[1];
                                dz = rec->r2.v[2] - joint->field_0x08.v[2];
                                step = -lam;
                                cross.v[0] = (rec->field_0x38.v[2] * dy - rec->field_0x38.v[1] * dz) * step;
                                cross.v[1] = (rec->field_0x38.v[0] * dz - rec->field_0x38.v[2] * dx) * step;
                                cross.v[2] = (rec->field_0x38.v[1] * dx - rec->field_0x38.v[0] * dy) * step;
                                torque.v[1] = cross.v[1] + torque.v[1];
                                torque.v[0] = cross.v[0] + torque.v[0];
                                torque.v[2] = cross.v[2] + torque.v[2];
                            }
                        }
                    }
                    if (torque.v[2] * torque.v[2] + torque.v[1] * torque.v[1] + torque.v[0] * torque.v[0] <= thr) {
                        obj->disable_move_rotate = 1;
                    } else {
                        changed = 1;
                        obj->field_0x1e0[0] = 0;
                        if (total > 0) {
                            for (i = 0; i < total; i++) {
                                if (contacts[i].object1 == obj || contacts[i].object2 == obj) {
                                    contacts[i].type = 5;
                                }
                            }
                        }
                    }
                }
            }
            obj = obj->field183_0x1d8;
        } while (obj != pObject);
        if (changed) {
            FUN_004bf490((int*)gPhysics_buffer3_ptr, 100, (int*)rows, nrows);
            FUN_004c3b60((float*)gPhysics_buffer3_ptr, solve_b, solve_a, nrows, 100, (int)(uintptr_t)solve_flags, (int*)solve_other, joint_rows, contact_count, (undefined4)(uintptr_t)groups, group_count);
        }
    }

    max_value = gFLOAT_0058a768;
    for (i = 0; i < contact_count; i++) {
        if (max_value < a_c[i]) {
            max_value = a_c[i];
        }
    }
    for (i = 0; i < joint_rows; i++) {
        if (gFLOAT_0058a7e8 < solve_a[i]) {
            i++;
        }
    }
    for (i = 0; i < total; i++) {
        if (a_c[i] == gFLOAT_0058a768) {
            contacts[i].type = 9;
        }
    }
    gINT_00692dc8 = 0;
    if (pWorld_callbacks->process_forces != NULL) {
        pWorld_callbacks->process_forces(pBuffer1, a_c, total);
    }
    if (gINT_00692dc8 == 0) {
        if (pWorld_callbacks->process_joint_forces != NULL) {
            pWorld_callbacks->process_joint_forces((undefined4)(uintptr_t)rows, (undefined4)(uintptr_t)solve_a, joint_rows);
        }
        if (gINT_00692dc8 == 0) {
            for (i = 0; i < joint_rows; i++) {
                FUN_004b7960((tPhysics_joint_data*)rows[i], solve_a[i]);
            }
        }
    }
    for (i = 0; i < total; i++) {
        rec = &contacts[i];
        value = a_c[i];
        if (value != gFLOAT_0058a768 && rec->type != 9) {
            o2 = rec->object2;
            o1 = rec->object1;
            FUN_004b7960((tPhysics_joint_data*)rec, value);
            if (rec->type != 0xb) {
                if (o2 != NULL) {
                    if ((o1->collision_flag & 2) != 0 && (o2->collision_flag & 2) != 0) {
                        continue;
                    }
                } else if ((o1->collision_flag & 1) != 0) {
                    continue;
                }
                if (gINT_00692ddc < 0x32) {
                    gCollision_contacts[gINT_00692ddc] = *rec;
                    gCollision_contacts[gINT_00692ddc].impulse = a_c[i];
                    gINT_00692ddc++;
                }
            }
        }
    }
    if (solve_result != 0 && gINT_00692dc8 == 0) {
        FUN_004bc190((int*)rows, solve_a, joint_rows, (int)(uintptr_t)pWorld_callbacks);
    }

    ring_length = 0;
    obj = pObject;

    do {
        ring_length++;
        if (ring_length > 100) {
            PhysicsError((tPhysicsError)10, "Corrupt group");
        }
        obj = obj->field183_0x1d8;
    } while (obj != pObject);

    sum_all = 0;
    obj = pObject;
    do {
        sum_all += ClampObjectSpin(obj);
        child = obj->child;
        if (child != NULL) {
            sum_children = 0;
            for (; child != NULL; child = child->next) {
                sum_children += ClampObjectSpin(child);
                if (child->child != NULL) {
                    sum_children += ClampObjectSpinTree(child->child);
                }
            }
            sum_all += sum_children;
        }
        obj = obj->field183_0x1d8;
    } while (obj != pObject);

    if (sum_all != 0) {
        obj = pObject;
        do {
            group = (tPhysics_group_data*)(uintptr_t)obj->field_0x218;
            if (group != NULL) {
                p = group->data;
                for (k = 0; k < group->count; k++) {
                    group_rows[k] = p;
                    p++;
                }
                FUN_004b77b0((int*)group_b, (int*)group_rows, group->count);
                RqBackSub(group->field_0x18, group->field_0x14, group_b, group_a, group->count);
                for (k = 0; k < group->count; k++) {
                    FUN_004b7960((tPhysics_joint_data*)group_rows[k], group_a[k]);
                }
            }
            obj = obj->field183_0x1d8;
        } while (obj != pObject);
    }

    flags_acc = 0;
    obj = pObject;
    do {
        if (obj->disable_move_rotate == 0) {
            if (obj->field_0xf0 == 1) {
                BrMatrix34Copy(&obj->actor->t.t.mat, (const br_matrix34*)&obj->field_0xbc);
            } else {
                BrMatrix34LPNormalise(&obj->actor->t.t.mat, &obj->transform_matrix);
            }
            for (child = obj->child; child != NULL; child = child->next) {
                ResetObjectAndChildren(child);
            }
            FUN_004b8ee0(obj CARPOCALYPSE2_THISCALL_EDX, 0.04f);
            MergeBoundsIntoParent(obj, NULL);
            if (obj->field_0x194 != 0) {
                flags_acc |= ((int (C2_HOOK_FASTCALL *)(tPhysics_object*))(uintptr_t)obj->field_0x194)(obj);
            }
        }
        obj = obj->field183_0x1d8;
    } while (obj != pObject);
    if ((flags_acc | sum_all) != 0) {
        return -total;
    }
    return total;
#endif
}

// FUNCTION: CARMA2_HW 0x004bd9b0
void C2_HOOK_FASTCALL SetFieldF0Tree(tPhysics_object* pObject, int pValue) {
    tPhysics_object* child;

    pObject->field_0xf0 = pValue;
    for (child = pObject->child; child != NULL; child = child->next) {
        SetFieldF0Tree(child, pValue);
    }
}

// FUNCTION: CARMA2_HW 0x004c03f0
void C2_HOOK_FASTCALL ResetOmegaTree(tPhysics_object* pObject) {
    tPhysics_object* obj;

    obj = pObject;
    while (pObject != NULL) {
        pObject->omega.v[0] = pObject->rotate_omega.v[0];
        pObject->omega.v[1] = pObject->rotate_omega.v[1];
        pObject->omega.v[2] = pObject->rotate_omega.v[2];
        if (obj->child != NULL) {
            ResetOmegaTree(obj->child);
        }
        pObject = pObject->next;
        obj = pObject;
    }
}

// C2PORT-NONMATCHING: best 23.40% | difftest PASS 2000 trials (spec difftest/specs/004c0430_FUN_004c0430.json) | reason vc5-register-allocation-and-stack-layout
// FUNCTION: CARMA2_HW 0x004c0430
int C2_HOOK_FASTCALL FUN_004c0430(float* pFactors, void* pContact_list, int pCapacity, int pCount) {
    tCollision_contact** contacts;
    tCollision_contact* contact;
    tPhysics_object* object1;
    tPhysics_object* object2;
    br_vector3 velocity;
    br_vector3 point;
    br_vector3 world_point;
    br_vector3 local_point1;
    br_vector3 local_point2;
    br_vector3 separation;
    br_vector3 transformed_normal;
    br_vector3 normal_world;
    float value;
    float scale;
    float alternate_value;
    int has_bad_contact;
    int has_other_contact;
    int i;

    contacts = (tCollision_contact**)pContact_list;
    has_bad_contact = 0;
    if (pCount < pCapacity) {
        for (i = pCount; i < pCapacity; i++) {
            pFactors[i] = 0.0f;
        }
    }
    for (i = 0; i < pCount; i++) {
        contact = contacts[i];
        object1 = contact->object1;
        object2 = contact->object2;
        if (contact->type == 0xb) {
            value = -(object1->omega.v[2] * contact->normal.v[2]
                + contact->normal.v[1] * object1->omega.v[1]
                + contact->normal.v[0] * object1->omega.v[0]);
            if (object2 != NULL) {
                value += object2->omega.v[1] * contact->field_0x38.v[1]
                    + contact->field_0x38.v[0] * object2->omega.v[0]
                    + contact->field_0x38.v[2] * object2->omega.v[2];
            }
            pFactors[i] = value;
            if (gDOUBLE_0058a7a0 <= value) {
                has_bad_contact = 1;
            } else {
                pFactors[i] = 0.001f;
            }
        } else {
            velocity.v[2] = object1->velocity_car_space.v[2]
                + object1->omega.v[0] * contact->r1.v[1] - contact->r1.v[0] * object1->omega.v[1];
            velocity.v[1] = object1->velocity_car_space.v[1]
                + object1->omega.v[2] * contact->r1.v[0] - contact->r1.v[2] * object1->omega.v[0];
            velocity.v[0] = object1->velocity_car_space.v[0]
                + object1->omega.v[1] * contact->r1.v[2] - contact->r1.v[1] * object1->omega.v[2];
            value = -(contact->normal.v[2] * velocity.v[2] + contact->normal.v[1] * velocity.v[1]
                + contact->normal.v[0] * velocity.v[0]);
            if (object2 != NULL) {
                velocity.v[1] = object2->velocity_car_space.v[1]
                    + object2->omega.v[2] * contact->r2.v[0] - contact->r2.v[2] * object2->omega.v[0];
                velocity.v[0] = object2->velocity_car_space.v[0]
                    + object2->omega.v[1] * contact->r2.v[2] - contact->r2.v[1] * object2->omega.v[2];
                velocity.v[2] = object2->velocity_car_space.v[2]
                    + object2->omega.v[0] * contact->r2.v[1] - contact->r2.v[0] * object2->omega.v[1];
                value += contact->field_0x38.v[1] * velocity.v[1]
                    + contact->field_0x38.v[0] * velocity.v[0]
                    + contact->field_0x38.v[2] * velocity.v[2];
            }
            if (gDOUBLE_0058a798 < value) {
                has_bad_contact = 1;
            }
            if (gDOUBLE_0058a830 < value) {
                scale = (float)gDOUBLE_0058a830 - (contact->impulse - gFLOAT_0058a800);
                if (gFLOAT_0058a800 < scale) {
                    scale = gFLOAT_0058a800;
                }
                if (scale < (float)gDOUBLE_0058a830) {
                    scale = gFLOAT_0058a804;
                }
                value = scale * (value - (float)gDOUBLE_0058a830) - (float)gDOUBLE_0058a838;
            }
            if (value < gFLOAT_0058a768) {
                value = gFLOAT_0058a768;
            }
            value = (float)gDOUBLE_0058a798 - value * (float)gDOUBLE_0058a7d8;
            if ((object2 == NULL || object2->field_0x1df != 0) && object1->field_0x1df != 0) {
                value -= (float)gDOUBLE_0058a840;
            }
            pFactors[i] = value;
        }
    }
    if (!has_bad_contact) {
        has_other_contact = 0;
        for (i = 0; i < pCount; i++) {
            contact = contacts[i];
            object1 = contact->object1;
            object2 = contact->object2;
            if (contact->type == 0xb) {
                value = ((float)gDOUBLE_0058a7a0 - contact->impulse) * (float)gDOUBLE_0058a7e0;
                pFactors[i] = value;
                if ((float)gDOUBLE_0058a7a0 <= value) {
                    has_bad_contact = 1;
                } else {
                    pFactors[i] = 0.001f;
                }
            } else {
                if (object2 == NULL) {
                    point.v[0] = contact->r1.v[0] + object1->cmpos.v[0];
                    point.v[1] = contact->r1.v[1] + object1->cmpos.v[1];
                    point.v[2] = contact->r1.v[2] + object1->cmpos.v[2];
                    BrMatrix34ApplyP(&world_point, &point, &object1->actor->t.t.mat);
                    DrMatrix34ApplyLPInverse(&local_point1, &world_point, &object1->transform_matrix);
                    separation.v[0] = local_point1.v[0] - point.v[0];
                    separation.v[1] = local_point1.v[1] - point.v[1];
                    separation.v[2] = local_point1.v[2] - point.v[2];
                    pFactors[i] = (float)gDOUBLE_0058a798
                        - (contact->normal.v[0] * separation.v[0] + contact->normal.v[2] * separation.v[2]
                            + contact->normal.v[1] * separation.v[1]) * gFLOAT_0058a7c8;
                } else {
                    point.v[0] = contact->r1.v[0] + object1->cmpos.v[0];
                    point.v[1] = contact->r1.v[1] + object1->cmpos.v[1];
                    point.v[2] = contact->r1.v[2] + object1->cmpos.v[2];
                    BrMatrix34ApplyP(&world_point, &point, &object1->transform_matrix);
                    DrMatrix34ApplyLPInverse(&local_point1, &world_point, &object2->transform_matrix);
                    BrMatrix34ApplyP(&world_point, &point, &object1->actor->t.t.mat);
                    DrMatrix34ApplyLPInverse(&local_point2, &world_point, &object2->actor->t.t.mat);
                    separation.v[0] = local_point2.v[0] - local_point1.v[0];
                    separation.v[1] = local_point2.v[1] - local_point1.v[1];
                    separation.v[2] = local_point2.v[2] - local_point1.v[2];
                    pFactors[i] = -(contact->field_0x38.v[2] * separation.v[2]
                        + contact->field_0x38.v[1] * separation.v[1]
                        + contact->field_0x38.v[0] * separation.v[0]) * gFLOAT_0058a7c8;
                    if (contact->type == 5) {
                        BrMatrix34ApplyV(&transformed_normal, &contact->field_0x38, &object2->transform_matrix);
                        BrMatrix34TApplyV(&normal_world, &transformed_normal, &object2->actor->t.t.mat);
                        alternate_value = -(normal_world.v[0] * separation.v[0]
                            + normal_world.v[1] * separation.v[1] + normal_world.v[2] * separation.v[2]) * gFLOAT_0058a7c8;
                        if (pFactors[i] < alternate_value) {
                            pFactors[i] = alternate_value;
                        }
                    }
                    if (contact->field_0x10 > gFLOAT_0058a768) {
                        alternate_value = contact->field_0x10 * gFLOAT_0058a7c8;
                        if (pFactors[i] < alternate_value) {
                            pFactors[i] = alternate_value;
                        }
                    }
                }
                if (pFactors[i] <= (float)gDOUBLE_0058a830) {
                    if (pFactors[i] < gFLOAT_0058a768) {
                        pFactors[i] = gFLOAT_0058a768;
                    }
                } else {
                    scale = (float)gDOUBLE_0058a830 - (contact->impulse - gFLOAT_0058a800);
                    if (gFLOAT_0058a800 < scale) {
                        scale = gFLOAT_0058a800;
                    }
                    if (scale < (float)gDOUBLE_0058a830) {
                        scale = gFLOAT_0058a804;
                    }
                    pFactors[i] = scale * (pFactors[i] - (float)gDOUBLE_0058a830)
                        - (float)gDOUBLE_0058a838;
                }
                if ((float)gDOUBLE_0058a7a0 < pFactors[i]) {
                    has_other_contact = 1;
                }
                pFactors[i] = (float)gDOUBLE_0058a798 - pFactors[i] * (float)gDOUBLE_0058a7d8;
            }
        }
        if (!has_other_contact) {
            for (i = 0; i < pCount; i++) {
                if (contacts[i]->type != 0xb) {
                    pFactors[i] = 0.05f;
                }
            }
        }
    }
    return !has_bad_contact;
}

#pragma auto_inline(off)
#pragma inline_depth(0)
// FUNCTION: CARMA2_HW 0x004c0ac0
void C2_HOOK_FASTCALL RotateObjectFirstOrder(void* pContact, tWorld_callbacks* pWorld_callbacks) {
    tCollision_contact* contact;
    tPhysics_object* obj1;
    tPhysics_object* obj2;
    br_vector3 tmp;
    br_vector3 rel;
    br_vector3 nw;
    br_vector3 tan;
    br_vector3 t1;
    br_vector3 t2;
    br_vector3 dir;
    br_scalar dot;
    br_scalar speed;
    br_scalar max;
    br_scalar inv;
    br_scalar J;
    br_scalar denom;
    br_scalar m1inv;
    br_scalar m2inv;
    br_scalar ix;
    br_scalar iy;
    br_scalar iz;
    br_scalar jx;
    br_scalar jy;
    br_scalar jz;

    contact = (tCollision_contact*)pContact;
    obj1 = contact->object1;
    obj2 = contact->object2;
    if (obj2 != NULL && contact->type != 0xe) {
        br_scalar         imp0 = contact->impulse;
        obj1->collision_flag |= 2;
        obj2->collision_flag |= 2;
        t1.v[0] = (obj1->omega.v[1] * contact->r1.v[2] - obj1->omega.v[2] * contact->r1.v[1]) + obj1->velocity_car_space.v[0];
        t1.v[1] = (obj1->omega.v[2] * contact->r1.v[0] - obj1->omega.v[0] * contact->r1.v[2]) + obj1->velocity_car_space.v[1];
        t1.v[2] = (obj1->omega.v[0] * contact->r1.v[1] - obj1->omega.v[1] * contact->r1.v[0]) + obj1->velocity_car_space.v[2];
        t2.v[0] = (obj2->omega.v[1] * contact->r2.v[2] - obj2->omega.v[2] * contact->r2.v[1]) + obj2->velocity_car_space.v[0];
        t2.v[1] = (obj2->omega.v[2] * contact->r2.v[0] - obj2->omega.v[0] * contact->r2.v[2]) + obj2->velocity_car_space.v[1];
        t2.v[2] = (obj2->omega.v[0] * contact->r2.v[1] - obj2->omega.v[1] * contact->r2.v[0]) + obj2->velocity_car_space.v[2];
        BrMatrix34ApplyV(&tmp, &t2, (br_matrix34*)&obj2->actor->t.t.mat);
        BrMatrix34TApplyV(&t2, &tmp, (br_matrix34*)&obj1->actor->t.t.mat);
        rel.v[0] = t1.v[0] - t2.v[0];
        rel.v[1] = t1.v[1] - t2.v[1];
        rel.v[2] = t1.v[2] - t2.v[2];
        BrMatrix34ApplyV(&tmp, &contact->normal, &obj1->transform_matrix);
        BrMatrix34TApplyV(&nw, &tmp, (br_matrix34*)&obj1->actor->t.t.mat);
        dot = nw.v[0] * rel.v[0] + nw.v[1] * rel.v[1] + nw.v[2] * rel.v[2];
        tan.v[0] = rel.v[0] - nw.v[0] * dot;
        tan.v[1] = rel.v[1] - nw.v[1] * dot;
        tan.v[2] = rel.v[2] - nw.v[2] * dot;
        speed = sqrtf(tan.v[0] * tan.v[0] + tan.v[1] * tan.v[1] + tan.v[2] * tan.v[2]);
        max = imp0 * 0.35f;
        max = obj1->object_friction * max + max;
        max = obj2->object_friction * max + max;
        if (speed < 0.001f) {
            CapOmega(obj1);
            CapOmega(obj2);
            return;
        }
        inv = 1.0f / -speed;
        dir.v[0] = tan.v[0] * inv;
        dir.v[1] = tan.v[1] * inv;
        dir.v[2] = tan.v[2] * inv;
        BrMatrix34ApplyV(&tmp, &dir, (br_matrix34*)&obj1->actor->t.t.mat);
        BrMatrix34TApplyV(&dir, &tmp, (br_matrix34*)&obj2->actor->t.t.mat);
        if ((obj1->flags & 0x800) == 0) {
            ix = (contact->r1.v[1] * dir.v[2] - contact->r1.v[2] * dir.v[1]) / obj1->I.v[0];
            iy = (contact->r1.v[2] * dir.v[0] - contact->r1.v[0] * dir.v[2]) / obj1->I.v[1];
            iz = (contact->r1.v[0] * dir.v[1] - contact->r1.v[1] * dir.v[0]) / obj1->I.v[2];
        } else {
            ix = 0.0f;
            iy = 0.0f;
            iz = 0.0f;
        }
        if ((obj2->flags & 0x800) == 0) {
            jx = (contact->r2.v[1] * dir.v[2] - contact->r2.v[2] * dir.v[1]) / obj2->I.v[0];
            jy = (contact->r2.v[2] * dir.v[0] - contact->r2.v[0] * dir.v[2]) / obj2->I.v[1];
            jz = (contact->r2.v[0] * dir.v[1] - contact->r2.v[1] * dir.v[0]) / obj2->I.v[2];
        } else {
            jx = 0.0f;
            jy = 0.0f;
            jz = 0.0f;
        }
        m1inv = 1.0f / obj1->M;
        m2inv = 1.0f / obj2->M;
        denom = (contact->r1.v[1] * dir.v[2] - contact->r1.v[2] * dir.v[1]) * dir.v[0]
              + (contact->r1.v[2] * dir.v[0] - contact->r1.v[0] * dir.v[2]) * dir.v[1]
              + (contact->r1.v[0] * dir.v[1] - contact->r1.v[1] * dir.v[0]) * dir.v[2]
              + m1inv + m2inv;
        if (fabsf(denom) <= 0.0001f) {
            CapOmega(obj1);
            CapOmega(obj2);
            return;
        }
        J = speed / denom;
        if (J > max) {
            J = max;
        }
        contact->impulse1.v[0] = dir.v[0] * J;
        contact->impulse1.v[1] = dir.v[1] * J;
        contact->impulse1.v[2] = dir.v[2] * J;
        contact->impulse2.v[0] = dir.v[0] * -J;
        contact->impulse2.v[1] = dir.v[1] * -J;
        contact->impulse2.v[2] = dir.v[2] * -J;
        contact->type |= 0x100;
        obj1->omega.v[0] += J * ix;
        obj1->omega.v[1] += J * iy;
        obj1->omega.v[2] += J * iz;
        obj2->omega.v[0] += J * jx;
        obj2->omega.v[1] += J * jy;
        obj2->omega.v[2] += J * jz;
        BrMatrix34ApplyV(&tmp, &dir, (br_matrix34*)&obj1->actor->t.t.mat);
        obj1->v.v[0] += tmp.v[0] * J;
        obj1->v.v[1] += tmp.v[1] * J;
        obj1->v.v[2] += tmp.v[2] * J;
        obj1->velocity_car_space.v[0] += tmp.v[0] * J;
        obj1->velocity_car_space.v[1] += tmp.v[1] * J;
        obj1->velocity_car_space.v[2] += tmp.v[2] * J;
        BrMatrix34ApplyV(&tmp, &dir, (br_matrix34*)&obj2->actor->t.t.mat);
        obj2->v.v[0] += tmp.v[0] * J;
        obj2->v.v[1] += tmp.v[1] * J;
        obj2->v.v[2] += tmp.v[2] * J;
        obj2->velocity_car_space.v[0] += tmp.v[0] * J;
        obj2->velocity_car_space.v[1] += tmp.v[1] * J;
        obj2->velocity_car_space.v[2] += tmp.v[2] * J;
        CapOmega(obj1);
        CapOmega(obj2);
        return;
    }
    {
        br_vector3 r1d;
        br_vector3 s1;
        br_vector3 stmp;
        br_vector3 srel;
        br_vector3 snw;
        br_vector3 stan;
        br_vector3 sdir;
        br_scalar sdot;
        br_scalar sspeed;
        br_scalar smax;
        br_scalar sinv;
        br_scalar sJ;
        br_scalar sdenom;
        br_scalar sm1inv;
        br_scalar six;
        br_scalar siy;
        br_scalar siz;
        br_scalar simp0;

        simp0 = contact->impulse;
        obj1->collision_flag |= 1;
        if (contact->type == 1) {
            r1d.v[0] = contact->r1.v[0] * -0.005f + contact->r1.v[0];
            r1d.v[1] = contact->r1.v[1] * -0.005f + contact->r1.v[1];
            r1d.v[2] = contact->r1.v[2] * -0.005f + contact->r1.v[2];
            s1.v[0] = (obj1->omega.v[1] * r1d.v[2] - obj1->omega.v[2] * r1d.v[1]) + obj1->velocity_car_space.v[0];
            s1.v[1] = (obj1->omega.v[2] * r1d.v[0] - obj1->omega.v[0] * r1d.v[2]) + obj1->velocity_car_space.v[1];
            s1.v[2] = (obj1->omega.v[0] * r1d.v[1] - obj1->omega.v[1] * r1d.v[0]) + obj1->velocity_car_space.v[2];
        } else {
            s1.v[0] = (obj1->omega.v[1] * contact->r1.v[2] - obj1->omega.v[2] * contact->r1.v[1]) + obj1->velocity_car_space.v[0];
            s1.v[1] = (obj1->omega.v[2] * contact->r1.v[0] - obj1->omega.v[0] * contact->r1.v[2]) + obj1->velocity_car_space.v[1];
            s1.v[2] = (obj1->omega.v[0] * contact->r1.v[1] - obj1->omega.v[1] * contact->r1.v[0]) + obj1->velocity_car_space.v[2];
        }
        BrMatrix34ApplyV(&stmp, &s1, &obj1->transform_matrix);
        BrMatrix34TApplyV(&s1, &stmp, (br_matrix34*)&obj1->actor->t.t.mat);
        BrMatrix34ApplyV(&stmp, &contact->normal, &obj1->transform_matrix);
        BrMatrix34TApplyV(&snw, &stmp, (br_matrix34*)&obj1->actor->t.t.mat);
        sdot = snw.v[0] * s1.v[0] + snw.v[1] * s1.v[1] + snw.v[2] * s1.v[2];
        stan.v[0] = s1.v[0] - snw.v[0] * sdot;
        stan.v[1] = s1.v[1] - snw.v[1] * sdot;
        stan.v[2] = s1.v[2] - snw.v[2] * sdot;
        sspeed = sqrtf(stan.v[0] * stan.v[0] + stan.v[1] * stan.v[1] + stan.v[2] * stan.v[2]);
        if (pWorld_callbacks != NULL && pWorld_callbacks->get_friction_from_face != NULL && contact->field_0xc != NULL) {
            smax = pWorld_callbacks->get_friction_from_face(contact->field_0xc) * simp0;
        } else {
            smax = simp0;
        }
        smax = smax * 0.35f;
        smax = obj1->world_friction * smax + smax;
        if (sspeed < 0.001f) {
            CapOmega(obj1);
            return;
        }
        sinv = 1.0f / -sspeed;
        sdir.v[0] = stan.v[0] * sinv;
        sdir.v[1] = stan.v[1] * sinv;
        sdir.v[2] = stan.v[2] * sinv;
        if ((obj1->flags & 0x800) == 0) {
            six = (contact->r1.v[1] * sdir.v[2] - contact->r1.v[2] * sdir.v[1]) / obj1->I.v[0];
            siy = (contact->r1.v[2] * sdir.v[0] - contact->r1.v[0] * sdir.v[2]) / obj1->I.v[1];
            siz = (contact->r1.v[0] * sdir.v[1] - contact->r1.v[1] * sdir.v[0]) / obj1->I.v[2];
        } else {
            six = 0.0f;
            siy = 0.0f;
            siz = 0.0f;
        }
        sm1inv = 1.0f / obj1->M;
        sdenom = (contact->r1.v[1] * sdir.v[2] - contact->r1.v[2] * sdir.v[1]) * sdir.v[0]
               + (contact->r1.v[2] * sdir.v[0] - contact->r1.v[0] * sdir.v[2]) * sdir.v[1]
               + (contact->r1.v[0] * sdir.v[1] - contact->r1.v[1] * sdir.v[0]) * sdir.v[2]
               + sm1inv;
        if (fabsf(sdenom) <= 0.0001f) {
            CapOmega(obj1);
            return;
        }
        sJ = sspeed / sdenom;
        if (sJ > smax) {
            sJ = smax;
        }
        contact->impulse1.v[0] = obj1->M * sdir.v[0] * sJ * sm1inv;
        contact->impulse1.v[1] = obj1->M * sdir.v[1] * sJ * sm1inv;
        contact->impulse1.v[2] = obj1->M * sdir.v[2] * sJ * sm1inv;
        contact->type |= 0x100;
        obj1->omega.v[0] += sJ * six;
        obj1->omega.v[1] += sJ * siy;
        obj1->omega.v[2] += sJ * siz;
        BrMatrix34ApplyV(&stmp, &sdir, (br_matrix34*)&obj1->actor->t.t.mat);
        obj1->v.v[0] += stmp.v[0] * sJ;
        obj1->v.v[1] += stmp.v[1] * sJ;
        obj1->v.v[2] += stmp.v[2] * sJ;
        obj1->velocity_car_space.v[0] += stmp.v[0] * sJ;
        obj1->velocity_car_space.v[1] += stmp.v[1] * sJ;
        obj1->velocity_car_space.v[2] += stmp.v[2] * sJ;
        CapOmega(obj1);
    }
}
#pragma inline_depth()
#pragma auto_inline(on)

// FUNCTION: CARMA2_HW 0x004c1840
void C2_HOOK_FASTCALL CapOmega(tPhysics_object* pObject) {
    br_scalar t;

    t = (pObject->omega.v[0] * pObject->omega.v[0] + pObject->omega.v[1] * pObject->omega.v[1] + pObject->omega.v[2] * pObject->omega.v[2]) * 0.0016f;
    if (!(t > 2.4649f)) {
        return;
    }
    t = sqrtf(t) * 0.6369f;
    t += t;
    pObject->omega.v[0] /= t;
    pObject->omega.v[1] /= t;
    pObject->omega.v[2] /= t;
}

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x004c18d0
void C2_HOOK_FASTCALL TranslateObject(tPhysics_object* pObject) {
    tPhysics_joint* joint;
    tPhysics_object* child;
    br_vector3 axis;
    br_vector3 tmp[3];
    br_scalar imp[3];
    br_scalar friction;
    br_scalar friction_sq;
    br_scalar sum_sq;
    br_scalar num;
    br_scalar denom;
    br_scalar scale;
    int i;
    int j;

    joint = pObject->physics_joint1;
    if (joint != NULL && joint->friction != 0.0f) {
        if (pObject->field_0x230 != NULL) {
            br_vector3* src = (br_vector3*)&pObject->transform_matrix.m[0][0];
            float* impPtr = imp;
            br_vector3* tmpPtr = tmp;
            float* iPtr = &pObject->I.v[0];
            float* oPtr = &pObject->omega.v[0];
            int k = 3;
            friction = joint->friction * 0.04f;
            sum_sq = 0.0f;
            friction_sq = friction * friction;
            do {
                float t;
                float* tp = &tmpPtr->v[0];
                float* ap = &axis.v[0];
                float* ip = &pObject->field_0x230->I.v[0];
                int n = 3;
                BrMatrix34TApplyV(&axis, src, &pObject->field_0x230->transform_matrix);
                num = pObject->field_0x230->omega.v[0] * axis.v[0] + pObject->field_0x230->omega.v[2] * axis.v[2] + pObject->field_0x230->omega.v[1] * axis.v[1] - *oPtr;
                do {
                    *tp = *ap / *ip;
                    tp++;
                    ap++;
                    ip++;
                    n--;
                } while (n != 0);
                denom = 1.0 / *iPtr + (tmpPtr->v[0] * axis.v[0] + tmpPtr->v[2] * axis.v[2] + tmpPtr->v[1] * axis.v[1]);
                t = num / denom;
                sum_sq += t * t;
                *impPtr = t;
                src++;
                impPtr++;
                tmpPtr++;
                iPtr++;
                oPtr++;
                k--;
            } while (k != 0);
            if (friction_sq < sum_sq) {
                scale = sqrtf(friction_sq / sum_sq);
                imp[0] *= scale;
                imp[1] *= scale;
                imp[2] *= scale;
            }
            for (i = 0; i < 3; i++) {
                pObject->omega.v[i] += imp[i] / pObject->I.v[i];
                pObject->field_0x230->omega.v[0] -= imp[i] * tmp[i].v[0];
                pObject->field_0x230->omega.v[1] -= imp[i] * tmp[i].v[1];
                pObject->field_0x230->omega.v[2] -= imp[i] * tmp[i].v[2];
            }
        }
    }

    joint = pObject->physics_joint2;
    if (joint != NULL && joint->friction != 0.0f) {
        float* b = &pObject->I.v[0];
        friction = joint->friction * 0.04f;
        friction_sq = friction * friction;
        sum_sq = 0.0f;
        for (i = 0; i < 3; i++) {
            float u = b[0] * b[27];
            sum_sq += u * u;
            imp[i] = u;
            b++;
        }
        if (sum_sq > friction_sq) {
            scale = sqrtf(friction_sq / sum_sq);
            imp[0] *= scale;
            imp[1] *= scale;
            imp[2] *= scale;
        }
        for (i = 0; i < 3; i++) {
            pObject->omega.v[i] -= imp[i] / pObject->I.v[i];
        }
    }

    for (child = pObject->child; child != NULL; child = child->next) {
        if (child->disable_move_rotate == 0) {
            TranslateObject(child);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004c1be0
void C2_HOOK_FAKE_THISCALL RotateObject(tPhysics_object* pObject, undefined4 pArg2, float pDelta_time) {
    union {
        br_vector3 axis;
        br_vector3 omega;
    } axis_omega;
    br_vector3 angular_momentum;
    br_vector3 rotated_angular_momentum;
    br_matrix34 rotation;
    br_vector3 translation;
    tU16 packed_rotation[4];
    br_scalar rad_rate;
    br_scalar rad;
    br_scalar rad_squared;
    int steps;
    int i;

    pObject->rotate_omega.v[0] = pObject->omega.v[0];
    rad_squared = pDelta_time * pDelta_time * BrVector3LengthSquared(&pObject->omega);
    pObject->rotate_omega.v[1] = pObject->omega.v[1];
    pObject->rotate_omega.v[2] = pObject->omega.v[2];
    if (rad_squared >= gDOUBLE_0058a860) {
        if (rad_squared > gFLOAT_0058a868) {
            steps = (int)sqrtf(rad_squared * gFLOAT_0058a86c) + 1;
            pDelta_time /= steps;
            for (i = 0; i < steps && i < 400; i++) {
                rad_rate = BrVector3Length(&pObject->omega);
                axis_omega.axis.v[0] = pObject->omega.v[0] / rad_rate;
                axis_omega.axis.v[1] = pObject->omega.v[1] / rad_rate;
                axis_omega.axis.v[2] = pObject->omega.v[2] / rad_rate;
                angular_momentum.v[0] = pObject->I.v[0] * pObject->omega.v[0];
                angular_momentum.v[1] = pObject->I.v[1] * pObject->omega.v[1];
                angular_momentum.v[2] = pObject->I.v[2] * pObject->omega.v[2];
                rad = rad_rate * pDelta_time;
                BrMatrix34Rotate(&rotation, (br_angle)(((br_angle)(rad * gDOUBLE_0058a770)) / 2), &axis_omega.axis);
                BrMatrix34TApplyV(&rotated_angular_momentum, &angular_momentum, &rotation);
                axis_omega.omega.v[0] = rotated_angular_momentum.v[0] / pObject->I.v[0];
                axis_omega.omega.v[1] = rotated_angular_momentum.v[1] / pObject->I.v[1];
                axis_omega.omega.v[2] = rotated_angular_momentum.v[2] / pObject->I.v[2];

                rad_rate = BrVector3Length(&axis_omega.omega);
                axis_omega.axis.v[0] = axis_omega.omega.v[0] / rad_rate;
                axis_omega.axis.v[1] = axis_omega.omega.v[1] / rad_rate;
                axis_omega.axis.v[2] = axis_omega.omega.v[2] / rad_rate;
                rad = rad_rate * pDelta_time;
                BrMatrix34Rotate(&rotation, (br_angle)(rad * gDOUBLE_0058a770), &axis_omega.axis);
                BrMatrix34PreTranslate(&rotation, -pObject->cmpos.v[0], -pObject->cmpos.v[1], -pObject->cmpos.v[2]);
                BrMatrix34PostTranslate(&rotation, pObject->cmpos.v[0], pObject->cmpos.v[1], pObject->cmpos.v[2]);
                BrMatrix34Pre(&pObject->actor->t.t.mat, &rotation);
                BrMatrix34TApplyV(&rotated_angular_momentum, &angular_momentum, &rotation);
                pObject->omega.v[0] = rotated_angular_momentum.v[0] / pObject->I.v[0];
                pObject->omega.v[1] = rotated_angular_momentum.v[1] / pObject->I.v[1];
                pObject->omega.v[2] = rotated_angular_momentum.v[2] / pObject->I.v[2];
            }
        } else {
            RotateObjectFirstOrderRigid(pObject, *(undefined4*)&pDelta_time, pDelta_time);
        }
        if (gNet_mode != 0) {
            GetOrientationFromMatrix(packed_rotation, &pObject->actor->t.t.mat);
            translation.v[0] = pObject->actor->t.t.mat.m[3][0];
            translation.v[1] = pObject->actor->t.t.mat.m[3][1];
            translation.v[2] = pObject->actor->t.t.mat.m[3][2];
            GetMatrixFromOrientation(&pObject->actor->t.t.mat, packed_rotation);
            pObject->actor->t.t.mat.m[3][0] = translation.v[0];
            pObject->actor->t.t.mat.m[3][1] = translation.v[1];
            pObject->actor->t.t.mat.m[3][2] = translation.v[2];
        }
    }
}

#pragma auto_inline(on)
#pragma auto_inline(on)

// FUNCTION: CARMA2_HW 0x004ba6b0
void C2_HOOK_FASTCALL DoCollisions(tPhysics_object** pObject_list, tWorld_callbacks* pWorld_callbacks) {
    char* pBuf1;
    char* pBuf2;
    char* tmp;
    tPhysics_object* obj;
    tPhysics_object* head;
    tPhysics_object* tail;
    tPhysics_object* cur;
    tPhysics_object* next;
    tPhysics_object* pos;
    tPhysics_object* sibling;
    tPhysics_object* node;
    tCollision_contact* contact;
    tU32 marker;
    int mask;
    int loops;
    int i;
    int r;
    char bufB[0x28 * sizeof(tCollision_contact)];
    char bufA[0x28 * sizeof(tCollision_contact)];

    pBuf1 = bufA;
    pBuf2 = bufB;

    ClearPhysicsScratchSpace();

    gINT_006940a0 = 1;
    gINT_00692ddc = 0;
    gFLOAT_00692dd0 = 0.f;
    gFLOAT_00692dd4 = 0.f;

    for (obj = *pObject_list; obj != NULL; obj = obj->next) {
        InternalPrepareObject(obj);
    }

    head = *pObject_list;
    head->prev = NULL;
    tail = head;
    cur = head->next;
    while (cur != NULL) {
        pos = tail;
        while (pos != NULL && cur->transform_matrix.m[3][0] < pos->transform_matrix.m[3][0]) {
            pos = pos->prev;
        }
        if (pos == tail) {
            tail->next = cur;
            cur->prev = tail;
            tail = cur;
            cur = cur->next;
        } else if (pos != NULL) {
            next = cur->next;
            cur->next = pos->next;
            pos->next->prev = cur;
            pos->next = cur;
            cur->prev = pos;
            cur = next;
        } else {
            head->prev = cur;
            next = cur->next;
            cur->next = head;
            head = cur;
            cur->prev = NULL;
            cur = next;
        }
    }
    tail->next = NULL;
    *pObject_list = head;

    do {
        gPrepared_objects = 0;
        for (obj = *pObject_list; obj != NULL; obj = obj->next) {
            if (obj->field_0x1dc != 0) {
                continue;
            }
            if (obj->disable_move_rotate != 0) {
                continue;


            }
            if (obj->child != NULL && obj->child->physics_joint1 != NULL && obj->child->physics_joint1->type != 0) {
                marker = gINT_006940a0;
                obj->field_0x1dc = (tU8)marker;
                for (sibling = obj->child; sibling != NULL; sibling = sibling->next) {
                    if (sibling->physics_joint1 != NULL && sibling->physics_joint1->type != 0) {
                        DRMatrix33Inverse(sibling, marker);
                    }
                }
                gINT_006940a0++;
            }
            loops = 0;
            gINT_006940a4 = 0;
            r = SetUpQuickHingeData(pObject_list, obj, pBuf1, pBuf2, 0, pWorld_callbacks);
            while (r != 0) {
                loops++;
                tmp = pBuf1;
                pBuf1 = pBuf2;
                pBuf2 = tmp;
                if (loops >= 0x1e) {
                    break;
                }
                if (gINT_006940a4 >= 3) {
                    break;
                }
                r = SetUpQuickHingeData(pObject_list, obj, pBuf1, pBuf2, r, pWorld_callbacks);
            }
            if (loops >= 0x1e) {
                PhysicsWarning("Too many loops in DoCollisions");
            }
        }
    } while (gPrepared_objects != 0);

    /* APPROXIMATION: retail applies the solved contact impulses to the
     * velocities inside the 0x4bf490/0x4c3b60 solver passes; nothing else
     * applies the normal component, so do it here before friction. */
    ApplyContactImpulses(gCollision_contacts, gINT_00692ddc);

    for (i = 0, contact = gCollision_contacts; i < gINT_00692ddc; i++, contact++) {
        RotateObjectFirstOrder(contact, pWorld_callbacks);
    }

    AddDoubleTorqueToMatrix(*pObject_list);

    for (obj = *pObject_list; obj != NULL; obj = obj->next) {
        if (obj->disable_move_rotate != 0) {
            continue;
        }
        if (!(obj->field_0x1df & 0x2)) {
            continue;
        }
        mask = 0x2;
        for (node = obj->field183_0x1d8; node != obj; node = node->field183_0x1d8) {
            mask &= node->field_0x1df;
        }
        if (!(mask & 0x2)) {
            continue;
        }
        obj->disable_move_rotate = 1;
        for (cur = obj;; cur = cur->field183_0x1d8) {
            sibling = cur->child;
            cur->disable_move_rotate = 1;
            cur->field_0x1df = 0;
            if (sibling != NULL) {
                for (sibling = cur->child; sibling != NULL; sibling = sibling->next) {
                    sibling->disable_move_rotate = 1;
                    sibling->field_0x1df = 0;
                    if (sibling->child != NULL) {
                        SetCollisionInfoDoNothing(sibling->child, 1);
                    }
                }
            }
            if (cur->field183_0x1d8 == NULL || cur->field183_0x1d8 == obj) {
                break;
            }
        }
    }

    for (obj = *pObject_list; obj != NULL; obj = obj->next) {
        if (obj->disable_move_rotate == 0) {
            TranslateObject(obj);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004b61a0
int C2_HOOK_FASTCALL PHILSetPassiveObjectsMatrix(tPhysics_object* pObject, br_matrix34* pMatrix) {
    tPHIL_queued_object_info* queued;
    tPHIL_queued_header* info;

    if (gPHIL_enabled) {
        return 0;
    }
    if (pObject->field_0x239 == 2) {
        queued = pObject->field_0x240;
        queued->field_0x30 = *pMatrix;
        queued->flags |= 0x4;
        return 0;
    }
    info = pObject->field_0x240;
    if (info != NULL) {
        if (info->field_0x8 != 1) {
            return 4;
        }
        info->collision_info->actor->t.t.mat = *pMatrix;
        return 0;
    } else {
        return 3;
    }
}

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x004c2830
void C2_HOOK_FAKE_THISCALL RepositionObjectGroup(tPhysics_object* pObject, float pDt) {
    tPhysics_object* pChild;
    float t0;
    float t1;
    float t2;

    t0 = pObject->v.v[0] * pDt;
    t1 = pObject->v.v[1] * pDt;
    t2 = pObject->v.v[2] * pDt;
    pObject->actor->t.t.mat.m[3][0] += t0;
    pObject->actor->t.t.mat.m[3][1] += t1;
    pObject->actor->t.t.mat.m[3][2] += t2;
    InterpolateSingleObject(pObject, pDt);
    for (pChild = pObject->child; pChild != NULL; pChild = pChild->next) {
        RepositionObjectGroup(pChild, pDt);
    }
}
#pragma auto_inline(on)

// FUNCTION: CARMA2_HW 0x004c2670
void C2_HOOK_FAKE_THISCALL InterpolateSingleObject(tPhysics_object* pObject, float pDt) {
    br_vector3 temp;
    br_vector3 axis;
    float len;
    float angle;

    if (pObject->disable_move_rotate) {
        return;
    }

    len = BrVector3Length(&pObject->rotate_omega);
    axis.v[0] = pObject->rotate_omega.v[0] / len;
    axis.v[1] = pObject->rotate_omega.v[1] / len;
    axis.v[2] = pObject->rotate_omega.v[2] / len;
    angle = len * pDt;
    if (fabs(angle) < 0.0001) {
        return;
    }

    BrMatrix34ApplyV(&temp, &pObject->cmpos, &pObject->actor->t.t.mat);
    pObject->actor->t.t.mat.m[3][0] += temp.v[0];
    pObject->actor->t.t.mat.m[3][1] += temp.v[1];
    pObject->actor->t.t.mat.m[3][2] += temp.v[2];
    BrMatrix34PreRotate(&pObject->actor->t.t.mat, BrRadianToAngle(angle), &axis);
    BrMatrix34ApplyV(&temp, &pObject->cmpos, &pObject->actor->t.t.mat);
    pObject->actor->t.t.mat.m[3][0] -= temp.v[0];
    pObject->actor->t.t.mat.m[3][1] -= temp.v[1];
    pObject->actor->t.t.mat.m[3][2] -= temp.v[2];
}

// FUNCTION: CARMA2_HW 0x004b6be0
void C2_HOOK_FASTCALL ChangedObjectsCallbacks(tPhysics_object* pObjects, tPhysics_callbacks* pCallbacks, tU32 pPeriod) {
    tPhysics_object* pObject;

    if (pObjects == NULL) {
        return;
    }

    for (pObject = pObjects; pObject != NULL; pObject = pObject->next) {
        tU32 val;
        tCollision_info_owner* owner;
        tPhysics_object* child;

        if (gPHIL_enabled) {
            val = 0;
        } else if (pObject->field_0x239 == 2) {
            val = ((tCollision_info_owner*)pObject->field_0x240)->field_0x08;
        } else {
            owner = pObject->field_0x240;
            val = owner != NULL ? owner->field_0x08 : 0;
        }

        if (val != 2) {
            continue;
        }

        if (pObject->disable_move_rotate) {
            owner = pObject->field_0x240;
            if (owner == NULL) {
                continue;
            }
            if (!(owner->field_0x04 & 0x2)) {
                continue;
            }
            if (pCallbacks->active_halted != NULL && !pCallbacks->active_halted(pObject)) {
                continue;
            }
            if (gPHIL_enabled) {
                continue;
            }
            if (pObject->field_0x239 == 2) {
                ((tCollision_info_owner*)pObject->field_0x240)->field_0x08 = 1;
            } else {
                if (owner == NULL) {
                    continue;
                }
                owner->field_0x08 = 1;
                pObject->flags |= 0x20;
            }
        } else {
            if (pCallbacks->changed_objects != NULL) {
                pCallbacks->changed_objects(pObject, pPeriod);
            }
            owner = pObject->field_0x240;
            if (owner == NULL) {
                continue;
            }
            if (!(owner->field_0x04 & 0x4)) {
                continue;
            }
            for (child = pObject->child; child != NULL; child = child->next) {
                PipeSinglePHILObject(child);
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x004c25a0
static void ResetObjectAndChildren(tPhysics_object* pObject) {
    tPhysics_object* child;

    if (pObject->field_0xf0 == 1) {
        BrMatrix34Copy(&pObject->actor->t.t.mat, (const br_matrix34*)&pObject->field_0xbc);
    } else {
        BrMatrix34LPNormalise(&pObject->actor->t.t.mat, &pObject->transform_matrix);
    }
    for (child = pObject->child; child != NULL; child = child->next) {
        ResetObjectAndChildren(child);
    }
}

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x004c2600
void C2_HOOK_FASTCALL ResetObjectList(tPhysics_object* pObjects) {

    while (pObjects != NULL) {
        ResetObjectAndChildren(pObjects);
        pObjects = pObjects->next;
    }
}
#pragma auto_inline(on)

// FUNCTION: CARMA2_HW 0x004c64b0
int C2_HOOK_FASTCALL PhysicsObjectRecurseChildren(tPhysics_object* pObject, tEnumCollision_cbfn* pCallback, void* pContext) {
    tPhysics_object* child;

    for (child = pObject->child; child != NULL; child = child->next) {
        int r;

        r = PhysicsObjectRecurse(child, pCallback, pContext);
        if (r != 0) {
            return r;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0049c5e0
int C2_HOOK_STDCALL QuantizeFloatToInt(float pVal, float pMin, float pMax) {

    if (pVal > pMax) {

        pVal = pMax;
    }
    if (pVal < pMin) {
        pVal = pMin;
    }
    return (int)((pVal - pMin) / (pMax - pMin) * 65534.f - 0.5f);
}

// C2PORT-NONMATCHING: best 27.23% | difftest PASS 2000 trials (spec difftest/specs/004b8350_FUN_004b8350.json) | reason VC5 stack/register allocation
// FUNCTION: CARMA2_HW 0x004b8350
int C2_HOOK_FASTCALL FUN_004b8350(tPhysics_object* pObject, tCollision_contact* pContacts) {
    int count;
    int local_58;
    float local_54[9];
    float inverse[12];
    float fVar1;
    float fVar4;
    float fVar5;
    float* axis_component;
    tPhysics_object* other;
    tCollision_contact* contact;
    br_matrix34* object_matrix;
    int i;

    count = 0;
    if (pObject->physics_joint1 != NULL && pObject->physics_joint1->type != eJoint_none && (pObject->flags & 0x400) == 0) {
        other = pObject->field_0x230;
        if (pObject->physics_joint1->type == eJoint_quick_hinge) {
            CreateJointForce(pObject, other, pContacts, &pObject->physics_joint1->hinge_axis2);
            pContacts++;
            CreateJointForce(pObject, other, pContacts, &pObject->physics_joint1->hinge_axis3);
            pContacts++;
            count = 2;
        } else {
            axis_component = &local_54[0];
            local_58 = 3;
            count = 3;
            do {
                local_54[0] = 0.f;
                local_54[1] = 0.f;
                local_54[2] = 0.f;
                *axis_component = 1.f;
                CreateJointForce(pObject, other, pContacts, (br_vector3*)local_54);
                pContacts++;
                axis_component++;
                local_58--;
            } while (local_58 != 0);
        }

        switch (pObject->physics_joint1->type) {
        case eJoint_hinge:
            object_matrix = &pObject->transform_matrix;
            contact = pContacts;
            contact->impulse = 0.f;
            contact->object1 = pObject;
            contact->object2 = other;
            contact->r1.v[0] = pObject->physics_joint1->field_0x08.v[0] - pObject->cmpos.v[0];
            contact->r1.v[1] = pObject->physics_joint1->field_0x08.v[1] - pObject->cmpos.v[1];
            contact->r1.v[2] = pObject->physics_joint1->field_0x08.v[2] - pObject->cmpos.v[2];
            contact->r2.v[0] = pObject->physics_joint1->field_0x14.v[0] - other->cmpos.v[0];
            contact->r2.v[1] = pObject->physics_joint1->field_0x14.v[1] - other->cmpos.v[1];
            contact->r2.v[2] = pObject->physics_joint1->field_0x14.v[2] - other->cmpos.v[2];
            contact->normal.v[0] = pObject->physics_joint1->hinge_axis2.v[0];
            contact->normal.v[1] = pObject->physics_joint1->hinge_axis2.v[1];
            contact->normal.v[2] = pObject->physics_joint1->hinge_axis2.v[2];
            BrMatrix34ApplyV((br_vector3*)local_54, &contact->normal, object_matrix);
            BrMatrix34TApplyV(&contact->field_0x38, (br_vector3*)local_54, &other->transform_matrix);
            contact->type = 0xb;
            pContacts++;

            contact = pContacts;
            contact->impulse = 0.f;
            contact->object1 = pObject;
            contact->object2 = other;
            contact->r1.v[0] = pObject->physics_joint1->field_0x08.v[0] - pObject->cmpos.v[0];
            contact->r1.v[1] = pObject->physics_joint1->field_0x08.v[1] - pObject->cmpos.v[1];
            contact->r1.v[2] = pObject->physics_joint1->field_0x08.v[2] - pObject->cmpos.v[2];
            contact->r2.v[0] = pObject->physics_joint1->field_0x14.v[0] - other->cmpos.v[0];
            contact->r2.v[1] = pObject->physics_joint1->field_0x14.v[1] - other->cmpos.v[1];
            contact->r2.v[2] = pObject->physics_joint1->field_0x14.v[2] - other->cmpos.v[2];
            contact->normal.v[0] = pObject->physics_joint1->hinge_axis3.v[0];
            contact->normal.v[1] = pObject->physics_joint1->hinge_axis3.v[1];
            contact->normal.v[2] = pObject->physics_joint1->hinge_axis3.v[2];
            BrMatrix34ApplyV((br_vector3*)local_54, &contact->normal, object_matrix);
            BrMatrix34TApplyV(&contact->field_0x38, (br_vector3*)local_54, &other->transform_matrix);
            contact->type = 0xb;
            pContacts++;
            count += 2;
            break;

        case eJoint_universal:
            object_matrix = &pObject->transform_matrix;
            contact = pContacts;
            contact->impulse = 0.f;
            contact->object1 = pObject;
            contact->object2 = other;
            contact->r1.v[0] = pObject->physics_joint1->field_0x08.v[0] - pObject->cmpos.v[0];
            contact->r1.v[1] = pObject->physics_joint1->field_0x08.v[1] - pObject->cmpos.v[1];
            contact->r1.v[2] = pObject->physics_joint1->field_0x08.v[2] - pObject->cmpos.v[2];
            contact->r2.v[0] = pObject->physics_joint1->field_0x14.v[0] - other->cmpos.v[0];
            contact->r2.v[1] = pObject->physics_joint1->field_0x14.v[1] - other->cmpos.v[1];
            contact->r2.v[2] = pObject->physics_joint1->field_0x14.v[2] - other->cmpos.v[2];
            contact->normal.v[0] = pObject->physics_joint1->hinge_axis2.v[0];
            contact->normal.v[1] = pObject->physics_joint1->hinge_axis2.v[1];
            contact->normal.v[2] = pObject->physics_joint1->hinge_axis2.v[2];
            BrMatrix34ApplyV((br_vector3*)local_54, &contact->normal, object_matrix);
            BrMatrix34TApplyV(&contact->field_0x38, (br_vector3*)local_54, &other->transform_matrix);
            contact->type = 0xb;
            pContacts++;
            count++;
            break;

        case eJoint_ball_n_socket:
            break;

        case eJoint_quick_hinge: {
            tPhysics_joint_data* joint_data;

            joint_data = (tPhysics_joint_data*)GetPhysicsScratchPointer(0xd0);
            IncreasePhysicsScratchPointer(0xd0);
            pObject->field_0x1e4 = (int)(uintptr_t)joint_data;
            other->field_0x1e4 = (int)(uintptr_t)((tU8*)joint_data + 0x68);
            *(int*)(uintptr_t)pObject->field_0x1e4 = 0;
            *(tPhysics_object**)(uintptr_t)(pObject->field_0x1e4 + 4) = other;
            *(int*)(uintptr_t)other->field_0x1e4 = 0;
            *(tPhysics_object**)(uintptr_t)(other->field_0x1e4 + 4) = pObject;

            BrMatrix34LPInverse((br_matrix34*)inverse, &pObject->transform_matrix);
            BrMatrix34Mul((br_matrix34*)(uintptr_t)(pObject->field_0x1e4 + 0x38), &other->transform_matrix, (br_matrix34*)inverse);
            BrMatrix34LPInverse((br_matrix34*)(uintptr_t)(other->field_0x1e4 + 0x38), (br_matrix34*)(uintptr_t)(pObject->field_0x1e4 + 0x38));
            *(int*)(uintptr_t)pObject->field_0x1e4 = 1;
            *(int*)(uintptr_t)other->field_0x1e4 = 1;
            FUN_004b8cf0(pObject);

            fVar1 = pObject->physics_joint1->parent_bone_axis.v[1] * other->omega.v[1]
                    + other->omega.v[0] * pObject->physics_joint1->parent_bone_axis.v[0]
                    + pObject->physics_joint1->parent_bone_axis.v[2] * other->omega.v[2];
            local_54[0] = other->omega.v[0] - pObject->physics_joint1->parent_bone_axis.v[0] * fVar1;
            local_54[1] = other->omega.v[1] - pObject->physics_joint1->parent_bone_axis.v[1] * fVar1;
            local_54[2] = other->omega.v[2] - pObject->physics_joint1->parent_bone_axis.v[2] * fVar1;
            BrMatrix34ApplyV((br_vector3*)&local_54[3], (br_vector3*)local_54, &other->transform_matrix);
            BrMatrix34TApplyV((br_vector3*)local_54, (br_vector3*)&local_54[3], &pObject->transform_matrix);

            fVar4 = pObject->physics_joint1->hinge_axis.v[1] * pObject->omega.v[1]
                    + pObject->physics_joint1->hinge_axis.v[2] * pObject->omega.v[2]
                    + pObject->physics_joint1->hinge_axis.v[0] * pObject->omega.v[0];
            local_54[0] += pObject->physics_joint1->hinge_axis.v[0] * fVar4;
            local_54[1] += pObject->physics_joint1->hinge_axis.v[1] * fVar4;
            local_54[2] += pObject->physics_joint1->hinge_axis.v[2] * fVar4;
            pObject->omega.v[0] = local_54[0];
            pObject->omega.v[1] = local_54[1];
            pObject->omega.v[2] = local_54[2];

            fVar1 = pObject->physics_joint1->field_0x14.v[0] - other->cmpos.v[0];
            fVar4 = pObject->physics_joint1->field_0x14.v[1] - other->cmpos.v[1];
            fVar5 = pObject->physics_joint1->field_0x14.v[2] - other->cmpos.v[2];
            local_54[3] = other->omega.v[1] * fVar5 - other->omega.v[2] * fVar4;
            local_54[4] = other->omega.v[2] * fVar1 - other->omega.v[0] * fVar5;
            local_54[5] = other->omega.v[0] * fVar4 - other->omega.v[1] * fVar1;
            BrMatrix34ApplyV((br_vector3*)local_54, (br_vector3*)&local_54[3], &other->transform_matrix);
            local_54[0] += other->v.v[0];
            local_54[1] += other->v.v[1];
            local_54[2] += other->v.v[2];
            BrMatrix34TApplyV((br_vector3*)&local_54[6], (br_vector3*)local_54, &pObject->transform_matrix);

            fVar1 = pObject->physics_joint1->field_0x08.v[0] - pObject->cmpos.v[0];
            fVar4 = pObject->physics_joint1->field_0x08.v[1] - pObject->cmpos.v[1];
            fVar5 = pObject->physics_joint1->field_0x08.v[2] - pObject->cmpos.v[2];
            local_54[3] = pObject->omega.v[1] * fVar5 - pObject->omega.v[2] * fVar4;
            local_54[4] = pObject->omega.v[2] * fVar1 - pObject->omega.v[0] * fVar5;
            local_54[5] = pObject->omega.v[0] * fVar4 - pObject->omega.v[1] * fVar1;
            local_54[6] -= local_54[3];
            local_54[7] -= local_54[4];
            local_54[8] -= local_54[5];
            local_54[2] = (pObject->physics_joint1->hinge_axis.v[1] * local_54[7]
                    + pObject->physics_joint1->hinge_axis.v[0] * local_54[6]
                    + pObject->physics_joint1->hinge_axis.v[2] * local_54[8])
                    - (pObject->physics_joint1->hinge_axis.v[1] * pObject->velocity_car_space.v[1]
                    + pObject->physics_joint1->hinge_axis.v[0] * pObject->velocity_car_space.v[0]
                    + pObject->physics_joint1->hinge_axis.v[2] * pObject->velocity_car_space.v[2]);
            local_54[0] = pObject->physics_joint1->hinge_axis.v[0] * local_54[2];
            local_54[1] = pObject->physics_joint1->hinge_axis.v[1] * local_54[2];
            local_54[2] = pObject->physics_joint1->hinge_axis.v[2] * local_54[2];
            pObject->velocity_car_space.v[0] += local_54[0];
            pObject->velocity_car_space.v[1] += local_54[1];
            pObject->velocity_car_space.v[2] += local_54[2];
            BrMatrix34ApplyV(&pObject->v, &pObject->velocity_car_space, &pObject->transform_matrix);
            break;
        }

        default:
            PhysicsError((tPhysicsError)5, NULL);
        }
    }

    if (pObject->physics_joint2 != NULL) {
        if (pObject->physics_joint2->type == eJoint_translation) {
            contact = pContacts;
            contact->impulse = 0.f;
            contact->object1 = pObject;
            contact->object2 = NULL;
            contact->field_0xc = NULL;
            contact->normal.v[0] = pObject->physics_joint2->hinge_axis2.v[0];
            contact->normal.v[1] = pObject->physics_joint2->hinge_axis2.v[1];
            contact->normal.v[2] = pObject->physics_joint2->hinge_axis2.v[2];
            contact->type = 10;
            pContacts++;
            count++;

            contact = pContacts;
            contact->impulse = 0.f;
            contact->object1 = pObject;
            contact->object2 = NULL;
            contact->field_0xc = NULL;
            contact->normal.v[0] = pObject->physics_joint2->hinge_axis3.v[0];
            contact->normal.v[1] = pObject->physics_joint2->hinge_axis3.v[1];
            contact->normal.v[2] = pObject->physics_joint2->hinge_axis3.v[2];
            contact->type = 10;
            pContacts++;
            count++;
        } else {
            count += 3;
            i = 0;
            do {
                contact = pContacts++;
                contact->impulse = 0.f;
                contact->object1 = pObject;
                contact->object2 = NULL;
                contact->field_0xc = NULL;
                contact->r1.v[0] = pObject->physics_joint2->field_0x08.v[0] - pObject->cmpos.v[0];
                contact->r1.v[1] = pObject->physics_joint2->field_0x08.v[1] - pObject->cmpos.v[1];
                contact->r1.v[2] = pObject->physics_joint2->field_0x08.v[2] - pObject->cmpos.v[2];
                contact->normal.v[0] = 0.f;
                contact->normal.v[1] = 0.f;
                contact->normal.v[2] = 0.f;
                contact->normal.v[i] = 1.f;
                contact->type = 10;
                i++;
            } while (i < 3);

            switch (pObject->physics_joint2->type) {
            case eJoint_hinge:
            case eJoint_ball_n_socket:
                break;

            case eJoint_universal:
                contact = pContacts;
                contact->impulse = 0.f;
                contact->object1 = pObject;
                contact->object2 = NULL;
                contact->field_0xc = NULL;
                contact->r1.v[0] = pObject->physics_joint2->field_0x08.v[0] - pObject->cmpos.v[0];
                contact->r1.v[1] = pObject->physics_joint2->field_0x08.v[1] - pObject->cmpos.v[1];
                contact->r1.v[2] = pObject->physics_joint2->field_0x08.v[2] - pObject->cmpos.v[2];
                contact->normal.v[0] = pObject->physics_joint2->hinge_axis2.v[0];
                contact->normal.v[1] = pObject->physics_joint2->hinge_axis2.v[1];
                contact->normal.v[2] = pObject->physics_joint2->hinge_axis2.v[2];
                contact->type = 0xb;
                pContacts++;
                count++;
                break;

            default:
                PhysicsError((tPhysicsError)5, NULL);
            }
        }
    }

    for (other = pObject->child; other != NULL; other = other->next) {
        int child_count;

        child_count = FUN_004b8350(other, pContacts);
        count += child_count;
        pContacts += child_count;
    }
    return count;
}

// FUNCTION: CARMA2_HW 0x0049c0d0
#pragma inline_depth(0)
int C2_HOOK_FASTCALL GetObjectNetworkStuff(tPhysics_object* pObject, tU8* pBuffer, int pSize) {
    tPhysics_joint* joint;
    br_quat quat;

    joint = pObject->physics_joint1;
    if (joint == NULL) {
        goto check_big;
    }
    switch (joint->type) {
    case 1:
    case 4:
        if ((tU32)pSize < 0xc) {
            return -1;
        }
        pBuffer[0] = (tU8)joint->type;
        pBuffer[1] = pObject->uid;
        *(float*)&pSize = pObject->omega.v[0] * joint->hinge_axis.v[0]
            + pObject->omega.v[1] * joint->hinge_axis.v[1]
            + pObject->omega.v[2] * joint->hinge_axis.v[2];
        if (*(float*)&pSize > 39.2699f) {
            *(float*)&pSize = 39.2699f;
        }
        if (*(float*)&pSize < -39.2699f) {
            *(float*)&pSize = -39.2699f;
        }
        ((tU16*)pBuffer)[1] = (tU16)((*(float*)&pSize + 39.2699f) * 0.0127324f * 65534.f - 0.5f);
        pObject = (tPhysics_object*)((tU8*)pObject + 0x8c);
        BrMatrix34ToQuat(&quat, (br_matrix34*)pObject);
        {
            float q = quat.z;
            if (q > 1.0f) {
                q = 1.0f;
            }
            if (q < -1.0f) {
                q = -1.0f;
            }
            ((tU16*)pBuffer)[2] = (tU16)((q + 1.0f) * 0.5f * 65534.f - 0.5f);
        }
        {
            float q = quat.y;
            if (q > 1.0f) {
                q = 1.0f;
            }
            if (q < -1.0f) {
                q = -1.0f;
            }
            ((tU16*)pBuffer)[3] = (tU16)((q + 1.0f) * 0.5f * 65534.f - 0.5f);
        }
        ((tU16*)pBuffer)[4] = QuantizeFloatToInt(quat.z, -1.0f, 1.0f);
        ((tU16*)pBuffer)[5] = QuantizeFloatToInt(quat.w, -1.0f, 1.0f);
        return 0xc;
    case 2:
    case 3:
        if ((tU32)pSize < 0x10) {
            return -1;
        }
        pBuffer[0] = (tU8)joint->type;
        pBuffer[1] = pObject->uid;
        {
            int k;
            br_vector3* v = (br_vector3*)((tU8*)pObject + 0x74);
            tU16* out = (tU16*)&pBuffer[2];
            for (k = 0; k < 3; k++) {
                float c = v->v[0];
                if (c > 39.2699f) {
                    c = 39.2699f;
                }
                if (c < -39.2699f) {
                    c = -39.2699f;
                }
                *out++ = (tU16)((c + 39.2699f) * 0.0127324f * 65534.f - 0.5f);
                v = (br_vector3*)((tU8*)v + 4);
            }
        }
        pObject = (tPhysics_object*)((tU8*)pObject + 0x8c);
        BrMatrix34ToQuat(&quat, (br_matrix34*)pObject);
        {
            float q = quat.z;
            if (q > 1.0f) {
                q = 1.0f;
            }
            if (q < -1.0f) {
                q = -1.0f;
            }
            ((tU16*)pBuffer)[4] = (tU16)((q + 1.0f) * 0.5f * 65534.f - 0.5f);
        }
        {
            float q = quat.y;
            if (q > 1.0f) {
                q = 1.0f;
            }
            if (q < -1.0f) {
                q = -1.0f;
            }
            ((tU16*)pBuffer)[5] = (tU16)((q + 1.0f) * 0.5f * 65534.f - 0.5f);
        }
        ((tU16*)pBuffer)[6] = QuantizeFloatToInt(quat.z, -1.0f, 1.0f);
        ((tU16*)pBuffer)[7] = QuantizeFloatToInt(quat.w, -1.0f, 1.0f);
        return 0x10;
    default:
        PhysicsError((tPhysicsError)9, "netphys got messed up");
        /* fall through */
    case 0:
check_big:
        if ((tU32)pSize < 0x28) {
            return -1;
        }
        pBuffer[0] = 0;
        pBuffer[1] = pObject->uid;
        {
            int k;
            br_vector3* v = (br_vector3*)((tU8*)pObject + 0x74);
            tU16* out = (tU16*)&pBuffer[2];
            for (k = 0; k < 3; k++) {
                float c = v->v[0];
                if (c > 39.2699f) {
                    c = 39.2699f;
                }
                if (c < -39.2699f) {
                    c = -39.2699f;
                }
                *out++ = (tU16)((c + 39.2699f) * 0.0127324f * 65534.f - 0.5f);
                v = (br_vector3*)((tU8*)v + 4);
            }
        }
        {
            tU32 vx = *(tU32*)((tU8*)pObject + 0x68);
            pObject = (tPhysics_object*)((tU8*)pObject + 0x8c);
            ((tU32*)pBuffer)[2] = vx;
            ((tU32*)pBuffer)[3] = *(tU32*)((tU8*)pObject - 0x20);
            ((tU32*)pBuffer)[4] = *(tU32*)((tU8*)pObject - 0x1c);
        }
        BrMatrix34ToQuat(&quat, (br_matrix34*)pObject);
        {
            float q = quat.z;
            if (q > 1.0f) {
                q = 1.0f;
            }
            if (q < -1.0f) {
                q = -1.0f;
            }
            ((tU16*)pBuffer)[10] = (tU16)((q + 1.0f) * 0.5f * 65534.f - 0.5f);
        }
        {
            float q = quat.y;
            if (q > 1.0f) {
                q = 1.0f;
            }
            if (q < -1.0f) {
                q = -1.0f;
            }
            ((tU16*)pBuffer)[11] = (tU16)((q + 1.0f) * 0.5f * 65534.f - 0.5f);
        }
        ((tU16*)pBuffer)[12] = QuantizeFloatToInt(quat.z, -1.0f, 1.0f);
        ((tU16*)pBuffer)[13] = QuantizeFloatToInt(quat.w, -1.0f, 1.0f);
        ((tU32*)pBuffer)[7] = *(tU32*)((tU8*)pObject + 0x24);
        ((tU32*)pBuffer)[8] = *(tU32*)((tU8*)pObject + 0x28);
        ((tU32*)pBuffer)[9] = *(tU32*)((tU8*)pObject + 0x2c);
        return 0x28;
    }
}

// C2PORT-NONMATCHING: best 31.19% | difftest PASS 2000 trials (spec difftest/specs/0049c630_SetObjectNetworkStuff.json) | reason VC5 x87 scheduling and register allocation
// FUNCTION: CARMA2_HW 0x0049c630
int C2_HOOK_FASTCALL SetObjectNetworkStuff(const tU8* pBuffer, tPhysics_object* pObject, int pMode) {
    br_scalar component;
    br_scalar relative_rate;
    br_scalar projection;
    br_quat quat;
    br_vector3 tv;
    register tPhysics_joint* joint;
    register tPhysics_object* parent;
    register br_matrix34* parent_matrix;
    register br_matrix34* matrix;
    int result;

    C2_HOOK_BUG_ON(offsetof(tPhysics_object, v) != 0x68);
    C2_HOOK_BUG_ON(offsetof(tPhysics_object, omega) != 0x74);
    C2_HOOK_BUG_ON(offsetof(tPhysics_object, transform_matrix) != 0x8C);
    C2_HOOK_BUG_ON(offsetof(tPhysics_object, physics_joint1) != 0x188);
    C2_HOOK_BUG_ON(offsetof(tPhysics_object, parent) != 0x228);
    C2_HOOK_BUG_ON(offsetof(tPhysics_joint, hinge_axis) != 0x38);

    if (pObject == NULL) {
        switch (pBuffer[0] & 0xF) {
        case 0:
            return 0x28;
        case 1:
        case 4:
            return 0xC;
        case 2:
        case 3:
            return 0x10;
        default:
            PhysicsError(9, "netphys got messed up");
            return 0;
        }
    }

    parent = pObject->parent;
    if (pMode == 1) {
        matrix = &pObject->actor->t.t.mat;
        parent_matrix = parent != NULL ? &parent->actor->t.t.mat : NULL;
    } else {
        matrix = &pObject->transform_matrix;
        parent_matrix = parent != NULL ? &parent->transform_matrix : NULL;
    }

    switch (pBuffer[0] & 0xF) {
    case 0:
        pObject->omega.v[0] = (br_scalar)((const tU16*)pBuffer)[1] * gFLOAT_0058a40c * gFLOAT_0058a420 - gFLOAT_0058a414;
        pObject->omega.v[1] = (br_scalar)((const tU16*)pBuffer)[2] * gFLOAT_0058a40c * gFLOAT_0058a420 - gFLOAT_0058a414;
        pObject->omega.v[2] = (br_scalar)((const tU16*)pBuffer)[3] * gFLOAT_0058a40c * gFLOAT_0058a420 - gFLOAT_0058a414;
        memcpy(&pObject->v, pBuffer + 8, sizeof(pObject->v));
        quat.x = (br_scalar)((const tU16*)pBuffer)[10] * gFLOAT_0058a40c;
        quat.x = quat.x + quat.x - gFLOAT_0058a3f8;
        quat.y = (br_scalar)((const tU16*)pBuffer)[11] * gFLOAT_0058a40c;
        quat.y = quat.y + quat.y - gFLOAT_0058a3f8;
        quat.z = (br_scalar)((const tU16*)pBuffer)[12] * gFLOAT_0058a40c;
        quat.z = quat.z + quat.z - gFLOAT_0058a3f8;
        quat.w = (br_scalar)((const tU16*)pBuffer)[13] * gFLOAT_0058a40c;
        quat.w = quat.w + quat.w - gFLOAT_0058a3f8;
        BrQuatNormalise(&quat, &quat);
        BrQuatToMatrix34(matrix, &quat);
        memcpy(matrix->m[3], pBuffer + 0x1C, sizeof(matrix->m[3]));
        result = 0x28;
        break;
    case 1:
    case 4:
        quat.x = (br_scalar)((const tU16*)pBuffer)[2] * gFLOAT_0058a40c;
        quat.x = quat.x + quat.x - gFLOAT_0058a3f8;
        quat.y = (br_scalar)((const tU16*)pBuffer)[3] * gFLOAT_0058a40c;
        quat.y = quat.y + quat.y - gFLOAT_0058a3f8;
        quat.z = (br_scalar)((const tU16*)pBuffer)[4] * gFLOAT_0058a40c;
        quat.z = quat.z + quat.z - gFLOAT_0058a3f8;
        quat.w = (br_scalar)((const tU16*)pBuffer)[5] * gFLOAT_0058a40c;
        quat.w = quat.w + quat.w - gFLOAT_0058a3f8;
        BrQuatNormalise(&quat, &quat);
        BrQuatToMatrix34(matrix, &quat);
        BrMatrix34ApplyV(&tv, &parent->omega, parent_matrix);
        BrMatrix34TApplyV(&pObject->omega, &tv, matrix);
        joint = pObject->physics_joint1;
        component = (br_scalar)((const tU16*)pBuffer)[1] * gFLOAT_0058a40c * gFLOAT_0058a420 - gFLOAT_0058a414;
        projection = pObject->omega.v[0] * joint->hinge_axis.v[0]
            + pObject->omega.v[1] * joint->hinge_axis.v[1]
            + pObject->omega.v[2] * joint->hinge_axis.v[2];
        relative_rate = component - projection;
        pObject->omega.v[0] += joint->hinge_axis.v[0] * relative_rate;
        pObject->omega.v[1] += joint->hinge_axis.v[1] * relative_rate;
        pObject->omega.v[2] += joint->hinge_axis.v[2] * relative_rate;
        FUN_0049cb00(pObject, matrix, (int)parent_matrix);
        result = 0xC;
        break;
    case 2:
    case 3:
        pObject->omega.v[0] = (br_scalar)((const tU16*)pBuffer)[1] * gFLOAT_0058a40c * gFLOAT_0058a420 - gFLOAT_0058a414;
        pObject->omega.v[1] = (br_scalar)((const tU16*)pBuffer)[2] * gFLOAT_0058a40c * gFLOAT_0058a420 - gFLOAT_0058a414;
        pObject->omega.v[2] = (br_scalar)((const tU16*)pBuffer)[3] * gFLOAT_0058a40c * gFLOAT_0058a420 - gFLOAT_0058a414;
        quat.x = (br_scalar)((const tU16*)pBuffer)[4] * gFLOAT_0058a40c;
        quat.x = quat.x + quat.x - gFLOAT_0058a3f8;
        quat.y = (br_scalar)((const tU16*)pBuffer)[5] * gFLOAT_0058a40c;
        quat.y = quat.y + quat.y - gFLOAT_0058a3f8;
        quat.z = (br_scalar)((const tU16*)pBuffer)[6] * gFLOAT_0058a40c;
        quat.z = quat.z + quat.z - gFLOAT_0058a3f8;
        quat.w = (br_scalar)((const tU16*)pBuffer)[7] * gFLOAT_0058a40c;
        quat.w = quat.w + quat.w - gFLOAT_0058a3f8;
        BrQuatNormalise(&quat, &quat);
        BrQuatToMatrix34(matrix, &quat);
        FUN_0049cb00(pObject, matrix, (int)parent_matrix);
        result = 0x10;
        break;
    default:
        PhysicsError(9, "netphys got messed up");
        result = pMode;
        break;
    }

    if (pMode == 2) {
        BrMatrix34Copy(&pObject->actor->t.t.mat, matrix);
    }
    return result;
}

// C2PORT-NONMATCHING: best 21.88% | difftest PASS 2000 trials (spec difftest/specs/0049cb00_FUN_0049cb00.json) | reason VC5 x87 scheduling and register allocation
// FUNCTION: CARMA2_HW 0x0049cb00
void C2_HOOK_FASTCALL FUN_0049cb00(tPhysics_object* pObject, br_matrix34* pMatrix, int pParent_matrix) {
    float fVar1;
    float fVar2;
    float fVar3;
    float fVar4;
    float fVar5;
    float fVar6;
    float fVar9;
    float fVar10;
    float fVar11;
    float fVar12;
    float fVar13;
    float fVar14;
    register tPhysics_joint* joint;
    register tPhysics_object* parent;
    register br_matrix34* matrix;
    register br_matrix34* parent_matrix;
    br_vector3 temp1;
    br_vector3 temp2;

    matrix = pMatrix;
    joint = pObject->physics_joint1;
    fVar9 = joint->field_0x08.v[0] - pObject->cmpos.v[0];
    fVar10 = joint->field_0x08.v[1] - pObject->cmpos.v[1];
    fVar11 = joint->field_0x08.v[2] - pObject->cmpos.v[2];
    fVar1 = pObject->omega.v[2];
    fVar2 = pObject->omega.v[1];
    fVar3 = pObject->omega.v[2];
    fVar4 = pObject->omega.v[1];
    fVar5 = pObject->omega.v[0];
    fVar6 = pObject->omega.v[0];
    temp1.v[0] = fVar1 * fVar10 - fVar2 * fVar11;
    temp1.v[1] = fVar5 * fVar11 - fVar3 * fVar9;
    temp1.v[2] = fVar4 * fVar9 - fVar6 * fVar10;
    BrMatrix34ApplyV(&pObject->v, &temp1, matrix);

    if (pParent_matrix != 0) {
        parent = pObject->parent;
        parent_matrix = (br_matrix34*)pParent_matrix;

        fVar10 = joint->field_0x14.v[1] - parent->cmpos.v[1];
        fVar9 = joint->field_0x14.v[2] - parent->cmpos.v[2];
        fVar1 = parent->omega.v[2];
        fVar2 = parent->omega.v[1];
        fVar11 = joint->field_0x14.v[0] - parent->cmpos.v[0];
        fVar3 = parent->omega.v[0];
        fVar4 = parent->omega.v[2];
        fVar5 = parent->omega.v[1];
        fVar6 = parent->omega.v[0];
        temp1.v[0] = fVar2 * fVar9 - fVar1 * fVar10;
        temp1.v[1] = fVar4 * fVar11 - fVar3 * fVar9;
        temp1.v[2] = fVar6 * fVar10 - fVar5 * fVar11;
        BrMatrix34ApplyV(&temp2, &temp1, matrix);

        fVar12 = parent->v.v[0] + temp2.v[0];
        fVar13 = parent->v.v[1] + temp2.v[1];
        fVar14 = parent->v.v[2] + temp2.v[2];
        pObject->v.v[0] = fVar12 + pObject->v.v[0];
        pObject->v.v[1] = fVar13 + pObject->v.v[1];
        pObject->v.v[2] = fVar14 + pObject->v.v[2];

        BrMatrix34ApplyV(&temp1, &joint->field_0x08, matrix);
        BrMatrix34ApplyP(&temp2, &joint->field_0x14, parent_matrix);
        matrix->m[3][0] = temp2.v[0] - temp1.v[0];
        matrix->m[3][1] = temp2.v[1] - temp1.v[1];
        matrix->m[3][2] = temp2.v[2] - temp1.v[2];
    } else {
        BrMatrix34ApplyV(&temp1, &joint->field_0x08, matrix);
        matrix->m[3][0] = pObject->field_0x17c.v[0] - temp1.v[0];
        matrix->m[3][1] = pObject->field_0x17c.v[1] - temp1.v[1];
        matrix->m[3][2] = pObject->field_0x17c.v[2] - temp1.v[2];
    }
}

#pragma inline_depth()

// FUNCTION: CARMA2_HW 0x0049cd00
int C2_HOOK_FASTCALL GetHierarchyNetworkStuff(tPhysics_object* pObject, tU8* pBuffer, int pSize) {
    int size;
    int child_size;
    tPhysics_object* child;
    tU8* child_buffer;

    size = GetObjectNetworkStuff(pObject, pBuffer, pSize);
    if (size < 0) {
        return -1;
    }
    child = pObject->child;

#undef local_48
#undef local_38
#undef local_34
#undef local_30
#undef local_2c
#undef local_28
#undef local_24
#undef local_14
#undef local_10
#undef local_4

    pSize -= size;
    if (child != NULL) {
        *pBuffer |= 0x80;
        for (; child != NULL; child = child->next) {
            child_buffer = pBuffer + size;
            child_size = GetHierarchyNetworkStuff(child, child_buffer, pSize);
            if (child_size < 0) {
                return -1;
            }
            pSize -= child_size;
            size += child_size;
            *child_buffer |= 0x40;
        }
        *child_buffer &= ~0x40;
    }
    return size;
}

#pragma inline_depth(0)
int C2_HOOK_FASTCALL SizeOfObjectNetworkStuff(tPhysics_joint_type pType) {
    switch (pType & 0xf) {
    case eJoint_none:
        return 0x28; // FIXME!
    case eJoint_hinge:
    case eJoint_quick_hinge:
        return 0xc; // FIXME!
    case eJoint_universal:
    case eJoint_ball_n_socket:
        return 0x10; // FIXME!
    default:
        PhysicsError(9, "netphys got messed up");
        return 0;
    }
}
#pragma inline_depth()

// FUNCTION: CARMA2_HW 0x0049ce60
int C2_HOOK_FASTCALL GetHierarchyNetworkSize(tPhysics_object* pObject) {
    int size;
    tPhysics_object* child;

    size = SizeOfObjectNetworkStuff(pObject->physics_joint1 != NULL ? pObject->physics_joint1->type : eJoint_none);
    for (child = pObject->child; child != NULL; child = child->next) {
        size += GetHierarchyNetworkSize(child);
    }
    return size;
}
// FUNCTION: CARMA2_HW 0x004c5e80
void C2_HOOK_FASTCALL FreeShapeList(tPhysics_shape *pShape) {

    if (pShape->common.next != NULL) {
        FreeShapeList(pShape->common.next);
    }
    BrMemFree(pShape);
}

// FUNCTION: CARMA2_HW 0x004c5ea0
void C2_HOOK_FASTCALL FreePhysicsJoint(tPhysics_joint* pJoint) {

    BrMemFree(pJoint);
}

// GetBoundsFromPointList

// FillInShape

// UpdateCollisionObject

// PhysicsError

// PhysicsWarning

// PhysicsSetErrorHandler

// FUNCTION: CARMA2_HW 0x004b8cf0
void C2_HOOK_FASTCALL FUN_004b8cf0(tPhysics_object* pObject) {
    int uVar1;
    int uVar2;
    int i;
    int* piVar4;
    char* puVar3;
    int local_174[6];
    float local_15c[15];
    /* retail esp+0x120/0xc0/0x60: three consecutive 0x60-byte solver rows, walked below by
     * stepping 0x60 bytes from the first. Separate locals only worked where the compiler happened
     * to lay them out contiguously (MSVC x86); GCC/ARM did not and the solver read garbage objects
     * (Vita 00.23 crash in FUN_004be4b0 via FUN_004bf490). */
    union {
        tCollision_contact contact;
        tPhysics_joint_data joint;
    } rows[3];
    tPhysics_object* pObject2;
    tPhysics_object* pOther;

    pObject2 = pObject->field_0x230;
    pOther = (tPhysics_object*)*(int*)(pObject->field_0x1e4 + 4);
    if (pOther != pObject2) {
        pObject2 = pObject;
        pObject = pOther;
    }
    CreateJointForce(pObject, pObject2, &rows[0].contact, (br_vector3*)((char*)pObject->physics_joint1 + 0x38));
    CreateJointTorque(pObject, pObject2, &rows[1].joint, (br_vector3*)((char*)pObject->physics_joint1 + 0x20));
    CreateJointTorque(pObject, pObject2, &rows[2].joint, (br_vector3*)((char*)pObject->physics_joint1 + 0x2c));
    piVar4 = local_174;
    puVar3 = (char*)rows;
    i = 3;
    do {
        *piVar4 = (int)puVar3;
        puVar3 += 0x60;
        piVar4++;
        i--;
    } while (i != 0);
    uVar1 = pObject2->field_0x1e4;
    uVar2 = pObject->field_0x1e4;
    pObject->field_0x1e4 = 0;
    pObject2->field_0x1e4 = 0;
    FUN_004bf490((int*)&local_15c[3], 3, local_174, 3);
    FUN_004be350((float*)(uintptr_t)(uVar2 + 8), &local_15c[3]);
    FUN_004b77b0(&local_174[3], local_174, 3);
    BrMatrix34ApplyV((br_vector3*)&local_15c[0], (br_vector3*)&local_174[3], (br_matrix34*)(uintptr_t)(uVar2 + 8));
    i = 0;
    do {
        FUN_004b7960((tPhysics_joint_data*)*(int*)((char*)local_174 + i), *(float*)((char*)local_15c + i));
        i += 4;
    } while (i < 0xc);
    pObject->field_0x1e4 = uVar2;
    pObject2->field_0x1e4 = uVar1;
}

// C2PORT-NONMATCHING: best 21.42% | difftest PASS 2000 trials (spec difftest/specs/004bf490_FUN_004bf490.json) | reason original stack layout and x87 operation ordering differ
// FUNCTION: CARMA2_HW 0x004bf490
void C2_HOOK_FASTCALL FUN_004bf490(int* param_1, int param_2, int* param_3, int param_4) {
  /* Keep each four-byte stack slot addressable for contiguous vector outputs. */
  union { int i[62]; float f[62]; } local_storage;
#define local_e0 local_storage.i[6]
#define local_dc local_storage.f[7]
#define local_d8 local_storage.f[8]
#define local_d4 local_storage.f[9]
#define local_d0 local_storage.f[10]
#define local_cc local_storage.f[11]
#define local_c8 local_storage.f[12]
#define local_c4 local_storage.f[13]
#define local_c0 local_storage.f[14]
#define local_bc local_storage.f[15]
#define local_b8 local_storage.f[16]
#define local_b4 local_storage.f[17]
#define local_b0 local_storage.f[18]
#define local_ac local_storage.f[19]
#define local_a8 local_storage.f[20]
#define local_a4 local_storage.f[21]
#define local_a0 local_storage.f[22]
#define local_9c local_storage.f[23]
#define local_98 local_storage.f[24]
#define local_94 local_storage.f[25]
#define local_90 local_storage.f[26]
#define local_8c local_storage.f[27]
#define local_88 local_storage.f[28]
#define local_84 local_storage.f[29]
#define local_80 local_storage.f[30]
#define local_7c local_storage.i[31]
#define local_78 local_storage.i[32]
#define local_74 local_storage.i[33]
#define local_70 local_storage.i[34]
#define local_6c local_storage.i[35]
#define local_68 local_storage.i[36]
#define local_64 local_storage.i[37]
#define local_60 local_storage.f[38]
#define local_5c local_storage.f[39]
#define local_58 local_storage.f[40]
#define local_54 local_storage.f[41]
#define local_50 local_storage.f[42]
#define local_4c local_storage.f[43]
#define local_48 local_storage.f[44]
#define local_44 local_storage.f[45]
#define local_40 local_storage.f[46]
#define local_3c local_storage.f[47]
#define local_38 local_storage.f[48]
#define local_34 local_storage.f[49]
#define local_30 local_storage.f[50]
#define local_2c local_storage.f[51]
#define local_28 local_storage.f[52]
#define local_24 local_storage.f[53]
#define local_20 local_storage.f[54]
#define local_1c local_storage.f[55]
#define local_18 local_storage.f[56]
#define local_14 local_storage.f[57]
#define local_10 local_storage.f[58]
#define local_c local_storage.f[59]
#define local_8 local_storage.f[60]
#define local_4 local_storage.f[61]
  int iVar1;
  float fVar2;
  int iVar3;
  float *pfVar4;
  int *piVar5;
  float *pfVar6;
  int iVar7;
  int iVar8;
  int iVar9;

  local_74 = 0;
  local_78 = param_2;
  local_64 = (int)(uintptr_t)param_1;
  if (0 < param_4) {
    do {
      iVar7 = local_74;
      iVar1 = param_3[local_74];
      iVar9 = *(int *)(iVar1 + 8);
      iVar8 = *(int *)(iVar1 + 4);
      if ((iVar9 != 0) && (*(int *)(iVar1 + 0x5c) == 0xe)) {
        iVar9 = 0;
      }
      local_7c = iVar8;
      if (*(int *)(iVar1 + 0x5c) == 0xb) {
        iVar9 = *(int *)(iVar1 + 4);
        iVar8 = *(int *)(iVar1 + 8);
        iVar3 = (int)(uintptr_t)FUN_004be550(&local_c4, &local_48, &local_94, &local_30, (const br_vector3*)(iVar1 + 0x2c), (tPhysics_object*)iVar9);
        if (iVar8 != 0) {
          local_6c = (int)(uintptr_t)FUN_004be550(&local_a0, &local_18, &local_b8, &local_3c, (const br_vector3*)(iVar1 + 0x38), (tPhysics_object*)iVar8);
        }
        if (-1 < iVar7) {
          pfVar6 = (float *)(local_64 + iVar7 * local_78 * 4);
          pfVar4 = (float *)(local_64 + iVar7 * 4);
          local_7c = iVar7 + 1;
          piVar5 = param_3;
          do {
            iVar1 = *piVar5;
            if (*(int *)(iVar1 + 0x5c) == 0xb) {
              if (*(int *)(iVar1 + 4) == iVar9) {
                *pfVar4 = *(float *)(iVar1 + 0x2c) * local_c4 +
                          *(float *)(iVar1 + 0x30) * local_c0 + *(float *)(iVar1 + 0x34) * local_bc;
              }
              else if (*(int *)(iVar1 + 8) == iVar9) {
                *pfVar4 = -(*(float *)(iVar1 + 0x40) * local_bc +
                           *(float *)(iVar1 + 0x38) * local_c4 + *(float *)(iVar1 + 0x3c) * local_c0
                           );
              }
              else {
                *pfVar4 = 0.0;
              }
              if (iVar3 != 0) {
                iVar1 = *piVar5;
                if (*(int *)(iVar1 + 4) == iVar3) {
                  fVar2 = *(float *)(iVar1 + 0x2c) * local_94 +
                          *(float *)(iVar1 + 0x30) * local_90 + *(float *)(iVar1 + 0x34) * local_8c
                          + *pfVar4;
                }
                else {
                  if (*(int *)(iVar1 + 8) != iVar3) goto LAB_004bf646;
                  fVar2 = *pfVar4 - (*(float *)(iVar1 + 0x40) * local_8c +
                                    *(float *)(iVar1 + 0x38) * local_94 +
                                    *(float *)(iVar1 + 0x3c) * local_90);
                }
                *pfVar4 = fVar2;
              }
LAB_004bf646:
              if (iVar8 != 0) {
                iVar1 = *piVar5;
                if (*(int *)(iVar1 + 4) == iVar8) {
                  fVar2 = *pfVar4 - (*(float *)(iVar1 + 0x2c) * local_a0 +
                                    *(float *)(iVar1 + 0x30) * local_9c +
                                    *(float *)(iVar1 + 0x34) * local_98);
LAB_004bf6a6:
                  *pfVar4 = fVar2;
                }
                else if (*(int *)(iVar1 + 8) == iVar8) {
                  fVar2 = *(float *)(iVar1 + 0x40) * local_98 +
                          *(float *)(iVar1 + 0x38) * local_a0 + *(float *)(iVar1 + 0x3c) * local_9c
                          + *pfVar4;
                  goto LAB_004bf6a6;
                }
                if (local_6c != 0) {
                  iVar1 = *piVar5;
                  if (*(int *)(iVar1 + 4) == local_6c) {
                    *pfVar4 = *pfVar4 - (*(float *)(iVar1 + 0x2c) * local_b8 +
                                        *(float *)(iVar1 + 0x30) * local_b4 +
                                        *(float *)(iVar1 + 0x34) * local_b0);
                  }
                  else if (*(int *)(iVar1 + 8) == local_6c) {
                    *pfVar4 = *(float *)(iVar1 + 0x40) * local_b0 +
                              *(float *)(iVar1 + 0x38) * local_b8 +
                              *(float *)(iVar1 + 0x3c) * local_b4 + *pfVar4;
                  }
                }
              }
            }
            else {
              if (*(int *)(iVar1 + 4) == iVar9) {
                *pfVar4 = *(float *)(iVar1 + 0x2c) *
                          ((*(float *)(iVar1 + 0x1c) * local_c0 -
                           *(float *)(iVar1 + 0x18) * local_bc) + local_48) +
                          *(float *)(iVar1 + 0x34) *
                          ((*(float *)(iVar1 + 0x18) * local_c4 -
                           *(float *)(iVar1 + 0x14) * local_c0) + local_40) +
                          *(float *)(iVar1 + 0x30) *
                          ((*(float *)(iVar1 + 0x14) * local_bc -
                           *(float *)(iVar1 + 0x1c) * local_c4) + local_44);
              }
              else if (*(int *)(iVar1 + 8) == iVar9) {
                *pfVar4 = -(*(float *)(iVar1 + 0x40) *
                            ((*(float *)(iVar1 + 0x24) * local_c4 -
                             *(float *)(iVar1 + 0x20) * local_c0) + local_40) +
                           *(float *)(iVar1 + 0x3c) *
                           ((*(float *)(iVar1 + 0x20) * local_bc -
                            *(float *)(iVar1 + 0x28) * local_c4) + local_44) +
                           *(float *)(iVar1 + 0x38) *
                           ((*(float *)(iVar1 + 0x28) * local_c0 -
                            *(float *)(iVar1 + 0x24) * local_bc) + local_48));
              }
              else {
                *pfVar4 = 0.0;
              }
              if (iVar3 != 0) {
                iVar1 = *piVar5;
                if (*(int *)(iVar1 + 4) == iVar3) {
                  fVar2 = *(float *)(iVar1 + 0x2c) *
                          ((*(float *)(iVar1 + 0x1c) * local_90 -
                           *(float *)(iVar1 + 0x18) * local_8c) + local_30) +
                          *(float *)(iVar1 + 0x34) *
                          ((*(float *)(iVar1 + 0x18) * local_94 -
                           *(float *)(iVar1 + 0x14) * local_90) + local_28) +
                          *(float *)(iVar1 + 0x30) *
                          ((*(float *)(iVar1 + 0x14) * local_8c -
                           *(float *)(iVar1 + 0x1c) * local_94) + local_2c) + *pfVar4;
                }
                else {
                  if (*(int *)(iVar1 + 8) != iVar3) goto LAB_004bf981;
                  fVar2 = *pfVar4 - (*(float *)(iVar1 + 0x40) *
                                     ((*(float *)(iVar1 + 0x24) * local_94 -
                                      *(float *)(iVar1 + 0x20) * local_90) + local_28) +
                                    *(float *)(iVar1 + 0x3c) *
                                    ((*(float *)(iVar1 + 0x20) * local_8c -
                                     *(float *)(iVar1 + 0x28) * local_94) + local_2c) +
                                    *(float *)(iVar1 + 0x38) *
                                    ((*(float *)(iVar1 + 0x28) * local_90 -
                                     *(float *)(iVar1 + 0x24) * local_8c) + local_30));
                }
                *pfVar4 = fVar2;
              }
LAB_004bf981:
              if (iVar8 == 0) goto LAB_004bfbea;
              iVar1 = *piVar5;
              if (*(int *)(iVar1 + 4) == iVar8) {
                fVar2 = *pfVar4 - (*(float *)(iVar1 + 0x2c) *
                                   ((*(float *)(iVar1 + 0x1c) * local_9c -
                                    *(float *)(iVar1 + 0x18) * local_98) + local_18) +
                                  *(float *)(iVar1 + 0x34) *
                                  ((*(float *)(iVar1 + 0x18) * local_a0 -
                                   *(float *)(iVar1 + 0x14) * local_9c) + local_10) +
                                  *(float *)(iVar1 + 0x30) *
                                  ((*(float *)(iVar1 + 0x14) * local_98 -
                                   *(float *)(iVar1 + 0x1c) * local_a0) + local_14));
LAB_004bfaae:
                *pfVar4 = fVar2;
              }
              else if (*(int *)(iVar1 + 8) == iVar8) {
                fVar2 = *(float *)(iVar1 + 0x40) *
                        ((*(float *)(iVar1 + 0x24) * local_a0 - *(float *)(iVar1 + 0x20) * local_9c)
                        + local_10) +
                        *(float *)(iVar1 + 0x3c) *
                        ((*(float *)(iVar1 + 0x20) * local_98 - *(float *)(iVar1 + 0x28) * local_a0)
                        + local_14) +
                        *(float *)(iVar1 + 0x38) *
                        ((*(float *)(iVar1 + 0x28) * local_9c - *(float *)(iVar1 + 0x24) * local_98)
                        + local_18) + *pfVar4;
                goto LAB_004bfaae;
              }
              if (local_6c != 0) {
                iVar1 = *piVar5;
                if (*(int *)(iVar1 + 4) == local_6c) {
                  fVar2 = *pfVar4 - (*(float *)(iVar1 + 0x2c) *
                                     ((*(float *)(iVar1 + 0x1c) * local_b4 -
                                      *(float *)(iVar1 + 0x18) * local_b0) + local_3c) +
                                    *(float *)(iVar1 + 0x34) *
                                    ((*(float *)(iVar1 + 0x18) * local_b8 -
                                     *(float *)(iVar1 + 0x14) * local_b4) + local_34) +
                                    *(float *)(iVar1 + 0x30) *
                                    ((*(float *)(iVar1 + 0x14) * local_b0 -
                                     *(float *)(iVar1 + 0x1c) * local_b8) + local_38));
                }
                else {
                  if (*(int *)(iVar1 + 8) != local_6c) goto LAB_004bfbea;
                  fVar2 = *(float *)(iVar1 + 0x40) *
                          ((*(float *)(iVar1 + 0x24) * local_b8 -
                           *(float *)(iVar1 + 0x20) * local_b4) + local_34) +
                          *(float *)(iVar1 + 0x3c) *
                          ((*(float *)(iVar1 + 0x20) * local_b0 -
                           *(float *)(iVar1 + 0x28) * local_b8) + local_38) +
                          *(float *)(iVar1 + 0x38) *
                          ((*(float *)(iVar1 + 0x28) * local_b4 -
                           *(float *)(iVar1 + 0x24) * local_b0) + local_3c) + *pfVar4;
                }
                *pfVar4 = fVar2;
              }
            }
LAB_004bfbea:
            piVar5 = piVar5 + 1;
            *pfVar6 = *pfVar4;
            pfVar4 = pfVar4 + local_78;
            pfVar6 = pfVar6 + 1;
            local_7c = local_7c + -1;
            iVar7 = local_74;
          } while (local_7c != 0);
        }
      }
      else {
        FUN_004be4b0(&local_d0, &local_60, (const float*)(iVar1 + 0x14), (const float*)(iVar1 + 0x2c), (tPhysics_object*)iVar8);
        if (*(int *)(iVar8 + 0x1e4) == 0) {
          local_e0 = 0;
        }
        else {
          local_e0 = (int)(uintptr_t)FUN_004becf0(&local_d0, &local_60, &local_ac, &local_24, (tPhysics_object*)iVar8);
        }
        iVar8 = 0;
        local_68 = 0;
        if ((iVar9 != 0) &&
           (FUN_004be4b0(&local_dc, &local_54, (const float*)(iVar1 + 0x20), (const float*)(iVar1 + 0x38), (tPhysics_object*)iVar9), *(int *)(iVar9 + 0x1e4) != 0)) {
          iVar8 = (int)(uintptr_t)FUN_004becf0(&local_dc, &local_54, &local_88, &local_c, (tPhysics_object*)iVar9);
          local_68 = iVar8;
        }
        if (-1 < iVar7) {
          pfVar6 = (float *)(local_64 + local_74 * local_78 * 4);
          pfVar4 = (float *)(local_64 + local_74 * 4);
          local_70 = local_74 + 1;
          piVar5 = param_3;
          do {
            *pfVar4 = 0.0;
            iVar1 = *piVar5;
            if (*(int *)(iVar1 + 0x5c) == 0xb) {
              if (*(int *)(iVar1 + 4) == local_7c) {
                fVar2 = *(float *)(iVar1 + 0x34) * local_c8 +
                        *(float *)(iVar1 + 0x30) * local_cc + *(float *)(iVar1 + 0x2c) * local_d0;
LAB_004bfd67:
                *pfVar4 = fVar2;
              }
              else if (*(int *)(iVar1 + 8) == local_7c) {
                fVar2 = -(*(float *)(iVar1 + 0x3c) * local_cc +
                         *(float *)(iVar1 + 0x38) * local_d0 + *(float *)(iVar1 + 0x40) * local_c8);
                goto LAB_004bfd67;
              }
              if (iVar9 != 0) {
                if (*(int *)(iVar1 + 4) == iVar9) {
                  fVar2 = *pfVar4 - (*(float *)(iVar1 + 0x34) * local_d4 +
                                    *(float *)(iVar1 + 0x30) * local_d8 +
                                    *(float *)(iVar1 + 0x2c) * local_dc);
                }
                else {
                  if (*(int *)(iVar1 + 8) != iVar9) goto LAB_004bfdc5;
                  fVar2 = *(float *)(iVar1 + 0x3c) * local_d8 +
                          *(float *)(iVar1 + 0x38) * local_dc + *(float *)(iVar1 + 0x40) * local_d4
                          + *pfVar4;
                }
                *pfVar4 = fVar2;
              }
LAB_004bfdc5:
              if (local_e0 != 0) {
                if (*(int *)(iVar1 + 4) == local_e0) {
                  fVar2 = *(float *)(iVar1 + 0x34) * local_a4 +
                          *(float *)(iVar1 + 0x30) * local_a8 + *(float *)(iVar1 + 0x2c) * local_ac
                          + *pfVar4;
                }
                else {
                  if (*(int *)(iVar1 + 8) != local_e0) goto LAB_004bfe25;
                  fVar2 = *pfVar4 - (*(float *)(iVar1 + 0x3c) * local_a8 +
                                    *(float *)(iVar1 + 0x38) * local_ac +
                                    *(float *)(iVar1 + 0x40) * local_a4);
                }
                *pfVar4 = fVar2;
              }
LAB_004bfe25:
              if (iVar8 != 0) {
                if (*(int *)(iVar1 + 4) == iVar8) {
                  *pfVar4 = *pfVar4 - (*(float *)(iVar1 + 0x34) * local_80 +
                                      *(float *)(iVar1 + 0x30) * local_84 +
                                      *(float *)(iVar1 + 0x2c) * local_88);
                }
                else if (*(int *)(iVar1 + 8) == iVar8) {
                  *pfVar4 = *(float *)(iVar1 + 0x3c) * local_84 +
                            *(float *)(iVar1 + 0x38) * local_88 +
                            *(float *)(iVar1 + 0x40) * local_80 + *pfVar4;
                }
              }
            }
            else {
              if (*(int *)(iVar1 + 4) == local_7c) {
                fVar2 = *(float *)(iVar1 + 0x30) *
                        ((*(float *)(iVar1 + 0x14) * local_c8 - *(float *)(iVar1 + 0x1c) * local_d0)
                        + local_5c) +
                        *(float *)(iVar1 + 0x34) *
                        ((*(float *)(iVar1 + 0x18) * local_d0 - *(float *)(iVar1 + 0x14) * local_cc)
                        + local_58) +
                        *(float *)(iVar1 + 0x2c) *
                        ((*(float *)(iVar1 + 0x1c) * local_cc - *(float *)(iVar1 + 0x18) * local_c8)
                        + local_60);
LAB_004bffd0:
                *pfVar4 = fVar2;
              }
              else if ((*(int *)(iVar1 + 8) == local_7c) && (*(int *)(iVar1 + 0x5c) != 0xe)) {
                fVar2 = -(*(float *)(iVar1 + 0x38) *
                          ((*(float *)(iVar1 + 0x28) * local_cc -
                           *(float *)(iVar1 + 0x24) * local_c8) + local_60) +
                         *(float *)(iVar1 + 0x3c) *
                         ((*(float *)(iVar1 + 0x20) * local_c8 - *(float *)(iVar1 + 0x28) * local_d0
                          ) + local_5c) +
                         *(float *)(iVar1 + 0x40) *
                         ((*(float *)(iVar1 + 0x24) * local_d0 - *(float *)(iVar1 + 0x20) * local_cc
                          ) + local_58));
                goto LAB_004bffd0;
              }
              if (iVar9 != 0) {
                if (*(int *)(iVar1 + 4) == iVar9) {
                  fVar2 = *pfVar4 - (*(float *)(iVar1 + 0x30) *
                                     ((*(float *)(iVar1 + 0x14) * local_d4 -
                                      *(float *)(iVar1 + 0x1c) * local_dc) + local_50) +
                                    *(float *)(iVar1 + 0x34) *
                                    ((*(float *)(iVar1 + 0x18) * local_dc -
                                     *(float *)(iVar1 + 0x14) * local_d8) + local_4c) +
                                    *(float *)(iVar1 + 0x2c) *
                                    ((*(float *)(iVar1 + 0x1c) * local_d8 -
                                     *(float *)(iVar1 + 0x18) * local_d4) + local_54));
                }
                else {
                  if ((*(int *)(iVar1 + 8) != iVar9) || (*(int *)(iVar1 + 0x5c) == 0xe))
                  goto LAB_004c0107;
                  fVar2 = *(float *)(iVar1 + 0x38) *
                          ((*(float *)(iVar1 + 0x28) * local_d8 -
                           *(float *)(iVar1 + 0x24) * local_d4) + local_54) +
                          *(float *)(iVar1 + 0x3c) *
                          ((*(float *)(iVar1 + 0x20) * local_d4 -
                           *(float *)(iVar1 + 0x28) * local_dc) + local_50) +
                          *(float *)(iVar1 + 0x40) *
                          ((*(float *)(iVar1 + 0x24) * local_dc -
                           *(float *)(iVar1 + 0x20) * local_d8) + local_4c) + *pfVar4;
                }
                *pfVar4 = fVar2;
              }
LAB_004c0107:
              if (local_e0 != 0) {
                if (*(int *)(iVar1 + 4) == local_e0) {
                  fVar2 = *(float *)(iVar1 + 0x30) *
                          ((*(float *)(iVar1 + 0x14) * local_a4 -
                           *(float *)(iVar1 + 0x1c) * local_ac) + local_20) +
                          *(float *)(iVar1 + 0x34) *
                          ((*(float *)(iVar1 + 0x18) * local_ac -
                           *(float *)(iVar1 + 0x14) * local_a8) + local_1c) +
                          *(float *)(iVar1 + 0x2c) *
                          ((*(float *)(iVar1 + 0x1c) * local_a8 -
                           *(float *)(iVar1 + 0x18) * local_a4) + local_24) + *pfVar4;
                }
                else {
                  if (*(int *)(iVar1 + 8) != local_e0) goto LAB_004c0234;
                  fVar2 = *pfVar4 - (*(float *)(iVar1 + 0x38) *
                                     ((*(float *)(iVar1 + 0x28) * local_a8 -
                                      *(float *)(iVar1 + 0x24) * local_a4) + local_24) +
                                    *(float *)(iVar1 + 0x3c) *
                                    ((*(float *)(iVar1 + 0x20) * local_a4 -
                                     *(float *)(iVar1 + 0x28) * local_ac) + local_20) +
                                    *(float *)(iVar1 + 0x40) *
                                    ((*(float *)(iVar1 + 0x24) * local_ac -
                                     *(float *)(iVar1 + 0x20) * local_a8) + local_1c));
                }
                *pfVar4 = fVar2;
              }
LAB_004c0234:
              iVar8 = local_68;
              if (local_68 != 0) {
                if (*(int *)(iVar1 + 4) == local_68) {
                  fVar2 = *pfVar4 - (*(float *)(iVar1 + 0x30) *
                                     ((*(float *)(iVar1 + 0x14) * local_80 -
                                      *(float *)(iVar1 + 0x1c) * local_88) + local_8) +
                                    *(float *)(iVar1 + 0x34) *
                                    ((*(float *)(iVar1 + 0x18) * local_88 -
                                     *(float *)(iVar1 + 0x14) * local_84) + local_4) +
                                    *(float *)(iVar1 + 0x2c) *
                                    ((*(float *)(iVar1 + 0x1c) * local_84 -
                                     *(float *)(iVar1 + 0x18) * local_80) + local_c));
                }
                else {
                  if (*(int *)(iVar1 + 8) != local_68) goto LAB_004c0388;
                  fVar2 = *(float *)(iVar1 + 0x38) *
                          ((*(float *)(iVar1 + 0x28) * local_84 -
                           *(float *)(iVar1 + 0x24) * local_80) + local_c) +
                          *(float *)(iVar1 + 0x3c) *
                          ((*(float *)(iVar1 + 0x20) * local_80 -
                           *(float *)(iVar1 + 0x28) * local_88) + local_8) +
                          *(float *)(iVar1 + 0x40) *
                          ((*(float *)(iVar1 + 0x24) * local_88 -
                           *(float *)(iVar1 + 0x20) * local_84) + local_4) + *pfVar4;
                }
                *pfVar4 = fVar2;
              }
            }
LAB_004c0388:
            *pfVar6 = *pfVar4;
            piVar5 = piVar5 + 1;
            pfVar6 = pfVar6 + 1;
            pfVar4 = pfVar4 + local_78;
            local_70 = local_70 + -1;
            iVar7 = local_74;
          } while (local_70 != 0);
        }
      }
      local_74 = iVar7 + 1;
    } while (local_74 < param_4);
  }
  return;
}

#undef local_e0
#undef local_dc
#undef local_d8
#undef local_d4
#undef local_d0
#undef local_cc
#undef local_c8
#undef local_c4
#undef local_c0
#undef local_bc
#undef local_b8
#undef local_b4
#undef local_b0
#undef local_ac
#undef local_a8
#undef local_a4
#undef local_a0
#undef local_9c
#undef local_98
#undef local_94
#undef local_90
#undef local_8c
#undef local_88
#undef local_84
#undef local_80
#undef local_7c
#undef local_78
#undef local_74
#undef local_70
#undef local_6c
#undef local_68
#undef local_64
#undef local_60
#undef local_5c
#undef local_58
#undef local_54
#undef local_50
#undef local_4c
#undef local_48
#undef local_44
#undef local_40
#undef local_3c
#undef local_38
#undef local_34
#undef local_30
#undef local_2c
#undef local_28
#undef local_24
#undef local_20
#undef local_1c
#undef local_18
#undef local_14
#undef local_10
#undef local_c
#undef local_8
#undef local_4
#pragma auto_inline(on)

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

// GLOBAL: CARMA2_HW 0x006923d8
tPhysics_object* gReset_man;

void C2_HOOK_FASTCALL ResetMan2(tPhysics_object* pObject);

// FUNCTION: CARMA2_HW 0x004b5ab0
void C2_HOOK_FASTCALL ResetMan(void) {

    if (gReset_man == NULL) {
        return;
    }
    BrMatrix34Translate(&gReset_man->transform_matrix, -54.0f, 7.1325f, -155.0f);
    gReset_man->disable_move_rotate = 1;
    BrVector3SetFloat(&gReset_man->v, 0.f, 0.f, 0.f);
    BrVector3SetFloat(&gReset_man->omega, 0.f, 0.f, 0.f);
    BrVector3SetFloat(&gReset_man->rotate_omega, 0.f, 0.f, 0.f);
    BrVector3SetFloat(&gReset_man->velocity_car_space, 0.f, 0.f, 0.f);
    ResetMan2(gReset_man->child);
    PositionChildren(gReset_man);
}

// UNLABELED: CARMA2_HW 0x004b5c00
void C2_HOOK_FASTCALL ResetMan2(tPhysics_object* pObject) {

    while (pObject != NULL) {
        BrMatrix34Translate(&pObject->transform_matrix, 0.f, 0.f, 0.f);
        BrVector3SetFloat(&pObject->v, 0.f, 0.f, 0.f);
        BrVector3SetFloat(&pObject->omega, 0.f, 0.f, 0.f);
        BrVector3SetFloat(&pObject->rotate_omega, 0.f, 0.f, 0.f);
        BrVector3SetFloat(&pObject->velocity_car_space, 0.f, 0.f, 0.f);
        pObject->disable_move_rotate = 1;
        if (pObject->child != NULL) {
            ResetMan2(pObject->child);
        }
        pObject = pObject->next;
    }
}

// FUNCTION: CARMA2_HW 0x004b5ca0
void C2_HOOK_FASTCALL InitPhysics(void) {
    PhysicsSetErrorHandler(DoPhysicsError);
    InitPhysicsWorkspace(gTemporary_physics_render_buffer, (int)sizeof(gTemporary_physics_render_buffer));
}

// PointOutOfSightNotAR

// CalcCollObjIDFromCarSpec

// CalcCollObjID

// GetCarSpecFromCollObjID

// GetCollObjFromID

// FUNCTION: CARMA2_HW 0x004b7220
void C2_HOOK_FASTCALL PHILSetObjectMatrix(tPhysics_object* pObject, br_matrix34* pMat) {

    BrMatrix34Copy(&pObject->transform_matrix, pMat);
    BrMatrix34Copy(&pObject->actor->t.t.mat, pMat);
}

/* Physics helpers ported from the reference decompilation. */
#pragma auto_inline(off)
void C2_HOOK_FASTCALL FUN_004b73a0(tPhysics_object* pObject, const br_matrix34* pParent_matrix, const br_matrix34* pRoot_matrix);

// FUNCTION: CARMA2_HW 0x0041e570
void C2_HOOK_FASTCALL FUN_0041e570(void) {

}

// FUNCTION: CARMA2_HW 0x00421fd0
tCollision_shape_polyhedron_data* C2_HOOK_FASTCALL FUN_00421fd0(int pCount_points) {
    tCollision_shape_polyhedron_data* result;
    int count_minus_two;
    int count_plus_two;
    int count_edges;

    count_minus_two = pCount_points - 2;
    count_plus_two = pCount_points + 2;
    count_edges = count_minus_two * 3;
#ifdef __vita__
    result = BrMemAllocate(sizeof(*result) + pCount_points * sizeof(br_vector3)
        + count_edges * sizeof(tPolyhedron_edge_indexes) + 2 * count_minus_two * sizeof(br_vector4) + 3, 0x80);
#else
    result = BrMemAllocate(2 * (count_edges + count_minus_two * 16 + count_plus_two * 6), 0x80);
#endif
    if (result == NULL) {
        return NULL;
    }
    result->points = (br_vector3*)((tU8*)result + sizeof(*result));
    result->count_points = 0;
    result->count_edges = 0;
    result->edges = (tPolyhedron_edge_indexes*)(result->points + pCount_points);
    result->count_planes = 0;
    result->planes = (br_vector4*)(result->edges + count_edges);
#ifdef __vita__
    result->planes = (br_vector4*)(((uintptr_t)result->planes + 3) & ~(uintptr_t)3);
#endif
    return result;
}

// FUNCTION: CARMA2_HW 0x00422400
tPhysicsError C2_HOOK_FASTCALL FUN_00422400(tCollision_shape_polyhedron_data* pPolyhedron, int* pVertex_indices, br_model* pModel, int pCount_vertices) {
    int i;
    int result;

    if (pCount_vertices < pModel->nvertices) {
        result = FUN_00422030(pModel, pCount_vertices, pVertex_indices);
        if (result != ePhysicsError_Ok) {
            return result;
        }
        pPolyhedron->count_points = pCount_vertices;
        if (pCount_vertices > 0) {
            for (i = 0; i < pCount_vertices; i++) {
                pPolyhedron->points[i].v[0] = pModel->vertices[pVertex_indices[i]].p.v[0];
                pPolyhedron->points[i].v[1] = pModel->vertices[pVertex_indices[i]].p.v[1];
                pPolyhedron->points[i].v[2] = pModel->vertices[pVertex_indices[i]].p.v[2];
            }
            return ConvexHull3D(pPolyhedron);
        }
    } else {
        pPolyhedron->count_points = pModel->nvertices;
        for (i = 0; i < pModel->nvertices; i++) {
            pVertex_indices[i] = i;
            pPolyhedron->points[i].v[0] = pModel->vertices[i].p.v[0];
            pPolyhedron->points[i].v[1] = pModel->vertices[i].p.v[1];
            pPolyhedron->points[i].v[2] = pModel->vertices[i].p.v[2];
        }
    }
    return ConvexHull3D(pPolyhedron);
}

// FUNCTION: CARMA2_HW 0x00422500
int C2_HOOK_FASTCALL FUN_00422500(tCollision_shape_polyhedron_data* pPolyhedron, br_model* pModel, int pMin_point_count, int pMax_point_count, int* pVertex_indices) {
    int result;
    int count_points;
    int point_count;

    if (pModel == NULL) {
        return 5;
    }
    if (pMax_point_count < 3) {
        return 6;
    }
    if (pMax_point_count > (int)pModel->nvertices) {
        pMax_point_count = pModel->nvertices;
    }
    if (pMin_point_count < 3) {
        pMin_point_count = 3;
    } else if (pMin_point_count > pMax_point_count) {
        pMin_point_count = pMax_point_count;
    }
    point_count = pMax_point_count;
    result = FUN_00422400(pPolyhedron, pVertex_indices, pModel, point_count);
    count_points = pPolyhedron->count_points;
    while (count_points != 0
            && ((result == 0 || result == 2)
                && (point_count = point_count * pMin_point_count / count_points, count_points != pMin_point_count))
            && point_count < pMax_point_count) {
        result = FUN_00422400(pPolyhedron, pVertex_indices, pModel, point_count);
        count_points = pPolyhedron->count_points;
    }
    return result;
}

// FUNCTION: CARMA2_HW 0x00428cf0
int C2_HOOK_FASTCALL CopyPhysicsJointData(tPhysics_joint_data* pDestination, int pDestination_index, const tPhysics_joint_data* pSource, int pCount) {
    int i;

    for (i = 0; i < pCount && pDestination_index < 40; i++) {
        if (pSource[i].field_0x5c != 9) {
            memcpy(&pDestination[pDestination_index], &pSource[i], sizeof(*pSource));
            pDestination_index++;
        }
    }
    return pDestination_index;
}

// C2PORT-NONMATCHING: best 28.52% | difftest PASS 2000 trials (spec difftest/specs/004bc190_FUN_004bc190.json) | reason x87 scheduling, register allocation, and stack layout
// FUNCTION: CARMA2_HW 0x004bc190
void C2_HOOK_FASTCALL FUN_004bc190(int* pObject_list, float* pVectors, int pCount, int pUnused) {
  int *piVar1;
  float *pfVar2;
  float *pfVar3;
  double fVar4;
  float fVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  undefined4 *puVar9;
  float fVar10;
  float fVar11;
  float fVar12;
  float fVar13;
  int iVar14;
  float *pfVar15;
  float *pfVar16;
  int *piVar17;
  float *pfVar18;
  int *piVar19;
  int *local_a0;
  float *local_9c;
  int local_98;
  int local_90;
  br_vector3 local_60;
  float local_48;
  br_vector3 local_3c;
  br_vector3 local_24;
  float local_20;
  br_matrix34 local_matrix;

  (void)pUnused;
  local_98 = 0;
  local_90 = 0;
  if (0 < pCount) {
    do {
      piVar1 = pObject_list + local_98;
      iVar6 = *(int *)(pObject_list[local_98] + 4);
      iVar14 = local_98;
      if ((*(int *)(iVar6 + 0x224) != 0) && (local_98 < pCount)) {
        pfVar2 = pVectors + local_98;
        pfVar3 = pVectors + 2 + local_98;
        piVar17 = pObject_list + 2 + local_98;
        iVar14 = local_98 * 4 + 4;
        pfVar15 = pVectors + local_98 + 1;
        piVar19 = pObject_list + local_98 + 1;
        pfVar16 = pfVar15;
        pfVar18 = pfVar2;
        local_a0 = piVar1;
        local_9c = pfVar3;
        do {
          iVar14 = *local_a0;
          if ((*(int *)(iVar14 + 8) == iVar6) &&
             (*(float *)(iVar6 + 0x1c8) * *(float *)(iVar6 + 0x1c8) +
              *(float *)(iVar6 + 0x1c4) * *(float *)(iVar6 + 0x1c4) +
              *(float *)(iVar6 + 0x1c0) * *(float *)(iVar6 + 0x1c0) <
              *(float *)(iVar6 + 0x7c) * *(float *)(iVar6 + 0x7c) +
              *(float *)(iVar6 + 0x78) * *(float *)(iVar6 + 0x78) +
              *(float *)(iVar6 + 0x74) * *(float *)(iVar6 + 0x74))) {
            iVar7 = *(int *)(iVar14 + 4);
            fVar10 = *(float *)(*(int *)(iVar6 + 0x188) + 8) -
                     *(float *)(*(int *)(iVar7 + 0x188) + 0x14);
            fVar11 = *(float *)(*(int *)(iVar6 + 0x188) + 0xc) -
                     *(float *)(*(int *)(iVar7 + 0x188) + 0x18);
            fVar12 = *(float *)(*(int *)(iVar6 + 0x188) + 0x10) -
                     *(float *)(*(int *)(iVar7 + 0x188) + 0x1c);
            fVar4 = sqrt(fVar12 * fVar12 + fVar11 * fVar11 + fVar10 * fVar10);
            fVar10 = fVar10 / fVar4;
            fVar11 = fVar11 / fVar4;
            fVar12 = fVar12 / fVar4;
            fVar4 = *(float *)(iVar6 + 4) * gDOUBLE_0058a7f0;
            if (fVar4 <= *pfVar3 * fVar12 + *pfVar2 * fVar10 + *pfVar15 * fVar11) {
              iVar7 = *piVar19;
              iVar8 = *piVar17;
              if (fVar4 <= (*(float *)(iVar8 + 0x40) * fVar12 +
                           *(float *)(iVar8 + 0x38) * fVar10 + *(float *)(iVar8 + 0x3c) * fVar11) *
                           *local_9c +
                           *pfVar16 *
                           (*(float *)(iVar7 + 0x40) * fVar12 +
                           *(float *)(iVar7 + 0x38) * fVar10 + *(float *)(iVar7 + 0x3c) * fVar11) +
                           *pfVar18 *
                           (*(float *)(iVar14 + 0x3c) * fVar11 +
                           *(float *)(iVar14 + 0x38) * fVar10 + *(float *)(iVar14 + 0x40) * fVar12))
              {
                local_20 = sqrt(*(float *)(iVar6 + 0x7c) * *(float *)(iVar6 + 0x7c) +
                                 *(float *)(iVar6 + 0x78) * *(float *)(iVar6 + 0x78) +
                                 *(float *)(iVar6 + 0x74) * *(float *)(iVar6 + 0x74));
                local_3c.v[0] = *(float *)(iVar6 + 0x74) / local_20;
                local_3c.v[1] = *(float *)(iVar6 + 0x78) / local_20;
                local_3c.v[2] = *(float *)(iVar6 + 0x7c) / local_20;
                BrMatrix34Rotate(&local_matrix, (br_angle)(local_20 * gFLOAT_0058a7f8 * gDOUBLE_0058a770), &local_3c);
                local_24.v[0] = fVar10;
                local_24.v[1] = fVar11;
                local_24.v[2] = fVar12;
                BrMatrix34ApplyV(&local_60, &local_24, &local_matrix);
                fVar4 = *pfVar2;
                fVar5 = *pfVar15;
                local_48 = *pfVar3;
                fVar13 = sqrt(local_48 * local_48 + fVar5 * fVar5 + fVar4 * fVar4);
                if (fVar13 <= gFLOAT_0058a7fc) {
                  local_48 = 0.0;
                  fVar4 = gFLOAT_0058a800;
                  fVar5 = gFLOAT_0058a768;
                }
                else {
                  fVar13 = (float)gDOUBLE_0058a780 / fVar13;
                  local_48 = fVar13 * local_48;
                  fVar4 = fVar13 * fVar4;
                  fVar5 = fVar13 * fVar5;
                }
                if ((fVar10 * fVar5 - fVar11 * fVar4) * (fVar5 * local_60.v[0] - fVar4 * local_60.v[1]) +
                    (fVar12 * fVar4 - fVar10 * local_48) * (fVar4 * local_60.v[2] - local_48 * local_60.v[0])
                    + (fVar11 * local_48 - fVar12 * fVar5) *
                      (local_48 * local_60.v[1] - fVar5 * local_60.v[2]) < gFLOAT_0058a768) {
                  fVar10 = *(float *)(iVar6 + 0x78) * gFLOAT_0058a804;
                  fVar4 = *(float *)(iVar6 + 0x7c) * gFLOAT_0058a804;
                  *(float *)(iVar6 + 0x74) = *(float *)(iVar6 + 0x74) * gFLOAT_0058a804;
                  *(float *)(iVar6 + 0x78) = fVar10;
                  *(float *)(iVar6 + 0x7c) = fVar4;
                  FUN_004bc8e0((tPhysics_object*)iVar6, 25.0f);
                }
              }
            }
          }
          iVar14 = *local_a0;
          puVar9 = *(undefined4 **)(*(int *)(iVar14 + 4) + 0x188);
          if (puVar9 != (undefined4 *)0x0) {
            switch(*puVar9) {
            case 1:
              pfVar18 = pfVar18 + 5;
              local_98 = local_98 + 5;
              piVar19 = piVar19 + 5;
              local_9c = local_9c + 5;
              pfVar16 = pfVar16 + 5;
              piVar17 = piVar17 + 5;
              local_a0 = local_a0 + 5;
              break;
            case 2:
              pfVar18 = pfVar18 + 4;
              local_98 = local_98 + 4;
              piVar19 = piVar19 + 4;
              local_9c = local_9c + 4;
              pfVar16 = pfVar16 + 4;
              piVar17 = piVar17 + 4;
              local_a0 = local_a0 + 4;
              break;
            case 3:
              pfVar18 = pfVar18 + 3;
              local_98 = local_98 + 3;
              piVar19 = piVar19 + 3;
              local_9c = local_9c + 3;
              pfVar16 = pfVar16 + 3;
              piVar17 = piVar17 + 3;
              local_a0 = local_a0 + 3;
              break;
            case 4:
              pfVar18 = pfVar18 + 2;
              local_98 = local_98 + 2;
              piVar19 = piVar19 + 2;
              local_9c = local_9c + 2;
              pfVar16 = pfVar16 + 2;
              piVar17 = piVar17 + 2;
              local_a0 = local_a0 + 2;
              break;
            default:
              ((void (C2_HOOK_FASTCALL *)(tPhysicsError, const char*))PhysicsError)(ePhysicsError_UnknownHingeType, gEmpty_string_00676920);
            }
          }
          puVar9 = *(undefined4 **)(*(int *)(iVar14 + 4) + 0x18c);
          if (puVar9 != (undefined4 *)0x0) {
            switch(*puVar9) {
            case 1:
              local_98 = local_98 + 5;
              iVar14 = 0x14;
              break;
            case 2:
              local_98 = local_98 + 4;
              iVar14 = 0x10;
              break;
            case 3:
              local_98 = local_98 + 3;
              iVar14 = 0xc;
              break;
            default:
              ((void (C2_HOOK_FASTCALL *)(tPhysicsError, const char*))PhysicsError)(ePhysicsError_UnknownHingeType, gEmpty_string_00676920);
              goto LAB_004bc7a6;
            case 5:
              local_98 = local_98 + 2;
              iVar14 = 8;
            }
            pfVar18 = (float *)((int)pfVar18 + iVar14);
            local_9c = (float *)((int)local_9c + iVar14);
            piVar19 = (int *)((int)piVar19 + iVar14);
            pfVar16 = (float *)((int)pfVar16 + iVar14);
            piVar17 = (int *)((int)piVar17 + iVar14);
            local_a0 = (int *)((int)local_a0 + iVar14);
          }
LAB_004bc7a6:
          iVar14 = local_90;
        } while (local_98 < pCount);
      }
      iVar6 = *piVar1;
      puVar9 = *(undefined4 **)(*(int *)(iVar6 + 4) + 0x188);
      if (puVar9 != (undefined4 *)0x0) {
        switch(*puVar9) {
        case 1:
          iVar14 = iVar14 + 5;
          local_90 = iVar14;
          break;
        case 2:
          iVar14 = iVar14 + 4;
          local_90 = iVar14;
          break;
        case 3:
          iVar14 = iVar14 + 3;
          local_90 = iVar14;
          break;
        case 4:
          iVar14 = iVar14 + 2;
          local_90 = iVar14;
          break;
        default:
          ((void (C2_HOOK_FASTCALL *)(tPhysicsError, const char*))PhysicsError)(ePhysicsError_UnknownHingeType, gEmpty_string_00676920);
          iVar14 = local_90;
        }
      }
      puVar9 = *(undefined4 **)(*(int *)(iVar6 + 4) + 0x18c);
      local_98 = iVar14;
      if (puVar9 != (undefined4 *)0x0) {
        switch(*puVar9) {
        case 1:
          local_98 = iVar14 + 5;
          local_90 = local_98;
          break;
        case 2:
          local_98 = iVar14 + 4;
          local_90 = local_98;
          break;
        case 3:
          local_98 = iVar14 + 3;
          local_90 = local_98;
          break;
        default:
          ((void (C2_HOOK_FASTCALL *)(tPhysicsError, const char*))PhysicsError)(ePhysicsError_UnknownHingeType, gEmpty_string_00676920);
          local_98 = local_90;
          break;
        case 5:
          local_98 = iVar14 + 2;
          local_90 = local_98;
        }
      }
    } while (local_98 < pCount);
  }
  return;
}

#pragma auto_inline(on)
// FUNCTION: CARMA2_HW 0x004b7250
float C2_HOOK_FASTCALL PhysicsObjectWeightedSquareSum(tPhysics_object* pObject) {
    float sum;
    int i;

    sum = gFLOAT_0058a768;
    for (i = 0; i < 3; i++) {
        sum += BR_MUL(pObject->M, BR_SQR(pObject->v.v[i])) + BR_MUL(pObject->I.v[i], BR_SQR(pObject->omega.v[i]));
    }
    return sum;
}

// FUNCTION: CARMA2_HW 0x004b7290
float C2_HOOK_FASTCALL PhysicsObjectsWeightedSquareSum(tPhysics_object* pObject) {
    float sum;
    float object_sum;

    sum = 0.0f;
    while (pObject != NULL) {
        object_sum = PhysicsObjectWeightedSquareSum(pObject);
        sum += object_sum;
        sum += PhysicsObjectsWeightedSquareSum(pObject->child);
        pObject = pObject->next;
    }
    return sum;
}

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x004b7310
void C2_HOOK_FASTCALL FUN_004b7310(tPhysics_object* pObject) {
    tPhysics_object* parent;
    br_matrix34 inverse;
    br_matrix34 normalised;

    parent = pObject->parent;
    pObject->flags |= 0x400;
    while (parent != NULL && (parent->flags & 0x400) != 0) {
        parent = parent->parent;
    }
    BrMatrix34LPInverse(&inverse, &parent->transform_matrix);
    BrMatrix34LPNormalise(&normalised, &inverse);
    BrMatrix34Mul(&pObject->field_0x1e8, &pObject->transform_matrix, &normalised);
    FUN_004b73a0(pObject->child, &pObject->transform_matrix, &parent->transform_matrix);
}

// FUNCTION: CARMA2_HW 0x004b73a0
void C2_HOOK_FASTCALL FUN_004b73a0(tPhysics_object* pObject, const br_matrix34* pParent_matrix, const br_matrix34* pRoot_matrix) {
    br_matrix34 inverse;
    br_matrix34 normalised;
    br_vector3 transformed;
    tPhysics_joint* joint;
    int i;

    for (; pObject != NULL; pObject = pObject->next) {
        if ((pObject->flags & 0x400) != 0) {
            BrMatrix34LPInverse(&inverse, &pObject->parent->transform_matrix);
            BrMatrix34LPNormalise(&normalised, &inverse);
            BrMatrix34Mul(&inverse, &pObject->transform_matrix, &normalised);
            BrMatrix34Mul(&pObject->field_0x1e8, &inverse, &pObject->parent->field_0x1e8);
        }
        joint = pObject->physics_joint1;
        if (joint != NULL) {
            if ((pObject->flags & 0x400) != 0) {
                FUN_004b73a0(pObject->child, pParent_matrix, pRoot_matrix);
            } else {
                BrMatrix34ApplyP(&transformed, &joint->field_0x14, pParent_matrix);
                DrMatrix34ApplyLPInverse(&joint->field_0x14, &transformed, pRoot_matrix);
                if (joint->type == 1 || joint->type == 4) {
                    BrMatrix34ApplyV(&transformed, &joint->parent_bone_axis, pParent_matrix);
                    BrMatrix34TApplyV(&joint->parent_bone_axis, &transformed, pRoot_matrix);
                }
            }
            for (i = 0; i < joint->count_limits; i++) {
                BrMatrix34ApplyV(&transformed, &joint->limits[i].parent, pParent_matrix);
                BrMatrix34TApplyV(&joint->limits[i].parent, &transformed, pRoot_matrix);
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x004c27c0
void C2_HOOK_FAKE_THISCALL InterpolateObjects(tPhysics_object* pObject, float pDt) {
    br_vector3 omega_world;

    pObject->rotate_omega.v[0] = pObject->omega.v[0];
    pObject->rotate_omega.v[1] = pObject->omega.v[1];
    pObject->rotate_omega.v[2] = pObject->omega.v[2];
    BrMatrix34ApplyV(&omega_world, &pObject->omega, &pObject->actor->t.t.mat);
    InterpolateSingleObject(pObject, pDt);
    BrMatrix34TApplyV(&pObject->omega, &omega_world, &pObject->actor->t.t.mat);
}

// FUNCTION: CARMA2_HW 0x004c2d60
int C2_HOOK_FASTCALL FUN_004c2d60(tPhysics_object* pObject, tWorld_callbacks* pCar_callbacks) {
    br_vector3 normal;
    tPhysics_object** next_link;
    int next;
    int count;

    count = 0;
    next_link = &pObject->next;
    next = (int)*next_link;
    while (next != 0) {
        next = TestForObjectInSensiblePlace(pObject, *next_link, &normal, pCar_callbacks);
        if (next == 0) {
            count += 1;
        }
        pObject = *next_link;
        next_link = &pObject->next;
        next = (int)*next_link;
    }
    return count;
}

// FUNCTION: CARMA2_HW 0x004c62e0
void C2_HOOK_FASTCALL UpdateChildTransforms(tPhysics_object* pObject, br_matrix34* pParent_matrix) {
    br_matrix34 inverse;
    br_matrix34 tmp_matrix;
    br_matrix34 tmp_matrix2;
    tPhysics_object* child;

    child = pObject->child;
    if (pParent_matrix != NULL) {
        BrMatrix34Mul(&pObject->actor->t.t.mat, &pObject->transform_matrix, pParent_matrix);
    } else {
        BrMatrix34LPInverse(&inverse, &pObject->transform_matrix);
        BrMatrix34Mul(&tmp_matrix2, &inverse, &pObject->actor->t.t.mat);
        BrMatrix34LPNormalise(&tmp_matrix, &tmp_matrix2);
        pParent_matrix = &tmp_matrix;
    }
    if (child != NULL) {
        BrMatrix34LPInverse(&inverse, &pObject->transform_matrix);
        BrMatrix34Mul(&tmp_matrix, &inverse, &pObject->actor->t.t.mat);
        do {
            UpdateChildTransforms(child, pParent_matrix);
            child = child->next;
        } while (child != NULL);
    }
    BrMatrix34Copy(&pObject->transform_matrix, &pObject->actor->t.t.mat);
}

// FUNCTION: CARMA2_HW 0x004c28a0
void C2_HOOK_FAKE_THISCALL FUN_004c28a0(tPhysics_object* pObject, float pDt) {
    float t0;
    float t1;
    float t2;

    for (; pObject != NULL; pObject = pObject->next) {
        t0 = pDt * pObject->v.v[0];
        t1 = pDt * pObject->v.v[1];
        t2 = pDt * pObject->v.v[2];
        pObject->actor->t.t.mat.m[3][0] += t0;
        pObject->actor->t.t.mat.m[3][1] += t1;
        pObject->actor->t.t.mat.m[3][2] += t2;
        InterpolateSingleObject(pObject, pDt);
        if (pObject->child != NULL) {
            FUN_004c28a0(pObject->child, pDt);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004be4b0
void C2_HOOK_FASTCALL FUN_004be4b0(float* pNormal, float* pScaledVector, const float* pVector1, const float* pVector2, tPhysics_object* pObject) {
    float fVar1;
    int iVar2;

    if ((pObject->flags & 0x800) == 0) {
        pNormal[0] = pVector1[1] * pVector2[2] - pVector1[2] * pVector2[1];
        pNormal[1] = pVector1[2] * pVector2[0] - pVector2[2] * pVector1[0];
        pNormal[2] = pVector2[1] * pVector1[0] - pVector1[1] * pVector2[0];
        for (iVar2 = 0; iVar2 < 3; iVar2++) {
            pNormal[iVar2] = pNormal[iVar2] / pObject->I.v[iVar2];
        }
    } else {
        pNormal[0] = 0.f;
        pNormal[1] = 0.f;
        pNormal[2] = 0.f;
    }
    fVar1 = (float)(1.0 / pObject->M);
    pScaledVector[0] = fVar1 * pVector2[0];
    pScaledVector[1] = fVar1 * pVector2[1];
    pScaledVector[2] = fVar1 * pVector2[2];
}

// C2PORT-NONMATCHING: best 29.39% | difftest PASS 2000 trials (spec difftest/specs/004be550_FUN_004be550.json) | reason x87-order
// FUNCTION: CARMA2_HW 0x004be550
tPhysics_object* C2_HOOK_FASTCALL FUN_004be550(br_vector3* pAngular_delta, br_vector3* pLinear_delta, br_vector3* pAngular_delta_other, br_vector3* pLinear_delta_other, const br_vector3* pImpulse, tPhysics_object* pObject) {
  float fVar1;
  int iVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float *pfVar7;
  int iVar8;
  int iVar9;
  float *pfVar10;
  struct {
    float local_48[4];
    float local_38;
    float local_34;
    float local_30;
    float local_2c;
    float local_28;
    float local_24[4];
    float local_14;
    float local_10;
    float local_4;
    float stack_padding[3];
  } local_storage;
#define local_48 local_storage.local_48
#define local_38 local_storage.local_38
#define local_34 local_storage.local_34
#define local_30 local_storage.local_30
#define local_2c local_storage.local_2c
#define local_28 local_storage.local_28
#define local_24 local_storage.local_24
#define local_14 local_storage.local_14
#define local_10 local_storage.local_10
#define local_4 local_storage.local_4

    register float* param_2 = (float*)pLinear_delta;
    register float* param_1 = (float*)pAngular_delta;
    register float* param_3 = (float*)pAngular_delta_other;
    float* param_4 = (float*)pLinear_delta_other;
    float* param_5 = (float*)pImpulse;
    int param_6 = (int)(uintptr_t)pObject;
  if ((*(unsigned int *)(param_6 + 0x19c) & 0x800) == 0) {
    *param_1 = *param_5 / *(float *)(param_6 + 8);
    param_1[1] = param_5[1] / *(float *)(param_6 + 0xc);
    param_1[2] = param_5[2] / *(float *)(param_6 + 0x10);
  }
  else {
    *param_1 = 0.0;
    param_1[1] = 0.0;
    param_1[2] = 0.0;
  }
  if (*(int *)(param_6 + 0x1e4) == 0) {
    BrVector3SetFloat((br_vector3*)param_2, 0.f, 0.f, 0.f);
    return 0;
  }
  iVar2 = *(int *)(*(int *)(param_6 + 0x1e4) + 4);
  if (iVar2 == *(int *)(param_6 + 0x230)) {
    iVar9 = *(int *)(param_6 + 0x188);
    local_48[3] = *(float *)(iVar9 + 8) - *(float *)(param_6 + 0x14);
    local_38 = *(float *)(iVar9 + 0xc) - *(float *)(param_6 + 0x18);
    local_34 = *(float *)(iVar9 + 0x10) - *(float *)(param_6 + 0x1c);
    local_24[3] = -((param_1[1] * local_34 - param_1[2] * local_38) * *(float *)(iVar9 + 0x38) +
                   *(float *)(iVar9 + 0x3c) * (param_1[2] * local_48[3] - *param_1 * local_34) +
                   *(float *)(iVar9 + 0x40) * (*param_1 * local_38 - param_1[1] * local_48[3]));
    local_14 = -(*param_1 * *(float *)(iVar9 + 0x20) +
                param_1[1] * *(float *)(iVar9 + 0x24) + param_1[2] * *(float *)(iVar9 + 0x28));
    local_10 = -(*param_1 * *(float *)(iVar9 + 0x2c) +
                param_1[1] * *(float *)(iVar9 + 0x30) + param_1[2] * *(float *)(iVar9 + 0x34));
    BrMatrix34ApplyV((br_vector3*)&local_30, (br_vector3*)&local_24[3], (br_matrix34*)(*(int *)(param_6 + 0x1e4) + 8));
    fVar1 = local_30 / *(float *)(param_6 + 4);
    *param_2 = *(float *)(*(int *)(param_6 + 0x188) + 0x38) * fVar1;
    param_2[1] = *(float *)(*(int *)(param_6 + 0x188) + 0x3c) * fVar1;
    param_2[2] = *(float *)(*(int *)(param_6 + 0x188) + 0x40) * fVar1;
    fVar1 = -(local_30 / *(float *)(iVar2 + 4));
    *param_4 = *(float *)(*(int *)(param_6 + 0x188) + 0x44) * fVar1;
    param_4[1] = *(float *)(*(int *)(param_6 + 0x188) + 0x48) * fVar1;
    iVar8 = 3;
    param_4[2] = *(float *)(*(int *)(param_6 + 0x188) + 0x4c) * fVar1;
    iVar9 = *(int *)(param_6 + 0x188);
    local_24[0] = *(float *)(iVar9 + 0x20) * local_2c + *(float *)(iVar9 + 0x2c) * local_28;
    local_24[1] = *(float *)(iVar9 + 0x24) * local_2c + *(float *)(iVar9 + 0x30) * local_28;
    local_24[2] = *(float *)(iVar9 + 0x28) * local_2c + *(float *)(iVar9 + 0x34) * local_28;
    fVar1 = *(float *)(iVar9 + 0x38) * local_30;
    fVar3 = *(float *)(iVar9 + 0x3c) * local_30;
    fVar4 = *(float *)(iVar9 + 0x40) * local_30;
    pfVar7 = (float *)(param_6 + 8);
    local_4 = local_48[3] * fVar3 - local_38 * fVar1;
    local_48[1] = (local_34 * fVar1 - local_48[3] * fVar4) + local_24[1];
    local_48[2] = local_4 + local_24[2];
    local_48[0] = (local_38 * fVar4 - local_34 * fVar3) + local_24[0];
    pfVar10 = param_1;
    do {
      fVar1 = *pfVar7;
      pfVar7 = pfVar7 + 1;
      iVar8 = iVar8 + -1;
      *pfVar10 = *(float *)(((int)local_48 - (int)param_1) + (int)pfVar10) / fVar1 + *pfVar10;
      pfVar10 = pfVar10 + 1;
    } while (iVar8 != 0);
    BrMatrix34ApplyV((br_vector3*)local_48, (br_vector3*)local_24, (br_matrix34*)(param_6 + 0x8c));
    BrMatrix34TApplyV((br_vector3*)param_3, (br_vector3*)local_48, (br_matrix34*)(iVar2 + 0x8c));
    iVar9 = *(int *)(param_6 + 0x188);
    pfVar7 = (float *)(iVar2 + 8);
    fVar1 = *(float *)(iVar9 + 0x44) * local_30;
    fVar3 = *(float *)(iVar9 + 0x14) - *(float *)(iVar2 + 0x14);
    fVar4 = *(float *)(iVar9 + 0x18) - *(float *)(iVar2 + 0x18);
    fVar5 = *(float *)(iVar9 + 0x1c) - *(float *)(iVar2 + 0x1c);
    fVar6 = *(float *)(iVar9 + 0x48) * local_30;
    local_30 = *(float *)(iVar9 + 0x4c) * local_30;
    *param_3 = (fVar4 * local_30 - fVar5 * fVar6) + *param_3;
    iVar9 = 3;
    param_3[1] = (fVar5 * fVar1 - fVar3 * local_30) + param_3[1];
    param_3[2] = (fVar3 * fVar6 - fVar4 * fVar1) + param_3[2];
    do {
      fVar1 = *pfVar7;
      pfVar7 = pfVar7 + 1;
      iVar9 = iVar9 + -1;
      *param_3 = -(*param_3 / fVar1);
      param_3 = param_3 + 1;
    } while (iVar9 != 0);
    return (tPhysics_object*)(uintptr_t)iVar2;
  }
  iVar9 = *(int *)(iVar2 + 0x188);
  local_48[3] = *(float *)(iVar9 + 0x14) - *(float *)(param_6 + 0x14);
  local_38 = *(float *)(iVar9 + 0x18) - *(float *)(param_6 + 0x18);
  local_34 = *(float *)(iVar9 + 0x1c) - *(float *)(param_6 + 0x1c);
  local_24[3] = (param_1[1] * local_34 - param_1[2] * local_38) * *(float *)(iVar9 + 0x44) +
                *(float *)(iVar9 + 0x48) * (param_1[2] * local_48[3] - *param_1 * local_34) +
                *(float *)(iVar9 + 0x4c) * (*param_1 * local_38 - param_1[1] * local_48[3]);
  BrMatrix34ApplyV((br_vector3*)local_24, (br_vector3*)param_1, (br_matrix34*)(param_6 + 0x8c));
  BrMatrix34TApplyV((br_vector3*)local_48, (br_vector3*)local_24, (br_matrix34*)(iVar2 + 0x8c));
  iVar9 = *(int *)(iVar2 + 0x188);
  local_14 = local_48[0] * *(float *)(iVar9 + 0x20) +
             local_48[1] * *(float *)(iVar9 + 0x24) + local_48[2] * *(float *)(iVar9 + 0x28);
  iVar9 = *(int *)(iVar2 + 0x188);
  local_10 = local_48[0] * *(float *)(iVar9 + 0x2c) +
             local_48[1] * *(float *)(iVar9 + 0x30) + local_48[2] * *(float *)(iVar9 + 0x34);
  BrMatrix34ApplyV((br_vector3*)&local_30, (br_vector3*)&local_24[3], (br_matrix34*)(*(int *)(iVar2 + 0x1e4) + 8));
  fVar1 = local_30 / *(float *)(iVar2 + 4);
  *param_4 = *(float *)(*(int *)(iVar2 + 0x188) + 0x38) * fVar1;
  param_4[1] = *(float *)(*(int *)(iVar2 + 0x188) + 0x3c) * fVar1;
  param_4[2] = *(float *)(*(int *)(iVar2 + 0x188) + 0x40) * fVar1;
  fVar1 = -(local_30 / *(float *)(param_6 + 4));
  *param_2 = *(float *)(*(int *)(iVar2 + 0x188) + 0x44) * fVar1;
  param_2[1] = *(float *)(*(int *)(iVar2 + 0x188) + 0x48) * fVar1;
  param_2[2] = *(float *)(*(int *)(iVar2 + 0x188) + 0x4c) * fVar1;
  *param_3 = *(float *)(*(int *)(iVar2 + 0x188) + 0x20) * local_2c;
  param_3[1] = *(float *)(*(int *)(iVar2 + 0x188) + 0x24) * local_2c;
  param_3[2] = *(float *)(*(int *)(iVar2 + 0x188) + 0x28) * local_2c;
  *param_3 = *(float *)(*(int *)(iVar2 + 0x188) + 0x2c) * local_28 + *param_3;
  param_3[1] = *(float *)(*(int *)(iVar2 + 0x188) + 0x30) * local_28 + param_3[1];
  param_3[2] = *(float *)(*(int *)(iVar2 + 0x188) + 0x34) * local_28 + param_3[2];
  BrMatrix34ApplyV((br_vector3*)local_48, (br_vector3*)param_3, (br_matrix34*)(iVar2 + 0x8c));
  BrMatrix34TApplyV((br_vector3*)local_24, (br_vector3*)local_48, (br_matrix34*)(param_6 + 0x8c));
  fVar1 = *(float *)(*(int *)(iVar2 + 0x188) + 0x44) * local_30;
  pfVar10 = (float *)(param_6 + 8);
  fVar3 = *(float *)(*(int *)(iVar2 + 0x188) + 0x48) * local_30;
  fVar4 = *(float *)(*(int *)(iVar2 + 0x188) + 0x4c) * local_30;
  local_24[0] = (local_38 * fVar4 - local_34 * fVar3) + local_24[0];
  local_24[1] = (local_34 * fVar1 - local_48[3] * fVar4) + local_24[1];
  local_24[2] = (local_48[3] * fVar3 - local_38 * fVar1) + local_24[2];
  iVar9 = *(int *)(iVar2 + 0x188);
  local_48[0] = *(float *)(iVar9 + 0x38) * local_30;
  fVar1 = *(float *)(iVar9 + 8) - *(float *)(iVar2 + 0x14);
  local_38 = *(float *)(iVar9 + 0xc) - *(float *)(iVar2 + 0x18);
  local_34 = *(float *)(iVar9 + 0x10) - *(float *)(iVar2 + 0x1c);
  local_48[1] = *(float *)(*(int *)(iVar2 + 0x188) + 0x3c) * local_30;
  local_48[2] = *(float *)(*(int *)(iVar2 + 0x188) + 0x40) * local_30;
  iVar9 = 3;
  *param_3 = (local_38 * local_48[2] - local_34 * local_48[1]) + *param_3;
  param_3[1] = (local_34 * local_48[0] - fVar1 * local_48[2]) + param_3[1];
  param_3[2] = (fVar1 * local_48[1] - local_38 * local_48[0]) + param_3[2];
  pfVar7 = param_1;
  do {
    fVar1 = *pfVar10;
    pfVar10 = pfVar10 + 1;
    iVar9 = iVar9 + -1;
    *pfVar7 = *pfVar7 - *(float *)(((int)local_24 - (int)param_1) + (int)pfVar7) / fVar1;
    pfVar7 = pfVar7 + 1;
  } while (iVar9 != 0);
  pfVar7 = (float *)(iVar2 + 8);
  iVar9 = 3;
  do {
    fVar1 = *pfVar7;
    pfVar7 = pfVar7 + 1;
    iVar9 = iVar9 + -1;
    *param_3 = *param_3 / fVar1;
    param_3 = param_3 + 1;
  } while (iVar9 != 0);
  return (tPhysics_object*)(uintptr_t)iVar2;
}

#undef local_48
#undef local_38
#undef local_34
#undef local_30
#undef local_2c
#undef local_28
#undef local_24
#undef local_14
#undef local_10
#undef local_4

// C2PORT-NONMATCHING: best 18.54% | difftest PASS 2000 trials (spec difftest/specs/004becf0_FUN_004becf0.json) | reason x87-order, register-allocation, stack-layout
// FUNCTION: CARMA2_HW 0x004becf0
tPhysics_object* C2_HOOK_FASTCALL FUN_004becf0(br_vector3* pAngular_delta, br_vector3* pLinear_delta, br_vector3* pAngular_delta_other, br_vector3* pLinear_delta_other, tPhysics_object* pObject) {
    tPhysics_object* pPartner;
    tPhysics_joint* joint;
    tPhysics_joint* partner_joint;
    int i;
    float fVar1;
    float fVar3;
    float fVar4;
    float fVar5;
    float fVar6;
    float local_48[4];
    float local_38;
    float local_34;
    float local_30[4];
    float local_20;
    float local_1c;
    float local_18;
    float local_14;
    float local_10;
    float local_4;
    br_vector3 matrix_input;
    br_vector3 matrix_output;
    pPartner = *(tPhysics_object**)(uintptr_t)(pObject->field_0x1e4 + 4);
    joint = pObject->physics_joint1;
    partner_joint = pPartner->physics_joint1;
    if (pPartner == pObject->field_0x230) {
        local_38 = joint->field_0x08.v[1] - pObject->cmpos.v[1];
        local_34 = joint->field_0x08.v[2] - pObject->cmpos.v[2];
        local_48[3] = joint->field_0x08.v[0] - pObject->cmpos.v[0];
        local_30[3] = (pAngular_delta->v[1] * local_34 - pAngular_delta->v[2] * local_38) + pLinear_delta->v[0];
        local_20 = (pAngular_delta->v[2] * local_48[3] - local_34 * pAngular_delta->v[0]) + pLinear_delta->v[1];
        local_1c = (local_38 * pAngular_delta->v[0] - pAngular_delta->v[1] * local_48[3]) + pLinear_delta->v[2];
        local_18 = -(joint->hinge_axis.v[0] * local_30[3]
            + joint->hinge_axis.v[1] * local_20
            + joint->hinge_axis.v[2] * local_1c);
        local_14 = -(joint->hinge_axis2.v[0] * pAngular_delta->v[0]
            + joint->hinge_axis2.v[1] * pAngular_delta->v[1]
            + joint->hinge_axis2.v[2] * pAngular_delta->v[2]);
        local_10 = -(joint->hinge_axis3.v[0] * pAngular_delta->v[0]
            + joint->hinge_axis3.v[1] * pAngular_delta->v[1]
            + joint->hinge_axis3.v[2] * pAngular_delta->v[2]);
        matrix_input.v[0] = local_18;
        matrix_input.v[1] = local_14;
        matrix_input.v[2] = local_10;
        BrMatrix34ApplyV(&matrix_output, &matrix_input,
            (br_matrix34*)(uintptr_t)(pObject->field_0x1e4 + 8));
        local_30[3] = matrix_output.v[0];
        local_20 = matrix_output.v[1];
        local_1c = matrix_output.v[2];
        fVar1 = local_30[3] / pObject->M;
        pLinear_delta->v[0] = joint->hinge_axis.v[0] * fVar1 + pLinear_delta->v[0];
        pLinear_delta->v[1] = joint->hinge_axis.v[1] * fVar1 + pLinear_delta->v[1];
        pLinear_delta->v[2] = joint->hinge_axis.v[2] * fVar1 + pLinear_delta->v[2];
        fVar1 = -(local_30[3] / pPartner->M);
        pLinear_delta_other->v[0] = joint->parent_bone_axis.v[0] * fVar1;
        pLinear_delta_other->v[1] = joint->parent_bone_axis.v[1] * fVar1;
        pLinear_delta_other->v[2] = joint->parent_bone_axis.v[2] * fVar1;

        local_30[0] = joint->hinge_axis2.v[0] * local_20 + joint->hinge_axis3.v[0] * local_1c;
        local_30[1] = joint->hinge_axis2.v[1] * local_20 + joint->hinge_axis3.v[1] * local_1c;
        local_30[2] = joint->hinge_axis2.v[2] * local_20 + joint->hinge_axis3.v[2] * local_1c;
        fVar1 = joint->hinge_axis.v[0] * local_30[3];
        fVar3 = joint->hinge_axis.v[1] * local_30[3];
        fVar4 = joint->hinge_axis.v[2] * local_30[3];
        local_4 = local_48[3] * fVar3 - local_38 * fVar1;
        local_48[1] = (local_34 * fVar1 - local_48[3] * fVar4) + local_30[1];
        local_48[2] = local_4 + local_30[2];
        local_48[0] = (local_38 * fVar4 - local_34 * fVar3) + local_30[0];
        for (i = 0; i < 3; i++) {
            pAngular_delta->v[i] = local_48[i] / pObject->I.v[i] + pAngular_delta->v[i];
        }
        BrMatrix34ApplyV((br_vector3*)&local_48[0], (br_vector3*)&local_30[0], &pObject->transform_matrix);
        BrMatrix34TApplyV(pAngular_delta_other, (br_vector3*)&local_48[0], &pPartner->transform_matrix);

        fVar1 = joint->parent_bone_axis.v[0] * local_30[3];
        fVar3 = joint->field_0x14.v[0] - pPartner->cmpos.v[0];
        fVar4 = joint->field_0x14.v[1] - pPartner->cmpos.v[1];
        fVar5 = joint->field_0x14.v[2] - pPartner->cmpos.v[2];
        fVar6 = joint->parent_bone_axis.v[1] * local_30[3];
        local_30[3] = joint->parent_bone_axis.v[2] * local_30[3];
        pAngular_delta_other->v[0] = (fVar4 * local_30[3] - fVar5 * fVar6) + pAngular_delta_other->v[0];
        pAngular_delta_other->v[1] = (fVar5 * fVar1 - fVar3 * local_30[3]) + pAngular_delta_other->v[1];
        pAngular_delta_other->v[2] = (fVar3 * fVar6 - fVar4 * fVar1) + pAngular_delta_other->v[2];
        for (i = 0; i < 3; i++) {
            pAngular_delta_other->v[i] = -pAngular_delta_other->v[i] / pPartner->I.v[i];
        }
        return pPartner;
    }

    local_38 = partner_joint->field_0x14.v[1] - pObject->cmpos.v[1];
    local_48[3] = partner_joint->field_0x14.v[0] - pObject->cmpos.v[0];
    local_34 = partner_joint->field_0x14.v[2] - pObject->cmpos.v[2];
    local_20 = (pAngular_delta->v[2] * local_48[3] - local_34 * pAngular_delta->v[0]) + pLinear_delta->v[1];
    local_1c = (local_38 * pAngular_delta->v[0] - pAngular_delta->v[1] * local_48[3]) + pLinear_delta->v[2];
    local_18 = partner_joint->parent_bone_axis.v[1] * local_20
        + partner_joint->parent_bone_axis.v[2] * local_1c
        + partner_joint->parent_bone_axis.v[0]
        * ((pAngular_delta->v[1] * local_34 - pAngular_delta->v[2] * local_38) + pLinear_delta->v[0]);
    BrMatrix34ApplyV((br_vector3*)&local_30[0], pAngular_delta, &pObject->transform_matrix);
    BrMatrix34TApplyV((br_vector3*)&local_48[0], (br_vector3*)&local_30[0], &pPartner->transform_matrix);
    local_14 = partner_joint->hinge_axis2.v[0] * local_48[0]
        + partner_joint->hinge_axis2.v[1] * local_48[1]
        + partner_joint->hinge_axis2.v[2] * local_48[2];
    local_10 = partner_joint->hinge_axis3.v[0] * local_48[0]
        + partner_joint->hinge_axis3.v[1] * local_48[1]
        + partner_joint->hinge_axis3.v[2] * local_48[2];
    matrix_input.v[0] = local_18;
    matrix_input.v[1] = local_14;
    matrix_input.v[2] = local_10;
    BrMatrix34ApplyV(&matrix_output, &matrix_input,
        (br_matrix34*)(uintptr_t)(pPartner->field_0x1e4 + 8));
    local_30[3] = matrix_output.v[0];
    local_20 = matrix_output.v[1];
    local_1c = matrix_output.v[2];
    fVar1 = local_30[3] / pPartner->M;
    pLinear_delta_other->v[0] = partner_joint->hinge_axis.v[0] * fVar1;
    pLinear_delta_other->v[1] = partner_joint->hinge_axis.v[1] * fVar1;
    pLinear_delta_other->v[2] = partner_joint->hinge_axis.v[2] * fVar1;
    fVar1 = -(local_30[3] / pObject->M);
    pLinear_delta->v[0] = partner_joint->parent_bone_axis.v[0] * fVar1 + pLinear_delta->v[0];
    pLinear_delta->v[1] = partner_joint->parent_bone_axis.v[1] * fVar1 + pLinear_delta->v[1];
    pLinear_delta->v[2] = partner_joint->parent_bone_axis.v[2] * fVar1 + pLinear_delta->v[2];

    pAngular_delta_other->v[0] = partner_joint->hinge_axis2.v[0] * local_20;
    pAngular_delta_other->v[1] = partner_joint->hinge_axis2.v[1] * local_20;
    pAngular_delta_other->v[2] = partner_joint->hinge_axis2.v[2] * local_20;
    pAngular_delta_other->v[0] = partner_joint->hinge_axis3.v[0] * local_1c + pAngular_delta_other->v[0];
    pAngular_delta_other->v[1] = partner_joint->hinge_axis3.v[1] * local_1c + pAngular_delta_other->v[1];
    pAngular_delta_other->v[2] = partner_joint->hinge_axis3.v[2] * local_1c + pAngular_delta_other->v[2];
    BrMatrix34ApplyV((br_vector3*)&local_48[0], pAngular_delta_other, &pPartner->transform_matrix);
    BrMatrix34TApplyV((br_vector3*)&local_30[0], (br_vector3*)&local_48[0], &pObject->transform_matrix);

    local_48[0] = partner_joint->parent_bone_axis.v[0] * local_30[3];
    local_48[1] = partner_joint->parent_bone_axis.v[1] * local_30[3];
    local_48[2] = partner_joint->parent_bone_axis.v[2] * local_30[3];
    local_30[0] = (local_38 * local_48[2] - local_34 * local_48[1]) + local_30[0];
    local_30[1] = (local_34 * local_48[0] - local_48[3] * local_48[2]) + local_30[1];
    local_30[2] = (local_48[3] * local_48[1] - local_38 * local_48[0]) + local_30[2];
    for (i = 0; i < 3; i++) {
        pAngular_delta->v[i] = pAngular_delta->v[i] - local_30[i] / pObject->I.v[i];
    }
    fVar1 = pPartner->physics_joint1->hinge_axis.v[0] * local_30[3];
    fVar3 = pPartner->physics_joint1->field_0x08.v[0] - pPartner->cmpos.v[0];
    fVar4 = pPartner->physics_joint1->field_0x08.v[1] - pPartner->cmpos.v[1];
    fVar5 = pPartner->physics_joint1->field_0x08.v[2] - pPartner->cmpos.v[2];
    fVar6 = pPartner->physics_joint1->hinge_axis.v[1] * local_30[3];
    local_30[3] = pPartner->physics_joint1->hinge_axis.v[2] * local_30[3];
    pAngular_delta_other->v[0] = (fVar4 * local_30[3] - fVar5 * fVar6) + pAngular_delta_other->v[0];
    pAngular_delta_other->v[1] = (fVar5 * fVar1 - fVar3 * local_30[3]) + pAngular_delta_other->v[1];
    pAngular_delta_other->v[2] = (fVar3 * fVar6 - fVar4 * fVar1) + pAngular_delta_other->v[2];
    for (i = 0; i < 3; i++) {
        pAngular_delta_other->v[i] = pAngular_delta_other->v[i] / pPartner->I.v[i];
    }
    return pPartner;
}

// C2PORT-NONMATCHING: best 48.59% | difftest PASS 2000 trials (spec difftest/specs/004b7960_FUN_004b7960.json) | reason VC5 register allocation and x87 scheduling; simple impulse paths also PASS (spec 004b7960_FUN_004b7960_SIMPLE.json)
// FUNCTION: CARMA2_HW 0x004b7960
void C2_HOOK_FAKE_THISCALL FUN_004b7960(tPhysics_joint_data* pJoint_data, float pDt) {
    tPhysics_object* pObject1;
    tPhysics_object* pObject2;
    tPhysics_object* pPartner;
    br_vector3 angular_delta;
    br_vector3 linear_delta;
    br_vector3 angular_delta_other;
    br_vector3 linear_delta_other;
    br_vector3 world_delta;
    br_vector3 impulse;
    br_vector3 linear_accel;
    float inverse_dt;

    if (pJoint_data->field_0x5c == 9 || pDt == gFLOAT_0058a768) {
        return;
    }

    pObject1 = pJoint_data->object1;
    pObject2 = pJoint_data->object2;
    if (pJoint_data->field_0x5c == 0xb) {
        pPartner = FUN_004be550(&angular_delta, &linear_delta, &angular_delta_other, &linear_delta_other, &pJoint_data->axis1, pObject1);
        pObject1->omega.v[0] += angular_delta.v[0] * pDt;
        pObject1->omega.v[1] += angular_delta.v[1] * pDt;
        pObject1->omega.v[2] += angular_delta.v[2] * pDt;
        if (pPartner != NULL) {
            linear_delta.v[0] *= pDt;
            linear_delta.v[1] *= pDt;
            linear_delta.v[2] *= pDt;
            pObject1->velocity_car_space.v[0] += linear_delta.v[0];
            pObject1->velocity_car_space.v[1] += linear_delta.v[1];
            pObject1->velocity_car_space.v[2] += linear_delta.v[2];
            BrMatrix34ApplyV(&pObject1->v, &pObject1->velocity_car_space, &pObject1->transform_matrix);
            linear_delta_other.v[0] *= pDt;
            linear_delta_other.v[1] *= pDt;
            linear_delta_other.v[2] *= pDt;
            pPartner->velocity_car_space.v[0] += linear_delta_other.v[0];
            pPartner->velocity_car_space.v[1] += linear_delta_other.v[1];
            pPartner->velocity_car_space.v[2] += linear_delta_other.v[2];
            BrMatrix34ApplyV(&pPartner->v, &pPartner->velocity_car_space, &pPartner->transform_matrix);
            pPartner->omega.v[0] += angular_delta_other.v[0] * pDt;
            pPartner->omega.v[1] += angular_delta_other.v[1] * pDt;
            pPartner->omega.v[2] += angular_delta_other.v[2] * pDt;
        }
        if (pObject2 != NULL) {
            pPartner = FUN_004be550(&angular_delta, &linear_delta, &angular_delta_other, &linear_delta_other, &pJoint_data->axis2, pObject2);
            inverse_dt = -pDt;
            pObject2->omega.v[0] += angular_delta.v[0] * inverse_dt;
            pObject2->omega.v[1] += angular_delta.v[1] * inverse_dt;
            pObject2->omega.v[2] += angular_delta.v[2] * inverse_dt;
            if (pPartner != NULL) {
                linear_delta.v[0] *= inverse_dt;
                linear_delta.v[1] *= inverse_dt;
                linear_delta.v[2] *= inverse_dt;
                pObject2->velocity_car_space.v[0] += linear_delta.v[0];
                pObject2->velocity_car_space.v[1] += linear_delta.v[1];
                pObject2->velocity_car_space.v[2] += linear_delta.v[2];
                BrMatrix34ApplyV(&pObject2->v, &pObject2->velocity_car_space, &pObject2->transform_matrix);
                linear_delta_other.v[0] *= inverse_dt;
                linear_delta_other.v[1] *= inverse_dt;
                linear_delta_other.v[2] *= inverse_dt;
                pPartner->velocity_car_space.v[0] += linear_delta_other.v[0];
                pPartner->velocity_car_space.v[1] += linear_delta_other.v[1];
                pPartner->velocity_car_space.v[2] += linear_delta_other.v[2];
                BrMatrix34ApplyV(&pPartner->v, &pPartner->velocity_car_space, &pPartner->transform_matrix);
                pPartner->omega.v[0] += angular_delta_other.v[0] * inverse_dt;
                pPartner->omega.v[1] += angular_delta_other.v[1] * inverse_dt;
                pPartner->omega.v[2] += angular_delta_other.v[2] * inverse_dt;
            }
        }
        return;
    }

    if (pObject1->field_0x1e4 == 0) {
        impulse.v[0] = pJoint_data->axis1.v[0] * pDt;
        linear_accel.v[0] = impulse.v[0] / pObject1->M;
        impulse.v[1] = pJoint_data->axis1.v[1] * pDt;
        linear_accel.v[1] = impulse.v[1] / pObject1->M;
        impulse.v[2] = pJoint_data->axis1.v[2] * pDt;
        linear_accel.v[2] = impulse.v[2] / pObject1->M;
        pObject1->velocity_car_space.v[0] += linear_accel.v[0];
        pObject1->velocity_car_space.v[1] += linear_accel.v[1];
        pObject1->velocity_car_space.v[2] += linear_accel.v[2];
        BrMatrix34ApplyV(&world_delta, &linear_accel, &pObject1->transform_matrix);
        pObject1->v.v[0] += world_delta.v[0];
        pObject1->v.v[1] += world_delta.v[1];
        pObject1->v.v[2] += world_delta.v[2];
        if ((pObject1->flags & 0x800) == 0) {
            angular_delta.v[0] = pJoint_data->anchor1.v[1] * impulse.v[2] - pJoint_data->anchor1.v[2] * impulse.v[1];
            angular_delta.v[1] = pJoint_data->anchor1.v[2] * impulse.v[0] - pJoint_data->anchor1.v[0] * impulse.v[2];
            angular_delta.v[2] = pJoint_data->anchor1.v[0] * impulse.v[1] - pJoint_data->anchor1.v[1] * impulse.v[0];
            pObject1->omega.v[0] += angular_delta.v[0] / pObject1->I.v[0];
            pObject1->omega.v[1] += angular_delta.v[1] / pObject1->I.v[1];
            pObject1->omega.v[2] += angular_delta.v[2] / pObject1->I.v[2];
        }
    } else {
        FUN_004be4b0(angular_delta.v, linear_delta.v, pJoint_data->anchor1.v, pJoint_data->axis1.v, pObject1);
        pPartner = FUN_004becf0(&angular_delta, &linear_delta, &angular_delta_other, &linear_delta_other, pObject1);
        angular_delta.v[0] *= pDt;
        angular_delta.v[1] *= pDt;
        angular_delta.v[2] *= pDt;
        pObject1->omega.v[0] += angular_delta.v[0];
        pObject1->omega.v[1] += angular_delta.v[1];
        pObject1->omega.v[2] += angular_delta.v[2];
        angular_delta_other.v[0] *= pDt;
        angular_delta_other.v[1] *= pDt;
        angular_delta_other.v[2] *= pDt;
        pPartner->omega.v[0] += angular_delta_other.v[0];
        pPartner->omega.v[1] += angular_delta_other.v[1];
        pPartner->omega.v[2] += angular_delta_other.v[2];
        linear_delta.v[0] *= pDt;
        linear_delta.v[1] *= pDt;
        linear_delta.v[2] *= pDt;
        pObject1->velocity_car_space.v[0] += linear_delta.v[0];
        pObject1->velocity_car_space.v[1] += linear_delta.v[1];
        pObject1->velocity_car_space.v[2] += linear_delta.v[2];
        BrMatrix34ApplyV(&world_delta, &linear_delta, &pObject1->transform_matrix);
        pObject1->v.v[0] += world_delta.v[0];
        pObject1->v.v[1] += world_delta.v[1];
        pObject1->v.v[2] += world_delta.v[2];
        linear_delta_other.v[0] *= pDt;
        linear_delta_other.v[1] *= pDt;
        linear_delta_other.v[2] *= pDt;
        pPartner->velocity_car_space.v[0] += linear_delta_other.v[0];
        pPartner->velocity_car_space.v[1] += linear_delta_other.v[1];
        pPartner->velocity_car_space.v[2] += linear_delta_other.v[2];
        BrMatrix34ApplyV(&world_delta, &linear_delta_other, &pPartner->transform_matrix);
        pPartner->v.v[0] += world_delta.v[0];
        pPartner->v.v[1] += world_delta.v[1];
        pPartner->v.v[2] += world_delta.v[2];
    }

    if (pObject2 != NULL && pJoint_data->field_0x5c != 0xe) {
        if (pObject2->field_0x1e4 != 0) {
            FUN_004be4b0(angular_delta.v, linear_delta.v, pJoint_data->anchor2.v, pJoint_data->axis2.v, pObject2);
            pPartner = FUN_004becf0(&angular_delta, &linear_delta, &angular_delta_other, &linear_delta_other, pObject2);
            pDt = -pDt;
            angular_delta.v[0] *= pDt;
            angular_delta.v[1] *= pDt;
            angular_delta.v[2] *= pDt;
            pObject2->omega.v[0] += angular_delta.v[0];
            pObject2->omega.v[1] += angular_delta.v[1];
            pObject2->omega.v[2] += angular_delta.v[2];
            angular_delta_other.v[0] *= pDt;
            angular_delta_other.v[1] *= pDt;
            angular_delta_other.v[2] *= pDt;
            pPartner->omega.v[0] += angular_delta_other.v[0];
            pPartner->omega.v[1] += angular_delta_other.v[1];
            pPartner->omega.v[2] += angular_delta_other.v[2];
            linear_delta.v[0] *= pDt;
            linear_delta.v[1] *= pDt;
            linear_delta.v[2] *= pDt;
            pObject2->velocity_car_space.v[0] += linear_delta.v[0];
            pObject2->velocity_car_space.v[1] += linear_delta.v[1];
            pObject2->velocity_car_space.v[2] += linear_delta.v[2];
            BrMatrix34ApplyV(&world_delta, &linear_delta, &pObject2->transform_matrix);
            pObject2->v.v[0] += world_delta.v[0];
            pObject2->v.v[1] += world_delta.v[1];
            pObject2->v.v[2] += world_delta.v[2];
            linear_delta_other.v[0] *= pDt;
            linear_delta_other.v[1] *= pDt;
            linear_delta_other.v[2] *= pDt;
            pPartner->velocity_car_space.v[0] += linear_delta_other.v[0];
            pPartner->velocity_car_space.v[1] += linear_delta_other.v[1];
            pPartner->velocity_car_space.v[2] += linear_delta_other.v[2];
            BrMatrix34ApplyV(&world_delta, &linear_delta_other, &pPartner->transform_matrix);
            pPartner->v.v[0] += world_delta.v[0];
            pPartner->v.v[1] += world_delta.v[1];
            pPartner->v.v[2] += world_delta.v[2];
        } else {
            inverse_dt = -pDt;
            impulse.v[0] = pJoint_data->axis2.v[0] * inverse_dt;
            linear_accel.v[0] = impulse.v[0] / pObject2->M;
            impulse.v[1] = pJoint_data->axis2.v[1] * inverse_dt;
            linear_accel.v[1] = impulse.v[1] / pObject2->M;
            impulse.v[2] = pJoint_data->axis2.v[2] * inverse_dt;
            linear_accel.v[2] = impulse.v[2] / pObject2->M;
            pObject2->velocity_car_space.v[0] += linear_accel.v[0];
            pObject2->velocity_car_space.v[1] += linear_accel.v[1];
            pObject2->velocity_car_space.v[2] += linear_accel.v[2];
            BrMatrix34ApplyV(&world_delta, &linear_accel, &pObject2->transform_matrix);
            pObject2->v.v[0] += world_delta.v[0];
            pObject2->v.v[1] += world_delta.v[1];
            pObject2->v.v[2] += world_delta.v[2];
            if ((pObject2->flags & 0x800) == 0) {
                angular_delta.v[0] = pJoint_data->anchor2.v[1] * impulse.v[2] - pJoint_data->anchor2.v[2] * impulse.v[1];
                angular_delta.v[1] = pJoint_data->anchor2.v[2] * impulse.v[0] - pJoint_data->anchor2.v[0] * impulse.v[2];
                angular_delta.v[2] = pJoint_data->anchor2.v[0] * impulse.v[1] - pJoint_data->anchor2.v[1] * impulse.v[0];
                pObject2->omega.v[0] += angular_delta.v[0] / pObject2->I.v[0];
                pObject2->omega.v[1] += angular_delta.v[1] / pObject2->I.v[1];
                pObject2->omega.v[2] += angular_delta.v[2] / pObject2->I.v[2];
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x004c6200
int C2_HOOK_FASTCALL FUN_004c6200(tPhysics_object* pObject, tPhysics_object* pBuffer, int pRemaining) {
    tPhysics_object* pOutput;

    pOutput = pBuffer;
    if (pObject != NULL) {
        do {
            if (pRemaining != 0) {
                int remaining_before_children;

                memcpy(pOutput++, pObject, sizeof(tPhysics_object));
                pRemaining -= 1;
                remaining_before_children = pRemaining;
                if (pObject->child != NULL) {
                    pRemaining = FUN_004c6200(pObject->child, pOutput, pRemaining);
                    pOutput += remaining_before_children - pRemaining;
                }
            }
            pObject = pObject->next;
        } while (pObject != NULL);
    }
    return pRemaining;
}

// FUNCTION: CARMA2_HW 0x004c5d30
tCollision_shape_polyhedron* C2_HOOK_FASTCALL AllocateShapeWireframePolyhedron(int pCount_points, int pType) {
    tCollision_shape_polyhedron* result;

    result = BrMemAllocate(pCount_points * 30 + 0x70, pType);
    result->polyhedron.points = (br_vector3*)((tU8*)result + sizeof(tCollision_shape_polyhedron));
    result->polyhedron.count_points = pCount_points;
    result->polyhedron.edges = (tPolyhedron_edge_indexes*)(result->polyhedron.points + pCount_points);
    result->polyhedron.planes = (br_vector4*)((tU8*)result->polyhedron.edges + pCount_points * sizeof(tPolyhedron_edge_indexes));
    result->polyhedron.count_edges = 0;
    result->polyhedron.count_planes = 0;
    result->common.type = kCollisionShapeType_Wireframe_Polyhedron;
    return result;
}
#pragma auto_inline(on)

// FUNCTION: CARMA2_HW 0x004be350
void C2_HOOK_FASTCALL FUN_004be350(float* pDestination, const float* pSource) {
    float fVar1;
    float fVar2;
    float fVar3;
    float fVar4;
    float fVar5;

    pDestination[0] = -pSource[5] * pSource[7] + pSource[8] * pSource[4];
    pDestination[1] = -(-pSource[2] * pSource[7] + pSource[1] * pSource[8]);
    fVar1 = -pSource[2] * pSource[4] + pSource[1] * pSource[5];
    pDestination[2] = fVar1;
    fVar5 = (float)gDOUBLE_0058a780 /
        (pSource[1] * pDestination[1] + pSource[2] * fVar1 + pSource[0] * pDestination[0]);
    pDestination[0] = pDestination[0] * fVar5;
    pDestination[3] = -((-pSource[5] * pSource[6] + pSource[8] * pSource[3]) * fVar5);
    fVar1 = pSource[4];
    fVar2 = pSource[3];
    fVar3 = pSource[7];
    fVar4 = pSource[6];
    pDestination[1] = pDestination[1] * fVar5;
    pDestination[6] = (-fVar1 * fVar4 + fVar3 * fVar2) * fVar5;
    pDestination[4] = (-pSource[2] * pSource[6] + pSource[8] * pSource[0]) * fVar5;
    fVar1 = pSource[1];
    fVar2 = pSource[7];
    fVar3 = pSource[6];
    fVar4 = pSource[0];
    pDestination[2] = pDestination[2] * fVar5;
    pDestination[7] = -((-fVar1 * fVar3 + fVar2 * fVar4) * fVar5);
    pDestination[5] = -((-pSource[2] * pSource[3] + pSource[5] * pSource[0]) * fVar5);
    pDestination[8] = (-pSource[1] * pSource[3] + pSource[4] * pSource[0]) * fVar5;
}

// FUNCTION: CARMA2_HW 0x004b77b0
void C2_HOOK_FASTCALL FUN_004b77b0(int* pDestination, int* pJoint_data, int pCount) {
    int destination_offset;
    tPhysics_joint_data* joint_data;
    tPhysics_object* object1;
    tPhysics_object* object2;
    br_scalar* destination;
    volatile float angular_delta[3];

    if (pCount <= 0) {
        return;
    }
    destination_offset = (int)pDestination - (int)pJoint_data;
    do {
        joint_data = (tPhysics_joint_data*)(uintptr_t)*pJoint_data;
        object1 = joint_data->object1;
        destination = (br_scalar*)((char*)joint_data + destination_offset);
        if (joint_data->field_0x5c == eJoint_limit_11) {
            *destination = -(object1->omega.v[0] * joint_data->axis1.v[0]
                + joint_data->axis1.v[2] * object1->omega.v[2]
                + joint_data->axis1.v[1] * object1->omega.v[1]);
            object2 = joint_data->object2;
            if (object2 != NULL) {
                *destination = joint_data->axis2.v[2] * object2->omega.v[2]
                    + joint_data->axis2.v[0] * object2->omega.v[0]
                    + joint_data->axis2.v[1] * object2->omega.v[1]
                    + *destination;
            }
        } else {
            angular_delta[0] = (joint_data->anchor1.v[2] * object1->omega.v[1]
                    - joint_data->anchor1.v[1] * object1->omega.v[2])
                + object1->velocity_car_space.v[0];
            angular_delta[1] = (object1->omega.v[2] * joint_data->anchor1.v[0]
                    - object1->omega.v[0] * joint_data->anchor1.v[2])
                + object1->velocity_car_space.v[1];
            angular_delta[2] = (object1->omega.v[0] * joint_data->anchor1.v[1]
                    - object1->omega.v[1] * joint_data->anchor1.v[0])
                + object1->velocity_car_space.v[2];
            *destination = -(joint_data->axis1.v[0] * angular_delta[0]
                + joint_data->axis1.v[2] * angular_delta[2]
                + joint_data->axis1.v[1] * angular_delta[1]);
            object2 = joint_data->object2;
            if (object2 != NULL) {
                angular_delta[0] = (object2->omega.v[0] * joint_data->anchor2.v[1]
                        - joint_data->anchor2.v[0] * object2->omega.v[1])
                    + object2->velocity_car_space.v[2];
                angular_delta[1] = (object2->omega.v[2] * joint_data->anchor2.v[0]
                        - object2->omega.v[0] * joint_data->anchor2.v[2])
                    + object2->velocity_car_space.v[1];
                angular_delta[2] = (object2->omega.v[1] * joint_data->anchor2.v[2]
                        - joint_data->anchor2.v[1] * object2->omega.v[2])
                    + object2->velocity_car_space.v[0];
                *destination = joint_data->axis2.v[2] * angular_delta[0]
                    + joint_data->axis2.v[1] * angular_delta[1]
                    + joint_data->axis2.v[0] * angular_delta[2]
                    + *destination;
            }
        }
        pJoint_data++;
        pCount--;
    } while (pCount != 0);
}

// FUNCTION: CARMA2_HW 0x004b9650
int C2_HOOK_FASTCALL FUN_004b9650(tPhysics_object* pObject, tPhysics_object* pParent_context) {
    int result;
    volatile int pObject_value;
    tPhysics_joint* joint;
    tPhysics_object* child;
    tPhysics_object* child_parent_context;

    result = 0;
    pObject_value = (int)pObject;
    joint = pObject->physics_joint1;
    pObject->field_0x230 = pParent_context;
    pObject->field_0x1e4 = 0;
    if (joint != NULL && (pObject->flags & 0x400) == 0) {
        switch (joint->type) {
        case eJoint_none:
            break;
        case eJoint_hinge:
            result = 5;
            break;
        case eJoint_universal:
            result = 4;
            break;
        case eJoint_ball_n_socket:
            result = 3;
            break;
        case eJoint_quick_hinge:
            if (pParent_context->field_0x1e4 == 0) {
                result = 2;
                pObject->field_0x1e4 = -1;
                pParent_context->field_0x1e4 = -1;
                break;
            }
            joint->type = eJoint_hinge;
            result = 5;
            break;
        default:
            ((void (C2_HOOK_FASTCALL *)(tPhysicsError, const char*))PhysicsError)(8, "");
            result = pObject_value;
            break;
        }
    }
    if (pObject->physics_joint2 != NULL) {
        switch (pObject->physics_joint2->type) {
        case eJoint_universal:
            result += 4;
            break;
        case eJoint_ball_n_socket:
            result += 3;
            break;
        case eJoint_translation:
            result += 2;
            break;
        }
    }
    child_parent_context = (pObject->flags & 0x400) != 0 ? pParent_context : pObject;
    for (child = pObject->child; child != NULL; child = child->next) {
        result += FUN_004b9650(child, child_parent_context);
    }
    return result;
}
