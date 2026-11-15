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
#ifdef CARPOCALYPSE2_MATCHING
extern char gData_00662f18[];
extern char gFloat_00662e48[];
extern char gFloat_00662e68[];
extern char gFloat_00662ea8[];
extern char gFloat_00662ee8[];
extern char gFloat_00662f34[];
extern char gFloat_00662f50[];
extern char gFloat_00662f64[];
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern char gMatchData_00662f18[];
extern char gMatchFloat_00662e48[];
extern char gMatchFloat_00662e68[];
extern char gMatchFloat_00662ea8[];
extern char gMatchFloat_00662ee8[];
extern char gMatchFloat_00662f34[];
extern char gMatchFloat_00662f50[];
extern char gMatchFloat_00662f64[];
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
            push offset gMatchFloat_00662f64
            call dr_dprintf
            _emit 0x8b
        _emit 0x35
        _emit 0xf8
        _emit 0x91
        _emit 0x58
        _emit 0x00
            _emit 0x8b
        _emit 0x3d
        _emit 0x04
        _emit 0x92
        _emit 0x58
        _emit 0x00
            _emit 0x8b
        _emit 0x1d
        _emit 0x08
        _emit 0x92
        _emit 0x58
        _emit 0x00
            _emit 0x8b
        _emit 0x2d
        _emit 0x00
        _emit 0x92
        _emit 0x58
        _emit 0x00
            add esp, 4
    L0051cafc:
            cmp dword ptr [gWindowActiveState], 1
            jne L0051cb11
            mov eax, dword ptr [gHWnd]
            push eax
            _emit 0xff
        _emit 0x15
        _emit 0x10
        _emit 0x92
        _emit 0x58
        _emit 0x00
    L0051cb11:
            mov eax, dword ptr [gWindowActiveState]
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
            mov eax, dword ptr [gWindowActiveState]
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
            push offset gMatchFloat_00662f50
            call dr_dprintf
            mov eax, dword ptr [gWindowActiveState]
            add esp, 4
            cmp eax, 2
            jne L0051cb85
            push offset gMatchFloat_00662f34
            call dr_dprintf
            add esp, 4
            push offset gMatchData_00662f18
            call dr_dprintf
            add esp, 4
            call QuitGame
    L0051cb85:
            call PDShutdownSystem
    L0051cb8a:
            lea eax, [esp + 0x10]
            push eax
            call ebx
            push offset gMatchFloat_00662ee8
            call dr_dprintf
            add esp, 4
            lea ecx, [esp + 0x10]
            push ecx
            call ebp
            jmp L0051cafc
    L0051cbaa:
            push offset gMatchFloat_00662ea8
            jmp L0051cbb6
    L0051cbb1:
            push offset gMatchFloat_00662e68
    L0051cbb6:
            call dr_dprintf
            add esp, 4
            push offset gMatchFloat_00662e48
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
