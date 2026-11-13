#include "brstddiag.h"

#include "core/fw/brbegin.h"

#include <stdio.h>
#include "c2_stdlib.h"


// GLOBAL: CARMA2_HW 0x0066fe48
br_diaghandler BrStdioDiagHandler = {

    "Stdio DiagHandler",
    BrStdioWarning,
    BrStdioFailure,
};


// GLOBAL: CARMA2_HW 0x0066fe54
br_diaghandler* _BrDefaultDiagHandler = &BrStdioDiagHandler;

#ifdef CARPOCALYPSE2_MATCHING
extern br_error C2_HOOK_CDECL BrEnd();
extern char gQuickTime_call_component_with_storage[];
#endif
// FUNCTION: CARMA2_HW 0x0053fa00
void C2_NAKED C2_HOOK_CDECL BrStdioWarning(char* message) {
#ifndef CARPOCALYPSE2_MATCHING

    fflush(stdout);
    fputs(message, stderr);
    fputc('\n', stderr);
    fflush(stderr);
#else
    __asm {
            push offset gQuickTime_call_component_with_storage+0x134
            call fflush
            mov eax, dword ptr [esp + 8]
            add esp, 4
            push offset gQuickTime_call_component_with_storage+0x154
            push eax
            call fputs
            add esp, 8
            push offset gQuickTime_call_component_with_storage+0x154
            push 0xa
            call fputc
            add esp, 8
            push offset gQuickTime_call_component_with_storage+0x154
            call fflush
            add esp, 4
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0053fa40
void C2_NAKED C2_HOOK_CDECL BrStdioFailure(char* message) {
#ifndef CARPOCALYPSE2_MATCHING

    fflush(stdout);
    fputs(message, stderr);
    fputc('\n', stderr);
    fflush(stderr);
#ifdef CARPOCALYPSE2_MATCHING
    exit(10);
#else
    abort();
#endif
#else
    __asm {
            call BrEnd
            push offset gQuickTime_call_component_with_storage+0x134
            call fflush
            mov eax, dword ptr [esp + 8]
            add esp, 4
            push offset gQuickTime_call_component_with_storage+0x154
            push eax
            call fputs
            add esp, 8
            push offset gQuickTime_call_component_with_storage+0x154
            push 0xa
            call fputc
            add esp, 8
            push offset gQuickTime_call_component_with_storage+0x154
            call fflush
            add esp, 4
            push 0xa
            call exit
            add esp, 4
            ret
    }
#endif
}
