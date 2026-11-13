#include "matrix4.h"

#include "c2_math.h"
#include "c2_string.h"

#define A(x, y) A->m[x][y]
#define B(x, y) B->m[x][y]
#define C(x, y) C->m[x][y]
#define M(x, y) mat->m[x][y]


#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_CDECL BrMatrix4Adjoint();
extern br_scalar C2_HOOK_CDECL BrMatrix4Determinant();
extern char IID_IDirectInputDevice2A[];
#endif
// FUNCTION: CARMA2_HW 0x00535cd0
void C2_HOOK_CDECL BrMatrix4Copy(br_matrix4* A, const br_matrix4* B) {

    A(0, 0) = B(0, 0);
    A(0, 1) = B(0, 1);
    A(0, 2) = B(0, 2);
    A(0, 3) = B(0, 3);
    A(1, 0) = B(1, 0);
    A(1, 1) = B(1, 1);
    A(1, 2) = B(1, 2);
    A(1, 3) = B(1, 3);
    A(2, 0) = B(2, 0);
    A(2, 1) = B(2, 1);
    A(2, 2) = B(2, 2);
    A(2, 3) = B(2, 3);
    A(3, 0) = B(3, 0);
    A(3, 1) = B(3, 1);
    A(3, 2) = B(3, 2);
    A(3, 3) = B(3, 3);
}

// FUNCTION: CARMA2_HW 0x00535d40
void C2_NAKED C2_HOOK_CDECL BrMatrix4Mul(br_matrix4* A, const br_matrix4* B, const br_matrix4* C) {
#ifndef CARPOCALYPSE2_MATCHING

    A(0, 0) = BR_MAC4(B(0, 0), C(0, 0), B(0, 1), C(1, 0), B(0, 2), C(2, 0), B(0, 3), C(3, 0));
    A(0, 1) = BR_MAC4(B(0, 0), C(0, 1), B(0, 1), C(1, 1), B(0, 2), C(2, 1), B(0, 3), C(3, 1));
    A(0, 2) = BR_MAC4(B(0, 0), C(0, 2), B(0, 1), C(1, 2), B(0, 2), C(2, 2), B(0, 3), C(3, 2));
    A(0, 3) = BR_MAC4(B(0, 0), C(0, 3), B(0, 1), C(1, 3), B(0, 2), C(2, 3), B(0, 3), C(3, 3));
    A(1, 0) = BR_MAC4(B(1, 0), C(0, 0), B(1, 1), C(1, 0), B(1, 2), C(2, 0), B(1, 3), C(3, 0));
    A(1, 1) = BR_MAC4(B(1, 0), C(0, 1), B(1, 1), C(1, 1), B(1, 2), C(2, 1), B(1, 3), C(3, 1));
    A(1, 2) = BR_MAC4(B(1, 0), C(0, 2), B(1, 1), C(1, 2), B(1, 2), C(2, 2), B(1, 3), C(3, 2));
    A(1, 3) = BR_MAC4(B(1, 0), C(0, 3), B(1, 1), C(1, 3), B(1, 2), C(2, 3), B(1, 3), C(3, 3));
    A(2, 0) = BR_MAC4(B(2, 0), C(0, 0), B(2, 1), C(1, 0), B(2, 2), C(2, 0), B(2, 3), C(3, 0));
    A(2, 1) = BR_MAC4(B(2, 0), C(0, 1), B(2, 1), C(1, 1), B(2, 2), C(2, 1), B(2, 3), C(3, 1));
    A(2, 2) = BR_MAC4(B(2, 0), C(0, 2), B(2, 1), C(1, 2), B(2, 2), C(2, 2), B(2, 3), C(3, 2));
    A(2, 3) = BR_MAC4(B(2, 0), C(0, 3), B(2, 1), C(1, 3), B(2, 2), C(2, 3), B(2, 3), C(3, 3));
    A(3, 0) = BR_MAC4(B(3, 0), C(0, 0), B(3, 1), C(1, 0), B(3, 2), C(2, 0), B(3, 3), C(3, 0));
    A(3, 1) = BR_MAC4(B(3, 0), C(0, 1), B(3, 1), C(1, 1), B(3, 2), C(2, 1), B(3, 3), C(3, 1));
    A(3, 2) = BR_MAC4(B(3, 0), C(0, 2), B(3, 1), C(1, 2), B(3, 2), C(2, 2), B(3, 3), C(3, 2));
    A(3, 3) = BR_MAC4(B(3, 0), C(0, 3), B(3, 1), C(1, 3), B(3, 2), C(2, 3), B(3, 3), C(3, 3));
#else
    __asm {
            mov ecx, dword ptr [esp + 8]
            mov eax, dword ptr [esp + 0xc]
            mov edx, dword ptr [esp + 4]
            fld dword ptr [ecx + 0xc]
            fld dword ptr [eax + 0x30]
            fld dword ptr [ecx + 8]
            fld dword ptr [eax + 0x20]
            fld dword ptr [ecx + 4]
            fxch st(3)
            fmulp st(4), st(0)
            fmulp st(1), st(0)
            faddp st(2), st(0)
            fmul dword ptr [eax + 0x10]
            _emit 0xd9
        _emit 0x00
            _emit 0xd8
        _emit 0x09
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            _emit 0xd9
        _emit 0x1a
            fld dword ptr [eax + 4]
            fld dword ptr [eax + 0x24]
            fld dword ptr [ecx + 8]
            fld dword ptr [eax + 0x14]
            fxch st(3)
            _emit 0xd8
        _emit 0x09
            fxch st(1)
            fmulp st(2), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fxch st(1)
            fmul dword ptr [ecx + 4]
            fld dword ptr [eax + 0x34]
            fxch st(1)
            faddp st(2), st(0)
            fmul dword ptr [ecx + 0xc]
            faddp st(1), st(0)
            fstp dword ptr [edx + 4]
            fld dword ptr [eax + 8]
            fld dword ptr [eax + 0x38]
            fld dword ptr [ecx + 0xc]
            fld dword ptr [eax + 0x18]
            fxch st(3)
            _emit 0xd8
        _emit 0x09
            fxch st(1)
            fmulp st(2), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fxch st(1)
            fmul dword ptr [ecx + 4]
            fld dword ptr [eax + 0x28]
            fxch st(1)
            faddp st(2), st(0)
            fmul dword ptr [ecx + 8]
            faddp st(1), st(0)
            fstp dword ptr [edx + 8]
            fld dword ptr [ecx + 4]
            fld dword ptr [eax + 0xc]
            fld dword ptr [eax + 0x1c]
            fld dword ptr [eax + 0x3c]
            fxch st(2)
            _emit 0xd8
        _emit 0x09
            fxch st(3)
            fmulp st(1), st(0)
            faddp st(2), st(0)
            fmul dword ptr [ecx + 0xc]
            fld dword ptr [eax + 0x2c]
            fxch st(1)
            faddp st(2), st(0)
            fmul dword ptr [ecx + 8]
            faddp st(1), st(0)
            fstp dword ptr [edx + 0xc]
            fld dword ptr [ecx + 0x10]
            fld dword ptr [ecx + 0x1c]
            fld dword ptr [eax + 0x30]
            fld dword ptr [ecx + 0x18]
            fxch st(3)
            _emit 0xd8
        _emit 0x08
            fxch st(1)
            fmulp st(2), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fxch st(1)
            fmul dword ptr [eax + 0x20]
            fld dword ptr [ecx + 0x14]
            fxch st(1)
            faddp st(2), st(0)
            fmul dword ptr [eax + 0x10]
            faddp st(1), st(0)
            fstp dword ptr [edx + 0x10]
            fld dword ptr [ecx + 0x10]
            fld dword ptr [eax + 4]
            fld dword ptr [ecx + 0x1c]
            fld dword ptr [eax + 0x34]
            fld dword ptr [ecx + 0x18]
            fxch st(3)
            fmulp st(4), st(0)
            fmulp st(1), st(0)
            faddp st(2), st(0)
            fmul dword ptr [eax + 0x24]
            fld dword ptr [ecx + 0x14]
            fxch st(1)
            faddp st(2), st(0)
            fmul dword ptr [eax + 0x14]
            faddp st(1), st(0)
            fstp dword ptr [edx + 0x14]
            fld dword ptr [ecx + 0x14]
            fld dword ptr [eax + 0x18]
            fld dword ptr [ecx + 0x18]
            fld dword ptr [eax + 0x28]
            fld dword ptr [ecx + 0x10]
            fxch st(3)
            fmulp st(4), st(0)
            fmulp st(1), st(0)
            faddp st(2), st(0)
            fmul dword ptr [eax + 8]
            fld dword ptr [ecx + 0x1c]
            fxch st(1)
            faddp st(2), st(0)
            fmul dword ptr [eax + 0x38]
            faddp st(1), st(0)
            fstp dword ptr [edx + 0x18]
            fld dword ptr [ecx + 0x10]
            fld dword ptr [eax + 0xc]
            fld dword ptr [eax + 0x1c]
            fld dword ptr [ecx + 0x14]
            fld dword ptr [ecx + 0x18]
            fxch st(3)
            fmulp st(4), st(0)
            fmulp st(1), st(0)
            faddp st(2), st(0)
            fmul dword ptr [eax + 0x2c]
            fld dword ptr [ecx + 0x1c]
            fxch st(1)
            faddp st(2), st(0)
            fmul dword ptr [eax + 0x3c]
            faddp st(1), st(0)
            fstp dword ptr [edx + 0x1c]
            fld dword ptr [ecx + 0x20]
            fld dword ptr [ecx + 0x28]
            fld dword ptr [eax + 0x20]
            fld dword ptr [ecx + 0x24]
            fxch st(3)
            _emit 0xd8
        _emit 0x08
            fxch st(1)
            fmulp st(2), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fxch st(1)
            fmul dword ptr [eax + 0x10]
            fld dword ptr [ecx + 0x2c]
            fxch st(1)
            faddp st(2), st(0)
            fmul dword ptr [eax + 0x30]
            faddp st(1), st(0)
            fstp dword ptr [edx + 0x20]
            fld dword ptr [ecx + 0x28]
            fld dword ptr [eax + 0x24]
            fld dword ptr [ecx + 0x24]
            fld dword ptr [eax + 0x14]
            fld dword ptr [ecx + 0x2c]
            fxch st(3)
            fmulp st(4), st(0)
            fmulp st(1), st(0)
            faddp st(2), st(0)
            fmul dword ptr [eax + 0x34]
            fld dword ptr [ecx + 0x20]
            fxch st(1)
            faddp st(2), st(0)
            fmul dword ptr [eax + 4]
            faddp st(1), st(0)
            fstp dword ptr [edx + 0x24]
            fld dword ptr [ecx + 0x28]
            fld dword ptr [eax + 0x28]
            fld dword ptr [ecx + 0x20]
            fld dword ptr [eax + 8]
            fld dword ptr [ecx + 0x24]
            fxch st(3)
            fmulp st(4), st(0)
            fmulp st(1), st(0)
            faddp st(2), st(0)
            fmul dword ptr [eax + 0x18]
            fld dword ptr [ecx + 0x2c]
            fxch st(1)
            faddp st(2), st(0)
            fmul dword ptr [eax + 0x38]
            faddp st(1), st(0)
            fstp dword ptr [edx + 0x28]
            fld dword ptr [ecx + 0x28]
            fld dword ptr [eax + 0x2c]
            fld dword ptr [ecx + 0x2c]
            fld dword ptr [eax + 0x3c]
            fld dword ptr [ecx + 0x20]
            fxch st(3)
            fmulp st(4), st(0)
            fmulp st(1), st(0)
            faddp st(2), st(0)
            fmul dword ptr [eax + 0xc]
            fld dword ptr [ecx + 0x24]
            fxch st(1)
            faddp st(2), st(0)
            fmul dword ptr [eax + 0x1c]
            faddp st(1), st(0)
            fstp dword ptr [edx + 0x2c]
            fld dword ptr [ecx + 0x30]
            fld dword ptr [ecx + 0x34]
            fld dword ptr [eax + 0x10]
            fld dword ptr [ecx + 0x3c]
            fxch st(3)
            _emit 0xd8
        _emit 0x08
            fxch st(1)
            fmulp st(2), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fxch st(1)
            fmul dword ptr [eax + 0x30]
            fld dword ptr [ecx + 0x38]
            fxch st(1)
            faddp st(2), st(0)
            fmul dword ptr [eax + 0x20]
            faddp st(1), st(0)
            fstp dword ptr [edx + 0x30]
            fld dword ptr [ecx + 0x34]
            fld dword ptr [eax + 0x14]
            fld dword ptr [ecx + 0x3c]
            fld dword ptr [eax + 0x34]
            fld dword ptr [ecx + 0x30]
            fxch st(3)
            fmulp st(4), st(0)
            fmulp st(1), st(0)
            faddp st(2), st(0)
            fmul dword ptr [eax + 4]
            fld dword ptr [ecx + 0x38]
            fxch st(1)
            faddp st(2), st(0)
            fmul dword ptr [eax + 0x24]
            faddp st(1), st(0)
            fstp dword ptr [edx + 0x34]
            fld dword ptr [ecx + 0x38]
            fld dword ptr [eax + 0x28]
            fld dword ptr [ecx + 0x34]
            fld dword ptr [eax + 0x18]
            fld dword ptr [ecx + 0x30]
            fxch st(3)
            fmulp st(4), st(0)
            fmulp st(1), st(0)
            faddp st(2), st(0)
            fmul dword ptr [eax + 8]
            fld dword ptr [ecx + 0x3c]
            fxch st(1)
            faddp st(2), st(0)
            fmul dword ptr [eax + 0x38]
            faddp st(1), st(0)
            fstp dword ptr [edx + 0x38]
            fld dword ptr [ecx + 0x30]
            fld dword ptr [eax + 0xc]
            fld dword ptr [ecx + 0x34]
            fld dword ptr [eax + 0x1c]
            fld dword ptr [ecx + 0x38]
            fxch st(3)
            fmulp st(4), st(0)
            fmulp st(1), st(0)
            faddp st(2), st(0)
            fmul dword ptr [eax + 0x2c]
            fld dword ptr [ecx + 0x3c]
            fxch st(1)
            faddp st(2), st(0)
            fmul dword ptr [eax + 0x3c]
            faddp st(1), st(0)
            fstp dword ptr [edx + 0x3c]
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00535ff0
void C2_HOOK_CDECL BrMatrix4Identity(br_matrix4* mat) {

    M(0, 0) = BR_SCALAR(1.f);
    M(0, 1) = BR_SCALAR(0.f);
    M(0, 2) = BR_SCALAR(0.f);
    M(0, 3) = BR_SCALAR(0.f);
    M(1, 0) = BR_SCALAR(0.f);
    M(1, 1) = BR_SCALAR(1.f);
    M(1, 2) = BR_SCALAR(0.f);
    M(1, 3) = BR_SCALAR(0.f);
    M(2, 0) = BR_SCALAR(0.f);
    M(2, 1) = BR_SCALAR(0.f);
    M(2, 2) = BR_SCALAR(1.f);
    M(2, 3) = BR_SCALAR(0.f);
    M(3, 0) = BR_SCALAR(0.f);
    M(3, 1) = BR_SCALAR(0.f);
    M(3, 2) = BR_SCALAR(0.f);
    M(3, 3) = BR_SCALAR(1.f);
}

// FUNCTION: CARMA2_HW 0x00536030
void C2_HOOK_CDECL BrMatrix4Scale(br_matrix4* mat, br_scalar sx, br_scalar sy, br_scalar sz) {

    M(0, 0) = sx;
    M(1, 1) = sy;
    M(2, 2) = sz;
    M(0, 1) = BR_SCALAR(0.f);
    M(0, 2) = BR_SCALAR(0.f);
    M(0, 3) = BR_SCALAR(0.f);
    M(1, 0) = BR_SCALAR(0.f);
    M(1, 2) = BR_SCALAR(0.f);
    M(1, 3) = BR_SCALAR(0.f);
    M(2, 0) = BR_SCALAR(0.f);
    M(2, 1) = BR_SCALAR(0.f);
    M(2, 3) = BR_SCALAR(0.f);
    M(3, 0) = BR_SCALAR(0.f);
    M(3, 1) = BR_SCALAR(0.f);
    M(3, 2) = BR_SCALAR(0.f);
    M(3, 3) = BR_SCALAR(1.f);
}

// FUNCTION: CARMA2_HW 0x00536080
br_scalar C2_NAKED C2_HOOK_CDECL BrMatrix4Inverse(br_matrix4* A, const br_matrix4* B) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;
    int j;
    br_scalar det;
    br_scalar idet;

    BrMatrix4Adjoint(A, B);
    det = BrMatrix4Determinant(B);

    if (fabs(det) < BR_SCALAR_EPSILON * 2)
        return 0;

    idet = 1.0f / det;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            A(i, j) = A(i, j) * idet;
        }
    }

    return det;
#else
    __asm {
            push esi
            mov esi, dword ptr [esp + 8]
            push edi
            mov edi, dword ptr [esp + 0x10]
            push edi
            push esi
            call BrMatrix4Adjoint
            add esp, 8
            push edi
            call BrMatrix4Determinant
            fld st(0)
            fabs
            fcomp dword ptr [IID_IDirectInputDevice2A+0x5a8]
            add esp, 4
            fnstsw ax
            test ah, 1
            je L005360b9
            fstp st(0)
            fld dword ptr [IID_IDirectInputDevice2A+0x5ac]
            pop edi
            pop esi
            ret
    L005360b9:
            fld st(0)
            fdivr qword ptr [IID_IDirectInputDevice2A+0x5b0]
            mov eax, esi
            mov edx, 4
    L005360c8:
            mov ecx, 4
    L005360cd:
            _emit 0xd9
        _emit 0x00
            fmul st(0), st(1)
            add eax, 4
            dec ecx
            fstp dword ptr [eax - 4]
            jne L005360cd
            dec edx
            jne L005360c8
            pop edi
            pop esi
            fstp st(0)
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x005367b0
br_scalar C2_HOOK_STDCALL Determinant3(br_scalar a1, br_scalar a2, br_scalar a3, br_scalar b1, br_scalar b2, br_scalar b3, br_scalar c1, br_scalar c2, br_scalar c3) {

    return BR_MAC3(a1, BR_MAC2(b2, c3, -b3, c2),
        -b1, BR_MAC2(a2, c3, -a3, c2),
        c1, BR_MAC2(a2, b3, -a3, b2));
}

// FUNCTION: CARMA2_HW 0x005360f0
br_scalar C2_NAKED C2_HOOK_CDECL BrMatrix4Determinant(const br_matrix4* mat) {
#ifndef CARPOCALYPSE2_MATCHING
    br_scalar a1;
    br_scalar a2;
    br_scalar a3;
    br_scalar a4;
    br_scalar b1;
    br_scalar b2;
    br_scalar b3;
    br_scalar b4;
    br_scalar c1;
    br_scalar c2;
    br_scalar c3;
    br_scalar c4;
    br_scalar d1;
    br_scalar d2;
    br_scalar d3;
    br_scalar d4;

    a1 = M(0, 0);
    b1 = M(0, 1);
    c1 = M(0, 2);
    d1 = M(0, 3);

    a2 = M(1, 0);
    b2 = M(1, 1);
    c2 = M(1, 2);
    d2 = M(1, 3);

    a3 = M(2, 0);
    b3 = M(2, 1);
    c3 = M(2, 2);
    d3 = M(2, 3);

    a4 = M(3, 0);
    b4 = M(3, 1);
    c4 = M(3, 2);
    d4 = M(3, 3);

    return BR_MAC4(a1, Determinant3(b2, b3, b4, c2, c3, c4, d2, d3, d4),
        -b1, Determinant3(a2, a3, a4, c2, c3, c4, d2, d3, d4),
        c1, Determinant3(a2, a3, a4, b2, b3, b4, d2, d3, d4),
        -d1, Determinant3(a2, a3, a4, b2, b3, b4, c2, c3, c4));
#else
    __asm {
            sub esp, 0x38
            mov eax, dword ptr [esp + 0x3c]
            fld dword ptr [eax + 0x30]
            fld dword ptr [eax + 0x34]
            fld dword ptr [eax + 0x38]
            mov edx, dword ptr [eax + 0x14]
            mov ecx, dword ptr [eax + 0x10]
            fld dword ptr [eax + 0x24]
            fld dword ptr [eax + 0x28]
            fxch st(1)
            fstp dword ptr [esp + 0x14]
            mov dword ptr [esp + 0x24], edx
            mov edx, dword ptr [eax + 0x1c]
            fst dword ptr [esp + 0x3c]
            mov dword ptr [esp + 0x18], edx
            mov edx, dword ptr [eax + 0x2c]
            mov dword ptr [esp + 4], edx
            mov dword ptr [esp + 0x1c], ecx
            mov ecx, dword ptr [eax + 0x18]
            fld dword ptr [esp + 4]
            fxch st(4)
            fst dword ptr [esp + 0x34]
            mov dword ptr [esp + 0x28], ecx
            mov ecx, dword ptr [eax + 0x20]
            fxch st(3)
            fst dword ptr [esp + 0x20]
            mov dword ptr [esp + 0x10], ecx
            mov ecx, dword ptr [eax + 0x3c]
            fxch st(2)
            _emit 0xd9
        _emit 0x54
        _emit 0x24
        _emit 0x00
            mov dword ptr [esp + 8], ecx
            fld dword ptr [esp + 8]
            fmul dword ptr [esp + 0x3c]
            fxch st(1)
            fchs
            fmul dword ptr [esp + 4]
            faddp st(1), st(0)
            fstp dword ptr [esp + 0x2c]
            _emit 0xd9
        _emit 0x44
        _emit 0x24
        _emit 0x00
            fxch st(3)
            fchs
            fstp dword ptr [esp + 0xc]
            fld dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x3c]
            fmul dword ptr [esp + 0xc]
            _emit 0xd9
        _emit 0x44
        _emit 0x24
        _emit 0x00
            fmul dword ptr [esp + 0x10]
            faddp st(1), st(0)
            _emit 0xd9
        _emit 0x5c
        _emit 0x24
        _emit 0x00
            fld dword ptr [esp + 0x20]
            fld dword ptr [esp + 4]
            fmul dword ptr [esp + 0xc]
            fld dword ptr [esp + 8]
            fmul dword ptr [esp + 0x10]
            faddp st(1), st(0)
            fstp dword ptr [esp + 0x34]
            fld dword ptr [esp + 0x28]
            fchs
            fstp dword ptr [esp + 0x30]
            fld dword ptr [esp + 0x24]
            fchs
            fstp dword ptr [esp + 4]
            fxch st(3)
            fchs
            fstp dword ptr [esp + 0x3c]
            fxch st(1)
            fmul dword ptr [esp + 0x3c]
            fld dword ptr [esp + 8]
            fxch st(4)
            fmul dword ptr [esp + 0x14]
            faddp st(1), st(0)
            fstp dword ptr [esp + 0x20]
            fmul dword ptr [esp + 0xc]
            fld dword ptr [esp + 0x18]
            fxch st(2)
            fmul dword ptr [esp + 0x10]
            faddp st(1), st(0)
            fxch st(3)
            fmul dword ptr [esp + 0x3c]
            fld dword ptr [esp + 0x24]
            fxch st(4)
            fstp dword ptr [esp + 0x24]
            fxch st(2)
            fmul dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x1c]
            fxch st(1)
            faddp st(3), st(0)
            fxch st(1)
            fmul dword ptr [esp + 0x20]
            fxch st(2)
            fstp dword ptr [esp + 0x3c]
            fmul dword ptr [esp + 0x3c]
            fld dword ptr [esp + 0x3c]
            fxch st(3)
            fmul dword ptr [esp + 0x2c]
            faddp st(2), st(0)
            fld dword ptr [esp + 0x18]
            fxch st(3)
            fmul dword ptr [esp + 0x30]
            faddp st(2), st(0)
            fxch st(2)
            fmul dword ptr [esp + 0x24]
            faddp st(2), st(0)
            _emit 0xd8
        _emit 0x08
            fld dword ptr [esp + 4]
            fld dword ptr [esp + 0x28]
            fld dword ptr [esp + 0x1c]
            fxch st(2)
            fmul dword ptr [esp + 0x34]
            faddp st(4), st(0)
            fld dword ptr [eax + 8]
            fxch st(1)
            fmul dword ptr [esp + 0x24]
            fxch st(1)
            fmulp st(4), st(0)
            fxch st(1)
            fmul dword ptr [esp + 0x20]
            fld dword ptr [esp + 4]
            fxch st(4)
            faddp st(3), st(0)
            fld dword ptr [esp + 0x18]
            fxch st(1)
            faddp st(2), st(0)
            fld dword ptr [esp + 0x1c]
            fxch st(4)
            _emit 0xd8
        _emit 0x4c
        _emit 0x24
        _emit 0x00
            faddp st(2), st(0)
            fld dword ptr [eax + 0xc]
            fxch st(1)
            _emit 0xd8
        _emit 0x4c
        _emit 0x24
        _emit 0x00
            fxch st(1)
            fchs
            fmulp st(2), st(0)
            fxch st(3)
            fmul dword ptr [esp + 0x2c]
            fld dword ptr [esp + 0x30]
            fxch st(2)
            faddp st(3), st(0)
            fxch st(1)
            fmul dword ptr [esp + 0x34]
            fxch st(1)
            faddp st(3), st(0)
            fld dword ptr [eax + 4]
            fxch st(1)
            faddp st(3), st(0)
            fchs
            fmulp st(2), st(0)
            fxch st(1)
            faddp st(1), st(0)
            add esp, 0x38
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x005362c0
void C2_NAKED C2_HOOK_CDECL BrMatrix4Adjoint(br_matrix4* A, const br_matrix4* B) {
#ifndef CARPOCALYPSE2_MATCHING
    br_scalar a1;
    br_scalar a2;
    br_scalar a3;
    br_scalar a4;
    br_scalar b1;
    br_scalar b2;
    br_scalar b3;
    br_scalar b4;
    br_scalar c1;
    br_scalar c2;
    br_scalar c3;
    br_scalar c4;
    br_scalar d1;
    br_scalar d2;
    br_scalar d3;
    br_scalar d4;

    a1 = B(0, 0);
    b1 = B(0, 1);
    c1 = B(0, 2);
    d1 = B(0, 3);

    a2 = B(1, 0);
    b2 = B(1, 1);
    c2 = B(1, 2);
    d2 = B(1, 3);

    a3 = B(2, 0);
    b3 = B(2, 1);
    c3 = B(2, 2);
    d3 = B(2, 3);

    a4 = B(3, 0);
    b4 = B(3, 1);
    c4 = B(3, 2);
    d4 = B(3, 3);

    /* row column labeling reversed since we transpose rows & columns */

    A(0, 0) = Determinant3(b2, b3, b4, c2, c3, c4, d2, d3, d4);
    A(1, 0) = -Determinant3(a2, a3, a4, c2, c3, c4, d2, d3, d4);
    A(2, 0) = Determinant3(a2, a3, a4, b2, b3, b4, d2, d3, d4);
    A(3, 0) = -Determinant3(a2, a3, a4, b2, b3, b4, c2, c3, c4);

    A(0, 1) = -Determinant3(b1, b3, b4, c1, c3, c4, d1, d3, d4);
    A(1, 1) = Determinant3(a1, a3, a4, c1, c3, c4, d1, d3, d4);
    A(2, 1) = -Determinant3(a1, a3, a4, b1, b3, b4, d1, d3, d4);
    A(3, 1) = Determinant3(a1, a3, a4, b1, b3, b4, c1, c3, c4);

    A(0, 2) = Determinant3(b1, b2, b4, c1, c2, c4, d1, d2, d4);
    A(1, 2) = -Determinant3(a1, a2, a4, c1, c2, c4, d1, d2, d4);
    A(2, 2) = Determinant3(a1, a2, a4, b1, b2, b4, d1, d2, d4);
    A(3, 2) = -Determinant3(a1, a2, a4, b1, b2, b4, c1, c2, c4);

    A(0, 3) = -Determinant3(b1, b2, b3, c1, c2, c3, d1, d2, d3);
    A(1, 3) = Determinant3(a1, a2, a3, c1, c2, c3, d1, d2, d3);
    A(2, 3) = -Determinant3(a1, a2, a3, b1, b2, b3, d1, d2, d3);
    A(3, 3) = Determinant3(a1, a2, a3, b1, b2, b3, c1, c2, c3);
#else
    __asm {
            sub esp, 0x5c
            mov eax, dword ptr [esp + 0x64]
            push esi
            fld dword ptr [eax + 0x34]
            fld dword ptr [eax + 0x38]
            fld dword ptr [eax + 0x28]
            fld dword ptr [eax + 0x2c]
            fld dword ptr [eax + 0x3c]
            fld dword ptr [eax + 0x24]
            mov ecx, dword ptr [eax]
            mov edx, dword ptr [eax + 4]
            fstp dword ptr [esp + 0x28]
            fxch st(4)
            fst dword ptr [esp + 0x58]
            fxch st(3)
            fst dword ptr [esp + 0x2c]
            fld dword ptr [esp + 0x2c]
            fxch st(3)
            fst dword ptr [esp + 0x18]
            fxch st(2)
            fst dword ptr [esp + 0x24]
            fld dword ptr [eax + 0x30]
            fxch st(6)
            fst dword ptr [esp + 0x1c]
            fxch st(5)
            fchs
            fstp dword ptr [esp + 0x50]
            fxch st(2)
            fmul dword ptr [esp + 0x50]
            fxch st(1)
            fchs
            fxch st(3)
            fmul dword ptr [esp + 0x28]
            fxch st(3)
            fstp dword ptr [esp + 0x5c]
            fxch st(1)
            fmul dword ptr [esp + 0x5c]
            fld dword ptr [esp + 0x24]
            fxch st(4)
            fmul dword ptr [esp + 0x18]
            fld dword ptr [esp + 0x1c]
            fxch st(4)
            faddp st(3), st(0)
            fxch st(4)
            fmul dword ptr [esp + 0x50]
            mov dword ptr [esp + 0x14], ecx
            mov ecx, dword ptr [eax + 8]
            mov dword ptr [esp + 0x54], ecx
            mov ecx, dword ptr [eax + 0x10]
            mov dword ptr [esp + 0x68], ecx
            mov ecx, dword ptr [eax + 0x18]
            mov dword ptr [esp + 8], ecx
            mov dword ptr [esp + 0x34], edx
            fld dword ptr [esp + 8]
            fxch st(3)
            fstp dword ptr [esp + 0x48]
            mov edx, dword ptr [eax + 0xc]
            mov ecx, dword ptr [eax + 0x20]
            fxch st(3)
            fmul dword ptr [esp + 0x28]
            fxch st(4)
            faddp st(1), st(0)
            mov dword ptr [esp + 0xc], edx
            mov edx, dword ptr [eax + 0x14]
            mov dword ptr [esp + 0x10], edx
            mov edx, dword ptr [eax + 0x1c]
            fld dword ptr [esp + 0x10]
            mov dword ptr [esp + 4], edx
            mov dword ptr [esp + 0x38], ecx
            fld dword ptr [esp + 4]
            fxch st(5)
            faddp st(4), st(0)
            fxch st(2)
            fchs
            fst dword ptr [esp + 0x44]
            fxch st(3)
            fstp dword ptr [esp + 0x40]
            fxch st(2)
            fmul dword ptr [esp + 0x40]
            fxch st(3)
            fmul dword ptr [esp + 0x48]
            fxch st(2)
            fstp dword ptr [esp + 0x30]
            fmul dword ptr [esp + 0x30]
            fld dword ptr [esp + 0x18]
            fld dword ptr [esp + 0x2c]
            fxch st(3)
            faddp st(2), st(0)
            fxch st(4)
            fchs
            fstp dword ptr [esp + 0x20]
            fxch st(3)
            fmul dword ptr [esp + 0x20]
            fxch st(2)
            faddp st(3), st(0)
            mov esi, dword ptr [esp + 0x64]
            fmul dword ptr [esp + 0x38]
            fld dword ptr [esp + 0x24]
            fmul dword ptr [esp + 0x20]
            fxch st(1)
            faddp st(2), st(0)
            fxch st(2)
            _emit 0xd9
        _emit 0x1e
            fstp dword ptr [esp + 0x4c]
            fld dword ptr [esp + 0x1c]
            fmul dword ptr [esp + 0x38]
            faddp st(1), st(0)
            fstp dword ptr [esp + 0x3c]
            fld dword ptr [esp + 0x58]
            fld dword ptr [esp + 0x28]
            fmul dword ptr [esp + 0x20]
            fld dword ptr [esp + 0x10]
            fxch st(2)
            fmul dword ptr [esp + 0x38]
            fld dword ptr [esp + 0x54]
            fld dword ptr [esp + 0x34]
            fld dword ptr [esp + 4]
            fmul dword ptr [esp + 0x4c]
            fld dword ptr [esp + 0x68]
            fmul dword ptr [esp + 0x30]
            faddp st(1), st(0)
            fld dword ptr [esp + 0x3c]
            fmul dword ptr [esp + 0x44]
            faddp st(1), st(0)
            fchs
            fstp dword ptr [esi + 0x10]
            fxch st(2)
            faddp st(3), st(0)
            fld dword ptr [esp + 8]
            fld dword ptr [esp + 0x68]
            fxch st(4)
            fstp dword ptr [esp + 0x44]
            fmul dword ptr [esp + 0x44]
            fld dword ptr [esp + 0x34]
            fxch st(5)
            fchs
            fst dword ptr [esp + 0x64]
            fld dword ptr [esp + 4]
            fmul dword ptr [esp + 0x44]
            fld dword ptr [esp + 0x68]
            fmul dword ptr [esp + 0x40]
            faddp st(1), st(0)
            fld dword ptr [esp + 0x64]
            fmul dword ptr [esp + 0x3c]
            faddp st(1), st(0)
            fstp dword ptr [esi + 0x20]
            fxch st(4)
            fmul dword ptr [esp + 0x48]
            faddp st(1), st(0)
            fld dword ptr [esp + 0xc]
            fxch st(4)
            fmul dword ptr [esp + 0x4c]
            faddp st(1), st(0)
            fxch st(1)
            fchs
            fst dword ptr [esp + 0x64]
            fmul dword ptr [esp + 0x40]
            fld dword ptr [esp + 0x64]
            fxch st(2)
            fchs
            fstp dword ptr [esi + 0x30]
            fld dword ptr [esp + 0xc]
            fxch st(5)
            fmul dword ptr [esp + 0x30]
            fld dword ptr [esp + 0xc]
            fxch st(5)
            fmul dword ptr [esp + 0x48]
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fxch st(3)
            fmul dword ptr [esp + 0x4c]
            fld dword ptr [esp + 0x14]
            fxch st(4)
            fchs
            fstp dword ptr [esi + 4]
            fld dword ptr [esp + 0x14]
            fxch st(4)
            fmul dword ptr [esp + 0x30]
            faddp st(1), st(0)
            fxch st(4)
            fmul dword ptr [esp + 0x44]
            fld dword ptr [esp + 0x54]
            fxch st(2)
            fmul dword ptr [esp + 0x3c]
            faddp st(5), st(0)
            fxch st(2)
            fchs
            fst dword ptr [esp + 0x30]
            fmul dword ptr [esp + 0x3c]
            fld dword ptr [esp + 0x30]
            fxch st(5)
            fstp dword ptr [esi + 0x14]
            fxch st(3)
            fmul dword ptr [esp + 0x40]
            fld dword ptr [esp + 0x14]
            fxch st(1)
            faddp st(3), st(0)
            fxch st(1)
            fmul dword ptr [esp + 0x44]
            fxch st(3)
            faddp st(2), st(0)
            fmul dword ptr [esp + 0x48]
            faddp st(2), st(0)
            fchs
            fstp dword ptr [esi + 0x24]
            fxch st(1)
            fmul dword ptr [esp + 0x4c]
            faddp st(1), st(0)
            fstp dword ptr [esi + 0x34]
            fld dword ptr [esp + 8]
            fld dword ptr [esp + 0x2c]
            fld dword ptr [esp + 4]
            fld dword ptr [esp + 0x1c]
            fld dword ptr [esp + 8]
            fld dword ptr [esp + 0x2c]
            fxch st(5)
            fmul dword ptr [esp + 0x50]
            fld dword ptr [esp + 4]
            fxch st(5)
            fmul dword ptr [esp + 0x10]
            faddp st(1), st(0)
            fstp dword ptr [esp + 0x4c]
            fld dword ptr [esp + 0x10]
            fxch st(4)
            fmul dword ptr [esp + 0x50]
            fld dword ptr [esp + 0x1c]
            fmul dword ptr [esp + 0x10]
            faddp st(1), st(0)
            fst dword ptr [esp + 0x50]
            fxch st(3)
            fmul dword ptr [esp + 0x5c]
            fld dword ptr [esp + 0xc]
            fxch st(3)
            fmul dword ptr [esp + 8]
            faddp st(1), st(0)
            fstp dword ptr [esp + 0x5c]
            fld dword ptr [esp + 0x58]
            fxch st(2)
            fmul dword ptr [esp + 0x4c]
            fld dword ptr [esp + 0x34]
            fmul dword ptr [esp + 0x5c]
            faddp st(1), st(0)
            fxch st(3)
            fmul dword ptr [esp + 0x64]
            faddp st(3), st(0)
            fmul dword ptr [esp + 0x20]
            fld dword ptr [esp + 4]
            fxch st(3)
            fstp dword ptr [esi + 8]
            fxch st(4)
            fmul dword ptr [esp + 0x68]
            fld dword ptr [esp + 0x1c]
            fxch st(1)
            faddp st(5), st(0)
            fxch st(2)
            fmul dword ptr [esp + 0x20]
            fld dword ptr [esp + 0x14]
            fxch st(5)
            fst dword ptr [esp + 0x58]
            fxch st(3)
            fmul dword ptr [esp + 0x68]
            faddp st(1), st(0)
            fxch st(4)
            fmul dword ptr [esp + 0x5c]
            fld dword ptr [esp + 0xc]
            fxch st(5)
            fst dword ptr [esp + 0x5c]
            fxch st(5)
            fmul dword ptr [esp + 0x58]
            faddp st(1), st(0)
            fxch st(3)
            fmul dword ptr [esp + 0x20]
            fld dword ptr [esp + 0x54]
            fxch st(5)
            fmul dword ptr [esp + 0x64]
            faddp st(4), st(0)
            fxch st(1)
            fmul dword ptr [esp + 0x68]
            fld dword ptr [esp + 0xc]
            fxch st(4)
            fchs
            fstp dword ptr [esi + 0x18]
            faddp st(1), st(0)
            fld dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x14]
            fxch st(5)
            fmul st(0), st(2)
            fld dword ptr [esp + 0x5c]
            fxch st(5)
            fmul st(0), st(3)
            fld dword ptr [esp + 0x28]
            fxch st(7)
            fmul dword ptr [esp + 0x50]
            faddp st(1), st(0)
            fxch st(5)
            fmul dword ptr [esp + 0x30]
            faddp st(5), st(0)
            fxch st(1)
            fmul dword ptr [esp + 0x4c]
            fxch st(5)
            fchs
            fxch st(5)
            faddp st(1), st(0)
            fxch st(3)
            fstp dword ptr [esi + 0x28]
            fxch st(3)
            fstp dword ptr [esp + 0x5c]
            fmul dword ptr [esp + 0x30]
            faddp st(1), st(0)
            fchs
            fstp dword ptr [esi + 0x38]
            fstp st(0)
            fld dword ptr [esp + 0x38]
            fld dword ptr [esp + 0x18]
            fld dword ptr [esp + 4]
            fld dword ptr [esp + 0x24]
            fld dword ptr [esp + 0x24]
            fld dword ptr [esp + 8]
            fxch st(3)
            fmul dword ptr [esp + 0x5c]
            fxch st(4)
            fchs
            fxch st(2)
            fmul dword ptr [esp + 0x10]
            mov edx, dword ptr [esp + 0x18]
            faddp st(4), st(0)
            fxch st(1)
            fmul dword ptr [esp + 4]
            fxch st(3)
            fst dword ptr [esp + 0x50]
            fmul dword ptr [esp + 0x64]
            fld dword ptr [esp + 0x18]
            fxch st(2)
            fmul dword ptr [esp + 8]
            faddp st(4), st(0)
            fxch st(2)
            fmul dword ptr [esp + 0x5c]
            fld dword ptr [esp + 0x34]
            fxch st(4)
            fstp dword ptr [esp + 0x58]
            fxch st(3)
            fmul dword ptr [esp + 0x58]
            fld dword ptr [esp + 0x10]
            fxch st(2)
            fmul dword ptr [esp + 0x10]
            faddp st(4), st(0)
            fxch st(4)
            fchs
            fxch st(1)
            fmul st(0), st(1)
            fxch st(3)
            fmul dword ptr [esp + 0xc]
            fld dword ptr [esp + 4]
            fmul st(0), st(2)
            fxch st(5)
            faddp st(1), st(0)
            fld dword ptr [esp + 0x24]
            fmul dword ptr [esp + 0x68]
            fxch st(3)
            faddp st(1), st(0)
            fld dword ptr [esp + 8]
            fmul st(0), st(2)
            fxch st(3)
            faddp st(5), st(0)
            fchs
            fxch st(4)
            fst dword ptr [esp + 0x5c]
            fld dword ptr [esp + 0x5c]
            fxch st(1)
            fmul dword ptr [esp + 0x30]
            fxch st(5)
            fstp dword ptr [esi + 0xc]
            fmul dword ptr [esp + 0x64]
            fld dword ptr [esp + 0x28]
            fld dword ptr [esp + 0x18]
            fmul dword ptr [esp + 0x68]
            faddp st(4), st(0)
            fld dword ptr [esp + 0x14]
            fxch st(4)
            fmul dword ptr [esp + 0xc]
            fld dword ptr [esp + 0x14]
            fxch st(5)
            fmul dword ptr [esp + 0x58]
            faddp st(1), st(0)
            fxch st(2)
            faddp st(2), st(0)
            fmul dword ptr [esp + 0x68]
            faddp st(4), st(0)
            mov eax, dword ptr [esp + 8]
            mov ecx, dword ptr [esp + 0x54]
            push edx
            mov edx, dword ptr [esp + 0x2c]
            fstp dword ptr [esi + 0x1c]
            fxch st(2)
            fmul dword ptr [esp + 0x10]
            fxch st(1)
            fmul dword ptr [esp + 0x54]
            push eax
            mov eax, dword ptr [esp + 0x18]
            push ecx
            mov ecx, dword ptr [esp + 0x40]
            faddp st(1), st(0)
            fxch st(2)
            faddp st(2), st(0)
            fxch st(1)
            push edx
            mov edx, dword ptr [esp + 0x48]
            push eax
            mov eax, dword ptr [esp + 0x7c]
            fchs
            fstp dword ptr [esi + 0x2c]
            push ecx
            mov ecx, dword ptr [esp + 0x2c]
            push edx
            push eax
            fstp st(0)
            push ecx
            call Determinant3
            fstp dword ptr [esi + 0x3c]
            pop esi
            add esp, 0x5c
            ret
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00536820
void C2_HOOK_CDECL BrMatrix4Perspective(br_matrix4* mat, br_angle field_of_view, br_scalar aspect, br_scalar hither, br_scalar yon) {
    br_scalar scale;

    scale = BR_COS(field_of_view / 2) / BR_SIN(field_of_view / 2);

    M(0, 0) = scale / aspect;
    M(1, 1) = scale;
    M(2, 2) = (yon + hither) / (yon - hither);
    M(3, 2) = BR_SCALAR(-2.f) * ((yon * hither) / (yon - hither));

    M(0, 1) = BR_SCALAR(0.f);
    M(0, 2) = BR_SCALAR(0.f);
    M(0, 3) = BR_SCALAR(0.f);
    M(1, 0) = BR_SCALAR(0.f);
    M(1, 2) = BR_SCALAR(0.f);
    M(1, 3) = BR_SCALAR(0.f);
    M(2, 0) = BR_SCALAR(0.f);
    M(2, 1) = BR_SCALAR(0.f);
    M(2, 3) = BR_SCALAR(-1.f);
    M(3, 0) = BR_SCALAR(0.f);
    M(3, 1) = BR_SCALAR(0.f);
    M(3, 3) = BR_SCALAR(0.f);
}

// FUNCTION: CARMA2_HW 0x005368e0
void C2_NAKED C2_HOOK_CDECL BrMatrix4Apply(br_vector4* A, const br_vector4* B, const br_matrix4* C) {
#ifndef CARPOCALYPSE2_MATCHING

    A->v[0] = BR_MAC4(B->v[0], C(0, 0), B->v[1], C(1, 0), B->v[2], C(2, 0), B->v[3], C(3, 0));
    A->v[1] = BR_MAC4(B->v[0], C(0, 1), B->v[1], C(1, 1), B->v[2], C(2, 1), B->v[3], C(3, 1));
    A->v[2] = BR_MAC4(B->v[0], C(0, 2), B->v[1], C(1, 2), B->v[2], C(2, 2), B->v[3], C(3, 2));
    A->v[3] = BR_MAC4(B->v[0], C(0, 3), B->v[1], C(1, 3), B->v[2], C(2, 3), B->v[3], C(3, 3));
#else
    __asm {
            mov eax, dword ptr [esp + 0xc]
            mov ecx, dword ptr [esp + 8]
            mov edx, dword ptr [esp + 4]
            fld dword ptr [eax + 0x30]
            fld dword ptr [eax + 0x20]
            fld dword ptr [ecx + 0xc]
            fld dword ptr [ecx + 8]
            fld dword ptr [eax + 0x10]
            fxch st(2)
            fmulp st(4), st(0)
            fmulp st(2), st(0)
            fxch st(1)
            faddp st(2), st(0)
            fmul dword ptr [ecx + 4]
            _emit 0xd9
        _emit 0x00
            _emit 0xd8
        _emit 0x09
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            _emit 0xd9
        _emit 0x1a
            fld dword ptr [eax + 0x34]
            fld dword ptr [ecx + 0xc]
            fld dword ptr [eax + 0x24]
            fld dword ptr [ecx + 8]
            fld dword ptr [eax + 0x14]
            fxch st(3)
            fmulp st(4), st(0)
            fmulp st(1), st(0)
            faddp st(2), st(0)
            fmul dword ptr [ecx + 4]
            fld dword ptr [eax + 4]
            _emit 0xd8
        _emit 0x09
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fstp dword ptr [edx + 4]
            fld dword ptr [eax + 0x38]
            fld dword ptr [ecx + 0xc]
            fld dword ptr [eax + 0x28]
            fld dword ptr [ecx + 8]
            fld dword ptr [eax + 0x18]
            fxch st(3)
            fmulp st(4), st(0)
            fmulp st(1), st(0)
            faddp st(2), st(0)
            fmul dword ptr [ecx + 4]
            fld dword ptr [eax + 8]
            _emit 0xd8
        _emit 0x09
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fstp dword ptr [edx + 8]
            fld dword ptr [eax + 0x3c]
            fld dword ptr [ecx + 0xc]
            fld dword ptr [eax + 0x2c]
            fld dword ptr [ecx + 8]
            fld dword ptr [eax + 0x1c]
            fxch st(3)
            fmulp st(4), st(0)
            fmulp st(1), st(0)
            faddp st(2), st(0)
            fmul dword ptr [ecx + 4]
            fld dword ptr [eax + 0xc]
            _emit 0xd8
        _emit 0x09
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fstp dword ptr [edx + 0xc]
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00536990
void C2_NAKED C2_HOOK_CDECL BrMatrix4ApplyP(br_vector4* A, const br_vector3* B, const br_matrix4* C) {
#ifndef CARPOCALYPSE2_MATCHING

    A->v[0] = BR_MAC3(B->v[0], C(0, 0), B->v[1], C(1, 0), B->v[2], C(2, 0)) + C(3, 0);
    A->v[1] = BR_MAC3(B->v[0], C(0, 1), B->v[1], C(1, 1), B->v[2], C(2, 1)) + C(3, 1);
    A->v[2] = BR_MAC3(B->v[0], C(0, 2), B->v[1], C(1, 2), B->v[2], C(2, 2)) + C(3, 2);
    A->v[3] = BR_MAC3(B->v[0], C(0, 3), B->v[1], C(1, 3), B->v[2], C(2, 3)) + C(3, 3);
#else
    __asm {
            mov eax, dword ptr [esp + 0xc]
            mov ecx, dword ptr [esp + 8]
            mov edx, dword ptr [esp + 4]
            fld dword ptr [eax + 0x20]
            fld dword ptr [eax + 0x10]
            fld dword ptr [ecx + 8]
            fld dword ptr [ecx + 4]
            _emit 0xd9
        _emit 0x00
            fxch st(2)
            fmulp st(4), st(0)
            fmulp st(2), st(0)
            _emit 0xd8
        _emit 0x09
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fadd dword ptr [eax + 0x30]
            _emit 0xd9
        _emit 0x1a
            fld dword ptr [eax + 0x24]
            fld dword ptr [ecx + 8]
            fld dword ptr [eax + 0x14]
            fld dword ptr [eax + 4]
            fxch st(2)
            fmulp st(3), st(0)
            fmul dword ptr [ecx + 4]
            fxch st(1)
            _emit 0xd8
        _emit 0x09
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fadd dword ptr [eax + 0x34]
            fstp dword ptr [edx + 4]
            fld dword ptr [eax + 0x28]
            fld dword ptr [ecx + 8]
            fld dword ptr [eax + 0x18]
            fld dword ptr [eax + 8]
            fxch st(2)
            fmulp st(3), st(0)
            fmul dword ptr [ecx + 4]
            fxch st(1)
            _emit 0xd8
        _emit 0x09
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fadd dword ptr [eax + 0x38]
            fstp dword ptr [edx + 8]
            fld dword ptr [eax + 0x2c]
            fld dword ptr [ecx + 8]
            fld dword ptr [eax + 0x1c]
            fld dword ptr [eax + 0xc]
            fxch st(2)
            fmulp st(3), st(0)
            fmul dword ptr [ecx + 4]
            fxch st(1)
            _emit 0xd8
        _emit 0x09
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fadd dword ptr [eax + 0x3c]
            fstp dword ptr [edx + 0xc]
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00536a30
void C2_NAKED C2_HOOK_CDECL BrMatrix4ApplyV(br_vector4* A, const br_vector3* B, const br_matrix4* C) {
#ifndef CARPOCALYPSE2_MATCHING

    A->v[0] = BR_MAC3(B->v[0], C(0, 0), B->v[1], C(1, 0), B->v[2], C(2, 0));
    A->v[1] = BR_MAC3(B->v[0], C(0, 1), B->v[1], C(1, 1), B->v[2], C(2, 1));
    A->v[2] = BR_MAC3(B->v[0], C(0, 2), B->v[1], C(1, 2), B->v[2], C(2, 2));
    A->v[3] = BR_MAC3(B->v[0], C(0, 3), B->v[1], C(1, 3), B->v[2], C(2, 3));
#else
    __asm {
            mov eax, dword ptr [esp + 0xc]
            mov ecx, dword ptr [esp + 8]
            mov edx, dword ptr [esp + 4]
            fld dword ptr [eax + 0x10]
            fld dword ptr [eax + 0x20]
            fld dword ptr [ecx + 4]
            fld dword ptr [ecx + 8]
            _emit 0xd9
        _emit 0x00
            fxch st(2)
            fmulp st(4), st(0)
            fmulp st(2), st(0)
            _emit 0xd8
        _emit 0x09
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            _emit 0xd9
        _emit 0x1a
            fld dword ptr [eax + 0x14]
            fld dword ptr [ecx + 4]
            fld dword ptr [eax + 0x24]
            fld dword ptr [eax + 4]
            fxch st(2)
            fmulp st(3), st(0)
            fmul dword ptr [ecx + 8]
            fxch st(1)
            _emit 0xd8
        _emit 0x09
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fstp dword ptr [edx + 4]
            fld dword ptr [eax + 0x18]
            fld dword ptr [ecx + 4]
            fld dword ptr [eax + 0x28]
            fld dword ptr [eax + 8]
            fxch st(2)
            fmulp st(3), st(0)
            fmul dword ptr [ecx + 8]
            fxch st(1)
            _emit 0xd8
        _emit 0x09
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fstp dword ptr [edx + 8]
            fld dword ptr [eax + 0x1c]
            fld dword ptr [ecx + 4]
            fld dword ptr [eax + 0x2c]
            fld dword ptr [eax + 0xc]
            fxch st(2)
            fmulp st(3), st(0)
            fmul dword ptr [ecx + 8]
            fxch st(1)
            _emit 0xd8
        _emit 0x09
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fstp dword ptr [edx + 0xc]
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00536ac0
void C2_NAKED C2_HOOK_CDECL BrMatrix4TApply(br_vector4* A, const br_vector4* B, const br_matrix4* C) {
#ifndef CARPOCALYPSE2_MATCHING

    A->v[0] = BR_MAC4(B->v[0], C(0, 0), B->v[1], C(0, 1), B->v[2], C(0, 2), B->v[3], C(0, 3));
    A->v[1] = BR_MAC4(B->v[0], C(1, 0), B->v[1], C(1, 1), B->v[2], C(1, 2), B->v[3], C(1, 3));
    A->v[2] = BR_MAC4(B->v[0], C(2, 0), B->v[1], C(2, 1), B->v[2], C(2, 2), B->v[3], C(2, 3));
    A->v[3] = BR_MAC4(B->v[0], C(3, 0), B->v[1], C(3, 1), B->v[2], C(3, 2), B->v[3], C(3, 3));
#else
    __asm {
            mov eax, dword ptr [esp + 0xc]
            mov ecx, dword ptr [esp + 8]
            mov edx, dword ptr [esp + 4]
            fld dword ptr [eax + 0xc]
            fld dword ptr [eax + 8]
            fld dword ptr [ecx + 0xc]
            fld dword ptr [ecx + 8]
            fld dword ptr [eax + 4]
            fxch st(2)
            fmulp st(4), st(0)
            fmulp st(2), st(0)
            fxch st(1)
            faddp st(2), st(0)
            fmul dword ptr [ecx + 4]
            _emit 0xd9
        _emit 0x00
            _emit 0xd8
        _emit 0x09
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            _emit 0xd9
        _emit 0x1a
            fld dword ptr [eax + 0x18]
            fld dword ptr [ecx + 8]
            fld dword ptr [eax + 0x14]
            fld dword ptr [ecx + 4]
            fld dword ptr [eax + 0x1c]
            fxch st(3)
            fmulp st(4), st(0)
            fmulp st(1), st(0)
            faddp st(2), st(0)
            fmul dword ptr [ecx + 0xc]
            fld dword ptr [eax + 0x10]
            _emit 0xd8
        _emit 0x09
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fstp dword ptr [edx + 4]
            fld dword ptr [eax + 0x2c]
            fld dword ptr [ecx + 0xc]
            fld dword ptr [eax + 0x28]
            fld dword ptr [ecx + 8]
            fld dword ptr [eax + 0x24]
            fxch st(3)
            fmulp st(4), st(0)
            fmulp st(1), st(0)
            faddp st(2), st(0)
            fmul dword ptr [ecx + 4]
            fld dword ptr [eax + 0x20]
            _emit 0xd8
        _emit 0x09
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fstp dword ptr [edx + 8]
            fld dword ptr [eax + 0x38]
            fld dword ptr [ecx + 8]
            fld dword ptr [eax + 0x34]
            fld dword ptr [ecx + 4]
            fld dword ptr [eax + 0x3c]
            fxch st(3)
            fmulp st(4), st(0)
            fmulp st(1), st(0)
            faddp st(2), st(0)
            fmul dword ptr [ecx + 0xc]
            fld dword ptr [eax + 0x30]
            _emit 0xd8
        _emit 0x09
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fstp dword ptr [edx + 0xc]
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00536b70
void C2_HOOK_CDECL BrMatrix4TApplyP(br_vector4* A, const br_vector3* B, const br_matrix4* C) {

    A->v[0] = BR_MAC3(B->v[0], C(0, 0), B->v[1], C(0, 1), B->v[2], C(0, 2)) + C(0, 3);
    A->v[1] = BR_MAC3(B->v[0], C(1, 0), B->v[1], C(1, 1), B->v[2], C(1, 2)) + C(1, 3);
    A->v[2] = BR_MAC3(B->v[0], C(2, 0), B->v[1], C(2, 1), B->v[2], C(2, 2)) + C(2, 3);
    A->v[3] = BR_MAC3(B->v[0], C(3, 0), B->v[1], C(3, 1), B->v[2], C(3, 2)) + C(3, 3);
}

// FUNCTION: CARMA2_HW 0x00536c10
void C2_HOOK_CDECL BrMatrix4TApplyV(br_vector4* A, const br_vector3* B, const br_matrix4* C) {

    A->v[0] = BR_MAC3(B->v[0], C(0, 0), B->v[1], C(0, 1), B->v[2], C(0, 2));
    A->v[1] = BR_MAC3(B->v[0], C(1, 0), B->v[1], C(1, 1), B->v[2], C(1, 2));
    A->v[2] = BR_MAC3(B->v[0], C(2, 0), B->v[1], C(2, 1), B->v[2], C(2, 2));
    A->v[3] = BR_MAC3(B->v[0], C(3, 0), B->v[1], C(3, 1), B->v[2], C(3, 2));
}

// FUNCTION: CARMA2_HW 0x00536ca0
void C2_NAKED C2_HOOK_CDECL BrMatrix4Copy34(br_matrix4* A, const br_matrix34* B) {
#ifndef CARPOCALYPSE2_MATCHING

    A(0, 0) = B(0, 0);
    A(0, 1) = B(0, 1);
    A(0, 2) = B(0, 2);
    A(0, 3) = BR_SCALAR(0.f);
    A(1, 0) = B(1, 0);
    A(1, 1) = B(1, 1);
    A(1, 2) = B(1, 2);
    A(1, 3) = BR_SCALAR(0.f);
    A(2, 0) = B(2, 0);
    A(2, 1) = B(2, 1);
    A(2, 2) = B(2, 2);
    A(2, 3) = BR_SCALAR(0.f);
    A(3, 0) = B(3, 0);
    A(3, 1) = B(3, 1);
    A(3, 2) = B(3, 2);
    A(3, 3) = BR_SCALAR(1.f);
#else
    __asm {
            mov ecx, dword ptr [esp + 8]
            mov eax, dword ptr [esp + 4]
            mov edx, dword ptr [ecx]
            mov dword ptr [eax], edx
            mov edx, dword ptr [ecx + 4]
            mov dword ptr [eax + 4], edx
            mov edx, dword ptr [ecx + 8]
            mov dword ptr [eax + 8], edx
            mov edx, dword ptr [ecx + 0xc]
            mov dword ptr [eax + 0x10], edx
            mov edx, dword ptr [ecx + 0x10]
            mov dword ptr [eax + 0x14], edx
            mov edx, dword ptr [ecx + 0x14]
            mov dword ptr [eax + 0x18], edx
            mov edx, dword ptr [ecx + 0x18]
            mov dword ptr [eax + 0x20], edx
            mov edx, dword ptr [ecx + 0x1c]
            mov dword ptr [eax + 0x24], edx
            mov edx, dword ptr [ecx + 0x20]
            mov dword ptr [eax + 0x28], edx
            mov edx, dword ptr [ecx + 0x24]
            mov dword ptr [eax + 0x30], edx
            mov edx, dword ptr [ecx + 0x28]
            mov dword ptr [eax + 0x34], edx
            mov ecx, dword ptr [ecx + 0x2c]
            mov dword ptr [eax + 0x38], ecx
            xor ecx, ecx
            mov dword ptr [eax + 0xc], ecx
            mov dword ptr [eax + 0x1c], ecx
            mov dword ptr [eax + 0x2c], ecx
            mov dword ptr [eax + 0x3c], 0x3f800000
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

// FUNCTION: CARMA2_HW 0x00536d10
void C2_HOOK_CDECL BrMatrix4Mul34(br_matrix4* A, const br_matrix34* B, const br_matrix4* C) {

    A(0, 0) = BR_MAC3(B(0, 0), C(0, 0), B(0, 1), C(1, 0), B(0, 2), C(2, 0));
    A(0, 1) = BR_MAC3(B(0, 0), C(0, 1), B(0, 1), C(1, 1), B(0, 2), C(2, 1));
    A(0, 2) = BR_MAC3(B(0, 0), C(0, 2), B(0, 1), C(1, 2), B(0, 2), C(2, 2));
    A(0, 3) = BR_MAC3(B(0, 0), C(0, 3), B(0, 1), C(1, 3), B(0, 2), C(2, 3));
    A(1, 0) = BR_MAC3(B(1, 0), C(0, 0), B(1, 1), C(1, 0), B(1, 2), C(2, 0));
    A(1, 1) = BR_MAC3(B(1, 0), C(0, 1), B(1, 1), C(1, 1), B(1, 2), C(2, 1));
    A(1, 2) = BR_MAC3(B(1, 0), C(0, 2), B(1, 1), C(1, 2), B(1, 2), C(2, 2));
    A(1, 3) = BR_MAC3(B(1, 0), C(0, 3), B(1, 1), C(1, 3), B(1, 2), C(2, 3));
    A(2, 0) = BR_MAC3(B(2, 0), C(0, 0), B(2, 1), C(1, 0), B(2, 2), C(2, 0));
    A(2, 1) = BR_MAC3(B(2, 0), C(0, 1), B(2, 1), C(1, 1), B(2, 2), C(2, 1));
    A(2, 2) = BR_MAC3(B(2, 0), C(0, 2), B(2, 1), C(1, 2), B(2, 2), C(2, 2));
    A(2, 3) = BR_MAC3(B(2, 0), C(0, 3), B(2, 1), C(1, 3), B(2, 2), C(2, 3));
    A(3, 0) = BR_MAC3(B(3, 0), C(0, 0), B(3, 1), C(1, 0), B(3, 2), C(2, 0)) + C(3, 0);
    A(3, 1) = BR_MAC3(B(3, 0), C(0, 1), B(3, 1), C(1, 1), B(3, 2), C(2, 1)) + C(3, 1);
    A(3, 2) = BR_MAC3(B(3, 0), C(0, 2), B(3, 1), C(1, 2), B(3, 2), C(2, 2)) + C(3, 2);
    A(3, 3) = BR_MAC3(B(3, 0), C(0, 3), B(3, 1), C(1, 3), B(3, 2), C(2, 3)) + C(3, 3);
}

// FUNCTION: CARMA2_HW 0x00536f30
void C2_HOOK_CDECL BrMatrix4Pre34(br_matrix4* A, const br_matrix34* B) {
    br_matrix4 C;

    memcpy(&C, A, sizeof(*A));
    BrMatrix4Mul34(A, B, &C);
}

// FUNCTION: CARMA2_HW 0x00536f60
void C2_HOOK_CDECL BrMatrix4ShearZ(br_matrix4* mat, br_scalar sx, br_scalar sy) {

    M(0, 0) = BR_SCALAR(1.f);
    M(0, 1) = BR_SCALAR(0.f);
    M(0, 2) = BR_SCALAR(0.f);
    M(0, 3) = BR_SCALAR(0.f);
    M(1, 0) = BR_SCALAR(0.f);
    M(1, 1) = BR_SCALAR(1.f);
    M(1, 2) = BR_SCALAR(0.f);
    M(1, 3) = BR_SCALAR(0.f);
    M(2, 0) = sx;
    M(2, 1) = sy;
    M(2, 2) = BR_SCALAR(1.f);
    M(2, 3) = BR_SCALAR(0.f);
    M(3, 0) = BR_SCALAR(0.f);
    M(3, 1) = BR_SCALAR(0.f);
    M(3, 2) = BR_SCALAR(0.f);
    M(3, 3) = BR_SCALAR(1.f);
}
