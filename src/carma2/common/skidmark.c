#include "skidmark.h"

#include "errors.h"
#include "globvars.h"
#include "graphics.h"
#include "loading.h"
#include "loading3.h"
#include "piping.h"
#include "temp.h"

#include <brender/brender.h>

#include "carpocalypse2_macros.h"

#include "c2_string.h"
#include "skidmark.h"

// StretchMark

// MaterialFromIndex

// AdjustSkid

// FarFromLine2D

// Reflex2D

// GLOBAL: CARMA2_HW 0x0065fe08
static char gBoring_material_name_0[] = "OILSMEAR.MAT";
static char gBoring_material_name_1[] = "ROBSMEAR.MAT";
static char gMaterial_name_0[] = "OILSMEAR.MAT";
static char gMaterial_name_1[] = "GIBSMEAR.MAT";
char* gBoring_material_names[2] = {
    gBoring_material_name_0,
    gBoring_material_name_1,
};

// GLOBAL: CARMA2_HW 0x0065fe10
char* gMaterial_names[2] = {
    gMaterial_name_0,
    gMaterial_name_1,
};

// GLOBAL: CARMA2_HW 0x006a27f0
tSkid gSkids[100];

// GLOBAL: CARMA2_HW 0x006a27e8
int gCurrent_skid;

#ifdef CARPOCALYPSE2_MATCHING
extern br_material* C2_HOOK_CDECL BrMaterialFind();
extern void C2_HOOK_CDECL BrMatrix34Scale();
extern void C2_HOOK_CDECL BrModelUpdate();
extern void C2_NORETURN C2_HOOK_CDECL FatalError();
extern void C2_HOOK_FASTCALL GetAString(FILE* pF, char* pString);
extern void C2_HOOK_FASTCALL GetPairOfFloats(FILE* pF, float* pF1, float* pF2);
extern void C2_HOOK_FASTCALL GetThreeFloats(FILE * pF, float* pF1, float* pF2, float* pF3);
extern void C2_HOOK_FASTCALL PipeSingleSkidAdjustment(int pSkid_num, br_matrix34* pMatrix, br_material* pMaterial);
extern void __cdecl _ftol();
extern char gConst_kangeroo_force_factor[];
extern char gFloat_0058aec0[];
extern br_material* gMaterials_to_adapt[200];
extern int gMaterials_to_adapt_count;
extern char gNet_avail_names[];
#endif
// FUNCTION: CARMA2_HW 0x004e9ee0
void C2_HOOK_FASTCALL HideSkid(int pSkid_num) {

    gSkids[pSkid_num].actor->render_style = BR_RSTYLE_NONE;
}

// FUNCTION: CARMA2_HW 0x004e9f00
void C2_HOOK_FASTCALL HideSkids(void) {
    int skid;

    for (skid = 0; skid < (int)CARPOCALYPSE2_ASIZE(gSkids); skid++) {
        HideSkid(skid);
    }
}

// FUNCTION: CARMA2_HW 0x004ee5e0
void C2_NAKED C2_HOOK_FASTCALL ReadSpillData(FILE* pF, tSlick_spec* pSlick_spec) {
#ifndef CARPOCALYPSE2_MATCHING
    float f1, f2, f3;
    char s[256];

    GetAString(pF, s);
    if (strcmp(s, "none") == 0) {
        pSlick_spec->material = NULL;
        return;
    }

    pSlick_spec->material = LoadTemporaryMaterial(s);

    GetThreeFloats(pF, &f1, &f2, &f3);
    pSlick_spec->field_0x0 = (int)f1;
    pSlick_spec->field_0xc = f2;
    pSlick_spec->field_0x10 = f3;
    GetThreeFloats(pF, &f1, &f2, &f3);
    pSlick_spec->field_0x14 = f1;
    pSlick_spec->field_0x4 = f2;
    pSlick_spec->field_0x8 = f3;
    GetPairOfFloats(pF, &f1, &f2);
    pSlick_spec->field_0x20 = f2;
    pSlick_spec->field_0x24 = f3;
    if (pSlick_spec->field_0x24 != 0.f) {
        GetPairOfFloats(pF, &f1, &f2);
        pSlick_spec->field_0x18 = f2;
        pSlick_spec->field_0x1c = f3;
        GetAString(pF, s);
        pSlick_spec->material2 = LoadTemporaryMaterial(s);
    }
#else
    __asm {
            sub esp, 0x10c
            push ebx
            push ebp
            push esi
            push edi
            mov edi, edx
            mov ebp, ecx
            lea edx, [esp + 0x1c]
            call GetAString
            mov esi, offset gNet_avail_names+0x7b8
            lea eax, [esp + 0x1c]
    L004ee600:
            mov dl, byte ptr [eax]
            mov bl, byte ptr [esi]
            mov cl, dl
            cmp dl, bl
            jne L004ee628
            test cl, cl
            je L004ee624
            mov dl, byte ptr [eax + 1]
            mov bl, byte ptr [esi + 1]
            mov cl, dl
            cmp dl, bl
            jne L004ee628
            add eax, 2
            add esi, 2
            test cl, cl
            jne L004ee600
    L004ee624:
            xor eax, eax
            jmp L004ee62d
    L004ee628:
            sbb eax, eax
            sbb eax, -1
    L004ee62d:
            test eax, eax
            je L004ee76e
            lea eax, [esp + 0x1c]
            push eax
            call BrMaterialFind
            add esp, 4
            mov dword ptr [edi + 0x28], eax
            test eax, eax
            jne L004ee658
            lea ecx, [esp + 0x1c]
            push ecx
            push 0x78
            call FatalError
            add esp, 8
    L004ee658:
            mov eax, dword ptr [gMaterials_to_adapt_count]
            mov ecx, dword ptr [edi + 0x28]
            cmp eax, 0xc8
            jge L004ee674
            mov dword ptr [eax*4 + gMaterials_to_adapt], ecx
            inc eax
            mov dword ptr [gMaterials_to_adapt_count], eax
    L004ee674:
            lea edx, [esp + 0x10]
            lea eax, [esp + 0x18]
            push edx
            push eax
            lea edx, [esp + 0x1c]
            mov ecx, ebp
            call GetThreeFloats
            fld dword ptr [esp + 0x14]
            call _ftol
            mov ecx, dword ptr [esp + 0x18]
            mov edx, dword ptr [esp + 0x10]
            mov dword ptr [edi], eax
            mov dword ptr [edi + 0xc], ecx
            lea eax, [esp + 0x14]
            lea ecx, [esp + 0x18]
            push eax
            mov dword ptr [edi + 0x10], edx
            push ecx
            lea edx, [esp + 0x18]
            mov ecx, ebp
            call GetThreeFloats
            mov edx, dword ptr [esp + 0x10]
            mov ecx, dword ptr [esp + 0x14]
            mov eax, dword ptr [esp + 0x18]
            mov dword ptr [edi + 0x14], edx
            lea edx, [esp + 0x14]
            mov dword ptr [edi + 8], ecx
            push edx
            lea edx, [esp + 0x14]
            mov ecx, ebp
            mov dword ptr [edi + 4], eax
            call GetPairOfFloats
            fld dword ptr [esp + 0x14]
            fcomp dword ptr [gFloat_0058aec0]
            mov eax, dword ptr [esp + 0x10]
            mov ecx, dword ptr [esp + 0x14]
            mov dword ptr [edi + 0x20], eax
            mov dword ptr [edi + 0x24], ecx
            fnstsw ax
            test ah, 0x40
            jne L004ee775
            lea edx, [esp + 0x18]
            mov ecx, ebp
            push edx
            lea edx, [esp + 0x14]
            call GetPairOfFloats
            lea edx, [esp + 0x1c]
            mov ecx, dword ptr [esp + 0x18]
            mov eax, dword ptr [esp + 0x10]
            mov dword ptr [edi + 0x1c], ecx
            mov ecx, ebp
            mov dword ptr [edi + 0x18], eax
            call GetAString
            lea edx, [esp + 0x1c]
            push edx
            call BrMaterialFind
            add esp, 4
            mov dword ptr [edi + 0x2c], eax
            test eax, eax
            jne L004ee747
            lea eax, [esp + 0x1c]
            push eax
            push 0x78
            call FatalError
            add esp, 8
    L004ee747:
            mov eax, dword ptr [gMaterials_to_adapt_count]
            mov edi, dword ptr [edi + 0x2c]
            cmp eax, 0xc8
            jge L004ee775
            mov dword ptr [eax*4 + gMaterials_to_adapt], edi
            inc eax
            mov dword ptr [gMaterials_to_adapt_count], eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x10c
            ret
    L004ee76e:
            mov dword ptr [edi + 0x28], 0
    L004ee775:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x10c
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004e9be0
void C2_HOOK_FASTCALL AdjustSkid(int pSkid_num, br_matrix34* pMatrix, br_material* pMaterial) {

    gSkids[pSkid_num].actor->t.t.mat = *pMatrix;
    gSkids[pSkid_num].pos = *(br_vector3*)pMatrix->m[3];
    gSkids[pSkid_num].actor->material = pMaterial;
    gSkids[pSkid_num].actor->render_style = BR_RSTYLE_DEFAULT;
}

// FUNCTION: CARMA2_HW 0x004ea2c0
void C2_NAKED C2_HOOK_FASTCALL StretchMark(tSkid* pMark, br_vector3* pFrom, br_vector3* pTo, br_scalar pTexture_start, br_scalar pTexture_step) {
#ifndef CARPOCALYPSE2_MATCHING
    br_vector3 temp;
    br_vector3 average;
    br_scalar len;
    br_scalar length_scale;
    br_scalar texture_scale;
    br_matrix34* mat;
    br_model* model;

    mat = &pMark->actor->t.t.mat;
    BrVector3Sub(&temp, pTo, pFrom);
    len = BrVector3Length(&temp);
    if (!(len >= 0.001f)) {
        BrMatrix34Scale(mat, 0.001f, 1.0f, 0.001f);
        return;
    }

    texture_scale = pTexture_step / len;
    mat->m[1][0] = pMark->normal.v[0];
    mat->m[1][1] = pMark->normal.v[1];
    mat->m[1][2] = pMark->normal.v[2];
    length_scale = len / len;
    mat->m[2][0] = temp.v[1] * pMark->normal.v[2] - temp.v[2] * pMark->normal.v[1];
    mat->m[2][1] = temp.v[2] * pMark->normal.v[0] - temp.v[0] * pMark->normal.v[2];
    mat->m[2][2] = temp.v[0] * pMark->normal.v[1] - temp.v[1] * pMark->normal.v[0];
    mat->m[2][0] *= texture_scale;
    mat->m[2][1] *= texture_scale;
    mat->m[2][2] *= texture_scale;
    mat->m[0][0] = temp.v[0] * length_scale;
    mat->m[0][1] = temp.v[1] * length_scale;
    mat->m[0][2] = temp.v[2] * length_scale;

    average.v[0] = (pFrom->v[0] + pTo->v[0]) * 0.5f;
    average.v[1] = (pFrom->v[1] + pTo->v[1]) * 0.5f;
    average.v[2] = (pFrom->v[2] + pTo->v[2]) * 0.5f;
    pMark->pos.v[0] = average.v[0];
    pMark->pos.v[1] = average.v[1];
    pMark->pos.v[2] = average.v[2];
    mat->m[3][0] = pMark->pos.v[0];
    mat->m[3][1] = pMark->pos.v[1];
    mat->m[3][2] = pMark->pos.v[2];

    model = pMark->actor->model;
    model->vertices[1].map.v[0] = pTexture_start / pTexture_step;
    model->vertices[0].map.v[0] = model->vertices[1].map.v[0];
    model->vertices[3].map.v[0] = (pTexture_start + len) / pTexture_step;
    model->vertices[2].map.v[0] = model->vertices[3].map.v[0];
    BrModelUpdate(model, BR_MODU_VERTEX_MAPPING);
#else
    __asm {
            sub esp, 0xc
            push esi
            push edi
            mov edi, dword ptr [esp + 0x18]
            mov eax, dword ptr [ecx]
            _emit 0xd9
        _emit 0x07
            _emit 0xd8
        _emit 0x22
            fld dword ptr [edi + 4]
            fsub dword ptr [edx + 4]
            fld dword ptr [edi + 8]
            fsub dword ptr [edx + 8]
            fxch st(2)
            fst dword ptr [esp + 8]
            fxch st(1)
            fst dword ptr [esp + 0xc]
            fxch st(1)
            fmul dword ptr [esp + 8]
            fxch st(2)
            fstp dword ptr [esp + 0x10]
            lea esi, [eax + 0x2c]
            fmul dword ptr [esp + 0xc]
            fld dword ptr [esp + 0x10]
            fmul dword ptr [esp + 0x10]
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fsqrt
            fcom dword ptr [gConst_kangeroo_force_factor+0x218]
            fnstsw ax
            test ah, 1
            je L004ea339
            push 0x3a83126f
            push 0x3f800000
            push 0x3a83126f
            push esi
            fstp st(0)
            call BrMatrix34Scale
            add esp, 0x10
            pop edi
            pop esi
            add esp, 0xc
            ret 0xc
    L004ea339:
            fld st(0)
            fld dword ptr [esp + 0x20]
            mov eax, dword ptr [ecx + 4]
            fdiv st(0), st(2)
            mov dword ptr [esi + 0xc], eax
            mov eax, dword ptr [ecx + 8]
            mov dword ptr [esi + 0x10], eax
            mov eax, dword ptr [ecx + 0xc]
            mov dword ptr [esi + 0x14], eax
            fld dword ptr [ecx + 0xc]
            fld dword ptr [ecx + 8]
            fxch st(3)
            fdiv st(0), st(4)
            fxch st(1)
            fmul dword ptr [esp + 0xc]
            fld dword ptr [esp + 8]
            fxch st(4)
            fmul dword ptr [esp + 0x10]
            fld dword ptr [esp + 0xc]
            fxch st(1)
            fsubp st(2), st(0)
            fxch st(3)
            fstp dword ptr [esp + 0x18]
            fstp dword ptr [esi + 0x18]
            fld dword ptr [ecx + 0xc]
            fld dword ptr [ecx + 4]
            fmul dword ptr [esp + 0x10]
            fxch st(1)
            fmul dword ptr [esp + 8]
            fld dword ptr [esi + 0x18]
            fxch st(1)
            fsubp st(2), st(0)
            fxch st(4)
            fmul st(0), st(2)
            fxch st(1)
            fstp dword ptr [esi + 0x1c]
            fld dword ptr [ecx + 4]
            fld dword ptr [ecx + 8]
            fmul dword ptr [esp + 8]
            fxch st(1)
            fmul dword ptr [esp + 0xc]
            fld dword ptr [esi + 0x1c]
            fxch st(1)
            fsubp st(2), st(0)
            fxch st(5)
            fmul dword ptr [esp + 0x18]
            fxch st(1)
            fstp dword ptr [esi + 0x20]
            fld dword ptr [esi + 0x20]
            fxch st(5)
            fmul dword ptr [esp + 0x18]
            fxch st(5)
            fmul dword ptr [esp + 0x18]
            fxch st(1)
            fstp dword ptr [esi + 0x18]
            fxch st(4)
            fstp dword ptr [esi + 0x1c]
            fxch st(3)
            fstp dword ptr [esi + 0x20]
            fld dword ptr [esp + 0x10]
            fxch st(2)
            fmul st(0), st(1)
            fxch st(2)
            fmul st(0), st(1)
            fxch st(3)
            _emit 0xd9
        _emit 0x1e
            fxch st(1)
            fstp dword ptr [esi + 4]
            fxch st(1)
            fstp dword ptr [esi + 8]
            add esi, 0x24
            push 4
            fstp st(0)
            fld dword ptr [edx + 8]
            fld dword ptr [edx + 4]
            fadd dword ptr [edi + 4]
            fxch st(1)
            fadd dword ptr [edi + 8]
            fstp dword ptr [esp + 0x14]
            _emit 0xd9
        _emit 0x02
            _emit 0xd8
        _emit 0x07
            fmul dword ptr [gConst_kangeroo_force_factor+0x20c]
            fstp dword ptr [ecx + 0x10]
            fmul dword ptr [gConst_kangeroo_force_factor+0x20c]
            fstp dword ptr [ecx + 0x14]
            fld dword ptr [esp + 0x14]
            fmul dword ptr [gConst_kangeroo_force_factor+0x20c]
            fstp dword ptr [ecx + 0x18]
            fld dword ptr [esp + 0x20]
            fdiv dword ptr [esp + 0x24]
            mov edx, dword ptr [ecx + 0x10]
            mov dword ptr [esi], edx
            mov eax, dword ptr [ecx + 0x14]
            mov dword ptr [esi + 4], eax
            mov edx, dword ptr [ecx + 0x18]
            mov dword ptr [esi + 8], edx
            mov eax, dword ptr [ecx]
            mov eax, dword ptr [eax + 0x18]
            push eax
            mov ecx, dword ptr [eax + 8]
            fstp dword ptr [ecx + 0x34]
            mov ecx, dword ptr [eax + 8]
            fadd dword ptr [esp + 0x24]
            mov edx, dword ptr [ecx + 0x34]
            mov dword ptr [ecx + 0xc], edx
            mov ecx, dword ptr [eax + 8]
            fdiv dword ptr [esp + 0x28]
            fstp dword ptr [ecx + 0x84]
            mov ecx, dword ptr [eax + 8]
            mov edx, dword ptr [ecx + 0x84]
            mov dword ptr [ecx + 0x5c], edx
            call BrModelUpdate
            add esp, 8
            pop edi
            pop esi
            add esp, 0xc
            ret 0xc
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004ea490
void C2_HOOK_FASTCALL SkidMark(tCar_spec* pCar_spec, int pWheel_num) {
    br_material* material;
    int skid_mask;
    br_vector3 disp;
    br_scalar dist;
    int on_ground;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tRace_info, material_modifiers, 0xf08);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, material_index, 0x1274);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, oldd, 0x1264);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, new_skidding, 0x159c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, old_skidding, 0x15a0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, old_skid, 0x15a4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, special_start, 0x15ac);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, prev_skid_pos, 0x15dc);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, field_0x163c, 0x163c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, nor, 0x166c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, total_length, 0x16ec);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, oil_remaining, 0x16cc);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, prev_nor, 0x169c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, field_0x16fc, 0x16fc);

    on_ground = pCar_spec->oldd[pWheel_num] < pCar_spec->susp_height[pWheel_num >> 1];
    material = gCurrent_race.material_modifiers[pCar_spec->material_index[pWheel_num]].skid_mark_material;
    if (pCar_spec->oil_remaining[pWheel_num] != 0.0f && on_ground) {
        pCar_spec->new_skidding |= 1 << pWheel_num;
        if (pCar_spec->field_0x16fc[pWheel_num] != NULL) {
            material = pCar_spec->field_0x16fc[pWheel_num];
        } else {
            material = gMaterial[0];
        }
    }
    if (material == NULL) {
        pCar_spec->old_skidding &= ~(1 << pWheel_num);
        return;
    }
    skid_mask = 1 << pWheel_num;
    if (!(pCar_spec->new_skidding & skid_mask) && !(pCar_spec->old_skidding & skid_mask)) {
        return;
    }
    if (pCar_spec->new_skidding & skid_mask) {
        if (skid_mask & pCar_spec->old_skidding) {
            BrVector3Sub(&disp, &pCar_spec->special_start[pWheel_num], &pCar_spec->prev_skid_pos[pWheel_num]);
            dist = BrVector3Length(&disp);
            if (dist < 0.05f) {
                return;
            }
            if (SkidSection(&pCar_spec->old_skid[pWheel_num],
                    &pCar_spec->field_0x160c[pWheel_num],
                    &pCar_spec->field_0x163c[pWheel_num],
                    material,
                    &pCar_spec->special_start[pWheel_num],
                    &pCar_spec->nor[pWheel_num],
                    &pCar_spec->prev_skid_pos[pWheel_num],
                    &pCar_spec->prev_nor[pWheel_num],
                    pCar_spec->total_length[pWheel_num],
                    0.05f)) {
                pCar_spec->old_skidding &= ~skid_mask;
            }
            pCar_spec->total_length[pWheel_num] += dist;
            pCar_spec->oil_remaining[pWheel_num] -= dist;
            if (pCar_spec->oil_remaining[pWheel_num] < 0.0f) {
                pCar_spec->oil_remaining[pWheel_num] = 0.0f;
            }
        } else {
            pCar_spec->old_skidding |= skid_mask;
            pCar_spec->total_length[pWheel_num] = 0.0f;
            pCar_spec->old_skid[pWheel_num] = -1;
        }
    } else {
        pCar_spec->old_skidding &= ~skid_mask;
    }
    pCar_spec->prev_skid_pos[pWheel_num] = pCar_spec->special_start[pWheel_num];
    pCar_spec->prev_nor[pWheel_num] = pCar_spec->nor[pWheel_num];
}

// FUNCTION: CARMA2_HW 0x004e9f20
int C2_NAKED C2_HOOK_FASTCALL SkidSection(tS16* pSkid_id, br_vector3* pSkid_start, br_vector3* pSkid_end, br_material* pMaterial, br_vector3* pPos, br_vector3* pNorm, br_vector3* pPrev_pos, br_vector3* pPrev_norm, br_scalar pTexture_start, br_scalar pTexture_step) {
#ifndef CARPOCALYPSE2_MATCHING

    if (BrVector3Dot(pNorm, pPrev_norm) < 0.997f || fabsf(BrVector3Dot(pNorm, pPos) - BrVector3Dot(pPrev_pos, pNorm)) > 0.01f) {
        *pSkid_id = -1;
        return 1;
    }

    C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(gSkids) != 100);

    if (*pSkid_id < CARPOCALYPSE2_ASIZE(gSkids) && gSkids[*pSkid_id].actor->material == pMaterial) {
        br_scalar tmp = BrVector3LengthSquared((br_vector3*)gSkids[*pSkid_id].actor->t.t.mat.m[2]);
        if (tmp < 2 * (pTexture_step * pTexture_step) && pTexture_step * pTexture_step <= 2 * tmp) {
            if (BrVector3LengthSquared((br_vector3*)gSkids[*pSkid_id].actor->t.t.mat.m[0]) <= 0.5f) {
                br_vector3 t1, t2, t3;
                br_vector3 t4, t5;

                BrVector3Sub(&t1, pSkid_end, pSkid_start);
                t1.v[1] = 0.f;
                BrVector3Sub(&t2, pPos, pSkid_end);
                t2.v[1] = 0.f;
                BrVector3Cross(&t3, &t1, &t2);

                BrVector3Sub(&t4, pPos, pPrev_pos);
                t4.v[1] = 0.f;
                BrVector3Sub(&t5, pPrev_pos, pSkid_start);
                t5.v[1] = 0.f;

                if (fabsf(t3.v[1]) <= 0.05f * BrVector3Length(&t1) && BrVector3Dot(&t4, &t5) >= 0.f) {
                    StretchMark(&gSkids[*pSkid_id], pSkid_start, pPos, pTexture_start, pTexture_step);
                    PipeSingleSkidAdjustment(*pSkid_id, &gSkids[*pSkid_id].actor->t.t.mat, pMaterial);
                    return 0;
                }
            }
        }
    }
    BrVector3Copy(pSkid_start, pPrev_pos);
    BrVector3Copy(pSkid_end, pPos);
    gSkids[gCurrent_skid].actor->render_style = BR_RSTYLE_DEFAULT;
    gSkids[gCurrent_skid].actor->material = pMaterial;
    BrVector3Copy(&gSkids[gCurrent_skid].normal, pNorm);
    StretchMark(&gSkids[gCurrent_skid], pPrev_pos, pPos, pTexture_start, pTexture_step);
    PipeSingleSkidAdjustment(gCurrent_skid, &gSkids[gCurrent_skid].actor->t.t.mat, pMaterial);
    *pSkid_id = gCurrent_skid;
    gCurrent_skid = (gCurrent_skid + 1) % CARPOCALYPSE2_ASIZE(gSkids);
    return 0;
#else
    __asm {
            sub esp, 8
            mov eax, dword ptr [esp + 0x20]
            push ebx
            push ebp
            push esi
            fld dword ptr [eax + 4]
            mov esi, dword ptr [esp + 0x24]
            push edi
            fld dword ptr [eax + 8]
            fld dword ptr [esi + 4]
            fld dword ptr [esi + 8]
            _emit 0xd9
        _emit 0x06
            fxch st(2)
            fmulp st(4), st(0)
            mov dword ptr [esp + 0x28], ecx
            fmulp st(2), st(0)
            _emit 0xd8
        _emit 0x08
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fcomp dword ptr [gConst_kangeroo_force_factor+0x200]
            fnstsw ax
            test ah, 1
            jne L004ea2a0
            mov ebx, dword ptr [esp + 0x24]
            mov ebp, dword ptr [esp + 0x2c]
            fld dword ptr [ebx + 4]
            fld dword ptr [esi + 4]
            fld dword ptr [esi + 8]
            fld dword ptr [ebp + 4]
            fld dword ptr [ebx + 8]
            fxch st(3)
            fmulp st(4), st(0)
            fld dword ptr [esi + 4]
            fld dword ptr [ebp + 8]
            fxch st(3)
            fmulp st(4), st(0)
            fxch st(3)
            faddp st(4), st(0)
            fld dword ptr [esi + 8]
            _emit 0xd9
        _emit 0x06
            _emit 0xd8
        _emit 0x0b
            faddp st(5), st(0)
            _emit 0xd9
        _emit 0x45
        _emit 0x00
            fxch st(4)
            fmulp st(2), st(0)
            fmulp st(2), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fxch st(1)
            _emit 0xd8
        _emit 0x0e
            faddp st(1), st(0)
            fsubp st(1), st(0)
            fabs
            fcomp dword ptr [gConst_kangeroo_force_factor+0x204]
            fnstsw ax
            test ah, 0x41
            je L004ea2a0
            mov ax, word ptr [ecx]
            mov edi, dword ptr [esp + 0x1c]
            cmp ax, 0x64
            jae L004ea1a0
            and eax, 0xffff
            mov dword ptr [esp + 0x30], eax
            lea ecx, [eax*8]
            sub ecx, eax
            lea eax, [ecx*4 + gSkids]
            mov ecx, dword ptr [esp + 0x20]
            mov dword ptr [esp + 0x24], eax
            mov eax, dword ptr [eax]
            cmp ecx, dword ptr [eax + 0x1c]
            jne L004ea1a0
            mov ecx, dword ptr [esp + 0x30]
            mov eax, ecx
            fld dword ptr [esp + 0x38]
            shl eax, 3
            sub eax, ecx
            mov eax, dword ptr [eax*4 + gSkids]
            fld dword ptr [eax + 0x44]
            fld dword ptr [eax + 0x48]
            fld dword ptr [eax + 0x4c]
            fxch st(2)
            fst dword ptr [esp + 0x30]
            fld st(1)
            fxch st(1)
            fmul dword ptr [esp + 0x30]
            fxch st(1)
            fmul st(0), st(2)
            fxch st(3)
            fst dword ptr [esp + 0x30]
            fxch st(4)
            fmul dword ptr [esp + 0x38]
            fxch st(3)
            faddp st(1), st(0)
            fxch st(3)
            fmul dword ptr [esp + 0x30]
            fxch st(2)
            fstp st(1)
            fld st(0)
            fadd st(0), st(0)
            fxch st(2)
            faddp st(3), st(0)
            fxch st(1)
            fcomp st(2)
            fxch st(1)
            fnstsw ax
            test ah, 1
            jne L004ea19c
            fadd st(0), st(0)
            fxch st(1)
            fcompp
            fnstsw ax
            test ah, 0x41
            je L004ea1a0
            mov eax, ecx
            shl eax, 3
            sub eax, ecx
            mov eax, dword ptr [eax*4 + gSkids]
            fld dword ptr [eax + 0x2c]
            fld dword ptr [eax + 0x30]
            fld dword ptr [eax + 0x34]
            fxch st(2)
            fst dword ptr [esp + 0x30]
            fld st(1)
            fxch st(1)
            fmul dword ptr [esp + 0x30]
            fxch st(1)
            fmul st(0), st(2)
            fxch st(3)
            fst dword ptr [esp + 0x30]
            fmul dword ptr [esp + 0x30]
            fxch st(3)
            faddp st(1), st(0)
            fxch st(2)
            faddp st(2), st(0)
            fxch st(1)
            fsqrt
            fstp st(1)
            fcomp dword ptr [gConst_kangeroo_force_factor+0x20c]
            fnstsw ax
            test ah, 0x41
            je L004ea1a0
            _emit 0xd9
        _emit 0x07
            _emit 0xd8
        _emit 0x22
            fld dword ptr [edi + 8]
            fsub dword ptr [edx + 8]
            fxch st(1)
            fst dword ptr [esp + 0x10]
            fmul dword ptr [esp + 0x10]
            fld st(1)
            fmul st(0), st(2)
            _emit 0xd9
        _emit 0x03
            fld dword ptr [ebx + 8]
            fxch st(2)
            faddp st(3), st(0)
            _emit 0xd8
        _emit 0x27
            fxch st(1)
            fsub dword ptr [edi + 8]
            fxch st(2)
            fsqrt
            fld dword ptr [esp + 0x10]
            fxch st(2)
            fmul st(0), st(4)
            fxch st(2)
            fchs
            fmulp st(3), st(0)
            fst dword ptr [esp + 0x30]
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            fmul dword ptr [gConst_kangeroo_force_factor+0x210]
            fxch st(1)
            fst dword ptr [esp + 0x30]
            fabs
            fst dword ptr [esp + 0x30]
            fxch st(1)
            fcompp
            fnstsw ax
            test ah, 1
            fstp st(0)
            jne L004ea1a0
            fld dword ptr [ebp + 8]
            fld dword ptr [ebx + 8]
            _emit 0xd9
        _emit 0x45
        _emit 0x00
            _emit 0xd9
        _emit 0x03
            fxch st(3)
            fsub dword ptr [edx + 8]
            fxch st(2)
            fsub dword ptr [ebp + 8]
            fxch st(1)
            _emit 0xd8
        _emit 0x22
            fxch st(3)
            _emit 0xd8
        _emit 0x65
        _emit 0x00
            fxch st(1)
            fmulp st(2), st(0)
            fmulp st(2), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fcomp dword ptr [gConst_kangeroo_force_factor+0x214]
            fnstsw ax
            test ah, 1
            jne L004ea1a0
            mov ecx, dword ptr [esp + 0x38]
            mov eax, dword ptr [esp + 0x34]
            push ecx
            mov ecx, dword ptr [esp + 0x28]
            push eax
            push ebx
            call StretchMark
            xor ecx, ecx
            mov edx, dword ptr [esp + 0x28]
            mov eax, dword ptr [esp + 0x20]
            push eax
            mov cx, word ptr [edx]
            lea edx, [ecx*8]
            sub edx, ecx
            mov edx, dword ptr [edx*4 + gSkids]
            add edx, 0x2c
            call PipeSingleSkidAdjustment
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 8
            ret 0x20
    L004ea19c:
            fstp st(0)
            fstp st(0)
    L004ea1a0:
            mov eax, ebp
            mov ecx, dword ptr [eax]
            mov dword ptr [edx], ecx
            mov ecx, dword ptr [eax + 4]
            mov dword ptr [edx + 4], ecx
            mov ecx, ebx
            mov eax, dword ptr [eax + 8]
            mov dword ptr [edx + 8], eax
            mov edx, dword ptr [ecx]
            mov dword ptr [edi], edx
            mov eax, dword ptr [ecx + 4]
            mov dword ptr [edi + 4], eax
            mov ecx, dword ptr [ecx + 8]
            mov dword ptr [edi + 8], ecx
            mov eax, dword ptr [gCurrent_skid]
            and eax, 0xffff
            mov edi, dword ptr [esp + 0x20]
            mov edx, eax
            shl edx, 3
            sub edx, eax
            mov eax, dword ptr [edx*4 + gSkids]
            mov byte ptr [eax + 0x20], 0
            mov eax, dword ptr [gCurrent_skid]
            and eax, 0xffff
            mov ecx, eax
            shl ecx, 3
            sub ecx, eax
            mov edx, dword ptr [ecx*4 + gSkids]
            mov dword ptr [edx + 0x1c], edi
            mov ecx, dword ptr [gCurrent_skid]
            and ecx, 0xffff
            mov edx, dword ptr [esi]
            mov eax, ecx
            shl eax, 3
            sub eax, ecx
            shl eax, 2
            lea ecx, [eax + gSkids+0x4]
            mov dword ptr [eax + gSkids+0x4], edx
            mov edx, dword ptr [esi + 4]
            mov dword ptr [ecx + 4], edx
            mov edx, dword ptr [esi + 8]
            mov dword ptr [ecx + 8], edx
            mov ecx, dword ptr [esp + 0x38]
            mov edx, dword ptr [esp + 0x34]
            push ecx
            push edx
            push ebx
            mov edx, ebp
            lea ecx, [eax + gSkids]
            call StretchMark
            mov ecx, dword ptr [gCurrent_skid]
            and ecx, 0xffff
            push edi
            lea eax, [ecx*8]
            sub eax, ecx
            mov edx, dword ptr [eax*4 + gSkids]
            add edx, 0x2c
            call PipeSingleSkidAdjustment
            mov cx, word ptr [gCurrent_skid]
            mov edx, dword ptr [esp + 0x28]
            mov word ptr [edx], cx
            mov eax, dword ptr [gCurrent_skid]
            and eax, 0xffff
            mov ecx, 0x64
            inc eax
            cdq
            idiv ecx
            xor eax, eax
            mov word ptr [gCurrent_skid], dx
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 8
            ret 0x20
    L004ea2a0:
            pop edi
            pop esi
            pop ebp
            mov word ptr [ecx], 0xffff
            mov eax, 1
            pop ebx
            add esp, 8
            ret 0x20
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004ea700
void C2_HOOK_FASTCALL InitCarSkidStuff(tCar_spec* pCar) {
    int wheel;

    pCar->old_skidding = 0;
    for (wheel = 0; wheel < (int)CARPOCALYPSE2_ASIZE(pCar->oil_remaining); wheel++) {
        pCar->oil_remaining[wheel] = 0.0f;
    }
}

// FUNCTION: CARMA2_HW 0x004ea720
void C2_HOOK_FASTCALL SkidsPerFrame(void) {
    tSkid *skid;

    for (skid = &gSkids[0]; (int)skid < (int)&gSkids[CARPOCALYPSE2_ASIZE(gSkids)]; skid++) {
        if (skid->actor->render_style != BR_RSTYLE_NONE) {
            skid->actor->t.t.translate.t = skid->pos;
        }
    }
}
// FUNCTION: CARMA2_HW 0x004e9c40
void C2_HOOK_FASTCALL InitSkids(void) {
    int skid;
    int mat;
    int sl;
    br_model* square;
    br_pixelmap* pm;
    char* str;

    for (mat = 0; mat < (int)CARPOCALYPSE2_ASIZE(gMaterial_names); mat++) {
        if (gProgram_state.sausage_eater_mode) {
            str = gBoring_material_names[mat];
        } else {
            str = gMaterial_names[mat];
        }
        gMaterial[mat] = BrMaterialFind(str);
        if (gMaterial[mat] == NULL) {
            if (gProgram_state.sausage_eater_mode) {
                str = gBoring_material_names[mat];
            } else {
                str = gMaterial_names[mat];
            }

            str = strtok(str, ".");
            sl = strlen(str);
            strcat(str, ".PIX");
            pm = LoadPixelmap(str);
            if (pm == NULL) {
                FatalError(kFatalError_CantLoadPixelmapFile_S, str);
            }
            BrMapAdd(pm);
            str[sl] = '\0';
            strcat(str, ".MAT");
            gMaterial[mat] = LoadMaterial(str);
            if (gMaterial[mat] == NULL) {
                BrFatal("C:\\Carma2\\Source\\Common\\Skidmark.c", 215, "Couldn't find %s", gMaterial_names[mat]);
            } else {
                GlorifyMaterial(&gMaterial[mat], 1, kRendererShadingType_AmbientOnly);
            }
            BrMaterialAdd(gMaterial[mat]);
        }
    }

    C2_HOOK_BUG_ON(sizeof(tSkid) != 28);

    for (skid = 0; skid < (int)CARPOCALYPSE2_ASIZE(gSkids); skid++) {
        gSkids[skid].actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
        BrActorAdd(gOther_selfs[3], gSkids[skid].actor);
        gSkids[skid].actor->render_style = BR_RSTYLE_NONE;
        square = BrModelAllocate(NULL, 4, 2);
        BrVector3Set(&square->vertices[0].p, -0.5f, 0.0f, -0.5f);
        BrVector3Set(&square->vertices[1].p, -0.5f, 0.0f, 0.5f);
        BrVector3Set(&square->vertices[2].p, 0.5f, 0.0f, 0.5f);
        BrVector3Set(&square->vertices[3].p, 0.5f, 0.0f, -0.5f);
        BrVector2Set(&square->vertices[0].map, 0.0f, 0.0f);
        BrVector2Set(&square->vertices[1].map, 0.0f, 1.0f);
        BrVector2Set(&square->vertices[2].map, 1.0f, 1.0f);
        BrVector2Set(&square->vertices[3].map, 1.0f, 0.0f);
        square->faces[0].vertices[0] = 0;
        square->faces[0].vertices[1] = 1;
        square->faces[0].vertices[2] = 2;
        square->faces[0].smoothing = 1;
        square->faces[1].vertices[0] = 0;
        square->faces[1].vertices[1] = 2;
        square->faces[1].vertices[2] = 3;
        square->faces[1].smoothing = 1;
        square->flags |= BR_MODF_KEEP_ORIGINAL;
        BrModelAdd(square);
        gSkids[skid].actor->model = square;
    }
}

// HideSkid

// HideSkids

// SkidLen

// UglyWidthChange

// GenericSkidSection

// WheelSkidSection

// SkidMark

// InitCarSkidStuff

// SkidsPerFrame
