#ifndef GUARD_DEPTH_H
#define GUARD_DEPTH_H

#include <brender/brender.h>


#include "c2_hooks.h"
#include "brender/brender.h"
#include "carpocalypse2_types.h"

// Log2

// CalculateWrappingMultiplier

extern br_pixelmap* gDepth_shade_table;

extern br_scalar C2_HOOK_FASTCALL DepthCueingShiftToDistance(int pDistance);

extern void C2_HOOK_FASTCALL FogAccordingToGPSCDE(br_material* pMaterial);

extern void C2_HOOK_FASTCALL FogCars(void);

// extern void C2_HOOK_FASTCALL FrobFog(void);

extern void C2_HOOK_FASTCALL MungeSkyVs(br_model* pModel, br_material* pMaterial);

extern void C2_HOOK_FASTCALL InstantDepthChange(tDepth_effect_type pType, br_pixelmap* pSky_texture, int pStart, int pEnd, int pRed, int pGreen, int pBlue, int pParam_8);

// Tan

// EdgeU

// MungeSkyModel

// CreateHorizonModel

// LoadDepthTable

extern void C2_HOOK_FASTCALL InitDepthEffects(void);

// DoHorizon

// DoDepthCue

// DoFog

// DepthEffect

// DepthEffectSky

// DoWobbleCamera

// DoDrugWobbleCamera

// DoSubaquaCam

// DoSpecialCameraEffect

// AssertYons

// IncreaseYon

// DecreaseYon

extern void C2_HOOK_STDCALL SetYon(br_scalar pYon);

// GetYon

// GetSkyTextureOn

extern void C2_HOOK_FASTCALL SetSkyTextureOn(int skyTextureOn);

extern void C2_HOOK_FASTCALL ToggleSkyQuietly(void);

// ToggleSky

// GetDepthCueingOn

extern void C2_HOOK_FASTCALL SetDepthCueingOn(int pOn);

extern void C2_HOOK_FASTCALL ToggleDepthCueingQuietly(void);

// ToggleDepthCueing

// ChangeDepthEffect

// MungeForwardSky

// MungeRearviewSky

// SkyTextureChanged

// SetSkyColour

#define CARPOCALYPSE2_DEPTH_H

#include "c2_hooks.h"

#include "carpocalypse2_types.h"
#include <brender/brender.h>


extern int gSky_on;

extern br_pixelmap* gFog_shade_table;

extern br_pixelmap* gAcid_shade_table;

extern tSpecial_volume* gDAT_0079ec44;

extern tDepth_effect_type gSwap_depth_effect_type;

extern br_pixelmap* gSwap_sky_texture;

extern int gSwap_depth_effect_start;

extern int gSwap_depth_effect_end;

extern int gSwap_depth_effect_colour_blue;

extern int gSwap_depth_effect_colour_red;

extern int gSwap_depth_effect_colour_green;

extern float gSky_width;

extern float gSky_height;

extern float gSky_x_multiplier;

extern float gSky_y_multiplier;

extern int gHas_sky_texture;

extern br_material* gHorizon_material;

extern br_actor* gForward_sky_actor;

extern br_model* gForward_sky_model;

extern br_actor* gRearview_sky_actor;

extern br_angle gOld_fov;

extern br_scalar gOld_yon;

extern tSpecial_volume* gLast_camera_special_volume;

intptr_t C2_HOOK_CDECL SwitchCarModel(br_actor* pActor, void* pData);

void C2_HOOK_FASTCALL SwitchCarModels(tCar_spec* pCar, int pIndex);

void C2_HOOK_FASTCALL ProcessModelFaceMaterials2(br_model* pModel, material_cbfn* pCallback);

void C2_HOOK_FASTCALL FrobFog(void);

int C2_HOOK_FASTCALL GetSkyTextureOn(void);

int C2_HOOK_FASTCALL GetDepthCueingOn(void);

void C2_HOOK_FASTCALL ToggleDepthCueing(void);

br_scalar C2_HOOK_STDCALL GetYon(void);

void C2_HOOK_FASTCALL LoadDepthTable(char* pName, br_pixelmap** pTable, int* pPower);

br_scalar C2_HOOK_FASTCALL Tan(br_scalar pAngle);

br_scalar C2_HOOK_FASTCALL EdgeU(br_angle pSky, br_angle pView, br_angle pPerfect);

br_scalar C2_HOOK_FASTCALL CalculateWrappingMultiplier(br_scalar pValue, br_scalar pYon);

void C2_HOOK_FASTCALL MungeSkyModel(br_actor* pCamera, br_model* pModel);

void C2_HOOK_FASTCALL MungeForwardSky(void);

void C2_HOOK_FASTCALL AssertYons(void);

void C2_HOOK_FASTCALL MungeRearviewSky(void);

void C2_HOOK_FASTCALL SkyTextureChanged(void);

void C2_HOOK_FASTCALL SetSkyColour(br_colour c);

void C2_HOOK_FASTCALL ChangeDepthEffect(void);

void C2_HOOK_FASTCALL DoHorizon(br_pixelmap* pRender_buffer, br_pixelmap* pDepth_buffer, br_actor* pCamera, br_matrix34* pCamera_to_world);

void C2_HOOK_FASTCALL DepthEffectSky(br_pixelmap* pRender_buffer, br_pixelmap* pDepth_buffer, br_actor* pCamera, br_matrix34* pCamera_to_world);

void C2_HOOK_FASTCALL DoWobbleCamera(br_actor* pCamera);

void C2_HOOK_FASTCALL DoDrugWobbleCamera(br_actor* pCamera);

int C2_HOOK_FASTCALL DoSubAquaCam(void);

void C2_HOOK_FASTCALL DoSpecialCameraEffect(br_actor* pCamera, br_matrix34* pCamera_to_world);

#endif // GUARD_DEPTH_H
