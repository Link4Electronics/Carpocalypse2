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
void C2_HOOK_FASTCALL ReadSpillData(FILE* pF, tSlick_spec* pSlick_spec) {
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
}

// FUNCTION: CARMA2_HW 0x004e9be0
void C2_HOOK_FASTCALL AdjustSkid(int pSkid_num, br_matrix34* pMatrix, br_material* pMaterial) {

    gSkids[pSkid_num].actor->t.t.mat = *pMatrix;
    gSkids[pSkid_num].pos = *(br_vector3*)pMatrix->m[3];
    gSkids[pSkid_num].actor->material = pMaterial;
    gSkids[pSkid_num].actor->render_style = BR_RSTYLE_DEFAULT;
}

// FUNCTION: CARMA2_HW 0x004ea2c0
void C2_HOOK_FASTCALL StretchMark(tSkid* pMark, br_vector3* pFrom, br_vector3* pTo, br_scalar pTexture_start, br_scalar pTexture_step) {
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
}

// FUNCTION: CARMA2_HW 0x004ea490
void C2_HOOK_FASTCALL SkidMark(tCar_spec* pCar_spec, int pWheel_num) {
    br_material* material;
    int skid_mask;

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

    material = gCurrent_race.material_modifiers[pCar_spec->material_index[pWheel_num]].skid_mark_material;
    if (pCar_spec->oil_remaining[pWheel_num] != 0.f && pCar_spec->oldd[pWheel_num] < pCar_spec->susp_height[pWheel_num / 2]) {

        pCar_spec->new_skidding |= 1 << pWheel_num;
        material = pCar_spec->field_0x16fc[pWheel_num];
        if (material == NULL) {
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
    if (!(pCar_spec->new_skidding & skid_mask)) {

        pCar_spec->old_skidding &= ~skid_mask;
    } else if (!(pCar_spec->old_skidding & skid_mask)) {

        pCar_spec->old_skidding |= skid_mask;
        pCar_spec->total_length[pWheel_num] = 0.f;
        pCar_spec->old_skid[pWheel_num] = -1;
    } else {
        br_vector3* world_pos;
        br_vector3 disp;
        br_scalar dist;

        world_pos = &pCar_spec->special_start[pWheel_num];

        BrVector3Sub(&disp, world_pos, &pCar_spec->prev_skid_pos[pWheel_num]);
        dist = BrVector3Length(&disp);
        if (dist < 0.05f) {
            return;
        }
        if (SkidSection(&pCar_spec->old_skid[pWheel_num],
                &pCar_spec->field_0x160c[pWheel_num],
                &pCar_spec->field_0x163c[pWheel_num],
                material,
                world_pos,
                &pCar_spec->nor[pWheel_num],
                &pCar_spec->prev_skid_pos[pWheel_num],
                &pCar_spec->prev_nor[pWheel_num],
                pCar_spec->total_length[pWheel_num],
                0.05f)) {
            pCar_spec->old_skidding &= ~skid_mask;
        }
        pCar_spec->total_length[pWheel_num] += dist;
        pCar_spec->oil_remaining[pWheel_num] -= dist;
        if (pCar_spec->oil_remaining[pWheel_num] < 0.f) {
            pCar_spec->oil_remaining[pWheel_num] = 0.f;
        }
    }
    BrVector3Copy(&pCar_spec->prev_skid_pos[pWheel_num], &pCar_spec->special_start[pWheel_num]);
    BrVector3Copy(&pCar_spec->prev_nor[pWheel_num], &pCar_spec->nor[pWheel_num]);
}

// FUNCTION: CARMA2_HW 0x004e9f20
int C2_HOOK_FASTCALL SkidSection(tS16* pSkid_id, br_vector3* pSkid_start, br_vector3* pSkid_end, br_material* pMaterial, br_vector3* pPos, br_vector3* pNorm, br_vector3* pPrev_pos, br_vector3* pPrev_norm, br_scalar pTexture_start, br_scalar pTexture_step) {

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
}

// FUNCTION: CARMA2_HW 0x004ea700
void C2_HOOK_FASTCALL InitCarSkidStuff(tCar_spec* pCar) {

    pCar->old_skidding = 0;
    pCar->oil_remaining[0] = 0.f;
    pCar->oil_remaining[1] = 0.f;
    pCar->oil_remaining[2] = 0.f;
    pCar->oil_remaining[3] = 0.f;
}

// FUNCTION: CARMA2_HW 0x004ea720
void C2_HOOK_FASTCALL SkidsPerFrame(void) {
    int i;

    for (i = 0; i < CARPOCALYPSE2_ASIZE(gSkids); i++) {
        tSkid *skid = &gSkids[i];

        if (skid->actor->render_style != BR_RSTYLE_NONE) {
            BrVector3Copy(&skid->actor->t.t.translate.t, &skid->pos);
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
