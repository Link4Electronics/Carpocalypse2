#ifndef CARPOCALYPSE2_PMSETUP_H
#define CARPOCALYPSE2_PMSETUP_H

#include "c2_hooks.h"

#include "brender/br_types.h"

extern br_pixelmap_state _pixelmap;

void C2_HOOK_CDECL BrPixelmapBegin(void);

void C2_HOOK_CDECL BrPixelmapEnd(void);

#endif // CARPOCALYPSE2_PMSETUP_H