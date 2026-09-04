#include "fog.h"

// GLOBAL: CARMA2_HW 0x006a3340
extern br_material* gMaterials_to_adapt[200];

// GLOBAL: CARMA2_HW 0x006a6d38
int gMaterials_to_adapt_count;

// GLOBAL: CARMA2_HW 0x006a8298
int gAllow_material_adapt;

// FUNCTION: CARMA2_HW 0x004ea760
void C2_HOOK_FASTCALL InitFogificateMaterials(void) {

    gMaterials_to_adapt_count = 0;
}

// FogificateMaterials

// AutoFogificateMaterials

// AddFogificateMaterial

// EnableAutoFogification

// FUNCTION: CARMA2_HW 0x004ea850
void C2_HOOK_FASTCALL DisableAutoFogification(void) {
    gAllow_material_adapt = 0;
}

// FUNCTION: CARMA2_HW 0x004ea860
void C2_HOOK_FASTCALL RemoveMaterialFromFogification(br_material* pMaterial) {
    int i;

    for (i = 0; i < gMaterials_to_adapt_count; i++) {
        if (gMaterials_to_adapt[i] == pMaterial) {
            gMaterials_to_adapt[i] = NULL;
        }
    }
}