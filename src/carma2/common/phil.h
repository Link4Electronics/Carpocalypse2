#ifndef GUARD_PHIL_H
#define GUARD_PHIL_H

#include "c2_hooks.h"
#include "carpocalypse2_types.h"

// ScalarToFraction

// FractionToScalar

extern void C2_HOOK_FASTCALL GetOrientationFromMatrix(tU16* pOrientation, const br_matrix34* pMatrix);

extern void C2_HOOK_FASTCALL GetMatrixFromOrientation(br_matrix34* pMatrix, const tU16* pOrientation);

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
extern int C2_HOOK_FASTCALL GetNetworkDataSize(tU8* pNet_data);

// GetSingleMatrixFromNetworkData
extern int C2_HOOK_FASTCALL GetSingleMatrixFromNetworkData(tU8* pNet_data, br_matrix34* pMatrix);

// PHILInit

extern void C2_HOOK_FASTCALL PHILDisable(void);

// PHILAddObject

extern int C2_HOOK_FASTCALL PHILRemoveObject(tPhysics_object* pObject);

// PHILGetFirstObject

// PHILGetNextObject

extern tU32 C2_HOOK_FASTCALL PHILReturnObjectStatus(tPhysics_object* pObject);

extern int C2_HOOK_FASTCALL PHILMakeObjectPassive(tPhysics_object* pObject);

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

#endif // GUARD_PHIL_H
