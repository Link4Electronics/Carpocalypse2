#ifndef GUARD_WORLD2_H
#define GUARD_WORLD2_H

#include "c2_hooks.h"
#include "carpocalypse2_types.h"

extern tRoad_texturing_level gRoad_texturing_level;

extern tWall_texturing_level gWall_texturing_level;

extern void C2_HOOK_FASTCALL InitialiseExtraRenders(void);

extern void C2_HOOK_FASTCALL AddExtraRender(br_actor* pActor, br_material* pMaterial);

extern int C2_HOOK_FASTCALL PointOutOfSight(const br_vector3* pPoint, undefined4 pArg2, br_scalar pMax_distance);

extern void C2_HOOK_FASTCALL CancelificateClipulatingPlaneyThings(void);

extern void C2_HOOK_FASTCALL FixificateClipulatingPlaneyThings(void);

extern int C2_HOOK_FASTCALL FUN_004e5f10(int pRed, tU32 pGreen, tU32 pBlue);

extern void C2_HOOK_FASTCALL ProcessModelFaceMaterials(br_model* pModel, tPMFMCB* pCallback);

extern void C2_HOOK_FASTCALL ProcessModelFaceMaterials2(br_model* pModel, material_cbfn* pCallback);

extern intptr_t C2_HOOK_CDECL ProcessFaceMaterials(br_actor* pActor, void* pData);

extern int C2_HOOK_FASTCALL DRPixelmapHasZeros(br_pixelmap* pm);

extern int C2_HOOK_FASTCALL StorageContainsPixelmap(tBrender_storage* pStorage, br_pixelmap* pMap);

extern void C2_HOOK_FASTCALL HideStoredOpaqueTextures(tBrender_storage* pStorage);

extern void C2_HOOK_FASTCALL RevealStoredTransparentTextures(tBrender_storage* pStorage);

extern void C2_HOOK_FASTCALL HideStoredTextures(tBrender_storage* pStorage);

extern void C2_HOOK_FASTCALL RevealStoredTextures(tBrender_storage* pStorage);

extern void C2_HOOK_FASTCALL SetCarStorageTexturingLevel(tBrender_storage* pStorage, tCar_texturing_level pNew, tCar_texturing_level pOld);

extern tCar_texturing_level C2_HOOK_FASTCALL GetCarTexturingLevel(void);

extern void C2_HOOK_FASTCALL SetCarTexturingLevel(tCar_texturing_level pLevel);

extern int C2_HOOK_FASTCALL HasThisSuffix(const char* pIdent, const char* pSuffix);

extern br_material* C2_HOOK_FASTCALL UnsuffixedMaterial(const char* pOld_ident, const char* pSuffix);

extern br_material* C2_HOOK_FASTCALL WallLinearToUntex(br_model* pModel, tU16 pFace);

extern int C2_HOOK_FASTCALL FUN_00447ef0(br_model* pModel, tU16 pIndex);

extern br_material* C2_HOOK_FASTCALL FUN_00448480(void* pArg1, tU16 pIndex);

extern br_material* C2_HOOK_FASTCALL RoadUntexToPersp(br_model* pModel, tU16 pFace);

extern br_material* C2_HOOK_FASTCALL WallPerspToLinear(br_model* pModel, tU16 pFace);

extern br_material* C2_HOOK_FASTCALL WallUntexToPersp(br_model* pModel, tU16 pFace);

extern br_material* C2_HOOK_FASTCALL WallPerspToUntex(br_model* pModel, tU16 pFace);

extern br_material* C2_HOOK_FASTCALL WallUntexToLinear(br_model* pModel, tU16 pFace);

extern br_material* C2_HOOK_FASTCALL WallLinearToPersp(br_model* pModel, tU16 pFace);

extern tRoad_texturing_level C2_HOOK_FASTCALL GetRoadTexturingLevel(void);

extern void C2_HOOK_FASTCALL SetRoadTexturingLevel(tRoad_texturing_level pLevel);

extern void C2_HOOK_FASTCALL FUN_00448790(tRoad_texturing_level pLevel);

// GetWallTexturingLevel

extern void C2_HOOK_FASTCALL SetWallTexturingLevel(tWall_texturing_level pLevel);

extern void C2_HOOK_FASTCALL ReallySetWallTexturingLevel(tWall_texturing_level pLevel);

extern br_material* C2_HOOK_FASTCALL DisposeSuffixedMaterials(br_model* pModel, tU16 pFace);

// DisposeTexturingMaterials

extern intptr_t C2_HOOK_CDECL SetAccessoryRenderingCB(br_actor* pActor, void* pFlag);

extern void C2_HOOK_FASTCALL SetAccessoryRendering(int pOn);

extern int C2_HOOK_FASTCALL GetAccessoryRendering(void);

extern void C2_HOOK_FASTCALL SetCarSimplificationLevel(int pLevel);

extern void C2_HOOK_FASTCALL ToggleTrackRenderStyle(void);

// GetCarSimplificationLevel

extern intptr_t C2_HOOK_CDECL ProcessMaterials(br_actor* pActor, void* pContext);

extern void C2_HOOK_FASTCALL BuildColourTable(br_pixelmap* pPalette);

#endif // GUARD_WORLD2_H
