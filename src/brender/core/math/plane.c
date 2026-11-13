#include "plane.h"

#include "core/std/brmath.h"

#include "c2_math.h"

#ifdef CARPOCALYPSE2_MATCHING
extern float C2_HOOK_CDECL BrFloatSqrt();
extern char IID_IDirectInputDevice2A[];
#endif
// FUNCTION: CARMA2_HW 0x00536fb0
br_int_32 C2_NAKED C2_HOOK_CDECL BrPlaneEquation(br_vector4* eqn, const br_vector3* v0, const br_vector3* v1, const br_vector3* v2) {
#ifndef CARPOCALYPSE2_MATCHING
    float ax;
    float ay;
    float az;
    float bx;
    float by;
    float bz;
    float nx;
    float ny;
    float nz;
    float d;
    float l;

    ax = v1->v[0] - v0->v[0];
    ay = v1->v[1] - v0->v[1];
    az = v1->v[2] - v0->v[2];
    bx = v2->v[0] - v0->v[0];
    by = v2->v[1] - v0->v[1];
    bz = v2->v[2] - v0->v[2];
    nx = ay * bz - az * by;
    ny = az * bx - ax * bz;
    nz = ax * by - ay * bx;
    l = nx * nx + ny * ny + nz * nz;
    l = BrFloatSqrt(l);
    if (isnan(l)) {
        nx = 0.f;
        ny = 0.f;
        nz = 1.f;
    } else {
        nx = nx * (1.f / l);
        ny = ny * (1.f / l);
        nz = nz * (1.f / l);
    }
    d = nx * v0->v[0] + ny * v0->v[1] + nz * v0->v[2];
    eqn->v[0] = nx;
    eqn->v[1] = ny;
    eqn->v[2] = nz;
    eqn->v[3] = d;
    return !isnan(l);
#else
    __asm {
            sub esp, 0x18
            mov eax, dword ptr [esp + 0x28]
            push esi
            mov esi, dword ptr [esp + 0x24]
            push ecx
            _emit 0xd9
        _emit 0x00
            fld dword ptr [eax + 4]
            fld dword ptr [eax + 8]
            mov eax, dword ptr [esp + 0x2c]
            _emit 0xd9
        _emit 0x00
            fld dword ptr [eax + 4]
            fld dword ptr [eax + 8]
            fxch st(2)
            _emit 0xd8
        _emit 0x26
            fstp dword ptr [esp + 8]
            fxch st(4)
            _emit 0xd8
        _emit 0x26
            fxch st(3)
            fsub dword ptr [esi + 4]
            fxch st(2)
            fsub dword ptr [esi + 8]
            fxch st(4)
            fsub dword ptr [esi + 4]
            fxch st(1)
            fsub dword ptr [esi + 8]
            fxch st(3)
            fst dword ptr [esp + 0x14]
            fxch st(2)
            fst dword ptr [esp + 0x18]
            fld st(4)
            fxch st(2)
            fstp dword ptr [esp + 0xc]
            fxch st(1)
            fmul dword ptr [esp + 0xc]
            fxch st(3)
            fstp dword ptr [esp + 0x10]
            fmul dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x18]
            fxch st(2)
            fmul dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x14]
            fxch st(2)
            fsubp st(4), st(0)
            fxch st(4)
            fmul dword ptr [esp + 8]
            fxch st(3)
            fstp dword ptr [esp + 0x30]
            fxch st(2)
            fsubp st(3), st(0)
            fld dword ptr [esp + 0x30]
            fmul dword ptr [esp + 0x30]
            fxch st(3)
            fst dword ptr [esp + 0x2c]
            fmul dword ptr [esp + 0x2c]
            fxch st(1)
            fmul dword ptr [esp + 8]
            fxch st(1)
            faddp st(3), st(0)
            fxch st(1)
            fmul dword ptr [esp + 0xc]
            fsubp st(1), st(0)
            fst dword ptr [esp + 0x28]
            fmul dword ptr [esp + 0x28]
            faddp st(1), st(0)
            _emit 0xd9
        _emit 0x1c
        _emit 0x24
            call BrFloatSqrt
            fcom dword ptr [IID_IDirectInputDevice2A+0x5c4]
            add esp, 4
            fnstsw ax
            test ah, 0x40
            jne L005370a3
            fdivr dword ptr [IID_IDirectInputDevice2A+0x5c8]
            fld st(0)
            fld st(1)
            fld st(2)
            fxch st(2)
            fmul dword ptr [esp + 0x2c]
            fxch st(1)
            fmul dword ptr [esp + 0x28]
            fxch st(2)
            fmul dword ptr [esp + 0x24]
            fxch st(1)
            fxch st(2)
            fxch st(1)
            jmp L005370b5
    L005370a3:
            fld dword ptr [IID_IDirectInputDevice2A+0x5c4]
            fld dword ptr [IID_IDirectInputDevice2A+0x5c4]
            fld dword ptr [IID_IDirectInputDevice2A+0x5c8]
    L005370b5:
            fld dword ptr [esi + 8]
            fld dword ptr [esi + 4]
            fld st(4)
            fxch st(2)
            fmul st(0), st(3)
            fxch st(1)
            fmul st(0), st(4)
            fxch st(2)
            _emit 0xd8
        _emit 0x0e
            fxch st(2)
            faddp st(1), st(0)
            fxch st(5)
            fcomp dword ptr [IID_IDirectInputDevice2A+0x5c4]
            faddp st(4), st(0)
            fxch st(3)
            fnstsw ax
            fst dword ptr [esp + 0x2c]
            test ah, 0x40
            mov eax, dword ptr [esp + 0x20]
            fxch st(2)
            _emit 0xd9
        _emit 0x18
            fstp dword ptr [eax + 4]
            fxch st(1)
            fstp dword ptr [eax + 8]
            fstp dword ptr [eax + 0xc]
            je L00537101
            mov eax, 1
            pop esi
            add esp, 0x18
            ret
    L00537101:
            xor eax, eax
            pop esi
            add esp, 0x18
            ret
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
