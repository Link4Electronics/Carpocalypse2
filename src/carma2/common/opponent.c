#include "opponent.h"

#include "car.h"
#include "controls.h"

#include "crush.h"

#include "displays.h"

#include "drone.h"

#include "errors.h"
#include "finteray.h"

#include "graphics.h"

#include "globvars.h"
#include "globvrkm.h"
#include "globvrme.h"
#include "globvrpb.h"
#include "loading.h"
#include "mainloop.h"
#include "oppoproc.h"
#include "physics.h"
#include "piping.h"
#include "platform.h"

#include "skidmark.h"

#include "structur.h"
#include "trig.h"
#include "utility.h"

#include <brender/brender.h>

#include "carpocalypse2_macros.h"

#include "c2_string.h"

#include <stdarg.h>
#include "c2_math.h"


extern const br_scalar gFLOAT_0058a61c;

// GLOBAL: CARMA2_HW 0x0058a620
const br_scalar gFLOAT_0058a620 = 6.9f;

// GLOBAL: CARMA2_HW 0x0058a69c
const br_scalar gFLOAT_0058a69c = 0.0625f;

// GLOBAL: CARMA2_HW 0x0058a6a0
const br_scalar gFLOAT_0058a6a0 = -0.001f;

// GLOBAL: CARMA2_HW 0x0058a6a4
const br_scalar gFLOAT_0058a6a4 = 0.14492753148078918f;

#include <float.h>

#define CAR_SPEC_IS_ROZZER(CAR_SPEC) (VEHICLE_TYPE_FROM_ID((tU16)(CAR_SPEC)->car_ID) == eVehicle_rozzer)
#define CAR_SPEC_IS_OPPONENT(CAR_SPEC) (VEHICLE_TYPE_FROM_ID((tU16)(CAR_SPEC)->car_ID) == eVehicle_opponent)

#define GET_CAR_SPEED_FACTOR(CAR) (CAR_SPEC_IS_ROZZER(CAR) ? gCop_speed_factor : gOpponent_speed_factor)
#include "opponent.h"
#include "globvars.h"

#include <c2_hooks.h>

/* PC53 operations retain the original x87 rounding and NaN operand selection. */
static double C2_HOOK_CDECL OpponentFloatDifference(const float* pA, const float* pB) {
    double result;
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        mov eax, pA
        mov edx, pB
        fld dword ptr [eax]
        fsub dword ptr [edx]
        fstp qword ptr [result]
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ volatile ("flds %1; fsubs %2; fstpl %0" : "=m"(result) : "m"(*pA), "m"(*pB) : "st");
#else
    result = (double)*pA - (double)*pB;
#endif
    return result;
}

static double C2_HOOK_CDECL OpponentAdd(double pA, double pB) {
    volatile double result = pA + pB;
    return result;
}

static double C2_HOOK_CDECL OpponentSubtract(double pA, double pB) {
    volatile double result = pA - pB;
    return result;
}

static double C2_HOOK_CDECL OpponentMultiply(double pA, double pB) {
    volatile double result = pA * pB;
    return result;
}

static double C2_HOOK_CDECL OpponentDivide(double pA, double pB) {
    volatile double result = pA / pB;
    return result;
}

static double C2_HOOK_CDECL OpponentSqrt(double pA) {
    double result;
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        fld qword ptr [pA]
        fsqrt
        fstp qword ptr [result]
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ volatile ("fldl %1; fsqrt; fstpl %0" : "=m"(result) : "m"(pA) : "st");
#else
    result = sqrt(pA);
#endif
    return result;
}

static float C2_HOOK_CDECL OpponentScalarRound(double pA) {
    volatile union { float scalar; tU32 bits; } stored;
    union { float scalar; tU32 bits; } result;

    stored.scalar = (float)pA;
    result.bits = stored.bits;
    return result.scalar;
}

static double C2_HOOK_CDECL OpponentHybridLengthSquared(double pX, double pY, double pZ) {
    float x = OpponentScalarRound(pX);
    float y = OpponentScalarRound(pY);
    return OpponentAdd(OpponentAdd(OpponentMultiply(pX, x), OpponentMultiply(pY, y)), OpponentMultiply(pZ, pZ));
}

// GLOBAL: CARMA2_HW 0x0069173c
int gActive_car_list_rebuild_required;

// GLOBAL: CARMA2_HW 0x0065a3bc
const char* gFaceless_cop_name = "Faceless Cop";

// GLOBAL: CARMA2_HW 0x0065a3c0
const char* gInnocent_civilian_name = "Innocent Civilian";

// GLOBAL: CARMA2_HW 0x0065a3c4
int gBIG_APC_index = -1;

// GLOBAL: CARMA2_HW 0x00691744
int gNumber_of_cops_before_faffage;

// GLOBAL: CARMA2_HW 0x00691754
tU32 gNext_grudge_reduction;

// GLOBAL: CARMA2_HW 0x00691738
int gFirst_frame;

// GLOBAL: CARMA2_HW 0x00691724
int gGrudge_reduction_per_period;

// GLOBAL: CARMA2_HW 0x00691750
int gAcknowledged_start;

// GLOBAL: CARMA2_HW 0x00691720
int gStart_jumped;

// GLOBAL: CARMA2_HW 0x00691760
br_scalar gMinimum_yness_before_knackerisation;

// GLOBAL: CARMA2_HW 0x00691728
tU32 gAcme_frame_count;

// GLOBAL: CARMA2_HW 0x00691768
br_scalar gHead_on_cos_value;

// GLOBAL: CARMA2_HW 0x00691714
int gBig_bang;

// GLOBAL: CARMA2_HW 0x00691764
br_scalar gIn_view_distance;

// GLOBAL: CARMA2_HW 0x0065a3c8
int gChallenger_index__opponent = -1;

// GLOBAL: CARMA2_HW 0x0069175c
tU8* gBit_per_node;

// GLOBAL: CARMA2_HW 0x0069171c
br_scalar gSFS_shortest_dist;

// GLOBAL: CARMA2_HW 0x0069172c
int gSFS_routes_found;

// GLOBAL: CARMA2_HW 0x0069174c
int gSFS_routes_found_copy;

// GLOBAL: CARMA2_HW 0x00691770
int gSFS_count;

// GLOBAL: CARMA2_HW 0x0069176c
int gSFS_cycles_this_time;

// GLOBAL: CARMA2_HW 0x00691770
int gSFS_search_count;

// GLOBAL: CARMA2_HW 0x00691774
int gSFS_total_cycles;

// GLOBAL: CARMA2_HW 0x00691778
int gSFS_max_cycles;

// GLOBAL: CARMA2_HW 0x0058a47c
float DAT_0058a47c = 100.0f;

// GLOBAL: CARMA2_HW 0x0058a55c
float DAT_0058a55c = -0.0625f;

// GLOBAL: CARMA2_HW 0x0058a4e0
double DAT_0058a4e0 = 0.14492753623188406;

// GLOBAL: CARMA2_HW 0x0058a4a0
const double DAT_0058a4a0 = 1.0;

// GLOBAL: CARMA2_HW 0x0058a4a8
const double DAT_0058a4a8 = 10430.378350470453;

// GLOBAL: CARMA2_HW 0x0058a4b0
const double DAT_0058a4b0 = 6.9;

// GLOBAL: CARMA2_HW 0x0058a4b8
const float DAT_0058a4b8 = -1.0f;

// GLOBAL: CARMA2_HW 0x0058a4bc
const float DAT_0058a4bc = -100.0f;

// GLOBAL: CARMA2_HW 0x0058a4c0
const float DAT_0058a4c0 = -5.0f;

// GLOBAL: CARMA2_HW 0x0058a4c4
const float DAT_0058a4c4 = -0.05f;

// GLOBAL: CARMA2_HW 0x0058a4c8
const float DAT_0058a4c8 = -0.05f;

// GLOBAL: CARMA2_HW 0x0058a4d0
const double DAT_0058a4d0 = -10430.378350470453;

// GLOBAL: CARMA2_HW 0x0058a4d8
const float DAT_0058a4d8 = 0.14492753148078918f;

// GLOBAL: CARMA2_HW 0x0058a4e8
const float DAT_0058a4e8 = 0.0000002384185791015625f;

// GLOBAL: CARMA2_HW 0x0058a4ec
const float DAT_0058a4ec = 10.0f;

// GLOBAL: CARMA2_HW 0x0058a4f0
const float DAT_0058a4f0 = 0.1f;

// GLOBAL: CARMA2_HW 0x0069177c
int gRematerialisation_count;

// GLOBAL: CARMA2_HW 0x00691780
int gRematerialisation_extra_attempts;

// GLOBAL: CARMA2_HW 0x00691784
int gRematerialisation_max_extra_attempts;

// GLOBAL: CARMA2_HW 0x00676858
br_material* gMat_dk_yel = NULL;

// GLOBAL: CARMA2_HW 0x0067685c
br_material* gMat_md_yel = NULL;

// GLOBAL: CARMA2_HW 0x00676860
br_material* gMat_lt_yel = NULL;

// GLOBAL: CARMA2_HW 0x00676864
br_material* gMat_dk_red = NULL;

// GLOBAL: CARMA2_HW 0x00676868
br_material* gMat_md_red = NULL;

// GLOBAL: CARMA2_HW 0x0067686c
br_material* gMat_lt_red = NULL;

// GLOBAL: CARMA2_HW 0x00676870
br_material* gMat_dk_grn = NULL;

// GLOBAL: CARMA2_HW 0x00676874
br_material* gMat_md_grn = NULL;

// GLOBAL: CARMA2_HW 0x00676878
br_material* gMat_lt_grn = NULL;

// GLOBAL: CARMA2_HW 0x0067687c
br_material* gMat_dk_blu = NULL;

// GLOBAL: CARMA2_HW 0x00676880
br_material* gMat_lt_blu = NULL;

// GLOBAL: CARMA2_HW 0x00676884
br_material* gMat_dk_turq = NULL;

// GLOBAL: CARMA2_HW 0x00676888
br_material* gMat_lt_turq = NULL;

// GLOBAL: CARMA2_HW 0x0067688c
br_material* gMat_dk_gry = NULL;

// GLOBAL: CARMA2_HW 0x00676890
br_material* gMat_md_gry = NULL;

// GLOBAL: CARMA2_HW 0x00676894
br_material* gMat_lt_gry = NULL;

// GLOBAL: CARMA2_HW 0x00691718
int gNum_of_opponents_pursuing;

// GLOBAL: CARMA2_HW 0x00691740
int gNum_of_opponents_getting_near;

// GLOBAL: CARMA2_HW 0x00691758
int gNum_of_opponents_completing_race;

// GLOBAL: CARMA2_HW 0x00691730
float gCop_pursuit_speed_value_00691730;

// GLOBAL: CARMA2_HW 0x00691734
float gDefinite_no_cop_pursuit_speed;

// GLOBAL: CARMA2_HW 0x00691748
float gCop_pursuit_speed_percentage_multiplier;

// GLOBAL: CARMA2_HW 0x0058a478
float gOppo_render_zero = 0.0f;

// GLOBAL: CARMA2_HW 0x0058a480
float gOppo_pursuit_one = 1.0f;

// GLOBAL: CARMA2_HW 0x0058a488
float gOppo_pursuit_reverse_brake = 15.0f;

// GLOBAL: CARMA2_HW 0x0058a48c
float gOppo_pursuit_velocity_scale = 0.5f;

// GLOBAL: CARMA2_HW 0x0058a494
float gOppo_pursuit_minimum_velocity_ratio = 2.0f;

// GLOBAL: CARMA2_HW 0x0058a504
float gOppo_pursuit_height_offset = -0.15f;

// GLOBAL: CARMA2_HW 0x0058a50c
float gOppo_pursuit_precinct_distance = 75.0f;

// GLOBAL: CARMA2_HW 0x0058a510
float gOppo_pursuit_near_distance = 3.0f;

// GLOBAL: CARMA2_HW 0x0058a514
float gOppo_pursuit_stationary_speed = 0.043478261679410934f;

// GLOBAL: CARMA2_HW 0x0058a518
float gOppo_pursuit_cheat_distance = 30.0f;

// GLOBAL: CARMA2_HW 0x0058a51c
float gOppo_pursuit_sight_distance = 35.0f;

// GLOBAL: CARMA2_HW 0x0058a520
float gOppo_pursuit_trail_distance = 20.0f;

// GLOBAL: CARMA2_HW 0x0058a524
float gOppo_pursuit_reverse_acceleration = -8.0f;

// GLOBAL: CARMA2_HW 0x0065a3cc
float gOpponent_nastyness_frigger = 1.0f;

// GLOBAL: CARMA2_HW 0x0058a5a8
const double gCop_nastiness_skill_adjustment = -0.5;

// GLOBAL: CARMA2_HW 0x0058a5d8
const double gCop_nastiness_bias = -2.25;

// GLOBAL: CARMA2_HW 0x0058a5b0
const double gCop_nastiness_divisor = 0.33333333333333331;

// GLOBAL: CARMA2_HW 0x0074a684
int gMinTimeOpponentRepair;

// GLOBAL: CARMA2_HW 0x0074a688
int gMaxTimeOpponentRepair;

#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern br_material* C2_HOOK_CDECL BrMaterialAdd();
extern br_material* C2_HOOK_CDECL BrMaterialAllocate();
extern void C2_HOOK_FASTCALL CalcDistanceFromHome(tOpponent_spec* pOpponent_spec);
extern br_material* C2_HOOK_FASTCALL CreateSimpleMaterial(int pIndex);
extern void C2_HOOK_FASTCALL FUN_004af490(void);
extern br_scalar C2_HOOK_FASTCALL FastScalarCos(int pAngle_in_degrees);
extern const br_vector3* C2_HOOK_FASTCALL GetOpponentsSectionFinishNodePoint(const tOpponent_spec* pOpponent_spec, tS16 pSection);
extern int C2_HOOK_FASTCALL GetOpponentsSectionMaxSpeed(tOpponent_spec* pOpponent_spec, tS16 pSection, int pTowards_finish);
extern const br_vector3* C2_HOOK_FASTCALL GetOpponentsSectionStartNodePoint(const tOpponent_spec* pOpponent_spec, tS16 pSection);
extern void C2_HOOK_FASTCALL MakeFaceAndTextureIt(br_model* pModel, int pFace_num, br_uint_16 pV0, br_uint_16 pV1, br_uint_16 pV2, br_material* pMaterial);
extern void C2_HOOK_FASTCALL PipeSingleOppoRenderage(tOpponent_spec *pOpponent_spec, int pEnable);
extern void C2_HOOK_FASTCALL RebuildActiveCarList(void);
extern void C2_HOOK_FASTCALL ShowDronePaths(void);
extern br_uint_8 bl;
extern int gCountdown;
extern tU32 gFrame_period_for_this_munging;
extern tOpponent* gOpponents;
extern tProgram_state gProgram_state;
extern br_pixelmap* gRender_palette;
extern tU32 gTime_stamp_for_this_munging;
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_CDECL BrFatal();
extern void C2_HOOK_CDECL BrMatrix34ApplyP();
extern void C2_HOOK_CDECL BrMatrix34LPInverse();
extern void* C2_HOOK_CDECL BrMemAllocate();
extern void C2_HOOK_CDECL BrMemFree();
extern int C2_HOOK_FASTCALL CalcCorners(tCorner* pCorners, int pSection, float pWidth, const br_vector2* pStart, tOpponent_spec* pOpponent_spec);
extern int C2_HOOK_FASTCALL CalcSOCs(int pNext_section, int pCount_corners, tCorner *pCorners, tOpponent_spec *pOpponent_spec, tSOC* pSocs, tCar_spec* pCar);
extern void C2_HOOK_FASTCALL ChangeHeadupText(int pHeadup_index, char* pNew_text);
extern void C2_HOOK_FASTCALL ChooseNewObjective(tOpponent_spec* pOpponent_spec, int pMust_choose_one);
extern void C2_HOOK_CDECL DoNotDprintf_opponent();
extern void C2_HOOK_CDECL DoNotDprintf_oppoproc();
extern void C2_HOOK_FASTCALL FindFace(br_vector3* pPosition, br_vector3* pDir, br_vector3* nor, br_scalar* t, br_material** material);
extern tS16 C2_HOOK_FASTCALL FindNearestGeneralSection(tCar_spec* pCar, const br_vector3* pActor_coords, br_vector3* pPath_direction, br_vector3* pIntersect, br_scalar* pDistance);
extern char * C2_HOOK_FASTCALL GetALineAndDontArgue(FILE* pF, char* pS);
extern int C2_HOOK_FASTCALL GetAnInt(FILE* pF);
extern int C2_HOOK_FASTCALL GetCarCount(tVehicle_type pCategory);
extern tCar_spec* C2_HOOK_FASTCALL GetCarSpec(tVehicle_type pCategory, int pIndex);
extern const char* C2_HOOK_FASTCALL GetMiscString(int pIndex);
extern void C2_HOOK_FASTCALL GetNScalars(FILE* pF, int n, br_scalar* pS);
extern int C2_HOOK_FASTCALL GetOpponentsFirstSection(const tOpponent_spec* pOpponent_spec);
extern int C2_HOOK_FASTCALL GetOpponentsNextSection(const tOpponent_spec* pOpponent_spec, tS16 pCurrent_section);
extern int C2_HOOK_FASTCALL GetOpponentsRealSection(tOpponent_spec* pOpponent_spec, int pSection_no);
extern int C2_HOOK_FASTCALL GetOpponentsSectionMinSpeed(tOpponent_spec* pOpponent_spec, tS16 pSection, int pTowards_finish);
extern float C2_HOOK_FASTCALL GetOpponentsSectionWidth(const tOpponent_spec* pOpponent_spec, tS16 pSection);
extern int C2_HOOK_FASTCALL GetStraight(br_vector2* pStart, br_vector2* pFinish, float* pWidth, int pSection, tOpponent_spec* pOpponent_spec);
extern void C2_HOOK_FASTCALL GetThreeFloats(FILE * pF, float* pF1, float* pF2, float* pF3);
extern int C2_HOOK_FASTCALL IRandomBetween(int pA, int pB);
extern void C2_HOOK_FASTCALL NewCalcGetNearPlayerRoute(tOpponent_spec* pOpponent_spec, tCar_spec* pPursuee, int pForce);
extern void C2_HOOK_CDECL NewObjective();
extern int C2_HOOK_FASTCALL NewTextHeadupSlot(int pSlot_index, int pFlash_rate, int pLifetime, int pFont_index, const char* pText);
extern void C2_HOOK_FASTCALL PDEnterDebugger(const char* pStr);
extern void C2_NORETURN C2_HOOK_FASTCALL PDFatalError(const char* pThe_str);
extern int C2_HOOK_FASTCALL PHILAddActiveObject(tPhysics_object* pInfo, undefined4* pArg2, const br_vector3* pArg3, const br_vector3* pArg4);
extern tPhysics_object* C2_HOOK_FASTCALL PHILGetFirstObject(void);
extern tPhysics_object* C2_HOOK_FASTCALL PHILGetNextObject(tPhysics_object* pCollision_info);
extern void C2_HOOK_FASTCALL PossibleService(void);
extern void C2_HOOK_FASTCALL ProcessCurrentObjective(tOpponent_spec* pOpponent_spec, tProcess_objective_command pCommand);
extern int C2_HOOK_FASTCALL RematerialiseOpponentOnNearestSection(tOpponent_spec* pOpponent_spec, br_scalar pSpeed);
extern int C2_HOOK_FASTCALL RematerialiseOpponentOnThisSection(tOpponent_spec* pOpponent_spec, tS16 pSection_no, float pSpeed);
extern int C2_HOOK_FASTCALL ShiftOpponentsProjectedRoute(tOpponent_spec* pOpponent_spec, int pPlaces);
extern void __cdecl _ftol();
extern char _CIpow[];
extern tCar_spec* gActive_car_list[139];
extern int gAusterity_mode;
extern tCar_spec* gCar_to_view;
extern float gCop_speed_factor;
extern char gDOUBLE_0058a630[];
extern char gDefault_drone_direction[];
extern int gDisable_sound;
extern char gFLOAT_0058a638[];
extern char gFLOAT_0058a644[];
extern char gFLOAT_0058a64c[];
extern char gFLOAT_0058a670[];
extern int gMap_view;
extern tNet_mode gNet_mode;
extern tNet_game_player_info gNet_players[12];
extern int gNum_active_cars;
extern int gNumber_of_net_players;
extern float gOpponent_speed_factor;
extern int gPed_676914;
extern tU32 gRace_finished;
extern int gShow_opponents;
extern int gStop_opponents_moving;
extern int gThis_net_player_index;
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern int C2_HOOK_FASTCALL HasCarFallenOffWorld(tCar_spec* pCar);
extern int C2_HOOK_FASTCALL PercentageChance(int pC);
extern int C2_HOOK_FASTCALL SearchForSection(tRoute_section* pTemp_store, tRoute_section* pPerm_store, int* pNum_of_perm_store_sections, tS16 pTarget_section, int pDepth, br_scalar pDistance_so_far, tOpponent_spec* pOpponent_spec);
extern void C2_HOOK_FASTCALL TeleportOpponentToNearestSafeLocation(tOpponent_spec* pOpponent_spec);
#endif
// FUNCTION: CARMA2_HW 0x004a7a20
void C2_HOOK_FASTCALL SetOppoRender(tOpponent_spec* pOpponent_spec, int pRender) {
    if (gAction_replay_mode && ARGetReplayRate() < gOppo_render_zero) {
        pRender = (pRender == 0);
    }
    pOpponent_spec->car_spec->car_master_actor->render_style = (pRender == 0) ? BR_RSTYLE_NONE : BR_RSTYLE_DEFAULT;
}

// FUNCTION: CARMA2_HW 0x004013d0
void C2_HOOK_FASTCALL PointActorAlongThisBloodyVector(br_actor* pThe_actor, br_vector3* pThe_vector) {
    br_transform trans;

    trans.type = BR_TRANSFORM_LOOK_UP;
    BrVector3Copy(&trans.t.look_up.t, &pThe_actor->t.t.translate.t);
    BrVector3Copy(&trans.t.look_up.look, pThe_vector);
    BrVector3Set(&trans.t.look_up.up, 0.f, 1.f, 0.f);
    BrTransformToTransform(&pThe_actor->t, &trans);
}

// FUNCTION: CARMA2_HW 0x00401430
void C2_HOOK_FASTCALL PointActorAlongVectorWithUp(br_actor* pThe_actor, br_vector3* pLook, br_vector3* pUp) {
    br_transform trans;

    trans.type = BR_TRANSFORM_LOOK_UP;
    BrVector3Copy(&trans.t.look_up.t, &pThe_actor->t.t.translate.t);
    BrVector3Copy(&trans.t.look_up.look, pLook);
    BrVector3Copy(&trans.t.look_up.up, pUp);
    BrTransformToTransform(&pThe_actor->t, &trans);
}

// GLOBAL: CARMA2_HW 0x00676850
int gPalette_materials_initialized;

// FUNCTION: CARMA2_HW 0x004014a0
void C2_NAKED C2_HOOK_FASTCALL InitialiseOpponentPaletteMaterials(void) {
#ifndef CARPOCALYPSE2_MATCHING
    br_material* material;
    br_colour* palette_colours;
    br_colour palette_colour;

    if (!gPalette_materials_initialized) {
        material = BrMaterialAllocate(NULL);
        palette_colours = gRender_palette->pixels;
        palette_colour = palette_colours[0x32];
        material->index_base = 0x32;
        material->colour = palette_colour;
        material->index_range = 1;
        material->flags = 0x1000;
        material->index_shade = NULL;
        material->colour_map = NULL;
        material->identifier = NULL;
        BrMaterialAdd(material);
        gMat_dk_yel = material;

        material = BrMaterialAllocate(NULL);
        palette_colours = gRender_palette->pixels;
        palette_colour = palette_colours[0x33];
        material->index_base = 0x33;
        material->colour = palette_colour;
        material->index_range = 1;
        material->flags = 0x1000;
        material->index_shade = NULL;
        material->colour_map = NULL;
        material->identifier = NULL;
        BrMaterialAdd(material);
        gMat_md_yel = material;

        material = BrMaterialAllocate(NULL);
        palette_colours = gRender_palette->pixels;
        palette_colour = palette_colours[0x34];
        material->index_base = 0x34;
        material->colour = palette_colour;
        material->index_range = 1;
        material->flags = 0x1000;
        material->index_shade = NULL;
        material->colour_map = NULL;
        material->identifier = NULL;
        BrMaterialAdd(material);
        gMat_lt_yel = material;

        material = BrMaterialAllocate(NULL);
        palette_colours = gRender_palette->pixels;
        palette_colour = palette_colours[2];
        material->index_base = 2;
        material->colour = palette_colour;
        material->index_range = 1;
        material->flags = 0x1000;
        material->index_shade = NULL;
        material->colour_map = NULL;
        material->identifier = NULL;
        BrMaterialAdd(material);
        gMat_dk_red = material;

        material = BrMaterialAllocate(NULL);
        palette_colours = gRender_palette->pixels;
        palette_colour = palette_colours[3];
        material->index_base = 3;
        material->colour = palette_colour;
        material->index_range = 1;
        material->flags = 0x1000;
        material->index_shade = NULL;
        material->colour_map = NULL;
        material->identifier = NULL;
        BrMaterialAdd(material);
        gMat_md_red = material;

        material = BrMaterialAllocate(NULL);
        palette_colours = gRender_palette->pixels;
        palette_colour = palette_colours[4];
        material->index_base = 4;
        material->colour = palette_colour;
        material->index_range = 1;
        material->flags = 0x1000;
        material->index_shade = NULL;
        material->colour_map = NULL;
        material->identifier = NULL;
        BrMaterialAdd(material);
        gMat_lt_red = material;

        material = BrMaterialAllocate(NULL);
        palette_colours = gRender_palette->pixels;
        palette_colour = palette_colours[0x42];
        material->index_base = 0x42;
        material->colour = palette_colour;
        material->index_range = 1;
        material->flags = 0x1000;
        material->index_shade = NULL;
        material->colour_map = NULL;
        material->identifier = NULL;
        BrMaterialAdd(material);
        gMat_dk_grn = material;

        material = BrMaterialAllocate(NULL);
        palette_colours = gRender_palette->pixels;
        palette_colour = palette_colours[0x43];
        material->index_base = 0x43;
        material->colour = palette_colour;
        material->index_range = 1;
        material->flags = 0x1000;
        material->index_shade = NULL;
        material->colour_map = NULL;
        material->identifier = NULL;
        BrMaterialAdd(material);
        gMat_md_grn = material;

        material = BrMaterialAllocate(NULL);
        palette_colours = gRender_palette->pixels;
        palette_colour = palette_colours[0x44];
        material->index_base = 0x44;
        material->colour = palette_colour;
        material->index_range = 1;
        material->flags = 0x1000;
        material->index_shade = NULL;
        material->colour_map = NULL;
        material->identifier = NULL;
        BrMaterialAdd(material);
        gMat_lt_grn = material;

        gMat_dk_blu = CreateSimpleMaterial(0xa2);
        gMat_lt_blu = CreateSimpleMaterial(0xa4);
        gMat_dk_turq = CreateSimpleMaterial(0x82);
        gMat_lt_turq = CreateSimpleMaterial(0x84);
        gMat_dk_gry = CreateSimpleMaterial(0xfd);
        gMat_md_gry = CreateSimpleMaterial(0xfe);
        material = CreateSimpleMaterial(0xff);
        gPalette_materials_initialized = 1;
        gMat_lt_gry = material;
    }
    FUN_004af490();
    ShowDronePaths();
#else
    __asm {
            mov eax, dword ptr [gPalette_materials_initialized]
            push esi
            xor esi, esi
            cmp eax, esi
            jne L00401754
            push edi
            push ebp
            push ebx
            push esi
            call BrMaterialAllocate
            mov edi, eax
            mov eax, dword ptr [gRender_palette]
            add esp, 4
            mov ebx, 1
            mov ecx, dword ptr [eax + 8]
            mov ebp, 0x1000
            push edi
            mov edx, dword ptr [ecx + 0xc8]
            mov byte ptr [edi + 0x3c], 0x32
            mov dword ptr [edi + 8], edx
            mov byte ptr [edi + 0x3d], bl
            mov dword ptr [edi + 0x20], ebp
            mov dword ptr [edi + 0x48], esi
            mov dword ptr [edi + 0x40], esi
            mov dword ptr [edi + 4], esi
            call BrMaterialAdd
            add esp, 4
            mov dword ptr [gMat_dk_yel], edi
            push esi
            call BrMaterialAllocate
            mov edi, eax
            mov eax, dword ptr [gRender_palette]
            add esp, 4
            mov ecx, dword ptr [eax + 8]
            push edi
            mov edx, dword ptr [ecx + 0xcc]
            mov byte ptr [edi + 0x3c], 0x33
            mov dword ptr [edi + 8], edx
            mov byte ptr [edi + 0x3d], bl
            mov dword ptr [edi + 0x20], ebp
            mov dword ptr [edi + 0x48], esi
            mov dword ptr [edi + 0x40], esi
            mov dword ptr [edi + 4], esi
            call BrMaterialAdd
            add esp, 4
            mov dword ptr [gMat_md_yel], edi
            push esi
            call BrMaterialAllocate
            mov edi, eax
            mov eax, dword ptr [gRender_palette]
            add esp, 4
            mov ecx, dword ptr [eax + 8]
            push edi
            mov edx, dword ptr [ecx + 0xd0]
            mov byte ptr [edi + 0x3c], 0x34
            mov dword ptr [edi + 8], edx
            mov byte ptr [edi + 0x3d], bl
            mov dword ptr [edi + 0x20], ebp
            mov dword ptr [edi + 0x48], esi
            mov dword ptr [edi + 0x40], esi
            mov dword ptr [edi + 4], esi
            call BrMaterialAdd
            add esp, 4
            mov dword ptr [gMat_lt_yel], edi
            push esi
            call BrMaterialAllocate
            mov edi, eax
            mov eax, dword ptr [gRender_palette]
            add esp, 4
            mov ecx, dword ptr [eax + 8]
            push edi
            mov edx, dword ptr [ecx + 8]
            mov byte ptr [edi + 0x3c], 2
            mov dword ptr [edi + 8], edx
            mov byte ptr [edi + 0x3d], bl
            mov dword ptr [edi + 0x20], ebp
            mov dword ptr [edi + 0x48], esi
            mov dword ptr [edi + 0x40], esi
            mov dword ptr [edi + 4], esi
            call BrMaterialAdd
            add esp, 4
            mov dword ptr [gMat_dk_red], edi
            push esi
            call BrMaterialAllocate
            add esp, 4
            mov edi, eax
            mov eax, dword ptr [gRender_palette]
            push edi
            mov ecx, dword ptr [eax + 8]
            mov edx, dword ptr [ecx + 0xc]
            mov byte ptr [edi + 0x3c], 3
            mov dword ptr [edi + 8], edx
            mov byte ptr [edi + 0x3d], bl
            mov dword ptr [edi + 0x20], ebp
            mov dword ptr [edi + 0x48], esi
            mov dword ptr [edi + 0x40], esi
            mov dword ptr [edi + 4], esi
            call BrMaterialAdd
            add esp, 4
            mov dword ptr [gMat_md_red], edi
            push esi
            call BrMaterialAllocate
            mov edi, eax
            mov eax, dword ptr [gRender_palette]
            add esp, 4
            mov ecx, dword ptr [eax + 8]
            push edi
            mov edx, dword ptr [ecx + 0x10]
            mov byte ptr [edi + 0x3c], 4
            mov dword ptr [edi + 8], edx
            mov byte ptr [edi + 0x3d], bl
            mov dword ptr [edi + 0x20], ebp
            mov dword ptr [edi + 0x48], esi
            mov dword ptr [edi + 0x40], esi
            mov dword ptr [edi + 4], esi
            call BrMaterialAdd
            add esp, 4
            mov dword ptr [gMat_lt_red], edi
            push esi
            call BrMaterialAllocate
            mov edi, eax
            mov eax, dword ptr [gRender_palette]
            add esp, 4
            mov ecx, dword ptr [eax + 8]
            push edi
            mov edx, dword ptr [ecx + 0x108]
            mov byte ptr [edi + 0x3c], 0x42
            mov dword ptr [edi + 8], edx
            mov byte ptr [edi + 0x3d], bl
            mov dword ptr [edi + 0x20], ebp
            mov dword ptr [edi + 0x48], esi
            mov dword ptr [edi + 0x40], esi
            mov dword ptr [edi + 4], esi
            call BrMaterialAdd
            add esp, 4
            mov dword ptr [gMat_dk_grn], edi
            push esi
            call BrMaterialAllocate
            mov edi, eax
            mov eax, dword ptr [gRender_palette]
            add esp, 4
            mov ecx, dword ptr [eax + 8]
            push edi
            mov edx, dword ptr [ecx + 0x10c]
            mov byte ptr [edi + 0x3c], 0x43
            mov dword ptr [edi + 8], edx
            mov byte ptr [edi + 0x3d], bl
            mov dword ptr [edi + 0x20], ebp
            mov dword ptr [edi + 0x48], esi
            mov dword ptr [edi + 0x40], esi
            mov dword ptr [edi + 4], esi
            call BrMaterialAdd
            add esp, 4
            mov dword ptr [gMat_md_grn], edi
            push esi
            call BrMaterialAllocate
            mov edi, eax
            mov eax, dword ptr [gRender_palette]
            add esp, 4
            mov ecx, dword ptr [eax + 8]
            mov edx, dword ptr [ecx + 0x110]
            mov byte ptr [edi + 0x3c], 0x44
            mov dword ptr [edi + 8], edx
            mov byte ptr [edi + 0x3d], bl
            mov dword ptr [edi + 0x20], ebp
            mov dword ptr [edi + 0x48], esi
            mov dword ptr [edi + 0x40], esi
            push edi
            mov dword ptr [edi + 4], esi
            call BrMaterialAdd
            add esp, 4
            mov ecx, 0xa2
            mov dword ptr [gMat_lt_grn], edi
            call CreateSimpleMaterial
            mov ecx, 0xa4
            mov dword ptr [gMat_dk_blu], eax
            call CreateSimpleMaterial
            mov ecx, 0x82
            mov dword ptr [gMat_lt_blu], eax
            call CreateSimpleMaterial
            mov ecx, 0x84
            mov dword ptr [gMat_dk_turq], eax
            call CreateSimpleMaterial
            mov ecx, 0xfd
            mov dword ptr [gMat_lt_turq], eax
            call CreateSimpleMaterial
            mov ecx, 0xfe
            mov dword ptr [gMat_dk_gry], eax
            call CreateSimpleMaterial
            mov ecx, 0xff
            mov dword ptr [gMat_md_gry], eax
            call CreateSimpleMaterial
            mov dword ptr [gPalette_materials_initialized], ebx
            mov dword ptr [gMat_lt_gry], eax
            pop ebx
            pop ebp
            pop edi
    L00401754:
            call FUN_004af490
            call ShowDronePaths
            pop esi
            ret
    }
#endif
}

void C2_HOOK_FASTCALL ReallocExtraPathNodes(int pCount) {
    tPath_node* nodes;

    C2_HOOK_BUG_ON(sizeof(tPath_node) != 0x20);

    if (pCount != 0) {

        nodes = BrMemAllocate((gProgram_state.AI_vehicles.number_of_path_nodes + pCount) * sizeof(tPath_node), kMem_oppo_new_nodes);
        memcpy(nodes, gProgram_state.AI_vehicles.path_nodes, gProgram_state.AI_vehicles.number_of_path_nodes * sizeof(tPath_node));

        if (gProgram_state.AI_vehicles.path_nodes != NULL) {
            BrMemFree(gProgram_state.AI_vehicles.path_nodes);
        }
        gProgram_state.AI_vehicles.path_nodes = nodes;
        gProgram_state.AI_vehicles.number_of_path_nodes += pCount;
    }
    DoNotDprintf_opponent("ReallocExtraPathNodes(): Allocated %d bytes for %d path nodes", (pCount + gProgram_state.AI_vehicles.number_of_path_nodes) * sizeof(tPath_node), pCount);
}

void C2_HOOK_FASTCALL ReallocExtraPathSections(int pCount) {
    tPath_section* sections;

    C2_HOOK_BUG_ON(sizeof(tPath_section) != 0x14);

    if (pCount != 0) {

        sections = BrMemAllocate((gProgram_state.AI_vehicles.number_of_path_sections + pCount) * sizeof(tPath_section), kMem_oppo_new_sections);
        memcpy(sections, gProgram_state.AI_vehicles.path_sections, gProgram_state.AI_vehicles.number_of_path_sections * sizeof(tPath_section));

        if (gProgram_state.AI_vehicles.path_sections != NULL) {
            BrMemFree(gProgram_state.AI_vehicles.path_sections);
        }
        gProgram_state.AI_vehicles.path_sections = sections;
        gProgram_state.AI_vehicles.number_of_path_sections += pCount;
    }
    DoNotDprintf_opponent("ReallocExtraPathSections(): Allocated %d bytes for %d path sections", (pCount + gProgram_state.AI_vehicles.number_of_path_sections) * sizeof(tPath_section), pCount);
}

// FUNCTION: CARMA2_HW 0x004a9610
void C2_NAKED C2_HOOK_FASTCALL LoadInOppoPaths(FILE* pF) {
#ifndef CARPOCALYPSE2_MATCHING
    char s[256];
    int count_nodes;
    int count_sections;
    int i;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tProgram_state, current_car, 0xac);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tProgram_state, player_name, 0x1a10);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tProgram_state, special_volumes, 0x1c00);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tProgram_state, AI_vehicles, 0x1c20);
    C2_HOOK_BUG_ON(sizeof(tFollow_path_data) != 0x58);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFollow_path_data, struggle_time, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFollow_path_data, last_struggle_section, 0x30);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFollow_path_data, corner_width, 0x50);
    C2_HOOK_BUG_ON(sizeof(tOpponent_spec) != 0x1a4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tOpponent_spec, index, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tOpponent_spec, current_objective, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tOpponent_spec, player_to_oppo_v, 0x40);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tOpponent_spec, nnext_sections, 0x7c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tOpponent_spec, complete_race_data, 0xb8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tOpponent_spec, follow_path_data, 0xbc);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tOpponent_spec, pursue_car_data, 0x114);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tIntelligent_vehicles, number_of_path_nodes, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tIntelligent_vehicles, number_of_path_sections, 0xc);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tIntelligent_vehicles, cop_start_points, 0x10);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tIntelligent_vehicles, cop_start_vectors, 0x88);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tIntelligent_vehicles, opponents, 0x100);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tIntelligent_vehicles, cops, 0x3238);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tIntelligent_vehicles, path_nodes, 0x42a0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tIntelligent_vehicles, path_sections, 0x42a4);

    DoNotDprintf_opponent("Start of LoadInOppoPaths()...");
    gProgram_state.AI_vehicles.number_of_path_nodes = 0;
    gProgram_state.AI_vehicles.number_of_path_sections = 0;
    gProgram_state.AI_vehicles.path_nodes = NULL;
    gProgram_state.AI_vehicles.path_sections = NULL;
    gBit_per_node = NULL;
    gBIG_APC_index = -1;

    for (;;) {
        if (!GetALineAndDontArgue(pF, s)) {
            return;
        }
        if (strcmp(s, "START OF OPPONENT PATHS") == 0) {
            break;
        }
    }

    /* Number of path nodes */
    count_nodes = GetAnInt(pF);
    ReallocExtraPathNodes(count_nodes);

    for (i = 0; i < gProgram_state.AI_vehicles.number_of_path_nodes; i++) {
        br_vector3 pos;

        /* Node #i */
        GetThreeFloats(pF, &pos.v[0], &pos.v[1], &pos.v[2]);
        BrVector3Copy(&gProgram_state.AI_vehicles.path_nodes[i].pos, &pos);
        gProgram_state.AI_vehicles.path_nodes[i].number_of_sections = 0;
    }

    /* Number of path sections */
    count_sections = GetAnInt(pF);
    ReallocExtraPathSections(count_sections);

    for (i = 0; i < gProgram_state.AI_vehicles.number_of_path_sections; i++) {
        br_scalar scalars[8];
        br_vector3 dp;
        int j;

        C2_HOOK_BUG_ON(BR_ASIZE(scalars) != 8);

        PossibleService();

        /* Section #i: node[0], node[1], min_speed[0], max_speed[0], min_speed[1], max_speed[1], width, type*/
        GetNScalars(pF, BR_ASIZE(scalars), scalars);
        gProgram_state.AI_vehicles.path_sections[i].node_indices[0] = (tU16)scalars[0];
        gProgram_state.AI_vehicles.path_sections[i].node_indices[1] = (tU16)scalars[1];
        gProgram_state.AI_vehicles.path_sections[i].min_speed[0] = (tU8)scalars[2];
        gProgram_state.AI_vehicles.path_sections[i].max_speed[0] = (tU8)scalars[3];
        gProgram_state.AI_vehicles.path_sections[i].min_speed[1] = (tU8)scalars[4];
        gProgram_state.AI_vehicles.path_sections[i].max_speed[1] = (tU8)scalars[5];
        gProgram_state.AI_vehicles.path_sections[i].width = scalars[6];

        BrVector3Sub(&dp,
            &gProgram_state.AI_vehicles.path_nodes[gProgram_state.AI_vehicles.path_sections[i].node_indices[0]].pos,
            &gProgram_state.AI_vehicles.path_nodes[gProgram_state.AI_vehicles.path_sections[i].node_indices[1]].pos);
        gProgram_state.AI_vehicles.path_sections[i].length = BrVector3Length(&dp);

        if (scalars[7] < 1000.0f) {
            gProgram_state.AI_vehicles.path_sections[i].type = (tU8)scalars[7];
            gProgram_state.AI_vehicles.path_sections[i].one_way = 0;
        } else {
            gProgram_state.AI_vehicles.path_sections[i].type = (tU8)(scalars[7] - 1000.0f);
            gProgram_state.AI_vehicles.path_sections[i].one_way = 1;
        }
        for (j = 0; j < 2; j++) {
            tPath_node* node_ptr;

            node_ptr = &gProgram_state.AI_vehicles.path_nodes[gProgram_state.AI_vehicles.path_sections[i].node_indices[j]];
            if (node_ptr->number_of_sections >= 8) {
                DoNotDprintf_opponent("ERROR: Too many sections (including section #%d) attached to node #%d",
                    i, gProgram_state.AI_vehicles.path_sections[i].node_indices[j]);
            } else {
                node_ptr->sections[node_ptr->number_of_sections] = (tS16)i;
                node_ptr->number_of_sections += 1;
            }
        }
    }

    if (gAusterity_mode || gNet_mode != eNet_mode_none) {

        gProgram_state.AI_vehicles.number_of_cops = GetAnInt(pF);
        for (i = 0; i < gProgram_state.AI_vehicles.number_of_cops; i++) {
            GetALineAndDontArgue(pF, s);
        }
        gProgram_state.AI_vehicles.number_of_cops = 0;
    } else {

        gProgram_state.AI_vehicles.number_of_cops = GetAnInt(pF);
        for (i = 0; i < gProgram_state.AI_vehicles.number_of_cops; i++) {
            br_scalar scalars[6];
            br_vector3 cop_to_section;
            br_vector3 intersect;
            br_vector3 section_v;
            br_scalar distance;

            C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(scalars) != 6);

            PossibleService();

            /* x, y, z, type[0], type[1], type[2] */
            GetNScalars(pF, CARPOCALYPSE2_ASIZE(scalars), scalars);
            BrVector3Set(&gProgram_state.AI_vehicles.cop_start_points[i],
                scalars[0], scalars[1], scalars[2]);

            if (scalars[3] == 9.0f && scalars[4] == 9.0f && scalars[5] == 9.0f) {
                gBIG_APC_index = i;
            }

            FindNearestPathSection(&gProgram_state.AI_vehicles.cop_start_points[i], &cop_to_section, &intersect, &distance);
            BrVector3Cross(&gProgram_state.AI_vehicles.cop_start_vectors[i],
                &y_unit_vector, &cop_to_section);
            BrVector3Sub(&section_v, &intersect, &gProgram_state.AI_vehicles.cop_start_points[i]);
            if (BrVector3Dot(&gProgram_state.AI_vehicles.cop_start_vectors[i], &section_v) < 0.0f) {
                BrVector3Negate(&gProgram_state.AI_vehicles.cop_start_vectors[i], &gProgram_state.AI_vehicles.cop_start_vectors[i]);
            }
        }
    }

    for (;;) {
        GetALineAndDontArgue(pF, s);
        if (strcmp(s, "END OF OPPONENT PATHS") == 0) {
            break;
        }
    }

    if (gProgram_state.AI_vehicles.number_of_path_sections != 0) {
        gBit_per_node = BrMemAllocate((gProgram_state.AI_vehicles.number_of_path_nodes + 7) / 8, kMem_oppo_bit_per_node);
    } else {
        gBit_per_node = NULL;
    }
    DoNotDprintf_opponent("End of LoadInOppoPaths(), totals:");
    DoNotDprintf_opponent("Nodes: %d", gProgram_state.AI_vehicles.number_of_path_nodes);
    DoNotDprintf_opponent("Sections: %d", gProgram_state.AI_vehicles.number_of_path_sections);
#else
    __asm {
            sub esp, 0x14c
            push ebx
            push ebp
            push esi
            push edi
            mov ebx, ecx
            push offset gOpponent_nastyness_frigger+0x364
            mov dword ptr [esp + 0x14], ebx
            call DoNotDprintf_opponent
            add esp, 4
            xor eax, eax
            mov dword ptr [gProgram_state+0x1c28], eax
            mov dword ptr [gProgram_state+0x1c2c], eax
            mov dword ptr [gProgram_state+0x5ec0], eax
            mov dword ptr [gProgram_state+0x5ec4], eax
            mov dword ptr [gBit_per_node], eax
            mov dword ptr [gBIG_APC_index], 0xffffffff
    L004a9652:
            lea edx, [esp + 0x5c]
            mov ecx, ebx
            call GetALineAndDontArgue
            mov edi, eax
            test edi, edi
            je L004a9c43
            lea esi, [esp + 0x5c]
            mov ecx, offset gOpponent_nastyness_frigger+0x34c
    L004a9670:
            mov al, byte ptr [ecx]
            mov dl, al
            cmp al, byte ptr [esi]
            jne L004a9694
            test dl, dl
            je L004a9690
            mov al, byte ptr [ecx + 1]
            mov dl, al
            cmp al, byte ptr [esi + 1]
            jne L004a9694
            add ecx, 2
            add esi, 2
            test dl, dl
            jne L004a9670
    L004a9690:
            xor ecx, ecx
            jmp L004a9699
    L004a9694:
            sbb ecx, ecx
            sbb ecx, -1
    L004a9699:
            test ecx, ecx
            jne L004a9652
            test edi, edi
            je L004a9c43
            mov ecx, ebx
            call GetAnInt
            mov ebp, eax
            test ebp, ebp
            je L004a9716
            mov ecx, dword ptr [gProgram_state+0x1c28]
            push 0xba
            lea edx, [ebp + ecx]
            shl edx, 5
            push edx
            call BrMemAllocate
            mov ecx, dword ptr [gProgram_state+0x1c28]
            mov esi, dword ptr [gProgram_state+0x5ec0]
            shl ecx, 5
            mov ebx, eax
            mov eax, ecx
            mov edi, ebx
            add esp, 8
            shr ecx, 2
            rep movsd
            mov ecx, eax
            and ecx, 3
            rep movsb
            mov eax, dword ptr [gProgram_state+0x5ec0]
            test eax, eax
            je L004a9700
            push eax
            call BrMemFree
            add esp, 4
    L004a9700:
            mov eax, dword ptr [gProgram_state+0x1c28]
            mov dword ptr [gProgram_state+0x5ec0], ebx
            mov ebx, dword ptr [esp + 0x10]
            add eax, ebp
            mov dword ptr [gProgram_state+0x1c28], eax
    L004a9716:
            mov ecx, dword ptr [gProgram_state+0x1c28]
            push ebp
            add ebp, ecx
            shl ebp, 5
            push ebp
            push offset gOpponent_nastyness_frigger+0x30c
            call DoNotDprintf_opponent
            mov eax, dword ptr [gProgram_state+0x1c28]
            add esp, 0xc
            test eax, eax
            je L004a9796
            xor edi, edi
            test eax, eax
            jle L004a9796
            xor esi, esi
    L004a9741:
            lea edx, [esp + 0x40]
            lea eax, [esp + 0x48]
            push edx
            push eax
            lea edx, [esp + 0x4c]
            mov ecx, ebx
            call GetThreeFloats
            fld dword ptr [esp + 0x44]
            mov ecx, dword ptr [gProgram_state+0x5ec0]
            inc edi
            fstp dword ptr [ecx + esi]
            fld dword ptr [esp + 0x48]
            mov edx, dword ptr [gProgram_state+0x5ec0]
            add esi, 0x20
            fstp dword ptr [edx + esi - 0x1c]
            fld dword ptr [esp + 0x40]
            mov eax, dword ptr [gProgram_state+0x5ec0]
            fstp dword ptr [eax + esi - 0x18]
            mov ecx, dword ptr [gProgram_state+0x5ec0]
            mov byte ptr [ecx + esi - 4], 0
            mov eax, dword ptr [gProgram_state+0x1c28]
            cmp edi, eax
            jl L004a9741
    L004a9796:
            mov ecx, ebx
            call GetAnInt
            mov ebp, eax
            test ebp, ebp
            je L004a980c
            mov edx, dword ptr [gProgram_state+0x1c2c]
            push 0xbb
            lea eax, [ebp + edx]
            lea eax, [eax + eax*4]
            shl eax, 2
            push eax
            call BrMemAllocate
            mov ebx, eax
            mov eax, dword ptr [gProgram_state+0x1c2c]
            mov esi, dword ptr [gProgram_state+0x5ec4]
            mov edi, ebx
            lea ecx, [eax + eax*4]
            add esp, 8
            shl ecx, 2
            mov edx, ecx
            shr ecx, 2
            rep movsd
            mov ecx, edx
            and ecx, 3
            rep movsb
            mov eax, dword ptr [gProgram_state+0x5ec4]
            test eax, eax
            je L004a97f6
            push eax
            call BrMemFree
            add esp, 4
    L004a97f6:
            mov eax, dword ptr [gProgram_state+0x1c2c]
            mov dword ptr [gProgram_state+0x5ec4], ebx
            mov ebx, dword ptr [esp + 0x10]
            add eax, ebp
            mov dword ptr [gProgram_state+0x1c2c], eax
    L004a980c:
            mov eax, dword ptr [gProgram_state+0x1c2c]
            push ebp
            add ebp, eax
            lea ecx, [ebp + ebp*4]
            shl ecx, 2
            push ecx
            push offset gOpponent_nastyness_frigger+0x2c8
            call DoNotDprintf_opponent
            mov eax, dword ptr [gProgram_state+0x1c2c]
            add esp, 0xc
            test eax, eax
            je L004a9a09
            xor ebp, ebp
            test eax, eax
            jle L004a9a09
            xor esi, esi
    L004a9842:
            call PossibleService
            lea edx, [esp + 0x20]
            mov ecx, ebx
            push edx
            mov edx, 8
            call GetNScalars
            fld dword ptr [esp + 0x20]
            call _ftol
            mov ecx, dword ptr [gProgram_state+0x5ec4]
            mov word ptr [ecx + esi], ax
            fld dword ptr [esp + 0x24]
            call _ftol
            mov edx, dword ptr [gProgram_state+0x5ec4]
            mov word ptr [edx + esi + 2], ax
            fld dword ptr [esp + 0x28]
            call _ftol
            mov ecx, dword ptr [gProgram_state+0x5ec4]
            mov byte ptr [ecx + esi + 4], al
            fld dword ptr [esp + 0x2c]
            call _ftol
            mov edx, dword ptr [gProgram_state+0x5ec4]
            mov byte ptr [edx + esi + 6], al
            fld dword ptr [esp + 0x30]
            call _ftol
            mov ecx, dword ptr [gProgram_state+0x5ec4]
            mov byte ptr [ecx + esi + 5], al
            fld dword ptr [esp + 0x34]
            call _ftol
            mov edx, dword ptr [gProgram_state+0x5ec4]
            mov byte ptr [edx + esi + 7], al
            mov eax, dword ptr [gProgram_state+0x5ec4]
            fld dword ptr [esp + 0x38]
            fstp dword ptr [eax + esi + 8]
            mov ecx, dword ptr [gProgram_state+0x5ec4]
            mov eax, dword ptr [gProgram_state+0x5ec0]
            movsx edx, word ptr [ecx + esi + 2]
            movsx edi, word ptr [ecx + esi]
            shl edx, 5
            shl edi, 5
            fld dword ptr [edx + eax]
            fsub dword ptr [edi + eax]
            fstp dword ptr [esp + 0x14]
            movsx edx, word ptr [ecx + esi + 2]
            movsx edi, word ptr [ecx + esi]
            shl edx, 5
            shl edi, 5
            fld dword ptr [edx + eax + 4]
            fsub dword ptr [edi + eax + 4]
            fstp dword ptr [esp + 0x18]
            movsx edx, word ptr [ecx + esi + 2]
            movsx edi, word ptr [ecx + esi]
            shl edx, 5
            shl edi, 5
            fld dword ptr [edx + eax + 8]
            fsub dword ptr [edi + eax + 8]
            fstp dword ptr [esp + 0x1c]
            fld dword ptr [esp + 0x14]
            fmul dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x18]
            fmul dword ptr [esp + 0x18]
            faddp st(1), st(0)
            fld dword ptr [esp + 0x1c]
            fmul dword ptr [esp + 0x1c]
            faddp st(1), st(0)
            fsqrt
            fstp dword ptr [ecx + esi + 0xc]
            fld dword ptr [esp + 0x3c]
            fcomp dword ptr [DAT_0058a4f0+0x4]
            fld dword ptr [esp + 0x3c]
            fnstsw ax
            test ah, 1
            jne L004a998b
            fsub dword ptr [DAT_0058a4f0+0x4]
            call _ftol
            mov ecx, dword ptr [gProgram_state+0x5ec4]
            mov byte ptr [ecx + esi + 0x10], al
            mov edx, dword ptr [gProgram_state+0x5ec4]
            mov byte ptr [edx + esi + 0x11], 1
            jmp L004a99a5
    L004a998b:
            call _ftol
            mov ecx, dword ptr [gProgram_state+0x5ec4]
            mov byte ptr [ecx + esi + 0x10], al
            mov edx, dword ptr [gProgram_state+0x5ec4]
            mov byte ptr [edx + esi + 0x11], 0
    L004a99a5:
            mov edi, esi
            mov ebx, 2
    L004a99ac:
            mov eax, dword ptr [gProgram_state+0x5ec4]
            mov ecx, dword ptr [gProgram_state+0x5ec0]
            movsx edx, word ptr [eax + edi]
            mov eax, edx
            shl eax, 5
            add eax, ecx
            mov cl, byte ptr [eax + 0x1c]
            cmp cl, 8
            jae L004a99df
            and ecx, 0xff
            mov word ptr [eax + ecx*2 + 0xc], bp
            mov cl, byte ptr [eax + 0x1c]
            inc cl
            mov byte ptr [eax + 0x1c], cl
            jmp L004a99ee
    L004a99df:
            push edx
            push ebp
            push offset gOpponent_nastyness_frigger+0x280
            call DoNotDprintf_opponent
            add esp, 0xc
    L004a99ee:
            add edi, 2
            dec ebx
            jne L004a99ac
            mov eax, dword ptr [gProgram_state+0x1c2c]
            mov ebx, dword ptr [esp + 0x10]
            inc ebp
            add esi, 0x14
            cmp ebp, eax
            jl L004a9842
    L004a9a09:
            mov eax, dword ptr [gAusterity_mode]
            test eax, eax
            jne L004a9b63
            mov eax, dword ptr [gNet_mode]
            test eax, eax
            jne L004a9b63
            mov ecx, ebx
            call GetAnInt
            test eax, eax
            mov dword ptr [gProgram_state+0x1c24], eax
            je L004a9b94
            xor edi, edi
            test eax, eax
            jle L004a9b94
            mov esi, offset gProgram_state+0x1c30
    L004a9a46:
            call PossibleService
            lea ecx, [esp + 0x20]
            mov edx, 6
            push ecx
            mov ecx, ebx
            call GetNScalars
            fld dword ptr [esp + 0x2c]
            fcomp dword ptr [DAT_0058a4f0+0x8]
            mov edx, dword ptr [esp + 0x20]
            mov eax, dword ptr [esp + 0x24]
            mov ecx, dword ptr [esp + 0x28]
            mov dword ptr [esi], edx
            mov dword ptr [esi + 4], eax
            mov dword ptr [esi + 8], ecx
            fnstsw ax
            test ah, 0x40
            je L004a9aa9
            fld dword ptr [esp + 0x30]
            fcomp dword ptr [DAT_0058a4f0+0x8]
            fnstsw ax
            test ah, 0x40
            je L004a9aa9
            fld dword ptr [esp + 0x34]
            fcomp dword ptr [DAT_0058a4f0+0x8]
            fnstsw ax
            test ah, 0x40
            je L004a9aa9
            mov dword ptr [gBIG_APC_index], edi
    L004a9aa9:
            lea edx, [esp + 0x58]
            lea eax, [esp + 0x4c]
            push edx
            lea ecx, [esp + 0x18]
            push eax
            push ecx
            mov edx, esi
            xor ecx, ecx
            call FindNearestGeneralSection
            fld dword ptr [gOppo_render_zero]
            fld dword ptr [esp + 0x1c]
            fld dword ptr [esp + 0x14]
            fsubr st(0), st(2)
            fxch st(1)
            fsub st(0), st(2)
            fxch st(1)
            fst dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x54]
            fld dword ptr [esp + 0x4c]
            fxch st(3)
            fstp dword ptr [esi + 0x78]
            fld dword ptr [esp + 0x50]
            fxch st(2)
            mov dword ptr [esi + 0x7c], 0
            fstp dword ptr [esi + 0x80]
            fsub dword ptr [esi + 8]
            fxch st(2)
            _emit 0xd8
        _emit 0x26
            fld dword ptr [esi + 0x78]
            fxch st(2)
            fsub dword ptr [esi + 4]
            fld dword ptr [esi + 0x7c]
            fxch st(4)
            fmul dword ptr [esp + 0x10]
            fxch st(3)
            fmulp st(2), st(0)
            fxch st(1)
            faddp st(2), st(0)
            fxch st(2)
            fmulp st(2), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fcomp dword ptr [gOppo_render_zero]
            fnstsw ax
            test ah, 1
            fstp st(0)
            je L004a9b50
            fld dword ptr [esi + 0x78]
            fchs
            fstp dword ptr [esi + 0x78]
            fld dword ptr [esi + 0x7c]
            fchs
            fstp dword ptr [esi + 0x7c]
            fld dword ptr [esi + 0x80]
            fchs
            fstp dword ptr [esi + 0x80]
    L004a9b50:
            mov eax, dword ptr [gProgram_state+0x1c24]
            inc edi
            add esi, 0xc
            cmp edi, eax
            jl L004a9a46
            jmp L004a9b94
    L004a9b63:
            mov ecx, ebx
            call GetAnInt
            xor esi, esi
            mov dword ptr [gProgram_state+0x1c24], eax
            test eax, eax
            jle L004a9b8a
    L004a9b75:
            lea edx, [esp + 0x5c]
            mov ecx, ebx
            call GetALineAndDontArgue
            mov eax, dword ptr [gProgram_state+0x1c24]
            inc esi
            cmp esi, eax
            jl L004a9b75
    L004a9b8a:
            mov dword ptr [gProgram_state+0x1c24], 0
    L004a9b94:
            lea edx, [esp + 0x5c]
            mov ecx, ebx
            call GetALineAndDontArgue
            lea esi, [esp + 0x5c]
            mov eax, offset gOpponent_nastyness_frigger+0x268
    L004a9ba8:
            mov dl, byte ptr [eax]
            mov cl, dl
            cmp dl, byte ptr [esi]
            jne L004a9bcc
            test cl, cl
            je L004a9bc8
            mov dl, byte ptr [eax + 1]
            mov cl, dl
            cmp dl, byte ptr [esi + 1]
            jne L004a9bcc
            add eax, 2
            add esi, 2
            test cl, cl
            jne L004a9ba8
    L004a9bc8:
            xor eax, eax
            jmp L004a9bd1
    L004a9bcc:
            sbb eax, eax
            sbb eax, -1
    L004a9bd1:
            test eax, eax
            jne L004a9b94
            mov eax, dword ptr [gProgram_state+0x1c2c]
            test eax, eax
            je L004a9c04
            mov eax, dword ptr [gProgram_state+0x1c28]
            push 0xbd
            add eax, 7
            cdq
            and edx, 7
            add eax, edx
            sar eax, 3
            push eax
            call BrMemAllocate
            add esp, 8
            mov dword ptr [gBit_per_node], eax
            jmp L004a9c0e
    L004a9c04:
            mov dword ptr [gBit_per_node], 0
    L004a9c0e:
            push offset gOpponent_nastyness_frigger+0x244
            call DoNotDprintf_opponent
            mov ecx, dword ptr [gProgram_state+0x1c28]
            add esp, 4
            push ecx
            push offset gDefault_drone_direction+0x7e8
            call DoNotDprintf_opponent
            mov edx, dword ptr [gProgram_state+0x1c2c]
            add esp, 8
            push edx
            push offset gOpponent_nastyness_frigger+0x234
            call DoNotDprintf_opponent
            add esp, 8
    L004a9c43:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x14c
            ret
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004aa400
int C2_HOOK_FASTCALL PointVisibleFromHere(br_vector3* pFrom, br_vector3* pTo) {
    br_vector3 from;
    br_vector3 dir;
    br_vector3 norm;
    br_scalar t;
    br_material* material;

    BrVector3Sub(&dir, pTo, pFrom);
    BrVector3Copy(&from, pFrom);
    from.v[1] -= gOppo_pursuit_height_offset;
    dir.v[1] -= gOppo_pursuit_height_offset;
    FindFace(&from, &dir, &norm, &t, &material);
    if (t > gOppo_pursuit_one) {
        return 1;
    } else {
        return 0;
    }
}

int C2_HOOK_FASTCALL OpponentRepairNecessary(tOpponent_spec* pOpponent_spec) {
    int i;
    tDamage_unit* damage;

    for (i = 0, damage = pOpponent_spec->car_spec->damage_units;
            i < (int)CARPOCALYPSE2_ASIZE(pOpponent_spec->car_spec->damage_units);
            i++, damage++) {
        if (i != eDamage_driver && damage != NULL) {
            return 1;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004ad910
void C2_HOOK_FASTCALL PossiblyRepairOpponent(tOpponent_spec* pOpponent_spec) {
    char message[256];
    char headup[256];

    if (((tU16)pOpponent_spec->car_spec->car_ID >> 8) != 3
            && gTime_stamp_for_this_munging > pOpponent_spec->next_repair_check
            && pOpponent_spec->cheating
            && pOpponent_spec->current_objective != eOOT_levitate
            && pOpponent_spec->current_objective != eOOT_knackered_and_freewheeling
            && !pOpponent_spec->car_spec->knackered) {
        pOpponent_spec->next_repair_check = IRandomBetween(0, 2000) + 3000 + gTime_stamp_for_this_munging;
        if (gTime_stamp_for_this_munging > pOpponent_spec->car_spec->field_0x1548 + pOpponent_spec->repair_interval) {
            if (OpponentRepairNecessary(pOpponent_spec)) {
                DoNotDprintf_opponent("*** Opponent %d, REPAIRING, timestamp %d", pOpponent_spec->car_spec->driver_name, gTime_stamp_for_this_munging);
                TotallyRepairACar(pOpponent_spec->car_spec);
                sprintf(message, "%s %s", pOpponent_spec->car_spec->driver_name, GetMiscString(0x111));
                strcpy(headup, message);
                pOpponent_spec->repair_interval = IRandomBetween(gMinTimeOpponentRepair, gMaxTimeOpponentRepair) * 60000;
                pOpponent_spec->last_repair_time = gTime_stamp_for_this_munging;
                DoNotDprintf_opponent("*** Opponent %s, repair_interval is %d", pOpponent_spec->car_spec->driver_name, pOpponent_spec->repair_interval);
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x004adae0
void C2_NAKED C2_HOOK_FASTCALL CalcPlayerConspicuousness(tOpponent_spec* pOpponent_spec)  {
#ifndef CARPOCALYPSE2_MATCHING
    br_vector3 pos_in_cop_space;
    br_matrix34 inverse_transform;

    if (pOpponent_spec->next_player_visibility_check >= gTime_stamp_for_this_munging) {
        return;
    }
    pOpponent_spec->player_in_view_now = 0;
    if (CAR_SPEC_IS_ROZZER(pOpponent_spec->car_spec)) {
        pOpponent_spec->next_player_visibility_check = gTime_stamp_for_this_munging + IRandomBetween(0, 900) + 100;
        if (pOpponent_spec->player_to_oppo_d < 20.f) {
            BrMatrix34LPInverse(&inverse_transform, &pOpponent_spec->car_spec->car_master_actor->t.t.mat);
            BrMatrix34ApplyP(&pos_in_cop_space, &gProgram_state.current_car.car_master_actor->t.t.translate.t, &inverse_transform);
            if (pos_in_cop_space.v[2] < 0.f && PointVisibleFromHere(&gProgram_state.current_car.car_master_actor->t.t.translate.t, &pOpponent_spec->car_spec->car_master_actor->t.t.translate.t)) {
                pOpponent_spec->player_in_view_now = 1;
                pOpponent_spec->acknowledged_piv = 0;
            }
        }
    } else {
        pOpponent_spec->next_player_visibility_check = gTime_stamp_for_this_munging + IRandomBetween(0, 900) + 4000;
        DoNotDprintf_opponent("%s: Time now: %9.2f; next vis check at %9.2f", pOpponent_spec->car_spec->driver_name, gTime_stamp_for_this_munging / 1000.f, pOpponent_spec->next_player_visibility_check / 1000.0f);
        if (pOpponent_spec->player_to_oppo_d < 50.f && PointVisibleFromHere(&gProgram_state.current_car.car_master_actor->t.t.translate.t, &pOpponent_spec->car_spec->car_master_actor->t.t.translate.t)) {
            pOpponent_spec->player_in_view_now = 1;
            pOpponent_spec->acknowledged_piv = 0;
        }
    }
#else
    __asm {
            mov eax, dword ptr [gTime_stamp_for_this_munging]
            sub esp, 0x6c
            push esi
            mov esi, ecx
            push edi
            cmp eax, dword ptr [esi + 0x60]
            jbe L004add40
            mov edx, dword ptr [esi + 0xb0]
            mov ecx, dword ptr [esi + 8]
            and dh, 0xfd
            mov dword ptr [esi + 0xb0], edx
            xor edx, edx
            mov dx, word ptr [ecx + 0x80]
            and dl, 0
            cmp edx, 0x300
            mov edx, 0x384
            jne L004adc28
            xor ecx, ecx
            call IRandomBetween
            fld dword ptr [esi + 0x18]
            fcomp dword ptr [gOppo_pursuit_trail_distance]
            mov ecx, dword ptr [gTime_stamp_for_this_munging]
            lea edx, [eax + ecx + 0x64]
            fnstsw ax
            mov dword ptr [esi + 0x60], edx
            test ah, 1
            je L004add40
            mov eax, dword ptr [esi + 8]
            lea edx, [esp + 0x44]
            mov ecx, dword ptr [eax + 0x10]
            add ecx, 0x2c
            push ecx
            push edx
            call BrMatrix34LPInverse
            mov ecx, dword ptr [gProgram_state+0xbc]
            add esp, 8
            lea eax, [esp + 0x44]
            add ecx, 0x50
            push eax
            lea edx, [esp + 0x30]
            push ecx
            push edx
            call BrMatrix34ApplyP
            fld dword ptr [esp + 0x40]
            fcomp dword ptr [gOppo_render_zero]
            add esp, 0xc
            fnstsw ax
            test ah, 1
            je L004add40
            mov ecx, dword ptr [esi + 8]
            mov eax, dword ptr [gProgram_state+0xbc]
            lea edx, [esp + 0xc]
            lea edi, [esp + 8]
            mov ecx, dword ptr [ecx + 0x10]
            push edx
            add ecx, 0x50
            push edi
            lea edx, [esp + 0x28]
            fld dword ptr [ecx + 4]
            fsub dword ptr [eax + 0x54]
            fld dword ptr [eax + 0x54]
            fld dword ptr [ecx + 8]
            _emit 0xd9
        _emit 0x01
            fsub dword ptr [eax + 0x50]
            fxch st(1)
            fsub dword ptr [eax + 0x58]
            fxch st(3)
            fsub dword ptr [gOppo_pursuit_height_offset]
            fxch st(2)
            fst dword ptr [esp + 0x20]
            fsub dword ptr [gOppo_pursuit_height_offset]
            fld dword ptr [eax + 0x50]
            fld dword ptr [eax + 0x58]
            fxch st(3)
            fstp dword ptr [esp + 0x28]
            fxch st(4)
            fstp dword ptr [esp + 0x30]
            fxch st(3)
            fstp dword ptr [esp + 0x1c]
            lea eax, [esp + 0x40]
            lea ecx, [esp + 0x1c]
            fstp dword ptr [esp + 0x24]
            fxch st(1)
            fstp dword ptr [esp + 0x20]
            push eax
            fstp dword ptr [esp + 0x30]
            call FindFace
            fld dword ptr [esp + 8]
            fcomp dword ptr [gOppo_pursuit_one]
            fnstsw ax
            test ah, 0x41
            jne L004add40
            jmp L004add2e
    L004adc28:
            xor ecx, ecx
            call IRandomBetween
            mov edx, dword ptr [gTime_stamp_for_this_munging]
            sub esp, 8
            sub esp, 8
            lea eax, [eax + edx + 0xfa0]
            mov edx, dword ptr [esi + 8]
            mov dword ptr [esi + 0x60], eax
            mov dword ptr [esp + 0x1c], eax
            xor eax, eax
            mov ecx, dword ptr [gTime_stamp_for_this_munging]
            mov dword ptr [esp + 0x20], eax
            add edx, 0xec
            fild qword ptr [esp + 0x1c]
            mov dword ptr [esp + 0x1c], ecx
            mov dword ptr [esp + 0x20], eax
            fmul qword ptr [gOppo_pursuit_reverse_acceleration+0x14]
            fstp qword ptr [esp + 8]
            fild qword ptr [esp + 0x1c]
            fmul qword ptr [gOppo_pursuit_reverse_acceleration+0x14]
            _emit 0xdd
        _emit 0x1c
        _emit 0x24
            push edx
            push offset gOpponent_nastyness_frigger+0x1218
            call DoNotDprintf_opponent
            fld dword ptr [esi + 0x18]
            fcomp dword ptr [gOppo_pursuit_cheat_distance]
            add esp, 0x18
            fnstsw ax
            test ah, 1
            je L004add40
            mov ecx, dword ptr [esi + 8]
            mov eax, dword ptr [gProgram_state+0xbc]
            lea edx, [esp + 0xc]
            lea edi, [esp + 8]
            mov ecx, dword ptr [ecx + 0x10]
            push edx
            add ecx, 0x50
            push edi
            lea edx, [esp + 0x28]
            fld dword ptr [ecx + 4]
            fsub dword ptr [eax + 0x54]
            fld dword ptr [eax + 0x54]
            fld dword ptr [ecx + 8]
            _emit 0xd9
        _emit 0x01
            fsub dword ptr [eax + 0x50]
            fxch st(1)
            fsub dword ptr [eax + 0x58]
            fxch st(3)
            fsub dword ptr [gOppo_pursuit_height_offset]
            fxch st(2)
            fst dword ptr [esp + 0x20]
            fsub dword ptr [gOppo_pursuit_height_offset]
            fld dword ptr [eax + 0x50]
            fld dword ptr [eax + 0x58]
            fxch st(3)
            fstp dword ptr [esp + 0x28]
            fxch st(4)
            fstp dword ptr [esp + 0x30]
            fxch st(3)
            fstp dword ptr [esp + 0x1c]
            lea eax, [esp + 0x40]
            lea ecx, [esp + 0x1c]
            fstp dword ptr [esp + 0x24]
            fxch st(1)
            fstp dword ptr [esp + 0x20]
            push eax
            fstp dword ptr [esp + 0x30]
            call FindFace
            fld dword ptr [esp + 8]
            fcomp dword ptr [gOppo_pursuit_one]
            fnstsw ax
            test ah, 0x41
            jne L004add40
    L004add2e:
            mov ecx, dword ptr [esi + 0xb0]
            and ch, 0xfb
            or ch, 2
            mov dword ptr [esi + 0xb0], ecx
    L004add40:
            pop edi
            pop esi
            add esp, 0x6c
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004ab190
void C2_HOOK_FASTCALL ProcessPursueAndTwat(tOpponent_spec* pOpponent_spec, tProcess_objective_command pCommand) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004a9c50
void C2_HOOK_FASTCALL DisposeOpponentPaths(void) {
    if (gProgram_state.AI_vehicles.path_nodes != NULL) {
        BrMemFree(gProgram_state.AI_vehicles.path_nodes);
    }
    if (gProgram_state.AI_vehicles.path_sections != NULL) {
        BrMemFree(gProgram_state.AI_vehicles.path_sections);
    }
    if (gBit_per_node != NULL) {
        BrMemFree(gBit_per_node);
    }

    gProgram_state.AI_vehicles.number_of_path_nodes = 0;
    gProgram_state.AI_vehicles.number_of_path_sections = 0;
    gProgram_state.AI_vehicles.path_nodes = NULL;
    gProgram_state.AI_vehicles.path_sections = NULL;
    gBit_per_node = NULL;
}

// FUNCTION: CARMA2_HW 0x004a9cb0
void C2_NAKED C2_HOOK_FASTCALL UpdateViewedOpponentHeadup(void) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;
    tOpponent_spec* opponent;
    tCar_spec** car_specs;
    int* opponent_data;
    int displayed_section_no;
    const char* cheating_marker;
    int section_no;
    const char* direction;
    const char* extra_section;
    char message[256];
    tCar_spec* viewed_car;

    viewed_car = gCar_to_view;
    if (viewed_car == NULL || viewed_car->driver != eDriver_oppo) {
        message[0] = '\0';
    } else {
        if ((viewed_car->car_ID & 0xff00) == 0x0200) {
            for (i = 0, car_specs = &gProgram_state.AI_vehicles.opponents[0].car_spec;
                i < gProgram_state.AI_vehicles.number_of_opponents;
                i++, car_specs += sizeof(tOpponent_spec) / sizeof(*car_specs)) {
                if (*car_specs == viewed_car) {
                    opponent_data = (int*)&gProgram_state.AI_vehicles.opponents[0]
                        + i * sizeof(tOpponent_spec) / sizeof(int);
                    goto opponent_found;
                }
            }
        } else if ((viewed_car->car_ID & 0xff00) == 0x0300) {
            for (i = 0, car_specs = &gProgram_state.AI_vehicles.cops[0].car_spec;
                i < gNumber_of_cops_before_faffage;
                i++, car_specs += sizeof(tOpponent_spec) / sizeof(*car_specs)) {
                if (*car_specs == viewed_car) {
                    opponent_data = (int*)&gProgram_state.AI_vehicles.cops[0]
                        + i * sizeof(tOpponent_spec) / sizeof(int);
                    goto opponent_found;
                }
            }
        }
        opponent_data = NULL;

opponent_found:
        opponent = (tOpponent_spec*)opponent_data;
        section_no = opponent->follow_path_data.section_no;
        cheating_marker = "*";
        if (!opponent->cheating) {
            cheating_marker = "";
        }

        if (section_no < 20000 || opponent->next_sections[section_no - 20000].direction != 0) {
            direction = "1";
        } else {
            direction = "0";
        }
        if (section_no < 15000 || section_no >= 20000) {
            extra_section = "";
        } else {
            extra_section = "T";
        }
        displayed_section_no = section_no;
        if (section_no >= 20000) {
            displayed_section_no = opponent->next_sections[section_no - 20000].section_no;
        }
        sprintf(message, "%s: obj: %d nsect: %d sect: %d(%d%s)%s%s", viewed_car->name,
            opponent->current_objective, opponent->nnext_sections, section_no, displayed_section_no,
            extra_section, direction, cheating_marker);
    }
    ChangeHeadupText(gProgram_state.frame_rate_headup, message);
#else
    __asm {
            sub esp, 0x100
            push ebx
            push ebp
            mov ebp, dword ptr [gCar_to_view]
            push esi
            test ebp, ebp
            push edi
            je L004a9de4
            cmp dword ptr [ebp + 0xc], 6
            jne L004a9de4
            xor eax, eax
            mov ax, word ptr [ebp + 0x80]
            and al, 0
            cmp eax, 0x200
            jne L004a9d18
            mov edx, dword ptr [gProgram_state+0x1c20]
            xor eax, eax
            mov ecx, offset gProgram_state+0x1d28
    L004a9cf1:
            cmp eax, edx
            jge L004a9d53
            cmp dword ptr [ecx], ebp
            je L004a9d02
            inc eax
            add ecx, 0x1a4
            jmp L004a9cf1
    L004a9d02:
            mov ecx, eax
            shl ecx, 3
            sub ecx, eax
            lea eax, [ecx + ecx*2]
            lea eax, [eax + eax*4]
            lea eax, [eax*4 + gProgram_state+0x1d20]
            jmp L004a9d55
    L004a9d18:
            cmp eax, 0x300
            jne L004a9d53
            mov edx, dword ptr [gNumber_of_cops_before_faffage]
            xor eax, eax
            mov ecx, offset gProgram_state+0x4e60
    L004a9d2c:
            cmp eax, edx
            jge L004a9d53
            cmp dword ptr [ecx], ebp
            je L004a9d3d
            inc eax
            add ecx, 0x1a4
            jmp L004a9d2c
    L004a9d3d:
            mov ecx, eax
            shl ecx, 3
            sub ecx, eax
            lea eax, [ecx + ecx*2]
            lea ecx, [eax + eax*4]
            lea eax, [ecx*4 + gProgram_state+0x4e58]
            jmp L004a9d55
    L004a9d53:
            xor eax, eax
    L004a9d55:
            mov dl, byte ptr [eax + 0xb0]
            mov ebx, offset gOpponent_nastyness_frigger+0x3bc
            movsx ecx, word ptr [eax + 0xe8]
            test dl, 0x20
            jne L004a9d71
            mov ebx, offset gDisable_sound+0x8
    L004a9d71:
            cmp ecx, 0x4e20
            jl L004a9d89
            mov dl, byte ptr [eax + ecx*4 - 0x137fe]
            mov edi, offset gOpponent_nastyness_frigger+0x3b8
            test dl, dl
            je L004a9d8e
    L004a9d89:
            mov edi, offset gOpponent_nastyness_frigger+0x3b4
    L004a9d8e:
            cmp ecx, 0x3a98
            jl L004a9da3
            cmp ecx, 0x4e20
            mov esi, offset gOpponent_nastyness_frigger+0x3b0
            jl L004a9da8
    L004a9da3:
            mov esi, offset gDisable_sound+0x8
    L004a9da8:
            cmp ecx, 0x4e20
            jl L004a9dba
            movsx edx, word ptr [eax + ecx*4 - 0x13800]
            jmp L004a9dbc
    L004a9dba:
            mov edx, ecx
    L004a9dbc:
            push ebx
            push edi
            push esi
            push edx
            mov edx, dword ptr [eax + 0x7c]
            mov eax, dword ptr [eax + 4]
            push ecx
            push edx
            add ebp, 0xcc
            push eax
            push ebp
            lea ecx, [esp + 0x30]
            push offset gOpponent_nastyness_frigger+0x384
            push ecx
            call sprintf
            add esp, 0x28
            jmp L004a9de9
    L004a9de4:
            mov byte ptr [esp + 0x10], 0
    L004a9de9:
            mov ecx, dword ptr [gProgram_state+0x64]
            lea edx, [esp + 0x10]
            call ChangeHeadupText
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
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004a7ea0
void C2_HOOK_FASTCALL AddIfNotInList(tPhysics_object* pObject, tPhysics_object** pList, int pCount) {
    int i;

    for (i = 0; i < pCount; i++) {
        if (pList[i] == pObject) {
            pList[i] = NULL;
            return;
        }
    }
    PHILAddActiveObject(pObject, NULL, NULL, NULL);
}

// FUNCTION: CARMA2_HW 0x004a7ef0
void C2_HOOK_FASTCALL RemoveAnythingStillInList(tPhysics_object** pList, int pCount) {
    int i;

    for (i = 0; i < pCount; i++) {
        if (pList[i] != NULL) {
            PHILRemoveObject(pList[i]);
            pList[i]->field183_0x1d8 = 0;
        }
    }
}

void C2_HOOK_FASTCALL NoteCarsCurrentlyUsed(tPhysics_object** pCollision_infos, int* pCount) {
    tPhysics_object* info;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, flags_0x238, 0x238);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, owner, 0x23c);

    for (info = PHILGetFirstObject(); info != NULL; info = PHILGetNextObject(info)) {
        if (info != NULL && info->owner != NULL && info->flags_0x238 == 1 && ((tCar_spec*)info->owner)->driver > 5) {
            pCollision_infos[*pCount] = info;
            *pCount += 1;
        }
    }
}

int C2_HOOK_FASTCALL IsNetCarActive(const br_vector3* pPoint) {
    br_vector3 tv;

    BrVector3Sub(&tv, &gProgram_state.current_car.car_master_actor->t.t.translate.t, pPoint);
    if (BrVector3LengthSquared(&tv) < 100.f) {
        return 1;
    }
    if (gCar_to_view != &gProgram_state.current_car) {
        BrVector3Sub(&tv, &gCar_to_view->car_master_actor->t.t.translate.t, pPoint);
        return BrVector3LengthSquared(&tv) < 100.f;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004a7a80
void C2_NAKED C2_HOOK_FASTCALL RebuildActiveCarList(void) {
#ifndef CARPOCALYPSE2_MATCHING
    int new_count_active_cars = gNum_active_cars;

    if (gActive_car_list_rebuild_required) {
        int count_used_cars;
        tPhysics_object* used_cars[1000];
        int i;

        gActive_car_list_rebuild_required = 0;

        count_used_cars = 0;
        NoteCarsCurrentlyUsed(used_cars, &count_used_cars);

        new_count_active_cars = 0;
        for (i = 0; i < GetCarCount(eVehicle_self); i++) {
            tCar_spec* car;

            if (i > 0) {
                PDFatalError("mGet_car_count( eVehicle_self ) > 1 - I didn't know this could happen!");
            }
            car = GetCarSpec(eVehicle_self, i);
            if (!car->disabled || gAction_replay_mode) {
                car->active = 1;
                gActive_car_list[new_count_active_cars] = car;
                new_count_active_cars += 1;
                AddIfNotInList(car->collision_info, used_cars, count_used_cars);
            }
        }
        if (gNet_mode == eNet_mode_host) {
            for (i = 0; i < GetCarCount(eVehicle_net_player); i++) {
                tCar_spec* car;

                car = GetCarSpec(eVehicle_net_player, i);
                if (!car->disabled) {
                    car->active = 1;
                    gActive_car_list[new_count_active_cars] = car;
                    new_count_active_cars += 1;

                    AddIfNotInList(car->collision_info, used_cars, count_used_cars);
                } else {
                    car->active = 0;
                }
            }
        } else if (gNet_mode == eNet_mode_client) {
            for (i = 0; i < GetCarCount(eVehicle_net_player); i++) {
                tCar_spec* car;

                car = GetCarSpec(eVehicle_net_player, i);
                if (!car->disabled && IsNetCarActive(&car->car_master_actor->t.t.translate.t)) {
                    car->active = 1;
                    gActive_car_list[new_count_active_cars] = car;
                    new_count_active_cars += 1;
                    AddIfNotInList(car->collision_info, used_cars, count_used_cars);
                } else {
                    car->active = 0;
                }
            }
        }

        for (i = 0; i < gProgram_state.AI_vehicles.number_of_opponents; i++) {
            tCar_spec* car;

            car = GetCarSpec(eVehicle_opponent, i);
            if (gProgram_state.AI_vehicles.opponents[i].physics_me || gAction_replay_mode) {
                car->active = 1;
                gActive_car_list[new_count_active_cars] = car;
                new_count_active_cars += 1;
                AddIfNotInList(car->collision_info,used_cars, count_used_cars);
            } else {
                car->active = 0;
            }
        }

        for (i = 0; i < gNumber_of_cops_before_faffage; i++) {
            tCar_spec* car;

            car = GetCarSpec(eVehicle_rozzer, i);
            if (gProgram_state.AI_vehicles.cops[i].physics_me || gAction_replay_mode) {
                car->active = 1;
                gActive_car_list[new_count_active_cars] = car;
                new_count_active_cars += 1;
                AddIfNotInList(car->collision_info, used_cars, count_used_cars);
            } else {
                car->active = 0;
            }
        }
        RemoveAnythingStillInList(used_cars, count_used_cars);
    }
    if (gNum_active_cars != new_count_active_cars) {
        gNum_active_cars = new_count_active_cars;
    }
#else
    __asm {
            mov ecx, dword ptr [gActive_car_list_rebuild_required]
            mov eax, dword ptr [gNum_active_cars]
            sub esp, 0xfb4
            push ebx
            xor ebx, ebx
            push ebp
            push esi
            cmp ecx, ebx
            push edi
            je L004a7e70
            mov dword ptr [gActive_car_list_rebuild_required], ebx
            call PHILGetFirstObject
            mov esi, eax
            cmp esi, ebx
            je L004a7ae8
            lea edi, [esp + 0x24]
            mov ebp, 5
    L004a7ab9:
            mov ecx, esi
            call PHILGetNextObject
            test esi, esi
            je L004a7ae2
            mov ecx, dword ptr [esi + 0x23c]
            test ecx, ecx
            je L004a7ae2
            cmp byte ptr [esi + 0x238], 1
            jne L004a7ae2
            cmp dword ptr [ecx + 0xc], ebp
            jle L004a7ae2
            mov dword ptr [edi], esi
            inc ebx
            add edi, 4
    L004a7ae2:
            test eax, eax
            mov esi, eax
            jne L004a7ab9
    L004a7ae8:
            mov ecx, dword ptr [gProgram_state+0xb4]
            xor ebp, ebp
            mov dword ptr [esp + 0x10], ebp
            xor edi, edi
            mov esi, offset gActive_car_list
    L004a7afb:
            test edi, edi
            jle L004a7b0f
            mov ecx, offset gOpponent_nastyness_frigger+0x28
            call PDFatalError
            mov ecx, dword ptr [gProgram_state+0xb4]
    L004a7b0f:
            mov eax, dword ptr [gProgram_state+0xb0]
            test eax, eax
            je L004a7b21
            mov eax, dword ptr [gPed_676914]
            test eax, eax
            je L004a7b60
    L004a7b21:
            mov dword ptr [esi], 0x75bc2c
            inc ebp
            add esi, 4
            xor eax, eax
            test ebx, ebx
            mov dword ptr [gProgram_state+0x27c], 1
            jle L004a7b4f
            lea edx, [esp + 0x24]
    L004a7b3f:
            cmp dword ptr [edx], ecx
            je L004a7e88
            inc eax
            add edx, 4
            cmp eax, ebx
            jl L004a7b3f
    L004a7b4f:
            push 0
            push 0
            xor edx, edx
            call PHILAddActiveObject
            mov ecx, dword ptr [gProgram_state+0xb4]
    L004a7b60:
            inc edi
            cmp edi, 1
            jl L004a7afb
            mov eax, dword ptr [gNet_mode]
            mov dword ptr [esp + 0x10], ebp
            cmp eax, 2
            jne L004a7c1d
            xor edx, edx
            lea edi, [ebp*4 + gActive_car_list]
            mov dword ptr [esp + 0x14], edx
            mov ebp, offset gNet_players+0x19c
    L004a7b8a:
            xor ecx, ecx
            cmp eax, ecx
            je L004a7b98
            mov eax, dword ptr [gNumber_of_net_players]
            dec eax
            jmp L004a7b9a
    L004a7b98:
            xor eax, eax
    L004a7b9a:
            cmp edx, eax
            jge L004a7d77
            cmp edx, dword ptr [gThis_net_player_index]
            jge L004a7bb2
            mov eax, dword ptr [ebp - 0xd4]
            jmp L004a7bb5
    L004a7bb2:
            mov eax, dword ptr [ebp]
    L004a7bb5:
            cmp dword ptr [eax + 4], ecx
            jne L004a7bfe
            mov esi, dword ptr [esp + 0x10]
            mov dword ptr [edi], eax
            inc esi
            add edi, 4
            xor edx, edx
            mov dword ptr [eax + 0x1d0], 1
            mov eax, dword ptr [eax + 8]
            cmp ebx, ecx
            mov dword ptr [esp + 0x10], esi
            jle L004a7beb
            lea esi, [esp + 0x24]
    L004a7bdf:
            cmp dword ptr [esi], eax
            je L004a7bf8
            inc edx
            add esi, 4
            cmp edx, ebx
            jl L004a7bdf
    L004a7beb:
            push ecx
            push ecx
            xor edx, edx
            mov ecx, eax
            call PHILAddActiveObject
            jmp L004a7c04
    L004a7bf8:
            mov dword ptr [esp + edx*4 + 0x24], ecx
            jmp L004a7c04
    L004a7bfe:
            mov dword ptr [eax + 0x1d0], ecx
    L004a7c04:
            mov edx, dword ptr [esp + 0x14]
            mov eax, dword ptr [gNet_mode]
            inc edx
            add ebp, 0xd4
            mov dword ptr [esp + 0x14], edx
            jmp L004a7b8a
    L004a7c1d:
            cmp eax, 3
            jne L004a7d81
            xor ecx, ecx
            lea esi, [ebp*4 + gActive_car_list]
            mov dword ptr [esp + 0x14], ecx
            mov edi, offset gNet_players+0x19c
    L004a7c38:
            test eax, eax
            je L004a7c47
            mov edx, dword ptr [gNumber_of_net_players]
            lea eax, [edx - 1]
            jmp L004a7c49
    L004a7c47:
            xor eax, eax
    L004a7c49:
            cmp ecx, eax
            jge L004a7d7d
            cmp ecx, dword ptr [gThis_net_player_index]
            jge L004a7c61
            mov ecx, dword ptr [edi - 0xd4]
            jmp L004a7c63
    L004a7c61:
            mov ecx, dword ptr [edi]
    L004a7c63:
            mov eax, dword ptr [ecx + 4]
            test eax, eax
            jne L004a7d54
            mov eax, dword ptr [gProgram_state+0xbc]
            mov edx, dword ptr [ecx + 0x10]
            add edx, 0x50
            fld dword ptr [eax + 0x54]
            fld dword ptr [eax + 0x50]
            _emit 0xd8
        _emit 0x22
            fxch st(1)
            fsub dword ptr [edx + 4]
            fld dword ptr [eax + 0x58]
            fsub dword ptr [edx + 8]
            fxch st(2)
            fst dword ptr [esp + 0x18]
            fxch st(1)
            fst dword ptr [esp + 0x1c]
            fxch st(1)
            fmul dword ptr [esp + 0x18]
            fxch st(1)
            fmul dword ptr [esp + 0x1c]
            fld st(2)
            fmul st(0), st(3)
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fcomp dword ptr [DAT_0058a47c]
            fnstsw ax
            test ah, 1
            fstp st(0)
            jne L004a7d14
            mov eax, dword ptr [gCar_to_view]
            cmp eax, offset gProgram_state+0xac
            je L004a7d54
            mov eax, dword ptr [eax + 0x10]
            fld dword ptr [eax + 0x54]
            fld dword ptr [eax + 0x50]
            _emit 0xd8
        _emit 0x22
            fxch st(1)
            fsub dword ptr [edx + 4]
            fld dword ptr [eax + 0x58]
            fsub dword ptr [edx + 8]
            fxch st(2)
            fst dword ptr [esp + 0x18]
            fxch st(1)
            fst dword ptr [esp + 0x1c]
            fxch st(1)
            fmul dword ptr [esp + 0x18]
            fxch st(1)
            fmul dword ptr [esp + 0x1c]
            fld st(2)
            fmul st(0), st(3)
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fcomp dword ptr [DAT_0058a47c]
            fnstsw ax
            test ah, 1
            fstp st(0)
            je L004a7d54
    L004a7d14:
            mov dword ptr [esi], ecx
            inc ebp
            add esi, 4
            xor eax, eax
            mov dword ptr [ecx + 0x1d0], 1
            mov ecx, dword ptr [ecx + 8]
            test ebx, ebx
            jle L004a7d3d
            lea edx, [esp + 0x24]
    L004a7d31:
            cmp dword ptr [edx], ecx
            je L004a7d4a
            inc eax
            add edx, 4
            cmp eax, ebx
            jl L004a7d31
    L004a7d3d:
            push 0
            push 0
            xor edx, edx
            call PHILAddActiveObject
            jmp L004a7d5e
    L004a7d4a:
            mov dword ptr [esp + eax*4 + 0x24], 0
            jmp L004a7d5e
    L004a7d54:
            mov dword ptr [ecx + 0x1d0], 0
    L004a7d5e:
            mov ecx, dword ptr [esp + 0x14]
            mov eax, dword ptr [gNet_mode]
            inc ecx
            add edi, 0xd4
            mov dword ptr [esp + 0x14], ecx
            jmp L004a7c38
    L004a7d77:
            mov ebp, dword ptr [esp + 0x10]
            jmp L004a7d81
    L004a7d7d:
            mov dword ptr [esp + 0x10], ebp
    L004a7d81:
            mov eax, dword ptr [gProgram_state+0x1c20]
            xor edi, edi
            test eax, eax
            jle L004a7df9
            lea esi, [ebp*4 + gActive_car_list]
            mov ebp, offset gProgram_state+0x1dd0
    L004a7d98:
            mov edx, edi
            mov ecx, 2
            call GetCarSpec
            test byte ptr [ebp], 8
            jne L004a7dc0
            mov ecx, dword ptr [gPed_676914]
            test ecx, ecx
            jne L004a7dc0
            mov dword ptr [eax + 0x1d0], 0
            jmp L004a7de5
    L004a7dc0:
            mov ecx, dword ptr [esp + 0x10]
            mov dword ptr [esi], eax
            inc ecx
            push ebx
            mov dword ptr [esp + 0x14], ecx
            mov ecx, dword ptr [eax + 8]
            lea edx, [esp + 0x28]
            add esi, 4
            mov dword ptr [eax + 0x1d0], 1
            call AddIfNotInList
    L004a7de5:
            mov eax, dword ptr [gProgram_state+0x1c20]
            inc edi
            add ebp, 0x1a4
            cmp edi, eax
            jl L004a7d98
            mov ebp, dword ptr [esp + 0x10]
    L004a7df9:
            mov eax, dword ptr [gNumber_of_cops_before_faffage]
            xor edi, edi
            test eax, eax
            jle L004a7e61
            lea esi, [ebp*4 + gActive_car_list]
            mov ebp, offset gProgram_state+0x4f08
    L004a7e10:
            mov edx, edi
            mov ecx, 3
            call GetCarSpec
            test byte ptr [ebp], 8
            jne L004a7e2c
            mov ecx, dword ptr [gPed_676914]
            test ecx, ecx
            je L004a7e51
    L004a7e2c:
            mov ecx, dword ptr [esp + 0x10]
            mov dword ptr [esi], eax
            inc ecx
            push ebx
            mov dword ptr [esp + 0x14], ecx
            mov ecx, dword ptr [eax + 8]
            lea edx, [esp + 0x28]
            add esi, 4
            mov dword ptr [eax + 0x1d0], 1
            call AddIfNotInList
    L004a7e51:
            mov eax, dword ptr [gNumber_of_cops_before_faffage]
            inc edi
            add ebp, 0x1a4
            cmp edi, eax
            jl L004a7e10
    L004a7e61:
            mov edx, ebx
            lea ecx, [esp + 0x24]
            call RemoveAnythingStillInList
            mov eax, dword ptr [esp + 0x10]
    L004a7e70:
            cmp dword ptr [gNum_active_cars], eax
            je L004a7e7d
            mov dword ptr [gNum_active_cars], eax
    L004a7e7d:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0xfb4
            ret
    L004a7e88:
            mov dword ptr [esp + eax*4 + 0x24], 0
            jmp L004a7b60
    }
#endif
}

// C2PORT-NONMATCHING: best 57.78% | difftest PASS 2000 trials (spec difftest/specs/004ae380_SetInitialCopPositions.json) | reason VC5 pointer-base and loop register allocation
// FUNCTION: CARMA2_HW 0x004ae380
void C2_HOOK_FASTCALL SetInitialCopPositions(void) {
    int i;
    tU32 time_stamp;
    volatile tU32* pCop_fields;
    volatile tU32* pStart_words;

    i = 0;
    gNumber_of_cops_before_faffage = gProgram_state.AI_vehicles.number_of_cops;
    gDefinite_no_cop_pursuit_speed = 17.8788f;
    gCop_pursuit_speed_value_00691730 = 44.697f;
    gCop_pursuit_speed_percentage_multiplier = 3.7288110256195068359375f;

    /* pCop_fields starts at stun_time_ends; its relative words address the full tOpponent_spec. */
    if (gProgram_state.AI_vehicles.number_of_cops > 0) {
        pCop_fields = (volatile tU32*)&gProgram_state.AI_vehicles.cops[0].stun_time_ends;
        pStart_words = (tU32*)&gProgram_state.AI_vehicles.cop_start_points[0].v[1];
        do {
            PossibleService();
            ((tCar_spec*)pCop_fields[-0x15])->car_ID = i + 0x300;
            pCop_fields[-0x17] = 0x62;
            time_stamp = gTime_stamp_for_this_munging;
            pCop_fields[4] = time_stamp;
            pCop_fields[5] = time_stamp;
            pCop_fields[2] = time_stamp;
            pCop_fields[3] = 0;
            pCop_fields[0] = 0;
            pCop_fields[1] = IRandomBetween(0, 900) + 5000 + gTime_stamp_for_this_munging;
            pCop_fields[-4] = gTime_stamp_for_this_munging + 500;
            *(volatile tU32*)&pCop_fields[0x15] = (*(volatile tU32*)&pCop_fields[0x15] & 0xfffff7c9) | 8;
            *(tU16*)&pCop_fields[7] = 0xffff;
            pCop_fields[8] = 0;
            *(volatile tU32*)&pCop_fields[0x15] |= 1;
            pCop_fields[-0x16] = 0;
            *(volatile tU32*)&pCop_fields[0x15] &= 0xfffff8ff;
            /* The original pointer walk writes this value into opponents[i].nastiness. */
            *(volatile float*)&pCop_fields[-0xc62] = (((gCurrent_race.race_spec->opponent_nastiness_level - (float)gProgram_state.skill_level * gCop_nastiness_skill_adjustment) - gCop_nastiness_bias) * gCop_nastiness_divisor) * gOpponent_nastyness_frigger;
            pCop_fields[-0x10] = pStart_words[-1];
            pCop_fields[-0xf] = pStart_words[0];
            pCop_fields[-0xe] = pStart_words[1];
            pCop_fields[-0xd] = pStart_words[0x1d];
            pCop_fields[-0xc] = pStart_words[0x1e];
            pCop_fields[-0xb] = pStart_words[0x1f];
            pCop_fields[-10] = 0;
            pCop_fields[-9] = 0;
            pCop_fields[-8] = 0;
            gViewable_car_list[gNum_viewable_cars] = (tCar_spec*)pCop_fields[-0x15];
            gNum_viewable_cars++;
            ((tCar_spec*)pCop_fields[-0x15])->last_person_to_hit_us = NULL;
            ((tCar_spec*)pCop_fields[-0x15])->last_person_we_hit = NULL;
            ((tCar_spec*)pCop_fields[-0x15])->last_collision_time = 0;
            ((tCar_spec*)pCop_fields[-0x15])->field_0x1548 = 0;
            ((tCar_spec*)pCop_fields[-0x15])->no_of_processes_recording_my_trail = 0;
            ((tCar_spec*)pCop_fields[-0x15])->grudge_raised_recently = 1;
            gOpponents[pCop_fields[-0x17]].psyche.grudge_against_player = 10;
            ((tCar_spec*)pCop_fields[-0x15])->acc_force = 0.f;
            ((tCar_spec*)pCop_fields[-0x15])->brake_force = 0.f;
            ((tCar_spec*)pCop_fields[-0x15])->curvature = 0.f;
            time_stamp = gTime_stamp_for_this_munging + 0x0fffffff;
            if (pCop_fields[0] > time_stamp) {
                time_stamp = pCop_fields[0];
            }
            pCop_fields[0] = time_stamp;
            i++;
            pStart_words += 3;
            pCop_fields += 0x69;
        } while (i < gProgram_state.AI_vehicles.number_of_cops);
    }

    i = 0;
    pCop_fields = (volatile tU32*)&gProgram_state.AI_vehicles.cops[0].car_spec;
    pStart_words = (tU32*)&gProgram_state.AI_vehicles.cop_start_points[0].v[1];
second_cop_loop:
    if (i >= *(volatile int*)&gNumber_of_cops_before_faffage) {
        goto done;
    }
    PossibleService();
    *(tU32*)&((tCar_spec*)pCop_fields[0])->car_master_actor->t.t.translate.t.v[0] = pStart_words[-1];
    *(tU32*)&((tCar_spec*)pCop_fields[0])->car_master_actor->t.t.translate.t.v[1] = pStart_words[0];
    *(tU32*)&((tCar_spec*)pCop_fields[0])->car_master_actor->t.t.translate.t.v[2] = pStart_words[1];
    PointActorAlongThisBloodyVector(((tCar_spec*)pCop_fields[0])->car_master_actor, (br_vector3*)(pStart_words + 0x1d));
    pCop_fields[0x2a] &= 0xfffffff7;
    RematerialiseOpponent((tOpponent_spec*)&pCop_fields[-2], 0.f);
    InitCarSkidStuff((tCar_spec*)pCop_fields[0]);
    i++;
    pStart_words += 3;
    pCop_fields += 0x69;
    goto second_cop_loop;
done:
    return;
}

// FUNCTION: CARMA2_HW 0x004ae5d0
void C2_HOOK_FASTCALL DisposeOpponents(void) {
    int i;

    C2_HOOK_BUG_ON(offsetof(tIntelligent_vehicles, cops) != 12856);
    if (gProgram_state.AI_vehicles.number_of_cops != 0) {
        for (i = 0; i < gProgram_state.AI_vehicles.number_of_cops; i++) {
            DisposeCar(gProgram_state.AI_vehicles.cops[i].car_spec, (i == gBIG_APC_index) ? 99 : 98);
            BrMemFree(gProgram_state.AI_vehicles.cops[i].car_spec);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004ae880
const char* C2_HOOK_FASTCALL GetDriverName(tVehicle_type pCategory, int pIndex) {
    tCar_spec* c;
    switch (pCategory) {
    case eVehicle_self:
        return gProgram_state.player_name;
    case eVehicle_opponent:
        c = gProgram_state.AI_vehicles.opponents[pIndex].car_spec;
        if (c != NULL) {
            if (c->driver_name != NULL) {
                return c->driver_name;
            }
        }
        return "???";
    case eVehicle_rozzer:
        return gFaceless_cop_name;
    case eVehicle_drone:
        return gInnocent_civilian_name;
    case eVehicle_not_really:
        return NULL;
    default:
        return NULL;
    }
}

void C2_HOOK_FASTCALL StunTheBugger(tOpponent_spec* pOpponent_spec, int pMilliseconds) {

    pOpponent_spec->car_spec->acc_force = 0.f;
    pOpponent_spec->car_spec->brake_force = 0.f;
    pOpponent_spec->car_spec->curvature = 0.f;
    pOpponent_spec->stun_time_ends = MAX(pOpponent_spec->stun_time_ends, gTime_stamp_for_this_munging + pMilliseconds);
}

// FUNCTION: CARMA2_HW 0x004ae970
tCar_spec* C2_HOOK_FASTCALL GetCarSpecFromGlobalOppoIndex(int pIndex) {
    int i;

    for (i = 0; i < gProgram_state.AI_vehicles.number_of_opponents; i++) {
        if (gProgram_state.AI_vehicles.opponents[i].index == pIndex) {
            return gProgram_state.AI_vehicles.opponents[i].car_spec;
        }
    }
    return NULL;
}

// FUNCTION: CARMA2_HW 0x004ae8e0
tOpponent_spec* C2_HOOK_FASTCALL GetOpponentSpecFromCarSpec(tCar_spec* pCar_spec) {
    int i;
    tU16 car_ID;
    int car_category;

    car_ID = pCar_spec->car_ID;
    car_category = car_ID & ~0xff;
    if (car_category == eVehicle_opponent << 8) {
        for (i = 0; i < GetCarCount(eVehicle_opponent); i++) {
            if (gProgram_state.AI_vehicles.opponents[i].car_spec == pCar_spec) {
                return &gProgram_state.AI_vehicles.opponents[i];
            }
        }
    } else if (car_category == eVehicle_rozzer << 8) {
        for (i = 0; i < GetCarCount(eVehicle_rozzer); i++) {
            if (gProgram_state.AI_vehicles.cops[i].car_spec == pCar_spec) {
                return &gProgram_state.AI_vehicles.cops[i];
            }
        }
    }
    return NULL;
}

// FUNCTION: CARMA2_HW 0x004a8160
void C2_HOOK_CDECL DoNotDprintf_opponent(const char* pMessage, ...) {
}

// FUNCTION: CARMA2_HW 0x004aeef0
void C2_NAKED C2_HOOK_FASTCALL RecordOpponentTwattageOccurrence(tCar_spec* pTwatter, tCar_spec* pTwattee) {
#ifndef CARPOCALYPSE2_MATCHING
    int bangness;
    int twatter_index;
    int twattee_index;
    int grudginess_caused_by_damage;
    int new_grudge_value;
    float damage;
    tOpponent_spec* twattee_opponent_spec;
    tOpponent_spec* twatter_opponent_spec;

    if ((pTwatter != NULL && pTwatter->driver == eDriver_oppo) || (pTwattee != NULL && pTwattee->driver == eDriver_oppo)) {
        damage = pTwattee->damage_magnitude_accumulator;
        bangness = (int)((100.0 < sqrt(damage * 300000.0)) ? 100.0 : sqrt(damage * 300000.0));
        grudginess_caused_by_damage = bangness / 10;
        grudginess_caused_by_damage += 50 * ((pTwattee->car_ID & 0xff00) == 0x300);
        DoNotDprintf_opponent("Frame %0.6d: %s hit %s, damage %f, bangness %d, gBig_bang %d, grudginess %d",
            gAcme_frame_count,
            pTwatter->driver_name,
            pTwattee->driver_name,
            damage,
            bangness,
            gBig_bang,
            grudginess_caused_by_damage);
        if (bangness < 5) {
            return;
        }
        pTwatter->last_person_we_hit = pTwattee;
        pTwatter->last_collision_time = gTime_stamp_for_this_munging;
        pTwattee->last_collision_time = gTime_stamp_for_this_munging;
        pTwattee->last_person_to_hit_us = pTwatter;
        pTwattee->grudge_raised_recently = 1;
        if (bangness >= gBig_bang || (((pTwattee->car_ID & 0xff00) == 0x300) != 0)) {
            pTwattee->big_bang = 1;
        }
        if (bangness >= 80) {
            pTwattee->scary_bang = 1;
        }
        if (pTwatter->driver == eDriver_local_human) {
            twattee_opponent_spec = NULL;
            if ((pTwattee->car_ID & 0xff00) == 0x200) {
                for (twattee_index = 0; twattee_index < gProgram_state.AI_vehicles.number_of_opponents; twattee_index++) {
                    if (gProgram_state.AI_vehicles.opponents[twattee_index].car_spec == pTwattee) {
                        twattee_opponent_spec = &gProgram_state.AI_vehicles.opponents[twattee_index];
                        break;
                    }
                }
            } else if ((pTwattee->car_ID & 0xff00) == 0x300) {
                for (twattee_index = 0; twattee_index < gNumber_of_cops_before_faffage; twattee_index++) {
                    if (gProgram_state.AI_vehicles.cops[twattee_index].car_spec == pTwattee) {
                        twattee_opponent_spec = &gProgram_state.AI_vehicles.cops[twattee_index];
                        break;
                    }
                }
            }
            if (pTwattee->scary_bang) {
                if (gOpponents[twattee_opponent_spec->index].strength_rating < 5) {
                    twattee_opponent_spec->car_spec->acc_force = 0.f;
                    twattee_opponent_spec->car_spec->brake_force = 0.f;
                    twattee_opponent_spec->car_spec->curvature = 0.f;
                    twattee_opponent_spec->stun_time_ends = MAX(twattee_opponent_spec->stun_time_ends, gTime_stamp_for_this_munging + 30 * MIN(bangness, 100) + 1000);
                }
            }
            twattee_index = twattee_opponent_spec->index;
            new_grudge_value = grudginess_caused_by_damage + gOpponents[twattee_index].psyche.grudge_against_player;
            if (new_grudge_value > 100) {
                new_grudge_value = 100;
            }
            gOpponents[twattee_index].psyche.grudge_against_player = new_grudge_value;
        } else if (pTwattee->driver == eDriver_local_human) {
            twatter_opponent_spec = NULL;
            if ((pTwatter->car_ID & 0xff00) == 0x200) {
                for (twatter_index = 0; twatter_index < gProgram_state.AI_vehicles.number_of_opponents; twatter_index++) {
                    if (gProgram_state.AI_vehicles.opponents[twatter_index].car_spec == pTwatter) {
                        twatter_opponent_spec = &gProgram_state.AI_vehicles.opponents[twatter_index];
                        break;
                    }
                }
            } else if ((pTwatter->car_ID & 0xff00) == 0x300) {
                for (twatter_index = 0; twatter_index < gNumber_of_cops_before_faffage; twatter_index++) {
                    if (gProgram_state.AI_vehicles.cops[twatter_index].car_spec == pTwatter) {
                        twatter_opponent_spec = &gProgram_state.AI_vehicles.cops[twatter_index];
                        break;
                    }
                }
            }
            twatter_index = twatter_opponent_spec->index;
            if (twatter_opponent_spec->current_objective == eOOT_pursue_and_twat && twatter_opponent_spec->pursue_car_data.pursuee == pTwattee) {
                twatter_opponent_spec->pursue_car_data.time_last_twatted_em = gTime_stamp_for_this_munging;
            }
            new_grudge_value = gOpponents[twatter_index].psyche.grudge_against_player - (twatter_opponent_spec->current_objective == eOOT_pursue_and_twat ? 0 : 2 * grudginess_caused_by_damage);
            if (new_grudge_value < 0) {
                new_grudge_value = 0;
            }
            gOpponents[twatter_index].psyche.grudge_against_player = new_grudge_value;
        } else {
            twatter_opponent_spec = NULL;
            if ((pTwatter->car_ID & 0xff00) == 0x200) {
                for (twatter_index = 0; twatter_index < gProgram_state.AI_vehicles.number_of_opponents; twatter_index++) {
                    if (gProgram_state.AI_vehicles.opponents[twatter_index].car_spec == pTwatter) {
                        twatter_opponent_spec = &gProgram_state.AI_vehicles.opponents[twatter_index];
                        break;
                    }
                }
            } else if ((pTwatter->car_ID & 0xff00) == 0x300) {
                for (twatter_index = 0; twatter_index < gNumber_of_cops_before_faffage; twatter_index++) {
                    if (gProgram_state.AI_vehicles.cops[twatter_index].car_spec == pTwatter) {
                        twatter_opponent_spec = &gProgram_state.AI_vehicles.cops[twatter_index];
                        break;
                    }
                }
            }
            twattee_opponent_spec = NULL;
            if ((pTwattee->car_ID & 0xff00) == 0x200) {
                for (twattee_index = 0; twattee_index < gProgram_state.AI_vehicles.number_of_opponents; twattee_index++) {
                    if (gProgram_state.AI_vehicles.opponents[twattee_index].car_spec == pTwattee) {
                        twattee_opponent_spec = &gProgram_state.AI_vehicles.opponents[twattee_index];
                        break;
                    }
                }
            } else if ((pTwattee->car_ID & 0xff00) == 0x300) {
                for (twattee_index = 0; twattee_index < gNumber_of_cops_before_faffage; twattee_index++) {
                    if (gProgram_state.AI_vehicles.cops[twattee_index].car_spec == pTwattee) {
                        twattee_opponent_spec = &gProgram_state.AI_vehicles.cops[twattee_index];
                        break;
                    }
                }
            }
            if (pTwattee->scary_bang) {
                if (gOpponents[twattee_opponent_spec->index].strength_rating < 5) {
                    twattee_opponent_spec->car_spec->acc_force = 0.f;
                    twattee_opponent_spec->car_spec->brake_force = 0.f;
                    twattee_opponent_spec->car_spec->curvature = 0.f;
                    twattee_opponent_spec->stun_time_ends = MAX(twattee_opponent_spec->stun_time_ends, gTime_stamp_for_this_munging + 30 * bangness + 1000);
                }
            }
            if (twatter_opponent_spec->current_objective == eOOT_pursue_and_twat && twatter_opponent_spec->pursue_car_data.pursuee == pTwattee) {
                twatter_opponent_spec->pursue_car_data.time_last_twatted_em = gTime_stamp_for_this_munging;
            }
            if (CAR_SPEC_IS_OPPONENT(pTwatter) && (((pTwattee->car_ID & 0xff00) == 0x300) != 0)) {
                twattee_index = twattee_opponent_spec->index;
                new_grudge_value = grudginess_caused_by_damage + gOpponents[twattee_index].psyche.grudge_against_player;
                if (new_grudge_value > 100) {
                    new_grudge_value = 100;
                }
                gOpponents[twattee_index].psyche.grudge_against_player = new_grudge_value;
            }
        }
    }
#else
    __asm {
            sub esp, 0x14
            push ebx
            push ebp
            push esi
            push edi
            mov ebx, ecx
            mov edi, edx
            test ebx, ebx
            mov dword ptr [esp + 0x10], edi
            mov eax, 6
            je L004aef0d
            cmp dword ptr [ebx + 0xc], eax
            je L004aef1e
    L004aef0d:
            test edi, edi
            je L004af464
            cmp dword ptr [edi + 0xc], eax
            jne L004af464
    L004aef1e:
            fld dword ptr [edi + 0x133c]
            fst dword ptr [esp + 0x14]
            fld qword ptr [gCop_nastiness_bias+0x10]
            fxch st(1)
            fst qword ptr [esp + 0x1c]
            fmul qword ptr [gCop_nastiness_bias+0x8]
            fsqrt
            fxch st(1)
            fcomp st(1)
            fnstsw ax
            test ah, 1
            je L004aef51
            fstp st(0)
            fld qword ptr [gCop_nastiness_bias+0x10]
            jmp L004aef59
    L004aef51:
            fld dword ptr [esp + 0x14]
            fstp qword ptr [esp + 0x1c]
    L004aef59:
            call _ftol
            mov esi, eax
            mov eax, 0x66666667
            imul esi
            sar edx, 2
            mov eax, edx
            xor ecx, ecx
            mov cx, word ptr [edi + 0x80]
            mov dword ptr [esp + 0x18], esi
            shr eax, 0x1f
            add edx, eax
            and cl, 0
            xor eax, eax
            cmp ecx, 0x300
            sete al
            mov ecx, dword ptr [gBig_bang]
            lea eax, [eax + eax*4]
            lea eax, [eax + eax*4]
            lea edx, [edx + eax*2]
            mov eax, dword ptr [esp + 0x1c]
            mov dword ptr [esp + 0x14], edx
            push edx
            mov edx, dword ptr [esp + 0x24]
            push ecx
            push esi
            push edx
            lea ecx, [edi + 0xec]
            push eax
            mov eax, dword ptr [gAcme_frame_count]
            lea edx, [ebx + 0xec]
            push ecx
            push edx
            push eax
            push offset gOpponent_nastyness_frigger+0x15b4
            call DoNotDprintf_opponent
            add esp, 0x24
            cmp esi, 5
            jl L004af464
            mov ecx, dword ptr [gTime_stamp_for_this_munging]
            mov dword ptr [ebx + 0x1564], edi
            mov dword ptr [ebx + 0x1544], ecx
            mov edx, dword ptr [gTime_stamp_for_this_munging]
            mov dword ptr [edi + 0x1544], edx
            mov edx, dword ptr [edi + 0x153c]
            or edx, 1
            mov dword ptr [edi + 0x1560], ebx
            mov dword ptr [edi + 0x153c], edx
            mov ecx, dword ptr [gBig_bang]
            cmp esi, ecx
            mov eax, edx
            mov ebp, 2
            jge L004af02e
            xor ecx, ecx
            mov cx, word ptr [edi + 0x80]
            and cl, 0
            cmp ecx, 0x300
            jne L004af036
    L004af02e:
            or eax, ebp
            mov dword ptr [edi + 0x153c], eax
    L004af036:
            cmp esi, 0x50
            jl L004af049
            mov eax, dword ptr [edi + 0x153c]
            or al, 4
            mov dword ptr [edi + 0x153c], eax
    L004af049:
            test ebx, ebx
            mov eax, 8
            je L004af193
            cmp dword ptr [ebx + 0xc], eax
            jne L004af193
            xor eax, eax
            mov ax, word ptr [edi + 0x80]
            and al, 0
            cmp eax, 0x200
            jne L004af0a5
            mov edx, dword ptr [gProgram_state+0x1c20]
            xor eax, eax
            mov ecx, offset gProgram_state+0x1d28
    L004af07e:
            cmp eax, edx
            jge L004af0e0
            cmp dword ptr [ecx], edi
            je L004af08f
            inc eax
            add ecx, 0x1a4
            jmp L004af07e
    L004af08f:
            mov ecx, eax
            shl ecx, 3
            sub ecx, eax
            lea eax, [ecx + ecx*2]
            lea edx, [eax + eax*4]
            lea eax, [edx*4 + gProgram_state+0x1d20]
            jmp L004af0e2
    L004af0a5:
            cmp eax, 0x300
            jne L004af0e0
            mov edx, dword ptr [gNumber_of_cops_before_faffage]
            xor eax, eax
            mov ecx, offset gProgram_state+0x4e60
    L004af0b9:
            cmp eax, edx
            jge L004af0e0
            cmp dword ptr [ecx], edi
            je L004af0ca
            inc eax
            add ecx, 0x1a4
            jmp L004af0b9
    L004af0ca:
            mov ecx, eax
            shl ecx, 3
            sub ecx, eax
            lea eax, [ecx + ecx*2]
            lea eax, [eax + eax*4]
            lea eax, [eax*4 + gProgram_state+0x4e58]
            jmp L004af0e2
    L004af0e0:
            xor eax, eax
    L004af0e2:
            test byte ptr [edi + 0x153c], 4
            je L004af151
            mov ecx, dword ptr [eax]
            mov edx, ecx
            shl edx, 3
            sub edx, ecx
            mov ecx, dword ptr [gOpponents]
            shl edx, 6
            cmp dword ptr [edx + ecx + 0x19c], 5
            jge L004af151
            cmp esi, 0x64
            mov ecx, 0x64
            jg L004af113
            mov ecx, esi
    L004af113:
            mov esi, dword ptr [eax + 8]
            lea ecx, [ecx + ecx*2]
            lea edx, [ecx + ecx*4]
            lea ecx, [edx + edx + 0x3e8]
            xor edx, edx
            mov dword ptr [esi + 0x12b8], edx
            mov esi, dword ptr [eax + 8]
            mov dword ptr [esi + 0x12c0], edx
            mov esi, dword ptr [eax + 8]
            mov dword ptr [esi + 0x1254], edx
            mov edx, dword ptr [gTime_stamp_for_this_munging]
            add ecx, edx
            mov edx, dword ptr [eax + 0x5c]
            cmp edx, ecx
            jbe L004af14e
            mov ecx, edx
    L004af14e:
            mov dword ptr [eax + 0x5c], ecx
    L004af151:
            mov eax, dword ptr [eax]
            mov edx, dword ptr [gOpponents]
            mov ecx, eax
            shl ecx, 3
            sub ecx, eax
            xor eax, eax
            shl ecx, 6
            mov al, byte ptr [ecx + edx + 0x1b8]
            lea ecx, [ecx + edx + 0x1b8]
            mov edx, eax
            mov eax, dword ptr [esp + 0x14]
            add eax, edx
            cmp eax, 0x64
            jle L004af462
            mov eax, 0x64
            mov byte ptr [ecx], al
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x14
            ret
    L004af193:
            test edi, edi
            je L004af283
            cmp dword ptr [edi + 0xc], eax
            jne L004af283
            xor eax, eax
            mov ax, word ptr [ebx + 0x80]
            and al, 0
            cmp eax, 0x200
            jne L004af1ea
            mov edx, dword ptr [gProgram_state+0x1c20]
            xor eax, eax
            mov ecx, offset gProgram_state+0x1d28
    L004af1c3:
            cmp eax, edx
            jge L004af225
            cmp dword ptr [ecx], ebx
            je L004af1d4
            inc eax
            add ecx, 0x1a4
            jmp L004af1c3
    L004af1d4:
            mov ecx, eax
            shl ecx, 3
            sub ecx, eax
            lea eax, [ecx + ecx*2]
            lea eax, [eax + eax*4]
            lea eax, [eax*4 + gProgram_state+0x1d20]
            jmp L004af227
    L004af1ea:
            cmp eax, 0x300
            jne L004af225
            mov edx, dword ptr [gNumber_of_cops_before_faffage]
            xor eax, eax
            mov ecx, offset gProgram_state+0x4e60
    L004af1fe:
            cmp eax, edx
            jge L004af225
            cmp dword ptr [ecx], ebx
            je L004af20f
            inc eax
            add ecx, 0x1a4
            jmp L004af1fe
    L004af20f:
            mov ecx, eax
            shl ecx, 3
            sub ecx, eax
            lea eax, [ecx + ecx*2]
            lea ecx, [eax + eax*4]
            lea eax, [ecx*4 + gProgram_state+0x4e58]
            jmp L004af227
    L004af225:
            xor eax, eax
    L004af227:
            cmp dword ptr [eax + 4], ebp
            jne L004af240
            cmp dword ptr [eax + 0x114], edi
            jne L004af240
            mov edx, dword ptr [gTime_stamp_for_this_munging]
            mov dword ptr [eax + 0x120], edx
    L004af240:
            mov edx, dword ptr [eax + 4]
            mov ecx, dword ptr [eax]
            cmp edx, ebp
            jne L004af24d
            xor edx, edx
            jmp L004af254
    L004af24d:
            mov eax, dword ptr [esp + 0x14]
            lea edx, [eax + eax]
    L004af254:
            mov eax, ecx
            shl eax, 3
            sub eax, ecx
            mov ecx, dword ptr [gOpponents]
            shl eax, 6
            lea ecx, [eax + ecx + 0x1b8]
            xor eax, eax
            mov al, byte ptr [ecx]
            sub eax, edx
            jns L004af462
            xor eax, eax
            mov byte ptr [ecx], al
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x14
            ret
    L004af283:
            mov esi, dword ptr [gProgram_state+0x1c20]
            xor eax, eax
            mov ax, word ptr [ebx + 0x80]
            and al, 0
            cmp eax, 0x200
            jne L004af2c9
            xor eax, eax
            mov ecx, offset gProgram_state+0x1d28
    L004af2a2:
            cmp eax, esi
            jge L004af304
            cmp dword ptr [ecx], ebx
            je L004af2b3
            inc eax
            add ecx, 0x1a4
            jmp L004af2a2
    L004af2b3:
            mov ecx, eax
            shl ecx, 3
            sub ecx, eax
            lea eax, [ecx + ecx*2]
            lea edx, [eax + eax*4]
            lea ebp, [edx*4 + gProgram_state+0x1d20]
            jmp L004af306
    L004af2c9:
            cmp eax, 0x300
            jne L004af304
            mov edx, dword ptr [gNumber_of_cops_before_faffage]
            xor eax, eax
            mov ecx, offset gProgram_state+0x4e60
    L004af2dd:
            cmp eax, edx
            jge L004af304
            cmp dword ptr [ecx], ebx
            je L004af2ee
            inc eax
            add ecx, 0x1a4
            jmp L004af2dd
    L004af2ee:
            mov ecx, eax
            shl ecx, 3
            sub ecx, eax
            lea eax, [ecx + ecx*2]
            lea eax, [eax + eax*4]
            lea ebp, [eax*4 + gProgram_state+0x4e58]
            jmp L004af306
    L004af304:
            xor ebp, ebp
    L004af306:
            xor eax, eax
            mov ax, word ptr [edi + 0x80]
            and al, 0
            cmp eax, 0x200
            jne L004af346
            xor eax, eax
            mov ecx, offset gProgram_state+0x1d28
    L004af31f:
            cmp eax, esi
            jge L004af389
            cmp dword ptr [ecx], edi
            je L004af330
            inc eax
            add ecx, 0x1a4
            jmp L004af31f
    L004af330:
            mov ecx, eax
            shl ecx, 3
            sub ecx, eax
            lea eax, [ecx + ecx*2]
            lea ecx, [eax + eax*4]
            lea eax, [ecx*4 + gProgram_state+0x1d20]
            jmp L004af38b
    L004af346:
            cmp eax, 0x300
            jne L004af389
            mov ecx, 3
            xor esi, esi
            call GetCarCount
            test eax, eax
            jle L004af389
            mov edi, offset gProgram_state+0x4e60
    L004af362:
            mov edx, dword ptr [esp + 0x10]
            mov eax, dword ptr [edi]
            cmp eax, edx
            je L004af46c
            mov ecx, 3
            inc esi
            add edi, 0x1a4
            call GetCarCount
            cmp esi, eax
            jl L004af362
            mov edi, dword ptr [esp + 0x10]
    L004af389:
            xor eax, eax
    L004af38b:
            test byte ptr [edi + 0x153c], 4
            je L004af3f2
            mov ecx, dword ptr [eax]
            mov edx, ecx
            shl edx, 3
            sub edx, ecx
            mov ecx, dword ptr [gOpponents]
            shl edx, 6
            cmp dword ptr [edx + ecx + 0x19c], 5
            jge L004af3f2
            mov ecx, dword ptr [esp + 0x18]
            mov esi, dword ptr [eax + 8]
            lea ecx, [ecx + ecx*2]
            lea edx, [ecx + ecx*4]
            lea ecx, [edx + edx + 0x3e8]
            xor edx, edx
            mov dword ptr [esi + 0x12b8], edx
            mov esi, dword ptr [eax + 8]
            mov dword ptr [esi + 0x12c0], edx
            mov esi, dword ptr [eax + 8]
            mov dword ptr [esi + 0x1254], edx
            mov edx, dword ptr [gTime_stamp_for_this_munging]
            add ecx, edx
            mov edx, dword ptr [eax + 0x5c]
            cmp edx, ecx
            jbe L004af3ef
            mov ecx, edx
    L004af3ef:
            mov dword ptr [eax + 0x5c], ecx
    L004af3f2:
            mov ecx, dword ptr [ebp + 4]
            mov eax, dword ptr [eax]
            cmp ecx, 2
            jne L004af410
            cmp dword ptr [ebp + 0x114], edi
            jne L004af410
            mov ecx, dword ptr [gTime_stamp_for_this_munging]
            mov dword ptr [ebp + 0x120], ecx
    L004af410:
            xor edx, edx
            mov dx, word ptr [ebx + 0x80]
            and dl, 0
            cmp edx, 0x200
            jne L004af464
            xor ecx, ecx
            mov cx, word ptr [edi + 0x80]
            and cl, 0
            cmp ecx, 0x300
            jne L004af464
            mov edx, eax
            shl edx, 3
            sub edx, eax
            mov eax, dword ptr [gOpponents]
            shl edx, 6
            lea ecx, [edx + eax + 0x1b8]
            mov eax, dword ptr [esp + 0x14]
            xor edx, edx
            mov dl, byte ptr [ecx]
            add eax, edx
            cmp eax, 0x64
            jle L004af462
            mov eax, 0x64
    L004af462:
            mov byte ptr [ecx], al
    L004af464:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x14
            ret
    L004af46c:
            mov eax, esi
            mov edi, dword ptr [esp + 0x10]
            shl eax, 3
            sub eax, esi
            lea eax, [eax + eax*2]
            lea eax, [eax + eax*4]
            lea eax, [eax*4 + gProgram_state+0x4e58]
            jmp L004af38b
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004add50
void C2_HOOK_FASTCALL InitOpponents(tRace_info* pRace_info) {
    int i;
    int opponent_number;
    int rank_dependent_difficulty;
    br_bounds bounds;
    tCar_spec* car_spec;
    tOpponent_spec* opponent_spec;
    int opponents_left;

    gNext_grudge_reduction = gTime_stamp_for_this_munging + 10000;
    gFirst_frame = 1;
    gNum_viewable_cars = 1;
    gGrudge_reduction_per_period = 3 - gProgram_state.skill_level;
    gAcknowledged_start = 0;
    gStart_jumped = 0;
    gViewable_car_list[0] = &gProgram_state.current_car;
    BrActorToBounds(&bounds, gProgram_state.track_spec.the_actor);
    gMinimum_yness_before_knackerisation = bounds.min.v[1] - 2.f;
    gAcme_frame_count = 0;
    gProgram_state.current_car.no_of_processes_recording_my_trail = 0;
    gProgram_state.current_car.field_0x1540 = 0;
    gProgram_state.current_car.field_0x155c = -1;
    gHead_on_cos_value = cosf(.5235668420791626f); /* around 30 degrees */
    rank_dependent_difficulty = (int)((101.f - 99.f * (1.f - (float)gCurrent_race.index / (float)gNumber_of_races)) / -10.f);
    gBig_bang = (int)(70.f - (10 * gProgram_state.skill_level - 3 * rank_dependent_difficulty) * gOpponent_nastyness_frigger);
    gIn_view_distance = gCamera_yon + 10.f;
    if (gIn_view_distance < 45.f) {
        gIn_view_distance = 45.f;
    }
    if (gNet_mode == eNet_mode_none) {
        gProgram_state.AI_vehicles.number_of_opponents = pRace_info->number_of_racers - 1;
    } else {
        gProgram_state.AI_vehicles.number_of_opponents = 0;
    }
    for (i = 0, opponent_number = 0; i < gProgram_state.AI_vehicles.number_of_opponents; i++, opponent_number++) {
        PossibleService();
        if (pRace_info->opponent_list[opponent_number].index < 0) {
            opponent_number++;
        }
        gProgram_state.AI_vehicles.opponents[i].car_spec = pRace_info->opponent_list[opponent_number].car_spec;
        gProgram_state.AI_vehicles.opponents[i].car_spec->car_ID = i | 0x200;
        DoNotDprintf_opponent("Car '%s', car_ID %x",
            gProgram_state.AI_vehicles.opponents[i].car_spec->driver_name,
            gProgram_state.AI_vehicles.opponents[i].car_spec->car_ID);
        gProgram_state.AI_vehicles.opponents[i].index = pRace_info->opponent_list[opponent_number].index;
        gProgram_state.AI_vehicles.opponents[i].time_last_processed = gTime_stamp_for_this_munging;
        gProgram_state.AI_vehicles.opponents[i].next_repair_check = 0;
        gProgram_state.AI_vehicles.opponents[i].repair_interval = 60000 * IRandomBetween(gMinTimeOpponentRepair, gMaxTimeOpponentRepair);
        gProgram_state.AI_vehicles.opponents[i].last_repair_time = gTime_stamp_for_this_munging;
        DoNotDprintf_opponent("*** Opponent %s, repair_interval is %d",
            gProgram_state.AI_vehicles.opponents[i].car_spec->driver_name,
            gProgram_state.AI_vehicles.opponents[i].repair_interval);
        gProgram_state.AI_vehicles.opponents[i].time_this_objective_started = gTime_stamp_for_this_munging;
        gProgram_state.AI_vehicles.opponents[i].last_moved_ok = gTime_stamp_for_this_munging;
        gProgram_state.AI_vehicles.opponents[i].last_in_view = 0;
        gProgram_state.AI_vehicles.opponents[i].stun_time_ends = 0;
        gProgram_state.AI_vehicles.opponents[i].next_player_visibility_check = gTime_stamp_for_this_munging + IRandomBetween(0, 900) + 2000;
        gProgram_state.AI_vehicles.opponents[i].next_out_of_world_check = gTime_stamp_for_this_munging + 500;

        /*
         * gProgram_state.AI_vehicles.opponents[i].flags &= ~0x36;
         * gProgram_state.AI_vehicles.opponents[i].flags |= 0x8;
         */
        gProgram_state.AI_vehicles.opponents[i].finished_for_this_race = 0;
        gProgram_state.AI_vehicles.opponents[i].physics_me = 1;
        gProgram_state.AI_vehicles.opponents[i].pursue_from_start = 0;
        gProgram_state.AI_vehicles.opponents[i].cheating = 0;
        gProgram_state.AI_vehicles.opponents[i].knackeredness_detected = 0;

        gProgram_state.AI_vehicles.opponents[i].players_section_when_last_calced_full_path = -1;
        gProgram_state.AI_vehicles.opponents[i].car_spec->last_person_to_hit_us = NULL;
        gProgram_state.AI_vehicles.opponents[i].car_spec->last_person_we_hit = NULL;
        gProgram_state.AI_vehicles.opponents[i].car_spec->last_collision_time = 0;
        gProgram_state.AI_vehicles.opponents[i].car_spec->field_0x1548 = gTime_stamp_for_this_munging;
        gProgram_state.AI_vehicles.opponents[i].car_spec->grudge_raised_recently = 1;
        gProgram_state.AI_vehicles.opponents[i].car_spec->no_of_processes_recording_my_trail = 0;
        gProgram_state.AI_vehicles.opponents[i].nnext_sections = 0;
        gProgram_state.AI_vehicles.opponents[i].new_objective_required = 1;
        gProgram_state.AI_vehicles.opponents[i].current_objective = eOOT_none;

        /* gProgram_state.AI_vehicles.opponents[i].flags &= ~0x700; */
        gProgram_state.AI_vehicles.opponents[i].has_moved_at_some_point = 0;
        gProgram_state.AI_vehicles.opponents[i].player_in_view_now = 0;
        gProgram_state.AI_vehicles.opponents[i].acknowledged_piv = 0;
        gProgram_state.AI_vehicles.opponents[i].field_0xb4 = 0;
        gProgram_state.AI_vehicles.opponents[i].nastiness = (
                  gCurrent_race.race_spec->opponent_nastiness_level
                + gProgram_state.skill_level / 2.f
                + (gOpponents[gProgram_state.AI_vehicles.opponents[i].index].strength_rating - 1) / 4.f
                ) / 3.f * gOpponent_nastyness_frigger;

        BrVector3Set(&gProgram_state.AI_vehicles.opponents[i].pos_last_frame, 0.f, 0.f, 0.f);
        gOpponents[gProgram_state.AI_vehicles.opponents[i].index].psyche.grudge_against_player = 10;
        gViewable_car_list[gNum_viewable_cars] = gProgram_state.AI_vehicles.opponents[i].car_spec;
        gNum_viewable_cars++;
        StunTheBugger(&gProgram_state.AI_vehicles.opponents[i], 0xfffffff);
    }
    if (gChallenger_index__opponent >= 0) {
        car_spec = GetCarSpecFromGlobalOppoIndex(gChallenger_index__opponent);
        opponent_spec = GetOpponentSpecFromCarSpec(car_spec);
        if (opponent_spec == NULL) {
            DoNotDprintf_opponent("ERROR - can't record dare - no opponent_spec for car_spec");
        } else {
            opponent_spec->pursue_from_start = 1;
        }
        gChallenger_index__opponent = -1;
    }
    /* cops are not initialized in carpocalypse2 */

    opponents_left = NumberOfOpponentsStillRunning();
    if (opponents_left != 0) {
        if (gCurrent_race.race_spec->opponent_nastiness_level < 10.f) {
            int count_type_2;
            int count_type_1;

            for (i = 0; i < GetCarCount(eVehicle_opponent); i++) {
                gProgram_state.AI_vehicles.opponents[i].field_0xb4 = 0;
            }

            count_type_2 = (int)(0.6f * opponents_left);
            for (i = 0; i < count_type_2; i++) {
                int rstart;
                int r;

                rstart = IRandomBetween(0, opponents_left - 1 - i);
                for (r = rstart;;) {
                    if (gProgram_state.AI_vehicles.opponents[r].field_0xb4 == 0) {
                        gProgram_state.AI_vehicles.opponents[r].field_0xb4 = 2;
                        break;
                    }
                    r++;
                    if (r >= opponents_left) {
                        r = 0;
                    }
                    if (r == rstart) {
                        break;
                    }
                }
            }
            count_type_1 = (int)(0.2f * opponents_left);
            for (i = 0; i < count_type_1; i++) {
                int rstart;
                int r;

                rstart = IRandomBetween(0, opponents_left - count_type_2 - 1 - i);
                for (r = rstart;;) {
                    if (gProgram_state.AI_vehicles.opponents[r].field_0xb4 == 0) {
                        gProgram_state.AI_vehicles.opponents[r].field_0xb4 = 1;
                        break;
                    }
                    r++;
                    if (r >= opponents_left) {
                        r = 0;
                    }
                    if (r == rstart) {
                        break;
                    }
                }

            }
        } else {
            for (i = 0; i < GetCarCount(eVehicle_opponent); i++) {
                gProgram_state.AI_vehicles.opponents[i].field_0xb4 = 3;
            }
        }
    }

    gActive_car_list_rebuild_required = 1;
    RebuildActiveCarList();
}

// FUNCTION: CARMA2_HW 0x00401fe0
void C2_HOOK_FASTCALL CalcNegativeXVector(br_vector3* pNegative_x_vector, br_vector3* pStart, br_vector3* pFinish, br_scalar pLength) {
    br_vector3 positive_y_vector;
    br_vector3 path_vector;

    BrVector3Sub(&path_vector, pFinish, pStart);
    BrVector3Set(&positive_y_vector, 0.f, 1.f, 0.f);
    BrVector3Cross(pNegative_x_vector, &path_vector, &positive_y_vector);
    BrVector3Normalise(pNegative_x_vector, pNegative_x_vector);
    BrVector3Scale(pNegative_x_vector, pNegative_x_vector, pLength);
}

// GLOBAL: CARMA2_HW 0x00589318
const br_scalar gFLOAT_00589318 = -0.15f;

// GLOBAL: CARMA2_HW 0x0058931c
const br_scalar gFLOAT_0058931c = -0.1f;

// GLOBAL: CARMA2_HW 0x00589320
const br_scalar gFLOAT_00589320 = 0.1f;

// FUNCTION: CARMA2_HW 0x00401aa0
void C2_NAKED C2_HOOK_FASTCALL BuildDronePathNodeMesh(br_model* pModel, int pVert_index, int pFace_index, const br_vector3* pPosition, tU32 pMaterial1, tU32 pMaterial2, tU32 pMaterial3) {
#ifndef CARPOCALYPSE2_MATCHING
    tU32 uVar2;
    tU32 uVar3;
    float fVar5;
    int iVar1;
    int iVar6;
    int iVar7;
    int iVar8;
    int iVar9;
    int iVar10;
    int vertex1;
    int vertex2;
    int vertex3;
    int vertex4;
    int vertex5;
    int vertex6;
    int vertex7;
    tS16 sVar4;
    float* pfVar11;
    br_face* faces;

    uVar2 = *(tU32*)&pPosition->v[0];
    uVar3 = *(tU32*)&pPosition->v[2];
    fVar5 = pPosition->v[1] - gFLOAT_00589318;
    vertex1 = pVert_index + 1;
    vertex2 = pVert_index + 2;
    vertex3 = pVert_index + 3;
    vertex4 = pVert_index + 4;
    vertex5 = pVert_index + 5;
    vertex6 = pVert_index + 6;
    vertex7 = pVert_index + 7;
    iVar6 = pVert_index * 0x28;
    *(tU32*)((tU8*)pModel->vertices + iVar6) = uVar2;
    *(float*)((tU8*)pModel->vertices + iVar6 + 4) = fVar5;
    *(tU32*)((tU8*)pModel->vertices + iVar6 + 8) = uVar3;
    pfVar11 = (float*)((tU8*)pModel->vertices + iVar6);
    *pfVar11 = *pfVar11 - gFLOAT_0058931c;
    *(float*)((tU8*)pModel->vertices + iVar6 + 4) = *(float*)((tU8*)pModel->vertices + iVar6 + 4) - gFLOAT_0058931c;
    *(float*)((tU8*)pModel->vertices + iVar6 + 8) = *(float*)((tU8*)pModel->vertices + iVar6 + 8) - gFLOAT_0058931c;
    iVar6 = pVert_index + 1;
    iVar7 = iVar6 * 0x28;
    *(tU32*)((tU8*)pModel->vertices + iVar7) = uVar2;
    *(float*)((tU8*)pModel->vertices + iVar7 + 4) = fVar5;
    *(tU32*)((tU8*)pModel->vertices + iVar7 + 8) = uVar3;
    pfVar11 = (float*)((tU8*)pModel->vertices + iVar7);
    *pfVar11 = *pfVar11 - gFLOAT_0058931c;
    *(float*)((tU8*)pModel->vertices + iVar7 + 4) = *(float*)((tU8*)pModel->vertices + iVar7 + 4) - gFLOAT_00589320;
    *(float*)((tU8*)pModel->vertices + iVar7 + 8) = *(float*)((tU8*)pModel->vertices + iVar7 + 8) - gFLOAT_0058931c;
    iVar7 = pVert_index + 2;
    iVar8 = iVar7 * 0x28;
    *(tU32*)((tU8*)pModel->vertices + iVar8) = uVar2;
    *(float*)((tU8*)pModel->vertices + iVar8 + 4) = fVar5;
    *(tU32*)((tU8*)pModel->vertices + iVar8 + 8) = uVar3;
    pfVar11 = (float*)((tU8*)pModel->vertices + iVar8);
    *pfVar11 = *pfVar11 - gFLOAT_00589320;
    *(float*)((tU8*)pModel->vertices + iVar8 + 4) = *(float*)((tU8*)pModel->vertices + iVar8 + 4) - gFLOAT_00589320;
    *(float*)((tU8*)pModel->vertices + iVar8 + 8) = *(float*)((tU8*)pModel->vertices + iVar8 + 8) - gFLOAT_0058931c;
    iVar8 = (pVert_index + 3) * 0x28;
    *(tU32*)((tU8*)pModel->vertices + iVar8) = uVar2;
    *(float*)((tU8*)pModel->vertices + iVar8 + 4) = fVar5;
    *(tU32*)((tU8*)pModel->vertices + iVar8 + 8) = uVar3;
    pfVar11 = (float*)((tU8*)pModel->vertices + iVar8);
    *pfVar11 = *pfVar11 - gFLOAT_00589320;
    *(float*)((tU8*)pModel->vertices + iVar8 + 4) = *(float*)((tU8*)pModel->vertices + iVar8 + 4) - gFLOAT_0058931c;
    *(float*)((tU8*)pModel->vertices + iVar8 + 8) = *(float*)((tU8*)pModel->vertices + iVar8 + 8) - gFLOAT_0058931c;
    iVar8 = (pVert_index + 4) * 0x28;
    *(tU32*)((tU8*)pModel->vertices + iVar8) = uVar2;
    *(float*)((tU8*)pModel->vertices + iVar8 + 4) = fVar5;
    *(tU32*)((tU8*)pModel->vertices + iVar8 + 8) = uVar3;
    pfVar11 = (float*)((tU8*)pModel->vertices + iVar8);
    *pfVar11 = *pfVar11 - gFLOAT_0058931c;
    *(float*)((tU8*)pModel->vertices + iVar8 + 4) = *(float*)((tU8*)pModel->vertices + iVar8 + 4) - gFLOAT_0058931c;
    *(float*)((tU8*)pModel->vertices + iVar8 + 8) = *(float*)((tU8*)pModel->vertices + iVar8 + 8) - gFLOAT_00589320;
    iVar9 = pVert_index + 5;
    iVar9 = iVar9 * 0x28;
    *(tU32*)((tU8*)pModel->vertices + iVar9) = uVar2;
    *(float*)((tU8*)pModel->vertices + iVar9 + 4) = fVar5;
    *(tU32*)((tU8*)pModel->vertices + iVar9 + 8) = uVar3;
    pfVar11 = (float*)((tU8*)pModel->vertices + iVar9);
    *pfVar11 = *pfVar11 - gFLOAT_0058931c;
    *(float*)((tU8*)pModel->vertices + iVar9 + 4) = *(float*)((tU8*)pModel->vertices + iVar9 + 4) - gFLOAT_00589320;
    *(float*)((tU8*)pModel->vertices + iVar9 + 8) = *(float*)((tU8*)pModel->vertices + iVar9 + 8) - gFLOAT_00589320;
    iVar1 = pVert_index + 6;
    iVar9 = iVar1 * 0x28;
    *(tU32*)((tU8*)pModel->vertices + iVar9) = uVar2;
    *(float*)((tU8*)pModel->vertices + iVar9 + 4) = fVar5;
    *(tU32*)((tU8*)pModel->vertices + iVar9 + 8) = uVar3;
    pfVar11 = (float*)((tU8*)pModel->vertices + iVar9);
    *pfVar11 = *pfVar11 - gFLOAT_00589320;
    *(float*)((tU8*)pModel->vertices + iVar9 + 4) = *(float*)((tU8*)pModel->vertices + iVar9 + 4) - gFLOAT_00589320;
    *(float*)((tU8*)pModel->vertices + iVar9 + 8) = *(float*)((tU8*)pModel->vertices + iVar9 + 8) - gFLOAT_00589320;
    iVar9 = pVert_index + 7;
    iVar10 = iVar9 * 0x28;
    *(tU32*)((tU8*)pModel->vertices + iVar10) = uVar2;
    *(float*)((tU8*)pModel->vertices + iVar10 + 4) = fVar5;
    *(tU32*)((tU8*)pModel->vertices + iVar10 + 8) = uVar3;
    pfVar11 = (float*)((tU8*)pModel->vertices + iVar10);
    *pfVar11 = *pfVar11 - gFLOAT_00589320;
    *(float*)((tU8*)pModel->vertices + iVar10 + 4) = *(float*)((tU8*)pModel->vertices + iVar10 + 4) - gFLOAT_0058931c;
    *(float*)((tU8*)pModel->vertices + iVar10 + 8) = *(float*)((tU8*)pModel->vertices + iVar10 + 8) - gFLOAT_00589320;
    sVar4 = (tS16)pVert_index;
    iVar10 = pFace_index * 0x28;
    *(tS16*)((tU8*)pModel->faces + iVar10) = sVar4 + 3;
    *(tS16*)((tU8*)pModel->faces + iVar10 + 2) = sVar4 + 2;
    *(tS16*)((tU8*)pModel->faces + iVar10 + 4) = sVar4 + 1;
    *(tU16*)((tU8*)pModel->faces + iVar10 + 6) = 0xffff;
    *(tU32*)((tU8*)pModel->faces + iVar10 + 8) = pMaterial1;
    iVar10 = (pFace_index + 1) * 0x28;
    *(tS16*)((tU8*)pModel->faces + iVar10) = sVar4;
    *(tS16*)((tU8*)pModel->faces + iVar10 + 2) = sVar4 + 3;
    *(tS16*)((tU8*)pModel->faces + iVar10 + 4) = sVar4 + 1;
    *(tU16*)((tU8*)pModel->faces + iVar10 + 6) = 0xffff;
    *(tU32*)((tU8*)pModel->faces + iVar10 + 8) = pMaterial1;
    iVar10 = (pFace_index + 2) * 0x28;
    *(tS16*)((tU8*)pModel->faces + iVar10) = sVar4 + 1;
    *(tS16*)((tU8*)pModel->faces + iVar10 + 2) = sVar4 + 5;
    *(tS16*)((tU8*)pModel->faces + iVar10 + 4) = sVar4 + 4;
    *(tU16*)((tU8*)pModel->faces + iVar10 + 6) = 0xffff;
    *(tU32*)((tU8*)pModel->faces + iVar10 + 8) = pMaterial2;
    iVar10 = (pFace_index + 3) * 0x28;
    *(tS16*)((tU8*)pModel->faces + iVar10) = sVar4 + 1;
    *(tS16*)((tU8*)pModel->faces + iVar10 + 2) = sVar4 + 4;
    *(tS16*)((tU8*)pModel->faces + iVar10 + 4) = sVar4;
    *(tU16*)((tU8*)pModel->faces + iVar10 + 6) = 0xffff;
    *(tU32*)((tU8*)pModel->faces + iVar10 + 8) = pMaterial2;
    iVar10 = (pFace_index + 4) * 0x28;
    *(tS16*)((tU8*)pModel->faces + iVar10) = sVar4;
    *(tS16*)((tU8*)pModel->faces + iVar10 + 2) = sVar4 + 4;
    *(tS16*)((tU8*)pModel->faces + iVar10 + 4) = sVar4 + 3;
    *(tU16*)((tU8*)pModel->faces + iVar10 + 6) = 0xffff;
    *(tU32*)((tU8*)pModel->faces + iVar10 + 8) = pMaterial3;
    iVar10 = (pFace_index + 5) * 0x28;
    *(tS16*)((tU8*)pModel->faces + iVar10) = sVar4 + 3;
    *(tS16*)((tU8*)pModel->faces + iVar10 + 2) = sVar4 + 4;
    *(tS16*)((tU8*)pModel->faces + iVar10 + 4) = sVar4 + 7;
    *(tU16*)((tU8*)pModel->faces + iVar10 + 6) = 0xffff;
    *(tU32*)((tU8*)pModel->faces + iVar10 + 8) = pMaterial3;    MakeFaceAndTextureIt((int)pModel, pFace_index + 6, vertex4, vertex5, vertex7, pMaterial1);
    MakeFaceAndTextureIt((int)pModel, pFace_index + 7, vertex5, vertex6, vertex7, pMaterial1);
    MakeFaceAndTextureIt((int)pModel, pFace_index + 8, vertex2, vertex7, vertex6, pMaterial2);
    MakeFaceAndTextureIt((int)pModel, pFace_index + 9, vertex2, vertex3, vertex7, pMaterial2);
    MakeFaceAndTextureIt((int)pModel, pFace_index + 10, vertex1, vertex2, vertex6, pMaterial3);
    MakeFaceAndTextureIt((int)pModel, pFace_index + 11, vertex1, vertex6, vertex5, pMaterial3);
#else
    __asm {
            sub esp, 0x20
            mov eax, dword ptr [esp + 0x28]
            push ebx
            push ebp
            push esi
            fld dword ptr [eax]
            fld dword ptr [eax + 8]
            mov esi, ecx
            push edi
            fst dword ptr [esp + 0x2c]
            fld dword ptr [eax + 4]
            mov ecx, dword ptr [esi + 8]
            lea eax, [edx + edx*4]
            fsub dword ptr [gFLOAT_00589318]
            shl eax, 3
            fst dword ptr [esp + 0x28]
            fld st(2)
            fstp dword ptr [eax + ecx]
            mov ecx, dword ptr [esi + 8]
            fst dword ptr [eax + ecx + 4]
            mov ecx, dword ptr [esi + 8]
            fld st(1)
            fstp dword ptr [eax + ecx + 8]
            mov ecx, dword ptr [esi + 8]
            add ecx, eax
            fld dword ptr [ecx]
            fsub dword ptr [gFLOAT_0058931c]
            fstp dword ptr [ecx]
            mov ecx, dword ptr [esi + 8]
            fld dword ptr [eax + ecx + 4]
            fsub dword ptr [gFLOAT_0058931c]
            lea ecx, [eax + ecx + 4]
            fstp dword ptr [ecx]
            mov ecx, dword ptr [esi + 8]
            fld dword ptr [eax + ecx + 8]
            fsub dword ptr [gFLOAT_0058931c]
            lea eax, [eax + ecx + 8]
            fstp dword ptr [eax]
            mov ecx, dword ptr [esi + 8]
            lea eax, [edx + 1]
            mov dword ptr [esp + 0x20], eax
            lea eax, [eax + eax*4]
            shl eax, 3
            fld st(2)
            fstp dword ptr [eax + ecx]
            mov ecx, dword ptr [esi + 8]
            fst dword ptr [eax + ecx + 4]
            mov ecx, dword ptr [esi + 8]
            fld st(1)
            fstp dword ptr [eax + ecx + 8]
            mov ecx, dword ptr [esi + 8]
            add ecx, eax
            fld dword ptr [ecx]
            fsub dword ptr [gFLOAT_0058931c]
            fstp dword ptr [ecx]
            mov ecx, dword ptr [esi + 8]
            fld dword ptr [eax + ecx + 4]
            fsub dword ptr [gFLOAT_00589320]
            lea ecx, [eax + ecx + 4]
            fstp dword ptr [ecx]
            mov ecx, dword ptr [esi + 8]
            fld dword ptr [eax + ecx + 8]
            fsub dword ptr [gFLOAT_0058931c]
            lea eax, [eax + ecx + 8]
            fstp dword ptr [eax]
            mov ecx, dword ptr [esi + 8]
            lea eax, [edx + 2]
            mov dword ptr [esp + 0x10], eax
            lea eax, [eax + eax*4]
            shl eax, 3
            fld st(2)
            fstp dword ptr [eax + ecx]
            mov ecx, dword ptr [esi + 8]
            fst dword ptr [eax + ecx + 4]
            mov ecx, dword ptr [esi + 8]
            fld st(1)
            fstp dword ptr [eax + ecx + 8]
            mov ecx, dword ptr [esi + 8]
            add ecx, eax
            fld dword ptr [ecx]
            fsub dword ptr [gFLOAT_00589320]
            fstp dword ptr [ecx]
            mov ecx, dword ptr [esi + 8]
            fld dword ptr [eax + ecx + 4]
            fsub dword ptr [gFLOAT_00589320]
            lea ecx, [eax + ecx + 4]
            fstp dword ptr [ecx]
            mov ecx, dword ptr [esi + 8]
            fld dword ptr [eax + ecx + 8]
            fsub dword ptr [gFLOAT_0058931c]
            lea eax, [eax + ecx + 8]
            fstp dword ptr [eax]
            mov ecx, dword ptr [esi + 8]
            lea eax, [edx + 3]
            mov dword ptr [esp + 0x1c], eax
            lea eax, [eax + eax*4]
            shl eax, 3
            fld st(2)
            fstp dword ptr [eax + ecx]
            mov ecx, dword ptr [esi + 8]
            fst dword ptr [eax + ecx + 4]
            mov ecx, dword ptr [esi + 8]
            fld st(1)
            fstp dword ptr [eax + ecx + 8]
            mov ecx, dword ptr [esi + 8]
            add ecx, eax
            fld dword ptr [ecx]
            fsub dword ptr [gFLOAT_00589320]
            fstp dword ptr [ecx]
            mov ecx, dword ptr [esi + 8]
            fld dword ptr [eax + ecx + 4]
            fsub dword ptr [gFLOAT_0058931c]
            lea ecx, [eax + ecx + 4]
            fstp dword ptr [ecx]
            mov ecx, dword ptr [esi + 8]
            fld dword ptr [eax + ecx + 8]
            fsub dword ptr [gFLOAT_0058931c]
            lea eax, [eax + ecx + 8]
            fstp dword ptr [eax]
            mov ecx, dword ptr [esi + 8]
            lea eax, [edx + 4]
            mov dword ptr [esp + 0x18], eax
            lea eax, [eax + eax*4]
            shl eax, 3
            fld st(2)
            fstp dword ptr [eax + ecx]
            mov ecx, dword ptr [esi + 8]
            fst dword ptr [eax + ecx + 4]
            mov ecx, dword ptr [esi + 8]
            fld st(1)
            fstp dword ptr [eax + ecx + 8]
            mov ecx, dword ptr [esi + 8]
            add ecx, eax
            fld dword ptr [ecx]
            fsub dword ptr [gFLOAT_0058931c]
            fstp dword ptr [ecx]
            mov ecx, dword ptr [esi + 8]
            fld dword ptr [eax + ecx + 4]
            fsub dword ptr [gFLOAT_0058931c]
            lea ecx, [eax + ecx + 4]
            fstp dword ptr [ecx]
            mov ecx, dword ptr [esi + 8]
            fld dword ptr [eax + ecx + 8]
            fsub dword ptr [gFLOAT_00589320]
            lea eax, [eax + ecx + 8]
            fstp dword ptr [eax]
            mov ecx, dword ptr [esi + 8]
            lea eax, [edx + 5]
            mov dword ptr [esp + 0x38], eax
            lea eax, [eax + eax*4]
            shl eax, 3
            fld st(2)
            fstp dword ptr [eax + ecx]
            mov ecx, dword ptr [esi + 8]
            fst dword ptr [eax + ecx + 4]
            mov ecx, dword ptr [esi + 8]
            fld st(1)
            fstp dword ptr [eax + ecx + 8]
            mov ecx, dword ptr [esi + 8]
            add ecx, eax
            fld dword ptr [ecx]
            fsub dword ptr [gFLOAT_0058931c]
            fstp dword ptr [ecx]
            mov ecx, dword ptr [esi + 8]
            lea ebx, [edx + 6]
            mov edi, dword ptr [esp + 0x34]
            fld dword ptr [eax + ecx + 4]
            fsub dword ptr [gFLOAT_00589320]
            lea ecx, [eax + ecx + 4]
            fstp dword ptr [ecx]
            mov ecx, dword ptr [esi + 8]
            fld dword ptr [eax + ecx + 8]
            fsub dword ptr [gFLOAT_00589320]
            lea eax, [eax + ecx + 8]
            fstp dword ptr [eax]
            mov ecx, dword ptr [esi + 8]
            lea eax, [ebx + ebx*4]
            shl eax, 3
            fld st(2)
            fstp dword ptr [eax + ecx]
            mov ecx, dword ptr [esi + 8]
            fstp dword ptr [eax + ecx + 4]
            mov ecx, dword ptr [esi + 8]
            fstp dword ptr [eax + ecx + 8]
            mov ecx, dword ptr [esi + 8]
            add ecx, eax
            fld dword ptr [ecx]
            fsub dword ptr [gFLOAT_00589320]
            fstp dword ptr [ecx]
            mov ecx, dword ptr [esi + 8]
            fld dword ptr [eax + ecx + 4]
            fsub dword ptr [gFLOAT_00589320]
            lea ecx, [eax + ecx + 4]
            fstp dword ptr [ecx]
            mov ecx, dword ptr [esi + 8]
            fld dword ptr [eax + ecx + 8]
            fsub dword ptr [gFLOAT_00589320]
            lea eax, [eax + ecx + 8]
            fstp dword ptr [eax]
            mov ecx, dword ptr [esi + 8]
            lea eax, [edx + 7]
            mov dword ptr [esp + 0x14], eax
            lea eax, [eax + eax*4]
            shl eax, 3
            fstp dword ptr [eax + ecx]
            mov ecx, dword ptr [esi + 8]
            fld dword ptr [esp + 0x28]
            fstp dword ptr [eax + ecx + 4]
            mov ecx, dword ptr [esi + 8]
            fld dword ptr [esp + 0x2c]
            fstp dword ptr [eax + ecx + 8]
            mov ecx, dword ptr [esi + 8]
            add ecx, eax
            fld dword ptr [ecx]
            fsub dword ptr [gFLOAT_00589320]
            fstp dword ptr [ecx]
            mov ecx, dword ptr [esi + 8]
            fld dword ptr [eax + ecx + 4]
            fsub dword ptr [gFLOAT_0058931c]
            lea ecx, [eax + ecx + 4]
            fstp dword ptr [ecx]
            mov ecx, dword ptr [esi + 8]
            fld dword ptr [eax + ecx + 8]
            fsub dword ptr [gFLOAT_00589320]
            lea eax, [eax + ecx + 8]
            lea ecx, [edx + 3]
            fstp dword ptr [eax]
            mov ebp, dword ptr [esi + 0xc]
            lea eax, [edi + edi*4]
            shl eax, 3
            mov word ptr [eax + ebp], cx
            mov ebp, dword ptr [esi + 0xc]
            lea ecx, [edx + 2]
            mov word ptr [eax + ebp + 2], cx
            mov ebp, dword ptr [esi + 0xc]
            lea ecx, [edx + 1]
            mov word ptr [eax + ebp + 4], cx
            mov ecx, dword ptr [esi + 0xc]
            mov ebp, dword ptr [esp + 0x3c]
            mov word ptr [eax + ecx + 6], 0xffff
            mov ecx, dword ptr [esi + 0xc]
            mov dword ptr [eax + ecx + 8], ebp
            lea eax, [edi + 1]
            mov ecx, dword ptr [esi + 0xc]
            lea eax, [eax + eax*4]
            shl eax, 3
            mov word ptr [eax + ecx], dx
            mov ebp, dword ptr [esi + 0xc]
            lea ecx, [edx + 3]
            mov word ptr [eax + ebp + 2], cx
            mov ebp, dword ptr [esi + 0xc]
            lea ecx, [edx + 1]
            mov word ptr [eax + ebp + 4], cx
            mov ecx, dword ptr [esi + 0xc]
            mov ebp, dword ptr [esp + 0x3c]
            mov word ptr [eax + ecx + 6], 0xffff
            mov ecx, dword ptr [esi + 0xc]
            mov dword ptr [eax + ecx + 8], ebp
            mov ebp, dword ptr [esi + 0xc]
            lea eax, [edi + 2]
            lea ecx, [edx + 1]
            lea eax, [eax + eax*4]
            shl eax, 3
            mov word ptr [eax + ebp], cx
            mov ebp, dword ptr [esi + 0xc]
            lea ecx, [edx + 5]
            mov word ptr [eax + ebp + 2], cx
            mov ebp, dword ptr [esi + 0xc]
            lea ecx, [edx + 4]
            mov word ptr [eax + ebp + 4], cx
            mov ecx, dword ptr [esi + 0xc]
            mov ebp, dword ptr [esp + 0x40]
            mov word ptr [eax + ecx + 6], 0xffff
            mov ecx, dword ptr [esi + 0xc]
            mov dword ptr [eax + ecx + 8], ebp
            mov ebp, dword ptr [esi + 0xc]
            lea eax, [edi + 3]
            lea ecx, [edx + 1]
            lea eax, [eax + eax*4]
            shl eax, 3
            mov word ptr [eax + ebp], cx
            mov ebp, dword ptr [esi + 0xc]
            lea ecx, [edx + 4]
            mov word ptr [eax + ebp + 2], cx
            mov ecx, dword ptr [esi + 0xc]
            mov ebp, dword ptr [esp + 0x40]
            mov word ptr [eax + ecx + 4], dx
            mov ecx, dword ptr [esi + 0xc]
            mov word ptr [eax + ecx + 6], 0xffff
            mov ecx, dword ptr [esi + 0xc]
            mov dword ptr [eax + ecx + 8], ebp
            mov ecx, dword ptr [esi + 0xc]
            lea eax, [edi + 4]
            lea eax, [eax + eax*4]
            shl eax, 3
            mov word ptr [eax + ecx], dx
            mov ebp, dword ptr [esi + 0xc]
            lea ecx, [edx + 4]
            mov word ptr [eax + ebp + 2], cx
            mov ebp, dword ptr [esi + 0xc]
            lea ecx, [edx + 3]
            mov word ptr [eax + ebp + 4], cx
            mov ecx, dword ptr [esi + 0xc]
            mov ebp, dword ptr [esp + 0x44]
            mov word ptr [eax + ecx + 6], 0xffff
            mov ecx, dword ptr [esi + 0xc]
            mov dword ptr [eax + ecx + 8], ebp
            mov ebp, dword ptr [esi + 0xc]
            lea eax, [edi + 5]
            lea ecx, [edx + 3]
            lea eax, [eax + eax*4]
            shl eax, 3
            mov word ptr [eax + ebp], cx
            mov ebp, dword ptr [esi + 0xc]
            lea ecx, [edx + 4]
            add edx, 7
            mov word ptr [eax + ebp + 2], cx
            mov ecx, dword ptr [esi + 0xc]
            mov word ptr [eax + ecx + 4], dx
            mov edx, dword ptr [esi + 0xc]
            mov word ptr [eax + edx + 6], 0xffff
            mov ecx, dword ptr [esi + 0xc]
            mov edx, dword ptr [esp + 0x44]
            mov dword ptr [eax + ecx + 8], edx
            mov eax, dword ptr [esp + 0x3c]
            mov ebp, dword ptr [esp + 0x14]
            mov ecx, dword ptr [esp + 0x38]
            mov edx, dword ptr [esp + 0x18]
            push eax
            push ebp
            push ecx
            push edx
            lea edx, [edi + 6]
            mov ecx, esi
            call MakeFaceAndTextureIt
            lea edx, [edi + 7]
            mov eax, dword ptr [esp + 0x3c]
            mov ecx, dword ptr [esp + 0x38]
            push eax
            push ebp
            push ebx
            push ecx
            mov ecx, esi
            call MakeFaceAndTextureIt
            mov ecx, esi
            mov edx, dword ptr [esp + 0x40]
            mov eax, dword ptr [esp + 0x10]
            push edx
            push ebx
            push ebp
            push eax
            lea edx, [edi + 8]
            call MakeFaceAndTextureIt
            mov ecx, dword ptr [esp + 0x40]
            mov edx, dword ptr [esp + 0x1c]
            push ecx
            push ebp
            mov ebp, dword ptr [esp + 0x18]
            push edx
            push ebp
            lea edx, [edi + 9]
            mov ecx, esi
            call MakeFaceAndTextureIt
            lea edx, [edi + 0xa]
            mov ecx, esi
            mov eax, dword ptr [esp + 0x44]
            push eax
            push ebx
            push ebp
            mov ebp, dword ptr [esp + 0x2c]
            push ebp
            call MakeFaceAndTextureIt
            mov ecx, dword ptr [esp + 0x44]
            mov edx, dword ptr [esp + 0x38]
            push ecx
            push edx
            push ebx
            push ebp
            lea edx, [edi + 0xb]
            mov ecx, esi
            call MakeFaceAndTextureIt
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x20
            ret 0x14
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004a9550
void C2_HOOK_FASTCALL CalcOpponentConspicuousnessWithAViewToCheatingLikeFuck(tOpponent_spec* pOpponent_spec) {
    BrVector3Sub(&pOpponent_spec->player_to_oppo_v, &pOpponent_spec->car_spec->car_master_actor->t.t.translate.t, &gProgram_state.current_car.car_master_actor->t.t.translate.t);
    pOpponent_spec->player_to_oppo_d = BrVector3Length(&pOpponent_spec->player_to_oppo_v);
    if (pOpponent_spec->player_to_oppo_d < gIn_view_distance) {
        pOpponent_spec->last_in_view = gTime_stamp_for_this_munging;
    }
}

// FUNCTION: CARMA2_HW 0x004a7960
void C2_HOOK_FASTCALL TurnOpponentPhysicsOn(tOpponent_spec* pOpponent_spec) {
    pOpponent_spec->car_spec->car_master_actor->render_style = BR_RSTYLE_DEFAULT;
    PipeSingleOppoRenderage(pOpponent_spec, 1);
    if (!pOpponent_spec->physics_me) {
        pOpponent_spec->physics_me = 1;
        gActive_car_list_rebuild_required = 1;
    }
}

// FUNCTION: CARMA2_HW 0x004a79a0
void C2_HOOK_FASTCALL TurnOpponentPhysicsOff(tOpponent_spec* pOpponent_spec) {
    tCar_spec* car = pOpponent_spec->car_spec;

    car->acc_force = 0.f;
    car->brake_force = 0.f;
    car->curvature = 0.f;
    car->gear = 0;
    car->revs = 0.f;
    BrVector3Set(&car->collision_info->omega, 0.f, 0.f, 0.f);
    BrVector3Set(&car->collision_info->v, 0.f, 0.f, 0.f);
    if (pOpponent_spec->physics_me) {
        pOpponent_spec->physics_me = 0;
        pOpponent_spec->car_spec->car_master_actor->render_style = BR_RSTYLE_NONE;
        PipeSingleOppoRenderage(pOpponent_spec, 0);
        gActive_car_list_rebuild_required = 1;
    }
}

// FUNCTION: CARMA2_HW 0x004af8d0
int C2_HOOK_FASTCALL TimeToStopStruggling(tOpponent_spec* pOpponent_spec) {

    if (gTime_stamp_for_this_munging >
        (float)(750 * (pOpponent_spec->follow_path_data.number_of_struggles - 1))
            - pOpponent_spec->car_spec->collision_info->M * -30.f
            + (float)(pOpponent_spec->follow_path_data.struggle_time + 2750)) {
        return 1;
    } else {
        return 0;
    }
}

void C2_HOOK_FASTCALL DisplayOpponentRecoveringHeadup(tOpponent_spec* pOpponent_spec) {

    // empty
}

void C2_HOOK_FASTCALL OiStopCheating(tOpponent_spec* pOpponent_spec) {

    DoNotDprintf_opponent("%s: OiStopCheating() - End of cheating sesh", pOpponent_spec->car_spec->driver_name);
    pOpponent_spec->cheating = 0;
    if (CAR_SPEC_IS_ROZZER(pOpponent_spec->car_spec)) {
        DoNotDprintf_opponent ("%s: OiStopCheating() - Turning physics ON", pOpponent_spec->car_spec->driver_name);
        TurnOpponentPhysicsOn(pOpponent_spec);
        RebuildActiveCarList();
    }
}

void C2_HOOK_FASTCALL StartToCheat(tOpponent_spec* pOpponent_spec) {

    if (CAR_SPEC_IS_ROZZER(pOpponent_spec->car_spec)) {
        DoNotDprintf_opponent("%s: StartToCheat() - Turning physics OFF", pOpponent_spec->car_spec->driver_name);
        TurnOpponentPhysicsOff(pOpponent_spec);
        RebuildActiveCarList();
    } else {
        DoNotDprintf_opponent("%s: StartToCheat() - Starting to cheat", pOpponent_spec->car_spec->driver_name);
        pOpponent_spec->cheating = 1;
        switch (pOpponent_spec->current_objective) {
        case eOOT_levitate:
        case eOOT_knackered_and_freewheeling:
        case eOOT_none:
            break;
        default:
            if (!pOpponent_spec->car_spec->knackered
                    && ((pOpponent_spec->car_spec->collision_info->last_special_volume != NULL && pOpponent_spec->car_spec->collision_info->last_special_volume->gravity_multiplier < 1.f)
                        || !pOpponent_spec->follow_path_data.has_moved_during_this_task
                        || (pOpponent_spec->follow_path_data.struggle_time != 0 && !TimeToStopStruggling(pOpponent_spec)))) {

                DisplayOpponentRecoveringHeadup(pOpponent_spec);
            }
            break;
        }

        switch (pOpponent_spec->current_objective) {
        case eOOT_complete_race:
        case eOOT_pursue_and_twat:
        case eOOT_run_away:
        case eOOT_get_near_player:
        case eOOT_return_to_start:
            if (pOpponent_spec->follow_path_data.section_no < 15000
                    || pOpponent_spec->follow_path_data.section_no >= 20000) {
                int section = pOpponent_spec->follow_path_data.section_no - 20000;
                if (section >= 0 && section < pOpponent_spec->nnext_sections - 1) {
                    br_vector3 direction_v;
                    br_vector3 intersect;
                    br_scalar distance;

                    tS16 nearest_section = FindNearestPathSection(&pOpponent_spec->car_spec->car_master_actor->t.t.translate.t, &direction_v, &intersect, &distance);
                    tS16 real_section = GetOpponentsRealSection(pOpponent_spec, pOpponent_spec->follow_path_data.section_no);

                    if (nearest_section != real_section) {
                        int i;

                        for (i = section; i < pOpponent_spec->nnext_sections; i++) {
                            if (pOpponent_spec->next_sections[i].section_no == nearest_section) {
                                break;
                            }
                        }
                        if (i < pOpponent_spec->nnext_sections) {
                            ShiftOpponentsProjectedRoute(pOpponent_spec, i);
                            pOpponent_spec->follow_path_data.section_no = 20000;
                        } else {
                            br_vector3 dir;

                            pOpponent_spec->follow_path_data.section_no = 20000;
                            pOpponent_spec->nnext_sections = 1;
                            pOpponent_spec->next_sections[0].section_no = nearest_section;
                            BrVector3Sub(&dir, &intersect, &pOpponent_spec->car_spec->car_master_actor->t.t.translate.t);
                            pOpponent_spec->next_sections[0].direction = BrVector3Dot(&dir, (br_vector3*)pOpponent_spec->car_spec->car_master_actor->t.t.mat.m[2]) >= 0;
                        }
                    }
                }
            }
            break;
        default:
            break;
        }
    }
}

// FUNCTION: CARMA2_HW 0x004aa570
void C2_NAKED C2_HOOK_FASTCALL ProcessThisOpponent(tOpponent_spec* pOpponent_spec) {
#ifndef CARPOCALYPSE2_MATCHING

    if ((gMap_view == 2 && gShow_opponents) || gTime_stamp_for_this_munging <= pOpponent_spec->last_in_view + 3000) {
        if (pOpponent_spec->cheating) {
            OiStopCheating(pOpponent_spec);
        }
    } else {
        if (!pOpponent_spec->cheating) {
            StartToCheat(pOpponent_spec);
        }
    }
    ChooseNewObjective(pOpponent_spec, pOpponent_spec->new_objective_required);
    pOpponent_spec->new_objective_required = 0;
    if (gCountdown != 0 || gRace_finished) {
        pOpponent_spec->car_spec->brake_force = 10.f * pOpponent_spec->car_spec->collision_info->M;
    }
    if (!pOpponent_spec->finished_for_this_race && !gStop_opponents_moving && !gRace_finished && pOpponent_spec->stun_time_ends < gTime_stamp_for_this_munging) {
        ProcessCurrentObjective(pOpponent_spec, ePOC_run);
    }
    if (pOpponent_spec->cheating) {
        BrVector3Copy(&pOpponent_spec->car_spec->pos, &pOpponent_spec->car_spec->car_master_actor->t.t.translate.t);
    }
#else
    __asm {
            mov eax, dword ptr [gMap_view]
            sub esp, 0x1c
            cmp eax, 2
            push ebp
            push esi
            push edi
            mov esi, ecx
            jne L004aa58f
            mov eax, dword ptr [gShow_opponents]
            test eax, eax
            jne L004aa79d
    L004aa58f:
            mov eax, dword ptr [esi + 0x68]
            mov ecx, dword ptr [gTime_stamp_for_this_munging]
            add eax, 0xbb8
            cmp ecx, eax
            jbe L004aa79d
            test byte ptr [esi + 0xb0], 0x20
            jne L004aa831
            mov eax, dword ptr [esi + 8]
            xor ecx, ecx
            mov cx, word ptr [eax + 0x80]
            and cl, 0
            cmp ecx, 0x300
            je L004aa77e
            add eax, 0xec
            push eax
            push offset gOpponent_nastyness_frigger+0x4a0
            call DoNotDprintf_opponent
            mov edx, dword ptr [esi + 0xb0]
            mov eax, dword ptr [esi + 4]
            add esp, 8
            or edx, 0x20
            cmp eax, 5
            mov dword ptr [esi + 0xb0], edx
            je L004aa649
            cmp eax, 6
            je L004aa649
            test eax, eax
            je L004aa649
            mov eax, dword ptr [esi + 8]
            mov ecx, dword ptr [eax + 0x1d4]
            test ecx, ecx
            jne L004aa649
            mov edx, dword ptr [eax + 8]
            mov eax, dword ptr [edx + 0x258]
            test eax, eax
            je L004aa62d
            fld dword ptr [eax + 0x88]
            fcomp dword ptr [gOppo_pursuit_one]
            fnstsw ax
            test ah, 1
            jne L004aa649
    L004aa62d:
            mov eax, dword ptr [esi + 0xf0]
            test ah, 1
            je L004aa649
            mov eax, dword ptr [esi + 0xbc]
            test eax, eax
            je L004aa649
            mov ecx, esi
            call TimeToStopStruggling
    L004aa649:
            mov eax, dword ptr [esi + 4]
            cmp eax, 1
            je L004aa669
            cmp eax, 2
            je L004aa669
            cmp eax, 3
            je L004aa669
            cmp eax, 4
            je L004aa669
            cmp eax, 9
            jne L004aa831
    L004aa669:
            mov ax, word ptr [esi + 0xe8]
            cmp ax, 0x3a98
            jl L004aa680
            cmp ax, 0x4e20
            jl L004aa831
    L004aa680:
            movsx edi, ax
            sub edi, 0x4e20
            js L004aa831
            mov eax, dword ptr [esi + 0x7c]
            dec eax
            cmp edi, eax
            jge L004aa831
            lea ecx, [esp + 0xc]
            lea edx, [esp + 0x10]
            push ecx
            mov ecx, dword ptr [esi + 8]
            push edx
            lea eax, [esp + 0x24]
            mov edx, dword ptr [ecx + 0x10]
            push eax
            add edx, 0x50
            xor ecx, ecx
            call FindNearestGeneralSection
            movsx edx, word ptr [esi + 0xe8]
            mov ecx, esi
            mov ebp, eax
            call GetOpponentsRealSection
            movsx edx, bp
            cmp edx, eax
            je L004aa831
            mov ecx, dword ptr [esi + 0x7c]
            cmp edi, ecx
            jge L004aa70b
            lea eax, [esi + edi*4 + 0x80]
    L004aa6e3:
            cmp word ptr [eax], bp
            je L004aa6f0
            inc edi
            add eax, 4
            cmp edi, ecx
            jl L004aa6e3
    L004aa6f0:
            cmp edi, ecx
            jge L004aa70b
            mov edx, edi
            mov ecx, esi
            call ShiftOpponentsProjectedRoute
            mov word ptr [esi + 0xe8], 0x4e20
            jmp L004aa831
    L004aa70b:
            mov eax, dword ptr [esi + 8]
            mov word ptr [esi + 0xe8], 0x4e20
            fld dword ptr [esp + 0x18]
            fld dword ptr [esp + 0x10]
            mov dword ptr [esi + 0x7c], 1
            mov word ptr [esi + 0x80], bp
            mov eax, dword ptr [eax + 0x10]
            fsub dword ptr [eax + 0x50]
            fxch st(1)
            fsub dword ptr [eax + 0x58]
            fld dword ptr [eax + 0x44]
            fld dword ptr [eax + 0x4c]
            fld dword ptr [esp + 0x14]
            fsub dword ptr [eax + 0x54]
            fxch st(2)
            fmulp st(4), st(0)
            fmulp st(2), st(0)
            fxch st(1)
            faddp st(2), st(0)
            fmul dword ptr [eax + 0x48]
            faddp st(1), st(0)
            fcomp dword ptr [gOppo_render_zero]
            fnstsw ax
            test ah, 1
            jne L004aa771
            mov eax, 1
            mov byte ptr [esi + 0x82], al
            jmp L004aa831
    L004aa771:
            xor eax, eax
            mov byte ptr [esi + 0x82], al
            jmp L004aa831
    L004aa77e:
            add eax, 0xec
            push eax
            push offset gOpponent_nastyness_frigger+0x474
            call DoNotDprintf_opponent
            add esp, 8
            mov ecx, esi
            call TurnOpponentPhysicsOff
            jmp L004aa82c
    L004aa79d:
            test byte ptr [esi + 0xb0], 0x20
            je L004aa831
            mov ecx, dword ptr [esi + 8]
            add ecx, 0xec
            push ecx
            push offset gOpponent_nastyness_frigger+0x448
            call DoNotDprintf_opponent
            mov ebp, dword ptr [esi + 0xb0]
            mov eax, dword ptr [esi + 8]
            and ebp, 0xffffffdf
            xor edx, edx
            mov dword ptr [esi + 0xb0], ebp
            mov dx, word ptr [eax + 0x80]
            and dl, 0
            add esp, 8
            cmp edx, 0x300
            jne L004aa831
            add eax, 0xec
            push eax
            push offset gOpponent_nastyness_frigger+0x41c
            call DoNotDprintf_opponent
            mov eax, dword ptr [esi + 8]
            add esp, 8
            mov edx, 1
            mov ecx, dword ptr [eax + 0x10]
            mov byte ptr [ecx + 0x20], 0
            mov ecx, esi
            call PipeSingleOppoRenderage
            mov eax, dword ptr [esi + 0xb0]
            test al, 8
            jne L004aa82c
            or al, 8
            mov dword ptr [esi + 0xb0], eax
            mov dword ptr [gActive_car_list_rebuild_required], 1
    L004aa82c:
            call RebuildActiveCarList
    L004aa831:
            mov ecx, esi
            call PossiblyRepairOpponent
            test byte ptr [esi + 0xb0], 1
            jne L004aa852
            mov edx, dword ptr [gTime_stamp_for_this_munging]
            mov eax, dword ptr [esi + 0x74]
            cmp edx, eax
            ja L004aa852
            xor edx, edx
            jmp L004aa857
    L004aa852:
            mov edx, 1
    L004aa857:
            mov ecx, esi
            call ChooseNewObjective
            mov ecx, dword ptr [esi + 0xb0]
            and ecx, 0xfffffffe
            mov dword ptr [esi + 0xb0], ecx
            mov eax, dword ptr [gCountdown]
            test eax, eax
            jne L004aa87f
            mov eax, dword ptr [gRace_finished]
            test eax, eax
            je L004aa894
    L004aa87f:
            mov eax, dword ptr [esi + 8]
            mov ecx, dword ptr [eax + 8]
            fld dword ptr [ecx + 4]
            fmul dword ptr [DAT_0058a4ec]
            fstp dword ptr [eax + 0x12c0]
    L004aa894:
            test byte ptr [esi + 0xb0], 2
            jne L004aa8c8
            mov eax, dword ptr [gStop_opponents_moving]
            test eax, eax
            jne L004aa8c8
            mov eax, dword ptr [gRace_finished]
            test eax, eax
            jne L004aa8c8
            mov edx, dword ptr [gTime_stamp_for_this_munging]
            mov eax, dword ptr [esi + 0x5c]
            cmp edx, eax
            jbe L004aa8c8
            mov edx, 1
            mov ecx, esi
            call ProcessCurrentObjective
    L004aa8c8:
            test byte ptr [esi + 0xb0], 0x20
            je L004aa8f5
            mov eax, dword ptr [esi + 8]
            mov ecx, dword ptr [eax + 0x10]
            mov edx, dword ptr [ecx + 0x50]
            mov dword ptr [eax + 0x4c], edx
            mov eax, dword ptr [esi + 8]
            mov ecx, dword ptr [eax + 0x10]
            mov edx, dword ptr [ecx + 0x54]
            mov dword ptr [eax + 0x50], edx
            mov esi, dword ptr [esi + 8]
            mov eax, dword ptr [esi + 0x10]
            mov ecx, dword ptr [eax + 0x58]
            mov dword ptr [esi + 0x54], ecx
    L004aa8f5:
            pop edi
            pop esi
            pop ebp
            add esp, 0x1c
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004aa490
void C2_HOOK_FASTCALL StartRecordingTrail(tCar_spec* pPursuee) {

    if (pPursuee->no_of_processes_recording_my_trail == 0) {
        DoNotDprintf_opponent("StartRecordingTrail - starting from scratch");
        pPursuee->no_of_processes_recording_my_trail = 1;
        pPursuee->my_trail.nodes_shifted_this_frame = 0;
        pPursuee->my_trail.has_deviated_recently = 0;
        pPursuee->my_trail.number_of_nodes = 2;
        pPursuee->my_trail.time_of_next_recording = gTime_stamp_for_this_munging + 500;
        BrVector3Copy(&pPursuee->my_trail.base_heading, &pPursuee->direction);
        BrVector3Copy(&pPursuee->my_trail.trail_nodes[0], &pPursuee->car_master_actor->t.t.translate.t);
        BrVector3Copy(&pPursuee->my_trail.trail_nodes[1], &pPursuee->car_master_actor->t.t.translate.t);
        pPursuee->my_trail.trail_nodes[0].v[2] += 0.2f;
    } else {
        DoNotDprintf_opponent("StartRecordingTrail - another pursuer attaching");
        pPursuee->no_of_processes_recording_my_trail += 1;
    }
}

void C2_HOOK_FASTCALL RecordNextTrailNode(tCar_spec* pPursuee) {
    tPursuee_trail* trail;
    br_scalar length;
    br_vector3 car_to_last_point_v;
    br_vector3 offset_v;
    br_vector3 start1;
    br_vector3 finish1;
    br_vector3 start2;
    br_vector3 finish2;
    int visible;

    trail = &pPursuee->my_trail;
    trail->nodes_shifted_this_frame = 0;
    if (trail->time_of_next_recording >= gTime_stamp_for_this_munging) {
        return;
    }
    trail->time_of_next_recording = gTime_stamp_for_this_munging + 500;
    if (BrVector3Dot(&pPursuee->direction, &trail->base_heading) < FastScalarCos(30)) {
        trail->has_deviated_recently = 1;
    }
    BrVector3Sub(&car_to_last_point_v, &trail->trail_nodes[trail->number_of_nodes - 2], &pPursuee->car_master_actor->t.t.translate.t);
    length = BrVector3Length(&car_to_last_point_v);
    if (length < .3f) {
        return;
    }
    CalcNegativeXVector(&offset_v, &trail->trail_nodes[trail->number_of_nodes - 2], &pPursuee->car_master_actor->t.t.translate.t, .5f);

    BrVector3Add(&start1, &trail->trail_nodes[trail->number_of_nodes - 2], &offset_v);
    BrVector3Add(&finish1, &pPursuee->car_master_actor->t.t.translate.t, &offset_v);
    BrVector3Sub(&start2, &trail->trail_nodes[trail->number_of_nodes - 2], &offset_v);
    BrVector3Sub(&finish2, &pPursuee->car_master_actor->t.t.translate.t, &offset_v);
    visible = 1;
    if (!pPursuee->my_trail.has_deviated_recently) {
        visible = PointVisibleFromHere(&start1, &finish1);
        if (!visible) {
            visible = PointVisibleFromHere(&start2, &finish2);
        }
        if (!visible) {
            visible = PointVisibleFromHere(&trail->trail_nodes[trail->number_of_nodes - 2], &pPursuee->car_master_actor->t.t.translate.t);
        }
    }

    C2_HOOK_BUG_ON(sizeof(pPursuee->my_trail.trail_nodes) != 0x12c);

    if (pPursuee->my_trail.has_deviated_recently || !visible) {
        if ((visible && length > 2.0f) || (!visible && length > 1.5f)) {
            if (pPursuee->my_trail.number_of_nodes < CARPOCALYPSE2_ASIZE(pPursuee->my_trail.trail_nodes)) {
                pPursuee->my_trail.number_of_nodes += 1;
            } else {
                memmove(&pPursuee->my_trail.trail_nodes[0], &pPursuee->my_trail.trail_nodes[1], sizeof(pPursuee->my_trail.trail_nodes[0]));
                pPursuee->my_trail.nodes_shifted_this_frame = 1;
            }
            pPursuee->my_trail.has_deviated_recently = 0;
            BrVector3Copy(&trail->base_heading, &pPursuee->direction);
        }
    }
    BrVector3Copy(&trail->trail_nodes[trail->number_of_nodes - 1], &pPursuee->car_master_actor->t.t.translate.t);
}

void C2_HOOK_FASTCALL ClearTwattageOccurrenceVariables(tOpponent_spec* pOpponent_spec) {

    pOpponent_spec->car_spec->big_bang = 0;
    pOpponent_spec->car_spec->scary_bang = 0;
    pOpponent_spec->car_spec->grudge_raised_recently = 0;
    pOpponent_spec->car_spec->last_person_to_hit_us = NULL;
    pOpponent_spec->car_spec->last_person_we_hit = NULL;
}

// GLOBAL: CARMA2_HW 0x0058a4fc
static const float gTrail_minimum_distance = .3f;

// GLOBAL: CARMA2_HW 0x0058a500
static const float gTrail_obstructed_distance = 1.5f;

// FUNCTION: CARMA2_HW 0x004a9e10
void C2_NAKED C2_HOOK_FASTCALL MungeOpponents(void) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;
    int un_stun_flag;

    un_stun_flag = 0;
    if (gProgram_state.AI_vehicles.number_of_opponents == 0 && gNumber_of_cops_before_faffage == 0) {
        return;
    }
    if (gProgram_state.AI_vehicles.number_of_path_nodes == 0) {
        return;
    }
    if (gProgram_state.AI_vehicles.number_of_path_sections == 0) {
        return;
    }
    gAcme_frame_count += 1;
    if (!gAcknowledged_start && gCountdown == 0) {
        gAcknowledged_start = 1;
        if (!gStart_jumped) {
            un_stun_flag = 1;
        }
    }
    if (gProgram_state.current_car.no_of_processes_recording_my_trail == 0) {
        StartRecordingTrail(&gProgram_state.current_car);
    } else {
        RecordNextTrailNode(&gProgram_state.current_car);
    }
    gNum_of_opponents_pursuing = 0;
    gNum_of_opponents_getting_near = 0;
    gNum_of_opponents_completing_race = 0;
    for (i = 0; i < gProgram_state.AI_vehicles.number_of_opponents; i++) {
        tOpponent_spec* oppo = &gProgram_state.AI_vehicles.opponents[i];

        if (!oppo->finished_for_this_race) {
            switch (oppo->current_objective) {
            case eOOT_pursue_and_twat:
                gNum_of_opponents_pursuing += 1;
                break;
            case eOOT_get_near_player:
                gNum_of_opponents_getting_near += 1;
                break;
            case eOOT_complete_race:
                gNum_of_opponents_completing_race += 1;
                break;
            default:
                break;
            }
        }
    }
    for (i = 0; i < gProgram_state.AI_vehicles.number_of_opponents; i++) {
        tOpponent_spec* oppo = &gProgram_state.AI_vehicles.opponents[i];

        if (!oppo->finished_for_this_race) {
            if (un_stun_flag) {
                UnStunTheBugger(oppo);
            }
            CalcOpponentConspicuousnessWithAViewToCheatingLikeFuck(oppo);
            CalcPlayerConspicuousness(oppo);
            ProcessThisOpponent(oppo);
            ClearTwattageOccurrenceVariables(oppo);
        }
    }
    for (i = 0; i < gNumber_of_cops_before_faffage; i++) {
        tOpponent_spec* oppo = &gProgram_state.AI_vehicles.cops[i];

        if (!oppo->finished_for_this_race) {
            if (un_stun_flag) {
                UnStunTheBugger(oppo);
            }
            CalcOpponentConspicuousnessWithAViewToCheatingLikeFuck(oppo);
            CalcPlayerConspicuousness(oppo);
            ProcessThisOpponent(oppo);
            ClearTwattageOccurrenceVariables(oppo);
            oppo->murder_reported = 0;
        }
    }
    if (gNext_grudge_reduction < gTime_stamp_for_this_munging) {
        gNext_grudge_reduction = gTime_stamp_for_this_munging + 5000;

        for (i = 0; i < gProgram_state.AI_vehicles.number_of_opponents; i++) {
            tOpponent_spec* oppo = &gProgram_state.AI_vehicles.opponents[i];

            if (!oppo->finished_for_this_race) {
                if (gOpponents[i].psyche.grudge_against_player > gGrudge_reduction_per_period) {
                    gOpponents[i].psyche.grudge_against_player -= gGrudge_reduction_per_period;
                }
            }
        }
    }
    RebuildActiveCarList();
    gFirst_frame = 0;
#else
    __asm {
            sub esp, 0x50
            mov eax, dword ptr [gProgram_state+0x1c20]
            push ebx
            push ebp
            push esi
            test eax, eax
            push edi
            mov dword ptr [esp + 0x14], 0
            jne L004a9e35
            mov eax, dword ptr [gNumber_of_cops_before_faffage]
            test eax, eax
            je L004aa3f7
    L004a9e35:
            mov eax, dword ptr [gProgram_state+0x1c28]
            test eax, eax
            je L004aa3f7
            mov eax, dword ptr [gProgram_state+0x1c2c]
            test eax, eax
            je L004aa3f7
            mov ecx, dword ptr [gAcme_frame_count]
            mov eax, dword ptr [gAcknowledged_start]
            inc ecx
            mov ebx, 1
            test eax, eax
            mov dword ptr [gAcme_frame_count], ecx
            jne L004a9e86
            mov eax, dword ptr [gCountdown]
            test eax, eax
            jne L004a9e86
            mov eax, dword ptr [gStart_jumped]
            mov dword ptr [gAcknowledged_start], ebx
            test eax, eax
            jne L004a9e86
            mov dword ptr [esp + 0x14], ebx
    L004a9e86:
            mov eax, dword ptr [gProgram_state+0x14a0]
            test eax, eax
            je L004aa190
            mov eax, dword ptr [gTime_stamp_for_this_munging]
            mov ecx, dword ptr [gProgram_state+0x15dc]
            cmp eax, ecx
            mov edi, ebx
            mov byte ptr [gProgram_state+0x15e6], 0
            jbe L004aa19a
            fld dword ptr [gProgram_state+0x15d8]
            fld dword ptr [gProgram_state+0x124]
            fld dword ptr [gProgram_state+0x15d4]
            fld dword ptr [gProgram_state+0x120]
            fld dword ptr [gProgram_state+0x15d0]
            fxch st(3)
            fmulp st(4), st(0)
            add eax, 0x1f4
            mov ecx, 0x1e
            fmulp st(1), st(0)
            mov dword ptr [gProgram_state+0x15dc], eax
            faddp st(2), st(0)
            fmul dword ptr [gProgram_state+0x11c]
            faddp st(1), st(0)
            fstp dword ptr [esp + 0x10]
            call FastScalarCos
            fcomp dword ptr [esp + 0x10]
            fnstsw ax
            test ah, 0x41
            jne L004a9f06
            mov byte ptr [gProgram_state+0x15e5], bl
    L004a9f06:
            mov eax, dword ptr [gProgram_state+0x15e4]
            mov esi, dword ptr [gProgram_state+0xbc]
            and eax, 0xff
            lea ecx, [esi + 0x50]
            lea edx, [eax + eax*2]
            lea eax, [eax + eax*2]
            fld dword ptr [eax*4 + gProgram_state+0x1490]
            fld dword ptr [edx*4 + gProgram_state+0x148c]
            fsub dword ptr [ecx]
            fxch st(1)
            fsub dword ptr [esi + 0x54]
            lea edx, [edx*4 + gProgram_state+0x148c]
            fld dword ptr [eax*4 + gProgram_state+0x1494]
            fsub dword ptr [esi + 0x58]
            fxch st(2)
            fst dword ptr [esp + 0x24]
            fxch st(1)
            fst dword ptr [esp + 0x28]
            fxch st(1)
            fmul dword ptr [esp + 0x24]
            fxch st(1)
            fmul dword ptr [esp + 0x28]
            fld st(2)
            fmul st(0), st(3)
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fsqrt
            fstp st(1)
            fst dword ptr [esp + 0x10]
            fcomp dword ptr [gTrail_minimum_distance]
            fnstsw ax
            test ah, 1
            jne L004aa19a
            push 0x3f000000
            push ecx
            lea ecx, [esp + 0x20]
            call CalcNegativeXVector
            mov eax, dword ptr [gProgram_state+0x15e4]
            mov ecx, dword ptr [gProgram_state+0xbc]
            fld dword ptr [esp + 0x18]
            fld dword ptr [esp + 0x1c]
            fxch st(1)
            and eax, 0xff
            cmp byte ptr [gProgram_state+0x15e5], 0
            lea edx, [eax + eax*2]
            lea eax, [eax + eax*2]
            fadd dword ptr [edx*4 + gProgram_state+0x148c]
            fld dword ptr [esp + 0x20]
            fld dword ptr [esp + 0x18]
            fxch st(2)
            fstp dword ptr [esp + 0x3c]
            fxch st(2)
            fadd dword ptr [eax*4 + gProgram_state+0x1490]
            fld dword ptr [esp + 0x1c]
            fld dword ptr [esp + 0x20]
            fxch st(2)
            fstp dword ptr [esp + 0x40]
            fxch st(3)
            fadd dword ptr [eax*4 + gProgram_state+0x1494]
            fstp dword ptr [esp + 0x44]
            fxch st(1)
            fadd dword ptr [ecx + 0x50]
            fstp dword ptr [esp + 0x30]
            fxch st(1)
            fadd dword ptr [ecx + 0x54]
            fstp dword ptr [esp + 0x34]
            fadd dword ptr [ecx + 0x58]
            fstp dword ptr [esp + 0x38]
            fld dword ptr [edx*4 + gProgram_state+0x148c]
            fsub dword ptr [esp + 0x18]
            fstp dword ptr [esp + 0x54]
            fld dword ptr [eax*4 + gProgram_state+0x1490]
            fsub dword ptr [esp + 0x1c]
            fstp dword ptr [esp + 0x58]
            fld dword ptr [eax*4 + gProgram_state+0x1494]
            fsub dword ptr [esp + 0x20]
            fstp dword ptr [esp + 0x5c]
            fld dword ptr [ecx + 0x50]
            fsub dword ptr [esp + 0x18]
            fstp dword ptr [esp + 0x48]
            fld dword ptr [ecx + 0x54]
            fsub dword ptr [esp + 0x1c]
            fstp dword ptr [esp + 0x4c]
            fld dword ptr [ecx + 0x58]
            fsub dword ptr [esp + 0x20]
            fstp dword ptr [esp + 0x50]
            jne L004aa0af
            lea edx, [esp + 0x30]
            lea ecx, [esp + 0x3c]
            call PointVisibleFromHere
            mov edi, eax
            test edi, edi
            je L004aa0a9
            lea edx, [esp + 0x48]
            lea ecx, [esp + 0x54]
            call PointVisibleFromHere
            mov edi, eax
            test edi, edi
            je L004aa0a9
            mov eax, dword ptr [gProgram_state+0xbc]
            lea edx, [eax + 0x50]
            mov eax, dword ptr [gProgram_state+0x15e4]
            and eax, 0xff
            lea ecx, [eax + eax*2]
            lea ecx, [ecx*4 + gProgram_state+0x148c]
            call PointVisibleFromHere
            mov edi, eax
            test edi, edi
            jne L004aa139
    L004aa0a9:
            mov ecx, dword ptr [gProgram_state+0xbc]
    L004aa0af:
            test edi, edi
            je L004aa0c8
            fld dword ptr [esp + 0x10]
            fcomp dword ptr [gOppo_pursuit_minimum_velocity_ratio]
            fnstsw ax
            test ah, 0x41
            je L004aa0d9
            test edi, edi
            jne L004aa13f
    L004aa0c8:
            fld dword ptr [esp + 0x10]
            fcomp dword ptr [gTrail_obstructed_distance]
            fnstsw ax
            test ah, 0x41
            jne L004aa13f
    L004aa0d9:
            mov al, byte ptr [gProgram_state+0x15e4]
            cmp al, 0x19
            jae L004aa0eb
            inc al
            mov byte ptr [gProgram_state+0x15e4], al
            jmp L004aa10e
    L004aa0eb:
            push 0x120
            push offset gProgram_state+0x14b0
            push offset gProgram_state+0x14a4
            call memmove
            mov ecx, dword ptr [gProgram_state+0xbc]
            add esp, 0xc
            mov byte ptr [gProgram_state+0x15e6], bl
    L004aa10e:
            mov edx, dword ptr [gProgram_state+0x11c]
            mov eax, dword ptr [gProgram_state+0x120]
            mov dword ptr [gProgram_state+0x15d0], edx
            mov edx, dword ptr [gProgram_state+0x124]
            mov byte ptr [gProgram_state+0x15e5], 0
            mov dword ptr [gProgram_state+0x15d4], eax
            mov dword ptr [gProgram_state+0x15d8], edx
            jmp L004aa13f
    L004aa139:
            mov ecx, dword ptr [gProgram_state+0xbc]
    L004aa13f:
            mov eax, dword ptr [gProgram_state+0x15e4]
            mov ecx, dword ptr [ecx + 0x50]
            and eax, 0xff
            lea eax, [eax + eax*2]
            mov dword ptr [eax*4 + gProgram_state+0x1498], ecx
            mov eax, dword ptr [gProgram_state+0x15e4]
            and eax, 0xff
            lea edx, [eax + eax*2]
            mov eax, dword ptr [gProgram_state+0xbc]
            mov ecx, dword ptr [eax + 0x54]
            mov dword ptr [edx*4 + gProgram_state+0x149c], ecx
            mov eax, dword ptr [gProgram_state+0x15e4]
            and eax, 0xff
            lea edx, [eax + eax*2]
            mov eax, dword ptr [gProgram_state+0xbc]
            mov ecx, dword ptr [eax + 0x58]
            mov dword ptr [edx*4 + gProgram_state+0x14a0], ecx
            jmp L004aa19a
    L004aa190:
            mov ecx, offset gProgram_state+0xac
            call StartRecordingTrail
    L004aa19a:
            mov edx, dword ptr [gProgram_state+0x1c20]
            xor ebp, ebp
            xor edi, edi
            xor esi, esi
            test edx, edx
            mov dword ptr [gNum_of_opponents_pursuing], ebp
            mov dword ptr [gNum_of_opponents_getting_near], edi
            mov dword ptr [gNum_of_opponents_completing_race], esi
            jle L004aa1fc
            mov ecx, offset gProgram_state+0x1d24
    L004aa1c1:
            test byte ptr [ecx + 0xac], 2
            jne L004aa1e1
            mov eax, dword ptr [ecx]
            cmp eax, 2
            jne L004aa1d4
            inc ebp
            jmp L004aa1e1
    L004aa1d4:
            cmp eax, 4
            jne L004aa1dc
            inc edi
            jmp L004aa1e1
    L004aa1dc:
            cmp eax, ebx
            jne L004aa1e1
            inc esi
    L004aa1e1:
            add ecx, 0x1a4
            dec edx
            jne L004aa1c1
            mov dword ptr [gNum_of_opponents_completing_race], esi
            mov dword ptr [gNum_of_opponents_getting_near], edi
            mov dword ptr [gNum_of_opponents_pursuing], ebp
    L004aa1fc:
            mov eax, dword ptr [gProgram_state+0x1c20]
            mov dword ptr [esp + 0x10], 0
            test eax, eax
            mov ebx, 0xfffffffb
            mov ebp, 0xfffffffe
            jle L004aa2b0
            mov esi, offset gProgram_state+0x1d28
    L004aa220:
            test byte ptr [esi + 0xa8], 2
            jne L004aa293
            mov eax, dword ptr [esp + 0x14]
            test eax, eax
            je L004aa238
            mov dword ptr [esi + 0x54], 0
    L004aa238:
            lea edi, [esi - 8]
            mov ecx, edi
            call CalcOpponentConspicuousnessWithAViewToCheatingLikeFuck
            mov ecx, edi
            call CalcPlayerConspicuousness
            mov ecx, edi
            call ProcessThisOpponent
            mov eax, dword ptr [esi]
            mov edi, dword ptr [eax + 0x153c]
            and edi, 0xfffffffd
            mov dword ptr [eax + 0x153c], edi
            mov eax, dword ptr [esi]
            mov edx, dword ptr [eax + 0x153c]
            and edx, ebx
            mov dword ptr [eax + 0x153c], edx
            mov eax, dword ptr [esi]
            mov ecx, dword ptr [eax + 0x153c]
            and ecx, ebp
            mov dword ptr [eax + 0x153c], ecx
            mov edx, dword ptr [esi]
            xor eax, eax
            mov dword ptr [edx + 0x1560], eax
            mov ecx, dword ptr [esi]
            mov dword ptr [ecx + 0x1564], eax
    L004aa293:
            mov eax, dword ptr [esp + 0x10]
            mov ecx, dword ptr [gProgram_state+0x1c20]
            inc eax
            add esi, 0x1a4
            cmp eax, ecx
            mov dword ptr [esp + 0x10], eax
            jl L004aa220
    L004aa2b0:
            mov eax, dword ptr [gNumber_of_cops_before_faffage]
            mov dword ptr [esp + 0x10], 0
            test eax, eax
            jle L004aa374
            mov esi, offset gProgram_state+0x4e60
    L004aa2ca:
            test byte ptr [esi + 0xa8], 2
            jne L004aa357
            mov eax, dword ptr [esp + 0x14]
            test eax, eax
            je L004aa2e6
            mov dword ptr [esi + 0x54], 0
    L004aa2e6:
            lea edi, [esi - 8]
            mov ecx, edi
            call CalcDistanceFromHome
            mov ecx, edi
            call CalcOpponentConspicuousnessWithAViewToCheatingLikeFuck
            mov ecx, edi
            call CalcPlayerConspicuousness
            mov ecx, edi
            call ProcessThisOpponent
            mov eax, dword ptr [esi]
            mov edi, dword ptr [eax + 0x153c]
            and edi, 0xfffffffd
            mov dword ptr [eax + 0x153c], edi
            mov eax, dword ptr [esi]
            mov edx, dword ptr [eax + 0x153c]
            and edx, ebx
            mov dword ptr [eax + 0x153c], edx
            mov eax, dword ptr [esi]
            mov ecx, dword ptr [eax + 0x153c]
            and ecx, ebp
            mov dword ptr [eax + 0x153c], ecx
            mov edx, dword ptr [esi]
            xor eax, eax
            mov dword ptr [edx + 0x1560], eax
            mov ecx, dword ptr [esi]
            mov dword ptr [ecx + 0x1564], eax
            mov eax, dword ptr [esi + 0xa8]
            and ah, 0xf7
            mov dword ptr [esi + 0xa8], eax
    L004aa357:
            mov eax, dword ptr [esp + 0x10]
            mov ecx, dword ptr [gNumber_of_cops_before_faffage]
            inc eax
            add esi, 0x1a4
            cmp eax, ecx
            mov dword ptr [esp + 0x10], eax
            jl L004aa2ca
    L004aa374:
            mov eax, dword ptr [gTime_stamp_for_this_munging]
            mov ecx, dword ptr [gNext_grudge_reduction]
            cmp eax, ecx
            jbe L004aa3e8
            add eax, 0x1388
            xor ebx, ebx
            mov dword ptr [gNext_grudge_reduction], eax
            mov eax, dword ptr [gProgram_state+0x1c20]
            test eax, eax
            jle L004aa3e8
            mov esi, offset gProgram_state+0x1d20
    L004aa39d:
            test byte ptr [esi + 0xb0], 2
            jne L004aa3d8
            mov eax, dword ptr [esi]
            mov edi, dword ptr [gGrudge_reduction_per_period]
            mov edx, eax
            shl edx, 3
            sub edx, eax
            mov eax, dword ptr [gOpponents]
            shl edx, 6
            lea edx, [edx + eax + 0x1b8]
            mov al, byte ptr [edx]
            mov ecx, eax
            and ecx, 0xff
            cmp edi, ecx
            jge L004aa3d4
            mov ecx, edi
    L004aa3d4:
            sub al, cl
            mov byte ptr [edx], al
    L004aa3d8:
            mov eax, dword ptr [gProgram_state+0x1c20]
            inc ebx
            add esi, 0x1a4
            cmp ebx, eax
            jl L004aa39d
    L004aa3e8:
            call RebuildActiveCarList
            mov dword ptr [gFirst_frame], 0
    L004aa3f7:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x50
            ret
            nop
    }
#endif
}

void C2_HOOK_FASTCALL UnStunTheBugger(tOpponent_spec* pOpponent_spec) {

    pOpponent_spec->stun_time_ends = 0;
}

// FUNCTION: CARMA2_HW 0x004ae620
void C2_HOOK_FASTCALL WakeUpOpponentsToTheFactThatTheStartHasBeenJumped(int pWhat_the_countdown_was) {
    int i;
    tOpponent_spec* opponent;

    for (i = 0; i < gProgram_state.AI_vehicles.number_of_opponents; i++) {
        opponent = &gProgram_state.AI_vehicles.opponents[i];
        UnStunTheBugger(opponent);
        StunTheBugger(opponent, MIN(IRandomBetween(1000, 2500), 1000 * pWhat_the_countdown_was));
    }
    for (i = 0; i < gProgram_state.AI_vehicles.number_of_cops; i++) {
        opponent = &gProgram_state.AI_vehicles.cops[i];
        UnStunTheBugger(opponent);
        StunTheBugger(opponent, MIN(IRandomBetween(1000, 2500), 1000 * pWhat_the_countdown_was));
    }
    gAcknowledged_start = 1;
    gStart_jumped = 1;
}

tS16 C2_HOOK_FASTCALL FindNearestPathSection(br_vector3* pActor_coords, br_vector3* pPath_direction, br_vector3* pIntersect, br_scalar* pDistance) {

    return FindNearestGeneralSection(NULL, pActor_coords, pPath_direction, pIntersect, pDistance);
}

void C2_HOOK_FASTCALL WeightedFindNearestNodeAndSection(tCar_spec* pCar, br_vector3* pActor_coords, tS16* pNearest_node_section_no, tS16* pNearest_node, br_scalar* pT, float pWeight) {
    int section_no;
    int no_sections;
    tS16 nearest_section;
    br_scalar nearest_node_distance_squared;
    br_scalar closest_distance_squared;
    br_scalar the_distance_squared;
    br_scalar t;
    br_scalar length_squared_a;
    br_vector3 a;
    br_vector3 p;

#ifdef CARPOCALYPSE2_FIX_BUGS
    nearest_section = -1;
#endif

    *pNearest_node_section_no = -1;
    *pNearest_node = -1;
    nearest_node_distance_squared = BR_SCALAR_MAX;
    closest_distance_squared = BR_SCALAR_MAX;
    if (pCar != NULL) {
        no_sections = pCar->my_trail.number_of_nodes - 1;
    } else {
        no_sections = gProgram_state.AI_vehicles.number_of_path_sections;
    }
    for (section_no = 0; section_no < no_sections; section_no++) {
        br_vector3* start;
        br_vector3* finish;

        if (pCar != NULL) {
            start = &pCar->my_trail.trail_nodes[section_no + 0];
            finish = &pCar->my_trail.trail_nodes[section_no + 1];
        } else {
            start = &gProgram_state.AI_vehicles.path_nodes[gProgram_state.AI_vehicles.path_sections[section_no].node_indices[0]].pos;
            finish = &gProgram_state.AI_vehicles.path_nodes[gProgram_state.AI_vehicles.path_sections[section_no].node_indices[1]].pos;
        }
        BrVector3Sub(&a, finish, start);
        a.v[1] *= pWeight;
        BrVector3Sub(&p, pActor_coords, start);
        p.v[1] *= pWeight;
        the_distance_squared = Vector3DistanceSquared(&p, &a);
        if (the_distance_squared < closest_distance_squared) {
            if (pCar != NULL) {
                *pNearest_node = section_no;
            } else {
                *pNearest_node = gProgram_state.AI_vehicles.path_sections[section_no].node_indices[1];
            }
            nearest_section = section_no + 0;
            closest_distance_squared = the_distance_squared;
        }
        the_distance_squared = BrVector3LengthSquared(&p);
        if (the_distance_squared < closest_distance_squared) {
            if (pCar != NULL) {
                *pNearest_node = section_no + 1;
            } else {
                *pNearest_node = gProgram_state.AI_vehicles.path_sections[section_no].node_indices[0];
            }
            nearest_section = section_no;
            closest_distance_squared = the_distance_squared;
        }
        length_squared_a = BrVector3LengthSquared(&a);
        if (length_squared_a >= 0.0001f) {
            t = BrVector3Dot(&p, &a) / length_squared_a;
            if (t >= 0 && t <= 1.f) {
                *pNearest_node_section_no = section_no;
                *pT = t;
                nearest_node_distance_squared = the_distance_squared;
            }
        }
    }
    if (nearest_node_distance_squared <= closest_distance_squared) {
        *pNearest_node = -1;
    } else {
        *pNearest_node_section_no = nearest_section;
    }
}

// FUNCTION: CARMA2_HW 0x004a82d0
tS16 C2_NAKED C2_HOOK_FASTCALL FindNearestGeneralSection(tCar_spec* pCar, const br_vector3* pActor_coords, br_vector3* pPath_direction, br_vector3* pIntersect, br_scalar* pDistance) {
#ifndef CARPOCALYPSE2_MATCHING
    tS16 nearest_node_section_no;
    tS16 nearest_node;
    float t;
    br_vector3 delta;

    WeightedFindNearestNodeAndSection(pCar, pActor_coords, &nearest_node_section_no, &nearest_node, &t, 2.f);

    if (pCar != NULL) {
        BrVector3Sub(&delta,
                &pCar->my_trail.trail_nodes[nearest_node_section_no + 1],
                &pCar->my_trail.trail_nodes[nearest_node_section_no + 0]);
    } else {
        BrVector3Sub(&delta,
            &gProgram_state.AI_vehicles.path_nodes[gProgram_state.AI_vehicles.path_sections[nearest_node_section_no].node_indices[1]].pos,
            &gProgram_state.AI_vehicles.path_nodes[gProgram_state.AI_vehicles.path_sections[nearest_node_section_no].node_indices[0]].pos);
    }
    if (nearest_node >= 0) {
        BrVector3Copy(pIntersect, pCar != NULL ? &pCar->my_trail.trail_nodes[nearest_node] : &gProgram_state.AI_vehicles.path_nodes[nearest_node_section_no].pos);
    } else {
        BrVector3Scale(pIntersect, &delta, t);
        BrVector3Accumulate(pIntersect, &gProgram_state.AI_vehicles.path_nodes[gProgram_state.AI_vehicles.path_sections[nearest_node_section_no].node_indices[0]].pos);
    }
    BrVector3InvScale(pPath_direction, &delta, BrVector3Length(&delta));
    *pDistance = Vector3Distance(pActor_coords, pIntersect);
    if (pCar != NULL) {
        nearest_node_section_no += 15000;
    }
    return nearest_node_section_no;
#else
    __asm {
            sub esp, 0x44
            fld dword ptr [gOppo_pursuit_velocity_scale+0x4]
            push ebx
            push ebp
            or ebx, 0xffffffff
            or ebp, 0xffffffff
            push esi
            push edi
            test ecx, ecx
            mov dword ptr [esp + 0x2c], edx
            mov dword ptr [esp + 0x14], ebx
            mov dword ptr [esp + 0x18], 0x7f7fffff
            je L004a8302
            xor eax, eax
            mov al, byte ptr [ecx + 0x1538]
            dec eax
            jmp L004a8307
    L004a8302:
            mov eax, dword ptr [gProgram_state+0x1c2c]
    L004a8307:
            mov edx, dword ptr [gProgram_state+0x5ec0]
            xor edi, edi
            test eax, eax
            mov dword ptr [esp + 0x24], eax
            jle L004a8556
            mov ebx, dword ptr [gProgram_state+0x5ec4]
            lea esi, [ecx + 0x1404]
            mov dword ptr [esp + 0x20], esi
    L004a832b:
            test ecx, ecx
            je L004a8334
            lea eax, [esi - 0xc]
            jmp L004a8345
    L004a8334:
            movsx eax, word ptr [ebx]
            movsx esi, word ptr [ebx + 2]
            shl eax, 5
            shl esi, 5
            add eax, edx
            add esi, edx
    L004a8345:
            mov edx, dword ptr [esp + 0x2c]
            fld dword ptr [esi + 4]
            fld dword ptr [edx + 4]
            fsub dword ptr [eax + 4]
            fxch st(1)
            fsub dword ptr [eax + 4]
            _emit 0xd9
        _emit 0x06
            _emit 0xd9
        _emit 0x02
            _emit 0xd8
        _emit 0x20
            fxch st(1)
            _emit 0xd8
        _emit 0x20
            fld dword ptr [edx + 8]
            fxch st(4)
            fadd st(0), st(0)
            fld dword ptr [esi + 8]
            fxch st(2)
            fstp dword ptr [esp + 0x30]
            fxch st(4)
            fsub dword ptr [eax + 8]
            fxch st(3)
            fadd st(0), st(0)
            fxch st(1)
            fsub dword ptr [eax + 8]
            fxch st(2)
            fst dword ptr [esp + 0x3c]
            fxch st(4)
            fst dword ptr [esp + 0x40]
            fxch st(4)
            fsub dword ptr [esp + 0x30]
            fxch st(1)
            fstp dword ptr [esp + 0x34]
            fxch st(3)
            fsub dword ptr [esp + 0x34]
            fxch st(2)
            fst dword ptr [esp + 0x44]
            fxch st(1)
            fstp dword ptr [esp + 0x38]
            fsub dword ptr [esp + 0x38]
            fxch st(2)
            fst dword ptr [esp + 0x48]
            fxch st(1)
            fst dword ptr [esp + 0x4c]
            fxch st(1)
            fmul dword ptr [esp + 0x48]
            fxch st(1)
            fmul dword ptr [esp + 0x4c]
            fld st(2)
            fmul st(0), st(3)
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fst dword ptr [esp + 0x10]
            fstp st(1)
            fcomp st(1)
            fnstsw ax
            test ah, 1
            je L004a83f2
            fstp st(0)
            fld dword ptr [esp + 0x10]
            test ecx, ecx
            mov ebp, edi
            jne L004a83ee
            movsx ebp, word ptr [ebx + 2]
    L004a83ee:
            mov dword ptr [esp + 0x28], edi
    L004a83f2:
            fld dword ptr [esp + 0x40]
            fld dword ptr [esp + 0x3c]
            fmul dword ptr [esp + 0x3c]
            fld dword ptr [esp + 0x44]
            fxch st(2)
            fmul dword ptr [esp + 0x40]
            fxch st(2)
            fmul dword ptr [esp + 0x44]
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fst dword ptr [esp + 0x10]
            fcomp st(1)
            fnstsw ax
            test ah, 1
            je L004a8437
            fstp st(0)
            fld dword ptr [esp + 0x10]
            test ecx, ecx
            lea ebp, [edi + 1]
            jne L004a8433
            movsx ebp, word ptr [ebx]
    L004a8433:
            mov dword ptr [esp + 0x28], edi
    L004a8437:
            fld dword ptr [esp + 0x34]
            fld dword ptr [esp + 0x30]
            fmul dword ptr [esp + 0x30]
            fld dword ptr [esp + 0x38]
            fxch st(2)
            fmul dword ptr [esp + 0x34]
            fxch st(2)
            fmul dword ptr [esp + 0x38]
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fld st(0)
            fcomp dword ptr [gOppo_pursuit_minimum_velocity_ratio+0x4]
            fnstsw ax
            test ah, 1
            jne L004a852f
            fld dword ptr [esp + 0x40]
            fld dword ptr [esp + 0x3c]
            fmul dword ptr [esp + 0x30]
            fld dword ptr [esp + 0x44]
            fxch st(2)
            fmul dword ptr [esp + 0x34]
            fxch st(2)
            fmul dword ptr [esp + 0x38]
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fdiv st(0), st(1)
            fstp dword ptr [esp + 0x10]
            fstp st(0)
            fld dword ptr [esp + 0x10]
            fcomp dword ptr [gOppo_render_zero]
            fnstsw ax
            test ah, 1
            jne L004a8531
            fld dword ptr [esp + 0x10]
            fcomp dword ptr [gOppo_pursuit_one]
            fnstsw ax
            test ah, 0x41
            je L004a8531
            fld dword ptr [esp + 0x34]
            fld dword ptr [esp + 0x30]
            fmul dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x38]
            fxch st(2)
            fmul dword ptr [esp + 0x10]
            fxch st(2)
            fmul dword ptr [esp + 0x10]
            fxch st(1)
            fsubr dword ptr [esp + 0x3c]
            fxch st(2)
            fsubr dword ptr [esp + 0x40]
            fxch st(1)
            fsubr dword ptr [esp + 0x44]
            fxch st(2)
            fst dword ptr [esp + 0x48]
            fxch st(1)
            fst dword ptr [esp + 0x4c]
            fxch st(1)
            fmul dword ptr [esp + 0x48]
            fxch st(1)
            fmul dword ptr [esp + 0x4c]
            fld st(2)
            fmul st(0), st(3)
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fstp st(1)
            fcom dword ptr [esp + 0x18]
            fnstsw ax
            test ah, 1
            je L004a852f
            mov eax, dword ptr [esp + 0x10]
            mov dword ptr [esp + 0x14], edi
            fstp dword ptr [esp + 0x18]
            mov dword ptr [esp + 0x1c], eax
            jmp L004a8531
    L004a852f:
            fstp st(0)
    L004a8531:
            mov esi, dword ptr [esp + 0x20]
            mov eax, dword ptr [esp + 0x24]
            mov edx, dword ptr [gProgram_state+0x5ec0]
            inc edi
            add esi, 0xc
            add ebx, 0x14
            cmp edi, eax
            mov dword ptr [esp + 0x20], esi
            jl L004a832b
            mov ebx, dword ptr [esp + 0x14]
    L004a8556:
            fcomp dword ptr [esp + 0x18]
            fnstsw ax
            test ah, 1
            je L004a856d
            mov eax, dword ptr [esp + 0x28]
            mov dword ptr [esp + 0x14], eax
            mov ebx, eax
            jmp L004a8570
    L004a856d:
            or ebp, 0xffffffff
    L004a8570:
            test ecx, ecx
            je L004a85af
            lea eax, [ebx + 0x1ab]
            lea esi, [eax + eax*2]
            lea eax, [ebx + 0x1aa]
            fld dword ptr [ecx + esi*4]
            lea eax, [eax + eax*2]
            fsub dword ptr [ecx + eax*4]
            lea eax, [ebx + ebx*2]
            fld dword ptr [ecx + eax*4 + 0x1408]
            fsub dword ptr [ecx + eax*4 + 0x13fc]
            fld dword ptr [ecx + eax*4 + 0x140c]
            lea eax, [ecx + eax*4]
            fsub dword ptr [eax + 0x1400]
            jmp L004a85de
    L004a85af:
            mov esi, dword ptr [gProgram_state+0x5ec4]
            lea eax, [ebx + ebx*4]
            lea esi, [esi + eax*4]
            movsx eax, word ptr [esi + 2]
            movsx esi, word ptr [esi]
            shl eax, 5
            add eax, edx
            shl esi, 5
            _emit 0xd9
        _emit 0x00
            fsub dword ptr [esi + edx]
            fld dword ptr [eax + 4]
            fsub dword ptr [esi + edx + 4]
            add esi, edx
            fld dword ptr [eax + 8]
            fsub dword ptr [esi + 8]
    L004a85de:
            test ebp, ebp
            jl L004a865c
            test ecx, ecx
            je L004a85f4
            lea eax, [ebp + 0x1aa]
            lea edx, [eax + eax*2]
            lea eax, [ecx + edx*4]
            jmp L004a85fb
    L004a85f4:
            mov eax, ebp
            shl eax, 5
            add eax, edx
    L004a85fb:
            mov edx, dword ptr [eax]
            mov eax, dword ptr [esp + 0x5c]
            test ecx, ecx
            mov dword ptr [eax], edx
            je L004a8615
            lea esi, [ebp + 0x1aa]
            lea edx, [esi + esi*2]
            lea esi, [ecx + edx*4]
            jmp L004a8622
    L004a8615:
            mov edx, dword ptr [gProgram_state+0x5ec0]
            mov esi, ebp
            shl esi, 5
            add esi, edx
    L004a8622:
            mov edx, dword ptr [esi + 4]
            test ecx, ecx
            mov dword ptr [eax + 4], edx
            je L004a8644
            lea esi, [ebp + 0x1aa]
            lea edx, [esi + esi*2]
            lea esi, [ecx + edx*4]
            mov edx, dword ptr [ecx + edx*4 + 8]
            mov dword ptr [eax + 8], edx
            jmp L004a86f0
    L004a8644:
            mov edx, dword ptr [gProgram_state+0x5ec0]
            shl ebp, 5
            add ebp, edx
            mov esi, ebp
            mov edx, dword ptr [esi + 8]
            mov dword ptr [eax + 8], edx
            jmp L004a86f0
    L004a865c:
            fld st(2)
            fld st(2)
            fld st(2)
            fxch st(2)
            fmul dword ptr [esp + 0x1c]
            fxch st(1)
            fmul dword ptr [esp + 0x1c]
            fxch st(2)
            fmul dword ptr [esp + 0x1c]
            mov eax, dword ptr [esp + 0x5c]
            lea esi, [ebx + ebx*4]
            fld st(1)
            fxch st(3)
            fst dword ptr [esp + 0x5c]
            fxch st(1)
            fst dword ptr [esp + 0x28]
            fxch st(3)
            _emit 0xd9
        _emit 0x18
            shl esi, 2
            fstp dword ptr [eax + 4]
            fxch st(1)
            fstp dword ptr [eax + 8]
            mov edx, dword ptr [gProgram_state+0x5ec4]
            mov edi, dword ptr [gProgram_state+0x5ec0]
            movsx edx, word ptr [edx + esi]
            shl edx, 5
            fld dword ptr [edx + edi]
            fadd st(0), st(1)
            _emit 0xd9
        _emit 0x18
            mov edx, dword ptr [gProgram_state+0x5ec4]
            mov edi, dword ptr [gProgram_state+0x5ec0]
            fstp st(0)
            movsx edx, word ptr [edx + esi]
            shl edx, 5
            fld dword ptr [edx + edi + 4]
            fadd dword ptr [esp + 0x5c]
            fstp dword ptr [eax + 4]
            mov edx, dword ptr [gProgram_state+0x5ec4]
            movsx edx, word ptr [edx + esi]
            mov esi, dword ptr [gProgram_state+0x5ec0]
            shl edx, 5
            fld dword ptr [edx + esi + 8]
            fadd dword ptr [esp + 0x28]
            fstp dword ptr [eax + 8]
    L004a86f0:
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
            test ecx, ecx
            mov ecx, dword ptr [esp + 0x58]
            faddp st(1), st(0)
            fsqrt
            fdivr qword ptr [DAT_0058a4a0]
            fld st(0)
            fld st(1)
            fxch st(1)
            fmul st(0), st(5)
            fxch st(1)
            fmul st(0), st(4)
            fxch st(2)
            fmul st(0), st(3)
            fxch st(1)
            _emit 0xd9
        _emit 0x19
            fxch st(1)
            fstp dword ptr [ecx + 4]
            fstp dword ptr [ecx + 8]
            mov ecx, dword ptr [esp + 0x2c]
            fstp st(0)
            fstp st(0)
            fstp st(0)
            fld dword ptr [ecx + 4]
            _emit 0xd9
        _emit 0x01
            _emit 0xd8
        _emit 0x20
            fxch st(1)
            fsub dword ptr [eax + 4]
            fld dword ptr [ecx + 8]
            fsub dword ptr [eax + 8]
            fxch st(2)
            fst dword ptr [esp + 0x48]
            fxch st(1)
            fst dword ptr [esp + 0x4c]
            fxch st(1)
            fmul dword ptr [esp + 0x48]
            fxch st(1)
            fmul dword ptr [esp + 0x4c]
            fld st(2)
            fmul st(0), st(3)
            fxch st(1)
            faddp st(2), st(0)
            mov eax, dword ptr [esp + 0x60]
            faddp st(1), st(0)
            fsqrt
            fstp st(1)
            _emit 0xd9
        _emit 0x18
            je L004a8784
            add ebx, 0x3a98
    L004a8784:
            pop edi
            pop esi
            mov ax, bx
            pop ebp
            pop ebx
            add esp, 0x44
            ret 0xc
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004ae9e0
int C2_HOOK_FASTCALL GetOpponentsRealSection(tOpponent_spec* pOpponent_spec, int pSection_no) {
    if (pSection_no >= 20000) {
        return pOpponent_spec->next_sections[pSection_no - 20000].section_no;
    } else if (pSection_no >= 15000) {
        return pSection_no - 15000;
    } else {
        return pSection_no;
    }
}

// FUNCTION: CARMA2_HW 0x004ab100
int C2_HOOK_FASTCALL ShiftOpponentsProjectedRoute(tOpponent_spec* pOpponent_spec, int pPlaces) {
    int i;

    C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(pOpponent_spec->next_sections) != 12);

    if (pOpponent_spec->nnext_sections > pPlaces) {
        for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(pOpponent_spec->next_sections) - pPlaces; i++) {
            pOpponent_spec->next_sections[i].section_no = pOpponent_spec->next_sections[pPlaces + i].section_no;
            pOpponent_spec->next_sections[i].direction = pOpponent_spec->next_sections[pPlaces + i].direction;
        }
        pOpponent_spec->nnext_sections -= pPlaces;
        return 1;
    } else {
        return 0;
    }
}

__inline int C2_HOOK_FASTCALL AlreadyPursuingCar(tOpponent_spec* pOpponent_spec, tCar_spec* pPursuee) {

    return pOpponent_spec->current_objective == eOOT_pursue_and_twat && pOpponent_spec->pursue_car_data.pursuee == pPursuee;
}

__inline int C2_HOOK_FASTCALL LastTwatterAPlayer(tOpponent_spec* pOpponent_spec) {

    return pOpponent_spec->car_spec->last_person_to_hit_us != NULL && pOpponent_spec->car_spec->last_person_to_hit_us->driver == eDriver_local_human;
}

__inline int C2_HOOK_FASTCALL LastTwatteeAPlayer(tOpponent_spec* pOpponent_spec) {

    return pOpponent_spec->car_spec->last_person_we_hit != NULL && pOpponent_spec->car_spec->last_person_we_hit->driver == eDriver_local_human;
}

__inline int C2_HOOK_FASTCALL HeadOnWithPlayerPossible(tOpponent_spec* pOpponent_spec) {
    br_vector3 oppo_to_player_norm;

    BrVector3Sub(&oppo_to_player_norm,
        &gProgram_state.current_car.car_master_actor->t.t.translate.t,
        &pOpponent_spec->car_spec->car_master_actor->t.t.translate.t);
    {
        br_scalar _scale;

        _scale = BR_LENGTH3(oppo_to_player_norm.v[0], oppo_to_player_norm.v[1], oppo_to_player_norm.v[2]);
        if (_scale > DAT_0058a4e8) {
            _scale = (br_scalar)(DAT_0058a4a0 / _scale);
            oppo_to_player_norm.v[0] = oppo_to_player_norm.v[0] * _scale;
            oppo_to_player_norm.v[1] = oppo_to_player_norm.v[1] * _scale;
            oppo_to_player_norm.v[2] = oppo_to_player_norm.v[2] * _scale;
        } else {
            oppo_to_player_norm.v[0] = gOppo_pursuit_one;
            oppo_to_player_norm.v[1] = gOppo_render_zero;
            oppo_to_player_norm.v[2] = gOppo_render_zero;
        }
    }
    if (BrVector3Dot(&pOpponent_spec->car_spec->direction, &oppo_to_player_norm) > gHead_on_cos_value
            && BrVector3Dot(&gProgram_state.current_car.direction, &oppo_to_player_norm) < -gHead_on_cos_value) {
        DoNotDprintf_opponent("HOORAY! Head-on imminent");
        return 1;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004ace70
void C2_NAKED C2_HOOK_FASTCALL ChooseNewObjective(tOpponent_spec* pOpponent_spec, int pMust_choose_one) {
#ifndef CARPOCALYPSE2_MATCHING
    char str[256];

    if (pOpponent_spec->current_objective == eOOT_knackered_and_freewheeling || pOpponent_spec->knackeredness_detected) {
        return;
    }
    if (gTime_stamp_for_this_munging > pOpponent_spec->next_out_of_world_check) {
        pOpponent_spec->next_out_of_world_check = gTime_stamp_for_this_munging + 500;
        if (HasCarFallenOffWorld(pOpponent_spec->car_spec)) {
            TeleportOpponentToNearestSafeLocation(pOpponent_spec);
            NewObjective(pOpponent_spec, eOOT_get_near_player);
            return;
        }
    }
    if (pOpponent_spec->car_spec->knackered && !pOpponent_spec->knackeredness_detected) {
        pOpponent_spec->knackeredness_detected = 1;
        DoNotDprintf_opponent("%s: Knackered - dealing with appropriately", pOpponent_spec->car_spec->driver_name);
        if (pOpponent_spec->car_spec->has_been_stolen) {
            NewObjective(pOpponent_spec, eOOT_levitate);
        } else {
            NewObjective(pOpponent_spec, eOOT_knackered_and_freewheeling);
        }
        return;
    }

    if (pOpponent_spec->current_objective == eOOT_frozen) {
        if (GET_CAR_SPEED_FACTOR(pOpponent_spec->car_spec) == gOppo_render_zero) {
            return;
        }
        DoNotDprintf_opponent("%s: Time to unfreeze", pOpponent_spec->car_spec->driver_name);
        if (pOpponent_spec->pursuing_player_before_freeze) {
            NewObjective(pOpponent_spec, eOOT_pursue_and_twat, &gProgram_state.current_car);
        } else {
            NewObjective(pOpponent_spec, eOOT_get_near_player);
        }
        return;
    }
    if (GET_CAR_SPEED_FACTOR(pOpponent_spec->car_spec) == gOppo_render_zero) {
        DoNotDprintf_opponent("%s: Decided to freeze", pOpponent_spec->car_spec->driver_name);
        pOpponent_spec->pursuing_player_before_freeze = pOpponent_spec->current_objective == eOOT_pursue_and_twat && pOpponent_spec->pursue_car_data.pursuee == &gProgram_state.current_car;
        NewObjective(pOpponent_spec, eOOT_frozen);
        return;
    }
    if (!gFirst_frame) {
        int general_grudge_increase;

        general_grudge_increase = (int)(pOpponent_spec->nastiness * 40.0f + DAT_0058a4ec);
        if (pOpponent_spec->car_spec->scary_bang && pOpponent_spec->player_to_oppo_d < DAT_0058a4ec) {
            int pursuit_percentage;

            if (pOpponent_spec->current_objective == eOOT_pursue_and_twat) {
                pursuit_percentage = 40;
            } else {
                pursuit_percentage = 0;
            }
            if (CAR_SPEC_IS_ROZZER(pOpponent_spec->car_spec)) {
                if (PercentageChance(20)) {
                    DoNotDprintf_opponent("%s: Decided to run away", pOpponent_spec->car_spec->driver_name);
                    NewObjective(pOpponent_spec, eOOT_run_away);
                    return;
                }
            } else if (gOpponents[pOpponent_spec->index].strength_rating < 5) {
                if (PercentageChance((int)((pursuit_percentage + 60) - 50.f * pOpponent_spec->nastiness))) {
                    DoNotDprintf_opponent("%s: Decided to run away", pOpponent_spec->car_spec->driver_name);
                    NewObjective(pOpponent_spec, eOOT_run_away);
                    return;
                }
            }
        }

        if (pOpponent_spec->current_objective != eOOT_run_away || gTime_stamp_for_this_munging >= pOpponent_spec->time_this_objective_started + 20000) {
            if (CAR_SPEC_IS_ROZZER(pOpponent_spec->car_spec) && pOpponent_spec->murder_reported && pOpponent_spec->player_to_oppo_d < gOppo_pursuit_trail_distance && !AlreadyPursuingCar(pOpponent_spec, &gProgram_state.current_car)) {
                gOpponents[pOpponent_spec->index].psyche.grudge_against_player = (tU8)MIN(100, MAX(gOpponents[pOpponent_spec->index].psyche.grudge_against_player, 20) + general_grudge_increase);
                sprintf(str, "%s: Furderous melon!", pOpponent_spec->car_spec->driver_name);
                DoNotDprintf_opponent("%s: Decided to pursue after MURDER", pOpponent_spec->car_spec->driver_name);
                NewObjective(pOpponent_spec, eOOT_pursue_and_twat, &gProgram_state.current_car);
                return;
            }
            if (pOpponent_spec->car_spec->big_bang
                    && LastTwatterAPlayer(pOpponent_spec)
                    && !AlreadyPursuingCar(pOpponent_spec, pOpponent_spec->car_spec->last_person_to_hit_us)) {

                gOpponents[pOpponent_spec->index].psyche.grudge_against_player = (tU8)MIN(100, MAX(gOpponents[pOpponent_spec->index].psyche.grudge_against_player, 20) + general_grudge_increase);
                sprintf(str, "%s: Christ! What was that?", pOpponent_spec->car_spec->driver_name);
                DoNotDprintf_opponent("%s: Decided to pursue after big bang; last person to twat us was %s",
                    pOpponent_spec->car_spec->driver_name,
                    pOpponent_spec->car_spec->last_person_to_hit_us->driver_name);
                NewObjective(pOpponent_spec, eOOT_pursue_and_twat, pOpponent_spec->car_spec->last_person_to_hit_us);
                return;
            }
            if (LastTwatteeAPlayer(pOpponent_spec) && !AlreadyPursuingCar(pOpponent_spec, pOpponent_spec->car_spec->last_person_we_hit)) {

                gOpponents[pOpponent_spec->index].psyche.grudge_against_player = (tU8)MIN(100, MAX(gOpponents[pOpponent_spec->index].psyche.grudge_against_player, 20) + general_grudge_increase);
                sprintf(str, "%s: Ha! Bet you weren't expecting that!", pOpponent_spec->car_spec->driver_name);
                DoNotDprintf_opponent("%s: Decided to pursue %s after accidentally hitting them",
                    pOpponent_spec->car_spec->driver_name,
                    pOpponent_spec->car_spec->last_person_we_hit->driver_name);
                NewObjective(pOpponent_spec, eOOT_pursue_and_twat, &gProgram_state.current_car);
                return;
            }
            if (!AlreadyPursuingCar(pOpponent_spec, &gProgram_state.current_car)) {
                if (pOpponent_spec->car_spec->grudge_raised_recently
                        && (!CAR_SPEC_IS_ROZZER(pOpponent_spec->car_spec) || pOpponent_spec->player_to_oppo_d <= gOppo_pursuit_trail_distance)
                        && LastTwatterAPlayer(pOpponent_spec)
                        && gOpponents[pOpponent_spec->index].psyche.grudge_against_player > 20) {
                    gOpponents[pOpponent_spec->index].psyche.grudge_against_player = (tU8)MIN(100, MAX(gOpponents[pOpponent_spec->index].psyche.grudge_against_player, 20) + general_grudge_increase);
                    sprintf(str, "%s: Right! That's enough, %s!", pOpponent_spec->car_spec->driver_name, gProgram_state.current_car.driver_name);
                    DoNotDprintf_opponent("%s: Decided to pursue after grudginess raised; last person to twat us was %s",
                        pOpponent_spec->car_spec->driver_name, pOpponent_spec->car_spec->last_person_to_hit_us->driver_name);
                    NewObjective(pOpponent_spec, eOOT_pursue_and_twat, &gProgram_state.current_car);
                    return;
                }
                if (pOpponent_spec->player_in_view_now && !pOpponent_spec->acknowledged_piv) {
                    int pursuit_percentage;
                    int do_it;

                    pOpponent_spec->acknowledged_piv = 1;
                    if (CAR_SPEC_IS_ROZZER(pOpponent_spec->car_spec)) {
                        pursuit_percentage = (int)(gCop_pursuit_speed_percentage_multiplier * (BrVector3Length(&gProgram_state.current_car.collision_info->v) * DAT_0058a4b0 - gDefinite_no_cop_pursuit_speed));
                    } else if (gNum_of_opponents_pursuing < gProgram_state.skill_level + 3) {
                        pursuit_percentage = (int)((float)(gOpponents[pOpponent_spec->index].psyche.grudge_against_player - 20) + 30.f * pOpponent_spec->nastiness);
                    } else {
                        pursuit_percentage = 0;
                    }

                    pursuit_percentage += 50 * HeadOnWithPlayerPossible(pOpponent_spec);
                    do_it = PercentageChance(pursuit_percentage);
                    DoNotDprintf_opponent("%s: Spotted player; chance of pursuing %d%%: %s", pOpponent_spec->car_spec->driver_name, pursuit_percentage, do_it ? "YES, Decided to pursue" : "NO, Decided NOT to pursue");
                    if (do_it) {
                        gOpponents[pOpponent_spec->index].psyche.grudge_against_player = (tU8)MIN(100, MAX(gOpponents[pOpponent_spec->index].psyche.grudge_against_player, 20) + general_grudge_increase);
                        sprintf(str, "%s: I've decided to kill you for the fun of it", pOpponent_spec->car_spec->driver_name);
                        NewObjective(pOpponent_spec, eOOT_pursue_and_twat, &gProgram_state.current_car);
                        return;
                    }
                }
            }
        }
    }
    if (pMust_choose_one) {
        DoNotDprintf_opponent("%s: Choosing new objective because we have to...", pOpponent_spec->car_spec->driver_name, pOpponent_spec->car_spec->last_person_to_hit_us);
        if (pOpponent_spec->has_moved_at_some_point) {
            int pursuit_percentage;

            if (CAR_SPEC_IS_ROZZER(pOpponent_spec->car_spec)) {
                NewObjective(pOpponent_spec, eOOT_return_to_start);
                return;
            }
            switch (pOpponent_spec->field_0xb4) {
            case 1:
                NewObjective(pOpponent_spec, eOOT_complete_race);
                break;
            case 2:
                NewObjective(pOpponent_spec, eOOT_get_near_player);
                break;
            case 3:
                NewObjective(pOpponent_spec, eOOT_pursue_and_twat, &gProgram_state.current_car);
                break;
            default:
                pursuit_percentage = (int)pOpponent_spec->player_to_oppo_d - 15;
                if (PercentageChance(pursuit_percentage)) {
                    DoNotDprintf_opponent("%s: Choosing to get_near because chance dictated it (%d%%)", pOpponent_spec->car_spec->driver_name, pursuit_percentage);
                    NewObjective(pOpponent_spec, eOOT_get_near_player);
                } else {
                    DoNotDprintf_opponent("%s: Choosing to complete_race because chance dictated it (%d%%)", pOpponent_spec->car_spec->driver_name, pursuit_percentage);
                    NewObjective(pOpponent_spec, eOOT_complete_race);
                }
                return;
            }
        } else if (CAR_SPEC_IS_ROZZER(pOpponent_spec->car_spec)) {
            NewObjective(pOpponent_spec, eOOT_wait_for_some_hapless_sod);
            return;
        } else if (pOpponent_spec->pursue_from_start) {

            gOpponents[pOpponent_spec->index].psyche.grudge_against_player = (tU8)MIN(DAT_0058a47c, MAX(gOpponents[pOpponent_spec->index].psyche.grudge_against_player, 20) + 20 + 40 * pOpponent_spec->nastiness);
            NewObjective(pOpponent_spec, eOOT_pursue_and_twat, &gProgram_state.current_car);
            return;
        } else {
            NewObjective(pOpponent_spec, eOOT_complete_race);
        }
    }
#else
    __asm {
            sub esp, 0x10c
            push ebx
            push ebp
            push esi
            mov esi, ecx
            push edi
            mov dword ptr [esp + 0x14], edx
            cmp dword ptr [esi + 4], 6
            je L004ad818
            test byte ptr [esi + 0xb0], 4
            jne L004ad818
            mov eax, dword ptr [gTime_stamp_for_this_munging]
            mov ecx, dword ptr [esi + 0x4c]
            cmp eax, ecx
            jbe L004aced4
            mov ecx, dword ptr [esi + 8]
            add eax, 0x1f4
            mov dword ptr [esi + 0x4c], eax
            call HasCarFallenOffWorld
            test eax, eax
            je L004aced4
            mov ecx, esi
            call TeleportOpponentToNearestSafeLocation
            push 4
            push esi
            call NewObjective
            add esp, 8
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x10c
            ret
    L004aced4:
            mov ebp, dword ptr [esi + 8]
            mov eax, dword ptr [ebp + 0x1d4]
            test eax, eax
            je L004acf40
            mov eax, dword ptr [esi + 0xb0]
            test al, 4
            jne L004acf40
            add ebp, 0xec
            or al, 4
            push ebp
            push offset gOpponent_nastyness_frigger+0x1198
            mov dword ptr [esi + 0xb0], eax
            call DoNotDprintf_opponent
            mov eax, dword ptr [esi + 8]
            add esp, 8
            mov ecx, dword ptr [eax + 0x1cc]
            test ecx, ecx
            je L004acf2a
            push 5
            push esi
            call NewObjective
            add esp, 8
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x10c
            ret
    L004acf2a:
            push 6
            push esi
            call NewObjective
            add esp, 8
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x10c
            ret
    L004acf40:
            mov ebx, dword ptr [esi + 4]
            cmp ebx, 7
            jne L004acfa2
            xor ecx, ecx
            mov cx, word ptr [ebp + 0x80]
            and cl, 0
            cmp ecx, 0x300
            jne L004acf64
            fld dword ptr [gCop_speed_factor]
            jmp L004acf6a
    L004acf64:
            fld dword ptr [gOpponent_speed_factor]
    L004acf6a:
            fcomp dword ptr [gOppo_render_zero]
            fnstsw ax
            test ah, 0x40
            jne L004ad818
            add ebp, 0xec
            push ebp
            push offset gOpponent_nastyness_frigger+0x1180
            call DoNotDprintf_opponent
            mov al, byte ptr [esi + 0xb0]
            add esp, 8
            test al, 0x80
            je L004ad823
            jmp L004ad8f2
    L004acfa2:
            xor edi, edi
            mov di, word ptr [ebp + 0x80]
            and edi, 0xffffff00
            cmp edi, 0x300
            jne L004acfc1
            fld dword ptr [gCop_speed_factor]
            jmp L004acfc7
    L004acfc1:
            fld dword ptr [gOpponent_speed_factor]
    L004acfc7:
            fcomp dword ptr [gOppo_render_zero]
            fnstsw ax
            test ah, 0x40
            je L004ad032
            add ebp, 0xec
            push ebp
            push offset gOpponent_nastyness_frigger+0x1168
            call DoNotDprintf_opponent
            mov eax, dword ptr [esi + 4]
            add esp, 8
            cmp eax, 2
            jne L004ad003
            cmp dword ptr [esi + 0x114], 0x75bc2c
            jne L004ad003
            mov eax, 1
            jmp L004ad005
    L004ad003:
            xor eax, eax
    L004ad005:
            mov edx, dword ptr [esi + 0xb0]
            and eax, 1
            and dl, 0x7f
            push 7
            shl eax, 7
            or edx, eax
            push esi
            mov dword ptr [esi + 0xb0], edx
            call NewObjective
            add esp, 8
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x10c
            ret
    L004ad032:
            mov eax, dword ptr [gFirst_frame]
            test eax, eax
            jne L004ad73a
            fld dword ptr [esi + 0xc]
            fmul dword ptr [DAT_0058a55c+0x4]
            fsubr dword ptr [DAT_0058a4ec]
            call _ftol
            mov dword ptr [esp + 0x18], eax
            mov al, byte ptr [ebp + 0x153c]
            test al, 4
            je L004ad13a
            fld dword ptr [esi + 0x18]
            fcomp dword ptr [DAT_0058a4ec]
            fnstsw ax
            test ah, 1
            je L004ad13a
            xor ecx, ecx
            cmp ebx, 2
            setne cl
            dec ecx
            and ecx, 0x28
            cmp edi, 0x300
            jne L004ad0cb
            mov ecx, 0x14
            call PercentageChance
            test eax, eax
            je L004ad13a
            mov eax, dword ptr [esi + 8]
            add eax, 0xec
            push eax
            push offset gOpponent_nastyness_frigger+0x1150
            call DoNotDprintf_opponent
            add esp, 8
            push 3
            push esi
            call NewObjective
            add esp, 8
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x10c
            ret
    L004ad0cb:
            mov eax, dword ptr [esi]
            mov edx, dword ptr [gOpponents]
            mov edi, eax
            shl edi, 3
            sub edi, eax
            shl edi, 6
            cmp dword ptr [edi + edx + 0x19c], 5
            jge L004ad140
            add ecx, 0x3c
            mov dword ptr [esp + 0x10], ecx
            fild dword ptr [esp + 0x10]
            fld dword ptr [esi + 0xc]
            fmul dword ptr [DAT_0058a55c+0x8]
            fsubp st(1), st(0)
            call _ftol
            mov ecx, eax
            call PercentageChance
            test eax, eax
            je L004ad13a
            mov ecx, dword ptr [esi + 8]
            add ecx, 0xec
            push ecx
            push offset gOpponent_nastyness_frigger+0x1150
            call DoNotDprintf_opponent
            add esp, 8
            push 3
            push esi
            call NewObjective
            add esp, 8
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x10c
            ret
    L004ad13a:
            mov edx, dword ptr [gOpponents]
    L004ad140:
            mov edi, dword ptr [esi + 4]
            cmp edi, 3
            jne L004ad15e
            mov eax, dword ptr [esi + 0x70]
            mov ecx, dword ptr [gTime_stamp_for_this_munging]
            add eax, 0x4e20
            cmp ecx, eax
            jb L004ad73a
    L004ad15e:
            mov ebp, dword ptr [esi + 8]
            xor ecx, ecx
            mov cx, word ptr [ebp + 0x80]
            and cl, 0
            cmp ecx, 0x300
            jne L004ad23c
            mov eax, dword ptr [esi + 0xb0]
            test ah, 8
            je L004ad23c
            fld dword ptr [esi + 0x18]
            fcomp dword ptr [gOppo_pursuit_trail_distance]
            fnstsw ax
            test ah, 1
            je L004ad23c
            cmp edi, 2
            jne L004ad1b1
            cmp dword ptr [esi + 0x114], 0x75bc2c
            je L004ad23c
    L004ad1b1:
            mov eax, dword ptr [esi]
            mov ecx, eax
            shl ecx, 3
            sub ecx, eax
            shl ecx, 6
            mov al, byte ptr [ecx + edx + 0x1b8]
            lea edx, [ecx + edx + 0x1b8]
            cmp al, 0x14
            jbe L004ad1d9
            mov ecx, eax
            and ecx, 0xff
            jmp L004ad1de
    L004ad1d9:
            mov ecx, 0x14
    L004ad1de:
            mov edi, dword ptr [esp + 0x18]
            add ecx, edi
            cmp ecx, 0x64
            jle L004ad1f0
            mov eax, 0x64
            jmp L004ad202
    L004ad1f0:
            cmp al, 0x14
            jbe L004ad1fb
            and eax, 0xff
            jmp L004ad200
    L004ad1fb:
            mov eax, 0x14
    L004ad200:
            add eax, edi
    L004ad202:
            mov byte ptr [edx], al
            mov edx, dword ptr [esi + 8]
            add edx, 0xec
            lea eax, [esp + 0x1c]
            push edx
            push offset gOpponent_nastyness_frigger+0x1138
            push eax
            call sprintf
            mov ecx, dword ptr [esi + 8]
            add esp, 0xc
            add ecx, 0xec
            push ecx
            push offset gOpponent_nastyness_frigger+0x1114
            call DoNotDprintf_opponent
            add esp, 8
            jmp L004ad8f2
    L004ad23c:
            mov ebx, dword ptr [ebp + 0x153c]
            test bl, 2
            je L004ad326
            mov eax, dword ptr [ebp + 0x1560]
            test eax, eax
            je L004ad326
            cmp dword ptr [eax + 0xc], 8
            jne L004ad326
            cmp edi, 2
            jne L004ad274
            cmp dword ptr [esi + 0x114], eax
            je L004ad326
    L004ad274:
            mov eax, dword ptr [esi]
            mov ecx, eax
            shl ecx, 3
            sub ecx, eax
            shl ecx, 6
            mov al, byte ptr [ecx + edx + 0x1b8]
            lea edx, [ecx + edx + 0x1b8]
            cmp al, 0x14
            jbe L004ad29c
            mov ecx, eax
            and ecx, 0xff
            jmp L004ad2a1
    L004ad29c:
            mov ecx, 0x14
    L004ad2a1:
            mov edi, dword ptr [esp + 0x18]
            add ecx, edi
            cmp ecx, 0x64
            jle L004ad2b3
            mov eax, 0x64
            jmp L004ad2c5
    L004ad2b3:
            cmp al, 0x14
            jbe L004ad2be
            and eax, 0xff
            jmp L004ad2c3
    L004ad2be:
            mov eax, 0x14
    L004ad2c3:
            add eax, edi
    L004ad2c5:
            mov byte ptr [edx], al
            mov edx, dword ptr [esi + 8]
            add edx, 0xec
            lea eax, [esp + 0x1c]
            push edx
            push offset gOpponent_nastyness_frigger+0x10f8
            push eax
            call sprintf
            mov eax, dword ptr [esi + 8]
            add esp, 0xc
            mov ecx, dword ptr [eax + 0x1560]
            add eax, 0xec
            add ecx, 0xec
            push ecx
            push eax
            push offset gOpponent_nastyness_frigger+0x10b4
            call DoNotDprintf_opponent
            mov edx, dword ptr [esi + 8]
            add esp, 0xc
            mov eax, dword ptr [edx + 0x1560]
            push eax
            push 2
            push esi
            call NewObjective
            add esp, 0xc
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x10c
            ret
    L004ad326:
            mov eax, dword ptr [ebp + 0x1564]
            test eax, eax
            je L004ad3e6
            cmp dword ptr [eax + 0xc], 8
            jne L004ad3e6
            cmp edi, 2
            jne L004ad34f
            cmp dword ptr [esi + 0x114], eax
            je L004ad3e6
    L004ad34f:
            mov eax, dword ptr [esi]
            mov ecx, eax
            shl ecx, 3
            sub ecx, eax
            shl ecx, 6
            mov al, byte ptr [ecx + edx + 0x1b8]
            lea edx, [ecx + edx + 0x1b8]
            cmp al, 0x14
            jbe L004ad377
            mov ecx, eax
            and ecx, 0xff
            jmp L004ad37c
    L004ad377:
            mov ecx, 0x14
    L004ad37c:
            mov edi, dword ptr [esp + 0x18]
            add ecx, edi
            cmp ecx, 0x64
            jle L004ad38e
            mov eax, 0x64
            jmp L004ad3a0
    L004ad38e:
            cmp al, 0x14
            jbe L004ad399
            and eax, 0xff
            jmp L004ad39e
    L004ad399:
            mov eax, 0x14
    L004ad39e:
            add eax, edi
    L004ad3a0:
            mov byte ptr [edx], al
            mov edx, dword ptr [esi + 8]
            add edx, 0xec
            lea eax, [esp + 0x1c]
            push edx
            push offset gOpponent_nastyness_frigger+0x108c
            push eax
            call sprintf
            mov eax, dword ptr [esi + 8]
            add esp, 0xc
            mov ecx, dword ptr [eax + 0x1564]
            add eax, 0xec
            add ecx, 0xec
            push ecx
            push eax
            push offset gOpponent_nastyness_frigger+0x1050
            call DoNotDprintf_opponent
            add esp, 0xc
            jmp L004ad8f2
    L004ad3e6:
            cmp edi, 2
            jne L004ad3fb
            cmp dword ptr [esi + 0x114], 0x75bc2c
            je L004ad73a
    L004ad3fb:
            test bl, 1
            je L004ad4bb
            cmp ecx, 0x300
            jne L004ad420
            fld dword ptr [esi + 0x18]
            fcomp dword ptr [gOppo_pursuit_trail_distance]
            fnstsw ax
            test ah, 0x41
            je L004ad4bb
    L004ad420:
            mov eax, dword ptr [ebp + 0x1560]
            test eax, eax
            je L004ad4bb
            cmp dword ptr [eax + 0xc], 8
            jne L004ad4bb
            mov eax, dword ptr [esi]
            mov ecx, eax
            shl ecx, 3
            sub ecx, eax
            shl ecx, 6
            mov al, byte ptr [ecx + edx + 0x1b8]
            lea ecx, [ecx + edx + 0x1b8]
            cmp al, 0x14
            jbe L004ad4bb
            mov edx, dword ptr [esp + 0x18]
            and eax, 0xff
            lea edi, [edx + eax]
            cmp edi, 0x64
            jle L004ad46e
            mov eax, 0x64
            jmp L004ad470
    L004ad46e:
            add eax, edx
    L004ad470:
            mov byte ptr [ecx], al
            mov edx, dword ptr [esi + 8]
            add edx, 0xec
            push offset gProgram_state+0x198
            push edx
            lea eax, [esp + 0x24]
            push offset gOpponent_nastyness_frigger+0x1030
            push eax
            call sprintf
            mov eax, dword ptr [esi + 8]
            add esp, 0x10
            mov ecx, dword ptr [eax + 0x1560]
            add eax, 0xec
            add ecx, 0xec
            push ecx
            push eax
            push offset gOpponent_nastyness_frigger+0xfe0
            call DoNotDprintf_opponent
            add esp, 0xc
            jmp L004ad8f2
    L004ad4bb:
            mov eax, dword ptr [esi + 0xb0]
            test ah, 2
            je L004ad73a
            test ah, 4
            jne L004ad73a
            or ah, 4
            xor edx, edx
            mov dword ptr [esi + 0xb0], eax
            mov dx, word ptr [ebp + 0x80]
            and dl, 0
            cmp edx, 0x300
            jne L004ad54b
            mov eax, dword ptr [gProgram_state+0xb4]
            fld dword ptr [eax + 0x68]
            fld dword ptr [eax + 0x6c]
            fld dword ptr [eax + 0x70]
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
            fld dword ptr [gCop_pursuit_speed_percentage_multiplier]
            fxch st(3)
            faddp st(1), st(0)
            fsqrt
            fst dword ptr [esp + 0x10]
            fmul qword ptr [DAT_0058a4b0]
            fsub dword ptr [gDefinite_no_cop_pursuit_speed]
            fxch st(2)
            fmulp st(2), st(0)
            fxch st(1)
            call _ftol
            fstp st(0)
            mov edi, eax
            jmp L004ad598
    L004ad54b:
            mov eax, dword ptr [gProgram_state+0x20]
            mov ecx, dword ptr [gNum_of_opponents_pursuing]
            add eax, 3
            cmp ecx, eax
            jge L004ad596
            mov eax, dword ptr [esi]
            xor edx, edx
            mov ecx, eax
            shl ecx, 3
            sub ecx, eax
            mov eax, dword ptr [gOpponents]
            shl ecx, 6
            mov dl, byte ptr [ecx + eax + 0x1b8]
            sub edx, 0x14
            mov dword ptr [esp + 0x10], edx
            fild dword ptr [esp + 0x10]
            fld dword ptr [esi + 0xc]
            fmul dword ptr [DAT_0058a55c+0xc]
            fsubp st(1), st(0)
            call _ftol
            mov edi, eax
            jmp L004ad598
    L004ad596:
            xor edi, edi
    L004ad598:
            mov ecx, dword ptr [gProgram_state+0xbc]
            mov eax, dword ptr [ebp + 0x10]
            fld dword ptr [ecx + 0x54]
            fld dword ptr [ecx + 0x50]
            fsub dword ptr [eax + 0x50]
            fxch st(1)
            fsub dword ptr [eax + 0x54]
            fld dword ptr [ecx + 0x58]
            fsub dword ptr [eax + 0x58]
            fld st(2)
            fld st(2)
            fxch st(1)
            fmul st(0), st(4)
            fxch st(1)
            fmul st(0), st(3)
            fld st(2)
            fmul st(0), st(3)
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fsqrt
            fcom dword ptr [DAT_0058a4e8]
            fnstsw ax
            test ah, 0x41
            jne L004ad5ee
            fdivr qword ptr [DAT_0058a4a0]
            fld st(0)
            fld st(1)
            fxch st(1)
            fmulp st(5), st(0)
            fmulp st(3), st(0)
            fmulp st(1), st(0)
            jmp L004ad608
    L004ad5ee:
            fstp st(0)
            fstp st(0)
            fstp st(0)
            fstp st(0)
            fld dword ptr [gOppo_pursuit_one]
            fld dword ptr [gOppo_render_zero]
            fld dword ptr [gOppo_render_zero]
    L004ad608:
            fld dword ptr [ebp + 0x78]
            fld dword ptr [ebp + 0x70]
            fld dword ptr [ebp + 0x74]
            fxch st(2)
            fmul st(0), st(3)
            fxch st(1)
            fmul st(0), st(5)
            fxch st(2)
            fmul st(0), st(4)
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fcomp dword ptr [gHead_on_cos_value]
            fnstsw ax
            test ah, 0x41
            jne L004ad67b
            fxch st(2)
            fmul dword ptr [gProgram_state+0x11c]
            fxch st(1)
            fmul dword ptr [gProgram_state+0x120]
            fld dword ptr [gHead_on_cos_value]
            fxch st(3)
            fmul dword ptr [gProgram_state+0x124]
            fxch st(1)
            faddp st(2), st(0)
            fxch st(2)
            fchs
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            fcompp
            fnstsw ax
            test ah, 0x41
            jne L004ad681
            push offset gOpponent_nastyness_frigger+0xfc4
            call DoNotDprintf_opponent
            add esp, 4
            mov eax, 1
            jmp L004ad683
    L004ad67b:
            fstp st(0)
            fstp st(0)
            fstp st(0)
    L004ad681:
            xor eax, eax
    L004ad683:
            lea eax, [eax + eax*4]
            lea ecx, [eax + eax*4]
            lea edi, [edi + ecx*2]
            mov ecx, edi
            call PercentageChance
            mov ebp, eax
            mov eax, offset gOpponent_nastyness_frigger+0xfac
            test ebp, ebp
            jne L004ad6a3
            mov eax, offset gOpponent_nastyness_frigger+0xf90
    L004ad6a3:
            mov edx, dword ptr [esi + 8]
            push eax
            add edx, 0xec
            push edi
            push edx
            push offset gOpponent_nastyness_frigger+0xf60
            call DoNotDprintf_opponent
            add esp, 0x10
            test ebp, ebp
            je L004ad73a
            mov eax, dword ptr [esi]
            mov edx, dword ptr [gOpponents]
            mov ecx, eax
            shl ecx, 3
            sub ecx, eax
            shl ecx, 6
            mov al, byte ptr [ecx + edx + 0x1b8]
            lea edx, [ecx + edx + 0x1b8]
            cmp al, 0x14
            jbe L004ad6ee
            mov ecx, eax
            and ecx, 0xff
            jmp L004ad6f3
    L004ad6ee:
            mov ecx, 0x14
    L004ad6f3:
            mov edi, dword ptr [esp + 0x18]
            add ecx, edi
            cmp ecx, 0x64
            jle L004ad705
            mov eax, 0x64
            jmp L004ad717
    L004ad705:
            cmp al, 0x14
            jbe L004ad710
            and eax, 0xff
            jmp L004ad715
    L004ad710:
            mov eax, 0x14
    L004ad715:
            add eax, edi
    L004ad717:
            mov byte ptr [edx], al
            mov edx, dword ptr [esi + 8]
            add edx, 0xec
            lea eax, [esp + 0x1c]
            push edx
            push offset gOpponent_nastyness_frigger+0xf30
            push eax
            call sprintf
            add esp, 0xc
            jmp L004ad8f2
    L004ad73a:
            mov eax, dword ptr [esp + 0x14]
            test eax, eax
            je L004ad818
            mov eax, dword ptr [esi + 8]
            mov ecx, dword ptr [eax + 0x1560]
            add eax, 0xec
            push ecx
            push eax
            push offset gOpponent_nastyness_frigger+0xefc
            call DoNotDprintf_opponent
            mov eax, dword ptr [esi + 0xb0]
            add esp, 0xc
            test ah, 1
            je L004ad839
            mov edx, dword ptr [esi + 8]
            xor eax, eax
            mov ax, word ptr [edx + 0x80]
            and al, 0
            cmp eax, 0x300
            jne L004ad79d
            push 9
            push esi
            call NewObjective
            add esp, 8
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x10c
            ret
    L004ad79d:
            mov eax, dword ptr [esi + 0xb4]
            dec eax
            je L004ad80d
            dec eax
            je L004ad823
            dec eax
            je L004ad8f2
            fld dword ptr [esi + 0x18]
            call _ftol
            mov edi, eax
            sub edi, 0xf
            mov ecx, edi
            call PercentageChance
            test eax, eax
            push edi
            je L004ad7f6
            mov ecx, dword ptr [esi + 8]
            add ecx, 0xec
            push ecx
            push offset gOpponent_nastyness_frigger+0xec0
            call DoNotDprintf_opponent
            add esp, 0xc
            push 4
            push esi
            call NewObjective
            add esp, 8
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x10c
            ret
    L004ad7f6:
            mov edx, dword ptr [esi + 8]
            add edx, 0xec
            push edx
            push offset gOpponent_nastyness_frigger+0xe80
            call DoNotDprintf_opponent
            add esp, 0xc
    L004ad80d:
            push 1
            push esi
            call NewObjective
            add esp, 8
    L004ad818:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x10c
            ret
    L004ad823:
            push 4
            push esi
            call NewObjective
            add esp, 8
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x10c
            ret
    L004ad839:
            mov ecx, dword ptr [esi + 8]
            xor edx, edx
            mov dx, word ptr [ecx + 0x80]
            and dl, 0
            cmp edx, 0x300
            jne L004ad866
            push 8
            push esi
            call NewObjective
            add esp, 8
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x10c
            ret
    L004ad866:
            test al, 0x10
            je L004ad80d
            mov eax, dword ptr [esi]
            mov edx, dword ptr [gOpponents]
            mov ecx, eax
            shl ecx, 3
            sub ecx, eax
            shl ecx, 6
            lea edi, [ecx + edx + 0x1b8]
            mov cl, byte ptr [ecx + edx + 0x1b8]
            cmp cl, 0x14
            jbe L004ad898
            mov eax, ecx
            and eax, 0xff
            jmp L004ad89d
    L004ad898:
            mov eax, 0x14
    L004ad89d:
            fld dword ptr [esi + 0xc]
            fmul dword ptr [DAT_0058a55c+0x4]
            fld dword ptr [DAT_0058a47c]
            add eax, 0x14
            mov dword ptr [esp + 0x14], eax
            fild dword ptr [esp + 0x14]
            fsub st(0), st(2)
            fxch st(1)
            fcompp
            fnstsw ax
            test ah, 1
            je L004ad8ce
            fstp st(0)
            fld dword ptr [DAT_0058a47c]
            jmp L004ad8eb
    L004ad8ce:
            cmp cl, 0x14
            jbe L004ad8db
            and ecx, 0xff
            jmp L004ad8e0
    L004ad8db:
            mov ecx, 0x14
    L004ad8e0:
            add ecx, 0x14
            mov dword ptr [esp + 0x14], ecx
            fisubr dword ptr [esp + 0x14]
    L004ad8eb:
            call _ftol
            mov byte ptr [edi], al
    L004ad8f2:
            push offset gProgram_state+0xac
            push 2
            push esi
            call NewObjective
            add esp, 0xc
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x10c
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004aae20
void C2_NAKED C2_HOOK_FASTCALL CalcRaceRoute(tOpponent_spec *pOpponent_spec) {
#ifndef CARPOCALYPSE2_MATCHING
    tS16 section_no;
    tS16 node_no;
    tS16 race_section_count;
    tS16 normal_section_ok_direction_count;
    tS16 normal_section_wrong_direction_count;
    tS16 temp_section_array[8];
    br_scalar distance;
    br_vector3 direction_v;
    br_vector3 intersect;
    int i;

    if (pOpponent_spec->nnext_sections >= CARPOCALYPSE2_ASIZE(pOpponent_spec->next_sections)) {
        DoNotDprintf_opponent("%s: CalcRaceRoute() - Pissing off 'cos projected route full up", pOpponent_spec->car_spec->driver_name);
        return;
    }
    if (pOpponent_spec->nnext_sections == 0) {
        DoNotDprintf_opponent("%s: CalcRaceRoute() - Projected route empty; starting from nearest section", pOpponent_spec->car_spec->driver_name);
        pOpponent_spec->complete_race_data.finished_calcing_race_route = 0;
        pOpponent_spec->complete_race_data.found_race_section = 0;
        section_no = FindNearestPathSection(&pOpponent_spec->car_spec->car_master_actor->t.t.translate.t, &direction_v, &intersect, &distance);
        if (section_no < 0) {
            return;
        }
        AddToOpponentsProjectedRoute(pOpponent_spec, section_no, 1);
        if (gProgram_state.AI_vehicles.path_sections[section_no].type == ePST_race_path) {
            pOpponent_spec->complete_race_data.found_race_section = 1;
        }
    }
    while (pOpponent_spec->nnext_sections < CARPOCALYPSE2_ASIZE(pOpponent_spec->next_sections) && !pOpponent_spec->complete_race_data.finished_calcing_race_route) {
        node_no = gProgram_state.AI_vehicles.path_sections[pOpponent_spec->next_sections[pOpponent_spec->nnext_sections - 1].section_no].node_indices[pOpponent_spec->next_sections[pOpponent_spec->nnext_sections - 1].direction];
        race_section_count = 0;
        normal_section_ok_direction_count = 0;
        normal_section_wrong_direction_count = 0;
        for (i = 0; i < gProgram_state.AI_vehicles.path_nodes[node_no].number_of_sections; i++) {
            section_no = gProgram_state.AI_vehicles.path_nodes[node_no].sections[i];
            if (pOpponent_spec->next_sections[pOpponent_spec->nnext_sections - 1].section_no != section_no) {
                if (gProgram_state.AI_vehicles.path_sections[section_no].type == ePST_race_path && gProgram_state.AI_vehicles.path_sections[section_no].node_indices[0] == node_no) {
                    pOpponent_spec->complete_race_data.found_race_section = 1;
                    temp_section_array[race_section_count] = section_no;
                    race_section_count += 1;
                } else if (race_section_count == 0 && gProgram_state.AI_vehicles.path_sections[section_no].node_indices[0] == node_no) {
                    temp_section_array[normal_section_ok_direction_count] = section_no;
                    normal_section_ok_direction_count++;
                } else if (race_section_count == 0 && normal_section_ok_direction_count == 0 && (!gProgram_state.AI_vehicles.path_sections[section_no].one_way || gProgram_state.AI_vehicles.path_sections[section_no].node_indices[1] != node_no)) {
                    temp_section_array[normal_section_wrong_direction_count] = section_no;
                    normal_section_wrong_direction_count++;
                }
            }
        }
        if (race_section_count != 0) {
            AddToOpponentsProjectedRoute(pOpponent_spec, temp_section_array[IRandomBetween(0, race_section_count - 1)], 1);
        } else if (normal_section_ok_direction_count != 0) {
            AddToOpponentsProjectedRoute(pOpponent_spec, temp_section_array[IRandomBetween(0, normal_section_ok_direction_count - 1)], 1);
        } else if (normal_section_wrong_direction_count != 0) {
            AddToOpponentsProjectedRoute(pOpponent_spec, temp_section_array[IRandomBetween(0, normal_section_wrong_direction_count - 1)], 1);
        } else if (!pOpponent_spec->complete_race_data.found_race_section) {
            AddToOpponentsProjectedRoute(pOpponent_spec, pOpponent_spec->next_sections[pOpponent_spec->nnext_sections - 1].section_no, pOpponent_spec->next_sections[pOpponent_spec->nnext_sections - 1].direction == 0);
        } else {
            pOpponent_spec->complete_race_data.finished_calcing_race_route = 1;
        }
    }
#else
    __asm {
            sub esp, 0x40
            push ebx
            push ebp
            push esi
            mov esi, ecx
            push edi
            mov eax, dword ptr [esi + 0x7c]
            cmp eax, 0xc
            jl L004aae4f
            mov eax, dword ptr [esi + 8]
            add eax, 0xec
            push eax
            push offset gOpponent_nastyness_frigger+0x674
            call DoNotDprintf_opponent
            add esp, 8
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x40
            ret
    L004aae4f:
            xor edi, edi
            mov bl, 1
            cmp eax, edi
            jne L004aaee0
            mov ecx, dword ptr [esi + 8]
            add ecx, 0xec
            push ecx
            push offset gOpponent_nastyness_frigger+0x628
            call DoNotDprintf_opponent
            add esp, 8
            lea edx, [esp + 0x24]
            lea eax, [esp + 0x28]
            mov byte ptr [esi + 0xb8], 0
            push edx
            mov edx, dword ptr [esi + 8]
            mov byte ptr [esi + 0xb9], 0
            lea ecx, [esp + 0x38]
            mov edx, dword ptr [edx + 0x10]
            push eax
            push ecx
            add edx, 0x50
            xor ecx, ecx
            call FindNearestGeneralSection
            cmp ax, di
            jl L004ab0f4
            mov ecx, dword ptr [esi + 0x7c]
            cmp ecx, 0xc
            jge L004aaec9
            mov word ptr [esi + ecx*4 + 0x80], ax
            mov ecx, dword ptr [esi + 0x7c]
            mov byte ptr [esi + ecx*4 + 0x82], bl
            mov ecx, dword ptr [esi + 0x7c]
            inc ecx
            mov dword ptr [esi + 0x7c], ecx
    L004aaec9:
            movsx eax, ax
            lea edx, [eax + eax*4]
            mov eax, dword ptr [gProgram_state+0x5ec4]
            cmp byte ptr [eax + edx*4 + 0x10], bl
            jne L004aaee0
            mov byte ptr [esi + 0xb9], bl
    L004aaee0:
            mov ecx, dword ptr [esi + 0x7c]
            cmp ecx, 0xc
            jge L004ab0f4
            jmp L004aaef0
    L004aaeee:
            xor edi, edi
    L004aaef0:
            mov al, byte ptr [esi + 0xb8]
            test al, al
            jne L004ab0f4
            movsx eax, word ptr [esi + ecx*4 + 0x7c]
            mov ebx, dword ptr [gProgram_state+0x5ec0]
            mov dword ptr [esp + 0x10], edi
            lea edx, [eax + eax*4]
            xor eax, eax
            mov al, byte ptr [esi + ecx*4 + 0x7e]
            mov dword ptr [esp + 0x18], edi
            lea edx, [eax + edx*2]
            mov eax, dword ptr [gProgram_state+0x5ec4]
            mov ax, word ptr [eax + edx*2]
            xor edx, edx
            mov dword ptr [esp + 0x14], eax
            mov dword ptr [esp + 0x1c], edx
            movsx eax, ax
            mov ebp, eax
            shl ebp, 5
            cmp byte ptr [ebx + ebp + 0x1c], dl
            jbe L004ab002
            shl eax, 4
            mov dword ptr [esp + 0x20], eax
    L004aaf49:
            mov eax, dword ptr [esp + 0x20]
            movsx edx, dx
            add edx, eax
            mov ax, word ptr [ebx + edx*2 + 0xc]
            cmp ax, word ptr [esi + ecx*4 + 0x7c]
            je L004aafe4
            mov ebx, dword ptr [gProgram_state+0x5ec4]
            movsx edx, ax
            lea edx, [edx + edx*4]
            lea edx, [ebx + edx*4]
            cmp byte ptr [edx + 0x10], 1
            jne L004aaf93
            mov bx, word ptr [esp + 0x14]
            cmp bx, word ptr [edx]
            jne L004aaf93
            movsx edx, di
            mov byte ptr [esi + 0xb9], 1
            inc edi
            mov word ptr [esp + edx*2 + 0x40], ax
            jmp L004aafe4
    L004aaf93:
            test di, di
            jne L004aafe4
            mov bx, word ptr [esp + 0x14]
            cmp bx, word ptr [edx]
            jne L004aafb5
            mov edx, dword ptr [esp + 0x10]
            movsx ebx, dx
            inc edx
            mov word ptr [esp + ebx*2 + 0x40], ax
            mov dword ptr [esp + 0x10], edx
            jmp L004aafe4
    L004aafb5:
            test di, di
            jne L004aafe4
            cmp word ptr [esp + 0x10], di
            jne L004aafe4
            mov bl, byte ptr [edx + 0x11]
            test bl, bl
            je L004aafd3
            mov bx, word ptr [esp + 0x14]
            cmp bx, word ptr [edx + 2]
            je L004aafe4
    L004aafd3:
            mov edx, dword ptr [esp + 0x18]
            movsx ebx, dx
            inc edx
            mov word ptr [esp + ebx*2 + 0x40], ax
            mov dword ptr [esp + 0x18], edx
    L004aafe4:
            mov ebx, dword ptr [gProgram_state+0x5ec0]
            mov edx, dword ptr [esp + 0x1c]
            inc edx
            movzx ax, byte ptr [ebx + ebp + 0x1c]
            cmp dx, ax
            mov dword ptr [esp + 0x1c], edx
            jl L004aaf49
    L004ab002:
            test di, di
            je L004ab03b
            movsx edx, di
            dec edx
            xor ecx, ecx
            call IRandomBetween
            mov ecx, dword ptr [esi + 0x7c]
            mov ax, word ptr [esp + eax*2 + 0x40]
            cmp ecx, 0xc
            jge L004ab0e8
            mov word ptr [esi + ecx*4 + 0x80], ax
            mov ecx, dword ptr [esi + 0x7c]
            mov byte ptr [esi + ecx*4 + 0x82], 1
            jmp L004ab0e5
    L004ab03b:
            mov eax, dword ptr [esp + 0x10]
            test ax, ax
            je L004ab075
            movsx edx, ax
            dec edx
            xor ecx, ecx
            call IRandomBetween
            mov ecx, dword ptr [esi + 0x7c]
            mov ax, word ptr [esp + eax*2 + 0x40]
            cmp ecx, 0xc
            jge L004ab0e8
            mov word ptr [esi + ecx*4 + 0x80], ax
            mov edx, dword ptr [esi + 0x7c]
            mov byte ptr [esi + edx*4 + 0x82], 1
            jmp L004ab0e5
    L004ab075:
            mov eax, dword ptr [esp + 0x18]
            test ax, ax
            je L004ab0ab
            movsx edx, ax
            dec edx
            xor ecx, ecx
            call IRandomBetween
            mov ecx, dword ptr [esi + 0x7c]
            mov ax, word ptr [esp + eax*2 + 0x40]
            cmp ecx, 0xc
            jge L004ab0e8
            mov word ptr [esi + ecx*4 + 0x80], ax
            mov eax, dword ptr [esi + 0x7c]
            mov byte ptr [esi + eax*4 + 0x82], 0
            jmp L004ab0e5
    L004ab0ab:
            mov al, byte ptr [esi + 0xb9]
            test al, al
            je L004ab0be
            mov byte ptr [esi + 0xb8], 1
            jmp L004ab0e8
    L004ab0be:
            mov bl, byte ptr [esi + ecx*4 + 0x7e]
            mov dx, word ptr [esi + ecx*4 + 0x7c]
            xor eax, eax
            test bl, bl
            sete al
            cmp ecx, 0xc
            jge L004ab0e8
            mov word ptr [esi + ecx*4 + 0x80], dx
            mov ecx, dword ptr [esi + 0x7c]
            mov byte ptr [esi + ecx*4 + 0x82], al
    L004ab0e5:
            inc dword ptr [esi + 0x7c]
    L004ab0e8:
            mov ecx, dword ptr [esi + 0x7c]
            cmp ecx, 0xc
            jl L004aaeee
    L004ab0f4:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x40
            ret
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004b18b0
void C2_NAKED C2_HOOK_FASTCALL SetMaxSpeedFromSOCs(tSOC* pSocs, int pCount_socs, float* pDesired_speed, float pSpeed, const br_vector2* pPos2d, tCorner* pCorners, tOpponent_spec* pOpponent_spec) {
#ifndef CARPOCALYPSE2_MATCHING
    br_vector2 previous;
    br_vector2 position;
    const br_vector3* point;
    int i;
    int speed;
    int previous_corner = 0;
    float previous_extent;
    float distance = 0.f;
    float speed_squared;
    float stopping_distance;
    double x, y, length, offset, threshold;

    speed_squared = NavigationScalarRound(NavigationMultiply(pSpeed, pSpeed));
    memcpy(&previous, pPos2d, sizeof(previous));
    threshold = NavigationSubtract(NavigationMultiply(speed_squared, 0.0625f), NavigationMultiply(NavigationMultiply((double)gFrame_period_for_this_munging, pSpeed), -0.001f));
    stopping_distance = NavigationScalarRound(NavigationMultiply(threshold, 0.14492753148078918f));
    for (i = 0; i < pCount_socs; i++) {
        if (!NavigationLessOrUnordered(distance, stopping_distance)) {
            return;
        }
        if (previous_corner) {
            distance = NavigationScalarRound(NavigationSubtract(distance, previous_extent));
        }
        previous_corner = !pSocs[i].field_0x0;
        offset = distance;
        if (previous_corner) {
            point = GetOpponentsSectionStartNodePoint(pOpponent_spec, (tS16)pCorners[pSocs[i].field_0x4].section);
            speed = (int)pCorners[pSocs[i].field_0x4].field_0x18;
            memcpy(&previous_extent, &pCorners[pSocs[i].field_0x4].field_0x0c, 4);
            offset = NavigationSubtract(distance, previous_extent);
        } else {
            point = GetOpponentsSectionFinishNodePoint(pOpponent_spec, (tS16)pSocs[i].field_0x4);
            speed = GetOpponentsSectionMaxSpeed(pOpponent_spec, (tS16)pSocs[i].field_0x4, 1);
        }
        memcpy(&position.v[0], &point->v[2], 4);
        memcpy(&position.v[1], &point->v[0], 4);
        x = NavigationFloatDifference(&previous.v[0], &position.v[0]);
        y = NavigationFloatDifference(&previous.v[1], &position.v[1]);
        length = NavigationSqrt(NavigationAdd(NavigationMultiply(x, NavigationScalarRound(x)), NavigationMultiply(y, y)));
        distance = NavigationScalarRound(NavigationAdd(length, offset));
        if (NavigationLessOrUnordered(distance, 0.f)) {
            distance = 0.f;
        }
        threshold = NavigationMultiply(NavigationSubtract(speed_squared, NavigationMultiply((double)speed, (double)speed)), 0.0625f);
        threshold = NavigationSubtract(threshold, NavigationMultiply(NavigationMultiply((double)gFrame_period_for_this_munging, pSpeed), -0.001f));
        if (NavigationOrderedGreater(threshold, NavigationMultiply(distance, 6.9f)) && NavigationLessOrUnordered((double)speed, *pDesired_speed)) {
            *pDesired_speed = NavigationScalarRound((double)speed);
        }
        memcpy(&previous, &position, sizeof(previous));
    }
#else
    __asm {
            sub esp, 0x34
            fld dword ptr [esp + 0x3c]
            mov eax, dword ptr [gFrame_period_for_this_munging]
            push ebx
            fmul dword ptr [esp + 0x40]
            xor ebx, ebx
            mov dword ptr [esp + 0x20], eax
            mov dword ptr [esp + 0x24], ebx
            mov eax, dword ptr [esp + 0x44]
            fild qword ptr [esp + 0x20]
            fld dword ptr [gFLOAT_0058a61c]
            fxch st(2)
            fstp dword ptr [esp + 4]
            push ebp
            push esi
            fmul dword ptr [esp + 0x48]
            fld dword ptr [esp + 0xc]
            fmul dword ptr [gFLOAT_0058a69c]
            fxch st(1)
            mov esi, dword ptr [eax]
            mov eax, dword ptr [eax + 4]
            fmul dword ptr [gFLOAT_0058a6a0]
            cmp edx, ebx
            push edi
            fsubp st(1), st(0)
            fxch st(1)
            fstp dword ptr [esp + 0x50]
            mov dword ptr [esp + 0x20], edx
            mov dword ptr [esp + 0x2c], esi
            fmul dword ptr [gFLOAT_0058a6a4]
            mov dword ptr [esp + 0x30], eax
            mov dword ptr [esp + 0x14], ebx
            fstp dword ptr [esp + 0x18]
            jle L004b1aa3
            mov ebp, dword ptr [esp + 0x58]
            lea esi, [ecx + 4]
    L004b192e:
            fld dword ptr [esp + 0x50]
            fcomp dword ptr [esp + 0x18]
            fnstsw ax
            test ah, 1
            je L004b1aa3
            test ebx, ebx
            je L004b1951
            fld dword ptr [esp + 0x50]
            fsub dword ptr [esp + 0x58]
            fstp dword ptr [esp + 0x50]
    L004b1951:
            test byte ptr [esi - 4], 1
            je L004b197f
            mov dx, word ptr [esi]
            mov ecx, ebp
            xor ebx, ebx
            call GetOpponentsSectionFinishNodePoint
            mov dx, word ptr [esi]
            mov ecx, ebp
            push 1
            mov edi, eax
            call GetOpponentsSectionMaxSpeed
            mov dword ptr [esp + 0x1c], eax
            fild dword ptr [esp + 0x1c]
            fld dword ptr [esp + 0x50]
            jmp L004b19c2
    L004b197f:
            mov eax, dword ptr [esi]
            mov edx, dword ptr [esp + 0x54]
            mov ecx, eax
            mov ebx, 1
            shl ecx, 3
            sub ecx, eax
            mov dx, word ptr [edx + ecx*4 + 0x14]
            mov ecx, ebp
            call GetOpponentsSectionStartNodePoint
            mov edx, dword ptr [esp + 0x54]
            mov edi, eax
            mov eax, dword ptr [esi]
            mov ecx, eax
            shl ecx, 3
            sub ecx, eax
            fild dword ptr [edx + ecx*4 + 0x18]
            fld dword ptr [esp + 0x50]
            lea eax, [edx + ecx*4]
            mov eax, dword ptr [eax + 0xc]
            mov dword ptr [esp + 0x58], eax
            fsub dword ptr [esp + 0x58]
    L004b19c2:
            fld dword ptr [esp + 0x30]
            mov ecx, dword ptr [edi + 8]
            mov edx, dword ptr [edi]
            fld dword ptr [esp + 0x2c]
            mov dword ptr [esp + 0x24], ecx
            mov dword ptr [esp + 0x28], edx
            fsub dword ptr [esp + 0x24]
            fxch st(1)
            fsub dword ptr [esp + 0x28]
            fxch st(1)
            fst dword ptr [esp + 0x34]
            fld st(1)
            fxch st(1)
            fmul dword ptr [esp + 0x34]
            fxch st(1)
            fmul st(0), st(2)
            faddp st(1), st(0)
            fsqrt
            fstp st(1)
            fadd st(0), st(1)
            fstp dword ptr [esp + 0x50]
            fstp st(0)
            fld dword ptr [esp + 0x50]
            fcomp dword ptr [gFLOAT_0058a61c]
            fnstsw ax
            test ah, 1
            je L004b1a1a
            mov dword ptr [esp + 0x50], 0
    L004b1a1a:
            mov eax, dword ptr [gFrame_period_for_this_munging]
            mov dword ptr [esp + 0x40], 0
            fld st(0)
            fmul st(0), st(1)
            mov dword ptr [esp + 0x3c], eax
            mov ecx, dword ptr [esp + 0x24]
            fild qword ptr [esp + 0x3c]
            fxch st(1)
            fsubr dword ptr [esp + 0x10]
            fxch st(1)
            fmul dword ptr [esp + 0x4c]
            fld dword ptr [esp + 0x50]
            fxch st(2)
            fmul dword ptr [gFLOAT_0058a69c]
            fxch st(1)
            mov edx, dword ptr [esp + 0x28]
            mov dword ptr [esp + 0x2c], ecx
            fmul dword ptr [gFLOAT_0058a6a0]
            fxch st(2)
            fmul dword ptr [gFLOAT_0058a620]
            fxch st(2)
            fsubp st(1), st(0)
            mov dword ptr [esp + 0x30], edx
            fcompp
            fnstsw ax
            test ah, 0x41
            jne L004b1a89
            mov ecx, dword ptr [esp + 0x48]
            fcom dword ptr [ecx]
            fnstsw ax
            test ah, 1
            je L004b1a89
            fstp dword ptr [ecx]
            jmp L004b1a8b
    L004b1a89:
            fstp st(0)
    L004b1a8b:
            mov eax, dword ptr [esp + 0x14]
            mov ecx, dword ptr [esp + 0x20]
            inc eax
            add esi, 8
            cmp eax, ecx
            mov dword ptr [esp + 0x14], eax
            jl L004b192e
    L004b1aa3:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x34
            ret 0x14
            nop
            nop
            nop
    }
#endif
}

int C2_HOOK_FASTCALL TimeToBeBrakingWhenStruggling(tOpponent_spec* pOpponent_spec) {

    return gTime_stamp_for_this_munging >
        pOpponent_spec->follow_path_data.struggle_time + 2000
            + 15.f * pOpponent_spec->car_spec->collision_info->M
            + 750 * (pOpponent_spec->follow_path_data.number_of_struggles - 1);
}

void C2_HOOK_FASTCALL SetCurvature(tCar_spec* pCar_spec, float pCurvature) {

    float error = pCurvature - pCar_spec->curvature;
    float max_error = gFrame_period_for_this_munging * 5.f / 1000.f;
    if (error > max_error) {
        pCar_spec->curvature += max_error;
    } else if (error < -max_error) {
        pCar_spec->curvature -= max_error;
    } else {
        pCar_spec->curvature = pCurvature;
    }
}

br_scalar C2_HOOK_FASTCALL Distance2D(const br_vector3* pV1, const br_vector3* pV2) {
    br_vector2 v;

    BrVector2Set(&v, pV1->v[2] - pV2->v[2], pV1->v[0] - pV2->v[0]);
    return BrVector2Length(&v);
}

br_scalar C2_HOOK_FASTCALL SectionLength2D(const tOpponent_spec* pOpponent_spec, int pSection) {

    return Distance2D(
        GetOpponentsSectionStartNodePoint(pOpponent_spec, pSection),
        GetOpponentsSectionFinishNodePoint(pOpponent_spec, pSection));
}

int C2_HOOK_FASTCALL NearestSectionInStraight(const br_vector2* pPos2d, const br_vector2* pStart2d, const br_vector2* pFinish2d, tS16 pSection_no, tS16 pEnd_section, const tOpponent_spec* pOpponent_spec) {
    br_vector2 dir;
    br_vector2 rel_pos;
    br_scalar cumLength;
    br_scalar unknown;

    BrVector2Sub(&dir, pFinish2d, pStart2d);
    BrVector2Sub(&rel_pos, pPos2d, pStart2d);
    unknown = BrVector2Dot(&dir, &rel_pos) / BrVector2Dot(&dir, &dir);
    cumLength = 0.f;
    for (;;) {
        tS16 prev_section;
        int smaller;

        smaller = 0;
        prev_section = pSection_no;
        pSection_no = GetOpponentsNextSection(pOpponent_spec, prev_section);
#ifndef CARPOCALYPSE2_MATCHING
        if (pSection_no < 0) {
            return prev_section;
        }
#endif
        if (pSection_no != pEnd_section) {
            cumLength += SectionLength2D(pOpponent_spec, prev_section);
            if (cumLength < unknown) {
                smaller = 1;
            }
        }
        if (!smaller) {
            return prev_section;
        }
    }
}

void C2_HOOK_FASTCALL StuffDataFromCorner(tFollow_path_data* pFollow_path, tCorner *pCorner) {

    pFollow_path->left_not_right = pCorner->left_not_right;
    pFollow_path->cornering = 1;
    BrVector2Copy(&pFollow_path->field_0x40, &pCorner->field_0x00);
    pFollow_path->field_0x48 = pCorner->field_0x08;
    pFollow_path->field_0x4c = pCorner->field_0x0c;
    pFollow_path->corner_section = pCorner->section;
}

float C2_HOOK_FASTCALL MaxCurvatureForCarSpeed(const tCar_spec* pCar, float pSpeed) {
    float max_curve;

    max_curve = pCar->maxcurve;
    if (pSpeed >= 12.5) {
        max_curve = pCar->maxcurve * 12.5f / pSpeed;
    }
    return max_curve;
}

// FUNCTION: CARMA2_HW 0x004af960
tFollow_path_result C2_NAKED C2_HOOK_FASTCALL ProcessFollowPath(tOpponent_spec* pOpponent_spec, tProcess_objective_command pCommand, int pPursuit_mode, int pIgnore_end, int pNever_struggle) {
#ifndef CARPOCALYPSE2_MATCHING
    tFollow_path_data* data;
    tCorner corners[4];
    tSOC socs[10];
    br_vector3 wank;
    br_vector3 section_dir;
    br_vector3 section_v;
    br_vector3 goal_dir;
    br_actor* car_master_actor;
    br_scalar stopped_speed;
    br_scalar t;
    br_scalar acc;
    br_scalar speed;
    br_scalar dist_along;
    br_scalar acc_factor;
    br_scalar max_acc;
    br_scalar error;
    tCar_spec* car_spec;
    int engine_damage;
    int trans_damage;
    br_vector2 oppo_pos2d;
    br_vector2 start2d;
    br_vector2 finish2d;
    br_vector2 oppo_pos_rel;
    br_vector2 oppo_pos_rel_next;
    br_vector2 v2d;
    br_scalar width;
    br_scalar effective_speed_factor;
    int count_corners;
    int count_socs;
    int straight_section_no;
    br_scalar unk1;
    br_scalar unk2;
    br_scalar unk3;
    br_scalar curv;

    car_spec = pOpponent_spec->car_spec;
    engine_damage = car_spec->damage_units[eDamage_engine].damage_level;
    trans_damage = car_spec->damage_units[eDamage_transmission].damage_level;
    data = &pOpponent_spec->follow_path_data;
    car_master_actor = car_spec->car_master_actor;

    switch (pCommand) {
    case ePOC_start:
        data->first_section_no = GetOpponentsFirstSection(pOpponent_spec);
        data->section_no = data->first_section_no;
        DoNotDprintf_oppoproc("%s: ProcessFollowPath() - new task started, first real section #%d", pOpponent_spec->car_spec->driver_name, GetOpponentsRealSection(pOpponent_spec, data->first_section_no));
        data->has_moved_during_this_task = 0;
        data->struggle_time = 0;
        data->last_finished_struggle_time = gTime_stamp_for_this_munging;
        data->prev_acc = 0.f;
        data->prev_acc_error = 0.f;
        data->borrowed_time_start = gTime_stamp_for_this_munging;
        data->last_struggle_section = -1;
        data->made_it = 1;
        data->cheating = 0;
        data->cornering = 0;
        if (!pOpponent_spec->cheating && !pOpponent_spec->physics_me) {
            DoNotDprintf_oppoproc("%s: Rematerialising from ePOC_start in ProcessFollowPath().+..", pOpponent_spec->car_spec->driver_name);
            /* speed is not initialized in exe */
            RematerialiseOpponentOnNearestSection(pOpponent_spec, BrVector3Length(&car_spec->collision_info->v));
        }
        return eFPR_OK;
    case ePOC_run:
        if (data->cheating || pOpponent_spec->cheating != data->cheating) {
            return FollowCheatyPath(pOpponent_spec);
        }
        if (!pIgnore_end && !data->made_it && gTime_stamp_for_this_munging > data->borrowed_time_start + 1000 && gTime_stamp_for_this_munging < data->borrowed_time_start + 10000) {
            BrVector3Sub(&section_dir, GetOpponentsSectionFinishNodePoint(pOpponent_spec, data->section_no), GetOpponentsSectionStartNodePoint(pOpponent_spec, data->section_no));
            BrVector3Sub(&goal_dir, &car_master_actor->t.t.translate.t, GetOpponentsSectionStartNodePoint(pOpponent_spec, data->section_no));
            dist_along = BrVector3Dot(&section_dir, &goal_dir) / BrVector3LengthSquared(&section_dir);
            BrVector3Scale(&section_v, &section_dir, dist_along);
            BrVector3Sub(&wank, &goal_dir, &section_v);
            if (GetOpponentsSectionWidth(pOpponent_spec, data->section_no) >= BrVector3Length(&wank)) {
                data->made_it = 1;
            }
        }
        if (gTime_stamp_for_this_munging > data->borrowed_time_start + 10000 && !data->made_it) {
            DoNotDprintf_oppoproc("%s: ProcessFollowPath() giving up due to not making the corner", pOpponent_spec->car_spec->driver_name);
            return eFPR_given_up;
        }
        car_spec->keys.acc = 1;
        speed = WORLD_SCALE * BrVector3Length(&car_spec->collision_info->v);
        if (speed > 0.2f) {
            data->has_moved_during_this_task = 1;
            pOpponent_spec->has_moved_at_some_point = 1;
        }
        if (data->struggle_time != 0) {

            if (!TimeToStopStruggling(pOpponent_spec)) {
                if (TimeToBeBrakingWhenStruggling(pOpponent_spec)) {
                    car_spec->acc_force = 0.f;
                    car_spec->brake_force = 15.f * car_spec->collision_info->M;
                    SetCurvature(car_spec, 0.f);
                } else {
                    car_spec->brake_force = 0.f;
                    car_spec->acc_force = -6.f * car_spec->collision_info->M;
                    SetCurvature(car_spec, 0.f);
                }
                return eFPR_OK;
            }
            DoNotDprintf_oppoproc("%s: done struggling. speed = %.2f m/s", pOpponent_spec->car_spec->driver_name, speed);
            data->made_it = 0;
            data->borrowed_time_start = gTime_stamp_for_this_munging;
            data->struggle_time = 0;
            data->last_finished_struggle_time = gTime_stamp_for_this_munging;
            car_spec->brake_force = 0.0f;
            car_spec->acc_force = 0.0f;
        } else {
            stopped_speed = (pIgnore_end ? 2.f : 6.f) / 30.f;
            if (!pNever_struggle && stopped_speed >= speed && gTime_stamp_for_this_munging > data->last_finished_struggle_time + 2000 && (pPursuit_mode || data->has_moved_during_this_task)) {
                DoNotDprintf_oppoproc("%s: 'Stopped,' section #%d, speed = %.2f m/s, about to start a-strugglin'", pOpponent_spec->car_spec->driver_name, data->section_no, speed);
                data->struggle_time = gTime_stamp_for_this_munging;
                if (pIgnore_end || data->section_no != data->last_struggle_section) {
                    data->last_struggle_section = data->section_no;
                    data->number_of_struggles = 0;
                } else {
                    if (data->number_of_struggles >= 3) {
                        car_spec->acc_force = 0.0f;
                        car_spec->brake_force = 0.0f;
                        DoNotDprintf_oppoproc("%s: Giving up trying to follow path 'cos we've struggled too much", pOpponent_spec->car_spec->driver_name);
                        return eFPR_given_up;
                    }
                    data->number_of_struggles += 1;
                }
            }
        }

        straight_section_no = GetStraight(&start2d, &finish2d, &width, pOpponent_spec->follow_path_data.section_no, pOpponent_spec);
        BrVector2Set(&oppo_pos2d,
            car_spec->car_master_actor->t.t.translate.t.v[2],
            car_spec->car_master_actor->t.t.translate.t.v[0]);
        BrVector2Sub(&oppo_pos_rel_next, &oppo_pos2d, &finish2d);
        BrVector2Set(&v2d,
            car_spec->collision_info->v.v[2],
            car_spec->collision_info->v.v[0]);
        BrVector2Scale(&v2d, &v2d, WORLD_SCALE);
        speed = BrVector2Length(&v2d);
        if (!data->cornering) {
            tS16 nearest_straight_section;

            count_corners = CalcCorners(corners, straight_section_no, width, &start2d, pOpponent_spec);
            nearest_straight_section = NearestSectionInStraight(&oppo_pos2d, &start2d, &finish2d, pOpponent_spec->follow_path_data.section_no, straight_section_no, pOpponent_spec);
            count_socs = CalcSOCs(nearest_straight_section, count_corners, corners, pOpponent_spec, socs, pOpponent_spec->car_spec);
            if (count_corners != 0 && gFrame_period_for_this_munging * speed * .0005f > WORLD_SCALE * (BrVector2Length(&oppo_pos_rel_next) - corners[0].field_0x0c)) {

                StuffDataFromCorner(data, &corners[0]);
                pOpponent_spec->follow_path_data.corner_width = GetOpponentsSectionWidth(pOpponent_spec, corners[0].section);
            } else {
                data->desired_speed = 80.f;
                SetMaxSpeedFromSOCs(socs, count_socs, &data->desired_speed, speed, &oppo_pos2d, &corners[0], pOpponent_spec);
            }
        } else {
            if (BrVector2LengthSquared(&oppo_pos_rel_next) <= CARPOCALYPSE2_SQR(data->field_0x4c)) {

                count_corners = CalcCorners(corners, straight_section_no, width, &start2d, pOpponent_spec);
                count_socs = CalcSOCs(corners[0].section, count_corners, corners, pOpponent_spec, socs, pOpponent_spec->car_spec);
                if (corners[0].field_0x18 > 80.f) {
                    data->desired_speed = 80.f;
                } else {
                    data->desired_speed = (float)corners[0].field_0x18;
                }
                SetMaxSpeedFromSOCs(socs, count_socs, &data->desired_speed, speed, &oppo_pos2d, &corners[0], pOpponent_spec);
            } else {
                data->cornering = 0;
                data->section_no = data->corner_section;
                if (data->section_no >= 20000) {
                    GetStraight(&start2d, &finish2d, &width, data->section_no, pOpponent_spec);
                }
            }
        }
        if (data->cornering) {
            br_vector2 delta;

            BrVector2Sub(&delta, &oppo_pos2d, &data->field_0x40);
            unk1 = (BrVector2Length(&delta) - data->field_0x48) / data->corner_width;
            if (speed > BR_SCALAR_EPSILON) {
                unk2 = BrVector2Dot(&v2d, &delta) / data->field_0x48 * speed;
            } else {
                unk2 = 0.f;
            }
            unk3 = 1.f / (WORLD_SCALE * data->field_0x48);
            if (data->left_not_right) {
                unk2 = -unk2;
            } else {
                unk1 = -unk1;
                unk3 = -unk3;
            }
        } else {
            br_scalar l;
            br_vector2 delta;
            br_vector2 oppo_pos_rel;

            BrVector2Sub(&delta, &finish2d, &start2d);
            BrVector2Sub(&oppo_pos_rel, &oppo_pos2d, &start2d);
            l = BrVector2Length(&delta);
            if (l <= BR_SCALAR_EPSILON) {
                unk1 = 0.f;
            } else {
                unk1 = Vector2Cross(&oppo_pos_rel, &delta) / l;
                if (unk1 > 0.f) {
                    if (unk1 > l) {
                        unk1 -= l;
                    } else {
                        unk1 /= l;
                    }
                }
            }
            if (l > BR_SCALAR_EPSILON && speed > BR_SCALAR_EPSILON) {
                unk2 = Vector2Cross(&delta, &v2d) / (speed * l);
                unk3 = 0.f;
            } else {
                unk2 = 0.f;
                unk3 = 0.f;
            }
        }
        if (unk1 <= 1.f) {
            data->field_0x08 = 0;
            if (pOpponent_spec->follow_path_data.desired_speed > 6.f) {
                pOpponent_spec->follow_path_data.desired_speed += (6.f - pOpponent_spec->follow_path_data.desired_speed) * fabsf(unk2);
            }
            curv = unk3 + MaxCurvatureForCarSpeed(car_spec, speed) * (unk1 / 10.f - unk2);
            if (curv > car_spec->maxcurve) {
                curv = car_spec->maxcurve;
            } else if (curv < -car_spec->maxcurve) {
                curv = -car_spec->maxcurve;
            }
        } else {
            curv = 0.f;
            pOpponent_spec->follow_path_data.desired_speed = 6.f;
            if (speed < 12.0) {
                br_vector2 delta;
                br_scalar l;
                br_scalar crs;

                BrVector2Sub(&delta, &finish2d, &start2d);
                BrVector2Sub(&oppo_pos_rel, &oppo_pos2d, &start2d);
                t = BrVector2Dot(&delta, &oppo_pos_rel) / BrVector2Dot(&delta, &delta);
                if (t < 0.f) {
                    BrVector2Set(&delta, 0.f, 0.f);
                } else if (t <= 1.f) {
                    BrVector2Scale(&delta, &delta, t);
                }
                BrVector2Sub(&delta, &oppo_pos_rel, &delta);
                l = BrVector2Length(&delta);
                crs = Vector2Cross(&v2d, &delta);
                if (speed != 0.f && l != 0.f) {
                    crs /= speed * l;
                }
                curv = -crs * MaxCurvatureForCarSpeed(car_spec, speed);
                if (BrVector2Dot(&delta, &v2d) > 0.f) {
                    if (data->field_0x08 == 0) {
                        data->field_0x38 = 0;
                        data->field_0x08 = gTime_stamp_for_this_munging;
                    } else if (gTime_stamp_for_this_munging - data->field_0x08 > 5000) {
                        data->field_0x08 = gTime_stamp_for_this_munging;
                        data->field_0x38 ^= 0x1;
                    }
                    if (data->field_0x38) {
                        curv = -1.f * curv;
                    }
                }
            }
        }
        effective_speed_factor = GET_CAR_SPEED_FACTOR(car_spec);
        acc_factor = MAX(effective_speed_factor, 1.f);
        if (engine_damage > 50 && engine_damage < 98) {
            acc_factor -= (engine_damage - 50) / 80.f;
        } else if (engine_damage >= 98) {
            acc_factor -= .6f;
        }
        if (trans_damage > 50 && trans_damage < 98) {
            acc_factor -= (trans_damage - 50) / 160.f;
        } else if (trans_damage >= 98) {
            acc_factor -= .3f;
        }
        if (engine_damage >= 99 || trans_damage >= 99) {
            acc_factor = 0.f;
        }
        max_acc = acc_factor * 12.0f;
        error = data->desired_speed * effective_speed_factor - speed;
        acc = (error - data->prev_acc_error) * 1000.0f / gFrame_period_for_this_munging / 10.f + error + data->prev_acc;
        if (acc > max_acc) {
            acc = max_acc;
        }
        if (acc < -max_acc) {
            acc = -max_acc;
        }
        data->prev_acc = acc;
        data->prev_acc_error = error;
        acc *= car_spec->collision_info->M;
        if (acc > 0.0f) {
            car_spec->acc_force = acc;
            car_spec->brake_force = 0.0f;
        } else {
            car_spec->acc_force = 0.0f;
            car_spec->brake_force = -acc;
        }
        SetCurvature(car_spec, curv);
        return eFPR_OK;
    default:
        BrFatal("C:\\Carma2\\Source\\Common\\Oppoproc.c", 1266, "C:\\Carma2\\Source\\Common\\Oppoproc.c line %d", 1266);
        return eFPR_OK;
    }
#else
    __asm {
            sub esp, 0x130
            push ebx
            push ebp
            push esi
            mov esi, ecx
            xor ebx, ebx
            push edi
            mov ebp, dword ptr [esi + 8]
            mov eax, dword ptr [ebp + 0x4e0]
            mov ecx, dword ptr [ebp + 0x50c]
            mov dword ptr [esp + 0x68], eax
            mov eax, edx
            sub eax, ebx
            mov dword ptr [esp + 0x48], ecx
            je L004b10c9
            dec eax
            je L004af9bd
            push 0x4f2
            push offset gOpponent_nastyness_frigger+0x1938
            push 0x4f2
            push offset gOpponent_nastyness_frigger+0x1914
            call BrFatal
            add esp, 0x10
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x130
            ret 0xc
    L004af9bd:
            mov eax, dword ptr [esi + 0xf0]
            test ah, 4
            jne L004b0815
            mov edx, dword ptr [esi + 0xb0]
            mov ecx, eax
            shr ecx, 5
            and edx, 0x20
            and ecx, 0x20
            xor edx, ecx
            jne L004b0815
            cmp dword ptr [esp + 0x148], ebx
            jne L004afb9f
            test ah, 2
            jne L004afb9f
            mov eax, dword ptr [esi + 0xc8]
            mov ecx, dword ptr [gTime_stamp_for_this_munging]
            lea edx, [eax + 0x3e8]
            cmp ecx, edx
            jbe L004afb9f
            add eax, 0x2710
            cmp ecx, eax
            jae L004afb9f
            mov dx, word ptr [esi + 0xe8]
            mov ecx, esi
            call GetOpponentsSectionFinishNodePoint
            mov dx, word ptr [esi + 0xe8]
            mov ecx, esi
            mov edi, eax
            call GetOpponentsSectionStartNodePoint
            _emit 0xd9
        _emit 0x07
            _emit 0xd8
        _emit 0x20
            mov dx, word ptr [esi + 0xe8]
            mov ecx, esi
            fstp dword ptr [esp + 0x14]
            call GetOpponentsSectionFinishNodePoint
            mov dx, word ptr [esi + 0xe8]
            mov ecx, esi
            mov edi, eax
            call GetOpponentsSectionStartNodePoint
            fld dword ptr [edi + 4]
            fsub dword ptr [eax + 4]
            mov dx, word ptr [esi + 0xe8]
            mov ecx, esi
            fstp dword ptr [esp + 0x18]
            call GetOpponentsSectionFinishNodePoint
            mov dx, word ptr [esi + 0xe8]
            mov ecx, esi
            mov edi, eax
            call GetOpponentsSectionStartNodePoint
            fld dword ptr [edi + 8]
            fsub dword ptr [eax + 8]
            mov eax, dword ptr [esi + 8]
            mov dx, word ptr [esi + 0xe8]
            mov ecx, esi
            mov edi, dword ptr [eax + 0x10]
            fstp dword ptr [esp + 0x1c]
            call GetOpponentsSectionStartNodePoint
            fld dword ptr [edi + 0x50]
            _emit 0xd8
        _emit 0x20
            mov ecx, dword ptr [esi + 8]
            mov dx, word ptr [esi + 0xe8]
            mov edi, dword ptr [ecx + 0x10]
            mov ecx, esi
            fstp dword ptr [esp + 0x28]
            call GetOpponentsSectionStartNodePoint
            fld dword ptr [edi + 0x54]
            fsub dword ptr [eax + 4]
            mov edx, dword ptr [esi + 8]
            mov ecx, esi
            mov edi, dword ptr [edx + 0x10]
            mov dx, word ptr [esi + 0xe8]
            fstp dword ptr [esp + 0x2c]
            call GetOpponentsSectionStartNodePoint
            fld dword ptr [edi + 0x58]
            fsub dword ptr [eax + 8]
            fld dword ptr [esp + 0x28]
            fmul dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x2c]
            fmul dword ptr [esp + 0x18]
            faddp st(1), st(0)
            fld st(1)
            fmul dword ptr [esp + 0x1c]
            faddp st(1), st(0)
            fld dword ptr [esp + 0x14]
            fmul dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x18]
            fmul dword ptr [esp + 0x18]
            faddp st(1), st(0)
            fld dword ptr [esp + 0x1c]
            fmul dword ptr [esp + 0x1c]
            faddp st(1), st(0)
            fdivp st(1), st(0)
            fld st(0)
            fld st(1)
            fxch st(1)
            fmul dword ptr [esp + 0x14]
            fxch st(1)
            fmul dword ptr [esp + 0x18]
            fxch st(2)
            fmul dword ptr [esp + 0x1c]
            fxch st(1)
            fsubr dword ptr [esp + 0x28]
            fxch st(2)
            fsubr dword ptr [esp + 0x2c]
            fxch st(1)
            fsubp st(3), st(0)
            fxch st(1)
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
            mov dx, word ptr [esi + 0xe8]
            mov ecx, esi
            faddp st(1), st(0)
            fsqrt
            fstp st(1)
            fstp dword ptr [esp + 0x10]
            call GetOpponentsSectionWidth
            fld dword ptr [esp + 0x10]
            fcompp
            fnstsw ax
            test ah, 0x41
            je L004afb9f
            mov eax, dword ptr [esi + 0xf0]
            or ah, 2
            mov dword ptr [esi + 0xf0], eax
    L004afb9f:
            mov eax, dword ptr [esi + 0xc8]
            mov ecx, dword ptr [gTime_stamp_for_this_munging]
            add eax, 0x2710
            cmp ecx, eax
            jbe L004afbe8
            mov eax, dword ptr [esi + 0xf0]
            test ah, 2
            jne L004afbe8
            mov ecx, dword ptr [esi + 8]
            add ecx, 0xec
            push ecx
            push offset gOpponent_nastyness_frigger+0x18d4
            call DoNotDprintf_oppoproc
            add esp, 8
            mov eax, 2
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x130
            ret 0xc
    L004afbe8:
            mov ecx, dword ptr [ebp + 0x12d0]
            mov eax, dword ptr [ebp + 8]
            or ecx, 0x40000
            mov dword ptr [ebp + 0x12d0], ecx
            mov ecx, 0x100
            fld dword ptr [eax + 0x68]
            fld dword ptr [eax + 0x6c]
            fld dword ptr [eax + 0x70]
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
            fmul qword ptr [gCop_nastiness_bias+0x20]
            fst dword ptr [esp + 0x10]
            fcomp dword ptr [gCop_nastiness_bias+0x28]
            fnstsw ax
            test ah, 0x41
            jne L004afc68
            mov edx, dword ptr [esi + 0xf0]
            or edx, ecx
            mov dword ptr [esi + 0xf0], edx
            mov eax, dword ptr [esi + 0xb0]
            or eax, ecx
            mov dword ptr [esi + 0xb0], eax
    L004afc68:
            cmp dword ptr [esi + 0xbc], ebx
            je L004afecc
            mov eax, dword ptr [esi + 0xf0]
            mov edx, dword ptr [gTime_stamp_for_this_munging]
            and eax, 0xff
            mov dword ptr [esp + 0x14], edx
            mov dword ptr [esp + 0x18], ebx
            mov edi, dword ptr [esi + 0xbc]
            lea eax, [eax + eax*2]
            fild qword ptr [esp + 0x14]
            lea eax, [eax + eax*4]
            mov dword ptr [esp + 0x18], ebx
            lea eax, [eax + eax*4]
            lea eax, [eax + eax*4]
            lea ecx, [eax + eax - 0x2ee]
            lea eax, [edi + 0xabe]
            mov dword ptr [esp + 0x14], ecx
            mov ecx, dword ptr [esi + 8]
            fild qword ptr [esp + 0x14]
            mov edx, dword ptr [ecx + 8]
            mov dword ptr [esp + 0x14], eax
            mov dword ptr [esp + 0x18], ebx
            fld dword ptr [edx + 4]
            fmul dword ptr [gCop_nastiness_bias+0x1c]
            fsubr st(0), st(1)
            fiadd dword ptr [esp + 0x14]
            fld st(2)
            fcompp
            fnstsw ax
            test ah, 0x41
            jne L004afd44
            fstp st(0)
            fstp st(0)
            fld dword ptr [esp + 0x10]
            sub esp, 8
            add ecx, 0xec
            _emit 0xdd
        _emit 0x1c
        _emit 0x24
            push ecx
            push offset gOpponent_nastyness_frigger+0x18ac
            call DoNotDprintf_oppoproc
            mov eax, dword ptr [esi + 0xf0]
            add esp, 0x10
            and ah, 0xfd
            mov dword ptr [esi + 0xf0], eax
            mov ecx, dword ptr [gTime_stamp_for_this_munging]
            mov dword ptr [esi + 0xc8], ecx
            mov dword ptr [esi + 0xbc], ebx
            mov edx, dword ptr [gTime_stamp_for_this_munging]
            mov dword ptr [esi + 0xc0], edx
            mov dword ptr [ebp + 0x12c0], ebx
            mov dword ptr [ebp + 0x12b8], ebx
            jmp L004afff9
    L004afd44:
            fld dword ptr [edx + 4]
            fmul dword ptr [gCop_nastiness_bias+0x18]
            add edi, 0x7d0
            mov dword ptr [esp + 0x18], ebx
            mov dword ptr [esp + 0x14], edi
            fild qword ptr [esp + 0x14]
            fxch st(1)
            fsubp st(1), st(0)
            fadd st(0), st(1)
            fxch st(1)
            fxch st(2)
            fcompp
            fnstsw ax
            test ah, 0x41
            fstp st(0)
            jne L004afe1a
            mov eax, dword ptr [ebp + 8]
            mov dword ptr [ebp + 0x12b8], ebx
            mov dword ptr [esp + 0x18], ebx
            fld dword ptr [eax + 4]
            fmul dword ptr [gCop_nastiness_bias+0x2c]
            fld dword ptr [ebp + 0x1254]
            fchs
            fxch st(1)
            fstp dword ptr [ebp + 0x12c0]
            mov ecx, dword ptr [gFrame_period_for_this_munging]
            mov dword ptr [esp + 0x14], ecx
            fild qword ptr [esp + 0x14]
            fld st(1)
            fxch st(1)
            fmul qword ptr [gCop_nastiness_bias+0x30]
            fstp dword ptr [esp + 0x10]
            fcomp dword ptr [esp + 0x10]
            fnstsw ax
            test ah, 0x41
            jne L004afde6
            fstp st(0)
            fld dword ptr [esp + 0x10]
            fadd dword ptr [ebp + 0x1254]
            xor eax, eax
            fstp dword ptr [ebp + 0x1254]
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x130
            ret 0xc
    L004afde6:
            fld dword ptr [esp + 0x10]
            fchs
            fxch st(1)
            fcompp
            fnstsw ax
            test ah, 1
            je L004afeb7
            fld dword ptr [ebp + 0x1254]
            fsub dword ptr [esp + 0x10]
            xor eax, eax
            fstp dword ptr [ebp + 0x1254]
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x130
            ret 0xc
    L004afe1a:
            mov edx, dword ptr [ebp + 8]
            mov dword ptr [ebp + 0x12c0], ebx
            mov dword ptr [esp + 0x18], ebx
            fld dword ptr [edx + 4]
            fmul dword ptr [gCop_nastiness_bias+0x38]
            fld dword ptr [ebp + 0x1254]
            fchs
            fxch st(1)
            fstp dword ptr [ebp + 0x12b8]
            mov eax, dword ptr [gFrame_period_for_this_munging]
            mov dword ptr [esp + 0x14], eax
            fild qword ptr [esp + 0x14]
            fld st(1)
            fxch st(1)
            fmul qword ptr [gCop_nastiness_bias+0x30]
            fstp dword ptr [esp + 0x10]
            fcomp dword ptr [esp + 0x10]
            fnstsw ax
            test ah, 0x41
            jne L004afe87
            fstp st(0)
            fld dword ptr [esp + 0x10]
            fadd dword ptr [ebp + 0x1254]
            xor eax, eax
            fstp dword ptr [ebp + 0x1254]
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x130
            ret 0xc
    L004afe87:
            fld dword ptr [esp + 0x10]
            fchs
            fxch st(1)
            fcompp
            fnstsw ax
            test ah, 1
            je L004afeb7
            fld dword ptr [ebp + 0x1254]
            fsub dword ptr [esp + 0x10]
            xor eax, eax
            fstp dword ptr [ebp + 0x1254]
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x130
            ret 0xc
    L004afeb7:
            mov dword ptr [ebp + 0x1254], ebx
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x130
            ret 0xc
    L004afecc:
            mov edi, dword ptr [esp + 0x148]
            cmp edi, ebx
            je L004afedf
            fld dword ptr [gCop_nastiness_bias+0x3c]
            jmp L004afee5
    L004afedf:
            fld dword ptr [gCop_nastiness_bias+0x28]
    L004afee5:
            cmp dword ptr [esp + 0x14c], ebx
            jne L004afff7
            fld dword ptr [esp + 0x10]
            fcomp st(1)
            fnstsw ax
            test ah, 0x41
            fstp st(0)
            je L004afff9
            mov edx, dword ptr [esi + 0xc0]
            mov eax, dword ptr [gTime_stamp_for_this_munging]
            add edx, 0x7d0
            cmp eax, edx
            jbe L004afff9
            cmp dword ptr [esp + 0x144], ebx
            jne L004aff33
            test dword ptr [esi + 0xf0], ecx
            je L004afff9
    L004aff33:
            fld dword ptr [esp + 0x10]
            movsx eax, word ptr [esi + 0xe8]
            mov ecx, dword ptr [esi + 8]
            sub esp, 8
            add ecx, 0xec
            _emit 0xdd
        _emit 0x1c
        _emit 0x24
            push eax
            push ecx
            push offset gOpponent_nastyness_frigger+0x1860
            call DoNotDprintf_oppoproc
            mov edx, dword ptr [gTime_stamp_for_this_munging]
            add esp, 0x14
            cmp edi, ebx
            mov dword ptr [esi + 0xbc], edx
            jne L004affd9
            mov ax, word ptr [esi + 0xec]
            cmp ax, word ptr [esi + 0xe8]
            jne L004affd9
            mov eax, dword ptr [esi + 0xf0]
            mov ecx, eax
            and ecx, 0xff
            cmp ecx, 3
            jb L004affc4
            mov dword ptr [ebp + 0x12b8], ebx
            mov dword ptr [ebp + 0x12c0], ebx
            mov edx, dword ptr [esi + 8]
            add edx, 0xec
            push edx
            push offset gOpponent_nastyness_frigger+0x181c
            call DoNotDprintf_oppoproc
            add esp, 8
            mov eax, 2
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x130
            ret 0xc
    L004affc4:
            lea ecx, [eax + 1]
            xor ecx, eax
            and ecx, 0xff
            xor ecx, eax
            mov dword ptr [esi + 0xf0], ecx
            jmp L004afff9
    L004affd9:
            mov eax, dword ptr [esi + 0xf0]
            mov dx, word ptr [esi + 0xe8]
            and al, 0
            mov word ptr [esi + 0xec], dx
            mov dword ptr [esi + 0xf0], eax
            jmp L004afff9
    L004afff7:
            fstp st(0)
    L004afff9:
            movsx eax, word ptr [esi + 0xe8]
            push esi
            lea ecx, [esp + 0x60]
            push eax
            push ecx
            lea edx, [esp + 0x6c]
            lea ecx, [esp + 0x58]
            call GetStraight
            mov edx, dword ptr [esi + 8]
            mov ebx, eax
            mov ecx, dword ptr [esi + 0xf0]
            mov dword ptr [esp + 0x40], ebx
            mov eax, dword ptr [edx + 0x10]
            test ch, 8
            mov eax, dword ptr [eax + 0x58]
            mov dword ptr [esp + 0x54], eax
            mov edx, dword ptr [edx + 0x10]
            fld dword ptr [esp + 0x54]
            mov eax, dword ptr [edx + 0x50]
            fsub dword ptr [esp + 0x60]
            mov dword ptr [esp + 0x58], eax
            mov eax, dword ptr [ebp + 8]
            fld dword ptr [eax + 0x68]
            fld dword ptr [eax + 0x70]
            fmul qword ptr [gCop_nastiness_bias+0x20]
            fld dword ptr [esp + 0x58]
            fxch st(2)
            fmul qword ptr [gCop_nastiness_bias+0x20]
            fxch st(2)
            fsub dword ptr [esp + 0x64]
            fxch st(1)
            fst dword ptr [esp + 0x28]
            fmul dword ptr [esp + 0x28]
            fxch st(2)
            fst dword ptr [esp + 0x2c]
            fmul dword ptr [esp + 0x2c]
            fxch st(3)
            fstp dword ptr [esp + 0x14]
            fstp dword ptr [esp + 0x18]
            fxch st(1)
            faddp st(1), st(0)
            fsqrt
            fstp dword ptr [esp + 0x24]
            je L004b11d3
            fld dword ptr [esi + 0x108]
            fld dword ptr [esp + 0x18]
            fld dword ptr [esp + 0x14]
            fmul dword ptr [esp + 0x14]
            fxch st(1)
            fmul dword ptr [esp + 0x18]
            fxch st(1)
            fld st(2)
            fmul st(0), st(3)
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            fcompp
            fnstsw ax
            test ah, 1
            fstp st(0)
            je L004b00fd
            mov ax, word ptr [esi + 0x110]
            and ch, 0xf7
            cmp ax, 0x4e20
            mov dword ptr [esi + 0xf0], ecx
            mov word ptr [esi + 0xe8], ax
            jl L004b0180
            movsx ecx, ax
            push esi
            lea edx, [esp + 0x60]
            push ecx
            push edx
            lea edx, [esp + 0x6c]
            lea ecx, [esp + 0x58]
            call GetStraight
            jmp L004b0180
    L004b00fd:
            mov ecx, dword ptr [esp + 0x5c]
            lea eax, [esp + 0x4c]
            push esi
            push eax
            push ecx
            mov edx, ebx
            lea ecx, [esp + 0x8c]
            call CalcCorners
            lea edx, [esp + 0xf0]
            lea ecx, [esp + 0x80]
            push ebp
            push edx
            push esi
            push ecx
            mov ecx, dword ptr [esp + 0xa4]
            mov edx, eax
            call CalcSOCs
            fild dword ptr [esp + 0x98]
            fld dword ptr [gCop_nastiness_bias+0x40]
            mov edx, eax
            fcomp st(1)
            fnstsw ax
            test ah, 1
            je L004b11c2
            fstp st(0)
    L004b0154:
            lea eax, [esi + 0xd4]
            mov dword ptr [eax], 0x42a00000
    L004b0160:
            lea ecx, [esp + 0x80]
            push esi
            push ecx
            lea ecx, [esp + 0x5c]
            push ecx
            mov ecx, dword ptr [esp + 0x30]
            push ecx
            push eax
            lea ecx, [esp + 0x104]
            call SetMaxSpeedFromSOCs
    L004b0180:
            mov ecx, dword ptr [esi + 0xf0]
            test ch, 8
            jne L004b02b8
            fld dword ptr [esp + 0x64]
            fld dword ptr [esp + 0x60]
            fsub dword ptr [esp + 0x4c]
            fxch st(1)
            fsub dword ptr [esp + 0x50]
            fld dword ptr [esp + 0x54]
            fld dword ptr [esp + 0x58]
            fxch st(3)
            fst dword ptr [esp + 0x34]
            fmul dword ptr [esp + 0x34]
            fxch st(2)
            fst dword ptr [esp + 0x38]
            fmul dword ptr [esp + 0x38]
            fxch st(1)
            fsub dword ptr [esp + 0x4c]
            fxch st(3)
            fsub dword ptr [esp + 0x50]
            fxch st(1)
            faddp st(2), st(0)
            fxch st(2)
            fstp dword ptr [esp + 0x40]
            fsqrt
            fxch st(1)
            fxch st(1)
            fst dword ptr [esp + 0x20]
            fcomp dword ptr [gFLOAT_0058a620+0x8]
            fstp dword ptr [esp + 0x44]
            fnstsw ax
            test ah, 0x41
            jne L004b024e
            fld dword ptr [esp + 0x38]
            fld dword ptr [esp + 0x34]
            fmul dword ptr [esp + 0x40]
            fxch st(1)
            fmul dword ptr [esp + 0x44]
            faddp st(1), st(0)
            fdiv dword ptr [esp + 0x20]
            fcom dword ptr [gFLOAT_0058a61c]
            fnstsw ax
            test ah, 0x41
            jne L004b0254
            fcom dword ptr [esp + 0x20]
            fnstsw ax
            test ah, 0x41
            jne L004b0224
            fsub dword ptr [esp + 0x20]
            jmp L004b0254
    L004b0224:
            fstp st(0)
            fld dword ptr [esp + 0x38]
            fld dword ptr [esp + 0x44]
            fchs
            fld dword ptr [esp + 0x5c]
            fxch st(1)
            fmul dword ptr [esp + 0x34]
            fxch st(2)
            fmul dword ptr [esp + 0x40]
            fxch st(1)
            fmul dword ptr [esp + 0x20]
            fxch st(1)
            faddp st(2), st(0)
            fdivp st(1), st(0)
            jmp L004b0254
    L004b024e:
            fld dword ptr [gFLOAT_0058a61c]
    L004b0254:
            fld dword ptr [esp + 0x20]
            fcomp dword ptr [gFLOAT_0058a620+0x8]
            fnstsw ax
            test ah, 0x41
            jne L004b02a7
            fld dword ptr [esp + 0x24]
            fcomp dword ptr [gFLOAT_0058a620+0x8]
            fnstsw ax
            test ah, 0x41
            jne L004b02a7
            fld dword ptr [esp + 0x2c]
            fld dword ptr [esp + 0x38]
            fchs
            fld dword ptr [esp + 0x24]
            fxch st(1)
            fmul dword ptr [esp + 0x28]
            fxch st(2)
            fmul dword ptr [esp + 0x34]
            fxch st(1)
            fmul dword ptr [esp + 0x20]
            fxch st(1)
            faddp st(2), st(0)
            fdivp st(1), st(0)
            fld dword ptr [gFLOAT_0058a61c]
            jmp L004b035e
    L004b02a7:
            fld dword ptr [gFLOAT_0058a61c]
            fld dword ptr [gFLOAT_0058a61c]
            jmp L004b035e
    L004b02b8:
            fld dword ptr [esp + 0x58]
            fld dword ptr [esp + 0x54]
            fsub dword ptr [esi + 0xfc]
            fxch st(1)
            fsub dword ptr [esi + 0x100]
            fxch st(1)
            fst dword ptr [esp + 0x40]
            fmul dword ptr [esp + 0x40]
            fxch st(1)
            fst dword ptr [esp + 0x44]
            fmul dword ptr [esp + 0x44]
            fld dword ptr [esp + 0x24]
            fxch st(1)
            faddp st(2), st(0)
            fcomp dword ptr [gFLOAT_0058a620+0x8]
            fsqrt
            fst dword ptr [esp + 0x34]
            fsub dword ptr [esi + 0x104]
            fnstsw ax
            fdiv dword ptr [esi + 0x10c]
            test ah, 0x41
            jne L004b0331
            fld dword ptr [esp + 0x2c]
            fld dword ptr [esp + 0x28]
            fmul dword ptr [esp + 0x40]
            fld dword ptr [esi + 0x104]
            fxch st(2)
            fmul dword ptr [esp + 0x44]
            fxch st(2)
            fmul dword ptr [esp + 0x24]
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            fdivp st(1), st(0)
            jmp L004b0337
    L004b0331:
            fld dword ptr [gFLOAT_0058a61c]
    L004b0337:
            fld dword ptr [esi + 0x104]
            fmul dword ptr [gFLOAT_0058a620]
            test ch, 0x10
            fdivr qword ptr [gDOUBLE_0058a630]
            jne L004b0358
            fxch st(2)
            fchs
            fxch st(2)
            fchs
            jmp L004b035e
    L004b0358:
            fxch st(1)
            fchs
            fxch st(1)
    L004b035e:
            fld st(2)
            fabs
            fcomp dword ptr [gFLOAT_0058a638]
            fnstsw ax
            test ah, 0x41
            jne L004b0569
            fstp st(0)
            fstp st(0)
            fstp st(0)
            fld dword ptr [gFLOAT_0058a61c]
            fld dword ptr [esp + 0x24]
            fcomp dword ptr [gFLOAT_0058a638+0x4]
            mov dword ptr [esi + 0xd4], 0x40c00000
            fnstsw ax
            test ah, 1
            je L004b0619
            fstp st(0)
            fld dword ptr [esp + 0x64]
            fld dword ptr [esp + 0x60]
            fsub dword ptr [esp + 0x4c]
            fxch st(1)
            fsub dword ptr [esp + 0x50]
            fld dword ptr [esp + 0x58]
            fld dword ptr [esp + 0x54]
            fsub dword ptr [esp + 0x4c]
            fxch st(1)
            fsub dword ptr [esp + 0x50]
            fld st(3)
            fld st(4)
            fxch st(4)
            fst dword ptr [esp + 0x38]
            fxch st(3)
            fstp dword ptr [esp + 0x40]
            fmul dword ptr [esp + 0x40]
            fld dword ptr [esp + 0x38]
            fxch st(2)
            fstp dword ptr [esp + 0x44]
            fxch st(2)
            fmul dword ptr [esp + 0x44]
            fxch st(3)
            fmul st(0), st(4)
            fxch st(3)
            faddp st(2), st(0)
            fmul dword ptr [esp + 0x38]
            faddp st(2), st(0)
            fxch st(1)
            fdivp st(1), st(0)
            fst dword ptr [esp + 0x20]
            fcomp dword ptr [gFLOAT_0058a61c]
            fnstsw ax
            test ah, 1
            je L004b041b
            fstp st(0)
            fld dword ptr [gFLOAT_0058a61c]
            fld dword ptr [gFLOAT_0058a61c]
            jmp L004b043c
    L004b041b:
            fld dword ptr [esp + 0x20]
            fcomp dword ptr [gFLOAT_0058a638]
            fld dword ptr [esp + 0x38]
            fnstsw ax
            test ah, 0x41
            je L004b043c
            fxch st(1)
            fmul dword ptr [esp + 0x20]
            fxch st(1)
            fmul dword ptr [esp + 0x20]
    L004b043c:
            fld dword ptr [esp + 0x44]
            fld dword ptr [esp + 0x40]
            fsubrp st(3), st(0)
            fsub st(0), st(1)
            fld dword ptr [esp + 0x2c]
            fld dword ptr [esp + 0x24]
            fxch st(2)
            fxch st(1)
            fld st(4)
            fmul st(0), st(5)
            fxch st(2)
            fst dword ptr [esp + 0x18]
            fmul dword ptr [esp + 0x18]
            fld dword ptr [esp + 0x18]
            fxch st(4)
            fcomp dword ptr [gFLOAT_0058a61c]
            faddp st(2), st(0)
            fnstsw ax
            fchs
            fxch st(1)
            fsqrt
            test ah, 0x40
            fxch st(1)
            fmul st(0), st(4)
            fxch st(1)
            fstp dword ptr [esp + 0x34]
            fxch st(1)
            fmul dword ptr [esp + 0x28]
            fld dword ptr [esp + 0x34]
            fxch st(1)
            faddp st(2), st(0)
            fstp dword ptr [esp + 0x20]
            fstp st(1)
            jne L004b04b6
            fld dword ptr [esp + 0x20]
            fcomp dword ptr [gFLOAT_0058a61c]
            fnstsw ax
            test ah, 0x40
            jne L004b04b6
            fld dword ptr [esp + 0x24]
            fmul dword ptr [esp + 0x20]
            fdivp st(1), st(0)
    L004b04b6:
            fld dword ptr [esp + 0x24]
            fcomp dword ptr [gFLOAT_0058a638+0x8]
            fld dword ptr [ebp + 0x1258]
            fnstsw ax
            test ah, 1
            jne L004b04d7
            fmul dword ptr [gFLOAT_0058a638+0x8]
            fdiv dword ptr [esp + 0x24]
    L004b04d7:
            fld dword ptr [esp + 0x18]
            fxch st(3)
            fmul dword ptr [esp + 0x28]
            fxch st(3)
            fmul dword ptr [esp + 0x2c]
            fld st(1)
            fxch st(3)
            fchs
            fxch st(1)
            faddp st(4), st(0)
            fxch st(3)
            fcomp dword ptr [gFLOAT_0058a61c]
            fxch st(2)
            fmulp st(1), st(0)
            fxch st(1)
            fstp st(0)
            fnstsw ax
            test ah, 0x41
            jne L004b0619
            mov eax, dword ptr [esi + 0xc4]
            xor edx, edx
            cmp eax, edx
            jne L004b052b
            mov eax, dword ptr [gTime_stamp_for_this_munging]
            mov dword ptr [esi + 0xf4], edx
            mov dword ptr [esi + 0xc4], eax
            jmp L004b0552
    L004b052b:
            mov ecx, dword ptr [gTime_stamp_for_this_munging]
            mov edi, ecx
            sub edi, eax
            cmp edi, 0x1388
            jbe L004b0552
            mov eax, dword ptr [esi + 0xf4]
            mov dword ptr [esi + 0xc4], ecx
            xor eax, 1
            mov dword ptr [esi + 0xf4], eax
    L004b0552:
            cmp dword ptr [esi + 0xf4], edx
            je L004b061b
            fmul dword ptr [gFLOAT_0058a644]
            jmp L004b061b
    L004b0569:
            fld dword ptr [esi + 0xd4]
            fcomp dword ptr [gFLOAT_0058a644+0x4]
            xor edx, edx
            mov dword ptr [esi + 0xc4], edx
            fnstsw ax
            test ah, 0x41
            jne L004b05a6
            fld st(1)
            fld dword ptr [gFLOAT_0058a644+0x4]
            fsub dword ptr [esi + 0xd4]
            fxch st(1)
            fabs
            fxch st(1)
            fmulp st(1), st(0)
            fadd dword ptr [esi + 0xd4]
            fstp dword ptr [esi + 0xd4]
    L004b05a6:
            fld dword ptr [esp + 0x24]
            fcomp dword ptr [gFLOAT_0058a638+0x8]
            fld dword ptr [ebp + 0x1258]
            fnstsw ax
            test ah, 1
            jne L004b05c7
            fmul dword ptr [gFLOAT_0058a638+0x8]
            fdiv dword ptr [esp + 0x24]
    L004b05c7:
            fxch st(3)
            fmul dword ptr [gFLOAT_0058a64c]
            fsub st(0), st(2)
            fxch st(3)
            fmulp st(3), st(0)
            fxch st(2)
            fadd st(0), st(2)
            fxch st(2)
            fstp st(0)
            fstp st(0)
            fcom dword ptr [ebp + 0x1258]
            fnstsw ax
            test ah, 0x41
            jne L004b05f6
            fstp st(0)
            fld dword ptr [ebp + 0x1258]
            jmp L004b061b
    L004b05f6:
            fld dword ptr [ebp + 0x1258]
            fld st(1)
            fxch st(1)
            fchs
            fstp dword ptr [esp + 0x20]
            fcomp dword ptr [esp + 0x20]
            fnstsw ax
            test ah, 1
            je L004b061b
            fstp st(0)
            fld dword ptr [esp + 0x20]
            jmp L004b061b
    L004b0619:
            xor edx, edx
    L004b061b:
            xor ecx, ecx
            mov cx, word ptr [ebp + 0x80]
            and cl, 0
            cmp ecx, 0x300
            jne L004b0637
            fld dword ptr [gCop_speed_factor]
            jmp L004b063d
    L004b0637:
            fld dword ptr [gOpponent_speed_factor]
    L004b063d:
            fcom dword ptr [gFLOAT_0058a638]
            fnstsw ax
            test ah, 1
            je L004b0652
            fld dword ptr [gFLOAT_0058a638]
            jmp L004b0654
    L004b0652:
            fld st(0)
    L004b0654:
            mov ecx, dword ptr [esp + 0x68]
            cmp ecx, 0x32
            jle L004b0677
            cmp ecx, 0x62
            jge L004b067c
            lea eax, [ecx - 0x32]
            mov dword ptr [esp + 0x68], eax
            fild dword ptr [esp + 0x68]
            fmul dword ptr [gFLOAT_0058a64c+0x4]
            fsubp st(1), st(0)
            jmp L004b0682
    L004b0677:
            cmp ecx, 0x62
            jl L004b0682
    L004b067c:
            fsub dword ptr [gFLOAT_0058a64c+0x8]
    L004b0682:
            mov eax, dword ptr [esp + 0x48]
            cmp eax, 0x32
            jle L004b06a5
            cmp eax, 0x62
            jge L004b06aa
            lea edi, [eax - 0x32]
            mov dword ptr [esp + 0x48], edi
            fild dword ptr [esp + 0x48]
            fmul dword ptr [gFLOAT_0058a64c+0xc]
            fsubp st(1), st(0)
            jmp L004b06b0
    L004b06a5:
            cmp eax, 0x62
            jl L004b06b0
    L004b06aa:
            fsub dword ptr [gFLOAT_0058a64c+0x10]
    L004b06b0:
            cmp ecx, 0x63
            jge L004b06ba
            cmp eax, 0x63
            jl L004b06c2
    L004b06ba:
            fstp st(0)
            fld dword ptr [gFLOAT_0058a61c]
    L004b06c2:
            fld dword ptr [esi + 0xd4]
            mov ecx, dword ptr [gFrame_period_for_this_munging]
            mov dword ptr [esp + 0x18], edx
            fmul st(0), st(2)
            mov dword ptr [esp + 0x14], ecx
            fsub dword ptr [esp + 0x10]
            fild qword ptr [esp + 0x14]
            fxch st(2)
            fmul dword ptr [gFLOAT_0058a638+0x4]
            fxch st(1)
            fst dword ptr [esp + 0x20]
            fsub dword ptr [esi + 0xd0]
            fmul dword ptr [gFLOAT_0058a64c+0x14]
            fxch st(1)
            fstp dword ptr [esp + 0x10]
            fxch st(1)
            fdivp st(1), st(0)
            fmul dword ptr [gFLOAT_0058a64c]
            fadd dword ptr [esp + 0x20]
            fadd dword ptr [esi + 0xcc]
            fstp st(1)
            fcom dword ptr [esp + 0x10]
            fnstsw ax
            test ah, 0x41
            jne L004b0727
            fstp st(0)
            fld dword ptr [esp + 0x10]
    L004b0727:
            fld dword ptr [esp + 0x10]
            fld st(1)
            fxch st(1)
            fchs
            fstp dword ptr [esp + 0x10]
            fcomp dword ptr [esp + 0x10]
            fnstsw ax
            test ah, 1
            je L004b0746
            fstp st(0)
            fld dword ptr [esp + 0x10]
    L004b0746:
            fld st(0)
            fld dword ptr [esp + 0x20]
            fxch st(1)
            fstp dword ptr [esi + 0xcc]
            fstp dword ptr [esi + 0xd0]
            mov eax, dword ptr [ebp + 8]
            fmul dword ptr [eax + 4]
            fcom dword ptr [gFLOAT_0058a61c]
            fnstsw ax
            test ah, 0x41
            jne L004b077b
            fstp dword ptr [ebp + 0x12b8]
            mov dword ptr [ebp + 0x12c0], edx
            jmp L004b0789
    L004b077b:
            fchs
            fstp dword ptr [ebp + 0x12c0]
            mov dword ptr [ebp + 0x12b8], edx
    L004b0789:
            mov ecx, dword ptr [gFrame_period_for_this_munging]
            mov dword ptr [esp + 0x18], edx
            fld st(0)
            mov dword ptr [esp + 0x14], ecx
            fild qword ptr [esp + 0x14]
            fxch st(1)
            fsub dword ptr [ebp + 0x1254]
            fxch st(1)
            fmul qword ptr [gCop_nastiness_bias+0x30]
            fld st(1)
            fxch st(1)
            fstp dword ptr [esp + 0x10]
            fcomp dword ptr [esp + 0x10]
            fnstsw ax
            test ah, 0x41
            jne L004b07e3
            fstp st(0)
            fstp st(0)
            fld dword ptr [esp + 0x10]
            fadd dword ptr [ebp + 0x1254]
            xor eax, eax
            fstp dword ptr [ebp + 0x1254]
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x130
            ret 0xc
    L004b07e3:
            fld dword ptr [esp + 0x10]
            fchs
            fxch st(1)
            fcompp
            fnstsw ax
            test ah, 1
            je L004b0800
            fstp st(0)
            fld dword ptr [ebp + 0x1254]
            fsub dword ptr [esp + 0x10]
    L004b0800:
            fstp dword ptr [ebp + 0x1254]
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x130
            ret 0xc
    L004b0815:
            mov dx, word ptr [esi + 0xe8]
            mov ecx, esi
            call GetOpponentsSectionStartNodePoint
            mov dx, word ptr [esi + 0xe8]
            mov ecx, esi
            mov ebx, eax
            call GetOpponentsSectionFinishNodePoint
            mov ecx, dword ptr [esi + 0xf0]
            mov edi, eax
            mov eax, dword ptr [esi + 0xb0]
            mov edx, ecx
            and eax, 0x20
            shr edx, 5
            mov ebp, eax
            and edx, 0x20
            and ebp, 0xffffffe0
            xor edx, ebp
            je L004b0cc2
            and ch, 0xfb
            shl eax, 5
            or ecx, eax
            mov dword ptr [esi + 0xf0], ecx
            mov eax, dword ptr [esi + 8]
            test ch, 4
            je L004b0b39
            add eax, 0xec
            push eax
            push offset gOpponent_nastyness_frigger+0x1808
            call DoNotDprintf_oppoproc
            fld dword ptr [edi + 4]
            _emit 0xd9
        _emit 0x07
            _emit 0xd8
        _emit 0x23
            fxch st(1)
            fsub dword ptr [ebx + 4]
            mov ecx, dword ptr [esi + 8]
            add esp, 8
            fld dword ptr [edi + 8]
            mov eax, dword ptr [ecx + 0x10]
            fsub dword ptr [ebx + 8]
            fld dword ptr [eax + 0x58]
            fld dword ptr [eax + 0x54]
            fsub dword ptr [ebx + 4]
            fxch st(4)
            fst dword ptr [esp + 0x14]
            fxch st(3)
            fst dword ptr [esp + 0x18]
            fxch st(2)
            fst dword ptr [esp + 0x1c]
            fxch st(4)
            fmul dword ptr [esp + 0x18]
            fxch st(1)
            fsub dword ptr [ebx + 8]
            fmul dword ptr [esp + 0x1c]
            faddp st(1), st(0)
            fld dword ptr [eax + 0x50]
            _emit 0xd8
        _emit 0x23
            fmul dword ptr [esp + 0x14]
            faddp st(1), st(0)
            fxch st(2)
            fmul dword ptr [esp + 0x14]
            fxch st(1)
            fmul dword ptr [esp + 0x18]
            fxch st(3)
            fmul dword ptr [esp + 0x1c]
            fxch st(3)
            faddp st(1), st(0)
            fxch st(2)
            faddp st(2), st(0)
            fxch st(1)
            fdivp st(1), st(0)
            fst dword ptr [esp + 0x10]
            fcomp dword ptr [gFLOAT_0058a61c]
            fnstsw ax
            test ah, 1
            je L004b0923
            mov edx, dword ptr [ebx]
            mov dword ptr [esi + 0xdc], edx
            mov eax, dword ptr [ebx + 4]
            mov dword ptr [esi + 0xe0], eax
            mov ecx, dword ptr [ebx + 8]
            mov dword ptr [esi + 0xe4], ecx
            jmp L004b09b8
    L004b0923:
            fld dword ptr [esp + 0x10]
            fcomp dword ptr [gFLOAT_0058a638]
            fnstsw ax
            test ah, 0x41
            jne L004b0950
            mov edx, dword ptr [edi]
            mov dword ptr [esi + 0xdc], edx
            mov eax, dword ptr [edi + 4]
            mov dword ptr [esi + 0xe0], eax
            mov ecx, dword ptr [edi + 8]
            mov dword ptr [esi + 0xe4], ecx
            jmp L004b09b8
    L004b0950:
            fld dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x10]
            fxch st(2)
            fmul dword ptr [esp + 0x14]
            fxch st(1)
            fmul dword ptr [esp + 0x18]
            fxch st(2)
            fmul dword ptr [esp + 0x1c]
            fld st(1)
            fxch st(3)
            fst dword ptr [esp + 0x10]
            fxch st(1)
            fst dword ptr [esp + 0x20]
            fxch st(3)
            fstp dword ptr [esi + 0xdc]
            fstp dword ptr [esi + 0xe0]
            fxch st(1)
            fstp dword ptr [esi + 0xe4]
            _emit 0xd9
        _emit 0x03
            fadd st(0), st(1)
            fstp dword ptr [esi + 0xdc]
            fstp st(0)
            fld dword ptr [ebx + 4]
            fadd dword ptr [esp + 0x10]
            fstp dword ptr [esi + 0xe0]
            fld dword ptr [ebx + 8]
            fadd dword ptr [esp + 0x20]
            fstp dword ptr [esi + 0xe4]
    L004b09b8:
            mov edx, dword ptr [gFrame_period_for_this_munging]
            xor edi, edi
            mov dword ptr [esp + 0x14], edx
            mov dword ptr [esp + 0x18], edi
            fild qword ptr [esp + 0x14]
            fld dword ptr [esi + 0xdc]
            fxch st(1)
            fmul qword ptr [gFLOAT_0058a64c+0x1c]
            fld dword ptr [esi + 0xe0]
            mov eax, dword ptr [esi + 8]
            mov ecx, esi
            fld dword ptr [esi + 0xe4]
            mov eax, dword ptr [eax + 0x10]
            fld st(2)
            fxch st(4)
            fsub dword ptr [eax + 0x50]
            fxch st(2)
            fsub dword ptr [eax + 0x54]
            fxch st(1)
            fsub dword ptr [eax + 0x58]
            fxch st(4)
            fmul dword ptr [gFLOAT_0058a644+0x4]
            fxch st(2)
            fstp dword ptr [esp + 0x14]
            fstp dword ptr [esp + 0x18]
            fxch st(2)
            fstp dword ptr [esp + 0x1c]
            fxch st(1)
            fstp dword ptr [esp + 0x24]
            fstp st(0)
            call TurnOpponentPhysicsOff
            fld dword ptr [esp + 0x18]
            fld dword ptr [esp + 0x14]
            fmul dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x1c]
            fxch st(2)
            fmul dword ptr [esp + 0x18]
            fxch st(2)
            fmul dword ptr [esp + 0x1c]
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fsqrt
            fcom dword ptr [esp + 0x24]
            fnstsw ax
            test ah, 1
            je L004b0a99
            mov eax, dword ptr [esi + 0xdc]
            mov dword ptr [esi + 0xf8], edi
            mov ecx, dword ptr [esi + 8]
            fstp st(0)
            mov edx, dword ptr [ecx + 0x10]
            mov dword ptr [edx + 0x50], eax
            mov ecx, dword ptr [esi + 8]
            mov eax, dword ptr [esi + 0xe0]
            mov edx, dword ptr [ecx + 0x10]
            mov dword ptr [edx + 0x54], eax
            mov ecx, dword ptr [esi + 8]
            mov eax, dword ptr [esi + 0xe4]
            mov edx, dword ptr [ecx + 0x10]
            mov dword ptr [edx + 0x58], eax
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x130
            ret 0xc
    L004b0a99:
            fcom dword ptr [gFLOAT_0058a670]
            mov dword ptr [esi + 0xf8], 1
            fnstsw ax
            test ah, 0x41
            jne L004b0ad4
            fdivr qword ptr [gDOUBLE_0058a630]
            fld st(0)
            fld st(1)
            fxch st(1)
            fmul dword ptr [esp + 0x14]
            fxch st(1)
            fmul dword ptr [esp + 0x18]
            fxch st(2)
            fmul dword ptr [esp + 0x1c]
            fxch st(1)
            fxch st(2)
            fxch st(1)
            jmp L004b0ae8
    L004b0ad4:
            fstp st(0)
            fld dword ptr [gFLOAT_0058a638]
            fld dword ptr [gFLOAT_0058a61c]
            fld dword ptr [gFLOAT_0058a61c]
    L004b0ae8:
            fld dword ptr [esp + 0x24]
            mov ecx, dword ptr [esi + 8]
            xor eax, eax
            fmul st(0), st(3)
            mov ecx, dword ptr [ecx + 0x10]
            fld dword ptr [esp + 0x24]
            fxch st(1)
            fadd dword ptr [ecx + 0x50]
            fxch st(1)
            fmul st(0), st(3)
            fld dword ptr [esp + 0x24]
            fmul st(0), st(3)
            fxch st(2)
            fstp dword ptr [ecx + 0x50]
            mov edx, dword ptr [esi + 8]
            mov ecx, dword ptr [edx + 0x10]
            fadd dword ptr [ecx + 0x54]
            fstp dword ptr [ecx + 0x54]
            mov ecx, dword ptr [esi + 8]
            mov ecx, dword ptr [ecx + 0x10]
            fadd dword ptr [ecx + 0x58]
            fstp dword ptr [ecx + 0x58]
            fstp st(0)
            fstp st(0)
            fstp st(0)
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x130
            ret 0xc
    L004b0b39:
            mov ecx, dword ptr [eax + 0x10]
            lea edx, [eax + 0xec]
            _emit 0xd9
        _emit 0x07
            fsub dword ptr [ecx + 0x50]
            push edx
            push offset gOpponent_nastyness_frigger+0x17d8
            fstp dword ptr [esp + 0x30]
            mov edx, dword ptr [eax + 0x10]
            fld dword ptr [edi + 4]
            fsub dword ptr [edx + 0x54]
            fld dword ptr [esp + 0x30]
            fmul dword ptr [esp + 0x30]
            fxch st(1)
            fstp dword ptr [esp + 0x34]
            mov eax, dword ptr [eax + 0x10]
            fld dword ptr [edi + 8]
            fsub dword ptr [eax + 0x58]
            fld dword ptr [esp + 0x34]
            fmul dword ptr [esp + 0x34]
            fxch st(1)
            fst dword ptr [esp + 0x38]
            fmul dword ptr [esp + 0x38]
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fsqrt
            fstp dword ptr [esp + 0x50]
            call DoNotDprintf_oppoproc
            xor ebp, ebp
            add esp, 8
            cmp word ptr [esi + 0xe8], bp
            jge L004b0bac
            mov ecx, offset gOpponent_nastyness_frigger+0x17a8
            call PDFatalError
    L004b0bac:
            mov dx, word ptr [esi + 0xe8]
            push 1
            mov ecx, esi
            call GetOpponentsSectionMaxSpeed
            cmp eax, 0xff
            jge L004b0bea
            mov dx, word ptr [esi + 0xe8]
            push 1
            mov ecx, esi
            call GetOpponentsSectionMaxSpeed
            mov dword ptr [esp + 0x48], eax
            fild dword ptr [esp + 0x48]
            fmul qword ptr [gFLOAT_0058a670+0x8]
            fstp dword ptr [esp + 0x10]
            jmp L004b0c79
    L004b0bea:
            mov dx, word ptr [esi + 0xe8]
            push 1
            mov ecx, esi
            call GetOpponentsSectionMinSpeed
            test eax, eax
            jle L004b0c22
            mov dx, word ptr [esi + 0xe8]
            push 1
            mov ecx, esi
            call GetOpponentsSectionMinSpeed
            mov dword ptr [esp + 0x48], eax
            fild dword ptr [esp + 0x48]
            fmul qword ptr [gFLOAT_0058a670+0x8]
            fstp dword ptr [esp + 0x10]
            jmp L004b0c79
    L004b0c22:
            fld dword ptr [esp + 0x48]
            fadd st(0), st(0)
            fld dword ptr [gFLOAT_0058a670+0x14]
            fcomp st(1)
            fnstsw ax
            test ah, 1
            je L004b0c3f
            fld dword ptr [gFLOAT_0058a670+0x14]
            jmp L004b0c41
    L004b0c3f:
            fld st(0)
    L004b0c41:
            fcomp dword ptr [gFLOAT_0058a638]
            fnstsw ax
            test ah, 0x41
            jne L004b0c6f
            fld dword ptr [gFLOAT_0058a670+0x14]
            fcomp st(1)
            fnstsw ax
            test ah, 1
            je L004b0c69
            fstp st(0)
            mov dword ptr [esp + 0x10], 0x40e00000
            jmp L004b0c79
    L004b0c69:
            fstp dword ptr [esp + 0x10]
            jmp L004b0c79
    L004b0c6f:
            fstp st(0)
            mov dword ptr [esp + 0x10], 0x3f800000
    L004b0c79:
            mov ecx, dword ptr [esp + 0x10]
            mov dx, word ptr [esi + 0xe8]
            push ecx
            mov ecx, esi
            call RematerialiseOpponentOnThisSection
            test eax, eax
            je L004b0cb1
            mov edx, dword ptr [esi + 8]
            mov dword ptr [edx + 0x12c0], ebp
            mov eax, dword ptr [esi + 8]
            mov dword ptr [eax + 0x12b8], ebp
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x130
            ret 0xc
    L004b0cb1:
            mov eax, dword ptr [esi + 0xf0]
            or ah, 4
            mov dword ptr [esi + 0xf0], eax
            jmp L004b0cc4
    L004b0cc2:
            xor ebp, ebp
    L004b0cc4:
            mov edx, dword ptr [esi + 8]
            mov ecx, dword ptr [gFrame_period_for_this_munging]
            mov dword ptr [esp + 0x14], ecx
            mov dword ptr [esp + 0x18], ebp
            mov eax, dword ptr [edx + 8]
            fild qword ptr [esp + 0x14]
            mov dword ptr [eax + 0x68], ebp
            mov ecx, dword ptr [esi + 8]
            mov edx, dword ptr [ecx + 8]
            fmul qword ptr [gFLOAT_0058a64c+0x1c]
            mov dword ptr [edx + 0x6c], 0x4225999a
            mov eax, dword ptr [esi + 8]
            mov ecx, dword ptr [eax + 8]
            fmul dword ptr [gFLOAT_0058a644+0x4]
            mov dword ptr [ecx + 0x70], ebp
            mov eax, dword ptr [esi + 0xf8]
            cmp eax, ebp
            fst dword ptr [esp + 0x24]
            je L004b0ea7
            mov edx, dword ptr [esi + 8]
            fld dword ptr [esi + 0xdc]
            mov ecx, dword ptr [edx + 0x10]
            fld dword ptr [esi + 0xe0]
            fsub dword ptr [ecx + 0x54]
            fxch st(1)
            fsub dword ptr [ecx + 0x50]
            fld dword ptr [esi + 0xe4]
            fsub dword ptr [ecx + 0x58]
            fxch st(2)
            fst dword ptr [esp + 0x18]
            fxch st(1)
            fst dword ptr [esp + 0x14]
            fmul dword ptr [esp + 0x14]
            fxch st(2)
            fstp dword ptr [esp + 0x1c]
            fmul dword ptr [esp + 0x18]
            fld dword ptr [esp + 0x1c]
            fmul dword ptr [esp + 0x1c]
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fsqrt
            fcom st(1)
            fnstsw ax
            test ah, 1
            je L004b0dda
            mov edx, dword ptr [esi + 0xdc]
            mov dword ptr [esi + 0xf8], ebp
            mov eax, dword ptr [esi + 8]
            fstp st(0)
            mov ecx, dword ptr [eax + 0x10]
            fstp st(0)
            mov dword ptr [ecx + 0x50], edx
            mov eax, dword ptr [esi + 8]
            mov edx, dword ptr [esi + 0xe0]
            mov ecx, dword ptr [eax + 0x10]
            mov dword ptr [ecx + 0x54], edx
            mov eax, dword ptr [esi + 8]
            mov edx, dword ptr [esi + 0xe4]
            mov ecx, dword ptr [eax + 0x10]
            mov dword ptr [ecx + 0x58], edx
            mov eax, dword ptr [esi + 8]
            _emit 0xd9
        _emit 0x07
            _emit 0xd8
        _emit 0x23
            lea edx, [esp + 0x34]
            fstp dword ptr [esp + 0x34]
            fld dword ptr [edi + 4]
            fsub dword ptr [ebx + 4]
            fstp dword ptr [esp + 0x38]
            fld dword ptr [edi + 8]
            fsub dword ptr [ebx + 8]
            fstp dword ptr [esp + 0x3c]
            mov ecx, dword ptr [eax + 0x10]
            call PointActorAlongThisBloodyVector
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x130
            ret 0xc
    L004b0dda:
            fcom dword ptr [gFLOAT_0058a670]
            fnstsw ax
            test ah, 0x41
            jne L004b0e13
            fdivr qword ptr [gDOUBLE_0058a630]
            fld dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x18]
            fld dword ptr [esp + 0x1c]
            fxch st(2)
            fxch st(1)
            fxch st(1)
            fmul st(0), st(3)
            fxch st(1)
            fmul st(0), st(3)
            fxch st(2)
            fmul st(0), st(3)
            fxch st(1)
            fstp st(3)
            fstp dword ptr [esp + 0x1c]
            jmp L004b0e29
    L004b0e13:
            fstp st(0)
            fld dword ptr [gFLOAT_0058a638]
            fld dword ptr [gFLOAT_0058a61c]
            mov dword ptr [esp + 0x1c], 0
    L004b0e29:
            fld st(2)
            fmul st(0), st(2)
            fxch st(3)
            fmul st(0), st(1)
            fxch st(3)
            fst dword ptr [esp + 0x14]
            fadd dword ptr [ecx + 0x50]
            fxch st(3)
            fst dword ptr [esp + 0x18]
            fxch st(3)
            fstp dword ptr [ecx + 0x50]
            mov eax, dword ptr [esi + 8]
            lea edx, [esp + 0x34]
            fld dword ptr [esp + 0x24]
            mov eax, dword ptr [eax + 0x10]
            fmul dword ptr [esp + 0x1c]
            fxch st(3)
            fadd dword ptr [eax + 0x54]
            fld st(3)
            fxch st(1)
            fstp dword ptr [eax + 0x54]
            mov ecx, dword ptr [esi + 8]
            fld dword ptr [esp + 0x14]
            mov eax, dword ptr [ecx + 0x10]
            fld dword ptr [esp + 0x18]
            fxch st(2)
            fadd dword ptr [eax + 0x58]
            fstp dword ptr [eax + 0x58]
            mov eax, dword ptr [esi + 8]
            fstp dword ptr [esp + 0x34]
            fstp dword ptr [esp + 0x38]
            fxch st(2)
            fstp dword ptr [esp + 0x3c]
            mov ecx, dword ptr [eax + 0x10]
            fxch st(1)
            fstp st(0)
            fstp st(0)
            call PointActorAlongThisBloodyVector
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x130
            ret 0xc
    L004b0ea7:
            fcom dword ptr [gFLOAT_0058a61c]
            fnstsw ax
            test ah, 0x41
            jne L004b1083
    L004b0eb8:
            mov ecx, dword ptr [esi + 8]
            _emit 0xd9
        _emit 0x07
            mov edx, dword ptr [ecx + 0x10]
            fsub dword ptr [edx + 0x50]
            fstp dword ptr [esp + 0x28]
            mov eax, dword ptr [ecx + 0x10]
            fld dword ptr [edi + 4]
            fsub dword ptr [eax + 0x54]
            fld dword ptr [esp + 0x28]
            fmul dword ptr [esp + 0x28]
            fxch st(1)
            fstp dword ptr [esp + 0x2c]
            mov edx, dword ptr [ecx + 0x10]
            fld dword ptr [edi + 8]
            fsub dword ptr [edx + 0x58]
            fld dword ptr [esp + 0x2c]
            fmul dword ptr [esp + 0x2c]
            fxch st(1)
            fst dword ptr [esp + 0x30]
            fmul dword ptr [esp + 0x30]
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fsqrt
            fcom st(1)
            fnstsw ax
            test ah, 1
            je L004b0fd2
            mov eax, dword ptr [ecx + 0x10]
            mov ecx, dword ptr [edi]
            fxch st(1)
            fsub st(0), st(1)
            mov dword ptr [eax + 0x50], ecx
            mov edx, dword ptr [esi + 8]
            mov ecx, dword ptr [edi + 4]
            mov eax, dword ptr [edx + 0x10]
            fstp dword ptr [esp + 0x24]
            mov dword ptr [eax + 0x54], ecx
            mov edx, dword ptr [esi + 8]
            mov ecx, dword ptr [edi + 8]
            mov eax, dword ptr [edx + 0x10]
            fstp st(0)
            mov dword ptr [eax + 0x58], ecx
            mov ecx, esi
            movsx edx, word ptr [esi + 0xe8]
            call GetOpponentsRealSection
            movsx edx, word ptr [esi + 0xe8]
            push eax
            mov eax, dword ptr [esi + 8]
            add eax, 0xec
            push edx
            push eax
            push offset gOpponent_nastyness_frigger+0x1768
            call DoNotDprintf_oppoproc
            mov dx, word ptr [esi + 0xe8]
            add esp, 0x10
            mov ecx, esi
            call GetOpponentsNextSection
            cmp ax, 0xffff
            mov word ptr [esi + 0xe8], ax
            je L004b1048
            movsx edx, ax
            mov ecx, esi
            call GetOpponentsRealSection
            movsx ecx, word ptr [esi + 0xe8]
            mov edx, dword ptr [esi + 8]
            push eax
            add edx, 0xec
            push ecx
            push edx
            push offset gOpponent_nastyness_frigger+0x172c
            call DoNotDprintf_oppoproc
            mov dx, word ptr [esi + 0xe8]
            add esp, 0x10
            mov ecx, esi
            call GetOpponentsSectionStartNodePoint
            mov dx, word ptr [esi + 0xe8]
            mov ecx, esi
            call GetOpponentsSectionFinishNodePoint
            fld dword ptr [esp + 0x24]
            mov edi, eax
            jmp L004b1035
    L004b0fd2:
            fxch st(1)
            fdiv st(0), st(1)
            fxch st(1)
            fstp st(0)
            fld st(0)
            fld st(1)
            fxch st(1)
            fmul dword ptr [esp + 0x28]
            fld dword ptr [gFLOAT_0058a61c]
            fxch st(2)
            fmul dword ptr [esp + 0x2c]
            fxch st(3)
            fmul dword ptr [esp + 0x30]
            fxch st(1)
            fstp dword ptr [esp + 0x28]
            fxch st(2)
            fstp dword ptr [esp + 0x2c]
            fxch st(1)
            fstp dword ptr [esp + 0x30]
            mov ecx, dword ptr [ecx + 0x10]
            fld dword ptr [esp + 0x28]
            fadd dword ptr [ecx + 0x50]
            fstp dword ptr [ecx + 0x50]
            mov eax, dword ptr [esi + 8]
            fld dword ptr [esp + 0x2c]
            mov eax, dword ptr [eax + 0x10]
            fadd dword ptr [eax + 0x54]
            fstp dword ptr [eax + 0x54]
            mov ecx, dword ptr [esi + 8]
            fld dword ptr [esp + 0x30]
            mov eax, dword ptr [ecx + 0x10]
            fadd dword ptr [eax + 0x58]
            fstp dword ptr [eax + 0x58]
    L004b1035:
            fcom dword ptr [gFLOAT_0058a61c]
            fnstsw ax
            test ah, 0x41
            je L004b0eb8
            jmp L004b1083
    L004b1048:
            mov edx, dword ptr [esi + 8]
            mov dword ptr [edx + 0x12b8], ebp
            mov eax, dword ptr [esi + 8]
            mov dword ptr [eax + 0x12c0], ebp
            mov ecx, dword ptr [esi + 8]
            add ecx, 0xec
            push ecx
            push offset gOpponent_nastyness_frigger+0x16ec
            call DoNotDprintf_oppoproc
            add esp, 8
            mov eax, 1
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x130
            ret 0xc
    L004b1083:
            mov esi, dword ptr [esi + 8]
            fstp st(0)
            mov edx, dword ptr [esi + 0x10]
            _emit 0xd9
        _emit 0x07
            fsub dword ptr [edx + 0x50]
            lea edx, [esp + 0x28]
            fstp dword ptr [esp + 0x28]
            mov eax, dword ptr [esi + 0x10]
            fld dword ptr [edi + 4]
            fsub dword ptr [eax + 0x54]
            fstp dword ptr [esp + 0x2c]
            mov ecx, dword ptr [esi + 0x10]
            fld dword ptr [edi + 8]
            fsub dword ptr [ecx + 0x58]
            fstp dword ptr [esp + 0x30]
            mov ecx, dword ptr [esi + 0x10]
            call PointActorAlongThisBloodyVector
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x130
            ret 0xc
    L004b10c9:
            mov ecx, esi
            call GetOpponentsFirstSection
            movsx edx, ax
            mov ecx, esi
            mov word ptr [esi + 0xea], ax
            mov word ptr [esi + 0xe8], ax
            call GetOpponentsRealSection
            mov edx, dword ptr [esi + 8]
            add edx, 0xec
            push eax
            push edx
            push offset gOpponent_nastyness_frigger+0x16a8
            call DoNotDprintf_oppoproc
            mov edi, dword ptr [esi + 0xf0]
            mov dword ptr [esi + 0xbc], ebx
            and edi, 0xfffffeff
            add esp, 0xc
            mov dword ptr [esi + 0xf0], edi
            mov ecx, dword ptr [gTime_stamp_for_this_munging]
            mov eax, edi
            mov dword ptr [esi + 0xc0], ecx
            and ah, 0xf3
            mov dword ptr [esi + 0xcc], ebx
            mov dword ptr [esi + 0xd0], ebx
            mov edx, dword ptr [gTime_stamp_for_this_munging]
            or ah, 2
            mov dword ptr [esi + 0xc8], edx
            mov word ptr [esi + 0xec], 0xffff
            mov dword ptr [esi + 0xf0], eax
            test byte ptr [esi + 0xb0], 0x28
            jne L004b11b3
            mov eax, dword ptr [esi + 8]
            add eax, 0xec
            push eax
            push offset gOpponent_nastyness_frigger+0x1668
            call DoNotDprintf_oppoproc
            mov ebp, dword ptr [ebp + 8]
            add esp, 8
            fld dword ptr [ebp + 0x68]
            fld dword ptr [ebp + 0x6c]
            fld dword ptr [ebp + 0x70]
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
            push ecx
            mov ecx, esi
            fsqrt
            fstp st(1)
            _emit 0xd9
        _emit 0x1c
        _emit 0x24
            call RematerialiseOpponentOnNearestSection
    L004b11b3:
            pop edi
            pop esi
            pop ebp
            xor eax, eax
            pop ebx
            add esp, 0x130
            ret 0xc
    L004b11c2:
            fstp dword ptr [esi + 0xd4]
            lea eax, [esi + 0xd4]
            jmp L004b0160
    L004b11d3:
            mov edx, dword ptr [esp + 0x5c]
            lea eax, [esp + 0x4c]
            push esi
            push eax
            push edx
            mov edx, ebx
            lea ecx, [esp + 0x8c]
            call CalcCorners
            fld dword ptr [esp + 0x64]
            fld dword ptr [esp + 0x60]
            fsub dword ptr [esp + 0x4c]
            fxch st(1)
            fsub dword ptr [esp + 0x50]
            fld dword ptr [esp + 0x54]
            fld dword ptr [esp + 0x58]
            fxch st(3)
            fst dword ptr [esp + 0x34]
            fld st(2)
            fxch st(1)
            fmul dword ptr [esp + 0x34]
            fxch st(2)
            fsub dword ptr [esp + 0x4c]
            fxch st(1)
            fmul st(0), st(3)
            fxch st(4)
            fsub dword ptr [esp + 0x50]
            fxch st(4)
            faddp st(2), st(0)
            movsx edi, word ptr [esi + 0xe8]
            fmul dword ptr [esp + 0x34]
            fxch st(1)
            fsqrt
            mov dword ptr [esp + 0x34], eax
            fxch st(3)
            fmul st(0), st(2)
            fxch st(3)
            fst dword ptr [esp + 0x20]
            fxch st(3)
            faddp st(1), st(0)
            fxch st(2)
            mov dword ptr [esp + 0x20], 0
            fdivp st(2), st(0)
            fxch st(1)
            fstp dword ptr [esp + 0x70]
            fstp st(0)
    L004b125d:
            mov edx, edi
            mov ecx, esi
            call GetOpponentsNextSection
            cmp eax, ebx
            mov dword ptr [esp + 0x6c], eax
            je L004b12c3
            mov edx, edi
            mov ecx, esi
            call GetOpponentsSectionFinishNodePoint
            mov edx, edi
            mov ecx, esi
            mov ebx, eax
            call GetOpponentsSectionStartNodePoint
            fld dword ptr [eax + 8]
            _emit 0xd9
        _emit 0x00
            _emit 0xd8
        _emit 0x23
            fxch st(1)
            fsub dword ptr [ebx + 8]
            fxch st(1)
            fst dword ptr [esp + 0x74]
            fld st(1)
            fxch st(1)
            fmul dword ptr [esp + 0x74]
            fxch st(1)
            fmul st(0), st(2)
            faddp st(1), st(0)
            fsqrt
            fstp st(1)
            fadd dword ptr [esp + 0x20]
            fst dword ptr [esp + 0x20]
            fcomp dword ptr [esp + 0x70]
            fnstsw ax
            test ah, 1
            je L004b12c3
            mov edi, dword ptr [esp + 0x6c]
            mov ebx, dword ptr [esp + 0x40]
            jmp L004b125d
    L004b12c3:
            mov ebx, dword ptr [esp + 0x34]
            lea ecx, [esp + 0xf0]
            push ebp
            push ecx
            lea edx, [esp + 0x88]
            push esi
            push edx
            mov edx, ebx
            mov ecx, edi
            call CalcSOCs
            test ebx, ebx
            mov edx, eax
            je L004b0154
            fld dword ptr [esp + 0x18]
            fld dword ptr [esp + 0x14]
            fmul dword ptr [esp + 0x14]
            fxch st(1)
            fmul dword ptr [esp + 0x18]
            mov eax, dword ptr [gFrame_period_for_this_munging]
            mov dword ptr [esp + 0x38], 0
            mov dword ptr [esp + 0x34], eax
            fild qword ptr [esp + 0x34]
            fxch st(1)
            faddp st(2), st(0)
            fmul dword ptr [esp + 0x24]
            fxch st(1)
            fsqrt
            fxch st(1)
            fxch st(1)
            fsub dword ptr [esp + 0x8c]
            fxch st(1)
            fmul dword ptr [gFLOAT_0058a620+0x4]
            fxch st(1)
            fmul dword ptr [gFLOAT_0058a620]
            fxch st(1)
            fcompp
            fnstsw ax
            test ah, 0x41
            jne L004b0154
            mov ecx, dword ptr [esp + 0x90]
            mov edx, dword ptr [esi + 0xf0]
            and ecx, 1
            mov eax, dword ptr [esp + 0x80]
            shl ecx, 0xc
            and dh, 0xef
            mov dword ptr [esi + 0xfc], eax
            mov eax, dword ptr [esp + 0x8c]
            or ecx, edx
            mov edx, dword ptr [esp + 0x88]
            or ch, 8
            mov dword ptr [esi + 0xf0], ecx
            mov ecx, dword ptr [esp + 0x84]
            mov dword ptr [esi + 0x104], edx
            mov edx, dword ptr [esp + 0x94]
            mov dword ptr [esi + 0x100], ecx
            mov ecx, esi
            mov dword ptr [esi + 0x108], eax
            mov dword ptr [esi + 0x110], edx
            call GetOpponentsSectionWidth
            fstp dword ptr [esi + 0x10c]
            jmp L004b0180
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004ace20
void C2_HOOK_FASTCALL ObjectiveComplete(tOpponent_spec* pOpponent_spec) {

    DoNotDprintf_opponent("%s: Objective Completed", pOpponent_spec->car_spec->driver_name);
    pOpponent_spec->new_objective_required = 1;
    switch (pOpponent_spec->current_objective) {
    case eOOT_complete_race:
        gNum_of_opponents_completing_race -= 1;
        break;
    case eOOT_pursue_and_twat:
        gNum_of_opponents_pursuing -= 1;
        break;
    case eOOT_get_near_player:
        gNum_of_opponents_getting_near -= 1;
        break;
    default:
        break;
    }
}

// FUNCTION: CARMA2_HW 0x004ab150
int C2_HOOK_FASTCALL AddToOpponentsProjectedRoute(tOpponent_spec* pOpponent_spec, tS16 pSection_no, int pDirection) {
    if (pOpponent_spec->nnext_sections < (int)CARPOCALYPSE2_ASIZE(pOpponent_spec->next_sections)) {
        pOpponent_spec->next_sections[pOpponent_spec->nnext_sections].section_no = pSection_no;
        pOpponent_spec->next_sections[pOpponent_spec->nnext_sections].direction = pDirection;
        pOpponent_spec->nnext_sections++;
        return 1;
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x004a93c0
void C2_NAKED C2_HOOK_FASTCALL TopUpRandomRoute(tOpponent_spec* pOpponent_spec, int pSections_to_add) {
#ifndef CARPOCALYPSE2_MATCHING
    tS16 section_no;
    tS16 node_no;
    tS16 temp_section_array[8];
    int i;
    int target;
    int num_of_temp_sections;
    int direction;

    if (!pSections_to_add) {
        PDEnterDebugger("TopUpRandomRoute() called with no seed (woof, bark, etc.)");
    }
    if (pSections_to_add < 0) {
        target = CARPOCALYPSE2_ASIZE(pOpponent_spec->next_sections);
    } else {
        target = MIN(pSections_to_add + pOpponent_spec->nnext_sections, (int)CARPOCALYPSE2_ASIZE(pOpponent_spec->next_sections));
    }
    while (pOpponent_spec->nnext_sections < target) {
        node_no = gProgram_state.AI_vehicles.path_sections[pOpponent_spec->next_sections[pOpponent_spec->nnext_sections - 1].section_no].node_indices[pOpponent_spec->next_sections[pOpponent_spec->nnext_sections - 1].direction];
        if (gProgram_state.AI_vehicles.path_nodes[node_no].number_of_sections > 1) {
            num_of_temp_sections = 0;
            for (i = 0; i < gProgram_state.AI_vehicles.path_nodes[node_no].number_of_sections; i++) {
                section_no = gProgram_state.AI_vehicles.path_nodes[node_no].sections[i];
                if (pOpponent_spec->next_sections[pOpponent_spec->nnext_sections - 1].section_no != section_no
                    && (!gProgram_state.AI_vehicles.path_sections[section_no].one_way || gProgram_state.AI_vehicles.path_sections[section_no].node_indices[1] != node_no)
                    && (pOpponent_spec->cheating || gProgram_state.AI_vehicles.path_sections[section_no].type != ePST_cheat_only)) {
                    temp_section_array[num_of_temp_sections] = section_no;
                    num_of_temp_sections++;
                }
            }

            if (num_of_temp_sections > 1) {
                section_no = temp_section_array[IRandomBetween(0, num_of_temp_sections - 1)];
                direction = gProgram_state.AI_vehicles.path_sections[section_no].node_indices[1] != node_no;
            } else if (num_of_temp_sections == 1) {
                section_no = temp_section_array[0];
                direction = gProgram_state.AI_vehicles.path_sections[section_no].node_indices[1] != node_no;
            } else {
                section_no = pOpponent_spec->next_sections[pOpponent_spec->nnext_sections - 1].section_no;
                direction = pOpponent_spec->next_sections[pOpponent_spec->nnext_sections - 1].direction == 0;
            }
        } else {
            section_no = pOpponent_spec->next_sections[pOpponent_spec->nnext_sections - 1].section_no;
            direction = pOpponent_spec->next_sections[pOpponent_spec->nnext_sections - 1].direction == 0;
        }
        AddToOpponentsProjectedRoute(pOpponent_spec, section_no, direction);
    }
#else
    __asm {
            sub esp, 0x24
            push ebx
            push ebp
            push esi
            push edi
            mov edi, edx
            mov esi, ecx
            test edi, edi
            jne L004a93db
            mov ecx, offset gOpponent_nastyness_frigger+0x1f8
            call PDEnterDebugger
            test edi, edi
    L004a93db:
            jl L004a93eb
            mov eax, dword ptr [esi + 0x7c]
            add eax, edi
            cmp eax, 0xc
            mov dword ptr [esp + 0x10], eax
            jl L004a93f3
    L004a93eb:
            mov dword ptr [esp + 0x10], 0xc
    L004a93f3:
            mov eax, dword ptr [esi + 0x7c]
            mov ecx, dword ptr [esp + 0x10]
            cmp eax, ecx
            jge L004a953f
            mov ebx, dword ptr [gProgram_state+0x5ec4]
    L004a9408:
            mov dx, word ptr [esi + eax*4 + 0x7c]
            mov cl, byte ptr [esi + eax*4 + 0x7e]
            movsx eax, dx
            mov byte ptr [esp + 0x20], cl
            and ecx, 0xff
            lea eax, [eax + eax*4]
            lea ecx, [ecx + eax*2]
            mov di, word ptr [ebx + ecx*2]
            mov ecx, dword ptr [gProgram_state+0x5ec0]
            movsx eax, di
            shl eax, 5
            add ecx, eax
            mov al, byte ptr [ecx + 0x1c]
            cmp al, 1
            jbe L004a94fc
            xor ebp, ebp
            and eax, 0xff
            mov dword ptr [esp + 0x18], ebp
            jle L004a94b8
            add ecx, 0xc
            lea ebp, [esp + 0x24]
            mov dword ptr [esp + 0x14], ecx
            mov dword ptr [esp + 0x1c], eax
    L004a945d:
            mov eax, dword ptr [esp + 0x14]
            mov cx, word ptr [eax]
            cmp cx, dx
            je L004a949e
            movsx eax, cx
            lea eax, [eax + eax*4]
            cmp byte ptr [ebx + eax*4 + 0x11], 0
            lea eax, [ebx + eax*4]
            je L004a947f
            cmp di, word ptr [eax + 2]
            je L004a949e
    L004a947f:
            test byte ptr [esi + 0xb0], 0x20
            jne L004a948e
            cmp byte ptr [eax + 0x10], 2
            je L004a949e
    L004a948e:
            mov word ptr [ebp], cx
            mov ecx, dword ptr [esp + 0x18]
            inc ecx
            add ebp, 2
            mov dword ptr [esp + 0x18], ecx
    L004a949e:
            mov ecx, dword ptr [esp + 0x14]
            mov eax, dword ptr [esp + 0x1c]
            add ecx, 2
            dec eax
            mov dword ptr [esp + 0x14], ecx
            mov dword ptr [esp + 0x1c], eax
            jne L004a945d
            mov ebp, dword ptr [esp + 0x18]
    L004a94b8:
            cmp ebp, 1
            jle L004a94e4
            lea edx, [ebp - 1]
            xor ecx, ecx
            call IRandomBetween
            mov ax, word ptr [esp + eax*2 + 0x24]
            mov ebx, dword ptr [gProgram_state+0x5ec4]
            movsx ecx, ax
            lea edx, [ecx + ecx*4]
            xor ecx, ecx
            cmp di, word ptr [ebx + edx*4 + 2]
            setne cl
            jmp L004a9509
    L004a94e4:
            jne L004a94fc
            mov eax, dword ptr [esp + 0x24]
            movsx ecx, ax
            lea edx, [ecx + ecx*4]
            xor ecx, ecx
            cmp di, word ptr [ebx + edx*4 + 2]
            setne cl
            jmp L004a9509
    L004a94fc:
            mov eax, edx
            mov dl, byte ptr [esp + 0x20]
            xor ecx, ecx
            test dl, dl
            sete cl
    L004a9509:
            mov edx, dword ptr [esi + 0x7c]
            cmp edx, 0xc
            jge L004a9530
            mov word ptr [esi + edx*4 + 0x80], ax
            mov eax, dword ptr [esi + 0x7c]
            mov byte ptr [esi + eax*4 + 0x82], cl
            mov eax, dword ptr [esi + 0x7c]
            inc eax
            mov dword ptr [esi + 0x7c], eax
            mov ebx, dword ptr [gProgram_state+0x5ec4]
    L004a9530:
            mov eax, dword ptr [esi + 0x7c]
            mov ecx, dword ptr [esp + 0x10]
            cmp eax, ecx
            jl L004a9408
    L004a953f:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x24
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004ac890
void C2_NAKED C2_HOOK_FASTCALL ProcessGetNearPlayer(tOpponent_spec* pOpponent_spec, tProcess_objective_command pCommand) {
#ifndef CARPOCALYPSE2_MATCHING

    NOT_IMPLEMENTED();
#else
    __asm {
            mov eax, edx
            push ebp
            push esi
            sub eax, 0
            push edi
            mov esi, ecx
            je L004ac99e
            dec eax
            jne L004ac9d2
            mov ecx, dword ptr [esi + 8]
            xor eax, eax
            mov ax, word ptr [ecx + 0x80]
            and al, 0
            cmp eax, 0x300
            jne L004ac8ef
            fld dword ptr [esi + 0x14]
            fcomp dword ptr [gOppo_pursuit_precinct_distance]
            fnstsw ax
            test ah, 0x41
            jne L004ac8ef
            add ecx, 0xec
            push ecx
            push offset gOpponent_nastyness_frigger+0xde0
            call DoNotDprintf_opponent
            add esp, 8
            push 9
            push esi
            call NewObjective
            add esp, 8
            pop edi
            pop esi
            pop ebp
            ret
    L004ac8ef:
            mov ax, word ptr [esi + 0xe8]
            cmp ax, 0x4e20
            jle L004ac945
            movsx ebp, ax
            mov eax, dword ptr [esi + 0x7c]
            sub ebp, 0x4e20
            cmp eax, ebp
            jle L004ac93c
            mov edi, 0xc
            sub edi, ebp
            test edi, edi
            jle L004ac939
            lea ecx, [esi + 0x82]
            lea eax, [esi + ebp*4 + 0x80]
    L004ac924:
            mov dx, word ptr [eax]
            add eax, 4
            mov word ptr [ecx - 2], dx
            mov dl, byte ptr [eax - 2]
            mov byte ptr [ecx], dl
            add ecx, 4
            dec edi
            jne L004ac924
    L004ac939:
            sub dword ptr [esi + 0x7c], ebp
    L004ac93c:
            mov word ptr [esi + 0xe8], 0x4e20
    L004ac945:
            cmp dword ptr [esi + 0x7c], 0xa
            jg L004ac959
            push 0
            mov edx, offset gProgram_state+0xac
            mov ecx, esi
            call NewCalcGetNearPlayerRoute
    L004ac959:
            push 0
            push 0
            push 0
            mov edx, 1
            mov ecx, esi
            call ProcessFollowPath
            cmp eax, 2
            je L004ac97a
            cmp eax, 1
            jne L004ac9d2
            cmp eax, 2
            jne L004ac98e
    L004ac97a:
            push offset gProgram_state+0xac
            push 2
            push esi
            call NewObjective
            add esp, 0xc
            pop edi
            pop esi
            pop ebp
            ret
    L004ac98e:
            mov eax, dword ptr [esi + 8]
            add eax, 0xec
            push eax
            push offset gOpponent_nastyness_frigger+0xda0
            jmp L004ac9ad
    L004ac99e:
            mov ecx, dword ptr [esi + 8]
            add ecx, 0xec
            push ecx
            push offset gOpponent_nastyness_frigger+0xd6c
    L004ac9ad:
            call DoNotDprintf_opponent
            add esp, 8
            mov edx, offset gProgram_state+0xac
            mov ecx, esi
            push 1
            call NewCalcGetNearPlayerRoute
            xor edx, edx
            mov ecx, esi
            push 0
            push 0
            push 0
            call ProcessFollowPath
    L004ac9d2:
            pop edi
            pop esi
            pop ebp
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004ac540
void C2_NAKED C2_HOOK_FASTCALL ProcessLevitate(tOpponent_spec* pOpponent_spec, tProcess_objective_command pCommand) {
#ifndef CARPOCALYPSE2_MATCHING
    float t;
    float terminal_time;
    float y;

    switch (pCommand) {
    case ePOC_start:
        DoNotDprintf_opponent("%s: ProcessLevitate() - new objective started", pOpponent_spec->car_spec->driver_name);
        pOpponent_spec->levitate_data.waiting_to_levitate = 1;
        pOpponent_spec->car_spec->brake_force = 15.f * pOpponent_spec->car_spec->collision_info->M;
        pOpponent_spec->car_spec->acc_force = 0.f;
        pOpponent_spec->levitate_data.time_started = gTime_stamp_for_this_munging;
        break;

    case ePOC_run:
        if (pOpponent_spec->levitate_data.waiting_to_levitate) {
            if ((BrVector3Length(&pOpponent_spec->car_spec->collision_info->v) < .01f && BrVector3Length(&pOpponent_spec->car_spec->collision_info->omega) < 1.f) || gTime_stamp_for_this_munging - pOpponent_spec->levitate_data.time_started > 4000) {
                pOpponent_spec->levitate_data.waiting_to_levitate = 0;
                pOpponent_spec->levitate_data.time_started = gTime_stamp_for_this_munging;
                pOpponent_spec->levitate_data.initial_y = pOpponent_spec->car_spec->car_master_actor->t.t.translate.t.v[1];
                if (pOpponent_spec->car_spec->has_been_stolen) {
                    NewTextHeadupSlot(4, 250, 5000, -4, GetMiscString(170));
                }
            } else {
                pOpponent_spec->car_spec->brake_force = 15.f * pOpponent_spec->car_spec->collision_info->M;
                pOpponent_spec->car_spec->acc_force = 0.f;
                BrVector3InvScale(&pOpponent_spec->car_spec->collision_info->omega, &pOpponent_spec->car_spec->collision_info->omega,
                    pow(2.f, gFrame_period_for_this_munging / 1000.0));
            }
        }
        if (!pOpponent_spec->levitate_data.waiting_to_levitate) {
            TurnOpponentPhysicsOff(pOpponent_spec);
            t = (gTime_stamp_for_this_munging - pOpponent_spec->levitate_data.time_started) / 1000.0;
            terminal_time = 20.f;
            if (t < terminal_time) {
                y = t * t * .5f / 2.f;
            } else {
                y = terminal_time * terminal_time * 0.5 / 2.0 + (t - terminal_time) * 10.0;
            }
            pOpponent_spec->car_spec->car_master_actor->t.t.translate.t.v[1] = pOpponent_spec->levitate_data.initial_y + y;
            if (y > 200.f) {
                pOpponent_spec->finished_for_this_race = 1;
            }
        }
        break;
    case ePOC_die:
        return;
    default:
        break;
    }
#else
    __asm {
            sub esp, 8
            mov eax, edx
            push ebx
            push ebp
            push esi
            push edi
            xor edi, edi
            mov esi, ecx
            sub eax, edi
            je L004ac82e
            dec eax
            jne L004ac87c
            mov ebx, dword ptr [esi + 0x18c]
            test bl, 1
            je L004ac72c
            mov edx, dword ptr [esi + 8]
            mov ecx, dword ptr [edx + 8]
            fld dword ptr [ecx + 0x68]
            fld dword ptr [ecx + 0x6c]
            fld dword ptr [ecx + 0x70]
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
            fcomp dword ptr [gOppo_pursuit_reverse_acceleration+0x4]
            fnstsw ax
            test ah, 1
            je L004ac5f1
            fld dword ptr [ecx + 0x74]
            fld dword ptr [ecx + 0x78]
            fld dword ptr [ecx + 0x7c]
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
            fcomp dword ptr [gOppo_pursuit_one]
            fnstsw ax
            test ah, 1
            jne L004ac605
    L004ac5f1:
            mov eax, dword ptr [gTime_stamp_for_this_munging]
            mov ebp, dword ptr [esi + 0x188]
            sub eax, ebp
            cmp eax, 0xfa0
            jbe L004ac65a
    L004ac605:
            and ebx, 0xfffffffe
            mov dword ptr [esi + 0x18c], ebx
            mov ecx, dword ptr [gTime_stamp_for_this_munging]
            mov dword ptr [esi + 0x188], ecx
            mov eax, dword ptr [edx + 0x10]
            mov ecx, dword ptr [eax + 0x54]
            mov dword ptr [esi + 0x184], ecx
            mov eax, dword ptr [edx + 0x1cc]
            cmp eax, edi
            je L004ac72c
            mov ecx, 0xaa
            call GetMiscString
            mov edx, 0xfa
            mov ecx, 4
            push eax
            push -4
            push 0x1388
            call NewTextHeadupSlot
            jmp L004ac72c
    L004ac65a:
            fld dword ptr [ecx + 4]
            fmul dword ptr [gOppo_pursuit_reverse_brake]
            fld qword ptr [gOppo_pursuit_reverse_acceleration+0xc]
            fxch st(1)
            fstp dword ptr [edx + 0x12c0]
            mov edx, dword ptr [esi + 8]
            mov dword ptr [esp + 0x14], edi
            mov dword ptr [edx + 0x12b8], edi
            mov eax, dword ptr [esi + 8]
            mov ecx, dword ptr [gFrame_period_for_this_munging]
            mov ebp, dword ptr [eax + 8]
            mov dword ptr [esp + 0x10], ecx
            fld dword ptr [ebp + 0x74]
            fild qword ptr [esp + 0x10]
            fmul qword ptr [gOppo_pursuit_reverse_acceleration+0x14]
            fxch st(1)
            fxch st(2)
            fxch st(1)
            call _CIpow
            fst dword ptr [esp + 0x10]
            fdivp st(1), st(0)
            mov dword ptr [esp + 0x14], edi
            fstp dword ptr [ebp + 0x74]
            mov eax, dword ptr [gFrame_period_for_this_munging]
            mov edx, dword ptr [esi + 8]
            mov dword ptr [esp + 0x10], eax
            fild qword ptr [esp + 0x10]
            fld qword ptr [gOppo_pursuit_reverse_acceleration+0xc]
            fxch st(1)
            fmul qword ptr [gOppo_pursuit_reverse_acceleration+0x14]
            mov ebp, dword ptr [edx + 8]
            fld dword ptr [ebp + 0x78]
            fxch st(1)
            fxch st(1)
            fxch st(2)
            fxch st(1)
            call _CIpow
            fst dword ptr [esp + 0x10]
            fdivp st(1), st(0)
            mov dword ptr [esp + 0x14], edi
            fstp dword ptr [ebp + 0x78]
            mov edx, dword ptr [gFrame_period_for_this_munging]
            mov ecx, dword ptr [esi + 8]
            mov dword ptr [esp + 0x10], edx
            fild qword ptr [esp + 0x10]
            fld qword ptr [gOppo_pursuit_reverse_acceleration+0xc]
            fxch st(1)
            fmul qword ptr [gOppo_pursuit_reverse_acceleration+0x14]
            mov ebp, dword ptr [ecx + 8]
            fld dword ptr [ebp + 0x7c]
            fxch st(1)
            fxch st(1)
            fxch st(2)
            fxch st(1)
            call _CIpow
            fst dword ptr [esp + 0x10]
            fdivp st(1), st(0)
            fstp dword ptr [ebp + 0x7c]
    L004ac72c:
            test byte ptr [esi + 0x18c], 1
            jne L004ac87c
            mov eax, dword ptr [esi + 8]
            mov ecx, dword ptr [eax + 8]
            mov dword ptr [eax + 0x12b8], edi
            mov dword ptr [eax + 0x12c0], edi
            mov dword ptr [eax + 0x1254], edi
            mov dword ptr [eax + 0x135c], edi
            mov dword ptr [eax + 0x1340], edi
            mov dword ptr [ecx + 0x74], edi
            mov edx, dword ptr [eax + 8]
            mov dword ptr [edx + 0x78], edi
            mov ecx, dword ptr [eax + 8]
            mov dword ptr [ecx + 0x7c], edi
            mov edx, dword ptr [eax + 8]
            mov dword ptr [edx + 0x68], edi
            mov ecx, dword ptr [eax + 8]
            mov dword ptr [ecx + 0x6c], edi
            mov edx, dword ptr [eax + 8]
            mov dword ptr [edx + 0x70], edi
            mov eax, dword ptr [esi + 0xb0]
            test al, 8
            je L004ac7ad
            and al, 0xf7
            xor edx, edx
            mov dword ptr [esi + 0xb0], eax
            mov eax, dword ptr [esi + 8]
            mov ecx, dword ptr [eax + 0x10]
            mov byte ptr [ecx + 0x20], 1
            mov ecx, esi
            call PipeSingleOppoRenderage
            mov dword ptr [gActive_car_list_rebuild_required], 1
    L004ac7ad:
            mov edx, dword ptr [gTime_stamp_for_this_munging]
            mov eax, dword ptr [esi + 0x188]
            sub edx, eax
            mov dword ptr [esp + 0x14], edi
            mov dword ptr [esp + 0x10], edx
            fild qword ptr [esp + 0x10]
            fmul qword ptr [gOppo_pursuit_reverse_acceleration+0x14]
            fcom dword ptr [gOppo_pursuit_trail_distance]
            fnstsw ax
            test ah, 1
            je L004ac7e6
            fld st(0)
            fmulp st(1), st(0)
            fmul dword ptr [gOppo_pursuit_reverse_acceleration+0x1c]
            jmp L004ac7f8
    L004ac7e6:
            fsub dword ptr [gOppo_pursuit_trail_distance]
            fmul qword ptr [gOppo_pursuit_reverse_acceleration+0x24]
            fsub qword ptr [gOppo_pursuit_reverse_acceleration+0x2c]
    L004ac7f8:
            fcom dword ptr [gOppo_pursuit_reverse_acceleration+0x34]
            fld dword ptr [esi + 0x184]
            mov eax, dword ptr [esi + 8]
            fadd st(0), st(1)
            mov ecx, dword ptr [eax + 0x10]
            fnstsw ax
            fstp dword ptr [ecx + 0x54]
            test ah, 0x41
            fstp st(0)
            jne L004ac87c
            mov eax, dword ptr [esi + 0xb0]
            or al, 2
            mov dword ptr [esi + 0xb0], eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 8
            ret
    L004ac82e:
            mov edx, dword ptr [esi + 8]
            add edx, 0xec
            push edx
            push offset gOpponent_nastyness_frigger+0xd3c
            call DoNotDprintf_opponent
            mov eax, dword ptr [esi + 0x18c]
            add esp, 8
            or al, 1
            mov dword ptr [esi + 0x18c], eax
            mov eax, dword ptr [esi + 8]
            mov ecx, dword ptr [eax + 8]
            fld dword ptr [ecx + 4]
            fmul dword ptr [gOppo_pursuit_reverse_brake]
            fstp dword ptr [eax + 0x12c0]
            mov edx, dword ptr [esi + 8]
            mov dword ptr [edx + 0x12b8], edi
            mov eax, dword ptr [gTime_stamp_for_this_munging]
            mov dword ptr [esi + 0x188], eax
    L004ac87c:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 8
            ret
    }
#endif
}

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x004a87a0
int C2_HOOK_FASTCALL RematerialiseOpponent(tOpponent_spec* pOpponent_spec, br_scalar pSpeed) {

    NOT_IMPLEMENTED();
    return 0;
}
#pragma auto_inline(on)

// FUNCTION: CARMA2_HW 0x004a8170
int C2_NAKED C2_HOOK_FASTCALL RematerialiseOpponentOnNearestSection(tOpponent_spec* pOpponent_spec, br_scalar pSpeed) {
#ifndef CARPOCALYPSE2_MATCHING
    br_vector3 intersect;
    br_vector3 direction_v;
    br_vector3 car_to_end;
    br_vector3* finish;
    br_scalar distance;
    br_scalar distance_to_end;
    tS16 section_no;

    if (pOpponent_spec->physics_me) {
        DoNotDprintf_opponent("%s: Actually, we're already materialised", pOpponent_spec->car_spec->driver_name);
        return 1;
    }
    section_no = FindNearestPathSection(&pOpponent_spec->car_spec->car_master_actor->t.t.translate.t, &direction_v, &intersect, &distance);
    finish = &gProgram_state.AI_vehicles.path_nodes[gProgram_state.AI_vehicles.path_sections[section_no].node_indices[1]].pos;
    BrVector3Copy(&pOpponent_spec->car_spec->car_master_actor->t.t.translate.t, &intersect);
    PointActorAlongThisBloodyVector(pOpponent_spec->car_spec->car_master_actor, &direction_v);
    BrVector3Sub(&car_to_end, finish, &pOpponent_spec->car_spec->car_master_actor->t.t.translate.t);
    if (RematerialiseOpponent(pOpponent_spec, pSpeed)) {
        pOpponent_spec->car_spec->brake_force = 0.0f;
        pOpponent_spec->car_spec->acc_force = 0.0f;

        distance_to_end = BrVector3Length(&car_to_end);
        if (distance_to_end < 5.0f) {
            pOpponent_spec->car_spec->brake_force = pOpponent_spec->car_spec->collision_info->M * 15.0f;
        } else {
            pOpponent_spec->car_spec->acc_force = pOpponent_spec->car_spec->collision_info->M / 2.0f;
        }
    }
    return 0;
#else
    __asm {
            sub esp, 0x28
            push esi
            mov esi, ecx
            test byte ptr [esi + 0xb0], 8
            je L004a81a1
            mov eax, dword ptr [esi + 8]
            add eax, 0xec
            push eax
            push offset gOpponent_nastyness_frigger+0x10c
            call DoNotDprintf_opponent
            add esp, 8
            mov eax, 1
            pop esi
            add esp, 0x28
            ret 4
    L004a81a1:
            lea ecx, [esp + 4]
            push edi
            push ecx
            mov ecx, dword ptr [esi + 8]
            lea edx, [esp + 0x1c]
            lea eax, [esp + 0x28]
            push edx
            mov edx, dword ptr [ecx + 0x10]
            push eax
            add edx, 0x50
            xor ecx, ecx
            call FindNearestGeneralSection
            movsx eax, ax
            mov ecx, dword ptr [esi + 8]
            lea edx, [eax + eax*4]
            mov eax, dword ptr [gProgram_state+0x5ec4]
            movsx edi, word ptr [eax + edx*4 + 2]
            mov eax, dword ptr [gProgram_state+0x5ec0]
            mov edx, dword ptr [ecx + 0x10]
            shl edi, 5
            add edi, eax
            mov eax, dword ptr [esp + 0x18]
            mov dword ptr [edx + 0x50], eax
            mov ecx, dword ptr [esi + 8]
            mov eax, dword ptr [esp + 0x1c]
            mov edx, dword ptr [ecx + 0x10]
            mov dword ptr [edx + 0x54], eax
            mov ecx, dword ptr [esi + 8]
            mov eax, dword ptr [esp + 0x20]
            mov edx, dword ptr [ecx + 0x10]
            mov dword ptr [edx + 0x58], eax
            mov ecx, dword ptr [esi + 8]
            lea edx, [esp + 0x24]
            mov ecx, dword ptr [ecx + 0x10]
            call PointActorAlongThisBloodyVector
            mov edx, dword ptr [esi + 8]
            mov ecx, esi
            _emit 0xd9
        _emit 0x07
            mov eax, dword ptr [edx + 0x10]
            fsub dword ptr [eax + 0x50]
            fstp dword ptr [esp + 0xc]
            fld dword ptr [edi + 4]
            fsub dword ptr [eax + 0x54]
            fstp dword ptr [esp + 0x10]
            fld dword ptr [edi + 8]
            fsub dword ptr [eax + 0x58]
            mov eax, dword ptr [esp + 0x34]
            push eax
            fstp dword ptr [esp + 0x18]
            call RematerialiseOpponent
            test eax, eax
            je L004a82bc
            fld dword ptr [esp + 0x10]
            fld dword ptr [esp + 0xc]
            fmul dword ptr [esp + 0xc]
            fld dword ptr [esp + 0x14]
            fxch st(2)
            fmul dword ptr [esp + 0x10]
            fxch st(2)
            fmul dword ptr [esp + 0x14]
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            mov ecx, dword ptr [esi + 8]
            xor eax, eax
            faddp st(1), st(0)
            mov dword ptr [ecx + 0x12c0], eax
            mov edx, dword ptr [esi + 8]
            fsqrt
            mov dword ptr [edx + 0x12b8], eax
            mov esi, dword ptr [esi + 8]
            fcomp dword ptr [gOppo_pursuit_one+0x4]
            fnstsw ax
            test ah, 1
            je L004a82aa
            mov eax, dword ptr [esi + 8]
            pop edi
            fld dword ptr [eax + 4]
            fmul dword ptr [gOppo_pursuit_reverse_brake]
            xor eax, eax
            fstp dword ptr [esi + 0x12c0]
            pop esi
            add esp, 0x28
            ret 4
    L004a82aa:
            mov ecx, dword ptr [esi + 8]
            fld dword ptr [ecx + 4]
            fmul dword ptr [gOppo_pursuit_velocity_scale]
            fstp dword ptr [esi + 0x12b8]
    L004a82bc:
            pop edi
            xor eax, eax
            pop esi
            add esp, 0x28
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
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004abc00
void C2_HOOK_FASTCALL ProcessWaitForSomeHaplessSod(tOpponent_spec* pOpponent_spec, tProcess_objective_command pCommand) {
    switch (pCommand) {
    case ePOC_start:
        pOpponent_spec->car_spec->brake_force = pOpponent_spec->car_spec->collision_info->M * gOppo_pursuit_reverse_brake;
        return;
    case ePOC_run:
        pOpponent_spec->car_spec->brake_force = pOpponent_spec->car_spec->collision_info->M * gOppo_pursuit_reverse_brake;
        return;
    case ePOC_die:
        return;
    }
}

// FUNCTION: CARMA2_HW 0x004abf60
void C2_NAKED C2_HOOK_FASTCALL CalcReturnToStartPointRoute(tOpponent_spec* pOpponent_spec) {
#ifndef CARPOCALYPSE2_MATCHING
    br_vector3 direction;
    br_vector3 intersect;
    br_scalar distance;
    br_scalar scale;
    tRoute_section route[10];
    tRoute_section best[10];
    tS16 section;
    int best_count;
    int count;

    C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(pOpponent_spec->next_sections) != 12);
    C2_HOOK_BUG_ON(sizeof(tRoute_section) != 4);

    ClearOpponentsProjectedRoute(pOpponent_spec);
    section = FindNearestPathSection(&pOpponent_spec->car_spec->car_master_actor->t.t.translate.t, &direction, &intersect, &distance);
    scale = BR_LENGTH3(direction.v[0], direction.v[1], direction.v[2]);
    if (scale > DAT_0058a4e8) {
        scale = DAT_0058a4a0 / scale;
        direction.v[0] = direction.v[0] * scale;
        direction.v[1] = direction.v[1] * scale;
        direction.v[2] = direction.v[2] * scale;
    } else {
        direction.v[0] = 1.0f;
        direction.v[1] = 0.0f;
        direction.v[2] = 0.0f;
    }
    if (BrVector3Dot(&pOpponent_spec->car_spec->direction, &direction) > gOppo_render_zero) {
        AddToOpponentsProjectedRoute(pOpponent_spec, section, 1);
    } else {
        AddToOpponentsProjectedRoute(pOpponent_spec, section, 0);
    }
    route[0].section_no = pOpponent_spec->next_sections[pOpponent_spec->nnext_sections - 1].section_no;
    route[0].direction = pOpponent_spec->next_sections[pOpponent_spec->nnext_sections - 1].direction;
    gSFS_cycles_this_time = 0;
    gSFS_count++;
    SearchForSection(route, best, &best_count, pOpponent_spec->return_to_start_data.section_no, 1, 0.0f, pOpponent_spec);
    gSFS_total_cycles += gSFS_cycles_this_time;
    if (gSFS_cycles_this_time > gSFS_max_cycles) {
        gSFS_max_cycles = gSFS_cycles_this_time;
    }
    DoNotDprintf_opponent(">>>SearchForSection() - max %d, avg %.1f", gSFS_max_cycles, (double)gSFS_total_cycles / (double)gSFS_count);
    if (best_count > 1) {
        count = MIN(best_count - 1, (int)CARPOCALYPSE2_ASIZE(pOpponent_spec->next_sections) - pOpponent_spec->nnext_sections);
        memcpy(&pOpponent_spec->next_sections[pOpponent_spec->nnext_sections], &best[1], count * sizeof(tRoute_section));
        pOpponent_spec->nnext_sections += count;
        TopUpRandomRoute(pOpponent_spec, 2);
    } else {
        DoNotDprintf_opponent("%s: CalcReturnToStartPointRoute() - SearchForSection() produced bugger all", pOpponent_spec->car_spec->driver_name);
        if (pOpponent_spec->nnext_sections <= (int)CARPOCALYPSE2_ASIZE(pOpponent_spec->next_sections)) {
            TopUpRandomRoute(pOpponent_spec, (int)CARPOCALYPSE2_ASIZE(pOpponent_spec->next_sections) + 2 - pOpponent_spec->nnext_sections);
        }
    }
#else
    __asm {
            sub esp, 0x70
            lea eax, [esp + 4]
            lea edx, [esp + 8]
            push ebp
            push esi
            mov ebp, ecx
            push edi
            push eax
            lea ecx, [esp + 0x24]
            mov eax, dword ptr [ebp + 8]
            push ecx
            mov dword ptr [ebp + 0x7c], 0
            push edx
            mov edx, dword ptr [eax + 0x10]
            xor ecx, ecx
            add edx, 0x50
            call FindNearestGeneralSection
            fld dword ptr [esp + 0x14]
            fmul dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x18]
            fmul dword ptr [esp + 0x18]
            mov ecx, eax
            faddp st(1), st(0)
            fld dword ptr [esp + 0x1c]
            fmul dword ptr [esp + 0x1c]
            faddp st(1), st(0)
            fsqrt
            fcom dword ptr [DAT_0058a4e8]
            fnstsw ax
            test ah, 0x41
            jne L004abfe9
            fdivr qword ptr [DAT_0058a4a0]
            fld st(0)
            fld st(1)
            fxch st(1)
            fmul dword ptr [esp + 0x14]
            fxch st(1)
            fmul dword ptr [esp + 0x18]
            fxch st(2)
            fmul dword ptr [esp + 0x1c]
            fxch st(1)
            fstp dword ptr [esp + 0x14]
            fxch st(1)
            fstp dword ptr [esp + 0x18]
            fstp dword ptr [esp + 0x1c]
            jmp L004ac003
    L004abfe9:
            fstp st(0)
            mov dword ptr [esp + 0x14], 0x3f800000
            mov dword ptr [esp + 0x18], 0
            mov dword ptr [esp + 0x1c], 0
    L004ac003:
            mov eax, dword ptr [ebp + 8]
            fld dword ptr [eax + 0x78]
            fld dword ptr [eax + 0x70]
            fld dword ptr [eax + 0x74]
            fxch st(2)
            fmul dword ptr [esp + 0x1c]
            fxch st(1)
            fmul dword ptr [esp + 0x14]
            fxch st(2)
            fmul dword ptr [esp + 0x18]
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fcomp dword ptr [gOppo_render_zero]
            fnstsw ax
            test ah, 0x41
            mov eax, dword ptr [ebp + 0x7c]
            jne L004ac053
            cmp eax, 0xc
            jge L004ac06e
            mov word ptr [ebp + eax*4 + 0x80], cx
            mov ecx, dword ptr [ebp + 0x7c]
            mov byte ptr [ebp + ecx*4 + 0x82], 1
            jmp L004ac06b
    L004ac053:
            cmp eax, 0xc
            jge L004ac06e
            mov word ptr [ebp + eax*4 + 0x80], cx
            mov edx, dword ptr [ebp + 0x7c]
            mov byte ptr [ebp + edx*4 + 0x82], 0
    L004ac06b:
            inc dword ptr [ebp + 0x7c]
    L004ac06e:
            mov eax, dword ptr [ebp + 0x7c]
            push ebp
            push 0
            push 1
            mov cx, word ptr [ebp + eax*4 + 0x7c]
            mov word ptr [esp + 0x38], cx
            mov dl, byte ptr [ebp + eax*4 + 0x7e]
            mov eax, dword ptr [gSFS_count]
            mov dword ptr [gSFS_cycles_this_time], 0
            inc eax
            lea ecx, [esp + 0x18]
            mov dword ptr [gSFS_count], eax
            mov ax, word ptr [ebp + 0x1a0]
            push eax
            mov byte ptr [esp + 0x3e], dl
            push ecx
            lea edx, [esp + 0x68]
            lea ecx, [esp + 0x40]
            call SearchForSection
            mov eax, dword ptr [gSFS_cycles_this_time]
            mov esi, dword ptr [gSFS_total_cycles]
            mov ecx, dword ptr [gSFS_max_cycles]
            add esi, eax
            cmp eax, ecx
            mov dword ptr [gSFS_total_cycles], esi
            jle L004ac0dc
            mov ecx, eax
            mov dword ptr [gSFS_max_cycles], ecx
    L004ac0dc:
            fild dword ptr [gSFS_total_cycles]
            sub esp, 8
            fidiv dword ptr [gSFS_count]
            _emit 0xdd
        _emit 0x1c
        _emit 0x24
            push ecx
            push offset gOpponent_nastyness_frigger+0xb0c
            call DoNotDprintf_opponent
            mov ecx, dword ptr [esp + 0x1c]
            add esp, 0x10
            cmp ecx, 1
            jle L004ac140
            mov edx, dword ptr [ebp + 0x7c]
            mov eax, 0xc
            sub eax, edx
            dec ecx
            cmp ecx, eax
            jge L004ac116
            mov eax, ecx
    L004ac116:
            mov ecx, eax
            lea esi, [esp + 0x58]
            lea edi, [ebp + edx*4 + 0x80]
            mov edx, 2
            rep movsd
            mov ecx, dword ptr [ebp + 0x7c]
            add ecx, eax
            mov dword ptr [ebp + 0x7c], ecx
            mov ecx, ebp
            call TopUpRandomRoute
            pop edi
            pop esi
            pop ebp
            add esp, 0x70
            ret
    L004ac140:
            mov edx, dword ptr [ebp + 8]
            add edx, 0xec
            push edx
            push offset gOpponent_nastyness_frigger+0xac0
            call DoNotDprintf_opponent
            mov eax, dword ptr [ebp + 0x7c]
            add esp, 8
            cmp eax, 0xc
            jg L004ac16d
            mov edx, 0xe
            mov ecx, ebp
            sub edx, eax
            call TopUpRandomRoute
    L004ac16d:
            pop edi
            pop esi
            pop ebp
            add esp, 0x70
            ret
    }
#endif
}

int C2_HOOK_FASTCALL TeleportCopToStart(tOpponent_spec* pOpponent_spec) {
    br_vector3 wank;

    if (!pOpponent_spec->cheating || !CAR_SPEC_IS_ROZZER(pOpponent_spec->car_spec)) {
        return 0;
    }
    BrVector3Sub(&wank, &gProgram_state.current_car.car_master_actor->t.t.translate.t, &pOpponent_spec->start_pos);
    if (BrVector3Length(&wank) <= gIn_view_distance) {
        return 0;
    }
    BrVector3Copy(&pOpponent_spec->car_spec->car_master_actor->t.t.translate.t, &pOpponent_spec->start_pos);
    PointActorAlongThisBloodyVector(pOpponent_spec->car_spec->car_master_actor, &pOpponent_spec->start_direction);
    pOpponent_spec->physics_me = 0;
    RematerialiseOpponent(pOpponent_spec, 0.0);
    TurnOpponentPhysicsOff(pOpponent_spec);
    RebuildActiveCarList();
    NewObjective(pOpponent_spec, eOOT_wait_for_some_hapless_sod);
    return 1;
}

// FUNCTION: CARMA2_HW 0x004abc40
void C2_NAKED C2_HOOK_FASTCALL ProcessReturnToStart(tOpponent_spec* pOpponent_spec, tProcess_objective_command pCommand) {
#ifndef CARPOCALYPSE2_MATCHING
    br_vector3 section_v;
    br_vector3 our_pos_xz;
    br_vector3 cop_to_start;
    br_scalar distance;
    int res;

    switch (pCommand) {
    case ePOC_start:
        DoNotDprintf_opponent("%s: ProcessReturnToStart() - new objective started", pOpponent_spec->car_spec->driver_name);
        pOpponent_spec->return_to_start_data.waiting_near_start = 0;
        pOpponent_spec->return_to_start_data.section_no = FindNearestPathSection(&pOpponent_spec->start_pos, &section_v, &pOpponent_spec->return_to_start_data.nearest_path_point, &distance);
        pOpponent_spec->return_to_start_data.nearest_path_point.v[1] = 0.0;
        CalcReturnToStartPointRoute(pOpponent_spec);
        ProcessFollowPath(pOpponent_spec, ePOC_start, 0, 0, 0);
        break;
    case ePOC_run:
        if (TeleportCopToStart(pOpponent_spec)) {
            break;
        }
        if (pOpponent_spec->return_to_start_data.waiting_near_start) {
            pOpponent_spec->car_spec->brake_force = pOpponent_spec->car_spec->collision_info->M * 15.0f;
        } else {
            BrVector3Copy(&our_pos_xz, &pOpponent_spec->car_spec->car_master_actor->t.t.translate.t);
            our_pos_xz.v[1] = 0.0f;
            BrVector3Sub(&cop_to_start, &pOpponent_spec->start_pos, &our_pos_xz);
            if (BrVector3Length(&cop_to_start) >= 10.0) {
                if (pOpponent_spec->follow_path_data.section_no > 20000) {
                    ShiftOpponentsProjectedRoute(pOpponent_spec, pOpponent_spec->follow_path_data.section_no - 20000);
                    pOpponent_spec->follow_path_data.section_no = 20000;
                }
                if (pOpponent_spec->nnext_sections < CARPOCALYPSE2_ASIZE(pOpponent_spec->next_sections) - 1) {
                    CalcReturnToStartPointRoute(pOpponent_spec);
                }
                res = ProcessFollowPath(pOpponent_spec, ePOC_run, 0, 0, 0);
                if (res == eFPR_given_up || res == eFPR_end_of_path) {
                    if (res == eFPR_given_up) {
                        DoNotDprintf_opponent("%s: Restarting return_to_start route because ProcessFollowPath() gave up.", pOpponent_spec->car_spec->driver_name);
                    } else {
                        DoNotDprintf_opponent("%s: Restarting return_to_start route because ran out of path!", pOpponent_spec->car_spec->driver_name);
                    }
                    ClearOpponentsProjectedRoute(pOpponent_spec);
                    CalcReturnToStartPointRoute(pOpponent_spec);
                    ProcessFollowPath(pOpponent_spec, ePOC_start, 0, 0, 0);
                }
            } else {
                pOpponent_spec->return_to_start_data.waiting_near_start = 1;
                pOpponent_spec->car_spec->brake_force = pOpponent_spec->car_spec->collision_info->M * 15.0f;
            }
        }
        break;
    default:
        break;
    }
#else
    __asm {
            sub esp, 0x10
            mov eax, edx
            push ebx
            xor ebx, ebx
            push ebp
            push esi
            sub eax, ebx
            push edi
            mov esi, ecx
            je L004abef6
            dec eax
            jne L004abf4d
            test byte ptr [esi + 0xb0], 0x20
            je L004abda4
            mov ecx, dword ptr [esi + 8]
            xor eax, eax
            mov ax, word ptr [ecx + 0x80]
            and al, bl
            cmp eax, 0x300
            jne L004abda4
            mov eax, dword ptr [gProgram_state+0xbc]
            fld dword ptr [eax + 0x54]
            fld dword ptr [eax + 0x50]
            fsub dword ptr [esi + 0x1c]
            fxch st(1)
            fsub dword ptr [esi + 0x20]
            fld dword ptr [eax + 0x58]
            fsub dword ptr [esi + 0x24]
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
            fsqrt
            fstp st(1)
            fcomp dword ptr [gIn_view_distance]
            fnstsw ax
            test ah, 0x41
            jne L004abda4
            mov ecx, dword ptr [ecx + 0x10]
            mov edx, dword ptr [esi + 0x1c]
            mov dword ptr [ecx + 0x50], edx
            mov eax, dword ptr [esi + 8]
            mov edx, dword ptr [esi + 0x20]
            mov ecx, dword ptr [eax + 0x10]
            mov dword ptr [ecx + 0x54], edx
            mov eax, dword ptr [esi + 8]
            mov edx, dword ptr [esi + 0x24]
            mov ecx, dword ptr [eax + 0x10]
            mov dword ptr [ecx + 0x58], edx
            mov eax, dword ptr [esi + 8]
            lea edx, [esi + 0x28]
            mov ecx, dword ptr [eax + 0x10]
            call PointActorAlongThisBloodyVector
            mov edi, dword ptr [esi + 0xb0]
            mov ecx, esi
            and edi, 0xfffffff7
            push ebx
            mov dword ptr [esi + 0xb0], edi
            call RematerialiseOpponent
            mov eax, dword ptr [esi + 8]
            mov ecx, dword ptr [eax + 8]
            mov dword ptr [eax + 0x12b8], ebx
            mov dword ptr [eax + 0x12c0], ebx
            mov dword ptr [eax + 0x1254], ebx
            mov dword ptr [eax + 0x135c], ebx
            mov dword ptr [eax + 0x1340], ebx
            mov dword ptr [ecx + 0x74], ebx
            mov edx, dword ptr [eax + 8]
            mov dword ptr [edx + 0x78], ebx
            mov ecx, dword ptr [eax + 8]
            mov dword ptr [ecx + 0x7c], ebx
            mov edx, dword ptr [eax + 8]
            mov dword ptr [edx + 0x68], ebx
            mov ecx, dword ptr [eax + 8]
            mov dword ptr [ecx + 0x6c], ebx
            mov edx, dword ptr [eax + 8]
            mov dword ptr [edx + 0x70], ebx
            mov eax, dword ptr [esi + 0xb0]
            test al, 8
            je L004abd8c
            and al, 0xf7
            xor edx, edx
            mov dword ptr [esi + 0xb0], eax
            mov eax, dword ptr [esi + 8]
            mov ecx, dword ptr [eax + 0x10]
            mov byte ptr [ecx + 0x20], 1
            mov ecx, esi
            call PipeSingleOppoRenderage
            mov dword ptr [gActive_car_list_rebuild_required], 1
    L004abd8c:
            call RebuildActiveCarList
            push 8
            push esi
            call NewObjective
            add esp, 8
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x10
            ret
    L004abda4:
            cmp byte ptr [esi + 0x1a2], bl
            jne L004abed9
            mov ecx, dword ptr [esi + 8]
            mov eax, dword ptr [ecx + 0x10]
            fld dword ptr [eax + 0x50]
            fld dword ptr [esi + 0x20]
            fld dword ptr [eax + 0x58]
            fxch st(2)
            fsubr dword ptr [esi + 0x1c]
            fxch st(2)
            fsubr dword ptr [esi + 0x24]
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
            fsqrt
            fstp st(1)
            fcomp dword ptr [DAT_0058a4ec]
            fnstsw ax
            test ah, 1
            je L004abe1d
            mov byte ptr [esi + 0x1a2], 1
            mov edx, dword ptr [ecx + 8]
            fld dword ptr [edx + 4]
            fmul dword ptr [gOppo_pursuit_reverse_brake]
            fstp dword ptr [ecx + 0x12c0]
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x10
            ret
    L004abe1d:
            mov ax, word ptr [esi + 0xe8]
            cmp ax, 0x4e20
            jle L004abe73
            movsx ebp, ax
            mov eax, dword ptr [esi + 0x7c]
            sub ebp, 0x4e20
            cmp eax, ebp
            jle L004abe6a
            mov edi, 0xc
            sub edi, ebp
            cmp edi, ebx
            jle L004abe67
            lea ecx, [esi + 0x82]
            lea eax, [esi + ebp*4 + 0x80]
    L004abe52:
            mov dx, word ptr [eax]
            add eax, 4
            mov word ptr [ecx - 2], dx
            mov dl, byte ptr [eax - 2]
            mov byte ptr [ecx], dl
            add ecx, 4
            dec edi
            jne L004abe52
    L004abe67:
            sub dword ptr [esi + 0x7c], ebp
    L004abe6a:
            mov word ptr [esi + 0xe8], 0x4e20
    L004abe73:
            cmp dword ptr [esi + 0x7c], 0xa
            jg L004abe80
            mov ecx, esi
            call CalcReturnToStartPointRoute
    L004abe80:
            push ebx
            push ebx
            push ebx
            mov edx, 1
            mov ecx, esi
            call ProcessFollowPath
            cmp eax, 2
            je L004abea2
            cmp eax, 1
            jne L004abf4d
            cmp eax, 2
            jne L004abebd
    L004abea2:
            mov eax, dword ptr [esi + 8]
            add eax, 0xec
            push eax
            push offset gOpponent_nastyness_frigger+0xa74
            call DoNotDprintf_opponent
            add esp, 8
            mov dword ptr [esi + 0x7c], ebx
            jmp L004abf3a
    L004abebd:
            mov ecx, dword ptr [esi + 8]
            add ecx, 0xec
            push ecx
            push offset gOpponent_nastyness_frigger+0xa34
            call DoNotDprintf_opponent
            add esp, 8
            mov dword ptr [esi + 0x7c], ebx
            jmp L004abf3a
    L004abed9:
            mov esi, dword ptr [esi + 8]
            mov edx, dword ptr [esi + 8]
            fld dword ptr [edx + 4]
            fmul dword ptr [gOppo_pursuit_reverse_brake]
            fstp dword ptr [esi + 0x12c0]
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x10
            ret
    L004abef6:
            mov eax, dword ptr [esi + 8]
            add eax, 0xec
            push eax
            push offset gOpponent_nastyness_frigger+0xa00
            call DoNotDprintf_opponent
            add esp, 8
            lea ecx, [esp + 0x10]
            lea edx, [esi + 0x194]
            lea eax, [esp + 0x14]
            push ecx
            push edx
            push eax
            lea edx, [esi + 0x1c]
            xor ecx, ecx
            mov byte ptr [esi + 0x1a2], bl
            call FindNearestGeneralSection
            mov word ptr [esi + 0x1a0], ax
            mov dword ptr [esi + 0x198], ebx
    L004abf3a:
            mov ecx, esi
            call CalcReturnToStartPointRoute
            xor edx, edx
            mov ecx, esi
            push ebx
            push ebx
            push ebx
            call ProcessFollowPath
    L004abf4d:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x10
            ret
    }
#endif
}

void C2_HOOK_FASTCALL ClearOpponentsProjectedRoute(tOpponent_spec* pOpponent_spec) {

    pOpponent_spec->nnext_sections = 0;
}

void C2_HOOK_FASTCALL ProcessCompleteRace(tOpponent_spec* pOpponent_spec, tProcess_objective_command pCommand) {

    switch (pCommand) {
    case ePOC_start:
        DoNotDprintf_opponent("%s: ProcessCompleteRace() - new objective started", pOpponent_spec->car_spec->driver_name);
        ClearOpponentsProjectedRoute(pOpponent_spec);
        CalcRaceRoute(pOpponent_spec);
        ProcessFollowPath(pOpponent_spec, ePOC_start, 0, 0, 0);
        break;
    case ePOC_run:
        if (pOpponent_spec->follow_path_data.section_no > 20000) {
            ShiftOpponentsProjectedRoute(pOpponent_spec, pOpponent_spec->follow_path_data.section_no - 20000);
            pOpponent_spec->follow_path_data.section_no = 20000;
        }
        if (pOpponent_spec->nnext_sections < CARPOCALYPSE2_ASIZE(pOpponent_spec->next_sections) - 1 && !pOpponent_spec->complete_race_data.finished_calcing_race_route) {
            CalcRaceRoute(pOpponent_spec);
        }
        if (pOpponent_spec->nnext_sections == 0) {
            DoNotDprintf_opponent("%s: Giving up following race path because ran out of race path", pOpponent_spec->car_spec->driver_name);
            NewObjective(pOpponent_spec, eOOT_get_near_player);
        } else {
            switch (ProcessFollowPath(pOpponent_spec, ePOC_run, 0, 0, 0)) {
            case eFPR_end_of_path:
                DoNotDprintf_opponent("%s: Giving up following race path because ran out of race path", pOpponent_spec->car_spec->driver_name);
                NewObjective(pOpponent_spec, eOOT_get_near_player);
                break;
            case eFPR_given_up:
                ClearOpponentsProjectedRoute(pOpponent_spec);
                CalcRaceRoute(pOpponent_spec);
                ProcessFollowPath(pOpponent_spec, ePOC_start, 0, 0, 0);
                break;
            default:
                ObjectiveComplete(pOpponent_spec);
                break;
            }
        }
        break;
    default:
        break;
    }
}

void C2_HOOK_FASTCALL ProcessRunAway(tOpponent_spec* pOpponent_spec, tProcess_objective_command pCommand) {
    tS16 section_no;
    br_scalar distance;
    br_vector3 intersect;
    br_vector3 direction_v;
    char str[256];

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tOpponent_spec, run_away_data, 0x190);

    switch (pCommand) {
    case ePOC_start:
        DoNotDprintf_opponent("%s: ProcessRunAway() - new objective started", pOpponent_spec->car_spec->driver_name);
        pOpponent_spec->run_away_data.time_to_stop = gTime_stamp_for_this_munging + 1000 * IRandomBetween(30, 90);
        ClearOpponentsProjectedRoute(pOpponent_spec);
        section_no = FindNearestPathSection(&pOpponent_spec->car_spec->car_master_actor->t.t.translate.t, &direction_v, &intersect, &distance);
        if (BrVector3Dot(&pOpponent_spec->car_spec->direction, &direction_v) < 0.0f) {
            AddToOpponentsProjectedRoute(pOpponent_spec, section_no, 0);
        } else {
            AddToOpponentsProjectedRoute(pOpponent_spec, section_no, 1);
        }
        TopUpRandomRoute(pOpponent_spec, -1);
        ProcessFollowPath(pOpponent_spec, ePOC_start, 0, 0, 0);
        sprintf(str, "%s: Shit! I'm out of here...", pOpponent_spec->car_spec->driver_name);
        break;
    case ePOC_run:
        if (gTime_stamp_for_this_munging <= pOpponent_spec->run_away_data.time_to_stop) {
            if (pOpponent_spec->follow_path_data.section_no > 20000) {
                ShiftOpponentsProjectedRoute(pOpponent_spec, pOpponent_spec->follow_path_data.section_no - 20000);
                pOpponent_spec->follow_path_data.section_no = 20000;
            }
            if (pOpponent_spec->nnext_sections < CARPOCALYPSE2_ASIZE(pOpponent_spec->next_sections)) {
                TopUpRandomRoute(pOpponent_spec, CARPOCALYPSE2_ASIZE(pOpponent_spec->next_sections) - pOpponent_spec->nnext_sections);
            }
            if (ProcessFollowPath(pOpponent_spec, ePOC_run, 0, 0, 0) == eFPR_given_up) {
                ClearOpponentsProjectedRoute(pOpponent_spec);
                section_no = FindNearestPathSection(&pOpponent_spec->car_spec->car_master_actor->t.t.translate.t, &direction_v, &intersect, &distance);
                if (BrVector3Dot(&pOpponent_spec->car_spec->direction, &direction_v) < 0.0f) {
                    AddToOpponentsProjectedRoute(pOpponent_spec, section_no, 0);
                } else {
                    AddToOpponentsProjectedRoute(pOpponent_spec, section_no, 1);
                }
                TopUpRandomRoute(pOpponent_spec, -1);
                ProcessFollowPath(pOpponent_spec, ePOC_start, 0, 0, 0);
            }
        } else {
            ObjectiveComplete(pOpponent_spec);
        }
        break;
    default:
        break;
    }
}

void C2_HOOK_FASTCALL ProcessFrozen(tOpponent_spec* pOpponent_spec, tProcess_objective_command pCommand) {

    switch (pCommand) {
    case ePOC_start:
        DoNotDprintf_opponent("%d ProcessFrozen() - new task started", pOpponent_spec->index);
        DoNotDprintf_opponent("%s: Rematerialising from ePOC_start in ProcessFrozen()...", pOpponent_spec->car_spec->driver_name);
        RematerialiseOpponentOnNearestSection(pOpponent_spec, 0.f);
        pOpponent_spec->car_spec->acc_force = 0.f;
        pOpponent_spec->car_spec->brake_force = 15.f * pOpponent_spec->car_spec->collision_info->M;
        break;
    case ePOC_run:
        pOpponent_spec->car_spec->brake_force = 15.f * pOpponent_spec->car_spec->collision_info->M;
        break;
    case ePOC_die:
        pOpponent_spec->car_spec->brake_force = 0.f;
        break;
    }
}

// FUNCTION: CARMA2_HW 0x004aa900
void C2_HOOK_FASTCALL ProcessCurrentObjective(tOpponent_spec* pOpponent_spec, tProcess_objective_command pCommand) {

    switch (pOpponent_spec->current_objective) {
    case eOOT_none:
        break;
    case eOOT_complete_race:
        ProcessCompleteRace(pOpponent_spec, pCommand);
        break;
    case eOOT_pursue_and_twat:
        ProcessPursueAndTwat(pOpponent_spec, pCommand);
        break;
    case eOOT_run_away:
        ProcessRunAway(pOpponent_spec, pCommand);
        break;
    case eOOT_get_near_player:
        ProcessGetNearPlayer(pOpponent_spec, pCommand);
        break;
    case eOOT_levitate:
        ProcessLevitate(pOpponent_spec, pCommand);
        break;
    case eOOT_knackered_and_freewheeling:
        // FIXME: is keys correct?
        memset(&pOpponent_spec->car_spec->keys, 0, sizeof(pOpponent_spec->car_spec->keys));
        pOpponent_spec->car_spec->acc_force = 0.f;
        pOpponent_spec->car_spec->brake_force = 0.f;
        pOpponent_spec->car_spec->curvature = 0.f;
        break;
    case eOOT_frozen:
        ProcessFrozen(pOpponent_spec, pCommand);
        break;
    case eOOT_wait_for_some_hapless_sod:
        ProcessWaitForSomeHaplessSod(pOpponent_spec, pCommand);
        break;
    case eOOT_return_to_start:
        ProcessReturnToStart(pOpponent_spec, pCommand);
        break;
    default:
        break;
    }
}

// FUNCTION: CARMA2_HW 0x004a9180
void C2_NAKED C2_HOOK_FASTCALL TeleportOpponentToNearestSafeLocation(tOpponent_spec* pOpponent_spec) {
#ifndef CARPOCALYPSE2_MATCHING
    tS16 section_no;
    tU8 section_direction;
    br_scalar distance;
    br_vector3 direction_v;
    br_vector3 intersect;
    int section_counter;
    int found_safe_place;

    found_safe_place = 0;
    section_no = FindNearestGeneralSection(NULL, &pOpponent_spec->car_spec->car_master_actor->t.t.translate.t, &direction_v, &intersect, &distance);
    if (section_no < 0) {
        return;
    }
    pOpponent_spec->last_in_view = 0;
    TurnOpponentPhysicsOff(pOpponent_spec);
    section_direction = BrVector3Dot(&pOpponent_spec->car_spec->direction, &direction_v) < 0.0f;
    ClearOpponentsProjectedRoute(pOpponent_spec);
    AddToOpponentsProjectedRoute(pOpponent_spec, section_no, section_direction);
    TopUpRandomRoute(pOpponent_spec, -1);
    section_counter = 1;
    while (!found_safe_place) {
        BrVector3Copy(&pOpponent_spec->car_spec->car_master_actor->t.t.translate.t, &gProgram_state.AI_vehicles.path_nodes[gProgram_state.AI_vehicles.path_sections[pOpponent_spec->next_sections[section_counter].section_no].node_indices[pOpponent_spec->next_sections[section_counter].direction]].pos);
        CalcOpponentConspicuousnessWithAViewToCheatingLikeFuck(pOpponent_spec);
        if (pOpponent_spec->player_to_oppo_d > gIn_view_distance) {
            found_safe_place = 1;
        }
        section_counter++;
        if (pOpponent_spec->nnext_sections <= section_counter) {
            ShiftOpponentsProjectedRoute(pOpponent_spec, section_counter - 1);
            section_counter = 0;
            TopUpRandomRoute(pOpponent_spec, -1);
        }
    }
#else
    __asm {
            sub esp, 0x20
            push ebx
            push esi
            mov esi, ecx
            lea eax, [esp + 0xc]
            push edi
            push eax
            mov eax, dword ptr [esi + 8]
            lea ecx, [esp + 0x24]
            lea edx, [esp + 0x18]
            push ecx
            push edx
            mov edx, dword ptr [eax + 0x10]
            xor ebx, ebx
            add edx, 0x50
            xor ecx, ecx
            mov dword ptr [esp + 0x18], ebx
            call FindNearestGeneralSection
            mov edi, eax
            cmp di, bx
            jl L004a93ac
            mov eax, dword ptr [esi + 8]
            mov dword ptr [esi + 0x68], ebx
            push ebp
            mov ecx, dword ptr [eax + 8]
            mov dword ptr [eax + 0x12b8], ebx
            mov dword ptr [eax + 0x12c0], ebx
            mov dword ptr [eax + 0x1254], ebx
            mov dword ptr [eax + 0x135c], ebx
            mov dword ptr [eax + 0x1340], ebx
            mov dword ptr [ecx + 0x74], ebx
            mov edx, dword ptr [eax + 8]
            mov dword ptr [edx + 0x78], ebx
            mov ecx, dword ptr [eax + 8]
            mov dword ptr [ecx + 0x7c], ebx
            mov edx, dword ptr [eax + 8]
            mov dword ptr [edx + 0x68], ebx
            mov ecx, dword ptr [eax + 8]
            mov dword ptr [ecx + 0x6c], ebx
            mov edx, dword ptr [eax + 8]
            mov dword ptr [edx + 0x70], ebx
            mov eax, dword ptr [esi + 0xb0]
            test al, 8
            je L004a9230
            and al, 0xf7
            xor edx, edx
            mov dword ptr [esi + 0xb0], eax
            mov eax, dword ptr [esi + 8]
            mov ecx, dword ptr [eax + 0x10]
            mov byte ptr [ecx + 0x20], 1
            mov ecx, esi
            call PipeSingleOppoRenderage
            mov dword ptr [gActive_car_list_rebuild_required], 1
    L004a9230:
            mov eax, dword ptr [esi + 8]
            fld dword ptr [eax + 0x78]
            fld dword ptr [eax + 0x74]
            fld dword ptr [eax + 0x70]
            fxch st(2)
            fmul dword ptr [esp + 0x20]
            fxch st(1)
            fmul dword ptr [esp + 0x1c]
            fxch st(2)
            fmul dword ptr [esp + 0x18]
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fcomp dword ptr [gOppo_render_zero]
            fnstsw ax
            test ah, 1
            je L004a9267
            mov al, 1
            jmp L004a9269
    L004a9267:
            xor al, al
    L004a9269:
            mov dword ptr [esi + 0x7c], ebx
            mov word ptr [esi + 0x80], di
            mov edx, dword ptr [esi + 0x7c]
            mov ecx, esi
            mov byte ptr [esi + edx*4 + 0x82], al
            mov eax, dword ptr [esi + 0x7c]
            inc eax
            or edx, 0xffffffff
            mov dword ptr [esi + 0x7c], eax
            call TopUpRandomRoute
            mov edi, 1
    L004a9293:
            movsx eax, word ptr [esi + edi*4 + 0x80]
            xor ecx, ecx
            mov cl, byte ptr [esi + edi*4 + 0x82]
            lea eax, [eax + eax*4]
            lea edx, [ecx + eax*2]
            mov eax, dword ptr [gProgram_state+0x5ec4]
            movsx ecx, word ptr [eax + edx*2]
            mov edx, dword ptr [esi + 8]
            shl ecx, 5
            mov eax, dword ptr [edx + 0x10]
            mov edx, dword ptr [gProgram_state+0x5ec0]
            mov ecx, dword ptr [ecx + edx]
            mov dword ptr [eax + 0x50], ecx
            movsx eax, word ptr [esi + edi*4 + 0x80]
            lea edx, [eax + eax*4]
            xor eax, eax
            mov al, byte ptr [esi + edi*4 + 0x82]
            lea ecx, [eax + edx*2]
            mov edx, dword ptr [gProgram_state+0x5ec4]
            movsx eax, word ptr [edx + ecx*2]
            mov ecx, dword ptr [esi + 8]
            shl eax, 5
            mov edx, dword ptr [ecx + 0x10]
            mov ecx, dword ptr [gProgram_state+0x5ec0]
            mov eax, dword ptr [eax + ecx + 4]
            mov dword ptr [edx + 0x54], eax
            xor edx, edx
            movsx eax, word ptr [esi + edi*4 + 0x80]
            mov dl, byte ptr [esi + edi*4 + 0x82]
            lea ecx, [eax + eax*4]
            lea eax, [edx + ecx*2]
            mov ecx, dword ptr [gProgram_state+0x5ec4]
            movsx edx, word ptr [ecx + eax*2]
            mov eax, dword ptr [esi + 8]
            shl edx, 5
            mov ecx, dword ptr [eax + 0x10]
            mov eax, dword ptr [gProgram_state+0x5ec0]
            mov edx, dword ptr [edx + eax + 8]
            mov dword ptr [ecx + 0x58], edx
            mov ecx, esi
            call CalcOpponentConspicuousnessWithAViewToCheatingLikeFuck
            fld dword ptr [esi + 0x18]
            fcomp dword ptr [gIn_view_distance]
            fnstsw ax
            test ah, 0x41
            jne L004a9354
            mov dword ptr [esp + 0x10], 1
    L004a9354:
            mov eax, dword ptr [esi + 0x7c]
            inc edi
            cmp edi, eax
            jl L004a93a1
            lea ebp, [edi - 1]
            cmp eax, ebp
            jle L004a9395
            mov edx, 0xc
            sub edx, ebp
            cmp edx, ebx
            jle L004a9392
            lea ecx, [esi + 0x82]
            lea eax, [esi + ebp*4 + 0x80]
            mov edi, edx
    L004a937d:
            mov dx, word ptr [eax]
            add eax, 4
            mov word ptr [ecx - 2], dx
            mov dl, byte ptr [eax - 2]
            mov byte ptr [ecx], dl
            add ecx, 4
            dec edi
            jne L004a937d
    L004a9392:
            sub dword ptr [esi + 0x7c], ebp
    L004a9395:
            or edx, 0xffffffff
            mov ecx, esi
            xor edi, edi
            call TopUpRandomRoute
    L004a93a1:
            cmp dword ptr [esp + 0x10], ebx
            je L004a9293
            pop ebp
    L004a93ac:
            pop edi
            pop esi
            pop ebx
            add esp, 0x20
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

// FUNCTION: CARMA2_HW 0x004aad70
void C2_HOOK_CDECL NewObjective(tOpponent_spec* pOpponent_spec, tOpponent_objective_type pObjective_type, ...) {
    va_list ap;

    if (pOpponent_spec->current_objective != eOOT_none) {
        ProcessCurrentObjective(pOpponent_spec, ePOC_die);
    }
    pOpponent_spec->current_objective = pObjective_type;
    pOpponent_spec->time_this_objective_started = gTime_stamp_for_this_munging;
    pOpponent_spec->time_for_this_objective_to_finish = gTime_stamp_for_this_munging + IRandomBetween(20, 60) * 1000;
    if (pObjective_type == eOOT_pursue_and_twat) {
        pOpponent_spec->time_for_this_objective_to_finish += 90000;
    }
    switch (pObjective_type) {
    case eOOT_complete_race:
        gNum_of_opponents_completing_race += 1;
        break;
    case eOOT_pursue_and_twat:
        va_start(ap, pObjective_type);
        pOpponent_spec->pursue_car_data.pursuee = va_arg(ap, tCar_spec*);
        va_end(ap);
        gNum_of_opponents_pursuing += 1;
        break;
    case eOOT_get_near_player:
        gNum_of_opponents_getting_near += 1;
        break;
    default:
        break;
    }
    DoNotDprintf_opponent("%s: NewObjective() - type %d", pOpponent_spec->car_spec->driver_name, pObjective_type);
    ProcessCurrentObjective(pOpponent_spec, ePOC_start);
}

tS16 C2_HOOK_FASTCALL CalcNextTrailSection(const tOpponent_spec* pOpponent_spec, int pSection) {
    int section_no;
    tPursuee_trail* trail;

    trail = &pOpponent_spec->pursue_car_data.pursuee->my_trail;
    section_no = pSection - 15000;

    if (trail->number_of_nodes - 2 > section_no) {
        return pSection + 1;
    }
    return -1;
}

// FUNCTION: CARMA2_HW 0x004aea30
int C2_HOOK_FASTCALL GetOpponentsNextSection(const tOpponent_spec* pOpponent_spec, tS16 pCurrent_section) {
    if (pCurrent_section >= 20000) {
        if (pOpponent_spec->nnext_sections > pCurrent_section - 19999
                && (pOpponent_spec->cheating || gProgram_state.AI_vehicles.path_sections[pCurrent_section - 19999].type != ePST_cheat_only)) {
            return pCurrent_section + 1;
        }
        return -1;
    }
    if (pCurrent_section >= 15000) {
        int trail_section = pCurrent_section - 15000;

        if (trail_section >= pOpponent_spec->pursue_car_data.pursuee->my_trail.number_of_nodes - 2) {
            trail_section = -1;
        } else {
            trail_section += 15001;
        }
        return (tS16)trail_section;
    }
    return -1;
}

// FUNCTION: CARMA2_HW 0x004aeab0
tU16 C2_NAKED C2_HOOK_FASTCALL GetOpponentsSectionStartNode(const tOpponent_spec* pOpponent, tS16 pSection) {
#ifndef CARPOCALYPSE2_MATCHING
    int section;

    if (pSection >= 20000) {
        section = pSection - 20000;
        if (pOpponent->nnext_sections > section) {
            const tRoute_section* route_section = &pOpponent->next_sections[section];

            return gProgram_state.AI_vehicles.path_sections[route_section->section_no]
                .node_indices[route_section->direction == 0];
        }
    }
    DoNotDprintf_opponent("BIG ERROR - GetOpponentsSectionStartNode() - section not found in next_section array for opponent %s", pOpponent->car_spec->driver_name);
    PDEnterDebugger("BIG ERROR - GetOpponentsSectionStartNode()");
    return 0xffff;
#else
    __asm {
            push ebx
            cmp dx, 0x4e20
            push esi
            jl L004aeaf7
            mov esi, dword ptr [ecx + 0x7c]
            movsx eax, dx
            lea edx, [eax - 0x4e20]
            cmp esi, edx
            jle L004aeaf7
            mov si, word ptr [ecx + eax*4 - 0x13800]
            mov bl, byte ptr [ecx + eax*4 - 0x137fe]
            xor edx, edx
            movsx eax, si
            test bl, bl
            sete dl
            mov ecx, edx
            mov edx, dword ptr [gProgram_state+0x5ec4]
            lea eax, [eax + eax*4]
            lea ecx, [ecx + eax*2]
            mov ax, word ptr [edx + ecx*2]
            pop esi
            pop ebx
            ret
    L004aeaf7:
            mov eax, dword ptr [ecx + 8]
            add eax, 0xec
            push eax
            push offset gOpponent_nastyness_frigger+0x1300
            call DoNotDprintf_opponent
            add esp, 8
            mov ecx, offset gOpponent_nastyness_frigger+0x12d4
            call PDEnterDebugger
            or ax, 0xffff
            pop esi
            pop ebx
            ret
            nop
            nop
            push ebx
            cmp dx, 0x4e20
            push esi
            jl L004aeb5d
            mov esi, dword ptr [ecx + 0x7c]
            movsx eax, dx
            lea edx, [eax - 0x4e20]
            cmp esi, edx
            jle L004aeb5d
            movsx edx, word ptr [ecx + eax*4 - 0x13800]
            mov esi, dword ptr [gProgram_state+0x5ec4]
            xor ebx, ebx
            mov bl, byte ptr [ecx + eax*4 - 0x137fe]
            lea edx, [edx + edx*4]
            lea edx, [esi + edx*4]
            mov ax, word ptr [edx + ebx*2]
            pop esi
            pop ebx
            ret
    L004aeb5d:
            mov eax, dword ptr [ecx + 8]
            add eax, 0xec
            push eax
            push offset gOpponent_nastyness_frigger+0x1394
            call DoNotDprintf_opponent
            add esp, 8
            mov ecx, offset gOpponent_nastyness_frigger+0x1368
            call PDEnterDebugger
            or ax, 0xffff
            pop esi
            pop ebx
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004aeb90
C2_NAKED const br_vector3* C2_HOOK_FASTCALL GetOpponentsSectionStartNodePoint(const tOpponent_spec* pOpponent_spec, tS16 pSection) {
#ifndef CARPOCALYPSE2_MATCHING
    tS16 section_no;
    tS16 node_no;
    int node_index_index;

    if (pSection >= 20000 && pOpponent_spec->nnext_sections > pSection - 20000) {
        section_no = pOpponent_spec->next_sections[pSection - 20000].section_no;
        node_index_index = pOpponent_spec->next_sections[pSection - 20000].direction;
        node_no = gProgram_state.AI_vehicles.path_sections[section_no].node_indices[node_index_index == 0];
        return &gProgram_state.AI_vehicles.path_nodes[node_no].pos;
    }

    if (pSection >= 15000) {
        return &pOpponent_spec->pursue_car_data.pursuee->my_trail.trail_nodes[pSection - 15000];
    }
    if (pSection == 10000) {
        return &pOpponent_spec->pursue_car_data.direct_line_nodes[0].pos;
    }
    DoNotDprintf_opponent("BIG ERROR - GetOpponentsSectionStartNodePoint() - section not found in next_section array for opponent %s", pOpponent_spec->car_spec->driver_name);
    PDEnterDebugger("BIG ERROR - GetOpponentsSectionStartNodePoint()");
    return NULL;
#else
    __asm {
            push ebx
            push esi
            cmp dx, 0x4e20
            push edi
            jl L004aebdf
            mov edi, dword ptr [ecx + 0x7c]
            movsx eax, dx
            lea esi, [eax - 0x4e20]
            cmp edi, esi
            jle L004aebdf
            mov bl, byte ptr [ecx + eax*4 - 0x137fe]
            xor edx, edx
            movsx eax, word ptr [ecx + eax*4 - 0x13800]
            test bl, bl
            sete dl
            lea eax, [eax + eax*4]
            lea ecx, [edx + eax*2]
            mov edx, dword ptr [gProgram_state+0x5ec4]
            movsx eax, word ptr [edx + ecx*2]
            mov ecx, dword ptr [gProgram_state+0x5ec0]
            shl eax, 5
            add eax, ecx
            pop edi
            pop esi
            pop ebx
            ret
    L004aebdf:
            cmp dx, 0x3a98
            jl L004aebfe
            mov ecx, dword ptr [ecx + 0x114]
            movsx eax, dx
            sub eax, 0x38ee
            lea eax, [eax + eax*2]
            lea eax, [ecx + eax*4]
            pop edi
            pop esi
            pop ebx
            ret
    L004aebfe:
            cmp dx, 0x2710
            jne L004aec0f
            lea eax, [ecx + 0x12c]
            pop edi
            pop esi
            pop ebx
            ret
    L004aec0f:
            mov edx, dword ptr [ecx + 8]
            add edx, 0xec
            push edx
            push offset gOpponent_nastyness_frigger+0xa0
            call DoNotDprintf_opponent
            add esp, 8
            mov ecx, offset gOpponent_nastyness_frigger+0x70
            call PDEnterDebugger
            xor eax, eax
            pop edi
            pop esi
            pop ebx
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004aec40
C2_NAKED const br_vector3* C2_HOOK_FASTCALL GetOpponentsSectionFinishNodePoint(const tOpponent_spec* pOpponent_spec, tS16 pSection) {
#ifndef CARPOCALYPSE2_MATCHING
    tS16 section_no;
    tS16 node_no;
    int node_index_index;

    if (pSection >= 20000 && pOpponent_spec->nnext_sections > pSection - 20000) {
        section_no = pOpponent_spec->next_sections[pSection - 20000].section_no;
        node_index_index = pOpponent_spec->next_sections[pSection - 20000].direction;
        node_no = gProgram_state.AI_vehicles.path_sections[section_no].node_indices[node_index_index];
        return &gProgram_state.AI_vehicles.path_nodes[node_no].pos;
    } else if (pSection >= 15000) {
        return &pOpponent_spec->pursue_car_data.pursuee->my_trail.trail_nodes[(pSection + 1) - 15000];
    } else if (pSection == 10000) {
        return &pOpponent_spec->pursue_car_data.direct_line_nodes[1].pos;
    } else {
        dr_dprintf("BIG ERROR - GetOpponentsSectionFinishNodePoint() - section not found in next_section array for opponent %s",
                   pOpponent_spec->car_spec->driver_name);
        PDEnterDebugger("BIG ERROR - GetOpponentsSectionFinishNodePoint()");
        return NULL;
    }
#else
    __asm {
            push esi
            cmp dx, 0x4e20
            push edi
            jl L004aec88
            mov edi, dword ptr [ecx + 0x7c]
            movsx eax, dx
            lea esi, [eax - 0x4e20]
            cmp edi, esi
            jle L004aec88
            xor edx, edx
            mov dl, byte ptr [ecx + eax*4 - 0x137fe]
            movsx eax, word ptr [ecx + eax*4 - 0x13800]
            lea eax, [eax + eax*4]
            lea ecx, [edx + eax*2]
            mov edx, dword ptr [gProgram_state+0x5ec4]
            movsx eax, word ptr [edx + ecx*2]
            mov ecx, dword ptr [gProgram_state+0x5ec0]
            shl eax, 5
            add eax, ecx
            pop edi
            pop esi
            ret
    L004aec88:
            cmp dx, 0x3a98
            jl L004aeca6
            mov ecx, dword ptr [ecx + 0x114]
            movsx eax, dx
            sub eax, 0x38ed
            lea eax, [eax + eax*2]
            lea eax, [ecx + eax*4]
            pop edi
            pop esi
            ret
    L004aeca6:
            cmp dx, 0x2710
            jne L004aecb6
            lea eax, [ecx + 0x14c]
            pop edi
            pop esi
            ret
    L004aecb6:
            mov edx, dword ptr [ecx + 8]
            add edx, 0xec
            push edx
            push offset gOpponent_nastyness_frigger+0x1430
            call DoNotDprintf_opponent
            add esp, 8
            mov ecx, offset gOpponent_nastyness_frigger+0x13fc
            call PDEnterDebugger
            xor eax, eax
            pop edi
            pop esi
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004aea00
int C2_HOOK_FASTCALL GetOpponentsFirstSection(const tOpponent_spec* pOpponent_spec) {
    if (pOpponent_spec->current_objective == eOOT_pursue_and_twat) {
        if (pOpponent_spec->pursue_car_data.state == ePCS_following_trail) {
            return pOpponent_spec->follow_path_data.section_no;
        } else if (pOpponent_spec->pursue_car_data.state == ePCS_following_line_of_sight) {
            return 10000;
        }
    }
    return 20000;
}

// FUNCTION: CARMA2_HW 0x004aedf0
int C2_HOOK_FASTCALL GetOpponentsSectionMaxSpeed(tOpponent_spec* pOpponent_spec, tS16 pSection, int pTowards_finish) {
    if (pSection >= 20000 && pOpponent_spec->nnext_sections > pSection - 20000) {
        const tRoute_section* route = &pOpponent_spec->next_sections[pSection - 20000];
        return gProgram_state.AI_vehicles.path_sections[route->section_no].max_speed[(route->direction ^ pTowards_finish) == 0];
    } else if (pSection >= 15000) {
        return 255;
    } else if (pSection == 10000) {
        return pOpponent_spec->pursue_car_data.direct_line_section.max_speed[pTowards_finish];
    }
    DoNotDprintf_opponent("WARNING - GetOpponentsSectionMaxSpeed() - section not found in next_section array for opponent %s", pOpponent_spec->car_spec->driver_name);
    PDEnterDebugger("WARNING - GetOpponentsSectionMaxSpeed()");
    return 255;
}

// FUNCTION: CARMA2_HW 0x004aed50
int C2_HOOK_FASTCALL GetOpponentsSectionMinSpeed(tOpponent_spec* pOpponent_spec, tS16 pSection, int pTowards_finish) {
    if (pSection >= 20000 && pOpponent_spec->nnext_sections > pSection - 20000) {
        const tRoute_section* route = &pOpponent_spec->next_sections[pSection - 20000];
        return gProgram_state.AI_vehicles.path_sections[route->section_no].min_speed[(route->direction ^ pTowards_finish) == 0];
    } else if (pSection >= 15000) {
        return 0;
    } else if (pSection == 10000) {
        return pOpponent_spec->pursue_car_data.direct_line_section.min_speed[pTowards_finish];
    }
    DoNotDprintf_opponent("WARNING - GetOpponentsSectionMinSpeed() - section not found in next_section array for opponent %s", pOpponent_spec->car_spec->driver_name);
    PDEnterDebugger("WARNING - GetOpponentsSectionMinSpeed()");
    return 0;
}

// FUNCTION: CARMA2_HW 0x004a7f20
int C2_NAKED C2_HOOK_FASTCALL RematerialiseOpponentOnThisSection(tOpponent_spec* pOpponent_spec, tS16 pSection_no, float pSpeed) {
#ifndef CARPOCALYPSE2_MATCHING
    const br_vector3* start;
    const br_vector3* finish;
    br_vector3 a;
    br_vector3 p;
    br_vector3 section_v;
    br_scalar t;
    br_scalar length;

    if (pOpponent_spec->physics_me) {
        DoNotDprintf_opponent("%s: Actually, we're already materialised", pOpponent_spec->car_spec->driver_name);
        return 1;
    }

    if (pSection_no >= 20000 && pOpponent_spec->nnext_sections > pSection_no - 20000) {
        tS16 section_no = pOpponent_spec->next_sections[pSection_no - 20000].section_no;
        tU8 direction = pOpponent_spec->next_sections[pSection_no - 20000].direction;
        tS16 node_no = gProgram_state.AI_vehicles.path_sections[section_no].node_indices[direction == 0];
        start = &gProgram_state.AI_vehicles.path_nodes[node_no].pos;
    } else if (pSection_no >= 15000) {
        start = &pOpponent_spec->pursue_car_data.pursuee->my_trail.trail_nodes[pSection_no - 15000];
    } else if (pSection_no == 10000) {
        start = &pOpponent_spec->pursue_car_data.direct_line_nodes[0].pos;
    } else {
        DoNotDprintf_opponent("BIG ERROR - GetOpponentsSectionStartNodePoint() - section not found in next_section array for opponent %s", pOpponent_spec->car_spec->driver_name);
        PDEnterDebugger("BIG ERROR - GetOpponentsSectionStartNodePoint()");
        start = NULL;
    }

    finish = GetOpponentsSectionFinishNodePoint(pOpponent_spec, pSection_no);

    a.v[0] = finish->v[0] - start->v[0];
    a.v[1] = finish->v[1] - start->v[1];
    a.v[2] = finish->v[2] - start->v[2];
    length = sqrtf(a.v[0] * a.v[0] + a.v[1] * a.v[1] + a.v[2] * a.v[2]);
    if (length != gOppo_render_zero) {
        p.v[0] = pOpponent_spec->car_spec->car_master_actor->t.t.translate.t.v[0] - start->v[0];
        p.v[1] = pOpponent_spec->car_spec->car_master_actor->t.t.translate.t.v[1] - start->v[1];
        p.v[2] = pOpponent_spec->car_spec->car_master_actor->t.t.translate.t.v[2] - start->v[2];
        t = (a.v[0] * p.v[0] + a.v[1] * p.v[1] + a.v[2] * p.v[2]) / (a.v[0] * a.v[0] + a.v[1] * a.v[1] + a.v[2] * a.v[2]);
        if (t < gOppo_render_zero) {
            pOpponent_spec->car_spec->car_master_actor->t.t.translate.t.v[0] = start->v[0];
            pOpponent_spec->car_spec->car_master_actor->t.t.translate.t.v[1] = start->v[1];
            pOpponent_spec->car_spec->car_master_actor->t.t.translate.t.v[2] = start->v[2];
        } else if (t > 1.f) {
            pOpponent_spec->car_spec->car_master_actor->t.t.translate.t.v[0] = finish->v[0];
            pOpponent_spec->car_spec->car_master_actor->t.t.translate.t.v[1] = finish->v[1];
            pOpponent_spec->car_spec->car_master_actor->t.t.translate.t.v[2] = finish->v[2];
        } else {
            pOpponent_spec->car_spec->car_master_actor->t.t.translate.t.v[0] = start->v[0] + a.v[0] * t;
            pOpponent_spec->car_spec->car_master_actor->t.t.translate.t.v[1] = start->v[1] + a.v[1] * t;
            pOpponent_spec->car_spec->car_master_actor->t.t.translate.t.v[2] = start->v[2] + a.v[2] * t;
        }
section_v.v[0] = finish->v[0] - start->v[0];
            section_v.v[1] = finish->v[1] - start->v[1];
            section_v.v[2] = finish->v[2] - start->v[2];
            PointActorAlongThisBloodyVector(pOpponent_spec->car_spec->car_master_actor, &section_v);
        }
        return RematerialiseOpponent(pOpponent_spec, pSpeed);

#else
    __asm {
            sub esp, 0x1c
            push edi
            mov edi, ecx
            mov word ptr [esp + 4], dx
            test byte ptr [edi + 0xb0], 8
            je L004a7f56
            mov eax, dword ptr [edi + 8]
            add eax, 0xec
            push eax
            push offset gOpponent_nastyness_frigger+0x10c
            call DoNotDprintf_opponent
            add esp, 8
            mov eax, 1
            pop edi
            add esp, 0x1c
            ret 4
    L004a7f56:
            cmp dx, 0x4e20
            push esi
            jl L004a7fa0
            mov esi, dword ptr [edi + 0x7c]
            movsx eax, dx
            lea ecx, [eax - 0x4e20]
            cmp esi, ecx
            jle L004a7fa0
            mov cl, byte ptr [edi + eax*4 - 0x137fe]
            xor edx, edx
            movsx eax, word ptr [edi + eax*4 - 0x13800]
            test cl, cl
            sete dl
            lea eax, [eax + eax*4]
            lea ecx, [edx + eax*2]
            mov edx, dword ptr [gProgram_state+0x5ec4]
            mov eax, dword ptr [gProgram_state+0x5ec0]
            movsx esi, word ptr [edx + ecx*2]
            shl esi, 5
            add esi, eax
            jmp L004a7fef
    L004a7fa0:
            cmp dx, 0x3a98
            jl L004a7fbd
            mov ecx, dword ptr [edi + 0x114]
            movsx eax, dx
            sub eax, 0x38ee
            lea eax, [eax + eax*2]
            lea esi, [ecx + eax*4]
            jmp L004a7fef
    L004a7fbd:
            cmp dx, 0x2710
            jne L004a7fcc
            lea esi, [edi + 0x12c]
            jmp L004a7fef
    L004a7fcc:
            mov edx, dword ptr [edi + 8]
            add edx, 0xec
            push edx
            push offset gOpponent_nastyness_frigger+0xa0
            call DoNotDprintf_opponent
            add esp, 8
            mov ecx, offset gOpponent_nastyness_frigger+0x70
            call PDEnterDebugger
            xor esi, esi
    L004a7fef:
            mov edx, dword ptr [esp + 8]
            mov ecx, edi
            call GetOpponentsSectionFinishNodePoint
            mov ecx, eax
            _emit 0xd9
        _emit 0x01
            _emit 0xd8
        _emit 0x26
            fld dword ptr [ecx + 4]
            fsub dword ptr [esi + 4]
            fld dword ptr [ecx + 8]
            fsub dword ptr [esi + 8]
            fxch st(2)
            fst dword ptr [esp + 0xc]
            fxch st(1)
            fst dword ptr [esp + 0x10]
            fxch st(1)
            fmul dword ptr [esp + 0xc]
            fxch st(2)
            fstp dword ptr [esp + 0x14]
            fmul dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x14]
            fmul dword ptr [esp + 0x14]
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fld st(0)
            fsqrt
            fcomp dword ptr [gOppo_render_zero]
            fnstsw ax
            test ah, 0x40
            jne L004a8142
            mov eax, dword ptr [edi + 8]
            mov edx, dword ptr [eax + 0x10]
            fld dword ptr [edx + 0x54]
            fld dword ptr [edx + 0x58]
            fsub dword ptr [esi + 8]
            fxch st(1)
            fsub dword ptr [esi + 4]
            fld dword ptr [edx + 0x50]
            _emit 0xd8
        _emit 0x26
            fxch st(2)
            fmul dword ptr [esp + 0x14]
            fxch st(1)
            fmul dword ptr [esp + 0x10]
            fxch st(2)
            fmul dword ptr [esp + 0xc]
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fdiv st(0), st(1)
            fstp dword ptr [esp + 8]
            fstp st(0)
            fld dword ptr [esp + 8]
            fcomp dword ptr [gOppo_render_zero]
            fnstsw ax
            test ah, 1
            je L004a80a1
            _emit 0xd9
        _emit 0x06
            fld dword ptr [esi + 4]
            fld dword ptr [esi + 8]
            jmp L004a80e8
    L004a80a1:
            fld dword ptr [esp + 8]
            fcomp dword ptr [gOppo_pursuit_one]
            fnstsw ax
            test ah, 0x41
            jne L004a80bc
            _emit 0xd9
        _emit 0x01
            fld dword ptr [ecx + 4]
            fld dword ptr [ecx + 8]
            jmp L004a80e8
    L004a80bc:
            fld dword ptr [esp + 8]
            fld dword ptr [esp + 8]
            fld dword ptr [esp + 8]
            fxch st(2)
            fmul dword ptr [esp + 0xc]
            fxch st(1)
            fmul dword ptr [esp + 0x10]
            fxch st(2)
            fmul dword ptr [esp + 0x14]
            fxch st(1)
            _emit 0xd8
        _emit 0x06
            fxch st(2)
            fadd dword ptr [esi + 4]
            fxch st(1)
            fadd dword ptr [esi + 8]
    L004a80e8:
            fxch st(2)
            fstp dword ptr [edx + 0x50]
            mov eax, dword ptr [edi + 8]
            lea edx, [esp + 0x18]
            fxch st(1)
            fxch st(1)
            mov eax, dword ptr [eax + 0x10]
            fstp dword ptr [eax + 0x54]
            mov eax, dword ptr [edi + 8]
            mov eax, dword ptr [eax + 0x10]
            fstp dword ptr [eax + 0x58]
            _emit 0xd9
        _emit 0x01
            _emit 0xd8
        _emit 0x26
            mov eax, dword ptr [edi + 8]
            fstp dword ptr [esp + 0x18]
            fld dword ptr [ecx + 4]
            fsub dword ptr [esi + 4]
            fstp dword ptr [esp + 0x1c]
            fld dword ptr [ecx + 8]
            fsub dword ptr [esi + 8]
            fstp dword ptr [esp + 0x20]
            mov ecx, dword ptr [eax + 0x10]
            call PointActorAlongThisBloodyVector
            mov ecx, dword ptr [esp + 0x28]
            push ecx
            mov ecx, edi
            call RematerialiseOpponent
            pop esi
            pop edi
            add esp, 0x1c
            ret 4
    L004a8142:
            mov ecx, dword ptr [esp + 0x28]
            push ecx
            mov ecx, edi
            fstp st(0)
            call RematerialiseOpponent
            pop esi
            pop edi
            add esp, 0x1c
            ret 4
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

// FUNCTION: CARMA2_HW 0x004aece0
float C2_HOOK_FASTCALL GetOpponentsSectionWidth(const tOpponent_spec* pOpponent_spec, tS16 pSection) {
    if (pSection >= 20000) {
        int sections = pOpponent_spec->nnext_sections;
        int s = pSection - 20000;
        if (sections > s) {
            tS16 idx = pOpponent_spec->next_sections[pSection - 20000].section_no;
            return gProgram_state.AI_vehicles.path_sections[idx].width;
        }
    }
    if (pSection >= 15000) {
        return gOppo_pursuit_velocity_scale;
    }
    if (pSection == 10000) {
        return pOpponent_spec->pursue_car_data.direct_line_section.width;
    }
    return gProgram_state.AI_vehicles.path_sections[(tS32)pSection].width;
}
// DoNotDprintf

// ProcessCurrentObjective

// ReallocExtraPathNodes

// ReallocExtraPathSections

// PointVisibleFromHere

// WeightedFindNearestNodeAndSection

// FindNearestGeneralSection

// FindNearestPathSection

// DeadStopCar

// TurnOpponentPhysicsOn

// TurnOpponentPhysicsOff

// ApplyOppoRenderage2

// NewObjective

// CalcRaceRoute

// TopUpRandomRoute

// FUNCTION: CARMA2_HW 0x004ac180
int C2_HOOK_FASTCALL SearchForSection(tRoute_section* pTemp_store, tRoute_section* pPerm_store, int* pNum_of_perm_store_sections, tS16 pTarget_section, int pDepth, br_scalar pDistance_so_far, tOpponent_spec* pOpponent_spec) {
    char depth_indent[32];
    const tPath_node* path_node;
    tS16 node;
    tS16 next_node;
    tS16 section;
    tS16 i;
    int direction;
    br_scalar length;

    C2_HOOK_BUG_ON(sizeof(tRoute_section) != 4);

    DoNotDprintf_opponent("%sSearchForSection() - START: depth %d", depth_indent, pDepth);
    gSFS_cycles_this_time++;
    if (pDepth == 1) {
        DoNotDprintf_opponent("%sSearchForSection() - First time called", depth_indent);
        memset(gBit_per_node, 0, (gProgram_state.AI_vehicles.number_of_path_nodes + 7) / 8);
        gSFS_shortest_dist = FLT_MAX;
        gSFS_routes_found = 0;
        *pNum_of_perm_store_sections = 0;
    }
    if (pDepth >= 10) {
        DoNotDprintf_opponent("%sSearchForSection() - *** Reached max depth ***", depth_indent);
        return 0;
    }
    node = (tS16)gProgram_state.AI_vehicles.path_sections[pTemp_store[pDepth - 1].section_no].node_indices[pTemp_store[pDepth - 1].direction];
    path_node = &gProgram_state.AI_vehicles.path_nodes[node];
    gBit_per_node[node / 8] |= 1 << (node % 8);
    DoNotDprintf_opponent("%sSearchForSection() - node #%d has %d sections", depth_indent, node, path_node->number_of_sections);
    for (i = 0; i < path_node->number_of_sections; i++) {
        section = (tS16)path_node->sections[i];
        direction = node != (tS16)gProgram_state.AI_vehicles.path_sections[section].node_indices[1];
        DoNotDprintf_opponent("%sSearchForSection() - node's section #%d is #%d", depth_indent, i, section);
        next_node = (tS16)gProgram_state.AI_vehicles.path_sections[section].node_indices[direction];
        if (gBit_per_node[next_node / 8] & (1 << (next_node % 8))) {
            DoNotDprintf_opponent("%sSearchForSection() - *** node #%d already visited ***", depth_indent, next_node);
            continue;
        }
        if (gProgram_state.AI_vehicles.path_sections[section].one_way && node == (tS16)gProgram_state.AI_vehicles.path_sections[section].node_indices[1]) {
            DoNotDprintf_opponent("%sSearchForSection() - *** no entry ***", depth_indent);
            continue;
        }
        if (!pOpponent_spec->cheating && gProgram_state.AI_vehicles.path_sections[section].type == ePST_cheat_only) {
            continue;
        }
        DoNotDprintf_opponent("%sSearchForSection() - adding section #%d to list", depth_indent, section);
        pTemp_store[pDepth].section_no = section;
        pTemp_store[pDepth].direction = direction;
        length = gProgram_state.AI_vehicles.path_sections[section].length + pDistance_so_far;
        if (section == pTarget_section && length < gSFS_shortest_dist) {
            gSFS_shortest_dist = length;
            *pNum_of_perm_store_sections = pDepth + 1;
            memcpy(pPerm_store, pTemp_store, (pDepth + 1) * sizeof(tRoute_section));
            gSFS_routes_found_copy = gSFS_routes_found + 1;
            gSFS_routes_found++;
            if (gSFS_routes_found >= 2) {
                DoNotDprintf_opponent("%sSearchForSection() - Found final target", depth_indent);
                return 1;
            }
            DoNotDprintf_opponent("%sSearchForSection() - Found a target", depth_indent);
            DoNotDprintf_opponent("%sSearchForSection() - Returning to depth %d", depth_indent, pDepth - 1);
            gBit_per_node[node / 8] &= ~(1 << (node % 8));
            return 0;
        }
        if (pDepth < 9 && SearchForSection(pTemp_store, pPerm_store, pNum_of_perm_store_sections, pTarget_section, pDepth + 1, length, pOpponent_spec)) {
            return 1;
        }
    }
    gBit_per_node[node / 8] &= ~(1 << (node % 8));
    DoNotDprintf_opponent("%sSearchForSection() - Returning to depth %d", depth_indent, pDepth - 1);
    return 0;
}

// UpdatePlayersSection

// AllowForDecel

// ShiftOpponentsProjectedRoute

// FUNCTION: CARMA2_HW 0x004ac9e0
void C2_NAKED C2_HOOK_FASTCALL NewCalcGetNearPlayerRoute(tOpponent_spec* pOpponent_spec, tCar_spec* pPursuee, int pForce) {
#ifndef CARPOCALYPSE2_MATCHING
    br_vector3 direction;
    br_vector3 intersect;
    br_vector3 player_direction;
    br_vector3 player_intersect;
    br_scalar distance;
    tRoute_section route[10];
    tRoute_section best[10];
    int best_count;
    int i;
    int index;
    int count;
    int failed = 0;
    tS16 section;
    tS16 node;
    tU32 now;
    double x;
    double y;
    double z;
    double stopping;
    double length;
    const br_vector3* point;
    const br_matrix34* transform;
    int add;

    if (pOpponent_spec->nnext_sections >= 12) {
        return;
    }
    now = gTime_stamp_for_this_munging;
    if (pPursuee->field_0x155c < 0 || now > pPursuee->field_0x1540) {
        add = IRandomBetween(-100, 100);
        pPursuee->field_0x1540 = gTime_stamp_for_this_munging + (tU32)add + 1000u;
        pPursuee->field_0x155c = FindNearestGeneralSection(NULL, &pPursuee->car_master_actor->t.t.translate.t, &player_direction, &player_intersect, &distance);
    }
    if (pPursuee->field_0x155c < 0) {
        PDEnterDebugger("No path section near player. THIS CAN'T HAPPEN!");
        return;
    }
    if (pOpponent_spec->nnext_sections == 0) {
        pForce = 1;
    }
    if (pForce || pPursuee->field_0x155c != pOpponent_spec->players_section_when_last_calced_full_path) {
        section = FindNearestGeneralSection(NULL, &pOpponent_spec->car_spec->car_master_actor->t.t.translate.t, &direction, &intersect, &distance);
        if (section < 0) {
            PDEnterDebugger("No path section near us. THIS CAN'T HAPPEN!");
            return;
        }
        index = -1;
        for (i = 0; i < pOpponent_spec->nnext_sections; i++) {
            /* The original writes before testing section against zero. */
            pOpponent_spec->next_sections[i].section_no = section;
            if (section != 0) {
                index = i;
                break;
            }
        }
        if (index >= 0) {
            const br_vector3* velocity = &pOpponent_spec->car_spec->collision_info->v;

            length = OpponentAdd(OpponentAdd(OpponentMultiply(velocity->v[0], velocity->v[0]), OpponentMultiply(velocity->v[1], velocity->v[1])), OpponentMultiply(velocity->v[2], velocity->v[2]));
            stopping = OpponentMultiply(OpponentMultiply(OpponentSubtract(100.f, length), -0.0625f), 0.14492753623188406);
            if (stopping > 0.f) {
                /* This first lookup uses the route ordinal, not section_no. */
                node = (tS16)gProgram_state.AI_vehicles.path_sections[index].node_indices[pOpponent_spec->next_sections[index].direction];
                point = &gProgram_state.AI_vehicles.path_nodes[node].pos;
                x = OpponentFloatDifference(&point->v[0], &intersect.v[0]);
                y = OpponentFloatDifference(&point->v[1], &intersect.v[1]);
                z = OpponentFloatDifference(&point->v[2], &intersect.v[2]);
                length = OpponentSqrt(OpponentHybridLengthSquared(x, y, z));
                while (index < pOpponent_spec->nnext_sections - 1 && (length < stopping || length != length || stopping != stopping)) {
                    stopping = OpponentSubtract(stopping, length);
                    index++;
                    length = (double)gProgram_state.AI_vehicles.path_sections[pOpponent_spec->next_sections[index].section_no].length;
                }
            }
            ShiftOpponentsProjectedRoute(pOpponent_spec, index);
        } else {
            pOpponent_spec->nnext_sections = 0;
            transform = &pOpponent_spec->car_spec->car_master_actor->t.t.mat;
            x = OpponentFloatDifference(&intersect.v[0], &transform->m[3][0]);
            y = OpponentFloatDifference(&intersect.v[1], &transform->m[3][1]);
            z = OpponentFloatDifference(&intersect.v[2], &transform->m[3][2]);
            length = OpponentAdd(OpponentAdd(OpponentMultiply(z, transform->m[2][2]), OpponentMultiply(x, transform->m[2][0])), OpponentMultiply(y, transform->m[2][1]));
            AddToOpponentsProjectedRoute(pOpponent_spec, section, length >= 0.f);
        }
    }
    while (pOpponent_spec->nnext_sections < 12 && !failed) {
        route[0].section_no = pOpponent_spec->next_sections[pOpponent_spec->nnext_sections - 1].section_no;
        route[0].direction = pOpponent_spec->next_sections[pOpponent_spec->nnext_sections - 1].direction;
        gSFS_cycles_this_time = 0;
        gSFS_count = (int)((unsigned int)gSFS_count + 1u);
        SearchForSection(route, best, &best_count, pPursuee->field_0x155c, 1, 0.f, pOpponent_spec);
        gSFS_total_cycles = (int)((unsigned int)gSFS_total_cycles + (unsigned int)gSFS_cycles_this_time);
        if (gSFS_cycles_this_time > gSFS_max_cycles) {
            gSFS_max_cycles = gSFS_cycles_this_time;
        }
        if (best_count > 1) {
            count = best_count - 1;
            if (count >= 12 - pOpponent_spec->nnext_sections) {
                count = 12 - pOpponent_spec->nnext_sections;
            }
            memcpy(&pOpponent_spec->next_sections[pOpponent_spec->nnext_sections], &best[1], (size_t)count * sizeof(tRoute_section));
            pOpponent_spec->nnext_sections += count;
            if (pOpponent_spec->next_sections[pOpponent_spec->nnext_sections].section_no == pPursuee->field_0x155c) {
                add = gOpponents[pOpponent_spec->index].strength_rating >= 5 ? 1 : IRandomBetween(1, 2);
                TopUpRandomRoute(pOpponent_spec, add);
            }
        } else {
            failed = 1;
            if (pOpponent_spec->nnext_sections < 12) {
                TopUpRandomRoute(pOpponent_spec, gOpponents[pOpponent_spec->index].strength_rating >= 5 ? 2 : 4);
            }
        }
    }
    pOpponent_spec->players_section_when_last_calced_full_path = pPursuee->field_0x155c;
#else
    __asm {
            sub esp, 0x94
            push ebx
            mov ebx, ecx
            push ebp
            push esi
            mov eax, dword ptr [ebx + 0x7c]
            mov ebp, edx
            xor esi, esi
            cmp eax, 0xc
            push edi
            mov dword ptr [esp + 0x14], ebp
            mov dword ptr [esp + 0x18], esi
            jge L004ace13
            cmp word ptr [ebp + 0x155c], si
            jl L004aca1c
            mov eax, dword ptr [gTime_stamp_for_this_munging]
            mov ecx, dword ptr [ebp + 0x1540]
            cmp eax, ecx
            jbe L004aca61
    L004aca1c:
            mov edx, 0x64
            mov ecx, 0xffffff9c
            call IRandomBetween
            mov ecx, dword ptr [gTime_stamp_for_this_munging]
            lea edx, [eax + ecx + 0x3e8]
            lea eax, [esp + 0x10]
            mov dword ptr [ebp + 0x1540], edx
            lea ecx, [esp + 0x28]
            push eax
            lea edx, [esp + 0x40]
            push ecx
            push edx
            mov edx, dword ptr [ebp + 0x10]
            xor ecx, ecx
            add edx, 0x50
            call FindNearestGeneralSection
            mov word ptr [ebp + 0x155c], ax
    L004aca61:
            mov cx, word ptr [ebp + 0x155c]
            cmp cx, si
            jge L004aca84
            mov ecx, offset gOpponent_nastyness_frigger+0xe50
            call PDEnterDebugger
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x94
            ret 4
    L004aca84:
            mov eax, dword ptr [ebx + 0x7c]
            cmp eax, esi
            mov eax, 1
            je L004aca97
            mov eax, dword ptr [esp + 0xa8]
    L004aca97:
            cmp eax, esi
            jne L004acaa5
            cmp cx, word ptr [ebx + 0x78]
            je L004accb8
    L004acaa5:
            lea eax, [esp + 0x38]
            lea ecx, [esp + 0x1c]
            push eax
            mov eax, dword ptr [ebx + 8]
            lea edx, [esp + 0x4c]
            push ecx
            push edx
            mov edx, dword ptr [eax + 0x10]
            add edx, 0x50
            xor ecx, ecx
            call FindNearestGeneralSection
            mov edx, eax
            cmp dx, si
            jge L004acae2
            mov ecx, offset gOpponent_nastyness_frigger+0xe24
            call PDEnterDebugger
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x94
            ret 4
    L004acae2:
            mov ecx, dword ptr [ebx + 0x7c]
            or edi, 0xffffffff
            xor eax, eax
            cmp ecx, esi
            jle L004acb09
            lea ecx, [ebx + 0x80]
    L004acaf4:
            cmp dx, si
            mov word ptr [ecx], dx
            jne L004acb07
            inc eax
            add ecx, 4
            cmp eax, dword ptr [ebx + 0x7c]
            jl L004acaf4
            jmp L004acb09
    L004acb07:
            mov edi, eax
    L004acb09:
            cmp edi, esi
            jl L004acc53
            mov ecx, dword ptr [ebx + 8]
            mov eax, dword ptr [ecx + 8]
            fld dword ptr [eax + 0x68]
            fld dword ptr [eax + 0x6c]
            fld dword ptr [eax + 0x70]
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
            fsubr dword ptr [DAT_0058a47c]
            fmul dword ptr [DAT_0058a55c]
            fmul qword ptr [DAT_0058a4e0]
            fstp st(1)
            fcom dword ptr [gOppo_render_zero]
            fnstsw ax
            test ah, 0x41
            jne L004acc0e
            mov eax, dword ptr [ebx + 0x7c]
            xor edx, edx
            mov dl, byte ptr [ebx + edi*4 + 0x82]
            lea ecx, [edi + edi*4]
            mov esi, edx
            lea edx, [eax - 1]
            lea eax, [esi + ecx*2]
            mov esi, dword ptr [gProgram_state+0x5ec4]
            movsx ecx, word ptr [esi + eax*2]
            mov eax, dword ptr [gProgram_state+0x5ec0]
            shl ecx, 5
            cmp edi, edx
            fld dword ptr [ecx + eax + 4]
            fld dword ptr [ecx + eax]
            fsub dword ptr [esp + 0x1c]
            fxch st(1)
            fsub dword ptr [esp + 0x20]
            lea eax, [ecx + eax]
            fld dword ptr [eax + 8]
            fsub dword ptr [esp + 0x24]
            fxch st(2)
            fst dword ptr [esp + 0x28]
            fxch st(1)
            fst dword ptr [esp + 0x2c]
            fxch st(1)
            fmul dword ptr [esp + 0x28]
            fxch st(1)
            fmul dword ptr [esp + 0x2c]
            fld st(2)
            fmul st(0), st(3)
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fsqrt
            fstp st(1)
            jge L004acc04
            lea ecx, [ebx + edi*4 + 0x80]
    L004acbe0:
            fcom st(1)
            fnstsw ax
            test ah, 1
            je L004acc04
            movsx eax, word ptr [ecx + 4]
            fxch st(1)
            fsub st(0), st(1)
            fxch st(1)
            fstp st(0)
            add ecx, 4
            inc edi
            lea eax, [eax + eax*4]
            cmp edi, edx
            fld dword ptr [esi + eax*4 + 0xc]
            jl L004acbe0
    L004acc04:
            mov ebp, dword ptr [esp + 0x14]
            fstp st(0)
            fstp st(0)
            jmp L004acc10
    L004acc0e:
            fstp st(0)
    L004acc10:
            cmp dword ptr [ebx + 0x7c], edi
            jle L004accb8
            mov ecx, 0xc
            sub ecx, edi
            test ecx, ecx
            jle L004acc4c
            lea esi, [ebx + 0x82]
            lea eax, [ebx + edi*4 + 0x80]
            mov ebp, ecx
    L004acc33:
            mov cx, word ptr [eax]
            add eax, 4
            mov word ptr [esi - 2], cx
            mov dl, byte ptr [eax - 2]
            mov byte ptr [esi], dl
            add esi, 4
            dec ebp
            jne L004acc33
            mov ebp, dword ptr [esp + 0x14]
    L004acc4c:
            mov eax, dword ptr [ebx + 0x7c]
            sub eax, edi
            jmp L004accb5
    L004acc53:
            mov eax, dword ptr [ebx + 8]
            mov dword ptr [ebx + 0x7c], esi
            fld dword ptr [esp + 0x24]
            mov eax, dword ptr [eax + 0x10]
            fld dword ptr [esp + 0x1c]
            fxch st(1)
            fsub dword ptr [eax + 0x58]
            fxch st(1)
            fsub dword ptr [eax + 0x50]
            fld dword ptr [eax + 0x4c]
            fld dword ptr [eax + 0x44]
            fld dword ptr [esp + 0x20]
            fsub dword ptr [eax + 0x54]
            fxch st(2)
            fmulp st(4), st(0)
            fmulp st(2), st(0)
            fxch st(1)
            faddp st(2), st(0)
            fmul dword ptr [eax + 0x48]
            faddp st(1), st(0)
            fcomp dword ptr [gOppo_render_zero]
            fnstsw ax
            test ah, 1
            jne L004acc9e
            mov eax, 1
            jmp L004acca0
    L004acc9e:
            xor eax, eax
    L004acca0:
            mov word ptr [ebx + 0x80], dx
            mov ecx, dword ptr [ebx + 0x7c]
            mov byte ptr [ebx + ecx*4 + 0x82], al
            mov eax, dword ptr [ebx + 0x7c]
            inc eax
    L004accb5:
            mov dword ptr [ebx + 0x7c], eax
    L004accb8:
            mov eax, dword ptr [ebx + 0x7c]
            cmp eax, 0xc
            jge L004ace08
    L004accc4:
            mov ecx, dword ptr [esp + 0x18]
            test ecx, ecx
            jne L004ace08
            mov dx, word ptr [ebx + eax*4 + 0x7c]
            mov esi, dword ptr [gSFS_count]
            mov word ptr [esp + 0x54], dx
            mov al, byte ptr [ebx + eax*4 + 0x7e]
            inc esi
            mov dword ptr [gSFS_cycles_this_time], 0
            mov dword ptr [gSFS_count], esi
            mov cx, word ptr [ebp + 0x155c]
            push ebx
            push 0
            push 1
            lea edx, [esp + 0x40]
            push ecx
            push edx
            lea edx, [esp + 0x90]
            lea ecx, [esp + 0x68]
            mov byte ptr [esp + 0x6a], al
            call SearchForSection
            mov eax, dword ptr [gSFS_cycles_this_time]
            mov edx, dword ptr [gSFS_total_cycles]
            mov ecx, dword ptr [gSFS_max_cycles]
            add edx, eax
            cmp eax, ecx
            mov dword ptr [gSFS_total_cycles], edx
            jle L004acd3d
            mov dword ptr [gSFS_max_cycles], eax
    L004acd3d:
            mov ecx, dword ptr [esp + 0x34]
            cmp ecx, 1
            jle L004acdbd
            mov esi, dword ptr [ebx + 0x7c]
            mov eax, 0xc
            sub eax, esi
            dec ecx
            cmp ecx, eax
            jge L004acd57
            mov eax, ecx
    L004acd57:
            mov edx, dword ptr [ebx + 0x7c]
            mov ecx, eax
            lea esi, [esp + 0x80]
            lea edi, [ebx + edx*4 + 0x80]
            rep movsd
            mov edx, dword ptr [ebx + 0x7c]
            add edx, eax
            mov eax, edx
            mov dword ptr [ebx + 0x7c], edx
            mov ax, word ptr [ebx + eax*4 + 0x80]
            cmp ax, word ptr [ebp + 0x155c]
            jne L004acdfc
            mov eax, dword ptr [ebx]
            mov edx, dword ptr [gOpponents]
            mov ecx, eax
            shl ecx, 3
            sub ecx, eax
            shl ecx, 6
            cmp dword ptr [ecx + edx + 0x19c], 5
            jge L004acdb6
            mov edx, 2
            mov ecx, 1
            call IRandomBetween
            mov edx, eax
            jmp L004acdf5
    L004acdb6:
            mov edx, 1
            jmp L004acdf5
    L004acdbd:
            mov eax, dword ptr [ebx + 0x7c]
            mov dword ptr [esp + 0x18], 1
            cmp eax, 0xc
            jge L004acdfc
            mov eax, dword ptr [ebx]
            mov edx, dword ptr [gOpponents]
            mov ecx, eax
            shl ecx, 3
            sub ecx, eax
            shl ecx, 6
            mov eax, dword ptr [ecx + edx + 0x19c]
            mov edx, 4
            cmp eax, 5
            jl L004acdf5
            mov edx, 2
    L004acdf5:
            mov ecx, ebx
            call TopUpRandomRoute
    L004acdfc:
            mov eax, dword ptr [ebx + 0x7c]
            cmp eax, 0xc
            jl L004accc4
    L004ace08:
            mov ax, word ptr [ebp + 0x155c]
            mov word ptr [ebx + 0x78], ax
    L004ace13:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x94
            ret 4
    }
#endif
}

// CalcReturnToStartPointRoute

// ClearOpponentsProjectedRoute

// AddToOpponentsProjectedRoute

// StunTheBugger

// UnStunTheBugger

// ProcessCompleteRace

// StartRecordingTrail

// RecordNextTrailNode

// FindNearestTrailSection

// CalcNextTrailSection

// ProcessPursueAndTwat

// ProcessRunAway

// ProcessWaitForSomeHaplessSod

// ProcessReturnToStart

// ProcessLevitate

// ProcessGetNearPlayer

// ProcessFrozen

// HeadOnWithPlayerPossible

// AlreadyPursuingCar

// LastTwatteeAPlayer

// LastTwatterAPlayer

// ObjectiveComplete

// TeleportOpponentToNearestSafeLocation

// ChooseNewObjective

// ProcessThisOpponent

// IsNetCarActive

// NoteCarsCurrentlyUsed

// AddIfNotInList

// RemoveAnythingStillInList

// RebuildActiveCarList

// FUNCTION: CARMA2_HW 0x004a7a60
void C2_HOOK_FASTCALL ForceRebuildActiveCarList(void) {
    gActive_car_list_rebuild_required = 1;
    if (gProgram_state.racing) {
        RebuildActiveCarList();
    }
}

// OpponentRepairNecessary

// PossiblyRepairOpponent

// DisplayOpponentRecoveringHeadup

// StartToCheat

// OiStopCheating

// TeleportCopToStart

// FUNCTION: CARMA2_HW 0x004ada90
void C2_HOOK_FASTCALL CalcDistanceFromHome(tOpponent_spec* pOpponent_spec) {
    br_vector3 wank;

    BrVector3Sub(&wank, &pOpponent_spec->car_spec->car_master_actor->t.t.translate.t, &pOpponent_spec->start_pos);
    pOpponent_spec->distance_from_home = BrVector3Length(&wank);
}

// MassageOpponentPosition

// RematerialiseOpponentOnThisSection

// RematerialiseOpponentOnNearestSection

// RematerialiseOpponent

// CalcPlayerConspicuousness

// CalcOpponentConspicuousnessWithAViewToCheatingLikeFuck

// NumberOfOpponentsStillRunning

// ResetPredominantJobbies

// LoadCopCars

// LoadInOppoPaths

// DisposeOpponentPaths

// MungeOpponents

// InitOpponents

// DisposeOpponents

// WakeUpOpponentsToTheFactThatTheStartHasBeenJumped

// FUNCTION: CARMA2_HW 0x004ae790
int C2_HOOK_FASTCALL GetCarCount(tVehicle_type pCategory) {
    switch (pCategory) {
    case eVehicle_self:
        return 1;
    case eVehicle_opponent:
        return gProgram_state.AI_vehicles.number_of_opponents;
    case eVehicle_net_player:
        if (gNet_mode != eNet_mode_none) {
            return gNumber_of_net_players - 1;
        } else {
            return 0;
        }
    case eVehicle_rozzer:
        return gNumber_of_cops_before_faffage;
    case eVehicle_drone:
        return 0;
    case eVehicle_not_really:
        return gNum_active_non_cars;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004ae7e0
tCar_spec* C2_HOOK_FASTCALL GetCarSpec(tVehicle_type pCategory, int pIndex) {
#ifndef CARPOCALYPSE2_MATCHING
    switch (pCategory) {
    case eVehicle_self:
        return &gProgram_state.current_car;
    case eVehicle_net_player:
        if (pIndex < gThis_net_player_index) {
            return gRace_board[pIndex].car;
        } else {
            return (&gRace_board[1])[pIndex].car;
        }
    case eVehicle_opponent:
        return gProgram_state.AI_vehicles.opponents[pIndex].car_spec;
    case eVehicle_rozzer:
        return gProgram_state.AI_vehicles.cops[pIndex].car_spec;
    case eVehicle_drone:
        PDEnterDebugger("OPPONENT.C: GetCarSpec() can't return drone car_specs");
        return NULL;
    case eVehicle_not_really:
        return (tCar_spec*)gActive_non_car_list[pIndex];
    default:
        return NULL;
    }
#else
    switch (pCategory) {
    case eVehicle_self:
        return &gProgram_state.current_car;
    case eVehicle_net_player:
        if (pIndex < gThis_net_player_index) {
            return gRace_board[pIndex].car;
        } else {
            return (&gRace_board[1])[pIndex].car;
        }
    case eVehicle_opponent:
        return gProgram_state.AI_vehicles.opponents[pIndex].car_spec;
    case eVehicle_rozzer:
        return gProgram_state.AI_vehicles.cops[pIndex].car_spec;
    case eVehicle_drone:
        PDEnterDebugger("OPPONENT.C: GetCarSpec() can't return drone car_specs");
        return NULL;
    case eVehicle_not_really:
        return (tCar_spec*)gActive_non_car_list[pIndex];
    default:
        return NULL;
    }
#endif
}

// GetDriverName

// GetOpponentSpecFromCarSpec

// GetCarSpecFromGlobalOppoIndex

// GetOpponentsRealSection

// GetOpponentsFirstSection

// GetOpponentsNextSection

// GetOpponentsSectionStartNodePoint

// GetOpponentsSectionFinishNodePoint

// GetOpponentsSectionWidth

// GetOpponentsSectionMinSpeed

// GetOpponentsSectionMaxSpeed

// FUNCTION: CARMA2_HW 0x004aee90
void C2_HOOK_FASTCALL InitOpponentPsyche(int pOpponent_index) {
    gOpponents[pOpponent_index].psyche.grudge_against_player = 0;
}

// ClearTwattageOccurrenceVariables

// FUNCTION: CARMA2_HW 0x004aeeb0
void C2_HOOK_FASTCALL TwoCarsHitEachOther(tCar_spec* pA_car, tCar_spec* pAnother_car) {
    if (pA_car != NULL && pA_car->driver == eDriver_local_human) {
        pAnother_car->field_0x1548 = gTime_stamp_for_this_munging;
    }
    if (pAnother_car != NULL && pAnother_car->driver == eDriver_local_human) {
        pA_car->field_0x1548 = gTime_stamp_for_this_munging;
    }
}

// FUNCTION: CARMA2_HW 0x004ae750
void C2_HOOK_FASTCALL ReportMurderToPoliceDepartment(tCar_spec* pCar_spec) {
    int i;

    if (pCar_spec == &gProgram_state.current_car) {
        for (i = 0; i < gProgram_state.AI_vehicles.number_of_cops; i++) {
            gProgram_state.AI_vehicles.cops[i].murder_reported = 1;
        }
    }
}

// FUNCTION: CARMA2_HW 0x004a95f0
void C2_HOOK_FASTCALL SetChallengerIndexIf(int pIndex, int pSet) {
    if (pSet) {
        gChallenger_index__opponent = pIndex;
    }
}

// RecordOpponentTwattageOccurrence

// GetOpponentMood

// FUNCTION: CARMA2_HW 0x004af490
void C2_HOOK_FASTCALL FUN_004af490(void) {
    // empty
}
