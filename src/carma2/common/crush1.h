#ifndef GUARD_CRUSH1_H
#define GUARD_CRUSH1_H

#include "c2_hooks.h"
#include "carpocalypse2_types.h"

#include <stdio.h>

extern void C2_HOOK_FASTCALL ReadCrushSettings(FILE* pF);

// ClearCrushLists

// RemoveCarFromCrushLists

extern void C2_HOOK_FASTCALL InitCrushSystems(void);

// ResetCrushSystems

// ReadCrushData

// CreateDefaultCrushDataCB

// CreateDefaultCrushData

// ReadMasterCrushData

// LoadCrushDataFile

// LinkCrushDataToActor

// LinkCrushDataToActors

// AccumulateSquashVertices

// InitModelMasterCrushData

// InitNetworkShapesStuff

// IncreasingCompare

// DecreasingCompare

// CrushLimitNumber

// SoftnessOfNearestPointCB

// DRActorEnumRecurseWithTranslation

// SoftnessOfNearestPoint

// SetUpShapeLimitingStuff

// CopyShapePolyhedron

// InitShapeStuff

// InitPhysMasterCrushData

// CheckHingePointOrder

// FindNearestVertex

// DistanceToBounds

// FindNearestParentVertex

// SetUpSemiDetachJointStuff

// InitPhysCrushDataCB

// InitPhysCrushData

// CalculateReferencePoints

// InitModelCrushDataCB

// InitModelCrushData

// PointEdgeDistSq

// SetFlapCheckVertices

// InitModelVertexData

// InitVertexDataCB

// InitVertexData

// CheckWheelPositions

// InitPhysModCrushData

extern void C2_HOOK_FASTCALL DisposeMasterCrushData(tCar_crush_spec* pCar_crush, tCar_spec* pCar);
// DistanceToBounds
float C2_HOOK_FASTCALL DistanceToBounds(const br_vector3* pPoint, const br_bounds3* pBounds);

// DisposeCrushData
void C2_HOOK_FASTCALL DisposeCrushData(tCar_crush_buffer_entry* pCrush_data);

// DisposeVertexData
void C2_HOOK_FASTCALL DisposeVertexData(tModel_detail_vertex_data* pVertex_data);

extern void C2_HOOK_FASTCALL FUN_0042c4f0(tU8* pData);
extern void C2_HOOK_FASTCALL FreeIfNotNull(void* pPtr);

// DisposeCrushData

// DisposeVertexData

// BuckleTheBugger

// OutsideEdgeLine

// PointTriangleDistSq

// PointRectDistSq

// PointSimplexDistSq

// DistSq

// LimitImpactScale

// GetImpactScale

// TooFarToCrush

// NearestAxis

// CrushModel

// PossibleShockSmash

// CrushModels

// CrushPoint

// ClipPointToLimits

// CrushCollisionShapes

// SimpleCrushCarModelsCB

// SimpleCrushCar

// CrushCar

// SendCrush

// CompressCrush

// ExpandCrush

// RemoveDuplicatePoints

// SimpleCrushModel
void C2_HOOK_FASTCALL SimpleCrushModel(br_model* pModel, int pCount, float* pPoints, float* pDir, float* pBounds);
float C2_HOOK_FASTCALL DistSq(const br_vector3* pP, int pCount, const br_vector3* pVertices);

float C2_HOOK_FASTCALL PointTriangleDistSq(const br_vector3* pP, const br_vector3* pA, const br_vector3* pB, const br_vector3* pC);

int C2_HOOK_FASTCALL OutsideEdgeLine(const br_vector3* pPoint, const br_vector3* pA, const br_vector3* pDirection, const br_vector3* pInside);

float C2_HOOK_FASTCALL PointRectDistSq(const br_vector3* pP, const br_vector3* pA, const br_vector3* pB, const br_vector3* pC);

float C2_HOOK_FASTCALL PointSimplexDistSq(const br_vector3* pP, int pCount, const br_vector3* pVertices);

int C2_HOOK_FASTCALL TooFarToCrush(const br_model* pModel, int pCount, const br_vector3* pVertices, float pMax_dist_sq);

int C2_HOOK_FASTCALL NearestAxis(const br_vector3* pVector);

void C2_HOOK_FASTCALL RemoveDuplicatePoints(int* pCount_out, br_vector3* pPoints_out, int pCount_in, const br_vector3* pPoints_in);


// CrushDrone

// CrushDroneObject

// DoDroneCrushing

// AddDroneCrush

// QueueDroneCrush

// DoCrushing

// SetCrush

// QueueCarCrush

// SetBitForDetachment

// SetFlappage

// GetUniqueID

// InitializeCollisionInfo

// RemoveBit

// RemoveNonCarChildren

// RemoveNonCar

// GetNonCar

// GetNonCarObject

// SetRelationsDoingNothingFlags

// SetUpDetachedBitObject

// SetUpSemiDetachedBitJoint

// BitIsInBentPartOfCar

// SetUpFlapJoint

// MakeModelMaterialsDoubleSided

// CheckFlapAngles

// MakeModelMaterialsSingleSided

// ReAttachBit

// CentreModelOnPoint

// SemiDetachBit

// GetSDBJointPosAndBounds

// SendSemiDetachBit

// SendDetachBit

// SendAllSemiDetachedBits

// AddNonCar

// DetachBit

// DoDetaching

// FlapBit

// FlapBits

// StartFlapping

// MungeDetachLists

// SendReAttachBit

// StopFlapping

// DoFlapping

// ToggleFlapsCB2

// ToggleFlapsCB

// SendToggleFlaps

// ToggleFlaps

// BitObjectIsSufficientlyOutsideCarObjectToDetach

// FullyDetachBit

// SendFullyDetachBit

// DoFullyDetaching

// CalculateCollisionObjectStuffForSplitHalves

// SetUpCollisionInfoStuffForSplitHalf

// SetFace

// AddSplitVertexIndex

// CreateSplitVertex

// GetSplitVertex

// SplitModel

// CreateSplitPoints

// SplitCollisionShape

// SplitCarCollisionShapes

// QuantizeWithinBounds

// SaveVitalBitsOfObject

// MakeCarModelsMaterialsDoubleSided

// SwapCarShapes

// IsAWheelOrPivotActor

// MakeCarModelsMaterialsSingleSided

// IsOnNonCarHalf

// SplitCar

// QuantizePlane

// WithinBounds

// SplitCheck

// FindForceEdge

// DoXZSplitting

// DoYSplitting

// DoSplitting

// DoJointSnapping

// PointIsOutsideShape

// PointIsOutsideLimits

// ClipPointToPolyhedron

// QuantizeVector3

// MoveCarToSensiblePlace

// SwapShapesIfPossible

// DoShapeSwapping

// BendPoint

// EdgeCrossesZPlane

// BendCollisionShape

// BendCollisionShapes

// BendModel

// BendModels

// BrAngleToSignedDegree

// BendWheels

// BendActor

// BendOtherThings

// ReAttachOrFullyDetachDifficultBits

// BendCar

// QuantizeAngle

// BendCarOneForce

// DoBending

// JitModelUpdate

// SetModelForUpdate

// RandomlyXZSplitCar

// RandomlyBendCar

// TotallySpamTheCar

// DRActorEnumRecurseUltraPlus

// FireOffBitsCB

// FireSomeBitsOffCar

// DoSpams

// CrushBendFlapRend

// SphericizeModel

// SphericizePhysics

// SphericizeCar

// BattenDownTheHatches

// BattenDownTheObjects

// CompletelyUnBendCollisionShape

// CompletelyUnBendCollisionShapes

// CompletelyUnBendWheels

// CompletelyUnBendOtherThings

// EnableGroovers

// DoCompletelyUnBentThings

// TotallyRepairCarCollisionShapes

// TotallyRepairModel

// TotallyRepairModels

// TotallyRepairObject

// SendReAttachAllBits

extern void C2_HOOK_FASTCALL TotallyRepairACar(tCar_spec* pCar_spec);
extern void C2_HOOK_FASTCALL AddDroneCrush(tQueued_drone_crush_item* pCrush, const br_vector3* pVector, float pScale, int pCount, const br_vector3* pPoints);

extern int C2_HOOK_FASTCALL QuantizeWithinBounds(br_scalar pQuant, br_scalar pMin, br_scalar pMax, br_scalar* pValue);

extern void C2_HOOK_FASTCALL QuantizePlane(br_vector4* pPlane);

extern int C2_HOOK_FASTCALL WithinBounds(br_scalar pValue, br_scalar pMin, br_scalar pMax);

extern int C2_HOOK_FASTCALL SplitCheck(tCar_spec* pCar, int pMode, const br_vector3* pLine, br_scalar pHeight, br_vector4* pPlane);


// TotallyRepairCar

// AddTotalDeflectionOfModel

// AddTotalDeflection

// RepairModel

// RepairModels

// UnBendModel

// UnBendModels

// UnBendActor

// UnBendCollisionShape

// UnBendCollisionShapes

// UnBendWheels

// UnBendOtherThings

// NormalizedAngle

// CalculateAmountRepairedFraction

// UnBendCar

// CompletelyUnBendCar

// ReAttachBits

// RepairObject

// RepairCarCollisionShapes

// RepairCar2

// RepairCar

// RestoreVitalBitsOfObject

// WeldCar

// ARCompressCrush

// ARApplyCrush

// ARUndoCrush

// ARCompressModelMash

// ARApplyModelMash

// ARUndoModelMash

// ARApplyFlap

// ARApplySplitWeld

// ARUndoSplitWeld

// ReceivedCrush

// FindBitWithID

// AttachedBitFromID

// ReceivedSemiDetachBit

// ReceivedDetachBit

// SemiDetachedBitObjectFromID

// ReceivedFullyDetachBit

// DetachedBitFromID

// SemiDetachedBitFromID

// ReceivedReAttachBit

// ReceivedReAttachAllBits

// DoQueuedNetDetaching

// DBNonCarOwner

// SendDBNonCar

// ReceivedDBNonCar

// SendAllDBNonCars

// ReceivedDBNonCarPosition

// ReceivedToggleFlaps

// SHNonCarOwner

// SendSHNonCar

// SendAllSHNonCars

// ReceivedSHNonCar

// GetNetShapeDataLengthFromShape

// GetNetShapeDataLengthFromData

// EncodeNetShapeData

// DecodeNetShapeData

// CopyBendInMessage

// Vector4Equal

// CompressPlane

// ExpandPlane

// CopySplitInMessage

// SetAllShapeStatusFlags

extern void C2_HOOK_FASTCALL SetAllShapeStatusFlags(tCar_crush_network_shapes* pShapes);
// NormalizedAngle
br_scalar C2_HOOK_FASTCALL NormalizedAngle(br_angle pAngle);

br_scalar C2_HOOK_FASTCALL CalculateAmountRepairedFraction(br_angle pOriginal_x, br_angle pOriginal_y, br_angle pOriginal_z, br_angle pRepair_x, br_angle pRepair_y, br_angle pRepair_z, br_scalar* pOriginal_amount);

// ARApplyModelMash
void C2_HOOK_FASTCALL ARApplyModelMash(br_model* pModel, tCompressed_vector3* pSrc);

// ARUndoModelMash
void C2_HOOK_FASTCALL ARUndoModelMash(br_model* pModel, tCompressed_vector3* pSrc);

// ARApplyFlap
void C2_HOOK_FASTCALL ARApplyFlap(br_actor* pActor, tS16 pAngle);

// ARApplySplitWeld
void C2_HOOK_FASTCALL ARApplySplitWeld(tCar_spec* pCar_spec, br_vector3* pVector);

// ARUndoSplitWeld
void C2_HOOK_FASTCALL ARUndoSplitWeld(tCar_spec* pCar_spec);

extern void C2_HOOK_FASTCALL EncodeNetShapeData(tU8* pData, tCar_spec* pCar);

extern void C2_HOOK_FASTCALL CompressPlane(tU16 pDest[4], const br_vector4* pSrc);

extern void C2_HOOK_FASTCALL ExpandPlane(br_vector4* pDest, const tU16 pSrc[4]);


#endif // GUARD_CRUSH1_H
