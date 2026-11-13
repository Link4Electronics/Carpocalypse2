#include "quat.h"

#include "matrix34.h"
#include "matrix4.h"

#include "c2_math.h"

#define M(x, y) mat->m[x][y]
#define M_DIAG(x) (((br_scalar*)mat->m)[4*(x)])
#define Q_EL(e) (((br_scalar*)&(q->x))[(e)])

#ifdef CARPOCALYPSE2_MATCHING
extern char IID_IDirectInputDevice2A[];
extern char _CIacos[];
extern br_uint_8 _CombineTransforms[7][7];
extern void __cdecl _ftol();
#endif
// FUNCTION: CARMA2_HW 0x005355e0
C2_NAKED br_quat* C2_HOOK_CDECL BrQuatMul(br_quat* q, const br_quat* l, const br_quat* r) {
#ifndef CARPOCALYPSE2_MATCHING
    br_scalar x1;
    br_scalar x2;
    br_scalar x3;
    br_scalar x4;
    br_scalar s;

    x1 = (l->x + l->z) * (r->y + r->x);
    x2 = (l->w + l->y) * (r->w - r->z);
    x3 = (l->w - l->y) * (r->w + r->z);
    s = x1 + x2 + x3;
    x4 = ((l->z - l->x) * (r->x - r->y) + s) * 0.5f;
    q->x = x4 + (l->w + l->x) * (r->w + r->x) - s;
    q->y = x4 + (l->w - l->x) * (r->z + r->y) - x3;
    q->z = x4 + (l->y + l->z) * (r->w - r->x) - x2;
    q->w = x4 + (l->z - l->y) * (r->y - r->z) - x1;
    return q;
#else
    __asm {
            sub esp, 0x14
            mov eax, dword ptr [esp + 0x1c]
            mov ecx, dword ptr [esp + 0x20]
            fld dword ptr [eax + 0xc]
            fld dword ptr [ecx + 0xc]
            fld dword ptr [eax + 0xc]
            fld dword ptr [ecx + 0xc]
            fld dword ptr [eax + 0xc]
            fxch st(4)
            _emit 0xd8
        _emit 0x00
            fld dword ptr [ecx + 0xc]
            fxch st(4)
            _emit 0xd8
        _emit 0x01
            fmulp st(1), st(0)
            fstp dword ptr [esp + 8]
            fld dword ptr [eax + 0xc]
            fxch st(2)
            _emit 0xd8
        _emit 0x20
            fld dword ptr [ecx + 8]
            fadd dword ptr [ecx + 4]
            fmulp st(1), st(0)
            fstp dword ptr [esp + 0xc]
            fld dword ptr [ecx + 4]
            fxch st(1)
            _emit 0xd8
        _emit 0x21
            fld dword ptr [eax + 4]
            fadd dword ptr [eax + 8]
            fmulp st(1), st(0)
            fstp dword ptr [esp + 0x10]
            fld dword ptr [eax + 8]
            fxch st(1)
            _emit 0xd8
        _emit 0x01
            fld dword ptr [eax + 4]
            fxch st(2)
            _emit 0xd8
        _emit 0x00
            fmulp st(1), st(0)
            _emit 0xd9
        _emit 0x5c
        _emit 0x24
        _emit 0x00
            fld dword ptr [ecx + 8]
            fxch st(4)
            faddp st(1), st(0)
            fld dword ptr [ecx + 0xc]
            fxch st(4)
            fsubp st(3), st(0)
            fxch st(2)
            fmulp st(2), st(0)
            fsub dword ptr [eax + 4]
            fxch st(1)
            fxch st(2)
            fxch st(1)
            fxch st(2)
            fstp dword ptr [esp + 4]
            fadd dword ptr [ecx + 8]
            fmulp st(1), st(0)
            fstp dword ptr [esp + 0x20]
            fld dword ptr [ecx + 4]
            fld dword ptr [eax + 8]
            fld dword ptr [esp + 0x20]
            fadd dword ptr [esp + 4]
            fxch st(2)
            _emit 0xd8
        _emit 0x29
            fxch st(1)
            _emit 0xd8
        _emit 0x20
            fxch st(2)
            _emit 0xd8
        _emit 0x44
        _emit 0x24
        _emit 0x00
            fxch st(2)
            fmulp st(1), st(0)
            fld dword ptr [eax + 8]
            fld dword ptr [eax + 4]
            fld dword ptr [ecx + 4]
            fxch st(4)
            fstp dword ptr [esp + 0x1c]
            fld dword ptr [ecx + 8]
            fxch st(3)
            fadd dword ptr [esp + 0x1c]
            fmul dword ptr [IID_IDirectInputDevice2A+0x580]
            fld st(0)
            fxch st(2)
            fsubp st(3), st(0)
            fxch st(3)
            fsubp st(4), st(0)
            fxch st(3)
            fmulp st(1), st(0)
            fxch st(2)
            fadd dword ptr [esp + 8]
            fld st(1)
            fadd dword ptr [esp + 0xc]
            fxch st(3)
            fadd st(0), st(2)
            fxch st(1)
            fsub dword ptr [esp + 0x1c]
            fxch st(3)
            fsub dword ptr [esp + 0x20]
            fxch st(1)
            _emit 0xd8
        _emit 0x64
        _emit 0x24
        _emit 0x00
            fxch st(2)
            fadd dword ptr [esp + 0x10]
            fxch st(3)
            mov eax, dword ptr [esp + 0x18]
            _emit 0xd9
        _emit 0x18
            fxch st(1)
            fstp dword ptr [eax + 0xc]
            fxch st(1)
            fsub dword ptr [esp + 4]
            fxch st(1)
            fstp dword ptr [eax + 4]
            fstp dword ptr [eax + 8]
            add esp, 0x14
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00535710
C2_NAKED br_quat* C2_HOOK_CDECL BrQuatNormalise(br_quat* q, const br_quat* qq) {
#ifndef CARPOCALYPSE2_MATCHING
    br_scalar s;

    s = 1.0f / (float)sqrt(qq->x * qq->x + qq->y * qq->y + qq->z * qq->z + qq->w * qq->w);
    q->x = qq->x * s;
    q->y = qq->y * s;
    q->z = qq->z * s;
    q->w = qq->w * s;
    return q;
#else
    __asm {
            mov ecx, dword ptr [esp + 8]
            mov eax, dword ptr [esp + 4]
            fld dword ptr [ecx + 4]
            fld dword ptr [ecx + 8]
            fld dword ptr [ecx + 0xc]
            fxch st(2)
            fst dword ptr [esp + 8]
            fld st(1)
            fxch st(1)
            fmul dword ptr [esp + 8]
            _emit 0xd9
        _emit 0x01
            fxch st(2)
            fmul st(0), st(3)
            fxch st(4)
            fst dword ptr [esp + 8]
            fmul dword ptr [esp + 8]
            fxch st(2)
            fst dword ptr [esp + 8]
            fxch st(4)
            faddp st(1), st(0)
            fxch st(3)
            fmul dword ptr [esp + 8]
            fxch st(1)
            faddp st(3), st(0)
            faddp st(2), st(0)
            fxch st(1)
            fsqrt
            fstp st(1)
            fdivr dword ptr [IID_IDirectInputDevice2A+0x584]
            fld st(0)
            _emit 0xd8
        _emit 0x09
            _emit 0xd9
        _emit 0x18
            fld dword ptr [ecx + 4]
            fmul st(0), st(1)
            fstp dword ptr [eax + 4]
            fld dword ptr [ecx + 8]
            fmul st(0), st(1)
            fstp dword ptr [eax + 8]
            fld dword ptr [ecx + 0xc]
            fmul st(0), st(1)
            fstp dword ptr [eax + 0xc]
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
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00535790
C2_NAKED br_quat* C2_HOOK_CDECL BrQuatInvert(br_quat* q, const br_quat* qq) {
#ifndef CARPOCALYPSE2_MATCHING

    q->x = -qq->x;
    q->y = -qq->y;
    q->z = -qq->z;
    q->w = qq->w;
    return q;
#else
    __asm {
            mov ecx, dword ptr [esp + 8]
            mov eax, dword ptr [esp + 4]
            _emit 0xd9
        _emit 0x01
            fchs
            _emit 0xd9
        _emit 0x18
            fld dword ptr [ecx + 4]
            fchs
            fstp dword ptr [eax + 4]
            fld dword ptr [ecx + 8]
            fchs
            fstp dword ptr [eax + 8]
            fld dword ptr [ecx + 0xc]
            fstp dword ptr [eax + 0xc]
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x005357c0
C2_NAKED br_quat* C2_HOOK_CDECL BrQuatSlerp(br_quat* q, const br_quat* l, const br_quat* r, br_scalar a, br_int_16 spins) {
#ifndef CARPOCALYPSE2_MATCHING
    int omega;
    int omega_spin;
    br_scalar s_omega;
    br_scalar c_omega;
    br_scalar scale_l;
    br_scalar scale_r;
    br_quat t;

    // Animating rotation with quaternion curves, Ken Shoemake,SIGGRAPH Computer Graphics, 19(3):245–254, 1985. doi:10.1145/325165.325242.
    c_omega = l->x * r->x + l->y * r->y + l->z * r->z + l->w * r->w;
    if (c_omega >= 0.f) {
        t.x = r->x;
        t.y = r->y;
        t.z = r->z;
        t.w = r->w;
    } else {
        t.x = -r->x;
        t.y = -r->y;
        t.z = -r->z;
        t.w = -r->w;
        c_omega = -c_omega;
    }

    if ((1.f - c_omega) <= BR_SCALAR_EPSILON) {
        scale_l = 1.f - a;
        scale_r = a;
    } else {
        omega = BrRadianToAngle(acosf(c_omega));
        s_omega = sinf(BrAngleToRadian(omega));
        omega_spin = omega + spins * BR_ANGLE_RAD(PI);
        scale_l = sinf(BrAngleToRadian(omega - a * omega_spin)) / s_omega;
        scale_r = sinf(BrAngleToRadian(a * omega_spin)) / s_omega;
    }
    q->x = scale_l * l->x + scale_r * t.x;
    q->y = scale_l * l->y + scale_r * t.y;
    q->z = scale_l * l->z + scale_r * t.z;
    q->w = scale_l * l->w + scale_r * t.w;
    return q;
#else
    __asm {
            sub esp, 0x10
            mov ecx, dword ptr [esp + 0x1c]
            push esi
            mov esi, dword ptr [esp + 0x1c]
            fld dword ptr [ecx + 8]
            fld dword ptr [esi + 8]
            fld dword ptr [ecx + 0xc]
            fld dword ptr [esi + 0xc]
            fld dword ptr [ecx + 4]
            fxch st(3)
            fmulp st(4), st(0)
            fmulp st(1), st(0)
            faddp st(2), st(0)
            _emit 0xd9
        _emit 0x01
            fxch st(1)
            fmul dword ptr [esi + 4]
            fxch st(1)
            _emit 0xd8
        _emit 0x0e
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fst dword ptr [esp + 0x1c]
            fcomp dword ptr [IID_IDirectInputDevice2A+0x588]
            fnstsw ax
            test ah, 1
            je L00535834
            _emit 0xd9
        _emit 0x01
            fchs
            fstp dword ptr [esp + 4]
            fld dword ptr [ecx + 4]
            fchs
            fstp dword ptr [esp + 8]
            fld dword ptr [ecx + 8]
            fchs
            fstp dword ptr [esp + 0xc]
            fld dword ptr [ecx + 0xc]
            fchs
            fstp dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x1c]
            fchs
            fstp dword ptr [esp + 0x1c]
            jmp L0053584f
    L00535834:
            fld dword ptr [ecx + 4]
            fstp dword ptr [esp + 8]
            fld dword ptr [ecx + 8]
            mov eax, dword ptr [ecx]
            fstp dword ptr [esp + 0xc]
            fld dword ptr [ecx + 0xc]
            fstp dword ptr [esp + 0x10]
            mov dword ptr [esp + 4], eax
    L0053584f:
            fld dword ptr [IID_IDirectInputDevice2A+0x584]
            fsub dword ptr [esp + 0x1c]
            fcomp dword ptr [IID_IDirectInputDevice2A+0x58c]
            fnstsw ax
            test ah, 0x41
            jne L0053591c
            fld dword ptr [esp + 0x1c]
            call _CIacos
            fmul qword ptr [IID_IDirectInputDevice2A+0x590]
            call _ftol
            and eax, 0xffff
            mov ecx, eax
            mov dword ptr [esp + 0x20], eax
            and ecx, 0xffff
            mov dword ptr [esp + 0x1c], ecx
            fild dword ptr [esp + 0x1c]
            fild dword ptr [esp + 0x20]
            fxch st(1)
            fmul qword ptr [IID_IDirectInputDevice2A+0x598]
            fxch st(1)
            fxch st(1)
            fsin
            movsx ecx, word ptr [esp + 0x28]
            mov edx, ecx
            shl edx, 0xe
            sub edx, ecx
            lea eax, [eax + edx*2]
            mov dword ptr [esp + 0x28], eax
            fild dword ptr [esp + 0x28]
            fxch st(1)
            fst dword ptr [esp + 0x28]
            fxch st(1)
            fmul dword ptr [esp + 0x24]
            fxch st(1)
            fxch st(2)
            fxch st(1)
            fxch st(1)
            fsub st(0), st(1)
            call _ftol
            and eax, 0xffff
            mov dword ptr [esp + 0x28], eax
            fild dword ptr [esp + 0x28]
            fmul qword ptr [IID_IDirectInputDevice2A+0x598]
            fxch st(1)
            fxch st(1)
            fsin
            fst dword ptr [esp + 0x28]
            fdiv st(0), st(2)
            fstp dword ptr [esp + 0x28]
            call _ftol
            and eax, 0xffff
            mov dword ptr [esp + 0x24], eax
            fild dword ptr [esp + 0x24]
            fmul qword ptr [IID_IDirectInputDevice2A+0x598]
            fsin
            fdivrp st(1), st(0)
            jmp L00535930
    L0053591c:
            fld dword ptr [IID_IDirectInputDevice2A+0x584]
            fsub dword ptr [esp + 0x24]
            fld dword ptr [esp + 0x24]
            fxch st(1)
            fstp dword ptr [esp + 0x28]
    L00535930:
            fld dword ptr [esp + 0x28]
            fld dword ptr [esp + 4]
            fmul st(0), st(2)
            fld dword ptr [esp + 8]
            fxch st(2)
            _emit 0xd8
        _emit 0x0e
            fld dword ptr [esp + 0xc]
            fxch st(3)
            fmul st(0), st(4)
            fxch st(1)
            faddp st(2), st(0)
            fxch st(2)
            fmul st(0), st(3)
            fld dword ptr [esp + 0x10]
            fxch st(2)
            mov eax, dword ptr [esp + 0x18]
            _emit 0xd9
        _emit 0x18
            fld dword ptr [esi + 4]
            fmul dword ptr [esp + 0x28]
            fxch st(2)
            fmul st(0), st(4)
            fxch st(3)
            faddp st(2), st(0)
            fxch st(1)
            fstp dword ptr [eax + 4]
            fld dword ptr [esi + 8]
            fmul dword ptr [esp + 0x28]
            fxch st(1)
            faddp st(1), st(0)
            fstp dword ptr [eax + 8]
            fld dword ptr [esi + 0xc]
            fmul dword ptr [esp + 0x28]
            fxch st(1)
            pop esi
            faddp st(1), st(0)
            fstp dword ptr [eax + 0xc]
            fstp st(0)
            add esp, 0x10
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x005359a0
C2_NAKED br_matrix34* C2_HOOK_CDECL BrQuatToMatrix34(br_matrix34* mat, const br_quat* q) {
#ifndef CARPOCALYPSE2_MATCHING
    br_scalar xs;
    br_scalar ys;
    br_scalar zs;
    br_scalar wx;
    br_scalar wy;
    br_scalar wz;
    br_scalar xx;
    br_scalar xy;
    br_scalar xz;
    br_scalar yy;
    br_scalar yz;
    br_scalar zz;

    zz = q->z + q->z;
    yy = q->y + q->y;
    zs = q->z * zz;
    xx = q->x + q->x;
    wx = q->w * xx;
    wy = q->w * yy;
    wz = q->w * zz;
    xy = q->x * yy;
    xz = q->x * zz;
    ys = q->y * yy;
    xs = q->x * xx;
    yz = q->y * zz;

    M(0, 0) = 1.f - (ys + zs);
    M(1, 0) = xy - wz;
    M(2, 0) = xz + wy;
    M(0, 1) = xy + wz;
    M(1, 1) = 1.f - (zs + xs);
    M(2, 1) = yz - wx;
    M(0, 2) = xz - wy;
    M(1, 2) = yz + wx;
    M(2, 2) = 1.f - (xs + ys);
    M(3, 0) = 0.f;
    M(3, 1) = 0.f;
    M(3, 2) = 0.f;

    return mat;
#else
    __asm {
            sub esp, 0x1c
            mov eax, dword ptr [esp + 0x24]
            fld dword ptr [eax + 4]
            fld dword ptr [eax + 8]
            fadd st(0), st(0)
            fld dword ptr [eax + 8]
            fxch st(2)
            fadd st(0), st(0)
            fld dword ptr [eax + 4]
            fxch st(2)
            fstp dword ptr [esp + 0x24]
            _emit 0xd9
        _emit 0x00
            fld dword ptr [esp + 0x24]
            fxch st(4)
            fmul dword ptr [esp + 0x24]
            fxch st(2)
            _emit 0xd9
        _emit 0x54
        _emit 0x24
        _emit 0x00
            fxch st(1)
            fadd st(0), st(0)
            fld dword ptr [eax + 0xc]
            fld st(1)
            fld dword ptr [eax + 0xc]
            fmul st(0), st(3)
            fstp dword ptr [esp + 0x18]
            fxch st(1)
            _emit 0xd8
        _emit 0x4c
        _emit 0x24
        _emit 0x00
            fstp dword ptr [esp + 0x14]
            fld dword ptr [eax + 0xc]
            fmul dword ptr [esp + 0x24]
            fstp dword ptr [esp + 4]
            fxch st(2)
            _emit 0xd8
        _emit 0x08
            fxch st(5)
            _emit 0xd8
        _emit 0x08
            fxch st(4)
            _emit 0xd8
        _emit 0x4c
        _emit 0x24
        _emit 0x00
            fxch st(3)
            fst dword ptr [esp + 8]
            fld dword ptr [eax + 4]
            fxch st(3)
            _emit 0xd8
        _emit 0x08
            fstp dword ptr [esp + 0x10]
            fxch st(3)
            fstp dword ptr [esp + 0xc]
            fxch st(2)
            fadd dword ptr [esp + 0xc]
            fxch st(4)
            _emit 0xd9
        _emit 0x54
        _emit 0x24
        _emit 0x00
            _emit 0xd9
        _emit 0x44
        _emit 0x24
        _emit 0x00
            fxch st(4)
            _emit 0xd9
        _emit 0x54
        _emit 0x24
        _emit 0x00
            fxch st(1)
            fsub dword ptr [esp + 4]
            fxch st(1)
            fadd dword ptr [esp + 0x14]
            fxch st(4)
            fadd dword ptr [esp + 4]
            fxch st(2)
            fmul dword ptr [esp + 0x24]
            fxch st(5)
            fsubr dword ptr [IID_IDirectInputDevice2A+0x584]
            fxch st(5)
            fstp dword ptr [esp + 0x24]
            mov eax, dword ptr [esp + 0x20]
            xor ecx, ecx
            fxch st(4)
            _emit 0xd9
        _emit 0x18
            fxch st(3)
            fstp dword ptr [eax + 0xc]
            fxch st(1)
            fstp dword ptr [eax + 0x18]
            fxch st(1)
            fstp dword ptr [eax + 4]
            fstp st(0)
            fld dword ptr [esp + 0xc]
            fld dword ptr [esp + 8]
            fadd dword ptr [esp + 0x10]
            fxch st(1)
            fadd dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x24]
            _emit 0xd9
        _emit 0x44
        _emit 0x24
        _emit 0x00
            fld dword ptr [esp + 0x24]
            fxch st(2)
            fsub dword ptr [esp + 0x18]
            fxch st(1)
            fsub dword ptr [esp + 0x14]
            fxch st(2)
            fadd dword ptr [esp + 0x18]
            fxch st(4)
            fsubr dword ptr [IID_IDirectInputDevice2A+0x584]
            fxch st(3)
            fsubr dword ptr [IID_IDirectInputDevice2A+0x584]
            fxch st(3)
            fstp dword ptr [eax + 0x10]
            fstp dword ptr [eax + 0x1c]
            fstp dword ptr [eax + 8]
            fxch st(1)
            fstp dword ptr [eax + 0x14]
            fstp dword ptr [eax + 0x20]
            mov dword ptr [eax + 0x24], ecx
            mov dword ptr [eax + 0x28], ecx
            mov dword ptr [eax + 0x2c], ecx
            add esp, 0x1c
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00535ae0
C2_NAKED br_quat* C2_HOOK_CDECL BrMatrix34ToQuat(br_quat* q, const br_matrix34* mat) {
#ifndef CARPOCALYPSE2_MATCHING
    br_scalar tr;
    br_scalar s;
    int i;
    int j;
    int k;

    tr = M(0, 0) + M(1, 1) + M(2, 2);
    if (tr >= 0.f) {
        tr = (br_scalar)sqrt(tr + 1.0f);
        s = 0.5f / tr;
        q->w = tr * 0.5f;
        q->x = (M(1, 2) - M(2, 1)) * s;
        q->y = (M(2, 0) - M(0, 2)) * s;
        q->z = (M(0, 1) - M(1, 0)) * s;
        return q;
    }
    i = M_DIAG(1) >= M_DIAG(0) ? 1 : 0;
    i = M_DIAG(2) >= M_DIAG(i) ? 2 : i;
    j = (i + 1) % 3;
    k = (j + 1) % 3;
    tr = (br_scalar)sqrt(M_DIAG(i) - (M_DIAG(j) + M_DIAG(k)) + 1.f);
    s = 0.5f / tr;
    Q_EL(i) = 0.5f * tr;
    Q_EL(j) = (M(i, j) + M(j, i)) * s;
    Q_EL(k) = (M(i, k) + M(k, i)) * s;
    q->w = (M(k, j) - M(j, k)) * s;
    return q;
#else
    __asm {
            mov ecx, dword ptr [esp + 8]
            sub esp, 0x14
            _emit 0xd9
        _emit 0x01
            fadd dword ptr [ecx + 0x20]
            fadd dword ptr [ecx + 0x10]
            fcom dword ptr [IID_IDirectInputDevice2A+0x588]
            fnstsw ax
            test ah, 1
            jne L00535b4f
            fsub dword ptr [IID_IDirectInputDevice2A+0x5a4]
            fld dword ptr [IID_IDirectInputDevice2A+0x580]
            fxch st(1)
            fsqrt
            mov eax, dword ptr [esp + 0x18]
            fxch st(1)
            fxch st(1)
            _emit 0xd9
        _emit 0x5c
        _emit 0x24
        _emit 0x00
            _emit 0xd8
        _emit 0x74
        _emit 0x24
        _emit 0x00
            _emit 0xd9
        _emit 0x44
        _emit 0x24
        _emit 0x00
            fmul dword ptr [IID_IDirectInputDevice2A+0x580]
            fstp dword ptr [eax + 0xc]
            fld dword ptr [ecx + 0x14]
            fsub dword ptr [ecx + 0x1c]
            fmul st(0), st(1)
            _emit 0xd9
        _emit 0x18
            fld dword ptr [ecx + 0x18]
            fsub dword ptr [ecx + 8]
            fmul st(0), st(1)
            fstp dword ptr [eax + 4]
            fld dword ptr [ecx + 4]
            fsub dword ptr [ecx + 0xc]
            fmul st(0), st(1)
            fstp dword ptr [eax + 8]
            fstp st(0)
            add esp, 0x14
            ret
    L00535b4f:
            fstp st(0)
            fld dword ptr [ecx + 0x10]
            _emit 0xd8
        _emit 0x19
            push edi
            push esi
            xor edx, edx
            push ebp
            push ebx
            mov dword ptr [esp + 0x10], edx
            fnstsw ax
            test ah, 0x41
            jne L00535b70
            mov edx, 1
            mov dword ptr [esp + 0x10], edx
    L00535b70:
            fld dword ptr [ecx + 0x20]
            mov eax, edx
            shl eax, 4
            fcomp dword ptr [eax + ecx]
            fnstsw ax
            test ah, 0x41
            jne L00535b8b
            mov edx, 2
            mov dword ptr [esp + 0x10], edx
    L00535b8b:
            lea eax, [edx*4]
            mov edi, edx
            mov dword ptr [esp + 0x1c], eax
            mov eax, dword ptr [eax + _CombineTransforms+0xb8]
            mov dword ptr [esp + 0x18], eax
            lea edx, [eax*4]
            mov dword ptr [esp + 0x20], edx
            mov edx, dword ptr [edx + _CombineTransforms+0xb8]
            mov dword ptr [esp + 0x14], edx
            shl edx, 4
            shl eax, 4
            fld dword ptr [edx + ecx]
            fld dword ptr [IID_IDirectInputDevice2A+0x580]
            fxch st(1)
            fadd dword ptr [eax + ecx]
            mov eax, dword ptr [esp + 0x18]
            shl edi, 4
            fsubr dword ptr [edi + ecx]
            mov ecx, dword ptr [esp + 0x14]
            lea esi, [eax + eax*2]
            fsub dword ptr [IID_IDirectInputDevice2A+0x5a4]
            lea edx, [ecx + esi]
            lea edi, [ecx + ecx*2]
            mov dword ptr [esp + 0x18], edx
            mov edx, dword ptr [esp + 0x10]
            fsqrt
            lea ebx, [eax + edi]
            add esi, edx
            add edi, edx
            lea edx, [edx + edx*2]
            lea ebp, [eax + edx]
            mov eax, dword ptr [esp + 0x1c]
            add edx, ecx
            mov ecx, dword ptr [esp + 0x28]
            fxch st(1)
            fxch st(1)
            fstp dword ptr [esp + 0x10]
            fdiv dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x10]
            fmul dword ptr [IID_IDirectInputDevice2A+0x580]
            fstp dword ptr [ecx + eax]
            mov eax, dword ptr [esp + 0x18]
            mov ecx, dword ptr [esp + 0x2c]
            fld dword ptr [ecx + eax*4]
            fsub dword ptr [ecx + ebx*4]
            mov eax, dword ptr [esp + 0x28]
            pop ebx
            fmul st(0), st(1)
            fstp dword ptr [eax + 0xc]
            fld dword ptr [ecx + ebp*4]
            fadd dword ptr [ecx + esi*4]
            mov esi, dword ptr [esp + 0x1c]
            pop ebp
            fmul st(0), st(1)
            fstp dword ptr [eax + esi]
            fld dword ptr [ecx + edx*4]
            fadd dword ptr [ecx + edi*4]
            mov ecx, dword ptr [esp + 0xc]
            pop esi
            pop edi
            fmul st(0), st(1)
            fstp dword ptr [eax + ecx*4]
            fstp st(0)
            add esp, 0x14
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00535c70
br_matrix4* C2_HOOK_CDECL BrQuatToMatrix4(br_matrix4* mat, const br_quat* q) {
    br_matrix34 tmp;

    BrQuatToMatrix34(&tmp, q);
    BrMatrix4Copy34(mat, &tmp);
    return mat;
}

// FUNCTION: CARMA2_HW 0x00535ca0
br_quat* C2_HOOK_CDECL BrMatrix4ToQuat(br_quat* q, const br_matrix4* mat) {
    br_matrix34 tmp;

    BrMatrix34Copy4(&tmp, mat);
    return BrMatrix34ToQuat(q, &tmp);
}
