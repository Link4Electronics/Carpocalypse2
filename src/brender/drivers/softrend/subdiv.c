#include "subdiv.h"

#include "setup.h"

// GLOBAL: CARMA2_HW 0x00670810
br_uint_32 rightLeftTable[16] = {
    OUTCODE_RIGHT | OUTCODE_N_RIGHT,
    OUTCODE_RIGHT | OUTCODE_N_RIGHT,
    0x0,
    0x0,
    OUTCODE_RIGHT | OUTCODE_N_RIGHT,
    OUTCODE_RIGHT | OUTCODE_N_RIGHT,
    OUTCODE_RIGHT | OUTCODE_N_RIGHT | OUTCODE_LEFT | OUTCODE_N_LEFT,
    0x0,
    0x0,
    OUTCODE_LEFT | OUTCODE_N_LEFT,
    0x0,
    0x0,
    OUTCODE_RIGHT | OUTCODE_N_RIGHT | OUTCODE_LEFT | OUTCODE_N_LEFT,
    OUTCODE_LEFT | OUTCODE_N_LEFT,
    OUTCODE_RIGHT | OUTCODE_N_RIGHT | OUTCODE_LEFT | OUTCODE_N_LEFT,
    0x0,
};

// GLOBAL: CARMA2_HW 0x00670850
br_uint_32 topBottomTable[16] = {
    OUTCODE_TOP | OUTCODE_N_TOP,
    OUTCODE_TOP | OUTCODE_N_TOP,
    0x0,
    0x0,
    OUTCODE_TOP | OUTCODE_N_TOP,
    OUTCODE_TOP | OUTCODE_N_TOP,
    OUTCODE_BOTTOM | OUTCODE_N_BOTTOM | OUTCODE_TOP | OUTCODE_N_TOP,
    0x0,
    0x0,
    OUTCODE_BOTTOM | OUTCODE_N_BOTTOM,
    0x0,
    0x0,
    OUTCODE_BOTTOM | OUTCODE_N_BOTTOM | OUTCODE_TOP | OUTCODE_N_TOP,
    OUTCODE_BOTTOM | OUTCODE_N_BOTTOM,
    OUTCODE_BOTTOM | OUTCODE_N_BOTTOM | OUTCODE_TOP | OUTCODE_N_TOP,
    0x0,
};

// GLOBAL: CARMA2_HW 0x00670890
br_uint_32 hitherYonTable[16] = {
    OUTCODE_HITHER | OUTCODE_N_HITHER,
    OUTCODE_HITHER | OUTCODE_N_HITHER,
    0x0,
    0x0,
    OUTCODE_HITHER | OUTCODE_N_HITHER,
    OUTCODE_HITHER | OUTCODE_N_HITHER,
    OUTCODE_YON | OUTCODE_N_YON | OUTCODE_HITHER | OUTCODE_N_HITHER,
    0x0,
    0x0,
    OUTCODE_YON | OUTCODE_N_YON,
    0x0,
    0x0,
    OUTCODE_YON | OUTCODE_N_YON | OUTCODE_HITHER | OUTCODE_N_HITHER,
    OUTCODE_YON | OUTCODE_N_YON,
    OUTCODE_YON | OUTCODE_N_YON | OUTCODE_HITHER | OUTCODE_N_HITHER,
    0x0,
};

#define COMPUTE_COMPONENT_MID_POINT_VALUES(COMP) \
    do {                                         \
        br_scalar c1 = src1->comp[COMP];         \
        br_scalar c2 = src2->comp[COMP];         \
        br_scalar c3 = src3->comp[COMP];         \
        dest3->comp[COMP] = .5f * (c1 + c2);     \
        dest1->comp[COMP] = .5f * (c2 + c3);     \
        dest2->comp[COMP] = .5f * (c1 + c3);     \
    } while (0)

#define PROJECT_COMPONENT(C_P, C_SP)                                                                                                            \
    do {                                                                                                                                        \
        dest1->comp[C_SP] = renderer->state.cache.comp_offsets[C_SP] + renderer->state.cache.comp_scales[C_SP] * dest1->comp[C_P] * one_div_w1; \
        dest2->comp[C_SP] = renderer->state.cache.comp_offsets[C_SP] + renderer->state.cache.comp_scales[C_SP] * dest2->comp[C_P] * one_div_w2; \
        dest3->comp[C_SP] = renderer->state.cache.comp_offsets[C_SP] + renderer->state.cache.comp_scales[C_SP] * dest3->comp[C_P] * one_div_w3; \
    } while (0)

#define TEST_AND_PROJECT_VERTEX(R)                                                                                                                  \
    do {                                                                                                                                            \
        if (!((R)->flags & OUTCODES_ALL)) {                                                                                                         \
            br_scalar one_div_w = 1.f / (R)->comp[C_W];                                                                                             \
            dest1->comp[C_SX] = renderer->state.cache.comp_offsets[C_SX] + renderer->state.cache.comp_scales[C_SX] * (R)->comp[C_X] * one_div_w;    \
            dest2->comp[C_SY] = renderer->state.cache.comp_offsets[C_SY] + renderer->state.cache.comp_scales[C_SY] * (R)->comp[C_Y] * one_div_w;    \
            dest3->comp[C_SZ] = renderer->state.cache.comp_offsets[C_SZ] + renderer->state.cache.comp_scales[C_SZ] * (R)->comp[C_Z] * one_div_w;    \
        }                                                                                                                                           \
    } while (0)

#ifdef CARPOCALYPSE2_MATCHING
extern rend_block_soft rend;
extern char rendererStateStoredTemplateEntries[];
#endif
// FUNCTION: CARMA2_HW 0x0054c400
void C2_NAKED C2_HOOK_CDECL averageVerticesOnScreen(br_soft_renderer* renderer, brp_vertex* dest1, brp_vertex* dest2, brp_vertex* dest3, brp_vertex* src1, brp_vertex* src2, brp_vertex* src3) {
#ifndef CARPOCALYPSE2_MATCHING
    br_scalar one_div_w1;
    br_scalar one_div_w2;
    br_scalar one_div_w3;
    br_scalar w1w2w3;

    COMPUTE_COMPONENT_MID_POINT_VALUES(C_X);
    COMPUTE_COMPONENT_MID_POINT_VALUES(C_Y);
    COMPUTE_COMPONENT_MID_POINT_VALUES(C_Z);
    COMPUTE_COMPONENT_MID_POINT_VALUES(C_W);

    one_div_w1 = dest2->comp[C_W] * dest3->comp[C_W]; /* = w2 * w3 */
    one_div_w2 = dest1->comp[C_W] * dest3->comp[C_W]; /* = w1 * w3 */
    one_div_w3 = dest1->comp[C_W] * dest2->comp[C_W]; /* = w1 * w2 */
    w1w2w3 = 1.f / (dest1->comp[C_W] * one_div_w1); /* 1 / ( w1 * w2 * w3) */

    if (rend.block->convert_mask_f & (CM_R | CM_G | CM_B)) {
        COMPUTE_COMPONENT_MID_POINT_VALUES(C_R);
        COMPUTE_COMPONENT_MID_POINT_VALUES(C_G);
        COMPUTE_COMPONENT_MID_POINT_VALUES(C_B);
    }
    COMPUTE_COMPONENT_MID_POINT_VALUES(C_I);
    COMPUTE_COMPONENT_MID_POINT_VALUES(C_U);
    COMPUTE_COMPONENT_MID_POINT_VALUES(C_V);

    one_div_w1 *= w1w2w3; /* (w2 * w3) * 1 / ( w1 * w2 * w3) = 1 / w1 */
    one_div_w2 *= w1w2w3; /* (w1 * w3) * 1 / ( w1 * w2 * w3) = 1 / w2 */
    one_div_w3 *= w1w2w3; /* (w1 * w2) * 1 / ( w1 * w2 * w3) = 1 / w3 */

    PROJECT_COMPONENT(C_X, C_SX);
    PROJECT_COMPONENT(C_Y, C_SY);
    PROJECT_COMPONENT(C_Z, C_SZ);
#else
    __asm {
            push ebp
            mov ebp, esp
            push ebx
            push edi
            push esi
            mov edi, dword ptr [ebp + 0x1c]
            mov esi, dword ptr [ebp + 0x20]
            mov ecx, dword ptr [ebp + 0x14]
            mov edx, dword ptr [ebp + 0x18]
            mov eax, dword ptr [ebp + 0xc]
            mov ebx, dword ptr [ebp + 0x10]
            fld dword ptr [edx + 4]
            fadd dword ptr [edi + 4]
            fld dword ptr [edi + 4]
            fadd dword ptr [esi + 4]
            fxch st(1)
            fmul dword ptr [rendererStateStoredTemplateEntries+0x174]
            fld dword ptr [edx + 4]
            fxch st(2)
            fmul dword ptr [rendererStateStoredTemplateEntries+0x174]
            fxch st(2)
            fadd dword ptr [esi + 4]
            fxch st(1)
            fstp dword ptr [ecx + 4]
            fmul dword ptr [rendererStateStoredTemplateEntries+0x174]
            fxch st(1)
            fstp dword ptr [eax + 4]
            fstp dword ptr [ebx + 4]
            fld dword ptr [edx + 8]
            fadd dword ptr [edi + 8]
            fld dword ptr [edi + 8]
            fadd dword ptr [esi + 8]
            fxch st(1)
            fmul dword ptr [rendererStateStoredTemplateEntries+0x174]
            fld dword ptr [edx + 8]
            fxch st(2)
            fmul dword ptr [rendererStateStoredTemplateEntries+0x174]
            fxch st(2)
            fadd dword ptr [esi + 8]
            fxch st(1)
            fstp dword ptr [ecx + 8]
            fmul dword ptr [rendererStateStoredTemplateEntries+0x174]
            fxch st(1)
            fstp dword ptr [eax + 8]
            fstp dword ptr [ebx + 8]
            fld dword ptr [edx + 0xc]
            fadd dword ptr [edi + 0xc]
            fld dword ptr [edi + 0xc]
            fadd dword ptr [esi + 0xc]
            fxch st(1)
            fmul dword ptr [rendererStateStoredTemplateEntries+0x174]
            fld dword ptr [edx + 0xc]
            fxch st(2)
            fmul dword ptr [rendererStateStoredTemplateEntries+0x174]
            fxch st(2)
            fadd dword ptr [esi + 0xc]
            fxch st(1)
            fstp dword ptr [ecx + 0xc]
            fmul dword ptr [rendererStateStoredTemplateEntries+0x174]
            fxch st(1)
            fstp dword ptr [eax + 0xc]
            fstp dword ptr [ebx + 0xc]
            fld dword ptr [edx + 0x10]
            fadd dword ptr [edi + 0x10]
            fld dword ptr [edi + 0x10]
            fadd dword ptr [esi + 0x10]
            fxch st(1)
            fmul dword ptr [rendererStateStoredTemplateEntries+0x174]
            fld dword ptr [edx + 0x10]
            fxch st(2)
            fmul dword ptr [rendererStateStoredTemplateEntries+0x174]
            fxch st(2)
            fadd dword ptr [esi + 0x10]
            fxch st(1)
            fstp dword ptr [ecx + 0x10]
            fmul dword ptr [rendererStateStoredTemplateEntries+0x174]
            fxch st(1)
            fstp dword ptr [eax + 0x10]
            fstp dword ptr [ebx + 0x10]
            fld dword ptr [eax + 0x10]
            fmul dword ptr [ebx + 0x10]
            fld dword ptr [ebx + 0x10]
            fmul dword ptr [ecx + 0x10]
            fld st(1)
            fmul dword ptr [ecx + 0x10]
            fld dword ptr [eax + 0x10]
            fmul dword ptr [ecx + 0x10]
            fxch st(1)
            fdivr dword ptr [rendererStateStoredTemplateEntries+0x170]
            push ebp
            mov ebp, dword ptr [rend+0x4c]
            mov ebp, dword ptr [ebp + 0x20]
            and ebp, 0x3800
            pop ebp
            mov ebp, dword ptr [ebp + 8]
            je L0054c5d2
            fld dword ptr [edx + 0x2c]
            fadd dword ptr [edi + 0x2c]
            fld dword ptr [edi + 0x2c]
            fadd dword ptr [esi + 0x2c]
            fxch st(1)
            fmul dword ptr [rendererStateStoredTemplateEntries+0x174]
            fld dword ptr [edx + 0x2c]
            fxch st(2)
            fmul dword ptr [rendererStateStoredTemplateEntries+0x174]
            fxch st(2)
            fadd dword ptr [esi + 0x2c]
            fxch st(1)
            fstp dword ptr [ecx + 0x2c]
            fmul dword ptr [rendererStateStoredTemplateEntries+0x174]
            fxch st(1)
            fstp dword ptr [eax + 0x2c]
            fstp dword ptr [ebx + 0x2c]
            fld dword ptr [edx + 0x30]
            fadd dword ptr [edi + 0x30]
            fld dword ptr [edi + 0x30]
            fadd dword ptr [esi + 0x30]
            fxch st(1)
            fmul dword ptr [rendererStateStoredTemplateEntries+0x174]
            fld dword ptr [edx + 0x30]
            fxch st(2)
            fmul dword ptr [rendererStateStoredTemplateEntries+0x174]
            fxch st(2)
            fadd dword ptr [esi + 0x30]
            fxch st(1)
            fstp dword ptr [ecx + 0x30]
            fmul dword ptr [rendererStateStoredTemplateEntries+0x174]
            fxch st(1)
            fstp dword ptr [eax + 0x30]
            fstp dword ptr [ebx + 0x30]
            fld dword ptr [edx + 0x34]
            fadd dword ptr [edi + 0x34]
            fld dword ptr [edi + 0x34]
            fadd dword ptr [esi + 0x34]
            fxch st(1)
            fmul dword ptr [rendererStateStoredTemplateEntries+0x174]
            fld dword ptr [edx + 0x34]
            fxch st(2)
            fmul dword ptr [rendererStateStoredTemplateEntries+0x174]
            fxch st(2)
            fadd dword ptr [esi + 0x34]
            fxch st(1)
            fstp dword ptr [ecx + 0x34]
            fmul dword ptr [rendererStateStoredTemplateEntries+0x174]
            fxch st(1)
            fstp dword ptr [eax + 0x34]
            fstp dword ptr [ebx + 0x34]
    L0054c5d2:
            fld dword ptr [edx + 0x28]
            fadd dword ptr [edi + 0x28]
            fld dword ptr [edi + 0x28]
            fadd dword ptr [esi + 0x28]
            fxch st(1)
            fmul dword ptr [rendererStateStoredTemplateEntries+0x174]
            fld dword ptr [edx + 0x28]
            fxch st(2)
            fmul dword ptr [rendererStateStoredTemplateEntries+0x174]
            fxch st(2)
            fadd dword ptr [esi + 0x28]
            fxch st(1)
            fstp dword ptr [ecx + 0x28]
            fmul dword ptr [rendererStateStoredTemplateEntries+0x174]
            fxch st(1)
            fstp dword ptr [eax + 0x28]
            fstp dword ptr [ebx + 0x28]
            fld dword ptr [edx + 0x20]
            fadd dword ptr [edi + 0x20]
            fld dword ptr [edi + 0x20]
            fadd dword ptr [esi + 0x20]
            fxch st(1)
            fmul dword ptr [rendererStateStoredTemplateEntries+0x174]
            fld dword ptr [edx + 0x20]
            fxch st(2)
            fmul dword ptr [rendererStateStoredTemplateEntries+0x174]
            fxch st(2)
            fadd dword ptr [esi + 0x20]
            fxch st(1)
            fstp dword ptr [ecx + 0x20]
            fmul dword ptr [rendererStateStoredTemplateEntries+0x174]
            fxch st(1)
            fstp dword ptr [eax + 0x20]
            fstp dword ptr [ebx + 0x20]
            fld dword ptr [edx + 0x24]
            fadd dword ptr [edi + 0x24]
            fld dword ptr [edi + 0x24]
            fadd dword ptr [esi + 0x24]
            fxch st(1)
            fmul dword ptr [rendererStateStoredTemplateEntries+0x174]
            fld dword ptr [edx + 0x24]
            fxch st(2)
            fmul dword ptr [rendererStateStoredTemplateEntries+0x174]
            fxch st(2)
            fadd dword ptr [esi + 0x24]
            fxch st(1)
            fstp dword ptr [ecx + 0x24]
            fmul dword ptr [rendererStateStoredTemplateEntries+0x174]
            fxch st(1)
            fstp dword ptr [eax + 0x24]
            fstp dword ptr [ebx + 0x24]
            fmul st(2), st(0)
            fmul st(1), st(0)
            fmulp st(3), st(0)
            fld dword ptr [ebp + 0x5f8]
            fmul dword ptr [eax + 4]
            fld dword ptr [ebp + 0x5f8]
            fmul dword ptr [ebx + 4]
            fld dword ptr [ebp + 0x5f8]
            fxch st(2)
            fmul st(0), st(4)
            fxch st(1)
            fmul st(0), st(3)
            fxch st(1)
            fadd dword ptr [ebp + 0x5b8]
            fxch st(2)
            fmul dword ptr [ecx + 4]
            fxch st(1)
            fadd dword ptr [ebp + 0x5b8]
            fxch st(2)
            fstp dword ptr [eax + 0x14]
            fmul st(0), st(4)
            fxch st(1)
            fstp dword ptr [ebx + 0x14]
            fadd dword ptr [ebp + 0x5b8]
            fstp dword ptr [ecx + 0x14]
            fld dword ptr [ebp + 0x5fc]
            fmul dword ptr [eax + 8]
            fld dword ptr [ebp + 0x5fc]
            fmul dword ptr [ebx + 8]
            fld dword ptr [ebp + 0x5fc]
            fxch st(2)
            fmul st(0), st(4)
            fxch st(1)
            fmul st(0), st(3)
            fxch st(1)
            fadd dword ptr [ebp + 0x5bc]
            fxch st(2)
            fmul dword ptr [ecx + 8]
            fxch st(1)
            fadd dword ptr [ebp + 0x5bc]
            fxch st(2)
            fstp dword ptr [eax + 0x18]
            fmul st(0), st(4)
            fxch st(1)
            fstp dword ptr [ebx + 0x18]
            fadd dword ptr [ebp + 0x5bc]
            fstp dword ptr [ecx + 0x18]
            fld dword ptr [ebp + 0x600]
            fmul dword ptr [eax + 0xc]
            fld dword ptr [ebp + 0x600]
            fmul dword ptr [ebx + 0xc]
            fld dword ptr [ebp + 0x600]
            fxch st(2)
            fmul st(0), st(4)
            fxch st(1)
            fmul st(0), st(3)
            fxch st(1)
            fadd dword ptr [ebp + 0x5c0]
            fxch st(2)
            fmul dword ptr [ecx + 0xc]
            fxch st(1)
            fadd dword ptr [ebp + 0x5c0]
            fxch st(2)
            fstp dword ptr [eax + 0x1c]
            fmul st(0), st(4)
            fxch st(1)
            fstp dword ptr [ebx + 0x1c]
            fadd dword ptr [ebp + 0x5c0]
            fstp dword ptr [ecx + 0x1c]
            fstp st(0)
            fstp st(0)
            fstp st(0)
            pop esi
            pop edi
            pop ebx
            pop ebp
            ret
    }
#endif
}

static br_uint_32 OUTCODE_ORDINATE(br_uint_32 lValue, br_uint_32 rValue, br_uint_32* tableName1) {
#if 0
    br_uint_32 ldown = lValue & 0x7fffffff;
    br_uint_32 rdown = rValue & 0x7fffffff;
#endif
    br_uint_32 idx =
      (rValue & 0x80000000) / 2
    | (lValue & 0x80000000)
    | (rValue - lValue) / 8
    | ((lValue - rValue) & 0x80000000) / 4;
    return tableName1[idx >> 28];
}

// FUNCTION: CARMA2_HW 0x0054c7ef
void C2_HOOK_CDECL averageVertices(br_soft_renderer* renderer, brp_vertex* dest1, brp_vertex* dest2, brp_vertex* dest3, brp_vertex* src1, brp_vertex* src2, brp_vertex* src3) {

    COMPUTE_COMPONENT_MID_POINT_VALUES(C_X);
    COMPUTE_COMPONENT_MID_POINT_VALUES(C_Y);
    COMPUTE_COMPONENT_MID_POINT_VALUES(C_Z);
    COMPUTE_COMPONENT_MID_POINT_VALUES(C_W);

    COMPUTE_COMPONENT_MID_POINT_VALUES(C_R);
    COMPUTE_COMPONENT_MID_POINT_VALUES(C_G);
    COMPUTE_COMPONENT_MID_POINT_VALUES(C_B);

    COMPUTE_COMPONENT_MID_POINT_VALUES(C_I);
    COMPUTE_COMPONENT_MID_POINT_VALUES(C_U);
    COMPUTE_COMPONENT_MID_POINT_VALUES(C_V);

    dest1->flags = OUTCODES_NOT
            | OUTCODE_ORDINATE(dest1->comp_i[C_X], dest1->comp_i[C_W], rightLeftTable)
            | OUTCODE_ORDINATE(dest1->comp_i[C_Y], dest1->comp_i[C_W], topBottomTable)
            | OUTCODE_ORDINATE(dest1->comp_i[C_Z], dest1->comp_i[C_W], hitherYonTable);

    dest2->flags = OUTCODES_NOT
            | OUTCODE_ORDINATE(dest2->comp_i[C_X], dest2->comp_i[C_W], rightLeftTable)
            | OUTCODE_ORDINATE(dest2->comp_i[C_Y], dest2->comp_i[C_W], topBottomTable)
            | OUTCODE_ORDINATE(dest2->comp_i[C_Z], dest2->comp_i[C_W], hitherYonTable);

    dest3->flags = OUTCODES_NOT
            | OUTCODE_ORDINATE(dest3->comp_i[C_X], dest3->comp_i[C_W], rightLeftTable)
            | OUTCODE_ORDINATE(dest3->comp_i[C_Y], dest3->comp_i[C_W], topBottomTable)
            | OUTCODE_ORDINATE(dest3->comp_i[C_Z], dest3->comp_i[C_W], hitherYonTable);

    if (scache.user_clip_active) {
        int i;

        /* VERIFYME: this is different from Carmageddon II and BRender sources  */

        for (i = 0; i < BR_ASIZE(renderer->state.clip); i++) {
            soft_state_clip* clip;
            br_scalar dot;

            clip = &renderer->state.clip[i];
            if (clip->type != BRT_PLANE) {
                continue;
            }

            dot = BrVector4Dot((br_vector4*)&dest1->comp[C_X], &clip->plane);
            if (dot < 0) {
                dest1->flags ^= (OUTCODE_USER | OUTCODE_N_USER) << (BR_ASIZE(renderer->state.clip) - i - 1);
            }

            dot = BrVector4Dot((br_vector4*)&dest2->comp[C_X], &clip->plane);
            if (dot < 0) {
                dest2->flags ^= (OUTCODE_USER | OUTCODE_N_USER) << (BR_ASIZE(renderer->state.clip) - i - 1);
            }

            dot = BrVector4Dot((br_vector4*)&dest3->comp[C_X], &clip->plane);
            if (dot < 0) {
                dest3->flags ^= (OUTCODE_USER | OUTCODE_N_USER) << (BR_ASIZE(renderer->state.clip) - i - 1);
            }
        }
    }
    if (!(dest1->flags & OUTCODES_ALL) && !(dest2->flags & OUTCODES_ALL) && !(dest3->flags & OUTCODES_ALL)) {
        br_scalar one_div_w1 = dest2->comp[C_W] * dest3->comp[C_W]; /* = w2 * w3 */
        br_scalar one_div_w2 = dest1->comp[C_W] * dest3->comp[C_W]; /* = w1 * w3 */
        br_scalar one_div_w3 = dest1->comp[C_W] * dest2->comp[C_W]; /* = w1 * w2 */
        br_scalar w1w2w3 = 1.f / (dest1->comp[C_W] * one_div_w1); /* 1 / ( w1 * w2 * w3) */

        one_div_w1 *= w1w2w3; /* (w2 * w3) * 1 / ( w1 * w2 * w3) = 1 / w1 */
        one_div_w2 *= w1w2w3; /* (w1 * w3) * 1 / ( w1 * w2 * w3) = 1 / w2 */
        one_div_w3 *= w1w2w3; /* (w1 * w2) * 1 / ( w1 * w2 * w3) = 1 / w3 */

        PROJECT_COMPONENT(C_X, C_SX);
        PROJECT_COMPONENT(C_Y, C_SY);
        PROJECT_COMPONENT(C_Z, C_SZ);
        return;
    }
    TEST_AND_PROJECT_VERTEX(dest1);
    TEST_AND_PROJECT_VERTEX(dest2);
    TEST_AND_PROJECT_VERTEX(dest3);
}

// FUNCTION: CARMA2_HW 0x00560020
char* FUN_00560020(void) {

    return "1.0.4";
}
