#include "phil.h"

#include "physics.h"
// GLOBAL: CARMA2_HW 0x0065d004
int gPHIL_enabled = 1;

// ScalarToFraction

// FractionToScalar

// GetOrientationFromMatrix

// GetMatrixFromOrientation

// OmegaCompScalarTo16

// OmegaComp16ToScalar

// GetOmega16FromOmega

// GetOmegaFromOmega16

// GetObjectNetworkStuff

// GetVelocityAndPosition

// SizeOfObjectNetworkStuff

// WriteObjectNetworkStuff

// GetHierarchyNetworkStuff

// FindObject

// WriteHierarchyNetworkStuff

// GetHierarchyNetworkSize

// GetNetworkDataSize

// GetSingleMatrixFromNetworkData

// PHILInit

// FUNCTION: CARMA2_HW 0x004b5d20
void C2_HOOK_FASTCALL PHILDisable(void) {
    gPHIL_enabled = 1;
}

// PHILAddObject

// FUNCTION: CARMA2_HW 0x004b5ea0
int C2_HOOK_FASTCALL PHILRemoveObject(tPhysics_object* pObject) {
    tPHIL_queued_header* object_info;
    tPhysics_object* child;

    if (gPHIL_munging_objects && !gPHIL_object_added) {
        gPHIL_queued_objects_for_removal[gPHIL_count_queued_objects_for_removal] = pObject;
        gPHIL_count_queued_objects_for_removal += 1;
        pObject->field_0x239 = 0;
        return 0;
    }

    if (pObject->physics_joint1 != NULL) {
        FreePhysicsJoint(pObject->physics_joint1);
        pObject->physics_joint1 = NULL;
    }
    if (pObject->physics_joint2 != NULL) {
        FreePhysicsJoint(pObject->physics_joint2);
        pObject->physics_joint2 = NULL;
    }

    object_info = pObject->field_0x240;
    if (object_info != NULL) {
        object_info->collision_info = NULL;
        pObject->field_0x240 = NULL;
    }

    pObject->flags &= ~0x20;
    pObject->field_0x239 = 0;

    if (gPHIL_list_collision_infos == pObject) {
        gPHIL_list_collision_infos = pObject->next;
    }
    if (pObject->prev != NULL) {
        pObject->prev->next = pObject->next;
    }
    if (pObject->next != NULL) {
        pObject->next->prev = pObject->prev;
    }
    pObject->next = NULL;
    pObject->prev = NULL;
    if (gPHIL_count_list_collision_infos > 0) {
        gPHIL_count_list_collision_infos -= 1;
    }

    for (child = pObject->child; child != NULL; child = child->next) {
        PHILRemoveObject(child);
    }
    return 0;
}

// PHILGetFirstObject

// PHILGetNextObject

// PHILReturnObjectStatus

// PHILMakeObjectPassive

// PHILMakeObjectActive

// PHILSetPassiveObjectsMatrix

// PHILAddActiveObject

// PHILAddObjectImmediately

// PHILAddActiveObjectImmediately

// PHILSetObjectProperty

// PHILGetObjectProperty

// FlushQueuedAddsAndRemoves

// ChangedObjectsCallbacks

// ProcessDrag2

// ProcessDrag

// MarkObjectAndChildrenAsPassive

// SetStandardGravity

// ProcessGravity

// LevelOutOnSurface

// PHILMungeObjects

// PHILActivatePassive

// PHILInterpolateObjects

// PHILDoPhysics

// PHILApplyPHILObject

// PHILGetPHILObjectState

// PhysicsObjectSetImpulse

// PhysicsObjectMoveVelocity

// FUNCTION: CARMA2_HW 0x004c29d0
void C2_HOOK_FASTCALL PhysicsObjectSetImpulse(tPhysics_object* pObject, br_vector3* pImpulse) {
    tPhysics_object* pChild;

    pObject->field_0x54 = *pImpulse;
    for (pChild = pObject->child; pChild != NULL; pChild = pChild->next) {
        PhysicsObjectSetImpulse(pChild, pImpulse);
    }
}

// FUNCTION: CARMA2_HW 0x004c2910
void C2_HOOK_FASTCALL PhysicsObjectMoveVelocity(tPhysics_object* pObject) {
    tPhysics_object* pChild;

    if (!pObject->disable_move_rotate) {
        pObject->v.v[0] += pObject->field_0x54.v[0];
        pObject->v.v[1] += pObject->field_0x54.v[1];
        pObject->v.v[2] += pObject->field_0x54.v[2];
    }
    for (pChild = pObject->child; pChild != NULL; pChild = pChild->next) {
        PhysicsObjectMoveVelocity(pChild);
    }
}