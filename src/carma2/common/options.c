#include "options.h"

#include "errors.h"
#include "globvars.h"

#include "drmem.h"

#include "flicplay.h"

#include "font.h"

#include "graphics.h"

#include "interface-graphics.h"

#include "intrface.h"

#include "network.h"

#include "platform.h"

extern void C2_HOOK_FASTCALL LoadKeyMapping(void);
extern void C2_HOOK_FASTCALL FUN_004b3ec0(void);
extern int gUNK_0065bd80;
extern int gUNK_0065bda0;
extern int gUNK_0065bdc0;
extern int gUNK_0065bde0;
extern int gUNK_0065be00;
extern int gUNK_0065be20;
extern int gUNK_0065be40;
extern int gUNK_0065be60;
extern int gUNK_0065be80;
extern int gUNK_0065bea0;
extern int gUNK_0065bec0;
extern int gUNK_0065bee0;
extern float gUNK_0058a6e0;

#include "loading.h"
#include "utility.h"

#include "brucetrk.h"

#include "depth.h"

#include "graphics1.h"

#include "sound.h"

#include "spark.h"

#include "world.h"

#include "c2_string.h"

#include "carpocalypse2_macros.h"
#include "carpocalypse2_types.h"
// (options.h contents inlined in the merged headers)
#include "loading1.h"
#include "packfile.h"
#include "globvars.h"
extern void C2_HOOK_FASTCALL LoadKeyMapping(void);
// GLOBAL: CARMA2_HW 0x006869e0
extern int gBackup_key_mappings[4][77];
extern int gKey_map_index;
extern int gKey_mapping[77];
extern int gOrig_key_map_index;

// GLOBAL: CARMA2_HW 0x00688458
char* gKey_names[153];

// GLOBAL: CARMA2_HW 0x006918d0
char* gOptions_key_names[153];

// GLOBAL: CARMA2_HW 0x0065cd70
tInterface_spec gInterface_0065cd70;

// GLOBAL: CARMA2_HW 0x0065ca90
tInterface_spec gInterface_0065ca90;

// GLOBAL: CARMA2_HW 0x0065c780
tInterface_spec gInterface_0065c780;

// GLOBAL: CARMA2_HW 0x0065c068
tInterface_spec gInterface_0065c068;

// GLOBAL: CARMA2_HW 0x00691798
int gINT_00691798[77];

// GLOBAL: CARMA2_HW 0x0069178c
int gINT_0069178c;

// GLOBAL: CARMA2_HW 0x00691b34
int gINT_00691b34;

// GLOBAL: CARMA2_HW 0x00691b38
int gINT_00691b38;

extern int gUNK_00691788;
extern tU8* gUNK_006918cc;

// GLOBAL: CARMA2_HW 0x0058a6e4
const br_scalar gFloat_0058a6e4 = 0.2f;

// GLOBAL: CARMA2_HW 0x0058a6e8
const br_scalar gFloat_0058a6e8 = 1.0f;

// GLOBAL: CARMA2_HW 0x0058a6ec
const br_scalar gFloat_0058a6ec = 0.5f;

// GLOBAL: CARMA2_HW 0x0058a6f0
const br_scalar gFloat_0058a6f0 = 0.25f;

// GLOBAL: CARMA2_HW 0x00688440
int gOrig_key_map_index;

// GLOBAL: CARMA2_HW 0x006869e0
int gBackup_key_mappings[4][77];


#include "errors.h"
#include "globvars.h"
#include "loading1.h"
#include "utility.h"

#include "c2_string.h"

#include "carpocalypse2_macros.h"
#include "carpocalypse2_types.h"


// FUNCTION: CARMA2_HW 0x004b39f0
void C2_HOOK_FASTCALL DoOptions(void) {

    // Relict from Carmageddon 1, unused here
    UNUSED();
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004b4020
void C2_HOOK_FASTCALL StripControls(char* pStr) {
    int i;
    int len;

    len = strlen(pStr);
    for (i = 0; i < len; i++) {
        if ((tU8)pStr[i] < 0x20 /* ' ' */) {
#ifdef CARPOCALYPSE2_FIX_BUGS
            memmove(&pStr[i], &pStr[i + 1], (len - i) * sizeof(char));
#else
            memcpy(&pStr[i], &pStr[i + 1], (len - i) * sizeof(char));
#endif
            len--;
#ifdef CARPOCALYPSE2_FIX_BUGS
            /* correctly handle stripping consecutive control characters */
            i--;
#endif
        }
    }
}

void C2_HOOK_FASTCALL LoadKeyNames(void) {
    int i;
    FILE* f;
    tPath_name the_path;
    char s[256];

    PathCat(the_path, gApplication_path, "KEYNAMES.TXT");
    f = DRfopen(the_path, "rt");
    if (f == NULL) {
        FatalError(kFatalError_CantOpenKeyNamesFile);
    }
    for (i = 0; i < CARPOCALYPSE2_ASIZE(gKey_names); i++) {
        PFfgets(s, sizeof(s), f);
        StripControls(s);
        gKey_names[i] = BrMemAllocate(strlen(s) + 1, kMem_misc);
        strcpy(gKey_names[i], s);
    }
    PFfclose(f);
}

void C2_HOOK_FASTCALL DisposeKeyNames(void) {
    int i;

    for (i = 0; i < CARPOCALYPSE2_ASIZE(gKey_names); i++) {
        BrMemFree(gKey_names[i]);
    }
}

// FUNCTION: CARMA2_HW 0x004b4070
void C2_HOOK_FASTCALL FUN_004b4070(void) {
    char** p;

    p = gKey_names;
    do {
        BrMemFree(*p);
        p++;
    } while ((int)p < (int)&gKey_names[CARPOCALYPSE2_ASIZE(gKey_names)]);
}

void C2_HOOK_FASTCALL BackupKeyMappings(void) {
    int i;

    gOrig_key_map_index = gKey_map_index;

    C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(gBackup_key_mappings) != 4);
    C2_HOOK_BUG_ON(sizeof(gKey_mapping) != 4 * 77);

    for (i = 0; i < CARPOCALYPSE2_ASIZE(gBackup_key_mappings); i++) {

        gKey_map_index = i;
        LoadKeyMapping();
        memcpy(gBackup_key_mappings[i],
            gKey_mapping,
            sizeof(gKey_mapping));
    }
    gKey_map_index = gOrig_key_map_index;
    memcpy(gKey_mapping,
        gBackup_key_mappings[gKey_map_index],
        sizeof(gKey_mapping));
}

// GLOBAL: CARMA2_HW 0x0065bd80
int gUNK_0065bd80;

// GLOBAL: CARMA2_HW 0x0065bda0
int gUNK_0065bda0;

// GLOBAL: CARMA2_HW 0x0065bdc0
int gUNK_0065bdc0;

// GLOBAL: CARMA2_HW 0x0065bde0
int gUNK_0065bde0;

// GLOBAL: CARMA2_HW 0x0065be00
int gUNK_0065be00;

// GLOBAL: CARMA2_HW 0x0065be20
int gUNK_0065be20;

// GLOBAL: CARMA2_HW 0x0065be40
int gUNK_0065be40;

// GLOBAL: CARMA2_HW 0x0065be60
int gUNK_0065be60;

// GLOBAL: CARMA2_HW 0x0065be80
int gUNK_0065be80;

// GLOBAL: CARMA2_HW 0x0065bea0
int gUNK_0065bea0;

// GLOBAL: CARMA2_HW 0x0065bec0
int gUNK_0065bec0;

// GLOBAL: CARMA2_HW 0x0065bee0
int gUNK_0065bee0;

// GLOBAL: CARMA2_HW 0x0058a6f4
float gUNK_0058a6f4 = 5.0f;

// GLOBAL: CARMA2_HW 0x0058a6e0
float gUNK_0058a6e0 = 35.0f;

// C2PORT-NONMATCHING: best 85.17% | difftest PASS 2000 trials (spec difftest/specs/004b3ec0_FUN_004b3ec0.json) | reason regalloc-switch-chain
// FUNCTION: CARMA2_HW 0x004b3ec0
void C2_HOOK_FASTCALL FUN_004b3ec0(void) {
    int level;

    SetCarSimplificationLevel(gUNK_0065bd80 >= 2 ? gUNK_0065bd80 + 1 : gUNK_0065bd80);
    level = -1;
    switch (gUNK_0065bda0) {
    case 0: level = 2; break;
    case 1: level = 1; break;
    case 2: level = 0; break;
    }
    if (level != -1) {
        SetCarTexturingLevel(level);
    }
    level = -1;
    switch (gUNK_0065bdc0) {
    case 0: level = 3; break;
    case 1: level = 1; break;
    case 2: level = 0; break;
    }
    if (level != -1) {
        SetShadowLevel(level);
    }
    level = -1;
    switch (gUNK_0065bde0) {
    case 0: level = 2; break;
    case 1: level = 1; break;
    case 2: level = 0; break;
    }
    if (level != -1) {
        SetWallTexturingLevel(level);
    }
    level = -1;
    switch (gUNK_0065be00) {
    case 0: level = 1; break;
    case 2: level = 0; break;
    }
    if (level != -1) {
        SetRoadTexturingLevel(level);
    }
    SetYon(gUNK_0058a6e0 - (float)gUNK_0065be20 * gUNK_0058a6f4);
    switch (gUNK_0065be40) {
    case 0:
        SetYonFactor(1.0f);
        break;
    case 1:
        SetYonFactor(0.5f);
        break;
    case 2:
        SetYonFactor(0.25f);
        break;
    case 3:
        SetYonFactor(0.125f);
        break;
    default:
        break;
    }
    SetSkyTextureOn(1 - gUNK_0065be60);
    SetDepthCueingOn(1 - gUNK_0065be80);
    SetAccessoryRendering(1 - gUNK_0065bea0);
    SetSmokeOn(1 - gUNK_0065bec0);
    SetSoundDetailLevel(2 - gUNK_0065bee0);
    SaveOptions();
}
