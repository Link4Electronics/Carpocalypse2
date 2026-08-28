#ifndef GUARD_ERRORS_H
#define GUARD_ERRORS_H

#include "brender/br_types.h"


#include "c2_hooks.h"

extern void C2_NORETURN C2_HOOK_CDECL FatalError(int pStr_index, ...);

extern void C2_HOOK_FASTCALL CloseDiagnostics(void);

extern void C2_HOOK_FASTCALL OpenDiagnostics(void);

extern void C2_HOOK_CDECL dr_dprintf(const char* fmt_string, ...);

#define CARPOCALYPSE2_ERRORS_H

#include "brender/br_types.h"

#include "c2_hooks.h"


extern int gError_code;

#endif // GUARD_ERRORS_H
