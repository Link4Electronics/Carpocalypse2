#include "win32.h"

#include "win32_dinput.h"
#include "win32_sound.h"
#include "win32_windows.h"
#include "win32_ssdx.h"

#include "drmem.h"
#include "errors.h"
#include "globvars.h"
#include "graphics2.h"
#include "init.h"
#include "input.h"
#include "main.h"
#include "errors.h"
#include "grafdata.h"
#include "platform.h"
#include "utility.h"
#include "packfile.h"

#include "world.h"

#include "carpocalypse2_macros.h"

#include "c2_direct.h"
#include <stdio.h>
#include "c2_string.h"

#include "brender/brender.h"
#include "carpocalypse2_types.h"

#include <direct.h>
#include <windows.h>
#include <dinput.h>


#ifndef GET_SC_WPARAM
#endif

#if defined(_MSC_VER) && _MSC_VER < 1300
#define GET_SC_WPARAM(wParam) ((int)wParam &0xfff0)
#define INVALID_FILE_ATTRIBUTES ((DWORD)-1)
typedef LONG LSTATUS;
#define __debugbreak() __asm int 3
#endif

// GLOBAL: CARMA2_HW 0x00662200
int gDefault_spec_index = 1;

// GLOBAL: CARMA2_HW 0x00662204
int gDefault_data_index = 1;

// GLOBAL: CARMA2_HW 0x00662208
tGraf_spec gGraf_specs[2] = {
    {8, 1, 0, 320, 200, 0, 0, "32X20X8", "320x200 init string", 320, 320, 200, 0},
    {8, 1, 0, 640, 480, 0, 0, "64X48X8", "640x480 init string", 640, 640, 480, 0},
};

// GLOBAL: CARMA2_HW 0x00762324
int gGraf_spec_index;

// GLOBAL: CARMA2_HW 0x0074ca60
int gNbPixelBits;

// GLOBAL: CARMA2_HW 0x0074ca94
int gUnknown_int_0074ca94;

// GLOBAL: CARMA2_HW 0x0074cf48
int gUnknown_int_0074cf48;


// GLOBAL: CARMA2_HW 0x006acc88
char gFatalErrorMessage[512];

// GLOBAL: CARMA2_HW 0x006ad498
int gIsFatalError;

// GLOBAL: CARMA2_HW 0x006ad494
int gExit_code;


// GLOBAL: CARMA2_HW 0x006ad4c8
void *gHWnd;

// GLOBAL: CARMA2_HW 0x006621e0
int gWindowActiveState = 2; // FIXME: enum: 0, 1 or 2


// GLOBAL: CARMA2_HW 0x006ad4d0
int gWindowMovingResizing;

// GLOBAL: CARMA2_HW 0x006acea0
LPDIRECTINPUTDEVICEA gDirectInputDevice;

// GLOBAL: CARMA2_HW 0x006621dc
char *gRenderer = "3DFX_WIN";

// GLOBAL: CARMA2_HW 0x006ad488
int gMouseLButtonDown;

// GLOBAL: CARMA2_HW 0x006ad48c
int gMouseRButtonDown;

// GLOBAL: CARMA2_HW 0x006ad490
int gMouseCaptured;

// GLOBAL: CARMA2_HW 0x006621d4
POINT gCursorPos_LastClick = {-1, -1};

// GLOBAL: CARMA2_HW 0x006ad458
POINT gPD_mouse_position;

// GLOBAL: CARMA2_HW 0x006ad454
int gCursorPos_LastClick_Valid;

// GLOBAL: CARMA2_HW 0x006ad468
int gScaleMouse;

// GLOBAL: CARMA2_HW 0x006ad460
int gScreenWidth;

// GLOBAL: CARMA2_HW 0x006ad464
int gScreenHeight;


// GLOBAL: CARMA2_HW 0x006ad4b0
int gKeyboardBufferLength;

// GLOBAL: CARMA2_HW 0x006ace88
unsigned char gKeyboardBuffer[20];

// GLOBAL: CARMA2_HW 0x006acc70
int gPerformanceCounterInitialized;

// GLOBAL: CARMA2_HW 0x006acc80
LARGE_INTEGER gPerformanceCounterStart;

// GLOBAL: CARMA2_HW 0x006acc68
LARGE_INTEGER gPerformanceCounterFrequency_s;

// GLOBAL: CARMA2_HW 0x006ad1f0
LARGE_INTEGER gPerformanceCounterFrequency_ms;

// GLOBAL: CARMA2_HW 0x006ac858
LARGE_INTEGER gPerformanceCounterFrequency_us;


// GLOBAL: CARMA2_HW 0x006ad49c
tU32 gTime_app_activated;


// GLOBAL: CARMA2_HW 0x006aceb0
br_diaghandler gPD_error_handler;

// GLOBAL: CARMA2_HW 0x00662150
char gExtendedAsciiToNormalAscii[128] = {
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
    ' ', '\'', '\'', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
    ' ', '~', 'c', '$', '*', 'Y', '|', ' ', '"', 'c', 'a', '<', '-', '-', 'R', '-',
    'o', ' ', '2', '3', '\'', 'u', ' ', '.', ',', '1', 'o', '>', ' ', ' ', ' ', '{',
    'A', 'A', 'A', 'A', 'A', 'A', 'A', 'C', 'E', 'E', 'E', 'E', 'I', 'I', 'I', 'I',
    'D', '}', 'O', 'O', 'O', 'O', 'O', 'x', '0', 'U', 'U', 'U', 'U', 'Y', 'b', 'B',
    'a', 'a', 'a', 'a', 'a', 'a', 'a', 'C', 'e', 'e', 'e', 'e', 'i', 'i', 'i', 'i',
    'd', '}', 'o', 'o', 'o', 'o', 'o', '/', '0', 'u', 'u', 'u', 'u', 'y', 'b', 'y',
};


// GLOBAL: CARMA2_HW 0x006ad4d4
int gWin32ActionReplayBufferAllocated;

// GLOBAL: CARMA2_HW 0x006ad470
void *gPDActionReplayBuffer;

// GLOBAL: CARMA2_HW 0x006ad474
int gPDActionReplayBufferSize;

// GLOBAL: CARMA2_HW 0x006ad46c
HPALETTE gPDPalette;

// GLOBAL: CARMA2_HW 0x006ad4cc
int gSignal_handler_entered;

// GLOBAL: CARMA2_HW 0x0079ea84
SYSTEM_INFO gSystem_info;

// GLOBAL: CARMA2_HW 0x0068c830
DWORD gPage_size;

// GLOBAL: CARMA2_HW 0x00681fa0
FILE *gActionReplayBufferFile;

#define Win32AllocateActionReplayBuffer PDReallyAllocateActionReplayBuffer

/* ==== Quit flag plumbing (mirrors the SDL3 platform) ==== */

static int menu_quit_requested;

int C2_HOOK_FASTCALL carpocalypse2_MenuQuitRequested(void) {
    return menu_quit_requested;
}

void C2_HOOK_FASTCALL carpocalypse2_RequestQuit(void) {
    menu_quit_requested = 1;
}

#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
/* Inline-asm bodies call the CRT 64-bit divide helper directly, so pull
   lldiv.obj into the link even when no C expression needs it. */
#pragma comment(linker, "/include:__alldiv")
extern void C2_HOOK_CDECL dr_dprintf();
extern char gRegionDescriminator[];
extern int gS3_CDA_enabled;
extern char gSocket[];
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern void* C2_HOOK_CDECL BrMemAllocate();
extern void C2_HOOK_FASTCALL CloseDiagnostics(void);
extern void C2_HOOK_FASTCALL CloseGlobalPackedFile(void);
extern int C2_HOOK_FASTCALL DRstrcmpi(const char* str1, const char* str2);
extern void C2_HOOK_FASTCALL PathCat(char* pDestn_str, const char* pStr_1, const char* pStr_2);
extern void SSDXRelease(void);
extern void SSDXStop(void);
extern void __cdecl _alldiv();
extern void __cdecl _ftol();
extern br_pixelmap* gBack_screen;
extern const char* gDir_separator;
extern char gDirectInput[];
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern char gData_006572c4[];
extern char gData_00662bcc[];
extern char gData_00662c44[];
extern char gData_007a1200[];
extern char gFloat_00662644[];
extern char gFloat_006627e8[];
extern char gFloat_0066293c[];
extern char gFloat_0066297c[];
extern char gFloat_006629d4[];
extern char gFloat_00662a24[];
extern char gFloat_00662a5c[];
extern char gFloat_00662bb0[];
extern char gFloat_00662be4[];
extern char gFloat_00662bfc[];
extern char gFloat_00662c18[];
extern char gFloat_00662c5c[];
extern char gFloat_00662c74[];
extern char gString_006572b8[];
extern char gString_006572c0[];
extern char gString_006620d0[];
extern char gString_00673698[];
extern char gString_006736a0[];
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern char gMatchData_006572c4[];
extern char gMatchData_00662bcc[];
extern char gMatchData_00662c44[];
extern char gMatchData_007a1200[];
extern char gMatchFloat_00662644[];
extern char gMatchFloat_006627e8[];
extern char gMatchFloat_0066293c[];
extern char gMatchFloat_0066297c[];
extern char gMatchFloat_006629d4[];
extern char gMatchFloat_00662a24[];
extern char gMatchFloat_00662a5c[];
extern char gMatchFloat_00662bb0[];
extern char gMatchFloat_00662be4[];
extern char gMatchFloat_00662bfc[];
extern char gMatchFloat_00662c18[];
extern char gMatchFloat_00662c5c[];
extern char gMatchFloat_00662c74[];
extern char gMatchString_006572b8[];
extern char gMatchString_006572c0[];
extern char gMatchString_006620d0[];
extern char gMatchString_00673698[];
extern char gMatchString_006736a0[];
#endif
// FUNCTION: CARMA2_HW 0x0051b810
void C2_NAKED C2_HOOK_FASTCALL Win32AllocateActionReplayBuffer(void) {
#ifndef CARPOCALYPSE2_MATCHING
    MEMORYSTATUS memory_status;
    unsigned int bufferSize;
    void *buffer;

    if (gWin32ActionReplayBufferAllocated) {
        return;
    }
    gWin32ActionReplayBufferAllocated = 1;
    memory_status.dwLength = sizeof(memory_status);
    GlobalMemoryStatus(&memory_status);
    dr_dprintf("Win32AllocateActionReplayBuffer(): Memory Status BEFORE Action Replay Allocation:\n"
               "             dwLength        %u\n"
               "             dwMemoryLoad    %u\n"
               "             dwTotalPhys     %u\n"
               "             dwAvailPhys     %u\n"
               "             dwTotalPageFile %u\n"
               "             dwAvailPageFile %u\n"
               "             dwTotalVirtual  %u\n"
               "             dwAvailVirtual  %u",
               memory_status.dwLength,
               memory_status.dwMemoryLoad,
               memory_status.dwTotalPhys,
               memory_status.dwAvailPhys,
               memory_status.dwTotalPageFile,
               memory_status.dwAvailPageFile,
               memory_status.dwTotalVirtual,
               memory_status.dwAvailVirtual);
    bufferSize = 8000000;
    if (memory_status.dwTotalPhys < 24000000) {
        bufferSize = 500000;
    } else if (memory_status.dwTotalPhys < 32000000) {
        bufferSize = 2000000;
    } else if (memory_status.dwTotalPhys < 48000000) {
        bufferSize = 4000000;
    }
    dr_dprintf("Win32AllocateActionReplayBuffer(): We want %d bytes...", bufferSize);
    if (memory_status.dwAvailPageFile + memory_status.dwAvailPhys < bufferSize) {
        bufferSize = memory_status.dwAvailPageFile + memory_status.dwAvailPhys - 0x100000;
        dr_dprintf("Win32AllocateActionReplayBuffer(): ...but there's only %d bytes available...", bufferSize);
    }
    if (bufferSize < 0x10000) {
        bufferSize = 0x10000;
        dr_dprintf("Win32AllocateActionReplayBuffer(): ...but we have to have a minimum size of %d bytes...",
                   bufferSize);
    }
    while (1) {
        buffer = BrMemAllocate(bufferSize, kMem_action_replay_buffer);
        if (buffer != NULL) {
            break;
        }
        bufferSize -= 0x10000;
    }
    gPDActionReplayBuffer = buffer;
    if (buffer == NULL) {
        gPDActionReplayBufferSize = 0;
    } else {
        gPDActionReplayBufferSize = bufferSize;
        Sleep(1000);
    }
    dr_dprintf("Win32AllocateActionReplayBuffer(): Actually allocated %d bytes.", bufferSize);
    GlobalMemoryStatus(&memory_status);
    dr_dprintf("Win32AllocateActionReplayBuffer(): Memory Status AFTER Action Replay Allocation:\n"
               "             dwLength        %u\n"
               "             dwMemoryLoad    %u\n"
               "             dwTotalPhys     %u\n"
               "             dwAvailPhys     %u\n"
               "             dwTotalPageFile %u\n"
               "             dwAvailPageFile %u\n"
               "             dwTotalVirtual  %u\n"
               "             dwAvailVirtual  %u",
               memory_status.dwLength,
               memory_status.dwMemoryLoad,
               memory_status.dwTotalPhys,
               memory_status.dwAvailPhys,
               memory_status.dwTotalPageFile,
               memory_status.dwAvailPageFile,
               memory_status.dwTotalVirtual,
               memory_status.dwAvailVirtual);
#else
    __asm {
            mov eax, dword ptr [gWin32ActionReplayBufferAllocated]
            sub esp, 0x20
            push edi
            xor edi, edi
            test eax, eax
            jne L0051b98e
            push esi
            lea eax, [esp + 8]
            push ebx
            _emit 0x8b
        _emit 0x1d
        _emit 0x44
        _emit 0x90
        _emit 0x58
        _emit 0x00
            push eax
            mov dword ptr [gWin32ActionReplayBufferAllocated], 1
            mov dword ptr [esp + 0x10], 0x20
            call ebx
            mov ecx, dword ptr [esp + 0x28]
            mov edx, dword ptr [esp + 0x24]
            mov eax, dword ptr [esp + 0x20]
            push ecx
            mov ecx, dword ptr [esp + 0x20]
            push edx
            mov edx, dword ptr [esp + 0x20]
            push eax
            mov eax, dword ptr [esp + 0x20]
            push ecx
            mov ecx, dword ptr [esp + 0x20]
            push edx
            mov edx, dword ptr [esp + 0x20]
            push eax
            push ecx
            push edx
            push offset gMatchFloat_00662a5c
            call dr_dprintf
            mov eax, dword ptr [esp + 0x38]
            add esp, 0x24
            cmp eax, 0x16e3600
            mov esi, offset gMatchData_007a1200
            jae L0051b890
            mov esi, 0x7a120
            jmp L0051b8aa
    L0051b890:
            cmp eax, 0x1e84800
            jae L0051b89e
            mov esi, 0x1e8480
            jmp L0051b8aa
    L0051b89e:
            cmp eax, 0x2dc6c00
            jae L0051b8aa
            mov esi, 0x3d0900
    L0051b8aa:
            push esi
            push offset gMatchFloat_00662a24
            call dr_dprintf
            mov eax, dword ptr [esp + 0x28]
            mov ecx, dword ptr [esp + 0x20]
            add eax, ecx
            add esp, 8
            cmp esi, eax
            jbe L0051b8da
            lea esi, [eax - 0x100000]
            push esi
            push offset gMatchFloat_006629d4
            call dr_dprintf
            add esp, 8
    L0051b8da:
            cmp esi, 0x10000
            jge L0051b8f9
            mov esi, 0x10000
            push esi
            push offset gMatchFloat_0066297c
            call dr_dprintf
            add esp, 8
            cmp esi, esi
            jl L0051b91b
    L0051b8f9:
            push 0xf4
            push esi
            call BrMemAllocate
            mov edi, eax
            add esp, 8
            test edi, edi
            jne L0051b91b
            sub esi, 0x10000
            cmp esi, 0x10000
            jge L0051b8f9
    L0051b91b:
            test edi, edi
            mov dword ptr [gPDActionReplayBuffer], edi
            je L0051b938
            push 0x3e8
            mov dword ptr [gPDActionReplayBufferSize], esi
            _emit 0xff
        _emit 0x15
        _emit 0x94
        _emit 0x90
        _emit 0x58
        _emit 0x00
            jmp L0051b942
    L0051b938:
            mov dword ptr [gPDActionReplayBufferSize], 0
    L0051b942:
            push esi
            push offset gMatchFloat_0066293c
            call dr_dprintf
            add esp, 8
            lea edx, [esp + 0xc]
            push edx
            call ebx
            mov eax, dword ptr [esp + 0x28]
            mov ecx, dword ptr [esp + 0x24]
            mov edx, dword ptr [esp + 0x20]
            push eax
            mov eax, dword ptr [esp + 0x20]
            push ecx
            mov ecx, dword ptr [esp + 0x20]
            push edx
            mov edx, dword ptr [esp + 0x20]
            push eax
            mov eax, dword ptr [esp + 0x20]
            push ecx
            mov ecx, dword ptr [esp + 0x20]
            push edx
            push eax
            push ecx
            push offset gMatchFloat_006627e8
            call dr_dprintf
            add esp, 0x24
            pop ebx
            pop esi
    L0051b98e:
            pop edi
            add esp, 0x20
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

// FUNCTION: CARMA2_HW 0x0051b9f0
void C2_HOOK_FASTCALL PDInitialiseSystem(void) {
    gBack_screen = NULL;
    gScreen = NULL;
    ShowCursor(FALSE);
    KeyBegin();
    Win32InitInputDevice();
}

// FUNCTION: CARMA2_HW 0x0051c6f0
void C2_HOOK_FASTCALL PDSetFileVariables(void) {
    gDir_separator = "\\";
}

// FUNCTION: CARMA2_HW 0x0051c700
void C2_HOOK_FASTCALL PDBuildAppPath(char *pThe_path) {
    GetCurrentDirectoryA(253, pThe_path);
    GetShortPathNameA(pThe_path, pThe_path, 253);
    strcat(pThe_path, "\\");
    dr_dprintf("Application path '%s'", pThe_path);
}

// FUNCTION: CARMA2_HW 0x0051af20
void C2_NORETURN C2_HOOK_FASTCALL PDFatalError(const char *pThe_str) {
    const char *str1;
    const char *str2;

    dr_dprintf("FATAL ERROR: %s", pThe_str);
    str1 = pThe_str;
    str2 = "";
    if (str1 == NULL) {
        str1 = "NULL str1";
    }
    if (str2 == NULL) {
        str2 = "NULL str2";
    }
    gIsFatalError = 1;
    sprintf(gFatalErrorMessage, "%s\n%s", str1, str2);

    gExit_code = 700;
    if (gBack_screen != NULL) {
        if (gBack_screen->pixels != NULL) {
            PossibleUnlock(1);
        }
    }
    if (gBr_initialized) {
        DRBrEnd();
    }
    PDShutdownSystem();
}

// FUNCTION: CARMA2_HW 0x0051afb0
void C2_HOOK_FASTCALL PDNonFatalError(const char *pThe_str) {
    dr_dprintf("*** ERROR...");
    dr_dprintf(pThe_str);
}

// FUNCTION: CARMA2_HW 0x0051afd0
int C2_HOOK_FASTCALL PDIsWindowInactive(void) {
    return gWindowActiveState == 1;
}

// FUNCTION: CARMA2_HW 0x0051afe0
char C2_NAKED C2_HOOK_FASTCALL PDConvertToASCIILessThan128(char pChar) {
#ifndef CARPOCALYPSE2_MATCHING
    if ((unsigned char) pChar >= 0x80) {
        char c = gExtendedAsciiToNormalAscii[(unsigned char) pChar - 128];
        dr_dprintf("PDConvertToASCIILessThan128() Returning %d", c);
        return c;
    }
    dr_dprintf("PDConvertToASCIILessThan128() Returning %d", pChar);
    return pChar;
#else
    __asm {
            push ecx
            push ebx
            mov bl, cl
            cmp bl, 0x80
            mov byte ptr [esp + 4], bl
            jb L0051b018
            push esi
            mov esi, dword ptr [esp + 8]
            and esi, 0xff
            xor eax, eax
            mov al, byte ptr [esi + gMatchString_006620d0]
            push eax
            push offset gMatchFloat_00662644
            call dr_dprintf
            mov al, byte ptr [esi + gMatchString_006620d0]
            add esp, 8
            pop esi
            pop ebx
            pop ecx
            ret
    L0051b018:
            mov ecx, dword ptr [esp + 4]
            and ecx, 0xff
            push ecx
            push offset gMatchFloat_00662644
            call dr_dprintf
            add esp, 8
            mov al, bl
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
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0051b040
int C2_HOOK_FASTCALL PDGetKeyboardCharacter(void) {
    unsigned int key;

    SDL3ServiceMessages();
    if (gKeyboardBufferLength != 0) {
        key = gKeyboardBuffer[0];
        if (gKeyboardBufferLength > 1) {
            memmove(&gKeyboardBuffer[0], &gKeyboardBuffer[1], gKeyboardBufferLength - 1);
        }
        gKeyboardBufferLength -= 1;
        dr_dprintf("KEY RETURNED %d", key);
        return key;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0051b0a0
void C2_HOOK_FASTCALL PDClearKeyboardBuffer(void) {
    dr_dprintf("KEYBOARD BUFFER CLEARED");
    gKeyboardBufferLength = 0;
}

// FUNCTION: CARMA2_HW 0x0051c110
C2_NORETURN void C2_NAKED C2_HOOK_FASTCALL PDShutdownSystem(void) {
#ifndef CARPOCALYPSE2_MATCHING
    // GLOBAL: CARMA2_HW 0x006ad4d8
    static int been_here;

    gBack_screen = NULL;
    if (been_here) {
        PDDisposeActionReplayBuffer();
        ExitProcess(702);
    } else {
        been_here = 1;
        dr_dprintf("PDShutdownSystem()");
        SSDXStop();
        SSDXRelease();
        CloseDirectInput();
        ShowCursor(TRUE);
        if (gHWnd != NULL) {
            dr_dprintf("Resizing main window...");
            SetWindowPos(gHWnd, NULL, -100, -100, 64, 64, SWP_NOSENDCHANGING | SWP_NOACTIVATE | SWP_NOZORDER);
        }
        dr_dprintf("Servicing messages...");
        SDL3ServiceMessages();
        dr_dprintf("Sending WM_SHOWWINDOW broadcast message...");
        SendMessageA(HWND_BROADCAST, WM_SHOWWINDOW, TRUE, 0);
        if (gIsFatalError) {
            dr_dprintf("Displaying fatal error...");
            MessageBoxA(NULL, gFatalErrorMessage, "Carmageddon Fatal error", MB_ICONERROR);
        }
        if (gHWnd != NULL) {
            dr_dprintf("Destroying window...");
            DestroyWindow(gHWnd);
            gHWnd = NULL;
        }
        dr_dprintf("End of PDShutdownSystem()");
        CloseDiagnostics();
        PDDisposeActionReplayBuffer();
        ExitProcess(gExit_code);
    }
#else
    __asm {
            _emit 0xa1
        _emit 0xd8
        _emit 0xd4
        _emit 0x6a
        _emit 0x00
            push esi
            xor esi, esi
            push edi
            _emit 0x8b
        _emit 0x3d
        _emit 0x5c
        _emit 0x90
        _emit 0x58
        _emit 0x00
            cmp eax, esi
            mov dword ptr [gBack_screen], esi
            jne L0051c259
            push offset gMatchFloat_00662c74
            _emit 0xc7
        _emit 0x05
        _emit 0xd8
        _emit 0xd4
        _emit 0x6a
        _emit 0x00
        _emit 0x01
        _emit 0x00
        _emit 0x00
        _emit 0x00
            call dr_dprintf
            add esp, 4
            call SSDXStop
            call SSDXRelease
            mov eax, dword ptr [gDirectInputDevice]
            cmp eax, esi
            je L0051c16e
            mov ecx, dword ptr [eax]
            push eax
            call dword ptr [ecx + 0x20]
            mov eax, dword ptr [gDirectInputDevice]
            push eax
            mov edx, dword ptr [eax]
            call dword ptr [edx + 8]
            mov dword ptr [gDirectInputDevice], esi
    L0051c16e:
            mov eax, dword ptr [gDirectInput]
            cmp eax, esi
            je L0051c183
            mov ecx, dword ptr [eax]
            push eax
            call dword ptr [ecx + 8]
            mov dword ptr [gDirectInput], esi
    L0051c183:
            push 1
            _emit 0xff
        _emit 0x15
        _emit 0xd4
        _emit 0x91
        _emit 0x58
        _emit 0x00
            cmp dword ptr [gHWnd], esi
            je L0051c1bb
            push offset gMatchFloat_00662c5c
            call dr_dprintf
            mov edx, dword ptr [gHWnd]
            add esp, 4
            push 0x414
            push 0x40
            push 0x40
            push -0x64
            push -0x64
            push esi
            push edx
            _emit 0xff
        _emit 0x15
        _emit 0x1c
        _emit 0x92
        _emit 0x58
        _emit 0x00
    L0051c1bb:
            push offset gMatchData_00662c44
            call dr_dprintf
            add esp, 4
            call SDL3ServiceMessages
            push offset gMatchFloat_00662c18
            call dr_dprintf
            add esp, 4
            push esi
            push 1
            push 0x18
            push 0xffff
            _emit 0xff
        _emit 0x15
        _emit 0x14
        _emit 0x92
        _emit 0x58
        _emit 0x00
            cmp dword ptr [gIsFatalError], esi
            je L0051c212
            push offset gMatchFloat_00662bfc
            call dr_dprintf
            add esp, 4
            push 0x10
            push offset gMatchFloat_00662be4
            push offset gFatalErrorMessage
            push esi
            _emit 0xff
        _emit 0x15
        _emit 0xdc
        _emit 0x91
        _emit 0x58
        _emit 0x00
    L0051c212:
            cmp dword ptr [gHWnd], esi
            je L0051c239
            push offset gMatchData_00662bcc
            call dr_dprintf
            mov eax, dword ptr [gHWnd]
            add esp, 4
            push eax
            _emit 0xff
        _emit 0x15
        _emit 0xfc
        _emit 0x91
        _emit 0x58
        _emit 0x00
            mov dword ptr [gHWnd], esi
    L0051c239:
            push offset gMatchFloat_00662bb0
            call dr_dprintf
            add esp, 4
            call CloseDiagnostics
            call CloseGlobalPackedFile
            mov ecx, dword ptr [gExit_code]
            push ecx
            call edi
    L0051c259:
            call CloseGlobalPackedFile
            push 0x2be
            call edi
            pop edi
            pop esi
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

// FUNCTION: CARMA2_HW 0x0051c2e0
void C2_HOOK_FASTCALL PDLockRealBackScreen(void) {
}

// FUNCTION: CARMA2_HW 0x0051c2f0
void C2_HOOK_FASTCALL PDUnlockRealBackScreen(void) {
}

void DeActivateApp(void) {
    dr_dprintf("DeActivateApp() - START");
    if (!gWindowMovingResizing && gWindowActiveState == 2) {
        dr_dprintf("DeActivateApp() - deactivating app");
        gWindowMovingResizing = 1;
        if (gDirectInputDevice != NULL) {
            IDirectInputDevice_Unacquire(gDirectInputDevice);
        }
        if (gHWnd != NULL) {
            SetWindowPos(gHWnd, NULL, 0, 0, 0, 0, SWP_NOSENDCHANGING | SWP_NOACTIVATE | SWP_NOZORDER);
        }
        gWindowActiveState = (strcmp(gRenderer, "D3D") == 0) ? 0 : 1;
        gWindowMovingResizing = 0;
    }
    dr_dprintf("DeActivateApp() - END; active state now %d", gWindowActiveState);
}

// FUNCTION: CARMA2_HW 0x0051b0c0
LRESULT C2_NAKED C2_HOOK_STDCALL Carma2MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
#ifndef CARPOCALYPSE2_MATCHING
    PAINTSTRUCT paint;
    POINT point;
    char buffer[256];

    switch (uMsg) {
        case WM_PAINT:
            BeginPaint(hWnd, &paint);
            EndPaint(hWnd, &paint);
            return 0;
        case WM_DESTROY:
            dr_dprintf("WM_DESTROY received - doing nothing.");
            break;
#if !defined(KEEP_ACTIVE_IN_BACKGROUND)
        case WM_MOVE:
        case WM_SIZE:
            if (IsIconic(hWnd)) {
                dr_dprintf("WM_SIZE/WM_MOVE: Window is iconic");
                DeActivateApp();
            }
            break;
#endif
        case WM_SYSCOMMAND:
            if (GET_SC_WPARAM(wParam) == SC_CLOSE) {
                DestroyWindow(gHWnd);
                gHWnd = NULL;
                PDShutdownSystem();
                return 0;
            }
            break;
        case WM_LBUTTONDOWN:
            gMouseLButtonDown = gWindowActiveState == 2 ? 1 : 0;
            if (!gMouseCaptured && gWindowActiveState == 2) {
                SetCapture(gHWnd);
                gMouseCaptured = 1;
            }
            break;
        case WM_LBUTTONUP:
            gMouseLButtonDown = 0;
            if (gMouseCaptured) {
                ReleaseCapture();
                gMouseCaptured = 0;
            }
            break;
        case WM_LBUTTONDBLCLK:
        case WM_RBUTTONDBLCLK:
            if (gWindowActiveState != 2) {
                gCursorPos_LastClick = gPD_mouse_position;
                gCursorPos_LastClick_Valid = 1;
                break;
            }
            GetCursorPos(&point);
            if (!gScaleMouse || strcmp(gRenderer, "D3D") == 0) {
                ScreenToClient(gHWnd, &point);
                gPD_mouse_position = gCursorPos_LastClick = point;
            } else {
                gPD_mouse_position.x = gCursorPos_LastClick.x = (int) ((double) point.x / (gScreenWidth / 640.));
                gPD_mouse_position.y = gCursorPos_LastClick.y = (int) ((double) point.y / (gScreenHeight / 480.));
            }
            gCursorPos_LastClick_Valid = 1;
            break;
        case WM_RBUTTONDOWN:
            gMouseRButtonDown = gWindowActiveState == 2 ? 1 : 0;
            if (!gMouseCaptured && gWindowActiveState == 2) {
                SetCapture(gHWnd);
                gMouseCaptured = 1;
            }
            break;
        case WM_RBUTTONUP:
            gMouseRButtonDown = 0;
            if (gMouseCaptured) {
                ReleaseCapture();
                gMouseCaptured = 0;
            }
            break;
        case WM_CHAR:
            if (0x1f < (char) wParam && (char) wParam != 0x7f) {
                if (gKeyboardBufferLength >= CARPOCALYPSE2_ASIZE(gKeyboardBuffer)) {
                    memmove(gKeyboardBuffer, &gKeyboardBuffer[1], CARPOCALYPSE2_ASIZE(gKeyboardBuffer) - 1);
                    gKeyboardBufferLength = CARPOCALYPSE2_ASIZE(gKeyboardBuffer) - 1;
                }
                gKeyboardBuffer[gKeyboardBufferLength] = (unsigned char) wParam;
                gKeyboardBufferLength++;
                strncpy(buffer, (char *) gKeyboardBuffer, gKeyboardBufferLength);
                buffer[gKeyboardBufferLength] = '\0';
                dr_dprintf("KEY PRESSED, BUFFER NOW IS: '%s'", buffer);
            }
            break;
#if !defined(KEEP_ACTIVE_IN_BACKGROUND)
        case WM_ACTIVATEAPP:
            dr_dprintf(
                "WM_ACTIVATEAPP: wparam is %d, lparam is %d, fg window is %p, main win is %p, hWnd is %p, isiconic is %d",
                wParam, lParam, GetForegroundWindow(), gHWnd, hWnd, IsIconic(gHWnd));
            if (gHWnd != NULL) {
                if (GetForegroundWindow() == gHWnd && !IsIconic(gHWnd)) {
                    dr_dprintf("Activating app");
                    if (gDirectInputDevice != NULL) {
                        IDirectInputDevice_Acquire(gDirectInputDevice);
                    }
                    if (gHWnd != NULL && gScreenWidth != 0) {
                        SetWindowPos(gHWnd, NULL, 0, 0, gScreenWidth, gScreenHeight,
                                     SWP_NOSENDCHANGING | SWP_NOACTIVATE | SWP_NOZORDER);
                    }
                    gWindowActiveState = 2;
                    gTime_app_activated = PDGetTotalTime();
                } else {
                    if (gBack_screen != NULL && gBack_screen->pixels != NULL) {
                        return 1;
                    }
                    DeActivateApp();
                }
            }
            break;
#endif
        default:
            break;
    }
    return DefWindowProcA(hWnd, uMsg, wParam, lParam);
#else
    __asm {
            push ebp
            mov ebp, esp
            sub esp, 0x154
            push ebx
            push esi
            push edi
            mov edi, dword ptr [ebp + 0xc]
            cmp edi, 0xf
            ja L0051b215
            je L0051b1f1
            mov eax, edi
            sub eax, 2
            je L0051b1df
            dec eax
            je L0051b0f5
            sub eax, 2
            jne L0051b367
    L0051b0f5:
            mov eax, dword ptr [ebp + 8]
            push eax
            _emit 0xff
        _emit 0x15
        _emit 0x0c
        _emit 0x92
        _emit 0x58
        _emit 0x00
            test eax, eax
            je L0051b367
            push 0x6627c4
            call dr_dprintf
            add esp, 4
            push 0x6627ac
            call dr_dprintf
            mov eax, dword ptr [gWindowMovingResizing]
            xor ebx, ebx
            add esp, 4
            cmp eax, ebx
            jne L0051b1c6
            cmp dword ptr [gWindowActiveState], 2
            jne L0051b1c6
            push 0x662788
            call dr_dprintf
            mov eax, dword ptr [gDirectInputDevice]
            add esp, 4
            cmp eax, ebx
            mov dword ptr [gWindowMovingResizing], 1
            je L0051b163
            mov ecx, dword ptr [eax]
            push eax
            call dword ptr [ecx + 0x20]
    L0051b163:
            mov eax, dword ptr [gHWnd]
            cmp eax, ebx
            je L0051b17d
            push 0x414
            push ebx
            push ebx
            push ebx
            push ebx
            push ebx
            push eax
            _emit 0xff
        _emit 0x15
        _emit 0x1c
        _emit 0x92
        _emit 0x58
        _emit 0x00
    L0051b17d:
            mov eax, dword ptr [gRenderer]
            mov esi, 0x662518
    L0051b187:
            mov dl, byte ptr [eax]
            mov cl, dl
            cmp dl, byte ptr [esi]
            jne L0051b1ab
            cmp cl, bl
            je L0051b1a7
            mov dl, byte ptr [eax + 1]
            mov cl, dl
            cmp dl, byte ptr [esi + 1]
            jne L0051b1ab
            add eax, 2
            add esi, 2
            cmp cl, bl
            jne L0051b187
    L0051b1a7:
            xor eax, eax
            jmp L0051b1b0
    L0051b1ab:
            sbb eax, eax
            sbb eax, -1
    L0051b1b0:
            mov edi, dword ptr [ebp + 0xc]
            xor ecx, ecx
            cmp eax, ebx
            mov dword ptr [gWindowMovingResizing], ebx
            setne cl
            mov dword ptr [gWindowActiveState], ecx
    L0051b1c6:
            mov edx, dword ptr [gWindowActiveState]
            push edx
            push 0x66275c
            call dr_dprintf
            add esp, 8
            jmp L0051b367
    L0051b1df:
            push 0x662734
            call dr_dprintf
            add esp, 4
            jmp L0051b367
    L0051b1f1:
            mov esi, dword ptr [ebp + 8]
            lea eax, [ebp - 0x54]
            push eax
            push esi
            _emit 0xff
        _emit 0x15
        _emit 0x20
        _emit 0x92
        _emit 0x58
        _emit 0x00
            lea ecx, [ebp - 0x54]
            push ecx
            push esi
            _emit 0xff
        _emit 0x15
        _emit 0x18
        _emit 0x92
        _emit 0x58
        _emit 0x00
            xor eax, eax
            pop edi
            pop esi
            pop ebx
            mov esp, ebp
            pop ebp
            ret 0x10
    L0051b215:
            cmp edi, 0x102
            ja L0051b4f8
            je L0051b478
            cmp edi, 0x1c
            jne L0051b367
            mov edx, dword ptr [gHWnd]
            _emit 0x8b
        _emit 0x3d
        _emit 0x0c
        _emit 0x92
        _emit 0x58
        _emit 0x00
            push edx
            call edi
            mov ecx, dword ptr [gHWnd]
            _emit 0x8b
        _emit 0x35
        _emit 0x28
        _emit 0x92
        _emit 0x58
        _emit 0x00
            push eax
            mov eax, dword ptr [ebp + 8]
            push eax
            push ecx
            call esi
            mov edx, dword ptr [ebp + 0x14]
            push eax
            mov eax, dword ptr [ebp + 0x10]
            push edx
            push eax
            push 0x6626cc
            call dr_dprintf
            mov eax, dword ptr [gHWnd]
            xor ebx, ebx
            add esp, 0x1c
            cmp eax, ebx
            je L0051b364
            call esi
            mov ecx, dword ptr [gHWnd]
            cmp eax, ecx
            jne L0051b396
            push ecx
            call edi
            test eax, eax
            jne L0051b396
            push 0x6626bc
            call dr_dprintf
            mov eax, dword ptr [gDirectInputDevice]
            add esp, 4
            cmp eax, ebx
            je L0051b2af
            mov ecx, dword ptr [eax]
            push eax
            call dword ptr [ecx + 0x1c]
    L0051b2af:
            mov ecx, dword ptr [gHWnd]
            cmp ecx, ebx
            je L0051b2d9
            mov eax, dword ptr [gScreenWidth]
            cmp eax, ebx
            je L0051b2d9
            mov edx, dword ptr [gScreenHeight]
            push 0x414
            push edx
            push eax
            push ebx
            push ebx
            push ebx
            push ecx
            _emit 0xff
        _emit 0x15
        _emit 0x1c
        _emit 0x92
        _emit 0x58
        _emit 0x00
    L0051b2d9:
            cmp dword ptr [gPerformanceCounterInitialized], ebx
            je L0051b383
            lea eax, [ebp - 0x14]
            push eax
            _emit 0xff
        _emit 0x15
        _emit 0x40
        _emit 0x90
        _emit 0x58
        _emit 0x00
            mov eax, dword ptr [ebp - 0x10]
            mov esi, 0x10
            cdq
            mov dword ptr [ebp - 8], eax
            mov dword ptr [ebp - 4], edx
            mov dword ptr [ebp - 0xc], esi
            mov ecx, dword ptr [ebp - 0xc]
            mov eax, dword ptr [ebp - 8]
            mov edx, dword ptr [ebp - 4]
            shld edx, eax, cl
            shl eax, cl
            mov dword ptr [ebp - 4], edx
            mov dword ptr [ebp - 8], eax
            mov dword ptr [ebp - 0xc], esi
            mov ecx, dword ptr [ebp - 0xc]
            mov eax, dword ptr [ebp - 8]
            mov edx, dword ptr [ebp - 4]
            shld edx, eax, cl
            shl eax, cl
            mov ecx, dword ptr [ebp - 0x14]
            xor esi, esi
            add ecx, eax
            mov eax, dword ptr [gPerformanceCounterStart+0x4]
            adc esi, edx
            mov edx, dword ptr [gPerformanceCounterStart]
            sub ecx, edx
            mov edx, dword ptr [gPerformanceCounterFrequency_ms+0x4]
            sbb esi, eax
            mov eax, dword ptr [gPerformanceCounterFrequency_ms]
            push edx
            push eax
            push esi
            push ecx
            call _alldiv
            mov dword ptr [ebp - 4], edx
            mov dword ptr [gTime_app_activated], eax
            mov dword ptr [gWindowActiveState], 2
    L0051b364:
            mov edi, dword ptr [ebp + 0xc]
    L0051b367:
            mov edx, dword ptr [ebp + 0x14]
            mov eax, dword ptr [ebp + 0x10]
            mov ecx, dword ptr [ebp + 8]
            push edx
            push eax
            push edi
            push ecx
            _emit 0xff
        _emit 0x15
        _emit 0x2c
        _emit 0x92
        _emit 0x58
        _emit 0x00
            pop edi
            pop esi
            pop ebx
            mov esp, ebp
            pop ebp
            ret 0x10
    L0051b383:
            xor eax, eax
            mov dword ptr [gWindowActiveState], 2
            mov dword ptr [gTime_app_activated], eax
            jmp L0051b364
    L0051b396:
            mov eax, dword ptr [gBack_screen]
            cmp eax, ebx
            je L0051b3b2
            cmp dword ptr [eax + 8], ebx
            je L0051b3b2
            mov eax, 1
            pop edi
            pop esi
            pop ebx
            mov esp, ebp
            pop ebp
            ret 0x10
    L0051b3b2:
            push 0x6627ac
            call dr_dprintf
            mov eax, dword ptr [gWindowMovingResizing]
            add esp, 4
            cmp eax, ebx
            jne L0051b45f
            cmp dword ptr [gWindowActiveState], 2
            jne L0051b45f
            push 0x662788
            call dr_dprintf
            mov eax, dword ptr [gDirectInputDevice]
            add esp, 4
            cmp eax, ebx
            mov dword ptr [gWindowMovingResizing], 1
            je L0051b3ff
            mov ecx, dword ptr [eax]
            push eax
            call dword ptr [ecx + 0x20]
    L0051b3ff:
            mov eax, dword ptr [gHWnd]
            cmp eax, ebx
            je L0051b419
            push 0x414
            push ebx
            push ebx
            push ebx
            push ebx
            push ebx
            push eax
            _emit 0xff
        _emit 0x15
        _emit 0x1c
        _emit 0x92
        _emit 0x58
        _emit 0x00
    L0051b419:
            mov eax, dword ptr [gRenderer]
            mov esi, 0x662518
    L0051b423:
            mov dl, byte ptr [eax]
            mov cl, dl
            cmp dl, byte ptr [esi]
            jne L0051b447
            cmp cl, bl
            je L0051b443
            mov dl, byte ptr [eax + 1]
            mov cl, dl
            cmp dl, byte ptr [esi + 1]
            jne L0051b447
            add eax, 2
            add esi, 2
            cmp cl, bl
            jne L0051b423
    L0051b443:
            xor eax, eax
            jmp L0051b44c
    L0051b447:
            sbb eax, eax
            sbb eax, -1
    L0051b44c:
            xor ecx, ecx
            cmp eax, ebx
            setne cl
            mov dword ptr [gWindowActiveState], ecx
            mov dword ptr [gWindowMovingResizing], ebx
    L0051b45f:
            mov edx, dword ptr [gWindowActiveState]
            push edx
            push 0x66275c
            call dr_dprintf
            add esp, 8
            jmp L0051b364
    L0051b478:
            mov bl, byte ptr [ebp + 0x10]
            cmp bl, 0x20
            jb L0051b367
            cmp bl, 0x7f
            je L0051b367
            mov eax, dword ptr [gKeyboardBufferLength]
            cmp eax, 0x14
            jl L0051b4b0
            push 0x13
            push offset gKeyboardBuffer+0x1
            push offset gKeyboardBuffer
            call memmove
            add esp, 0xc
            mov eax, 0x13
    L0051b4b0:
            mov byte ptr [eax + gKeyboardBuffer], bl
            inc eax
            mov dword ptr [gKeyboardBufferLength], eax
            push eax
            lea eax, [ebp - 0x154]
            push offset gKeyboardBuffer
            push eax
            call strncpy
            mov ecx, dword ptr [gKeyboardBufferLength]
            add esp, 0xc
            lea edx, [ebp - 0x154]
            push edx
            push 0x662698
            mov byte ptr [ebp + ecx - 0x154], 0
            call dr_dprintf
            add esp, 8
            jmp L0051b367
    L0051b4f8:
            lea eax, [edi - 0x112]
            cmp eax, 0xf4
            ja L0051b367
            xor ecx, ecx
            mov cl, byte ptr [eax + 0x51b710]
            jmp dword ptr [ecx*4 + 0x51b6f0]
            cmp dword ptr [gWindowActiveState], 2
            je L0051b546
            mov edx, dword ptr [gPD_mouse_position]
            mov eax, dword ptr [gPD_mouse_position+0x4]
            mov dword ptr [gCursorPos_LastClick], edx
            mov dword ptr [gCursorPos_LastClick+0x4], eax
            mov dword ptr [gCursorPos_LastClick_Valid], 1
            jmp L0051b367
    L0051b546:
            lea ecx, [ebp - 8]
            push ecx
            _emit 0xff
        _emit 0x15
        _emit 0x24
        _emit 0x92
        _emit 0x58
        _emit 0x00
            mov eax, dword ptr [gScaleMouse]
            xor ebx, ebx
            cmp eax, ebx
            je L0051b5d3
            mov eax, dword ptr [gRenderer]
            mov esi, 0x662518
    L0051b565:
            mov dl, byte ptr [eax]
            mov cl, dl
            cmp dl, byte ptr [esi]
            jne L0051b589
            cmp cl, bl
            je L0051b585
            mov dl, byte ptr [eax + 1]
            mov cl, dl
            cmp dl, byte ptr [esi + 1]
            jne L0051b589
            add eax, 2
            add esi, 2
            cmp cl, bl
            jne L0051b565
    L0051b585:
            xor eax, eax
            jmp L0051b58e
    L0051b589:
            sbb eax, eax
            sbb eax, -1
    L0051b58e:
            cmp eax, ebx
            je L0051b5d0
            fild dword ptr [ebp - 8]
            fild dword ptr [gScreenWidth]
            _emit 0xdc
        _emit 0x0d
        _emit 0x48
        _emit 0xb7
        _emit 0x58
        _emit 0x00
            fdivp st(1), st(0)
            call _ftol
            fild dword ptr [ebp - 4]
            fild dword ptr [gScreenHeight]
            mov esi, eax
            mov dword ptr [gCursorPos_LastClick], esi
            _emit 0xdc
        _emit 0x0d
        _emit 0x50
        _emit 0xb7
        _emit 0x58
        _emit 0x00
            fdivp st(1), st(0)
            call _ftol
            mov edi, dword ptr [ebp + 0xc]
            mov dword ptr [gCursorPos_LastClick+0x4], eax
            jmp L0051b5f5
    L0051b5d0:
            mov edi, dword ptr [ebp + 0xc]
    L0051b5d3:
            mov ecx, dword ptr [gHWnd]
            lea eax, [ebp - 8]
            push eax
            push ecx
            _emit 0xff
        _emit 0x15
        _emit 0xb8
        _emit 0x91
        _emit 0x58
        _emit 0x00
            mov esi, dword ptr [ebp - 8]
            mov eax, dword ptr [ebp - 4]
            mov dword ptr [gCursorPos_LastClick], esi
            mov dword ptr [gCursorPos_LastClick+0x4], eax
    L0051b5f5:
            mov dword ptr [gPD_mouse_position], esi
            mov dword ptr [gPD_mouse_position+0x4], eax
            mov dword ptr [gCursorPos_LastClick_Valid], 1
            jmp L0051b367
            mov eax, dword ptr [gWindowActiveState]
            mov ecx, dword ptr [gMouseCaptured]
            xor edx, edx
            cmp eax, 2
            sete dl
            test ecx, ecx
            mov dword ptr [gMouseLButtonDown], edx
            jne L0051b367
            cmp eax, 2
            jne L0051b367
            mov eax, dword ptr [gHWnd]
            push eax
            jmp L0051b67c
            xor ebx, ebx
            mov dword ptr [gMouseLButtonDown], ebx
            jmp L0051b699
            mov eax, dword ptr [gWindowActiveState]
            xor ecx, ecx
            cmp eax, 2
            sete cl
            mov dword ptr [gMouseRButtonDown], ecx
            mov ecx, dword ptr [gMouseCaptured]
            test ecx, ecx
            jne L0051b367
            cmp eax, 2
            jne L0051b367
            mov edx, dword ptr [gHWnd]
            push edx
    L0051b67c:
            _emit 0xff
        _emit 0x15
        _emit 0x30
        _emit 0x92
        _emit 0x58
        _emit 0x00
            mov dword ptr [gMouseCaptured], 1
            jmp L0051b367
            xor ebx, ebx
            mov dword ptr [gMouseRButtonDown], ebx
    L0051b699:
            cmp dword ptr [gMouseCaptured], ebx
            je L0051b367
            _emit 0xff
        _emit 0x15
        _emit 0xbc
        _emit 0x91
        _emit 0x58
        _emit 0x00
            mov dword ptr [gMouseCaptured], ebx
            jmp L0051b367
            mov eax, dword ptr [ebp + 0x10]
            and eax, 0xfff0
            cmp eax, 0xf060
            jne L0051b367
            mov ecx, dword ptr [gHWnd]
            push ecx
            _emit 0xff
        _emit 0x15
        _emit 0xfc
        _emit 0x91
        _emit 0x58
        _emit 0x00
            mov dword ptr [gHWnd], 0
            call PDShutdownSystem
            pop edi
            pop esi
            xor eax, eax
            pop ebx
            mov esp, ebp
            pop ebp
            ret 0x10
            _emit 0xb6
            _emit 0xb6
            _emit 0x51
            _emit 0x00
            _emit 0x0f
            _emit 0xb6
            _emit 0x51
            _emit 0x00
            _emit 0x41
            _emit 0xb6
            _emit 0x51
            _emit 0x00
            _emit 0x18
            _emit 0xb5
            _emit 0x51
            _emit 0x00
            _emit 0x4b
            _emit 0xb6
            _emit 0x51
            _emit 0x00
            _emit 0x91
            _emit 0xb6
            _emit 0x51
            _emit 0x00
            _emit 0x18
            _emit 0xb5
            _emit 0x51
            _emit 0x00
            _emit 0x67
            _emit 0xb3
            _emit 0x51
            _emit 0x00
            _emit 0x00
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x07
            _emit 0x01
            _emit 0x02
            _emit 0x03
            _emit 0x04
            _emit 0x05
            _emit 0x06
            _emit 0x90
            _emit 0x90
            _emit 0x90
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

// FUNCTION: CARMA2_HW 0x0051cab0
int C2_HOOK_FASTCALL PDGetASCIIFromKey(int pKey) {
    if (PDKeyDown(0)) {
        return gASCII_shift_table[pKey];
    } else {
        return gASCII_table[pKey];
    }
}

// FUNCTION: CARMA2_HW 0x0051d500
int C2_NAKED C2_HOOK_FASTCALL PDCheckDriveExists2(const char *pThe_path, const char *pFile_name, tU32 pMin_size) {
#ifndef CARPOCALYPSE2_MATCHING
    char the_path[256];
    tU32 file_size;
    HANDLE hFile;

    file_size = 0;
    if (pFile_name != NULL) {
        PathCat(the_path, pThe_path, pFile_name);
    } else {
        strcpy(the_path, pThe_path);
    }
    if (the_path[0] && the_path[1] == ':' && the_path[2] == '\0') {
        strcat(the_path, gDir_separator);
    }
    if (GetFileAttributesA(pThe_path) == INVALID_FILE_ATTRIBUTES) {
        return 0;
    }
    hFile = CreateFileA(the_path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile != INVALID_HANDLE_VALUE) {
        file_size = GetFileSize(hFile, NULL);
        CloseHandle(hFile);
    }
    return file_size >= pMin_size;
#else
    __asm {
            sub esp, 0x100
            push ebx
            push ebp
            xor ebp, ebp
            push esi
            test edx, edx
            push edi
            mov ebx, ecx
            je L0051d520
            push edx
            mov edx, ebx
            lea ecx, [esp + 0x14]
            call PathCat
            jmp L0051d543
    L0051d520:
            mov edi, ebx
            or ecx, 0xffffffff
            xor eax, eax
            lea edx, [esp + 0x10]
            repne scasb
            not ecx
            sub edi, ecx
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            shr ecx, 2
            rep movsd
            mov ecx, eax
            and ecx, 3
            rep movsb
    L0051d543:
            mov al, byte ptr [esp + 0x10]
            test al, al
            je L0051d589
            cmp byte ptr [esp + 0x11], 0x3a
            jne L0051d589
            mov al, byte ptr [esp + 0x12]
            test al, al
            jne L0051d589
            mov edi, dword ptr [gDir_separator]
            or ecx, 0xffffffff
            xor eax, eax
            lea edx, [esp + 0x10]
            repne scasb
            not ecx
            sub edi, ecx
            mov esi, edi
            mov edi, edx
            mov edx, ecx
            or ecx, 0xffffffff
            repne scasb
            mov ecx, edx
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, edx
            and ecx, 3
            rep movsb
    L0051d589:
            push ebx
            _emit 0xff
        _emit 0x15
        _emit 0x80
        _emit 0x90
        _emit 0x58
        _emit 0x00
            cmp eax, -1
            je L0051d5e2
            push 0
            push 0x80
            push 3
            push 0
            push 0
            lea eax, [esp + 0x24]
            push 0x80000000
            push eax
            _emit 0xff
        _emit 0x15
        _emit 0x64
        _emit 0x90
        _emit 0x58
        _emit 0x00
            mov esi, eax
            cmp esi, -1
            je L0051d5cb
            push 0
            push esi
            _emit 0xff
        _emit 0x15
        _emit 0x60
        _emit 0x90
        _emit 0x58
        _emit 0x00
            mov ebp, eax
            push esi
            _emit 0xff
        _emit 0x15
        _emit 0xb0
        _emit 0x90
        _emit 0x58
        _emit 0x00
    L0051d5cb:
            cmp ebp, dword ptr [esp + 0x114]
            sbb eax, eax
            inc eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x100
            ret 4
    L0051d5e2:
            pop edi
            pop esi
            pop ebp
            xor eax, eax
            pop ebx
            add esp, 0x100
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
            nop
            nop
    }
#endif
}

int C2_HOOK_FASTCALL PDReadSourceLocation(tPath_name pPath) {
    HKEY hKey;
    LSTATUS status;
    char buffer[256];
    DWORD lenBuffer;

    status = RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\SCI\\CARMAGEDDON2", 0, KEY_ALL_ACCESS, &hKey);
    if (status == ERROR_SUCCESS) {
        lenBuffer = sizeof(tPath_name) - 1; // FIXME: use array_sizeof
        status = RegQueryValueExA(hKey, "SourceLocation", NULL, NULL, (LPBYTE) pPath, &lenBuffer);
        if (status == ERROR_SUCCESS) {
            RegCloseKey(hKey);
            return 1;
        }
        FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, NULL, status, 0, buffer, sizeof(buffer) - 1, NULL);
        RegCloseKey(hKey);
    }
    return 0;
}


// FUNCTION: CARMA2_HW 0x0051d600
int C2_HOOK_FASTCALL PDDoWeLeadAnAustereExistance(void) {
    return 0;
}

// FUNCTION: CARMA2_HW 0x0051c850
void C2_HOOK_FASTCALL PDSetPaletteEntries(br_pixelmap *pPalette, int pFirst_colour, int pCount) {
    tU32 *pixels = pPalette->pixels;
    PALETTEENTRY colours[256];
    int i;
    for (i = 0; i < pCount; i++) {
        tU32 c;

        c = pixels[pFirst_colour + i];
        colours[i].peFlags = 0;
        colours[i].peRed = c >> 16;
        colours[i].peGreen = c >> 8;
        colours[i].peBlue = pixels[pFirst_colour + i];
    }
    SetPaletteEntries(gPDPalette, pFirst_colour, pCount, colours);
    BrPixelmapDoubleBuffer(gScreen, gBack_screen);
}

// FUNCTION: CARMA2_HW 0x0051c840
void C2_HOOK_FASTCALL PDSetPalette(br_pixelmap *pixelmap) {
    BrPixelmapPaletteSet(gScreen, pixelmap);
}

// FUNCTION: CARMA2_HW 0x0051cbe0
int C2_HOOK_FASTCALL PDServiceSystem(tU32 pTime_since_last_call) {
    SDL3ServiceMessages();
    return 0;
}

// FUNCTION: CARMA2_HW 0x0051c520
void C2_HOOK_FASTCALL PDScreenBufferSwap(int pRendering_area_only) {
    BrPixelmapDoubleBuffer(gScreen, gBack_screen);
}

// FUNCTION: CARMA2_HW 0x0051d7c0
void C2_NAKED C2_HOOK_FASTCALL PDInitTimer(void) {
#ifndef CARPOCALYPSE2_MATCHING
    LARGE_INTEGER freq;
    LARGE_INTEGER current_count;


    gPerformanceCounterInitialized = 0;
    if (QueryPerformanceFrequency(&freq) != 0) {
        gPerformanceCounterInitialized = 1;
        gPerformanceCounterFrequency_s = freq;
        QueryPerformanceCounter(&current_count);
        gPerformanceCounterStart = current_count;
        gPerformanceCounterFrequency_ms.QuadPart = gPerformanceCounterFrequency_s.QuadPart / 1000;
        gPerformanceCounterFrequency_us.QuadPart = gPerformanceCounterFrequency_s.QuadPart / 1000000;
    }
#else
    __asm {
            push ebp
            mov ebp, esp
            sub esp, 0x14
            lea eax, [ebp - 0xc]
            mov dword ptr [gPerformanceCounterInitialized], 0
            push eax
            _emit 0xff
        _emit 0x15
        _emit 0xa8
        _emit 0x90
        _emit 0x58
        _emit 0x00
            test eax, eax
            je L0051d8ef
            mov eax, dword ptr [ebp - 8]
            push edi
            push esi
            mov esi, 0x10
            cdq
            mov dword ptr [gPerformanceCounterInitialized], 1
            mov dword ptr [gPerformanceCounterFrequency_s], eax
            mov dword ptr [gPerformanceCounterFrequency_s+0x4], edx
            mov dword ptr [ebp - 4], esi
            mov ecx, dword ptr [ebp - 4]
            mov eax, dword ptr [gPerformanceCounterFrequency_s]
            mov edx, dword ptr [gPerformanceCounterFrequency_s+0x4]
            shld edx, eax, cl
            shl eax, cl
            mov dword ptr [gPerformanceCounterFrequency_s+0x4], edx
            mov dword ptr [gPerformanceCounterFrequency_s], eax
            mov dword ptr [ebp - 4], esi
            mov ecx, dword ptr [ebp - 4]
            mov eax, dword ptr [gPerformanceCounterFrequency_s]
            mov edx, dword ptr [gPerformanceCounterFrequency_s+0x4]
            shld edx, eax, cl
            shl eax, cl
            mov ecx, dword ptr [ebp - 0xc]
            add eax, ecx
            lea ecx, [ebp - 0x14]
            adc edx, 0
            push ecx
            mov dword ptr [gPerformanceCounterFrequency_s], eax
            mov dword ptr [gPerformanceCounterFrequency_s+0x4], edx
            _emit 0xff
        _emit 0x15
        _emit 0x40
        _emit 0x90
        _emit 0x58
        _emit 0x00
            mov eax, dword ptr [ebp - 0x10]
            mov dword ptr [ebp - 4], esi
            cdq
            mov dword ptr [gPerformanceCounterStart], eax
            mov dword ptr [gPerformanceCounterStart+0x4], edx
            mov ecx, dword ptr [ebp - 4]
            mov eax, dword ptr [gPerformanceCounterStart]
            mov edx, dword ptr [gPerformanceCounterStart+0x4]
            shld edx, eax, cl
            shl eax, cl
            mov dword ptr [gPerformanceCounterStart+0x4], edx
            mov dword ptr [gPerformanceCounterStart], eax
            mov dword ptr [ebp - 4], esi
            mov ecx, dword ptr [ebp - 4]
            mov eax, dword ptr [gPerformanceCounterStart]
            mov edx, dword ptr [gPerformanceCounterStart+0x4]
            shld edx, eax, cl
            shl eax, cl
            mov ecx, dword ptr [ebp - 0x14]
            mov esi, dword ptr [gPerformanceCounterFrequency_s+0x4]
            mov edi, dword ptr [gPerformanceCounterFrequency_s]
            add eax, ecx
            push 0
            push 0x3e8
            adc edx, 0
            push esi
            push edi
            mov dword ptr [gPerformanceCounterStart], eax
            mov dword ptr [gPerformanceCounterStart+0x4], edx
            call _alldiv
            mov dword ptr [gPerformanceCounterFrequency_ms], eax
            mov dword ptr [gPerformanceCounterFrequency_ms+0x4], edx
            push 0
            push 0xf4240
            push esi
            push edi
            call _alldiv
            mov dword ptr [gPerformanceCounterFrequency_us], eax
            mov dword ptr [gPerformanceCounterFrequency_us+0x4], edx
            pop esi
            pop edi
    L0051d8ef:
            mov esp, ebp
            pop ebp
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

// FUNCTION: CARMA2_HW 0x0051d410
tU32 C2_NAKED C2_HOOK_FASTCALL PDGetTotalTime(void) {
#ifndef CARPOCALYPSE2_MATCHING
    LARGE_INTEGER perfCountValue;
    LARGE_INTEGER delta;

    if (gPerformanceCounterInitialized) {
        QueryPerformanceCounter(&perfCountValue);
        delta.QuadPart = perfCountValue.QuadPart - gPerformanceCounterStart.QuadPart;
        return (tU32) (delta.QuadPart / gPerformanceCounterFrequency_ms.QuadPart);
    }
    return 0;
#else
    __asm {
            push ebp
            mov ebp, esp
            sub esp, 0x14
            mov eax, dword ptr [gPerformanceCounterInitialized]
            test eax, eax
            je L0051d495
            lea eax, [ebp - 0xc]
            push esi
            push eax
            _emit 0xff
        _emit 0x15
        _emit 0x40
        _emit 0x90
        _emit 0x58
        _emit 0x00
            mov eax, dword ptr [ebp - 8]
            mov esi, 0x10
            cdq
            mov dword ptr [ebp - 0x14], eax
            mov dword ptr [ebp - 0x10], edx
            mov dword ptr [ebp - 4], esi
            mov ecx, dword ptr [ebp - 4]
            mov eax, dword ptr [ebp - 0x14]
            mov edx, dword ptr [ebp - 0x10]
            shld edx, eax, cl
            shl eax, cl
            mov dword ptr [ebp - 0x10], edx
            mov dword ptr [ebp - 0x14], eax
            mov dword ptr [ebp - 4], esi
            mov ecx, dword ptr [ebp - 4]
            mov eax, dword ptr [ebp - 0x14]
            mov edx, dword ptr [ebp - 0x10]
            shld edx, eax, cl
            shl eax, cl
            mov ecx, dword ptr [ebp - 0xc]
            xor esi, esi
            add ecx, eax
            mov eax, dword ptr [gPerformanceCounterStart]
            adc esi, edx
            mov edx, dword ptr [gPerformanceCounterStart+0x4]
            sub ecx, eax
            mov eax, dword ptr [gPerformanceCounterFrequency_ms]
            sbb esi, edx
            mov edx, dword ptr [gPerformanceCounterFrequency_ms+0x4]
            push edx
            push eax
            push esi
            push ecx
            call _alldiv
            mov dword ptr [ebp - 0x10], edx
            pop esi
            mov esp, ebp
            pop ebp
            ret
    L0051d495:
            xor eax, eax
            mov esp, ebp
            pop ebp
            ret
            nop
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0051d990
int C2_NAKED C2_HOOK_FASTCALL PDGetMicroseconds(void) {
#ifndef CARPOCALYPSE2_MATCHING
    if (gPerformanceCounterInitialized) {
        LARGE_INTEGER perfCountValue;
        QueryPerformanceCounter(&perfCountValue);
        // Is it okay to convert unsigned to int here?
        return (int) ((perfCountValue.QuadPart - gPerformanceCounterStart.QuadPart) / gPerformanceCounterFrequency_us.
                      QuadPart);
    }
    return 0;
#else
    __asm {
            push ebp
            mov ebp, esp
            sub esp, 0x14
            mov eax, dword ptr [gPerformanceCounterInitialized]
            test eax, eax
            je L0051da17
            lea eax, [ebp - 0xc]
            push esi
            push eax
            _emit 0xff
        _emit 0x15
        _emit 0x40
        _emit 0x90
        _emit 0x58
        _emit 0x00
            mov eax, dword ptr [ebp - 8]
            mov esi, 0x10
            cdq
            mov dword ptr [ebp - 0x14], eax
            mov dword ptr [ebp - 0x10], edx
            mov dword ptr [ebp - 4], esi
            mov ecx, dword ptr [ebp - 4]
            mov eax, dword ptr [ebp - 0x14]
            mov edx, dword ptr [ebp - 0x10]
            shld edx, eax, cl
            shl eax, cl
            mov dword ptr [ebp - 0x10], edx
            mov dword ptr [ebp - 0x14], eax
            mov dword ptr [ebp - 4], esi
            mov ecx, dword ptr [ebp - 4]
            mov eax, dword ptr [ebp - 0x14]
            mov edx, dword ptr [ebp - 0x10]
            shld edx, eax, cl
            shl eax, cl
            mov ecx, dword ptr [ebp - 0xc]
            xor esi, esi
            add eax, ecx
            mov ecx, dword ptr [gPerformanceCounterStart+0x4]
            adc edx, esi
            mov esi, dword ptr [gPerformanceCounterStart]
            sub eax, esi
            sbb edx, ecx
            mov ecx, dword ptr [gPerformanceCounterFrequency_us+0x4]
            push ecx
            mov ecx, dword ptr [gPerformanceCounterFrequency_us]
            push ecx
            push edx
            push eax
            call _alldiv
            mov dword ptr [ebp - 0x10], edx
            pop esi
            mov esp, ebp
            pop ebp
            ret
    L0051da17:
            xor eax, eax
            mov esp, ebp
            pop ebp
            ret
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0051a6b0
void C2_HOOK_FASTCALL SignalHandler(int pSignal) {
    char buffer[256];

    if (gSignal_handler_entered == 0) {
        strcpy(buffer, "*** SIGNAL RECEIVED: ");
        gSignal_handler_entered = 1;
        switch (pSignal) {
        case 0x16:
            strcat(buffer, "SIGABRT");
            break;
        case 8:
            strcat(buffer, "SIGFPE");
            break;
        case 4:
            strcat(buffer, "SIGILL");
            break;
        case 0xb:
            strcat(buffer, "SIGSEGV");
            break;
        case 0xf:
            strcat(buffer, "SIGTERM");
            break;
        }
        dr_dprintf(buffer);
        PDShutdownSystem();
    }
    ExitProcess(0x2bd);
}

// FUNCTION: CARMA2_HW 0x0051d900
tU32 C2_NAKED C2_HOOK_FASTCALL PDGetElapsedMilliseconds(void) {
#ifndef CARPOCALYPSE2_MATCHING
    LARGE_INTEGER perfCountValue;
    unsigned __int64 elapsed;
    volatile int shift;

    if (gPerformanceCounterInitialized) {
        QueryPerformanceCounter(&perfCountValue);
        elapsed = perfCountValue.HighPart;
        shift = 0x10;
        elapsed <<= shift;
        elapsed <<= shift;
        elapsed += perfCountValue.LowPart;
        elapsed -= (unsigned __int64)gPerformanceCounterStart.QuadPart;
        return (tU32)(elapsed / (unsigned __int64)gPerformanceCounterFrequency_ms.QuadPart);
    }
    return 0;
#else
    __asm {
            push ebp
            mov ebp, esp
            sub esp, 0x14
            mov eax, dword ptr [gPerformanceCounterInitialized]
            test eax, eax
            je L0051d987
            lea eax, [ebp - 0xc]
            push esi
            push eax
            _emit 0xff
        _emit 0x15
        _emit 0x40
        _emit 0x90
        _emit 0x58
        _emit 0x00
            mov eax, dword ptr [ebp - 8]
            mov esi, 0x10
            cdq
            mov dword ptr [ebp - 0x14], eax
            mov dword ptr [ebp - 0x10], edx
            mov dword ptr [ebp - 4], esi
            mov ecx, dword ptr [ebp - 4]
            mov eax, dword ptr [ebp - 0x14]
            mov edx, dword ptr [ebp - 0x10]
            shld edx, eax, cl
            shl eax, cl
            mov dword ptr [ebp - 0x10], edx
            mov dword ptr [ebp - 0x14], eax
            mov dword ptr [ebp - 4], esi
            mov ecx, dword ptr [ebp - 4]
            mov eax, dword ptr [ebp - 0x14]
            mov edx, dword ptr [ebp - 0x10]
            shld edx, eax, cl
            shl eax, cl
            mov ecx, dword ptr [ebp - 0xc]
            xor esi, esi
            add eax, ecx
            mov ecx, dword ptr [gPerformanceCounterStart+0x4]
            adc edx, esi
            mov esi, dword ptr [gPerformanceCounterStart]
            sub eax, esi
            sbb edx, ecx
            mov ecx, dword ptr [gPerformanceCounterFrequency_ms+0x4]
            push ecx
            mov ecx, dword ptr [gPerformanceCounterFrequency_ms]
            push ecx
            push edx
            push eax
            call _alldiv
            mov dword ptr [ebp - 0x10], edx
            pop esi
            mov esp, ebp
            pop ebp
            ret
    L0051d987:
            xor eax, eax
            mov esp, ebp
            pop ebp
            ret
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0051a7b0
void C2_HOOK_FASTCALL PDEnterDebugger(const char *pStr) {
    dr_dprintf("PDEnterDebugger(): %s", pStr);
    ShowCursor(TRUE);
#ifndef CARPOCALYPSE2_MATCHING
    __debugbreak();
#endif
    ShowCursor(FALSE);
}

// FUNCTION: CARMA2_HW 0x0051d4b0
int C2_HOOK_FASTCALL PDFileUnlock(const char *pThe_path) {
    DWORD dwAttributes;

    dwAttributes = GetFileAttributesA(pThe_path);
    if (dwAttributes != INVALID_FILE_ATTRIBUTES) {
        return SetFileAttributesA(pThe_path, dwAttributes & ~FILE_ATTRIBUTE_READONLY);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00486c30
int C2_NAKED C2_HOOK_FASTCALL PDEnumPath(const char *path, tEnumPathCallback pCallback, void *data) {
#ifndef CARPOCALYPSE2_MATCHING
    char originalCurrentDirectory[MAX_PATH];
    tPath_name filePath;
    BOOL bSucceeded;
    WIN32_FIND_DATAA findFileData;
    HANDLE hFindFile;
    size_t lenFilename;
    size_t lenLnk;
    int callback_res;

    callback_res = 0;
    GetCurrentDirectory(CARPOCALYPSE2_ASIZE(originalCurrentDirectory), originalCurrentDirectory);
    bSucceeded = SetCurrentDirectoryA(path);
    if (!bSucceeded) {
        return 0;
    }
    hFindFile = FindFirstFileA("*.???", &findFileData);
    if (hFindFile != NULL) {
        while (1) {
            if ((findFileData.dwFileAttributes & (
                     FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_HIDDEN)) == 0) {
                lenFilename = strlen(findFileData.cFileName);
                lenLnk = strlen(".lnk");
                if (lenFilename > lenLnk && strcasecmp(&findFileData.cFileName[lenFilename - lenLnk], ".lnk") == 0) {
                    memmove(filePath, findFileData.cFileName, lenFilename - lenLnk);
                    filePath[lenFilename - lenLnk] = '\0';
                } else {
                    memmove(filePath, findFileData.cFileName, lenFilename + 1);
                }
                callback_res = pCallback(filePath, data);
                if (callback_res != 0) {
                    break;
                }
            }
            bSucceeded = FindNextFileA(hFindFile, &findFileData);
            if (!bSucceeded) {
                break;
            }
        }
        FindClose(hFindFile);
    }
    SetCurrentDirectoryA(originalCurrentDirectory);
    return callback_res;
#else
    __asm {
            sub esp, 0x348
            lea eax, [esp + 0x244]
            push ebp
            push esi
            push edi
            push eax
            mov ebp, edx
            mov edi, ecx
            push 0x104
            xor esi, esi
            _emit 0xff
        _emit 0x15
        _emit 0x68
        _emit 0x90
        _emit 0x58
        _emit 0x00
            push edi
            _emit 0xff
        _emit 0x15
        _emit 0x84
        _emit 0x90
        _emit 0x58
        _emit 0x00
            test eax, eax
            je L00486d4a
            lea ecx, [esp + 0x10]
            push ecx
            push offset gMatchString_006572c0
            _emit 0xff
        _emit 0x15
        _emit 0x88
        _emit 0x90
        _emit 0x58
        _emit 0x00
            cmp eax, -1
            mov dword ptr [esp + 0xc], eax
            je L00486d3c
            push ebx
            mov ebx, dword ptr [esp + 0x35c]
    L00486c86:
            test byte ptr [esp + 0x14], 0x16
            jne L00486d14
            lea edi, [esp + 0x40]
            or ecx, 0xffffffff
            xor eax, eax
            repne scasb
            not ecx
            dec ecx
            mov edi, offset gMatchString_006572b8
            mov esi, ecx
            or ecx, 0xffffffff
            repne scasb
            not ecx
            dec ecx
            mov edi, ecx
            cmp esi, edi
            jl L00486cf0
            lea ecx, [esp + esi + 0x40]
            mov edx, offset gMatchString_006572b8
            sub ecx, edi
            call DRstrcmpi
            test eax, eax
            jne L00486cf0
            mov edx, esi
            lea eax, [esp + 0x40]
            sub edx, edi
            lea ecx, [esp + 0x154]
            push edx
            push eax
            push ecx
            call memmove
            lea edx, [esp + esi + 0x160]
            add esp, 0xc
            sub edx, edi
            mov byte ptr [edx], 0
            jmp L00486d07
    L00486cf0:
            inc esi
            lea eax, [esp + 0x40]
            push esi
            lea ecx, [esp + 0x158]
            push eax
            push ecx
            call memmove
            add esp, 0xc
    L00486d07:
            mov edx, ebx
            lea ecx, [esp + 0x154]
            call ebp
            mov esi, eax
    L00486d14:
            test esi, esi
            jne L00486d30
            mov eax, dword ptr [esp + 0x10]
            lea edx, [esp + 0x14]
            push edx
            push eax
            _emit 0xff
        _emit 0x15
        _emit 0x8c
        _emit 0x90
        _emit 0x58
        _emit 0x00
            test eax, eax
            jne L00486c86
    L00486d30:
            mov ecx, dword ptr [esp + 0x10]
            push ecx
            _emit 0xff
        _emit 0x15
        _emit 0x90
        _emit 0x90
        _emit 0x58
        _emit 0x00
            pop ebx
    L00486d3c:
            lea edx, [esp + 0x250]
            push edx
            _emit 0xff
        _emit 0x15
        _emit 0x84
        _emit 0x90
        _emit 0x58
        _emit 0x00
    L00486d4a:
            mov eax, esi
            pop edi
            pop esi
            pop ebp
            add esp, 0x348
            ret 4
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

// FUNCTION: CARMA2_HW 0x00486c20
int C2_HOOK_FASTCALL PDmkdir(const char *path) {
    return c2_mkdir(path);
}

// FUNCTION: CARMA2_HW 0x0044c9f0
void C2_HOOK_FASTCALL MAMSUnlock(void **pPtr) {
    void *mem = *pPtr;
    if (mem == NULL) {
        FatalError(kFatalError_OOM_S, "NULL POINTER BEING FREED");
    }
    HeapFree(GetProcessHeap(), 0, mem);
    *pPtr = NULL;
}

// FUNCTION: CARMA2_HW 0x0044ca30
void C2_HOOK_FASTCALL MAMSLock(void **pPtr) {
}

// FUNCTION: CARMA2_HW 0x0051d640
void C2_HOOK_FASTCALL PDForEveryFileRecurse(const char *pThe_path, tPDForEveryFileRecurse_cbfn pAction_routine) {
    char found_path[256];
    WIN32_FIND_DATAA find_data;
    HANDLE hFindFile;
    char file_filter[256];
    char current_dir[260];

    GetCurrentDirectoryA(sizeof(current_dir), current_dir);
    if (SetCurrentDirectoryA(pThe_path)) {
        dr_dprintf("*PDForEveryFileRecurse() - NEW DIRECTORY '%s'", pThe_path);
        strcpy(file_filter, "*");
        hFindFile = FindFirstFileA(file_filter, &find_data);
        if (hFindFile != INVALID_HANDLE_VALUE) {
            do {
                if (strcmp(find_data.cFileName, ".") != 0 && strcmp(find_data.cFileName, "..") != 0) {
                    PathCat(found_path, pThe_path, find_data.cFileName);
                    SetCurrentDirectoryA(current_dir);
                    if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                        PDForEveryFileRecurse(found_path, pAction_routine);
                    } else {
                        dr_dprintf("PDForEveryFileRecurse() - Calling action proc with file '%s' in dir '%s'",
                                   found_path, current_dir);
                        pAction_routine(found_path);
                    }
                    SetCurrentDirectoryA(pThe_path);
                }
            } while (FindNextFileA(hFindFile, &find_data) != 0);
            FindClose(hFindFile);
        }
        SetCurrentDirectoryA(current_dir);
    }
}

// FUNCTION: CARMA2_HW 0x0051c760
void C2_NAKED C2_HOOK_FASTCALL PDForEveryFile(const char *pThe_path, tPDForEveryFile_cbfn pAction_routine) {
#ifndef CARPOCALYPSE2_MATCHING
    char found_path[256];
    WIN32_FIND_DATAA find_data;
    HANDLE hFindFile;
    char file_filter[6];
    char current_dir[260];

    GetCurrentDirectoryA(sizeof(current_dir), current_dir);
    if (SetCurrentDirectoryA(pThe_path)) {
        strcpy(file_filter, "*.???");
        hFindFile = FindFirstFileA(file_filter, &find_data);
        if (hFindFile != INVALID_HANDLE_VALUE) {
            do {
                PathCat(found_path, pThe_path, find_data.cFileName);
                pAction_routine(found_path);
            } while (FindNextFileA(hFindFile, &find_data) != 0);
            FindClose(hFindFile);
        }
        SetCurrentDirectoryA(current_dir);
    }
#else
    __asm {
            sub esp, 0x444
            lea eax, [esp + 0x340]
            push ebx
            push esi
            push eax
            mov ebx, edx
            mov esi, ecx
            push 0x104
            _emit 0xff
        _emit 0x15
        _emit 0x68
        _emit 0x90
        _emit 0x58
        _emit 0x00
            push esi
            _emit 0xff
        _emit 0x15
        _emit 0x84
        _emit 0x90
        _emit 0x58
        _emit 0x00
            test eax, eax
            je L0051c833
            mov ecx, dword ptr [gMatchString_006572c0]
            mov dx, word ptr [gMatchData_006572c4]
            mov dword ptr [esp + 0x248], ecx
            lea eax, [esp + 8]
            push edi
            lea ecx, [esp + 0x24c]
            push eax
            push ecx
            mov word ptr [esp + 0x258], dx
            _emit 0xff
        _emit 0x15
        _emit 0x88
        _emit 0x90
        _emit 0x58
        _emit 0x00
            mov edi, eax
            cmp edi, -1
            je L0051c824
            lea edx, [esp + 0x38]
            push ebp
            push edx
            mov edx, esi
            lea ecx, [esp + 0x154]
            call PathCat
            lea ecx, [esp + 0x150]
            call ebx
            _emit 0x8b
        _emit 0x2d
        _emit 0x8c
        _emit 0x90
        _emit 0x58
        _emit 0x00
            lea eax, [esp + 0x10]
            push eax
            push edi
            call ebp
            test eax, eax
            je L0051c81c
    L0051c7f4:
            lea ecx, [esp + 0x3c]
            mov edx, esi
            push ecx
            lea ecx, [esp + 0x154]
            call PathCat
            lea ecx, [esp + 0x150]
            call ebx
            lea edx, [esp + 0x10]
            push edx
            push edi
            call ebp
            test eax, eax
            jne L0051c7f4
    L0051c81c:
            push edi
            _emit 0xff
        _emit 0x15
        _emit 0x90
        _emit 0x90
        _emit 0x58
        _emit 0x00
            pop ebp
    L0051c824:
            lea eax, [esp + 0x34c]
            push eax
            _emit 0xff
        _emit 0x15
        _emit 0x84
        _emit 0x90
        _emit 0x58
        _emit 0x00
            pop edi
    L0051c833:
            pop esi
            pop ebx
            add esp, 0x444
            ret
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0051da20
void C2_HOOK_FASTCALL PDGetCurrentDate(char *pTimeStr) {
    SYSTEMTIME time;
    char buffer[256];

    GetLocalTime(&time);
    GetDateFormatA(LOCALE_SYSTEM_DEFAULT, LOCALE_NOUSEROVERRIDE, &time, NULL, buffer, sizeof(buffer));
    strcpy(pTimeStr, buffer);
}

// FUNCTION: CARMA2_HW 0x0051da90
void C2_HOOK_FASTCALL PDGetCurrentTime(char *pTimeStr) {
    SYSTEMTIME time;
    char buffer[256];

    C2_HOOK_BUG_ON(LOCALE_SYSTEM_DEFAULT != 0x800);
    C2_HOOK_BUG_ON(LOCALE_NOUSEROVERRIDE != 0x80000000);

    GetLocalTime(&time);
    GetTimeFormatA(LOCALE_SYSTEM_DEFAULT, LOCALE_NOUSEROVERRIDE, &time, NULL, buffer, sizeof(buffer));
    strcpy(pTimeStr, buffer);
}

// FUNCTION: CARMA2_HW 0x0051db00
int C2_NAKED C2_HOOK_FASTCALL PDGetMouseClickPosition(int *pX_coord, int *pY_coord) {
#ifndef CARPOCALYPSE2_MATCHING
    if (!gCursorPos_LastClick_Valid) {
        gCursorPos_LastClick_Valid = 0;
        return 0;
    }
    *pX_coord = gCursorPos_LastClick.x;
    *pY_coord = gCursorPos_LastClick.y;
    gCursorPos_LastClick_Valid = 0;
    return 1;
#else
    __asm {
            push esi
            mov esi, dword ptr [gCursorPos_LastClick_Valid]
            xor eax, eax
            cmp esi, eax
            je L0051db29
            mov esi, dword ptr [gCursorPos_LastClick]
            mov dword ptr [ecx], esi
            mov ecx, dword ptr [gCursorPos_LastClick+0x4]
            mov dword ptr [edx], ecx
            mov dword ptr [gCursorPos_LastClick_Valid], eax
            mov eax, 1
            pop esi
            ret
    L0051db29:
            mov dword ptr [gCursorPos_LastClick_Valid], eax
            xor eax, eax
            pop esi
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
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0051c5f0
static void C2_HOOK_CDECL OnWarnCallback(char *text) {
    dr_dprintf("*******************************************************************************");
    dr_dprintf("BRender WARNING: '%s'", text);
    dr_dprintf("*******************************************************************************");
}

// FUNCTION: CARMA2_HW 0x0051c620
static void C2_HOOK_CDECL OnErrorCallback(char *text) {
    const char *str1;
    const char *str2;

    dr_dprintf("*******************************************************************************");
    dr_dprintf("BRender FAILURE: '%s'", text);
    dr_dprintf("*******************************************************************************");
    str1 = "BRender error detected:";
    str2 = text;
    if (str1 == NULL) {
        str1 = "NULL str1";
    }
    if (str2 == NULL) {
        str2 = "NULL str2";
    }
    gIsFatalError = 1;
    sprintf(gFatalErrorMessage, "%s\n%s", str1, str2);
    gExit_code = 700;
    if (gBack_screen != NULL && gBack_screen->pixels != NULL) {
        PossibleUnlock(1);
    }
    if (gBr_initialized) {
        DRBrEnd();
    }
    PDShutdownSystem();
}

// FUNCTION: CARMA2_HW 0x0051c6c0
void C2_HOOK_FASTCALL PDInstallErrorHandlers(void) {
    gPD_error_handler.identifier = "LlantisilioBlahBlahBlahOgOgOch";
    gPD_error_handler.warning = OnWarnCallback;
    gPD_error_handler.failure = OnErrorCallback;
    BrDiagHandlerSet(&gPD_error_handler);
}

// FUNCTION: CARMA2_HW 0x0051c290
int C2_HOOK_FASTCALL PDInitScreenVars(int pArgc, const char **pArgv) {
    gGraf_spec_index = gDefault_spec_index;
    gGraf_data_index = gDefault_spec_index;
    return 1;
}

// FUNCTION: CARMA2_HW 0x0051c270
void C2_HOOK_FASTCALL PDSaveOriginalPalette(void) {
}

// FUNCTION: CARMA2_HW 0x0051c2b0
void C2_HOOK_FASTCALL PDInitScreen(void) {
}

// FUNCTION: CARMA2_HW 0x0051c300
void C2_HOOK_FASTCALL PDAllocateScreenAndBack(void) {
    dr_dprintf("PDAllocateScreenAndBack() - START...");

    dr_dprintf("Setting up DirectSound stuff...");

    SSDXStart(gHWnd, 0, 1, SSDX_InitDirectSound);

    gNbPixelBits = 16;
    BrDevBeginVar(&gScreen, gRenderer,
        BRT_WINDOW_FULLSCREEN_B, 1,
        BRT_WINDOW_HANDLE_H, gHWnd,
        BRT_WIDTH_I32, 640,
        BRT_HEIGHT_I32, 480,
        BRT_PIXEL_BITS_I32, 16,
        BRT_PIXEL_TYPE_U8, BR_PMT_RGB_565,
        BR_NULL_TOKEN);
    if (gScreen == NULL) {
        BrFailure("Unable to allocate Main Front Screen");
    }

    if (strcmp(gScreen->identifier, "Voodoo Graphics") == 0) {
        dr_dprintf("%s: lock seldom", gScreen->identifier);
    } else {
        dr_dprintf("%s: lock often", gScreen->identifier);
        gLock_often = 1;
    }
    gShadow_workaround = 1; /* Not 100% sure this is shadow related */
    gEnable_texture_interpolation = 1;
    gEnable_texture_antialiasing = 1;
    gTexture_power_of_2 = 1;
    gUnknown_int_0074ca94 = 8;
    gUnknown_int_0074cf48 = 256;
    gAdapt_sky_model_for_cockpit = 1;
    gNo_fog = 1; /* Enables lut for horizon material */
    gRenderer_fixup_basename = "VOODOO";
    gRenderer_fixup_extension = ".TXT";
    gNo_render_indent = 1;
    gNo_2d_effects = 1; /* maybe? */
    gEnable_perspective_maps = 1;
    gNoTransients = 1;
    gDevious_2d = 1;

    gScreen->origin_x = 0;
    gScreen->origin_y = 0;
    gBack_screen = BrPixelmapMatch(gScreen, BR_PMMATCH_OFFSCREEN);
    if (gBack_screen == NULL) {
        BrFailure("Unable to allocate Back Screen");
    }

    gPixelFlags = kPixelFlags_unknown_0x4 + (gNbPixelBits == 16 ? kPixelFlags_16bbp : 0);

    gDouble_back_screen = BrPixelmapMatch(gBack_screen, BR_PMMATCH_OFFSCREEN);
    gHas_double_back_screen = 1;

    memset(gBack_screen->pixels, 0, gBack_screen->height * gBack_screen->row_bytes);

    BrPixelmapDoubleBuffer(gScreen, gBack_screen);

    dr_dprintf("PDAllocateScreenAndBack() - END.");
}

int PDGetRegisterSourceLocation(char *buffer, int *buffer_size) {
    HKEY hKey;
    LONG status;

    status = RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\SCI\\CARMAGEDDON2", 0, KEY_ALL_ACCESS, &hKey);
    if (status == ERROR_SUCCESS) {
        status = RegQueryValueExA(hKey, "SourceLocation", NULL, NULL, (LPBYTE) buffer, (LPDWORD) buffer_size);
        if (status == ERROR_SUCCESS) {
            RegCloseKey(hKey);
            return 1;
        } else {
            char message[256];
            FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, NULL, status, 0, message, sizeof(message) - 1, NULL);
            RegCloseKey(hKey);
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0051c900
void C2_NAKED C2_HOOK_FASTCALL PDGetMousePosition(int *pX, int *pY) {
#ifndef CARPOCALYPSE2_MATCHING
    POINT pnt;

    if (gWindowActiveState != 2) {
        *pX = gPD_mouse_position.x;
        *pY = gPD_mouse_position.y;
        return;
    }
    GetCursorPos(&pnt);
    ScreenToClient(gHWnd, &pnt);
    *pX = pnt.x;
    *pY = pnt.y;
    gPD_mouse_position.x = *pX;
    gPD_mouse_position.y = *pY;
#else
    __asm {
            mov eax, dword ptr [gWindowActiveState]
            sub esp, 8
            cmp eax, 2
            push ebp
            push edi
            mov edi, edx
            mov ebp, ecx
            je L0051c929
            mov eax, dword ptr [gPD_mouse_position]
            mov dword ptr [ebp], eax
            mov ecx, dword ptr [gPD_mouse_position+0x4]
            mov dword ptr [edi], ecx
            pop edi
            pop ebp
            add esp, 8
            ret
    L0051c929:
            push esi
            lea edx, [esp + 0xc]
            push ebx
            push edx
            _emit 0xff
        _emit 0x15
        _emit 0x24
        _emit 0x92
        _emit 0x58
        _emit 0x00
            mov eax, dword ptr [gScaleMouse]
            test eax, eax
            jmp L0051c9ad
            mov eax, dword ptr [gRenderer]
            mov esi, 0x662518
    L0051c949:
            mov dl, byte ptr [eax]
            mov bl, byte ptr [esi]
            mov cl, dl
            cmp dl, bl
            jne L0051c971
            test cl, cl
            je L0051c96d
            mov dl, byte ptr [eax + 1]
            mov bl, byte ptr [esi + 1]
            mov cl, dl
            cmp dl, bl
            jne L0051c971
            add eax, 2
            add esi, 2
            test cl, cl
            jne L0051c949
    L0051c96d:
            xor eax, eax
            jmp L0051c976
    L0051c971:
            sbb eax, eax
            sbb eax, -1
    L0051c976:
            test eax, eax
            je L0051c9ad
            fild dword ptr [esp + 0x10]
            fild dword ptr [gScreenWidth]
            _emit 0xdc
        _emit 0x0d
        _emit 0x48
        _emit 0xb7
        _emit 0x58
        _emit 0x00
            fdivp st(1), st(0)
            call _ftol
            fild dword ptr [esp + 0x14]
            mov dword ptr [ebp], eax
            fild dword ptr [gScreenHeight]
            _emit 0xdc
        _emit 0x0d
        _emit 0x50
        _emit 0xb7
        _emit 0x58
        _emit 0x00
            fdivp st(1), st(0)
            call _ftol
            jmp L0051c9ca
    L0051c9ad:
            mov ecx, dword ptr [gHWnd]
            lea eax, [esp + 0x10]
            push eax
            push ecx
            _emit 0xff
        _emit 0x15
        _emit 0xb8
        _emit 0x91
        _emit 0x58
        _emit 0x00
            mov edx, dword ptr [esp + 0x10]
            mov eax, dword ptr [esp + 0x14]
            mov dword ptr [ebp], edx
    L0051c9ca:
            mov dword ptr [edi], eax
            mov ecx, dword ptr [ebp]
            mov dword ptr [gPD_mouse_position], ecx
            mov edx, dword ptr [edi]
            pop ebx
            pop esi
            pop edi
            mov dword ptr [gPD_mouse_position+0x4], edx
            pop ebp
            add esp, 8
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
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0051c280
void C2_HOOK_FASTCALL PDRevertPalette(void) {
}

// FUNCTION: CARMA2_HW 0x0051c8e0
void C2_HOOK_FASTCALL PDMouseButtons(int *pLeftButtonDown, int *pRightButtonDown) {
    *pLeftButtonDown = gMouseLButtonDown;
    if (pRightButtonDown != NULL) {
        *pRightButtonDown = gMouseRButtonDown;
    }
}

// FUNCTION: CARMA2_HW 0x0056a3a9
void C2_NAKED C2_HOOK_FASTCALL PDExtractFilename(char *pDest, const char *pPath) {
#ifndef CARPOCALYPSE2_MATCHING
    char dirname[256];
    char drive[4];
    char filename[256];
    char extension[256];

    _splitpath(pPath, drive, dirname, filename, extension);
    sprintf(pDest, "%s%s", filename, extension);
#else
    __asm {
            push ebp
            mov ebp, esp
            sub esp, 0x30c
            mov dword ptr [ebp - 0x30c], edx
            mov dword ptr [ebp - 0x308], ecx
            lea eax, [ebp - 0x204]
            push eax
            lea ecx, [ebp - 0x104]
            push ecx
            lea edx, [ebp - 0x304]
            push edx
            lea eax, [ebp - 4]
            push eax
            mov ecx, dword ptr [ebp - 0x30c]
            push ecx
            call sprintf
            add esp, 0x14
            lea edx, [ebp - 0x204]
            push edx
            lea eax, [ebp - 0x104]
            push eax
            push offset gMatchString_006736a0
            mov ecx, dword ptr [ebp - 0x308]
            push ecx
            call sprintf
            add esp, 0x10
            mov esp, ebp
            pop ebp
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0056a349
void C2_NAKED C2_HOOK_FASTCALL PDExtractDirectory(char *pDest, const char *pPath) {
#ifndef CARPOCALYPSE2_MATCHING
    char dirname[256];
    char drive[4];
    char filename[256];
    char extension[256];

    _splitpath(pPath, drive, dirname, filename, extension);
    sprintf(pDest, "%s%s", drive, dirname);
#else
    __asm {
            push ebp
            mov ebp, esp
            sub esp, 0x30c
            mov dword ptr [ebp - 0x30c], edx
            mov dword ptr [ebp - 0x308], ecx
            lea eax, [ebp - 0x204]
            push eax
            lea ecx, [ebp - 0x104]
            push ecx
            lea edx, [ebp - 0x304]
            push edx
            lea eax, [ebp - 4]
            push eax
            mov ecx, dword ptr [ebp - 0x30c]
            push ecx
            call sprintf
            add esp, 0x14
            lea edx, [ebp - 0x304]
            push edx
            lea eax, [ebp - 4]
            push eax
            push offset gMatchString_00673698
            mov ecx, dword ptr [ebp - 0x308]
            push ecx
            call sprintf
            add esp, 0x10
            mov esp, ebp
            pop ebp
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004928a0
void C2_NAKED C2_HOOK_FASTCALL PDPageInMemory(void *pMemory) {
#ifndef CARPOCALYPSE2_MATCHING
    tHeap_block_information *heap_block;

    C2_HOOK_BUG_ON(offsetof(tHeap_block_information, next) != 0x48);
    C2_HOOK_BUG_ON(offsetof(tHeap_block_information, field_0x72) != 0x72);
    C2_HOOK_BUG_ON(sizeof(tHeap_block_information) != 0x74);

    heap_block = pMemory;

    if (!IsBadReadPtr(heap_block, sizeof(tHeap_block_information)) && heap_block->field_0x72 == 0x4948) {
        for (; heap_block != NULL; heap_block = heap_block->next) {
            tHeap_memory *block;

            block = (tHeap_memory *) (heap_block + 1);
            for (;;) {
                DWORD size = block->flags & 0x5ffffffc;
                DWORD current;
                if (size == 0) {
                    break;
                }
                if (!(block->flags & 0x1)) {
                    if (gPage_size == 0) {
                        GetSystemInfo(&gSystem_info);
                        gPage_size = gSystem_info.dwPageSize;
                    }
                    for (current = 16 * gPage_size; current < size - 4; current += gPage_size) {
                        block->data[current + 16 - 16 * gPage_size] = block->data[current + 16 - 16 * gPage_size];
                        block->data[current + 16] = block->data[current + 16];
                    }
                }
                block = (tHeap_memory *) ((BYTE *) block + size);
            }
        }
    }
#else
    __asm {
            push ecx
            push ebx
            push ebp
            push esi
            push edi
            mov esi, ecx
            push 0x74
            push esi
            mov dword ptr [esp + 0x18], esi
            _emit 0xff
        _emit 0x15
        _emit 0x78
        _emit 0x90
        _emit 0x58
        _emit 0x00
            test eax, eax
            jne L00492943
            cmp word ptr [esi + 0x72], 0x4948
            jne L00492943
            test esi, esi
            je L00492943
            mov ecx, dword ptr [gPage_size]
    L004928ce:
            mov eax, dword ptr [esp + 0x10]
            lea ebx, [eax + 0x74]
    L004928d5:
            mov eax, dword ptr [ebx]
            mov ebp, eax
            and ebp, 0x5ffffffc
            je L00492934
            test al, 1
            jne L00492930
            test ecx, ecx
            lea edi, [ebp - 4]
            lea esi, [ebx + 0x10]
            jne L00492906
            push offset gSystem_info
            _emit 0xff
        _emit 0x15
        _emit 0x50
        _emit 0x91
        _emit 0x58
        _emit 0x00
            mov ecx, dword ptr [gSystem_info]
            mov dword ptr [gPage_size], ecx
    L00492906:
            mov eax, ecx
            shl eax, 4
            cmp eax, edi
            jae L00492930
    L0049290f:
            shl ecx, 4
            mov edx, esi
            sub edx, ecx
            lea ecx, [edx + eax]
            mov dl, byte ptr [edx + eax]
            mov byte ptr [ecx], dl
            mov cl, byte ptr [esi + eax]
            mov byte ptr [esi + eax], cl
            mov ecx, dword ptr [gPage_size]
            add eax, ecx
            cmp eax, edi
            jb L0049290f
    L00492930:
            add ebx, ebp
            jmp L004928d5
    L00492934:
            mov edx, dword ptr [esp + 0x10]
            mov eax, dword ptr [edx + 0x48]
            test eax, eax
            mov dword ptr [esp + 0x10], eax
            jne L004928ce
    L00492943:
            pop edi
            pop esi
            pop ebp
            xor eax, eax
            pop ebx
            pop ecx
            ret
            nop
            nop
            nop
            nop
            nop
    }
#endif
}

void C2_HOOK_FASTCALL PDPageInProcessMemory(void) {
    PDPageInMemory(GetProcessHeap());
    Sleep(1500);
}

// FUNCTION: CARMA2_HW 0x0051d4d0
void C2_HOOK_FASTCALL PDFileDelete(const char *pPath, int pIgnore_read_only) {
    if (pIgnore_read_only) {
        DWORD flags = GetFileAttributesA(pPath);
        if (flags != INVALID_FILE_ATTRIBUTES) {
            SetFileAttributesA(pPath, flags & ~FILE_ATTRIBUTE_READONLY);
        }
    }
    DeleteFileA(pPath);
}

int C2_HOOK_FASTCALL PDmemicmp(const char* str1, const char* str2, size_t count) {

    return c2_memicmp(str1, str2, count);
}

// FUNCTION: CARMA2_HW 0x0051a7d0
int C2_HOOK_CDECL PDMissingMaterial(const char* pName) {

    dr_dprintf("SOMETHING MISSING WARNING - Can't find material '%s'", pName);
    return 0;
}

// FUNCTION: CARMA2_HW 0x0051a7f0
int C2_HOOK_CDECL PDMissingTable(const char* pName) {

    dr_dprintf("SOMETHING MISSING WARNING - Can't find shade table '%s'", pName);
    return 0;
}

// FUNCTION: CARMA2_HW 0x0051a810
int C2_HOOK_CDECL PDMissingModel(const char* pName) {

    dr_dprintf("SOMETHING MISSING WARNING - Can't find model '%s'", pName);
    return 0;
}

// FUNCTION: CARMA2_HW 0x0051a830
int C2_HOOK_CDECL PDMissingMap(const char* pName) {

    dr_dprintf("SOMETHING MISSING WARNING - Can't find pixelmap '%s'", pName);
    return 0;
}

// FUNCTION: CARMA2_HW 0x0051c540
void C2_HOOK_FASTCALL PDRectangleCopy(br_pixelmap* dst, br_int_16 dx, br_int_16 dy, br_pixelmap* src, br_int_16 sx, br_int_16 sy, br_uint_16 w, br_uint_16 h) {
    if (src != dst) {
        BrPixelmapRectangleCopy(dst, dx, dy, src, sx, sy, w, h);
    }
}

// FUNCTION: CARMA2_HW 0x0051c9f0
void C2_HOOK_FASTCALL PDConvertMousePosition(LPPOINT pPoint, LONG* pX, LONG* pY) {
    if (gScaleMouse && strcmp(gRenderer, "D3D") != 0) {
        *pX = (LONG)(pPoint->x / (gScreenWidth / 640.));
        *pY = (LONG)(pPoint->y / (gScreenHeight / 480.));
    } else {
        ScreenToClient(gHWnd, pPoint);
        *pX = pPoint->x;
        *pY = pPoint->y;
    }
    gPD_mouse_position.x = *pX;
    gPD_mouse_position.y = *pY;
}

// FUNCTION: CARMA2_HW 0x0051d4a0
void C2_HOOK_FASTCALL FUN_0051d4a0(void) {
}
