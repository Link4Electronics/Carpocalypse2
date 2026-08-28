#ifndef GUARD_CUTSCENE_H
#define GUARD_CUTSCENE_H

#include "c2_hooks.h"


// DoSCILogo

// DoStainlessLogo

// PlaySmackerFile

// DoOpeningAnimation

// DoEndRaceAnimation

// DoGameCompletedAnimation

// StartLoadingScreen

#define CARPOCALYPSE2_CUTSCENE_H

#include "c2_hooks.h"


void C2_HOOK_FASTCALL DoSCILogo(void);

void C2_HOOK_FASTCALL DoStainlessLogo(void);

void C2_HOOK_FASTCALL PlaySmackerFile(const char* pSmack_name);

void C2_HOOK_FASTCALL DoOpeningAnimation(void);

void C2_HOOK_FASTCALL DoNewGameAnimation(void);

void C2_HOOK_FASTCALL DoGoToRaceAnimation(void);

void C2_HOOK_FASTCALL DoEndRaceAnimation(void);

void C2_HOOK_FASTCALL DoGameOverAnimation(void);

void C2_HOOK_FASTCALL DoGameCompletedAnimation(void);

void C2_HOOK_FASTCALL StartLoadingScreen(void);

#endif // GUARD_CUTSCENE_H
