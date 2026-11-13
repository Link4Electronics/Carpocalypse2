#include "win32_windows.h"

#include "win32.h"

#include "main.h"
#include "errors.h"
#include "packfile.h"
#include "platform.h"

#include <windows.h>

#ifdef CARPOCALYPSE2_MATCHING
extern C2_NORETURN void C2_HOOK_FASTCALL PDShutdownSystem(void);
extern C2_NORETURN void C2_HOOK_FASTCALL QuitGame(void);
extern void C2_HOOK_CDECL dr_dprintf();
#endif
// FUNCTION: CARMA2_HW 0x0051cad0
void C2_NAKED C2_HOOK_CDECL SDL3ServiceMessages(void) {
#ifndef CARPOCALYPSE2_MATCHING
    MSG msg;

    dr_dprintf("Win32ServiceMessages() - START");
    while (1) {
        if (gWindowActiveState == 1) {
            SetForegroundWindow(gHWnd);
        }

        if (gWindowActiveState) {
            if (PeekMessageA(&msg, NULL, 0, 0, 1) == 0) {
                dr_dprintf("Win32ServiceMessages() - breaking cos PeekMessage() returned 0");
                break;
            }
        }
        if (!gWindowActiveState) {
            if (GetMessageA(&msg, NULL, 0, 0) == -1) {
                dr_dprintf("Win32ServiceMessages() - breaking cos GetMessage() returned -1");
                break;
            }
        }
        if (msg.message == WM_QUIT) {
            dr_dprintf("WM_QUIT received.");
            if (gWindowActiveState == 2) {
                dr_dprintf("Active, so lock the surface");
                dr_dprintf("QuitGame being called...");
                QuitGame();
            }
            PDShutdownSystem();
        }
        TranslateMessage(&msg);
        dr_dprintf("Win32ServiceMessages() - dispatching message...");
        DispatchMessageA(&msg);
    }
    dr_dprintf("Win32ServiceMessages() - END");
#else
    __asm {
            sub esp, 0x1c
            push ebx
            push ebp
            push esi
            push edi
            push 0x662f64
            call dr_dprintf
            mov esi, dword ptr ds:[0x5891f8]
            mov edi, dword ptr ds:[0x589204]
            mov ebx, dword ptr ds:[0x589208]
            mov ebp, dword ptr ds:[0x589200]
            add esp, 4
    L0051cafc:
            cmp dword ptr ds:[gWindowActiveState], 1
            jne L0051cb11
            mov eax, dword ptr ds:[gHWnd]
            push eax
            call dword ptr ds:[0x589210]
    L0051cb11:
            mov eax, dword ptr ds:[gWindowActiveState]
            test eax, eax
            je L0051cb36
            push 1
            push 0
            push 0
            lea ecx, [esp + 0x1c]
            push 0
            push ecx
            call esi
            test eax, eax
            je L0051cbaa
            mov eax, dword ptr ds:[gWindowActiveState]
            test eax, eax
            jne L0051cb48
    L0051cb36:
            push 0
            push 0
            lea edx, [esp + 0x18]
            push 0
            push edx
            call edi
            cmp eax, -1
            je L0051cbb1
    L0051cb48:
            cmp dword ptr [esp + 0x14], 0x12
            jne L0051cb8a
            push 0x662f50
            call dr_dprintf
            mov eax, dword ptr ds:[gWindowActiveState]
            add esp, 4
            cmp eax, 2
            jne L0051cb85
            push 0x662f34
            call dr_dprintf
            add esp, 4
            push 0x662f18
            call dr_dprintf
            add esp, 4
            call QuitGame
    L0051cb85:
            call PDShutdownSystem
    L0051cb8a:
            lea eax, [esp + 0x10]
            push eax
            call ebx
            push 0x662ee8
            call dr_dprintf
            add esp, 4
            lea ecx, [esp + 0x10]
            push ecx
            call ebp
            jmp L0051cafc
    L0051cbaa:
            push 0x662ea8
            jmp L0051cbb6
    L0051cbb1:
            push 0x662e68
    L0051cbb6:
            call dr_dprintf
            add esp, 4
            push 0x662e48
            call dr_dprintf
            add esp, 4
            pop edi
            pop esi
            pop ebp
            pop ebx
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
            nop
            nop
            nop
            nop
            nop
    }
#endif
}


void C2_HOOK_FASTCALL PDDisposeActionReplayBuffer(void) {
    if (gActionReplayBufferFile != NULL) {
        PFfclose(gActionReplayBufferFile);
        gActionReplayBufferFile = NULL;
    }
}
