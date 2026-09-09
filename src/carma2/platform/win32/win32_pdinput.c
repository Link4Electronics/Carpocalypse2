/*
 * PD (physics diagnostics) joystick helpers compiled as their own translation
 * unit. The original game kept these in a separate source module, which stops
 * the compiler from inlining them into every trivial caller (PDIsJoystickDPadEnabled
 * et al). See win32_dinput.c for the rest of the DirectInput code.
 */

#include "c2_hooks.h"

#include "joystick.h"
#include "platform.h"
#include "win32_dinput.h"
#include "win32_types.h"

// FUNCTION: CARMA2_HW 0x00459f80
tButtonJoystickInfo* C2_HOOK_FASTCALL PDGetCurrentJoystickData(void) {
    if (gJoystick_index != -1) {
        if (gDirectInputJoystickInfos[gJoystick_index].data != NULL) {
            return gDirectInputJoystickInfos[gJoystick_index].data;
        }
    }
    return NULL;
}

// FUNCTION: CARMA2_HW 0x00458040
void C2_HOOK_FASTCALL UnloadDinputFFBEffectAtIndex(int index) {
    IDirectInputEffect* effect = gDirectInputEffects[index];
    if (effect != NULL) {
        IDirectInputEffect_Unload(effect);
    }
}

// FUNCTION: CARMA2_HW 0x00458020
void C2_HOOK_FASTCALL DownloadDinputFFBEffectAtIndex(int index) {
    IDirectInputEffect* effect = gDirectInputEffects[index];
    if (effect != NULL) {
        IDirectInputEffect_Download(effect);
    }
}