#include "loading2.h"
extern void C2_HOOK_FASTCALL PossibleService(void);
extern void C2_HOOK_FASTCALL InitFunkGrooveFlags(void);
extern void C2_HOOK_FASTCALL FinishLoadGeneralParameters(void);
extern tHeadup_info gHeadup_image_info[45];
extern br_pixelmap* gHeadup_images[45];
extern tNet_mode gNet_mode;
extern char* gRaces_file_names[9];
extern int gCurrent_race_file_index;
extern int gCountRaceGroups;
extern tRace_group_spec* gRace_groups;


#define DECODE_OFFSET 50
extern void C2_HOOK_FASTCALL PDSetPalette(br_pixelmap* pPalette);
extern int gKey_map_index;
extern int gGoreLevel;
extern int gAnimalsOn;
extern int gFlameThrowerOn;
extern int gExplosives_on;
extern int gTraffic_disabled;
extern tNet_game_player_info gNet_players[12];
extern int gThis_net_player_index;
const char* gNet_avail_names[4] = {
    "never",
    "human",
    "stiff",
    "always"
};

int gGroove_funk_offset;

extern int gFunk_groove_flags[30];

// GLOBAL: CARMA2_HW 0x0068c6ec
FILE* gTempFile;

// GLOBAL: CARMA2_HW 0x0068c718
const char* gPedTextTxtPath;


// GLOBAL: CARMA2_HW 0x00655e38
char gDecode_string[14] = {
    (char)('i' + DECODE_OFFSET),
    (char)(' ' + DECODE_OFFSET),
    (char)('a' + DECODE_OFFSET),
    (char)('m' + DECODE_OFFSET),
    (char)(' ' + DECODE_OFFSET),
    (char)('f' + DECODE_OFFSET),
    (char)('i' + DECODE_OFFSET),
    (char)('d' + DECODE_OFFSET),
    (char)('d' + DECODE_OFFSET),
    (char)('l' + DECODE_OFFSET),
    (char)('i' + DECODE_OFFSET),
    (char)('n' + DECODE_OFFSET),
    (char)('g' + DECODE_OFFSET),
    '\0'
};

// GLOBAL: CARMA2_HW 0x0068c6f8
char gUnderwater_screen_name[32];

// GLOBAL: CARMA2_HW 0x00764ea0
char gDefault_car[32];

// GLOBAL: CARMA2_HW 0x00764ec0
char gBasic_car_name[32];

// GLOBAL: CARMA2_HW 0x007638c0
int gNet_score_targets[8];

// GLOBAL: CARMA2_HW 0x0074b58c
int gDemo_race_rank_equivalent;

// GLOBAL: CARMA2_HW 0x0074b4fc
int gCount_demo_opponents;

// GLOBAL: CARMA2_HW 0x0074b4c0
int gDemo_opponents[15];

// GLOBAL: CARMA2_HW 0x00679308
int gCamera_type;

extern char* gCurrent_palette_pixels;
extern br_pixelmap* gOrig_render_palette;
extern br_pixelmap* gFlic_palette;
extern int gFaded_palette;
extern int gPalette_munged;
extern br_pixelmap* gScratch_palette;
br_pixelmap* gMini_map_glowing_line_palettes[3];
br_pixelmap* gPalette_0074a604;

extern br_colour* gScratch_pixels;
br_pixelmap* gPalette_0074a600;
br_pixelmap* gPalette_0074a66c;
br_pixelmap* gPalette_0074a5fc;
br_pixelmap* gPalette_0074a670;

#include "car.h"
#include "init.h"
#include "structur.h"
#include "loading1.h"
#include "crush1.h"
#include "displays.h"
#include "graphics1.h"
#include "world2.h"
#include "graphics2.h"
#include "mainloop.h"
#include "replay.h"
#include "powerup.h"
#include "world3.h"
#include "opponent.h"
#include "spark.h"
#include "depth.h"
#include "brucetrk.h"
#include "flicplay.h"
#include "utility.h"
#include "input.h"
#include "errors.h"
#include "controls.h"
#include "volume.h"
#include "drone.h"
#include "crush2.h"
#include "camera.h"
#include "pedestrn.h"
#include "graphics3.h"
#include "loading3.h"
#include "movie.h"
#include "sound.h"
#include "packfile.h"
#include "newgame.h"
#include "globvars.h"
#include "carpocalypse2_macros.h"

#include "c2_string.h"

void C2_HOOK_FASTCALL LoadInRegisteeDir(const char *pRoot, const char *pSubDir, int pInitialize_palettes) {
    tPath_name the_path;
    tPath_name the_path2;
    tTWTVFS twt;

    PathCat(the_path, pRoot, pSubDir);
    LoadInFiles(the_path, "PALETTES", DRLoadPalette);
    LoadInFiles(the_path, "SHADETAB", DRLoadShadeTable);

    if (pInitialize_palettes) {
        InitializePalettes();
    }

    PathCat(the_path2, the_path, "PIXELMAP");
    twt = OpenPackFileAndSetTiffLoading(the_path2);
    LoadAllImagesInDirectory(&gMisc_storage_space, the_path2);
    ClosePackFileAndSetTiffLoading(twt);

    LoadInFiles(the_path, "MATERIAL", DRLoadMaterials);
    LoadInFiles(the_path, "MODELS", DRLoadModels);
    LoadInFiles(the_path, "ACTORS", DRLoadActors);
    LoadInFiles(the_path, "LIGHTS", DRLoadLights);
}

// FUNCTION: CARMA2_HW 0x00486e10
void C2_HOOK_FASTCALL LoadInRegistees(void) {

    LoadInRegisteeDir(gApplication_path, "REG", 1);
}

void C2_HOOK_FASTCALL LoadBunchOParameters(tSlot_info* pSlot_info) {
    char s[256];
    const char *str;
    int i;

    /* (armour|power|offensive), single player, each skill level */
    GetThreeInts(gTempFile, &pSlot_info->initial[0], &pSlot_info->initial[1], &pSlot_info->initial[2]);
    /* (armour|power|offensive), each network game type */
    GetALineAndDontArgue(gTempFile, s);
    str = strtok(s, "\t ,/");
    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(pSlot_info->initial_network); i++) {
        sscanf(str, "%d", &pSlot_info->initial_network[i]);
        str = strtok(NULL, "\t ,/");
    }
}

void C2_HOOK_FASTCALL LoadBunchOFloatParameters(tFloat_bunch_info *pBunch) {
    char s[256];
    const char *str;
    int i;

    GetThreeFloats(gTempFile, &pBunch->initial[0], &pBunch->initial[1], &pBunch->initial[2]);
    GetALineAndDontArgue(gTempFile, s);
    str = strtok(s, "\t ,/");
    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(pBunch->initial_network); i++) {
        sscanf(str, "%f", &pBunch->initial_network[i]);
        str = strtok(NULL, "\t ,/");
    }
}

void C2_HOOK_FASTCALL GetHithers(void) {
    char s[256];
    int result;
    int position;
    float camera_hither;
    float cockpit_hither;
    char whitespace[] = "\t ,";

    /* Hithers, general then cockpit mode */
    GetALineAndDontArgue(gTempFile, s);
    result = sscanf(&s[strspn(s, whitespace)], "%f%n", &camera_hither, &position);
    if (result == 0) {
        FatalError(kFatalError_Mysterious_SS, s, "GENERAL.TXT");
    }
    gCamera_hither = camera_hither;
    if (sscanf(&s[position + strspn(&s[position], "\t ,")], "%f", &cockpit_hither) == 0) {
        cockpit_hither = gCamera_hither;
    }
    gCamera_hither *= 2;
    gCamera_cockpit_hither = 2 * cockpit_hither;
}

// FUNCTION: CARMA2_HW 0x00486ef0
void C2_HOOK_FASTCALL LoadGeneralParameters(void) {
    tPath_name the_path;
    int i;
    char* str;
    char s[256];
    br_scalar armour_mult, power_mult, offensive_mult;
    int time;

    PathCat(the_path, gApplication_path, "ACTORS");
    PathCat(the_path, the_path, "PROG.ACT");

    gTempFile = PFfopen(the_path, "rb");
    if (gTempFile != NULL) {
        PFfgets(s, CARPOCALYPSE2_ASIZE(s) - 1, gTempFile);
        PFfclose(gTempFile);

        for (i = 0; (unsigned)i < strlen(gDecode_string); i++) {
            gDecode_string[i] -= DECODE_OFFSET;
        }

        // trim trailing CRLF etc
        while (s[0] != '\0' && s[strlen(s) - 1] < 0x20) {
            s[strlen(s) - 1] = 0;
        }

        if (strcmp(s, gDecode_string) == 0) {
            gDecode_thing = 0;
        }

        for (i = 0; (unsigned)i < strlen(gDecode_string); i++) {
            gDecode_string[i] += DECODE_OFFSET;
        }
    }
    PathCat(the_path, gApplication_path, "GENERAL.TXT");
    gTempFile = DRfopen(the_path, "rt");
    if (gTempFile == NULL) {
        FatalError(kFatalError_FailToOpenGeneralSettings);
    }

    /* Disable TIFF conversion */
    gDisableTiffConversion = GetAnInt(gTempFile);

    GetHithers();

    /* Yon */
    gCamera_yon = GetAFloat(gTempFile);
    /* Camera angle */
    gCamera_angle = GetAFloat(gTempFile);
    /* Headup background brightness amount */
    gDim_amount = GetAnInt(gTempFile);
    /* Initial rank */
    gInitial_rank = GetAnInt(gTempFile);
    /* Credits per rank for each skill level */
    GetThreeInts(gTempFile, &gCredits_per_rank[0], &gCredits_per_rank[1], &gCredits_per_rank[2]);

    ReadCrushSettings(gTempFile);

    /* Time per ped kill for each skill level */
    GetThreeInts(gTempFile, &gTime_per_ped_kill[0], &gTime_per_ped_kill[1], &gTime_per_ped_kill[2]);
    /* Seconds per unit car damage for each skill level (with peds */
    GetThreeFloats(gTempFile, &gSeconds_per_unit_car_damage[0], &gSeconds_per_unit_car_damage[1], &gSeconds_per_unit_car_damage[2]);
    /* Credits per unit car damage for each skill level (with peds) */
    GetThreeFloats(gTempFile, &gCredits_per_unit_car_damage[0], &gCredits_per_unit_car_damage[1], &gCredits_per_unit_car_damage[2]);
    /* Time awarded for wasting car for each skill level (with peds) */
    GetThreeInts(gTempFile, &gTime_wasting_car[0], &gTime_wasting_car[1], &gTime_wasting_car[2]);
    /* Credits awarded for wasting car for each skill level (with peds) */
    GetThreeInts(gTempFile, &gCredits_wasting_car[0], &gCredits_wasting_car[1], &gCredits_wasting_car[2]);
    /* Time awarded for rolling car for each skill level (with peds) */
    GetThreeInts(gTempFile, &gTime_rolling_car[0], &gTime_rolling_car[1], &gTime_rolling_car[2]);
    /* Credits awarded for rolling car for each skill level (with peds) */
    GetThreeInts(gTempFile, &gCredits_rolling_car[0], &gCredits_rolling_car[1], &gCredits_rolling_car[2]);
    /* Credits awarded for checkpoints for each skill level (with peds) */
    GetThreeInts(gTempFile, &gCredits_checkpoint[0], &gCredits_checkpoint[1], &gCredits_checkpoint[2]);
    /* Jump start fine for each level */
    GetThreeInts(gTempFile, &gJump_start_fine[0], &gJump_start_fine[1], &gJump_start_fine[2]);
    /* Credits per second of time bonus */
    GetThreeInts(gTempFile, &gCredits_per_second_time_bonus[0], &gCredits_per_second_time_bonus[1], &gCredits_per_second_time_bonus[2]);
    /* Cunning stunt bonus for each skill level */
    GetThreeInts(gTempFile, &gCunning_stunt_bonus[0], &gCunning_stunt_bonus[1], &gCunning_stunt_bonus[2]);

    /* Cars to use as defaults: */
    GetAString(gTempFile, gDefault_car);
    GetAString(gTempFile, gBasic_car_name);

    gKnobbled_frame_period = 0;
    gOpponent_nastyness_frigger = 1.0f;
    /* Min time in secs after last contact with play before opponent considers repairing */
    gMinTimeOpponentRepair = (int)GetAScalar(gTempFile);
    /* Max time in secs after last contact with play before opponent considers repairing */
    gMaxTimeOpponentRepair = (int)GetAScalar(gTempFile);

    /* Default underwater special volume parameters */
    ParseSpecialVolume(gTempFile, &gDefault_default_water_spec_vol, gUnderwater_screen_name, 0);

    /* Initial armour, single player, each skill level */
    /* Initial armour, each network game type */
    LoadBunchOParameters(&gInitial_APO[0]);

    /* Initial power, single player, each skill level */
    /* Initial power, each network game type */
    LoadBunchOParameters(&gInitial_APO[1]);

    /* Initial offensive, single player, each skill level */
    /* Initial offensive, each network game type */
    LoadBunchOParameters(&gInitial_APO[2]);

    /* Initial potential armour, single player, each skill level */
    /* Initial potential armour, each network game type */
    LoadBunchOParameters(&gInitial_APO_potential[0]);

    /* Initial potential power, single player, each skill level */
    /* Initial potential power, each network game type */
    LoadBunchOParameters(&gInitial_APO_potential[1]);

    /* Initial potential offensive, single player, each skill level */
    /* Initial potential offensive, each network game type */
    LoadBunchOParameters(&gInitial_APO_potential[2]);

    /* Max armour, single player, each skill level */
    /* Max armour, each network game type */
    LoadBunchOParameters(&gMax_APO[0]);

    /* Max power, single player, each skill level */
    /* Max power, each network game type */
    LoadBunchOParameters(&gMax_APO[1]);

    /* Max offensive, single player, each skill level */
    /* Max offensive, each network game type */
    LoadBunchOParameters(&gMax_APO[2]);

    /* APO cost, single player */
    /* APO cost, each network game type */
    LoadBunchOParameters(&gCost_APO);

    /* Trade-in APO value, single player */
    /* Trade-in APO value, each network game type */
    LoadBunchOParameters(&gTrade_in_value_APO);

    /* APO substitution value, single player */
    /* APO substitution, each network game type */
    LoadBunchOParameters(&gSubstitution_value_APO);

    /* APO potential substitution value, single player */
    /* APO potential substitution, each network game type */
    LoadBunchOParameters(&gPotential_substitution_value_APO);

    /* Armour starting value */
    gArmour_starting_value[0] = GetAScalar(gTempFile);
    /* Power starting value */
    gPower_starting_value[0] = GetAScalar(gTempFile);
    /* Offensive starting value */
    gOffensive_starting_value[0] = GetAScalar(gTempFile);
    /* Armour per-level multiplier */
    armour_mult = GetAScalar(gTempFile);
    /* Power per-level multiplier */
    power_mult = GetAScalar(gTempFile);
    /* Offensive per-level multiplier */
    offensive_mult = GetAScalar(gTempFile);
    for (i = 1; i < (int)CARPOCALYPSE2_ASIZE(gArmour_starting_value); i++) {
        gArmour_starting_value[i] = gArmour_starting_value[i - 1] * armour_mult;
        gPower_starting_value[i] = gPower_starting_value[i - 1] * power_mult;
        gOffensive_starting_value[i] = gOffensive_starting_value[i - 1] * offensive_mult;
    }

    /* Powerup number to use when time powerup got during network game */
    gNet_powerup_time_replacement = GetAnInt(gTempFile);

    /* Starting money for each skill level */
    GetThreeInts(gTempFile, &gStarting_money[0], &gStarting_money[1], &gStarting_money[2]);
    /* Starting money in network mode */
    GetALineAndDontArgue(gTempFile, s);
    str = strtok(s, "\t ,/");
#if defined(CARPOCALYPSE2_FIX_BUGS)
    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gNet_starting_money); i++) {
#else
    for (i = 0; i < 5; i++) {
#endif
        sscanf(str, "%d", &gNet_starting_money[i]);
        str = strtok(NULL, "\t ,/");
    }

    /* Repair cost for each skill level (cred per % damage) */
    /* Repair cost for each net game (cred per % damage) */
    LoadBunchOFloatParameters(&gRepair_cost);

    /* Recovery cost for each skill level */
    /* Recovery cost for each net game type */
    LoadBunchOFloatParameters(&gRecovery_cost);

    /* Car softness factor for each net skill level */
    /* Car softness factor for each net game type */
    LoadBunchOFloatParameters(&gCar_softness);

    /* Car-to-car damage multiplier for each net skill level */
    /* Car-to-car damage multiplier for each net game type */
    LoadBunchOFloatParameters(&gCar_car_damage_multiplier);

    /* Score targets for each net game type */
    GetALineAndDontArgue(gTempFile, s);
    str = strtok(s, "\t ,/");
    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gNet_score_targets); i++) {
        sscanf(str, "%d", &gNet_score_targets[i]);
        str = strtok(NULL, "\t ,/");
    }

    /* Pickup respawn min time (seconds) */
    gPickup_respawn_min_time_ms = 1000 * GetAnInt(gTempFile);
    /* Pickup respawn max extra time (seconds) */
    gPickup_respawn_max_extra_time_ms = 1000 * GetAnInt(gTempFile);

    /* Demo race rank equivalent */
    gDemo_race_rank_equivalent = GetAnInt(gTempFile);

    /* Number of demo opponents */
    gCount_demo_opponents = GetAnInt(gTempFile);
    /* Demo opponents */
    for (i = 0; (int)i < gCount_demo_opponents; i++) {
        gDemo_opponents[i] = GetAnInt(gTempFile);
    }

    /* default Gravity Multiplier */
    gDefault_gravity = GetAScalar(gTempFile);

    /* Flic sound delays */
    /* Delay (in seconds) before sound during pre-smack flic */
    gFlic_sound_delay_pre_smack = GetAFloat(gTempFile);
    /* Delay (in seconds) before sound during post-smack flic */
    gFlic_sound_delay_post_smack = GetAFloat(gTempFile);
    /* Delay (in seconds) before sound during 'not in demo' flic */
    gFlic_sound_delay_not_in_demo = GetAFloat(gTempFile);
    /* Delay (in seconds) before sound during post-demo slideshow flic */
    gFlic_sound_delay_post_demo = GetAFloat(gTempFile);

    /* Time (in seconds) that credits take before they reach the recovery
     * amount when self-increasing (if starting at zero)
     *
     * First line is for single-player games, second is for each type of
     * network game. Zero means don't tick up.
     */
    time = 1000 * GetAnInt(gTempFile);
    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gAuto_increase_credits_dt); i++) {
        gAuto_increase_credits_dt[i] = (int)((float)time / (0.02f * (float)gRecovery_cost.initial[i]));
    }
    GetALineAndDontArgue(gTempFile, s);
    str = strtok(s, "\t ,/");
    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gNet_score_targets); i++) {
        int t;
        sscanf(str, "%d", &t);
        gNet_auto_increase_credits_dt[i] = (int)((float)(1000 * t) / (.02f * (float)gRecovery_cost.initial_network[i]));
        str = strtok(NULL, "\t ,/");
    }

    /* Mutant tail thing settings */
    /* Number of links including ball */
    gCount_mutant_tail_parts = GetAnInt(gTempFile);
    /* Mass of each link */
    gMass_mutant_tail_link = GetAFloat(gTempFile);
    /* Mass of ball */
    gMass_mutant_tail_ball = GetAFloat(gTempFile);

    /*  Mine / Mortar weight */
    gMass_mine = GetAFloat(gTempFile);
}

// STUB: CARMA2_HW 0x00487dc0
void C2_HOOK_FASTCALL FinishLoadingGeneral(void) {
#ifndef CARPOCALYPSE2_MATCHING
    FinishLoadGeneralParameters();
#else
    NOT_IMPLEMENTED();
#endif
}

// FUNCTION: CARMA2_HW 0x00487e10
void C2_HOOK_FASTCALL LoadKeyMapping(void) {
    FILE* f;
    tPath_name the_path;
    int i;

    PathCat(the_path, gApplication_path, "KEYMAP_X.TXT");
    the_path[strlen(the_path) - 5] = '0' + gKey_map_index;
    f = DRfopen(the_path, "rt");
    if (f == NULL) {
        FatalError(kFatalError_CouldNotOpenKeyMapFile);
    }

    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gKey_mapping); i++) {
        fscanf((FILE*)f, "%d", &gKey_mapping[i]);
    }
    PFfclose(f);
}

// FUNCTION: CARMA2_HW 0x00487ea0
void C2_HOOK_FASTCALL LoadInterfaceStuff(int pWithin_race) {

    // empty
}

// FUNCTION: CARMA2_HW 0x00487eb0
void C2_HOOK_FASTCALL InitInterfaceLoadState(void) {

    // empty
}

// AdjustCarCoordinates

// LoadSpeedo

// LoadTacho

// LoadHeadups

// OldReadNonCarMechanicsData

// GetPerpendicular

// ReadNonCarMechanicsData

// ReadPartHeader

// ReadMechanicsShapes

// NewReadMechanicsData

// LoadGear

// AddRefOffset

// GetDamageProgram

// AllocateCarActorData

// AllocateCarModelData

// AddCarModelToList

// LinkCarModel

// LinkCarModelsToActor

// GetSimpleMaterial

// ReadShrapnelMaterials

// SetModelFlags

// FindCarShellModel

// LinkCarActorsToGroovidelicsCB

// LinkCarActorsToGroovidelics

// ReadCarKeywordData

// TrashModelMaterialIdentifiers

// CentreModel

// PivotizeWheel

// SetUpGraphicalWheelStuff

// LoadCar

// FUNCTION: CARMA2_HW 0x0048c150
void C2_HOOK_FASTCALL LoadHeadupImages(void) {
    int i;

    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gHeadup_image_info); i++) {
        PossibleService();
        if ((gHeadup_image_info[i].avail == eNet_or_otherwise) ||
            ((gHeadup_image_info[i].avail == eNot_net) && (gNet_mode == 0)) ||
            ((gHeadup_image_info[i].avail == eNet_only) && (gNet_mode != 0))) {
            gHeadup_images[i] = LoadPixelmap(gHeadup_image_info[i].name);
            BRPM_convert(gHeadup_images[i], gBack_screen->type);
        } else {
            gHeadup_images[i] = NULL;
        }
    }
}

// OpenRaceFile

// FUNCTION: CARMA2_HW 0x0048c1c0
void C2_HOOK_FASTCALL LoadRaces(tRace_list_spec* pRace_list, int* pCount, int pRace_type_index) {
    FILE* f;
    tPath_name the_path;
    char s[256];
    char* str;
    tRace_list_spec* race;
    tRace_group_spec* group;
    tTWTVFS twt_file;
    int i;
    int count;
    int j;
    int val;
    int powerup_exclusions[96];
    float low_count;
    float low_nastiness;
    float x_offset;
    float y_offset;
    float high_nastiness;
    float x_span;
    float high_count;
    float y_span;
    float v;
    float r;

    if (!gApplicationDataTwtMounted) {
        twt_file = OpenPackFileAndSetTiffLoading(gApplication_path);
    }
    gCurrent_race_file_index = pRace_type_index + 1;
    PathCat(the_path, gApplication_path, gRaces_file_names[gCurrent_race_file_index]);
    f = DRfopen(the_path, "rt");
    if (f == NULL) {
        FatalError(kFatalError_CannotOpenRacesFile);
    }

    GetAnInt(f);
    gDev_initial_race = 0;
    low_count = GetAScalar(f);
    high_count = GetAScalar(f);
    GetPairOfFloats(f, &y_offset, &x_offset);
    GetPairOfFloats(f, &x_span, &y_span);
    low_nastiness = GetAScalar(f);
    high_nastiness = GetAScalar(f);
    gCountRaceGroups = 0;

    race = pRace_list;
    i = 0;
    for (;;) {
        GetALineAndDontArgue(f, race->name);
        if (strcmp(race->name, "END") == 0) {
            break;
        }
        GetAString(f, race->file_name);
        GetAString(f, race->interface_name);
        race->count_opponents = GetAnInt(f);
        race->count_explicit_opponents = GetAnInt(f);
        for (j = 0; j < race->count_explicit_opponents; j++) {
            race->explicit_opponents[j] = GetAnInt(f);
        }
        race->opponent_nastiness_level = GetAScalar(f);

        /* Powerup exclusions list, terminated by a negative entry */
        GetALineAndDontArgue(f, s);
        str = strtok(s, "\t ,/");
        count = 0;
        while (str != NULL) {
            sscanf(str, "%d", &powerup_exclusions[count]);
            if (powerup_exclusions[count] < 0) {
                break;
            }
            count++;
            str = strtok(NULL, "\t ,/");
        }
        race->count_powerup_exclusions = count;
        if (count > 0) {
            race->powerup_exclusions = BrMemAllocate(count * sizeof(int), kMem_misc);
            memcpy(race->powerup_exclusions, powerup_exclusions, count * sizeof(int));
        }

        race->no_time_awards = GetAnInt(f);
        race->is_boundary = GetAnInt(f);
        if (race->is_boundary) {
            gCountRaceGroups++;
        }
        race->race_type = GetAnInt(f);
        GetThreeInts(f, &race->initial_timer[0], &race->initial_timer[1], &race->initial_timer[2]);
        switch (race->race_type) {
            case kRaceType_Carma1:
            case kRaceType_Checkpoints:
                race->count_laps = GetAnInt(f);
                break;
            case kRaceType_Cars:
                race->options.cars.count_opponents = GetAnInt(f);
                for (j = 0; j < race->options.cars.count_opponents; j++) {
                    race->options.cars.opponents[j] = GetAnInt(f);
                }
                break;
            case kRaceType_Peds:
                race->options.peds.count_ped_groups = GetAnInt(f);
                for (j = 0; j < race->options.peds.count_ped_groups; j++) {
                    race->options.peds.ped_groups[j] = GetAnInt(f);
                }
                break;
            case kRaceType_Smash:
                race->options.smash.var_smash_number = GetAnInt(f);
                race->options.smash.var_smash_target = GetAnInt(f);
                break;
            case kRaceType_SmashNPed:
                race->options.smash_and_peds.var_smash_number = GetAnInt(f);
                race->options.smash_and_peds.var_smash_target = GetAnInt(f);
                race->options.smash_and_peds.ped_group_index = GetAnInt(f);
                break;
            default:
                break;
        }
        GetThreeInts(f, &race->completion_bonus[0], &race->completion_bonus[1], &race->completion_bonus[2]);
        if (race->race_type == kRaceType_Carma1) {
            GetThreeInts(f, &race->completion_bonus_peds[0], &race->completion_bonus_peds[1], &race->completion_bonus_peds[2]);
            GetThreeInts(f, &race->completion_bonus_opponents[0], &race->completion_bonus_opponents[1], &race->completion_bonus_opponents[2]);
        }
        GetALineAndDontArgue(f, race->description);
        if (DRStricmp(race->description, "none") == 0) {
            race->description[0] = 0;
        }
        race->expansion = GetAnInt(f) & 1;

        if ((pRace_type_index < 0) || (race->race_type == kRaceType_Carma1)) {
            race++;
            i++;
        }
    }

    *pCount = i;
    PFfclose(f);
    if (gRace_groups != NULL) {
        BrMemFree(gRace_groups);
    }
    gRace_groups = BrMemAllocate(sizeof(tRace_group_spec) * (gCountRaceGroups ? gCountRaceGroups : 1), kMem_misc);
    gRace_groups[0].count_races = 0;
    gRace_groups[0].races = pRace_list;

    race = pRace_list;
    group = gRace_groups;
    for (i = 0; i < *pCount; i++) {
        if (race->opponent_nastiness_level <= 0.0f) {
            race->opponent_nastiness_level = low_nastiness + (high_nastiness - low_nastiness) * ((float)i / (*pCount - 1));
        }
        if (race->count_opponents < 0) {
            count = (int)(low_count + (high_count - low_count) * ((float)i / (*pCount - 1)));
        } else {
            count = race->count_opponents;
        }
        race->count_opponents = 0;
        if (count > race->count_explicit_opponents) {
            v = x_offset + (x_span - x_offset) * ((float)i / (*pCount - 1));
            r = y_offset + (y_span - y_offset) * ((float)i / (*pCount - 1));
            for (j = race->count_explicit_opponents; j < count; j++) {
                val = (int)(v + (r - v) * ((float)(j - race->count_explicit_opponents) / (race->count_explicit_opponents - 1)));
                if (val < 1) {
                    val = -1;
                } else if (val > 5) {
                    val = -5;
                } else {
                    val = -val;
                }
                race->explicit_opponents[j] = val;
            }
        }
        race->count_explicit_opponents = count;
        race->group = group;
        if (race->is_boundary) {
            group->mission = race;
            group++;
            if ((group - gRace_groups) < gCountRaceGroups) {
                group->count_races = 0;
                group->races = &race[1];
            }
        } else {
            group->count_races++;
        }
        race++;
    }

    gCurrent_race_group = gRace_groups;
    if (!gApplicationDataTwtMounted) {
        ClosePackFileAndSetTiffLoading(twt_file);
    }
}

// LoadRaceInfo

// FUNCTION: CARMA2_HW 0x0048c930
void C2_HOOK_FASTCALL LoadOpponents(void) {
    FILE* f;
    tPath_name the_path;
    int i;
    int j;
    int k;
    char s[256];
    char* str;
    tText_chunk* the_chunk;

    PathCat(the_path, gApplication_path, "OPPONENT.TXT");
    f = DRfopen(the_path, "rt");
    if (f == NULL) {
        FatalError(kFatalError_CannotOpenOpponentsFile);
    }
    GetALineAndDontArgue(f, s);
    str = strtok(s, "\t ,/");
    sscanf(str, "%d", &gNumber_of_racers);
    gOpponents = BrMemAllocate(sizeof(tOpponent) * gNumber_of_racers, kMem_oppo_array);

    for (i = 0; i < gNumber_of_racers; i++) {
        PossibleService();
        GetALineAndDontArgue(f, gOpponents[i].name);
        if (strcmp(gOpponents[i].name, "END") == 0) {
            FatalError(kFatalError_OpponentCountMismatchesActualNumberOfOpponents);
        }
        GetALineAndDontArgue(f, s);
        strcpy(gOpponents[i].abbrev_name, strtok(s, "\t ,/"));
        GetALineAndDontArgue(f, gOpponents[i].car_name);
        /* Strength rating (1-5) */
        gOpponents[i].strength_rating = GetAnInt(f);
        /* Cost to buy it */
        gOpponents[i].price = GetAnInt(f);
        /* Network availability ('eagle', or 'all') */
        gOpponents[i].network_availability = GetALineAndInterpretCommand(f, gNet_avail_names, CARPOCALYPSE2_ASIZE(gNet_avail_names));

        gOpponents[i].mug_shot_image_data = NULL;
        gOpponents[i].grid_icon_image = NULL;
        gOpponents[i].stolen_car_image_data = NULL;

        /* vehicle filename */
        GetALineAndDontArgue(f, s);
        strcpy(gOpponents[i].car_file_name, strtok(s, "\t ,/"));

        /* vehicle description */
        GetALineAndDontArgue(f, gOpponents[i].line1_topspeed);
        GetALineAndDontArgue(f, gOpponents[i].line2_weight);
        GetALineAndDontArgue(f, gOpponents[i].line3_acceleration);
        GetALineAndDontArgue(f, gOpponents[i].line4_description);

        C2_HOOK_BUG_ON(sizeof(tText_chunk) != 52);
        gOpponents[i].text_chunk_count = 0;
        gOpponents[i].text_chunks = BrMemAllocate(sizeof(tText_chunk) * gOpponents[i].text_chunk_count, kMem_oppo_text_chunk);

        for (j = 0; j < gOpponents[i].text_chunk_count; j++) {
            the_chunk = &gOpponents[i].text_chunks[j];
            PossibleService();
            GetPairOfInts(f, &the_chunk->x_coord, &the_chunk->y_coord);
            GetPairOfInts(f, &the_chunk->frame_cue, &the_chunk->frame_end);
            the_chunk->line_count = GetAnInt(f);
            while (the_chunk->line_count > CARPOCALYPSE2_ASIZE(the_chunk->text)) {
                the_chunk->line_count--;
                GetALineAndDontArgue(f, s);
            }

            for (k = 0; k < the_chunk->line_count; k++) {
                GetALineAndDontArgue(f, s);
                the_chunk->text[k] = BrMemAllocate(strlen(s) + 1, kMem_oppo_text_str);
                strcpy(the_chunk->text[k], s);
            }
        }
        gOpponents[i].dead = 0;
        InitOpponentPsyche(i);
    }
    GetALineAndDontArgue(f, s);
    if (strcmp(s, "END") != 0) {
        FatalError(kFatalError_OpponentCountMismatchesActualNumberOfOpponents);
    }
    PFfclose(f);
}


// FUNCTION: CARMA2_HW 0x0048cd80
void C2_HOOK_FASTCALL AboutToLoadFirstCar(void) {

    memset(&gFunk_groove_flags[1], 0, (CARPOCALYPSE2_ASIZE(gFunk_groove_flags) - 1) * sizeof(int));
    gGroove_funk_offset = 0;
}

// LoadOpponentsCars

// FUNCTION: CARMA2_HW 0x0048cfc0
void C2_HOOK_FASTCALL LoadMiscStrings(void) {
    int i;
    FILE *f;
    char s[256];
    tPath_name the_path;

    if (gPedTextTxtPath == NULL) {
        SetDefaultTextFileName();
    }
    PathCat(the_path, gApplication_path, gPedTextTxtPath);
    f = DRfopen(the_path, "rt");
    if (f == NULL) {
        FatalError(kFatalError_CannotOpenTEXT_TXT);
    }
    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gMisc_strings); i++) {
        if (PFfeof(f)) {
            break;
        }
        GetALineAndDontArgue(f, s);
        gMisc_strings[i] = BrMemAllocate(strlen(s) + 1, kMem_misc_string);
        strcpy(gMisc_strings[i], s);
    }
    // Thousands delimiter
    gMisc_strings[294][1] = '\0';
    if (gMisc_strings[294][0] == 'C') {
        gMisc_strings[294][0] = ',';
    } else if (gMisc_strings[294][0] == 'P') {
        gMisc_strings[294][0] = '.';
    } else if (gMisc_strings[294][0] == 'S') {
        gMisc_strings[294][0] = ' ';
    }
    PFfclose(f);
}

// FillInRaceInfo

// FUNCTION: CARMA2_HW 0x0048d110
void C2_HOOK_FASTCALL ReadNetworkSettings(FILE* pF, tNet_game_options* pOptions) {

    pOptions->enable_text_messages = GetAnInt(pF);
    pOptions->show_players_on_map = GetAnInt(pF);
    pOptions->show_powerups_on_map = GetAnInt(pF);
    pOptions->powerup_respawn = GetAnInt(pF);
    pOptions->waste_to_transfer = GetAnInt(pF);
    pOptions->open_game = GetAnInt(pF);
    pOptions->grid_start = GetAnInt(pF);
    pOptions->race_sequence_type = GetAnInt(pF);
    pOptions->random_car_choice = GetAnInt(pF);
    pOptions->car_choice = GetAnInt(pF);
    pOptions->starting_credits = GetAnInt(pF);
    pOptions->starting_target = GetAnInt(pF);
}

// PrintNetOptions

// FUNCTION: CARMA2_HW 0x0048d190
int C2_HOOK_FASTCALL SaveOptions(void) {
    tPath_name the_path;
    FILE* f;
    int i;

#ifndef CARPOCALYPSE2_MATCHING
    PathCat(the_path, gApplication_path, "OPTIONS.TXT");
    f = DRfopen(the_path, "wt");
    if (f == NULL) {
        return 0;
    }

    fprintf(f, "YonFactor %f\n", GetYonFactor());
    fprintf(f, "SkyTextureOn %d\n", GetSkyTextureOn());
    fprintf(f, "CarTexturingLevel %d\n", GetCarTexturingLevel());
    fprintf(f, "RoadTexturingLevel %d\n", GetRoadTexturingLevel());
    fprintf(f, "WallTexturingLevel %d\n", GetWallTexturingLevel());
    fprintf(f, "ShadowLevel %d\n", GetShadowLevel());
    fprintf(f, "DepthCueingOn %d\n", GetDepthCueingOn());
    fprintf(f, "Yon %f\n", GetYon());
    fprintf(f, "CarSimplificationLevel %d\n", GetCarSimplificationLevel());
    fprintf(f, "AccessoryRendering %d\n", GetAccessoryRendering());
    fprintf(f, "SmokeOn %d\n", GetSmokeOn());
    fprintf(f, "SoundDetailLevel %d\n", GetSoundDetailLevel());
    fprintf(f, "ScreenSize %d\n", GetScreenSize());
    fprintf(f, "MapRenderX %f\n", gMap_render_x);
    fprintf(f, "MapRenderY %f\n", gMap_render_y);
    fprintf(f, "MapRenderWidth %f\n", gMap_render_width);
    fprintf(f, "MapRenderHeight %f\n", gMap_render_height);
    fprintf(f, "MapMode %d\n", gMap_view);
    fprintf(f, "MapTrans %d\n", gMap_trans);
    fprintf(f, "HeadupMapX %d\n", gHeadup_map_x);
    fprintf(f, "HeadupMapY %d\n", gHeadup_map_y);
    fprintf(f, "HeadupMapW %d\n", gHeadup_map_w);
    fprintf(f, "HeadupMapH %d\n", gHeadup_map_h);
    fprintf(f, "PlayerName\n%s\n", gProgram_state.player_name);
    fprintf(f, "EVolume %d\n", gProgram_state.effects_volume);
    fprintf(f, "MVolume %d\n", gProgram_state.music_volume);
    fprintf(f, "KeyMapIndex %d\n", gKey_map_index);
    fprintf(f, "CameraType %d\n", gCamera_type);
    fprintf(f, "ARCameraType %d\n", gAR_camera_type);
    fprintf(f, "GoreLevel %d\n", 2 - gGoreLevel);
    fprintf(f, "AnimalsOn %d\n", gAnimalsOn);
    fprintf(f, "FlameThrowerOn %d\n", gFlameThrowerOn);
    fprintf(f, "MinesOn %d\n", gExplosives_on);
    fprintf(f, "DronesOn %d\n", !gTraffic_disabled);
    fprintf(f, "MiniMapVisible %d\n", gMini_map_visible);
    fprintf(f, "SkillLevel %d\n", gProgram_state.skill_level);
    fprintf(f, "AmbientSound %d\n", gAmbient_sound);
    fprintf(f, "AutoLoad %d\n", gAuto_load);
    fprintf(f, "RussellsFannies %d\n", gRussels_fannies);
    fprintf(f, "QuickTimeQuality\n%s\n", gQuick_time_quality);
    fprintf(f, "QuickTimeCompressor\n%s\n", gQuick_time_compressor);
    fprintf(f, "QuickTimeBanner %d\n%s\n", gQuick_time_banner_number, gQuick_time_banner_texture_name);
    fprintf(f, "QuickTimeTempPath\n%s\n", gQuick_time_temp_path);
    fprintf(f, "QuickTimeMoviePathStub\n%s\n", gQuick_time_movie_path_stub);
    fprintf(f, "NetName\n%s\n", gNet_players[gThis_net_player_index].player_name);
    fprintf(f, "NETGAMETYPE %d\n", gNet_last_game_type);
    for (i = 0; i < 9; i++) {
        PrintNetOptions(f, i);
    }
    fprintf(f, "HeadupDetailLevel %d\n", gHeadup_detail_level);

    PFfclose(f);
    return 1;
#else
    NOT_IMPLEMENTED();
    return 0;
#endif
}

// FUNCTION: CARMA2_HW 0x0048d8f0
int C2_HOOK_FASTCALL RestoreOptions(void) {
    tPath_name the_path;
    FILE* f;
    float arg;
    char line[80];
    char token[80];
    char* s;

    gProgram_state.music_volume = 200;
    gProgram_state.effects_volume = 200;
    gProgram_state.skill_level = 1;
    gMap_render_x = 6.0;
    gMap_render_y = 6.0;
    gMap_render_width = 64.0;
    gMap_render_height = 40.0;
    gMap_trans = 0;
    gHeadup_map_x = 228;
    gHeadup_map_y = 150;
    gHeadup_map_w = 48;
    gHeadup_map_h = 48;
    gHeadup_detail_level = kMax_headup_detail_level;
    gMap_view = 1;
    gMini_map_visible = 1;

    DefaultNetSettings();
    SetQuickTimeDefaults();

    PathCat(the_path, gApplication_path, "OPTIONS.TXT");
    f = DRfopen(the_path, "rt");
    if (f == NULL) {
        return 0;
    }

    while (PFfgets(line, sizeof(line), f)) {
        if (sscanf(line, "%79s%f", token, &arg) == 2) {
            if (strcmp(token, "YonFactor") == 0) {
                SetYonFactor(arg);
            } else if (strcmp(token, "SkyTextureOn") == 0) {
                SetSkyTextureOn((int)arg);
            } else if (strcmp(token, "CarTexturingLevel") == 0) {
                SetCarTexturingLevel((tCar_texturing_level)arg);
            } else if (strcmp(token, "RoadTexturingLevel") == 0) {
                SetRoadTexturingLevel((tRoad_texturing_level)arg);
            } else if (strcmp(token, "WallTexturingLevel") == 0) {
                SetWallTexturingLevel((tWall_texturing_level)arg);
            } else if (strcmp(token, "ShadowLevel") == 0) {
                SetShadowLevel((tShadow_level)arg);
            } else if (strcmp(token, "DepthCueingOn") == 0) {
                SetDepthCueingOn((int)arg);
            } else if (strcmp(token, "Yon") == 0) {
                SetYon(arg);
            } else if (strcmp(token, "CarSimplificationLevel") == 0) {
                SetCarSimplificationLevel((int)arg);
            } else if (strcmp(token, "AccessoryRendering") == 0) {
                SetAccessoryRendering((int)arg);
            } else if (strcmp(token, "SmokeOn") == 0) {
                SetSmokeOn((int)arg);
            } else if (strcmp(token, "SoundDetailLevel") == 0) {
                SetSoundDetailLevel((int)arg);
            } else if (strcmp(token, "ScreenSize") == 0) {
                SetScreenSize((int)arg);
            } else if (strcmp(token, "MapRenderX") == 0) {
                gMap_render_x = arg;
            } else if (strcmp(token, "MapRenderY") == 0) {
                gMap_render_y = arg;
            } else if (strcmp(token, "MapRenderWidth") == 0) {
                gMap_render_width = arg;
            } else if (strcmp(token, "MapRenderHeight") == 0) {
                gMap_render_height = arg;
            } else if (strcmp(token, "MapMode") == 0) {
            } else if (strcmp(token, "MapTrans") == 0) {
                gMap_trans = (int)arg;
            } else if (strcmp(token, "HeadupMapX") == 0) {
                gHeadup_map_x = (int)arg;
            } else if (strcmp(token, "HeadupMapY") == 0) {
                gHeadup_map_y = (int)arg;
            } else if (strcmp(token, "HeadupMapW") == 0) {
                gHeadup_map_w = (int)arg;
            } else if (strcmp(token, "HeadupMapH") == 0) {
                gHeadup_map_h = (int)arg;
            } else if (strcmp(token, "PlayerName") == 0) {
                PFfgets(line, sizeof(line), f);
                s = strtok(line, "\n\r");
                strcpy(gProgram_state.player_name, s);
            } else if (strcmp(token, "EVolume") == 0) {
                gProgram_state.effects_volume = (int)arg;
            } else if (strcmp(token, "MVolume") == 0) {
                gProgram_state.music_volume = (int)arg;
            } else if (strcmp(token, "KeyMapIndex") == 0) {
                gKey_map_index = (int)arg;
            } else if (strcmp(token, "CameraType") == 0) {
                gAction_replay_camera_mode = gCamera_type = (int)arg;
            } else if (strcmp(token, "ARCameraType") == 0) {
                gAR_camera_type = (int)arg;
            } else if (strcmp(token, "GoreLevel") == 0) {
                SetGoreLevel((int)arg);
            } else if (strcmp(token, "AnimalsOn") == 0) {
                SetAnimalsOn((int)arg);
            } else if (strcmp(token, "FlameThrowerOn") == 0) {
                SetFlameThrowerOn((int)arg);
            } else if (strcmp(token, "MinesOn") == 0) {
                SetExplosivesOn((int)arg);
            } else if (strcmp(token, "DronesOn") == 0) {
                SetTrafficOn((int)arg);
            } else if (strcmp(token, "MiniMapVisible") == 0) {
                gMini_map_visible = (int)arg;
            } else if (strcmp(token, "SkillLevel") == 0) {
                gProgram_state.skill_level = (int)arg;
            } else if (strcmp(token, "AmbientSound") == 0) {
                gAmbient_sound = (int)arg;
            } else if (strcmp(token, "AutoLoad") == 0) {
                gAuto_load = (int)arg;
            } else if (strcmp(token, "RussellsFannies") == 0) {
                gRussels_fannies = (int)arg;
            } else if (strcmp(token, "QuickTimeQuality") == 0) {
                PFfgets(line, sizeof(line), f);
                s = strtok(line, "\n\r");
                strcpy(gQuick_time_quality, s);
            } else if (strcmp(token, "QuickTimeCompressor") == 0) {
                PFfgets(line, sizeof(line), f);
                s = strtok(line, "\n\r");
                strcpy(gQuick_time_compressor, s);
            } else if (strcmp(token, "QuickTimeBanner") == 0) {
                gQuick_time_banner_number = (int)arg;
                PFfgets(line, sizeof(line), f);
                s = strtok(line, "\n\r");
                strcpy(gQuick_time_banner_texture_name, s);
            } else if (strcmp(token, "QuickTimeTempPath") == 0) {
                PFfgets(line, sizeof(line), f);
                s = strtok(line, "\n\r");
                strcpy(gQuick_time_temp_path, s);
            } else if (strcmp(token, "QuickTimeMoviePathStub") == 0) {
                PFfgets(line, sizeof(line), f);
                s = strtok(line, "\n\r");
                strcpy(gQuick_time_movie_path_stub, s);
            } else if (strcmp(token, "NetName") == 0) {
                PFfgets(line, sizeof(line), f);
                s = strtok(line, "\n\r");
            } else if (strcmp(token, "NETGAMETYPE") == 0) {
                gNet_last_game_type = (int)arg;
            } else if (strcmp(token, "NETSETTINGS") == 0) {
                ReadNetworkSettings(f, &gNet_settings[(int)arg]);
            } else if (strcmp(token, "HeadupDetailLevel") == 0) {
                gHeadup_detail_level = (int)arg;
            }
        }
    }
    PFfclose(f);
    gMap_view = 1;
    gMap_render_x = 80.f;
    gMap_render_y = 400.f;
    gMap_render_width = 128.f;
    gMap_render_height = 80.f;
    return 1;
}

// InitFunkGrooveFlags

// FUNCTION: CARMA2_HW 0x0048ea00
void C2_HOOK_FASTCALL SetDefaultTextFileName(void) {
    gPedTextTxtPath = "TEXT.TXT";
}


// FUNCTION: CARMA2_HW 0x004b5090
void C2_HOOK_FASTCALL InitializePalettes(void) {
    br_pixelmap* render_palette;

    gCurrent_palette_pixels = BrMemAllocate(256 * sizeof(br_uint_32), kMem_misc);
    gPalette_changed = 0;
    gCurrent_palette = DRPixelmapAllocate(BR_PMT_RGBX_888, 1, 256, gCurrent_palette_pixels, 0);
    gRender_palette = BrTableFind("DRRENDER.PAL");
    if (gRender_palette == NULL) {
        FatalError(kFatalError_unableToFindRequiredPalette);
    }
    NobbleNonzeroBlacks(gRender_palette);
    gOrig_render_palette = BrPixelmapAllocateSub(gRender_palette, 0, 0, gRender_palette->width, gRender_palette->height);
    gOrig_render_palette->pixels = BrMemAllocate(256 * sizeof(br_uint_32), kMem_misc);
    memcpy(gOrig_render_palette->pixels, gRender_palette->pixels, 256 * sizeof(br_uint_32));
    gFlic_palette = BrTableFind("DRACEFLC.PAL");
    render_palette = gRender_palette;
    if (gFlic_palette == NULL) {
        FatalError(kFatalError_unableToFindRequiredPalette);
    }
    ((br_uint_32*)gRender_palette->pixels)[0] = 0;
    memcpy(gCurrent_palette_pixels, gRender_palette->pixels, 256 * sizeof(br_uint_32));
    gPalette_changed = 0;
    if (!gFaded_palette) {
        PDSetPalette(gRender_palette);
    }
    gPalette_munged |= render_palette != gRender_palette;
    gScratch_pixels = BrMemAllocate(256 * sizeof(br_uint_32), kMem_misc);
    gScratch_palette = DRPixelmapAllocate(BR_PMT_RGBX_888, 1, 256, gScratch_pixels, 0);
    gMini_map_glowing_line_palettes[0] = (br_pixelmap*)-1; /* FIXME: invalid pointer! */
    gMini_map_glowing_line_palettes[1] = gPalette_0074a604;
    gMini_map_glowing_line_palettes[2] = gPalette_0074a600;
    gPalette_0074a66c = gPalette_0074a5fc;
    gPalette_0074a670 = NULL;
}