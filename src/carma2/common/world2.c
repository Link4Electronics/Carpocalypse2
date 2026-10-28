#include "world2.h"

#include "utility.h"
#include "globvars.h"
#include "brucetrk.h"
#include "input.h"
#include "carpocalypse2_macros.h"

#include "c2_string.h"

extern br_pixelmap* gRender_palette;

// GLOBAL: CARMA2_HW 0x00591374
tCar_texturing_level gCar_texturing_level = eCTL_full;

// GLOBAL: CARMA2_HW 0x0059136c
tRoad_texturing_level gRoad_texturing_level = eRTL_full;

// GLOBAL: CARMA2_HW 0x00591370
tWall_texturing_level gWall_texturing_level = eWTL_full;

// GLOBAL: CARMA2_HW 0x005913ac
char gMaterial_suffix_lwall[] = ".lwall";

// GLOBAL: CARMA2_HW 0x005913b4
char gMaterial_suffix_pwall[] = ".pwall";

// GLOBAL: CARMA2_HW 0x00591378
static tPMFMCB* gWall_texturing_tweakers[3][3] = {
    { NULL, WallUntexToLinear, WallUntexToPersp },
    { WallLinearToUntex, NULL, WallLinearToPersp },
    { WallPerspToUntex, WallPerspToLinear, NULL },
};

// GLOBAL: CARMA2_HW 0x00589d20
const br_scalar gFLOAT_00589d20 = 0.7f;

// GLOBAL: CARMA2_HW 0x006793d8
int gCar_simplification_level;

// GLOBAL: CARMA2_HW 0x00591368
int gRendering_accessories = 1;

// GLOBAL: CARMA2_HW 0x006a22c0
int gCount_extra_renders;

// GLOBAL: CARMA2_HW 0x006a22c8
tExtra_render gExtra_renders[6];

// GLOBAL: CARMA2_HW 0x00704e40
int gMirror_on__graphics;

// GLOBAL: CARMA2_HW 0x0065cf30
br_colour gRGB_colours[9] = {
    0x000000,   0xffffff,   0xff0000,   0x00ff00,
    0x0000ff,   0xffff00,   0x00ffff,   0xff00ff,
    0xd04702,
};

// GLOBAL: CARMA2_HW 0x0074a620
br_colour gColours[9];

// FUNCTION: CARMA2_HW 0x004e5cb0
void C2_HOOK_FASTCALL InitialiseExtraRenders(void) {

    gCount_extra_renders = 0;
}

// FUNCTION: CARMA2_HW 0x004e5cc0
void C2_HOOK_FASTCALL AddExtraRender(br_actor* pActor, br_material* pMaterial) {

    gExtra_renders[gCount_extra_renders].actor = pActor;
    gExtra_renders[gCount_extra_renders].material = pMaterial;
    gCount_extra_renders += 1;
}

// FUNCTION: CARMA2_HW 0x004e5ce0
int C2_HOOK_FASTCALL PointOutOfSight(const br_vector3* pPoint, undefined4 pArg2, br_scalar pMax_distance) {
    br_vector3 distance_vector;
    int i;

#define CAMERA_MAX_DISTANCE(A) ((pMax_distance != 0.f) ? pMax_distance : CARPOCALYPSE2_SQR(((br_camera*)(A)->type_data)->yon_z))

    if (gMirror_on__graphics) {
        BrVector3Sub(&distance_vector, pPoint, (br_vector3*)gRearview_camera_to_world.m[3]);
        if (BrVector3LengthSquared(&distance_vector) < CAMERA_MAX_DISTANCE(gRearview_camera)
                && BrVector3Dot(&distance_vector, (br_vector3*)gRearview_camera_to_world.m[2]) < 0.f) {

            return 0;
        }
    }

    for (i = 0; i < gCount_extra_renders; i++) {
        br_actor* a = gExtra_renders[i].actor;

        BrVector3Sub(&distance_vector, pPoint, &a->t.t.translate.t);
        if (BrVector3LengthSquared(&distance_vector) < CAMERA_MAX_DISTANCE(a)) {

            return 0;
        }
    }

    BrVector3Sub(&distance_vector, pPoint, (br_vector3*)gCamera_to_world.m[3]);
    if (BrVector3LengthSquared(&distance_vector) >= CAMERA_MAX_DISTANCE(gCamera)) {
        return 1;
    }
    if (BrVector3Dot(&distance_vector, (br_vector3*)gCamera_to_world.m[2]) >= 0.f) {

        return 1;
    }
#undef CAMERA_MAX_DISTANCE
    return 0;
}

// FUNCTION: CARMA2_HW 0x004e5ef0
void C2_HOOK_FASTCALL CancelificateClipulatingPlaneyThings(void) {

    // empty
}

// FUNCTION: CARMA2_HW 0x004e5f00
void C2_HOOK_FASTCALL FixificateClipulatingPlaneyThings(void) {

    // empty
}

// C2PORT-NONMATCHING: best 46.81% | difftest PASS 2000 trials (spec difftest/specs/004e5f10_FUN_004e5f10.json) | reason register-allocation
// FUNCTION: CARMA2_HW 0x004e5f10
int C2_HOOK_FASTCALL FUN_004e5f10(int pRed, tU32 pGreen, tU32 pBlue) {
    register tU32 colour;
    register int blue;
    register int red;
    register int green;
    register int blue_delta;
    register int red_delta;
    register int green_delta;
    register int best_index;
    register tU32 best_distance;
    register tU32 distance;
    register tU32 palette_colour;
    tU32* volatile pPalette;

    colour = ((pRed << 8 | pGreen) << 8) | pBlue;
    blue = pBlue & 0xff;
    pPalette = (tU32*)gRender_palette->pixels + 1;
    palette_colour = *pPalette;
    red = (colour >> 16) & 0xff;
    green = (colour >> 8) & 0xff;
    blue_delta = blue - (palette_colour & 0xff);
    red_delta = red - ((palette_colour >> 16) & 0xff);
    green_delta = green - ((palette_colour >> 8) & 0xff);
    best_distance = blue_delta * blue_delta + red_delta * red_delta + green_delta * green_delta;
    best_index = 1;
    pBlue = 2;
    pPalette++;
    do {
        palette_colour = *pPalette;
        blue_delta = blue - (palette_colour & 0xff);
        red_delta = red - ((palette_colour >> 16) & 0xff);
        green_delta = green - ((palette_colour >> 8) & 0xff);
        distance = blue_delta * blue_delta + red_delta * red_delta + green_delta * green_delta;
        if (distance < best_distance) {
            best_index = pBlue;
            best_distance = distance;
        }
        pBlue++;
        pPalette++;
    } while ((int)pBlue < 256);
    return best_index;
}

void C2_HOOK_FASTCALL ProcessModelFaceMaterials(br_model* pModel, tPMFMCB* pCallback) {
    tU16 f;
    br_material* possible_mat;
    br_material* new_mat = NULL;

    for (f = 0; f < pModel->nfaces; f++) {

        if (pModel->faces[f].material == NULL) {
            continue;
        }
        possible_mat = pCallback(pModel, f);
        if (possible_mat == NULL) {
            continue;
        }
        pModel->faces[f].material = possible_mat;
        new_mat = possible_mat;
    }
    if (new_mat != NULL) {
        BrModelUpdate(pModel, BR_MODU_ALL);
    }
}

// FUNCTION: CARMA2_HW 0x00448fd0
void C2_HOOK_FASTCALL ProcessModelFaceMaterials2(br_model* pModel, material_cbfn* pCallback) {
    tU16 i;
    tU16 j;

    if (pModel->faces == NULL) {
        if (pModel->prepared != NULL) {
            for (i = 0; i < pModel->prepared->ngroups; i++) {

                /* FIXME: can this inner loop be removed? */
                for (j = 0; j < pModel->prepared->groups[i].nfaces; j++) {
                    if (*pModel->prepared->groups[i].face_colours.materials != NULL) {
                        pCallback(*pModel->prepared->groups[i].face_colours.materials);
                    }
                }
            }
        }
    } else if (pModel->faces != NULL) {
        for (i = 0; i < pModel->nfaces; i++) {
            if (pModel->faces[i].material != NULL) {
                pCallback(pModel->faces[i].material);
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x00449070
void C2_HOOK_FASTCALL ShowTimerString(char* pStr, tU32 pArg3) {
    TimerString(gTimer, pStr, pArg3, 0, 0);
}

// FUNCTION: CARMA2_HW 0x00448850
intptr_t C2_HOOK_CDECL ProcessFaceMaterials(br_actor* pActor, void* pData) {
    tPMFMCB* callback = pData;

    if (pActor->identifier != NULL && pActor->identifier[0] == '&') {
        return 0;
    }
    if (pActor->type == BR_ACTOR_MODEL && pActor->model != NULL) {
        ProcessModelFaceMaterials(pActor->model, callback);
    }
    return BrActorEnum(pActor, ProcessFaceMaterials, pData);
}

// FUNCTION: CARMA2_HW 0x004475c0
int C2_HOOK_FASTCALL DRPixelmapHasZeros(br_pixelmap* pm) {
    int x;
    int y;
    tU8* row_ptr;
    tU8* pp;

    if (pm->flags & BR_PMF_NO_ACCESS) {
        return 1;
    }
    row_ptr = (tU8*)pm->pixels + pm->base_y * pm->row_bytes;
    row_ptr += pm->base_x;
    for (y = 0; y < pm->height; y++) {
        pp = row_ptr;
        for (x = 0; x < pm->width; x++, pp++) {
            if (*pp == '\0') {
                return 1;
            }
        }
        row_ptr += pm->row_bytes;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004479c0
int C2_HOOK_FASTCALL StorageContainsPixelmap(tBrender_storage* pStorage, br_pixelmap* pMap) {
    int i;

    for (i = 0; i < pStorage->pixelmaps_count; i++) {

        if (pStorage->pixelmaps[i] == pMap) {
            break;
        }
    }
    return i != pStorage->pixelmaps_count;
}

// FUNCTION: CARMA2_HW 0x004479f0
void C2_HOOK_FASTCALL HideStoredOpaqueTextures(tBrender_storage* pStorage) {
    int i;

    for (i = 0; i < pStorage->materials_count; i++) {
        br_pixelmap* colour_map;

        colour_map = pStorage->materials[i]->colour_map;

        if (colour_map != NULL && StorageContainsPixelmap(pStorage, colour_map) && !DRPixelmapHasZeros(colour_map)) {
            pStorage->materialProps[i] = colour_map;
            pStorage->materials[i]->colour_map = NULL;
            pStorage->materials[i]->flags &= ~BR_MATF_PRELIT;
            BrMaterialUpdate(pStorage->materials[i], BR_MATU_ALL);
        }
    }
}

// FUNCTION: CARMA2_HW 0x00447b00
void C2_HOOK_FASTCALL RevealStoredTransparentTextures(tBrender_storage* pStorage) {
    int i;

    for (i = 0; i < pStorage->materials_count; i++) {

        if (pStorage->materialProps[i] != NULL && DRPixelmapHasZeros(pStorage->materialProps[i])) {
            pStorage->materials[i]->colour_map = pStorage->materialProps[i];
            pStorage->materialProps[i] = NULL;
            pStorage->materials[i]->flags |= BR_MATF_PRELIT;
            BrMaterialUpdate(pStorage->materials[i], BR_MATU_ALL);
        }
    }
}

// FUNCTION: CARMA2_HW 0x00447be0
void C2_HOOK_FASTCALL HideStoredTextures(tBrender_storage* pStorage) {
    int i;

    for (i = 0; i < pStorage->materials_count; i++) {
        br_pixelmap* colour_map;

        colour_map = pStorage->materials[i]->colour_map;

        if (colour_map != NULL && StorageContainsPixelmap(pStorage, colour_map)) {
            pStorage->materialProps[i] = colour_map;
            pStorage->materials[i]->colour_map = NULL;
            pStorage->materials[i]->flags &= ~BR_MATF_PRELIT;
            BrMaterialUpdate(pStorage->materials[i], BR_MATU_ALL);
        }
    }
}

void C2_HOOK_FASTCALL RevealStoredTextures(tBrender_storage* pStorage) {
    int i;
    br_pixelmap* colour_map;

    for (i = 0; i < pStorage->materials_count; i++) {

        colour_map = pStorage->materialProps[i];

        if (colour_map != NULL) {
            pStorage->materials[i]->colour_map = colour_map;
            pStorage->materialProps[i] = NULL;
            pStorage->materials[i]->flags |= BR_MATF_PRELIT;
            BrMaterialUpdate(pStorage->materials[i], BR_MATU_ALL);
        }
    }
}

// FUNCTION: CARMA2_HW 0x00447350
void C2_HOOK_FASTCALL SetCarStorageTexturingLevel(tBrender_storage* pStorage, tCar_texturing_level pNew, tCar_texturing_level pOld) {

    switch (pNew) {
    case eCTL_full:
        RevealStoredTextures(pStorage);
        break;
    case eCTL_transparent:
        switch (pOld) {
        case eCTL_full:
            HideStoredOpaqueTextures(pStorage);
            break;
        case eCTL_none:
            RevealStoredTransparentTextures(pStorage);
            break;
        default:
            break;
        }
        break;
    case eCTL_none:
        HideStoredTextures(pStorage);
        break;
    default:
        break;
    }
}

// FUNCTION: CARMA2_HW 0x00447640
tCar_texturing_level C2_HOOK_FASTCALL GetCarTexturingLevel(void) {

    return gCar_texturing_level;
}

// FUNCTION: CARMA2_HW 0x00447650
    void C2_HOOK_FASTCALL SetCarTexturingLevel(tCar_texturing_level pLevel) {

    if (gCar_texturing_level != pLevel) {
        if (gOur_car_storage_space.models_count != 0) {
            SetCarStorageTexturingLevel(&gOur_car_storage_space, pLevel, gCar_texturing_level);
        }
        if (gTheir_cars_storage_space.models_count != 0) {
            SetCarStorageTexturingLevel(&gTheir_cars_storage_space, pLevel, gCar_texturing_level);
        }
        if (gNet_cars_storage_space.models_count != 0) {
            // FIXME: wrong BRender storage?
            SetCarStorageTexturingLevel(&gTheir_cars_storage_space, pLevel, gCar_texturing_level);
        }
    }
    gCar_texturing_level = pLevel;
}

// FUNCTION: CARMA2_HW 0x00448ce0
int C2_HOOK_FASTCALL HasThisSuffix(const char* pIdent, const char* pSuffix) {
    size_t len_ident;
    size_t len_suffix;

    len_ident = strlen(pIdent);
    len_suffix = strlen(pSuffix);
    if (pIdent == NULL) {
        return 0;
    }
    if (len_ident < len_suffix) {
        return 0;
    }
    return strcmp(pIdent + len_ident - len_suffix, pSuffix) == 0;
}

// FUNCTION: CARMA2_HW 0x00448d70
br_material* C2_HOOK_FASTCALL UnsuffixedMaterial(const char* pOld_ident, const char* pSuffix) {
    br_material* result;
    int unsuffixed_len;
    char* new_id;

    unsuffixed_len = strlen(pOld_ident) - strlen(pSuffix);
    new_id = BrMemAllocate(unsuffixed_len + 1, kMem_new_mat_id);
    sprintf(new_id, "%.*s", unsuffixed_len, pOld_ident);
    result = BrMaterialFind(new_id);
    BrMemFree(new_id);
    return result;
}

// FUNCTION: CARMA2_HW 0x00447d50
br_material* C2_HOOK_FASTCALL WallLinearToUntex(br_model* pModel, tU16 pFace) {
    br_material* old_mat;
    br_material* new_mat;
    size_t identifier_length;
    size_t suffix_length;
    char* identifier;
    char* new_id;
    int has_pwall_suffix;

    old_mat = pModel->faces[pFace].material;
    identifier = old_mat->identifier;
    identifier_length = strlen(identifier);
    suffix_length = strlen(gMaterial_suffix_pwall);
    if (identifier != NULL) {
        if (identifier_length >= suffix_length) {
            has_pwall_suffix = strcmp(identifier + identifier_length - suffix_length, gMaterial_suffix_pwall) == 0;
        } else {
            has_pwall_suffix = 0;
        }
    } else {
        has_pwall_suffix = 0;
    }
    if (has_pwall_suffix) {
        if (old_mat->colour_map != NULL) {
            old_mat->colour_map = NULL;
            BrMaterialUpdate(old_mat, 0x7fff);
        }
        return old_mat;
    }
    if (FUN_00447ef0(pModel, pFace) != 0 || old_mat->identifier == NULL || old_mat->colour_map == NULL) {
        return NULL;
    }
    suffix_length = strlen(gMaterial_suffix_lwall);
    identifier_length = strlen(old_mat->identifier);
    new_id = BrMemAllocate(identifier_length + suffix_length + 1, kMem_new_mat_id);
    sprintf(new_id, "%s%s", old_mat->identifier, gMaterial_suffix_lwall);
    new_mat = BrMaterialFind(new_id);
    if (new_mat != NULL) {
        BrMemFree(new_id);
    } else {
        new_mat = BrMaterialAllocate(0);
        CopyBrMaterial(new_mat, old_mat);
        new_mat->identifier = new_id;
        BrMaterialAdd(new_mat);
    }
    if (new_mat->colour_map != NULL) {
        new_mat->colour_map = NULL;
        BrMaterialUpdate(new_mat, 0x7fff);
    }
    return new_mat;
}

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x00447ef0
int C2_HOOK_FASTCALL FUN_00447ef0(br_model* pModel, tU16 pIndex) {
    br_vector3 edge0;
    br_vector3 edge1;
    br_vector3 cross;

    BrVector3Sub(&edge0, &pModel->vertices[pModel->faces[pIndex].vertices[0]].p, &pModel->vertices[pModel->faces[pIndex].vertices[1]].p);
    BrVector3Sub(&edge1, &pModel->vertices[pModel->faces[pIndex].vertices[1]].p, &pModel->vertices[pModel->faces[pIndex].vertices[2]].p);
    BrVector3Cross(&cross, &edge0, &edge1);
    return cross.v[1] * gFLOAT_00589d20 > sqrt(cross.v[0] * cross.v[0] + cross.v[2] * cross.v[2]);
}
#pragma auto_inline(on)

// FUNCTION: CARMA2_HW 0x00448480
br_material* C2_HOOK_FASTCALL FUN_00448480(void* pArg1, tU16 pIndex) {
    br_material* material;
    char* dest;
    char* suffix;
    tU8* entry;

    entry = *(tU8**)(*(tU8**)((tU8*)pArg1 + 0xc) + 8 + pIndex * 0x28);
    if (((br_material*)entry)->colour_map != NULL) {
        if (FUN_00447ef0(pArg1, pIndex) != 0) {
            suffix = ".road";
            dest = BrMemAllocate(strlen(suffix) + strlen(*(char**)(entry + 4)) + 1, 0xcf);
            sprintf(dest, "%s%s", *(char**)(entry + 4), suffix);
            material = BrMaterialFind(dest);
            if (material != NULL) {
                BrMemFree(dest);
            } else {
                material = BrMaterialAllocate(0);
                CopyBrMaterial(material, (br_material*)entry);
                material->identifier = dest;
                BrMaterialAdd(material);
            }
            if (material->colour_map != NULL) {
                material->colour_map = NULL;
                BrMaterialUpdate(material, 0x7fff);
            }
            return material;
        }
    }
    return NULL;
}

// FUNCTION: CARMA2_HW 0x00447c60
br_material* C2_HOOK_FASTCALL RoadUntexToPersp(br_model* pModel, tU16 pFace) {
    br_material* old_mat;
    br_material* new_mat;

    old_mat = pModel->faces[pFace].material;
    if (HasThisSuffix(old_mat->identifier, ".road")) {
        new_mat = UnsuffixedMaterial(old_mat->identifier, ".road");
    } else {
        new_mat = NULL;
    }
    return new_mat;
}

// FUNCTION: CARMA2_HW 0x004481d0
br_material* C2_HOOK_FASTCALL WallUntexToPersp(br_model* pModel, tU16 pFace) {
    br_material* old_mat;
    br_material* new_mat;

    old_mat = pModel->faces[pFace].material;
    if (HasThisSuffix(old_mat->identifier, ".lwall")) {
        new_mat = UnsuffixedMaterial(old_mat->identifier, ".lwall");
    } else if (HasThisSuffix(old_mat->identifier, ".pwall")) {
        new_mat = UnsuffixedMaterial(old_mat->identifier, ".pwall");
    } else {
        new_mat = NULL;
    }
    return new_mat;
}

// FUNCTION: CARMA2_HW 0x00448390
br_material* C2_HOOK_FASTCALL WallLinearToPersp(br_model* pModel, tU16 pFace) {
    br_material* old_mat;
    br_material* new_mat;

    old_mat = pModel->faces[pFace].material;
    if (HasThisSuffix(old_mat->identifier, ".pwall")) {
        new_mat = UnsuffixedMaterial(old_mat->identifier, ".pwall");
    } else {
        new_mat = NULL;
    }
    return new_mat;
}

// FUNCTION: CARMA2_HW 0x00448000
br_material* C2_HOOK_FASTCALL WallUntexToLinear(br_model* pModel, tU16 pFace) {
    br_material* old_mat;
    br_material* new_mat;

    old_mat = pModel->faces[pFace].material;
    if (HasThisSuffix(old_mat->identifier, ".lwall")) {
        return UnsuffixedMaterial(old_mat->identifier, ".lwall");
    }
    if (HasThisSuffix(old_mat->identifier, ".pwall")) {
        new_mat = UnsuffixedMaterial(old_mat->identifier, ".pwall");
        old_mat->colour_map = new_mat->colour_map;
        old_mat->flags &= ~BR_MATF_PERSPECTIVE;
        BrMaterialUpdate(old_mat, BR_MATU_ALL);
    }
    return NULL;
}

// FUNCTION: CARMA2_HW 0x00448560
br_material* C2_HOOK_FASTCALL WallPerspToLinear(br_model* pModel, tU16 pFace) {
    br_material* old_mat;
    br_material* new_mat;
    char* new_id;

    old_mat = pModel->faces[pFace].material;
    if (old_mat->colour_map == NULL || (old_mat->flags & BR_MATF_PERSPECTIVE) == 0 || FUN_00447ef0(pModel, pFace) != 0) {
        return NULL;
    }
    new_id = BrMemAllocate(strlen(old_mat->identifier) + strlen(gMaterial_suffix_pwall) + 1, kMem_new_mat_id);
    sprintf(new_id, "%s%s", old_mat->identifier, gMaterial_suffix_pwall);
    new_mat = BrMaterialFind(new_id);
    if (new_mat != NULL) {
        BrMemFree(new_id);
    } else {
        new_mat = BrMaterialAllocate(0);
        CopyBrMaterial(new_mat, old_mat);
        new_mat->identifier = new_id;
        BrMaterialAdd(new_mat);
    }
    if ((new_mat->flags & BR_MATF_PERSPECTIVE) != 0) {
        new_mat->flags &= ~BR_MATF_PERSPECTIVE;
        BrMaterialUpdate(new_mat, 0x7fff);
    }
    return new_mat;
}

static br_material* SuffixedMaterial(br_material* pOld, char* pSuffix);

// FUNCTION: CARMA2_HW 0x00448650
br_material* C2_HOOK_FASTCALL WallPerspToUntex(br_model* pModel, tU16 pFace) {
    register br_material* old_mat;
    register br_material* new_mat;

    old_mat = pModel->faces[pFace].material;
    if (old_mat->colour_map != NULL && FUN_00447ef0(pModel, pFace) == 0) {
        if ((old_mat->flags & BR_MATF_PERSPECTIVE) != 0) {
            new_mat = SuffixedMaterial(old_mat, gMaterial_suffix_pwall);
        } else {
            new_mat = SuffixedMaterial(old_mat, gMaterial_suffix_lwall);
        }
        if (new_mat->colour_map != NULL) {
            new_mat->colour_map = NULL;
            BrMaterialUpdate(new_mat, 0x7fff);
        }
        return new_mat;
    }
    return NULL;
}

static br_material* SuffixedMaterial(br_material* pOld, char* pSuffix) {
    br_material* new_mat;
    char* new_id;

    new_id = BrMemAllocate(strlen(pOld->identifier) + strlen(pSuffix) + 1, kMem_new_mat_id);
    sprintf(new_id, "%s%s", pOld->identifier, pSuffix);
    new_mat = BrMaterialFind(new_id);
    if (new_mat != NULL) {
        BrMemFree(new_id);
    } else {
        new_mat = BrMaterialAllocate(0);
        CopyBrMaterial(new_mat, pOld);
        new_mat->identifier = new_id;
        BrMaterialAdd(new_mat);
    }
    return new_mat;
}

// FUNCTION: CARMA2_HW 0x00448770
tRoad_texturing_level C2_HOOK_FASTCALL GetRoadTexturingLevel(void) {

    return gRoad_texturing_level;
}

// FUNCTION: CARMA2_HW 0x00448780
void C2_HOOK_FASTCALL SetRoadTexturingLevel(tRoad_texturing_level pLevel) {

    gRoad_texturing_level = pLevel;
}

// FUNCTION: CARMA2_HW 0x00448790
void C2_HOOK_FASTCALL FUN_00448790(tRoad_texturing_level pLevel) {
    tPMFMCB* callback;
    br_actor* track_actor;
    br_model* model;
    br_material* last_material;
    tU16 face_index;

    if (gRoad_texturing_level == pLevel) {
        return;
    }
    callback = RoadUntexToPersp;
    if (pLevel == 0) {
        callback = (tPMFMCB*)FUN_00448480;
    }
    track_actor = gProgram_state.track_spec.the_actor;
    if (track_actor->identifier != NULL && track_actor->identifier[0] == '&') {
        return;
    }
    if (track_actor->type == BR_ACTOR_MODEL && track_actor->model != NULL) {
        model = track_actor->model;
        last_material = NULL;
        for (face_index = 0; face_index < model->nfaces; face_index++) {
            if (model->faces[face_index].material != NULL) {
                br_material* new_material = callback(model, face_index);

                if (new_material != NULL) {
                    model->faces[face_index].material = new_material;
                    last_material = new_material;
                }
            }
        }
        if (last_material != NULL) {
            BrModelUpdate(model, BR_MODU_ALL);
        }
    }
    BrActorEnum(track_actor, ProcessFaceMaterials, callback);
}

// GetWallTexturingLevel

// FUNCTION: CARMA2_HW 0x00448900
void C2_HOOK_FASTCALL SetWallTexturingLevel(tWall_texturing_level pLevel) {

    gWall_texturing_level = pLevel;
}

// FUNCTION: CARMA2_HW 0x00448910
void C2_HOOK_FASTCALL ReallySetWallTexturingLevel(tWall_texturing_level pLevel) {
    tPMFMCB* callback;
    br_actor* track_actor;
    br_model* model;
    br_material* last_material;
    tU16 face_index;

    if (gWall_texturing_level == pLevel) {
        return;
    }
    callback = gWall_texturing_tweakers[gWall_texturing_level][pLevel];
    track_actor = gProgram_state.track_spec.the_actor;
    if (track_actor->identifier != NULL && track_actor->identifier[0] == '&') {
        return;
    }
    if (track_actor->type == BR_ACTOR_MODEL && track_actor->model != NULL) {
        model = track_actor->model;
        last_material = NULL;
        for (face_index = 0; face_index < model->nfaces; face_index++) {
            if (model->faces[face_index].material != NULL) {
                br_material* new_material = callback(model, face_index);

                if (new_material != NULL) {
                    model->faces[face_index].material = new_material;
                    last_material = new_material;
                }
            }
        }
        if (last_material != NULL) {
            BrModelUpdate(model, 0x7fff);
        }
    }
    BrActorEnum(track_actor, ProcessFaceMaterials, callback);
}

// FUNCTION: CARMA2_HW 0x00448dd0
br_material* C2_HOOK_FASTCALL DisposeSuffixedMaterials(br_model* pModel, tU16 pFace) {
    size_t max_suffix_len;
    br_material* mat;
    br_material* victim;
    // GLOBAL: CARMA2_HW 0x005913a0
    static const char* suffixes[3] = {
        ".road",
        ".pwall",
        ".lwall",
    };
    int s;
    char* id;

    mat = pModel->faces[pFace].material;
    if (mat->identifier != NULL) {
        max_suffix_len = 0;
        for (s = 0; s < CARPOCALYPSE2_ASIZE(suffixes); s++) {
            if (max_suffix_len < strlen(suffixes[s])) {
                max_suffix_len = strlen(suffixes[s]);
            }
        }
        id = BrMemAllocate(strlen(mat->identifier) + max_suffix_len + 1, kMem_new_mat_id);
        for (s = 0; s < CARPOCALYPSE2_ASIZE(suffixes); s++) {
            sprintf(id, "%s%s", mat->identifier, suffixes[s]);
            victim = BrMaterialFind(id);
            if (victim != NULL) {
                BrMaterialRemove(victim);
                BrMaterialFree(victim);
            }
        }
#ifdef CARPOCALYPSE2_FIX_BUGS
        BrMemFree(id);
#endif
    }
    return NULL;
}

// DisposeTexturingMaterials

// FUNCTION: CARMA2_HW 0x00448ea0
intptr_t C2_HOOK_CDECL SetAccessoryRenderingCB(br_actor* pActor, void* pFlag) {

    if (pActor->identifier != NULL && pActor->identifier[0] == '&') {
        pActor->render_style = *(br_uint_8*)pFlag;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00448ec0
void C2_HOOK_FASTCALL SetAccessoryRendering(int pOn) {
    int style;

    if (gTrack_actor != NULL)  {
        if (pOn) {
            style = BR_RSTYLE_FACES;
        } else {
            style = BR_RSTYLE_NONE;
        }
        DRActorEnumRecurse(gTrack_actor, SetAccessoryRenderingCB, &style);
    }
    gRendering_accessories = pOn;
}

// FUNCTION: CARMA2_HW 0x00448f00
int C2_HOOK_FASTCALL GetAccessoryRendering(void) {

    return gRendering_accessories;
}

// FUNCTION: CARMA2_HW 0x00448f10
void C2_HOOK_FASTCALL SetCarSimplificationLevel(int pLevel) {

    gCar_simplification_level = pLevel;
}

// FUNCTION: CARMA2_HW 0x00448f20
int C2_HOOK_FASTCALL GetCarSimplificationLevel(void) {

    return gCar_simplification_level;
}

// GetCarSimplificationLevel

static intptr_t C2_HOOK_CDECL ToggleTrackActorRenderStyleCB(br_actor* pActor, void* pArg);

// FUNCTION: CARMA2_HW 0x00448f30
void C2_HOOK_FASTCALL ToggleTrackRenderStyle(void) {
    int key_is_down;
    br_actor* track_actor;

    key_is_down = KeyIsDown(0);
    if (key_is_down != 0) {
        gRender_alternative_track_actors = (gRender_alternative_track_actors == 0);
        return;
    }
    track_actor = gTrack_actor;
    if (track_actor->render_style == BR_RSTYLE_DEFAULT) {
        track_actor->render_style = BR_RSTYLE_FACES;
    }
    DRActorEnumRecurse(track_actor, ToggleTrackActorRenderStyleCB, NULL);
}

// FUNCTION: CARMA2_HW 0x00448f70
static intptr_t C2_HOOK_CDECL ToggleTrackActorRenderStyleCB(br_actor* pActor, void* pArg) {
    if (pActor->render_style == BR_RSTYLE_FACES) {
        pActor->render_style = BR_RSTYLE_EDGES;
    } else if (pActor->render_style == BR_RSTYLE_EDGES) {
        pActor->render_style = BR_RSTYLE_FACES;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00448f90
intptr_t C2_HOOK_CDECL ProcessMaterials(br_actor* pActor, void* pContext) {
    material_cbfn* callback = pContext;

    if (pActor->material != NULL) {
        callback(pActor->material);
    }
    if (pActor->type == BR_ACTOR_MODEL && pActor->model != NULL) {
        ProcessModelFaceMaterials2(pActor->model, callback);
    }
    return BrActorEnum(pActor, ProcessMaterials, callback);
}

// FUNCTION: CARMA2_HW 0x004b4ed0
void C2_HOOK_FASTCALL BuildColourTable(br_pixelmap* pPalette) {
    int i;
    int j;
    int nearest_index;
    int red;
    int green;
    int blue;
    float nearest_distance;
    float distance;
    double sum;

    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gRGB_colours); i++) {
        nearest_distance = 196608.f;
        red = (gRGB_colours[i] >> 16) & 0xFF;
        green = (gRGB_colours[i] >> 8) & 0xFF;
        blue = gRGB_colours[i] & 0xFF;
        for (j = 0; j < 256; j++) {
            sum = sqr((double)(signed int)(*((br_uint_8*)pPalette->pixels + 4 * j + 2) - red));
            sum += sqr((double)(signed int)(*((br_uint_8*)pPalette->pixels + 4 * j) - blue));
            sum += sqr((double)(signed int)(*((br_uint_8*)pPalette->pixels + 4 * j + 1) - green));
            distance = (float)sum;
            if (distance < nearest_distance) {
                nearest_index = j;
                nearest_distance = distance;
            }
        }
        gColours[i] = nearest_index;
    }
}