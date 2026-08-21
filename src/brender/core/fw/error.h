#ifndef CARPOCALYPSE2_ERROR_H
#define CARPOCALYPSE2_ERROR_H

#include "c2_hooks.h"

#include "brender/br_types.h"

br_error C2_HOOK_CDECL BrLastErrorGet(void** valuep);

void C2_HOOK_CDECL BrLastErrorSet(br_error type, void* value);

#endif // CARPOCALYPSE2_ERROR_H