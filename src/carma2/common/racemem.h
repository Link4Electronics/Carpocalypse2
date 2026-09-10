#ifndef C2_RACEMEM_H
#define C2_RACEMEM_H

#include "c2_hooks.h"

#include "carpocalypse2_types.h"

extern tU8 gRace_memory[1000];

void C2_HOOK_FASTCALL ClearAllRuntimeVariables(void);

int C2_HOOK_FASTCALL GetRuntimeVariable(int pAddress);

int C2_HOOK_FASTCALL SetRuntimeVariable(int pAddress, int pValue);

#endif /* C2_RACEMEM_H */
