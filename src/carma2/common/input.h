#ifndef GUARD_INPUT_H
#define GUARD_INPUT_H

#include "carpocalypse2_types.h"


#include "c2_hooks.h"

extern int gKey_mapping[77];

// SetJoystickArrays

extern void C2_HOOK_FASTCALL PollKeys(void);

// CyclePollKeys

// ResetPollKeys

// CheckKeysForMouldiness

extern int C2_HOOK_FASTCALL EitherMouseButtonDown(void);

// AnyModifiersDown

// EdgeTriggeryKey

extern int C2_HOOK_FASTCALL PDKeyDown(int pKey_index);

// PDKeyDown3

extern int C2_HOOK_FASTCALL PDAnyKeyDown(void);

extern int C2_HOOK_FASTCALL AnyKeyDown(void);

// KevKeyService

extern int C2_HOOK_FASTCALL KeyIsDown(int pKey_index);

// KeyIsDownNoMouldiness

extern void C2_HOOK_FASTCALL WaitForNoKeys(void);

extern void C2_HOOK_FASTCALL GetMousePosition(int *pX, int *pY);

extern void C2_HOOK_FASTCALL InitRollingLetters(void);

extern void C2_HOOK_FASTCALL EndRollingLetters(void);

// AddRollingLetter

// ChangeCharTo

extern void C2_HOOK_FASTCALL RevertTyping(int pSlot_index, char* pRevert_str);

extern void C2_HOOK_FASTCALL EdgeTriggerModeOn(void);

extern void C2_HOOK_FASTCALL EdgeTriggerModeOff(void);

#define CARPOCALYPSE2_INPUT_H

#include "c2_hooks.h"

#include "carpocalypse2_types.h"


extern tJoy_array gJoy_array;

extern tKey_array gKey_array;

extern int gGo_ahead_keys[3];

extern int GEdge_trigger_mode;

extern tMouse_coord gCurrent_mouse_position;

extern tU32 gLast_roll;

extern int gCurrent_cursor;

extern tRolling_letter* gRolling_letters;

extern int gLetter_x_coords[15];

extern int gVisible_length;

extern int gLetter_y_coords[15];

extern int gThe_length;

extern tU32 gLast_poll_keys;

extern int gINT_006621e4;

extern int gINT_006621e8;

extern int gINT_006621ec;

extern int gINT_006621f0;

extern int gJoy1_x;

extern int gJoy1_y;

extern int gJoy2_x;

extern int gJoy2_y;

extern int gLast_key_down;

extern tU32 gLast_key_down_time;

extern int gModifiers_down;

int C2_HOOK_FASTCALL LoadJoystickPreferences(void);

int C2_HOOK_FASTCALL AnyModifiersDown(void);

tKey_down_result C2_HOOK_FASTCALL EdgeTriggeryKey(int pKey_index, int pReset);

tU32* C2_HOOK_FASTCALL KevKeyService(void);

void C2_HOOK_FASTCALL CyclePollKeys(void);

void C2_HOOK_FASTCALL ResetPollKeys(void);

void C2_HOOK_FASTCALL CheckKeysForMouldiness(void);

int C2_HOOK_FASTCALL KeyIsDownNoMouldiness(int pKey_index);

int C2_HOOK_FASTCALL AddRollingLetter(char pChar, int pX, int pY, tRolling_type rolling_type);

int C2_HOOK_FASTCALL ChangeCharTo(int pSlot_index, int pChar_index, char pNew_char);

void C2_HOOK_FASTCALL SetJoystickArrays(int* pKeys, int pMark);

#endif // GUARD_INPUT_H
