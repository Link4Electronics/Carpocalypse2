#include "drone.h"

#include "compress.h"

#include "car.h"

#include "displays.h"

#include "errors.h"
#include "finteray.h"
#include "globvars.h"
#include "globvrpb.h"
#include "loading.h"
#include "opponent.h"
#include "pedestrn.h"
#include "physics.h"
#include "piping.h"
#include "platform.h"
#include "utility.h"
#include "world.h"

#include <brender/brender.h>

#include <stdio.h>
#include <math.h>
#include "c2_string.h"

#include "carpocalypse2_macros.h"

#include <stdarg.h>
#include "drone.h"

extern const br_scalar gFLOAT_00589318;
extern const br_scalar gFLOAT_0058931c;
extern const br_scalar gFLOAT_00589320;

// GLOBAL: CARMA2_HW 0x006820d0
int gCount_drones;

// GLOBAL: CARMA2_HW 0x00682178
tDrone_form gDrone_forms[64];

// GLOBAL: CARMA2_HW 0x0068450c
int gCount_drone_forms;

// GLOBAL: CARMA2_HW 0x00594738
tDrone_state_function* gDrone_state_functions[6] = {
    NULL,
    DroneStateFuncReset,
    DroneStateFuncControlledMovement,
    DroneStateFuncPhysicsActive,
    DroneStateFuncStationaryPassive,
    NULL,
};

// GLOBAL: CARMA2_HW 0x006820b8
int gDrones_unmodified;

// GLOBAL: CARMA2_HW 0x00684500
int gCount_rendered_drones;

// GLOBAL: CARMA2_HW 0x006844f8
int gCurrent_selected_drone;

// GLOBAL: CARMA2_HW 0x006820d4
int gINT_006820d4;

// GLOBAL: CARMA2_HW 0x006844fc
int gINT_006844fc;

// GLOBAL: CARMA2_HW 0x00681fb0
int gINT_00681fb0;

// GLOBAL: CARMA2_HW 0x00684504
tDrone_spec* gDrone_specs;

// GLOBAL: CARMA2_HW 0x0068452c
int gShow_drone_paths;

// GLOBAL: CARMA2_HW 0x00681fb4
int gCount_active_drones;

// GLOBAL: CARMA2_HW 0x00684514
int gFrame;

// GLOBAL: CARMA2_HW 0x00684508
tDrone_path_node* gDrone_path_nodes;

// GLOBAL: CARMA2_HW 0x00684510
int gCount_drone_path_nodes;

// GLOBAL: CARMA2_HW 0x006820c0
br_vector3 gRender_bounds_centre;

// GLOBAL: CARMA2_HW 0x00594760
tDrone_form_within_rendering_distance_cbfn* gDrone_form_within_rendering_distance_functions[4] = {
    DroneCarWithinRenderingDistance,
    DronePlaneWithinRenderingDistance,
    DroneTrainWithinRenderingDistance,
    DronePlaneWithinRenderingDistance,
};

// GLOBAL: CARMA2_HW 0x00594750
tDrone_form_within_processing_distance_cbfn* gDrone_form_within_processing_distance_functions[4] = {
    DroneCarWithinProcessingDistance,
    DronePlaneWithinProcessingDistance,
    DroneTrainWithinProcessingDistance,
    DronePlaneWithinProcessingDistance,
};

// GLOBAL: CARMA2_HW 0x005947c0
br_vector3 gDefault_drone_direction = {{ 0.f, 0.f, -1.f }};

// GLOBAL: CARMA2_HW 0x00684520
br_model* gElasticatey_drone_path_model;

// GLOBAL: CARMA2_HW 0x00684528
br_actor* gDrone_path_model_actor;

// GLOBAL: CARMA2_HW 0x00684530
int gDrone_paths_elasticating;

// GLOBAL: CARMA2_HW 0x005947b0
int gSelected_drone_path_node_index = -1;

// GLOBAL: CARMA2_HW 0x005947b4
int gSelected_edit_drone_path = -1;

// GLOBAL: CARMA2_HW 0x006820cc
tU32 gNext_drone_edit_path_munge;

// GLOBAL: CARMA2_HW 0x006820d8
tDrone_crush_data gDrone_crush_data;

// GLOBAL: CARMA2_HW 0x005947b8
int gSelected_drone_path_index = -1;

// GLOBAL: CARMA2_HW 0x005947bc
int gSelected_drone_form_index = -1;

// GLOBAL: CARMA2_HW 0x00589dd4
const float gDrone_select_max_distance_squared = 2.0f;

// GLOBAL: CARMA2_HW 0x00684378
char gDrone_selection_buffer[256];

// GLOBAL: CARMA2_HW 0x00684534
int gDrone_path_output_ready;

// GLOBAL: CARMA2_HW 0x00684538
int gStatic_drone_path_model_vertex_capacity;

// GLOBAL: CARMA2_HW 0x0068453c
int gStatic_drone_path_model_face_capacity;

// GLOBAL: CARMA2_HW 0x00681fb8
tPath_name gDrone_path_output_name;

// GLOBAL: CARMA2_HW 0x00684518
int gTraffic_disabled;

// GLOBAL: CARMA2_HW 0x0068451c
br_model* gStatic_drone_path_model;

// GLOBAL: CARMA2_HW 0x00589dc4
float gDrone_render_zero = 0.0f;

// GLOBAL: CARMA2_HW 0x00684524
br_actor* gStatic_drone_path_model_actor;

#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern int C2_HOOK_FASTCALL IRandomBetween(int pA, int pB);
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern tCollision_shape_box* C2_HOOK_FASTCALL AllocateBoxCollisionShape(int pType);
extern br_actor* C2_HOOK_CDECL BrActorAdd();
extern br_actor* C2_HOOK_CDECL BrActorAllocate();
extern br_bounds* C2_HOOK_CDECL BrActorToBounds();
extern void C2_HOOK_CDECL BrMatrix34ApplyV();
extern void C2_HOOK_CDECL BrMatrix34Copy();
extern void C2_HOOK_CDECL BrMatrix34LPInverse();
extern void C2_HOOK_CDECL BrMatrix34PreRotateX();
extern void C2_HOOK_CDECL BrMatrix34PreRotateY();
extern void C2_HOOK_CDECL BrMatrix34PreRotateZ();
extern void C2_HOOK_CDECL BrMatrix34Rotate();
extern void* C2_HOOK_CDECL BrMemAllocate();
extern br_model* C2_HOOK_CDECL BrModelAdd();
extern br_model* C2_HOOK_CDECL BrModelAllocate();
extern void C2_HOOK_CDECL BrModelUpdate();
extern void C2_HOOK_CDECL BrVector3SetFloat();
extern void C2_HOOK_FASTCALL BuildDronePathNodeMesh(br_model* pModel, int pVert_index, int pFace_index, const br_vector3* pPosition, tU32 pMaterial1, tU32 pMaterial2, tU32 pMaterial3);
extern void C2_HOOK_FASTCALL CalcRenderBoundsCentre(void);
extern tU16 C2_HOOK_FASTCALL DRScalarToU16(float pValue, float pMin, float pMax);
extern FILE* C2_HOOK_FASTCALL DRfopen(const char* pFilename, const char* pMode);
extern void C2_HOOK_FASTCALL FUN_00452f40(br_model* pModel, int pFace_count, int pVertex_count);
extern void C2_HOOK_FASTCALL FillInShape(tPhysics_shape* pShape);
extern char * C2_HOOK_FASTCALL GetALineAndDontArgue(FILE* pF, char* pS);
extern void C2_HOOK_FASTCALL GetAString(FILE* pF, char* pString);
extern int C2_HOOK_FASTCALL GetAnInt(FILE* pF);
extern void C2_HOOK_FASTCALL GetNScalars(FILE* pF, int n, br_scalar* pS);
extern void C2_HOOK_FASTCALL GetNewBoundingBox(br_bounds* b2, br_bounds* b1, br_matrix34* m);
extern int C2_HOOK_FASTCALL InitialiseEditModelsEtc(void);
extern void C2_HOOK_FASTCALL LoadDroneActorsModels(tDrone_spec* pDrone);
extern int C2_HOOK_FASTCALL NewTextHeadupSlot(int pSlot_index, int pFlash_rate, int pLifetime, int pFont_index, const char* pText);
extern void C2_HOOK_FASTCALL PDEnterDebugger(const char* pStr);
extern void C2_NORETURN C2_HOOK_FASTCALL PDFatalError(const char* pThe_str);
extern int C2_HOOK_FASTCALL PFfclose(FILE* pFile);
extern int C2_HOOK_FASTCALL PHILAddObject(tPhysics_object* pObject);
extern int C2_HOOK_FASTCALL PHILRemoveObject(tPhysics_object* pObject);
extern int C2_HOOK_CDECL PHILSetObjectProperty();
extern void C2_HOOK_FASTCALL PathCat(char* pDestn_str, const char* pStr_1, const char* pStr_2);
extern void C2_HOOK_FASTCALL PipeDroneMatrix(tDrone_spec* pDrone);
extern void C2_HOOK_FASTCALL PipeSingleDroneRender(tDrone_spec* pDrone_spec, int pRender);
extern void C2_HOOK_FASTCALL PipeSingleDroneStraightPos(tDrone_spec* pDrone, undefined4 pParam1, tU16 pParam2, tU16 pParam3);
extern void C2_HOOK_FASTCALL PointActorAlongThisBloodyVector(br_actor* pThe_actor, br_vector3* pThe_vector);
extern void C2_HOOK_FASTCALL PreprocessDronePaths(void);
extern void C2_HOOK_FASTCALL PutOpponentsInNeutral(void);
extern void C2_HOOK_FASTCALL SelectNearestDrone(void);
extern void C2_HOOK_FASTCALL StartRenderingThisDrone(tDrone_spec* pDrone);
extern void C2_HOOK_FASTCALL StopProcessingThisDrone(tDrone_spec* pDrone, int pForce);
extern void C2_HOOK_FASTCALL StopRenderingThisDrone(tDrone_spec* pDrone_spec);
extern void C2_HOOK_FASTCALL UpdateCollisionObject(tPhysics_object* pCollision_info);
extern void C2_HOOK_FASTCALL Uppercaseificate(char* dest, const char* src);
extern void C2_HOOK_FASTCALL WriteOutDronePaths(void);
extern char _CIacos[];
extern void __cdecl _ftol();
extern void C2_HOOK_CDECL dr_dprintf();
extern tPath_name gApplication_path;
extern br_matrix34 gCamera_to_world;
extern char gDOUBLE_00589d80[];
extern tBrender_storage gDroneStorage;
extern float gDrone_delta_time;
extern int gFace_num__car;
extern int gFirst_drone_processing;
extern tU32 gFrame_period;
extern tPhysics_object* gList_collision_infos;
extern br_material* gMat_dk_grn;
extern br_material* gMat_dk_turq;
extern br_material* gMat_dk_yel;
extern br_material* gMat_lt_grn;
extern br_material* gMat_lt_turq;
extern br_material* gMat_lt_yel;
extern br_material* gMat_md_grn;
extern br_material* gMat_md_yel;
extern tNet_mode gNet_mode;
extern br_actor* gNon_track_actor;
extern int gPed_676914;
extern char gPed_flags_0058f2e4[];
extern tProgram_state gProgram_state;
extern tU32 gTime_stamp_for_this_munging;
extern float gTrack_drone_min_y;
#endif
// FUNCTION: CARMA2_HW 0x00452810
void C2_HOOK_FASTCALL SetDroneRender(int pIndex, int pRender) {
    if (gPed_676914 && ARGetReplayRate() < gDrone_render_zero) {
        pRender = (pRender == 0);
        DoNotDprintf("B DRONERENDER");
    } else {
        DoNotDprintf("F DRONERENDER");
    }
    gDrone_specs[pIndex].actor->render_style = (pRender == 0) ? BR_RSTYLE_NONE : BR_RSTYLE_FACES;
}

// FUNCTION: CARMA2_HW 0x0044cfc0
void C2_HOOK_CDECL DoNotDprintf(const char* format, ...) {
// Disabled because too noisy
#if 0
    va_list ap;

    va_start(ap, format);
    vfprintf(stderr, format, ap);
    fputc('\n', stderr);
    va_end(ap);
#endif
}

// FUNCTION: CARMA2_HW 0x00451210
void C2_NAKED C2_HOOK_FASTCALL InitDroneSpec(tDrone_spec* pDrone_spec, int pNode) {
#ifndef CARPOCALYPSE2_MATCHING

    C2_HOOK_BUG_ON(sizeof(*pDrone_spec) != 0x5d8);

    DoNotDprintf("InitDroneSpec() - INITIALISING DRONE SPEC");
    memset(pDrone_spec, 0, sizeof(*pDrone_spec));
    pDrone_spec->field_0xa_pathnode_id = pNode;
    pDrone_spec->field_0x8_pathnode_id = pNode;
    pDrone_spec->field_0xe = -1;
    pDrone_spec->field_0xc = -1;
    BrVector3Set(&pDrone_spec->field_0x18, 0.f, 0.f, 0.f);
    BrVector3Copy(&pDrone_spec->pos, &gDrone_path_nodes[pNode].position);
    pDrone_spec->field_0xdc = 0;
    pDrone_spec->field_0xf4 = -1;
    pDrone_spec->current_state = 0;
    pDrone_spec->field_0x5d4 = 0;
    pDrone_spec->form = &gDrone_forms[gDrone_path_nodes[pNode].type];
    pDrone_spec->field_0xe4 = 1.0f;
    pDrone_spec->field_0x48 = 1.0f;
#else
    __asm {
            push ebx
            push esi
            push edi
            mov ebx, edx
            mov esi, ecx
            push offset gDefault_drone_direction+0x954
            call DoNotDprintf
            mov ecx, 0x176
            xor eax, eax
            mov edi, esi
            or edx, 0xffffffff
            rep stosd
            lea eax, [ebx + ebx*8]
            xor ecx, ecx
            mov word ptr [esi + 0xa], bx
            mov word ptr [esi + 8], bx
            lea eax, [ebx + eax*2]
            mov word ptr [esi + 0xe], dx
            mov word ptr [esi + 0xc], dx
            mov dword ptr [esi + 0x18], ecx
            lea eax, [ebx + eax*4]
            mov dword ptr [esi + 0x1c], ecx
            mov dword ptr [esi + 0x20], ecx
            mov edi, dword ptr [gDrone_path_nodes]
            shl eax, 2
            add esp, 4
            mov edi, dword ptr [edi + eax]
            mov dword ptr [esi + 0x24], edi
            mov edi, dword ptr [gDrone_path_nodes]
            mov edi, dword ptr [edi + eax + 4]
            mov dword ptr [esi + 0x28], edi
            mov edi, dword ptr [gDrone_path_nodes]
            mov edi, dword ptr [edi + eax + 8]
            mov dword ptr [esi + 0xdc], ecx
            mov dword ptr [esi + 0x2c], edi
            mov edi, dword ptr [gDrone_path_nodes]
            movsx eax, word ptr [edi + eax + 0x130]
            mov edi, eax
            mov dword ptr [esi + 0xf4], edx
            shl edi, 4
            add edi, eax
            mov dword ptr [esi + 0x38], ecx
            mov dword ptr [esi + 0x5d4], ecx
            lea eax, [edi*8 + gDrone_forms]
            pop edi
            mov dword ptr [esi + 4], eax
            mov eax, 0x3f800000
            mov dword ptr [esi + 0xe4], eax
            mov dword ptr [esi + 0x48], eax
            pop esi
            pop ebx
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00451070
void C2_NAKED C2_HOOK_FASTCALL PreprocessDronePaths(void) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;
    for (i = 0; i < gCount_drone_path_nodes; i++) {
        tDrone_path_node* node;
        int j;

        node = &gDrone_path_nodes[i];
        for (j = 0; j < node->count_sections; j++) {
            tDrone_path_node_section* section;

            section = &node->sections[j];
            BrVector3Sub(&section->field_0x04, &gDrone_path_nodes[section->node1].position, &node->position);
            section->field_0x1c = BrVector3Length(&section->field_0x04);
            BrVector3Normalise(&section->field_0x10, &section->field_0x04);
        }
    }
#else
    __asm {
            sub esp, 8
            mov eax, dword ptr [gCount_drone_path_nodes]
            push ebx
            xor ebx, ebx
            cmp eax, ebx
            mov dword ptr [esp + 8], ebx
            jle L0045120b
            mov edx, dword ptr [gDrone_path_nodes]
            push edi
            push esi
            push ebp
            xor esi, esi
    L00451092:
            xor edi, edi
            cmp word ptr [edx + esi + 0x12c], bx
            jbe L004511eb
            mov ecx, esi
    L004510a4:
            movsx eax, word ptr [edx + ecx + 0xc]
            lea ebp, [eax + eax*8]
            lea ebp, [eax + ebp*2]
            lea eax, [eax + ebp*4]
            fld dword ptr [edx + eax*4]
            fsub dword ptr [edx + esi]
            fstp dword ptr [edx + ecx + 0x10]
            mov edx, dword ptr [gDrone_path_nodes]
            movsx eax, word ptr [edx + ecx + 0xc]
            lea ebp, [eax + eax*8]
            lea ebp, [eax + ebp*2]
            lea eax, [eax + ebp*4]
            fld dword ptr [edx + eax*4 + 4]
            fsub dword ptr [edx + esi + 4]
            fstp dword ptr [edx + ecx + 0x14]
            mov edx, dword ptr [gDrone_path_nodes]
            movsx eax, word ptr [edx + ecx + 0xc]
            lea ebp, [eax + eax*8]
            lea ebp, [eax + ebp*2]
            lea eax, [eax + ebp*4]
            fld dword ptr [edx + eax*4 + 8]
            fsub dword ptr [edx + esi + 8]
            fstp dword ptr [edx + ecx + 0x18]
            mov eax, dword ptr [gDrone_path_nodes]
            fld dword ptr [eax + ecx + 0x10]
            fld dword ptr [eax + ecx + 0x14]
            fld dword ptr [eax + ecx + 0x18]
            fxch st(2)
            fst dword ptr [esp + 0x10]
            fld st(1)
            fxch st(1)
            fmul dword ptr [esp + 0x10]
            fxch st(1)
            fmul st(0), st(2)
            fxch st(3)
            fst dword ptr [esp + 0x10]
            fmul dword ptr [esp + 0x10]
            fxch st(3)
            faddp st(1), st(0)
            fxch st(2)
            faddp st(2), st(0)
            fxch st(1)
            fsqrt
            fstp st(1)
            fstp dword ptr [eax + ecx + 0x28]
            mov edx, dword ptr [gDrone_path_nodes]
            fld dword ptr [edx + ecx + 0x14]
            fld dword ptr [edx + ecx + 0x18]
            fld dword ptr [edx + ecx + 0x10]
            fst dword ptr [esp + 0x10]
            fld st(2)
            fxch st(1)
            fmul dword ptr [esp + 0x10]
            fxch st(1)
            fmul st(0), st(3)
            fxch st(2)
            fst dword ptr [esp + 0x10]
            fmul dword ptr [esp + 0x10]
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fsqrt
            fstp st(1)
            fcom dword ptr [gDrone_select_max_distance_squared+0x10]
            fnstsw ax
            test ah, 0x41
            jne L004511b2
            fdivr qword ptr [gDrone_select_max_distance_squared+0x14]
            fld dword ptr [edx + ecx + 0x10]
            fmul st(0), st(1)
            fstp dword ptr [edx + ecx + 0x1c]
            mov eax, dword ptr [gDrone_path_nodes]
            fld dword ptr [eax + ecx + 0x14]
            fmul st(0), st(1)
            fstp dword ptr [eax + ecx + 0x20]
            mov eax, dword ptr [gDrone_path_nodes]
            fld dword ptr [eax + ecx + 0x18]
            fmul st(0), st(1)
            fstp dword ptr [eax + ecx + 0x24]
            fstp st(0)
            jmp L004511cf
    L004511b2:
            mov dword ptr [edx + ecx + 0x1c], 0x3f800000
            mov edx, dword ptr [gDrone_path_nodes]
            fstp st(0)
            mov dword ptr [edx + ecx + 0x20], ebx
            mov eax, dword ptr [gDrone_path_nodes]
            mov dword ptr [eax + ecx + 0x24], ebx
    L004511cf:
            mov edx, dword ptr [gDrone_path_nodes]
            xor eax, eax
            inc edi
            add ecx, 0x24
            mov ax, word ptr [edx + esi + 0x12c]
            cmp edi, eax
            jl L004510a4
    L004511eb:
            mov eax, dword ptr [esp + 0x14]
            mov ecx, dword ptr [gCount_drone_path_nodes]
            inc eax
            add esi, 0x134
            cmp eax, ecx
            mov dword ptr [esp + 0x14], eax
            jl L00451092
            pop ebp
            pop esi
            pop edi
    L0045120b:
            pop ebx
            add esp, 8
            ret
    }
#endif
}

void C2_HOOK_FASTCALL AllocateAndInitDrones(void) {
    int i;
    int drone_i;

    C2_HOOK_BUG_ON(sizeof(tDrone_spec) != 0x5d8);

    gDrone_specs = BrMemAllocate(gCount_drones * sizeof(tDrone_spec), kMem_drone_specs);
    drone_i = 0;
    for (i = 0; i < gCount_drone_path_nodes; i++) {
        tDrone_path_node* node;

        node = &gDrone_path_nodes[i];
        if (node->type >= 0) {
            InitDroneSpec(&gDrone_specs[drone_i], i);
            drone_i += 1;
        }
    }
}

// FUNCTION: CARMA2_HW 0x00450bf0
void C2_NAKED C2_HOOK_FASTCALL LoadInDronePaths(FILE* pF) {
#ifndef CARPOCALYPSE2_MATCHING
    char s[256];

    DoNotDprintf("Start of LoadInDronePaths()...");
    for (;;) {
        if (!GetALineAndDontArgue(pF, s)) {
            PDFatalError("Corrupt race text file - No drone path info");
            break;
        }
        if (strcmp(s, "START OF DRONE PATHS") == 0) {
            break;
        }
    }
    gCount_drones = 0;
    gCount_drone_path_nodes = 0;
    if (gNet_mode == eNet_mode_none && !gTraffic_disabled) {
        int version;

        /* version */
        version = GetAnInt(pF);

        if (version > 0) {
            int i;

            if (version != 2) {
                PDFatalError("Drones paths are not version 2");
            }

            C2_HOOK_BUG_ON(sizeof(tDrone_path_node) != 0x134);

            /* n_nodes */
            gCount_drone_path_nodes = GetAnInt(pF);
            if (gCount_drone_path_nodes != 0) {
                gDrone_path_nodes = BrMemAllocate(gCount_drone_path_nodes * sizeof(tDrone_path_node), kMem_drone_paths);
            }
            for (i = 0; i < gCount_drone_path_nodes; i++) {
                br_scalar float_buffer[4];
                tDrone_path_node *node = &gDrone_path_nodes[i];
                int j;

                GetNScalars(pF, 3, float_buffer);
                node->position.v[0] = float_buffer[0];
                node->position.v[1] = float_buffer[1];
                node->position.v[2] = float_buffer[2];

                s[0] = '\0';
                GetAString(pF, s);
                Uppercaseificate(s, s);
                node->type = -1;
                if (strlen(s) == 0) {
                    sprintf(s, "Corrupt race text file - drone type corrupt in drone node %d", i);
                    PDFatalError(s);
                }

                if (strcmp(s, "NONE") != 0) {
                    for (j = 0; j < gCount_drone_forms; j++) {
                        if (strcmp(gDrone_forms[j].name, s) == 0) {
                            node->type = j;
                        }
                    }
                    if (node->type < 0) {
                        char s2[256];

                        sprintf(s2, "ERROR - '%s' no such drone type in drone node %d", s, i);
                        PDFatalError(s2);
                    }
                }
                GetNScalars(pF, 1, float_buffer);
                node->field_0x12e = (int)float_buffer[0];

                C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(node->sections) != 8);

                node->count_sections = GetAnInt(pF);
                if (node->count_sections > CARPOCALYPSE2_ASIZE(node->sections)) {

                    sprintf(s, "ERROR - drone node %d has too many sections", i);
                    PDFatalError(s);
                }

                C2_HOOK_BUG_ON(sizeof(tDrone_path_node_section) != 0x24);
                for (j = 0; j < node->count_sections; j++) {
                    tDrone_path_node_section *section = &node->sections[j];

                    GetNScalars(pF, 4, float_buffer);
                    section->node1 = (int)float_buffer[0];
                    section->node2 = (int)float_buffer[1];
                    section->field_0x21 = (int)float_buffer[2];
                    section->type = (int)float_buffer[3];
                    if (section->type > 7) {
                        PDEnterDebugger("Weird type for drone section");
                    }
                }
                if (node->type >= 0) {
                    /* FIXME: use constant for drone limit */
                    if (gCount_drones >= 200) {
                        sprintf(s, "Too many drones in race (limit %d)", 200);
                        PDFatalError(s);
                    }
                    gCount_drones += 1;
                }
            }
        }
    }
    for (;;) {
        const char *str = GetALineAndDontArgue(pF, s);
        if (str == NULL) {
            break;
        }
        if (strcmp(s, "END OF DRONE PATHS") == 0) {
            break;
        }
    }
    PreprocessDronePaths();
    AllocateAndInitDrones();
    DoNotDprintf("End of LoadInDronePaths(), totals:");
    DoNotDprintf("Nodes: %d", gCount_drone_path_nodes);
#else
    __asm {
            sub esp, 0x23c
            push ebx
            push ebp
            push esi
            push edi
            mov ebp, ecx
            push offset gDefault_drone_direction+0x934
            mov dword ptr [esp + 0x14], ebp
            call DoNotDprintf
            add esp, 4
    L00450c0d:
            lea edx, [esp + 0x4c]
            mov ecx, ebp
            call GetALineAndDontArgue
            mov edi, eax
            test edi, edi
            je L00450c5c
            lea esi, [esp + 0x4c]
            mov ecx, offset gDefault_drone_direction+0x91c
    L00450c27:
            mov al, byte ptr [ecx]
            mov bl, byte ptr [esi]
            mov dl, al
            cmp al, bl
            jne L00450c4f
            test dl, dl
            je L00450c4b
            mov al, byte ptr [ecx + 1]
            mov bl, byte ptr [esi + 1]
            mov dl, al
            cmp al, bl
            jne L00450c4f
            add ecx, 2
            add esi, 2
            test dl, dl
            jne L00450c27
    L00450c4b:
            xor ecx, ecx
            jmp L00450c54
    L00450c4f:
            sbb ecx, ecx
            sbb ecx, -1
    L00450c54:
            test ecx, ecx
            jne L00450c0d
            test edi, edi
            jne L00450c66
    L00450c5c:
            mov ecx, offset gDefault_drone_direction+0x8f0
            call PDFatalError
    L00450c66:
            mov eax, dword ptr [gNet_mode]
            mov dword ptr [gCount_drones], 0
            test eax, eax
            mov dword ptr [gCount_drone_path_nodes], 0
            jne L00450f57
            mov eax, dword ptr [gTraffic_disabled]
            test eax, eax
            jne L00450f57
            mov ecx, ebp
            call GetAnInt
            test eax, eax
            jle L00450f57
            cmp eax, 2
            je L00450cb2
            mov ecx, offset gDefault_drone_direction+0x8d0
            call PDFatalError
    L00450cb2:
            mov ecx, ebp
            call GetAnInt
            test eax, eax
            mov dword ptr [gCount_drone_path_nodes], eax
            je L00450ce6
            lea ecx, [eax + eax*8]
            push 0xeb
            lea edx, [eax + ecx*2]
            lea eax, [eax + edx*4]
            shl eax, 2
            push eax
            call BrMemAllocate
            mov dword ptr [gDrone_path_nodes], eax
            mov eax, dword ptr [gCount_drone_path_nodes]
            add esp, 8
    L00450ce6:
            xor ebx, ebx
            test eax, eax
            mov dword ptr [esp + 0x18], ebx
            jle L00450f57
            mov dword ptr [esp + 0x14], ebx
    L00450cf8:
            mov ecx, dword ptr [gDrone_path_nodes]
            mov edx, dword ptr [esp + 0x14]
            mov esi, dword ptr [esp + 0x10]
            lea eax, [esp + 0x1c]
            lea ebp, [ecx + edx]
            push eax
            mov edx, 3
            mov ecx, esi
            call GetNScalars
            mov ecx, dword ptr [esp + 0x1c]
            mov dword ptr [ebp], ecx
            mov edx, dword ptr [esp + 0x20]
            mov dword ptr [ebp + 4], edx
            mov eax, dword ptr [esp + 0x24]
            mov dword ptr [ebp + 8], eax
            lea edx, [esp + 0x4c]
            mov ecx, esi
            mov byte ptr [esp + 0x4c], 0
            call GetAString
            lea edx, [esp + 0x4c]
            lea ecx, [esp + 0x4c]
            call Uppercaseificate
            or ecx, 0xffffffff
            lea edi, [esp + 0x4c]
            xor eax, eax
            mov word ptr [ebp + 0x130], cx
            repne scasb
            not ecx
            dec ecx
            je L0045104b
            mov esi, offset gPed_flags_0058f2e4+0xd8
            lea eax, [esp + 0x4c]
    L00450d70:
            mov dl, byte ptr [eax]
            mov cl, dl
            cmp dl, byte ptr [esi]
            jne L00450d94
            test cl, cl
            je L00450d90
            mov dl, byte ptr [eax + 1]
            mov cl, dl
            cmp dl, byte ptr [esi + 1]
            jne L00450d94
            add eax, 2
            add esi, 2
            test cl, cl
            jne L00450d70
    L00450d90:
            xor eax, eax
            jmp L00450d99
    L00450d94:
            sbb eax, eax
            sbb eax, -1
    L00450d99:
            test eax, eax
            je L00450e30
            mov eax, dword ptr [gCount_drone_forms]
            xor edi, edi
            test eax, eax
            jle L00450dff
            mov edx, offset gDrone_forms
    L00450db1:
            mov esi, edx
            lea eax, [esp + 0x4c]
    L00450db7:
            mov bl, byte ptr [eax]
            mov cl, bl
            cmp bl, byte ptr [esi]
            jne L00450ddb
            test cl, cl
            je L00450dd7
            mov bl, byte ptr [eax + 1]
            mov cl, bl
            cmp bl, byte ptr [esi + 1]
            jne L00450ddb
            add eax, 2
            add esi, 2
            test cl, cl
            jne L00450db7
    L00450dd7:
            xor eax, eax
            jmp L00450de0
    L00450ddb:
            sbb eax, eax
            sbb eax, -1
    L00450de0:
            test eax, eax
            jne L00450deb
            mov word ptr [ebp + 0x130], di
    L00450deb:
            mov eax, dword ptr [gCount_drone_forms]
            inc edi
            add edx, 0x88
            cmp edi, eax
            jl L00450db1
            mov ebx, dword ptr [esp + 0x18]
    L00450dff:
            cmp word ptr [ebp + 0x130], 0
            jge L00450e30
            lea eax, [esp + 0x4c]
            push ebx
            push eax
            lea ecx, [esp + 0x154]
            push offset gDefault_drone_direction+0x89c
            push ecx
            call sprintf
            add esp, 0x10
            lea ecx, [esp + 0x14c]
            call PDFatalError
    L00450e30:
            mov esi, dword ptr [esp + 0x10]
    L00450e34:
            lea eax, [esp + 0x1c]
            mov edx, 1
            push eax
            mov ecx, esi
            call GetNScalars
            fld dword ptr [esp + 0x1c]
            call _ftol
            mov ecx, esi
            mov byte ptr [ebp + 0x12e], al
            call GetAnInt
            cmp ax, 8
            mov word ptr [ebp + 0x12c], ax
            jbe L00450e84
            push ebx
            lea ecx, [esp + 0x50]
            push offset gDefault_drone_direction+0x870
            push ecx
            call sprintf
            add esp, 0xc
            lea ecx, [esp + 0x4c]
            call PDFatalError
    L00450e84:
            xor edi, edi
            cmp word ptr [ebp + 0x12c], di
            jbe L00450ef5
            lea esi, [ebp + 0xe]
    L00450e92:
            mov ecx, dword ptr [esp + 0x10]
            lea edx, [esp + 0x1c]
            push edx
            mov edx, 4
            call GetNScalars
            fld dword ptr [esp + 0x1c]
            call _ftol
            mov word ptr [esi - 2], ax
            fld dword ptr [esp + 0x20]
            call _ftol
            mov word ptr [esi], ax
            fld dword ptr [esp + 0x24]
            call _ftol
            mov byte ptr [esi + 0x1f], al
            fld dword ptr [esp + 0x28]
            call _ftol
            cmp al, 7
            mov byte ptr [esi + 0x1e], al
            jbe L00450ee4
            mov ecx, offset gDefault_drone_direction+0x850
            call PDEnterDebugger
    L00450ee4:
            xor eax, eax
            inc edi
            mov ax, word ptr [ebp + 0x12c]
            add esi, 0x24
            cmp edi, eax
            jl L00450e92
    L00450ef5:
            cmp word ptr [ebp + 0x130], 0
            jl L00450f33
            mov eax, dword ptr [gCount_drones]
            cmp eax, 0xc8
            jge L00450f13
            inc eax
            mov dword ptr [gCount_drones], eax
            jmp L00450f33
    L00450f13:
            push 0xc8
            lea ecx, [esp + 0x50]
            push offset gDefault_drone_direction+0x82c
            push ecx
            call sprintf
            add esp, 0xc
            lea ecx, [esp + 0x4c]
            call PDFatalError
    L00450f33:
            mov ecx, dword ptr [esp + 0x14]
            mov eax, dword ptr [gCount_drone_path_nodes]
            inc ebx
            add ecx, 0x134
            cmp ebx, eax
            mov dword ptr [esp + 0x18], ebx
            mov dword ptr [esp + 0x14], ecx
            jl L00450cf8
            mov ebp, dword ptr [esp + 0x10]
    L00450f57:
            lea edx, [esp + 0x4c]
            mov ecx, ebp
            call GetALineAndDontArgue
            test eax, eax
            je L00450fa0
            lea esi, [esp + 0x4c]
            mov eax, offset gDefault_drone_direction+0x818
    L00450f6f:
            mov dl, byte ptr [eax]
            mov bl, byte ptr [esi]
            mov cl, dl
            cmp dl, bl
            jne L00450f97
            test cl, cl
            je L00450f93
            mov dl, byte ptr [eax + 1]
            mov bl, byte ptr [esi + 1]
            mov cl, dl
            cmp dl, bl
            jne L00450f97
            add eax, 2
            add esi, 2
            test cl, cl
            jne L00450f6f
    L00450f93:
            xor eax, eax
            jmp L00450f9c
    L00450f97:
            sbb eax, eax
            sbb eax, -1
    L00450f9c:
            test eax, eax
            jne L00450f57
    L00450fa0:
            call PreprocessDronePaths
            mov ecx, dword ptr [gCount_drones]
            xor ebp, ebp
            mov eax, ecx
            push 0xea
            shl eax, 5
            sub eax, ecx
            lea eax, [eax + eax*2]
            lea ecx, [ecx + eax*2]
            shl ecx, 3
            push ecx
            call BrMemAllocate
            mov dword ptr [gDrone_specs], eax
            mov eax, dword ptr [gCount_drone_path_nodes]
            add esp, 8
            xor edi, edi
            test eax, eax
            jle L0045101f
            xor esi, esi
            xor ebx, ebx
    L00450fdf:
            mov edx, dword ptr [gDrone_path_nodes]
            cmp word ptr [edx + ebx + 0x130], 0
            jl L0045100f
            mov eax, dword ptr [gDrone_specs]
            mov edx, edi
            lea ecx, [eax + esi]
            call InitDroneSpec
            mov ecx, dword ptr [gDrone_specs]
            mov dword ptr [ecx + esi], ebp
            inc ebp
            add esi, 0x5d8
    L0045100f:
            mov eax, dword ptr [gCount_drone_path_nodes]
            inc edi
            add ebx, 0x134
            cmp edi, eax
            jl L00450fdf
    L0045101f:
            push offset gDefault_drone_direction+0x7f4
            call DoNotDprintf
            mov edx, dword ptr [gCount_drone_path_nodes]
            add esp, 4
            push edx
            push offset gDefault_drone_direction+0x7e8
            call DoNotDprintf
            add esp, 8
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x23c
            ret
    L0045104b:
            push ebx
            lea edx, [esp + 0x50]
            push offset gDefault_drone_direction+0x7a8
            push edx
            call sprintf
            add esp, 0xc
            lea ecx, [esp + 0x4c]
            call PDFatalError
            jmp L00450e34
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004516b0
void C2_HOOK_FASTCALL DoDefaultDroneStateAction(tDrone_spec* pDrone_spec) {
    if (gDrone_state_functions[pDrone_spec->current_state] != NULL) {
        gDrone_state_functions[pDrone_spec->current_state](pDrone_spec, eDrone_state_DEFAULT);
    }
}

void C2_HOOK_FASTCALL NewDroneState(tDrone_spec* pDrone_spec, int pNew_state) {
    DoNotDprintf("NewDroneState() Drone %d, Current state %d, New state %d",
        pDrone_spec->id, pDrone_spec->current_state, pNew_state);
    if (pDrone_spec->current_state != pNew_state) {
        if (gDrone_state_functions[pDrone_spec->current_state] != NULL) {
            gDrone_state_functions[pDrone_spec->current_state](pDrone_spec, eDrone_state_STOP);
        }
        pDrone_spec->time_last_munge = gTime_stamp_for_this_munging;
        pDrone_spec->prev_states[1] = pDrone_spec->prev_states[0];
        pDrone_spec->prev_states[0] = pDrone_spec->current_state;
        pDrone_spec->current_state = pNew_state;
        if (gDrone_state_functions[pNew_state] != NULL) {
            gDrone_state_functions[pNew_state](pDrone_spec, eDrone_state_START);
        }
    }
}

// FUNCTION: CARMA2_HW 0x00428d40
int C2_HOOK_FASTCALL TestObjectOverlap(tPhysics_object* pCollision_1, tPhysics_object* pCollision_2) {
    tPhysics_shape* shape1;
    tPhysics_shape* shape2;

    shape1 = pCollision_1->shape;
    shape2 = pCollision_2->shape;
    if (FlagsAreGo(pCollision_1, pCollision_2)
        && BoundsOverlapTest__finteray(&pCollision_1->field_0xf4, &pCollision_2->field_0xf4)) {
        while (shape1 != NULL) {
            while (shape2 != NULL) {
                if (TestShapeEdgeIntersection(pCollision_1, pCollision_2, shape1, shape2)
                    || TestShapeEdgeIntersection(pCollision_2, pCollision_1, shape2, shape1)) {
                    return 1;
                }
                shape2 = shape2->common.next;
            }
            shape1 = shape1->common.next;
        }
    }
    return 0;
}

int C2_HOOK_FASTCALL DroneHasCollided(tDrone_spec* pDrone_spec) {
    tPhysics_object* collision = gList_collision_infos;
    for (collision = gList_collision_infos; collision != NULL; collision = collision->next) {
        if (&pDrone_spec->collision_info != collision && TestObjectOverlap(&pDrone_spec->collision_info, collision)) {
            return 1;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0044f980
void C2_NAKED C2_HOOK_FASTCALL InitDroneCollisionObject(tDrone_spec *pDrone_spec) {
#ifndef CARPOCALYPSE2_MATCHING
    br_bounds3 bnds;
    br_vector3 size;
    tPhysics_shape* original_shape;

#ifdef CARPOCALYPSE2_FIX_BUGS
    original_shape = NULL;
#endif

    BrActorToBounds(&bnds, pDrone_spec->actor);
    bnds.min.v[1] += .001f;
    if (pDrone_spec->field_0x46) {
        original_shape = pDrone_spec->collision_info.shape;
    }
    memset(&pDrone_spec->collision_info, 0, sizeof(pDrone_spec->collision_info));
    if (pDrone_spec->field_0x46) {
        pDrone_spec->collision_info.shape = original_shape;
    } else {
        pDrone_spec->collision_info.shape = (tPhysics_shape*)AllocateBoxCollisionShape(kMem_drone_collision_stuff);
        pDrone_spec->collision_info.shape->box.common.bb = bnds;
        pDrone_spec->collision_info.shape->box.common.field_0x1c = bnds;
        pDrone_spec->collision_info.shape->box.common.next = NULL;
        pDrone_spec->field_0x46 = 1;
    }
    pDrone_spec->collision_info.flags_0x238 = 16;
    pDrone_spec->collision_info.owner = pDrone_spec;
    pDrone_spec->collision_info.actor = pDrone_spec->actor;
    pDrone_spec->collision_info.M =  pDrone_spec->form->M;
    BrVector3Set(&pDrone_spec->collision_info.cmpos, .0f, .09f, .0f);
    BrVector3Set(&pDrone_spec->collision_info.field_0x54, .0f, -.4f / WORLD_SCALE, .0f);
    BrMatrix34Copy(&pDrone_spec->collision_info.transform_matrix, &pDrone_spec->actor->t.t.mat);
    pDrone_spec->collision_info.box_face_ref = gFace_num__car - 2;
    pDrone_spec->collision_info.field_0x1a0 = 0x100000;
    FillInShape(pDrone_spec->collision_info.shape);
    UpdateCollisionObject(&pDrone_spec->collision_info);
    GetNewBoundingBox(&pDrone_spec->collision_info.field_0xf4,
        &pDrone_spec->collision_info.bb1,
        &pDrone_spec->collision_info.transform_matrix);
    BrVector3Sub(&size, &bnds.max, &bnds.min);
    pDrone_spec->collision_info.I.v[0] = (size.v[2] * size.v[2] + size.v[1] * size.v[1]) * pDrone_spec->collision_info.M / 12.f;
    pDrone_spec->collision_info.I.v[1] = (size.v[2] * size.v[2] + size.v[0] * size.v[0]) * pDrone_spec->collision_info.M / 12.f;
    pDrone_spec->collision_info.I.v[2] = (size.v[1] * size.v[1] + size.v[0] * size.v[0]) * pDrone_spec->collision_info.M / 12.f;
    // FIXME: is this calculation correct?
    pDrone_spec->collision_info.cmpos.v[0] = bnds.min.v[0] + .01f * pDrone_spec->form->center.v[1] * size.v[0];
    pDrone_spec->collision_info.cmpos.v[1] = bnds.min.v[1] + .01f * pDrone_spec->form->center.v[2] * size.v[1];
    pDrone_spec->collision_info.cmpos.v[2] = bnds.min.v[2] + .01f * pDrone_spec->form->center.v[0] * size.v[2];
    if (pDrone_spec->form->type == kDroneType_train) {
        pDrone_spec->collision_info.world_friction = -.75;
    }
    pDrone_spec->collision_info.disable_move_rotate = 1;
    pDrone_spec->collision_info.field_0xed = 1;
    pDrone_spec->collision_info.flags |= 0x40;
    pDrone_spec->collision_info.drivable_on = !!(pDrone_spec->form->flags & 0x8);
    BrVector3SetFloat(&pDrone_spec->collision_info.v, .0f, .0f, .0f);
    BrVector3SetFloat(&pDrone_spec->collision_info.omega, .0f, .0f, .0f);
    BrVector3SetFloat(&pDrone_spec->collision_info.rotate_omega, .0f, .0f, .0f);
    BrVector3SetFloat(&pDrone_spec->collision_info.velocity_car_space, .0f, .0f, .0f);
#else
    __asm {
            sub esp, 0x24
            push ebx
            mov ebx, ecx
            push ebp
            push esi
            mov eax, dword ptr [ebx + 0xec]
            push edi
            lea ecx, [esp + 0x1c]
            push eax
            push ecx
            lea ebp, [ebx + 0xf8]
            call BrActorToBounds
            fld dword ptr [esp + 0x28]
            fsub dword ptr [gDrone_select_max_distance_squared+0x74]
            mov al, byte ptr [ebx + 0x46]
            add esp, 8
            test al, al
            fstp dword ptr [esp + 0x20]
            je L0044f9bd
            mov edx, dword ptr [ebp + 0x20]
            jmp L0044f9c1
    L0044f9bd:
            mov edx, dword ptr [esp + 0x18]
    L0044f9c1:
            mov ecx, 0x136
            xor eax, eax
            mov edi, ebp
            rep stosd
            mov al, byte ptr [ebx + 0x46]
            test al, al
            je L0044f9da
            mov dword ptr [ebp + 0x20], edx
            xor edi, edi
            jmp L0044fa12
    L0044f9da:
            mov ecx, 0x8b
            call AllocateBoxCollisionShape
            lea edi, [eax + 4]
            mov ecx, 6
            lea esi, [esp + 0x1c]
            mov dword ptr [ebp + 0x20], eax
            rep movsd
            mov edi, dword ptr [ebp + 0x20]
            mov ecx, 6
            add edi, 0x1c
            lea esi, [esp + 0x1c]
            rep movsd
            mov edx, dword ptr [ebp + 0x20]
            xor edi, edi
            mov dword ptr [edx + 0x34], edi
            mov byte ptr [ebx + 0x46], 1
    L0044fa12:
            mov byte ptr [ebp + 0x238], 0x10
            mov dword ptr [ebp + 0x23c], ebx
            mov eax, dword ptr [ebx + 0xec]
            lea esi, [ebp + 0x8c]
            mov dword ptr [ebp], eax
            mov ecx, dword ptr [ebx + 4]
            mov edx, dword ptr [ecx + 0x14]
            mov dword ptr [ebp + 0x14], edi
            mov dword ptr [ebp + 4], edx
            mov dword ptr [ebp + 0x18], 0x3db851ec
            mov dword ptr [ebp + 0x1c], edi
            mov dword ptr [ebp + 0x54], edi
            mov dword ptr [ebp + 0x58], 0xbd6d7304
            mov dword ptr [ebp + 0x5c], edi
            mov eax, dword ptr [ebx + 0xec]
            add eax, 0x2c
            push eax
            push esi
            call BrMatrix34Copy
            mov edx, dword ptr [gFace_num__car]
            mov ecx, dword ptr [ebp + 0x20]
            add edx, -2
            add esp, 8
            mov word ptr [ebp + 0x178], dx
            mov dword ptr [ebp + 0x1a0], 0x100000
            call FillInShape
            mov ecx, ebp
            call UpdateCollisionObject
            lea edx, [ebp + 0x24]
            lea ecx, [ebp + 0xf4]
            push esi
            call GetNewBoundingBox
            fld dword ptr [esp + 0x2c]
            fld dword ptr [esp + 0x30]
            fld dword ptr [esp + 0x28]
            fxch st(2)
            fsub dword ptr [esp + 0x20]
            fxch st(1)
            fsub dword ptr [esp + 0x24]
            fxch st(2)
            fsub dword ptr [esp + 0x1c]
            fld dword ptr [ebp + 4]
            fld st(2)
            fld st(4)
            fxch st(1)
            fmul st(0), st(4)
            fxch st(3)
            fst dword ptr [esp + 0x10]
            fxch st(1)
            fmul st(0), st(5)
            fxch st(3)
            fstp dword ptr [esp + 0x18]
            fmul dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x18]
            fxch st(3)
            fst dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x14]
            fadd dword ptr [esp + 0x18]
            fxch st(2)
            fstp dword ptr [esp + 0x18]
            fxch st(3)
            fadd dword ptr [esp + 0x18]
            fxch st(3)
            fadd dword ptr [esp + 0x18]
            fxch st(2)
            fmulp st(3), st(0)
            fxch st(2)
            fmul dword ptr [gDrone_select_max_distance_squared+0x78]
            fxch st(1)
            fmul dword ptr [ebp + 4]
            fxch st(1)
            fstp dword ptr [ebp + 0x10]
            fmul dword ptr [gDrone_select_max_distance_squared+0x78]
            fstp dword ptr [ebp + 0xc]
            fmul dword ptr [ebp + 4]
            fmul dword ptr [gDrone_select_max_distance_squared+0x78]
            fstp dword ptr [ebp + 8]
            mov eax, dword ptr [ebx + 4]
            fld dword ptr [esp + 0x1c]
            fld dword ptr [eax + 0x34]
            fmul dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x20]
            fld dword ptr [esp + 0x24]
            fxch st(2)
            fmul qword ptr [gDrone_select_max_distance_squared+0x7c]
            fsubp st(3), st(0)
            fxch st(2)
            fstp dword ptr [ebp + 0x14]
            mov ecx, dword ptr [ebx + 4]
            fld dword ptr [ecx + 0x38]
            fmul st(0), st(3)
            fmul qword ptr [gDrone_select_max_distance_squared+0x7c]
            fsubp st(2), st(0)
            fxch st(1)
            fstp dword ptr [ebp + 0x18]
            mov edx, dword ptr [ebx + 4]
            fld dword ptr [edx + 0x30]
            fmul st(0), st(3)
            fmul qword ptr [gDrone_select_max_distance_squared+0x7c]
            fsubp st(1), st(0)
            fstp dword ptr [ebp + 0x1c]
            mov eax, dword ptr [ebx + 4]
            fxch st(1)
            fstp st(0)
            cmp dword ptr [eax + 0x10], 2
            fstp st(0)
            jne L0044fb8b
            mov dword ptr [ebp + 0x60], 0xbf400000
    L0044fb8b:
            mov eax, dword ptr [ebp + 0x19c]
            mov byte ptr [ebp + 0xec], 1
            or al, 0x40
            mov byte ptr [ebp + 0xed], 1
            mov dword ptr [ebp + 0x19c], eax
            mov ecx, dword ptr [ebx + 4]
            push edi
            push edi
            mov edx, dword ptr [ecx + 0x2c]
            lea eax, [ebp + 0x68]
            shr edx, 3
            and dl, 1
            push edi
            push eax
            mov byte ptr [ebp + 0x4d4], dl
            call BrVector3SetFloat
            add esp, 0x10
            lea ecx, [ebp + 0x74]
            push edi
            push edi
            push edi
            push ecx
            call BrVector3SetFloat
            add esp, 0x10
            lea edx, [ebp + 0x1c0]
            push edi
            push edi
            push edi
            push edx
            call BrVector3SetFloat
            add esp, 0x10
            add ebp, 0x1a8
            push edi
            push edi
            push edi
            push ebp
            call BrVector3SetFloat
            add esp, 0x10
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x24
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
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00451620
void C2_HOOK_FASTCALL CrappyLittleVector3DPrintf(const char* pMessage, br_vector3* pPosition) {
    DoNotDprintf("%d: %s: %3.3f, %3.3f, %3.3f", gFrame, pMessage, pPosition->v[0], pPosition->v[1], pPosition->v[2]);
}

// FUNCTION: CARMA2_HW 0x004518e0
int C2_NAKED C2_HOOK_FASTCALL CheckDroneInSensiblePlaceBeforeStartingToProcessTheCuntingThing(tDrone_spec* pDrone) {
#ifndef CARPOCALYPSE2_MATCHING

    if (gTime_stamp_for_this_munging > pDrone->last_collide_check + 1000) {
        pDrone->last_collide_check = gTime_stamp_for_this_munging;
        InitDroneCollisionObject(pDrone);

        return !DroneHasCollided(pDrone);
    } else {
        return 0;
    }
#else
    __asm {
            sub esp, 0x24
            mov eax, dword ptr [gTime_stamp_for_this_munging]
            push ebx
            mov ebx, ecx
            mov ecx, dword ptr [ebx + 0x34]
            add ecx, 0x3e8
            cmp eax, ecx
            jbe L00451bbb
            mov edx, dword ptr [ebx + 0xec]
            push edi
            push esi
            mov dword ptr [ebx + 0x34], eax
            push ebp
            lea eax, [esp + 0x1c]
            push edx
            push eax
            lea ebp, [ebx + 0xf8]
            call BrActorToBounds
            fld dword ptr [esp + 0x28]
            fsub dword ptr [gDrone_select_max_distance_squared+0x74]
            mov al, byte ptr [ebx + 0x46]
            add esp, 8
            test al, al
            fstp dword ptr [esp + 0x20]
            je L00451936
            mov edx, dword ptr [ebp + 0x20]
            jmp L0045193a
    L00451936:
            mov edx, dword ptr [esp + 0x18]
    L0045193a:
            mov ecx, 0x136
            xor eax, eax
            mov edi, ebp
            rep stosd
            mov al, byte ptr [ebx + 0x46]
            test al, al
            je L00451953
            mov dword ptr [ebp + 0x20], edx
            xor edi, edi
            jmp L0045198b
    L00451953:
            mov ecx, 0x8b
            call AllocateBoxCollisionShape
            lea edi, [eax + 4]
            mov ecx, 6
            lea esi, [esp + 0x1c]
            mov dword ptr [ebp + 0x20], eax
            rep movsd
            mov edi, dword ptr [ebp + 0x20]
            mov ecx, 6
            add edi, 0x1c
            lea esi, [esp + 0x1c]
            rep movsd
            mov ecx, dword ptr [ebp + 0x20]
            xor edi, edi
            mov dword ptr [ecx + 0x34], edi
            mov byte ptr [ebx + 0x46], 1
    L0045198b:
            mov byte ptr [ebp + 0x238], 0x10
            mov dword ptr [ebp + 0x23c], ebx
            mov edx, dword ptr [ebx + 0xec]
            lea esi, [ebp + 0x8c]
            mov dword ptr [ebp], edx
            mov eax, dword ptr [ebx + 4]
            mov ecx, dword ptr [eax + 0x14]
            mov dword ptr [ebp + 0x14], edi
            mov dword ptr [ebp + 4], ecx
            mov dword ptr [ebp + 0x18], 0x3db851ec
            mov dword ptr [ebp + 0x1c], edi
            mov dword ptr [ebp + 0x54], edi
            mov dword ptr [ebp + 0x58], 0xbd6d7304
            mov dword ptr [ebp + 0x5c], edi
            mov edx, dword ptr [ebx + 0xec]
            add edx, 0x2c
            push edx
            push esi
            call BrMatrix34Copy
            mov eax, dword ptr [gFace_num__car]
            mov ecx, dword ptr [ebp + 0x20]
            add eax, -2
            add esp, 8
            mov word ptr [ebp + 0x178], ax
            mov dword ptr [ebp + 0x1a0], 0x100000
            call FillInShape
            mov ecx, ebp
            call UpdateCollisionObject
            lea edx, [ebp + 0x24]
            lea ecx, [ebp + 0xf4]
            push esi
            call GetNewBoundingBox
            fld dword ptr [esp + 0x2c]
            fld dword ptr [esp + 0x30]
            fld dword ptr [esp + 0x28]
            fxch st(2)
            fsub dword ptr [esp + 0x20]
            fxch st(1)
            fsub dword ptr [esp + 0x24]
            fxch st(2)
            fsub dword ptr [esp + 0x1c]
            fld dword ptr [ebp + 4]
            fld st(2)
            fld st(4)
            fxch st(1)
            fmul st(0), st(4)
            fxch st(3)
            fst dword ptr [esp + 0x10]
            fxch st(1)
            fmul st(0), st(5)
            fxch st(3)
            fstp dword ptr [esp + 0x18]
            fmul dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x18]
            fxch st(3)
            fst dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x14]
            fadd dword ptr [esp + 0x18]
            fxch st(2)
            fstp dword ptr [esp + 0x18]
            fxch st(3)
            fadd dword ptr [esp + 0x18]
            fxch st(3)
            fadd dword ptr [esp + 0x18]
            fxch st(2)
            fmulp st(3), st(0)
            fxch st(2)
            fmul dword ptr [gDrone_select_max_distance_squared+0x78]
            fxch st(1)
            fmul dword ptr [ebp + 4]
            fxch st(1)
            fstp dword ptr [ebp + 0x10]
            fmul dword ptr [gDrone_select_max_distance_squared+0x78]
            fstp dword ptr [ebp + 0xc]
            fmul dword ptr [ebp + 4]
            fmul dword ptr [gDrone_select_max_distance_squared+0x78]
            fstp dword ptr [ebp + 8]
            mov ecx, dword ptr [ebx + 4]
            fld dword ptr [esp + 0x1c]
            fld dword ptr [ecx + 0x34]
            fmul dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x20]
            fld dword ptr [esp + 0x24]
            fxch st(2)
            fmul qword ptr [gDrone_select_max_distance_squared+0x7c]
            fsubp st(3), st(0)
            fxch st(2)
            fstp dword ptr [ebp + 0x14]
            mov edx, dword ptr [ebx + 4]
            fld dword ptr [edx + 0x38]
            fmul st(0), st(3)
            fmul qword ptr [gDrone_select_max_distance_squared+0x7c]
            fsubp st(2), st(0)
            fxch st(1)
            fstp dword ptr [ebp + 0x18]
            mov eax, dword ptr [ebx + 4]
            fld dword ptr [eax + 0x30]
            fmul st(0), st(3)
            fmul qword ptr [gDrone_select_max_distance_squared+0x7c]
            fsubp st(1), st(0)
            fstp dword ptr [ebp + 0x1c]
            mov ecx, dword ptr [ebx + 4]
            fxch st(1)
            fstp st(0)
            cmp dword ptr [ecx + 0x10], 2
            fstp st(0)
            jne L00451b03
            mov dword ptr [ebp + 0x60], 0xbf400000
    L00451b03:
            mov eax, dword ptr [ebp + 0x19c]
            mov byte ptr [ebp + 0xec], 1
            or al, 0x40
            mov byte ptr [ebp + 0xed], 1
            mov dword ptr [ebp + 0x19c], eax
            mov edx, dword ptr [ebx + 4]
            push edi
            push edi
            mov eax, dword ptr [edx + 0x2c]
            lea ecx, [ebp + 0x68]
            shr eax, 3
            and al, 1
            push edi
            push ecx
            mov byte ptr [ebp + 0x4d4], al
            call BrVector3SetFloat
            add esp, 0x10
            lea edx, [ebp + 0x74]
            push edi
            push edi
            push edi
            push edx
            call BrVector3SetFloat
            add esp, 0x10
            lea eax, [ebp + 0x1c0]
            push edi
            push edi
            push edi
            push eax
            call BrVector3SetFloat
            add esp, 0x10
            lea ecx, [ebp + 0x1a8]
            push edi
            push edi
            push edi
            push ecx
            call BrVector3SetFloat
            mov esi, dword ptr [gList_collision_infos]
            add esp, 0x10
            cmp esi, edi
            je L00451bae
    L00451b7c:
            cmp ebp, esi
            je L00451b8d
            mov edx, esi
            mov ecx, ebp
            call TestObjectOverlap
            test eax, eax
            jne L00451ba4
    L00451b8d:
            mov esi, dword ptr [esi + 0x220]
            cmp esi, edi
            jne L00451b7c
            pop ebp
            pop esi
            mov eax, 1
            pop edi
            pop ebx
            add esp, 0x24
            ret
    L00451ba4:
            pop ebp
            pop esi
            xor eax, eax
            pop edi
            pop ebx
            add esp, 0x24
            ret
    L00451bae:
            pop ebp
            pop esi
            mov eax, 1
            pop edi
            pop ebx
            add esp, 0x24
            ret
    L00451bbb:
            xor eax, eax
            pop ebx
            add esp, 0x24
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00451650
int C2_HOOK_FASTCALL ReallyAddDroneToPHIL(tDrone_spec* pDrone) {
    if (PHILAddObject(&pDrone->collision_info)) {
        return 0;
    }
    PHILSetObjectProperty(&pDrone->collision_info, 0, 1.0);
    PHILSetObjectProperty(&pDrone->collision_info, 3, 1);
    PHILSetObjectProperty(&pDrone->collision_info, 7, 1);
    PHILSetObjectProperty(&pDrone->collision_info, 6, 1);
    return 1;
}

int C2_HOOK_FASTCALL AddDroneToPHIL(tDrone_spec* pDrone) {

    return ReallyAddDroneToPHIL(pDrone);
}

void C2_HOOK_FASTCALL UnPauseDroneState(tDrone_spec* pDrone) {

    if (gDrone_state_functions[pDrone->current_state] != NULL) {
        gDrone_state_functions[pDrone->current_state](pDrone, eDrone_state_DEFAULT);
    }
}

void C2_HOOK_FASTCALL StartProcessingThisDrone(tDrone_spec* pDrone) {
    tDrone_form* form = pDrone->form;

    if (!gShow_drone_paths && pDrone->field_0x44 == 0 && ((form->flags & 0x4) != 0 || gCount_active_drones < 12))  {

        if (CheckDroneInSensiblePlaceBeforeStartingToProcessTheCuntingThing(pDrone) && AddDroneToPHIL(pDrone)) {

            BrMatrix34Copy(&pDrone->collision_info.transform_matrix, &pDrone->actor->t.t.mat);
            pDrone->field_0x44 = 1;
            if ((pDrone->form->flags & 0x4) == 0) {
                gCount_active_drones += 1;
            }
            UnPauseDroneState(pDrone);
            DoNotDprintf("PROCESSING ON: Frame %d, Drone %d, state %d", gFrame, pDrone->id, pDrone->current_state);
            CrappyLittleVector3DPrintf("    Pos", &pDrone->actor->t.t.translate.t);
        }
    }
}

// FUNCTION: CARMA2_HW 0x0044f700
void C2_NAKED C2_HOOK_FASTCALL InitDrones(tRace_info* pRace_info) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;

    (void)pRace_info;
    C2_HOOK_BUG_ON(sizeof(tDrone_spec) != 1496);

    gDrones_unmodified = 1;
    gCount_rendered_drones = 0;
    gCurrent_selected_drone = 0;
    gINT_006820d4 = 0;
    gINT_006844fc = 0;
    gINT_00681fb0 = 0;

    for (i = 0; i < gCount_drones; i++) {
        tDrone_spec* drone = &gDrone_specs[i];
        tDrone_form* form = drone->form;

        NewDroneState(drone, 1);
        if (form->type == kDroneType_plane) {
            StartProcessingThisDrone(drone);
        }
    }
#else
    __asm {
            push ebp
            mov ebp, esp
            and esp, 0xfffffff8
            sub esp, 8
            mov eax, dword ptr [gCount_drones]
            push ebx
            push ebp
            push esi
            push edi
            xor ebx, ebx
            mov edi, 1
            cmp eax, ebx
            mov dword ptr [gDrones_unmodified], edi
            mov dword ptr [gCount_rendered_drones], ebx
            mov dword ptr [gCurrent_selected_drone], ebx
            mov dword ptr [gINT_006820d4], ebx
            mov dword ptr [gINT_006844fc], ebx
            mov dword ptr [gINT_00681fb0], ebx
            mov dword ptr [esp + 0x10], ebx
            jle L0044f96b
            xor ebp, ebp
            mov dword ptr [esp + 0x14], ebp
            jmp L0044f756
    L0044f751:
            mov edi, 1
    L0044f756:
            mov eax, dword ptr [gDrone_specs]
            push edi
            mov ecx, dword ptr [eax + ebp + 0x38]
            mov edx, dword ptr [eax + ebp]
            lea esi, [eax + ebp]
            push ecx
            push edx
            push offset gDefault_drone_direction+0x138
            call DoNotDprintf
            mov eax, dword ptr [esi + 0x38]
            add esp, 0x10
            cmp eax, edi
            je L0044f7b6
            mov eax, dword ptr [eax*4 + gDrone_state_functions]
            cmp eax, ebx
            je L0044f790
            mov edx, 4
            mov ecx, esi
            call eax
    L0044f790:
            mov eax, dword ptr [gTime_stamp_for_this_munging]
            mov ecx, dword ptr [esi + 0x3c]
            mov edx, dword ptr [esi + 0x38]
            mov dword ptr [esi + 0x30], eax
            mov dword ptr [esi + 0x40], ecx
            mov dword ptr [esi + 0x3c], edx
            mov dword ptr [esi + 0x38], edi
            mov eax, dword ptr [gDrone_state_functions+0x4]
            cmp eax, ebx
            je L0044f7b6
            xor edx, edx
            mov ecx, esi
            call eax
    L0044f7b6:
            mov eax, dword ptr [gDrone_specs]
            mov ecx, dword ptr [eax + ebp + 4]
            lea esi, [eax + ebp]
            cmp dword ptr [ecx + 0x10], edi
            jne L0044f94a
            cmp dword ptr [gShow_drone_paths], ebx
            jne L0044f94a
            cmp byte ptr [esi + 0x44], bl
            jne L0044f94a
            mov edx, ecx
            test byte ptr [edx + 0x2c], 4
            jne L0044f7f5
            cmp dword ptr [gCount_active_drones], 0xc
            jge L0044f94a
    L0044f7f5:
            mov ecx, dword ptr [esi + 0x34]
            mov eax, dword ptr [gTime_stamp_for_this_munging]
            add ecx, 0x3e8
            cmp eax, ecx
            jbe L0044f85a
            mov ecx, esi
            mov dword ptr [esi + 0x34], eax
            call InitDroneCollisionObject
            mov edi, dword ptr [gList_collision_infos]
            lea ebp, [esi + 0xf8]
            cmp edi, ebx
            je L0044f84f
    L0044f821:
            cmp ebp, edi
            je L0044f832
            mov edx, edi
            mov ecx, ebp
            call TestObjectOverlap
            test eax, eax
            jne L0044f847
    L0044f832:
            mov edi, dword ptr [edi + 0x220]
            cmp edi, ebx
            jne L0044f821
            mov ebp, dword ptr [esp + 0x14]
            mov eax, 1
            jmp L0044f85c
    L0044f847:
            mov ebp, dword ptr [esp + 0x14]
            xor eax, eax
            jmp L0044f85c
    L0044f84f:
            mov ebp, dword ptr [esp + 0x14]
            mov eax, 1
            jmp L0044f85c
    L0044f85a:
            xor eax, eax
    L0044f85c:
            cmp eax, ebx
            je L0044f94a
            lea edi, [esi + 0xf8]
            mov ecx, edi
            call PHILAddObject
            cmp eax, ebx
            jne L0044f94a
            push 0x3ff00000
            push ebx
            push ebx
            push edi
            call PHILSetObjectProperty
            add esp, 0x10
            push 1
            push 3
            push edi
            call PHILSetObjectProperty
            add esp, 0xc
            push 1
            push 7
            push edi
            call PHILSetObjectProperty
            add esp, 0xc
            push 1
            push 6
            push edi
            call PHILSetObjectProperty
            mov edx, dword ptr [esi + 0xec]
            add esp, 0xc
            add edx, 0x2c
            lea eax, [esi + 0x184]
            push edx
            push eax
            call BrMatrix34Copy
            mov ecx, dword ptr [esi + 4]
            mov byte ptr [esi + 0x44], 1
            add esp, 8
            test byte ptr [ecx + 0x2c], 4
            jne L0044f8dc
            inc dword ptr [gCount_active_drones]
    L0044f8dc:
            mov edx, dword ptr [esi + 0x38]
            mov eax, dword ptr [edx*4 + gDrone_state_functions]
            cmp eax, ebx
            je L0044f8f3
            mov edx, 3
            mov ecx, esi
            call eax
    L0044f8f3:
            mov eax, dword ptr [esi + 0x38]
            mov ecx, dword ptr [esi]
            mov edx, dword ptr [gFrame]
            push eax
            push ecx
            push edx
            push offset gDefault_drone_direction+0x608
            call DoNotDprintf
            mov esi, dword ptr [esi + 0xec]
            add esp, 0x10
            add esi, 0x50
            sub esp, 8
            sub esp, 8
            mov eax, dword ptr [gFrame]
            fld dword ptr [esi + 8]
            fstp qword ptr [esp + 8]
            fld dword ptr [esi + 4]
            _emit 0xdd
        _emit 0x1c
        _emit 0x24
            _emit 0xd9
        _emit 0x06
            sub esp, 8
            _emit 0xdd
        _emit 0x1c
        _emit 0x24
            push offset gDefault_drone_direction+0x600
            push eax
            push offset gDefault_drone_direction+0xb0
            call DoNotDprintf
            add esp, 0x24
    L0044f94a:
            mov eax, dword ptr [esp + 0x10]
            mov ecx, dword ptr [gCount_drones]
            inc eax
            add ebp, 0x5d8
            cmp eax, ecx
            mov dword ptr [esp + 0x10], eax
            mov dword ptr [esp + 0x14], ebp
            jl L0044f751
    L0044f96b:
            pop edi
            pop esi
            pop ebp
            pop ebx
            mov esp, ebp
            pop ebp
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
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0044fc10
void C2_HOOK_FASTCALL DisposeDronesRaceStuff(void) {
    int i;

    if (gCount_drones == 0) {
        return;
    }

    for (i = 0; i < gCount_drones; i++) {

        if (gDrone_specs[i].actor->parent != NULL) {
            BrActorRemove(gDrone_specs[i].actor);
        }
        BrActorFree(gDrone_specs[i].actor);
        RemoveDroneFromPHIL(&gDrone_specs[i]);
        if (gDrone_specs[i].field_0x46) {
            FreeShape(gDrone_specs[i].collision_info.shape);
            gDrone_specs[i].field_0x46 = 0;
        }

        if (gDrone_specs[i].funk_grooves != NULL) {
            BrMemFree(gDrone_specs[i].funk_grooves);
            gDrone_specs[i].funk_grooves = NULL;
        }
    }

    for (i = 0; i < gCount_drone_forms; i++) {

        if (gDrone_forms[i].field_0x80 != NULL) {
            int j;

            BrMemFree(gDrone_forms[i].field_0x80);
            gDrone_forms[i].field_0x80 = NULL;
            for (j = 0; j < gDrone_forms[i].count_models; j++) {
                BrModelRemove(gDrone_forms[i].models[j]);
                gDrone_forms[i].models[j]->nfaces = 0;
                gDrone_forms[i].models[j]->faces = NULL;
                BrModelFree(gDrone_forms[i].models[j]);
            }
            if (gDrone_forms[i].models != NULL) {
                BrMemFree(gDrone_forms[i].models);
                gDrone_forms[i].models = NULL;
            }
        }
    }
    ClearOutStorageSpace(&gDroneStorage);
    BrMemFree(gDrone_specs);
    gDrone_specs = NULL;
    gCount_drones = 0;
    BrMemFree(gDrone_path_nodes);
    gDrone_path_nodes = NULL;
    gCount_drone_path_nodes = 0;
}

// FUNCTION: CARMA2_HW 0x004020e0
void C2_NAKED C2_HOOK_FASTCALL MakeAISimpleEditSectionHere(br_model* pModel, int pVert_index, int pFace_index, tDrone_path_node* pNode, br_vector3* pPos, br_material* pMaterial1, br_material* pMaterial2) {
#ifndef CARPOCALYPSE2_MATCHING

    NOT_IMPLEMENTED();
#else
    __asm {
            sub esp, 0x18
            mov eax, dword ptr [esp + 0x20]
            push ebx
            push ebp
            push esi
            fld dword ptr [eax]
            fld dword ptr [eax + 8]
            mov esi, dword ptr [esp + 0x30]
            push edi
            fst dword ptr [esp + 0x24]
            fld dword ptr [eax + 4]
            fsub dword ptr [gFLOAT_00589318]
            mov edi, dword ptr [esi]
            lea eax, [edx + edx*4]
            mov dword ptr [esp + 0x10], edi
            mov edi, dword ptr [esi + 8]
            shl eax, 3
            fst dword ptr [esp + 0x20]
            fld dword ptr [esi + 4]
            fsub dword ptr [gFLOAT_00589318]
            mov esi, dword ptr [ecx + 8]
            mov dword ptr [esp + 0x18], edi
            fstp dword ptr [esp + 0x14]
            fld st(2)
            fstp dword ptr [eax + esi]
            mov esi, dword ptr [ecx + 8]
            fstp dword ptr [eax + esi + 4]
            mov esi, dword ptr [ecx + 8]
            fstp dword ptr [eax + esi + 8]
            mov esi, dword ptr [ecx + 8]
            add esi, eax
            mov edi, dword ptr [esi]
            mov dword ptr [esi], edi
            mov esi, dword ptr [ecx + 8]
            fld dword ptr [eax + esi + 4]
            fsub dword ptr [gFLOAT_0058931c]
            lea esi, [eax + esi + 4]
            fstp dword ptr [esi]
            mov esi, dword ptr [ecx + 8]
            fld dword ptr [esp + 0x10]
            lea eax, [eax + esi + 8]
            mov esi, dword ptr [eax]
            mov dword ptr [eax], esi
            mov esi, dword ptr [ecx + 8]
            lea eax, [edx + 1]
            lea eax, [eax + eax*4]
            shl eax, 3
            fstp dword ptr [eax + esi]
            mov esi, dword ptr [ecx + 8]
            fld dword ptr [esp + 0x14]
            fstp dword ptr [eax + esi + 4]
            mov esi, dword ptr [ecx + 8]
            fld dword ptr [esp + 0x18]
            fstp dword ptr [eax + esi + 8]
            mov esi, dword ptr [ecx + 8]
            add esi, eax
            mov edi, dword ptr [esi]
            mov dword ptr [esi], edi
            mov esi, dword ptr [ecx + 8]
            fld dword ptr [eax + esi + 4]
            fsub dword ptr [gFLOAT_0058931c]
            lea esi, [eax + esi + 4]
            fstp dword ptr [esi]
            mov esi, dword ptr [ecx + 8]
            lea eax, [eax + esi + 8]
            mov esi, dword ptr [eax]
            mov dword ptr [eax], esi
            mov esi, dword ptr [ecx + 8]
            lea eax, [edx + 2]
            lea eax, [eax + eax*4]
            shl eax, 3
            fstp dword ptr [eax + esi]
            mov esi, dword ptr [ecx + 8]
            fld dword ptr [esp + 0x20]
            fstp dword ptr [eax + esi + 4]
            mov esi, dword ptr [ecx + 8]
            fld dword ptr [esp + 0x24]
            fstp dword ptr [eax + esi + 8]
            mov esi, dword ptr [ecx + 8]
            add esi, eax
            mov edi, dword ptr [esi]
            mov dword ptr [esi], edi
            mov esi, dword ptr [ecx + 8]
            fld dword ptr [eax + esi + 4]
            fsub dword ptr [gFLOAT_00589320]
            lea esi, [eax + esi + 4]
            fstp dword ptr [esi]
            mov esi, dword ptr [ecx + 8]
            fld dword ptr [esp + 0x10]
            lea eax, [eax + esi + 8]
            mov esi, dword ptr [eax]
            mov dword ptr [eax], esi
            mov esi, dword ptr [ecx + 8]
            lea eax, [edx + 3]
            lea eax, [eax + eax*4]
            shl eax, 3
            fstp dword ptr [eax + esi]
            mov esi, dword ptr [ecx + 8]
            fld dword ptr [esp + 0x14]
            fstp dword ptr [eax + esi + 4]
            mov esi, dword ptr [ecx + 8]
            fld dword ptr [esp + 0x18]
            fstp dword ptr [eax + esi + 8]
            mov esi, dword ptr [ecx + 8]
            add esi, eax
            mov edi, dword ptr [esi]
            mov dword ptr [esi], edi
            mov esi, dword ptr [ecx + 8]
            mov edi, dword ptr [esp + 0x2c]
            fld dword ptr [eax + esi + 4]
            fsub dword ptr [gFLOAT_00589320]
            lea esi, [eax + esi + 4]
            fstp dword ptr [esi]
            mov esi, dword ptr [ecx + 8]
            lea eax, [eax + esi + 8]
            mov esi, dword ptr [eax]
            mov dword ptr [eax], esi
            mov ebx, dword ptr [ecx + 0xc]
            lea eax, [edi + edi*4]
            lea esi, [edx + 1]
            shl eax, 3
            mov word ptr [eax + ebx], si
            mov ebp, dword ptr [ecx + 0xc]
            lea ebx, [edx + 3]
            mov word ptr [eax + ebp + 2], bx
            mov ebp, dword ptr [ecx + 0xc]
            lea ebx, [edx + 2]
            mov dword ptr [esp + 0x30], ebx
            mov word ptr [eax + ebp + 4], bx
            mov ebx, dword ptr [ecx + 0xc]
            mov ebp, dword ptr [esp + 0x38]
            mov word ptr [eax + ebx + 6], 0xffff
            mov ebx, dword ptr [ecx + 0xc]
            mov dword ptr [eax + ebx + 8], ebp
            lea eax, [edi + 1]
            mov edi, dword ptr [ecx + 0xc]
            lea eax, [eax + eax*4]
            shl eax, 3
            mov word ptr [eax + edi], dx
            mov edx, dword ptr [ecx + 0xc]
            pop edi
            mov word ptr [eax + edx + 2], si
            mov edx, dword ptr [ecx + 0xc]
            mov si, word ptr [esp + 0x2c]
            mov word ptr [eax + edx + 4], si
            mov edx, dword ptr [ecx + 0xc]
            pop esi
            pop ebp
            mov word ptr [eax + edx + 6], 0xffff
            mov ecx, dword ptr [ecx + 0xc]
            mov edx, dword ptr [esp + 0x30]
            pop ebx
            mov dword ptr [eax + ecx + 8], edx
            add esp, 0x18
            ret 0x14
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

// FUNCTION: CARMA2_HW 0x00451e70
void C2_NAKED C2_HOOK_FASTCALL DoDroneFunkyGroovyThings(tDrone_spec *pDrone) {
#ifndef CARPOCALYPSE2_MATCHING
    float delta_time;
    float flt_unk0;
    float flt_unk1;
    int left_turn;
    int i;

    if (gAction_replay_mode) {
        delta_time = gFrame_period / 1000.f;
    } else {
        delta_time = gDrone_delta_time;
    }
    if (gAction_replay_mode) {
        flt_unk0 = pDrone->field_0xe0;
        flt_unk1 = pDrone->field_0xe4;
        if (flt_unk1 < 0.f) {
            flt_unk1 = -flt_unk1;
            left_turn = 0;
        } else {
            left_turn = 1;
        }
    } else {
        flt_unk0 = pDrone->field_0x74;
        flt_unk1 = pDrone->field_0x48;
        left_turn = pDrone->left_turn;
    }
    if (pDrone->funk_grooves == NULL) {
        return;
    }
    if (!gAction_replay_mode && (pDrone->current_state == 3 || pDrone->current_state == 4)) {
        return;
    }
    for (i = 0; i < pDrone->funk_grooves->count; i++) {
        tFunk_groove* groove = &pDrone->funk_grooves->items[i];

        switch (groove->type) {
        case 0: /* steering */
            {
                float angle;
                if (groove->spinny.speed_control) {
                    angle = delta_time * flt_unk1 / groove->spinny.omega;
                } else {
                    angle = 0.f;
                }
                if (groove->spinny.reverse) {
                    angle = -angle;
                }
                switch (groove->spinny.axis) {
                case 0:
                    BrMatrix34PreRotateX(&groove->actor->t.t.mat, (br_angle)(angle * 65536.f));
                    break;
                case 1:
                    BrMatrix34PreRotateY(&groove->actor->t.t.mat, (br_angle)(angle * 65536.f));
                    break;
                case 2:
                    BrMatrix34PreRotateZ(&groove->actor->t.t.mat, (br_angle)(angle * 65536.f));
                    break;
                }
            }
            break;
        case 1: /* spinning */
            {
                float f;
                float factor;
                float angle;
                if (gAction_replay_mode) {
                    if (pDrone->field_0xe8 < 0) {
                        f = 0;
                    } else {
                        f = pDrone->field_0xe8;
                    }
                    if (f > 10.f) {
                        f = 0.f;
                    } else if (pDrone->field_0xe8 < 0.f) {
                        f = 1.f / WORLD_SCALE;
                    } else {
                        f = (10.f - pDrone->field_0xe8) / WORLD_SCALE / 10.f;
                    }
                } else {
                    if (pDrone->field_0xdc == 2) {
                        if (pDrone->h_radius < 0.f) {
                            f = 0.f;
                        } else {
                            f = pDrone->h_radius;
                        }
                        if (f > 10.f) {
                            f = 0.f;
                        } else if (pDrone->h_radius < 0.f) {
                            f = 1.f / WORLD_SCALE;
                        } else {
                            f = (10.f - pDrone->h_radius) / WORLD_SCALE / 10.f;
                        }
                    } else {
                        f = 0.0;
                    }
                }
                if (flt_unk0 < .5f) {
                    factor = flt_unk0;
                } else {
                    factor = 1.f - flt_unk0;
                }
                angle = 2 * factor * f;
                if (groove->steering.reverse) {
                    angle = -angle;
                }
                if (left_turn) {
                    angle = -angle;
                }
                BrVector3Set((br_vector3*)groove->actor->t.t.mat.m[0], 1.f, 0.f, 0.f);
                BrVector3Set((br_vector3*)groove->actor->t.t.mat.m[1], 0.f, 1.f, 0.f);
                BrVector3Set((br_vector3*)groove->actor->t.t.mat.m[2], 0.f, 0.f, 1.f);
                BrMatrix34PreRotateY(&groove->actor->t.t.mat, (br_angle)(angle * 65536.f));
                if (pDrone->field_0xdc == 2) {
                    DoNotDprintf("STEERING GROOVE: rot %f, h_radius %f, reverse %d, left_turn %d",
                        angle, pDrone->h_radius, groove->steering.reverse, pDrone->left_turn);
                }
            }
            break;
        }
    }
#else
    __asm {
            push ebp
            mov ebp, esp
            and esp, 0xfffffff8
            sub esp, 0x20
            push ebx
            push ebp
            push esi
            mov ebx, ecx
            mov ecx, dword ptr [gPed_676914]
            push edi
            xor edi, edi
            cmp ecx, edi
            je L00451ea4
            mov eax, dword ptr [gFrame_period]
            mov dword ptr [esp + 0x2c], edi
            mov dword ptr [esp + 0x28], eax
            fild qword ptr [esp + 0x28]
            fmul qword ptr [gDrone_select_max_distance_squared+0x94]
            jmp L00451eaa
    L00451ea4:
            fld dword ptr [gDrone_delta_time]
    L00451eaa:
            fstp dword ptr [esp + 0x28]
            cmp ecx, edi
            je L00451ebe
            mov edx, dword ptr [ebx + 0xe0]
            mov dword ptr [esp + 0x1c], edx
            jmp L00451ec5
    L00451ebe:
            mov eax, dword ptr [ebx + 0x74]
            mov dword ptr [esp + 0x1c], eax
    L00451ec5:
            cmp ecx, edi
            je L00451ed5
            mov edx, dword ptr [ebx + 0xe4]
            mov dword ptr [esp + 0x18], edx
            jmp L00451edc
    L00451ed5:
            mov eax, dword ptr [ebx + 0x48]
            mov dword ptr [esp + 0x18], eax
    L00451edc:
            cmp ecx, edi
            je L00451f0b
            fld dword ptr [esp + 0x18]
            fcomp dword ptr [gDrone_render_zero]
            fnstsw ax
            test ah, 1
            je L00451f01
            fld dword ptr [esp + 0x18]
            fchs
            fstp dword ptr [esp + 0x18]
            mov dword ptr [esp + 0x20], edi
            jmp L00451f17
    L00451f01:
            mov dword ptr [esp + 0x20], 1
            jmp L00451f17
    L00451f0b:
            xor edx, edx
            mov dl, byte ptr [ebx + 0xd8]
            mov dword ptr [esp + 0x20], edx
    L00451f17:
            mov esi, dword ptr [ebx + 0x5d0]
            cmp esi, edi
            je L0045221f
            cmp ecx, edi
            jne L00451f3e
            mov eax, dword ptr [ebx + 0x38]
            cmp eax, 3
            je L0045221f
            cmp eax, 4
            je L0045221f
    L00451f3e:
            mov eax, dword ptr [esi]
            mov dword ptr [esp + 0x24], edi
            cmp eax, edi
            jle L0045221f
            jmp L00451f54
    L00451f4e:
            mov ecx, dword ptr [gPed_676914]
    L00451f54:
            xor eax, eax
            mov al, byte ptr [edi + esi + 4]
            sub eax, 0
            je L00452177
            dec eax
            jne L00452203
            test ecx, ecx
            je L00452000
            fld dword ptr [ebx + 0xe8]
            fld dword ptr [gDrone_render_zero]
            fcompp
            fnstsw ax
            test ah, 0x41
            jne L00451f8f
            fld dword ptr [gDrone_render_zero]
            jmp L00451f95
    L00451f8f:
            fld dword ptr [ebx + 0xe8]
    L00451f95:
            fld dword ptr [gDrone_select_max_distance_squared+0x9c]
            fcomp st(1)
            fnstsw ax
            test ah, 1
            fstp st(0)
            je L00451fbd
            fld dword ptr [gDrone_select_max_distance_squared+0x9c]
            fsubr dword ptr [gDrone_select_max_distance_squared+0x9c]
            fmul dword ptr [gDrone_select_max_distance_squared+0xa0]
            jmp L00452088
    L00451fbd:
            fld dword ptr [ebx + 0xe8]
            fld dword ptr [gDrone_render_zero]
            fcompp
            fnstsw ax
            test ah, 0x41
            jne L00451fe9
            fld dword ptr [gDrone_render_zero]
            fsubr dword ptr [gDrone_select_max_distance_squared+0x9c]
            fmul dword ptr [gDrone_select_max_distance_squared+0xa0]
            jmp L00452088
    L00451fe9:
            fld dword ptr [ebx + 0xe8]
            fsubr dword ptr [gDrone_select_max_distance_squared+0x9c]
            fmul dword ptr [gDrone_select_max_distance_squared+0xa0]
            jmp L00452088
    L00452000:
            cmp dword ptr [ebx + 0xdc], 2
            jne L00452082
            fld dword ptr [ebx + 0x70]
            fld dword ptr [gDrone_render_zero]
            fcompp
            fnstsw ax
            test ah, 0x41
            jne L00452023
            fld dword ptr [gDrone_render_zero]
            jmp L00452026
    L00452023:
            fld dword ptr [ebx + 0x70]
    L00452026:
            fld dword ptr [gDrone_select_max_distance_squared+0x9c]
            fcomp st(1)
            fnstsw ax
            test ah, 1
            fstp st(0)
            je L0045204b
            fld dword ptr [gDrone_select_max_distance_squared+0x9c]
            fsubr dword ptr [gDrone_select_max_distance_squared+0x9c]
            fmul dword ptr [gDrone_select_max_distance_squared+0xa0]
            jmp L00452088
    L0045204b:
            fld dword ptr [ebx + 0x70]
            fld dword ptr [gDrone_render_zero]
            fcompp
            fnstsw ax
            test ah, 0x41
            jne L00452071
            fld dword ptr [gDrone_render_zero]
            fsubr dword ptr [gDrone_select_max_distance_squared+0x9c]
            fmul dword ptr [gDrone_select_max_distance_squared+0xa0]
            jmp L00452088
    L00452071:
            fld dword ptr [ebx + 0x70]
            fsubr dword ptr [gDrone_select_max_distance_squared+0x9c]
            fmul dword ptr [gDrone_select_max_distance_squared+0xa0]
            jmp L00452088
    L00452082:
            fld dword ptr [gDrone_render_zero]
    L00452088:
            fld dword ptr [esp + 0x1c]
            fcomp qword ptr [gDrone_select_max_distance_squared+0xa4]
            fnstsw ax
            test ah, 1
            je L0045209f
            fld dword ptr [esp + 0x1c]
            jmp L004520a9
    L0045209f:
            fld dword ptr [gDrone_render_zero+0x4]
            fsub dword ptr [esp + 0x1c]
    L004520a9:
            fmul st(0), st(1)
            mov al, byte ptr [edi + esi + 0xc]
            lea ebp, [edi + esi + 0xc]
            test al, 1
            fadd st(0), st(0)
            fstp dword ptr [esp + 0x14]
            fstp st(0)
            je L004520c9
            fld dword ptr [esp + 0x14]
            fchs
            fstp dword ptr [esp + 0x14]
    L004520c9:
            mov ecx, dword ptr [esp + 0x20]
            xor eax, eax
            cmp ecx, eax
            je L004520dd
            fld dword ptr [esp + 0x14]
            fchs
            fstp dword ptr [esp + 0x14]
    L004520dd:
            mov edx, dword ptr [edi + esi + 8]
            lea esi, [edi + esi + 8]
            mov ecx, 0x3f800000
            mov dword ptr [edx + 0x2c], ecx
            mov edx, dword ptr [esi]
            fld dword ptr [esp + 0x14]
            mov dword ptr [edx + 0x30], eax
            mov edx, dword ptr [esi]
            fmul dword ptr [gDrone_select_max_distance_squared+0xac]
            mov dword ptr [edx + 0x34], eax
            mov edx, dword ptr [esi]
            mov dword ptr [edx + 0x38], eax
            mov edx, dword ptr [esi]
            mov dword ptr [edx + 0x3c], ecx
            mov edx, dword ptr [esi]
            mov dword ptr [edx + 0x40], eax
            mov edx, dword ptr [esi]
            mov dword ptr [edx + 0x44], eax
            mov edx, dword ptr [esi]
            mov dword ptr [edx + 0x48], eax
            mov eax, dword ptr [esi]
            mov dword ptr [eax + 0x4c], ecx
            call _ftol
            mov ecx, dword ptr [esi]
            push eax
            add ecx, 0x2c
            push ecx
            call BrMatrix34PreRotateY
            mov eax, dword ptr [ebx + 0xdc]
            add esp, 8
            cmp eax, 2
            jne L00452203
            mov eax, dword ptr [ebp]
            xor edx, edx
            mov dl, byte ptr [ebx + 0xd8]
            and eax, 1
            fld dword ptr [ebx + 0x70]
            push edx
            push eax
            sub esp, 8
            _emit 0xdd
        _emit 0x1c
        _emit 0x24
            fld dword ptr [esp + 0x24]
            sub esp, 8
            _emit 0xdd
        _emit 0x1c
        _emit 0x24
            push offset gDefault_drone_direction+0xa04
            call DoNotDprintf
            add esp, 0x1c
            jmp L00452203
    L00452177:
            mov eax, dword ptr [edi + esi + 0x10]
            test al, 4
            je L0045218d
            fld dword ptr [esp + 0x28]
            fmul dword ptr [esp + 0x18]
            fdiv dword ptr [edi + esi + 0xc]
            jmp L00452193
    L0045218d:
            fld dword ptr [gDrone_render_zero]
    L00452193:
            test al, 8
            jne L00452199
            fchs
    L00452199:
            and eax, 3
            sub eax, 0
            je L004521e3
            dec eax
            je L004521c5
            dec eax
            jne L00452201
            fmul dword ptr [gDrone_select_max_distance_squared+0xac]
            call _ftol
            mov ecx, dword ptr [edi + esi + 8]
            push eax
            add ecx, 0x2c
            push ecx
            call BrMatrix34PreRotateZ
            add esp, 8
            jmp L00452203
    L004521c5:
            fmul dword ptr [gDrone_select_max_distance_squared+0xac]
            call _ftol
            mov edx, dword ptr [edi + esi + 8]
            push eax
            add edx, 0x2c
            push edx
            call BrMatrix34PreRotateY
            add esp, 8
            jmp L00452203
    L004521e3:
            fmul dword ptr [gDrone_select_max_distance_squared+0xac]
            call _ftol
            push eax
            mov eax, dword ptr [edi + esi + 8]
            add eax, 0x2c
            push eax
            call BrMatrix34PreRotateX
            add esp, 8
            jmp L00452203
    L00452201:
            fstp st(0)
    L00452203:
            mov esi, dword ptr [ebx + 0x5d0]
            mov eax, dword ptr [esp + 0x24]
            inc eax
            add edi, 0x10
            mov ecx, dword ptr [esi]
            mov dword ptr [esp + 0x24], eax
            cmp eax, ecx
            jl L00451f4e
    L0045221f:
            pop edi
            pop esi
            pop ebp
            pop ebx
            mov esp, ebp
            pop ebp
            ret
    }
#endif
}

void C2_HOOK_FASTCALL CrappyLittleDrivingStateInfoDprintf(void) {
}

int C2_HOOK_FASTCALL DroneOrientationChanged(tDrone_spec* pDrone, float pDot) {
    br_actor* actor;

    actor = pDrone->actor;

    if (BrVector3Dot((br_vector3*)pDrone->collision_info.transform_matrix.m[0], (br_vector3*)actor->t.t.mat.m[0]) < pDot) {
        return 1;
    }
    if (BrVector3Dot((br_vector3*)pDrone->collision_info.transform_matrix.m[1], (br_vector3*)actor->t.t.mat.m[1]) < pDot) {
        return 1;
    }
    if (BrVector3Dot((br_vector3*)pDrone->collision_info.transform_matrix.m[2], (br_vector3*)actor->t.t.mat.m[2]) < pDot) {
        return 1;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00451ca0
void C2_NAKED C2_HOOK_FASTCALL ProcessThisDrone(int pIndex) {
#ifndef CARPOCALYPSE2_MATCHING
    tDrone_spec* drone;

    drone = &gDrone_specs[pIndex];
    BrVector3Copy(&drone->pos, &drone->actor->t.t.translate.t);
    if (gDrone_state_functions[drone->current_state] != NULL) {
        gDrone_state_functions[drone->current_state](drone, eDrone_state_RUN);
    }
    if (drone->field_0x45) {
        DoDroneFunkyGroovyThings(drone);
    }
    BrVector3Sub(&drone->field_0x18, &drone->actor->t.t.translate.t, &drone->pos);
    if (drone->field_0x44 && DroneOrientationChanged(drone, .92f) && BrVector3Length(&drone->field_0x18) > 1.f) {
        BrMatrix34Copy(&drone->collision_info.transform_matrix, &drone->actor->t.t.mat);
        DoNotDprintf("REASSERTING OBJECT MATRIX: v length %f, Frame %d, Drone %d, state %d: ",
            BrVector3Length(&drone->field_0x18), gFrame, drone->id, drone->current_state);
        if (drone->current_state == 2) {
            CrappyLittleDrivingStateInfoDprintf();
        }
    }
    BrVector3InvScale(&drone->field_0x18, &drone->field_0x18, gDrone_delta_time);
#else
    __asm {
            push ecx
            mov eax, ecx
            mov edx, dword ptr [gDrone_specs]
            shl eax, 5
            sub eax, ecx
            push esi
            lea eax, [eax + eax*2]
            lea ecx, [ecx + eax*2]
            mov eax, dword ptr [edx + ecx*8 + 0xec]
            lea esi, [edx + ecx*8]
            mov ecx, dword ptr [eax + 0x50]
            mov dword ptr [esi + 0x24], ecx
            mov edx, dword ptr [eax + 0x54]
            mov ecx, dword ptr [esi + 0x38]
            mov dword ptr [esi + 0x28], edx
            mov eax, dword ptr [eax + 0x58]
            mov dword ptr [esi + 0x2c], eax
            mov eax, dword ptr [ecx*4 + gDrone_state_functions]
            test eax, eax
            je L00451ce8
            mov edx, 1
            mov ecx, esi
            call eax
    L00451ce8:
            mov al, byte ptr [esi + 0x45]
            test al, al
            je L00451cf6
            mov ecx, esi
            call DoDroneFunkyGroovyThings
    L00451cf6:
            mov ecx, dword ptr [esi + 0xec]
            mov al, byte ptr [esi + 0x44]
            test al, al
            fld dword ptr [ecx + 0x50]
            fsub dword ptr [esi + 0x24]
            fstp dword ptr [esi + 0x18]
            fld dword ptr [ecx + 0x54]
            fsub dword ptr [esi + 0x28]
            fstp dword ptr [esi + 0x1c]
            fld dword ptr [ecx + 0x58]
            fsub dword ptr [esi + 0x2c]
            fstp dword ptr [esp + 4]
            mov edx, dword ptr [esp + 4]
            mov dword ptr [esi + 0x20], edx
            je L00451dc6
            fld dword ptr [esi + 0x188]
            fld dword ptr [ecx + 0x30]
            fld dword ptr [esi + 0x18c]
            fld dword ptr [ecx + 0x2c]
            fxch st(2)
            fmulp st(3), st(0)
            fmul dword ptr [ecx + 0x34]
            fxch st(1)
            fmul dword ptr [esi + 0x184]
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fcomp dword ptr [gDrone_select_max_distance_squared+0x90]
            fnstsw ax
            test ah, 1
            jne L00451e0b
            fld dword ptr [esi + 0x190]
            fld dword ptr [ecx + 0x38]
            fld dword ptr [esi + 0x194]
            fld dword ptr [ecx + 0x3c]
            fld dword ptr [esi + 0x198]
            fxch st(3)
            fmulp st(4), st(0)
            fmulp st(1), st(0)
            faddp st(2), st(0)
            fmul dword ptr [ecx + 0x40]
            faddp st(1), st(0)
            fcomp dword ptr [gDrone_select_max_distance_squared+0x90]
            fnstsw ax
            test ah, 1
            jne L00451e0b
            fld dword ptr [esi + 0x19c]
            fld dword ptr [ecx + 0x44]
            fld dword ptr [esi + 0x1a0]
            fld dword ptr [ecx + 0x48]
            fld dword ptr [esi + 0x1a4]
            fxch st(3)
            fmulp st(4), st(0)
            fmulp st(1), st(0)
            faddp st(2), st(0)
            fmul dword ptr [ecx + 0x4c]
            faddp st(1), st(0)
            fcomp dword ptr [gDrone_select_max_distance_squared+0x90]
            fnstsw ax
            test ah, 1
            jne L00451e0b
    L00451dc6:
            fld dword ptr [esi + 0x18]
            fld dword ptr [esi + 0x1c]
            fld dword ptr [esp + 4]
            fxch st(2)
            fst dword ptr [esp + 4]
            fld st(1)
            fxch st(1)
            fmul dword ptr [esp + 4]
            fxch st(1)
            fmul st(0), st(2)
            fxch st(3)
            fst dword ptr [esp + 4]
            fmul dword ptr [esp + 4]
            fxch st(3)
            faddp st(1), st(0)
            fxch st(2)
            faddp st(2), st(0)
            fxch st(1)
            fsqrt
            fstp st(1)
            fst dword ptr [esp + 4]
            fcomp qword ptr [gDrone_select_max_distance_squared+0x14]
            fnstsw ax
            test ah, 0x41
            jne L00451e42
    L00451e0b:
            add ecx, 0x2c
            lea eax, [esi + 0x184]
            push ecx
            push eax
            call BrMatrix34Copy
            mov ecx, dword ptr [esi + 0x38]
            mov edx, dword ptr [esi]
            mov eax, dword ptr [gFrame]
            add esp, 8
            fld dword ptr [esp + 4]
            push ecx
            push edx
            push eax
            sub esp, 8
            _emit 0xdd
        _emit 0x1c
        _emit 0x24
            push offset gDefault_drone_direction+0x9bc
            call DoNotDprintf
            add esp, 0x18
    L00451e42:
            fld dword ptr [esi + 0x18]
            fdiv dword ptr [gDrone_delta_time]
            fstp dword ptr [esi + 0x18]
            fld dword ptr [esi + 0x1c]
            fdiv dword ptr [gDrone_delta_time]
            fstp dword ptr [esi + 0x1c]
            fld dword ptr [esi + 0x20]
            fdiv dword ptr [gDrone_delta_time]
            fstp dword ptr [esi + 0x20]
            pop esi
            pop ecx
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00452ba0
int C2_NAKED C2_HOOK_FASTCALL InitialiseEditModelsEtc(void) {
#ifndef CARPOCALYPSE2_MATCHING
    int uVar1;
    tU16* puVar2;
    tS16* psVar6;
    int iVar4;
    int iVar7;
    int iVar8;
    int iVar5;
    int local_c;
    int local_8;
    int local_4;
    tU8 bVar3;
    tU32 uVar9;
    tU32 uVar10;
    tU32 uVar11;
    br_material* material1;
    br_material* material2;
    tDrone_path_node* node;
    br_vector3* other_position;

    if (gStatic_drone_path_model_actor == NULL) {
        gStatic_drone_path_model_actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
        BrActorAdd(gNon_track_actor, gStatic_drone_path_model_actor);
    }
    if (gDrone_path_model_actor == NULL) {
        gDrone_path_model_actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
        BrActorAdd(gNon_track_actor, gDrone_path_model_actor);
    }
    if (gStatic_drone_path_model == NULL) {
        gStatic_drone_path_model = BrModelAllocate("StaticDronePathModel", 3, 1);
        gStatic_drone_path_model->flags |= BR_MODF_DONT_WELD | BR_MODF_KEEP_ORIGINAL | BR_MODF_GENERATE_TAGS;
        BrModelAdd(gStatic_drone_path_model);
    }
    if (gElasticatey_drone_path_model == NULL) {
        gElasticatey_drone_path_model = BrModelAllocate("ElasticateyDronePathModel", 3, 1);
        gElasticatey_drone_path_model->flags |= BR_MODF_DONT_WELD | BR_MODF_KEEP_ORIGINAL | BR_MODF_GENERATE_TAGS;
        BrModelAdd(gElasticatey_drone_path_model);
    }
    if (gStatic_drone_path_model_actor == NULL || gDrone_path_model_actor == NULL || gStatic_drone_path_model == NULL || gElasticatey_drone_path_model == NULL) {
        return 0;
    }

    gStatic_drone_path_model_actor->model = gStatic_drone_path_model;
    gStatic_drone_path_model_actor->type = BR_ACTOR_MODEL;
    gStatic_drone_path_model_actor->render_style = gCount_drone_path_nodes != 0 ? BR_RSTYLE_FACES : BR_RSTYLE_NONE;
    gDrone_path_model_actor->model = gElasticatey_drone_path_model;
    gDrone_path_model_actor->type = BR_ACTOR_MODEL;
    gDrone_path_model_actor->render_style = gDrone_paths_elasticating != 0 ? BR_RSTYLE_FACES : BR_RSTYLE_NONE;

    FUN_00452f40(gElasticatey_drone_path_model, 2, 4);
    iVar7 = gCount_drone_path_nodes * 8;
    iVar8 = gCount_drone_path_nodes * 12;
    if (0 < gCount_drone_path_nodes) {
        puVar2 = (tU16*)((tU8*)gDrone_path_nodes + 0x12c);
        iVar4 = gCount_drone_path_nodes;
        do {
            uVar1 = *puVar2;
            if (uVar1 > 0) {
                iVar7 += uVar1 * 4;
                iVar8 += uVar1 * 2;
            }
            puVar2 += 0x9a;
            iVar4--;
        } while (iVar4 != 0);
    }
    if (iVar7 > gStatic_drone_path_model_vertex_capacity || iVar8 > gStatic_drone_path_model_face_capacity) {
        FUN_00452f40(gStatic_drone_path_model, iVar8, iVar7);
        gStatic_drone_path_model_vertex_capacity = iVar7;
        gStatic_drone_path_model_face_capacity = iVar8;
    } else {
        gStatic_drone_path_model->nvertices = (br_uint_16)iVar7;
        gStatic_drone_path_model->nfaces = (br_uint_16)iVar8;
    }

    iVar4 = 0;
    iVar8 = 0;
    iVar7 = 0;
    if (0 < gCount_drone_path_nodes) {
        node = gDrone_path_nodes;
        do {
            if (iVar7 != gSelected_edit_drone_path) {
                uVar9 = (tU32)gMat_dk_grn;
                uVar10 = (tU32)gMat_md_grn;
                uVar11 = (tU32)gMat_lt_grn;
                if ((node->field_0x12e & 1) != 0) {
                    uVar9 = (tU32)gMat_dk_yel;
                    uVar10 = (tU32)gMat_md_yel;
                    uVar11 = (tU32)gMat_lt_yel;
                }
            } else {
                uVar9 = (tU32)gMat_dk_turq;
                uVar10 = (tU32)gMat_lt_turq;
                uVar11 = (tU32)gMat_lt_turq;
            }
            BuildDronePathNodeMesh(gStatic_drone_path_model, iVar8, iVar4, &node->position, uVar9, uVar10, uVar11);
            iVar4 += 0xc;
            iVar8 += 8;
            iVar7++;
            node++;
        } while (iVar7 < gCount_drone_path_nodes);
    }

    local_4 = 0;
    if (0 < gCount_drone_path_nodes) {
        local_8 = 0;
        iVar5 = (int)gDrone_path_nodes;
        do {
            local_c = 0;
            node = (tDrone_path_node*)(iVar5 + local_8);
            psVar6 = (tS16*)((tU8*)node + 0xc);
            while (1) {
                bVar3 = node->field_0x12e & 1;
                if (bVar3 != 0) {
                    uVar1 = 1;
                } else {
                    uVar1 = node->count_sections;
                }
                if (local_c >= uVar1) {
                    break;
                }
                if (bVar3 == 0 || *psVar6 >= 0) {
                    if (bVar3 != 0) {
                        material1 = gMat_dk_yel;
                        material2 = gMat_lt_yel;
                    } else {
                        material1 = gMat_dk_grn;
                        material2 = gMat_lt_grn;
                    }
                    other_position = &gDrone_path_nodes[*psVar6].position;
                    MakeAISimpleEditSectionHere(gStatic_drone_path_model, iVar8, iVar4, node, other_position, material1, material2);
                    iVar8 += 4;
                    iVar4 += 2;
                    iVar5 = (int)gDrone_path_nodes;
                }
                local_c++;
                psVar6 += 0x12;
            }
            local_4++;
            local_8 += 0x134;
        } while (local_4 < gCount_drone_path_nodes);
    }

    if (iVar8 != 0) {
        BrModelUpdate(gStatic_drone_path_model, BR_MODU_ALL);
        WriteOutDronePaths();
        return 1;
    }
    gStatic_drone_path_model_actor->render_style = BR_RSTYLE_NONE;
    WriteOutDronePaths();
    return 1;
#else
    __asm {
            mov eax, dword ptr [gStatic_drone_path_model_actor]
            sub esp, 0xc
            test eax, eax
            push ebx
            push ebp
            push esi
            push edi
            jne L00452bd0
            push 0
            push 1
            call BrActorAllocate
            add esp, 8
            mov dword ptr [gStatic_drone_path_model_actor], eax
            push eax
            mov eax, dword ptr [gNon_track_actor]
            push eax
            call BrActorAdd
            add esp, 8
    L00452bd0:
            mov eax, dword ptr [gDrone_path_model_actor]
            test eax, eax
            jne L00452bfa
            push 0
            push 1
            call BrActorAllocate
            mov ecx, dword ptr [gNon_track_actor]
            add esp, 8
            mov dword ptr [gDrone_path_model_actor], eax
            push eax
            push ecx
            call BrActorAdd
            add esp, 8
    L00452bfa:
            mov eax, dword ptr [gStatic_drone_path_model]
            mov esi, 7
            test eax, eax
            jne L00452c31
            push 1
            push 3
            push offset gDefault_drone_direction+0xc58
            call BrModelAllocate
            mov dword ptr [gStatic_drone_path_model], eax
            or word ptr [eax + 0x20], si
            mov edx, dword ptr [gStatic_drone_path_model]
            add esp, 0xc
            push edx
            call BrModelAdd
            add esp, 4
    L00452c31:
            mov eax, dword ptr [gElasticatey_drone_path_model]
            test eax, eax
            jne L00452c62
            push 1
            push 3
            push offset gDefault_drone_direction+0xc3c
            call BrModelAllocate
            mov dword ptr [gElasticatey_drone_path_model], eax
            or word ptr [eax + 0x20], si
            mov eax, dword ptr [gElasticatey_drone_path_model]
            add esp, 0xc
            push eax
            call BrModelAdd
            add esp, 4
    L00452c62:
            mov ecx, dword ptr [gStatic_drone_path_model_actor]
            test ecx, ecx
            je L00452f2f
            mov eax, dword ptr [gDrone_path_model_actor]
            test eax, eax
            je L00452f2f
            mov eax, dword ptr [gStatic_drone_path_model]
            test eax, eax
            je L00452f2f
            mov edx, dword ptr [gElasticatey_drone_path_model]
            test edx, edx
            je L00452f2f
            mov dword ptr [ecx + 0x18], eax
            mov ecx, dword ptr [gStatic_drone_path_model_actor]
            push 4
            mov byte ptr [ecx + 0x12], 1
            mov edx, dword ptr [gCount_drone_path_nodes]
            mov eax, dword ptr [gStatic_drone_path_model_actor]
            neg edx
            sbb edx, edx
            and edx, 3
            inc edx
            mov byte ptr [eax + 0x20], dl
            mov ecx, dword ptr [gDrone_path_model_actor]
            mov edx, dword ptr [gElasticatey_drone_path_model]
            mov dword ptr [ecx + 0x18], edx
            mov eax, dword ptr [gDrone_path_model_actor]
            mov byte ptr [eax + 0x12], 1
            mov ecx, dword ptr [gDrone_paths_elasticating]
            mov edx, dword ptr [gDrone_path_model_actor]
            neg ecx
            sbb ecx, ecx
            and ecx, 3
            inc ecx
            mov byte ptr [edx + 0x20], cl
            mov ecx, dword ptr [gElasticatey_drone_path_model]
            mov edx, 2
            call FUN_00452f40
            mov eax, dword ptr [gCount_drone_path_nodes]
            lea esi, [eax + eax*2]
            lea edi, [eax*8]
            shl esi, 2
            test eax, eax
            jle L00452d38
            mov ecx, dword ptr [gDrone_path_nodes]
            mov edx, eax
            add ecx, 0x12c
    L00452d20:
            xor eax, eax
            mov ax, word ptr [ecx]
            test eax, eax
            jle L00452d2f
            lea edi, [edi + eax*4]
            lea esi, [esi + eax*2]
    L00452d2f:
            add ecx, 0x134
            dec edx
            jne L00452d20
    L00452d38:
            cmp edi, dword ptr [gStatic_drone_path_model_vertex_capacity]
            jg L00452d5d
            cmp esi, dword ptr [gStatic_drone_path_model_face_capacity]
            jg L00452d5d
            mov edx, dword ptr [gStatic_drone_path_model]
            mov word ptr [edx + 0x10], di
            mov eax, dword ptr [gStatic_drone_path_model]
            mov word ptr [eax + 0x12], si
            jmp L00452d77
    L00452d5d:
            mov ecx, dword ptr [gStatic_drone_path_model]
            push edi
            mov edx, esi
            call FUN_00452f40
            mov dword ptr [gStatic_drone_path_model_vertex_capacity], edi
            mov dword ptr [gStatic_drone_path_model_face_capacity], esi
    L00452d77:
            mov eax, dword ptr [gCount_drone_path_nodes]
            xor edi, edi
            xor esi, esi
            xor ebp, ebp
            test eax, eax
            jle L00452e12
            xor ebx, ebx
    L00452d8c:
            cmp ebp, dword ptr [gSelected_edit_drone_path]
            jne L00452dad
            mov eax, dword ptr [gMat_lt_turq]
            mov edx, dword ptr [gDrone_path_nodes]
            mov ecx, dword ptr [gMat_dk_turq]
            push eax
            push eax
            add edx, ebx
            push ecx
            push edx
            jmp L00452dea
    L00452dad:
            mov eax, dword ptr [gDrone_path_nodes]
            add eax, ebx
            test byte ptr [eax + 0x12e], 1
            je L00452dd4
            mov ecx, dword ptr [gMat_lt_yel]
            mov edx, dword ptr [gMat_md_yel]
            push ecx
            mov ecx, dword ptr [gMat_dk_yel]
            push edx
            push ecx
            jmp L00452de9
    L00452dd4:
            mov edx, dword ptr [gMat_lt_grn]
            mov ecx, dword ptr [gMat_md_grn]
            push edx
            mov edx, dword ptr [gMat_dk_grn]
            push ecx
            push edx
    L00452de9:
            push eax
    L00452dea:
            mov ecx, dword ptr [gStatic_drone_path_model]
            push edi
            mov edx, esi
            call BuildDronePathNodeMesh
            mov eax, dword ptr [gCount_drone_path_nodes]
            add edi, 0xc
            add esi, 8
            inc ebp
            add ebx, 0x134
            cmp ebp, eax
            jl L00452d8c
    L00452e12:
            xor ecx, ecx
            cmp eax, ecx
            mov dword ptr [esp + 0x18], ecx
            jle L00452eea
            mov dword ptr [esp + 0x14], ecx
            mov ecx, dword ptr [gDrone_path_nodes]
    L00452e2a:
            mov eax, dword ptr [esp + 0x14]
            mov dword ptr [esp + 0x10], 0
            lea ebp, [ecx + eax]
            lea ebx, [ebp + 0xc]
    L00452e3c:
            mov dl, byte ptr [ebp + 0x12e]
            and dl, 1
            je L00452e4e
            mov eax, 1
            jmp L00452e57
    L00452e4e:
            xor eax, eax
            mov ax, word ptr [ebp + 0x12c]
    L00452e57:
            cmp dword ptr [esp + 0x10], eax
            jge L00452ec5
            test dl, dl
            je L00452e7c
            mov ax, word ptr [ebx]
            test ax, ax
            jl L00452eae
            mov edx, dword ptr [gMat_lt_yel]
            push edx
            mov edx, dword ptr [gMat_dk_yel]
            push edx
            movsx eax, ax
            jmp L00452e8c
    L00452e7c:
            mov edx, dword ptr [gMat_lt_grn]
            mov eax, dword ptr [gMat_dk_grn]
            push edx
            push eax
            movsx eax, word ptr [ebx]
    L00452e8c:
            lea edx, [eax + eax*8]
            lea edx, [eax + edx*2]
            lea eax, [eax + edx*4]
            mov edx, esi
            lea ecx, [ecx + eax*4]
            push ecx
            mov ecx, dword ptr [gStatic_drone_path_model]
            push ebp
            push edi
            call MakeAISimpleEditSectionHere
            mov ecx, dword ptr [gDrone_path_nodes]
    L00452eae:
            mov edx, dword ptr [esp + 0x10]
            add edi, 2
            add esi, 4
            inc edx
            mov dword ptr [esp + 0x10], edx
            add ebx, 0x24
            jmp L00452e3c
    L00452ec5:
            mov eax, dword ptr [esp + 0x18]
            mov ebx, dword ptr [esp + 0x14]
            mov edx, dword ptr [gCount_drone_path_nodes]
            inc eax
            add ebx, 0x134
            cmp eax, edx
            mov dword ptr [esp + 0x18], eax
            mov dword ptr [esp + 0x14], ebx
            jl L00452e2a
    L00452eea:
            test esi, esi
            je L00452f14
            mov edx, dword ptr [gStatic_drone_path_model]
            push 0x7fff
            push edx
            call BrModelUpdate
            add esp, 8
            call WriteOutDronePaths
            mov eax, 1
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0xc
            ret
    L00452f14:
            mov eax, dword ptr [gStatic_drone_path_model_actor]
            mov byte ptr [eax + 0x20], 1
            call WriteOutDronePaths
            mov eax, 1
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0xc
            ret
    L00452f2f:
            pop edi
            pop esi
            pop ebp
            xor eax, eax
            pop ebx
            add esp, 0xc
            ret
    }
#endif
}
void C2_HOOK_FASTCALL UpdateEditModels(void) {

    if (gDrone_path_model_actor != NULL && gElasticatey_drone_path_model != NULL && gDrone_paths_elasticating && gSelected_drone_path_node_index >= 0) {
        MakeAISimpleEditSectionHere(gElasticatey_drone_path_model, 0, 0,
            &gDrone_path_nodes[gSelected_drone_path_node_index],
            &gProgram_state.current_car.car_master_actor->t.t.translate.t,
            gMat_dk_grn, gMat_lt_grn);
        BrModelUpdate(gElasticatey_drone_path_model, BR_MATU_ALL);
    }
    if (gSelected_edit_drone_path >= 0 && gTime_stamp_for_this_munging > gNext_drone_edit_path_munge) {
        gSelected_edit_drone_path = -1;
        InitialiseEditModelsEtc();
    }
    if (gSelected_drone_path_index >= 0) {
        if (gTime_stamp_for_this_munging % 700 <= 700) {
            gDrone_specs[gSelected_drone_path_index].actor->render_style = BR_RSTYLE_FACES;
        } else {
            gDrone_specs[gSelected_drone_path_index].actor->render_style = BR_RSTYLE_BOUNDING_FACES;
        }
    }
}

void C2_HOOK_FASTCALL DoDroneDistanceChecks(tDrone_spec* pDrone) {

    if (gTrack_drone_min_y - 50 > pDrone->actor->t.t.translate.t.v[1]) {
        NewDroneState(pDrone, 5);
    } else {
        int process_drone;

        CalcRenderBoundsCentre();
        process_drone = gDrone_form_within_rendering_distance_functions[pDrone->form->type](&pDrone->actor->t.t.translate.t);
        pDrone->field_0x45 = process_drone;
        if (!process_drone) {
            if (!(pDrone->form->flags & 0x4)) {
                process_drone = gDrone_form_within_processing_distance_functions[pDrone->form->type](&pDrone->actor->t.t.translate.t);
            }
        }
        if (!process_drone) {
            StopProcessingThisDrone(pDrone, 0);
        } else if (!pDrone->field_0x45 || gFirst_drone_processing) {
            StartProcessingThisDrone(pDrone);
        }
        if (pDrone->field_0x45) {
            StartRenderingThisDrone(pDrone);
        } else {
            StopRenderingThisDrone(pDrone);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004512f0
void C2_NAKED C2_HOOK_FASTCALL DoDronePerGameFrameStuff(void) {
#ifndef CARPOCALYPSE2_MATCHING

    UpdateEditModels();
    gINT_006844fc = 0;
    gINT_006820d4 = 0;
    if (gDrones_unmodified) {
        int i;

        for (i = 0; i < gCount_drones; i++) {
            tDrone_spec *drone = &gDrone_specs[i];

            DoDroneDistanceChecks(drone);
            if (drone->current_state != 5) {
                ProcessThisDrone(i);
            }
            DoNotDprintf("FINISHED processing Drone %d, state %d", drone->id, drone->current_state);
        }
    }
#else
    __asm {
            mov eax, dword ptr [gDrone_path_model_actor]
            push ebp
            push esi
            push edi
            test eax, eax
            je L00451360
            mov ecx, dword ptr [gElasticatey_drone_path_model]
            test ecx, ecx
            je L00451360
            mov eax, dword ptr [gDrone_paths_elasticating]
            test eax, eax
            je L00451360
            mov eax, dword ptr [gSelected_drone_path_node_index]
            test eax, eax
            jl L00451360
            mov edx, dword ptr [gMat_lt_grn]
            push edx
            mov edx, dword ptr [gMat_dk_grn]
            push edx
            mov edx, dword ptr [gProgram_state+0xbc]
            add edx, 0x50
            push edx
            lea edx, [eax + eax*8]
            lea edx, [eax + edx*2]
            lea eax, [eax + edx*4]
            mov edx, dword ptr [gDrone_path_nodes]
            lea eax, [edx + eax*4]
            xor edx, edx
            push eax
            push 0
            call MakeAISimpleEditSectionHere
            mov ecx, dword ptr [gElasticatey_drone_path_model]
            push 0x7fff
            push ecx
            call BrModelUpdate
            add esp, 8
    L00451360:
            mov eax, dword ptr [gSelected_edit_drone_path]
            test eax, eax
            jl L00451387
            mov edx, dword ptr [gTime_stamp_for_this_munging]
            mov eax, dword ptr [gNext_drone_edit_path_munge]
            cmp edx, eax
            jbe L00451387
            mov dword ptr [gSelected_edit_drone_path], 0xffffffff
            call InitialiseEditModelsEtc
    L00451387:
            mov ecx, dword ptr [gSelected_drone_path_index]
            test ecx, ecx
            jl L004513e3
            mov eax, dword ptr [gTime_stamp_for_this_munging]
            xor edx, edx
            mov esi, 0x3e8
            div esi
            mov eax, ecx
            cmp edx, 0x2bc
            jbe L004513c7
            shl eax, 5
            mov edx, dword ptr [gDrone_specs]
            sub eax, ecx
            lea eax, [eax + eax*2]
            lea ecx, [ecx + eax*2]
            mov eax, dword ptr [edx + ecx*8 + 0xec]
            mov byte ptr [eax + 0x20], 7
            jmp L004513e3
    L004513c7:
            shl eax, 5
            sub eax, ecx
            lea edx, [eax + eax*2]
            lea eax, [ecx + edx*2]
            mov ecx, dword ptr [gDrone_specs]
            mov edx, dword ptr [ecx + eax*8 + 0xec]
            mov byte ptr [edx + 0x20], 4
    L004513e3:
            mov eax, dword ptr [gDrones_unmodified]
            mov dword ptr [gINT_006844fc], 0
            test eax, eax
            mov dword ptr [gINT_006820d4], 0
            je L0045160e
            mov eax, dword ptr [gCount_drones]
            xor ebp, ebp
            test eax, eax
            jle L0045160e
            xor edi, edi
    L00451415:
            mov eax, dword ptr [gDrone_specs]
            fld dword ptr [gTrack_drone_min_y]
            fsub dword ptr [gDOUBLE_00589d80+0x28]
            mov ecx, dword ptr [eax + edi + 0xec]
            lea esi, [eax + edi]
            fcomp dword ptr [ecx + 0x54]
            fnstsw ax
            test ah, 0x41
            jne L004514a4
            mov edx, dword ptr [esi + 0x38]
            mov eax, dword ptr [esi]
            push 5
            push edx
            push eax
            push offset gDefault_drone_direction+0x138
            call DoNotDprintf
            mov eax, dword ptr [esi + 0x38]
            add esp, 0x10
            cmp eax, 5
            je L004515cc
            mov eax, dword ptr [eax*4 + gDrone_state_functions]
            test eax, eax
            je L00451470
            mov edx, 4
            mov ecx, esi
            call eax
    L00451470:
            mov ecx, dword ptr [gTime_stamp_for_this_munging]
            mov edx, dword ptr [esi + 0x3c]
            mov eax, dword ptr [esi + 0x38]
            mov dword ptr [esi + 0x30], ecx
            mov dword ptr [esi + 0x40], edx
            mov dword ptr [esi + 0x3c], eax
            mov dword ptr [esi + 0x38], 5
            mov eax, dword ptr [gDrone_state_functions+0x14]
            test eax, eax
            je L004515cc
            xor edx, edx
            mov ecx, esi
            call eax
            jmp L004515cc
    L004514a4:
            call CalcRenderBoundsCentre
            mov ecx, dword ptr [esi + 4]
            mov edx, dword ptr [ecx + 0x10]
            mov ecx, dword ptr [esi + 0xec]
            add ecx, 0x50
            call dword ptr [edx*4 + gDrone_form_within_rendering_distance_functions]
            test al, al
            mov byte ptr [esi + 0x45], al
            jne L004514eb
            mov eax, dword ptr [esi + 4]
            test byte ptr [eax + 0x2c], 4
            jne L004514e6
            mov ecx, dword ptr [esi + 0xec]
            mov eax, dword ptr [eax + 0x10]
            add ecx, 0x50
            call dword ptr [eax*4 + gDrone_form_within_processing_distance_functions]
            test eax, eax
            je L004514eb
    L004514e6:
            mov eax, 1
    L004514eb:
            test eax, eax
            je L004515ae
            mov al, byte ptr [esi + 0x45]
            test al, al
            je L00451507
            mov eax, dword ptr [gFirst_drone_processing]
            test eax, eax
            je L004515b7
    L00451507:
            mov eax, dword ptr [gShow_drone_paths]
            test eax, eax
            jne L004515b7
            mov al, byte ptr [esi + 0x44]
            test al, al
            jne L004515b7
            mov ecx, dword ptr [esi + 4]
            test byte ptr [ecx + 0x2c], 4
            jne L00451535
            cmp dword ptr [gCount_active_drones], 0xc
            jge L004515b7
    L00451535:
            mov ecx, esi
            call CheckDroneInSensiblePlaceBeforeStartingToProcessTheCuntingThing
            test eax, eax
            je L004515b7
            mov ecx, esi
            call ReallyAddDroneToPHIL
            test eax, eax
            je L004515b7
            mov edx, dword ptr [esi + 0xec]
            lea eax, [esi + 0x184]
            add edx, 0x2c
            push edx
            push eax
            call BrMatrix34Copy
            mov ecx, dword ptr [esi + 4]
            mov byte ptr [esi + 0x44], 1
            add esp, 8
            test byte ptr [ecx + 0x2c], 4
            jne L00451577
            inc dword ptr [gCount_active_drones]
    L00451577:
            mov ecx, esi
            call DoDefaultDroneStateAction
            mov edx, dword ptr [esi + 0x38]
            mov eax, dword ptr [esi]
            mov ecx, dword ptr [gFrame]
            push edx
            push eax
            push ecx
            push offset gDefault_drone_direction+0x608
            call DoNotDprintf
            mov edx, dword ptr [esi + 0xec]
            add esp, 0x10
            add edx, 0x50
            mov ecx, offset gDefault_drone_direction+0x600
            call CrappyLittleVector3DPrintf
            jmp L004515b7
    L004515ae:
            xor edx, edx
            mov ecx, esi
            call StopProcessingThisDrone
    L004515b7:
            mov al, byte ptr [esi + 0x45]
            mov ecx, esi
            test al, al
            je L004515c7
            call StartRenderingThisDrone
            jmp L004515cc
    L004515c7:
            call StopRenderingThisDrone
    L004515cc:
            mov eax, dword ptr [gDrone_specs]
            cmp dword ptr [eax + edi + 0x38], 5
            je L004515e4
            mov ecx, ebp
            call ProcessThisDrone
            mov eax, dword ptr [gDrone_specs]
    L004515e4:
            mov edx, dword ptr [eax + edi + 0x38]
            mov eax, dword ptr [eax + edi]
            push edx
            push eax
            push offset gDefault_drone_direction+0x980
            call DoNotDprintf
            mov eax, dword ptr [gCount_drones]
            add esp, 0xc
            inc ebp
            add edi, 0x5d8
            cmp ebp, eax
            jl L00451415
    L0045160e:
            pop edi
            pop esi
            pop ebp
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004516d0
void C2_HOOK_FASTCALL StopRenderingThisDrone(tDrone_spec* pDrone_spec) {
    if (pDrone_spec->actor->render_style == BR_RSTYLE_FACES) {
        DoNotDprintf("STOP RENDERING: %d", pDrone_spec->id);
        gCount_rendered_drones -= 1;
        pDrone_spec->actor->render_style = BR_RSTYLE_NONE;
        PipeSingleDroneRender(pDrone_spec, 0);
    }
}

void C2_HOOK_FASTCALL RemoveDroneFromPHIL(tDrone_spec* pDrone) {
    ReallyRemoveDroneFromPHIL(pDrone);
}

void C2_HOOK_FASTCALL ReallyRemoveDroneFromPHIL(tDrone_spec* pDrone) {
    PHILRemoveObject(&pDrone->collision_info);
}

void C2_HOOK_FASTCALL PauseDroneState(tDrone_spec* pDrone) {
    if (gDrone_state_functions[pDrone->current_state] != NULL) {
        gDrone_state_functions[pDrone->current_state](pDrone, 2);
    }
}

// FUNCTION: CARMA2_HW 0x00451710
void C2_HOOK_FASTCALL StopProcessingThisDrone(tDrone_spec* pDrone, int pForce) {

    if (pDrone->field_0x44) {
        if (pForce || (!(pDrone->form->flags & 0x4) && pDrone->current_state != 3 && gCount_active_drones > 0)) {
            pDrone->field_0x44 = 0;
            gCount_active_drones -= 1;
            StopRenderingThisDrone(pDrone);
            RemoveDroneFromPHIL(pDrone);
            PauseDroneState(pDrone);
            DoNotDprintf("PROCESSING OFF: Frame %d, Drone %d, state %d", gFrame, pDrone->id, pDrone->current_state);
            CrappyLittleVector3DPrintf("     Pos", &pDrone->actor->t.t.translate.t);
        }
    }
}

void C2_HOOK_FASTCALL ResetDroneModel(tDrone_spec* pDrone) {

    C2_HOOK_BUG_ON(sizeof(pDrone->form->field_0x80[0]) != 1);

    if (pDrone->field_0xf4 >= 0) {
        ResetDroneCrushyModel(gDroneStorage.models[pDrone->form->model_index],
            pDrone->form->models[pDrone->field_0xf4]);
        pDrone->form->field_0x80[pDrone->field_0xf4] = 0;
        pDrone->field_0xf4 = -1;
        pDrone->model_actor->model = gDroneStorage.models[pDrone->form->model_index];
    }
}

void C2_HOOK_FASTCALL SemiInitDroneSpec(tDrone_spec* pDrone) {

    DoNotDprintf("SemiInitDroneSpec() - REINITIALISING DRONE SPEC");
    pDrone->field_0xa_pathnode_id = pDrone->field_0x8_pathnode_id;
    pDrone->field_0xe = -1;
    pDrone->field_0xc = -1;
    pDrone->field_0x14 = 0;
    pDrone->field_0x10 = 0;
    BrVector3Set(&pDrone->field_0x18, 0.f, 0.f, 0.f);
    BrVector3Copy(&pDrone->pos, &gDrone_path_nodes[pDrone->field_0xa_pathnode_id].position);
    pDrone->field_0xdc = 0;
    pDrone->field_0xe4 = 1.0;
    pDrone->field_0x48 = 1.0;
    ResetDroneModel(pDrone);
    pDrone->field_0x45 = gDrone_form_within_rendering_distance_functions[pDrone->form->type](&pDrone->actor->t.t.translate.t);
    pDrone->field_0x5d4 = 0;
    PipeDroneMatrix(pDrone);
}

// FUNCTION: CARMA2_HW 0x0044cc70
void C2_NAKED C2_HOOK_FASTCALL DroneStateFuncReset(tDrone_spec* pDrone, tDroneStateFuncState state) {
#ifndef CARPOCALYPSE2_MATCHING

    switch (state) {
    case eDrone_state_START:
        DoNotDprintf("DroneStateFuncReset() START");
        StopRenderingThisDrone(pDrone);
        StopProcessingThisDrone(pDrone, 1);
        SemiInitDroneSpec(pDrone);
        BrVector3Copy(&pDrone->actor->t.t.translate.t, &gDrone_path_nodes[pDrone->field_0x8_pathnode_id].position);
        break;
    case eDrone_state_RUN:
        DoNotDprintf("DroneStateFuncReset() RUN");
        if (!gShow_drone_paths) {
            NewDroneState(pDrone,2);
        }
        break;
    case eDrone_state_STOP:
        DoNotDprintf("DroneStateFuncReset() STOP");
        break;
    default:
        break;
    }
#else
    __asm {
            push ebx
            mov eax, edx
            xor ebx, ebx
            push ebp
            push esi
            sub eax, ebx
            push edi
            mov ebp, ecx
            je L0044cd25
            dec eax
            je L0044cca0
            sub eax, 3
            jne L0044cfbb
            push offset gDefault_drone_direction+0x190
            call DoNotDprintf
            add esp, 4
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret
    L0044cca0:
            push offset gDefault_drone_direction+0x174
            call DoNotDprintf
            mov eax, dword ptr [gShow_drone_paths]
            add esp, 4
            cmp eax, ebx
            jne L0044cfbb
            mov eax, dword ptr [ebp + 0x38]
            mov ecx, dword ptr [ebp]
            push 2
            push eax
            push ecx
            push offset gDefault_drone_direction+0x138
            call DoNotDprintf
            mov eax, dword ptr [ebp + 0x38]
            add esp, 0x10
            cmp eax, 2
            je L0044cfbb
            mov eax, dword ptr [eax*4 + gDrone_state_functions]
            cmp eax, ebx
            je L0044ccf1
            mov edx, 4
            mov ecx, ebp
            call eax
    L0044ccf1:
            mov edx, dword ptr [gTime_stamp_for_this_munging]
            mov eax, dword ptr [ebp + 0x3c]
            mov ecx, dword ptr [ebp + 0x38]
            mov dword ptr [ebp + 0x30], edx
            mov dword ptr [ebp + 0x40], eax
            mov dword ptr [ebp + 0x3c], ecx
            mov dword ptr [ebp + 0x38], 2
            mov eax, dword ptr [gDrone_state_functions+0x8]
            cmp eax, ebx
            je L0044cfbb
            xor edx, edx
            mov ecx, ebp
            call eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret
    L0044cd25:
            push offset gDefault_drone_direction+0x11c
            call DoNotDprintf
            mov edx, dword ptr [ebp + 0xec]
            add esp, 4
            cmp byte ptr [edx + 0x20], 4
            jne L0044cd6d
            mov eax, dword ptr [ebp]
            push eax
            push offset gDefault_drone_direction+0x108
            call DoNotDprintf
            mov eax, dword ptr [gCount_rendered_drones]
            add esp, 8
            dec eax
            xor edx, edx
            mov dword ptr [gCount_rendered_drones], eax
            mov ecx, dword ptr [ebp + 0xec]
            mov byte ptr [ecx + 0x20], 1
            mov ecx, ebp
            call PipeSingleDroneRender
    L0044cd6d:
            cmp byte ptr [ebp + 0x44], bl
            je L0044ce3b
            mov byte ptr [ebp + 0x44], bl
            mov ecx, dword ptr [gCount_active_drones]
            dec ecx
            mov dword ptr [gCount_active_drones], ecx
            mov edx, dword ptr [ebp + 0xec]
            cmp byte ptr [edx + 0x20], 4
            jne L0044cdc1
            mov eax, dword ptr [ebp]
            push eax
            push offset gDefault_drone_direction+0x108
            call DoNotDprintf
            mov eax, dword ptr [gCount_rendered_drones]
            add esp, 8
            dec eax
            xor edx, edx
            mov dword ptr [gCount_rendered_drones], eax
            mov ecx, dword ptr [ebp + 0xec]
            mov byte ptr [ecx + 0x20], 1
            mov ecx, ebp
            call PipeSingleDroneRender
    L0044cdc1:
            lea ecx, [ebp + 0xf8]
            call PHILRemoveObject
            mov edx, dword ptr [ebp + 0x38]
            mov eax, dword ptr [edx*4 + gDrone_state_functions]
            cmp eax, ebx
            je L0044cde3
            mov edx, 2
            mov ecx, ebp
            call eax
    L0044cde3:
            mov eax, dword ptr [ebp + 0x38]
            mov ecx, dword ptr [ebp]
            mov edx, dword ptr [gFrame]
            push eax
            push ecx
            push edx
            push offset gDefault_drone_direction+0xd8
            call DoNotDprintf
            mov eax, dword ptr [ebp + 0xec]
            add esp, 0x10
            add eax, 0x50
            sub esp, 8
            sub esp, 8
            fld dword ptr [eax + 8]
            fstp qword ptr [esp + 8]
            fld dword ptr [eax + 4]
            _emit 0xdd
        _emit 0x1c
        _emit 0x24
            _emit 0xd9
        _emit 0x00
            mov eax, dword ptr [gFrame]
            sub esp, 8
            _emit 0xdd
        _emit 0x1c
        _emit 0x24
            push offset gDefault_drone_direction+0xcc
            push eax
            push offset gDefault_drone_direction+0xb0
            call DoNotDprintf
            add esp, 0x24
    L0044ce3b:
            push offset gDefault_drone_direction+0x80
            call DoNotDprintf
            mov ax, word ptr [ebp + 8]
            or ecx, 0xffffffff
            mov word ptr [ebp + 0xa], ax
            mov word ptr [ebp + 0xe], cx
            movsx eax, ax
            mov word ptr [ebp + 0xc], cx
            mov dword ptr [ebp + 0x14], ebx
            lea ecx, [eax + eax*8]
            mov dword ptr [ebp + 0x10], ebx
            mov dword ptr [ebp + 0x18], ebx
            mov dword ptr [ebp + 0x1c], ebx
            lea edx, [eax + ecx*2]
            mov dword ptr [ebp + 0x20], ebx
            mov ecx, dword ptr [gDrone_path_nodes]
            add esp, 4
            lea eax, [eax + edx*4]
            shl eax, 2
            mov edx, dword ptr [ecx + eax]
            mov dword ptr [ebp + 0x24], edx
            mov ecx, dword ptr [gDrone_path_nodes]
            mov edx, dword ptr [ecx + eax + 4]
            mov dword ptr [ebp + 0x28], edx
            mov ecx, dword ptr [gDrone_path_nodes]
            mov edx, dword ptr [ecx + eax + 8]
            mov eax, 0x3f800000
            mov dword ptr [ebp + 0xe4], eax
            mov dword ptr [ebp + 0x48], eax
            mov eax, dword ptr [ebp + 0xf4]
            mov dword ptr [ebp + 0x2c], edx
            cmp eax, ebx
            mov dword ptr [ebp + 0xdc], ebx
            jl L0044cf36
            mov ecx, dword ptr [ebp + 4]
            mov edx, dword ptr [ecx + 0x84]
            mov ecx, dword ptr [ecx + 0x78]
            mov eax, dword ptr [edx + eax*4]
            mov edx, dword ptr [gDroneStorage+0x2c]
            mov edx, dword ptr [edx + ecx*4]
            mov edi, dword ptr [eax + 8]
            mov ecx, dword ptr [edx + 0xc]
            mov dword ptr [eax + 0xc], ecx
            mov cx, word ptr [edx + 0x12]
            mov word ptr [eax + 0x12], cx
            xor ecx, ecx
            mov cx, word ptr [eax + 0x10]
            mov esi, dword ptr [edx + 8]
            lea ecx, [ecx + ecx*4]
            shl ecx, 3
            mov edx, ecx
            shr ecx, 2
            rep movsd
            mov ecx, edx
            and ecx, 3
            rep movsb
            mov eax, dword ptr [ebp + 4]
            mov edx, dword ptr [ebp + 0xf4]
            mov ecx, dword ptr [eax + 0x80]
            mov byte ptr [ecx + edx], bl
            mov eax, dword ptr [ebp + 4]
            mov edx, dword ptr [ebp + 0xf0]
            mov dword ptr [ebp + 0xf4], 0xffffffff
            mov ecx, dword ptr [eax + 0x78]
            mov eax, dword ptr [gDroneStorage+0x2c]
            mov ecx, dword ptr [eax + ecx*4]
            mov dword ptr [edx + 0x18], ecx
    L0044cf36:
            mov edx, dword ptr [ebp + 4]
            mov ecx, dword ptr [ebp + 0xec]
            add ecx, 0x50
            mov eax, dword ptr [edx + 0x10]
            call dword ptr [eax*4 + gDrone_form_within_rendering_distance_functions]
            mov ecx, ebp
            mov byte ptr [ebp + 0x45], al
            mov dword ptr [ebp + 0x5d4], ebx
            call PipeDroneMatrix
            movsx eax, word ptr [ebp + 8]
            lea ecx, [eax + eax*8]
            lea edx, [eax + ecx*2]
            mov ecx, dword ptr [ebp + 0xec]
            lea eax, [eax + edx*4]
            mov edx, dword ptr [gDrone_path_nodes]
            mov eax, dword ptr [edx + eax*4]
            mov dword ptr [ecx + 0x50], eax
            movsx eax, word ptr [ebp + 8]
            lea ecx, [eax + eax*8]
            lea edx, [eax + ecx*2]
            mov ecx, dword ptr [ebp + 0xec]
            lea eax, [eax + edx*4]
            mov edx, dword ptr [gDrone_path_nodes]
            mov eax, dword ptr [edx + eax*4 + 4]
            mov dword ptr [ecx + 0x54], eax
            movsx eax, word ptr [ebp + 8]
            lea ecx, [eax + eax*8]
            lea edx, [eax + ecx*2]
            mov ecx, dword ptr [ebp + 0xec]
            lea eax, [eax + edx*4]
            mov edx, dword ptr [gDrone_path_nodes]
            mov eax, dword ptr [edx + eax*4 + 8]
            mov dword ptr [ecx + 0x58], eax
    L0044cfbb:
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0044e410
static int C2_NAKED C2_HOOK_FASTCALL AdvanceDroneSection(tDrone_spec* pDrone) {
#ifndef CARPOCALYPSE2_MATCHING
    tDrone_path_node* node;
    tDrone_path_node_section* section;
    int pick;

    if (pDrone->field_0xc < 0) {
        node = &gDrone_path_nodes[pDrone->field_0xa_pathnode_id];
        if (node->count_sections != 0) {
            pick = IRandomBetween(0, node->count_sections - 1);
            pDrone->field_0x10 = &node->sections[pick];
        } else {
            pDrone->field_0x14 = 0;
            pDrone->field_0x10 = 0;
            pDrone->field_0xc = -1;
            pDrone->field_0xa_pathnode_id = -1;
        }
    } else {
        pDrone->field_0xa_pathnode_id = pDrone->field_0xc;
        pDrone->field_0x10 = pDrone->field_0x14;
    }

    section = pDrone->field_0x10;
    if (section != NULL) {
        pDrone->field_0xc = section->node1;
    } else {
        pDrone->field_0xc = -1;
    }
    if (pDrone->field_0xc < 0) {
        return 0;
    }

    node = &gDrone_path_nodes[pDrone->field_0xc];
    if (node->count_sections != 0) {
        pick = IRandomBetween(0, node->count_sections - 1);
        pDrone->field_0x14 = &node->sections[pick];
        pDrone->field_0xe = pDrone->field_0x14->node1;
        return 1;
    }
    pDrone->field_0x14 = 0;
    pDrone->field_0xe = -1;
    return 1;
#else
    __asm {
            push ebx
            push esi
            mov esi, ecx
            or ebx, 0xffffffff
            mov ax, word ptr [esi + 0xc]
            test ax, ax
            jge L0044e491
            movsx eax, word ptr [esi + 0xa]
            lea ecx, [eax + eax*8]
            lea edx, [eax + ecx*2]
            mov ecx, dword ptr [gDrone_path_nodes]
            lea eax, [eax + edx*4]
            mov ax, word ptr [ecx + eax*4 + 0x12c]
            test ax, ax
            je L0044e479
            and eax, 0xffff
            xor edx, edx
            dec eax
            test eax, eax
            setl dl
            dec edx
            xor ecx, ecx
            and edx, eax
            call IRandomBetween
            lea edx, [eax + eax*8]
            movsx eax, word ptr [esi + 0xa]
            lea ecx, [eax + eax*8]
            lea ecx, [eax + ecx*2]
            lea eax, [eax + ecx*4]
            mov ecx, dword ptr [gDrone_path_nodes]
            lea eax, [ecx + eax*4]
            lea ecx, [eax + edx*4 + 0xc]
            mov dword ptr [esi + 0x10], ecx
            jmp L0044e49b
    L0044e479:
            mov dword ptr [esi + 0x14], 0
            mov dword ptr [esi + 0x10], 0
            mov word ptr [esi + 0xc], bx
            mov word ptr [esi + 0xa], bx
            jmp L0044e49b
    L0044e491:
            mov edx, dword ptr [esi + 0x14]
            mov word ptr [esi + 0xa], ax
            mov dword ptr [esi + 0x10], edx
    L0044e49b:
            mov eax, dword ptr [esi + 0x10]
            test eax, eax
            je L0044e4ab
            mov ax, word ptr [eax]
            mov word ptr [esi + 0xc], ax
            jmp L0044e4af
    L0044e4ab:
            mov word ptr [esi + 0xc], bx
    L0044e4af:
            mov ax, word ptr [esi + 0xc]
            test ax, ax
            jge L0044e4bd
            xor eax, eax
            pop esi
            pop ebx
            ret
    L0044e4bd:
            movsx eax, ax
            lea ecx, [eax + eax*8]
            lea edx, [eax + ecx*2]
            mov ecx, dword ptr [gDrone_path_nodes]
            lea eax, [eax + edx*4]
            mov ax, word ptr [ecx + eax*4 + 0x12c]
            test ax, ax
            je L0044e522
            and eax, 0xffff
            xor edx, edx
            dec eax
            test eax, eax
            setl dl
            dec edx
            xor ecx, ecx
            and edx, eax
            call IRandomBetween
            lea edx, [eax + eax*8]
            movsx eax, word ptr [esi + 0xc]
            lea ecx, [eax + eax*8]
            lea ecx, [eax + ecx*2]
            lea eax, [eax + ecx*4]
            mov ecx, dword ptr [gDrone_path_nodes]
            lea eax, [ecx + eax*4]
            lea eax, [eax + edx*4 + 0xc]
            mov dword ptr [esi + 0x14], eax
            mov cx, word ptr [eax]
            mov eax, 1
            mov word ptr [esi + 0xe], cx
            pop esi
            pop ebx
            ret
    L0044e522:
            mov dword ptr [esi + 0x14], 0
            mov word ptr [esi + 0xe], bx
            pop esi
            mov eax, 1
            pop ebx
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
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0044dac0
static void C2_NAKED C2_HOOK_FASTCALL CalculateDrivingInfo(tDrone_spec* pDrone) {
#ifndef CARPOCALYPSE2_MATCHING
    tDrone_form* form;

    form = pDrone->form;
    pDrone->field_0xe0 = 0.f;
    pDrone->field_0x74 = 0.f;
    if (pDrone->field_0x14 == 0 || (form->flags & 1) == 0) {
        pDrone->field_0x68 = 400.f;
        pDrone->field_0x6c = 0;
    } else {
        /* TODO: arc radius through (previous node, centre node, next node) */
        pDrone->field_0x68 = 400.f;
        pDrone->field_0x6c = 0;
    }

    if (form->speed >= 0.f) {
        pDrone->field_0x50 = form->speed;
    } else {
        pDrone->field_0x50 = pDrone->field_0x68 * 0.5f;
        if (pDrone->field_0x50 < form->min_speed) {
            pDrone->field_0x50 = form->min_speed;
        }
        if (pDrone->field_0x50 > form->max_speed) {
            pDrone->field_0x50 = form->max_speed;
        }
    }
    DoNotDprintf("CalculateDrivingInfo called - centre node %d, radius %f, section after corner %p",
        pDrone->field_0xc, pDrone->field_0x68, (void*)pDrone->field_0x10);
#else
    __asm {
            sub esp, 0x68
            push ebx
            push esi
            mov esi, ecx
            push edi
            mov dword ptr [esp + 0x20], esi
            mov ecx, dword ptr [esi + 4]
            mov dword ptr [esp + 0x1c], ecx
            mov eax, dword ptr [ecx + 0x10]
            dec eax
            je L0044daeb
            mov dword ptr [esp + 0x14], 0x3ca3d70a
            mov dword ptr [esp + 0xc], 0x3f000000
            jmp L0044dafb
    L0044daeb:
            mov dword ptr [esp + 0x14], 0x3dcccccd
            mov dword ptr [esp + 0xc], 0x40400000
    L0044dafb:
            mov edx, dword ptr [esi + 0x14]
            xor eax, eax
            cmp edx, eax
            mov dword ptr [esi + 0xe0], eax
            mov dword ptr [esi + 0x74], eax
            je L0044e36d
            test byte ptr [ecx + 0x2c], 1
            je L0044e36d
            movsx eax, word ptr [esi + 0xa]
            movsx ecx, word ptr [esi + 0xc]
            mov edi, dword ptr [gDrone_path_nodes]
            lea edx, [eax + eax*8]
            lea esi, [ecx + ecx*8]
            mov dword ptr [esp + 0x28], ecx
            lea edx, [eax + edx*2]
            lea esi, [ecx + esi*2]
            lea eax, [eax + edx*4]
            lea ecx, [ecx + esi*4]
            mov esi, dword ptr [esp + 0x20]
            fld dword ptr [edi + eax*4]
            fsub dword ptr [edi + ecx*4]
            lea edx, [edi + eax*4]
            lea eax, [edi + ecx*4]
            movsx ecx, word ptr [esi + 0xe]
            fstp dword ptr [esp + 0x44]
            fld dword ptr [edx + 4]
            fsub dword ptr [eax + 4]
            lea ebx, [ecx + ecx*8]
            fstp dword ptr [esp + 0x48]
            fld dword ptr [edx + 8]
            fsub dword ptr [eax + 8]
            lea edx, [ecx + ebx*2]
            lea ecx, [ecx + edx*4]
            fstp dword ptr [esp + 0x4c]
            fld dword ptr [edi + ecx*4]
            _emit 0xd8
        _emit 0x20
            lea ecx, [edi + ecx*4]
            fstp dword ptr [esp + 0x50]
            fld dword ptr [ecx + 4]
            fsub dword ptr [eax + 4]
            fld dword ptr [esp + 0x50]
            fmul dword ptr [esp + 0x4c]
            fld dword ptr [esp + 0x50]
            fxch st(2)
            fstp dword ptr [esp + 0x54]
            fld dword ptr [ecx + 8]
            fsub dword ptr [eax + 8]
            fld dword ptr [esp + 0x54]
            fld dword ptr [esp + 0x54]
            fxch st(2)
            fst dword ptr [esp + 0x58]
            fmul dword ptr [esp + 0x44]
            fld dword ptr [esp + 0x58]
            fxch st(2)
            fmul dword ptr [esp + 0x4c]
            fxch st(2)
            fmul dword ptr [esp + 0x48]
            fsubp st(2), st(0)
            fxch st(3)
            fsubp st(3), st(0)
            fxch st(3)
            fmul dword ptr [esp + 0x48]
            fld st(2)
            fcomp dword ptr [gDrone_render_zero]
            fxch st(1)
            fmul dword ptr [esp + 0x44]
            fnstsw ax
            test ah, 1
            fsubp st(1), st(0)
            je L0044dbf1
            fxch st(2)
            fchs
            fxch st(2)
            fxch st(1)
            fchs
            fxch st(1)
            fchs
    L0044dbf1:
            fld st(2)
            fld st(2)
            fxch st(1)
            fmul st(0), st(4)
            fld st(2)
            fxch st(2)
            fmul st(0), st(4)
            fxch st(2)
            fmul st(0), st(3)
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fsqrt
            fcom dword ptr [gDrone_select_max_distance_squared+0x10]
            fnstsw ax
            test ah, 0x41
            jne L0044dc3a
            fdivr qword ptr [gDrone_select_max_distance_squared+0x14]
            fld st(0)
            fld st(1)
            fxch st(1)
            fmulp st(5), st(0)
            fmul st(0), st(3)
            fxch st(1)
            fmul st(0), st(2)
            fxch st(1)
            fstp dword ptr [esp + 0x6c]
            fstp st(2)
            fstp st(0)
            jmp L0044dc56
    L0044dc3a:
            fstp st(0)
            fstp st(0)
            fstp st(0)
            fstp st(0)
            fld dword ptr [gDrone_render_zero+0x4]
            fld dword ptr [gDrone_render_zero]
            mov dword ptr [esp + 0x6c], 0
    L0044dc56:
            fld dword ptr [gDrone_render_zero]
            fld dword ptr [gDrone_render_zero]
            fxch st(3)
            fxch st(2)
            fsubr st(0), st(3)
            fxch st(2)
            fsub st(0), st(3)
            fxch st(2)
            fstp st(3)
            fld st(2)
            fmul st(0), st(3)
            fxch st(1)
            fxch st(2)
            fld st(2)
            fmul st(0), st(3)
            fxch st(1)
            fst dword ptr [esp + 0x40]
            fmul dword ptr [esp + 0x40]
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fsqrt
            fcom dword ptr [gDrone_select_max_distance_squared+0x10]
            fnstsw ax
            test ah, 0x41
            jne L0044dcc9
            fdivr qword ptr [gDrone_select_max_distance_squared+0x14]
            fld st(0)
            fld st(1)
            fxch st(1)
            fmul st(0), st(4)
            fxch st(1)
            fmul st(0), st(3)
            fxch st(2)
            fmul dword ptr [esp + 0x40]
            fxch st(1)
            fstp dword ptr [esp + 0x38]
            fxch st(1)
            fstp dword ptr [esp + 0x3c]
            fstp dword ptr [esp + 0x40]
            fstp st(0)
            fstp st(0)
            jmp L0044dce7
    L0044dcc9:
            fstp st(0)
            fstp st(0)
            fstp st(0)
            mov dword ptr [esp + 0x38], 0x3f800000
            mov dword ptr [esp + 0x3c], 0
            mov dword ptr [esp + 0x40], 0
    L0044dce7:
            fld dword ptr [esp + 0x6c]
            call _CIacos
            fmul qword ptr [gDrone_select_max_distance_squared+0x1c]
            call _ftol
            mov ecx, eax
            mov edx, ecx
            and edx, 0xffff
            mov dword ptr [esp + 0x28], edx
            fild dword ptr [esp + 0x28]
            fmul qword ptr [gDrone_select_max_distance_squared+0x24]
            fcom qword ptr [gDrone_select_max_distance_squared+0x2c]
            fst dword ptr [esp + 0x2c]
            fnstsw ax
            test ah, 0x41
            jne L0044dd2c
            fstp st(0)
            fld dword ptr [gDrone_render_zero]
    L0044dd2c:
            fcom qword ptr [gDrone_select_max_distance_squared+0x34]
            fnstsw ax
            test ah, 0x41
            jne L0044dd3f
            fsubr qword ptr [gDrone_select_max_distance_squared+0x2c]
    L0044dd3f:
            mov eax, dword ptr [esp + 0x1c]
            mov eax, dword ptr [eax + 0x10]
            cmp eax, 1
            je L0044dd74
            cmp eax, 3
            je L0044dd74
            fmul qword ptr [gDrone_select_max_distance_squared+0x3c]
            fld dword ptr [esp + 0xc]
            fsub dword ptr [esp + 0x14]
            fxch st(1)
            fsubr qword ptr [gDrone_select_max_distance_squared+0x14]
            fxch st(1)
            fmulp st(1), st(0)
            fadd dword ptr [esp + 0x14]
            fstp dword ptr [esp + 0xc]
            jmp L0044dd7e
    L0044dd74:
            mov edx, dword ptr [esp + 0xc]
            fstp st(0)
            mov dword ptr [esp + 0xc], edx
    L0044dd7e:
            lea eax, [esp + 0x38]
            lea edi, [esi + 0x78]
            push eax
            push ecx
            push edi
            call BrMatrix34Rotate
            add esp, 0xc
            lea ecx, [esi + 0xa8]
            push edi
            push ecx
            call BrMatrix34LPInverse
            add esp, 8
            lea edx, [esp + 0x44]
            lea eax, [esp + 0x5c]
            push edi
            push edx
            push eax
            call BrMatrix34ApplyV
            add esp, 0xc
            lea ecx, [esp + 0x50]
            lea edx, [esp + 0x68]
            push edi
            push ecx
            push edx
            call BrMatrix34ApplyV
            mov eax, dword ptr [esp + 0x68]
            mov ecx, dword ptr [esp + 0x70]
            mov dword ptr [esp + 0x2c], eax
            mov dword ptr [esp + 0x30], ecx
            fld dword ptr [esp + 0x2c]
            fmul dword ptr [esp + 0x2c]
            fld dword ptr [esp + 0x30]
            fmul dword ptr [esp + 0x30]
            mov edx, dword ptr [esp + 0x74]
            mov eax, dword ptr [esp + 0x7c]
            mov dword ptr [esp + 0x3c], edx
            mov dword ptr [esp + 0x40], eax
            faddp st(1), st(0)
            add esp, 0xc
            fstp dword ptr [esp + 0x1c]
            fld dword ptr [esp + 0x30]
            fmul dword ptr [esp + 0x30]
            fld dword ptr [esp + 0x34]
            fmul dword ptr [esp + 0x34]
            faddp st(1), st(0)
            fstp dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x1c]
            fcomp dword ptr [gDrone_render_zero]
            fnstsw ax
            test ah, 0x40
            jne L0044e35d
            fld dword ptr [esp + 0x14]
            fcomp dword ptr [gDrone_render_zero]
            fnstsw ax
            test ah, 0x40
            jne L0044e35d
            fld dword ptr [esp + 0x34]
            fld dword ptr [esp + 0x30]
            fld dword ptr [esp + 0x34]
            fld dword ptr [esp + 0x20]
            fld dword ptr [esp + 0x30]
            fxch st(3)
            fmul dword ptr [esp + 0x20]
            fxch st(4)
            fchs
            fxch st(2)
            fmul dword ptr [esp + 0x24]
            fxch st(2)
            fstp dword ptr [esp + 0x28]
            fmul dword ptr [esp + 0x28]
            fxch st(1)
            faddp st(3), st(0)
            fxch st(1)
            fmul dword ptr [esp + 0x24]
            faddp st(1), st(0)
            fld st(1)
            fld st(1)
            fxch st(1)
            fmul st(0), st(3)
            fxch st(1)
            fmul st(0), st(2)
            faddp st(1), st(0)
            fsqrt
            fcom dword ptr [gDrone_select_max_distance_squared+0x10]
            fnstsw ax
            test ah, 0x41
            jne L0044dea1
            fdivr qword ptr [gDrone_select_max_distance_squared+0x14]
            fld st(0)
            fmulp st(3), st(0)
            fmulp st(1), st(0)
            jmp L0044deb3
    L0044dea1:
            fstp st(0)
            fstp st(0)
            fstp st(0)
            fld dword ptr [gDrone_render_zero+0x4]
            fld dword ptr [gDrone_render_zero]
    L0044deb3:
            fcom dword ptr [gDrone_render_zero]
            fnstsw ax
            test ah, 0x41
            jne L0044dec7
            mov ebx, 1
            jmp L0044decb
    L0044dec7:
            xor ebx, ebx
            fchs
    L0044decb:
            fld st(1)
            fld st(1)
            fxch st(1)
            fmul dword ptr [esp + 0xc]
            fxch st(1)
            fcomp dword ptr [gDrone_select_max_distance_squared+0x44]
            fnstsw ax
            fadd dword ptr [esp + 0xc]
            test ah, 1
            jne L0044e357
            fxch st(2)
            fsub dword ptr [gDrone_select_max_distance_squared+0x48]
            fld st(2)
            fdiv st(0), st(2)
            fxch st(1)
            fdivr st(0), st(2)
            fxch st(3)
            fxch st(1)
            fld st(3)
            fmul st(0), st(4)
            fxch st(1)
            fst dword ptr [esp + 0x20]
            fmul st(0), st(4)
            fxch st(1)
            fmul dword ptr [esp + 0xc]
            fxch st(1)
            fmul dword ptr [gDrone_select_max_distance_squared+0x4c]
            fld st(4)
            fxch st(1)
            fsubp st(2), st(0)
            fmul dword ptr [esp + 0xc]
            fxch st(1)
            fsub dword ptr [esp + 0xc]
            fmul dword ptr [esp + 0xc]
            fsqrt
            fst dword ptr [esp + 0xc]
            fxch st(1)
            faddp st(1), st(0)
            fadd dword ptr [esp + 0x20]
            fstp st(2)
            fstp st(0)
            fld st(0)
            fmul st(0), st(1)
            fmul dword ptr [gDrone_select_max_distance_squared+0x50]
            fxch st(1)
            fstp dword ptr [esi + 0x6c]
            fcomp dword ptr [esp + 0x1c]
            fnstsw ax
            test ah, 0x41
            jne L0044df69
            fld dword ptr [esp + 0x1c]
            fsqrt
            fmul dword ptr [gDrone_select_max_distance_squared+0x54]
            fstp dword ptr [esi + 0x6c]
    L0044df69:
            fld dword ptr [esi + 0x6c]
            fld st(0)
            fmul st(0), st(1)
            fmul dword ptr [gDrone_select_max_distance_squared+0x50]
            fcomp dword ptr [esp + 0x14]
            fnstsw ax
            test ah, 0x41
            fstp st(0)
            jne L0044df92
            fld dword ptr [esp + 0x14]
            fsqrt
            fmul dword ptr [gDrone_select_max_distance_squared+0x54]
            fstp dword ptr [esi + 0x6c]
    L0044df92:
            fmul dword ptr [esi + 0x6c]
            fst dword ptr [esi + 0x68]
            fld dword ptr [esp + 0x30]
            test ebx, ebx
            je L0044dfcd
            fxch st(1)
            fmul dword ptr [esp + 0x28]
            fld dword ptr [esp + 0x34]
            fxch st(2)
            fmul dword ptr [esi + 0x6c]
            fld dword ptr [esp + 0x30]
            fxch st(1)
            faddp st(2), st(0)
            fxch st(1)
            fstp dword ptr [esi + 0x54]
            fxch st(1)
            fmul dword ptr [esi + 0x6c]
            fxch st(1)
            fmul dword ptr [esi + 0x68]
            faddp st(1), st(0)
            fstp dword ptr [esi + 0x58]
            jmp L0044dffa
    L0044dfcd:
            fld dword ptr [esp + 0x34]
            fmul st(0), st(2)
            fld dword ptr [esp + 0x34]
            fxch st(2)
            fmul dword ptr [esi + 0x6c]
            faddp st(1), st(0)
            fstp dword ptr [esi + 0x54]
            fld dword ptr [esi + 0x68]
            fxch st(1)
            fmul dword ptr [esi + 0x6c]
            fxch st(1)
            fchs
            fmul dword ptr [esp + 0x30]
            fxch st(1)
            faddp st(1), st(0)
            fstp dword ptr [esi + 0x58]
            fstp st(0)
    L0044dffa:
            fld dword ptr [esp + 0x14]
            fsqrt
            fld dword ptr [esi + 0x58]
            fld dword ptr [esi + 0x54]
            fdiv st(0), st(2)
            fxch st(1)
            fdiv st(0), st(2)
            fld dword ptr [esp + 0x5c]
            fld dword ptr [esp + 0x64]
            fxch st(3)
            fstp dword ptr [esi + 0x54]
            fxch st(1)
            fstp dword ptr [esi + 0x58]
            fld dword ptr [esi + 0x54]
            fld dword ptr [esi + 0x58]
            fxch st(2)
            fsub dword ptr [esi + 0x54]
            fxch st(3)
            fsub dword ptr [esi + 0x58]
            fxch st(1)
            fchs
            fxch st(2)
            fchs
            fxch st(2)
            fstp dword ptr [esp + 0x30]
            fxch st(1)
            fstp dword ptr [esp + 0x34]
            fxch st(1)
            fsub dword ptr [esp + 0x30]
            fxch st(1)
            fsub dword ptr [esp + 0x34]
            fld dword ptr [esi + 0x54]
            fld dword ptr [esi + 0x58]
            fxch st(3)
            fstp st(4)
            fld st(3)
            fld st(2)
            fld dword ptr [esp + 0x2c]
            fxch st(2)
            fmul st(0), st(6)
            fxch st(1)
            fmul st(0), st(4)
            fxch st(3)
            fst dword ptr [esp + 0x28]
            fxch st(5)
            fst dword ptr [esp + 0x20]
            fxch st(5)
            fmul dword ptr [esp + 0x28]
            fxch st(2)
            fsin
            mov byte ptr [esi + 0xd8], bl
            fxch st(5)
            fmul dword ptr [esp + 0x20]
            fxch st(3)
            faddp st(1), st(0)
            fxch st(4)
            fstp dword ptr [esp + 0x2c]
            fxch st(1)
            faddp st(1), st(0)
            fxch st(2)
            fsqrt
            fxch st(2)
            fsqrt
            fld dword ptr [esp + 0x2c]
            fmul dword ptr [gDrone_select_max_distance_squared+0x58]
            fxch st(3)
            fst dword ptr [esp + 0x2c]
            fxch st(3)
            fsubr dword ptr [gDrone_render_zero+0x4]
            fxch st(1)
            fst dword ptr [esp + 0x2c]
            fxch st(3)
            fst dword ptr [esp + 0x20]
            fcomp dword ptr [gDrone_select_max_distance_squared+0x10]
            fld dword ptr [esp + 0x68]
            fld dword ptr [esp + 0x70]
            fxch st(2)
            fmulp st(4), st(0)
            fnstsw ax
            fsub dword ptr [esi + 0x54]
            fxch st(1)
            fsub dword ptr [esi + 0x58]
            fld dword ptr [esi + 0x68]
            fxch st(4)
            fst dword ptr [esp + 0x28]
            fld dword ptr [esp + 0x28]
            fxch st(1)
            fstp dword ptr [esi + 0xe8]
            test ah, 0x41
            fstp dword ptr [esi + 0x70]
            fxch st(3)
            fstp dword ptr [esp + 0xc]
            fstp dword ptr [esp + 0x14]
            fxch st(1)
            fstp dword ptr [esp + 0x18]
            jne L0044e125
            fld dword ptr [esp + 0x20]
            fdivr qword ptr [gDrone_select_max_distance_squared+0x14]
            fld st(0)
            fmulp st(3), st(0)
            fmul st(0), st(1)
            fstp dword ptr [esp + 0x24]
            fstp st(0)
            jmp L0044e137
    L0044e125:
            fstp st(0)
            fstp st(0)
            fld dword ptr [gDrone_render_zero+0x4]
            mov dword ptr [esp + 0x24], 0
    L0044e137:
            fld dword ptr [esi + 0x68]
            fld dword ptr [esi + 0x6c]
            fmul st(0), st(2)
            fxch st(1)
            fstp st(2)
            fld st(1)
            fmul st(0), st(2)
            fxch st(1)
            fadd dword ptr [esp + 0x30]
            fld dword ptr [esi + 0x6c]
            fxch st(2)
            fstp dword ptr [esp + 0x28]
            mov ecx, dword ptr [esp + 0x28]
            fmul dword ptr [esp + 0xc]
            fld qword ptr [gDrone_select_max_distance_squared+0x5c]
            fxch st(1)
            fst dword ptr [esp + 0x2c]
            mov dword ptr [esp + 0x1c], ecx
            fdiv dword ptr [esp + 0x1c]
            fxch st(2)
            fmul dword ptr [esp + 0x24]
            fxch st(2)
            fstp st(3)
            fld st(2)
            fxch st(1)
            fcompp
            fadd dword ptr [esp + 0x34]
            fnstsw ax
            test ah, 0x41
            fstp dword ptr [esp + 0x24]
            jne L0044e199
            fld qword ptr [gDrone_select_max_distance_squared+0x5c]
            jmp L0044e19b
    L0044e199:
            fld st(0)
    L0044e19b:
            fld qword ptr [gDrone_select_max_distance_squared+0x14]
            fcomp st(1)
            fnstsw ax
            test ah, 1
            fstp st(0)
            je L0044e1b6
            fstp st(0)
            fld qword ptr [gDrone_select_max_distance_squared+0x14]
            jmp L0044e1cd
    L0044e1b6:
            fld st(0)
            fcomp qword ptr [gDrone_select_max_distance_squared+0x5c]
            fnstsw ax
            test ah, 1
            je L0044e1cd
            fstp st(0)
            fld qword ptr [gDrone_select_max_distance_squared+0x5c]
    L0044e1cd:
            fst dword ptr [esp + 0x2c]
            call _CIacos
            fld dword ptr [esp + 0x24]
            fcomp dword ptr [gDrone_render_zero]
            fnstsw ax
            fld st(0)
            test ah, 1
            fstp dword ptr [esi + 0x5c]
            je L0044e1f7
            fsubr qword ptr [gDrone_select_max_distance_squared+0x64]
            fstp dword ptr [esi + 0x5c]
            jmp L0044e1f9
    L0044e1f7:
            fstp st(0)
    L0044e1f9:
            fld dword ptr [esp + 0x18]
            fld dword ptr [esp + 0x14]
            fsub dword ptr [esp + 0x30]
            fxch st(1)
            fsub dword ptr [esp + 0x34]
            fld st(1)
            fld st(1)
            fxch st(1)
            fmul st(0), st(3)
            fxch st(1)
            fmul st(0), st(2)
            faddp st(1), st(0)
            fsqrt
            fcom dword ptr [gDrone_select_max_distance_squared+0x10]
            fnstsw ax
            test ah, 0x41
            jne L0044e23c
            fdivr qword ptr [gDrone_select_max_distance_squared+0x14]
            fld st(0)
            fmulp st(3), st(0)
            fmul st(0), st(1)
            fstp dword ptr [esp + 0x24]
            fstp st(0)
            jmp L0044e250
    L0044e23c:
            fstp st(0)
            fstp st(0)
            fstp st(0)
            fld dword ptr [gDrone_render_zero+0x4]
            mov dword ptr [esp + 0x24], 0
    L0044e250:
            fmul dword ptr [esi + 0x6c]
            fld dword ptr [esi + 0x6c]
            fxch st(1)
            fadd dword ptr [esp + 0x30]
            fld qword ptr [gDrone_select_max_distance_squared+0x5c]
            fxch st(1)
            fmul dword ptr [esp + 0xc]
            fxch st(2)
            fxch st(1)
            fxch st(2)
            mov edx, dword ptr [esp + 0x28]
            mov dword ptr [esp + 0x1c], edx
            fdiv dword ptr [esp + 0x1c]
            fxch st(1)
            fmul dword ptr [esp + 0x24]
            fxch st(1)
            fxch st(2)
            fld st(2)
            fxch st(1)
            fcompp
            fadd dword ptr [esp + 0x34]
            fnstsw ax
            test ah, 0x41
            fstp dword ptr [esp + 0x24]
            jne L0044e2a1
            fld qword ptr [gDrone_select_max_distance_squared+0x5c]
            jmp L0044e2a3
    L0044e2a1:
            fld st(0)
    L0044e2a3:
            fld qword ptr [gDrone_select_max_distance_squared+0x14]
            fcomp st(1)
            fnstsw ax
            test ah, 1
            fstp st(0)
            je L0044e2be
            fstp st(0)
            fld qword ptr [gDrone_select_max_distance_squared+0x14]
            jmp L0044e2d5
    L0044e2be:
            fld st(0)
            fcomp qword ptr [gDrone_select_max_distance_squared+0x5c]
            fnstsw ax
            test ah, 1
            je L0044e2d5
            fstp st(0)
            fld qword ptr [gDrone_select_max_distance_squared+0x5c]
    L0044e2d5:
            fst dword ptr [esp + 0x2c]
            call _CIacos
            fld dword ptr [esp + 0x24]
            fcomp dword ptr [gDrone_render_zero]
            fnstsw ax
            fld st(0)
            test ah, 1
            fstp dword ptr [esi + 0x64]
            je L0044e2ff
            fsubr qword ptr [gDrone_select_max_distance_squared+0x64]
            fstp dword ptr [esi + 0x64]
            jmp L0044e301
    L0044e2ff:
            fstp st(0)
    L0044e301:
            fld dword ptr [esi + 0x64]
            fcomp dword ptr [esi + 0x5c]
            test ebx, ebx
            fnstsw ax
            je L0044e32a
            test ah, 1
            je L0044e345
            fld dword ptr [esi + 0x64]
    L0044e315:
            fsub qword ptr [gDrone_select_max_distance_squared+0x6c]
            fcom dword ptr [esi + 0x5c]
            fnstsw ax
            test ah, 1
            jne L0044e315
            fstp dword ptr [esi + 0x64]
            jmp L0044e345
    L0044e32a:
            test ah, 0x41
            jne L0044e345
    L0044e32f:
            fld dword ptr [esi + 0x5c]
            fsub qword ptr [gDrone_select_max_distance_squared+0x6c]
            fcom dword ptr [esi + 0x64]
            fstp dword ptr [esi + 0x5c]
            fnstsw ax
            test ah, 1
            jne L0044e32f
    L0044e345:
            mov eax, dword ptr [esi + 0x5c]
            mov dword ptr [esi + 0xdc], 1
            mov dword ptr [esi + 0x60], eax
            jmp L0044e377
    L0044e357:
            fstp st(0)
            fstp st(0)
            fstp st(0)
    L0044e35d:
            mov dword ptr [esi + 0x68], 0x447a0000
            mov dword ptr [esi + 0x6c], 0
            jmp L0044e377
    L0044e36d:
            mov dword ptr [esi + 0x68], 0x447a0000
            mov dword ptr [esi + 0x6c], eax
    L0044e377:
            mov ecx, dword ptr [esi + 4]
            fld dword ptr [ecx + 0x18]
            fcomp dword ptr [gDrone_render_zero]
            fnstsw ax
            test ah, 1
            je L0044e3d7
            fld dword ptr [esi + 0x68]
            fmul dword ptr [gDrone_render_zero+0xc]
            fst dword ptr [esi + 0x50]
            fld dword ptr [ecx + 0x1c]
            fcomp st(1)
            fnstsw ax
            test ah, 0x41
            jne L0044e3a7
            fld dword ptr [ecx + 0x1c]
            jmp L0044e3a9
    L0044e3a7:
            fld st(0)
    L0044e3a9:
            fld dword ptr [ecx + 0x20]
            fcomp st(1)
            fnstsw ax
            test ah, 1
            fstp st(0)
            je L0044e3c1
            fstp st(0)
            fld dword ptr [ecx + 0x20]
            fstp dword ptr [esi + 0x50]
            jmp L0044e3dd
    L0044e3c1:
            fld dword ptr [ecx + 0x1c]
            fcomp st(1)
            fnstsw ax
            test ah, 0x41
            jne L0044e3d2
            fstp st(0)
            fld dword ptr [ecx + 0x1c]
    L0044e3d2:
            fstp dword ptr [esi + 0x50]
            jmp L0044e3dd
    L0044e3d7:
            mov ecx, dword ptr [ecx + 0x18]
            mov dword ptr [esi + 0x50], ecx
    L0044e3dd:
            mov edx, dword ptr [esi + 0x10]
            fld dword ptr [esi + 0x68]
            movsx eax, word ptr [esi + 0xc]
            push edx
            sub esp, 8
            _emit 0xdd
        _emit 0x1c
        _emit 0x24
            push eax
            push offset gDefault_drone_direction+0x498
            call DoNotDprintf
            add esp, 0x14
            pop edi
            pop esi
            pop ebx
            add esp, 0x68
            ret
    }
#endif
}

typedef enum {
    eDDS_straight_start = 0,
    eDDS_straight_driving = 1,
    eDDS_corner = 2,
} tDrone_driving_state;

// FUNCTION: CARMA2_HW 0x0044d2a0
void C2_NAKED C2_HOOK_FASTCALL MoveThisDronePlane(tDrone_spec* pDrone) {
#ifndef CARPOCALYPSE2_MATCHING

    DoNotDprintf("MoveThisDronePlane()");
    if (gDrone_delta_time <= gDrone_render_zero) {
        return;
    }
    for (;;) {
        switch (pDrone->field_0xdc) {
        case 0:
            DoNotDprintf("eDDS_straight_start");
            if (!AdvanceDroneSection(pDrone)) {
                NewDroneState(pDrone, 4);
                return;
            }
            CalculateDrivingInfo(pDrone);
            PointActorAlongThisBloodyVector(pDrone->actor,
                &pDrone->field_0x10->field_0x04);
            pDrone->field_0xdc = 1;
            break;
        default:
            /* TODO: eDDS_straight_driving / corner traversal not yet decompiled */
            return;
        }
    }
#else
    __asm {
            push ebp
            mov ebp, esp
            and esp, 0xfffffff8
            sub esp, 0x48
            push ebx
            push ebp
            push esi
            push edi
            mov esi, ecx
            push offset gDefault_drone_direction+0x480
            call DoNotDprintf
            mov eax, dword ptr [gDrone_delta_time]
            add esp, 4
            mov dword ptr [esp + 0x14], eax
            fld dword ptr [esp + 0x14]
            fcomp dword ptr [gDrone_render_zero]
            fnstsw ax
            test ah, 0x41
            jne L0044dab3
            xor ebx, ebx
    L0044d2dc:
            mov eax, dword ptr [esi + 0xdc]
            sub eax, ebx
            je L0044da00
            dec eax
            je L0044d532
            dec eax
            jne L0044da3a
            fld dword ptr [esi + 0x68]
            fcomp dword ptr [gDrone_render_zero]
            fnstsw ax
            test ah, 0x41
            jne L0044d51b
            fld dword ptr [esp + 0x14]
            fmul dword ptr [esi + 0x48]
            mov al, byte ptr [esi + 0xd8]
            mov edi, ebx
            cmp al, bl
            fdiv dword ptr [esi + 0x68]
            je L0044d35a
            fadd dword ptr [esi + 0x60]
            fcom dword ptr [esi + 0x64]
            fnstsw ax
            test ah, 1
            jne L0044d34e
            fstp st(0)
            fld dword ptr [esi + 0x64]
            fsub dword ptr [esi + 0x60]
            fmul dword ptr [esi + 0x68]
            mov ecx, dword ptr [esi + 0x64]
            mov edi, 1
            fdiv dword ptr [esi + 0x48]
            mov dword ptr [esi + 0x60], ecx
            fstp dword ptr [esp + 0x1c]
            jmp L0044d393
    L0044d34e:
            mov edx, dword ptr [gDrone_delta_time]
            mov dword ptr [esp + 0x1c], edx
            jmp L0044d390
    L0044d35a:
            fsubr dword ptr [esi + 0x60]
            fcom dword ptr [esi + 0x64]
            fnstsw ax
            test ah, 0x41
            je L0044d386
            fstp st(0)
            fld dword ptr [esi + 0x60]
            fsub dword ptr [esi + 0x64]
            fmul dword ptr [esi + 0x68]
            mov eax, dword ptr [esi + 0x64]
            mov edi, 1
            fdiv dword ptr [esi + 0x48]
            mov dword ptr [esi + 0x60], eax
            fstp dword ptr [esp + 0x1c]
            jmp L0044d393
    L0044d386:
            mov ecx, dword ptr [gDrone_delta_time]
            mov dword ptr [esp + 0x1c], ecx
    L0044d390:
            fstp dword ptr [esi + 0x60]
    L0044d393:
            fld dword ptr [esi + 0x60]
            fcos
            mov ecx, dword ptr [esi + 0xec]
            lea edx, [esi + 0xa8]
            lea eax, [esp + 0x4c]
            push edx
            add ecx, 0x50
            push eax
            push ecx
            mov dword ptr [esp + 0x5c], 0
            fmul dword ptr [esi + 0x68]
            fadd dword ptr [esi + 0x54]
            fstp dword ptr [esp + 0x58]
            fld dword ptr [esi + 0x60]
            fsin
            fmul dword ptr [esi + 0x68]
            fadd dword ptr [esi + 0x58]
            fstp dword ptr [esp + 0x60]
            call BrMatrix34ApplyV
            movsx eax, word ptr [esi + 0xc]
            mov ecx, dword ptr [esi + 0xec]
            add esp, 0xc
            lea edx, [eax + eax*8]
            lea edx, [eax + edx*2]
            lea eax, [eax + edx*4]
            mov edx, dword ptr [gDrone_path_nodes]
            fld dword ptr [edx + eax*4]
            fadd dword ptr [ecx + 0x50]
            fstp dword ptr [ecx + 0x50]
            movsx eax, word ptr [esi + 0xc]
            mov ecx, dword ptr [esi + 0xec]
            lea edx, [eax + eax*8]
            lea edx, [eax + edx*2]
            lea eax, [eax + edx*4]
            mov edx, dword ptr [gDrone_path_nodes]
            fld dword ptr [edx + eax*4 + 4]
            fadd dword ptr [ecx + 0x54]
            fstp dword ptr [ecx + 0x54]
            movsx eax, word ptr [esi + 0xc]
            mov ecx, dword ptr [esi + 0xec]
            lea edx, [eax + eax*8]
            lea edx, [eax + edx*2]
            lea eax, [eax + edx*4]
            mov edx, dword ptr [gDrone_path_nodes]
            fld dword ptr [edx + eax*4 + 8]
            fadd dword ptr [ecx + 0x58]
            fstp dword ptr [ecx + 0x58]
            fld dword ptr [esi + 0x60]
            fsub dword ptr [esi + 0x5c]
            fld dword ptr [esi + 0x64]
            fsub dword ptr [esi + 0x5c]
            mov eax, dword ptr [esi + 0x14]
            mov ecx, dword ptr [esi + 0x10]
            fdivp st(1), st(0)
            fst dword ptr [esp + 0x18]
            fld dword ptr [esp + 0x18]
            fxch st(1)
            fstp dword ptr [esi + 0xe0]
            fstp dword ptr [esi + 0x74]
            fld dword ptr [eax + 0x10]
            fmul dword ptr [esp + 0x18]
            fld dword ptr [ecx + 0x10]
            fld dword ptr [ecx + 0x14]
            fld dword ptr [ecx + 0x18]
            fld dword ptr [gDrone_render_zero+0x4]
            fsub dword ptr [esp + 0x18]
            fxch st(4)
            fstp dword ptr [esp + 0x34]
            fld dword ptr [eax + 0x14]
            fmul dword ptr [esp + 0x18]
            fxch st(3)
            fmul st(0), st(4)
            fxch st(3)
            fstp dword ptr [esp + 0x38]
            fld dword ptr [eax + 0x18]
            fxch st(2)
            fmul st(0), st(4)
            fxch st(1)
            fmul st(0), st(4)
            fxch st(2)
            fmul dword ptr [esp + 0x18]
            fxch st(3)
            fst dword ptr [esp + 0x40]
            fxch st(1)
            fst dword ptr [esp + 0x44]
            fxch st(2)
            fst dword ptr [esp + 0x48]
            fxch st(1)
            fadd dword ptr [esp + 0x34]
            fxch st(2)
            fadd dword ptr [esp + 0x38]
            fxch st(3)
            fstp dword ptr [esp + 0x3c]
            mov ecx, dword ptr [esi + 0xec]
            lea edx, [esp + 0x34]
            fadd dword ptr [esp + 0x3c]
            fxch st(1)
            fstp dword ptr [esp + 0x34]
            fxch st(1)
            fstp dword ptr [esp + 0x38]
            fstp dword ptr [esp + 0x3c]
            fstp st(0)
            call PointActorAlongThisBloodyVector
            fld dword ptr [esp + 0x14]
            fsub dword ptr [esp + 0x1c]
            cmp edi, ebx
            fstp dword ptr [esp + 0x14]
            je L0044d524
            mov eax, dword ptr [esi + 0x6c]
            mov ecx, dword ptr [esi + 0x48]
            mov dword ptr [esi + 0x4c], eax
            mov dword ptr [esi + 0xdc], ebx
            mov dword ptr [esi + 0xe4], ecx
            jmp L0044da3a
    L0044d51b:
            mov dword ptr [esi + 0x4c], ebx
            mov dword ptr [esi + 0xdc], ebx
    L0044d524:
            mov ecx, dword ptr [esi + 0x48]
            mov dword ptr [esi + 0xe4], ecx
            jmp L0044da3a
    L0044d532:
            fld dword ptr [esp + 0x14]
            fmul dword ptr [esi + 0x48]
            mov edx, dword ptr [esi + 0x10]
            fadd dword ptr [esi + 0x4c]
            mov al, byte ptr [edx + 0x20]
            cmp al, 4
            fstp dword ptr [esp + 0x30]
            jb L0044d5a8
            cmp al, 5
            jbe L0044d57a
            cmp al, 6
            jne L0044d5a8
            mov eax, dword ptr [esi + 4]
            sub esp, 8
            fld dword ptr [eax + 0x1c]
            fst dword ptr [esp + 0x20]
            fstp qword ptr [esp + 0x30]
            fld dword ptr [esi + 0x48]
            mov edi, dword ptr [esp + 0x34]
            mov ebp, dword ptr [esp + 0x30]
            _emit 0xdd
        _emit 0x1c
        _emit 0x24
            push edi
            push ebp
            push offset gDefault_drone_direction+0x454
            jmp L0044d5ce
    L0044d57a:
            mov ecx, dword ptr [esi + 4]
            sub esp, 8
            fld dword ptr [ecx + 0x20]
            fmul dword ptr [gDrone_render_zero+0x8]
            fst dword ptr [esp + 0x20]
            fstp qword ptr [esp + 0x30]
            fld dword ptr [esi + 0x48]
            mov edi, dword ptr [esp + 0x34]
            mov ebp, dword ptr [esp + 0x30]
            _emit 0xdd
        _emit 0x1c
        _emit 0x24
            push edi
            push ebp
            push offset gDefault_drone_direction+0x42c
            jmp L0044d5ce
    L0044d5a8:
            mov edx, dword ptr [esi + 4]
            sub esp, 8
            fld dword ptr [edx + 0x20]
            fst dword ptr [esp + 0x20]
            fstp qword ptr [esp + 0x30]
            fld dword ptr [esi + 0x48]
            mov edi, dword ptr [esp + 0x34]
            mov ebp, dword ptr [esp + 0x30]
            _emit 0xdd
        _emit 0x1c
        _emit 0x24
            push edi
            push ebp
            push offset gDefault_drone_direction+0x3fc
    L0044d5ce:
            call DoNotDprintf
            mov eax, dword ptr [esi + 0x10]
            add esp, 0x14
            fld dword ptr [esi + 0x4c]
            fld dword ptr [eax + 0x1c]
            fsub dword ptr [esi + 0x6c]
            fstp dword ptr [esp + 0x20]
            fcomp dword ptr [esp + 0x20]
            fnstsw ax
            test ah, 1
            jne L0044d606
            fld dword ptr [gDrone_render_zero]
            mov dword ptr [esi + 0xdc], 2
            jmp L0044d8f4
    L0044d606:
            fld dword ptr [esp + 0x30]
            fcomp dword ptr [esp + 0x20]
            fld dword ptr [esi + 0x48]
            fnstsw ax
            test ah, 0x41
            jne L0044d672
            fcomp dword ptr [esp + 0x18]
            fld dword ptr [esi + 0x48]
            fnstsw ax
            test ah, 0x41
            jne L0044d654
            sub esp, 8
            _emit 0xdd
        _emit 0x1c
        _emit 0x24
            push edi
            push ebp
            push offset gDefault_drone_direction+0x3d4
            call DoNotDprintf
            fld dword ptr [esi + 0x48]
            mov ecx, dword ptr [esi + 4]
            add esp, 0x14
            fsub dword ptr [esp + 0x18]
            fdiv dword ptr [ecx + 0x24]
            fld dword ptr [esp + 0x18]
            fstp dword ptr [esi + 0x48]
            jmp L0044d8f4
    L0044d654:
            sub esp, 8
            _emit 0xdd
        _emit 0x1c
        _emit 0x24
            push edi
            push ebp
            push offset gDefault_drone_direction+0x3ac
            call DoNotDprintf
            fld dword ptr [esp + 0x28]
            add esp, 0x14
            jmp L0044d8f4
    L0044d672:
            fld dword ptr [esp + 0x18]
            fmul dword ptr [esp + 0x18]
            fld st(1)
            fmulp st(2), st(0)
            sub esp, 8
            fstp dword ptr [esp + 0x30]
            fld dword ptr [esi + 0x48]
            fxch st(1)
            fsub dword ptr [esp + 0x30]
            fmul dword ptr [gDrone_render_zero+0xc]
            fxch st(1)
            _emit 0xdd
        _emit 0x1c
        _emit 0x24
            push edi
            push ebp
            fsubr dword ptr [esp + 0x30]
            sub esp, 8
            fst dword ptr [esp + 0x3c]
            _emit 0xdd
        _emit 0x1c
        _emit 0x24
            push offset gDefault_drone_direction+0x37c
            call DoNotDprintf
            mov edx, dword ptr [esi + 0x10]
            add esp, 0x1c
            mov cl, byte ptr [edx + 0x20]
            cmp cl, 2
            je L0044d890
            cmp cl, 3
            je L0044d7dd
            fld dword ptr [esi + 0x48]
            fcomp dword ptr [esp + 0x18]
            fnstsw ax
            test ah, 0x41
            jne L0044d7dd
            fld dword ptr [esi + 0x4c]
            fcomp dword ptr [esp + 0x24]
            fnstsw ax
            test ah, 1
            jne L0044d7dd
            fld dword ptr [esi + 0x48]
            sub esp, 8
            _emit 0xdd
        _emit 0x1c
        _emit 0x24
            push edi
            push ebp
            push offset gDefault_drone_direction+0x354
            call DoNotDprintf
            mov eax, dword ptr [esi + 0x10]
            add esp, 0x14
            fld dword ptr [esi + 0x48]
            fld dword ptr [eax + 0x1c]
            fsub dword ptr [esi + 0x6c]
            fld st(1)
            fxch st(1)
            fsub dword ptr [esi + 0x4c]
            fxch st(1)
            fmul st(0), st(2)
            fld dword ptr [esi + 0x48]
            fxch st(2)
            fadd st(0), st(0)
            fxch st(1)
            fsub dword ptr [esp + 0x28]
            fxch st(1)
            fdivp st(1), st(0)
            sub esp, 8
            fchs
            fxch st(1)
            fsub dword ptr [esp + 0x20]
            fxch st(1)
            fst dword ptr [esp + 0x2c]
            fchs
            fdivp st(1), st(0)
            fld dword ptr [esi + 0x48]
            _emit 0xdd
        _emit 0x1c
        _emit 0x24
            push edi
            push ebp
            fstp dword ptr [esp + 0x30]
            sub esp, 8
            fstp st(0)
            fld dword ptr [esp + 0x38]
            fld dword ptr [esp + 0x3c]
            fxch st(1)
            _emit 0xdd
        _emit 0x1c
        _emit 0x24
            sub esp, 8
            _emit 0xdd
        _emit 0x1c
        _emit 0x24
            push offset gDefault_drone_direction+0x310
            call DoNotDprintf
            fld dword ptr [esp + 0x44]
            fcomp dword ptr [esp + 0x38]
            fld dword ptr [esi + 0x48]
            add esp, 0x24
            fnstsw ax
            test ah, 0x41
            je L0044d7af
            sub esp, 8
            _emit 0xdd
        _emit 0x1c
        _emit 0x24
            push edi
            push ebp
            push offset gDefault_drone_direction+0x2e8
            call DoNotDprintf
            mov ecx, dword ptr [esp + 0x2c]
            add esp, 0x14
            fld dword ptr [esp + 0x20]
            mov dword ptr [esi + 0x48], ecx
            jmp L0044d8f4
    L0044d7af:
            sub esp, 8
            _emit 0xdd
        _emit 0x1c
        _emit 0x24
            push edi
            push ebp
            push offset gDefault_drone_direction+0x2c0
            call DoNotDprintf
            fld dword ptr [esp + 0x38]
            fmul dword ptr [esp + 0x28]
            fld dword ptr [esp + 0x28]
            fxch st(1)
            fadd dword ptr [esi + 0x48]
            add esp, 0x14
            fstp dword ptr [esi + 0x48]
            jmp L0044d8f4
    L0044d7dd:
            cmp cl, 2
            je L0044d890
            cmp cl, 3
            je L0044d829
            fld dword ptr [esi + 0x48]
            fcomp dword ptr [esp + 0x18]
            fnstsw ax
            test ah, 0x41
            jne L0044d829
            fld dword ptr [esp + 0x30]
            fcomp dword ptr [esp + 0x24]
            fnstsw ax
            test ah, 0x41
            jne L0044d829
            fld dword ptr [esi + 0x48]
            sub esp, 8
            _emit 0xdd
        _emit 0x1c
        _emit 0x24
            push edi
            push ebp
            push offset gDefault_drone_direction+0x298
            call DoNotDprintf
            fld dword ptr [esp + 0x28]
            add esp, 0x14
            jmp L0044d8f4
    L0044d829:
            cmp cl, 2
            je L0044d890
            cmp cl, 3
            je L0044d890
            fld dword ptr [esi + 0x48]
            fcomp dword ptr [esp + 0x18]
            fnstsw ax
            test ah, 0x41
            jne L0044d890
            fld dword ptr [esp + 0x14]
            fmul dword ptr [esp + 0x14]
            mov edx, dword ptr [esi + 4]
            fld dword ptr [esi + 0x48]
            fld dword ptr [edx + 0x24]
            fmulp st(2), st(0)
            fmul dword ptr [esp + 0x14]
            fxch st(1)
            fmul dword ptr [gDrone_render_zero+0xc]
            fxch st(1)
            faddp st(1), st(0)
            fadd dword ptr [esi + 0x4c]
            fcomp dword ptr [esp + 0x24]
            fnstsw ax
            test ah, 0x41
            jne L0044d890
            fld dword ptr [esi + 0x48]
            sub esp, 8
            _emit 0xdd
        _emit 0x1c
        _emit 0x24
            push edi
            push ebp
            push offset gDefault_drone_direction+0x270
            call DoNotDprintf
            fld dword ptr [esp + 0x28]
            add esp, 0x14
            jmp L0044d8f4
    L0044d890:
            fld dword ptr [esi + 0x48]
            sub esp, 8
            _emit 0xdd
        _emit 0x1c
        _emit 0x24
            push edi
            push ebp
            push offset gDefault_drone_direction+0x248
            call DoNotDprintf
            fld dword ptr [esi + 0x48]
            fcomp dword ptr [esp + 0x2c]
            mov ecx, dword ptr [esi + 4]
            add esp, 0x14
            fld dword ptr [ecx + 0x24]
            fmul dword ptr [esp + 0x14]
            fnstsw ax
            test ah, 0x41
            jne L0044d8d8
            fsubr dword ptr [esi + 0x48]
            fst dword ptr [esi + 0x48]
            fcomp dword ptr [ecx + 0x1c]
            fnstsw ax
            test ah, 1
            je L0044d8ee
            mov eax, dword ptr [ecx + 0x1c]
            mov dword ptr [esi + 0x48], eax
            jmp L0044d8ee
    L0044d8d8:
            fadd dword ptr [esi + 0x48]
            fst dword ptr [esi + 0x48]
            fcomp dword ptr [ecx + 0x20]
            fnstsw ax
            test ah, 0x41
            jne L0044d8ee
            mov ecx, dword ptr [ecx + 0x20]
            mov dword ptr [esi + 0x48], ecx
    L0044d8ee:
            fld dword ptr [gDrone_delta_time]
    L0044d8f4:
            cmp dword ptr [esi + 0xdc], 2
            jne L0044d93f
            mov eax, dword ptr [esi + 0x10]
            mov edx, dword ptr [esi + 0xec]
            fld dword ptr [eax + 0x1c]
            fsub dword ptr [esi + 0x6c]
            fst dword ptr [esi + 0x4c]
            fld dword ptr [eax + 0x10]
            fmul st(0), st(1)
            fstp dword ptr [edx + 0x50]
            mov eax, dword ptr [esi + 0x10]
            mov ecx, dword ptr [esi + 0xec]
            fstp st(0)
            fld dword ptr [eax + 0x14]
            fmul dword ptr [esi + 0x4c]
            fstp dword ptr [ecx + 0x54]
            mov edx, dword ptr [esi + 0x10]
            mov eax, dword ptr [esi + 0xec]
            fld dword ptr [edx + 0x18]
            fmul dword ptr [esi + 0x4c]
            fstp dword ptr [eax + 0x58]
            jmp L0044d981
    L0044d93f:
            fld st(0)
            fmul dword ptr [esi + 0x48]
            mov eax, dword ptr [esi + 0x10]
            mov ecx, dword ptr [esi + 0xec]
            fadd dword ptr [esi + 0x4c]
            fst dword ptr [esi + 0x4c]
            fld dword ptr [eax + 0x10]
            fmul st(0), st(1)
            fstp dword ptr [ecx + 0x50]
            mov edx, dword ptr [esi + 0x10]
            mov eax, dword ptr [esi + 0xec]
            fstp st(0)
            fld dword ptr [edx + 0x14]
            fmul dword ptr [esi + 0x4c]
            fstp dword ptr [eax + 0x54]
            mov ecx, dword ptr [esi + 0x10]
            mov edx, dword ptr [esi + 0xec]
            fld dword ptr [ecx + 0x18]
            fmul dword ptr [esi + 0x4c]
            fstp dword ptr [edx + 0x58]
    L0044d981:
            movsx eax, word ptr [esi + 0xa]
            mov ecx, dword ptr [esi + 0xec]
            lea edx, [eax + eax*8]
            lea edx, [eax + edx*2]
            lea eax, [eax + edx*4]
            mov edx, dword ptr [gDrone_path_nodes]
            fld dword ptr [edx + eax*4]
            fadd dword ptr [ecx + 0x50]
            fstp dword ptr [ecx + 0x50]
            movsx eax, word ptr [esi + 0xa]
            mov ecx, dword ptr [esi + 0xec]
            lea edx, [eax + eax*8]
            lea edx, [eax + edx*2]
            lea eax, [eax + edx*4]
            mov edx, dword ptr [gDrone_path_nodes]
            fld dword ptr [edx + eax*4 + 4]
            fadd dword ptr [ecx + 0x54]
            fstp dword ptr [ecx + 0x54]
            movsx eax, word ptr [esi + 0xa]
            mov ecx, dword ptr [esi + 0xec]
            lea edx, [eax + eax*8]
            lea edx, [eax + edx*2]
            lea eax, [eax + edx*4]
            mov edx, dword ptr [gDrone_path_nodes]
            fld dword ptr [edx + eax*4 + 8]
            fadd dword ptr [ecx + 0x58]
            fstp dword ptr [ecx + 0x58]
            fld dword ptr [esp + 0x14]
            mov eax, dword ptr [esi + 0x48]
            fsub st(0), st(1)
            mov dword ptr [esi + 0xe4], eax
            fstp dword ptr [esp + 0x14]
            fstp st(0)
            jmp L0044da3a
    L0044da00:
            push offset gDefault_drone_direction+0x234
            call DoNotDprintf
            add esp, 4
            mov ecx, esi
            call AdvanceDroneSection
            test eax, eax
            je L0044da57
            mov ecx, esi
            call CalculateDrivingInfo
            mov edx, dword ptr [esi + 0x10]
            mov ecx, dword ptr [esi + 0xec]
            add edx, 4
            call PointActorAlongThisBloodyVector
            mov dword ptr [esi + 0xdc], 1
    L0044da3a:
            fld dword ptr [esp + 0x14]
            fcomp dword ptr [gDrone_render_zero]
            fnstsw ax
            test ah, 0x41
            je L0044d2dc
            pop edi
            pop esi
            pop ebp
            pop ebx
            mov esp, ebp
            pop ebp
            ret
    L0044da57:
            mov ecx, dword ptr [esi + 0x38]
            mov edx, dword ptr [esi]
            push 4
            push ecx
            push edx
            push offset gDefault_drone_direction+0x138
            call DoNotDprintf
            mov eax, dword ptr [esi + 0x38]
            add esp, 0x10
            cmp eax, 4
            je L0044dab3
            mov eax, dword ptr [eax*4 + gDrone_state_functions]
            cmp eax, ebx
            je L0044da89
            mov edx, 4
            mov ecx, esi
            call eax
    L0044da89:
            mov eax, dword ptr [gTime_stamp_for_this_munging]
            mov ecx, dword ptr [esi + 0x3c]
            mov edx, dword ptr [esi + 0x38]
            mov dword ptr [esi + 0x30], eax
            mov dword ptr [esi + 0x40], ecx
            mov dword ptr [esi + 0x3c], edx
            mov dword ptr [esi + 0x38], 4
            mov eax, dword ptr [gDrone_state_functions+0x10]
            cmp eax, ebx
            je L0044dab3
            xor edx, edx
            mov ecx, esi
            call eax
    L0044dab3:
            pop edi
            pop esi
            pop ebp
            pop ebx
            mov esp, ebp
            pop ebp
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0044e540
void C2_NAKED C2_HOOK_FASTCALL MoveThisDroneCar(tDrone_spec* pDrone) {
#ifndef CARPOCALYPSE2_MATCHING

    DoNotDprintf("MoveThisDroneCar()");
    if (gDrone_delta_time <= gDrone_render_zero) {
        return;
    }
    for (;;) {
        switch (pDrone->field_0xdc) {
        case 0: {
            br_vector3 dir;

            DoNotDprintf("eDDS_straight_start");
            if (!AdvanceDroneSection(pDrone)) {
                NewDroneState(pDrone, 4);
                return;
            }
            CalculateDrivingInfo(pDrone);
            dir = pDrone->field_0x10->field_0x04;
            if ((pDrone->form->flags & 0x10) != 0) {
                dir.v[1] = 0.f;
            }
            PointActorAlongThisBloodyVector(pDrone->actor, &dir);
            pDrone->field_0xdc = 1;
            break;
        }
        default:
            /* TODO: eDDS_straight_driving / corner traversal not yet decompiled */
            return;
        }
    }
#else
    __asm {
            sub esp, 0x3c
            push ebx
            push ebp
            push esi
            push edi
            mov esi, ecx
            push offset gDefault_drone_direction+0x4ec
            call DoNotDprintf
            mov eax, dword ptr [gDrone_delta_time]
            add esp, 4
            mov dword ptr [esp + 0x10], eax
            fld dword ptr [esp + 0x10]
            fcomp dword ptr [gDrone_render_zero]
            fnstsw ax
            test ah, 0x41
            jne L0044eb62
            mov bl, 0x10
    L0044e576:
            mov eax, dword ptr [esi + 0xdc]
            xor ebp, ebp
            sub eax, ebp
            je L0044ea86
            dec eax
            je L0044e7de
            dec eax
            jne L0044eae8
            fld dword ptr [esi + 0x68]
            fcomp dword ptr [gDrone_render_zero]
            fnstsw ax
            test ah, 0x41
            jne L0044e7c7
            fld dword ptr [esp + 0x10]
            fmul dword ptr [esi + 0x48]
            mov al, byte ptr [esi + 0xd8]
            mov edi, ebp
            test al, al
            fdiv dword ptr [esi + 0x68]
            je L0044e5f6
            fadd dword ptr [esi + 0x60]
            fcom dword ptr [esi + 0x64]
            fnstsw ax
            test ah, 1
            jne L0044e5ea
            fstp st(0)
            fld dword ptr [esi + 0x64]
            fsub dword ptr [esi + 0x60]
            fmul dword ptr [esi + 0x68]
            mov ecx, dword ptr [esi + 0x64]
            mov edi, 1
            fdiv dword ptr [esi + 0x48]
            mov dword ptr [esi + 0x60], ecx
            fstp dword ptr [esp + 0x18]
            jmp L0044e62f
    L0044e5ea:
            mov edx, dword ptr [gDrone_delta_time]
            mov dword ptr [esp + 0x18], edx
            jmp L0044e62c
    L0044e5f6:
            fsubr dword ptr [esi + 0x60]
            fcom dword ptr [esi + 0x64]
            fnstsw ax
            test ah, 0x41
            je L0044e622
            fstp st(0)
            fld dword ptr [esi + 0x60]
            fsub dword ptr [esi + 0x64]
            fmul dword ptr [esi + 0x68]
            mov eax, dword ptr [esi + 0x64]
            mov edi, 1
            fdiv dword ptr [esi + 0x48]
            mov dword ptr [esi + 0x60], eax
            fstp dword ptr [esp + 0x18]
            jmp L0044e62f
    L0044e622:
            mov ecx, dword ptr [gDrone_delta_time]
            mov dword ptr [esp + 0x18], ecx
    L0044e62c:
            fstp dword ptr [esi + 0x60]
    L0044e62f:
            fld dword ptr [esi + 0x60]
            fcos
            mov ecx, dword ptr [esi + 0xec]
            lea edx, [esi + 0xa8]
            lea eax, [esp + 0x40]
            push edx
            add ecx, 0x50
            push eax
            push ecx
            mov dword ptr [esp + 0x50], 0
            fmul dword ptr [esi + 0x68]
            fadd dword ptr [esi + 0x54]
            fstp dword ptr [esp + 0x4c]
            fld dword ptr [esi + 0x60]
            fsin
            fmul dword ptr [esi + 0x68]
            fadd dword ptr [esi + 0x58]
            fstp dword ptr [esp + 0x54]
            call BrMatrix34ApplyV
            movsx eax, word ptr [esi + 0xc]
            mov ecx, dword ptr [esi + 0xec]
            add esp, 0xc
            lea edx, [eax + eax*8]
            lea edx, [eax + edx*2]
            lea eax, [eax + edx*4]
            mov edx, dword ptr [gDrone_path_nodes]
            fld dword ptr [edx + eax*4]
            fadd dword ptr [ecx + 0x50]
            fstp dword ptr [ecx + 0x50]
            movsx eax, word ptr [esi + 0xc]
            mov ecx, dword ptr [esi + 0xec]
            lea edx, [eax + eax*8]
            lea edx, [eax + edx*2]
            lea eax, [eax + edx*4]
            mov edx, dword ptr [gDrone_path_nodes]
            fld dword ptr [edx + eax*4 + 4]
            fadd dword ptr [ecx + 0x54]
            fstp dword ptr [ecx + 0x54]
            movsx eax, word ptr [esi + 0xc]
            mov ecx, dword ptr [esi + 0xec]
            lea edx, [eax + eax*8]
            lea edx, [eax + edx*2]
            lea eax, [eax + edx*4]
            mov edx, dword ptr [gDrone_path_nodes]
            fld dword ptr [edx + eax*4 + 8]
            fadd dword ptr [ecx + 0x58]
            fstp dword ptr [ecx + 0x58]
            fld dword ptr [esi + 0x60]
            fsub dword ptr [esi + 0x5c]
            fld dword ptr [esi + 0x64]
            fsub dword ptr [esi + 0x5c]
            mov eax, dword ptr [esi + 0x14]
            mov ecx, dword ptr [esi + 0x10]
            fdivp st(1), st(0)
            fst dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x14]
            fxch st(1)
            fstp dword ptr [esi + 0xe0]
            fstp dword ptr [esi + 0x74]
            fld dword ptr [eax + 0x10]
            fmul dword ptr [esp + 0x14]
            fld dword ptr [ecx + 0x10]
            fld dword ptr [ecx + 0x14]
            fld dword ptr [ecx + 0x18]
            fld dword ptr [gDrone_render_zero+0x4]
            fsub dword ptr [esp + 0x14]
            fxch st(4)
            fstp dword ptr [esp + 0x28]
            fld dword ptr [eax + 0x14]
            fmul dword ptr [esp + 0x14]
            fxch st(3)
            fmul st(0), st(4)
            fxch st(3)
            fstp dword ptr [esp + 0x2c]
            fld dword ptr [eax + 0x18]
            fxch st(2)
            fmul st(0), st(4)
            fxch st(1)
            fmul st(0), st(4)
            fxch st(2)
            fmul dword ptr [esp + 0x14]
            fxch st(3)
            fst dword ptr [esp + 0x34]
            fxch st(1)
            fst dword ptr [esp + 0x38]
            fxch st(2)
            fst dword ptr [esp + 0x3c]
            fxch st(1)
            fadd dword ptr [esp + 0x28]
            fxch st(2)
            fadd dword ptr [esp + 0x2c]
            fxch st(3)
            fstp dword ptr [esp + 0x30]
            mov eax, dword ptr [esi + 4]
            fadd dword ptr [esp + 0x30]
            fxch st(1)
            fstp dword ptr [esp + 0x28]
            fxch st(1)
            fstp dword ptr [esp + 0x2c]
            fstp dword ptr [esp + 0x30]
            test byte ptr [eax + 0x2c], bl
            fstp st(0)
            je L0044e78e
            mov dword ptr [esp + 0x2c], 0
    L0044e78e:
            mov ecx, dword ptr [esi + 0xec]
            lea edx, [esp + 0x28]
            call PointActorAlongThisBloodyVector
            fld dword ptr [esp + 0x10]
            fsub dword ptr [esp + 0x18]
            cmp edi, ebp
            fstp dword ptr [esp + 0x10]
            je L0044e7d0
            mov ecx, dword ptr [esi + 0x6c]
            mov edx, dword ptr [esi + 0x48]
            mov dword ptr [esi + 0x4c], ecx
            mov dword ptr [esi + 0xdc], ebp
            mov dword ptr [esi + 0xe4], edx
            jmp L0044eae8
    L0044e7c7:
            mov dword ptr [esi + 0x4c], ebp
            mov dword ptr [esi + 0xdc], ebp
    L0044e7d0:
            mov edx, dword ptr [esi + 0x48]
            mov dword ptr [esi + 0xe4], edx
            jmp L0044eae8
    L0044e7de:
            mov edx, dword ptr [esi + 4]
            fld dword ptr [edx + 0x18]
            fcomp dword ptr [gDrone_render_zero]
            fnstsw ax
            test ah, 1
            jne L0044e7fa
            fld dword ptr [edx + 0x18]
            fst dword ptr [esi + 0x48]
            fstp dword ptr [esi + 0x50]
    L0044e7fa:
            mov ecx, dword ptr [esi + 0x10]
            fld dword ptr [ecx + 0x1c]
            fsub dword ptr [esi + 0x6c]
            fld dword ptr [esi + 0x4c]
            fld dword ptr [esp + 0x10]
            fmul dword ptr [esi + 0x48]
            fxch st(2)
            fstp dword ptr [esp + 0x14]
            fcomp dword ptr [esp + 0x14]
            fadd dword ptr [esi + 0x4c]
            fnstsw ax
            test ah, 1
            fstp dword ptr [esp + 0x24]
            jne L0044e851
            fld dword ptr [gDrone_render_zero]
            fld dword ptr [esi + 0x48]
            fcomp dword ptr [esi + 0x50]
            mov dword ptr [esi + 0xdc], 2
            fnstsw ax
            test ah, 0x41
            jne L0044e9b7
            mov eax, dword ptr [esi + 0x50]
            mov dword ptr [esi + 0x48], eax
            jmp L0044e9b7
    L0044e851:
            fld dword ptr [esp + 0x24]
            fcomp dword ptr [esp + 0x14]
            fnstsw ax
            test ah, 0x41
            jne L0044e882
            fld dword ptr [esi + 0x48]
            fcomp dword ptr [esi + 0x50]
            fnstsw ax
            test ah, 0x41
            jne L0044e9b3
            fld dword ptr [esi + 0x48]
            fsub dword ptr [esi + 0x50]
            mov edx, dword ptr [esi + 0x50]
            mov dword ptr [esi + 0x48], edx
            jmp L0044e9b7
    L0044e882:
            fld dword ptr [edx + 0x18]
            fcomp dword ptr [gDrone_render_zero]
            fnstsw ax
            test ah, 1
            je L0044e9b3
            fld dword ptr [esi + 0x50]
            fld dword ptr [esi + 0x48]
            fst dword ptr [esp + 0x1c]
            fld st(1)
            fxch st(1)
            fmul dword ptr [esp + 0x1c]
            fld dword ptr [esi + 0x48]
            fxch st(2)
            fmul st(0), st(3)
            fxch st(2)
            fcomp dword ptr [esi + 0x50]
            fxch st(1)
            fsubp st(1), st(0)
            fnstsw ax
            fstp dword ptr [esp + 0x20]
            test ah, 0x41
            fstp st(0)
            fld dword ptr [esp + 0x20]
            fmul dword ptr [gDrone_render_zero+0xc]
            fsubr dword ptr [esp + 0x14]
            fstp dword ptr [esp + 0x1c]
            jne L0044e935
            fld dword ptr [esi + 0x4c]
            fcomp dword ptr [esp + 0x1c]
            fnstsw ax
            test ah, 1
            jne L0044e935
            fld dword ptr [esp + 0x14]
            fsub dword ptr [esi + 0x4c]
            fld dword ptr [esi + 0x48]
            fsub dword ptr [esi + 0x50]
            fxch st(1)
            fadd st(0), st(0)
            fdivr dword ptr [esp + 0x20]
            fchs
            fst dword ptr [esp + 0x20]
            fchs
            fdivp st(1), st(0)
            fcom dword ptr [esp + 0x10]
            fnstsw ax
            test ah, 0x41
            je L0044e91a
            mov eax, dword ptr [esi + 0x50]
            mov dword ptr [esi + 0x48], eax
            jmp L0044e9b7
    L0044e91a:
            fstp st(0)
            fld dword ptr [esp + 0x20]
            fmul dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x10]
            fxch st(1)
            fadd dword ptr [esi + 0x48]
            fstp dword ptr [esi + 0x48]
            jmp L0044e9b7
    L0044e935:
            fld dword ptr [esi + 0x48]
            fcomp dword ptr [esi + 0x50]
            fnstsw ax
            test ah, 0x41
            jne L0044e951
            fld dword ptr [esp + 0x24]
            fcomp dword ptr [esp + 0x1c]
            fnstsw ax
            test ah, 0x41
            je L0044e9b3
    L0044e951:
            fld dword ptr [esi + 0x48]
            fcomp dword ptr [esi + 0x50]
            fnstsw ax
            test ah, 0x41
            jne L0044e98e
            fld dword ptr [esp + 0x10]
            fmul dword ptr [esp + 0x10]
            fld dword ptr [edx + 0x24]
            fld dword ptr [esi + 0x48]
            fxch st(1)
            fmulp st(2), st(0)
            fmul dword ptr [esp + 0x10]
            fxch st(1)
            fmul dword ptr [gDrone_render_zero+0xc]
            fxch st(1)
            faddp st(1), st(0)
            fadd dword ptr [esi + 0x4c]
            fcomp dword ptr [esp + 0x1c]
            fnstsw ax
            test ah, 0x41
            je L0044e9b3
    L0044e98e:
            fld dword ptr [edx + 0x24]
            fmul dword ptr [esp + 0x10]
            fadd dword ptr [esi + 0x48]
            fst dword ptr [esi + 0x48]
            fcomp dword ptr [edx + 0x20]
            fnstsw ax
            test ah, 0x41
            jne L0044e9ab
            mov edx, dword ptr [edx + 0x20]
            mov dword ptr [esi + 0x48], edx
    L0044e9ab:
            fld dword ptr [gDrone_delta_time]
            jmp L0044e9b7
    L0044e9b3:
            fld dword ptr [esp + 0x10]
    L0044e9b7:
            cmp dword ptr [esi + 0xdc], 2
            jne L0044e9c8
            fld dword ptr [ecx + 0x1c]
            fsub dword ptr [esi + 0x6c]
            jmp L0044e9d0
    L0044e9c8:
            fld st(0)
            fmul dword ptr [esi + 0x48]
            fadd dword ptr [esi + 0x4c]
    L0044e9d0:
            fst dword ptr [esi + 0x4c]
            fld dword ptr [ecx + 0x10]
            mov eax, dword ptr [esi + 0xec]
            fmul st(0), st(1)
            fstp dword ptr [eax + 0x50]
            mov ecx, dword ptr [esi + 0x10]
            mov edx, dword ptr [esi + 0xec]
            fstp st(0)
            fld dword ptr [ecx + 0x14]
            fmul dword ptr [esi + 0x4c]
            fstp dword ptr [edx + 0x54]
            mov eax, dword ptr [esi + 0x10]
            mov ecx, dword ptr [esi + 0xec]
            fld dword ptr [eax + 0x18]
            fmul dword ptr [esi + 0x4c]
            fstp dword ptr [ecx + 0x58]
            movsx eax, word ptr [esi + 0xa]
            mov ecx, dword ptr [esi + 0xec]
            lea edx, [eax + eax*8]
            lea edx, [eax + edx*2]
            lea eax, [eax + edx*4]
            mov edx, dword ptr [gDrone_path_nodes]
            fld dword ptr [edx + eax*4]
            fadd dword ptr [ecx + 0x50]
            fstp dword ptr [ecx + 0x50]
            movsx eax, word ptr [esi + 0xa]
            mov ecx, dword ptr [esi + 0xec]
            lea edx, [eax + eax*8]
            lea edx, [eax + edx*2]
            lea eax, [eax + edx*4]
            mov edx, dword ptr [gDrone_path_nodes]
            fld dword ptr [edx + eax*4 + 4]
            fadd dword ptr [ecx + 0x54]
            fstp dword ptr [ecx + 0x54]
            movsx eax, word ptr [esi + 0xa]
            mov ecx, dword ptr [esi + 0xec]
            lea edx, [eax + eax*8]
            lea edx, [eax + edx*2]
            lea eax, [eax + edx*4]
            mov edx, dword ptr [gDrone_path_nodes]
            fld dword ptr [edx + eax*4 + 8]
            fadd dword ptr [ecx + 0x58]
            fstp dword ptr [ecx + 0x58]
            fld dword ptr [esp + 0x10]
            mov eax, dword ptr [esi + 0x48]
            fsub st(0), st(1)
            mov dword ptr [esi + 0xe4], eax
            fstp dword ptr [esp + 0x10]
            fstp st(0)
            jmp L0044eae8
    L0044ea86:
            push offset gDefault_drone_direction+0x234
            call DoNotDprintf
            add esp, 4
            mov ecx, esi
            call AdvanceDroneSection
            test eax, eax
            je L0044eb05
            mov ecx, esi
            call CalculateDrivingInfo
            mov eax, dword ptr [esi + 0x10]
            mov ecx, dword ptr [eax + 4]
            mov dword ptr [esp + 0x28], ecx
            mov edx, dword ptr [eax + 8]
            mov ecx, dword ptr [esi + 4]
            mov dword ptr [esp + 0x2c], edx
            mov eax, dword ptr [eax + 0xc]
            mov dword ptr [esp + 0x30], eax
            mov al, byte ptr [ecx + 0x2c]
            test al, bl
            je L0044eacf
            mov dword ptr [esp + 0x2c], 0
    L0044eacf:
            mov ecx, dword ptr [esi + 0xec]
            lea edx, [esp + 0x28]
            call PointActorAlongThisBloodyVector
            mov dword ptr [esi + 0xdc], 1
    L0044eae8:
            fld dword ptr [esp + 0x10]
            fcomp dword ptr [gDrone_render_zero]
            fnstsw ax
            test ah, 0x41
            je L0044e576
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x3c
            ret
    L0044eb05:
            mov edx, dword ptr [esi + 0x38]
            mov eax, dword ptr [esi]
            push 4
            push edx
            push eax
            push offset gDefault_drone_direction+0x138
            call DoNotDprintf
            mov eax, dword ptr [esi + 0x38]
            add esp, 0x10
            cmp eax, 4
            je L0044eb62
            mov eax, dword ptr [eax*4 + gDrone_state_functions]
            cmp eax, ebp
            je L0044eb37
            mov edx, 4
            mov ecx, esi
            call eax
    L0044eb37:
            mov ecx, dword ptr [gTime_stamp_for_this_munging]
            mov edx, dword ptr [esi + 0x3c]
            mov eax, dword ptr [esi + 0x38]
            mov dword ptr [esi + 0x30], ecx
            mov dword ptr [esi + 0x40], edx
            mov dword ptr [esi + 0x3c], eax
            mov dword ptr [esi + 0x38], 4
            mov eax, dword ptr [gDrone_state_functions+0x10]
            cmp eax, ebp
            je L0044eb62
            xor edx, edx
            mov ecx, esi
            call eax
    L0044eb62:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x3c
            ret
    }
#endif
}

void C2_HOOK_FASTCALL InitDroneDrivingInfo(tDrone_spec* pDrone) {

    pDrone->field_0xdc = 0;
    pDrone->field_0xe4 = 0.f;
    pDrone->field_0x48 = 0.f;
    pDrone->field_0x4c = 0.f;
}

void C2_HOOK_FASTCALL PossiblyPipeDroneMovement(tDrone_spec* pDrone) {

    switch (pDrone->field_0xdc) {
    case 1:
        PipeSingleDroneStraightPos(pDrone,
            (undefined4)(uintptr_t)pDrone->field_0x10,
            DRScalarToU16(pDrone->field_0x48, -500.f, 500.f),
            DRScalarToU16(pDrone->field_0x4c, 0.f, 255.f));
        break;
    case 2:
        PipeDroneMatrix(pDrone);
        break;
    default:
        PDEnterDebugger("Keith");
        break;
    }
}

// FUNCTION: CARMA2_HW 0x0044d1d0
void C2_NAKED C2_HOOK_FASTCALL DroneStateFuncControlledMovement(tDrone_spec* pDrone, tDroneStateFuncState state) {
#ifndef CARPOCALYPSE2_MATCHING

    switch (state) {
    case eDrone_state_START:
        DoNotDprintf("DroneStateFuncControlledMovement() START");
        InitDroneDrivingInfo(pDrone);
        break;
    case eDrone_state_RUN:
        DoNotDprintf("DroneStateFuncControlledMovement() RUN");
        if (pDrone->field_0x44) {
            if (pDrone->form->type == kDroneType_plane) {
                MoveThisDronePlane(pDrone);
            } else {
                MoveThisDroneCar(pDrone);
            }
            PossiblyPipeDroneMovement(pDrone);
            return;
        }
        break;
    default:
        DoNotDprintf("DroneStateFuncControlledMovement() DEFAULT");
        break;
    }
#else
    __asm {
            push ebx
            mov eax, edx
            xor ebx, ebx
            push esi
            sub eax, ebx
            mov esi, ecx
            je L0044d27a
            dec eax
            je L0044d1f3
            push offset gDefault_drone_direction+0x208
            call DoNotDprintf
            add esp, 4
            pop esi
            pop ebx
            ret
    L0044d1f3:
            push offset gDefault_drone_direction+0x1e0
            call DoNotDprintf
            mov al, byte ptr [esi + 0x44]
            add esp, 4
            cmp al, bl
            je L0044d299
            mov eax, dword ptr [esi + 4]
            mov ecx, esi
            mov eax, dword ptr [eax + 0x10]
            dec eax
            je L0044d21d
            call MoveThisDroneCar
            jmp L0044d222
    L0044d21d:
            call MoveThisDronePlane
    L0044d222:
            mov eax, dword ptr [esi + 0xdc]
            cmp eax, 1
            jne L0044d25e
            mov ecx, dword ptr [esi + 0x4c]
            push 0x437f0000
            push ebx
            push ecx
            call DRScalarToU16
            mov edx, dword ptr [esi + 0x48]
            push eax
            push 0x43fa0000
            push 0xc3fa0000
            push edx
            call DRScalarToU16
            mov edx, dword ptr [esi + 0x10]
            mov ecx, esi
            push eax
            call PipeSingleDroneStraightPos
            pop esi
            pop ebx
            ret
    L0044d25e:
            cmp eax, 2
            jne L0044d26d
            mov ecx, esi
            call PipeDroneMatrix
            pop esi
            pop ebx
            ret
    L0044d26d:
            mov ecx, offset gDefault_drone_direction+0x1d8
            call PDEnterDebugger
            pop esi
            pop ebx
            ret
    L0044d27a:
            push offset gDefault_drone_direction+0x1ac
            call DoNotDprintf
            add esp, 4
            mov dword ptr [esi + 0xdc], ebx
            mov dword ptr [esi + 0xe4], ebx
            mov dword ptr [esi + 0x48], ebx
            mov dword ptr [esi + 0x4c], ebx
    L0044d299:
            pop esi
            pop ebx
            ret
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0044eb70
void C2_HOOK_FASTCALL DroneStateFuncPhysicsActive(tDrone_spec* pDrone, tDroneStateFuncState state) {

    if (state != eDrone_state_RUN) {
        return;
    }
    DoNotDprintf("DroneStateFuncPhysicsActive() RUN");
    if (pDrone->field_0x44) {
        if (!(pDrone->form->flags & 0x4)) {
            return;
        }
        if (pDrone->field_0x45) {
            return;
        }
    }
    if (!(pDrone->form->flags & 0x2)) {
        return;
    }
    PHILRemoveObject(&pDrone->collision_info);
    pDrone->collision_info.field_0x1a0 = 0x100000;
    PHILSetObjectProperty(&pDrone->collision_info, 3, 1);
    pDrone->field_0x5d4 = 0;
    NewDroneState(pDrone, 1);
}

// FUNCTION: CARMA2_HW 0x0044ec50
void C2_HOOK_FASTCALL DroneStateFuncStationaryPassive(tDrone_spec* pDrone, tDroneStateFuncState state) {

    if (state != eDrone_state_RUN) {
        return;
    }
    DoNotDprintf(" DRONE PATHS");
    if (pDrone->field_0x44) {
        if (!(pDrone->form->flags & 0x4)) {
            return;
        }
        if (pDrone->field_0x45) {
            return;
        }
    }
    if (!(pDrone->form->flags & 0x2)) {
        return;
    }
    DoNotDprintf("naryPassive() RUN", pDrone->id, pDrone->current_state, 1);
    if (pDrone->current_state != 1) {
        if (gDrone_state_functions[pDrone->current_state] != NULL) {
            gDrone_state_functions[pDrone->current_state](pDrone, eDrone_state_STOP);
        }
        pDrone->time_last_munge = gTime_stamp_for_this_munging;
        pDrone->prev_states[1] = pDrone->prev_states[0];
        pDrone->prev_states[0] = pDrone->current_state;
        pDrone->current_state = 1;
        if (gDrone_state_functions[1] != NULL) {
            gDrone_state_functions[1](pDrone, eDrone_state_START);
        }
    }
}

// FUNCTION: CARMA2_HW 0x0044ca90
int C2_HOOK_FASTCALL DroneCarWithinRenderingDistance(const br_vector3* pPos) {

    return fabsf(gRender_bounds_centre.v[0] - pPos->v[0]) < 15.f
        && fabsf(gRender_bounds_centre.v[1] - pPos->v[1]) < 12.f
        && fabsf(gRender_bounds_centre.v[2] - pPos->v[2]) < 15.f;
}

// FUNCTION: CARMA2_HW 0x0044cae0
int C2_HOOK_FASTCALL DronePlaneWithinRenderingDistance(const br_vector3* pPos) {

    return fabsf(gRender_bounds_centre.v[0] - pPos->v[0]) < 40.f
           && fabsf(gRender_bounds_centre.v[1] - pPos->v[1]) < 30.f
           && fabsf(gRender_bounds_centre.v[2] - pPos->v[2]) < 40.f;
}

// FUNCTION: CARMA2_HW 0x0044cb30
int C2_HOOK_FASTCALL DroneTrainWithinRenderingDistance(const br_vector3* pPos) {

    return fabsf(gRender_bounds_centre.v[0] - pPos->v[0]) < 50.f
           && fabsf(gRender_bounds_centre.v[1] - pPos->v[1]) < 20.f
           && fabsf(gRender_bounds_centre.v[2] - pPos->v[2]) < 50.f;

}

// FUNCTION: CARMA2_HW 0x0044cb80
int C2_HOOK_FASTCALL DroneCarWithinProcessingDistance(const br_vector3* pPos) {

    return fabsf(gRender_bounds_centre.v[0] - pPos->v[0]) < 18.f
           && fabsf(gRender_bounds_centre.v[1] - pPos->v[1]) < 13.f
           && fabsf(gRender_bounds_centre.v[2] - pPos->v[2]) < 18.f;
}

// FUNCTION: CARMA2_HW 0x0044cbd0
int C2_HOOK_FASTCALL DronePlaneWithinProcessingDistance(const br_vector3* pPos) {

    return fabsf(gRender_bounds_centre.v[0] - pPos->v[0]) < 43.f
           && fabsf(gRender_bounds_centre.v[1] - pPos->v[1]) < 31.f
           && fabsf(gRender_bounds_centre.v[2] - pPos->v[2]) < 43.f;
}

// FUNCTION: CARMA2_HW 0x0044cc20
int C2_HOOK_FASTCALL DroneTrainWithinProcessingDistance(const br_vector3* pPos) {

    return fabsf(gRender_bounds_centre.v[0] - pPos->v[0]) < 43.f
           && fabsf(gRender_bounds_centre.v[1] - pPos->v[1]) < 31.f
           && fabsf(gRender_bounds_centre.v[2] - pPos->v[2]) < 43.f;

}

// FUNCTION: CARMA2_HW 0x00451bd0
void C2_NAKED C2_HOOK_FASTCALL CalcRenderBoundsCentre(void) {
#ifndef CARPOCALYPSE2_MATCHING
    br_vector3 tv;

    BrVector3Negate(&tv, (br_vector3 *) gCamera_to_world.m[2]);
    BrVector3Normalise(&tv, &tv);
    BrVector3Scale(&tv, &tv, 5.f);
    BrVector3Add(&gRender_bounds_centre, (br_vector3 *) gCamera_to_world.m[3], &tv);
#else
    __asm {
            fld dword ptr [gCamera_to_world+0x18]
            fld dword ptr [gCamera_to_world+0x1c]
            fld dword ptr [gCamera_to_world+0x20]
            fxch st(2)
            fchs
            fxch st(1)
            fchs
            fld st(1)
            fmul st(0), st(2)
            fld st(1)
            fmul st(0), st(2)
            fxch st(4)
            fchs
            fld st(0)
            fmul st(0), st(1)
            fxch st(5)
            faddp st(2), st(0)
            fxch st(4)
            faddp st(1), st(0)
            fsqrt
            fcom dword ptr [gDrone_select_max_distance_squared+0x10]
            fxch st(2)
            fxch st(3)
            fxch st(1)
            fxch st(2)
            fnstsw ax
            test ah, 0x41
            jne L00451c2d
            fdivr qword ptr [gDrone_select_max_distance_squared+0x14]
            fld st(0)
            fld st(1)
            fxch st(1)
            fmulp st(5), st(0)
            fmulp st(3), st(0)
            fmulp st(1), st(0)
            jmp L00451c47
    L00451c2d:
            fstp st(0)
            fstp st(0)
            fstp st(0)
            fstp st(0)
            fld dword ptr [gDrone_render_zero+0x4]
            fld dword ptr [gDrone_render_zero]
            fld dword ptr [gDrone_render_zero]
    L00451c47:
            fld st(0)
            fxch st(3)
            fmul dword ptr [gDrone_select_max_distance_squared+0x8c]
            fxch st(2)
            fmul dword ptr [gDrone_select_max_distance_squared+0x8c]
            fxch st(3)
            fmul dword ptr [gDrone_select_max_distance_squared+0x8c]
            fxch st(2)
            fadd dword ptr [gCamera_to_world+0x24]
            fxch st(3)
            fadd dword ptr [gCamera_to_world+0x28]
            fxch st(2)
            fadd dword ptr [gCamera_to_world+0x2c]
            fxch st(3)
            fstp dword ptr [gRender_bounds_centre]
            fxch st(1)
            fstp dword ptr [gRender_bounds_centre+0x4]
            fxch st(1)
            fstp dword ptr [gRender_bounds_centre+0x8]
            fstp st(0)
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
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0044cfd0
void C2_HOOK_FASTCALL PipeDroneMatrix(tDrone_spec* pDrone) {
    tCompressed_matrix34 compressed_mat;
    br_scalar radius;
    br_scalar speed;

    BrVector3Set(&compressed_mat.m[0],
        DRScalarToU16(pDrone->actor->t.t.mat.m[0][0], -1.f, 1.f),
        DRScalarToU16(pDrone->actor->t.t.mat.m[0][1], -1.f, 1.f),
        DRScalarToU16(pDrone->actor->t.t.mat.m[0][2], -1.f, 1.f));
    BrVector3Set(&compressed_mat.m[1],
        DRScalarToU16(pDrone->actor->t.t.mat.m[1][0], -1.f, 1.f),
        DRScalarToU16(pDrone->actor->t.t.mat.m[1][1], -1.f, 1.f),
        DRScalarToU16(pDrone->actor->t.t.mat.m[1][2], -1.f, 1.f));
    BrVector3Set(&compressed_mat.m[2],
        DRScalarToU16(pDrone->actor->t.t.mat.m[2][0], -1.f, 1.f),
        DRScalarToU16(pDrone->actor->t.t.mat.m[2][1], -1.f, 1.f),
        DRScalarToU16(pDrone->actor->t.t.mat.m[2][2], -1.f, 1.f));
    BrVector3Set(&compressed_mat.m[3],
        DRScalarToU16(pDrone->actor->t.t.mat.m[3][0], -1000.f, 1000.f),
        DRScalarToU16(pDrone->actor->t.t.mat.m[3][1], -1000.f, 1000.f),
        DRScalarToU16(pDrone->actor->t.t.mat.m[3][2], -1000.f, 1000.f));

    radius = MIN(pDrone->h_radius, 100.f);
    speed = pDrone->left_turn ? pDrone->field_0x48 : -pDrone->field_0x48;
    PipeSingleDroneCornerPos(pDrone, DRScalarToU16(speed, -500.f, 500.f), DRScalarToU16(pDrone->field_0x74, 0.f, 1.f), DRScalarToU16(radius, 0.f, 100.f), &compressed_mat);
}

// FUNCTION: CARMA2_HW 0x00452680
int C2_HOOK_FASTCALL OKToViewDrones(void) {
    return 0;
}

// FUNCTION: CARMA2_HW 0x00452690
br_matrix34* C2_HOOK_FASTCALL GetCurrentViewDroneMat(void) {
    return &gDrone_specs[gCurrent_selected_drone].actor->t.t.mat;
}

// FUNCTION: CARMA2_HW 0x00451810
void C2_HOOK_FASTCALL StartRenderingThisDrone(tDrone_spec* pDrone) {

    if (pDrone->actor->render_style == BR_RSTYLE_FACES) {
        return;
    }
    if (gShow_drone_paths && pDrone->actor->render_style == BR_RSTYLE_BOUNDING_FACES) {
        return;
    }
    if (gShow_drone_paths || (pDrone->field_0x44 && gCount_rendered_drones < 10)) {

        DoNotDprintf("START RENDERING: %d", pDrone->id);
        gCount_rendered_drones += 1;
        pDrone->actor->render_style = BR_RSTYLE_FACES;
        PipeSingleDroneRender(pDrone, 1);
    } else {
        NewDroneState(pDrone,1);
    }
}

// FUNCTION: CARMA2_HW 0x004526c0
br_vector3* C2_HOOK_FASTCALL GetCurrentViewDroneDirection(void) {

    StartRenderingThisDrone(&gDrone_specs[gCurrent_selected_drone]);
    if (BrVector3LengthSquared(&gDrone_specs[gCurrent_selected_drone].field_0x18) > gDrone_render_zero) {
        return &gDrone_specs[gCurrent_selected_drone].field_0x18;
    } else {
        return &gDefault_drone_direction;
    }
}

// FUNCTION: CARMA2_HW 0x00452410
void C2_HOOK_FASTCALL MoveMyDroneBaby(tPhysics_object* pObject, undefined4 pArg2) {
    tDrone_spec* pDrone;

    pDrone = (tDrone_spec*)pObject->owner;
    DoNotDprintf("MoveMyDroneBaby(): Drone %d, State %d", pDrone->id, pDrone->current_state);
}

// DoNotDprintf

// CrappyLittleVector3DPrintf

// CrappyLittleDrivingStateInfoDprintf

// DroneStraightestArcForCorner2D

// OpenDroneFile

// LoadDroneTypeInfo

// ReallyRemoveDroneFromPHIL

// RemoveDroneFromPHIL

// InitDroneCollisionObject

// ReallyAddDroneToPHIL

// AddDroneToPHIL

// PreprocessDronePaths

// LinkyCallback

// LinkDroneActorsToModelsAndSetRenderStyle

// LoadDroneActorsModels

// ReadPastBoundsShapesHeader

// ReadPastThisLine

// MatchFGType

// GetAxisFromString

// GetSpeedControlFromString

// MrFindy

// FindDroneChildActor

// GetReversenessFromString

// LoadFunksAndGrooves

// LoadDrone

// CalculateDrivingInfo

// PauseDroneState

// UnPauseDroneState

// NewDroneState

// ResetDroneCrushyModel

// ResetDroneModel

// DroneCarWithinRenderingDistance

// DronePlaneWithinRenderingDistance

// DroneTrainWithinRenderingDistance

// SemiInitDroneSpec

// InitDroneSpec

// ChooseNextNode

// DroneOrientationChanged

// DroneyObjectInSensiblePlace

// StopRenderingThisDrone

// StopProcessingThisDrone

// StartRenderingThisDrone

// CheckDroneInSensiblePlaceBeforeStartingToProcessTheCuntingThing

// StartProcessingThisDrone

// DroneCarWithinProcessingDistance

// DronePlaneWithinProcessingDistance

// DroneTrainWithinProcessingDistance

// CalcRenderBoundsCentre

// DoDroneDistanceChecks

// DoDroneFunkyGroovyThings

// ProcessThisDrone

// DroneStateFuncReset

// InitDroneDrivingInfo

// MoveThisDronePlane

// MoveThisDroneCar

// PipeDroneMatrix

// PossiblyPipeDroneMovement

// DroneStateFuncControlledMovement

// DoDroneHaltingStuff

// DroneStateFuncPhysicsActive

// DroneStateFuncStationaryPassive

// GetNewCrushyModelForDrone

// FUNCTION: CARMA2_HW 0x0044ed00
int C2_HOOK_FASTCALL GetDronesOn(void) {
    return !gTraffic_disabled;
}

// FUNCTION: CARMA2_HW 0x0044ecf0
void C2_HOOK_FASTCALL SetTrafficOn(int pTraffic_on) {
    gTraffic_disabled = !pTraffic_on;
}

// OtherPerambulatoryInstasificationIsEnablificated

// LoadPanGameDroneInfo

// InitDrones

// DisposeDronesRaceStuff

// LoadPerRaceDroneStuff

// GetDroneCrushData

// AllocateAndInitDrones

// LoadInDronePaths

// FUNCTION: CARMA2_HW 0x004512d0
void C2_HOOK_FASTCALL DronePreCollisionStuff(void) {
    gFrame += 1;
    gINT_00681fb0 = 0;
}

// DoDronePerGameFrameStuff

// FUNCTION: CARMA2_HW 0x00452230
int C2_HOOK_FASTCALL MyDroneHathCollideth(tPhysics_object* pObject, tPhysics_object* pOther) {
    tDrone_spec* pDrone;

    DoNotDprintf("MyDroneHathCollideth(): PHIL called a bloody callback. Rejoice!");
    pDrone = (tDrone_spec*)pObject->owner;
    pObject->disable_move_rotate = 0;
    pObject->field_0xed = 1;
    pObject->field_0x1a0 = 0x200000;
    pObject->v.v[0] = pDrone->field_0x18.v[0];
    pObject->v.v[1] = pDrone->field_0x18.v[1];
    pObject->v.v[2] = pDrone->field_0x18.v[2];
    PHILMakeObjectActive(pObject, NULL, NULL, 1);
    PHILSetObjectProperty(pObject, 3, 0);
    if ((pOther != NULL && pOther->owner != NULL && pOther->flags_0x238 == 1)
            || (pOther->flags_0x238 == 0x20
                && (gMutant_tail_state == 2 || GetTotalTime() - gINT_00705b70 < 0x1d4c))) {
        ((tDrone_spec*)pObject->owner)->field_0x5d4 = (undefined4)pOther->owner;
    } else if (pOther != NULL && pOther->flags_0x238 == 0x40) {
        ((tDrone_spec*)pObject->owner)->field_0x5d4 = ((tDrone_spec*)pOther->owner)->time_last_munge;
    }
    NewDroneState((tDrone_spec*)pObject->owner, 3);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00452360
int C2_HOOK_FASTCALL MyDroneHathHalteth(tPhysics_object* pObject) {
    tDrone_spec* pDrone;

    pDrone = (tDrone_spec*)pObject->owner;
    DoNotDprintf("MyDroneHathHalteth(): PHIL called a bloody callback. Rejoice!");
    pDrone->collision_info.field_0x1a0 = 0x100000;
    PHILSetObjectProperty(&pDrone->collision_info, 3, 1);
    pDrone->field_0x5d4 = 0;
    NewDroneState((tDrone_spec*)pObject->owner, 4);
    return 1;
}

// MoveMyDroneBaby

// DroneLookWhereYoureGoingYouBlindCunt

// OKToViewDrones

// GetCurrentViewDroneMat

// GetCurrentViewDroneDirection

// ApplyDroneRender2

// ApplyDroneStraightPos2

// ApplyDroneCornerPos2

// ReallocModelFacesAndVertices

// CalcNFacesAndVerticesInEditModel

// InitialiseEditModelsEtc

// UpdateEditModels

// FUNCTION: CARMA2_HW 0x004525d0
void C2_HOOK_FASTCALL SelectNextDrone(void) {
    char text[64];

    gCurrent_selected_drone++;
    if (gCurrent_selected_drone >= gCount_drones) {
        gCurrent_selected_drone = 0;
    }
    sprintf(text, "Drone %d", gCurrent_selected_drone);
    NewTextHeadupSlot(4, 0, 3000, -1, text);
}

// FUNCTION: CARMA2_HW 0x00452620
void C2_HOOK_FASTCALL SelectPreviousDrone(void) {
    char text[64];

    if (gCount_drones != 0) {
        gCurrent_selected_drone--;
        if (gCurrent_selected_drone < 0) {
            gCurrent_selected_drone = gCount_drones - 1;
        }
    }
    sprintf(text, "Drone %d", gCurrent_selected_drone);
    NewTextHeadupSlot(4, 0, 3000, -1, text);
}

// FUNCTION: CARMA2_HW 0x004532e0
void C2_NAKED C2_HOOK_FASTCALL SelectNearestDrone(void) {
#ifndef CARPOCALYPSE2_MATCHING
    float closest_distance;
    int nearest_index;
    int i;

    closest_distance = 3.4028235e+38f;
    nearest_index = 0;
    if (gCount_drones > 0) {
        br_vector3* car_position = &gProgram_state.current_car.car_master_actor->t.t.translate.t;

        for (i = 0; i < gCount_drones; i++) {
            br_vector3* drone_position = &gDrone_specs[i].actor->t.t.translate.t;
            float delta[3];
            float distance;

            delta[0] = car_position->v[0] - drone_position->v[0];
            delta[1] = car_position->v[1] - drone_position->v[1];
            delta[2] = car_position->v[2] - drone_position->v[2];
            distance = delta[1] * delta[1] + delta[0] * delta[0] + delta[2] * delta[2];
            if (distance < closest_distance) {
                nearest_index = i;
                closest_distance = distance;
            }
        }
    }
    if (closest_distance < gDrone_select_max_distance_squared) {
        if (gSelected_drone_path_index >= 0) {
            gDrone_specs[gSelected_drone_path_index].actor->render_style = BR_RSTYLE_FACES;
        }
        gSelected_drone_path_index = nearest_index;
        sprintf(gDrone_selection_buffer, "Selected drone #%d", nearest_index);
        NewTextHeadupSlot(4, 0, 3000, -1, gDrone_selection_buffer);
        for (i = 0; i < gCount_drone_forms; i++) {
            if (gDrone_specs[gSelected_drone_path_index].form == &gDrone_forms[i]) {
                gSelected_drone_form_index = i;
                return;
            }
        }
    } else {
        NewTextHeadupSlot(4, 0, 3000, -1, "No drones close enough");
    }
#else
    __asm {
            sub esp, 0x10
            push ebx
            push ebp
            mov ebp, dword ptr [gDrone_specs]
            push esi
            push edi
            mov edi, dword ptr [gCount_drones]
            xor ecx, ecx
            mov dword ptr [esp + 0x10], 0x7f7fffff
            test edi, edi
            jle L0045336f
            mov esi, dword ptr [gProgram_state+0xbc]
            mov ebx, dword ptr [esp + 0x10]
            lea edx, [ebp + 0xec]
    L00453311:
            fld dword ptr [esi + 0x54]
            mov eax, dword ptr [edx]
            fld dword ptr [esi + 0x50]
            fsub dword ptr [eax + 0x50]
            fxch st(1)
            fsub dword ptr [eax + 0x54]
            fld dword ptr [esi + 0x58]
            fsub dword ptr [eax + 0x58]
            fxch st(2)
            fst dword ptr [esp + 0x14]
            fxch st(1)
            fst dword ptr [esp + 0x18]
            fxch st(1)
            fmul dword ptr [esp + 0x14]
            fxch st(1)
            fmul dword ptr [esp + 0x18]
            fld st(2)
            fmul st(0), st(3)
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fstp st(1)
            fld st(0)
            fcomp dword ptr [esp + 0x10]
            fnstsw ax
            test ah, 1
            je L00453360
            fstp dword ptr [esp + 0x10]
            mov ebx, ecx
            jmp L00453362
    L00453360:
            fstp st(0)
    L00453362:
            inc ecx
            add edx, 0x5d8
            cmp ecx, edi
            jl L00453311
            jmp L00453373
    L0045336f:
            mov ebx, dword ptr [esp + 0x10]
    L00453373:
            fld dword ptr [esp + 0x10]
            fcomp dword ptr [gDrone_select_max_distance_squared]
            fnstsw ax
            test ah, 1
            je L00453437
            mov ecx, dword ptr [gSelected_drone_path_index]
            test ecx, ecx
            jl L004533aa
            mov eax, ecx
            shl eax, 5
            sub eax, ecx
            lea eax, [eax + eax*2]
            lea ecx, [ecx + eax*2]
            mov edx, dword ptr [ebp + ecx*8 + 0xec]
            mov byte ptr [edx + 0x20], 4
    L004533aa:
            fld dword ptr [esp + 0x10]
            sub esp, 8
            mov dword ptr [gSelected_drone_path_index], ebx
            _emit 0xdd
        _emit 0x1c
        _emit 0x24
            push ebx
            push offset gDefault_drone_direction+0xd40
            push offset gDrone_selection_buffer
            call sprintf
            add esp, 0x14
            xor edx, edx
            mov ecx, 4
            push offset gDrone_selection_buffer
            push -1
            push 0xbb8
            call NewTextHeadupSlot
            mov esi, dword ptr [gCount_drone_forms]
            xor eax, eax
            test esi, esi
            jle L0045344f
            mov edx, dword ptr [gSelected_drone_path_index]
            mov ecx, edx
            shl ecx, 5
            sub ecx, edx
            lea ecx, [ecx + ecx*2]
            lea edx, [edx + ecx*2]
            mov ecx, dword ptr [gDrone_specs]
            mov edx, dword ptr [ecx + edx*8 + 4]
            mov ecx, offset gDrone_forms
    L00453413:
            cmp edx, ecx
            je L0045342a
            inc eax
            add ecx, 0x88
            cmp eax, esi
            jl L00453413
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x10
            ret
    L0045342a:
            mov dword ptr [gSelected_drone_form_index], eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x10
            ret
    L00453437:
            push offset gDefault_drone_direction+0xd28
            push -1
            push 0xbb8
            xor edx, edx
            mov ecx, 4
            call NewTextHeadupSlot
    L0045344f:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x10
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00453460
void C2_NAKED C2_HOOK_FASTCALL ShowDronePaths(void) {
#ifndef CARPOCALYPSE2_MATCHING
    tDrone_spec* drone;

    if (gSelected_drone_path_index >= 0) {
        gDrone_specs[gSelected_drone_path_index].actor->render_style = 4;
        gSelected_drone_path_index = -1;
    }
    if (!gShow_drone_paths) {
        if (!InitialiseEditModelsEtc()) {
            NewTextHeadupSlot(4, 0, 5000, -1, "Drone Path Error - Can't allocate actors/models");
            return;
        }
        gShow_drone_paths = 1;
        PutOpponentsInNeutral();
        for (drone = gDrone_specs; drone < gDrone_specs + gCount_drones; drone++) {
            if (drone->field_0x44) {
                drone->field_0x44 = 0;
                gCount_active_drones--;
                if (drone->actor->render_style == 4) {
                    DoNotDprintf("STOP RENDERING: %d", drone->id);
                    gCount_rendered_drones--;
                    drone->actor->render_style = 1;
                    PipeSingleDroneRender(drone, 0);
                }
                PHILRemoveObject(&drone->collision_info);
                if (gDrone_state_functions[drone->current_state] != NULL) {
                    gDrone_state_functions[drone->current_state](drone, 2);
                }
                DoNotDprintf("PROCESSING OFF: Frame %d, Drone %d, state %d", gFrame, drone->id, drone->current_state);
                CrappyLittleVector3DPrintf("     Pos", (br_vector3*)&drone->actor->t.t.mat.m[3]);
            }
            DoNotDprintf("NewDroneState() Drone %d, Current state %d, New state %d", drone->id, drone->current_state, 1);
            if (drone->current_state != 1) {
                if (gDrone_state_functions[drone->current_state] != NULL) {
                    gDrone_state_functions[drone->current_state](drone, eDrone_state_STOP);
                }
                drone->time_last_munge = gTime_stamp_for_this_munging;
                drone->prev_states[1] = drone->prev_states[0];
                drone->prev_states[0] = drone->current_state;
                drone->current_state = 1;
                if (gDrone_state_functions[eDrone_state_RUN] != NULL) {
                    gDrone_state_functions[eDrone_state_RUN](drone, eDrone_state_START);
                }
            }
        }
        NewTextHeadupSlot(4, 0, 3000, -1, "Showing Drone Paths");
        SelectNearestDrone();
    } else {
        NewTextHeadupSlot(4, 0, 3000, -1, "Already Showing Drone Paths");
    }
#else
    __asm {
            mov ecx, dword ptr [gSelected_drone_path_index]
            push ebx
            push ebp
            push esi
            test ecx, ecx
            push edi
            jl L00453496
            mov eax, ecx
            mov edx, dword ptr [gDrone_specs]
            shl eax, 5
            sub eax, ecx
            lea eax, [eax + eax*2]
            lea ecx, [ecx + eax*2]
            mov eax, dword ptr [edx + ecx*8 + 0xec]
            mov byte ptr [eax + 0x20], 4
            mov dword ptr [gSelected_drone_path_index], 0xffffffff
    L00453496:
            mov eax, dword ptr [gShow_drone_paths]
            test eax, eax
            jne L0045365a
            call InitialiseEditModelsEtc
            test eax, eax
            jne L004534c9
            push offset gDefault_drone_direction+0xd84
            push -1
            push 0x1388
            xor edx, edx
            mov ecx, 4
            call NewTextHeadupSlot
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret
    L004534c9:
            mov ebx, 1
            mov dword ptr [gShow_drone_paths], ebx
            call PutOpponentsInNeutral
            mov eax, dword ptr [gCount_drones]
            xor ebp, ebp
            test eax, eax
            jle L00453638
            xor edi, edi
    L004534ea:
            mov ecx, dword ptr [gDrone_specs]
            mov al, byte ptr [ecx + edi + 0x44]
            lea esi, [ecx + edi]
            test al, al
            je L004535c2
            mov byte ptr [esi + 0x44], 0
            mov ecx, dword ptr [gCount_active_drones]
            dec ecx
            mov dword ptr [gCount_active_drones], ecx
            mov edx, dword ptr [esi + 0xec]
            cmp byte ptr [edx + 0x20], 4
            jne L00453549
            mov eax, dword ptr [esi]
            push eax
            push offset gDefault_drone_direction+0x108
            call DoNotDprintf
            mov eax, dword ptr [gCount_rendered_drones]
            add esp, 8
            dec eax
            xor edx, edx
            mov dword ptr [gCount_rendered_drones], eax
            mov ecx, dword ptr [esi + 0xec]
            mov byte ptr [ecx + 0x20], bl
            mov ecx, esi
            call PipeSingleDroneRender
    L00453549:
            lea ecx, [esi + 0xf8]
            call PHILRemoveObject
            mov edx, dword ptr [esi + 0x38]
            mov eax, dword ptr [edx*4 + gDrone_state_functions]
            test eax, eax
            je L0045356b
            mov edx, 2
            mov ecx, esi
            call eax
    L0045356b:
            mov eax, dword ptr [esi + 0x38]
            mov ecx, dword ptr [esi]
            mov edx, dword ptr [gFrame]
            push eax
            push ecx
            push edx
            push offset gDefault_drone_direction+0xd8
            call DoNotDprintf
            mov esi, dword ptr [esi + 0xec]
            add esp, 0x10
            add esi, 0x50
            sub esp, 8
            sub esp, 8
            mov eax, dword ptr [gFrame]
            fld dword ptr [esi + 8]
            fstp qword ptr [esp + 8]
            fld dword ptr [esi + 4]
            _emit 0xdd
        _emit 0x1c
        _emit 0x24
            _emit 0xd9
        _emit 0x06
            sub esp, 8
            _emit 0xdd
        _emit 0x1c
        _emit 0x24
            push offset gDefault_drone_direction+0xcc
            push eax
            push offset gDefault_drone_direction+0xb0
            call DoNotDprintf
            add esp, 0x24
    L004535c2:
            mov ecx, dword ptr [gDrone_specs]
            push ebx
            mov edx, dword ptr [ecx + edi + 0x38]
            mov eax, dword ptr [ecx + edi]
            lea esi, [ecx + edi]
            push edx
            push eax
            push offset gDefault_drone_direction+0x138
            call DoNotDprintf
            mov eax, dword ptr [esi + 0x38]
            add esp, 0x10
            cmp eax, ebx
            je L00453624
            mov eax, dword ptr [eax*4 + gDrone_state_functions]
            test eax, eax
            je L004535fd
            mov edx, 4
            mov ecx, esi
            call eax
    L004535fd:
            mov ecx, dword ptr [gTime_stamp_for_this_munging]
            mov edx, dword ptr [esi + 0x3c]
            mov eax, dword ptr [esi + 0x38]
            mov dword ptr [esi + 0x30], ecx
            mov dword ptr [esi + 0x40], edx
            mov dword ptr [esi + 0x3c], eax
            mov dword ptr [esi + 0x38], ebx
            mov eax, dword ptr [gDrone_state_functions+0x4]
            test eax, eax
            je L00453624
            xor edx, edx
            mov ecx, esi
            call eax
    L00453624:
            mov eax, dword ptr [gCount_drones]
            inc ebp
            add edi, 0x5d8
            cmp ebp, eax
            jl L004534ea
    L00453638:
            push offset gDefault_drone_direction+0xd70
            push -1
            push 0xbb8
            xor edx, edx
            mov ecx, 4
            call NewTextHeadupSlot
            call SelectNearestDrone
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret
    L0045365a:
            push offset gDefault_drone_direction+0xd54
            push -1
            push 0xbb8
            xor edx, edx
            mov ecx, 4
            call NewTextHeadupSlot
            pop edi
            pop esi
            pop ebp
            pop ebx
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
            mov ecx, dword ptr [gSelected_drone_path_index]
            test ecx, ecx
            jl L004536b2
            mov eax, ecx
            mov edx, dword ptr [gDrone_specs]
            shl eax, 5
            sub eax, ecx
            lea eax, [eax + eax*2]
            lea ecx, [ecx + eax*2]
            mov eax, dword ptr [edx + ecx*8 + 0xec]
            mov byte ptr [eax + 0x20], 4
            mov dword ptr [gSelected_drone_path_index], 0xffffffff
    L004536b2:
            mov eax, dword ptr [gShow_drone_paths]
            test eax, eax
            je L004536fd
            mov eax, dword ptr [gStatic_drone_path_model_actor]
            mov cl, 1
            test eax, eax
            je L004536c9
            mov byte ptr [eax + 0x20], cl
    L004536c9:
            mov eax, dword ptr [gDrone_path_model_actor]
            test eax, eax
            je L004536d5
            mov byte ptr [eax + 0x20], cl
    L004536d5:
            call WriteOutDronePaths
            xor edx, edx
            mov ecx, 4
            push offset gDefault_drone_direction+0xdd0
            push -1
            push 0xbb8
            mov dword ptr [gShow_drone_paths], 0
            call NewTextHeadupSlot
            ret
    L004536fd:
            push offset gDefault_drone_direction+0xdb4
            push -1
            push 0xbb8
            xor edx, edx
            mov ecx, 4
            call NewTextHeadupSlot
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
            nop
            mov eax, dword ptr [gShow_drone_paths]
            push esi
            test eax, eax
            jne L00453744
            push offset gDefault_drone_direction+0xde4
            push -1
            push 0xbb8
            xor edx, edx
            mov ecx, 4
            call NewTextHeadupSlot
            pop esi
            ret
    L00453744:
            mov ecx, 1
            call L004538a0
            test eax, eax
            je L004537cc
            mov eax, dword ptr [gProgram_state+0xbc]
            push 0x40000000
            lea ecx, [eax + 0x50]
            call L004537d0
            test eax, eax
            jl L004537cc
            mov ecx, dword ptr [gSelected_drone_path_node_index]
            cmp eax, ecx
            je L004537cc
            lea edx, [ecx + ecx*8]
            mov esi, dword ptr [gDrone_path_nodes]
            lea edx, [ecx + edx*2]
            lea ecx, [ecx + edx*4]
            shl ecx, 2
            mov dx, word ptr [esi + ecx + 0x12c]
            cmp dx, 8
            jae L004537b0
            and edx, 0xffff
            lea edx, [edx + edx*8]
            lea edx, [esi + edx*4]
            mov word ptr [edx + ecx + 0xc], ax
            mov eax, dword ptr [gDrone_path_nodes]
            inc word ptr [eax + ecx + 0x12c]
    L004537b0:
            call InitialiseEditModelsEtc
            mov eax, dword ptr [gDrone_path_model_actor]
            mov dword ptr [gDrone_paths_elasticating], 0
            test eax, eax
            je L004537cc
            mov byte ptr [eax + 0x20], 1
    L004537cc:
            pop esi
            ret
            nop
            nop
    L004537d0:
            sub esp, 0x10
            fld dword ptr [gDrone_select_max_distance_squared+0xb0]
            fld dword ptr [esp + 0x14]
            fmul dword ptr [esp + 0x14]
            push ebx
            push esi
            push edi
            mov edi, dword ptr [gCount_drone_path_nodes]
            or ebx, 0xffffffff
            xor edx, edx
            fstp dword ptr [esp + 0xc]
            test edi, edi
            jle L00453868
            mov eax, dword ptr [gDrone_path_nodes]
            lea esi, [eax + 8]
    L004537ff:
            fld dword ptr [ecx + 4]
            _emit 0xd9
        _emit 0x01
            fsub dword ptr [esi - 8]
            fxch st(1)
            fsub dword ptr [esi - 4]
            fld dword ptr [ecx + 8]
            _emit 0xd8
        _emit 0x26
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
            fstp dword ptr [esp + 0x20]
            fstp st(0)
            fld dword ptr [esp + 0x20]
            fcomp st(1)
            fnstsw ax
            test ah, 1
            je L0045385d
            fld dword ptr [esp + 0x20]
            fcomp dword ptr [esp + 0xc]
            fnstsw ax
            test ah, 1
            je L0045385d
            fstp st(0)
            fld dword ptr [esp + 0x20]
            mov ebx, edx
    L0045385d:
            inc edx
            add esi, 0x134
            cmp edx, edi
            jl L004537ff
    L00453868:
            test ebx, ebx
            fstp st(0)
            jge L00453886
            push offset gDefault_drone_direction+0xe04
            push -1
            push 0xbb8
            xor edx, edx
            mov ecx, 4
            call NewTextHeadupSlot
    L00453886:
            pop edi
            mov eax, ebx
            pop esi
            pop ebx
            add esp, 0x10
            ret 4
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
            nop
            nop
    L004538a0:
            mov eax, dword ptr [gDrone_paths_elasticating]
            test eax, eax
            je L0045391d
            mov eax, dword ptr [gSelected_drone_path_node_index]
            test ecx, ecx
            lea edx, [eax + eax*8]
            lea edx, [eax + edx*2]
            lea eax, [eax + edx*4]
            mov edx, dword ptr [gDrone_path_nodes]
            lea eax, [edx + eax*4]
            mov edx, dword ptr [gProgram_state+0xbc]
            fld dword ptr [edx + 0x50]
            _emit 0xd8
        _emit 0x20
            fld dword ptr [edx + 0x54]
            fsub dword ptr [eax + 4]
            fld dword ptr [edx + 0x58]
            fsub dword ptr [eax + 8]
            je L00453911
            fld st(2)
            fld st(2)
            fxch st(1)
            fmulp st(4), st(0)
            fld st(1)
            fxch st(1)
            fmul st(0), st(3)
            fxch st(1)
            fmul st(0), st(2)
            fxch st(1)
            faddp st(4), st(0)
            faddp st(3), st(0)
            fxch st(2)
            fsqrt
            fxch st(2)
            fstp st(0)
            fstp st(0)
            fcomp dword ptr [gDrone_select_max_distance_squared+0xb4]
            fnstsw ax
            test ah, 0x41
            jne L00453917
            push offset gDefault_drone_direction+0xe44
            jmp L00453926
    L00453911:
            fstp st(0)
            fstp st(0)
            fstp st(0)
    L00453917:
            mov eax, 1
            ret
    L0045391d:
            test ecx, ecx
            je L00453939
            push offset gDefault_drone_direction+0xe20
    L00453926:
            push -1
            push 0xbb8
            xor edx, edx
            mov ecx, 4
            call NewTextHeadupSlot
    L00453939:
            xor eax, eax
            ret
            nop
            nop
            nop
            nop
            mov eax, dword ptr [gShow_drone_paths]
            push esi
            test eax, eax
            jne L00453964
            push offset gDefault_drone_direction+0xde4
            push -1
            push 0xbb8
            xor edx, edx
            mov ecx, 4
            call NewTextHeadupSlot
            pop esi
            ret
    L00453964:
            mov eax, dword ptr [gDrone_paths_elasticating]
            test eax, eax
            je L00453987
            push offset gDefault_drone_direction+0xe7c
            push -1
            push 0xbb8
            xor edx, edx
            mov ecx, 4
            call NewTextHeadupSlot
            pop esi
            ret
    L00453987:
            call L00453ab0
            test eax, eax
            je L00453aa2
            mov eax, dword ptr [gCount_drone_path_nodes]
            mov esi, eax
            lea ecx, [eax + eax*8]
            lea edx, [eax + ecx*2]
            lea ecx, [eax + edx*4]
            mov edx, dword ptr [gDrone_path_nodes]
            inc eax
            mov dword ptr [gCount_drone_path_nodes], eax
            mov eax, dword ptr [gProgram_state+0xbc]
            lea ecx, [edx + ecx*4]
            mov edx, dword ptr [eax + 0x50]
            mov dword ptr [ecx], edx
            mov eax, dword ptr [gProgram_state+0xbc]
            mov edx, dword ptr [eax + 0x54]
            mov dword ptr [ecx + 4], edx
            mov eax, dword ptr [gProgram_state+0xbc]
            mov edx, dword ptr [eax + 0x58]
            lea eax, [ecx + 0xc]
            mov dword ptr [ecx + 8], edx
            mov edx, 8
    L004539db:
            mov word ptr [eax], 0xffff
            add eax, 0x24
            dec edx
            jne L004539db
            mov byte ptr [ecx + 0x12e], 0
            mov word ptr [ecx + 0x12c], 0
            mov word ptr [ecx + 0x130], 0xffff
            call InitialiseEditModelsEtc
            mov eax, dword ptr [gDrone_path_model_actor]
            test eax, eax
            je L00453a77
            mov ecx, dword ptr [gElasticatey_drone_path_model]
            test ecx, ecx
            je L00453a77
            mov dword ptr [gDrone_paths_elasticating], 1
            mov byte ptr [eax + 0x20], 4
            mov eax, dword ptr [gMat_lt_grn]
            mov ecx, dword ptr [gMat_dk_grn]
            mov edx, dword ptr [gProgram_state+0xbc]
            push eax
            lea eax, [esi + esi*8]
            push ecx
            add edx, 0x50
            mov dword ptr [gSelected_drone_path_node_index], esi
            lea ecx, [esi + eax*2]
            mov eax, dword ptr [gDrone_path_nodes]
            push edx
            lea edx, [esi + ecx*4]
            lea ecx, [eax + edx*4]
            xor edx, edx
            push ecx
            mov ecx, dword ptr [gElasticatey_drone_path_model]
            push 0
            call MakeAISimpleEditSectionHere
            mov edx, dword ptr [gElasticatey_drone_path_model]
            push 0x7fff
            push edx
            call BrModelUpdate
            add esp, 8
    L00453a77:
            push esi
            push offset gDefault_drone_direction+0xe5c
            push offset gDrone_selection_buffer
            call sprintf
            add esp, 0xc
            xor edx, edx
            mov ecx, 4
            push offset gDrone_selection_buffer
            push -1
            push 0x7d0
            call NewTextHeadupSlot
    L00453aa2:
            pop esi
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
            nop
            nop
            nop
    L00453ab0:
            sub esp, 0x18
            push esi
            mov esi, dword ptr [gCount_drone_path_nodes]
            cmp esi, 0xc8
            jl L00453acc
            push offset gDefault_drone_direction+0xeac
            jmp L00453b5a
    L00453acc:
            mov eax, dword ptr [gProgram_state+0xbc]
            mov ecx, dword ptr [eax + 0x50]
            mov edx, dword ptr [eax + 0x54]
            mov eax, dword ptr [eax + 0x58]
            mov dword ptr [esp + 4], ecx
            xor ecx, ecx
            mov dword ptr [esp + 8], edx
            test esi, esi
            mov dword ptr [esp + 0xc], eax
            jle L00453b74
            mov edx, dword ptr [gDrone_path_nodes]
            add edx, 8
    L00453af9:
            fld dword ptr [esp + 8]
            fld dword ptr [esp + 4]
            fsub dword ptr [edx - 8]
            fxch st(1)
            fsub dword ptr [edx - 4]
            fld dword ptr [esp + 0xc]
            _emit 0xd8
        _emit 0x22
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
            fcomp dword ptr [gDrone_render_zero+0xc]
            fnstsw ax
            test ah, 1
            fstp st(0)
            jne L00453b55
            inc ecx
            add edx, 0x134
            cmp ecx, esi
            jl L00453af9
            mov eax, 1
            pop esi
            add esp, 0x18
            ret
    L00453b55:
            push offset gDefault_drone_direction+0xe94
    L00453b5a:
            push -1
            push 0xbb8
            xor edx, edx
            mov ecx, 4
            call NewTextHeadupSlot
            xor eax, eax
            pop esi
            add esp, 0x18
            ret
    L00453b74:
            mov eax, 1
            pop esi
            add esp, 0x18
            ret
            nop
            nop
            mov eax, dword ptr [gShow_drone_paths]
            push esi
            test eax, eax
            jne L00453ba4
            push offset gDefault_drone_direction+0xde4
            push -1
            push 0xbb8
            xor edx, edx
            mov ecx, 4
            call NewTextHeadupSlot
            pop esi
            ret
    L00453ba4:
            xor ecx, ecx
            call L004538a0
            test eax, eax
            je L00453c94
            mov ecx, dword ptr [gCount_drone_path_nodes]
            mov esi, ecx
            lea eax, [ecx + ecx*8]
            lea edx, [ecx + eax*2]
            lea eax, [ecx + edx*4]
            mov edx, dword ptr [gDrone_path_nodes]
            inc ecx
            mov dword ptr [gCount_drone_path_nodes], ecx
            mov ecx, dword ptr [gProgram_state+0xbc]
            lea eax, [edx + eax*4]
            mov edx, dword ptr [ecx + 0x50]
            mov dword ptr [eax], edx
            mov ecx, dword ptr [gProgram_state+0xbc]
            mov edx, dword ptr [ecx + 0x54]
            mov dword ptr [eax + 4], edx
            mov ecx, dword ptr [gProgram_state+0xbc]
            mov edx, dword ptr [ecx + 0x58]
            lea ecx, [eax + 0xc]
            mov dword ptr [eax + 8], edx
            mov edx, 8
    L00453bff:
            mov word ptr [ecx], 0xffff
            add ecx, 0x24
            dec edx
            jne L00453bff
            mov byte ptr [eax + 0x12e], 0
            mov word ptr [eax + 0x12c], 0
            mov word ptr [eax + 0x130], 0xffff
            mov eax, dword ptr [gSelected_drone_path_node_index]
            lea ecx, [eax + eax*8]
            lea edx, [eax + ecx*2]
            lea eax, [eax + edx*4]
            mov edx, dword ptr [gDrone_path_nodes]
            shl eax, 2
            mov cx, word ptr [edx + eax + 0x12c]
            cmp cx, 8
            jae L00453c67
            and ecx, 0xffff
            lea ecx, [ecx + ecx*8]
            lea edx, [edx + ecx*4]
            mov word ptr [edx + eax + 0xc], si
            mov ecx, dword ptr [gDrone_path_nodes]
            inc word ptr [ecx + eax + 0x12c]
    L00453c67:
            push esi
            push offset gDefault_drone_direction+0xe5c
            push offset gDrone_selection_buffer
            call sprintf
            add esp, 0xc
            xor edx, edx
            mov ecx, 4
            push offset gDrone_selection_buffer
            push -1
            push 0x7d0
            call NewTextHeadupSlot
            jmp L00453ce1
    L00453c94:
            mov eax, dword ptr [gProgram_state+0xbc]
            push 0x40000000
            lea ecx, [eax + 0x50]
            call L004537d0
            mov esi, eax
            test esi, esi
            jl L00453ce1
            lea ecx, [esi + esi*8]
            lea edx, [esi + ecx*2]
            mov ecx, dword ptr [gDrone_path_nodes]
            lea eax, [esi + edx*4]
            cmp word ptr [ecx + eax*4 + 0x12c], 8
            jb L00453ce1
            push offset gDefault_drone_direction+0xec8
            push -1
            push 0xbb8
            xor edx, edx
            mov ecx, 4
            call NewTextHeadupSlot
            or esi, 0xffffffff
    L00453ce1:
            call InitialiseEditModelsEtc
            test esi, esi
            jl L00453d5e
            mov eax, dword ptr [gDrone_path_model_actor]
            test eax, eax
            je L00453d5e
            mov ecx, dword ptr [gElasticatey_drone_path_model]
            test ecx, ecx
            je L00453d5e
            mov dword ptr [gDrone_paths_elasticating], 1
            mov byte ptr [eax + 0x20], 4
            mov edx, dword ptr [gMat_lt_grn]
            mov eax, dword ptr [gMat_dk_grn]
            mov ecx, dword ptr [gProgram_state+0xbc]
            push edx
            lea edx, [esi + esi*8]
            push eax
            add ecx, 0x50
            mov dword ptr [gSelected_drone_path_node_index], esi
            lea eax, [esi + edx*2]
            mov edx, dword ptr [gDrone_path_nodes]
            push ecx
            lea ecx, [esi + eax*4]
            lea eax, [edx + ecx*4]
            mov ecx, dword ptr [gElasticatey_drone_path_model]
            push eax
            push 0
            xor edx, edx
            call MakeAISimpleEditSectionHere
            mov ecx, dword ptr [gElasticatey_drone_path_model]
            push 0x7fff
            push ecx
            call BrModelUpdate
            add esp, 8
    L00453d5e:
            pop esi
            ret
            mov eax, dword ptr [gDrone_path_model_actor]
            mov dword ptr [gDrone_paths_elasticating], 0
            test eax, eax
            je L00453d77
            mov byte ptr [eax + 0x20], 1
    L00453d77:
            ret
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            mov eax, dword ptr [gShow_drone_paths]
            push esi
            test eax, eax
            jne L00453da4
            push offset gDefault_drone_direction+0xde4
            push -1
            push 0xbb8
            xor edx, edx
            mov ecx, 4
            call NewTextHeadupSlot
            pop esi
            ret
    L00453da4:
            mov ecx, 1
            call L004538a0
            test eax, eax
            je L00453ec3
            call L00453ab0
            test eax, eax
            je L00453ec3
            mov ecx, dword ptr [gCount_drone_path_nodes]
            mov esi, ecx
            lea eax, [ecx + ecx*8]
            lea edx, [ecx + eax*2]
            lea eax, [ecx + edx*4]
            mov edx, dword ptr [gDrone_path_nodes]
            inc ecx
            mov dword ptr [gCount_drone_path_nodes], ecx
            mov ecx, dword ptr [gProgram_state+0xbc]
            lea eax, [edx + eax*4]
            mov edx, dword ptr [ecx + 0x50]
            mov dword ptr [eax], edx
            mov ecx, dword ptr [gProgram_state+0xbc]
            mov edx, dword ptr [ecx + 0x54]
            mov dword ptr [eax + 4], edx
            mov ecx, dword ptr [gProgram_state+0xbc]
            mov edx, dword ptr [ecx + 0x58]
            lea ecx, [eax + 0xc]
            mov dword ptr [eax + 8], edx
            mov edx, 8
    L00453e0f:
            mov word ptr [ecx], 0xffff
            add ecx, 0x24
            dec edx
            jne L00453e0f
            mov byte ptr [eax + 0x12e], 0
            mov word ptr [eax + 0x12c], 0
            mov word ptr [eax + 0x130], 0xffff
            mov eax, dword ptr [gSelected_drone_path_node_index]
            lea ecx, [eax + eax*8]
            lea edx, [eax + ecx*2]
            lea eax, [eax + edx*4]
            mov edx, dword ptr [gDrone_path_nodes]
            shl eax, 2
            mov cx, word ptr [edx + eax + 0x12c]
            cmp cx, 8
            jae L00453e77
            and ecx, 0xffff
            lea ecx, [ecx + ecx*8]
            lea edx, [edx + ecx*4]
            mov word ptr [edx + eax + 0xc], si
            mov ecx, dword ptr [gDrone_path_nodes]
            inc word ptr [ecx + eax + 0x12c]
    L00453e77:
            call InitialiseEditModelsEtc
            mov eax, dword ptr [gDrone_path_model_actor]
            mov dword ptr [gDrone_paths_elasticating], 0
            test eax, eax
            je L00453e93
            mov byte ptr [eax + 0x20], 1
    L00453e93:
            mov eax, dword ptr [gCount_drone_path_nodes]
            push eax
            push offset gDefault_drone_direction+0xe5c
            push offset gDrone_selection_buffer
            call sprintf
            add esp, 0xc
            xor edx, edx
            mov ecx, 4
            push offset gDrone_selection_buffer
            push -1
            push 0x7d0
            call NewTextHeadupSlot
    L00453ec3:
            pop esi
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
            nop
            nop
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
            nop
            nop
            nop
            nop
            nop
            nop
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
            nop
            nop
            nop
            nop
            nop
            nop
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
            nop
            nop
            nop
            nop
            nop
            nop
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
            nop
            nop
            nop
            nop
            nop
            nop
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
            nop
            nop
            nop
            nop
            nop
            nop
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
            nop
            nop
            nop
            nop
            nop
            nop
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
            nop
            nop
            nop
            nop
            nop
            nop
            mov eax, dword ptr [gShow_drone_paths]
            sub esp, 8
            test eax, eax
            push ebx
            push ebp
            push esi
            push edi
            jne L00453f70
            push offset gDefault_drone_direction+0xde4
            push -1
            push 0xbb8
            xor edx, edx
            mov ecx, 4
            call NewTextHeadupSlot
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 8
            ret
    L00453f70:
            mov eax, dword ptr [gDrone_paths_elasticating]
            test eax, eax
            je L00453f99
            push offset gDefault_drone_direction+0xe7c
            push -1
            push 0xbb8
            xor edx, edx
            mov ecx, 4
            call NewTextHeadupSlot
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 8
            ret
    L00453f99:
            mov eax, dword ptr [gProgram_state+0xbc]
            push 0x40000000
            lea ecx, [eax + 0x50]
            call L004537d0
            mov ebp, eax
            test ebp, ebp
            jl L00454353
            lea ecx, [ebp + ebp*8]
            mov esi, dword ptr [gDrone_path_nodes]
            lea edx, [ebp + ecx*2]
            lea ebx, [ebp + edx*4]
            shl ebx, 2
            mov dword ptr [esp + 0x10], ebx
            mov al, byte ptr [esi + ebx + 0x12e]
            lea ecx, [esi + ebx]
            test al, 1
            je L004540fd
            mov edi, dword ptr [gCount_drone_path_nodes]
            mov dword ptr [esp + 0x14], ebp
            mov dword ptr [esp + 0x10], 0
    L00453ff2:
            xor ebx, ebx
            xor ecx, ecx
            test edi, edi
            jle L00454028
            lea edx, [esi + 0xc]
    L00453ffd:
            movsx eax, word ptr [edx]
            cmp eax, dword ptr [esp + 0x14]
            je L00454013
            inc ecx
            add edx, 0x134
            cmp ecx, edi
            jl L00453ffd
            jmp L00454028
    L00454013:
            cmp ecx, ebp
            mov dword ptr [esp + 0x14], ecx
            jne L00454023
            mov dword ptr [esp + 0x10], 1
    L00454023:
            mov ebx, 1
    L00454028:
            test ebx, ebx
            je L00454034
            mov eax, dword ptr [esp + 0x10]
            test eax, eax
            je L00453ff2
    L00454034:
            mov eax, dword ptr [esp + 0x10]
            or edi, 0xffffffff
            test eax, eax
            mov eax, edi
            jne L00454045
            mov eax, dword ptr [esp + 0x14]
    L00454045:
            test eax, eax
            jge L004540a0
            mov eax, ebp
            mov dl, 0xfe
    L0045404d:
            lea ecx, [eax + eax*8]
            lea ecx, [eax + ecx*2]
            lea eax, [eax + ecx*4]
            shl eax, 2
            mov word ptr [esi + eax + 0x30], di
            mov ecx, dword ptr [gDrone_path_nodes]
            mov word ptr [ecx + eax + 0x12c], 1
            mov ecx, dword ptr [gDrone_path_nodes]
            mov bl, byte ptr [ecx + eax + 0x12e]
            and bl, dl
            mov byte ptr [ecx + eax + 0x12e], bl
            mov esi, dword ptr [gDrone_path_nodes]
            movsx eax, word ptr [esi + eax + 0xc]
            cmp eax, ebp
            jne L0045404d
            call InitialiseEditModelsEtc
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 8
            ret
    L004540a0:
            mov dl, 0xfe
    L004540a2:
            lea ecx, [eax + eax*8]
            xor ebx, ebx
            lea ecx, [eax + ecx*2]
            lea eax, [eax + ecx*4]
            shl eax, 2
            mov word ptr [esi + eax + 0x30], di
            mov ecx, dword ptr [gDrone_path_nodes]
            cmp word ptr [ecx + eax + 0xc], bx
            setge bl
            mov word ptr [ecx + eax + 0x12c], bx
            mov ecx, dword ptr [gDrone_path_nodes]
            mov bl, byte ptr [ecx + eax + 0x12e]
            and bl, dl
            mov byte ptr [ecx + eax + 0x12e], bl
            mov esi, dword ptr [gDrone_path_nodes]
            movsx eax, word ptr [esi + eax + 0xc]
            test eax, eax
            jge L004540a2
            call InitialiseEditModelsEtc
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 8
            ret
    L004540fd:
            movsx eax, word ptr [ecx + 0xc]
            lea edx, [eax + eax*8]
            lea ecx, [eax + edx*2]
            lea edx, [eax + ecx*4]
            lea ecx, [esi + edx*4]
            mov edx, 1
            cmp word ptr [ecx + 0x12c], dx
            jne L0045411f
            cmp eax, ebp
            jne L004540fd
    L0045411f:
            lea ecx, [eax + eax*8]
            lea ecx, [eax + ecx*2]
            lea eax, [eax + ecx*4]
            cmp word ptr [esi + eax*4 + 0x12c], dx
            ja L0045433b
            mov edi, dword ptr [gCount_drone_path_nodes]
            mov dword ptr [esp + 0x14], ebp
    L00454140:
            or edx, 0xffffffff
            xor eax, eax
            test edi, edi
            jle L00454178
            lea ecx, [esi + 0xc]
    L0045414c:
            cmp word ptr [ecx + 0x120], 0
            je L00454169
            movsx ebx, word ptr [ecx]
            cmp ebx, dword ptr [esp + 0x14]
            jne L00454169
            test edx, edx
            jge L0045433b
            mov edx, eax
    L00454169:
            inc eax
            add ecx, 0x134
            cmp eax, edi
            jl L0045414c
            mov ebx, dword ptr [esp + 0x10]
    L00454178:
            test edx, edx
            mov dword ptr [esp + 0x14], edx
            jl L00454184
            cmp edx, ebp
            jne L00454140
    L00454184:
            mov ecx, ebp
            call L00454360
            test eax, eax
            jge L0045422b
            mov eax, dword ptr [gDrone_path_nodes]
            mov edi, 2
            mov edx, ebp
            mov cl, byte ptr [eax + ebx + 0x12e]
            or cl, 1
            mov byte ptr [eax + ebx + 0x12e], cl
            mov ecx, dword ptr [gDrone_path_nodes]
            mov word ptr [ecx + ebx + 0x12c], di
            mov eax, dword ptr [gDrone_path_nodes]
            mov word ptr [eax + ebx + 0x30], 0xffff
            mov esi, dword ptr [gDrone_path_nodes]
            movsx ecx, word ptr [esi + ebx + 0xc]
    L004541d5:
            lea eax, [ecx + ecx*8]
            lea eax, [ecx + eax*2]
            lea eax, [ecx + eax*4]
            shl eax, 2
            mov bl, byte ptr [esi + eax + 0x12e]
            or bl, 1
            mov byte ptr [esi + eax + 0x12e], bl
            mov esi, dword ptr [gDrone_path_nodes]
            mov word ptr [esi + eax + 0x12c], di
            mov esi, dword ptr [gDrone_path_nodes]
            mov word ptr [esi + eax + 0x30], dx
            mov esi, dword ptr [gDrone_path_nodes]
            mov edx, ecx
            movsx ecx, word ptr [esi + eax + 0xc]
            cmp edx, ebp
            jne L004541d5
            call InitialiseEditModelsEtc
            push offset gDefault_drone_direction+0xefc
            jmp L00454340
    L0045422b:
            lea ecx, [eax + eax*8]
            lea edx, [eax + ecx*2]
            lea ecx, [eax + edx*4]
            mov edx, dword ptr [gDrone_path_nodes]
            shl ecx, 2
            cmp word ptr [edx + ecx + 0x12c], 0
            je L0045432f
            mov bl, byte ptr [edx + ecx + 0x12e]
            lea edx, [edx + ecx + 0x12e]
            or bl, 1
            mov edi, 2
            mov byte ptr [edx], bl
            mov edx, dword ptr [gDrone_path_nodes]
            or ebx, 0xffffffff
            mov word ptr [edx + ecx + 0x12c], di
            mov edx, dword ptr [gDrone_path_nodes]
            mov word ptr [edx + ecx + 0x30], bx
            mov esi, dword ptr [gDrone_path_nodes]
            movsx ecx, word ptr [esi + ecx + 0xc]
            lea edx, [ecx + ecx*8]
            lea edx, [ecx + edx*2]
            lea edx, [ecx + edx*4]
            shl edx, 2
            cmp word ptr [esi + edx + 0x12c], 0
            je L004542ee
    L004542a2:
            or byte ptr [esi + edx + 0x12e], 1
            lea esi, [esi + edx + 0x12e]
            mov esi, dword ptr [gDrone_path_nodes]
            mov word ptr [esi + edx + 0x12c], di
            mov esi, dword ptr [gDrone_path_nodes]
            mov word ptr [esi + edx + 0x30], ax
            mov esi, dword ptr [gDrone_path_nodes]
            mov eax, ecx
            movsx ecx, word ptr [esi + edx + 0xc]
            lea edx, [ecx + ecx*8]
            lea edx, [ecx + edx*2]
            lea edx, [ecx + edx*4]
            shl edx, 2
            cmp word ptr [esi + edx + 0x12c], 0
            jne L004542a2
    L004542ee:
            lea edx, [ecx + ecx*8]
            lea edx, [ecx + edx*2]
            lea ecx, [ecx + edx*4]
            shl ecx, 2
            mov dl, byte ptr [esi + ecx + 0x12e]
            or dl, 1
            mov byte ptr [esi + ecx + 0x12e], dl
            mov edx, dword ptr [gDrone_path_nodes]
            mov word ptr [edx + ecx + 0x12c], di
            mov edx, dword ptr [gDrone_path_nodes]
            mov word ptr [edx + ecx + 0xc], bx
            mov edx, dword ptr [gDrone_path_nodes]
            mov word ptr [edx + ecx + 0x30], ax
    L0045432f:
            call InitialiseEditModelsEtc
            push offset gDefault_drone_direction+0xefc
            jmp L00454340
    L0045433b:
            push offset gDefault_drone_direction+0xee8
    L00454340:
            push -1
            push 0x7d0
            xor edx, edx
            mov ecx, 4
            call NewTextHeadupSlot
    L00454353:
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
    L00454360:
            push ecx
            push ebx
            push ebp
            push esi
            mov esi, dword ptr [gCount_drone_path_nodes]
            push edi
            mov dword ptr [esp + 0x10], ecx
            mov ebx, ecx
            xor ebp, ebp
    L00454373:
            xor edi, edi
            xor eax, eax
            test esi, esi
            jle L004543ac
            mov ecx, dword ptr [gDrone_path_nodes]
            lea edx, [ecx + 0xc]
    L00454384:
            movsx ecx, word ptr [edx]
            cmp ecx, ebx
            je L00454398
            inc eax
            add edx, 0x134
            cmp eax, esi
            jl L00454384
            jmp L004543ac
    L00454398:
            mov ecx, dword ptr [esp + 0x10]
            mov ebx, eax
            cmp eax, ecx
            jne L004543a7
            mov ebp, 1
    L004543a7:
            mov edi, 1
    L004543ac:
            test edi, edi
            je L004543b4
            test ebp, ebp
            je L00454373
    L004543b4:
            test ebp, ebp
            je L004543c1
            or eax, 0xffffffff
            pop edi
            pop esi
            pop ebp
            pop ebx
            pop ecx
            ret
    L004543c1:
            pop edi
            pop esi
            mov eax, ebx
            pop ebp
            pop ebx
            pop ecx
            ret
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            mov eax, dword ptr [gShow_drone_paths]
            push esi
            test eax, eax
            jne L004543e1
            push offset gDefault_drone_direction+0xde4
            jmp L004543ef
    L004543e1:
            mov eax, dword ptr [gDrone_paths_elasticating]
            test eax, eax
            je L00454404
            push offset gDefault_drone_direction+0xe7c
    L004543ef:
            push -1
            push 0xbb8
            xor edx, edx
            mov ecx, 4
            call NewTextHeadupSlot
            pop esi
            ret
    L00454404:
            mov eax, dword ptr [gProgram_state+0xbc]
            push 0x40000000
            lea ecx, [eax + 0x50]
            call L004537d0
            mov esi, eax
            test esi, esi
            jl L00454464
            lea ecx, [esi + esi*8]
            lea edx, [esi + ecx*2]
            mov ecx, dword ptr [gDrone_path_nodes]
            lea eax, [esi + edx*4]
            lea ecx, [ecx + eax*4 + 0x12e]
            mov al, byte ptr [ecx]
            test al, 0x10
            je L0045443c
            and al, 0xef
            jmp L0045443e
    L0045443c:
            or al, 0x10
    L0045443e:
            mov byte ptr [ecx], al
            mov ecx, esi
            call L00454470
            mov edx, dword ptr [gTime_stamp_for_this_munging]
            mov dword ptr [gSelected_edit_drone_path], esi
            add edx, 0x3e8
            mov dword ptr [gNext_drone_edit_path_munge], edx
            call InitialiseEditModelsEtc
    L00454464:
            pop esi
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
            nop
    L00454470:
            sub esp, 0x100
            lea eax, [esp]
            push ebx
            push ebp
            push esi
            mov esi, ecx
            push edi
            push esi
            push offset gDefault_drone_direction+0xf5c
            push eax
            call sprintf
            lea ecx, [esi + esi*8]
            add esp, 0xc
            lea ebx, [esp + 0x10]
            mov edi, offset gDefault_drone_direction+0xf4c
            lea edx, [esi + ecx*2]
            mov ecx, dword ptr [gDrone_path_nodes]
            lea eax, [esi + edx*4]
            lea edx, [ecx + eax*4 + 0x12e]
            mov al, byte ptr [ecx + eax*4 + 0x12e]
            test al, 0x10
            jne L004544be
            mov edi, offset gDefault_drone_direction+0xf40
    L004544be:
            or ecx, 0xffffffff
            xor eax, eax
            repne scasb
            not ecx
            sub edi, ecx
            mov esi, edi
            mov ebp, ecx
            mov edi, ebx
            or ecx, 0xffffffff
            repne scasb
            mov ecx, ebp
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, ebp
            lea ebx, [esp + 0x10]
            and ecx, 3
            rep movsb
            mov edi, offset gDefault_drone_direction+0xf3c
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            mov esi, edi
            mov ebp, ecx
            mov edi, ebx
            or ecx, 0xffffffff
            repne scasb
            mov ecx, ebp
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, ebp
            lea ebx, [esp + 0x10]
            and ecx, 3
            rep movsb
            mov al, byte ptr [edx]
            mov edi, offset gDefault_drone_direction+0xf34
            test al, 2
            jne L00454523
            mov edi, offset gDefault_drone_direction+0xf28
    L00454523:
            or ecx, 0xffffffff
            xor eax, eax
            repne scasb
            not ecx
            sub edi, ecx
            mov esi, edi
            mov ebp, ecx
            mov edi, ebx
            or ecx, 0xffffffff
            repne scasb
            mov ecx, ebp
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, ebp
            lea ebx, [esp + 0x10]
            and ecx, 3
            rep movsb
            mov edi, offset gDefault_drone_direction+0xf3c
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            mov esi, edi
            mov ebp, ecx
            mov edi, ebx
            or ecx, 0xffffffff
            repne scasb
            mov ecx, ebp
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, ebp
            lea ebx, [esp + 0x10]
            and ecx, 3
            rep movsb
            mov al, byte ptr [edx]
            mov edi, offset gDefault_drone_direction+0xf18
            test al, 4
            jne L00454588
            mov edi, offset gDefault_drone_direction+0xf14
    L00454588:
            or ecx, 0xffffffff
            xor eax, eax
            repne scasb
            not ecx
            sub edi, ecx
            mov esi, edi
            mov ebp, ecx
            mov edi, ebx
            or ecx, 0xffffffff
            repne scasb
            mov ecx, ebp
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, ebp
            lea ebx, [esp + 0x10]
            and ecx, 3
            rep movsb
            mov edi, offset gDefault_drone_direction+0xf3c
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            mov esi, edi
            mov ebp, ecx
            mov edi, ebx
            or ecx, 0xffffffff
            repne scasb
            mov ecx, ebp
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, ebp
            and ecx, 3
            rep movsb
            mov al, byte ptr [edx]
            lea edx, [esp + 0x10]
            test al, 8
            mov edi, offset gDefault_drone_direction+0xf0c
            jne L004545ed
            mov edi, offset gDefault_drone_direction+0xf14
    L004545ed:
            or ecx, 0xffffffff
            xor eax, eax
            repne scasb
            not ecx
            sub edi, ecx
            mov esi, edi
            mov ebx, ecx
            mov edi, edx
            or ecx, 0xffffffff
            repne scasb
            mov ecx, ebx
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, ebx
            lea eax, [esp + 0x10]
            and ecx, 3
            push eax
            rep movsb
            push -1
            push 0x3e8
            xor edx, edx
            mov ecx, 4
            call NewTextHeadupSlot
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x100
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
            nop
            nop
            jmp WriteOutDronePaths
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
            jmp WriteOutDronePaths
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
            jmp WriteOutDronePaths
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
            jmp WriteOutDronePaths
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
            jmp WriteOutDronePaths
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
            mov eax, dword ptr [gShow_drone_paths]
            sub esp, 0x100
            test eax, eax
            jne L004546be
            push offset gDefault_drone_direction+0xde4
            push -1
            push 0xbb8
            xor edx, edx
            mov ecx, 4
            call NewTextHeadupSlot
            add esp, 0x100
            ret
    L004546be:
            mov eax, dword ptr [gProgram_state+0xbc]
            push 0x40000000
            lea ecx, [eax + 0x50]
            call L004537d0
            test eax, eax
            jl L004546ff
            push eax
            lea ecx, [esp + 4]
            push offset gDefault_drone_direction+0xf70
            push ecx
            call sprintf
            add esp, 0xc
            lea edx, [esp]
            mov ecx, 4
            push edx
            push -1
            push 0x3e8
            xor edx, edx
            call NewTextHeadupSlot
    L004546ff:
            add esp, 0x100
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
            nop
            mov eax, dword ptr [gShow_drone_paths]
            test eax, eax
            jne L00454720
            push offset gDefault_drone_direction+0xde4
            jmp L0045472e
    L00454720:
            mov eax, dword ptr [gDrone_paths_elasticating]
            test eax, eax
            je L00454742
            push offset gDefault_drone_direction+0xe7c
    L0045472e:
            push -1
            push 0xbb8
            xor edx, edx
            mov ecx, 4
            call NewTextHeadupSlot
    L00454741:
            ret
    L00454742:
            mov eax, dword ptr [gProgram_state+0xbc]
            push 0x40000000
            lea ecx, [eax + 0x50]
            call L004537d0
            test eax, eax
            jl L00454741
            mov ecx, eax
            jmp L00454470
            nop
            mov eax, dword ptr [gShow_drone_paths]
            sub esp, 0xc
            test eax, eax
            push ebx
            push ebp
            push esi
            push edi
            jne L00454790
            push offset gDefault_drone_direction+0xde4
            push -1
            push 0xbb8
            xor edx, edx
            mov ecx, 4
            call NewTextHeadupSlot
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0xc
            ret
    L00454790:
            mov eax, dword ptr [gDrone_paths_elasticating]
            test eax, eax
            je L004547b9
            push offset gDefault_drone_direction+0xe7c
            push -1
            push 0xbb8
            xor edx, edx
            mov ecx, 4
            call NewTextHeadupSlot
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0xc
            ret
    L004547b9:
            mov eax, dword ptr [gProgram_state+0xbc]
            push 0x40000000
            lea ecx, [eax + 0x50]
            call L004537d0
            mov ebx, eax
            test ebx, ebx
            mov dword ptr [esp + 0x18], ebx
            jl L004549db
            mov ecx, dword ptr [gCount_drone_path_nodes]
            push ecx
            push ebx
            push offset gDefault_drone_direction+0x11dc
            call DoNotDprintf
            mov eax, dword ptr [gCount_drones]
            add esp, 0xc
            xor edi, edi
            test eax, eax
            jle L0045488a
            xor esi, esi
    L004547ff:
            mov edx, dword ptr [gDrone_specs]
            movsx eax, word ptr [edx + esi + 8]
            cmp eax, ebx
            jne L00454876
            push ebx
            push edi
            push offset gDefault_drone_direction+0x1190
            call DoNotDprintf
            add esp, 0xc
            push edi
            push offset gDefault_drone_direction+0x116c
            call dr_dprintf
            mov ecx, dword ptr [gCount_drones]
            lea edx, [esi + 0x5d8]
            mov eax, ecx
            add esp, 8
            shl eax, 5
            sub eax, ecx
            lea eax, [eax + eax*2]
            lea ecx, [ecx + eax*2]
            mov eax, dword ptr [gDrone_specs]
            shl ecx, 3
            sub ecx, edx
            add edx, eax
            push ecx
            add eax, esi
            push edx
            push eax
            call memmove
            mov eax, dword ptr [gCount_drones]
            add esp, 0xc
            dec eax
            push eax
            push offset gDefault_drone_direction+0x1144
            mov dword ptr [gCount_drones], eax
            call dr_dprintf
            add esp, 8
    L00454876:
            mov eax, dword ptr [gCount_drones]
            inc edi
            add esi, 0x5d8
            cmp edi, eax
            jl L004547ff
    L0045488a:
            mov ecx, ebx
            call L004549f0
            mov edx, dword ptr [gCount_drone_path_nodes]
            or edi, 0xffffffff
            test edi, edi
            jl L004548cc
    L0045489e:
            push edi
            push offset gDefault_drone_direction+0x10fc
            call DoNotDprintf
            add esp, 8
            mov ecx, edi
            call L004549f0
            mov eax, dword ptr [gCount_drone_path_nodes]
            push eax
            push offset gDefault_drone_direction+0x10b4
            call DoNotDprintf
            mov edx, dword ptr [gCount_drone_path_nodes]
            add esp, 8
    L004548cc:
            or edi, 0xffffffff
            xor esi, esi
            test edx, edx
            mov dword ptr [esp + 0x14], edi
            jle L00454981
            mov dword ptr [esp + 0x10], esi
    L004548e1:
            mov ebx, dword ptr [gDrone_path_nodes]
            mov ecx, dword ptr [esp + 0x10]
            cmp word ptr [ebx + ecx + 0x12c], 0
            jne L00454953
            xor edi, edi
            test edx, edx
            jle L0045496b
            add ebx, 0x12c
    L00454902:
            cmp edi, esi
            je L0045492c
            xor eax, eax
            xor ecx, ecx
            mov ax, word ptr [ebx]
            test eax, eax
            jle L0045492c
            lea edx, [ebx - 0x120]
    L00454917:
            movsx ebp, word ptr [edx]
            cmp ebp, esi
            je L00454939
            inc ecx
            add edx, 0x24
            cmp ecx, eax
            jl L00454917
            mov edx, dword ptr [gCount_drone_path_nodes]
    L0045492c:
            inc edi
            add ebx, 0x134
            cmp edi, edx
            jge L0045496b
            jmp L00454902
    L00454939:
            push ecx
            push edi
            push esi
            push offset gDefault_drone_direction+0x1074
            call DoNotDprintf
            mov edx, dword ptr [gCount_drone_path_nodes]
            mov edi, dword ptr [esp + 0x24]
            add esp, 0x10
    L00454953:
            mov ecx, dword ptr [esp + 0x10]
            inc esi
            add ecx, 0x134
            cmp esi, edx
            mov dword ptr [esp + 0x10], ecx
            jge L00454981
            jmp L004548e1
    L0045496b:
            push esi
            push offset gDefault_drone_direction+0x1008
            call DoNotDprintf
            mov edx, dword ptr [gCount_drone_path_nodes]
            add esp, 8
            mov edi, esi
    L00454981:
            test edi, edi
            jge L0045489e
            mov eax, dword ptr [gCount_drones]
            push eax
            push edx
            push offset gDefault_drone_direction+0xfb4
            call DoNotDprintf
            add esp, 0xc
            mov dword ptr [gDrones_unmodified], 0
            call InitialiseEditModelsEtc
            mov ecx, dword ptr [esp + 0x18]
            push ecx
            push offset gDefault_drone_direction+0xf80
            push offset gDrone_selection_buffer
            call sprintf
            add esp, 0xc
            xor edx, edx
            mov ecx, 4
            push offset gDrone_selection_buffer
            push -1
            push 0xdac
            call NewTextHeadupSlot
    L004549db:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0xc
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
            nop
            nop
            nop
            nop
    L004549f0:
            sub esp, 8
            push ebp
            mov ebp, ecx
            push esi
            push ebp
            push offset gDefault_drone_direction+0x13d0
            mov dword ptr [esp + 0x14], ebp
            call DoNotDprintf
            mov eax, dword ptr [gCount_drone_path_nodes]
            add esp, 8
            xor ecx, ecx
            test eax, eax
            mov dword ptr [esp + 8], ecx
            jle L00454afb
            push edi
            push ebx
            mov ebx, 0x12c
    L00454a23:
            cmp ecx, ebp
            je L00454ae6
            mov edx, dword ptr [gDrone_path_nodes]
            lea esi, [ebx - 0x12c]
            xor edi, edi
            cmp word ptr [edx + esi + 0x12c], di
            jbe L00454ae6
            mov ebp, 0x24
    L00454a4c:
            movsx eax, word ptr [edx + esi + 0xc]
            mov edx, dword ptr [esp + 0x14]
            cmp eax, edx
            jle L00454a75
            push eax
            push edi
            push ecx
            push offset gDefault_drone_direction+0x1380
            call DoNotDprintf
            mov eax, dword ptr [gDrone_path_nodes]
            add esp, 0x10
            dec word ptr [eax + esi + 0xc]
            jmp L00454abe
    L00454a75:
            jne L00454ac2
            push eax
            push edi
            push ecx
            push offset gDefault_drone_direction+0x1334
            call DoNotDprintf
            mov ecx, dword ptr [gDrone_path_nodes]
            xor eax, eax
            add esp, 0x10
            mov ax, word ptr [ecx + ebx]
            lea edx, [ecx + esi + 0x30]
            lea eax, [eax + eax*8]
            shl eax, 2
            sub eax, ebp
            push eax
            lea eax, [ecx + esi + 0xc]
            push edx
            push eax
            call memmove
            mov eax, dword ptr [gDrone_path_nodes]
            add esp, 0xc
            sub ebp, 0x24
            dec word ptr [eax + ebx]
            dec edi
            sub esi, 0x24
    L00454abe:
            mov ecx, dword ptr [esp + 0x10]
    L00454ac2:
            mov edx, dword ptr [gDrone_path_nodes]
            xor eax, eax
            inc edi
            add ebp, 0x24
            mov ax, word ptr [edx + ebx]
            add esi, 0x24
            cmp edi, eax
            jl L00454a4c
            mov ebp, dword ptr [esp + 0x14]
            mov eax, dword ptr [gCount_drone_path_nodes]
    L00454ae6:
            inc ecx
            add ebx, 0x134
            cmp ecx, eax
            mov dword ptr [esp + 0x10], ecx
            jl L00454a23
            pop ebx
            pop edi
    L00454afb:
            lea ecx, [ebp + 1]
            lea edx, [ecx + ecx*8]
            lea edx, [ecx + edx*2]
            lea esi, [ecx + edx*4]
            lea ecx, [eax + eax*8]
            shl esi, 2
            lea edx, [eax + ecx*2]
            lea ecx, [eax + edx*4]
            sub eax, ebp
            shl ecx, 2
            sub ecx, esi
            dec eax
            push ecx
            push eax
            push 0x134
            push ebp
            push offset gDefault_drone_direction+0x12a0
            call DoNotDprintf
            add esp, 0x14
            push 0x134
            push 0x5d8
            push 0x134
            push 0x24
            push offset gDefault_drone_direction+0x1244
            call DoNotDprintf
            mov eax, dword ptr [gCount_drone_path_nodes]
            add esp, 0x14
            lea edx, [eax + eax*8]
            lea ecx, [eax + edx*2]
            lea edx, [eax + ecx*4]
            mov eax, dword ptr [gDrone_path_nodes]
            shl edx, 2
            sub edx, esi
            lea ecx, [eax + esi]
            push edx
            lea edx, [ebp + ebp*8]
            push ecx
            lea ecx, [ebp + edx*2]
            lea edx, [ebp + ecx*4]
            lea eax, [eax + edx*4]
            push eax
            call memmove
            mov edx, dword ptr [gCount_drone_path_nodes]
            add esp, 0xc
            dec edx
            push offset gDefault_drone_direction+0x1224
            mov dword ptr [gCount_drone_path_nodes], edx
            call DoNotDprintf
            add esp, 4
            pop esi
            pop ebp
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
            nop
            nop
            nop
            nop
            mov eax, dword ptr [gShow_drone_paths]
            test eax, eax
            jne L00454bd2
            push offset gDefault_drone_direction+0xde4
            push -1
            push 0xbb8
            xor edx, edx
            mov ecx, 4
            call NewTextHeadupSlot
            ret
    L00454bd2:
            mov eax, dword ptr [gDrone_paths_elasticating]
            test eax, eax
            je L00454bf4
            push offset gDefault_drone_direction+0xe7c
            push -1
            push 0xbb8
            xor edx, edx
            mov ecx, 4
            call NewTextHeadupSlot
            ret
    L00454bf4:
            mov eax, dword ptr [gProgram_state+0xbc]
            push 0x40000000
            lea ecx, [eax + 0x50]
            call L004537d0
            test eax, eax
            jl L00454c7a
            lea ecx, [eax + eax*8]
            lea edx, [eax + ecx*2]
            mov ecx, dword ptr [gProgram_state+0xbc]
            lea eax, [eax + edx*4]
            mov edx, dword ptr [ecx + 0x50]
            mov ecx, dword ptr [gDrone_path_nodes]
            shl eax, 2
            mov dword ptr [ecx + eax], edx
            mov edx, dword ptr [gProgram_state+0xbc]
            mov ecx, dword ptr [edx + 0x54]
            mov edx, dword ptr [gDrone_path_nodes]
            mov dword ptr [edx + eax + 4], ecx
            mov ecx, dword ptr [gProgram_state+0xbc]
            mov edx, dword ptr [ecx + 0x58]
            mov ecx, dword ptr [gDrone_path_nodes]
            mov dword ptr [ecx + eax + 8], edx
            call PreprocessDronePaths
            mov dword ptr [gDrones_unmodified], 0
            call InitialiseEditModelsEtc
            xor edx, edx
            mov ecx, 4
            push offset gDefault_drone_direction+0x13f4
            push -1
            push 0x7d0
            call NewTextHeadupSlot
    L00454c7a:
            ret
            nop
            nop
            nop
            nop
            nop
            mov ecx, dword ptr [gSelected_drone_path_index]
            test ecx, ecx
            jge L00454ca3
            push offset gDefault_drone_direction+0x13fc
            push -4
            push 0xbb8
            xor edx, edx
            mov ecx, 4
            call NewTextHeadupSlot
            ret
    L00454ca3:
            mov edx, dword ptr [gSelected_drone_form_index]
            mov eax, dword ptr [gCount_drone_forms]
            inc edx
            cmp edx, eax
            mov dword ptr [gSelected_drone_form_index], edx
            jl L00454cc1
            xor edx, edx
            mov dword ptr [gSelected_drone_form_index], edx
    L00454cc1:
            mov eax, ecx
            shl eax, 5
            sub eax, ecx
            lea eax, [eax + eax*2]
            lea ecx, [ecx + eax*2]
            mov eax, dword ptr [gDrone_specs]
            lea ecx, [eax + ecx*8]
            mov eax, edx
            shl eax, 4
            add eax, edx
            lea edx, [eax*8 + gDrone_forms]
            mov dword ptr [ecx + 4], edx
            call LoadDroneActorsModels
            jmp WriteOutDronePaths
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
            nop
            nop
            mov ecx, dword ptr [gSelected_drone_path_index]
            test ecx, ecx
            jge L00454d23
            push offset gDefault_drone_direction+0x13fc
            push -4
            push 0xbb8
            xor edx, edx
            mov ecx, 4
            call NewTextHeadupSlot
            ret
    L00454d23:
            mov edx, dword ptr [gSelected_drone_form_index]
            dec edx
            mov dword ptr [gSelected_drone_form_index], edx
            jns L00454d40
            mov eax, dword ptr [gCount_drone_forms]
            lea edx, [eax - 1]
            mov dword ptr [gSelected_drone_form_index], edx
    L00454d40:
            mov eax, ecx
            shl eax, 5
            sub eax, ecx
            lea eax, [eax + eax*2]
            lea ecx, [ecx + eax*2]
            mov eax, dword ptr [gDrone_specs]
            lea ecx, [eax + ecx*8]
            mov eax, edx
            shl eax, 4
            add eax, edx
            lea edx, [eax*8 + gDrone_forms]
            mov dword ptr [ecx + 4], edx
            call LoadDroneActorsModels
            jmp WriteOutDronePaths
            mov eax, dword ptr [gShow_drone_paths]
            push esi
            test eax, eax
            jne L00454d81
            push offset gDefault_drone_direction+0xde4
            jmp L00454d8f
    L00454d81:
            mov eax, dword ptr [gDrone_paths_elasticating]
            test eax, eax
            je L00454da4
            push offset gDefault_drone_direction+0xe7c
    L00454d8f:
            push -1
            push 0xbb8
            xor edx, edx
            mov ecx, 4
            call NewTextHeadupSlot
            pop esi
            ret
    L00454da4:
            mov eax, dword ptr [gProgram_state+0xbc]
            push 0x40000000
            lea ecx, [eax + 0x50]
            call L004537d0
            test eax, eax
            jl L00454f39
            lea ecx, [eax + eax*8]
            lea edx, [eax + ecx*2]
            lea ecx, [eax + edx*4]
            mov edx, dword ptr [gDrone_path_nodes]
            cmp word ptr [edx + ecx*4 + 0x130], 0
            lea ecx, [edx + ecx*4 + 0x130]
            jl L00454df9
            push eax
            push offset gDefault_drone_direction+0x142c
            push offset gDrone_selection_buffer
            call sprintf
            add esp, 0xc
            push offset gDrone_selection_buffer
            jmp L00454e0a
    L00454df9:
            cmp dword ptr [gCount_drones], 0xc8
            jl L00454e1f
            push offset gDefault_drone_direction+0x1414
    L00454e0a:
            push -1
            push 0xdac
            xor edx, edx
            mov ecx, 4
            call NewTextHeadupSlot
            pop esi
            ret
    L00454e1f:
            mov edx, dword ptr [gSelected_drone_form_index]
            test edx, edx
            jge L00454e31
            xor edx, edx
            mov dword ptr [gSelected_drone_form_index], edx
    L00454e31:
            mov word ptr [ecx], dx
            mov ecx, dword ptr [gCount_drones]
            mov edx, eax
            mov eax, ecx
            shl eax, 5
            sub eax, ecx
            lea eax, [eax + eax*2]
            lea ecx, [ecx + eax*2]
            mov eax, dword ptr [gDrone_specs]
            lea ecx, [eax + ecx*8]
            call InitDroneSpec
            mov ecx, dword ptr [gCount_drones]
            mov eax, ecx
            shl eax, 5
            sub eax, ecx
            lea edx, [eax + eax*2]
            lea eax, [ecx + edx*2]
            mov edx, dword ptr [gDrone_specs]
            mov dword ptr [edx + eax*8], ecx
            mov ecx, dword ptr [gCount_drones]
            mov eax, ecx
            mov edx, dword ptr [gDrone_specs]
            shl eax, 5
            sub eax, ecx
            mov dword ptr [gSelected_drone_path_index], ecx
            lea eax, [eax + eax*2]
            lea ecx, [ecx + eax*2]
            mov eax, dword ptr [gSelected_drone_form_index]
            lea ecx, [edx + ecx*8]
            mov edx, eax
            shl edx, 4
            add edx, eax
            lea eax, [edx*8 + gDrone_forms]
            mov dword ptr [ecx + 4], eax
            call LoadDroneActorsModels
            mov ecx, dword ptr [gCount_drones]
            mov eax, ecx
            push 1
            shl eax, 5
            sub eax, ecx
            lea edx, [eax + eax*2]
            lea eax, [ecx + edx*2]
            mov ecx, dword ptr [gDrone_specs]
            mov edx, dword ptr [ecx + eax*8 + 0x38]
            lea esi, [ecx + eax*8]
            push edx
            mov eax, dword ptr [esi]
            push eax
            push offset gDefault_drone_direction+0x138
            call DoNotDprintf
            mov eax, dword ptr [esi + 0x38]
            add esp, 0x10
            cmp eax, 1
            je L00454f29
            mov eax, dword ptr [eax*4 + gDrone_state_functions]
            test eax, eax
            je L00454efe
            mov edx, 4
            mov ecx, esi
            call eax
    L00454efe:
            mov ecx, dword ptr [gTime_stamp_for_this_munging]
            mov edx, dword ptr [esi + 0x3c]
            mov eax, dword ptr [esi + 0x38]
            mov dword ptr [esi + 0x30], ecx
            mov dword ptr [esi + 0x40], edx
            mov dword ptr [esi + 0x3c], eax
            mov dword ptr [esi + 0x38], 1
            mov eax, dword ptr [gDrone_state_functions+0x4]
            test eax, eax
            je L00454f29
            xor edx, edx
            mov ecx, esi
            call eax
    L00454f29:
            inc dword ptr [gCount_drones]
            call SelectNearestDrone
            call WriteOutDronePaths
    L00454f39:
            pop esi
            ret
            nop
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00452430
void C2_NAKED C2_HOOK_FASTCALL DronePhysicsWarning(tPhysics_object* pObject) {
#ifndef CARPOCALYPSE2_MATCHING
    volatile br_actor* actor;
    tDrone_spec* pDrone;
    volatile br_vector3 matrix_to_actor;
    volatile br_vector3 alleged_current_v;

    pDrone = (tDrone_spec*)pObject->owner;
    matrix_to_actor.v[0] = pObject->actor->t.t.mat.m[3][0] - pObject->transform_matrix.m[3][0];
    matrix_to_actor.v[1] = pObject->actor->t.t.mat.m[3][1] - pObject->transform_matrix.m[3][1];
    matrix_to_actor.v[2] = pObject->actor->t.t.mat.m[3][2] - pObject->transform_matrix.m[3][2];

    DoNotDprintf("************");
    DoNotDprintf("%d: DRONE PHYSICS WARNING - Drone index %d", gFrame, pDrone->id);
    DoNotDprintf("guilty length %f", sqrt(matrix_to_actor.v[0] * matrix_to_actor.v[0]
        + matrix_to_actor.v[1] * matrix_to_actor.v[1]
        + matrix_to_actor.v[2] * matrix_to_actor.v[2]));
    DoNotDprintf("State %d, last state %d, last last state %d", pDrone->current_state, pDrone->prev_states[0], pDrone->prev_states[1]);
    if (pObject->actor == pDrone->actor) {
        DoNotDprintf("Object and drone actors are the same thing. OK.");
    } else {
        DoNotDprintf("FUCKING HELL! Object and drone actors differ!");
    }
    actor = pObject->actor;
    alleged_current_v.v[0] = actor->t.t.mat.m[3][0] - ((volatile tDrone_spec*)pDrone)->pos.v[0];
    alleged_current_v.v[1] = actor->t.t.mat.m[3][1] - ((volatile tDrone_spec*)pDrone)->pos.v[1];
    alleged_current_v.v[2] = actor->t.t.mat.m[3][2] - ((volatile tDrone_spec*)pDrone)->pos.v[2];
    DoNotDprintf("%d: %s: %3.3f, %3.3f, %3.3f", gFrame, "Object matrix-to-actor v",
        matrix_to_actor.v[0], matrix_to_actor.v[1], matrix_to_actor.v[2]);
    DoNotDprintf("%d: %s: %3.3f, %3.3f, %3.3f", gFrame, "Alleged current_v",
        alleged_current_v.v[0], alleged_current_v.v[1], alleged_current_v.v[2]);
    DoNotDprintf("%d: %s: %3.3f, %3.3f, %3.3f", gFrame, "Drone spec current_v",
        pDrone->field_0x18.v[0], pDrone->field_0x18.v[1], pDrone->field_0x18.v[2]);
#else
    __asm {
            sub esp, 0x18
            push esi
            push edi
            mov edi, ecx
            push offset gDefault_drone_direction+0xc00
            mov eax, dword ptr [edi]
            mov esi, dword ptr [edi + 0x23c]
            fld dword ptr [eax + 0x50]
            fsub dword ptr [edi + 0xb0]
            fstp dword ptr [esp + 0xc]
            fld dword ptr [eax + 0x54]
            fsub dword ptr [edi + 0xb4]
            fstp dword ptr [esp + 0x10]
            fld dword ptr [eax + 0x58]
            fsub dword ptr [edi + 0xb8]
            fstp dword ptr [esp + 0x14]
            call DoNotDprintf
            mov eax, dword ptr [esi]
            mov ecx, dword ptr [gFrame]
            add esp, 4
            push eax
            push ecx
            push offset gDefault_drone_direction+0xbd4
            call DoNotDprintf
            fld dword ptr [esp + 0x14]
            fmul dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x18]
            fmul dword ptr [esp + 0x18]
            add esp, 0xc
            sub esp, 8
            faddp st(1), st(0)
            fld dword ptr [esp + 0x18]
            fmul dword ptr [esp + 0x18]
            faddp st(1), st(0)
            fsqrt
            _emit 0xdd
        _emit 0x1c
        _emit 0x24
            push offset gDefault_drone_direction+0xbc0
            call DoNotDprintf
            mov edx, dword ptr [esi + 0x40]
            mov eax, dword ptr [esi + 0x3c]
            mov ecx, dword ptr [esi + 0x38]
            add esp, 0xc
            push edx
            push eax
            push ecx
            push offset gDefault_drone_direction+0xb94
            call DoNotDprintf
            mov edx, dword ptr [edi]
            mov eax, dword ptr [esi + 0xec]
            add esp, 0x10
            cmp edx, eax
            jne L004524e7
            push offset gDefault_drone_direction+0xb64
            jmp L004524ec
    L004524e7:
            push offset gDefault_drone_direction+0xb34
    L004524ec:
            call DoNotDprintf
            mov edi, dword ptr [edi]
            add esp, 4
            sub esp, 8
            mov eax, dword ptr [gFrame]
            fld dword ptr [edi + 0x50]
            fsub dword ptr [esi + 0x24]
            sub esp, 8
            sub esp, 8
            fstp dword ptr [esp + 0x2c]
            fld dword ptr [edi + 0x54]
            fsub dword ptr [esi + 0x28]
            fstp dword ptr [esp + 0x30]
            fld dword ptr [edi + 0x58]
            fsub dword ptr [esi + 0x2c]
            fstp dword ptr [esp + 0x34]
            fld dword ptr [esp + 0x28]
            fstp qword ptr [esp + 0x10]
            fld dword ptr [esp + 0x24]
            fstp qword ptr [esp + 8]
            fld dword ptr [esp + 0x20]
            _emit 0xdd
        _emit 0x1c
        _emit 0x24
            push offset gDefault_drone_direction+0xb18
            push eax
            push offset gDefault_drone_direction+0xb0
            call DoNotDprintf
            fld dword ptr [esp + 0x40]
            add esp, 0x24
            mov ecx, dword ptr [gFrame]
            sub esp, 8
            _emit 0xdd
        _emit 0x1c
        _emit 0x24
            fld dword ptr [esp + 0x20]
            sub esp, 8
            _emit 0xdd
        _emit 0x1c
        _emit 0x24
            fld dword ptr [esp + 0x24]
            sub esp, 8
            _emit 0xdd
        _emit 0x1c
        _emit 0x24
            push offset gDefault_drone_direction+0xb04
            push ecx
            push offset gDefault_drone_direction+0xb0
            call DoNotDprintf
            fld dword ptr [esi + 0x20]
            add esp, 0x24
            mov edx, dword ptr [gFrame]
            sub esp, 8
            _emit 0xdd
        _emit 0x1c
        _emit 0x24
            fld dword ptr [esi + 0x1c]
            sub esp, 8
            _emit 0xdd
        _emit 0x1c
        _emit 0x24
            fld dword ptr [esi + 0x18]
            sub esp, 8
            _emit 0xdd
        _emit 0x1c
        _emit 0x24
            push offset gDefault_drone_direction+0xaec
            push edx
            push offset gDefault_drone_direction+0xb0
            call DoNotDprintf
            add esp, 0x24
            pop edi
            pop esi
            add esp, 0x18
            ret
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00453050
void C2_NAKED C2_HOOK_FASTCALL WriteOutDronePaths(void) {
#ifndef CARPOCALYPSE2_MATCHING
    volatile int node_index;
    int section_index;
    int path_index;
    char filename[16];
    char path[256];
    FILE* file;

    if (!gDrone_path_output_ready) {
        path_index = 0;
        sprintf(filename, "DPATH%0.3d.TXT", path_index);
        PathCat(path, gApplication_path, filename);
        file = DRfopen(path, "r+");
        while (file != NULL) {
            PFfclose(file);
            path_index++;
            sprintf(filename, "DPATH%0.3d.TXT", path_index);
            PathCat(path, gApplication_path, filename);
            file = DRfopen(path, "r+");
        }
        gDrone_path_output_ready = 1;
        strcpy(gDrone_path_output_name, path);
    }

    file = DRfopen(gDrone_path_output_name, "wt");
    if (file != NULL) {
        fprintf(file, "%s\n", "START OF DRONE PATHS");
        fprintf(file, "\n%-3d   // version\n", 2);
        fprintf(file, "%-3d   // n_nodes\n", gCount_drone_path_nodes);
        for (node_index = 0; node_index < gCount_drone_path_nodes; node_index++) {
            tDrone_path_node* node;

            node = &gDrone_path_nodes[node_index];
            fprintf(file, "\t\n// %d:\n", node_index);
            fprintf(file, "\t%.3f, %.3f, %.3f\n",
                (double)node->position.v[0],
                (double)node->position.v[1],
                (double)node->position.v[2]);
            fprintf(file, "\t%s\n", node->type >= 0 ? gDrone_forms[node->type].name : "NONE");
            fprintf(file, "\t%d\n", node->field_0x12e);
            fprintf(file, "\t%d\n", node->count_sections);
            for (section_index = 0; section_index < node->count_sections; section_index++) {
                tDrone_path_node_section* section;

                section = &node->sections[section_index];
                fprintf(file, "\t\t%d, %d, %d, %d\n",
                    section->node1,
                    (tU16)section->node2,
                    section->type,
                    section->field_0x21);
            }
        }
        fprintf(file, "\n%s\n", "END OF DRONE PATHS");
        PFfclose(file);
    } else {
        PDFatalError("Unable to open drone path output file");
    }
#else
    __asm {
            push ebp
            mov ebp, esp
            and esp, 0xfffffff8
            mov eax, dword ptr [gDrone_path_output_ready]
            sub esp, 0x118
            test eax, eax
            push ebx
            push ebp
            push esi
            push edi
            jne L00453115
            xor esi, esi
            lea eax, [esp + 0x18]
            push esi
            push offset gDefault_drone_direction+0xd18
            push eax
            call sprintf
            add esp, 0xc
            lea ecx, [esp + 0x18]
            mov edx, offset gApplication_path
            push ecx
            lea ecx, [esp + 0x2c]
            call PathCat
            mov edx, offset gDefault_drone_direction+0xd14
            lea ecx, [esp + 0x28]
            call DRfopen
            test eax, eax
            je L004530e7
    L004530a7:
            mov ecx, eax
            call PFfclose
            inc esi
            lea edx, [esp + 0x18]
            push esi
            push offset gDefault_drone_direction+0xd18
            push edx
            call sprintf
            add esp, 0xc
            lea eax, [esp + 0x18]
            mov edx, offset gApplication_path
            lea ecx, [esp + 0x28]
            push eax
            call PathCat
            mov edx, offset gDefault_drone_direction+0xd14
            lea ecx, [esp + 0x28]
            call DRfopen
            test eax, eax
            jne L004530a7
    L004530e7:
            lea edi, [esp + 0x28]
            or ecx, 0xffffffff
            xor eax, eax
            mov dword ptr [gDrone_path_output_ready], 1
            repne scasb
            not ecx
            sub edi, ecx
            mov edx, ecx
            mov esi, edi
            mov edi, offset gDrone_path_output_name
            shr ecx, 2
            rep movsd
            mov ecx, edx
            and ecx, 3
            rep movsb
    L00453115:
            mov edx, offset gDefault_drone_direction+0xd10
            mov ecx, offset gDrone_path_output_name
            call DRfopen
            mov ebx, eax
            test ebx, ebx
            je L004532c2
            push offset gDefault_drone_direction+0x91c
            push offset gDefault_drone_direction+0xd0c
            push ebx
            call fprintf
            add esp, 0xc
            push 2
            push offset gDefault_drone_direction+0xcf8
            push ebx
            call fprintf
            mov eax, dword ptr [gCount_drone_path_nodes]
            add esp, 0xc
            push eax
            push offset gDefault_drone_direction+0xce4
            push ebx
            call fprintf
            mov eax, dword ptr [gCount_drone_path_nodes]
            add esp, 0xc
            xor esi, esi
            test eax, eax
            mov dword ptr [esp + 0x14], esi
            jle L004532a0
            xor edi, edi
    L0045317a:
            push esi
            push offset gDefault_drone_direction+0xcd8
            push ebx
            call fprintf
            mov eax, dword ptr [gDrone_path_nodes]
            add esp, 0xc
            sub esp, 8
            fld dword ptr [eax + edi + 8]
            _emit 0xdd
        _emit 0x1c
        _emit 0x24
            fld dword ptr [eax + edi + 4]
            sub esp, 8
            _emit 0xdd
        _emit 0x1c
        _emit 0x24
            fld dword ptr [eax + edi]
            sub esp, 8
            _emit 0xdd
        _emit 0x1c
        _emit 0x24
            push offset gDefault_drone_direction+0xcc4
            push ebx
            call fprintf
            mov ecx, dword ptr [gDrone_path_nodes]
            add esp, 0x20
            mov ax, word ptr [ecx + edi + 0x130]
            test ax, ax
            jl L004531e0
            movsx eax, ax
            mov edx, eax
            shl edx, 4
            add edx, eax
            lea eax, [edx*8 + gDrone_forms]
            push eax
            jmp L004531e5
    L004531e0:
            push offset gPed_flags_0058f2e4+0xd8
    L004531e5:
            push offset gDefault_drone_direction+0xcbc
            push ebx
            call fprintf
            mov edx, dword ptr [gDrone_path_nodes]
            xor ecx, ecx
            add esp, 0xc
            mov cl, byte ptr [edx + edi + 0x12e]
            push ecx
            push offset gDefault_drone_direction+0xcb4
            push ebx
            call fprintf
            mov ecx, dword ptr [gDrone_path_nodes]
            xor eax, eax
            add esp, 0xc
            mov ax, word ptr [ecx + edi + 0x12c]
            push eax
            push offset gDefault_drone_direction+0xcb4
            push ebx
            call fprintf
            mov eax, dword ptr [gDrone_path_nodes]
            xor ebp, ebp
            add esp, 0xc
            cmp word ptr [eax + edi + 0x12c], bp
            jbe L00453288
            mov esi, edi
    L00453243:
            xor edx, edx
            xor ecx, ecx
            mov dl, byte ptr [eax + esi + 0x2c]
            mov cl, byte ptr [eax + esi + 0x2d]
            push edx
            xor edx, edx
            mov dx, word ptr [eax + esi + 0xe]
            push ecx
            movsx eax, word ptr [eax + esi + 0xc]
            push edx
            push eax
            push offset gDefault_drone_direction+0xca0
            push ebx
            call fprintf
            mov eax, dword ptr [gDrone_path_nodes]
            xor ecx, ecx
            add esp, 0x18
            inc ebp
            mov cx, word ptr [eax + edi + 0x12c]
            add esi, 0x24
            cmp ebp, ecx
            jl L00453243
            mov esi, dword ptr [esp + 0x14]
    L00453288:
            mov eax, dword ptr [gCount_drone_path_nodes]
            inc esi
            add edi, 0x134
            cmp esi, eax
            mov dword ptr [esp + 0x14], esi
            jl L0045317a
    L004532a0:
            push offset gDefault_drone_direction+0x818
            push offset gDefault_drone_direction+0xc98
            push ebx
            call fprintf
            add esp, 0xc
            mov ecx, ebx
            call PFfclose
            pop edi
            pop esi
            pop ebp
            pop ebx
            mov esp, ebp
            pop ebp
            ret
    L004532c2:
            mov ecx, offset gDefault_drone_direction+0xc70
            call PDFatalError
            pop edi
            pop esi
            pop ebp
            pop ebx
            mov esp, ebp
            pop ebp
            ret
    }
#endif
}
// FUNCTION: CARMA2_HW 0x004525c0
void C2_HOOK_FASTCALL DroneNoOp(void) {
}

// FUNCTION: CARMA2_HW 0x00452f40
void C2_HOOK_FASTCALL FUN_00452f40(br_model* pModel, int pFace_count, int pVertex_count) {
    br_vertex* new_vertices;
    br_face* new_faces;
    int vertex_size;
    int face_size;
    int offset;
    int i;

    vertex_size = pVertex_count * sizeof(br_vertex);
    new_vertices = BrResAllocate(pModel, vertex_size, BR_MEMORY_VERTICES);
    BrBlockFill((br_uint_32*)new_vertices, 0, vertex_size / sizeof(br_uint_32));
    if (pModel->nvertices != 0) {
        i = 0;
        offset = 0;
        while (i < (pModel->nvertices < pVertex_count ? pModel->nvertices : pVertex_count)) {
            memcpy(&new_vertices[i], &pModel->vertices[i], sizeof(br_vertex));
            i++;
            offset += sizeof(br_vertex);
        }
        BrResRemove(pModel->vertices);
        BrResFree(pModel->vertices);
    }
    pModel->nvertices = (br_uint_16)pVertex_count;
    pModel->vertices = new_vertices;

    face_size = pFace_count * sizeof(br_face);
    new_faces = BrResAllocate(pModel, face_size, BR_MEMORY_FACES);
    BrBlockFill((br_uint_32*)new_faces, 0, face_size / sizeof(br_uint_32));
    if (pModel->nfaces != 0) {
        i = 0;
        offset = 0;
        while (i < (pModel->nfaces < pFace_count ? pModel->nfaces : pFace_count)) {
            memcpy(&new_faces[i], &pModel->faces[i], sizeof(br_face));
            i++;
            offset += sizeof(br_face);
        }
        BrResRemove(pModel->faces);
        BrResFree(pModel->faces);
    }
    pModel->nfaces = (br_uint_16)pFace_count;
    pModel->faces = new_faces;
}
