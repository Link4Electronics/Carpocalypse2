#include "modsupt.h"

#include "dbsetup.h"

#include "core/fw/resource.h"
#include "core/math/matrix34.h"

#include "c2_math.h"

// FIXME: move this definition (+BR_MAC3+BR_MAC4) to br_defs.h (and remove from matrix34.c matrix4.c ...)
#define BR_MAC2(A, B, C, D) ((A) * (B) + (C) * (D))

#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_CDECL BrMatrix34Identity();
extern void C2_HOOK_CDECL BrMatrix34PreScale();
extern void C2_HOOK_CDECL BrMatrix34PreTranslate();
extern char IID_IDirectInputDevice2A[];
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern char gZlib_filesystem[];
#endif
// FUNCTION: CARMA2_HW 0x00521490
void C2_HOOK_CDECL BrModelApplyMap(br_model* model, int map_type, br_matrix34* xform) {
    int v;
    br_vertex* vp;
    br_vector3 mv;
    br_matrix34 default_xform;
    br_scalar d;

    if (xform == NULL) {
        BrMatrix34Identity(&default_xform);
        xform = &default_xform;
    }
    for (v = 0; v < model->nvertices; v++, vp++) {
        vp = &model->vertices[v];
        BrMatrix34ApplyP(&mv, &vp->p, xform);
        switch (map_type) {
        case BR_APPLYMAP_PLANE:
            vp->map.v[0] = (mv.v[0] + 1.f) * 0.5f;
            vp->map.v[1] = (mv.v[1] + 1.f) * 0.5f;
            break;
        case BR_APPLYMAP_SPHERE:
            vp->map.v[0] = BrAngleToDegrees(BR_ATAN2(-mv.v[2], mv.v[0])) / 360.f;
            d = BR_MAC2(mv.v[0], mv.v[0], mv.v[2], mv.v[2]);
            vp->map.v[1] = 1.f - BrAngleToDegrees(BR_ATAN2(d, mv.v[1])) / 180.f;
            break;
        case BR_APPLYMAP_CYLINDER:
            vp->map.v[0] = BrAngleToDegrees(BR_ATAN2(-mv.v[2], mv.v[0])) / 360.f;
            vp->map.v[1] = (mv.v[1] + 1.f) * 0.5f;
            break;
        case BR_APPLYMAP_DISC:
            vp->map.v[0] = BrAngleToDegrees(BR_ATAN2(-mv.v[1], mv.v[0])) / 360.f;
            vp->map.v[1] = (br_scalar)sqrt(BR_MAC2(mv.v[0], mv.v[0], mv.v[1], mv.v[1]));
            break;
        case BR_APPLYMAP_NONE:
            vp->map.v[0] = 0.f;
            vp->map.v[1] = 0.f;
            break;
        }
    }
}

// FUNCTION: CARMA2_HW 0x00521660
C2_NAKED br_matrix34* C2_HOOK_CDECL BrModelFitMap(br_model* model, int axis_0, int axis_1, br_matrix34* transform) {
#ifndef CARPOCALYPSE2_MATCHING
    br_vector3 axis_3;
    br_vector3 tr;
    br_vector3 sc;
    int i;
    static br_vector3 axis_vectors[6] = {
        { {  1.f,  0.f,  0.f } },
        { {  0.f,  1.f,  0.f } },
        { {  0.f,  0.f,  1.f } },
        { { -1.f,  0.f,  0.f } },
        { {  0.f, -1.f,  0.f } },
        { {  0.f,  0.f, -1.f } },
    };

    BrMatrix34Identity(transform);
    axis_3.v[0] = axis_vectors[axis_1].v[2] * axis_vectors[axis_0].v[1] - axis_vectors[axis_1].v[1] * axis_vectors[axis_0].v[2];
    axis_3.v[1] = axis_vectors[axis_1].v[0] * axis_vectors[axis_0].v[2] - axis_vectors[axis_1].v[2] * axis_vectors[axis_0].v[0];
    axis_3.v[2] = axis_vectors[axis_1].v[1] * axis_vectors[axis_0].v[0] - axis_vectors[axis_1].v[0] * axis_vectors[axis_0].v[1];
    for (i = 0; i < 3; i++) {
        transform->m[i][0] = axis_vectors[axis_0].v[i];
        transform->m[i][1] = axis_vectors[axis_1].v[i];
        transform->m[i][2] = axis_3.v[i];
    }
    for (i = 0; i < 3; i++) {
        tr.v[i] = -(model->bounds.max.v[i] * .5f + model->bounds.min.v[i] * .5f);
    }
    for (i = 0; i < 3; i++) {
        if (model->bounds.max.v[i] == model->bounds.min.v[i]) {
            sc.v[i] = 1.f;
        } else {
            sc.v[i] = 1.f / (model->bounds.max.v[i] * .5f - model->bounds.min.v[i] * .5f);
        }
    }

    BrMatrix34PreScale(transform, sc.v[0], sc.v[1], sc.v[2]);
    BrMatrix34PreTranslate(transform, tr.v[0], tr.v[1], tr.v[2]);
    return transform;
#else
    __asm {
            sub esp, 0x18
            push ebx
            push ebp
            mov ebp, dword ptr [esp + 0x30]
            push esi
            push edi
            push ebp
            call BrMatrix34Identity
            mov eax, dword ptr [esp + 0x38]
            mov edx, dword ptr [esp + 0x34]
            add esp, 4
            lea esi, [esp + 0x10]
            lea ecx, [eax + eax*2]
            lea edx, [edx + edx*2]
            shl ecx, 2
            shl edx, 2
            lea edi, [ecx + gZlib_filesystem+0x188]
            lea eax, [ebp + 8]
            fld dword ptr [edx + gZlib_filesystem+0x18c]
            fld dword ptr [ecx + gZlib_filesystem+0x190]
            fld dword ptr [ecx + gZlib_filesystem+0x190]
            fld dword ptr [edx + gZlib_filesystem+0x18c]
            fld dword ptr [ecx + gZlib_filesystem+0x18c]
            fxch st(3)
            fmulp st(4), st(0)
            fld dword ptr [edx + gZlib_filesystem+0x190]
            fxch st(3)
            fmul dword ptr [edx + gZlib_filesystem+0x190]
            lea ebx, [edx + gZlib_filesystem+0x188]
            mov edx, edi
            fsubp st(4), st(0)
            _emit 0xd9
        _emit 0x07
            fxch st(4)
            fstp dword ptr [esp + 0x10]
            _emit 0xd9
        _emit 0x03
            fld dword ptr [ecx + gZlib_filesystem+0x18c]
            fxch st(4)
            fmulp st(5), st(0)
            _emit 0xd9
        _emit 0x03
            fxch st(1)
            fmulp st(3), st(0)
            fxch st(2)
            fsubp st(4), st(0)
            fxch st(1)
            fmulp st(2), st(0)
            fxch st(2)
            fstp dword ptr [esp + 0x14]
            _emit 0xd9
        _emit 0x07
            fmulp st(2), st(0)
            fxch st(1)
            fsubp st(1), st(0)
            mov ecx, ebx
            mov edi, 3
            fstp dword ptr [esp + 0x18]
    L0052170a:
            mov ebx, dword ptr [ecx]
            add ecx, 4
            mov dword ptr [eax - 8], ebx
            mov ebx, dword ptr [edx]
            mov dword ptr [eax - 4], ebx
            mov ebx, dword ptr [esi]
            mov dword ptr [eax], ebx
            add edx, 4
            add esi, 4
            add eax, 0xc
            dec edi
            jne L0052170a
            mov eax, dword ptr [esp + 0x2c]
            lea edx, [esp + 0x1c]
            mov esi, 3
            lea ecx, [eax + 0x34]
            mov eax, ecx
    L00521739:
            _emit 0xd9
        _emit 0x00
            fld dword ptr [eax + 0xc]
            fmul dword ptr [IID_IDirectInputDevice2A+0x414]
            fxch st(1)
            fmul dword ptr [IID_IDirectInputDevice2A+0x414]
            add eax, 4
            add edx, 4
            faddp st(1), st(0)
            dec esi
            fchs
            fstp dword ptr [edx - 4]
            jne L00521739
            lea edx, [esp + 0x10]
            mov esi, 3
    L00521765:
            fld dword ptr [ecx + 0xc]
            _emit 0xd8
        _emit 0x19
            fnstsw ax
            test ah, 0x40
            jne L00521790
            _emit 0xd9
        _emit 0x01
            fld dword ptr [ecx + 0xc]
            fmul dword ptr [IID_IDirectInputDevice2A+0x414]
            fxch st(1)
            fmul dword ptr [IID_IDirectInputDevice2A+0x414]
            fsubp st(1), st(0)
            fdivr qword ptr [IID_IDirectInputDevice2A+0x438]
            _emit 0xd9
        _emit 0x1a
            jmp L00521796
    L00521790:
            mov dword ptr [edx], 0x3f800000
    L00521796:
            add ecx, 4
            add edx, 4
            dec esi
            jne L00521765
            mov ecx, dword ptr [esp + 0x18]
            mov edx, dword ptr [esp + 0x14]
            mov eax, dword ptr [esp + 0x10]
            push ecx
            push edx
            push eax
            push ebp
            call BrMatrix34PreScale
            mov ecx, dword ptr [esp + 0x34]
            mov edx, dword ptr [esp + 0x30]
            mov eax, dword ptr [esp + 0x2c]
            add esp, 0x10
            push ecx
            push edx
            push eax
            push ebp
            call BrMatrix34PreTranslate
            add esp, 0x10
            mov eax, ebp
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x18
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x005217e0
void C2_HOOK_CDECL BrModelFree(br_model* m) {

    BrResFree(m);
}

// FUNCTION: CARMA2_HW 0x005217f0
br_model* C2_HOOK_CDECL BrModelAllocate(const char* name, int nvertices, int nfaces) {
    br_model* m;

    C2_HOOK_BUG_ON(sizeof(br_model) != 92);
    C2_HOOK_BUG_ON(sizeof(br_vertex) != 40);
    C2_HOOK_BUG_ON(sizeof(br_face) != 40);

    m = BrResAllocate(v1db.res, sizeof(br_model), BR_MEMORY_MODEL);
    m->nvertices = nvertices;
    m->nfaces = nfaces;
    if (name != NULL) {
        m->identifier = BrResStrDup(m, name);
    }
    if (nvertices != 0) {
        m->vertices = BrResAllocate(m, sizeof(br_vertex) * nvertices, BR_MEMORY_VERTICES);
    }
    if (nfaces != 0) {
        m->faces = BrResAllocate(m, sizeof(br_face) * nfaces, BR_MEMORY_FACES);
    }
    return m;
}

// FUNCTION: CARMA2_HW 0x00521870
br_primitive_list* C2_HOOK_CDECL BrPrimitiveListAllocate(br_uint_32 prim_type, br_uint_16 num_prims) {

    return NULL;
}

// FUNCTION: CARMA2_HW 0x00521880
br_uint_32 C2_HOOK_CDECL BrModelAddPrimitiveList(br_model* model, br_primitive_list* primitive_list) {

    return 0;
}
