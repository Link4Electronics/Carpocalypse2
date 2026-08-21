#ifndef CARPOCALYPSE2_STDDIAG_H
#define CARPOCALYPSE2_STDDIAG_H

#include "c2_hooks.h"

#include "brender/br_types.h"

extern br_diaghandler BrStdioDiagHandler;
extern br_diaghandler* _BrDefaultDiagHandler;

void C2_HOOK_CDECL BrStdioWarning(char* message);

void C2_HOOK_CDECL BrStdioFailure(char* message);

#endif // CARPOCALYPSE2_STDDIAG_H