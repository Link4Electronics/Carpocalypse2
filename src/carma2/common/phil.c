#include "phil.h"

#include "carpocalypse2_macros.h"
#include "physics.h"

#include <brender/brender.h>

/* Quaternion APIs used by the packed orientation conversion below. */
extern br_quat* C2_HOOK_CDECL BrQuatNormalise(br_quat* q, const br_quat* qq);
extern br_matrix34* C2_HOOK_CDECL BrQuatToMatrix34(br_matrix34* mat, const br_quat* q);

extern const br_scalar gFLOAT_0058a3f8;
extern const br_scalar gFLOAT_0058a3fc;
extern const br_scalar gFLOAT_0058a400;
extern const br_scalar gFLOAT_0058a404;
extern const br_scalar gFLOAT_0058a408;
extern const br_scalar gFLOAT_0058a40c;

// GLOBAL: CARMA2_HW 0x0065d004
int gPHIL_enabled = 1;

// ScalarToFraction

// FractionToScalar

static double C2_HOOK_CDECL PhilOrientationScalar(double value) {
    union { br_scalar scalar; tU32 word; } bits;
    volatile tU32 stored;

    /* The integer reload forces the scalar rounding even with MSVC5. */
    bits.scalar = (br_scalar)value;
    stored = bits.word;
    bits.word = stored;
    return bits.scalar;
}

#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern br_quat* C2_HOOK_CDECL BrMatrix34ToQuat();
extern void __cdecl _ftol();
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_FASTCALL PhysicsError(tPhysicsError pError, const char* pMessage);
#endif
// FUNCTION: CARMA2_HW 0x0049be90
void C2_NAKED C2_HOOK_FASTCALL GetOrientationFromMatrix(tU16* pOrientation, const br_matrix34* pMatrix) {
#ifndef CARPOCALYPSE2_MATCHING
    br_quat quaternion;
    br_scalar value;

    BrMatrix34ToQuat(&quaternion, pMatrix);
    value = quaternion.x;
    if (value > gFLOAT_0058a3f8) {
        value = gFLOAT_0058a3f8;
    }
    if (value < gFLOAT_0058a3fc) {
        value = gFLOAT_0058a3fc;
    }
    value = (value - gFLOAT_0058a3fc) * gFLOAT_0058a400;
    value *= gFLOAT_0058a404;
    pOrientation[0] = (tU16)(value - gFLOAT_0058a408);
    value = quaternion.y;
    if (value > gFLOAT_0058a3f8) {
        value = gFLOAT_0058a3f8;
    }
    if (value < gFLOAT_0058a3fc) {
        value = gFLOAT_0058a3fc;
    }
    value = (value - gFLOAT_0058a3fc) * gFLOAT_0058a400;
    value *= gFLOAT_0058a404;
    pOrientation[1] = (tU16)(value - gFLOAT_0058a408);
    value = quaternion.z;
    if (value > gFLOAT_0058a3f8) {
        value = gFLOAT_0058a3f8;
    }
    if (value < gFLOAT_0058a3fc) {
        value = gFLOAT_0058a3fc;
    }
    value = (value - gFLOAT_0058a3fc) * gFLOAT_0058a400;
    value *= gFLOAT_0058a404;
    pOrientation[2] = (tU16)(value - gFLOAT_0058a408);
    value = quaternion.w;
    if (value > gFLOAT_0058a3f8) {
        value = gFLOAT_0058a3f8;
    }
    if (value < gFLOAT_0058a3fc) {
        value = gFLOAT_0058a3fc;
    }
    value = (value - gFLOAT_0058a3fc) * gFLOAT_0058a400;
    value *= gFLOAT_0058a404;
    pOrientation[3] = (tU16)(value - gFLOAT_0058a408);
#else
    __asm {
            sub esp, 0x10
            lea eax, [esp]
            push esi
            push edx
            mov esi, ecx
            push eax
            call BrMatrix34ToQuat
            fld dword ptr [esp + 0xc]
            fld dword ptr [esp + 0xc]
            fcomp dword ptr [gFLOAT_0058a3f8]
            add esp, 8
            fnstsw ax
            test ah, 0x41
            jne L0049bec1
            fstp st(0)
            fld dword ptr [gFLOAT_0058a3f8]
    L0049bec1:
            fcom dword ptr [gFLOAT_0058a3fc]
            fnstsw ax
            test ah, 1
            je L0049bed6
            fstp st(0)
            fld dword ptr [gFLOAT_0058a3fc]
    L0049bed6:
            fsub dword ptr [gFLOAT_0058a3fc]
            fmul dword ptr [gFLOAT_0058a400]
            fmul dword ptr [gFLOAT_0058a404]
            fsub dword ptr [gFLOAT_0058a408]
            call _ftol
            fld dword ptr [esp + 8]
            fcomp dword ptr [gFLOAT_0058a3f8]
            fld dword ptr [esp + 8]
            mov word ptr [esi], ax
            fnstsw ax
            test ah, 0x41
            jne L0049bf13
            fstp st(0)
            fld dword ptr [gFLOAT_0058a3f8]
    L0049bf13:
            fcom dword ptr [gFLOAT_0058a3fc]
            fnstsw ax
            test ah, 1
            je L0049bf28
            fstp st(0)
            fld dword ptr [gFLOAT_0058a3fc]
    L0049bf28:
            fsub dword ptr [gFLOAT_0058a3fc]
            fmul dword ptr [gFLOAT_0058a400]
            fmul dword ptr [gFLOAT_0058a404]
            fsub dword ptr [gFLOAT_0058a408]
            call _ftol
            fld dword ptr [esp + 0xc]
            fcomp dword ptr [gFLOAT_0058a3f8]
            fld dword ptr [esp + 0xc]
            mov word ptr [esi + 2], ax
            fnstsw ax
            test ah, 0x41
            jne L0049bf66
            fstp st(0)
            fld dword ptr [gFLOAT_0058a3f8]
    L0049bf66:
            fcom dword ptr [gFLOAT_0058a3fc]
            fnstsw ax
            test ah, 1
            je L0049bf7b
            fstp st(0)
            fld dword ptr [gFLOAT_0058a3fc]
    L0049bf7b:
            fsub dword ptr [gFLOAT_0058a3fc]
            fmul dword ptr [gFLOAT_0058a400]
            fmul dword ptr [gFLOAT_0058a404]
            fsub dword ptr [gFLOAT_0058a408]
            call _ftol
            fld dword ptr [esp + 0x10]
            fcomp dword ptr [gFLOAT_0058a3f8]
            fld dword ptr [esp + 0x10]
            mov word ptr [esi + 4], ax
            fnstsw ax
            test ah, 0x41
            jne L0049bfb9
            fstp st(0)
            fld dword ptr [gFLOAT_0058a3f8]
    L0049bfb9:
            fcom dword ptr [gFLOAT_0058a3fc]
            fnstsw ax
            test ah, 1
            je L0049bfce
            fstp st(0)
            fld dword ptr [gFLOAT_0058a3fc]
    L0049bfce:
            fsub dword ptr [gFLOAT_0058a3fc]
            fmul dword ptr [gFLOAT_0058a400]
            fmul dword ptr [gFLOAT_0058a404]
            fsub dword ptr [gFLOAT_0058a408]
            call _ftol
            mov word ptr [esi + 6], ax
            pop esi
            add esp, 0x10
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0049c000
void C2_NAKED C2_HOOK_FASTCALL GetMatrixFromOrientation(br_matrix34* pMatrix, const tU16* pOrientation) {
#ifndef CARPOCALYPSE2_MATCHING
    br_quat quaternion;
    tU16 parts[4];
    double value;
    int i;

    parts[0] = pOrientation[0];
    parts[1] = pOrientation[1];
    parts[2] = pOrientation[2];
    parts[3] = pOrientation[3];
    for (i = 0; i < 4; i++) {
        /* 0x37800100 is the original scalar; double its product unspilled. */
        value = (double)parts[i] * 1.5259254723787308e-05;
        (&quaternion.x)[i] = (br_scalar)PhilOrientationScalar((value + value) - 1.0);
    }
    BrQuatNormalise(&quaternion, &quaternion);
    BrQuatToMatrix34(pMatrix, &quaternion);
#else
    __asm {
            sub esp, 0x1c
            xor eax, eax
            push esi
            mov ax, word ptr [edx]
            push edi
            xor edi, edi
            mov dword ptr [esp + 8], eax
            mov di, word ptr [edx + 2]
            xor eax, eax
            mov ax, word ptr [edx + 4]
            mov dword ptr [esp + 0xc], edi
            fild dword ptr [esp + 8]
            fild dword ptr [esp + 0xc]
            xor edi, edi
            mov dword ptr [esp + 0x10], eax
            mov di, word ptr [edx + 6]
            lea esi, [esp + 0x14]
            fild dword ptr [esp + 0x10]
            mov dword ptr [esp + 0x10], edi
            lea edx, [esp + 0x14]
            fild dword ptr [esp + 0x10]
            fxch st(3)
            fmul dword ptr [gFLOAT_0058a40c]
            fxch st(2)
            fmul dword ptr [gFLOAT_0058a40c]
            fxch st(1)
            fmul dword ptr [gFLOAT_0058a40c]
            fxch st(3)
            fmul dword ptr [gFLOAT_0058a40c]
            fxch st(2)
            fadd st(0), st(0)
            fxch st(1)
            fadd st(0), st(0)
            fxch st(3)
            fadd st(0), st(0)
            fxch st(2)
            fadd st(0), st(0)
            fxch st(1)
            fsub dword ptr [gFLOAT_0058a3f8]
            fxch st(3)
            fsub dword ptr [gFLOAT_0058a3f8]
            fxch st(2)
            fsub dword ptr [gFLOAT_0058a3f8]
            fxch st(1)
            fsub dword ptr [gFLOAT_0058a3f8]
            fxch st(3)
            fstp dword ptr [esp + 0x14]
            fxch st(1)
            fstp dword ptr [esp + 0x18]
            push esi
            push edx
            fstp dword ptr [esp + 0x24]
            fstp dword ptr [esp + 0x28]
            mov esi, ecx
            call BrQuatNormalise
            add esp, 8
            lea eax, [esp + 0x14]
            push eax
            push esi
            call BrQuatToMatrix34
            add esp, 8
            pop edi
            pop esi
            add esp, 0x1c
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

// OmegaCompScalarTo16

// OmegaComp16ToScalar

// GetOmega16FromOmega

// GetOmegaFromOmega16

// GetObjectNetworkStuff

// GetVelocityAndPosition

// SizeOfObjectNetworkStuff

// WriteObjectNetworkStuff

// GetHierarchyNetworkStuff

// FindObject

// WriteHierarchyNetworkStuff

// GetHierarchyNetworkSize

// GetNetworkDataSize
// FUNCTION: CARMA2_HW 0x0049cee0
int C2_HOOK_FASTCALL GetNetworkDataSize(tU8* pNet_data) {
    int size;
    tU8* next;
    tU8 more;

    switch (*pNet_data & 0xf) {
    case 0:
        size = 0x28;
        break;
    case 1:
    case 4:
        size = 0xc;
        break;
    case 2:
    case 3:
        size = 0x10;
        break;
    default:
        PhysicsError(9, "netphys got messed up");
        size = 0;
    }
    more = *pNet_data & 0x80;
    while (more != 0) {
        next = pNet_data + size;
        size += GetNetworkDataSize(next);
        more = *next & 0x40;
    }
    return size;
}

// FUNCTION: CARMA2_HW 0x0049cf50
int C2_NAKED C2_HOOK_FASTCALL GetSingleMatrixFromNetworkData(tU8* pNet_data, br_matrix34* pMatrix) {
#ifndef CARPOCALYPSE2_MATCHING
    br_quat quaternion;

    switch (pNet_data[0] & 0xF) {
    case 0:
        quaternion.x = (br_scalar)((const tU16*)(pNet_data + 0x14))[0] * gFLOAT_0058a40c;
        quaternion.x = quaternion.x + quaternion.x - gFLOAT_0058a3f8;
        quaternion.y = (br_scalar)((const tU16*)(pNet_data + 0x14))[1] * gFLOAT_0058a40c;
        quaternion.y = quaternion.y + quaternion.y - gFLOAT_0058a3f8;
        quaternion.z = (br_scalar)((const tU16*)(pNet_data + 0x14))[2] * gFLOAT_0058a40c;
        quaternion.z = quaternion.z + quaternion.z - gFLOAT_0058a3f8;
        quaternion.w = (br_scalar)((const tU16*)(pNet_data + 0x14))[3] * gFLOAT_0058a40c;
        quaternion.w = quaternion.w + quaternion.w - gFLOAT_0058a3f8;
        BrQuatNormalise(&quaternion, &quaternion);
        BrQuatToMatrix34(pMatrix, &quaternion);
        memcpy(pMatrix->m[3], pNet_data + 0x1C, sizeof(pMatrix->m[3]));
        return 0x28;
    case 1:
    case 4:
        quaternion.x = (br_scalar)((const tU16*)(pNet_data + 4))[0] * gFLOAT_0058a40c;
        quaternion.x = quaternion.x + quaternion.x - gFLOAT_0058a3f8;
        quaternion.y = (br_scalar)((const tU16*)(pNet_data + 4))[1] * gFLOAT_0058a40c;
        quaternion.y = quaternion.y + quaternion.y - gFLOAT_0058a3f8;
        quaternion.z = (br_scalar)((const tU16*)(pNet_data + 4))[2] * gFLOAT_0058a40c;
        quaternion.z = quaternion.z + quaternion.z - gFLOAT_0058a3f8;
        quaternion.w = (br_scalar)((const tU16*)(pNet_data + 4))[3] * gFLOAT_0058a40c;
        quaternion.w = quaternion.w + quaternion.w - gFLOAT_0058a3f8;
        BrQuatNormalise(&quaternion, &quaternion);
        BrQuatToMatrix34(pMatrix, &quaternion);
        return 0xC;
    case 2:
    case 3:
        quaternion.x = (br_scalar)((const tU16*)(pNet_data + 8))[0] * gFLOAT_0058a40c;
        quaternion.x = quaternion.x + quaternion.x - gFLOAT_0058a3f8;
        quaternion.y = (br_scalar)((const tU16*)(pNet_data + 8))[1] * gFLOAT_0058a40c;
        quaternion.y = quaternion.y + quaternion.y - gFLOAT_0058a3f8;
        quaternion.z = (br_scalar)((const tU16*)(pNet_data + 8))[2] * gFLOAT_0058a40c;
        quaternion.z = quaternion.z + quaternion.z - gFLOAT_0058a3f8;
        quaternion.w = (br_scalar)((const tU16*)(pNet_data + 8))[3] * gFLOAT_0058a40c;
        quaternion.w = quaternion.w + quaternion.w - gFLOAT_0058a3f8;
        BrQuatNormalise(&quaternion, &quaternion);
        BrQuatToMatrix34(pMatrix, &quaternion);
        return 0x10;
    default:
        PhysicsError((tPhysicsError)9, "netphys got messed up");
        return -1;
    }
#else
    __asm {
            sub esp, 0x1c
            push esi
            mov esi, ecx
            push edi
            mov edi, edx
            mov al, byte ptr [esi]
            and eax, 0xf
            cmp eax, 4
            ja L0049d1d9
            jmp dword ptr [eax*4 + 0x49d1f4]
            lea eax, [esp + 0x14]
            xor edx, edx
            mov dx, word ptr [esi + 0x14]
            push eax
            xor eax, eax
            mov dword ptr [esp + 0xc], edx
            mov ax, word ptr [esi + 0x16]
            xor edx, edx
            mov dx, word ptr [esi + 0x18]
            mov dword ptr [esp + 0x10], eax
            fild dword ptr [esp + 0xc]
            fild dword ptr [esp + 0x10]
            xor eax, eax
            mov dword ptr [esp + 0x14], edx
            mov ax, word ptr [esi + 0x1a]
            lea ecx, [esp + 0x18]
            fild dword ptr [esp + 0x14]
            mov dword ptr [esp + 0x14], eax
            push ecx
            fild dword ptr [esp + 0x18]
            fxch st(3)
            fmul dword ptr ds:[gFLOAT_0058a40c]
            fxch st(2)
            fmul dword ptr ds:[gFLOAT_0058a40c]
            fxch st(1)
            fmul dword ptr ds:[gFLOAT_0058a40c]
            fxch st(3)
            fmul dword ptr ds:[gFLOAT_0058a40c]
            fxch st(2)
            fadd st(0), st(0)
            fxch st(1)
            fadd st(0), st(0)
            fxch st(3)
            fadd st(0), st(0)
            fxch st(2)
            fadd st(0), st(0)
            fxch st(1)
            fsub dword ptr ds:[gFLOAT_0058a3f8]
            fxch st(3)
            fsub dword ptr ds:[gFLOAT_0058a3f8]
            fxch st(2)
            fsub dword ptr ds:[gFLOAT_0058a3f8]
            fxch st(1)
            fsub dword ptr ds:[gFLOAT_0058a3f8]
            fxch st(3)
            fstp dword ptr [esp + 0x1c]
            fxch st(1)
            fstp dword ptr [esp + 0x20]
            fstp dword ptr [esp + 0x24]
            fstp dword ptr [esp + 0x28]
            call BrQuatNormalise
            add esp, 8
            lea ecx, [esp + 0x14]
            push ecx
            push edi
            call BrQuatToMatrix34
            mov edx, dword ptr [esi + 0x1c]
            mov dword ptr [edi + 0x24], edx
            mov eax, dword ptr [esi + 0x20]
            mov dword ptr [edi + 0x28], eax
            mov ecx, dword ptr [esi + 0x24]
            add esp, 8
            mov dword ptr [edi + 0x2c], ecx
            mov eax, 0x28
            pop edi
            pop esi
            add esp, 0x1c
            ret
            lea eax, [esi + 4]
            lea edx, [esp + 0x14]
            xor esi, esi
            push edx
            mov si, word ptr [eax]
            xor edx, edx
            mov dx, word ptr [eax + 2]
            mov dword ptr [esp + 0x14], esi
            fild dword ptr [esp + 0x14]
            xor esi, esi
            mov dword ptr [esp + 0x10], edx
            mov si, word ptr [eax + 4]
            xor edx, edx
            mov dx, word ptr [eax + 6]
            mov dword ptr [esp + 0xc], esi
            fild dword ptr [esp + 0x10]
            fild dword ptr [esp + 0xc]
            mov dword ptr [esp + 0x14], edx
            lea ecx, [esp + 0x18]
            fild dword ptr [esp + 0x14]
            fxch st(3)
            fmul dword ptr ds:[gFLOAT_0058a40c]
            fxch st(2)
            fmul dword ptr ds:[gFLOAT_0058a40c]
            fxch st(1)
            fmul dword ptr ds:[gFLOAT_0058a40c]
            fxch st(3)
            fmul dword ptr ds:[gFLOAT_0058a40c]
            fxch st(2)
            fadd st(0), st(0)
            fxch st(1)
            fadd st(0), st(0)
            fxch st(3)
            fadd st(0), st(0)
            fxch st(2)
            fadd st(0), st(0)
            fxch st(1)
            fsub dword ptr ds:[gFLOAT_0058a3f8]
            fxch st(3)
            fsub dword ptr ds:[gFLOAT_0058a3f8]
            fxch st(2)
            fsub dword ptr ds:[gFLOAT_0058a3f8]
            fxch st(1)
            fsub dword ptr ds:[gFLOAT_0058a3f8]
            fxch st(3)
            fstp dword ptr [esp + 0x18]
            fxch st(1)
            fstp dword ptr [esp + 0x1c]
            push ecx
            fstp dword ptr [esp + 0x24]
            fstp dword ptr [esp + 0x28]
            call BrQuatNormalise
            add esp, 8
            lea eax, [esp + 0x14]
            push eax
            push edi
            call BrQuatToMatrix34
            add esp, 8
            mov eax, 0xc
            pop edi
            pop esi
            add esp, 0x1c
            ret
            lea eax, [esi + 8]
            lea ecx, [esp + 0x14]
            xor esi, esi
            push ecx
            mov si, word ptr [eax]
            xor ecx, ecx
            mov cx, word ptr [eax + 2]
            mov dword ptr [esp + 0x14], esi
            fild dword ptr [esp + 0x14]
            xor esi, esi
            mov dword ptr [esp + 0x10], ecx
            mov si, word ptr [eax + 4]
            xor ecx, ecx
            mov cx, word ptr [eax + 6]
            mov dword ptr [esp + 0xc], esi
            fild dword ptr [esp + 0x10]
            fild dword ptr [esp + 0xc]
            mov dword ptr [esp + 0x14], ecx
            lea edx, [esp + 0x18]
            fild dword ptr [esp + 0x14]
            fxch st(3)
            fmul dword ptr ds:[gFLOAT_0058a40c]
            fxch st(2)
            fmul dword ptr ds:[gFLOAT_0058a40c]
            fxch st(1)
            fmul dword ptr ds:[gFLOAT_0058a40c]
            fxch st(3)
            fmul dword ptr ds:[gFLOAT_0058a40c]
            fxch st(2)
            fadd st(0), st(0)
            fxch st(1)
            fadd st(0), st(0)
            fxch st(3)
            fadd st(0), st(0)
            fxch st(2)
            fadd st(0), st(0)
            fxch st(1)
            fsub dword ptr ds:[gFLOAT_0058a3f8]
            fxch st(3)
            fsub dword ptr ds:[gFLOAT_0058a3f8]
            fxch st(2)
            fsub dword ptr ds:[gFLOAT_0058a3f8]
            fxch st(1)
            fsub dword ptr ds:[gFLOAT_0058a3f8]
            fxch st(3)
            fstp dword ptr [esp + 0x18]
            fxch st(1)
            fstp dword ptr [esp + 0x1c]
            push edx
            fstp dword ptr [esp + 0x24]
            fstp dword ptr [esp + 0x28]
            call BrQuatNormalise
            add esp, 8
            lea edx, [esp + 0x14]
            push edx
            push edi
            call BrQuatToMatrix34
            add esp, 8
            mov eax, 0x10
            pop edi
            pop esi
            add esp, 0x1c
            ret
    L0049d1d9:
            mov edx, 0x659d18
            mov ecx, 9
            call PhysicsError
            or eax, 0xffffffff
            pop edi
            pop esi
            add esp, 0x1c
            ret
            lea ecx, [ecx]
            _emit 0x6e
            _emit 0xcf
            _emit 0x49
            _emit 0x00
            _emit 0x47
            _emit 0xd0
            _emit 0x49
            _emit 0x00
            _emit 0x10
            _emit 0xd1
            _emit 0x49
            _emit 0x00
            _emit 0x10
            _emit 0xd1
            _emit 0x49
            _emit 0x00
            _emit 0x47
            _emit 0xd0
            _emit 0x49
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

// PHILInit

// FUNCTION: CARMA2_HW 0x004b5d20
void C2_HOOK_FASTCALL PHILDisable(void) {
    gPHIL_enabled = 1;
}

// FUNCTION: CARMA2_HW 0x004b5d30
int C2_HOOK_FASTCALL FUN_004b5d30(void) {
    return 100 - gPHIL_count_list_collision_infos;
}

// PHILAddObject

// FUNCTION: CARMA2_HW 0x004b5ea0
int C2_HOOK_FASTCALL PHILRemoveObject(tPhysics_object* pObject) {
    tCollision_info_owner* owner;
    int i;

    if (gPHIL_enabled) {
        return 0;
    }

    if (pObject->field_0x239 == 2) {
        ((tQueued_object_info*)pObject->field_0x240)->object = NULL;
        pObject->field_0x240 = NULL;
        pObject->field_0x239 = 0;
        return 0;
    }

    if (gPHIL_munging_objects && !gPHIL_object_added) {
        gPHIL_queued_objects_for_removal[gPHIL_count_queued_objects_for_removal] = pObject;
        gPHIL_count_queued_objects_for_removal += 1;
        return 0;
    }

    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gPhil_queued_objects.headers); i++) {
        if (gPhil_queued_objects.headers[i].collision_info == pObject) {
            break;
        }
    }
    if (i == (int)CARPOCALYPSE2_ASIZE(gPhil_queued_objects.headers)
        || (owner = pObject->field_0x240) == NULL) {
        return 3;
    }
    owner->field_0x00 = NULL;
    pObject->field_0x240 = NULL;
    gPHIL_count_list_collision_infos -= 1;
    if (pObject == gList_collision_infos) {
        gList_collision_infos = pObject->next;
        if (pObject->next != NULL) {
            pObject->next->prev = NULL;
        }
    }

    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gPhil_queued_objects.headers); i++) {
        if (gPhil_queued_objects.headers[i].collision_info != NULL
            && gPhil_queued_objects.headers[i].collision_info->next == pObject) {
            gPhil_queued_objects.headers[i].collision_info->next = pObject->next;
            if (pObject->next != NULL) {
                pObject->next->prev = gPhil_queued_objects.headers[i].collision_info;
            }
            break;
        }
    }
    return 0;
}

// PHILGetFirstObject

// PHILGetNextObject

// FUNCTION: CARMA2_HW 0x004b6010
tU32 C2_HOOK_FASTCALL PHILReturnObjectStatus(tPhysics_object* pObject) {
    tCollision_info_owner* owner;

    if (!gPHIL_enabled) {
        if (pObject->field_0x239 == 2) {
            return ((tCollision_info_owner*)pObject->field_0x240)->field_0x08;
        }
        owner = pObject->field_0x240;
        if (owner != NULL) {
            return owner->field_0x08;
        }
    }
    return 0;
}

// PHILMakeObjectPassive

// FUNCTION: CARMA2_HW 0x004b6040
int C2_HOOK_FASTCALL PHILMakeObjectPassive(tPhysics_object* pObject) {
    tPHIL_queued_header* info;

    if (gPHIL_enabled) {
        return 0;
    }
    if (pObject->field_0x239 == 2) {
        info = pObject->field_0x240;
        info->field_0x8 = 1;
        return 0;
    }
    info = pObject->field_0x240;
    if (info != NULL) {
        info->field_0x8 = 1;
        pObject->flags |= 0x20;
        return 0;
    } else {
        return 3;
    }
}

// PHILMakeObjectActive

// PHILSetPassiveObjectsMatrix

// PHILAddActiveObject

// PHILAddObjectImmediately

// PHILAddActiveObjectImmediately

// PHILSetObjectProperty

// PHILGetObjectProperty

// FlushQueuedAddsAndRemoves

// ChangedObjectsCallbacks

// ProcessDrag2

// ProcessDrag

// MarkObjectAndChildrenAsPassive

// SetStandardGravity

// ProcessGravity

// LevelOutOnSurface

// PHILMungeObjects

// PHILActivatePassive

// PHILInterpolateObjects

// PHILDoPhysics

// PHILApplyPHILObject

// PHILGetPHILObjectState

// PhysicsObjectSetImpulse

// PhysicsObjectMoveVelocity

// FUNCTION: CARMA2_HW 0x004c29d0
void C2_HOOK_FASTCALL PhysicsObjectSetImpulse(tPhysics_object* pObject, br_vector3* pImpulse) {
    tPhysics_object* pChild;

    pObject->field_0x54 = *pImpulse;
    for (pChild = pObject->child; pChild != NULL; pChild = pChild->next) {
        PhysicsObjectSetImpulse(pChild, pImpulse);
    }
}

// FUNCTION: CARMA2_HW 0x004c2910
void C2_HOOK_FASTCALL PhysicsObjectMoveVelocity(tPhysics_object* pObject) {
    tPhysics_object* pChild;

    if (!pObject->disable_move_rotate) {
        pObject->v.v[0] += pObject->field_0x54.v[0];
        pObject->v.v[1] += pObject->field_0x54.v[1];
        pObject->v.v[2] += pObject->field_0x54.v[2];
    }
    if (pObject->child != NULL) {
        for (pChild = pObject->child; pChild != NULL; pChild = pChild->next) {
            PhysicsObjectMoveVelocity(pChild);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004c2970
void C2_HOOK_FASTCALL PhysicsObjectMoveVelocityList(tPhysics_object* pObject) {
    while (pObject != NULL) {
        if (!pObject->disable_move_rotate) {
            pObject->v.v[0] += pObject->field_0x54.v[0];
            pObject->v.v[1] += pObject->field_0x54.v[1];
            pObject->v.v[2] += pObject->field_0x54.v[2];
        }
        if (pObject->child != NULL) {
            PhysicsObjectMoveVelocityList(pObject->child);
        }
        pObject = pObject->next;
    }
}