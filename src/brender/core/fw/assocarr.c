#include "assocarr.h"

#include "fwsetup.h"
#include "mem.h"
#include "resource.h"
#include "token.h"

#include "core/std/brstdlib.h"

#include "c2_stdlib.h"

#ifdef CARPOCALYPSE2_MATCHING
extern void* C2_HOOK_CDECL BrMemCpy();
extern void C2_HOOK_CDECL BrMemFree();
extern void* C2_HOOK_CDECL BrResAllocate();
extern void C2_HOOK_CDECL BrResFree();
extern char* C2_HOOK_CDECL BrResStrDup();
extern br_token C2_HOOK_CDECL BrTokenType();
#endif
// FUNCTION: CARMA2_HW 0x00531500
br_associative_array* C2_HOOK_CDECL BrAssociativeArrayAllocate(void) {
    br_associative_array* pArray;

    pArray = BrResAllocate(fw.res, sizeof(br_associative_array), BR_MEMORY_SCRATCH);
    if (pArray == NULL) {
        return NULL;
    }
    pArray->tv = BrResAllocate(pArray, 10 * sizeof(br_token_value), BR_MEMORY_SCRATCH);
    if (pArray->tv == NULL) {
        return NULL;
    }
    pArray->max_elements = 10;
    pArray->num_elements = 0;
    return pArray;
}

br_error C2_HOOK_CDECL Set_Associative_Array_Value(br_associative_array* pArray, int index, br_value v) {

    if (BrTokenType(pArray->tv[index].t) == BRT_STRING) {
        if (pArray->tv[index].v.str != NULL) {
            BrResFree(pArray->tv[index].v.str);
        }
        if (v.str != NULL) {
            pArray->tv[index].v.str = BrResStrDup(pArray, v.str);
        } else {
            pArray->tv[index].v.str = NULL;
        }
    } else {
        pArray->tv[index].v = v;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00531540
br_error C2_NAKED C2_HOOK_STDCALL BrAssociativeArraySetEntry(br_associative_array* pArray, br_token t, br_value v) {
#ifndef CARPOCALYPSE2_MATCHING
    br_uint_16 i;
    br_token_value* temp;

    for (i = 0; i < pArray->num_elements; i++) {
        if (pArray->tv[i].t == t) {
            br_error res = Set_Associative_Array_Value(pArray, i, v);
            return res;
        }
    }
    if (pArray->num_elements < pArray->max_elements) {
        pArray->tv[pArray->num_elements].t = t;
        Set_Associative_Array_Value(pArray, pArray->num_elements, v);
        pArray->num_elements += 1;
        return 0;
    }
    temp = BrResAllocate(pArray, (pArray->max_elements + 10) * sizeof(br_token_value), BR_MEMORY_APPLICATION);
    BrMemCpy(temp, pArray->tv, pArray->max_elements * sizeof(br_token_value));
    BrResFree(pArray->tv);
    pArray->tv = temp;
    if (temp == NULL) {
        return 0x1002;
    }
    pArray->max_elements += 10;
    Set_Associative_Array_Value(pArray, pArray->num_elements, v);
    pArray->num_elements += 1;
    return 0;
#else
    __asm {
            push ebx
            push ebp
            mov ebx, dword ptr [esp + 0x10]
            push esi
            mov esi, dword ptr [esp + 0x10]
            xor ebp, ebp
            xor eax, eax
            push edi
            mov cx, word ptr [esi + 4]
            cmp cx, bp
            jbe L00531572
            mov edx, dword ptr [esi]
    L0053155b:
            mov edi, eax
            and edi, 0xffff
            cmp dword ptr [edx + edi*8], ebx
            je L00531700
            inc eax
            cmp ax, cx
            jb L0053155b
    L00531572:
            mov ax, word ptr [esi + 6]
            cmp cx, ax
            jae L00531616
            mov eax, dword ptr [esi]
            and ecx, 0xffff
            mov dword ptr [eax + ecx*8], ebx
            mov edx, dword ptr [esi]
            xor ecx, ecx
            mov cx, word ptr [esi + 4]
            mov dword ptr [edx + ecx*8 + 4], ebp
            mov cx, word ptr [esi + 4]
            mov eax, ecx
            and eax, 0xffff
            inc ecx
            mov word ptr [esi + 4], cx
            lea edi, [eax*8]
            mov eax, dword ptr [esi]
            mov ecx, dword ptr [eax + edi]
            push ecx
            call BrTokenType
            add esp, 4
            cmp eax, 0x1f
            je L005315d3
            mov edx, dword ptr [esi]
            mov eax, dword ptr [esp + 0x1c]
            mov dword ptr [edx + edi + 4], eax
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 0xc
    L005315d3:
            mov ecx, dword ptr [esi]
            mov eax, dword ptr [ecx + edi + 4]
            cmp eax, ebp
            je L005315e6
            push eax
            call BrResFree
            add esp, 4
    L005315e6:
            mov eax, dword ptr [esp + 0x1c]
            cmp eax, ebp
            je L00531607
            push eax
            push esi
            call BrResStrDup
            mov edx, dword ptr [esi]
            add esp, 8
            mov dword ptr [edx + edi + 4], eax
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 0xc
    L00531607:
            mov eax, dword ptr [esi]
            mov dword ptr [eax + edi + 4], ebp
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 0xc
    L00531616:
            and eax, 0xffff
            push 0x80
            lea ecx, [eax*8 + 0x50]
            push ecx
            push esi
            call BrResAllocate
            xor edx, edx
            add esp, 0xc
            mov dx, word ptr [esi + 6]
            mov edi, eax
            mov eax, dword ptr [esi]
            shl edx, 3
            push edx
            push eax
            push edi
            call BrMemCpy
            mov ecx, dword ptr [esi]
            add esp, 0xc
            push ecx
            call BrMemFree
            add esp, 4
            cmp edi, ebp
            mov dword ptr [esi], edi
            jne L00531666
            mov eax, 0x1002
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 0xc
    L00531666:
            add word ptr [esi + 6], 0xa
            xor edx, edx
            mov dx, word ptr [esi + 4]
            xor eax, eax
            mov dword ptr [edi + edx*8], ebx
            mov ax, word ptr [esi + 4]
            mov ecx, dword ptr [esi]
            mov dword ptr [ecx + eax*8 + 4], ebp
            mov cx, word ptr [esi + 4]
            mov edx, dword ptr [esi]
            mov eax, ecx
            and eax, 0xffff
            inc ecx
            mov word ptr [esi + 4], cx
            lea edi, [eax*8]
            mov eax, dword ptr [edx + eax*8]
            push eax
            call BrTokenType
            add esp, 4
            cmp eax, 0x1f
            je L005316bd
            mov ecx, dword ptr [esi]
            mov edx, dword ptr [esp + 0x1c]
            xor eax, eax
            mov dword ptr [ecx + edi + 4], edx
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 0xc
    L005316bd:
            mov eax, dword ptr [esi]
            mov eax, dword ptr [eax + edi + 4]
            cmp eax, ebp
            je L005316d0
            push eax
            call BrResFree
            add esp, 4
    L005316d0:
            mov eax, dword ptr [esp + 0x1c]
            cmp eax, ebp
            je L005316f1
    L005316d8:
            push eax
            push esi
            call BrResStrDup
            mov ecx, dword ptr [esi]
            add esp, 8
            mov dword ptr [ecx + edi + 4], eax
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 0xc
    L005316f1:
            mov edx, dword ptr [esi]
            xor eax, eax
            mov dword ptr [edx + edi + 4], ebp
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 0xc
    L00531700:
            and eax, 0xffff
            lea edi, [eax*8]
            mov eax, dword ptr [edi + edx]
            push eax
            call BrTokenType
            add esp, 4
            cmp eax, 0x1f
            je L00531730
            mov ecx, dword ptr [esi]
            mov edx, dword ptr [esp + 0x1c]
            xor eax, eax
            mov dword ptr [ecx + edi + 4], edx
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 0xc
    L00531730:
            mov eax, dword ptr [esi]
            mov eax, dword ptr [eax + edi + 4]
            cmp eax, ebp
            je L00531743
            push eax
            call BrResFree
            add esp, 4
    L00531743:
            mov eax, dword ptr [esp + 0x1c]
            cmp eax, ebp
            je L005316f1
            jmp L005316d8
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00531750
br_error C2_HOOK_STDCALL BrAssociativeArrayRemoveEntry(br_associative_array* pArray, br_token t) {
    br_uint_16 i;
    br_boolean bFound;

    bFound = 0;
    for (i = 0; i < pArray->num_elements; i++) {
        if (pArray->tv[i].t == t) {
            bFound = 1;
            break;
        }
    }
    if (!bFound) {
        return 0x1002;
    }
    if (BrTokenType(t) == BRT_STRING && pArray->tv[i].v.str != NULL) {
        BrResFree(pArray->tv[i].v.str);
    }
    for (; i < pArray->num_elements - 1; i++) {
        BrMemCpy(&pArray->tv[i], &pArray->tv[i + 1], sizeof(br_token_value));
    }
    pArray->num_elements -= 1;
    return 0; // BRE_OK
}

// FUNCTION: CARMA2_HW 0x00531810
br_error C2_HOOK_STDCALL BrAssociativeArrayQuery(br_associative_array* pArray, br_token t, br_value* pValue) {
    br_uint_16 i;

    for (i = 0; i < pArray->num_elements; i++) {
        if (pArray->tv[i].t == t) {
            *pValue = pArray->tv[i].v;
            return 0;
        }
    }
    return 0x1002;
}
