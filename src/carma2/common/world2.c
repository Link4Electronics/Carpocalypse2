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

// GLOBAL: CARMA2_HW 0x00591378
tPMFMCB* gWall_texturing_tweakers[3][3] = {
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

#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_CDECL BrMaterialUpdate();
extern tBrender_storage gNet_cars_storage_space;
extern tBrender_storage gOur_car_storage_space;
extern tBrender_storage gTheir_cars_storage_space;
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern br_material* C2_HOOK_CDECL BrMaterialAdd();
extern br_material* C2_HOOK_CDECL BrMaterialAllocate();
extern br_material* C2_HOOK_CDECL BrMaterialFind();
extern void* C2_HOOK_CDECL BrMemAllocate();
extern void C2_HOOK_CDECL BrMemFree();
extern br_uint_32 C2_HOOK_CDECL BrPixelmapPixelGet();
extern void C2_HOOK_FASTCALL CopyBrMaterial(br_material* pDestination, const br_material* pSource);
extern int C2_HOOK_FASTCALL FUN_00447ef0(br_model* pModel, tU16 pIndex);
extern float gCamera_to_horiz_angle;
extern int gHud_actor_storage_size;
extern int gHud_tinted4;
#endif
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

// FUNCTION: CARMA2_HW 0x004e5f10
int C2_NAKED C2_HOOK_FASTCALL FUN_004e5f10(int pRed, tU32 pGreen, tU32 pBlue) {
#ifndef CARPOCALYPSE2_MATCHING
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
#else
    __asm {
            sub esp, 0x10
            mov eax, edx
            push ebx
            mov edx, dword ptr [gRender_palette]
            push ebp
            shl ecx, 8
            mov edx, dword ptr [edx + 8]
            or ecx, eax
            mov eax, dword ptr [esp + 0x1c]
            push esi
            shl ecx, 8
            or ecx, eax
            mov eax, dword ptr [edx + 4]
            mov ebx, ecx
            push edi
            and ebx, 0xff
            mov dword ptr [esp + 0x24], eax
            mov ebp, ecx
            and eax, 0xff
            mov edi, ebx
            mov dword ptr [esp + 0x18], 1
            shr ebp, 0x10
            shr ecx, 8
            sub edi, eax
            and ebp, 0xff
            xor eax, eax
            and ecx, 0xff
            mov al, byte ptr [esp + 0x26]
            mov esi, ebp
            mov dword ptr [esp + 0x14], ecx
            xor ecx, ecx
            mov cl, byte ptr [esp + 0x25]
            sub esi, eax
            mov eax, dword ptr [esp + 0x14]
            imul edi, edi
            sub eax, ecx
            mov ecx, esi
            imul ecx, esi
            add edi, ecx
            mov ecx, eax
            imul ecx, eax
            add edi, ecx
            add edx, 8
            mov dword ptr [esp + 0x24], 2
            mov dword ptr [esp + 0x10], edx
    L004e5f9e:
            mov edx, dword ptr [esp + 0x10]
            mov ecx, ebx
            mov esi, ebp
            mov edx, dword ptr [edx]
            mov eax, edx
            mov dword ptr [esp + 0x1c], edx
            and eax, 0xff
            sub ecx, eax
            xor eax, eax
            mov al, byte ptr [esp + 0x1e]
            imul ecx, ecx
            sub esi, eax
            xor eax, eax
            mov al, dh
            mov edx, dword ptr [esp + 0x14]
            sub edx, eax
            mov eax, esi
            imul eax, esi
            add ecx, eax
            mov eax, edx
            imul eax, edx
            add ecx, eax
            cmp ecx, edi
            jae L004e5fe6
            mov edi, ecx
            mov ecx, dword ptr [esp + 0x24]
            mov dword ptr [esp + 0x18], ecx
    L004e5fe6:
            mov eax, dword ptr [esp + 0x24]
            mov edx, dword ptr [esp + 0x10]
            inc eax
            add edx, 4
            cmp eax, 0x100
            mov dword ptr [esp + 0x24], eax
            mov dword ptr [esp + 0x10], edx
            jl L004e5f9e
            mov eax, dword ptr [esp + 0x18]
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x10
            ret 4
            nop
            sub esp, 0x1c
            push ebx
            push ebp
            push esi
            xor esi, esi
            mov eax, ecx
            xor ebp, ebp
            mov ecx, dword ptr [esp + 0x2c]
            xor ebx, ebx
            cmp edx, esi
            push edi
            mov dword ptr [esp + 0x20], edx
            mov dword ptr [esp + 0x24], eax
            mov dword ptr [esp + 0x14], esi
            je L004e6244
            cmp ecx, esi
            je L004e6244
            xor edi, edi
            mov di, word ptr [eax + 0x34]
            dec edi
            cmp edx, edi
            je L004e6244
            xor edi, edi
            mov di, word ptr [eax + 0x36]
            dec edi
            cmp ecx, edi
            je L004e6244
            cmp dword ptr [gHud_tinted4+0x4], esi
            je L004e60a3
            mov eax, dword ptr [gRender_palette]
            mov esi, dword ptr [eax + 8]
            mov eax, offset gHud_actor_storage_size+0x1c
    L004e6072:
            xor edx, edx
            add eax, 0x10
            mov dl, byte ptr [esi + 2]
            add esi, 4
            mov dword ptr [eax - 0x14], edx
            xor edx, edx
            mov dl, byte ptr [esi - 3]
            mov dword ptr [eax - 0x10], edx
            mov edx, dword ptr [esi - 4]
            and edx, 0xff
            cmp eax, offset gCamera_to_horiz_angle
            mov dword ptr [eax - 0xc], edx
            jb L004e6072
            xor esi, esi
            mov dword ptr [gHud_tinted4+0x4], esi
    L004e60a3:
            mov edi, offset gHud_tinted4+0xc
            mov dword ptr [esp + 0x10], esi
            mov dword ptr [esp + 0x18], edi
            jmp L004e60b6
    L004e60b2:
            mov ecx, dword ptr [esp + 0x30]
    L004e60b6:
            mov eax, dword ptr [esp + 0x10]
            xor esi, esi
            lea ecx, [eax + ecx - 1]
            mov dword ptr [esp + 0x1c], ecx
    L004e60c4:
            mov eax, dword ptr [esp + 0x20]
            mov edx, dword ptr [esp + 0x1c]
            push edx
            mov edx, dword ptr [esp + 0x28]
            lea ecx, [esi + eax - 1]
            push ecx
            push edx
            call BrPixelmapPixelGet
            mov byte ptr [esp + 0x34], al
            mov ecx, dword ptr [edi]
            mov eax, dword ptr [esp + 0x34]
            add esp, 0xc
            and eax, 0xff
            add edi, 0xc
            shl eax, 4
            mov edx, dword ptr [eax + gHud_actor_storage_size+0x18]
            imul edx, ecx
            add ebp, edx
            mov edx, dword ptr [eax + gHud_actor_storage_size+0x1c]
            mov eax, dword ptr [eax + gHud_actor_storage_size+0x20]
            imul edx, ecx
            imul eax, ecx
            mov ecx, dword ptr [esp + 0x14]
            add ebx, edx
            add ecx, eax
            inc esi
            cmp esi, 3
            mov dword ptr [esp + 0x14], ecx
            jb L004e60c4
            mov edi, dword ptr [esp + 0x18]
            mov edx, dword ptr [esp + 0x10]
            add edi, 4
            inc edx
            cmp edi, offset gHud_tinted4+0x18
            mov dword ptr [esp + 0x10], edx
            mov dword ptr [esp + 0x18], edi
            jb L004e60b2
            mov eax, dword ptr [esp + 0x14]
            mov ecx, dword ptr [gRender_palette]
            and ebp, 0xffffffe0
            and ebx, 0xffffffe0
            mov edx, dword ptr [ecx + 8]
            mov dword ptr [esp + 0x10], 1
            shl ebp, 8
            mov ecx, dword ptr [edx + 4]
            or ebx, ebp
            shl ebx, 3
            shr eax, 5
            or ebx, eax
            mov dword ptr [esp + 0x30], ecx
            mov eax, ebx
            mov ebp, ebx
            and eax, 0xff
            and ecx, 0xff
            mov edi, eax
            shr ebp, 0x10
            shr ebx, 8
            sub edi, ecx
            and ebp, 0xff
            xor ecx, ecx
            and ebx, 0xff
            mov cl, byte ptr [esp + 0x32]
            mov esi, ebp
            mov dword ptr [esp + 0x18], ebx
            xor ebx, ebx
            mov bl, byte ptr [esp + 0x31]
            sub esi, ecx
            mov ecx, dword ptr [esp + 0x18]
            imul edi, edi
            sub ecx, ebx
            mov ebx, esi
            imul ebx, esi
            mov esi, ecx
            add edi, ebx
            imul esi, ecx
            add edi, esi
            add edx, 8
            mov dword ptr [esp + 0x30], 2
            mov dword ptr [esp + 0x14], edx
    L004e61d1:
            mov edx, dword ptr [esp + 0x14]
            mov ecx, eax
            xor ebx, ebx
            mov edx, dword ptr [edx]
            mov esi, edx
            mov dword ptr [esp + 0x28], edx
            mov bl, byte ptr [esp + 0x2a]
            and esi, 0xff
            sub ecx, esi
            mov esi, ebp
            sub esi, ebx
            xor ebx, ebx
            mov bl, dh
            mov edx, dword ptr [esp + 0x18]
            sub edx, ebx
            mov ebx, esi
            imul ebx, esi
            imul ecx, ecx
            mov esi, edx
            add ecx, ebx
            imul esi, edx
            add ecx, esi
            cmp ecx, edi
            jae L004e621a
            mov edi, ecx
            mov ecx, dword ptr [esp + 0x30]
            mov dword ptr [esp + 0x10], ecx
    L004e621a:
            mov ecx, dword ptr [esp + 0x30]
            mov esi, dword ptr [esp + 0x14]
            inc ecx
            add esi, 4
            cmp ecx, 0x100
            mov dword ptr [esp + 0x30], ecx
            mov dword ptr [esp + 0x14], esi
            jl L004e61d1
            mov eax, dword ptr [esp + 0x10]
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x1c
            ret 4
    L004e6244:
            push ecx
            push edx
            push eax
            call BrPixelmapPixelGet
            add esp, 0xc
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x1c
            ret 4
    }
#endif
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
void C2_NAKED C2_HOOK_FASTCALL ProcessModelFaceMaterials2(br_model* pModel, material_cbfn* pCallback) {
#ifndef CARPOCALYPSE2_MATCHING
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
#else
    __asm {
            push ecx
            push ebx
            mov ebx, ecx
            push ebp
            push esi
            mov eax, dword ptr [ebx + 0xc]
            push edi
            test eax, eax
            mov dword ptr [esp + 0x10], edx
            jne L0044903b
            mov ecx, dword ptr [ebx + 0x4c]
            test ecx, ecx
            je L00449037
            xor ebp, ebp
            cmp word ptr [ecx + 8], bp
            jbe L00449063
    L00448ff1:
            mov eax, ebp
            xor edi, edi
            and eax, 0xffff
            lea esi, [eax + eax*8]
            mov eax, dword ptr [ecx + 0x18]
            shl esi, 2
            add eax, esi
            cmp word ptr [eax + 0x1c], di
            jbe L00449027
    L0044900b:
            mov ecx, dword ptr [eax + 8]
            mov ecx, dword ptr [ecx]
            test ecx, ecx
            je L00449018
            call dword ptr [esp + 0x10]
    L00449018:
            mov edx, dword ptr [ebx + 0x4c]
            inc edi
            mov eax, dword ptr [edx + 0x18]
            add eax, esi
            cmp di, word ptr [eax + 0x1c]
            jb L0044900b
    L00449027:
            mov ecx, dword ptr [ebx + 0x4c]
            inc ebp
            cmp bp, word ptr [ecx + 8]
            jb L00448ff1
            pop edi
            pop esi
            pop ebp
            pop ebx
            pop ecx
            ret
    L00449037:
            test eax, eax
            je L00449063
    L0044903b:
            xor esi, esi
            cmp word ptr [ebx + 0x12], si
            jbe L00449063
    L00449043:
            mov edx, dword ptr [ebx + 0xc]
            mov eax, esi
            and eax, 0xffff
            lea ecx, [eax + eax*4]
            mov ecx, dword ptr [edx + ecx*8 + 8]
            test ecx, ecx
            je L0044905c
            call dword ptr [esp + 0x10]
    L0044905c:
            inc esi
            cmp si, word ptr [ebx + 0x12]
            jb L00449043
    L00449063:
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
    }
#endif
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
void C2_NAKED C2_HOOK_FASTCALL SetCarStorageTexturingLevel(tBrender_storage* pStorage, tCar_texturing_level pNew, tCar_texturing_level pOld) {
#ifndef CARPOCALYPSE2_MATCHING

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
#else
    __asm {
            push ecx
            push ebx
            push ebp
            mov eax, edx
            xor ebp, ebp
            push esi
            sub eax, ebp
            push edi
            mov esi, ecx
            je L00447543
            dec eax
            je L004473ca
            dec eax
            jne L004475af
            mov eax, dword ptr [esi + 8]
            xor edi, edi
            cmp eax, ebp
            jle L004475af
            mov ebx, 2
    L0044737f:
            mov eax, dword ptr [esi + 0x40]
            mov eax, dword ptr [eax + edi*4]
            cmp eax, ebp
            je L004473ba
            mov ecx, dword ptr [esi + 0x38]
            push 0x7fff
            mov edx, dword ptr [ecx + edi*4]
            mov dword ptr [edx + 0x40], eax
            mov eax, dword ptr [esi + 0x40]
            mov dword ptr [eax + edi*4], ebp
            mov ecx, dword ptr [esi + 0x38]
            mov eax, dword ptr [ecx + edi*4]
            mov ecx, dword ptr [eax + 0x20]
            or ecx, ebx
            mov dword ptr [eax + 0x20], ecx
            mov edx, dword ptr [esi + 0x38]
            mov eax, dword ptr [edx + edi*4]
            push eax
            call BrMaterialUpdate
            add esp, 8
    L004473ba:
            mov eax, dword ptr [esi + 8]
            inc edi
            cmp edi, eax
            jl L0044737f
            pop edi
            pop esi
            pop ebp
            pop ebx
            pop ecx
            ret 4
    L004473ca:
            mov eax, dword ptr [esp + 0x18]
            sub eax, ebp
            je L004474d7
            sub eax, 2
            jne L004475af
            mov eax, dword ptr [esi + 8]
            xor ecx, ecx
            cmp eax, ebp
            mov dword ptr [esp + 0x18], ecx
            jle L004475af
    L004473f0:
            mov edx, dword ptr [esi + 0x38]
            mov eax, dword ptr [edx + ecx*4]
            mov edi, dword ptr [eax + 0x40]
            cmp edi, ebp
            je L004474bf
            mov edx, dword ptr [esi + 0x10]
            xor eax, eax
            cmp edx, ebp
            jle L00447419
            mov ebx, dword ptr [esi + 0x30]
    L0044740d:
            cmp dword ptr [ebx], edi
            je L00447419
            inc eax
            add ebx, 4
            cmp eax, edx
            jl L0044740d
    L00447419:
            cmp eax, edx
            je L004474bf
            test byte ptr [edi + 0x2d], 1
            jne L004474bf
            movsx edx, word ptr [edi + 0x28]
            xor eax, eax
            xor ebx, ebx
            mov ax, word ptr [edi + 0x32]
            mov bx, word ptr [edi + 0x36]
            imul eax, edx
            mov edx, dword ptr [edi + 8]
            mov dword ptr [esp + 0x10], ebp
            add eax, edx
            xor edx, edx
            mov dx, word ptr [edi + 0x30]
            add eax, edx
            cmp ebx, ebp
            jle L00447485
            xor ebp, ebp
            mov bp, word ptr [edi + 0x34]
    L00447459:
            xor ecx, ecx
            mov edx, eax
            test ebp, ebp
            jle L0044746c
    L00447461:
            cmp byte ptr [edx], 0
            je L004474b9
            inc ecx
            inc edx
            cmp ecx, ebp
            jl L00447461
    L0044746c:
            movsx ecx, word ptr [edi + 0x28]
            add eax, ecx
            mov ecx, dword ptr [esp + 0x10]
            inc ecx
            cmp ecx, ebx
            mov dword ptr [esp + 0x10], ecx
            jl L00447459
            mov ecx, dword ptr [esp + 0x18]
            xor ebp, ebp
    L00447485:
            mov eax, dword ptr [esi + 0x40]
            push 0x7fff
            mov dword ptr [eax + ecx*4], edi
            mov edx, dword ptr [esi + 0x38]
            mov eax, dword ptr [edx + ecx*4]
            mov dword ptr [eax + 0x40], ebp
            mov edx, dword ptr [esi + 0x38]
            mov eax, dword ptr [edx + ecx*4]
            mov ebx, dword ptr [eax + 0x20]
            and ebx, 0xfffffffd
            mov dword ptr [eax + 0x20], ebx
            mov eax, dword ptr [esi + 0x38]
            mov ecx, dword ptr [eax + ecx*4]
            push ecx
            call BrMaterialUpdate
            add esp, 8
            jmp L004474bb
    L004474b9:
            xor ebp, ebp
    L004474bb:
            mov ecx, dword ptr [esp + 0x18]
    L004474bf:
            mov eax, dword ptr [esi + 8]
            inc ecx
            cmp ecx, eax
            mov dword ptr [esp + 0x18], ecx
            jl L004473f0
            pop edi
            pop esi
            pop ebp
            pop ebx
            pop ecx
            ret 4
    L004474d7:
            mov eax, dword ptr [esi + 8]
            xor edi, edi
            cmp eax, ebp
            jle L004475af
            mov ebx, 2
    L004474e9:
            mov edx, dword ptr [esi + 0x40]
            mov ecx, dword ptr [edx + edi*4]
            cmp ecx, ebp
            je L00447533
            call DRPixelmapHasZeros
            test eax, eax
            je L00447533
            mov ecx, dword ptr [esi + 0x38]
            mov eax, dword ptr [esi + 0x40]
            push 0x7fff
            mov edx, dword ptr [ecx + edi*4]
            mov eax, dword ptr [eax + edi*4]
            mov dword ptr [edx + 0x40], eax
            mov ecx, dword ptr [esi + 0x40]
            mov dword ptr [ecx + edi*4], ebp
            mov edx, dword ptr [esi + 0x38]
            mov eax, dword ptr [edx + edi*4]
            mov ecx, dword ptr [eax + 0x20]
            or ecx, ebx
            mov dword ptr [eax + 0x20], ecx
            mov eax, dword ptr [esi + 0x38]
            mov ecx, dword ptr [eax + edi*4]
            push ecx
            call BrMaterialUpdate
            add esp, 8
    L00447533:
            mov eax, dword ptr [esi + 8]
            inc edi
            cmp edi, eax
            jl L004474e9
            pop edi
            pop esi
            pop ebp
            pop ebx
            pop ecx
            ret 4
    L00447543:
            mov eax, dword ptr [esi + 8]
            xor ebx, ebx
            cmp eax, ebp
            jle L004475af
    L0044754c:
            mov edx, dword ptr [esi + 0x38]
            mov eax, dword ptr [edx + ebx*4]
            mov edi, dword ptr [eax + 0x40]
            cmp edi, ebp
            je L004475a7
            mov ecx, dword ptr [esi + 0x10]
            xor eax, eax
            cmp ecx, ebp
            jle L00447571
            mov edx, dword ptr [esi + 0x30]
    L00447565:
            cmp dword ptr [edx], edi
            je L00447571
            inc eax
            add edx, 4
            cmp eax, ecx
            jl L00447565
    L00447571:
            cmp eax, ecx
            je L004475a7
            mov ecx, dword ptr [esi + 0x40]
            push 0x7fff
            mov dword ptr [ecx + ebx*4], edi
            mov edx, dword ptr [esi + 0x38]
            mov eax, dword ptr [edx + ebx*4]
            mov dword ptr [eax + 0x40], ebp
            mov ecx, dword ptr [esi + 0x38]
            mov eax, dword ptr [ecx + ebx*4]
            mov edi, dword ptr [eax + 0x20]
            and edi, 0xfffffffd
            mov dword ptr [eax + 0x20], edi
            mov edx, dword ptr [esi + 0x38]
            mov eax, dword ptr [edx + ebx*4]
            push eax
            call BrMaterialUpdate
            add esp, 8
    L004475a7:
            mov eax, dword ptr [esi + 8]
            inc ebx
            cmp ebx, eax
            jl L0044754c
    L004475af:
            pop edi
            pop esi
            pop ebp
            pop ebx
            pop ecx
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
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00447640
tCar_texturing_level C2_HOOK_FASTCALL GetCarTexturingLevel(void) {
    return gCar_texturing_level;
}

// FUNCTION: CARMA2_HW 0x00447650
    void C2_NAKED C2_HOOK_FASTCALL SetCarTexturingLevel(tCar_texturing_level pLevel) {
#ifndef CARPOCALYPSE2_MATCHING

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
#else
    __asm {
            push ecx
            mov eax, ecx
            push ebx
            mov ecx, dword ptr [gCar_texturing_level]
            push ebp
            push esi
            cmp ecx, eax
            push edi
            mov dword ptr [esp + 0x10], eax
            je L004479a8
            mov edx, dword ptr [gOur_car_storage_space+0x4]
            xor edi, edi
            cmp edx, edi
            mov ebx, 0xfffffffd
            mov ebp, 2
            je L0044779d
            sub eax, edi
            je L00447724
            dec eax
            je L004476fb
            dec eax
            jne L0044779d
            mov eax, dword ptr [gOur_car_storage_space+0x8]
            xor esi, esi
            cmp eax, edi
            jle L0044779d
    L004476a4:
            mov eax, dword ptr [gOur_car_storage_space+0x40]
            mov eax, dword ptr [eax + esi*4]
            cmp eax, edi
            je L004476ec
            mov ecx, dword ptr [gOur_car_storage_space+0x38]
            push 0x7fff
            mov edx, dword ptr [ecx + esi*4]
            mov dword ptr [edx + 0x40], eax
            mov eax, dword ptr [gOur_car_storage_space+0x40]
            mov dword ptr [eax + esi*4], edi
            mov ecx, dword ptr [gOur_car_storage_space+0x38]
            mov eax, dword ptr [ecx + esi*4]
            mov ecx, dword ptr [eax + 0x20]
            or ecx, ebp
            mov dword ptr [eax + 0x20], ecx
            mov edx, dword ptr [gOur_car_storage_space+0x38]
            mov eax, dword ptr [edx + esi*4]
            push eax
            call BrMaterialUpdate
            add esp, 8
    L004476ec:
            mov eax, dword ptr [gOur_car_storage_space+0x8]
            inc esi
            cmp esi, eax
            jl L004476a4
            jmp L0044779d
    L004476fb:
            mov eax, ecx
            sub eax, edi
            je L00447718
            sub eax, ebp
            jne L0044779d
            mov ecx, offset gOur_car_storage_space
            call HideStoredOpaqueTextures
            jmp L0044779d
    L00447718:
            mov ecx, offset gOur_car_storage_space
            call RevealStoredTransparentTextures
            jmp L0044779d
    L00447724:
            mov eax, dword ptr [gOur_car_storage_space+0x8]
            xor esi, esi
            cmp eax, edi
            jle L0044779d
    L0044772f:
            mov ecx, dword ptr [gOur_car_storage_space+0x38]
            mov edx, dword ptr [ecx + esi*4]
            mov edx, dword ptr [edx + 0x40]
            cmp edx, edi
            je L00447793
            mov ecx, offset gOur_car_storage_space
            call StorageContainsPixelmap
            test eax, eax
            je L00447793
            mov eax, dword ptr [gOur_car_storage_space+0x38]
            push 0x7fff
            mov ecx, dword ptr [eax + esi*4]
            mov eax, dword ptr [gOur_car_storage_space+0x40]
            mov edx, dword ptr [ecx + 0x40]
            mov dword ptr [eax + esi*4], edx
            mov ecx, dword ptr [gOur_car_storage_space+0x38]
            mov edx, dword ptr [ecx + esi*4]
            mov dword ptr [edx + 0x40], edi
            mov eax, dword ptr [gOur_car_storage_space+0x38]
            mov eax, dword ptr [eax + esi*4]
            mov ecx, dword ptr [eax + 0x20]
            and ecx, ebx
            mov dword ptr [eax + 0x20], ecx
            mov ecx, dword ptr [gOur_car_storage_space+0x38]
            mov edx, dword ptr [ecx + esi*4]
            push edx
            call BrMaterialUpdate
            add esp, 8
    L00447793:
            mov eax, dword ptr [gOur_car_storage_space+0x8]
            inc esi
            cmp esi, eax
            jl L0044772f
    L0044779d:
            cmp dword ptr [gTheir_cars_storage_space+0x4], edi
            je L004478ca
            mov eax, dword ptr [esp + 0x10]
            sub eax, edi
            je L00447851
            dec eax
            je L00447825
            dec eax
            jne L004478ca
            mov eax, dword ptr [gTheir_cars_storage_space+0x8]
            xor esi, esi
            cmp eax, edi
            jle L004478ca
    L004477ce:
            mov eax, dword ptr [gTheir_cars_storage_space+0x40]
            mov eax, dword ptr [eax + esi*4]
            cmp eax, edi
            je L00447816
            mov ecx, dword ptr [gTheir_cars_storage_space+0x38]
            push 0x7fff
            mov edx, dword ptr [ecx + esi*4]
            mov dword ptr [edx + 0x40], eax
            mov eax, dword ptr [gTheir_cars_storage_space+0x40]
            mov dword ptr [eax + esi*4], edi
            mov ecx, dword ptr [gTheir_cars_storage_space+0x38]
            mov eax, dword ptr [ecx + esi*4]
            mov ebx, dword ptr [eax + 0x20]
            or ebx, ebp
            mov dword ptr [eax + 0x20], ebx
            mov edx, dword ptr [gTheir_cars_storage_space+0x38]
            mov eax, dword ptr [edx + esi*4]
            push eax
            call BrMaterialUpdate
            add esp, 8
    L00447816:
            mov eax, dword ptr [gTheir_cars_storage_space+0x8]
            inc esi
            cmp esi, eax
            jl L004477ce
            jmp L004478ca
    L00447825:
            mov eax, dword ptr [gCar_texturing_level]
            sub eax, edi
            je L00447845
            sub eax, ebp
            jne L004478ca
            mov ecx, offset gTheir_cars_storage_space
            call HideStoredOpaqueTextures
            jmp L004478ca
    L00447845:
            mov ecx, offset gTheir_cars_storage_space
            call RevealStoredTransparentTextures
            jmp L004478ca
    L00447851:
            mov eax, dword ptr [gTheir_cars_storage_space+0x8]
            xor esi, esi
            cmp eax, edi
            jle L004478ca
    L0044785c:
            mov ecx, dword ptr [gTheir_cars_storage_space+0x38]
            mov edx, dword ptr [ecx + esi*4]
            mov edx, dword ptr [edx + 0x40]
            cmp edx, edi
            je L004478c0
            mov ecx, offset gTheir_cars_storage_space
            call StorageContainsPixelmap
            test eax, eax
            je L004478c0
            mov eax, dword ptr [gTheir_cars_storage_space+0x38]
            push 0x7fff
            mov ecx, dword ptr [eax + esi*4]
            mov eax, dword ptr [gTheir_cars_storage_space+0x40]
            mov edx, dword ptr [ecx + 0x40]
            mov dword ptr [eax + esi*4], edx
            mov ecx, dword ptr [gTheir_cars_storage_space+0x38]
            mov edx, dword ptr [ecx + esi*4]
            mov dword ptr [edx + 0x40], edi
            mov eax, dword ptr [gTheir_cars_storage_space+0x38]
            mov eax, dword ptr [eax + esi*4]
            mov ecx, dword ptr [eax + 0x20]
            and ecx, ebx
            mov dword ptr [eax + 0x20], ecx
            mov ecx, dword ptr [gTheir_cars_storage_space+0x38]
            mov edx, dword ptr [ecx + esi*4]
            push edx
            call BrMaterialUpdate
            add esp, 8
    L004478c0:
            mov eax, dword ptr [gTheir_cars_storage_space+0x8]
            inc esi
            cmp esi, eax
            jl L0044785c
    L004478ca:
            cmp dword ptr [gNet_cars_storage_space+0x4], edi
            je L004479a8
            mov eax, dword ptr [esp + 0x10]
            sub eax, edi
            je L0044799e
            dec eax
            je L0044795d
            dec eax
            jne L004479a8
            mov eax, dword ptr [gTheir_cars_storage_space+0x8]
            xor esi, esi
            cmp eax, edi
            jle L004479a8
    L004478fb:
            mov eax, dword ptr [gTheir_cars_storage_space+0x40]
            mov eax, dword ptr [eax + esi*4]
            cmp eax, edi
            je L00447943
            mov ecx, dword ptr [gTheir_cars_storage_space+0x38]
            push 0x7fff
            mov edx, dword ptr [ecx + esi*4]
            mov dword ptr [edx + 0x40], eax
            mov eax, dword ptr [gTheir_cars_storage_space+0x40]
            mov dword ptr [eax + esi*4], edi
            mov ecx, dword ptr [gTheir_cars_storage_space+0x38]
            mov eax, dword ptr [ecx + esi*4]
            mov ebx, dword ptr [eax + 0x20]
            or ebx, ebp
            mov dword ptr [eax + 0x20], ebx
            mov edx, dword ptr [gTheir_cars_storage_space+0x38]
            mov eax, dword ptr [edx + esi*4]
            push eax
            call BrMaterialUpdate
            add esp, 8
    L00447943:
            mov eax, dword ptr [gTheir_cars_storage_space+0x8]
            inc esi
            cmp esi, eax
            jl L004478fb
            mov ecx, dword ptr [esp + 0x10]
            mov dword ptr [gCar_texturing_level], ecx
            pop edi
            pop esi
            pop ebp
            pop ebx
            pop ecx
            ret
    L0044795d:
            mov eax, dword ptr [gCar_texturing_level]
            sub eax, edi
            je L00447984
            sub eax, ebp
            jne L004479a8
            mov ecx, offset gTheir_cars_storage_space
            call HideStoredOpaqueTextures
            mov ecx, dword ptr [esp + 0x10]
            mov dword ptr [gCar_texturing_level], ecx
            pop edi
            pop esi
            pop ebp
            pop ebx
            pop ecx
            ret
    L00447984:
            mov ecx, offset gTheir_cars_storage_space
            call RevealStoredTransparentTextures
            mov ecx, dword ptr [esp + 0x10]
            mov dword ptr [gCar_texturing_level], ecx
            pop edi
            pop esi
            pop ebp
            pop ebx
            pop ecx
            ret
    L0044799e:
            mov ecx, offset gTheir_cars_storage_space
            call HideStoredTextures
    L004479a8:
            mov ecx, dword ptr [esp + 0x10]
            pop edi
            pop esi
            pop ebp
            mov dword ptr [gCar_texturing_level], ecx
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
    }
#endif
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

    unsuffixed_len = strlen(pOld_ident);
    unsuffixed_len -= strlen(pSuffix);
    new_id = BrMemAllocate(unsuffixed_len + 1, kMem_new_mat_id);
    sprintf(new_id, "%.*s", unsuffixed_len, pOld_ident);
    result = BrMaterialFind(new_id);
    BrMemFree(new_id);
    return result;
}

// FUNCTION: CARMA2_HW 0x00447d50
C2_NAKED br_material* C2_HOOK_FASTCALL WallLinearToUntex(br_model* pModel, tU16 pFace) {
#ifndef CARPOCALYPSE2_MATCHING
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
    suffix_length = strlen(".pwall");
    if (identifier != NULL) {
        if (identifier_length >= suffix_length) {
            has_pwall_suffix = strcmp(identifier + identifier_length - suffix_length, ".pwall") == 0;
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
    suffix_length = strlen(".lwall");
    identifier_length = strlen(old_mat->identifier);
    new_id = BrMemAllocate(identifier_length + suffix_length + 1, kMem_new_mat_id);
    sprintf(new_id, "%s%s", old_mat->identifier, ".lwall");
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
#else
    __asm {
            sub esp, 8
            mov word ptr [esp], dx
            push ebx
            mov eax, dword ptr [esp + 4]
            push ebp
            push esi
            mov esi, ecx
            and eax, 0xffff
            push edi
            mov ecx, dword ptr [esi + 0xc]
            mov dword ptr [esp + 0x14], esi
            lea eax, [eax + eax*4]
            mov ebx, dword ptr [ecx + eax*8 + 8]
            or ecx, 0xffffffff
            xor eax, eax
            mov ebp, dword ptr [ebx + 4]
            mov edi, ebp
            repne scasb
            not ecx
            dec ecx
            mov edi, offset gWall_texturing_tweakers+0x3c
            mov edx, ecx
            or ecx, 0xffffffff
            repne scasb
            not ecx
            dec ecx
            test ebp, ebp
            je L00447de0
            cmp edx, ecx
            jae L00447d9f
            xor eax, eax
            jmp L00447de0
    L00447d9f:
            lea eax, [ebp + edx]
            mov esi, offset gWall_texturing_tweakers+0x3c
            sub eax, ecx
    L00447daa:
            mov dl, byte ptr [eax]
            mov cl, dl
            cmp dl, byte ptr [esi]
            jne L00447dce
            test cl, cl
            je L00447dca
            mov dl, byte ptr [eax + 1]
            mov cl, dl
            cmp dl, byte ptr [esi + 1]
            jne L00447dce
            add eax, 2
            add esi, 2
            test cl, cl
            jne L00447daa
    L00447dca:
            xor eax, eax
            jmp L00447dd3
    L00447dce:
            sbb eax, eax
            sbb eax, -1
    L00447dd3:
            mov esi, dword ptr [esp + 0x14]
            xor ecx, ecx
            test eax, eax
            sete cl
            mov eax, ecx
    L00447de0:
            test eax, eax
            je L00447e0a
            mov eax, dword ptr [ebx + 0x40]
            test eax, eax
            je L00447e00
            push 0x7fff
            push ebx
            mov dword ptr [ebx + 0x40], 0
            call BrMaterialUpdate
            add esp, 8
    L00447e00:
            mov eax, ebx
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 8
            ret
    L00447e0a:
            mov edx, dword ptr [esp + 0x10]
            mov ecx, esi
            call FUN_00447ef0
            test eax, eax
            jne L00447ed7
            mov edx, dword ptr [ebx + 4]
            test edx, edx
            je L00447ed7
            mov eax, dword ptr [ebx + 0x40]
            test eax, eax
            je L00447ed7
            mov edi, offset gWall_texturing_tweakers+0x34
            or ecx, 0xffffffff
            xor eax, eax
            push 0xcf
            repne scasb
            not ecx
            dec ecx
            mov edi, edx
            mov esi, ecx
            or ecx, 0xffffffff
            repne scasb
            not ecx
            dec ecx
            lea edx, [esi + ecx + 1]
            push edx
            call BrMemAllocate
            add esp, 8
            mov edi, eax
            mov eax, dword ptr [ebx + 4]
            push offset gWall_texturing_tweakers+0x34
            push eax
            push offset gWall_texturing_tweakers+0x54
            push edi
            call sprintf
            add esp, 0x10
            push edi
            call BrMaterialFind
            mov esi, eax
            add esp, 4
            test esi, esi
            je L00447e90
            push edi
            call BrMemFree
            jmp L00447eae
    L00447e90:
            push 0
            call BrMaterialAllocate
            mov esi, eax
            add esp, 4
            mov edx, ebx
            mov ecx, esi
            call CopyBrMaterial
            mov dword ptr [esi + 4], edi
            push esi
            call BrMaterialAdd
    L00447eae:
            mov eax, dword ptr [esi + 0x40]
            add esp, 4
            test eax, eax
            je L00447ecd
            push 0x7fff
            push esi
            mov dword ptr [esi + 0x40], 0
            call BrMaterialUpdate
            add esp, 8
    L00447ecd:
            mov eax, esi
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 8
            ret
    L00447ed7:
            pop edi
            pop esi
            pop ebp
            xor eax, eax
            pop ebx
            add esp, 8
            ret
    }
#endif
}

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x00447ef0
int C2_NAKED C2_HOOK_FASTCALL FUN_00447ef0(br_model* pModel, tU16 pIndex) {
#ifndef CARPOCALYPSE2_MATCHING
    br_vector3 edge0;
    br_vector3 edge1;
    br_vector3 cross;

    BrVector3Sub(&edge0, &pModel->vertices[pModel->faces[pIndex].vertices[0]].p, &pModel->vertices[pModel->faces[pIndex].vertices[1]].p);
    BrVector3Sub(&edge1, &pModel->vertices[pModel->faces[pIndex].vertices[1]].p, &pModel->vertices[pModel->faces[pIndex].vertices[2]].p);
    BrVector3Cross(&cross, &edge0, &edge1);
    return cross.v[1] * gFLOAT_00589d20 > sqrt(cross.v[0] * cross.v[0] + cross.v[2] * cross.v[2]);
#else
    __asm {
            sub esp, 0x1c
            mov word ptr [esp], dx
            push esi
            mov eax, dword ptr [esp + 4]
            mov edx, dword ptr [ecx + 0xc]
            mov esi, dword ptr [ecx + 8]
            and eax, 0xffff
            push edi
            xor ecx, ecx
            lea edi, [eax + eax*4]
            xor eax, eax
            mov ax, word ptr [edx + edi*8]
            mov cx, word ptr [edx + edi*8 + 2]
            lea edi, [edx + edi*8]
            xor edx, edx
            mov dx, word ptr [edi + 4]
            lea ecx, [ecx + ecx*4]
            lea edi, [eax + eax*4]
            fld dword ptr [esi + ecx*8]
            fld dword ptr [esi + edi*8]
            fld dword ptr [esi + ecx*8 + 4]
            lea eax, [esi + ecx*8]
            lea ecx, [esi + edi*8]
            lea edx, [edx + edx*4]
            fld dword ptr [eax + 8]
            fld dword ptr [ecx + 4]
            fxch st(3)
            _emit 0xd8
        _emit 0x20
            fstp dword ptr [esp + 0xc]
            fld dword ptr [ecx + 8]
            fxch st(3)
            fsub dword ptr [eax + 4]
            fstp dword ptr [esp + 0x10]
            fxch st(2)
            fsub dword ptr [eax + 8]
            fxch st(3)
            fsub dword ptr [esi + edx*8]
            fxch st(1)
            fsub dword ptr [esi + edx*8 + 4]
            fxch st(2)
            fsub dword ptr [esi + edx*8 + 8]
            fxch st(3)
            fstp dword ptr [esp + 0x14]
            lea eax, [esi + edx*8]
            fst dword ptr [esp + 0x18]
            fld st(1)
            fld st(2)
            fxch st(4)
            fst dword ptr [esp + 0x20]
            fmul dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x18]
            fxch st(2)
            fmul dword ptr [esp + 0x14]
            fsubp st(1), st(0)
            fxch st(4)
            fmul dword ptr [esp + 0xc]
            fld dword ptr [esp + 0x20]
            fxch st(5)
            fst dword ptr [esp + 0x18]
            fxch st(3)
            fmul dword ptr [esp + 0x10]
            fsubp st(1), st(0)
            fxch st(1)
            fmul dword ptr [esp + 0x14]
            fxch st(1)
            fst dword ptr [esp + 0x20]
            fmul dword ptr [esp + 0x20]
            fxch st(2)
            fmul dword ptr [esp + 0x18]
            fxch st(4)
            fmul dword ptr [esp + 0xc]
            fxch st(2)
            faddp st(4), st(0)
            fxch st(3)
            fsqrt
            fxch st(1)
            fsubp st(3), st(0)
            fst dword ptr [esp + 8]
            fxch st(2)
            fmul dword ptr [gFLOAT_00589d20]
            fxch st(1)
            fxch st(2)
            fcompp
            fnstsw ax
            test ah, 1
            fstp st(0)
            je L00447ff8
            mov eax, 1
            pop edi
            pop esi
            add esp, 0x1c
            ret
    L00447ff8:
            pop edi
            xor eax, eax
            pop esi
            add esp, 0x1c
            ret
    }
#endif
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
    const char* suffix;

    old_mat = pModel->faces[pFace].material;
    if (old_mat->colour_map == NULL || (old_mat->flags & BR_MATF_PERSPECTIVE) == 0 || FUN_00447ef0(pModel, pFace) != 0) {
        return NULL;
    }
    suffix = ".pwall";
    new_id = BrMemAllocate(strlen(old_mat->identifier) + strlen(suffix) + 1, kMem_new_mat_id);
    sprintf(new_id, "%s%s", old_mat->identifier, ".pwall");
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
            new_mat = SuffixedMaterial(old_mat, ".pwall");
        } else {
            new_mat = SuffixedMaterial(old_mat, ".lwall");
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