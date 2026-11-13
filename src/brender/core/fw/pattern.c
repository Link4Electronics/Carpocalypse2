#include "pattern.h"

#include <ctype.h>

// FUNCTION: CARMA2_HW 0x00529860
br_boolean C2_NAKED C2_HOOK_CDECL BrNamePatternMatch(const char* p, const char* s) {
#ifndef CARPOCALYPSE2_MATCHING
    const char* cp;

    if (p == NULL) {
        return 1;
    }
    if (s == NULL) {
        return 0;
    }

    for (;; s++, p++) {

        if (*p == '\0') {
            // if pattern string ends at the same point as search string, we match
            return *s == 0;
        }

        if (*p == '*') {
            cp = s;
            do {
                if (BrNamePatternMatch(p + 1, cp)) {
                    return 1;
                }
            } while (*cp++);
            return 0;
        }
        if (*p == '/') {
            return *s == 0;
        }
        if (*p == '?') {
            if (*s == '\0') {
                return 0;
            }
            continue;
        }

        if (toupper(*p) != toupper(*s)) {
            return 0;
        }
    }
    return 1;
#else
    __asm {
    L00529860:
            push esi
            push edi
            mov edi, dword ptr [esp + 0xc]
            test edi, edi
            jne L00529872
            mov eax, 1
            pop edi
            pop esi
            ret
    L00529872:
            mov esi, dword ptr [esp + 0x10]
            test esi, esi
            je L00529904
    L0052987e:
            mov cl, byte ptr [edi]
            movsx edx, cl
            cmp edx, 0x3f
            ja L005298a0
            xor eax, eax
            mov al, byte ptr [edx + 0x529920]
            jmp dword ptr [eax*4 + 0x52990c]
            cmp byte ptr [esi], 0
            je L00529904
            inc edi
            inc esi
            jmp L0052987e
    L005298a0:
            mov al, byte ptr [esi]
            cmp al, 0x61
            jl L005298b2
            cmp al, 0x7a
            jg L005298b2
            movsx eax, al
            sub eax, 0x20
            jmp L005298b5
    L005298b2:
            movsx eax, al
    L005298b5:
            cmp cl, 0x61
            jl L005298c2
            cmp cl, 0x7a
            lea ecx, [edx - 0x20]
            jle L005298c4
    L005298c2:
            mov ecx, edx
    L005298c4:
            cmp eax, ecx
            jne L00529904
            inc edi
            inc esi
            jmp L0052987e
            mov cl, byte ptr [esi]
            xor eax, eax
            test cl, cl
            sete al
            pop edi
            pop esi
            ret
            inc edi
            push esi
            push edi
            call L00529860
            add esp, 8
            test eax, eax
            jne L005298fc
    L005298e7:
            mov cl, byte ptr [esi]
            inc esi
            test cl, cl
            je L00529904
            push esi
            push edi
            call L00529860
            add esp, 8
            test eax, eax
            je L005298e7
    L005298fc:
            mov eax, 1
            pop edi
            pop esi
            ret
    L00529904:
            pop edi
            xor eax, eax
            pop esi
            ret
            lea ecx, [ecx]
    }
#endif
}
