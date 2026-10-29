#ifndef GUARD_SPARK_H
#define GUARD_SPARK_H

#include "brender/brender.h"
#include "c2_stdio.h"


#include "c2_hooks.h"
#include "carpocalypse2_types.h"

// DrawDot

// SetWorldToScreen

// DrawLine3DThroughBRender

// DrawLine3D

// DrawLine2D

// SetLineModelCols

// ReplaySparks

// RenderSparks

// CreateSingleSpark

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

// LoadInShrapnel

// KillShrapnel

// DisposeShrapnel

// ReplayShrapnel

// ActorFunks

extern void C2_HOOK_FASTCALL MasterDisableCarFunks(tCar_spec* pCar);

void C2_HOOK_FASTCALL MasterEnableCarFunks(tCar_spec* pCar);

extern int C2_HOOK_FASTCALL SetPixelmap(br_material* pMaterial);

extern void C2_HOOK_FASTCALL DoCamouflageThing(tCar_spec* pCar);

// DoSolidGraniteThing

// DoTrueColModelThing

// SetFoxModelCallback

extern int C2_HOOK_FASTCALL ResetFoxModelCallback(br_model* pModel);

extern int C2_HOOK_FASTCALL ResetFoxLighting(br_material* pMaterial);

// CancelAlternateFoxEffect

// SetFoxLighting

// SetupAlternateFoxEffect

// RestorePixelmap

void C2_HOOK_FASTCALL RestoreCarPixelmaps(tCar_spec* pCar_spec);

// MungeShrapnel

// DrMatrix34Rotate

// CmpSmokeZ

extern void C2_HOOK_FASTCALL InitSmokeStuff(void);

// RenderRecordedSmokeCircles

// RecordSmokeCircle

// SmokeCircle3D

// ReplaySmoke

// GenerateContinuousSmoke

// RenderSmoke

// CreatePuffOfSmoke

// ResetSmoke

extern void C2_HOOK_FASTCALL AdjustSmoke(int pIndex, tU8 pType, br_vector3* pPos, br_scalar pRadius, br_scalar pStrength);

// ActorError

extern void C2_HOOK_FASTCALL AdjustSmokeColumn(int pIndex, const tSmoke_column_core* pCore, int pVertex, int pColour);

// CreateSmokeColumn2

void C2_HOOK_FASTCALL CreateSmokeColumn(tCar_spec* pCar, int pType, int pRandom, int pLifetime);

// GenerateSmokeShades

extern void C2_HOOK_FASTCALL GenerateItFoxShadeTable(void);

extern void C2_HOOK_FASTCALL AdjustFlame(int pIndex, int pFrame, br_scalar pScale_x, br_scalar pScale_y, br_scalar pOffset_x, br_scalar pOffset_z);

// ReplayFlame

// FlameAnimate

// DoSmokeColumn

// ReplaySmokeColumn

// MungeSmokeColumn

extern void C2_HOOK_FASTCALL DisposeFlame(void);

// InitFlame

// InitSplash

// DisposeSplash
extern void C2_HOOK_FASTCALL DisposeSplash(void);

// DrawTheGlow

// PipeInstantUnSmudge

// SmudgeCar

void C2_HOOK_FASTCALL SetSmokeOn(int pSmoke_on);

extern int C2_HOOK_FASTCALL GetSmokeOn(void);

// StopCarSmoking

// StopCarSmokingInstantly

// StopObjectSmokingInstantly

void C2_HOOK_FASTCALL ConditionalSmokeColumn(tCar_spec* pCar, int fire_point, int level);

// SingleSplash

// GeneralCreateSplash

// CreateSplash

// GetVelocitiesFromMatrices

// MungeSplash

// GetSmokeShadeTables

// FreeSmokeShadeTables

// LoadInKevStuff

// DisposeKevStuff

void C2_HOOK_FASTCALL DisposeKevStuffCar(tCar_spec* pCar);

// MakeCarIt

// StopCarBeingIt

// ForEveryModelMaterial

// ForEveryActorMaterial

// ForEveryActorMaterialNoGrooves

// ForEveryCarMaterial

extern void C2_HOOK_FASTCALL RemoveCarFromEffects(tCar_spec* pCar);

extern void C2_HOOK_FASTCALL ForEveryCarVertex(tCar_spec* pCar_spec, tCar_vertex_cbfn* pCallback, int pUnknown);

// ForEveryCarModelCB

// ForEveryCarModel

// MightBlendifyMaterial

// BlendifyMaterialCB

extern void C2_HOOK_FASTCALL BlendifyCar(tCar_spec* pCar);

// UnBlendifyMaterialCB

void C2_HOOK_FASTCALL UnBlendifyCar(tCar_spec* pCar_spec);

int C2_HOOK_FASTCALL IsCarSmoking(tCar_spec* pCar);

// ClearSplashReplay

void C2_HOOK_FASTCALL ClearSplashReplay(uintptr_t pObject);

extern void* C2_HOOK_FASTCALL GetSplashIndex(tPhysics_object* pObject);

// ClearSplashes

// AdjustSplashReplay

// InitReplaySplashes
extern void C2_HOOK_FASTCALL InitReplaySplashes(void);

#define CARPOCALYPSE2_SPARK_H

#include "carpocalypse2_types.h"
#include "brender/brender.h"

#include "c2_stdio.h"

#include "c2_hooks.h"


extern int gSmoke_on;

extern int gShade_list[16];

extern int* gShade_table; /* FIXME: rename to gDust_table*/

extern int gNum_dust_tables;

extern br_model* gShrapnel_model[2];

extern br_material* gBlack_material;

extern tShrapnel gShrapnel[30];

extern int gColumn_flags;

extern br_model* gLollipop_model;

extern br_pixelmap* gFlame_map[20];

extern tSmoke_column gSmoke_column[10];

extern int gSplash_flags;

extern br_model* gSplash_model;

extern int gNum_splash_types;

extern br_material* gSplash_material[20];

extern tSplash gSplash[32];

extern int gSpark_flags;

extern br_pixelmap* gIt_shade_table;

extern int gDust_rotate;

extern tReplay_splash gReplay_splashes[10];

extern int gReplay_splash_flags;

extern tU32 gNext_AFE_color_cycle;

extern int gAFE_color_cycle;

extern int gShrapnel_flags;

extern br_camera* gSpark_cam;

extern br_matrix4 gCameraToScreen;

extern tSpark gSparks[32];

extern tBRender_smoke* gBR_smoke_pointers[35];

extern int gN_BR_smoke_structs;

extern tSmoke gSmoke[25];

extern int gSmoke_flags;

extern int gN_smoke;

void C2_HOOK_FASTCALL GenerateSmokeShades(void);

void C2_HOOK_FASTCALL GetSmokeShadeTables(FILE* f);

void C2_HOOK_FASTCALL ModelScale(br_model* pModel, float pScale);

float C2_HOOK_FASTCALL DistanceFromFaceND(const br_vector3* pP, const br_vector3* pN, br_scalar pF);

void C2_HOOK_FASTCALL ResetSparks(void);

void C2_HOOK_FASTCALL LoadInShrapnel(void);

void C2_HOOK_FASTCALL InitShrapnel(void);

void C2_HOOK_FASTCALL InitFlame(void);

void C2_HOOK_FASTCALL FreeSmokeShadeTables(void);

void C2_HOOK_FASTCALL LoadInKevStuff(FILE* pF);

void C2_HOOK_FASTCALL DisposeKevStuff(void);

void C2_HOOK_FASTCALL StopCarBeingIt(tCar_spec* pCar);

void C2_HOOK_FASTCALL MungeSplash(tU32 pTime);

void C2_HOOK_FASTCALL ReplayShrapnel(tU32 pTime);

void C2_HOOK_FASTCALL KillShrapnel(int i);

void C2_HOOK_FASTCALL MungeShrapnel(tU32 pTime);

intptr_t C2_HOOK_CDECL ForEveryActorMaterial(br_actor* pActor, void* pContext);

intptr_t C2_HOOK_CDECL ForEveryActorMaterialNoGrooves(br_actor* pActor, void* pContext);

void C2_HOOK_FASTCALL ForEveryCarMaterial(tCar_spec* pCar_spec, tMaterialMaybeUpdate_cbfn* pCallback, int pGrooves);

void C2_HOOK_CDECL AFEModelUpdate(br_actor* pActor, br_model* pModel, br_material* pMaterial, void* pOrder_table, br_uint_8 pStyle, int pOn_screen);

int C2_HOOK_FASTCALL SetModelAFEUpdateCB(br_model* pModel);

int C2_HOOK_FASTCALL SetMaterialPrelitCB(br_material* pMaterial);

intptr_t C2_HOOK_CDECL ForEveryActorModelNoGrooves(br_actor* pActor, void* pContext);

void C2_HOOK_FASTCALL DisposeFlames(void);

intptr_t C2_HOOK_FASTCALL UnBlendifyMaterialCB(br_material* pMaterial);

intptr_t C2_HOOK_FASTCALL BlendifyMaterialCB(br_material* pMaterial);

void C2_HOOK_FASTCALL StopCarSmokingInstantly(tCar_spec* pCar_spec);

void C2_HOOK_FASTCALL PipeInstantUnSmudge(tCar_spec* pCar_spec);

void C2_HOOK_FASTCALL StopCarSmoking(tCar_spec* pCar);

void C2_HOOK_FASTCALL StopObjectSmokingInstantly(tPhysics_object* pObject);

void C2_HOOK_FASTCALL GenerateContinuousSmoke(tCar_spec* pCar, int pWheel, tU32 pTime);

void C2_HOOK_FASTCALL CreatePuffOfSmoke(br_vector3* pPos, br_vector3* pV, float pStrength, float pDecay_factor, int pType);

void C2_HOOK_FASTCALL ClearSplashes(tPhysics_object* pObject);

void C2_HOOK_FASTCALL CreateSmokeColumn2(int pType, br_actor* pActor, tCar_spec* pCar, tPhysics_object* pObject, int pColour, int pVertex, tU32 pLifetime);

void C2_HOOK_FASTCALL ReplaySmokeColumn(tU32 pTime);

void C2_HOOK_FASTCALL MungeSmokeColumn(tU32 pTime);

void C2_HOOK_FASTCALL GeneralCreateSplash(tPhysics_object* pObject, const br_vector3* pVelocity, const br_vector3* pOmega, const br_vector3* pNormal, br_scalar pWater_distance, const br_bounds3* pBounds, const br_matrix34* pMatrix, tU32 pTime);

void C2_HOOK_FASTCALL CreateSplash(tPhysics_object* pObject, tU32 pTime);

void C2_HOOK_FASTCALL GetVelocitiesFromMatrices(br_vector3* pVel, br_vector3* pRot, br_matrix34* pMat_now, br_matrix34* pMat_prev, tU32 pTime);

void C2_HOOK_FASTCALL DrMatrix34Rotate(br_matrix34* mat, br_angle r, br_vector3* a);

void C2_HOOK_FASTCALL DoSmokeColumn(int i, tU32 pTime, br_vector3* pRet_car_pos);

void C2_HOOK_FASTCALL FlameAnimate(int pColumn, const br_vector3* pPos, tU32 pTime);

int C2_HOOK_FASTCALL IsOnNonCarHalf(tCar_spec* pCar, br_vector3 *pPos);

void C2_HOOK_FASTCALL SmudgeCar(tCar_spec* pCar, int fire_point);

void C2_HOOK_FASTCALL SetLineModelCols(tU8 pCol);

int C2_HOOK_FASTCALL DrawLine3D(br_vector3* start, br_vector3* end, br_pixelmap* pScreen, br_pixelmap* pDepth_buffer, br_pixelmap* shade_table);

void C2_HOOK_FASTCALL SetWorldToScreen(br_pixelmap* pScreen);

void C2_HOOK_FASTCALL ReplaySparks(br_pixelmap* pRender_screen, br_pixelmap* pDepth_buffer, br_actor* pCamera, tU32 pTime);

void C2_HOOK_FASTCALL RenderSparks(br_pixelmap* pRender_screen, br_pixelmap* pDepth_buffer, br_actor* pCamera, br_matrix34* pCamera_to_world, tU32 pTime);

int C2_HOOK_CDECL CmpSmokeZ(const void* p1, const void* p2);

void C2_HOOK_FASTCALL RenderRecordedSmokeCircles(void);

void C2_HOOK_FASTCALL RecordSmokeCircle(br_vector3* pCent, br_scalar pR, br_scalar pStrength, br_colour pShade, br_scalar pAspect);

void C2_HOOK_FASTCALL SmokeCircle3D(br_vector3* o, br_scalar r, br_scalar strength, br_scalar pAspect, br_pixelmap* pRender_screen, br_pixelmap* pDepth_buffer, br_colour pShade, br_actor* pCamera);

void C2_HOOK_FASTCALL DrawTheGlow(br_pixelmap* pRender_screen, br_pixelmap* pDepth_buffer, br_actor* pCamera);

void C2_HOOK_FASTCALL ReplaySmoke(br_pixelmap* pRender_screen, br_pixelmap* pDepth_buffer, br_actor* pCamera);

void C2_HOOK_FASTCALL RenderSmoke(br_pixelmap* pRender_screen, br_pixelmap* pDepth_buffer, br_actor* pCamera, br_matrix34* pCamera_to_world, tU32 pTime);

#endif // GUARD_SPARK_H
