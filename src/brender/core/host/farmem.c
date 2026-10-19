#include "farmem.h"

#include "c2_stdlib.h"

// Win32 uses protected mode. No need to support segmented memory.

#ifdef CARPOCALYPSE2_MATCHING
// FUNCTION: CARMA2_HW 0x005403a0
__declspec(naked) void C2_HOOK_CDECL HostFarBlockWrite(br_uint_32 offset, br_uint_16 sel, void *block, br_uint_32 count) {

    __asm {
        push    ebp
        mov     ebp, esp
        push    es
        push    esi
        push    edi
        les     edi, fword ptr [ebp + 8]
        mov     esi, dword ptr [block]
        mov     ecx, dword ptr [count]
        shr     ecx, 1
        rep movsw
        adc     ecx, ecx
        rep movsb
        pop     edi
        pop     esi
        pop     es
        leave
        ret
    }
}
#else
void C2_HOOK_CDECL HostFarBlockWrite(br_uint_32 offset, br_uint_16 sel, void *block, br_uint_32 count) {
    abort();
}
#endif

#ifdef CARPOCALYPSE2_MATCHING
// FUNCTION: CARMA2_HW 0x005403bd
__declspec(naked) void C2_HOOK_CDECL HostFarBlockFill(br_uint_32 offset, br_uint_16 sel, br_uint_8 value, br_uint_32 count) {

    __asm {
        push    ebp
        mov     ebp, esp
        push    es
        push    esi
        push    edi
        les     edi, fword ptr [ebp + 8]
        mov     al, byte ptr [value]
        mov     ecx, dword ptr [count]
        mov     ah, al
        shr     ecx, 1
        rep stosw
        adc     ecx, ecx
        rep stosb
        pop     edi
        pop     esi
        pop     es
        leave
        ret
    }
}
#else
void C2_HOOK_CDECL HostFarBlockFill(br_uint_32 offset, br_uint_16 sel, br_uint_8 value, br_uint_32 count) {

    NOT_IMPLEMENTED();
}
#endif

#ifdef CARPOCALYPSE2_MATCHING
// FUNCTION: CARMA2_HW 0x005403dc
__declspec(naked) void C2_HOOK_CDECL HostFarBlockRead(br_uint_32 offset, br_uint_16 sel, void *block, br_uint_32 count) {

    __asm {
        push    ebp
        mov     ebp, esp
        push    ds
        push    esi
        push    edi
        lds     esi, fword ptr [ebp + 8]
        mov     edi, dword ptr [block]
        mov     ecx, dword ptr [count]
        shr     ecx, 1
        rep movsw
        adc     ecx, ecx
        rep movsb
        pop     edi
        pop     esi
        pop     ds
        leave
        ret
    }
}
#else
void C2_HOOK_CDECL HostFarBlockRead(br_uint_32 offset, br_uint_16 sel, void *block, br_uint_32 count) {
    abort();
}
#endif

#ifdef CARPOCALYPSE2_MATCHING
// FUNCTION: CARMA2_HW 0x005403f9
__declspec(naked) br_uint_32 C2_HOOK_CDECL HostFarStringWrite(br_uint_32 offset, br_uint_16 sel, br_uint_8 *string, br_uint_32 max) {

    __asm {
        push    ebp
        mov     ebp, esp
        push    es
        push    esi
        push    edi
        les     edi, fword ptr [ebp + 8]
        mov     esi, dword ptr [string]
        mov     ecx, dword ptr [max]
        xor     eax, eax
        and     ecx, ecx
        je      far_string_write_done
    far_string_write_loop:
        mov     dl, byte ptr [esi]
        mov     byte ptr es:[edi], dl
        inc     eax
        and     dl, dl
        je      far_string_write_done
        inc     esi
        inc     edi
        dec     ecx
        jne     far_string_write_loop
        mov     byte ptr es:[edi - 1], 0
    far_string_write_done:
        pop     edi
        pop     esi
        pop     es
        leave
        ret
    }
}
#else
br_uint_32 C2_HOOK_CDECL HostFarStringWrite(br_uint_32 offset, br_uint_16 sel, br_uint_8 *string, br_uint_32 max) {

    NOT_IMPLEMENTED();
    return 0;
}
#endif

#ifdef CARPOCALYPSE2_MATCHING
// FUNCTION: CARMA2_HW 0x00540427
__declspec(naked) br_uint_32 C2_HOOK_CDECL HostFarStringRead(br_uint_32 offset, br_uint_16 sel, br_uint_8 *string, br_uint_32 max) {

    __asm {
        push    ebp
        mov     ebp, esp
        push    es
        push    esi
        push    edi
        les     edi, fword ptr [ebp + 8]
        mov     esi, dword ptr [string]
        mov     ecx, dword ptr [max]
        xor     eax, eax
        and     ecx, ecx
        je      far_string_read_done
    far_string_read_loop:
        mov     dl, byte ptr es:[edi]
        mov     byte ptr [esi], dl
        inc     eax
        and     dl, dl
        je      far_string_read_done
        inc     esi
        inc     edi
        dec     ecx
        jne     far_string_read_loop
        mov     byte ptr [esi - 1], 0
    far_string_read_done:
        pop     edi
        pop     esi
        pop     es
        leave
        ret
    }
}
#else
br_uint_32 C2_HOOK_CDECL HostFarStringRead(br_uint_32 offset, br_uint_16 sel, br_uint_8 *string, br_uint_32 max) {

    NOT_IMPLEMENTED();
    return 0;
}
#endif

#ifdef CARPOCALYPSE2_MATCHING
// FUNCTION: CARMA2_HW 0x00540454
__declspec(naked) void C2_HOOK_CDECL HostFarByteWrite(br_uint_32 offset, br_uint_16 sel, br_uint_8 value) {

    __asm {
        push    ebp
        mov     ebp, esp
        push    ds
        lds     edx, fword ptr [ebp + 8]
        mov     al, byte ptr [value]
        mov     byte ptr [edx], al
        pop     ds
        leave
        ret
    }
}
#else
void C2_HOOK_CDECL HostFarByteWrite(br_uint_32 offset, br_uint_16 sel, br_uint_8 value) {

    NOT_IMPLEMENTED();
}
#endif

#ifdef CARPOCALYPSE2_MATCHING
// FUNCTION: CARMA2_HW 0x00540463
__declspec(naked) br_uint_8 C2_HOOK_CDECL HostFarByteRead(br_uint_32 offset, br_uint_16 sel) {

    __asm {
        push    ebp
        mov     ebp, esp
        push    ds
        lds     eax, fword ptr [ebp + 8]
        mov     al, byte ptr [eax]
        pop     ds
        leave
        ret
    }
}
#else
br_uint_8 C2_HOOK_CDECL HostFarByteRead(br_uint_32 offset, br_uint_16 sel) {

    NOT_IMPLEMENTED();
    return 0;
}
#endif

#ifdef CARPOCALYPSE2_MATCHING
// FUNCTION: CARMA2_HW 0x0054046f
__declspec(naked) void C2_HOOK_CDECL HostFarWordWrite(br_uint_32 offset, br_uint_16 sel, br_uint_16 value) {

    __asm {
        push    ebp
        mov     ebp, esp
        push    ds
        lds     edx, fword ptr [ebp + 8]
        mov     ax, word ptr [value]
        mov     word ptr [edx], ax
        pop     ds
        leave
        ret
    }
}
#else
void C2_HOOK_CDECL HostFarWordWrite(br_uint_32 offset, br_uint_16 sel, br_uint_16 value) {

    NOT_IMPLEMENTED();
}
#endif

#ifdef CARPOCALYPSE2_MATCHING
// FUNCTION: CARMA2_HW 0x00540480
__declspec(naked) br_uint_16 C2_HOOK_CDECL HostFarWordRead(br_uint_32 offset, br_uint_16 sel) {

    __asm {
        push    ebp
        mov     ebp, esp
        push    ds
        lds     eax, fword ptr [ebp + 8]
        mov     ax, word ptr [eax]
        pop     ds
        leave
        ret
    }
}
#else
br_uint_16 C2_HOOK_CDECL HostFarWordRead(br_uint_32 offset, br_uint_16 sel) {

    NOT_IMPLEMENTED();
    return 0;
}
#endif

#ifdef CARPOCALYPSE2_MATCHING
// FUNCTION: CARMA2_HW 0x0054048d
__declspec(naked) void C2_HOOK_CDECL HostFarDWordWrite(br_uint_32 offset, br_uint_16 sel, br_uint_32 value) {

    __asm {
        push    ebp
        mov     ebp, esp
        push    ds
        lds     edx, fword ptr [ebp + 8]
        mov     eax, dword ptr [value]
        mov     dword ptr [edx], eax
        pop     ds
        leave
        ret
    }
}
#else
void C2_HOOK_CDECL HostFarDWordWrite(br_uint_32 offset, br_uint_16 sel, br_uint_32 value) {

    NOT_IMPLEMENTED();
}
#endif

#ifdef CARPOCALYPSE2_MATCHING
// FUNCTION: CARMA2_HW 0x0054049c
__declspec(naked) br_uint_32 C2_HOOK_CDECL HostFarDWordRead(br_uint_32 offset, br_uint_16 sel) {

    __asm {
        push    ebp
        mov     ebp, esp
        push    ds
        lds     eax, fword ptr [ebp + 8]
        mov     eax, dword ptr [eax]
        pop     ds
        leave
        ret
    }
}
#else
br_uint_32 C2_HOOK_CDECL HostFarDWordRead(br_uint_32 offset, br_uint_16 sel) {

    NOT_IMPLEMENTED();
    return 0;
}
#endif
