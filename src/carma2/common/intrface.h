#ifndef GUARD_INTRFACE_H
#define GUARD_INTRFACE_H

#include "c2_hooks.h"
#include "carpocalypse2_types.h"

extern int gMouse_in_use;

extern void C2_HOOK_FASTCALL SetAlwaysTyping(void);

extern void C2_HOOK_FASTCALL ClearAlwaysTyping(void);

extern int C2_HOOK_FASTCALL ChoiceDisabled(int pChoice);

extern void C2_HOOK_FASTCALL ResetInterfaceTimeout__intrface(void);

extern void C2_HOOK_FASTCALL ChangeSelection(const tInterface_spec* pSpec, int* pOld_selection, int* pNew_selection, int pMode, int pSkip_disabled);

extern void C2_HOOK_FASTCALL RecopyAreas(const tInterface_spec* pSpec);

extern int C2_HOOK_FASTCALL DoInterfaceScreen(const tInterface_spec* pSpec, int pOptions, int pCurrent_choice);

extern int C2_HOOK_FASTCALL WhichItemIsSelectedIn(tFrontend_spec* pFrontend, int pGroup);

extern void C2_HOOK_FASTCALL SelectThisItemIn(tFrontend_spec* pFrontend, int pGroup, int pValue);

#define CARPOCALYPSE2_INTRFACE_H

#include "c2_hooks.h"

#include "carpocalypse2_types.h"


extern int gAlways_typing;

extern int gDisabled_count;

extern int gDisabled_choices[10];

extern tU32 gStart_time;

extern int gCurrent_mode;

extern int gCurrent_choice;

void C2_HOOK_FASTCALL OriginalResetInterfaceTimeout(void);

void C2_HOOK_FASTCALL LoadMenuSettings(tFrontend_spec* pFrontend);

void C2_HOOK_FASTCALL ChangeSelectionTo(int pNew_choice, int pNew_mode);

#endif // GUARD_INTRFACE_H
