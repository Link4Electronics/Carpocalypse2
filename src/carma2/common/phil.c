#include "phil.h"

#include "carpocalypse2_macros.h"
#include "physics.h"

#include <brender/brender.h>

/* Quaternion APIs used by the packed orientation conversion below. */
extern br_quat* C2_HOOK_CDECL BrQuatNormalise(br_quat* q, const br_quat* qq);
extern br_matrix34* C2_HOOK_CDECL BrQuatToMatrix34(br_matrix34* mat, const br_quat* q);

extern const br_scalar gFLOAT_0058a3f8;
extern const br_scalar gFLOAT_0058a40c;

// GLOBAL: CARMA2_HW 0x0065d004
int gPHIL_enabled = 1;

// ScalarToFraction

// FractionToScalar

static double C2_HOOK_CDECL PhilOrientationScalar(double value) {
    union { br_scalar scalar; tU32 word; } bits;
    volatile tU32 stored;

    /* The integer reload forces the scalar rounding even with MSVC5. */
    bits.scalar = (br_scalar)value;
    stored = bits.word;
    bits.word = stored;
    return bits.scalar;
}

// FUNCTION: CARMA2_HW 0x0049be90
void C2_HOOK_FASTCALL GetOrientationFromMatrix(tU16* pOrientation, const br_matrix34* pMatrix) {
    br_quat quaternion;
    br_scalar value;

    BrMatrix34ToQuat(&quaternion, pMatrix);
    value = quaternion.x;
    if (value > 1.f) {
        value = 1.f;
    }
    if (value < -1.f) {
        value = -1.f;
    }
    value = (value + 1.f) * .5f;
    value *= 65534.f;
    pOrientation[0] = (tU16)(value + .5f);
    value = quaternion.y;
    if (value > 1.f) {
        value = 1.f;
    }
    if (value < -1.f) {
        value = -1.f;
    }
    value = (value + 1.f) * .5f;
    value *= 65534.f;
    pOrientation[1] = (tU16)(value + .5f);
    value = quaternion.z;
    if (value > 1.f) {
        value = 1.f;
    }
    if (value < -1.f) {
        value = -1.f;
    }
    value = (value + 1.f) * .5f;
    value *= 65534.f;
    pOrientation[2] = (tU16)(value + .5f);
    value = quaternion.w;
    if (value > 1.f) {
        value = 1.f;
    }
    if (value < -1.f) {
        value = -1.f;
    }
    value = (value + 1.f) * .5f;
    value *= 65534.f;
    pOrientation[3] = (tU16)(value + .5f);
}

// FUNCTION: CARMA2_HW 0x0049c000
void C2_HOOK_FASTCALL GetMatrixFromOrientation(br_matrix34* pMatrix, const tU16* pOrientation) {
    br_quat quaternion;
    tU16 parts[4];
    double value;
    int i;

    parts[0] = pOrientation[0];
    parts[1] = pOrientation[1];
    parts[2] = pOrientation[2];
    parts[3] = pOrientation[3];
    for (i = 0; i < 4; i++) {
        /* 0x37800100 is the original scalar; double its product unspilled. */
        value = (double)parts[i] * 1.5259254723787308e-05;
        (&quaternion.x)[i] = (br_scalar)PhilOrientationScalar((value + value) - 1.0);
    }
    BrQuatNormalise(&quaternion, &quaternion);
    BrQuatToMatrix34(pMatrix, &quaternion);
}

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

// FUNCTION: CARMA2_HW 0x0049cf50
int C2_HOOK_FASTCALL GetSingleMatrixFromNetworkData(tU8* pNet_data, br_matrix34* pMatrix) {
    br_quat quaternion;

    switch (pNet_data[0] & 0xF) {
    case 0:
        quaternion.x = (br_scalar)((const tU16*)(pNet_data + 0x14))[0] * gFLOAT_0058a40c;
        quaternion.x = quaternion.x + quaternion.x - gFLOAT_0058a3f8;
        quaternion.y = (br_scalar)((const tU16*)(pNet_data + 0x14))[1] * gFLOAT_0058a40c;
        quaternion.y = quaternion.y + quaternion.y - gFLOAT_0058a3f8;
        quaternion.z = (br_scalar)((const tU16*)(pNet_data + 0x14))[2] * gFLOAT_0058a40c;
        quaternion.z = quaternion.z + quaternion.z - gFLOAT_0058a3f8;
        quaternion.w = (br_scalar)((const tU16*)(pNet_data + 0x14))[3] * gFLOAT_0058a40c;
        quaternion.w = quaternion.w + quaternion.w - gFLOAT_0058a3f8;
        BrQuatNormalise(&quaternion, &quaternion);
        BrQuatToMatrix34(pMatrix, &quaternion);
        memcpy(pMatrix->m[3], pNet_data + 0x1C, sizeof(pMatrix->m[3]));
        return 0x28;
    case 1:
    case 4:
        quaternion.x = (br_scalar)((const tU16*)(pNet_data + 4))[0] * gFLOAT_0058a40c;
        quaternion.x = quaternion.x + quaternion.x - gFLOAT_0058a3f8;
        quaternion.y = (br_scalar)((const tU16*)(pNet_data + 4))[1] * gFLOAT_0058a40c;
        quaternion.y = quaternion.y + quaternion.y - gFLOAT_0058a3f8;
        quaternion.z = (br_scalar)((const tU16*)(pNet_data + 4))[2] * gFLOAT_0058a40c;
        quaternion.z = quaternion.z + quaternion.z - gFLOAT_0058a3f8;
        quaternion.w = (br_scalar)((const tU16*)(pNet_data + 4))[3] * gFLOAT_0058a40c;
        quaternion.w = quaternion.w + quaternion.w - gFLOAT_0058a3f8;
        BrQuatNormalise(&quaternion, &quaternion);
        BrQuatToMatrix34(pMatrix, &quaternion);
        return 0xC;
    case 2:
    case 3:
        quaternion.x = (br_scalar)((const tU16*)(pNet_data + 8))[0] * gFLOAT_0058a40c;
        quaternion.x = quaternion.x + quaternion.x - gFLOAT_0058a3f8;
        quaternion.y = (br_scalar)((const tU16*)(pNet_data + 8))[1] * gFLOAT_0058a40c;
        quaternion.y = quaternion.y + quaternion.y - gFLOAT_0058a3f8;
        quaternion.z = (br_scalar)((const tU16*)(pNet_data + 8))[2] * gFLOAT_0058a40c;
        quaternion.z = quaternion.z + quaternion.z - gFLOAT_0058a3f8;
        quaternion.w = (br_scalar)((const tU16*)(pNet_data + 8))[3] * gFLOAT_0058a40c;
        quaternion.w = quaternion.w + quaternion.w - gFLOAT_0058a3f8;
        BrQuatNormalise(&quaternion, &quaternion);
        BrQuatToMatrix34(pMatrix, &quaternion);
        return 0x10;
    default:
        PhysicsError((tPhysicsError)9, "netphys got messed up");
        return -1;
    }
}

// PHILInit

// FUNCTION: CARMA2_HW 0x004b5d20
void C2_HOOK_FASTCALL PHILDisable(void) {
    gPHIL_enabled = 1;
}

// PHILAddObject

// FUNCTION: CARMA2_HW 0x004b5ea0
int C2_HOOK_FASTCALL PHILRemoveObject(tPhysics_object* pObject) {
    tCollision_info_owner* owner;
    int i;

    if (gPHIL_enabled) {
        return 0;
    }

    if (pObject->field_0x239 == 2) {
        ((tQueued_object_info*)pObject->field_0x240)->object = NULL;
        pObject->field_0x240 = NULL;
        pObject->field_0x239 = 0;
        return 0;
    }

    if (gPHIL_munging_objects && !gPHIL_object_added) {
        gPHIL_queued_objects_for_removal[gPHIL_count_queued_objects_for_removal] = pObject;
        gPHIL_count_queued_objects_for_removal += 1;
        return 0;
    }

    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gPhil_queued_objects.headers); i++) {
        if (gPhil_queued_objects.headers[i].collision_info == pObject) {
            break;
        }
    }
    if (i == (int)CARPOCALYPSE2_ASIZE(gPhil_queued_objects.headers)
        || (owner = pObject->field_0x240) == NULL) {
        return 3;
    }
    owner->field_0x00 = NULL;
    pObject->field_0x240 = NULL;
    gPHIL_count_list_collision_infos -= 1;
    if (pObject == gList_collision_infos) {
        gList_collision_infos = pObject->next;
        if (pObject->next != NULL) {
            pObject->next->prev = NULL;
        }
    }

    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gPhil_queued_objects.headers); i++) {
        if (gPhil_queued_objects.headers[i].collision_info != NULL
            && gPhil_queued_objects.headers[i].collision_info->next == pObject) {
            break;
        }
    }
    if (i >= (int)CARPOCALYPSE2_ASIZE(gPhil_queued_objects.headers)) {
        return 0;
    }
    gPhil_queued_objects.headers[i].collision_info->next = pObject->next;
    if (pObject->next != NULL) {
        pObject->next->prev = gPhil_queued_objects.headers[i].collision_info;
    }
    return 0;
}

// PHILGetFirstObject

// PHILGetNextObject

// FUNCTION: CARMA2_HW 0x004b6010
tU32 C2_HOOK_FASTCALL PHILReturnObjectStatus(tPhysics_object* pObject) {
    tCollision_info_owner* owner;

    if (!gPHIL_enabled) {
        if (pObject->field_0x239 == 2) {
            return ((tCollision_info_owner*)pObject->field_0x240)->field_0x08;
        }
        owner = pObject->field_0x240;
        if (owner != NULL) {
            return owner->field_0x08;
        }
    }
    return 0;
}

// PHILMakeObjectPassive

// FUNCTION: CARMA2_HW 0x004b6040
int C2_HOOK_FASTCALL PHILMakeObjectPassive(tPhysics_object* pObject) {
    tPHIL_queued_header* info;

    if (gPHIL_enabled) {
        return 0;
    }
    if (pObject->field_0x239 == 2) {
        info = pObject->field_0x240;
        info->field_0x8 = 1;
        return 0;
    }
    info = pObject->field_0x240;
    if (info != NULL) {
        info->field_0x8 = 1;
        pObject->flags |= 0x20;
        return 0;
    } else {
        return 3;
    }
}

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
    if (pObject->child != NULL) {
        for (pChild = pObject->child; pChild != NULL; pChild = pChild->next) {
            PhysicsObjectMoveVelocity(pChild);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004c2970
void C2_HOOK_FASTCALL PhysicsObjectMoveVelocityList(tPhysics_object* pObject) {

    while (pObject != NULL) {
        if (!pObject->disable_move_rotate) {
            pObject->v.v[0] += pObject->field_0x54.v[0];
            pObject->v.v[1] += pObject->field_0x54.v[1];
            pObject->v.v[2] += pObject->field_0x54.v[2];
        }
        if (pObject->child != NULL) {
            PhysicsObjectMoveVelocityList(pObject->child);
        }
        pObject = pObject->next;
    }
}