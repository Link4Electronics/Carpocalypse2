#include "utility.h"

#include "errors.h"
#include "globvars.h"
#include "globvrpb.h"
#include "graphics.h"
#include "init.h"
#include "input.h"
#include "loading.h"
#include "main.h"
#include "mainloop.h"
#include "mainmenu.h"
#include "network.h"
#include "platform.h"
#include "sound.h"
#include "world.h"

#include "brender/brender.h"

#include "carpocalypse2_macros.h"

#include <ctype.h>
#include "c2_stdlib.h"
#include "c2_string.h"

#include <float.h>
#include "c2_math.h"
#include "utility.h"
// GLOBAL: CARMA2_HW 0x006abef4
extern tU32 gLost_time;
// GLOBAL: CARMA2_HW 0x006abee0
int gIn_check_quit;

// GLOBAL: CARMA2_HW 0x006585f0
extern const tU8 gLong_key[16];
// GLOBAL: CARMA2_HW 0x00658600
extern const tU8 gOther_long_key[16];
// GLOBAL: CARMA2_HW 0x00655df0
br_vector3 y_unit_vector = { { 0.f, 1.f, 0.f } };

extern br_pixelmap* gRender_palette;
extern tPixelFlags gPixelFlags;

// GLOBAL: CARMA2_HW 0x006abef8
tU32 last_service;

// GLOBAL: CARMA2_HW 0x006b5f40
char* gMisc_strings[300];

// GLOBAL: CARMA2_HW 0x0074ca04
tMaterial_exception* gMaterial_exceptions;

// GLOBAL: CARMA2_HW 0x006b63f4
br_pixelmap* g16bit_palette;

// GLOBAL: CARMA2_HW 0x006b63f0
br_pixelmap* gPalette_source;

// GLOBAL: CARMA2_HW 0x006abef4
tU32 gLost_time;

// GLOBAL: CARMA2_HW 0x0079efb0
tU32 gLast_tick_count;

// GLOBAL: CARMA2_HW 0x0058b280
const float gFloatZero_0058b280 = 0.0f;

// GLOBAL: CARMA2_HW 0x0058b26c
float gFLOAT_0058b26c = 3.0517578125e-05f;

// GLOBAL: CARMA2_HW 0x0058b270
double gDOUBLE_0058b270 = 0.001;

#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern void* C2_HOOK_CDECL BrMemAllocate();
extern void C2_HOOK_CDECL BrMemFree();
extern void C2_HOOK_CDECL BrModelUpdate();
extern void C2_HOOK_FASTCALL FUN_005192a0(br_uint_16* pPixel);
extern void C2_HOOK_FASTCALL FUN_005192c0(tU16* pPixel);
extern void C2_HOOK_FASTCALL WriteU16L(FILE* pF, tU16 pNumber);
extern void C2_HOOK_FASTCALL WriteU32L(FILE* pF, tU32 pNumber);
extern void C2_HOOK_FASTCALL WriteU8L(FILE* pF, tU8 pNumber);
extern br_uint_8 bl;
extern br_pixelmap* gBack_screen;
extern br_pixelmap* gCurrent_palette;
extern tU32 gFrame_period;
extern tU32 gLast_replay_frame_time;
extern tNet_mode gNet_mode;
extern br_actor* gNon_track_actor;
extern int gPed_676914;
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_CDECL BrMatrix34ApplyP();
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern C2_NORETURN void C2_HOOK_CDECL BrFailure();
extern br_pixelmap* C2_HOOK_CDECL BrPixelmapAllocate();
extern void C2_HOOK_CDECL BrPixelmapCopy();
extern br_pixelmap* C2_HOOK_CDECL BrPixelmapDirectLock();
extern br_pixelmap* C2_HOOK_CDECL BrPixelmapDirectUnlock();
extern br_pixelmap* C2_HOOK_CDECL BrPixelmapLoad();
extern br_uint_32 C2_HOOK_CDECL BrPixelmapSave();
extern br_pixelmap* C2_HOOK_CDECL BrTableAdd();
extern br_pixelmap* C2_HOOK_FASTCALL DRLdImg(const char* texturePathDir, const char* textureName, br_pixelmap* pPalette, int flags, int* errorCode);
extern FILE* C2_HOOK_FASTCALL DRfopen(const char* pFilename, const char* pMode);
extern void C2_NORETURN C2_HOOK_CDECL FatalError();
extern int C2_HOOK_FASTCALL GetAdditionalFileName(const char* path, tName_list* pList);
extern int C2_HOOK_FASTCALL GetFileName(const char *path, tName_list *pList);
extern void C2_HOOK_FASTCALL PFForEveryFile2(const char* path, tEnumPathCallback pCallback, void* data);
extern int C2_HOOK_FASTCALL PFfclose(FILE* pFile);
extern void __cdecl _chkstk();
extern void __cdecl _ftol();
extern tPath_name gApplication_path;
extern char gCurrent_load_directory[256];
extern char gCurrent_load_name[256];
extern const char* gDir_separator;
extern tFrontend_spec gFrontend_QUIT;
extern int gGraf_spec_index;
extern tGraf_spec gGraf_specs[2];
extern int gLast_key_down;
extern tRendererShadingType gMaterial_shading_for_callback;
extern char gNext_crush_actor_id[];
extern int gPalette_changed;
extern char gPed_flags_0058f2e4[];
extern char gSplit_triangle_vertex_orders[];
extern tDepth_effect_type gSwap_depth_effect_type;
extern char gVector3_00661678[];
#endif
// FUNCTION: CARMA2_HW 0x00515910
void C2_HOOK_FASTCALL StringTransformToLower(char* pStr) {
    int i;
    int len;

    len = strlen(pStr);
    for (i = 0; i < len; i++) {
        pStr[i] = tolower(pStr[i]);
    }
}

// FUNCTION: CARMA2_HW 0x00513770
float C2_HOOK_STDCALL tandeg(float pAngle) {
    double radians;

    radians = pAngle * (CARPOCALYPSE2_PI / 180.0);
    return (float)(sin(radians) / cos(radians));
}

// FUNCTION: CARMA2_HW 0x00518b00
intptr_t C2_HOOK_CDECL DumpVisibleActorsCB(br_actor* pActor, void* pData) {

    if (pActor->render_style != BR_RSTYLE_NONE) {
        const char* str;

        switch (pActor->type) {
        case BR_ACTOR_NONE:
            printf("DUMMY ACTOR\n");
            break;
        case BR_ACTOR_MODEL:
            printf("MODEL ACTOR\n");
            break;
        case BR_ACTOR_LIGHT:
            printf("LIGHT ACTOR\n");
            break;
        case BR_ACTOR_CAMERA:
            printf("CAMERA ACTOR\n");
            break;
        case BR_ACTOR_BOUNDS:
            printf("BOUNDS ACTOR\n");
            break;
        case BR_ACTOR_BOUNDS_CORRECT:
            printf("BOUNDS-CORRECT ACTOR\n");
            break;
        case BR_ACTOR_CLIP_PLANE:
            printf("CLIP-PLANE ACTOR\n");
            break;
        default:
            printf("******** UNKNOWN ********\n");
            break;
        }
        str = pActor->identifier;
        if (str == NULL) {
            str = "No Identifier";
        }
        printf("\tName:\t\t\"%s\"\n", str);
        printf("\tAddress:\t%08x\n", (unsigned int)(uintptr_t)pActor);
        printf("\tParent:\t\t%08x\n", (unsigned int)(uintptr_t)pActor->parent);
        printf("\tDepth:\t\t%i\n", pActor->depth);
        printf("\tModelPtr:\t0x%08x", (unsigned int)(uintptr_t)pActor->model);
        if (pActor->model != NULL) {
            str = pActor->model->identifier;
            if (str == NULL) {
                str = "No Identifier";
            }
            printf(" (\"%s\")\n", str);
        } else {
            printf("\n");
        }
        printf("\tMaterialPtr:\t0x%08x", (unsigned int)(uintptr_t)pActor->material);
        if (pActor->material != NULL) {
            str = pActor->material->identifier;
            if (str == NULL) {
                str = "No Identifier";
            }
            printf(" (\"%s\")\n", str);
        } else {
            printf("\n");
        }
        switch (pActor->render_style) {
        case BR_RSTYLE_DEFAULT:
            str = "BR_RSTYLE_DEFAULT";
            break;
        case BR_RSTYLE_NONE:
            str = "BR_RSTYLE_NONE";
            break;
        case BR_RSTYLE_POINTS:
            str = "BR_RSTYLE_POINTS";
            break;
        case BR_RSTYLE_EDGES:
            str = "BR_RSTYLE_EDGES";
            break;
        case BR_RSTYLE_FACES:
            str = "BR_RSTYLE_FACES";
            break;
        case BR_RSTYLE_BOUNDING_POINTS:
            str = "BR_RSTYLE_BOUNDING_POINTS";
            break;
        case BR_RSTYLE_BOUNDING_EDGES:
            str = "BR_RSTYLE_BOUNDING_EDGES";
            break;
        case BR_RSTYLE_BOUNDING_FACES:
            str = "BR_RSTYLE_BOUNDING_FACES";
            break;
        default:
            str = "******** UNKNOWN ********";
            break;
        }
        printf("\tRenderStyle:\t%s\n", str);
        printf("\n");
    }
    return BrActorEnum(pActor, DumpVisibleActorsCB, pData);
}

// FUNCTION: CARMA2_HW 0x00518cf0
const char* C2_HOOK_FASTCALL BrActorRenderStyleName(br_actor* pActor) {
    const char* str;

    switch (pActor->render_style) {
    case BR_RSTYLE_DEFAULT:
        str = "BR_RSTYLE_DEFAULT";
        break;
    case BR_RSTYLE_NONE:
        str = "BR_RSTYLE_NONE";
        break;
    case BR_RSTYLE_POINTS:
        str = "BR_RSTYLE_POINTS";
        break;
    case BR_RSTYLE_EDGES:
        str = "BR_RSTYLE_EDGES";
        break;
    case BR_RSTYLE_FACES:
        str = "BR_RSTYLE_FACES";
        break;
    case BR_RSTYLE_BOUNDING_POINTS:
        str = "BR_RSTYLE_BOUNDING_POINTS";
        break;
    case BR_RSTYLE_BOUNDING_EDGES:
        str = "BR_RSTYLE_BOUNDING_EDGES";
        break;
    case BR_RSTYLE_BOUNDING_FACES:
        str = "BR_RSTYLE_BOUNDING_FACES";
        break;
    default:
        str = "******** UNKNOWN ********";
        break;
    }
    return str;
}

// FUNCTION: CARMA2_HW 0x00518ac0
void C2_HOOK_FASTCALL DumpVisibleActors(br_actor* pActor, const char* pMsg) {

    printf("MSG:\"%s\"\n", pMsg);
    BrActorEnum(pActor, DumpVisibleActorsCB, NULL);
    printf("----------------------------------\n\n");
    fflush(stdout);
}

// FUNCTION: CARMA2_HW 0x00513a30
int C2_NAKED C2_HOOK_FASTCALL LoadTextureTryAllLocations(char* pName, br_pixelmap** pMaps, int pCapacity) {
#ifndef CARPOCALYPSE2_MATCHING
    tPath_name path1;
    tPath_name path2;
    tPath_name path3;
    tPath_name path4;
    tName_list list;
    const char* load_path;
    int i;
    int error;

    (void)pCapacity;
    list.size = 0;
    pName[strlen(pName) - 4] = '\0';
    PathCat(path1, gApplication_path, gCurrent_load_directory);
    if (strcmp(gCurrent_load_directory, "COMMON") != 0 && strcmp(gCurrent_load_directory, "INTRFACE") != 0) {
        PathCat(path1, path1, gCurrent_load_name);
    }
    PathCat(path1, path1, gGraf_specs[gGraf_spec_index].data_dir_name);
    PathCat(path1, path1, pName);
    PathCat(path3, gApplication_path, gCurrent_load_directory);
    if (strcmp(gCurrent_load_directory, "COMMON") != 0 && strcmp(gCurrent_load_directory, "INTRFACE") != 0) {
        PathCat(path3, path3, gCurrent_load_name);
    }
    PathCat(path3, path3, pName);

    PathCat(path2, path1, "TIFFX");
    PFForEveryFile2(path2, (tEnumPathCallback)GetFileName, &list);
    PathCat(path4, path1, "PIX8");
    PFForEveryFile2(path4, (tEnumPathCallback)GetAdditionalFileName, &list);
    PathCat(path2, path1, "TIFFRGB");
    PFForEveryFile2(path2, (tEnumPathCallback)GetAdditionalFileName, &list);
    PathCat(path4, path1, "PIX16");
    PFForEveryFile2(path4, (tEnumPathCallback)GetAdditionalFileName, &list);
    load_path = path1;
    if (list.size == 0) {
        PathCat(path2, path3, "TIFFX");
        PFForEveryFile2(path2, (tEnumPathCallback)GetFileName, &list);
        PathCat(path4, path3, "PIX8");
        PFForEveryFile2(path4, (tEnumPathCallback)GetAdditionalFileName, &list);
        PathCat(path2, path3, "TIFFRGB");
        PFForEveryFile2(path2, (tEnumPathCallback)GetAdditionalFileName, &list);
        PathCat(path4, path3, "PIX16");
        PFForEveryFile2(path4, (tEnumPathCallback)GetAdditionalFileName, &list);
        load_path = path3;
    }
    for (i = 0; i < list.size; i++) {
        pMaps[i] = DRLdImg(load_path, list.items[i], gRender_palette, gPixelFlags, &error);
    }
    return i;
#else
    __asm {
            mov eax, 0x4298
            call _chkstk
            mov dword ptr [esp + 8], edx
            xor eax, eax
            push ebx
            push ebp
            push esi
            mov edx, ecx
            push edi
            mov edi, edx
            or ecx, 0xffffffff
            xor ebx, ebx
            repne scasb
            not ecx
            dec ecx
            mov dword ptr [esp + 0x424], ebx
            mov edi, offset gApplication_path
            mov dword ptr [esp + 0x10], edx
            mov byte ptr [ecx + edx - 4], al
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            lea edx, [esp + 0x24]
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            shr ecx, 2
            rep movsd
            mov ecx, eax
            xor eax, eax
            and ecx, 3
            rep movsb
            mov edi, offset gCurrent_load_directory
            or ecx, 0xffffffff
            repne scasb
            not ecx
            dec ecx
            je L00513aef
            mov edi, dword ptr [gDir_separator]
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            lea edx, [esp + 0x24]
            mov esi, edi
            mov ebp, ecx
            mov edi, edx
            or ecx, 0xffffffff
            repne scasb
            mov ecx, ebp
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, ebp
            lea edx, [esp + 0x24]
            and ecx, 3
            rep movsb
            mov edi, offset gCurrent_load_directory
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            mov esi, edi
            mov ebp, ecx
            mov edi, edx
            or ecx, 0xffffffff
            repne scasb
            mov ecx, ebp
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, ebp
            and ecx, 3
            rep movsb
    L00513aef:
            mov esi, offset gSplit_triangle_vertex_orders+0x200
            mov eax, offset gCurrent_load_directory
    L00513af9:
            mov dl, byte ptr [eax]
            mov cl, dl
            cmp dl, byte ptr [esi]
            jne L00513b1d
            test cl, cl
            je L00513b19
            mov dl, byte ptr [eax + 1]
            mov cl, dl
            cmp dl, byte ptr [esi + 1]
            jne L00513b1d
            add eax, 2
            add esi, 2
            test cl, cl
            jne L00513af9
    L00513b19:
            xor ebp, ebp
            jmp L00513b22
    L00513b1d:
            sbb ebp, ebp
            sbb ebp, -1
    L00513b22:
            mov dword ptr [esp + 0x14], ebp
            mov esi, offset gFrontend_QUIT+0xb900
            mov eax, offset gCurrent_load_directory
    L00513b30:
            mov dl, byte ptr [eax]
            mov cl, dl
            cmp dl, byte ptr [esi]
            jne L00513b52
            test cl, cl
            je L00513b57
            mov dl, byte ptr [eax + 1]
            mov cl, dl
            cmp dl, byte ptr [esi + 1]
            jne L00513b52
            add eax, 2
            add esi, 2
            test cl, cl
            jne L00513b30
            jmp L00513b57
    L00513b52:
            sbb ebx, ebx
            sbb ebx, -1
    L00513b57:
            test ebp, ebp
            je L00513bf6
            test ebx, ebx
            je L00513bf6
            lea edi, [esp + 0x24]
            or ecx, 0xffffffff
            xor eax, eax
            lea edx, [esp + 0x24]
            repne scasb
            not ecx
            sub edi, ecx
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            shr ecx, 2
            rep movsd
            mov ecx, eax
            xor eax, eax
            and ecx, 3
            rep movsb
            mov edi, offset gCurrent_load_name
            or ecx, 0xffffffff
            repne scasb
            not ecx
            dec ecx
            je L00513bf6
            mov edi, dword ptr [gDir_separator]
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            lea edx, [esp + 0x24]
            mov esi, edi
            mov edi, edx
            mov edx, ecx
            or ecx, 0xffffffff
            repne scasb
            mov ecx, edx
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, edx
            lea edx, [esp + 0x24]
            and ecx, 3
            rep movsb
            mov edi, offset gCurrent_load_name
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            mov esi, edi
            mov edi, edx
            mov edx, ecx
            or ecx, 0xffffffff
            repne scasb
            mov ecx, edx
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, edx
            and ecx, 3
            rep movsb
    L00513bf6:
            mov eax, dword ptr [gGraf_spec_index]
            lea edi, [esp + 0x24]
            lea esi, [esp + 0x24]
            lea ecx, [eax + eax*2]
            mov dword ptr [esp + 0x20], esi
            lea edx, [eax + ecx*4]
            or ecx, 0xffffffff
            xor eax, eax
            repne scasb
            not ecx
            sub edi, ecx
            mov edx, dword ptr [edx*4 + gGraf_specs+0x1c]
            mov eax, ecx
            mov esi, edi
            mov edi, dword ptr [esp + 0x20]
            shr ecx, 2
            rep movsd
            mov ecx, eax
            xor eax, eax
            and ecx, 3
            rep movsb
            mov edi, edx
            or ecx, 0xffffffff
            repne scasb
            not ecx
            dec ecx
            je L00513c9b
            mov edi, dword ptr [gDir_separator]
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            lea ebp, [esp + 0x24]
            mov esi, edi
            mov edi, ebp
            mov ebp, ecx
            or ecx, 0xffffffff
            repne scasb
            mov ecx, ebp
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, ebp
            lea ebp, [esp + 0x24]
            and ecx, 3
            rep movsb
            mov edi, edx
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            mov esi, edi
            mov edx, ecx
            mov edi, ebp
            or ecx, 0xffffffff
            repne scasb
            mov ecx, edx
            dec edi
            shr ecx, 2
            rep movsd
            mov ebp, dword ptr [esp + 0x14]
            mov ecx, edx
            and ecx, 3
            rep movsb
    L00513c9b:
            lea edi, [esp + 0x24]
            or ecx, 0xffffffff
            xor eax, eax
            lea edx, [esp + 0x24]
            repne scasb
            not ecx
            sub edi, ecx
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            shr ecx, 2
            rep movsd
            mov ecx, eax
            xor eax, eax
            and ecx, 3
            rep movsb
            mov edi, dword ptr [esp + 0x10]
            or ecx, 0xffffffff
            repne scasb
            not ecx
            dec ecx
            je L00513d28
            mov edi, dword ptr [gDir_separator]
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            lea edx, [esp + 0x24]
            mov esi, edi
            mov edi, edx
            mov edx, ecx
            or ecx, 0xffffffff
            repne scasb
            mov ecx, edx
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, edx
            lea edx, [esp + 0x24]
            and ecx, 3
            rep movsb
            mov edi, dword ptr [esp + 0x10]
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            mov esi, edi
            mov edi, edx
            mov edx, ecx
            or ecx, 0xffffffff
            repne scasb
            mov ecx, edx
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, edx
            and ecx, 3
            rep movsb
    L00513d28:
            mov edi, offset gApplication_path
            or ecx, 0xffffffff
            xor eax, eax
            lea edx, [esp + 0x224]
            repne scasb
            not ecx
            sub edi, ecx
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            shr ecx, 2
            rep movsd
            mov ecx, eax
            xor eax, eax
            and ecx, 3
            rep movsb
            mov edi, offset gCurrent_load_directory
            or ecx, 0xffffffff
            repne scasb
            not ecx
            dec ecx
            je L00513dc1
            mov edi, dword ptr [gDir_separator]
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            lea edx, [esp + 0x224]
            mov esi, edi
            mov edi, edx
            mov edx, ecx
            or ecx, 0xffffffff
            repne scasb
            mov ecx, edx
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, edx
            lea edx, [esp + 0x224]
            and ecx, 3
            rep movsb
            mov edi, offset gCurrent_load_directory
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            mov esi, edi
            mov edi, edx
            mov edx, ecx
            or ecx, 0xffffffff
            repne scasb
            mov ecx, edx
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, edx
            and ecx, 3
            rep movsb
    L00513dc1:
            test ebp, ebp
            je L00513e70
            test ebx, ebx
            je L00513e70
            lea edi, [esp + 0x224]
            or ecx, 0xffffffff
            xor eax, eax
            lea edx, [esp + 0x224]
            repne scasb
            not ecx
            sub edi, ecx
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            shr ecx, 2
            rep movsd
            mov ecx, eax
            xor eax, eax
            and ecx, 3
            rep movsb
            mov edi, offset gCurrent_load_name
            or ecx, 0xffffffff
            repne scasb
            not ecx
            dec ecx
            je L00513e70
            mov edx, dword ptr [gDir_separator]
            or ecx, 0xffffffff
            mov edi, edx
            lea ebx, [esp + 0x224]
            repne scasb
            not ecx
            sub edi, ecx
            mov esi, edi
            mov ebp, ecx
            mov edi, ebx
            or ecx, 0xffffffff
            repne scasb
            mov ecx, ebp
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, ebp
            lea ebx, [esp + 0x224]
            and ecx, 3
            rep movsb
            mov edi, offset gCurrent_load_name
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            mov esi, edi
            mov ebp, ecx
            mov edi, ebx
            or ecx, 0xffffffff
            repne scasb
            mov ecx, ebp
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, ebp
            and ecx, 3
            rep movsb
            jmp L00513e76
    L00513e70:
            mov edx, dword ptr [gDir_separator]
    L00513e76:
            lea edi, [esp + 0x224]
            or ecx, 0xffffffff
            xor eax, eax
            lea ebx, [esp + 0x224]
            repne scasb
            not ecx
            sub edi, ecx
            mov eax, ecx
            mov esi, edi
            mov edi, ebx
            mov ebx, dword ptr [esp + 0x10]
            shr ecx, 2
            rep movsd
            mov ecx, eax
            xor eax, eax
            and ecx, 3
            rep movsb
            mov edi, ebx
            or ecx, 0xffffffff
            repne scasb
            not ecx
            dec ecx
            je L00513f0b
            mov edi, edx
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            lea ebp, [esp + 0x224]
            mov esi, edi
            mov edi, ebp
            mov ebp, ecx
            or ecx, 0xffffffff
            repne scasb
            mov ecx, ebp
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, ebp
            lea ebp, [esp + 0x224]
            and ecx, 3
            rep movsb
            mov edi, ebx
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            mov esi, edi
            mov ebx, ecx
            mov edi, ebp
            or ecx, 0xffffffff
            repne scasb
            mov ecx, ebx
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, ebx
            and ecx, 3
            rep movsb
    L00513f0b:
            lea edi, [esp + 0x24]
            or ecx, 0xffffffff
            xor eax, eax
            lea ebx, [esp + 0x124]
            repne scasb
            not ecx
            sub edi, ecx
            mov eax, ecx
            mov esi, edi
            mov edi, ebx
            shr ecx, 2
            rep movsd
            mov ecx, eax
            xor eax, eax
            and ecx, 3
            rep movsb
            mov edi, offset gLast_key_down+0xa4
            or ecx, 0xffffffff
            repne scasb
            not ecx
            dec ecx
            je L00513f9f
            mov edi, edx
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            lea ebx, [esp + 0x124]
            mov esi, edi
            mov edx, ecx
            mov edi, ebx
            or ecx, 0xffffffff
            repne scasb
            mov ecx, edx
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, edx
            lea edx, [esp + 0x124]
            and ecx, 3
            rep movsb
            mov edi, offset gLast_key_down+0xa4
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            mov esi, edi
            mov ebx, ecx
            mov edi, edx
            or ecx, 0xffffffff
            repne scasb
            mov ecx, ebx
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, ebx
            and ecx, 3
            rep movsb
    L00513f9f:
            lea eax, [esp + 0x424]
            mov edx, offset GetFileName
            push eax
            lea ecx, [esp + 0x128]
            call PFForEveryFile2
            lea edi, [esp + 0x24]
            or ecx, 0xffffffff
            xor eax, eax
            lea edx, [esp + 0x324]
            repne scasb
            not ecx
            sub edi, ecx
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            shr ecx, 2
            rep movsd
            mov ecx, eax
            xor eax, eax
            and ecx, 3
            rep movsb
            mov edi, offset gLast_key_down+0x84
            or ecx, 0xffffffff
            repne scasb
            not ecx
            dec ecx
            je L00514050
            mov edi, dword ptr [gDir_separator]
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            lea edx, [esp + 0x324]
            mov esi, edi
            mov ebx, ecx
            mov edi, edx
            or ecx, 0xffffffff
            repne scasb
            mov ecx, ebx
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, ebx
            lea edx, [esp + 0x324]
            and ecx, 3
            rep movsb
            mov edi, offset gLast_key_down+0x84
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            mov esi, edi
            mov ebx, ecx
            mov edi, edx
            or ecx, 0xffffffff
            repne scasb
            mov ecx, ebx
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, ebx
            and ecx, 3
            rep movsb
    L00514050:
            lea eax, [esp + 0x424]
            mov edx, offset GetAdditionalFileName
            push eax
            lea ecx, [esp + 0x328]
            call PFForEveryFile2
            lea edi, [esp + 0x24]
            or ecx, 0xffffffff
            xor eax, eax
            lea edx, [esp + 0x124]
            repne scasb
            not ecx
            sub edi, ecx
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            shr ecx, 2
            rep movsd
            mov ecx, eax
            xor eax, eax
            and ecx, 3
            rep movsb
            mov edi, offset gLast_key_down+0x9c
            or ecx, 0xffffffff
            repne scasb
            not ecx
            dec ecx
            je L00514101
            mov edi, dword ptr [gDir_separator]
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            lea edx, [esp + 0x124]
            mov esi, edi
            mov ebx, ecx
            mov edi, edx
            or ecx, 0xffffffff
            repne scasb
            mov ecx, ebx
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, ebx
            lea edx, [esp + 0x124]
            and ecx, 3
            rep movsb
            mov edi, offset gLast_key_down+0x9c
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            mov esi, edi
            mov ebx, ecx
            mov edi, edx
            or ecx, 0xffffffff
            repne scasb
            mov ecx, ebx
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, ebx
            and ecx, 3
            rep movsb
    L00514101:
            lea eax, [esp + 0x424]
            mov edx, offset GetAdditionalFileName
            push eax
            lea ecx, [esp + 0x128]
            call PFForEveryFile2
            lea edi, [esp + 0x24]
            or ecx, 0xffffffff
            xor eax, eax
            lea edx, [esp + 0x324]
            repne scasb
            not ecx
            sub edi, ecx
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            shr ecx, 2
            rep movsd
            mov ecx, eax
            xor eax, eax
            and ecx, 3
            rep movsb
            mov edi, offset gLast_key_down+0x7c
            or ecx, 0xffffffff
            repne scasb
            not ecx
            dec ecx
            je L005141b2
            mov edi, dword ptr [gDir_separator]
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            lea edx, [esp + 0x324]
            mov esi, edi
            mov ebx, ecx
            mov edi, edx
            or ecx, 0xffffffff
            repne scasb
            mov ecx, ebx
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, ebx
            lea edx, [esp + 0x324]
            and ecx, 3
            rep movsb
            mov edi, offset gLast_key_down+0x7c
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            mov esi, edi
            mov ebx, ecx
            mov edi, edx
            or ecx, 0xffffffff
            repne scasb
            mov ecx, ebx
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, ebx
            and ecx, 3
            rep movsb
    L005141b2:
            lea eax, [esp + 0x424]
            mov edx, offset GetAdditionalFileName
            push eax
            lea ecx, [esp + 0x328]
            call PFForEveryFile2
            mov eax, dword ptr [esp + 0x424]
            test eax, eax
            je L0051422b
            xor ebx, ebx
            test eax, eax
            jle L0051454b
            mov edi, dword ptr [esp + 0x18]
            lea esi, [esp + 0x428]
    L005141eb:
            mov edx, dword ptr [gPixelFlags]
            mov eax, dword ptr [gRender_palette]
            lea ecx, [esp + 0x1c]
            push ecx
            push edx
            push eax
            mov edx, esi
            lea ecx, [esp + 0x30]
            call DRLdImg
            mov dword ptr [edi], eax
            inc ebx
            mov eax, dword ptr [esp + 0x424]
            add esi, 0x10
            add edi, 4
            cmp ebx, eax
            jl L005141eb
            mov eax, ebx
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x4298
            ret 4
    L0051422b:
            lea edi, [esp + 0x224]
            or ecx, 0xffffffff
            xor eax, eax
            lea edx, [esp + 0x124]
            repne scasb
            not ecx
            sub edi, ecx
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            shr ecx, 2
            rep movsd
            mov ecx, eax
            xor eax, eax
            and ecx, 3
            rep movsb
            mov edi, offset gLast_key_down+0xa4
            or ecx, 0xffffffff
            repne scasb
            not ecx
            dec ecx
            je L005142c6
            mov edi, dword ptr [gDir_separator]
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            lea edx, [esp + 0x124]
            mov esi, edi
            mov ebx, ecx
            mov edi, edx
            or ecx, 0xffffffff
            repne scasb
            mov ecx, ebx
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, ebx
            lea edx, [esp + 0x124]
            and ecx, 3
            rep movsb
            mov edi, offset gLast_key_down+0xa4
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            mov esi, edi
            mov ebx, ecx
            mov edi, edx
            or ecx, 0xffffffff
            repne scasb
            mov ecx, ebx
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, ebx
            and ecx, 3
            rep movsb
    L005142c6:
            lea eax, [esp + 0x424]
            mov edx, offset GetFileName
            push eax
            lea ecx, [esp + 0x128]
            call PFForEveryFile2
            lea edi, [esp + 0x224]
            or ecx, 0xffffffff
            xor eax, eax
            lea edx, [esp + 0x324]
            repne scasb
            not ecx
            sub edi, ecx
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            shr ecx, 2
            rep movsd
            mov ecx, eax
            xor eax, eax
            and ecx, 3
            rep movsb
            mov edi, offset gLast_key_down+0x84
            or ecx, 0xffffffff
            repne scasb
            not ecx
            dec ecx
            je L0051437a
            mov edi, dword ptr [gDir_separator]
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            lea edx, [esp + 0x324]
            mov esi, edi
            mov ebx, ecx
            mov edi, edx
            or ecx, 0xffffffff
            repne scasb
            mov ecx, ebx
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, ebx
            lea edx, [esp + 0x324]
            and ecx, 3
            rep movsb
            mov edi, offset gLast_key_down+0x84
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            mov esi, edi
            mov ebx, ecx
            mov edi, edx
            or ecx, 0xffffffff
            repne scasb
            mov ecx, ebx
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, ebx
            and ecx, 3
            rep movsb
    L0051437a:
            lea eax, [esp + 0x424]
            mov edx, offset GetAdditionalFileName
            push eax
            lea ecx, [esp + 0x328]
            call PFForEveryFile2
            lea edi, [esp + 0x224]
            or ecx, 0xffffffff
            xor eax, eax
            lea edx, [esp + 0x124]
            repne scasb
            not ecx
            sub edi, ecx
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            shr ecx, 2
            rep movsd
            mov ecx, eax
            xor eax, eax
            and ecx, 3
            rep movsb
            mov edi, offset gLast_key_down+0x9c
            or ecx, 0xffffffff
            repne scasb
            not ecx
            dec ecx
            je L0051442e
            mov edi, dword ptr [gDir_separator]
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            lea edx, [esp + 0x124]
            mov esi, edi
            mov ebx, ecx
            mov edi, edx
            or ecx, 0xffffffff
            repne scasb
            mov ecx, ebx
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, ebx
            lea edx, [esp + 0x124]
            and ecx, 3
            rep movsb
            mov edi, offset gLast_key_down+0x9c
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            mov esi, edi
            mov ebx, ecx
            mov edi, edx
            or ecx, 0xffffffff
            repne scasb
            mov ecx, ebx
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, ebx
            and ecx, 3
            rep movsb
    L0051442e:
            lea eax, [esp + 0x424]
            mov edx, offset GetAdditionalFileName
            push eax
            lea ecx, [esp + 0x128]
            call PFForEveryFile2
            lea edi, [esp + 0x224]
            or ecx, 0xffffffff
            xor eax, eax
            lea edx, [esp + 0x324]
            repne scasb
            not ecx
            sub edi, ecx
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            shr ecx, 2
            rep movsd
            mov ecx, eax
            xor eax, eax
            and ecx, 3
            rep movsb
            mov edi, offset gLast_key_down+0x7c
            or ecx, 0xffffffff
            repne scasb
            not ecx
            dec ecx
            je L005144e2
            mov edi, dword ptr [gDir_separator]
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            lea edx, [esp + 0x324]
            mov esi, edi
            mov ebx, ecx
            mov edi, edx
            or ecx, 0xffffffff
            repne scasb
            mov ecx, ebx
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, ebx
            lea edx, [esp + 0x324]
            and ecx, 3
            rep movsb
            mov edi, offset gLast_key_down+0x7c
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            mov esi, edi
            mov ebx, ecx
            mov edi, edx
            or ecx, 0xffffffff
            repne scasb
            mov ecx, ebx
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, ebx
            and ecx, 3
            rep movsb
    L005144e2:
            lea eax, [esp + 0x424]
            mov edx, offset GetAdditionalFileName
            push eax
            lea ecx, [esp + 0x328]
            call PFForEveryFile2
            mov eax, dword ptr [esp + 0x424]
            test eax, eax
            je L0051455a
            xor ebx, ebx
            test eax, eax
            jle L0051454b
            mov edi, dword ptr [esp + 0x18]
            lea esi, [esp + 0x428]
    L00514517:
            mov edx, dword ptr [gPixelFlags]
            mov eax, dword ptr [gRender_palette]
            lea ecx, [esp + 0x1c]
            push ecx
            push edx
            push eax
            mov edx, esi
            lea ecx, [esp + 0x230]
            call DRLdImg
            mov dword ptr [edi], eax
            inc ebx
            mov eax, dword ptr [esp + 0x424]
            add esi, 0x10
            add edi, 4
            cmp ebx, eax
            jl L00514517
    L0051454b:
            mov eax, ebx
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x4298
            ret 4
    L0051455a:
            pop edi
            pop esi
            pop ebp
            xor eax, eax
            pop ebx
            add esp, 0x4298
            ret 4
    }
#endif
}

int C2_HOOK_FASTCALL GetBlendificatiousnessOfMaterialTablishly(br_material *pMaterial) {

    if (pMaterial->index_blend == BrTableFind("BLEND75.TAB")) {
        return 25;
    }
    if (pMaterial->index_blend == BrTableFind("BLEND50.TAB")) {
        return 50;
    }
    if (pMaterial->index_blend == BrTableFind("BLEND25.TAB")) {
        return 75;
    }
    return 100;
}

int C2_HOOK_FASTCALL GetBlendificatiousnessOfMaterialPrimitively(br_material *pMaterial) {
    br_token_value* prims = pMaterial->extra_prim;

    if (prims != NULL && prims[0].t == BRT_BLEND_B && prims[0].v.b && prims[1].t == BRT_OPACITY_X) {
        return (100 * BrFixedToInt(prims[1].v.x)) >> 8;
    }
    return 100;
}

int C2_HOOK_FASTCALL GetBlenficatiousnessOfMaterialTablishly(br_material *pMaterial) {

    if (pMaterial->index_blend == BrTableFind("BLEND75.TAB")) {
        return 25;
    }
    if (pMaterial->index_blend == BrTableFind("BLEND50.TAB")) {
        return 50;
    }
    if (pMaterial->index_blend == BrTableFind("BLEND25.TAB")) {
        return 75;
    }
    return 100;
}

int C2_HOOK_FASTCALL GetBlenficatiousnessOfMaterialPrimitively(br_material *pMaterial) {
    br_token_value* prims = pMaterial->extra_prim;

    if (prims != NULL && prims[0].t == BRT_BLEND_B && prims[0].v.b && prims[1].t == BRT_OPACITY_X) {
        return (100 * BrFixedToInt(prims[1].v.x)) >> 8;
    }
    return 100;
}

// FUNCTION: CARMA2_HW 0x00518e70
int C2_HOOK_FASTCALL GetBlendificatiousnessOfMaterial(br_material* pMaterial) {
    br_token_value* prims;

    if (gScreen->type == BR_PMT_INDEX_8) {
        if (pMaterial->index_blend == BrTableFind("BLEND75.TAB")) {
            return 25;
        }
        if (pMaterial->index_blend == BrTableFind("BLEND50.TAB")) {
            return 50;
        }
        if (pMaterial->index_blend == BrTableFind("BLEND25.TAB")) {
            return 75;
        }
        return 100;
    }
    prims = pMaterial->extra_prim;
    if (prims != NULL && prims[0].t == BRT_BLEND_B && prims[0].v.b == 1 && prims[1].t == BRT_OPACITY_X) {
        return (100 * (prims[1].v.x >> 16)) / 256;
    }
    return 100;
}

// FUNCTION: CARMA2_HW 0x00517050
tU16 C2_NAKED C2_HOOK_FASTCALL Colour24BitTo16Bit(br_colour pColour) {
#ifndef CARPOCALYPSE2_MATCHING

    switch (gBack_screen->type) {
    default:
        BrFailure("Unsupported back buffer type.");
        break;
    case BR_PMT_RGB_565:
        return (((((pColour >> 3) & 0x1f0000) | (pColour & 0xfc00)) >> 2) | (pColour & 0xf8)) >> 3;
    case BR_PMT_RGB_555:
        return (((((pColour >> 3) & 0x1f0000) | (pColour & 0xf800)) >> 3) | (pColour & 0xf8)) >> 3;
    }
#else
    __asm {
            mov edx, dword ptr [gBack_screen]
            xor eax, eax
            mov al, byte ptr [edx + 0x2c]
            sub eax, 4
            je L00517094
            dec eax
            je L00517071
            push offset gVector3_00661678+0xa8
            call BrFailure
            add esp, 4
            ret
    L00517071:
            mov eax, ecx
            mov edx, ecx
            shr eax, 3
            and eax, 0x1f0000
            and edx, 0xfc00
            or eax, edx
            and ecx, 0xf8
            shr eax, 2
            or eax, ecx
            shr eax, 3
            ret
    L00517094:
            mov eax, ecx
            mov edx, ecx
            shr eax, 3
            and eax, 0x1f0000
            and edx, 0xf800
            or eax, edx
            and ecx, 0xf8
            shr eax, 3
            or eax, ecx
            shr eax, 3
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00514930
int C2_HOOK_FASTCALL sign(int pNumber) {
    if (pNumber > 0) {
        return 1;
    } else if (pNumber < 0) {
        return -1;
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x005146c0
void C2_HOOK_FASTCALL WaitFor(tU32 pDelay) {
    tU32 start_time;

    start_time = PDGetTotalTime();
    while (PDGetTotalTime() < start_time + pDelay && !AnyKeyDown()) {
        SoundService();
    }
}
#include "network.h"
#include "init.h"
#include "loading1.h"
#include "world3.h"
#include "graphics2.h"
#include "main.h"
#include "input.h"
#include "mainmenu.h"
#include "errors.h"
#include "loading3.h"
#include "sound.h"
#include "packfile.h"
#include "globvars.h"
#include "globvrpb.h"
#include "platform.h"
#include "carpocalypse2_macros.h"

#include "c2_string.h"

#include <ctype.h>
#include <float.h>


// FUNCTION: CARMA2_HW 0x00513400
br_error C2_HOOK_FASTCALL DRBrEnd(void) {
    br_device *dev;

    if (!gBr_initialized) {
        return 0x1006;
    }

    _BrEndHook();
    gBr_initialized = 0;
    while (BrDevFind(&dev, NULL) == 0) {
        if (dev != NULL) {
            BrDevRemove(dev);
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00513460
void C2_HOOK_FASTCALL Uppercaseificate(char* dest, const char* src) {
    int len;
    int i;

    len = (int)strlen(src);
    for (i = 0; i < (int)len; i++) {
        dest[i] = toupper(src[i]);
    }
    dest[len] = '\0';
}

// FUNCTION: CARMA2_HW 0x005134b0
int C2_HOOK_FASTCALL CheckQuit(void) {
    int result;

    result = 0;
    if (!gIn_check_quit) {
        if (KeyIsDown(1) && KeyIsDown(7)) {
            gIn_check_quit = 1;
            do {
            } while (AnyKeyDown());
            result = 1;
            if (DoVerifyQuit(1)) {
                QuitGame();
            }
            gIn_check_quit = 0;
        }
    }
    return result;
}

// FUNCTION: CARMA2_HW 0x00513510
double C2_HOOK_FASTCALL sqr(double pN) {
    return pN * pN;
}

// FUNCTION: CARMA2_HW 0x00513520
int C2_HOOK_FASTCALL IRandomBetween(int pA, int pB) {
    int num;

#if RAND_MAX == 0x7fff
    num = (pB + 1 - pA) * rand() / (RAND_MAX + 1) + pA;
    return num;
#else
    num = (pB + 1 - pA) * (rand() % 0x8000) / (0x7fff + 1) + pA;
    return num;
#endif
}

// FUNCTION: CARMA2_HW 0x00513550
int C2_HOOK_FASTCALL PercentageChance(int pC) {
    if (pC == 0) {
        return 0;
    }
    return IRandomBetween(0, 99) < pC;
}

// FUNCTION: CARMA2_HW 0x00513580
int C2_HOOK_FASTCALL IRandomPosNeg(int pN) {
    return IRandomBetween(-pN, pN);
}

// FUNCTION: CARMA2_HW 0x005135b0
float C2_HOOK_STDCALL FRandomBetween(float pA, float pB) {

#if RAND_MAX == 0x7fff
    return (float)rand() * (pB - pA) / (float)RAND_MAX + pA;
#else
    return (float)(rand() % 0x10000) * (pB - pA) / (float)(0x7fff + 1) + pA;
#endif
}

// FUNCTION: CARMA2_HW 0x005135e0
float C2_HOOK_STDCALL FRandomPosNeg(float pN) {

    return FRandomBetween(-pN, pN);
}

// FUNCTION: CARMA2_HW 0x00513620
br_scalar C2_HOOK_STDCALL SRandomBetween(br_scalar pA, br_scalar pB) {

    return FRandomBetween(pA, pB);
}

// FUNCTION: CARMA2_HW 0x00513650
br_scalar C2_HOOK_STDCALL SRandomPosNeg(br_scalar pN) {

    return SRandomBetween(-pN, pN);
}

// FUNCTION: CARMA2_HW 0x00513690
void C2_HOOK_FASTCALL PathCat(char* pDestn_str, const char* pStr_1, const char* pStr_2) {
#ifdef CARPOCALYPSE2_FIX_BUGS
    if (pDestn_str != pStr_1) {
 // Added to avoid strcpy overlap checks
        strcpy(pDestn_str, pStr_1);
    }
#else
    strcpy(pDestn_str, pStr_1);
#endif

    if (strlen(pStr_2) != 0) {
        strcat(pDestn_str, gDir_separator);
        strcat(pDestn_str, pStr_2);
    }
}

// tandeg

// FUNCTION: CARMA2_HW 0x00513790
tU32 C2_HOOK_FASTCALL GetFileLength(FILE* pF) {
    tU32 the_size;

    PFfseek(pF, 0, SEEK_END);
    the_size = PFftell(pF);
    PFrewind(pF);
    return the_size;
}

// FUNCTION: CARMA2_HW 0x005137d0
br_pixelmap* C2_HOOK_FASTCALL DRPixelmapAllocate(br_uint_8 pType, br_uint_16 pW, br_uint_16 pH, void* pPixels, int pFlags) {
    br_pixelmap* pm;

    pm = BrPixelmapAllocate(pType, pW, pH, pPixels, pFlags);
    if (pm != NULL) {
        pm->origin_x = 0;
        pm->origin_y = 0;
    }
    return pm;
}

// FUNCTION: CARMA2_HW 0x00513820
br_pixelmap* C2_HOOK_FASTCALL DRPixelmapAllocateSub(br_pixelmap* pPm, br_uint_16 pX, br_uint_16 pY, br_uint_16 pW, br_uint_16 pH) {
    br_pixelmap* the_map;

    the_map = BrPixelmapAllocateSub(pPm, pX, pY, pW, pH);
    if (the_map != NULL) {
        the_map->origin_x = 0;
        the_map->origin_y = 0;
    }
    return the_map;
}

// FUNCTION: CARMA2_HW 0x00513870
br_pixelmap* C2_HOOK_FASTCALL DRImageLoad(const char* path) {
    int errorFlags;
    tPath_name pathBuffer;
    tPath_name pathDir;
    tPath_name textureName;
    size_t fileStart;
    size_t pathLen;
    size_t i;

    strcpy(pathBuffer, path);
    pathLen = strlen(pathBuffer);
    fileStart = 0;
    for (i = pathLen - 1; i != 0; i--) {
        if (pathBuffer[i] == *gDir_separator) {
            fileStart = i;
            break;
        }
    }
    memcpy(pathDir, pathBuffer, fileStart);
    pathDir[fileStart] = '\0';
    if (pathDir[0] != '\0') {
        fileStart += 1;
    }
    for (i = 0; pathBuffer[fileStart + i] != '.' && fileStart + i != pathLen; i++) {
        textureName[i] = pathBuffer[fileStart + i];
    }
    textureName[i] = '\0';
    return DRLdImg(pathDir, textureName, gRender_palette, gPixelFlags, &errorFlags);
}

// DRPixelmapLoad

// FUNCTION: CARMA2_HW 0x005139a0
void C2_HOOK_FASTCALL SepDirAndFilename(const char* path, char* dirPath, char* stemPath) {
    size_t pathLen;
    size_t dirLen;
    size_t i;

    pathLen = strlen(path);
    dirLen = 0;
    for (i = pathLen - 1; ; i--) {
        if (i == 0) {
            break;
        }
        if (path[i] == gDir_separator[0]) {
            dirLen = i;
            break;
        }
    }
    memcpy(dirPath, path, dirLen);
    dirPath[dirLen] = '\0';
    if (*dirPath != '\0') {
        dirLen += 1;
    }
    for (i = 0; path[dirLen + i] != '.' && dirLen + i != pathLen; i++) {
        stemPath[i] = path[dirLen + i];
    }
    stemPath[i] = '\0';
}

// DRLoadMultiplePix

// FUNCTION: CARMA2_HW 0x00514570
int C2_NAKED C2_HOOK_FASTCALL DRPixelmapLoadMany(const char* texturePathNoExt, br_pixelmap** pixelmaps, size_t capacity) {
#ifndef CARPOCALYPSE2_MATCHING
    tPath_name texturePath;
    tPath_name texturePathDir;
    tPath_name texturePathStem;
    int errorCode;

    strcpy(texturePath, texturePathNoExt);
    strcat(texturePath, ".TIF");
    SepDirAndFilename(texturePath, texturePathDir, texturePathStem);
    pixelmaps[0] = DRLdImg(texturePathDir, texturePathStem, gRender_palette, gPixelFlags, &errorCode);
    return (pixelmaps[0] != NULL && errorCode == 0) ? 1 : 0;
#else
    __asm {
            sub esp, 0x304
            xor eax, eax
            push ebx
            push ebp
            push esi
            push edi
            mov edi, ecx
            or ecx, 0xffffffff
            repne scasb
            not ecx
            mov ebx, edx
            sub edi, ecx
            lea edx, [esp + 0x14]
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            shr ecx, 2
            rep movsd
            mov ecx, eax
            xor eax, eax
            and ecx, 3
            lea edx, [esp + 0x14]
            rep movsb
            mov edi, offset gFrontend_QUIT+0xbbac
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            mov esi, edi
            mov ebp, ecx
            mov edi, edx
            or ecx, 0xffffffff
            repne scasb
            mov ecx, ebp
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, ebp
            xor edx, edx
            and ecx, 3
            rep movsb
            lea edi, [esp + 0x14]
            or ecx, 0xffffffff
            repne scasb
            not ecx
            dec ecx
            mov ebp, ecx
            lea eax, [ebp - 1]
            test eax, eax
            je L005145f9
            mov ecx, dword ptr [gDir_separator]
            mov cl, byte ptr [ecx]
    L005145ec:
            cmp byte ptr [esp + eax + 0x14], cl
            je L005145f7
            dec eax
            jne L005145ec
            jmp L005145f9
    L005145f7:
            mov edx, eax
    L005145f9:
            mov ecx, edx
            lea esi, [esp + 0x14]
            mov eax, ecx
            lea edi, [esp + 0x214]
            shr ecx, 2
            rep movsd
            mov ecx, eax
            and ecx, 3
            rep movsb
            mov byte ptr [esp + edx + 0x214], 0
            mov al, byte ptr [esp + 0x214]
            test al, al
            je L00514628
            inc edx
    L00514628:
            lea ecx, [esp + edx + 0x14]
            xor eax, eax
            cmp byte ptr [ecx], 0x2e
            je L0051465f
            mov esi, edx
            lea edx, [esp + 0x114]
            sub esi, edx
    L0051463e:
            lea edx, [esp + eax + 0x114]
            add edx, esi
            cmp edx, ebp
            je L0051465f
            mov dl, byte ptr [eax + ecx]
            mov byte ptr [esp + eax + 0x114], dl
            mov dl, byte ptr [eax + ecx + 1]
            inc eax
            cmp dl, 0x2e
            jne L0051463e
    L0051465f:
            mov ecx, dword ptr [gPixelFlags]
            mov edx, dword ptr [gRender_palette]
            mov byte ptr [esp + eax + 0x114], 0
            lea eax, [esp + 0x10]
            push eax
            push ecx
            push edx
            lea edx, [esp + 0x120]
            lea ecx, [esp + 0x220]
            call DRLdImg
            test eax, eax
            mov dword ptr [ebx], eax
            je L005146ad
            mov eax, dword ptr [esp + 0x10]
            test eax, eax
            jne L005146ad
            mov eax, 1
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x304
            ret 4
    L005146ad:
            pop edi
            pop esi
            pop ebp
            xor eax, eax
            pop ebx
            add esp, 0x304
            ret 4
    }
#endif
}

// WaitFor

// FUNCTION: CARMA2_HW 0x005146f0
intptr_t C2_HOOK_FASTCALL DRActorEnumRecurse(br_actor* pActor, br_actor_enum_cbfn* callback, void* arg) {
    intptr_t result;

    result = callback(pActor, arg);
    if (result != 0) {
        return result;
    }
    for (pActor = pActor->children; pActor != NULL; pActor = pActor->next) {
        result = DRActorEnumRecurse(pActor, callback, arg);
        if (result != 0) {
            return result;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x005147b0
intptr_t C2_HOOK_CDECL CompareActorID(br_actor* pActor, void* pArg) {
    if (pActor->identifier != NULL && strcmp(pActor->identifier, (const char*)pArg) == 0) {
        return (intptr_t)pActor;
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x00514730
br_actor* C2_HOOK_FASTCALL DRActorFindRecurse(br_actor* pSearch_root, const char* pName) {
    return (br_actor*)DRActorEnumRecurse(pSearch_root, CompareActorID, (void*)pName);
}

// FUNCTION: CARMA2_HW 0x00514800
br_uint_32 C2_HOOK_FASTCALL DRActorEnumRecurseWithMat(br_actor* pActor, br_material* pMat, recurse_with_mat_cbfn* pCall_back, void* pArg) {
    br_uint_32 result;

    if (pActor->material != NULL) {
        pMat = pActor->material;
    }
    result = pCall_back(pActor, pMat, pArg);
    if (result != 0) {
        return result;
    }
    for (pActor = pActor->children; pActor != NULL; pActor = pActor->next) {
        result = DRActorEnumRecurseWithMat(pActor, pMat, pCall_back, pArg);
        if (result != 0) {
            return result;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00514850
br_uint_32 C2_HOOK_FASTCALL DRActorEnumRecurseWithTrans(br_actor* pActor, br_matrix34* pMatrix, recurse_with_trans_cbfn* pCall_back, void* pArg) {
    br_uint_32 result;
    br_matrix34 combined_transform;

    if (pMatrix == NULL) {
        BrMatrix34Copy(&combined_transform, &pActor->t.t.mat);
    } else {
        BrMatrix34Mul(&combined_transform, pMatrix, &pActor->t.t.mat);
    }
    result = pCall_back(pActor, &combined_transform, pArg);
    if (result != 0) {
        return result;
    }
    for (pActor = pActor->children; pActor != NULL; pActor = pActor->next) {
        result = DRActorEnumRecurseWithTrans(pActor, &combined_transform, pCall_back, pArg);
        if (result != 0) {
            return result;
        }
    }
    return 0;
}

// DRActorEnumRecurseWithSnart

// sign

// FUNCTION: CARMA2_HW 0x005149a0
FILE* C2_HOOK_FASTCALL OpenUniqueFileB(char* pPrefix, char* pExtension) {
    int index;
    FILE* f;
    tPath_name the_path;

    for (index = 0; index < 10000; index++) {
        PathCat(the_path, gApplication_path, pPrefix);
        sprintf(the_path + strlen(the_path), "%04d.%s", index, pExtension);
        f = DRfopen(the_path, "rt");
        if (f == NULL) {
            return DRfopen(the_path, "wb");
        }
        PFfclose(f);
    }
    return NULL;
}

// FUNCTION: CARMA2_HW 0x00514ab0
void C2_NAKED C2_HOOK_FASTCALL PrintScreenFile(FILE* pF) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;
    int j;
    int bit_map_size;
    tU8* pixel_ptr;

    bit_map_size = gBack_screen->height * gBack_screen->row_bytes;

    // 1. BMP Header
    //    1. 'BM' Signature
    WriteU8L(pF, 'B');
    WriteU8L(pF, 'M');
    //    2. File size in bytes (header = 0xe bytes; infoHeader = 0x28 bytes; colorTable = 0x400 bytes; pixelData = xxx)
    WriteU32L(pF, bit_map_size + 0x436);
    //    3. unused
    WriteU16L(pF, 0);
    //    4. unused
    WriteU16L(pF, 0);
    //    5. pixelData offset (from beginning of file)
    WriteU32L(pF, 0x436);

    // 2. Info Header
    //    1. InfoHeader Size
    WriteU32L(pF, 0x28);
    //    2. Width of bitmap in pixels
    WriteU32L(pF, gBack_screen->row_bytes);
    //    3. Height of bitmap in pixels
    WriteU32L(pF, gBack_screen->height);
    //    4. Number of planes
    WriteU16L(pF, 1);
    //    5. Bits per pixels / palletization (8 -> 8bit palletized ==> #colors = 256)
    WriteU16L(pF, 8);
    //    6. Compression (0 = BI_RGB -> no compression)
    WriteU32L(pF, 0);
    //    7. Image Size (0 --> no compression)
    WriteU32L(pF, 0);
    //    8. Horizontal Pixels Per Meter
    WriteU32L(pF, 0);
    //    9. Vertical Pixels Per Meter
    WriteU32L(pF, 0);
    //    10. # Actually used colors
    WriteU32L(pF, 0);
    //    11. Number of important colors
    WriteU32L(pF, 256);

    // 3. Color table (=palette)
    for (i = 0; i < 256; i++) {
        // red, green, blue, unused
        WriteU8L(pF, ((tU8*)gCurrent_palette->pixels)[4 * i + 0]);
        WriteU8L(pF, ((tU8*)gCurrent_palette->pixels)[4 * i + 1]);
        WriteU8L(pF, ((tU8*)gCurrent_palette->pixels)[4 * i + 2]);
        WriteU8L(pF, 0);
    }

    // 4. Pixel Data (=LUT)
    pixel_ptr = (tU8*)gBack_screen->pixels + bit_map_size - gBack_screen->row_bytes;
    for (i = 0; i < gBack_screen->height; i++) {
        for (j = 0; j < gBack_screen->row_bytes; j++) {
            WriteU8L(pF, *pixel_ptr++);
        }
        pixel_ptr -= 2 * gBack_screen->row_bytes;
    }
    WriteU16L(pF, 0);
#else
    __asm {
            mov eax, dword ptr [gBack_screen]
            push ebx
            xor ebx, ebx
            push ebp
            mov bx, word ptr [eax + 0x36]
            push esi
            movsx eax, word ptr [eax + 0x28]
            push edi
            mov esi, ecx
            mov dl, 0x42
            imul ebx, eax
            call WriteU8L
            mov dl, 0x4d
            mov ecx, esi
            call WriteU8L
            lea edx, [ebx + 0x436]
            mov ecx, esi
            call WriteU32L
            xor edx, edx
            mov ecx, esi
            call WriteU16L
            xor edx, edx
            mov ecx, esi
            call WriteU16L
            mov edx, 0x436
            mov ecx, esi
            call WriteU32L
            mov edx, 0x28
            mov ecx, esi
            call WriteU32L
            mov ecx, dword ptr [gBack_screen]
            movsx edx, word ptr [ecx + 0x28]
            mov ecx, esi
            call WriteU32L
            mov eax, dword ptr [gBack_screen]
            xor edx, edx
            mov ecx, esi
            mov dx, word ptr [eax + 0x36]
            call WriteU32L
            mov edx, 1
            mov ecx, esi
            call WriteU16L
            mov edx, 8
            mov ecx, esi
            call WriteU16L
            xor edx, edx
            mov ecx, esi
            call WriteU32L
            xor edx, edx
            mov ecx, esi
            call WriteU32L
            xor edx, edx
            mov ecx, esi
            call WriteU32L
            xor edx, edx
            mov ecx, esi
            call WriteU32L
            xor edx, edx
            mov ecx, esi
            call WriteU32L
            mov edx, 0x100
            mov ecx, esi
            call WriteU32L
            xor edi, edi
    L00514b85:
            mov ecx, dword ptr [gCurrent_palette]
            mov edx, dword ptr [ecx + 8]
            mov ecx, esi
            mov dl, byte ptr [edx + edi]
            call WriteU8L
            mov eax, dword ptr [gCurrent_palette]
            mov ecx, dword ptr [eax + 8]
            mov dl, byte ptr [ecx + edi + 1]
            mov ecx, esi
            call WriteU8L
            mov edx, dword ptr [gCurrent_palette]
            mov ecx, esi
            mov eax, dword ptr [edx + 8]
            mov dl, byte ptr [eax + edi + 2]
            call WriteU8L
            xor dl, dl
            mov ecx, esi
            call WriteU8L
            add edi, 4
            cmp edi, 0x400
            jl L00514b85
            mov eax, dword ptr [gBack_screen]
            xor ebp, ebp
            movsx ecx, word ptr [eax + 0x28]
            mov edi, dword ptr [eax + 8]
            add edi, ebx
            sub edi, ecx
            cmp word ptr [eax + 0x36], bp
            jbe L00514c1f
    L00514beb:
            xor ebx, ebx
            test ecx, ecx
            jle L00514c09
    L00514bf1:
            mov dl, byte ptr [edi]
            mov ecx, esi
            inc edi
            call WriteU8L
            mov eax, dword ptr [gBack_screen]
            inc ebx
            movsx ecx, word ptr [eax + 0x28]
            cmp ebx, ecx
            jl L00514bf1
    L00514c09:
            movsx ecx, word ptr [eax + 0x28]
            mov edx, ecx
            neg edx
            inc ebp
            lea edi, [edi + edx*2]
            xor edx, edx
            mov dx, word ptr [eax + 0x36]
            cmp ebp, edx
            jl L00514beb
    L00514c1f:
            xor edx, edx
            mov ecx, esi
            call WriteU16L
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret
            nop
            nop
            nop
    }
#endif
}

void C2_HOOK_FASTCALL PrintScreenFile16(FILE* pF) {
    int i;
    int j;
    int bit_map_size;
    tU16* pixel_ptr;

    if (gBack_screen->pixels == NULL) {
        BrPixelmapDirectLock(gBack_screen, 1);
    }

    bit_map_size = gBack_screen->height * 3 * gBack_screen->width;

    // 1. BMP Header
    //    1. 'BM' Signature
    WriteU8L(pF, 'B');
    WriteU8L(pF, 'M');
    //    2. File size in bytes (header = 0xe bytes; infoHeader = 0x28 bytes; colorTable = 0x400 bytes; pixelData = xxx)
    WriteU32L(pF, 0x36 + bit_map_size);
    //    3. unused
    WriteU16L(pF, 0);
    //    4. unused
    WriteU16L(pF, 0);
    //    5. pixelData offset (from beginning of file)
    WriteU32L(pF, 0x36);

    // 2. Info Header
    //    1. InfoHeader Size
    WriteU32L(pF, 0x28);
    //    2. Width of bitmap in pixels
    WriteU32L(pF, gBack_screen->width);
    //    3. Height of bitmap in pixels
    WriteU32L(pF, gBack_screen->height);
    //    4. Number of planes
    WriteU16L(pF, 1);
    //    5. Bits per pixels / palletization (0x18 -> 24bit colours ==> #colors = 2^24)
    WriteU16L(pF, 0x18);
    //    6. Compression (0 = BI_RGB -> no compression)
    WriteU32L(pF, 0);
    //    7. Image Size (0 --> no compression)
    WriteU32L(pF, 0);
    //    8. Horizontal Pixels Per Meter
    WriteU32L(pF, 0);
    //    9. Vertical Pixels Per Meter
    WriteU32L(pF, 0);
    //    10. # Actually used colors
    WriteU32L(pF, 0);
    //    11. Number of important colors
    WriteU32L(pF, 256);

    // 4. Pixel Data (=LUT)
    pixel_ptr = (tU16*)((tU8*)gBack_screen->pixels + bit_map_size - gBack_screen->row_bytes);
    for (i = 0; i < gBack_screen->height; i++) {
        for (j = 0; j < gBack_screen->width; j++) {
            tU8 r, g, b;
            tU16 pixel = *pixel_ptr++;
            if (gBack_screen->type == BR_PMT_RGB_565) {
                b = pixel << 3;
                g = (pixel >> 3) & 0xf8;
                r = (pixel >> 8) & 0xf8;
            } else if (gBack_screen->type == BR_PMT_RGB_555) {
                b = pixel << 3;
                g = (pixel >> 2) & 0xf8;
                r = (pixel >> 7) & 0xf8;
            } else {
                b = 0;
                g = 0;
                r = 0;
            }
            WriteU8L(pF, b);
            WriteU8L(pF, g);
            WriteU8L(pF, r);
        }
        pixel_ptr = (tU16*)((tU8*)pixel_ptr + 2 * gBack_screen->width - gBack_screen->row_bytes);
    }
    WriteU16L(pF, 0);

    if (gBack_screen->pixels != NULL) {
        BrPixelmapDirectUnlock(gBack_screen);
    }
}

// FUNCTION: CARMA2_HW 0x00514c30
tU32 C2_NAKED C2_HOOK_FASTCALL GetTotalTime(void) {
#ifndef CARPOCALYPSE2_MATCHING
    if (gAction_replay_mode) {
        return gLast_replay_frame_time;
    }
    if (gNet_mode != eNet_mode_none) {
        return gLast_tick_count + gFrame_period;
    }
    return gFrame_period + gLast_tick_count - gLost_time;
#else
    __asm {
            mov eax, dword ptr [gPed_676914]
            test eax, eax
            je L00514c3f
            mov eax, dword ptr [gLast_replay_frame_time]
            ret
    L00514c3f:
            mov eax, dword ptr [gNet_mode]
            test eax, eax
            je L00514c56
            mov eax, dword ptr [gLast_tick_count]
            mov ecx, dword ptr [gFrame_period]
            add eax, ecx
            ret
    L00514c56:
            mov edx, dword ptr [gLast_tick_count]
            mov eax, dword ptr [gFrame_period]
            mov ecx, dword ptr [gLost_time]
            add eax, edx
            sub eax, ecx
            ret
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00514c70
tU32 C2_HOOK_FASTCALL GetRaceTime(void) {
    return PDGetTotalTime() - gRace_start;
}

// FUNCTION: CARMA2_HW 0x00514c80
void C2_HOOK_FASTCALL AddLostTime(tU32 pLost_time) {
    gLost_time += pLost_time;
}

// FUNCTION: CARMA2_HW 0x00514c90
void C2_HOOK_FASTCALL AssertThisTimeAsCurrentTime(void) {
    tU32 now;

    now = PDGetTotalTime();
    gLost_time = now - gLast_tick_count;
    gLast_tick_count += gLost_time;
}

// FUNCTION: CARMA2_HW 0x00514cb0
void C2_HOOK_FASTCALL TimerString(tU32 pTime, char* pStr, undefined4 pArg3, int pFudge_colon, int pFloat) {
    int seconds;

    seconds = (pTime + 500) / 1000;
    if (pFudge_colon || seconds > 59) {
        if (pArg3) {
            sprintf(pStr, "%d:%02d", seconds / 60, seconds % 60);
        } else {
            sprintf(pStr, "%d:%02d", seconds / 60, seconds % 60);
        }
    } else {
        if (pFloat) {
            sprintf(pStr, "%.1f", (double)pTime * gDOUBLE_0058b270);
        } else {
            sprintf(pStr, "%d", seconds);
        }
    }
}

// FUNCTION: CARMA2_HW 0x00514d70
const char* C2_HOOK_FASTCALL GetMiscString(int pIndex) {
    return gMisc_strings[pIndex];
}

// FUNCTION: CARMA2_HW 0x00514db0
int C2_HOOK_FASTCALL Flash(tU32 pPeriod, tU32* pLast_change, int* pCurrent_state) {
    tU32 the_time;

    the_time = PDGetTotalTime();
    if (the_time - *pLast_change > pPeriod) {
        *pCurrent_state = !*pCurrent_state;
        *pLast_change = the_time;
    }
    return *pCurrent_state;
}


// FUNCTION: CARMA2_HW 0x00514e40
br_pixelmap* C2_HOOK_FASTCALL GenerateShadeTable(int pHeight, br_pixelmap* pPalette, int pRed_mix, int pGreen_mix, int pBlue_mix, float pQuarter, float pHalf, float pThree_quarter) {
    PossibleService();
    return GenerateDarkenedShadeTable(
        pHeight,
        pPalette,
        pRed_mix,
        pGreen_mix,
        pBlue_mix,
        pQuarter,
        pHalf,
        pThree_quarter,
        1.0f);
}

// FUNCTION: CARMA2_HW 0x00514eb0
C2_NAKED br_pixelmap* C2_HOOK_FASTCALL GenerateDarkenedShadeTable(int pHeight, br_pixelmap* pPalette, int pRed_mix, int pGreen_mix, int pBlue_mix, float pQuarter, float pHalf, float pThree_quarter, br_scalar pDarken) {
#ifndef CARPOCALYPSE2_MATCHING
    br_pixelmap* the_table;
    tRGB_colour the_RGB;
    tRGB_colour new_RGB;
    tRGB_colour ref_col;
    br_colour* cp;
    char* tab_ptr;
    char* shade_ptr;
    double f_i;
    double f_total_minus_1;
    double ratio1;
    double ratio2;
    int i;
    int c;

    the_table = LoadGeneratedShadeTable(pRed_mix, pGreen_mix, pBlue_mix);
    if (the_table == NULL) {
        the_table = BrPixelmapAllocate(BR_PMT_INDEX_8, 256, pHeight, NULL, 0);
        if (the_table == NULL) {
            FatalError(kFatalError_CannotLoadAGeneratedShadeTable);
        }
        cp = pPalette->pixels;

        ref_col.red = pRed_mix;
        ref_col.green = pGreen_mix;
        ref_col.blue = pBlue_mix;

        for (c = 0, tab_ptr = the_table->pixels; c < 256; c++, tab_ptr++) {
            the_RGB.red = (int)(((cp[c] >> 16) & 0xff) * pDarken);
            the_RGB.green = (int)(((cp[c] >> 8) & 0xff) * pDarken);
            the_RGB.blue = (int)(((cp[c] >> 0) & 0xff) * pDarken);

            if (pHeight == 1) {
                f_total_minus_1 = 1.;
            } else {
                f_total_minus_1 = pHeight - 1;
            }
            shade_ptr = tab_ptr;
            for (i = 0, shade_ptr = tab_ptr; i < pHeight; i++, shade_ptr += 0x100) {
                if (pHeight == 1) {
                    f_i = 1.f;
                } else {
                    f_i = (float)i;
                }
                ratio1 = f_i / f_total_minus_1;
                if (ratio1 < .5f) {
                    if (ratio1 < .25) {
                        ratio2 = pQuarter * ratio1 * 4.;
                    } else {
                        ratio2 = (ratio1 - .25f) * (pHalf - pQuarter) * 4. + pQuarter;
                    }
                } else {
                    if (ratio1 < 0.75f) {
                        ratio2 = (ratio1 - .5f) * (pThree_quarter - pHalf) * 4. + pHalf;
                    } else {
                        ratio2 = 1.f - (1.f - pThree_quarter) * (1.f - ratio1) * 4.f;
                    }
                }
                new_RGB.red = (int)(ref_col.red * ratio2 + the_RGB.red * (1. - ratio2));
                new_RGB.green = (int)(ref_col.green * ratio2 + the_RGB.green * (1. - ratio2));
                new_RGB.blue = (int)(ref_col.blue * ratio2 + the_RGB.blue * (1. - ratio2));
                *shade_ptr = FindBestMatch(&new_RGB, pPalette);
            }
        }
        SaveGeneratedShadeTable(the_table, pRed_mix, pGreen_mix, pBlue_mix);
    }
    BrTableAdd(the_table);
    return the_table;
#else
    __asm {
            push ebp
            mov ebp, esp
            and esp, 0xfffffff8
            sub esp, 0x1ac
            push ebx
            mov ebx, ecx
            mov ecx, dword ptr [ebp + 8]
            mov dword ptr [esp + 0x50], edx
            mov eax, ecx
            push esi
            cdq
            and edx, 0xf
            push edi
            add eax, edx
            mov edi, offset gMaterial_shading_for_callback+0x4
            sar eax, 4
            add al, 0x41
            mov byte ptr [esp + 0x38], 0x73
            mov byte ptr [esp + 0x14], al
            mov byte ptr [esp + 0x3a], al
            mov eax, ecx
            mov ecx, dword ptr [ebp + 0xc]
            cdq
            xor eax, edx
            mov byte ptr [esp + 0x39], 0x74
            sub eax, edx
            mov byte ptr [esp + 0x40], 0
            and eax, 0xf
            mov dword ptr [esp + 0x24], ebx
            xor eax, edx
            sub eax, edx
            add al, 0x41
            mov byte ptr [esp + 0x17], al
            mov byte ptr [esp + 0x3b], al
            mov eax, ecx
            cdq
            and edx, 0xf
            add eax, edx
            sar eax, 4
            add al, 0x41
            mov byte ptr [esp + 0x16], al
            mov byte ptr [esp + 0x3c], al
            mov eax, ecx
            mov ecx, dword ptr [ebp + 0x10]
            cdq
            xor eax, edx
            sub eax, edx
            and eax, 0xf
            xor eax, edx
            sub eax, edx
            add al, 0x41
            mov byte ptr [esp + 0x13], al
            mov byte ptr [esp + 0x3d], al
            mov eax, ecx
            cdq
            and edx, 0xf
            add eax, edx
            sar eax, 4
            add al, 0x41
            mov byte ptr [esp + 0x15], al
            mov byte ptr [esp + 0x3e], al
            mov eax, ecx
            or ecx, 0xffffffff
            cdq
            xor eax, edx
            sub eax, edx
            and eax, 0xf
            xor eax, edx
            sub eax, edx
            lea edx, [esp + 0x38]
            add al, 0x41
            mov byte ptr [esp + 0x12], al
            mov byte ptr [esp + 0x3f], al
            xor eax, eax
            repne scasb
            not ecx
            sub edi, ecx
            mov esi, edi
            mov edi, edx
            mov edx, ecx
            or ecx, 0xffffffff
            repne scasb
            mov ecx, edx
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, edx
            lea edx, [esp + 0xb8]
            and ecx, 3
            rep movsb
            mov edi, offset gApplication_path
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            shr ecx, 2
            rep movsd
            mov ecx, eax
            xor eax, eax
            and ecx, 3
            rep movsb
            mov edi, offset gSwap_depth_effect_type+0xdc
            or ecx, 0xffffffff
            repne scasb
            not ecx
            dec ecx
            je L0051502d
            mov edi, dword ptr [gDir_separator]
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            lea edx, [esp + 0xb8]
            mov esi, edi
            mov edi, edx
            mov edx, ecx
            or ecx, 0xffffffff
            repne scasb
            mov ecx, edx
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, edx
            lea edx, [esp + 0xb8]
            and ecx, 3
            rep movsb
            mov edi, offset gSwap_depth_effect_type+0xdc
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            mov esi, edi
            mov edi, edx
            mov edx, ecx
            or ecx, 0xffffffff
            repne scasb
            mov ecx, edx
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, edx
            and ecx, 3
            rep movsb
    L0051502d:
            lea edi, [esp + 0xb8]
            or ecx, 0xffffffff
            xor eax, eax
            lea edx, [esp + 0xb8]
            repne scasb
            not ecx
            sub edi, ecx
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            shr ecx, 2
            rep movsd
            mov ecx, eax
            xor eax, eax
            and ecx, 3
            rep movsb
            lea edi, [esp + 0x38]
            or ecx, 0xffffffff
            repne scasb
            not ecx
            dec ecx
            je L005150c6
            mov edi, dword ptr [gDir_separator]
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            lea edx, [esp + 0xb8]
            mov esi, edi
            mov edi, edx
            mov edx, ecx
            or ecx, 0xffffffff
            repne scasb
            mov ecx, edx
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, edx
            lea edx, [esp + 0xb8]
            and ecx, 3
            rep movsb
            lea edi, [esp + 0x38]
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            mov esi, edi
            mov edi, edx
            mov edx, ecx
            or ecx, 0xffffffff
            repne scasb
            mov ecx, edx
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, edx
            and ecx, 3
            rep movsb
    L005150c6:
            lea eax, [esp + 0xb8]
            push eax
            call BrPixelmapLoad
            mov esi, eax
            add esp, 4
            test esi, esi
            jne L005155ad
            push eax
            push eax
            push ebx
            push 0x100
            push 3
            call BrPixelmapAllocate
            mov esi, eax
            add esp, 0x14
            test esi, esi
            mov dword ptr [esp + 0x5c], esi
            jne L00515106
            push 0x6d
            call FatalError
            add esp, 4
    L00515106:
            mov ecx, dword ptr [esp + 0x58]
            mov edi, dword ptr [esi + 8]
            mov dword ptr [esp + 0x30], 0x100
            mov edx, dword ptr [ecx + 8]
            mov dword ptr [esp + 0x20], edx
    L0051511c:
            mov eax, dword ptr [esp + 0x20]
            mov dword ptr [esp + 0x80], 0
            mov esi, dword ptr [eax]
            mov ecx, esi
            shr ecx, 0x10
            and ecx, 0xff
            mov dword ptr [esp + 0x7c], ecx
            fild qword ptr [esp + 0x7c]
            fmul dword ptr [ebp + 0x20]
            call _ftol
            mov edx, esi
            mov dword ptr [esp + 0x88], 0
            shr edx, 8
            and edx, 0xff
            mov dword ptr [esp + 0x64], eax
            mov dword ptr [esp + 0x84], edx
            fild qword ptr [esp + 0x84]
            fmul dword ptr [ebp + 0x20]
            call _ftol
            and esi, 0xff
            mov dword ptr [esp + 0x68], eax
            mov dword ptr [esp + 0x8c], esi
            xor esi, esi
            mov dword ptr [esp + 0x90], esi
            fild qword ptr [esp + 0x8c]
            fmul dword ptr [ebp + 0x20]
            call _ftol
            mov dword ptr [esp + 0x34], edi
            inc edi
            cmp ebx, 1
            mov dword ptr [esp + 0x6c], eax
            mov dword ptr [esp + 0x60], edi
            jne L005151bb
            fld qword ptr [gFloatZero_0058b280+0x8]
            jmp L005151c6
    L005151bb:
            lea eax, [ebx - 1]
            mov dword ptr [esp + 0x1c], eax
            fild dword ptr [esp + 0x1c]
    L005151c6:
            cmp ebx, esi
            mov dword ptr [esp + 0x28], esi
            jle L005155c1
            fild dword ptr [ebp + 8]
            fild dword ptr [esp + 0x64]
            fild dword ptr [ebp + 0xc]
            fild dword ptr [esp + 0x68]
            fild dword ptr [ebp + 0x10]
            fild dword ptr [esp + 0x6c]
            fxch st(2)
            fstp qword ptr [esp + 0xb0]
            fstp qword ptr [esp + 0xa0]
            fstp qword ptr [esp + 0xa8]
            jmp L00515204
    L00515200:
            mov ebx, dword ptr [esp + 0x24]
    L00515204:
            cmp ebx, 1
            jne L0051520f
            mov dword ptr [esp + 0x18], ebx
            jmp L00515217
    L0051520f:
            mov ecx, dword ptr [esp + 0x28]
            mov dword ptr [esp + 0x18], ecx
    L00515217:
            fild dword ptr [esp + 0x18]
            fdiv st(0), st(4)
            fcom qword ptr [gFloatZero_0058b280+0x10]
            fnstsw ax
            test ah, 1
            je L00515263
            fcom qword ptr [gFloatZero_0058b280+0x18]
            fnstsw ax
            test ah, 1
            je L00515242
            fmul dword ptr [ebp + 0x14]
            fmul qword ptr [gFloatZero_0058b280+0x20]
            jmp L005152b0
    L00515242:
            fld dword ptr [ebp + 0x18]
            fsub dword ptr [ebp + 0x14]
            fxch st(1)
            fsub qword ptr [gFloatZero_0058b280+0x18]
            fld dword ptr [ebp + 0x14]
            fxch st(1)
            fmulp st(2), st(0)
            fxch st(1)
            fmul qword ptr [gFloatZero_0058b280+0x28]
            fsubp st(1), st(0)
            jmp L005152b0
    L00515263:
            fcom qword ptr [gFloatZero_0058b280+0x30]
            fnstsw ax
            test ah, 1
            je L00515291
            fld dword ptr [ebp + 0x1c]
            fsub dword ptr [ebp + 0x18]
            fxch st(1)
            fsub qword ptr [gFloatZero_0058b280+0x10]
            fld dword ptr [ebp + 0x18]
            fxch st(1)
            fmulp st(2), st(0)
            fxch st(1)
            fmul qword ptr [gFloatZero_0058b280+0x28]
            fsubp st(1), st(0)
            jmp L005152b0
    L00515291:
            fld dword ptr [gFloatZero_0058b280+0x38]
            fld qword ptr [gFloatZero_0058b280+0x8]
            fsubrp st(2), st(0)
            fsub dword ptr [ebp + 0x1c]
            fmulp st(1), st(0)
            fmul qword ptr [gFloatZero_0058b280+0x20]
            fsubr qword ptr [gFloatZero_0058b280+0x8]
    L005152b0:
            fld qword ptr [gFloatZero_0058b280+0x8]
            fsub st(0), st(1)
            fld st(1)
            fld st(4)
            fxch st(1)
            fmul st(0), st(6)
            fxch st(1)
            fmul st(0), st(2)
            faddp st(1), st(0)
            call _ftol
            fld st(1)
            fld qword ptr [esp + 0xb0]
            fxch st(1)
            fmul st(0), st(4)
            fxch st(1)
            fmul st(0), st(2)
            mov ebx, eax
            faddp st(1), st(0)
            call _ftol
            fld qword ptr [esp + 0xa8]
            fxch st(2)
            fmul qword ptr [esp + 0xa0]
            fxch st(2)
            fmul st(0), st(1)
            mov dword ptr [esp + 0x98], eax
            faddp st(2), st(0)
            fxch st(1)
            call _ftol
            mov edx, dword ptr [esp + 0x58]
            mov dword ptr [esp + 0x9c], eax
            fstp st(0)
            fld qword ptr [gFloatZero_0058b280+0x40]
            mov edi, dword ptr [edx + 8]
            mov dword ptr [esp + 0x1c], 0x7f
            fstp qword ptr [esp + 0x70]
            mov dword ptr [esp + 0x18], 0
    L00515333:
            mov eax, dword ptr [edi]
            xor ecx, ecx
            mov cl, ah
            mov dword ptr [esp + 0x2c], eax
            xor edx, edx
            mov esi, ecx
            mov dl, byte ptr [esp + 0x2e]
            mov ecx, ebx
            sub ecx, edx
            mov edx, dword ptr [esp + 0x98]
            sub edx, esi
            mov esi, dword ptr [esp + 0x9c]
            and eax, 0xff
            sub esi, eax
            mov eax, ecx
            imul eax, ecx
            mov ecx, edx
            imul ecx, edx
            mov edx, esi
            add eax, ecx
            imul edx, esi
            add eax, edx
            mov dword ptr [esp + 0x2c], eax
            fild dword ptr [esp + 0x2c]
            fcom qword ptr [esp + 0x70]
            fnstsw ax
            test ah, 1
            je L00515394
            mov eax, dword ptr [esp + 0x18]
            fstp qword ptr [esp + 0x70]
            mov dword ptr [esp + 0x1c], eax
            jmp L00515396
    L00515394:
            fstp st(0)
    L00515396:
            mov eax, dword ptr [esp + 0x18]
            add edi, 4
            inc eax
            cmp eax, 0x100
            mov dword ptr [esp + 0x18], eax
            jl L00515333
            mov eax, dword ptr [esp + 0x34]
            mov cl, byte ptr [esp + 0x1c]
            mov byte ptr [eax], cl
            mov ecx, dword ptr [esp + 0x24]
            add eax, 0x100
            mov dword ptr [esp + 0x34], eax
            mov eax, dword ptr [esp + 0x28]
            inc eax
            cmp eax, ecx
            mov dword ptr [esp + 0x28], eax
            jl L00515200
            mov edi, dword ptr [esp + 0x60]
            mov ebx, ecx
            fstp st(0)
            fstp st(0)
            fstp st(0)
            fstp st(0)
    L005153df:
            mov ecx, dword ptr [esp + 0x20]
            mov eax, dword ptr [esp + 0x30]
            add ecx, 4
            dec eax
            mov dword ptr [esp + 0x20], ecx
            mov dword ptr [esp + 0x30], eax
            jne L0051511c
            mov al, byte ptr [esp + 0x17]
            mov cl, byte ptr [esp + 0x16]
            mov byte ptr [esp + 0x3b], al
            mov al, byte ptr [esp + 0x15]
            mov byte ptr [esp + 0x3c], cl
            mov cl, byte ptr [esp + 0x12]
            mov dl, byte ptr [esp + 0x14]
            mov byte ptr [esp + 0x3e], al
            mov byte ptr [esp + 0x3f], cl
            mov edi, offset gMaterial_shading_for_callback+0x4
            or ecx, 0xffffffff
            xor eax, eax
            repne scasb
            mov byte ptr [esp + 0x3a], dl
            mov dl, byte ptr [esp + 0x13]
            not ecx
            mov byte ptr [esp + 0x3d], dl
            sub edi, ecx
            lea edx, [esp + 0x38]
            mov esi, edi
            mov ebx, ecx
            mov edi, edx
            or ecx, 0xffffffff
            mov byte ptr [esp + 0x38], 0x73
            mov byte ptr [esp + 0x39], 0x74
            mov byte ptr [esp + 0x40], 0
            repne scasb
            mov ecx, ebx
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, ebx
            lea edx, [esp + 0xb8]
            and ecx, 3
            rep movsb
            mov edi, offset gApplication_path
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            mov edx, dword ptr [gDir_separator]
            shr ecx, 2
            rep movsd
            mov ecx, eax
            xor eax, eax
            and ecx, 3
            rep movsb
            mov edi, offset gSwap_depth_effect_type+0xdc
            or ecx, 0xffffffff
            repne scasb
            not ecx
            dec ecx
            je L005154ff
            mov edi, edx
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            lea ebx, [esp + 0xb8]
            mov esi, edi
            mov edi, ebx
            mov ebx, ecx
            or ecx, 0xffffffff
            repne scasb
            mov ecx, ebx
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, ebx
            lea ebx, [esp + 0xb8]
            and ecx, 3
            rep movsb
            mov edi, offset gSwap_depth_effect_type+0xdc
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            mov esi, edi
            mov edi, ebx
            mov ebx, ecx
            or ecx, 0xffffffff
            repne scasb
            mov ecx, ebx
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, ebx
            and ecx, 3
            rep movsb
    L005154ff:
            lea edi, [esp + 0xb8]
            or ecx, 0xffffffff
            xor eax, eax
            lea ebx, [esp + 0xb8]
            repne scasb
            not ecx
            sub edi, ecx
            mov eax, ecx
            mov esi, edi
            mov edi, ebx
            shr ecx, 2
            rep movsd
            mov ecx, eax
            xor eax, eax
            and ecx, 3
            rep movsb
            lea edi, [esp + 0x38]
            or ecx, 0xffffffff
            repne scasb
            not ecx
            dec ecx
            je L00515594
            mov edi, edx
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            lea ebx, [esp + 0xb8]
            mov esi, edi
            mov edx, ecx
            mov edi, ebx
            or ecx, 0xffffffff
            repne scasb
            mov ecx, edx
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, edx
            lea edx, [esp + 0xb8]
            and ecx, 3
            rep movsb
            lea edi, [esp + 0x38]
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            mov esi, edi
            mov ebx, ecx
            mov edi, edx
            or ecx, 0xffffffff
            repne scasb
            mov ecx, ebx
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, ebx
            and ecx, 3
            rep movsb
    L00515594:
            mov eax, dword ptr [esp + 0x5c]
            lea ecx, [esp + 0xb8]
            push eax
            push ecx
            call BrPixelmapSave
            mov esi, dword ptr [esp + 0x64]
            add esp, 8
    L005155ad:
            push esi
            call BrTableAdd
            add esp, 4
            mov eax, esi
            pop edi
            pop esi
            pop ebx
            mov esp, ebp
            pop ebp
            ret 0x1c
    L005155c1:
            fstp st(0)
            jmp L005153df
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

// FUNCTION: CARMA2_HW 0x005155d0
void C2_HOOK_FASTCALL PossibleService(void) {
    tU32 time;

    time = PDGetTotalTime();
    if (time - last_service > 200 && !gProgram_state.racing) {
        SoundService();
        NetService(gProgram_state.racing);
        last_service = time;
    }
}

// FUNCTION: CARMA2_HW 0x00515610
void C2_NAKED C2_HOOK_FASTCALL DRMatrix34TApplyP(br_vector3* pA, const br_vector3* pB, const br_matrix34* pC) {
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
            fld dword ptr [edx + 4]
            fld dword ptr [edx + 8]
            _emit 0xd9
        _emit 0x02
            fxch st(2)
            mov eax, dword ptr [esp + 4]
            fsub dword ptr [eax + 0x28]
            fxch st(1)
            fsub dword ptr [eax + 0x2c]
            fxch st(2)
            fsub dword ptr [eax + 0x24]
            fld dword ptr [eax + 8]
            fld dword ptr [eax + 4]
            fmul st(0), st(3)
            fxch st(1)
            fmul st(0), st(4)
            fld st(2)
            _emit 0xd8
        _emit 0x08
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            _emit 0xd9
        _emit 0x19
            fld dword ptr [eax + 0xc]
            fld dword ptr [eax + 0x10]
            fld dword ptr [eax + 0x14]
            fxch st(2)
            fmul st(0), st(3)
            fxch st(1)
            fmul st(0), st(4)
            fxch st(2)
            fmul st(0), st(5)
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fstp dword ptr [ecx + 4]
            fld dword ptr [eax + 0x18]
            fld dword ptr [eax + 0x1c]
            fld dword ptr [eax + 0x20]
            fxch st(2)
            fmul st(0), st(3)
            fxch st(1)
            fmul st(0), st(4)
            fxch st(2)
            fmul st(0), st(5)
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fstp dword ptr [ecx + 8]
            fstp st(0)
            fxch st(1)
            fstp st(0)
            fstp st(0)
            ret 4
            nop
            nop
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ volatile(
        ".intel_syntax noprefix\n\t"
        "fld dword ptr [edx + 4]\n\t"
        "fld dword ptr [edx + 8]\n\t"
        "fld dword ptr [edx]\n\t"
        "fxch st(2)\n\t"
        "fsub dword ptr [eax + 0x28]\n\t"
        "fxch st(1)\n\t"
        "fsub dword ptr [eax + 0x2c]\n\t"
        "fxch st(2)\n\t"
        "fsub dword ptr [eax + 0x24]\n\t"
        "fld dword ptr [eax + 8]\n\t"
        "fld dword ptr [eax + 4]\n\t"
        "fmul st(0), st(3)\n\t"
        "fxch st(1)\n\t"
        "fmul st(0), st(4)\n\t"
        "fld st(2)\n\t"
        "fmul dword ptr [eax]\n\t"
        "fxch st(1)\n\t"
        "faddp st(2), st(0)\n\t"
        "faddp st(1), st(0)\n\t"
        "fstp dword ptr [ecx]\n\t"
        "fld dword ptr [eax + 0xc]\n\t"
        "fld dword ptr [eax + 0x10]\n\t"
        "fld dword ptr [eax + 0x14]\n\t"
        "fxch st(2)\n\t"
        "fmul st(0), st(3)\n\t"
        "fxch st(1)\n\t"
        "fmul st(0), st(4)\n\t"
        "fxch st(2)\n\t"
        "fmul st(0), st(5)\n\t"
        "fxch st(2)\n\t"
        "faddp st(1), st(0)\n\t"
        "fxch st(1)\n\t"
        "faddp st(1), st(0)\n\t"
        "fstp dword ptr [ecx + 4]\n\t"
        "fld dword ptr [eax + 0x18]\n\t"
        "fld dword ptr [eax + 0x1c]\n\t"
        "fld dword ptr [eax + 0x20]\n\t"
        "fxch st(2)\n\t"
        "fmul st(0), st(3)\n\t"
        "fxch st(1)\n\t"
        "fmul st(0), st(4)\n\t"
        "fxch st(2)\n\t"
        "fmul st(0), st(5)\n\t"
        "fxch st(2)\n\t"
        "faddp st(1), st(0)\n\t"
        "fxch st(1)\n\t"
        "faddp st(1), st(0)\n\t"
        "fstp dword ptr [ecx + 8]\n\t"
        "fstp st(0)\n\t"
        "fxch st(1)\n\t"
        "fstp st(0)\n\t"
        "fstp st(0)\n\t"
        ".att_syntax prefix"
        :
        : "a" (pC), "c" (pA), "d" (pB)
        : "memory", "st", "st(1)", "st(2)", "st(3)", "st(4)", "st(5)");
#else
    double x = (double)pB->v[0] - pC->m[3][0];
    double y = (double)pB->v[1] - pC->m[3][1];
    double z = (double)pB->v[2] - pC->m[3][2];
    volatile double term_x;
    volatile double term_y;
    volatile double term_z;
    volatile double first_sum;
    int i;

    for (i = 0; i < 3; i++) {
        term_x = (double)pC->m[i][0] * x;
        term_y = (double)pC->m[i][1] * y;
        term_z = (double)pC->m[i][2] * z;
        first_sum = i == 0 ? term_y + term_z : term_x + term_y;
        pA->v[i] = (br_scalar)(first_sum + (i == 0 ? term_x : term_z));
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00515690
void C2_HOOK_FASTCALL DRPixelmapRectangleCopy(br_pixelmap* dst, br_int_16 dx, br_int_16 dy, br_pixelmap* src, br_int_16 sx, br_int_16 sy, br_uint_16 w, br_uint_16 h) {
    BrPixelmapRectangleCopy(dst, dx, dy, src, sx, sy, w, h);
}

// FUNCTION: CARMA2_HW 0x00515700
int C2_NAKED C2_HOOK_FASTCALL NormalSideOfPlane(br_vector3* pPoint, br_vector3* pNormal, br_scalar pD) {
#ifndef CARPOCALYPSE2_MATCHING

    return (BrVector3Dot(pNormal, pPoint) - pD) * BrVector3Dot(pNormal, pNormal) >= 0.0f;
#else
    __asm {
            sub esp, 8
            fld dword ptr [edx + 4]
            fld dword ptr [edx + 8]
            fld dword ptr [edx + 8]
            fld dword ptr [edx + 4]
            fld dword ptr [ecx + 8]
            fxch st(4)
            _emit 0xd9
        _emit 0x54
        _emit 0x24
        _emit 0x00
            fxch st(3)
            fst dword ptr [esp + 4]
            _emit 0xd9
        _emit 0x02
            fld dword ptr [ecx + 4]
            fxch st(4)
            fmulp st(6), st(0)
            _emit 0xd9
        _emit 0x02
            fxch st(3)
            fmulp st(4), st(0)
            fxch st(3)
            faddp st(5), st(0)
            fxch st(1)
            _emit 0xd8
        _emit 0x09
            fld st(2)
            fxch st(1)
            faddp st(5), st(0)
            fxch st(3)
            _emit 0xd8
        _emit 0x4c
        _emit 0x24
        _emit 0x00
            fxch st(4)
            fsub dword ptr [esp + 0xc]
            fxch st(1)
            fmul dword ptr [esp + 4]
            fxch st(3)
            fmul st(0), st(2)
            fxch st(3)
            faddp st(4), st(0)
            fxch st(2)
            faddp st(3), st(0)
            fxch st(2)
            fmulp st(1), st(0)
            fcomp dword ptr [gFloatZero_0058b280]
            fnstsw ax
            test ah, 1
            fstp st(0)
            jne L00515777
            mov eax, 1
            add esp, 8
            ret 4
    L00515777:
            xor eax, eax
            add esp, 8
            ret 4
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00515780
br_material* C2_HOOK_FASTCALL DRMaterialClone(br_material* pMaterial, int pSet_identifier) {
    br_material* the_material;
    char s[256];
    int version;

    // GLOBAL: CARMA2_HW 0x006abefc
    static int gVersion_suffix;

    the_material = BrMaterialAllocate(NULL);
    the_material->flags = pMaterial->flags;
    the_material->ka = pMaterial->ka;
    the_material->kd = pMaterial->kd;
    the_material->ks = pMaterial->ks;
    the_material->power = pMaterial->power;
    the_material->colour = pMaterial->colour;
    the_material->index_base = pMaterial->index_base;
    the_material->index_range = pMaterial->index_range;
    the_material->index_shade = pMaterial->index_shade;
    the_material->index_blend = pMaterial->index_blend;
    the_material->colour_map = pMaterial->colour_map;
    memcpy(&the_material->map_transform, &pMaterial->map_transform, sizeof(the_material->map_transform));
    if (pSet_identifier) {
        version = gVersion_suffix++;
        sprintf(s, "%s(%d)", pMaterial->identifier, version);
        the_material->identifier = BrResAllocate(the_material, strlen(s) + 1, BR_MEMORY_STRING);
        strcpy(the_material->identifier, s);
    }
    BrMaterialAdd(the_material);
    return the_material;
}

// FUNCTION: CARMA2_HW 0x00515870
int C2_HOOK_FASTCALL DRStricmp(const char* p1, const char* p2) {
    int val;

    val = tolower(*p1);
    val -= tolower(*p2);
    while (val == 0) {
        if (*p1++ == '\0') {
            break;
        }
        if (*p2++ == '\0') {
            break;
        }
        val = tolower(*p1);
        val -= tolower(*p2);
    }
    return val;
}

// FUNCTION: CARMA2_HW 0x005158d0
void C2_HOOK_FASTCALL DRstrlwr(char* s) {
    int l = (int)strlen(s);
    int i;

    for (i = 0; i < l; i++) {
        s[i] = toupper(s[i]);
    }
}

// FUNCTION: CARMA2_HW 0x00515950
int C2_HOOK_FASTCALL PDCheckDriveExists(const char* pThe_path) {
    return PDCheckDriveExists2(pThe_path, NULL, 0);
}

// FUNCTION: CARMA2_HW 0x00515960
C2_NAKED void* C2_HOOK_FASTCALL EncodePixelmapRunLength(br_pixelmap* pPixelmap) {
#ifndef CARPOCALYPSE2_MATCHING
    tU8 row_buffer[800];
    tU8* source_row;
    tU8* scratch;
    tU8* result;
    tU8* row_cursor;
    tU8* control_byte;
    tU8 run_value;
    tU8 zero_run;
    tU8 packet_count;
    br_uint_32 output_size;
    br_uint_32 row_size;
    int row_index;
    br_uint_32 pixel_index;
    br_uint_32 run_length;
    br_uint_32 row_width;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_pixelmap, pixels, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_pixelmap, row_bytes, 0x28);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_pixelmap, width, 0x34);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_pixelmap, height, 0x36);

    scratch = BrMemAllocate(pPixelmap->row_bytes * pPixelmap->height, kMem_misc);
    *(br_uint_16*)scratch = pPixelmap->height;
    output_size = 2;
    source_row = pPixelmap->pixels;
    for (row_index = 0; row_index < pPixelmap->height; row_index++) {
        source_row = (tU8*)pPixelmap->pixels + row_index * pPixelmap->row_bytes;
        row_width = pPixelmap->width;
        row_cursor = row_buffer + 1;
        packet_count = 0;
        row_size = 2;
        pixel_index = 0;
        run_value = *source_row;
        zero_run = run_value == 0;
        while (1) {
            control_byte = row_cursor++;
            run_length = 0;
            while (run_length < 0x7f
                    && pixel_index != row_width
                    && ((source_row[pixel_index] == 0) == zero_run)) {
                run_value = source_row[pixel_index];
                if (!zero_run) {
                    *row_cursor++ = run_value;
                    row_size++;
                }
                run_length++;
                pixel_index++;
            }
            if (zero_run) {
                *control_byte = run_length;
            } else {
                *control_byte = (tU8)(-(int)run_length);
            }
            packet_count++;
            if (pixel_index == row_width) {
                break;
            }
            row_size++;
            run_value = source_row[pixel_index];
            zero_run = run_value == 0;
        }
        row_buffer[0] = packet_count;
        memcpy(scratch + output_size, row_buffer, row_size);
        output_size += row_size;
    }
    result = BrMemAllocate(output_size, kMem_misc);
    memcpy(result, scratch, output_size);
    BrMemFree(scratch);
    return result;
#else
    __asm {
            sub esp, 0x340
            push ebx
            push ebp
            mov ebp, ecx
            push esi
            xor ecx, ecx
            push edi
            movsx eax, word ptr [ebp + 0x28]
            mov cx, word ptr [ebp + 0x36]
            push 0xfd
            imul eax, ecx
            push eax
            call BrMemAllocate
            mov dx, word ptr [ebp + 0x36]
            mov esi, eax
            xor eax, eax
            add esp, 8
            mov edi, 2
            mov word ptr [esi], dx
            cmp word ptr [ebp + 0x36], ax
            mov dword ptr [esp + 0x24], esi
            mov dword ptr [esp + 0x1c], edi
            mov dword ptr [esp + 0x28], eax
            jbe L00515ac2
            mov bl, byte ptr [esp + 0x13]
    L005159b1:
            movsx ecx, word ptr [ebp + 0x28]
            imul eax, ecx
            mov edx, dword ptr [ebp + 8]
            xor ecx, ecx
            add eax, edx
            mov dword ptr [esp + 0x14], 2
            mov dword ptr [esp + 0x2c], eax
            cmp byte ptr [eax], 0
            sete cl
            mov eax, ecx
            xor ecx, ecx
            xor edi, edi
            xor esi, esi
            mov si, word ptr [ebp + 0x34]
            mov dword ptr [esp + 0x20], ecx
            lea edx, [esp + 0x32]
    L005159e5:
            mov dword ptr [esp + 0x18], edx
    L005159e9:
            cmp ecx, 0x7e
            jg L00515a28
            cmp edi, esi
            je L00515a28
            mov edx, dword ptr [esp + 0x2c]
            mov bl, byte ptr [edi + edx]
            xor edx, edx
            test bl, bl
            sete dl
            cmp edx, eax
            mov byte ptr [esp + 0x13], bl
            jne L00515a28
            test eax, eax
            jne L00515a24
            mov edx, dword ptr [esp + 0x18]
            mov byte ptr [edx], bl
            mov ebx, dword ptr [esp + 0x14]
            inc ebx
            inc edx
            mov dword ptr [esp + 0x14], ebx
            mov bl, byte ptr [esp + 0x13]
            mov dword ptr [esp + 0x18], edx
    L00515a24:
            inc ecx
            inc edi
            jmp L005159e9
    L00515a28:
            test eax, eax
            je L00515a35
            mov eax, dword ptr [esp + 0x18]
            mov byte ptr [eax - 1], cl
            jmp L00515a49
    L00515a35:
            mov dl, cl
            lea eax, [esp + 0x30]
            not dl
            inc dl
            sub eax, ecx
            mov ecx, dword ptr [esp + 0x14]
            mov byte ptr [eax + ecx - 1], dl
    L00515a49:
            mov edx, dword ptr [esp + 0x20]
            xor eax, eax
            test bl, bl
            sete al
            xor ecx, ecx
            inc edx
            cmp edi, esi
            mov dword ptr [esp + 0x20], edx
            je L00515a72
            mov edx, dword ptr [esp + 0x14]
            inc edx
            mov dword ptr [esp + 0x14], edx
            mov edx, dword ptr [esp + 0x18]
            inc edx
            jmp L005159e5
    L00515a72:
            mov dl, byte ptr [esp + 0x20]
            mov eax, dword ptr [esp + 0x1c]
            mov ecx, dword ptr [esp + 0x14]
            mov byte ptr [esp + 0x30], dl
            mov edx, dword ptr [esp + 0x24]
            lea esi, [esp + 0x30]
            lea edi, [edx + eax]
            mov edx, ecx
            shr ecx, 2
            rep movsd
            mov ecx, edx
            and ecx, 3
            rep movsb
            mov edi, edx
            xor ecx, ecx
            mov cx, word ptr [ebp + 0x36]
            add eax, edi
            mov dword ptr [esp + 0x1c], eax
            mov eax, dword ptr [esp + 0x28]
            inc eax
            cmp eax, ecx
            mov dword ptr [esp + 0x28], eax
            jl L005159b1
            mov edi, dword ptr [esp + 0x1c]
            mov esi, dword ptr [esp + 0x24]
    L00515ac2:
            push 0xfd
            push edi
            call BrMemAllocate
            mov ecx, edi
            mov ebx, eax
            mov edx, ecx
            mov edi, ebx
            shr ecx, 2
            rep movsd
            mov eax, dword ptr [esp + 0x2c]
            mov ecx, edx
            add esp, 8
            and ecx, 3
            rep movsb
            push eax
            call BrMemFree
            add esp, 4
            mov eax, ebx
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x340
            ret
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00515b00
br_actor* C2_HOOK_FASTCALL CloneActor(br_actor* pActor) {
    br_actor *clone;
    br_actor *child;

    clone = BrActorAllocate(pActor->type, pActor->type_data);
    clone->model = pActor->model;
    clone->material = pActor->material;
    if (pActor->identifier != NULL) {
        if (clone->identifier != NULL) {
            BrResFree(clone->identifier);
        }
        clone->identifier = BrResStrDup(clone, pActor->identifier);;
    }
    clone->t = pActor->t;
    for (child = pActor->children; child != NULL; child = child->next) {
        BrActorAdd(clone, CloneActor(child));
    }
    return clone;
}

// FUNCTION: CARMA2_HW 0x00515b80
void C2_NAKED C2_HOOK_FASTCALL CalcActorGlobalPos(br_vector3* pResult, br_actor* pActor) {
#ifndef CARPOCALYPSE2_MATCHING
    br_vector3 tv;

    BrVector3Set(pResult, 0.0f, 0.0f, 0.0f);
    for (;pActor != NULL && pActor != gNon_track_actor; pActor = pActor->parent) {
        if (pActor->t.t.mat.m[0][0] == 1.0f) {
            BrVector3Accumulate(pResult, &pActor->t.t.translate.t);
        } else {
            BrMatrix34ApplyP(&tv, pResult, &pActor->t.t.mat);
            BrVector3Accumulate(pResult, &tv);
        }
    }
#else
    __asm {
            sub esp, 0xc
            push esi
            push edi
            mov edi, edx
            mov esi, ecx
            test edi, edi
            mov dword ptr [esi], 0
            mov dword ptr [esi + 4], 0
            mov dword ptr [esi + 8], 0
            je L00515c0b
    L00515ba1:
            cmp edi, dword ptr [gNon_track_actor]
            je L00515c0b
            mov ecx, dword ptr [edi + 0x2c]
            lea eax, [edi + 0x2c]
            cmp ecx, 0x3f800000
            jne L00515bcf
            fld dword ptr [edi + 0x50]
            fadd dword ptr [esi]
            fstp dword ptr [esi]
            fld dword ptr [edi + 0x54]
            fadd dword ptr [esi + 4]
            fstp dword ptr [esi + 4]
            fld dword ptr [edi + 0x58]
            fadd dword ptr [esi + 8]
            jmp L00515c01
    L00515bcf:
            push eax
            lea eax, [esp + 0xc]
            push esi
            push eax
            call BrMatrix34ApplyP
            fld dword ptr [esi]
            fld dword ptr [esp + 0x18]
            fld dword ptr [esp + 0x1c]
            fxch st(2)
            fadd dword ptr [esp + 0x14]
            fxch st(1)
            fadd dword ptr [esi + 4]
            fxch st(2)
            fadd dword ptr [esi + 8]
            fxch st(1)
            fstp dword ptr [esi]
            fxch st(1)
            fstp dword ptr [esi + 4]
            add esp, 0xc
    L00515c01:
            fstp dword ptr [esi + 8]
            mov edi, dword ptr [edi + 0xc]
            test edi, edi
            jne L00515ba1
    L00515c0b:
            pop edi
            pop esi
            add esp, 0xc
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
            nop
            nop
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00515c20
float C2_HOOK_STDCALL frac(float pN) {
    return pN - (float)(int)pN;
}

// FUNCTION: CARMA2_HW 0x00515d90
intptr_t C2_HOOK_CDECL FindMaterialCB(br_actor* pActor, void* data) {
    const char* name = data;
    br_model* model = pActor->model;
    br_face *face;
    int face_i;

    if (model != NULL) {

        face = model->faces;
        for (face_i = 0; face_i < model->nfaces; face_i++, face++) {

            if (face->material != NULL
                    && face->material->identifier != NULL
                    && strcmp(face->material->identifier, name) == 0) {
                return (intptr_t)face->material;
            }
        }
    }
    return (intptr_t)NULL;
}

// FUNCTION: CARMA2_HW 0x00515c40
br_material* C2_HOOK_FASTCALL FindMaterial(const char* pName, br_actor* pActor, int pRecursive) {
    if (pRecursive) {
        return (br_material*)DRActorEnumRecurse(pActor, FindMaterialCB, (void*)pName);
    } else {
        return (br_material*)FindMaterialCB(pActor, (void*)pName);
    }
}

void C2_HOOK_FASTCALL BlendifyMaterialTablishly(br_material* pMaterial, int pPercent) {
    char* s;

    if (pPercent != 0 && pPercent != 100) {
        switch (pPercent) {
        case 25:
            s = "BLEND25.TAB";
            break;
        case 50:
            s = "BLEND50.TAB";
            break;
        case 75:
            s = "BLEND75.TAB";
            break;
        default:
            return;
        }
        pMaterial->index_blend = BrTableFind(s);
        if (pMaterial->index_blend == NULL) {
            pMaterial->index_blend = LoadSingleShadeTable(&gTrack_storage_space, s);
        }
    } else {
        pMaterial->index_blend = NULL;
    }
}

void C2_HOOK_FASTCALL BlendifyMaterialPrimitively(br_material* pMaterial, int pPercent) {

    // GLOBAL: CARMA2_HW 0x00661688
    static br_token_value alpha25[] = {
        { BRT_BLEND_B, { /*.b = */ 1 } },
        { BRT_OPACITY_X, { /*.x = */ BR_FIXED_INT(0x40) } },
        { 0 },
    };
    // GLOBAL: CARMA2_HW 0x006616a0
    static br_token_value alpha50[] = {
        { BRT_BLEND_B, { /*.b = */ 1 } },
        { BRT_OPACITY_X, { /*.x = */ BR_FIXED_INT(0x80) } },
        { 0 },
    };
    // GLOBAL: CARMA2_HW 0x006616b8
    static br_token_value alpha75[] = {
        { BRT_BLEND_B, { /*.b = */ 1 } },
        { BRT_OPACITY_X, { /*.x = */ BR_FIXED_INT(0xc0) } },
        { 0 },
    };

    switch (pPercent) {
    case 25:
        pMaterial->extra_prim = alpha25;
        break;
    case 50:
        pMaterial->extra_prim = alpha50;
        break;
    case 75:
        pMaterial->extra_prim = alpha75;
        break;
    case 0:
    case 100:
        pMaterial->extra_prim = NULL;
        break;
    }
}

// FUNCTION: CARMA2_HW 0x00515e70
void C2_HOOK_FASTCALL BlendifyMaterial(br_material* pMaterial, int pPercent) {

    if (gScreen->type == BR_PMT_INDEX_8) {
        BlendifyMaterialTablishly(pMaterial, pPercent);
    } else {
        BlendifyMaterialPrimitively(pMaterial, pPercent);
    }
}

// FUNCTION: CARMA2_HW 0x00515fa0
void C2_NAKED C2_HOOK_FASTCALL DRModelUpdateAndKevificateMaterials(br_model* pModel, br_uint_16 pFlags) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;
    v11group* v11g;

    C2_HOOK_BUG_ON(sizeof(v11group) != 0x24);

    if (pModel->nvertices != 0 && pModel->nfaces != 0) {

        BrModelUpdate(pModel, pFlags);
        for (i = 0; i < V11MODEL(pModel)->ngroups; i++) {

            v11g = &V11MODEL(pModel)->groups[i];
            *v11g->face_colours.materials = pModel->faces[*v11g->face_user].material;
        }
    }
#else
    __asm {
            push ecx
            push esi
            mov esi, ecx
            mov word ptr [esp + 4], dx
            cmp word ptr [esi + 0x10], 0
            je L00516003
            cmp word ptr [esi + 0x12], 0
            je L00516003
            mov eax, dword ptr [esp + 4]
            push eax
            push esi
            call BrModelUpdate
            mov eax, dword ptr [esi + 0x4c]
            xor edx, edx
            add esp, 8
            cmp word ptr [eax + 8], dx
            jbe L00516003
            push edi
            push ebx
            xor edi, edi
    L00515fd4:
            mov ecx, dword ptr [eax + 0x18]
            xor eax, eax
            add ecx, edi
            inc edx
            add edi, 0x24
            mov ebx, dword ptr [ecx + 0xc]
            mov ecx, dword ptr [ecx + 8]
            mov ax, word ptr [ebx]
            mov ebx, dword ptr [esi + 0xc]
            lea eax, [eax + eax*4]
            mov eax, dword ptr [ebx + eax*8 + 8]
            mov dword ptr [ecx], eax
            mov eax, dword ptr [esi + 0x4c]
            xor ecx, ecx
            mov cx, word ptr [eax + 8]
            cmp edx, ecx
            jl L00515fd4
            pop ebx
            pop edi
    L00516003:
            pop esi
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
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00516010
int C2_NAKED C2_HOOK_FASTCALL DRModelUpdateDeluxTurbo(br_actor* pActor, br_model* pModel, br_uint_16 pFlags) {
#ifndef CARPOCALYPSE2_MATCHING

    if (pModel->nfaces == 0 || pModel->nvertices == 0) {

        pActor->type = BR_ACTOR_NONE;
        pActor->model = NULL;
        return 0;
    } else {

        DRModelUpdateAndKevificateMaterials(pModel,pFlags);
        return 1;
    }
#else
    __asm {
            push ebx
            push esi
            mov esi, edx
            xor ebx, ebx
            push edi
            cmp word ptr [esi + 0x12], bx
            je L00516076
            cmp word ptr [esi + 0x10], bx
            je L00516076
            mov eax, dword ptr [esp + 0x10]
            push eax
            push esi
            call BrModelUpdate
            mov eax, dword ptr [esi + 0x4c]
            add esp, 8
            xor edx, edx
            cmp word ptr [eax + 8], bx
            jbe L0051606b
            xor edi, edi
    L0051603e:
            mov ecx, dword ptr [eax + 0x18]
            xor eax, eax
            add ecx, edi
            inc edx
            add edi, 0x24
            mov ebx, dword ptr [ecx + 0xc]
            mov ecx, dword ptr [ecx + 8]
            mov ax, word ptr [ebx]
            mov ebx, dword ptr [esi + 0xc]
            lea eax, [eax + eax*4]
            mov eax, dword ptr [ebx + eax*8 + 8]
            mov dword ptr [ecx], eax
            mov eax, dword ptr [esi + 0x4c]
            xor ecx, ecx
            mov cx, word ptr [eax + 8]
            cmp edx, ecx
            jl L0051603e
    L0051606b:
            mov eax, 1
            pop edi
            pop esi
            pop ebx
            ret 4
    L00516076:
            pop edi
            mov byte ptr [ecx + 0x12], bl
            mov dword ptr [ecx + 0x18], ebx
            pop esi
            xor eax, eax
            pop ebx
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
            nop
            nop
            nop
    }
#endif
}

// DistanceFromFace

// FUNCTION: CARMA2_HW 0x00516090
br_scalar C2_NAKED C2_HOOK_FASTCALL DistanceFromFace(const br_vector3* pP, const tFace_ref* pFace) {
#ifndef CARPOCALYPSE2_MATCHING

    return pFace->normal.v[2] * pP->v[2] + pFace->normal.v[1] * pP->v[1] + pFace->normal.v[0] * pP->v[0] - pFace->d;
#else
    __asm {
            fld dword ptr [edx + 0x3c]
            fld dword ptr [edx + 0x38]
            fld dword ptr [ecx + 8]
            fld dword ptr [ecx + 4]
            fld dword ptr [edx + 0x34]
            fxch st(4)
            fmulp st(2), st(0)
            fxch st(2)
            fmulp st(2), st(0)
            fxch st(2)
            fmul dword ptr [ecx]
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fsub dword ptr [edx + 0x44]
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
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x005160c0
void C2_HOOK_FASTCALL DRBoundsCopy(br_bounds3* pDest, const br_bounds3* pSrc) {
    pDest->min.v[0] = pSrc->min.v[0];
    pDest->min.v[1] = pSrc->min.v[1];
    pDest->min.v[2] = pSrc->min.v[2];
    pDest->max.v[0] = pSrc->max.v[0];
    pDest->max.v[1] = pSrc->max.v[1];
    pDest->max.v[2] = pSrc->max.v[2];
}

// FUNCTION: CARMA2_HW 0x005160f0
int C2_HOOK_FASTCALL TestForNan(const float* f) {
    if (f != NULL) {
        return ((~*(tU32*)f) & 0x7f800000) == 0;
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x00516160
int C2_HOOK_FASTCALL DRVector3TestForNan(const br_vector3* pV) {
    if (pV != NULL) {
        return TestForNan(&pV->v[0]) || TestForNan(&pV->v[1]) || TestForNan(&pV->v[2]);
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x00516240
void C2_NAKED C2_HOOK_FASTCALL DRScaleModel(br_model* pModel, float pScale) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;
    br_vertex *vertex;

    if (pModel->nvertices != 0) {
        vertex = pModel->vertices;
        for (i = 0; i < pModel->nvertices; i++, vertex++) {

            BrVector3Scale(&vertex->p, &vertex->p, pScale);
        }
        BrModelUpdate(pModel, BR_MODU_ALL);
    }
#else
    __asm {
            push esi
            mov si, word ptr [ecx + 0x10]
            test si, si
            je L00516296
            mov eax, dword ptr [ecx + 8]
            xor edx, edx
            test si, si
            jbe L00516288
    L00516254:
            fld dword ptr [eax]
            fld dword ptr [eax + 4]
            fld dword ptr [eax + 8]
            fxch st(2)
            fmul dword ptr [esp + 8]
            fxch st(1)
            fmul dword ptr [esp + 8]
            fxch st(2)
            fmul dword ptr [esp + 8]
            fxch st(1)
            fstp dword ptr [eax]
            fxch st(1)
            fstp dword ptr [eax + 4]
            fstp dword ptr [eax + 8]
            xor esi, esi
            inc edx
            mov si, word ptr [ecx + 0x10]
            add eax, 0x28
            cmp edx, esi
            jl L00516254
    L00516288:
            push 0x7fff
            push ecx
            call BrModelUpdate
            add esp, 8
    L00516296:
            pop esi
            ret 4
            nop
            nop
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00516350
int C2_HOOK_FASTCALL DRVector3NonZero(const br_vector3* pV) {
    if (pV->v[0] == gFloatZero_0058b280 && pV->v[1] == gFloatZero_0058b280 && pV->v[2] == gFloatZero_0058b280) {
        return 0;
    } else {
        return 1;
    }
}

// FUNCTION: CARMA2_HW 0x00516390
void C2_NAKED C2_HOOK_FASTCALL DRVector3Diminish(br_vector3* pV1, const br_vector3* pV2) {
#ifndef CARPOCALYPSE2_MATCHING
    br_vector3 v1 = *pV1;

    BrVector3Sub(pV1, pV1, pV2);
    if (pV1->v[0] * v1.v[0] <= gFloatZero_0058b280) {
        pV1->v[0] = 0.0f;
    }
    if (v1.v[1] * pV1->v[1] <= gFloatZero_0058b280) {
        pV1->v[1] = 0.0f;
    }
    if (v1.v[2] * pV1->v[2] <= gFloatZero_0058b280) {
        pV1->v[2] = 0.0f;
    }
#else
    __asm {
            sub esp, 0xc
            fld dword ptr [ecx]
            fsub dword ptr [edx]
            mov eax, ecx
            push esi
            mov esi, dword ptr [eax]
            mov dword ptr [esp + 4], esi
            mov esi, dword ptr [eax + 4]
            mov dword ptr [esp + 8], esi
            mov eax, dword ptr [eax + 8]
            fstp dword ptr [ecx]
            fld dword ptr [ecx + 4]
            fsub dword ptr [edx + 4]
            mov dword ptr [esp + 0xc], eax
            fstp dword ptr [ecx + 4]
            fld dword ptr [ecx + 8]
            fsub dword ptr [edx + 8]
            xor edx, edx
            fstp dword ptr [ecx + 8]
            fld dword ptr [esp + 4]
            fmul dword ptr [ecx]
            fcomp dword ptr [gFloatZero_0058b280]
            fnstsw ax
            test ah, 0x41
            je L005163d9
            mov dword ptr [ecx], edx
    L005163d9:
            fld dword ptr [esp + 8]
            fmul dword ptr [ecx + 4]
            fcomp dword ptr [gFloatZero_0058b280]
            fnstsw ax
            test ah, 0x41
            je L005163f0
            mov dword ptr [ecx + 4], edx
    L005163f0:
            fld dword ptr [esp + 0xc]
            fmul dword ptr [ecx + 8]
            fcomp dword ptr [gFloatZero_0058b280]
            fnstsw ax
            test ah, 0x41
            je L00516407
            mov dword ptr [ecx + 8], edx
    L00516407:
            pop esi
            add esp, 0xc
            ret
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00516410
tU16 C2_HOOK_FASTCALL DRScalarToU16(float pValue, float pMin, float pMax) {
    float scaled;

    if (pValue < pMin) {
        pValue = pMin;
    } else if (pValue > pMax) {
        pValue = pMax;
    }
    scaled = (pValue - pMin) * 65535.0f / (pMax - pMin);
    return (tU16)(scaled + 0.5f);
}

// FUNCTION: CARMA2_HW 0x00516460
br_scalar C2_HOOK_FASTCALL DRU16ToScalar(tU16 pValue, float pMin, float pMax) {

    return ((float)pValue * (pMax - pMin) / 65535.0f) + pMin;
}

// FUNCTION: CARMA2_HW 0x005162a0
float C2_NAKED C2_HOOK_FASTCALL DistanceFromFaceND(const br_vector3* pP, const br_vector3* pN, br_scalar pF) {
#ifndef CARPOCALYPSE2_MATCHING

    return BrVector3Dot(pP, pN) - pF;
#else
    __asm {
            fld dword ptr [edx + 4]
            fld dword ptr [edx + 8]
            fld dword ptr [ecx + 4]
            fld dword ptr [ecx + 8]
            fld dword ptr [edx]
            fxch st(4)
            fmulp st(2), st(0)
            fxch st(2)
            fmulp st(2), st(0)
            fxch st(2)
            fmul dword ptr [ecx]
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fsub dword ptr [esp + 4]
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

// FUNCTION: CARMA2_HW 0x005162f0
void C2_HOOK_FASTCALL DRVector4Set(br_vector4* pDst, float pX, float pY, float pZ, float pU) {
    pDst->v[0] = pX;
    pDst->v[1] = pY;
    pDst->v[2] = pZ;
    pDst->v[3] = pU;
}

// FUNCTION: CARMA2_HW 0x00516310
void C2_HOOK_FASTCALL DRPlaneEquation(br_vector4* pDst, const br_vector3* pN, const br_vector3* pP) {
    pDst->v[0] = pN->v[0];
    pDst->v[1] = pN->v[1];
    pDst->v[2] = pN->v[2];
    /* FIXME: this should be negative... */
    pDst->v[3] = BrVector3Dot(pP, pN);
}

// FUNCTION: CARMA2_HW 0x00516490
void C2_HOOK_FASTCALL CompressVector3(tCompressed_vector3* pDest, const br_vector3* pSrc, float pMin, float pMax) {

    pDest->v[0] = DRScalarToU16(pSrc->v[0], pMin, pMax);
    pDest->v[1] = DRScalarToU16(pSrc->v[1], pMin, pMax);
    pDest->v[2] = DRScalarToU16(pSrc->v[2], pMin, pMax);
}

// FUNCTION: CARMA2_HW 0x00516570
void C2_HOOK_FASTCALL ExpandVector3(br_vector3* pDest, const tCompressed_vector3 *pSrc, float pMin, float pMax) {

    pDest->v[0] = DRU16ToScalar(pSrc->v[0], pMin, pMax);
    pDest->v[1] = DRU16ToScalar(pSrc->v[1], pMin, pMax);
    pDest->v[2] = DRU16ToScalar(pSrc->v[2], pMin, pMax);
}

// FUNCTION: CARMA2_HW 0x005165e0
void C2_HOOK_FASTCALL CompressMatrix34(tCompressed_matrix3* pCompressed_matrix3, int* pInactive, const br_matrix34* pMatrix) {
    br_vector3 pos;

    if (pMatrix->m[3][0] >= 500.0f) {
        *pInactive = 1;
        BrVector3Set(&pos,
            pMatrix->m[3][0] - 1000.0f,
            pMatrix->m[3][1] - 1000.0f,
            pMatrix->m[3][2] - 1000.0f);
    } else {
        *pInactive = 0;
        BrVector3Copy(&pos, (const br_vector3*)pMatrix->m[3]);
    }
    CompressVector3(&pCompressed_matrix3->m0, (const br_vector3*)pMatrix->m[0], -1.1f, 1.1f);
    CompressVector3(&pCompressed_matrix3->m1, (const br_vector3*)pMatrix->m[1], -1.1f, 1.1f);
    CompressVector3(&pCompressed_matrix3->p, &pos, -300.0f, 300.0f);
}

// FUNCTION: CARMA2_HW 0x00516910
void C2_NAKED C2_HOOK_FASTCALL ExpandMatrix34(br_matrix34* pMatrix, const tCompressed_matrix3* pCompressed, int pInactive) {
#ifndef CARPOCALYPSE2_MATCHING

    ExpandVector3((br_vector3*)pMatrix->m[0], &pCompressed->m0, -1.1f, 1.1f);
    BrVector3Normalise((br_vector3*)pMatrix->m[0], (br_vector3*)pMatrix->m[0]);
    ExpandVector3((br_vector3*)pMatrix->m[1], &pCompressed->m1, -1.1f, 1.1f);
    BrVector3Normalise((br_vector3*)pMatrix->m[1], (br_vector3*)pMatrix->m[1]);
    BrVector3Cross((br_vector3*)pMatrix->m[2], (br_vector3*)pMatrix->m[0], (br_vector3*)pMatrix->m[1]);
    BrVector3Normalise((br_vector3*)pMatrix->m[2], (br_vector3*)pMatrix->m[2]);
    ExpandVector3((br_vector3*)pMatrix->m[3], &pCompressed->p, -300.0f, 300.0f);
    if (pInactive) {
        BrVector3Set((br_vector3*)pMatrix->m[3],
            pMatrix->m[3][0] + 1000.0f,
            pMatrix->m[3][1] + 1000.0f,
            pMatrix->m[3][2] + 1000.0f);
    }
#else
    __asm {
            sub esp, 8
            xor eax, eax
            push ebx
            mov ax, word ptr [edx]
            push esi
            mov dword ptr [esp + 0xc], eax
            xor eax, eax
            fild dword ptr [esp + 0xc]
            xor esi, esi
            push edi
            mov edi, 0x3f800000
            fmul dword ptr [gFloatZero_0058b280+0x74]
            fsub dword ptr [gFloatZero_0058b280+0x60]
            _emit 0xd9
        _emit 0x19
            mov ax, word ptr [edx + 2]
            _emit 0xd9
        _emit 0x01
            mov dword ptr [esp + 0x10], eax
            fild dword ptr [esp + 0x10]
            fmul dword ptr [gFloatZero_0058b280+0x74]
            fld st(1)
            fxch st(1)
            fsub dword ptr [gFloatZero_0058b280+0x60]
            fxch st(1)
            fmul st(0), st(2)
            fxch st(1)
            fstp dword ptr [ecx + 4]
            mov si, word ptr [edx + 4]
            fld dword ptr [ecx + 4]
            mov dword ptr [esp + 0x10], esi
            xor esi, esi
            fild dword ptr [esp + 0x10]
            fmul dword ptr [gFloatZero_0058b280+0x74]
            fxch st(1)
            fst dword ptr [esp + 0xc]
            fxch st(1)
            fsub dword ptr [gFloatZero_0058b280+0x60]
            fxch st(1)
            fmul dword ptr [esp + 0xc]
            fxch st(1)
            fst dword ptr [esp + 0x10]
            fst dword ptr [esp + 0xc]
            fmul dword ptr [esp + 0xc]
            fld dword ptr [esp + 0x10]
            fxch st(1)
            faddp st(2), st(0)
            fstp dword ptr [ecx + 8]
            fxch st(1)
            faddp st(1), st(0)
            fsqrt
            fstp st(1)
            fcom dword ptr [gFloatZero_0058b280+0x78]
            fnstsw ax
            test ah, 0x41
            jne L005169e2
            fdivr qword ptr [gFloatZero_0058b280+0x8]
            fld dword ptr [ecx + 8]
            fld dword ptr [ecx + 4]
            fld st(2)
            _emit 0xd8
        _emit 0x09
            fxch st(1)
            fmul st(0), st(3)
            fxch st(2)
            fmul st(0), st(3)
            fxch st(1)
            _emit 0xd9
        _emit 0x19
            fxch st(1)
            fstp dword ptr [ecx + 4]
            fstp dword ptr [ecx + 8]
            fstp st(0)
            jmp L005169ec
    L005169e2:
            fstp st(0)
            mov dword ptr [ecx], edi
            mov dword ptr [ecx + 4], esi
            mov dword ptr [ecx + 8], esi
    L005169ec:
            xor eax, eax
            xor ebx, ebx
            mov ax, word ptr [edx + 6]
            mov dword ptr [esp + 0x10], eax
            xor eax, eax
            fild dword ptr [esp + 0x10]
            fmul dword ptr [gFloatZero_0058b280+0x74]
            fsub dword ptr [gFloatZero_0058b280+0x60]
            fstp dword ptr [ecx + 0xc]
            mov ax, word ptr [edx + 8]
            mov dword ptr [esp + 0x10], eax
            fild dword ptr [esp + 0x10]
            fmul dword ptr [gFloatZero_0058b280+0x74]
            fsub dword ptr [gFloatZero_0058b280+0x60]
            fstp dword ptr [ecx + 0x10]
            mov bx, word ptr [edx + 0xa]
            mov dword ptr [esp + 0x10], ebx
            fild dword ptr [esp + 0x10]
            fmul dword ptr [gFloatZero_0058b280+0x74]
            fsub dword ptr [gFloatZero_0058b280+0x60]
            fstp dword ptr [ecx + 0x14]
            fld dword ptr [ecx + 0xc]
            fld dword ptr [ecx + 0x10]
            fld dword ptr [ecx + 0x14]
            fxch st(2)
            fst dword ptr [esp + 0x10]
            fld st(1)
            fxch st(1)
            fmul dword ptr [esp + 0x10]
            fxch st(1)
            fmul st(0), st(2)
            fxch st(3)
            fst dword ptr [esp + 0x10]
            fmul dword ptr [esp + 0x10]
            fxch st(3)
            faddp st(1), st(0)
            fxch st(2)
            faddp st(2), st(0)
            fxch st(1)
            fsqrt
            fstp st(1)
            fcom dword ptr [gFloatZero_0058b280+0x78]
            fnstsw ax
            test ah, 0x41
            jne L00516ab3
            fdivr qword ptr [gFloatZero_0058b280+0x8]
            fld dword ptr [ecx + 0xc]
            fld dword ptr [ecx + 0x10]
            fld dword ptr [ecx + 0x14]
            fxch st(2)
            fxch st(1)
            fxch st(1)
            fmul st(0), st(3)
            fxch st(1)
            fmul st(0), st(3)
            fxch st(2)
            fmul st(0), st(3)
            fxch st(1)
            fstp dword ptr [ecx + 0xc]
            fxch st(1)
            fstp dword ptr [ecx + 0x10]
            fstp dword ptr [ecx + 0x14]
            fstp st(0)
            jmp L00516abe
    L00516ab3:
            fstp st(0)
            mov dword ptr [ecx + 0xc], edi
            mov dword ptr [ecx + 0x10], esi
            mov dword ptr [ecx + 0x14], esi
    L00516abe:
            fld dword ptr [ecx + 0x14]
            fld dword ptr [ecx + 4]
            fld dword ptr [ecx + 0x10]
            fld dword ptr [ecx + 8]
            fld dword ptr [ecx + 0xc]
            fxch st(3)
            fmulp st(4), st(0)
            fld dword ptr [ecx + 0x14]
            fxch st(1)
            fmulp st(2), st(0)
            fxch st(1)
            fsubp st(3), st(0)
            fxch st(1)
            fmul dword ptr [ecx + 8]
            fxch st(2)
            fstp dword ptr [ecx + 0x18]
            fld dword ptr [ecx + 0x18]
            fld st(0)
            fmulp st(1), st(0)
            fld dword ptr [ecx + 0x10]
            fld dword ptr [ecx + 0xc]
            fxch st(3)
            _emit 0xd8
        _emit 0x09
            fsubp st(4), st(0)
            _emit 0xd8
        _emit 0x09
            fxch st(3)
            fstp dword ptr [ecx + 0x1c]
            fld dword ptr [ecx + 0x1c]
            fst dword ptr [esp + 0x10]
            fmul dword ptr [esp + 0x10]
            fld dword ptr [ecx + 4]
            fmulp st(3), st(0)
            faddp st(1), st(0)
            fxch st(1)
            fsubp st(2), st(0)
            fxch st(1)
            fst dword ptr [esp + 0xc]
            fst dword ptr [esp + 0x10]
            fmul dword ptr [esp + 0x10]
            fld dword ptr [esp + 0xc]
            fxch st(1)
            faddp st(2), st(0)
            fstp dword ptr [ecx + 0x20]
            fsqrt
            fcom dword ptr [gFloatZero_0058b280+0x78]
            fnstsw ax
            test ah, 0x41
            jne L00516b69
            fdivr qword ptr [gFloatZero_0058b280+0x8]
            fld dword ptr [ecx + 0x1c]
            fld dword ptr [ecx + 0x18]
            fld st(2)
            fmul dword ptr [esp + 0xc]
            fxch st(1)
            fmul st(0), st(3)
            fxch st(2)
            fmul st(0), st(3)
            fxch st(2)
            fstp dword ptr [ecx + 0x18]
            fxch st(1)
            fstp dword ptr [ecx + 0x1c]
            fstp dword ptr [ecx + 0x20]
            fstp st(0)
            jmp L00516b74
    L00516b69:
            fstp st(0)
            mov dword ptr [ecx + 0x18], edi
            mov dword ptr [ecx + 0x1c], esi
            mov dword ptr [ecx + 0x20], esi
    L00516b74:
            lea eax, [edx + 0xc]
            xor edx, edx
            mov ebx, dword ptr [esp + 0x18]
            xor edi, edi
            mov dx, word ptr [eax]
            mov dword ptr [esp + 0x10], edx
            xor edx, edx
            fild dword ptr [esp + 0x10]
            cmp ebx, esi
            fmul dword ptr [gFloatZero_0058b280+0x7c]
            fsub dword ptr [gFloatZero_0058b280+0x6c]
            fstp dword ptr [ecx + 0x24]
            mov dx, word ptr [eax + 2]
            mov dword ptr [esp + 0x18], edx
            fild dword ptr [esp + 0x18]
            fmul dword ptr [gFloatZero_0058b280+0x7c]
            fsub dword ptr [gFloatZero_0058b280+0x6c]
            fstp dword ptr [ecx + 0x28]
            mov di, word ptr [eax + 4]
            mov dword ptr [esp + 0x18], edi
            fild dword ptr [esp + 0x18]
            fmul dword ptr [gFloatZero_0058b280+0x7c]
            fsub dword ptr [gFloatZero_0058b280+0x6c]
            fstp dword ptr [ecx + 0x2c]
            je L00516bfd
            fld dword ptr [ecx + 0x24]
            fsub dword ptr [gFloatZero_0058b280+0x80]
            fld dword ptr [ecx + 0x2c]
            fld dword ptr [ecx + 0x28]
            fsub dword ptr [gFloatZero_0058b280+0x80]
            fxch st(1)
            fsub dword ptr [gFloatZero_0058b280+0x80]
            fxch st(2)
            fstp dword ptr [ecx + 0x24]
            fstp dword ptr [ecx + 0x28]
            fstp dword ptr [ecx + 0x2c]
    L00516bfd:
            pop edi
            pop esi
            pop ebx
            add esp, 8
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
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00516c10
void C2_HOOK_FASTCALL PossibleLock(int pValue) {
    if (gBack_screen->pixels == NULL) {
        BrPixelmapDirectLock(gBack_screen, 1);
    }
}

// FUNCTION: CARMA2_HW 0x00516c30
int C2_HOOK_FASTCALL PossibleUnlock(int pValue) {

    if (gBack_screen->pixels != NULL) {
        BrPixelmapDirectUnlock(gBack_screen);
        return 1;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00516fd0
tU16 C2_HOOK_FASTCALL PaletteEntry16Bit(br_pixelmap* pPal, int pEntry) {
    tU32* src_entry;
    int red;
    int green;
    int blue;

    src_entry = pPal->pixels;
    switch (gBack_screen->type) {
    default:
        BrFailure("Unsupported back buffer type.");
        return 0;
    case BR_PMT_RGB_565:
        red = (src_entry[pEntry] >> 8) & 0xf800;
        green = (src_entry[pEntry] >> 5) & 0x07e0;
        blue = (src_entry[pEntry] >> 3) & 0x001f;
        return red | green | blue;
    case BR_PMT_RGB_555:
        red = (src_entry[pEntry] >> 9) & 0x7c00;
        green = (src_entry[pEntry] >> 6) & 0x03e0;
        blue = (src_entry[pEntry] >> 3) & 0x001f;
        return red | green | blue;
    }
}

// Colour24BitTo16Bit

// FUNCTION: CARMA2_HW 0x005170c0
C2_NAKED __inline br_pixelmap* C2_HOOK_FASTCALL PaletteOf16Bits(br_pixelmap* pSrc) {
#ifndef CARPOCALYPSE2_MATCHING
    tU16* dst_entry;
    int i;

    if (g16bit_palette == NULL) {
        g16bit_palette = BrPixelmapAllocate(BR_PMT_RGB_565, 1, 256, NULL, 0);
        if (g16bit_palette == NULL) {
            FatalError(kFatalError_OOM_S, "16-bit palette");
        }
    }
    if (!gPalette_changed || gPalette_source != pSrc) {
        dst_entry = g16bit_palette->pixels;
        for (i = 0; i < 256; i++) {
            *dst_entry++ = PaletteEntry16Bit(pSrc, i);
        }
        gPalette_changed = 1;
        gPalette_source = pSrc;
    }
    return g16bit_palette;
#else
    __asm {
            mov eax, dword ptr [g16bit_palette]
            push ebx
            push esi
            push edi
            test eax, eax
            mov ebx, ecx
            jne L005170fe
            push eax
            push eax
            push 0x100
            push 1
            push 5
            call BrPixelmapAllocate
            add esp, 0x14
            mov dword ptr [g16bit_palette], eax
            test eax, eax
            jne L005170fe
            push offset gVector3_00661678+0xc8
            push 0x5e
            call FatalError
            mov eax, dword ptr [g16bit_palette]
            add esp, 8
    L005170fe:
            mov ecx, dword ptr [gPalette_changed]
            test ecx, ecx
            je L00517114
            cmp dword ptr [gPalette_source], ebx
            je L005171a4
    L00517114:
            mov eax, dword ptr [eax + 8]
            xor esi, esi
            mov edi, eax
    L0051711b:
            mov edx, dword ptr [gBack_screen]
            mov eax, dword ptr [ebx + 8]
            xor ecx, ecx
            mov cl, byte ptr [edx + 0x2c]
            sub ecx, 4
            je L0051715c
            dec ecx
            je L00517142
            push offset gVector3_00661678+0xa8
            call BrFailure
            add esp, 4
            xor eax, eax
            jmp L0051717e
    L00517142:
            mov ecx, dword ptr [eax + esi]
            mov eax, ecx
            mov edx, ecx
            shr eax, 8
            shr edx, 5
            and eax, 0xf800
            and edx, 0x7e0
            jmp L00517174
    L0051715c:
            mov ecx, dword ptr [eax + esi]
            mov eax, ecx
            mov edx, ecx
            shr eax, 9
            shr edx, 6
            and eax, 0x7c00
            and edx, 0x3e0
    L00517174:
            shr ecx, 3
            or eax, edx
            and ecx, 0x1f
            or eax, ecx
    L0051717e:
            mov word ptr [edi], ax
            add esi, 4
            add edi, 2
            cmp esi, 0x400
            jl L0051711b
            mov eax, dword ptr [g16bit_palette]
            mov dword ptr [gPalette_changed], 1
            mov dword ptr [gPalette_source], ebx
    L005171a4:
            pop edi
            pop esi
            pop ebx
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x005171b0
void C2_NAKED C2_HOOK_FASTCALL Copy8BitTo16Bit(br_pixelmap* pDst, br_pixelmap* pSrc, br_pixelmap* pPalette) {
#ifndef CARPOCALYPSE2_MATCHING
    int x;
    int y;
    tU8* src_start;
    tU16* dst_start;
    tU16* palette_entry;

    palette_entry = PaletteOf16Bits(pPalette)->pixels;
    for (y = 0; y < pDst->height; y++) {
        src_start = (tU8*)pSrc->pixels + pSrc->row_bytes * y;
        dst_start = (tU16*)((tU8*)pDst->pixels + pDst->row_bytes * y);
        for (x = 0; x < pDst->width; x++) {
            *dst_start = palette_entry[*src_start];
            src_start++;
            dst_start++;
        }
    }
#else
    __asm {
            push ecx
            mov eax, dword ptr [g16bit_palette]
            push ebx
            push ebp
            push esi
            push edi
            mov dword ptr [esp + 0x10], edx
            test eax, eax
            mov edi, ecx
            jne L005171f4
            push eax
            push eax
            push 0x100
            push 1
            push 5
            call BrPixelmapAllocate
            add esp, 0x14
            mov dword ptr [g16bit_palette], eax
            test eax, eax
            jne L005171f4
            push offset gVector3_00661678+0xc8
            push 0x5e
            call FatalError
            mov eax, dword ptr [g16bit_palette]
            add esp, 8
    L005171f4:
            mov ecx, dword ptr [gPalette_changed]
            mov ebp, dword ptr [esp + 0x18]
            test ecx, ecx
            je L0051720e
            cmp dword ptr [gPalette_source], ebp
            je L0051729e
    L0051720e:
            mov eax, dword ptr [eax + 8]
            xor esi, esi
            mov ebx, eax
    L00517215:
            mov edx, dword ptr [gBack_screen]
            mov eax, dword ptr [ebp + 8]
            xor ecx, ecx
            mov cl, byte ptr [edx + 0x2c]
            sub ecx, 4
            je L00517256
            dec ecx
            je L0051723c
            push offset gVector3_00661678+0xa8
            call BrFailure
            add esp, 4
            xor eax, eax
            jmp L00517278
    L0051723c:
            mov ecx, dword ptr [eax + esi]
            mov eax, ecx
            mov edx, ecx
            shr eax, 8
            shr edx, 5
            and eax, 0xf800
            and edx, 0x7e0
            jmp L0051726e
    L00517256:
            mov ecx, dword ptr [eax + esi]
            mov eax, ecx
            mov edx, ecx
            shr eax, 9
            shr edx, 6
            and eax, 0x7c00
            and edx, 0x3e0
    L0051726e:
            shr ecx, 3
            or eax, edx
            and ecx, 0x1f
            or eax, ecx
    L00517278:
            mov word ptr [ebx], ax
            add esi, 4
            add ebx, 2
            cmp esi, 0x400
            jl L00517215
            mov eax, dword ptr [g16bit_palette]
            mov dword ptr [gPalette_changed], 1
            mov dword ptr [gPalette_source], ebp
    L0051729e:
            mov ebx, dword ptr [eax + 8]
            xor ebp, ebp
            cmp word ptr [edi + 0x36], bp
            jbe L005172f9
    L005172a9:
            mov ecx, dword ptr [esp + 0x10]
            mov eax, ebp
            movsx edx, word ptr [ecx + 0x28]
            imul eax, edx
            mov edx, dword ptr [ecx + 8]
            movsx ecx, word ptr [edi + 0x28]
            add eax, edx
            mov edx, ebp
            imul edx, ecx
            mov ecx, dword ptr [edi + 8]
            add edx, ecx
            xor ecx, ecx
            cmp word ptr [edi + 0x34], cx
            jbe L005172ee
            mov esi, edx
    L005172d3:
            xor edx, edx
            add esi, 2
            mov dl, byte ptr [eax + ecx]
            inc ecx
            mov dx, word ptr [ebx + edx*2]
            mov word ptr [esi - 2], dx
            xor edx, edx
            mov dx, word ptr [edi + 0x34]
            cmp ecx, edx
            jl L005172d3
    L005172ee:
            xor eax, eax
            inc ebp
            mov ax, word ptr [edi + 0x36]
            cmp ebp, eax
            jl L005172a9
    L005172f9:
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
            nop
            nop
            nop
            nop
            nop
            nop
            sub esp, 0xc
            mov eax, dword ptr [g16bit_palette]
            push ebx
            push ebp
            mov ebp, ecx
            push esi
            push edi
            test eax, eax
            mov word ptr [esp + 0x10], dx
            mov dword ptr [esp + 0x14], ebp
            jne L0051735b
            push eax
            push eax
            push 0x100
            push 1
            push 5
            call BrPixelmapAllocate
            add esp, 0x14
            mov dword ptr [g16bit_palette], eax
            test eax, eax
            jne L0051735b
            push offset gVector3_00661678+0xc8
            push 0x5e
            call FatalError
            mov eax, dword ptr [g16bit_palette]
            add esp, 8
    L0051735b:
            mov ecx, dword ptr [gPalette_changed]
            mov ebx, dword ptr [esp + 0x38]
            test ecx, ecx
            je L00517375
            cmp dword ptr [gPalette_source], ebx
            je L0051740d
    L00517375:
            mov eax, dword ptr [eax + 8]
            xor esi, esi
            mov edi, eax
    L0051737c:
            mov edx, dword ptr [gBack_screen]
            mov eax, dword ptr [ebx + 8]
            xor ecx, ecx
            mov cl, byte ptr [edx + 0x2c]
            sub ecx, 4
            je L005173c5
            dec ecx
            je L005173a3
            push offset gVector3_00661678+0xa8
            call BrFailure
            add esp, 4
            xor eax, eax
            jmp L005173e7
    L005173a3:
            mov ecx, dword ptr [eax + esi]
            mov eax, ecx
            mov edx, ecx
            shr eax, 8
            shr edx, 5
            and eax, 0xf800
            and edx, 0x7e0
            shr ecx, 3
            or eax, edx
            and ecx, 0x1f
            jmp L005173e5
    L005173c5:
            mov ecx, dword ptr [eax + esi]
            mov eax, ecx
            mov edx, ecx
            shr eax, 9
            shr edx, 3
            and eax, 0x7c00
            and edx, 0x1f
            shr ecx, 6
            or eax, edx
            and ecx, 0x3e0
    L005173e5:
            or eax, ecx
    L005173e7:
            mov word ptr [edi], ax
            add esi, 4
            add edi, 2
            cmp esi, 0x400
            jl L0051737c
            mov eax, dword ptr [g16bit_palette]
            mov dword ptr [gPalette_changed], 1
            mov dword ptr [gPalette_source], ebx
    L0051740d:
            mov eax, dword ptr [eax + 8]
            mov ebx, dword ptr [esp + 0x24]
            mov edi, dword ptr [esp + 0x2c]
            mov dword ptr [esp + 0x18], eax
            mov cx, word ptr [ebx + 0x38]
            mov eax, dword ptr [esp + 0x28]
            mov dx, word ptr [ebx + 0x3a]
            add eax, ecx
            mov ecx, dword ptr [esp + 0x10]
            add edi, edx
            mov dx, word ptr [ebp + 0x38]
            mov esi, dword ptr [esp + 0x30]
            add ecx, edx
            mov dx, word ptr [ebp + 0x3a]
            mov ebp, dword ptr [esp + 0x20]
            mov dword ptr [esp + 0x2c], edi
            add ebp, edx
            mov dword ptr [esp + 0x10], ecx
            test ax, ax
            mov dword ptr [esp + 0x20], ebp
            jge L0051745f
            sub ecx, eax
            add esi, eax
            mov dword ptr [esp + 0x10], ecx
            xor eax, eax
    L0051745f:
            cmp word ptr [esp + 0x10], 0
            jge L00517473
            add esi, ecx
            sub eax, ecx
            mov dword ptr [esp + 0x10], 0
    L00517473:
            mov edx, dword ptr [esp + 0x34]
            test di, di
            jge L0051748a
            add edx, edi
            sub ebp, edi
            xor edi, edi
            mov dword ptr [esp + 0x20], ebp
            mov dword ptr [esp + 0x2c], edi
    L0051748a:
            test bp, bp
            jge L0051749f
            add edx, ebp
            sub edi, ebp
            mov dword ptr [esp + 0x2c], edi
            mov dword ptr [esp + 0x20], 0
    L0051749f:
            mov cx, word ptr [ebx + 0x34]
            movsx edi, ax
            movsx ebp, si
            mov dword ptr [esp + 0x34], edi
            add ebp, edi
            mov edi, ecx
            and edi, 0xffff
            cmp ebp, edi
            jle L005174bf
            sub ecx, eax
            mov esi, ecx
    L005174bf:
            movsx ecx, word ptr [esp + 0x10]
            mov ebp, dword ptr [esp + 0x14]
            movsx edi, si
            mov ax, word ptr [ebp + 0x34]
            add edi, ecx
            mov ecx, eax
            and ecx, 0xffff
            cmp edi, ecx
            jle L005174e3
            sub eax, dword ptr [esp + 0x10]
            mov esi, eax
    L005174e3:
            movsx ecx, word ptr [esp + 0x2c]
            mov ax, word ptr [ebx + 0x36]
            mov dword ptr [esp + 0x30], ecx
            movsx edi, dx
            add edi, ecx
            mov ecx, eax
            and ecx, 0xffff
            cmp edi, ecx
            jle L00517507
            sub eax, dword ptr [esp + 0x2c]
            mov edx, eax
    L00517507:
            movsx ecx, word ptr [esp + 0x20]
            mov ax, word ptr [ebp + 0x36]
            mov dword ptr [esp + 0x2c], ecx
            movsx edi, dx
            add edi, ecx
            mov ecx, eax
            and ecx, 0xffff
            cmp edi, ecx
            jle L0051752b
            sub eax, dword ptr [esp + 0x20]
            mov edx, eax
    L0051752b:
            test si, si
            jle L005175db
            test dx, dx
            jle L005175db
            movsx eax, dx
            test eax, eax
            jle L005175db
            movsx ecx, word ptr [esp + 0x10]
            movsx edi, si
            lea edx, [ecx + ecx]
            mov ecx, dword ptr [esp + 0x2c]
            mov dword ptr [esp + 0x2c], ecx
            mov ecx, dword ptr [esp + 0x30]
            mov dword ptr [esp + 0x38], edx
            mov dword ptr [esp + 0x20], ecx
            mov dword ptr [esp + 0x30], eax
    L0051756b:
            movsx edx, word ptr [ebx + 0x28]
            imul ecx, edx
            mov eax, dword ptr [esp + 0x34]
            mov esi, dword ptr [ebx + 8]
            movsx edx, word ptr [ebp + 0x28]
            add ecx, eax
            mov eax, dword ptr [esp + 0x2c]
            imul eax, edx
            mov edx, dword ptr [esp + 0x38]
            add ecx, esi
            mov esi, dword ptr [ebp + 8]
            add eax, edx
            add eax, esi
            xor esi, esi
            test edi, edi
            jle L005175be
    L00517599:
            mov dl, byte ptr [ecx + esi]
            test dl, dl
            je L005175b6
            mov ebp, dword ptr [esp + 0x18]
            and edx, 0xff
            mov dx, word ptr [ebp + edx*2]
            mov ebp, dword ptr [esp + 0x14]
            mov word ptr [eax], dx
    L005175b6:
            inc esi
            add eax, 2
            cmp esi, edi
            jl L00517599
    L005175be:
            mov ecx, dword ptr [esp + 0x20]
            mov edx, dword ptr [esp + 0x2c]
            mov eax, dword ptr [esp + 0x30]
            inc ecx
            inc edx
            dec eax
            mov dword ptr [esp + 0x20], ecx
            mov dword ptr [esp + 0x2c], edx
            mov dword ptr [esp + 0x30], eax
            jne L0051756b
    L005175db:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0xc
            ret 0x1c
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
            sub esp, 0xc
            mov eax, dword ptr [g16bit_palette]
            push ebx
            push ebp
            xor ebp, ebp
            push esi
            cmp eax, ebp
            push edi
            mov word ptr [esp + 0x10], dx
            mov dword ptr [esp + 0x14], ecx
            jne L0051763b
            push ebp
            push ebp
            push 0x100
            push 1
            push 5
            call BrPixelmapAllocate
            add esp, 0x14
            cmp eax, ebp
            mov dword ptr [g16bit_palette], eax
            jne L0051763b
            push offset gVector3_00661678+0xc8
            push 0x5e
            call FatalError
            mov eax, dword ptr [g16bit_palette]
            add esp, 8
    L0051763b:
            mov ecx, dword ptr [gPalette_changed]
            mov ebx, dword ptr [esp + 0x38]
            cmp ecx, ebp
            je L00517655
            cmp dword ptr [gPalette_source], ebx
            je L005176ed
    L00517655:
            mov eax, dword ptr [eax + 8]
            xor esi, esi
            mov edi, eax
    L0051765c:
            mov edx, dword ptr [gBack_screen]
            mov eax, dword ptr [ebx + 8]
            xor ecx, ecx
            mov cl, byte ptr [edx + 0x2c]
            sub ecx, 4
            je L005176a5
            dec ecx
            je L00517683
            push offset gVector3_00661678+0xa8
            call BrFailure
            add esp, 4
            xor eax, eax
            jmp L005176c7
    L00517683:
            mov ecx, dword ptr [eax + esi]
            mov eax, ecx
            mov edx, ecx
            shr eax, 8
            shr edx, 5
            and eax, 0xf800
            and edx, 0x7e0
            shr ecx, 3
            or eax, edx
            and ecx, 0x1f
            jmp L005176c5
    L005176a5:
            mov ecx, dword ptr [eax + esi]
            mov eax, ecx
            mov edx, ecx
            shr eax, 9
            shr edx, 3
            and eax, 0x7c00
            and edx, 0x1f
            shr ecx, 6
            or eax, edx
            and ecx, 0x3e0
    L005176c5:
            or eax, ecx
    L005176c7:
            mov word ptr [edi], ax
            add esi, 4
            add edi, 2
            cmp esi, 0x400
            jl L0051765c
            mov eax, dword ptr [g16bit_palette]
            mov dword ptr [gPalette_changed], 1
            mov dword ptr [gPalette_source], ebx
    L005176ed:
            mov eax, dword ptr [eax + 8]
            mov edx, dword ptr [esp + 0x30]
            mov dword ptr [esp + 0x18], eax
            mov eax, dword ptr [esp + 0x28]
            cmp ax, bp
            jge L0051770f
            mov ecx, dword ptr [esp + 0x10]
            add edx, eax
            sub ecx, eax
            xor eax, eax
            mov dword ptr [esp + 0x10], ecx
    L0051770f:
            cmp word ptr [esp + 0x10], bp
            jge L00517722
            mov ecx, dword ptr [esp + 0x10]
            mov dword ptr [esp + 0x10], ebp
            add edx, ecx
            sub eax, ecx
    L00517722:
            mov ecx, dword ptr [esp + 0x2c]
            cmp cx, bp
            jge L00517743
            mov esi, dword ptr [esp + 0x34]
            mov edi, dword ptr [esp + 0x20]
            add esi, ecx
            sub edi, ecx
            xor ecx, ecx
            mov dword ptr [esp + 0x20], edi
            mov dword ptr [esp + 0x2c], ecx
            jmp L0051774b
    L00517743:
            mov edi, dword ptr [esp + 0x20]
            mov esi, dword ptr [esp + 0x34]
    L0051774b:
            cmp di, bp
            jge L0051775c
            add esi, edi
            sub ecx, edi
            mov dword ptr [esp + 0x2c], ecx
            mov dword ptr [esp + 0x20], ebp
    L0051775c:
            mov ebx, dword ptr [esp + 0x24]
            movsx edi, ax
            mov cx, word ptr [ebx + 0x34]
            mov dword ptr [esp + 0x34], edi
            movsx ebp, dx
            add ebp, edi
            mov edi, ecx
            and edi, 0xffff
            cmp ebp, edi
            jle L00517780
            sub ecx, eax
            mov edx, ecx
    L00517780:
            movsx ecx, word ptr [esp + 0x10]
            mov ebp, dword ptr [esp + 0x14]
            movsx edi, dx
            mov ax, word ptr [ebp + 0x34]
            add edi, ecx
            mov ecx, eax
            and ecx, 0xffff
            cmp edi, ecx
            jle L005177a4
            sub eax, dword ptr [esp + 0x10]
            mov edx, eax
    L005177a4:
            movsx ecx, word ptr [esp + 0x2c]
            mov ax, word ptr [ebx + 0x36]
            mov dword ptr [esp + 0x30], ecx
            movsx edi, si
            add edi, ecx
            mov ecx, eax
            and ecx, 0xffff
            cmp edi, ecx
            jle L005177c8
            sub eax, dword ptr [esp + 0x2c]
            mov esi, eax
    L005177c8:
            movsx ecx, word ptr [esp + 0x20]
            mov ax, word ptr [ebp + 0x36]
            mov dword ptr [esp + 0x2c], ecx
            movsx edi, si
            add edi, ecx
            mov ecx, eax
            and ecx, 0xffff
            cmp edi, ecx
            jle L005177ec
            sub eax, dword ptr [esp + 0x20]
            mov esi, eax
    L005177ec:
            test dx, dx
            jle L0051789b
            test si, si
            jle L0051789b
            movsx eax, si
            test eax, eax
            jle L0051789b
            movsx ecx, word ptr [esp + 0x10]
            add ecx, ecx
            mov dword ptr [esp + 0x38], ecx
            mov ecx, dword ptr [esp + 0x30]
            movsx edi, dx
            mov edx, dword ptr [esp + 0x2c]
            mov dword ptr [esp + 0x2c], ecx
            mov dword ptr [esp + 0x20], edx
            mov dword ptr [esp + 0x30], eax
    L0051782b:
            movsx eax, word ptr [ebx + 0x28]
            imul ecx, eax
            mov eax, dword ptr [esp + 0x34]
            mov esi, dword ptr [ebx + 8]
            movsx edx, word ptr [ebp + 0x28]
            add ecx, eax
            mov eax, dword ptr [esp + 0x20]
            imul eax, edx
            mov edx, dword ptr [esp + 0x38]
            add ecx, esi
            mov esi, dword ptr [ebp + 8]
            add eax, edx
            add eax, esi
            xor esi, esi
            test edi, edi
            jle L0051787e
    L00517859:
            mov dl, byte ptr [ecx + esi]
            test dl, dl
            je L00517876
            mov ebp, dword ptr [esp + 0x18]
            and edx, 0xff
            mov dx, word ptr [ebp + edx*2]
            mov ebp, dword ptr [esp + 0x14]
            mov word ptr [eax], dx
    L00517876:
            inc esi
            add eax, 2
            cmp esi, edi
            jl L00517859
    L0051787e:
            mov ecx, dword ptr [esp + 0x2c]
            mov edx, dword ptr [esp + 0x20]
            mov eax, dword ptr [esp + 0x30]
            inc ecx
            inc edx
            dec eax
            mov dword ptr [esp + 0x2c], ecx
            mov dword ptr [esp + 0x20], edx
            mov dword ptr [esp + 0x30], eax
            jne L0051782b
    L0051789b:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0xc
            ret 0x1c
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
            sub esp, 0xc
            mov eax, dword ptr [g16bit_palette]
            push ebx
            push ebp
            mov ebp, ecx
            push esi
            push edi
            test eax, eax
            mov word ptr [esp + 0x10], dx
            mov dword ptr [esp + 0x18], ebp
            jne L005178fb
            push eax
            push eax
            push 0x100
            push 1
            push 5
            call BrPixelmapAllocate
            add esp, 0x14
            mov dword ptr [g16bit_palette], eax
            test eax, eax
            jne L005178fb
            push offset gVector3_00661678+0xc8
            push 0x5e
            call FatalError
            mov eax, dword ptr [g16bit_palette]
            add esp, 8
    L005178fb:
            mov ecx, dword ptr [gPalette_changed]
            mov ebx, dword ptr [esp + 0x38]
            test ecx, ecx
            je L00517915
            cmp dword ptr [gPalette_source], ebx
            je L005179a5
    L00517915:
            mov eax, dword ptr [eax + 8]
            xor esi, esi
            mov edi, eax
    L0051791c:
            mov edx, dword ptr [gBack_screen]
            mov eax, dword ptr [ebx + 8]
            xor ecx, ecx
            mov cl, byte ptr [edx + 0x2c]
            sub ecx, 4
            je L0051795d
            dec ecx
            je L00517943
            push offset gVector3_00661678+0xa8
            call BrFailure
            add esp, 4
            xor eax, eax
            jmp L0051797f
    L00517943:
            mov ecx, dword ptr [eax + esi]
            mov eax, ecx
            mov edx, ecx
            shr eax, 8
            shr edx, 5
            and eax, 0xf800
            and edx, 0x7e0
            jmp L00517975
    L0051795d:
            mov ecx, dword ptr [eax + esi]
            mov eax, ecx
            mov edx, ecx
            shr eax, 9
            shr edx, 6
            and eax, 0x7c00
            and edx, 0x3e0
    L00517975:
            shr ecx, 3
            or eax, edx
            and ecx, 0x1f
            or eax, ecx
    L0051797f:
            mov word ptr [edi], ax
            add esi, 4
            add edi, 2
            cmp esi, 0x400
            jl L0051791c
            mov eax, dword ptr [g16bit_palette]
            mov dword ptr [gPalette_changed], 1
            mov dword ptr [gPalette_source], ebx
    L005179a5:
            mov edi, dword ptr [esp + 0x24]
            mov eax, dword ptr [eax + 8]
            mov ecx, dword ptr [esp + 0x28]
            mov esi, dword ptr [esp + 0x10]
            mov dx, word ptr [edi + 0x38]
            mov dword ptr [esp + 0x14], eax
            mov ax, word ptr [edi + 0x3a]
            add ecx, edx
            mov edx, dword ptr [esp + 0x2c]
            add edx, eax
            mov ax, word ptr [ebp + 0x38]
            add esi, eax
            mov ax, word ptr [ebp + 0x3a]
            mov dword ptr [esp + 0x10], esi
            mov esi, dword ptr [esp + 0x20]
            add esi, eax
            movsx eax, word ptr [esp + 0x34]
            test eax, eax
            jle L00517a82
            movsx ecx, cx
            mov dword ptr [esp + 0x24], ecx
            mov dword ptr [esp + 0x38], eax
            movsx ecx, word ptr [esp + 0x10]
            shl ecx, 1
            mov dword ptr [esp + 0x28], ecx
            movsx ebx, word ptr [esp + 0x30]
            movsx ecx, si
            mov dword ptr [esp + 0x34], ecx
            movsx ecx, dx
            mov dword ptr [esp + 0x30], ecx
    L00517a12:
            movsx edx, word ptr [edi + 0x28]
            imul ecx, edx
            mov eax, dword ptr [esp + 0x24]
            mov esi, dword ptr [edi + 8]
            movsx edx, word ptr [ebp + 0x28]
            add ecx, eax
            mov eax, dword ptr [esp + 0x34]
            imul eax, edx
            mov edx, dword ptr [esp + 0x28]
            add ecx, esi
            mov esi, dword ptr [ebp + 8]
            add eax, edx
            add eax, esi
            xor esi, esi
            test ebx, ebx
            jle L00517a65
    L00517a40:
            mov dl, byte ptr [ecx + esi]
            test dl, dl
            je L00517a5d
            mov ebp, dword ptr [esp + 0x14]
            and edx, 0xff
            mov dx, word ptr [ebp + edx*2]
            mov ebp, dword ptr [esp + 0x18]
            mov word ptr [eax], dx
    L00517a5d:
            inc esi
            add eax, 2
            cmp esi, ebx
            jl L00517a40
    L00517a65:
            mov ecx, dword ptr [esp + 0x30]
            mov edx, dword ptr [esp + 0x34]
            mov eax, dword ptr [esp + 0x38]
            inc ecx
            inc edx
            dec eax
            mov dword ptr [esp + 0x30], ecx
            mov dword ptr [esp + 0x34], edx
            mov dword ptr [esp + 0x38], eax
            jne L00517a12
    L00517a82:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0xc
            ret 0x1c
            nop
            nop
            nop
            nop
            sub esp, 0x10
            mov eax, dword ptr [g16bit_palette]
            push ebx
            push ebp
            mov ebp, ecx
            push esi
            push edi
            test eax, eax
            mov word ptr [esp + 0x10], dx
            mov dword ptr [esp + 0x14], ebp
            jne L00517adb
            push eax
            push eax
            push 0x100
            push 1
            push 5
            call BrPixelmapAllocate
            add esp, 0x14
            mov dword ptr [g16bit_palette], eax
            test eax, eax
            jne L00517adb
            push offset gVector3_00661678+0xc8
            push 0x5e
            call FatalError
            mov eax, dword ptr [g16bit_palette]
            add esp, 8
    L00517adb:
            mov ecx, dword ptr [gPalette_changed]
            mov ebx, dword ptr [esp + 0x40]
            test ecx, ecx
            je L00517af5
            cmp dword ptr [gPalette_source], ebx
            je L00517b8d
    L00517af5:
            mov eax, dword ptr [eax + 8]
            xor esi, esi
            mov edi, eax
    L00517afc:
            mov edx, dword ptr [gBack_screen]
            mov eax, dword ptr [ebx + 8]
            xor ecx, ecx
            mov cl, byte ptr [edx + 0x2c]
            sub ecx, 4
            je L00517b45
            dec ecx
            je L00517b23
            push offset gVector3_00661678+0xa8
            call BrFailure
            add esp, 4
            xor eax, eax
            jmp L00517b67
    L00517b23:
            mov ecx, dword ptr [eax + esi]
            mov eax, ecx
            mov edx, ecx
            shr eax, 8
            shr edx, 5
            and eax, 0xf800
            and edx, 0x7e0
            shr ecx, 3
            or eax, edx
            and ecx, 0x1f
            jmp L00517b65
    L00517b45:
            mov ecx, dword ptr [eax + esi]
            mov eax, ecx
            mov edx, ecx
            shr eax, 9
            shr edx, 3
            and eax, 0x7c00
            and edx, 0x1f
            shr ecx, 6
            or eax, edx
            and ecx, 0x3e0
    L00517b65:
            or eax, ecx
    L00517b67:
            mov word ptr [edi], ax
            add esi, 4
            add edi, 2
            cmp esi, 0x400
            jl L00517afc
            mov eax, dword ptr [g16bit_palette]
            mov dword ptr [gPalette_changed], 1
            mov dword ptr [gPalette_source], ebx
    L00517b8d:
            mov ebx, dword ptr [esp + 0x28]
            mov edx, dword ptr [esp + 0x2c]
            mov edi, dword ptr [esp + 0x10]
            mov eax, dword ptr [eax + 8]
            mov cx, word ptr [ebx + 0x38]
            mov si, word ptr [ebx + 0x3a]
            add edx, ecx
            mov ecx, dword ptr [esp + 0x30]
            add ecx, esi
            mov si, word ptr [ebp + 0x38]
            add edi, esi
            mov esi, dword ptr [esp + 0x24]
            mov dword ptr [esp + 0x10], edi
            mov di, word ptr [ebp + 0x3a]
            mov dword ptr [esp + 0x1c], eax
            xor eax, eax
            add esi, edi
            mov edi, dword ptr [esp + 0x38]
            test cx, cx
            mov dword ptr [esp + 0x18], eax
            mov dword ptr [esp + 0x2c], edx
            jge L00517bdd
            add edi, ecx
            sub esi, ecx
            xor ecx, ecx
    L00517bdd:
            test si, si
            jge L00517be8
            add edi, esi
            sub ecx, esi
            xor esi, esi
    L00517be8:
            mov bx, word ptr [ebx + 0x36]
            mov word ptr [esp + 0x38], bx
            movsx ebp, cx
            movsx ebx, di
            mov dword ptr [esp + 0x30], ebp
            add ebx, ebp
            mov ebp, dword ptr [esp + 0x38]
            and ebp, 0xffff
            cmp ebx, ebp
            jle L00517c11
            mov edi, dword ptr [esp + 0x38]
            sub edi, ecx
    L00517c11:
            mov ebx, dword ptr [esp + 0x14]
            movsx ecx, si
            mov bp, word ptr [ebx + 0x36]
            mov dword ptr [esp + 0x40], ecx
            mov word ptr [esp + 0x38], bp
            movsx ebp, di
            add ebp, ecx
            mov ecx, dword ptr [esp + 0x38]
            and ecx, 0xffff
            cmp ebp, ecx
            jle L00517c3e
            mov edi, dword ptr [esp + 0x38]
            sub edi, esi
    L00517c3e:
            test di, di
            jle L00517d7c
            mov ecx, dword ptr [esp + 0x34]
            test dx, dx
            jge L00517c66
            mov esi, dword ptr [esp + 0x10]
            add ecx, edx
            sub esi, edx
            xor edx, edx
            mov dword ptr [esp + 0x34], ecx
            mov dword ptr [esp + 0x10], esi
            mov dword ptr [esp + 0x2c], edx
    L00517c66:
            mov esi, dword ptr [esp + 0x28]
            movsx ebp, cx
            mov si, word ptr [esi + 0x34]
            mov word ptr [esp + 0x38], si
            movsx esi, dx
            add esi, ebp
            mov ebp, dword ptr [esp + 0x38]
            and ebp, 0xffff
            cmp esi, ebp
            jle L00517c93
            mov ecx, dword ptr [esp + 0x38]
            sub ecx, edx
            mov dword ptr [esp + 0x34], ecx
    L00517c93:
            movsx esi, di
            test esi, esi
            jle L00517d7c
            mov edi, dword ptr [esp + 0x40]
            mov dword ptr [esp + 0x40], edi
            mov edi, dword ptr [esp + 0x30]
            mov dword ptr [esp + 0x38], edi
            mov dword ptr [esp + 0x30], esi
            jmp L00517cbc
    L00517cb4:
            mov edx, dword ptr [esp + 0x2c]
            mov ecx, dword ptr [esp + 0x34]
    L00517cbc:
            mov edi, dword ptr [esp + 0x10]
            sar eax, 0x10
            add eax, edi
            test ax, ax
            jge L00517cd0
            add ecx, eax
            sub edx, eax
            xor eax, eax
    L00517cd0:
            mov di, word ptr [ebx + 0x34]
            movsx esi, ax
            movsx ebx, cx
            mov ebp, edi
            add ebx, esi
            and ebp, 0xffff
            cmp ebx, ebp
            jle L00517cec
            sub edi, eax
            mov ecx, edi
    L00517cec:
            test cx, cx
            jle L00517d49
            mov edi, dword ptr [esp + 0x28]
            mov ebx, dword ptr [esp + 0x14]
            movsx edx, dx
            movsx eax, word ptr [edi + 0x28]
            imul eax, dword ptr [esp + 0x38]
            mov ebp, dword ptr [edi + 8]
            mov edi, dword ptr [ebx + 8]
            add eax, ebp
            add eax, edx
            movsx edx, word ptr [ebx + 0x28]
            imul edx, dword ptr [esp + 0x40]
            add edx, edi
            lea edi, [edx + esi*2]
            xor edx, edx
            movsx esi, cx
            test esi, esi
            jle L00517d4d
            mov ebp, dword ptr [esp + 0x1c]
    L00517d2a:
            mov cl, byte ptr [eax + edx]
            test cl, cl
            je L00517d3f
            and ecx, 0xff
            mov cx, word ptr [ebp + ecx*2]
            mov word ptr [edi], cx
    L00517d3f:
            inc edx
            add edi, 2
            cmp edx, esi
            jl L00517d2a
            jmp L00517d4d
    L00517d49:
            mov ebx, dword ptr [esp + 0x14]
    L00517d4d:
            mov eax, dword ptr [esp + 0x18]
            mov edi, dword ptr [esp + 0x3c]
            mov esi, dword ptr [esp + 0x38]
            mov edx, dword ptr [esp + 0x40]
            mov ecx, dword ptr [esp + 0x30]
            add eax, edi
            inc esi
            inc edx
            dec ecx
            mov dword ptr [esp + 0x18], eax
            mov dword ptr [esp + 0x38], esi
            mov dword ptr [esp + 0x40], edx
            mov dword ptr [esp + 0x30], ecx
            jne L00517cb4
    L00517d7c:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x10
            ret 0x20
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

// FUNCTION: CARMA2_HW 0x00517d90
void C2_NAKED C2_HOOK_FASTCALL DRPixelmapCopy(br_pixelmap* dst, br_pixelmap* src) {
#ifndef CARPOCALYPSE2_MATCHING

    if (dst->type == src->type) {
        BrPixelmapCopy(dst, src);
    } else if (dst->type != BR_PMT_INDEX_8 && src->type == BR_PMT_INDEX_8) {
        Copy8BitTo16Bit(dst, src, gCurrent_palette);
    }
#else
    __asm {
            push ecx
            push ebx
            push ebp
            push esi
            push edi
            mov edi, ecx
            mov dword ptr [esp + 0x10], edx
            mov cl, byte ptr [edx + 0x2c]
            mov al, byte ptr [edi + 0x2c]
            cmp al, cl
            je L00517f00
            cmp al, 3
            je L00517f0a
            cmp cl, 3
            jne L00517f0a
            mov eax, dword ptr [g16bit_palette]
            mov ebp, dword ptr [gCurrent_palette]
            test eax, eax
            jne L00517df9
            push eax
            push eax
            push 0x100
            push 1
            push 5
            call BrPixelmapAllocate
            add esp, 0x14
            mov dword ptr [g16bit_palette], eax
            test eax, eax
            jne L00517df9
            push offset gVector3_00661678+0xc8
            push 0x5e
            call FatalError
            mov eax, dword ptr [g16bit_palette]
            add esp, 8
    L00517df9:
            mov ecx, dword ptr [gPalette_changed]
            test ecx, ecx
            je L00517e0f
            cmp dword ptr [gPalette_source], ebp
            je L00517e9f
    L00517e0f:
            mov eax, dword ptr [eax + 8]
            xor esi, esi
            mov ebx, eax
    L00517e16:
            mov edx, dword ptr [gBack_screen]
            mov eax, dword ptr [ebp + 8]
            xor ecx, ecx
            mov cl, byte ptr [edx + 0x2c]
            sub ecx, 4
            je L00517e57
            dec ecx
            je L00517e3d
            push offset gVector3_00661678+0xa8
            call BrFailure
            add esp, 4
            xor eax, eax
            jmp L00517e79
    L00517e3d:
            mov ecx, dword ptr [eax + esi]
            mov eax, ecx
            mov edx, ecx
            shr eax, 8
            shr edx, 5
            and eax, 0xf800
            and edx, 0x7e0
            jmp L00517e6f
    L00517e57:
            mov ecx, dword ptr [eax + esi]
            mov eax, ecx
            mov edx, ecx
            shr eax, 9
            shr edx, 6
            and eax, 0x7c00
            and edx, 0x3e0
    L00517e6f:
            shr ecx, 3
            or eax, edx
            and ecx, 0x1f
            or eax, ecx
    L00517e79:
            mov word ptr [ebx], ax
            add esi, 4
            add ebx, 2
            cmp esi, 0x400
            jl L00517e16
            mov eax, dword ptr [g16bit_palette]
            mov dword ptr [gPalette_changed], 1
            mov dword ptr [gPalette_source], ebp
    L00517e9f:
            mov ebx, dword ptr [eax + 8]
            xor ebp, ebp
            cmp word ptr [edi + 0x36], bp
            jbe L00517f0a
    L00517eaa:
            mov ecx, dword ptr [esp + 0x10]
            mov eax, ebp
            movsx edx, word ptr [ecx + 0x28]
            imul eax, edx
            mov edx, dword ptr [ecx + 8]
            movsx ecx, word ptr [edi + 0x28]
            add eax, edx
            mov edx, ebp
            imul edx, ecx
            mov ecx, dword ptr [edi + 8]
            add edx, ecx
            xor ecx, ecx
            cmp word ptr [edi + 0x34], cx
            jbe L00517eef
            mov esi, edx
    L00517ed4:
            xor edx, edx
            add esi, 2
            mov dl, byte ptr [eax + ecx]
            inc ecx
            mov dx, word ptr [ebx + edx*2]
            mov word ptr [esi - 2], dx
            xor edx, edx
            mov dx, word ptr [edi + 0x34]
            cmp ecx, edx
            jl L00517ed4
    L00517eef:
            xor eax, eax
            inc ebp
            mov ax, word ptr [edi + 0x36]
            cmp ebp, eax
            jl L00517eaa
            pop edi
            pop esi
            pop ebp
            pop ebx
            pop ecx
            ret
    L00517f00:
            push edx
            push edi
            call BrPixelmapCopy
            add esp, 8
    L00517f0a:
            pop edi
            pop esi
            pop ebp
            pop ebx
            pop ecx
            ret
            sub esp, 8
            push ebx
            push ebp
            mov ebp, edx
            mov ebx, ecx
            push esi
            push edi
            test ebp, ebp
            mov dword ptr [esp + 0x10], ebx
            je L00517f8b
            jmp L00517f29
    L00517f25:
            mov ebx, dword ptr [esp + 0x10]
    L00517f29:
            mov edi, dword ptr [ebp + 4]
            movsx eax, byte ptr [edi]
            push eax
            call tolower
            movsx ecx, byte ptr [ebx]
            add esp, 4
            mov esi, eax
            push ecx
            call tolower
            mov dword ptr [esp + 0x18], esi
            mov esi, eax
            mov edx, dword ptr [esp + 0x18]
            add esp, 4
            sub esi, edx
            jne L00517f80
    L00517f54:
            mov al, byte ptr [ebx]
            inc ebx
            test al, al
            je L00517f80
            mov cl, byte ptr [edi]
            inc edi
            test cl, cl
            je L00517f80
            movsx edx, byte ptr [ebx]
            push edx
            call tolower
            mov esi, eax
            add esp, 4
            movsx eax, byte ptr [edi]
            push eax
            call tolower
            add esp, 4
            sub esi, eax
            je L00517f54
    L00517f80:
            test esi, esi
            je L00517f8b
            mov ebp, dword ptr [ebp]
            test ebp, ebp
            jne L00517f25
    L00517f8b:
            pop edi
            mov eax, ebp
            pop esi
            pop ebp
            pop ebx
            add esp, 8
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00517f10
static tMaterial_exception* C2_HOOK_FASTCALL FindExceptionInList(const char* pIdentifier, tMaterial_exception* pList) {

    for (; pList != NULL; pList = pList->next) {
        if (DRStricmp(pIdentifier, pList->texture_name) == 0) {
            break;
        }
    }
    return pList;
}

// FUNCTION: CARMA2_HW 0x00517fa0
void C2_HOOK_FASTCALL NobbleNonzeroBlacks(br_pixelmap* pPalette) {
    int modified;
    int i;
    br_colour *pixels;
    br_colour c;
    int r;
    int g;
    int b;

    pixels = pPalette->pixels;
    modified = 0;
    if (pixels[0] != BR_COLOUR_RGBA(0, 0, 0, 0)) {
        pixels[0] = BR_COLOUR_RGBA(0, 0, 0, 0);
        modified = 1;
    }
    for (i = 1; i < 256; i++) {
        c = pixels[i];
        r = BR_COLOUR_RED(c);
        g = BR_COLOUR_GRN(c);
        b = BR_COLOUR_BLU(c);
        if (r == 0 && g == 0 && b == 0) {
            pixels[i] = BR_COLOUR_RGB(1, 1, 1);
            modified = 1;
        }
    }
    if (modified) {
        BrMapUpdate(pPalette, BR_MAPU_ALL);
    }
}

// FUNCTION: CARMA2_HW 0x00518010
C2_NAKED tU8* C2_HOOK_FASTCALL FUN_00518010(br_pixelmap* pPixelmap) {
#ifndef CARPOCALYPSE2_MATCHING
    int iVar1;
    char cVar2;
    tU16 uVar3;
    char* extraout_EAX;
    tU32* extraout_EAX_00;
    tU32* extraout_EAX_01;
    tU8* pbVar4;
    tU32 uVar5;
    tU16 uVar6;
    tU16* puVar7;
    tU32 uVar8;
    int iVar9;
    tU32* puVar10;
    char* pcVar11;
    char* pcVar12;
    tU32* puVar13;
    int bVar14;
    tU32 local_24;
    int local_14;
    tU8 local_10;
    tU32 local_c;
    br_pixelmap* pBack_screen;
    tU32* source_palette;

    iVar1 = (int)gRender_palette;
    if ((g16bit_palette == NULL) &&
        (g16bit_palette = BrPixelmapAllocate(5, 1, 0x100, NULL, 0), g16bit_palette == NULL)) {
        FatalError(kFatalError_OOM_S, "16-bit palette");
    }
    if ((gPalette_changed == 0) || (gPalette_source != (br_pixelmap*)iVar1)) {
        puVar7 = *(tU16**)((tU8*)g16bit_palette + 8);
        pBack_screen = gBack_screen;
        source_palette = ((br_pixelmap*)iVar1)->pixels;
        iVar9 = 0;
        do {
            switch (pBack_screen->type) {
            case 4:
                uVar8 = source_palette[iVar9 / 4];
                uVar3 = (tU16)(uVar8 >> 9) & 0x7c00;
                uVar6 = (tU16)(uVar8 >> 6) & 0x3e0;
                goto LAB_005180d2;
            case 5:
                uVar8 = source_palette[iVar9 / 4];
                uVar3 = (tU16)(uVar8 >> 8) & 0xf800;
                uVar6 = (tU16)(uVar8 >> 5) & 0x7e0;
LAB_005180d2:
                uVar3 = uVar3 | uVar6 | (tU16)(uVar8 >> 3) & 0x1f;
                break;
            default:
                BrFailure("Unsupported back buffer type.");
                uVar3 = 0;
            }
            *puVar7 = uVar3;
            iVar9 += 4;
            puVar7++;
        } while (iVar9 < 0x400);
        gPalette_changed = 1;
        gPalette_source = (br_pixelmap*)iVar1;
    }
    iVar1 = *(int*)((tU8*)g16bit_palette + 8);
    extraout_EAX = BrMemAllocate((((pPixelmap->width + 0x3d) / 62) * 125 + 2), 0xfd);
    extraout_EAX_00 = BrMemAllocate((((pPixelmap->width + 0x3d) / 62) * 125 + 2) * pPixelmap->height, 0xfd);
    uVar8 = 2;
    *(tU16*)extraout_EAX_00 = pPixelmap->height;
    local_c = 2;
    local_14 = 0;
    if (pPixelmap->height != 0) {
        do {
            iVar9 = local_14 * pPixelmap->row_bytes;
            pbVar4 = (tU8*)(iVar9 + (int)pPixelmap->pixels);
            uVar8 = 2;
            bVar14 = *(char*)(iVar9 + (int)pPixelmap->pixels) == '\0';
            iVar9 = 0;
            cVar2 = '\0';
            local_24 = 0;
            pcVar12 = extraout_EAX + 1;
            while (1) {
                pcVar11 = pcVar12++;
                iVar9 = 0;
                while (iVar9 < 0x3e && local_24 != pPixelmap->width &&
                       ((local_10 = *pbVar4, (local_10 == 0) == bVar14))) {
                    if (!bVar14) {
                        uVar8 += 2;
                        *(tU16*)pcVar12 = *(tU16*)(iVar1 + (tU32)local_10 * 2);
                        pcVar12 += 2;
                    }
                    iVar9++;
                    local_24++;
                    pbVar4++;
                }
                if (!bVar14) {
                    *pcVar11 = (char)(iVar9 * -2);
                } else {
                    *pcVar11 = (char)(iVar9 << 1);
                }
                bVar14 = local_10 == 0;
                iVar9 = 0;
                cVar2++;
                if (local_24 == pPixelmap->width) {
                    break;
                }
                uVar8++;
            }
            *extraout_EAX = cVar2;
            memcpy((char*)extraout_EAX_00 + local_c, extraout_EAX, uVar8);
            uVar8 = local_c + uVar8;
            local_14++;
            local_c = uVar8;
        } while (local_14 < pPixelmap->height);
    }
    extraout_EAX_01 = BrMemAllocate(uVar8, 0xfd);
    memcpy(extraout_EAX_01, extraout_EAX_00, uVar8);
    BrMemFree(extraout_EAX_00);
    return (tU8*)extraout_EAX_01;
#else
    __asm {
            sub esp, 0x24
            mov eax, dword ptr [g16bit_palette]
            push ebx
            push ebp
            mov ebp, dword ptr [gRender_palette]
            push esi
            push edi
            mov edi, ecx
            test eax, eax
            mov dword ptr [esp + 0x14], edi
            jne L0051805c
            push eax
            push eax
            push 0x100
            push 1
            push 5
            call BrPixelmapAllocate
            add esp, 0x14
            mov dword ptr [g16bit_palette], eax
            test eax, eax
            jne L0051805c
            push offset gVector3_00661678+0xc8
            push 0x5e
            call FatalError
            mov eax, dword ptr [g16bit_palette]
            add esp, 8
    L0051805c:
            mov ecx, dword ptr [gPalette_changed]
            test ecx, ecx
            je L00518072
            cmp dword ptr [gPalette_source], ebp
            je L00518102
    L00518072:
            mov eax, dword ptr [eax + 8]
            xor esi, esi
            mov ebx, eax
    L00518079:
            mov edx, dword ptr [gBack_screen]
            mov eax, dword ptr [ebp + 8]
            xor ecx, ecx
            mov cl, byte ptr [edx + 0x2c]
            sub ecx, 4
            je L005180ba
            dec ecx
            je L005180a0
            push offset gVector3_00661678+0xa8
            call BrFailure
            add esp, 4
            xor eax, eax
            jmp L005180dc
    L005180a0:
            mov ecx, dword ptr [eax + esi]
            mov eax, ecx
            mov edx, ecx
            shr eax, 8
            shr edx, 5
            and eax, 0xf800
            and edx, 0x7e0
            jmp L005180d2
    L005180ba:
            mov ecx, dword ptr [eax + esi]
            mov eax, ecx
            mov edx, ecx
            shr eax, 9
            shr edx, 6
            and eax, 0x7c00
            and edx, 0x3e0
    L005180d2:
            shr ecx, 3
            or eax, edx
            and ecx, 0x1f
            or eax, ecx
    L005180dc:
            mov word ptr [ebx], ax
            add esi, 4
            add ebx, 2
            cmp esi, 0x400
            jl L00518079
            mov eax, dword ptr [g16bit_palette]
            mov dword ptr [gPalette_changed], 1
            mov dword ptr [gPalette_source], ebp
    L00518102:
            mov eax, dword ptr [eax + 8]
            xor ecx, ecx
            mov cx, word ptr [edi + 0x34]
            mov dword ptr [esp + 0x30], eax
            add ecx, 0x3d
            mov eax, 0x84210843
            imul ecx
            add edx, ecx
            push 0xfd
            sar edx, 5
            mov ecx, edx
            shr ecx, 0x1f
            add edx, ecx
            lea eax, [edx + edx*4]
            lea esi, [eax + eax*4]
            lea esi, [esi + esi*4 + 2]
            push esi
            call BrMemAllocate
            xor edx, edx
            add esp, 8
            mov dx, word ptr [edi + 0x36]
            mov dword ptr [esp + 0x1c], eax
            imul esi, edx
            push 0xfd
            push esi
            call BrMemAllocate
            mov ebp, eax
            mov ax, word ptr [edi + 0x36]
            add esp, 8
            mov ebx, 2
            mov word ptr [ebp], ax
            cmp word ptr [edi + 0x36], 0
            mov dword ptr [esp + 0x2c], ebp
            mov dword ptr [esp + 0x28], ebx
            mov dword ptr [esp + 0x20], 0
            jbe L005182a6
            jmp L00518188
    L00518184:
            mov edi, dword ptr [esp + 0x14]
    L00518188:
            movsx edx, word ptr [edi + 0x28]
            mov ecx, dword ptr [esp + 0x20]
            mov esi, dword ptr [edi + 8]
            imul ecx, edx
            mov dl, byte ptr [ecx + esi]
            add ecx, esi
            xor eax, eax
            mov ebp, 2
            test dl, dl
            sete al
            mov esi, eax
            mov eax, dword ptr [esp + 0x1c]
            xor edx, edx
            mov dword ptr [esp + 0x18], edx
            mov dword ptr [esp + 0x10], edx
            lea edi, [eax + 2]
    L005181ba:
            cmp edx, 0x3e
            jge L0051820e
            mov ebx, dword ptr [esp + 0x14]
            xor eax, eax
            mov ax, word ptr [ebx + 0x34]
            mov ebx, dword ptr [esp + 0x10]
            cmp ebx, eax
            je L0051820e
            mov al, byte ptr [ecx]
            xor ebx, ebx
            test al, al
            sete bl
            cmp ebx, esi
            mov byte ptr [esp + 0x24], al
            jne L0051820e
            test esi, esi
            jne L00518201
            mov eax, dword ptr [esp + 0x24]
            mov ebx, dword ptr [esp + 0x30]
            and eax, 0xff
            add ebp, 2
            add edi, 2
            mov ax, word ptr [ebx + eax*2]
            mov word ptr [edi - 2], ax
    L00518201:
            mov eax, dword ptr [esp + 0x10]
            inc ecx
            inc edx
            inc eax
            mov dword ptr [esp + 0x10], eax
            jmp L005181ba
    L0051820e:
            test esi, esi
            je L00518219
            shl dl, 1
            mov byte ptr [edi - 1], dl
            jmp L0051822b
    L00518219:
            mov al, dl
            mov bl, 0xfe
            mov esi, dword ptr [esp + 0x1c]
            imul bl
            add edx, edx
            sub esi, edx
            mov byte ptr [esi + ebp - 1], al
    L0051822b:
            mov dl, byte ptr [esp + 0x24]
            xor eax, eax
            test dl, dl
            mov ebx, dword ptr [esp + 0x14]
            sete al
            mov esi, eax
            mov eax, dword ptr [esp + 0x18]
            xor edx, edx
            inc eax
            mov dword ptr [esp + 0x18], eax
            xor eax, eax
            mov ax, word ptr [ebx + 0x34]
            mov ebx, dword ptr [esp + 0x10]
            cmp ebx, eax
            je L0051825c
            inc ebp
            inc edi
            jmp L005181ba
    L0051825c:
            mov esi, dword ptr [esp + 0x1c]
            mov cl, byte ptr [esp + 0x18]
            mov ebx, dword ptr [esp + 0x28]
            mov edx, dword ptr [esp + 0x2c]
            mov byte ptr [esi], cl
            mov ecx, ebp
            mov eax, ecx
            lea edi, [edx + ebx]
            shr ecx, 2
            rep movsd
            mov edx, dword ptr [esp + 0x14]
            mov ecx, eax
            mov eax, dword ptr [esp + 0x20]
            and ecx, 3
            rep movsb
            xor ecx, ecx
            add ebx, ebp
            mov cx, word ptr [edx + 0x36]
            inc eax
            cmp eax, ecx
            mov dword ptr [esp + 0x28], ebx
            mov dword ptr [esp + 0x20], eax
            jl L00518184
            mov ebp, dword ptr [esp + 0x2c]
    L005182a6:
            push 0xfd
            push ebx
            call BrMemAllocate
            mov ecx, ebx
            mov dword ptr [esp + 0x38], eax
            mov edi, eax
            mov eax, ecx
            mov esi, ebp
            add esp, 8
            shr ecx, 2
            rep movsd
            mov ecx, eax
            push ebp
            and ecx, 3
            rep movsb
            call BrMemFree
            mov eax, dword ptr [esp + 0x34]
            add esp, 4
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x24
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
            nop
            nop
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x005182f0
void C2_HOOK_FASTCALL GlorifyMaterial(br_material** pMaterials, int pCount, tRendererShadingType pShading) {
    int i;
    tMaterial_exception *material_exception;

    for (i = 0; i < pCount; i++) {

        if (pMaterials[i]->colour_map != NULL) {
            material_exception = FindExceptionInList(pMaterials[i]->colour_map->identifier, gMaterial_exceptions);
            if (gEnable_texture_interpolation && (material_exception == NULL || !(material_exception->flags & 0x1))) {
                pMaterials[i]->flags |= BR_MATF_MAP_INTERPOLATION;
            }
            if (gEnable_texture_interpolation && material_exception != NULL && material_exception->flags & 0x8) {
                pMaterials[i]->map_transform.m[2][0] = 0.02f;
                pMaterials[i]->map_transform.m[2][1] = 0.02f;
            }
            if (gEnable_texture_antialiasing) {
                pMaterials[i]->flags |= BR_MATF_MAP_ANTIALIASING;
            }
            if (gEnable_perspective_maps) {
                pMaterials[i]->flags |= BR_MATF_PERSPECTIVE;
            }
        }
        switch (pShading) {
        case kRendererShadingType_AmbientOnly:
            pMaterials[i]->ka = 1.0f;
            pMaterials[i]->kd = 0.0f;
            pMaterials[i]->ks = 0.0f;
            pMaterials[i]->flags &= ~BR_MATF_PRELIT;
            pMaterials[i]->flags |= BR_MATF_LIGHT;
            pMaterials[i]->flags |= BR_MATF_SMOOTH;
            break;
        case kRendererShadingType_Diffuse1:
            pMaterials[i]->ka = 0.4f;
            pMaterials[i]->kd = 0.8f;
            pMaterials[i]->ks = 0.0f;
            pMaterials[i]->flags &= ~BR_MATF_PRELIT;
            pMaterials[i]->flags |= BR_MATF_LIGHT;
            pMaterials[i]->flags |= BR_MATF_SMOOTH;
            break;
        case kRendererShadingType_Specular:
            pMaterials[i]->ka = 0.6f;
            pMaterials[i]->kd = 0.2f;
            pMaterials[i]->ks = 0.8f;
            pMaterials[i]->flags &= ~BR_MATF_PRELIT;
            pMaterials[i]->flags |= BR_MATF_LIGHT;
            pMaterials[i]->flags |= BR_MATF_SMOOTH;
            break;
        case kRendererShadingType_Default:
            pMaterials[i]->ka = 0.2f;
            pMaterials[i]->kd = 0.8f;
            pMaterials[i]->ks = 0.0f;
            pMaterials[i]->flags &= ~BR_MATF_PRELIT;
            pMaterials[i]->flags |= BR_MATF_LIGHT;
            pMaterials[i]->flags |= BR_MATF_SMOOTH;
            break;
        default:
            pMaterials[i]->ka = 1.0f;
            pMaterials[i]->kd = 0.0f;
            pMaterials[i]->ks = 0.0f;
            break;
        }
    }
}

// FUNCTION: CARMA2_HW 0x005184c0
int C2_HOOK_FASTCALL FindBestColourMatch(br_colour pColour) {
    tRGB_colour rgb;

    rgb.red = BR_RED(pColour);
    rgb.green = BR_GRN(pColour);
    rgb.blue = BR_BLU(pColour);
    return FindBestMatch(&rgb, gRender_palette);
}

// FUNCTION: CARMA2_HW 0x00518690
void C2_HOOK_FASTCALL WhitenVertexRGB(br_model** pModels, int pCount) {
    int i;
    br_vertex* vertex;

    if (gScreen == NULL) {
        return;
    }
    if (gScreen->type == BR_PMT_INDEX_8) {
        return;
    }
    for (i = 0; i < pCount; i++) {
        int j;

        for (j = 0; j < pModels[i]->nvertices; j++) {
            vertex = &pModels[i]->vertices[j];

            vertex->red = 0xff;
            vertex->grn = 0xff;
            vertex->blu = 0xff;
        }
    }
}

#ifdef CARPOCALYPSE2_MATCHING
// FUNCTION: CARMA2_HW 0x00518700
void C2_NAKED C2_HOOK_FASTCALL BRPM_convert(br_pixelmap* pMap, int pPixel_type) {
    __asm {
            push ebp
            mov ebp, esp
            sub esp, 0x14
            test ecx, ecx
            mov dword ptr [ebp - 0x14], ecx
            je L00518776
            cmp edx, 4
            jne L00518776
            cmp byte ptr [ecx + 0x2c], 5
            jne L00518776
            mov eax, dword ptr [ecx + 8]
            test eax, eax
            je L00518776
            mov dword ptr [ebp - 0xc], eax
            xor edx, edx
            movsx eax, word ptr [ecx + 0x28]
            mov dx, word ptr [ecx + 0x34]
            mov dword ptr [ebp - 8], eax
            xor eax, eax
            push edi
            mov ax, word ptr [ecx + 0x36]
            push esi
            push ebx
            mov dword ptr [ebp - 4], edx
            mov dword ptr [ebp - 0x10], eax
            mov ecx, dword ptr [ebp - 0x10]
            mov esi, dword ptr [ebp - 0xc]
            mov edx, dword ptr [ebp - 8]
            mov edi, dword ptr [ebp - 4]
    L0051874a:
            push ecx
            mov ecx, edi
            push esi
    L0051874e:
            mov eax, dword ptr [esi]
            mov ebx, eax
            and eax, 0xffc0
            and ebx, 0x1f
            shr eax, 1
            or eax, ebx
            mov word ptr [esi], ax
            add esi, 2
            loop L0051874e
            pop esi
            add esi, edx
            pop ecx
            loop L0051874a
            mov ecx, dword ptr [ebp - 0x14]
            pop ebx
            pop esi
            pop edi
            mov byte ptr [ecx + 0x2c], 4
    L00518776:
            mov esp, ebp
            pop ebp
            ret
            nop
            nop
            nop
            nop
            nop
            nop
    }
}
#else
void C2_HOOK_FASTCALL BRPM_convert(br_pixelmap* pMap, int pPixel_type) {
    br_uint_8* row_pointer;
    br_uint_16* pixel;
    br_uint_16 row_stride;
    br_uint_32 original_w;
    br_uint_32 w;
    br_uint_32 h;

    if (pMap != NULL && pPixel_type == BR_PMT_RGB_555 && pMap->type == BR_PMT_RGB_565 && pMap->pixels != NULL) {
        row_pointer = pMap->pixels;
        pixel = pMap->pixels;
        row_stride = pMap->row_bytes;
        original_w = pMap->width;
        h = pMap->height;
        w = pMap->width;

        while (h-- != 0) {
            w = original_w;
            pixel = (br_uint_16*)row_pointer;
            while (w-- != 0) {
                // RGB565 -> XRGB1555: remove least significant bit of green (6 bits -> 5 bits)
                *pixel = ((*pixel & 0xffc0)>>1) | (*pixel & 0x001f);
                pixel++;
            }
            row_pointer += row_stride;
        }
        pMap->type = BR_PMT_RGB_555;
    }
}
#endif

// FUNCTION: CARMA2_HW 0x00518780
void C2_NAKED C2_HOOK_FASTCALL PrintScreen(void) {
#ifndef CARPOCALYPSE2_MATCHING
    FILE* f;

    f = OpenUniqueFileB("DUMP", "BMP");
    if (f != NULL) {
        if (gBack_screen->type == BR_PMT_INDEX_8) {
            PrintScreenFile(f);
        } else {
            PrintScreenFile16(f);
        }
        PFfclose(f);
    }
#else
    __asm {
            sub esp, 0x104
            push ebx
            push ebp
            push esi
            push edi
            xor ebx, ebx
    L0051878c:
            mov edi, offset gApplication_path
            or ecx, 0xffffffff
            xor eax, eax
            lea edx, [esp + 0x14]
            repne scasb
            not ecx
            sub edi, ecx
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            shr ecx, 2
            rep movsd
            mov ecx, eax
            xor eax, eax
            and ecx, 3
            rep movsb
            mov edi, offset gVector3_00661678+0x100
            or ecx, 0xffffffff
            repne scasb
            not ecx
            dec ecx
            je L0051881c
            mov edi, dword ptr [gDir_separator]
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            lea edx, [esp + 0x14]
            mov esi, edi
            mov ebp, ecx
            mov edi, edx
            or ecx, 0xffffffff
            repne scasb
            mov ecx, ebp
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, ebp
            lea edx, [esp + 0x14]
            and ecx, 3
            rep movsb
            mov edi, offset gVector3_00661678+0x100
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            mov esi, edi
            mov ebp, ecx
            mov edi, edx
            or ecx, 0xffffffff
            repne scasb
            mov ecx, ebp
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, ebp
            and ecx, 3
            rep movsb
    L0051881c:
            lea edi, [esp + 0x14]
            or ecx, 0xffffffff
            xor eax, eax
            push offset gVector3_00661678+0xfc
            repne scasb
            not ecx
            dec ecx
            push ebx
            push offset gVector3_00661678+0x58
            lea eax, [esp + ecx + 0x20]
            push eax
            call sprintf
            add esp, 0x10
            mov edx, offset gNext_crush_actor_id+0x180
            lea ecx, [esp + 0x14]
            call DRfopen
            test eax, eax
            je L00518a9d
            mov ecx, eax
            call PFfclose
            inc ebx
            cmp ebx, 0x2710
            jl L0051878c
            xor esi, esi
    L0051886e:
            test esi, esi
            je L00518a92
            mov ecx, dword ptr [gBack_screen]
            cmp byte ptr [ecx + 0x2c], 3
            je L00518a84
            mov eax, dword ptr [ecx + 8]
            test eax, eax
            jne L0051889e
            push 1
            push ecx
            call BrPixelmapDirectLock
            mov ecx, dword ptr [gBack_screen]
            add esp, 8
    L0051889e:
            xor eax, eax
            xor edx, edx
            mov ax, word ptr [ecx + 0x34]
            mov dx, word ptr [ecx + 0x36]
            imul eax, edx
            mov dl, 0x42
            mov ecx, esi
            lea edi, [eax + eax*2]
            call WriteU8L
            mov dl, 0x4d
            mov ecx, esi
            call WriteU8L
            lea edx, [edi + 0x36]
            mov ecx, esi
            call WriteU32L
            xor edx, edx
            mov ecx, esi
            call WriteU16L
            xor edx, edx
            mov ecx, esi
            call WriteU16L
            mov edx, 0x36
            mov ecx, esi
            call WriteU32L
            mov edx, 0x28
            mov ecx, esi
            call WriteU32L
            mov eax, dword ptr [gBack_screen]
            xor edx, edx
            mov ecx, esi
            mov dx, word ptr [eax + 0x34]
            call WriteU32L
            mov ecx, dword ptr [gBack_screen]
            xor edx, edx
            mov dx, word ptr [ecx + 0x36]
            mov ecx, esi
            call WriteU32L
            mov edx, 1
            mov ecx, esi
            call WriteU16L
            mov edx, 0x18
            mov ecx, esi
            call WriteU16L
            xor edx, edx
            mov ecx, esi
            call WriteU32L
            xor edx, edx
            mov ecx, esi
            call WriteU32L
            xor edx, edx
            mov ecx, esi
            call WriteU32L
            xor edx, edx
            mov ecx, esi
            call WriteU32L
            xor edx, edx
            mov ecx, esi
            call WriteU32L
            mov edx, 0x100
            mov ecx, esi
            call WriteU32L
            mov eax, dword ptr [gBack_screen]
            xor ecx, ecx
            mov dword ptr [esp + 0x10], 0
            mov cx, word ptr [eax + 0x36]
            mov ebx, dword ptr [eax + 8]
            movsx edi, word ptr [eax + 0x28]
            lea edx, [ecx - 1]
            imul edi, edx
            add edi, ebx
            test ecx, ecx
            jle L00518a54
    L00518996:
            xor ebp, ebp
            cmp word ptr [eax + 0x34], bp
            jbe L00518a2b
    L005189a2:
            mov al, byte ptr [eax + 0x2c]
            mov bx, word ptr [edi]
            cmp al, 5
            jne L005189d0
            mov dl, bl
            mov ecx, esi
            shl dl, 3
            call WriteU8L
            mov edx, ebx
            mov ecx, esi
            shr edx, 3
            and dl, 0xf8
            call WriteU8L
            xor edx, edx
            mov dl, bh
            and dl, 0xf8
            jmp L00518a0d
    L005189d0:
            cmp al, 4
            jne L005189f9
            mov dl, bl
            mov ecx, esi
            shl dl, 3
            call WriteU8L
            mov edx, ebx
            mov ecx, esi
            shr edx, 2
            and dl, 0xf8
            call WriteU8L
            mov edx, ebx
            shr edx, 7
            and dl, 0xf8
            jmp L00518a0d
    L005189f9:
            xor dl, dl
            mov ecx, esi
            call WriteU8L
            xor dl, dl
            mov ecx, esi
            call WriteU8L
            xor dl, dl
    L00518a0d:
            mov ecx, esi
            call WriteU8L
            mov eax, dword ptr [gBack_screen]
            xor ecx, ecx
            inc ebp
            add edi, 2
            mov cx, word ptr [eax + 0x34]
            cmp ebp, ecx
            jl L005189a2
    L00518a2b:
            movsx ecx, word ptr [eax + 0x28]
            xor edx, edx
            mov dx, word ptr [eax + 0x34]
            shl edx, 1
            neg ecx
            sub ecx, edx
            xor edx, edx
            mov dx, word ptr [eax + 0x36]
            add edi, ecx
            mov ecx, dword ptr [esp + 0x10]
            inc ecx
            cmp ecx, edx
            mov dword ptr [esp + 0x10], ecx
            jl L00518996
    L00518a54:
            xor edx, edx
            mov ecx, esi
            call WriteU16L
            mov eax, dword ptr [gBack_screen]
            mov ecx, dword ptr [eax + 8]
            test ecx, ecx
            je L00518a8b
            push eax
            call BrPixelmapDirectUnlock
            add esp, 4
            mov ecx, esi
            call PFfclose
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x104
            ret
    L00518a84:
            mov ecx, esi
            call PrintScreenFile
    L00518a8b:
            mov ecx, esi
            call PFfclose
    L00518a92:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x104
            ret
    L00518a9d:
            mov edx, offset gFrontend_QUIT+0xbcc4
            lea ecx, [esp + 0x14]
            call DRfopen
            mov esi, eax
            jmp L0051886e
    }
#endif
}


// FUNCTION: CARMA2_HW 0x00518d60
tU32 C2_HOOK_FASTCALL FudgeBRenderIntoTheNinetiesWithSomeProperFuckingColourSupport(br_pixelmap* pm, tU32 red, tU32 grn, tU32 blu, tU32 alp) {
    tU32 colour;

    switch (pm->type) {
    case BR_PMT_RGB_555:
        colour = red & 0xf8;
        break;
    case BR_PMT_RGB_565:
        return ((((red & 0xf8) << 5) | (grn & 0xfc)) << 3) | ((blu >> 3) & 0x1f);
    case BR_PMT_RGBA_8888:
        return (((((alp << 8) | red) << 8) | grn) << 8) | blu;
    case BR_PMT_RGBA_4444:
        /* The original masks AL only, retaining the upper alpha bits. */
        return ((((alp & 0xfffffff0u) << 4) | (red & 0xfffffff0u)) << 4) | (blu >> 4) | (grn & 0xfffffff0u);
    case BR_PMT_ARGB_1555:
        colour = ((alp & 0xffffff80u) << 1) | (red & 0xf8);
        break;
    default:
        return (((red << 8) | grn) << 8) | blu;
    }
    return (((colour << 5) | (grn & 0xf8)) << 2) | ((blu >> 3) & 0x1f);
}

// GetBlenficatiousnessOfMaterialTablishly

// GetBlenficatiousnessOfMaterialPrimitively

// GetBlenficatiousnessOfMaterial

// FUNCTION: CARMA2_HW 0x00518f20
char* C2_HOOK_FASTCALL MungeCommas(int pValue) {
    // GLOBAL: CARMA2_HW 0x006b5f20
    static char result_buffer[32];
    char buffer[32];
    int len;
    int get_pos;
    int put_pos;
    int remaining;

    sprintf(buffer, "%i", pValue);
    len = (int)strlen(buffer);
    for (get_pos = 0, put_pos = 0, remaining = len; get_pos < len; get_pos++, put_pos++, remaining--) {

        if (remaining % 3 == 0 && get_pos != 0) {
            result_buffer[put_pos] = gMisc_strings[294][0];
            put_pos++;
        }
        result_buffer[put_pos] = buffer[get_pos];
    }
    result_buffer[put_pos] = '\0';
    return result_buffer;
}

// FUNCTION: CARMA2_HW 0x00518fa0
void C2_NAKED C2_HOOK_FASTCALL MungeMetaCharacters(char* pText, char pMeta, const char* pRepl) {
#ifndef CARPOCALYPSE2_MATCHING
    int len_text;
    int len_repl;
    int i;

    len_text = strlen(pText);
    len_repl = strlen(pRepl);

    for (i = 0; i < len_text; i++) {
        if (pText[i] == '@' && pText[i + 1] == pMeta) {
            memmove(&pText[i + len_repl], &pText[i + 2], len_text - i - 1);
            memcpy(&pText[i], pRepl, len_repl);
            i += len_repl;
            len_text += len_repl - 2;
        }
    }
#else
    __asm {
            sub esp, 0xc
            push ebx
            mov byte ptr [esp + 7], dl
            push ebp
            push esi
            mov edx, ecx
            push edi
            mov edi, edx
            or ecx, 0xffffffff
            xor eax, eax
            xor ebp, ebp
            repne scasb
            mov edi, dword ptr [esp + 0x20]
            mov dword ptr [esp + 0x18], edx
            not ecx
            dec ecx
            mov esi, ecx
            or ecx, 0xffffffff
            repne scasb
            not ecx
            dec ecx
            mov dword ptr [esp + 0x14], esi
            test esi, esi
            mov ebx, ecx
            jle L00519033
    L00518fd7:
            mov al, byte ptr [ebp + edx]
            lea edi, [ebp + edx]
            cmp al, 0x40
            jne L0051902e
            mov al, byte ptr [esp + 0x13]
            mov cl, byte ptr [ebp + edx + 1]
            cmp cl, al
            jne L0051902e
            sub esi, ebp
            lea eax, [ebx + ebp]
            dec esi
            lea ecx, [ebp + edx + 2]
            push esi
            add eax, edx
            push ecx
            push eax
            call memmove
            mov esi, dword ptr [esp + 0x2c]
            mov ecx, ebx
            mov edx, ecx
            mov eax, dword ptr [esp + 0x20]
            shr ecx, 2
            rep movsd
            mov ecx, edx
            mov edx, dword ptr [esp + 0x24]
            and ecx, 3
            add esp, 0xc
            rep movsb
            lea ecx, [eax + ebx - 2]
            add ebp, ebx
            mov dword ptr [esp + 0x14], ecx
            mov esi, ecx
    L0051902e:
            inc ebp
            cmp ebp, esi
            jl L00518fd7
    L00519033:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0xc
            ret 4
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00519040
void C2_NAKED C2_HOOK_FASTCALL MungeMetaCharactersChar(char* pText, char pMeta, char pChar) {
#ifndef CARPOCALYPSE2_MATCHING
    char repl[2];

    repl[1] = '\0';
    repl[0] = pChar;
    MungeMetaCharacters(pText, pMeta, repl);
#else
    __asm {
            sub esp, 0xc
            mov al, byte ptr [esp + 0x10]
            push ebx
            push ebp
            push esi
            mov ebx, ecx
            push edi
            mov byte ptr [esp + 0x20], al
            mov edi, ebx
            or ecx, 0xffffffff
            xor eax, eax
            repne scasb
            not ecx
            mov byte ptr [esp + 0x13], dl
            dec ecx
            mov edx, ecx
            lea edi, [esp + 0x20]
            or ecx, 0xffffffff
            mov byte ptr [esp + 0x21], 0
            repne scasb
            not ecx
            dec ecx
            xor ebp, ebp
            mov esi, ecx
            mov dword ptr [esp + 0x14], edx
            test edx, edx
            mov dword ptr [esp + 0x18], esi
            jle L005190e0
    L00519084:
            mov al, byte ptr [ebp + ebx]
            lea edi, [ebp + ebx]
            cmp al, 0x40
            jne L005190db
            mov cl, byte ptr [esp + 0x13]
            mov al, byte ptr [ebp + ebx + 1]
            cmp al, cl
            jne L005190db
            sub edx, ebp
            lea eax, [esi + ebp]
            dec edx
            add eax, ebx
            push edx
            lea edx, [ebp + ebx + 2]
            push edx
            push eax
            call memmove
            mov ecx, esi
            lea esi, [esp + 0x2c]
            mov edx, ecx
            mov eax, dword ptr [esp + 0x24]
            shr ecx, 2
            rep movsd
            mov ecx, edx
            add esp, 0xc
            and ecx, 3
            add ebp, eax
            rep movsb
            mov ecx, dword ptr [esp + 0x14]
            mov esi, eax
            lea edx, [ecx + eax - 2]
            mov dword ptr [esp + 0x14], edx
    L005190db:
            inc ebp
            cmp ebp, edx
            jl L00519084
    L005190e0:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0xc
            ret 4
            nop
            nop
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x005190f0
void C2_NAKED C2_HOOK_FASTCALL MungeMetaCharactersNum(char* pText, char pMeta, int pNum) {
#ifndef CARPOCALYPSE2_MATCHING
    char text[16];

    sprintf(text, "%d", pNum);
    MungeMetaCharacters(pText, pMeta, text);
#else
    __asm {
            sub esp, 0x18
            mov eax, dword ptr [esp + 0x1c]
            push ebx
            push ebp
            push esi
            mov ebx, ecx
            push edi
            push eax
            lea ecx, [esp + 0x1c]
            push offset gPed_flags_0058f2e4+0x134
            mov byte ptr [esp + 0x1b], dl
            push ecx
            call sprintf
            mov edi, ebx
            or ecx, 0xffffffff
            xor eax, eax
            add esp, 0xc
            repne scasb
            not ecx
            dec ecx
            lea edi, [esp + 0x18]
            mov edx, ecx
            or ecx, 0xffffffff
            repne scasb
            not ecx
            dec ecx
            xor ebp, ebp
            mov esi, ecx
            mov dword ptr [esp + 0x2c], edx
            test edx, edx
            mov dword ptr [esp + 0x14], esi
            jle L0051919a
    L0051913e:
            mov al, byte ptr [ebp + ebx]
            lea edi, [ebp + ebx]
            cmp al, 0x40
            jne L00519195
            mov al, byte ptr [esp + 0x13]
            mov cl, byte ptr [ebp + ebx + 1]
            cmp cl, al
            jne L00519195
            sub edx, ebp
            lea ecx, [ebp + ebx + 2]
            dec edx
            push edx
            lea edx, [esi + ebp]
            add edx, ebx
            push ecx
            push edx
            call memmove
            mov ecx, esi
            lea esi, [esp + 0x24]
            mov eax, ecx
            add esp, 0xc
            shr ecx, 2
            rep movsd
            mov ecx, eax
            mov eax, dword ptr [esp + 0x14]
            and ecx, 3
            add ebp, eax
            rep movsb
            mov ecx, dword ptr [esp + 0x2c]
            mov esi, eax
            lea edx, [ecx + eax - 2]
            mov dword ptr [esp + 0x2c], edx
    L00519195:
            inc ebp
            cmp ebp, edx
            jl L0051913e
    L0051919a:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x18
            ret 4
    }
#endif
}

void C2_HOOK_FASTCALL DrPixelmapRectangleCopyPossibleLock(br_pixelmap* dst, br_int_32 dx, br_int_32 dy, br_pixelmap* src, br_int_32 sx, br_int_32 sy, br_int_32 w, br_int_32 h) {

    if (gLock_often) {
        PossibleUnlock(0);
    }
    BrPixelmapRectangleCopy(dst, dx, dy, src, sx, sy, w, h);
    if (gLock_often) {
        PossibleUnlock(0);
    }
}

// FUNCTION: CARMA2_HW 0x005191b0
int C2_HOOK_FASTCALL FRONTEND_Redraw(br_pixelmap* dst, br_int_16 dx, br_int_16 dy, br_pixelmap* src, br_int_16 sx, br_int_16 sy, br_int_16 w, br_int_16 h) {
    BrPixelmapRectangleCopy(dst, dx, dy, src, sx, sy, w, h);
}

// FUNCTION: CARMA2_HW 0x005191f0
void C2_HOOK_FASTCALL PixelmapSwapByteOrder(br_pixelmap* pMap) {
#if 0 // FIXME: introduce endian.h-like carpocalypse2_endian.h header
    br_uint_16 y;
    br_uint_8* row_ptr;
    br_colour* ptr;
    int w;

    row_ptr = pMap->pixels;
    for (y = 0; y < pMap->height; y++) {
        w = (pMap->width * 2) / sizeof(br_colour);
        ptr = (br_colour*) row_ptr;
        while (w) {
            *ptr = (*ptr >> 0x18) + (*ptr << 0x18) + ((*ptr >> 0x8) & 0xff00) + ((*ptr & 0xff00) <<0x8);
            ptr++;
        }
        row_ptr += pMap->row_bytes;
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00519200
void C2_NAKED C2_HOOK_FASTCALL ConvertPixelmapType(br_pixelmap* pMap, int pPixel_type) {
#ifndef CARPOCALYPSE2_MATCHING
    void (C2_HOOK_FASTCALL *convert_pixel)(br_uint_16*);
    br_uint_16* row;
    int x;
    br_uint_16* pixel;
    int y;

    convert_pixel = NULL;
    if (pMap->type != BR_PMT_RGB_565) {
        if (pMap->type == BR_PMT_RGBA_4444 && pPixel_type == BR_PMT_ARGB_1555) {
            convert_pixel = (void (C2_HOOK_FASTCALL *)(br_uint_16*))FUN_005192c0;
        }
    } else {
        if (pPixel_type == BR_PMT_RGB_555) {
            convert_pixel = FUN_005192a0;
        }
    }
    if (convert_pixel != NULL) {
        row = (br_uint_16*)pMap->pixels;
        y = 0;
        if (pMap->height > 0) {
            do {
                x = 0;
                pixel = row;
                if (pMap->width > 0) {
                    do {
                        convert_pixel(pixel);
                        x++;
                        pixel++;
                    } while (x < pMap->width);
                }
                row += pMap->row_bytes / 2;
                y++;
            } while (y < pMap->height);
        }
        pMap->type = (br_uint_8)pPixel_type;
    }
#else
    __asm {
            sub esp, 0xc
            push ebx
            push ebp
            mov ebp, ecx
            xor ebx, ebx
            push esi
            push edi
            mov al, byte ptr [ebp + 0x2c]
            mov dword ptr [esp + 0x18], edx
            cmp al, 5
            je L00519226
            cmp al, 0x12
            jne L00519230
            cmp edx, 0x1f
            jne L00519230
            mov ebx, offset FUN_005192c0
            jmp L00519230
    L00519226:
            cmp edx, 4
            jne L00519230
            mov ebx, offset FUN_005192a0
    L00519230:
            test ebx, ebx
            je L00519296
            mov eax, dword ptr [ebp + 8]
            cmp word ptr [ebp + 0x36], 0
            mov dword ptr [esp + 0x10], eax
            mov dword ptr [esp + 0x14], 0
            jbe L00519293
    L0051924a:
            mov edi, dword ptr [esp + 0x10]
            xor esi, esi
            cmp word ptr [ebp + 0x34], si
            jbe L00519268
    L00519256:
            mov ecx, edi
            call ebx
            xor ecx, ecx
            inc esi
            mov cx, word ptr [ebp + 0x34]
            add edi, 2
            cmp esi, ecx
            jl L00519256
    L00519268:
            movsx eax, word ptr [ebp + 0x28]
            cdq
            mov ecx, dword ptr [esp + 0x14]
            sub eax, edx
            mov edx, dword ptr [esp + 0x10]
            inc ecx
            sar eax, 1
            mov dword ptr [esp + 0x14], ecx
            lea eax, [edx + eax*2]
            xor edx, edx
            mov dx, word ptr [ebp + 0x36]
            mov dword ptr [esp + 0x10], eax
            cmp ecx, edx
            jl L0051924a
            mov edx, dword ptr [esp + 0x18]
    L00519293:
            mov byte ptr [ebp + 0x2c], dl
    L00519296:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0xc
            ret
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x005193f0
void C2_HOOK_FASTCALL EnsurePixelmapAllowed(br_pixelmap* pMap, undefined4 pArg2) {
}

// ===== Ported from bojan1986819 fork =====

// FUNCTION: CARMA2_HW 0x00513720
int C2_HOOK_FASTCALL FUN_00513720(int pChance, float pScale) {
    int scratch = pChance;
    int random_value = rand();
    float chance = (float)scratch;

    scratch = random_value;
    if ((float)scratch * gFLOAT_0058b26c <= chance * pScale * gDOUBLE_0058b270) {
        return 1;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x005137c0
int C2_HOOK_FASTCALL FUN_005137c0(int pArg) {
    return pArg != 0;
}

// FUNCTION: CARMA2_HW 0x00514950
float C2_HOOK_FASTCALL signf(float pValue) {
    volatile int* pResult;

    pResult = (volatile int*)&pValue;
    if (pValue > gFloatZero_0058b280) {
        *pResult = 1;
        return (float)*pResult;
    }

    if (pValue < gFloatZero_0058b280) {
        *pResult = -1;
    } else {
        *pResult = 0;
    }
    return (float)*pResult;
}

// FUNCTION: CARMA2_HW 0x00514d80
void C2_HOOK_FASTCALL CopyMiscString(int pIndex, char* pDest) {
    strcpy(pDest, gMisc_strings[pIndex]);
}

// FUNCTION: CARMA2_HW 0x00514de0
void C2_HOOK_FASTCALL CopyBrMaterial(br_material* pDestination, const br_material* pSource) {
    pDestination->flags = pSource->flags;
    pDestination->ka = pSource->ka;
    pDestination->kd = pSource->kd;
    pDestination->ks = pSource->ks;
    pDestination->power = pSource->power;
    pDestination->colour = pSource->colour;
    pDestination->index_base = pSource->index_base;
    pDestination->index_range = pSource->index_range;
    pDestination->index_shade = pSource->index_shade;
    pDestination->colour_map = pSource->colour_map;
    pDestination->map_transform = pSource->map_transform;
    pDestination->identifier = pSource->identifier;
}

double C2_HOOK_FASTCALL RGBDifferenceSqr(tRGB_colour* pColour_1, tRGB_colour* pColour_2) {

    return CARPOCALYPSE2_SQR(pColour_1->red - pColour_2->red)
           + CARPOCALYPSE2_SQR(pColour_1->green - pColour_2->green)
           + CARPOCALYPSE2_SQR(pColour_1->blue - pColour_2->blue);
}

int C2_HOOK_FASTCALL FindBestMatch(tRGB_colour* pRGB_colour, br_pixelmap* pPalette) {
    int n;
    int near_c;
    double min_d;
    double d;
    tRGB_colour trial_RGB;
    br_colour* dp;

    near_c = 127;
    min_d = DBL_MAX;
    dp = pPalette->pixels;
    for (n = 0; n < 256; n++) {
        trial_RGB.red = (dp[n] >> 16) & 0xff;
        trial_RGB.green = (dp[n] >> 8) & 0xff;
        trial_RGB.blue = (dp[n] >> 0) & 0xff;
        d = RGBDifferenceSqr(pRGB_colour, &trial_RGB);
        if (d < min_d) {
            min_d = d;
            near_c = n;
        }
    }
    return near_c;
}

void C2_HOOK_FASTCALL BuildShadeTablePath(char* pThe_path, int pR, int pG, int pB) {
    char s[32];

    s[0] = 's';
    s[1] = 't';
    s[2] = 'A' + ((pR >> 4) % 0x10);
    s[3] = 'A' + ((pR >> 0) % 0x10);
    s[4] = 'A' + ((pG >> 4) % 0x10);
    s[5] = 'A' + ((pG >> 0) % 0x10);
    s[6] = 'A' + ((pB >> 4) % 0x10);
    s[7] = 'A' + ((pB >> 0) % 0x10);
    s[8] = '\0';
    strcat(s, ".TAB");
    PathCat(pThe_path, gApplication_path, "SHADETAB");
    PathCat(pThe_path, pThe_path, s);
}

br_pixelmap* C2_HOOK_FASTCALL LoadGeneratedShadeTable(int pR, int pG, int pB) {
    char the_path[256];

    BuildShadeTablePath(the_path, pR, pG, pB);
    return BrPixelmapLoad(the_path);
}

void C2_HOOK_FASTCALL SaveGeneratedShadeTable(br_pixelmap* pThe_table, int pR, int pG, int pB) {
    char the_path[256];

    BuildShadeTablePath(the_path, pR, pG, pB);
    BrPixelmapSave(the_path, pThe_table);
}

// FUNCTION: CARMA2_HW 0x005156d0
void C2_HOOK_FASTCALL DRPixelmapRectangleFill(br_pixelmap* dst, br_int_16 x, br_int_16 y, br_uint_16 w, br_uint_16 h, br_uint_32 colour) {
    BrPixelmapRectangleFill(dst, x, y, w, h, colour);
}

// FUNCTION: CARMA2_HW 0x00515e10
void C2_HOOK_FASTCALL GetBlendTableName(int pPercent, char* pDest) {

    sprintf(pDest, "BLEND%d.TAB", 100 - pPercent);
}

// FUNCTION: CARMA2_HW 0x00516110
int C2_HOOK_FASTCALL DRVector2TestForNan(const br_vector2* pV) {
    if (pV != NULL) {
        return TestForNan(&pV->v[0]) || TestForNan(&pV->v[1]);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x005161c0
int C2_HOOK_FASTCALL DRVector4TestForNan(const float* pV) {
    if (pV != NULL) {
        return TestForNan(&pV[0]) || TestForNan(&pV[1]) || TestForNan(&pV[2]) || TestForNan(&pV[3]);
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x005162d0
void C2_HOOK_FASTCALL FUN_005162d0(br_vector4* pDst, const br_vector4* pSrc) {
    pDst->v[0] = -pSrc->v[0];
    pDst->v[1] = -pSrc->v[1];
    pDst->v[2] = -pSrc->v[2];
    pDst->v[3] = -pSrc->v[3];
}

// FUNCTION: CARMA2_HW 0x005192a0
void C2_HOOK_FASTCALL FUN_005192a0(br_uint_16* pPixel) {
    *pPixel = ((*pPixel >> 1) & 0x7fe0) + (*pPixel & 0x1f);
}

// FUNCTION: CARMA2_HW 0x005192c0
void C2_HOOK_FASTCALL FUN_005192c0(tU16* pPixel) {
    tU16 pixel;
    tU16 converted;

    pixel = *pPixel;
    converted = ((pixel & 0xf) + ((pixel & 0xf0) + (pixel & 0xf00) * 2) * 2) * 2;
    *pPixel = converted;
    if ((pixel & 0xf000) != 0) {
        *pPixel = converted + 0x8000;
    }
}

// FUNCTION: CARMA2_HW 0x00519400
void C2_HOOK_FASTCALL FUN_00519400(br_pixelmap* pMap) {
    void (C2_HOOK_FASTCALL *convert_pixel)(br_uint_16*);
    br_uint_16* row;
    br_uint_16* pixel;
    int x;
    int y;

    switch (pMap->type) {
    case 4:
    case 0x1f:
        break;
    case 5:
        convert_pixel = NULL;
        if (pMap->type == 5) {
            convert_pixel = FUN_005192a0;
        }
        if (convert_pixel != NULL) {
            row = (br_uint_16*)pMap->pixels;
            y = 0;
            if (pMap->height > 0) {
                do {
                    x = 0;
                    pixel = row;
                    if (pMap->width > 0) {
                        do {
                            convert_pixel(pixel);
                            x++;
                            pixel++;
                        } while (x < pMap->width);
                    }
                    y++;
                    row += pMap->row_bytes / 2;
                } while (y < pMap->height);
            }
            pMap->type = 4;
        }
        break;
    default:
        return;
    }
    row = (br_uint_16*)pMap->pixels;
    y = 0;
    if (pMap->height > 0) {
        do {
            x = 0;
            pixel = row;
            if (pMap->width > 0) {
                do {
                    *pixel = (br_uint_16)((((*pixel & 0x7fff) != 0) << 15) | (*pixel & 0x7fff));
                    x++;
                    pixel++;
                } while (x < pMap->width);
            }
            y++;
            row += pMap->row_bytes / 2;
        } while (y < pMap->height);
    }
    pMap->type = 0x1f;
}

// FUNCTION: CARMA2_HW 0x0047d850
int C2_HOOK_FASTCALL DRstrcmpi(const char* str1, const char* str2) {
    return strcasecmp(str1, str2);
}

// FUNCTION: CARMA2_HW 0x0047d860
int C2_HOOK_FASTCALL DRmemicmp(const char* str1, const char* str2, size_t count) {
    return c2_memicmp(str1, str2, count);
}
