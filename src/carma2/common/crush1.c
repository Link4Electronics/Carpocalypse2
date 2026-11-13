#include "crush1.h"
#include "crush.h"
#include "errors.h"
#include "physics.h"
#include "globvars.h"
#include "compress.h"

#include "smash.h"

#include "piping.h"

#include "world.h"

#include "car.h"

#include "spark.h"
#include "utility.h"
#include "carpocalypse2_macros.h"

#include "loading1.h"
#include "platform.h"

#include <math.h>
#include <brender/brender.h>

#include "c2_string.h"

extern const double gDouble_00589b40;

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
// GLOBAL: CARMA2_HW 0x00589a70
const br_scalar gFloat_00589a70 = 0.005f;

// GLOBAL: CARMA2_HW 0x00589a78
const br_scalar gFloat_00589a78 = 47.6099967956543f;

extern br_scalar gMax_crush_dist_sq;
extern br_scalar gCrushZero;
extern const br_scalar gFLOAT_00589a2c;
extern br_scalar gCrushSqrtHalf;
extern const br_scalar gFLOAT_00589a54;
extern br_scalar gFLOAT_00589a5c;
extern const double gDOUBLE_00589a60;
int C2_HOOK_FASTCALL TooFarToCrush(const br_model* pModel, int pCount, const br_vector3* pVertices, float pMax_dist_sq);
void C2_HOOK_FASTCALL FUN_004c7f40(br_model* pModel, br_vector3* pSrc, const float* pScalars);

#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_CDECL BrMatrix34PostRotate();
extern void C2_HOOK_CDECL BrMatrix34PostTranslate();
extern void C2_HOOK_CDECL BrMatrix34Translate();
extern void C2_HOOK_CDECL BrModelFree();
extern br_model* C2_HOOK_CDECL BrModelRemove();
extern void C2_HOOK_FASTCALL CompressVector3(tCompressed_vector3* pDest, const br_vector3* pSrc, float pMin, float pMax);
extern intptr_t C2_HOOK_FASTCALL DRActorEnumRecurse(br_actor* pActor, br_actor_enum_cbfn* callback, void* arg);
extern tU16 C2_HOOK_FASTCALL DRScalarToU16(float pValue, float pMin, float pMax);
extern br_scalar C2_HOOK_FASTCALL DRU16ToScalar(tU16 pValue, float pMin, float pMax);
extern void C2_HOOK_FASTCALL ExpandVector3(br_vector3* pDest, const tCompressed_vector3 *pSrc, float pMin, float pMax);
extern intptr_t C2_HOOK_CDECL MakeCarModelsMaterialsSingleSided();
extern int C2_HOOK_FASTCALL OutsideEdgeLine(const br_vector3* pPoint, const br_vector3* pA, const br_vector3* pDirection, const br_vector3* pInside);
extern float C2_HOOK_FASTCALL PointEdgeDistSq(const br_vector3* pP, const br_vector3* pA, const br_vector3* pB);
extern br_uint_8 bl;
extern char gFLOAT_00589a28[];
extern char gFLOAT_00589a7c[];
extern char gFLOAT_00589aa8[];
extern char gFLOAT_00589aac[];
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_FASTCALL AREndPipingSession(void);
extern int C2_HOOK_FASTCALL ARIsActionReplayAvailable(void);
extern void C2_HOOK_FASTCALL ARStartPipingSession(tPipe_chunk_type pType);
extern tGroovidelic_spec* C2_HOOK_FASTCALL ActorsGroove(br_actor* pActor);
extern int C2_HOOK_FASTCALL BattenDownTheObjects(tPhysics_object* pCollision_info, void* pUser_data);
extern float C2_HOOK_FASTCALL DistSq(const br_vector3* pP, int pCount, const br_vector3* pVertices);
extern intptr_t C2_HOOK_CDECL EnableGroovers();
extern void C2_NORETURN C2_HOOK_FASTCALL PDFatalError(const char* pThe_str);
extern int C2_HOOK_FASTCALL PhysicsObjectRecurse(tPhysics_object* pCollision_info, tEnumCollision_cbfn* pCallback, void* pUser_data);
extern void C2_HOOK_FASTCALL PipeInstantUnSmudge(tCar_spec* pCar_spec);
extern void C2_HOOK_FASTCALL PipeSingleDSModel(int pArg1, void* pThing);
extern void C2_HOOK_FASTCALL PipeSingleVector3(br_vector3* pDest, br_vector3* pValue);
extern void C2_HOOK_FASTCALL ReAttachBit(tCar_spec* pCar, br_actor* pParent_actor, br_actor* pActor, tPhysics_object* pObject);
extern void C2_HOOK_FASTCALL RemoveCarFromCrushLists(tCar_spec* pCar_spec);
extern void C2_HOOK_FAKE_THISCALL SetCarSuspGiveAndHeight(tCar_spec* pCar, float pFront_give_factor, float pRear_give_factor, float pDamping_factor, float pExtra_front_height, float pExtra_rear_height);
extern void C2_HOOK_FASTCALL StopCarSmokingInstantly(tCar_spec* pCar_spec);
extern int C2_HOOK_FASTCALL SwapShapesIfPossible(tCar_spec *pCar_spec);
extern void C2_HOOK_FASTCALL TotallyRepairCarCollisionShapes(tCar_spec *pCar_spec);
extern int C2_HOOK_FASTCALL TotallyRepairObject(tPhysics_object* pCollision_info, void* pUser_data);
extern void C2_HOOK_FASTCALL WeldCar(tCar_spec* pCar_spec);
extern void __cdecl _chkstk();
extern void C2_HOOK_CDECL dr_dprintf();
extern char gNext_crush_actor_id[];
extern tProgram_state gProgram_state;
#endif
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

// FUNCTION: CARMA2_HW 0x0042c540
void C2_NAKED C2_HOOK_FASTCALL SimpleCrushModel(br_model* pModel, int pCount, float* pPoints, float* pDir, float* pBounds) {
#ifndef CARPOCALYPSE2_MATCHING
    float unique[12];
    float weights[1024];
    float* pCur;
    float* pNext;
    float* pOut;
    float* pVert;
    int remaining;
    int count;
    int axis;
    int found;
    int k;
    float len;
    float scale;
    float norm_x;
    float norm_y;
    float bend;
    float a;
    float b;
    float dist_sq;
    tU16 v;

    if (pModel == NULL) {
        return;
    }
    if (pModel->nvertices > 0x400) {
        PDFatalError("SimpleCrushModel can't cope with that many vertices");
    }
    if (pCount <= 0) {
        return;
    }
    if (pCount > 4) {
        pCount = 4;
    }

    count = 1;
    unique[0] = pPoints[0];
    unique[1] = pPoints[1];
    unique[2] = pPoints[2];
    if (pCount > 1) {
        remaining = pCount - 1;
        pCur = pPoints;
        pOut = unique + 4;
        do {
            pNext = pCur + 3;
            found = 0;
            for (k = 0; k < count; k++) {
                float* pU = unique + k * 3;
                if ((float)fabs(pU[0] - pNext[0]) < gFloat_00589a70 && (float)fabs(pU[1] - pNext[1]) < gFloat_00589a70 &&
                    (float)fabs(pU[2] - pNext[2]) < gFloat_00589a70) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                pOut[-1] = pNext[0];
                pOut[0] = pNext[1];
                pOut[1] = pNext[2];
                pOut += 3;
                count++;
            }
            remaining--;
            pCur = pNext;
        } while (remaining != 0);
    }

    if (TooFarToCrush(pModel, count, (br_vector3*)unique, gMax_crush_dist_sq) != 0) {
        return;
    }

    len = (float)sqrt(pDir[0] * pDir[0] + pDir[2] * pDir[2] + pDir[1] * pDir[1]);
    if (len <= gFLOAT_00589a5c) {
        norm_y = gCrushZero;
        norm_x = gFLOAT_00589a54;
    } else {
        scale = (float)gDOUBLE_00589a60 / len;
        norm_y = pDir[1] * scale;
        norm_x = pDir[0] * scale;
    }

    if ((float)fabs(norm_x) <= gCrushSqrtHalf) {
        axis = 1;
        if ((float)fabs(norm_y) <= gCrushSqrtHalf) {
            axis = 2;
        }
    } else {
        axis = 0;
    }

    v = 0;
    if (pModel->nvertices != 0) {
        do {
            pVert = (float*)((tU8*)pModel->vertices + (tU32)v * 0x28);
            dist_sq = CrushShapeDistSq(pVert, count, unique);
            if (dist_sq <= gMax_crush_dist_sq) {
                bend = gFLOAT_00589a54 - (float)sqrt(dist_sq * gFloat_00589a78);
            } else {
                bend = gCrushZero;
            }
            weights[v] = bend;
            if (bend != gCrushZero) {
                a = (pBounds[axis] - pVert[axis]) / pDir[axis];
                b = (pBounds[axis + 3] - pVert[axis]) / pDir[axis];
                if (b <= a) {
                    a = b;
                }
                if (gCrushZero < a) {
                    if (a < bend) {
                        weights[v] = a;
                    }
                } else {
                    weights[v] = gCrushZero;
                }
                if (weights[v] != gCrushZero) {
                    pVert[0] = pVert[0] + weights[v] * pDir[0];
                    pVert[1] = pDir[1] * weights[v] + pVert[1];
                    pVert[2] = pDir[2] * weights[v] + pVert[2];
                }
            }
            v++;
        } while (v < pModel->nvertices);
    }

    FUN_004c7f40(pModel, (br_vector3*)pDir, weights);

    if ((pModel->flags & 0x20) != 0 && pModel->user != NULL) {
        *(int*)pModel->user = 1;
        return;
    }
    pModel->custom = JitModelUpdate;
    pModel->flags = pModel->flags | 0x20;
#else
    __asm {
            mov eax, 0x1050
            call _chkstk
            push ebx
            push ebp
            push esi
            push edi
            mov edi, ecx
            mov esi, edx
            test edi, edi
            mov dword ptr [esp + 0x18], edi
            je L0042c896
            cmp word ptr [edi + 0x10], 0x400
            jbe L0042c570
            mov ecx, offset gNext_crush_actor_id+0x238
            call PDFatalError
    L0042c570:
            cmp esi, 1
            jl L0042c896
            cmp esi, 4
            jle L0042c583
            mov esi, 4
    L0042c583:
            mov edx, dword ptr [esp + 0x1064]
            mov ebp, 1
            cmp esi, ebp
            mov eax, dword ptr [edx]
            mov ecx, dword ptr [edx + 4]
            mov dword ptr [esp + 0x30], eax
            mov eax, dword ptr [edx + 8]
            mov dword ptr [esp + 0x34], ecx
            mov dword ptr [esp + 0x38], eax
            jle L0042c639
            add edx, 0xc
            dec esi
            lea ebx, [esp + 0x40]
            mov dword ptr [esp + 0x10], esi
    L0042c5b7:
            xor esi, esi
            mov edi, 1
            test ebp, ebp
            jle L0042c60f
            lea ecx, [esp + 0x34]
    L0042c5c6:
            fld dword ptr [ecx - 4]
            _emit 0xd8
        _emit 0x22
            fabs
            fcomp dword ptr [gFloat_00589a70]
            fnstsw ax
            test ah, 1
            je L0042c603
            _emit 0xd9
        _emit 0x01
            fsub dword ptr [edx + 4]
            fabs
            fcomp dword ptr [gFloat_00589a70]
            fnstsw ax
            test ah, 1
            je L0042c603
            fld dword ptr [ecx + 4]
            fsub dword ptr [edx + 8]
            fabs
            fcomp dword ptr [gFloat_00589a70]
            fnstsw ax
            test ah, 1
            jne L0042c60d
    L0042c603:
            inc esi
            add ecx, 0xc
            cmp esi, ebp
            jl L0042c5c6
            jmp L0042c60f
    L0042c60d:
            xor edi, edi
    L0042c60f:
            test edi, edi
            je L0042c627
            mov ecx, dword ptr [edx]
            inc ebp
            mov dword ptr [ebx - 4], ecx
            mov eax, dword ptr [edx + 4]
            mov dword ptr [ebx], eax
            mov ecx, dword ptr [edx + 8]
            mov dword ptr [ebx + 4], ecx
            add ebx, 0xc
    L0042c627:
            mov eax, dword ptr [esp + 0x10]
            add edx, 0xc
            dec eax
            mov dword ptr [esp + 0x10], eax
            jne L0042c5b7
            mov edi, dword ptr [esp + 0x18]
    L0042c639:
            mov edx, dword ptr [gMax_crush_dist_sq]
            lea eax, [esp + 0x30]
            push edx
            push eax
            mov edx, ebp
            mov ecx, edi
            call TooFarToCrush
            test eax, eax
            jne L0042c896
            mov ebx, dword ptr [esp + 0x1068]
            fld dword ptr [ebx + 4]
            fld dword ptr [ebx + 8]
            _emit 0xd9
        _emit 0x03
            fxch st(2)
            fst dword ptr [esp + 0x10]
            fxch st(1)
            fst dword ptr [esp + 0x14]
            fxch st(1)
            fmul dword ptr [esp + 0x10]
            fxch st(1)
            fmul dword ptr [esp + 0x14]
            fld st(2)
            fmul st(0), st(3)
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fsqrt
            fstp st(1)
            fcom dword ptr [gFLOAT_00589a5c]
            fnstsw ax
            test ah, 0x41
            jne L0042c6b5
            fdivr qword ptr [gDOUBLE_00589a60]
            _emit 0xd9
        _emit 0x03
            fld dword ptr [ebx + 4]
            fxch st(1)
            fmul st(0), st(2)
            fxch st(1)
            fmul st(0), st(2)
            fxch st(1)
            fstp st(2)
            fstp dword ptr [esp + 0x28]
            jmp L0042c6c5
    L0042c6b5:
            fstp st(0)
            fld dword ptr [gFLOAT_00589a54]
            mov dword ptr [esp + 0x28], 0
    L0042c6c5:
            fabs
            fcomp dword ptr [gCrushSqrtHalf]
            fnstsw ax
            test ah, 0x41
            jne L0042c6de
            mov dword ptr [esp + 0x14], 0
            jmp L0042c701
    L0042c6de:
            fld dword ptr [esp + 0x28]
            fabs
            fcomp dword ptr [gCrushSqrtHalf]
            mov dword ptr [esp + 0x14], 1
            fnstsw ax
            test ah, 0x41
            je L0042c701
            mov dword ptr [esp + 0x14], 2
    L0042c701:
            cmp word ptr [edi + 0x10], 0
            mov dword ptr [esp + 0x10], 0
            jbe L0042c859
    L0042c714:
            mov edi, dword ptr [esp + 0x10]
            mov edx, dword ptr [esp + 0x18]
            and edi, 0xffff
            mov eax, dword ptr [edx + 8]
            mov edx, ebp
            lea ecx, [edi + edi*4]
            lea esi, [eax + ecx*8]
            lea ecx, [esp + 0x30]
            push ecx
            mov ecx, esi
            call DistSq
            fcom dword ptr [gMax_crush_dist_sq]
            fnstsw ax
            test ah, 0x41
            jne L0042c752
            fstp st(0)
            mov dword ptr [esp + 0x1c], 0
            jmp L0042c764
    L0042c752:
            fmul dword ptr [gFloat_00589a78]
            fsqrt
            fsubr dword ptr [gFLOAT_00589a54]
            fstp dword ptr [esp + 0x1c]
    L0042c764:
            fld dword ptr [esp + 0x1c]
            fcomp dword ptr [gCrushZero]
            fld dword ptr [esp + 0x1c]
            lea ecx, [esp + edi*4 + 0x60]
            _emit 0xd9
        _emit 0x19
            fnstsw ax
            test ah, 0x40
            jne L0042c840
            mov edx, dword ptr [esp + 0x14]
            mov eax, dword ptr [esp + 0x106c]
            fld dword ptr [eax + edx*4]
            fld dword ptr [esi + edx*4]
            fld dword ptr [eax + edx*4 + 0xc]
            lea edi, [edx*4]
            fld dword ptr [esi + edi]
            fld dword ptr [ebx + edi]
            fxch st(3)
            fsubp st(4), st(0)
            fsubp st(1), st(0)
            fxch st(1)
            fdivp st(2), st(0)
            fdiv dword ptr [ebx + edi]
            fld st(1)
            fxch st(1)
            fstp dword ptr [esp + 0x20]
            fcomp dword ptr [esp + 0x20]
            fnstsw ax
            test ah, 1
            jne L0042c7cb
            fstp st(0)
            fld dword ptr [esp + 0x20]
    L0042c7cb:
            fcom dword ptr [gCrushZero]
            fnstsw ax
            test ah, 0x41
            je L0042c7e2
            fstp st(0)
            mov dword ptr [ecx], 0
            jmp L0042c7f5
    L0042c7e2:
            fld dword ptr [esp + 0x1c]
            fcomp st(1)
            fnstsw ax
            test ah, 0x41
            jne L0042c7f3
            _emit 0xd9
        _emit 0x19
            jmp L0042c7f5
    L0042c7f3:
            fstp st(0)
    L0042c7f5:
            _emit 0xd9
        _emit 0x01
            fcomp dword ptr [gCrushZero]
            fnstsw ax
            test ah, 0x40
            jne L0042c840
            _emit 0xd9
        _emit 0x01
            _emit 0xd9
        _emit 0x01
            fld dword ptr [ebx + 4]
            fld dword ptr [ebx + 8]
            _emit 0xd9
        _emit 0x01
            fxch st(2)
            fmulp st(4), st(0)
            fmulp st(2), st(0)
            _emit 0xd8
        _emit 0x0b
            _emit 0xd9
        _emit 0x06
            fxch st(2)
            fst dword ptr [esp + 0x2c]
            fxch st(1)
            fstp dword ptr [esp + 0x24]
            fxch st(1)
            fadd dword ptr [esp + 0x24]
            fxch st(2)
            fadd dword ptr [esi + 4]
            fxch st(1)
            fadd dword ptr [esi + 8]
            fxch st(2)
            _emit 0xd9
        _emit 0x1e
            fstp dword ptr [esi + 4]
            fstp dword ptr [esi + 8]
    L0042c840:
            mov eax, dword ptr [esp + 0x10]
            mov edx, dword ptr [esp + 0x18]
            inc eax
            cmp ax, word ptr [edx + 0x10]
            mov dword ptr [esp + 0x10], eax
            jb L0042c714
            mov edi, edx
    L0042c859:
            lea eax, [esp + 0x60]
            mov edx, ebx
            push eax
            mov ecx, edi
            call FUN_004c7f40
            mov ax, word ptr [edi + 0x20]
            test al, 0x20
            je L0042c889
            mov ecx, dword ptr [edi + 0x28]
            test ecx, ecx
            je L0042c889
            mov dword ptr [ecx], 1
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x1050
            ret 0xc
    L0042c889:
            or al, 0x20
            mov dword ptr [edi + 0x24], 0x431590
            mov word ptr [edi + 0x20], ax
    L0042c896:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x1050
            ret 0xc
    }
#endif
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
float C2_NAKED C2_HOOK_FASTCALL PointTriangleDistSq(const br_vector3* pP, const br_vector3* pA, const br_vector3* pB, const br_vector3* pC) {
#ifndef CARPOCALYPSE2_MATCHING
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
#else
    __asm {
            sub esp, 0x30
            push ebx
            mov ebx, dword ptr [esp + 0x3c]
            push ebp
            push esi
            fld dword ptr [ebx]
            push edi
            mov edi, dword ptr [esp + 0x44]
            mov esi, edx
            mov ebp, ecx
            fld dword ptr [edi]
            fxch st(1)
            fsub dword ptr [edi]
            fld dword ptr [ebx + 4]
            fld dword ptr [ebx + 8]
            fxch st(3)
            fsub dword ptr [esi]
            fld dword ptr [edi + 4]
            fld dword ptr [edi + 8]
            fxch st(2)
            fstp dword ptr [esp + 0x10]
            fxch st(2)
            fsub dword ptr [edi + 4]
            fxch st(4)
            fsub dword ptr [edi + 8]
            fxch st(2)
            fsub dword ptr [esi + 4]
            fxch st(1)
            fsub dword ptr [esi + 8]
            fxch st(3)
            fst dword ptr [esp + 0x1c]
            fxch st(4)
            fst dword ptr [esp + 0x20]
            fxch st(2)
            fst dword ptr [esp + 0x24]
            fld dword ptr [esp + 0x24]
            fxch st(2)
            fstp dword ptr [esp + 0x14]
            fmul dword ptr [esp + 0x14]
            fxch st(3)
            fstp dword ptr [esp + 0x18]
            fxch st(1)
            fmul dword ptr [esp + 0x18]
            fxch st(3)
            fmul dword ptr [esp + 0x18]
            fxch st(3)
            fsubp st(2), st(0)
            fld dword ptr [esi]
            fld st(2)
            fmul st(0), st(3)
            fxch st(1)
            fsub dword ptr [ebx]
            fld dword ptr [esi + 4]
            fsub dword ptr [ebx + 4]
            fld dword ptr [esi + 8]
            fxch st(2)
            fstp dword ptr [esp + 0x34]
            fxch st(3)
            fmul dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x20]
            fxch st(4)
            fstp dword ptr [esp + 0x38]
            fld dword ptr [esp + 0x1c]
            fxch st(2)
            fsub dword ptr [ebx + 8]
            fstp dword ptr [esp + 0x3c]
            fxch st(3)
            fmul dword ptr [esp + 0x10]
            fxch st(1)
            fmul dword ptr [esp + 0x14]
            fxch st(3)
            fsubp st(5), st(0)
            fld st(4)
            fxch st(3)
            fsubp st(1), st(0)
            fxch st(2)
            fmul st(0), st(4)
            fld st(2)
            fmul st(0), st(3)
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fsqrt
            fcom dword ptr [gFLOAT_00589a5c]
            fxch st(2)
            fxch st(3)
            fxch st(2)
            fnstsw ax
            test ah, 0x41
            jne L0042cb2e
            fdivr qword ptr [gDOUBLE_00589a60]
            fld st(0)
            fld st(1)
            fxch st(1)
            fmulp st(5), st(0)
            fmulp st(3), st(0)
            fmulp st(1), st(0)
            jmp L0042cb48
    L0042cb2e:
            fstp st(0)
            fstp st(0)
            fstp st(0)
            fstp st(0)
            fld dword ptr [gFLOAT_00589a54]
            fld dword ptr [gCrushZero]
            fld dword ptr [gCrushZero]
    L0042cb48:
            fld dword ptr [ebp + 8]
            fld dword ptr [ebp + 4]
            fsub dword ptr [esi + 4]
            fxch st(1)
            fsub dword ptr [esi + 8]
            fld dword ptr [ebp]
            fsub dword ptr [esi]
            fxch st(2)
            fmul st(0), st(4)
            fxch st(1)
            fmul st(0), st(3)
            fxch st(2)
            fmul st(0), st(5)
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            lea eax, [esp + 0x10]
            push ebx
            push eax
            mov edx, esi
            fst dword ptr [esp + 0x50]
            fld dword ptr [esp + 0x50]
            fld dword ptr [esp + 0x50]
            fxch st(2)
            fmul st(0), st(5)
            fxch st(1)
            fmul st(0), st(4)
            fxch st(2)
            fmul st(0), st(3)
            fxch st(1)
            fsubr dword ptr [ebp]
            fxch st(2)
            fsubr dword ptr [ebp + 4]
            fxch st(1)
            fsubr dword ptr [ebp + 8]
            fxch st(2)
            fstp dword ptr [esp + 0x30]
            lea ecx, [esp + 0x30]
            fstp dword ptr [esp + 0x34]
            fstp dword ptr [esp + 0x38]
            fstp st(0)
            fstp st(0)
            fstp st(0)
            call OutsideEdgeLine
            test eax, eax
            je L0042cbd3
            push edi
            mov edx, esi
            mov ecx, ebp
            call PointEdgeDistSq
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x30
            ret 8
    L0042cbd3:
            lea ecx, [esp + 0x1c]
            push esi
            push ecx
            mov edx, edi
            lea ecx, [esp + 0x30]
            call OutsideEdgeLine
            test eax, eax
            je L0042cbfc
            push ebx
            mov edx, edi
            mov ecx, ebp
            call PointEdgeDistSq
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x30
            ret 8
    L0042cbfc:
            lea edx, [esp + 0x34]
            push edi
            push edx
            mov edx, esi
            lea ecx, [esp + 0x30]
            call OutsideEdgeLine
            test eax, eax
            je L0042cc25
            push ebx
            mov edx, esi
            mov ecx, ebp
            call PointEdgeDistSq
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x30
            ret 8
    L0042cc25:
            fld dword ptr [esp + 0x48]
            fmul dword ptr [esp + 0x48]
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x30
            ret 8
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0042cc40
int C2_NAKED C2_HOOK_FASTCALL OutsideEdgeLine(const br_vector3* pPoint, const br_vector3* pA, const br_vector3* pDirection, const br_vector3* pInside) {
#ifndef CARPOCALYPSE2_MATCHING
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
#else
    __asm {
            sub esp, 0x1c
            push esi
            mov esi, dword ptr [esp + 0x24]
            fld dword ptr [esi + 4]
            fld dword ptr [esi + 8]
            fld dword ptr [esi]
            fxch st(2)
            fst dword ptr [esp + 0x24]
            fxch st(1)
            fst dword ptr [esp + 4]
            fxch st(1)
            fmul dword ptr [esp + 0x24]
            fxch st(1)
            fmul dword ptr [esp + 4]
            fld st(2)
            fmul st(0), st(3)
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fsqrt
            fstp st(1)
            fcom dword ptr [gFLOAT_00589a5c]
            fnstsw ax
            test ah, 0x41
            jne L0042ccb5
            fdivr qword ptr [gDOUBLE_00589a60]
            fld dword ptr [esi + 4]
            fld dword ptr [esi + 8]
            fld dword ptr [esi]
            fxch st(2)
            fxch st(1)
            fxch st(2)
            fmul st(0), st(3)
            fxch st(1)
            fmul st(0), st(3)
            fxch st(2)
            fmul st(0), st(3)
            fxch st(1)
            fstp dword ptr [esp + 8]
            fxch st(1)
            fstp dword ptr [esp + 0xc]
            fstp dword ptr [esp + 0x10]
            fstp st(0)
            jmp L0042cccf
    L0042ccb5:
            fstp st(0)
            mov dword ptr [esp + 8], 0x3f800000
            mov dword ptr [esp + 0xc], 0
            mov dword ptr [esp + 0x10], 0
    L0042cccf:
            fld dword ptr [ecx + 8]
            fld dword ptr [ecx]
            fld dword ptr [ecx + 4]
            fxch st(2)
            fsub dword ptr [edx + 8]
            fxch st(1)
            fsub dword ptr [edx]
            fxch st(2)
            fsub dword ptr [edx + 4]
            fld dword ptr [esp + 0x10]
            fld dword ptr [esp + 8]
            fxch st(3)
            fstp dword ptr [esp + 0x1c]
            fmul dword ptr [esp + 0x1c]
            fld dword ptr [esp + 0xc]
            fxch st(4)
            fstp dword ptr [esp + 0x14]
            fxch st(2)
            fmul dword ptr [esp + 0x14]
            fxch st(1)
            fstp dword ptr [esp + 0x18]
            fxch st(2)
            fmul dword ptr [esp + 0x18]
            fxch st(1)
            fstp dword ptr [esp + 0x24]
            fxch st(1)
            fadd dword ptr [esp + 0x24]
            fld dword ptr [esp + 0x24]
            fxch st(1)
            fadd st(0), st(2)
            fxch st(1)
            fadd st(0), st(2)
            fld dword ptr [esp + 0x14]
            fxch st(2)
            fmul dword ptr [esp + 8]
            mov eax, dword ptr [esp + 0x28]
            fld dword ptr [esp + 0x18]
            fld dword ptr [eax]
            fxch st(2)
            fst dword ptr [esp + 8]
            fmul dword ptr [esp + 0x14]
            fxch st(4)
            fsub dword ptr [esp + 8]
            fld dword ptr [eax + 4]
            fxch st(5)
            fstp dword ptr [esp + 0x28]
            fxch st(3)
            fadd dword ptr [esp + 0x28]
            fxch st(3)
            fstp dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x1c]
            fxch st(3)
            fmul dword ptr [esp + 0xc]
            fxch st(2)
            fsub dword ptr [edx]
            fxch st(4)
            fsub dword ptr [edx + 4]
            fxch st(2)
            fst dword ptr [esp + 0xc]
            fmul dword ptr [esp + 0x18]
            fxch st(1)
            fsub dword ptr [esp + 0xc]
            fxch st(1)
            fadd dword ptr [esp + 0x24]
            fxch st(4)
            fmul dword ptr [esp + 0x14]
            fxch st(1)
            fstp dword ptr [esp + 0x18]
            fxch st(3)
            fadd dword ptr [esp + 0x28]
            fxch st(1)
            fmul dword ptr [esp + 0x18]
            fxch st(1)
            fmul dword ptr [esp + 0x10]
            fxch st(1)
            faddp st(3), st(0)
            fld dword ptr [eax + 8]
            fxch st(1)
            fstp dword ptr [esp + 0x10]
            fxch st(1)
            fsub dword ptr [esp + 0x10]
            fxch st(1)
            fsub dword ptr [edx + 8]
            fxch st(1)
            fstp dword ptr [esp + 0x1c]
            fmul dword ptr [esp + 0x1c]
            faddp st(1), st(0)
            fcomp dword ptr [gCrushZero]
            fnstsw ax
            test ah, 1
            fstp st(0)
            je L0042cdea
            mov eax, 1
            pop esi
            add esp, 0x1c
            ret 8
    L0042cdea:
            xor eax, eax
            pop esi
            add esp, 0x1c
            ret 8
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0042ce00
float C2_NAKED C2_HOOK_FASTCALL PointRectDistSq(const br_vector3* pP, const br_vector3* pA, const br_vector3* pB, const br_vector3* pC) {
#ifndef CARPOCALYPSE2_MATCHING
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
#else
    __asm {
            sub esp, 0x24
            push esi
            mov esi, dword ptr [esp + 0x30]
            push edi
            mov edi, dword ptr [esp + 0x30]
            fld dword ptr [esi]
            fld dword ptr [edi]
            fxch st(1)
            fsub dword ptr [edx]
            fld dword ptr [esi + 4]
            fld dword ptr [edi + 4]
            fxch st(3)
            fsub dword ptr [edx]
            fxch st(1)
            fsub dword ptr [edx + 4]
            fxch st(3)
            fsub dword ptr [edx + 4]
            fld dword ptr [edi + 8]
            fld dword ptr [esi + 8]
            fsub dword ptr [edx + 8]
            fxch st(1)
            fsub dword ptr [edx + 8]
            fxch st(4)
            fst dword ptr [esp + 8]
            fxch st(5)
            fst dword ptr [esp + 0xc]
            fxch st(5)
            fmul st(0), st(3)
            fxch st(2)
            fstp dword ptr [esp + 0x18]
            fxch st(4)
            fmul dword ptr [esp + 0x18]
            fxch st(4)
            fst dword ptr [esp + 0x10]
            fxch st(3)
            fstp dword ptr [esp + 0x1c]
            fxch st(2)
            fmul dword ptr [esp + 0x1c]
            fxch st(3)
            faddp st(2), st(0)
            fxch st(2)
            faddp st(1), st(0)
            fabs
            fcomp dword ptr [gFLOAT_00589a7c]
            fnstsw ax
            test ah, 0x41
            jne L0042cf3f
            fstp st(0)
            fld dword ptr [esi]
            fld dword ptr [esi + 4]
            fld dword ptr [edx]
            fld dword ptr [edx + 4]
            fxch st(3)
            fsub dword ptr [edi]
            fxch st(2)
            fsub dword ptr [edi + 4]
            fxch st(1)
            fsub dword ptr [edi]
            fxch st(3)
            fsub dword ptr [edi + 4]
            fld dword ptr [edx + 8]
            fld dword ptr [esi + 8]
            fsub dword ptr [edi + 8]
            fxch st(1)
            fsub dword ptr [edi + 8]
            fxch st(4)
            fst dword ptr [esp + 8]
            fxch st(3)
            fst dword ptr [esp + 0xc]
            fxch st(3)
            fmul st(0), st(5)
            fxch st(2)
            fstp dword ptr [esp + 0x18]
            fxch st(2)
            fmul dword ptr [esp + 0x18]
            fxch st(2)
            fst dword ptr [esp + 0x10]
            fxch st(3)
            fstp dword ptr [esp + 0x1c]
            fxch st(2)
            fmul dword ptr [esp + 0x1c]
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fabs
            fcomp dword ptr [gFLOAT_00589a7c]
            fnstsw ax
            test ah, 0x41
            jne L0042cf3d
            fstp st(0)
            fld dword ptr [edx]
            fld dword ptr [edx + 4]
            fld dword ptr [edx + 8]
            fld dword ptr [edi]
            fld dword ptr [edi + 4]
            fld dword ptr [edi + 8]
            fxch st(5)
            fsub dword ptr [esi]
            fxch st(4)
            fsub dword ptr [esi + 4]
            fxch st(3)
            fsub dword ptr [esi + 8]
            fxch st(2)
            fsub dword ptr [esi]
            fxch st(1)
            fsub dword ptr [esi + 4]
            fxch st(5)
            fsub dword ptr [esi + 8]
            fxch st(3)
            fstp dword ptr [esp + 0x18]
            fxch st(1)
            fstp dword ptr [esp + 0x1c]
            mov edx, esi
            fstp dword ptr [esp + 8]
            fxch st(2)
            fstp dword ptr [esp + 0xc]
            fxch st(1)
            fstp dword ptr [esp + 0x10]
            jmp L0042cf3f
    L0042cf3d:
            mov edx, edi
    L0042cf3f:
            fld dword ptr [ecx]
            fsub dword ptr [edx]
            fld dword ptr [ecx + 4]
            fsub dword ptr [edx + 4]
            fld dword ptr [ecx + 8]
            fsub dword ptr [edx + 8]
            fld st(3)
            fld dword ptr [esp + 0x18]
            fxch st(4)
            fst dword ptr [esp + 0x20]
            fxch st(3)
            fst dword ptr [esp + 0x24]
            fld st(2)
            fxch st(4)
            fmul st(0), st(6)
            fld dword ptr [esp + 0x1c]
            fxch st(2)
            fmul dword ptr [esp + 0x18]
            faddp st(1), st(0)
            fxch st(2)
            fmul st(0), st(6)
            fld dword ptr [esp + 0x20]
            fxch st(5)
            fmul dword ptr [esp + 0x1c]
            faddp st(3), st(0)
            fxch st(5)
            fmul dword ptr [esp + 0x18]
            faddp st(5), st(0)
            fmul dword ptr [esp + 0x1c]
            fld dword ptr [esp + 0x24]
            fxch st(1)
            faddp st(5), st(0)
            fxch st(3)
            fmul dword ptr [esp + 8]
            fld st(2)
            fxch st(5)
            fdivp st(2), st(0)
            fxch st(1)
            fst dword ptr [esp + 0x34]
            fcomp dword ptr [gCrushZero]
            fxch st(2)
            fmul dword ptr [esp + 0xc]
            fld dword ptr [esp + 8]
            fmul dword ptr [esp + 8]
            fxch st(1)
            faddp st(3), st(0)
            fld dword ptr [esp + 0xc]
            fmul dword ptr [esp + 0xc]
            fld dword ptr [esp + 0x10]
            fxch st(5)
            fmul dword ptr [esp + 0x10]
            fnstsw ax
            faddp st(4), st(0)
            test ah, 1
            faddp st(1), st(0)
            fxch st(3)
            fmul dword ptr [esp + 0x10]
            faddp st(3), st(0)
            fxch st(2)
            fdivp st(1), st(0)
            fstp st(1)
            je L0042cff6
            mov dword ptr [esp + 0x34], 0
            jmp L0042d00f
    L0042cff6:
            fld dword ptr [esp + 0x34]
            fcomp dword ptr [gFLOAT_00589a54]
            fnstsw ax
            test ah, 0x41
            jne L0042d00f
            mov dword ptr [esp + 0x34], 0x3f800000
    L0042d00f:
            fcom dword ptr [gCrushZero]
            fnstsw ax
            test ah, 1
            je L0042d026
            fstp st(0)
            fld dword ptr [gCrushZero]
            jmp L0042d03b
    L0042d026:
            fcom dword ptr [gFLOAT_00589a54]
            fnstsw ax
            test ah, 0x41
            jne L0042d03b
            fstp st(0)
            fld dword ptr [gFLOAT_00589a54]
    L0042d03b:
            fld st(0)
            fld dword ptr [esp + 0x34]
            fld st(2)
            fld dword ptr [esp + 0x34]
            fld dword ptr [esp + 0x34]
            fxch st(5)
            fmul dword ptr [esp + 8]
            fxch st(5)
            fmul st(0), st(6)
            pop edi
            pop esi
            faddp st(5), st(0)
            fxch st(3)
            fmul dword ptr [esp + 4]
            fld dword ptr [ecx + 4]
            fxch st(5)
            fadd dword ptr [edx]
            fst dword ptr [esp + 0xc]
            fxch st(3)
            fmul dword ptr [esp + 0x10]
            faddp st(1), st(0)
            fxch st(1)
            fmul dword ptr [esp + 8]
            fxch st(1)
            fadd dword ptr [edx + 4]
            fxch st(3)
            fmul dword ptr [esp + 0x14]
            fld dword ptr [ecx + 8]
            fxch st(4)
            fstp dword ptr [esp + 0x10]
            fxch st(4)
            fsubr dword ptr [esp + 0x10]
            fxch st(4)
            faddp st(1), st(0)
            fadd dword ptr [edx + 8]
            fxch st(3)
            fst dword ptr [esp + 0x2c]
            fmul dword ptr [esp + 0x2c]
            fxch st(3)
            fstp dword ptr [esp + 0x14]
            fxch st(1)
            fsubr dword ptr [esp + 0x14]
            fxch st(1)
            fsub dword ptr [ecx]
            fxch st(1)
            fst dword ptr [esp + 0x2c]
            fmul dword ptr [esp + 0x2c]
            fxch st(1)
            fst dword ptr [esp + 0x2c]
            fmul dword ptr [esp + 0x2c]
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fstp st(1)
            add esp, 0x24
            ret 8
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
    }
#endif
}

float C2_HOOK_FASTCALL PointSimplexDistSq(const br_vector3* pP, int pCount, const br_vector3* pVertices) {
    float closest_distance = BR_SCALAR_MAX;
    int i;

    for (i = 1; i < pCount; i++) {
        int j;

        for (j = i + 1; j < pCount; j++) {
            float distance = PointTriangleDistSq(pP, &pVertices[0], &pVertices[i], &pVertices[j]);

            if (distance < closest_distance) {
                closest_distance = distance;
            }
        }
    }
    return closest_distance;
}

// LimitImpactScale

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
    if ((float)fabs(pVector->v[0]) > 0.707107f) {
        return 0;
    }
    if ((float)fabs(pVector->v[1]) > 0.707107f) {
        return 1;
    }
    return 2;
}

// CrushModel

// PossibleShockSmash

// CrushModels

// CrushPoint

// CrushCollisionShapes

// SimpleCrushCarModelsCB

// SimpleCrushCar

// CrushCar

// SendCrush

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

// FUNCTION: CARMA2_HW 0x0042d390
void C2_HOOK_FASTCALL AddDroneCrush(tQueued_drone_crush_item* pCrush, const br_vector3* pVector, float pScale, int pCount, const br_vector3* pPoints) {
    int i;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tQueued_drone_crush_item, points, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tQueued_drone_crush_item, points, 0x30);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tQueued_drone_crush_item, field_0x34, 0x34);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tQueued_drone_crush_item, field_0x38, 0x38);

    if (pCount > (int)CARPOCALYPSE2_ASIZE(pCrush->points)) {
        pCount = CARPOCALYPSE2_ASIZE(pCrush->points);
    }
    BrVector3Copy(&pCrush->field_0x38, pVector);
    pCrush->field_0x34 = pScale;
    pCrush->count = pCount;
    for (i = 0; i < pCrush->count; i++) {
        BrVector3Copy(&pCrush->points[i], &pPoints[i]);
    }
}

// DoCrushing

// FUNCTION: CARMA2_HW 0x0042d770
void C2_HOOK_FASTCALL SetCrush(tCar_crush* pCrush, int pCount_vectors, br_vector3* pVectors, br_vector3* pVector1, br_vector3* pVector2, float pScalar, tU32 pValue) {
    int i;

    pCrush->count = pCount_vectors;
    for (i = 0; i < pCount_vectors; i++) {
        BrVector3Copy(&pCrush->vectors[i], &pVectors[i]);
    }
    BrVector3Copy(&pCrush->field_0x34, pVector1);
    BrVector3Copy(&pCrush->field_0x44, pVector2);
    pCrush->field_0x40 = pScalar;
    pCrush->field_0x50 = pValue;
}

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
    tS32 count;
    br_scalar step;
    tS32 index;

    /* Choose a whole number of cells spanning the bounds. */
    rounded = range / pQuant;
    if (rounded > gCrushZero) {
        rounded += 0.5f;
    } else {
        rounded -= gFLOAT_00589a2c;
    }
    count = (tS32)rounded;
    if (count == 0) {
        count = 1;
    }
    step = range / (br_scalar)count;

    /* Snap to the grid only when the selected cell is within the bounds. */
    rounded = (*pValue - pMin) / step;
    if (rounded > gCrushZero) {
        rounded += 0.5f;
    } else {
        rounded -= gFLOAT_00589a2c;
    }
    index = (tS32)rounded;
    if (index < 0 || index > count) {
        return 0;
    }
    *pValue = (br_scalar)index * step + pMin;
    return 1;
}

// SaveVitalBitsOfObject

// FUNCTION: CARMA2_HW 0x00430730
intptr_t C2_HOOK_CDECL MakeCarModelsMaterialsDoubleSided(br_actor* pActor, void* pArg) {
    tUser_crush_data* user_crush = (tUser_crush_data*)pActor->user;
    tCar_spec* car_spec = (tCar_spec*)pArg;
    int i;

    if (user_crush != NULL) {
        for (i = 0; i < car_spec->count_detail_levels; i++) {
            br_model* model = user_crush->models[i];
            if (model != NULL) {
                int g;
                for (g = 0; g < model->prepared->ngroups; g++) {
                    br_material* material;

                    material = model->faces[*model->prepared->groups[g].face_user].material;
                    if (material != NULL && !(material->flags & BR_MATF_TWO_SIDED)) {
                        material->flags |= BR_MATF_TWO_SIDED;
                        BrMaterialUpdate(material, BR_MATU_RENDERING);
                    }
                }
            }
        }
    }
    return 0;
}

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
int C2_NAKED C2_HOOK_FASTCALL SplitCheck(tCar_spec* pCar, int pMode, const br_vector3* pLine, br_scalar pHeight, br_vector4* pPlane) {
#ifndef CARPOCALYPSE2_MATCHING
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
#else
    __asm {
            sub esp, 0x44
            mov eax, dword ptr [ecx + 0x1b4]
            push ebx
            xor ebx, ebx
            push ebp
            push esi
            cmp eax, ebx
            push edi
            mov dword ptr [esp + 0x1c], ecx
            je L00430803
            cmp dword ptr [ecx + 0x1d4], ebx
            je L00430d74
    L00430803:
            fld dword ptr [ecx + 0x4c8]
            fcomp qword ptr [gDOUBLE_00589a60]
            fnstsw ax
            test ah, 0x40
            je L00430d74
            mov eax, dword ptr [ecx + 0x18d4]
            cmp eax, ebx
            mov dword ptr [esp + 0x20], eax
            je L00430d74
            cmp byte ptr [eax + 0x144], bl
            jne L00430d74
            cmp byte ptr [eax + 0x4b8], bl
            jne L00430d74
            mov edi, dword ptr [ecx + 8]
            mov eax, edx
            add edi, 0x24
            sub eax, ebx
            fld dword ptr [edi + 0xc]
            fsub dword ptr [edi]
            fld dword ptr [edi + 0x14]
            fsub dword ptr [edi + 8]
            fld dword ptr [edi + 0x10]
            fsub dword ptr [edi + 4]
            fxch st(2)
            fst dword ptr [esp + 0x30]
            fmul dword ptr [gFLOAT_00589a28]
            fxch st(1)
            fmul dword ptr [gFLOAT_00589a28]
            fld dword ptr [edi]
            fld dword ptr [edi + 0xc]
            fld dword ptr [edi + 0x14]
            fxch st(5)
            fst dword ptr [esp + 0x34]
            fxch st(4)
            fstp dword ptr [esp + 0x24]
            fxch st(1)
            fadd dword ptr [esp + 0x24]
            fxch st(1)
            fsub dword ptr [esp + 0x24]
            fxch st(3)
            fmul dword ptr [gFLOAT_00589a28]
            fxch st(2)
            fst dword ptr [esp + 0x2c]
            fadd dword ptr [edi + 8]
            fxch st(4)
            fsub dword ptr [esp + 0x2c]
            fxch st(2)
            fstp dword ptr [esp + 0x28]
            fstp dword ptr [esp + 0x3c]
            fxch st(2)
            fstp dword ptr [esp + 0x44]
            fstp dword ptr [esp + 0x48]
            fstp dword ptr [esp + 0x50]
            je L0043090b
            dec eax
            jne L00430d74
            mov esi, dword ptr [esp + 0x60]
            mov eax, dword ptr [esp + 0x5c]
            lea ebp, [esi + 0xc]
            mov dword ptr [esi], ebx
            mov dword ptr [esi + 4], 0x3f800000
            mov dword ptr [esi + 8], ebx
            mov dword ptr [ebp], eax
            mov ecx, dword ptr [edi + 0x10]
            mov edx, dword ptr [edi + 4]
            push ecx
            push edx
            push 0x3ca3d70a
            mov ecx, ebp
            call QuantizeWithinBounds
            test eax, eax
            jne L00430bb1
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x44
            ret 0xc
    L0043090b:
            mov esi, dword ptr [esp + 0x58]
            fld dword ptr [esi]
            fabs
            fcomp dword ptr [gFLOAT_00589a7c]
            fnstsw ax
            test ah, 0x41
            jne L00430a60
            fld dword ptr [edi + 4]
            fld dword ptr [esi + 4]
            fld dword ptr [esi + 8]
            fxch st(1)
            fmulp st(2), st(0)
            fld dword ptr [esi + 4]
            fxch st(2)
            fsubp st(1), st(0)
            fld dword ptr [esi + 8]
            fxch st(1)
            fdiv dword ptr [esi]
            lea ecx, [esp + 0x5c]
            fstp dword ptr [esp + 0x5c]
            fld dword ptr [edi + 0x10]
            fxch st(2)
            fmulp st(2), st(0)
            fxch st(1)
            fsubp st(1), st(0)
            fdiv dword ptr [esi]
            fstp dword ptr [esp + 0x10]
            mov eax, dword ptr [edi + 0xc]
            mov edx, dword ptr [edi]
            push eax
            push edx
            push 0x3ca3d70a
            call QuantizeWithinBounds
            test eax, eax
            je L00430a60
            mov eax, dword ptr [edi + 0xc]
            mov ecx, dword ptr [edi]
            push eax
            push ecx
            push 0x3ca3d70a
            lea ecx, [esp + 0x1c]
            call QuantizeWithinBounds
            test eax, eax
            je L00430a60
            fld dword ptr [esp + 0x10]
            fadd dword ptr [esp + 0x5c]
            fmul dword ptr [gFLOAT_00589a2c]
            fcom dword ptr [esp + 0x3c]
            fnstsw ax
            test ah, 1
            jne L00430a5e
            fcomp dword ptr [esp + 0x48]
            fnstsw ax
            test ah, 0x41
            je L00430a60
            fld dword ptr [esp + 0x10]
            fsub dword ptr [esp + 0x5c]
            fld dword ptr [esp + 0x34]
            fchs
            fld st(1)
            fld st(1)
            fxch st(1)
            fst dword ptr [esp + 0x18]
            fmul dword ptr [esp + 0x18]
            fld st(2)
            fxch st(2)
            fmul st(0), st(3)
            mov esi, dword ptr [esp + 0x60]
            fld st(4)
            fxch st(1)
            faddp st(2), st(0)
            fxch st(2)
            fstp dword ptr [esi]
            fxch st(1)
            fstp dword ptr [esi + 8]
            mov dword ptr [esi + 4], ebx
            fsqrt
            fcom dword ptr [gFLOAT_00589a5c]
            fnstsw ax
            test ah, 0x41
            jne L00430a1f
            fdivr qword ptr [gDOUBLE_00589a60]
            mov dword ptr [esi + 4], ebx
            fld st(0)
            fmul st(0), st(2)
            fxch st(1)
            fmul st(0), st(3)
            fxch st(1)
            fstp dword ptr [esi]
            fstp dword ptr [esi + 8]
            fstp st(0)
            fstp st(0)
            jmp L00430a31
    L00430a1f:
            fstp st(0)
            fstp st(0)
            fstp st(0)
            mov dword ptr [esi], 0x3f800000
            mov dword ptr [esi + 4], ebx
            mov dword ptr [esi + 8], ebx
    L00430a31:
            fld dword ptr [edi + 4]
            fld dword ptr [esi + 8]
            fld dword ptr [esi]
            fmul dword ptr [esp + 0x5c]
            fxch st(2)
            fstp dword ptr [esp + 0x2c]
            mov edx, dword ptr [esp + 0x5c]
            mov dword ptr [esp + 0x28], 0
            fmul dword ptr [esp + 0x2c]
            fxch st(1)
            mov dword ptr [esp + 0x24], edx
            jmp L00430ba9
    L00430a5e:
            fstp st(0)
    L00430a60:
            fld dword ptr [esi + 4]
            fabs
            fcomp dword ptr [gFLOAT_00589a7c]
            fnstsw ax
            test ah, 0x41
            jne L00430d74
            fld dword ptr [edi]
            fmul dword ptr [esi]
            fld dword ptr [esi + 8]
            fxch st(1)
            fsubp st(1), st(0)
            fld dword ptr [esi + 8]
            fxch st(1)
            fdiv dword ptr [esi + 4]
            lea ecx, [esp + 0x58]
            fstp dword ptr [esp + 0x58]
            fld dword ptr [edi + 0xc]
            fmul dword ptr [esi]
            fsubp st(1), st(0)
            fdiv dword ptr [esi + 4]
            fstp dword ptr [esp + 0x14]
            mov eax, dword ptr [edi + 0x14]
            mov edx, dword ptr [edi + 8]
            push eax
            push edx
            push 0x3ca3d70a
            call QuantizeWithinBounds
            test eax, eax
            je L00430d74
            mov eax, dword ptr [edi + 0x14]
            mov ecx, dword ptr [edi + 8]
            push eax
            push ecx
            push 0x3ca3d70a
            lea ecx, [esp + 0x20]
            call QuantizeWithinBounds
            test eax, eax
            je L00430d74
            fld dword ptr [esp + 0x14]
            fadd dword ptr [esp + 0x58]
            fmul dword ptr [gFLOAT_00589a2c]
            fcom dword ptr [esp + 0x44]
            fnstsw ax
            test ah, 1
            jne L00430d72
            fcomp dword ptr [esp + 0x50]
            fnstsw ax
            test ah, 0x41
            je L00430d74
            fld dword ptr [esp + 0x30]
            fld dword ptr [esp + 0x14]
            fsub dword ptr [esp + 0x58]
            fxch st(1)
            fchs
            fld st(0)
            fld st(2)
            fxch st(1)
            fst dword ptr [esp + 0x18]
            fmul dword ptr [esp + 0x18]
            fld st(3)
            fxch st(2)
            fmul st(0), st(4)
            mov esi, dword ptr [esp + 0x60]
            fld st(3)
            fxch st(1)
            faddp st(2), st(0)
            fxch st(2)
            fstp dword ptr [esi]
            fxch st(1)
            fstp dword ptr [esi + 8]
            mov dword ptr [esi + 4], ebx
            fsqrt
            fcom dword ptr [gFLOAT_00589a5c]
            fxch st(1)
            fxch st(2)
            fxch st(1)
            fnstsw ax
            test ah, 0x41
            jne L00430b70
            fdivr qword ptr [gDOUBLE_00589a60]
            mov dword ptr [esi + 4], ebx
            fld st(0)
            fmul st(0), st(2)
            fxch st(1)
            fmul st(0), st(3)
            fxch st(1)
            fstp dword ptr [esi]
            fstp dword ptr [esi + 8]
            fstp st(0)
            fstp st(0)
            jmp L00430b82
    L00430b70:
            fstp st(0)
            fstp st(0)
            fstp st(0)
            mov dword ptr [esi], 0x3f800000
            mov dword ptr [esi + 4], ebx
            mov dword ptr [esi + 8], ebx
    L00430b82:
            fld dword ptr [esi + 8]
            mov edx, dword ptr [edi]
            mov eax, dword ptr [esp + 0x58]
            fld dword ptr [esi]
            mov dword ptr [esp + 0x24], edx
            mov dword ptr [esp + 0x28], 0
            fmul dword ptr [esp + 0x24]
            fxch st(1)
            fmul dword ptr [esp + 0x58]
            fxch st(1)
            mov dword ptr [esp + 0x2c], eax
    L00430ba9:
            faddp st(1), st(0)
            lea ebp, [esi + 0xc]
            fstp dword ptr [ebp]
    L00430bb1:
            fld dword ptr [esi]
            mov ecx, dword ptr [esp + 0x1c]
            fabs
            mov edx, dword ptr [ecx + 0xe10]
            fcomp dword ptr [gCrushSqrtHalf]
            mov ecx, dword ptr [edx + 0x18]
            add ecx, 0x34
            fnstsw ax
            test ah, 0x41
            jne L00430bd6
            xor edx, edx
            jmp L00430bf2
    L00430bd6:
            fld dword ptr [esi + 4]
            fabs
            fcomp dword ptr [gCrushSqrtHalf]
            mov edx, 1
            fnstsw ax
            test ah, 0x41
            je L00430bf2
            mov edx, 2
    L00430bf2:
            lea eax, [edx + 1]
            mov dword ptr [esp + 0x60], edx
            lea ebx, [edx + 2]
            mov edi, 3
            cdq
            fld dword ptr [esi + 4]
            idiv edi
            fld dword ptr [esi + 8]
            fld dword ptr [esi]
            fld dword ptr [ebp]
            mov eax, ebx
            mov ebx, edi
            mov edi, dword ptr [esp + 0x60]
            shl edi, 2
            fld dword ptr [esi + edi]
            fld dword ptr [ecx + edi]
            mov dword ptr [esp + edi + 0x24], 0
            mov edi, edx
            cdq
            fld dword ptr [ecx + edi*4]
            idiv ebx
            lea eax, [edi*4]
            fadd dword ptr [ecx + edi*4 + 0xc]
            fmul dword ptr [gFLOAT_00589a2c]
            fstp dword ptr [esp + eax + 0x24]
            fld dword ptr [ecx + edx*4]
            fadd dword ptr [ecx + edx*4 + 0xc]
            lea ebx, [edx*4]
            fmul dword ptr [gFLOAT_00589a2c]
            fstp dword ptr [esp + ebx + 0x24]
            fxch st(5)
            fmul dword ptr [esp + 0x28]
            fxch st(4)
            fmul dword ptr [esp + 0x2c]
            faddp st(4), st(0)
            fxch st(2)
            fmul dword ptr [esp + 0x24]
            fxch st(4)
            fsub dword ptr [gFLOAT_00589aa8]
            fxch st(4)
            faddp st(3), st(0)
            fsubp st(2), st(0)
            fdivp st(1), st(0)
            fchs
            fst dword ptr [esp + 0x18]
            fcompp
            fnstsw ax
            test ah, 1
            jne L00430d74
            mov eax, dword ptr [esp + 0x60]
            fld dword ptr [ecx + eax*4 + 0xc]
            fsub dword ptr [gFLOAT_00589aac]
            fld dword ptr [esp + 0x18]
            fcompp
            fnstsw ax
            test ah, 0x41
            je L00430d74
            mov eax, dword ptr [esp + 0x20]
            fld dword ptr [eax + 0x55c]
            fld dword ptr [esi + 4]
            fld dword ptr [eax + 0x560]
            fld dword ptr [eax + 0x558]
            fxch st(2)
            fmulp st(3), st(0)
            fmul dword ptr [esi + 8]
            fxch st(1)
            fmul dword ptr [esi]
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fsub dword ptr [ebp]
            fcomp dword ptr [gCrushZero]
            fnstsw ax
            test ah, 1
            je L00430d0b
            fld dword ptr [esi]
            fchs
            fstp dword ptr [esi]
            fld dword ptr [esi + 4]
            fchs
            fstp dword ptr [esi + 4]
            fld dword ptr [esi + 8]
            fchs
            fstp dword ptr [esi + 8]
            fld dword ptr [ebp]
            fchs
            fstp dword ptr [ebp]
    L00430d0b:
            push 0x3f800000
            push 0xbf800000
            mov edx, esi
            lea ecx, [esp + 0x2c]
            call CompressVector3
            mov ecx, dword ptr [ebp]
            push 0x40200000
            push 0xc0200000
            push ecx
            call DRScalarToU16
            lea edx, [esp + 0x24]
            mov ecx, esi
            push 0x3f800000
            push 0xbf800000
            mov word ptr [esp + 0x32], ax
            call ExpandVector3
            mov ecx, dword ptr [esp + 0x2a]
            push 0x40200000
            push 0xc0200000
            call DRU16ToScalar
            fstp dword ptr [ebp]
            mov eax, 1
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x44
            ret 0xc
    L00430d72:
            fstp st(0)
    L00430d74:
            pop edi
            pop esi
            pop ebp
            xor eax, eax
            pop ebx
            add esp, 0x44
            ret 0xc
    }
#endif
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
void C2_NAKED C2_HOOK_FASTCALL TotallyRepairACar(tCar_spec* pCar_spec) {
#ifndef CARPOCALYPSE2_MATCHING
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
#else
    __asm {
            sub esp, 0x10
            push ebx
            push ebp
            push esi
            push edi
            mov esi, ecx
            call StopCarSmokingInstantly
            call ARIsActionReplayAvailable
            test eax, eax
            je L0043952e
            mov ecx, esi
            call PipeInstantUnSmudge
    L0043952e:
            mov eax, dword ptr [esi + 0x18a4]
            mov ecx, 0xc
            add eax, 0x186a0
            mov dword ptr [esi + 0x18a4], eax
            lea eax, [esi + 0x4e4]
    L0043954a:
            xor edx, edx
            mov dword ptr [eax - 4], edx
            mov dword ptr [eax], edx
            mov dword ptr [eax + 4], edx
            add eax, 0x2c
            dec ecx
            jne L0043954a
            mov eax, dword ptr [esi + 0xe24]
            test eax, eax
            jne L004398aa
            mov ebp, dword ptr [esi + 0x18d4]
            test ebp, ebp
            je L004398aa
            mov al, byte ptr [ebp + 0x4b8]
            test al, al
            je L00439587
            mov ecx, esi
            call WeldCar
    L00439587:
            mov eax, dword ptr [ebp + 0x2b0]
            xor ebx, ebx
            test eax, eax
            jle L004395b6
            lea edi, [ebp + 0x2b8]
    L00439599:
            mov eax, dword ptr [edi - 4]
            mov edx, dword ptr [edi]
            push 0
            push eax
            mov ecx, esi
            call ReAttachBit
            mov eax, dword ptr [ebp + 0x2b0]
            inc ebx
            add edi, 0x14
            cmp ebx, eax
            jl L00439599
    L004395b6:
            mov eax, dword ptr [ebp + 0x270]
            xor ebx, ebx
            cmp eax, ebx
            mov dword ptr [ebp + 0x2b0], ebx
            jle L004395ed
            lea edi, [ebp + 0x274]
    L004395ce:
            mov ecx, dword ptr [edi + 8]
            mov edx, dword ptr [edi]
            push ecx
            push edx
            mov edx, dword ptr [edi + 4]
            mov ecx, esi
            call ReAttachBit
            mov eax, dword ptr [ebp + 0x270]
            inc ebx
            add edi, 0x14
            cmp ebx, eax
            jl L004395ce
    L004395ed:
            xor ebx, ebx
            push esi
            mov dword ptr [ebp + 0x270], ebx
            mov ecx, dword ptr [esi + 0xe0c]
            mov edx, offset MakeCarModelsMaterialsSingleSided
            call DRActorEnumRecurse
            mov edx, esi
            xor ecx, ecx
            call PipeSingleDSModel
            mov al, byte ptr [ebp + 0x144]
            test al, al
            je L00439808
            mov eax, dword ptr [esi + 0x18d4]
            mov dword ptr [esp + 0x10], ebx
            mov byte ptr [eax + 0x144], bl
            mov eax, dword ptr [esi + 0x18d4]
            cmp dword ptr [eax], ebx
            jle L0043968e
    L00439637:
            mov eax, dword ptr [eax + 4]
            xor edi, edi
            add eax, ebx
            mov ecx, dword ptr [eax + 0x14]
            test ecx, ecx
            mov dword ptr [eax + 0xc], ecx
            jle L00439676
            xor ecx, ecx
    L0043964a:
            mov edx, dword ptr [eax + 0x18]
            add edx, ecx
            mov ebp, dword ptr [edx + 0x2c]
            mov dword ptr [edx + 0x18], ebp
            mov edx, dword ptr [eax + 0x18]
            add edx, ecx
            mov ebp, dword ptr [edx + 0x30]
            mov dword ptr [edx + 0x1c], ebp
            mov edx, dword ptr [eax + 0x18]
            add edx, ecx
            inc edi
            add ecx, 0x38
            mov ebp, dword ptr [edx + 0x34]
            mov dword ptr [edx + 0x20], ebp
            mov edx, dword ptr [eax + 0xc]
            cmp edi, edx
            jl L0043964a
    L00439676:
            mov eax, dword ptr [esi + 0x18d4]
            mov ecx, dword ptr [esp + 0x10]
            inc ecx
            add ebx, 0x1c
            mov edx, dword ptr [eax]
            mov dword ptr [esp + 0x10], ecx
            cmp ecx, edx
            jl L00439637
    L0043968e:
            mov eax, dword ptr [esi + 0x18d4]
            xor edi, edi
            lea ebx, [esi + 0x12ec]
            mov dword ptr [eax + 8], 3
    L004396a3:
            mov ecx, dword ptr [ebx + 0x18]
            test ecx, ecx
            jne L004396b4
            mov ecx, dword ptr [ebx]
            test ecx, ecx
            je L0043973c
    L004396b4:
            call ActorsGroove
            test eax, eax
            je L0043973c
            mov ecx, dword ptr [eax + 0x18]
            test ecx, ecx
            jne L0043973c
            fld dword ptr [eax + 0x38]
            lea ecx, [eax + 0x38]
            mov eax, dword ptr [esi + 0x18d4]
            fchs
            fstp dword ptr [esp + 0x14]
            fld dword ptr [ecx + 4]
            fchs
            fstp dword ptr [esp + 0x18]
            fld dword ptr [ecx + 8]
            fchs
            fstp dword ptr [esp + 0x1c]
            mov eax, dword ptr [edi + eax + 0x1ec]
            lea edx, [esp + 0x14]
            mov dword ptr [ecx], eax
            mov eax, dword ptr [esi + 0x18d4]
            _emit 0xd9
        _emit 0x01
            mov eax, dword ptr [edi + eax + 0x1f0]
            mov dword ptr [ecx + 4], eax
            mov eax, dword ptr [esi + 0x18d4]
            mov eax, dword ptr [edi + eax + 0x1f4]
            mov dword ptr [ecx + 8], eax
            fadd dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x18]
            fld dword ptr [esp + 0x1c]
            fxch st(2)
            fstp dword ptr [esp + 0x14]
            fadd dword ptr [ecx + 4]
            fstp dword ptr [esp + 0x18]
            fadd dword ptr [ecx + 8]
            fstp dword ptr [esp + 0x1c]
            call PipeSingleVector3
    L0043973c:
            add edi, 0xc
            add ebx, 4
            cmp edi, 0x48
            jl L004396a3
            mov edx, 0xfffff00c
            mov edi, 0xffffedd8
            sub edx, esi
            lea eax, [esi + 0x1228]
            sub edi, esi
            mov ebx, 4
    L00439764:
            mov ebp, dword ptr [esi + 0x18d4]
            lea ecx, [edx + eax]
            mov ecx, dword ptr [ecx + ebp]
            mov dword ptr [eax - 4], ecx
            mov ebp, dword ptr [esi + 0x18d4]
            lea ecx, [edi + eax]
            add eax, 0xc
            dec ebx
            mov ebp, dword ptr [ecx + ebp + 0x238]
            mov dword ptr [eax - 0xc], ebp
            mov ebp, dword ptr [esi + 0x18d4]
            mov ecx, dword ptr [ecx + ebp + 0x23c]
            mov dword ptr [eax - 8], ecx
            jne L00439764
            push 0
            push 0
            push 0x3f800000
            push 0x3f800000
            push 0x3f800000
            mov ecx, esi
            call SetCarSuspGiveAndHeight
            mov edx, dword ptr [esi + 0x18d4]
            mov eax, dword ptr [esi + 8]
            mov dword ptr [esi + 0x1260], 0
            push 0
            mov ecx, dword ptr [edx + 0x264]
            mov dword ptr [eax + 0x14], ecx
            mov edx, dword ptr [esi + 0x18d4]
            mov eax, dword ptr [esi + 8]
            mov ecx, dword ptr [edx + 0x268]
            mov dword ptr [eax + 0x18], ecx
            mov edx, dword ptr [esi + 0x18d4]
            mov eax, dword ptr [esi + 8]
            mov ecx, dword ptr [edx + 0x26c]
            mov edx, offset EnableGroovers
            mov dword ptr [eax + 0x1c], ecx
            mov ecx, dword ptr [esi + 0xe0c]
            call DRActorEnumRecurse
    L00439808:
            mov ecx, 0x18
            call ARStartPipingSession
            mov ecx, dword ptr [esi + 8]
            mov edx, offset TotallyRepairObject
            push esi
            call PhysicsObjectRecurse
            call AREndPipingSession
            push offset gNext_crush_actor_id+0x440
            call dr_dprintf
            add esp, 4
            mov ecx, esi
            call TotallyRepairCarCollisionShapes
            push offset gNext_crush_actor_id+0x420
            call dr_dprintf
            mov ecx, dword ptr [esi + 8]
            add esp, 4
            mov edx, offset BattenDownTheObjects
            push esi
            call PhysicsObjectRecurse
            push offset gNext_crush_actor_id+0x400
            call dr_dprintf
            mov eax, dword ptr [gProgram_state+0xa4]
            add esp, 4
            cmp eax, 2
            je L0043987f
            push offset gNext_crush_actor_id+0x3e0
            call dr_dprintf
            add esp, 4
            mov ecx, esi
            call SwapShapesIfPossible
    L0043987f:
            push offset gNext_crush_actor_id+0x3c0
            call dr_dprintf
            add esp, 4
            mov ecx, esi
            mov dword ptr [esi + 0xe24], 1
            call RemoveCarFromCrushLists
            push offset gNext_crush_actor_id+0x3a0
            call dr_dprintf
            add esp, 4
    L004398aa:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x10
            ret
    }
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

// FUNCTION: CARMA2_HW 0x0043ac90
br_scalar C2_HOOK_FASTCALL NormalizedAngle(br_angle pAngle) {
    if (pAngle < BR_ANGLE_DEG(180)) {
        return (br_scalar)(pAngle * gDouble_00589b40);
    }
    pAngle = (br_angle)-pAngle;
    return (br_scalar)(pAngle * gDouble_00589b40);
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

// FUNCTION: CARMA2_HW 0x0043bf80
void C2_HOOK_FASTCALL ARApplyModelMash(br_model* pModel, tCompressed_vector3* pSrc) {
    br_uint_16 i;
    br_vector3 delta;

    for (i = 0; i < pModel->nvertices; i++) {
        ExpandVector3(&delta, &pSrc[i], -10.f, 10.f);
        pModel->vertices[i].p.v[0] += delta.v[0];
        pModel->vertices[i].p.v[1] += delta.v[1];
        pModel->vertices[i].p.v[2] += delta.v[2];
    }
    if ((pModel->flags & BR_MODF_CUSTOM) != 0 && pModel->user != NULL) {
        *(int*)((tUser_detail_level_model*)pModel->user)->field_0x0 = 1;
    } else {
        pModel->custom = JitModelUpdate;
        pModel->flags |= BR_MODF_CUSTOM;
    }
}

// FUNCTION: CARMA2_HW 0x0043c030
void C2_HOOK_FASTCALL ARUndoModelMash(br_model* pModel, tCompressed_vector3* pSrc) {
    br_uint_16 i;
    br_vector3 delta;

    for (i = 0; i < pModel->nvertices; i++) {
        ExpandVector3(&delta, &pSrc[i], -10.f, 10.f);
        pModel->vertices[i].p.v[0] -= delta.v[0];
        pModel->vertices[i].p.v[1] -= delta.v[1];
        pModel->vertices[i].p.v[2] -= delta.v[2];
    }
    if ((pModel->flags & BR_MODF_CUSTOM) != 0 && pModel->user != NULL) {
        *(int*)((tUser_detail_level_model*)pModel->user)->field_0x0 = 1;
    } else {
        pModel->custom = JitModelUpdate;
        pModel->flags |= BR_MODF_CUSTOM;
    }
}

// FUNCTION: CARMA2_HW 0x0043c0e0
void C2_NAKED C2_HOOK_FASTCALL ARApplyFlap(br_actor* pActor, tS16 pAngle) {
#ifndef CARPOCALYPSE2_MATCHING
    tUser_crush_data* user_crush_data;
    tCar_crush_flap_data* flap_data;
    br_vertex* vertex;
    br_vector3 axis;
    br_scalar length;

    user_crush_data = pActor->user;
    if (user_crush_data == NULL) {
        return;
    }
    if (user_crush_data->crush_data == NULL) {
        return;
    }
    flap_data = user_crush_data->crush_data->flap_data;
    if (flap_data == NULL) {
        return;
    }
    flap_data->field_0x4 += pAngle;
    vertex = &user_crush_data->models[0]->vertices[flap_data->hinge0];
    axis.v[0] = user_crush_data->models[0]->vertices[flap_data->hinge1].p.v[0] - vertex->p.v[0];
    axis.v[1] = user_crush_data->models[0]->vertices[flap_data->hinge1].p.v[1] - vertex->p.v[1];
    axis.v[2] = user_crush_data->models[0]->vertices[flap_data->hinge1].p.v[2] - vertex->p.v[2];
    length = sqrtf(axis.v[0] * axis.v[0] + axis.v[1] * axis.v[1] + axis.v[2] * axis.v[2]);
    if (length > gFLOAT_00589a5c) {
        br_scalar scale = (br_scalar)(gDOUBLE_00589a60 / length);
        axis.v[0] *= scale;
        axis.v[1] *= scale;
        axis.v[2] *= scale;
    } else {
        axis.v[0] = 1.0f;
        axis.v[1] = 0.0f;
        axis.v[2] = 0.0f;
    }
    BrMatrix34Translate(&pActor->t.t.mat, -vertex->p.v[0], -vertex->p.v[1], -vertex->p.v[2]);
    BrMatrix34PostRotate(&pActor->t.t.mat, flap_data->field_0x4, &axis);
    vertex = &((tUser_crush_data*)pActor->parent->user)->models[0]->vertices[flap_data->field_0xc];
    BrMatrix34PostTranslate(&pActor->t.t.mat, vertex->p.v[0], vertex->p.v[1], vertex->p.v[2]);
#else
    __asm {
            sub esp, 0xc
            push ebx
            mov ebx, ecx
            push esi
            push edi
            mov edi, dword ptr [ebx + 0x60]
            test edi, edi
            je L0043c234
            mov eax, dword ptr [edi + 8]
            test eax, eax
            je L0043c234
            mov esi, dword ptr [eax + 0x30]
            test esi, esi
            je L0043c234
            add word ptr [esi + 4], dx
            mov eax, dword ptr [edi]
            mov edx, dword ptr [eax + 8]
            xor eax, eax
            mov ax, word ptr [esi + 0xa]
            lea ecx, [eax + eax*4]
            xor eax, eax
            mov ax, word ptr [esi + 0xe]
            lea ecx, [edx + ecx*8]
            lea eax, [eax + eax*4]
            fld dword ptr [edx + eax*8]
            fsub dword ptr [ecx]
            xor eax, eax
            fstp dword ptr [esp + 0xc]
            mov edx, dword ptr [edi]
            mov ax, word ptr [esi + 0xe]
            mov edx, dword ptr [edx + 8]
            lea eax, [eax + eax*4]
            fld dword ptr [edx + eax*8 + 4]
            fsub dword ptr [ecx + 4]
            xor eax, eax
            fstp dword ptr [esp + 0x10]
            mov edx, dword ptr [edi]
            mov ax, word ptr [esi + 0xe]
            mov edx, dword ptr [edx + 8]
            lea eax, [eax + eax*4]
            fld dword ptr [edx + eax*8 + 8]
            fsub dword ptr [ecx + 8]
            fld dword ptr [esp + 0xc]
            fmul dword ptr [esp + 0xc]
            fld dword ptr [esp + 0x10]
            fmul dword ptr [esp + 0x10]
            faddp st(1), st(0)
            fld st(1)
            fmul st(0), st(2)
            faddp st(1), st(0)
            fsqrt
            fcom dword ptr [gFLOAT_00589a5c]
            fnstsw ax
            test ah, 0x41
            jne L0043c1b3
            fdivr qword ptr [gDOUBLE_00589a60]
            fld st(0)
            fld st(1)
            fxch st(1)
            fmul dword ptr [esp + 0xc]
            fxch st(1)
            fmul dword ptr [esp + 0x10]
            fxch st(2)
            fmul st(0), st(3)
            fxch st(1)
            fstp dword ptr [esp + 0xc]
            fxch st(1)
            fstp dword ptr [esp + 0x10]
            fstp dword ptr [esp + 0x14]
            fstp st(0)
            jmp L0043c1cf
    L0043c1b3:
            fstp st(0)
            fstp st(0)
            mov dword ptr [esp + 0xc], 0x3f800000
            mov dword ptr [esp + 0x10], 0
            mov dword ptr [esp + 0x14], 0
    L0043c1cf:
            fld dword ptr [ecx + 8]
            fchs
            push ecx
            lea edi, [ebx + 0x2c]
            fstp dword ptr [esp]
            fld dword ptr [ecx + 4]
            fchs
            push ecx
            fstp dword ptr [esp]
            fld dword ptr [ecx]
            fchs
            push ecx
            fstp dword ptr [esp]
            push edi
            call BrMatrix34Translate
            mov cx, word ptr [esi + 4]
            add esp, 0x10
            lea eax, [esp + 0xc]
            push eax
            push ecx
            push edi
            call BrMatrix34PostRotate
            mov edx, dword ptr [ebx + 0xc]
            add esp, 0xc
            mov eax, dword ptr [edx + 0x60]
            mov ecx, dword ptr [eax]
            xor eax, eax
            mov ax, word ptr [esi + 0xc]
            lea edx, [eax + eax*4]
            mov eax, dword ptr [ecx + 8]
            mov ecx, dword ptr [eax + edx*8 + 8]
            lea eax, [eax + edx*8]
            push ecx
            mov edx, dword ptr [eax + 4]
            mov eax, dword ptr [eax]
            push edx
            push eax
            push edi
            call BrMatrix34PostTranslate
            add esp, 0x10
    L0043c234:
            pop edi
            pop esi
            pop ebx
            add esp, 0xc
            ret
            nop
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0043c3d0
void C2_NAKED C2_HOOK_FASTCALL ARUndoSplitWeld(tCar_spec* pCar_spec) {
#ifndef CARPOCALYPSE2_MATCHING
    tCar_crush_spec* car_crush_spec;
    br_model** car_models;
    int i;
    br_model** crush_models;

    i = 0;
    car_crush_spec = pCar_spec->car_crush_spec;
    car_models = (br_model**)pCar_spec->car_model_actor->user;
    if (pCar_spec->count_detail_levels > 0) {
        crush_models = car_crush_spec->field_0x4c4;
        do {
            if (car_models[i] != NULL) {
                BrModelFree(BrModelRemove(car_models[i]));
                car_models[i] = *crush_models;
            }
            i++;
            crush_models++;
        } while (i < pCar_spec->count_detail_levels);
    }
    pCar_spec->car_model_actor->model = car_models[0];
    BrModelFree(BrModelRemove(car_crush_spec->model_actor->model));
    DRActorEnumRecurse(pCar_spec->car_model_actor, MakeCarModelsMaterialsSingleSided, pCar_spec);
#else
    __asm {
            sub esp, 8
            push ebx
            push ebp
            push esi
            push edi
            mov edi, ecx
            xor ebx, ebx
            mov eax, dword ptr [edi + 0xe0c]
            mov ebp, dword ptr [edi + 0x18d4]
            mov dword ptr [esp + 0x14], ebp
            mov esi, dword ptr [eax + 0x60]
            mov eax, dword ptr [edi + 0xe14]
            test eax, eax
            mov dword ptr [esp + 0x10], esi
            jle L0043c438
            add ebp, 0x4c4
    L0043c402:
            mov eax, dword ptr [esi]
            test eax, eax
            je L0043c41f
            push eax
            call BrModelRemove
            add esp, 4
            push eax
            call BrModelFree
            mov ecx, dword ptr [ebp]
            add esp, 4
            mov dword ptr [esi], ecx
    L0043c41f:
            mov eax, dword ptr [edi + 0xe14]
            inc ebx
            add ebp, 4
            add esi, 4
            cmp ebx, eax
            jl L0043c402
            mov esi, dword ptr [esp + 0x10]
            mov ebp, dword ptr [esp + 0x14]
    L0043c438:
            mov edx, dword ptr [edi + 0xe0c]
            mov eax, dword ptr [esi]
            mov dword ptr [edx + 0x18], eax
            mov ecx, dword ptr [ebp + 0x4d0]
            mov edx, dword ptr [ecx + 0x18]
            push edx
            call BrModelRemove
            add esp, 4
            push eax
            call BrModelFree
            mov ecx, dword ptr [edi + 0xe0c]
            add esp, 4
            mov edx, offset MakeCarModelsMaterialsSingleSided
            push edi
            call DRActorEnumRecurse
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 8
            ret
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
    }
#endif
}


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

// FUNCTION: CARMA2_HW 0x0043d9c0
void C2_HOOK_FASTCALL EncodeNetShapeData(tU8* pData, tCar_spec* pCar) {
    int i;
    int j;
    int count;
    tCar_crush_shape_info* shape_info;
    tPhysics_shape* shape;
    br_vector3* point;

    C2_HOOK_BUG_ON(sizeof(tCompressed_vector3) != 6);

    if (pCar->car_crush_spec->field_0x4b8) {
        count = 0;
        for (shape = pCar->collision_info->shape; shape != NULL; shape = shape->common.next) {
            if (shape->common.type == kCollisionShapeType_Polyhedron || shape->common.type == kCollisionShapeType_Wireframe_Polyhedron) {
                count++;
            }
        }
        *pData++ = (tU8)count;
        for (shape = pCar->collision_info->shape; shape != NULL; shape = shape->common.next) {
            if (shape->common.type == kCollisionShapeType_Polyhedron || shape->common.type == kCollisionShapeType_Wireframe_Polyhedron) {
                *pData = (tU8)shape->polyhedron.polyhedron.count_points;
                pData++;
                for (j = 0; j < shape->polyhedron.polyhedron.count_points; j++) {
                    point = shape->polyhedron.polyhedron.points;
                    point += j;
                    CompressVector3((tCompressed_vector3*)pData, point, -2.5f, 2.5f);
                    pData += sizeof(tCompressed_vector3);
                }
            }
        }
    } else {
        count = 0;
        for (i = 0; i < pCar->car_crush_spec->count_shapes; i++) {
            count += pCar->car_crush_spec->field_0x4[i].count_points;
        }
        *pData++ = (tU8)count;
        for (i = 0; i < pCar->car_crush_spec->count_shapes; i++) {
            shape_info = &pCar->car_crush_spec->field_0x4[i];
            for (j = 0; j < shape_info->count_points; j++) {
                CompressVector3((tCompressed_vector3*)pData, &shape_info->field_0x18[j].field_0xc, -2.5f, 2.5f);
                pData += sizeof(tCompressed_vector3);
            }
        }
    }
}

// DecodeNetShapeData

// CopyBendInMessage

// Vector4Equal

// FUNCTION: CARMA2_HW 0x0043deb0
void C2_HOOK_FASTCALL CompressPlane(tU16 pDest[4], const br_vector4* pSrc) {
    CompressVector3((tCompressed_vector3*)pDest, (const br_vector3*)pSrc, -1.f, 1.f);
    pDest[3] = DRScalarToU16(pSrc->v[3], -2.5f, 2.5f);
}

// FUNCTION: CARMA2_HW 0x0043dee0
void C2_HOOK_FASTCALL ExpandPlane(br_vector4* pDest, const tU16 pSrc[4]) {
    ExpandVector3((br_vector3*)pDest, (const tCompressed_vector3*)pSrc, -1.f, 1.f);
    pDest->v[3] = DRU16ToScalar(pSrc[3], -2.5f, 2.5f);
}

// CopySplitInMessage

// FUNCTION: CARMA2_HW 0x0043dfd0
void C2_HOOK_FASTCALL SetAllShapeStatusFlags(tCar_crush_network_shapes* pShapes) {
    int i;

    if (pShapes == NULL) {
        return;
    }
    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(pShapes->items); i++) {
        if (pShapes->items[i].network_id != 0) {
            pShapes->items[i].field_0x4 = 1;
        }
    }
}
