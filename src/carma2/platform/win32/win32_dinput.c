#include "win32_dinput.h"

#include "win32.h"
#include "win32_windows.h"

#include "displays.h"
#include "errors.h"
#include "globvars.h"
#include "init.h"
#include "input.h"
#include "joystick.h"
#include "utility.h"

#include "carpocalypse2_macros.h"

#include "c2_stdlib.h"
#include "c2_string.h"

#include <windows.h>
#include <dinput.h>
#include <iforce2.h>

#include <brender/brender.h>


// GLOBAL: CARMA2_HW 0x0058b398
extern const GUID IID_IDirectInputDevice2A;

#define MAX_COUNT_JOYSTICKS 16
#define MAX_COUNT_EFFECTS 30


// GLOBAL: CARMA2_HW 0x006ad1f8
int gASCII_table[151];

// GLOBAL: CARMA2_HW 0x006ac588
int gASCII_shift_table[151];

// GLOBAL: CARMA2_HW 0x006b3470
int gScan_code[256];


// GLOBAL: CARMA2_HW 0x006acc78
LPDIRECTINPUTA gDirectInput;


// GLOBAL: CARMA2_HW 0x00762280
int gJoystick_deadzone;


// GLOBAL: CARMA2_HW 0x00686158
IDirectInputA* gDirectInputJoystickHandle;

// GLOBAL: CARMA2_HW 0x0079d9c0
tDirectInputJoystickInfo gDirectInputJoystickInfos[MAX_COUNT_JOYSTICKS];

// GLOBAL: CARMA2_HW 0x0079e060
IDirectInputDevice2A* gDirectInputJoystickDevices[MAX_COUNT_JOYSTICKS];

// GLOBAL: CARMA2_HW 0x0079e0a0
IDirectInputEffect* gDirectInputEffects[MAX_COUNT_EFFECTS];

// GLOBAL: CARMA2_HW 0x0068615c
int gJoystickFFB;

// GLOBAL: CARMA2_HW 0x00686160
int gCountEnumeratedJoystickDinputDevices;

// GLOBAL: CARMA2_HW 0x00686170
int gDirectInputJoystickEnumerated;

// GLOBAL: CARMA2_HW 0x00684628
tJoystick_force_effect gJoystick_effects[30];

// GLOBAL: CARMA2_HW 0x00686168
int gForceFeedbackAvailable;

// GLOBAL: CARMA2_HW 0x00595f74
int gBasic_friction_joystick_effect_index = -1;

// GLOBAL: CARMA2_HW 0x00595f78
int gBasic_force_joystick_effect_index = -1;

// GLOBAL: CARMA2_HW 0x00595f80
int gCollision_force_joystick_effect_index = -1;

// GLOBAL: CARMA2_HW 0x0059630c
int gCurrent_joystick_effect_index = -1;

// GLOBAL: CARMA2_HW 0x0079e040
IFR_Project * gIfr_project;

// GLOBAL: CARMA2_HW 0x00686164
int gCount_joystick_effects;

// GLOBAL: CARMA2_HW 0x00589f38
const GUID* gJoystick_effect_guids[13] = {
    &GUID_ConstantForce,
    &GUID_RampForce,
    &GUID_CustomForce,
    &GUID_Square,
    &GUID_Sine,
    &GUID_Triangle,
    &GUID_SawtoothUp,
    &GUID_SawtoothDown,
    &GUID_Spring,
    &GUID_Spring,
    &GUID_Damper,
    &GUID_Inertia,
    &GUID_Friction,
};

// GLOBAL: CARMA2_HW 0x006ad4b8
int gJoy1_valid;

// GLOBAL: CARMA2_HW 0x006ac7e8
JOYINFOEX gJoy1_info;

// GLOBAL: CARMA2_HW 0x006ad4bc
int gJoy2_valid;

// GLOBAL: CARMA2_HW 0x006ac820
JOYINFOEX gJoy2_info;

// GLOBAL: CARMA2_HW 0x00595f98
int gINT_00595f98 = 1;

// GLOBAL: CARMA2_HW 0x00595f9c
int gINT_00595f9c = 1;

// GLOBAL: CARMA2_HW 0x006ad094
tU32 gUINT_006ad094;

// GLOBAL: CARMA2_HW 0x006ad0b8
int gINT_006ad0b8;

static int InitDirectInput(void) {
    int i;

    if (gDirectInputJoystickHandle == NULL) {
        memset(gDirectInputJoystickDevices, 0, sizeof(gDirectInputJoystickDevices));
        for (i = 0; i < MAX_COUNT_JOYSTICKS; i++) {
            gDirectInputJoystickInfos[i].buttonMask = 0;
            gDirectInputJoystickInfos[i].axisMask = 0;
            gDirectInputJoystickInfos[i].devSubType = 0;
            gDirectInputJoystickInfos[i].productName[0] = '\0';
            gDirectInputJoystickInfos[i].sizeData = 0;
            gDirectInputJoystickInfos[i].data = NULL;
        }
        memset(gDirectInputEffects, 0, sizeof(gDirectInputEffects));
        gJoystickFFB = 0;
        gCountEnumeratedJoystickDinputDevices = 0;
        DirectInputCreateA(GetModuleHandleA(NULL), 0x0500, &gDirectInputJoystickHandle, NULL);
        if (gDirectInputJoystickHandle == NULL) {
            return 1;
        }
        gDirectInputJoystickEnumerated = 0;
    }
    return 0;
}

// C2PORT-NONMATCHING: best 47.22% | difftest PASS 2000 trials (spec difftest/specs/004587c0_FUN_004587c0.json) | reason initializer-loop
#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern size_t C2_HOOK_FASTCALL GetCurrentJoystickCountButtons(void);
extern const char* C2_HOOK_FASTCALL GetCurrentJoystickName(void);
extern tButtonJoystickInfo* C2_HOOK_FASTCALL PDGetCurrentJoystickData(void);
extern tU32 C2_HOOK_FASTCALL PDGetJoystickButtonStates(void);
extern br_uint_8 bl;
extern int gJoystick_index;
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_FASTCALL CollectJoystickButtonInfos(void);
extern br_error C2_HOOK_FASTCALL DRBrEnd(void);
extern int C2_HOOK_FASTCALL JoystickDInputBegin(void);
extern int C2_HOOK_FASTCALL LoadJoystickPreferences(void);
extern C2_NORETURN void C2_HOOK_FASTCALL PDShutdownSystem(void);
extern int C2_HOOK_FASTCALL PossibleUnlock(int pValue);
extern void __cdecl _ftol();
extern void C2_HOOK_CDECL dr_dprintf();
extern br_pixelmap* gBack_screen;
extern int gBr_initialized;
extern int gCancel_recover;
extern int gDisable_sound;
extern int gExit_code;
extern char gFatalErrorMessage[512];
extern HINSTANCE gHInstance;
extern void* gHWnd;
extern int gIsFatalError;
extern float gJoystick_x_steering;
extern float gJoystick_y_throttle;
extern char gRegionDescriminator[];
extern int gUNK_00595f9c;
#endif
// FUNCTION: CARMA2_HW 0x004587c0
void C2_HOOK_FASTCALL FUN_004587c0(void) {
    tU32* fields;
    int zero;
    tU32* fields_end;

    C2_HOOK_BUG_ON(sizeof(tDirectInputJoystickInfo) != 0x68);
    zero = 0;
    if (gDirectInputJoystickHandle == (IDirectInputA*)zero) {
        memset(gDirectInputJoystickDevices, 0, sizeof(gDirectInputJoystickDevices));
        fields = (tU32*)gDirectInputJoystickInfos;
        fields_end = fields + CARPOCALYPSE2_ASIZE(gDirectInputJoystickInfos) * 0x1a;
        do {
            fields[0] = zero;
            fields[1] = zero;
            fields[2] = zero;
            ((tU8*)fields)[12] = (tU8)zero;
            fields[0x18] = zero;
            fields[0x19] = zero;
            fields += 0x1a;
        } while (fields < fields_end);
        memset(gDirectInputEffects, 0, sizeof(gDirectInputEffects));
        gJoystickFFB = zero;
        gCountEnumeratedJoystickDinputDevices = zero;
        DirectInputCreateA(GetModuleHandleA((LPCSTR)zero), 0x0500, &gDirectInputJoystickHandle, (LPVOID)zero);
    }
}

void C2_HOOK_FASTCALL CloseDirectInput(void) {

    if (gDirectInputDevice != NULL) {
        IDirectInputDevice_Unacquire(gDirectInputDevice);
        IDirectInputDevice_Release(gDirectInputDevice);
        gDirectInputDevice = NULL;
    }
    if (gDirectInput != NULL) {
        IDirectInput_Release(gDirectInput);
        gDirectInput = NULL;
    }
}

int C2_HOOK_FASTCALL AcquireDInputJoystickDevice(int pIndex) {
    IDirectInputDevice2A *device;
    HRESULT result;

    if (gDirectInputJoystickHandle == NULL) {
        return -1;
    }
    device = gDirectInputJoystickDevices[pIndex];
    if (device == NULL) {
        return -1;
    }
    result = IDirectInputDevice2_Acquire(device);
    if (result == DI_OK) {
        dr_dprintf("Joystick #%d acquired\n", pIndex);
        return 1;
    } else if (result == DI_NOEFFECT) {
        dr_dprintf("Joystick #%d already acquired\n", pIndex);
        return 1;
    } else if (result == DIERR_INVALIDPARAM) {
        dr_dprintf("Joystick %d DIERR_INVALIDPARAM\n", pIndex);
        return 0;
    } else if (result == DIERR_OTHERAPPHASPRIO) {
        dr_dprintf("Joystick %d DIERR_OTHERAPPHASPRIO\n", pIndex);
        return 0;
    } else {
        dr_dprintf("Unknown Error acquiring joystick %d\n", pIndex);
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x004589d0
tDirectInputJoystickInfo* C2_HOOK_FASTCALL JoystickDInputGetInfo(int pJoystickIndex, tU32* pButtonMask, tU32* pAxisMask) {
    IDirectInputDevice2A* device;
    tDirectInputJoystickInfo* info;

    if (pButtonMask != NULL) {
        *pButtonMask = 0;
    }
    if (pAxisMask != NULL) {
        *pAxisMask = 0;
    }
    if (InitDirectInput()) {
        return NULL;
    }
    device = gDirectInputJoystickDevices[pJoystickIndex];
    if (device == NULL) {
        return NULL;
    }
    AcquireDInputJoystickDevice(pJoystickIndex);
    info = &gDirectInputJoystickInfos[pJoystickIndex];
    if (info->buttonMask == 0 && info->axisMask == 0) {
        DIDEVCAPS caps;
        HRESULT hRes;
        DIDEVICEOBJECTINSTANCE device_obj_descr;
        tU32 i;

        caps.dwSize = sizeof(caps);
        hRes = IDirectInputDevice2_GetCapabilities(device, &caps);
        if (hRes != DI_OK) {
            return NULL;
        }
        if (caps.dwFlags & DIDC_FORCEFEEDBACK) {
            gJoystickFFB |= 1 << pJoystickIndex;
        }
        info->count_buttons = caps.dwButtons; // unsure
        device_obj_descr.dwSize = sizeof(device_obj_descr);

        for (i = 0; i < caps.dwButtons && i < CARPOCALYPSE2_ASIZE(((tJoystickInputState*)NULL)->buttons); i++) {
            hRes = IDirectInputDevice2_GetObjectInfo(device, &device_obj_descr,
                                                     offsetof(tJoystickInputState, buttons) + i, DIPH_BYOFFSET);
            if (hRes == DI_OK) {
                info->buttonMask |= 1 << i;
                if (pButtonMask != NULL) {
                    *pButtonMask = info->buttonMask; // unsure
                }
            }
        }

        for (i = 0; i < caps.dwAxes && i < 8; i++) {
            hRes = IDirectInputDevice2_GetObjectInfo(device, &device_obj_descr,
                                                     sizeof(tU32) * i, DIPH_BYOFFSET);
            if (hRes == DI_OK) {
                info->axisMask |= 1 << i;
                if (pAxisMask != NULL) {
                    *pAxisMask = info->axisMask; // unsure
                }
            }
        }
    } else {
        if (pButtonMask != NULL) {
            *pButtonMask = info->buttonMask;
        }
        if (pAxisMask != NULL) {
            *pAxisMask = info->axisMask;
        }
    }
    return info;
}

// FUNCTION: CARMA2_HW 0x00459c70
BOOL CALLBACK Win32DInputJoystickEnum(const DIDEVICEINSTANCEA* pDeviceInstance, void *pContext) {
    HRESULT hResult;
    IDirectInputDeviceA *device;
    IDirectInputDevice2A *device2;
    int nb;
    IDirectInputA* pDirectInput = pContext;
    DIPROPRANGE diphRange;
    DIPROPDWORD zoneProp;

    C2_HOOK_ASSERT(sizeof(tJoystickInputState) == 0x50);

    nb = gCountEnumeratedJoystickDinputDevices;
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-truncation"
#endif
    strncpy(gDirectInputJoystickInfos[nb].productName, pDeviceInstance->tszProductName, sizeof(gDirectInputJoystickInfos[nb].productName));
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
    gDirectInputJoystickInfos[nb].productName[sizeof(gDirectInputJoystickInfos[nb].productName) - 1] = '\0';
    switch (pDeviceInstance->dwDevType) {
    case DIDEVTYPEJOYSTICK_GAMEPAD:
        gDirectInputJoystickInfos[nb].count_buttons = 4;
        break;
    case DIDEVTYPEJOYSTICK_WHEEL:
        gDirectInputJoystickInfos[nb].count_buttons = 2;
        break;
    default:
        gDirectInputJoystickInfos[nb].count_buttons = 1;
        break;
    }
    hResult = IDirectInput_CreateDevice(pDirectInput, &pDeviceInstance->guidInstance, &device, NULL);
    if (hResult != DI_OK) {
        dr_dprintf("Could not create dinput object\n");
        return DIENUM_CONTINUE;
    }
    hResult = IDirectInputDevice_SetDataFormat(device, &c_dfDIJoystick);
    if (hResult != DI_OK) {
        dr_dprintf("Could not set dinput device data format\n");
        IDirectInputDevice2_Release(device);
        return DIENUM_CONTINUE;
    }
    hResult = IDirectInputDevice_QueryInterface(device, &IID_IDirectInputDevice2A,
                                                (void **)&gDirectInputJoystickDevices[nb]);
    IDirectInputDevice2_Release(device);
    if (hResult != DI_OK) {
        dr_dprintf("QueryInterface did not return DI_OK\n");
        return DIENUM_CONTINUE;
    }
    device2 = gDirectInputJoystickDevices[nb];
    if (device2 == NULL) {
        dr_dprintf("Could not set dinput coop level\n");
        return DIENUM_STOP;
    }
    hResult = IDirectInputDevice2_SetCooperativeLevel(device2, gHWnd, DISCL_BACKGROUND | DISCL_EXCLUSIVE);
    if (hResult != DI_OK) {
        dr_dprintf("Could not set dinput coop level\n");
        return DIENUM_STOP;
    }
    diphRange.diph.dwSize = sizeof(diphRange);
    diphRange.diph.dwHeaderSize = sizeof(diphRange.diph);
    diphRange.diph.dwObj = offsetof(tJoystickInputState, xaxis);
    diphRange.diph.dwHow = DIPH_BYOFFSET;
    diphRange.lMin = 0;
    diphRange.lMax = 0xffff;
    hResult = IDirectInputDevice2_SetProperty(device2, DIPROP_RANGE, (DIPROPHEADER*)&diphRange);
    if (hResult != DI_OK) {
        dr_dprintf("IDirectInputDevice::SetProperty(DIPH_RANGE)  FAILED\n");
        IDirectInputDevice2_Release(device2);
        return DIENUM_CONTINUE;
    }
    zoneProp.diph.dwSize = sizeof(zoneProp);
    zoneProp.diph.dwHeaderSize = sizeof(zoneProp.diph);
    diphRange.diph.dwObj = offsetof(tJoystickInputState, xaxis);
    zoneProp.diph.dwHow = DIPH_BYOFFSET;
    zoneProp.dwData = 1000;
    hResult = IDirectInputDevice2_SetProperty(device2, DIPROP_DEADZONE, (DIPROPHEADER*)&diphRange);
    if (hResult != DI_OK) {
        dr_dprintf("IDirectInputDevice:: SetProperty(DIPH_DEADZONE) FAILED\n");
        IDirectInputDevice2_Release(device2);
        return DIENUM_CONTINUE;
    }
    gCountEnumeratedJoystickDinputDevices++;
    return DIENUM_CONTINUE;
}

// FUNCTION: CARMA2_HW 0x00459ed0
tButtonJoystickInfo* C2_HOOK_FASTCALL AllocateCurrentJoystickButtonInfos(size_t pSize) {
    tButtonJoystickInfo* data;
    int joystick_index;

    if (pSize != 0 && gJoystick_index != -1 && gDirectInputJoystickInfos[gJoystick_index].data == NULL) {
        data = malloc(pSize);
        joystick_index = gJoystick_index;
        gDirectInputJoystickInfos[gJoystick_index].data = data;
        if (data != NULL) {
            gDirectInputJoystickInfos[joystick_index].sizeData = pSize;
            return gDirectInputJoystickInfos[gJoystick_index].data;
        }
    }
    return NULL;
}

// FUNCTION: CARMA2_HW 0x00459f30
void C2_HOOK_FASTCALL FreeCurrentJoystickButtonInfos(void) {
    int joystick_index;

    if (gJoystick_index != -1 && gDirectInputJoystickInfos[gJoystick_index].data != NULL) {
        BrMemFree(gDirectInputJoystickInfos[gJoystick_index].data);
        joystick_index = gJoystick_index;
        gDirectInputJoystickInfos[gJoystick_index].data = NULL;
        gDirectInputJoystickInfos[joystick_index].sizeData = 0;
    }
}

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x00459fe0
void C2_NAKED C2_HOOK_FASTCALL AttachJoystickButtonInfos(size_t pSize, tWin32_void_voidptr_cbfn pCallback) {
#ifndef CARPOCALYPSE2_MATCHING
    int original_index;
    int i;

    original_index = gJoystick_index;
    for (i = 0; i < CARPOCALYPSE2_ASIZE(gDirectInputJoystickDevices); i++) {
        IDirectInputDevice2A *device;

        device = gDirectInputJoystickDevices[i];
        if (device != NULL && gDirectInputJoystickInfos[i].data == NULL) {
            gJoystick_index = i;
            if (pSize != 0) {
                gDirectInputJoystickInfos[i].data = malloc(pSize);
                if (gDirectInputJoystickInfos[i].data != NULL) {
                    gDirectInputJoystickInfos[i].sizeData = pSize;
                }
            }
            if (gDirectInputJoystickInfos[i].data != NULL) {
                pCallback(gDirectInputJoystickInfos[i].data);
            }
        }
    }
    gJoystick_index = original_index;
#else
    __asm {
            sub esp, 8
            mov eax, dword ptr [gJoystick_index]
            push ebx
            push ebp
            push esi
            push edi
            mov dword ptr [esp + 0x10], edx
            mov ebp, ecx
            mov dword ptr [esp + 0x14], eax
            xor edi, edi
            mov ebx, offset gDirectInputJoystickInfos+0x64
            mov esi, offset gDirectInputJoystickDevices
    L0045a002:
            cmp dword ptr [esi], 0
            je L0045a04d
            cmp dword ptr [ebx], 0
            jne L0045a04d
            test ebp, ebp
            mov dword ptr [gJoystick_index], edi
            je L0045a043
            cmp edi, -1
            je L0045a043
            push ebp
            call malloc
            mov ecx, dword ptr [gJoystick_index]
            add esp, 4
            lea edx, [ecx + ecx*2]
            lea ecx, [ecx + edx*4]
            shl ecx, 3
            test eax, eax
            mov dword ptr [ecx + gDirectInputJoystickInfos+0x64], eax
            je L0045a043
            mov dword ptr [ecx + gDirectInputJoystickInfos+0x60], ebp
    L0045a043:
            mov ecx, dword ptr [ebx]
            test ecx, ecx
            je L0045a04d
            call dword ptr [esp + 0x10]
    L0045a04d:
            add esi, 4
            inc edi
            add ebx, 0x68
            cmp esi, offset gDirectInputEffects
            jl L0045a002
            mov eax, dword ptr [esp + 0x14]
            pop edi
            pop esi
            pop ebp
            mov dword ptr [gJoystick_index], eax
            pop ebx
            add esp, 8
            ret
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00459970
void C2_NAKED C2_HOOK_FASTCALL CloseDInputJoysticks(void) {
#ifndef CARPOCALYPSE2_MATCHING
    IDirectInputDevice2A** device;

    if (gDirectInputJoystickHandle != NULL) {
        device = gDirectInputJoystickDevices;
        /* Visit every device slot. */
        do {
            if (*device != NULL) {
                IDirectInputDevice2_Unacquire(*device);
                IDirectInputDevice2_Release(*device);
                *device = NULL;
            }
            device++;
        } while ((int)device < (int)(gDirectInputJoystickDevices + CARPOCALYPSE2_ASIZE(gDirectInputJoystickDevices)));
        IDirectInput_Release(gDirectInputJoystickHandle);
        gDirectInputJoystickHandle = NULL;
    }
    gJoystickFFB = 0;
    gCountEnumeratedJoystickDinputDevices = 0;
#else
    __asm {
            mov eax, dword ptr [gDirectInputJoystickHandle]
            test eax, eax
            je L004599ba
            push esi
            mov esi, offset gDirectInputJoystickDevices
    L0045997f:
            mov eax, dword ptr [esi]
            test eax, eax
            je L00459999
            mov ecx, dword ptr [eax]
            push eax
            call dword ptr [ecx + 0x20]
            mov eax, dword ptr [esi]
            push eax
            mov edx, dword ptr [eax]
            call dword ptr [edx + 8]
            mov dword ptr [esi], 0
    L00459999:
            add esi, 4
            cmp esi, offset gDirectInputEffects
            jl L0045997f
            mov eax, dword ptr [gDirectInputJoystickHandle]
            push eax
            mov ecx, dword ptr [eax]
            call dword ptr [ecx + 8]
            mov dword ptr [gDirectInputJoystickHandle], 0
            pop esi
    L004599ba:
            mov dword ptr [gJoystickFFB], 0
            mov dword ptr [gCountEnumeratedJoystickDinputDevices], 0
            ret
            nop
    }
#endif
}

#pragma auto_inline(on)

// FUNCTION: CARMA2_HW 0x004599d0
const char* C2_HOOK_FASTCALL GetCurrentJoystickName(void) {
    return gDirectInputJoystickInfos[gJoystick_index].productName;
}

// FUNCTION: CARMA2_HW 0x00459a70
void C2_HOOK_FASTCALL SelectNextDInputJoystick(void) {
    char buffer[256];

    if (gJoystick_index != -1 && gDirectInputJoystickDevices[gJoystick_index] != NULL) {
        if ((gJoystickFFB & (1 << gJoystick_index)) != 0) {
            ResetDInputJoystickFFB(gJoystick_index);
        }
        gJoystick_index++;
        if (gJoystick_index > gCountEnumeratedJoystickDinputDevices - 1 || 0xf < gJoystick_index) {
            gJoystick_index = 0;
        }
        JoystickDInputGetInfo(gJoystick_index, NULL, NULL);
        if (gJoystick_index != -1 && gDirectInputJoystickDevices[gJoystick_index] != NULL) {
            sprintf(buffer, "Joystick %d: %s %d", gJoystick_index,
                    gDirectInputJoystickInfos[gJoystick_index].productName,
                    (gJoystickFFB & (1 << gJoystick_index)) != 0);
            NewTextHeadupSlot(4, 0, 3000, -1, buffer);
        }
    }
}

// FUNCTION: CARMA2_HW 0x00459b50
int C2_HOOK_FASTCALL SetDirectInputDeviceRange(IDirectInputDevice2A* pDevice, LONG pMin, LONG pMax) {
    DIPROPRANGE range;
    HRESULT hResult;

    range.diph.dwSize = sizeof(range);
    range.diph.dwHeaderSize = sizeof(range.diph);
    range.diph.dwHow = DIPH_BYOFFSET;
    range.lMin = pMin;
    range.lMax = pMax;
    range.diph.dwObj = 0;
    hResult = IDirectInputDevice2_SetProperty(pDevice, DIPROP_RANGE, &range.diph);
    if (hResult == DI_OK) {
        range.diph.dwObj = 4;
        hResult = IDirectInputDevice2_SetProperty(pDevice, DIPROP_RANGE, &range.diph);
    }
    if (hResult != DI_OK) {
        dr_dprintf("IDirectInputDevice::SetProperty(DIPH_RANGE)  FAILED\n");
        IDirectInputDevice2_Release(pDevice);
        return 0;
    }
    return 1;
}

#pragma auto_inline(off)
#pragma inline_depth(0)
// FUNCTION: CARMA2_HW 0x0045c420
void C2_HOOK_FASTCALL FUN_0045c420(void) {
    tButtonJoystickInfo* info;

    SelectNextDInputJoystick();
    info = PDGetCurrentJoystickData();
    if (info != NULL) {
        gJoystick_x_steering = info->field_0xd8;
        gJoystick_y_throttle = info->field_0xdc;
        gUNK_00595f9c = info->field_0xd4;
    }
}
#pragma inline_depth()
#pragma auto_inline(on)

// FUNCTION: CARMA2_HW 0x00459fb0
size_t C2_HOOK_FASTCALL GetCurrentJoystickCountButtons(void) {
    return gDirectInputJoystickInfos[gJoystick_index].count_buttons;
}

// FUNCTION: CARMA2_HW 0x00459fd0
int C2_HOOK_FASTCALL PDIsJoystickAvailable(int pIndex) {
    return gDirectInputJoystickDevices[pIndex] != NULL;
}

// FUNCTION: CARMA2_HW 0x0045c370
void C2_NAKED C2_HOOK_FASTCALL CollectJoystickButtonInfo(tButtonJoystickInfo* pInfo) {
#ifndef CARPOCALYPSE2_MATCHING
    unsigned int i;

    if (pInfo == NULL) {
        return;
    }
    strcpy(pInfo->productName, GetCurrentJoystickName());
    pInfo->count_buttons = GetCurrentJoystickCountButtons();
    for (i = 0; i < CARPOCALYPSE2_ASIZE(((tButtonJoystickInfo*)NULL)->buttons); i++) {
        if (i < pInfo->count_buttons) {
            pInfo->buttons[i] = i;
        } else {
            pInfo->buttons[i] = -1;
        }
        pInfo->field_0xd4 = 1;
        pInfo->field_0xd8 = 1.0f;
        pInfo->field_0xdc = 1.0f;
        pInfo->field_0xe0 = 0; /* or 0.f */
        pInfo->field_0xe4 = 50;
        pInfo->field_0xe8 = 0; /* or 0.f */
    }
#else
    __asm {
            push ebx
            mov ebx, ecx
            test ebx, ebx
            je L0045c401
            push edi
            push esi
            call GetCurrentJoystickName
            mov edi, eax
            or ecx, 0xffffffff
            xor eax, eax
            repne scasb
            not ecx
            sub edi, ecx
            mov eax, ecx
            mov esi, edi
            mov edi, ebx
            shr ecx, 2
            rep movsd
            mov ecx, eax
            and ecx, 3
            rep movsb
            call GetCurrentJoystickCountButtons
            mov dword ptr [ebx + 0x50], eax
            xor eax, eax
            lea ecx, [ebx + 0x54]
    L0045c3ae:
            cmp eax, dword ptr [ebx + 0x50]
            jae L0045c3b7
            mov dword ptr [ecx], eax
            jmp L0045c3bd
    L0045c3b7:
            mov dword ptr [ecx], 0xffffffff
    L0045c3bd:
            inc eax
            add ecx, 4
            cmp eax, 0x20
            jb L0045c3ae
            mov eax, 0x3f800000
            pop esi
            mov dword ptr [ebx + 0xd4], 1
            mov dword ptr [ebx + 0xd8], eax
            mov dword ptr [ebx + 0xdc], eax
            mov dword ptr [ebx + 0xe0], 0
            mov dword ptr [ebx + 0xe4], 0x32
            mov dword ptr [ebx + 0xe8], 0
            pop edi
    L0045c401:
            pop ebx
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

// FUNCTION: CARMA2_HW 0x0045c410
void C2_HOOK_FASTCALL CollectJoystickButtonInfos(void) {
    C2_HOOK_BUG_ON(sizeof(tButtonJoystickInfo) != 0xec);
    AttachJoystickButtonInfos(sizeof(tButtonJoystickInfo), (void(C2_HOOK_FASTCALL*)(void*))CollectJoystickButtonInfo);
}

// FUNCTION: CARMA2_HW 0x00457a60
void C2_HOOK_FASTCALL FUN_00457a60(void) {
    IDirectInputDevice2A* device;

    if (gForceFeedbackAvailable && gJoystick_index != -1) {
        device = gDirectInputJoystickDevices[gJoystick_index];
        if (device != NULL && (gJoystickFFB & (1 << gJoystick_index))) {
            IDirectInputDevice2_SendForceFeedbackCommand(device, DISFFC_PAUSE);
        }
    }
}

// FUNCTION: CARMA2_HW 0x00457b70
void C2_HOOK_FASTCALL SetAllFFBEffectsGain(int pGain) {
    DIPROPDWORD dipdw;
    int i;

    dipdw.diph.dwSize = sizeof(DIPROPDWORD);
    dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    dipdw.diph.dwObj = 0;
    dipdw.diph.dwHow = DIPH_DEVICE;
    if (gForceFeedbackAvailable && gJoystick_index != -1) {
        if (pGain > 100) {
            dipdw.dwData = 10000;
        } else if (pGain < 0) {
            dipdw.dwData = 0;
        } else {
            dipdw.dwData = pGain * 100;
        }
        for (i = 0; i < gCount_joystick_effects; i++) {
            UnloadDinputFFBEffectAtIndex(i);
        }
        IDirectInputDevice_SetProperty((IDirectInputDeviceA *)gDirectInputJoystickDevices[gJoystick_index], DIPROP_FFGAIN, &dipdw.diph);
        for (i = 0; i < gCount_joystick_effects; i++) {
            DownloadDinputFFBEffectAtIndex(i);
        }
    }
}

// FUNCTION: CARMA2_HW 0x0045c780
int C2_HOOK_FASTCALL IncreaseJoystickFFBGain(void) {
    gUNK_00596310 += 25;
    if (gUNK_00596310 > 100) {
        gUNK_00596310 = 0;
    }
    SetAllFFBEffectsGain(gUNK_00596310);
    return gUNK_00596310;
}

// FUNCTION: CARMA2_HW 0x0045a070
int C2_HOOK_FASTCALL PDEnableJoysticks(void) {
    int count;

    count = gCountEnumeratedJoystickDinputDevices;
    if (count != 0) {
        gJoystick_index = (gUNK_00595f8c == -1) ? 0 : gUNK_00595f8c;
    }
    if (gDirectInputJoystickDevices[gJoystick_index] != NULL && (gJoystickFFB & (1 << gJoystick_index))) {
        gForceFeedbackAvailable = 1;
    }
    return count;
}

// FUNCTION: CARMA2_HW 0x00457c20
int C2_HOOK_FASTCALL CreateDinputEffect(int joystick_index, tJoystick_effect_description* description, const char *name) {
    int effect_id = gCount_joystick_effects;
    IDirectInputDevice2A *device = gDirectInputJoystickDevices[joystick_index];
    // GLOBAL: CARMA2_HW 0x00684620
    static DWORD axis[2];

    C2_HOOK_BUG_ON(sizeof(tJoystick_effect_description) != 0x50);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tJoystick_effect_description, type, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tJoystick_effect_description, type_specific_params, 0x4);
    C2_HOOK_BUG_ON(sizeof(description->type_specific_params) != 0x18);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tJoystick_effect_description, time_ms, 0x34);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tJoystick_effect_description, field_0x3c, 0x3c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tJoystick_effect_description, field_0x48, 0x48);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tJoystick_effect_description, field_0x4c, 0x4c);

    if (effect_id >= CARPOCALYPSE2_ASIZE(gJoystick_effects)) {
        dr_dprintf("Reached hardcoded limit for # of effects.");
        return -1;
    }
    if (device == NULL || !(gJoystickFFB & (1 << joystick_index))) {
        return -1;
    }
    gJoystick_effects[effect_id].di_effect.dwDuration = description->time_ms != -1 ? 1000 * description->time_ms : -1;
    gJoystick_effects[effect_id].di_effect.dwGain = description->field_0x3c;
    // FIXME: dwTriggerButton contains offset
    gJoystick_effects[effect_id].di_effect.dwTriggerButton = description->field_0x44 != -1 ? description->field_0x44 + 0x30 : -1;
    gJoystick_effects[effect_id].di_effect.dwTriggerRepeatInterval = description->field_0x48;
    gJoystick_effects[effect_id].field_0x60 = 100 * description->field_0x4c;
    if (description->field_0x40 == 0 || (gDirectInputJoystickInfos[joystick_index].devSubType & 0x2)) {
        gJoystick_effects[effect_id].di_effect.cAxes = 1;
        gJoystick_effects[effect_id].di_effect.rgdwAxes = axis;
        // FIXME: offset
        axis[0] = 0;
    } else if (description->field_0x40 == 1) {
        gJoystick_effects[effect_id].di_effect.cAxes = 1;
        gJoystick_effects[effect_id].di_effect.rgdwAxes = axis;
        // FIXME: offset
        axis[0] = 4;
    } else {
        gJoystick_effects[effect_id].di_effect.cAxes = 2;
        gJoystick_effects[effect_id].di_effect.rgdwAxes = axis;
        // FIXME: offset
        axis[0] = 0;
        axis[1] = 4;
    }
    memcpy(&gJoystick_effects[effect_id].type_specific_params,
        &description->type_specific_params,
        sizeof(tJoystick_effect_type_params));
    C2_HOOK_BUG_ON(sizeof(DIEFFECT) != 0x34);
    gJoystick_effects[effect_id].di_effect.dwSize = sizeof(DIEFFECT);
    gJoystick_effects[effect_id].di_effect.dwFlags = DIEFF_OBJECTOFFSETS | DIEFF_POLAR;
    gJoystick_effects[effect_id].di_effect.dwSamplePeriod = 0;
    gJoystick_effects[effect_id].di_effect.rglDirection = &gJoystick_effects[effect_id].field_0x60;
    gJoystick_effects[effect_id].di_effect.lpEnvelope = NULL;
    gJoystick_effects[effect_id].di_effect.lpvTypeSpecificParams = &gJoystick_effects[effect_id].type_specific_params;
    gJoystick_effects[effect_id].field_0x64 = 0;

    C2_HOOK_BUG_ON(sizeof(DICONSTANTFORCE) != 0x4);
    C2_HOOK_BUG_ON(sizeof(DIRAMPFORCE) != 0x8);
    C2_HOOK_BUG_ON(sizeof(DICONDITION) != 0x18);
    switch (description->type) {
    case eJoystick_effect_ConstantForce:
        gJoystick_effects[effect_id].di_effect.cbTypeSpecificParams = sizeof(DICONSTANTFORCE);
        break;
    case eJoystick_effect_RampForce:
    case eJoystick_effect_CustomForce:
    case eJoystick_effect_Square:
    case eJoystick_effect_Sine:
    case eJoystick_effect_Triangle:
    case eJoystick_effect_SawtoothUp:
        gJoystick_effects[effect_id].di_effect.cbTypeSpecificParams = sizeof(DIRAMPFORCE);
        break;
    case eJoystick_effect_Spring_8:
    case eJoystick_effect_Damper:
    case eJoystick_effect_Inertia:
    case eJoystick_effect_Friction:
        gJoystick_effects[effect_id].di_effect.cbTypeSpecificParams = sizeof(DICONDITION);
        break;
    case eJoystick_effect_Spring_9:
        // FIXME: what sizeof() is this? (tye = Spring)
        gJoystick_effects[effect_id].di_effect.cbTypeSpecificParams = 0x30;
        break;
    default:
        dr_dprintf("Illegal effect Type");
        return -1;
    }
    IDirectInputDevice2_CreateEffect(device,
        gJoystick_effect_guids[description->type],
        &gJoystick_effects[effect_id].di_effect,
        &gDirectInputEffects[gCount_joystick_effects],
        NULL);
    strcpy(gJoystick_effects[gCount_joystick_effects].name, name);
    gCount_joystick_effects += 1;
    return effect_id;
}

// FUNCTION: CARMA2_HW 0x0045a100
int C2_HOOK_FASTCALL CreateDinputEffectFromSpec(int pJoystickIndex, const tJoystick_effect_spec* pEffectSpec, const char* pName) {
    IDirectInputDevice2A* device;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tJoystick_effect_spec, effect, 0x10);
    C2_HOOK_BUG_ON(sizeof(DIEFFECT) != 0x34);

    memcpy(&gJoystick_effects[gCount_joystick_effects].di_effect, pEffectSpec->effect, sizeof(DIEFFECT));
    device = gDirectInputJoystickDevices[pJoystickIndex];
    IDirectInputDevice2_CreateEffect(device,
        &pEffectSpec->guid,
        &gJoystick_effects[gCount_joystick_effects].di_effect,
        &gDirectInputEffects[gCount_joystick_effects],
        NULL);
    strcpy(gJoystick_effects[gCount_joystick_effects].name, pName);
    gCount_joystick_effects += 1;
    return gCount_joystick_effects - 1;
}

// FUNCTION: CARMA2_HW 0x0045a390
int C2_NAKED C2_HOOK_FASTCALL ReadIforceEffectsNames(const char *path, char **names, size_t capacity) {
#ifndef CARPOCALYPSE2_MATCHING
    /* Implementation differs */

    FILE *f;
    tU32 count;
    size_t i;
    char buffer[128];

    f = fopen(path, "rb");
    if (f == NULL) {
        return -1;
    }
    rewind(f);
    fread(buffer, 1, 4, f);
    if (memcmp(buffer, "ifpr", 4) != 0) {
        return 0;
    }
    fread(&count, 1, sizeof(count), f);
    for (i = 0; i < count && i < capacity && !feof(f); i++) {
        tU32 chunk_size;
        size_t len_name = 0;
        char c;
        fread(&chunk_size, 1, sizeof(tU32), f);

        while ((c = fgetc(f)) != '\0' && len_name < CARPOCALYPSE2_ASIZE(buffer) && !feof(f)) {
            buffer[len_name] = c;
            len_name += 1;
        }
        names[i] = malloc(len_name + 1);
        strncpy(names[i], buffer, len_name);
        names[i][len_name] = '\0';
        fseek(f, chunk_size - len_name - 1 - 4, SEEK_CUR);
    }
    return i;
#else
    __asm {
            sub esp, 0x90
            push ebx
            push ebp
            push esi
            push edi
            push offset gCancel_recover+0x1a4
            mov esi, edx
            push ecx
            mov dword ptr [esp + 0x20], 0
            call fopen
            mov ebp, eax
            add esp, 8
            test ebp, ebp
            jne L0045a3c8
            or eax, 0xffffffff
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x90
            ret 4
    L0045a3c8:
            push ebp
            call rewind
            add esp, 4
            lea eax, [esp + 0x20]
            push ebp
            push 5
            push 1
            push eax
            call fread
            add esp, 0x10
            lea ecx, [esp + 0x20]
            push 4
            push offset gUNK_00595f9c+0x2ac
            push ecx
            call strncmp
            add esp, 0xc
            test eax, eax
            je L0045a40a
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x90
            ret 4
    L0045a40a:
            push 1
            push 7
            push ebp
            call fseek
            mov al, byte ptr [ebp + 0xc]
            add esp, 0xc
            test al, 0x10
            jne L0045a654
            mov dword ptr [esp + 0x1c], esi
    L0045a426:
            mov edx, dword ptr [esp + 0x18]
            mov eax, dword ptr [esp + 0xa4]
            cmp edx, eax
            jge L0045a654
            mov al, byte ptr [gDisable_sound+0x8]
            xor ebx, ebx
            push ebp
            mov byte ptr [esp + 0x24], al
            mov byte ptr [esp + 0x1b], bl
            call fgetc
            add esp, 4
            mov byte ptr [esp + 0x16], al
            test al, al
            je L0045a4a6
    L0045a459:
            cmp ebx, 0x80
            jge L0045a4a6
            test byte ptr [ebp + 0xc], 0x10
            jne L0045a4a6
            lea edi, [esp + 0x16]
            or ecx, 0xffffffff
            xor eax, eax
            lea edx, [esp + 0x20]
            repne scasb
            not ecx
            sub edi, ecx
            push ebp
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
            inc ebx
            rep movsb
            call fgetc
            add esp, 4
            mov byte ptr [esp + 0x16], al
            test al, al
            jne L0045a459
    L0045a4a6:
            lea edi, [esp + 0x20]
            or ecx, 0xffffffff
            xor eax, eax
            repne scasb
            not ecx
            push ecx
            call malloc
            mov edx, eax
            mov eax, dword ptr [esp + 0x20]
            lea edi, [esp + 0x24]
            or ecx, 0xffffffff
            mov dword ptr [eax], edx
            xor eax, eax
            add esp, 4
            xor ebx, ebx
            repne scasb
            not ecx
            sub edi, ecx
            push ebp
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            mov byte ptr [esp + 0x19], bl
            shr ecx, 2
            rep movsd
            mov ecx, eax
            and ecx, 3
            rep movsb
            mov cl, byte ptr [gDisable_sound+0x8]
            mov byte ptr [esp + 0x24], cl
            call fgetc
            add esp, 4
            mov byte ptr [esp + 0x14], al
            test al, al
            je L0045a553
    L0045a506:
            cmp ebx, 0x80
            jge L0045a553
            test byte ptr [ebp + 0xc], 0x10
            jne L0045a553
            lea edi, [esp + 0x14]
            or ecx, 0xffffffff
            xor eax, eax
            lea edx, [esp + 0x20]
            repne scasb
            not ecx
            sub edi, ecx
            push ebp
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
            inc ebx
            rep movsb
            call fgetc
            add esp, 4
            mov byte ptr [esp + 0x14], al
            test al, al
            jne L0045a506
    L0045a553:
            push 1
            push 0x2f
            push ebp
            call fseek
            add esp, 0xc
            mov byte ptr [esp + 0x13], 0
    L0045a565:
            mov al, byte ptr [gDisable_sound+0x8]
            push ebp
            xor ebx, ebx
            mov byte ptr [esp + 0x24], al
            call fgetc
            add esp, 4
            mov byte ptr [esp + 0x12], al
            test al, al
            je L0045a5ce
    L0045a581:
            cmp ebx, 0x80
            jge L0045a5ce
            test byte ptr [ebp + 0xc], 0x10
            jne L0045a5ce
            lea edi, [esp + 0x12]
            or ecx, 0xffffffff
            xor eax, eax
            lea edx, [esp + 0x20]
            repne scasb
            not ecx
            sub edi, ecx
            push ebp
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
            inc ebx
            rep movsb
            call fgetc
            add esp, 4
            mov byte ptr [esp + 0x12], al
            test al, al
            jne L0045a581
    L0045a5ce:
            movsx esi, byte ptr [ebp + 0xc]
            and esi, 0x10
            lea edi, [esp + 0x20]
            or ecx, 0xffffffff
            xor eax, eax
            shr esi, 4
            repne scasb
            not ecx
            dec ecx
            je L0045a5f0
            test esi, esi
            je L0045a565
    L0045a5f0:
            push ebp
            call fgetc
            add esp, 4
            movsx eax, al
            push ebp
            push eax
            call ungetc
            add esp, 8
            test eax, eax
            jne L0045a635
    L0045a60a:
            test esi, esi
            jne L0045a635
            push 1
            push 1
            push ebp
            call fseek
            add esp, 0xc
            push ebp
            call fgetc
            add esp, 4
            movsx ecx, al
            push ebp
            push ecx
            call ungetc
            add esp, 8
            test eax, eax
            je L0045a60a
    L0045a635:
            mov edx, dword ptr [esp + 0x18]
            mov ecx, dword ptr [esp + 0x1c]
            mov al, byte ptr [ebp + 0xc]
            inc edx
            add ecx, 4
            mov dword ptr [esp + 0x18], edx
            test al, 0x10
            mov dword ptr [esp + 0x1c], ecx
            je L0045a426
    L0045a654:
            mov eax, dword ptr [esp + 0x18]
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x90
            ret 4
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00458060
int C2_HOOK_FASTCALL UnloadDinputFFBEfectwithName(const char *name) {
    int i;

    for (i = 0; i < gCount_joystick_effects; i++) {
        if (strcmp(gJoystick_effects[i].name, name) == 0) {
            UnloadDinputFFBEffectAtIndex(i);
            return 1;
        }
    }
    return 0;
}

int C2_HOOK_FASTCALL ResetForceFeedback(void) {
	int result;
    int i;

    result = 0;
    if (gJoystickFFB != 0) {
        for (i = 0; i < MAX_COUNT_JOYSTICKS; i++) {
            IDirectInputDevice2A *device = gDirectInputJoystickDevices[i];
            if (device != NULL && (gJoystickFFB & (1 << i))) {
                IDirectInputDevice2_SendForceFeedbackCommand(device, DISFFC_RESET);
                result = 1;
            }
        }
    }
    return result;
}

// FUNCTION: CARMA2_HW 0x00457760
int C2_HOOK_FASTCALL InitForceFeedback(void) {
    tPath_name path;
    int count_effects;
    char *effect_names[20];
    int i;

    if (gCountEnumeratedJoystickDinputDevices == 0) {
        JoystickDInputBegin();
    }

    if (ResetForceFeedback()) {
        gForceFeedbackAvailable = 1;
    }
    if (!gForceFeedbackAvailable) {
        return 0;
    }
    PathCat(path, gApplication_path, "FFB.IFR");
    gIfr_project = IFLoadProjectFile(path, gDirectInputJoystickDevices[gJoystick_index]);
    if (gIfr_project == NULL) {
        PDFatalError("Cant enable I-Force");
    }
    count_effects = ReadIforceEffectsNames(path, effect_names, CARPOCALYPSE2_ASIZE(effect_names));
    for (i = 0; i < count_effects; i++) {
        int count;
        DIEFFECT di_effect;
        IFR_Effect **effects;

        effects = IFCreateEffectStructs(gIfr_project, effect_names[i], &count);
        di_effect = *effects[0]->di_effect;
        IDirectInputDevice2_CreateEffect(gDirectInputJoystickDevices[gJoystick_index], &effects[0]->guid, &di_effect, &gDirectInputEffects[gCount_joystick_effects], NULL);
        strcpy(gJoystick_effects[gCount_joystick_effects].name, effect_names[i]);
        gCount_joystick_effects += 1;
        UnloadDinputFFBEfectwithName(effect_names[i]);
    }
    IFReleaseProject(gIfr_project);
    return gForceFeedbackAvailable;
}

#if defined(CARPOCALYPSE2_MATCHING) && defined(_MSC_VER)
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x00457ff0
void C2_HOOK_FASTCALL ResetDInputJoystickFFB(int pIndex) {
    IDirectInputDevice2A *device;

    device = gDirectInputJoystickDevices[pIndex];
    if (device != NULL && (gJoystickFFB & (1 << pIndex))) {
        IDirectInputDevice2_SendForceFeedbackCommand(device, DISFFC_STOPALL);
    }
}

#if defined(CARPOCALYPSE2_MATCHING) && defined(_MSC_VER)
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00458520
void C2_HOOK_FASTCALL RegisterJoystickFFBForces(void) {
    tJoystick_effect_description description;

#ifdef CARPOCALYPSE2_FIX_BUGS
    memset(&description, 0, sizeof(description));
#endif
    description.type = eJoystick_effect_Friction;
    description.type_specific_params.friction.field_0x0 = 0;
    description.type_specific_params.friction.field_0x4 = 10000;
    description.type_specific_params.friction.field_0x8 = 10000;
    description.time_ms = 5000000;
    description.field_0x3c = 5000;
    description.field_0x44 = -1;
    description.field_0x4c = 0x5a;
    gBasic_friction_joystick_effect_index = CreateDinputEffect(gJoystick_index, &description, "Basic Friction");

#ifdef CARPOCALYPSE2_FIX_BUGS
    memset(&description, 0, sizeof(description));
#endif
    description.type = eJoystick_effect_ConstantForce;
    description.type_specific_params.constant.field_0x0 = 0;
    description.time_ms = 5000000;
    description.field_0x38 = 1000;
    description.field_0x3c = 5000;
    description.field_0x44 = -1;
    description.field_0x4c = 0x5a;
    gBasic_force_joystick_effect_index = CreateDinputEffect(gJoystick_index, &description, "Basic Force");

#ifdef CARPOCALYPSE2_FIX_BUGS
    memset(&description, 0, sizeof(description));
#endif
    description.type = eJoystick_effect_ConstantForce;
    description.type_specific_params.constant.field_0x0 = 0;
    description.time_ms = 1000000;
    description.field_0x38 = 1000;
    description.field_0x3c = 5000;
    description.field_0x44 = -1;
    description.field_0x4c = 0x5a;
    gCollision_force_joystick_effect_index = CreateDinputEffect(gJoystick_index, &description, "Collision Force");
}

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x00457eb0
void C2_HOOK_FASTCALL StartDinputEffect(int pIndex) {
    IDirectInputEffect* effect;

    effect = gDirectInputEffects[pIndex];
    if (effect != NULL) {
        IDirectInputEffect_Start(effect, 1, 0);
    }
}

// FUNCTION: CARMA2_HW 0x00457ed0
void C2_HOOK_FASTCALL StopDinputEffect(int pIndex) {
    IDirectInputEffect* effect;

    effect = gDirectInputEffects[pIndex];
    if (effect != NULL) {
        IDirectInputEffect_Stop(effect);
    }
}

// FUNCTION: CARMA2_HW 0x00457ef0
int C2_NAKED C2_HOOK_FASTCALL StartJoystickEffectByName(const char* pName) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;

    for (i = 0; i < gCount_joystick_effects; i++) {
        if (strcmp(pName, gJoystick_effects[i].name) == 0) {
            StartDinputEffect(i);
            return 1;
        }
    }
    return 0;
#else
    __asm {
            mov eax, dword ptr [gCount_joystick_effects]
            push ebx
            push ebp
            xor ebp, ebp
            push esi
            push edi
            test eax, eax
            jle L00457f5d
            mov edi, offset gJoystick_effects+0x68
    L00457f04:
            mov esi, ecx
            mov eax, edi
    L00457f08:
            mov bl, byte ptr [eax]
            mov dl, bl
            cmp bl, byte ptr [esi]
            jne L00457f2c
            test dl, dl
            je L00457f28
            mov bl, byte ptr [eax + 1]
            mov dl, bl
            cmp bl, byte ptr [esi + 1]
            jne L00457f2c
            add eax, 2
            add esi, 2
            test dl, dl
            jne L00457f08
    L00457f28:
            xor eax, eax
            jmp L00457f31
    L00457f2c:
            sbb eax, eax
            sbb eax, -1
    L00457f31:
            test eax, eax
            je L00457f4c
            mov eax, dword ptr [gCount_joystick_effects]
            inc ebp
            add edi, 0xe8
            cmp ebp, eax
            jl L00457f04
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret
    L00457f4c:
            mov ecx, ebp
            call StartDinputEffect
            mov eax, 1
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret
    L00457f5d:
            pop edi
            pop esi
            pop ebp
            xor eax, eax
            pop ebx
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
            mov eax, dword ptr [gCount_joystick_effects]
            push ebx
            push ebp
            xor ebp, ebp
            push esi
            push edi
            test eax, eax
            jle L00457fdd
            mov edi, offset gJoystick_effects+0x68
    L00457f84:
            mov esi, ecx
            mov eax, edi
    L00457f88:
            mov bl, byte ptr [eax]
            mov dl, bl
            cmp bl, byte ptr [esi]
            jne L00457fac
            test dl, dl
            je L00457fa8
            mov bl, byte ptr [eax + 1]
            mov dl, bl
            cmp bl, byte ptr [esi + 1]
            jne L00457fac
            add eax, 2
            add esi, 2
            test dl, dl
            jne L00457f88
    L00457fa8:
            xor eax, eax
            jmp L00457fb1
    L00457fac:
            sbb eax, eax
            sbb eax, -1
    L00457fb1:
            test eax, eax
            je L00457fcc
            mov eax, dword ptr [gCount_joystick_effects]
            inc ebp
            add edi, 0xe8
            cmp ebp, eax
            jl L00457f84
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret
    L00457fcc:
            mov ecx, ebp
            call StopDinputEffect
            mov eax, 1
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret
    L00457fdd:
            pop edi
            pop esi
            pop ebp
            xor eax, eax
            pop ebx
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
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00458610
int C2_HOOK_FASTCALL SetDinputEffectGain(int pIndex, int pGain) {
    IDirectInputEffect* effect;

    if (!gForceFeedbackAvailable) {
        return 0;
    }
    if (gJoystick_effects[pIndex].di_effect.dwGain != pGain) {
        gJoystick_effects[pIndex].di_effect.dwGain = pGain;
    }
    effect = gDirectInputEffects[pIndex];
    if (effect != NULL) {
        IDirectInputEffect_SetParameters(effect, &gJoystick_effects[pIndex].di_effect, DIEP_GAIN);
    }
    return 1;
}
#pragma auto_inline(on)

// FUNCTION: CARMA2_HW 0x00458660
int C2_HOOK_FASTCALL SetJoystickFFBForce(int pMagnitude) {
    if (!gForceFeedbackAvailable) {
        return 0;
    }
    if (pMagnitude < 1) {
        StopDinputEffect(gBasic_friction_joystick_effect_index);
        return 1;
    }
    if (pMagnitude > 10000) {
        pMagnitude = 10000;
    }
    SetDinputEffectGain(gBasic_friction_joystick_effect_index, pMagnitude);
    StartDinputEffect(gBasic_friction_joystick_effect_index);
    return 1;
}

// FUNCTION: CARMA2_HW 0x004586b0
int C2_HOOK_FASTCALL SetJoystickFFBSteeringForce(int pMagnitude) {
    IDirectInputEffect* effect;
    int index;

    if (gForceFeedbackAvailable) {
        if (pMagnitude > 10000) {
            pMagnitude = 10000;
        }
        index = gBasic_force_joystick_effect_index;
        if (*(LONG*)&gJoystick_effects[index].type_specific_params != pMagnitude) {
            *(LONG*)&gJoystick_effects[index].type_specific_params = pMagnitude;
            effect = gDirectInputEffects[index];
            if (effect != NULL) {
                IDirectInputEffect_SetParameters(effect, &gJoystick_effects[index].di_effect, DIEP_TYPESPECIFICPARAMS);
            }
            StartDinputEffect(gBasic_force_joystick_effect_index);
            return 1;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00458720
int C2_HOOK_FASTCALL SetJoystickFFBCollisionForce(LONG pMagnitude, LONG pDirection) {
    if (!gForceFeedbackAvailable) {
        return 0;
    }
    if (pMagnitude > 10000) {
        pMagnitude = 10000;
    }
    *(LONG*)&gJoystick_effects[gCollision_force_joystick_effect_index].type_specific_params = pMagnitude;
    gJoystick_effects[gCollision_force_joystick_effect_index].field_0x60 = pDirection;
    if (gDirectInputEffects[gCollision_force_joystick_effect_index] != NULL) {
        IDirectInputEffect_SetParameters(gDirectInputEffects[gCollision_force_joystick_effect_index], &gJoystick_effects[gCollision_force_joystick_effect_index].di_effect, DIEP_TYPESPECIFICPARAMS);
        IDirectInputEffect_SetParameters(gDirectInputEffects[gCollision_force_joystick_effect_index], &gJoystick_effects[gCollision_force_joystick_effect_index].di_effect, DIEP_DIRECTION);
    }
    StartDinputEffect(gCollision_force_joystick_effect_index);
    return 1;
}

// FUNCTION: CARMA2_HW 0x00458860
int C2_HOOK_FASTCALL JoystickDInputBegin(void) {
    int i;
    if (InitDirectInput()) {
        return 0;
    }
    if (!gDirectInputJoystickEnumerated) {
        gDirectInputJoystickEnumerated = 1;
        if (gCountEnumeratedJoystickDinputDevices == 0) {
            IDirectInput_EnumDevices(gDirectInputJoystickHandle, DIDEVTYPE_JOYSTICK, Win32DInputJoystickEnum, gDirectInputJoystickHandle, DIEDFL_ATTACHEDONLY);
        }
        if (gCountEnumeratedJoystickDinputDevices != 0) {
            for (i = 0; i < MAX_COUNT_JOYSTICKS; i++) {
                JoystickDInputGetInfo(i, NULL, NULL);
                AcquireDInputJoystickDevice(i);
            }
            gJoystick_index = 0;
        }
        if (gJoystick_index < 0) {
            return 0;
        }
        if (InitForceFeedback()) {
            ResetDInputJoystickFFB(gJoystick_index);
            RegisterJoystickFFBForces();
        }
    }
    return gCountEnumeratedJoystickDinputDevices;
}

// FUNCTION: CARMA2_HW 0x0051ba10
void C2_NAKED C2_HOOK_FASTCALL KeyBegin(void) {
#ifndef CARPOCALYPSE2_MATCHING
    memset(gASCII_table, 0, sizeof(gASCII_table));
    memset(gASCII_shift_table, 0, sizeof(gASCII_shift_table));

    gASCII_table[11] = '0';
    gASCII_table[12] = '1';
    gASCII_table[13] = '2';
    gASCII_table[14] = '3';
    gASCII_table[15] = '4';
    gASCII_table[16] = '5';
    gASCII_table[17] = '6';
    gASCII_table[18] = '7';
    gASCII_table[19] = '8';
    gASCII_table[20] = '9';
    gASCII_table[21] = 'a';
    gASCII_table[22] = 'b';
    gASCII_table[23] = 'c';
    gASCII_table[24] = 'd';
    gASCII_table[25] = 'e';
    gASCII_table[26] = 'f';
    gASCII_table[27] = 'g';
    gASCII_table[28] = 'h';
    gASCII_table[29] = 'i';
    gASCII_table[30] = 'j';
    gASCII_table[31] = 'k';
    gASCII_table[32] = 'l';
    gASCII_table[33] = 'm';
    gASCII_table[34] = 'n';
    gASCII_table[35] = 'o';
    gASCII_table[36] = 'p';
    gASCII_table[37] = 'q';
    gASCII_table[38] = 'r';
    gASCII_table[39] = 's';
    gASCII_table[40] = 't';
    gASCII_table[41] = 'u';
    gASCII_table[42] = 'v';
    gASCII_table[43] = 'w';
    gASCII_table[44] = 'x';
    gASCII_table[45] = 'y';
    gASCII_table[46] = 'z';
    gASCII_table[48] = '-';
    gASCII_table[49] = '=';
    gASCII_table[50] = 0x7f;
    gASCII_table[51] = '\r';
    gASCII_table[52] = '\r';
    gASCII_table[53] = '\t';
    gASCII_table[58] = '.';
    gASCII_table[59] = ',';
    gASCII_table[63] = 0x1b;
    gASCII_table[106] = ' ';

    memcpy(gASCII_shift_table, gASCII_table, sizeof(gASCII_table));

    gASCII_shift_table[21] = 'A';
    gASCII_shift_table[22] = 'B';
    gASCII_shift_table[23] = 'C';
    gASCII_shift_table[24] = 'D';
    gASCII_shift_table[25] = 'E';
    gASCII_shift_table[26] = 'F';
    gASCII_shift_table[27] = 'G';
    gASCII_shift_table[28] = 'H';
    gASCII_shift_table[29] = 'I';
#if defined(CARPOCALYPSE2_FIX_BUGS)
    gASCII_shift_table[30] = 'J';
#endif
    gASCII_shift_table[31] = 'K';
    gASCII_shift_table[32] = 'L';
    gASCII_shift_table[33] = 'M';
    gASCII_shift_table[34] = 'N';
    gASCII_shift_table[35] = 'O';
    gASCII_shift_table[36] = 'P';
    gASCII_shift_table[37] = 'Q';
    gASCII_shift_table[38] = 'R';
    gASCII_shift_table[39] = 'S';
    gASCII_shift_table[40] = 'T';
    gASCII_shift_table[41] = 'U';
    gASCII_shift_table[42] = 'V';
    gASCII_shift_table[43] = 'W';
    gASCII_shift_table[44] = 'X';
    gASCII_shift_table[45] = 'Y';
    gASCII_shift_table[46] = 'Z';

    gScan_code[12] = 0xff;
    gScan_code[13] = 0xff;
    gScan_code[14] = 0xff;
    gScan_code[15] = 0xff;
    gScan_code[16] = DIK_CAPITAL;
    gScan_code[17] = DIK_RSHIFT;
    gScan_code[18] = DIK_RMENU;
    gScan_code[19] = DIK_RCONTROL;
    gScan_code[20] = DIK_LSHIFT;
    gScan_code[21] = DIK_LMENU;
    gScan_code[22] = DIK_LCONTROL;
    gScan_code[23] = DIK_0;
    gScan_code[24] = DIK_1;
    gScan_code[25] = DIK_2;
    gScan_code[26] = DIK_3;
    gScan_code[27] = DIK_4;
    gScan_code[28] = DIK_5;
    gScan_code[29] = DIK_6;
    gScan_code[30] = DIK_7;
    gScan_code[31] = DIK_8;
    gScan_code[32] = DIK_9;
    gScan_code[33] = DIK_A;
    gScan_code[34] = DIK_B;
    gScan_code[35] = DIK_C;
    gScan_code[36] = DIK_D;
    gScan_code[37] = DIK_E;
    gScan_code[38] = DIK_F;
    gScan_code[39] = DIK_G;
    gScan_code[40] = DIK_H;
    gScan_code[41] = DIK_I;
    gScan_code[42] = DIK_J;
    gScan_code[43] = DIK_K;
    gScan_code[44] = DIK_L;
    gScan_code[45] = DIK_M;
    gScan_code[46] = DIK_N;
    gScan_code[47] = DIK_O;
    gScan_code[48] = DIK_P;
    gScan_code[49] = DIK_Q;
    gScan_code[50] = DIK_R;
    gScan_code[51] = DIK_S;
    gScan_code[52] = DIK_T;
    gScan_code[53] = DIK_U;
    gScan_code[54] = DIK_V;
    gScan_code[55] = DIK_W;
    gScan_code[56] = DIK_X;
    gScan_code[57] = DIK_Y;
    gScan_code[58] = DIK_Z;
    gScan_code[59] = DIK_GRAVE;
    gScan_code[60] = DIK_MINUS;
    gScan_code[61] = DIK_EQUALS;
    gScan_code[62] = DIK_BACK;
    gScan_code[63] = DIK_RETURN;
    gScan_code[64] = DIK_NUMPADENTER;
    gScan_code[65] = DIK_TAB;
    gScan_code[66] = DIK_SLASH;
    gScan_code[67] = DIK_BACKSLASH;
    gScan_code[68] = DIK_SEMICOLON;
    gScan_code[69] = DIK_APOSTROPHE;
    gScan_code[70] = DIK_PERIOD;
    gScan_code[71] = DIK_COMMA;
    gScan_code[72] = DIK_LBRACKET;
    gScan_code[73] = DIK_RBRACKET;
    gScan_code[74] = 0;
    gScan_code[75] = DIK_ESCAPE;
    gScan_code[76] = DIK_INSERT;
    gScan_code[77] = DIK_DELETE;
    gScan_code[78] = DIK_HOME;
    gScan_code[79] = DIK_END;
    gScan_code[80] = DIK_PRIOR;
    gScan_code[81] = DIK_NEXT;
    gScan_code[82] = DIK_LEFT;
    gScan_code[83] = DIK_RIGHT;
    gScan_code[84] = DIK_UP;
    gScan_code[85] = DIK_DOWN;
    gScan_code[86] = DIK_NUMLOCK;
    gScan_code[87] = DIK_DIVIDE;
    gScan_code[88] = DIK_MULTIPLY;
    gScan_code[89] = DIK_SUBTRACT;
    gScan_code[90] = DIK_ADD;
    gScan_code[91] = DIK_DECIMAL;
    gScan_code[92] = 0;
    gScan_code[93] = DIK_NUMPAD0;
    gScan_code[94] = DIK_NUMPAD1;
    gScan_code[95] = DIK_NUMPAD2;
    gScan_code[96] = DIK_NUMPAD3;
    gScan_code[97] = DIK_NUMPAD4;
    gScan_code[98] = DIK_NUMPAD5;
    gScan_code[99] = DIK_NUMPAD6;
    gScan_code[100] = DIK_NUMPAD7;
    gScan_code[101] = DIK_NUMPAD8;
    gScan_code[102] = DIK_NUMPAD9;
    gScan_code[103] = DIK_F1;
    gScan_code[104] = DIK_F2;
    gScan_code[105] = DIK_F3;
    gScan_code[106] = DIK_F4;
    gScan_code[107] = DIK_F5;
    gScan_code[108] = DIK_F6;
    gScan_code[109] = DIK_F7;
    gScan_code[110] = DIK_F8;
    gScan_code[111] = DIK_F9;
    gScan_code[112] = DIK_F10;
    gScan_code[113] = DIK_F11;
    gScan_code[114] = DIK_F12;
    gScan_code[115] = DIK_SYSRQ;
    gScan_code[116] = DIK_SCROLL;
    gScan_code[117] = 0xff;
    gScan_code[118] = DIK_SPACE;
#else
    __asm {
            push esi
            push edi
            mov ecx, 0x97
            xor eax, eax
            mov edi, offset gASCII_table
            mov esi, offset gASCII_table
            rep stosd
            mov ecx, 0x97
            mov edi, offset gASCII_shift_table
            rep stosd
            mov eax, 0xd
            mov ecx, 0x97
            mov edi, offset gASCII_shift_table
            mov dword ptr [gASCII_table+0x2c], 0x30
            mov dword ptr [gASCII_table+0x30], 0x31
            mov dword ptr [gASCII_table+0x34], 0x32
            mov dword ptr [gASCII_table+0x38], 0x33
            mov dword ptr [gASCII_table+0x3c], 0x34
            mov dword ptr [gASCII_table+0x40], 0x35
            mov dword ptr [gASCII_table+0x44], 0x36
            mov dword ptr [gASCII_table+0x48], 0x37
            mov dword ptr [gASCII_table+0x4c], 0x38
            mov dword ptr [gASCII_table+0x50], 0x39
            mov dword ptr [gASCII_table+0x54], 0x61
            mov dword ptr [gASCII_table+0x58], 0x62
            mov dword ptr [gASCII_table+0x5c], 0x63
            mov dword ptr [gASCII_table+0x60], 0x64
            mov dword ptr [gASCII_table+0x64], 0x65
            mov dword ptr [gASCII_table+0x68], 0x66
            mov dword ptr [gASCII_table+0x6c], 0x67
            mov dword ptr [gASCII_table+0x70], 0x68
            mov dword ptr [gASCII_table+0x74], 0x69
            mov dword ptr [gASCII_table+0x78], 0x6a
            mov dword ptr [gASCII_table+0x7c], 0x6b
            mov dword ptr [gASCII_table+0x80], 0x6c
            mov dword ptr [gASCII_table+0x84], 0x6d
            mov dword ptr [gASCII_table+0x88], 0x6e
            mov dword ptr [gASCII_table+0x8c], 0x6f
            mov dword ptr [gASCII_table+0x90], 0x70
            mov dword ptr [gASCII_table+0x94], 0x71
            mov dword ptr [gASCII_table+0x98], 0x72
            mov dword ptr [gASCII_table+0x9c], 0x73
            mov dword ptr [gASCII_table+0xa0], 0x74
            mov dword ptr [gASCII_table+0xa4], 0x75
            mov dword ptr [gASCII_table+0xa8], 0x76
            mov dword ptr [gASCII_table+0xac], 0x77
            mov dword ptr [gASCII_table+0xb0], 0x78
            mov dword ptr [gASCII_table+0xb4], 0x79
            mov dword ptr [gASCII_table+0xb8], 0x7a
            mov dword ptr [gASCII_table+0xc0], 0x2d
            mov dword ptr [gASCII_table+0xc4], 0x3d
            mov dword ptr [gASCII_table+0xc8], 0x7f
            mov dword ptr [gASCII_table+0xcc], eax
            mov dword ptr [gASCII_table+0xd0], eax
            mov dword ptr [gASCII_table+0xd4], 9
            mov dword ptr [gASCII_table+0xe8], 0x2e
            mov dword ptr [gASCII_table+0xec], 0x2c
            mov dword ptr [gASCII_table+0xfc], 0x1b
            mov dword ptr [gASCII_table+0x1a8], 0x20
            rep movsd
            mov edi, 0x52
            mov esi, 0x53
            mov dword ptr [gASCII_shift_table+0x54], 0x41
            mov dword ptr [gASCII_shift_table+0x58], 0x42
            mov dword ptr [gASCII_shift_table+0x5c], 0x43
            mov dword ptr [gASCII_shift_table+0x60], 0x44
            mov dword ptr [gASCII_shift_table+0x64], 0x45
            mov dword ptr [gASCII_shift_table+0x68], 0x46
            mov dword ptr [gASCII_shift_table+0x6c], 0x47
            mov dword ptr [gASCII_shift_table+0x70], 0x48
            mov dword ptr [gASCII_shift_table+0x74], 0x49
            mov dword ptr [gASCII_shift_table+0x7c], 0x4b
            mov dword ptr [gASCII_shift_table+0x80], 0x4c
            mov dword ptr [gASCII_shift_table+0x84], 0x4d
            mov dword ptr [gASCII_shift_table+0x88], 0x4e
            mov dword ptr [gASCII_shift_table+0x8c], 0x4f
            mov dword ptr [gASCII_shift_table+0x90], 0x50
            mov dword ptr [gASCII_shift_table+0x94], 0x51
            mov dword ptr [gASCII_shift_table+0x98], edi
            mov dword ptr [gASCII_shift_table+0x9c], esi
            mov dword ptr [gASCII_shift_table+0xa0], 0x54
            mov edx, 0x57
            mov ecx, 0x58
            mov dword ptr [gASCII_shift_table+0xa4], 0x55
            mov dword ptr [gASCII_shift_table+0xa8], 0x56
            mov dword ptr [gASCII_shift_table+0xac], edx
            mov dword ptr [gASCII_shift_table+0xb0], ecx
            mov dword ptr [gASCII_shift_table+0xb4], 0x59
            mov dword ptr [gASCII_shift_table+0xb8], 0x5a
            mov dword ptr [gScan_code+0x5c], 0xb
            mov dword ptr [gScan_code+0x60], 2
            mov dword ptr [gScan_code+0x64], 3
            mov dword ptr [gScan_code+0x68], 4
            mov dword ptr [gScan_code+0x6c], 5
            mov dword ptr [gScan_code+0x70], 6
            mov dword ptr [gScan_code+0x74], 7
            mov dword ptr [gScan_code+0x78], 8
            mov dword ptr [gScan_code+0x7c], 9
            mov dword ptr [gScan_code+0x80], 0xa
            mov dword ptr [gScan_code+0x84], 0x1e
            mov dword ptr [gScan_code+0x88], 0x30
            mov dword ptr [gScan_code+0x8c], 0x2e
            mov dword ptr [gScan_code+0x90], 0x20
            mov dword ptr [gScan_code+0x94], 0x12
            mov dword ptr [gScan_code+0x98], 0x21
            mov dword ptr [gScan_code+0x9c], 0x22
            mov dword ptr [gScan_code+0xa0], 0x23
            mov dword ptr [gScan_code+0xa4], 0x17
            mov dword ptr [gScan_code+0xa8], 0x24
            mov dword ptr [gScan_code+0xac], 0x25
            mov dword ptr [gScan_code+0xb0], 0x26
            mov dword ptr [gScan_code+0xb4], 0x32
            mov dword ptr [gScan_code+0xb8], 0x31
            mov dword ptr [gScan_code+0xbc], 0x18
            mov dword ptr [gScan_code+0xc0], 0x19
            mov dword ptr [gScan_code+0xc4], 0x10
            mov dword ptr [gScan_code+0xc8], 0x13
            mov dword ptr [gScan_code+0xcc], 0x1f
            mov dword ptr [gScan_code+0xd0], 0x14
            mov dword ptr [gScan_code+0xd4], 0x16
            mov dword ptr [gScan_code+0xd8], 0x2f
            mov dword ptr [gScan_code+0xdc], 0x11
            mov dword ptr [gScan_code+0xe0], 0x2d
            mov dword ptr [gScan_code+0xe4], 0x15
            mov dword ptr [gScan_code+0xe8], 0x2c
            mov dword ptr [gScan_code+0xec], 0x29
            mov dword ptr [gScan_code+0xf0], 0xc
            mov dword ptr [gScan_code+0xf4], eax
            mov dword ptr [gScan_code+0xf8], 0xe
            mov dword ptr [gScan_code+0xfc], 0x1c
            mov dword ptr [gScan_code+0x100], 0x9c
            mov dword ptr [gScan_code+0x40], 0x3a
            mov dword ptr [gScan_code+0x104], 0xf
            mov dword ptr [gScan_code+0x108], 0x35
            mov dword ptr [gScan_code+0x10c], 0x2b
            mov dword ptr [gScan_code+0x110], 0x27
            mov dword ptr [gScan_code+0x114], 0x28
            mov dword ptr [gScan_code+0x118], 0x34
            mov dword ptr [gScan_code+0x11c], 0x33
            mov dword ptr [gScan_code+0x120], 0x1a
            mov dword ptr [gScan_code+0x124], 0x1b
            mov dword ptr [gScan_code+0x12c], 1
            mov dword ptr [gScan_code+0x130], 0xd2
            mov dword ptr [gScan_code+0x134], 0xd3
            mov dword ptr [gScan_code+0x138], 0xc7
            mov dword ptr [gScan_code+0x13c], 0xcf
            mov dword ptr [gScan_code+0x140], 0xc9
            mov dword ptr [gScan_code+0x144], 0xd1
            mov dword ptr [gScan_code+0x148], 0xcb
            mov dword ptr [gScan_code+0x14c], 0xcd
            mov dword ptr [gScan_code+0x150], 0xc8
            mov dword ptr [gScan_code+0x154], 0xd0
            mov dword ptr [gScan_code+0x158], 0x45
            mov dword ptr [gScan_code+0x15c], 0xb5
            mov dword ptr [gScan_code+0x160], 0x37
            mov dword ptr [gScan_code+0x164], 0x4a
            mov dword ptr [gScan_code+0x168], 0x4e
            mov dword ptr [gScan_code+0x16c], esi
            mov dword ptr [gScan_code+0x174], edi
            mov dword ptr [gScan_code+0x178], 0x4f
            mov dword ptr [gScan_code+0x17c], 0x50
            mov dword ptr [gScan_code+0x180], 0x51
            xor eax, eax
            pop edi
            mov dword ptr [gScan_code+0x170], eax
            mov dword ptr [gScan_code+0x128], eax
            mov eax, 0xff
            mov dword ptr [gScan_code+0x184], 0x4b
            mov dword ptr [gScan_code+0x188], 0x4c
            mov dword ptr [gScan_code+0x18c], 0x4d
            mov dword ptr [gScan_code+0x190], 0x47
            mov dword ptr [gScan_code+0x194], 0x48
            mov dword ptr [gScan_code+0x198], 0x49
            mov dword ptr [gScan_code+0x19c], 0x3b
            mov dword ptr [gScan_code+0x1a0], 0x3c
            mov dword ptr [gScan_code+0x1a4], 0x3d
            mov dword ptr [gScan_code+0x1a8], 0x3e
            mov dword ptr [gScan_code+0x1ac], 0x3f
            mov dword ptr [gScan_code+0x1b0], 0x40
            mov dword ptr [gScan_code+0x1b4], 0x41
            mov dword ptr [gScan_code+0x1b8], 0x42
            mov dword ptr [gScan_code+0x1bc], 0x43
            mov dword ptr [gScan_code+0x1c0], 0x44
            mov dword ptr [gScan_code+0x1c4], edx
            mov dword ptr [gScan_code+0x1c8], ecx
            mov dword ptr [gScan_code+0x1cc], 0xb7
            mov dword ptr [gScan_code+0x1d0], 0x46
            mov dword ptr [gScan_code+0x1d8], 0x39
            mov dword ptr [gScan_code+0x44], 0x36
            mov dword ptr [gScan_code+0x48], 0xb8
            mov dword ptr [gScan_code+0x4c], 0x9d
            mov dword ptr [gScan_code+0x50], 0x2a
            mov dword ptr [gScan_code+0x54], 0x38
            mov dword ptr [gScan_code+0x58], 0x1d
            mov dword ptr [gScan_code+0x3c], eax
            mov dword ptr [gScan_code+0x1d4], eax
            mov dword ptr [gScan_code+0x30], eax
            mov dword ptr [gScan_code+0x34], eax
            mov dword ptr [gScan_code+0x38], eax
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

// FUNCTION: CARMA2_HW 0x0051cbf0
void C2_NAKED C2_HOOK_FASTCALL Win32InitInputDevice(void) {
#ifndef CARPOCALYPSE2_MATCHING
    HRESULT hRes;
    GUID guid_sysKeyboard;

    hRes = DirectInputCreateA(gHInstance, DIRECTINPUT_VERSION, &gDirectInput, NULL);
    if (hRes != S_OK) {
        PDFatalError("Unable to create DirectInput object - please check that DirectX is installed");
    }

    guid_sysKeyboard = GUID_SysKeyboard;
    hRes = IDirectInput_CreateDevice(gDirectInput, &guid_sysKeyboard, &gDirectInputDevice, NULL);
    if (hRes != S_OK) {
        PDFatalError("Direct Input: Can't create device");
    }

    hRes = IDirectInputDevice_SetDataFormat(gDirectInputDevice, &c_dfDIKeyboard);
    if (hRes != S_OK) {
        PDFatalError("Direct Input: Can't create device");
    }

    hRes = IDirectInputDevice_SetCooperativeLevel(gDirectInputDevice, gHWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
    if (hRes != S_OK) {
        PDFatalError("Direct Input: Can't set keyboard cooperative level");
    }

    hRes = IDirectInputDevice_Acquire(gDirectInputDevice);
    if (!SUCCEEDED(hRes)) {
        dr_dprintf("ERROR: Can't aquire keyboard; HRESULT %x", hRes);
    }
    if (JoystickDInputBegin()) {
        CollectJoystickButtonInfos();
        LoadJoystickPreferences();
    }
    gJoystick_deadzone = 8000;
#else
    __asm {
            sub esp, 0x10
            mov ecx, dword ptr [IID_IDirectInputDevice2A+0xf4]
            mov eax, dword ptr [IID_IDirectInputDevice2A+0xf0]
            mov edx, dword ptr [IID_IDirectInputDevice2A+0xf8]
            mov dword ptr [esp + 4], ecx
            mov ecx, dword ptr [gHInstance]
            push 0
            mov dword ptr [esp + 4], eax
            mov eax, dword ptr [IID_IDirectInputDevice2A+0xfc]
            push offset gDirectInput
            push 0x500
            push ecx
            mov dword ptr [esp + 0x18], edx
            mov dword ptr [esp + 0x1c], eax
            call DirectInputCreateA
            test eax, eax
            je L0051ccaf
            push offset gRegionDescriminator+0xdbc
            push offset gRegionDescriminator+0x1bc
            call dr_dprintf
            mov ecx, offset gRegionDescriminator+0xdbc
            add esp, 8
            mov edx, ecx
            mov eax, offset gDisable_sound+0x8
            test edx, edx
            jne L0051cc5c
            mov ecx, offset gRegionDescriminator+0x1b0
    L0051cc5c:
            mov edx, offset gDisable_sound+0x8
            test edx, edx
            jne L0051cc6a
            mov eax, offset gRegionDescriminator+0x1a4
    L0051cc6a:
            push eax
            push ecx
            push offset gRegionDescriminator+0x19c
            push offset gFatalErrorMessage
            mov dword ptr [gIsFatalError], 1
            call sprintf
            mov eax, dword ptr [gBack_screen]
            add esp, 0x10
            test eax, eax
            mov dword ptr [gExit_code], 0x2bc
            je L0051ce90
            mov ecx, dword ptr [eax + 8]
            test ecx, ecx
            je L0051ce90
            jmp L0051ce86
    L0051ccaf:
            mov eax, dword ptr [gDirectInput]
            push 0
            lea edx, [esp + 4]
            push offset gDirectInputDevice
            mov ecx, dword ptr [eax]
            push edx
            push eax
            call dword ptr [ecx + 0xc]
            test eax, eax
            je L0051cd44
            push offset gRegionDescriminator+0xd98
            push offset gRegionDescriminator+0x1bc
            call dr_dprintf
            mov ecx, offset gRegionDescriminator+0xd98
            add esp, 8
            mov edx, ecx
            mov eax, offset gDisable_sound+0x8
            test edx, edx
            jne L0051ccf1
            mov ecx, offset gRegionDescriminator+0x1b0
    L0051ccf1:
            mov edx, offset gDisable_sound+0x8
            test edx, edx
            jne L0051ccff
            mov eax, offset gRegionDescriminator+0x1a4
    L0051ccff:
            push eax
            push ecx
            push offset gRegionDescriminator+0x19c
            push offset gFatalErrorMessage
            mov dword ptr [gIsFatalError], 1
            call sprintf
            mov eax, dword ptr [gBack_screen]
            add esp, 0x10
            test eax, eax
            mov dword ptr [gExit_code], 0x2bc
            je L0051ce90
            mov ecx, dword ptr [eax + 8]
            test ecx, ecx
            je L0051ce90
            jmp L0051ce86
    L0051cd44:
            mov eax, dword ptr [gDirectInputDevice]
            push 0x54e330
            push eax
            mov ecx, dword ptr [eax]
            call dword ptr [ecx + 0x2c]
            test eax, eax
            je L0051cdfd
            push offset gRegionDescriminator+0xd74
            push offset gRegionDescriminator+0x1bc
            call dr_dprintf
            mov ecx, offset gRegionDescriminator+0xd74
            add esp, 8
            mov edx, ecx
            mov eax, offset gDisable_sound+0x8
            test edx, edx
            jne L0051ce40
            mov edx, eax
            mov ecx, offset gRegionDescriminator+0x1b0
            test edx, edx
            jne L0051ce4e
            mov eax, offset gRegionDescriminator+0x1a4
            mov dword ptr [gIsFatalError], 1
            push eax
            push ecx
            push offset gRegionDescriminator+0x19c
            push offset gFatalErrorMessage
            call sprintf
            mov eax, dword ptr [gBack_screen]
            add esp, 0x10
            test eax, eax
            mov dword ptr [gExit_code], 0x2bc
            je L0051ce90
            mov ecx, dword ptr [eax + 8]
            test ecx, ecx
            je L0051ce90
            mov ecx, 1
            call PossibleUnlock
            mov eax, dword ptr [gBr_initialized]
            test eax, eax
            je L0051ce9e
            call DRBrEnd
            call PDShutdownSystem
            xor eax, eax
            add esp, 0x10
            ret
    L0051cdfd:
            mov eax, dword ptr [gDirectInputDevice]
            mov edx, dword ptr [gHWnd]
            push 6
            push edx
            mov ecx, dword ptr [eax]
            push eax
            call dword ptr [ecx + 0x34]
            test eax, eax
            je L0051cea9
            push offset gRegionDescriminator+0xd40
            push offset gRegionDescriminator+0x1bc
            call dr_dprintf
            mov ecx, offset gRegionDescriminator+0xd40
            add esp, 8
            mov edx, ecx
            mov eax, offset gDisable_sound+0x8
            test edx, edx
            jne L0051ce40
            mov ecx, offset gRegionDescriminator+0x1b0
    L0051ce40:
            mov edx, offset gDisable_sound+0x8
            test edx, edx
            jne L0051ce4e
            mov eax, offset gRegionDescriminator+0x1a4
    L0051ce4e:
            push eax
            push ecx
            push offset gRegionDescriminator+0x19c
            push offset gFatalErrorMessage
            mov dword ptr [gIsFatalError], 1
            call sprintf
            mov eax, dword ptr [gBack_screen]
            add esp, 0x10
            test eax, eax
            mov dword ptr [gExit_code], 0x2bc
            je L0051ce90
            mov ecx, dword ptr [eax + 8]
            test ecx, ecx
            je L0051ce90
    L0051ce86:
            mov ecx, 1
            call PossibleUnlock
    L0051ce90:
            mov eax, dword ptr [gBr_initialized]
            test eax, eax
            je L0051ce9e
            call DRBrEnd
    L0051ce9e:
            call PDShutdownSystem
            xor eax, eax
            add esp, 0x10
            ret
    L0051cea9:
            mov eax, dword ptr [gDirectInputDevice]
            push eax
            mov ecx, dword ptr [eax]
            call dword ptr [ecx + 0x1c]
            test eax, eax
            jge L0051cec6
            push eax
            push offset gRegionDescriminator+0xd14
            call dr_dprintf
            add esp, 8
    L0051cec6:
            call JoystickDInputBegin
            test eax, eax
            je L0051ced9
            call CollectJoystickButtonInfos
            call LoadJoystickPreferences
    L0051ced9:
            mov dword ptr [gJoystick_deadzone], 0x1f40
            mov eax, 1
            add esp, 0x10
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00459690
tU32 C2_NAKED C2_HOOK_FASTCALL PDGetJoystickButtonStates(void) {
#ifndef CARPOCALYPSE2_MATCHING
    IDirectInputDevice2 *device;
    HRESULT res;

    if (gJoystick_index == -1) {
        return 0;
    }
    device = gDirectInputJoystickDevices[gJoystick_index];
    if (device == NULL) {
        return 0;
    }
    res = IDirectInputDevice2_Poll(device);
    if (FAILED(res)) {
        switch (res) {
        case DIERR_INPUTLOST:
        case DIERR_NOTACQUIRED:
            if (AcquireDInputJoystickDevice(gJoystick_index) == 0) {
                dr_dprintf("couldn't reacquire joystick %d ", gJoystick_index);
                return 0;
            }
            break;
        default:
            dr_dprintf("Can't poll, unknown error\n");
            return 0;
        }
    } else {
        DIJOYSTATE device_state;
        tU32 button_mask;
        int i;

        C2_HOOK_BUG_ON(sizeof(device_state) != 0x50);
        res = IDirectInputDevice2_GetDeviceState(device, sizeof(device_state), &device_state);
        if (FAILED(res)) {
            if (res == DIERR_INPUTLOST) {
                AcquireDInputJoystickDevice(gJoystick_index);
            }
            return 0;
        }
        button_mask = 0;
        for (i = 0; i < 32; i++) {
            if (device_state.rgbButtons[i] & 0x80) {
                button_mask |= (1 << i);
            }
        }
        return button_mask;
    }
    return 0;
#else
    __asm {
            mov eax, dword ptr [gJoystick_index]
            sub esp, 0x50
            cmp eax, -1
            push esi
            jne L004596a5
            xor eax, eax
            pop esi
            add esp, 0x50
            ret
    L004596a5:
            mov eax, dword ptr [eax*4 + gDirectInputJoystickDevices]
            test eax, eax
            jne L004596b5
            pop esi
            add esp, 0x50
            ret
    L004596b5:
            mov ecx, dword ptr [eax]
            push eax
            call dword ptr [ecx + 0x64]
            test eax, eax
            jge L0045986c
            cmp eax, 0x8007001e
            jne L0045978d
            mov eax, dword ptr [gDirectInputJoystickHandle]
            mov esi, dword ptr [gJoystick_index]
            test eax, eax
            je L00459768
            mov eax, dword ptr [esi*4 + gDirectInputJoystickDevices]
            test eax, eax
            je L00459768
            mov edx, dword ptr [eax]
            push eax
            call dword ptr [edx + 0x1c]
            test eax, eax
            jne L0045970b
            push esi
            push offset gUNK_00595f9c+0x90
            call dr_dprintf
            add esp, 8
            mov eax, 1
            jmp L0045976b
    L0045970b:
            cmp eax, 1
            jne L00459725
            push esi
            push offset gUNK_00595f9c+0x70
            call dr_dprintf
            add esp, 8
            mov eax, 1
            jmp L0045976b
    L00459725:
            cmp eax, 0x80070057
            jne L0045973e
            push esi
            push offset gUNK_00595f9c+0x50
            call dr_dprintf
            add esp, 8
            xor eax, eax
            jmp L0045976b
    L0045973e:
            cmp eax, 0x80070005
            push esi
            jne L00459757
            push offset gUNK_00595f9c+0x2c
            call dr_dprintf
            add esp, 8
            xor eax, eax
            jmp L0045976b
    L00459757:
            push offset gUNK_00595f9c+0x4
            call dr_dprintf
            add esp, 8
            xor eax, eax
            jmp L0045976b
    L00459768:
            or eax, 0xffffffff
    L0045976b:
            test eax, eax
            jne L00459944
            mov eax, dword ptr [gJoystick_index]
            push eax
            push offset gUNK_00595f9c+0x170
            call dr_dprintf
            add esp, 8
            xor eax, eax
            pop esi
            add esp, 0x50
            ret
    L0045978d:
            cmp eax, 0x8007000c
            jne L00459858
            mov eax, dword ptr [gDirectInputJoystickHandle]
            mov esi, dword ptr [gJoystick_index]
            test eax, eax
            je L00459832
            mov eax, dword ptr [esi*4 + gDirectInputJoystickDevices]
            test eax, eax
            je L00459832
            mov ecx, dword ptr [eax]
            push eax
            call dword ptr [ecx + 0x1c]
            test eax, eax
            jne L004597d5
            push esi
            push offset gUNK_00595f9c+0x90
            call dr_dprintf
            add esp, 8
            mov eax, 1
            jmp L00459835
    L004597d5:
            cmp eax, 1
            jne L004597ef
            push esi
            push offset gUNK_00595f9c+0x70
            call dr_dprintf
            add esp, 8
            mov eax, 1
            jmp L00459835
    L004597ef:
            cmp eax, 0x80070057
            jne L00459808
            push esi
            push offset gUNK_00595f9c+0x50
            call dr_dprintf
            add esp, 8
            xor eax, eax
            jmp L00459835
    L00459808:
            cmp eax, 0x80070005
            push esi
            jne L00459821
            push offset gUNK_00595f9c+0x2c
            call dr_dprintf
            add esp, 8
            xor eax, eax
            jmp L00459835
    L00459821:
            push offset gUNK_00595f9c+0x4
            call dr_dprintf
            add esp, 8
            xor eax, eax
            jmp L00459835
    L00459832:
            or eax, 0xffffffff
    L00459835:
            test eax, eax
            jne L00459944
            mov edx, dword ptr [gJoystick_index]
            push edx
            push offset gUNK_00595f9c+0x170
            call dr_dprintf
            add esp, 8
            xor eax, eax
            pop esi
            add esp, 0x50
            ret
    L00459858:
            push offset gUNK_00595f9c+0x154
            call dr_dprintf
            add esp, 4
            xor eax, eax
            pop esi
            add esp, 0x50
            ret
    L0045986c:
            mov eax, dword ptr [gJoystick_index]
            lea edx, [esp + 4]
            push edx
            push 0x50
            mov eax, dword ptr [eax*4 + gDirectInputJoystickDevices]
            push eax
            mov ecx, dword ptr [eax]
            call dword ptr [ecx + 0x24]
            mov esi, eax
            test esi, esi
            jge L0045994b
            push offset gUNK_00595f9c+0x138
            call dr_dprintf
            add esp, 4
            cmp esi, 0x8007001e
            jne L00459944
            mov eax, dword ptr [gDirectInputJoystickHandle]
            mov esi, dword ptr [gJoystick_index]
            test eax, eax
            je L00459944
            mov eax, dword ptr [esi*4 + gDirectInputJoystickDevices]
            test eax, eax
            je L00459944
            mov ecx, dword ptr [eax]
            push eax
            call dword ptr [ecx + 0x1c]
            test eax, eax
            jne L004598e5
            push esi
            push offset gUNK_00595f9c+0x90
            call dr_dprintf
            add esp, 8
            xor eax, eax
            pop esi
            add esp, 0x50
            ret
    L004598e5:
            cmp eax, 1
            jne L004598ff
            push esi
            push offset gUNK_00595f9c+0x70
            call dr_dprintf
            add esp, 8
            xor eax, eax
            pop esi
            add esp, 0x50
            ret
    L004598ff:
            cmp eax, 0x80070057
            jne L0045991b
            push esi
            push offset gUNK_00595f9c+0x50
            call dr_dprintf
            add esp, 8
            xor eax, eax
            pop esi
            add esp, 0x50
            ret
    L0045991b:
            cmp eax, 0x80070005
            push esi
            jne L00459937
            push offset gUNK_00595f9c+0x2c
            call dr_dprintf
            add esp, 8
            xor eax, eax
            pop esi
            add esp, 0x50
            ret
    L00459937:
            push offset gUNK_00595f9c+0x4
            call dr_dprintf
            add esp, 8
    L00459944:
            xor eax, eax
            pop esi
            add esp, 0x50
            ret
    L0045994b:
            xor eax, eax
            xor ecx, ecx
            mov dl, 0x80
    L00459951:
            test byte ptr [esp + ecx + 0x34], dl
            je L00459960
            mov esi, 1
            shl esi, cl
            or eax, esi
    L00459960:
            inc ecx
            cmp ecx, 0x20
            jl L00459951
            pop esi
            add esp, 0x50
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0045b360
void C2_NAKED C2_HOOK_FASTCALL PDSetKeysFromJoystick(int *keys) {
#ifndef CARPOCALYPSE2_MATCHING
    if (gJoystick_index != 1) {
        tU32 button_mask;

        button_mask = PDGetJoystickButtonStates();
        if (PDGetCurrentJoystickData() != NULL) {
            int i;

            for (i = 0; i < 32; i++) {
                keys[107 + i] = !!(button_mask & (i << i));
            }
        }
    }
#else
    __asm {
            mov eax, dword ptr [gJoystick_index]
            push esi
            push edi
            cmp eax, -1
            mov edi, ecx
            je L0045b3a1
            call PDGetJoystickButtonStates
            mov esi, eax
            call PDGetCurrentJoystickData
            test eax, eax
            je L0045b3a1
            xor ecx, ecx
            lea eax, [edi + 0x1ac]
    L0045b386:
            mov edx, 1
            shl edx, cl
            and edx, esi
            neg edx
            sbb edx, edx
            add eax, 4
            neg edx
            mov dword ptr [eax - 4], edx
            inc ecx
            cmp ecx, 0x20
            jl L0045b386
    L0045b3a1:
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
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0051cef0
void C2_HOOK_FASTCALL PDSetKeyArray(int* pKeys, int pMark) {
    BYTE diKeys[256];
    HRESULT res;
    int i;

    gKeys_pressed = 0;
    SDL3ServiceMessages();
    if (gWindowActiveState != 2) {
        *pKeys = 0;
        return;
    }
    if (gTime_app_activated != 0) {
        if (PDGetTotalTime() < gTime_app_activated + 1000) {
            *pKeys = 0;
            return;
        }
    }
    gTime_app_activated = 0;
    res = IDirectInputDevice_GetDeviceState(gDirectInputDevice, sizeof(diKeys), diKeys);
    if (res != DI_OK) {
        dr_dprintf("ACTIVE but couldn't get keyboard device state on 1st attempt");
        dr_dprintf("Keyboard input lost; reacquiring...");
        if (gDirectInputDevice != NULL) {
            IDirectInputDevice_Acquire(gDirectInputDevice);
        }
        res = IDirectInputDevice_GetDeviceState(gDirectInputDevice, sizeof(diKeys), diKeys);
        if (res != DI_OK) {
            dr_dprintf("Couldn't get keyboard device state on 2nd attempt");
            dr_dprintf("Zeroing pKeys");
            *pKeys = 0;
            return;
        }
        dr_dprintf("Keyboard reacquired OK.");
    }
    for (i = 0; i < 151; i++) {
        int scan_code;

        scan_code = gScan_code[12 + i];
        if (scan_code == 0) {
            continue;
        }
        if (scan_code == 0xff) {
            switch (i) {
            case 0:
                if ((diKeys[gScan_code[20]] & 0x80) || (diKeys[gScan_code[17]] & 0x80)) {
                    pKeys[0] = pMark;
                } else if (pKeys[0] == pMark) {
                    pKeys[0] = 0;
                }
                break;
            case 1:
                if ((diKeys[gScan_code[21]] & 0x80) || (diKeys[gScan_code[18]] & 0x80)) {
                    pKeys[1] = pMark;
                } else if (pKeys[1] == pMark) {
                    pKeys[1] = 0;
                }
                break;
            case 2:
            case 3:
                if ((diKeys[gScan_code[22]] & 0x80) || (diKeys[gScan_code[19]] & 0x80)) {
                    pKeys[2] = pMark;
                    pKeys[3] = pMark;
                } else if (pKeys[2] == pMark || pKeys[3] == pMark) {
                    pKeys[2] = 0;
                    pKeys[3] = 0;
                }
                break;
            }
        } else {
            if (diKeys[scan_code] & 0x80) {
                pKeys[i] = pMark;
                gKeys_pressed = gKeys_pressed << 8 | (i + 1);
            } else if (pKeys[i] == pMark) {
                pKeys[i] = 0;
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x0051d1d0
void C2_HOOK_FASTCALL PDReadJoysticks(void) {

    if (gJoystick_index != -1) {
        gJoy1_valid = 1;
        return;
    }
    if (gJoy1_valid) {
        if (joyGetPosEx(0, &gJoy1_info) != JOYERR_NOERROR) {
            gJoy1_valid = 0;
        }
    }
    if (gJoy2_valid) {
        if (joyGetPosEx(1, &gJoy2_info) != JOYERR_NOERROR) {
            gJoy2_valid = 0;
        }
    }
}

// FUNCTION: CARMA2_HW 0x00459050
int C2_NAKED C2_HOOK_FASTCALL GetDirectInputJoy1X(void) {
#ifndef CARPOCALYPSE2_MATCHING
    HRESULT res;
    DIJOYSTATE joy_state;
    int x;

    if (gJoystick_index == -1) {
        return -1;
    }
    if (gDirectInputJoystickDevices[gJoystick_index] == NULL) {
        return -1;
    }
    if (!gINT_00595f98) {
        return -1;
    }
    res = IDirectInputDevice2_Poll(gDirectInputJoystickDevices[gJoystick_index]);
    if (res != DI_OK) {
        if (res == DIERR_INPUTLOST) {
            if (AcquireDInputJoystickDevice(gJoystick_index) == 0) {
                dr_dprintf("couldn't reacquire joystick %d\n", gJoystick_index);
                return -1;
            }
        } else if (res == DIERR_NOTACQUIRED) {
            if (AcquireDInputJoystickDevice(gJoystick_index) == 0) {
                dr_dprintf("couldn't reacquire joystick %d\n", gJoystick_index);
                return -1;
            }
        } else {
            dr_dprintf("Can't poll, unknown error\n");
            return -1;
        }
        return -1;
    }

    res = IDirectInputDevice_GetDeviceState(gDirectInputJoystickDevices[gJoystick_index], sizeof(joy_state), &joy_state);
    if (res != DI_OK) {
        dr_dprintf("attempting to reacquire\n");
        if (res == DIERR_INPUTLOST) {
            AcquireDInputJoystickDevice(gJoystick_index);
        }
        return -1;
    }
    x = (int)((float)(joy_state.lX - 0x7fff) * gJoystick_x_steering + (float)0x7fff);
    if (x > 0xffff) {
        x = 0xffff;
    }
    if (x < 0) {
        x = 0;
    }
    return x;
#else
    __asm {
            mov eax, dword ptr [gJoystick_index]
            sub esp, 0x54
            cmp eax, -1
            push esi
            jne L00459065
            or eax, eax
            pop esi
            add esp, 0x54
            ret
    L00459065:
            mov eax, dword ptr [eax*4 + gDirectInputJoystickDevices]
            test eax, eax
            jne L00459078
            or eax, 0xffffffff
            pop esi
            add esp, 0x54
            ret
    L00459078:
            mov ecx, dword ptr [gINT_00595f98]
            test ecx, ecx
            jne L0045908a
            or eax, 0xffffffff
            pop esi
            add esp, 0x54
            ret
    L0045908a:
            mov ecx, dword ptr [eax]
            push eax
            call dword ptr [ecx + 0x64]
            test eax, eax
            je L00459244
            cmp eax, 0x8007001e
            jne L00459163
            mov eax, dword ptr [gDirectInputJoystickHandle]
            mov esi, dword ptr [gJoystick_index]
            test eax, eax
            je L0045913d
            mov eax, dword ptr [esi*4 + gDirectInputJoystickDevices]
            test eax, eax
            je L0045913d
            mov edx, dword ptr [eax]
            push eax
            call dword ptr [edx + 0x1c]
            test eax, eax
            jne L004590e0
            push esi
            push offset gUNK_00595f9c+0x90
            call dr_dprintf
            add esp, 8
            mov eax, 1
            jmp L00459140
    L004590e0:
            cmp eax, 1
            jne L004590fa
            push esi
            push offset gUNK_00595f9c+0x70
            call dr_dprintf
            add esp, 8
            mov eax, 1
            jmp L00459140
    L004590fa:
            cmp eax, 0x80070057
            jne L00459113
            push esi
            push offset gUNK_00595f9c+0x50
            call dr_dprintf
            add esp, 8
            xor eax, eax
            jmp L00459140
    L00459113:
            cmp eax, 0x80070005
            push esi
            jne L0045912c
            push offset gUNK_00595f9c+0x2c
            call dr_dprintf
            add esp, 8
            xor eax, eax
            jmp L00459140
    L0045912c:
            push offset gUNK_00595f9c+0x4
            call dr_dprintf
            add esp, 8
            xor eax, eax
            jmp L00459140
    L0045913d:
            or eax, 0xffffffff
    L00459140:
            test eax, eax
            jne L00459324
            mov eax, dword ptr [gJoystick_index]
            push eax
            push offset gUNK_00595f9c+0x170
            call dr_dprintf
            add esp, 8
            or eax, 0xffffffff
            pop esi
            add esp, 0x54
            ret
    L00459163:
            cmp eax, 0x8007000c
            jne L0045922f
            mov eax, dword ptr [gDirectInputJoystickHandle]
            mov esi, dword ptr [gJoystick_index]
            test eax, eax
            je L00459208
            mov eax, dword ptr [esi*4 + gDirectInputJoystickDevices]
            test eax, eax
            je L00459208
            mov ecx, dword ptr [eax]
            push eax
            call dword ptr [ecx + 0x1c]
            test eax, eax
            jne L004591ab
            push esi
            push offset gUNK_00595f9c+0x90
            call dr_dprintf
            add esp, 8
            mov eax, 1
            jmp L0045920b
    L004591ab:
            cmp eax, 1
            jne L004591c5
            push esi
            push offset gUNK_00595f9c+0x70
            call dr_dprintf
            add esp, 8
            mov eax, 1
            jmp L0045920b
    L004591c5:
            cmp eax, 0x80070057
            jne L004591de
            push esi
            push offset gUNK_00595f9c+0x50
            call dr_dprintf
            add esp, 8
            xor eax, eax
            jmp L0045920b
    L004591de:
            cmp eax, 0x80070005
            push esi
            jne L004591f7
            push offset gUNK_00595f9c+0x2c
            call dr_dprintf
            add esp, 8
            xor eax, eax
            jmp L0045920b
    L004591f7:
            push offset gUNK_00595f9c+0x4
            call dr_dprintf
            add esp, 8
            xor eax, eax
            jmp L0045920b
    L00459208:
            or eax, 0xffffffff
    L0045920b:
            test eax, eax
            jne L00459324
            mov edx, dword ptr [gJoystick_index]
            push edx
            push offset gUNK_00595f9c+0x170
            call dr_dprintf
            add esp, 8
            or eax, 0xffffffff
            pop esi
            add esp, 0x54
            ret
    L0045922f:
            push offset gUNK_00595f9c+0x154
            call dr_dprintf
            add esp, 4
            or eax, 0xffffffff
            pop esi
            add esp, 0x54
            ret
    L00459244:
            mov eax, dword ptr [gJoystick_index]
            lea edx, [esp + 8]
            push edx
            push 0x50
            mov eax, dword ptr [eax*4 + gDirectInputJoystickDevices]
            push eax
            mov ecx, dword ptr [eax]
            call dword ptr [ecx + 0x24]
            mov esi, eax
            test esi, esi
            je L0045932c
            push offset gUNK_00595f9c+0x138
            call dr_dprintf
            add esp, 4
            cmp esi, 0x8007001e
            jne L00459324
            mov eax, dword ptr [gDirectInputJoystickHandle]
            mov esi, dword ptr [gJoystick_index]
            test eax, eax
            je L00459324
            mov eax, dword ptr [esi*4 + gDirectInputJoystickDevices]
            test eax, eax
            je L00459324
            mov ecx, dword ptr [eax]
            push eax
            call dword ptr [ecx + 0x1c]
            test eax, eax
            jne L004592c2
            push esi
            push offset gUNK_00595f9c+0x90
            call dr_dprintf
            add esp, 8
            or eax, 0xffffffff
            pop esi
            add esp, 0x54
            ret
    L004592c2:
            cmp eax, 1
            jne L004592dd
            push esi
            push offset gUNK_00595f9c+0x70
            call dr_dprintf
            add esp, 8
            or eax, 0xffffffff
            pop esi
            add esp, 0x54
            ret
    L004592dd:
            cmp eax, 0x80070057
            jne L004592fa
            push esi
            push offset gUNK_00595f9c+0x50
            call dr_dprintf
            add esp, 8
            or eax, 0xffffffff
            pop esi
            add esp, 0x54
            ret
    L004592fa:
            cmp eax, 0x80070005
            push esi
            jne L00459317
            push offset gUNK_00595f9c+0x2c
            call dr_dprintf
            add esp, 8
            or eax, 0xffffffff
            pop esi
            add esp, 0x54
            ret
    L00459317:
            push offset gUNK_00595f9c+0x4
            call dr_dprintf
            add esp, 8
    L00459324:
            or eax, 0xffffffff
            pop esi
            add esp, 0x54
            ret
    L0045932c:
            mov edx, dword ptr [esp + 8]
            add edx, 0xffff8001
            mov dword ptr [esp + 4], edx
            fild dword ptr [esp + 4]
            fmul dword ptr [gJoystick_x_steering]
            fsub dword ptr [gJoystick_effect_guids+0x34]
            call _ftol
            cmp eax, 0xffff
            jle L00459360
            mov eax, 0xffff
            pop esi
            add esp, 0x54
            ret
    L00459360:
            test eax, eax
            jge L00459366
            xor eax, eax
    L00459366:
            pop esi
            add esp, 0x54
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00459370
int C2_NAKED C2_HOOK_FASTCALL GetDirectInputJoy1Y(void) {
#ifndef CARPOCALYPSE2_MATCHING
    HRESULT res;
    DIJOYSTATE joy_state;
    int y;

    if (gJoystick_index == -1) {
        return -1;
    }
    if (gDirectInputJoystickDevices[gJoystick_index] == NULL) {
        return -1;
    }
    if (!gUNK_00595f9c) {
        return -1;
    }
    res = IDirectInputDevice2_Poll(gDirectInputJoystickDevices[gJoystick_index]);
    if (res != DI_OK) {
        if (res == DIERR_INPUTLOST) {
            if (AcquireDInputJoystickDevice(gJoystick_index) == 0) {
                dr_dprintf("couldn't reacquire joystick %d\n", gJoystick_index);
                return -1;
            }
        } else if (res == DIERR_NOTACQUIRED) {
            if (AcquireDInputJoystickDevice(gJoystick_index) == 0) {
                dr_dprintf("couldn't reacquire joystick %d\n", gJoystick_index);
                return -1;
            }
        } else {
            dr_dprintf("Can't poll, unknown error\n");
            return -1;
        }
        return -1;
    }

    res = IDirectInputDevice_GetDeviceState(gDirectInputJoystickDevices[gJoystick_index], sizeof(joy_state), &joy_state);
    if (res != DI_OK) {
        dr_dprintf("attempting to reacquire\n");
        if (res == DIERR_INPUTLOST) {
            AcquireDInputJoystickDevice(gJoystick_index);
        }
        return -1;
    }
    y = (int)((float)(joy_state.lY - 0x7fff) * gJoystick_y_throttle + (float)0x7fff);
    if (y > 0xffff) {
        y = 0xffff;
    }
    if (y < 0) {
        y = 0;
    }
    return y;
#else
    __asm {
            mov eax, dword ptr [gJoystick_index]
            sub esp, 0x54
            cmp eax, -1
            push esi
            jne L00459385
            or eax, eax
            pop esi
            add esp, 0x54
            ret
    L00459385:
            mov eax, dword ptr [eax*4 + gDirectInputJoystickDevices]
            test eax, eax
            jne L00459398
            or eax, 0xffffffff
            pop esi
            add esp, 0x54
            ret
    L00459398:
            mov ecx, dword ptr [gUNK_00595f9c]
            test ecx, ecx
            jne L004593aa
            or eax, 0xffffffff
            pop esi
            add esp, 0x54
            ret
    L004593aa:
            mov ecx, dword ptr [eax]
            push eax
            call dword ptr [ecx + 0x64]
            test eax, eax
            je L00459564
            cmp eax, 0x8007001e
            jne L00459483
            mov eax, dword ptr [gDirectInputJoystickHandle]
            mov esi, dword ptr [gJoystick_index]
            test eax, eax
            je L0045945d
            mov eax, dword ptr [esi*4 + gDirectInputJoystickDevices]
            test eax, eax
            je L0045945d
            mov edx, dword ptr [eax]
            push eax
            call dword ptr [edx + 0x1c]
            test eax, eax
            jne L00459400
            push esi
            push offset gUNK_00595f9c+0x90
            call dr_dprintf
            add esp, 8
            mov eax, 1
            jmp L00459460
    L00459400:
            cmp eax, 1
            jne L0045941a
            push esi
            push offset gUNK_00595f9c+0x70
            call dr_dprintf
            add esp, 8
            mov eax, 1
            jmp L00459460
    L0045941a:
            cmp eax, 0x80070057
            jne L00459433
            push esi
            push offset gUNK_00595f9c+0x50
            call dr_dprintf
            add esp, 8
            xor eax, eax
            jmp L00459460
    L00459433:
            cmp eax, 0x80070005
            push esi
            jne L0045944c
            push offset gUNK_00595f9c+0x2c
            call dr_dprintf
            add esp, 8
            xor eax, eax
            jmp L00459460
    L0045944c:
            push offset gUNK_00595f9c+0x4
            call dr_dprintf
            add esp, 8
            xor eax, eax
            jmp L00459460
    L0045945d:
            or eax, 0xffffffff
    L00459460:
            test eax, eax
            jne L00459644
            mov eax, dword ptr [gJoystick_index]
            push eax
            push offset gUNK_00595f9c+0x170
            call dr_dprintf
            add esp, 8
            or eax, 0xffffffff
            pop esi
            add esp, 0x54
            ret
    L00459483:
            cmp eax, 0x8007000c
            jne L0045954f
            mov eax, dword ptr [gDirectInputJoystickHandle]
            mov esi, dword ptr [gJoystick_index]
            test eax, eax
            je L00459528
            mov eax, dword ptr [esi*4 + gDirectInputJoystickDevices]
            test eax, eax
            je L00459528
            mov ecx, dword ptr [eax]
            push eax
            call dword ptr [ecx + 0x1c]
            test eax, eax
            jne L004594cb
            push esi
            push offset gUNK_00595f9c+0x90
            call dr_dprintf
            add esp, 8
            mov eax, 1
            jmp L0045952b
    L004594cb:
            cmp eax, 1
            jne L004594e5
            push esi
            push offset gUNK_00595f9c+0x70
            call dr_dprintf
            add esp, 8
            mov eax, 1
            jmp L0045952b
    L004594e5:
            cmp eax, 0x80070057
            jne L004594fe
            push esi
            push offset gUNK_00595f9c+0x50
            call dr_dprintf
            add esp, 8
            xor eax, eax
            jmp L0045952b
    L004594fe:
            cmp eax, 0x80070005
            push esi
            jne L00459517
            push offset gUNK_00595f9c+0x2c
            call dr_dprintf
            add esp, 8
            xor eax, eax
            jmp L0045952b
    L00459517:
            push offset gUNK_00595f9c+0x4
            call dr_dprintf
            add esp, 8
            xor eax, eax
            jmp L0045952b
    L00459528:
            or eax, 0xffffffff
    L0045952b:
            test eax, eax
            jne L00459644
            mov edx, dword ptr [gJoystick_index]
            push edx
            push offset gUNK_00595f9c+0x170
            call dr_dprintf
            add esp, 8
            or eax, 0xffffffff
            pop esi
            add esp, 0x54
            ret
    L0045954f:
            push offset gUNK_00595f9c+0x154
            call dr_dprintf
            add esp, 4
            or eax, 0xffffffff
            pop esi
            add esp, 0x54
            ret
    L00459564:
            mov eax, dword ptr [gJoystick_index]
            lea edx, [esp + 8]
            push edx
            push 0x50
            mov eax, dword ptr [eax*4 + gDirectInputJoystickDevices]
            push eax
            mov ecx, dword ptr [eax]
            call dword ptr [ecx + 0x24]
            mov esi, eax
            test esi, esi
            je L0045964c
            push offset gUNK_00595f9c+0x138
            call dr_dprintf
            add esp, 4
            cmp esi, 0x8007001e
            jne L00459644
            mov eax, dword ptr [gDirectInputJoystickHandle]
            mov esi, dword ptr [gJoystick_index]
            test eax, eax
            je L00459644
            mov eax, dword ptr [esi*4 + gDirectInputJoystickDevices]
            test eax, eax
            je L00459644
            mov ecx, dword ptr [eax]
            push eax
            call dword ptr [ecx + 0x1c]
            test eax, eax
            jne L004595e2
            push esi
            push offset gUNK_00595f9c+0x90
            call dr_dprintf
            add esp, 8
            or eax, 0xffffffff
            pop esi
            add esp, 0x54
            ret
    L004595e2:
            cmp eax, 1
            jne L004595fd
            push esi
            push offset gUNK_00595f9c+0x70
            call dr_dprintf
            add esp, 8
            or eax, 0xffffffff
            pop esi
            add esp, 0x54
            ret
    L004595fd:
            cmp eax, 0x80070057
            jne L0045961a
            push esi
            push offset gUNK_00595f9c+0x50
            call dr_dprintf
            add esp, 8
            or eax, 0xffffffff
            pop esi
            add esp, 0x54
            ret
    L0045961a:
            cmp eax, 0x80070005
            push esi
            jne L00459637
            push offset gUNK_00595f9c+0x2c
            call dr_dprintf
            add esp, 8
            or eax, 0xffffffff
            pop esi
            add esp, 0x54
            ret
    L00459637:
            push offset gUNK_00595f9c+0x4
            call dr_dprintf
            add esp, 8
    L00459644:
            or eax, 0xffffffff
            pop esi
            add esp, 0x54
            ret
    L0045964c:
            mov edx, dword ptr [esp + 0xc]
            add edx, 0xffff8001
            mov dword ptr [esp + 4], edx
            fild dword ptr [esp + 4]
            fmul dword ptr [gJoystick_y_throttle]
            fsub dword ptr [gJoystick_effect_guids+0x34]
            call _ftol
            cmp eax, 0xffff
            jle L00459680
            mov eax, 0xffff
            pop esi
            add esp, 0x54
            ret
    L00459680:
            test eax, eax
            jge L00459686
            xor eax, eax
    L00459686:
            pop esi
            add esp, 0x54
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0051d2c0
tU32 C2_HOOK_FASTCALL PDGetJoy1Button1(void) {
    if (gJoy1_valid) {
        return gJoy1_info.dwButtons & (1 << 0);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0051d2e0
tU32 C2_HOOK_FASTCALL PDGetJoy1Button2(void) {
    if (gJoy1_valid) {
        return gJoy1_info.dwButtons & (1 << 1);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0051d300
tU32 C2_HOOK_FASTCALL PDGetJoy1Button3(void) {
    if (gJoy1_valid) {
        return gJoy1_info.dwButtons & (1 << 2);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0051d320
tU32 C2_HOOK_FASTCALL PDGetJoy1Button4(void) {
    if (gJoy1_valid) {
        return gJoy1_info.dwButtons & (1 << 3);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0051d230
int C2_HOOK_FASTCALL PDGetJoy1X(void) {
    if (gJoystick_index != -1) {
        return GetDirectInputJoy1X();
    }
    return -1;
}

// FUNCTION: CARMA2_HW 0x0051d250
int C2_HOOK_FASTCALL PDGetJoy1Y(void) {
    if (gJoystick_index != -1) {
        return GetDirectInputJoy1Y();
    }
    return -1;
}

// FUNCTION: CARMA2_HW 0x0051d340
tU32 C2_HOOK_FASTCALL PDGetJoy2Button1(void) {
    if (gJoy1_valid && gUINT_006ad094 >= 5  ) {
        return gJoy1_info.dwButtons & (1 << 4);
    }
    if (gJoy2_valid) {
        return gJoy2_info.dwButtons & (1 << 0);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0051d370
tU32 C2_HOOK_FASTCALL PDGetJoy2Button2(void) {
    if (gJoy1_valid && gUINT_006ad094 >= 6) {
        return gJoy1_info.dwButtons & (1 << 5);
    }
    if (gJoy2_valid) {
        return gJoy2_info.dwButtons & (1 << 1);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0051d3a0
tU32 C2_HOOK_FASTCALL PDGetJoy2Button3(void) {
    if (gJoy1_valid && gUINT_006ad094 >= 7) {
        return gJoy1_info.dwButtons & (1 << 6);
    }
    if (gJoy2_valid) {
        return gJoy2_info.dwButtons & (1 << 2);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0051d3d0
tU32 C2_HOOK_FASTCALL PDGetJoy2Button4(void) {
    if (gJoy1_valid && gUINT_006ad094 >= 8) {
        return gJoy1_info.dwButtons & (1 << 7);
    }
    if (gJoy2_valid) {
        return gJoy2_info.dwButtons & (1 << 3);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0051d270
int C2_HOOK_FASTCALL PDGetJoy2X(void) {
    if (gJoy1_valid && (gINT_006ad0b8 & 0x2)) {
        return gJoy1_info.dwRpos;
    }
    if (gJoy2_valid) {
        return gJoy2_info.dwXpos;
    }
    return -1;
}

// FUNCTION: CARMA2_HW 0x0051d2a0
int C2_HOOK_FASTCALL PDGetJoy2Y(void) {
    if (gJoy2_valid) {
        return gJoy2_info.dwYpos;
    }
    return -1;
}

// FUNCTION: CARMA2_HW 0x004575b0
void C2_HOOK_FASTCALL PDInitJoysticks(void) {

    if (gCountEnumeratedJoystickDinputDevices == 0) {
        JoystickDInputBegin();
    }

    ResetForceFeedback();
}

// FUNCTION: CARMA2_HW 0x0045a0c0
void C2_HOOK_FASTCALL FUN_0045a0c0(void) {
    int old_index;

    old_index = gJoystick_index;
    gJoystick_index = -1;
    gUNK_00595f8c = old_index;
    *(volatile int*)&gCountEnumeratedJoystickDinputDevices;
    gForceFeedbackAvailable = 0;
}

void C2_HOOK_FASTCALL PDPlayFFBEffectIndex(int index) {
    IDirectInputEffect* effect;

    effect = gDirectInputEffects[index];
    if (effect != NULL) {
        IDirectInputEffect_Start(effect, 1, DIES_NODOWNLOAD);
    }
    return;
}

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x00458430
tJoystick_force_effect* C2_HOOK_FASTCALL GetJoystickEffect(int pIndex) {
    if (pIndex < 0 || pIndex > gCount_joystick_effects) {
        return NULL;
    }
    return &gJoystick_effects[pIndex];
}

// FUNCTION: CARMA2_HW 0x00458150
int C2_NAKED C2_HOOK_FASTCALL FUN_00458150(int pEffect_index, int pStart_effect, int* pDirection, tU32* pDuration, tU32* pGain, char* pType_specific_params, int* pTrigger_button, int* pEffect_data) {
#ifndef CARPOCALYPSE2_MATCHING
    IDirectInputEffect* direct_input_effect;
    int value;
    tU32 params_size;
    tU32 flags;
    tU32 saved_flags;

    flags = 0;
    saved_flags = 0;
    if (pDirection != NULL) {
        if (*pDirection > 0x168) {
            *pDirection = 0x168;
        } else if (*pDirection < 0) {
            *pDirection = 0;
        }
        value = *pDirection * 100;
        *pDirection = value;
        if (gJoystick_effects[pEffect_index].field_0x60 != value) {
            flags = 0x40;
            gJoystick_effects[pEffect_index].field_0x60 = value;
            saved_flags = flags;
        }
    }
    if (pDuration != NULL) {
        if (*pDuration == 0xffffff) {
            *pDuration = 0xffffffff;
        } else {
            if (*pDuration > 0x5dc) {
                *pDuration = 0x5dc;
            }
            *pDuration *= 1000;
        }
        if (gJoystick_effects[pEffect_index].di_effect.dwDuration != *pDuration) {
            flags |= 1;
            gJoystick_effects[pEffect_index].di_effect.dwDuration = *pDuration;
            saved_flags = flags;
        }
    }
    if (pGain != NULL) {
        if (10000 < *pGain) {
            *pGain = 10000;
        }
        if (gJoystick_effects[pEffect_index].di_effect.dwGain != *pGain) {
            flags |= 4;
            gJoystick_effects[pEffect_index].di_effect.dwGain = *pGain;
            saved_flags = flags;
        }
    }
    if (pType_specific_params != NULL) {
        params_size = gJoystick_effects[pEffect_index].di_effect.cbTypeSpecificParams;
        flags = saved_flags;
        if (memcmp(&gJoystick_effects[pEffect_index].type_specific_params, pType_specific_params, params_size) != 0) {
            memcpy(&gJoystick_effects[pEffect_index].type_specific_params, pType_specific_params, params_size);
            flags = saved_flags | 0x100;
        }
    }
    if (pTrigger_button != NULL && (value = *pTrigger_button, value < 0x20)) {
        if (value == -1) {
            value = -1;
        } else {
            value += 0x30;
        }
        flags |= 8;
        gJoystick_effects[pEffect_index].di_effect.dwTriggerButton = value;
    }
    if (pEffect_data != NULL) {
        if ((((*(int*)&gJoystick_effects[pEffect_index].field_0x4c[4] != *pEffect_data) ||
                (*(int*)&gJoystick_effects[pEffect_index].field_0x4c[8] != pEffect_data[1])) ||
                (*(int*)&gJoystick_effects[pEffect_index].field_0x4c[12] != pEffect_data[2])) ||
                (*(int*)&gJoystick_effects[pEffect_index].field_0x4c[16] != pEffect_data[3])) {
            *(int*)&gJoystick_effects[pEffect_index].field_0x4c[4] = *pEffect_data;
            *(int*)&gJoystick_effects[pEffect_index].field_0x4c[8] = pEffect_data[1];
            *(int*)&gJoystick_effects[pEffect_index].field_0x4c[12] = pEffect_data[2];
            *(int*)&gJoystick_effects[pEffect_index].field_0x4c[16] = pEffect_data[3];
            flags |= 0x80;
        }
    }
    if (flags == 0) {
        if (pStart_effect == 1) {
            StartDinputEffect(pEffect_index);
        }
        return 0;
    }
    direct_input_effect = gDirectInputEffects[pEffect_index];
    if (direct_input_effect != NULL) {
        IDirectInputEffect_SetParameters(direct_input_effect, &gJoystick_effects[pEffect_index], flags);
    }
    if (pStart_effect != 0) {
        StartDinputEffect(pEffect_index);
    }
    return 1;
#else
    __asm {
    L00458150:
            sub esp, 8
            push ebx
            mov ebx, ecx
            mov ecx, dword ptr [esp + 0x10]
            push ebp
            push esi
            xor esi, esi
            test ecx, ecx
            push edi
            mov dword ptr [esp + 0x14], edx
            mov dword ptr [esp + 0x10], esi
            je L004581b6
            mov eax, dword ptr [ecx]
            cmp eax, 0x168
            jle L0045817c
            mov dword ptr [ecx], 0x168
            jmp L00458186
    L0045817c:
            test eax, eax
            jge L00458186
            mov dword ptr [ecx], 0
    L00458186:
            mov eax, dword ptr [ecx]
            lea eax, [eax + eax*4]
            lea eax, [eax + eax*4]
            shl eax, 2
            mov dword ptr [ecx], eax
            mov ecx, ebx
            shl ecx, 3
            sub ecx, ebx
            lea ecx, [ebx + ecx*4]
            cmp dword ptr [ecx*8 + gJoystick_effects+0x60], eax
            je L004581b6
            mov esi, 0x40
            mov dword ptr [ecx*8 + gJoystick_effects+0x60], eax
            mov dword ptr [esp + 0x10], esi
    L004581b6:
            mov eax, dword ptr [esp + 0x20]
            test eax, eax
            je L00458211
            mov ecx, dword ptr [eax]
            cmp ecx, 0xffffff
            jne L004581d0
            mov dword ptr [eax], 0xffffffff
            jmp L004581ee
    L004581d0:
            cmp ecx, 0x5dc
            jbe L004581de
            mov dword ptr [eax], 0x5dc
    L004581de:
            mov ecx, dword ptr [eax]
            lea ecx, [ecx + ecx*4]
            lea ecx, [ecx + ecx*4]
            lea edx, [ecx + ecx*4]
            shl edx, 3
            mov dword ptr [eax], edx
    L004581ee:
            mov ecx, ebx
            mov eax, dword ptr [eax]
            shl ecx, 3
            sub ecx, ebx
            lea ecx, [ebx + ecx*4]
            cmp dword ptr [ecx*8 + gJoystick_effects+0x8], eax
            je L00458211
            or esi, 1
            mov dword ptr [ecx*8 + gJoystick_effects+0x8], eax
            mov dword ptr [esp + 0x10], esi
    L00458211:
            mov ecx, dword ptr [esp + 0x24]
            test ecx, ecx
            je L0045824a
            cmp dword ptr [ecx], 0x2710
            jbe L00458227
            mov dword ptr [ecx], 0x2710
    L00458227:
            mov edx, ebx
            mov ecx, dword ptr [ecx]
            shl edx, 3
            sub edx, ebx
            lea eax, [ebx + edx*4]
            cmp dword ptr [eax*8 + gJoystick_effects+0x10], ecx
            je L0045824a
            or esi, 4
            mov dword ptr [eax*8 + gJoystick_effects+0x10], ecx
            mov dword ptr [esp + 0x10], esi
    L0045824a:
            mov edi, dword ptr [esp + 0x28]
            test edi, edi
            je L00458299
            mov eax, ebx
            shl eax, 3
            sub eax, ebx
            lea eax, [ebx + eax*4]
            shl eax, 3
            xor ebp, ebp
            mov edx, dword ptr [eax + gJoystick_effects+0x2c]
            lea eax, [eax + gJoystick_effects+0x34]
            mov ecx, edx
            mov esi, eax
            repe cmpsb
            je L00458295
            mov esi, dword ptr [esp + 0x28]
            mov ecx, edx
            mov edi, eax
            shr ecx, 2
            rep movsd
            mov ecx, edx
            and ecx, 3
            rep movsb
            mov esi, dword ptr [esp + 0x10]
            or esi, 0x100
            jmp L00458299
    L00458295:
            mov esi, dword ptr [esp + 0x10]
    L00458299:
            mov eax, dword ptr [esp + 0x2c]
            test eax, eax
            je L004582c8
            mov eax, dword ptr [eax]
            cmp eax, 0x20
            jge L004582c8
            cmp eax, -1
            jne L004582b1
            or eax, eax
            jmp L004582b4
    L004582b1:
            add eax, 0x30
    L004582b4:
            mov ecx, ebx
            shl ecx, 3
            sub ecx, ebx
            or esi, 8
            lea edx, [ebx + ecx*4]
            mov dword ptr [edx*8 + gJoystick_effects+0x14], eax
    L004582c8:
            mov ecx, dword ptr [esp + 0x30]
            test ecx, ecx
            je L00458335
            mov eax, ebx
            mov edx, dword ptr [ecx]
            shl eax, 3
            sub eax, ebx
            lea eax, [ebx + eax*4]
            shl eax, 3
            cmp dword ptr [eax + gJoystick_effects+0x50], edx
            jne L0045830e
            mov edi, dword ptr [eax + gJoystick_effects+0x54]
            mov ebp, dword ptr [ecx + 4]
            cmp edi, ebp
            jne L0045830e
            mov edi, dword ptr [eax + gJoystick_effects+0x58]
            mov ebp, dword ptr [ecx + 8]
            cmp edi, ebp
            jne L0045830e
            mov edi, dword ptr [eax + gJoystick_effects+0x5c]
            mov ebp, dword ptr [ecx + 0xc]
            cmp edi, ebp
            je L00458335
    L0045830e:
            mov dword ptr [eax + gJoystick_effects+0x50], edx
            mov edx, dword ptr [ecx + 4]
            mov dword ptr [eax + gJoystick_effects+0x54], edx
            mov edx, dword ptr [ecx + 8]
            mov dword ptr [eax + gJoystick_effects+0x58], edx
            mov ecx, dword ptr [ecx + 0xc]
            mov dword ptr [eax + gJoystick_effects+0x5c], ecx
            or esi, 0x80
    L00458335:
            test esi, esi
            jne L00458353
            cmp dword ptr [esp + 0x14], 1
            jne L00458347
            mov ecx, ebx
            call StartDinputEffect
    L00458347:
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 8
            ret 0x18
    L00458353:
            mov eax, dword ptr [ebx*4 + gDirectInputEffects]
            test eax, eax
            je L00458377
            mov ecx, ebx
            mov edx, dword ptr [eax]
            shl ecx, 3
            sub ecx, ebx
            push esi
            lea ecx, [ebx + ecx*4]
            lea ecx, [ecx*8 + gJoystick_effects]
            push ecx
            push eax
            call dword ptr [edx + 0x18]
    L00458377:
            mov eax, dword ptr [esp + 0x14]
            test eax, eax
            je L00458386
            mov ecx, ebx
            call StartDinputEffect
    L00458386:
            pop edi
            pop esi
            pop ebp
            mov eax, 1
            pop ebx
            add esp, 8
            ret 0x18
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
            push ecx
            mov eax, dword ptr [gCount_joystick_effects]
            push ebx
            push ebp
            xor ebp, ebp
            push esi
            push edi
            test eax, eax
            mov dword ptr [esp + 0x10], edx
            jle L00458425
            mov edi, offset gJoystick_effects+0x68
    L004583b9:
            mov esi, ecx
            mov eax, edi
    L004583bd:
            mov bl, byte ptr [eax]
            mov dl, bl
            cmp bl, byte ptr [esi]
            jne L004583e1
            test dl, dl
            je L004583dd
            mov bl, byte ptr [eax + 1]
            mov dl, bl
            cmp bl, byte ptr [esi + 1]
            jne L004583e1
            add eax, 2
            add esi, 2
            test dl, dl
            jne L004583bd
    L004583dd:
            xor eax, eax
            jmp L004583e6
    L004583e1:
            sbb eax, eax
            sbb eax, -1
    L004583e6:
            test eax, eax
            je L00458402
            mov eax, dword ptr [gCount_joystick_effects]
            inc ebp
            add edi, 0xe8
            cmp ebp, eax
            jl L004583b9
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            pop ecx
            ret
    L00458402:
            lea eax, [esp + 0x10]
            push 0
            push eax
            push 0
            push 0
            push 0
            push 0
            xor edx, edx
            mov ecx, ebp
            call L00458150
            mov eax, 1
            pop edi
            pop esi
            pop ebp
            pop ebx
            pop ecx
            ret
    L00458425:
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
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00458460
void C2_HOOK_FASTCALL FUN_00458460(void) {
    tU32 duration;
    tJoystick_effect_description description;
    int effect_index;

    duration = 250000;
    description.type = eJoystick_effect_ConstantForce;
    description.type_specific_params.constant.field_0x0 = 5000;
    description.time_ms = 500000;
    description.field_0x38 = 1000;
    description.field_0x3c = 10000;
    description.field_0x44 = -1;
    description.field_0x48 = 0;
    description.field_0x4c = 0x5a;
    effect_index = CreateDinputEffect(gJoystick_index, &description, "Hello");
    if (effect_index >= 0) {
        StartDinputEffect(effect_index);
        Sleep(100);
        FUN_00458150(effect_index, 1, NULL, &duration, NULL, NULL, NULL, NULL);
        StartDinputEffect(effect_index);
        Sleep(500);
        ResetDInputJoystickFFB(gJoystick_index);
    }
}

// FUNCTION: CARMA2_HW 0x004580e0
int C2_HOOK_FASTCALL PDFindJoystickEffect(const char* effectName) {
    int i;

    for (i = 0; i < gCount_joystick_effects; i++) {

        if (strcmp(gJoystick_effects[i].name, effectName) == 0) {
            return i;
        }
    }
    return -1;
}
#pragma auto_inline(on)

// FUNCTION: CARMA2_HW 0x0045c720
void C2_HOOK_FASTCALL PDPlayFFBEffect(const char* effectName) {
    if (gForceFeedbackAvailable) {
        int index = PDFindJoystickEffect(effectName);
        if (index != -1) {
            StartDinputEffect(index);
        }
    }
}

// FUNCTION: CARMA2_HW 0x0045c740
void C2_HOOK_FASTCALL PDStopFFBEffect(const char* effectName) {
    if (gForceFeedbackAvailable) {
        int index = PDFindJoystickEffect(effectName);
        if (index != -1) {
            StopDinputEffect(index);
        }
    }
}

// FUNCTION: CARMA2_HW 0x0045c550
int C2_HOOK_FASTCALL PDIsJoystickDPadEnabled(void) {
    tButtonJoystickInfo* joystick_info;

    joystick_info = PDGetCurrentJoystickData();
    if (joystick_info != NULL) {
        return joystick_info->field_0xe0;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0045c460
int C2_HOOK_FASTCALL ToggleJoystickYInvert(void) {
    tButtonJoystickInfo* data;

    data = PDGetCurrentJoystickData();
    if (data != NULL) {
        data->field_0xd4 = (data->field_0xd4 == 0);
        return (gUNK_00595f9c = data->field_0xd4);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0045c490
void C2_HOOK_FASTCALL SetJoystickXAxis(float pValue) {

    tButtonJoystickInfo* joystick;

    joystick = PDGetCurrentJoystickData();
    if (joystick != NULL) {
        joystick->field_0xd8 = pValue;
        if (joystick->field_0xd8 > 2.0) {
            joystick->field_0xd8 = 2.0f;
        }
        if (joystick->field_0xd8 < 0.0) {
            joystick->field_0xd8 = 0.0f;
        }
        gJoystick_x_steering = joystick->field_0xd8;
    }
}

// FUNCTION: CARMA2_HW 0x0045c4f0
void C2_HOOK_FASTCALL SetJoystickYAxis(float pValue) {

    tButtonJoystickInfo* joystick;

    joystick = PDGetCurrentJoystickData();
    if (joystick != NULL) {
        joystick->field_0xdc = pValue;
        if (joystick->field_0xdc > 2.0) {
            joystick->field_0xdc = 2.0f;
        }
        if (joystick->field_0xdc < 0.0) {
            joystick->field_0xdc = 0.0f;
        }
        gJoystick_y_throttle = joystick->field_0xdc;
    }
}
