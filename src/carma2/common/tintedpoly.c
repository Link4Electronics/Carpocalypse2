/*
 * Tinted polygon system (ported from tinted.c — real implementation).
 */
#include "tintedpoly.h"

#include "globvars.h"
#include "globvrkm.h"
#include "input.h"
#include "loading1.h"
#include "platform.h"
#include "utility.h"
#include "structur.h"

#include "carpocalypse2_macros.h"
#include "carpocalypse2_types.h"
#include "brender/brender.h"

#include "c2_string.h"

extern br_pixelmap* gRender_palette;

/* forward declarations */
br_model* C2_HOOK_FASTCALL BuildTintedPolyModel(int x0, int y0, int width, int height, int class, int arg1);
br_material* C2_HOOK_FASTCALL BuildTintedPolyMaterial(int pIndex);
void C2_HOOK_FASTCALL SetTintedPolyColour(int pTintedIndex, int pRed, int pGreen, int pBlue);
void C2_HOOK_FASTCALL DisablePlingMaterials(void);
void C2_HOOK_FASTCALL EnablePlingMaterials(void);
void C2_HOOK_FASTCALL ProcessTintedPoly(int pIndex);
void C2_HOOK_FASTCALL UpdatePulsePoly(int pIndex);
void C2_HOOK_FASTCALL TintedAnimateFire(int pIndex);
void C2_HOOK_FASTCALL TintedAnimatePulse(int pIndex);
void C2_HOOK_FASTCALL TintedAnimateWater(int pIndex);
void C2_HOOK_FASTCALL UpdateFirePoly(int pIndex);
void C2_HOOK_FASTCALL GetRangeOfValuesFromPixelmap(br_pixelmap* pPixelmap, int* pMin, int* pMax);
void C2_HOOK_FASTCALL UpdateTintedPolyByMaterial(int pTintedIndex);
void C2_HOOK_FASTCALL TurnTintedPolyOn(int pTintedIndex);
// (removed unused mismatched fwd-decl; real impl below)
extern void C2_HOOK_FASTCALL LoadAllImagesInDirectory(tBrender_storage* pStorage_space, const char* path);
extern void C2_HOOK_FASTCALL FindFace(br_vector3* pPosition, br_vector3* pDir, br_vector3* pNorm, br_scalar* pDistance, br_material** pMaterial);
// GLOBAL: CARMA2_HW 0x00705c80
tTintedPoly gTintedPolys[10];

br_colour* gTinted_poly_palette = NULL;

#define TINTED_COUNT_TOKENS 2

// GLOBAL: CARMA2_HW 0x0065e840
br_token_value gTinted_poly_material_tokens[TINTED_COUNT_TOKENS] = {
  { BRT_BLEND_B, { 1 } },
  { BRT_OPACITY_X, { BR_FIXED_INT(0x80) } },
};

// GLOBAL: CARMA2_HW 0x0065e850
br_token_value gTinted_poly_zero_opacity_tokens[TINTED_COUNT_TOKENS] = {
  { BRT_BLEND_B, { 1 } },
  { BRT_OPACITY_X, { BR_FIXED_INT(0x80) } },
};

// GLOBAL: CARMA2_HW 0x0065e860
br_token_value gTinted_poly_custom_opacity_tokens[TINTED_COUNT_TOKENS] = {
  { BRT_BLEND_B, { 1 } },
  { BRT_OPACITY_X, { BR_FIXED_INT(0x80) } },
};

// GLOBAL: CARMA2_HW 0x006a0430
br_actor* gTintedPolyCamera;

// GLOBAL: CARMA2_HW 0x0065e874
int gDefaultOpacity_TintedPoly = 0x80;

// GLOBAL: CARMA2_HW 0x0065e870
br_uint_32 gTintedColourMap_red = 0x80;

// GLOBAL: CARMA2_HW 0x006a0438
br_uint_32 gTintedColourMap_grn;

// GLOBAL: CARMA2_HW 0x006a043c
br_uint_32 gTintedColourMap_blu;


// GLOBAL: CARMA2_HW 0x006a0440
int gINT_006a0440;

// GLOBAL: CARMA2_HW 0x006a0444
int gINT_006a0444;

// GLOBAL: CARMA2_HW 0x006a0448
int gINT_006a0448;

// GLOBAL: CARMA2_HW 0x006a044c
int gINT_006a044c;

#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_CDECL BrActorFree();
extern br_actor* C2_HOOK_CDECL BrActorRemove();
extern void C2_HOOK_CDECL BrMaterialFree();
extern void C2_HOOK_CDECL BrModelFree();
extern char gEthereal_pedestrians[];
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_CDECL BrMapUpdate();
extern br_model* C2_HOOK_CDECL BrModelAllocate();
extern void C2_HOOK_CDECL BrModelUpdate();
extern void C2_HOOK_CDECL BrPixelmapPixelSet();
extern br_uint_32 C2_HOOK_CDECL BrPixelmapSave();
extern FILE* C2_HOOK_FASTCALL DRfopen(const char* pFilename, const char* pMode);
extern int C2_HOOK_FASTCALL IRandomBetween(int pA, int pB);
extern void C2_HOOK_FASTCALL PathCat(char* pDestn_str, const char* pStr_1, const char* pStr_2);
extern tPath_name gApplication_path;
extern int gCancel_recover;
extern char gNext_crush_actor_id[];
extern int gUNK_00596310;
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_CDECL BrMaterialUpdate();
extern br_uint_32 C2_HOOK_CDECL BrPixelmapPixelGet();
extern tU32 C2_HOOK_FASTCALL GetTotalTime(void);
extern void __cdecl _ftol();
extern char gConst_average_ped_scale_factor[];
#endif
// FUNCTION: CARMA2_HW 0x004d7040
void C2_HOOK_FASTCALL InitTintedPolyStuff(void) {
    br_camera *camera;

    C2_HOOK_BUG_ON(sizeof(tTintedPoly) != 0x6450);

    memset(gTintedPolys, 0, sizeof(gTintedPolys));

    if (gTintedPolyCamera == NULL) {
        gTintedPolyCamera = BrActorAllocate(BR_ACTOR_CAMERA, NULL);
        gTintedPolyCamera->identifier = "tinted_poly_camera";

        camera = gTintedPolyCamera->type_data;
        camera->type = BR_CAMERA_PARALLEL;
        camera->hither_z = 1.0f;
        camera->yon_z = 3.0f;
        camera->width = gScreen->width;
        camera->height = gScreen->height;
    }
}

br_material* C2_HOOK_FASTCALL BuildTintedPolyMaterial(int pOpacity) {
    br_material *material;

    material = BrMaterialAllocate("Tint Poly Mat");
    if (material == NULL) {
        return NULL;
    }
    material->colour = BR_COLOUR_RGB(0xc8, 0xc8, 0xc8);
    material->flags = BR_MATF_ALWAYS_VISIBLE;
    BrMaterialAdd(material);
    if (pOpacity != 0) {
        material->extra_prim = gTinted_poly_material_tokens;
        material->extra_prim[1].v.x = BR_FIXED_INT(pOpacity);
        BrMaterialUpdate(material, BR_MATU_EXTRA_PRIM);
    }
    return material;
}

void C2_HOOK_FASTCALL GetRangeOfValuesFromPixelmap(br_pixelmap* pMap, int* pDarkest, int* pBrightest) {
    int y;
    int x;
    br_uint_32 colour;
    int colour_component;

    *pDarkest = 255;
    *pBrightest = 0;
    for (y = 0; y < pMap->height; y++) {
        for (x = 0; x < pMap->width; x++) {
            colour = BrPixelmapPixelGet(pMap, x, y);
            colour_component = CARPOCALYPSE2_RGB555_B(colour);
            if (colour_component < *pDarkest) {
                *pDarkest = colour_component;
            }
            if (colour_component > *pBrightest) {
                *pBrightest = colour_component;
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x004d70c0
int C2_HOOK_FASTCALL CreateTintedPoly(int x0, int y0, int width, int height, int class, int arg1, int arg2, int arg3) {
    tPath_name the_path;
    int i;
    int x;
    int y;
    int tintedIndex;
    br_pixelmap* map;
    int darkest_colour;
    int brightest_colour;
    br_uint_32 colour;
    br_scalar colour_scale;
    br_uint_32* pTint;

    tintedIndex = -1;
    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gTintedPolys); i++) {
        if (!gTintedPolys[i].used) {
            tintedIndex = i;
            break;
        }
    }
    if (tintedIndex == -1) {
        return -1;
    }
    switch (class) {
        case 2:
            gTintedPolys[tintedIndex].class = class;
            gTintedPolys[tintedIndex].material = BuildTintedPolyMaterial(gDefaultOpacity_TintedPoly);
            gTintedPolys[tintedIndex].model = BuildTintedPolyModel(x0, y0, width, height, 1, 1);
            gTintedPolys[tintedIndex].actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
            gTintedPolys[tintedIndex].actor->material = gTintedPolys[tintedIndex].material;
            for (i = 0; i < gTintedPolys[tintedIndex].model->nfaces; i++) {
                gTintedPolys[tintedIndex].model->faces[i].material = gTintedPolys[tintedIndex].material;
            }
            gTintedPolys[tintedIndex].actor->model = gTintedPolys[tintedIndex].model;
            BrModelAdd(gTintedPolys[tintedIndex].model);
            break;
        case 3:
            gTintedPolys[tintedIndex].class = class;
            gTintedPolys[tintedIndex].material = BuildTintedPolyMaterial(gDefaultOpacity_TintedPoly);
            gTintedPolys[tintedIndex].material->flags |= BR_MATF_PRELIT | BR_MATF_SMOOTH;
            BrMaterialUpdate(gTintedPolys[tintedIndex].material, BR_MATU_ALL);
            gTintedPolys[tintedIndex].model = BuildTintedPolyModel(x0, y0, width, height, 4, 4);
            gTintedPolys[tintedIndex].actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
            gTintedPolys[tintedIndex].actor->material = gTintedPolys[tintedIndex].material;
            for (i = 0; i < gTintedPolys[tintedIndex].model->nfaces; i++) {
                gTintedPolys[tintedIndex].model->faces[i].material = gTintedPolys[tintedIndex].material;
            }
            gTintedPolys[tintedIndex].actor->model = gTintedPolys[tintedIndex].model;
            gTintedPolys[tintedIndex].colour = arg1;
            BrModelAdd(gTintedPolys[tintedIndex].model);
            break;
        case 5:
            gTintedPolys[tintedIndex].class = class;
            gTintedPolys[tintedIndex].material = BuildTintedPolyMaterial(gDefaultOpacity_TintedPoly);
            gTintedPolys[tintedIndex].material->flags |= BR_MATF_PRELIT | BR_MATF_SMOOTH;
            gTintedPolys[tintedIndex].material->extra_prim = gTinted_poly_zero_opacity_tokens;
            gTintedPolys[tintedIndex].material->extra_prim[1].v.x = BR_FIXED_INT(0);
            BrMaterialUpdate(gTintedPolys[tintedIndex].material, BR_MATU_ALL);
            gTintedPolys[tintedIndex].model = BuildTintedPolyModel(x0, y0, width, height, 1, 1);
            gTintedPolys[tintedIndex].actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
            gTintedPolys[tintedIndex].actor->material = gTintedPolys[tintedIndex].material;
            for (i = 0; i < gTintedPolys[tintedIndex].model->nfaces; i++) {
                gTintedPolys[tintedIndex].model->faces[i].material = gTintedPolys[tintedIndex].material;
            }
            gTintedPolys[tintedIndex].actor->model = gTintedPolys[tintedIndex].model;
            gTintedPolys[tintedIndex].unknown_1 = 0;
            gTintedPolys[tintedIndex].unknown_2 = arg1;
            BrModelAdd(gTintedPolys[tintedIndex].model);
            SetTintedPolyColour(tintedIndex, 0, 0, 0);
            break;
        case 4:
            gTintedPolys[tintedIndex].class = class;
            gTintedPolys[tintedIndex].material = BuildTintedPolyMaterial(0);
            gTintedPolys[tintedIndex].material->flags |= BR_MATF_QUAD_MAPPING;
            PathCat(the_path, gApplication_path, "fire");
            LoadAllImagesInDirectory(&gMisc_storage_space, the_path);
            map = BrMapFind("testfire");
            gTintedPolys[tintedIndex].pixelmap = map;
            gTintedPolys[tintedIndex].material->colour_map = map;
            BrMaterialUpdate(gTintedPolys[tintedIndex].material, BR_MATU_ALL);

            map = BrMapFind("firepal1");
            for (i = 0; i < 256; i++) {
                gTintedPolys[tintedIndex].colours[i] = BrPixelmapPixelGet(map, 0, i);
            }

            map = BrMapFind("coolmapx");
            GetRangeOfValuesFromPixelmap(map, &darkest_colour, &brightest_colour);

            // Map `coolmapx` colors to [0, arg1]
            colour_scale = (br_scalar)(br_uint_32)arg1 / (brightest_colour - darkest_colour);
            for (y = 0; y < 64; y++) {
                pTint = &gTintedPolys[tintedIndex].tints1[y];
                for (x = 0; x < 32; x++, pTint += 64) {
                    int v;
                    colour = BrPixelmapPixelGet(map, x, y);
                    v = (int)(br_int_64)(((int)CARPOCALYPSE2_RGB555_B(colour) - darkest_colour) * (double)colour_scale);
                    if (v < 0) {
                        *pTint = 0;
                    } else if ((br_uint_32)v > (br_uint_32)arg1) {
                        *pTint = (br_uint_32)arg1;
                    } else {
                        *pTint = (br_uint_32)v;
                    }
                }
            }
            memset(gTintedPolys[tintedIndex].tints2, 0, sizeof(gTintedPolys[tintedIndex].tints2));
            memset(gTintedPolys[tintedIndex].tints3, 0, sizeof(gTintedPolys[tintedIndex].tints3));

            gTintedPolys[tintedIndex].model = BuildTintedPolyModel(x0, y0, width, height, 1, 1);
            gTintedPolys[tintedIndex].actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
            gTintedPolys[tintedIndex].actor->material = gTintedPolys[tintedIndex].material;
            for (i = 0; i < gTintedPolys[tintedIndex].model->nfaces; i++) {
                gTintedPolys[tintedIndex].model->faces[i].material = gTintedPolys[tintedIndex].material;
            }
            gTintedPolys[tintedIndex].actor->model = gTintedPolys[tintedIndex].model;
            BrModelAdd(gTintedPolys[tintedIndex].model);
            break;
        case 6:
            gTintedPolys[tintedIndex].class = class;
            gTintedPolys[tintedIndex].material = BuildTintedPolyMaterial(gDefaultOpacity_TintedPoly);
            gTintedPolys[tintedIndex].model = BuildTintedPolyModel(x0, y0, width, height, 1, 1);
            gTintedPolys[tintedIndex].actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
            gTintedPolys[tintedIndex].actor->material = gTintedPolys[tintedIndex].material;
            for (i = 0; i < gTintedPolys[tintedIndex].model->nfaces; i++) {
                gTintedPolys[tintedIndex].model->faces[i].material = gTintedPolys[tintedIndex].material;
            }
            gTintedPolys[tintedIndex].actor->model = gTintedPolys[tintedIndex].model;
            BrModelAdd(gTintedPolys[tintedIndex].model);
            gTintedPolys[tintedIndex].subClass = arg1;
            switch (arg1) {
                case 0:
                    gTintedPolys[tintedIndex].color2_red = CARPOCALYPSE2_RGB888_R(arg2);
                    gTintedPolys[tintedIndex].color2_grn = CARPOCALYPSE2_RGB888_G(arg2);
                    gTintedPolys[tintedIndex].color2_blu = CARPOCALYPSE2_RGB888_B(arg2);
                    gTintedPolys[tintedIndex].colour = arg3;
                    gTintedPolys[tintedIndex].material->colour = BR_COLOUR_RGB(gTintedPolys[tintedIndex].color2_red, gTintedPolys[tintedIndex].color2_grn, gTintedPolys[tintedIndex].color2_blu);
                    BrMaterialUpdate(gTintedPolys[tintedIndex].material, BR_MATU_ALL);
                    break;
                case 1:
                    gTintedPolys[tintedIndex].material->flags |= BR_MATF_PRELIT | BR_MATF_SMOOTH;
                    BrMaterialUpdate(gTintedPolys[tintedIndex].material, BR_MATU_ALL);
                    gTintedPolys[tintedIndex].colour = arg2;
                    break;
                default:
                    PDFatalError("Invalid Pulse Poly subclass");
            }
            break;
        default:
            gTintedPolys[tintedIndex].class = class;
            gTintedPolys[tintedIndex].material = BuildTintedPolyMaterial(gDefaultOpacity_TintedPoly);
            gTintedPolys[tintedIndex].model = BuildTintedPolyModel(x0, y0, width, height, 1, 1);
            gTintedPolys[tintedIndex].actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
            gTintedPolys[tintedIndex].actor->material = gTintedPolys[tintedIndex].material;
            for (i = 0; i < gTintedPolys[tintedIndex].model->nfaces; i++) {
                gTintedPolys[tintedIndex].model->faces[i].material = gTintedPolys[tintedIndex].material;
            }
            gTintedPolys[tintedIndex].actor->model = gTintedPolys[tintedIndex].model;
            BrModelAdd(gTintedPolys[tintedIndex].model);
            gTintedPolys[tintedIndex].material->colour = arg1;
            gTintedPolys[tintedIndex].material->extra_prim = gTinted_poly_custom_opacity_tokens;
            gTintedPolys[tintedIndex].material->extra_prim[1].v.x = BR_FIXED_INT(arg2);
            BrMaterialUpdate(gTintedPolys[tintedIndex].material, BR_MATU_ALL);
            break;
    }
    if (gTintedPolys[tintedIndex].actor != NULL) {
        BrActorAdd(gTintedPolyCamera, gTintedPolys[tintedIndex].actor);
    }
    gTintedPolys[tintedIndex].used = 1;
    if (gTintedPolys[tintedIndex].actor != NULL) {
        (gTintedPolys[tintedIndex].actor)->render_style = BR_RSTYLE_NONE;
    }
    gTintedPolys[tintedIndex].color_red = 0;
    gTintedPolys[tintedIndex].color_grn = 0;
    gTintedPolys[tintedIndex].color_blu = 0;
    gTintedPolys[tintedIndex].visible = 0;
    return tintedIndex;
}

// FUNCTION: CARMA2_HW 0x004d7ad0
C2_NAKED br_model* C2_HOOK_FASTCALL BuildTintedPolyModel(int x0, int y0, int width, int height, int nbX, int nbY) {
#ifndef CARPOCALYPSE2_MATCHING
    br_model* model;
    int dx;
    int dy;
    int step_x;
    int step_y;
    int i;
    int j;
    int vert_below;
    int vert_above;
    br_vertex* vertex;
    br_face* face;

    if (nbX <= 0 || nbY < 0) {
        return NULL;
    }
    model = BrModelAllocate("Tint Poly", (nbX + 1) * (nbY + 1), 2 * nbX * nbY);
    if (model == NULL) {
        return NULL;
    }
    step_x = width / nbX;
    step_y = height / nbY;
    vertex = model->vertices;
    for (j = 0; j < nbY + 1; j++) {
        dy = j * step_y;
        for (i = 0; i < nbX + 1; i++) {
            dx = i * step_x;
            vertex->p.v[0] = (br_scalar)(x0 + dx);
            vertex->p.v[1] = (br_scalar)(-(y0 + dy));
            vertex->p.v[2] = -1.02f;
            vertex->map.v[0] = (br_scalar)((double)dx / width);
            vertex->map.v[1] = (br_scalar)((double)dy / height);
            vertex++;
        }
    }
    face = model->faces;
    for (j = 0; j < nbY; j++) {
        vert_above = j * (nbX + 1);
        vert_below = (j + 1) * (nbX + 1);
        for (i = 0; i < nbX; i++) {
            face[0].vertices[0] = (br_uint_16)(vert_above + i);
            face[0].vertices[1] = (br_uint_16)(vert_above + i + 1);
            face[0].vertices[2] = (br_uint_16)(vert_below + i);
            face[1].vertices[0] = (br_uint_16)(vert_above + i + 1);
            face[1].vertices[1] = (br_uint_16)(vert_below + i + 1);
            face[1].vertices[2] = (br_uint_16)(vert_below + i);
            face += 2;
        }
    }
    model->flags |= BR_MODF_DONT_WELD | BR_MODF_KEEP_ORIGINAL;
    return model;
#else
    __asm {
            sub esp, 0x1c
            push ebx
            push ebp
            mov ebp, dword ptr [esp + 0x30]
            push esi
            cmp ebp, 1
            push edi
            mov dword ptr [esp + 0x1c], edx
            mov ebx, ecx
            jl L004d7c69
            mov eax, dword ptr [esp + 0x3c]
            test eax, eax
            jl L004d7c69
            lea esi, [eax + 1]
            imul eax, ebp
            lea ecx, [ebp + 1]
            imul ecx, esi
            shl eax, 1
            push eax
            push ecx
            push offset gUNK_00596310+0x10c
            call BrModelAllocate
            add esp, 0xc
            mov dword ptr [esp + 0x14], eax
            test eax, eax
            je L004d7c69
            mov eax, dword ptr [esp + 0x30]
            mov ecx, dword ptr [esp + 0x14]
            cdq
            idiv ebp
            mov ecx, dword ptr [ecx + 8]
            mov edi, eax
            mov eax, dword ptr [esp + 0x34]
            cdq
            idiv dword ptr [esp + 0x3c]
            test esi, esi
            mov dword ptr [esp + 0x28], eax
            jle L004d7bf0
            mov edx, eax
            mov eax, dword ptr [esp + 0x1c]
            neg edx
            neg eax
            mov dword ptr [esp + 0x38], 0
            mov dword ptr [esp + 0x24], edx
            mov dword ptr [esp + 0x10], eax
            mov dword ptr [esp + 0x1c], esi
    L004d7b62:
            lea esi, [ebp + 1]
            test esi, esi
            jle L004d7bc9
            fild dword ptr [esp + 0x38]
            fild dword ptr [esp + 0x34]
            fild dword ptr [esp + 0x10]
            fild dword ptr [esp + 0x30]
            fxch st(2)
            fdivp st(3), st(0)
            fxch st(2)
            xor edx, edx
            mov dword ptr [esp + 0x18], edx
    L004d7b85:
            fild dword ptr [esp + 0x18]
            fld st(3)
            fxch st(1)
            fdiv st(0), st(3)
            lea eax, [ebx + edx]
            mov dword ptr [ecx + 8], 0xbf828f5c
            mov dword ptr [esp + 0x20], eax
            add edx, edi
            add ecx, 0x28
            dec esi
            mov dword ptr [esp + 0x18], edx
            fild dword ptr [esp + 0x20]
            fld st(3)
            fxch st(1)
            fstp dword ptr [ecx - 0x28]
            fxch st(2)
            fstp dword ptr [ecx - 0x24]
            fstp dword ptr [ecx - 0x1c]
            fstp dword ptr [ecx - 0x18]
            jne L004d7b85
            mov edx, dword ptr [esp + 0x24]
            fstp st(0)
            fstp st(0)
            fstp st(0)
    L004d7bc9:
            mov eax, dword ptr [esp + 0x28]
            mov esi, dword ptr [esp + 0x38]
            add esi, eax
            mov eax, dword ptr [esp + 0x1c]
            mov dword ptr [esp + 0x38], esi
            mov esi, dword ptr [esp + 0x10]
            add esi, edx
            dec eax
            mov dword ptr [esp + 0x10], esi
            mov dword ptr [esp + 0x1c], eax
            jne L004d7b62
    L004d7bf0:
            mov ecx, dword ptr [esp + 0x14]
            xor ebx, ebx
            mov dword ptr [esp + 0x34], ebx
            mov eax, dword ptr [ecx + 0xc]
            mov ecx, dword ptr [esp + 0x3c]
            test ecx, ecx
            jle L004d7c57
    L004d7c05:
            xor ecx, ecx
            test ebp, ebp
            jle L004d7c4a
            lea edx, [ebp + 1]
            mov edi, ebx
            imul edi, edx
            inc ebx
            imul ebx, edx
    L004d7c17:
            lea edx, [ecx + edi]
            lea esi, [ecx + ebx]
            mov word ptr [eax], dx
            lea edx, [ecx + edi + 1]
            mov word ptr [eax + 2], dx
            mov word ptr [eax + 4], si
            add eax, 0x28
            mov word ptr [eax], dx
            lea edx, [ecx + ebx + 1]
            mov word ptr [eax + 2], dx
            mov word ptr [eax + 4], si
            add eax, 0x28
            inc ecx
            cmp ecx, ebp
            jl L004d7c17
            mov ebx, dword ptr [esp + 0x34]
    L004d7c4a:
            mov ecx, dword ptr [esp + 0x3c]
            inc ebx
            cmp ebx, ecx
            mov dword ptr [esp + 0x34], ebx
            jl L004d7c05
    L004d7c57:
            mov eax, dword ptr [esp + 0x14]
            or byte ptr [eax + 0x20], 3
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x1c
            ret 0x10
    L004d7c69:
            pop edi
            pop esi
            pop ebp
            xor eax, eax
            pop ebx
            add esp, 0x1c
            ret 0x10
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004d7c80
void C2_HOOK_FASTCALL FreeTintedPolyActor(int pTintedIndex) {
    if (!gTintedPolys[pTintedIndex].used) {
        return;
    }
    if (gTintedPolys[pTintedIndex].material != NULL) {
        BrMaterialFree(gTintedPolys[pTintedIndex].material);
    }
    if (gTintedPolys[pTintedIndex].model != NULL) {
        BrModelFree(gTintedPolys[pTintedIndex].model);
    }
    if (gTintedPolys[pTintedIndex].actor != NULL) {
        BrActorRemove(gTintedPolys[pTintedIndex].actor);
        BrActorFree(gTintedPolys[pTintedIndex].actor);
        gTintedPolys[pTintedIndex].actor = NULL;
    }
    gTintedPolys[pTintedIndex].class = 0;
    gTintedPolys[pTintedIndex].material2 =NULL;
    gTintedPolys[pTintedIndex].visible = 0;
    gTintedPolys[pTintedIndex].used = 0;
}

// FUNCTION: CARMA2_HW 0x004d7d10
void C2_NAKED C2_HOOK_FASTCALL FreeAllTintedPolyActor(void) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;

    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gTintedPolys); i++) {
        if (!gTintedPolys[i].used) {
            continue;
        }
        if (gTintedPolys[i].material != NULL) {
            BrMaterialFree(gTintedPolys[i].material);
        }
        if (gTintedPolys[i].model != NULL) {
            BrModelFree(gTintedPolys[i].model);
        }
        if (gTintedPolys[i].actor != NULL) {
            BrActorRemove(gTintedPolys[i].actor);
            BrActorFree(gTintedPolys[i].actor);
            gTintedPolys[i].actor = NULL;
        }
        gTintedPolys[i].class = 0;
        gTintedPolys[i].material2 = NULL;
        gTintedPolys[i].visible = 0;
        gTintedPolys[i].used = 0;
    }
#else
    __asm {
            push esi
            push edi
            mov esi, offset gTintedPolys
            xor edi, edi
    L004d7d19:
            cmp dword ptr [esi + 0x30], edi
            je L004d7d66
            mov eax, dword ptr [esi + 4]
            cmp eax, edi
            je L004d7d2e
            push eax
            call BrMaterialFree
            add esp, 4
    L004d7d2e:
            mov eax, dword ptr [esi + 8]
            cmp eax, edi
            je L004d7d3e
            push eax
            call BrModelFree
            add esp, 4
    L004d7d3e:
            mov eax, dword ptr [esi]
            cmp eax, edi
            je L004d7d5a
            push eax
            call BrActorRemove
            mov eax, dword ptr [esi]
            add esp, 4
            push eax
            call BrActorFree
            add esp, 4
            mov dword ptr [esi], edi
    L004d7d5a:
            mov dword ptr [esi + 0x38], edi
            mov dword ptr [esi + 0x2c], edi
            mov dword ptr [esi + 0x34], edi
            mov dword ptr [esi + 0x30], edi
    L004d7d66:
            add esi, 0x6450
            cmp esi, offset gEthereal_pedestrians
            jl L004d7d19
            pop edi
            pop esi
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

// FUNCTION: CARMA2_HW 0x004d7d80
void C2_NAKED C2_HOOK_FASTCALL UpdateTintedPolyByMaterial(int pIndex) {
#ifndef CARPOCALYPSE2_MATCHING
    br_uint_32 new_red, new_grn, new_blu;
    tU32 time;
    tU32 colour_sum;
    int material_changed;
    tU32 time_since_last_update_s;

    if (!gTintedPolys[pIndex].visible || gTintedPolys[pIndex].material2 == NULL) {
        return;
    }
    if (gTintedPolys[pIndex].material2->colour_map == NULL || gTintedPolys[pIndex].material2->colour_map->pixels == NULL) {
        new_red = gTintedColourMap_red;
        new_grn = gTintedColourMap_grn;
        new_blu = gTintedColourMap_blu;
    } else {
        br_uint_32 v;
        br_uint_32 w = gTintedPolys[pIndex].material2->colour_map->width;
        br_uint_32 h = gTintedPolys[pIndex].material2->colour_map->height;
        br_uint_32 quarter_width = w / 4;
        br_uint_32 quarter_height = h / 4;

        v = BrPixelmapPixelGet(gTintedPolys[pIndex].material2->colour_map, quarter_width, quarter_height);
        new_red = CARPOCALYPSE2_RGB888_R(v);
        new_grn = CARPOCALYPSE2_RGB888_G(v);
        new_blu = CARPOCALYPSE2_RGB888_B(v);
        v = BrPixelmapPixelGet(gTintedPolys[pIndex].material2->colour_map, w - quarter_width, quarter_height);
        new_red += CARPOCALYPSE2_RGB888_R(v);
        new_grn += CARPOCALYPSE2_RGB888_G(v);
        new_blu += CARPOCALYPSE2_RGB888_B(v);
        v = BrPixelmapPixelGet(gTintedPolys[pIndex].material2->colour_map, quarter_width, h - quarter_height);
        new_red += CARPOCALYPSE2_RGB888_R(v);
        new_grn += CARPOCALYPSE2_RGB888_G(v);
        new_blu += CARPOCALYPSE2_RGB888_B(v);
        v = BrPixelmapPixelGet(gTintedPolys[pIndex].material2->colour_map, w - quarter_width, h - quarter_height);
        new_red += CARPOCALYPSE2_RGB888_R(v);
        new_grn += CARPOCALYPSE2_RGB888_G(v);
        new_blu += CARPOCALYPSE2_RGB888_B(v);
        new_red = (br_int_32)new_red >> 2;
        new_grn = (br_int_32)new_grn >> 2;
        new_blu = (br_int_32)new_blu >> 2;
    }
    time = GetTotalTime();
    colour_sum = gTintedPolys[pIndex].color_blu;
    colour_sum += gTintedPolys[pIndex].color_grn;
    if (gTintedPolys[pIndex].color_red + colour_sum == 0) {
        gTintedPolys[pIndex].color_red = new_red;
        gTintedPolys[pIndex].color_grn = new_grn;
        gTintedPolys[pIndex].color_blu = new_blu;
        gTintedPolys[pIndex].lastTime = time;
        gTintedPolys[pIndex].material->colour = BR_COLOUR_RGB(new_red, new_grn, new_blu);
        BrMaterialUpdate(gTintedPolys[pIndex].material, BR_MATU_LIGHTING);
        return;
    }

    material_changed = 0;
    time_since_last_update_s = (time - (tU32)gTintedPolys[pIndex].lastTime) >> 12;
    if ((br_int_32)time_since_last_update_s < 0) {
        time_since_last_update_s = 0;
    }
    if (time_since_last_update_s != 0) {
        if (gTintedPolys[pIndex].color_red != new_red) {
            if ((br_int_32)gTintedPolys[pIndex].color_red < (br_int_32)new_red) {
                gTintedPolys[pIndex].color_red += time_since_last_update_s;
            } else {
                gTintedPolys[pIndex].color_red -= time_since_last_update_s;
            }
            if ((br_int_32)gTintedPolys[pIndex].color_red > (br_int_32)(new_red - time_since_last_update_s) && (br_int_32)gTintedPolys[pIndex].color_red < (br_int_32)(time_since_last_update_s + new_red)) {
                gTintedPolys[pIndex].color_red = new_red;
            }
            material_changed = 1;
        }
        if (gTintedPolys[pIndex].color_grn != new_grn) {
            if ((br_int_32)gTintedPolys[pIndex].color_grn < (br_int_32)new_grn) {
                gTintedPolys[pIndex].color_grn += time_since_last_update_s;
            } else {
                gTintedPolys[pIndex].color_grn -= time_since_last_update_s;
            }
            if ((br_int_32)gTintedPolys[pIndex].color_grn > (br_int_32)(new_grn - time_since_last_update_s) && (br_int_32)gTintedPolys[pIndex].color_grn < (br_int_32)(time_since_last_update_s + new_grn)) {
                gTintedPolys[pIndex].color_grn = new_grn;
            }
            material_changed = 1;
        }
        if (gTintedPolys[pIndex].color_blu != new_blu) {
            if ((br_int_32)gTintedPolys[pIndex].color_blu < (br_int_32)new_blu) {
                gTintedPolys[pIndex].color_blu += time_since_last_update_s;
            } else {
                gTintedPolys[pIndex].color_blu -= time_since_last_update_s;
            }
            if ((br_int_32)gTintedPolys[pIndex].color_blu > (br_int_32)(new_blu - time_since_last_update_s) && (br_int_32)gTintedPolys[pIndex].color_blu < (br_int_32)(time_since_last_update_s + new_blu)) {
                gTintedPolys[pIndex].color_blu = new_blu;
            }
            material_changed = 1;
        }
        if (material_changed) {
            gTintedPolys[pIndex].material->colour = BR_COLOUR_RGB(gTintedPolys[pIndex].color_red, gTintedPolys[pIndex].color_grn, gTintedPolys[pIndex].color_blu);
            BrMaterialUpdate(gTintedPolys[pIndex].material, BR_MATU_LIGHTING);
        }
    }
#else
    __asm {
            sub esp, 0x14
            push ebx
            push ebp
            push esi
            lea esi, [ecx + ecx*4]
            shl esi, 6
            add esi, ecx
            push edi
            lea esi, [esi + esi*4]
            shl esi, 4
            mov eax, dword ptr [esi + gTintedPolys+0x34]
            test eax, eax
            je L004d803b
            mov eax, dword ptr [esi + gTintedPolys+0x2c]
            test eax, eax
            je L004d803b
            mov eax, dword ptr [eax + 0x40]
            test eax, eax
            je L004d7edd
            mov ecx, dword ptr [eax + 8]
            test ecx, ecx
            je L004d7edd
            xor ebp, ebp
            xor ebx, ebx
            mov bp, word ptr [eax + 0x34]
            mov bx, word ptr [eax + 0x36]
            mov dword ptr [esp + 0x20], ebx
            mov edi, ebp
            shr ebx, 2
            shr edi, 2
            push ebx
            push edi
            push eax
            call BrPixelmapPixelGet
            xor edx, edx
            mov dword ptr [esp + 0x1c], eax
            mov dl, ah
            and eax, 0xff
            xor ecx, ecx
            mov dword ptr [esp + 0x20], eax
            mov cl, byte ptr [esp + 0x1e]
            mov eax, dword ptr [esi + gTintedPolys+0x2c]
            add esp, 0xc
            mov dword ptr [esp + 0x18], ecx
            mov ecx, dword ptr [eax + 0x40]
            sub ebp, edi
            push ebx
            push ebp
            push ecx
            mov dword ptr [esp + 0x28], edx
            call BrPixelmapPixelGet
            mov ecx, dword ptr [esp + 0x24]
            mov dword ptr [esp + 0x1c], eax
            xor edx, edx
            add esp, 0xc
            mov dl, byte ptr [esp + 0x12]
            add ecx, edx
            mov edx, dword ptr [esp + 0x1c]
            mov dword ptr [esp + 0x18], ecx
            xor ecx, ecx
            mov cl, ah
            and eax, 0xff
            add edx, ecx
            mov ecx, dword ptr [esp + 0x14]
            add ecx, eax
            mov eax, dword ptr [esp + 0x20]
            mov dword ptr [esp + 0x1c], edx
            mov edx, dword ptr [esi + gTintedPolys+0x2c]
            sub eax, ebx
            mov dword ptr [esp + 0x14], ecx
            mov dword ptr [esp + 0x20], eax
            push eax
            mov eax, dword ptr [edx + 0x40]
            push edi
            push eax
            call BrPixelmapPixelGet
            mov ebx, dword ptr [esp + 0x28]
            mov edi, dword ptr [esp + 0x24]
            xor edx, edx
            mov dword ptr [esp + 0x1c], eax
            xor ecx, ecx
            mov dl, ah
            mov cl, byte ptr [esp + 0x1e]
            add ebx, edx
            mov edx, dword ptr [esp + 0x20]
            and eax, 0xff
            add edi, ecx
            mov ecx, dword ptr [esi + gTintedPolys+0x2c]
            add esp, 0xc
            add edx, eax
            mov eax, dword ptr [esp + 0x20]
            mov dword ptr [esp + 0x14], edx
            mov edx, dword ptr [ecx + 0x40]
            push eax
            push ebp
            push edx
            call BrPixelmapPixelGet
            mov dword ptr [esp + 0x1c], eax
            xor ecx, ecx
            mov cl, byte ptr [esp + 0x1e]
            add esp, 0xc
            add ecx, edi
            sar ecx, 2
            xor edx, edx
            mov edi, ecx
            mov dl, ah
            and eax, 0xff
            add edx, ebx
            sar edx, 2
            mov ebx, edx
            mov edx, dword ptr [esp + 0x14]
            add eax, edx
            sar eax, 2
            mov ebp, eax
            jmp L004d7eef
    L004d7edd:
            mov edi, dword ptr [gTintedColourMap_red]
            mov ebx, dword ptr [gTintedColourMap_grn]
            mov ebp, dword ptr [gTintedColourMap_blu]
    L004d7eef:
            call GetTotalTime
            mov edx, dword ptr [esi + gTintedPolys+0x14]
            mov ecx, dword ptr [esi + gTintedPolys+0xc]
            add edx, dword ptr [esi + gTintedPolys+0x10]
            add edx, ecx
            jne L004d7f4e
            mov dword ptr [esi + gTintedPolys+0xc], edi
            mov dword ptr [esi + gTintedPolys+0x10], ebx
            shl edi, 8
            or edi, ebx
            mov dword ptr [esi + gTintedPolys+0x14], ebp
            mov dword ptr [esi + gTintedPolys+0x3c], eax
            mov eax, dword ptr [esi + gTintedPolys+0x4]
            shl edi, 8
            or edi, ebp
            push 4
            mov dword ptr [eax + 8], edi
            mov ecx, dword ptr [esi + gTintedPolys+0x4]
            push ecx
            call BrMaterialUpdate
            add esp, 8
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x14
            ret
    L004d7f4e:
            sub eax, dword ptr [esi + gTintedPolys+0x3c]
            xor edx, edx
            shr eax, 0xc
            jns L004d7f5d
            xor eax, eax
    L004d7f5d:
            test eax, eax
            je L004d803b
            cmp ecx, edi
            je L004d7f9e
            jge L004d7f76
            lea edx, [eax + ecx]
            mov dword ptr [esi + gTintedPolys+0xc], edx
            jmp L004d7f7e
    L004d7f76:
            sub ecx, eax
            mov dword ptr [esi + gTintedPolys+0xc], ecx
    L004d7f7e:
            mov ecx, dword ptr [esi + gTintedPolys+0xc]
            mov edx, edi
            sub edx, eax
            cmp ecx, edx
            jle L004d7f99
            lea edx, [eax + edi]
            cmp ecx, edx
            jge L004d7f99
            mov dword ptr [esi + gTintedPolys+0xc], edi
    L004d7f99:
            mov edx, 1
    L004d7f9e:
            mov ecx, dword ptr [esi + gTintedPolys+0x10]
            cmp ecx, ebx
            je L004d7fd0
            jge L004d7fae
            add ecx, eax
            jmp L004d7fb0
    L004d7fae:
            sub ecx, eax
    L004d7fb0:
            mov edx, ebx
            mov dword ptr [esi + gTintedPolys+0x10], ecx
            sub edx, eax
            cmp ecx, edx
            jle L004d7fcb
            lea edx, [eax + ebx]
            cmp ecx, edx
            jge L004d7fcb
            mov dword ptr [esi + gTintedPolys+0x10], ebx
    L004d7fcb:
            mov edx, 1
    L004d7fd0:
            mov ecx, dword ptr [esi + gTintedPolys+0x14]
            cmp ecx, ebp
            je L004d8001
            jge L004d7fe0
            add ecx, eax
            jmp L004d7fe2
    L004d7fe0:
            sub ecx, eax
    L004d7fe2:
            mov edx, ebp
            mov dword ptr [esi + gTintedPolys+0x14], ecx
            sub edx, eax
            cmp ecx, edx
            jle L004d7ffc
            add eax, ebp
            cmp ecx, eax
            jge L004d7ffc
            mov dword ptr [esi + gTintedPolys+0x14], ebp
    L004d7ffc:
            mov edx, 1
    L004d8001:
            test edx, edx
            je L004d803b
            mov eax, dword ptr [esi + gTintedPolys+0xc]
            mov ebp, dword ptr [esi + gTintedPolys+0x10]
            mov edi, dword ptr [esi + gTintedPolys+0x14]
            mov ecx, dword ptr [esi + gTintedPolys+0x4]
            shl eax, 8
            or eax, ebp
            push 4
            shl eax, 8
            or eax, edi
            mov dword ptr [ecx + 8], eax
            mov edx, dword ptr [esi + gTintedPolys+0x4]
            push edx
            call BrMaterialUpdate
            add esp, 8
    L004d803b:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x14
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004d8220
void C2_HOOK_FASTCALL TurnTintedPolyOn(int pTintedIndex) {
    if (gTintedPolys[pTintedIndex].used) {
        gTintedPolys[pTintedIndex].actor->render_style = BR_RSTYLE_FACES;
        gTintedPolys[pTintedIndex].visible = 1;
    }
}

// FUNCTION: CARMA2_HW 0x004d8250
void C2_HOOK_FASTCALL TurnTintedPolyOff(int pTintedIndex) {

#ifdef CARPOCALYPSE2_FIX_BUGS
    if (pTintedIndex < 0) {
        return;
    }
#endif
    if (gTintedPolys[pTintedIndex].used) {
        (gTintedPolys[pTintedIndex].actor)->render_style = BR_RSTYLE_NONE;
        gTintedPolys[pTintedIndex].color_red = 0;
        gTintedPolys[pTintedIndex].color_grn = 0;
        gTintedPolys[pTintedIndex].color_blu = 0;
        gTintedPolys[pTintedIndex].visible = 0;
    }
}

// FUNCTION: CARMA2_HW 0x004d8290
void C2_HOOK_FASTCALL RenderTintedPolys(void) {
    BrZbsSceneRender(gTintedPolyCamera, gTintedPolyCamera, gBack_screen, gDepth_buffer);
}

// FUNCTION: CARMA2_HW 0x004d82b0
void C2_HOOK_FASTCALL SetTintedPolyColour(int pTintedIndex, int pRed, int pGreen, int pBlue) {
    int changed;

    changed = 0;
    if (gTintedPolys[pTintedIndex].color_red != (br_uint_32)pRed) {
        gTintedPolys[pTintedIndex].color_red = pRed;
        changed = 1;
    }
    if (gTintedPolys[pTintedIndex].color_grn != (br_uint_32)pGreen) {
        gTintedPolys[pTintedIndex].color_grn = pGreen;
        changed = 1;
    }
    if (gTintedPolys[pTintedIndex].color_blu != (br_uint_32)pBlue) {
        gTintedPolys[pTintedIndex].color_blu = pBlue;
        changed = 1;
    }
    if (changed) {
        (gTintedPolys[pTintedIndex].material)->colour = BR_COLOUR_RGB(gTintedPolys[pTintedIndex].color_red, gTintedPolys[pTintedIndex].color_grn, gTintedPolys[pTintedIndex].color_blu);
        BrMaterialUpdate(gTintedPolys[pTintedIndex].material ,BR_MATU_LIGHTING);
    }
}

void C2_HOOK_FASTCALL SetTintedPolyRefMaterial(int pTintedIndex, br_vector3* pPosition) {
    tSpecial_volume* volume;
    br_vector3 dir;
    br_vector3 nor;
    br_scalar t;
    br_material* mat;

    volume = gCar_to_view->collision_info->last_special_volume;
    if ((volume != NULL && volume->gravity_multiplier < 1.f) || gAction_replay_mode) {
        BrVector3Set(&dir, 0.f, 200.f, 0.f);
        DisablePlingMaterials();
        FindFace(pPosition, &dir, &nor, &t, &mat);
        EnablePlingMaterials();
        if (t >= 100.f || mat == NULL || mat->identifier == NULL || strlen(mat->identifier) == 0) {
            gTintedPolys[pTintedIndex].field_0x28 = 0;
            gTintedPolys[pTintedIndex].material2 = NULL;
        } else if (mat->identifier[0] == '!' || mat->identifier[0] == '#') {
            gTintedPolys[pTintedIndex].field_0x28 = '#';
            gTintedPolys[pTintedIndex].material2 = mat;
        } else if (mat->identifier[0] == '@') {
            gTintedPolys[pTintedIndex].field_0x28 = '@';
            gTintedPolys[pTintedIndex].material2 = mat;
        }
    } else {
        gTintedPolys[pTintedIndex].field_0x28 = 0;
        gTintedPolys[pTintedIndex].material2 = NULL;
    }
}

// FUNCTION: CARMA2_HW 0x004d8470
br_material* C2_HOOK_FASTCALL InWater(int pTintedIndex) {

    if (gTintedPolys[pTintedIndex].field_0x28 == '#') {
        return gTintedPolys[pTintedIndex].material2;
    }
    return NULL;
}

// FUNCTION: CARMA2_HW 0x004d8a20
void C2_HOOK_FASTCALL UpdateTintedPolys(void) {
    int i;
    br_uint_32 red = 0;
    br_uint_32 grn = 0;
    br_uint_32 blu = 0;

    /* The original inlined pulse path retains these colours between slots. */
    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gTintedPolys); i++) {
        if (!gTintedPolys[i].used) {
            continue;
        }
        switch (gTintedPolys[i].class) {
        case 2: {
            tSpecial_volume *volume = gCar_to_view->collision_info->last_special_volume;
            if ((volume != NULL && !(volume->gravity_multiplier >= 1.0f)) || gAction_replay_mode) {
                UpdateTintedPolyByMaterial(i);
            }
            break;
        }
        case 3:
            UpdatePulsePoly(i);
            break;
        case 4:
            if (PDKeyDown(32) || (gINT_006a0444 > 0 && gINT_006a0444 <= 200)) {
                UpdateFirePoly(i);
            }
            if (PDKeyDown(35) || (gINT_006a0444 > 0 && gINT_006a0444 <= 200)) {
                gINT_006a0440 = 1;
                UpdateFirePoly(i);
            }
            break;
        case 6:
            if (gTintedPolys[i].visible) {
                if (gTintedPolys[i].subClass == 0) {
                    unsigned int mask;
                    mask = GetTotalTime() >> (gTintedPolys[i].colour & 31);
                    if ((mask & 0x1ff) > 254) {
                        mask = -2 - (mask & 0x1ff);
                    }
                    mask &= 0xff;
                    red = gTintedPolys[i].color2_red & mask;
                    grn = gTintedPolys[i].color2_grn & mask;
                    blu = gTintedPolys[i].color2_blu & mask;
                } else {
                    if (gTintedPolys[i].subClass == 1) {
                        UpdatePulsePoly(i);
                    }
                }
                gTintedPolys[i].material->colour = BR_COLOUR_RGB(red, grn, blu);
                BrMaterialUpdate(gTintedPolys[i].material, BR_MATU_LIGHTING);
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x004d8bb0
void C2_NAKED C2_HOOK_FASTCALL SetTintedPolySize(int pTintedIndex, int x0, int y0, int width, int height) {
#ifndef CARPOCALYPSE2_MATCHING
    tTintedPoly* tinted;
    int nb_x;
    int nb_y;
    int step_x;
    int step_y;
    int i;
    int j;
    br_vertex* vertex;

    if (pTintedIndex == -1) {
        return;
    }
    tinted = &gTintedPolys[pTintedIndex];
    if (tinted->class == 3) {
        nb_x = 4;
        nb_y = 4;
    } else {
        nb_x = 1;
        nb_y = 1;
    }
    if (tinted->model == NULL) {
        return;
    }
    step_x = width / nb_x;
    step_y = height / nb_y;
    vertex = tinted->model->vertices;
    for (i = 0; i <= nb_y; i++) {
        int dy = i * step_y;
        for (j = 0; j <= nb_x; j++) {
            int dx = j * step_x;

            BrVector3Set(&vertex->p, (br_scalar)(x0 + dx), (br_scalar)(-(y0 + dy)), -1.02f);
            BrVector2Set(&vertex->map, (br_scalar)dx / (br_scalar)width, (br_scalar)dy / (br_scalar)height);
            vertex++;
        }
    }
    BrModelUpdate(tinted->model, BR_MODU_VERTICES);
#else
    __asm {
            sub esp, 0x1c
            cmp ecx, -1
            push ebx
            push ebp
            push esi
            push edi
            mov ebp, edx
            je L004d8cdb
            lea eax, [ecx + ecx*4]
            shl eax, 6
            add eax, ecx
            lea eax, [eax + eax*4]
            shl eax, 4
            cmp dword ptr [eax + gTintedPolys+0x38], 3
            jne L004d8be2
            mov esi, 4
            mov ebx, esi
            jmp L004d8be9
    L004d8be2:
            mov esi, 1
            mov ebx, esi
    L004d8be9:
            mov ecx, dword ptr [eax + gTintedPolys+0x8]
            test ecx, ecx
            mov dword ptr [esp + 0x28], ecx
            je L004d8cdb
            mov eax, dword ptr [esp + 0x34]
            mov ecx, dword ptr [ecx + 8]
            cdq
            idiv esi
            mov edi, eax
            mov eax, dword ptr [esp + 0x38]
            cdq
            idiv ebx
            lea edx, [ebx + 1]
            test edx, edx
            mov dword ptr [esp + 0x14], eax
            jle L004d8ccc
            mov ebx, dword ptr [esp + 0x30]
            lea eax, [esi + 1]
            mov esi, dword ptr [esp + 0x14]
            mov dword ptr [esp + 0x24], eax
            neg esi
            neg ebx
            mov dword ptr [esp + 0x10], 0
            mov dword ptr [esp + 0x20], esi
            mov dword ptr [esp + 0x30], ebx
            mov dword ptr [esp + 0x18], edx
    L004d8c44:
            test eax, eax
            jle L004d8caf
            fild dword ptr [esp + 0x10]
            fild dword ptr [esp + 0x38]
            fild dword ptr [esp + 0x30]
            fild dword ptr [esp + 0x34]
            fxch st(2)
            fdivp st(3), st(0)
            fxch st(2)
            xor edx, edx
            mov esi, eax
            mov dword ptr [esp + 0x30], edx
    L004d8c66:
            fild dword ptr [esp + 0x30]
            fld st(3)
            fxch st(1)
            fdiv st(0), st(3)
            lea eax, [ebp + edx]
            mov dword ptr [ecx + 8], 0xbf828f5c
            mov dword ptr [esp + 0x1c], eax
            add edx, edi
            add ecx, 0x28
            dec esi
            mov dword ptr [esp + 0x30], edx
            fild dword ptr [esp + 0x1c]
            fld st(3)
            fxch st(1)
            fstp dword ptr [ecx - 0x28]
            fxch st(2)
            fstp dword ptr [ecx - 0x24]
            fstp dword ptr [ecx - 0x1c]
            fstp dword ptr [ecx - 0x18]
            jne L004d8c66
            mov esi, dword ptr [esp + 0x20]
            mov eax, dword ptr [esp + 0x24]
            fstp st(0)
            fstp st(0)
            fstp st(0)
    L004d8caf:
            mov edx, dword ptr [esp + 0x14]
            add ebx, esi
            add dword ptr [esp + 0x10], edx
            mov edx, dword ptr [esp + 0x18]
            dec edx
            mov dword ptr [esp + 0x30], ebx
            mov dword ptr [esp + 0x18], edx
            jne L004d8c44
    L004d8ccc:
            mov eax, dword ptr [esp + 0x28]
            push 0x40
            push eax
            call BrModelUpdate
            add esp, 8
    L004d8cdb:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x1c
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
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004d8350
void C2_HOOK_FASTCALL SetTintedFromSpecialVolume(int pIndex, br_vector3* pPosition) {
    tSpecial_volume* volume;
    br_vector3 dir;
    br_vector3 nor;
    br_scalar t;
    br_material* mat;

    volume = gCar_to_view->collision_info->last_special_volume;
    if ((volume != NULL && volume->gravity_multiplier < 1.0f) || gAction_replay_mode) {
        BrVector3Set(&dir, 0.0f, 200.0f, 0.0f);
        DisablePlingMaterials();
        FindFace(pPosition, &dir, &nor, &t, &mat);
        EnablePlingMaterials();
        if (!(t < 100.0f) || mat == NULL || mat->identifier == NULL || strlen(mat->identifier) == 0) {
            gTintedPolys[pIndex].field_0x28 = 0;
            gTintedPolys[pIndex].material2 = NULL;
        } else if (mat->identifier[0] == '!' || mat->identifier[0] == '#') {
            gTintedPolys[pIndex].field_0x28 = '#';
            gTintedPolys[pIndex].material2 = mat;
        } else if (mat->identifier[0] == '@') {
            gTintedPolys[pIndex].field_0x28 = '@';
            gTintedPolys[pIndex].material2 = mat;
        } else {
            gTintedPolys[pIndex].field_0x28 = 0;
            gTintedPolys[pIndex].material2 = NULL;
        }
    } else {
        gTintedPolys[pIndex].field_0x28 = 0;
        gTintedPolys[pIndex].material2 = NULL;
    }
}

// FUNCTION: CARMA2_HW 0x004d84a0
void C2_HOOK_FASTCALL ProcessTintedPoly(int pIndex) {

    if (gTintedPolys[pIndex].used) {
        switch (gTintedPolys[pIndex].class) {
        case 2: {
            tSpecial_volume *volume = gCar_to_view->collision_info->last_special_volume;
            if ((volume != NULL && volume->gravity_multiplier < 1.0f) || gAction_replay_mode) {
                UpdateTintedPolyByMaterial(pIndex);
                return;
            }
            break;
        }
        case 3:
            UpdatePulsePoly(pIndex);
            return;
        case 4:
            if (PDKeyDown(32) || (gINT_006a0444 > 0 && gINT_006a0444 <= 200)) {
                UpdateFirePoly(pIndex);
            }
            if (PDKeyDown(35) || (gINT_006a0444 > 0 && gINT_006a0444 <= 200)) {
                gINT_006a0448 = 1;
                UpdateFirePoly(pIndex);
                return;
            }
            break;
        case 6:
            if (gTintedPolys[pIndex].visible) {
                unsigned int red;
                unsigned int grn;
                unsigned int blu;
                unsigned int value;

                switch (gTintedPolys[pIndex].subClass) {
                case 1:
                    UpdatePulsePoly(pIndex);
                    /* fall through */
                default:
                    red = grn = blu = pIndex;
                    break;
                case 0:
                    value = (GetTotalTime() >> gTintedPolys[pIndex].colour) & 0x1ff;
                    if (value < 255) {
                        value &= 0xff;
                    } else {
                        value = (-2 - value) & 0xff;
                    }
                    red = gTintedPolys[pIndex].color2_red & value;
                    grn = gTintedPolys[pIndex].color2_grn & value;
                    blu = gTintedPolys[pIndex].color2_blu & value;
                    break;
                }
                gTintedPolys[pIndex].material->colour = BR_COLOUR_RGB(red, grn, blu);
                BrMaterialUpdate(gTintedPolys[pIndex].material, BR_MATU_LIGHTING);
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x004d8050
void C2_NAKED C2_HOOK_FASTCALL UpdatePolyVertexColours(int pTintedIndex) {
#ifndef CARPOCALYPSE2_MATCHING
    br_uint_32 maximum;
    double scale;
    double tint_red;
    double tint_grn;
    double tint_blu;
    int i;

    if (gTinted_poly_palette == NULL || pTintedIndex == -1) {
        return;
    }
    maximum = (br_int_32)gTintedPolys[pTintedIndex].color_red > (br_int_32)gTintedPolys[pTintedIndex].color_grn
        ? gTintedPolys[pTintedIndex].color_red : gTintedPolys[pTintedIndex].color_grn;
    if (maximum <= gTintedPolys[pTintedIndex].color_blu) {
        maximum = gTintedPolys[pTintedIndex].color_blu;
    }
    scale = 50.0 / maximum;
    tint_red = 0.5 - ((br_int_32)gTintedPolys[pTintedIndex].color_red * scale) * -0.01;
    tint_grn = 0.5 - ((br_int_32)gTintedPolys[pTintedIndex].color_grn * scale) * -0.01;
    tint_blu = 0.5 - ((br_int_32)gTintedPolys[pTintedIndex].color_blu * scale) * -0.01;
    for (i = 1; i < 256; i++) {
        br_colour colour = gTinted_poly_palette[i];
        br_uint_32 red = CARPOCALYPSE2_RGB888_R(colour);
        br_uint_32 grn = CARPOCALYPSE2_RGB888_G(colour);
        br_uint_32 blu = CARPOCALYPSE2_RGB888_B(colour);
        br_uint_32 half_brightness = ((red + grn + blu) / 3) >> 1;

        if (2 * half_brightness != 0) {
            double amount = (100.0 / (2 * half_brightness)) * half_brightness * 0.01;
            br_uint_32 new_grn = (br_uint_32)(br_int_64)((half_brightness + grn) * tint_grn * amount) & 0xff;
            br_uint_32 new_red = (br_uint_32)(br_int_64)((half_brightness + red) * tint_red * amount) & 0xff;
            br_uint_32 new_blu = (br_uint_32)(br_int_64)((half_brightness + blu) * tint_blu * amount) & 0xff;

            ((br_colour*)gRender_palette->pixels)[i] = BR_COLOUR_RGB(new_red, new_grn, new_blu);
        }
    }
#else
    __asm {
            mov eax, dword ptr [gTintedPolyCamera+0x4]
            sub esp, 0x30
            push ebx
            xor ebx, ebx
            cmp eax, ebx
            je L004d8216
            cmp ecx, -1
            je L004d8216
            lea eax, [ecx + ecx*4]
            push edi
            shl eax, 6
            add eax, ecx
            push esi
            push ebp
            lea eax, [eax + eax*4]
            shl eax, 4
            mov ecx, dword ptr [eax + gTintedPolys+0xc]
            mov edx, dword ptr [eax + gTintedPolys+0x10]
            cmp ecx, edx
            mov dword ptr [esp + 0x10], ecx
            mov dword ptr [esp + 0x14], edx
            jg L004d8097
            mov ecx, edx
    L004d8097:
            mov eax, dword ptr [eax + gTintedPolys+0x14]
            cmp ecx, eax
            mov dword ptr [esp + 0x1c], eax
            ja L004d80a7
            mov ecx, eax
    L004d80a7:
            mov dword ptr [esp + 0x20], ecx
            mov dword ptr [esp + 0x24], ebx
            fild qword ptr [esp + 0x20]
            fild dword ptr [esp + 0x10]
            fild dword ptr [esp + 0x14]
            fxch st(2)
            fdivr qword ptr [gConst_average_ped_scale_factor+0x8]
            mov dword ptr [esp + 0x10], 4
            fild dword ptr [esp + 0x1c]
            fxch st(2)
            fmul st(0), st(1)
            fxch st(3)
            fmul st(0), st(1)
            fxch st(2)
            fmul st(0), st(1)
            fxch st(3)
            fmul qword ptr [gConst_average_ped_scale_factor+0x10]
            fxch st(2)
            fmul qword ptr [gConst_average_ped_scale_factor+0x10]
            fxch st(2)
            fsubr qword ptr [gConst_average_ped_scale_factor+0x18]
            fxch st(3)
            fmul qword ptr [gConst_average_ped_scale_factor+0x10]
            fxch st(2)
            fsubr qword ptr [gConst_average_ped_scale_factor+0x18]
            fxch st(3)
            fstp st(1)
            fxch st(1)
            fsubr qword ptr [gConst_average_ped_scale_factor+0x18]
            fxch st(1)
            fxch st(2)
            fxch st(1)
    L004d8115:
            mov eax, dword ptr [gTintedPolyCamera+0x4]
            mov ecx, dword ptr [esp + 0x10]
            xor edx, edx
            mov eax, dword ptr [eax + ecx]
            xor ecx, ecx
            mov dword ptr [esp + 0x1c], eax
            mov cl, ah
            mov dl, byte ptr [esp + 0x1e]
            and eax, 0xff
            mov ebp, eax
            mov edi, edx
            mov eax, 0xaaaaaaab
            lea edx, [ebp + ecx]
            add edx, edi
            mul edx
            shr edx, 1
            mov esi, edx
            shr esi, 1
            and esi, 0xff
            lea eax, [esi + esi]
            cmp eax, ebx
            je L004d81f7
            mov dword ptr [esp + 0x20], eax
            mov dword ptr [esp + 0x24], ebx
            fild qword ptr [esp + 0x20]
            mov dword ptr [esp + 0x14], esi
            mov dword ptr [esp + 0x18], ebx
            lea eax, [esi + ecx]
            mov dword ptr [esp + 0x2c], ebx
            fdivr qword ptr [gConst_average_ped_scale_factor+0x20]
            mov dword ptr [esp + 0x28], eax
            fimul dword ptr [esp + 0x14]
            fmul qword ptr [gConst_average_ped_scale_factor+0x28]
            fild qword ptr [esp + 0x28]
            fmul st(0), st(3)
            fmul st(0), st(1)
            call _ftol
            lea ecx, [esi + edi]
            mov dword ptr [esp + 0x34], ebx
            mov dword ptr [esp + 0x30], ecx
            xor edx, edx
            fild qword ptr [esp + 0x30]
            mov dl, al
            mov dword ptr [esp + 0x1c], edx
            fmul st(0), st(4)
            fmul st(0), st(1)
            call _ftol
            add esi, ebp
            mov dword ptr [esp + 0x3c], 0
            mov dword ptr [esp + 0x38], esi
            mov ebx, dword ptr [esp + 0x1c]
            fild qword ptr [esp + 0x38]
            mov bh, al
            shl ebx, 8
            fmul st(0), st(2)
            fmul st(0), st(1)
            call _ftol
            mov edx, dword ptr [gRender_palette]
            mov ecx, dword ptr [esp + 0x10]
            and eax, 0xff
            or ebx, eax
            mov eax, dword ptr [edx + 8]
            fstp st(0)
            mov dword ptr [eax + ecx], ebx
            xor ebx, ebx
    L004d81f7:
            mov eax, dword ptr [esp + 0x10]
            add eax, 4
            cmp eax, 0x400
            mov dword ptr [esp + 0x10], eax
            jl L004d8115
            fstp st(0)
            fstp st(0)
            pop ebp
            pop esi
            fstp st(0)
            pop edi
    L004d8216:
            pop ebx
            add esp, 0x30
            ret
    }
#endif
}

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x004d8630
void C2_HOOK_FASTCALL UpdatePulsePoly(int pIndex) {
    int i;
    int new_color;

    if (gTintedPolys[pIndex].actor->render_style == BR_RSTYLE_FACES) {
        new_color = GetTotalTime() >> gTintedPolys[pIndex].colour;
        for (i = 0; i < gTintedPolys[pIndex].model->nvertices; i++) {
            gTintedPolys[pIndex].model->vertices[i].red = new_color + 128;
            gTintedPolys[pIndex].model->vertices[i].grn = new_color + 30 * i;
            gTintedPolys[pIndex].model->vertices[i].blu = new_color;
        }
        BrModelUpdate(gTintedPolys[pIndex].model, BR_MODU_VERTICES);
    }
}
#pragma auto_inline(on)

// FUNCTION: CARMA2_HW 0x004d86e0
void C2_NAKED C2_HOOK_FASTCALL UpdateFirePoly(int pIndex) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;
    int j;

    if (!gINT_006a0444) {
        C2_HOOK_BUG_ON(sizeof(gTintedPolys[pIndex].tints2) != 8192);
        C2_HOOK_BUG_ON(sizeof(gTintedPolys[pIndex].tints3) != 8192);

        memset(gTintedPolys[pIndex].tints2, 0, sizeof(gTintedPolys[pIndex].tints2));
        memset(gTintedPolys[pIndex].tints3, 0, sizeof(gTintedPolys[pIndex].tints2));
    }

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 2; j++) {
            gTintedPolys[pIndex].tints2[(IRandomBetween(-(5 - i), 5 - i) + IRandomBetween(13, 19)) * 64 + i + 64 - 5] = 255 - gINT_006a044c;
        }
    }
    if (gINT_006a0444 > 150) {
        gINT_006a044c += 1;
    }
    gINT_006a0440 += 1;
    if (gINT_006a0440 >= 64) {
        gINT_006a0440 = 0;
    }

    for (i = 1; i < 64 - 1; i++) {
        for (j = 1; j < 32 - 1; j++) {
            int avg =
                ((gTintedPolys[pIndex].tints2[(j + 1) * 64 + i]
                + gTintedPolys[pIndex].tints2[(j - 1) * 64 + i]
                + gTintedPolys[pIndex].tints2[j * 64 + i + 1]
                + gTintedPolys[pIndex].tints2[j * 64 + i - 1]) / 4
                + gTintedPolys[pIndex].tints2[j * 64 + i]) / 2
            - gTintedPolys[pIndex].tints1[j * 64 + i]
            - gINT_006a044c;
            gTintedPolys[pIndex].tints3[j * 64 + i + 1] = MAX(0, avg);
        }
    }

    if (gINT_006a0444 < 150 && IRandomBetween(0, 6) > 3) {
        int idx;

        idx = IRandomBetween(12, 20) * 64 + IRandomBetween(30,55);
        gTintedPolys[pIndex].tints2[idx] = 0xff;
        gTintedPolys[pIndex].tints3[idx] = 0xbf;
    }
    memcpy(gTintedPolys[pIndex].tints2, gTintedPolys[pIndex].tints3, sizeof(gTintedPolys[pIndex].tints2));

    for (i = 0; i < 64; i++) {
        for (j = 0; j < 32; j++) {
            int idx = gTintedPolys[pIndex].tints3[j * 64 + i];
            BrPixelmapPixelSet(gTintedPolys[pIndex].pixelmap, i, j, gTintedPolys[pIndex].colours[idx]);
        }
    }
    BrMapUpdate(gTintedPolys[pIndex].pixelmap, BR_MAPU_ALL);

    if (gINT_006a0444 % 10 == 0 && gINT_006a0448 && gINT_006a0444 > 10) {
        tPath_name save_path;
        for (i = 1; i < 10000; i++) {
            tPath_name path;
            FILE* f;

            PathCat(path, gApplication_path, "FLM");
            sprintf(&path[strlen(path)], "%02d.%s", i, "PIX");
            f = DRfopen(path, "rt");
            if (f == NULL) {
                sprintf(save_path, "%s", path);
                break;
            }
        }
        BrPixelmapSave(save_path, gTintedPolys[pIndex].pixelmap);
    }
    gINT_006a0444 += 1;
    if (gINT_006a0444 >= 200) {
        gINT_006a0444 = 0;
        gINT_006a0448 = 0;
        gINT_006a044c = 0;
    }
#else
    __asm {
            sub esp, 0x210
            mov eax, dword ptr [gINT_006a0444]
            push ebx
            push ebp
            mov edx, ecx
            push esi
            push edi
            test eax, eax
            mov dword ptr [esp + 0x1c], edx
            jne L004d8723
            lea eax, [edx + edx*4]
            mov ecx, 0x800
            shl eax, 6
            add eax, edx
            lea esi, [eax + eax*4]
            shl esi, 4
            xor eax, eax
            lea edi, [esi + gTintedPolys+0x2448]
            rep stosd
            mov ecx, 0x800
            lea edi, [esi + gTintedPolys+0x4448]
            rep stosd
    L004d8723:
            lea eax, [edx + edx*4]
            xor edi, edi
            shl eax, 6
            add eax, edx
            mov ebx, 5
            lea eax, [eax + eax*4]
            shl eax, 2
            mov dword ptr [esp + 0x14], eax
    L004d873c:
            mov dword ptr [esp + 0x10], 2
    L004d8744:
            mov edx, dword ptr [gINT_006a044c]
            mov esi, 0xff
            sub esi, edx
            mov edx, ebx
            lea ecx, [edi - 5]
            call IRandomBetween
            mov edx, 0x13
            mov ecx, 0xd
            mov ebp, eax
            call IRandomBetween
            add ebp, eax
            shl ebp, 6
            add ebp, edi
            mov dword ptr [esp + 0x18], ebp
            mov ebp, dword ptr [esp + 0x14]
            mov eax, dword ptr [esp + 0x18]
            add eax, ebp
            mov dword ptr [eax*4 + gTintedPolys+0x2534], esi
            mov eax, dword ptr [esp + 0x10]
            dec eax
            mov dword ptr [esp + 0x10], eax
            jne L004d8744
            inc edi
            dec ebx
            cmp ebx, 1
            jg L004d873c
            mov eax, dword ptr [gINT_006a0444]
            mov edi, dword ptr [gINT_006a044c]
            cmp eax, 0x96
            jle L004d87b3
            inc edi
            mov dword ptr [gINT_006a044c], edi
    L004d87b3:
            mov eax, dword ptr [gINT_006a0440]
            inc eax
            cmp eax, 0x40
            mov dword ptr [gINT_006a0440], eax
            jl L004d87cd
            mov dword ptr [gINT_006a0440], 0
    L004d87cd:
            mov ecx, dword ptr [esp + 0x1c]
            mov dword ptr [esp + 0x10], 0x3e
            lea eax, [ecx + ecx*4]
            shl eax, 6
            add eax, ecx
            lea ebx, [eax + eax*4]
            shl ebx, 4
            mov dword ptr [esp + 0x14], ebx
            lea esi, [ebx + gTintedPolys+0x2548]
    L004d87f1:
            mov ecx, esi
            mov edx, 0x1e
    L004d87f8:
            mov eax, dword ptr [ecx + 0x104]
            add eax, dword ptr [ecx - 0xfc]
            add eax, dword ptr [ecx + 8]
            add eax, dword ptr [ecx]
            shr eax, 2
            add eax, dword ptr [ecx + 4]
            shr eax, 1
            sub eax, dword ptr [ecx - 0x1ffc]
            sub eax, edi
            test eax, eax
            jle L004d8825
            mov dword ptr [ecx + 0x2000], eax
            jmp L004d882f
    L004d8825:
            mov dword ptr [ecx + 0x2000], 0
    L004d882f:
            add ecx, 0x100
            dec edx
            jne L004d87f8
            mov eax, dword ptr [esp + 0x10]
            add esi, 4
            dec eax
            mov dword ptr [esp + 0x10], eax
            jne L004d87f1
            cmp dword ptr [gINT_006a0444], 0x96
            jge L004d88a1
            mov edx, 6
            xor ecx, ecx
            call IRandomBetween
            cmp eax, 3
            jle L004d88a1
            mov edx, 0x14
            mov ecx, 0xc
            call IRandomBetween
            mov edx, 0x37
            mov ecx, 0x1e
            mov esi, eax
            call IRandomBetween
            shl esi, 6
            add esi, eax
            add esi, ebp
            shl esi, 2
            mov dword ptr [esi + gTintedPolys+0x2448], 0xff
            mov dword ptr [esi + gTintedPolys+0x4448], 0xbf
    L004d88a1:
            mov ecx, 0x800
            lea esi, [ebx + gTintedPolys+0x4448]
            lea edi, [ebx + gTintedPolys+0x2448]
            xor ebx, ebx
            rep movsd
            lea ecx, [ebp*4 + gTintedPolys+0x4448]
            mov dword ptr [esp + 0x10], ecx
    L004d88c1:
            mov edi, dword ptr [esp + 0x10]
            xor esi, esi
    L004d88c7:
            mov edx, dword ptr [edi]
            mov ecx, dword ptr [esp + 0x14]
            add edx, ebp
            mov eax, dword ptr [edx*4 + gTintedPolys+0x48]
            mov edx, dword ptr [ecx + gTintedPolys+0x44]
            push eax
            push ebx
            push esi
            push edx
            call BrPixelmapPixelSet
            add esp, 0x10
            inc esi
            add edi, 0x100
            cmp esi, 0x20
            jl L004d88c7
            mov ecx, dword ptr [esp + 0x10]
            inc ebx
            add ecx, 4
            cmp ebx, 0x40
            mov dword ptr [esp + 0x10], ecx
            jl L004d88c1
            mov ebp, dword ptr [esp + 0x14]
            push 0xfff
            mov eax, dword ptr [ebp + gTintedPolys+0x44]
            push eax
            call BrMapUpdate
            mov ecx, dword ptr [gINT_006a0444]
            mov esi, 0xa
            mov eax, ecx
            add esp, 8
            cdq
            idiv esi
            test edx, edx
            jne L004d89e9
            mov eax, dword ptr [gINT_006a0448]
            xor ebx, ebx
            cmp eax, ebx
            je L004d89eb
            cmp ecx, esi
            jle L004d89eb
            mov esi, 1
    L004d8951:
            push offset gDefaultOpacity_TintedPoly+0x80
            mov edx, offset gApplication_path
            lea ecx, [esp + 0x24]
            call PathCat
            lea edi, [esp + 0x20]
            or ecx, 0xffffffff
            xor eax, eax
            push offset gDefaultOpacity_TintedPoly+0x7c
            repne scasb
            not ecx
            dec ecx
            push esi
            push offset gDefaultOpacity_TintedPoly+0x74
            lea ecx, [esp + ecx + 0x2c]
            push ecx
            call sprintf
            add esp, 0x10
            mov edx, offset gNext_crush_actor_id+0x180
            lea ecx, [esp + 0x20]
            call DRfopen
            cmp eax, ebx
            je L004d89b0
            push eax
            call fclose
            add esp, 4
            inc esi
            cmp esi, 0x2710
            jl L004d8951
            jmp L004d89ca
    L004d89b0:
            lea edx, [esp + 0x20]
            lea eax, [esp + 0x120]
            push edx
            push offset gCancel_recover+0xbc
            push eax
            call sprintf
            add esp, 0xc
    L004d89ca:
            mov ecx, dword ptr [ebp + gTintedPolys+0x44]
            lea edx, [esp + 0x120]
            push ecx
            push edx
            call BrPixelmapSave
            mov ecx, dword ptr [gINT_006a0444]
            add esp, 8
            jmp L004d89eb
    L004d89e9:
            xor ebx, ebx
    L004d89eb:
            inc ecx
            cmp ecx, 0xc8
            mov dword ptr [gINT_006a0444], ecx
            jl L004d8a0c
            mov dword ptr [gINT_006a0444], ebx
            mov dword ptr [gINT_006a0448], ebx
            mov dword ptr [gINT_006a044c], ebx
    L004d8a0c:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x210
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

// FUNCTION: CARMA2_HW 0x004d8cf0
int C2_HOOK_FASTCALL TintedPolyIsOn(int pIndex) {

    return gTintedPolys[pIndex].visible;
}
