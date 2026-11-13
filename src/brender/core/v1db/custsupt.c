#include "custsupt.h"

#include "dbsetup.h"

#include "core/math/fixed.h"
#include "core/math/matrix4.h"

// Carmageddon 2 adaptation

// GLOBAL: CARMA2_HW 0x0079feb4
br_scalar gScreenZOffset;

// GLOBAL: CARMA2_HW 0x00670530
const float gScreenZOffsetPresetChoices[7] = {
     0.0f,
    -1.5f,
    -3.0f,
    -4.5f,
    -6.0f,
    -7.5f,
    -9.0f,
};

#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_CDECL BrMatrix4ApplyP();
extern void C2_HOOK_CDECL BrMatrix4Mul34();
extern char IID_IDirectInputDevice2A[];
extern br_v1db_state v1db;
#endif
// FUNCTION: CARMA2_HW 0x00540560
void C2_HOOK_CDECL BrSetScreenZOffset(br_uint_32 pOffset) {

    C2_HOOK_BUG_ON(BR_ASIZE(gScreenZOffsetPresetChoices) != 7);

    if (pOffset < BR_ASIZE(gScreenZOffsetPresetChoices)) {
        gScreenZOffset = gScreenZOffsetPresetChoices[pOffset];
    } else {
        gScreenZOffset = 0.f;
    }
}

static br_uint_32 calculate_outcode(const br_vector4* v) {
    br_uint_32 outcode;

    outcode = 0xfff0000;
    if (v->v[0] >= v->v[3]) {
        outcode ^= 0x00020002;
    }
    if (v->v[0] < -v->v[3]) {
        outcode ^= 0x00010001;
    }
    if (v->v[1] >=  v->v[3]) {
        outcode ^= 0x00040004;
    }
    if (v->v[1] < -v->v[3]) {
        outcode ^= 0x00080008;
    }
    if (v->v[2] >= v->v[3]) {
        outcode ^= 0x00100010;
    }
    if (v->v[2] < -v->v[3]) {
        outcode ^= 0x00200020;
    }
    return outcode;
}

// FUNCTION: CARMA2_HW 0x005269f0
void C2_HOOK_CDECL BrModelToScreenQuery(br_matrix4* dest) {
    br_uint_32 dummy;
    br_matrix4 v2s;
    br_matrix34 m2v;

    v1db.renderer->dispatch->_partQueryBuffer(v1db.renderer, BRT_MATRIX, 0, &dummy, (br_uint_32*)&v2s, sizeof(v2s), BRT_VIEW_TO_SCREEN_M4_F);
    v1db.renderer->dispatch->_partQueryBuffer(v1db.renderer, BRT_MATRIX, 0, &dummy, (br_uint_32*)&m2v, sizeof(m2v), BRT_MODEL_TO_VIEW_M34_F);
    BrMatrix4Mul34(dest, &m2v, &v2s);
}

// FUNCTION: CARMA2_HW 0x00526a60
void C2_HOOK_CDECL BrModelToViewQuery(br_matrix34* dest) {
    br_uint_32 dummy;

    v1db.renderer->dispatch->_partQueryBuffer(v1db.renderer, BRT_MATRIX, 0, &dummy, (br_uint_32*)dest, sizeof(*dest), BRT_MODEL_TO_VIEW_M34_F);
}

// FUNCTION: CARMA2_HW 0x00526a90
br_uint_8 C2_HOOK_CDECL BrOriginToScreenXY(br_vector2* screen) {

    if (!v1db.model_to_screen_valid) {
        BrModelToScreenQuery(&v1db.model_to_screen);
        v1db.model_to_screen_valid = 1;
    }

    screen->v[0] = v1db.vp_ox + v1db.vp_width * v1db.model_to_screen.m[3][0] / v1db.model_to_screen.m[3][3];
    screen->v[1] = v1db.vp_oy + v1db.vp_height * v1db.model_to_screen.m[3][1] / v1db.model_to_screen.m[3][3];

    return v1db.model_to_screen.m[3][2] > 0.0f;
}

// FUNCTION: CARMA2_HW 0x00526b70
br_uint_32 C2_NAKED C2_HOOK_CDECL BrOriginToScreenXYZO(br_vector3* screen) {
#ifndef CARPOCALYPSE2_MATCHING
    br_uint_32 outcode;

    if (!v1db.model_to_screen_valid) {
        BrModelToScreenQuery(&v1db.model_to_screen);
        v1db.model_to_screen_valid = 1;
    }

    outcode = calculate_outcode((br_vector4*)v1db.model_to_screen.m[3]);

    if ((outcode & 0x00000fff) == 0) {
        screen->v[0] = v1db.vp_ox + v1db.vp_width * v1db.model_to_screen.m[3][0] / v1db.model_to_screen.m[3][3];
        screen->v[1] = v1db.vp_oy + v1db.vp_height * v1db.model_to_screen.m[3][1] / v1db.model_to_screen.m[3][3];
        screen->v[2] = gScreenZOffset + v1db.model_to_screen.m[3][3] / v1db.model_to_screen.m[3][2] * 32767.f;
    }
    return outcode;
#else
    __asm {
            mov eax, dword ptr [v1db+0x90]
            sub esp, 0x74
            test eax, eax
            push esi
            mov esi, 0xfff0000
            jne L00526bef
            mov eax, dword ptr [v1db+0xc]
            push 0xee
            lea edx, [esp + 0x3c]
            push 0x40
            mov ecx, dword ptr [eax]
            push edx
            lea edx, [esp + 0x10]
            push edx
            push 0
            push 0x76
            push eax
            call dword ptr [ecx + 0x8c]
            mov eax, dword ptr [v1db+0xc]
            add esp, 0x1c
            lea edx, [esp + 8]
            mov ecx, dword ptr [eax]
            push 0xea
            push 0x30
            push edx
            lea edx, [esp + 0x10]
            push edx
            push 0
            push 0x76
            push eax
            call dword ptr [ecx + 0x8c]
            add esp, 0x1c
            lea eax, [esp + 0x38]
            lea ecx, [esp + 8]
            push eax
            push ecx
            push offset v1db+0x20
            call BrMatrix4Mul34
            add esp, 0xc
            mov dword ptr [v1db+0x90], 1
    L00526bef:
            fld dword ptr [v1db+0x50]
            fcomp dword ptr [v1db+0x5c]
            fnstsw ax
            test ah, 1
            jne L00526c07
            mov esi, 0xffd0002
    L00526c07:
            fld dword ptr [v1db+0x50]
            fld dword ptr [v1db+0x5c]
            fchs
            fxch st(1)
            fcomp st(1)
            fnstsw ax
            test ah, 1
            je L00526c26
            xor esi, 0x10001
    L00526c26:
            fld dword ptr [v1db+0x54]
            fcomp dword ptr [v1db+0x5c]
            fnstsw ax
            test ah, 1
            jne L00526c3f
            xor esi, 0x40004
    L00526c3f:
            fld dword ptr [v1db+0x54]
            fcomp st(1)
            fnstsw ax
            test ah, 1
            je L00526c54
            xor esi, 0x80008
    L00526c54:
            fld dword ptr [v1db+0x58]
            fcomp dword ptr [v1db+0x5c]
            fnstsw ax
            test ah, 1
            jne L00526c6d
            xor esi, 0x100010
    L00526c6d:
            fld dword ptr [v1db+0x58]
            fcomp st(1)
            fnstsw ax
            test ah, 1
            fstp st(0)
            je L00526c84
            xor esi, 0x200020
    L00526c84:
            test esi, 0xfff
            jne L00526ce6
            fld dword ptr [v1db+0x4f4]
            fmul dword ptr [v1db+0x50]
            mov eax, dword ptr [esp + 0x7c]
            fdiv dword ptr [v1db+0x5c]
            fadd dword ptr [v1db+0x4ec]
            _emit 0xd9
        _emit 0x18
            fld dword ptr [v1db+0x4f8]
            fmul dword ptr [v1db+0x54]
            fdiv dword ptr [v1db+0x5c]
            fadd dword ptr [v1db+0x4f0]
            fstp dword ptr [eax + 4]
            fld dword ptr [v1db+0x58]
            fmul qword ptr [IID_IDirectInputDevice2A+0x470]
            fld dword ptr [gScreenZOffset]
            fld dword ptr [v1db+0x5c]
            fdivp st(2), st(0)
            fxch st(1)
            fsubp st(1), st(0)
            fstp dword ptr [eax + 8]
    L00526ce6:
            mov eax, esi
            pop esi
            add esp, 0x74
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00526cf0
br_uint_8 C2_HOOK_CDECL BrPointToScreenXY(br_vector2* screen, const br_vector3* point) {
    br_vector4 sp;

    if (!v1db.model_to_screen_valid) {
        BrModelToScreenQuery(&v1db.model_to_screen);
        v1db.model_to_screen_valid = 1;
    }
    BrMatrix4ApplyP(&sp, point, &v1db.model_to_screen);
    screen->v[0] = v1db.vp_ox + sp.v[0] * v1db.vp_width / sp.v[3];
    screen->v[1] = v1db.vp_oy + sp.v[1] * v1db.vp_height / sp.v[3];

    return sp.v[2] > 0.f;
}

// FUNCTION: CARMA2_HW 0x00526df0
br_uint_32 C2_NAKED C2_HOOK_CDECL BrPointToScreenXYZO(br_vector3* screen, const br_vector3* point) {
#ifndef CARPOCALYPSE2_MATCHING
    br_vector4 sp;
    br_uint_32 outcode;

    if (!v1db.model_to_screen_valid) {
        BrModelToScreenQuery(&v1db.model_to_screen);
        v1db.model_to_screen_valid = 1;
    }
    BrMatrix4ApplyP(&sp, point, &v1db.model_to_screen);
    outcode = calculate_outcode(&sp);

    if ((outcode & 0x00000fff) == 0) {
        screen->v[0] = v1db.vp_ox + v1db.vp_width * v1db.model_to_screen.m[3][0] / v1db.model_to_screen.m[3][3];
        screen->v[1] = v1db.vp_oy + v1db.vp_height * v1db.model_to_screen.m[3][1] / v1db.model_to_screen.m[3][3];
        screen->v[2] = gScreenZOffset + v1db.model_to_screen.m[3][3] / v1db.model_to_screen.m[3][2] * 32767.f;
    }
    return outcode;
#else
    __asm {
            mov eax, dword ptr [v1db+0x90]
            sub esp, 0x84
            test eax, eax
            push esi
            mov esi, 0xfff0000
            jne L00526e72
            mov eax, dword ptr [v1db+0xc]
            push 0xee
            lea edx, [esp + 0x4c]
            push 0x40
            mov ecx, dword ptr [eax]
            push edx
            lea edx, [esp + 0x20]
            push edx
            push 0
            push 0x76
            push eax
            call dword ptr [ecx + 0x8c]
            mov eax, dword ptr [v1db+0xc]
            add esp, 0x1c
            lea edx, [esp + 0x18]
            mov ecx, dword ptr [eax]
            push 0xea
            push 0x30
            push edx
            lea edx, [esp + 0x20]
            push edx
            push 0
            push 0x76
            push eax
            call dword ptr [ecx + 0x8c]
            add esp, 0x1c
            lea eax, [esp + 0x48]
            lea ecx, [esp + 0x18]
            push eax
            push ecx
            push offset v1db+0x20
            call BrMatrix4Mul34
            add esp, 0xc
            mov dword ptr [v1db+0x90], 1
    L00526e72:
            mov edx, dword ptr [esp + 0x90]
            push offset v1db+0x20
            lea eax, [esp + 8]
            push edx
            push eax
            call BrMatrix4ApplyP
            fld dword ptr [esp + 0x10]
            fcomp dword ptr [esp + 0x1c]
            add esp, 0xc
            fnstsw ax
            test ah, 1
            jne L00526ea0
            mov esi, 0xffd0002
    L00526ea0:
            fld dword ptr [esp + 4]
            fld dword ptr [esp + 0x10]
            fchs
            fxch st(1)
            fcomp st(1)
            fnstsw ax
            test ah, 1
            je L00526ebb
            xor esi, 0x10001
    L00526ebb:
            fld dword ptr [esp + 8]
            fcomp dword ptr [esp + 0x10]
            fnstsw ax
            test ah, 1
            jne L00526ed0
            xor esi, 0x40004
    L00526ed0:
            fld dword ptr [esp + 8]
            fcomp st(1)
            fnstsw ax
            test ah, 1
            je L00526ee3
            xor esi, 0x80008
    L00526ee3:
            fld dword ptr [esp + 0xc]
            fcomp dword ptr [esp + 0x10]
            fnstsw ax
            test ah, 1
            jne L00526ef8
            xor esi, 0x100010
    L00526ef8:
            fld dword ptr [esp + 0xc]
            fcomp st(1)
            fnstsw ax
            test ah, 1
            fstp st(0)
            je L00526f0d
            xor esi, 0x200020
    L00526f0d:
            test esi, 0xfff
            jne L00526f6e
            fld dword ptr [esp + 4]
            fmul dword ptr [v1db+0x4f4]
            fld dword ptr [esp + 8]
            fld dword ptr [esp + 0xc]
            fxch st(2)
            fdiv dword ptr [esp + 0x10]
            mov eax, dword ptr [esp + 0x8c]
            fadd dword ptr [v1db+0x4ec]
            fxch st(2)
            fmul qword ptr [IID_IDirectInputDevice2A+0x470]
            fxch st(2)
            _emit 0xd9
        _emit 0x18
            fmul dword ptr [v1db+0x4f8]
            fxch st(1)
            fdiv dword ptr [esp + 0x10]
            fxch st(1)
            fdiv dword ptr [esp + 0x10]
            fadd dword ptr [v1db+0x4f0]
            fstp dword ptr [eax + 4]
            fld dword ptr [gScreenZOffset]
            fxch st(1)
            fsubp st(1), st(0)
            fstp dword ptr [eax + 8]
    L00526f6e:
            mov eax, esi
            pop esi
            add esp, 0x84
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00526f80
void C2_HOOK_CDECL BrPointToScreenXYMany(br_vector2* screens, const br_vector3* points, br_uint_32 npoints) {
    br_vector4 sp;
    br_uint_32 i;

    if (!v1db.model_to_screen_valid) {
        BrModelToScreenQuery(&v1db.model_to_screen);
        v1db.model_to_screen_valid = 1;
    }

    for (i = 0; i < npoints; i++, points++, screens++) {
        BrMatrix4ApplyP(&sp, points, &v1db.model_to_screen);

        screens->v[0] = v1db.vp_ox + sp.v[0] * v1db.vp_width / sp.v[3];
        screens->v[1] = v1db.vp_oy + sp.v[1] * v1db.vp_height / sp.v[3];
    }
}

// FUNCTION: CARMA2_HW 0x00527070
void C2_NAKED C2_HOOK_CDECL BrPointToScreenXYZOMany(br_vector3* screens, br_uint_32* outcodes, br_vector3* points, br_uint_32 npoints) {
#ifndef CARPOCALYPSE2_MATCHING
    br_uint_32 outcode;
    br_vector4 sp;
    br_uint_32 i;

    if (!v1db.model_to_screen_valid) {
        BrModelToScreenQuery(&v1db.model_to_screen);
        v1db.model_to_screen_valid = 1;
    }

    for (i = 0; i < npoints; i++) {
        BrMatrix4ApplyP(&sp, &points[i], &v1db.model_to_screen);
        outcode = calculate_outcode(&sp);

        if ((outcode & 0x00000fff) == 0) {
            screens[i].v[0] = v1db.vp_ox + v1db.vp_width * v1db.model_to_screen.m[3][0] / v1db.model_to_screen.m[3][3];
            screens[i].v[1] = v1db.vp_oy + v1db.vp_height * v1db.model_to_screen.m[3][1] / v1db.model_to_screen.m[3][3];
            screens[i].v[2] = gScreenZOffset + v1db.model_to_screen.m[3][3] / v1db.model_to_screen.m[3][2] * 32767.f;
        }
    }
#else
    __asm {
            mov eax, dword ptr [v1db+0x90]
            sub esp, 0x84
            test eax, eax
            push ebx
            push ebp
            push esi
            push edi
            mov esi, 0xfff0000
            jne L005270f5
            mov eax, dword ptr [v1db+0xc]
            push 0xee
            lea edx, [esp + 0x58]
            push 0x40
            mov ecx, dword ptr [eax]
            push edx
            lea edx, [esp + 0x1c]
            push edx
            push 0
            push 0x76
            push eax
            call dword ptr [ecx + 0x8c]
            mov eax, dword ptr [v1db+0xc]
            add esp, 0x1c
            lea edx, [esp + 0x24]
            mov ecx, dword ptr [eax]
            push 0xea
            push 0x30
            push edx
            lea edx, [esp + 0x1c]
            push edx
            push 0
            push 0x76
            push eax
            call dword ptr [ecx + 0x8c]
            add esp, 0x1c
            lea eax, [esp + 0x54]
            lea ecx, [esp + 0x24]
            push eax
            push ecx
            push offset v1db+0x20
            call BrMatrix4Mul34
            add esp, 0xc
            mov dword ptr [v1db+0x90], 1
    L005270f5:
            mov eax, dword ptr [esp + 0xa4]
            test eax, eax
            jbe L00527226
            mov ebp, dword ptr [esp + 0xa0]
            mov edi, dword ptr [esp + 0x9c]
            mov ebx, dword ptr [esp + 0x98]
            mov dword ptr [esp + 0x10], eax
    L0052711d:
            push offset v1db+0x20
            lea edx, [esp + 0x18]
            push ebp
            push edx
            call BrMatrix4ApplyP
            fld dword ptr [esp + 0x20]
            fcomp dword ptr [esp + 0x2c]
            add esp, 0xc
            fnstsw ax
            test ah, 1
            jne L00527145
            xor esi, 0x20002
    L00527145:
            fld dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x20]
            fchs
            fxch st(1)
            fcomp st(1)
            fnstsw ax
            test ah, 1
            je L00527160
            xor esi, 0x10001
    L00527160:
            fld dword ptr [esp + 0x18]
            fcomp dword ptr [esp + 0x20]
            fnstsw ax
            test ah, 1
            jne L00527175
            xor esi, 0x40004
    L00527175:
            fld dword ptr [esp + 0x18]
            fcomp st(1)
            fnstsw ax
            test ah, 1
            je L00527188
            xor esi, 0x80008
    L00527188:
            fld dword ptr [esp + 0x1c]
            fcomp dword ptr [esp + 0x20]
            fnstsw ax
            test ah, 1
            jne L0052719d
            xor esi, 0x100010
    L0052719d:
            fld dword ptr [esp + 0x1c]
            fcomp st(1)
            fnstsw ax
            test ah, 1
            fstp st(0)
            je L005271b2
            xor esi, 0x200020
    L005271b2:
            test esi, 0xfff
            mov dword ptr [edi], esi
            jne L0052720e
            fld dword ptr [esp + 0x14]
            fmul dword ptr [v1db+0x4f4]
            fld dword ptr [esp + 0x18]
            fld dword ptr [esp + 0x1c]
            fxch st(2)
            fdiv dword ptr [esp + 0x20]
            fadd dword ptr [v1db+0x4ec]
            fxch st(2)
            fmul qword ptr [IID_IDirectInputDevice2A+0x470]
            fxch st(2)
            _emit 0xd9
        _emit 0x1b
            fmul dword ptr [v1db+0x4f8]
            fxch st(1)
            fdiv dword ptr [esp + 0x20]
            fxch st(1)
            fdiv dword ptr [esp + 0x20]
            fadd dword ptr [v1db+0x4f0]
            fstp dword ptr [ebx + 4]
            fld dword ptr [gScreenZOffset]
            fxch st(1)
            fsubp st(1), st(0)
            fstp dword ptr [ebx + 8]
    L0052720e:
            mov eax, dword ptr [esp + 0x10]
            add ebx, 0xc
            add ebp, 0xc
            add edi, 4
            dec eax
            mov dword ptr [esp + 0x10], eax
            jne L0052711d
    L00527226:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x84
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00527240
br_scalar C2_HOOK_CDECL BrZbDepthToScreenZ(br_uint_32 depth_z, br_camera* camera) {

    return BrFixedToScalar((br_int_32)BrFixedNeg(depth_z));
}

// FUNCTION: CARMA2_HW 0x00527260
br_uint_32 C2_HOOK_CDECL BrZbScreenZToDepth(br_scalar sz, br_camera* camera) {
    br_uint_32 depth;

    depth = BrScalarToFixed(sz) ^ 0x80000000;
    if (sz <= -32768.0f) {
        return 0;
    } else if (sz >= 32768.0f) {
        return -1;
    } else {
        return depth;
    }
}

// FUNCTION: CARMA2_HW 0x005272b0
br_scalar C2_HOOK_CDECL BrZsDepthToScreenZ(br_scalar depth_z, br_camera* camera) {
    br_scalar hither;
    br_scalar yon;

    hither = camera->hither_z;
    yon = camera->yon_z;

    if (depth_z <= hither) {
        return -32768.0f;
    }
    if (depth_z >= yon) {
        return 32768.0f;
    }
    return 2 * ((2 * depth_z - yon - hither) * 16384.0f / (yon - hither));
}

// FUNCTION: CARMA2_HW 0x00527320
br_scalar C2_NAKED C2_HOOK_CDECL BrZsScreenZToDepth(br_scalar sz, br_camera* camera) {
#ifndef CARPOCALYPSE2_MATCHING
    br_scalar hither;
    br_scalar yon;
    br_scalar depth;

    hither = camera->hither_z;
    yon = camera->yon_z;
    depth = ((yon + hither) + (sz / 2.0f) * (yon - hither) / 16384.0f) / 2.0f;
    if (sz <= -32768.0f) {
        return hither;
    } else if (sz >= 32768.0f) {
        return yon;
    }
    return depth;
#else
    __asm {
            push ecx
            mov eax, dword ptr [esp + 0xc]
            mov ecx, dword ptr [eax + 0xc]
            fld dword ptr [eax + 8]
            mov dword ptr [esp + 0xc], ecx
            fld dword ptr [esp + 0xc]
            fld dword ptr [esp + 8]
            fmul dword ptr [IID_IDirectInputDevice2A+0x490]
            fxch st(1)
            fsub st(0), st(2)
            fld dword ptr [esp + 0xc]
            fxch st(2)
            fmulp st(1), st(0)
            fxch st(1)
            fadd st(0), st(2)
            fld dword ptr [esp + 8]
            fxch st(2)
            fmul dword ptr [IID_IDirectInputDevice2A+0x494]
            fxch st(2)
            fcomp dword ptr [IID_IDirectInputDevice2A+0x480]
            faddp st(1), st(0)
            fnstsw ax
            fmul dword ptr [IID_IDirectInputDevice2A+0x490]
            test ah, 0x41
            _emit 0xd9
        _emit 0x5c
        _emit 0x24
        _emit 0x00
            jne L00527391
            fstp st(0)
            fld dword ptr [esp + 8]
            fcomp dword ptr [IID_IDirectInputDevice2A+0x484]
            fnstsw ax
            test ah, 1
            jne L0052738d
            fld dword ptr [esp + 0xc]
            pop ecx
            ret
    L0052738d:
            _emit 0xd9
        _emit 0x44
        _emit 0x24
        _emit 0x00
    L00527391:
            pop ecx
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x005273a0
br_scalar C2_NAKED C2_HOOK_CDECL BrScreenZToCamera(br_actor* camera, br_scalar sz) {
#ifndef CARPOCALYPSE2_MATCHING
    br_camera* data;
    br_scalar hither;
    br_scalar yon;

    data = camera->type_data;
    yon = data->yon_z;
    hither = data->hither_z;
    if (sz <= -32768.0f) {
        return -hither;
    }
    if (sz >= 32768.0f) {
        return -yon;
    }
    switch (data->type) {
    case BR_CAMERA_PARALLEL:
        return 0.5f * sz * (hither - yon) * 0.5f / 16384.0f - (yon + hither) * 0.5f;
    case BR_CAMERA_PERSPECTIVE_FOV:
        return 2.0f * ((yon * hither) / (0.5f * sz * (yon - hither) / 16384.0f - (yon + hither)));
    default:
        return 0.f;
    }
#else
    __asm {
            push ecx
            mov eax, dword ptr [esp + 8]
            fld dword ptr [esp + 0xc]
            mov ecx, dword ptr [eax + 0x5c]
            fcomp dword ptr [IID_IDirectInputDevice2A+0x480]
            mov eax, dword ptr [ecx + 0xc]
            mov edx, dword ptr [ecx + 8]
            mov dword ptr [esp + 8], eax
            mov dword ptr [esp], edx
            fnstsw ax
            test ah, 0x41
            je L005273cf
            _emit 0xd9
        _emit 0x44
        _emit 0x24
        _emit 0x00
            fchs
            pop ecx
            ret
    L005273cf:
            fld dword ptr [esp + 0xc]
            fcomp dword ptr [IID_IDirectInputDevice2A+0x484]
            fnstsw ax
            test ah, 1
            jne L005273e8
            fld dword ptr [esp + 8]
            fchs
            pop ecx
            ret
    L005273e8:
            xor eax, eax
            mov al, byte ptr [ecx + 4]
            sub eax, 0
            je L0052743b
            dec eax
            je L005273fd
            fld dword ptr [IID_IDirectInputDevice2A+0x468]
            pop ecx
            ret
    L005273fd:
            fld dword ptr [esp + 0xc]
            fld dword ptr [esp + 8]
            _emit 0xd8
        _emit 0x64
        _emit 0x24
        _emit 0x00
            fxch st(1)
            fmul dword ptr [IID_IDirectInputDevice2A+0x490]
            fld dword ptr [esp + 8]
            fxch st(1)
            fmulp st(2), st(0)
            _emit 0xd8
        _emit 0x44
        _emit 0x24
        _emit 0x00
            fld dword ptr [esp + 8]
            fxch st(2)
            fmul dword ptr [IID_IDirectInputDevice2A+0x494]
            fxch st(2)
            _emit 0xd8
        _emit 0x4c
        _emit 0x24
        _emit 0x00
            fxch st(1)
            fsubp st(2), st(0)
            fxch st(1)
            fdivp st(1), st(0)
            fadd st(0), st(0)
            pop ecx
            ret
    L0052743b:
            _emit 0xd9
        _emit 0x44
        _emit 0x24
        _emit 0x00
            fsub dword ptr [esp + 8]
            fld dword ptr [esp + 0xc]
            fmul dword ptr [IID_IDirectInputDevice2A+0x490]
            fld dword ptr [esp + 8]
            fxch st(2)
            fmul dword ptr [IID_IDirectInputDevice2A+0x490]
            fxch st(2)
            _emit 0xd8
        _emit 0x44
        _emit 0x24
        _emit 0x00
            fxch st(1)
            fmulp st(2), st(0)
            fxch st(1)
            fmul dword ptr [IID_IDirectInputDevice2A+0x494]
            fxch st(1)
            fmul dword ptr [IID_IDirectInputDevice2A+0x490]
            fsubp st(1), st(0)
            pop ecx
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00527480
void C2_NAKED C2_HOOK_CDECL BrScreenXYZToCamera(br_vector3* point, br_actor* camera, br_pixelmap* screen_buffer, br_int_16 x, br_int_16 y, br_scalar sz) {
#ifndef CARPOCALYPSE2_MATCHING
    br_scalar hx;
    br_scalar hy;
    br_scalar vz;
    br_camera* data;
    br_angle fov;
    br_scalar scale;


    data = camera->type_data;

    vz = BrScreenZToCamera(camera, sz);

    hx = (float)(screen_buffer->width - 2 * (x + screen_buffer->origin_x)) / (float)screen_buffer->width;
    hy = (float)(2 * (y + screen_buffer->origin_y) - screen_buffer->height) / (float)screen_buffer->height;

    switch (data->type) {
    case BR_CAMERA_PARALLEL:
        point->v[0] = -0.5f * hx * data->width * data->aspect;
        point->v[1] = -0.5f * hy * data->height / 2;
        point->v[2] = vz;
        break;
    case BR_CAMERA_PERSPECTIVE_FOV:
        fov=(br_angle)(data->field_of_view / 2);
        scale = BR_COS(fov) / BR_SIN(fov);
        point->v[0] = hx * data->aspect * vz / scale;
        point->v[1] = hy * vz / scale;
        point->v[2] = vz;
        break;
    default:
        break;
    }
#else
    __asm {
            mov ecx, dword ptr [esp + 0x18]
            mov eax, dword ptr [esp + 8]
            push esi
            push edi
            mov esi, dword ptr [eax + 0x5c]
            push ecx
            push eax
            call BrScreenZToCamera
            mov ecx, dword ptr [esp + 0x1c]
            xor eax, eax
            movsx edi, word ptr [esp + 0x20]
            fstp dword ptr [esp + 0x18]
            movsx edx, word ptr [ecx + 0x38]
            mov ax, word ptr [ecx + 0x34]
            add edx, edi
            shl edx, 1
            mov dword ptr [esp + 0x28], eax
            sub eax, edx
            mov dword ptr [esp + 0x20], eax
            xor eax, eax
            fild dword ptr [esp + 0x20]
            mov ax, word ptr [ecx + 0x36]
            add esp, 8
            movsx ecx, word ptr [ecx + 0x3a]
            fild dword ptr [esp + 0x20]
            movsx edx, word ptr [esp + 0x1c]
            fdivp st(1), st(0)
            add ecx, edx
            mov dword ptr [esp + 0x20], eax
            shl ecx, 1
            sub ecx, eax
            xor eax, eax
            mov dword ptr [esp + 0x18], ecx
            mov al, byte ptr [esi + 4]
            sub eax, 0
            fild dword ptr [esp + 0x18]
            fild dword ptr [esp + 0x20]
            fdivp st(1), st(0)
            fstp dword ptr [esp + 0x18]
            je L00527570
            dec eax
            jne L005275ae
            mov ax, word ptr [esi + 6]
            fld dword ptr [esi + 0x10]
            shr ax, 1
            and eax, 0xffff
            mov dword ptr [esp + 0x1c], eax
            mov eax, dword ptr [esp + 0xc]
            fild dword ptr [esp + 0x1c]
            fmul qword ptr [IID_IDirectInputDevice2A+0x498]
            fld dword ptr [esp + 0x10]
            fxch st(2)
            fmul st(0), st(3)
            fxch st(1)
            fstp st(3)
            fld st(2)
            fcos
            fxch st(3)
            fsin
            fxch st(3)
            fst dword ptr [esp + 0x1c]
            fxch st(3)
            fst dword ptr [esp + 0x1c]
            fdivp st(3), st(0)
            fmul dword ptr [esp + 0x10]
            fxch st(1)
            fmul dword ptr [esp + 0x18]
            fxch st(2)
            fstp dword ptr [esp + 0x18]
            fdiv dword ptr [esp + 0x18]
            fxch st(1)
            fdiv dword ptr [esp + 0x18]
            fld dword ptr [esp + 0x10]
            fxch st(2)
            _emit 0xd9
        _emit 0x18
            fstp dword ptr [eax + 4]
            fstp dword ptr [eax + 8]
            pop edi
            pop esi
            ret
    L00527570:
            fld dword ptr [esi + 0x14]
            fmul dword ptr [esi + 0x10]
            fxch st(1)
            fchs
            fmulp st(1), st(0)
            fld dword ptr [esp + 0x18]
            mov eax, dword ptr [esp + 0xc]
            fchs
            fxch st(1)
            fmul dword ptr [IID_IDirectInputDevice2A+0x490]
            _emit 0xd9
        _emit 0x18
            fld dword ptr [esi + 0x18]
            fxch st(1)
            fmulp st(1), st(0)
            fld dword ptr [esp + 0x10]
            fxch st(1)
            fmul dword ptr [IID_IDirectInputDevice2A+0x490]
            fxch st(1)
            fstp dword ptr [eax + 8]
            fstp dword ptr [eax + 4]
            pop edi
            pop esi
            ret
    L005275ae:
            pop edi
            pop esi
            fstp st(0)
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
    }
#endif
}
