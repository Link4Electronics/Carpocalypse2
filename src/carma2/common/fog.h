#ifndef GUARD_FOG_H
#define GUARD_FOG_H

#include "c2_hooks.h"
#include <brender/brender.h>

extern void C2_HOOK_FASTCALL InitFogificateMaterials(void);

// FogificateMaterials

// AutoFogificateMaterials

// AddFogificateMaterial

// EnableAutoFogification

extern void C2_HOOK_FASTCALL DisableAutoFogification(void);

extern void C2_HOOK_FASTCALL RemoveMaterialFromFogification(br_material* pMaterial);

#endif // GUARD_FOG_H
