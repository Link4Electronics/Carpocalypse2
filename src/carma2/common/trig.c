#include "trig.h"
#include "c2_math.h"
#include "trig.h"

// FastFloatSin

// FastFloatCos

// FastScalarSin

// FastScalarCos

// FastScalarSinAngle

// FastScalarCosAngle

// FastFloatArcTan2

// FastScalarArcTan2

// FastFloatArcTan2Angle

// FastScalarArcTan2Angle

// DRMatrix34RotateX

// DRMatrix34RotateY

// DRMatrix34RotateZ

// DRMatrix34PostRotateX

// DRMatrix34PostRotateY

// DRMatrix34PreRotateZ

// DRMatrix34PostRotateZ

// DRMatrix23Rotate

// DRMatrix23PostRotate
// Retail declares the sine table over the full 0..360 degree span (the
// quadrant-mirroring branches index it with `360 - angle`), so the table's
// end pointer is base+0x5a0.  Declare all 361 slots (only 0..90 are
// initialised) so that end-pointer constant stays inside the symbol, which
// is also what the original's folded immediates refer to.
// GLOBAL: CARMA2_HW 0x00661398
float gFloat_sine_table[360] = {
    0.0f,
    0.017452f,
    0.034899f,
    0.052336f,
    0.069756f,
    0.087156f,
    0.104528f,
    0.121869f,
    0.139173f,
    0.156434f,
    0.173648f,
    0.190809f,
    0.207912f,
    0.224951f,
    0.241922f,
    0.258819f,
    0.275637f,
    0.292372f,
    0.309017f,
    0.325568f,
    0.342020f,
    0.358368f,
    0.374607f,
    0.390731f,
    0.406737f,
    0.422618f,
    0.438371f,
    0.453990f,
    0.469472f,
    0.484810f,
    0.5f,
    0.51503801f,
    0.52991903f,
    0.54463899f,
    0.55919302f,
    0.57357597f,
    0.58778501f,
    0.60181499f,
    0.61566103f,
    0.62932003f,
    0.64278799f,
    0.65605903f,
    0.66913098f,
    0.68199801f,
    0.69465798f,
    0.70710701f,
    0.71934003f,
    0.731354f,
    0.74314499f,
    0.75471002f,
    0.76604402f,
    0.77714598f,
    0.78801101f,
    0.79863602f,
    0.809017f,
    0.819152f,
    0.82903802f,
    0.83867103f,
    0.84804797f,
    0.85716701f,
    0.86602497f,
    0.87462002f,
    0.88294798f,
    0.89100701f,
    0.898794f,
    0.906308f,
    0.91354501f,
    0.92050499f,
    0.92718399f,
    0.93357998f,
    0.93969297f,
    0.94551897f,
    0.95105702f,
    0.95630503f,
    0.96126199f,
    0.96592599f,
    0.97029603f,
    0.97437f,
    0.97814798f,
    0.98162699f,
    0.98480803f,
    0.987688f,
    0.99026799f,
    0.99254602f,
    0.99452198f,
    0.99619502f,
    0.99756402f,
    0.99862999f,
    0.99939102f,
    0.99984801f,
    1.0f,
};

/* Retail's folded end-of-table constants for the 271..360 branch point just
 * past the sine data, so keep the engine's funk-name table adjacent to it. */
// GLOBAL: CARMA2_HW 0x00655b50
const char* gFunk_type_names[5] = {
    "spin",
    "rock",
    "throb",
    "slither",
    "roll",
};

// GLOBAL: CARMA2_HW 0x006ab980
br_matrix34 mattmp1__trig;

// GLOBAL: CARMA2_HW 0x006ab9b0
br_matrix34 mattmp2__trig;

// GLOBAL: CARMA2_HW 0x006ab950
br_matrix23 mat23tmp1;

// GLOBAL: CARMA2_HW 0x006ab968
br_matrix23 mat23tmp2;

#ifdef CARPOCALYPSE2_MATCHING
extern void __cdecl _ftol();
extern char gDepth_effect_names[];
extern char gFloat_0058935c[];
extern char gNegative_half[];
extern char gVector3_00661678[];
#endif
// FUNCTION: CARMA2_HW 0x005113d0
float C2_HOOK_FASTCALL FastFloatSin(int pAngle_in_degrees) {

    if (pAngle_in_degrees < 0) {
        pAngle_in_degrees = 360 - -pAngle_in_degrees % 360;
    } else {
        pAngle_in_degrees = pAngle_in_degrees % 360;
    }
    if (pAngle_in_degrees > 270) {
        return -gFloat_sine_table[360 - pAngle_in_degrees];
    }
    if (pAngle_in_degrees > 180) {
        return -gFloat_sine_table[pAngle_in_degrees - 180];
    }
    if (pAngle_in_degrees > 90) {
        return *(gFloat_sine_table + 180 - pAngle_in_degrees);
    }
    return gFloat_sine_table[pAngle_in_degrees];
}

// FUNCTION: CARMA2_HW 0x00511440
float C2_HOOK_FASTCALL FastFloatCos(int pAngle_in_degrees) {

    if (pAngle_in_degrees < 0) {
        pAngle_in_degrees = 360 - -pAngle_in_degrees % 360;
    } else {
        pAngle_in_degrees = pAngle_in_degrees % 360;
    }

    if (pAngle_in_degrees > 270) {
        return gFloat_sine_table[pAngle_in_degrees - 270];
    }
    if (pAngle_in_degrees > 180) {
        return -gFloat_sine_table[270 - pAngle_in_degrees];
    }
    if (pAngle_in_degrees > 90) {
        return -gFloat_sine_table[pAngle_in_degrees - 90];
    }
    return *(gFloat_sine_table + 90 - pAngle_in_degrees);
}

// FUNCTION: CARMA2_HW 0x005114b0
float C2_HOOK_FASTCALL FastFloatTan(int pAngle_in_degrees) {

    return FastFloatSin(pAngle_in_degrees) / FastFloatCos(pAngle_in_degrees);
}

// FUNCTION: CARMA2_HW 0x005115b0
br_scalar C2_HOOK_FASTCALL FastScalarSin(int pAngle_in_degrees) {

    return FastFloatSin((int)pAngle_in_degrees);
}

// FUNCTION: CARMA2_HW 0x00511620
br_scalar C2_HOOK_FASTCALL FastScalarCos(int pAngle_in_degrees) {

    return FastFloatCos(pAngle_in_degrees);
}

// FUNCTION: CARMA2_HW 0x00511690
br_scalar C2_NAKED C2_HOOK_FASTCALL FastScalarTan(int pAngle_in_degrees) {
#ifndef CARPOCALYPSE2_MATCHING

    return FastScalarSin(pAngle_in_degrees) / FastScalarCos(pAngle_in_degrees);
#else
    __asm {
            test ecx, ecx
            push esi
            mov eax, ecx
            jge L005116a7
            neg eax
            cdq
            mov esi, 0x168
            idiv esi
            mov eax, esi
            sub eax, edx
            jmp L005116b1
    L005116a7:
            cdq
            mov esi, 0x168
            idiv esi
            mov eax, edx
    L005116b1:
            cmp eax, 0x10e
            jle L005116c8
            shl eax, 2
            mov edx, offset gVector3_00661678+0x2c0
            sub edx, eax
            _emit 0xd9
        _emit 0x02
            fchs
            jmp L005116f4
    L005116c8:
            cmp eax, 0xb4
            jle L005116da
            fld dword ptr [eax*4 + gDepth_effect_names+0x238]
            fchs
            jmp L005116f4
    L005116da:
            cmp eax, 0x5a
            jle L005116ed
            shl eax, 2
            mov edx, offset gFloat_sine_table+0x2d0
            sub edx, eax
            _emit 0xd9
        _emit 0x02
            jmp L005116f4
    L005116ed:
            fld dword ptr [eax*4 + gFloat_sine_table]
    L005116f4:
            test ecx, ecx
            mov eax, ecx
            jge L0051170a
            neg eax
            cdq
            mov ecx, 0x168
            idiv ecx
            mov eax, ecx
            sub eax, edx
            jmp L00511714
    L0051170a:
            cdq
            mov ecx, 0x168
            idiv ecx
            mov eax, edx
    L00511714:
            cmp eax, 0x10e
            jle L0051172a
            fld dword ptr [eax*4 + gDepth_effect_names+0xd0]
            fld st(0)
            fdivp st(2), st(0)
            fstp st(0)
            pop esi
            ret
    L0051172a:
            cmp eax, 0xb4
            jle L0051174b
            lea edx, [eax*4]
            mov eax, offset gVector3_00661678+0x158
            sub eax, edx
            _emit 0xd9
        _emit 0x00
            fchs
            fld st(0)
            fdivp st(2), st(0)
            fstp st(0)
            pop esi
            ret
    L0051174b:
            cmp eax, 0x5a
            jle L00511761
            fld dword ptr [eax*4 + gDepth_effect_names+0x3a0]
            fchs
            fld st(0)
            fdivp st(2), st(0)
            fstp st(0)
            pop esi
            ret
    L00511761:
            lea ecx, [eax*4]
            mov edx, offset gFloat_sine_table+0x168
            sub edx, ecx
            pop esi
            _emit 0xd9
        _emit 0x02
            fld st(0)
            fdivp st(2), st(0)
            fstp st(0)
            ret
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

// FUNCTION: CARMA2_HW 0x00511780
br_scalar C2_HOOK_FASTCALL FastScalarSinAngle(br_angle pBR_angle) {

    return FastScalarSin((int)((double)pBR_angle * (360.0 / 65536.0)));
}

// FUNCTION: CARMA2_HW 0x00511810
br_scalar C2_HOOK_FASTCALL FastScalarCosAngle(br_angle pBR_angle) {

    return FastScalarCos((int)((double)pBR_angle * (360.0 / 65536.0)));
}

// FUNCTION: CARMA2_HW 0x005118a0
br_scalar C2_NAKED C2_HOOK_FASTCALL FastScalarTanAngle(br_angle pBR_angle) {
#ifndef CARPOCALYPSE2_MATCHING
    int angle_in_degrees;

    angle_in_degrees = (int)BrAngleToDegrees(pBR_angle);
    return FastScalarSin(angle_in_degrees) / FastScalarCos(angle_in_degrees);
#else
    __asm {
            push ecx
            mov word ptr [esp], cx
            push esi
            mov eax, dword ptr [esp + 4]
            and eax, 0xffff
            mov dword ptr [esp + 4], eax
            fild dword ptr [esp + 4]
            fmul qword ptr [gNegative_half+0x1c]
            call _ftol
            mov ecx, eax
            test ecx, ecx
            jge L005118d9
            neg eax
            cdq
            mov esi, 0x168
            idiv esi
            mov eax, esi
            sub eax, edx
            jmp L005118e3
    L005118d9:
            cdq
            mov esi, 0x168
            idiv esi
            mov eax, edx
    L005118e3:
            cmp eax, 0x10e
            jle L005118fe
            lea edx, [eax*4]
            mov eax, offset gVector3_00661678+0x2c0
            sub eax, edx
            _emit 0xd9
        _emit 0x00
            fchs
            jmp L0051192e
    L005118fe:
            cmp eax, 0xb4
            jle L00511910
            fld dword ptr [eax*4 + gDepth_effect_names+0x238]
            fchs
            jmp L0051192e
    L00511910:
            cmp eax, 0x5a
            jle L00511927
            lea edx, [eax*4]
            mov eax, offset gFloat_sine_table+0x2d0
            sub eax, edx
            _emit 0xd9
        _emit 0x00
            jmp L0051192e
    L00511927:
            fld dword ptr [eax*4 + gFloat_sine_table]
    L0051192e:
            test ecx, ecx
            mov eax, ecx
            jge L00511944
            neg eax
            cdq
            mov ecx, 0x168
            idiv ecx
            mov eax, ecx
            sub eax, edx
            jmp L0051194e
    L00511944:
            cdq
            mov ecx, 0x168
            idiv ecx
            mov eax, edx
    L0051194e:
            cmp eax, 0x10e
            jle L00511965
            fld dword ptr [eax*4 + gDepth_effect_names+0xd0]
            fld st(0)
            fdivp st(2), st(0)
            fstp st(0)
            pop esi
            pop ecx
            ret
    L00511965:
            cmp eax, 0xb4
            jle L00511987
            lea edx, [eax*4]
            mov eax, offset gVector3_00661678+0x158
            sub eax, edx
            _emit 0xd9
        _emit 0x00
            fchs
            fld st(0)
            fdivp st(2), st(0)
            fstp st(0)
            pop esi
            pop ecx
            ret
    L00511987:
            cmp eax, 0x5a
            jle L0051199e
            fld dword ptr [eax*4 + gDepth_effect_names+0x3a0]
            fchs
            fld st(0)
            fdivp st(2), st(0)
            fstp st(0)
            pop esi
            pop ecx
            ret
    L0051199e:
            lea ecx, [eax*4]
            mov edx, offset gFloat_sine_table+0x168
            sub edx, ecx
            pop esi
            _emit 0xd9
        _emit 0x02
            fld st(0)
            fdivp st(2), st(0)
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
    }
#endif
}

// FUNCTION: CARMA2_HW 0x005119c0
float C2_HOOK_STDCALL FastFloatArcSin(float pValue) {
    float low_limit;
    float high_limit;
    float mid_point;

    if (pValue < 0.f) {
        return -FastFloatArcSin(-pValue);
    }
    high_limit = 90.f * pValue;
    low_limit = high_limit - 19.f;
    if (low_limit < 0.f) {
        low_limit = 0.f;
    }
    while (high_limit - low_limit >= 1.f) {
        mid_point = (low_limit + high_limit) / 2.f;

        if (gFloat_sine_table[(int)mid_point] > pValue) {
            high_limit = mid_point;
        } else {
            low_limit = mid_point;
        }
    }
    return low_limit;
}

// FUNCTION: CARMA2_HW 0x00511a80
float C2_HOOK_STDCALL FastFloatArcCos(float pValue) {

    return 90.f - FastFloatArcSin(pValue);
}

// FUNCTION: CARMA2_HW 0x00511b50
br_scalar C2_HOOK_STDCALL FastScalarArcSin(br_scalar pValue) {

    return FastFloatArcSin(pValue);
}

// FUNCTION: CARMA2_HW 0x00511c10
br_scalar C2_HOOK_STDCALL FastScalarArcCos(br_scalar pValue) {

    return 90.f - FastScalarArcSin(pValue);
}

// FUNCTION: CARMA2_HW 0x00511ce0
float C2_HOOK_STDCALL FastFloatArcTan2(float pY, float pX) {
    float abs_x;
    float abs_y;

    abs_x = (float)fabs(pX);
    abs_y = (float)fabs(pY);
    if (pX == 0.f) {
        if (pY < 0.f) {
            return 270.f;
        }
        if (pY > 0.f) {
            return 90.f;
        }
        return 0.f;
    }
    if (pX < 0.f) {
        if (pY < 0.f) {
            if (abs_x < abs_y) {
                return (float)((6.f - abs_x / abs_y) * 45.0);
            }
            return (float)((abs_y / abs_x + 4.f) * 45.0);
        }
        if (abs_x < abs_y) {
            return (float)((abs_x / abs_y + 2.f) * 45.0);
        }
        return (float)((4.f - abs_y / abs_x) * 45.0);
    }
    if (pY < 0.f) {
        if (abs_x < abs_y) {
            return (float)((abs_x / abs_y + 6.f) * 45.0);
        }
        return (float)((8.f - abs_y / abs_x) * 45.0);
    }
    if (abs_x < abs_y) {
        return (float)((2.f - abs_x / abs_y) * 45.0);
    }
    return (float)(abs_y / abs_x * 45.0);
}

// FUNCTION: CARMA2_HW 0x00511e50
br_scalar C2_HOOK_STDCALL FastScalarArcTan2(br_scalar pY, br_scalar pX) {

    return FastFloatArcTan2(pY, pX);
}

// FUNCTION: CARMA2_HW 0x00511e70
br_angle C2_HOOK_STDCALL FastFloatArcTan2Angle(float pY, float pX) {
    float abs_x;
    float abs_y;

    abs_x = (float)fabs(pX);
    abs_y = (float)fabs(pY);
    if (pX == 0.f) {
        if (pY < 0.f) {
            return BR_ANGLE_DEG(270);
        }
        if (pY > 0.f) {
            return BR_ANGLE_DEG(90);
        }
        return 0;
    }
    if (pX < 0.f) {
        if (pY < 0.f) {
            if (abs_x < abs_y) {
                return (br_angle)((6.f - abs_x / abs_y) * 8192.f);
            }
            return (br_angle)((abs_y / abs_x + 4.f) * 8192.f);
        }
        if (abs_x < abs_y) {
            return (br_angle)((abs_x / abs_y + 2.f) * 8192.f);
        }
        return (br_angle)((4.f - abs_y / abs_x) * 8192.f);
    }
    if (pY < 0.f) {
        if (abs_x < abs_y) {
            return (br_angle)((abs_x / abs_y + 6.f) * 8192.f);
        }
        return (br_angle)((8.f - abs_y / abs_x) * 8192.f);
    }
    if (abs_x < abs_y) {
        return (br_angle)((2.f - abs_x / abs_y) * 8192.f);
    }
    return (br_angle)(abs_y / abs_x * 8192.f);
}

// FUNCTION: CARMA2_HW 0x00512010
br_angle C2_HOOK_STDCALL FastScalarArcTan2Angle(br_scalar pY, br_scalar pX) {

    return FastFloatArcTan2Angle(pY, pX);
}

// FUNCTION: CARMA2_HW 0x00512030
void C2_HOOK_FASTCALL DRMatrix34RotateX(br_matrix34* mat, br_angle rx) {
    br_scalar s;
    br_scalar c;

    s = FastScalarSinAngle(rx);
    c = FastScalarCosAngle(rx);
    mat->m[0][0] = 1.0f;
    mat->m[0][1] = 0.0f;
    mat->m[0][2] = 0.0f;
    mat->m[1][0] = 0.0f;
    mat->m[1][1] = c;
    mat->m[1][2] = s;
    mat->m[2][0] = 0.0;
    mat->m[2][1] = -s;
    mat->m[2][2] = c;
    mat->m[3][0] = 0.0f;
    mat->m[3][1] = 0.0f;
    mat->m[3][2] = 0.0f;
}

// FUNCTION: CARMA2_HW 0x00512170
void C2_HOOK_FASTCALL DRMatrix34RotateY(br_matrix34* mat, br_angle ry) {
    br_scalar s;
    br_scalar c;

    s = FastScalarSinAngle(ry);
    c = FastScalarCosAngle(ry);
    mat->m[0][0] = c;
    mat->m[0][1] = 0.0f;
    mat->m[0][2] = -s;
    mat->m[1][0] = 0.0f;
    mat->m[1][1] = 1.0f;
    mat->m[1][2] = 0.0f;
    mat->m[2][0] = s;
    mat->m[2][1] = 0.0f;
    mat->m[2][2] = c;
    mat->m[3][0] = 0.0f;
    mat->m[3][1] = 0.0f;
    mat->m[3][2] = 0.0f;
}

// FUNCTION: CARMA2_HW 0x005122b0
void C2_HOOK_FASTCALL DRMatrix34RotateZ(br_matrix34* mat, br_angle rz) {
    br_scalar s;
    br_scalar c;

    s = FastScalarSinAngle(rz);
    c = FastScalarCosAngle(rz);
    mat->m[0][0] = c;
    mat->m[0][1] = s;
    mat->m[0][2] = 0.0f;
    mat->m[1][0] = -s;
    mat->m[1][1] = c;
    mat->m[1][2] = 0.0f;
    mat->m[2][0] = 0.0f;
    mat->m[2][1] = 0.0f;
    mat->m[2][2] = 1.0f;
    mat->m[3][0] = 0.0f;
    mat->m[3][1] = 0.0f;
    mat->m[3][2] = 0.0f;
}

// FUNCTION: CARMA2_HW 0x005123f0
void C2_NAKED C2_HOOK_FASTCALL DRMatrix34Rotate(br_matrix34* mat, br_angle r, br_vector3* a) {
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

    s = FastScalarSinAngle(r);
    c = FastScalarCosAngle(r);
    t = 1.f - c;
    txy = t * a->v[0] * a->v[1];
    txz = t * a->v[0] * a->v[2];
    tyz = t * a->v[1] * a->v[2];
    sx = s * a->v[0];
    sy = s * a->v[1];
    sz = s * a->v[2];

    mat->m[0][0] = a->v[0] * a->v[0] * t + c;
    mat->m[0][1] = sx + txy;
    mat->m[0][2] = txz - sy;
    mat->m[1][0] = txy - sz;
    mat->m[1][1] = a->v[1] * a->v[1] * t + c;
    mat->m[1][2] = sx + tyz;
    mat->m[2][0] = sy + txz;
    mat->m[2][1] = tyz - sx;
    mat->m[2][2] = a->v[2] * a->v[2] * t + c;
    mat->m[3][2] = 0.f;
    mat->m[3][1] = 0.f;
    mat->m[3][0] = 0.f;
#else
    __asm {
            sub esp, 0x14
            mov word ptr [esp + 0x10], dx
            push ebx
            mov eax, dword ptr [esp + 0x14]
            push esi
            and eax, 0xffff
            mov esi, ecx
            mov dword ptr [esp + 0x18], eax
            fild dword ptr [esp + 0x18]
            fmul qword ptr [gNegative_half+0x1c]
            call _ftol
            mov ecx, eax
            test ecx, ecx
            jge L0051242e
            neg eax
            cdq
            mov ebx, 0x168
            idiv ebx
            mov eax, ebx
            sub eax, edx
            jmp L00512438
    L0051242e:
            cdq
            mov ebx, 0x168
            idiv ebx
            mov eax, edx
    L00512438:
            cmp eax, 0x10e
            jle L00512453
            lea edx, [eax*4]
            mov eax, offset gVector3_00661678+0x2c0
            sub eax, edx
            _emit 0xd9
        _emit 0x00
            fchs
            jmp L00512483
    L00512453:
            cmp eax, 0xb4
            jle L00512465
            fld dword ptr [eax*4 + gDepth_effect_names+0x238]
            fchs
            jmp L00512483
    L00512465:
            cmp eax, 0x5a
            jle L0051247c
            lea edx, [eax*4]
            mov eax, offset gFloat_sine_table+0x2d0
            sub eax, edx
            _emit 0xd9
        _emit 0x00
            jmp L00512483
    L0051247c:
            fld dword ptr [eax*4 + gFloat_sine_table]
    L00512483:
            fstp dword ptr [esp + 0x10]
            test ecx, ecx
            mov eax, ecx
            jge L0051249d
            neg eax
            cdq
            mov ecx, 0x168
            idiv ecx
            mov eax, ecx
            sub eax, edx
            jmp L005124a7
    L0051249d:
            cdq
            mov ecx, 0x168
            idiv ecx
            mov eax, edx
    L005124a7:
            cmp eax, 0x10e
            jle L005124b7
            fld dword ptr [eax*4 + gDepth_effect_names+0xd0]
            jmp L005124f2
    L005124b7:
            cmp eax, 0xb4
            jle L005124d2
            lea edx, [eax*4]
            mov eax, offset gVector3_00661678+0x158
            sub eax, edx
            _emit 0xd9
        _emit 0x00
            fchs
            jmp L005124f2
    L005124d2:
            cmp eax, 0x5a
            jle L005124e2
            fld dword ptr [eax*4 + gDepth_effect_names+0x3a0]
            fchs
            jmp L005124f2
    L005124e2:
            lea ecx, [eax*4]
            mov edx, offset gFloat_sine_table+0x168
            sub edx, ecx
            _emit 0xd9
        _emit 0x02
    L005124f2:
            fld dword ptr [gNegative_half+0x30]
            fxch st(1)
            fstp dword ptr [esp + 0xc]
            mov eax, dword ptr [esp + 0x20]
            fsub dword ptr [esp + 0xc]
            _emit 0xd9
        _emit 0x00
            fld dword ptr [eax + 8]
            fxch st(2)
            fst dword ptr [esp + 8]
            _emit 0xd8
        _emit 0x08
            fld dword ptr [eax + 4]
            fld dword ptr [eax + 8]
            fxch st(3)
            fst dword ptr [esp + 0x20]
            fmul dword ptr [esp + 0x20]
            fld dword ptr [eax + 4]
            fxch st(3)
            fstp dword ptr [esp + 0x20]
            fxch st(4)
            fmul dword ptr [esp + 0x20]
            fxch st(1)
            fmul dword ptr [esp + 0x20]
            fxch st(3)
            fmul dword ptr [esp + 0x10]
            fxch st(2)
            fmul dword ptr [esp + 0x10]
            fxch st(4)
            fmul dword ptr [esp + 8]
            fxch st(1)
            fst dword ptr [esp + 0x18]
            fxch st(3)
            fst dword ptr [esp + 0x20]
            fxch st(2)
            fst dword ptr [esp + 0x14]
            fadd dword ptr [esp + 0x20]
            fxch st(2)
            fsub dword ptr [esp + 0x14]
            fxch st(4)
            fstp dword ptr [esp + 0x14]
            fxch st(2)
            fsub dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x10]
            fxch st(3)
            fadd dword ptr [esp + 0xc]
            fld dword ptr [eax + 8]
            fmul dword ptr [eax + 4]
            fmul dword ptr [esp + 8]
            fstp dword ptr [esp + 0x10]
            fxch st(3)
            _emit 0xd8
        _emit 0x08
            fxch st(3)
            _emit 0xd9
        _emit 0x1e
            fxch st(1)
            fstp dword ptr [esi + 4]
            fstp dword ptr [esi + 8]
            fxch st(1)
            fstp dword ptr [esi + 0xc]
            fld dword ptr [eax + 4]
            fst dword ptr [esp + 0x20]
            fmul dword ptr [esp + 0x20]
            fld dword ptr [esp + 0x14]
            fxch st(1)
            fmul dword ptr [esp + 8]
            fxch st(2)
            fst dword ptr [esp + 0x20]
            fadd dword ptr [esp + 0x10]
            fxch st(1)
            fadd dword ptr [esp + 0x18]
            fxch st(2)
            fadd dword ptr [esp + 0xc]
            fxch st(1)
            fstp dword ptr [esi + 0x14]
            fxch st(1)
            fstp dword ptr [esi + 0x18]
            fstp dword ptr [esi + 0x10]
            fld dword ptr [esp + 0x10]
            fsub dword ptr [esp + 0x20]
            fstp dword ptr [esi + 0x1c]
            fld dword ptr [eax + 8]
            fld st(0)
            fmul st(0), st(1)
            mov dword ptr [esi + 0x2c], 0
            mov dword ptr [esi + 0x28], 0
            mov dword ptr [esi + 0x24], 0
            fmul dword ptr [esp + 8]
            fadd dword ptr [esp + 0xc]
            fstp dword ptr [esi + 0x20]
            pop esi
            pop ebx
            fstp st(0)
            add esp, 0x14
            ret 4
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

// FUNCTION: CARMA2_HW 0x00512620
void C2_HOOK_FASTCALL DRMatrix34PreRotateX(br_matrix34* mat, br_angle rx) {

    DRMatrix34RotateX(&mattmp2__trig, rx);
    BrMatrix34Mul(&mattmp1__trig, &mattmp2__trig, mat);
    BrMatrix34Copy(mat, &mattmp1__trig);
}

// FUNCTION: CARMA2_HW 0x005127b0
void C2_HOOK_FASTCALL DRMatrix34PostRotateX(br_matrix34* mat, br_angle rx) {

    DRMatrix34RotateX(&mattmp2__trig, rx);
    BrMatrix34Mul(&mattmp1__trig, mat, &mattmp2__trig);
    BrMatrix34Copy(mat, &mattmp1__trig);
}

// FUNCTION: CARMA2_HW 0x00512940
void C2_HOOK_FASTCALL DRMatrix34PreRotateY(br_matrix34* mat, br_angle ry) {

    DRMatrix34RotateY(&mattmp2__trig, ry);
    BrMatrix34Mul(&mattmp1__trig, &mattmp2__trig, mat);
    BrMatrix34Copy(mat, &mattmp1__trig);
}

// FUNCTION: CARMA2_HW 0x00512ad0
void C2_HOOK_FASTCALL DRMatrix34PostRotateY(br_matrix34* mat, br_angle ry) {

    DRMatrix34RotateY(&mattmp2__trig, ry);
    BrMatrix34Mul(&mattmp1__trig, mat, &mattmp2__trig);
    BrMatrix34Copy(mat, &mattmp1__trig);
}

// FUNCTION: CARMA2_HW 0x00512c60
void C2_HOOK_FASTCALL DRMatrix34PreRotateZ(br_matrix34* mat, br_angle rz) {

    DRMatrix34RotateZ(&mattmp2__trig, rz);
    BrMatrix34Mul(&mattmp1__trig, &mattmp2__trig, mat);
    BrMatrix34Copy(mat, &mattmp1__trig);
}

// FUNCTION: CARMA2_HW 0x00512df0
void C2_HOOK_FASTCALL DRMatrix34PostRotateZ(br_matrix34* mat, br_angle rz) {

    DRMatrix34RotateZ(&mattmp2__trig, rz);
    BrMatrix34Mul(&mattmp1__trig, mat, &mattmp2__trig);
    BrMatrix34Copy(mat, &mattmp1__trig);
}

// FUNCTION: CARMA2_HW 0x00512f80
void C2_HOOK_FASTCALL DRMatrix34PreRotate(br_matrix34* mat, br_angle r, br_vector3* axis) {

    DRMatrix34Rotate(&mattmp2__trig, r, axis);
    BrMatrix34Mul(&mattmp1__trig, &mattmp2__trig, mat);
    BrMatrix34Copy(mat, &mattmp1__trig);
}

// FUNCTION: CARMA2_HW 0x00512fd0
void C2_HOOK_FASTCALL DRMatrix34PostRotate(br_matrix34* mat, br_angle r, br_vector3* axis) {

    DRMatrix34Rotate(&mattmp2__trig, r, axis);
    BrMatrix34Mul(&mattmp1__trig, mat, &mattmp2__trig);
    BrMatrix34Copy(mat, &mattmp1__trig);
}

// FUNCTION: CARMA2_HW 0x00513020
void C2_HOOK_FASTCALL DRMatrix23Rotate(br_matrix23* mat, br_angle rz) {
    br_scalar s;
    br_scalar c;

    s = FastScalarSinAngle(rz);
    c = FastScalarCosAngle(rz);

    mat->m[0][0] = c;
    mat->m[0][1] = s;
    mat->m[1][0] = -s;
    mat->m[1][1] = c;
    mat->m[2][0] = 0.f;
    mat->m[2][1] = 0.f;
}

// FUNCTION: CARMA2_HW 0x00513140
void C2_HOOK_FASTCALL DRMatrix23PreRotate(br_matrix23* mat, br_angle rz) {

    DRMatrix23Rotate(&mat23tmp2, rz);
    BrMatrix23Mul(&mat23tmp1, &mat23tmp2, mat);
    BrMatrix23Copy(mat, &mat23tmp1);
}

// FUNCTION: CARMA2_HW 0x005132a0
void C2_HOOK_FASTCALL DRMatrix23PostRotate(br_matrix23* mat, br_angle rz) {

    DRMatrix23Rotate(&mat23tmp2, rz);
    BrMatrix23Mul(&mat23tmp1, mat, &mat23tmp2);
    BrMatrix23Copy(mat, &mat23tmp1);
}

// FUNCTION: CARMA2_HW 0x0040aed0
void C2_NAKED C2_HOOK_FASTCALL DRMatrix34RotateCos(br_matrix34* pDest, const br_vector3* pAxis, float pCosA) {
#ifndef CARPOCALYPSE2_MATCHING
    float sinA;
    float mc;
    float mcx;
    float mcxz;
    float mcxy;
    float sz;
    float sy;
    float sx;
    float mcyz;

    if (pCosA < -1.f) {
        pCosA = -1.f;
    } else if (pCosA > 1.f) {
        pCosA = 1.f;
    }
    sinA = sqrtf(1.f - pCosA * pCosA);
    mc = 1.f - pCosA;
    mcx = pAxis->v[0] * mc;
    mcxy = pAxis->v[1] * mcx;
    mcxz = pAxis->v[2] * mcx;
    mcyz = pAxis->v[2] * pAxis->v[1] * mc;
    sx = pAxis->v[0] * sinA;
    sy = pAxis->v[1] * sinA;
    sz = pAxis->v[2] * sinA;
    pDest->m[0][0] = pAxis->v[0] * pAxis->v[0] * mc + pCosA;
    pDest->m[0][1] = sz + mcxy;
    pDest->m[0][2] = mcxz - sy;
    pDest->m[1][0] = mcxy - sz;
    pDest->m[1][1] = pAxis->v[1] * pAxis->v[1] * mc + pCosA;
    pDest->m[1][2] = sx + mcyz;
    pDest->m[2][0] = sy + mcxz;
    pDest->m[2][1] = mcyz - sx;
    pDest->m[2][2] = pAxis->v[2] * pAxis->v[2] * mc + pCosA;
    pDest->m[3][0] = 0.f;
    pDest->m[3][1] = 0.f;
    pDest->m[3][2] = 0.f;
#else
    __asm {
            fld dword ptr [esp + 4]
            fcomp dword ptr [gFloat_0058935c+0x8]
            sub esp, 0x10
            fnstsw ax
            test ah, 1
            je L0040aeee
            mov dword ptr [esp + 0x14], 0xbf800000
            jmp L0040af07
    L0040aeee:
            fld dword ptr [esp + 0x14]
            fcomp dword ptr [gFloat_0058935c]
            fnstsw ax
            test ah, 0x41
            jne L0040af07
            mov dword ptr [esp + 0x14], 0x3f800000
    L0040af07:
            fld dword ptr [esp + 0x14]
            fmul dword ptr [esp + 0x14]
            fld dword ptr [gFloat_0058935c]
            fsub dword ptr [esp + 0x14]
            fxch st(1)
            fsubr dword ptr [gFloat_0058935c]
            _emit 0xd9
        _emit 0x02
            _emit 0xd9
        _emit 0x02
            fxch st(2)
            fsqrt
            fxch st(1)
            fmul st(0), st(3)
            fxch st(1)
            fst dword ptr [esp + 0xc]
            fld dword ptr [edx + 8]
            fld dword ptr [edx + 4]
            fld dword ptr [edx + 8]
            fxch st(3)
            fstp dword ptr [esp + 4]
            fxch st(4)
            _emit 0xd9
        _emit 0x54
        _emit 0x24
        _emit 0x00
            _emit 0xd8
        _emit 0x4c
        _emit 0x24
        _emit 0x00
            fld dword ptr [edx + 4]
            fxch st(4)
            _emit 0xd9
        _emit 0x5c
        _emit 0x24
        _emit 0x00
            fxch st(1)
            _emit 0xd8
        _emit 0x4c
        _emit 0x24
        _emit 0x00
            fxch st(4)
            _emit 0xd8
        _emit 0x4c
        _emit 0x24
        _emit 0x00
            fxch st(2)
            fmul dword ptr [esp + 4]
            fxch st(3)
            fmul dword ptr [esp + 4]
            fxch st(1)
            fmul st(0), st(5)
            fld st(4)
            fxch st(3)
            _emit 0xd9
        _emit 0x54
        _emit 0x24
        _emit 0x00
            fxch st(4)
            fst dword ptr [esp + 8]
            _emit 0xd8
        _emit 0x44
        _emit 0x24
        _emit 0x00
            fxch st(4)
            fsub dword ptr [esp + 8]
            fxch st(2)
            _emit 0xd9
        _emit 0x5c
        _emit 0x24
        _emit 0x00
            fxch st(2)
            _emit 0xd8
        _emit 0x64
        _emit 0x24
        _emit 0x00
            fxch st(2)
            fadd dword ptr [esp + 0x14]
            fld dword ptr [edx + 8]
            fmul dword ptr [edx + 4]
            _emit 0xd9
        _emit 0x02
            fxch st(1)
            fmul st(0), st(7)
            fstp dword ptr [esp + 0xc]
            fxch st(1)
            _emit 0xd9
        _emit 0x19
            fxch st(3)
            fstp dword ptr [ecx + 4]
            fxch st(1)
            fstp dword ptr [ecx + 8]
            fstp dword ptr [ecx + 0xc]
            fld dword ptr [edx + 4]
            fst dword ptr [esp + 8]
            fmul dword ptr [esp + 8]
            fxch st(1)
            fmul dword ptr [esp + 4]
            fxch st(1)
            fmul st(0), st(3)
            fxch st(1)
            fstp dword ptr [esp + 8]
            xor eax, eax
            fadd dword ptr [esp + 0x14]
            fstp dword ptr [ecx + 0x10]
            fld dword ptr [esp + 8]
            _emit 0xd9
        _emit 0x44
        _emit 0x24
        _emit 0x00
            fld dword ptr [esp + 0xc]
            fxch st(2)
            fadd dword ptr [esp + 0xc]
            fxch st(1)
            fadd st(0), st(3)
            fxch st(2)
            fsub dword ptr [esp + 8]
            fxch st(1)
            fstp dword ptr [ecx + 0x14]
            fxch st(1)
            fstp dword ptr [ecx + 0x18]
            fstp dword ptr [ecx + 0x1c]
            fstp st(0)
            fld dword ptr [edx + 8]
            fld st(0)
            fmul st(0), st(1)
            mov dword ptr [ecx + 0x2c], eax
            mov dword ptr [ecx + 0x28], eax
            mov dword ptr [ecx + 0x24], eax
            fmul st(0), st(2)
            fadd dword ptr [esp + 0x14]
            fstp dword ptr [ecx + 0x20]
            fstp st(0)
            fstp st(0)
            add esp, 0x10
            ret 4
            nop
            nop
    }
#endif
}