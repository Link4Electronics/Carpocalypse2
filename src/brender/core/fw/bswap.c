#include "bswap.h"

#define SWAP(A, B) do { (A) ^= (B); (B) ^= (A); (A) ^= (B); } while (0)

// FUNCTION: CARMA2_HW 0x00528190
br_uint_32 C2_HOOK_CDECL BrSwap32(br_uint_32 l) {
    union { // size: 0x4
        unsigned long l; // @0x0
        unsigned char c[4]; // @0x0
    } u;

    u.l = l;
    SWAP(u.c[0], u.c[3]);
    SWAP(u.c[1], u.c[2]);
    return u.l;
}

// FUNCTION: CARMA2_HW 0x005281d0
br_uint_16 C2_HOOK_CDECL BrSwap16(br_uint_16 s) {
    union { // size: 0x2
        unsigned short s; // @0x0
        unsigned char c[2]; // @0x0
    } u;

    u.s = s;
    SWAP(u.c[0], u.c[1]);
    return u.s;
}

// FUNCTION: CARMA2_HW 0x00528200
br_float C2_HOOK_CDECL BrSwapFloat(br_float f) {
    union { // size: 0x4
        br_float f; // @0x0
        unsigned char c[4]; // @0x0
    } u;

    u.f = f;
    SWAP(u.c[0], u.c[3]);
    SWAP(u.c[1], u.c[2]);
    return u.f;
}

// FUNCTION: CARMA2_HW 0x00528240
C2_NAKED void* C2_HOOK_CDECL BrSwapBlock(void* block, int count, int size) {
#ifndef CARPOCALYPSE2_MATCHING
    br_uint_8* cp;
    int i;
    int k;

    switch (size) {
    case 1:
        break;
    case 2:
        for (i = 0, cp = block; i < count; i++, cp += 2) {
            SWAP(cp[0], cp[1]);
        }
        break;
    case 3:
        for (i = 0, cp = block; i < count; i++, cp += 3) {
            SWAP(cp[0], cp[2]);
        }
        break;
    case 4:
        for (i = 0, cp = block; i < count; i++, cp += 4) {
            SWAP(cp[0], cp[3]);
            SWAP(cp[1], cp[2]);
        }
        break;
    case 8:
        for (i = 0, cp = block; i < count; i++, cp += 8) {
            SWAP(cp[0], cp[7]);
            SWAP(cp[1], cp[6]);
            SWAP(cp[2], cp[5]);
            SWAP(cp[3], cp[4]);
        }
        break;
    default:
        for (i = 0, cp = block; i < count; i++, cp += size) {
            for (k = 0; k < size / 2; k++) {
                SWAP(cp[k], cp[size - k - 1]);
            }
        }
        break;
    }
    return block;
#else
    __asm {
            push ebx
            push ebp
            mov ebp, dword ptr [esp + 0x14]
            push esi
            push edi
            lea eax, [ebp - 1]
            cmp eax, 7
            ja L005283a8
            jmp dword ptr [eax*4 + 0x5283f0]
            mov edx, dword ptr [esp + 0x18]
            mov eax, dword ptr [esp + 0x14]
            test edx, edx
            jle L005283e5
    L0052826b:
            mov cl, byte ptr [eax + 1]
            mov bl, byte ptr [eax]
            xor bl, cl
            mov byte ptr [eax], bl
            mov cl, bl
            mov bl, byte ptr [eax + 1]
            add eax, 2
            xor bl, cl
            mov byte ptr [eax - 1], bl
            mov cl, bl
            mov bl, byte ptr [eax - 2]
            xor bl, cl
            dec edx
            mov byte ptr [eax - 2], bl
            jne L0052826b
            mov eax, dword ptr [esp + 0x14]
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret
            mov edx, dword ptr [esp + 0x18]
            mov eax, dword ptr [esp + 0x14]
            test edx, edx
            jle L005283e5
    L005282a7:
            mov cl, byte ptr [eax + 2]
            mov bl, byte ptr [eax]
            xor bl, cl
            mov byte ptr [eax], bl
            mov cl, bl
            mov bl, byte ptr [eax + 2]
            add eax, 3
            xor bl, cl
            mov byte ptr [eax - 1], bl
            mov cl, bl
            mov bl, byte ptr [eax - 3]
            xor bl, cl
            dec edx
            mov byte ptr [eax - 3], bl
            jne L005282a7
            mov eax, dword ptr [esp + 0x14]
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret
            mov esi, dword ptr [esp + 0x18]
            test esi, esi
            jle L005283e5
            mov edx, dword ptr [esp + 0x14]
            lea eax, [edx + 2]
    L005282e6:
            mov dl, byte ptr [eax + 1]
            mov bl, byte ptr [eax - 2]
            xor bl, dl
            mov cl, bl
            mov byte ptr [eax - 2], bl
            xor cl, dl
            mov dl, bl
            xor dl, cl
            mov byte ptr [eax + 1], cl
            mov cl, byte ptr [eax - 1]
            mov byte ptr [eax - 2], dl
            mov dl, byte ptr [eax]
            add eax, 4
            xor cl, dl
            mov byte ptr [eax - 5], cl
            mov bl, byte ptr [eax - 5]
            xor cl, dl
            xor bl, cl
            mov byte ptr [eax - 4], cl
            mov byte ptr [eax - 5], bl
            dec esi
            jne L005282e6
            mov eax, dword ptr [esp + 0x14]
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret
            mov esi, dword ptr [esp + 0x18]
            test esi, esi
            jle L005283e5
            mov eax, dword ptr [esp + 0x14]
            add eax, 6
    L00528338:
            mov dl, byte ptr [eax + 1]
            mov bl, byte ptr [eax - 6]
            xor bl, dl
            mov cl, bl
            mov byte ptr [eax - 6], bl
            xor cl, dl
            mov dl, bl
            mov bl, byte ptr [eax - 5]
            xor dl, cl
            mov byte ptr [eax - 6], dl
            mov dl, byte ptr [eax]
            xor bl, dl
            mov byte ptr [eax + 1], cl
            mov cl, bl
            mov byte ptr [eax - 5], bl
            xor cl, dl
            mov dl, bl
            mov bl, byte ptr [eax - 4]
            xor dl, cl
            mov byte ptr [eax - 5], dl
            mov dl, byte ptr [eax - 1]
            xor bl, dl
            mov byte ptr [eax], cl
            mov cl, bl
            mov byte ptr [eax - 4], bl
            xor cl, dl
            mov dl, bl
            xor dl, cl
            mov byte ptr [eax - 1], cl
            mov cl, byte ptr [eax - 3]
            mov byte ptr [eax - 4], dl
            mov dl, byte ptr [eax - 2]
            add eax, 8
            xor cl, dl
            mov byte ptr [eax - 0xb], cl
            mov bl, byte ptr [eax - 0xb]
            xor cl, dl
            xor bl, cl
            mov byte ptr [eax - 0xa], cl
            mov byte ptr [eax - 0xb], bl
            dec esi
            jne L00528338
            mov eax, dword ptr [esp + 0x14]
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret
    L005283a8:
            mov ecx, dword ptr [esp + 0x18]
            mov esi, dword ptr [esp + 0x14]
            test ecx, ecx
            jle L005283e5
            mov eax, ebp
            cdq
            sub eax, edx
            mov edi, eax
            mov eax, ecx
            sar edi, 1
    L005283bf:
            xor ecx, ecx
            test edi, edi
            jle L005283e0
            lea edx, [esi + ebp]
    L005283c8:
            mov bl, byte ptr [edx - 1]
            xor byte ptr [ecx + esi], bl
            mov bl, byte ptr [ecx + esi]
            xor byte ptr [edx - 1], bl
            mov bl, byte ptr [edx - 1]
            xor byte ptr [ecx + esi], bl
            inc ecx
            dec edx
            cmp ecx, edi
            jl L005283c8
    L005283e0:
            add esi, ebp
            dec eax
            jne L005283bf
    L005283e5:
            mov eax, dword ptr [esp + 0x14]
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret
            mov edi, edi
            _emit 0xe5
            _emit 0x83
            _emit 0x52
            _emit 0x00
            _emit 0x5b
            _emit 0x82
            _emit 0x52
            _emit 0x00
            _emit 0x97
            _emit 0x82
            _emit 0x52
            _emit 0x00
            _emit 0xd3
            _emit 0x82
            _emit 0x52
            _emit 0x00
            _emit 0xa8
            _emit 0x83
            _emit 0x52
            _emit 0x00
            _emit 0xa8
            _emit 0x83
            _emit 0x52
            _emit 0x00
            _emit 0xa8
            _emit 0x83
            _emit 0x52
            _emit 0x00
            _emit 0x25
            _emit 0x83
            _emit 0x52
            _emit 0x00
    }
#endif
}
