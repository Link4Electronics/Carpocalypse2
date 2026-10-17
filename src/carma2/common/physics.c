#include "physics.h"

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
#include "sound.h"
#include "spark.h"
#include "utility.h"
#include "world.h"
#include "finteray.h"

#include <brender/brender.h>

#include <stdarg.h>

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

#define PHYSICS_BUFFER_PART_SIZE 40000
#define PHYSICS_BUFFER_OTHER_SIZE 50000
#include "physics.h"

// count

// DrMatrix34ApplyLPInverse

// AddObjectWallForce

// AddObjectObjectForce

// RayCastThruFaceList

// RayCastThruFaceList2

// FaceOffCount2

// FaceOffCount

// FaceOnCount

// EliminateFaces

// GetShapePoint

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

// TrialPositionChildren

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

// RQDcmpV

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

// MatrixProcess

// RqBackSub

// GetPhysicsScratchPointer

// IncreasePhysicsScratchPointer

// ClearPhysicsScratchSpace

// AllocateShapeSphere

// AllocateShapePolyhedron

// AllocateShapeWireFrame

// AllocateShapeBox

// GetJointSize

// AllocatePhysicsJoint

// ClonePhysicsJoint

// FreeShape
typedef enum {
    ePlaneSide_positive = 0,
    ePlaneSide_negative = 1,
    ePlaneSide_zero = 2,
} tPlane_side;

typedef int C2_HOOK_FASTCALL tPhysicsObject_cbfn(tPhysics_object* pObject, void* pContext);
// GLOBAL: CARMA2_HW 0x006940bc
void* gPhysics_scratch_ptr;

// GLOBAL: CARMA2_HW 0x00676920
char gPhysics_empty_message;

// GLOBAL: CARMA2_HW 0x006940c0
tPhysicsError_cbfn* gPhysics_error_cb;

// GLOBAL: CARMA2_HW 0x00744818
void* gPhysics_buffer1_ptr;

// GLOBAL: CARMA2_HW 0x00744814
void* gPhysics_buffer2_ptr;

// GLOBAL: CARMA2_HW 0x0074a5e0
void* gPhysics_buffer3_ptr;

// GLOBAL: CARMA2_HW 0x006940c8
int gPhysics_other_buffer_capacity;

// GLOBAL: CARMA2_HW 0x006940c4
void* gPhysics_other_buffer;

// GLOBAL: CARMA2_HW 0x006a0adc
int gCollision_info_uid_counter;

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
tPHIL_queued_objects gPhil_queued_objects;

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

// GLOBAL: CARMA2_HW 0x00692dd0
br_scalar gFLOAT_00692dd0;

// GLOBAL: CARMA2_HW 0x00692dd4
br_scalar gFLOAT_00692dd4;

// GLOBAL: CARMA2_HW 0x00692ddc
int gINT_00692ddc;

// GLOBAL: CARMA2_HW 0x00692de0
tCollision_contact gCollision_contacts[50];

// GLOBAL: CARMA2_HW 0x006940a8
tPhysics_object** gReduced_object_list;

// GLOBAL: CARMA2_HW 0x006940ac
int gINT_006940ac;

// GLOBAL: CARMA2_HW 0x00692dc8
int gINT_00692dc8;

// GLOBAL: CARMA2_HW 0x006923ec
tPhysics_object* gPHIL_queued_objects_for_removal[1];

// GLOBAL: CARMA2_HW 0x00692db8
tWorld_callback_active_passive_cbfn* gPHIL_original_activate_passive;

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

// FUNCTION: CARMA2_HW 0x00420ef0
int C2_HOOK_FASTCALL Collinear(const br_vector3* pV1, const br_vector3* pV2, const br_vector3* pV3) {
    br_scalar dz31;
    br_scalar dy31;
    br_scalar dz21;
    br_scalar dy21;
    br_scalar dx31;
    br_scalar dx21;
    br_scalar cross;

    dz31 = CARPOCALYPSE2_FORCE_FIRST(pV3->v[2]) - pV1->v[2];
    dy31 = CARPOCALYPSE2_FORCE_FIRST(pV3->v[1]) - pV1->v[1];
    dz21 = pV2->v[2] - pV1->v[2];
    dy21 = pV2->v[1] - pV1->v[1];
    cross = dz31 * dy21 - dy31 * dz21;
    if (fabsf(cross) < 1.0e-6f) {
        dx31 = pV3->v[0] - pV1->v[0];
        dx21 = pV2->v[0] - pV1->v[0];
        cross = dx31 * dz21 - dx21 * dz31;
        if (fabsf(cross) < 1.0e-6f) {
            cross = dx21 * dy31 - dx31 * dy21;
            if (fabsf(cross) < 1.0e-6f) {
                return 1;
            }
        }
    }
    return 0;
}

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

    return (pV1->v[1] * pV2->v[2] - pV1->v[2] * pV2->v[1]) * gPhysics_reference_normal_comparison.v[0]
        + (pV1->v[2] * pV2->v[0] - pV1->v[0] * pV2->v[2]) * gPhysics_reference_normal_comparison.v[1]
        + (pV1->v[0] * pV2->v[1] - pV1->v[1] * pV2->v[0]) * gPhysics_reference_normal_comparison.v[2];
}

// FUNCTION: CARMA2_HW 0x00420dc0
int C2_HOOK_CDECL CompareAngle(const void* pIndex1, const void* pIndex2) {
    const int* index1 = pIndex1;
    const int* index2 = pIndex2;
    tCollision_shape_polyhedron_data* poly = gPolyhedron_to_sort;
    br_vector3 edge1;
    br_vector3 edge2;
    br_scalar area;
    br_scalar length1;
    br_scalar length2;

    BrVector3Sub(&edge1, &poly->points[*index1], &poly->points[0]);
    BrVector3Sub(&edge2, &poly->points[*index2], &poly->points[0]);
    area = SignedTwiceAreaOf3DTriangle(&edge1, &edge2);
    if (area > 1.0e-6f) {
        return -1;
    }
    if (!(area >= -1.0e-6f)) {
        return 1;
    }
    length1 = BrVector3Length(&edge1);
    length2 = BrVector3Length(&edge2);
    if (!(length1 >= length2)) {
        return -1;
    }
    if (length1 > length2) {
        return 1;
    }
    return 0;
}

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

    total = 0.f;
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
        if (pImpulses[i] < 0.f) {
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
// Collects track-face normals around the placement: refreshes/culls faces,
// transforms the object's box corners to world space and records the normals
// of unmarked track faces in range into the caller buffer (seed normal at
// +0x2c, rest at +0x90 in 0x60 strides). Returns the recorded count.
// NOTE: when gSensible_place_collect_active is set the original selects a
// scratch matrix whose setup was not recoverable; the actor-relative product
// below matches the determined branch.
int C2_HOOK_FASTCALL CollectPlacementFaces(tPhysics_object* pObject, tPlacement_face_buf* pBuf, int pFlags) {
    br_matrix34 inv_actor;
    br_matrix34 rel;
    br_vector3 corners[8];
    br_vector3 world;
    tFace_ref* face;
    int count;
    int i;
    int j;

    (void)pFlags;
    BrMatrix34LPInverse(&inv_actor, &pObject->actor->t.t.mat);
    BrMatrix34Mul(&rel, &inv_actor, &pObject->transform_matrix);
    CullPlacementFaces(pObject, pObject->shape, &rel);
    for (i = 0; i < 8; i++) {
        world.v[0] = (i & 4) ? pObject->field_0xf4.max.v[0] : pObject->field_0xf4.min.v[0];
        world.v[1] = (i & 2) ? pObject->field_0xf4.max.v[1] : pObject->field_0xf4.min.v[1];
        world.v[2] = (i & 1) ? pObject->field_0xf4.max.v[2] : pObject->field_0xf4.min.v[2];
        BrMatrix34ApplyP(&corners[i], &world, &pObject->transform_matrix);
    }
    count = 0;
    for (i = pObject->box_face_start; i < pObject->box_face_end; i++) {
        face = &gFace_list__car[i];
        if ((face->flags & 0x80) != 0) {
            continue;
        }
        for (j = 0; j < 8; j++) {
            if (fabsf(BrVector3Dot(&face->normal, &corners[j]) - face->d) < 0.5f) {
                break;
            }
        }
        if (j >= 8) {
            continue;
        }
        if (count >= 8) {
            break;
        }
        if (count == 0) {
            BrVector3Copy(&pBuf->seed, &face->normal);
        } else {
            BrVector3Copy(&pBuf->records[count - 1].normal, &face->normal);
        }
        count++;
    }
    return count;
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
#pragma inline_depth()

#pragma auto_inline(off)
#pragma inline_depth(0)
// FUNCTION: CARMA2_HW 0x00429070
int C2_HOOK_FASTCALL TestForObjectInSensiblePlace(tPhysics_object* pCollision_info_1, tPhysics_object* pCollision_info_2, br_vector3* pVec3, tWorld_callbacks* pCar_callbacks) {
    tU8 buf[0x300];
    tPlacement_face_buf* faces;
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
    faces = (tPlacement_face_buf*)buf;
    GetNewBoundingBox(&pCollision_info_1->field_0xf4, &pCollision_info_1->bb1, &pCollision_info_1->transform_matrix);
    GetFacesInBox(pCollision_info_1, pCar_callbacks);
    gSensible_place_collect_active = 1;
    count = CollectPlacementFaces(pCollision_info_1, faces, 0);
    gSensible_place_collect_active = 0;
    if (count != 0) {
        float x = faces->seed.v[0];
        float y = faces->seed.v[1];
        float z = faces->seed.v[2];
        if (count > 1) {
            i = 0;
            count--;
            do {
                x += faces->records[i].normal.v[0];
                y += faces->records[i].normal.v[1];
                z += faces->records[i].normal.v[2];
                i++;
            } while (--count != 0);
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
    *(int*)&pObject->field_0x1e0[4] = 0;
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
    tQueued_object_info* object_info;

    if (gPHIL_enabled) {
        return 0;
    }
    object_info = pObject->field_0x240;
    if (pObject->field_0x239 == 2) {
        object_info->field_0x8 = 2;
        if (pVel != NULL) {
            BrVector3Copy(&object_info->field_0x18, pVel);
        }
        if (pOmega != NULL) {
            BrVector3Copy(&object_info->field_0x24, pOmega);
        }
        if (pArg4) {
            object_info->field_0x4 |= 0x2;
        } else {
            object_info->field_0x4 &= ~0x2;
        }
        return 0;
    }
    if (object_info == NULL) {
        return 2;
    }
    object_info->field_0x8 = 2;
    pObject->flags &= ~0x20;
    if (pArg4) {
        object_info->field_0x4 |= 0x2;
    } else {
        object_info->field_0x4 &= 0x2;
    }
    if (pVel != NULL) {
        BrVector3Copy(&pObject->v, pVel);
    }
    if (pOmega != NULL) {
        BrVector3Copy(&pObject->omega, pOmega);
    }
    ResetCarSpecialVolume(pObject);
    return 0;
}

// FUNCTION: CARMA2_HW 0x004ba190
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
void C2_HOOK_FASTCALL PHILActivatePassive(tPhysics_object* pObject) {
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
        gPHIL_original_activate_passive(pObject);
    }

    if (gPHIL_callbacks->passive_activated != NULL) {
        if (gPHIL_callbacks->passive_activated(pObject)) {
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

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x004c5d00
void C2_HOOK_FASTCALL ClearPhysicsScratchSpace(void) {

    gPhysics_scratch_ptr = gPhysics_other_buffer;
}

// FUNCTION: CARMA2_HW 0x004c61e0
void C2_HOOK_FASTCALL PhysicsWarning(const char* pMessage) {
    (void)pMessage;
}

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

// UNLABELED DATA: CARMA2_HW 0x00692dd8
int gINT_00692dd8 = 0;

// Pending joint-limit record emitted by 0x4bca10 and consumed by
// SetUpQuickHingeData's second phase (0x4bb22b). Stride 0x60.
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

// UNLABELED: CARMA2_HW 0x004bca10
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

// UNLABELED: CARMA2_HW 0x004bd6d0
// Runs CollectJointLimitsIntoBuffer over pObject and its following siblings,
// recursing into the child chains first, and returns the accumulated record
// count.
extern int C2_HOOK_FASTCALL CollectJointLimitsOfTree(tPhysics_object* pObject, void* pBuf, int pCount, int pStride) {
    if (pObject == NULL) {
        return pCount;
    }
    do {
        pCount = CollectJointLimitsIntoBuffer(pObject, pBuf, pCount, pStride);
        if (pObject->child != NULL) {
            pCount = CollectJointLimitsOfTree(pObject->child, pBuf, pCount, pStride);
        }
        pObject = pObject->next;
    } while (pObject != NULL);
    return pCount;
}

// UNLABELED: CARMA2_HW 0x004bd720
// Clamps runaway angular velocity: once omega^2 * 0.0016 exceeds 2.4649 the
// spin (also of the object linked through field_0x1e4) is scaled by 1/(2*a),
// a = 1.27388525*sqrt(omega^2*0.0016), and the damping is reported through
// ApplySpinDampingFactor plus the field_0x198 callback. Returns 1 when
// clamped, 0 when the spin was already small.
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
    holder = *(void**)&pObject->field_0x1e0[4];
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

// UNLABELED: CARMA2_HW 0x004bd7f0
// Runs ClampObjectSpin over pObject, its following siblings and the whole
// child chains, returning the number of objects clamped.
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

// UNLABELED: CARMA2_HW 0x004bd830
static void** C2_HOOK_FASTCALL QuickHingeCollect(tPhysics_object* pObject, tPhysics_object* pHead, void** pOut) {
    tPhysics_object* obj;
    void** tail;
    void** p;
    float x_min;
    float x_max;
    void* local;

    tail = pOut;
    if (pHead->parent != NULL) {
        for (obj = pHead; obj != NULL; obj = obj->next) {
            if ((obj == pObject && (pObject->flags & 8) == 0) || BoundsOverlapTest__finteray(&pObject->field_0x10c, &obj->field_0x10c)) {
                *tail = obj;
                tail = &obj->field_0x234;
            }
        }
    } else {
        x_min = pObject->field_0x10c.min.v[0] - gFLOAT_00692dd0;
        for (obj = pObject->prev; obj != NULL; obj = obj->prev) {
            if (obj->transform_matrix.m[3][0] < x_min) {
                break;
            }
            if (BoundsOverlapTest__finteray(&pObject->field_0x10c, &obj->field_0x10c)) {
                *tail = obj;
                tail = &obj->field_0x234;
            }
        }
        x_max = pObject->field_0x10c.max.v[0] - gFLOAT_00692dd4;
        for (obj = pObject->next; obj != NULL; obj = obj->next) {
            if (obj->transform_matrix.m[3][0] > x_max) {
                break;
            }
            if (BoundsOverlapTest__finteray(&pObject->field_0x10c, &obj->field_0x10c)) {
                *tail = obj;
                tail = &obj->field_0x234;
            }
        }
        if ((pObject->flags & 8) == 0) {
            *tail = pObject;
            tail = &pObject->field_0x234;
        }
    }
    *tail = NULL;
    local = NULL;
    p = &local;
    for (obj = *pOut; obj != NULL; obj = (tPhysics_object*)obj->field_0x234) {
        if (obj->child != NULL) {
            p = QuickHingeCollect(pObject, obj->child, p);
        }
    }
    if (local != NULL) {
        *p = NULL;
        *tail = local;
        return p;
    }
    return tail;
}

// UNLABELED: CARMA2_HW 0x004bd9e0
static int C2_HOOK_FASTCALL QuickHingeGen(tPhysics_object* pObject, void* pTmp, void* pBuffer1, int pArg, tWorld_callbacks* pCallbacks, tPhysics_object* pHead) {
    (void)pObject;
    (void)pTmp;
    (void)pBuffer1;
    (void)pArg;
    (void)pCallbacks;
    (void)pHead;
    return 0;
}

static void ResetObjectAndChildren(tPhysics_object* pObject);

// FUNCTION: CARMA2_HW 0x004baec0
int C2_HOOK_FASTCALL SetUpQuickHingeData(tPhysics_object** pObject_list, tPhysics_object* pObject, void* pBuffer1, void* pBuffer2, int pArg, tWorld_callbacks* pWorld_callbacks) {
    tPhysics_object* head;
    tPhysics_object* cur;
    tPhysics_object* next;
    tPhysics_object* child;
    void* out;
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
        QuickHingeCollect(cur, head, &out);
        cnt = QuickHingeGen(cur, &out, pBuffer1, prev, pWorld_callbacks, head);
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
    return 0;
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
    char bufB[0xF00];
    char bufA[0xF00];

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
    tCollision_info_owner* owner;

    if (gPHIL_enabled) {
        return 0;
    }

    owner = pObject->field_0x240;
    if (pObject->field_0x239 == 2) {
        memcpy((char*)owner + 0x30, pMatrix, sizeof(br_matrix34));
        ((char*)owner)[0x60] |= 0x4;
        return 0;
    }

    if (owner == NULL) {
        return 3;
    }
    if (owner->field_0x08 != 1) {
        return 4;
    }

    memcpy((char*)(*(void**)&owner->field_0x00) + 0x2c, pMatrix, sizeof(br_matrix34));
    return 0;
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
    tPhysics_object* pObject;

    pObject = pObjects;
    while (pObject != NULL) {
        tPhysics_object* child;

        if (pObject->field_0xf0 == 1) {
            BrMatrix34Copy(&pObject->actor->t.t.mat, (const br_matrix34*)&pObject->field_0xbc);
        } else {
            BrMatrix34LPNormalise(&pObject->actor->t.t.mat, &pObject->transform_matrix);
        }
        for (child = pObject->child; child != NULL; child = child->next) {
            ResetObjectAndChildren(child);
        }
        pObject = pObject->next;
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
#pragma inline_depth()

// FUNCTION: CARMA2_HW 0x0049cd00
int C2_HOOK_FASTCALL GetHierarchyNetworkStuff(tPhysics_object* pObject, tU8* pNet_data, int pRemaining) {
    int size;
    int total_size;
    tPhysics_object* child;

    total_size = GetObjectNetworkStuff(pObject, pNet_data, pRemaining);
    if (total_size < 0) {
        return -1;
    }
    child = pObject->child;
    pRemaining -= total_size;
    if (child != NULL) {
        *pNet_data |= 0x80; // Mark parent node
        for (; child != NULL; child = child->next) {
            size = GetHierarchyNetworkStuff(child, &pNet_data[total_size], pRemaining);
            if (size < 0) {
                return -1;
            }
            pNet_data[total_size] |= 0x40; // Mark child node
            total_size += size;
            pRemaining -= size;
        }
        pNet_data[total_size] &= ~0x40; //
    }
    return total_size;
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
