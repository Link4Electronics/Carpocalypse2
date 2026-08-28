#include "tinted.h"

#include "finteray.h"
#include "globvars.h"
#include "globvrkm.h"
#include "input.h"
#include "loading.h"
#include "platform.h"
#include "utility.h"
#include "world.h"

#include "carpocalypse2_macros.h"
#include "carpocalypse2_types.h"
#include "brender/brender.h"

#include "c2_string.h"
void C2_HOOK_FASTCALL FreeAllTintedPolyActor(void);
// GLOBAL: CARMA2_HW 0x0065fb54
int gHud_tinted4 = -1;


// GLOBAL: CARMA2_HW 0x006a0430
extern br_actor* gTintedPolyCamera;
// GLOBAL: CARMA2_HW 0x0065e874
extern int gDefaultOpacity_TintedPoly;
// GLOBAL: CARMA2_HW 0x0065e870
extern br_uint_32 gTintedColourMap_red;
// GLOBAL: CARMA2_HW 0x006a0438
extern br_uint_32 gTintedColourMap_grn;
// GLOBAL: CARMA2_HW 0x006a043c
extern br_uint_32 gTintedColourMap_blu;