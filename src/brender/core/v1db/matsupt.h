#ifndef CARPOCALYPSE2_MATSUPT_H
#define CARPOCALYPSE2_MATSUPT_H

#include "c2_hooks.h"

#include "brender/br_types.h"

br_material* C2_HOOK_CDECL BrMaterialAllocate(const char* name);

void C2_HOOK_CDECL BrMaterialFree(br_material* m);

#endif // CARPOCALYPSE2_MATSUPT_H