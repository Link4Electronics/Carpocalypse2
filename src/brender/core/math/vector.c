#include "vector.h"
#include "c2_math.h"


#ifdef CARPOCALYPSE2_MATCHING
extern char IID_IDirectInputDevice2A[];
#endif
// FUNCTION: CARMA2_HW 0x00534390
void C2_HOOK_CDECL BrVector2Copy(br_vector2* v1, const br_vector2* v2) {

    v1->v[0] = v2->v[0];
    v1->v[1] = v2->v[1];
}

// FUNCTION: CARMA2_HW 0x005343b0
void C2_HOOK_CDECL BrVector2Set(br_vector2* v1, br_scalar s1, br_scalar s2) {

    v1->v[0] = s1;
    v1->v[1] = s2;
}

// FUNCTION: CARMA2_HW 0x005343d0
void C2_HOOK_CDECL BrVector2SetInt(br_vector2* v1, int i1, int i2) {

    v1->v[0] = BR_SCALAR(i1);
    v1->v[1] = BR_SCALAR(i2);
}

// FUNCTION: CARMA2_HW 0x005343f0
void C2_HOOK_CDECL BrVector2SetFloat(br_vector2* v1, float f1, float f2) {

    v1->v[0] = f1;
    v1->v[1] = f2;
}

// FUNCTION: CARMA2_HW 0x00534410
void C2_HOOK_CDECL BrVector2Negate(br_vector2* v1, const br_vector2* v2) {

    v1->v[0] = -v2->v[0];
    v1->v[1] = -v2->v[1];
}

// FUNCTION: CARMA2_HW 0x00534430
void C2_HOOK_CDECL BrVector2Add(br_vector2* v1, const br_vector2* v2, const br_vector2* v3) {

    v1->v[0] = v2->v[0] + v3->v[0];
    v1->v[1] = v2->v[1] + v3->v[1];
}

// FUNCTION: CARMA2_HW 0x00534450
void C2_NAKED C2_HOOK_CDECL BrVector2Accumulate(br_vector2* v1, const br_vector2* v2) {
#ifndef CARPOCALYPSE2_MATCHING

    v1->v[0] += v2->v[0];
    v1->v[1] += v2->v[1];
#else
    __asm {
            mov eax, dword ptr [esp + 4]
            mov ecx, dword ptr [esp + 8]
            _emit 0xd9
        _emit 0x00
            _emit 0xd8
        _emit 0x01
            _emit 0xd9
        _emit 0x18
            fld dword ptr [ecx + 4]
            fadd dword ptr [eax + 4]
            fstp dword ptr [eax + 4]
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00534470
void C2_HOOK_CDECL BrVector2Sub(br_vector2* v1, const br_vector2* v2, const br_vector2* v3) {

    v1->v[0] = v2->v[0] - v3->v[0];
    v1->v[1] = v2->v[1] - v3->v[1];
}

// FUNCTION: CARMA2_HW 0x00534490
void C2_HOOK_CDECL BrVector2Scale(br_vector2* v1, const br_vector2* v2, br_scalar s) {

    v1->v[0] = v2->v[0] * s;
    v1->v[1] = v2->v[1] * s;
}

// FUNCTION: CARMA2_HW 0x005344b0
void C2_HOOK_CDECL BrVector2InvScale(br_vector2* v1, const br_vector2* v2, br_scalar s) {

    v1->v[0] = v2->v[0] / s;
    v1->v[1] = v2->v[1] / s;
}

// FUNCTION: CARMA2_HW 0x005344d0
br_scalar C2_NAKED C2_HOOK_CDECL BrVector2Dot(br_vector2* v1, const br_vector2* v2) {
#ifndef CARPOCALYPSE2_MATCHING

    return BR_MAC2(v1->v[0], v2->v[0], v1->v[1], v2->v[1]);
#else
    __asm {
            mov eax, dword ptr [esp + 4]
            mov ecx, dword ptr [esp + 8]
            fld dword ptr [eax + 4]
            fld dword ptr [ecx + 4]
            _emit 0xd9
        _emit 0x01
            _emit 0xd8
        _emit 0x08
            fxch st(1)
            fmulp st(2), st(0)
            faddp st(1), st(0)
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x005344f0
br_scalar C2_HOOK_CDECL BrVector2Length(const br_vector2* v1) {

    return (br_scalar)sqrt(BR_MAC2(v1->v[0], v1->v[0], v1->v[1], v1->v[1]));
}

// FUNCTION: CARMA2_HW 0x00534510
br_scalar C2_HOOK_CDECL BrVector2LengthSquared(const br_vector2* v1) {

    return BR_MAC2(v1->v[0], v1->v[0], v1->v[1], v1->v[1]);
}

// FUNCTION: CARMA2_HW 0x00534530
void C2_HOOK_CDECL BrVector3Copy(br_vector3* v1, const br_vector3* v2) {

    v1->v[0] = v2->v[0];
    v1->v[1] = v2->v[1];
    v1->v[2] = v2->v[2];
}

// FUNCTION: CARMA2_HW 0x00534550
void C2_HOOK_CDECL BrVector3Set(br_vector3* v1, br_scalar s1, br_scalar s2, br_scalar s3) {

    v1->v[0] = s1;
    v1->v[1] = s2;
    v1->v[2] = s3;
}

// FUNCTION: CARMA2_HW 0x00534570
void C2_HOOK_CDECL BrVector3SetInt(br_vector3* v1, int i1, int i2, int i3) {

    v1->v[0] = BR_SCALAR(i1);
    v1->v[1] = BR_SCALAR(i2);
    v1->v[2] = BR_SCALAR(i3);
}

// FUNCTION: CARMA2_HW 0x00534590
void C2_HOOK_CDECL BrVector3SetFloat(br_vector3* v1, float f1, float f2, float f3) {

    v1->v[0] = BR_SCALAR(f1);
    v1->v[1] = BR_SCALAR(f2);
    v1->v[2] = BR_SCALAR(f3);
}

// FUNCTION: CARMA2_HW 0x005345b0
void C2_HOOK_CDECL BrVector3Negate(br_vector3* v1, const br_vector3* v2) {

    v1->v[0] = -v2->v[0];
    v1->v[1] = -v2->v[1];
    v1->v[2] = -v2->v[2];
}

// FUNCTION: CARMA2_HW 0x005345d0
void C2_NAKED C2_HOOK_CDECL BrVector3Add(br_vector3* v1, const br_vector3* v2, const br_vector3* v3) {
#ifndef CARPOCALYPSE2_MATCHING

    v1->v[0] = v2->v[0] + v3->v[0];
    v1->v[1] = v2->v[1] + v3->v[1];
    v1->v[2] = v2->v[2] + v3->v[2];
#else
    __asm {
            mov eax, dword ptr [esp + 8]
            mov ecx, dword ptr [esp + 0xc]
            mov edx, dword ptr [esp + 4]
            _emit 0xd9
        _emit 0x00
            _emit 0xd8
        _emit 0x01
            _emit 0xd9
        _emit 0x1a
            fld dword ptr [ecx + 4]
            fadd dword ptr [eax + 4]
            fstp dword ptr [edx + 4]
            fld dword ptr [ecx + 8]
            fadd dword ptr [eax + 8]
            fstp dword ptr [edx + 8]
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00534600
void C2_NAKED C2_HOOK_CDECL BrVector3Accumulate(br_vector3* v1, const br_vector3* v2) {
#ifndef CARPOCALYPSE2_MATCHING

    v1->v[0] += v2->v[0];
    v1->v[1] += v2->v[1];
    v1->v[2] += v2->v[2];
#else
    __asm {
            mov ecx, dword ptr [esp + 8]
            mov eax, dword ptr [esp + 4]
            _emit 0xd9
        _emit 0x01
            _emit 0xd8
        _emit 0x00
            _emit 0xd9
        _emit 0x18
            fld dword ptr [ecx + 4]
            fadd dword ptr [eax + 4]
            fstp dword ptr [eax + 4]
            fld dword ptr [ecx + 8]
            fadd dword ptr [eax + 8]
            fstp dword ptr [eax + 8]
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00534630
void C2_HOOK_CDECL BrVector3Sub(br_vector3* v1, const br_vector3* v2, const br_vector3* v3) {

    v1->v[0] = v2->v[0] - v3->v[0];
    v1->v[1] = v2->v[1] - v3->v[1];
    v1->v[2] = v2->v[2] - v3->v[2];
}

// FUNCTION: CARMA2_HW 0x00534660
void C2_HOOK_CDECL BrVector3Scale(br_vector3* v1, const br_vector3* v2, br_scalar s) {

    v1->v[0] = v2->v[0] * s;
    v1->v[1] = v2->v[1] * s;
    v1->v[2] = v2->v[2] * s;
}

// FUNCTION: CARMA2_HW 0x00534690
void C2_HOOK_CDECL BrVector3InvScale(br_vector3* v1, const br_vector3* v2, br_scalar s) {

    v1->v[0] = v2->v[0] / s;
    v1->v[1] = v2->v[1] / s;
    v1->v[2] = v2->v[2] / s;
}

// FUNCTION: CARMA2_HW 0x005346c0
br_scalar C2_NAKED C2_HOOK_CDECL BrVector3Dot(br_vector3* v1, const br_vector3* v2) {
#ifndef CARPOCALYPSE2_MATCHING

    return BR_MAC3(v2->v[1], v1->v[1], v2->v[2], v1->v[2], v2->v[0], v1->v[0]);
#else
    __asm {
            mov eax, dword ptr [esp + 8]
            mov ecx, dword ptr [esp + 4]
            fld dword ptr [eax + 4]
            fld dword ptr [ecx + 4]
            fld dword ptr [eax + 8]
            _emit 0xd9
        _emit 0x00
            fxch st(2)
            fmulp st(3), st(0)
            fmul dword ptr [ecx + 8]
            fxch st(1)
            _emit 0xd8
        _emit 0x09
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x005346f0
void C2_NAKED C2_HOOK_CDECL BrVector3Cross(br_vector3* v1, const br_vector3* v2, const br_vector3* v3) {
#ifndef CARPOCALYPSE2_MATCHING

    v1->v[0] = v2->v[1] * v3->v[2] - v2->v[2] * v3->v[1];
    v1->v[1] = v2->v[2] * v3->v[0] - v2->v[0] * v3->v[2];
    v1->v[2] = v2->v[0] * v3->v[1] - v2->v[1] * v3->v[0];
#else
    __asm {
            mov ecx, dword ptr [esp + 0xc]
            mov eax, dword ptr [esp + 8]
            mov edx, dword ptr [esp + 4]
            fld dword ptr [ecx + 8]
            fld dword ptr [eax + 4]
            fld dword ptr [eax + 8]
            fxch st(1)
            fmulp st(2), st(0)
            fmul dword ptr [ecx + 4]
            fsubp st(1), st(0)
            _emit 0xd9
        _emit 0x1a
            fld dword ptr [ecx + 8]
            fld dword ptr [eax + 8]
            _emit 0xd8
        _emit 0x09
            fxch st(1)
            _emit 0xd8
        _emit 0x08
            fsubp st(1), st(0)
            fstp dword ptr [edx + 4]
            fld dword ptr [eax + 4]
            fld dword ptr [ecx + 4]
            _emit 0xd8
        _emit 0x08
            fxch st(1)
            _emit 0xd8
        _emit 0x09
            fsubp st(1), st(0)
            fstp dword ptr [edx + 8]
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00534740
br_scalar C2_HOOK_CDECL BrVector3Length(const br_vector3* v1) {

    return (br_scalar)sqrt(BR_MAC3(v1->v[0], v1->v[0], v1->v[1], v1->v[1], v1->v[2], v1->v[2]));
}

// FUNCTION: CARMA2_HW 0x00534780
br_scalar C2_HOOK_CDECL BrVector3LengthSquared(const br_vector3* v1) {

    return BR_MAC3(v1->v[0], v1->v[0], v1->v[1], v1->v[1], v1->v[2], v1->v[2]);
}

// FUNCTION: CARMA2_HW 0x005347c0
void C2_NAKED C2_HOOK_CDECL BrVector3Normalise(br_vector3* v1, const br_vector3* v2) {
#ifndef CARPOCALYPSE2_MATCHING
    br_scalar scale;

    scale = BR_LENGTH3(v2->v[0], v2->v[1], v2->v[2]);

    if (scale > BR_SCALAR_EPSILON * 2) {
        v1->v[0] = v2->v[0] / scale;
        v1->v[1] = v2->v[1] / scale;
        v1->v[2] = v2->v[2] / scale;

    } else {
        v1->v[0] = BR_SCALAR(1.0);
        v1->v[1] = BR_SCALAR(0.0);
        v1->v[2] = BR_SCALAR(0.0);
    }
#else
    __asm {
            push ecx
            mov ecx, dword ptr [esp + 0xc]
            fld dword ptr [ecx + 4]
            fld dword ptr [ecx + 8]
            _emit 0xd9
        _emit 0x01
            fxch st(2)
            fst dword ptr [esp + 0xc]
            fxch st(1)
            _emit 0xd9
        _emit 0x54
        _emit 0x24
        _emit 0x00
            fxch st(1)
            fmul dword ptr [esp + 0xc]
            fxch st(1)
            _emit 0xd8
        _emit 0x4c
        _emit 0x24
        _emit 0x00
            fld st(2)
            fmul st(0), st(3)
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fsqrt
            fstp st(1)
            fcom dword ptr [IID_IDirectInputDevice2A+0x540]
            fnstsw ax
            test ah, 0x41
            jne L00534824
            fdivr dword ptr [IID_IDirectInputDevice2A+0x544]
            mov eax, dword ptr [esp + 8]
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
            fstp st(0)
            pop ecx
            ret
    L00534824:
            mov eax, dword ptr [esp + 8]
            xor ecx, ecx
            fstp st(0)
            mov dword ptr [eax], 0x3f800000
            mov dword ptr [eax + 4], ecx
            mov dword ptr [eax + 8], ecx
            pop ecx
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00534840
void C2_NAKED C2_HOOK_CDECL BrVector3NormaliseLP(br_vector3* v1, const br_vector3* v2) {
#ifndef CARPOCALYPSE2_MATCHING
    br_scalar scale;

    scale = 1.0 / sqrt(BR_MAC3(v2->v[0], v2->v[0], v2->v[1], v2->v[1], v2->v[2], v2->v[2]));
    if (scale != 0.f) {
        v1->v[0] = v2->v[0] * scale;
        v1->v[1] = v2->v[1] * scale;
        v1->v[2] = v2->v[2] * scale;
    }
#else
    __asm {
            push ecx
            mov ecx, dword ptr [esp + 0xc]
            fld dword ptr [ecx + 4]
            fld dword ptr [ecx + 8]
            _emit 0xd9
        _emit 0x01
            fxch st(2)
            fst dword ptr [esp + 0xc]
            fxch st(1)
            _emit 0xd9
        _emit 0x54
        _emit 0x24
        _emit 0x00
            fxch st(1)
            fmul dword ptr [esp + 0xc]
            fxch st(1)
            _emit 0xd8
        _emit 0x4c
        _emit 0x24
        _emit 0x00
            fld st(2)
            fmul st(0), st(3)
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fsqrt
            fdivr qword ptr [IID_IDirectInputDevice2A+0x548]
            fstp st(1)
            fcom dword ptr [IID_IDirectInputDevice2A+0x550]
            fnstsw ax
            test ah, 0x40
            jne L005348a0
            mov eax, dword ptr [esp + 8]
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
    L005348a0:
            fstp st(0)
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
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x005348b0
br_scalar C2_NAKED C2_HOOK_CDECL BrVector4Dot(const br_vector4* v1, const br_vector4* v2) {
#ifndef CARPOCALYPSE2_MATCHING

    return BR_MAC4(v1->v[0], v2->v[0], v1->v[1], v2->v[1], v1->v[2], v2->v[2], v1->v[3], v2->v[3]);
#else
    __asm {
            mov eax, dword ptr [esp + 8]
            mov ecx, dword ptr [esp + 4]
            fld dword ptr [eax + 4]
            fld dword ptr [ecx + 4]
            fld dword ptr [eax + 8]
            fld dword ptr [ecx + 8]
            fld dword ptr [eax + 0xc]
            fxch st(3)
            fmulp st(4), st(0)
            fmulp st(1), st(0)
            faddp st(2), st(0)
            fmul dword ptr [ecx + 0xc]
            _emit 0xd9
        _emit 0x01
            _emit 0xd8
        _emit 0x08
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x005348e0
void C2_HOOK_CDECL BrVector4Copy(br_vector4* v1, const br_vector4* v2) {

    v1->v[0] = v2->v[0];
    v1->v[1] = v2->v[1];
    v1->v[2] = v2->v[2];
    v1->v[3] = v2->v[3];
}

// FUNCTION: CARMA2_HW 0x00534900
br_scalar C2_NAKED C2_HOOK_STDCALL BrFVector2Dot(const br_fvector2* v1, const br_vector2* v2) {
#ifndef CARPOCALYPSE2_MATCHING

    return BR_MAC2(v1->v[0], v2->v[0], v1->v[1], v2->v[1]);
#else
    __asm {
            mov eax, dword ptr [esp + 4]
            mov ecx, dword ptr [esp + 8]
            fld dword ptr [eax + 4]
            fld dword ptr [ecx + 4]
            _emit 0xd9
        _emit 0x01
            _emit 0xd8
        _emit 0x08
            fxch st(1)
            fmulp st(2), st(0)
            faddp st(1), st(0)
            ret 8
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00534920
void C2_HOOK_STDCALL BrFVector3Copy(br_fvector3* v1, const br_vector3* v2) {

    br_scalar t1;
    br_scalar t2;
    v1->v[0] = v2->v[0];
    t1 = v2->v[1];
    v1->v[1] = t1;
    t2 = v2->v[2];
    v1->v[2] = t2;
}

// FUNCTION: CARMA2_HW 0x00534940
void C2_HOOK_STDCALL BrVector3ScaleF(br_vector3* v1, const br_fvector3* v2, br_scalar s) {

    v1->v[0] = s * v2->v[0];
    v1->v[1] = v2->v[1] * s;
    v1->v[2] = v2->v[2] * s;
}

// FUNCTION: CARMA2_HW 0x00534970
br_scalar C2_NAKED C2_HOOK_STDCALL BrFVector3Dot(const br_fvector3* v1, const br_vector3* v2) {
#ifndef CARPOCALYPSE2_MATCHING

    return BR_MAC3(v1->v[0], v2->v[0], v1->v[1], v2->v[1], v1->v[2], v2->v[2]);
#else
    __asm {
            mov eax, dword ptr [esp + 4]
            mov ecx, dword ptr [esp + 8]
            fld dword ptr [eax + 4]
            fld dword ptr [ecx + 4]
            fld dword ptr [eax + 8]
            _emit 0xd9
        _emit 0x01
            fxch st(2)
            fmulp st(3), st(0)
            fmul dword ptr [ecx + 8]
            fxch st(1)
            _emit 0xd8
        _emit 0x08
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            ret 8
    }
#endif
}

// FUNCTION: CARMA2_HW 0x005349a0
void C2_NAKED C2_HOOK_CDECL BrFVector3Normalise(br_fvector3* v1, const br_vector3* v2) {
#ifndef CARPOCALYPSE2_MATCHING
    br_scalar scale;

    scale = (br_scalar)sqrt(BR_MAC3(v2->v[0], v2->v[0], v2->v[1], v2->v[1], v2->v[2], v2->v[2]));
    if (scale != 0.f) {
        scale = 1.f / scale;
        v1->v[0] = v2->v[0] * scale;
        v1->v[1] = v2->v[1] * scale;
        v1->v[2] = v2->v[2] * scale;
    } else {
        v1->v[0] = 1.f;
        v1->v[1] = 0.f;
        v1->v[2] = 0.f;
    }
#else
    __asm {
            push ecx
            mov ecx, dword ptr [esp + 0xc]
            fld dword ptr [ecx + 4]
            fld dword ptr [ecx + 8]
            _emit 0xd9
        _emit 0x01
            fxch st(2)
            fst dword ptr [esp + 0xc]
            fxch st(1)
            _emit 0xd9
        _emit 0x54
        _emit 0x24
        _emit 0x00
            fxch st(1)
            fmul dword ptr [esp + 0xc]
            fxch st(1)
            _emit 0xd8
        _emit 0x4c
        _emit 0x24
        _emit 0x00
            fld st(2)
            fmul st(0), st(3)
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fsqrt
            fstp st(1)
            fcom dword ptr [IID_IDirectInputDevice2A+0x550]
            fnstsw ax
            test ah, 0x40
            jne L00534a06
            fdivr dword ptr [IID_IDirectInputDevice2A+0x544]
            mov eax, dword ptr [esp + 8]
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
            fstp st(0)
            pop ecx
            ret 8
    L00534a06:
            mov eax, dword ptr [esp + 8]
            xor ecx, ecx
            fstp st(0)
            mov dword ptr [eax], 0x3f800000
            mov dword ptr [eax + 4], ecx
            mov dword ptr [eax + 8], ecx
            pop ecx
            ret 8
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00534a20
void C2_NAKED C2_HOOK_CDECL BrFVector3NormaliseLP(br_fvector3* v1, const br_vector3* v2) {
#ifndef CARPOCALYPSE2_MATCHING
    br_scalar scale;

    scale = 1.f / (br_scalar)sqrt(BR_MAC3(v2->v[0], v2->v[0], v2->v[1], v2->v[1], v2->v[2], v2->v[2]));
v1->v[0] = v2->v[0] * scale;
        v1->v[1] = v2->v[1] * scale;
        v1->v[2] = v2->v[2] * scale;
#else
    __asm {
            push ecx
            mov eax, dword ptr [esp + 8]
            mov ecx, dword ptr [esp + 0xc]
            fld dword ptr [eax + 4]
            fld dword ptr [eax + 8]
            _emit 0xd9
        _emit 0x00
            fxch st(2)
            fst dword ptr [esp + 8]
            fxch st(1)
            _emit 0xd9
        _emit 0x54
        _emit 0x24
        _emit 0x00
            fxch st(1)
            fmul dword ptr [esp + 8]
            fxch st(1)
            _emit 0xd8
        _emit 0x4c
        _emit 0x24
        _emit 0x00
            fld st(2)
            fmul st(0), st(3)
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fsqrt
            fdivr qword ptr [IID_IDirectInputDevice2A+0x548]
            fstp st(1)
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
            fstp st(0)
            pop ecx
            ret 8
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00534a80
void C2_NAKED C2_HOOK_CDECL BrVector2Normalise(br_vector2* v1, const br_vector2* v2) {
#ifndef CARPOCALYPSE2_MATCHING
    br_scalar scale;

    scale = (br_scalar)sqrt(BR_MAC2(v2->v[0], v2->v[0], v2->v[1], v2->v[1]));
    if (scale > 2.3841858e-7f) {
        scale = 1.f / scale;
        v1->v[0] = v2->v[0] * scale;
        v1->v[1] = v2->v[1] * scale;

    } else {
        v1->v[0] = 1.f;
        v1->v[1] = 0.f;
    }
#else
    __asm {
            mov ecx, dword ptr [esp + 8]
            _emit 0xd9
        _emit 0x01
            fld dword ptr [ecx + 4]
            fst dword ptr [esp + 8]
            fld st(1)
            fxch st(1)
            fmul dword ptr [esp + 8]
            fxch st(1)
            fmul st(0), st(2)
            faddp st(1), st(0)
            fsqrt
            fstp st(1)
            fcom dword ptr [IID_IDirectInputDevice2A+0x540]
            fnstsw ax
            test ah, 0x41
            jne L00534ac7
            fdivr dword ptr [IID_IDirectInputDevice2A+0x544]
            mov eax, dword ptr [esp + 4]
            fld st(0)
            _emit 0xd8
        _emit 0x09
            _emit 0xd9
        _emit 0x18
            fld dword ptr [ecx + 4]
            fmul st(0), st(1)
            fstp dword ptr [eax + 4]
            fstp st(0)
            ret
    L00534ac7:
            mov eax, dword ptr [esp + 4]
            fstp st(0)
            mov dword ptr [eax], 0x3f800000
            mov dword ptr [eax + 4], 0
            ret
    }
#endif
}
