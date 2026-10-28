#include "apo.h"

#include "displays.h"
#include "utility.h"

// MaxOutAPO

// FUNCTION: CARMA2_HW 0x00502e20
void C2_HOOK_FASTCALL ImprovePSPowerup(tCar_spec* pCar, int pIndex) {

    pCar->power_up_levels[pIndex]++;
    NewTextHeadupSlot(4, 0, 3000, -4, GetMiscString(eMiscString_increase_apo_start + pIndex));
}

// BuyPSPowerup

// BuyArmour

// BuyPower

// BuyOffense