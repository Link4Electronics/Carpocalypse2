#ifndef CARPOCALYPSE2_JOYSTICK_H
#define CARPOCALYPSE2_JOYSTICK_H

#include "c2_hooks.h"

#include "carpocalypse2_types.h"

#include <stdio.h>

extern int gINT_00595f84;
extern int gJoystick_index;
extern int gUNK_00595f8c;
extern int gUNK_00596310;
extern float gJoystick_x_steering;
extern float gJoystick_y_throttle;
extern int gUNK_00595f9c;

extern int gJoystick_preferences_backup_valid;
extern tButtonJoystickInfo gJoystick_preferences_backup;

extern int C2_HOOK_FASTCALL ReadUntilCharacter(FILE* pF, char* pBuffer, int pBuffer_size, char pLimit_char);

extern int C2_HOOK_FASTCALL FindJoystickPreferences(FILE* pF, const char* pJoystick_name);

extern int C2_HOOK_FASTCALL WriteJoystickPreferences(FILE* pF, const tButtonJoystickInfo* pInfo);

extern int C2_HOOK_FASTCALL ReadJoystickPreferences(FILE* pF, tButtonJoystickInfo* pInfo);

extern int C2_HOOK_FASTCALL CountJoystickPreferences(FILE* pF);

extern int C2_HOOK_FASTCALL LoadJoystickPreferences(void);

int C2_HOOK_FASTCALL SaveAllJoystickData(void);

extern void C2_HOOK_FASTCALL BackupJoystickPreferences(void);

extern void C2_HOOK_FASTCALL RestoreJoystickPreferences(void);

void C2_HOOK_FASTCALL FUN_0045c240(void);

void C2_HOOK_FASTCALL FUN_0045c420(void);
void C2_HOOK_FASTCALL FUN_0045c910(tCar_spec* pCar, const br_vector3* pPosition, br_vector3* pForce, float pMagnitude);

int C2_HOOK_FASTCALL ReadJoystickInfo(FILE* pFile, tButtonJoystickInfo* pInfo);

int C2_HOOK_FASTCALL WriteJoystickInfo(FILE* pFile, tButtonJoystickInfo* pInfo);

int C2_HOOK_FASTCALL FUN_0045b7e0(FILE* pFile, char* pString);

extern float gForce_feedback_upper_limit;

extern float Force_feedback_lower_limit;

extern float gOriginal_joystick_x;

extern float gOriginal_joystick_y;

extern int gOriginal_joystick_fbb;

extern int gOriginal_joystick_dpad;

extern int gOrig_joystick_index;

extern int gINT_00596308;

extern int gCurrent_ffb_effect_index;

extern tHeadup_text_buffer gJoystick_headup_buffer_0079d8a0;
extern tHeadup_text_buffer gHeadup_text_buffer_0079d880;

typedef struct tJoystick_headup_text {
    const char* text;
    int key;
} tJoystick_headup_text;

extern tJoystick_headup_text gJoystick_headup_texts[21];

void C2_HOOK_FASTCALL SetupFFBValues(void);

void C2_HOOK_FASTCALL PlayExclusiveFFBEffect(const char* pEffect_name, int pArg2);

void C2_HOOK_FASTCALL InitJoysticks(void);

float C2_HOOK_FASTCALL GetJoystickX(void);

float C2_HOOK_FASTCALL GetJoystickY(void);

int C2_HOOK_FASTCALL GetJoystickFBBGain(void);

void C2_HOOK_FASTCALL SetJoystickX(float pValue);

void C2_HOOK_FASTCALL SetJoystickY(float pValue);

void C2_HOOK_FASTCALL SetJoystickFFBGain(int pValue);

void C2_HOOK_FASTCALL SetJoystickDPadEnabled(int pEnabled);

int C2_HOOK_FASTCALL ToggleJoystickYInvert(void);

void C2_HOOK_FASTCALL SetAllFFBEffectsGain(int pGain);

int C2_HOOK_FASTCALL SetJoystickFFBForce(int pMagnitude);

int C2_HOOK_FASTCALL SetJoystickFFBSteeringForce(int pMagnitude);

int C2_HOOK_FASTCALL IncreaseJoystickFFBGain(void);

int C2_HOOK_FASTCALL PDEnableJoysticks(void);

void C2_HOOK_FASTCALL FUN_0045a070(void);

void C2_HOOK_FASTCALL Joystick_BackupSettings(void);

void C2_HOOK_FASTCALL EnableJoysticks(void);

void C2_HOOK_FASTCALL DisableJoysticks(void);


void C2_HOOK_FASTCALL AdjustHeadupTextBuffer(tHeadup_text_buffer* pText_buffer);
int C2_HOOK_FASTCALL InsertHeadupText(tHeadup_text_buffer* pText_buffer, tHeadup_text* pText, int pSortAlphabetically);

int C2_HOOK_FASTCALL FUN_CheckJoystickHeadupButtons(tButtonJoystickInfo** pJoystick_info);

void C2_HOOK_FASTCALL MungeJoystickHeadups(void);

int C2_HOOK_FASTCALL HasCurrentJoystick(void);

int C2_HOOK_FASTCALL FUN_0045a920(tHeadup_text_buffer* pText_buffer, tHeadup_text* pText, int pSorted);

#endif /* CARPOCALYPSE2_JOYSTICK_H */
