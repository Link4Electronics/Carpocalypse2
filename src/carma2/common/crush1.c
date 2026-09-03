#include "crush1.h"
#include "crush.h"
#include "physics.h"
#include "globvars.h"
#include "compress.h"
#include "utility.h"
#include "carpocalypse2_macros.h"

#include "loading1.h"
#include "platform.h"

#include <brender/brender.h>

#include "c2_string.h"
// GLOBAL: CARMA2_HW 0x00679698
br_scalar gDistortion_factor;

// GLOBAL: CARMA2_HW 0x006796b0
br_scalar gForce_to_movement_factor;

// GLOBAL: CARMA2_HW 0x00679550
br_scalar gMax_detach_time_ms;

// GLOBAL: CARMA2_HW 0x006796b8
br_scalar gMin_crush_force;

// GLOBAL: CARMA2_HW 0x0067944c
br_scalar gNormal_force_to_detach;

// GLOBAL: CARMA2_HW 0x0067a18c
br_scalar gMin_bend_angle;

// GLOBAL: CARMA2_HW 0x0067b7b0
int gMin_bend_damage;

// GLOBAL: CARMA2_HW 0x0067b7b4
br_scalar gFlap_inertia_fudge_biscuit;

// GLOBAL: CARMA2_HW 0x0067b7d0
int gMax_split_damage;

// GLOBAL: CARMA2_HW 0x0067bac8
br_scalar gMin_force_to_split_XZ_per_tonne;

// GLOBAL: CARMA2_HW 0x0067bacc
br_scalar gMax_crush_dist_sq;

// GLOBAL: CARMA2_HW 0x0067bd5c
br_scalar gMin_force_to_split_Y_per_tonne;

// GLOBAL: CARMA2_HW 0x0067bd64
int gMin_split_damage;

// GLOBAL: CARMA2_HW 0x0067bde8
br_scalar gMin_bend_force;

// GLOBAL: CARMA2_HW 0x0067bdec
br_scalar gMax_crush_force;

// GLOBAL: CARMA2_HW 0x0067bdf4
int gMax_bend_damage;

// GLOBAL: CARMA2_HW 0x0067bdf8
br_scalar gTorque_to_snap_per_tonne;

// GLOBAL: CARMA2_HW 0x0067be00
br_scalar gChance_of_bending;

// GLOBAL: CARMA2_HW 0x0067be04
br_scalar gChance_of_inverse_buckle;

// GLOBAL: CARMA2_HW 0x0067be78
br_vector3 gBatty_gravity;

// FUNCTION: CARMA2_HW 0x00429bb0
void C2_HOOK_FASTCALL ReadCrushSettings(FILE* pF) {
    char s[256];
    char *result;

    while (1) {
        result = GetALineAndDontArgue(pF, s);
        if (result == NULL) {
            break;
        }
        if (strcmp("START OF CRUSH SETTINGS", s) == 0) {
            break;
        }
    }
    if (result == NULL) {
        PDFatalError("Can't find start of CRUSH SETTINGS in .TXT file");
    }
    if (GetAnInt(pF) != 4) {
        PDFatalError("Wrong version of CRUSH SETTINGS");
    }
    /* CRUSHING */
    gDistortion_factor = GetAScalar(pF);
    gMin_crush_force = GetAScalar(pF);
    gMax_crush_force = GetAScalar(pF);
    gForce_to_movement_factor = GetAScalar(pF);
    gMax_crush_dist_sq = GetAScalar(pF);

    /* SPLITTING */
    gMin_force_to_split_XZ_per_tonne = GetAScalar(pF);
    gMin_force_to_split_Y_per_tonne = GetAScalar(pF);
    gMin_split_damage = GetAnInt(pF);
    gMax_split_damage = GetAnInt(pF);

    /* BUCKLING */
    gChance_of_inverse_buckle = GetAScalar(pF);

    /* FLAPPING AND JOINT SNAPPING */
    gFlap_inertia_fudge_biscuit = GetAScalar(pF);
    gBatty_gravity.v[1] = GetAScalar(pF);
    gTorque_to_snap_per_tonne = GetAScalar(pF);

    /* DETACHING */
    gMax_detach_time_ms = GetAScalar(pF);
    gNormal_force_to_detach = GetAScalar(pF);

    /* BENDING */
    gMin_bend_force = GetAScalar(pF);
    gChance_of_bending = GetAScalar(pF);
    gMin_bend_angle = GetAScalar(pF);
    gMin_bend_damage = GetAnInt(pF);
    gMax_bend_damage = GetAnInt(pF);
    while (1) {
        result = GetALineAndDontArgue(pF, s);
        if (result == NULL) {
            break;
        }
        if (strcmp("END OF CRUSH SETTINGS", s) == 0) {
            break;
        }
    }
    if (result == NULL) {
        PDFatalError("Can't find end of CRUSH SETTINGS in .TXT file");
    }
}

// ClearCrushLists

// RemoveCarFromCrushLists

// FUNCTION: CARMA2_HW 0x00429fa0
void C2_HOOK_FASTCALL InitCrushSystems(void) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;

    C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(gTrack_crush_joints) != 32);

    for (i = 0; i < CARPOCALYPSE2_ASIZE(gTrack_crush_joints); i++) {
        gTrack_crush_joints[i] = AllocatePhysicsJoint(1, kMem_physics_joint);
        gTrack_crush_joints[i]->type = eJoint_none;
    }
#else
    NOT_IMPLEMENTED();
#endif
}

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

// FUNCTION: CARMA2_HW 0x0042c420
void C2_HOOK_FASTCALL DisposeMasterCrushData(tCar_crush_spec* pCar_crush, tCar_spec* pCar) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;

    if (pCar_crush->field_0x4 != NULL) {
        for (i = 0; i < pCar_crush->count_shapes; i++) {
            tCar_crush_shape_info* shape_info = &pCar_crush->field_0x4[i];

            if (shape_info->field_0x0 != NULL) {
                BrMemFree(shape_info->field_0x0);
            }
            if (shape_info->field_0x4 != NULL) {
                BrMemFree(shape_info->field_0x4);
            }
            if (shape_info->field_0x18 != NULL) {
                BrMemFree(shape_info->field_0x18);
            }
        }
        BrMemFree(pCar_crush->field_0x4);
        pCar_crush->field_0x4 = NULL;
    }
    if (pCar_crush->field_0x1cc != NULL) {
        BrMemFree(pCar_crush->field_0x1cc);
        pCar_crush->field_0x1cc = NULL;
    }
    for (i = 0; i < 4; i++) {
        if (pCar_crush->field_0x564[i] != NULL) {
            BrMemFree(pCar_crush->field_0x564[i]);
            pCar_crush->field_0x564[i] = NULL;
        }
    }
    if (pCar_crush->network_stuff != NULL) {
        BrMemFree(pCar_crush->network_stuff);
        pCar_crush->network_stuff = NULL;
    }
#else
    NOT_IMPLEMENTED();
#endif
}

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

intptr_t C2_HOOK_CDECL TotallyRepairCarCB(br_actor* pActor, void* data) {
    tCar_spec* pCar_spec = data;
    tUser_crush_data* user_crush_data;
    int i;

    user_crush_data = pActor->user;
    if (user_crush_data == NULL || user_crush_data->crush_data == NULL) {
        return 0;
    }
    for (i = 0; i < pCar_spec->count_detail_levels; i++) {
        br_model* model = user_crush_data->models[i];
        if (model != NULL && model->user != NULL && model->nvertices != 0) {
            tUser_detail_level_model* model_user_data = model->user;
            if (model_user_data->field_0x4 != NULL) {
                int j;
                for (j = 0; j < model->nvertices; j++) {
                    ExpandVector3(&model->vertices[j].p, &model_user_data->field_0x4[j].p, -10.f, 10.f);
                    model_user_data->field_0x4[j].flags = 0;
                }
            }
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00439510
void C2_HOOK_FASTCALL TotallyRepairACar(tCar_spec* pCar_spec) {
#ifndef CARPOCALYPSE2_MATCHING
    if (pCar_spec->car_crush_spec == NULL) {
        return;
    }
    DRActorEnumRecurse(pCar_spec->car_model_actor, TotallyRepairCarCB, pCar_spec);
#else
    NOT_IMPLEMENTED();
#endif
}

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