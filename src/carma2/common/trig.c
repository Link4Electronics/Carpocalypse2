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

/* Naming-only stand-in for the folded cos-table end constant (retail
 * 0x6617d0).  It is only referenced from the hand-written asm bodies, so
 * naming it keeps both diff sides comparable. */
// GLOBAL: CARMA2_HW 0x006617d0
float gTrig_006617d0;
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_CDECL BrMatrix23Copy();
extern void C2_HOOK_CDECL BrMatrix23Mul();
extern void C2_HOOK_CDECL BrMatrix34Copy();
extern void C2_HOOK_CDECL BrMatrix34Mul();
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern char gData_00660f60[];
extern char gFloat_00589364[];
extern char gString_00661230[];
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern char gMatchData_00660f60[];
extern char gMatchFloat_00589364[];
extern char gMatchString_00661230[];
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
float C2_NAKED C2_HOOK_FASTCALL FastFloatCos(int pAngle_in_degrees) {
#ifndef CARPOCALYPSE2_MATCHING

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
#else
    __asm {
            mov eax, ecx
            test eax, eax
            jge L00511456
            neg eax
            cdq
            mov ecx, 0x168
            idiv ecx
            mov eax, ecx
            sub eax, edx
            jmp L00511460
    L00511456:
            cdq
            mov ecx, 0x168
            idiv ecx
            mov eax, edx
    L00511460:
            cmp eax, 0x10e
            jle L0051146f
            fld dword ptr [eax*4 + gMatchData_00660f60]
            ret
    L0051146f:
            cmp eax, 0xb4
            jle L00511489
            lea edx, [eax*4]
            mov eax, offset gTrig_006617d0
            sub eax, edx
            _emit 0xd9
        _emit 0x00
            fchs
            ret
    L00511489:
            cmp eax, 0x5a
            jle L00511498
            fld dword ptr [eax*4 + gMatchString_00661230]
            fchs
            ret
    L00511498:
            lea ecx, [eax*4]
            mov edx, offset gFloat_sine_table+0x168
            sub edx, ecx
            _emit 0xd9
        _emit 0x02
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

// FUNCTION: CARMA2_HW 0x005114b0
float C2_NAKED C2_HOOK_FASTCALL FastFloatTan(int pAngle_in_degrees) {
#ifndef CARPOCALYPSE2_MATCHING

    return FastFloatSin(pAngle_in_degrees) / FastFloatCos(pAngle_in_degrees);
#else
    __asm {
            test ecx, ecx
            push esi
            mov eax, ecx
            jge L005114c7
            neg eax
            cdq
            mov esi, 0x168
            idiv esi
            mov eax, esi
            sub eax, edx
            jmp L005114d1
    L005114c7:
            cdq
            mov esi, 0x168
            idiv esi
            mov eax, edx
    L005114d1:
            cmp eax, 0x10e
            jle L005114e8
            shl eax, 2
            mov edx, 0x661938
            sub edx, eax
            _emit 0xd9
        _emit 0x02
            fchs
            jmp L00511514
    L005114e8:
            cmp eax, 0xb4
            jle L005114fa
            fld dword ptr [eax*4 + 0x6610c8]
            fchs
            jmp L00511514
    L005114fa:
            cmp eax, 0x5a
            jle L0051150d
            shl eax, 2
            mov edx, offset gFloat_sine_table+0x2d0
            sub edx, eax
            _emit 0xd9
        _emit 0x02
            jmp L00511514
    L0051150d:
            fld dword ptr [eax*4 + gFloat_sine_table]
    L00511514:
            test ecx, ecx
            mov eax, ecx
            jge L0051152a
            neg eax
            cdq
            mov ecx, 0x168
            idiv ecx
            mov eax, ecx
            sub eax, edx
            jmp L00511534
    L0051152a:
            cdq
            mov ecx, 0x168
            idiv ecx
            mov eax, edx
    L00511534:
            cmp eax, 0x10e
            jle L0051154c
            fld dword ptr [eax*4 + gMatchData_00660f60]
            fxch st(1)
            fdiv st(0), st(1)
            fxch st(1)
            fstp st(0)
            pop esi
            ret
    L0051154c:
            cmp eax, 0xb4
            jle L0051156f
            lea edx, [eax*4]
            mov eax, offset gTrig_006617d0
            sub eax, edx
            _emit 0xd9
        _emit 0x00
            fchs
            fxch st(1)
            fdiv st(0), st(1)
            fxch st(1)
            fstp st(0)
            pop esi
            ret
    L0051156f:
            cmp eax, 0x5a
            jle L00511587
            fld dword ptr [eax*4 + gMatchString_00661230]
            fchs
            fxch st(1)
            fdiv st(0), st(1)
            fxch st(1)
            fstp st(0)
            pop esi
            ret
    L00511587:
            lea ecx, [eax*4]
            mov edx, offset gFloat_sine_table+0x168
            sub edx, ecx
            pop esi
            _emit 0xd9
        _emit 0x02
            fxch st(1)
            fdiv st(0), st(1)
            fxch st(1)
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
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x005115b0
br_scalar C2_HOOK_FASTCALL FastScalarSin(int pAngle_in_degrees) {

    return FastFloatSin((int)pAngle_in_degrees);
}

// FUNCTION: CARMA2_HW 0x00511620
br_scalar C2_NAKED C2_HOOK_FASTCALL FastScalarCos(int pAngle_in_degrees) {
#ifndef CARPOCALYPSE2_MATCHING

    return FastFloatCos(pAngle_in_degrees);
#else
    __asm {
            mov eax, ecx
            test eax, eax
            jge L00511636
            neg eax
            cdq
            mov ecx, 0x168
            idiv ecx
            mov eax, ecx
            sub eax, edx
            jmp L00511640
    L00511636:
            cdq
            mov ecx, 0x168
            idiv ecx
            mov eax, edx
    L00511640:
            cmp eax, 0x10e
            jle L0051164f
            fld dword ptr [eax*4 + gMatchData_00660f60]
            ret
    L0051164f:
            cmp eax, 0xb4
            jle L00511669
            lea edx, [eax*4]
            mov eax, offset gTrig_006617d0
            sub eax, edx
            _emit 0xd9
        _emit 0x00
            fchs
            ret
    L00511669:
            cmp eax, 0x5a
            jle L00511678
            fld dword ptr [eax*4 + gMatchString_00661230]
            fchs
            ret
    L00511678:
            lea ecx, [eax*4]
            mov edx, offset gFloat_sine_table+0x168
            sub edx, ecx
            _emit 0xd9
        _emit 0x02
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
            mov edx, 0x661938
            sub edx, eax
            _emit 0xd9
        _emit 0x02
            fchs
            jmp L005116f4
    L005116c8:
            cmp eax, 0xb4
            jle L005116da
            fld dword ptr [eax*4 + 0x6610c8]
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
            fld dword ptr [eax*4 + gMatchData_00660f60]
            fld st(0)
            fdivp st(2), st(0)
            fstp st(0)
            pop esi
            ret
    L0051172a:
            cmp eax, 0xb4
            jle L0051174b
            lea edx, [eax*4]
            mov eax, offset gTrig_006617d0
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
            fld dword ptr [eax*4 + gMatchString_00661230]
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
br_scalar C2_NAKED C2_HOOK_FASTCALL FastScalarCosAngle(br_angle pBR_angle) {
#ifndef CARPOCALYPSE2_MATCHING

    return FastScalarCos((int)((double)pBR_angle * (360.0 / 65536.0)));
#else
    __asm {
            push ecx
            mov word ptr [esp], cx
            mov eax, dword ptr [esp]
            and eax, 0xffff
            mov dword ptr [esp], eax
            _emit 0xdb
        _emit 0x44
        _emit 0x24
        _emit 0x00
            _emit 0xdc
        _emit 0x0d
        _emit 0x18
        _emit 0xb2
        _emit 0x58
        _emit 0x00
            call _ftol
            test eax, eax
            jge L00511846
            neg eax
            cdq
            mov ecx, 0x168
            idiv ecx
            mov eax, ecx
            sub eax, edx
            jmp L00511850
    L00511846:
            cdq
            mov ecx, 0x168
            idiv ecx
            mov eax, edx
    L00511850:
            cmp eax, 0x10e
            jle L00511860
            fld dword ptr [eax*4 + gMatchData_00660f60]
            pop ecx
            ret
    L00511860:
            cmp eax, 0xb4
            jle L0051187b
            lea edx, [eax*4]
            mov eax, offset gTrig_006617d0
            sub eax, edx
            _emit 0xd9
        _emit 0x00
            fchs
            pop ecx
            ret
    L0051187b:
            cmp eax, 0x5a
            jle L0051188b
            fld dword ptr [eax*4 + gMatchString_00661230]
            fchs
            pop ecx
            ret
    L0051188b:
            lea ecx, [eax*4]
            mov edx, offset gFloat_sine_table+0x168
            sub edx, ecx
            _emit 0xd9
        _emit 0x02
            pop ecx
            ret
            nop
            nop
            nop
    }
#endif
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
            _emit 0xdc
        _emit 0x0d
        _emit 0x18
        _emit 0xb2
        _emit 0x58
        _emit 0x00
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
            mov eax, 0x661938
            sub eax, edx
            _emit 0xd9
        _emit 0x00
            fchs
            jmp L0051192e
    L005118fe:
            cmp eax, 0xb4
            jle L00511910
            fld dword ptr [eax*4 + 0x6610c8]
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
            fld dword ptr [eax*4 + gMatchData_00660f60]
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
            mov eax, offset gTrig_006617d0
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
            fld dword ptr [eax*4 + gMatchString_00661230]
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
void C2_NAKED C2_HOOK_FASTCALL DRMatrix34RotateX(br_matrix34* mat, br_angle rx) {
#ifndef CARPOCALYPSE2_MATCHING
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
#else
    __asm {
            push ecx
            mov word ptr [esp], dx
            push ebx
            mov eax, dword ptr [esp + 4]
            push esi
            and eax, 0xffff
            mov esi, ecx
            mov dword ptr [esp + 8], eax
            push edi
            fild dword ptr [esp + 0xc]
            _emit 0xdc
        _emit 0x0d
        _emit 0x18
        _emit 0xb2
        _emit 0x58
        _emit 0x00
            call _ftol
            mov ecx, eax
            xor edi, edi
            cmp ecx, edi
            jge L0051206f
            neg eax
            cdq
            mov ebx, 0x168
            idiv ebx
            mov eax, ebx
            sub eax, edx
            jmp L00512079
    L0051206f:
            cdq
            mov ebx, 0x168
            idiv ebx
            mov eax, edx
    L00512079:
            cmp eax, 0x10e
            jle L00512094
            lea edx, [eax*4]
            mov eax, 0x661938
            sub eax, edx
            _emit 0xd9
        _emit 0x00
            fchs
            jmp L005120c4
    L00512094:
            cmp eax, 0xb4
            jle L005120a6
            fld dword ptr [eax*4 + 0x6610c8]
            fchs
            jmp L005120c4
    L005120a6:
            cmp eax, 0x5a
            jle L005120bd
            lea edx, [eax*4]
            mov eax, offset gFloat_sine_table+0x2d0
            sub eax, edx
            _emit 0xd9
        _emit 0x00
            jmp L005120c4
    L005120bd:
            fld dword ptr [eax*4 + gFloat_sine_table]
    L005120c4:
            cmp ecx, edi
            mov eax, ecx
            jge L005120da
            neg eax
            cdq
            mov ecx, 0x168
            idiv ecx
            mov eax, ecx
            sub eax, edx
            jmp L005120e4
    L005120da:
            cdq
            mov ecx, 0x168
            idiv ecx
            mov eax, edx
    L005120e4:
            cmp eax, 0x10e
            jle L005120f4
            fld dword ptr [eax*4 + gMatchData_00660f60]
            jmp L0051212f
    L005120f4:
            cmp eax, 0xb4
            jle L0051210f
            lea edx, [eax*4]
            mov eax, offset gTrig_006617d0
            sub eax, edx
            _emit 0xd9
        _emit 0x00
            fchs
            jmp L0051212f
    L0051210f:
            cmp eax, 0x5a
            jle L0051211f
            fld dword ptr [eax*4 + gMatchString_00661230]
            fchs
            jmp L0051212f
    L0051211f:
            lea ecx, [eax*4]
            mov edx, offset gFloat_sine_table+0x168
            sub edx, ecx
            _emit 0xd9
        _emit 0x02
    L0051212f:
            fst dword ptr [esi + 0x10]
            fld st(1)
            mov dword ptr [esi + 4], edi
            mov dword ptr [esi + 8], edi
            fstp dword ptr [esi + 0x14]
            fxch st(1)
            fchs
            fstp dword ptr [esi + 0x1c]
            mov dword ptr [esi + 0xc], edi
            mov dword ptr [esi + 0x18], edi
            fstp dword ptr [esi + 0x20]
            mov dword ptr [esi + 0x24], edi
            mov dword ptr [esi + 0x28], edi
            mov dword ptr [esi + 0x2c], edi
            mov dword ptr [esi], 0x3f800000
            pop edi
            pop esi
            pop ebx
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
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00512170
void C2_NAKED C2_HOOK_FASTCALL DRMatrix34RotateY(br_matrix34* mat, br_angle ry) {
#ifndef CARPOCALYPSE2_MATCHING
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
#else
    __asm {
            push ecx
            mov word ptr [esp], dx
            push ebx
            mov eax, dword ptr [esp + 4]
            push esi
            and eax, 0xffff
            mov esi, ecx
            mov dword ptr [esp + 8], eax
            push edi
            fild dword ptr [esp + 0xc]
            _emit 0xdc
        _emit 0x0d
        _emit 0x18
        _emit 0xb2
        _emit 0x58
        _emit 0x00
            call _ftol
            mov ecx, eax
            xor edi, edi
            cmp ecx, edi
            jge L005121af
            neg eax
            cdq
            mov ebx, 0x168
            idiv ebx
            mov eax, ebx
            sub eax, edx
            jmp L005121b9
    L005121af:
            cdq
            mov ebx, 0x168
            idiv ebx
            mov eax, edx
    L005121b9:
            cmp eax, 0x10e
            jle L005121d4
            lea edx, [eax*4]
            mov eax, 0x661938
            sub eax, edx
            _emit 0xd9
        _emit 0x00
            fchs
            jmp L00512204
    L005121d4:
            cmp eax, 0xb4
            jle L005121e6
            fld dword ptr [eax*4 + 0x6610c8]
            fchs
            jmp L00512204
    L005121e6:
            cmp eax, 0x5a
            jle L005121fd
            lea edx, [eax*4]
            mov eax, offset gFloat_sine_table+0x2d0
            sub eax, edx
            _emit 0xd9
        _emit 0x00
            jmp L00512204
    L005121fd:
            fld dword ptr [eax*4 + gFloat_sine_table]
    L00512204:
            cmp ecx, edi
            mov eax, ecx
            jge L0051221a
            neg eax
            cdq
            mov ecx, 0x168
            idiv ecx
            mov eax, ecx
            sub eax, edx
            jmp L00512224
    L0051221a:
            cdq
            mov ecx, 0x168
            idiv ecx
            mov eax, edx
    L00512224:
            cmp eax, 0x10e
            jle L00512234
            fld dword ptr [eax*4 + gMatchData_00660f60]
            jmp L0051226f
    L00512234:
            cmp eax, 0xb4
            jle L0051224f
            lea edx, [eax*4]
            mov eax, offset gTrig_006617d0
            sub eax, edx
            _emit 0xd9
        _emit 0x00
            fchs
            jmp L0051226f
    L0051224f:
            cmp eax, 0x5a
            jle L0051225f
            fld dword ptr [eax*4 + gMatchString_00661230]
            fchs
            jmp L0051226f
    L0051225f:
            lea ecx, [eax*4]
            mov edx, offset gFloat_sine_table+0x168
            sub edx, ecx
            _emit 0xd9
        _emit 0x02
    L0051226f:
            _emit 0xd9
        _emit 0x16
            fld st(1)
            fchs
            fstp dword ptr [esi + 8]
            fxch st(1)
            fstp dword ptr [esi + 0x18]
            mov dword ptr [esi + 4], edi
            mov dword ptr [esi + 0xc], edi
            fstp dword ptr [esi + 0x20]
            mov dword ptr [esi + 0x14], edi
            mov dword ptr [esi + 0x1c], edi
            mov dword ptr [esi + 0x24], edi
            mov dword ptr [esi + 0x28], edi
            mov dword ptr [esi + 0x2c], edi
            mov dword ptr [esi + 0x10], 0x3f800000
            pop edi
            pop esi
            pop ebx
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
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x005122b0
void C2_NAKED C2_HOOK_FASTCALL DRMatrix34RotateZ(br_matrix34* mat, br_angle rz) {
#ifndef CARPOCALYPSE2_MATCHING
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
#else
    __asm {
            push ecx
            mov word ptr [esp], dx
            push ebx
            mov eax, dword ptr [esp + 4]
            push esi
            and eax, 0xffff
            mov esi, ecx
            mov dword ptr [esp + 8], eax
            push edi
            fild dword ptr [esp + 0xc]
            _emit 0xdc
        _emit 0x0d
        _emit 0x18
        _emit 0xb2
        _emit 0x58
        _emit 0x00
            call _ftol
            mov ecx, eax
            xor edi, edi
            cmp ecx, edi
            jge L005122ef
            neg eax
            cdq
            mov ebx, 0x168
            idiv ebx
            mov eax, ebx
            sub eax, edx
            jmp L005122f9
    L005122ef:
            cdq
            mov ebx, 0x168
            idiv ebx
            mov eax, edx
    L005122f9:
            cmp eax, 0x10e
            jle L00512314
            lea edx, [eax*4]
            mov eax, 0x661938
            sub eax, edx
            _emit 0xd9
        _emit 0x00
            fchs
            jmp L00512344
    L00512314:
            cmp eax, 0xb4
            jle L00512326
            fld dword ptr [eax*4 + 0x6610c8]
            fchs
            jmp L00512344
    L00512326:
            cmp eax, 0x5a
            jle L0051233d
            lea edx, [eax*4]
            mov eax, offset gFloat_sine_table+0x2d0
            sub eax, edx
            _emit 0xd9
        _emit 0x00
            jmp L00512344
    L0051233d:
            fld dword ptr [eax*4 + gFloat_sine_table]
    L00512344:
            cmp ecx, edi
            mov eax, ecx
            jge L0051235a
            neg eax
            cdq
            mov ecx, 0x168
            idiv ecx
            mov eax, ecx
            sub eax, edx
            jmp L00512364
    L0051235a:
            cdq
            mov ecx, 0x168
            idiv ecx
            mov eax, edx
    L00512364:
            cmp eax, 0x10e
            jle L00512374
            fld dword ptr [eax*4 + gMatchData_00660f60]
            jmp L005123af
    L00512374:
            cmp eax, 0xb4
            jle L0051238f
            lea edx, [eax*4]
            mov eax, offset gTrig_006617d0
            sub eax, edx
            _emit 0xd9
        _emit 0x00
            fchs
            jmp L005123af
    L0051238f:
            cmp eax, 0x5a
            jle L0051239f
            fld dword ptr [eax*4 + gMatchString_00661230]
            fchs
            jmp L005123af
    L0051239f:
            lea ecx, [eax*4]
            mov edx, offset gFloat_sine_table+0x168
            sub edx, ecx
            _emit 0xd9
        _emit 0x02
    L005123af:
            _emit 0xd9
        _emit 0x16
            fld st(1)
            mov dword ptr [esi + 8], edi
            mov dword ptr [esi + 0x14], edi
            fstp dword ptr [esi + 4]
            fxch st(1)
            fchs
            fstp dword ptr [esi + 0xc]
            mov dword ptr [esi + 0x18], edi
            mov dword ptr [esi + 0x1c], edi
            fstp dword ptr [esi + 0x10]
            mov dword ptr [esi + 0x24], edi
            mov dword ptr [esi + 0x28], edi
            mov dword ptr [esi + 0x2c], edi
            mov dword ptr [esi + 0x20], 0x3f800000
            pop edi
            pop esi
            pop ebx
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
            nop
            nop
            nop
    }
#endif
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
            _emit 0xdc
        _emit 0x0d
        _emit 0x18
        _emit 0xb2
        _emit 0x58
        _emit 0x00
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
            mov eax, 0x661938
            sub eax, edx
            _emit 0xd9
        _emit 0x00
            fchs
            jmp L00512483
    L00512453:
            cmp eax, 0xb4
            jle L00512465
            fld dword ptr [eax*4 + 0x6610c8]
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
            fld dword ptr [eax*4 + gMatchData_00660f60]
            jmp L005124f2
    L005124b7:
            cmp eax, 0xb4
            jle L005124d2
            lea edx, [eax*4]
            mov eax, offset gTrig_006617d0
            sub eax, edx
            _emit 0xd9
        _emit 0x00
            fchs
            jmp L005124f2
    L005124d2:
            cmp eax, 0x5a
            jle L005124e2
            fld dword ptr [eax*4 + gMatchString_00661230]
            fchs
            jmp L005124f2
    L005124e2:
            lea ecx, [eax*4]
            mov edx, offset gFloat_sine_table+0x168
            sub edx, ecx
            _emit 0xd9
        _emit 0x02
    L005124f2:
            _emit 0xd9
        _emit 0x05
        _emit 0x2c
        _emit 0xb2
        _emit 0x58
        _emit 0x00
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
void C2_NAKED C2_HOOK_FASTCALL DRMatrix34PreRotateX(br_matrix34* mat, br_angle rx) {
#ifndef CARPOCALYPSE2_MATCHING

    DRMatrix34RotateX(&mattmp2__trig, rx);
    BrMatrix34Mul(&mattmp1__trig, &mattmp2__trig, mat);
    BrMatrix34Copy(mat, &mattmp1__trig);
#else
    __asm {
            push ecx
            mov word ptr [esp], dx
            push esi
            mov eax, dword ptr [esp + 4]
            mov esi, ecx
            and eax, 0xffff
            push edi
            mov dword ptr [esp + 8], eax
            fild dword ptr [esp + 8]
            _emit 0xdc
        _emit 0x0d
        _emit 0x18
        _emit 0xb2
        _emit 0x58
        _emit 0x00
            call _ftol
            mov ecx, eax
            test ecx, ecx
            jge L0051265c
            neg eax
            cdq
            mov edi, 0x168
            idiv edi
            mov eax, edi
            sub eax, edx
            jmp L00512666
    L0051265c:
            cdq
            mov edi, 0x168
            idiv edi
            mov eax, edx
    L00512666:
            cmp eax, 0x10e
            jle L00512681
            lea edx, [eax*4]
            mov eax, 0x661938
            sub eax, edx
            _emit 0xd9
        _emit 0x00
            fchs
            jmp L005126b1
    L00512681:
            cmp eax, 0xb4
            jle L00512693
            fld dword ptr [eax*4 + 0x6610c8]
            fchs
            jmp L005126b1
    L00512693:
            cmp eax, 0x5a
            jle L005126aa
            lea edx, [eax*4]
            mov eax, offset gFloat_sine_table+0x2d0
            sub eax, edx
            _emit 0xd9
        _emit 0x00
            jmp L005126b1
    L005126aa:
            fld dword ptr [eax*4 + gFloat_sine_table]
    L005126b1:
            test ecx, ecx
            mov eax, ecx
            jge L005126c7
            neg eax
            cdq
            mov ecx, 0x168
            idiv ecx
            mov eax, ecx
            sub eax, edx
            jmp L005126d1
    L005126c7:
            cdq
            mov ecx, 0x168
            idiv ecx
            mov eax, edx
    L005126d1:
            cmp eax, 0x10e
            jle L005126e1
            fld dword ptr [eax*4 + gMatchData_00660f60]
            jmp L0051271c
    L005126e1:
            cmp eax, 0xb4
            jle L005126fc
            lea edx, [eax*4]
            mov eax, offset gTrig_006617d0
            sub eax, edx
            _emit 0xd9
        _emit 0x00
            fchs
            jmp L0051271c
    L005126fc:
            cmp eax, 0x5a
            jle L0051270c
            fld dword ptr [eax*4 + gMatchString_00661230]
            fchs
            jmp L0051271c
    L0051270c:
            lea ecx, [eax*4]
            mov edx, offset gFloat_sine_table+0x168
            sub edx, ecx
            _emit 0xd9
        _emit 0x02
    L0051271c:
            fst dword ptr [mattmp2__trig+0x10]
            fld st(1)
            push esi
            push offset mattmp2__trig
            fstp dword ptr [mattmp2__trig+0x14]
            fxch st(1)
            fchs
            fstp dword ptr [mattmp2__trig+0x1c]
            push offset mattmp1__trig
            mov dword ptr [mattmp2__trig], 0x3f800000
            fstp dword ptr [mattmp2__trig+0x20]
            mov dword ptr [mattmp2__trig+0x4], 0
            mov dword ptr [mattmp2__trig+0x8], 0
            mov dword ptr [mattmp2__trig+0xc], 0
            mov dword ptr [mattmp2__trig+0x18], 0
            mov dword ptr [mattmp2__trig+0x24], 0
            mov dword ptr [mattmp2__trig+0x28], 0
            mov dword ptr [mattmp2__trig+0x2c], 0
            call BrMatrix34Mul
            add esp, 0xc
            push offset mattmp1__trig
            push esi
            call BrMatrix34Copy
            add esp, 8
            pop edi
            pop esi
            pop ecx
            ret
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x005127b0
void C2_NAKED C2_HOOK_FASTCALL DRMatrix34PostRotateX(br_matrix34* mat, br_angle rx) {
#ifndef CARPOCALYPSE2_MATCHING

    DRMatrix34RotateX(&mattmp2__trig, rx);
    BrMatrix34Mul(&mattmp1__trig, mat, &mattmp2__trig);
    BrMatrix34Copy(mat, &mattmp1__trig);
#else
    __asm {
            push ecx
            mov word ptr [esp], dx
            push esi
            mov eax, dword ptr [esp + 4]
            mov esi, ecx
            and eax, 0xffff
            push edi
            mov dword ptr [esp + 8], eax
            fild dword ptr [esp + 8]
            _emit 0xdc
        _emit 0x0d
        _emit 0x18
        _emit 0xb2
        _emit 0x58
        _emit 0x00
            call _ftol
            mov ecx, eax
            test ecx, ecx
            jge L005127ec
            neg eax
            cdq
            mov edi, 0x168
            idiv edi
            mov eax, edi
            sub eax, edx
            jmp L005127f6
    L005127ec:
            cdq
            mov edi, 0x168
            idiv edi
            mov eax, edx
    L005127f6:
            cmp eax, 0x10e
            jle L00512811
            lea edx, [eax*4]
            mov eax, 0x661938
            sub eax, edx
            _emit 0xd9
        _emit 0x00
            fchs
            jmp L00512841
    L00512811:
            cmp eax, 0xb4
            jle L00512823
            fld dword ptr [eax*4 + 0x6610c8]
            fchs
            jmp L00512841
    L00512823:
            cmp eax, 0x5a
            jle L0051283a
            lea edx, [eax*4]
            mov eax, offset gFloat_sine_table+0x2d0
            sub eax, edx
            _emit 0xd9
        _emit 0x00
            jmp L00512841
    L0051283a:
            fld dword ptr [eax*4 + gFloat_sine_table]
    L00512841:
            test ecx, ecx
            mov eax, ecx
            jge L00512857
            neg eax
            cdq
            mov ecx, 0x168
            idiv ecx
            mov eax, ecx
            sub eax, edx
            jmp L00512861
    L00512857:
            cdq
            mov ecx, 0x168
            idiv ecx
            mov eax, edx
    L00512861:
            cmp eax, 0x10e
            jle L00512871
            fld dword ptr [eax*4 + gMatchData_00660f60]
            jmp L005128ac
    L00512871:
            cmp eax, 0xb4
            jle L0051288c
            lea edx, [eax*4]
            mov eax, offset gTrig_006617d0
            sub eax, edx
            _emit 0xd9
        _emit 0x00
            fchs
            jmp L005128ac
    L0051288c:
            cmp eax, 0x5a
            jle L0051289c
            fld dword ptr [eax*4 + gMatchString_00661230]
            fchs
            jmp L005128ac
    L0051289c:
            lea ecx, [eax*4]
            mov edx, offset gFloat_sine_table+0x168
            sub edx, ecx
            _emit 0xd9
        _emit 0x02
    L005128ac:
            fst dword ptr [mattmp2__trig+0x10]
            fld st(1)
            push offset mattmp2__trig
            push esi
            fstp dword ptr [mattmp2__trig+0x14]
            fxch st(1)
            fchs
            fstp dword ptr [mattmp2__trig+0x1c]
            push offset mattmp1__trig
            mov dword ptr [mattmp2__trig], 0x3f800000
            fstp dword ptr [mattmp2__trig+0x20]
            mov dword ptr [mattmp2__trig+0x4], 0
            mov dword ptr [mattmp2__trig+0x8], 0
            mov dword ptr [mattmp2__trig+0xc], 0
            mov dword ptr [mattmp2__trig+0x18], 0
            mov dword ptr [mattmp2__trig+0x24], 0
            mov dword ptr [mattmp2__trig+0x28], 0
            mov dword ptr [mattmp2__trig+0x2c], 0
            call BrMatrix34Mul
            add esp, 0xc
            push offset mattmp1__trig
            push esi
            call BrMatrix34Copy
            add esp, 8
            pop edi
            pop esi
            pop ecx
            ret
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00512940
void C2_NAKED C2_HOOK_FASTCALL DRMatrix34PreRotateY(br_matrix34* mat, br_angle ry) {
#ifndef CARPOCALYPSE2_MATCHING

    DRMatrix34RotateY(&mattmp2__trig, ry);
    BrMatrix34Mul(&mattmp1__trig, &mattmp2__trig, mat);
    BrMatrix34Copy(mat, &mattmp1__trig);
#else
    __asm {
            push ecx
            mov word ptr [esp], dx
            push esi
            mov eax, dword ptr [esp + 4]
            mov esi, ecx
            and eax, 0xffff
            push edi
            mov dword ptr [esp + 8], eax
            fild dword ptr [esp + 8]
            _emit 0xdc
        _emit 0x0d
        _emit 0x18
        _emit 0xb2
        _emit 0x58
        _emit 0x00
            call _ftol
            mov ecx, eax
            test ecx, ecx
            jge L0051297c
            neg eax
            cdq
            mov edi, 0x168
            idiv edi
            mov eax, edi
            sub eax, edx
            jmp L00512986
    L0051297c:
            cdq
            mov edi, 0x168
            idiv edi
            mov eax, edx
    L00512986:
            cmp eax, 0x10e
            jle L005129a1
            lea edx, [eax*4]
            mov eax, 0x661938
            sub eax, edx
            _emit 0xd9
        _emit 0x00
            fchs
            jmp L005129d1
    L005129a1:
            cmp eax, 0xb4
            jle L005129b3
            fld dword ptr [eax*4 + 0x6610c8]
            fchs
            jmp L005129d1
    L005129b3:
            cmp eax, 0x5a
            jle L005129ca
            lea edx, [eax*4]
            mov eax, offset gFloat_sine_table+0x2d0
            sub eax, edx
            _emit 0xd9
        _emit 0x00
            jmp L005129d1
    L005129ca:
            fld dword ptr [eax*4 + gFloat_sine_table]
    L005129d1:
            test ecx, ecx
            mov eax, ecx
            jge L005129e7
            neg eax
            cdq
            mov ecx, 0x168
            idiv ecx
            mov eax, ecx
            sub eax, edx
            jmp L005129f1
    L005129e7:
            cdq
            mov ecx, 0x168
            idiv ecx
            mov eax, edx
    L005129f1:
            cmp eax, 0x10e
            jle L00512a01
            fld dword ptr [eax*4 + gMatchData_00660f60]
            jmp L00512a3c
    L00512a01:
            cmp eax, 0xb4
            jle L00512a1c
            lea edx, [eax*4]
            mov eax, offset gTrig_006617d0
            sub eax, edx
            _emit 0xd9
        _emit 0x00
            fchs
            jmp L00512a3c
    L00512a1c:
            cmp eax, 0x5a
            jle L00512a2c
            fld dword ptr [eax*4 + gMatchString_00661230]
            fchs
            jmp L00512a3c
    L00512a2c:
            lea ecx, [eax*4]
            mov edx, offset gFloat_sine_table+0x168
            sub edx, ecx
            _emit 0xd9
        _emit 0x02
    L00512a3c:
            fst dword ptr [mattmp2__trig]
            fld st(1)
            fchs
            fstp dword ptr [mattmp2__trig+0x8]
            fxch st(1)
            fstp dword ptr [mattmp2__trig+0x18]
            push esi
            push offset mattmp2__trig
            fstp dword ptr [mattmp2__trig+0x20]
            push offset mattmp1__trig
            mov dword ptr [mattmp2__trig+0x4], 0
            mov dword ptr [mattmp2__trig+0xc], 0
            mov dword ptr [mattmp2__trig+0x10], 0x3f800000
            mov dword ptr [mattmp2__trig+0x14], 0
            mov dword ptr [mattmp2__trig+0x1c], 0
            mov dword ptr [mattmp2__trig+0x24], 0
            mov dword ptr [mattmp2__trig+0x28], 0
            mov dword ptr [mattmp2__trig+0x2c], 0
            call BrMatrix34Mul
            add esp, 0xc
            push offset mattmp1__trig
            push esi
            call BrMatrix34Copy
            add esp, 8
            pop edi
            pop esi
            pop ecx
            ret
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00512ad0
void C2_NAKED C2_HOOK_FASTCALL DRMatrix34PostRotateY(br_matrix34* mat, br_angle ry) {
#ifndef CARPOCALYPSE2_MATCHING

    DRMatrix34RotateY(&mattmp2__trig, ry);
    BrMatrix34Mul(&mattmp1__trig, mat, &mattmp2__trig);
    BrMatrix34Copy(mat, &mattmp1__trig);
#else
    __asm {
            push ecx
            mov word ptr [esp], dx
            push esi
            mov eax, dword ptr [esp + 4]
            mov esi, ecx
            and eax, 0xffff
            push edi
            mov dword ptr [esp + 8], eax
            fild dword ptr [esp + 8]
            _emit 0xdc
        _emit 0x0d
        _emit 0x18
        _emit 0xb2
        _emit 0x58
        _emit 0x00
            call _ftol
            mov ecx, eax
            test ecx, ecx
            jge L00512b0c
            neg eax
            cdq
            mov edi, 0x168
            idiv edi
            mov eax, edi
            sub eax, edx
            jmp L00512b16
    L00512b0c:
            cdq
            mov edi, 0x168
            idiv edi
            mov eax, edx
    L00512b16:
            cmp eax, 0x10e
            jle L00512b31
            lea edx, [eax*4]
            mov eax, 0x661938
            sub eax, edx
            _emit 0xd9
        _emit 0x00
            fchs
            jmp L00512b61
    L00512b31:
            cmp eax, 0xb4
            jle L00512b43
            fld dword ptr [eax*4 + 0x6610c8]
            fchs
            jmp L00512b61
    L00512b43:
            cmp eax, 0x5a
            jle L00512b5a
            lea edx, [eax*4]
            mov eax, offset gFloat_sine_table+0x2d0
            sub eax, edx
            _emit 0xd9
        _emit 0x00
            jmp L00512b61
    L00512b5a:
            fld dword ptr [eax*4 + gFloat_sine_table]
    L00512b61:
            test ecx, ecx
            mov eax, ecx
            jge L00512b77
            neg eax
            cdq
            mov ecx, 0x168
            idiv ecx
            mov eax, ecx
            sub eax, edx
            jmp L00512b81
    L00512b77:
            cdq
            mov ecx, 0x168
            idiv ecx
            mov eax, edx
    L00512b81:
            cmp eax, 0x10e
            jle L00512b91
            fld dword ptr [eax*4 + gMatchData_00660f60]
            jmp L00512bcc
    L00512b91:
            cmp eax, 0xb4
            jle L00512bac
            lea edx, [eax*4]
            mov eax, offset gTrig_006617d0
            sub eax, edx
            _emit 0xd9
        _emit 0x00
            fchs
            jmp L00512bcc
    L00512bac:
            cmp eax, 0x5a
            jle L00512bbc
            fld dword ptr [eax*4 + gMatchString_00661230]
            fchs
            jmp L00512bcc
    L00512bbc:
            lea ecx, [eax*4]
            mov edx, offset gFloat_sine_table+0x168
            sub edx, ecx
            _emit 0xd9
        _emit 0x02
    L00512bcc:
            fst dword ptr [mattmp2__trig]
            fld st(1)
            fchs
            fstp dword ptr [mattmp2__trig+0x8]
            fxch st(1)
            fstp dword ptr [mattmp2__trig+0x18]
            push offset mattmp2__trig
            push esi
            fstp dword ptr [mattmp2__trig+0x20]
            push offset mattmp1__trig
            mov dword ptr [mattmp2__trig+0x4], 0
            mov dword ptr [mattmp2__trig+0xc], 0
            mov dword ptr [mattmp2__trig+0x10], 0x3f800000
            mov dword ptr [mattmp2__trig+0x14], 0
            mov dword ptr [mattmp2__trig+0x1c], 0
            mov dword ptr [mattmp2__trig+0x24], 0
            mov dword ptr [mattmp2__trig+0x28], 0
            mov dword ptr [mattmp2__trig+0x2c], 0
            call BrMatrix34Mul
            add esp, 0xc
            push offset mattmp1__trig
            push esi
            call BrMatrix34Copy
            add esp, 8
            pop edi
            pop esi
            pop ecx
            ret
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00512c60
void C2_NAKED C2_HOOK_FASTCALL DRMatrix34PreRotateZ(br_matrix34* mat, br_angle rz) {
#ifndef CARPOCALYPSE2_MATCHING

    DRMatrix34RotateZ(&mattmp2__trig, rz);
    BrMatrix34Mul(&mattmp1__trig, &mattmp2__trig, mat);
    BrMatrix34Copy(mat, &mattmp1__trig);
#else
    __asm {
            push ecx
            mov word ptr [esp], dx
            push esi
            mov eax, dword ptr [esp + 4]
            mov esi, ecx
            and eax, 0xffff
            push edi
            mov dword ptr [esp + 8], eax
            fild dword ptr [esp + 8]
            _emit 0xdc
        _emit 0x0d
        _emit 0x18
        _emit 0xb2
        _emit 0x58
        _emit 0x00
            call _ftol
            mov ecx, eax
            test ecx, ecx
            jge L00512c9c
            neg eax
            cdq
            mov edi, 0x168
            idiv edi
            mov eax, edi
            sub eax, edx
            jmp L00512ca6
    L00512c9c:
            cdq
            mov edi, 0x168
            idiv edi
            mov eax, edx
    L00512ca6:
            cmp eax, 0x10e
            jle L00512cc1
            lea edx, [eax*4]
            mov eax, 0x661938
            sub eax, edx
            _emit 0xd9
        _emit 0x00
            fchs
            jmp L00512cf1
    L00512cc1:
            cmp eax, 0xb4
            jle L00512cd3
            fld dword ptr [eax*4 + 0x6610c8]
            fchs
            jmp L00512cf1
    L00512cd3:
            cmp eax, 0x5a
            jle L00512cea
            lea edx, [eax*4]
            mov eax, offset gFloat_sine_table+0x2d0
            sub eax, edx
            _emit 0xd9
        _emit 0x00
            jmp L00512cf1
    L00512cea:
            fld dword ptr [eax*4 + gFloat_sine_table]
    L00512cf1:
            test ecx, ecx
            mov eax, ecx
            jge L00512d07
            neg eax
            cdq
            mov ecx, 0x168
            idiv ecx
            mov eax, ecx
            sub eax, edx
            jmp L00512d11
    L00512d07:
            cdq
            mov ecx, 0x168
            idiv ecx
            mov eax, edx
    L00512d11:
            cmp eax, 0x10e
            jle L00512d21
            fld dword ptr [eax*4 + gMatchData_00660f60]
            jmp L00512d5c
    L00512d21:
            cmp eax, 0xb4
            jle L00512d3c
            lea edx, [eax*4]
            mov eax, offset gTrig_006617d0
            sub eax, edx
            _emit 0xd9
        _emit 0x00
            fchs
            jmp L00512d5c
    L00512d3c:
            cmp eax, 0x5a
            jle L00512d4c
            fld dword ptr [eax*4 + gMatchString_00661230]
            fchs
            jmp L00512d5c
    L00512d4c:
            lea ecx, [eax*4]
            mov edx, offset gFloat_sine_table+0x168
            sub edx, ecx
            _emit 0xd9
        _emit 0x02
    L00512d5c:
            fst dword ptr [mattmp2__trig]
            fld st(1)
            push esi
            push offset mattmp2__trig
            fstp dword ptr [mattmp2__trig+0x4]
            fxch st(1)
            fchs
            fstp dword ptr [mattmp2__trig+0xc]
            push offset mattmp1__trig
            mov dword ptr [mattmp2__trig+0x8], 0
            fstp dword ptr [mattmp2__trig+0x10]
            mov dword ptr [mattmp2__trig+0x14], 0
            mov dword ptr [mattmp2__trig+0x18], 0
            mov dword ptr [mattmp2__trig+0x1c], 0
            mov dword ptr [mattmp2__trig+0x20], 0x3f800000
            mov dword ptr [mattmp2__trig+0x24], 0
            mov dword ptr [mattmp2__trig+0x28], 0
            mov dword ptr [mattmp2__trig+0x2c], 0
            call BrMatrix34Mul
            add esp, 0xc
            push offset mattmp1__trig
            push esi
            call BrMatrix34Copy
            add esp, 8
            pop edi
            pop esi
            pop ecx
            ret
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00512df0
void C2_NAKED C2_HOOK_FASTCALL DRMatrix34PostRotateZ(br_matrix34* mat, br_angle rz) {
#ifndef CARPOCALYPSE2_MATCHING

    DRMatrix34RotateZ(&mattmp2__trig, rz);
    BrMatrix34Mul(&mattmp1__trig, mat, &mattmp2__trig);
    BrMatrix34Copy(mat, &mattmp1__trig);
#else
    __asm {
            push ecx
            mov word ptr [esp], dx
            push esi
            mov eax, dword ptr [esp + 4]
            mov esi, ecx
            and eax, 0xffff
            push edi
            mov dword ptr [esp + 8], eax
            fild dword ptr [esp + 8]
            _emit 0xdc
        _emit 0x0d
        _emit 0x18
        _emit 0xb2
        _emit 0x58
        _emit 0x00
            call _ftol
            mov ecx, eax
            test ecx, ecx
            jge L00512e2c
            neg eax
            cdq
            mov edi, 0x168
            idiv edi
            mov eax, edi
            sub eax, edx
            jmp L00512e36
    L00512e2c:
            cdq
            mov edi, 0x168
            idiv edi
            mov eax, edx
    L00512e36:
            cmp eax, 0x10e
            jle L00512e51
            lea edx, [eax*4]
            mov eax, 0x661938
            sub eax, edx
            _emit 0xd9
        _emit 0x00
            fchs
            jmp L00512e81
    L00512e51:
            cmp eax, 0xb4
            jle L00512e63
            fld dword ptr [eax*4 + 0x6610c8]
            fchs
            jmp L00512e81
    L00512e63:
            cmp eax, 0x5a
            jle L00512e7a
            lea edx, [eax*4]
            mov eax, offset gFloat_sine_table+0x2d0
            sub eax, edx
            _emit 0xd9
        _emit 0x00
            jmp L00512e81
    L00512e7a:
            fld dword ptr [eax*4 + gFloat_sine_table]
    L00512e81:
            test ecx, ecx
            mov eax, ecx
            jge L00512e97
            neg eax
            cdq
            mov ecx, 0x168
            idiv ecx
            mov eax, ecx
            sub eax, edx
            jmp L00512ea1
    L00512e97:
            cdq
            mov ecx, 0x168
            idiv ecx
            mov eax, edx
    L00512ea1:
            cmp eax, 0x10e
            jle L00512eb1
            fld dword ptr [eax*4 + gMatchData_00660f60]
            jmp L00512eec
    L00512eb1:
            cmp eax, 0xb4
            jle L00512ecc
            lea edx, [eax*4]
            mov eax, offset gTrig_006617d0
            sub eax, edx
            _emit 0xd9
        _emit 0x00
            fchs
            jmp L00512eec
    L00512ecc:
            cmp eax, 0x5a
            jle L00512edc
            fld dword ptr [eax*4 + gMatchString_00661230]
            fchs
            jmp L00512eec
    L00512edc:
            lea ecx, [eax*4]
            mov edx, offset gFloat_sine_table+0x168
            sub edx, ecx
            _emit 0xd9
        _emit 0x02
    L00512eec:
            fst dword ptr [mattmp2__trig]
            fld st(1)
            push offset mattmp2__trig
            push esi
            fstp dword ptr [mattmp2__trig+0x4]
            fxch st(1)
            fchs
            fstp dword ptr [mattmp2__trig+0xc]
            push offset mattmp1__trig
            mov dword ptr [mattmp2__trig+0x8], 0
            fstp dword ptr [mattmp2__trig+0x10]
            mov dword ptr [mattmp2__trig+0x14], 0
            mov dword ptr [mattmp2__trig+0x18], 0
            mov dword ptr [mattmp2__trig+0x1c], 0
            mov dword ptr [mattmp2__trig+0x20], 0x3f800000
            mov dword ptr [mattmp2__trig+0x24], 0
            mov dword ptr [mattmp2__trig+0x28], 0
            mov dword ptr [mattmp2__trig+0x2c], 0
            call BrMatrix34Mul
            add esp, 0xc
            push offset mattmp1__trig
            push esi
            call BrMatrix34Copy
            add esp, 8
            pop edi
            pop esi
            pop ecx
            ret
            nop
    }
#endif
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
void C2_NAKED C2_HOOK_FASTCALL DRMatrix23Rotate(br_matrix23* mat, br_angle rz) {
#ifndef CARPOCALYPSE2_MATCHING
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
#else
    __asm {
            push ecx
            mov word ptr [esp], dx
            push ebx
            mov eax, dword ptr [esp + 4]
            push esi
            and eax, 0xffff
            mov esi, ecx
            mov dword ptr [esp + 8], eax
            fild dword ptr [esp + 8]
            _emit 0xdc
        _emit 0x0d
        _emit 0x18
        _emit 0xb2
        _emit 0x58
        _emit 0x00
            call _ftol
            mov ecx, eax
            test ecx, ecx
            jge L0051305c
            neg eax
            cdq
            mov ebx, 0x168
            idiv ebx
            mov eax, ebx
            sub eax, edx
            jmp L00513066
    L0051305c:
            cdq
            mov ebx, 0x168
            idiv ebx
            mov eax, edx
    L00513066:
            cmp eax, 0x10e
            jle L00513081
            lea edx, [eax*4]
            mov eax, 0x661938
            sub eax, edx
            _emit 0xd9
        _emit 0x00
            fchs
            jmp L005130b1
    L00513081:
            cmp eax, 0xb4
            jle L00513093
            fld dword ptr [eax*4 + 0x6610c8]
            fchs
            jmp L005130b1
    L00513093:
            cmp eax, 0x5a
            jle L005130aa
            lea edx, [eax*4]
            mov eax, offset gFloat_sine_table+0x2d0
            sub eax, edx
            _emit 0xd9
        _emit 0x00
            jmp L005130b1
    L005130aa:
            fld dword ptr [eax*4 + gFloat_sine_table]
    L005130b1:
            test ecx, ecx
            mov eax, ecx
            jge L005130c7
            neg eax
            cdq
            mov ecx, 0x168
            idiv ecx
            mov eax, ecx
            sub eax, edx
            jmp L005130d1
    L005130c7:
            cdq
            mov ecx, 0x168
            idiv ecx
            mov eax, edx
    L005130d1:
            cmp eax, 0x10e
            jle L005130e1
            fld dword ptr [eax*4 + gMatchData_00660f60]
            jmp L0051311c
    L005130e1:
            cmp eax, 0xb4
            jle L005130fc
            lea edx, [eax*4]
            mov eax, offset gTrig_006617d0
            sub eax, edx
            _emit 0xd9
        _emit 0x00
            fchs
            jmp L0051311c
    L005130fc:
            cmp eax, 0x5a
            jle L0051310c
            fld dword ptr [eax*4 + gMatchString_00661230]
            fchs
            jmp L0051311c
    L0051310c:
            lea ecx, [eax*4]
            mov edx, offset gFloat_sine_table+0x168
            sub edx, ecx
            _emit 0xd9
        _emit 0x02
    L0051311c:
            _emit 0xd9
        _emit 0x16
            fld st(1)
            mov dword ptr [esi + 0x10], 0
            mov dword ptr [esi + 0x14], 0
            fstp dword ptr [esi + 4]
            fxch st(1)
            fchs
            fstp dword ptr [esi + 8]
            fstp dword ptr [esi + 0xc]
            pop esi
            pop ebx
            pop ecx
            ret
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00513140
void C2_NAKED C2_HOOK_FASTCALL DRMatrix23PreRotate(br_matrix23* mat, br_angle rz) {
#ifndef CARPOCALYPSE2_MATCHING

    DRMatrix23Rotate(&mat23tmp2, rz);
    BrMatrix23Mul(&mat23tmp1, &mat23tmp2, mat);
    BrMatrix23Copy(mat, &mat23tmp1);
#else
    __asm {
            push ecx
            mov word ptr [esp], dx
            push esi
            mov eax, dword ptr [esp + 4]
            mov esi, ecx
            and eax, 0xffff
            push edi
            mov dword ptr [esp + 8], eax
            fild dword ptr [esp + 8]
            _emit 0xdc
        _emit 0x0d
        _emit 0x18
        _emit 0xb2
        _emit 0x58
        _emit 0x00
            call _ftol
            mov ecx, eax
            test ecx, ecx
            jge L0051317c
            neg eax
            cdq
            mov edi, 0x168
            idiv edi
            mov eax, edi
            sub eax, edx
            jmp L00513186
    L0051317c:
            cdq
            mov edi, 0x168
            idiv edi
            mov eax, edx
    L00513186:
            cmp eax, 0x10e
            jle L005131a1
            lea edx, [eax*4]
            mov eax, 0x661938
            sub eax, edx
            _emit 0xd9
        _emit 0x00
            fchs
            jmp L005131d1
    L005131a1:
            cmp eax, 0xb4
            jle L005131b3
            fld dword ptr [eax*4 + 0x6610c8]
            fchs
            jmp L005131d1
    L005131b3:
            cmp eax, 0x5a
            jle L005131ca
            lea edx, [eax*4]
            mov eax, offset gFloat_sine_table+0x2d0
            sub eax, edx
            _emit 0xd9
        _emit 0x00
            jmp L005131d1
    L005131ca:
            fld dword ptr [eax*4 + gFloat_sine_table]
    L005131d1:
            test ecx, ecx
            mov eax, ecx
            jge L005131e7
            neg eax
            cdq
            mov ecx, 0x168
            idiv ecx
            mov eax, ecx
            sub eax, edx
            jmp L005131f1
    L005131e7:
            cdq
            mov ecx, 0x168
            idiv ecx
            mov eax, edx
    L005131f1:
            cmp eax, 0x10e
            jle L00513201
            fld dword ptr [eax*4 + gMatchData_00660f60]
            jmp L0051323c
    L00513201:
            cmp eax, 0xb4
            jle L0051321c
            lea edx, [eax*4]
            mov eax, offset gTrig_006617d0
            sub eax, edx
            _emit 0xd9
        _emit 0x00
            fchs
            jmp L0051323c
    L0051321c:
            cmp eax, 0x5a
            jle L0051322c
            fld dword ptr [eax*4 + gMatchString_00661230]
            fchs
            jmp L0051323c
    L0051322c:
            lea ecx, [eax*4]
            mov edx, offset gFloat_sine_table+0x168
            sub edx, ecx
            _emit 0xd9
        _emit 0x02
    L0051323c:
            fst dword ptr [mat23tmp2]
            fld st(1)
            push esi
            push offset mat23tmp2
            fstp dword ptr [mat23tmp2+0x4]
            fxch st(1)
            fchs
            fstp dword ptr [mat23tmp2+0x8]
            push offset mat23tmp1
            mov dword ptr [mat23tmp2+0x10], 0
            fstp dword ptr [mat23tmp2+0xc]
            mov dword ptr [mat23tmp2+0x14], 0
            call BrMatrix23Mul
            add esp, 0xc
            push offset mat23tmp1
            push esi
            call BrMatrix23Copy
            add esp, 8
            pop edi
            pop esi
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
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x005132a0
void C2_NAKED C2_HOOK_FASTCALL DRMatrix23PostRotate(br_matrix23* mat, br_angle rz) {
#ifndef CARPOCALYPSE2_MATCHING

    DRMatrix23Rotate(&mat23tmp2, rz);
    BrMatrix23Mul(&mat23tmp1, mat, &mat23tmp2);
    BrMatrix23Copy(mat, &mat23tmp1);
#else
    __asm {
            push ecx
            mov word ptr [esp], dx
            push esi
            mov eax, dword ptr [esp + 4]
            mov esi, ecx
            and eax, 0xffff
            push edi
            mov dword ptr [esp + 8], eax
            fild dword ptr [esp + 8]
            _emit 0xdc
        _emit 0x0d
        _emit 0x18
        _emit 0xb2
        _emit 0x58
        _emit 0x00
            call _ftol
            mov ecx, eax
            test ecx, ecx
            jge L005132dc
            neg eax
            cdq
            mov edi, 0x168
            idiv edi
            mov eax, edi
            sub eax, edx
            jmp L005132e6
    L005132dc:
            cdq
            mov edi, 0x168
            idiv edi
            mov eax, edx
    L005132e6:
            cmp eax, 0x10e
            jle L00513301
            lea edx, [eax*4]
            mov eax, 0x661938
            sub eax, edx
            _emit 0xd9
        _emit 0x00
            fchs
            jmp L00513331
    L00513301:
            cmp eax, 0xb4
            jle L00513313
            fld dword ptr [eax*4 + 0x6610c8]
            fchs
            jmp L00513331
    L00513313:
            cmp eax, 0x5a
            jle L0051332a
            lea edx, [eax*4]
            mov eax, offset gFloat_sine_table+0x2d0
            sub eax, edx
            _emit 0xd9
        _emit 0x00
            jmp L00513331
    L0051332a:
            fld dword ptr [eax*4 + gFloat_sine_table]
    L00513331:
            test ecx, ecx
            mov eax, ecx
            jge L00513347
            neg eax
            cdq
            mov ecx, 0x168
            idiv ecx
            mov eax, ecx
            sub eax, edx
            jmp L00513351
    L00513347:
            cdq
            mov ecx, 0x168
            idiv ecx
            mov eax, edx
    L00513351:
            cmp eax, 0x10e
            jle L00513361
            fld dword ptr [eax*4 + gMatchData_00660f60]
            jmp L0051339c
    L00513361:
            cmp eax, 0xb4
            jle L0051337c
            lea edx, [eax*4]
            mov eax, offset gTrig_006617d0
            sub eax, edx
            _emit 0xd9
        _emit 0x00
            fchs
            jmp L0051339c
    L0051337c:
            cmp eax, 0x5a
            jle L0051338c
            fld dword ptr [eax*4 + gMatchString_00661230]
            fchs
            jmp L0051339c
    L0051338c:
            lea ecx, [eax*4]
            mov edx, offset gFloat_sine_table+0x168
            sub edx, ecx
            _emit 0xd9
        _emit 0x02
    L0051339c:
            fst dword ptr [mat23tmp2]
            fld st(1)
            push offset mat23tmp2
            push esi
            fstp dword ptr [mat23tmp2+0x4]
            fxch st(1)
            fchs
            fstp dword ptr [mat23tmp2+0x8]
            push offset mat23tmp1
            mov dword ptr [mat23tmp2+0x10], 0
            fstp dword ptr [mat23tmp2+0xc]
            mov dword ptr [mat23tmp2+0x14], 0
            call BrMatrix23Mul
            add esp, 0xc
            push offset mat23tmp1
            push esi
            call BrMatrix23Copy
            add esp, 8
            pop edi
            pop esi
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
            nop
    }
#endif
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
            fcomp dword ptr [gMatchFloat_00589364]
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