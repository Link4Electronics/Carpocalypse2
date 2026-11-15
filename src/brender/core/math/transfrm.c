#include "transfrm.h"

#include "angles.h"
#include "matrix34.h"
#include "matrix4.h"
#include "quat.h"
#include "vector.h"

#include "c2_stdlib.h"
#include "c2_string.h"

#define M34 BR_TRANSFORM_MATRIX34
#define MLP BR_TRANSFORM_MATRIX34_LP
#define QUT BR_TRANSFORM_QUAT
#define EUL BR_TRANSFORM_EULER
#define LUP BR_TRANSFORM_LOOK_UP
#define TRA BR_TRANSFORM_TRANSLATION
#define IDT BR_TRANSFORM_IDENTITY

// GLOBAL: CARMA2_HW 0x0066cb68
br_uint_8 _CombineTransforms[7][7] = {

    /* M34 */ {
        M34,
        M34,
        M34,
        M34,
        M34,
        M34,
        M34,
    },
    /* MLP */ {
        M34,
        MLP,
        MLP,
        MLP,
        MLP,
        MLP,
        MLP,
    },
    /* QUT */ {
        M34,
        MLP,
        MLP,
        MLP,
        MLP,
        QUT,
        QUT,
    },
    /* EUL */ {
        M34,
        MLP,
        MLP,
        MLP,
        MLP,
        EUL,
        EUL,
    },
    /* LUP */ {
        M34,
        MLP,
        MLP,
        MLP,
        MLP,
        LUP,
        LUP,
    },
    /* TRA */ {
        M34,
        MLP,
        QUT,
        EUL,
        LUP,
        TRA,
        TRA,
    },
    /* IDT */ {
        M34,
        MLP,
        QUT,
        EUL,
        LUP,
        TRA,
        IDT,
    },
};

#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_CDECL BrMatrix34LPNormalise();
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern br_matrix34* C2_HOOK_CDECL BrEulerToMatrix34();
extern void C2_HOOK_CDECL BrMatrix34Identity();
extern br_euler* C2_HOOK_CDECL BrMatrix34ToEuler();
extern br_quat* C2_HOOK_CDECL BrMatrix34ToQuat();
extern void C2_HOOK_CDECL BrMatrix34Translate();
extern br_matrix34* C2_HOOK_CDECL BrQuatToMatrix34();
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern char gData_0058b894[];
extern char gDouble_0058b888[];
extern char gFloat_0058b880[];
extern char gFloat_0058b890[];
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern char gMatchData_0058b894[];
extern char gMatchDouble_0058b888[];
extern char gMatchFloat_0058b880[];
extern char gMatchFloat_0058b890[];
#endif
// FUNCTION: CARMA2_HW 0x00531870
void C2_NAKED C2_HOOK_CDECL BrTransformToMatrix34(br_matrix34* mat, const br_transform* xform) {
#ifndef CARPOCALYPSE2_MATCHING

    switch (xform->type) {
    case BR_TRANSFORM_MATRIX34:
    case BR_TRANSFORM_MATRIX34_LP:
        *mat = xform->t.mat;
        break;

    case BR_TRANSFORM_QUAT:

        BrQuatToMatrix34(mat, &xform->t.quat.q);

        mat->m[V_W][V_X] = xform->t.quat.t.v[V_X];
        mat->m[V_W][V_Y] = xform->t.quat.t.v[V_Y];
        mat->m[V_W][V_Z] = xform->t.quat.t.v[V_Z];
        break;

    case BR_TRANSFORM_EULER:

        BrEulerToMatrix34(mat, &xform->t.euler.e);

        mat->m[V_W][V_X] = xform->t.euler.t.v[V_X];
        mat->m[V_W][V_Y] = xform->t.euler.t.v[V_Y];
        mat->m[V_W][V_Z] = xform->t.euler.t.v[V_Z];

        break;

    case BR_TRANSFORM_TRANSLATION:
        BrMatrix34Translate(mat,
            xform->t.look_up.t.v[V_X],
            xform->t.look_up.t.v[V_Y],
            xform->t.look_up.t.v[V_Z]);

        break;

    case BR_TRANSFORM_LOOK_UP: {
        /*
			 * 1) Normalise Lookat vector to get Z component of matrix
			 * 2) Cross with up vector and normalise to get X component
			 * 3) Cross X & Z to get Y
			 */
        br_vector3 vx, vy, vz;

        BrVector3Normalise(&vz, &xform->t.look_up.look);
        BrVector3Negate(&vz, &vz);
        BrVector3Cross(&vx, &xform->t.look_up.up, &vz);
        BrVector3Normalise(&vx, &vx);
        BrVector3Cross(&vy, &vz, &vx);

        mat->m[V_X][V_X] = vx.v[V_X];
        mat->m[V_X][V_Y] = vx.v[V_Y];
        mat->m[V_X][V_Z] = vx.v[V_Z];
        mat->m[V_Y][V_X] = vy.v[V_X];
        mat->m[V_Y][V_Y] = vy.v[V_Y];
        mat->m[V_Y][V_Z] = vy.v[V_Z];
        mat->m[V_Z][V_X] = vz.v[V_X];
        mat->m[V_Z][V_Y] = vz.v[V_Y];
        mat->m[V_Z][V_Z] = vz.v[V_Z];

        mat->m[V_W][V_X] = xform->t.look_up.t.v[V_X];
        mat->m[V_W][V_Y] = xform->t.look_up.t.v[V_Y];
        mat->m[V_W][V_Z] = xform->t.look_up.t.v[V_Z];
    } break;

    case BR_TRANSFORM_IDENTITY:
        BrMatrix34Identity(mat);
        break;
    }
#else
    __asm {
            sub esp, 0x24
            xor eax, eax
            push esi
            mov esi, dword ptr [esp + 0x30]
            push edi
            mov ax, word ptr [esi]
            cmp eax, 6
            ja L00531b3c
            jmp dword ptr [eax*4 + 0x531b44]
            mov edi, dword ptr [esp + 0x30]
            add esi, 4
            mov ecx, 0xc
            rep movsd
            pop edi
            pop esi
            add esp, 0x24
            ret
            mov edi, dword ptr [esp + 0x30]
            lea eax, [esi + 4]
            push eax
            push edi
            call BrQuatToMatrix34
            mov ecx, dword ptr [esi + 0x28]
            add esp, 8
            mov dword ptr [edi + 0x24], ecx
            mov edx, dword ptr [esi + 0x2c]
            mov dword ptr [edi + 0x28], edx
            mov eax, dword ptr [esi + 0x30]
            mov dword ptr [edi + 0x2c], eax
            pop edi
            pop esi
            add esp, 0x24
            ret
            mov edi, dword ptr [esp + 0x30]
            lea ecx, [esi + 4]
            push ecx
            push edi
            call BrEulerToMatrix34
            mov edx, dword ptr [esi + 0x28]
            add esp, 8
            mov dword ptr [edi + 0x24], edx
            mov eax, dword ptr [esi + 0x2c]
            mov dword ptr [edi + 0x28], eax
            mov ecx, dword ptr [esi + 0x30]
            mov dword ptr [edi + 0x2c], ecx
            pop edi
            pop esi
            add esp, 0x24
            ret
            mov edx, dword ptr [esi + 0x30]
            mov eax, dword ptr [esi + 0x2c]
            mov ecx, dword ptr [esi + 0x28]
            push edx
            mov edx, dword ptr [esp + 0x34]
            push eax
            push ecx
            push edx
            call BrMatrix34Translate
            add esp, 0x10
            pop edi
            pop esi
            add esp, 0x24
            ret
            fld dword ptr [esi + 4]
            fld dword ptr [esi + 8]
            fld dword ptr [esi + 0xc]
            fxch st(2)
            fst dword ptr [esp + 0x34]
            fld st(1)
            fxch st(1)
            fmul dword ptr [esp + 0x34]
            fxch st(1)
            fmul st(0), st(2)
            fxch st(3)
            fst dword ptr [esp + 0x34]
            fmul dword ptr [esp + 0x34]
            fxch st(3)
            faddp st(1), st(0)
            fxch st(2)
            faddp st(2), st(0)
            fxch st(1)
            fsqrt
            fstp st(1)
            fcom dword ptr [gMatchFloat_0058b880]
            fnstsw ax
            test ah, 0x41
            jne L0053197c
            fdivr qword ptr [gMatchDouble_0058b888]
            fld dword ptr [esi + 4]
            fld dword ptr [esi + 8]
            fld dword ptr [esi + 0xc]
            fxch st(2)
            fxch st(1)
            fxch st(1)
            fmul st(0), st(3)
            fxch st(1)
            fmul st(0), st(3)
            fxch st(2)
            fmul st(0), st(3)
            fxch st(1)
            fstp st(3)
            fstp dword ptr [esp + 0x1c]
            jmp L00531992
    L0053197c:
            fstp st(0)
            fld dword ptr [gMatchFloat_0058b890]
            fld dword ptr [gMatchData_0058b894]
            mov dword ptr [esp + 0x1c], 0
    L00531992:
            fld st(0)
            fld dword ptr [esp + 0x1c]
            fld dword ptr [esi + 0x14]
            fld dword ptr [esi + 0x18]
            fld dword ptr [esi + 0x18]
            fxch st(6)
            fchs
            fstp dword ptr [esp + 0x14]
            fld dword ptr [esi + 0x10]
            fxch st(4)
            fchs
            fstp dword ptr [esp + 0x18]
            fxch st(2)
            fchs
            fld dword ptr [esi + 0x10]
            fxch st(1)
            fstp dword ptr [esp + 0x1c]
            fld dword ptr [esi + 0x14]
            fxch st(2)
            fmul dword ptr [esp + 0x1c]
            fxch st(3)
            fmul dword ptr [esp + 0x18]
            fsubp st(3), st(0)
            fxch st(5)
            fmul dword ptr [esp + 0x14]
            fxch st(2)
            fst dword ptr [esp + 8]
            fmul dword ptr [esp + 8]
            fxch st(3)
            fmul dword ptr [esp + 0x1c]
            fxch st(5)
            fmul dword ptr [esp + 0x18]
            fxch st(1)
            fmul dword ptr [esp + 0x14]
            fxch st(5)
            fsubp st(2), st(0)
            fxch st(1)
            fst dword ptr [esp + 0xc]
            fxch st(4)
            fsubp st(1), st(0)
            fxch st(3)
            fmul dword ptr [esp + 0xc]
            fxch st(3)
            fst dword ptr [esp + 0x10]
            fmul dword ptr [esp + 0x10]
            fxch st(3)
            faddp st(1), st(0)
            fxch st(2)
            faddp st(2), st(0)
            fxch st(1)
            fsqrt
            fstp dword ptr [esp + 0x34]
            fstp st(0)
            fld dword ptr [esp + 0x34]
            fcom dword ptr [gMatchFloat_0058b880]
            fnstsw ax
            test ah, 0x41
            jne L00531a63
            fdivr qword ptr [gMatchDouble_0058b888]
            fld st(0)
            fld st(1)
            fxch st(1)
            fmul dword ptr [esp + 8]
            fxch st(1)
            fmul dword ptr [esp + 0xc]
            fxch st(2)
            fmul dword ptr [esp + 0x10]
            fxch st(1)
            fstp dword ptr [esp + 8]
            fxch st(1)
            fstp dword ptr [esp + 0xc]
            fstp dword ptr [esp + 0x10]
            jmp L00531a7d
    L00531a63:
            fstp st(0)
            mov dword ptr [esp + 8], 0x3f800000
            mov dword ptr [esp + 0xc], 0
            mov dword ptr [esp + 0x10], 0
    L00531a7d:
            fld dword ptr [esp + 0x18]
            fld dword ptr [esp + 0x1c]
            fld dword ptr [esp + 0x1c]
            fld dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x18]
            fxch st(5)
            fmul dword ptr [esp + 0x10]
            fxch st(4)
            fmul dword ptr [esp + 0xc]
            mov eax, dword ptr [esp + 0x30]
            fsubp st(4), st(0)
            fxch st(2)
            fmul dword ptr [esp + 8]
            fld dword ptr [esp + 8]
            fld st(4)
            fxch st(3)
            fmul dword ptr [esp + 0x10]
            fsubp st(2), st(0)
            fxch st(3)
            fmul dword ptr [esp + 0xc]
            fxch st(3)
            _emit 0xd9
        _emit 0x18
            fstp dword ptr [esp + 0x24]
            fld dword ptr [esp + 0x24]
            fxch st(4)
            fmul dword ptr [esp + 8]
            fxch st(4)
            fstp dword ptr [eax + 0x10]
            fld dword ptr [esp + 0xc]
            fxch st(4)
            fsubp st(2), st(0)
            fxch st(3)
            fstp dword ptr [eax + 4]
            fld dword ptr [esp + 0x10]
            fxch st(1)
            fst dword ptr [esp + 0x28]
            fxch st(1)
            fstp dword ptr [eax + 8]
            fstp dword ptr [eax + 0x14]
            fld dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x18]
            fld dword ptr [esp + 0x1c]
            fxch st(4)
            fstp dword ptr [eax + 0xc]
            fxch st(1)
            fstp dword ptr [eax + 0x18]
            fstp dword ptr [eax + 0x1c]
            fxch st(1)
            fstp dword ptr [eax + 0x20]
            mov ecx, dword ptr [esi + 0x28]
            mov dword ptr [eax + 0x24], ecx
            mov edx, dword ptr [esi + 0x2c]
            mov dword ptr [eax + 0x28], edx
            mov ecx, dword ptr [esi + 0x30]
            fstp st(0)
            mov dword ptr [eax + 0x2c], ecx
            pop edi
            pop esi
            add esp, 0x24
            ret
            mov edx, dword ptr [esp + 0x30]
            push edx
            call BrMatrix34Identity
            add esp, 4
    L00531b3c:
            pop edi
            pop esi
            add esp, 0x24
            ret
            mov edi, edi
            _emit 0x8e
            _emit 0x18
            _emit 0x53
            _emit 0x00
            _emit 0x8e
            _emit 0x18
            _emit 0x53
            _emit 0x00
            _emit 0xa2
            _emit 0x18
            _emit 0x53
            _emit 0x00
            _emit 0xcb
            _emit 0x18
            _emit 0x53
            _emit 0x00
            _emit 0x13
            _emit 0x19
            _emit 0x53
            _emit 0x00
            _emit 0xf4
            _emit 0x18
            _emit 0x53
            _emit 0x00
            _emit 0x2f
            _emit 0x1b
            _emit 0x53
            _emit 0x00
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00531b60
void C2_HOOK_CDECL BrMatrix34PreTransform(br_matrix34* mat, const br_transform* xform) {
    br_matrix34 tmp;

    if (xform->type == BR_TRANSFORM_IDENTITY) {
        return;
    }

    BrTransformToMatrix34(&tmp, xform);
    BrMatrix34Pre(mat, &tmp);
}

// FUNCTION: CARMA2_HW 0x00531ba0
void C2_HOOK_CDECL BrMatrix34PostTransform(br_matrix34* mat, const br_transform* xform) {
    br_matrix34 tmp;

    if (xform->type == BR_TRANSFORM_IDENTITY) {
        return;
    }

    BrTransformToMatrix34(&tmp, xform);
    BrMatrix34Post(mat, &tmp);
}

// FUNCTION: CARMA2_HW 0x00531be0
void C2_HOOK_CDECL BrMatrix4PreTransform(br_matrix4* mat, const br_transform* xform) {
    br_matrix34 tmp;

    if (xform->type != BR_TRANSFORM_IDENTITY) {
        BrTransformToMatrix34(&tmp, xform);
        BrMatrix4Pre34(mat, &tmp);
    }
}

// FUNCTION: CARMA2_HW 0x00531c20
void C2_NAKED C2_HOOK_CDECL BrMatrix34ToTransform(br_transform* xform, const br_matrix34* mat) {
#ifndef CARPOCALYPSE2_MATCHING

    switch (xform->type) {
    case BR_TRANSFORM_MATRIX34:
        memcpy(&xform->t.mat, mat, sizeof(br_matrix34));
        break;
    case BR_TRANSFORM_MATRIX34_LP:
        memcpy(&xform->t.mat, mat, sizeof(br_matrix34));
        BrMatrix34LPNormalise(&xform->t.mat, &xform->t.mat);
        break;
    case BR_TRANSFORM_QUAT:
        BrMatrix34ToQuat(&xform->t.quat.q, mat);
        BrVector3Copy(&xform->t.quat.t, (br_vector3*)mat->m[3]);
        break;
    case BR_TRANSFORM_EULER:
        BrMatrix34ToEuler(&xform->t.euler.e, mat);
        BrVector3Copy(&xform->t.quat.t, (br_vector3*)mat->m[3]);
        break;
    case BR_TRANSFORM_LOOK_UP:
        BrVector3Negate(&xform->t.look_up.look, (br_vector3*)mat->m[2]);
        if (xform->t.look_up.up.v[0] == 0.f && xform->t.look_up.up.v[1] == 0.f && xform->t.look_up.up.v[2] == 0.f) {
            BrVector3Set(&xform->t.look_up.up, 0.f, 1.f, 0.f);
        }
        BrVector3Copy(&xform->t.look_up.t, (br_vector3*)mat->m[3]);
        break;
    case BR_TRANSFORM_TRANSLATION:
        BrVector3Copy(&xform->t.quat.t, (br_vector3*)mat->m[3]);
        break;
    default:
        abort();
    }
#else
    __asm {
            push esi
            mov esi, dword ptr [esp + 8]
            xor eax, eax
            push edi
            mov ax, word ptr [esi]
            cmp eax, 5
            ja L00531d4d
            jmp dword ptr [eax*4 + 0x531d50]
            lea edi, [esi + 4]
            mov esi, dword ptr [esp + 0x10]
            mov ecx, 0xc
            rep movsd
            pop edi
            pop esi
            ret
            lea eax, [esi + 4]
            mov esi, dword ptr [esp + 0x10]
            mov ecx, 0xc
            mov edi, eax
            push eax
            push eax
            rep movsd
            call BrMatrix34LPNormalise
            add esp, 8
            pop edi
            pop esi
            ret
            mov edi, dword ptr [esp + 0x10]
            lea eax, [esi + 4]
            push edi
            push eax
            call BrMatrix34ToQuat
            mov ecx, dword ptr [edi + 0x24]
            add esp, 8
            mov dword ptr [esi + 0x28], ecx
            mov edx, dword ptr [edi + 0x28]
            mov dword ptr [esi + 0x2c], edx
            mov eax, dword ptr [edi + 0x2c]
            mov dword ptr [esi + 0x30], eax
            pop edi
            pop esi
            ret
            mov edi, dword ptr [esp + 0x10]
            lea ecx, [esi + 4]
            push edi
            push ecx
            call BrMatrix34ToEuler
            mov edx, dword ptr [edi + 0x24]
            add esp, 8
            mov dword ptr [esi + 0x28], edx
            mov eax, dword ptr [edi + 0x28]
            mov dword ptr [esi + 0x2c], eax
            mov ecx, dword ptr [edi + 0x2c]
            mov dword ptr [esi + 0x30], ecx
            pop edi
            pop esi
            ret
            mov eax, dword ptr [esp + 0x10]
            mov edx, dword ptr [eax + 0x24]
            mov dword ptr [esi + 0x28], edx
            mov ecx, dword ptr [eax + 0x28]
            mov dword ptr [esi + 0x2c], ecx
            mov edx, dword ptr [eax + 0x2c]
            mov dword ptr [esi + 0x30], edx
            pop edi
            pop esi
            ret
            mov ecx, dword ptr [esp + 0x10]
            mov eax, dword ptr [ecx + 0x18]
            mov dword ptr [esi + 4], eax
            mov edx, dword ptr [ecx + 0x1c]
            fld dword ptr [esi + 4]
            mov dword ptr [esi + 8], edx
            mov eax, dword ptr [ecx + 0x20]
            fchs
            fstp dword ptr [esi + 4]
            fld dword ptr [esi + 8]
            fchs
            fstp dword ptr [esi + 8]
            mov dword ptr [esi + 0xc], eax
            fld dword ptr [esi + 0xc]
            fchs
            fstp dword ptr [esi + 0xc]
            fld dword ptr [esi + 0x10]
            fcomp dword ptr [gMatchData_0058b894]
            fnstsw ax
            test ah, 0x40
            je L00531d3b
            fld dword ptr [esi + 0x14]
            fcomp dword ptr [gMatchData_0058b894]
            fnstsw ax
            test ah, 0x40
            je L00531d3b
            fld dword ptr [esi + 0x18]
            fcomp dword ptr [gMatchData_0058b894]
            fnstsw ax
            test ah, 0x40
            je L00531d3b
            xor eax, eax
            mov dword ptr [esi + 0x14], 0x3f800000
            mov dword ptr [esi + 0x10], eax
            mov dword ptr [esi + 0x18], eax
    L00531d3b:
            mov edx, dword ptr [ecx + 0x24]
            mov dword ptr [esi + 0x28], edx
            mov eax, dword ptr [ecx + 0x28]
            mov dword ptr [esi + 0x2c], eax
            mov ecx, dword ptr [ecx + 0x2c]
            mov dword ptr [esi + 0x30], ecx
    L00531d4d:
            pop edi
            pop esi
            ret
            _emit 0x3b
            _emit 0x1c
            _emit 0x53
            _emit 0x00
            _emit 0x4c
            _emit 0x1c
            _emit 0x53
            _emit 0x00
            _emit 0x69
            _emit 0x1c
            _emit 0x53
            _emit 0x00
            _emit 0x8f
            _emit 0x1c
            _emit 0x53
            _emit 0x00
            _emit 0xce
            _emit 0x1c
            _emit 0x53
            _emit 0x00
            _emit 0xb5
            _emit 0x1c
            _emit 0x53
            _emit 0x00
            _emit 0x90
            _emit 0x90
            _emit 0x90
            _emit 0x90
            _emit 0x90
            _emit 0x90
            _emit 0x90
            _emit 0x90
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00531d70
void C2_NAKED C2_HOOK_CDECL BrTransformToTransform(br_transform* dest, const br_transform* src) {
#ifndef CARPOCALYPSE2_MATCHING
    br_matrix34 temp;

    if (src->type == dest->type) {
        memcpy(dest, src, sizeof(br_transform));
        return;
    }
    switch (dest->type) {
    case BR_TRANSFORM_MATRIX34:
        BrTransformToMatrix34(&dest->t.mat, src);
        break;
    case BR_TRANSFORM_MATRIX34_LP:
        BrTransformToMatrix34(&dest->t.mat, src);
        BrMatrix34LPNormalise(&dest->t.mat, &dest->t.mat);
        break;
    default:
        BrTransformToMatrix34(&temp, src);
        BrMatrix34ToTransform(dest, &temp);
    }
#else
    __asm {
            sub esp, 0x30
            push esi
            mov esi, dword ptr [esp + 0x3c]
            push edi
            mov edi, dword ptr [esp + 0x3c]
            mov ax, word ptr [edi]
            cmp ax, word ptr [esi]
            jne L00531d92
            mov ecx, 0xd
            rep movsd
            pop edi
            pop esi
            add esp, 0x30
            ret
    L00531d92:
            cmp ax, 1
            ja L00531dbb
            add edi, 4
            push esi
            push edi
            call BrTransformToMatrix34
            add esp, 8
            cmp word ptr [esi], 0
            jne L00531dd7
            push edi
            push edi
            call BrMatrix34LPNormalise
            add esp, 8
            pop edi
            pop esi
            add esp, 0x30
            ret
    L00531dbb:
            lea eax, [esp + 8]
            push esi
            push eax
            call BrTransformToMatrix34
            add esp, 8
            lea ecx, [esp + 8]
            push ecx
            push edi
            call BrMatrix34ToTransform
            add esp, 8
    L00531dd7:
            pop edi
            pop esi
            add esp, 0x30
            ret
            nop
            nop
            nop
    }
#endif
}
