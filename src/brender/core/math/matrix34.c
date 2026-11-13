#include "matrix34.h"

#include "vector.h"

#include "c2_math.h"


// GLOBAL: CARMA2_HW 0x006b09c0
br_matrix34 mattmp1;

// GLOBAL: CARMA2_HW 0x006b09f0
br_matrix34 mattmp2;

#define M(x, y) mat->m[x][y]
#define A(x, y) A->m[x][y]
#define B(x, y) B->m[x][y]
#define C(x, y) C->m[x][y]
#define BR_MAC3(a, b, c, d, e, f) ((a) * (b) + (c) * (d) + (e) * (f))
#define BR_MAC4(a, b, c, d, e, f, g, h) ((a) * (b) + (c) * (d) + (e) * (f) + (g) * (h))

#ifdef CARPOCALYPSE2_MATCHING
extern char IID_IDirectInputDevice2A[];
#endif
// FUNCTION: CARMA2_HW 0x005325d0
void C2_HOOK_CDECL BrMatrix34Copy(br_matrix34* A, const br_matrix34* B) {

    A(0, 0) = B(0, 0);
    A(0, 1) = B(0, 1);
    A(0, 2) = B(0, 2);

    A(1, 0) = B(1, 0);
    A(1, 1) = B(1, 1);
    A(1, 2) = B(1, 2);

    A(2, 0) = B(2, 0);
    A(2, 1) = B(2, 1);
    A(2, 2) = B(2, 2);

    A(3, 0) = B(3, 0);
    A(3, 1) = B(3, 1);
    A(3, 2) = B(3, 2);
}

// FUNCTION: CARMA2_HW 0x00532620
void C2_HOOK_CDECL BrMatrix34Mul(br_matrix34* A, const br_matrix34* B, const br_matrix34* C) {

    A(0, 0) = BR_MAC3(B(0, 0), C(0, 0), B(0, 1), C(1, 0), B(0, 2), C(2, 0));
    A(0, 1) = BR_MAC3(B(0, 0), C(0, 1), B(0, 1), C(1, 1), B(0, 2), C(2, 1));
    A(0, 2) = BR_MAC3(B(0, 0), C(0, 2), B(0, 1), C(1, 2), B(0, 2), C(2, 2));
    A(1, 0) = BR_MAC3(B(1, 0), C(0, 0), B(1, 1), C(1, 0), B(1, 2), C(2, 0));
    A(1, 1) = BR_MAC3(B(1, 0), C(0, 1), B(1, 1), C(1, 1), B(1, 2), C(2, 1));
    A(1, 2) = BR_MAC3(B(1, 0), C(0, 2), B(1, 1), C(1, 2), B(1, 2), C(2, 2));
    A(2, 0) = BR_MAC3(B(2, 0), C(0, 0), B(2, 1), C(1, 0), B(2, 2), C(2, 0));
    A(2, 1) = BR_MAC3(B(2, 0), C(0, 1), B(2, 1), C(1, 1), B(2, 2), C(2, 1));
    A(2, 2) = BR_MAC3(B(2, 0), C(0, 2), B(2, 1), C(1, 2), B(2, 2), C(2, 2));
    A(3, 0) = BR_MAC3(B(3, 0), C(0, 0), B(3, 1), C(1, 0), B(3, 2), C(2, 0)) + C(3, 0);
    A(3, 1) = BR_MAC3(B(3, 0), C(0, 1), B(3, 1), C(1, 1), B(3, 2), C(2, 1)) + C(3, 1);
    A(3, 2) = BR_MAC3(B(3, 0), C(0, 2), B(3, 1), C(1, 2), B(3, 2), C(2, 2)) + C(3, 2);
}

// FUNCTION: CARMA2_HW 0x005327c0
void C2_HOOK_CDECL BrMatrix34Identity(br_matrix34* mat) {
    // { 1, 0, 0},
    // { 0, 1, 0},
    // { 0, 0, 1}
    // ( 0, 0, 0 }
    M(0, 0) = 1.f;
    M(0, 1) = 0.f;
    M(0, 2) = 0.f;

    M(1, 0) = 0.f;
    M(1, 1) = 1.f;
    M(1, 2) = 0.f;

    M(2, 0) = 0.f;
    M(2, 1) = 0.f;
    M(2, 2) = 1.f;

    M(3, 0) = 0.f;
    M(3, 1) = 0.f;
    M(3, 2) = 0.f;
}

// FUNCTION: CARMA2_HW 0x005327f0
void C2_HOOK_CDECL BrMatrix34RotateX(br_matrix34* mat, br_angle rx) {
    br_scalar s;
    br_scalar c;

    s = BR_SIN(rx);
    c = BR_COS(rx);

    M(0, 0) = 1.f;
    M(0, 1) = 0;
    M(0, 2) = 0;
    M(1, 0) = 0;
    M(1, 1) = c;
    M(1, 2) = s;
    M(2, 0) = 0;
    M(2, 1) = -s;
    M(2, 2) = c;
    M(3, 0) = 0;
    M(3, 1) = 0;
    M(3, 2) = 0;
}

// FUNCTION: CARMA2_HW 0x00532860
void C2_HOOK_CDECL BrMatrix34RotateY(br_matrix34* mat, br_angle ry) {
    br_scalar s;
    br_scalar c;

    s = BR_SIN(ry);
    c = BR_COS(ry);

    M(0, 0) = c;
    M(0, 1) = 0;
    M(0, 2) = -s;
    M(1, 0) = 0;
    M(1, 1) = 1;
    M(1, 2) = 0;
    M(2, 0) = s;
    M(2, 1) = 0;
    M(2, 2) = c;
    M(3, 0) = 0;
    M(3, 1) = 0;
    M(3, 2) = 0;
}

// FUNCTION: CARMA2_HW 0x005328d0
void C2_HOOK_CDECL BrMatrix34RotateZ(br_matrix34* mat, br_angle rz) {
    br_scalar s;
    br_scalar c;

    s = BR_SIN(rz);
    c = BR_COS(rz);

    M(0, 0) = c;
    M(0, 1) = s;
    M(0, 2) = 0;
    M(1, 0) = -s;
    M(1, 1) = c;
    M(1, 2) = 0;
    M(2, 0) = 0;
    M(2, 1) = 0;
    M(2, 2) = 1;
    M(3, 0) = 0;
    M(3, 1) = 0;
    M(3, 2) = 0;
}

// FUNCTION: CARMA2_HW 0x00532940
void C2_NAKED C2_HOOK_CDECL BrMatrix34Rotate(br_matrix34* mat, br_angle r, const br_vector3* a) {
#ifndef CARPOCALYPSE2_MATCHING
    br_scalar t;
    br_scalar s;
    br_scalar c;
    br_scalar txy;
    br_scalar txz;
    br_scalar tyz;
    br_scalar sx;
    br_scalar sy;
    br_scalar sz;

    s = BR_SIN(r);
    c = BR_COS(r);
    t = 1.0f - c;

    txy = t * a->v[0];
    txz = txy * a->v[2];
    txy = txy * a->v[1];
    tyz = t * a->v[1] * a->v[2];

    sx = s * a->v[0];
    sy = s * a->v[1];
    sz = s * a->v[2];

    M(0, 0) = t * a->v[0] * a->v[0] + c;
    M(0, 1) = txy + sz;
    M(0, 2) = txz - sy;
    M(1, 0) = txy - sz;
    M(1, 1) = t * a->v[1] * a->v[1] + c;
    M(1, 2) = tyz + sx;
    M(2, 0) = txz + sy;
    M(2, 1) = tyz - sx;
    M(2, 2) = t * a->v[2] * a->v[2] + c;

    M(3, 0) = M(3, 1) = M(3, 2) = 0.0f;
#else
    __asm {
            sub esp, 0x18
            mov eax, dword ptr [esp + 0x20]
            mov ecx, dword ptr [esp + 0x24]
            and eax, 0xffff
            mov dword ptr [esp + 0x20], eax
            fild dword ptr [esp + 0x20]
            fld dword ptr [IID_IDirectInputDevice2A+0x520]
            _emit 0xd9
        _emit 0x01
            fxch st(2)
            fmul qword ptr [IID_IDirectInputDevice2A+0x518]
            fld dword ptr [ecx + 8]
            fld dword ptr [ecx + 4]
            fld st(2)
            fcos
            fld st(3)
            fsin
            fst dword ptr [esp + 0x20]
            fst dword ptr [esp + 4]
            fxch st(1)
            fst dword ptr [esp + 0x20]
            _emit 0xd9
        _emit 0x5c
        _emit 0x24
        _emit 0x00
            fxch st(4)
            _emit 0xd8
        _emit 0x64
        _emit 0x24
        _emit 0x00
            fld dword ptr [ecx + 8]
            fxch st(1)
            fst dword ptr [esp + 0x24]
            _emit 0xd8
        _emit 0x09
            fstp dword ptr [esp + 0x20]
            fxch st(2)
            fmul dword ptr [esp + 0x20]
            fxch st(1)
            fmul dword ptr [esp + 0x20]
            fxch st(1)
            fst dword ptr [esp + 0x10]
            fxch st(1)
            fst dword ptr [esp + 0x20]
            fld dword ptr [ecx + 8]
            fld dword ptr [ecx + 4]
            fmulp st(4), st(0)
            fxch st(3)
            fmul dword ptr [esp + 0x24]
            fstp dword ptr [esp + 0xc]
            fxch st(5)
            fst dword ptr [esp + 8]
            fmul dword ptr [esp + 8]
            fld dword ptr [ecx + 4]
            fxch st(5)
            _emit 0xd8
        _emit 0x09
            fstp dword ptr [esp + 0x14]
            fxch st(2)
            fmul dword ptr [esp + 4]
            fxch st(4)
            fmul dword ptr [esp + 4]
            fxch st(2)
            fmul dword ptr [esp + 0x24]
            fxch st(4)
            fst dword ptr [esp + 8]
            fadd dword ptr [esp + 0x20]
            fxch st(5)
            fsub dword ptr [esp + 8]
            fxch st(2)
            fstp dword ptr [esp + 0x20]
            fsub dword ptr [esp + 0x20]
            fxch st(3)
            _emit 0xd8
        _emit 0x44
        _emit 0x24
        _emit 0x00
            fxch st(4)
            mov eax, dword ptr [esp + 0x1c]
            fstp dword ptr [eax + 4]
            fxch st(2)
            fstp dword ptr [eax + 8]
            fxch st(1)
            fstp dword ptr [eax + 0xc]
            fxch st(1)
            _emit 0xd9
        _emit 0x18
            fstp st(0)
            fld dword ptr [ecx + 4]
            fld st(0)
            fmul st(0), st(1)
            fld dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x20]
            fxch st(2)
            fmul dword ptr [esp + 0x24]
            fld dword ptr [esp + 0xc]
            fxch st(2)
            fadd dword ptr [esp + 0xc]
            fxch st(3)
            fadd dword ptr [esp + 0x10]
            fxch st(2)
            fsub dword ptr [esp + 0x14]
            fxch st(1)
            _emit 0xd8
        _emit 0x44
        _emit 0x24
        _emit 0x00
            fxch st(3)
            fstp dword ptr [eax + 0x14]
            fxch st(1)
            fstp dword ptr [eax + 0x18]
            fstp dword ptr [eax + 0x1c]
            fstp dword ptr [eax + 0x10]
            fstp st(0)
            fld dword ptr [ecx + 8]
            fld st(0)
            fmul st(0), st(1)
            xor ecx, ecx
            mov dword ptr [eax + 0x2c], ecx
            mov dword ptr [eax + 0x28], ecx
            fmul dword ptr [esp + 0x24]
            mov dword ptr [eax + 0x24], ecx
            _emit 0xd8
        _emit 0x44
        _emit 0x24
        _emit 0x00
            fstp dword ptr [eax + 0x20]
            fstp st(0)
            add esp, 0x18
            ret
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00532a90
void C2_HOOK_CDECL BrMatrix34Translate(br_matrix34* mat, br_scalar dx, br_scalar dy, br_scalar dz) {

    M(0, 0) = 1.f;
    M(0, 1) = 0.f;
    M(0, 2) = 0.f;
    M(1, 0) = 0.f;
    M(1, 1) = 1.f;
    M(1, 2) = 0.f;
    M(2, 0) = 0.f;
    M(2, 1) = 0.f;
    M(2, 2) = 1.f;
    M(3, 0) = dx;
    M(3, 1) = dy;
    M(3, 2) = dz;
}

// FUNCTION: CARMA2_HW 0x00532ad0
void C2_HOOK_CDECL BrMatrix34Scale(br_matrix34* mat, br_scalar sx, br_scalar sy, br_scalar sz) {

    M(0, 0) = sx;
    M(0, 1) = 0.f;
    M(0, 2) = 0.f;
    M(1, 0) = 0.f;
    M(1, 1) = sy;
    M(1, 2) = 0.f;
    M(2, 0) = 0.f;
    M(2, 1) = 0.f;
    M(2, 2) = sz;
    M(3, 0) = 0.f;
    M(3, 1) = 0.f;
    M(3, 2) = 0.f;
}

// FUNCTION: CARMA2_HW 0x00532b10
void C2_HOOK_CDECL BrMatrix34ShearX(br_matrix34* mat, br_scalar sy, br_scalar sz) {

    M(0, 0) = 1.f;
    M(0, 1) = sy;
    M(0, 2) = sz;
    M(1, 0) = 0.f;
    M(1, 1) = 1.f;
    M(1, 2) = 0.f;
    M(2, 0) = 0.f;
    M(2, 1) = 0.f;
    M(2, 2) = 1.f;
    M(3, 0) = 0.f;
    M(3, 1) = 0.f;
    M(3, 2) = 0.f;
}

// FUNCTION: CARMA2_HW 0x00532b50
void C2_HOOK_CDECL BrMatrix34ShearY(br_matrix34* mat, br_scalar sx, br_scalar sz) {

    M(0, 0) = 1.f;
    M(0, 1) = 0.f;
    M(0, 2) = 0.f;
    M(1, 0) = sx;
    M(1, 1) = 1.f;
    M(1, 2) = sz;
    M(2, 0) = 0.f;
    M(2, 1) = 0.f;
    M(2, 2) = 1.f;
    M(3, 0) = 0.f;
    M(3, 1) = 0.f;
    M(3, 2) = 0.f;
}

// FUNCTION: CARMA2_HW 0x00532b90
void C2_HOOK_CDECL BrMatrix34ShearZ(br_matrix34* mat, br_scalar sx, br_scalar sy) {

    M(0, 0) = 1.f;
    M(0, 1) = 0.f;
    M(0, 2) = 0.f;
    M(1, 0) = 0.f;
    M(1, 1) = 1.f;
    M(1, 2) = 0.f;
    M(2, 0) = sx;
    M(2, 1) = sy;
    M(2, 2) = 1.f;
    M(3, 0) = 0.f;
    M(3, 1) = 0.f;
    M(3, 2) = 0.f;
}

// FUNCTION: CARMA2_HW 0x00532bd0
br_scalar C2_NAKED C2_HOOK_CDECL BrMatrix34Inverse(br_matrix34* B, const br_matrix34* A) {
#ifndef CARPOCALYPSE2_MATCHING
    float idet;
    float det;
    float pos;
    float neg;
    float temp;
    float AF[4][3];
    float BF[4][3];
    int i;

#define AF(x, y) (AF[x][y])
#define BF(x, y) (BF[x][y])

#define ACCUMULATE   \
    if (temp >= 0.f) \
        pos += temp; \
    else             \
        neg += temp;
#define PRECISION_LIMIT BR_SCALAR(1.0e-15)
#define ABS(a) (((a) < 0) ? -(a) : (a))

    for (i = 0; i < 4; i++) {
        AF(i, 0) = A(i, 0);
        AF(i, 1) = A(i, 1);
        AF(i, 2) = A(i, 2);
    }

    pos = neg = 0.0F;
    temp = AF(0, 0) * AF(1, 1) * AF(2, 2);
    ACCUMULATE
    temp = AF(0, 1) * AF(1, 2) * AF(2, 0);
    ACCUMULATE
    temp = AF(0, 2) * AF(1, 0) * AF(2, 1);
    ACCUMULATE
    temp = -AF(0, 2) * AF(1, 1) * AF(2, 0);
    ACCUMULATE
    temp = -AF(0, 1) * AF(1, 0) * AF(2, 2);
    ACCUMULATE
    temp = -AF(0, 0) * AF(1, 2) * AF(2, 1);
    ACCUMULATE
    det = pos + neg;

    if (ABS(det) <= PRECISION_LIMIT)
        return 0.f;

    if ((ABS(det / (pos - neg)) < PRECISION_LIMIT)) {
        return 0.f;
    }

    idet = 1.f / det;

    BF(0, 0) = (AF(1, 1) * AF(2, 2) - AF(1, 2) * AF(2, 1)) * idet;
    BF(1, 0) = -(AF(1, 0) * AF(2, 2) - AF(1, 2) * AF(2, 0)) * idet;
    BF(2, 0) = (AF(1, 0) * AF(2, 1) - AF(1, 1) * AF(2, 0)) * idet;
    BF(0, 1) = -(AF(0, 1) * AF(2, 2) - AF(0, 2) * AF(2, 1)) * idet;
    BF(1, 1) = (AF(0, 0) * AF(2, 2) - AF(0, 2) * AF(2, 0)) * idet;
    BF(2, 1) = -(AF(0, 0) * AF(2, 1) - AF(0, 1) * AF(2, 0)) * idet;
    BF(0, 2) = (AF(0, 1) * AF(1, 2) - AF(0, 2) * AF(1, 1)) * idet;
    BF(1, 2) = -(AF(0, 0) * AF(1, 2) - AF(0, 2) * AF(1, 0)) * idet;
    BF(2, 2) = (AF(0, 0) * AF(1, 1) - AF(0, 1) * AF(1, 0)) * idet;

    BF(3, 0) = -(AF(3, 0) * BF(0, 0) + AF(3, 1) * BF(1, 0) + AF(3, 2) * BF(2, 0));
    BF(3, 1) = -(AF(3, 0) * BF(0, 1) + AF(3, 1) * BF(1, 1) + AF(3, 2) * BF(2, 1));
    BF(3, 2) = -(AF(3, 0) * BF(0, 2) + AF(3, 1) * BF(1, 2) + AF(3, 2) * BF(2, 2));

    for (i = 0; i < 4; i++) {
        B(i, 0) = BF(i, 0);
        B(i, 1) = BF(i, 1);
        B(i, 2) = BF(i, 2);
    }

    return det;
#else
    __asm {
            push ecx
            mov ecx, dword ptr [esp + 0xc]
            fld dword ptr [ecx + 0x10]
            _emit 0xd8
        _emit 0x09
            fmul dword ptr [ecx + 0x20]
            fld dword ptr [IID_IDirectInputDevice2A+0x524]
            fld dword ptr [IID_IDirectInputDevice2A+0x524]
            fxch st(2)
            fst dword ptr [esp + 0xc]
            fcomp dword ptr [IID_IDirectInputDevice2A+0x524]
            fxch st(1)
            _emit 0xd9
        _emit 0x5c
        _emit 0x24
        _emit 0x00
            fnstsw ax
            test ah, 1
            jne L00532c08
            fld dword ptr [esp + 0xc]
            jmp L00532c12
    L00532c08:
            fstp st(0)
            fld dword ptr [esp + 0xc]
            _emit 0xd9
        _emit 0x44
        _emit 0x24
        _emit 0x00
    L00532c12:
            fld dword ptr [ecx + 0x14]
            fmul dword ptr [ecx + 4]
            fmul dword ptr [ecx + 0x18]
            fcom dword ptr [IID_IDirectInputDevice2A+0x524]
            fnstsw ax
            test ah, 1
            jne L00532c2c
            faddp st(1), st(0)
            jmp L00532c2e
    L00532c2c:
            faddp st(2), st(0)
    L00532c2e:
            fld dword ptr [ecx + 8]
            fmul dword ptr [ecx + 0xc]
            fmul dword ptr [ecx + 0x1c]
            fcom dword ptr [IID_IDirectInputDevice2A+0x524]
            fnstsw ax
            test ah, 1
            jne L00532c48
            faddp st(1), st(0)
            jmp L00532c4a
    L00532c48:
            faddp st(2), st(0)
    L00532c4a:
            fld dword ptr [ecx + 8]
            fld dword ptr [ecx + 0x10]
            fmulp st(1), st(0)
            fmul dword ptr [ecx + 0x18]
            fchs
            fcom dword ptr [IID_IDirectInputDevice2A+0x524]
            fnstsw ax
            test ah, 1
            jne L00532c68
            faddp st(1), st(0)
            jmp L00532c6a
    L00532c68:
            faddp st(2), st(0)
    L00532c6a:
            fld dword ptr [ecx + 0xc]
            fld dword ptr [ecx + 4]
            fmulp st(1), st(0)
            fmul dword ptr [ecx + 0x20]
            fchs
            fcom dword ptr [IID_IDirectInputDevice2A+0x524]
            fnstsw ax
            test ah, 1
            jne L00532c88
            faddp st(1), st(0)
            jmp L00532c8a
    L00532c88:
            faddp st(2), st(0)
    L00532c8a:
            fld dword ptr [ecx + 0x14]
            _emit 0xd8
        _emit 0x09
            fmul dword ptr [ecx + 0x1c]
            fchs
            fcom dword ptr [IID_IDirectInputDevice2A+0x524]
            fnstsw ax
            test ah, 1
            jne L00532ca5
            faddp st(1), st(0)
            jmp L00532ca7
    L00532ca5:
            faddp st(2), st(0)
    L00532ca7:
            fld st(1)
            fadd st(0), st(1)
            fst dword ptr [esp + 0xc]
            fcomp dword ptr [IID_IDirectInputDevice2A+0x524]
            fld dword ptr [esp + 0xc]
            fnstsw ax
            test ah, 1
            je L00532cc2
            fchs
    L00532cc2:
            fcomp dword ptr [IID_IDirectInputDevice2A+0x528]
            fnstsw ax
            test ah, 0x41
            je L00532cdb
            fstp st(0)
            fstp st(0)
            fld dword ptr [IID_IDirectInputDevice2A+0x524]
            pop ecx
            ret
    L00532cdb:
            fsubrp st(1), st(0)
            fdivr dword ptr [esp + 0xc]
            fcom dword ptr [IID_IDirectInputDevice2A+0x524]
            fnstsw ax
            test ah, 1
            je L00532cf0
            fchs
    L00532cf0:
            fcomp dword ptr [IID_IDirectInputDevice2A+0x52c]
            fnstsw ax
            test ah, 1
            je L00532d05
            fld dword ptr [IID_IDirectInputDevice2A+0x524]
            pop ecx
            ret
    L00532d05:
            fld dword ptr [IID_IDirectInputDevice2A+0x520]
            fdiv dword ptr [esp + 0xc]
            mov eax, dword ptr [esp + 8]
            fld dword ptr [ecx + 0x14]
            fld dword ptr [ecx + 0x10]
            fld dword ptr [ecx + 0x20]
            fld dword ptr [ecx + 0x1c]
            fxch st(3)
            fchs
            fxch st(1)
            fmulp st(2), st(0)
            fxch st(2)
            fmulp st(2), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fmul st(0), st(1)
            _emit 0xd9
        _emit 0x18
            fld dword ptr [ecx + 0xc]
            fld dword ptr [ecx + 0x14]
            fld dword ptr [ecx + 0x18]
            fld dword ptr [ecx + 0x20]
            fxch st(3)
            fchs
            fxch st(1)
            fmulp st(2), st(0)
            fxch st(2)
            fmulp st(2), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fmul st(0), st(1)
            fstp dword ptr [eax + 0xc]
            fld dword ptr [ecx + 0x10]
            fld dword ptr [ecx + 0xc]
            fld dword ptr [ecx + 0x1c]
            fld dword ptr [ecx + 0x18]
            fxch st(3)
            fchs
            fxch st(1)
            fmulp st(2), st(0)
            fxch st(2)
            fmulp st(2), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fmul st(0), st(1)
            fstp dword ptr [eax + 0x18]
            fld dword ptr [ecx + 4]
            fld dword ptr [ecx + 8]
            fld dword ptr [ecx + 0x1c]
            fld dword ptr [ecx + 0x20]
            fxch st(3)
            fchs
            fxch st(1)
            fmulp st(2), st(0)
            fxch st(2)
            fmulp st(2), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fmul st(0), st(1)
            fstp dword ptr [eax + 4]
            fld dword ptr [ecx + 8]
            fld dword ptr [ecx + 0x20]
            fxch st(1)
            fchs
            fxch st(1)
            _emit 0xd8
        _emit 0x09
            fxch st(1)
            fmul dword ptr [ecx + 0x18]
            faddp st(1), st(0)
            fmul st(0), st(1)
            fstp dword ptr [eax + 0x10]
            _emit 0xd9
        _emit 0x01
            fld dword ptr [ecx + 0x1c]
            fld dword ptr [ecx + 4]
            fld dword ptr [ecx + 0x18]
            fxch st(3)
            fchs
            fxch st(2)
            fmulp st(2), st(0)
            fxch st(2)
            fmulp st(2), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fmul st(0), st(1)
            fstp dword ptr [eax + 0x1c]
            fld dword ptr [ecx + 8]
            fld dword ptr [ecx + 0x14]
            fld dword ptr [ecx + 4]
            fld dword ptr [ecx + 0x10]
            fxch st(1)
            fmulp st(2), st(0)
            fxch st(2)
            fchs
            fxch st(2)
            fmulp st(2), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fmul st(0), st(1)
            fstp dword ptr [eax + 8]
            _emit 0xd9
        _emit 0x01
            fld dword ptr [ecx + 8]
            fxch st(1)
            fchs
            fld dword ptr [ecx + 0xc]
            fxch st(1)
            fmul dword ptr [ecx + 0x14]
            fxch st(1)
            fmulp st(2), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fmul st(0), st(1)
            fstp dword ptr [eax + 0x14]
            fld dword ptr [ecx + 4]
            fld dword ptr [ecx + 0x10]
            _emit 0xd8
        _emit 0x09
            fxch st(1)
            fchs
            fmul dword ptr [ecx + 0xc]
            faddp st(1), st(0)
            fmul st(0), st(1)
            fstp dword ptr [eax + 0x20]
            fld dword ptr [ecx + 0x24]
            fld dword ptr [ecx + 0x2c]
            fld dword ptr [ecx + 0x28]
            fxch st(2)
            _emit 0xd8
        _emit 0x08
            fxch st(1)
            fmul dword ptr [eax + 0x18]
            fxch st(2)
            fmul dword ptr [eax + 0xc]
            fxch st(1)
            fxch st(2)
            fxch st(1)
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fchs
            fstp dword ptr [eax + 0x24]
            fstp st(0)
            fld dword ptr [ecx + 0x24]
            fld dword ptr [eax + 4]
            fld dword ptr [ecx + 0x28]
            fld dword ptr [eax + 0x10]
            fld dword ptr [ecx + 0x2c]
            fxch st(3)
            fmulp st(4), st(0)
            fmulp st(1), st(0)
            faddp st(2), st(0)
            fmul dword ptr [eax + 0x1c]
            fld dword ptr [eax + 8]
            fxch st(1)
            faddp st(2), st(0)
            fld dword ptr [eax + 0x14]
            fxch st(2)
            fchs
            fstp dword ptr [eax + 0x28]
            fld dword ptr [ecx + 0x24]
            fld dword ptr [ecx + 0x28]
            fld dword ptr [ecx + 0x2c]
            fxch st(3)
            fmulp st(2), st(0)
            fld dword ptr [eax + 0x20]
            fxch st(1)
            fmulp st(4), st(0)
            fxch st(3)
            faddp st(1), st(0)
            fxch st(1)
            fmulp st(2), st(0)
            fld dword ptr [esp + 0xc]
            fxch st(2)
            faddp st(1), st(0)
            fchs
            fstp dword ptr [eax + 0x2c]
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
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00532eb0
void C2_HOOK_CDECL BrMatrix34LPInverse(br_matrix34* A, const br_matrix34* B) {

    A(0, 0) = B(0, 0);
    A(0, 1) = B(1, 0);
    A(0, 2) = B(2, 0);

    A(1, 0) = B(0, 1);
    A(1, 1) = B(1, 1);
    A(1, 2) = B(2, 1);

    A(2, 0) = B(0, 2);
    A(2, 1) = B(1, 2);
    A(2, 2) = B(2, 2);

    A(3, 0) = -BR_MAC3(B(3, 0), A(0, 0), B(3, 1), A(1, 0), B(3, 2), A(2, 0));
    A(3, 1) = -BR_MAC3(B(3, 0), A(0, 1), B(3, 1), A(1, 1), B(3, 2), A(2, 1));
    A(3, 2) = -BR_MAC3(B(3, 0), A(0, 2), B(3, 1), A(1, 2), B(3, 2), A(2, 2));
}

// FUNCTION: CARMA2_HW 0x00532f60
void C2_NAKED C2_HOOK_CDECL BrMatrix34LPNormalise(br_matrix34* A, const br_matrix34* B) {
#ifndef CARPOCALYPSE2_MATCHING

    BrVector3Normalise((br_vector3*)A->m[2], (br_vector3*)B->m[2]);
    BrVector3Cross((br_vector3*)A->m[0], (br_vector3*)B->m[1], (br_vector3*)A->m[2]);
    BrVector3Normalise((br_vector3*)A->m[0], (br_vector3*)A->m[0]);
    BrVector3Cross((br_vector3*)A->m[1], (br_vector3*)A->m[2], (br_vector3*)A->m[0]);

    A(3, 0) = B(3, 0);
    A(3, 1) = B(3, 1);
    A(3, 2) = B(3, 2);
#else
    __asm {
            push ecx
            mov edx, dword ptr [esp + 0xc]
            push esi
            xor esi, esi
            fld dword ptr [edx + 0x18]
            fld dword ptr [edx + 0x1c]
            fld dword ptr [edx + 0x20]
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
            fcom dword ptr [IID_IDirectInputDevice2A+0x528]
            fnstsw ax
            test ah, 0x41
            jne L00532fce
            fdivr qword ptr [IID_IDirectInputDevice2A+0x530]
            mov ecx, dword ptr [esp + 0xc]
            fld dword ptr [edx + 0x18]
            fmul st(0), st(1)
            fstp dword ptr [ecx + 0x18]
            fld dword ptr [edx + 0x1c]
            fmul st(0), st(1)
            fstp dword ptr [ecx + 0x1c]
            fld dword ptr [edx + 0x20]
            fmul st(0), st(1)
            fstp dword ptr [ecx + 0x20]
            fstp st(0)
            jmp L00532fe1
    L00532fce:
            mov ecx, dword ptr [esp + 0xc]
            fstp st(0)
            mov dword ptr [ecx + 0x18], 0x3f800000
            mov dword ptr [ecx + 0x1c], esi
            mov dword ptr [ecx + 0x20], esi
    L00532fe1:
            fld dword ptr [ecx + 0x20]
            fld dword ptr [ecx + 0x1c]
            fld dword ptr [edx + 0x10]
            fmulp st(2), st(0)
            fmul dword ptr [edx + 0x14]
            fsubp st(1), st(0)
            fld dword ptr [ecx + 0x20]
            fxch st(1)
            _emit 0xd9
        _emit 0x19
            fld dword ptr [edx + 0x14]
            fld dword ptr [edx + 0xc]
            fxch st(1)
            fmul dword ptr [ecx + 0x18]
            fxch st(1)
            fmulp st(2), st(0)
            fld dword ptr [ecx + 0x1c]
            fxch st(2)
            fsubp st(1), st(0)
            fstp dword ptr [ecx + 4]
            fld dword ptr [edx + 0x10]
            fld dword ptr [edx + 0xc]
            fmulp st(2), st(0)
            fmul dword ptr [ecx + 0x18]
            fld dword ptr [ecx + 4]
            fxch st(1)
            fsubp st(2), st(0)
            _emit 0xd9
        _emit 0x01
            fxch st(2)
            fst dword ptr [esp + 4]
            fxch st(1)
            fst dword ptr [esp + 0xc]
            fxch st(1)
            fst dword ptr [esp + 0x10]
            fxch st(1)
            fmul dword ptr [esp + 0xc]
            fxch st(1)
            fmul dword ptr [esp + 0x10]
            fld st(2)
            fmul st(0), st(3)
            fxch st(1)
            faddp st(2), st(0)
            fld dword ptr [esp + 4]
            fxch st(1)
            faddp st(2), st(0)
            fstp dword ptr [ecx + 8]
            fsqrt
            fstp st(1)
            fcom dword ptr [IID_IDirectInputDevice2A+0x528]
            fnstsw ax
            test ah, 0x41
            jne L0053308f
            fdivr qword ptr [IID_IDirectInputDevice2A+0x530]
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
            jmp L0053309d
    L0053308f:
            fstp st(0)
            mov dword ptr [ecx], 0x3f800000
            mov dword ptr [ecx + 4], esi
            mov dword ptr [ecx + 8], esi
    L0053309d:
            fld dword ptr [ecx + 8]
            fld dword ptr [ecx + 0x1c]
            fld dword ptr [ecx + 4]
            fld dword ptr [ecx + 0x20]
            fld dword ptr [ecx + 0x20]
            fld dword ptr [ecx + 8]
            fxch st(4)
            fmulp st(5), st(0)
            fld dword ptr [ecx + 0x18]
            fxch st(2)
            fmulp st(3), st(0)
            fxch st(2)
            fsubp st(4), st(0)
            fxch st(1)
            _emit 0xd8
        _emit 0x09
            fxch st(3)
            fstp dword ptr [ecx + 0xc]
            fld dword ptr [ecx + 4]
            fxch st(1)
            fmulp st(2), st(0)
            fmul dword ptr [ecx + 0x18]
            fxch st(1)
            fsubp st(2), st(0)
            fld dword ptr [ecx + 0x1c]
            _emit 0xd8
        _emit 0x09
            fxch st(2)
            fstp dword ptr [ecx + 0x10]
            fxch st(1)
            fsubp st(1), st(0)
            fstp dword ptr [ecx + 0x14]
            mov eax, dword ptr [edx + 0x24]
            pop esi
            mov dword ptr [ecx + 0x24], eax
            mov eax, dword ptr [edx + 0x28]
            mov dword ptr [ecx + 0x28], eax
            mov edx, dword ptr [edx + 0x2c]
            mov dword ptr [ecx + 0x2c], edx
            pop ecx
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00533100
void C2_NAKED C2_HOOK_CDECL BrMatrix34RollingBall(br_matrix34* mat, int dx, int dy, int radius) {
#ifndef CARPOCALYPSE2_MATCHING
    br_scalar nx;
    br_scalar ny;
    br_scalar ca;
    br_scalar sa;
    br_scalar dr;
    br_scalar h;

    // The rolling ball, Graphics Gems III (1993), pages 51-60, Academic Press

    dr = (br_scalar)sqrt((float)(dx * dx + dy * dy));
    if (dr == BR_SCALAR(.0f)) {
        BrMatrix34Identity(mat);
        return;
    }
    h = (br_scalar)sqrt(dr * dr + radius * radius);
    ca = radius / h;
    sa = dr / h;
    nx = -dy / dr;
    ny = dx / dr;
    // nz = 0;

    h = (1 - ca);

    M(0, 0) = nx * nx * h + ca;
    M(0, 1) = nx * ny * h;
    M(0, 2) = ny * sa;
    M(1, 1) = ca + ny * ny * h;
    M(1, 2) = -nx * sa;
    M(2, 2) = ca;

    M(1, 0) = M(0, 1);
    M(2, 0) = -M(0, 2);
    M(2, 1) = -M(1, 2);

    M(3, 0) = BR_SCALAR(0.f);
    M(3, 1) = BR_SCALAR(0.f);
    M(3, 2) = BR_SCALAR(0.f);
#else
    __asm {
            push ecx
            fild dword ptr [esp + 0x10]
            fild dword ptr [esp + 0xc]
            _emit 0xd9
        _emit 0x54
        _emit 0x24
        _emit 0x00
            fxch st(1)
            fst dword ptr [esp + 0x10]
            fxch st(1)
            fst dword ptr [esp + 0xc]
            fld st(1)
            fxch st(1)
            fmul dword ptr [esp + 0xc]
            fxch st(1)
            fmul st(0), st(2)
            faddp st(1), st(0)
            fsqrt
            fstp st(1)
            fst dword ptr [esp + 0xc]
            fcomp dword ptr [IID_IDirectInputDevice2A+0x524]
            fnstsw ax
            test ah, 0x40
            je L0053316c
            mov eax, dword ptr [esp + 8]
            mov edx, 0x3f800000
            xor ecx, ecx
            mov dword ptr [eax], edx
            mov dword ptr [eax + 4], ecx
            mov dword ptr [eax + 8], ecx
            mov dword ptr [eax + 0xc], ecx
            mov dword ptr [eax + 0x10], edx
            mov dword ptr [eax + 0x14], ecx
            mov dword ptr [eax + 0x18], ecx
            mov dword ptr [eax + 0x1c], ecx
            mov dword ptr [eax + 0x20], edx
            mov dword ptr [eax + 0x24], ecx
            mov dword ptr [eax + 0x28], ecx
            mov dword ptr [eax + 0x2c], ecx
            pop ecx
            ret
    L0053316c:
            fild dword ptr [esp + 0x14]
            fld dword ptr [esp + 0xc]
            fmul dword ptr [esp + 0xc]
            fxch st(1)
            fst dword ptr [esp + 0x14]
            fxch st(1)
            fld st(1)
            fmul st(0), st(2)
            fld dword ptr [esp + 0x10]
            fdiv dword ptr [esp + 0xc]
            fxch st(2)
            mov eax, dword ptr [esp + 8]
            faddp st(1), st(0)
            _emit 0xd9
        _emit 0x44
        _emit 0x24
        _emit 0x00
            fld dword ptr [esp + 0x14]
            fxch st(2)
            fsqrt
            fst dword ptr [esp + 0x10]
            fxch st(1)
            fdiv dword ptr [esp + 0xc]
            fld dword ptr [esp + 0xc]
            fxch st(2)
            fstp dword ptr [esp + 0xc]
            fxch st(2)
            fdiv dword ptr [esp + 0xc]
            fxch st(1)
            fdiv dword ptr [esp + 0xc]
            fld dword ptr [IID_IDirectInputDevice2A+0x520]
            fxch st(4)
            fchs
            fst dword ptr [esp + 0xc]
            fxch st(3)
            fst dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x10]
            fxch st(3)
            fstp st(6)
            fxch st(4)
            fsub st(0), st(5)
            fxch st(1)
            fst dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x14]
            fxch st(2)
            fstp dword ptr [esp + 0x14]
            fxch st(3)
            fmul dword ptr [esp + 0xc]
            fxch st(4)
            fmul dword ptr [esp + 0xc]
            fxch st(2)
            fmul dword ptr [esp + 0x10]
            fxch st(3)
            fmul dword ptr [esp + 0xc]
            fxch st(1)
            fmul dword ptr [esp + 0x10]
            fxch st(4)
            fmul dword ptr [esp + 0x14]
            fxch st(2)
            fmul dword ptr [esp + 0x14]
            fxch st(3)
            fmul dword ptr [esp + 0x14]
            fxch st(1)
            fchs
            fxch st(4)
            fst dword ptr [esp + 0xc]
            fld dword ptr [esp + 0xc]
            fxch st(3)
            fadd st(0), st(6)
            _emit 0xd9
        _emit 0x18
            fxch st(4)
            fst dword ptr [esp + 8]
            fld dword ptr [esp + 8]
            fxch st(4)
            fstp dword ptr [esp + 8]
            fld dword ptr [esp + 8]
            fxch st(3)
            fstp dword ptr [eax + 0x18]
            fxch st(1)
            fadd st(0), st(5)
            fxch st(4)
            fchs
            fxch st(1)
            fchs
            fxch st(2)
            fstp dword ptr [eax + 4]
            fxch st(3)
            fstp dword ptr [eax + 0x10]
            mov ecx, dword ptr [esp + 8]
            fxch st(1)
            fstp dword ptr [eax + 0x1c]
            fxch st(1)
            fstp dword ptr [eax + 8]
            mov dword ptr [eax + 0xc], ecx
            xor ecx, ecx
            fstp dword ptr [eax + 0x14]
            fstp dword ptr [eax + 0x20]
            mov dword ptr [eax + 0x24], ecx
            mov dword ptr [eax + 0x28], ecx
            mov dword ptr [eax + 0x2c], ecx
            pop ecx
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00533290
C2_NAKED br_matrix34* C2_HOOK_CDECL BrBoundsToMatrix34(br_matrix34* mat, const br_bounds* bounds) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;
    br_vector3 tr;
    br_vector3 sc;

    for (i = 0; i < 3; ++i) {
        tr.v[i] = 0.5f * bounds->min.v[i] + 0.5f * bounds->max.v[i];
        if (bounds->min.v[i] == bounds->max.v[i]) {
            sc.v[i] = 1.f;
        } else {
            sc.v[i] = 0.5f * bounds->max.v[i] - 0.5f * bounds->min.v[i];
        }
    }

    M(0, 0) = sc.v[0];
    M(0, 1) = 0.f;
    M(0, 2) = 0.f;
    M(1, 0) = 0.f;
    M(1, 1) = sc.v[1];
    M(1, 2) = 0.f;
    M(2, 0) = 0.f;
    M(2, 1) = 0.f;
    M(2, 2) = sc.v[2];
    M(3, 0) = tr.v[0];
    M(3, 1) = tr.v[1];
    M(3, 2) = tr.v[2];
    return mat;
#else
    __asm {
            mov ecx, dword ptr [esp + 8]
            sub esp, 0x18
            lea edx, [esp + 0xc]
            mov eax, ecx
            push esi
            sub edx, ecx
            mov esi, 3
    L005332a5:
            _emit 0xd9
        _emit 0x00
            fld dword ptr [eax + 0xc]
            fmul dword ptr [IID_IDirectInputDevice2A+0x538]
            fxch st(1)
            fmul dword ptr [IID_IDirectInputDevice2A+0x538]
            add eax, 4
            dec esi
            faddp st(1), st(0)
            fstp dword ptr [edx + eax - 4]
            jne L005332a5
            lea edx, [esp + 4]
            mov esi, 3
            sub edx, ecx
    L005332cf:
            fld dword ptr [ecx + 0xc]
            _emit 0xd8
        _emit 0x19
            fnstsw ax
            test ah, 0x40
            jne L005332f5
            _emit 0xd9
        _emit 0x01
            fld dword ptr [ecx + 0xc]
            fmul dword ptr [IID_IDirectInputDevice2A+0x538]
            fxch st(1)
            fmul dword ptr [IID_IDirectInputDevice2A+0x538]
            fsubp st(1), st(0)
            fstp dword ptr [edx + ecx]
            jmp L005332fc
    L005332f5:
            mov dword ptr [edx + ecx], 0x3f800000
    L005332fc:
            add ecx, 4
            dec esi
            jne L005332cf
            mov eax, dword ptr [esp + 0x20]
            mov edx, dword ptr [esp + 8]
            fld dword ptr [esp + 4]
            xor ecx, ecx
            mov dword ptr [eax + 0x10], edx
            mov edx, dword ptr [esp + 0x10]
            mov dword ptr [eax + 4], ecx
            _emit 0xd9
        _emit 0x18
            mov dword ptr [eax + 8], ecx
            mov dword ptr [eax + 0xc], ecx
            mov dword ptr [eax + 0x14], ecx
            mov dword ptr [eax + 0x18], ecx
            mov dword ptr [eax + 0x1c], ecx
            mov ecx, dword ptr [esp + 0xc]
            mov dword ptr [eax + 0x20], ecx
            mov ecx, dword ptr [esp + 0x14]
            mov dword ptr [eax + 0x24], edx
            mov edx, dword ptr [esp + 0x18]
            mov dword ptr [eax + 0x28], ecx
            mov dword ptr [eax + 0x2c], edx
            pop esi
            add esp, 0x18
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00533350
void C2_HOOK_CDECL BrMatrix34Copy4(br_matrix34* A, const br_matrix4* B) {

    A(0, 0) = B(0, 0);
    A(0, 1) = B(0, 1);
    A(0, 2) = B(0, 2);

    A(1, 0) = B(1, 0);
    A(1, 1) = B(1, 1);
    A(1, 2) = B(1, 2);

    A(2, 0) = B(2, 0);
    A(2, 1) = B(2, 1);
    A(2, 2) = B(2, 2);

    A(3, 0) = B(3, 0);
    A(3, 1) = B(3, 1);
    A(3, 2) = B(3, 2);
}

// FUNCTION: CARMA2_HW 0x005333a0
void C2_HOOK_STDCALL BrMatrix34TApplyFV(br_vector3* A, const br_fvector3* B, const br_matrix34* C) {

    A->v[0] = BR_MAC3(B->v[0], C(0, 0), B->v[1], C(0, 1), B->v[2], C(0, 2));
    A->v[1] = BR_MAC3(B->v[0], C(1, 0), B->v[1], C(1, 1), B->v[2], C(1, 2));
    A->v[2] = BR_MAC3(B->v[0], C(2, 0), B->v[1], C(2, 1), B->v[2], C(2, 2));
}

// FUNCTION: CARMA2_HW 0x00533410
void C2_HOOK_CDECL BrMatrix34Apply(br_vector3* A, const br_vector4* B, const br_matrix34* C) {

    A->v[0] = BR_MAC4(B->v[0], C(0, 0), B->v[1], C(1, 0), B->v[2], C(2, 0), B->v[3], C(3, 0));
    A->v[1] = BR_MAC4(B->v[0], C(0, 1), B->v[1], C(1, 1), B->v[2], C(2, 1), B->v[3], C(3, 1));
    A->v[2] = BR_MAC4(B->v[0], C(0, 2), B->v[1], C(1, 2), B->v[2], C(2, 2), B->v[3], C(3, 2));
}

// FUNCTION: CARMA2_HW 0x005334a0
void C2_NAKED C2_HOOK_CDECL BrMatrix34ApplyP(br_vector3* A, const br_vector3* B, const br_matrix34* C) {
#ifndef CARPOCALYPSE2_MATCHING

    A->v[0] = BR_MAC3(B->v[0], C(0, 0), B->v[1], C(1, 0), B->v[2], C(2, 0)) + C(3, 0);
    A->v[1] = BR_MAC3(B->v[0], C(0, 1), B->v[1], C(1, 1), B->v[2], C(2, 1)) + C(3, 1);
    A->v[2] = BR_MAC3(B->v[0], C(0, 2), B->v[1], C(1, 2), B->v[2], C(2, 2)) + C(3, 2);
#else
    __asm {
            mov eax, dword ptr [esp + 0xc]
            mov ecx, dword ptr [esp + 8]
            mov edx, dword ptr [esp + 4]
            fld dword ptr [eax + 0xc]
            fld dword ptr [eax + 0x18]
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
            fadd dword ptr [eax + 0x24]
            _emit 0xd9
        _emit 0x1a
            fld dword ptr [eax + 0x1c]
            fld dword ptr [ecx + 8]
            fld dword ptr [eax + 0x10]
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
            fadd dword ptr [eax + 0x28]
            fstp dword ptr [edx + 4]
            fld dword ptr [eax + 0x20]
            fld dword ptr [ecx + 8]
            fld dword ptr [eax + 0x14]
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
            fadd dword ptr [eax + 0x2c]
            fstp dword ptr [edx + 8]
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00533520
void C2_NAKED C2_HOOK_CDECL BrMatrix34ApplyV(br_vector3* A, const br_vector3* B, const br_matrix34* C) {
#ifndef CARPOCALYPSE2_MATCHING

    A->v[0] = BR_MAC3(B->v[0], C(0, 0), B->v[1], C(1, 0), B->v[2], C(2, 0));
    A->v[1] = BR_MAC3(B->v[0], C(0, 1), B->v[1], C(1, 1), B->v[2], C(2, 1));
    A->v[2] = BR_MAC3(B->v[0], C(0, 2), B->v[1], C(1, 2), B->v[2], C(2, 2));
#else
    __asm {
            mov eax, dword ptr [esp + 0xc]
            mov ecx, dword ptr [esp + 8]
            mov edx, dword ptr [esp + 4]
            fld dword ptr [eax + 0x18]
            fld dword ptr [eax + 0xc]
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
            _emit 0xd9
        _emit 0x1a
            fld dword ptr [eax + 0x1c]
            fld dword ptr [ecx + 8]
            fld dword ptr [eax + 0x10]
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
            fstp dword ptr [edx + 4]
            fld dword ptr [eax + 0x14]
            fld dword ptr [ecx + 4]
            fld dword ptr [eax + 0x20]
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
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00533590
void C2_NAKED C2_HOOK_CDECL BrMatrix34TApply(br_vector4* A, const br_vector4* B, const br_matrix34* C) {
#ifndef CARPOCALYPSE2_MATCHING

    A->v[0] = BR_MAC3(B->v[0], C(0, 0), B->v[1], C(0, 1), B->v[2], C(0, 2));
    A->v[1] = BR_MAC3(B->v[0], C(1, 0), B->v[1], C(1, 1), B->v[2], C(1, 2));
    A->v[2] = BR_MAC3(B->v[0], C(2, 0), B->v[1], C(2, 1), B->v[2], C(2, 2));
    A->v[3] = BR_MAC3(B->v[0], C(3, 0), B->v[1], C(3, 1), B->v[2], C(3, 2)) + B->v[3];
#else
    __asm {
            mov ecx, dword ptr [esp + 0xc]
            mov eax, dword ptr [esp + 8]
            mov edx, dword ptr [esp + 4]
            fld dword ptr [ecx + 8]
            fld dword ptr [ecx + 4]
            fld dword ptr [eax + 8]
            fld dword ptr [eax + 4]
            _emit 0xd9
        _emit 0x01
            fxch st(2)
            fmulp st(4), st(0)
            fmulp st(2), st(0)
            _emit 0xd8
        _emit 0x08
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            _emit 0xd9
        _emit 0x1a
            fld dword ptr [ecx + 0x14]
            fld dword ptr [eax + 8]
            fld dword ptr [ecx + 0x10]
            fld dword ptr [ecx + 0xc]
            fxch st(2)
            fmulp st(3), st(0)
            fmul dword ptr [eax + 4]
            fxch st(1)
            _emit 0xd8
        _emit 0x08
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fstp dword ptr [edx + 4]
            fld dword ptr [ecx + 0x1c]
            fld dword ptr [eax + 4]
            fld dword ptr [ecx + 0x20]
            fld dword ptr [ecx + 0x18]
            fxch st(2)
            fmulp st(3), st(0)
            fmul dword ptr [eax + 8]
            fxch st(1)
            _emit 0xd8
        _emit 0x08
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fstp dword ptr [edx + 8]
            fld dword ptr [ecx + 0x28]
            fld dword ptr [eax + 4]
            fld dword ptr [ecx + 0x2c]
            fld dword ptr [ecx + 0x24]
            fxch st(2)
            fmulp st(3), st(0)
            fmul dword ptr [eax + 8]
            fxch st(1)
            _emit 0xd8
        _emit 0x08
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fadd dword ptr [eax + 0xc]
            fstp dword ptr [edx + 0xc]
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00533620
void C2_NAKED C2_HOOK_CDECL BrMatrix34TApplyP(br_vector3* A, const br_vector3* B, const br_matrix34* C) {
#ifndef CARPOCALYPSE2_MATCHING

    // translation elements are presumed zero or irrelevant
    A->v[0] = BR_MAC3(B->v[0], C(0, 0), B->v[1], C(0, 1), B->v[2], C(0, 2));
    A->v[1] = BR_MAC3(B->v[0], C(1, 0), B->v[1], C(1, 1), B->v[2], C(1, 2));
    A->v[2] = BR_MAC3(B->v[0], C(2, 0), B->v[1], C(2, 1), B->v[2], C(2, 2));
#else
    __asm {
            mov eax, dword ptr [esp + 0xc]
            mov ecx, dword ptr [esp + 8]
            mov edx, dword ptr [esp + 4]
            fld dword ptr [eax + 4]
            fld dword ptr [eax + 8]
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
            fld dword ptr [eax + 0x10]
            fld dword ptr [ecx + 4]
            fld dword ptr [eax + 0x14]
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
            fstp dword ptr [edx + 4]
            fld dword ptr [eax + 0x1c]
            fld dword ptr [ecx + 4]
            fld dword ptr [eax + 0x20]
            fld dword ptr [eax + 0x18]
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
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00533690
void C2_NAKED C2_HOOK_CDECL BrMatrix34TApplyV(br_vector3* A, const br_vector3* B, const br_matrix34* C) {
#ifndef CARPOCALYPSE2_MATCHING

    // translation elements are presumed zero or irrelevant
    A->v[0] = BR_MAC3(B->v[0], C(0, 0), B->v[1], C(0, 1), B->v[2], C(0, 2));
    A->v[1] = BR_MAC3(B->v[0], C(1, 0), B->v[1], C(1, 1), B->v[2], C(1, 2));
    A->v[2] = BR_MAC3(B->v[0], C(2, 0), B->v[1], C(2, 1), B->v[2], C(2, 2));
#else
    __asm {
            mov eax, dword ptr [esp + 0xc]
            mov ecx, dword ptr [esp + 8]
            mov edx, dword ptr [esp + 4]
            fld dword ptr [eax + 8]
            fld dword ptr [eax + 4]
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
            _emit 0xd9
        _emit 0x1a
            fld dword ptr [eax + 0x10]
            fld dword ptr [ecx + 4]
            fld dword ptr [eax + 0x14]
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
            fstp dword ptr [edx + 4]
            fld dword ptr [eax + 0x1c]
            fld dword ptr [ecx + 4]
            fld dword ptr [eax + 0x20]
            fld dword ptr [eax + 0x18]
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
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00533700
void C2_HOOK_CDECL BrMatrix34Pre(br_matrix34* mat, const br_matrix34* A) {

    BrMatrix34Mul(&mattmp1, A, mat);
    BrMatrix34Copy(mat, &mattmp1);
}

// FUNCTION: CARMA2_HW 0x00533730
void C2_HOOK_CDECL BrMatrix34Post(br_matrix34* mat, const br_matrix34* A) {

    BrMatrix34Mul(&mattmp1, mat, A);
    BrMatrix34Copy(mat, &mattmp1);
}

// FUNCTION: CARMA2_HW 0x00533760
void C2_HOOK_CDECL BrMatrix34PreRotateX(br_matrix34* mat, br_angle rx) {

    BrMatrix34RotateX(&mattmp2, rx);
    BrMatrix34Mul(&mattmp1, &mattmp2, mat);
    BrMatrix34Copy(mat, &mattmp1);
}

// FUNCTION: CARMA2_HW 0x00533860
void C2_HOOK_CDECL BrMatrix34PostRotateX(br_matrix34* mat, br_angle rx) {

    BrMatrix34RotateX(&mattmp2, rx);
    BrMatrix34Mul(&mattmp1, mat, &mattmp2);
    BrMatrix34Copy(mat, &mattmp1);
}

// FUNCTION: CARMA2_HW 0x00533960
void C2_HOOK_CDECL BrMatrix34PreRotateY(br_matrix34* mat, br_angle ry) {

    BrMatrix34RotateY(&mattmp2, ry);
    BrMatrix34Mul(&mattmp1, &mattmp2, mat);
    BrMatrix34Copy(mat, &mattmp1);
}

// FUNCTION: CARMA2_HW 0x00533a60
void C2_HOOK_CDECL BrMatrix34PostRotateY(br_matrix34* mat, br_angle ry) {

    BrMatrix34RotateY(&mattmp2, ry);
    BrMatrix34Mul(&mattmp1, mat, &mattmp2);
    BrMatrix34Copy(mat, &mattmp1);
}

// FUNCTION: CARMA2_HW 0x00533b60
void C2_HOOK_CDECL BrMatrix34PreRotateZ(br_matrix34* mat, br_angle rz) {

    BrMatrix34RotateZ(&mattmp2, rz);
    BrMatrix34Mul(&mattmp1, &mattmp2, mat);
    BrMatrix34Copy(mat, &mattmp1);
}

// FUNCTION: CARMA2_HW 0x00533c50
void C2_HOOK_CDECL BrMatrix34PostRotateZ(br_matrix34* mat, br_angle rz) {

    BrMatrix34RotateZ(&mattmp2, rz);
    BrMatrix34Mul(&mattmp1, mat, &mattmp2);
    BrMatrix34Copy(mat, &mattmp1);
}

// FUNCTION: CARMA2_HW 0x00533d40
void C2_HOOK_CDECL BrMatrix34PreRotate(br_matrix34* mat, br_angle r, const br_vector3* axis) {

    BrMatrix34Rotate(&mattmp2, r, axis);
    BrMatrix34Mul(&mattmp1, &mattmp2, mat);
    BrMatrix34Copy(mat, &mattmp1);
}

// FUNCTION: CARMA2_HW 0x00533d80
void C2_HOOK_CDECL BrMatrix34PostRotate(br_matrix34* mat, br_angle r, const br_vector3* axis) {

    BrMatrix34Rotate(&mattmp2, r, axis);
    BrMatrix34Mul(&mattmp1, mat, &mattmp2);
    BrMatrix34Copy(mat, &mattmp1);
}

// FUNCTION: CARMA2_HW 0x00533dc0
void C2_HOOK_CDECL BrMatrix34PreTranslate(br_matrix34* mat, br_scalar x, br_scalar y, br_scalar z) {

    BrMatrix34Translate(&mattmp2, x, y, z);
    BrMatrix34Mul(&mattmp1, &mattmp2, mat);
    BrMatrix34Copy(mat, &mattmp1);
}

// FUNCTION: CARMA2_HW 0x00533e60
void C2_HOOK_CDECL BrMatrix34PostTranslate(br_matrix34* mat, br_scalar x, br_scalar y, br_scalar z) {

    M(3, 0) += x;
    M(3, 1) += y;
    M(3, 2) += z;
}

// FUNCTION: CARMA2_HW 0x00533e90
void C2_HOOK_CDECL BrMatrix34PreScale(br_matrix34* mat, br_scalar sx, br_scalar sy, br_scalar sz) {

    BrMatrix34Scale(&mattmp2, sx, sy, sz);
    BrMatrix34Mul(&mattmp1, &mattmp2, mat);
    BrMatrix34Copy(mat, &mattmp1);
}

// FUNCTION: CARMA2_HW 0x00533f30
void C2_HOOK_CDECL BrMatrix34PostScale(br_matrix34* mat, br_scalar sx, br_scalar sy, br_scalar sz) {

    BrMatrix34Scale(&mattmp2, sx, sy, sz);
    BrMatrix34Mul(&mattmp1, mat, &mattmp2);
    BrMatrix34Copy(mat, &mattmp1);
}

// FUNCTION: CARMA2_HW 0x00533fd0
void C2_HOOK_CDECL BrMatrix34PreShearX(br_matrix34* mat, br_scalar sy, br_scalar sz) {

    BrMatrix34ShearX(&mattmp2, sy, sz);
    BrMatrix34Mul(&mattmp1, &mattmp2, mat);
    BrMatrix34Copy(mat, &mattmp1);
}

// FUNCTION: CARMA2_HW 0x00534070
void C2_HOOK_CDECL BrMatrix34PostShearX(br_matrix34* mat, br_scalar sy, br_scalar sz) {

    BrMatrix34ShearX(&mattmp2, sy, sz);
    BrMatrix34Mul(&mattmp1, mat, &mattmp2);
    BrMatrix34Copy(mat, &mattmp1);
}

// FUNCTION: CARMA2_HW 0x00534110
void C2_HOOK_CDECL BrMatrix34PreShearY(br_matrix34* mat, br_scalar sx, br_scalar sz) {

    BrMatrix34ShearY(&mattmp2, sx, sz);
    BrMatrix34Mul(&mattmp1, &mattmp2, mat);
    BrMatrix34Copy(mat, &mattmp1);
}

// FUNCTION: CARMA2_HW 0x005341b0
void C2_HOOK_CDECL BrMatrix34PostShearY(br_matrix34* mat, br_scalar sx, br_scalar sz) {

    BrMatrix34ShearY(&mattmp2, sx, sz);
    BrMatrix34Mul(&mattmp1, mat, &mattmp2);
    BrMatrix34Copy(mat, &mattmp1);
}

// FUNCTION: CARMA2_HW 0x00534250
void C2_HOOK_CDECL BrMatrix34PreShearZ(br_matrix34* mat, br_scalar sx, br_scalar sy) {

    BrMatrix34ShearZ(&mattmp2, sx, sy);
    BrMatrix34Mul(&mattmp1, &mattmp2, mat);
    BrMatrix34Copy(mat, &mattmp1);
}

// FUNCTION: CARMA2_HW 0x005342f0
void C2_HOOK_CDECL BrMatrix34PostShearZ(br_matrix34* mat, br_scalar sx, br_scalar sy) {

    BrMatrix34ShearZ(&mattmp2, sx, sy);
    BrMatrix34Mul(&mattmp1, mat, &mattmp2);
    BrMatrix34Copy(mat, &mattmp1);
}
