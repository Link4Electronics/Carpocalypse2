#include "timers.h"

#include "displays.h"
#include "globvars.h"
#include "graphics.h"

#include "platform.h"

#include "brender/brender.h"

#include "c2_string.h"

#include "carpocalypse2_macros.h"
// GLOBAL: CARMA2_HW 0x006b75e0
int gTimers_stack[19];
