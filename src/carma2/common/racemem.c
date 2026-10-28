#include "racemem.h"

#include "carpocalypse2_macros.h"

#include "c2_string.h"
// GLOBAL: CARMA2_HW 0x006ba640
tS8 gRace_memory[1000];

// FUNCTION: CARMA2_HW 0x004e74b0
void C2_HOOK_FASTCALL ClearAllRuntimeVariables(void) {
    C2_HOOK_BUG_ON(sizeof(gRace_memory) != 1000);
    memset(gRace_memory, 0, sizeof(gRace_memory));
}

// FUNCTION: CARMA2_HW 0x004e7440
int C2_HOOK_FASTCALL GetRuntimeVariable(int pAddress) {
    if (pAddress >= 0 && pAddress < 1000) {
        return gRace_memory[pAddress];
    }
}

// FUNCTION: CARMA2_HW 0x004e7460
int C2_HOOK_FASTCALL AccumulateRuntimeVariable(int pAddress, int pAmount) {

    if (pAddress >= 0 && pAddress < 1000) {
        gRace_memory[pAddress] = (tS8)(gRace_memory[pAddress] + pAmount);
        return gRace_memory[pAddress];
    }
}

// FUNCTION: CARMA2_HW 0x004e7480
int C2_HOOK_FASTCALL ToggleRuntimeVariable(int pAddress) {

    if (pAddress >= 0 && pAddress < 1000) {
        if (gRace_memory[pAddress]) {
            gRace_memory[pAddress] = 0;
            return 0;
        } else {
            gRace_memory[pAddress] = 1;
            return 1;
        }
    }
}
