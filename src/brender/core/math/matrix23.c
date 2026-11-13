#include "matrix23.h"

#include "c2_math.h"

#define A(x, y) A->m[x][y]
#define B(x, y) B->m[x][y]
#define C(x, y) C->m[x][y]
#define M(x, y) mat->m[x][y]


// GLOBAL: CARMA2_HW 0x006b0a20
br_matrix23 mattmp1_23;

// GLOBAL: CARMA2_HW 0x006b0a38
br_matrix23 mattmp2_23;

#ifdef CARPOCALYPSE2_MATCHING
extern char IID_IDirectInputDevice2A[];
#endif
// FUNCTION: CARMA2_HW 0x00534ae0
void C2_HOOK_CDECL BrMatrix23Copy(br_matrix23* A, const br_matrix23* B) {

    A(0, 0) = B(0, 0);
    A(0, 1) = B(0, 1);
    A(1, 0) = B(1, 0);
    A(1, 1) = B(1, 1);
    A(2, 0) = B(2, 0);
    A(2, 1) = B(2, 1);
}

// FUNCTION: CARMA2_HW 0x00534b10
void C2_HOOK_CDECL BrMatrix23Mul(br_matrix23* A, const br_matrix23* B, const br_matrix23* C) {

    A(0, 0) = BR_MAC2(B(0, 0), C(0, 0), B(0, 1), C(1, 0));
    A(0, 1) = BR_MAC2(B(0, 0), C(0, 1), B(0, 1), C(1, 1));
    A(1, 0) = BR_MAC2(B(1, 0), C(0, 0), B(1, 1), C(1, 0));
    A(1, 1) = BR_MAC2(B(1, 0), C(0, 1), B(1, 1), C(1, 1));
    A(2, 0) = BR_MAC2(B(2, 0), C(0, 0), B(2, 1), C(1, 0)) + C(2, 0);
    A(2, 1) = BR_MAC2(B(2, 0), C(0, 1), B(2, 1), C(1, 1)) + C(2, 1);
}

// FUNCTION: CARMA2_HW 0x00534ba0
void C2_HOOK_CDECL BrMatrix23Identity(br_matrix23* mat) {

    M(0, 0) = 1.f;
    M(0, 1) = 0.f;
    M(1, 0) = 0.f;
    M(1, 1) = 1.f;
    M(2, 0) = 0.f;
    M(2, 1) = 0.f;
}

// FUNCTION: CARMA2_HW 0x00534bc0
void C2_HOOK_CDECL BrMatrix23Rotate(br_matrix23* mat, br_angle rz) {
    br_scalar s;
    br_scalar c;

    c = BR_COS(rz);
    s = BR_SIN(rz);

    M(0, 0) = c;
    M(0, 1) = s;
    M(1, 0) = -s;
    M(1, 1) = c;
    M(2, 0) = BR_SCALAR(0.f);
    M(2, 1) = BR_SCALAR(0.f);
}

// FUNCTION: CARMA2_HW 0x00534c20
void C2_HOOK_CDECL BrMatrix23Translate(br_matrix23* mat, br_scalar dx, br_scalar dy) {

    M(0, 0) = BR_SCALAR(1.f);
    M(0, 1) = BR_SCALAR(0.f);
    M(1, 0) = BR_SCALAR(0.f);
    M(1, 1) = BR_SCALAR(1.f);
    M(2, 0) = dx;
    M(2, 1) = dy;
}

// FUNCTION: CARMA2_HW 0x00534c50
void C2_HOOK_CDECL BrMatrix23Scale(br_matrix23* mat, br_scalar sx, br_scalar sy) {

    M(0, 0) = sx;
    M(0, 1) = BR_SCALAR(0.f);
    M(1, 0) = BR_SCALAR(0.f);
    M(1, 1) = sy;
    M(2, 0) = BR_SCALAR(0.f);
    M(2, 1) = BR_SCALAR(0.f);
}

// FUNCTION: CARMA2_HW 0x00534c70
void C2_HOOK_CDECL BrMatrix23ShearX(br_matrix23* mat, br_scalar sy) {

    M(0, 0) = BR_SCALAR(1.f);
    M(0, 1) = sy;
    M(1, 0) = BR_SCALAR(0.f);
    M(1, 1) = BR_SCALAR(1.f);
    M(2, 0) = BR_SCALAR(0.f);
    M(2, 1) = BR_SCALAR(0.f);
}

// FUNCTION: CARMA2_HW 0x00534ca0
void C2_HOOK_CDECL BrMatrix23ShearY(br_matrix23* mat, br_scalar sx) {

    M(0, 0) = BR_SCALAR(1.f);
    M(0, 1) = BR_SCALAR(0.f);
    M(1, 0) = sx;
    M(1, 1) = BR_SCALAR(1.f);
    M(2, 0) = BR_SCALAR(0.f);
    M(2, 1) = BR_SCALAR(0.f);
}

// FUNCTION: CARMA2_HW 0x00534cd0
br_scalar C2_NAKED C2_HOOK_CDECL BrMatrix23Inverse(br_matrix23* B, const br_matrix23* A) {
#ifndef CARPOCALYPSE2_MATCHING
    br_scalar det;
    br_scalar idet;
    br_scalar pos;
    br_scalar neg;

    idet = A(0, 0) * A(1, 1);
    if (idet < BR_SCALAR(0.f)) {
        pos = BR_SCALAR(0.f);
        neg = idet;
    } else {
        pos = idet;
        neg = BR_SCALAR(0.f);
    }
    idet = -A(0, 1) * A(1, 0);
    if (idet < BR_SCALAR(0.f)) {
        neg = neg + idet;
    } else {
        pos = pos + idet;
    }
    det = pos + neg;

    if (fabs(det) < 2.384186e-07f) {
        return BR_SCALAR(0.f);
    }
    idet = 1 / det;
    B(0, 0) = A(1, 1) * idet;
    B(0, 1) = -A(0, 1) * idet;
    B(1, 0) = -A(1, 0) * idet;
    B(1, 1) = A(0, 0) * idet;
    B(2, 0) = BR_MAC2(A(1, 0), A(2, 1), -A(2, 0), A(1, 1)) * idet;
    B(2, 1) = BR_MAC2(-A(2, 1), A(0, 0), A(0, 1), A(2, 0)) * idet;
    return det;
#else
    __asm {
            push ecx
            mov ecx, dword ptr [esp + 0xc]
            fld dword ptr [ecx + 0xc]
            _emit 0xd8
        _emit 0x09
            fld dword ptr [IID_IDirectInputDevice2A+0x560]
            fld dword ptr [IID_IDirectInputDevice2A+0x560]
            fxch st(2)
            fst dword ptr [esp + 0xc]
            fcomp dword ptr [IID_IDirectInputDevice2A+0x560]
            _emit 0xd9
        _emit 0x5c
        _emit 0x24
        _emit 0x00
            fnstsw ax
            test ah, 1
            jne L00534d03
            fld dword ptr [esp + 0xc]
            jmp L00534d0d
    L00534d03:
            fstp st(0)
            fld dword ptr [esp + 0xc]
            _emit 0xd9
        _emit 0x44
        _emit 0x24
        _emit 0x00
    L00534d0d:
            fld dword ptr [ecx + 8]
            fchs
            fmul dword ptr [ecx + 4]
            fcom dword ptr [IID_IDirectInputDevice2A+0x560]
            fnstsw ax
            test ah, 1
            jne L00534d2a
            fxch st(1)
            fadd st(0), st(1)
            fxch st(1)
            jmp L00534d30
    L00534d2a:
            fxch st(2)
            fadd st(0), st(2)
            fxch st(2)
    L00534d30:
            fstp st(0)
            fld st(1)
            fadd st(0), st(1)
            fst dword ptr [esp + 0xc]
            fabs
            fcomp dword ptr [IID_IDirectInputDevice2A+0x564]
            fnstsw ax
            test ah, 0x41
            je L00534d55
            fstp st(0)
            fstp st(0)
            fld dword ptr [IID_IDirectInputDevice2A+0x560]
            pop ecx
            ret
    L00534d55:
            fsub st(0), st(1)
            fdivr dword ptr [esp + 0xc]
            fcomp dword ptr [IID_IDirectInputDevice2A+0x568]
            fnstsw ax
            test ah, 1
            fstp st(0)
            je L00534d72
            fld dword ptr [IID_IDirectInputDevice2A+0x560]
            pop ecx
            ret
    L00534d72:
            fld dword ptr [esp + 0xc]
            fdivr qword ptr [IID_IDirectInputDevice2A+0x570]
            mov eax, dword ptr [esp + 8]
            fld dword ptr [ecx + 0xc]
            fmul st(0), st(1)
            fld st(1)
            fld dword ptr [esp + 0xc]
            fxch st(2)
            _emit 0xd9
        _emit 0x18
            fld dword ptr [ecx + 4]
            fmul st(0), st(2)
            fchs
            fstp dword ptr [eax + 4]
            fld dword ptr [ecx + 8]
            fmul st(0), st(2)
            fchs
            fstp dword ptr [eax + 8]
            _emit 0xd8
        _emit 0x09
            fstp dword ptr [eax + 0xc]
            fld dword ptr [ecx + 0x14]
            fld dword ptr [ecx + 8]
            fld dword ptr [ecx + 0x10]
            fxch st(1)
            fmulp st(2), st(0)
            fmul dword ptr [ecx + 0xc]
            fsubp st(1), st(0)
            fmul st(0), st(1)
            fstp dword ptr [eax + 0x10]
            fld dword ptr [ecx + 0x10]
            fld dword ptr [ecx + 4]
            fld dword ptr [ecx + 0x14]
            _emit 0xd8
        _emit 0x09
            fxch st(1)
            fmulp st(2), st(0)
            fsubp st(1), st(0)
            fmul st(0), st(1)
            fstp dword ptr [eax + 0x14]
            fstp st(0)
            pop ecx
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00534de0
void C2_NAKED C2_HOOK_CDECL BrMatrix23LPInverse(br_matrix23* B, const br_matrix23* A) {
#ifndef CARPOCALYPSE2_MATCHING

    B(0, 0) = A(1, 1);
    B(0, 1) = -A(0, 1);
    B(1, 0) = -A(1, 0);
    B(1, 1) = A(0, 0);
    B(2, 0) = BR_MAC2(A(1, 0), A(2, 1), -A(2, 0), A(1, 1));
    B(2, 1) = BR_MAC2(-A(2, 1), A(0, 0), A(0, 1), A(2, 0));
#else
    __asm {
            mov eax, dword ptr [esp + 8]
            mov ecx, dword ptr [esp + 4]
            mov edx, dword ptr [eax + 0xc]
            mov dword ptr [ecx], edx
            fld dword ptr [eax + 4]
            fchs
            fstp dword ptr [ecx + 4]
            fld dword ptr [eax + 8]
            fchs
            fstp dword ptr [ecx + 8]
            mov edx, dword ptr [eax]
            mov dword ptr [ecx + 0xc], edx
            fld dword ptr [eax + 8]
            fld dword ptr [eax + 0x14]
            fld dword ptr [eax + 0xc]
            fxch st(1)
            fmulp st(2), st(0)
            fmul dword ptr [eax + 0x10]
            fsubp st(1), st(0)
            fstp dword ptr [ecx + 0x10]
            fld dword ptr [eax + 4]
            fld dword ptr [eax + 0x10]
            fld dword ptr [eax + 0x14]
            _emit 0xd8
        _emit 0x08
            fxch st(1)
            fmulp st(2), st(0)
            fsubp st(1), st(0)
            fstp dword ptr [ecx + 0x14]
            ret
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00534e30
void C2_NAKED C2_HOOK_CDECL BrMatrix23LPNormalise(br_matrix23* A, const br_matrix23* B) {
#ifndef CARPOCALYPSE2_MATCHING

    br_scalar norm = (br_scalar)sqrt(BR_MAC2(B(1, 0), B(1, 0), B(1, 1), B(1,1)));
    if (norm < 2.384186e-07f) {
        A(1, 0) = BR_SCALAR(1.f);
        A(1, 1) = BR_SCALAR(0.f);
    } else {
        A(1, 0) = B(1, 0) / norm;
        A(1, 1) = B(1, 1) / norm;
    }
    A(0, 0) = A(1, 1);
    A(0, 1) = -A(1, 0);
    A(2, 0) = B(2, 0);
    A(2, 1) = B(2, 1);
#else
    __asm {
            mov ecx, dword ptr [esp + 8]
            fld dword ptr [ecx + 0xc]
            fld dword ptr [ecx + 8]
            fst dword ptr [esp + 8]
            fld st(1)
            fxch st(1)
            fmul dword ptr [esp + 8]
            fxch st(1)
            fmul st(0), st(2)
            faddp st(1), st(0)
            fsqrt
            fstp st(1)
            fcom dword ptr [IID_IDirectInputDevice2A+0x564]
            fnstsw ax
            test ah, 0x41
            jne L00534e7b
            fdivr qword ptr [IID_IDirectInputDevice2A+0x570]
            mov eax, dword ptr [esp + 4]
            fld dword ptr [ecx + 8]
            fmul st(0), st(1)
            fstp dword ptr [eax + 8]
            fld dword ptr [ecx + 0xc]
            fmul st(0), st(1)
            fstp dword ptr [eax + 0xc]
            fstp st(0)
            jmp L00534e8f
    L00534e7b:
            mov eax, dword ptr [esp + 4]
            fstp st(0)
            mov dword ptr [eax + 8], 0x3f800000
            mov dword ptr [eax + 0xc], 0
    L00534e8f:
            fld dword ptr [eax + 8]
            mov edx, dword ptr [eax + 0xc]
            fchs
            fstp dword ptr [eax + 4]
            mov dword ptr [eax], edx
            mov edx, dword ptr [ecx + 0x10]
            mov dword ptr [eax + 0x10], edx
            mov ecx, dword ptr [ecx + 0x14]
            mov dword ptr [eax + 0x14], ecx
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00534eb0
void C2_NAKED C2_HOOK_CDECL BrMatrix23ApplyP(br_vector2* A, const br_vector2* B, const br_matrix23* C) {
#ifndef CARPOCALYPSE2_MATCHING

    A->v[0] = BR_MAC2(B->v[0], C(0, 0), B->v[1], C(1, 0)) + C(2, 0);
    A->v[1] = BR_MAC2(B->v[0], C(0, 1), B->v[1], C(1, 1)) + C(2, 1);
#else
    __asm {
            mov eax, dword ptr [esp + 0xc]
            mov ecx, dword ptr [esp + 8]
            mov edx, dword ptr [esp + 4]
            fld dword ptr [eax + 8]
            _emit 0xd9
        _emit 0x00
            fld dword ptr [ecx + 4]
            fxch st(1)
            _emit 0xd8
        _emit 0x09
            fxch st(1)
            fmulp st(2), st(0)
            faddp st(1), st(0)
            fadd dword ptr [eax + 0x10]
            _emit 0xd9
        _emit 0x1a
            fld dword ptr [eax + 0xc]
            fld dword ptr [ecx + 4]
            fld dword ptr [eax + 4]
            _emit 0xd8
        _emit 0x09
            fxch st(1)
            fmulp st(2), st(0)
            faddp st(1), st(0)
            fadd dword ptr [eax + 0x14]
            fstp dword ptr [edx + 4]
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00534ef0
void C2_NAKED C2_HOOK_CDECL BrMatrix23ApplyV(br_vector2* A, const br_vector2* B, const br_matrix23* C) {
#ifndef CARPOCALYPSE2_MATCHING

    A->v[0] = BR_MAC2(B->v[0], C(0, 0), B->v[1], C(1, 0));
    A->v[1] = BR_MAC2(B->v[0], C(0, 1), B->v[1], C(1, 1));
#else
    __asm {
            mov eax, dword ptr [esp + 0xc]
            mov ecx, dword ptr [esp + 8]
            mov edx, dword ptr [esp + 4]
            fld dword ptr [eax + 8]
            _emit 0xd9
        _emit 0x00
            fld dword ptr [ecx + 4]
            fxch st(1)
            _emit 0xd8
        _emit 0x09
            fxch st(1)
            fmulp st(2), st(0)
            faddp st(1), st(0)
            _emit 0xd9
        _emit 0x1a
            fld dword ptr [eax + 0xc]
            fld dword ptr [ecx + 4]
            fld dword ptr [eax + 4]
            _emit 0xd8
        _emit 0x09
            fxch st(1)
            fmulp st(2), st(0)
            faddp st(1), st(0)
            fstp dword ptr [edx + 4]
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00534f30
void C2_NAKED C2_HOOK_CDECL BrMatrix23TApplyP(br_vector2* A, const br_vector2* B, const br_matrix23* C) {
#ifndef CARPOCALYPSE2_MATCHING

    A->v[0] = BR_MAC2(B->v[0], C(0, 0), B->v[1], C(0, 1));
    A->v[1] = BR_MAC2(B->v[0], C(1, 0), B->v[1], C(1, 1));
#else
    __asm {
            mov eax, dword ptr [esp + 0xc]
            mov ecx, dword ptr [esp + 8]
            mov edx, dword ptr [esp + 4]
            fld dword ptr [eax + 4]
            _emit 0xd9
        _emit 0x00
            fld dword ptr [ecx + 4]
            fxch st(1)
            _emit 0xd8
        _emit 0x09
            fxch st(1)
            fmulp st(2), st(0)
            faddp st(1), st(0)
            _emit 0xd9
        _emit 0x1a
            fld dword ptr [eax + 0xc]
            fld dword ptr [ecx + 4]
            fld dword ptr [eax + 8]
            _emit 0xd8
        _emit 0x09
            fxch st(1)
            fmulp st(2), st(0)
            faddp st(1), st(0)
            fstp dword ptr [edx + 4]
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00534f70
void C2_NAKED C2_HOOK_CDECL BrMatrix23TApplyV(br_vector2* A, const br_vector2* B, const br_matrix23* C) {
#ifndef CARPOCALYPSE2_MATCHING

    A->v[0] = BR_MAC2(B->v[0], C(0, 0), B->v[1], C(0, 1));
    A->v[1] = BR_MAC2(B->v[0], C(1, 0), B->v[1], C(1, 1));
#else
    __asm {
            mov eax, dword ptr [esp + 0xc]
            mov ecx, dword ptr [esp + 8]
            mov edx, dword ptr [esp + 4]
            fld dword ptr [eax + 4]
            _emit 0xd9
        _emit 0x00
            fld dword ptr [ecx + 4]
            fxch st(1)
            _emit 0xd8
        _emit 0x09
            fxch st(1)
            fmulp st(2), st(0)
            faddp st(1), st(0)
            _emit 0xd9
        _emit 0x1a
            fld dword ptr [eax + 0xc]
            fld dword ptr [ecx + 4]
            fld dword ptr [eax + 8]
            _emit 0xd8
        _emit 0x09
            fxch st(1)
            fmulp st(2), st(0)
            faddp st(1), st(0)
            fstp dword ptr [edx + 4]
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00534fb0
void C2_HOOK_CDECL BrMatrix23Pre(br_matrix23* mat, const br_matrix23* A) {

    BrMatrix23Mul(&mattmp1_23, A, mat);
    mat->m[0][0] = mattmp1_23.m[0][0];
    mat->m[0][1] = mattmp1_23.m[0][1];
    mat->m[1][0] = mattmp1_23.m[1][0];
    mat->m[1][1] = mattmp1_23.m[1][1];
    mat->m[2][0] = mattmp1_23.m[2][0];
    mat->m[2][1] = mattmp1_23.m[2][1];
}

// FUNCTION: CARMA2_HW 0x00535000
void C2_HOOK_CDECL BrMatrix23Post(br_matrix23* mat, const br_matrix23* A) {

    BrMatrix23Mul(&mattmp1_23, mat, A);
    BrMatrix23Copy(mat, &mattmp1_23);
}

// FUNCTION: CARMA2_HW 0x00535050
void C2_HOOK_CDECL BrMatrix23PreRotate(br_matrix23* mat, br_angle rz) {

    BrMatrix23Rotate(&mattmp2_23, rz);
    BrMatrix23Mul(&mattmp1_23, &mattmp2_23, mat);
    BrMatrix23Copy(mat, &mattmp1_23);
}

// FUNCTION: CARMA2_HW 0x00535110
void C2_HOOK_CDECL BrMatrix23PostRotate(br_matrix23* mat, br_angle rz) {

    BrMatrix23Rotate(&mattmp2_23, rz);
    BrMatrix23Mul(&mattmp1_23, mat, &mattmp2_23);
    BrMatrix23Copy(mat, &mattmp1_23);
}

// FUNCTION: CARMA2_HW 0x005351d0
void C2_HOOK_CDECL BrMatrix23PreTranslate(br_matrix23* mat, br_scalar x, br_scalar y) {

    BrMatrix23Translate(&mattmp2_23, x, y);
    BrMatrix23Mul(&mattmp1_23, &mattmp2_23, mat);
    BrMatrix23Copy(mat, &mattmp1_23);
}

// FUNCTION: CARMA2_HW 0x00535260
void C2_HOOK_CDECL BrMatrix23PostTranslate(br_matrix23* A, br_scalar x, br_scalar y) {

    A(2, 0) += x;
    A(2, 1) += y;
}

// FUNCTION: CARMA2_HW 0x00535280
void C2_HOOK_CDECL BrMatrix23PreScale(br_matrix23* mat, br_scalar sx, br_scalar sy) {

    BrMatrix23Scale(&mattmp2_23, sx, sy);
    BrMatrix23Mul(&mattmp1_23, &mattmp2_23, mat);
    BrMatrix23Copy(mat, &mattmp1_23);
}

// FUNCTION: CARMA2_HW 0x00535310
void C2_HOOK_CDECL BrMatrix23PostScale(br_matrix23* mat, br_scalar sx, br_scalar sy) {

    BrMatrix23Scale(&mattmp2_23, sx, sy);
    BrMatrix23Mul(&mattmp1_23, mat, &mattmp2_23);
    BrMatrix23Copy(mat, &mattmp1_23);
}

// FUNCTION: CARMA2_HW 0x005353a0
void C2_HOOK_CDECL BrMatrix23PreShearX(br_matrix23* mat, br_scalar sy) {

    BrMatrix23ShearX(&mattmp2_23, sy);
    BrMatrix23Mul(&mattmp1_23, &mattmp2_23, mat);
    BrMatrix23Copy(mat, &mattmp1_23);
}

// FUNCTION: CARMA2_HW 0x00535430
void C2_HOOK_CDECL BrMatrix23PostShearX(br_matrix23* mat, br_scalar sy) {

    BrMatrix23ShearX(&mattmp2_23, sy);
    BrMatrix23Mul(&mattmp1_23, mat, &mattmp2_23);
    BrMatrix23Copy(mat, &mattmp1_23);
}

// FUNCTION: CARMA2_HW 0x005354c0
void C2_HOOK_CDECL BrMatrix23PreShearY(br_matrix23* mat, br_scalar sx) {

    BrMatrix23ShearY(&mattmp2_23, sx);
    BrMatrix23Mul(&mattmp1_23, &mattmp2_23, mat);
    BrMatrix23Copy(mat, &mattmp1_23);
}

// FUNCTION: CARMA2_HW 0x00535550
void C2_HOOK_CDECL BrMatrix23PostShearY(br_matrix23* mat, br_scalar sx) {

    BrMatrix23ShearY(&mattmp2_23, sx);
    BrMatrix23Mul(&mattmp1_23, mat, &mattmp2_23);
    BrMatrix23Copy(mat, &mattmp1_23);
}
