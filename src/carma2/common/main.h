#ifndef GUARD_MAIN_H
#define GUARD_MAIN_H

#include "brender/br_types.h"


#include "c2_hooks.h"

extern C2_NORETURN void C2_HOOK_FASTCALL QuitGame(void);

// ServiceTheGame

extern void C2_HOOK_FASTCALL ServiceGame(void);

// ServiceGameInRace

void C2_HOOK_FASTCALL GameMain(int pArgc, const char** pArgv);

#define CARPOCALYPSE2_MAIN_H

#include "c2_hooks.h"

#include "brender/br_types.h"


void C2_HOOK_FASTCALL ServiceTheGame(int pRacing);

void C2_HOOK_FASTCALL ServiceGameInRace(void);

#endif // GUARD_MAIN_H
