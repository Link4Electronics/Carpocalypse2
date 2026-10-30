#ifndef GUARD_LIGHTING_H
#define GUARD_LIGHTING_H

#include "c2_hooks.h"

#include "carpocalypse2_types.h"
#include <brender/brender.h>

// LoadTrackModels

// SmoothlyLoadIfItsAModel

// AddSmoothModels

// LoadTrackMaterials

// LoadCarMaterials

// SmoothlySetCarMaterialFlags

// SmoothlySetWorldMaterialFlags

// SmoothificatePowerups

// SmoothificatePowerupMaterial

void C2_HOOK_FASTCALL SmoothificatePowerupMaterial(br_material* pMaterial);

// SmoothificateWorldMaterial

void C2_HOOK_FASTCALL SmoothificateCarMaterial(br_material* pMaterial);

// MungeLightingEffects

extern void C2_HOOK_FASTCALL KillLightingEffects(void);

extern void C2_HOOK_FASTCALL InitLightingEffects(void);

// LoadStaticLightingForRace

// ReadGlobalLightingInfo

#endif // GUARD_LIGHTING_H
