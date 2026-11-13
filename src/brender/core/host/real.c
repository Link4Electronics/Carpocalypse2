#include "real.h"

#include "c2_stdlib.h"

// GLOBAL: CARMA2_HW 0x0067052a
br_uint_16 _RealSelector;

#ifdef CARPOCALYPSE2_MATCHING
#pragma pack(push, 2)

// GLOBAL: CARMA2_HW 0x00670518
// pl_registers

struct {
    br_uint_16 interrupt;
    br_uint_16 rds;
    br_uint_16 res;
    br_uint_16 rfs;
    br_uint_16 rgs;
    br_uint_32 reax;
    br_uint_32 redx;
} pl_registers;
#pragma pack(pop)

#endif


#ifdef CARPOCALYPSE2_MATCHING
#ifdef CARPOCALYPSE2_MATCHING
extern char hostInfo[];
#endif
// FUNCTION: CARMA2_HW 0x00540086
__declspec(naked) br_error C2_HOOK_CDECL HostSelectorReal(br_uint_16 *selp) {

    __asm {
            push ebp
            mov ebp, esp
            mov eax, dword ptr [ebp + 8]
            mov dx, word ptr [_RealSelector]
            mov word ptr [eax], dx
            xor eax, eax
            leave
            ret
    }
}
#else
br_error C2_HOOK_CDECL HostSelectorReal(br_uint_16 *selp) {

    *selp = _RealSelector;
    return 0;
}
#endif

#ifdef CARPOCALYPSE2_MATCHING
// FUNCTION: CARMA2_HW 0x0053ff34
__declspec(naked) br_error C2_HOOK_CDECL HostRealAllocate(struct host_real_memory *mem, br_uint_32 size_arg) {

    __asm {
            push ebp
            mov ebp, esp
            push ebx
            push ecx
            mov ebx, dword ptr [ebp + 0xc]
            add ebx, 0xf
            shr ebx, 4
            xor eax, eax
            pop ecx
            pop ebx
            leave
            ret
            mov eax, 0x1002
            pop ecx
            pop ebx
            leave
            ret
    }
}
#else
br_error C2_HOOK_CDECL HostRealAllocate(struct host_real_memory *mem, br_uint_32 size_arg) {

    return 0;
}
#endif

#ifdef CARPOCALYPSE2_MATCHING
// FUNCTION: CARMA2_HW 0x0053ff51
__declspec(naked) br_error C2_HOOK_CDECL HostRealFree(struct host_real_memory *mem) {

    __asm {
            push ebp
            mov ebp, esp
            push ebx
            xor eax, eax
            pop ebx
            leave
            ret
    }
}
#else
br_error C2_HOOK_CDECL HostRealFree(struct host_real_memory *mem) {

    return 0;
}
#endif

#ifdef CARPOCALYPSE2_MATCHING
// FUNCTION: CARMA2_HW 0x0053ff5a
__declspec(naked) br_error C2_HOOK_CDECL HostRealInterruptGet(br_uint_32 vector, br_uint_16 *offp, br_uint_16 *vsegp) {

    __asm {
            push ebp
            mov ebp, esp
            push ebx
            xor ecx, ecx
            mov cl, byte ptr [ebp + 8]
            mov eax, 0x2503
            int 0x21
            mov eax, dword ptr [ebp + 0xc]
            mov word ptr [eax], bx
            shr ebx, 0x10
            mov eax, dword ptr [ebp + 0x10]
            mov word ptr [eax], bx
            xor eax, eax
            pop ebx
            leave
            ret
    }
}
#else
br_error C2_HOOK_CDECL HostRealInterruptGet(br_uint_32 vector, br_uint_16 *offp, br_uint_16 *vsegp) {

    return 0x2503;
}
#endif

#ifdef CARPOCALYPSE2_MATCHING
// FUNCTION: CARMA2_HW 0x0053ff7e
__declspec(naked) br_error C2_HOOK_CDECL HostRealInterruptSet(br_uint_32 vector, br_uint_32 voff, br_uint_32 vseg) {
    __asm {
            push ebp
            mov ebp, esp
            push ebx
            mov cl, byte ptr [ebp + 8]
            mov bx, word ptr [ebp + 0x10]
            shl ebx, 0x10
            mov bx, word ptr [ebp + 0xc]
            mov eax, 0x2505
            int 0x21
            xor eax, eax
            pop ebx
            leave
            ret
    }
}
#else
br_error C2_HOOK_CDECL HostRealInterruptSet(br_uint_32 vector, br_uint_32 voff, br_uint_32 vseg) {

    return 0x2505;
}
#endif

#ifdef CARPOCALYPSE2_MATCHING
// FUNCTION: CARMA2_HW 0x0053ff9c
__declspec(naked) br_error C2_HOOK_CDECL HostRealInterruptCall(br_uint_32 vector, union host_regs *regs) {

    __asm {
            push ebp
            mov ebp, esp
            push ebx
            push esi
            push edi
            push es
            mov bx, word ptr [ebp + 8]
            mov word ptr [hostInfo+0x470], bx
            push ebp
            mov edx, dword ptr [ebp + 0xc]
            mov edi, dword ptr [edx]
            mov esi, dword ptr [edx + 4]
            mov ebp, dword ptr [edx + 8]
            mov ecx, dword ptr [edx + 0x18]
            mov ebx, dword ptr [edx + 0x10]
            mov eax, dword ptr [edx + 0x1c]
            mov dword ptr [hostInfo+0x47a], eax
            mov eax, dword ptr [edx + 0x14]
            mov dword ptr [hostInfo+0x47e], eax
            mov ax, word ptr [edx + 0x24]
            mov word ptr [hostInfo+0x472], ax
            mov ax, word ptr [edx + 0x22]
            mov word ptr [hostInfo+0x474], ax
            mov ax, word ptr [edx + 0x26]
            mov word ptr [hostInfo+0x476], ax
            mov ax, word ptr [edx + 0x28]
            mov word ptr [hostInfo+0x478], ax
            push edx
            mov eax, 0x2511
            mov edx, offset hostInfo+0x470
            int 0x21
            mov dword ptr [hostInfo+0x47a], eax
            pushf
            pop ax
            pop edx
            mov word ptr [edx + 0x20], ax
            mov dword ptr [edx], edi
            mov dword ptr [edx + 4], esi
            mov dword ptr [edx + 8], ebp
            mov dword ptr [edx + 0x18], ecx
            mov dword ptr [edx + 0x10], ebx
            mov eax, dword ptr [hostInfo+0x47a]
            mov dword ptr [edx + 0x1c], eax
            mov eax, dword ptr [hostInfo+0x47e]
            mov dword ptr [edx + 0x14], eax
            mov ax, word ptr [hostInfo+0x472]
            mov word ptr [edx + 0x24], ax
            mov ax, word ptr [hostInfo+0x474]
            mov word ptr [edx + 0x22], ax
            mov ax, word ptr [hostInfo+0x476]
            mov word ptr [edx + 0x26], ax
            mov ax, word ptr [hostInfo+0x478]
            mov word ptr [edx + 0x28], ax
            pop ebp
            xor eax, eax
            pop es
            pop edi
            pop esi
            pop ebx
            leave
            ret
    }
}
#else
br_error C2_HOOK_CDECL HostRealInterruptCall(br_uint_32 vector, union host_regs *regs) {
    return 0x2511;
}
#endif

#ifdef CARPOCALYPSE2_MATCHING
// FUNCTION: CARMA2_HW 0x00540062
__declspec(naked) br_error C2_HOOK_CDECL RealSelectorBegin(void) {

    __asm {
            push ebx
            cmp word ptr [_RealSelector], 0
            je L00540074
            mov eax, 0x1007
            pop ebx
            ret
    L00540074:
            mov word ptr [_RealSelector], ax
            xor eax, eax
            pop ebx
            ret
            mov eax, 0x1002
            pop ebx
            ret
    }
}
#else
br_error C2_HOOK_CDECL RealSelectorBegin(void) {

    return 0x1002;
}
#endif

// FUNCTION: CARMA2_HW 0x00540085
void C2_HOOK_CDECL RealSelectorEnd(void) {

    // empty
}
