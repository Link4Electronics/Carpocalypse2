#include "spark.h"

#include "car.h"
#include "depth.h"

#include "displays.h"

#include "errors.h"
#include "globvars.h"
#include "globvrkm.h"
#include "globvrpb.h"
#include "graphics.h"
#include "loading.h"
#include "opponent.h"
#include "pedestrn.h"
#include "physics.h"
#include "piping.h"

#include "raycast.h"

#include "replay.h"
#include "trig.h"
#include "utility.h"

#include "carpocalypse2_macros.h"

#include "c2_string.h"

#include <stdlib.h>

#include "spark.h"

#include "funks.h"

extern tCar_spec* gRecalc_mass_car;

void C2_HOOK_FASTCALL SingleSplash(tPhysics_object* pObject, const br_matrix34* pMatrix, const br_vector3* pVelocity, const br_vector3* pNormal_velocity, const br_vector3* pOmega, const br_vector3* pPoint, const br_vector3* pNormal, tU32 pTime);

void C2_HOOK_FASTCALL AddExtendedSplashToPipingSession(tPhysics_object* pCollision, void* pArg2);

#define ePipe_chunk_extended_splash 59

#define CHARS1_TO_INT(A)            ((A) - '0')
#define CHARS2_TO_INT(A, B)         (10 * CHARS1_TO_INT(A) + CHARS1_TO_INT(B))
#define CHARS3_TO_INT(A, B, C)      (10 * CHARS2_TO_INT(A, B) + CHARS1_TO_INT(C))
#define CHARS4_TO_INT(A, B, C, D)   (10 * CHARS3_TO_INT(A, B, C) + CHARS1_TO_INT(D))

typedef struct tFunk_record {           // stride 0x7c
    tU8 field_0x0[0x40];
    int funk_index;                     // 0x40
    tU8 field_0x44[0x7c - 0x44];
} tFunk_record;

typedef struct tFunk_list {
    tU8 field_0x0[0x38];
    tU16 funk_count;                    // 0x38
    tU16 field_0x3a;
    tFunk_record* funk_records;         // 0x3c
} tFunk_list;

typedef struct tFunk_container {        /* br_actor->user of a funk-car burp with bit specs */
    tU8 field_0x0[0x8];
    tFunk_list* funk_list;              // 0x8
} tFunk_container;

typedef struct tCarFunks_master {       // assumed layout from FUNK data in car specs
    tU8 field_0x0[0x270];
    int count_0x270;
    tCar_bit_spec bit_0x274[3];
    int count_0x2b0;
    tCar_bit_spec bit_0x2b4[3];
} tCarFunks_master;

void C2_HOOK_FASTCALL RecordReplaySplash(void* pObject, int pType, tU32 pWater_density, const tU32* pNormal, const tU32* pBounds);

// GLOBAL: CARMA2_HW 0x006b7840
int gShade_list[16];

// GLOBAL: CARMA2_HW 0x00660140
int* gShade_table = &gShade_list[8]; /* FIXME: rename to gDust_table*/

// GLOBAL: CARMA2_HW 0x006a82b4
int gNum_dust_tables;

// GLOBAL: CARMA2_HW 0x006aa588
br_model* gShrapnel_model[2];

// GLOBAL: CARMA2_HW 0x006a9180
tShrapnel gShrapnel[30];

// GLOBAL: CARMA2_HW 0x006aa59c
int gColumn_flags;

// GLOBAL: CARMA2_HW 0x006aa5a0
int gNext_column;

// GLOBAL: CARMA2_HW 0x006aa5a0
int gColumn_next;

// GLOBAL: CARMA2_HW 0x006a82ac
int gSplash_next;

// GLOBAL: CARMA2_HW 0x006aa578
int gSpark_next;

// GLOBAL: CARMA2_HW 0x006a82b8
tSplash gSplash[32] = { 0 };

// GLOBAL: CARMA2_HW 0x006a8638
br_pixelmap* gFlame_map[20] = { 0 };

// GLOBAL: CARMA2_HW 0x006a96a8
tSmoke_column gSmoke_column[10] = { 0 };

// GLOBAL: CARMA2_HW 0x006a9b80
tSpark gSparks[32] = { 0 };

// GLOBAL: CARMA2_HW 0x006aa380
br_model* gLollipop_model;



// GLOBAL: CARMA2_HW 0x006aa570
int gSplash_flags;

// GLOBAL: CARMA2_HW 0x006a8758
br_model* gSplash_model;

// GLOBAL: CARMA2_HW 0x006aa5a4
int gNum_splash_types;

// GLOBAL: CARMA2_HW 0x006a9130
br_material* gSplash_material[20];


// GLOBAL: CARMA2_HW 0x006a82ac
int gNext_splash;

// GLOBAL: CARMA2_HW 0x006aa57c
int gSpark_flags;

// GLOBAL: CARMA2_HW 0x006aa5ac
br_pixelmap* gIt_shade_table;

// GLOBAL: CARMA2_HW 0x006aa568
int gDust_rotate;

// GLOBAL: CARMA2_HW 0x006aa388
tReplay_splash gReplay_splashes[10];

// GLOBAL: CARMA2_HW 0x006aa574
int gReplay_splash_flags;

// GLOBAL: CARMA2_HW 0x006b7884
tU32 gNext_AFE_color_cycle;

// GLOBAL: CARMA2_HW 0x006aa5b0
int gAFE_color_cycle;

// GLOBAL: CARMA2_HW 0x006aa584
int gShrapnel_flags;

// GLOBAL: CARMA2_HW 0x006aa580
int gShrapnel_index;

// GLOBAL: CARMA2_HW 0x006aa578
int gNext_spark;

// GLOBAL: CARMA2_HW 0x0058aff0
const br_scalar gFLOAT_0058aff0 = 10.0f;

// GLOBAL: CARMA2_HW 0x0058b010
const double gDOUBLE_0058b010 = 1.0;

// GLOBAL: CARMA2_HW 0x0058b018
const br_scalar gFLOAT_0058b018 = -0.5f;

// GLOBAL: CARMA2_HW 0x0058b020
const double gDOUBLE_0058b020 = 0.7;

// GLOBAL: CARMA2_HW 0x0058b028
const br_scalar gFLOAT_0058b028 = 0.01f;

// GLOBAL: CARMA2_HW 0x0058b02c
const br_scalar gFLOAT_0058b02c = -0.1f;

// GLOBAL: CARMA2_HW 0x0058b008
const br_scalar gFLOAT_0058b008 = 0.0f;

// GLOBAL: CARMA2_HW 0x0058b030
const br_scalar gFLOAT_0058b030 = 0.7f;

// GLOBAL: CARMA2_HW 0x0058b034
const br_scalar gFLOAT_0058b034 = 0.3f;

// GLOBAL: CARMA2_HW 0x0058b038
const br_scalar gFLOAT_0058b038 = 0.1f;

// GLOBAL: CARMA2_HW 0x0058b03c
const br_scalar gFLOAT_0058b03c = -20.0f;

// GLOBAL: CARMA2_HW 0x0058b040
const br_scalar gFLOAT_0058b040 = 3.0f;

// GLOBAL: CARMA2_HW 0x0058b044
const br_scalar gFLOAT_0058b044 = 0.005f;

// GLOBAL: CARMA2_HW 0x0058b048
const double gDOUBLE_0058b048 = 0.3;

// GLOBAL: CARMA2_HW 0x0058b0d8
const float gFLOAT_0058b0d8 = 0.003f;

// GLOBAL: CARMA2_HW 0x0058b0e0
const double gDOUBLE_0058b0e0 = 11.594202898550725;

// GLOBAL: CARMA2_HW 0x0058afdc
const br_scalar gFLOAT_0058afdc = 1.0f;

// GLOBAL: CARMA2_HW 0x0058afec
const br_scalar gFLOAT_0058afec = 0.5f;

// GLOBAL: CARMA2_HW 0x0058b054
const br_scalar gFLOAT_0058b054 = 6.9f;

// GLOBAL: CARMA2_HW 0x0058b060
const br_scalar gFLOAT_0058b060 = 0.14492753148078918f;

// GLOBAL: CARMA2_HW 0x0058b0c8
const double gDOUBLE_0058b0c8 = 6.9;

// GLOBAL: CARMA2_HW 0x0058b0f0
const double gDOUBLE_0058b0f0 = -0.2;

// GLOBAL: CARMA2_HW 0x0058b0f8
const double gDOUBLE_0058b0f8 = 0.07;

// GLOBAL: CARMA2_HW 0x0058b108
const double gDOUBLE_0058b108 = 47.61000000000001;

// GLOBAL: CARMA2_HW 0x0058b110
const br_scalar gFLOAT_0058b110 = 0.0000002384185791015625f;

// GLOBAL: CARMA2_HW 0x0058b114
const br_scalar gFLOAT_0058b114 = -0.028985507786273956f;

// GLOBAL: CARMA2_HW 0x0058b118
const double gDOUBLE_0058b118 = 0.01;

// GLOBAL: CARMA2_HW 0x006a82b0
br_camera* gSpark_cam;

// GLOBAL: CARMA2_HW 0x006a8718
br_matrix4 gCameraToScreen;


// GLOBAL: CARMA2_HW 0x006a8760
tBRender_smoke* gBR_smoke_pointers[35];

// GLOBAL: CARMA2_HW 0x006aa56c
int gN_BR_smoke_structs;

// GLOBAL: CARMA2_HW 0x006a8ce0
tSmoke gSmoke[25];

// GLOBAL: CARMA2_HW 0x006aa590
int gSmoke_flags;

// GLOBAL: CARMA2_HW 0x006aa594
int gSmoke_num;

#ifndef CARPOCALYPSE2_MATCHING
// The original keeps the smoke prim's token list as a raw blob at 0x660148
// (BRT_BLEND_B = 1, BRT_OPACITY_X = 0x4b0000, BR_NULL_TOKEN) and points every
// smoke material at it; RenderRecordedSmokeCircles rewrites the opacity entry
// just before each BrMaterialUpdate, so one shared list is enough.
//
// That blob is a 32-bit br_token_value array, which only lines up in the
// 32-bit matching build. On 64-bit br_value is 8 bytes wide, so a typed copy
// is needed here: without one the portable InitSmokeStuff leaves extra_prim
// NULL and the first recorded smoke circle faults writing
// smoke->material->extra_prim[1].v.x (spark.c, RenderRecordedSmokeCircles).
static br_token_value gSmoke_extra_prim[] = {
    { BRT_BLEND_B,   { 1u } },       // v.u32
    { BRT_OPACITY_X, { 0x4b0000u } }, // v.x (16.16 fixed; rewritten per smoke)
    { BR_NULL_TOKEN, { 0u } },
};
#endif

void (C2_HOOK_FASTCALL * StopCarBeingIt_original)(tCar_spec* pCar);
// GLOBAL: CARMA2_HW 0x006b7880
br_material* gBlack_material;

// GLOBAL: CARMA2_HW 0x00660110
int gSmoke_on = 1;

static br_scalar C2_HOOK_CDECL SmokeScalarRound(double pValue) {
    volatile union { br_scalar scalar; tU32 bits; } stored;
    union { br_scalar scalar; tU32 bits; } result;

    stored.scalar = (br_scalar)pValue;
    result.bits = stored.bits;
    return result.scalar;
}

static tU32 C2_HOOK_CDECL SmokeScalarBits(const br_scalar* pValue) {
    tU32 bits;

    memcpy(&bits, pValue, sizeof(bits));
    return bits;
}

static double C2_HOOK_CDECL SplashSubtractFromScalar(double pValue, const br_scalar* pScalar) {
    double result;

#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        mov ecx, pScalar
        fld pValue
        fsubr dword ptr [ecx]
        fstp result
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ volatile (
        "fldl %[value]\n\t"
        "fsubrs %[scalar]\n\t"
        "fstpl %[result]\n\t"
        : [result] "=m" (result)
        : [value] "m" (pValue), [scalar] "m" (*pScalar)
        : "st", "memory");
#else
    result = (double)*pScalar - pValue;
#endif
    return result;
}

static int C2_HOOK_CDECL SplashReplayMoving(void) {
    br_scalar rate;
    tU32 bits;

    rate = ARGetReplayRate();
    bits = SmokeScalarBits(&rate) & 0x7fffffffu;
    return bits != 0 && bits <= 0x7f800000u;
}

static tS32 C2_HOOK_CDECL SparkFtol(double pValue) {
    tU32 words[2];

    memcpy(words, &pValue, sizeof(words));
    if ((words[1] & 0x7fffffffu) >= 0x43e00000u) {
        return 0;
    }
    return (tS32)(tU32)(br_int_64)pValue;
}

static int C2_HOOK_CDECL SparkClipInteger(int pOrigin, int pOther, int pAmount, int pDivisor) {
    tS32 product;

    product = (tS32)(((tU32)pOrigin - (tU32)pOther) * (tU32)pAmount);
    return (tS32)((tU32)pOrigin - (tU32)(product / pDivisor));
}

static void C2_HOOK_CDECL SparkClipDepth(br_vector3* pPoint, const br_vector3* pOther, int pAmount, int pDivisor) {
    pPoint->v[2] = SmokeScalarRound((double)pOther->v[2]
        - (((double)pOther->v[2] - pPoint->v[2]) * pAmount) / pDivisor);
}

#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_FASTCALL AREndPipingSession(void);
extern float C2_HOOK_FASTCALL ARGetReplayRate(void);
extern void C2_HOOK_FASTCALL ARStartPipingSession(tPipe_chunk_type pType);
extern void C2_HOOK_FASTCALL AddFlameToPipingSession(int pIndex, int pFrame, br_scalar pScale_x, br_scalar pScale_y, br_scalar pOffset_x, br_scalar pOffset_z);
extern void C2_HOOK_FASTCALL AddSmokeColumnToPipingSession(int pIndex, tSmoke_column* pSmoke_column, int pVertex, int pColour);
extern void C2_HOOK_FASTCALL AddSmudgeToPipingSession(tU16 pCar_ID, int pModel_index, int pVertex_count, tSmudged_vertex* pCoordinates);
extern br_actor* C2_HOOK_CDECL BrActorAdd();
extern void C2_HOOK_CDECL BrActorFree();
extern br_actor* C2_HOOK_CDECL BrActorRemove();
extern br_pixelmap* C2_HOOK_CDECL BrMapRemove();
extern void C2_HOOK_CDECL BrMaterialFree();
extern br_material* C2_HOOK_CDECL BrMaterialRemove();
extern void C2_HOOK_CDECL BrMaterialUpdate();
extern void C2_HOOK_CDECL BrMatrix34Scale();
extern void C2_HOOK_CDECL BrMatrix34TApplyV();
extern void C2_HOOK_CDECL BrModelFree();
extern br_model* C2_HOOK_CDECL BrModelRemove();
extern void C2_HOOK_CDECL BrModelUpdate();
extern br_uint_32 C2_HOOK_CDECL BrOnScreenCheck();
extern void C2_HOOK_CDECL BrPixelmapFree();
extern void C2_HOOK_CDECL BrVector3SetFloat();
extern void C2_HOOK_CDECL BrZsModelRender();
extern void C2_HOOK_FASTCALL CreateShrapnelShower(const br_vector3* pPosition, const br_vector3* pVelocity, const br_vector3* pDirection, br_scalar pStrength, tCar_spec* pCar1, tCar_spec* pCar2, int pMaterialCount, br_material** pMaterials);
extern void C2_HOOK_FASTCALL CreateSmokeColumn2(int pType, br_actor* pActor, tCar_spec* pCar, tPhysics_object* pObject, int pColour, int pVertex, tU32 pLifetime);
extern intptr_t C2_HOOK_FASTCALL DRActorEnumRecurse(br_actor* pActor, br_actor_enum_cbfn* callback, void* arg);
extern void C2_HOOK_FASTCALL DRMatrix34RotateY(br_matrix34* mat, br_angle ry);
extern void C2_HOOK_FASTCALL DisposeFlameResources(void);
extern float C2_HOOK_STDCALL FRandomBetween(float pA, float pB);
extern br_angle C2_HOOK_STDCALL FastScalarArcTan2Angle(br_scalar pY, br_scalar pX);
extern br_scalar C2_HOOK_FASTCALL FastScalarCosAngle(br_angle pBR_angle);
extern br_scalar C2_HOOK_FASTCALL FastScalarSinAngle(br_angle pBR_angle);
extern void C2_HOOK_FASTCALL ForEveryCarMaterial(tCar_spec* pCar_spec, tMaterialMaybeUpdate_cbfn* pCallback, int pGrooves);
extern tU32 C2_HOOK_FASTCALL GetTotalTime(void);
extern int C2_HOOK_FASTCALL IRandomBetween(int pA, int pB);
extern void C2_HOOK_FASTCALL InitialiseExternalCamera(void);
extern void C2_HOOK_FASTCALL MasterDisableFunkotronic(int pFunk_index);
extern void C2_HOOK_FASTCALL MasterEnableFunkotronic(int pFunk_index);
extern void C2_HOOK_FASTCALL PositionExternalCamera(tCar_spec* pCar_spec, tU32 pTime);
extern int C2_HOOK_FASTCALL ResetFoxLighting(br_material* pMaterial);
extern int C2_HOOK_FASTCALL ResetFoxModelCallback(br_model* pModel);
extern int C2_HOOK_FASTCALL RestorePixelmap(br_material* pMaterial);
extern br_scalar C2_HOOK_STDCALL SRandomBetween(br_scalar pA, br_scalar pB);
extern br_scalar C2_HOOK_STDCALL SRandomPosNeg(br_scalar pN);
extern void C2_HOOK_FASTCALL SmudgeCar(tCar_spec* pCar, int fire_point);
extern intptr_t C2_HOOK_CDECL StopCarBeingItActorCallback();
extern void __cdecl _ftol();
extern int gAFE;
extern br_matrix34 gCamera_to_world;
extern br_angle gCamera_yaw;
extern tCar_spec* gCar_to_view;
extern char gCrushZero[];
extern br_actor* gNon_track_actor;
extern int gPHIL_mechanics_time_sync;
extern int gPed_676914;
extern tProgram_state gProgram_state;
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_CDECL BrMatrix34ApplyP();
extern int C2_HOOK_FASTCALL FUN_004f9390(br_model* pModel);
extern int C2_HOOK_FASTCALL FUN_004f93b0(br_material* pMaterial);
extern intptr_t C2_HOOK_CDECL ForEveryActorModelNoGrooves();
#endif
#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_FASTCALL DisposeFlames(void);
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern const br_scalar gFLOAT_0058b130;
extern intptr_t C2_HOOK_CDECL ActorFunks();
extern void C2_HOOK_FASTCALL AddShrapnelToPipingSession(int pShrapnel_index, const br_vector3* pPos, tU16 pAge, br_material* pMaterial);
extern void C2_HOOK_FASTCALL AddSmokeToPipingSession(int pIndex, tU8 pType, br_vector3* pPos, br_scalar pRadius, br_scalar pStrength);
extern void C2_HOOK_FASTCALL AddSparkToPipingSession(int pSpark_index, const br_vector3* pPos, const br_vector3* pV);
extern br_actor* C2_HOOK_CDECL BrActorAllocate();
extern br_uint_32 C2_HOOK_CDECL BrMapAddMany();
extern br_material* C2_HOOK_CDECL BrMaterialAdd();
extern br_material* C2_HOOK_CDECL BrMaterialAllocate();
extern br_material* C2_HOOK_CDECL BrMaterialFind();
extern void C2_HOOK_CDECL BrMatrix34ApplyV();
extern void C2_HOOK_CDECL BrMatrix34PreShearX();
extern br_quat* C2_HOOK_CDECL BrMatrix34ToQuat();
extern void C2_HOOK_CDECL BrMatrix4Mul();
extern void C2_HOOK_CDECL BrMatrix4Perspective();
extern void C2_HOOK_CDECL BrMatrix4Scale();
extern br_model* C2_HOOK_CDECL BrModelAdd();
extern br_model* C2_HOOK_CDECL BrModelAllocate();
extern br_quat* C2_HOOK_CDECL BrQuatInvert();
extern br_quat* C2_HOOK_CDECL BrQuatMul();
extern void C2_HOOK_FASTCALL CreatePuffOfSmoke(br_vector3* pPos, br_vector3* pV, float pStrength, float pDecay_factor, int pType);
extern void C2_HOOK_FASTCALL CreateSplash(tPhysics_object* pObject, tU32 pTime);
extern void C2_HOOK_FASTCALL DRMatrix34PreRotateZ(br_matrix34* mat, br_angle rz);
extern void C2_HOOK_FASTCALL DRMatrix34TApplyP(br_vector3* pA, const br_vector3* pB, const br_matrix34* pC);
extern void C2_HOOK_FASTCALL DoSmokeColumn(int i, tU32 pTime, br_vector3* pRet_car_pos);
extern void C2_HOOK_FASTCALL DrMatrix34Rotate(br_matrix34* mat, br_angle r, br_vector3* a);
extern void C2_NORETURN C2_HOOK_CDECL FatalError();
extern tPipe_chunk_data* C2_HOOK_FASTCALL FindNextChunk(tPipe_chunk_type pType, uintptr_t pOwner);
extern void C2_HOOK_FASTCALL FlameAnimate(int pColumn, const br_vector3* pPos, tU32 pTime);
extern void C2_HOOK_FASTCALL GeneralCreateSplash(tPhysics_object* pObject, const br_vector3* pVelocity, const br_vector3* pOmega, const br_vector3* pNormal, br_scalar pWater_distance, const br_bounds3* pBounds, const br_matrix34* pMatrix, tU32 pTime);
extern void C2_HOOK_FASTCALL GetAString(FILE* pF, char* pString);
extern int C2_HOOK_FASTCALL GetAnInt(FILE* pF);
extern int C2_HOOK_FASTCALL GetCarCount(tVehicle_type pCategory);
extern tCar_spec* C2_HOOK_FASTCALL GetCarSpec(tVehicle_type pCategory, int pIndex);
extern br_actor* C2_HOOK_FASTCALL GetCharacterActorPtr(tPed_character_instance* pCharacter);
extern void C2_HOOK_FASTCALL GetVelocitiesFromMatrices(br_vector3* pVel, br_vector3* pRot, br_matrix34* pMat_now, br_matrix34* pMat_prev, tU32 pTime);
extern int C2_HOOK_FASTCALL IsOnNonCarHalf(tCar_spec* pCar, br_vector3 *pPos);
extern int C2_HOOK_FASTCALL LoadTextureTryAllLocations(char* pName, br_pixelmap** pMaps, int pCapacity);
extern void C2_HOOK_FASTCALL MungeSmokeColumn(tU32 pTime);
extern tPhysics_object* C2_HOOK_FASTCALL PHILGetFirstObject(void);
extern tPhysics_object* C2_HOOK_FASTCALL PHILGetNextObject(tPhysics_object* pCollision_info);
extern void C2_HOOK_FASTCALL PathCat(char* pDestn_str, const char* pStr_1, const char* pStr_2);
extern void C2_HOOK_FASTCALL RenderRecordedSmokeCircles(void);
extern br_pixelmap* gAcid_shade_table;
extern tBRender_smoke gBR_smoke_structs[35];
extern br_model* gBlend_model;
extern br_model* gBlend_model2;
extern char gCurrent_load_directory[256];
extern tRace_info gCurrent_race;
extern br_token_value* gDAT_00660148;
extern br_actor* gDont_render_actor;
extern char gFloat_0058afb4[];
extern br_pixelmap* gFog_shade_table;
extern int gNo_2d_effects;
extern tPedestrian* gPedestrian_array;
extern br_matrix34 gRearview_camera_to_world;
extern int gRendering_mirror;
extern char gSplit_triangle_vertex_orders[];
#endif
// FUNCTION: CARMA2_HW 0x004fb910
void C2_HOOK_FASTCALL GenerateSmokeShades(void) {
    // GLOBAL: CARMA2_HW 0x006aa5b4
    static int rb = 0;
    // GLOBAL: CARMA2_HW 0x006aa5b8
    static int gb = 0;
    // GLOBAL: CARMA2_HW 0x006aa5bc
    static int bb = 0;
    // GLOBAL: CARMA2_HW 0x00660160
    static int rl = 0x40;
    // GLOBAL: CARMA2_HW 0x00660164
    static int gl = 0x40;
    // GLOBAL: CARMA2_HW 0x00660168
    static int bl = 0x40;
    // GLOBAL: CARMA2_HW 0x00660178
    static int rs = 0xc8;
    // GLOBAL: CARMA2_HW 0x0066017c
    static int gs = 0xc8;
    // GLOBAL: CARMA2_HW 0x00660180
    static int bs = 0xc8;
    // GLOBAL: CARMA2_HW 0x0066016c
    static int rm = 0x80;
    // GLOBAL: CARMA2_HW 0x00660170
    static int gm = 0x80;
    // GLOBAL: CARMA2_HW 0x00660174
    static int bm = 0x80;
    // GLOBAL: CARMA2_HW 0x00660184
    static int rd = 0xfe;
    // GLOBAL: CARMA2_HW 0x00660188
    static int gd = 0xdf;
    // GLOBAL: CARMA2_HW 0x0066018c
    static int bd = 0x43;

    gShade_list[0] = BR_COLOUR_RGB(rb, gb, bb);
    gShade_list[1] = BR_COLOUR_RGB(rl, gl, bl);
    gShade_list[2] = BR_COLOUR_RGB(rm, gm, bm);
    gShade_list[3] = BR_COLOUR_RGB(rs, gs, bs);
    gShade_list[4] = gShade_list[3];
    gShade_list[7] = BR_COLOUR_RGB(rd, gd, bd);
}

// FUNCTION: CARMA2_HW 0x004fdb70
void C2_HOOK_FASTCALL GetSmokeShadeTables(FILE* f) {
    int i;
    int red;
    int green;
    int blue;
    float quarter;
    float half;
    float three_quarter;

    gNum_dust_tables = GetAnInt(f);
    if (gNum_dust_tables > 8) {
        gNum_dust_tables = 8;
    }
    for (i = 0; i < gNum_dust_tables; i++) {
        PossibleService();
        GetThreeInts(f, &red, &green, &blue);
        GetThreeScalars(f, &quarter, &half, &three_quarter);
        gShade_table[i] = (red << 16) | (green << 8) | (blue << 0);
    }
}

void C2_HOOK_FASTCALL ModelScale(br_model* pModel, float pScale) {
    int i;

    for (i = 0; i < pModel->nvertices; i++) {

        BrVector3Scale(&pModel->vertices[i].p, &pModel->vertices[i].p, pScale);
    }
    BrModelUpdate(pModel, BR_MODU_ALL);
}

// FUNCTION: CARMA2_HW 0x004f87a0
void C2_HOOK_FASTCALL ResetSparks(void) {
    gSpark_flags = 0;
}

// FUNCTION: CARMA2_HW 0x004f8b90
void C2_HOOK_FASTCALL LoadInShrapnel(void) {

    gShrapnel_model[0] = LoadModel("FRAG4.DAT");
    DRScaleModel(gShrapnel_model[0], 0.5f);
    gShrapnel_model[1] = LoadModel("FRAG5.DAT");
    DRScaleModel(gShrapnel_model[1], 0.5f);
    BrModelAdd(gShrapnel_model[0]);
    BrModelAdd(gShrapnel_model[1]);
    gBlack_material = GetSimpleMaterial("M14.MAT", kRendererShadingType_AmbientOnly);
    gNon_track_actor->material = gBlack_material;
}

void C2_HOOK_FASTCALL InitShrapnel(void) {
    int i;

    for (i = 0; i < CARPOCALYPSE2_ASIZE(gShrapnel); i++) {
        gShrapnel[i].actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
        gShrapnel[i].actor->parent = NULL;
        gShrapnel[i].actor->model = gShrapnel_model[i % 2];
        gShrapnel[i].actor->render_style = BR_RSTYLE_DEFAULT;
        gShrapnel[i].actor->t.type = BR_TRANSFORM_MATRIX34;
        gShrapnel[i].actor->material = BrMaterialFind("DEBRIS.MAT");
        gShrapnel[i].age = 0;
        gShrapnel[i].shear1 = FRandomBetween(-2.f, 2.f);
        gShrapnel[i].shear2 = FRandomBetween(-2.f, 2.f);
        BrVector3SetFloat(&gShrapnel[i].axis,
            FRandomBetween(-1.f, 1.f),
            FRandomBetween(-1.f, 1.f),
            FRandomBetween(-1.f, 1.f));
        BrVector3Normalise(&gShrapnel[i].axis, &gShrapnel[i].axis);
    }
}

// FUNCTION: CARMA2_HW 0x004fc3a0
void C2_HOOK_FASTCALL InitFlame(void) {
    int i;
    int j;
    int num;
    char name[256];
    br_actor* actor;
    br_material* material;

    gColumn_flags = 0;
    gLollipop_model = BrModelAllocate("Lollipop", 4, 2);
    strcpy(name, "FLAMES.PIX");
    strcpy(gCurrent_load_directory, "COMMON");
    num = LoadTextureTryAllLocations(name, gFlame_map, (int)CARPOCALYPSE2_ASIZE(gFlame_map));
    if (num != (int)CARPOCALYPSE2_ASIZE(gFlame_map)) {
        FatalError(kFatalError_CantLoadPixelmapFile_S, name);
    }
    C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(gFlame_map) != 20);
    BrMapAddMany(gFlame_map, num);

    C2_HOOK_BUG_ON(sizeof(tSmoke_column) != 124);
    C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(gSmoke_column) != 10);
    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gSmoke_column); i++) {

        gSmoke_column[i].flame_actor = BrActorAllocate(BR_ACTOR_NONE, NULL);
        for (j = 0; j < (int)CARPOCALYPSE2_ASIZE(gSmoke_column[i].frame_count); j++) {
            actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
            material = BrMaterialAllocate(NULL);
            BrActorAdd(gSmoke_column[i].flame_actor, actor);
            actor->model = gLollipop_model;
            actor->material = material;
            material->flags &= ~BR_MATF_LIGHT;
            material->flags |= BR_MATF_ALWAYS_VISIBLE;
            material->colour_map = gFlame_map[0];
            GlorifyMaterial(&material, 1, kRendererShadingType_AmbientOnly);
            BrMaterialAdd(material);
            gSmoke_column[i].frame_count[j] = 100;
        }
    }
    gLollipop_model->nvertices = 4;
    BrVector3SetFloat(&gLollipop_model->vertices[0].p, -.5f, 0.f, .0f);
    BrVector3SetFloat(&gLollipop_model->vertices[1].p, .5f, 0.f, .0f);
    BrVector3SetFloat(&gLollipop_model->vertices[2].p, .5f, 1.f, .0f);
    BrVector3SetFloat(&gLollipop_model->vertices[3].p, -.5f, 1.f, .0f);
    BrVector2Set(&gLollipop_model->vertices[0].map, 0.f, 1.f);
    BrVector2Set(&gLollipop_model->vertices[1].map, 1.f, 1.f);
    BrVector2Set(&gLollipop_model->vertices[2].map, 1.f, 0.f);
    BrVector2Set(&gLollipop_model->vertices[3].map, 0.f, 0.f);

    gLollipop_model->nfaces = 2;
    gLollipop_model->faces[0].vertices[0] = 0;
    gLollipop_model->faces[0].vertices[1] = 1;
    gLollipop_model->faces[0].vertices[2] = 2;
    gLollipop_model->faces[1].vertices[0] = 0;
    gLollipop_model->faces[1].vertices[1] = 2;
    gLollipop_model->faces[1].vertices[2] = 3;
    gLollipop_model->faces[0].smoothing = 1;
    gLollipop_model->faces[1].smoothing = 1;
    BrModelAdd(gLollipop_model);
}

// FUNCTION: CARMA2_HW 0x004fdde0
void C2_NAKED C2_HOOK_FASTCALL InitSplash(FILE* pF) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;
    int num_files;
    int num;
    br_actor* actor;
    char the_path[256];
    char s[256];
    br_pixelmap* splash_maps[20];

    gSplash_flags = 0;
    gSplash_model = BrModelAllocate("Splash", 4, 2);
    strcpy(gCurrent_load_directory, "COMMON");
    if (pF != NULL) {
        num = GetAnInt(pF);
        gNum_splash_types = 0;
        for (i = 0; i < num; i++) {
            GetAString(pF, s);
#if 0
            PathCat(the_path, the_path, s);
#else
            strcpy(the_path, s);
#endif
            num_files = LoadTextureTryAllLocations(the_path, &splash_maps[gNum_splash_types], CARPOCALYPSE2_ASIZE(splash_maps));
            if (num_files == 0) {
                FatalError(kFatalError_CantLoadPixelmapFile_S, the_path);
            }
            gNum_splash_types += num_files;
        }
    } else {
        strcpy(the_path, "SPLSHBLU.PIX");
        gNum_splash_types = LoadTextureTryAllLocations(the_path, splash_maps, CARPOCALYPSE2_ASIZE(splash_maps));
    }
    BrMapAddMany(splash_maps, gNum_splash_types);
    for (i = 0; i < gNum_splash_types; i++) {
        gSplash_material[i] = BrMaterialAllocate(NULL);
        gSplash_material[i]->flags &= ~BR_MATF_LIGHT;
        gSplash_material[i]->flags |= BR_MATF_ALWAYS_VISIBLE;
        gSplash_material[i]->colour_map = splash_maps[i];
        BrMaterialAdd(gSplash_material[i]);
    }
    gSplash_model->nvertices = 4;
    BrVector3SetFloat(&gSplash_model->vertices[0].p, -0.5f, 0.0f, 0.0f);
    BrVector3SetFloat(&gSplash_model->vertices[1].p,  0.5f, 0.0f, 0.0f);
    BrVector3SetFloat(&gSplash_model->vertices[2].p,  0.5f, 1.0f, 0.0f);
    BrVector3SetFloat(&gSplash_model->vertices[3].p, -0.5f, 1.0f, 0.0f);
    BrVector2Set(&gSplash_model->vertices[0].map, 0.0f, 1.0f);
    BrVector2Set(&gSplash_model->vertices[1].map, 1.0f, 1.0f);
    BrVector2Set(&gSplash_model->vertices[2].map, 1.0f, 0.0f);
    BrVector2Set(&gSplash_model->vertices[3].map, 0.0f, 0.0f);
    gSplash_model->nfaces = 2;
    gSplash_model->faces[0].vertices[0] = 0;
    gSplash_model->faces[0].vertices[1] = 1;
    gSplash_model->faces[0].vertices[2] = 2;
    gSplash_model->faces[1].vertices[0] = 0;
    gSplash_model->faces[1].vertices[1] = 2;
    gSplash_model->faces[1].vertices[2] = 3;
    gSplash_model->faces[0].smoothing = 1;
    gSplash_model->faces[1].smoothing = 1;
    BrModelAdd(gSplash_model);
    for (i = 0; i < CARPOCALYPSE2_ASIZE(gSplash); i++) {
        gSplash[i].actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
        actor = gSplash[i].actor;
        actor->model = gSplash_model;
        if (gNum_splash_types != 0) {
            actor->material = gSplash_material[IRandomBetween(0, gNum_splash_types - 1)];
        } else {
            actor->material = NULL;
        }
        gSplash[i].scale_x = SRandomBetween(0.9f, 1.1f) * (2 * IRandomBetween(0, 1) - 1);
    }
#else
    __asm {
            sub esp, 0x254
            push ebx
            push ebp
            push esi
            push edi
            push 2
            xor esi, esi
            push 4
            mov ebx, ecx
            push offset gDAT_00660148+0x104
            mov dword ptr [gSplash_flags], esi
            call BrModelAllocate
            mov cx, word ptr [gSplit_triangle_vertex_orders+0x204]
            mov dl, byte ptr [gSplit_triangle_vertex_orders+0x206]
            mov dword ptr [gSplash_model], eax
            mov eax, dword ptr [gSplit_triangle_vertex_orders+0x200]
            add esp, 0xc
            cmp ebx, esi
            mov dword ptr [gCurrent_load_directory], eax
            mov word ptr [gCurrent_load_directory+0x4], cx
            mov byte ptr [gCurrent_load_directory+0x6], dl
            je L004fdede
            mov ecx, ebx
            call GetAnInt
            cmp eax, esi
            mov dword ptr [gNum_splash_types], esi
            jle L004fdf18
            mov ebp, eax
    L004fde4d:
            lea edx, [esp + 0x164]
            mov ecx, ebx
            call GetAString
            lea eax, [esp + 0x164]
            lea edx, [esp + 0x64]
            push eax
            lea ecx, [esp + 0x68]
            call PathCat
            lea edi, [esp + 0x164]
            or ecx, 0xffffffff
            xor eax, eax
            lea edx, [esp + 0x64]
            repne scasb
            not ecx
            sub edi, ecx
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            shr ecx, 2
            rep movsd
            mov ecx, eax
            mov eax, dword ptr [gNum_splash_types]
            and ecx, 3
            rep movsb
            mov ecx, 0x14
            lea edx, [esp + eax*4 + 0x14]
            sub ecx, eax
            push ecx
            lea ecx, [esp + 0x68]
            call LoadTextureTryAllLocations
            mov esi, eax
            test esi, esi
            jne L004fdec7
            lea edx, [esp + 0x64]
            push edx
            push 0x4f
            call FatalError
            add esp, 8
    L004fdec7:
            mov ecx, dword ptr [gNum_splash_types]
            add ecx, esi
            dec ebp
            mov dword ptr [gNum_splash_types], ecx
            jne L004fde4d
            jmp L004fdf18
    L004fdede:
            mov eax, dword ptr [gDAT_00660148+0xf4]
            mov ecx, dword ptr [gDAT_00660148+0xf8]
            mov edx, dword ptr [gDAT_00660148+0xfc]
            mov dword ptr [esp + 0x64], eax
            mov al, byte ptr [gDAT_00660148+0x100]
            mov dword ptr [esp + 0x68], ecx
            mov dword ptr [esp + 0x6c], edx
            push 0x14
            lea edx, [esp + 0x18]
            lea ecx, [esp + 0x68]
            mov byte ptr [esp + 0x74], al
            call LoadTextureTryAllLocations
            mov dword ptr [gNum_splash_types], eax
    L004fdf18:
            mov ecx, dword ptr [gNum_splash_types]
            lea edx, [esp + 0x14]
            push ecx
            push edx
            call BrMapAddMany
            mov eax, dword ptr [gNum_splash_types]
            add esp, 8
            xor esi, esi
            test eax, eax
            jle L004fdf91
            mov ebx, 0xfffffffe
            mov edi, 0x800
    L004fdf41:
            push 0
            call BrMaterialAllocate
            mov dword ptr [esi*4 + gSplash_material], eax
            mov edx, dword ptr [eax + 0x20]
            and edx, ebx
            add esp, 4
            mov dword ptr [eax + 0x20], edx
            mov eax, dword ptr [esi*4 + gSplash_material]
            mov ecx, dword ptr [eax + 0x20]
            or ecx, edi
            mov dword ptr [eax + 0x20], ecx
            mov eax, dword ptr [esi*4 + gSplash_material]
            mov ecx, dword ptr [esp + esi*4 + 0x14]
            mov dword ptr [eax + 0x40], ecx
            mov edx, dword ptr [esi*4 + gSplash_material]
            push edx
            call BrMaterialAdd
            mov eax, dword ptr [gNum_splash_types]
            add esp, 4
            inc esi
            cmp esi, eax
            jl L004fdf41
    L004fdf91:
            mov eax, dword ptr [gSplash_model]
            xor edi, edi
            push edi
            push edi
            mov word ptr [eax + 0x10], 4
            mov ecx, dword ptr [gSplash_model]
            push 0xbf000000
            mov edx, dword ptr [ecx + 8]
            push edx
            call BrVector3SetFloat
            mov eax, dword ptr [gSplash_model]
            add esp, 0x10
            mov ecx, dword ptr [eax + 8]
            push edi
            push edi
            add ecx, 0x28
            push 0x3f000000
            push ecx
            call BrVector3SetFloat
            mov edx, dword ptr [gSplash_model]
            add esp, 0x10
            mov esi, 0x3f800000
            mov eax, dword ptr [edx + 8]
            push edi
            push esi
            add eax, 0x50
            push 0x3f000000
            push eax
            call BrVector3SetFloat
            mov ecx, dword ptr [gSplash_model]
            add esp, 0x10
            mov edx, dword ptr [ecx + 8]
            push edi
            push esi
            add edx, 0x78
            push 0xbf000000
            push edx
            call BrVector3SetFloat
            mov eax, dword ptr [gSplash_model]
            mov ebx, 1
            add esp, 0x10
            mov ecx, dword ptr [eax + 8]
            mov dword ptr [ecx + 0xc], edi
            mov edx, dword ptr [gSplash_model]
            mov eax, dword ptr [edx + 8]
            mov dword ptr [eax + 0x10], esi
            mov ecx, dword ptr [gSplash_model]
            mov edx, dword ptr [ecx + 8]
            mov dword ptr [edx + 0x34], esi
            mov eax, dword ptr [gSplash_model]
            mov ecx, dword ptr [eax + 8]
            mov dword ptr [ecx + 0x38], esi
            mov edx, dword ptr [gSplash_model]
            mov eax, dword ptr [edx + 8]
            mov dword ptr [eax + 0x5c], esi
            mov ecx, dword ptr [gSplash_model]
            mov edx, dword ptr [ecx + 8]
            mov dword ptr [edx + 0x60], edi
            mov eax, dword ptr [gSplash_model]
            mov ecx, dword ptr [eax + 8]
            mov dword ptr [ecx + 0x84], edi
            mov edx, dword ptr [gSplash_model]
            mov eax, dword ptr [edx + 8]
            mov dword ptr [eax + 0x88], edi
            mov ecx, dword ptr [gSplash_model]
            mov eax, 2
            mov word ptr [ecx + 0x12], ax
            mov edx, dword ptr [gSplash_model]
            mov ecx, dword ptr [edx + 0xc]
            mov word ptr [ecx], di
            mov edx, dword ptr [gSplash_model]
            mov ecx, dword ptr [edx + 0xc]
            mov word ptr [ecx + 2], bx
            mov edx, dword ptr [gSplash_model]
            mov ecx, dword ptr [edx + 0xc]
            mov word ptr [ecx + 4], ax
            mov edx, dword ptr [gSplash_model]
            mov ecx, dword ptr [edx + 0xc]
            mov word ptr [ecx + 0x28], di
            mov edx, dword ptr [gSplash_model]
            mov ecx, dword ptr [edx + 0xc]
            mov word ptr [ecx + 0x2a], ax
            mov edx, dword ptr [gSplash_model]
            mov eax, dword ptr [edx + 0xc]
            mov word ptr [eax + 0x2c], 3
            mov ecx, dword ptr [gSplash_model]
            mov edx, dword ptr [ecx + 0xc]
            mov word ptr [edx + 6], bx
            mov eax, dword ptr [gSplash_model]
            mov ecx, dword ptr [eax + 0xc]
            mov word ptr [ecx + 0x2e], bx
            mov edx, dword ptr [gSplash_model]
            push edx
            call BrModelAdd
            add esp, 4
            mov esi, offset gSplash+0x18
    L004fe102:
            push 0
            push ebx
            call BrActorAllocate
            mov edi, eax
            mov eax, dword ptr [gSplash_model]
            mov dword ptr [esi - 0x18], edi
            add esp, 8
            mov dword ptr [edi + 0x18], eax
            mov eax, dword ptr [gNum_splash_types]
            test eax, eax
            je L004fe136
            lea edx, [eax - 1]
            xor ecx, ecx
            call IRandomBetween
            mov eax, dword ptr [eax*4 + gSplash_material]
            jmp L004fe138
    L004fe136:
            xor eax, eax
    L004fe138:
            mov edx, ebx
            xor ecx, ecx
            mov dword ptr [edi + 0x1c], eax
            call IRandomBetween
            lea ecx, [eax + eax - 1]
            mov dword ptr [esp + 0x10], ecx
            push 0x3f8ccccd
            fild dword ptr [esp + 0x14]
            push 0x3f666666
            fstp dword ptr [esp + 0x18]
            call SRandomBetween
            add esi, 0x1c
            fmul dword ptr [esp + 0x10]
            cmp esi, offset gFlame_map+0x18
            fstp dword ptr [esi - 0x1c]
            jl L004fe102
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x254
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004fdc00
void C2_HOOK_FASTCALL FreeSmokeShadeTables(void) {
}

// FUNCTION: CARMA2_HW 0x004fdc10
void C2_HOOK_FASTCALL LoadInKevStuff(FILE* pF) {

    PackFileRevertTiffLoading();
    PossibleService();
    LoadInShrapnel();
    PossibleService();
    InitShrapnel();
    PossibleService();
    InitFlame();
    PossibleService();
    InitSplash(pF);
    PackFileRerevertTiffLoading();
}

// UNLABELED: CARMA2_HW 0x004fc2e0
void C2_HOOK_FASTCALL DisposeFlames(void) {
    int i;
    int j;
    br_actor* child;

    for (i = 0; i < CARPOCALYPSE2_ASIZE(gFlame_map); i++) {
        BrMapRemove(gFlame_map[i]);
        BrPixelmapFree(gFlame_map[i]);
    }
    for (i = 0; i < CARPOCALYPSE2_ASIZE(gSmoke_column); i++) {
        if ((gColumn_flags & (1 << i)) != 0 && gSmoke_column[i].colour == 0) {
            BrActorRemove(gSmoke_column[i].flame_actor);
        }
        child = gSmoke_column[i].flame_actor->children;
        for (j = 0; j < 3; j++) {
            BrMaterialRemove(child->material);
            BrMaterialFree(child->material);
            child = child->next;
        }
        BrActorFree(gSmoke_column[i].flame_actor);
    }
    BrModelRemove(gLollipop_model);
    BrModelFree(gLollipop_model);
}

// FUNCTION: CARMA2_HW 0x004fe180
void C2_NAKED C2_HOOK_FASTCALL DisposeKevStuff(void) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;

    for (i = 0; i < CARPOCALYPSE2_ASIZE(gShrapnel); i++) {
        if ((gShrapnel_flags & (1 << i)) != 0 && gShrapnel[i].actor->parent != NULL) {
            BrActorRemove(gShrapnel[i].actor);
        }
        BrActorFree(gShrapnel[i].actor);
    }
    BrModelRemove(gShrapnel_model[0]);
    BrModelRemove(gShrapnel_model[1]);
    BrModelFree(gShrapnel_model[0]);
    BrModelFree(gShrapnel_model[1]);
    DisposeFlames();
    for (i = 0; i < gNum_splash_types; i++) {
        BrMapRemove(gSplash_material[i]->colour_map);
        BrPixelmapFree(gSplash_material[i]->colour_map);
        BrMaterialRemove(gSplash_material[i]);
        BrMaterialFree(gSplash_material[i]);
    }
    for (i = 0; i < CARPOCALYPSE2_ASIZE(gSplash); i++) {
        if ((gSplash_flags & (1 << i)) != 0 && gSplash[i].actor->parent != NULL) {
            BrActorRemove(gSplash[i].actor);
        }
        BrActorFree(gSplash[i].actor);
    }
    BrModelRemove(gSplash_model);
    BrModelFree(gSplash_model);
#else
    __asm {
            push esi
            push edi
            xor edi, edi
            mov esi, offset gShrapnel
    L004fe189:
            mov eax, 1
            mov ecx, edi
            shl eax, cl
            test dword ptr [gShrapnel_flags], eax
            je L004fe1ac
            mov eax, dword ptr [esi]
            mov ecx, dword ptr [eax + 0xc]
            test ecx, ecx
            je L004fe1ac
            push eax
            call BrActorRemove
            add esp, 4
    L004fe1ac:
            mov ecx, dword ptr [esi]
            push ecx
            call BrActorFree
            add esp, 4
            add esi, 0x2c
            inc edi
            cmp esi, offset gSmoke_column
            jl L004fe189
            mov edx, dword ptr [gShrapnel_model]
            mov dword ptr [gShrapnel_flags], 0
            push edx
            call BrModelRemove
            mov eax, dword ptr [gShrapnel_model+0x4]
            add esp, 4
            push eax
            call BrModelRemove
            mov ecx, dword ptr [gShrapnel_model]
            add esp, 4
            push ecx
            call BrModelFree
            mov edx, dword ptr [gShrapnel_model+0x4]
            add esp, 4
            push edx
            call BrModelFree
            add esp, 4
            call DisposeFlames
            mov eax, dword ptr [gNum_splash_types]
            xor edi, edi
            test eax, eax
            jle L004fe25c
            mov esi, offset gSplash_material
    L004fe21d:
            mov eax, dword ptr [esi]
            mov ecx, dword ptr [eax + 0x40]
            push ecx
            call BrMapRemove
            mov edx, dword ptr [esi]
            add esp, 4
            mov eax, dword ptr [edx + 0x40]
            push eax
            call BrPixelmapFree
            mov ecx, dword ptr [esi]
            add esp, 4
            push ecx
            call BrMaterialRemove
            mov edx, dword ptr [esi]
            add esp, 4
            push edx
            call BrMaterialFree
            mov eax, dword ptr [gNum_splash_types]
            add esp, 4
            inc edi
            add esi, 4
            cmp edi, eax
            jl L004fe21d
    L004fe25c:
            xor edi, edi
            mov esi, offset gSplash
    L004fe263:
            mov eax, 1
            mov ecx, edi
            shl eax, cl
            test dword ptr [gSplash_flags], eax
            je L004fe286
            mov eax, dword ptr [esi]
            mov ecx, dword ptr [eax + 0xc]
            test ecx, ecx
            je L004fe286
            push eax
            call BrActorRemove
            add esp, 4
    L004fe286:
            mov ecx, dword ptr [esi]
            push ecx
            call BrActorFree
            add esp, 4
            add esi, 0x1c
            inc edi
            cmp esi, offset gFlame_map
            jl L004fe263
            mov edx, dword ptr [gSplash_model]
            push edx
            call BrModelRemove
            mov eax, dword ptr [gSplash_model]
            add esp, 4
            push eax
            call BrModelFree
            add esp, 4
            pop edi
            pop esi
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004f9180
void C2_NAKED C2_HOOK_CDECL AFEModelUpdate(br_actor* pActor, br_model* pModel, br_material* pMaterial, void* pOrder_table, br_uint_8 pStyle, int pOn_screen) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;
    int j;
    int time;
    int fade;
    v11model* prepared;
    v11group* group;
    br_vertex* vertex;

    if (gAFE_color_cycle) {
        time = (int)GetTotalTime() >> 1;
        if (pModel != NULL) {
            prepared = pModel->prepared;
            if (prepared->ngroups != 0) {
                for (i = 0; i < prepared->ngroups; i++) {
                    group = &prepared->groups[i];
                    for (j = 0; j < group->nvertices; j++) {
                        vertex = &pModel->vertices[group->vertex_user[j]];
                        if (vertex == NULL) {
                            continue;
                        }
                        fade = time & 0x3fff;
                        if (fade >= 0x100) {
                            fade = 0x3ff - fade;
                        }
                        vertex->red = (br_uint_8)fade;
                        fade = (time + 0x100) & 0x3ff;
                        if (fade >= 0x100) {
                            fade = 0x3ff - fade;
                        }
                        vertex->grn = (br_uint_8)fade;
                        fade = (time - 0x200) & 0x3ff;
                        if (fade >= 0x100) {
                            fade = 0x3ff - fade;
                        }
                        vertex->blu = (br_uint_8)fade;
                    }
                }
            }
            if (pModel->flags & BR_MODF_UPDATEABLE) {
                BrModelUpdate(pModel, BR_MODU_VERTICES);
            }
        }
    }
    BrZsModelRender(pActor, pModel, pMaterial, pOrder_table, pStyle, BrOnScreenCheck(&pModel->bounds), 0);
#else
    __asm {
            push ecx
            mov eax, dword ptr [gAFE_color_cycle]
            push ebx
            push ebp
            xor ebp, ebp
            push esi
            cmp eax, ebp
            push edi
            je L004f928b
            call GetTotalTime
            mov esi, dword ptr [esp + 0x1c]
            cmp esi, ebp
            je L004f9278
            mov ecx, dword ptr [esi + 0x4c]
            mov dword ptr [esp + 0x10], ebp
            sar eax, 1
            cmp word ptr [ecx + 8], bp
            jbe L004f9278
    L004f91b8:
            mov ecx, dword ptr [ecx + 0x18]
            xor edi, edi
            cmp word ptr [ebp + ecx + 0x1e], di
            jbe L004f925b
    L004f91c8:
            mov edx, dword ptr [ebp + ecx + 0x18]
            xor ecx, ecx
            mov cx, word ptr [edx + edi*2]
            mov edx, dword ptr [esi + 8]
            lea ecx, [ecx + ecx*4]
            lea edx, [edx + ecx*8]
            test edx, edx
            je L004f9245
            mov esi, eax
            and esi, 0x3fff
            cmp esi, 0x100
            jge L004f91f3
            mov ecx, esi
            jmp L004f91fa
    L004f91f3:
            mov ecx, 0x3ff
            sub ecx, esi
    L004f91fa:
            mov byte ptr [edx + 0x15], cl
            lea ecx, [eax + 0x100]
            and ecx, 0x3ff
            cmp ecx, 0x100
            jge L004f9215
            mov ebx, ecx
            jmp L004f921c
    L004f9215:
            mov ebx, 0x3ff
            sub ebx, ecx
    L004f921c:
            lea ecx, [eax - 0x200]
            mov byte ptr [edx + 0x16], bl
            and ecx, 0x3ff
            cmp ecx, 0x100
            jge L004f9237
            mov ebx, ecx
            jmp L004f923e
    L004f9237:
            mov ebx, 0x3ff
            sub ebx, ecx
    L004f923e:
            mov esi, dword ptr [esp + 0x1c]
            mov byte ptr [edx + 0x17], bl
    L004f9245:
            mov ecx, dword ptr [esi + 0x4c]
            xor edx, edx
            inc edi
            mov ecx, dword ptr [ecx + 0x18]
            mov dx, word ptr [ebp + ecx + 0x1e]
            cmp edi, edx
            jl L004f91c8
    L004f925b:
            mov ecx, dword ptr [esi + 0x4c]
            mov edx, dword ptr [esp + 0x10]
            xor edi, edi
            inc edx
            mov di, word ptr [ecx + 8]
            add ebp, 0x24
            cmp edx, edi
            mov dword ptr [esp + 0x10], edx
            jl L004f91b8
    L004f9278:
            test byte ptr [esi + 0x20], 0x80
            je L004f928f
            push 0x40
            push esi
            call BrModelUpdate
            add esp, 8
            jmp L004f928f
    L004f928b:
            mov esi, dword ptr [esp + 0x1c]
    L004f928f:
            lea eax, [esi + 0x34]
            push 0
            push eax
            call BrOnScreenCheck
            mov ecx, dword ptr [esp + 0x30]
            mov edx, dword ptr [esp + 0x2c]
            add esp, 4
            push eax
            mov eax, dword ptr [esp + 0x28]
            push ecx
            mov ecx, dword ptr [esp + 0x24]
            push edx
            push eax
            push esi
            push ecx
            call BrZsModelRender
            add esp, 0x1c
            pop edi
            pop esi
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
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            push ebx
            push ebp
            mov ebp, ecx
            push esi
            push edi
            xor edi, edi
            mov eax, dword ptr [ebp + 0xe28]
            mov esi, dword ptr [ebp + 0x18d4]
            mov ebx, 0xffdf
            and word ptr [eax + 0x20], bx
            mov dword ptr [eax + 0x24], edi
            mov eax, dword ptr [ebp + 0xe0c]
            mov ecx, dword ptr [eax + 0x60]
            mov eax, dword ptr [ecx + 4]
            cmp eax, edi
            je L004f9307
            and word ptr [eax + 0x20], bx
            mov dword ptr [eax + 0x24], edi
    L004f9307:
            mov ecx, dword ptr [ebp + 0xe0c]
            push offset FUN_004f9390
            mov edx, offset ForEveryActorModelNoGrooves
            call DRActorEnumRecurse
            mov eax, dword ptr [esi + 0x270]
            xor ecx, ecx
            cmp eax, edi
            jle L004f9348
            lea edx, [esi + 0x274]
    L004f932e:
            mov eax, dword ptr [edx]
            add edx, 0x14
            mov eax, dword ptr [eax + 0x18]
            and word ptr [eax + 0x20], bx
            mov dword ptr [eax + 0x24], edi
            mov eax, dword ptr [esi + 0x270]
            inc ecx
            cmp ecx, eax
            jl L004f932e
    L004f9348:
            mov eax, dword ptr [esi + 0x2b0]
            xor ecx, ecx
            cmp eax, edi
            jle L004f9374
            lea edx, [esi + 0x2b4]
    L004f935a:
            mov eax, dword ptr [edx]
            add edx, 0x14
            mov eax, dword ptr [eax + 0x18]
            and word ptr [eax + 0x20], bx
            mov dword ptr [eax + 0x24], edi
            mov eax, dword ptr [esi + 0x2b0]
            inc ecx
            cmp ecx, eax
            jl L004f935a
    L004f9374:
            push 1
            mov edx, offset FUN_004f93b0
            mov ecx, ebp
            call ForEveryCarMaterial
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
    }
#endif
}

// UNLABELED: CARMA2_HW 0x004f95e0
int C2_HOOK_FASTCALL SetModelAFEUpdateCB(br_model* pModel) {

    pModel->flags |= BR_MODF_CUSTOM;
    pModel->custom = AFEModelUpdate;
    return 1;
}

// UNLABELED: CARMA2_HW 0x004f9600
int C2_HOOK_FASTCALL SetMaterialPrelitCB(br_material* pMaterial) {

    if (pMaterial->flags & BR_MATF_PRELIT) {
        return 0;
    }
    pMaterial->flags |= BR_MATF_PRELIT;
    BrMaterialUpdate(pMaterial, BR_MATU_RENDERING | BR_MATU_LIGHTING);
    return 1;
}

// UNLABELED: CARMA2_HW 0x004fec30
intptr_t C2_HOOK_CDECL ForEveryActorModelNoGrooves(br_actor* pActor, void* pContext) {
    tUser_crush_data* user_crush_data;
    tModelUpdate_cbfn* callback;

    callback = pContext;
    if (pActor->type != BR_ACTOR_MODEL) {
        return 0;
    }
    if (pActor->model == NULL) {
        return 0;
    }
    user_crush_data = pActor->user;
    if (user_crush_data == NULL) {
        return 0;
    }
    if (user_crush_data->groove != NULL) {
        return 0;
    }
    callback(pActor->model);
    return 0;
}

// FUNCTION: CARMA2_HW 0x004fe360
void C2_NAKED C2_HOOK_FASTCALL DisposeKevStuffCar(tCar_spec* pCar) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;
    int j;
    tCar_crush_spec* spec;
    br_actor* actor;
    br_model* model;
    tUser_crush_data* user;
    tCar_crush_buffer_entry* crush;
    br_material* solid_mat;

    if (!gAFE) {
        return;
    }
    spec = pCar->car_crush_spec;
    DRActorEnumRecurse(pCar->car_model_actor, ActorFunks, (void*)MasterDisableFunkotronic);
    for (i = 0; i < spec->field_0x270; i++) {
        actor = spec->field_0x274[i].field_0x0;
        user = (tUser_crush_data*)actor->user;
        if (user == NULL) {
            continue;
        }
        crush = user->crush_data;
        if (crush == NULL) {
            continue;
        }
        if (crush->smashables == NULL) {
            continue;
        }
        for (j = 0; j < crush->count_smashables; j++) {
            int funk_index = crush->smashables[j].funk;
            if (funk_index >= 0) {
                MasterDisableFunkotronic(funk_index);
            }
        }
    }
    for (i = 0; i < spec->field_0x2b0; i++) {
        actor = spec->field_0x2b4[i].field_0x0;
        user = (tUser_crush_data*)actor->user;
        if (user == NULL) {
            continue;
        }
        crush = user->crush_data;
        if (crush == NULL) {
            continue;
        }
        if (crush->smashables == NULL) {
            continue;
        }
        for (j = 0; j < crush->count_smashables; j++) {
            int funk_index = crush->smashables[j].funk;
            if (funk_index >= 0) {
                MasterDisableFunkotronic(funk_index);
            }
        }
    }
    solid_mat = BrMaterialFind("solidgrn");
    if (solid_mat == NULL) {
        return;
    }
    if (solid_mat->colour_map == NULL) {
        return;
    }
    gRecalc_mass_car = pCar;
    pCar->field_0x1960 = solid_mat->colour_map;
    ForEveryCarMaterial(pCar, SetMaterialPrelitCB, 1);
    pCar->shell_model->custom = AFEModelUpdate;
    pCar->shell_model->flags |= BR_MODF_CUSTOM;
    user = (tUser_crush_data*)pCar->car_model_actor->user;
    model = user->models[1];
    if (model != NULL) {
        model->flags |= BR_MODF_CUSTOM;
        model->custom = AFEModelUpdate;
    }
    DRActorEnumRecurse(pCar->car_model_actor, ForEveryActorModelNoGrooves, (void*)SetModelAFEUpdateCB);
    for (i = 0; i < spec->field_0x270; i++) {
        model = spec->field_0x274[i].field_0x0->model;
        model->flags |= BR_MODF_CUSTOM;
        model->custom = AFEModelUpdate;
    }
    for (i = 0; i < spec->field_0x2b0; i++) {
        model = spec->field_0x2b4[i].field_0x0->model;
        model->flags |= BR_MODF_CUSTOM;
        model->custom = AFEModelUpdate;
    }
#else
    __asm {
            sub esp, 0xc
            mov eax, dword ptr [gAFE]
            push ebx
            push ebp
            mov ebx, ecx
            push esi
            push edi
            test eax, eax
            mov dword ptr [esp + 0x14], ebx
            je L004fe567
            mov edi, dword ptr [ebx + 0x18d4]
            mov ecx, dword ptr [ebx + 0xe0c]
            push offset MasterDisableFunkotronic
            mov edx, offset ActorFunks
            mov dword ptr [esp + 0x1c], edi
            call DRActorEnumRecurse
            mov eax, dword ptr [edi + 0x270]
            test eax, eax
            mov dword ptr [esp + 0x10], 0
            jle L004fe40f
            lea ebp, [edi + 0x274]
    L004fe3b1:
            mov eax, dword ptr [ebp]
            mov eax, dword ptr [eax + 0x60]
            test eax, eax
            je L004fe3f5
            mov esi, dword ptr [eax + 8]
            test esi, esi
            je L004fe3f5
            mov eax, dword ptr [esi + 0x3c]
            test eax, eax
            je L004fe3f5
            xor ebx, ebx
            cmp word ptr [esi + 0x38], bx
            jbe L004fe3f5
            xor edi, edi
    L004fe3d3:
            mov eax, dword ptr [esi + 0x3c]
            mov ecx, dword ptr [edi + eax + 0x40]
            test ecx, ecx
            jl L004fe3e3
            call MasterDisableFunkotronic
    L004fe3e3:
            xor ecx, ecx
            inc ebx
            mov cx, word ptr [esi + 0x38]
            add edi, 0x7c
            cmp ebx, ecx
            jl L004fe3d3
            mov edi, dword ptr [esp + 0x18]
    L004fe3f5:
            mov eax, dword ptr [esp + 0x10]
            mov ecx, dword ptr [edi + 0x270]
            inc eax
            add ebp, 0x14
            cmp eax, ecx
            mov dword ptr [esp + 0x10], eax
            jl L004fe3b1
            mov ebx, dword ptr [esp + 0x14]
    L004fe40f:
            mov eax, dword ptr [edi + 0x2b0]
            mov dword ptr [esp + 0x10], 0
            test eax, eax
            jle L004fe485
            lea ebp, [edi + 0x2b4]
    L004fe427:
            mov eax, dword ptr [ebp]
            mov eax, dword ptr [eax + 0x60]
            test eax, eax
            je L004fe46b
            mov esi, dword ptr [eax + 8]
            test esi, esi
            je L004fe46b
            mov eax, dword ptr [esi + 0x3c]
            test eax, eax
            je L004fe46b
            xor ebx, ebx
            cmp word ptr [esi + 0x38], bx
            jbe L004fe46b
            xor edi, edi
    L004fe449:
            mov edx, dword ptr [esi + 0x3c]
            mov ecx, dword ptr [edi + edx + 0x40]
            test ecx, ecx
            jl L004fe459
            call MasterDisableFunkotronic
    L004fe459:
            xor eax, eax
            inc ebx
            mov ax, word ptr [esi + 0x38]
            add edi, 0x7c
            cmp ebx, eax
            jl L004fe449
            mov edi, dword ptr [esp + 0x18]
    L004fe46b:
            mov eax, dword ptr [esp + 0x10]
            mov ecx, dword ptr [edi + 0x2b0]
            inc eax
            add ebp, 0x14
            cmp eax, ecx
            mov dword ptr [esp + 0x10], eax
            jl L004fe427
            mov ebx, dword ptr [esp + 0x14]
    L004fe485:
            push offset gDAT_00660148+0x68
            call BrMaterialFind
            add esp, 4
            test eax, eax
            je L004fe567
            mov ecx, dword ptr [eax + 0x40]
            test ecx, ecx
            je L004fe567
            mov dword ptr [gRecalc_mass_car], ebx
            mov ecx, dword ptr [eax + 0x40]
            mov dword ptr [ebx + 0x1960], ecx
            push 1
            mov edx, offset SetMaterialPrelitCB
            mov ecx, ebx
            call ForEveryCarMaterial
            mov esi, dword ptr [ebx + 0x18d4]
            mov eax, dword ptr [ebx + 0xe28]
            mov ecx, dword ptr [esp + 0x14]
            mov ebx, offset AFEModelUpdate
            mov edi, 0x20
            mov dword ptr [eax + 0x24], ebx
            or word ptr [eax + 0x20], di
            mov edx, dword ptr [ecx + 0xe0c]
            mov eax, dword ptr [edx + 0x60]
            mov eax, dword ptr [eax + 4]
            test eax, eax
            je L004fe4fa
            or word ptr [eax + 0x20], di
            mov dword ptr [eax + 0x24], ebx
    L004fe4fa:
            mov ecx, dword ptr [ecx + 0xe0c]
            push offset SetModelAFEUpdateCB
            mov edx, offset ForEveryActorModelNoGrooves
            call DRActorEnumRecurse
            mov eax, dword ptr [esi + 0x270]
            xor ecx, ecx
            test eax, eax
            jle L004fe53b
            lea edx, [esi + 0x274]
    L004fe521:
            mov eax, dword ptr [edx]
            add edx, 0x14
            mov eax, dword ptr [eax + 0x18]
            or word ptr [eax + 0x20], di
            mov dword ptr [eax + 0x24], ebx
            mov eax, dword ptr [esi + 0x270]
            inc ecx
            cmp ecx, eax
            jl L004fe521
    L004fe53b:
            mov eax, dword ptr [esi + 0x2b0]
            xor ecx, ecx
            test eax, eax
            jle L004fe567
            lea edx, [esi + 0x2b4]
    L004fe54d:
            mov eax, dword ptr [edx]
            add edx, 0x14
            mov eax, dword ptr [eax + 0x18]
            or word ptr [eax + 0x20], di
            mov dword ptr [eax + 0x24], ebx
            mov eax, dword ptr [esi + 0x2b0]
            inc ecx
            cmp ecx, eax
            jl L004fe54d
    L004fe567:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0xc
            ret
            nop
    }
#endif
}

// UNLABELED: CARMA2_HW 0x004f9390
int C2_HOOK_FASTCALL FUN_004f9390(br_model* pModel) {

    pModel->flags &= ~BR_MODF_CUSTOM;
    pModel->custom = NULL;
    return 1;
}

// UNLABELED: CARMA2_HW 0x004fec30
int C2_HOOK_CDECL FUN_004fec30(br_actor* pActor, void* pCallback) {

    if (pActor->type != 1) {
        return 0;
    }
    if (pActor->model == NULL) {
        return 0;
    }
    if (pActor->user == NULL) {
        return 0;
    }
    if (((tUser_crush_data*)pActor->user)->groove == NULL) {
        return 0;
    }
    ((int (C2_HOOK_FASTCALL*)(br_model*))pCallback)(pActor->model);
    return 0;
}

// UNLABELED: CARMA2_HW 0x004f93b0
int C2_HOOK_FASTCALL FUN_004f93b0(br_material* pMaterial) {

    if ((pMaterial->flags & 2) == 0) {
        return 0;
    }
    pMaterial->flags &= ~2u;
    BrMaterialUpdate(pMaterial, 6);
    return 1;
}

// FUNCTION: CARMA2_HW 0x004fe570
void C2_NAKED C2_HOOK_FASTCALL StopCarBeingIt(tCar_spec* pCar) {
#ifndef CARPOCALYPSE2_MATCHING

    if (gAFE == 0) {
        return;
    }
    pCar->shell_model->flags &= ~BR_MODF_CUSTOM;
    pCar->shell_model->custom = NULL;
    {
        tUser_crush_data* user = (tUser_crush_data*)pCar->car_model_actor->user;

        if (user->models[1] != NULL) {
            user->models[1]->flags &= ~BR_MODF_CUSTOM;
            user->models[1]->custom = NULL;
        }
    }
    DRActorEnumRecurse(pCar->car_model_actor, FUN_004fec30, (void*)FUN_004f9390);
    {
        tCar_crush_spec* crush = pCar->car_crush_spec;
        int j;

        for (j = 0; j < crush->field_0x270; j++) {
            br_model* model = crush->field_0x274[j].field_0x0->model;

            model->flags &= ~BR_MODF_CUSTOM;
            model->custom = NULL;
        }
        for (j = 0; j < crush->field_0x2b0; j++) {
            br_model* model = crush->field_0x2b4[j].field_0x0->model;

            model->flags &= ~BR_MODF_CUSTOM;
            model->custom = NULL;
        }
    }
    ForEveryCarMaterial(pCar, FUN_004f93b0, 1);
#else
    __asm {
            mov eax, dword ptr [gAFE]
            push ebp
            push edi
            xor edi, edi
            cmp eax, edi
            mov ebp, ecx
            je L004fe631
            mov eax, dword ptr [ebp + 0xe28]
            push esi
            mov esi, dword ptr [ebp + 0x18d4]
            push ebx
            mov ebx, 0xffdf
            mov dword ptr [eax + 0x24], edi
            and word ptr [eax + 0x20], bx
            mov eax, dword ptr [ebp + 0xe0c]
            mov ecx, dword ptr [eax + 0x60]
            mov eax, dword ptr [ecx + 4]
            cmp eax, edi
            je L004fe5b4
            and word ptr [eax + 0x20], bx
            mov dword ptr [eax + 0x24], edi
    L004fe5b4:
            mov ecx, dword ptr [ebp + 0xe0c]
            push offset FUN_004f9390
            mov edx, offset ForEveryActorModelNoGrooves
            call DRActorEnumRecurse
            mov eax, dword ptr [esi + 0x270]
            xor ecx, ecx
            cmp eax, edi
            jle L004fe5f5
            lea edx, [esi + 0x274]
    L004fe5db:
            mov eax, dword ptr [edx]
            add edx, 0x14
            mov eax, dword ptr [eax + 0x18]
            and word ptr [eax + 0x20], bx
            mov dword ptr [eax + 0x24], edi
            mov eax, dword ptr [esi + 0x270]
            inc ecx
            cmp ecx, eax
            jl L004fe5db
    L004fe5f5:
            mov eax, dword ptr [esi + 0x2b0]
            xor ecx, ecx
            cmp eax, edi
            jle L004fe621
            lea edx, [esi + 0x2b4]
    L004fe607:
            mov eax, dword ptr [edx]
            add edx, 0x14
            mov eax, dword ptr [eax + 0x18]
            and word ptr [eax + 0x20], bx
            mov dword ptr [eax + 0x24], edi
            mov eax, dword ptr [esi + 0x2b0]
            inc ecx
            cmp ecx, eax
            jl L004fe607
    L004fe621:
            push 1
            mov edx, offset FUN_004f93b0
            mov ecx, ebp
            call ForEveryCarMaterial
            pop ebx
            pop esi
    L004fe631:
            pop edi
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
    }
#endif
}

void C2_HOOK_FASTCALL MungeSplash(tU32 pTime) {

    if (gNum_splash_types == 0) {
        return;
    }
    if (gAction_replay_mode && ARGetReplayRate() != 0.f) {
        tVehicle_type type;
        int i;

        for (type = eVehicle_net_player; type <= eVehicle_rozzer; type++) {

            for (i = 0; i < (type == eVehicle_self ? 1 : GetCarCount(type)); i++) {
                tCar_spec* car;

                if (type == eVehicle_self) {
                    car = &gProgram_state.current_car;
                } else {
                    car = GetCarSpec(type, i);
                }
                if (car->collision_info->water_d != 10000.f && !(car != NULL && car->driver == eDriver_local_human)) {
                    CreateSplash(car->collision_info, pTime);
                }
            }
        }
        if (gProgram_state.current_car.collision_info->water_d != 10000.f) {
            CreateSplash(gProgram_state.current_car.collision_info, pTime);
        }
        for (i = 0; i < CARPOCALYPSE2_ASIZE(gReplay_splashes); i++) {
            tReplay_splash *replay_splash = &gReplay_splashes[i];

            if (gReplay_splash_flags & (1 << i)) {
                switch (replay_splash->type) {
                case 1:
                    {
                        const char* ident = replay_splash->object.actor->identifier;
                        tPipe_chunk_data *chunk_non_car = FindNextChunk(ePipe_chunk_non_car, CHARS4_TO_INT(ident[4], ident[5], ident[6], ident[7]));
                        if (chunk_non_car != NULL) {
                            br_vector3 vel;
                            br_vector3 rot;

                            GetVelocitiesFromMatrices(&vel, &rot, &replay_splash->object.actor->t.t.mat, &chunk_non_car->data.non_car.matrix, pTime);
                            GeneralCreateSplash(NULL, &vel, &rot, &replay_splash->normal, replay_splash->water_density, &replay_splash->bb, &replay_splash->object.actor->t.t.mat, pTime);
                        }
                    }
                    break;
                case 2:
                    {
                        int id = (replay_splash->object.ped - gPedestrian_array) | (replay_splash->object.ped->field_0x06 << 16);
                        br_actor* actor = GetCharacterActorPtr(replay_splash->object.ped->character);
                        tPipe_chunk_data *chunk_ped_physics = FindNextChunk(ePipe_chunk_ped_physics, id);
                        if (chunk_ped_physics != NULL) {
                            br_vector3 vel;
                            br_vector3 rot;

                            GetVelocitiesFromMatrices(&vel, &rot, &actor->t.t.mat, &chunk_ped_physics->data.ped_physics.matrix, pTime);
                            GeneralCreateSplash(NULL, &vel, &rot, &replay_splash->normal, replay_splash->water_density, &replay_splash->bb, &replay_splash->object.actor->t.t.mat, pTime);
                        }
                    }
                    break;
                case 255:
                    {
                        tPipe_chunk_data *chunk_ped_phil_object = FindNextChunk(ePipe_chunk_phil_object, (uintptr_t)replay_splash->object.phil_object);
                        if (chunk_ped_phil_object != NULL) {
                            br_vector3 vel;
                            br_vector3 rot;

                            GetVelocitiesFromMatrices(&vel, &rot, &replay_splash->object.actor->t.t.mat, &chunk_ped_phil_object->data.phil_object.matrix, pTime);
                            GeneralCreateSplash(NULL, &vel, &rot, &replay_splash->normal, replay_splash->water_density, &replay_splash->bb, &replay_splash->object.actor->t.t.mat, pTime);
                        }
                    }
                    break;
                }
            }
        }
    } else if (!gAction_replay_mode) {
        tPhysics_object* object;

        for (object = PHILGetFirstObject(); object != NULL; object = PHILGetNextObject(object)) {

            if (object->water_d != 10000.f && object->last_special_volume != NULL && object->last_special_volume->gravity_multiplier < 1.f) {
                if (object != NULL && object->owner != NULL && object->flags_0x238 == 1 && object->owner != NULL && ((tCar_spec*)object->owner)->driver == eDriver_local_human) {
                    CreateSplash(object, 100);
                } else {
                    CreateSplash(object, pTime);
                }
            }
        }
    }
    if (gSplash_flags) {
        int i;

        for (i = 0; i < CARPOCALYPSE2_ASIZE(gSplash); i++) {
            tSplash *splash = &gSplash[i];

            if (!(gSplash_flags & (1 << i))) {
                continue;
            }
            if (splash->just_done || (gAction_replay_mode && ARGetReplayRate() == 0.f)) {
                br_scalar dt;

                dt = splash->size * splash->scale_x;
                BrVector3Scale((br_vector3*)splash->actor->t.t.mat.m[0], (br_vector3*)gCamera_to_world.m[0], dt);
                BrVector3Scale((br_vector3*)splash->actor->t.t.mat.m[1], (br_vector3*)gCamera_to_world.m[1], splash->size);
                BrVector3Scale((br_vector3*)splash->actor->t.t.mat.m[2], (br_vector3*)gCamera_to_world.m[2], splash->size);
                if (gProgram_state.cockpit_on) {
                    br_scalar ts;

                    ts = sqrtf(CARPOCALYPSE2_SQR(gCamera_to_world.m[0][2]) + CARPOCALYPSE2_SQR(gCamera_to_world.m[0][0]));
                    DRMatrix34PreRotateZ(&splash->actor->t.t.mat, -FastScalarArcTan2Angle(gCamera_to_world.m[0][1], ts));
                }
                splash->just_done = 0;
            } else {
                gSplash_flags &= ~(1 << i);
                if (splash->actor->parent != NULL) {
                    BrActorRemove(splash->actor);
                }
            }
        }
    }
}

void C2_HOOK_FASTCALL ReplayShrapnel(tU32 pTime) {
    int i;
    br_matrix34* mat;

    for (i = 0; i < CARPOCALYPSE2_ASIZE(gShrapnel); i++) {
        mat = &gShrapnel[i].actor->t.t.mat;
        if (gShrapnel_flags & (1 << i)) {
            gShrapnel[i].age += (tU32)(ARGetReplayRate() * (float)pTime);
            DrMatrix34Rotate(mat, gShrapnel[i].age * BR_ANGLE_DEG(1), &gShrapnel[i].axis);
            BrMatrix34PreShearX(mat, gShrapnel[i].shear1, gShrapnel[i].shear2);
        }
    }
}

void C2_HOOK_FASTCALL KillShrapnel(int i) {

    if (gShrapnel[i].actor->parent != NULL) {
        BrActorRemove(gShrapnel[i].actor);
    }
    gShrapnel_flags &= ~(1 << i);
}

// FUNCTION: CARMA2_HW 0x004f9790
void C2_NAKED C2_HOOK_FASTCALL MungeShrapnel(tU32 pTime) {
#ifndef CARPOCALYPSE2_MATCHING

    MungeSmokeColumn(pTime);
    MungeSplash(pTime);

    if (gAFE) {
        tU32 now = GetTotalTime();
        if (now - gNext_AFE_color_cycle > 100) {
            gAFE_color_cycle = 1;
            gNext_AFE_color_cycle = now;
        } else {
            gAFE_color_cycle = 0;
        }
    }
    if (gAction_replay_mode) {
        ReplayShrapnel(pTime);
    } else {
        int i;

        ARStartPipingSession(ePipe_chunk_shrapnel);
        for (i = 0; i < CARPOCALYPSE2_ASIZE(gShrapnel); i++) {
            br_matrix34* mat;
            br_scalar ts;

            mat = &gShrapnel[i].actor->t.t.mat;
            if (!(gShrapnel_flags & (1 << i))) {
                continue;
            }
            if (gShrapnel[i].age == -1) {
                KillShrapnel(i);
            } else {
                br_vector3 disp;

                if (gShrapnel[i].time_sync != 0) {
                    BrVector3Scale(&disp, &gShrapnel[i].v, (float)gShrapnel[i].time_sync / 1000.f);
                    gShrapnel[i].time_sync = 0;
                } else {
                    BrVector3Scale(&disp, &gShrapnel[i].v, pTime / 1000.0f);
                    gShrapnel[i].age += pTime;
                }
                BrVector3Accumulate((br_vector3*)mat->m[3], &disp);
                gShrapnel[i].v.v[1] -= (float)(10 * pTime) / 1000.f / WORLD_SCALE; /* Fall acceleration */
                DrMatrix34Rotate(mat, BR_ANGLE_DEG(gShrapnel[i].age), &gShrapnel[i].axis);
                BrMatrix34PreShearX(mat, gShrapnel[i].shear1, gShrapnel[i].shear2);
                ts = 1.f - BrVector3Length(&gShrapnel[i].v) / 1.4f * (float)pTime / 1000.f;
                if (ts < .1f) {
                    ts = .1f;
                }
                BrVector3Scale(&gShrapnel[i].v, &gShrapnel[i].v, ts);
                AddShrapnelToPipingSession(i + ((gShrapnel[i].age > 1000 || pTime > gShrapnel[i].age) << 15), (br_vector3*)mat->m[3], gShrapnel[i].age - pTime, gShrapnel[i].actor->material);
                if (gShrapnel[i].age > 1000) {
                    gShrapnel[i].age = -1;
                }
            }
        }
        AREndPipingSession();
    }
#else
    __asm {
            sub esp, 0x34
            push ebx
            push ebp
            push esi
            mov ebx, ecx
            push edi
            mov dword ptr [esp + 0x10], ebx
            call MungeSmokeColumn
            mov eax, dword ptr [gNum_splash_types]
            test eax, eax
            je L004f9b82
            mov eax, dword ptr [gPed_676914]
            test eax, eax
            je L004f99b8
            call ARGetReplayRate
            fcomp dword ptr [gFLOAT_0058b008]
            fnstsw ax
            test ah, 0x40
            jne L004f99af
            mov esi, 1
            mov ebp, 0x461c4000
    L004f97dc:
            xor edi, edi
    L004f97de:
            test esi, esi
            jne L004f97e9
            mov eax, 1
            jmp L004f97f0
    L004f97e9:
            mov ecx, esi
            call GetCarCount
    L004f97f0:
            cmp edi, eax
            jge L004f9845
            test esi, esi
            jne L004f97ff
            mov eax, offset gProgram_state+0xac
            jmp L004f9808
    L004f97ff:
            mov edx, edi
            mov ecx, esi
            call GetCarSpec
    L004f9808:
            mov ecx, dword ptr [eax + 8]
            cmp dword ptr [ecx + 0x250], ebp
            je L004f9842
            test eax, eax
            je L004f981d
            cmp dword ptr [eax + 0xc], 8
            je L004f9842
    L004f981d:
            mov eax, dword ptr [ecx]
            push ebx
            add eax, 0x2c
            lea edx, [ecx + 0x24]
            push eax
            mov eax, dword ptr [ecx + 0x250]
            push edx
            push eax
            lea edx, [ecx + 0x244]
            lea eax, [ecx + 0x74]
            push edx
            push eax
            lea edx, [ecx + 0x68]
            call GeneralCreateSplash
    L004f9842:
            inc edi
            jmp L004f97de
    L004f9845:
            inc esi
            cmp esi, 3
            jle L004f97dc
            mov ecx, dword ptr [gProgram_state+0xb4]
            cmp dword ptr [ecx + 0x250], ebp
            je L004f9860
            mov edx, ebx
            call CreateSplash
    L004f9860:
            xor ecx, ecx
            mov esi, offset gReplay_splashes
            mov dword ptr [esp + 0x14], ecx
    L004f986b:
            mov eax, dword ptr [gReplay_splash_flags]
            mov edx, 1
            shl edx, cl
            test eax, edx
            je L004f9992
            mov eax, dword ptr [esi + 4]
            dec eax
            je L004f9925
            dec eax
            je L004f98db
            sub eax, 0xfd
            jne L004f9992
            mov edx, dword ptr [esi]
            mov ecx, 0x30
            mov edi, dword ptr [edx]
            call FindNextChunk
            test eax, eax
            je L004f9992
            add eax, 4
            add edi, 0x2c
            push ebx
            push eax
            push edi
            lea edx, [esp + 0x44]
            lea ecx, [esp + 0x38]
            call GetVelocitiesFromMatrices
            mov ecx, dword ptr [esi + 8]
            lea eax, [esi + 0x18]
            push ebx
            push edi
            push eax
            lea edx, [esi + 0xc]
            push ecx
            lea eax, [esp + 0x48]
            push edx
            push eax
            jmp L004f9987
    L004f98db:
            mov ecx, dword ptr [esi]
            mov ebp, dword ptr [ecx + 0xe4]
            call GetCharacterActorPtr
            mov edx, dword ptr [gPedestrian_array]
            mov ecx, ebp
            mov edi, eax
            sub ecx, edx
            mov eax, 0x30c30c31
            add edi, 0x2c
            imul ecx
            movsx eax, word ptr [ebp + 6]
            sar edx, 4
            mov ecx, edx
            shr ecx, 0x1f
            add edx, ecx
            mov ecx, 0x2b
            shl eax, 0x10
            or edx, eax
            call FindNextChunk
            test eax, eax
            je L004f9992
            push ebx
            add eax, 4
            jmp L004f9965
    L004f9925:
            mov edi, dword ptr [esi]
            mov eax, dword ptr [edi + 0x14]
            movsx ecx, byte ptr [eax + 4]
            lea edx, [ecx + ecx*4]
            movsx ecx, byte ptr [eax + 5]
            lea ecx, [ecx + edx*2]
            lea edx, [ecx + ecx*4]
            movsx ecx, byte ptr [eax + 6]
            movsx eax, byte ptr [eax + 7]
            lea ecx, [ecx + edx*2]
            lea edx, [ecx + ecx*4]
            mov ecx, 0xe
            lea edx, [eax + edx*2 - 0xd050]
            call FindNextChunk
            test eax, eax
            je L004f9992
            add edi, 0x2c
            push ebx
            add eax, 8
    L004f9965:
            push eax
            push edi
            lea edx, [esp + 0x44]
            lea ecx, [esp + 0x38]
            call GetVelocitiesFromMatrices
            mov edx, dword ptr [esi + 8]
            lea ecx, [esi + 0x18]
            push ebx
            push edi
            push ecx
            lea eax, [esi + 0xc]
            push edx
            lea ecx, [esp + 0x48]
            push eax
            push ecx
    L004f9987:
            lea edx, [esp + 0x44]
            xor ecx, ecx
            call GeneralCreateSplash
    L004f9992:
            mov ecx, dword ptr [esp + 0x14]
            add esi, 0x30
            inc ecx
            cmp esi, offset gDust_rotate
            mov dword ptr [esp + 0x14], ecx
            jl L004f986b
            jmp L004f9a2f
    L004f99af:
            mov eax, dword ptr [gPed_676914]
            test eax, eax
            jne L004f9a2f
    L004f99b8:
            call PHILGetFirstObject
            mov esi, eax
            test esi, esi
            je L004f9a2f
            mov ebp, 0x461c4000
            mov edi, 8
    L004f99cd:
            cmp dword ptr [esi + 0x250], ebp
            je L004f9a22
            mov eax, dword ptr [esi + 0x258]
            test eax, eax
            je L004f9a22
            fld dword ptr [eax + 0x88]
            fcomp dword ptr [gFLOAT_0058afdc]
            fnstsw ax
            test ah, 1
            je L004f9a22
            test esi, esi
            je L004f9a19
            mov eax, dword ptr [esi + 0x23c]
            test eax, eax
            je L004f9a19
            cmp byte ptr [esi + 0x238], 1
            jne L004f9a19
            test eax, eax
            je L004f9a19
            mov ecx, dword ptr [eax + 0xc]
            mov edx, 0x64
            cmp ecx, edi
            je L004f9a1b
    L004f9a19:
            mov edx, ebx
    L004f9a1b:
            mov ecx, esi
            call CreateSplash
    L004f9a22:
            mov ecx, esi
            call PHILGetNextObject
            mov esi, eax
            test esi, esi
            jne L004f99cd
    L004f9a2f:
            mov eax, dword ptr [gSplash_flags]
            test eax, eax
            je L004f9b82
            xor ebp, ebp
            mov esi, offset gSplash
    L004f9a43:
            mov eax, dword ptr [gSplash_flags]
            mov edi, 1
            mov ecx, ebp
            shl edi, cl
            test eax, edi
            je L004f9b72
            mov eax, dword ptr [esi + 0x10]
            test eax, eax
            jne L004f9aa6
            mov eax, dword ptr [gPed_676914]
            test eax, eax
            je L004f9a7b
            call ARGetReplayRate
            fcomp dword ptr [gFLOAT_0058b008]
            fnstsw ax
            test ah, 0x40
            jne L004f9aa6
    L004f9a7b:
            mov edx, dword ptr [gSplash_flags]
            mov eax, dword ptr [esi]
            not edi
            and edx, edi
            mov dword ptr [gSplash_flags], edx
            mov ecx, dword ptr [eax + 0xc]
            test ecx, ecx
            je L004f9b72
            push eax
            call BrActorRemove
            add esp, 4
            jmp L004f9b72
    L004f9aa6:
            fld dword ptr [esi + 0x18]
            fmul dword ptr [esi + 0x14]
            mov edx, dword ptr [esi]
            fld st(0)
            fmul dword ptr [gCamera_to_world]
            fld st(1)
            fxch st(1)
            fstp dword ptr [edx + 0x2c]
            mov eax, dword ptr [esi]
            fmul dword ptr [gCamera_to_world+0x4]
            fstp dword ptr [eax + 0x30]
            mov ecx, dword ptr [esi]
            fmul dword ptr [gCamera_to_world+0x8]
            fstp dword ptr [ecx + 0x34]
            fld dword ptr [esi + 0x14]
            mov edx, dword ptr [esi]
            fmul dword ptr [gCamera_to_world+0xc]
            fstp dword ptr [edx + 0x38]
            fld dword ptr [esi + 0x14]
            mov eax, dword ptr [esi]
            fmul dword ptr [gCamera_to_world+0x10]
            fstp dword ptr [eax + 0x3c]
            fld dword ptr [esi + 0x14]
            mov ecx, dword ptr [esi]
            fmul dword ptr [gCamera_to_world+0x14]
            fstp dword ptr [ecx + 0x40]
            fld dword ptr [esi + 0x14]
            mov edx, dword ptr [esi]
            fmul dword ptr [gCamera_to_world+0x18]
            fstp dword ptr [edx + 0x44]
            fld dword ptr [esi + 0x14]
            mov eax, dword ptr [esi]
            fmul dword ptr [gCamera_to_world+0x1c]
            fstp dword ptr [eax + 0x48]
            fld dword ptr [esi + 0x14]
            mov ecx, dword ptr [esi]
            fmul dword ptr [gCamera_to_world+0x20]
            fstp dword ptr [ecx + 0x4c]
            mov eax, dword ptr [gProgram_state+0x4c]
            test eax, eax
            je L004f9b6b
            fld dword ptr [gCamera_to_world]
            fmul dword ptr [gCamera_to_world]
            fld dword ptr [gCamera_to_world+0x8]
            fmul dword ptr [gCamera_to_world+0x8]
            mov edx, dword ptr [gCamera_to_world+0x4]
            push ecx
            faddp st(1), st(0)
            fsqrt
            _emit 0xd9
        _emit 0x1c
        _emit 0x24
            push edx
            call FastScalarArcTan2Angle
            mov ecx, dword ptr [esi]
            mov dx, ax
            neg edx
            add ecx, 0x2c
            call DRMatrix34PreRotateZ
    L004f9b6b:
            mov dword ptr [esi + 0x10], 0
    L004f9b72:
            add esi, 0x1c
            inc ebp
            cmp esi, offset gFlame_map
            jl L004f9a43
    L004f9b82:
            mov eax, dword ptr [gAFE]
            test eax, eax
            je L004f9bba
            call GetTotalTime
            mov esi, dword ptr [gNext_AFE_color_cycle]
            mov ecx, eax
            sub ecx, esi
            cmp ecx, 0x64
            jbe L004f9bb0
            mov dword ptr [gAFE_color_cycle], 1
            mov dword ptr [gNext_AFE_color_cycle], eax
            jmp L004f9bba
    L004f9bb0:
            mov dword ptr [gAFE_color_cycle], 0
    L004f9bba:
            mov eax, dword ptr [gPed_676914]
            test eax, eax
            je L004f9c4a
            xor ebx, ebx
            mov esi, offset gShrapnel+0x14
    L004f9bce:
            mov edi, dword ptr [esi - 0x14]
            mov eax, dword ptr [gShrapnel_flags]
            mov edx, 1
            mov ecx, ebx
            lea ebp, [esi + 0xc]
            add edi, 0x2c
            shl edx, cl
            test eax, edx
            je L004f9c36
            call ARGetReplayRate
            mov eax, dword ptr [esp + 0x10]
            mov ecx, dword ptr [esi]
            mov dword ptr [esp + 0x14], eax
            xor eax, eax
            mov dword ptr [esp + 0x18], eax
            mov dword ptr [esp + 0x1c], ecx
            fimul dword ptr [esp + 0x14]
            mov dword ptr [esp + 0x20], eax
            fiadd dword ptr [esp + 0x1c]
            call _ftol
            mov dword ptr [esi], eax
            mov dx, word ptr [esi]
            imul dx, dx, 0xb6
            push ebp
            mov ecx, edi
            call DrMatrix34Rotate
            mov edx, dword ptr [esi + 8]
            mov eax, dword ptr [esi + 4]
            push edx
            push eax
            push edi
            call BrMatrix34PreShearX
            add esp, 0xc
    L004f9c36:
            add esi, 0x2c
            inc ebx
            cmp esi, offset gSmoke_column+0x14
            jl L004f9bce
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x34
            ret
    L004f9c4a:
            mov ecx, 0xb
            call ARStartPipingSession
            xor ebx, ebx
            mov esi, offset gShrapnel+0x10
    L004f9c5b:
            mov eax, dword ptr [esi - 0x10]
            mov ebp, 1
            mov ecx, ebx
            shl ebp, cl
            mov ecx, dword ptr [gShrapnel_flags]
            lea edi, [eax + 0x2c]
            test ecx, ebp
            je L004f9e5b
            mov ecx, dword ptr [esi + 4]
            cmp ecx, -1
            jne L004f9ca3
            mov ecx, dword ptr [eax + 0xc]
            test ecx, ecx
            je L004f9c90
            push eax
            call BrActorRemove
            add esp, 4
    L004f9c90:
            mov eax, dword ptr [gShrapnel_flags]
            not ebp
            and eax, ebp
            mov dword ptr [gShrapnel_flags], eax
            jmp L004f9e5b
    L004f9ca3:
            mov eax, dword ptr [esi]
            xor edx, edx
            cmp eax, edx
            je L004f9ce0
            fld dword ptr [esi - 0xc]
            mov dword ptr [esp + 0x1c], eax
            mov dword ptr [esp + 0x20], edx
            fild qword ptr [esp + 0x1c]
            fmul dword ptr [gFLOAT_0058afdc+0xc]
            fld dword ptr [esi - 8]
            fld dword ptr [esi - 4]
            fxch st(3)
            fmul st(0), st(2)
            fxch st(1)
            fmul st(0), st(2)
            fxch st(3)
            fmul st(0), st(2)
            fxch st(1)
            fstp st(2)
            fxch st(1)
            fxch st(2)
            fxch st(1)
            mov dword ptr [esi], edx
            jmp L004f9d1a
    L004f9ce0:
            mov eax, dword ptr [esp + 0x10]
            mov dword ptr [esp + 0x28], edx
            fld dword ptr [esi - 0xc]
            mov dword ptr [esp + 0x24], eax
            add eax, ecx
            fild qword ptr [esp + 0x24]
            fmul dword ptr [gFLOAT_0058afdc+0xc]
            fld dword ptr [esi - 8]
            fld dword ptr [esi - 4]
            fxch st(3)
            fmul st(0), st(2)
            fxch st(1)
            fmul st(0), st(2)
            fxch st(3)
            fmul st(0), st(2)
            fxch st(1)
            fstp st(2)
            fxch st(1)
            fxch st(2)
            fxch st(1)
            mov dword ptr [esi + 4], eax
    L004f9d1a:
            mov eax, dword ptr [esp + 0x10]
            lea ebp, [edi + 0x24]
            fxch st(2)
            _emit 0xd8
        _emit 0x45
        _emit 0x00
            lea ecx, [eax + eax*4]
            mov dword ptr [esp + 0x30], 0
            shl ecx, 1
            mov dword ptr [esp + 0x2c], ecx
            lea edx, [esi + 0x10]
            fild qword ptr [esp + 0x2c]
            fxch st(2)
            fadd dword ptr [edi + 0x28]
            fxch st(3)
            fadd dword ptr [edi + 0x2c]
            fxch st(2)
            fmul dword ptr [gDOUBLE_0058b048+0x8]
            fxch st(1)
            _emit 0xd9
        _emit 0x5d
        _emit 0x00
            fxch st(2)
            fstp dword ptr [edi + 0x28]
            push edx
            mov ecx, edi
            fstp dword ptr [edi + 0x2c]
            fsubr dword ptr [esi - 8]
            fstp dword ptr [esi - 8]
            mov dx, word ptr [esi + 4]
            imul dx, dx, 0xb6
            call DrMatrix34Rotate
            mov eax, dword ptr [esi + 0xc]
            mov ecx, dword ptr [esi + 8]
            push eax
            push ecx
            push edi
            call BrMatrix34PreShearX
            fld dword ptr [esi - 0xc]
            fld dword ptr [esi - 8]
            fld dword ptr [esi - 4]
            fxch st(2)
            fst dword ptr [esp + 0x20]
            fld st(1)
            fxch st(1)
            fmul dword ptr [esp + 0x20]
            fxch st(1)
            fmul st(0), st(2)
            fxch st(3)
            fst dword ptr [esp + 0x20]
            fmul dword ptr [esp + 0x20]
            fxch st(3)
            faddp st(1), st(0)
            mov edx, dword ptr [esp + 0x1c]
            mov dword ptr [esp + 0x48], 0
            mov dword ptr [esp + 0x44], edx
            add esp, 0xc
            fild qword ptr [esp + 0x38]
            fxch st(3)
            faddp st(1), st(0)
            fsqrt
            fst dword ptr [esp + 0x14]
            fmul dword ptr [gFLOAT_0058aff0+0x10]
            fxch st(2)
            fmulp st(2), st(0)
            fxch st(1)
            fmul dword ptr [gFLOAT_0058afdc+0xc]
            fsubr dword ptr [gFLOAT_0058afdc]
            fstp st(1)
            fcom dword ptr [gFLOAT_0058aff0+0x14]
            fnstsw ax
            test ah, 1
            je L004f9dfb
            fstp st(0)
            fld dword ptr [gFLOAT_0058aff0+0x14]
    L004f9dfb:
            fld dword ptr [esi - 0xc]
            fmul st(0), st(1)
            fstp dword ptr [esi - 0xc]
            fld dword ptr [esi - 8]
            fmul st(0), st(1)
            fstp dword ptr [esi - 8]
            fld dword ptr [esi - 4]
            fmul st(0), st(1)
            fstp dword ptr [esi - 4]
            mov eax, dword ptr [esi + 4]
            cmp eax, 0x3e8
            fstp st(0)
            ja L004f9e29
            cmp eax, dword ptr [esp + 0x10]
            jb L004f9e29
            xor ecx, ecx
            jmp L004f9e2e
    L004f9e29:
            mov ecx, 1
    L004f9e2e:
            mov eax, dword ptr [esi - 0x10]
            shl ecx, 0xf
            mov edx, dword ptr [eax + 0x1c]
            mov ax, word ptr [esi + 4]
            sub ax, word ptr [esp + 0x10]
            push edx
            mov edx, ebp
            add ecx, ebx
            push eax
            call AddShrapnelToPipingSession
            cmp dword ptr [esi + 4], 0x3e8
            jbe L004f9e5b
            mov dword ptr [esi + 4], 0xffffffff
    L004f9e5b:
            add esi, 0x2c
            inc ebx
            cmp esi, offset gSmoke_column+0x10
            jl L004f9c5b
            call AREndPipingSession
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x34
            ret
    }
#endif
}

static void C2_HOOK_FASTCALL ForEveryModelMaterial(br_model* pModel, tMaterialMaybeUpdate_cbfn* pCallback) {
    int i;
    int need_update = 0;

    if (pModel == NULL) {
        return;
    }
    for (i = 0; i < pModel->prepared->ngroups; i++) {
        br_material *material = pModel->faces[pModel->prepared->groups[i].face_user[0]].material;

        if (material != NULL) {
            need_update |= pCallback(material);
        }
    }
    if (need_update) {
        BrModelUpdate(pModel, 0x10);
    }
}

// FUNCTION: CARMA2_HW 0x004fe8a0
intptr_t C2_HOOK_CDECL ForEveryActorMaterial(br_actor* pActor, void *pContext) {
    tUser_crush_data* user_crush_data;
    tMaterialMaybeUpdate_cbfn* callback;

    callback = pContext;
    if (pActor->type != BR_ACTOR_MODEL) {
        return 0;
    }
    if (pActor->model == NULL) {
        return 0;
    }
    user_crush_data = pActor->user;
    if (user_crush_data == NULL) {
        return 0;
    }
    if (pActor->material != NULL) {
        callback(pActor->material);
    }
    ForEveryModelMaterial(pActor->model, callback);
    return 0;
}

// FUNCTION: CARMA2_HW 0x004fe930
intptr_t C2_HOOK_CDECL ForEveryActorMaterialNoGrooves(br_actor* pActor, void* pContext) {
    tUser_crush_data* user_crush_data;
    tMaterialMaybeUpdate_cbfn* callback;

    callback = pContext;
    if (pActor->type != BR_ACTOR_MODEL) {
        return 0;
    }
    if (pActor->model == NULL) {
        return 0;
    }
    user_crush_data = pActor->user;
    if (user_crush_data == NULL) {
        return 0;
    }
    if (user_crush_data->groove != NULL) {
        return 0;
    }
    if (pActor->material != NULL) {
        callback(pActor->material);
    }
    ForEveryModelMaterial(pActor->model, callback);
    return 0;
}

// FUNCTION: CARMA2_HW 0x004fe640
void C2_HOOK_FASTCALL ForEveryCarMaterial(tCar_spec* pCar_spec, tMaterialMaybeUpdate_cbfn* pCallback, int pGrooves) {
    int i;
    tCar_crush_spec* pCrush_spec = pCar_spec->car_crush_spec;
    tUser_crush_data* user_crush_data;

    if (pCar_spec->car_master_actor->material != NULL) {
        pCallback(pCar_spec->car_master_actor->material);
    }
    ForEveryModelMaterial(pCar_spec->shell_model, pCallback);
    user_crush_data = pCar_spec->car_model_actor->user;
    ForEveryModelMaterial(user_crush_data->models[1], pCallback);
    if (pGrooves) {
        DRActorEnumRecurse(pCar_spec->car_model_actor, ForEveryActorMaterial, pCallback);
    } else {
        DRActorEnumRecurse(pCar_spec->car_model_actor, ForEveryActorMaterialNoGrooves, pCallback);
    }
    for (i = 0; i < pCrush_spec->field_0x270; i++) {
        ForEveryModelMaterial(pCrush_spec->field_0x274[i].field_0x0->model, pCallback);
    }
    for (i = 0; i < pCrush_spec->field_0x2b0; i++) {
        ForEveryModelMaterial(pCrush_spec->field_0x2b4[i].field_0x0->model, pCallback);
    }
}

// FUNCTION: CARMA2_HW 0x004fed50
intptr_t C2_HOOK_FASTCALL UnBlendifyMaterialCB(br_material* pMaterial) {
    if (GetBlendificatiousnessOfMaterial(pMaterial) != 100) {
        BlendifyMaterial(pMaterial, 100);
        BrMaterialUpdate(pMaterial, BR_MATU_ALL);
        return 1;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004fed00
intptr_t C2_HOOK_FASTCALL BlendifyMaterialCB(br_material* pMaterial) {
    if (GetBlendificatiousnessOfMaterial(pMaterial) != 25) {
        BlendifyMaterial(pMaterial, 25);
        BrMaterialUpdate(pMaterial, BR_MATU_ALL);
        return 1;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004fca70
void C2_HOOK_FASTCALL StopCarSmokingInstantly(tCar_spec* pCar_spec) {
    int i;

    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gSmoke_column); i++) {
        if (gSmoke_column[i].core.car == pCar_spec) {
            gSmoke_column[i].lifetime = 0;
        }
    }
}

// FUNCTION: CARMA2_HW 0x004fc740
void C2_NAKED C2_HOOK_FASTCALL PipeInstantUnSmudge(tCar_spec* pCar_spec) {
#ifndef CARPOCALYPSE2_MATCHING
    br_model* model;
    br_model* b_model;
    br_actor* actor;
    br_actor* bonny;
    int j;
    int n;
    int v;
    int group;
    tSmudged_vertex data[1000];

    if (!gAction_replay_mode) {
        return;
    }
    actor = pCar_spec->car_actor;
    model = actor->model;
    bonny = pCar_spec->car_actor;
    n = 0;
    if ((model->flags & BR_MODF_KEEP_ORIGINAL) || (model->flags & BR_MODF_UPDATEABLE)) {
        ARStartPipingSession(ePipe_chunk_smudge);
        j = 0;
        for (group = 0; group < model->prepared->ngroups; group++) {
            for (v = 0; v < model->prepared->groups[group].nvertices; v++) {
                if ((model->prepared->groups[group].vertex_colours[v] >> 24) != 0) {
                    data[n].vertex_index = j;
                    data[n].light_index = -((tS32)(model->prepared->groups[group].vertex_colours[v] >> 24));
                    n += 1;
                    model->prepared->groups[group].vertex_colours[v] = 0;
                    if (model->flags & BR_MODF_UPDATEABLE) {
                        model->vertices[model->prepared->groups[group].vertex_user[v]].index = 0;
                    }
                    if (n >= CARPOCALYPSE2_ASIZE(data)) {
                        group = model->prepared->ngroups;
                        break;
                    }
                }
                j += 1;
            }
        }
        if (n != 0) {
            AddSmudgeToPipingSession(pCar_spec->car_ID, 0, n, data);
        }
        if (bonny != actor) {
            /* This branch is never taken */
            b_model = bonny->model;
            n = 0;
            j = 0;
            for (group = 0; group < b_model->prepared->ngroups; group++) {
                for (v = 0; v < b_model->prepared->groups[group].nvertices; v++) {
                    if ((b_model->prepared->groups[group].vertex_colours[v] >> 24) != 0) {
                        data[n].vertex_index = j;
                        data[n].light_index = -b_model->prepared->groups[group].nvertices;
                        n += 1;
                        b_model->prepared->groups[group].vertex_colours[v] = 0;
                        if (b_model->flags & BR_MODF_UPDATEABLE) {
                            b_model->vertices[b_model->prepared->groups[group].vertex_user[v]].index = 0;
                        }
                        if (n >= CARPOCALYPSE2_ASIZE(data)) {
                            group = b_model->prepared->groups[group].nvertices;
                            break;
                        }
                    }
                    j += 1;
                }
            }
            if (n != 0) {
                AddSmudgeToPipingSession(pCar_spec->car_ID, 0, n, data);
            }
        }
        AREndPipingSession();
    }
#else
    __asm {
            sub esp, 0xfb4
            mov eax, dword ptr [gPed_676914]
            push ebx
            push ebp
            xor ebp, ebp
            push esi
            cmp eax, ebp
            push edi
            mov dword ptr [esp + 0x1c], ecx
            jne L004fc9a6
            mov edi, dword ptr [ecx + 0xe10]
            xor ebx, ebx
            mov dword ptr [esp + 0x20], edi
            mov dword ptr [esp + 0x10], ebx
            mov esi, dword ptr [edi + 0x18]
            test byte ptr [esi + 0x20], 0x82
            je L004fc9a6
            mov ecx, 0x13
            call ARStartPipingSession
            mov ecx, dword ptr [esi + 0x4c]
            mov dword ptr [esp + 0x18], ebp
            mov dword ptr [esp + 0x14], ebp
            cmp word ptr [ecx + 8], bx
            jbe L004fc876
    L004fc799:
            mov ecx, dword ptr [ecx + 0x18]
            lea eax, [ebp + ebp*8]
            shl eax, 2
            add ecx, eax
            xor edi, edi
            cmp word ptr [ecx + 0x1e], di
            jbe L004fc85c
            lea edx, [esp + ebx*4 + 0x26]
    L004fc7b5:
            mov ecx, dword ptr [ecx + 0x14]
            test dword ptr [ecx + edi*4], 0xff000000
            je L004fc82d
            mov cx, word ptr [esp + 0x18]
            xor ebx, ebx
            mov word ptr [edx - 2], cx
            mov ecx, dword ptr [esi + 0x4c]
            add edx, 4
            mov ecx, dword ptr [ecx + 0x18]
            mov ecx, dword ptr [ecx + eax + 0x14]
            mov bl, byte ptr [ecx + edi*4 + 3]
            neg ebx
            mov word ptr [edx - 4], bx
            mov ecx, dword ptr [esi + 0x4c]
            mov ebx, dword ptr [esp + 0x10]
            mov ecx, dword ptr [ecx + 0x18]
            inc ebx
            mov dword ptr [esp + 0x10], ebx
            mov ecx, dword ptr [ecx + eax + 0x14]
            mov dword ptr [ecx + edi*4], 0
            mov cl, byte ptr [esi + 0x20]
            test cl, 0x80
            je L004fc821
            mov ecx, dword ptr [esi + 0x4c]
            mov ecx, dword ptr [ecx + 0x18]
            mov ebp, dword ptr [ecx + eax + 0x18]
            xor ecx, ecx
            mov cx, word ptr [ebp + edi*2]
            mov ebp, dword ptr [esi + 8]
            lea ecx, [ecx + ecx*4]
            mov byte ptr [ebp + ecx*8 + 0x14], 0
    L004fc821:
            cmp ebx, 0x3e8
            jge L004fc853
            mov ebp, dword ptr [esp + 0x14]
    L004fc82d:
            mov ecx, dword ptr [esp + 0x18]
            inc edi
            inc ecx
            xor ebx, ebx
            mov dword ptr [esp + 0x18], ecx
            mov ecx, dword ptr [esi + 0x4c]
            mov ecx, dword ptr [ecx + 0x18]
            add ecx, eax
            mov bx, word ptr [ecx + 0x1e]
            cmp edi, ebx
            jl L004fc7b5
            mov ebx, dword ptr [esp + 0x10]
            jmp L004fc85c
    L004fc853:
            mov edx, dword ptr [esi + 0x4c]
            xor ebp, ebp
            mov bp, word ptr [edx + 8]
    L004fc85c:
            mov ecx, dword ptr [esi + 0x4c]
            xor eax, eax
            inc ebp
            mov ax, word ptr [ecx + 8]
            mov dword ptr [esp + 0x14], ebp
            cmp ebp, eax
            jl L004fc799
            mov edi, dword ptr [esp + 0x20]
    L004fc876:
            test ebx, ebx
            je L004fc892
            mov eax, dword ptr [esp + 0x1c]
            lea ecx, [esp + 0x24]
            push ecx
            push ebx
            mov cx, word ptr [eax + 0x80]
            xor edx, edx
            call AddSmudgeToPipingSession
    L004fc892:
            cmp edi, edi
            je L004fc9a1
            mov eax, dword ptr [edi + 0x18]
            xor esi, esi
            xor ebp, ebp
            xor ebx, ebx
            mov edx, dword ptr [eax + 0x4c]
            mov dword ptr [esp + 0x10], ebp
            mov dword ptr [esp + 0x14], esi
            cmp word ptr [edx + 8], si
            jbe L004fc985
    L004fc8b8:
            mov edx, dword ptr [edx + 0x18]
            lea ecx, [esi + esi*8]
            shl ecx, 2
            add edx, ecx
            xor edi, edi
            cmp word ptr [edx + 0x1e], di
            jbe L004fc96b
            lea esi, [esp + ebp*4 + 0x26]
    L004fc8d3:
            mov edx, dword ptr [edx + 0x14]
            test dword ptr [edx + edi*4], 0xff000000
            je L004fc940
            mov word ptr [esi - 2], bx
            mov edx, dword ptr [eax + 0x4c]
            add esi, 4
            mov edx, dword ptr [edx + 0x18]
            mov dx, word ptr [edx + ecx + 0x1e]
            neg dx
            mov word ptr [esi - 4], dx
            mov edx, dword ptr [esp + 0x10]
            inc edx
            mov dword ptr [esp + 0x10], edx
            mov edx, dword ptr [eax + 0x4c]
            mov edx, dword ptr [edx + 0x18]
            mov edx, dword ptr [edx + ecx + 0x14]
            mov dword ptr [edx + edi*4], 0
            mov dl, byte ptr [eax + 0x20]
            test dl, 0x80
            je L004fc936
            mov edx, dword ptr [eax + 0x4c]
            mov edx, dword ptr [edx + 0x18]
            mov ebp, dword ptr [edx + ecx + 0x18]
            xor edx, edx
            mov dx, word ptr [ebp + edi*2]
            mov ebp, dword ptr [eax + 8]
            lea edx, [edx + edx*4]
            mov byte ptr [ebp + edx*8 + 0x14], 0
    L004fc936:
            cmp dword ptr [esp + 0x10], 0x3e8
            jge L004fc95e
    L004fc940:
            mov edx, dword ptr [eax + 0x4c]
            xor ebp, ebp
            inc edi
            inc ebx
            mov edx, dword ptr [edx + 0x18]
            add edx, ecx
            mov bp, word ptr [edx + 0x1e]
            cmp edi, ebp
            jl L004fc8d3
            mov esi, dword ptr [esp + 0x14]
            jmp L004fc96b
    L004fc95e:
            mov edx, dword ptr [eax + 0x4c]
            xor esi, esi
            mov edx, dword ptr [edx + 0x18]
            mov si, word ptr [edx + ecx + 0x1e]
    L004fc96b:
            mov edx, dword ptr [eax + 0x4c]
            mov ebp, dword ptr [esp + 0x10]
            xor ecx, ecx
            inc esi
            mov cx, word ptr [edx + 8]
            mov dword ptr [esp + 0x14], esi
            cmp esi, ecx
            jl L004fc8b8
    L004fc985:
            test ebp, ebp
            je L004fc9a1
            mov eax, dword ptr [esp + 0x1c]
            lea edx, [esp + 0x24]
            push edx
            push ebp
            mov cx, word ptr [eax + 0x80]
            xor edx, edx
            call AddSmudgeToPipingSession
    L004fc9a1:
            call AREndPipingSession
    L004fc9a6:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0xfb4
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
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004fca40
void C2_HOOK_FASTCALL StopCarSmoking(tCar_spec* pCar) {
    int i;

    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gSmoke_column); i++) {
        if (gSmoke_column[i].core.car == pCar && gSmoke_column[i].lifetime > 2000) {
            gSmoke_column[i].lifetime = 2000;
        }
    }
}

// FUNCTION: CARMA2_HW 0x004fca90
void C2_HOOK_FASTCALL StopObjectSmokingInstantly(tPhysics_object* pObject) {
    int i;

    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gSmoke_column); i++) {

        if (gSmoke_column[i].core.collision_info == pObject) {
            gSmoke_column[i].lifetime = 0;
        }
    }
}

// FUNCTION: CARMA2_HW 0x004fcab0
void C2_NAKED C2_HOOK_FASTCALL ConditionalSmokeColumn(tCar_spec* pCar, int fire_point, int level) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;
    int j;
    int flags;
    tU16 fire_value;

    if (level == 0 && (pCar == NULL || pCar->driver < 7)) {
        level = 1;
    }
    flags = gColumn_flags;
    for (i = 0; i < CARPOCALYPSE2_ASIZE(gSmoke_column); i++) {
        if ((flags & (1 << i)) && gSmoke_column[i].core.car == pCar) {
            for (j = 0; j < CARPOCALYPSE2_ASIZE(gSmoke_column); j++) {
                if (gSmoke_column[j].core.car != pCar) {
                    continue;
                }
                if (!(flags & (1 << j)) || gSmoke_column[j].colour > level) {
                    gSmoke_column[j].lifetime = 0x7d0;
                } else if (gSmoke_column[j].lifetime != 0) {
                    return;
                } else {
                    gSmoke_column[j].lifetime = 0x7d0;
                }
            }
            break;
        }
    }
    fire_value = pCar->fire_vertex[fire_point];
    if (pCar->collision_info != NULL && pCar->collision_info->last_special_volume != NULL) {
        if (pCar->collision_info->last_special_volume->gravity_multiplier < 1.0f) {
            return;
        }
    }
    SmudgeCar(pCar, fire_value);
    if (pCar->car_crush_spec != NULL) {
        SmudgeCar(pCar, pCar->fire_vertex[IRandomBetween(0, 0xb)]);
    }
    CreateSmokeColumn2(1, pCar->car_master_actor, pCar, NULL, level, fire_value, 0x989680);
#else
    __asm {
            push ebx
            mov ebx, dword ptr [esp + 8]
            push ebp
            push esi
            push edi
            mov edi, ecx
            test ebx, ebx
            jne L004fcacd
            test edi, edi
            je L004fcac8
            cmp dword ptr [edi + 0xc], 7
            jge L004fcacd
    L004fcac8:
            mov ebx, 1
    L004fcacd:
            mov esi, dword ptr [gColumn_flags]
            xor ecx, ecx
            mov eax, offset gSmoke_column+0x4
    L004fcada:
            mov ebp, 1
            shl ebp, cl
            test ebp, esi
            je L004fcae9
            cmp dword ptr [eax], edi
            je L004fcaf6
    L004fcae9:
            add eax, 0x7c
            inc ecx
            cmp eax, offset gSparks+0x4
            jl L004fcada
            jmp L004fcb2c
    L004fcaf6:
            xor ecx, ecx
            mov eax, offset gSmoke_column+0x14
    L004fcafd:
            cmp dword ptr [eax - 0x10], edi
            jne L004fcb21
            mov ebp, 1
            shl ebp, cl
            test ebp, esi
            je L004fcb1b
            cmp dword ptr [eax + 8], ebx
            jg L004fcb1b
            cmp dword ptr [eax], 0
            jne L004fcb9d
    L004fcb1b:
            mov dword ptr [eax], 0x7d0
    L004fcb21:
            add eax, 0x7c
            inc ecx
            cmp eax, offset gSparks+0x14
            jl L004fcafd
    L004fcb2c:
            mov eax, dword ptr [edi + 8]
            xor esi, esi
            mov si, word ptr [edi + edx*2 + 0xac]
            mov eax, dword ptr [eax + 0x258]
            test eax, eax
            je L004fcb56
            fld dword ptr [eax + 0x88]
            fcomp dword ptr [gFLOAT_0058afdc]
            fnstsw ax
            test ah, 1
            jne L004fcb9d
    L004fcb56:
            mov edx, esi
            mov ecx, edi
            call SmudgeCar
            mov eax, dword ptr [edi + 0x1d4]
            test eax, eax
            je L004fcb86
            mov edx, 0xb
            xor ecx, ecx
            call IRandomBetween
            xor edx, edx
            mov ecx, edi
            mov dx, word ptr [edi + eax*2 + 0xac]
            call SmudgeCar
    L004fcb86:
            mov edx, dword ptr [edi + 0x10]
            push 0x989680
            push esi
            push ebx
            push 0
            push edi
            mov ecx, 1
            call CreateSmokeColumn2
    L004fcb9d:
            pop edi
            pop esi
            pop ebp
            pop ebx
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
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004fa5b0
void C2_NAKED C2_HOOK_FASTCALL GenerateContinuousSmoke(tCar_spec* pCar, int pWheel, tU32 pTime) {
#ifndef CARPOCALYPSE2_MATCHING
    br_vector3 pos;
    br_vector3 v;
    br_vector3 vcs;
    br_vector3 tv;
    br_scalar decay_factor;
    br_scalar ts;
    br_scalar alpha;
    br_scalar beta;
    int colour;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, dust_time, 0x1284);

    if (pTime < (tU32)pCar->dust_time[pWheel]) {
        pCar->dust_time[pWheel] -= pTime;
        return;
    }
    pCar->dust_time[pWheel] += IRandomBetween(200, 400) - pTime;
    if (pCar->dust_time[pWheel] < 0) {
        pCar->dust_time[pWheel] = 0;
    }
    BrVector3Cross(&tv, &pCar->collision_info->omega, &pCar->wpos[pWheel]);
    BrVector3Scale(&vcs, &pCar->collision_info->velocity_car_space, WORLD_SCALE);
    BrVector3Accumulate(&vcs, &tv);
    ts = BrVector3LengthSquared(&vcs);
    if (ts < 25.f) {
        return;
    }
    decay_factor = sqrtf(ts) / 25.f;
    if (decay_factor > 1.f) {
        decay_factor = 1.f;
    }
    BrVector3InvScale(&tv, &pCar->wpos[pWheel], WORLD_SCALE);
    tv.v[1] -= pCar->oldd[pWheel] / WORLD_SCALE;

    alpha = -1000.0f;
    if (vcs.v[2] < 0.0f) {
        alpha = (pCar->collision_info->bb2.max.v[2] - tv.v[2]) / vcs.v[2];
    } else if (vcs.v[2] > 0.0f) {
        alpha = (pCar->collision_info->bb2.min.v[2] - tv.v[2]) / vcs.v[2];
    }

    beta = -1000.0f;
    if (vcs.v[0] < 0.0f) {
        beta = (pCar->collision_info->bb2.max.v[0] - tv.v[0]) / vcs.v[0];
    } else if (vcs.v[0] < 0.0f) {
        beta = (pCar->collision_info->bb2.min.v[0] - tv.v[0]) / vcs.v[0];
    }

    ts = MAX(alpha, beta);
    BrVector3Scale(&pos, &vcs, ts);
    BrVector3Accumulate(&tv, &pos);
    BrMatrix34ApplyP(&pos, &tv, &pCar->car_master_actor->t.t.mat);
    BrMatrix34ApplyV(&v, &vcs, &pCar->car_master_actor->t.t.mat);

    colour = gDust_rotate + gCurrent_race.material_modifiers[pCar->material_index[pWheel]].smoke_type - 2;
    while (colour >= gNum_dust_tables) {
        colour -= gNum_dust_tables;
    }
    CreatePuffOfSmoke(&pos, &v, decay_factor, 2 * decay_factor, colour + 8);
#else
    __asm {
            sub esp, 0x34
            push ebx
            mov ebx, dword ptr [esp + 0x3c]
            push esi
            push edi
            mov edi, edx
            mov esi, ecx
            mov eax, dword ptr [esi + edi*4 + 0x1284]
            cmp eax, ebx
            jbe L004fa5db
            sub eax, ebx
            mov dword ptr [esi + edi*4 + 0x1284], eax
            pop edi
            pop esi
            pop ebx
            add esp, 0x34
            ret 4
    L004fa5db:
            mov edx, 0x190
            mov ecx, 0xc8
            call IRandomBetween
            mov ecx, dword ptr [esi + edi*4 + 0x1284]
            sub eax, ebx
            add ecx, eax
            mov dword ptr [esi + edi*4 + 0x1284], ecx
            jns L004fa609
            mov dword ptr [esi + edi*4 + 0x1284], 0
    L004fa609:
            mov ecx, dword ptr [esi + 8]
            lea edx, [edi + edi*2]
            lea eax, [edi + 0x183]
            fld dword ptr [ecx + 0x7c]
            fld dword ptr [esi + edx*4 + 0x122c]
            fmul dword ptr [ecx + 0x78]
            fxch st(1)
            fmul dword ptr [esi + edx*4 + 0x1228]
            lea edx, [esi + edx*4]
            lea eax, [eax + eax*2]
            lea ebx, [esi + eax*4]
            fsubp st(1), st(0)
            _emit 0xd9
        _emit 0x03
            fxch st(1)
            fstp dword ptr [esp + 0x1c]
            fld dword ptr [ecx + 0x74]
            fld dword ptr [ecx + 0x7c]
            fmulp st(2), st(0)
            fmul dword ptr [edx + 0x122c]
            fsubp st(1), st(0)
            _emit 0xd9
        _emit 0x03
            fxch st(1)
            fstp dword ptr [esp + 0x20]
            fld dword ptr [ecx + 0x74]
            fld dword ptr [ecx + 0x78]
            fxch st(1)
            fmul dword ptr [edx + 0x1228]
            fxch st(1)
            fmulp st(2), st(0)
            fld dword ptr [esp + 0x1c]
            fxch st(2)
            fsubp st(1), st(0)
            fld dword ptr [esp + 0x20]
            fxch st(1)
            fstp dword ptr [esp + 0x24]
            fld dword ptr [ecx + 0x1a8]
            fmul dword ptr [gFLOAT_0058b054]
            fld dword ptr [esp + 0x24]
            fxch st(1)
            fstp dword ptr [esp + 0x10]
            fld dword ptr [ecx + 0x1ac]
            fmul dword ptr [gFLOAT_0058b054]
            fxch st(3)
            fadd dword ptr [esp + 0x10]
            fxch st(3)
            fstp dword ptr [esp + 0x14]
            fld dword ptr [ecx + 0x1b0]
            fmul dword ptr [gFLOAT_0058b054]
            fxch st(2)
            fadd dword ptr [esp + 0x14]
            fxch st(3)
            fstp dword ptr [esp + 0x10]
            fadd st(0), st(1)
            fxch st(2)
            fstp dword ptr [esp + 0x14]
            fxch st(1)
            fstp dword ptr [esp + 0x18]
            fstp st(0)
            fld dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x10]
            fmul dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x18]
            fxch st(2)
            fmul dword ptr [esp + 0x14]
            fxch st(2)
            fmul dword ptr [esp + 0x18]
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fcom dword ptr [gFLOAT_0058b054+0x4]
            fnstsw ax
            test ah, 1
            jne L004fa8c3
            fsqrt
            fmul dword ptr [gFLOAT_0058b054+0x8]
            fst dword ptr [esp + 0xc]
            fcomp dword ptr [gFLOAT_0058afdc]
            fnstsw ax
            test ah, 0x41
            jne L004fa726
            mov dword ptr [esp + 0xc], 0x3f800000
    L004fa726:
            fld dword ptr [edx + 0x1228]
            fld dword ptr [esi + edi*4 + 0x1264]
            fld dword ptr [esp + 0x18]
            fcomp dword ptr [gFLOAT_0058b008]
            _emit 0xd9
        _emit 0x03
            fld dword ptr [edx + 0x122c]
            fxch st(3)
            fmul dword ptr [gFLOAT_0058b060]
            fxch st(2)
            fmul qword ptr [gFLOAT_0058b060+0x8]
            fld dword ptr [gFLOAT_0058b060+0x10]
            fxch st(2)
            fmul dword ptr [gFLOAT_0058b060]
            fxch st(4)
            fmul dword ptr [gFLOAT_0058b060]
            fxch st(1)
            fsubp st(3), st(0)
            fxch st(3)
            fstp dword ptr [esp + 0x1c]
            fxch st(2)
            fstp dword ptr [esp + 0x24]
            fnstsw ax
            fstp dword ptr [esp + 0x20]
            test ah, 1
            fstp dword ptr [esp + 0x44]
            je L004fa78f
            fld dword ptr [ecx + 0x50]
            jmp L004fa7a3
    L004fa78f:
            fld dword ptr [esp + 0x18]
            fcomp dword ptr [gFLOAT_0058b008]
            fnstsw ax
            test ah, 0x41
            jne L004fa7af
            fld dword ptr [ecx + 0x44]
    L004fa7a3:
            fsub dword ptr [esp + 0x24]
            fdiv dword ptr [esp + 0x18]
            fstp dword ptr [esp + 0x44]
    L004fa7af:
            fld dword ptr [esp + 0x10]
            fcomp dword ptr [gFLOAT_0058b008]
            fld dword ptr [gFLOAT_0058b060+0x10]
            fnstsw ax
            test ah, 1
            je L004fa7cd
            fstp st(0)
            fld dword ptr [ecx + 0x48]
            jmp L004fa7e3
    L004fa7cd:
            fld dword ptr [esp + 0x10]
            fcomp dword ptr [gFLOAT_0058b008]
            fnstsw ax
            test ah, 0x41
            jne L004fa7eb
            fstp st(0)
            fld dword ptr [ecx + 0x3c]
    L004fa7e3:
            fsub dword ptr [esp + 0x1c]
            fdiv dword ptr [esp + 0x10]
    L004fa7eb:
            fld dword ptr [esp + 0x44]
            fcomp st(1)
            fnstsw ax
            test ah, 0x41
            jne L004fa7fe
            fstp st(0)
            fld dword ptr [esp + 0x44]
    L004fa7fe:
            fld st(0)
            fmul dword ptr [esp + 0x10]
            mov ecx, dword ptr [esi + 0x10]
            lea edx, [esp + 0x1c]
            add ecx, 0x2c
            lea eax, [esp + 0x28]
            fstp dword ptr [esp + 0x28]
            push ecx
            push edx
            fld st(0)
            fmul dword ptr [esp + 0x1c]
            push eax
            fstp dword ptr [esp + 0x38]
            fmul dword ptr [esp + 0x24]
            fstp dword ptr [esp + 0x3c]
            fld dword ptr [esp + 0x28]
            fadd dword ptr [esp + 0x34]
            fstp dword ptr [esp + 0x28]
            fld dword ptr [esp + 0x2c]
            fadd dword ptr [esp + 0x38]
            fstp dword ptr [esp + 0x2c]
            fld dword ptr [esp + 0x30]
            fadd dword ptr [esp + 0x3c]
            fstp dword ptr [esp + 0x30]
            call BrMatrix34ApplyP
            mov ecx, dword ptr [esi + 0x10]
            add esp, 0xc
            add ecx, 0x2c
            lea edx, [esp + 0x10]
            push ecx
            lea eax, [esp + 0x38]
            push edx
            push eax
            call BrMatrix34ApplyV
            mov esi, dword ptr [esi + edi*4 + 0x1274]
            mov eax, dword ptr [gDust_rotate]
            add esp, 0xc
            lea ecx, [esi + esi*4]
            mov edx, dword ptr [ecx*8 + gCurrent_race+0xf28]
            mov ecx, dword ptr [gNum_dust_tables]
            lea eax, [edx + eax - 2]
            cmp eax, ecx
            jl L004fa89a
    L004fa894:
            sub eax, ecx
            cmp eax, ecx
            jge L004fa894
    L004fa89a:
            fld dword ptr [esp + 0xc]
            add eax, 8
            lea edx, [esp + 0x34]
            fadd st(0), st(0)
            push eax
            push ecx
            mov ecx, dword ptr [esp + 0x14]
            _emit 0xd9
        _emit 0x1c
        _emit 0x24
            push ecx
            lea ecx, [esp + 0x34]
            call CreatePuffOfSmoke
            pop edi
            pop esi
            pop ebx
            add esp, 0x34
            ret 4
    L004fa8c3:
            pop edi
            pop esi
            fstp st(0)
            pop ebx
            add esp, 0x34
            ret 4
    }
#endif
}

extern const double gDOUBLE_0058b0b0;

// FUNCTION: CARMA2_HW 0x004fb330
void C2_HOOK_FASTCALL CreatePuffOfSmoke(br_vector3* pPos, br_vector3* pV, float pStrength, float pDecay_factor, int pType) {
    br_vector3 tv;

    C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(gSmoke) != 25);

    if (!gSmoke_on) {
        return;
    }
    BrVector3Sub(&tv, pPos, &gProgram_state.current_car.pos);
    if (BrVector3LengthSquared(&tv) > 625.f) {
        if (gCar_to_view == &gProgram_state.current_car) {
            return;
        }
        BrVector3Sub(&tv, pPos, &gCar_to_view->pos);
        if (BrVector3LengthSquared(&tv) > 625.f) {
            return;
        }
    }
    gSmoke[gSmoke_num].v.v[0] = pV->v[0] * gFLOAT_0058b060;
    gSmoke[gSmoke_num].v.v[1] = pV->v[1] * gFLOAT_0058b060;
    gSmoke[gSmoke_num].v.v[2] = pV->v[2] * gFLOAT_0058b060;
    gSmoke[gSmoke_num].v.v[1] = (br_scalar)(gSmoke[gSmoke_num].v.v[1] - gDOUBLE_0058b0b0);
    BrVector3Copy(&gSmoke[gSmoke_num].pos, pPos);
    gSmoke[gSmoke_num].radius = 0.05f;
    if ((pType & 0xf) == 7) {
        gSmoke[gSmoke_num].radius = 0.1f;
    } else {
        gSmoke[gSmoke_num].pos.v[1] += 0.04f;
    }
    gSmoke[gSmoke_num].pos.v[1] += 0.04f;
    if (pStrength > gFLOAT_0058afdc) {
        pStrength = gFLOAT_0058afdc;
    }
    gSmoke[gSmoke_num].strength = pStrength;
    gSmoke_flags |= 1 << gSmoke_num;
    gSmoke[gSmoke_num].time_sync = gPHIL_mechanics_time_sync;
    gSmoke[gSmoke_num].type = (tU8)pType;
    gSmoke[gSmoke_num].decay_factor = pDecay_factor;
    gSmoke[gSmoke_num].pipe_me = 1;
    gSmoke_num++;
    if (gSmoke_num >= (int)CARPOCALYPSE2_ASIZE(gSmoke)) {
        gSmoke_num = 0;
    }
}

// FUNCTION: CARMA2_HW 0x004fedd0
void C2_HOOK_FASTCALL ClearSplashReplay(uintptr_t pObject) {
    int flags;
    int i;

    flags = gReplay_splash_flags;
    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gReplay_splashes); i++) {
        uintptr_t object;

        memcpy(&object, &gReplay_splashes[i].object, sizeof(object));
        if (pObject == object) {
            flags &= ~(1 << i);
        }
    }
    gReplay_splash_flags = flags;
}

// FUNCTION: CARMA2_HW 0x004fb630
void C2_NAKED C2_HOOK_FASTCALL CreateSmokeColumn2(int pType, br_actor* pActor, tCar_spec* pCar, tPhysics_object* pObject, int pColour, int pVertex, tU32 pLifetime) {
#ifndef CARPOCALYPSE2_MATCHING
    tSmoke_column* old_column;
    tSmoke_column* column;
    tSmoke_column_core core;
    br_actor* flame;
    int i;

    old_column = &gSmoke_column[gColumn_next];
    if (pObject == NULL) {
        pObject = pCar == NULL ? NULL : pCar->collision_info;
    }
    core.field_0x0 = pType;
    core.car = pCar;
    core.collision_info = pObject;
    core.car_actor = pActor;
    if (!gSmoke_on) {
        return;
    }
    if ((gColumn_flags & (1u << (gColumn_next & 31))) != 0) {
        ARStartPipingSession((tPipe_chunk_type)17);
        column = &gSmoke_column[gColumn_next];
        AddSmokeColumnToPipingSession(gColumn_next, &column->core, column->vertex_index, column->colour);
        AREndPipingSession();
        if (gSmoke_column[gColumn_next].colour == 0) {
            ARStartPipingSession((tPipe_chunk_type)18);
            flame = old_column->flame_actor->children;
            for (i = 0; i < 3; i++) {
                if (flame->type == 1) {
                    AddFlameToPipingSession((gColumn_next << 4) + i, old_column->frame_count[i],
                        old_column->scale_x[i], old_column->scale_y[i], old_column->offset_x[i], old_column->offset_z[i]);
                }
                flame->type = 0;
                flame = flame->next;
            }
            AREndPipingSession();
        }
    }
    if (pColour == 0) {
        if ((gColumn_flags & (1u << (gColumn_next & 31))) == 0 || gSmoke_column[gColumn_next].colour != 0) {
            BrActorAdd(gNon_track_actor, gSmoke_column[gColumn_next].flame_actor);
        }
    } else if ((gColumn_flags & (1u << (gColumn_next & 31))) != 0 && gSmoke_column[gColumn_next].colour == 0) {
        BrActorRemove(gSmoke_column[gColumn_next].flame_actor);
    }
    ARStartPipingSession((tPipe_chunk_type)17);
    AddSmokeColumnToPipingSession(gColumn_next, &core, pVertex, pColour);
    AREndPipingSession();
    column = &gSmoke_column[gColumn_next];
    column->core = core;
    column->colour = pColour;
    column->lifetime = pLifetime;
    column->time = 0;
    column->smudge_timer = 1000;
    column->vertex_index = pVertex;
    column->upright = 1;
    gColumn_flags |= 1u << (gColumn_next & 31);
    column->frame_count[0] = 100;
    column->frame_count[1] = 100;
    column->frame_count[2] = 100;
    gColumn_next++;
    if (gColumn_next >= 10) {
        gColumn_next = 0;
    }
#else
    __asm {
            sub esp, 0x10
            push ebx
            push ebp
            push esi
            mov esi, ecx
            mov ecx, dword ptr [gNext_column]
            push edi
            mov eax, ecx
            mov edi, dword ptr [esp + 0x24]
            shl eax, 5
            sub eax, ecx
            lea ebx, [eax*4 + gSmoke_column]
            mov eax, dword ptr [esp + 0x28]
            test eax, eax
            jne L004fb664
            test edi, edi
            je L004fb662
            mov eax, dword ptr [edi + 8]
            jmp L004fb664
    L004fb662:
            xor eax, eax
    L004fb664:
            mov dword ptr [esp + 0x18], eax
            mov eax, dword ptr [gSmoke_on]
            test eax, eax
            mov dword ptr [esp + 0x14], edi
            mov dword ptr [esp + 0x1c], edx
            mov dword ptr [esp + 0x10], esi
            je L004fb883
            mov eax, dword ptr [gColumn_flags]
            mov edx, 1
            shl edx, cl
            test eax, edx
            je L004fb73c
            mov ecx, 0x11
            call ARStartPipingSession
            mov ecx, dword ptr [gNext_column]
            mov eax, ecx
            shl eax, 5
            sub eax, ecx
            shl eax, 2
            mov edx, dword ptr [eax + gSmoke_column+0x1c]
            push edx
            mov edx, dword ptr [eax + gSmoke_column+0x74]
            push edx
            lea edx, [eax + gSmoke_column]
            call AddSmokeColumnToPipingSession
            call AREndPipingSession
            mov ecx, dword ptr [gNext_column]
            mov eax, ecx
            shl eax, 5
            sub eax, ecx
            mov edx, dword ptr [eax*4 + gSmoke_column+0x1c]
            test edx, edx
            jne L004fb73c
            mov ecx, 0x12
            call ARStartPipingSession
            mov ecx, dword ptr [ebx + 0x24]
            xor ebp, ebp
            lea edi, [ebx + 0x58]
            mov esi, dword ptr [ecx + 8]
    L004fb6fa:
            cmp byte ptr [esi + 0x12], 1
            jne L004fb722
            mov edx, dword ptr [edi + 0xc]
            mov eax, dword ptr [edi]
            mov ecx, dword ptr [edi - 0xc]
            push edx
            mov edx, dword ptr [edi - 0x18]
            push eax
            push ecx
            mov ecx, dword ptr [gNext_column]
            shl ecx, 4
            push edx
            mov edx, dword ptr [edi - 0x30]
            add ecx, ebp
            call AddFlameToPipingSession
    L004fb722:
            inc ebp
            add edi, 4
            mov byte ptr [esi + 0x12], 0
            mov esi, dword ptr [esi]
            cmp ebp, 3
            jl L004fb6fa
            call AREndPipingSession
            mov ecx, dword ptr [gNext_column]
    L004fb73c:
            mov edi, dword ptr [esp + 0x2c]
            xor ebx, ebx
            cmp edi, ebx
            jne L004fb78f
            mov edx, dword ptr [gColumn_flags]
            mov eax, 1
            shl eax, cl
            test eax, edx
            je L004fb767
            mov eax, ecx
            shl eax, 5
            sub eax, ecx
            cmp dword ptr [eax*4 + gSmoke_column+0x1c], ebx
            je L004fb78b
    L004fb767:
            mov edx, ecx
            shl edx, 5
            sub edx, ecx
            mov ecx, dword ptr [gNon_track_actor]
            mov eax, dword ptr [edx*4 + gSmoke_column+0x24]
            push eax
            push ecx
            call BrActorAdd
            mov ecx, dword ptr [gNext_column]
            add esp, 8
    L004fb78b:
            cmp edi, ebx
            je L004fb7c0
    L004fb78f:
            mov eax, dword ptr [gColumn_flags]
            mov edx, 1
            shl edx, cl
            test edx, eax
            je L004fb7c0
            mov eax, ecx
            shl eax, 5
            sub eax, ecx
            shl eax, 2
            cmp dword ptr [eax + gSmoke_column+0x1c], ebx
            jne L004fb7c0
            mov ecx, dword ptr [eax + gSmoke_column+0x24]
            push ecx
            call BrActorRemove
            add esp, 4
    L004fb7c0:
            mov ecx, 0x11
            call ARStartPipingSession
            mov esi, dword ptr [esp + 0x30]
            mov ecx, dword ptr [gNext_column]
            push edi
            push esi
            lea edx, [esp + 0x18]
            call AddSmokeColumnToPipingSession
            call AREndPipingSession
            mov ecx, dword ptr [gNext_column]
            mov ebp, dword ptr [esp + 0x10]
            mov eax, ecx
            shl eax, 5
            sub eax, ecx
            shl eax, 2
            lea edx, [eax + gSmoke_column]
            mov dword ptr [eax + gSmoke_column], ebp
            mov ebp, dword ptr [esp + 0x14]
            mov dword ptr [edx + 4], ebp
            mov ebp, dword ptr [esp + 0x18]
            mov dword ptr [edx + 8], ebp
            mov ebp, dword ptr [esp + 0x1c]
            mov dword ptr [edx + 0xc], ebp
            mov edx, dword ptr [esp + 0x34]
            mov dword ptr [eax + gSmoke_column+0x1c], edi
            mov dword ptr [eax + gSmoke_column+0x14], edx
            mov edx, 1
            mov dword ptr [eax + gSmoke_column+0x10], ebx
            shl edx, cl
            mov dword ptr [eax + gSmoke_column+0x70], 0x3e8
            mov dword ptr [eax + gSmoke_column+0x74], esi
            mov esi, dword ptr [gColumn_flags]
            mov dword ptr [eax + gSmoke_column+0x78], 1
            lea eax, [eax + gSmoke_column+0x28]
            or esi, edx
            mov edx, 0x64
            mov dword ptr [eax], edx
            inc ecx
            mov dword ptr [eax + 4], edx
            cmp ecx, 0xa
            mov dword ptr [gColumn_flags], esi
            mov dword ptr [eax + 8], edx
            mov dword ptr [gNext_column], ecx
            jl L004fb883
            mov dword ptr [gNext_column], ebx
    L004fb883:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x10
            ret 0x14
            nop
            nop
            nop
    }
#endif
}

void C2_HOOK_FASTCALL ReplaySmokeColumn(tU32 pTime) {
    int i;

    for (i = 0; i < CARPOCALYPSE2_ASIZE(gSmoke_column); i++) {
        if (gColumn_flags & (1 << i)) {
            br_vector3 dummy;

            DoSmokeColumn(i, pTime, &dummy);
            if (gSmoke_column[i].colour == 0) {
                FlameAnimate(i, &gSmoke_column[i].pos, pTime);
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x004fba40
void C2_NAKED C2_HOOK_FASTCALL MungeSmokeColumn(tU32 pTime) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;

    if (gColumn_flags == 0) {
        return;
    }
    if (gAction_replay_mode) {
        ReplaySmokeColumn(pTime);
        return;
    }

    gPHIL_mechanics_time_sync = 1;
    for (i = 0; i < CARPOCALYPSE2_ASIZE(gSmoke_column); i++) {
        if (!(gColumn_flags & (1 << i))) {
            continue;
        }
        if (gSmoke_column[i].lifetime >= pTime) {
            tPhysics_object* object;
            tCar_spec* c;
            br_vector3 car_pos;

            gSmoke_column[i].lifetime -= pTime;
            object = gSmoke_column[i].core.collision_info;
            c = gSmoke_column[i].core.car;
            DoSmokeColumn(i, pTime, &car_pos);
            if (gSmoke_column[i].core.field_0x0 == 1 && c->car_crush_spec->field_0x4b8 != 0) {
                if (IsOnNonCarHalf(c, &car_pos)) {
                    gSmoke_column[i].lifetime = 0;
                }
            }
            if (gSmoke_column[i].colour == 0) {
                FlameAnimate(i, &gSmoke_column[i].pos, pTime);
                if (gSmoke_column[i].core.field_0x0 == 1) {
                    if (gSmoke_column[i].smudge_timer >= pTime) {
                        gSmoke_column[i].smudge_timer -= pTime;
                    } else {
                        gSmoke_column[i].smudge_timer += 2000;
                        SmudgeCar(c, gSmoke_column[i].vertex_index);
                        if (c->knackered) {
                            int plane;

                            plane = IRandomBetween(0, CARPOCALYPSE2_ASIZE(gSmoke_column[i].core.car->fire_vertex) - 1);
                            SmudgeCar(c, gSmoke_column[i].core.car->fire_vertex[plane]);
                        }
                    }
                }
            }
            gSmoke_column[i].time += pTime;
            if (gSmoke_column[i].time > 200) {
                br_vector3 pos;
                br_vector3 v;
                br_scalar decay_factor;

                gSmoke_column[i].time -= pTime;
                gSmoke_column[i].count += 1;
                pos.v[0] = gSmoke_column[i].pos.v[0] + SRandomBetween(-.03f, .03f);
                pos.v[1] = gSmoke_column[i].pos.v[1] + (gSmoke_column[i].colour == 0) * .05f;
                pos.v[2] = gSmoke_column[i].pos.v[2] + SRandomBetween(-.03f, .03f);
                if (!(gSmoke_column[i].whiter & 0x2) || IRandomBetween(0, 3)) {
                    if (gSmoke_column[i].whiter > 0) {
                        gSmoke_column[i].whiter = 2;
                    } else {
                        gSmoke_column[i].whiter = -2;
                    }
                } else {
                    gSmoke_column[i].whiter &= 0x1;
                }
                decay_factor = ((float)(gSmoke_column[i].whiter > 0) + 1.f) / 2.f;
                if (gSmoke_column[i].lifetime < 4000) {
                    decay_factor = gSmoke_column[i].lifetime * decay_factor / 4000.0f;
                }
                if (gSmoke_column[i].core.collision_info == NULL) {
                    BrVector3SetFloat(&v, 0.f, 2.8985507f, 0.f);
                } else {
                    BrVector3Cross(&v, &object->omega, &car_pos);
                    BrMatrix34ApplyV(&car_pos, &v, &object->actor->t.t.mat);
                    v.v[0] = car_pos.v[0] + WORLD_SCALE * object->v.v[0];
                    v.v[1] = car_pos.v[1] + WORLD_SCALE * object->v.v[1];
                    v.v[2] = car_pos.v[2] + WORLD_SCALE * object->v.v[2];
                }
                CreatePuffOfSmoke(&pos, &v, decay_factor, decay_factor, gSmoke_column[i].colour + 16);
            }
        } else {
            if (gSmoke_column[i].core.field_0x0 != 0) {
                ARStartPipingSession(ePipe_chunk_smoke_column);
                AddSmokeColumnToPipingSession(i, &gSmoke_column[i], gSmoke_column[i].vertex_index, gSmoke_column[i].colour);
                AREndPipingSession();
            }
            gColumn_flags &= ~(1 << i);
            if (gSmoke_column[i].colour == 0) {
                BrActorRemove(gSmoke_column[i].flame_actor);
            }
        }
    }
#else
    __asm {
            mov eax, dword ptr [gColumn_flags]
            sub esp, 0x38
            test eax, eax
            push ebx
            push ebp
            push esi
            push edi
            mov ebp, ecx
            je L004fbdc6
            mov eax, dword ptr [gPed_676914]
            test eax, eax
            je L004fbaac
            xor esi, esi
            mov edi, offset gSmoke_column+0x34
    L004fba66:
            mov eax, 1
            mov ecx, esi
            shl eax, cl
            mov ecx, dword ptr [gColumn_flags]
            test eax, ecx
            je L004fba98
            lea edx, [esp + 0x18]
            mov ecx, esi
            push edx
            mov edx, ebp
            call DoSmokeColumn
            mov eax, dword ptr [edi - 0x18]
            test eax, eax
            jne L004fba98
            push ebp
            mov edx, edi
            mov ecx, esi
            call FlameAnimate
    L004fba98:
            add edi, 0x7c
            inc esi
            cmp edi, offset gSparks+0x34
            jl L004fba66
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x38
            ret
    L004fbaac:
            xor ebx, ebx
            mov dword ptr [gPHIL_mechanics_time_sync], 1
            mov dword ptr [esp + 0x10], ebx
            mov esi, offset gSmoke_column
    L004fbac1:
            mov eax, dword ptr [gColumn_flags]
            mov edi, 1
            mov ecx, ebx
            shl edi, cl
            test eax, edi
            je L004fbdae
            mov eax, dword ptr [esi + 0x14]
            cmp eax, ebp
            jae L004fbb2f
            cmp dword ptr [esi], 0
            je L004fbb03
            mov ecx, 0x11
            call ARStartPipingSession
            mov eax, dword ptr [esi + 0x1c]
            mov ecx, dword ptr [esi + 0x74]
            push eax
            push ecx
            mov edx, esi
            mov ecx, ebx
            call AddSmokeColumnToPipingSession
            call AREndPipingSession
    L004fbb03:
            mov ecx, dword ptr [gColumn_flags]
            mov eax, dword ptr [esi + 0x1c]
            not edi
            and ecx, edi
            test eax, eax
            mov dword ptr [gColumn_flags], ecx
            jne L004fbdae
            mov edx, dword ptr [esi + 0x24]
            push edx
            call BrActorRemove
            add esp, 4
            jmp L004fbdae
    L004fbb2f:
            mov ecx, dword ptr [esp + 0x10]
            sub eax, ebp
            mov dword ptr [esi + 0x14], eax
            mov edi, dword ptr [esi + 4]
            mov ebx, dword ptr [esi + 8]
            lea eax, [esp + 0x30]
            push eax
            mov edx, ebp
            call DoSmokeColumn
            cmp dword ptr [esi], 1
            jne L004fbb75
            mov ecx, dword ptr [edi + 0x18d4]
            mov al, byte ptr [ecx + 0x4b8]
            test al, al
            je L004fbb75
            lea edx, [esp + 0x30]
            mov ecx, edi
            call IsOnNonCarHalf
            test eax, eax
            je L004fbb75
            mov dword ptr [esi + 0x14], 0
    L004fbb75:
            mov eax, dword ptr [esi + 0x1c]
            test eax, eax
            jne L004fbbd9
            mov ecx, dword ptr [esp + 0x10]
            push ebp
            lea edx, [esi + 0x34]
            call FlameAnimate
            cmp dword ptr [esi], 1
            jne L004fbbd9
            mov eax, dword ptr [esi + 0x70]
            cmp eax, ebp
            jae L004fbbd4
            add eax, 0x7d0
            test edi, edi
            mov dword ptr [esi + 0x70], eax
            je L004fbbd9
            mov edx, dword ptr [esi + 0x74]
            mov ecx, edi
            call SmudgeCar
            mov eax, dword ptr [edi + 0x1d4]
            test eax, eax
            je L004fbbd9
            mov edx, 0xb
            xor ecx, ecx
            call IRandomBetween
            xor edx, edx
            mov ecx, edi
            mov dx, word ptr [edi + eax*2 + 0xac]
            call SmudgeCar
            jmp L004fbbd9
    L004fbbd4:
            sub eax, ebp
            mov dword ptr [esi + 0x70], eax
    L004fbbd9:
            mov edx, dword ptr [esi + 0x10]
            lea eax, [ebp + edx]
            cmp eax, 0xc8
            mov dword ptr [esi + 0x10], eax
            jbe L004fbdae
            sub eax, ebp
            push 0x3cf5c28f
            mov dword ptr [esi + 0x10], eax
            mov edx, dword ptr [esi + 0x18]
            inc edx
            push 0xbcf5c28f
            mov dword ptr [esi + 0x18], edx
            call SRandomBetween
            mov edi, dword ptr [esi + 0x1c]
            xor eax, eax
            fadd dword ptr [esi + 0x34]
            test edi, edi
            sete al
            mov dword ptr [esp + 0x14], eax
            push 0x3cf5c28f
            fstp dword ptr [esp + 0x40]
            fild dword ptr [esp + 0x18]
            push 0xbcf5c28f
            fmul dword ptr [gDOUBLE_0058b0b0+0xc]
            fsubr dword ptr [esi + 0x38]
            fstp dword ptr [esp + 0x48]
            call SRandomBetween
            fadd dword ptr [esi + 0x3c]
            test byte ptr [esi + 0x20], 2
            fstp dword ptr [esp + 0x44]
            je L004fbc61
            mov edx, 3
            xor ecx, ecx
            call IRandomBetween
            test eax, eax
            jne L004fbc61
            and dword ptr [esi + 0x20], 1
            jmp L004fbc76
    L004fbc61:
            mov eax, dword ptr [esi + 0x20]
            xor ecx, ecx
            cmp eax, 1
            setge cl
            dec ecx
            and ecx, 0xfffffffd
            add ecx, 2
            mov dword ptr [esi + 0x20], ecx
    L004fbc76:
            mov edi, dword ptr [esi + 0x20]
            xor edx, edx
            test edi, edi
            setg dl
            mov dword ptr [esp + 0x14], edx
            mov eax, dword ptr [esi + 0x14]
            fild dword ptr [esp + 0x14]
            cmp eax, 0xfa0
            fsub dword ptr [gDOUBLE_0058b0b0+0x10]
            fmul dword ptr [gFLOAT_0058afec]
            fstp dword ptr [esp + 0x14]
            jae L004fbcc0
            mov dword ptr [esp + 0x18], eax
            mov dword ptr [esp + 0x1c], 0
            fild qword ptr [esp + 0x18]
            fmul dword ptr [esp + 0x14]
            fmul dword ptr [gFLOAT_0058b060+0x14]
            fstp dword ptr [esp + 0x14]
    L004fbcc0:
            mov eax, dword ptr [esi + 8]
            test eax, eax
            je L004fbd7e
            fld dword ptr [ebx + 0x7c]
            fld dword ptr [ebx + 0x78]
            fmul dword ptr [esp + 0x38]
            fxch st(1)
            fmul dword ptr [esp + 0x34]
            lea eax, [esp + 0x24]
            lea ecx, [esp + 0x30]
            fsubp st(1), st(0)
            fstp dword ptr [esp + 0x24]
            fld dword ptr [ebx + 0x74]
            fld dword ptr [ebx + 0x7c]
            fmul dword ptr [esp + 0x30]
            fxch st(1)
            fmul dword ptr [esp + 0x38]
            fsubp st(1), st(0)
            fstp dword ptr [esp + 0x28]
            fld dword ptr [ebx + 0x78]
            fld dword ptr [ebx + 0x74]
            fmul dword ptr [esp + 0x34]
            fxch st(1)
            fmul dword ptr [esp + 0x30]
            fsubp st(1), st(0)
            fstp dword ptr [esp + 0x2c]
            mov edx, dword ptr [ebx]
            add edx, 0x2c
            push edx
            push eax
            push ecx
            call BrMatrix34ApplyV
            fld dword ptr [ebx + 0x68]
            fmul qword ptr [gDOUBLE_0058b0c8]
            fstp dword ptr [esp + 0x30]
            fld dword ptr [ebx + 0x6c]
            fmul qword ptr [gDOUBLE_0058b0c8]
            fld dword ptr [esp + 0x30]
            fadd dword ptr [esp + 0x3c]
            fxch st(1)
            fstp dword ptr [esp + 0x34]
            fld dword ptr [esp + 0x34]
            fld dword ptr [ebx + 0x70]
            fmul qword ptr [gDOUBLE_0058b0c8]
            fxch st(1)
            fadd dword ptr [esp + 0x40]
            fxch st(2)
            fstp dword ptr [esp + 0x30]
            fxch st(1)
            fxch st(1)
            fadd dword ptr [esp + 0x44]
            fxch st(1)
            fsub qword ptr [gDOUBLE_0058b0c8+0x8]
            fxch st(1)
            fstp dword ptr [esp + 0x38]
            add esp, 0xc
            fstp dword ptr [esp + 0x28]
            jmp L004fbd94
    L004fbd7e:
            push 0
            push 0x403981db
            lea eax, [esp + 0x2c]
            push 0
            push eax
            call BrVector3SetFloat
            add esp, 0x10
    L004fbd94:
            mov ecx, dword ptr [esi + 0x1c]
            mov eax, dword ptr [esp + 0x14]
            add ecx, 0x10
            lea edx, [esp + 0x24]
            push ecx
            push eax
            push eax
            lea ecx, [esp + 0x48]
            call CreatePuffOfSmoke
    L004fbdae:
            mov ebx, dword ptr [esp + 0x10]
            add esi, 0x7c
            inc ebx
            cmp esi, offset gSparks
            mov dword ptr [esp + 0x10], ebx
            jl L004fbac1
    L004fbdc6:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x38
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004fcbb0
void C2_NAKED C2_HOOK_FASTCALL GeneralCreateSplash(tPhysics_object* pObject, const br_vector3* pVelocity, const br_vector3* pOmega, const br_vector3* pNormal, br_scalar pWater_distance, const br_bounds3* pBounds, const br_matrix34* pMatrix, tU32 pTime) {
#ifndef CARPOCALYPSE2_MATCHING
    br_vector3 local_normal;
    br_vector3 world_min;
    br_vector3 local_velocity;
    /* Original spills this projection, but passes local_velocity below. */
    volatile br_vector3 normal_velocity;
    br_vector3 point;
    br_vector3 minimum_point;
    br_vector3 maximum_point;
    br_vector3 span;
    br_vector3 side;
    br_vector3 offset;
    br_bounds3 bounds;
    tCar_spec* car;
    br_scalar plane_distance;
    br_scalar minimum_projection;
    br_scalar maximum_projection;
    br_scalar projection;
    br_scalar length;
    br_scalar near_distance;
    br_scalar far_distance;
    br_scalar height;
    double side_wide[3];
    double wide_height;
    double wide_near;
    double coordinate;
    double wide_projection;
    double lower;
    double upper;
    double dot;
    double value;
    double random;
    double second_random;
    double multiplier;
    int start;
    int axis;
    int a;
    int b;
    int corner;
    int pattern;
    int i;

    value = ((double)pVelocity->v[1] * pVelocity->v[1] + (double)pVelocity->v[2] * pVelocity->v[2])
        + (double)pVelocity->v[0] * pVelocity->v[0];
    if (!(value * 47.61000000000001 >= 1.0)) {
        return;
    }
    BrMatrix34TApplyV(&local_normal, pNormal, pMatrix);
    BrMatrix34ApplyP(&world_min, &pBounds->min, pMatrix);
    lower = ((double)pNormal->v[1] * world_min.v[1] + (double)pNormal->v[2] * world_min.v[2])
        + (double)pNormal->v[0] * world_min.v[0] - pWater_distance;
    upper = lower;
    for (i = 0; i < 3; i++) {
        value = ((double)pBounds->max.v[i] - pBounds->min.v[i]) * local_normal.v[i];
        if (local_normal.v[i] > 0.f) {
            upper += value;
        } else {
            lower += value;
        }
    }
    if (lower * upper > 0.0) {
        return;
    }
    dot = ((double)pVelocity->v[1] * pNormal->v[1] + (double)pVelocity->v[2] * pNormal->v[2])
        + (double)pVelocity->v[0] * pNormal->v[0];
    for (i = 0; i < 3; i++) {
        normal_velocity.v[i] = SmokeScalarRound(dot * local_normal.v[i]);
    }
    (void)&normal_velocity;
    memcpy(&bounds, pBounds, sizeof(bounds));
    bounds.min.v[1] = .01f;
    BrMatrix34TApplyV(&local_velocity, pVelocity, pMatrix);
    plane_distance = SmokeScalarRound((double)pWater_distance
        - (((double)pMatrix->m[3][2] * pNormal->v[2] + (double)pMatrix->m[3][1] * pNormal->v[1])
            + (double)pMatrix->m[3][0] * pNormal->v[0]));
    minimum_projection = 0.f;
    maximum_projection = 0.f;
    start = IRandomBetween(0, 3);
    for (axis = 0; axis < 3; axis++) {
        a = (axis + 1) % 3;
        b = (axis + 2) % 3;
        for (corner = 0; corner < 4; corner++) {
            pattern = start ^ corner;
            point.v[a] = SmokeScalarRound((pattern & 1) ? bounds.min.v[a] : bounds.max.v[a]);
            point.v[b] = SmokeScalarRound((pattern & 2) ? bounds.min.v[b] : bounds.max.v[b]);
            coordinate = (((double)plane_distance - (double)point.v[a] * local_normal.v[a])
                - (double)point.v[b] * local_normal.v[b]) / local_normal.v[axis];
            value = SmokeScalarRound(coordinate);
            if (!(coordinate >= bounds.min.v[axis]) || value > bounds.max.v[axis]) {
                continue;
            }
            point.v[axis] = SmokeScalarRound(value);
            wide_projection = ((double)local_velocity.v[0] * point.v[0] + (double)local_velocity.v[1] * point.v[1])
                + (double)local_velocity.v[2] * point.v[2];
            projection = SmokeScalarRound(wide_projection);
            if (!(wide_projection >= minimum_projection)) {
                if (!(maximum_projection >= 0.f)) {
                    SingleSplash(pObject, pMatrix, pVelocity, &local_velocity, pOmega, &maximum_point, &local_normal, pTime);
                }
                maximum_projection = minimum_projection;
                minimum_projection = projection;
                memcpy(&maximum_point, &minimum_point, sizeof(maximum_point));
                memcpy(&minimum_point, &point, sizeof(minimum_point));
            } else if (!(projection >= maximum_projection)) {
                if (!(maximum_projection >= 0.f)) {
                    SingleSplash(pObject, pMatrix, pVelocity, &local_velocity, pOmega, &maximum_point, &local_normal, pTime);
                }
                maximum_projection = projection;
                memcpy(&maximum_point, &point, sizeof(maximum_point));
            } else {
                SingleSplash(pObject, pMatrix, pVelocity, &local_velocity, pOmega, &point, &local_normal, pTime);
            }
        }
    }
    if (!(maximum_projection >= 0.f)) {
        for (i = 0; i < 3; i++) {
            span.v[i] = SmokeScalarRound((double)maximum_point.v[i] - minimum_point.v[i]);
        }
        random = SRandomBetween(.4f, .6f);
        point.v[0] = SmokeScalarRound((double)minimum_point.v[0] + random * span.v[0]);
        point.v[1] = SmokeScalarRound((double)minimum_point.v[1] + SmokeScalarRound(random * span.v[1]));
        point.v[2] = SmokeScalarRound((double)minimum_point.v[2] + SmokeScalarRound(random * span.v[2]));
        random = SRandomBetween(.2f, .3f);
        for (i = 0; i < 3; i++) {
            minimum_point.v[i] = SmokeScalarRound(random * span.v[i] + minimum_point.v[i]);
        }
        second_random = SRandomBetween(.2f, .3f);
        random = SmokeScalarRound(-second_random);
        dot = -(((double)local_velocity.v[2] * local_normal.v[2] + (double)local_velocity.v[1] * local_normal.v[1])
            + (double)local_velocity.v[0] * local_normal.v[0]);
        for (i = 0; i < 3; i++) {
            side_wide[i] = (double)local_velocity.v[i] + SmokeScalarRound((double)local_normal.v[i] * dot);
            side.v[i] = SmokeScalarRound(side_wide[i]);
        }
        length = SmokeScalarRound(sqrt((side_wide[0] * side.v[0] + side_wide[1] * side.v[1])
            + (double)side.v[2] * side.v[2]));
        if (!(length > 2.384185791015625e-7f)) {
            side.v[0] = 1.f;
            side.v[1] = 0.f;
            side.v[2] = 0.f;
            side_wide[0] = 1.0;
            side_wide[1] = 0.0;
            side_wide[2] = 0.0;
        } else {
            multiplier = 1.0 / length;
            for (i = 0; i < 3; i++) {
                side_wide[i] = multiplier * side.v[i];
                side.v[i] = SmokeScalarRound(side_wide[i]);
                if (i == 2) {
                    side_wide[i] = side.v[i];
                }
            }
        }
        for (i = 0; i < 3; i++) {
            value = side_wide[i] * -.028985507786273956f;
            offset.v[i] = SmokeScalarRound(value * .5f);
            point.v[i] = SmokeScalarRound((double)point.v[i] + SmokeScalarRound(value));
            minimum_point.v[i] = SmokeScalarRound((double)minimum_point.v[i] + offset.v[i]);
            maximum_point.v[i] = SmokeScalarRound((double)maximum_point.v[i]
                + (i == 0 ? -second_random : random) * span.v[i]);
            maximum_point.v[i] = SmokeScalarRound((double)maximum_point.v[i] + offset.v[i]);
        }
        SingleSplash(pObject, pMatrix, pVelocity, &local_velocity, pOmega, &point, &local_normal, pTime);
        SingleSplash(pObject, pMatrix, pVelocity, &local_velocity, pOmega, &minimum_point, &local_normal, pTime);
        SingleSplash(pObject, pMatrix, pVelocity, &local_velocity, pOmega, &maximum_point, &local_normal, pTime);
    } else if (!(minimum_projection >= 0.f)) {
        SingleSplash(pObject, pMatrix, pVelocity, &local_velocity, pOmega, &minimum_point, &local_normal, pTime);
    }
    plane_distance = SmokeScalarRound((double)plane_distance * 6.900000095367432f);
    if (pObject != NULL && pObject->flags_0x238 == 1 && pObject->owner != NULL) {
        car = pObject->owner;
        if (car->driver > 5) {
            for (i = 0; i < 4; i++) {
                value = (i & 1) ? pObject->shape->common.bb.max.v[0] : pObject->shape->common.bb.min.v[0];
                wide_height = (double)pObject->shape->common.bb.max.v[1] * 6.9;
                height = SmokeScalarRound(wide_height);
                wide_near = (double)plane_distance
                    - (((double)car->wpos[i].v[2] * local_normal.v[2] + wide_height * local_normal.v[1])
                        + value * 6.9 * local_normal.v[0]);
                near_distance = SmokeScalarRound(wide_near);
                far_distance = SmokeScalarRound(((double)pObject->shape->common.bb.max.v[1] - .01f)
                    * local_normal.v[1] * 6.900000095367432f + near_distance);
                if ((wide_near > 0.f) != (far_distance > 0.f)) {
                    point.v[0] = SmokeScalarRound(value * 6.9 * .14492753148078918f);
                    point.v[1] = SmokeScalarRound(((double)height - (wide_height - .01)
                        * ((double)near_distance / ((double)near_distance - far_distance))) * .14492753148078918f);
                    point.v[2] = SmokeScalarRound((double)car->wpos[i].v[2] * .14492753148078918f);
                    SingleSplash(pObject, pMatrix, pVelocity, &local_velocity, pOmega, &point, &local_normal, pTime);
                }
            }
        }
    }
#else
    __asm {
            sub esp, 0x88
            push ebx
            mov ebx, edx
            push ebp
            push esi
            fld dword ptr [ebx + 4]
            fld dword ptr [ebx + 8]
            fld dword ptr [ebx]
            fxch st(2)
            fst dword ptr [esp + 0x18]
            fxch st(1)
            fst dword ptr [esp + 0x24]
            fxch st(1)
            fmul dword ptr [esp + 0x18]
            fxch st(1)
            fmul dword ptr [esp + 0x24]
            fld st(2)
            fmul st(0), st(3)
            fxch st(1)
            faddp st(2), st(0)
            push edi
            mov dword ptr [esp + 0x3c], 0
            mov dword ptr [esp + 0x40], 0
            mov dword ptr [esp + 0x24], ecx
            faddp st(1), st(0)
            fmul qword ptr [gDOUBLE_0058b108]
            fcomp qword ptr [gDOUBLE_0058b010]
            fnstsw ax
            test ah, 1
            fstp st(0)
            jne L004fd522
            mov ebp, dword ptr [esp + 0xac]
            mov esi, dword ptr [esp + 0xa0]
            push ebp
            lea eax, [esp + 0x4c]
            push esi
            push eax
            call BrMatrix34TApplyV
            mov edi, dword ptr [esp + 0xb4]
            add esp, 0xc
            lea ecx, [esp + 0x10]
            push ebp
            push edi
            push ecx
            call BrMatrix34ApplyP
            fld dword ptr [esi + 4]
            fmul dword ptr [esp + 0x20]
            fld dword ptr [esi + 8]
            fmul dword ptr [esp + 0x24]
            lea edx, [esp + 0x54]
            add esp, 0xc
            mov ecx, edi
            sub edx, edi
            faddp st(1), st(0)
            fld dword ptr [esp + 0x10]
            fmul dword ptr [esi]
            mov dword ptr [esp + 0x1c], 3
            faddp st(1), st(0)
            fsub dword ptr [esp + 0xa4]
            fld st(0)
    L004fcc77:
            fld dword ptr [edx + ecx]
            fcomp dword ptr [gFLOAT_0058b008]
            fld dword ptr [ecx + 0xc]
            fsub dword ptr [ecx]
            fmul dword ptr [edx + ecx]
            fnstsw ax
            test ah, 0x41
            jne L004fcc93
            faddp st(2), st(0)
            jmp L004fcc95
    L004fcc93:
            faddp st(1), st(0)
    L004fcc95:
            mov eax, dword ptr [esp + 0x1c]
            add ecx, 4
            dec eax
            mov dword ptr [esp + 0x1c], eax
            jne L004fcc77
            fxch st(1)
            fmul st(0), st(1)
            fcomp dword ptr [gFLOAT_0058b008]
            fnstsw ax
            test ah, 0x41
            fstp st(0)
            je L004fd522
            fld dword ptr [ebx + 4]
            fld dword ptr [esi + 4]
            fld dword ptr [ebx + 8]
            fld dword ptr [ebx]
            fxch st(2)
            fmulp st(3), st(0)
            mov ecx, dword ptr [edi + 0xc]
            mov edx, dword ptr [edi]
            mov eax, dword ptr [edi + 8]
            mov dword ptr [esp + 0x8c], ecx
            fmul dword ptr [esi + 8]
            fxch st(1)
            fmul dword ptr [esi]
            fxch st(1)
            faddp st(2), st(0)
            mov dword ptr [esp + 0x80], edx
            mov edx, dword ptr [edi + 0x10]
            mov dword ptr [esp + 0x88], eax
            mov eax, dword ptr [edi + 0x14]
            faddp st(1), st(0)
            push ebp
            lea ecx, [esp + 0x70]
            push ebx
            push ecx
            fld st(0)
            fld st(1)
            fxch st(1)
            fmul dword ptr [esp + 0x54]
            fxch st(1)
            fmul dword ptr [esp + 0x58]
            fxch st(2)
            fmul dword ptr [esp + 0x5c]
            fxch st(1)
            fstp dword ptr [esp + 0x1c]
            fxch st(1)
            fstp dword ptr [esp + 0x20]
            fstp dword ptr [esp + 0x24]
            mov dword ptr [esp + 0x9c], edx
            mov dword ptr [esp + 0xa0], eax
            mov dword ptr [esp + 0x90], 0x3c23d70a
            call BrMatrix34TApplyV
            fld dword ptr [ebp + 0x2c]
            fld dword ptr [esi + 8]
            fld dword ptr [ebp + 0x28]
            fld dword ptr [ebp + 0x24]
            fxch st(2)
            fmulp st(3), st(0)
            fmul dword ptr [esi + 4]
            fxch st(1)
            fmul dword ptr [esi]
            fxch st(1)
            faddp st(2), st(0)
            add esp, 0xc
            mov edx, 3
            xor ecx, ecx
            faddp st(1), st(0)
            fsubr dword ptr [esp + 0xa4]
            fstp dword ptr [esp + 0x78]
            call IRandomBetween
            xor edx, edx
            mov dword ptr [esp + 0x7c], eax
            mov ecx, 2
            mov dword ptr [esp + 0x2c], edx
            mov dword ptr [esp + 0x1c], 3
    L004fcd92:
            mov eax, dword ptr [esp + 0x1c]
            mov dword ptr [esp + 0x28], 0
            sub eax, ecx
            lea esi, [ecx*4]
            lea edi, [eax*4]
    L004fcdae:
            mov eax, dword ptr [esp + 0x7c]
            mov ecx, dword ptr [esp + 0x28]
            xor eax, ecx
            test al, 1
            je L004fcdc5
            fld dword ptr [esp + edi + 0x80]
            jmp L004fcdcc
    L004fcdc5:
            fld dword ptr [esp + edi + 0x8c]
    L004fcdcc:
            fstp dword ptr [esp + edi + 0x30]
            test al, 2
            je L004fcddd
            fld dword ptr [esp + esi + 0x80]
            jmp L004fcde4
    L004fcddd:
            fld dword ptr [esp + esi + 0x8c]
    L004fcde4:
            fld st(0)
            fld dword ptr [esp + edi + 0x48]
            fxch st(1)
            fstp dword ptr [esp + esi + 0x30]
            fld dword ptr [esp + esi + 0x48]
            fld dword ptr [esp + edi + 0x30]
            fmulp st(2), st(0)
            fxch st(1)
            fsubr dword ptr [esp + 0x78]
            fxch st(2)
            fmulp st(1), st(0)
            fsubp st(1), st(0)
            fdiv dword ptr [esp + edx*4 + 0x48]
            fst dword ptr [esp + 0x20]
            fcomp dword ptr [esp + edx*4 + 0x80]
            fnstsw ax
            test ah, 1
            jne L004fcf9e
            fld dword ptr [esp + 0x20]
            fcomp dword ptr [esp + edx*4 + 0x8c]
            fnstsw ax
            test ah, 0x41
            je L004fcf9e
            fld dword ptr [esp + 0x20]
            fld dword ptr [esp + 0x6c]
            fld dword ptr [esp + 0x70]
            fld dword ptr [esp + 0x74]
            fxch st(3)
            fstp dword ptr [esp + edx*4 + 0x30]
            fxch st(1)
            fmul dword ptr [esp + 0x30]
            fxch st(1)
            fmul dword ptr [esp + 0x34]
            fxch st(2)
            fmul dword ptr [esp + 0x38]
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fst dword ptr [esp + 0x20]
            fcomp dword ptr [esp + 0x3c]
            fnstsw ax
            test ah, 1
            je L004fcefe
            fld dword ptr [esp + 0x40]
            fcomp dword ptr [gFLOAT_0058b008]
            fnstsw ax
            test ah, 1
            je L004fceb9
            mov edx, dword ptr [esp + 0xb0]
            lea eax, [esp + 0x48]
            push edx
            mov edx, dword ptr [esp + 0xa0]
            lea ecx, [esp + 0x64]
            push eax
            push ecx
            mov ecx, dword ptr [esp + 0x30]
            lea eax, [esp + 0x78]
            push edx
            push eax
            push ebx
            mov edx, ebp
            call SingleSplash
            mov edx, dword ptr [esp + 0x2c]
    L004fceb9:
            mov ecx, dword ptr [esp + 0x3c]
            mov eax, dword ptr [esp + 0x20]
            mov dword ptr [esp + 0x40], ecx
            mov ecx, dword ptr [esp + 0x54]
            mov dword ptr [esp + 0x3c], eax
            mov eax, dword ptr [esp + 0x58]
            mov dword ptr [esp + 0x60], ecx
            mov ecx, dword ptr [esp + 0x5c]
            mov dword ptr [esp + 0x64], eax
            mov eax, dword ptr [esp + 0x30]
            mov dword ptr [esp + 0x68], ecx
            mov ecx, dword ptr [esp + 0x34]
            mov dword ptr [esp + 0x54], eax
            mov eax, dword ptr [esp + 0x38]
            mov dword ptr [esp + 0x58], ecx
            mov dword ptr [esp + 0x5c], eax
            jmp L004fcf9e
    L004fcefe:
            fld dword ptr [esp + 0x20]
            fcomp dword ptr [esp + 0x40]
            fnstsw ax
            test ah, 1
            je L004fcf6f
            fld dword ptr [esp + 0x40]
            fcomp dword ptr [gFLOAT_0058b008]
            fnstsw ax
            test ah, 1
            je L004fcf4d
            mov ecx, dword ptr [esp + 0xb0]
            lea edx, [esp + 0x48]
            push ecx
            mov ecx, dword ptr [esp + 0xa0]
            lea eax, [esp + 0x64]
            push edx
            push eax
            lea edx, [esp + 0x78]
            push ecx
            mov ecx, dword ptr [esp + 0x34]
            push edx
            push ebx
            mov edx, ebp
            call SingleSplash
            mov edx, dword ptr [esp + 0x2c]
    L004fcf4d:
            mov eax, dword ptr [esp + 0x20]
            mov ecx, dword ptr [esp + 0x30]
            mov dword ptr [esp + 0x40], eax
            mov eax, dword ptr [esp + 0x34]
            mov dword ptr [esp + 0x60], ecx
            mov ecx, dword ptr [esp + 0x38]
            mov dword ptr [esp + 0x64], eax
            mov dword ptr [esp + 0x68], ecx
            jmp L004fcf9e
    L004fcf6f:
            mov edx, dword ptr [esp + 0xb0]
            lea eax, [esp + 0x48]
            push edx
            mov edx, dword ptr [esp + 0xa0]
            lea ecx, [esp + 0x34]
            push eax
            push ecx
            mov ecx, dword ptr [esp + 0x30]
            lea eax, [esp + 0x78]
            push edx
            push eax
            push ebx
            mov edx, ebp
            call SingleSplash
            mov edx, dword ptr [esp + 0x2c]
    L004fcf9e:
            mov eax, dword ptr [esp + 0x28]
            inc eax
            cmp eax, 4
            mov dword ptr [esp + 0x28], eax
            jl L004fcdae
            mov eax, dword ptr [esp + 0x1c]
            mov ecx, edx
            inc edx
            dec eax
            test eax, eax
            mov dword ptr [esp + 0x2c], edx
            mov dword ptr [esp + 0x1c], eax
            jg L004fcd92
            fld dword ptr [esp + 0x40]
            fcomp dword ptr [gFLOAT_0058b008]
            fnstsw ax
            test ah, 1
            je L004fd346
            fld dword ptr [esp + 0x60]
            fld dword ptr [esp + 0x64]
            fld dword ptr [esp + 0x68]
            fxch st(2)
            fsub dword ptr [esp + 0x54]
            fxch st(1)
            fsub dword ptr [esp + 0x58]
            fxch st(2)
            fsub dword ptr [esp + 0x5c]
            fxch st(1)
            fstp dword ptr [esp + 0x10]
            fxch st(1)
            fstp dword ptr [esp + 0x14]
            push 0x3f19999a
            push 0x3ecccccd
            fstp dword ptr [esp + 0x20]
            call SRandomBetween
            fld st(0)
            fld st(1)
            fxch st(1)
            fmul dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x54]
            fxch st(2)
            fmul dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x58]
            fxch st(4)
            fmul dword ptr [esp + 0x18]
            fld dword ptr [esp + 0x5c]
            fxch st(3)
            fxch st(5)
            fxch st(4)
            fxch st(3)
            fxch st(2)
            fxch st(1)
            fxch st(3)
            fadd st(0), st(5)
            fxch st(1)
            fstp dword ptr [esp + 0x34]
            fxch st(3)
            fadd dword ptr [esp + 0x34]
            fxch st(3)
            fstp dword ptr [esp + 0x30]
            fxch st(1)
            fstp dword ptr [esp + 0x38]
            push 0x3e99999a
            push 0x3e4ccccd
            fadd dword ptr [esp + 0x40]
            fxch st(1)
            fstp dword ptr [esp + 0x3c]
            fstp dword ptr [esp + 0x40]
            fstp st(0)
            call SRandomBetween
            fld st(0)
            fld st(1)
            fxch st(1)
            fmul dword ptr [esp + 0x10]
            fxch st(1)
            fmul dword ptr [esp + 0x14]
            fxch st(2)
            fmul dword ptr [esp + 0x18]
            fxch st(1)
            fadd dword ptr [esp + 0x54]
            fxch st(2)
            fadd dword ptr [esp + 0x58]
            fxch st(1)
            fadd dword ptr [esp + 0x5c]
            fxch st(2)
            fstp dword ptr [esp + 0x54]
            push 0x3e99999a
            push 0x3e4ccccd
            fstp dword ptr [esp + 0x60]
            fstp dword ptr [esp + 0x64]
            call SRandomBetween
            fld dword ptr [esp + 0x70]
            fld dword ptr [esp + 0x6c]
            fmul dword ptr [esp + 0x48]
            fxch st(2)
            fchs
            fxch st(1)
            fmul dword ptr [esp + 0x4c]
            fld dword ptr [esp + 0x74]
            fxch st(2)
            fst dword ptr [esp + 0x20]
            fld dword ptr [esp + 0x20]
            fld dword ptr [esp + 0x20]
            fxch st(2)
            fmul dword ptr [esp + 0x10]
            fadd dword ptr [esp + 0x60]
            fstp dword ptr [esp + 0x60]
            fmul dword ptr [esp + 0x14]
            fxch st(2)
            faddp st(4), st(0)
            fmul dword ptr [esp + 0x18]
            fxch st(2)
            fmul dword ptr [esp + 0x50]
            fxch st(1)
            fadd dword ptr [esp + 0x64]
            fxch st(2)
            fadd dword ptr [esp + 0x68]
            fxch st(1)
            faddp st(3), st(0)
            fxch st(1)
            fstp dword ptr [esp + 0x64]
            fstp dword ptr [esp + 0x68]
            fld dword ptr [esp + 0x48]
            fld dword ptr [esp + 0x4c]
            fxch st(2)
            fchs
            fxch st(2)
            fxch st(1)
            fmul st(0), st(2)
            fld dword ptr [esp + 0x50]
            fxch st(2)
            fmul st(0), st(3)
            fld dword ptr [esp + 0x6c]
            fxch st(3)
            fmul st(0), st(4)
            fld dword ptr [esp + 0x70]
            fld dword ptr [esp + 0x74]
            fxch st(4)
            fstp dword ptr [esp + 0x10]
            fxch st(4)
            fadd dword ptr [esp + 0x10]
            fxch st(2)
            fstp dword ptr [esp + 0x14]
            fxch st(3)
            fadd dword ptr [esp + 0x14]
            fxch st(3)
            fstp dword ptr [esp + 0x18]
            fxch st(1)
            fadd dword ptr [esp + 0x18]
            fxch st(1)
            fst dword ptr [esp + 0x3c]
            fxch st(2)
            fst dword ptr [esp + 0x40]
            fxch st(2)
            fmul dword ptr [esp + 0x3c]
            fxch st(1)
            fstp dword ptr [esp + 0x44]
            fxch st(1)
            fmul dword ptr [esp + 0x40]
            fld dword ptr [esp + 0x44]
            fmul dword ptr [esp + 0x44]
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fsqrt
            fstp dword ptr [esp + 0x7c]
            fstp st(0)
            fld dword ptr [esp + 0x7c]
            fcom dword ptr [gFLOAT_0058b110]
            fnstsw ax
            test ah, 0x41
            jne L004fd1e5
            fdivr qword ptr [gDOUBLE_0058b010]
            fld st(0)
            fld st(1)
            fxch st(1)
            fmul dword ptr [esp + 0x3c]
            fxch st(1)
            fmul dword ptr [esp + 0x40]
            fxch st(2)
            fmul dword ptr [esp + 0x44]
            fxch st(1)
            fxch st(2)
            fxch st(1)
            fstp dword ptr [esp + 0x18]
            jmp L004fd1fb
    L004fd1e5:
            fstp st(0)
            fld dword ptr [gFLOAT_0058afdc]
            fld dword ptr [gFLOAT_0058b008]
            mov dword ptr [esp + 0x18], 0
    L004fd1fb:
            fld st(0)
            fld dword ptr [esp + 0x18]
            fxch st(3)
            fmul dword ptr [gFLOAT_0058b114]
            fld dword ptr [esp + 0x54]
            fxch st(2)
            fmul dword ptr [gFLOAT_0058b114]
            fxch st(4)
            fmul dword ptr [gFLOAT_0058b114]
            fxch st(1)
            fst dword ptr [esp + 0x10]
            fmul dword ptr [gFLOAT_0058afec]
            fxch st(4)
            fst dword ptr [esp + 0x14]
            fmul dword ptr [gFLOAT_0058afec]
            fxch st(1)
            fst dword ptr [esp + 0x18]
            fmul dword ptr [gFLOAT_0058afec]
            fld dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x10]
            fadd dword ptr [esp + 0x30]
            mov edi, dword ptr [esp + 0xb0]
            mov eax, dword ptr [esp + 0x9c]
            mov esi, dword ptr [esp + 0x24]
            lea ecx, [esp + 0x48]
            push edi
            lea edx, [esp + 0x34]
            fstp dword ptr [esp + 0x34]
            fld dword ptr [esp + 0x1c]
            fxch st(1)
            fadd dword ptr [esp + 0x38]
            push ecx
            push edx
            lea ecx, [esp + 0x78]
            push eax
            push ecx
            mov edx, ebp
            fstp dword ptr [esp + 0x48]
            fld dword ptr [esp + 0x6c]
            fxch st(1)
            fadd dword ptr [esp + 0x4c]
            fstp dword ptr [esp + 0x4c]
            fld dword ptr [esp + 0x70]
            fxch st(6)
            fstp dword ptr [esp + 0x24]
            fxch st(3)
            fadd dword ptr [esp + 0x24]
            fxch st(2)
            fstp dword ptr [esp + 0x28]
            fxch st(2)
            fadd dword ptr [esp + 0x28]
            fxch st(1)
            fstp dword ptr [esp + 0x68]
            fxch st(1)
            fstp dword ptr [esp + 0x2c]
            fxch st(2)
            fadd dword ptr [esp + 0x2c]
            fxch st(2)
            fstp dword ptr [esp + 0x6c]
            fld dword ptr [esp + 0x74]
            fadd dword ptr [esp + 0x24]
            fxch st(2)
            fstp dword ptr [esp + 0x70]
            fld dword ptr [esp + 0x78]
            fadd dword ptr [esp + 0x28]
            fxch st(2)
            fstp dword ptr [esp + 0x74]
            fld dword ptr [esp + 0x7c]
            fadd dword ptr [esp + 0x2c]
            fxch st(2)
            fstp dword ptr [esp + 0x78]
            fxch st(1)
            fstp dword ptr [esp + 0x7c]
            fstp st(0)
            push ebx
            mov ecx, esi
            call SingleSplash
            lea edx, [esp + 0x48]
            lea eax, [esp + 0x54]
            push edi
            push edx
            mov edx, dword ptr [esp + 0xa4]
            push eax
            lea ecx, [esp + 0x78]
            push edx
            push ecx
            push ebx
            mov edx, ebp
            mov ecx, esi
            call SingleSplash
            lea eax, [esp + 0x48]
            lea ecx, [esp + 0x60]
            mov edx, dword ptr [esp + 0x9c]
            push edi
            push eax
            push ecx
            lea eax, [esp + 0x78]
            push edx
            push eax
            push ebx
            mov edx, ebp
            mov ecx, esi
            call SingleSplash
            jmp L004fd38d
    L004fd346:
            fld dword ptr [esp + 0x3c]
            fcomp dword ptr [gFLOAT_0058b008]
            fnstsw ax
            test ah, 1
            je L004fd382
            mov ecx, dword ptr [esp + 0xb0]
            lea edx, [esp + 0x48]
            push ecx
            mov ecx, dword ptr [esp + 0xa0]
            lea eax, [esp + 0x58]
            push edx
            push eax
            lea edx, [esp + 0x78]
            push ecx
            mov ecx, dword ptr [esp + 0x34]
            push edx
            push ebx
            mov edx, ebp
            call SingleSplash
    L004fd382:
            mov esi, dword ptr [esp + 0x24]
            mov edi, dword ptr [esp + 0xb0]
    L004fd38d:
            fld dword ptr [esp + 0x78]
            fmul dword ptr [gFLOAT_0058b054]
            test esi, esi
            fstp dword ptr [esp + 0x78]
            je L004fd522
            mov eax, dword ptr [esi + 0x23c]
            test eax, eax
            je L004fd3b6
            cmp byte ptr [esi + 0x238], 1
            je L004fd3b8
    L004fd3b6:
            xor eax, eax
    L004fd3b8:
            test eax, eax
            je L004fd522
            cmp dword ptr [eax + 0xc], 5
            jle L004fd522
            xor edx, edx
            add eax, 0x122c
            mov dword ptr [esp + 0x24], edx
            mov dword ptr [esp + 0x1c], eax
    L004fd3d9:
            mov eax, dword ptr [esi + 0x20]
            test dl, 1
            je L004fd3e6
            fld dword ptr [eax + 0x10]
            jmp L004fd3e9
    L004fd3e6:
            fld dword ptr [eax + 4]
    L004fd3e9:
            fld dword ptr [eax + 0x14]
            mov ecx, dword ptr [esp + 0x1c]
            fmul qword ptr [gDOUBLE_0058b0c8]
            fld dword ptr [ecx]
            fxch st(2)
            fmul qword ptr [gDOUBLE_0058b0c8]
            fxch st(1)
            fld st(0)
            fld st(2)
            fxch st(1)
            fst dword ptr [esp + 0x40]
            fxch st(1)
            fmul dword ptr [esp + 0x48]
            fld st(4)
            fxch st(2)
            fmul dword ptr [esp + 0x4c]
            fld dword ptr [eax + 0x14]
            fxch st(3)
            fmul dword ptr [esp + 0x50]
            fxch st(1)
            faddp st(2), st(0)
            fxch st(2)
            fsub dword ptr [gFLOAT_0058b028]
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            fmul dword ptr [esp + 0x4c]
            fxch st(1)
            fsubr dword ptr [esp + 0x78]
            fxch st(1)
            fmul dword ptr [gFLOAT_0058b054]
            fxch st(1)
            fst dword ptr [esp + 0x2c]
            fcomp dword ptr [gFLOAT_0058b008]
            fadd dword ptr [esp + 0x2c]
            fnstsw ax
            test ah, 0x41
            fstp dword ptr [esp + 0x28]
            fxch st(1)
            jne L004fd46b
            mov ecx, 1
            jmp L004fd46d
    L004fd46b:
            xor ecx, ecx
    L004fd46d:
            fld dword ptr [esp + 0x28]
            fcomp dword ptr [gFLOAT_0058b008]
            fnstsw ax
            test ah, 0x41
            jne L004fd485
            mov eax, 1
            jmp L004fd487
    L004fd485:
            xor eax, eax
    L004fd487:
            cmp ecx, eax
            je L004fd503
            fld dword ptr [esp + 0x2c]
            fsub dword ptr [esp + 0x28]
            fxch st(2)
            fsub qword ptr [gDOUBLE_0058b118]
            fxch st(2)
            fdivr dword ptr [esp + 0x2c]
            lea edx, [esp + 0x48]
            push edi
            lea eax, [esp + 0x14]
            push edx
            mov edx, dword ptr [esp + 0xa4]
            push eax
            lea ecx, [esp + 0x78]
            push edx
            push ecx
            push ebx
            mov edx, ebp
            mov ecx, esi
            fld dword ptr [esp + 0x58]
            fxch st(3)
            fmulp st(1), st(0)
            fld st(1)
            fxch st(1)
            fsubp st(3), st(0)
            fmul dword ptr [gFLOAT_0058b060]
            fxch st(2)
            fxch st(3)
            fxch st(3)
            fmul dword ptr [gFLOAT_0058b060]
            fxch st(3)
            fmul dword ptr [gFLOAT_0058b060]
            fxch st(2)
            fstp dword ptr [esp + 0x28]
            fxch st(2)
            fstp dword ptr [esp + 0x2c]
            fstp dword ptr [esp + 0x30]
            fstp st(0)
            call SingleSplash
            mov edx, dword ptr [esp + 0x24]
            jmp L004fd509
    L004fd503:
            fstp st(0)
            fstp st(0)
            fstp st(0)
    L004fd509:
            mov ecx, dword ptr [esp + 0x1c]
            inc edx
            add ecx, 0xc
            cmp edx, 4
            mov dword ptr [esp + 0x24], edx
            mov dword ptr [esp + 0x1c], ecx
            jl L004fd3d9
    L004fd522:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x88
            ret 0x18
            nop
    }
#endif
}

#pragma inline_depth(0)
// FUNCTION: CARMA2_HW 0x004fda40
void C2_HOOK_FASTCALL CreateSplash(tPhysics_object* pObject, tU32 pTime) {
    GeneralCreateSplash(pObject, &pObject->v, &pObject->omega, &pObject->water_normal, pObject->water_d,
        &pObject->bb1, &pObject->actor->t.t.mat, pTime);
}
#pragma inline_depth()

// FUNCTION: CARMA2_HW 0x004fda70
void C2_NAKED C2_HOOK_FASTCALL GetVelocitiesFromMatrices(br_vector3* pVel, br_vector3* pRot, br_matrix34* pMat_now, br_matrix34* pMat_prev, tU32 pTime) {
#ifndef CARPOCALYPSE2_MATCHING
    br_quat quat_now;
    br_quat quat_prev;
    br_quat quat_speed;
    br_scalar ts;

    ts = (float)pTime / 1000.f;
    BrVector3Sub(pVel, (br_vector3*)pMat_prev->m[3], (br_vector3*)pMat_now->m[3]);
    BrVector3InvScale(pVel, pVel, ts);
    BrMatrix34ToQuat(&quat_now, pMat_now);
    BrMatrix34ToQuat(&quat_prev, pMat_now);
    BrQuatInvert(&quat_now, &quat_now);
    BrQuatMul(&quat_speed, &quat_now, &quat_prev);
    pRot->v[0] = 2 * quat_speed.x / ts;
    pRot->v[1] = 2 * quat_speed.y / ts;
    pRot->v[2] = 2 * quat_speed.z / ts;
#else
    __asm {
            sub esp, 0x30
            mov eax, dword ptr [esp + 0x38]
            push esi
            mov esi, dword ptr [esp + 0x38]
            push edi
            fld dword ptr [eax + 0x24]
            fsub dword ptr [esi + 0x24]
            mov edi, edx
            mov dword ptr [esp + 0xc], 0
            mov edx, dword ptr [esp + 0x44]
            push esi
            mov dword ptr [esp + 0xc], edx
            lea edx, [esp + 0xc]
            fild qword ptr [esp + 0xc]
            fxch st(1)
            _emit 0xd9
        _emit 0x19
            fld dword ptr [eax + 0x28]
            fsub dword ptr [esi + 0x28]
            fxch st(1)
            fmul qword ptr [gFLOAT_0058b130+0x8]
            _emit 0xd9
        _emit 0x01
            fxch st(2)
            fstp dword ptr [ecx + 4]
            fld dword ptr [eax + 0x2c]
            fsub dword ptr [esi + 0x2c]
            fld dword ptr [ecx + 4]
            fxch st(2)
            fstp dword ptr [esp + 0x44]
            fxch st(2)
            fdiv dword ptr [esp + 0x44]
            fxch st(2)
            push edx
            fstp dword ptr [ecx + 8]
            fld dword ptr [ecx + 8]
            fxch st(1)
            fdiv dword ptr [esp + 0x48]
            fxch st(1)
            fdiv dword ptr [esp + 0x48]
            fxch st(2)
            _emit 0xd9
        _emit 0x19
            fstp dword ptr [ecx + 4]
            fstp dword ptr [ecx + 8]
            call BrMatrix34ToQuat
            add esp, 8
            lea eax, [esp + 0x28]
            push esi
            push eax
            call BrMatrix34ToQuat
            add esp, 8
            lea ecx, [esp + 8]
            lea edx, [esp + 8]
            push ecx
            push edx
            call BrQuatInvert
            add esp, 8
            lea eax, [esp + 0x28]
            lea ecx, [esp + 8]
            lea edx, [esp + 0x18]
            push eax
            push ecx
            push edx
            call BrQuatMul
            fld dword ptr [esp + 0x24]
            fld dword ptr [esp + 0x28]
            fld dword ptr [esp + 0x2c]
            fxch st(2)
            fadd st(0), st(0)
            fxch st(1)
            fadd st(0), st(0)
            fxch st(1)
            fdiv dword ptr [esp + 0x4c]
            fxch st(2)
            add esp, 0xc
            fadd st(0), st(0)
            fxch st(1)
            fdiv dword ptr [esp + 0x40]
            fxch st(1)
            fdiv dword ptr [esp + 0x40]
            fxch st(2)
            _emit 0xd9
        _emit 0x1f
            fstp dword ptr [edi + 4]
            fstp dword ptr [edi + 8]
            pop edi
            pop esi
            add esp, 0x30
            ret 0xc
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

// FUNCTION: CARMA2_HW 0x004f9e80
void C2_NAKED C2_HOOK_FASTCALL DrMatrix34Rotate(br_matrix34* mat, br_angle r, br_vector3* a) {
#ifndef CARPOCALYPSE2_MATCHING
    br_scalar t;
    br_scalar s;
    br_scalar c;
    br_scalar txy;
    br_scalar txz;
    br_scalar tyz;
    br_scalar sx;
    br_scalar sy;
    br_scalar sz;

    s = FastScalarSinAngle(r);
    c = FastScalarCosAngle(r);
    t = 1.0f - c;
    txy = t * a->v[0] * a->v[1];
    txz = t * a->v[0] * a->v[2];
    tyz = t * a->v[1] * a->v[2];
    sx = a->v[0] * s;
    sy = a->v[1] * s;
    sz = a->v[2] * s;
    mat->m[0][0] = a->v[0] * a->v[0] * t + c;
    mat->m[0][1] = sz + txy;
    mat->m[0][2] = txz - sy;
    mat->m[1][0] = txy - sz;
    mat->m[1][1] = a->v[1] * a->v[1] * t + c;
    mat->m[1][2] = sx + tyz;
    mat->m[2][0] = sy + txz;
    mat->m[2][1] = tyz - sx;
    mat->m[2][2] = a->v[2] * a->v[2] * t + c;
#else
    __asm {
            sub esp, 0x14
            push esi
            mov word ptr [esp + 0x14], dx
            push edi
            mov edi, dword ptr [esp + 0x18]
            mov esi, ecx
            mov ecx, edi
            call FastScalarSinAngle
            fstp dword ptr [esp + 0x10]
            mov ecx, edi
            call FastScalarCosAngle
            fld dword ptr [gFLOAT_0058afdc]
            fxch st(1)
            fstp dword ptr [esp + 0xc]
            mov eax, dword ptr [esp + 0x20]
            fsub dword ptr [esp + 0xc]
            fld dword ptr [eax]
            fld dword ptr [eax + 8]
            fxch st(2)
            fst dword ptr [esp + 8]
            fmul dword ptr [eax]
            fld dword ptr [eax + 4]
            fld dword ptr [eax + 8]
            fxch st(3)
            fst dword ptr [esp + 0x20]
            fmul dword ptr [esp + 0x20]
            fld dword ptr [eax + 4]
            fxch st(3)
            fstp dword ptr [esp + 0x20]
            fxch st(4)
            fmul dword ptr [esp + 0x20]
            fxch st(1)
            fmul dword ptr [esp + 0x20]
            fxch st(3)
            fmul dword ptr [esp + 0x10]
            fxch st(2)
            fmul dword ptr [esp + 0x10]
            fxch st(4)
            fmul dword ptr [esp + 8]
            fxch st(1)
            fst dword ptr [esp + 0x18]
            fxch st(3)
            fst dword ptr [esp + 0x20]
            fxch st(2)
            fst dword ptr [esp + 0x14]
            fadd dword ptr [esp + 0x20]
            fxch st(2)
            fsub dword ptr [esp + 0x14]
            fxch st(4)
            fstp dword ptr [esp + 0x14]
            fxch st(2)
            fsub dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x10]
            fxch st(3)
            fadd dword ptr [esp + 0xc]
            fld dword ptr [eax + 8]
            fmul dword ptr [eax + 4]
            fmul dword ptr [esp + 8]
            fstp dword ptr [esp + 0x10]
            fxch st(3)
            fmul dword ptr [eax]
            fxch st(3)
            fstp dword ptr [esi]
            fxch st(1)
            fstp dword ptr [esi + 4]
            fstp dword ptr [esi + 8]
            fxch st(1)
            fstp dword ptr [esi + 0xc]
            fld dword ptr [eax + 4]
            fst dword ptr [esp + 0x20]
            fmul dword ptr [esp + 0x20]
            fxch st(1)
            fstp dword ptr [esp + 0x20]
            pop edi
            fmul dword ptr [esp + 4]
            fadd dword ptr [esp + 8]
            fstp dword ptr [esi + 0x10]
            fld dword ptr [esp + 0x1c]
            fld dword ptr [esp + 0x10]
            fld dword ptr [esp + 0xc]
            fxch st(2)
            fadd dword ptr [esp + 0xc]
            fxch st(1)
            fadd dword ptr [esp + 0x14]
            fxch st(2)
            fsub dword ptr [esp + 0x1c]
            fxch st(1)
            fstp dword ptr [esi + 0x14]
            fxch st(1)
            fstp dword ptr [esi + 0x18]
            fstp dword ptr [esi + 0x1c]
            fld dword ptr [eax + 8]
            fld st(0)
            fmul st(0), st(1)
            fmul dword ptr [esp + 4]
            fadd dword ptr [esp + 8]
            fstp dword ptr [esi + 0x20]
            pop esi
            fstp st(0)
            add esp, 0x14
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
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004fc0e0
void C2_HOOK_FASTCALL DoSmokeColumn(int i, tU32 pTime, br_vector3* pRet_car_pos) {
    tSmoke_column* column;
    static const br_scalar upright_tilt_threshold = 0.1f;
    static const br_scalar descent_tilt_threshold = -0.1f;

    column = &gSmoke_column[i];
    if (column->core.field_0x0 == 1) {
        if (column->core.car->car_master_actor->t.t.mat.m[1][1] > upright_tilt_threshold) {
            column->upright = 1;
        }
        if (column->core.car->car_master_actor->t.t.mat.m[1][1] < descent_tilt_threshold) {
            column->upright = 0;
        }
        pRet_car_pos->v[0] = column->core.car->car_actor->model->prepared->groups->vertices[column->vertex_index].p.v[0]
            + column->core.car->car_actor->t.t.mat.m[3][0];
        pRet_car_pos->v[1] = column->core.car->car_actor->model->prepared->groups->vertices[column->vertex_index].p.v[1]
            + column->core.car->car_actor->t.t.mat.m[3][1];
        pRet_car_pos->v[2] = column->core.car->car_actor->model->prepared->groups->vertices[column->vertex_index].p.v[2]
            + column->core.car->car_actor->t.t.mat.m[3][2];
    } else {
        column->upright = 1;
        pRet_car_pos->v[0] = column->core.car_actor->model->prepared->groups->vertices[column->vertex_index].p.v[0];
        pRet_car_pos->v[1] = column->core.car_actor->model->prepared->groups->vertices[column->vertex_index].p.v[1];
        pRet_car_pos->v[2] = column->core.car_actor->model->prepared->groups->vertices[column->vertex_index].p.v[2];
    }
    if (column->core.field_0x0 == 1 && gProgram_state.cockpit_on && column->core.car != NULL
            && column->core.car->driver == eDriver_local_human
            && *(float*)((char*)column->core.car + 0x484) > pRet_car_pos->v[2]) {
        BrMatrix34ApplyP(pRet_car_pos, &column->core.car_actor->model->prepared->groups->vertices[column->vertex_index].p,
            &column->core.car_actor->t.t.mat);
    } else {
        pRet_car_pos->v[1] -= 0.0f;
    }
    if (column->upright == 0) {
        pRet_car_pos->v[1] = column->core.car->collision_info->shape->common.bb.min.v[1] * 0.14492754f;
    }
    BrMatrix34ApplyP(&column->pos, pRet_car_pos, &column->core.car_actor->t.t.mat);
    column->pos.v[1] -= 0.03f;
}

// GLOBAL: CARMA2_HW 0x00660118
static tU8 gFlame_frame_size[20][2] = {
    { 20, 25 }, { 24, 32 }, { 24, 34 }, { 28, 44 }, { 28, 49 },
    { 32, 53 }, { 28, 61 }, { 28, 69 }, { 24, 61 }, { 28, 64 },
    { 24, 70 }, { 20, 65 }, { 20, 64 }, { 20, 74 }, { 28, 55 },
    { 24, 62 }, { 28, 47 }, { 20, 51 }, { 20, 45 }, { 20, 45 },
};

// FUNCTION: CARMA2_HW 0x004fbdd0
void C2_NAKED C2_HOOK_FASTCALL FlameAnimate(int pColumn, const br_vector3* pPos, tU32 pTime) {
#ifndef CARPOCALYPSE2_MATCHING
    tSmoke_column* column;
    br_actor* actor;
    br_actor* flame;
    br_pixelmap* map;
    tPhysics_object* physics;
    tU32 bits;
    int i;
    int width;
    int height;
    int direction;
    br_scalar scale;
    br_scalar sx;
    br_scalar sy;
    double speed_squared;
    int can_start;
    typedef void (C2_HOOK_FASTCALL* raw_flame_fn)(int, int, tU32, tU32, tU32, tU32);
    volatile uintptr_t record_address;

    column = &gSmoke_column[pColumn];
    actor = column->flame_actor;
    DRMatrix34RotateY(&actor->t.t.mat, FastScalarArcTan2Angle(gCamera_to_world.m[2][0], gCamera_to_world.m[2][2]));
    memcpy(&actor->t.t.mat.m[3][0], &pPos->v[0], sizeof(br_scalar));
    memcpy(&actor->t.t.mat.m[3][1], &pPos->v[1], sizeof(br_scalar));
    bits = SmokeScalarBits(&pPos->v[2]);
    flame = actor->children;
    memcpy(&actor->t.t.mat.m[3][2], &bits, sizeof(bits));
    if (gAction_replay_mode) {
        for (i = 0; i < 3; i++) {
            column->frame_count[i] = (tS32)(tU32)(br_int_64)((double)ARGetReplayRate() + column->frame_count[i]);
            if (column->frame_count[i] >= 0 && column->frame_count[i] < 20) {
                flame->type = 1;
                flame->material->colour_map = gFlame_map[column->frame_count[i]];
                BrMaterialUpdate(flame->material, 0x7fff);
                map = gFlame_map[column->frame_count[i]];
                height = (tU16)map->height;
                width = (tU16)map->width;
                sy = SmokeScalarRound((double)height * column->scale_y[i]);
                sx = SmokeScalarRound((double)width * column->scale_x[i]);
                BrMatrix34Scale(&flame->t.t.mat, sx, sy, 1.0f);
                memcpy(&flame->t.t.mat.m[3][0], &column->offset_x[i], sizeof(br_scalar));
                memcpy(&flame->t.t.mat.m[3][2], &column->offset_z[i], sizeof(br_scalar));
            } else {
                flame->type = 0;
            }
            flame = flame->next;
        }
        return;
    }
    record_address = (uintptr_t)AddFlameToPipingSession;
    for (i = 0; i < 3; i++) {
        column->frame_count[i] = (tS32)((tU32)column->frame_count[i] + 1u);
        if (column->frame_count[i] >= 20) {
            ARStartPipingSession((tPipe_chunk_type)18);
            ((raw_flame_fn)record_address)((pColumn << 4) + i, (tS32)((tU32)column->frame_count[i] + 1u), SmokeScalarBits(&column->scale_x[i]), SmokeScalarBits(&column->scale_y[i]), SmokeScalarBits(&column->offset_x[i]), SmokeScalarBits(&column->offset_z[i]));
            AREndPipingSession();
            column->frame_count[i] = IRandomBetween(-5, -1);
            scale = SmokeScalarRound(SRandomBetween(1.0f, 1.5f) * .003000000026077032f);
            direction = IRandomBetween(0, 1) * 2 - 1;
            column->scale_x[i] = SmokeScalarRound((double)direction * scale);
            column->scale_y[i] = SmokeScalarRound(SRandomBetween(.5f, 1.0f) * .003000000026077032f);
            column->offset_x[i] = SmokeScalarRound(SRandomPosNeg(.03f));
            column->offset_z[i] = SmokeScalarRound(SRandomBetween(-.03f, 0.0f));
            flame->type = 0;
        }
        if (column->frame_count[i] == 0) {
            can_start = 1;
            if (column->core.car != NULL) {
                physics = column->core.car->collision_info;
                speed_squared = (double)physics->v.v[0] * physics->v.v[0] + (double)physics->v.v[1] * physics->v.v[1];
                speed_squared += (double)physics->v.v[2] * physics->v.v[2];
                can_start = !(speed_squared >= 11.594202898550725);
            }
            if (can_start && pTime * 30u < column->lifetime) {
                flame->type = 1;
                ARStartPipingSession((tPipe_chunk_type)18);
                ((raw_flame_fn)record_address)((pColumn << 4) + i, (tS32)((tU32)column->frame_count[i] - 1u), SmokeScalarBits(&column->scale_x[i]), SmokeScalarBits(&column->scale_y[i]), SmokeScalarBits(&column->offset_x[i]), SmokeScalarBits(&column->offset_z[i]));
                AREndPipingSession();
            } else {
                column->frame_count[i] = -5;
            }
        }
        if (column->frame_count[i] >= 0) {
            flame->material->colour_map = gFlame_map[column->frame_count[i]];
            BrMaterialUpdate(flame->material, 0x7fff);
            height = gFlame_frame_size[column->frame_count[i]][1];
            width = gFlame_frame_size[column->frame_count[i]][0];
            sy = SmokeScalarRound((double)height * column->scale_y[i]);
            sx = SmokeScalarRound((double)width * column->scale_x[i]);
            BrMatrix34Scale(&flame->t.t.mat, sx, sy, 1.0f);
            memcpy(&flame->t.t.mat.m[3][0], &column->offset_x[i], sizeof(br_scalar));
            memcpy(&flame->t.t.mat.m[3][2], &column->offset_z[i], sizeof(br_scalar));
        }
        flame = flame->next;
    }
#else
    __asm {
            sub esp, 0xc
            push ebx
            mov ebx, ecx
            mov ecx, dword ptr [gCamera_to_world+0x20]
            mov eax, ebx
            push ebp
            push esi
            shl eax, 5
            push edi
            sub eax, ebx
            mov edi, edx
            push ecx
            mov edx, dword ptr [gCamera_to_world+0x18]
            mov esi, dword ptr [eax*4 + gSmoke_column+0x24]
            lea ebp, [eax*4 + gSmoke_column]
            push edx
            mov dword ptr [esp + 0x18], ebp
            call FastScalarArcTan2Angle
            mov edx, eax
            lea ecx, [esi + 0x2c]
            call DRMatrix34RotateY
            mov eax, dword ptr [edi]
            mov dword ptr [esi + 0x50], eax
            mov ecx, dword ptr [edi + 4]
            mov dword ptr [esi + 0x54], ecx
            mov edx, dword ptr [edi + 8]
            mov edi, dword ptr [esi + 8]
            mov dword ptr [esi + 0x58], edx
            mov eax, dword ptr [gPed_676914]
            test eax, eax
            je L004fbee1
            lea esi, [ebp + 0x28]
            mov ebx, 3
    L004fbe3a:
            call ARGetReplayRate
            fiadd dword ptr [esi]
            call _ftol
            test eax, eax
            mov dword ptr [esi], eax
            jl L004fbec7
            cmp eax, 0x14
            jge L004fbec7
            mov ecx, dword ptr [edi + 0x1c]
            mov byte ptr [edi + 0x12], 1
            mov eax, dword ptr [esi]
            push 0x7fff
            mov edx, dword ptr [eax*4 + gFlame_map]
            mov dword ptr [ecx + 0x40], edx
            mov eax, dword ptr [edi + 0x1c]
            push eax
            call BrMaterialUpdate
            mov ecx, dword ptr [esi]
            xor edx, edx
            add esp, 8
            mov eax, dword ptr [ecx*4 + gFlame_map]
            push 0x3f800000
            push ecx
            mov dx, word ptr [eax + 0x36]
            xor ecx, ecx
            mov dword ptr [esp + 0x28], edx
            mov cx, word ptr [eax + 0x34]
            fild dword ptr [esp + 0x28]
            mov dword ptr [esp + 0x28], ecx
            lea edx, [edi + 0x2c]
            fmul dword ptr [esi + 0x24]
            fstp dword ptr [esp]
            fild dword ptr [esp + 0x28]
            push ecx
            fmul dword ptr [esi + 0x18]
            fstp dword ptr [esp]
            push edx
            call BrMatrix34Scale
            mov eax, dword ptr [esi + 0x30]
            add esp, 0x10
            mov dword ptr [edi + 0x50], eax
            mov ecx, dword ptr [esi + 0x3c]
            mov dword ptr [edi + 0x58], ecx
            jmp L004fbecb
    L004fbec7:
            mov byte ptr [edi + 0x12], 0
    L004fbecb:
            mov edi, dword ptr [edi]
            add esi, 4
            dec ebx
            jne L004fbe3a
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0xc
            ret 4
    L004fbee1:
            mov edx, dword ptr [esp + 0x10]
            xor ebp, ebp
            lea esi, [edx + 0x28]
    L004fbeea:
            mov ecx, dword ptr [esi]
            inc ecx
            mov eax, ecx
            mov dword ptr [esi], ecx
            cmp eax, 0x14
            jl L004fbfa7
            mov ecx, 0x12
            call ARStartPipingSession
            mov eax, dword ptr [esi + 0x3c]
            mov ecx, dword ptr [esi + 0x30]
            mov edx, dword ptr [esi + 0x24]
            push eax
            mov eax, dword ptr [esi + 0x18]
            push ecx
            mov ecx, ebx
            push edx
            mov edx, dword ptr [esi]
            push eax
            shl ecx, 4
            inc edx
            add ecx, ebp
            call AddFlameToPipingSession
            call AREndPipingSession
            or edx, 0xffffffff
            mov ecx, 0xfffffffb
            call IRandomBetween
            mov dword ptr [esi], eax
            push 0x3fc00000
            push 0x3f800000
            call SRandomBetween
            fmul dword ptr [gFLOAT_0058b0d8]
            mov edx, 1
            xor ecx, ecx
            fstp dword ptr [esp + 0x18]
            call IRandomBetween
            lea ecx, [eax + eax - 1]
            mov dword ptr [esp + 0x14], ecx
            push 0x3f800000
            fild dword ptr [esp + 0x18]
            push 0x3f000000
            fmul dword ptr [esp + 0x20]
            fstp dword ptr [esi + 0x18]
            call SRandomBetween
            fmul dword ptr [gFLOAT_0058b0d8]
            push 0x3cf5c28f
            fstp dword ptr [esi + 0x24]
            call SRandomPosNeg
            fstp dword ptr [esi + 0x30]
            push 0
            push 0xbcf5c28f
            call SRandomBetween
            fstp dword ptr [esi + 0x3c]
            mov byte ptr [edi + 0x12], 0
    L004fbfa7:
            cmp dword ptr [esi], 0
            jne L004fc049
            mov ecx, dword ptr [esp + 0x10]
            mov eax, dword ptr [ecx + 4]
            test eax, eax
            je L004fbffc
            mov eax, dword ptr [eax + 8]
            fld dword ptr [eax + 0x68]
            fld dword ptr [eax + 0x6c]
            fld dword ptr [eax + 0x70]
            fxch st(2)
            fst dword ptr [esp + 0x14]
            fld st(1)
            fxch st(1)
            fmul dword ptr [esp + 0x14]
            fxch st(1)
            fmul st(0), st(2)
            fxch st(3)
            fst dword ptr [esp + 0x14]
            fmul dword ptr [esp + 0x14]
            fxch st(3)
            faddp st(1), st(0)
            fxch st(2)
            faddp st(2), st(0)
            fxch st(1)
            fcomp qword ptr [gDOUBLE_0058b0e0]
            fnstsw ax
            test ah, 1
            fstp st(0)
            je L004fc043
    L004fbffc:
            mov eax, dword ptr [esp + 0x20]
            lea eax, [eax + eax*2]
            lea edx, [eax + eax*4]
            mov eax, dword ptr [ecx + 0x14]
            shl edx, 1
            cmp eax, edx
            jbe L004fc043
            mov ecx, 0x12
            mov byte ptr [edi + 0x12], 1
            call ARStartPipingSession
            mov eax, dword ptr [esi + 0x3c]
            mov ecx, dword ptr [esi + 0x30]
            mov edx, dword ptr [esi + 0x24]
            push eax
            mov eax, dword ptr [esi + 0x18]
            push ecx
            mov ecx, ebx
            push edx
            mov edx, dword ptr [esi]
            push eax
            shl ecx, 4
            dec edx
            add ecx, ebp
            call AddFlameToPipingSession
            call AREndPipingSession
            jmp L004fc049
    L004fc043:
            mov dword ptr [esi], 0xfffffffb
    L004fc049:
            mov eax, dword ptr [esi]
            test eax, eax
            jl L004fc0bc
            mov ecx, dword ptr [edi + 0x1c]
            mov edx, dword ptr [eax*4 + gFlame_map]
            push 0x7fff
            mov dword ptr [ecx + 0x40], edx
            mov eax, dword ptr [edi + 0x1c]
            push eax
            call BrMaterialUpdate
            mov eax, dword ptr [esi]
            xor ecx, ecx
            shl eax, 1
            add esp, 8
            xor edx, edx
            mov cl, byte ptr [eax + gFlame_frame_size+0x1]
            mov dl, byte ptr [eax + gFlame_frame_size]
            mov dword ptr [esp + 0x18], ecx
            push 0x3f800000
            fild dword ptr [esp + 0x1c]
            push ecx
            mov dword ptr [esp + 0x20], edx
            lea eax, [edi + 0x2c]
            fmul dword ptr [esi + 0x24]
            fstp dword ptr [esp]
            fild dword ptr [esp + 0x20]
            push ecx
            fmul dword ptr [esi + 0x18]
            fstp dword ptr [esp]
            push eax
            call BrMatrix34Scale
            mov ecx, dword ptr [esi + 0x30]
            add esp, 0x10
            mov dword ptr [edi + 0x50], ecx
            mov edx, dword ptr [esi + 0x3c]
            mov dword ptr [edi + 0x58], edx
    L004fc0bc:
            mov edi, dword ptr [edi]
            inc ebp
            add esi, 4
            cmp ebp, 3
            jl L004fbeea
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0xc
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
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0042df70
int C2_NAKED C2_HOOK_FASTCALL IsOnNonCarHalf(tCar_spec* pCar, br_vector3 *pPos) {
#ifndef CARPOCALYPSE2_MATCHING
    tCar_crush_spec* car_crush;

    car_crush = pCar->car_crush_spec;
    return car_crush != NULL
        && car_crush->field_0x4b8 != 0
        && BrVector3Dot(&car_crush->field_0x548, pPos) - car_crush->field_0x554 < 0.f;
#else
    __asm {
            mov eax, dword ptr [ecx + 0x18d4]
            test eax, eax
            je L0042dfc3
            mov cl, byte ptr [eax + 0x4b8]
            test cl, cl
            je L0042dfc3
            fld dword ptr [edx + 4]
            fld dword ptr [eax + 0x54c]
            fld dword ptr [edx + 8]
            fld dword ptr [eax + 0x548]
            fxch st(2)
            fmulp st(3), st(0)
            fmul dword ptr [eax + 0x550]
            fxch st(1)
            fmul dword ptr [edx]
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fsub dword ptr [eax + 0x554]
            fcomp dword ptr [gCrushZero]
            fnstsw ax
            test ah, 1
            je L0042dfc3
            mov eax, 1
            ret
    L0042dfc3:
            xor eax, eax
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
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004fc9c0
void C2_HOOK_FASTCALL SmudgeCar(tCar_spec* pCar, int fire_point) {
    /* empty */
}

// FUNCTION: CARMA2_HW 0x004f7cb0
void C2_HOOK_FASTCALL SetLineModelCols(tU8 pCol) {
    if (pCol != 0) {
        gLine_model->vertices[0].red = 0xff;
        gLine_model->vertices[0].grn = 0xff;
        gLine_model->vertices[0].blu = 0xff;
        gLine_model->vertices[1].red = 0xff;
        gLine_model->vertices[1].grn = 0xff;
        gLine_model->vertices[1].blu = 0xff;
    } else {
        gLine_model->vertices[0].red = 0xff;
        gLine_model->vertices[0].grn = 0x00;
        gLine_model->vertices[0].blu = 0x00;
        gLine_model->vertices[1].red = 0xff;
        gLine_model->vertices[1].grn = 0xff;
        gLine_model->vertices[1].blu = 0x00;
    }
    BrModelUpdate(gLine_model, BR_MODU_ALL);
}

// FUNCTION: CARMA2_HW 0x004f7d60
void C2_NAKED C2_HOOK_FASTCALL CreateSparks(const br_vector3* pPosition, br_vector3* pVelocity, const br_vector3* pForce, br_scalar pStrength, tCar_spec* pCar) {
#ifndef CARPOCALYPSE2_MATCHING
    br_vector3 force_normal;
    br_vector3 local_position;
    br_vector3 relative_position;
    br_vector3 local_normal;
    br_vector3 random_vector;
    br_scalar force_length;
    br_scalar dot;
    br_scalar scale;
    int spark_count;

    force_length = (float)sqrt((double)(pForce->v[0] * pForce->v[0] + pForce->v[2] * pForce->v[2] + pForce->v[1] * pForce->v[1]));
    force_normal.v[0] = pForce->v[0] / force_length;
    force_normal.v[1] = pForce->v[1] / force_length;
    force_normal.v[2] = pForce->v[2] / force_length;

    dot = pForce->v[0] * pVelocity->v[0] + pVelocity->v[2] * pForce->v[2] + pForce->v[1] * pVelocity->v[1];
    if (gFLOAT_0058b008 <= dot) {
        scale = (br_scalar)(gDOUBLE_0058b010 / (force_length * gFLOAT_0058aff0));
    } else {
        scale = (br_scalar)(gDOUBLE_0058b010 / (force_length * gFLOAT_0058aff0)) - dot / (force_length * force_length);
    }
    pVelocity->v[0] += pForce->v[0] * scale;
    pVelocity->v[1] += pForce->v[1] * scale;
    pVelocity->v[2] += pForce->v[2] * scale;

    spark_count = (int)(FRandomBetween(0.0f, (br_scalar)gDOUBLE_0058b020 - (float)sqrt((double)(pVelocity->v[0] * pVelocity->v[0] + pVelocity->v[2] * pVelocity->v[2] + pVelocity->v[1] * pVelocity->v[1])) * gFLOAT_0058b018) * pStrength);
    if (spark_count > 10) {
        spark_count = 10;
    }
    while (spark_count > 0) {
        gSparks[gSpark_next].pos = *pPosition;
        gSparks[gSpark_next].normal = force_normal;
        gSparks[gSpark_next].v = *pVelocity;
        gSparks[gSpark_next].v.v[0] *= FRandomBetween(0.5f, 0.9f);
        gSparks[gSpark_next].v.v[1] *= FRandomBetween(0.5f, 0.9f);
        gSparks[gSpark_next].v.v[2] *= FRandomBetween(0.5f, 0.9f);
        gSpark_flags |= 1 << gSpark_next;
        gSpark_next++;
        gSparks[gSpark_next - 1].count = 1000;
        gSparks[gSpark_next - 1].car = NULL;
        gSparks[gSpark_next - 1].time_sync = gPHIL_mechanics_time_sync;
        gSparks[gSpark_next - 1].colour = 0;
        if (gSpark_next >= CARPOCALYPSE2_ASIZE(gSparks)) {
            gSpark_next = 0;
        }
        spark_count--;
    }

    if (force_length * pStrength >= gFLOAT_0058aff0) {
        relative_position.v[0] = pPosition->v[0] - pCar->car_master_actor->t.t.mat.m[3][0];
        relative_position.v[1] = pPosition->v[1] - pCar->car_master_actor->t.t.mat.m[3][1];
        relative_position.v[2] = pPosition->v[2] - pCar->car_master_actor->t.t.mat.m[3][2];
        BrMatrix34TApplyV(&local_position, &relative_position, &pCar->car_master_actor->t.t.mat);
        BrMatrix34TApplyV(&local_normal, &force_normal, &pCar->car_master_actor->t.t.mat);
        spark_count = 3 - (int)(force_length * pStrength * gFLOAT_0058b02c);
        if (spark_count > 10) {
            spark_count = 10;
        }
        while (spark_count > 0) {
            gSparks[gSpark_next].pos.v[0] = local_position.v[0] + local_normal.v[0] * gFLOAT_0058b028;
            gSparks[gSpark_next].pos.v[1] = local_position.v[1] + local_normal.v[1] * gFLOAT_0058b028;
            gSparks[gSpark_next].pos.v[2] = local_position.v[2] + local_normal.v[2] * gFLOAT_0058b028;
            gSparks[gSpark_next].normal = local_normal;
            BrVector3SetFloat(&random_vector,
                FRandomBetween(-1.0f, 1.0f),
                FRandomBetween(-0.2f, 1.0f),
                FRandomBetween(-1.0f, 1.0f));
            dot = random_vector.v[0] * local_normal.v[0] + random_vector.v[1] * local_normal.v[1] + random_vector.v[2] * local_normal.v[2];
            gSparks[gSpark_next].v.v[0] = random_vector.v[0] - dot * local_normal.v[0];
            gSparks[gSpark_next].v.v[1] = random_vector.v[1] - dot * local_normal.v[1];
            gSparks[gSpark_next].v.v[2] = random_vector.v[2] - dot * local_normal.v[2];
            gSpark_flags |= 1 << gSpark_next;
            gSparks[gSpark_next].count = 1000;
            gSparks[gSpark_next].car = pCar;
            gSparks[gSpark_next].time_sync = gPHIL_mechanics_time_sync;
            gSparks[gSpark_next].colour = 0;
            gSpark_next++;
            if (gSpark_next >= CARPOCALYPSE2_ASIZE(gSparks)) {
                gSpark_next = 0;
            }
            spark_count--;
        }
        CreateShrapnelShower(pPosition, pVelocity, &force_normal, force_length, pCar, pCar, 0, NULL);
    }
#else
    __asm {
            sub esp, 0x40
            push ebx
            push ebp
            push esi
            push edi
            mov edi, edx
            mov ebp, ecx
            mov edx, dword ptr [esp + 0x54]
            fld dword ptr [edx + 4]
            fld dword ptr [edx + 8]
            fld dword ptr [edx]
            fxch st(2)
            fst dword ptr [esp + 0x54]
            fxch st(1)
            fst dword ptr [esp + 0x10]
            fxch st(1)
            fmul dword ptr [esp + 0x54]
            fxch st(1)
            fmul dword ptr [esp + 0x10]
            fld st(2)
            fmul st(0), st(3)
            fxch st(1)
            faddp st(2), st(0)
            fld dword ptr [edx + 4]
            fld dword ptr [edx + 8]
            fxch st(2)
            faddp st(3), st(0)
            fld dword ptr [edx]
            fxch st(3)
            fsqrt
            fst dword ptr [esp + 0x54]
            fstp dword ptr [esp + 0x54]
            fxch st(2)
            fdiv dword ptr [esp + 0x54]
            fxch st(2)
            fdiv dword ptr [esp + 0x54]
            fxch st(1)
            fdiv dword ptr [esp + 0x54]
            fld dword ptr [edx + 4]
            fxch st(3)
            fstp dword ptr [esp + 0x44]
            fld dword ptr [edx + 8]
            fxch st(2)
            fstp dword ptr [esp + 0x48]
            fld dword ptr [edi + 8]
            fxch st(1)
            fstp dword ptr [esp + 0x4c]
            fld dword ptr [edx]
            fxch st(3)
            fmul dword ptr [edi + 4]
            fxch st(1)
            fmulp st(2), st(0)
            fxch st(2)
            fmul dword ptr [edi]
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fstp st(1)
            fcom dword ptr [gFLOAT_0058b008]
            fnstsw ax
            test ah, 1
            je L004f7e26
            fld dword ptr [esp + 0x54]
            fmul dword ptr [gFLOAT_0058aff0]
            fld dword ptr [esp + 0x54]
            fmul dword ptr [esp + 0x54]
            fxch st(1)
            fdivr qword ptr [gDOUBLE_0058b010]
            fxch st(1)
            fdivr st(0), st(2)
            fxch st(1)
            fstp st(2)
            fsubp st(1), st(0)
            jmp L004f7e38
    L004f7e26:
            fstp st(0)
            fld dword ptr [esp + 0x54]
            fmul dword ptr [gFLOAT_0058aff0]
            fdivr qword ptr [gDOUBLE_0058b010]
    L004f7e38:
            fld dword ptr [edx + 8]
            fld dword ptr [edx + 4]
            fmul st(0), st(2)
            fxch st(1)
            fmul st(0), st(2)
            fxch st(1)
            fst dword ptr [esp + 0x24]
            fadd dword ptr [edi + 4]
            fxch st(1)
            fst dword ptr [esp + 0x28]
            fadd dword ptr [edi + 8]
            fxch st(2)
            fmul dword ptr [edx]
            fxch st(1)
            fst dword ptr [esp + 0x18]
            fxch st(2)
            fst dword ptr [esp + 0x1c]
            fxch st(1)
            fst dword ptr [esp + 0x20]
            fadd dword ptr [edi]
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
            fld dword ptr [esp + 0x18]
            fxch st(1)
            faddp st(2), st(0)
            fld dword ptr [esp + 0x1c]
            fxch st(2)
            fsqrt
            push ecx
            xor ebx, ebx
            fst dword ptr [esp + 0x20]
            fmul dword ptr [gFLOAT_0058b018]
            fxch st(3)
            fstp dword ptr [edi]
            fstp dword ptr [edi + 4]
            fxch st(1)
            fsubr qword ptr [gDOUBLE_0058b020]
            fxch st(1)
            fstp dword ptr [edi + 8]
            fstp dword ptr [esp]
            push ebx
            call FRandomBetween
            fmul dword ptr [esp + 0x58]
            call _ftol
            cmp eax, 0xa
            jle L004f7edb
            mov eax, 0xa
    L004f7edb:
            cmp eax, ebx
            jle L004f8003
            mov ecx, dword ptr [gSpark_next]
            mov dword ptr [esp + 0x1c], eax
    L004f7eed:
            mov edx, dword ptr [ebp]
            mov eax, ecx
            shl eax, 6
            shl ecx, 6
            mov dword ptr [eax + gSparks+0x4], edx
            mov edx, dword ptr [ebp + 4]
            mov dword ptr [eax + gSparks+0x8], edx
            mov edx, dword ptr [ebp + 8]
            mov dword ptr [eax + gSparks+0xc], edx
            mov edx, dword ptr [esp + 0x44]
            mov dword ptr [eax + gSparks+0x28], edx
            mov edx, dword ptr [esp + 0x48]
            mov dword ptr [eax + gSparks+0x2c], edx
            mov edx, dword ptr [esp + 0x4c]
            mov dword ptr [eax + gSparks+0x30], edx
            mov edx, dword ptr [edi]
            mov dword ptr [eax + gSparks+0x10], edx
            mov edx, dword ptr [edi + 4]
            mov dword ptr [eax + gSparks+0x14], edx
            mov edx, dword ptr [edi + 8]
            push 0x3f666666
            push 0x3f000000
            mov dword ptr [eax + gSparks+0x18], edx
            lea esi, [ecx + gSparks+0x10]
            call FRandomBetween
            mov eax, dword ptr [gSpark_next]
            fmul dword ptr [esi]
            shl eax, 6
            push 0x3f666666
            push 0x3f000000
            fstp dword ptr [esi]
            lea esi, [eax + gSparks+0x14]
            call FRandomBetween
            fmul dword ptr [esi]
            push 0x3f666666
            push 0x3f000000
            fstp dword ptr [esi]
            mov esi, dword ptr [gSpark_next]
            shl esi, 6
            call FRandomBetween
            mov ecx, dword ptr [gSpark_next]
            mov eax, dword ptr [gSpark_flags]
            mov edx, 1
            shl edx, cl
            fmul dword ptr [esi + gSparks+0x18]
            or eax, edx
            inc ecx
            fstp dword ptr [esi + gSparks+0x18]
            mov dword ptr [gSpark_flags], eax
            mov eax, dword ptr [gPHIL_mechanics_time_sync]
            mov dword ptr [esi + gSparks], 0x3e8
            mov dword ptr [esi + gSparks+0x38], ebx
            mov dword ptr [esi + gSparks+0x34], eax
            cmp ecx, 0x20
            mov byte ptr [esi + gSparks+0x3c], bl
            mov dword ptr [gSpark_next], ecx
            jl L004f7ff4
            xor ecx, ecx
            mov dword ptr [gSpark_next], ecx
    L004f7ff4:
            mov eax, dword ptr [esp + 0x1c]
            dec eax
            mov dword ptr [esp + 0x1c], eax
            jne L004f7eed
    L004f8003:
            fld dword ptr [esp + 0x54]
            fmul dword ptr [esp + 0x58]
            fst dword ptr [esp + 0x58]
            fcomp dword ptr [gFLOAT_0058aff0]
            fnstsw ax
            test ah, 1
            jne L004f827d
            mov edx, dword ptr [esp + 0x5c]
            lea ecx, [esp + 0x2c]
            fld dword ptr [ebp]
            mov eax, dword ptr [edx + 0x10]
            lea edx, [esp + 0x38]
            fsub dword ptr [eax + 0x50]
            fld dword ptr [ebp + 4]
            fld dword ptr [ebp + 8]
            fxch st(2)
            fstp dword ptr [esp + 0x2c]
            lea esi, [eax + 0x2c]
            fsub dword ptr [eax + 0x54]
            push esi
            push ecx
            push edx
            fstp dword ptr [esp + 0x3c]
            fsub dword ptr [eax + 0x58]
            fstp dword ptr [esp + 0x40]
            call BrMatrix34TApplyV
            mov esi, dword ptr [esp + 0x68]
            add esp, 0xc
            lea ecx, [esp + 0x44]
            lea edx, [esp + 0x20]
            mov eax, dword ptr [esi + 0x10]
            add eax, 0x2c
            push eax
            push ecx
            push edx
            call BrMatrix34TApplyV
            fld dword ptr [esp + 0x2c]
            fld dword ptr [esp + 0x30]
            fld dword ptr [esp + 0x34]
            fxch st(2)
            fmul dword ptr [gFLOAT_0058b028]
            fld dword ptr [esp + 0x44]
            fxch st(2)
            fmul dword ptr [gFLOAT_0058b028]
            fld dword ptr [esp + 0x48]
            fxch st(4)
            fmul dword ptr [gFLOAT_0058b028]
            fld dword ptr [esp + 0x4c]
            fxch st(3)
            fstp dword ptr [esp + 0x38]
            fxch st(3)
            fadd dword ptr [esp + 0x38]
            fxch st(1)
            fstp dword ptr [esp + 0x3c]
            fxch st(3)
            fadd dword ptr [esp + 0x3c]
            fxch st(3)
            fstp dword ptr [esp + 0x44]
            fxch st(1)
            fstp dword ptr [esp + 0x40]
            add esp, 0xc
            fadd dword ptr [esp + 0x34]
            fxch st(1)
            fstp dword ptr [esp + 0x3c]
            fld dword ptr [esp + 0x58]
            fmul dword ptr [gFLOAT_0058b02c]
            fxch st(1)
            fstp dword ptr [esp + 0x40]
            call _ftol
            mov ecx, 3
            sub ecx, eax
            cmp ecx, 0xa
            jle L004f80fe
            mov ecx, 0xa
    L004f80fe:
            cmp ecx, ebx
            jle L004f8266
            mov dword ptr [esp + 0x58], ecx
            mov ecx, dword ptr [gSpark_next]
    L004f8110:
            fld dword ptr [esp + 0x38]
            fld dword ptr [esp + 0x3c]
            fld dword ptr [esp + 0x40]
            fld dword ptr [esp + 0x20]
            fld dword ptr [esp + 0x24]
            fld dword ptr [esp + 0x28]
            fxch st(5)
            mov eax, ecx
            push 0x3f800000
            shl eax, 6
            push 0xbf800000
            fstp dword ptr [eax + gSparks+0x4]
            fxch st(3)
            fstp dword ptr [eax + gSparks+0x8]
            fxch st(1)
            fstp dword ptr [eax + gSparks+0xc]
            fstp dword ptr [eax + gSparks+0x28]
            fstp dword ptr [eax + gSparks+0x2c]
            fstp dword ptr [eax + gSparks+0x30]
            call FRandomBetween
            push ecx
            fstp dword ptr [esp]
            push 0x3f800000
            push 0xbe4ccccd
            call FRandomBetween
            push ecx
            fstp dword ptr [esp]
            push 0x3f800000
            push 0xbf800000
            call FRandomBetween
            lea eax, [esp + 0x34]
            push ecx
            fstp dword ptr [esp]
            push eax
            call BrVector3SetFloat
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
            mov ecx, dword ptr [gSpark_next]
            mov esi, dword ptr [gSpark_flags]
            mov eax, ecx
            mov edx, 1
            faddp st(1), st(0)
            shl eax, 6
            shl edx, cl
            fld st(0)
            fld st(1)
            fxch st(1)
            fmul dword ptr [esp + 0x30]
            fxch st(1)
            fmul dword ptr [esp + 0x34]
            fxch st(2)
            fmul dword ptr [esp + 0x38]
            fxch st(1)
            fsubr dword ptr [esp + 0x3c]
            fxch st(2)
            fsubr dword ptr [esp + 0x40]
            fxch st(1)
            fsubr dword ptr [esp + 0x44]
            fxch st(2)
            fstp dword ptr [eax + gSparks+0x10]
            add esp, 0x10
            or esi, edx
            fstp dword ptr [eax + gSparks+0x14]
            mov dword ptr [gSpark_flags], esi
            mov esi, dword ptr [esp + 0x5c]
            fstp dword ptr [eax + gSparks+0x18]
            mov dword ptr [eax + gSparks], 0x3e8
            mov dword ptr [eax + gSparks+0x38], esi
            mov edx, dword ptr [gPHIL_mechanics_time_sync]
            inc ecx
            mov dword ptr [eax + gSparks+0x34], edx
            cmp ecx, 0x20
            mov dword ptr [gSpark_next], ecx
            mov byte ptr [eax + gSparks+0x3c], bl
            jl L004f8257
            xor ecx, ecx
            mov dword ptr [gSpark_next], ecx
    L004f8257:
            mov eax, dword ptr [esp + 0x58]
            dec eax
            mov dword ptr [esp + 0x58], eax
            jne L004f8110
    L004f8266:
            mov eax, dword ptr [esp + 0x54]
            push ebx
            push ebx
            push esi
            push esi
            lea ecx, [esp + 0x54]
            push eax
            push ecx
            mov edx, edi
            mov ecx, ebp
            call CreateShrapnelShower
    L004f827d:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x40
            ret 0xc
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

int C2_HOOK_FASTCALL DrawLine3DThroughBRender(br_vector3* start, br_vector3* end, br_pixelmap* pScreen, br_pixelmap* pDepth_buffer, float scale, br_pixelmap* shade_table);

// FUNCTION: CARMA2_HW 0x004f6b80
int C2_HOOK_FASTCALL DrawLine3D(br_vector3* start, br_vector3* end, br_pixelmap* pScreen, br_pixelmap* pDepth_buffer, br_pixelmap* shade_table) {
    br_vector3 lStart, lEnd;
    br_vector4 projStart, projEnd;

    if (gNo_2d_effects) {
        gLine_model->vertices[0].p.v[0] = start->v[0];
        gLine_model->vertices[0].p.v[1] = start->v[1];
        gLine_model->vertices[0].p.v[2] = start->v[2];
        gLine_model->vertices[1].p.v[0] = end->v[0];
        gLine_model->vertices[1].p.v[1] = end->v[1];
        gLine_model->vertices[1].p.v[2] = end->v[2];
        BrModelUpdate(gLine_model, BR_MODU_VERTEX_POSITIONS);
        BrZbsSceneRenderAdd(gLine_actor);
        return 0x3e7;
    }

    lStart.v[0] = start->v[0];
    lStart.v[1] = start->v[1];
    lStart.v[2] = start->v[2];
    lEnd.v[0] = end->v[0];
    lEnd.v[1] = end->v[1];
    lEnd.v[2] = end->v[2];

    {
        float clip = -gSpark_cam->hither_z;

        if (lStart.v[2] > clip) goto common_1;
        if (lEnd.v[2] <= clip) goto project;
    common_1:
        if (lStart.v[2] <= clip || lEnd.v[2] <= clip) goto clip_line;
        return 0;

    clip_line:
        {
            float ratio = (lEnd.v[2] + gSpark_cam->hither_z) / (lEnd.v[2] - lStart.v[2]);
            if (lStart.v[2] > clip) {
                lStart.v[0] = lEnd.v[0] - (lEnd.v[0] - lStart.v[0]) * ratio;
                lStart.v[1] = lEnd.v[1] - (lEnd.v[1] - lStart.v[1]) * ratio;
                lStart.v[2] = -gSpark_cam->hither_z;
            }
            if (lEnd.v[2] > -gSpark_cam->hither_z) {
                lEnd.v[0] = lEnd.v[0] - (lEnd.v[0] - lStart.v[0]) * ratio;
                lEnd.v[1] = lEnd.v[1] - (lEnd.v[1] - lStart.v[1]) * ratio;
                lEnd.v[2] = -gSpark_cam->hither_z;
            }
        }
    }
project:

    BrMatrix4ApplyP(&projStart, &lStart, &gCameraToScreen);
    BrMatrix4ApplyP(&projEnd, &lEnd, &gCameraToScreen);

    lStart.v[0] = projStart.v[0] / projStart.v[3];
    lStart.v[1] = projStart.v[1] / projStart.v[3];
    lStart.v[2] = projStart.v[2] / projStart.v[3];
    lEnd.v[0] = projEnd.v[0] / projEnd.v[3];
    lEnd.v[1] = projEnd.v[1] / projEnd.v[3];
    lEnd.v[2] = projEnd.v[2] / projEnd.v[3];

    return DrawLine3DThroughBRender(
        &lStart, &lEnd,
        pScreen, pDepth_buffer, 1.0f, shade_table);
}

// FUNCTION: CARMA2_HW 0x004f6de0
int C2_NAKED C2_HOOK_FASTCALL DrawLine3DThroughBRender(br_vector3* start, br_vector3* end, br_pixelmap* pScreen, br_pixelmap* pDepth_buffer, float scale, br_pixelmap* shade_table) {
#ifndef CARPOCALYPSE2_MATCHING
    int xe;
    int ye;
    br_uint_8* screen_ptr;
    char* shade_tbl;
    int two_dy;
    int bres_error;
    int two_dx;
    int xs;
    int ys;
    int x_direction;
    int y_direction;
    br_scalar z, z_inc;
    int shade_inc;
    int shade_scale;
    br_uint_16* z_ptr;
    int shade_count;
    br_uint_16* zp;

    screen_ptr = (br_uint_8*)pScreen->pixels + pScreen->base_y * pScreen->row_bytes + pScreen->base_x;

    xs = (int)start->v[0] + pScreen->width / 2;
    xe = (int)end->v[0] + pScreen->width / 2;

    ys = pScreen->height / 2 - (int)start->v[1];
    ye = pScreen->height / 2 - (int)end->v[1];

    if (scale < 0.001f || scale > 1.0f) return 0;

    if (xs < 0 || xe < 0) {
        if (xs < 0) {
            int t;
            if (xe < 0) return 0;
            t = xe - xs;
            start->v[2] = end->v[2] - (end->v[2] - start->v[2]) * xe / t;
            ys = ye - (ye - ys) * xe / t;
            xs = 0;
        } else {
            int t = xs - xe;
            end->v[2] = start->v[2] - (start->v[2] - end->v[2]) * xs / t;
            ye = ys - (ys - ye) * xs / t;
            xe = 0;
        }
    }
    if (xs >= pScreen->width || xe >= pScreen->width) {
        if (xs >= pScreen->width) {
            int t;
            int n;
            if (xe >= pScreen->width) return 0;
            t = xe - xs;
            n = xe - pScreen->width + 1;
            start->v[2] = end->v[2] - (end->v[2] - start->v[2]) * n / t;
            ys = ye - (ye - ys) * n / t;
            xs = pScreen->width - 1;
        } else {
            int t = xs - xe;
            int n = xs - pScreen->width + 1;
            end->v[2] = start->v[2] - (start->v[2] - end->v[2]) * n / t;
            ye = ys - (ys - ye) * n / t;
            xe = pScreen->width - 1;
        }
    }

    if (ys < 0 || ye < 0) {
        if (ys < 0) {
            int t;
            if (ye < 0) return 0;
            t = ye - ys;
            start->v[2] = end->v[2] - (end->v[2] - start->v[2]) * ye / t;
            xs = xe - (xe - xs) * ye / t;
            ys = 0;
        } else {
            int t = ys - ye;
            end->v[2] = start->v[2] - (start->v[2] - end->v[2]) * ys / t;
            xe = xs - (xs - xe) * ys / t;
            ye = 0;
        }
    }
    if (ys >= pScreen->height || ye >= pScreen->height) {
        if (ys >= pScreen->height) {
            int t;
            int n;
            if (ye >= pScreen->height) return 0;
            t = ye - ys;
            n = ye - pScreen->height + 1;
            start->v[2] = end->v[2] - (end->v[2] - start->v[2]) * n / t;
            xs = xe - (xe - xs) * n / t;
            ys = pScreen->height - 1;
        } else {
            int t = ys - ye;
            int n = ys - pScreen->height + 1;
            end->v[2] = start->v[2] - (start->v[2] - end->v[2]) * n / t;
            xe = xs - (xs - xe) * n / t;
            ye = pScreen->height - 1;
        }
    }

    z = start->v[2];
    zp = (br_uint_16*)pDepth_buffer->pixels;
    shade_tbl = (char*)shade_table->pixels + shade_table->base_y * shade_table->row_bytes;
    two_dx = 2 * abs(xe - xs);
    two_dy = 2 * abs(ye - ys);
    x_direction = xe - xs < 0 ? -1 : 1;
    y_direction = ye - ys < 0 ? -1 : 1;

    screen_ptr += ys * pScreen->row_bytes + xs;
    z_ptr = zp + (pDepth_buffer->row_bytes / 2) * ys + xs;
    shade_scale = (int)((scale - 0.001f) * shade_table->height);

    if (two_dx > two_dy) {
        shade_inc = 500 * two_dx / shade_scale;
        z_inc = 2.0f * (end->v[2] - start->v[2]) / two_dx;
        bres_error = two_dy - two_dx / 2;
        shade_count = shade_inc;

        for (;;) {
            if ((1.0f - z) * 32768.0f < (float)*z_ptr) {
                *z_ptr = (br_uint_16)((1.0f - z) * 32768.0f);
                *screen_ptr = shade_tbl[*screen_ptr];
            }
            if (xs == xe) break;
            if (bres_error >= 0) {
                screen_ptr += y_direction * pScreen->row_bytes;
                z_ptr += (pDepth_buffer->row_bytes / 2) * y_direction;
                bres_error -= two_dx;
            }
            xs += x_direction;
            screen_ptr += x_direction;
            bres_error += two_dy;
            z_ptr += x_direction;
            z += z_inc;
            shade_count -= 1000;
            if (shade_count <= 0) {
                do {
                    shade_tbl += shade_table->row_bytes;
                    shade_count += shade_inc;
                } while (shade_count <= 0);
            }
        }
    } else {
        shade_inc = 500 * two_dy / shade_scale;
        z_inc = 2.0f * (end->v[2] - start->v[2]) / two_dy;
        bres_error = two_dx - two_dy / 2;
        shade_count = shade_inc;

        for (;;) {
            if ((1.0f - z) * 32768.0f < (float)*z_ptr) {
                *z_ptr = (br_uint_16)((1.0f - z) * 32768.0f);
                *screen_ptr = shade_tbl[*screen_ptr];
            }
            if (ys == ye) break;
            if (bres_error >= 0) {
                screen_ptr += x_direction;
                z_ptr += x_direction;
                bres_error -= two_dy;
            }
            ys += y_direction;
            screen_ptr += y_direction * pScreen->row_bytes;
            bres_error += two_dx;
            z_ptr += (pDepth_buffer->row_bytes / 2) * y_direction;
            z += z_inc;
            shade_count -= 1000;
            if (shade_count <= 0) {
                do {
                    shade_tbl += shade_table->row_bytes;
                    shade_count += shade_inc;
                } while (shade_count <= 0);
            }
        }
    }

    return 1;
#else
    __asm {
            sub esp, 0x2c
            push ebx
            mov ebx, ecx
            mov ecx, dword ptr [esp + 0x34]
            push ebp
            mov ebp, edx
            xor eax, eax
            movsx edx, word ptr [ecx + 0x28]
            _emit 0xd9
        _emit 0x03
            mov ax, word ptr [ecx + 0x32]
            push esi
            imul eax, edx
            xor edx, edx
            push edi
            mov dx, word ptr [ecx + 0x30]
            add eax, edx
            mov edx, dword ptr [ecx + 8]
            add eax, edx
            mov dword ptr [esp + 0x18], eax
            mov eax, dword ptr [esp + 0x44]
            mov edx, dword ptr [eax + 8]
            xor eax, eax
            mov dword ptr [esp + 0x24], edx
            mov edx, dword ptr [esp + 0x4c]
            movsx esi, word ptr [edx + 0x28]
            mov ax, word ptr [edx + 0x32]
            mov edi, dword ptr [edx + 8]
            imul eax, esi
            add eax, edi
            xor edi, edi
            mov di, word ptr [ecx + 0x34]
            mov dword ptr [esp + 0x1c], eax
            shr edi, 1
            call _ftol
            _emit 0xd9
        _emit 0x45
        _emit 0x00
            mov esi, eax
            add esi, edi
            mov dword ptr [esp + 0x34], esi
            call _ftol
            add eax, edi
            xor edi, edi
            fld dword ptr [ebx + 4]
            mov dword ptr [esp + 0x10], eax
            mov eax, dword ptr [esp + 0x40]
            mov di, word ptr [eax + 0x36]
            shr edi, 1
            mov dword ptr [esp + 0x38], edi
            call _ftol
            fld dword ptr [ebp + 4]
            sub edi, eax
            mov dword ptr [esp + 0x28], edi
            call _ftol
            fld dword ptr [esp + 0x48]
            fcomp dword ptr [gFloat_0058afb4+0x24]
            mov ecx, dword ptr [esp + 0x38]
            sub ecx, eax
            mov dword ptr [esp + 0x14], ecx
            fnstsw ax
            test ah, 1
            jne L004f7439
            fld dword ptr [esp + 0x48]
            fcomp dword ptr [gFLOAT_0058afdc]
            fnstsw ax
            test ah, 0x41
            je L004f7439
            mov edx, dword ptr [esp + 0x10]
            test esi, esi
            jl L004f6ebd
            test edx, edx
            jge L004f6f3b
    L004f6ebd:
            test esi, esi
            jge L004f6f03
            test edx, edx
            jl L004f7439
            test esi, esi
            jge L004f6f03
            mov eax, edx
            sub eax, esi
            mov dword ptr [esp + 0x20], eax
            mov eax, ecx
            sub eax, edi
            imul eax, edx
            fld dword ptr [ebp + 8]
            fsub dword ptr [ebx + 8]
            fimul dword ptr [esp + 0x10]
            fidiv dword ptr [esp + 0x20]
            cdq
            idiv dword ptr [esp + 0x20]
            mov edx, ecx
            fsubr dword ptr [ebp + 8]
            fstp dword ptr [ebx + 8]
            sub edx, eax
            xor esi, esi
            mov edi, edx
            mov dword ptr [esp + 0x28], edi
            jmp L004f6f3b
    L004f6f03:
            mov eax, esi
            mov dword ptr [esp + 0x10], 0
            sub eax, edx
            mov dword ptr [esp + 0x20], eax
            mov eax, edi
            sub eax, ecx
            mov ecx, edi
            imul eax, esi
            fld dword ptr [ebx + 8]
            fsub dword ptr [ebp + 8]
            fimul dword ptr [esp + 0x34]
            fidiv dword ptr [esp + 0x20]
            cdq
            idiv dword ptr [esp + 0x20]
            fsubr dword ptr [ebx + 8]
            fstp dword ptr [ebp + 8]
            sub ecx, eax
            mov dword ptr [esp + 0x14], ecx
    L004f6f3b:
            mov edx, dword ptr [esp + 0x40]
            xor eax, eax
            mov ax, word ptr [edx + 0x34]
            mov edx, dword ptr [esp + 0x10]
            cmp esi, eax
            jge L004f6f55
            cmp edx, eax
            jl L004f7001
    L004f6f55:
            cmp esi, eax
            jl L004f6faf
            cmp edx, eax
            jge L004f7439
            cmp esi, eax
            jl L004f6faf
            sub edx, eax
            mov eax, dword ptr [esp + 0x10]
            sub eax, esi
            inc edx
            mov dword ptr [esp + 0x20], eax
            mov eax, ecx
            sub eax, edi
            mov dword ptr [esp + 0x38], edx
            imul eax, edx
            fld dword ptr [ebp + 8]
            fsub dword ptr [ebx + 8]
            fimul dword ptr [esp + 0x38]
            fidiv dword ptr [esp + 0x20]
            cdq
            idiv dword ptr [esp + 0x20]
            mov edx, ecx
            fsubr dword ptr [ebp + 8]
            fstp dword ptr [ebx + 8]
            sub edx, eax
            xor eax, eax
            mov edi, edx
            mov edx, dword ptr [esp + 0x40]
            mov dword ptr [esp + 0x28], edi
            mov ax, word ptr [edx + 0x34]
            mov esi, eax
            dec esi
            jmp L004f6ffd
    L004f6faf:
            mov edx, esi
            sub edx, eax
            mov eax, esi
            inc edx
            mov dword ptr [esp + 0x30], edx
            mov edx, dword ptr [esp + 0x10]
            sub eax, edx
            mov dword ptr [esp + 0x20], eax
            mov eax, edi
            sub eax, ecx
            mov ecx, edi
            imul eax, dword ptr [esp + 0x30]
            fld dword ptr [ebx + 8]
            fsub dword ptr [ebp + 8]
            fimul dword ptr [esp + 0x30]
            fidiv dword ptr [esp + 0x20]
            cdq
            idiv dword ptr [esp + 0x20]
            xor edx, edx
            fsubr dword ptr [ebx + 8]
            fstp dword ptr [ebp + 8]
            sub ecx, eax
            mov eax, dword ptr [esp + 0x40]
            mov dword ptr [esp + 0x14], ecx
            mov dx, word ptr [eax + 0x34]
            dec edx
            mov dword ptr [esp + 0x10], edx
    L004f6ffd:
            mov edx, dword ptr [esp + 0x10]
    L004f7001:
            test edi, edi
            jl L004f700d
            test ecx, ecx
            jge L004f7087
            test edi, edi
            jge L004f704b
    L004f700d:
            test ecx, ecx
            jl L004f7439
            test edi, edi
            jge L004f704b
            mov eax, ecx
            sub eax, edi
            mov dword ptr [esp + 0x20], eax
            mov eax, edx
            sub eax, esi
            mov esi, dword ptr [esp + 0x10]
            imul eax, ecx
            fld dword ptr [ebp + 8]
            fsub dword ptr [ebx + 8]
            fimul dword ptr [esp + 0x14]
            fidiv dword ptr [esp + 0x20]
            cdq
            idiv dword ptr [esp + 0x20]
            fsubr dword ptr [ebp + 8]
            fstp dword ptr [ebx + 8]
            sub esi, eax
            xor edi, edi
            jmp L004f7087
    L004f704b:
            mov eax, edi
            mov dword ptr [esp + 0x14], 0
            sub eax, ecx
            mov ecx, esi
            mov dword ptr [esp + 0x20], eax
            mov eax, esi
            sub eax, edx
            imul eax, edi
            fld dword ptr [ebx + 8]
            fsub dword ptr [ebp + 8]
            fimul dword ptr [esp + 0x28]
            fidiv dword ptr [esp + 0x20]
            cdq
            idiv dword ptr [esp + 0x20]
            fsubr dword ptr [ebx + 8]
            fstp dword ptr [ebp + 8]
            sub ecx, eax
            mov dword ptr [esp + 0x10], ecx
            mov ecx, dword ptr [esp + 0x14]
    L004f7087:
            mov edx, dword ptr [esp + 0x40]
            xor eax, eax
            mov ax, word ptr [edx + 0x36]
            cmp edi, eax
            jge L004f70a1
            cmp ecx, eax
            jl L004f7143
            cmp edi, eax
            jl L004f70f5
    L004f70a1:
            cmp ecx, eax
            jge L004f7439
            cmp edi, eax
            jl L004f70f5
            mov edx, ecx
            sub edx, eax
            mov eax, ecx
            sub eax, edi
            mov edi, dword ptr [esp + 0x10]
            mov dword ptr [esp + 0x20], eax
            mov eax, edi
            inc edx
            sub eax, esi
            imul eax, edx
            fld dword ptr [ebp + 8]
            fsub dword ptr [ebx + 8]
            mov dword ptr [esp + 0x38], edx
            fimul dword ptr [esp + 0x38]
            cdq
            idiv dword ptr [esp + 0x20]
            fidiv dword ptr [esp + 0x20]
            mov edx, dword ptr [esp + 0x40]
            fsubr dword ptr [ebp + 8]
            fstp dword ptr [ebx + 8]
            sub edi, eax
            xor eax, eax
            mov ax, word ptr [edx + 0x36]
            mov esi, edi
            mov edi, eax
            dec edi
            jmp L004f7143
    L004f70f5:
            mov edx, edi
            sub edx, eax
            mov eax, edi
            sub eax, ecx
            mov ecx, dword ptr [esp + 0x10]
            mov dword ptr [esp + 0x20], eax
            mov eax, esi
            inc edx
            sub eax, ecx
            imul eax, edx
            fld dword ptr [ebx + 8]
            fsub dword ptr [ebp + 8]
            mov dword ptr [esp + 0x38], edx
            mov ecx, esi
            fimul dword ptr [esp + 0x38]
            cdq
            idiv dword ptr [esp + 0x20]
            fidiv dword ptr [esp + 0x20]
            xor edx, edx
            fsubr dword ptr [ebx + 8]
            fstp dword ptr [ebp + 8]
            sub ecx, eax
            mov eax, dword ptr [esp + 0x40]
            mov dword ptr [esp + 0x10], ecx
            mov dx, word ptr [eax + 0x36]
            dec edx
            mov dword ptr [esp + 0x14], edx
            mov ecx, edx
    L004f7143:
            mov eax, dword ptr [esp + 0x10]
            sub ecx, edi
            sub eax, esi
            mov dword ptr [esp + 0x2c], esi
            mov dword ptr [esp + 0x38], eax
            mov dword ptr [esp + 0x30], edi
            cdq
            fld dword ptr [ebx + 8]
            xor eax, edx
            sub eax, edx
            xor edx, edx
            shl eax, 1
            mov dword ptr [esp + 0x28], eax
            mov eax, dword ptr [esp + 0x38]
            test eax, eax
            setge dl
            dec edx
            mov eax, ecx
            and edx, 0xfffffffe
            inc edx
            mov dword ptr [esp + 0x34], edx
            cdq
            fld dword ptr [esp + 0x48]
            xor eax, edx
            sub eax, edx
            shl eax, 1
            mov dword ptr [esp + 0x20], eax
            xor eax, eax
            test ecx, ecx
            mov ecx, dword ptr [esp + 0x40]
            setge al
            movsx edx, word ptr [ecx + 0x28]
            fsub dword ptr [gFloat_0058afb4+0x24]
            dec eax
            mov ecx, dword ptr [esp + 0x44]
            and al, 0xfe
            inc eax
            mov dword ptr [esp + 0x38], eax
            mov eax, edi
            imul eax, edx
            mov edx, dword ptr [esp + 0x18]
            add eax, esi
            add edx, eax
            movsx eax, word ptr [ecx + 0x28]
            mov dword ptr [esp + 0x18], edx
            mov ecx, dword ptr [esp + 0x4c]
            cdq
            sub eax, edx
            mov edx, dword ptr [esp + 0x24]
            sar eax, 1
            imul eax, edi
            add eax, esi
            lea esi, [edx + eax*2]
            xor eax, eax
            mov ax, word ptr [ecx + 0x36]
            mov dword ptr [esp + 0x48], eax
            fimul dword ptr [esp + 0x48]
            call _ftol
            mov edi, dword ptr [esp + 0x28]
            mov ecx, dword ptr [esp + 0x20]
            cmp edi, ecx
            mov dword ptr [esp + 0x48], eax
            jle L004f7311
            mov eax, edi
            cdq
            fld dword ptr [ebp + 8]
            sub eax, edx
            sar eax, 1
            sub ecx, eax
            lea eax, [edi + edi*4]
            fsub dword ptr [ebx + 8]
            lea eax, [eax + eax*4]
            mov dword ptr [esp + 0x24], ecx
            lea eax, [eax + eax*4]
            shl eax, 2
            cdq
            idiv dword ptr [esp + 0x48]
            fadd st(0), st(0)
            fidiv dword ptr [esp + 0x28]
            mov edx, eax
            mov dword ptr [esp + 0x48], edx
            mov edi, edx
    L004f722e:
            fld dword ptr [gFLOAT_0058afdc]
            fsub st(0), st(2)
            xor eax, eax
            mov ax, word ptr [esi]
            fmul dword ptr [gFLOAT_0058afdc+0x8]
            mov dword ptr [esp + 0x30], eax
            fild dword ptr [esp + 0x30]
            fld st(1)
            fcompp
            fnstsw ax
            test ah, 1
            je L004f7277
            call _ftol
            mov ebx, dword ptr [esp + 0x18]
            mov edx, dword ptr [esp + 0x1c]
            xor ecx, ecx
            mov word ptr [esi], ax
            mov cl, byte ptr [ebx]
            mov al, byte ptr [ecx + edx]
            mov ecx, dword ptr [esp + 0x24]
            mov edx, dword ptr [esp + 0x48]
            mov byte ptr [ebx], al
            jmp L004f727d
    L004f7277:
            mov ebx, dword ptr [esp + 0x18]
            fstp st(0)
    L004f727d:
            mov eax, dword ptr [esp + 0x2c]
            mov ebp, dword ptr [esp + 0x10]
            cmp eax, ebp
            je L004f7426
            test ecx, ecx
            jl L004f72c1
            mov edx, dword ptr [esp + 0x40]
            mov ebp, dword ptr [esp + 0x38]
            movsx eax, word ptr [edx + 0x28]
            mov edx, ebp
            imul edx, eax
            mov eax, dword ptr [esp + 0x44]
            add ebx, edx
            movsx eax, word ptr [eax + 0x28]
            cdq
            sub eax, edx
            mov edx, dword ptr [esp + 0x48]
            sar eax, 1
            imul eax, ebp
            lea esi, [esi + eax*2]
            mov eax, dword ptr [esp + 0x28]
            sub ecx, eax
    L004f72c1:
            mov eax, dword ptr [esp + 0x34]
            mov ebp, dword ptr [esp + 0x2c]
            add ebp, eax
            sub edi, 0x3e8
            mov dword ptr [esp + 0x2c], ebp
            mov ebp, dword ptr [esp + 0x20]
            add ebx, eax
            add ecx, ebp
            fld st(0)
            test edi, edi
            mov dword ptr [esp + 0x18], ebx
            lea esi, [esi + eax*2]
            mov dword ptr [esp + 0x24], ecx
            faddp st(2), st(0)
            jg L004f722e
            mov eax, dword ptr [esp + 0x4c]
            movsx eax, word ptr [eax + 0x28]
    L004f72fc:
            mov ebp, dword ptr [esp + 0x1c]
            add edi, edx
            add ebp, eax
            test edi, edi
            mov dword ptr [esp + 0x1c], ebp
            jle L004f72fc
            jmp L004f722e
    L004f7311:
            mov eax, ecx
            mov ecx, edi
            cdq
            fld dword ptr [ebp + 8]
            sub eax, edx
            sar eax, 1
            sub ecx, eax
            mov eax, dword ptr [esp + 0x20]
            fsub dword ptr [ebx + 8]
            lea eax, [eax + eax*4]
            mov dword ptr [esp + 0x24], ecx
            lea eax, [eax + eax*4]
            lea eax, [eax + eax*4]
            shl eax, 2
            cdq
            fadd st(0), st(0)
            idiv dword ptr [esp + 0x48]
            fidiv dword ptr [esp + 0x20]
            mov dword ptr [esp + 0x48], eax
            mov edi, eax
    L004f7347:
            fld dword ptr [gFLOAT_0058afdc]
            fsub st(0), st(2)
            xor edx, edx
            mov dx, word ptr [esi]
            fmul dword ptr [gFLOAT_0058afdc+0x8]
            mov dword ptr [esp + 0x2c], edx
            fild dword ptr [esp + 0x2c]
            fld st(1)
            fcompp
            fnstsw ax
            test ah, 1
            je L004f738c
            call _ftol
            mov edx, dword ptr [esp + 0x1c]
            mov word ptr [esi], ax
            mov eax, dword ptr [esp + 0x18]
            xor ecx, ecx
            mov cl, byte ptr [eax]
            mov cl, byte ptr [ecx + edx]
            mov byte ptr [eax], cl
            mov ecx, dword ptr [esp + 0x24]
            jmp L004f7392
    L004f738c:
            mov eax, dword ptr [esp + 0x18]
            fstp st(0)
    L004f7392:
            mov edx, dword ptr [esp + 0x30]
            mov ebx, dword ptr [esp + 0x14]
            cmp edx, ebx
            je L004f7426
            test ecx, ecx
            jl L004f73b5
            mov edx, dword ptr [esp + 0x34]
            add eax, edx
            lea esi, [esi + edx*2]
            mov edx, dword ptr [esp + 0x20]
            sub ecx, edx
    L004f73b5:
            mov ebx, dword ptr [esp + 0x38]
            mov edx, dword ptr [esp + 0x30]
            mov ebp, dword ptr [esp + 0x28]
            add edx, ebx
            mov dword ptr [esp + 0x30], edx
            mov edx, dword ptr [esp + 0x40]
            add ecx, ebp
            mov ebp, ebx
            movsx edx, word ptr [edx + 0x28]
            fld st(0)
            faddp st(2), st(0)
            imul ebp, edx
            add eax, ebp
            sub edi, 0x3e8
            mov dword ptr [esp + 0x18], eax
            mov eax, dword ptr [esp + 0x44]
            mov dword ptr [esp + 0x24], ecx
            movsx eax, word ptr [eax + 0x28]
            cdq
            sub eax, edx
            sar eax, 1
            imul eax, ebx
            test edi, edi
            lea esi, [esi + eax*2]
            jg L004f7347
            mov edx, dword ptr [esp + 0x4c]
            movsx eax, word ptr [edx + 0x28]
    L004f740d:
            mov ebp, dword ptr [esp + 0x48]
            mov ebx, dword ptr [esp + 0x1c]
            add edi, ebp
            add ebx, eax
            test edi, edi
            mov dword ptr [esp + 0x1c], ebx
            jle L004f740d
            jmp L004f7347
    L004f7426:
            fstp st(0)
            fstp st(0)
            mov eax, 1
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x2c
            ret 0x10
    L004f7439:
            pop edi
            pop esi
            pop ebp
            xor eax, eax
            pop ebx
            add esp, 0x2c
            ret 0x10
    }
#endif
}

void C2_HOOK_FASTCALL SetWorldToScreen(br_pixelmap* pScreen) {
    br_matrix4 mat;
    br_matrix4 mat2;

    BrMatrix4Perspective(&mat, gSpark_cam->field_of_view, gSpark_cam->aspect, -gSpark_cam->hither_z, -gSpark_cam->yon_z);
    BrMatrix4Scale(&mat2, (float)(pScreen->width / 2), (float)(pScreen->height / 2), 1.0f);
    BrMatrix4Mul(&gCameraToScreen, &mat, &mat2);
}

void C2_HOOK_FASTCALL ReplaySparks(br_pixelmap* pRender_screen, br_pixelmap* pDepth_buffer, br_actor* pCamera, tU32 pTime) {
    int i;
    br_vector3 pos;
    br_vector3 o;
    br_vector3 p;
    br_vector3 tv;
    br_vector3 new_pos;

    for (i = 0; i < CARPOCALYPSE2_ASIZE(gSparks); i++) {
        if (gSpark_flags & (1 << i)) {
            if (gSparks[i].car != NULL) {
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif
                BrMatrix34ApplyP(&tv, &o, &gSparks[i].car->car_master_actor->t.t.mat);
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
                BrVector3Copy(&o, &tv);
                BrMatrix34ApplyP(&pos, &gSparks[i].pos, &gSparks[i].car->car_master_actor->t.t.mat);
            } else {
                BrVector3Copy(&pos, &gSparks[i].pos);
            }
            BrVector3Add(&o, &pos, &gSparks[i].length);
            if (gRendering_mirror) {
                BrVector3Sub(&tv, &pos, (br_vector3*)gRearview_camera_to_world.m[3]);
                BrMatrix34TApplyV(&new_pos, &tv, &gRearview_camera_to_world);
                BrVector3Sub(&tv, &o, (br_vector3*)gRearview_camera_to_world.m[3]);
                BrMatrix34TApplyV(&p, &tv, &gRearview_camera_to_world);
            } else {
                BrVector3Sub(&tv, &pos, (br_vector3*)gCamera_to_world.m[3]);
                BrMatrix34TApplyV(&new_pos, &tv, &gCamera_to_world);
                BrVector3Sub(&tv, &o, (br_vector3*)gCamera_to_world.m[3]);
                BrMatrix34TApplyV(&p, &tv, &gCamera_to_world);
            }
            if (gNo_2d_effects) {
                SetLineModelCols(gSparks[i].colour);
            }
            if (gSparks[i].colour != 0) {
                DrawLine3D(&p, &new_pos, pRender_screen, pDepth_buffer, gFog_shade_table);
            } else {
                DrawLine3D(&p, &new_pos, pRender_screen, pDepth_buffer, gAcid_shade_table);
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x004f7450
void C2_NAKED C2_HOOK_FASTCALL RenderSparks(br_pixelmap* pRender_screen, br_pixelmap* pDepth_buffer, br_actor* pCamera, br_matrix34* pCamera_to_world, tU32 pTime) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;
    int time;
    br_vector3 tv;
    br_vector3 o;
    br_vector3 p;
    br_vector3 pos;
    br_vector3 new_pos;
    br_scalar ts;

    C2_HOOK_BUG_ON(sizeof(tSpark) != 0x40);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSpark, count, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSpark, pos, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSpark, v, 0x10);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSpark, length, 0x1c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSpark, normal, 0x28);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSpark, time_sync, 0x34);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSpark, car, 0x38);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSpark, colour, 0x3c);

    gSpark_cam = pCamera->type_data;
    SetWorldToScreen(pRender_screen);

    if (gSpark_flags == 0) {
        return;
    }

    if (gNo_2d_effects) {
        BrActorRemove(gLine_actor);
        BrActorAdd(pCamera, gLine_actor);
    }

    if (gAction_replay_mode) {
        ReplaySparks(pRender_screen, pDepth_buffer, pCamera, pTime);
        if (gNo_2d_effects) {
            BrActorRemove(gLine_actor);
            BrActorAdd(gDont_render_actor, gLine_actor);
        }
        return;
    }
    ARStartPipingSession(ePipe_chunk_spark);
    for (i = 0; i < CARPOCALYPSE2_ASIZE(gSparks); i++) {
        if (!((1u << i) & gSpark_flags)) {
            continue;
        }
        if (gSparks[i].count <= 0) {
            gSparks[i].count = 0;
            gSpark_flags &= ~(1u << i);
        }
        ts = BrVector3Dot(&gSparks[i].normal, &gSparks[i].v);
        BrVector3Scale(&tv, &gSparks[i].normal, ts);
        BrVector3Sub(&gSparks[i].v, &gSparks[i].v, &tv);
        if (gSparks[i].time_sync) {
            BrVector3Scale(&o, &gSparks[i].v, gSparks[i].time_sync / 1000.f);
            gSparks[i].count += gSparks[i].time_sync - pTime;
            gSparks[i].time_sync = 0;
        } else {
            BrVector3Scale(&o, &gSparks[i].v, pTime / 1000.f);
            gSparks[i].count -= pTime;
        }
        BrVector3Accumulate(&gSparks[i].pos, &o);
        time = 1000 - gSparks[i].count;
        if (time > 150) {
            time = 150;
        }
        ts = -time / 1000.f;
        if (gSparks[i].colour) {
            ts /= 2.0;
        }
        BrVector3Scale(&gSparks[i].length, &gSparks[i].v, ts);
        ts = pTime * 10.f / (WORLD_SCALE * 100.f);
        if (gSparks[i].car != NULL) {
            BrMatrix34ApplyV(&tv, &gSparks[i].length, &gSparks[i].car->car_master_actor->t.t.mat);
            BrVector3Copy(&gSparks[i].length, &tv);
            BrMatrix34ApplyP(&pos, &gSparks[i].pos, &gSparks[i].car->car_master_actor->t.t.mat);
            BrVector3Copy(&o, &tv);
            gSparks[i].v.v[0] -= gSparks[i].car->car_master_actor->t.t.mat.m[0][1] * ts;
            gSparks[i].v.v[1] -= gSparks[i].car->car_master_actor->t.t.mat.m[1][1] * ts;
            gSparks[i].v.v[2] -= gSparks[i].car->car_master_actor->t.t.mat.m[2][1] * ts;
        } else {
            BrVector3Copy(&pos, &gSparks[i].pos);
            gSparks[i].v.v[1] -= ts;
        }
        AddSparkToPipingSession(i + (gSparks[i].colour << 8), &pos, &gSparks[i].length);
        BrVector3Add(&o, &gSparks[i].length, &pos);
        if (gRendering_mirror) {
            BrVector3Sub(&tv, &pos, (br_vector3*)gRearview_camera_to_world.m[3]);
            BrMatrix34TApplyV(&new_pos, &tv, &gRearview_camera_to_world);
            BrVector3Sub(&tv, &o, (br_vector3*)gRearview_camera_to_world.m[3]);
            BrMatrix34TApplyV(&p, &tv, &gRearview_camera_to_world);
        } else {
            BrVector3Sub(&tv, &pos, (br_vector3*)gCamera_to_world.m[3]);
            BrMatrix34TApplyV(&new_pos, &tv, &gCamera_to_world);
            BrVector3Sub(&tv, &o, (br_vector3*)gCamera_to_world.m[3]);
            BrMatrix34TApplyV(&p, &tv, &gCamera_to_world);
        }
        BrVector3SetFloat(&tv, FRandomBetween(-0.1f, 0.1f), FRandomBetween(-0.1f, 0.1f), FRandomBetween(-0.1f, 0.1f));
        BrVector3Accumulate(&gSparks[i].v, &tv);
        ts = 1.0f - BrVector3Length(&gSparks[i].v) / 1.4f * pTime / 1000.0f;
        if (ts < 0.1f) {
            ts = 0.1f;
        }
        BrVector3Scale(&gSparks[i].v, &gSparks[i].v, ts);
        if (gNo_2d_effects) {
            SetLineModelCols(gSparks[i].colour);
        }
        if (gSparks[i].colour != 0) {
            DrawLine3D(&p, &new_pos, pRender_screen, pDepth_buffer, gFog_shade_table);
        } else {
            DrawLine3D(&p, &new_pos, pRender_screen, pDepth_buffer, gAcid_shade_table);
        }
    }
    AREndPipingSession();
    if (gNo_2d_effects) {
        BrActorRemove(gLine_actor);
        BrActorAdd(gDont_render_actor, gLine_actor);
    }
#else
    __asm {
            sub esp, 0x100
            push ebx
            push ebp
            push esi
            mov esi, dword ptr [esp + 0x110]
            mov ebx, ecx
            push edi
            mov eax, dword ptr [esi + 0x5c]
            push ecx
            mov dword ptr [gSpark_cam], eax
            mov ebp, edx
            fld dword ptr [eax + 0xc]
            fchs
            _emit 0xd9
        _emit 0x1c
        _emit 0x24
            fld dword ptr [eax + 8]
            mov dx, word ptr [eax + 6]
            push ecx
            mov ecx, dword ptr [eax + 0x10]
            lea eax, [esp + 0xd8]
            fchs
            _emit 0xd9
        _emit 0x1c
        _emit 0x24
            push ecx
            push edx
            push eax
            mov dword ptr [esp + 0x78], ebp
            mov dword ptr [esp + 0x7c], ebx
            call BrMatrix4Perspective
            mov cx, word ptr [ebx + 0x36]
            mov dx, word ptr [ebx + 0x34]
            shr cx, 1
            and ecx, 0xffff
            add esp, 0x14
            mov dword ptr [esp + 0x20], ecx
            lea eax, [esp + 0x90]
            fild dword ptr [esp + 0x20]
            push 0x3f800000
            push ecx
            shr dx, 1
            _emit 0xd9
        _emit 0x1c
        _emit 0x24
            and edx, 0xffff
            push ecx
            mov dword ptr [esp + 0x2c], edx
            fild dword ptr [esp + 0x2c]
            _emit 0xd9
        _emit 0x1c
        _emit 0x24
            push eax
            call BrMatrix4Scale
            add esp, 0x10
            lea ecx, [esp + 0x90]
            lea edx, [esp + 0xd0]
            push ecx
            push edx
            push offset gCameraToScreen
            call BrMatrix4Mul
            mov eax, dword ptr [gSpark_flags]
            xor edi, edi
            add esp, 0xc
            cmp eax, edi
            je L004f7c99
            cmp dword ptr [gNo_2d_effects], edi
            je L004f7539
            mov eax, dword ptr [gLine_actor]
            push eax
            call BrActorRemove
            mov ecx, dword ptr [gLine_actor]
            add esp, 4
            push ecx
            push esi
            call BrActorAdd
            add esp, 8
    L004f7539:
            cmp dword ptr [gPed_676914], edi
            je L004f7776
            mov esi, offset gSparks+0x38
    L004f754a:
            mov eax, dword ptr [gSpark_flags]
            mov edx, 1
            mov ecx, edi
            shl edx, cl
            test edx, eax
            je L004f7738
            mov eax, dword ptr [esi]
            test eax, eax
            je L004f75b3
            mov ecx, dword ptr [eax + 0x10]
            lea edx, [esp + 0x3c]
            add ecx, 0x2c
            lea eax, [esp + 0x24]
            push ecx
            push edx
            push eax
            call BrMatrix34ApplyP
            mov ecx, dword ptr [esp + 0x30]
            mov edx, dword ptr [esp + 0x34]
            mov eax, dword ptr [esp + 0x38]
            mov dword ptr [esp + 0x48], ecx
            mov ecx, dword ptr [esi]
            mov dword ptr [esp + 0x4c], edx
            mov dword ptr [esp + 0x50], eax
            add esp, 0xc
            mov edx, dword ptr [ecx + 0x10]
            lea eax, [esi - 0x34]
            add edx, 0x2c
            lea ecx, [esp + 0x30]
            push edx
            push eax
            push ecx
            call BrMatrix34ApplyP
            add esp, 0xc
            jmp L004f75c8
    L004f75b3:
            mov edx, dword ptr [esi - 0x34]
            mov eax, dword ptr [esi - 0x30]
            mov ecx, dword ptr [esi - 0x2c]
            mov dword ptr [esp + 0x30], edx
            mov dword ptr [esp + 0x34], eax
            mov dword ptr [esp + 0x38], ecx
    L004f75c8:
            fld dword ptr [esi - 0x1c]
            fld dword ptr [esi - 0x18]
            fld dword ptr [esi - 0x14]
            fxch st(2)
            fadd dword ptr [esp + 0x30]
            fxch st(1)
            fadd dword ptr [esp + 0x34]
            fxch st(2)
            fadd dword ptr [esp + 0x38]
            fxch st(1)
            fstp dword ptr [esp + 0x3c]
            fxch st(1)
            fstp dword ptr [esp + 0x40]
            mov eax, dword ptr [gRendering_mirror]
            fstp dword ptr [esp + 0x44]
            fld dword ptr [esp + 0x30]
            fld dword ptr [esp + 0x34]
            fld dword ptr [esp + 0x38]
            fxch st(2)
            test eax, eax
            je L004f767e
            fsub dword ptr [gRearview_camera_to_world+0x24]
            fxch st(1)
            fsub dword ptr [gRearview_camera_to_world+0x28]
            fxch st(2)
            fsub dword ptr [gRearview_camera_to_world+0x2c]
            fxch st(1)
            fstp dword ptr [esp + 0x24]
            fxch st(1)
            fstp dword ptr [esp + 0x28]
            lea edx, [esp + 0x24]
            push offset gRearview_camera_to_world
            fstp dword ptr [esp + 0x30]
            lea eax, [esp + 0x58]
            push edx
            push eax
            call BrMatrix34TApplyV
            fld dword ptr [esp + 0x48]
            fld dword ptr [esp + 0x4c]
            fld dword ptr [esp + 0x50]
            fxch st(2)
            fsub dword ptr [gRearview_camera_to_world+0x24]
            fxch st(1)
            fsub dword ptr [gRearview_camera_to_world+0x28]
            fxch st(2)
            fsub dword ptr [gRearview_camera_to_world+0x2c]
            fxch st(1)
            add esp, 0xc
            lea ecx, [esp + 0x24]
            lea edx, [esp + 0x6c]
            push offset gRearview_camera_to_world
            push ecx
            push edx
            jmp L004f76f0
    L004f767e:
            fsub dword ptr [gCamera_to_world+0x24]
            fxch st(1)
            fsub dword ptr [gCamera_to_world+0x28]
            fxch st(2)
            fsub dword ptr [gCamera_to_world+0x2c]
            fxch st(1)
            fstp dword ptr [esp + 0x24]
            fxch st(1)
            fstp dword ptr [esp + 0x28]
            lea eax, [esp + 0x24]
            push offset gCamera_to_world
            fstp dword ptr [esp + 0x30]
            lea ecx, [esp + 0x58]
            push eax
            push ecx
            call BrMatrix34TApplyV
            fld dword ptr [esp + 0x48]
            fld dword ptr [esp + 0x4c]
            fld dword ptr [esp + 0x50]
            fxch st(2)
            fsub dword ptr [gCamera_to_world+0x24]
            fxch st(1)
            fsub dword ptr [gCamera_to_world+0x28]
            fxch st(2)
            fsub dword ptr [gCamera_to_world+0x2c]
            fxch st(1)
            add esp, 0xc
            lea edx, [esp + 0x24]
            lea eax, [esp + 0x6c]
            push offset gCamera_to_world
            push edx
            push eax
    L004f76f0:
            fstp dword ptr [esp + 0x30]
            fxch st(1)
            fstp dword ptr [esp + 0x34]
            fstp dword ptr [esp + 0x38]
            call BrMatrix34TApplyV
            mov eax, dword ptr [gNo_2d_effects]
            add esp, 0xc
            test eax, eax
            je L004f7717
            mov cl, byte ptr [esi + 4]
            call SetLineModelCols
    L004f7717:
            mov al, byte ptr [esi + 4]
            test al, al
            mov eax, dword ptr [gFog_shade_table]
            jne L004f7728
            mov eax, dword ptr [gAcid_shade_table]
    L004f7728:
            push eax
            push ebp
            push ebx
            lea edx, [esp + 0x60]
            lea ecx, [esp + 0x78]
            call DrawLine3D
    L004f7738:
            add esi, 0x40
            inc edi
            cmp esi, offset gReplay_splashes+0x30
            jl L004f754a
            mov eax, dword ptr [gNo_2d_effects]
            test eax, eax
            je L004f7c99
            mov ecx, dword ptr [gLine_actor]
            push ecx
            call BrActorRemove
            mov edx, dword ptr [gLine_actor]
            mov eax, dword ptr [gDont_render_actor]
            add esp, 4
            push edx
            push eax
            jmp L004f7c91
    L004f7776:
            mov ecx, 0xa
            call ARStartPipingSession
            mov ebx, dword ptr [esp + 0x11c]
            xor ecx, ecx
            mov dword ptr [esp + 0x60], ecx
            mov esi, offset gSparks+0x34
    L004f7792:
            mov eax, 1
            shl eax, cl
            test dword ptr [gSpark_flags], eax
            je L004f7c50
            cmp dword ptr [esi - 0x34], edi
            jg L004f77bd
            mov ecx, dword ptr [gSpark_flags]
            mov dword ptr [esi - 0x34], edi
            not eax
            and ecx, eax
            mov dword ptr [gSpark_flags], ecx
    L004f77bd:
            fld dword ptr [esi - 0x24]
            fld dword ptr [esi - 0x1c]
            fld dword ptr [esi - 0xc]
            fld dword ptr [esi - 4]
            fld dword ptr [esi - 0x20]
            fld dword ptr [esi - 8]
            fxch st(2)
            fmulp st(4), st(0)
            fxch st(2)
            fmulp st(4), st(0)
            fxch st(3)
            faddp st(2), st(0)
            fxch st(2)
            fmulp st(2), st(0)
            fld dword ptr [esi - 0xc]
            fxch st(2)
            faddp st(1), st(0)
            fld dword ptr [esi - 8]
            fxch st(2)
            fmul st(0), st(1)
            fld dword ptr [esi - 4]
            fxch st(3)
            fmul st(0), st(2)
            fxch st(1)
            fstp dword ptr [esp + 0x14]
            fstp dword ptr [esp + 0x18]
            fxch st(1)
            fmul st(0), st(1)
            fld dword ptr [esi - 0x24]
            fsub dword ptr [esp + 0x14]
            fxch st(1)
            fstp dword ptr [esp + 0x1c]
            fstp dword ptr [esi - 0x24]
            fstp st(0)
            fld dword ptr [esi - 0x20]
            fsub dword ptr [esp + 0x18]
            fstp dword ptr [esi - 0x20]
            fld dword ptr [esi - 0x1c]
            fsub dword ptr [esp + 0x1c]
            fst dword ptr [esi - 0x1c]
            mov eax, dword ptr [esi]
            fstp dword ptr [esp + 0x10]
            cmp eax, edi
            je L004f7870
            mov dword ptr [esp + 0x3c], eax
            mov dword ptr [esp + 0x40], edi
            fild qword ptr [esp + 0x3c]
            fld dword ptr [esi - 0x24]
            fld dword ptr [esi - 0x20]
            fxch st(2)
            fmul dword ptr [gFLOAT_0058afdc+0xc]
            mov ecx, dword ptr [esi - 0x34]
            sub eax, ebx
            fld st(0)
            fxch st(2)
            fmul st(0), st(1)
            fxch st(3)
            fmul st(0), st(1)
            fxch st(2)
            fmul dword ptr [esp + 0x10]
            fxch st(3)
            fstp st(1)
            add ecx, eax
            mov dword ptr [esi - 0x34], ecx
            mov dword ptr [esi], edi
            fxch st(2)
            jmp L004f78a8
    L004f7870:
            mov dword ptr [esp + 0x30], ebx
            mov dword ptr [esp + 0x34], edi
            fild qword ptr [esp + 0x30]
            fld dword ptr [esi - 0x24]
            fld dword ptr [esi - 0x20]
            fxch st(2)
            fmul dword ptr [gFLOAT_0058afdc+0xc]
            mov eax, dword ptr [esi - 0x34]
            fld st(0)
            fxch st(2)
            fmul st(0), st(1)
            fxch st(3)
            fmul st(0), st(1)
            fxch st(2)
            fmul dword ptr [esp + 0x10]
            fxch st(3)
            fstp st(1)
            sub eax, ebx
            fxch st(2)
            mov dword ptr [esi - 0x34], eax
    L004f78a8:
            lea ebp, [esi - 0x30]
            mov eax, 0x3e8
            fxch st(2)
            _emit 0xd8
        _emit 0x45
        _emit 0x00
            _emit 0xd9
        _emit 0x5d
        _emit 0x00
            fadd dword ptr [esi - 0x2c]
            fstp dword ptr [esi - 0x2c]
            fadd dword ptr [esi - 0x28]
            fstp dword ptr [esi - 0x28]
            sub eax, dword ptr [esi - 0x34]
            cmp eax, 0x96
            jle L004f78d3
            mov eax, 0x96
    L004f78d3:
            neg eax
            mov dword ptr [esp + 0x20], eax
            mov al, byte ptr [esi + 8]
            fild dword ptr [esp + 0x20]
            test al, al
            fmul dword ptr [gFLOAT_0058afdc+0xc]
            je L004f78f0
            fmul dword ptr [gFLOAT_0058afec]
    L004f78f0:
            fld dword ptr [esi - 0x24]
            fmul st(0), st(1)
            lea edi, [esi - 0x18]
            mov dword ptr [esp + 0x24], ebx
            mov dword ptr [esp + 0x28], 0
            _emit 0xd9
        _emit 0x1f
            fild qword ptr [esp + 0x24]
            fld dword ptr [esi - 0x20]
            fmul st(0), st(2)
            fxch st(1)
            fst dword ptr [esp + 0x20]
            fxch st(1)
            fstp dword ptr [esi - 0x14]
            fld dword ptr [esi - 0x1c]
            fxch st(1)
            fmul dword ptr [gFLOAT_0058aff0]
            fxch st(1)
            fmul st(0), st(2)
            fxch st(1)
            fmul qword ptr [gFLOAT_0058aff0+0x8]
            fxch st(1)
            fstp dword ptr [esi - 0x10]
            mov eax, dword ptr [esi + 4]
            test eax, eax
            fstp dword ptr [esp + 0x10]
            fstp st(0)
            je L004f79c1
            mov ecx, dword ptr [eax + 0x10]
            lea edx, [esp + 0x14]
            add ecx, 0x2c
            push ecx
            push edi
            push edx
            call BrMatrix34ApplyV
            fld dword ptr [esp + 0x20]
            fld dword ptr [esp + 0x24]
            fld dword ptr [esp + 0x28]
            fxch st(2)
            _emit 0xd9
        _emit 0x1f
            add esp, 0xc
            lea ecx, [esp + 0x48]
            fstp dword ptr [esi - 0x14]
            fstp dword ptr [esi - 0x10]
            mov eax, dword ptr [esi + 4]
            mov edx, dword ptr [eax + 0x10]
            add edx, 0x2c
            push edx
            push ebp
            push ecx
            call BrMatrix34ApplyP
            mov eax, dword ptr [esi + 4]
            add esp, 0xc
            mov ecx, dword ptr [eax + 0x10]
            fld dword ptr [ecx + 0x30]
            fmul dword ptr [esp + 0x10]
            fsubr dword ptr [esi - 0x24]
            fstp dword ptr [esi - 0x24]
            mov edx, dword ptr [esi + 4]
            mov eax, dword ptr [edx + 0x10]
            fld dword ptr [eax + 0x3c]
            fmul dword ptr [esp + 0x10]
            fsubr dword ptr [esi - 0x20]
            fstp dword ptr [esi - 0x20]
            mov ecx, dword ptr [esi + 4]
            mov edx, dword ptr [ecx + 0x10]
            fld dword ptr [edx + 0x48]
            fmul dword ptr [esp + 0x10]
            fsubr dword ptr [esi - 0x1c]
            fstp dword ptr [esi - 0x1c]
            jmp L004f79e0
    L004f79c1:
            fld dword ptr [esi - 0x20]
            fsub dword ptr [esp + 0x10]
            mov eax, dword ptr [ebp]
            mov ecx, dword ptr [esi - 0x2c]
            mov edx, dword ptr [esi - 0x28]
            mov dword ptr [esp + 0x48], eax
            mov dword ptr [esp + 0x4c], ecx
            mov dword ptr [esp + 0x50], edx
            fstp dword ptr [esi - 0x20]
    L004f79e0:
            mov ebp, dword ptr [esp + 0x60]
            xor ecx, ecx
            mov cl, byte ptr [esi + 8]
            push edi
            shl ecx, 8
            lea edx, [esp + 0x4c]
            add ecx, ebp
            call AddSparkToPipingSession
            fld dword ptr [esp + 0x48]
            _emit 0xd8
        _emit 0x07
            mov eax, dword ptr [gRendering_mirror]
            test eax, eax
            fstp dword ptr [esp + 0x54]
            fld dword ptr [esp + 0x4c]
            fadd dword ptr [esi - 0x14]
            fstp dword ptr [esp + 0x58]
            fld dword ptr [esp + 0x50]
            fadd dword ptr [esi - 0x10]
            fstp dword ptr [esp + 0x5c]
            fld dword ptr [esp + 0x48]
            fld dword ptr [esp + 0x4c]
            fld dword ptr [esp + 0x50]
            fxch st(2)
            je L004f7aa6
            fsub dword ptr [gRearview_camera_to_world+0x24]
            fxch st(1)
            fsub dword ptr [gRearview_camera_to_world+0x28]
            fxch st(2)
            fsub dword ptr [gRearview_camera_to_world+0x2c]
            fxch st(1)
            fstp dword ptr [esp + 0x14]
            fxch st(1)
            fstp dword ptr [esp + 0x18]
            lea eax, [esp + 0x14]
            push offset gRearview_camera_to_world
            fstp dword ptr [esp + 0x20]
            lea ecx, [esp + 0x88]
            push eax
            push ecx
            call BrMatrix34TApplyV
            fld dword ptr [esp + 0x60]
            fld dword ptr [esp + 0x64]
            fld dword ptr [esp + 0x68]
            fxch st(2)
            fsub dword ptr [gRearview_camera_to_world+0x24]
            fxch st(1)
            fsub dword ptr [gRearview_camera_to_world+0x28]
            fxch st(2)
            fsub dword ptr [gRearview_camera_to_world+0x2c]
            fxch st(1)
            add esp, 0xc
            lea edx, [esp + 0x14]
            lea eax, [esp + 0x78]
            push offset gRearview_camera_to_world
            push edx
            push eax
            jmp L004f7b1b
    L004f7aa6:
            fsub dword ptr [gCamera_to_world+0x24]
            fxch st(1)
            fsub dword ptr [gCamera_to_world+0x28]
            fxch st(2)
            fsub dword ptr [gCamera_to_world+0x2c]
            fxch st(1)
            fstp dword ptr [esp + 0x14]
            fxch st(1)
            fstp dword ptr [esp + 0x18]
            lea ecx, [esp + 0x14]
            push offset gCamera_to_world
            fstp dword ptr [esp + 0x20]
            lea edx, [esp + 0x88]
            push ecx
            push edx
            call BrMatrix34TApplyV
            fld dword ptr [esp + 0x60]
            fld dword ptr [esp + 0x64]
            fld dword ptr [esp + 0x68]
            fxch st(2)
            fsub dword ptr [gCamera_to_world+0x24]
            fxch st(1)
            fsub dword ptr [gCamera_to_world+0x28]
            fxch st(2)
            fsub dword ptr [gCamera_to_world+0x2c]
            fxch st(1)
            add esp, 0xc
            lea eax, [esp + 0x14]
            lea ecx, [esp + 0x78]
            push offset gCamera_to_world
            push eax
            push ecx
    L004f7b1b:
            fstp dword ptr [esp + 0x20]
            fxch st(1)
            fstp dword ptr [esp + 0x24]
            fstp dword ptr [esp + 0x28]
            call BrMatrix34TApplyV
            add esp, 0xc
            push 0x3dcccccd
            push 0xbdcccccd
            call FRandomBetween
            push ecx
            _emit 0xd9
        _emit 0x1c
        _emit 0x24
            push 0x3dcccccd
            push 0xbdcccccd
            call FRandomBetween
            push ecx
            _emit 0xd9
        _emit 0x1c
        _emit 0x24
            push 0x3dcccccd
            push 0xbdcccccd
            call FRandomBetween
            lea edx, [esp + 0x1c]
            push ecx
            _emit 0xd9
        _emit 0x1c
        _emit 0x24
            push edx
            call BrVector3SetFloat
            fld dword ptr [esp + 0x24]
            fadd dword ptr [esi - 0x24]
            fld dword ptr [esp + 0x28]
            fld dword ptr [esp + 0x2c]
            fxch st(2)
            fstp dword ptr [esi - 0x24]
            add esp, 0x10
            fadd dword ptr [esi - 0x20]
            fstp dword ptr [esi - 0x20]
            fadd dword ptr [esi - 0x1c]
            fst dword ptr [esi - 0x1c]
            fld dword ptr [esi - 0x20]
            fld dword ptr [esi - 0x24]
            fst dword ptr [esp + 0x10]
            fxch st(1)
            fxch st(2)
            fxch st(1)
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
            fmul dword ptr [gFLOAT_0058aff0+0x10]
            fmul dword ptr [esp + 0x20]
            fmul dword ptr [gFLOAT_0058afdc+0xc]
            fsubr dword ptr [gFLOAT_0058afdc]
            fcom dword ptr [gFLOAT_0058aff0+0x14]
            fnstsw ax
            test ah, 1
            je L004f7bf4
            fstp st(0)
            fld dword ptr [gFLOAT_0058aff0+0x14]
    L004f7bf4:
            fld dword ptr [esi - 0x24]
            fmul st(0), st(1)
            mov eax, dword ptr [gNo_2d_effects]
            test eax, eax
            fstp dword ptr [esi - 0x24]
            fld dword ptr [esi - 0x20]
            fmul st(0), st(1)
            fstp dword ptr [esi - 0x20]
            fld dword ptr [esi - 0x1c]
            fmul st(0), st(1)
            fstp dword ptr [esi - 0x1c]
            fstp st(0)
            je L004f7c1f
            mov cl, byte ptr [esi + 8]
            call SetLineModelCols
    L004f7c1f:
            mov al, byte ptr [esi + 8]
            test al, al
            mov eax, dword ptr [gFog_shade_table]
            jne L004f7c30
            mov eax, dword ptr [gAcid_shade_table]
    L004f7c30:
            mov ecx, dword ptr [esp + 0x68]
            push eax
            mov eax, dword ptr [esp + 0x68]
            lea edx, [esp + 0x88]
            push eax
            push ecx
            lea ecx, [esp + 0x84]
            call DrawLine3D
            xor edi, edi
    L004f7c50:
            mov ecx, dword ptr [esp + 0x60]
            add esi, 0x40
            inc ecx
            cmp esi, offset gReplay_splashes+0x2c
            mov dword ptr [esp + 0x60], ecx
            jl L004f7792
            call AREndPipingSession
            cmp dword ptr [gNo_2d_effects], edi
            je L004f7c99
            mov edx, dword ptr [gLine_actor]
            push edx
            call BrActorRemove
            mov eax, dword ptr [gLine_actor]
            mov ecx, dword ptr [gDont_render_actor]
            add esp, 4
            push eax
            push ecx
    L004f7c91:
            call BrActorAdd
            add esp, 8
    L004f7c99:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x100
            ret 0xc
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004fb2f0
int C2_HOOK_CDECL CmpSmokeZ(const void* p1, const void* p2) {
    tBRender_smoke** a;
    tBRender_smoke** b;

    a = (tBRender_smoke**)p1;
    b = (tBRender_smoke**)p2;
    if ((*a)->pos.v[2] < (*b)->pos.v[2]) {
        return -1;
    } else if ((*a)->pos.v[2] > (*b)->pos.v[2]) {
        return 1;
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x004fb1b0
void C2_HOOK_FASTCALL RenderRecordedSmokeCircles(void) {
    int i;
    int j;
    tBRender_smoke* smoke;
    br_model* model;
    tU8 red;
    tU8 grn;
    tU8 blu;

    BrQsort(gBR_smoke_pointers, gN_BR_smoke_structs, sizeof(gBR_smoke_pointers[0]), CmpSmokeZ);
    for (i = gN_BR_smoke_structs - 1; i >= 0; i--) {
        smoke = gBR_smoke_pointers[i];
        BrVector3Copy(&gBlend_actor->t.t.translate.t, &smoke->pos);
        gBlend_actor->t.t.mat.m[0][0] = smoke->r;
        gBlend_actor->t.t.mat.m[1][1] = smoke->r / smoke->aspect;
        gBlend_actor->material = smoke->material;
        smoke->material->extra_prim[1].v.x = BrFloatToFixed(smoke->strength * 150.f);
        BrMaterialUpdate(smoke->material, BR_MATU_EXTRA_PRIM);
        red = BR_RED(smoke->col);
        grn = BR_GRN(smoke->col);
        blu = BR_BLU(smoke->col);
        model = gBR_smoke_pointers[i]->model;
        for (j = 0; j < model->nvertices; j++) {
            model->vertices[j].red = red;
            model->vertices[j].grn = grn;
            model->vertices[j].blu = blu;
        }
        BrModelUpdate(model, BR_MODU_VERTEX_COLOURS);
        gBlend_actor->model = model;
        BrZbsSceneRenderAdd(gBlend_actor);
    }
}

void C2_HOOK_FASTCALL RecordSmokeCircle(br_vector3* pCent, br_scalar pR, br_scalar pStrength, br_colour pShade, br_scalar pAspect) {

    if (gRendering_mirror) {
        DRMatrix34TApplyP(&gBR_smoke_structs[gN_BR_smoke_structs].pos, pCent, &gRearview_camera_to_world);
    } else {
        DRMatrix34TApplyP(&gBR_smoke_structs[gN_BR_smoke_structs].pos, pCent, &gCamera_to_world);
    }

    gBR_smoke_structs[gN_BR_smoke_structs].r = pR;
    gBR_smoke_structs[gN_BR_smoke_structs].strength = pStrength;
    gBR_smoke_structs[gN_BR_smoke_structs].col = pShade;
    gBR_smoke_structs[gN_BR_smoke_structs].aspect = pAspect;
    gBR_smoke_pointers[gN_BR_smoke_structs] = &gBR_smoke_structs[gN_BR_smoke_structs];
    if (pShade == gShade_list[7]) {
        gBR_smoke_structs[gN_BR_smoke_structs].model = gBlend_model2;
    } else {
        gBR_smoke_structs[gN_BR_smoke_structs].model = gBlend_model;
    }
    gN_BR_smoke_structs += 1;
}

void C2_HOOK_FASTCALL SmokeCircle3D(br_vector3* o, br_scalar r, br_scalar strength, br_scalar pAspect, br_pixelmap* pRender_screen, br_pixelmap* pDepth_buffer, br_colour pShade, br_actor* pCamera) {

    if (gNo_2d_effects) {
        RecordSmokeCircle(o, r, strength, pShade, pAspect);
    }
}

void C2_HOOK_FASTCALL DrawTheGlow(br_pixelmap* pRender_screen, br_pixelmap* pDepth_buffer, br_actor* pCamera) {
    int i;
    br_scalar strength;
    br_vector3 tv;
    tU32 seed;

    if (gColumn_flags != 0) {
        seed = rand();
        srand(GetTotalTime());
for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gSmoke_column); i++) {
            if (((1u << i) & gColumn_flags) != 0 && gSmoke_column[i].colour <= 1) {
                strength = 0.5f;
                if (gSmoke_column[i].lifetime < 4000) {
                    strength = gSmoke_column[i].lifetime * 0.5f / 4000.f;
                }
                BrVector3Set(&tv, gSmoke_column[i].pos.v[0], gSmoke_column[i].pos.v[1] + 0.02f, gSmoke_column[i].pos.v[2]);
                SmokeCircle3D(&tv, 0.07f, strength, SRandomBetween(0.5f, 0.99f), pRender_screen, pDepth_buffer, gShade_list[7], pCamera);
            }
        }
        srand(seed);
    }
}

void C2_HOOK_FASTCALL ReplaySmoke(br_pixelmap* pRender_screen, br_pixelmap* pDepth_buffer, br_actor* pCamera) {
    br_scalar aspect;
    int i;

    for (i = 0; i < CARPOCALYPSE2_ASIZE(gSmoke); i++) {
        if (gSmoke_flags & (1u << i)) {
            aspect = 1.f + (gSmoke[i].radius - .05f) * 4.f / 2.f;
            if (gSmoke[i].type & 0x10) {
                SmokeCircle3D(&gSmoke[i].pos, gSmoke[i].radius / aspect, gSmoke[i].strength, 1.f,
                    pRender_screen, pDepth_buffer, gShade_list[gSmoke[i].type & 0xf], pCamera);
            } else {
                SmokeCircle3D(&gSmoke[i].pos, gSmoke[i].radius, gSmoke[i].strength, aspect,
                    pRender_screen, pDepth_buffer, gShade_list[gSmoke[i].type & 0xf], pCamera);
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x004fa910
void C2_NAKED C2_HOOK_FASTCALL RenderSmoke(br_pixelmap* pRender_screen, br_pixelmap* pDepth_buffer, br_actor* pCamera, br_matrix34* pCamera_to_world, tU32 pTime) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;
    int j;
    br_vector3 tv;
    br_scalar aspect;
    br_scalar ts;
    tU32 seed;
    tU32 not_lonely;

    not_lonely = 0;
    if (gNo_2d_effects) {
        gBlend_actor->render_style = BR_RSTYLE_FACES;
        if (gBlend_actor->parent != NULL) {
            BrActorRemove(gBlend_actor);
        }
        BrActorAdd(pCamera, gBlend_actor);
        gN_BR_smoke_structs = 0;
    }
    DrawTheGlow(pRender_screen, pDepth_buffer, pCamera);

    if (gSmoke_flags == 0) {
        if (gNo_2d_effects) {
            if (gBlend_actor->parent != NULL) {
                BrActorRemove(gBlend_actor);
            }
            BrActorAdd(gDont_render_actor, gBlend_actor);
        }
        return;
    }

    seed = rand();
    if (gAction_replay_mode) {
        ReplaySmoke(pRender_screen, pDepth_buffer, pCamera);
        srand(seed);
        if (gNo_2d_effects) {
            RenderRecordedSmokeCircles();
            if (gBlend_actor->parent != NULL) {
                BrActorRemove(gBlend_actor);
            }
            BrActorAdd(gDont_render_actor, gBlend_actor);
        }
        return;
    }
    ARStartPipingSession(ePipe_chunk_smoke);
    for (i = 0; i < CARPOCALYPSE2_ASIZE(gSmoke); i++) {
        if (gSmoke_flags & (1u << i)) {
            if (gSmoke[i].strength > 0.0) {
                if (gSmoke[i].time_sync != 0) {
                    BrVector3Scale(&tv, &gSmoke[i].v, gSmoke[i].time_sync / 1000.0f);
                    gSmoke[i].time_sync = 0;
                } else {
                    BrVector3Scale(&tv, &gSmoke[i].v, pTime / 1000.0f);
                }
                BrVector3Accumulate(&gSmoke[i].pos, &tv);
            } else {
                gSmoke_flags &= ~(1u << i);
            }
        }
    }
    for (i = 0; i < CARPOCALYPSE2_ASIZE(gSmoke); i++) {
        if (gSmoke_flags & (1u << i)) {
            if ((gSmoke[i].type & 0xf) == 7) {
                not_lonely |= 1u << i;
            } else if (!(not_lonely & (1u << i))) {
                for (j = i + 1; j < CARPOCALYPSE2_ASIZE(gSmoke); j++) {
                    if ((gSmoke_flags & (1u << j))) {
                        BrVector3Sub(&tv, &gSmoke[i].pos, &gSmoke[i].pos);
                        ts = BrVector3LengthSquared(&tv);
                        if (CARPOCALYPSE2_SQR(gSmoke[i].radius + gSmoke[j].radius) > ts) {
                            not_lonely |= (1u << j) | (1u << i);
                            break;
                        }
                    }
                }
            }
            if (!((1u << i) & not_lonely)) {
                gSmoke[i].strength = gSmoke[i].strength / 2.f;
            }
            aspect = 1.f + (gSmoke[i].radius - .05f) * 4.f / 2.f;
            if ((gSmoke[i].type & 0x10)) {
                SmokeCircle3D(&gSmoke[i].pos, gSmoke[i].radius / aspect, gBR_smoke_structs[i].strength, 1.0, pRender_screen, pDepth_buffer, gShade_list[gSmoke[i].type & 0xf], pCamera);
            } else {
                SmokeCircle3D(&gSmoke[i].pos, gSmoke[i].radius, gBR_smoke_structs[i].strength, aspect, pRender_screen, pDepth_buffer, gShade_list[gSmoke[i].type & 0xf], pCamera);
            }
            if (gSmoke[i].pipe_me) {
                AddSmokeToPipingSession(i, gSmoke[i].type, &gSmoke[i].pos, gSmoke[i].radius, gSmoke[i].strength);
            }
            gSmoke[i].radius = pTime / 1000.0f * gSmoke[i].strength * .5f + gSmoke[i].radius;
            gSmoke[i].strength -= pTime * gSmoke[i].decay_factor / 1000.0f;
            if (gSmoke[i].radius > 0.3f) {
                gSmoke[i].radius = 0.3f;
            }
            if (gSmoke[i].strength > 0.0f) {
                ts = 1.0f - pTime * 0.002f;
                if (ts < 0.5f) {
                    ts = 0.5f;
                }
                BrVector3Scale(&gSmoke[i].v, &gSmoke[i].v, ts);
                if (fabs(gSmoke[i].v.v[1]) < 0.43478259f && (gSmoke[i].type & 0xfu) < 7) {
                    if (gSmoke[i].v.v[1] >= 0.0) {
                        gSmoke[i].v.v[1] = 0.43478259f;
                    } else {
                        gSmoke[i].v.v[1] += 0.43478259f;
                    }
                }
            } else {
                gSmoke_flags &= ~(1u << i);
            }
        }
    }
    AREndPipingSession();
    srand(seed);
    if (gNo_2d_effects) {
        RenderRecordedSmokeCircles();
        if (gBlend_actor->parent != NULL) {
            BrActorRemove(gBlend_actor);
        }
        BrActorAdd(gDont_render_actor, gBlend_actor);
    }
#else
    __asm {
            sub esp, 0x2c
            mov eax, dword ptr [gNo_2d_effects]
            push ebx
            push ebp
            xor ebp, ebp
            push esi
            cmp eax, ebp
            push edi
            mov dword ptr [esp + 0x10], ebp
            je L004fa95b
            mov eax, dword ptr [gBlend_actor]
            mov byte ptr [eax + 0x20], 4
            mov eax, dword ptr [gBlend_actor]
            cmp dword ptr [eax + 0xc], ebp
            je L004fa947
            push eax
            call BrActorRemove
            mov eax, dword ptr [gBlend_actor]
            add esp, 4
    L004fa947:
            mov ecx, dword ptr [esp + 0x40]
            push eax
            push ecx
            call BrActorAdd
            add esp, 8
            mov dword ptr [gN_BR_smoke_structs], ebp
    L004fa95b:
            cmp dword ptr [gColumn_flags], ebp
            je L004faac2
            call rand
            mov dword ptr [esp + 0x14], eax
            call GetTotalTime
            push eax
            call srand
            add esp, 4
            mov esi, offset gSmoke_column+0x14
            mov ebx, 0x3d8f5c29
    L004fa988:
            mov eax, dword ptr [gColumn_flags]
            mov edx, 1
            mov ecx, ebp
            shl edx, cl
            test eax, edx
            je L004faaa3
            cmp dword ptr [esi + 8], 1
            jg L004faaa3
            mov eax, dword ptr [esi]
            mov dword ptr [esp + 0x40], 0x3f000000
            cmp eax, 0xfa0
            jae L004fa9d9
            mov dword ptr [esp + 0x28], eax
            mov dword ptr [esp + 0x2c], 0
            fild qword ptr [esp + 0x28]
            fmul dword ptr [gFLOAT_0058afec]
            fmul dword ptr [gFLOAT_0058b060+0x14]
            fstp dword ptr [esp + 0x40]
    L004fa9d9:
            fld dword ptr [esi + 0x24]
            fsub qword ptr [gFLOAT_0058b060+0x18]
            mov eax, dword ptr [esi + 0x20]
            mov ecx, dword ptr [esi + 0x28]
            mov edi, dword ptr [gShade_list+0x1c]
            push 0x3f7d70a4
            push 0x3f000000
            mov dword ptr [esp + 0x38], eax
            fstp dword ptr [esp + 0x3c]
            mov dword ptr [esp + 0x40], ecx
            call SRandomBetween
            mov eax, dword ptr [gNo_2d_effects]
            fstp dword ptr [esp + 0x18]
            test eax, eax
            je L004faaa3
            mov eax, dword ptr [gRendering_mirror]
            test eax, eax
            mov eax, offset gRearview_camera_to_world
            jne L004faa2d
            mov eax, offset gCamera_to_world
    L004faa2d:
            push eax
            mov eax, dword ptr [gN_BR_smoke_structs]
            lea edx, [esp + 0x34]
            lea eax, [eax + eax*8]
            lea ecx, [eax*4 + gBR_smoke_structs]
            call DRMatrix34TApplyP
            mov ecx, dword ptr [gN_BR_smoke_structs]
            mov edx, dword ptr [esp + 0x40]
            lea eax, [ecx + ecx*8]
            shl eax, 2
            mov dword ptr [eax + gBR_smoke_structs+0xc], ebx
            mov dword ptr [eax + gBR_smoke_structs+0x10], edx
            mov edx, dword ptr [esp + 0x18]
            mov dword ptr [eax + gBR_smoke_structs+0x14], edi
            mov dword ptr [eax + gBR_smoke_structs+0x18], edx
            lea eax, [eax + gBR_smoke_structs]
            mov dword ptr [ecx*4 + gBR_smoke_pointers], eax
            mov ecx, dword ptr [gShade_list+0x1c]
            cmp edi, ecx
            jne L004faa94
            mov ecx, dword ptr [gBlend_model2]
            mov dword ptr [eax + 0x20], ecx
            jmp L004faa9d
    L004faa94:
            mov edx, dword ptr [gBlend_model]
            mov dword ptr [eax + 0x20], edx
    L004faa9d:
            inc dword ptr [gN_BR_smoke_structs]
    L004faaa3:
            add esi, 0x7c
            inc ebp
            cmp esi, offset gSparks+0x14
            jl L004fa988
            mov eax, dword ptr [esp + 0x14]
            push eax
            call srand
            add esp, 4
            xor ebp, ebp
    L004faac2:
            cmp dword ptr [gSmoke_flags], ebp
            jne L004fab0c
            cmp dword ptr [gNo_2d_effects], ebp
            je L004fb1a3
            mov eax, dword ptr [gBlend_actor]
            cmp dword ptr [eax + 0xc], ebp
            je L004fad12
            push eax
            call BrActorRemove
            mov eax, dword ptr [gBlend_actor]
            mov ecx, dword ptr [gDont_render_actor]
            add esp, 4
            push eax
            push ecx
            call BrActorAdd
            add esp, 8
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x2c
            ret 0xc
    L004fab0c:
            call rand
            mov dword ptr [esp + 0x18], eax
            mov eax, dword ptr [gPed_676914]
            cmp eax, ebp
            je L004fad2c
            mov edi, dword ptr [gN_BR_smoke_structs]
            xor ebx, ebx
            mov esi, offset gSmoke+0x28
    L004fab2f:
            mov eax, dword ptr [gSmoke_flags]
            mov edx, 1
            mov ecx, ebx
            shl edx, cl
            test eax, edx
            je L004facc9
            fld dword ptr [esi - 0xc]
            fsub dword ptr [gFLOAT_0058b060+0x20]
            mov al, byte ptr [esi]
            test al, 0x10
            fmul dword ptr [gFLOAT_0058b060+0x24]
            fmul qword ptr [gFLOAT_0058b060+0x28]
            fsubr qword ptr [gDOUBLE_0058b010]
            fstp dword ptr [esp + 0x48]
            je L004fac16
            fld dword ptr [esi - 0xc]
            fdiv dword ptr [esp + 0x48]
            and eax, 0xf
            mov ebp, dword ptr [eax*4 + gShade_list]
            mov eax, dword ptr [esi - 8]
            mov dword ptr [esp + 0x40], eax
            mov eax, dword ptr [gNo_2d_effects]
            test eax, eax
            fstp dword ptr [esp + 0x48]
            je L004facc9
            mov eax, dword ptr [gRendering_mirror]
            test eax, eax
            mov eax, offset gRearview_camera_to_world
            jne L004fabaa
            mov eax, offset gCamera_to_world
    L004fabaa:
            lea ecx, [edi + edi*8]
            push eax
            lea edx, [esi - 0x24]
            lea ecx, [ecx*4 + gBR_smoke_structs]
            call DRMatrix34TApplyP
            mov ecx, dword ptr [gN_BR_smoke_structs]
            mov edx, dword ptr [esp + 0x48]
            lea eax, [ecx + ecx*8]
            shl eax, 2
            mov dword ptr [eax + gBR_smoke_structs+0xc], edx
            mov edx, dword ptr [esp + 0x40]
            mov dword ptr [eax + gBR_smoke_structs+0x10], edx
            mov dword ptr [eax + gBR_smoke_structs+0x14], ebp
            mov dword ptr [eax + gBR_smoke_structs+0x18], 0x3f800000
            lea eax, [eax + gBR_smoke_structs]
            mov dword ptr [ecx*4 + gBR_smoke_pointers], eax
            mov ecx, dword ptr [gShade_list+0x1c]
            cmp ebp, ecx
            jne L004facb3
            mov ecx, dword ptr [gBlend_model2]
            mov dword ptr [eax + 0x20], ecx
            jmp L004facbc
    L004fac16:
            and eax, 0xf
            mov ecx, dword ptr [esi - 0xc]
            mov dword ptr [esp + 0x14], ecx
            mov ebp, dword ptr [eax*4 + gShade_list]
            mov eax, dword ptr [esi - 8]
            mov dword ptr [esp + 0x10], eax
            mov eax, dword ptr [gNo_2d_effects]
            test eax, eax
            je L004facc9
            mov eax, dword ptr [gRendering_mirror]
            test eax, eax
            mov eax, offset gRearview_camera_to_world
            jne L004fac4e
            mov eax, offset gCamera_to_world
    L004fac4e:
            push eax
            lea eax, [edi + edi*8]
            lea edx, [esi - 0x24]
            lea ecx, [eax*4 + gBR_smoke_structs]
            call DRMatrix34TApplyP
            mov ecx, dword ptr [gN_BR_smoke_structs]
            mov edx, dword ptr [esp + 0x14]
            lea eax, [ecx + ecx*8]
            shl eax, 2
            mov dword ptr [eax + gBR_smoke_structs+0xc], edx
            mov edx, dword ptr [esp + 0x10]
            mov dword ptr [eax + gBR_smoke_structs+0x10], edx
            mov edx, dword ptr [esp + 0x48]
            mov dword ptr [eax + gBR_smoke_structs+0x14], ebp
            mov dword ptr [eax + gBR_smoke_structs+0x18], edx
            lea eax, [eax + gBR_smoke_structs]
            mov dword ptr [ecx*4 + gBR_smoke_pointers], eax
            mov ecx, dword ptr [gShade_list+0x1c]
            cmp ebp, ecx
            jne L004facb3
            mov ecx, dword ptr [gBlend_model2]
            mov dword ptr [eax + 0x20], ecx
            jmp L004facbc
    L004facb3:
            mov edx, dword ptr [gBlend_model]
            mov dword ptr [eax + 0x20], edx
    L004facbc:
            mov edi, dword ptr [gN_BR_smoke_structs]
            inc edi
            mov dword ptr [gN_BR_smoke_structs], edi
    L004facc9:
            add esi, 0x2c
            inc ebx
            cmp esi, offset gSplash_material+0x24
            jl L004fab2f
            mov eax, dword ptr [esp + 0x18]
            push eax
            call srand
            mov eax, dword ptr [gNo_2d_effects]
            add esp, 4
            test eax, eax
            je L004fb1a3
            call RenderRecordedSmokeCircles
            mov eax, dword ptr [gBlend_actor]
            mov ecx, dword ptr [eax + 0xc]
            test ecx, ecx
            je L004fad12
            push eax
            call BrActorRemove
            mov eax, dword ptr [gBlend_actor]
            add esp, 4
    L004fad12:
            mov ecx, dword ptr [gDont_render_actor]
            push eax
            push ecx
            call BrActorAdd
            add esp, 8
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x2c
            ret 0xc
    L004fad2c:
            mov ecx, 0xf
            call ARStartPipingSession
            mov ebx, dword ptr [esp + 0x48]
            mov edi, dword ptr [gSmoke_flags]
            xor ecx, ecx
            mov edx, offset gSmoke
    L004fad47:
            mov esi, 1
            shl esi, cl
            test edi, esi
            je L004fade8
            fld dword ptr [edx + 0x20]
            fcomp dword ptr [gFLOAT_0058b008]
            fnstsw ax
            test ah, 0x41
            je L004fad6c
            not esi
            and edi, esi
            jmp L004fade8
    L004fad6c:
            mov eax, dword ptr [edx]
            cmp eax, ebp
            je L004fada5
            mov dword ptr [esp + 0x28], eax
            mov dword ptr [esp + 0x2c], ebp
            fild qword ptr [esp + 0x28]
            fld dword ptr [edx + 0x10]
            fld dword ptr [edx + 0x14]
            fxch st(2)
            fmul dword ptr [gFLOAT_0058afdc+0xc]
            fld dword ptr [edx + 0x18]
            fxch st(2)
            fmul st(0), st(1)
            fxch st(3)
            fmul st(0), st(1)
            fxch st(2)
            fmul st(0), st(1)
            fxch st(3)
            fstp st(1)
            fxch st(2)
            mov dword ptr [edx], ebp
            jmp L004fadd4
    L004fada5:
            mov dword ptr [esp + 0x20], ebx
            mov dword ptr [esp + 0x24], ebp
            fild qword ptr [esp + 0x20]
            fld dword ptr [edx + 0x10]
            fld dword ptr [edx + 0x14]
            fxch st(2)
            fmul dword ptr [gFLOAT_0058afdc+0xc]
            fld dword ptr [edx + 0x18]
            fxch st(2)
            fmul st(0), st(1)
            fxch st(3)
            fmul st(0), st(1)
            fxch st(2)
            fmul st(0), st(1)
            fxch st(3)
            fstp st(1)
            fxch st(2)
    L004fadd4:
            fxch st(2)
            fadd dword ptr [edx + 4]
            fstp dword ptr [edx + 4]
            fadd dword ptr [edx + 8]
            fstp dword ptr [edx + 8]
            fadd dword ptr [edx + 0xc]
            fstp dword ptr [edx + 0xc]
    L004fade8:
            add edx, 0x2c
            inc ecx
            cmp edx, offset gSmoke+0x44c
            jl L004fad47
            mov dword ptr [esp + 0x40], ebp
            mov ebp, dword ptr [gN_BR_smoke_structs]
            mov dword ptr [gSmoke_flags], edi
            mov esi, offset gSmoke+0x1c
            jmp L004fae15
    L004fae0f:
            mov edi, dword ptr [gSmoke_flags]
    L004fae15:
            mov ecx, dword ptr [esp + 0x40]
            mov ebx, 1
            shl ebx, cl
            test edi, ebx
            je L004fb141
            mov dl, byte ptr [esi + 0xc]
            mov eax, dword ptr [esp + 0x10]
            and dl, 0xf
            cmp dl, 7
            jne L004fae3b
            or eax, ebx
            jmp L004fae97
    L004fae3b:
            test eax, ebx
            jne L004fae9b
            mov eax, dword ptr [esp + 0x40]
            lea ecx, [eax + 1]
            cmp ecx, 0x19
            jge L004fae9b
            mov edi, dword ptr [gSmoke_flags]
            lea edx, [esi + 0x2c]
    L004fae54:
            mov eax, 1
            shl eax, cl
            test eax, edi
            je L004fae7a
            _emit 0xd9
        _emit 0x06
            _emit 0xd8
        _emit 0x02
            fld dword ptr [gFLOAT_0058b008]
            fld st(1)
            fmul st(0), st(2)
            fxch st(1)
            fcompp
            fnstsw ax
            test ah, 1
            fstp st(0)
            jne L004fae88
    L004fae7a:
            add edx, 0x2c
            inc ecx
            cmp edx, offset gSplash_material+0x18
            jl L004fae54
            jmp L004fae9b
    L004fae88:
            mov eax, dword ptr [esp + 0x10]
            mov edx, 1
            shl edx, cl
            or edx, ebx
            or eax, edx
    L004fae97:
            mov dword ptr [esp + 0x10], eax
    L004fae9b:
            test dword ptr [esp + 0x10], ebx
            jne L004faead
            fld dword ptr [esi + 4]
            fmul dword ptr [gFLOAT_0058afec]
            fstp dword ptr [esi + 4]
    L004faead:
            _emit 0xd9
        _emit 0x06
            fsub dword ptr [gFLOAT_0058b060+0x20]
            mov al, byte ptr [esi + 0xc]
            test al, 0x10
            fmul dword ptr [gFLOAT_0058b060+0x24]
            fmul qword ptr [gFLOAT_0058b060+0x28]
            fsubr qword ptr [gDOUBLE_0058b010]
            fstp dword ptr [esp + 0x14]
            je L004faf7e
            _emit 0xd9
        _emit 0x06
            fdiv dword ptr [esp + 0x14]
            and eax, 0xf
            mov edi, dword ptr [eax*4 + gShade_list]
            mov eax, dword ptr [esi + 4]
            mov dword ptr [esp + 0x1c], eax
            mov eax, dword ptr [gNo_2d_effects]
            test eax, eax
            fstp dword ptr [esp + 0x14]
            je L004fb031
            mov eax, dword ptr [gRendering_mirror]
            test eax, eax
            mov eax, offset gRearview_camera_to_world
            jne L004faf11
            mov eax, offset gCamera_to_world
    L004faf11:
            lea ecx, [ebp + ebp*8]
            push eax
            lea edx, [esi - 0x18]
            lea ecx, [ecx*4 + gBR_smoke_structs]
            call DRMatrix34TApplyP
            mov ecx, dword ptr [gN_BR_smoke_structs]
            mov edx, dword ptr [esp + 0x14]
            lea eax, [ecx + ecx*8]
            shl eax, 2
            mov dword ptr [eax + gBR_smoke_structs+0xc], edx
            mov edx, dword ptr [esp + 0x1c]
            mov dword ptr [eax + gBR_smoke_structs+0x10], edx
            mov dword ptr [eax + gBR_smoke_structs+0x14], edi
            mov dword ptr [eax + gBR_smoke_structs+0x18], 0x3f800000
            lea eax, [eax + gBR_smoke_structs]
            mov dword ptr [ecx*4 + gBR_smoke_pointers], eax
            mov ecx, dword ptr [gShade_list+0x1c]
            cmp edi, ecx
            jne L004fb01b
            mov ecx, dword ptr [gBlend_model2]
            mov dword ptr [eax + 0x20], ecx
            jmp L004fb024
    L004faf7e:
            and eax, 0xf
            mov ecx, dword ptr [esi]
            mov dword ptr [esp + 0x20], ecx
            mov edi, dword ptr [eax*4 + gShade_list]
            mov eax, dword ptr [esi + 4]
            mov dword ptr [esp + 0x28], eax
            mov eax, dword ptr [gNo_2d_effects]
            test eax, eax
            je L004fb031
            mov eax, dword ptr [gRendering_mirror]
            test eax, eax
            mov eax, offset gRearview_camera_to_world
            jne L004fafb5
            mov eax, offset gCamera_to_world
    L004fafb5:
            push eax
            lea eax, [ebp + ebp*8]
            lea edx, [esi - 0x18]
            lea ecx, [eax*4 + gBR_smoke_structs]
            call DRMatrix34TApplyP
            mov ecx, dword ptr [gN_BR_smoke_structs]
            mov edx, dword ptr [esp + 0x20]
            lea eax, [ecx + ecx*8]
            shl eax, 2
            mov dword ptr [eax + gBR_smoke_structs+0xc], edx
            mov edx, dword ptr [esp + 0x28]
            mov dword ptr [eax + gBR_smoke_structs+0x10], edx
            mov edx, dword ptr [esp + 0x14]
            mov dword ptr [eax + gBR_smoke_structs+0x14], edi
            mov dword ptr [eax + gBR_smoke_structs+0x18], edx
            lea eax, [eax + gBR_smoke_structs]
            mov dword ptr [ecx*4 + gBR_smoke_pointers], eax
            mov ecx, dword ptr [gShade_list+0x1c]
            cmp edi, ecx
            jne L004fb01b
            mov ecx, dword ptr [gBlend_model2]
            mov dword ptr [eax + 0x20], ecx
            jmp L004fb024
    L004fb01b:
            mov edx, dword ptr [gBlend_model]
            mov dword ptr [eax + 0x20], edx
    L004fb024:
            mov ebp, dword ptr [gN_BR_smoke_structs]
            inc ebp
            mov dword ptr [gN_BR_smoke_structs], ebp
    L004fb031:
            mov al, byte ptr [esi + 0xd]
            lea edi, [esi - 0x18]
            test al, al
            je L004fb055
            mov eax, dword ptr [esi + 4]
            mov ecx, dword ptr [esi]
            mov dl, byte ptr [esi + 0xc]
            push eax
            push ecx
            mov ecx, dword ptr [esp + 0x48]
            push edi
            call AddSmokeToPipingSession
            mov ebp, dword ptr [gN_BR_smoke_structs]
    L004fb055:
            mov edx, dword ptr [esp + 0x48]
            mov dword ptr [esp + 0x34], 0
            mov dword ptr [esp + 0x30], edx
            fild qword ptr [esp + 0x30]
            fld st(0)
            fmul dword ptr [gFLOAT_0058afdc+0xc]
            fmul dword ptr [esi + 4]
            fmul qword ptr [gFLOAT_0058b060+0x30]
            _emit 0xd8
        _emit 0x06
            _emit 0xd9
        _emit 0x1e
            fld dword ptr [edi + 0x20]
            fmul st(0), st(1)
            fmul dword ptr [gFLOAT_0058afdc+0xc]
            fsubr dword ptr [esi + 4]
            fstp dword ptr [esi + 4]
            _emit 0xd9
        _emit 0x06
            fcomp dword ptr [gFLOAT_0058b034]
            fnstsw ax
            test ah, 0x41
            jne L004fb0a4
            mov dword ptr [esi], 0x3e99999a
    L004fb0a4:
            fld dword ptr [esi + 4]
            fcomp dword ptr [gFLOAT_0058b008]
            fnstsw ax
            test ah, 0x41
            je L004fb0c6
            mov eax, dword ptr [gSmoke_flags]
            not ebx
            and eax, ebx
            fstp st(0)
            mov dword ptr [gSmoke_flags], eax
            jmp L004fb141
    L004fb0c6:
            fmul dword ptr [gFLOAT_0058b060+0x38]
            fsubr dword ptr [gFLOAT_0058afdc]
            fcom dword ptr [gFLOAT_0058afec]
            fnstsw ax
            test ah, 1
            je L004fb0e7
            fstp st(0)
            fld dword ptr [gFLOAT_0058afec]
    L004fb0e7:
            fld dword ptr [esi - 0xc]
            fmul st(0), st(1)
            fstp dword ptr [esi - 0xc]
            fld dword ptr [esi - 8]
            fmul st(0), st(1)
            fstp dword ptr [esi - 8]
            fld dword ptr [esi - 4]
            fmul st(0), st(1)
            fstp dword ptr [esi - 4]
            fstp st(0)
            fld dword ptr [esi - 8]
            fabs
            fcomp dword ptr [gFLOAT_0058b060+0x3c]
            fnstsw ax
            test ah, 1
            je L004fb141
            mov al, byte ptr [esi + 0xc]
            and al, 0xf
            cmp al, 7
            jae L004fb141
            fld dword ptr [esi - 8]
            fcomp dword ptr [gFLOAT_0058b008]
            fnstsw ax
            test ah, 1
            je L004fb13a
            fld dword ptr [esi - 8]
            fsub dword ptr [gFLOAT_0058b060+0x40]
            fstp dword ptr [esi - 8]
            jmp L004fb141
    L004fb13a:
            mov dword ptr [esi - 8], 0x3ede9bd3
    L004fb141:
            mov edx, dword ptr [esp + 0x40]
            add esi, 0x2c
            inc edx
            cmp esi, offset gSplash_material+0x18
            mov dword ptr [esp + 0x40], edx
            jl L004fae0f
            call AREndPipingSession
            mov ecx, dword ptr [esp + 0x18]
            push ecx
            call srand
            mov eax, dword ptr [gNo_2d_effects]
            add esp, 4
            test eax, eax
            je L004fb1a3
            call RenderRecordedSmokeCircles
            mov eax, dword ptr [gBlend_actor]
            mov ecx, dword ptr [eax + 0xc]
            test ecx, ecx
            je L004fb193
            push eax
            call BrActorRemove
            mov eax, dword ptr [gBlend_actor]
            add esp, 4
    L004fb193:
            mov edx, dword ptr [gDont_render_actor]
            push eax
            push edx
            call BrActorAdd
            add esp, 8
    L004fb1a3:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x2c
            ret 0xc
    }
#endif
}
// DrawDot

// GLOBAL: CARMA2_HW 0x0058b0b0
const double gDOUBLE_0058b0b0 = -0.14492753623188406;

// GLOBAL: CARMA2_HW 0x0058b120
const br_scalar gFLOAT_0058b120 = -5.0f;

// GLOBAL: CARMA2_HW 0x0058b124
const br_scalar gFLOAT_0058b124 = 0.006666666828095913f;

// GLOBAL: CARMA2_HW 0x0058b128
const br_scalar gFLOAT_0058b128 = -0.04782608523964882f;

// GLOBAL: CARMA2_HW 0x0058b12c
const br_scalar gFLOAT_0058b12c = 0.014285714365541935f;

// GLOBAL: CARMA2_HW 0x0058b130
const br_scalar gFLOAT_0058b130 = 15.0f;

// SetWorldToScreen

// DrawLine3DThroughBRender

// DrawLine3D

// DrawLine2D

// SetLineModelCols

// ReplaySparks

// RenderSparks

// FUNCTION: CARMA2_HW 0x004f8290
void C2_NAKED C2_HOOK_FASTCALL CreateSparkShower(br_vector3* pPosition, br_vector3* pVelocity, br_vector3* pForce, tNon_car_spec* pOwner1, tNon_car_spec* pOwner2) {
#ifndef CARPOCALYPSE2_MATCHING
    br_vector3 normal;
    br_vector3 tv;
    br_vector3 tv2;
    br_scalar force_length;
    br_scalar force_length_squared;
    br_scalar projection;
    br_matrix34* owner_matrix;
    tNon_car_spec* selected_owner;
    int spark_count;

    C2_HOOK_BUG_ON(offsetof(tNon_car_spec, driver) != 0x0C);
    C2_HOOK_BUG_ON(offsetof(tNon_car_spec, actor) != 0x10);
    C2_HOOK_BUG_ON(offsetof(tNon_car_spec, flags) != 0x100);

    force_length = BrVector3Length(pForce);
    if (pOwner1 == NULL || pOwner1->driver != eDriver_local_human) {
        selected_owner = pOwner2;
    } else {
        selected_owner = pOwner1;
    }
    normal.v[0] = pForce->v[0] / force_length;
    normal.v[1] = pForce->v[1] / force_length;
    normal.v[2] = pForce->v[2] / force_length;
    if (force_length >= gFLOAT_0058aff0) {
        CreateShrapnelShower(pPosition, pVelocity, &normal, force_length,
            (tCar_spec*)pOwner1, (tCar_spec*)pOwner2, 0, NULL);
        if (!((pOwner1 != NULL && pOwner1->driver > 5)
                || (pOwner1->flags & 0x200000) != 0
                || (pOwner2 != NULL && pOwner2->driver > 5)
                || (pOwner2->flags & 0x200000) != 0)) {
            return;
        }

        owner_matrix = &selected_owner->actor->t.t.mat;
        force_length_squared = force_length * force_length;
        projection = BrVector3Dot(pForce, pVelocity) / force_length_squared;
        tv.v[0] = pVelocity->v[0] - pForce->v[0] * projection;
        tv.v[1] = pVelocity->v[1] - pForce->v[1] * projection;
        tv.v[2] = pVelocity->v[2] - pForce->v[2] * projection;
        BrMatrix34TApplyV(pVelocity, &tv, &pOwner1->actor->t.t.mat);

        BrVector3Sub(&tv, pPosition, (br_vector3*)owner_matrix->m[3]);
        BrMatrix34TApplyV(pPosition, &tv, owner_matrix);
        BrMatrix34TApplyV(&tv, pForce, owner_matrix);
        BrVector3Copy(pForce, &tv);

        spark_count = 3 - (int)(force_length * gFLOAT_0058b02c);
        if (spark_count > 20) {
            spark_count = 20;
        }
        while (spark_count > 0) {
            int spark_index;

            spark_index = gSpark_next;
            BrVector3Copy(&gSparks[spark_index].pos, pPosition);
            BrVector3SetFloat(&gSparks[spark_index].normal, 0.0f, 0.0f, 0.0f);
            BrVector3SetFloat(&tv,
                FRandomBetween(-1.0f, 1.0f),
                FRandomBetween(-0.2f, 1.0f),
                FRandomBetween(-1.0f, 1.0f));
            projection = BrVector3Dot(pForce, &tv) / force_length_squared;
            BrVector3Scale(&tv2, pForce, projection);
            BrVector3Sub(&gSparks[spark_index].v, &tv, &tv2);
            BrVector3Accumulate(&gSparks[spark_index].v, pVelocity);
            gSparks[spark_index].count = 1000;
            gSparks[spark_index].car = (tCar_spec*)selected_owner;
            gSpark_flags |= 1 << spark_index;
            gSparks[spark_index].time_sync = gPHIL_mechanics_time_sync;
            gSparks[spark_index].colour = 0;
            gSpark_next = spark_index + 1;
            if (gSpark_next >= CARPOCALYPSE2_ASIZE(gSparks)) {
                gSpark_next = 0;
            }
            spark_count--;
        }
    }
#else
    __asm {
            sub esp, 0x28
            push ebx
            push ebp
            push esi
            mov esi, dword ptr [esp + 0x38]
            push edi
            mov edi, edx
            fld dword ptr [esi + 4]
            fld dword ptr [esi + 8]
            fld dword ptr [esi]
            fxch st(2)
            fst dword ptr [esp + 0x3c]
            fxch st(1)
            fst dword ptr [esp + 0x10]
            fxch st(1)
            fmul dword ptr [esp + 0x3c]
            fxch st(1)
            fmul dword ptr [esp + 0x10]
            fld st(2)
            fmul st(0), st(3)
            fxch st(1)
            faddp st(2), st(0)
            mov edx, dword ptr [esp + 0x40]
            mov ebp, ecx
            test edx, edx
            faddp st(1), st(0)
            fsqrt
            fstp st(1)
            fst dword ptr [esp + 0x3c]
            je L004f82e3
            mov eax, dword ptr [edx + 0xc]
            mov ebx, edx
            cmp eax, 8
            je L004f82e7
    L004f82e3:
            mov ebx, dword ptr [esp + 0x44]
    L004f82e7:
            fld dword ptr [esi + 4]
            fld dword ptr [esi + 8]
            fld dword ptr [esi]
            fdiv st(0), st(3)
            fxch st(2)
            mov dword ptr [esp + 0x10], ebx
            fdiv st(0), st(3)
            fxch st(1)
            fdiv st(0), st(3)
            fxch st(3)
            fcomp dword ptr [gFLOAT_0058aff0]
            fxch st(1)
            fstp dword ptr [esp + 0x20]
            fstp dword ptr [esp + 0x24]
            fnstsw ax
            fstp dword ptr [esp + 0x28]
            test ah, 1
            jne L004f861e
            mov eax, dword ptr [esp + 0x44]
            mov ecx, dword ptr [esp + 0x3c]
            push 0
            push 0
            push eax
            push edx
            lea edx, [esp + 0x30]
            push ecx
            push edx
            mov edx, edi
            mov ecx, ebp
            call CreateShrapnelShower
            mov eax, dword ptr [esp + 0x40]
            test eax, eax
            je L004f8349
            cmp dword ptr [eax + 0xc], 5
            jg L004f8372
    L004f8349:
            mov ecx, dword ptr [eax + 0x100]
            mov edx, 0x200000
            test ecx, edx
            jne L004f8372
            mov ecx, dword ptr [esp + 0x44]
            test ecx, ecx
            je L004f8366
            cmp dword ptr [ecx + 0xc], 5
            jg L004f8372
    L004f8366:
            test dword ptr [ecx + 0x100], edx
            je L004f861e
    L004f8372:
            fld dword ptr [esi + 8]
            fld dword ptr [edi + 8]
            fld dword ptr [esi]
            fmul dword ptr [edi]
            fld dword ptr [edi + 4]
            fld dword ptr [esp + 0x3c]
            fmul dword ptr [esp + 0x3c]
            mov eax, dword ptr [eax + 0x10]
            lea ecx, [esp + 0x14]
            add eax, 0x2c
            push eax
            push ecx
            fstp dword ptr [esp + 0x4c]
            fxch st(3)
            fmulp st(2), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fxch st(1)
            fmul dword ptr [esi + 4]
            fld dword ptr [esi]
            fld dword ptr [esi + 4]
            fxch st(2)
            faddp st(3), st(0)
            fld dword ptr [esi + 8]
            fxch st(3)
            fdiv dword ptr [esp + 0x4c]
            fxch st(1)
            push edi
            fmul st(0), st(1)
            fld dword ptr [edi]
            fxch st(3)
            fmul st(0), st(2)
            fxch st(4)
            fmul st(0), st(2)
            fxch st(1)
            fstp dword ptr [esp + 0x20]
            fxch st(2)
            fsub dword ptr [esp + 0x20]
            fxch st(3)
            fstp st(1)
            fxch st(2)
            fstp dword ptr [esp + 0x20]
            fld dword ptr [edi + 8]
            fld dword ptr [edi + 4]
            fsub st(0), st(3)
            fxch st(1)
            fsub st(0), st(2)
            fxch st(1)
            fstp dword ptr [esp + 0x24]
            fstp dword ptr [esp + 0x28]
            fstp st(0)
            fstp st(0)
            call BrMatrix34TApplyV
            mov eax, dword ptr [ebx + 0x10]
            add esp, 0xc
            fld dword ptr [ebp]
            fsub dword ptr [eax + 0x50]
            fld dword ptr [ebp + 4]
            fld dword ptr [ebp + 8]
            fxch st(2)
            fstp dword ptr [esp + 0x14]
            lea edx, [eax + 0x2c]
            lea ecx, [esp + 0x14]
            fsub dword ptr [eax + 0x54]
            push edx
            push ecx
            push ebp
            fstp dword ptr [esp + 0x24]
            fsub dword ptr [eax + 0x58]
            fstp dword ptr [esp + 0x28]
            call BrMatrix34TApplyV
            mov edx, dword ptr [ebx + 0x10]
            add esp, 0xc
            add edx, 0x2c
            lea eax, [esp + 0x14]
            push edx
            push esi
            push eax
            call BrMatrix34TApplyV
            fld dword ptr [esp + 0x48]
            fld dword ptr [esp + 0x20]
            fld dword ptr [esp + 0x24]
            fld dword ptr [esp + 0x28]
            fxch st(3)
            fmul dword ptr [gFLOAT_0058b02c]
            fxch st(2)
            fstp dword ptr [esi]
            add esp, 0xc
            fstp dword ptr [esi + 4]
            fxch st(1)
            fstp dword ptr [esi + 8]
            call _ftol
            mov ecx, 3
            sub ecx, eax
            cmp ecx, 0x14
            jle L004f8481
            mov ecx, 0x14
    L004f8481:
            test ecx, ecx
            jle L004f861e
            mov dword ptr [esp + 0x40], ecx
            mov ecx, dword ptr [gSpark_next]
            jmp L004f8499
    L004f8495:
            mov ebx, dword ptr [esp + 0x10]
    L004f8499:
            mov eax, ecx
            mov ecx, dword ptr [ebp]
            shl eax, 6
            push 0
            push 0
            mov dword ptr [eax + gSparks+0x4], ecx
            mov edx, dword ptr [ebp + 4]
            mov dword ptr [eax + gSparks+0x8], edx
            mov ecx, dword ptr [ebp + 8]
            lea edx, [eax + gSparks+0x28]
            push 0
            push edx
            mov dword ptr [eax + gSparks+0xc], ecx
            call BrVector3SetFloat
            add esp, 0x10
            push 0x3f800000
            push 0xbf800000
            call FRandomBetween
            push ecx
            fstp dword ptr [esp]
            push 0x3f800000
            push 0xbe4ccccd
            call FRandomBetween
            push ecx
            fstp dword ptr [esp]
            push 0x3f800000
            push 0xbf800000
            call FRandomBetween
            lea eax, [esp + 0x1c]
            push ecx
            fstp dword ptr [esp]
            push eax
            call BrVector3SetFloat
            fld dword ptr [esi + 8]
            fld dword ptr [esi + 4]
            fld dword ptr [esi]
            fxch st(2)
            fmul dword ptr [esp + 0x2c]
            fxch st(1)
            fmul dword ptr [esp + 0x28]
            fld dword ptr [esi + 4]
            fxch st(3)
            fmul dword ptr [esp + 0x24]
            fxch st(1)
            faddp st(2), st(0)
            fld dword ptr [esi + 8]
            fld dword ptr [esi]
            fxch st(2)
            faddp st(3), st(0)
            fxch st(2)
            fdiv dword ptr [esp + 0x54]
            fxch st(3)
            mov ecx, dword ptr [gSpark_next]
            add esp, 0x10
            mov eax, ecx
            shl eax, 6
            fmul st(0), st(3)
            fld dword ptr [esp + 0x18]
            fxch st(3)
            fmul st(0), st(4)
            fld dword ptr [esp + 0x1c]
            fxch st(3)
            fmul st(0), st(5)
            fxch st(2)
            fstp dword ptr [esp + 0x30]
            fxch st(3)
            fsub dword ptr [esp + 0x30]
            fxch st(3)
            fstp dword ptr [esp + 0x34]
            fxch st(1)
            fsub dword ptr [esp + 0x34]
            fxch st(1)
            fsubr dword ptr [esp + 0x14]
            fstp dword ptr [eax + gSparks+0x10]
            fxch st(1)
            fstp dword ptr [eax + gSparks+0x14]
            mov edx, 1
            fstp dword ptr [eax + gSparks+0x18]
            shl edx, cl
            fstp st(0)
            fld dword ptr [edi]
            fadd dword ptr [eax + gSparks+0x10]
            fstp dword ptr [eax + gSparks+0x10]
            fld dword ptr [eax + gSparks+0x14]
            fadd dword ptr [edi + 4]
            fstp dword ptr [eax + gSparks+0x14]
            fld dword ptr [eax + gSparks+0x18]
            fadd dword ptr [edi + 8]
            fstp dword ptr [eax + gSparks+0x18]
            mov dword ptr [eax + gSparks], 0x3e8
            mov dword ptr [eax + gSparks+0x38], ebx
            mov ebx, dword ptr [gSpark_flags]
            or ebx, edx
            mov edx, dword ptr [gPHIL_mechanics_time_sync]
            inc ecx
            mov dword ptr [eax + gSparks+0x34], edx
            cmp ecx, 0x20
            mov dword ptr [gSpark_flags], ebx
            mov byte ptr [eax + gSparks+0x3c], 0
            mov dword ptr [gSpark_next], ecx
            jl L004f860f
            xor ecx, ecx
            mov dword ptr [gSpark_next], ecx
    L004f860f:
            mov eax, dword ptr [esp + 0x40]
            dec eax
            mov dword ptr [esp + 0x40], eax
            jne L004f8495
    L004f861e:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x28
            ret 0xc
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

// FUNCTION: CARMA2_HW 0x004f8630
void C2_HOOK_FASTCALL CreateSingleSpark(int pIndex_and_colour, br_vector3* pPos, br_vector3* pLength) {
    int index;

    index = pIndex_and_colour & 0xff;
    gSpark_flags |= 1 << pIndex_and_colour;
    if (gSparks[index].car != NULL) {
        br_vector3 position;
        br_actor* car_actor;

        car_actor = gSparks[index].car->car_master_actor;
        position.v[0] = pPos->v[0] - car_actor->t.t.translate.t.v[0];
        position.v[1] = pPos->v[1] - car_actor->t.t.translate.t.v[1];
        position.v[2] = pPos->v[2] - car_actor->t.t.translate.t.v[2];
        BrMatrix34TApplyV(&gSparks[index].pos, &position, &car_actor->t.t.mat);
    } else {
        gSparks[index].pos.v[0] = pPos->v[0];
        gSparks[index].pos.v[1] = pPos->v[1];
        gSparks[index].pos.v[2] = pPos->v[2];
    }
    gSparks[index].length.v[0] = pLength->v[0];
    gSparks[index].length.v[1] = pLength->v[1];
    gSparks[index].length.v[2] = pLength->v[2];
    gSparks[index].colour = (char)(pIndex_and_colour >> 8);
}

// CreateSparks

// CreateSparkShower

// AdjustSpark

// AdjustShrapnel

// ResetSparks

// ResetShrapnel

// CreateShrapnelShower2

// CreateShrapnelShowerWorld

// CreateShrapnelShower

// random

// InitShrapnel

// KillShrapnel

// DisposeShrapnel

// ReplayShrapnel

// ActorFunks

intptr_t C2_HOOK_CDECL ActorFunks(br_actor* pActor, void* pCallback);

// FUNCTION: CARMA2_HW 0x004f8ca0
void C2_NAKED C2_HOOK_FASTCALL MasterDisableCarFunks(tCar_spec* pCar) {
#ifndef CARPOCALYPSE2_MATCHING
#ifndef CARPOCALYPSE2_MATCHING
    /* stub: no-op for Linux boot */
#else
    int i;
    int j;
    tCar_crush_spec* spec = pCar->car_crush_spec;
    br_actor* actor;
    tUser_crush_data* user;
    tCar_crush_buffer_entry* crush;

    DRActorEnumRecurse(pCar->car_model_actor, ActorFunks, (void*)MasterDisableFunkotronic);
    for (i = 0; i < spec->field_0x270; i++) {
        actor = spec->field_0x274[i].field_0x0;
        user = (tUser_crush_data*)actor->user;
        if (user == NULL) {
            continue;
        }
        crush = user->crush_data;
        if (crush == NULL) {
            continue;
        }
        if (crush->smashables == NULL) {
            continue;
        }
        for (j = 0; j < crush->count_smashables; j++) {
            int funk_index = crush->smashables[j].funk;
            if (funk_index >= 0) {
                MasterDisableFunkotronic(funk_index);
            }
        }
    }
    for (i = 0; i < spec->field_0x2b0; i++) {
        actor = spec->field_0x2b4[i].field_0x0;
        user = (tUser_crush_data*)actor->user;
        if (user == NULL) {
            continue;
        }
        crush = user->crush_data;
        if (crush == NULL) {
            continue;
        }
        if (crush->smashables == NULL) {
            continue;
        }
        for (j = 0; j < crush->count_smashables; j++) {
            int funk_index = crush->smashables[j].funk;
            if (funk_index >= 0) {
                MasterDisableFunkotronic(funk_index);
            }
        }
    }
#endif
#else
    __asm {
            sub esp, 8
            push ebx
            push ebp
            push esi
            push edi
            mov edi, dword ptr [ecx + 0x18d4]
            mov ecx, dword ptr [ecx + 0xe0c]
            push offset MasterDisableFunkotronic
            mov edx, offset ActorFunks
            mov dword ptr [esp + 0x18], edi
            call DRActorEnumRecurse
            mov eax, dword ptr [edi + 0x270]
            test eax, eax
            mov dword ptr [esp + 0x10], 0
            jle L004f8d38
            lea ebp, [edi + 0x274]
    L004f8cde:
            mov eax, dword ptr [ebp]
            mov eax, dword ptr [eax + 0x60]
            test eax, eax
            je L004f8d22
            mov esi, dword ptr [eax + 8]
            test esi, esi
            je L004f8d22
            mov eax, dword ptr [esi + 0x3c]
            test eax, eax
            je L004f8d22
            xor ebx, ebx
            cmp word ptr [esi + 0x38], bx
            jbe L004f8d22
            xor edi, edi
    L004f8d00:
            mov eax, dword ptr [esi + 0x3c]
            mov ecx, dword ptr [eax + edi + 0x40]
            test ecx, ecx
            jl L004f8d10
            call MasterDisableFunkotronic
    L004f8d10:
            xor ecx, ecx
            inc ebx
            mov cx, word ptr [esi + 0x38]
            add edi, 0x7c
            cmp ebx, ecx
            jl L004f8d00
            mov edi, dword ptr [esp + 0x14]
    L004f8d22:
            mov eax, dword ptr [esp + 0x10]
            mov ecx, dword ptr [edi + 0x270]
            inc eax
            add ebp, 0x14
            cmp eax, ecx
            mov dword ptr [esp + 0x10], eax
            jl L004f8cde
    L004f8d38:
            mov eax, dword ptr [edi + 0x2b0]
            mov dword ptr [esp + 0x10], 0
            test eax, eax
            jle L004f8daa
            lea ebp, [edi + 0x2b4]
    L004f8d50:
            mov eax, dword ptr [ebp]
            mov eax, dword ptr [eax + 0x60]
            test eax, eax
            je L004f8d94
            mov esi, dword ptr [eax + 8]
            test esi, esi
            je L004f8d94
            mov eax, dword ptr [esi + 0x3c]
            test eax, eax
            je L004f8d94
            xor ebx, ebx
            cmp word ptr [esi + 0x38], bx
            jbe L004f8d94
            xor edi, edi
    L004f8d72:
            mov edx, dword ptr [esi + 0x3c]
            mov ecx, dword ptr [edi + edx + 0x40]
            test ecx, ecx
            jl L004f8d82
            call MasterDisableFunkotronic
    L004f8d82:
            xor eax, eax
            inc ebx
            mov ax, word ptr [esi + 0x38]
            add edi, 0x7c
            cmp ebx, eax
            jl L004f8d72
            mov edi, dword ptr [esp + 0x14]
    L004f8d94:
            mov eax, dword ptr [esp + 0x10]
            mov ecx, dword ptr [edi + 0x2b0]
            inc eax
            add ebp, 0x14
            cmp eax, ecx
            mov dword ptr [esp + 0x10], eax
            jl L004f8d50
    L004f8daa:
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
            nop
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004f8e10
void C2_NAKED C2_HOOK_FASTCALL MasterEnableCarFunks(tCar_spec* pCar) {
#ifndef CARPOCALYPSE2_MATCHING
#ifndef CARPOCALYPSE2_MATCHING
    /* stub: no-op for Linux boot */
#else
    int i;
    int j;
    tCar_crush_spec* spec = pCar->car_crush_spec;
    br_actor* actor;
    tUser_crush_data* user;
    tCar_crush_buffer_entry* crush;

    DRActorEnumRecurse(pCar->car_model_actor, ActorFunks, (void*)MasterEnableFunkotronic);
    for (i = 0; i < spec->field_0x270; i++) {
        actor = spec->field_0x274[i].field_0x0;
        user = (tUser_crush_data*)actor->user;
        if (user == NULL) {
            continue;
        }
        crush = user->crush_data;
        if (crush == NULL) {
            continue;
        }
        if (crush->smashables == NULL) {
            continue;
        }
        for (j = 0; j < crush->count_smashables; j++) {
            int funk_index = crush->smashables[j].funk;
            if (funk_index >= 0) {
                MasterEnableFunkotronic(funk_index);
            }
        }
    }

    for (i = 0; i < spec->field_0x2b0; i++) {
        actor = spec->field_0x2b4[i].field_0x0;
        user = (tUser_crush_data*)actor->user;
        if (user == NULL) {
            continue;
        }
        crush = user->crush_data;
        if (crush == NULL) {
            continue;
        }
        if (crush->smashables == NULL) {
            continue;
        }
        for (j = 0; j < crush->count_smashables; j++) {
            int funk_index = crush->smashables[j].funk;
            if (funk_index >= 0) {
                MasterEnableFunkotronic(funk_index);
            }
        }
    }
#endif
#else
    __asm {
            sub esp, 8
            push ebx
            push ebp
            push esi
            push edi
            mov edi, dword ptr [ecx + 0x18d4]
            mov ecx, dword ptr [ecx + 0xe0c]
            push offset MasterEnableFunkotronic
            mov edx, offset ActorFunks
            mov dword ptr [esp + 0x18], edi
            call DRActorEnumRecurse
            mov eax, dword ptr [edi + 0x270]
            test eax, eax
            mov dword ptr [esp + 0x10], 0
            jle L004f8ea8
            lea ebp, [edi + 0x274]
    L004f8e4e:
            mov eax, dword ptr [ebp]
            mov eax, dword ptr [eax + 0x60]
            test eax, eax
            je L004f8e92
            mov esi, dword ptr [eax + 8]
            test esi, esi
            je L004f8e92
            mov eax, dword ptr [esi + 0x3c]
            test eax, eax
            je L004f8e92
            xor ebx, ebx
            cmp word ptr [esi + 0x38], bx
            jbe L004f8e92
            xor edi, edi
    L004f8e70:
            mov eax, dword ptr [esi + 0x3c]
            mov ecx, dword ptr [eax + edi + 0x40]
            test ecx, ecx
            jl L004f8e80
            call MasterEnableFunkotronic
    L004f8e80:
            xor ecx, ecx
            inc ebx
            mov cx, word ptr [esi + 0x38]
            add edi, 0x7c
            cmp ebx, ecx
            jl L004f8e70
            mov edi, dword ptr [esp + 0x14]
    L004f8e92:
            mov eax, dword ptr [esp + 0x10]
            mov ecx, dword ptr [edi + 0x270]
            inc eax
            add ebp, 0x14
            cmp eax, ecx
            mov dword ptr [esp + 0x10], eax
            jl L004f8e4e
    L004f8ea8:
            mov eax, dword ptr [edi + 0x2b0]
            mov dword ptr [esp + 0x10], 0
            test eax, eax
            jle L004f8f1a
            lea ebp, [edi + 0x2b4]
    L004f8ec0:
            mov eax, dword ptr [ebp]
            mov eax, dword ptr [eax + 0x60]
            test eax, eax
            je L004f8f04
            mov esi, dword ptr [eax + 8]
            test esi, esi
            je L004f8f04
            mov eax, dword ptr [esi + 0x3c]
            test eax, eax
            je L004f8f04
            xor ebx, ebx
            cmp word ptr [esi + 0x38], bx
            jbe L004f8f04
            xor edi, edi
    L004f8ee2:
            mov edx, dword ptr [esi + 0x3c]
            mov ecx, dword ptr [edi + edx + 0x40]
            test ecx, ecx
            jl L004f8ef2
            call MasterEnableFunkotronic
    L004f8ef2:
            xor eax, eax
            inc ebx
            mov ax, word ptr [esi + 0x38]
            add edi, 0x7c
            cmp ebx, eax
            jl L004f8ee2
            mov edi, dword ptr [esp + 0x14]
    L004f8f04:
            mov eax, dword ptr [esp + 0x10]
            mov ecx, dword ptr [edi + 0x2b0]
            inc eax
            add ebp, 0x14
            cmp eax, ecx
            mov dword ptr [esp + 0x10], eax
            jl L004f8ec0
    L004f8f1a:
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
            nop
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004f8ff0
int C2_HOOK_FASTCALL SetPixelmap(br_material* pMaterial) {
    if (pMaterial->colour_map != gRecalc_mass_car->field_0x1960) {
        pMaterial->colour_map = gRecalc_mass_car->field_0x1960;
        BrMaterialUpdate(pMaterial, BR_MATU_ALL);
        return 1;
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x004f8f30
void C2_HOOK_FASTCALL DoCamouflageThing(tCar_spec* pCar) {
#ifdef CARPOCALYPSE2_MATCHING
    br_vector3 direction;
    br_vector3 position;
    br_vector3 collision_position;
    br_vector3 normal;
    br_scalar t;
    tFace_ref* face;

    BrVector3Set(&direction, 0.f, -0.5f, 0.f);
    t = 10.f;
    BrMatrix34ApplyP(&position, &pCar->collision_info->cmpos, &pCar->collision_info->transform_matrix);
    face = FUN_004226d0(&position, &direction, &normal, &t, &collision_position,
        &gFace_list__car[pCar->collision_info->box_face_start],
        pCar->collision_info->box_face_end - pCar->collision_info->box_face_start);
    if (face != NULL && face->material != NULL && face->material->colour_map != NULL
        && face->material->colour_map != pCar->field_0x1960) {
        pCar->field_0x1960 = face->material->colour_map;
        gRecalc_mass_car = pCar;
        ForEveryCarMaterial(pCar, SetPixelmap, 1);
    }
#else
    BlendifyCar(pCar);
#endif
}

// DoSolidGraniteThing

// DoTrueColModelThing

// SetFoxModelCallback

// FUNCTION: CARMA2_HW 0x004f9390
int C2_HOOK_FASTCALL ResetFoxModelCallback(br_model* pModel) {
    pModel->flags &= ~BR_MODF_CUSTOM;
    pModel->custom = NULL;
    return 1;
}

// FUNCTION: CARMA2_HW 0x004f93b0
int C2_HOOK_FASTCALL ResetFoxLighting(br_material* pMaterial) {

    if (pMaterial->flags & BR_MATF_PRELIT) {
        pMaterial->flags &= ~BR_MATF_PRELIT;
        BrMaterialUpdate(pMaterial, BR_MATU_LIGHTING | BR_MATU_RENDERING);
        return 1;
    } else {
        return 0;
    }
}

// CancelAlternateFoxEffect

// SetFoxLighting

// SetupAlternateFoxEffect

// RestorePixelmap

// FUNCTION: CARMA2_HW 0x004f9620
void C2_NAKED C2_HOOK_FASTCALL RestoreCarPixelmaps(tCar_spec* pCar_spec) {
#ifndef CARPOCALYPSE2_MATCHING
#ifndef CARPOCALYPSE2_MATCHING
    UnBlendifyCar(pCar_spec);
#else
    tCarFunks_master* funks;
    int i;
    int j;
    int funk_index;

    funks = (tCarFunks_master*)*(void**)((char*)pCar_spec + 0x18d4);

    DRActorEnumRecurse(pCar_spec->car_model_actor, ActorFunks, (void*)MasterEnableFunkotronic);

    for (i = 0; i < funks->count_0x270; i++) {
        br_actor* bit_actor = funks->bit_0x274[i].field_0x0;

        if (bit_actor->user != NULL) {
            tFunk_list* funk_list = ((tFunk_container*)bit_actor->user)->funk_list;

            if (funk_list != NULL) {
                if (funk_list->funk_records != NULL) {
                    for (j = 0; j < funk_list->funk_count; j++) {
                        funk_index = j[funk_list->funk_records].funk_index;
                        if (funk_index >= 0) {
                            MasterEnableFunkotronic(funk_index);
                        }
                    }
                }
            }
        }
    }

    for (i = 0; i < funks->count_0x2b0; i++) {
        br_actor* bit_actor = funks->bit_0x2b4[i].field_0x0;

        if (bit_actor->user != NULL) {
            tFunk_list* funk_list = ((tFunk_container*)bit_actor->user)->funk_list;

            if (funk_list != NULL) {
                if (funk_list->funk_records != NULL) {
                    for (j = 0; j < funk_list->funk_count; j++) {
                        funk_index = funk_list->funk_records[j].funk_index;
                        if (funk_index >= 0) {
                            MasterEnableFunkotronic(funk_index);
                        }
                    }
                }
            }
        }
    }

    ForEveryCarMaterial(pCar_spec, RestorePixelmap, 1);
    pCar_spec->field_0x1960 = NULL;
#endif
#else
    __asm {
            sub esp, 0xc
            push ebx
            push ebp
            push esi
            mov esi, ecx
            push edi
            push offset MasterEnableFunkotronic
            mov edi, dword ptr [esi + 0x18d4]
            mov ecx, dword ptr [esi + 0xe0c]
            mov edx, offset ActorFunks
            mov dword ptr [esp + 0x1c], esi
            mov dword ptr [esp + 0x18], edi
            call DRActorEnumRecurse
            mov eax, dword ptr [edi + 0x270]
            test eax, eax
            mov dword ptr [esp + 0x10], 0
            jle L004f96c2
            lea ebp, [edi + 0x274]
    L004f9664:
            mov eax, dword ptr [ebp]
            mov eax, dword ptr [eax + 0x60]
            test eax, eax
            je L004f96a8
            mov esi, dword ptr [eax + 8]
            test esi, esi
            je L004f96a8
            mov eax, dword ptr [esi + 0x3c]
            test eax, eax
            je L004f96a8
            xor ebx, ebx
            cmp word ptr [esi + 0x38], bx
            jbe L004f96a8
            xor edi, edi
    L004f9686:
            mov eax, dword ptr [esi + 0x3c]
            mov ecx, dword ptr [eax + edi + 0x40]
            test ecx, ecx
            jl L004f9696
            call MasterEnableFunkotronic
    L004f9696:
            xor ecx, ecx
            inc ebx
            mov cx, word ptr [esi + 0x38]
            add edi, 0x7c
            cmp ebx, ecx
            jl L004f9686
            mov edi, dword ptr [esp + 0x14]
    L004f96a8:
            mov eax, dword ptr [esp + 0x10]
            mov ecx, dword ptr [edi + 0x270]
            inc eax
            add ebp, 0x14
            cmp eax, ecx
            mov dword ptr [esp + 0x10], eax
            jl L004f9664
            mov esi, dword ptr [esp + 0x18]
    L004f96c2:
            mov eax, dword ptr [edi + 0x2b0]
            mov dword ptr [esp + 0x10], 0
            test eax, eax
            jle L004f9738
            lea ebp, [edi + 0x2b4]
    L004f96da:
            mov eax, dword ptr [ebp]
            mov eax, dword ptr [eax + 0x60]
            test eax, eax
            je L004f971e
            mov esi, dword ptr [eax + 8]
            test esi, esi
            je L004f971e
            mov eax, dword ptr [esi + 0x3c]
            test eax, eax
            je L004f971e
            xor ebx, ebx
            cmp word ptr [esi + 0x38], bx
            jbe L004f971e
            xor edi, edi
    L004f96fc:
            mov edx, dword ptr [esi + 0x3c]
            mov ecx, dword ptr [edi + edx + 0x40]
            test ecx, ecx
            jl L004f970c
            call MasterEnableFunkotronic
    L004f970c:
            xor eax, eax
            inc ebx
            mov ax, word ptr [esi + 0x38]
            add edi, 0x7c
            cmp ebx, eax
            jl L004f96fc
            mov edi, dword ptr [esp + 0x14]
    L004f971e:
            mov eax, dword ptr [esp + 0x10]
            mov ecx, dword ptr [edi + 0x2b0]
            inc eax
            add ebp, 0x14
            cmp eax, ecx
            mov dword ptr [esp + 0x10], eax
            jl L004f96da
            mov esi, dword ptr [esp + 0x18]
    L004f9738:
            push 1
            mov edx, offset RestorePixelmap
            mov ecx, esi
            call ForEveryCarMaterial
            mov dword ptr [esi + 0x1960], 0
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
    }
#endif
}

// MungeShrapnel

// DrMatrix34Rotate

// CmpSmokeZ

// FUNCTION: CARMA2_HW 0x004f9fc0
void C2_HOOK_FASTCALL InitSmokeStuff(void) {
#ifndef CARPOCALYPSE2_MATCHING
    br_pixelmap* pm;
    tPath_name path;
    int i;

    gBlend_model = BrModelAllocate("BlendModel", 4, 2);
    gBlend_model->flags |= BR_MODF_UPDATEABLE;
    BrVector3SetFloat(&gBlend_model->vertices[0].p, -0.5f, 0.f, 0.f);
    BrVector3SetFloat(&gBlend_model->vertices[1].p, 0.5f, 0.f, 0.f);
    BrVector3SetFloat(&gBlend_model->vertices[2].p, 0.5f, 1.f, 0.f);
    BrVector3SetFloat(&gBlend_model->vertices[3].p, -0.5f, 1.f, 0.f);
    BrVector2Set(&gBlend_model->vertices[0].map, 0.f, 1.f);
    BrVector2Set(&gBlend_model->vertices[1].map, 1.f, 1.f);
    BrVector2Set(&gBlend_model->vertices[2].map, 1.f, 0.f);
    BrVector2Set(&gBlend_model->vertices[3].map, 0.f, 0.f);
    gBlend_model->faces[0].vertices[0] = 0;
    gBlend_model->faces[0].vertices[1] = 1;
    gBlend_model->faces[0].vertices[2] = 2;
    gBlend_model->faces[1].vertices[0] = 0;
    gBlend_model->faces[1].vertices[1] = 2;
    gBlend_model->faces[1].vertices[2] = 3;
    BrModelAdd(gBlend_model);

    gBlend_model2 = BrModelAllocate("BlendModel2", 4, 2);
    gBlend_model2->flags |= BR_MODF_UPDATEABLE;
    BrVector3SetFloat(&gBlend_model2->vertices[0].p, -0.5f, 0.f, 0.f);
    BrVector3SetFloat(&gBlend_model2->vertices[1].p, 0.5f, 0.f, 0.f);
    BrVector3SetFloat(&gBlend_model2->vertices[2].p, 0.5f, 1.f, 0.f);
    BrVector3SetFloat(&gBlend_model2->vertices[3].p, -0.5f, 1.f, 0.f);
    BrVector2Set(&gBlend_model2->vertices[0].map, 0.f, 1.f);
    BrVector2Set(&gBlend_model2->vertices[1].map, 1.f, 1.f);
    BrVector2Set(&gBlend_model2->vertices[2].map, 1.f, 0.f);
    BrVector2Set(&gBlend_model2->vertices[3].map, 0.f, 0.f);
    gBlend_model2->faces[0].vertices[0] = 0;
    gBlend_model2->faces[0].vertices[1] = 1;
    gBlend_model2->faces[0].vertices[2] = 2;
    gBlend_model2->faces[1].vertices[0] = 0;
    gBlend_model2->faces[1].vertices[1] = 2;
    gBlend_model2->faces[1].vertices[2] = 3;
    BrModelAdd(gBlend_model2);

    gBlend_actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    gBlend_actor->render_style = BR_RSTYLE_FACES;
    gBlend_actor->model = gBlend_model;

    memcpy(path, "SMOKE.PIX", sizeof("SMOKE.PIX"));
    pm = LoadPixelmap(path);
    if (pm == NULL) {
        FatalError(kFatalError_CantLoadPixelmapFile_S, path);
    }
    pm->map = gRender_palette;
    BrMapAdd(pm);

    for (i = 0; i < CARPOCALYPSE2_ASIZE(gBR_smoke_structs); i++) {
        gBR_smoke_structs[i].material = BrMaterialAllocate(NULL);
        gBR_smoke_structs[i].material->flags = BR_MATF_LIGHT | BR_MATF_PRELIT | BR_MATF_SMOOTH | BR_MATF_PERSPECTIVE;
        gBR_smoke_structs[i].material->extra_prim = gSmoke_extra_prim;
        gBR_smoke_structs[i].material->colour_map = pm;
        BrMaterialAdd(gBR_smoke_structs[i].material);
    }
#else
    br_pixelmap* pm;
    tPath_name path;
    int i;

    gBlend_model = BrModelAllocate("gBlend_model", 4, 2);
    gBlend_model2 = BrModelAllocate("gBlend_model2", 6, 4);
    gBlend_actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);

    for (i = 0; i < 35; i++) {
        gBR_smoke_structs[i].material = BrMaterialAllocate("some smoke");
        if (gBR_smoke_structs[i].material == NULL) {
            break;
        }
    }

    if (gBlend_model == NULL || i != 35 || gBlend_actor == NULL) {
        FatalError(kFatalError_OOM_S);
    }

    gBlend_actor->identifier = "gBlend_actor";
    gBlend_actor->model = gBlend_model;

    gBlend_model->faces[0].vertices[0] = 0;
    gBlend_model->faces[0].vertices[1] = 1;
    gBlend_model->faces[0].vertices[2] = 2;
    gBlend_model->faces[1].vertices[0] = 2;
    gBlend_model->faces[1].vertices[1] = 3;
    gBlend_model->faces[1].vertices[2] = 0;

    gBlend_model2->faces[0].vertices[0] = 0;
    gBlend_model2->faces[0].vertices[1] = 1;
    gBlend_model2->faces[0].vertices[2] = 2;
    gBlend_model2->faces[1].vertices[0] = 2;
    gBlend_model2->faces[1].vertices[1] = 3;
    gBlend_model2->faces[1].vertices[2] = 0;
    gBlend_model2->faces[2].vertices[0] = 1;
    gBlend_model2->faces[2].vertices[1] = 5;
    gBlend_model2->faces[2].vertices[2] = 2;
    gBlend_model2->faces[3].vertices[0] = 1;
    gBlend_model2->faces[3].vertices[1] = 4;
    gBlend_model2->faces[3].vertices[2] = 5;

    gBlend_model->vertices[0].p.v[0] = -1.0f;
    gBlend_model->vertices[0].p.v[1] = 1.0f;
    gBlend_model->vertices[0].p.v[2] = 0.0f;
    gBlend_model->vertices[1].p.v[0] = -1.0f;
    gBlend_model->vertices[1].p.v[1] = -1.0f;
    gBlend_model->vertices[1].p.v[2] = 0.0f;
    gBlend_model->vertices[2].p.v[0] = 1.0f;
    gBlend_model->vertices[2].p.v[1] = -1.0f;
    gBlend_model->vertices[2].p.v[2] = 0.0f;
    gBlend_model->vertices[3].p.v[0] = 1.0f;
    gBlend_model->vertices[3].p.v[1] = 1.0f;
    gBlend_model->vertices[3].p.v[2] = 0.0f;

    gBlend_model2->vertices[0].p.v[0] = -1.0f;
    gBlend_model2->vertices[0].p.v[1] = 1.0f;
    gBlend_model2->vertices[0].p.v[2] = 0.0f;
    gBlend_model2->vertices[1].p.v[0] = -1.0f;
    gBlend_model2->vertices[1].p.v[1] = 0.0f;
    gBlend_model2->vertices[1].p.v[2] = 0.0f;
    gBlend_model2->vertices[2].p.v[0] = 1.0f;
    gBlend_model2->vertices[2].p.v[1] = 0.0f;
    gBlend_model2->vertices[2].p.v[2] = 0.0f;
    gBlend_model2->vertices[3].p.v[0] = 1.0f;
    gBlend_model2->vertices[3].p.v[1] = 1.0f;
    gBlend_model2->vertices[3].p.v[2] = 0.0f;
    gBlend_model2->vertices[4].p.v[0] = -1.0f;
    gBlend_model2->vertices[4].p.v[1] = -0.25f;
    gBlend_model2->vertices[4].p.v[2] = 0.0f;
    gBlend_model2->vertices[5].p.v[0] = 1.0f;
    gBlend_model2->vertices[5].p.v[1] = -0.25f;
    gBlend_model2->vertices[5].p.v[2] = 0.0f;

    gBlend_model->flags |= BR_MODF_KEEP_ORIGINAL;
    gBlend_model2->flags |= BR_MODF_KEEP_ORIGINAL;

    memcpy(path, "SMOKE.PIX", sizeof("SMOKE.PIX"));
    pm = LoadPixelmap(path);
    if (pm == NULL) {
        FatalError(kFatalError_CantLoadPixelmapFile_S, path);
    }

    pm->map = gRender_palette;
    BrMapAdd(pm);

    for (i = 0; i < 35; i++) {
        gBR_smoke_structs[i].material->flags = BR_MATF_LIGHT | BR_MATF_PRELIT | BR_MATF_SMOOTH | BR_MATF_PERSPECTIVE;
        gBR_smoke_structs[i].material->extra_prim = (br_token_value*)&gDAT_00660148;
        gBR_smoke_structs[i].material->colour_map = pm;
        BrMaterialAdd(gBR_smoke_structs[i].material);
    }

    gBlend_model->vertices[0].map.v[0] = 0.0f;
    gBlend_model->vertices[0].map.v[1] = 100.0f - 100.0f / (br_scalar)pm->height;
    gBlend_model->vertices[1].map.v[0] = 0.0f;
    gBlend_model->vertices[1].map.v[1] = 0.0f;
    gBlend_model->vertices[2].map.v[0] = 100.0f - 100.0f / (br_scalar)pm->width;
    gBlend_model->vertices[2].map.v[1] = 0.0f;
    gBlend_model->vertices[3].map.v[0] = 100.0f - 100.0f / (br_scalar)pm->width;
    gBlend_model->vertices[3].map.v[1] = 100.0f - 100.0f / (br_scalar)pm->height;

    gBlend_model2->vertices[0].map.v[0] = 0.0f;
    gBlend_model2->vertices[0].map.v[1] = 100.0f - 100.0f / (br_scalar)pm->height;
    gBlend_model2->vertices[1].map.v[0] = 0.0f;
    gBlend_model2->vertices[1].map.v[1] = 0.5f;
    gBlend_model2->vertices[2].map.v[0] = 100.0f - 100.0f / (br_scalar)pm->width;
    gBlend_model2->vertices[2].map.v[1] = 0.5f;
    gBlend_model2->vertices[3].map.v[0] = 100.0f - 100.0f / (br_scalar)pm->width;
    gBlend_model2->vertices[3].map.v[1] = 100.0f - 100.0f / (br_scalar)pm->height;
    gBlend_model2->vertices[4].map.v[0] = 0.0f;
    gBlend_model2->vertices[4].map.v[1] = 0.0f;
    gBlend_model2->vertices[5].map.v[0] = 100.0f - 100.0f / (br_scalar)pm->width;
    gBlend_model2->vertices[5].map.v[1] = 0.0f;

    BrModelAdd(gBlend_model);
    BrModelAdd(gBlend_model2);
    BrActorAdd(gDont_render_actor, gBlend_actor);
#endif
}

// RenderRecordedSmokeCircles

// RecordSmokeCircle

// SmokeCircle3D

// ReplaySmoke

// GenerateContinuousSmoke

// RenderSmoke

// CreatePuffOfSmoke

// ResetSmoke

// FUNCTION: CARMA2_HW 0x004fb520
void C2_HOOK_FASTCALL AdjustSmoke(int pIndex, tU8 pType, br_vector3* pPos, br_scalar pRadius, br_scalar pStrength) {
    gSmoke[pIndex].type = pType;
    memcpy(&gSmoke[pIndex].radius, &pRadius, sizeof(pRadius));
    memcpy(&gSmoke[pIndex].strength, &pStrength, sizeof(pStrength));
    memcpy(&gSmoke[pIndex].pos.v[0], &pPos->v[0], sizeof(br_scalar));
    memcpy(&gSmoke[pIndex].pos.v[1], &pPos->v[1], sizeof(br_scalar));
    memcpy(&gSmoke[pIndex].pos.v[2], &pPos->v[2], sizeof(br_scalar));
    gSmoke_flags |= 1u << pIndex;
}

// ActorError

// FUNCTION: CARMA2_HW 0x004fb580
void C2_NAKED C2_HOOK_FASTCALL AdjustSmokeColumn(int pIndex, const tSmoke_column_core* pCore, int pVertex, int pColour) {
#ifndef CARPOCALYPSE2_MATCHING
    tU32 bit;

    bit = 1 << pIndex;
    gColumn_flags ^= bit;
    gSmoke_column[pIndex].core = *pCore;
    gSmoke_column[pIndex].vertex_index = pVertex;
    gSmoke_column[pIndex].colour = pColour;
    gSmoke_column[pIndex].frame_count[0] = 100;
    gSmoke_column[pIndex].frame_count[1] = 100;
    gSmoke_column[pIndex].frame_count[2] = 100;
    if (pColour == 0) {
        if ((gColumn_flags & bit) != 0) {
            BrActorAdd(gNon_track_actor, gSmoke_column[pIndex].flame_actor);
        } else if (gSmoke_column[pIndex].flame_actor->depth != 0) {
            BrActorRemove(gSmoke_column[pIndex].flame_actor);
        }
    }
#else
    __asm {
            mov eax, dword ptr [gColumn_flags]
            push esi
            mov esi, 1
            push edi
            shl esi, cl
            xor eax, esi
            mov dword ptr [gColumn_flags], eax
            mov eax, ecx
            shl eax, 5
            mov edi, dword ptr [edx]
            sub eax, ecx
            shl eax, 2
            lea ecx, [eax + gSmoke_column]
            mov dword ptr [eax + gSmoke_column], edi
            mov edi, dword ptr [edx + 4]
            mov dword ptr [ecx + 4], edi
            mov edi, dword ptr [edx + 8]
            mov dword ptr [ecx + 8], edi
            mov edx, dword ptr [edx + 0xc]
            mov dword ptr [ecx + 0xc], edx
            mov ecx, dword ptr [esp + 0xc]
            mov dword ptr [eax + gSmoke_column+0x74], ecx
            mov ecx, dword ptr [esp + 0x10]
            mov edx, 0x64
            mov dword ptr [eax + gSmoke_column+0x1c], ecx
            mov dword ptr [eax + gSmoke_column+0x28], edx
            mov dword ptr [eax + gSmoke_column+0x2c], edx
            test ecx, ecx
            mov dword ptr [eax + gSmoke_column+0x30], edx
            jne L004fb623
            mov ecx, dword ptr [gColumn_flags]
            mov eax, dword ptr [eax + gSmoke_column+0x24]
            test ecx, esi
            je L004fb613
            mov ecx, dword ptr [gNon_track_actor]
            push eax
            push ecx
            call BrActorAdd
            add esp, 8
            pop edi
            pop esi
            ret 8
    L004fb613:
            cmp word ptr [eax + 0x10], 0
            je L004fb623
            push eax
            call BrActorRemove
            add esp, 4
    L004fb623:
            pop edi
            pop esi
            ret 8
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

// CreateSmokeColumn2

// FUNCTION: CARMA2_HW 0x004fb890
void C2_NAKED C2_HOOK_FASTCALL CreateSmokeColumn(tCar_spec* pCar, int pType, int pRandom, int pLifetime) {
#ifndef CARPOCALYPSE2_MATCHING

    if (pCar->collision_info->last_special_volume != NULL
        && pCar->collision_info->last_special_volume->gravity_multiplier < 1.0f) {
        return;
    }
    SmudgeCar(pCar, pRandom);
    if (pCar->knackered) {
        SmudgeCar(pCar, pCar->fire_vertex[IRandomBetween(0, 0xb)]);
    }
    CreateSmokeColumn2(1, pCar->car_master_actor, pCar, NULL, pType, pRandom, pLifetime);
#else
    __asm {
            push ebx
            push esi
            mov esi, ecx
            push edi
            mov ebx, edx
            mov eax, dword ptr [esi + 8]
            mov eax, dword ptr [eax + 0x258]
            test eax, eax
            je L004fb8b7
            fld dword ptr [eax + 0x88]
            fcomp dword ptr [gFLOAT_0058afdc]
            fnstsw ax
            test ah, 1
            jne L004fb902
    L004fb8b7:
            mov edi, dword ptr [esp + 0x10]
            mov ecx, esi
            mov edx, edi
            call SmudgeCar
            mov eax, dword ptr [esi + 0x1d4]
            test eax, eax
            je L004fb8eb
            mov edx, 0xb
            xor ecx, ecx
            call IRandomBetween
            xor edx, edx
            mov ecx, esi
            mov dx, word ptr [esi + eax*2 + 0xac]
            call SmudgeCar
    L004fb8eb:
            mov ecx, dword ptr [esp + 0x14]
            mov edx, dword ptr [esi + 0x10]
            push ecx
            push edi
            push ebx
            push 0
            push esi
            mov ecx, 1
            call CreateSmokeColumn2
    L004fb902:
            pop edi
            pop esi
            pop ebx
            ret 8
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

// GenerateSmokeShades

// FUNCTION: CARMA2_HW 0x004fb9c0
void C2_HOOK_FASTCALL GenerateItFoxShadeTable(void) {
    if (gIt_shade_table == NULL) {
        gIt_shade_table = GenerateDarkenedShadeTable(16, gRender_palette, 0, 255, 254, .25f, .5f, .75f, .6f);
    }
}

// FUNCTION: CARMA2_HW 0x004fba00
void C2_HOOK_FASTCALL AdjustFlame(int pIndex, int pFrame_count, br_scalar pScale_x, br_scalar pScale_y, br_scalar pOffset_x, br_scalar pOffset_z) {
    int i;
    tSmoke_column* col;

    i = pIndex & 0xf;
    col = &gSmoke_column[(pIndex & ~0xf) >> 4];
    col->frame_count[i] = pFrame_count;
    col->scale_x[i] = pScale_x;
    col->scale_y[i] = pScale_y;
    col->offset_x[i] = pOffset_x;
    col->offset_z[i] = pOffset_z;
}

// ReplayFlame

// FlameAnimate

// DoSmokeColumn

// ReplaySmokeColumn

// MungeSmokeColumn

// FUNCTION: CARMA2_HW 0x004fc2e0
void C2_HOOK_FASTCALL DisposeFlameResources(void) {
    int i;
    int j;
    br_actor* child;

    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gFlame_map); i++) {
        BrMapRemove(gFlame_map[i]);
        BrPixelmapFree(gFlame_map[i]);
    }
    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gSmoke_column); i++) {
        if ((gColumn_flags & (1u << i)) != 0 && gSmoke_column[i].colour == 0) {
            BrActorRemove(gSmoke_column[i].flame_actor);
        }
        child = gSmoke_column[i].flame_actor->children;
        for (j = 0; j < 3; j++) {
            BrMaterialRemove(child->material);
            BrMaterialFree(child->material);
            child = child->next;
        }
        BrActorFree(gSmoke_column[i].flame_actor);
    }
    BrModelRemove(gLollipop_model);
    BrModelFree(gLollipop_model);
}

// InitFlame

// InitSplash

// DisposeSplash

// DrawTheGlow

// PipeInstantUnSmudge

// SmudgeCar

// FUNCTION: CARMA2_HW 0x004fc9d0
void C2_HOOK_FASTCALL SetSmokeOn(int pSmoke_on) {
    gSmoke_on = pSmoke_on;
}

// FUNCTION: CARMA2_HW 0x004fc9e0
void C2_HOOK_FASTCALL DisposeFlame(void) {
    br_actor** flame_actor;
    int i;

    gSmoke_flags = 0;
    i = 0;
    flame_actor = &gSmoke_column[0].flame_actor;
    do {
        if (gColumn_flags & (1 << i)) {
            BrActorRemove(*flame_actor);
        }
        flame_actor += 31;
        i++;
    } while ((int)flame_actor < (int)((tU8*)gSmoke_column + sizeof(gSmoke_column) + ((tU8*)&gSmoke_column[0].flame_actor - (tU8*)&gSmoke_column[0])));
    gColumn_flags = 0;
}

// FUNCTION: CARMA2_HW 0x004fca30
int C2_HOOK_FASTCALL GetSmokeOn(void) {
    return gSmoke_on;
}

// StopCarSmoking

// StopCarSmokingInstantly

// StopObjectSmokingInstantly

// ConditionalSmokeColumn

// SingleSplash

// FUNCTION: CARMA2_HW 0x004fd530
void C2_HOOK_FASTCALL SingleSplash(tPhysics_object* pObject, const br_matrix34* pMatrix, const br_vector3* pVelocity, const br_vector3* pNormal_velocity, const br_vector3* pOmega, const br_vector3* pPoint, const br_vector3* pNormal, tU32 pTime) {
    br_vector3 cross;
    br_vector3 velocity;
    br_vector3 recoil;
    br_scalar scaled;
    br_scalar speed;
    br_scalar size;
    br_scalar range;
    double dot;
    double random;
    double wide;
    tCar_spec* car;
    int index;
    int i;

    BrMatrix34ApplyP((br_vector3*)gSplash[gSplash_next].actor->t.t.mat.m[3], pPoint, pMatrix);
    cross.v[0] = SmokeScalarRound((double)pOmega->v[1] * pPoint->v[2] - (double)pOmega->v[2] * pPoint->v[1]);
    cross.v[1] = SmokeScalarRound((double)pOmega->v[2] * pPoint->v[0] - (double)pOmega->v[0] * pPoint->v[2]);
    cross.v[2] = SmokeScalarRound((double)pOmega->v[0] * pPoint->v[1] - (double)pOmega->v[1] * pPoint->v[0]);
    BrMatrix34ApplyV(&velocity, &cross, pMatrix);
    for (i = 0; i < 3; i++) {
        scaled = SmokeScalarRound((double)pVelocity->v[i] * 6.9);
        velocity.v[i] = SmokeScalarRound((double)velocity.v[i] + scaled);
    }
    speed = SmokeScalarRound(sqrt(((double)velocity.v[0] * velocity.v[0]
        + (double)velocity.v[1] * velocity.v[1]) + (double)velocity.v[2] * velocity.v[2]));
    dot = ((double)pNormal->v[1] * velocity.v[1] + (double)pNormal->v[2] * velocity.v[2])
        + (double)pNormal->v[0] * velocity.v[0];
    wide = SplashSubtractFromScalar(fabs(dot) * -5.f, &speed) * .006666666828095913f - -.04782608523964882f;
    size = SmokeScalarRound(wide);
    if (wide > .5f) {
        size = .5f;
    }
    dot = ((double)pNormal_velocity->v[2] * pPoint->v[2] + (double)pNormal_velocity->v[1] * pPoint->v[1])
        + (double)pNormal_velocity->v[0] * pPoint->v[0];
    if (!(dot >= 0.0)) {
        size = SmokeScalarRound((double)size * .5f);
    }
    index = gSplash_next;
    gSplash[index].size = SmokeScalarRound(SRandomBetween(SmokeScalarRound((double)size * .5f), size));
    if (!((tU32)gSplash_flags & (1u << (gSplash_next & 31)))) {
        BrActorAdd(gNon_track_actor, gSplash[index].actor);
    }
    gSplash_flags = (int)((tU32)gSplash_flags | (1u << (gSplash_next & 31)));
    gSplash[gSplash_next].just_done = 1;
    if (pObject != NULL && !gAction_replay_mode) {
        car = pObject->flags_0x238 == 1 ? (tCar_spec*)pObject->owner : NULL;
        if (car != NULL) {
            random = SRandomBetween(0.f, 1.f);
            if (random < (double)pTime * .003f) {
                range = SmokeScalarRound(sqrt((double)speed * .014285714365541935f) * 15.f);
                for (i = 0; i < 3; i++) {
                    wide = (double)velocity.v[i] * .14492753148078918f;
                    velocity.v[i] = SmokeScalarRound(wide);
                    recoil.v[i] = SmokeScalarRound(wide * -.1f);
                }
                if (range > 15.f) {
                    range = 15.f;
                }
                random = SRandomBetween(5.f, range);
                recoil.v[1] = SmokeScalarRound((double)recoil.v[1] - random * -.14492753623188406);
                BrMatrix34TApplyV(&velocity, &recoil, &pObject->actor->t.t.mat);
                cross.v[0] = SmokeScalarRound(((double)pObject->water_normal.v[2] * velocity.v[1]
                    - (double)pObject->water_normal.v[1] * velocity.v[2]) * .5f);
                cross.v[1] = SmokeScalarRound(((double)pObject->water_normal.v[0] * velocity.v[2]
                    - (double)pObject->water_normal.v[2] * velocity.v[0]) * .5f);
                cross.v[2] = SmokeScalarRound(((double)pObject->water_normal.v[1] * velocity.v[0]
                    - (double)pObject->water_normal.v[0] * velocity.v[1]) * .5f);
                dot = ((double)pPoint->v[1] * cross.v[1] + (double)pPoint->v[2] * cross.v[2])
                    + (double)pPoint->v[0] * cross.v[0];
                for (i = 0; i < 3; i++) {
                    velocity.v[i] = SmokeScalarRound(dot > 0.0 ? (double)velocity.v[i] + cross.v[i]
                        : (double)velocity.v[i] - cross.v[i]);
                }
                index = gSpark_next;
                gSparks[index].pos.v[0] = pPoint->v[0];
                gSparks[index].pos.v[1] = pPoint->v[1];
                gSparks[index].pos.v[2] = pPoint->v[2];
                BrVector3SetFloat(&gSparks[index].normal, 0.f, 0.f, 0.f);
                index = gSpark_next;
                memcpy(&gSparks[index].v, &velocity, sizeof(velocity));
                gSparks[index].count = 500;
                gSparks[index].car = car;
                gSparks[index].time_sync = 1;
    gSparks[index].colour = 1;
                gSpark_flags = (int)((tU32)gSpark_flags | (1u << (index & 31)));
                gSpark_next = (int)((tU32)index + 1u);
                if (gSpark_next >= 32) {
                    gSpark_next = 0;
                }
            }
        }
    }
    gSplash_next = (int)((tU32)gSplash_next + 1u);
    if ((tU32)gSplash_next >= 32u) {
        gSplash_next = 0;
    }
}

// GeneralCreateSplash

// CreateSplash

// GetVelocitiesFromMatrices

// MungeSplash

// GetSmokeShadeTables

// FreeSmokeShadeTables

// LoadInKevStuff

// DisposeKevStuff

// DisposeKevStuffCar

// MakeCarIt

// StopCarBeingIt

// ForEveryModelMaterial

// ForEveryActorMaterial

// ForEveryActorMaterialNoGrooves

// ForEveryCarMaterial

// ForEveryCarModelCB

// ForEveryCarModel

// MightBlendifyMaterial

// BlendifyMaterialCB

// FUNCTION: CARMA2_HW 0x004fecf0
void C2_HOOK_FASTCALL BlendifyCar(tCar_spec* pCar) {
    ForEveryCarMaterial(pCar, BlendifyMaterialCB, 1);
}

// UnBlendifyMaterialCB

// FUNCTION: CARMA2_HW 0x004fed40
void C2_HOOK_FASTCALL UnBlendifyCar(tCar_spec* pCar_spec) {
    ForEveryCarMaterial(pCar_spec, UnBlendifyMaterialCB, 1);
}

// FUNCTION: CARMA2_HW 0x004fed90
int C2_HOOK_FASTCALL IsCarSmoking(tCar_spec* pCar) {
    int i;

    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gSmoke_column); i++) {
        if ((gColumn_flags & (1 << i)) && gSmoke_column[i].core.car == pCar) {
            return 1;
        }
    }
    return 0;
}

// ClearSplashReplay

// FUNCTION: CARMA2_HW 0x004fee10
void* C2_HOOK_FASTCALL GetSplashIndex(tPhysics_object* pObject) {
    if (PHILGetObjectProperty(pObject, 6) != gFLOAT_0058b008) {
        return pObject;
    }
    if (pObject != NULL && pObject->owner != NULL && pObject->flags_0x238 == 1) {
        return pObject->actor;
    }
    if (pObject != NULL && pObject->owner != NULL && pObject->flags_0x238 >= 2 && pObject->flags_0x238 <= 4) {
        return pObject->owner;
    }
    return NULL;
}

// FUNCTION: CARMA2_HW 0x004fee70
void C2_HOOK_FASTCALL ClearSplashes(tPhysics_object* pObject) {
    void* object;
    int i;

    if (pObject->water_d == 10000.f) {
        return;
    }
    pObject->water_d = 10000.f;
    object = GetSplashIndex(pObject);
    if (object == NULL) {
        return;
    }
    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gReplay_splashes); i++) {
        if ((gReplay_splash_flags & (1 << i)) && object == gReplay_splashes[i].object.actor) {
            ARStartPipingSession(ePipe_chunk_extended_splash);
            AddExtendedSplashToPipingSession(pObject, object);
            AREndPipingSession();
            return;
        }
    }
}

// AdjustSplashReplay

// InitReplaySplashes
// FUNCTION: CARMA2_HW 0x004ff030
void C2_HOOK_FASTCALL InitReplaySplashes(void) {
    tPhysics_object* object;

    object = gList_collision_infos;
    gReplay_splash_flags = 0;
    if (object != NULL) {
        do {
            if (object->water_d != 10000.0f) {
                void* replay_object;
                tU8 splash_type;
                tCollision_info_owner* owner;

                replay_object = NULL;
                if (PHILGetObjectProperty(object, 6) != gFLOAT_0058b008) {
                    replay_object = object;
                    splash_type = 0xff;
                } else if (object != NULL && object->owner != NULL && object->flags_0x238 == 1) {
                    owner = object->owner;
                    if (owner == NULL || *(int*)&owner->field_0x0c <= 5) {
                        replay_object = object->actor;
                        splash_type = 1;
                    }
                } else if (object != NULL && object->owner != NULL && object->flags_0x238 >= 2 && object->flags_0x238 <= 4) {
                    replay_object = object->owner;
                    splash_type = object->flags_0x238;
                }
                if (replay_object != NULL) {
                    RecordReplaySplash(replay_object, splash_type, *(const tU32*)&object->water_d,
                                       (const tU32*)&object->water_normal, (const tU32*)&object->bb1);
                }
            }
            object = object->next;
        } while (object != NULL);
    }
}

// FUNCTION: CARMA2_HW 0x004fef40
void C2_NAKED C2_HOOK_FASTCALL RecordReplaySplash(void* pObject, int pType, tU32 pWater_density, const tU32* pNormal, const tU32* pBounds) {
#ifndef CARPOCALYPSE2_MATCHING
    register int* replay_splash;
    register int i;
    register int slot_index;

    slot_index = -1;
    i = 0;
    replay_splash = (int*)gReplay_splashes;
    do {
        if ((gReplay_splash_flags & (1 << i)) == 0) {
            if (slot_index < 0) {
                slot_index = i;
            }
        } else if ((int)pObject == replay_splash[0]) {
            goto update_splash;
        }
        replay_splash += 12;
        i++;
    } while ((int)replay_splash < (int)(gReplay_splashes + CARPOCALYPSE2_ASIZE(gReplay_splashes)));
    if (slot_index < 0) {
        slot_index = IRandomBetween(0, 10);
    }
    gReplay_splash_flags |= 1 << slot_index;
    i = slot_index;
    ((int*)gReplay_splashes)[i * 12] = (int)pObject;
    ((int*)gReplay_splashes)[i * 12 + 1] = pType;
update_splash:
    ((int*)gReplay_splashes)[i * 12 + 2] = pWater_density;
    ((int*)gReplay_splashes)[i * 12 + 3] = pNormal[0];
    ((int*)gReplay_splashes)[i * 12 + 4] = pNormal[1];
    ((int*)gReplay_splashes)[i * 12 + 5] = pNormal[2];
    ((int*)gReplay_splashes)[i * 12 + 6] = pBounds[0];
    ((int*)gReplay_splashes)[i * 12 + 7] = pBounds[1];
    ((int*)gReplay_splashes)[i * 12 + 8] = pBounds[2];
    ((int*)gReplay_splashes)[i * 12 + 9] = pBounds[3];
    ((int*)gReplay_splashes)[i * 12 + 10] = pBounds[4];
    ((int*)gReplay_splashes)[i * 12 + 11] = pBounds[5];
#else
    __asm {
            push ebx
            push ebp
            push esi
            mov esi, dword ptr [gReplay_splash_flags]
            push edi
            mov ebx, edx
            mov edi, ecx
            or edx, 0xffffffff
            xor ecx, ecx
            mov eax, offset gReplay_splashes
    L004fef58:
            mov ebp, 1
            shl ebp, cl
            test ebp, esi
            jne L004fef6b
            test edx, edx
            jge L004fef73
            mov edx, ecx
            jmp L004fef73
    L004fef6b:
            cmp edi, dword ptr [eax]
            je L004ff023
    L004fef73:
            add eax, 0x30
            inc ecx
            cmp eax, offset gDust_rotate
            jl L004fef58
            test edx, edx
            jge L004fef96
            mov edx, 0xa
            xor ecx, ecx
            call IRandomBetween
            mov esi, dword ptr [gReplay_splash_flags]
            jmp L004fef98
    L004fef96:
            mov eax, edx
    L004fef98:
            mov edx, 1
            mov ecx, eax
            shl edx, cl
            lea eax, [eax + eax*2]
            or esi, edx
            shl eax, 4
            mov dword ptr [gReplay_splash_flags], esi
            mov dword ptr [eax + gReplay_splashes], edi
            mov dword ptr [eax + gReplay_splashes+0x4], ebx
    L004fefbb:
            mov ecx, dword ptr [esp + 0x14]
            pop edi
            mov dword ptr [eax + gReplay_splashes+0x8], ecx
            mov ecx, dword ptr [esp + 0x14]
            pop esi
            pop ebp
            mov edx, dword ptr [ecx]
            pop ebx
            mov dword ptr [eax + gReplay_splashes+0xc], edx
            mov edx, dword ptr [ecx + 4]
            mov dword ptr [eax + gReplay_splashes+0x10], edx
            mov ecx, dword ptr [ecx + 8]
            mov dword ptr [eax + gReplay_splashes+0x14], ecx
            mov ecx, dword ptr [esp + 0xc]
            mov edx, dword ptr [ecx]
            mov dword ptr [eax + gReplay_splashes+0x18], edx
            mov edx, dword ptr [ecx + 4]
            mov dword ptr [eax + gReplay_splashes+0x1c], edx
            mov edx, dword ptr [ecx + 8]
            mov dword ptr [eax + gReplay_splashes+0x20], edx
            mov edx, dword ptr [ecx + 0xc]
            mov dword ptr [eax + gReplay_splashes+0x24], edx
            mov edx, dword ptr [ecx + 0x10]
            mov dword ptr [eax + gReplay_splashes+0x28], edx
            mov ecx, dword ptr [ecx + 0x14]
            mov dword ptr [eax + gReplay_splashes+0x2c], ecx
            ret 0xc
    L004ff023:
            lea eax, [ecx + ecx*2]
            shl eax, 4
            jmp L004fefbb
            nop
            nop
            nop
            nop
            nop
    }
#endif
}

typedef int C2_HOOK_FASTCALL tStopCarBeingItModel_cbfn(br_model* pModel);
void C2_HOOK_CDECL AFEModelUpdate(br_actor* pActor, br_model* pModel, br_material* pMaterial, void* pOrder_table, br_uint_8 pStyle, int pOn_screen);

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x004f8800
void C2_NAKED C2_HOOK_FASTCALL CreateShrapnelShower(const br_vector3* pPosition, const br_vector3* pVelocity, const br_vector3* pDirection, br_scalar pStrength, tCar_spec* pCar1, tCar_spec* pCar2, int pMaterialCount, br_material** pMaterials) {
#ifndef CARPOCALYPSE2_MATCHING
    float fVar1;
    float fVar2;
    float fVar3;
    float fVar4;
    float fVar7;
    float fVar8;
    float fVar9;
    float fVar10;
    int shrapnel_count;
    int material_index;
    tCar_spec* selected_car;
    br_vector3 random_vector;
    br_vector3 scaled_direction;
    br_vector3 velocity;

    if (pStrength < gFLOAT_0058aff0) {
        return;
    }
    if (pMaterials != NULL) {
        fVar8 = gFLOAT_0058b030;
    } else {
        fVar8 = gFLOAT_0058b034;
    }
    if (pVelocity->v[1] < gFLOAT_0058b008) {
        fVar8 -= pVelocity->v[1];
    }
    fVar9 = pDirection->v[1] * fVar8;
    fVar1 = pDirection->v[2];
    fVar2 = pDirection->v[0];
    fVar3 = pVelocity->v[0];
    fVar4 = pVelocity->v[2];
    fVar8 = (fVar8 + pVelocity->v[1]) - pDirection->v[1] * fVar9;
    BrVector3Set(&velocity, fVar3 - fVar2 * fVar9, fVar8, fVar4 - fVar1 * fVar9);
    shrapnel_count = (int)(pStrength * gFLOAT_0058b038);
    shrapnel_count *= 12;
    if (shrapnel_count > 20) {
        shrapnel_count = 20;
    }
    fVar10 = pStrength - gFLOAT_0058b03c;
    fVar10 *= gFLOAT_0058b040;
    fVar10 *= gFLOAT_0058b044;
    if (shrapnel_count > 0) {
        do {
            if (((1 << gShrapnel_index) & gShrapnel_flags) == 0) {
                BrActorAdd(gNon_track_actor, gShrapnel[gShrapnel_index].actor);
            }
            gShrapnel_flags |= 1 << gShrapnel_index;
            BrVector3Copy(&gShrapnel[gShrapnel_index].actor->t.t.translate.t, pPosition);
            BrVector3SetFloat(&random_vector, FRandomBetween(-fVar10, fVar10), FRandomBetween((float)(gDOUBLE_0058b048 - fVar8), fVar10), FRandomBetween(-fVar10, fVar10));
            fVar7 = BrVector3Dot(pDirection, &random_vector);
            BrVector3Scale(&scaled_direction, pDirection, fVar7);
            BrVector3Sub(&gShrapnel[gShrapnel_index].v, &random_vector, &scaled_direction);
            BrVector3Accumulate(&gShrapnel[gShrapnel_index].v, &velocity);
            gShrapnel[gShrapnel_index].time_sync = gPHIL_mechanics_time_sync;
            gShrapnel[gShrapnel_index].age = 0;
            if (pMaterials != NULL) {
                material_index = IRandomBetween(0, pMaterialCount - 1);
                gShrapnel[gShrapnel_index].actor->material = pMaterials[material_index];
            } else if (IRandomBetween(0, 2) != 0) {
                selected_car = pCar1;
                if (IRandomBetween(0, 1) == 0) {
                    selected_car = pCar2;
                }
                material_index = IRandomBetween(0, selected_car->count_shrapnel_materials - 1);
                gShrapnel[gShrapnel_index].actor->material = selected_car->shrapnel_materials[material_index];
            } else {
                gShrapnel[gShrapnel_index].actor->material = gBlack_material;
            }
            gShrapnel_index += 1;
            if (gShrapnel_index >= 30) {
                gShrapnel_index = 0;
            }
            shrapnel_count--;
        } while (shrapnel_count != 0);
    }
#else
    __asm {
            fld dword ptr [esp + 8]
            fcomp dword ptr [gFLOAT_0058aff0]
            sub esp, 0x20
            push ebx
            push ebp
            fnstsw ax
            push esi
            push edi
            test ah, 1
            mov ebx, ecx
            jne L004f8b38
            mov eax, dword ptr [esp + 0x48]
            test eax, eax
            je L004f882e
            fld dword ptr [gFLOAT_0058b030]
            jmp L004f8834
    L004f882e:
            fld dword ptr [gFLOAT_0058b034]
    L004f8834:
            fld dword ptr [edx + 4]
            fcomp dword ptr [gFLOAT_0058b008]
            fnstsw ax
            test ah, 1
            je L004f8847
            fsub dword ptr [edx + 4]
    L004f8847:
            mov esi, dword ptr [esp + 0x34]
            fld dword ptr [esi + 4]
            fmul st(0), st(1)
            fld dword ptr [esi + 4]
            fld dword ptr [esi + 8]
            fld dword ptr [esi]
            fxch st(4)
            fadd dword ptr [edx + 4]
            fxch st(3)
            fxch st(4)
            fmul st(0), st(4)
            fxch st(2)
            fmul st(0), st(4)
            fxch st(1)
            fmul st(0), st(4)
            fld dword ptr [esp + 0x38]
            fmul dword ptr [gFLOAT_0058b038]
            fxch st(3)
            fsubr dword ptr [edx]
            fxch st(1)
            fsubr dword ptr [edx + 8]
            fxch st(2)
            fsubp st(4), st(0)
            fstp dword ptr [esp + 0x24]
            fstp dword ptr [esp + 0x2c]
            fxch st(1)
            fstp dword ptr [esp + 0x28]
            call _ftol
            lea eax, [eax + eax*2]
            shl eax, 2
            cmp eax, 0x14
            fstp st(0)
            jle L004f88a7
            mov eax, 0x14
    L004f88a7:
            fld dword ptr [esp + 0x38]
            fsub dword ptr [gFLOAT_0058b03c]
            test eax, eax
            fmul dword ptr [gFLOAT_0058b040]
            fmul dword ptr [gFLOAT_0058b044]
            fstp dword ptr [esp + 0x38]
            jle L004f8b38
            fld dword ptr [esp + 0x28]
            fsubr qword ptr [gDOUBLE_0058b048]
            fld dword ptr [esp + 0x38]
            fchs
            fxch st(1)
            fstp dword ptr [esp + 0x14]
            mov edi, dword ptr [esp + 0x38]
            mov ecx, dword ptr [gShrapnel_index]
            fstp dword ptr [esp + 0x10]
            mov ebp, dword ptr [esp + 0x10]
            mov dword ptr [esp + 0x34], eax
    L004f88f5:
            mov edx, dword ptr [gShrapnel_flags]
            mov eax, 1
            shl eax, cl
            test edx, eax
            jne L004f8929
            lea edx, [ecx + ecx*4]
            lea eax, [ecx + edx*2]
            mov edx, dword ptr [gNon_track_actor]
            mov ecx, dword ptr [eax*4 + gShrapnel]
            push ecx
            push edx
            call BrActorAdd
            mov ecx, dword ptr [gShrapnel_index]
            add esp, 8
    L004f8929:
            mov edx, dword ptr [gShrapnel_flags]
            mov eax, 1
            shl eax, cl
            push edi
            push ebp
            or edx, eax
            mov dword ptr [gShrapnel_flags], edx
            lea edx, [ecx + ecx*4]
            lea eax, [ecx + edx*2]
            mov edx, dword ptr [ebx]
            mov ecx, dword ptr [eax*4 + gShrapnel]
            mov dword ptr [ecx + 0x50], edx
            mov eax, dword ptr [gShrapnel_index]
            lea ecx, [eax + eax*4]
            lea edx, [eax + ecx*2]
            mov ecx, dword ptr [ebx + 4]
            mov eax, dword ptr [edx*4 + gShrapnel]
            mov dword ptr [eax + 0x54], ecx
            mov eax, dword ptr [gShrapnel_index]
            lea edx, [eax + eax*4]
            lea eax, [eax + edx*2]
            mov edx, dword ptr [ebx + 8]
            mov ecx, dword ptr [eax*4 + gShrapnel]
            mov dword ptr [ecx + 0x58], edx
            call FRandomBetween
            mov eax, dword ptr [esp + 0x14]
            push ecx
            fstp dword ptr [esp]
            push edi
            push eax
            call FRandomBetween
            push ecx
            fstp dword ptr [esp]
            push edi
            push ebp
            call FRandomBetween
            push ecx
            lea ecx, [esp + 0x24]
            fstp dword ptr [esp]
            push ecx
            call BrVector3SetFloat
            fld dword ptr [esi + 4]
            fmul dword ptr [esp + 0x2c]
            fld dword ptr [esi + 8]
            fmul dword ptr [esp + 0x30]
            mov eax, dword ptr [gShrapnel_index]
            mov ecx, dword ptr [esp + 0x58]
            add esp, 0x10
            faddp st(1), st(0)
            fld dword ptr [esi]
            fmul dword ptr [esp + 0x18]
            lea edx, [eax + eax*4]
            faddp st(1), st(0)
            fld dword ptr [esi + 4]
            fmul st(0), st(1)
            fld dword ptr [esi + 8]
            fmul st(0), st(2)
            fld dword ptr [esi]
            fmul st(0), st(3)
            fld dword ptr [esp + 0x20]
            fld dword ptr [esp + 0x1c]
            fsub st(0), st(4)
            fxch st(1)
            fsub st(0), st(3)
            fxch st(2)
            fsubr dword ptr [esp + 0x18]
            fld dword ptr [esp + 0x24]
            fxch st(1)
            lea eax, [eax + edx*2]
            shl eax, 2
            fstp dword ptr [eax + gShrapnel+0x4]
            test ecx, ecx
            fxch st(1)
            fstp dword ptr [eax + gShrapnel+0x8]
            fxch st(1)
            fstp dword ptr [eax + gShrapnel+0xc]
            mov ecx, dword ptr [gPHIL_mechanics_time_sync]
            fadd dword ptr [eax + gShrapnel+0x4]
            fld dword ptr [esp + 0x28]
            fld dword ptr [esp + 0x2c]
            fxch st(2)
            fstp dword ptr [eax + gShrapnel+0x4]
            fadd dword ptr [eax + gShrapnel+0x8]
            fstp dword ptr [eax + gShrapnel+0x8]
            fadd dword ptr [eax + gShrapnel+0xc]
            fstp dword ptr [eax + gShrapnel+0xc]
            mov dword ptr [eax + gShrapnel+0x10], ecx
            mov dword ptr [eax + gShrapnel+0x14], 0
            fstp st(0)
            fstp st(0)
            fstp st(0)
            je L004f8a90
            mov edx, dword ptr [esp + 0x44]
            xor ecx, ecx
            dec edx
            call IRandomBetween
            mov ecx, dword ptr [gShrapnel_index]
            lea edx, [ecx + ecx*4]
            lea ecx, [ecx + edx*2]
            mov edx, dword ptr [ecx*4 + gShrapnel]
            mov ecx, dword ptr [esp + 0x48]
            mov eax, dword ptr [ecx + eax*4]
            jmp L004f8b0c
    L004f8a90:
            mov edx, 2
            xor ecx, ecx
            call IRandomBetween
            test eax, eax
            jne L004f8abd
            mov eax, dword ptr [gShrapnel_index]
            lea ecx, [eax + eax*4]
            lea edx, [eax + ecx*2]
            mov ecx, dword ptr [gBlack_material]
            mov eax, dword ptr [edx*4 + gShrapnel]
            mov dword ptr [eax + 0x1c], ecx
            jmp L004f8b0f
    L004f8abd:
            mov edx, 1
            xor ecx, ecx
            call IRandomBetween
            test eax, eax
            je L004f8ad7
            mov edx, dword ptr [esp + 0x3c]
            mov dword ptr [esp + 0x38], edx
            jmp L004f8adf
    L004f8ad7:
            mov eax, dword ptr [esp + 0x40]
            mov dword ptr [esp + 0x38], eax
    L004f8adf:
            mov ecx, dword ptr [esp + 0x38]
            mov edx, dword ptr [ecx + 0x6c]
            xor ecx, ecx
            dec edx
            call IRandomBetween
            mov ecx, dword ptr [gShrapnel_index]
            lea edx, [ecx + ecx*4]
            lea ecx, [ecx + edx*2]
            mov edx, dword ptr [ecx*4 + gShrapnel]
            mov ecx, dword ptr [esp + 0x38]
            mov eax, dword ptr [ecx + eax*4 + 0x84]
    L004f8b0c:
            mov dword ptr [edx + 0x1c], eax
    L004f8b0f:
            mov ecx, dword ptr [gShrapnel_index]
            inc ecx
            cmp ecx, 0x1e
            mov dword ptr [gShrapnel_index], ecx
            jl L004f8b29
            xor ecx, ecx
            mov dword ptr [gShrapnel_index], ecx
    L004f8b29:
            mov eax, dword ptr [esp + 0x34]
            dec eax
            mov dword ptr [esp + 0x34], eax
            jne L004f88f5
    L004f8b38:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x20
            ret 0x18
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
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004f8b50
void C2_HOOK_FASTCALL FUN_004f8b50(const br_vector3* pPosition, const br_vector3* pVelocity, const br_vector3* pDirection, br_scalar pStrength, int pMaterialCount, br_material** pMaterials) {
    CreateShrapnelShower(pPosition, pVelocity, pDirection, pStrength, NULL, NULL, pMaterialCount, pMaterials);
}

// FUNCTION: CARMA2_HW 0x004f8b70
void C2_HOOK_FASTCALL FUN_004f8b70(const br_vector3* pPosition, const br_vector3* pVelocity, const br_vector3* pDirection, br_scalar pStrength, tCar_spec* pCar1, tCar_spec* pCar2) {
    CreateShrapnelShower(pPosition, pVelocity, pDirection, pStrength, pCar1, pCar2, 0, NULL);
}

// FUNCTION: CARMA2_HW 0x004f95e0
int C2_HOOK_FASTCALL SetCarModelColorCycle(br_model* pModel) {
    pModel->flags |= 0x20;
    pModel->custom = (void*)AFEModelUpdate;
    return 1;
}

// FUNCTION: CARMA2_HW 0x004fa8d0
void C2_HOOK_FASTCALL DustRotate(void) {

    gDust_rotate++;
    if (gDust_rotate >= gNum_dust_tables) {
        gDust_rotate = 0;
    }
    NewTextHeadupSlot(4, 0, 1000, -4, "Dust colour rotated");
}

// FUNCTION: CARMA2_HW 0x004fec30
intptr_t C2_HOOK_CDECL StopCarBeingItActorCallback(br_actor* pActor, tStopCarBeingItModel_cbfn* pCallback) {
    tUser_crush_data* user_crush_data;

    if (pActor->type != BR_ACTOR_MODEL || pActor->model == NULL) {
        return 0;
    }
    user_crush_data = pActor->user;
    if (user_crush_data == NULL || user_crush_data->groove != NULL) {
        return 0;
    }
    pCallback(pActor->model);
    return 0;
}
// FUNCTION: CARMA2_HW 0x004fe2c0
void C2_NAKED C2_HOOK_FASTCALL RemoveCarFromEffects(tCar_spec* pCar) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;

    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gSmoke_column); i++) {
        if (gSmoke_column[i].core.car == pCar) {
            gSmoke_column[i].lifetime = 0;
            gSmoke_column[i].core.car = NULL;
            gSmoke_column[i].core.car_actor = NULL;
        }
    }
    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gSparks); i++) {
        if ((gSpark_flags & (1 << i)) != 0 && gSparks[i].car == pCar) {
            gSparks[i].count = 0;
            gSpark_flags &= ~(1 << i);
        }
    }
    if (pCar == gCar_to_view) {
        gCamera_yaw = 0;
        gCar_to_view = &gProgram_state.current_car;
        InitialiseExternalCamera();
        PositionExternalCamera(gCar_to_view, 200);
        gCar_to_view = &gProgram_state.current_car;
    }
#else
    __asm {
            push ebx
            push esi
            push edi
            mov edi, ecx
            mov eax, offset gSmoke_column+0x4
            xor ebx, ebx
    L004fe2cc:
            cmp dword ptr [eax], edi
            jne L004fe2d8
            mov dword ptr [eax + 0x10], ebx
            mov dword ptr [eax], ebx
            mov dword ptr [eax + 8], ebx
    L004fe2d8:
            add eax, 0x7c
            cmp eax, offset gSparks+0x4
            jl L004fe2cc
            mov esi, dword ptr [gSpark_flags]
            xor ecx, ecx
            mov edx, offset gSparks
    L004fe2ef:
            mov eax, 1
            shl eax, cl
            test esi, eax
            je L004fe305
            cmp dword ptr [edx + 0x38], edi
            jne L004fe305
            not eax
            mov dword ptr [edx], ebx
            and esi, eax
    L004fe305:
            add edx, 0x40
            inc ecx
            cmp edx, offset gLollipop_model
            jl L004fe2ef
            mov eax, dword ptr [gCar_to_view]
            mov dword ptr [gSpark_flags], esi
            cmp edi, eax
            jne L004fe34d
            mov esi, offset gProgram_state+0xac
            mov word ptr [gCamera_yaw], bx
            mov dword ptr [gCar_to_view], esi
            call InitialiseExternalCamera
            mov ecx, dword ptr [gCar_to_view]
            mov edx, 0xc8
            call PositionExternalCamera
            mov dword ptr [gCar_to_view], esi
    L004fe34d:
            pop edi
            pop esi
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
            nop
            nop
            nop
            nop
    }
#endif
}

typedef int C2_HOOK_FASTCALL tStopCarBeingItModel_cbfn(br_model* pModel);

intptr_t C2_HOOK_CDECL StopCarBeingItActorCallback(br_actor* pActor, tStopCarBeingItModel_cbfn* pCallback);
int C2_HOOK_FASTCALL StopCarBeingItModelCallback(br_model* pModel);
int C2_HOOK_FASTCALL StopCarBeingItMaterialCallback(br_material* pMaterial);

typedef int C2_HOOK_FASTCALL tCarModelColorCycle_cbfn(br_model* pModel);

intptr_t C2_HOOK_CDECL ActorFunks(br_actor* pActor, void* pCallback);
void C2_HOOK_FASTCALL MasterDisableFunkotronic(int pFunk_index);
void C2_HOOK_CDECL CarModelColorCycleRender(br_actor* pActor, br_model* pModel, br_material* pMaterial, void* pRender_data, br_uint_8 pStyle, int pOn_screen);

// FUNCTION: CARMA2_HW 0x004fe9c0
void C2_NAKED C2_HOOK_FASTCALL ForEveryCarVertex(tCar_spec* pCar_spec, tCar_vertex_cbfn* pCallback, int pUnknown) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;
    tCar_crush_spec* crush_spec;
    int group_index;
    int vertex_index;
    int did_callback;
    int crush_did_callback;
    br_model* model;
    br_vertex* vertex;
    tUser_crush_data* user_crush_data;

    crush_spec = pCar_spec->car_crush_spec;
    did_callback = 0;
    model = pCar_spec->shell_model;
    if (model != NULL) {
        for (group_index = 0; group_index < model->prepared->ngroups; group_index++) {
            for (vertex_index = 0; vertex_index < model->prepared->groups[group_index].nvertices; vertex_index++) {
                vertex = &model->vertices[model->prepared->groups[group_index].vertex_user[vertex_index]];
                if (vertex != NULL) {
                    did_callback |= pCallback(vertex);
                }
            }
        }
    }

    did_callback = 0;
    user_crush_data = pCar_spec->car_model_actor->user;
    model = user_crush_data->models[1];
    if (model != NULL) {
        for (group_index = 0; group_index < model->prepared->ngroups; group_index++) {
            for (vertex_index = 0; vertex_index < model->prepared->groups[group_index].nvertices; vertex_index++) {
                vertex = &model->vertices[model->prepared->groups[group_index].vertex_user[vertex_index]];
                if (vertex != NULL) {
                    did_callback |= pCallback(vertex);
                }
            }
        }
    }

    for (i = 0; i < crush_spec->field_0x270; i++) {
        crush_did_callback = 0;
        model = crush_spec->field_0x274[i].field_0x0->model;
        if (model != NULL) {
            for (group_index = 0; group_index < model->prepared->ngroups; group_index++) {
                for (vertex_index = 0; vertex_index < model->prepared->groups[group_index].nvertices; vertex_index++) {
                    vertex = &model->vertices[model->prepared->groups[group_index].vertex_user[vertex_index]];
                    if (vertex != NULL) {
                        crush_did_callback |= pCallback(vertex);
                    }
                }
            }
        }
    }

    for (i = 0; i < crush_spec->field_0x2b0; i++) {
        crush_did_callback = 0;
        model = crush_spec->field_0x2b4[i].field_0x0->model;
        if (model != NULL) {
            for (group_index = 0; group_index < model->prepared->ngroups; group_index++) {
                for (vertex_index = 0; vertex_index < model->prepared->groups[group_index].nvertices; vertex_index++) {
                    vertex = &model->vertices[model->prepared->groups[group_index].vertex_user[vertex_index]];
                    if (vertex != NULL) {
                        crush_did_callback |= pCallback(vertex);
                    }
                }
            }
        }
    }
#else
    __asm {
            sub esp, 0x14
            push ebx
            mov ebx, dword ptr [ecx + 0xe28]
            push ebp
            push esi
            push edi
            mov dword ptr [esp + 0x14], edx
            mov edx, dword ptr [ecx + 0x18d4]
            xor edi, edi
            cmp ebx, edi
            mov dword ptr [esp + 0x20], ecx
            mov dword ptr [esp + 0x18], edx
            mov dword ptr [esp + 0x1c], edi
            je L004fea4a
            mov eax, dword ptr [ebx + 0x4c]
            xor ebp, ebp
            cmp word ptr [eax + 8], di
            jbe L004fea4a
    L004fe9f4:
            mov eax, dword ptr [eax + 0x18]
            xor esi, esi
            cmp word ptr [edi + eax + 0x1e], si
            jbe L004fea31
    L004fea00:
            mov ecx, dword ptr [edi + eax + 0x18]
            xor eax, eax
            mov ax, word ptr [ecx + esi*2]
            lea edx, [eax + eax*4]
            mov eax, dword ptr [ebx + 8]
            lea ecx, [eax + edx*8]
            test ecx, ecx
            je L004fea1f
            call dword ptr [esp + 0x14]
            or dword ptr [esp + 0x1c], eax
    L004fea1f:
            mov ecx, dword ptr [ebx + 0x4c]
            xor edx, edx
            inc esi
            mov eax, dword ptr [ecx + 0x18]
            mov dx, word ptr [edi + eax + 0x1e]
            cmp esi, edx
            jl L004fea00
    L004fea31:
            mov eax, dword ptr [ebx + 0x4c]
            xor ecx, ecx
            inc ebp
            add edi, 0x24
            mov cx, word ptr [eax + 8]
            cmp ebp, ecx
            jl L004fe9f4
            mov ecx, dword ptr [esp + 0x20]
            mov edx, dword ptr [esp + 0x18]
    L004fea4a:
            mov eax, dword ptr [ecx + 0xe0c]
            xor edi, edi
            mov dword ptr [esp + 0x1c], edi
            mov ecx, dword ptr [eax + 0x60]
            mov ebx, dword ptr [ecx + 4]
            cmp ebx, edi
            je L004feabd
            mov eax, dword ptr [ebx + 0x4c]
            xor ebp, ebp
            cmp word ptr [eax + 8], di
            jbe L004feabd
    L004fea6b:
            mov eax, dword ptr [eax + 0x18]
            xor esi, esi
            cmp word ptr [edi + eax + 0x1e], si
            jbe L004feaa8
    L004fea77:
            mov edx, dword ptr [edi + eax + 0x18]
            mov ecx, dword ptr [ebx + 8]
            xor eax, eax
            mov ax, word ptr [edx + esi*2]
            lea eax, [eax + eax*4]
            lea ecx, [ecx + eax*8]
            test ecx, ecx
            je L004fea96
            call dword ptr [esp + 0x14]
            or dword ptr [esp + 0x1c], eax
    L004fea96:
            mov edx, dword ptr [ebx + 0x4c]
            xor ecx, ecx
            inc esi
            mov eax, dword ptr [edx + 0x18]
            mov cx, word ptr [edi + eax + 0x1e]
            cmp esi, ecx
            jl L004fea77
    L004feaa8:
            mov eax, dword ptr [ebx + 0x4c]
            xor edx, edx
            inc ebp
            add edi, 0x24
            mov dx, word ptr [eax + 8]
            cmp ebp, edx
            jl L004fea6b
            mov edx, dword ptr [esp + 0x18]
    L004feabd:
            mov eax, dword ptr [edx + 0x270]
            mov dword ptr [esp + 0x10], 0
            test eax, eax
            jle L004feb6f
            lea eax, [edx + 0x274]
            mov dword ptr [esp + 0x1c], eax
    L004feadd:
            mov ecx, dword ptr [esp + 0x1c]
            xor edi, edi
            mov dword ptr [esp + 0x20], edi
            mov eax, dword ptr [ecx]
            mov ebx, dword ptr [eax + 0x18]
            cmp ebx, edi
            je L004feb4d
            mov eax, dword ptr [ebx + 0x4c]
            xor ebp, ebp
            cmp word ptr [eax + 8], di
            jbe L004feb4d
    L004feafb:
            mov eax, dword ptr [eax + 0x18]
            xor esi, esi
            cmp word ptr [edi + eax + 0x1e], si
            jbe L004feb38
    L004feb07:
            mov ecx, dword ptr [edi + eax + 0x18]
            xor eax, eax
            mov ax, word ptr [ecx + esi*2]
            lea edx, [eax + eax*4]
            mov eax, dword ptr [ebx + 8]
            lea ecx, [eax + edx*8]
            test ecx, ecx
            je L004feb26
            call dword ptr [esp + 0x14]
            or dword ptr [esp + 0x20], eax
    L004feb26:
            mov ecx, dword ptr [ebx + 0x4c]
            xor edx, edx
            inc esi
            mov eax, dword ptr [ecx + 0x18]
            mov dx, word ptr [edi + eax + 0x1e]
            cmp esi, edx
            jl L004feb07
    L004feb38:
            mov eax, dword ptr [ebx + 0x4c]
            xor ecx, ecx
            inc ebp
            add edi, 0x24
            mov cx, word ptr [eax + 8]
            cmp ebp, ecx
            jl L004feafb
            mov edx, dword ptr [esp + 0x18]
    L004feb4d:
            mov eax, dword ptr [esp + 0x10]
            mov esi, dword ptr [esp + 0x1c]
            mov ecx, dword ptr [edx + 0x270]
            inc eax
            add esi, 0x14
            cmp eax, ecx
            mov dword ptr [esp + 0x10], eax
            mov dword ptr [esp + 0x1c], esi
            jl L004feadd
    L004feb6f:
            mov eax, dword ptr [edx + 0x2b0]
            mov dword ptr [esp + 0x10], 0
            test eax, eax
            jle L004fec21
            lea eax, [edx + 0x2b4]
            mov dword ptr [esp + 0x1c], eax
    L004feb8f:
            mov ecx, dword ptr [esp + 0x1c]
            xor edi, edi
            mov dword ptr [esp + 0x20], edi
            mov eax, dword ptr [ecx]
            mov ebx, dword ptr [eax + 0x18]
            cmp ebx, edi
            je L004febff
            mov eax, dword ptr [ebx + 0x4c]
            xor ebp, ebp
            cmp word ptr [eax + 8], di
            jbe L004febff
    L004febad:
            mov eax, dword ptr [eax + 0x18]
            xor esi, esi
            cmp word ptr [edi + eax + 0x1e], si
            jbe L004febea
    L004febb9:
            mov ecx, dword ptr [edi + eax + 0x18]
            xor eax, eax
            mov ax, word ptr [ecx + esi*2]
            lea edx, [eax + eax*4]
            mov eax, dword ptr [ebx + 8]
            lea ecx, [eax + edx*8]
            test ecx, ecx
            je L004febd8
            call dword ptr [esp + 0x14]
            or dword ptr [esp + 0x20], eax
    L004febd8:
            mov ecx, dword ptr [ebx + 0x4c]
            xor edx, edx
            inc esi
            mov eax, dword ptr [ecx + 0x18]
            mov dx, word ptr [edi + eax + 0x1e]
            cmp esi, edx
            jl L004febb9
    L004febea:
            mov eax, dword ptr [ebx + 0x4c]
            xor ecx, ecx
            inc ebp
            add edi, 0x24
            mov cx, word ptr [eax + 8]
            cmp ebp, ecx
            jl L004febad
            mov edx, dword ptr [esp + 0x18]
    L004febff:
            mov eax, dword ptr [esp + 0x10]
            mov esi, dword ptr [esp + 0x1c]
            mov ecx, dword ptr [edx + 0x2b0]
            inc eax
            add esi, 0x14
            cmp eax, ecx
            mov dword ptr [esp + 0x10], eax
            mov dword ptr [esp + 0x1c], esi
            jl L004feb8f
    L004fec21:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x14
            ret 4
            nop
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004fc680
void C2_HOOK_FASTCALL DisposeSplash(void) {
    int i;
    tSplash* splash;

    for (i = 0; i < gNum_splash_types; i++) {
        BrMapRemove(gSplash_material[i]->colour_map);
        BrPixelmapFree(gSplash_material[i]->colour_map);
        BrMaterialRemove(gSplash_material[i]);
        BrMaterialFree(gSplash_material[i]);
    }
    i = 0;
    splash = &gSplash[0];
    do {
        if ((gSplash_flags & (1 << i)) != 0 && splash->actor->parent != NULL) {
            BrActorRemove(splash->actor);
        }
        BrActorFree(splash->actor);
        splash++;
        i++;
    } while ((int)splash < (int)((tU8*)gSplash + sizeof(gSplash)));
    BrModelRemove(gSplash_model);
    BrModelFree(gSplash_model);
}

#pragma auto_inline(on)
