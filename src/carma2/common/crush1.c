#include "crush1.h"
#include "crush.h"
#include "errors.h"
#include "physics.h"
#include "globvars.h"
#include "compress.h"
#include "piping.h"
#include "spark.h"
#include "utility.h"
#include "carpocalypse2_macros.h"

#include "loading1.h"
#include "platform.h"

#include <math.h>
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
    int i;

    memset(gCrush_array_0067a190, 0, sizeof(gCrush_array_0067a190));
    gDAT_00679440 = 0;
    gDetached_bit_crush_info_buffer.capacity = 16;
    gDetached_bit_crush_info_buffer.crush_infos = BrMemAllocate(gDetached_bit_crush_info_buffer.capacity * sizeof(tCrush_info), kMem_crush_data);
    gDetached_bit_collision_infos = BrMemAllocate(gDetached_bit_crush_info_buffer.capacity * sizeof(tPhysics_object), kMem_crush_data);
    for (i = 0; i < gDetached_bit_crush_info_buffer.capacity; i++) {
        gDetached_bit_crush_info_buffer.crush_infos[i].field_0xdc = 1.f;
        gDetached_bit_crush_info_buffer.crush_infos[i].field_0xe0 = 0.28985506f;
        gDetached_bit_crush_info_buffer.crush_infos[i].collision_object = &gDetached_bit_collision_infos[i];
        gDetached_bit_collision_infos[i].shape = (tPhysics_shape*)AllocateShapePolyhedron(16, kMem_crush_data);
    }
    gDetached_bit_driver = eDriver_detached_bit;
    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gTrack_crush_joints); i++) {
        gTrack_crush_joints[i] = AllocatePhysicsJoint(3, kMem_crush_data);
    }
    gSplit_car_crush_info_buffer.capacity = 3;
    gSplit_car_crush_info_buffer.crush_infos = BrMemAllocate(gSplit_car_crush_info_buffer.capacity * sizeof(tCrush_info), kMem_crush_data);
    gSplit_car_collision_infos = BrMemAllocate(gSplit_car_crush_info_buffer.capacity * sizeof(tPhysics_object), kMem_crush_data);
    for (i = 0; i < gSplit_car_crush_info_buffer.capacity; i++) {
        int j;
        tPhysics_shape* shape = NULL;

        gSplit_car_crush_info_buffer.crush_infos[i].field_0xdc = 0.f;
        gSplit_car_crush_info_buffer.crush_infos[i].collision_object = &gSplit_car_collision_infos[i];
        for (j = 0; j < 4; j++) {
            tPhysics_shape* polyhedron = (tPhysics_shape*)AllocateShapePolyhedron(16, kMem_crush_data);

            if (shape != NULL) {
                shape->common.next = polyhedron;
            } else {
                gSplit_car_collision_infos[i].shape = polyhedron;
            }
            shape = polyhedron;
        }
        if (shape != NULL) {
            shape->common.next = NULL;
        }
    }
    gSplit_car_driver = eDriver_split_car;
    gGonad_sphere_collision_shape = (tPhysics_shape*)AllocateShapeSphere(kMem_crush_data);
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

float C2_HOOK_FASTCALL DistanceToBounds(const br_vector3* pPoint, const br_bounds3* pBounds) {
    float closest_distance = BR_SCALAR_MAX;
    double ds;
    int i;

    for (i = 0; i < 3; i++) {
        ds = fabs((double)pPoint->v[i] - pBounds->min.v[i]);
        if (!(ds >= closest_distance)) {
            closest_distance = (float)ds;
        }
        ds = fabs((double)pPoint->v[i] - pBounds->max.v[i]);
        if (!(ds >= closest_distance)) {
            closest_distance = (float)ds;
        }
    }
    return closest_distance;
}

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

    if (pCar_crush == NULL) {
        return;
    }
    if (pCar_crush->actor != NULL) {
        BrActorFree(pCar_crush->actor);
    }
    if (pCar_crush->field_0x1cc != NULL) {
        BrMemFree(pCar_crush->field_0x1cc);
    }
    for (i = 0; i < pCar_crush->count_shapes; i++) {
        BrMemFree(pCar_crush->field_0x4[i].field_0x18);
        BrMemFree(pCar_crush->field_0x4[i].field_0x0);
        BrMemFree(pCar_crush->field_0x4[i].field_0x4);
    }
    BrMemFree(pCar_crush->field_0x4);
    if (pCar->collision_info != NULL) {
        pCar->collision_info->shape = pCar_crush->field_0xc;
    }
    if (pCar_crush->network_stuff != NULL) {
        BrMemFree(pCar_crush->network_stuff);
    }
    for (i = 0; i < 4; i++) {
        BrMemFree(pCar_crush->field_0x564[i]);
    }
    BrMemFree(pCar_crush);
#else
    int i;

    if (pCar_crush == NULL) {
        return;
    }
    BrActorFree(pCar_crush->actor);
    if (pCar_crush->field_0x1cc != NULL) {
        BrMemFree(pCar_crush->field_0x1cc);
    }
    for (i = 0; i < pCar_crush->count_shapes; i++) {
        BrMemFree(pCar_crush->field_0x4[i].field_0x18);
        BrMemFree(pCar_crush->field_0x4[i].field_0x0);
        BrMemFree(pCar_crush->field_0x4[i].field_0x4);
    }
    BrMemFree(pCar_crush->field_0x4);
    pCar->collision_info->shape = pCar_crush->field_0xc;
    if (pCar_crush->network_stuff != NULL) {
        BrMemFree(pCar_crush->network_stuff);
    }
    for (i = 0; i < 4; i++) {
        BrMemFree(pCar_crush->field_0x564[i]);
    }
    BrMemFree(pCar_crush);
#endif
}

// FUNCTION: CARMA2_HW 0x0042c4f0
void C2_HOOK_FASTCALL DisposeCrushData(tCar_crush_buffer_entry* pCrush_data) {
    if (pCrush_data != NULL) {
        if (pCrush_data->flap_data != NULL) {
            BrMemFree(pCrush_data->flap_data);
        }
        if (pCrush_data->detach_data != NULL) {
            BrMemFree(pCrush_data->detach_data);
        }
        DisposeSmashData(pCrush_data);
        BrMemFree(pCrush_data);
    }
}

// FUNCTION: CARMA2_HW 0x0042c530
void C2_HOOK_FASTCALL DisposeVertexData(tModel_detail_vertex_data* pVertex_data) {
    if (pVertex_data != NULL) {
        BrMemFree(pVertex_data);
    }
}

// BuckleTheBugger

// FUNCTION: CARMA2_HW 0x0042c8b0
float C2_HOOK_FASTCALL DistSq(const br_vector3* pP, int pCount, const br_vector3* pVertices) {
    switch (pCount) {
    case 0:
        return gCrushZero;
    case 1:
        return Vector3DistanceSquared(pP, &pVertices[0]);
    case 2:
        return PointEdgeDistSq(pP, &pVertices[0], &pVertices[1]);
    case 3:
        return PointTriangleDistSq(pP, &pVertices[0], &pVertices[1], &pVertices[2]);
    case 4:
        return PointRectDistSq(pP, &pVertices[0], &pVertices[1], &pVertices[2]);
    default:
        return PointSimplexDistSq(pP, pCount, pVertices);
    }
}

// FUNCTION: CARMA2_HW 0x0042ca30
float C2_HOOK_FASTCALL PointTriangleDistSq(const br_vector3* pP, const br_vector3* pA, const br_vector3* pB, const br_vector3* pC) {
    br_vector3 ab;
    br_vector3 bc;
    br_vector3 normal;
    br_vector3 projected;
    br_vector3 ca;
    float distance;

    BrVector3Sub(&ab, pB, pA);
    BrVector3Sub(&bc, pC, pB);
    BrVector3Cross(&normal, &ab, &bc);
    BrVector3Normalise(&normal, &normal);
    distance = (pP->v[0] - pA->v[0]) * normal.v[0] + (pP->v[1] - pA->v[1]) * normal.v[1] + (pP->v[2] - pA->v[2]) * normal.v[2];
    BrVector3Scale(&projected, &normal, distance);
    BrVector3Sub(&projected, pP, &projected);
    if (OutsideEdgeLine(&projected, pA, &ab, pC)) {
        return PointEdgeDistSq(pP, pA, pB);
    }
    if (OutsideEdgeLine(&projected, pB, &bc, pA)) {
        return PointEdgeDistSq(pP, pB, pC);
    }
    BrVector3Sub(&ca, pA, pC);
    if (OutsideEdgeLine(&projected, pA, &ca, pB)) {
        return PointEdgeDistSq(pP, pA, pC);
    }
    return distance * distance;
}

// FUNCTION: CARMA2_HW 0x0042cc40
int C2_HOOK_FASTCALL OutsideEdgeLine(const br_vector3* pPoint, const br_vector3* pA, const br_vector3* pDirection, const br_vector3* pInside) {
    br_vector3 direction;
    br_vector3 offset;
    br_vector3 inside_offset;

    BrVector3Normalise(&direction, pDirection);
    BrVector3Sub(&offset, pPoint, pA);
    /* Each new component changes the dot product used for the next one. */
    direction.v[0] *= BrVector3Dot(&offset, &direction);
    direction.v[1] *= BrVector3Dot(&offset, &direction);
    direction.v[2] *= BrVector3Dot(&offset, &direction);
    BrVector3Sub(&offset, &offset, &direction);
    BrVector3Sub(&inside_offset, pInside, pA);
    return !(BrVector3Dot(&offset, &inside_offset) >= 0.0f);
}

// FUNCTION: CARMA2_HW 0x0042ce00
float C2_HOOK_FASTCALL PointRectDistSq(const br_vector3* pP, const br_vector3* pA, const br_vector3* pB, const br_vector3* pC) {
    const br_vector3* origin = pA;
    br_vector3 u;
    br_vector3 v;
    br_vector3 offset;
    br_vector3 projected;
    br_vector3 residual;
    float u_scale;
    float v_scale;

    BrVector3Sub(&u, pC, pA);
    BrVector3Sub(&v, pB, pA);
    if (fabs(BrVector3Dot(&u, &v)) > 0.001f) {
        origin = pB;
        BrVector3Sub(&u, pC, pB);
        BrVector3Sub(&v, pA, pB);
        if (fabs(BrVector3Dot(&u, &v)) > 0.001f) {
            origin = pC;
            BrVector3Sub(&u, pB, pC);
            BrVector3Sub(&v, pA, pC);
        }
    }
    BrVector3Sub(&offset, pP, origin);
    v_scale = BrVector3Dot(&offset, &v) / BrVector3LengthSquared(&v);
    u_scale = BrVector3Dot(&offset, &u) / BrVector3LengthSquared(&u);
    if (!(v_scale >= 0.0f)) {
        v_scale = 0.0f;
    } else if (v_scale > 1.0f) {
        v_scale = 1.0f;
    }
    if (!(u_scale >= 0.0f)) {
        u_scale = 0.0f;
    } else if (u_scale > 1.0f) {
        u_scale = 1.0f;
    }
    projected.v[0] = u_scale * u.v[0] + v_scale * v.v[0] + origin->v[0];
    projected.v[1] = u_scale * u.v[1] + v_scale * v.v[1] + origin->v[1];
    projected.v[2] = u_scale * u.v[2] + v_scale * v.v[2] + origin->v[2];
    BrVector3Sub(&residual, &projected, pP);
    return BrVector3LengthSquared(&residual);
}

float C2_HOOK_FASTCALL PointSimplexDistSq(const br_vector3* pP, int pCount, const br_vector3* pVertices) {
    float closest_distance = BR_SCALAR_MAX;
    int i;

    for (i = 0; i < pCount; i++) {
        int j;

        for (j = i + 1; j < pCount; j++) {
            int k;

            for (k = j + 1; k < pCount; k++) {
                float distance = PointTriangleDistSq(pP, &pVertices[i], &pVertices[j], &pVertices[k]);

                if (distance < closest_distance) {
                    closest_distance = distance;
                }

            }
        }
    }
    return closest_distance;
}

// LimitImpactScale

// GetImpactScale

// FUNCTION: CARMA2_HW 0x0042d0e0
int C2_HOOK_FASTCALL TooFarToCrush(const br_model* pModel, int pCount, const br_vector3* pVertices, float pMax_dist_sq) {
    br_vector3 diagonal;
    float threshold;

    BrVector3Sub(&diagonal, &pModel->bounds.max, &pModel->bounds.min);
    threshold = BrVector3LengthSquared(&diagonal) + pMax_dist_sq;
    if (DistSq(&pModel->bounds.min, pCount, pVertices) <= threshold
            && DistSq(&pModel->bounds.max, pCount, pVertices) <= threshold) {
        return 0;
    }
    return 1;
}

int C2_HOOK_FASTCALL NearestAxis(const br_vector3* pVector) {
    if (fabs(pVector->v[0]) > 0.707107f) {
        return 0;
    }
    if (fabs(pVector->v[1]) > 0.707107f) {
        return 1;
    }
    return 2;
}

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

void C2_HOOK_FASTCALL RemoveDuplicatePoints(int* pCount_out, br_vector3* pPoints_out, int pCount_in, const br_vector3* pPoints_in) {
    int i;

    BrVector3Copy(&pPoints_out[0], &pPoints_in[0]);
    *pCount_out = 1;
    for (i = 1; i < pCount_in; i++) {
        int j;
        int duplicate = 0;

        for (j = 0; j < *pCount_out; j++) {
            if (!(fabs(pPoints_out[j].v[0] - pPoints_in[i].v[0]) >= 0.005f)
                && !(fabs(pPoints_out[j].v[1] - pPoints_in[i].v[1]) >= 0.005f)
                && !(fabs(pPoints_out[j].v[2] - pPoints_in[i].v[2]) >= 0.005f)) {
                duplicate = 1;
                break;
            }
        }
        if (!duplicate) {
            BrVector3Copy(&pPoints_out[*pCount_out], &pPoints_in[i]);
            *pCount_out += 1;
        }
    }
}

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

// FUNCTION: CARMA2_HW 0x00430d80
int C2_HOOK_FASTCALL QuantizeWithinBounds(br_scalar pQuant, br_scalar pMin, br_scalar pMax, br_scalar* pValue) {
    br_scalar range = pMax - pMin;
    br_scalar rounded;
    br_scalar step;
    int64_t integral;
    tS32 count;
    tS32 index;

    /* Choose a whole number of cells spanning the bounds. */
    rounded = range / pQuant;
    rounded += rounded > 0.f ? 0.5f : -0.5f;
    /* Counts and indices wrap to signed 32 bits after truncation. */
    if (rounded >= -9223372036854775808.0 && rounded < 9223372036854775808.0) {
        integral = (tU32)(int64_t)rounded;
    } else {
        integral = 0;
    }
    if (integral > 0x7fffffff) {
        integral -= (int64_t)0xffffffffu + 1;
    }
    count = (tS32)integral;
    if (count == 0) {
        count = 1;
    }
    step = range / (br_scalar)count;

    /* Snap to the grid only when the selected cell is within the bounds. */
    rounded = (*pValue - pMin) / step;
    rounded += rounded > 0.f ? 0.5f : -0.5f;
    if (rounded >= -9223372036854775808.0 && rounded < 9223372036854775808.0) {
        integral = (tU32)(int64_t)rounded;
    } else {
        integral = 0;
    }
    if (integral > 0x7fffffff) {
        integral -= (int64_t)0xffffffffu + 1;
    }
    index = (tS32)integral;
    if (index < 0 || index > count) {
        return 0;
    }
    *pValue = (br_scalar)index * step + pMin;
    return 1;
}

// SaveVitalBitsOfObject

// MakeCarModelsMaterialsDoubleSided

// SwapCarShapes

// IsAWheelOrPivotActor

// MakeCarModelsMaterialsSingleSided

// IsOnNonCarHalf

// SplitCar

// QuantizePlane
void C2_HOOK_FASTCALL QuantizePlane(br_vector4* pPlane) {
    tU16 compressed[4];

    CompressPlane(compressed, pPlane);
    ExpandPlane(pPlane, compressed);
}

// WithinBounds
int C2_HOOK_FASTCALL WithinBounds(br_scalar pValue, br_scalar pMin, br_scalar pMax) {
    return pValue >= pMin && !(pValue > pMax);
}

// FUNCTION: CARMA2_HW 0x004307e0
int C2_HOOK_FASTCALL SplitCheck(tCar_spec* pCar, int pMode, const br_vector3* pLine, br_scalar pHeight, br_vector4* pPlane) {
    tCar_crush_spec* crush;
    const br_bounds3* bounds;
    br_vector3 size;
    br_vector3 normal;
    br_vector3 point;
    br_scalar min_x;
    br_scalar max_x;
    br_scalar min_z;
    br_scalar max_z;
    br_scalar first;
    br_scalar second;
    br_scalar distance;
    int axis;
    int next_axis;
    int last_axis;
    int i;

    if (pCar->invulnerable_no_crushage != 0 && pCar->knackered == 0) {
        return 0;
    }
    if (pCar->field_0x4c8 < 1.f || pCar->field_0x4c8 > 1.f) {
        return 0;
    }
    crush = pCar->car_crush_spec;
    if (crush == NULL || crush->field_0x144 != 0 || crush->field_0x4b8 != 0) {
        return 0;
    }

    bounds = &pCar->collision_info->bb1;
    BrVector3Sub(&size, &bounds->max, &bounds->min);
    /* Keep vertical cuts within the middle half of the collision bounds. */
    min_x = bounds->min.v[0] + 0.25f * size.v[0];
    max_x = bounds->max.v[0] - 0.25f * size.v[0];
    min_z = bounds->min.v[2] + 0.25f * size.v[2];
    max_z = bounds->max.v[2] - 0.25f * size.v[2];

    if (pMode == 1) {
        pPlane->v[0] = 0.f;
        pPlane->v[1] = 1.f;
        pPlane->v[2] = 0.f;
        pPlane->v[3] = pHeight;
        if (!QuantizeWithinBounds(0.02f, bounds->min.v[1], bounds->max.v[1], &pPlane->v[3])) {
            return 0;
        }
    } else if (pMode == 0) {
        if (fabs(pLine->v[0]) > 0.001f) {
            first = (pLine->v[2] - bounds->min.v[1] * pLine->v[1]) / pLine->v[0];
            second = (pLine->v[2] - bounds->max.v[1] * pLine->v[1]) / pLine->v[0];
            if (QuantizeWithinBounds(0.02f, bounds->min.v[0], bounds->max.v[0], &first)
                    && QuantizeWithinBounds(0.02f, bounds->min.v[0], bounds->max.v[0], &second)
                    && WithinBounds(0.5f * (first + second), min_x, max_x)) {
                normal.v[0] = -size.v[1];
                normal.v[1] = 0.f;
                normal.v[2] = second - first;
                BrVector3Normalise(&normal, &normal);
                pPlane->v[0] = normal.v[0];
                pPlane->v[1] = normal.v[1];
                pPlane->v[2] = normal.v[2];
                pPlane->v[3] = pPlane->v[0] * first + pPlane->v[2] * bounds->min.v[1];
                goto check_plane;
            }
        }
        if (!(fabs(pLine->v[1]) > 0.001f)) {
            return 0;
        }
        first = (pLine->v[2] - bounds->min.v[0] * pLine->v[0]) / pLine->v[1];
        second = (pLine->v[2] - bounds->max.v[0] * pLine->v[0]) / pLine->v[1];
        if (!QuantizeWithinBounds(0.02f, bounds->min.v[2], bounds->max.v[2], &first)
                || !QuantizeWithinBounds(0.02f, bounds->min.v[2], bounds->max.v[2], &second)
                || !WithinBounds(0.5f * (first + second), min_z, max_z)) {
            return 0;
        }
        normal.v[0] = second - first;
        normal.v[1] = 0.f;
        normal.v[2] = -size.v[0];
        BrVector3Normalise(&normal, &normal);
        pPlane->v[0] = normal.v[0];
        pPlane->v[1] = normal.v[1];
        pPlane->v[2] = normal.v[2];
        pPlane->v[3] = pPlane->v[0] * bounds->min.v[0] + pPlane->v[2] * first;
    } else {
        return 0;
    }

check_plane:
    bounds = &pCar->car_actor->model->bounds;
    normal.v[0] = pPlane->v[0];
    normal.v[1] = pPlane->v[1];
    normal.v[2] = pPlane->v[2];
    axis = NearestAxis(&normal);
    next_axis = (axis + 1) % 3;
    last_axis = (axis + 2) % 3;
    point.v[axis] = 0.f;
    point.v[next_axis] = 0.5f * (bounds->min.v[next_axis] + bounds->max.v[next_axis]);
    point.v[last_axis] = 0.5f * (bounds->min.v[last_axis] + bounds->max.v[last_axis]);
    /* Leave clearance where the plane crosses the centre of the model. */
    distance = -(((point.v[1] * pPlane->v[1] + point.v[2] * pPlane->v[2])
            + point.v[0] * pPlane->v[0] - pPlane->v[3]) / pPlane->v[axis]);
    if (!(distance >= bounds->min.v[axis] + 0.057971016f)
            || distance > bounds->max.v[axis] - 0.057971016f) {
        return 0;
    }

    /* Keep the driver on the positive side of the plane. */
    distance = (crush->driver_position.v[1] * pPlane->v[1]
            + crush->driver_position.v[2] * pPlane->v[2])
            + crush->driver_position.v[0] * pPlane->v[0] - pPlane->v[3];
    if (!(distance >= 0.f)) {
        for (i = 0; i < 4; i++) {
            pPlane->v[i] = -pPlane->v[i];
        }
    }
    QuantizePlane(pPlane);
    return 1;
}

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
    int i;
    tCar_crush_spec* car_crush;

    StopCarSmokingInstantly(pCar_spec);
    if (ARIsActionReplayAvailable()) {
        PipeInstantUnSmudge(pCar_spec);
    }
    pCar_spec->repair_time += 100000;
    for (i = 0; i < CARPOCALYPSE2_ASIZE(pCar_spec->damage_units); i++) {
        pCar_spec->damage_units[i].damage_level = 0;
        pCar_spec->damage_units[i].last_level = 0;
        pCar_spec->damage_units[i].smoke_last_level = 0;
    }
    if (pCar_spec->use_shell_model) {
        return;
    }
    car_crush = pCar_spec->car_crush_spec;
    if (car_crush == NULL) {
        return;
    }
    if (car_crush->field_0x4b8 != 0) {
        WeldCar(pCar_spec);
    }
    for (i = 0; i < car_crush->field_0x2b0; i++) {
        ReAttachBit(pCar_spec, car_crush->field_0x2b4[i].field_0x4, car_crush->field_0x2b4[i].field_0x0, NULL);
    }
    for (i = 0; i < car_crush->field_0x270; i++) {
        ReAttachBit(pCar_spec, car_crush->field_0x274[i].field_0x4, car_crush->field_0x274[i].field_0x0, car_crush->field_0x274[i].field_0x8);
    }
    car_crush->field_0x270 = 0;
    DRActorEnumRecurse(pCar_spec->car_model_actor, MakeCarModelsMaterialsSingleSided, pCar_spec);
    PipeSingleDSModel(0, pCar_spec);
    if (car_crush->field_0x144) {
        DoCompletelyUnBentThings(pCar_spec);
    }
    ARStartPipingSession(24);
    PhysicsObjectRecurse(pCar_spec->collision_info, TotallyRepairObject, pCar_spec);
    AREndPipingSession();
    dr_dprintf("TotallyRepairACar() - Point 7");
    TotallyRepairCarCollisionShapes(pCar_spec);
    dr_dprintf("TotallyRepairACar() - Point 8");
    PhysicsObjectRecurse(pCar_spec->collision_info, BattenDownTheObjects, pCar_spec);
    dr_dprintf("TotallyRepairACar() - Point 19");
    if (gProgram_state.prog_status != eProg_idling) {
        dr_dprintf("TotallyRepairACar() - Point 10");
        SwapShapesIfPossible(pCar_spec);
    }
    dr_dprintf("TotallyRepairACar() - Point 11");
    pCar_spec->use_shell_model = 1;
    RemoveCarFromCrushLists(pCar_spec);
    dr_dprintf("TotallyRepairACar() - Point 12");
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

// FUNCTION: CARMA2_HW 0x0043ac90
br_scalar C2_HOOK_FASTCALL NormalizedAngle(br_angle pAngle) {
    if (pAngle < BR_ANGLE_DEG(180)) {
        return (br_scalar)(pAngle / 65536.0);
    }
    pAngle = (br_angle)-pAngle;
    return (br_scalar)(pAngle / 65536.0);
}

br_scalar C2_HOOK_FASTCALL CalculateAmountRepairedFraction(br_angle pOriginal_x, br_angle pOriginal_y, br_angle pOriginal_z, br_angle pRepair_x, br_angle pRepair_y, br_angle pRepair_z, br_scalar* pOriginal_amount) {
    br_scalar original_amount = NormalizedAngle(pOriginal_x) + NormalizedAngle(pOriginal_y) + NormalizedAngle(pOriginal_z);
    br_scalar repaired_amount = NormalizedAngle(pRepair_x) + NormalizedAngle(pRepair_y) + NormalizedAngle(pRepair_z);

    *pOriginal_amount = 2.0f * original_amount;
    return repaired_amount;
}

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

// FUNCTION: CARMA2_HW 0x0043deb0
void C2_HOOK_FASTCALL CompressPlane(tU16 pDest[4], const br_vector4* pSrc) {
    pDest[0] = DRScalarToU16(pSrc->v[0], -1.f, 1.f);
    pDest[1] = DRScalarToU16(pSrc->v[1], -1.f, 1.f);
    pDest[2] = DRScalarToU16(pSrc->v[2], -1.f, 1.f);
    pDest[3] = DRScalarToU16(pSrc->v[3], -2.5f, 2.5f);
}

// FUNCTION: CARMA2_HW 0x0043dee0
void C2_HOOK_FASTCALL ExpandPlane(br_vector4* pDest, const tU16 pSrc[4]) {
    pDest->v[0] = DRU16ToScalar(pSrc[0], -1.f, 1.f);
    pDest->v[1] = DRU16ToScalar(pSrc[1], -1.f, 1.f);
    pDest->v[2] = DRU16ToScalar(pSrc[2], -1.f, 1.f);
    pDest->v[3] = DRU16ToScalar(pSrc[3], -2.5f, 2.5f);
}

// CopySplitInMessage

// SetAllShapeStatusFlags
