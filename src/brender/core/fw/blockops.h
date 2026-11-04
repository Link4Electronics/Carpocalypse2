#ifndef C2_BLOCKOPS_H
#define C2_BLOCKOPS_H

#include "c2_hooks.h"

extern void C2_HOOK_CDECL BrBlockFill(void* pDestination, int pValue, int pCount_dwords);

extern void C2_HOOK_CDECL BrBlockCopy(void* pDestination, const void* pSource, int pCount_dwords);

extern void C2_HOOK_CDECL BrFarBlockCopy(void* pDestination, const void* pSource, int pCount_dwords);

#endif /* C2_BLOCKOPS_H */
