#include "lighting.h"

#include "world.h"
#include "world3.h"
#include "utility.h"
#include "errors.h"
#include "packfile.h"
#include "globvars.h"
#include "carpocalypse2_macros.h"

#include "carpocalypse2_types.h"
#include "brender/brender.h"

#include <string.h>
extern int gNbPixelBits;

// LoadTrackModels

// SmoothlyLoadIfItsAModel

// AddSmoothModels

// LoadTrackMaterials

// LoadCarMaterials

// SmoothlySetCarMaterialFlags

// SmoothlySetWorldMaterialFlags

// SmoothificatePowerups

// SmoothificatePowerupMaterial

// SmoothificateWorldMaterial

#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_CDECL BrMaterialUpdate();
extern void C2_HOOK_FASTCALL LoadIfItsAMaterial(const char* pPath);
extern void C2_HOOK_FASTCALL PFForEveryFile(const char* pThe_path, tPDForEveryFileRecurse_cbfn pAction_routine);
extern char gFloat_0058afb4[];
extern tLighting_data gLighting_data;
extern tRendererShadingType gMaterial_shading_for_callback;
extern tBrender_storage* gStorage_for_callbacks;
#endif
// FUNCTION: CARMA2_HW 0x004f6b50
void C2_HOOK_FASTCALL SmoothificateCarMaterial(br_material* pMaterial) {

  if (gNbPixelBits == 16) {
      pMaterial->ka = 0.6f;
      pMaterial->flags &= ~BR_MATF_PRELIT;
      pMaterial->flags |= BR_MATF_SMOOTH | BR_MATF_LIGHT;
      pMaterial->kd = 0.2f;
      BrMaterialUpdate(pMaterial,BR_MATU_ALL);
  }
}

// MungeLightingEffects

// FUNCTION: CARMA2_HW 0x004f6a20
void C2_NAKED C2_HOOK_FASTCALL SmoothificatePowerupMaterial(br_material* pMaterial) {
#ifndef CARPOCALYPSE2_MATCHING
    int need_update = 0;
    pMaterial->user = (void*)(uintptr_t)0x5ba0;

    if (pMaterial->ka <= 0.999) {
        need_update = 1;
    }
    pMaterial->ka = 1.f;

    /* ka is tested twice, but kd is set the 2nd time */
    if (pMaterial->ka < 0.999) {
        need_update = 1;
    }
    pMaterial->kd = 1.f;

    if (!(pMaterial->flags & BR_MATF_SMOOTH)) {
        need_update = 1;
    }
    pMaterial->flags |= BR_MATF_SMOOTH;

    if (!(pMaterial->flags & BR_MATF_LIGHT)) {
        need_update = 1;
    }
    pMaterial->flags |= BR_MATF_LIGHT;

    if (pMaterial->flags & BR_MATF_PRELIT) {
        need_update = 1;
    }
    pMaterial->flags &= ~BR_MATF_PRELIT;

    if (need_update) {
        BrMaterialUpdate(pMaterial, BR_MATU_LIGHTING | BR_MATU_RENDERING);
    }
#else
    __asm {
            fld dword ptr [ecx + 0x10]
            fcomp qword ptr [gFloat_0058afb4+0x1c]
            xor edx, edx
            mov dword ptr [ecx + 0x94], 0x5ba0
            fnstsw ax
            test ah, 0x41
            je L004f6a41
            mov edx, 1
    L004f6a41:
            mov eax, 0x3f800000
            mov dword ptr [ecx + 0x10], eax
            mov dword ptr [ecx + 0x14], eax
            mov eax, dword ptr [ecx + 0x20]
            test al, 4
            jne L004f6a58
            mov edx, 1
    L004f6a58:
            or al, 4
            test al, 1
            mov dword ptr [ecx + 0x20], eax
            jne L004f6a66
            mov edx, 1
    L004f6a66:
            or al, 1
            test al, 2
            mov dword ptr [ecx + 0x20], eax
            je L004f6a74
            mov edx, 1
    L004f6a74:
            and al, 0xfd
            test edx, edx
            mov dword ptr [ecx + 0x20], eax
            je L004f6a88
            push 6
            push ecx
            call BrMaterialUpdate
            add esp, 8
    L004f6a88:
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004f6900
void C2_NAKED C2_HOOK_FASTCALL SmoothificatePowerups(br_actor* pActor) {
#ifndef CARPOCALYPSE2_MATCHING
    br_model* model;
    int i;

    if (gNbPixelBits != 16) {
        return;
    }
    model = pActor->model;
    if (model == NULL) {
        return;
    }
    if (pActor->material != NULL) {
        SmoothificatePowerupMaterial(pActor->material);
    }
    for (i = 0; i < model->nfaces; i++) {

        if (model->faces[i].material != NULL) {
            SmoothificatePowerupMaterial(model->faces[i].material);
        }
    }
#else
    __asm {
            mov eax, dword ptr [gNbPixelBits]
            push ebx
            push ebp
            push esi
            cmp eax, 0x10
            push edi
            jne L004f6a12
            mov ebx, dword ptr [ecx + 0x18]
            test ebx, ebx
            je L004f6a12
            mov ecx, dword ptr [ecx + 0x1c]
            mov ebp, 0x3f800000
            test ecx, ecx
            je L004f698c
            fld dword ptr [ecx + 0x10]
            fcomp qword ptr [gFloat_0058afb4+0x1c]
            xor edx, edx
            mov dword ptr [ecx + 0x94], 0x5ba0
            fnstsw ax
            test ah, 0x41
            je L004f694a
            mov edx, 1
    L004f694a:
            mov eax, dword ptr [ecx + 0x20]
            mov dword ptr [ecx + 0x10], ebp
            test al, 4
            mov dword ptr [ecx + 0x14], ebp
            jne L004f695c
            mov edx, 1
    L004f695c:
            or al, 4
            test al, 1
            mov dword ptr [ecx + 0x20], eax
            jne L004f696a
            mov edx, 1
    L004f696a:
            or al, 1
            test al, 2
            mov dword ptr [ecx + 0x20], eax
            je L004f6978
            mov edx, 1
    L004f6978:
            and al, 0xfd
            test edx, edx
            mov dword ptr [ecx + 0x20], eax
            je L004f698c
            push 6
            push ecx
            call BrMaterialUpdate
            add esp, 8
    L004f698c:
            xor edi, edi
            cmp word ptr [ebx + 0x12], di
            jbe L004f6a12
            xor esi, esi
    L004f6996:
            mov eax, dword ptr [ebx + 0xc]
            mov ecx, dword ptr [esi + eax + 8]
            test ecx, ecx
            je L004f6a04
            fld dword ptr [ecx + 0x10]
            fcomp qword ptr [gFloat_0058afb4+0x1c]
            xor edx, edx
            mov dword ptr [ecx + 0x94], 0x5ba0
            fnstsw ax
            test ah, 0x41
            je L004f69c2
            mov edx, 1
    L004f69c2:
            mov eax, dword ptr [ecx + 0x20]
            mov dword ptr [ecx + 0x10], ebp
            test al, 4
            mov dword ptr [ecx + 0x14], ebp
            jne L004f69d4
            mov edx, 1
    L004f69d4:
            or al, 4
            test al, 1
            mov dword ptr [ecx + 0x20], eax
            jne L004f69e2
            mov edx, 1
    L004f69e2:
            or al, 1
            test al, 2
            mov dword ptr [ecx + 0x20], eax
            je L004f69f0
            mov edx, 1
    L004f69f0:
            and al, 0xfd
            test edx, edx
            mov dword ptr [ecx + 0x20], eax
            je L004f6a04
            push 6
            push ecx
            call BrMaterialUpdate
            add esp, 8
    L004f6a04:
            xor ecx, ecx
            inc edi
            mov cx, word ptr [ebx + 0x12]
            add esi, 0x28
            cmp edi, ecx
            jl L004f6996
    L004f6a12:
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004f6a90
void C2_NAKED C2_HOOK_FASTCALL SmoothificateWorldMaterial(br_material* pMaterial) {
#ifndef CARPOCALYPSE2_MATCHING

    if (gNbPixelBits != 16) {
        return;
    }
    if ((uintptr_t)pMaterial->user == 0x5ba0) {
        SmoothificatePowerupMaterial(pMaterial);
    } else {
        pMaterial->user = NULL;
        pMaterial->ka = gLighting_data.ambient_else;
        pMaterial->kd = gLighting_data.diffuse_else;
        pMaterial->ks = 0.f;
        pMaterial->flags &= ~BR_MATF_PRELIT;
        pMaterial->flags |= BR_MATF_LIGHT | BR_MATF_SMOOTH;
        BrMaterialUpdate(pMaterial, BR_MATU_LIGHTING | BR_MATU_RENDERING);
    }
#else
    __asm {
            cmp dword ptr [gNbPixelBits], 0x10
            jne L004f6b48
            mov edx, dword ptr [ecx + 0x94]
            mov eax, 0x5ba0
            cmp edx, eax
            jne L004f6b11
            fld dword ptr [ecx + 0x10]
            fcomp qword ptr [gFloat_0058afb4+0x1c]
            mov dword ptr [ecx + 0x94], eax
            xor edx, edx
            fnstsw ax
            test ah, 0x41
            je L004f6ac9
            mov edx, 1
    L004f6ac9:
            mov eax, 0x3f800000
            mov dword ptr [ecx + 0x10], eax
            mov dword ptr [ecx + 0x14], eax
            mov eax, dword ptr [ecx + 0x20]
            test al, 4
            jne L004f6ae0
            mov edx, 1
    L004f6ae0:
            or al, 4
            test al, 1
            mov dword ptr [ecx + 0x20], eax
            jne L004f6aee
            mov edx, 1
    L004f6aee:
            or al, 1
            test al, 2
            mov dword ptr [ecx + 0x20], eax
            je L004f6afc
            mov edx, 1
    L004f6afc:
            and al, 0xfd
            test edx, edx
            mov dword ptr [ecx + 0x20], eax
            je L004f6b48
            push 6
            push ecx
            call BrMaterialUpdate
            add esp, 8
            ret
    L004f6b11:
            mov dword ptr [ecx + 0x94], 0
            mov eax, dword ptr [gLighting_data+0xc]
            mov dword ptr [ecx + 0x10], eax
            mov eax, dword ptr [ecx + 0x20]
            mov edx, dword ptr [gLighting_data+0x10]
            and al, 0xfd
            or al, 5
            push 6
            push ecx
            mov dword ptr [ecx + 0x14], edx
            mov dword ptr [ecx + 0x18], 0
            mov dword ptr [ecx + 0x20], eax
            call BrMaterialUpdate
            add esp, 8
    L004f6b48:
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004f6820
void C2_NAKED C2_HOOK_FASTCALL SmoothlySetWorldMaterialFlags(tBrender_storage* pStorage) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;
    for (i = 0; i < pStorage->materials_count; i++) {
        br_material *material = pStorage->materials[i];

        SmoothificateWorldMaterial(material);
    }
#else
    __asm {
            push ebx
            mov ebx, ecx
            push esi
            push edi
            mov eax, dword ptr [ebx + 8]
            xor esi, esi
            xor edi, edi
            cmp eax, esi
            jle L004f68f1
            push ebp
            mov ebp, 0x3f800000
    L004f683a:
            mov eax, dword ptr [ebx + 0x38]
            mov ecx, dword ptr [eax + edi*4]
            mov eax, dword ptr [gNbPixelBits]
            cmp eax, 0x10
            jne L004f68e4
            mov edx, dword ptr [ecx + 0x94]
            mov eax, 0x5ba0
            cmp edx, eax
            jne L004f68b3
            fld dword ptr [ecx + 0x10]
            fcomp qword ptr [gFloat_0058afb4+0x1c]
            mov dword ptr [ecx + 0x94], eax
            xor edx, edx
            fnstsw ax
            test ah, 0x41
            je L004f687a
            mov edx, 1
    L004f687a:
            mov eax, dword ptr [ecx + 0x20]
            mov dword ptr [ecx + 0x10], ebp
            test al, 4
            mov dword ptr [ecx + 0x14], ebp
            jne L004f688c
            mov edx, 1
    L004f688c:
            or al, 4
            test al, 1
            mov dword ptr [ecx + 0x20], eax
            jne L004f689a
            mov edx, 1
    L004f689a:
            or al, 1
            test al, 2
            mov dword ptr [ecx + 0x20], eax
            je L004f68a8
            mov edx, 1
    L004f68a8:
            and al, 0xfd
            cmp edx, esi
            mov dword ptr [ecx + 0x20], eax
            je L004f68e4
            jmp L004f68d9
    L004f68b3:
            mov dword ptr [ecx + 0x94], esi
            mov edx, dword ptr [gLighting_data+0xc]
            mov dword ptr [ecx + 0x10], edx
            mov edx, dword ptr [ecx + 0x20]
            mov eax, dword ptr [gLighting_data+0x10]
            and edx, 0xfffffffd
            or edx, 5
            mov dword ptr [ecx + 0x14], eax
            mov dword ptr [ecx + 0x18], esi
            mov dword ptr [ecx + 0x20], edx
    L004f68d9:
            push 6
            push ecx
            call BrMaterialUpdate
            add esp, 8
    L004f68e4:
            mov eax, dword ptr [ebx + 8]
            inc edi
            cmp edi, eax
            jl L004f683a
            pop ebp
    L004f68f1:
            pop edi
            pop esi
            pop ebx
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004f6640
void C2_NAKED C2_HOOK_FASTCALL LoadTrackMaterials(tBrender_storage* pStorage, const char* pPath) {
#ifndef CARPOCALYPSE2_MATCHING

    gMaterial_shading_for_callback = kRendererShadingType_Default;
    gStorage_for_callbacks = pStorage;
    PFForEveryFile(pPath, LoadIfItsAMaterial);
    gMaterial_shading_for_callback = kRendererShadingType_Undefined;
    SmoothlySetWorldMaterialFlags(pStorage);
#else
    __asm {
            push ebx
            push esi
            mov eax, edx
            mov esi, ecx
            push edi
            mov edx, offset LoadIfItsAMaterial
            mov ecx, eax
            mov dword ptr [gMaterial_shading_for_callback], 1
            mov dword ptr [gStorage_for_callbacks], esi
            call PFForEveryFile
            mov dword ptr [gMaterial_shading_for_callback], 0xffffffff
            mov eax, dword ptr [esi + 8]
            xor ebx, ebx
            xor edi, edi
            cmp eax, ebx
            jle L004f6739
            push ebp
            mov ebp, 0x3f800000
    L004f6682:
            mov eax, dword ptr [esi + 0x38]
            mov ecx, dword ptr [eax + edi*4]
            mov eax, dword ptr [gNbPixelBits]
            cmp eax, 0x10
            jne L004f672c
            mov edx, dword ptr [ecx + 0x94]
            mov eax, 0x5ba0
            cmp edx, eax
            jne L004f66fb
            fld dword ptr [ecx + 0x10]
            fcomp qword ptr [gFloat_0058afb4+0x1c]
            mov dword ptr [ecx + 0x94], eax
            xor edx, edx
            fnstsw ax
            test ah, 0x41
            je L004f66c2
            mov edx, 1
    L004f66c2:
            mov eax, dword ptr [ecx + 0x20]
            mov dword ptr [ecx + 0x10], ebp
            test al, 4
            mov dword ptr [ecx + 0x14], ebp
            jne L004f66d4
            mov edx, 1
    L004f66d4:
            or al, 4
            test al, 1
            mov dword ptr [ecx + 0x20], eax
            jne L004f66e2
            mov edx, 1
    L004f66e2:
            or al, 1
            test al, 2
            mov dword ptr [ecx + 0x20], eax
            je L004f66f0
            mov edx, 1
    L004f66f0:
            and al, 0xfd
            cmp edx, ebx
            mov dword ptr [ecx + 0x20], eax
            je L004f672c
            jmp L004f6721
    L004f66fb:
            mov dword ptr [ecx + 0x94], ebx
            mov edx, dword ptr [gLighting_data+0xc]
            mov dword ptr [ecx + 0x10], edx
            mov edx, dword ptr [ecx + 0x20]
            mov eax, dword ptr [gLighting_data+0x10]
            and edx, 0xfffffffd
            or edx, 5
            mov dword ptr [ecx + 0x14], eax
            mov dword ptr [ecx + 0x18], ebx
            mov dword ptr [ecx + 0x20], edx
    L004f6721:
            push 6
            push ecx
            call BrMaterialUpdate
            add esp, 8
    L004f672c:
            mov eax, dword ptr [esi + 8]
            inc edi
            cmp edi, eax
            jl L004f6682
            pop ebp
    L004f6739:
            pop edi
            pop esi
            pop ebx
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004f6580
int C2_HOOK_FASTCALL AddSmoothModels(tBrender_storage* pStorage, const char* pPath) {
    br_model* temp_array[2000];
    int count;
    int new_ones;
    int i;

    new_ones = 0;
    count = BrModelLoadMany(pPath, temp_array, CARPOCALYPSE2_ASIZE(temp_array));
    WhitenVertexRGB(temp_array, count);
    if (count == 0) {
        FatalError(kFatalError_CannotLoadModelFileOrItIsEmpty_S, pPath);
    }
    for (i = 0; i < count; i++) {
        if (temp_array[i] == NULL) {
            continue;
        }
        switch (AddModelToStorage(pStorage, temp_array[i])) {
        case eStorage_not_enough_room:
            FatalError(kFatalError_InsufficientModelSlots);
            break;
        case eStorage_duplicate:
            BrModelFree(temp_array[i]);
            break;
        case eStorage_allocated:
            temp_array[i]->flags |= BR_MODF_UPDATEABLE;
            temp_array[i]->flags &= ~(BR_MODF_DONT_WELD | BR_MODF_CUSTOM_NORMALS);
            BrModelAdd(temp_array[i]);
            new_ones++;
            break;
        }
    }
    return new_ones;
}

// FUNCTION: CARMA2_HW 0x004f6540
void C2_HOOK_FASTCALL SmoothlyLoadIfItsAModel(const char* pPath) {
    char s[256];

    Uppercaseificate(s, pPath);
    if (strstr(s, ".DAT") != NULL) {
        AddSmoothModels(gStorage_for_callbacks, pPath);
    }
}

// FUNCTION: CARMA2_HW 0x004f6520
void C2_HOOK_FASTCALL LoadTrackModels(tBrender_storage *pStorage, const char *pPath) {
    gStorage_for_callbacks = pStorage;
    PFForEveryFile(pPath, SmoothlyLoadIfItsAModel);
}

// FUNCTION: CARMA2_HW 0x00486d70
void C2_HOOK_FASTCALL KillLightingEffects(void) {

    // empty
}

// FUNCTION: CARMA2_HW 0x00486d80
void C2_HOOK_FASTCALL InitLightingEffects(void) {

    // empty
}

// LoadStaticLightingForRace

// ReadGlobalLightingInfo
// FUNCTION: CARMA2_HW 0x00486d90
void C2_HOOK_STDCALL FUN_00486d90(int pArg1, int pArg2) {

    // empty
}
