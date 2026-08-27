#include "72-interface-graphics.h"

#include "72-interface.h"
#include "72-interface-options.h"
#include "10-loading2.h"
#include "16-graphics1.h"
#include "17-world2.h"
#include "33-depth.h"
#include "35-intrface.h"
#include "37-brucetrk.h"
#include "57-drone.h"
#include "61-pedestrn.h"
#include "69-sound.h"
#include "globvars.h"

extern int C2_HOOK_FASTCALL Generic_MenuHandler(tFrontend_spec* pFrontend);

extern int gCar_simplification_level;
extern br_scalar gYon_factor;
extern tShadow_level gShadow_level;
extern int gSound_detail_level;
extern int gGoreLevel;
extern int gSky_on;
extern int gDepth_cueing_on;
extern int gTraffic_disabled;
extern int gAnimalsOn;
extern int gFlameThrowerOn;
extern int gExplosives_on;

// GLOBAL: CARMA2_HW 0x0063e528
tFrontend_spec gFrontend_GRAPHICS = {
    "Graphics",
    0,
    53,
    Graphics_Infunc,
    Graphics_Outfunc,
    Generic_MenuHandler,
    &gFrontend_OPTIONS,
    1,
    0,
    0,
    8,
    0,
    0,
    {
        { 0xf9, temp, NULL,         0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 0x7,  temp, (void*)1,     0, 0, 0, 0, 0, 0, 0, 1, 1 },
        { 0x22, temp, NULL,         0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 0x34, temp, NULL,         0, 0, 0, 0, 0, 0, 0, 1, 1 },
        { 0x35, temp, NULL,         0, 0, 0, 0, 0, 0, 0, 1, 1 },
        { 0x36, temp, NULL,         0, 0, 0, 0, 0, 0, 0, 1, 1 },
        { 0x37, temp, NULL,         0, 0, 0, 0, 0, 0, 0, 1, 1 },
        { 0x24, temp, NULL,         0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 0x40, temp, NULL,         0, 0, 0, 0, 0, 0, 0, 1, 1 },
        { 0x41, temp, NULL,         0, 0, 0, 0, 0, 0, 0, 1, 1 },
        { 0x42, temp, NULL,         0, 0, 0, 0, 0, 0, 0, 1, 1 },
        { 0x43, temp, NULL,         0, 0, 0, 0, 0, 0, 0, 1, 1 },
        { 0x25, temp, NULL,         0, 0, 0, 0, 0, 0, 0, 1, 1 },
        { 0x40, temp, NULL,         0, 0, 0, 0, 0, 0, 0, 1, 1 },
        { 0x41, temp, NULL,         0, 0, 0, 0, 0, 0, 0, 1, 1 },
        { 0x42, temp, NULL,         0, 0, 0, 0, 0, 0, 0, 1, 1 },
        { 0x43, temp, NULL,         0, 0, 0, 0, 0, 0, 0, 1, 1 },
        { 0x23, temp, NULL,         0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 0x3b, temp, NULL,         0, 0, 0, 0, 0, 0, 0, 1, 1 },
        { 0x3c, temp, NULL,         0, 0, 0, 0, 0, 0, 0, 1, 1 },
        { 0x3d, temp, NULL,         0, 0, 0, 0, 0, 0, 0, 1, 1 },
        { 0x28, temp, NULL,         0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 0x44, temp, NULL,         0, 0, 0, 0, 0, 0, 0, 1, 1 },
        { 0x45, temp, NULL,         0, 0, 0, 0, 0, 0, 0, 1, 1 },
        { 0x46, temp, NULL,         0, 0, 0, 0, 0, 0, 0, 1, 1 },
        { 0x2a, temp, NULL,         0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 0x2b, temp, NULL,         0, 0, 0, 0, 0, 0, 0, 1, 1 },
        { 0x2c, temp, NULL,         0, 0, 0, 0, 0, 0, 0, 1, 1 },
        { 0x2d, temp, NULL,         0, 0, 0, 0, 0, 0, 0, 1, 1 },
        { 0x26, temp, NULL,         0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 0x38, temp, NULL,         0, 0, 0, 0, 0, 0, 0, 1, 1 },
        { 0x39, temp, NULL,         0, 0, 0, 0, 0, 0, 0, 1, 1 },
        { 0x27, temp, NULL,         0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 0x38, temp, NULL,         0, 0, 0, 0, 0, 0, 0, 1, 1 },
        { 0x39, temp, NULL,         0, 0, 0, 0, 0, 0, 0, 1, 1 },
        { 0x29, temp, NULL,         0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 0x38, temp, NULL,         0, 0, 0, 0, 0, 0, 0, 1, 1 },
        { 0x39, temp, NULL,         0, 0, 0, 0, 0, 0, 0, 1, 1 },
        { 0x2e, temp, NULL,         0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 0x38, temp, NULL,         0, 0, 0, 0, 0, 0, 0, 1, 1 },
        { 0x39, temp, NULL,         0, 0, 0, 0, 0, 0, 0, 1, 1 },
        { 0x2f, temp, NULL,         0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 0x38, temp, NULL,         0, 0, 0, 0, 0, 0, 0, 1, 1 },
        { 0x39, temp, NULL,         0, 0, 0, 0, 0, 0, 0, 1, 1 },
        { 0xf8, temp, NULL,         0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 0x38, temp, NULL,         0, 0, 0, 0, 0, 0, 0, 1, 1 },
        { 0x39, temp, NULL,         0, 0, 0, 0, 0, 0, 0, 1, 1 },
        { 0xe2, temp, NULL,         0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 0x38, temp, NULL,         0, 0, 0, 0, 0, 0, 0, 1, 1 },
        { 0x39, temp, NULL,         0, 0, 0, 0, 0, 0, 0, 1, 1 },
        { 0xe3, temp, NULL,         0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 0x38, temp, NULL,         0, 0, 0, 0, 0, 0, 0, 1, 1 },
        { 0x39, temp, NULL,         0, 0, 0, 0, 0, 0, 0, 1, 1 },
    },
};

// FUNCTION: CARMA2_HW 0x00448f20
int C2_HOOK_FASTCALL GetCarSimplificationLevel(void) {

    return gCar_simplification_level;
}

// FUNCTION: CARMA2_HW 0x00446bb0
br_scalar C2_HOOK_STDCALL GetYon(void) {

    return gCamera_yon;
}

// FUNCTION: CARMA2_HW 0x0040dfe0
br_scalar C2_HOOK_STDCALL GetYonFactor(void) {

    return gYon_factor;
}

// FUNCTION: CARMA2_HW 0x004e9950
tShadow_level C2_HOOK_FASTCALL GetShadowLevel(void) {

    return gShadow_level;
}

// FUNCTION: CARMA2_HW 0x004569e0
int C2_HOOK_FASTCALL GetSoundDetailLevel(void) {

    return gSound_detail_level;
}

// FUNCTION: CARMA2_HW 0x004d6fc0
int C2_HOOK_FASTCALL GetHowMuchBloodAndSnotToSmearAbout(void) {

    return 2 - gGoreLevel;
}

// FUNCTION: CARMA2_HW 0x00446e00
int C2_HOOK_FASTCALL GetSkyTextureOn(void) {

    return gSky_on;
}

// FUNCTION: CARMA2_HW 0x00446f90
int C2_HOOK_FASTCALL GetDepthCueingOn(void) {

    return gDepth_cueing_on;
}

// FUNCTION: CARMA2_HW 0x0044ed00
int C2_HOOK_FASTCALL GetDronesOn(void) {

    return !gTraffic_disabled;
}

// FUNCTION: CARMA2_HW 0x004d6fd0
int C2_HOOK_FASTCALL GetAnimalsOn(void) {

    return gAnimalsOn;
}

// FUNCTION: CARMA2_HW 0x004d6fe0
int C2_HOOK_FASTCALL IsItOkayToFireHorribleBallsOfNastyNapalmDeathAtPerfectlyInnocentPassersByAndByInnocentIDoMeanInTheBiblicalSense(void) {

    return gFlameThrowerOn;
}

// FUNCTION: CARMA2_HW 0x004d6ff0
int C2_HOOK_FASTCALL IsItReallyOKThatWeDontMakeAnyEffortToProtectAnySadFuckersOutThereThatDontWishToSeeInnocentPeopleBlownToBitsByHighExplosiveMinesAndShells(void) {

    return gExplosives_on;
}

// FUNCTION: CARMA2_HW 0x004b4090
void C2_HOOK_FASTCALL GetGraphicsSettingsData(tFrontend_spec* pFrontend) {
    float yon_factor;

    switch (GetCarSimplificationLevel()) {
    case 0:
        SelectThisItemIn(pFrontend, 1, 3);
        break;
    case 1:
        SelectThisItemIn(pFrontend, 1, 4);
        break;
    case 2:
        SelectThisItemIn(pFrontend, 1, 5);
        break;
    case 3:
        SelectThisItemIn(pFrontend, 1, 6);
        break;
    }

    switch ((int)((35.f - GetYon()) / 5.f)) {
    case 0:
        SelectThisItemIn(pFrontend, 2, 8);
        break;
    case 1:
        SelectThisItemIn(pFrontend, 2, 9);
        break;
    case 2:
        SelectThisItemIn(pFrontend, 2, 10);
        break;
    default:
        SelectThisItemIn(pFrontend, 2, 11);
        break;
    }

    yon_factor = GetYonFactor();
    if (yon_factor >= 1.f) {
        SelectThisItemIn(pFrontend, 3, 13);
    } else if (yon_factor >= 0.5f) {
        SelectThisItemIn(pFrontend, 3, 14);
    } else if (yon_factor >= 0.25f) {
        SelectThisItemIn(pFrontend, 3, 15);
    } else {
        SelectThisItemIn(pFrontend, 3, 16);
    }

    switch (GetShadowLevel()) {
    default:
        SelectThisItemIn(pFrontend, 4, 18);
        break;
    case 0:
        SelectThisItemIn(pFrontend, 4, 20);
        break;
    case 1:
        SelectThisItemIn(pFrontend, 4, 19);
        break;
    }

    switch (GetSoundDetailLevel()) {
    case 0:
        SelectThisItemIn(pFrontend, 5, 24);
        break;
    case 1:
        SelectThisItemIn(pFrontend, 5, 23);
        break;
    case 2:
        SelectThisItemIn(pFrontend, 5, 22);
        break;
    }

    switch (GetHowMuchBloodAndSnotToSmearAbout()) {
    case 0:
        SelectThisItemIn(pFrontend, 6, 26);
        break;
    case 1:
        SelectThisItemIn(pFrontend, 6, 27);
        break;
    case 2:
        SelectThisItemIn(pFrontend, 6, 28);
        break;
    }

    if (GetSkyTextureOn()) {
        SelectThisItemIn(pFrontend, 7, 30);
    } else {
        SelectThisItemIn(pFrontend, 7, 31);
    }

    if (GetDepthCueingOn()) {
        SelectThisItemIn(pFrontend, 8, 33);
    } else {
        SelectThisItemIn(pFrontend, 8, 34);
    }

    if (GetDronesOn()) {
        SelectThisItemIn(pFrontend, 9, 36);
    } else {
        SelectThisItemIn(pFrontend, 9, 37);
    }

    if (GetAnimalsOn()) {
        SelectThisItemIn(pFrontend, 10, 39);
    } else {
        SelectThisItemIn(pFrontend, 10, 40);
    }

    if (IsItOkayToFireHorribleBallsOfNastyNapalmDeathAtPerfectlyInnocentPassersByAndByInnocentIDoMeanInTheBiblicalSense()) {
        SelectThisItemIn(pFrontend, 11, 42);
    } else {
        SelectThisItemIn(pFrontend, 11, 43);
    }

    if (IsItReallyOKThatWeDontMakeAnyEffortToProtectAnySadFuckersOutThereThatDontWishToSeeInnocentPeopleBlownToBitsByHighExplosiveMinesAndShells()) {
        SelectThisItemIn(pFrontend, 12, 45);
    } else {
        SelectThisItemIn(pFrontend, 12, 46);
    }

    if (gAuto_load) {
        SelectThisItemIn(pFrontend, 13, 48);
    } else {
        SelectThisItemIn(pFrontend, 13, 49);
    }
    if (gAmbient_sound) {
        SelectThisItemIn(pFrontend, 14, 51);
    } else {
        SelectThisItemIn(pFrontend, 14, 52);
    }
}

// FUNCTION: CARMA2_HW 0x004747e0
int C2_HOOK_FASTCALL Graphics_Infunc(tFrontend_spec* pFrontend) {

    Generic_Infunc(pFrontend);
    GetGraphicsSettingsData(pFrontend);
    return 1;
}

// FUNCTION: CARMA2_HW 0x004b42c0
void C2_HOOK_FASTCALL SetGraphicsSettingsData(tFrontend_spec* pFrontend) {

    switch (WhichItemIsSelectedIn(pFrontend, 1)) {
    case 3:
        SetCarSimplificationLevel(0);
        break;
    case 4:
        SetCarSimplificationLevel(1);
        break;
    case 5:
        SetCarSimplificationLevel(2);
        break;
    case 6:
        SetCarSimplificationLevel(3);
        break;
    }

    switch (WhichItemIsSelectedIn(pFrontend, 2)) {
    case 8:
        SetYon(35.f);
        break;
    case 9:
        SetYon(30.f);
        break;
    case 10:
        SetYon(25.f);
        break;
    case 11:
        SetYon(20.f);
        break;
    }

    switch (WhichItemIsSelectedIn(pFrontend, 3)) {
    case 13:
        SetYonFactor(1.0f);
        break;
    case 14:
        SetYonFactor(0.5f);
        break;
    case 15:
        SetYonFactor(0.25f);
        break;
    case 16:
        SetYonFactor(0.125f);
        break;
    }

    switch (WhichItemIsSelectedIn(pFrontend, 4)) {
    case 18:
        SetShadowLevel(3);
        break;
    case 19:
        SetShadowLevel(1);
        break;
    case 20:
        SetShadowLevel(0);
        break;
    }

    switch (WhichItemIsSelectedIn(pFrontend, 5)) {
    case 22:
        SetSoundDetailLevel(2);
        break;
    case 23:
        SetSoundDetailLevel(1);
        break;
    case 24:
        SetSoundDetailLevel(0);
        break;
    }

    switch (WhichItemIsSelectedIn(pFrontend, 6)) {
    case 26:
        SetGoreLevel(0);
        break;
    case 27:
        SetGoreLevel(1);
        break;
    case 28:
        SetGoreLevel(2);
        break;
    }

    switch (WhichItemIsSelectedIn(pFrontend, 7)) {
    case 30:
        SetSkyTextureOn(1);
        break;
    case 31:
        SetSkyTextureOn(0);
        break;
    }

    switch (WhichItemIsSelectedIn(pFrontend, 8)) {
    case 33:
        SetDepthCueingOn(1);
        break;
    case 34:
        SetDepthCueingOn(0);
        break;
    }

    switch (WhichItemIsSelectedIn(pFrontend, 9)) {
    case 36:
        SetTrafficOn(1);
        break;
    case 37:
        SetTrafficOn(0);
        break;
    }

    switch (WhichItemIsSelectedIn(pFrontend, 10)) {
    case 39:
        SetAnimalsOn(1);
        break;
    case 40:
        SetAnimalsOn(0);
        break;
    }

    switch (WhichItemIsSelectedIn(pFrontend, 11)) {
    case 42:
        SetFlameThrowerOn(1);
        break;
    case 43:
        SetFlameThrowerOn(0);
        break;
    }

    switch (WhichItemIsSelectedIn(pFrontend, 12)) {
    case 45:
        SetExplosivesOn(1);
        break;
    case 46:
        SetExplosivesOn(0);
        break;
    }

    switch (WhichItemIsSelectedIn(pFrontend, 13)) {
    case 48:
        gAuto_load = 1;
        break;
    case 49:
        gAuto_load = 0;
        break;
    }

    switch (WhichItemIsSelectedIn(pFrontend, 14)) {
    case 51:
        gAmbient_sound = 1;
        break;
    case 52:
        gAmbient_sound = 0;
        break;
    }

    SaveOptions();
}

// FUNCTION: CARMA2_HW 0x00474850
int C2_HOOK_FASTCALL Graphics_Outfunc(tFrontend_spec* pFrontend) {

    Generic_Outfunc(pFrontend);
    SetGraphicsSettingsData(pFrontend);
    return 1;
}
