#ifndef GUARD_MAINMENU_H
#define GUARD_MAINMENU_H

#include "carpocalypse2_types.h"


#include "c2_hooks.h"

// QuitVerifyStart

// QuitVerifyDone
int C2_HOOK_FASTCALL QuitVerifyDone(int pCurrent_choice, int pCurrent_mode, int pGo_ahead, int pEscaped, int pTimed_out);

extern int C2_HOOK_FASTCALL DoVerifyQuit(int pReplace_background);

// DoMainScreen

// DoOptionsMenu

#define CARPOCALYPSE2_MAINMENU_H

#include "carpocalypse2_types.h"

#include "c2_hooks.h"


int C2_HOOK_FASTCALL DoMainScreen(void);

void C2_HOOK_FASTCALL DoOptionsMenu(void);

#endif // GUARD_MAINMENU_H
