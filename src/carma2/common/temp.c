#include "temp.h"

#include "depth.h"
#include "errors.h"
#include "graphics.h"

#include "carpocalypse2_macros.h"
#include "carpocalypse2_types.h"
// GLOBAL: CARMA2_HW 0x006a6d38
extern int gMaterials_to_adapt_count;
// GLOBAL: CARMA2_HW 0x006a3340
br_material* gMaterials_to_adapt[200];

// GLOBAL: CARMA2_HW 0x006a8298
extern int gAllow_material_adapt;
#ifndef CARPOCALYPSE2_MATCHING
extern tBrender_storage gTrack_storage_space;
extern tBrender_storage gMisc_storage_space;
#endif
br_material* C2_HOOK_FASTCALL LoadTemporaryMaterial(const char* pName) {
    br_material* material;

    material  = BrMaterialFind(pName);
    if (material == NULL) {
        FatalError(kFatalError_CannotFindSmashMaterial_S, pName);
    }
    if (gMaterials_to_adapt_count < CARPOCALYPSE2_ASIZE(gMaterials_to_adapt)) {
        gMaterials_to_adapt[gMaterials_to_adapt_count] = material;
        gMaterials_to_adapt_count++;
    }
    return material;
}

// FUNCTION: CARMA2_HW 0x004ea820
void C2_HOOK_FASTCALL TemporaryMaterialStore(br_material* pMaterial) {

    if (gMaterials_to_adapt_count < (int)CARPOCALYPSE2_ASIZE(gMaterials_to_adapt)) {
        gMaterials_to_adapt[gMaterials_to_adapt_count] = pMaterial;
        gMaterials_to_adapt_count += 1;
    }
}

// FUNCTION: CARMA2_HW 0x004ea840
void C2_HOOK_FASTCALL EnableAutoFogification(void) {
    gAllow_material_adapt = 1;
}

// FUNCTION: CARMA2_HW 0x004ea7c0
void C2_HOOK_FASTCALL AutoFogificateMaterials(tRendererShadingType pShading_type) {
    int i;

    if (gAllow_material_adapt) {
        for (i = 0; i < gMaterials_to_adapt_count; i++) {
            if (gMaterials_to_adapt[i] != NULL) {
                GlorifyMaterial(&gMaterials_to_adapt[i], 1, pShading_type);
                FogAccordingToGPSCDE(gMaterials_to_adapt[i]);
            }
        }
        gMaterials_to_adapt_count = 0;
    }
}

// FUNCTION: CARMA2_HW 0x004ea770
void C2_HOOK_FASTCALL FogificateMaterials(tRendererShadingType pShading_type) {
    int i;

    for (i = 0; i < gMaterials_to_adapt_count; i++) {
        if (gMaterials_to_adapt[i] != NULL) {
#ifndef CARPOCALYPSE2_MATCHING
            if (gMaterials_to_adapt[i] == (br_material*)&gTrack_storage_space || gMaterials_to_adapt[i] == (br_material*)&gMisc_storage_space) {
                continue;
            }
#endif
            GlorifyMaterial(&gMaterials_to_adapt[i], 1, pShading_type);
            FogAccordingToGPSCDE(gMaterials_to_adapt[i]);
        }
    }
    gMaterials_to_adapt_count = 0;
}