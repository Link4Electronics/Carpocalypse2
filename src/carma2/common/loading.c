#include "loading.h"

#include "brucetrk.h"
#include "controls.h"
#include "crush.h"
#include "depth.h"
#include "drmem.h"
#include "drone.h"
#include "errors.h"
#include "explosions.h"
#include "globvars.h"
#include "globvrpb.h"
#include "grafdata.h"
#include "graphics.h"
#include "init.h"
#include "loadsave.h"
#include "newgame.h"
#include "opponent.h"
#include "physics.h"
#include "powerups.h"
#include "replay.h"
#include "sound.h"
#include "spark.h"
#include "temp.h"
#include "utility.h"
#include "world.h"

#include "platform.h"

#include <brender/brender.h>
#include "carpocalypse2_logging.h"
#include "carpocalypse2_macros.h"

#include "c2_math.h"
#include <stdio.h>
#include "c2_stdlib.h"
#include "c2_string.h"

#include "brender/brender.h"
#include "carpocalypse2_types.h"
#include "brender/br_types.h"

#define OPPONENT_APC_IDX 98
// GLOBAL: CARMA2_HW 0x00692070
extern tTwatVfsFile gTwatVfsFiles[51];
// GLOBAL: CARMA2_HW 0x006923a0
extern int gDisableTiffConversionStackPos;
// GLOBAL: CARMA2_HW 0x00692068
extern int gDisableTiffConversionStack[2];
// GLOBAL: CARMA2_HW 0x0068c6ec
extern FILE* gTempFile;
// GLOBAL: CARMA2_HW 0x00763530
extern int gTime_per_ped_kill[3];
// GLOBAL: CARMA2_HW 0x00761ed0
extern float gSeconds_per_unit_car_damage[3];
// GLOBAL: CARMA2_HW 0x0075b930
extern float gCredits_per_unit_car_damage[3];
// GLOBAL: CARMA2_HW 0x00763520
extern int gTime_wasting_car[3];
// GLOBAL: CARMA2_HW 0x0074d3b0
extern int gCredits_wasting_car[3];
// GLOBAL: CARMA2_HW 0x0075b9b0
extern int gTime_rolling_car[3];
// GLOBAL: CARMA2_HW 0x00761a50
extern int gCredits_rolling_car[3];
// GLOBAL: CARMA2_HW 0x00764ea0
char gDefaultCar[32];

// GLOBAL: CARMA2_HW 0x00658610
extern int gAllow_open_to_fail;
// GLOBAL: CARMA2_HW 0x00657320
tHeadup_info gHeadup_image_info[45] = {
    { "", eNot_ever_ever_ever },
    { "", eNot_ever_ever_ever },
    { "", eNot_ever_ever_ever },
    { "", eNot_ever_ever_ever },
    { "GO.PIX", eNet_or_otherwise },
    { "NUMBER1.PIX", eNet_or_otherwise },
    { "NUMBER2.PIX", eNet_or_otherwise },
    { "NUMBER3.PIX", eNet_or_otherwise },
    { "NUMBER4.PIX", eNet_or_otherwise },
    { "NUMBER5.PIX", eNet_or_otherwise },
    { "SPLATTER.PIX", eNet_or_otherwise },
    { "PILEDRIV.PIX", eNet_or_otherwise },
    { "EXTRASTY.PIX", eNet_or_otherwise },
    { "ARTISTIC.PIX", eNet_or_otherwise },
    { "PLAYWITH.PIX", eNet_or_otherwise },
    { "PUNT.PIX", eNet_or_otherwise },
    { "DROP.PIX", eNet_or_otherwise },
    { "POLITE.PIX", eNet_or_otherwise },
    { "2XCOMBO.PIX", eNet_or_otherwise },
    { "3XCOMBO.PIX", eNet_or_otherwise },
    { "4XCOMBO.PIX", eNet_or_otherwise },
    { "5XCOMBO.PIX", eNet_or_otherwise },
    { "6XCOMBO.PIX", eNet_or_otherwise },
    { "7XCOMBO.PIX", eNet_or_otherwise },
    { "8XCOMBO.PIX", eNet_or_otherwise },
    { "9XCOMBO.PIX", eNet_or_otherwise },
    { "10XCOMBO.PIX", eNet_or_otherwise },
    { "BILLIARD.PIX", eNet_or_otherwise },
    { "POLITE.PIX", eNet_or_otherwise },
    { "HEADON.PIX", eNet_or_otherwise },
    { "DESTROY.PIX", eNet_or_otherwise },
    { "CHECKPNT.PIX", eNet_or_otherwise },
    { "TIMEUP.PIX", eNot_net },
    { "RACEOVER.PIX", eNet_or_otherwise },
    { "UWASTED.PIX", eNet_or_otherwise },
    { "MAD.PIX", eNet_only },
    { "GAMEOVER.PIX", eNet_only },
    { "UBROKE.PIX", eNet_only },
    { "ULOST.PIX", eNet_only },
    { "UWON.PIX", eNet_only },
    { "BBQBON.PIX", eNet_or_otherwise },
    { "TAILBON.PIX", eNet_or_otherwise },
    { "DETTAILBON.PIX", eNet_or_otherwise },
    { "MISSIONFAIL.PIX", eNet_or_otherwise },
    { "MISSIONDONE.PIX", eNet_or_otherwise },
};

// GLOBAL: CARMA2_HW 0x0079eb60
br_pixelmap* gHeadup_images[45];

// GLOBAL: CARMA2_HW 0x00655e38
extern char gDecode_string[14];
// GLOBAL: CARMA2_HW 0x00761f44
tSpecial_volume* gDefault_water_spec_vol_real;


// GLOBAL: CARMA2_HW 0x006a0414
extern const char* gPedsFolder;
// GLOBAL: CARMA2_HW 0x00684550
extern const char* gPed_sound_path;
// GLOBAL: CARMA2_HW 0x006a0ad4
extern const char* gPowerup_txt_path;
// GLOBAL: CARMA2_HW 0x0068c718
extern const char* gPedTextTxtPath;
// GLOBAL: CARMA2_HW 0x00657530
char* gRaces_file_names[9] = {
    "RACES.TXT",
    "NETRACES.TXT",
    "NETRACES.TXT",
    "PEDRACES.TXT",
    "RACES.TXT",
    "RACES.TXT",
    "NETRACES.TXT",
    "NETRACES.TXT",
    "NETRACES.TXT"
};

// GLOBAL: CARMA2_HW 0x00594770
const char* gDrone_type_names[4] = {
    "CAR",
    "PLANE",
    "TRAIN",
    "CHOPPER",
};


// GLOBAL: CARMA2_HW 0x0068c6f4
int gCurrent_race_file_index;

// GLOBAL: CARMA2_HW 0x007634ec
int gCountRaceGroups;

// GLOBAL: CARMA2_HW 0x0074d5e4
tRace_group_spec* gRaceGroups2;


// GLOBAL: CARMA2_HW 0x00763480
extern tSlot_info gCost_APO;
// GLOBAL: CARMA2_HW 0x0074d1c0
extern float gArmour_starting_value[100];
// GLOBAL: CARMA2_HW 0x00761f60
extern float gPower_starting_value[100];
// GLOBAL: CARMA2_HW 0x00761d40
extern float gOffensive_starting_value[100];
// GLOBAL: CARMA2_HW 0x0074d1a4
extern int gNet_powerup_time_replacement;
// GLOBAL: CARMA2_HW 0x0075ba20
extern tFloat_bunch_info gCar_softness;
// GLOBAL: CARMA2_HW 0x0074d600
extern tFloat_bunch_info gCar_car_damage_multiplier;
// GLOBAL: CARMA2_HW 0x007638c0
extern int gNet_score_targets[8];
// GLOBAL: CARMA2_HW 0x0074b58c
extern int gDemo_race_rank_equivalent;
// GLOBAL: CARMA2_HW 0x0074b4fc
extern int gCount_demo_opponents;
// GLOBAL: CARMA2_HW 0x0074b4c0
extern int gDemo_opponents[15];
// GLOBAL: CARMA2_HW 0x0068b8b4
extern float gFlic_sound_delay_pre_smack;
// GLOBAL: CARMA2_HW 0x0068b8b8
extern float gFlic_sound_delay_post_smack;
// GLOBAL: CARMA2_HW 0x0068b8bc
extern float gFlic_sound_delay_not_in_demo;
// GLOBAL: CARMA2_HW 0x0068b8c0
extern float gFlic_sound_delay_post_demo;
// GLOBAL: CARMA2_HW 0x00762120
int gWasted_explosion_chance;

// GLOBAL: CARMA2_HW 0x00761f5c
int gExplosion_sound_id;

// GLOBAL: CARMA2_HW 0x007620f8
tExplosion_animation gExplosion_pix_animation_groups;

// GLOBAL: CARMA2_HW 0x00657558
extern const char* gNet_avail_names[4];
// GLOBAL: CARMA2_HW 0x00763540
extern int gFunk_groove_flags[30];
// GLOBAL: CARMA2_HW 0x0074b4a8
extern int gGroove_funk_offset;
// GLOBAL: CARMA2_HW 0x0074b584
tCar_spec* gCurrent_car_spec;

// GLOBAL: CARMA2_HW 0x006574d0
const char* gDamage_names[12] = {
    "engine",
    "transmission",
    "driver",
    "steering",
    "lf_brake",
    "rf_brake",
    "lr_brake",
    "rr_brake",
    "lf_wheel",
    "rf_wheel",
    "lr_wheel",
    "rr_wheel"
};

// GLOBAL: CARMA2_HW 0x00657500
const char* gWheel_actor_names[6] = {
    "FLWHEEL.ACT",
    "FRWHEEL.ACT",
    "RLWHEEL.ACT",
    "RRWHEEL.ACT",
    "IRLWHEEL.ACT",
    "IRRWHEEL.ACT",
};

// GLOBAL: CARMA2_HW 0x00657518
const char* gPivot_actor_names[6] = {
    "FLPIVOT.ACT",
    "FRPIVOT.ACT",
    "RLPIVOT.ACT",
    "RRPIVOT.ACT",
    "IRLPIVOT.ACT",
    "IRRPIVOT.ACT",
};

// GLOBAL: CARMA2_HW 0x0074b500
br_material* gSimple_materials[32];

// GLOBAL: CARMA2_HW 0x006572f0
tU32 gSimple_material_colours[12] = {
    0x005d0303, 0x02a56e04, 0x03a9a905, 0x08006868,
    0x0d3c3626, 0x0e212121, 0x15284c21, 0x1900326a,
    0x1a93491d, 0x1ec0bfc0, 0x23141414, 0x00000000,
};

// GLOBAL: CARMA2_HW 0x00658614
extern int g_source_exists;
// GLOBAL: CARMA2_HW 0x0058f038
int gFirst_drone_processing = 1;

// GLOBAL: CARMA2_HW 0x0079efb8
tU32 gTime_stamp_for_this_munging;

// GLOBAL: CARMA2_HW 0x0079efc4
tU32 gFrame_period_for_this_munging;

// GLOBAL: CARMA2_HW 0x0079efc0
float gDrone_delta_time;

// GLOBAL: CARMA2_HW 0x0079efbc
float gTrack_drone_min_y;

// GLOBAL: CARMA2_HW 0x00594780
const char* gGroove_funk_type_names[3] = {
    "spinnyGroove",
    "steeringGroove",
    "spinnyFunk",
};

// GLOBAL: CARMA2_HW 0x00594790
const char* gAxis_names[3] = {
    "x",
    "y",
    "z",
};

// GLOBAL: CARMA2_HW 0x005947a8
const char* gReverseness_type_names[2] = {
    "forward",
    "reverse",
};

// GLOBAL: CARMA2_HW 0x005947a0
const char* gFunk_speed_control_names[2] = {
    "controller",
    "dronespeed",
};

/* FIXME: this might be a struct */

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x00457590
void C2_HOOK_FASTCALL ConfigureZombiePedSoundPath(void) {
    gPed_sound_path = "ZOMSOUND";
}

// FUNCTION: CARMA2_HW 0x004e0c20
void C2_HOOK_FASTCALL ConfigureZombiePedPowerupTxtPath(void) {
    gPowerup_txt_path = "ZOMPOWERUP.TXT";
}

// FUNCTION: CARMA2_HW 0x0048ea20
void C2_HOOK_FASTCALL ConfigureZombiePedTextTxtPath(void) {
    gPedTextTxtPath = "ZOMTEXT.TXT";
}

// FUNCTION: CARMA2_HW 0x0048f810
void C2_HOOK_FASTCALL ConfigureZombiePedTexturePath(void) {
    gPedTexturePath = "ZOMPIXELMAP";
}

// FUNCTION: CARMA2_HW 0x004d6f50
void C2_HOOK_FASTCALL ConfigurePedZombiePaths(void) {
    gPedsFolder = "ZOMBIES";
    ConfigureZombiePedSoundPath();
    ConfigureZombiePedPowerupTxtPath();
    ConfigureZombiePedTextTxtPath();
    ConfigureZombiePedTexturePath();
}

// FUNCTION: CARMA2_HW 0x00457580
void C2_HOOK_FASTCALL ConfigureBloodPedSoundPath(void) {
    gPed_sound_path = NULL;
}

// FUNCTION: CARMA2_HW 0x004e0c10
void C2_HOOK_FASTCALL ConfigureBloodPedPowerupTxtPath(void) {
    gPowerup_txt_path = "POWERUP.TXT";
}

// FUNCTION: CARMA2_HW 0x0048ea10
void C2_HOOK_FASTCALL ConfigureBloodPedTextTxtPath(void) {
    gPedTextTxtPath = "TEXT.TXT";
}

// FUNCTION: CARMA2_HW 0x0048f800
void C2_HOOK_FASTCALL ConfigureBloodPedTexturePath(void) {
    gPedTexturePath = "PIXELMAP";
}

// FUNCTION: CARMA2_HW 0x004d6f30
void C2_HOOK_FASTCALL ConfigurePedBloodPaths(void) {
    gPedsFolder = "PEDS";
    ConfigureBloodPedSoundPath();
    ConfigureBloodPedPowerupTxtPath();
    ConfigureBloodPedTextTxtPath();
    ConfigureBloodPedTexturePath();
}

// FUNCTION: CARMA2_HW 0x004575a0
void C2_HOOK_FASTCALL ConfigureAlienPedSoundPath(void) {
    gPed_sound_path = "ALSOUND";
}

// FUNCTION: CARMA2_HW 0x004e0c30
void C2_HOOK_FASTCALL ConfigureAlienPedPowerupTxtPath(void) {
    gPowerup_txt_path = "ALPOWERUP.TXT";
}

// FUNCTION: CARMA2_HW 0x0048ea30
void C2_HOOK_FASTCALL ConfigureAlienPedTextTxtPath(void) {
    gPedTextTxtPath = "ALTEXT.TXT";
}

// FUNCTION: CARMA2_HW 0x0048f820
void C2_HOOK_FASTCALL ConfigureAlienPedTexturePath(void) {
    gPedTexturePath = "ALPIXELMAP";
}
#pragma auto_inline(on)

// FUNCTION: CARMA2_HW 0x004d6f70
void C2_HOOK_FASTCALL ConfigurePedAlienPaths(void) {
    gPedsFolder = "ALIENS";
    ConfigureAlienPedSoundPath();
    ConfigureAlienPedPowerupTxtPath();
    ConfigureAlienPedTextTxtPath();
    ConfigureAlienPedTexturePath();
}

// FUNCTION: CARMA2_HW 0x0048f850
tU16 C2_HOOK_FASTCALL ReadU16(FILE* pF) {
    tU16 raw_short;

    PFfread(&raw_short, sizeof(raw_short), 1, pF);
    return raw_short;
}

// FUNCTION: CARMA2_HW 0x0048f870
tU8 C2_HOOK_FASTCALL ReadU8(FILE* pF) {
    tU8 raw_byte;

    PFfread(&raw_byte, sizeof(raw_byte), 1, pF);
    return raw_byte;
}

// FUNCTION: CARMA2_HW 0x0048f8d0
tS16 C2_HOOK_FASTCALL ReadS16(FILE* pF) {
    tS16 raw_short;

    PFfread(&raw_short, sizeof(raw_short), 1, pF);
    return raw_short;
}

// FUNCTION: CARMA2_HW 0x0048f8f0
tS8 C2_HOOK_FASTCALL ReadS8(FILE* pF) {
    tS8 raw_byte;

    PFfread(&raw_byte, sizeof(raw_byte), 1, pF);
    return raw_byte;
}

// FUNCTION: CARMA2_HW 0x0048f980
void C2_HOOK_FASTCALL WriteS32L(FILE* pF, tS32 pNumber) {
    tS32 raw_long;

    raw_long = pNumber;
    PFfwrite(&raw_long, sizeof(raw_long), 1, pF);
}

// FUNCTION: CARMA2_HW 0x0048f9a0
void C2_HOOK_FASTCALL WriteS16L(FILE* pF, tS16 pNumber) {
    tS16 raw_short;

    raw_short = pNumber;
    PFfwrite(&raw_short, sizeof(raw_short), 1, pF);
}

// FUNCTION: CARMA2_HW 0x0048f9d0
void C2_HOOK_FASTCALL WriteS8L(FILE* pF, tS8 pNumber) {
    tS8 raw_byte;

    raw_byte = pNumber;
    PFfwrite(&raw_byte, sizeof(raw_byte), 1, pF);
}

// FUNCTION: CARMA2_HW 0x0048f9f0
void C2_HOOK_FASTCALL SkipBytes(FILE* pF, int pBytes_to_skip) {

    PFfseek(pF, pBytes_to_skip, SEEK_CUR);
}

// FUNCTION: CARMA2_HW 0x0048fa30
tS32 C2_HOOK_FASTCALL MemReadS32(char** pPtr) {
    tS32 raw_long;

    memcpy(&raw_long, *pPtr, sizeof(raw_long));
    *pPtr += sizeof(raw_long);
    return raw_long;
}

// FUNCTION: CARMA2_HW 0x00490350
void C2_HOOK_FASTCALL GetFiveScalars(FILE* pF, br_scalar* pS1, br_scalar* pS2, br_scalar* pS3, br_scalar* pS4, br_scalar* pS5) {
    char s[256];
    char* str;

    GetALineAndDontArgue(pF, s);
    str = strtok(s, "\t ,/");
    sscanf(str, "%f", pS1);
    str = strtok(NULL, "\t ,/");
    sscanf(str, "%f", pS2);
    str = strtok(NULL, "\t ,/");
    sscanf(str, "%f", pS3);
    str = strtok(NULL, "\t ,/");
    sscanf(str, "%f", pS4);
    str = strtok(NULL, "\t ,/");
    sscanf(str, "%f", pS5);
}

void C2_HOOK_FASTCALL GetFourFloats(FILE * pF, float* pF1, float* pF2, float* pF3, float* pF4) {
    char s[256];
    char* str;

    GetALineAndDontArgue(pF, s);
    str = strtok(s, "\t ,/");
    sscanf(str, "%f", pF1);
    str = strtok(NULL, "\t ,/");
    sscanf(str, "%f", pF2);
    str = strtok(NULL, "\t ,/");
    sscanf(str, "%f", pF3);
    str = strtok(NULL, "\t ,/");
    sscanf(str, "%f", pF4);
}

// FUNCTION: CARMA2_HW 0x004b4ac0
int C2_HOOK_FASTCALL DRfgetpos(FILE* pFile, fpos_t* pos) {
    tTwatVfsFile* twtFile;

    if ((int)pFile < CARPOCALYPSE2_ASIZE(gTwatVfsFiles)) {
        twtFile = &gTwatVfsFiles[(uintptr_t)pFile];
        *(tU8 **) pos = twtFile->pos;
        twtFile->error = 0;
        return 0;
    }
    return fgetpos(pFile, pos);
}

// FUNCTION: CARMA2_HW 0x004b4c40
int C2_HOOK_FASTCALL DRferror(FILE* pFile) {
    tTwatVfsFile* twtFile;

    if ((int)pFile < CARPOCALYPSE2_ASIZE(gTwatVfsFiles)) {
        twtFile = &gTwatVfsFiles[(uintptr_t)pFile];
        return twtFile->error;
    }
    return ferror(pFile);
}

// FUNCTION: CARMA2_HW 0x004b4c60
void C2_HOOK_FASTCALL DRclearerr(FILE* pFile) {
    tTwatVfsFile* twtFile;

    if ((int)pFile < CARPOCALYPSE2_ASIZE(gTwatVfsFiles)) {
        twtFile = &gTwatVfsFiles[(uintptr_t)pFile];
        twtFile->error = 0;
        return;
    }
    clearerr(pFile);
}

// FUNCTION: CARMA2_HW 0x004b48c0
int C2_HOOK_FASTCALL DRfgetc2(FILE* pFile) {
    tTwatVfsFile* twtFile;
    int result;

    if ((int)pFile < CARPOCALYPSE2_ASIZE(gTwatVfsFiles)) {
        twtFile = &gTwatVfsFiles[(uintptr_t)pFile];
        if (twtFile->pos >= twtFile->end) {
            twtFile->error = -1;
            return EOF;
        }
        twtFile->error = 0;
        result = *twtFile->pos;
        twtFile->pos++;
        return result;
    }
    return fgetc(pFile);
}

// FUNCTION: CARMA2_HW 0x004b4b30
int C2_HOOK_FASTCALL DRfsetpos(FILE* pF, fpos_t* pos) {
    tTwatVfsFile* twtFile;

    if ((int)pF < CARPOCALYPSE2_ASIZE(gTwatVfsFiles)) {
        twtFile = &gTwatVfsFiles[(uintptr_t)pF];
        if (twtFile->start <= *(tU8**)pos && *(tU8**)pos <= twtFile->end) {
            twtFile->pos = *(tU8**)pos;
            twtFile->error = 0;
            return 0;
        }
        twtFile->error = -1;
        return -1;
    }
    return fsetpos(pF, pos);
}

// FUNCTION: CARMA2_HW 0x004b4730
void C2_HOOK_FASTCALL TWT_Unmount(tTWTVFS twt) {
    if (twt >= 0) {
        BrMemFree(gTwatVfsMountPoints[twt].header);
        gTwatVfsMountPoints[twt].header = NULL;
    }
}

// FUNCTION: CARMA2_HW 0x00487dc0
void C2_HOOK_FASTCALL FinishLoadGeneralParameters(void) {
    gDefault_default_water_spec_vol.screen_pixelmap = BrMapFind(gUnderwater_screen_name);
    ReadExplosionInfo(gTempFile, &gWasted_explosion_chance, &gExplosion_sound_id, &gExplosion_pix_animation_groups);
    ReadPowerupSmashables(gTempFile);
    PFfclose(gTempFile);
}


// FUNCTION: CARMA2_HW 0x0048d7d0
int C2_HOOK_FASTCALL PrintNetOptions(FILE* pF, int pIndex) {

    fprintf(pF, "NETSETTINGS %d\n", pIndex);
    fprintf(pF, "%d // Allow the sending of Abuse-o-Matic(tm) text messages\n", gNet_settings[pIndex].enable_text_messages);
    fprintf(pF, "%d // Show cars on map\n", gNet_settings[pIndex].show_players_on_map);
    fprintf(pF, "%d // Show pickups on map\n", gNet_settings[pIndex].show_powerups_on_map);
    fprintf(pF, "%d // Pickup respawn\n", gNet_settings[pIndex].powerup_respawn);
    fprintf(pF, "%d // Waste to transfer\n", gNet_settings[pIndex].waste_to_transfer);
    fprintf(pF, "%d // Open game\n", gNet_settings[pIndex].open_game);
    fprintf(pF, "%d // Grid start\n", gNet_settings[pIndex].grid_start);
    fprintf(pF, "%d // Race order\n", gNet_settings[pIndex].race_sequence_type);
    fprintf(pF, "%d // Auto-goody\n", gNet_settings[pIndex].random_car_choice);
    fprintf(pF, "%d // Car choice mode\n", gNet_settings[pIndex].car_choice);
    fprintf(pF, "%d // Starting credits\n\n", gNet_settings[pIndex].starting_credits);
    fprintf(pF, "%d // Target\n\n", gNet_settings[pIndex].starting_target);
    return 0;
}

FILE* C2_HOOK_FASTCALL OpenRaceFile(void) {
    FILE* f;
    tPath_name the_path;

    PathCat(the_path, gApplication_path, gRaces_file_names[gCurrent_race_file_index]);
    f = DRfopen(the_path, "rt");
    if (f == NULL) {
        FatalError(kFatalError_CannotOpenRacesFile);
    }
    return f;
}

// FUNCTION: CARMA2_HW 0x0048ba60
void C2_HOOK_FASTCALL LoadHeadups(FILE* pF, int pIndex, tCar_spec* pCar_spec) {
    char s[256];
    char* str;
    int j;
    int number_of_slots;

    C2_HOOK_BUG_ON(offsetof(tCar_spec, headup_slots) != 0x728);
    C2_HOOK_BUG_ON(sizeof(tHeadup_slot) != 44);

    /* pIndex=0 -> Number of headups (external) */
    /* pIndex=1 -> Number of headups (internal) */
    number_of_slots = GetAnInt(pF);
    for (j = 0; j < number_of_slots; j++) {
        /* [ 0]: Development info display
         * [ 1]: Credits earned
         * [ 2]: Pedestrian kill count
         * [ 3]: Pedestrian warning
         * [ 4]: Miscellaneous messages
         * [ 5]: Start of race countdown
         * [ 6]: Fancy bonuses, timeup, race over etc
         * [ 7]: Timer
         * [ 8]: Lap count
         * [ 9]: End of race bonus
         * [10]: Time bonus
         * [11]: Time awarded
         * [12]: Cars out count
         * [13]: Cash in network mode
         * [14]: Cash in network mode
         * */
        GetALineAndDontArgue(pF, s);
        str = strtok(s, "\t ,/");
        sscanf(str, "%d", &pCar_spec->headup_slots[pIndex][j].field_0x28);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->headup_slots[pIndex][j].x);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->headup_slots[pIndex][j].y);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->headup_slots[pIndex][j].colour);
        str = strtok(NULL, "\t ,/");
        strcpy(s, str);
        switch (s[0]) {
        case 'c':
            pCar_spec->headup_slots[pIndex][j].justification = eJust_centre;
            break;
        case 'l':
            pCar_spec->headup_slots[pIndex][j].justification = eJust_left;
            break;
        case 'r':
            pCar_spec->headup_slots[pIndex][j].justification = eJust_right;
            break;
        }
        if (s[1] == 'c') {
            pCar_spec->headup_slots[pIndex][j].cockpit_anchored = 1;
        }
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->headup_slots[pIndex][j].dim_left);
        if (pCar_spec->headup_slots[pIndex][j].dim_left < 0) {
            pCar_spec->headup_slots[pIndex][j].dimmed_background = 0;
        } else {
            pCar_spec->headup_slots[pIndex][j].dimmed_background = 1;
            str = strtok(NULL, "\t ,/");
            sscanf(str, "%d", &pCar_spec->headup_slots[pIndex][j].dim_top);
            str = strtok(NULL, "\t ,/");
            sscanf(str, "%d", &pCar_spec->headup_slots[pIndex][j].dim_right);
            str = strtok(NULL, "\t ,/");
            sscanf(str, "%d", &pCar_spec->headup_slots[pIndex][j].dim_bottom);
        }
    }
}

// FUNCTION: CARMA2_HW 0x0048f6a0
int C2_HOOK_FASTCALL OriginalCarmaCDinDrive(void) {

    return gCD_is_in_drive;
}

// FUNCTION: CARMA2_HW 0x0048f6b0
int C2_HOOK_FASTCALL CarmaCDinDriveOrFullGameInstalled(void) {

    if (gCD_fully_installed) {
        return 1;
    } else {
        return OriginalCarmaCDinDrive();
    }
}

// FUNCTION: CARMA2_HW 0x00513970
br_pixelmap* C2_HOOK_FASTCALL DRPixelmapLoad(char* pFile_name) {
    br_pixelmap* the_map;

    the_map = BrPixelmapLoad(pFile_name);
    if (the_map != NULL) {
        the_map->origin_x = 0;
        the_map->origin_y = 0;
        the_map->row_bytes = (the_map->row_bytes + sizeof(int32_t) - 1) & ~(sizeof(int32_t) - 1);
    }
    return the_map;
}

// FUNCTION: CARMA2_HW 0x0048d0e0
void C2_HOOK_FASTCALL FillInRaceInfo(tRace_info* pThe_race) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tProgram_state, track_file_name, 0x1a1e);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tRace_info, race_spec, 0x98);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tRace_list_spec, file_name, 0x60);

    strcpy(gProgram_state.track_file_name, pThe_race->race_spec->file_name);
}

// FUNCTION: CARMA2_HW 0x0044f640
FILE* C2_HOOK_FASTCALL OpenDroneFile(const char* pDrone_name) {
    tPath_name the_path;
    FILE* f;

    strcpy(the_path, gApplication_path);
    PathCat(the_path, the_path, "DRONES");
    PathCat(the_path, the_path, pDrone_name);
    PathCat(the_path, the_path, pDrone_name);
    strcat(the_path, ".TXT");
    f = DRfopen(the_path, "rt");
    if (f == NULL) {
        FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, the_path);
    }
    return f;
}

void C2_HOOK_FASTCALL LoadDroneTypeInfo(const char* pDrone_name) {
    tPath_name the_path;
    tTWTVFS twt;
    int version;
    int count;
    char s[256];
    char* str;
    FILE* f;
    tDrone_form* drone;

    C2_HOOK_BUG_ON(sizeof(tDrone_form) != 136);

    drone = &gDrone_forms[gCount_drone_forms];
    strcpy(the_path, gApplication_path);
    PathCat(the_path, the_path, "DRONES");
    PathCat(the_path, the_path, pDrone_name);
    twt = OpenPackFileAndSetTiffLoading(the_path);
    f = OpenDroneFile(pDrone_name);

    /* Version of this text file's format */
    GetALineAndDontArgue(f, s);
    strtok(s, "\t ,/");
    if (strcmp(s, "VERSION") != 0) {
        FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, pDrone_name);
    }
    str = strtok(NULL, "\t ,/");
    count = sscanf(str, "%d", &version);
    if (count != 1 || version < 2) {
        FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, pDrone_name);
    }

    /* Name of this drone, for cross-referencing porpoises */
    GetALineAndDontArgue(f, s);
    str = strtok(s," \t ,/");
    if (strcmp(pDrone_name, str) != 0) {
        FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, pDrone_name);
    }
    strncpy(drone->name, pDrone_name, CARPOCALYPSE2_ASIZE(drone->name) - 1);

    drone->type = GetALineAndInterpretCommand(f, gDrone_type_names, CARPOCALYPSE2_ASIZE(gDrone_type_names));
    if (drone->type < 0) {
        FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, pDrone_name);
    }

    /* Mass (tonnes) */
    drone->M = GetAScalar(f);

    /* Centre of mass, as %age distance from front, left, bottom (woof) */
    GetThreeFloats(f, &drone->center.v[0], &drone->center.v[1], &drone->center.v[2]);

    /* Cornering (smooth/sharp) */
    GetALineAndDontArgue(f, s);
    str = strtok(s," \t ,/");
    if (strcmp(str, "smooth") == 0) {
        drone->flags |= 0x1;
    } else if (strcmp(str, "sharp") == 0) {
        drone->flags &= ~0x1;
    } else {
        FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, pDrone_name);
    }

    /* Speed (constant/variable) */
    GetALineAndDontArgue(f, s);
    str = strtok(s," \t ,/");
    if (strcmp(str, "constant") == 0) {
        /* if constant, must be followed by ONE number (the speed), */

        /* (BRU/s) */
        drone->speed = GetAScalar(f);
    } else if (strcmp(str, "variable") == 0) {
        /* if variable, must be followed by THREE numbers on separate lines (accel, max speed, min speed) */

        drone->speed = -1.f;

        /* Max accel (BRU/s/s, 1 BRU/s = 15mph approx.) */
        drone->max_acceleration = GetAScalar(f);

        /* Max speed (BRU/s) (that's about 150 mph...we don't really want the F14 to go at 600mph now, do we?) */
        drone->max_speed = GetAScalar(f);

        /* Min speed (BRU/s) (or taxi speed for a plane) */
        drone->min_speed = GetAScalar(f);
    } else {
        FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, pDrone_name);
    }

    /* Crushability (0 = no crushing, 0.5 average softness, 1.0 very soft, 2.0 extremely soft) */
    drone->crushability = GetAScalar(f);

    if (drone->crushability != 0.f) {
        /* if not 0 (zero) then must be followed by three lines of 2 values */
        /* Crush limits front, back as %age of total length of vehicle */
        GetPairOfFloats(f, &drone->crush_limits_front, &drone->crush_limits_back);

        /* Crush limits left, right */
        GetPairOfFloats(f, &drone->crush_limits_left, &drone->crush_limits_right);

        /* Crush limits left, right */
        GetPairOfFloats(f, &drone->crush_limits_bottom, &drone->crush_limits_top);
    }

    /* Ability to be resurrected after twattage (respawn / norespawn) */
    GetALineAndDontArgue(f, s);
    str = strtok(s," \t ,/");
    if (strcmp(str, "respawn") == 0) {
        drone->flags |= 0x2;
    } else if (strcmp(str, "norespawn") == 0) {
        drone->flags &= ~0x2;
    } else {
        FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, pDrone_name);
    }

    /* orientation relative to path incline: inline (car, plane), vertical (cable car) */
    GetALineAndDontArgue(f, s);
    str = strtok(s," \t ,/");
    if (strcmp(str, "vertical") == 0) {
        drone->flags |= 0x10;
    } else if (strcmp(str, "inline") == 0) {
        drone->flags &= ~0x10;
    } else {
        FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, pDrone_name);
    }

    /* Processing - 'always' or 'distance' */
    GetALineAndDontArgue(f, s);
    str = strtok(s," \t ,/");
    if (strcmp(str, "always") == 0) {
        drone->flags |= 0x4;
    } else if (strcmp(str, "distance") == 0) {
        drone->flags &= ~0x4;
    } else {
        FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, pDrone_name);
    }

    drone->flags &= ~0x8;

    if (version > 2) {
        GetALineAndDontArgue(f, s);
        str = strtok(s, "\t ,/");
        if (strcmp(str, "drivable_on") == 0) {
            drone->flags |= 0x8;
        } else if (strcmp(str, "not_drivable_on") == 0) {
            drone->flags &= ~0x8;
        } else {
            FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, pDrone_name);
        }
    }
    PFfclose(f);
    ClosePackFileAndSetTiffLoading(twt);
}

// FUNCTION: CARMA2_HW 0x0044ed10
void C2_HOOK_FASTCALL LoadPanGameDroneInfo(void) {
    int version;
    FILE* f;
    tPath_name the_path;
    char s[256];
    char* str;

    strcpy(the_path, gApplication_path);
    PathCat(the_path, the_path, "DRONES");
    PathCat(the_path, the_path, "DRONE.TXT");

    f = DRfopen(the_path, "rt");
    if (f == NULL) {
        FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, the_path);
    }
    GetALineAndDontArgue(f, s);
    strtok(s, "\t ,/");
    if (strcmp(s, "VERSION") != 0) {
        FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, the_path);
    }
    str = strtok(NULL, "\t ,/");
    sscanf(str, "%d", &version);
    if (version == 1) {
        memset(gDrone_forms, 0, sizeof(gDrone_forms));
        for (gCount_drone_forms = 0; gCount_drone_forms < CARPOCALYPSE2_ASIZE(gDrone_forms); gCount_drone_forms++) {
            GetALineAndDontArgue(f, s);
            if (strcmp(s, "END OF DRONES") == 0 || PFfeof(f)) {
                break;
            }
            LoadDroneTypeInfo(s);
        }
        PFfclose(f);
    }
}

// FUNCTION: CARMA2_HW 0x0048c860
void C2_HOOK_FASTCALL LoadRaceInfo(int pRace_index, tRace_info* pRace_info) {
    int i;

    pRace_info->index = pRace_index;
    pRace_info->race_spec = &gRace_list[pRace_index];
    if (gNet_mode == eNet_mode_none) {
        pRace_info->number_of_racers = gRace_list[pRace_index].count_explicit_opponents;
    }
    pRace_info->total_laps = gRace_list[pRace_index].count_laps;
    for (i = 0; i < 3; i++) {
        pRace_info->initial_timer[i] = pRace_info->race_spec->initial_timer[i];
        pRace_info->completion_bonus[i] = pRace_info->race_spec->completion_bonus[i];
        pRace_info->completion_bonus_peds[i] = pRace_info->race_spec->completion_bonus_peds[i];
        pRace_info->completion_bonus_opponents[i] = pRace_info->race_spec->completion_bonus_opponents[i];
    }
}

// FUNCTION: CARMA2_HW 0x004a9600
void C2_HOOK_FASTCALL LoadCopCars(void) {

    gProgram_state.AI_vehicles.number_of_cops = 0;
}

// FUNCTION: CARMA2_HW 0x00401030
void C2_HOOK_FASTCALL LoadAIWorldTrackInfo(FILE* pF) {

    PrintMemoryDump(0, "BEFORE LoadInOppoPaths()");
    LoadInOppoPaths(pF);
    PrintMemoryDump(0, "AFTER LoadInOppoPaths(), BEFORE LoadInDronePaths()");
    LoadInDronePaths(pF);
    PrintMemoryDump(0, "AFTER LoadInDronePaths()");
}

void C2_HOOK_FASTCALL LoadGear(FILE* pF, int pIndex, tCar_spec* pCar_spec) {
    char s[256];
    char* str;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, gear_x, 0x2dc);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, gear_y, 0x2e4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, gears_image, 0x1a8);

    /* Gear display x,y,image */
    GetALineAndDontArgue(pF, s);
    str = strtok(s, "\t ,/");
    sscanf(str, "%d", &pCar_spec->gear_x[pIndex]);
    str = strtok(NULL, "\t ,/");
    sscanf(str, "%d", &pCar_spec->gear_y[pIndex]);
    str = strtok(NULL, "\t ,/");
    if (pIndex == 0) {
        pCar_spec->gears_image = LoadPixelmap(str);
    }
}

void C2_HOOK_FASTCALL AdjustCarCoordinates(tCar_spec* pCar) {
    int i;

    for (i = 0; i < CARPOCALYPSE2_ASIZE(pCar->render_left); i++) {
        pCar->render_left[i] -= gCurrent_graf_data->cock_margin_x;
        pCar->render_top[i] -= gCurrent_graf_data->cock_margin_y;
        pCar->render_right[i] -= gCurrent_graf_data->cock_margin_x;
        pCar->render_bottom[i] -= gCurrent_graf_data->cock_margin_y;
    }
    pCar->mirror_left -= gCurrent_graf_data->cock_margin_x;
    pCar->mirror_top -= gCurrent_graf_data->cock_margin_y;
    pCar->mirror_right -= gCurrent_graf_data->cock_margin_x;
    pCar->mirror_bottom -= gCurrent_graf_data->cock_margin_y;
    pCar->speedo_centre_x[1] -= gCurrent_graf_data->cock_margin_x;
    pCar->speedo_centre_y[1] -= gCurrent_graf_data->cock_margin_y;
    pCar->tacho_centre_x[1] -= gCurrent_graf_data->cock_margin_x;
    pCar->tacho_centre_y[1] -= gCurrent_graf_data->cock_margin_y;
    pCar->speedo_x[1] -= gCurrent_graf_data->cock_margin_x;
    pCar->speedo_y[1] -= gCurrent_graf_data->cock_margin_y;
    pCar->tacho_x[1] -= gCurrent_graf_data->cock_margin_x;
    pCar->tacho_y[1] -= gCurrent_graf_data->cock_margin_y;
    for (i = 0; i < CARPOCALYPSE2_ASIZE(pCar->lhands_x); i++) {
        pCar->lhands_x[i] -= gCurrent_graf_data->cock_margin_x;
        pCar->lhands_y[i] -= gCurrent_graf_data->cock_margin_y;
        pCar->rhands_x[i] -= gCurrent_graf_data->cock_margin_x;
        pCar->rhands_y[i] -= gCurrent_graf_data->cock_margin_y;
    }
    for (i = 0; i < CARPOCALYPSE2_ASIZE(pCar->damage_units); i++) {
        pCar->damage_units[i].x_coord -= gCurrent_graf_data->cock_margin_x;
        pCar->damage_units[i].y_coord -= gCurrent_graf_data->cock_margin_y;
    }
}

// FUNCTION: CARMA2_HW 0x0048b810
void C2_HOOK_FASTCALL LoadTacho(FILE* pF, int pIndex, tCar_spec* pCar_spec) {
    char s[256];
    char* str;
    char the_char1;

    /* tacho type, x, y, filename, x-pitch */
#ifdef CARPOCALYPSE2_FIX_BUGS
    /* LoadPixelmap may call strtok internally, which resets glibc's static
     * parse state and would break the strtok(NULL) calls that follow it in the
     * analogue branch. Resolve the image after finishing the token extraction. */
    {
        const char* image_name;
        GetALineAndDontArgue(pF, s);
        str = strtok(s, "\t ,/");
        sscanf(str, "%c", &the_char1);
        if (the_char1 == 'd') {
            pCar_spec->tacho_radius_2[pIndex] = -1;
            str = strtok(NULL, "\t ,/");
            sscanf(str, "%d", &pCar_spec->tacho_x[pIndex]);
            str = strtok(NULL, "\t ,/");
            sscanf(str, "%d", &pCar_spec->tacho_y[pIndex]);
            str = strtok(NULL, "\t ,/");
            pCar_spec->tacho_image[pIndex] = LoadPixelmap(str);
        } else {
            str = strtok(NULL, "\t ,/");
            sscanf(str, "%d", &pCar_spec->tacho_x[pIndex]);
            str = strtok(NULL, "\t ,/");
            sscanf(str, "%d", &pCar_spec->tacho_y[pIndex]);
            str = strtok(NULL, "\t ,/");
            image_name = str;
            str = strtok(NULL, "\t ,/");
            sscanf(str, "%d", &pCar_spec->tacho_centre_x[pIndex]);
            str = strtok(NULL, "\t ,/");
            sscanf(str, "%d", &pCar_spec->tacho_centre_y[pIndex]);
            str = strtok(NULL, "\t ,/");
            sscanf(str, "%d", &pCar_spec->tacho_radius_1[pIndex]);
            str = strtok(NULL, "\t ,/");
            sscanf(str, "%d", &pCar_spec->tacho_radius_2[pIndex]);
            str = strtok(NULL, "\t ,/");
            sscanf(str, "%d", &pCar_spec->tacho_start_angle[pIndex]);
            str = strtok(NULL, "\t ,/");
            sscanf(str, "%d", &pCar_spec->tacho_end_angle[pIndex]);
            str = strtok(NULL, "\t ,/");
            sscanf(str, "%d", &pCar_spec->tacho_needle_colour[pIndex]);
            pCar_spec->tacho_image[pIndex] = LoadPixelmap(image_name);
        }
    }
#else
    GetALineAndDontArgue(pF, s);
    str = strtok(s, "\t ,/");
    sscanf(str, "%c", &the_char1);
    if (the_char1 == 'd') {
        pCar_spec->tacho_radius_2[pIndex] = -1;
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->tacho_x[pIndex]);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->tacho_y[pIndex]);
        str = strtok(NULL, "\t ,/");
        pCar_spec->tacho_image[pIndex] = LoadPixelmap(str);
    } else {
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->tacho_x[pIndex]);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->tacho_y[pIndex]);
        str = strtok(NULL, "\t ,/");
        pCar_spec->tacho_image[pIndex] = LoadPixelmap(str);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->tacho_centre_x[pIndex]);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->tacho_centre_y[pIndex]);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->tacho_radius_1[pIndex]);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->tacho_radius_2[pIndex]);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->tacho_start_angle[pIndex]);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->tacho_end_angle[pIndex]);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->tacho_needle_colour[pIndex]);
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0048b560
void C2_HOOK_FASTCALL LoadSpeedo(FILE* pF, int pIndex, tCar_spec* pCar_spec) {
    char s[256];
    char* str;
    char the_char1;

#ifdef CARPOCALYPSE2_FIX_BUGS
    /* LoadPixelmap may itself call strtok (e.g. when resolving image paths),
     * which resets glibc's static strtok state and breaks any later strtok(NULL)
     * calls in the same line. Extract every token first and load the image
     * afterwards so the parse state cannot be clobbered mid-line. */
    {
        int xpitch = 0;
        const char* image_name;
        GetALineAndDontArgue(pF, s);
        str = strtok(s, "\t ,/");
        sscanf(str, "%c", &the_char1);
        if (the_char1 == 'd') {
            /* Speedo type, x, y, filename, x-pitch */
            pCar_spec->speedo_radius_2[pIndex] = -1;
            str = strtok(NULL, "\t ,/");
            sscanf(str, "%d", &pCar_spec->speedo_x[pIndex]);
            str = strtok(NULL, "\t ,/");
            sscanf(str, "%d", &pCar_spec->speedo_y[pIndex]);
            str = strtok(NULL, "\t ,/");
            image_name = str;
            str = strtok(NULL, "\t ,/");
            sscanf(str, "%d", &xpitch);
            pCar_spec->speedo_image[pIndex] = LoadPixelmap(image_name);
            if (pCar_spec->speedo_image[pIndex] == NULL) {
                FatalError(kFatalError_CannotLoadSpeedoImage);
            }
            pCar_spec->speedo_y_pitch[pIndex] = pCar_spec->speedo_image[pIndex]->height / 10;
            pCar_spec->speedo_x_pitch[pIndex] = xpitch;
        } else {
            str = strtok(NULL, "\t ,/");
            sscanf(str, "%d", &pCar_spec->speedo_x[pIndex]);
            str = strtok(NULL, "\t ,/");
            sscanf(str, "%d", &pCar_spec->speedo_y[pIndex]);
            str = strtok(NULL, "\t ,/");
            image_name = str;
            str = strtok(NULL, "\t ,/");
            sscanf(str, "%d", &pCar_spec->speedo_centre_x[pIndex]);
            str = strtok(NULL, "\t ,/");
            sscanf(str, "%d", &pCar_spec->speedo_centre_y[pIndex]);
            str = strtok(NULL, "\t ,/");
            sscanf(str, "%d", &pCar_spec->speedo_radius_1[pIndex]);
            str = strtok(NULL, "\t ,/");
            sscanf(str, "%d", &pCar_spec->speedo_radius_2[pIndex]);
            str = strtok(NULL, "\t ,/");
            sscanf(str, "%d", &pCar_spec->speedo_start_angle[pIndex]);
            str = strtok(NULL, "\t ,/");
            sscanf(str, "%d", &pCar_spec->speedo_end_angle[pIndex]);
            str = strtok(NULL, "\t ,/");
            sscanf(str, "%d", &pCar_spec->speedo_needle_colour[pIndex]);
            str = strtok(NULL, "\t ,/");
            sscanf(str, "%d", &pCar_spec->max_speed);
            pCar_spec->speedo_image[pIndex] = LoadPixelmap(image_name);
        }
    }
#else
    GetALineAndDontArgue(pF, s);
    str = strtok(s, "\t ,/");
    sscanf(str, "%c", &the_char1);
    if (the_char1 == 'd') {
        /* Speedo type, x, y, filename, x-pitch */
        pCar_spec->speedo_radius_2[pIndex] = -1;
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->speedo_x[pIndex]);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->speedo_y[pIndex]);
        str = strtok(NULL, "\t ,/");
        pCar_spec->speedo_image[pIndex] = LoadPixelmap(str);
        if (pCar_spec->speedo_image[pIndex] == NULL) {
            FatalError(kFatalError_CannotLoadSpeedoImage);
        }
        pCar_spec->speedo_y_pitch[pIndex] = pCar_spec->speedo_image[pIndex]->height / 10;
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->speedo_x_pitch[pIndex]);
    } else {
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->speedo_x[pIndex]);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->speedo_y[pIndex]);
        str = strtok(NULL, "\t ,/");
        pCar_spec->speedo_image[pIndex] = LoadPixelmap(str);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->speedo_centre_x[pIndex]);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->speedo_centre_y[pIndex]);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->speedo_radius_1[pIndex]);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->speedo_radius_2[pIndex]);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->speedo_start_angle[pIndex]);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->speedo_end_angle[pIndex]);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->speedo_needle_colour[pIndex]);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->max_speed);
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0048bca0
void C2_HOOK_FASTCALL GetDamageProgram(FILE* pF, tCar_spec* pCar_spec, tImpact_location pImpact_location) {
    tDamage_clause* the_clause;
    int i;
    int j;
    int k;
    int count;
    char s[256];
    char delim[64];
    char* str;

    PossibleService();
    /* Number of clauses */
    count = GetAnInt(pF);
    pCar_spec->damage_programs[pImpact_location].clause_count = count;
    pCar_spec->damage_programs[pImpact_location].clauses = BrMemAllocate(count * sizeof(tDamage_clause), kMem_damage_clauses);
    strcpy(delim, "\t ,/");
    strcat(delim, "&");

    for (i = 0; i < count; i++) {
        the_clause = &pCar_spec->damage_programs[pImpact_location].clauses[i];
        the_clause->condition_count = 0;
        GetALineAndDontArgue(pF, s);
        str = strtok(s, delim);
        do {
            if (str[0] == 'x') {
                the_clause->conditions[the_clause->condition_count].axis_comp = eAxis_x;
            } else if (str[0] == 'y') {
                the_clause->conditions[the_clause->condition_count].axis_comp = eAxis_y;
            } else if (str[0] == 'z') {
                the_clause->conditions[the_clause->condition_count].axis_comp = eAxis_z;
            } else {
                break;
            }
            if (str[1] == '>') {
                the_clause->conditions[the_clause->condition_count].condition_operator = 1;
            } else if (str[1] == '<') {
                the_clause->conditions[the_clause->condition_count].condition_operator = 0;
            } else {
                break;
            }
            if (str[1] == '>') {
                the_clause->conditions[the_clause->condition_count].condition_operator = eCondition_greater_than;
            } else if (str[1] == '<') {
                the_clause->conditions[the_clause->condition_count].condition_operator = eCondition_less_than;
            } else {
                FatalError(kFatalError_ConfusedByFormatOfConditionalDamageInCarFile);
            }
            sscanf(&str[2], "%f", &the_clause->conditions[the_clause->condition_count].comparitor);
            the_clause->condition_count++;
            str = strtok(NULL, delim);
        } while (the_clause->condition_count < 2);
        /* Systems count */
        the_clause->effect_count = GetAnInt(pF);
        for (j = 0; j < the_clause->effect_count; j++) {
            the_clause->effects[j].type = -1;
            /* Damage */
            GetALineAndDontArgue(pF, s);
            str = strtok(s, "\t ,/");
            for (k = 0; k < CARPOCALYPSE2_ASIZE(gDamage_names); k++) {
                if (strcmp(str, gDamage_names[k]) == 0) {
                    the_clause->effects[j].type = k;
                    break;
                }
            }
            if (the_clause->effects[j].type < 0) {
                FatalError(kFatalError_UnknownDamageType_S, str);
            }
            str = strtok(NULL, "\t ,/");
            sscanf(str, "%f", &the_clause->effects[j].weakness_factor);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004f67a0
void C2_HOOK_FASTCALL UpdateMaterialsForCar(tBrender_storage* pStorage, tCar_spec* pCar_spec) {
    int i;
    br_material* material;

    for (i = pCar_spec->old_material_count; i < pCar_spec->new_material_count; i++) {
        material = pStorage->materials[i];
        if (gNbPixelBits == 16) {
            material->ka = 0.6f;
            material->kd = 0.2f;
            material->flags &= ~BR_MATF_PRELIT;
            material->flags |= BR_MATF_LIGHT | BR_MATF_SMOOTH;
        }
    }
}

// FUNCTION: CARMA2_HW 0x004f6740
void C2_HOOK_FASTCALL LoadCarMaterials(tBrender_storage* pStorage, const char* pPath, tCar_spec* pCar_spec) {

    pCar_spec->old_material_count = pStorage->materials_count;
    LoadAllMaterialsInDirectory(pStorage, pPath, kRendererShadingType_Specular);
    pCar_spec->new_material_count = pStorage->materials_count;
    UpdateMaterialsForCar(pStorage, pCar_spec);
}

// FUNCTION: CARMA2_HW 0x00488f00
void C2_HOOK_FASTCALL LoadCarShrapnelMaterials(FILE* pF, tCar_spec* pCar_spec) {
    int i;
    char s[256];

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, count_shrapnel_materials, 0x6c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, shrapnel_materials, 0x84);

    /* number of materials */
    pCar_spec->count_shrapnel_materials = GetAnInt(pF);

    for (i = 0; i < pCar_spec->count_shrapnel_materials; i++) {
        GetALineAndDontArgue(pF, s);
        pCar_spec->shrapnel_materials[i] = GetSimpleMaterial(s, (pCar_spec == NULL || pCar_spec->driver < 6) ? 1 : 2);
    }

}

// FUNCTION: CARMA2_HW 0x00488f70
void C2_HOOK_FASTCALL LoadCar(const char* pCar_name, tDriver pDriver, tCar_spec* pCar_spec, int pOwner, const char* pDriver_name, tBrender_storage* pStorage_space) {
    tBrender_storage* prev_storage;
    tPath_name the_path;
    char load_name[256];
    char load_directory[256];
    char car_path[256];
    char actor_path[256];
    tTWTVFS twt;
    int i;
    int j;
    int k;
    int version;
    float temp_float;
    FILE* f;
    FILE* g;
    FILE* h;
    char s[256];
    char* str;
    int old_model_count;
    tCar_crush_buffer car_crush_buffer;
    br_model* model;
    tUser_crush_data* user_crush;
    int count_vertices;
    br_material* car_material;
    tCrush_model_pool model_pool;
    v11model* v11;
    int all_fire_zero;

    C2_HOOK_BUG_ON(sizeof(*pCar_spec) != 6500);
    memset(pCar_spec, 0, sizeof(*pCar_spec));

    prev_storage = gStorage_for_callbacks;
    gCurrent_car_spec = pCar_spec;

    if (pDriver == eDriver_local_human) {
        strcpy(gProgram_state.car_name, pCar_name);
    }
    strcpy(gCurrent_load_directory, "CARS");
    strcpy(gCurrent_load_name, pCar_name);
    gCurrent_load_name[strlen(gCurrent_load_name) - 4] = '\0';
    strcpy(load_directory, gCurrent_load_directory);
    strcpy(load_name, gCurrent_load_name);
    PathCat(car_path, gApplication_path, load_directory);
    PathCat(car_path, car_path, load_name);
    twt = OpenPackFileAndSetTiffLoading(car_path);

    pCar_spec->is_girl = 0;
    pCar_spec->driver = pDriver;
    pCar_spec->index = pOwner;

    if (pDriver == eDriver_local_human) {
        gFunk_groove_flags[0] = 1;
        gGroove_funk_offset = 0;
    } else {
        gGroove_funk_offset = -1;
        for (i = 1; i < CARPOCALYPSE2_ASIZE(gFunk_groove_flags); i++) {
            if (!gFunk_groove_flags[i]) {
                pCar_spec->fg_index = i;
                gFunk_groove_flags[i] = 1;
                gGroove_funk_offset = GROOVE_FUNK_MAX_PER_CAR * i;
                break;
            }
        }
    }
    if (gGroove_funk_offset < 0) {
        FatalError(kFatalError_RanOutOfFunkGrooveSlotBunches);
    }

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, proxy_ray_distance, 0x170c);

    if (strcmp(pCar_name, "STELLA.TXT") == 0) {
        pCar_spec->proxy_ray_distance = 6.0f;
    } else {
        pCar_spec->proxy_ray_distance = 0.0f;
    }

    PathCat(the_path, car_path, pCar_name);
    f = DRfopen(the_path, "rt");
    if (f == NULL) {
        FatalError(kFatalError_CannotLoadCarResolutionIndependentFile);
    }

    strcpy(the_path, car_path);
    PathCat(the_path, the_path, gGraf_specs[gGraf_spec_index].data_dir_name);
    PathCat(the_path, the_path, pCar_name);
    AllowOpenToFail();
    g = DRfopen(the_path, "rt");
    DoNotAllowOpenToFail();
    if (g == NULL) {
        PathCat(the_path, gApplication_path, "CARS");
        PathCat(the_path, the_path, gGraf_specs[gGraf_spec_index].data_dir_name);
        PathCat(the_path, the_path, gBasic_car_name);
        g = DRfopen(the_path, "rt");
        if (g == NULL) {
            FatalError(kFatalError_CannotOpenCarResolutionDependentFile);
        }
    }

    /* VERSION X
     *      Version 1 :             New crush data
     *              2 :             New windscreen spec
     **/
    GetALineAndDontArgue(f, s);
    str = strtok(s, "\t ,/");
    if (strcmp(str, "VERSION") == 0) {
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &version);
        GetAString(f, s);
    } else {
        version = 0;
    }
    if (strcmp(s, "BOY") == 0) {
        pCar_spec->is_girl = 0;
        GetAString(f, s);
    } else if (strcmp(s, "GIRL") == 0) {
        pCar_spec->is_girl = 1;
        GetAString(f, s);
    }
    /* Name of car */
    strcpy(pCar_spec->name, s);
    if (DRStricmp(s, pCar_name) != 0) {
        FatalError(kFatalError_FileIsCorrupted_S, pCar_name);
    }
    if (pDriver_name[0] != '\0') {
        memcpy(pCar_spec->driver_name, pDriver_name, CARPOCALYPSE2_ASIZE(pCar_spec->driver_name));
        pCar_spec->driver_name[CARPOCALYPSE2_ASIZE(pCar_spec->driver_name) - 1] = '\0';
    } else {
        strcpy(pCar_spec->driver_name, "X");
    }
    pCar_spec->can_be_stolen = 0;
    pCar_spec->has_been_stolen = 0;
    pCar_spec->knackered = 0;
    pCar_spec->time_last_hit = 0;
    pCar_spec->time_last_victim = 0;
    pCar_spec->disabled = 0;
    pCar_spec->active = 1;
    for (i = 0; i < CARPOCALYPSE2_ASIZE(pCar_spec->power_up_levels); ++i) {
        pCar_spec->power_up_levels[i] = gCurrent_APO_levels[i];
        pCar_spec->power_up_slots[i] = gCurrent_APO_potential_levels[i];
    }
    /* softness_factor */
    pCar_spec->softness_factor = GetAFloat(f);
    /* START OF DRIVABLE STUFF */
    GetALineAndDontArgue(f, s);
    C2_HOOK_ASSERT(strcmp(s, "START OF DRIVABLE STUFF") == 0);
    if (pDriver == eDriver_local_human) {
        if (twt >= 0) {
            PackFileRevertTiffLoading();
        }
        for (i = 0; i < CARPOCALYPSE2_ASIZE(pCar_spec->cockpit_images); i++) {
            /* [0] Cockpit forward image file names
             * [1] Cockpit left image file names
             * [2] Cockpit right image file names */
            GetALineAndDontArgue(g, s);
            strtok(s, "\t ,/");
            pCar_spec->cockpit_images[i] = NULL;

            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, cockpit_images, 0x138);
            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, render_left, 0x1dc);

            /* Left, top, right, bottom rendering coordinates */
            GetALineAndDontArgue(g, s);
            str = strtok(s, "\t ,/");
            sscanf(str, "%d", &pCar_spec->render_left[i]);
            str = strtok(NULL, "\t ,/");
            sscanf(str, "%d", &pCar_spec->render_top[i]);
            str = strtok(NULL, "\t ,/");
            sscanf(str, "%d", &pCar_spec->render_right[i]);
            str = strtok(NULL, "\t ,/");
            sscanf(str, "%d", &pCar_spec->render_bottom[i]);

            PossibleService();
        }
        /* Speedo type, x, y, filename, x-pitch */
        LoadSpeedo(g, 0, pCar_spec);
        /* Speedo type, x, y, filename, x-pitch */
        GetALineAndDontArgue(g, s);
        PossibleService();

        /* tacho type, x, y, filename, x-pitch */
        LoadTacho(g, 0, pCar_spec);
        /* Tacho x, y, filename */
        GetALineAndDontArgue(g, s);
        PossibleService();

        /* Gear display x,y,image (external) */
        LoadGear(g, 0, pCar_spec);
        /* Gear display x,y,image (internal) */
        GetALineAndDontArgue(g, s);
        PossibleService();

        /* Number of hands frames */
        GetALineAndDontArgue(g, s);
        str = strtok(s, "\t ,/");
        sscanf(str, "%d", &pCar_spec->number_of_hands_images);
        for (i = 0; i < pCar_spec->number_of_hands_images; i++) {
            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, lhands_y, 0x30c);
            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, lhands_images, 0x16c);

            /* [0] Left-most hands frame
             * [1] Leftish hands frame
             * [2] Leftish hands frame
             * [3] Centre hands frame
             * [4] Rightish hands frame
             * [5] Rightish hands frame
             * [6] Right-most hands frame */
            GetALineAndDontArgue(g, s);
#ifdef CARPOCALYPSE2_FIX_BUGS
            /* LoadPixelmap may clobber glibc's static strtok state, so finish
             * token extraction for the whole line before loading the image. */
            {
                int rhands_x = 0, rhands_y = 0;
                const char* image_name;
                str = strtok(s, "\t ,/");
                sscanf(str, "%d", &pCar_spec->lhands_x[i]);
                str = strtok(NULL, "\t ,/");
                sscanf(str, "%d", &pCar_spec->lhands_y[i]);
                str = strtok(NULL, "\t ,/");
                image_name = str;
                str = strtok(NULL, "\t ,/");
                sscanf(str, "%d", &rhands_x);
                str = strtok(NULL, "\t ,/");
                sscanf(str, "%d", &rhands_y);
                pCar_spec->lhands_images[i] = LoadPixelmap(image_name);
                pCar_spec->rhands_x[i] = rhands_x;
                pCar_spec->rhands_y[i] = rhands_y;
            }
#else
            str = strtok(s, "\t ,/");
            sscanf(str, "%d", &pCar_spec->lhands_x[i]);
            str = strtok(NULL, "\t ,/");
            sscanf(str, "%d", &pCar_spec->lhands_y[i]);
            str = strtok(NULL, "\t ,/");
            pCar_spec->lhands_images[i] = LoadPixelmap(str);
            str = strtok(NULL, "\t ,/");
            sscanf(str, "%d", &pCar_spec->rhands_x[i]);
            str = strtok(NULL, "\t ,/");
            sscanf(str, "%d", &pCar_spec->rhands_y[i]);
            str = strtok(NULL, "\t ,/");
#endif
            PossibleService();
        }
        pCar_spec->red_line = 8000;

        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, driver_x_offset, 0x47c);

        /* Offset of driver's head in 3D space */
        GetALineAndDontArgue(f, s);
        str = strtok(s, "\t ,/");
        sscanf(str, "%f", &pCar_spec->driver_x_offset);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%f", &pCar_spec->driver_y_offset);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%f", &pCar_spec->driver_z_offset);

        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, head_left_angle, 0x498);

        /* Angles to turn to make head go left and right */
        GetALineAndDontArgue(f, s);
        str = strtok(s, "\t ,/");
        sscanf(str, "%f", &pCar_spec->head_left_angle);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%f", &pCar_spec->head_right_angle);

        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, mirror_x_offset, 0x488);

        /* Offset of 'mirror camera' in 3D space, viewing angle of mirror */
        GetALineAndDontArgue(f, s);
        str = strtok(s, "\t ,/");
        sscanf(str, "%f", &pCar_spec->mirror_x_offset);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%f", &pCar_spec->mirror_y_offset);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%f", &pCar_spec->mirror_z_offset);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%f", &pCar_spec->rearview_camera_angle);

        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, mirror_left, 0x20c);

        /* Left, top, right, bottom of mirror */
        GetALineAndDontArgue(g, s);
        str = strtok(s, "\t ,/");
        sscanf(str, "%d", &pCar_spec->mirror_left);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->mirror_top);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->mirror_right);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->mirror_bottom);

        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, prat_left, 0x21c);

        /* Left, top, right, bottom of pratcam (*** relative to screen, not cockpit) */
        GetALineAndDontArgue(g, s);
        str = strtok(s, "\t ,/");
        sscanf(str, "%d", &pCar_spec->prat_left);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->prat_top);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->prat_right);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->prat_bottom);

        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, prat_cam_left, 0x144);

        /* Pratcam border names (left, top, right, bottom) */
        GetALineAndDontArgue(f, s);
        PossibleService();
#ifdef CARPOCALYPSE2_FIX_BUGS
        /* Each LoadPixelmap may clobber glibc's static strtok state, which would
         * break the strtok(NULL) calls that fetch the following border names.
         * Extract all four names first, then load the images. */
        {
            const char* cam_left;
            const char* cam_top;
            const char* cam_right;
            const char* cam_bottom;
            str = strtok(s, "\t ,/");
            cam_left = str;
            str = strtok(NULL, "\t ,/");
            cam_top = str;
            str = strtok(NULL, "\t ,/");
            cam_right = str;
            str = strtok(NULL, "\t ,/");
            cam_bottom = str;
            pCar_spec->prat_cam_left = LoadPixelmap(cam_left);
            pCar_spec->prat_cam_top = LoadPixelmap(cam_top);
            pCar_spec->prat_cam_right = LoadPixelmap(cam_right);
            pCar_spec->prat_cam_bottom = LoadPixelmap(cam_bottom);
        }
#else
        str = strtok(s, "\t ,/");
        pCar_spec->prat_cam_left = LoadPixelmap(str);
        str = strtok(NULL, "\t ,/");
        pCar_spec->prat_cam_top = LoadPixelmap(str);
        str = strtok(NULL, "\t ,/");
        pCar_spec->prat_cam_right = LoadPixelmap(str);
        str = strtok(NULL, "\t ,/");
        pCar_spec->prat_cam_bottom = LoadPixelmap(str);
#endif
        PossibleService();

        for (i = 0; i < CARPOCALYPSE2_ASIZE(pCar_spec->damage_units); i++) {
            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, damage_units, 0x4d8);
            if (i == eDamage_driver) {
                pCar_spec->damage_units[i].images = NULL;
            } else {
                /* [ 0] Engine damage x,y,flash1..5
                 * [ 1] Transmission damage x,y,flash1..5
                 * [ 3] Steering damage x,y,flash1..5
                 * [ 4] lf brake damage x,y,flash1..5
                 * [ 5] rf brake damage x,y,flash1..5
                 * [ 6] lr brake damage x,y,flash1..5
                 * [ 7] rr brake damage x,y,flash1..5
                 * [ 8] lf wheel damage x,y,flash1..5
                 * [ 9] rf wheel damage x,y,flash1..5
                 * [10] lr wheel damage x,y,flash1..5
                 * [11] rr wheel damage x,y,flash1..5 */
                GetALineAndDontArgue(g, s);
                str = strtok(s, "\t ,/");
                sscanf(str, "%d", &pCar_spec->damage_units[i].x_coord);
                str = strtok(NULL, "\t ,/");
                sscanf(str, "%d", &pCar_spec->damage_units[i].y_coord);
                for (k = 0; k < CARPOCALYPSE2_ASIZE(pCar_spec->damage_units[i].periods); k++) {
                    str = strtok(NULL, "\t ,/");
                    sscanf(str, "%f", &temp_float);
                    pCar_spec->damage_units[i].periods[k] = (int)(1000.0f / temp_float / 2.0f);
                }
                str = strtok(NULL, "\t ,/");
                pCar_spec->damage_units[i].images = LoadPixelmap(str);
                if (pCar_spec->damage_units[i].images == NULL) {
                    FatalError(kFatalError_CannotLoadDamageImage);
                }
            }
            pCar_spec->damage_units[i].damage_level = 0;
            PossibleService();
        }
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, damage_x_offset, 0x368);

        GetALineAndDontArgue(g, s);
        str = strtok(s, "\t ,/");
        sscanf(str, "%d", &pCar_spec->damage_x_offset);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->damage_y_offset);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->damage_background_x);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->damage_background_y);
        str = strtok(NULL, "\t ,/");
        pCar_spec->damage_background = LoadPixelmap(str);

        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, dim_count, 0x378);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, dim_right, 0x3c0);

        /* Number of dimmed areas (external) */
        pCar_spec->dim_count[0] = GetAnInt(g);
        for (i = 0; i < CARPOCALYPSE2_ASIZE(pCar_spec->dim_count); i++) {
            for (j = 0; j < pCar_spec->dim_count[i]; j++) {
                GetFourInts(g,
                    &pCar_spec->dim_left[i][j],
                    &pCar_spec->dim_top[i][j],
                    &pCar_spec->dim_right[i][j],
                    &pCar_spec->dim_bottom[i][j]);
            }
        }

        /* Number of dimmed areas (internal) */
        /* ignored */

        PathCat(the_path,
            gApplication_path,
            gGraf_specs[gGraf_spec_index].data_dir_name);
        PathCat(the_path, the_path, "HEADUP.TXT");
        h = DRfopen(the_path, "rt");
        if (h == NULL) {
            FatalError(kFatalError_CannotOpenHeadupsFile);
        }
        PossibleService();
        LoadHeadups(h, 0, pCar_spec);
        LoadHeadups(h, 1, pCar_spec);
        PossibleService();
        PFfclose(h);

        AdjustCarCoordinates(&gProgram_state.current_car);
        AdjustRenderScreenSize();
        PossibleService();
        ReinitialiseRearviewCamera();
        GetALineAndDontArgue(f, s);
        if (twt >= 0) {
            PackFileRerevertTiffLoading();
        }
        C2_HOOK_ASSERT(strcmp(s, "END OF DRIVABLE STUFF") == 0);
    } else {
        while (!PFfeof(f)) {
            GetALineAndDontArgue(f, s);
            if (strcmp(s, "END OF DRIVABLE STUFF") == 0) {
                break;
            }
        }
        pCar_spec->red_line = 8000;
    }
    PossibleService();

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, engine_noises, 0x470);

    /* Engine noise (normal, enclosed space, underwater) */
    GetThreeInts(f,
        &pCar_spec->engine_noises[0],
        &pCar_spec->engine_noises[1],
        &pCar_spec->engine_noises[2]);

    /* Can be stolen */
    GetAString(f, s);
    pCar_spec->can_be_stolen = strcmp(s, "stealworthy") == 0;

    /* Damage info for top impacts */
    GetDamageProgram(f, pCar_spec, eImpact_top);
    /* Damage info for bottom impacts */
    GetDamageProgram(f, pCar_spec, eImpact_bottom);
    /* Damage info for left impacts */
    GetDamageProgram(f, pCar_spec, eImpact_left);
    /* Damage info for right impacts */
    GetDamageProgram(f, pCar_spec, eImpact_right);
    /* Damage info for front impacts */
    GetDamageProgram(f, pCar_spec, eImpact_front);
    /* Damage info for rear impacts */
    GetDamageProgram(f, pCar_spec, eImpact_back);

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, grid_icon_names, 0x10c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, grid_icon_image, 0x1a4);

    /* Grid image (opponent, frank, annie */
    GetALineAndDontArgue(f, s);
    str = strtok(s, "\t ,/");
    strcpy(pCar_spec->grid_icon_names[0], str);
    str = strtok(NULL, "\t ,/");
    strcpy(pCar_spec->grid_icon_names[1], str);
    str = strtok(NULL, "\t ,/");
    strcpy(pCar_spec->grid_icon_names[2], str);
    pCar_spec->grid_icon_image = NULL;

    LoadAllImagesInDirectory(pStorage_space, car_path);
    LoadAllShadeTablesInDirectory(pStorage_space, car_path);
    LoadCarMaterials(pStorage_space, car_path, pCar_spec);
    old_model_count = pStorage_space->models_count;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, count_detail_levels, 0xe14);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, detail_levels, 0xe1c);

    /* Number of extra levels of detail */
    GetALineAndDontArgue(f, s);
    str = strtok(s, "\t ,/");
    sscanf(str, "%d", &pCar_spec->count_detail_levels);
    pCar_spec->count_detail_levels++;
    if (pCar_spec->count_detail_levels > CARPOCALYPSE2_ASIZE(((tUser_crush_data*)0)->models)) {
        PDFatalError("Too many levels of detail");
    }
    pCar_spec->detail_levels[0] = 0.f;
    for (i = 1; i < pCar_spec->count_detail_levels; i++) {
        /* min_dist_squared */
        pCar_spec->detail_levels[i] = GetAScalar(f);
    }
    LoadAllModelsInDirectory(pStorage_space, car_path);
    if (version > 0) {
        PossibleService();

        /* crush data file (will be incorporated into this file) (.WAM filename) */
        GetALineAndDontArgue(f, s);
        str = strtok(s, "\t ,/");
        PathCat(the_path, car_path, str);
        if (LoadCarCrush(&car_crush_buffer, the_path, pStorage_space, &pCar_spec->car_crush_spec)) {
            PathCat(the_path, car_path, pCar_name);
            strcpy(&the_path[strlen(the_path) - 3], "WAM");
            LoadCarCrush(&car_crush_buffer, the_path, pStorage_space, &pCar_spec->car_crush_spec);
        }
    }

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, car_master_actor, 0x10);

    pCar_spec->car_master_actor = BrActorAllocate(BR_ACTOR_NONE, NULL);
    BrActorAdd(gNon_track_actor, pCar_spec->car_master_actor);
    if (pDriver == eDriver_local_human) {
        gPlayer_car_master_actor = pCar_spec->car_master_actor;
    }

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, car_model_actor, 0xe0c);

    PathCat(actor_path, car_path, load_name);
    strcat(actor_path, ".ACT");
    pCar_spec->car_model_actor = BrActorLoad(actor_path);
    if (pCar_spec->car_model_actor == NULL) {
        FatalError(kFatalError_CannotLoadCarActor);
    }
    BrActorAdd(pCar_spec->car_master_actor, pCar_spec->car_model_actor);

    DRActorEnumRecurse(pCar_spec->car_model_actor, LinkCrushData, NULL);

    model_pool.models = &pStorage_space->models[old_model_count];
    model_pool.model_count = pStorage_space->models_count - old_model_count;
    model_pool.count_detail_levels = pCar_spec->count_detail_levels;
    DRActorEnumRecurse(pCar_spec->car_model_actor, LinkCrushModel, &model_pool);

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, shell_model, 0xe28);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, car_actor, 0xe10);

    model = NULL;
    for (i = old_model_count; i < pStorage_space->models_count; i++) {
        if (strcmp(pStorage_space->models[i]->identifier, "SHELL") == 0) {
            model = pStorage_space->models[i];
            break;
        }
    }
    pCar_spec->shell_model = model;
    pCar_spec->car_actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    user_crush = pCar_spec->car_model_actor->user;
    if (user_crush == NULL || user_crush->models[0] == NULL) {
        PDFatalError("Can't find main car model");
    }
    pCar_spec->car_actor->model = user_crush->models[0];
    BrMatrix34Copy(&pCar_spec->car_actor->t.t.mat, &pCar_spec->car_model_actor->t.t.mat);
    if (version > 0) {
        LinkCarCrushDatas(pCar_spec->car_model_actor, &car_crush_buffer);
    }
    DRActorEnumRecurse(pCar_spec->car_model_actor, AllocateUserDetailLevel, pCar_spec);
    PossibleService();
    for (i = old_model_count; i < pStorage_space->models_count; i++) {
        SetModelFlags(pStorage_space->models[i], pOwner);
    }

    /* Name of reflective screen material (or none if non-reflective) */
    GetAString(f, s);
    if (version > 1) {
        /* Percentage transparency of windscreen */
        GetAnInt(f);
    }

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, number_of_steerable_wheels, 0x404);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, steering_ref, 0x408);

    /* Number of steerable wheels */
    GetALineAndDontArgue(f, s);
    str = strtok(s, "\t ,/");
    sscanf(str, "%d", &pCar_spec->number_of_steerable_wheels);
    for (i = 0; i < pCar_spec->number_of_steerable_wheels; i++) {
        /* GroovyFunkRef of x'th steerable wheel */
        GetALineAndDontArgue(f, s);
        str = strtok(s, "\t ,/");
        sscanf(str, "%d", &pCar_spec->steering_ref[i]);
        AddRefOffset(&pCar_spec->steering_ref[i]);
    }

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, lf_sus_ref, 0x420);

    /* Left-front suspension parts GroovyFunkRef */
    GetALineAndDontArgue(f, s);
    str = strtok(s, "\t ,/");
    for (i = 0; i < CARPOCALYPSE2_ASIZE(pCar_spec->lf_sus_ref); i++) {
        sscanf(str, "%d", &pCar_spec->lf_sus_ref[i]);
        AddRefOffset(&pCar_spec->lf_sus_ref[i]);
        str = strtok(NULL, "\t ,/");
    }
    PossibleService();

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, rf_sus_ref, 0x430);

    /* Right-front suspension parts GroovyFunkRef */
    GetALineAndDontArgue(f, s);
    str = strtok(s, "\t ,/");
    for (i = 0; i < CARPOCALYPSE2_ASIZE(pCar_spec->rf_sus_ref); i++) {
        sscanf(str, "%d", &pCar_spec->rf_sus_ref[i]);
        AddRefOffset(&pCar_spec->rf_sus_ref[i]);
        str = strtok(NULL, "\t ,/");
    }

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, lr_sus_ref, 0x440);

    /* Left-rear suspension parts GroovyFunkRef */
    GetALineAndDontArgue(f, s);
    str = strtok(s, "\t ,/");
    for (i = 0; i < CARPOCALYPSE2_ASIZE(pCar_spec->lr_sus_ref); i++) {
        sscanf(str, "%d", &pCar_spec->lr_sus_ref[i]);
        AddRefOffset(&pCar_spec->lr_sus_ref[i]);
        str = strtok(NULL, "\t ,/");
    }

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, rr_sus_ref, 0x448);

    /* Right-rear suspension parts GroovyFunkRef */
    GetALineAndDontArgue(f, s);
    str = strtok(s, "\t ,/");
    for (i = 0; i < CARPOCALYPSE2_ASIZE(pCar_spec->rr_sus_ref); i++) {
        sscanf(str, "%d", &pCar_spec->rr_sus_ref[i]);
        AddRefOffset(&pCar_spec->rr_sus_ref[i]);
        str = strtok(NULL, "\t ,/");
    }

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, driven_wheels_spin_ref_1, 0x450);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, driven_wheels_spin_ref_2, 0x454);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, driven_wheels_spin_ref_3, 0x458);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, driven_wheels_spin_ref_4, 0x45c);

    /* Driven wheels GroovyFunkRefs (for spinning) - MUST BE 4 ITEMS */
    GetFourInts(f,
        &pCar_spec->driven_wheels_spin_ref_1,
        &pCar_spec->driven_wheels_spin_ref_2,
        &pCar_spec->driven_wheels_spin_ref_3,
        &pCar_spec->driven_wheels_spin_ref_4);
    AddRefOffset(&pCar_spec->driven_wheels_spin_ref_1);
    AddRefOffset(&pCar_spec->driven_wheels_spin_ref_2);
    AddRefOffset(&pCar_spec->driven_wheels_spin_ref_3);
    AddRefOffset(&pCar_spec->driven_wheels_spin_ref_4);
    PossibleService();

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, non_driven_wheels_spin_ref_1, 0x460);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, non_driven_wheels_spin_ref_2, 0x464);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, non_driven_wheels_spin_ref_3, 0x468);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, non_driven_wheels_spin_ref_4, 0x46c);

    /* Non-driven wheels GroovyFunkRefs (for spinning) - MUST BE 4 ITEMS */
    GetFourInts(f,
        &pCar_spec->non_driven_wheels_spin_ref_1,
        &pCar_spec->non_driven_wheels_spin_ref_2,
        &pCar_spec->non_driven_wheels_spin_ref_3,
        &pCar_spec->non_driven_wheels_spin_ref_4);
    AddRefOffset(&pCar_spec->non_driven_wheels_spin_ref_1);
    AddRefOffset(&pCar_spec->non_driven_wheels_spin_ref_2);
    AddRefOffset(&pCar_spec->non_driven_wheels_spin_ref_3);
    AddRefOffset(&pCar_spec->non_driven_wheels_spin_ref_4);

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, driven_wheels_circum, 0x4b8);

    /* Driven wheels diameter */
    GetALineAndDontArgue(f, s);
    str = strtok(s, "\t ,/");
    sscanf(str, "%f", &temp_float);
    pCar_spec->driven_wheels_circum = (float)(2.f * temp_float * CARPOCALYPSE2_PI);

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, non_driven_wheels_circum, 0x4bc);

    /* Non-driven wheels diameter */
    GetALineAndDontArgue(f, s);
    str = strtok(s, "\t ,/");
    sscanf(str, "%f", &temp_float);
    pCar_spec->non_driven_wheels_circum = (float)(temp_float * 2.f * CARPOCALYPSE2_PI);

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, car_model_variable, 0x400);

    pCar_spec->car_model_variable = pDriver != eDriver_local_human;
    PossibleService();

    /* START OF FUNK */
    GetALineAndDontArgue(f, s);
    AddFunkotronics(f, pOwner, gGroove_funk_offset, &car_crush_buffer);
    PossibleService();

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, wheel_actors, 0x12ec);

    for (i = 0; i < CARPOCALYPSE2_ASIZE(gWheel_actor_names); i++) {
        pCar_spec->wheel_actors[i] = DRActorFindRecurse(pCar_spec->car_master_actor, gWheel_actor_names[i]);
    }

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, pivot_actors, 0x1304);

    for (i = 0; i < CARPOCALYPSE2_ASIZE(gPivot_actor_names); i++) {
        pCar_spec->pivot_actors[i] = DRActorFindRecurse(pCar_spec->car_master_actor, gPivot_actor_names[i]);
    }

    for (i = 0; i < CARPOCALYPSE2_ASIZE(pCar_spec->wheel_actors); i++) {
        br_vector3 avg;
        br_actor* actor;
        br_model* model;

        actor = pCar_spec->wheel_actors[i];
        if (actor == NULL) {
            continue;
        }
        model = actor->model;
        if (model == NULL) {
            continue;
        }
        Vector3Average(&avg, &actor->model->bounds.min, &actor->model->bounds.max);
        if (!Vector3IsZero(&avg)) {
            BrVector3Accumulate(&actor->t.t.translate.t, &avg);
            for (j = 0; j < actor->model->nvertices; j++) {
                BrVector3Sub(&actor->model->vertices[j].p, &actor->model->vertices[j].p, &avg);
            }
            BrModelUpdate(actor->model, BR_MODU_VERTEX_POSITIONS);
        }
    }

    for (i = 0; i < CARPOCALYPSE2_ASIZE(pCar_spec->pivot_actors); i++) {
        br_actor* actor;

        actor = pCar_spec->pivot_actors[i];
        if (actor == NULL || actor->children == NULL) {
            continue;
        }
        BrVector3Accumulate(&actor->t.t.translate.t, &actor->children->t.t.translate.t);
        BrVector3Set(&actor->children->t.t.translate.t, 0.f, 0.f, 0.f);
    }

    /* START OF GROOVE */
    GetALineAndDontArgue(f, s);
    AddGroovidelics(f, pOwner, pCar_spec->car_master_actor, gGroove_funk_offset, 1);

    ReadMechanics(f, pCar_spec, version);

    PossibleService();

    /* Materials for shrapnel */
    LoadCarShrapnelMaterials(f, pCar_spec);

    count_vertices = 0;
    v11 = pCar_spec->car_actor->model->prepared;
    for (i = 0; i < v11->ngroups; i++) {
        count_vertices += v11->groups[i].nvertices;
    }

    all_fire_zero = 1;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, fire_vertex, 0xac);
    C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(pCar_spec->fire_vertex) != 12);

    /* damage vertices fire points */
    for (i = 0; i < CARPOCALYPSE2_ASIZE(pCar_spec->fire_vertex); i++) {
        int vertex_i;

        pCar_spec->fire_vertex[i] = GetAnInt(f);
        if (pCar_spec->fire_vertex[i] >= count_vertices) {
            pCar_spec->fire_vertex[i] = 0;
        }
        if (pCar_spec->fire_vertex[i] != 0) {
            all_fire_zero = 0;
        }
        vertex_i = 0;
        for (j = 0; j < v11->ngroups; j++) {
            for (k = 0; k < v11->groups[j].nvertices; k++) {
                if (v11->groups[j].vertex_user[k] == pCar_spec->fire_vertex[i]) {
                    pCar_spec->fire_vertex[i] = vertex_i;
                    j = v11->ngroups;
                    break;
                }
                vertex_i++;
            }
        }
    }

    if (all_fire_zero) {
        for (i = 0; i < CARPOCALYPSE2_ASIZE(pCar_spec->fire_vertex); i++) {
            pCar_spec->fire_vertex[i] = IRandomBetween(0, count_vertices - 1);
        }
    }

    while (!PFfeof(f)) {
        GetAString(f, s);

        if (DRStricmp(s, "END") == 0) {
            break;
        } else if (DRStricmp(s, "CAMERA_POSITIONS") == 0) {

            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, camera_bumper_position, 0x18d8);
            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, camera_cockpit_position, 0x18e4);

            /* bumper position */
            GetAVector(f, &pCar_spec->camera_bumper_position);

            /* cockpit position */
            GetAVector(f, &pCar_spec->camera_cockpit_position);
        } else if (DRStricmp(s, "CAMERA_TURN_OFF_MATERIALS") == 0) {

            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, count_window_materials, 0x18f0);
            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, window_materials, 0x18f4);

            /* Count */
            pCar_spec->count_window_materials = GetAnInt(f);
            for (i = 0; i < pCar_spec->count_window_materials; i++) {
                GetAString(f, s);
                pCar_spec->window_materials[i].material = BrMaterialFind(s);
                pCar_spec->window_materials[i].count_maps = GetAnInt(f);
                for (j = 0; j < pCar_spec->window_materials[i].count_maps; j++) {
                    GetAString(f, s);
                    pCar_spec->window_materials[i].maps[j] = BrMapFind(s);
                }
            }
        }
    }

    PFfclose(f);
    PFfclose(g);
    gCurrent_car_spec = NULL;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, field_0x18c8, 0x18c8);

    pCar_spec->field_0x18c8 = 0;
    DRActorEnumRecurse(pCar_spec->car_model_actor, AttachGroovidelic, NULL);
    AttachCrushDataToActorModels(pCar_spec->car_model_actor, pCar_spec);
    InitPhysModCrushData(pCar_spec);
    ClosePackFileAndSetTiffLoading(twt);
    car_material = BrMaterialAllocate("PorterIsAWxnkxr");
    if (AddMaterialToStorage(pStorage_space, car_material) == eStorage_not_enough_room) {
        FatalError(kFatalError_InsufficientMaterialSlots);
    }
    BrMaterialAdd(car_material);
    pCar_spec->car_master_actor->material = car_material;
    AdaptCachedMaterials(kRendererShadingType_Specular);
    for (i = pCar_spec->old_material_count; i < pCar_spec->new_material_count; i++) {
        br_material* material;

        material = pStorage_space->materials[i];
        if (material->identifier != NULL && material->identifier[0] != '\0') {
            material->identifier[0] = (material->flags & BR_MATF_TWO_SIDED) ? 'D' : 'S';
            BrMaterialUpdate(material, BR_MATU_ALL);
        }
    }
    gStorage_for_callbacks = prev_storage;
}

// FUNCTION: CARMA2_HW 0x0048cda0
void C2_HOOK_FASTCALL LoadOpponentsCars(tRace_info* pRace_info) {
    int i;
    int j;
    tOpp_spec* opponent;
    int sod_counter;
    char buffer[24];
    const char* driver_name;

    if (gNet_mode == eNet_mode_none) {
        AboutToLoadFirstCar();
        PrintMemoryDump(0, "JUST BEFORE LOADING YOUR CAR");
        LoadCar(gOpponents[gProgram_state.current_car_index].car_file_name,
            eDriver_local_human,
            &gProgram_state.current_car,
            0,
            gProgram_state.player_name,
            &gOur_car_storage_space);
        SetCarStorageTexturingLevel(&gOur_car_storage_space, GetCarTexturingLevel(), eCTL_full);
        PrintMemoryDump(0, "IMMEDIATELY AFTER LOADING YOUR CAR");
    }

    gGroove_funk_offset = GROOVE_FUNK_MAX_PER_CAR;
    gCount_opponents = 0;
    if (gNet_mode == eNet_mode_none) {
        sod_counter = 0;
        for (i = 0; i < pRace_info->number_of_racers; i++) {
            opponent = &pRace_info->opponent_list[i];
            PossibleService();
            if (opponent->index >= 0) {
                C2_HOOK_BUG_ON(sizeof(tCar_spec) != 6500);
                opponent->car_spec = BrMemAllocate(sizeof(tCar_spec), kMem_oppo_car_spec);
                if (DRStricmp("MAX DAMAGE", gOpponents[opponent->index].name) == 0) {
                    sod_counter++;
                    sprintf(buffer, "POOR SOD %d", sod_counter);
                    driver_name = buffer;
                } else {
                    driver_name = gOpponents[opponent->index].name;
                }
                LoadCar(gOpponents[opponent->index].car_file_name,
                    eDriver_oppo,
                    opponent->car_spec,
                    opponent->index,
                    driver_name,
                    &gTheir_cars_storage_space);
                PrintMemoryDump(0, "IMMEDIATELY AFTER LOADING AN OPPONENT");
            }
            if (gCurrent_race.race_spec->race_type == kRaceType_Cars) {
                if (gCurrent_race.race_spec->options.cars.count_opponents < 0) {
                    gCount_opponents++;
                    opponent->car_spec->is_race_goal = 1;
                } else {
                    for (j = 0; j < gCurrent_race.race_spec->options.cars.count_opponents; j++) {
                        if (opponent->index == gCurrent_race.race_spec->options.cars.opponents[j]) {
                            gCount_opponents++;
                            opponent->car_spec->is_race_goal = 1;
                            break;
                        }
                    }
                }
            }
        }
        SetCarStorageTexturingLevel(&gTheir_cars_storage_space, GetCarTexturingLevel(), eCTL_full);
    }
}

void C2_HOOK_FASTCALL SetModelFlags(br_model* pModel, int pOwner) {

    if (pModel != NULL && pModel->nfaces != 0) {
        if (pOwner == OPPONENT_APC_IDX || gAusterity_mode) {
            if (!(pModel->flags & BR_MODF_UPDATEABLE)) {
                return;
            }
            pModel->flags &= ~(BR_MODF_KEEP_ORIGINAL | BR_MODF_UPDATEABLE);
        } else {
            pModel->flags |= BR_MODF_DONT_WELD | BR_MODF_KEEP_ORIGINAL | BR_MODF_UPDATEABLE;
        }
        BrModelUpdate(pModel, BR_MODU_ALL);
    }
}

void C2_HOOK_FASTCALL AddRefOffset(int* pRef_holder) {

    if (*pRef_holder >= 0) {
        *pRef_holder += gGroove_funk_offset;
    }
}

void C2_HOOK_FASTCALL ReadMechanics(FILE* pF, tCar_spec* c, int pSpec_version) {
    int i;
    int j;
    char* str;
    char s[256];
    char* version_str;
    br_scalar red_gear_speed;
    br_scalar acceleration_highest_gear;
    float ixp, iyp, izp;
    int count_wheels;
    br_vector3 wpos;
    int steer_flags;
    br_scalar susp_give;
    br_scalar damping;
    br_scalar friction_slipping_reduction;
    br_scalar friction_angle_1, friction_angle_2;
    br_scalar traction_multiplier;
    br_scalar rolling_resistance;
    br_scalar friction_steer;
    br_scalar friction_non_steer_1, friction_non_steer_2;
    int wpos_i;

#if defined(CARPOCALYPSE2_FIX_BUGS)
    friction_non_steer_1 = 0.f;
    friction_non_steer_2 = 0.f;
    friction_steer = 0.f;
#endif

    if (pSpec_version == 0) {
        str = GetALineAndDontArgue(pF, s);
        if (str == NULL || strcmp(str, "START OF MECHANICS STUFF") != 0) {
            PDFatalError("Can't find old end of crush data comment");
        }
    } else {
        /* START OF MECHANICS STUFF version 1 */
        GetALineAndDontArgue(pF, s);
    }
    PossibleService();
    for (i = strlen(s) - 1; s[i] == ' '; i--) {
    }
    version_str = &s[i + 1];
    (void)version_str;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, maxcurve, 0x1258);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, initial_brake, 0x12ac);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, brake_increase, 0x12b0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, max_gear, 0x1364);

    /* Minimum turning circle. */
    c->maxcurve = 1.0f / GetAScalar(pF);

    /* Brake multiplier. */
    c->initial_brake = GetAScalar(pF);

    /* Braking strength multiplier. */
    c->brake_increase = GetAScalar(pF);

    /* Number of gears. */
    c->max_gear = GetAnInt(pF);

    /* Speed at red line in highest gear. */
    red_gear_speed = GetAScalar(pF);

    /* Acceleration in highest gear (m/s^2) i.e. engine strength. */
    acceleration_highest_gear = GetAScalar(pF);

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, collision_info, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, owner, 0x23c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, flags_0x238, 0x238);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, field_0x1a0, 0x1a0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, field_0x1a4, 0x1a4);

    C2_HOOK_BUG_ON(sizeof(tPhysics_object) != 0x4d8);
    c->collision_info = BrMemAllocate(sizeof(tPhysics_object), kMem_collision_object);
    c->collision_info->owner = c;
    c->collision_info->flags_0x238 = 1;
    c->collision_info->field_0x1a0 = 0xffff;
    c->collision_info->field_0x1a4 = 0;

    /* Sub member: Root part */
    /* Type */
    GetAString(pF, s);
    /* Identifier */
    GetAString(pF, s);
    /* Actor */
    GetAString(pF, s);

    c->collision_info->actor = c->car_master_actor;

    /* Sub member: Joint data */
    /* Type */
    GetAString(pF, s);

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, cmpos, 0x14);

    /* Centre of mass */
    GetThreeFloats(pF,
        &c->collision_info->cmpos.v[0],
        &c->collision_info->cmpos.v[1],
        &c->collision_info->cmpos.v[2]);

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, M, 0x4);

    /* Mass */
    c->collision_info->M = GetAScalar(pF);

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, I, 0x8);

    /* Angular momentum proportions */
    GetThreeFloats(pF, &ixp, &iyp, &izp);
    c->collision_info->I.v[0] = (izp * izp + iyp * iyp) * c->collision_info->M / 12.f;
    c->collision_info->I.v[1] = (ixp * ixp + izp * izp) * c->collision_info->M / 12.f;
    c->collision_info->I.v[2] = (iyp * iyp + ixp * ixp) * c->collision_info->M / 12.f;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, downforce_to_weight, 0x12a4);

    /* Downforce-to-weight balance speed */
    c->downforce_to_weight = GetAScalar(pF);

    /* Number of 'Wheels' entries. */
    count_wheels = GetAnInt(pF);

    if (count_wheels != 4) {
        FatalError(kFatalError_InvalidScreenDepthSetting);
    }

    C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(c->wpos) != 4);

    for (i = 0; i < CARPOCALYPSE2_ASIZE(c->wpos); i++) {
        /* Wheels entry #1 */
        /* Type */
        GetAnInt(pF);

        /* Identifier */
        GetAString(pF, s);

        /* Actor */
        GetAString(pF, s);

        /* Position */
        GetThreeFloats(pF, &wpos.v[0], &wpos.v[1], &wpos.v[2]);

        if (Vector3IsZero(&wpos)) {
            for (j = 0; j < CARPOCALYPSE2_ASIZE(c->wheel_actors); j++) {
                br_actor* parent;

                if (c->wheel_actors[j] == NULL) {
                    continue;
                }
                if (DRStricmp(s, c->wheel_actors[j]->identifier) != 0) {
                    continue;
                }

                BrVector3Copy(&wpos, &c->wheel_actors[j]->t.t.translate.t);
                parent = c->wheel_actors[j]->parent;
                while (parent != NULL && parent != c->car_master_actor) {
                    BrVector3Accumulate(&wpos, &parent->t.t.translate.t);
                    parent = parent->parent;
                }
                break;
            }
        }

        /* Steerable flags */
        steer_flags = GetAnInt(pF);

        /* Driven flags */
        GetAnInt(pF);

        /* Suspension give */
        susp_give = GetAScalar(pF);

        /* Damping factor */
        damping = GetAScalar(pF);

        /* Fractional reduction in friction when slipping */
        friction_slipping_reduction = GetAScalar(pF);

        /* Friction angles */
        GetPairOfFloats(pF, &friction_angle_1, &friction_angle_2);

        /* Traction fractional multiplier */
        traction_multiplier = GetAScalar(pF);

        /* Rolling resistance */
        rolling_resistance = GetAScalar(pF);

        if (steer_flags) {
            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, steerable_rolling_resistance, 0x12c8);
            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, steerable_suspension_give, 0x1210);

            c->steerable_rolling_resistance = rolling_resistance;
            c->steerable_suspension_give = susp_give;
            friction_steer = friction_angle_1;
            wpos_i = 2;
        } else {
            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, susp_give, 0x1214);
            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, rolling_resistance, 0x12cc);
            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, traction_multiplier, 0x12a0);

            c->susp_give = susp_give;
            c->rolling_resistance = rolling_resistance;
            c->traction_multiplier = traction_multiplier;
            friction_non_steer_1 = friction_angle_1;
            friction_non_steer_2 = friction_angle_2;
            wpos_i = 0;
        }
        if (wpos.v[0] > 0.f) {
            wpos_i += 1;
        }

        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, wpos, 0x1224);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, friction_slipping_reduction, 0x12b4);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, damping, 0x11fc);

        BrVector3Copy(&c->wpos[wpos_i], &wpos);
        c->friction_slipping_reduction = friction_slipping_reduction;
        c->damping = damping;
    }

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, object_friction, 0x64);

    c->collision_info->object_friction = 0.4f;

    ReadMechanicsShapes(&c->collision_info->shape, pF);

    UpdateCollisionObject(c->collision_info);

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, ride_height, 0x1220);

    c->maxcurve /= WORLD_SCALE;
    c->ride_height = (c->collision_info->bb1.min.v[1] + 0.01f) * WORLD_SCALE;

    c->initial_brake *= 12.f * c->collision_info->M;
    c->brake_increase *= 12.f * c->collision_info->M;

    BrVector3Scale(&c->centre_of_mass_world_scale, &c->collision_info->cmpos, WORLD_SCALE);

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, speed_revs_ratio, 0x1368);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, force_torque_ratio, 0x136c);

    c->speed_revs_ratio = red_gear_speed * 4.f / 9.f / c->max_gear / 6000.f;
    c->force_torque_ratio = c->collision_info->M * acceleration_highest_gear * c->max_gear;

    c->mu.v[0] = tanf(friction_non_steer_1 * (157.f / 9000.f)) / 4.f;
    c->mu.v[1] = tanf(friction_steer * (157.f / 9000.f)) / 4.f;
    c->mu.v[2] = tanf(friction_non_steer_2 * (157.f / 9000.f)) / 4.f;

    c->mu.v[0] *= sqrtf((c->wpos[2].v[2] - c->collision_info->cmpos.v[2]) / (c->wpos[2].v[2] - c->wpos[0].v[2]) * c->collision_info->M * 5.f);
    c->mu.v[1] *= sqrtf((c->wpos[0].v[2] - c->collision_info->cmpos.v[2]) / (c->wpos[0].v[2] - c->wpos[2].v[2]) * c->collision_info->M * 5.f);
    c->mu.v[2] *= sqrtf((c->wpos[2].v[2] - c->collision_info->cmpos.v[2]) / (c->wpos[2].v[2] - c->wpos[0].v[2]) * c->collision_info->M * 5.f);

    C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(c->wpos) != 4);

    for (i = 0; i < CARPOCALYPSE2_ASIZE(c->wpos); i++) {
        BrVector3Scale(&c->wpos[i], &c->wpos[i], WORLD_SCALE);
        c->wpos[i].v[1] = c->ride_height;
    }

    /* Number of sub-parts. */
    GetAnInt(pF);

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, bb1, 0x24);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, bb2, 0x3c);

    memcpy(&c->collision_info->bb2, &c->collision_info->bb1, sizeof(br_bounds3));

    /* END OF MECHANICS STUFF */
    GetAString(pF, s);
    C2_HOOK_ASSERT(strcmp(s, "END OF MECHANICS STUFF") != 0);
}

// FUNCTION: CARMA2_HW 0x00488dd0
br_material* C2_HOOK_FASTCALL GetSimpleMaterial(char* pName, tRendererShadingType pShading_type) {

    int i;
    br_colour colour;

    if (pName[0] == 'M') {
        strtok(pName, "\t ,/");
        for (i = 0; gSimple_material_colours[i] != 0; i++) {
            if (10 * (pName[1] - '0') + pName[2] - '0' == (gSimple_material_colours[i] >> 24)) {
                break;
            }
        }
        if (gSimple_material_colours[i] == 0) {
            PDFatalError("Obsolete simple material");
        }
        colour = gSimple_material_colours[i] & 0xffffff;
    } else {
        int r, g, b;
        sscanf(pName, "%d,%d,%d", &r, &g, &b);
        colour = (r << 16) | (g << 8) | (b << 0);
    }

    for (i = 0; i < CARPOCALYPSE2_ASIZE(gSimple_materials); i++) {
        if (gSimple_materials[i] == NULL) {
            break;
        }
        if (gSimple_materials[i]->colour == colour) {
            return gSimple_materials[i];
        }
    }
    if (i >= CARPOCALYPSE2_ASIZE(gSimple_materials)) {
        return NULL;
    }
    gSimple_materials[i] = BrMaterialAllocate("Simple");
    gSimple_materials[i]->colour = colour;
    gSimple_materials[i]->index_base = FindBestMatchingPaletteIndex(colour);
    gSimple_materials[i]->index_range = 0;
    GlorifyMaterial(&gSimple_materials[i], 1, pShading_type);
    BrMaterialAdd(gSimple_materials[i]);
    return gSimple_materials[i];
}

// FUNCTION: CARMA2_HW 0x0048fd30
void C2_HOOK_FASTCALL GetAVector(FILE* pF, br_vector3* pV) {
    char s[256];
    char* str;

    PossibleService();
    GetALineWithNoPossibleService(pF, s);
    str = strtok(s, "\t ,/");
    sscanf(str, "%f", &pV->v[0]);
    str = strtok(NULL, "\t ,/");
    sscanf(str, "%f", &pV->v[1]);
    str = strtok(NULL, "\t ,/");
    sscanf(str, "%f", &pV->v[2]);
}

// FUNCTION: CARMA2_HW 0x0048c130
int C2_HOOK_CDECL AttachGroovidelic(br_actor* pActor, void* pData) {
    tUser_crush_data* user_crush;

    user_crush = pActor->user;
    if (user_crush != NULL) {
        user_crush->groove = ActorsGroove(pActor);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0042a220
int C2_HOOK_CDECL ActorModelAttachCrushData(br_actor* pActor, void* pData) {
    tCar_spec* c;
    tUser_crush_data* user_crush;

    c = pData;
    (void)c;
    if (pActor->type != BR_ACTOR_MODEL) {
        return 0;
    }
    user_crush = pActor->user;
    if (user_crush == NULL || user_crush->crush_data == NULL || user_crush->groove == NULL) {
        return 0;
    }
    user_crush->crush_data = BrMemAllocate(sizeof(tCar_crush_buffer_entry), kMem_crush_data);
    strncpy(user_crush->crush_data->actor_name, pActor->identifier, sizeof(user_crush->crush_data->actor_name) - 1);
    user_crush->crush_data->id = 0xe9;
    user_crush->crush_data->softness_factor = 1.f;
    user_crush->crush_data->field_0x2c = 0;
    return 0;
}

// FUNCTION: CARMA2_HW 0x0042a210
void C2_HOOK_FASTCALL AttachCrushDataToActorModels(br_actor* pActor, tCar_spec* pCar_spec) {

    DRActorEnumRecurse(pActor, ActorModelAttachCrushData, pCar_spec);
}

// FUNCTION: CARMA2_HW 0x00486d60
void C2_HOOK_FASTCALL MungeLightingEffects(void) {

}

// FUNCTION: CARMA2_HW 0x00487ec0
void C2_HOOK_FASTCALL ReadNonCarMechanicsData(FILE* pF, tNon_car_spec* pNon_car_spec) {
    int index_version;
    int extra_point_num;
    int i;
    br_scalar len, wid, het;
    br_scalar snap_angle;
    br_scalar ts;
    br_vector3 attached_cmpos;

    C2_HOOK_BUG_ON(sizeof(tNon_car_spec) != 0x104);

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNon_car_spec, driver, 0xc);
    pNon_car_spec->driver = eDriver_non_car_unused_slot;

    /* Carmageddon 1: number of non car */
    /* Carmageddon 2: version number */
    index_version = GetAnInt(pF);
    if (index_version < 100) {
        br_vector3 attached_cmpos;
        br_bounds3 bounds;

        pNon_car_spec->index = index_version;

        /* centre of mass position */
        GetThreeFloats(pF,
            &pNon_car_spec->collision_info->cmpos.v[0],
            &pNon_car_spec->collision_info->cmpos.v[1],
            &pNon_car_spec->collision_info->cmpos.v[2]);

        /* centre of mass position when attached */
        GetThreeFloats(pF,
            &attached_cmpos.v[0],
            &attached_cmpos.v[1],
            &attached_cmpos.v[2]);

        /* min x, min y, min z */
        GetThreeFloats(pF, &bounds.min.v[0], &bounds.min.v[1], &bounds.min.v[2]);
        /* max x, max y, max z */
        GetThreeFloats(pF, &bounds.max.v[0], &bounds.max.v[1], &bounds.max.v[2]);

        /* number of extra points */
        extra_point_num = GetAnInt(pF);
        if (extra_point_num > 6) {
            FatalError(kFatalError_TooManyExtraPointsForCarIndex_S, index_version);
        }
        for (i = 0; i < extra_point_num; i++) {
            br_vector3 tmp;

            GetThreeFloats(pF, &tmp.v[0], &tmp.v[1], &tmp.v[2]);
        }

        /* mass in tonnes */
        GetPairOfFloats(pF, &pNon_car_spec->free_mass, &pNon_car_spec->attached_mass);

        /* am length, width, height */
        GetThreeFloats(pF, &len, &wid, &het);

        /* bend angle before snapping */
        snap_angle = GetAFloat(pF);
        pNon_car_spec->snap_off_cosine = cosf(BrAngleToRadian(BrDegreeToAngle(snap_angle)));
        pNon_car_spec->break_off_radians_squared = snap_angle * 3.14f / 180.f * (snap_angle * 3.14f / 180.f);

        /* torque (KN m) needed to move object */
        ts = GetAFloat(pF);
        pNon_car_spec->min_torque_squared = CARPOCALYPSE2_SQR(ts / WORLD_SCALE);

        BrVector3Set(&pNon_car_spec->I_over_M,
            (het * het + wid * wid) / 12.f,
            (het * het + len * len) / 12.f,
            (wid * wid + len * len) / 12.f);

        C2_HOOK_BUG_ON(sizeof(tCollision_shape_polyhedron) != 0x50);
        pNon_car_spec->collision_info->shape = BrMemAllocate(sizeof(tCollision_shape_polyhedron), kMem_collision_shape);
        memcpy(&pNon_car_spec->collision_info->shape->polyhedron.common.bb, &bounds, sizeof(br_bounds3));
        if (snap_angle != 0.f) {
            pNon_car_spec->field_0xf0 = pNon_car_spec->collision_info->physics_joint2 = AllocatePhysicsJoint(1, kMem_physics_joint);
            pNon_car_spec->collision_info->physics_joint2->type = eJoint_ball_n_socket;
            BrVector3Copy(&pNon_car_spec->collision_info->physics_joint2->field_0x08, &attached_cmpos);
            pNon_car_spec->collision_info->physics_joint2->count_limits = 1;
            pNon_car_spec->collision_info->physics_joint2->limits[0].type = eJoint_limit_10;
            pNon_car_spec->collision_info->physics_joint2->limits[0].value = pNon_car_spec->min_torque_squared;
        }
        UpdateCollisionObject(pNon_car_spec->collision_info);
        pNon_car_spec->collision_info->actor = pNon_car_spec->actor;
        pNon_car_spec->collision_info->bb2 = pNon_car_spec->collision_info->bb1;
    } else {
        /* non car number */
        pNon_car_spec->index = (int)GetAScalar(pF);

        /* centre of mass position */
        GetThreeFloats(pF,
            &pNon_car_spec->collision_info->cmpos.v[0],
            &pNon_car_spec->collision_info->cmpos.v[1],
            &pNon_car_spec->collision_info->cmpos.v[2]);

        /* centre of mass position when attached */
        GetThreeFloats(pF,
            &attached_cmpos.v[0],
            &attached_cmpos.v[1],
            &attached_cmpos.v[2]);

        ReadMechanicsShapes(&pNon_car_spec->collision_info->shape, pF);

        /* mass unattached, mass attached */
        GetPairOfFloats(pF, &pNon_car_spec->free_mass, &pNon_car_spec->attached_mass);

        /* am width height and length */
        GetThreeFloats(pF, &len, &wid, &het);

        /* bend angle before snapping */
        snap_angle = GetAFloat(pF);
        pNon_car_spec->snap_off_cosine = cosf(BrAngleToRadian(BrDegreeToAngle(snap_angle)));
        pNon_car_spec->break_off_radians_squared = snap_angle * 3.14f / 180.f * (snap_angle * 3.14f / 180.f);

        /* torque (KN m) needed to move object */
        ts = GetAFloat(pF);
        pNon_car_spec->min_torque_squared = CARPOCALYPSE2_SQR(ts / WORLD_SCALE);

        BrVector3Set(&pNon_car_spec->I_over_M,
            (het * het + wid * wid) / 12.f,
            (het * het + len * len) / 12.f,
            (wid * wid + len * len) / 12.f);

        if (snap_angle != 0.f) {
            pNon_car_spec->field_0xf0 = pNon_car_spec->collision_info->physics_joint2 = AllocatePhysicsJoint(1, kMem_physics_joint);
            pNon_car_spec->collision_info->physics_joint2->type = eJoint_ball_n_socket;
            BrVector3Copy(&pNon_car_spec->collision_info->physics_joint2->field_0x08, &attached_cmpos);
            pNon_car_spec->collision_info->physics_joint2->count_limits = 1;
            pNon_car_spec->collision_info->physics_joint2->limits[0].type = eJoint_limit_10;
            pNon_car_spec->collision_info->physics_joint2->limits[0].value = pNon_car_spec->min_torque_squared;
        }
        UpdateCollisionObject(pNon_car_spec->collision_info);
        pNon_car_spec->collision_info->actor = pNon_car_spec->actor;
        pNon_car_spec->collision_info->bb2 = pNon_car_spec->collision_info->bb1;

        /* Materials for shrapnel */
        pNon_car_spec->count_shrapnel_materials = GetAnInt(pF);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNon_car_spec, count_shrapnel_materials, 0x6c);

        for (i = 0; i < pNon_car_spec->count_shrapnel_materials; i++) {
            char s[256];

            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNon_car_spec, shrapnel_materials, 0x84);

            GetALineAndDontArgue(pF, s);
            pNon_car_spec->shrapnel_materials[i] = GetSimpleMaterial(s, (pNon_car_spec != NULL && pNon_car_spec->driver >= 6) ? kRendererShadingType_Specular : kRendererShadingType_Diffuse1);
        }
        pNon_car_spec->collision_info->object_friction = 0.4f;
        if (index_version > 100) {
            for (;;) {
                char s[256];

                GetAString(pF, s);
                if (DRStricmp(s, "END") == 0) {
                    break;
                } else if (DRStricmp(s, "WORLD_FRICTION") == 0) {
                    pNon_car_spec->collision_info->world_friction = GetAScalar(pF) - 1.f;
                    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, world_friction, 0x60);
                } else if (DRStricmp(s, "OBJECT_FRICTION") == 0) {
                    pNon_car_spec->collision_info->object_friction = GetAScalar(pF) - 1.f;
                    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, object_friction, 0x64);
                } else if (DRStricmp(s, "TUMBLE") == 0) {
                    pNon_car_spec->tumble_factor = GetAScalar(pF);
                    pNon_car_spec->tumble_threshold = GetAScalar(pF);
                    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNon_car_spec, tumble_factor, 0xdc);
                    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNon_car_spec, tumble_threshold, 0xe0);
                } else if (DRStricmp(s, "WORLD_BALL_JOINT") == 0) {
                    br_vector3 tv;
                    br_scalar friction;
                    int count_limits;

                    GetThreeFloats(pF, &tv.v[0], &tv.v[1], &tv.v[2]);

                    /* friction */
                    friction = GetAScalar(pF);

                    /* num limits */
                    count_limits = GetAnInt(pF);

                    pNon_car_spec->field_0xf0 = pNon_car_spec->collision_info->physics_joint2 = AllocatePhysicsJoint(count_limits, kMem_physics_joint);
                    pNon_car_spec->field_0xf0->friction = friction;
                    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint, friction, 0x4);
                    BrVector3Copy(&pNon_car_spec->field_0xf0->field_0x08, &tv);
                    pNon_car_spec->field_0xf0->type = eJoint_ball_n_socket;
                } else if (DRStricmp(s, "WORLD_HINGE_JOINT") == 0) {
                    br_vector3 tv1;
                    br_vector3 tv2;
                    br_scalar friction;
                    int count_limits;

                    GetThreeFloats(pF, &tv1.v[0], &tv1.v[1], &tv1.v[2]);
                    GetThreeFloats(pF, &tv2.v[0], &tv2.v[1], &tv2.v[2]);

                    /* friction */
                    friction = GetAScalar(pF);

                    /* num limits */
                    count_limits = GetAnInt(pF);

                    pNon_car_spec->field_0xf0 = pNon_car_spec->collision_info->physics_joint2 = AllocatePhysicsJoint(count_limits, kMem_physics_joint);
                    pNon_car_spec->field_0xf0->friction = friction;
                    BrVector3Copy(&pNon_car_spec->field_0xf0->field_0x08, &tv1);
                    pNon_car_spec->field_0xf0->type = eJoint_ball_n_socket;
                } else if (DRStricmp(s, "WORLD_TRANSLATION_JOINT") == 0) {
                    br_vector3 pos;
                    br_vector3 dir;
                    br_scalar limit;
                    /* joint position */
                    GetThreeFloats(pF, &pos.v[0], &pos.v[1], &pos.v[2]);

                    /* axis (direction of movement) */
                    GetThreeFloats(pF, &dir.v[0], &dir.v[1], &dir.v[2]);

                    /* lift height */
                    limit = GetAScalar(pF);

                    pNon_car_spec->field_0xf0 = pNon_car_spec->collision_info->physics_joint2 = AllocatePhysicsJoint(1, kMem_physics_joint);
                    BrVector3Copy(&pNon_car_spec->field_0xf0->field_0x08, &pos);
                    BrVector3Copy(&pNon_car_spec->field_0xf0->hinge_axis, &dir);

                    if (fabsf(dir.v[0]) <= fabsf(dir.v[1]) && fabsf(dir.v[0]) <= fabsf(dir.v[2])) {
                        BrVector3Set(&pNon_car_spec->field_0xf0->hinge_axis2, 0.f, dir.v[2], -dir.v[1]);
                    } else if (fabsf(dir.v[1]) <= fabsf(dir.v[2])) {
                        BrVector3Set(&pNon_car_spec->field_0xf0->hinge_axis2, -dir.v[2], 0.f, -dir.v[0]);
                    } else {
                        BrVector3Set(&pNon_car_spec->field_0xf0->hinge_axis2, dir.v[1], -dir.v[0], 0.f);
                    }
                    BrVector3Normalise(&pNon_car_spec->field_0xf0->hinge_axis2, &pNon_car_spec->field_0xf0->hinge_axis2);
                    BrVector3Cross(&pNon_car_spec->field_0xf0->hinge_axis3, &pNon_car_spec->field_0xf0->hinge_axis, &pNon_car_spec->field_0xf0->hinge_axis2);
                    pNon_car_spec->field_0xf0->type = eJoint_translation;
                    pNon_car_spec->flags |= 0x10000;
                    pNon_car_spec->field_0xf0->count_limits = 1;
                    pNon_car_spec->field_0xf0->limits[0].type = eJoint_limit_11;
                    pNon_car_spec->field_0xf0->limits[0].value = limit;

                    C2_HOOK_BUG_ON(sizeof(tJoint_translation_params) != 0x14);
                    pNon_car_spec->translation_parameters = BrMemAllocate(sizeof(tJoint_translation_params), kMem_non_car_spec);

                    /* forward accel in g */
                    pNon_car_spec->translation_parameters->forward_acceleration = GetAScalar(pF);

                    /* reverse accel in g (nb grvity will pull it down) */
                    pNon_car_spec->translation_parameters->reverse_acceleration = GetAScalar(pF);

                    /* resistance going up */
                    pNon_car_spec->translation_parameters->forward_resistance = GetAScalar(pF);

                    /* resistance going down */
                    pNon_car_spec->translation_parameters->reverse_resistance = GetAScalar(pF);

                    /* pause at the top */
                    pNon_car_spec->translation_parameters->pause_at_top = GetAnInt(pF);

                    pNon_car_spec->translation_parameters->forward_resistance = (float)exp(0.43 * (double)pNon_car_spec->translation_parameters->forward_resistance);
                    pNon_car_spec->translation_parameters->reverse_resistance = (float)exp(0.43 * (double)pNon_car_spec->translation_parameters->reverse_resistance);
                } else if (DRStricmp(s, "RISE_WHEN_HIT") == 0) {
                    pNon_car_spec->flags |= 0x20000;
                } else if (DRStricmp(s, "RISE_WHEN_DRIVEN_ON") == 0) {
                    pNon_car_spec->flags |= 0x40000;
                } else if (DRStricmp(s, "NUMBER_OF_PUSHES") == 0) {
                    pNon_car_spec->number_of_pushes = GetAnInt(pF);
                } else if (DRStricmp(s, "DRIVABLE_ON") == 0) {
                    pNon_car_spec->collision_info->drivable_on = 1;
                } else if (DRStricmp(s, "INFINITE_I") == 0) {
                    pNon_car_spec->collision_info->flags |= 0x800;
                } else if (DRStricmp(s, "IGNORE_WORLD_COLLISIONS") == 0) {
                    pNon_car_spec->collision_info->flags |= 0x80;
                } else if (DRStricmp(s, "BLOCK_CAMERA") == 0) {
                    pNon_car_spec->flags |= 0x80000;
                } else if (DRStricmp(s, "SPARKY") == 0) {
                    pNon_car_spec->flags |= 0x200000;
                } else {
                    FatalError(kFatalError_UnknownNonCarCommand);
                }
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x00450600
int C2_HOOK_CDECL LinkyCallback(br_actor* pActor, void* data) {
    tLinkyCallback_context* context = data;
    int i;

    if (pActor->type != BR_ACTOR_MODEL) {
        return 0;
    }
    pActor->render_style = BR_RSTYLE_DEFAULT;
    for (i = context->model_start; i < context->model_start + context->model_end; i++) {

        if (DRStricmp(gDroneStorage.models[i]->identifier, pActor->model->identifier) == 0) {
            pActor->model = gDroneStorage.models[i];
            return 0;
        }
    }
    PDFatalError("No drone model");
}

void C2_HOOK_FASTCALL LinkDroneActorsToModelsAndSetRenderStyle(tDrone_spec* pDrone, int pModel_start, int pModel_end) {
    tLinkyCallback_context data;

    data.drone = pDrone;
    data.model_start = pModel_start;
    data.model_end = pModel_end;
    DRActorEnumRecurse(pDrone->model_actor, LinkyCallback, &data);
}

// FUNCTION: CARMA2_HW 0x00450150
void C2_HOOK_FASTCALL LoadDroneActorsModels(tDrone_spec* pDrone) {
    br_matrix34 mat;
    tPath_name dir_path;
    tPath_name actor_path;
    char s[256];
    int i;

    PossibleService();
    BrMatrix34Identity(&mat);
    if (pDrone->model_actor != NULL) {
        if (pDrone->model_actor->parent != NULL) {
            BrActorRemove(pDrone->model_actor);
        }
        BrActorFree(pDrone->model_actor);
        pDrone->model_actor = NULL;
    }
    if (pDrone->actor != NULL) {
        BrMatrix34Copy(&mat, &pDrone->actor->t.t.mat);
        if (pDrone->actor->parent != NULL) {
            BrActorRemove(pDrone->actor);
        }
        BrActorFree(pDrone->actor);
        pDrone->actor = NULL;
    }
    strcpy(gCurrent_load_directory, "DRONES");
    strcpy(gCurrent_load_name, pDrone->form->name);
    strcpy(dir_path, gApplication_path);
    PathCat(dir_path, dir_path, gCurrent_load_directory);
    PathCat(dir_path, dir_path, gCurrent_load_name);

    if (pDrone->form->model_start < 0) {
        int material_start;

        PrintMemoryDump(0,"BEFORE LOADING DRONE ACTORS/MODELS");
        LoadAllImagesInDirectory(&gDroneStorage, dir_path);
        LoadAllShadeTablesInDirectory(&gDroneStorage, dir_path);
        material_start = gDroneStorage.materials_count;
        LoadAllMaterialsInDirectory(&gDroneStorage, dir_path, kRendererShadingType_Specular);
        for (i = material_start; i < gDroneStorage.materials_count; i++) {
            br_material* material;

            material = gDroneStorage.materials[i];
            if (material == NULL || material->colour_map == NULL) {
                continue;
            }
            material->flags |= BR_MATF_LIGHT | BR_MATF_PRELIT;
            material->flags |= BR_MATF_SMOOTH;
            BrMaterialUpdate(material, BR_MATU_ALL);
            PossibleService();
        }
        pDrone->form->model_start = gDroneStorage.models_count;
        LoadAllModelsInDirectory(&gDroneStorage, dir_path);
        pDrone->form->model_end = gDroneStorage.models_count - pDrone->form->model_start;
        if (pDrone->form->crushability != 0.f) {
            pDrone->form->model_index = -1;

            for (i = pDrone->form->model_start; i < pDrone->form->model_start + pDrone->form->model_end; i++) {
                if (DRStricmp(gDroneStorage.models[i]->identifier, pDrone->form->name) == 0) {
                    pDrone->form->model_index = i;
                    break;
                }
            }
            if (pDrone->form->model_index < 0) {
                sprintf(s, "Can't find model called '%s' for drone '%s'",
                    pDrone->form->name, pDrone->form->name);
                PDFatalError(s);
            }
        }
        PrintMemoryDump(0, "AFTER LOADING DRONE ACTORS/MODELS");
    }
    pDrone->actor = BrActorAllocate(BR_ACTOR_NONE, NULL);
    BrMatrix34Copy(&pDrone->actor->t.t.mat, &mat);
    sprintf(s, "Drone%d", pDrone->id);
    pDrone->actor->identifier = BrResStrDup(pDrone->actor, s);
    BrActorAdd(gNon_track_actor, pDrone->actor);
    PathCat(actor_path, dir_path, pDrone->form->name);
    strcat(actor_path,".ACT");
    pDrone->model_actor = BrActorLoad(actor_path);
    if (pDrone->model_actor == NULL) {
        sprintf(s, "Can't load drone car actor file for %s", pDrone->form->name);
        PDFatalError(s);
    }
    LinkDroneActorsToModelsAndSetRenderStyle(pDrone, pDrone->form->model_start, pDrone->form->model_end);
    pDrone->actor->render_style = BR_RSTYLE_NONE;
    BrActorAdd(pDrone->actor, pDrone->model_actor);
    if (!(pDrone->form->flags & 0x20)) {
        br_model* model = pDrone->model_actor->model;

        pDrone->form->field_0x54.min.v[0] = model->bounds.min.v[0] + pDrone->form->crush_limits_left    * 0.01f * (model->bounds.max.v[0] - model->bounds.min.v[0]);
        pDrone->form->field_0x54.min.v[1] = model->bounds.min.v[1] + pDrone->form->crush_limits_bottom  * 0.01f * (model->bounds.max.v[1] - model->bounds.min.v[1]);
        pDrone->form->field_0x54.min.v[2] = model->bounds.min.v[2] + pDrone->form->crush_limits_back    * 0.01f * (model->bounds.max.v[2] - model->bounds.min.v[2]);
        pDrone->form->field_0x54.max.v[0] = model->bounds.max.v[0] - pDrone->form->crush_limits_right   * 0.01f * (model->bounds.max.v[0] - model->bounds.min.v[0]);
        pDrone->form->field_0x54.max.v[1] = model->bounds.max.v[1] - pDrone->form->crush_limits_top     * 0.01f * (model->bounds.max.v[1] - model->bounds.min.v[1]);
        pDrone->form->field_0x54.max.v[2] = model->bounds.max.v[2] - pDrone->form->crush_limits_front   * 0.01f * (model->bounds.max.v[2] - model->bounds.min.v[2]);
    }
}

// FUNCTION: CARMA2_HW 0x00450b30
intptr_t C2_HOOK_CDECL MrFindy(br_actor* pActor, void* data) {
    const char* name = data;

    if (DRStricmp(pActor->identifier, name) == 0) {
        return (intptr_t)pActor;
    }
    return 0;
}

br_actor* C2_HOOK_FASTCALL FindDroneChildActor(tDrone_spec* pDrone, const char* pName) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tDrone_spec, model_actor, 0xf0);
    return (br_actor*)DRActorEnumRecurse(pDrone->model_actor, MrFindy, (void*)pName);
}

int C2_HOOK_FASTCALL ReadPastThisLine(FILE* pF, const char* pLine) {
    char s[256];

    for (;;) {
        if (PFfeof(pF)) {
            return 0;
        }
        GetALineAndDontArgue(pF, s);
        if (strcmp(pLine, s) == 0) {
            return 1;
        }
    }
}

int C2_HOOK_FASTCALL MatchFGType(const char* pS) {
    int i;

    C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(gGroove_funk_type_names) != 3);

    for (i = 0; i < CARPOCALYPSE2_ASIZE(gGroove_funk_type_names); i++) {

        if (strcmp(pS, gGroove_funk_type_names[i]) == 0) {
            return i;
        }
    }
    return -1;
}

tFunk_groove_axis GetAxisFromString(const char* pS) {
    int i;

    C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(gAxis_names) != 3);

    for (i = 0; i < CARPOCALYPSE2_ASIZE(gAxis_names); i++) {

        if (strcmp(pS, gAxis_names[i]) == 0) {
            return i;
        }
    }
    return -1;
}

tFunk_groove_reverseness GetReversenessFromString(const char* pS) {
    int i;

    C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(gReverseness_type_names) != 2);

    for (i = 0; i < CARPOCALYPSE2_ASIZE(gReverseness_type_names); i++) {

        if (strcmp(pS, gReverseness_type_names[i]) == 0) {
            return i;
        }
    }
    return -1;
}

tFunk_groove_speed_control GetSpeedControlFromString(const char* pS) {
    int i;

    C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(gFunk_speed_control_names) != 2);

    for (i = 0; i < CARPOCALYPSE2_ASIZE(gFunk_speed_control_names); i++) {

        if (strcmp(pS, gFunk_speed_control_names[i]) == 0) {
            return i;
        }
    }
    return -1;
}

// FUNCTION: CARMA2_HW 0x00450680
void C2_HOOK_FASTCALL LoadFunksAndGrooves(tDrone_spec* pDrone, FILE* pF) {

    C2_HOOK_BUG_ON(sizeof(tFunk_grooves) != 0xa4);
    C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(pDrone->funk_grooves->items) != 10);

    if (!ReadPastThisLine(pF, "START OF FUNKYGROOVY STUFF")) {
        FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, pDrone->form->name);
    }

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tDrone_spec, funk_grooves, 0x5d0);

    if (pDrone->funk_grooves == NULL) {
        pDrone->funk_grooves = BrMemAllocate(sizeof(tFunk_grooves), kMem_drone_funk_groove);
    }
    for (;;) {
        tFunk_groove* funk_groove;
        char s[256];
        int axis;
        int reverseness;
        int speed_control;

        GetALineAndDontArgue(pF, s);
        if (strcmp(s, "END OF FUNKYGROOVY STUFF") == 0 || PFfeof(pF)) {
            break;
        }
        if (pDrone->funk_grooves->count >= CARPOCALYPSE2_ASIZE(pDrone->funk_grooves->items)) {
            FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, pDrone->form->name);
        }
        funk_groove = &pDrone->funk_grooves->items[pDrone->funk_grooves->count];
        funk_groove->type = MatchFGType(s);
        switch (funk_groove->type) {
        case eFunk_groove_type_spinny_groove:
            /* wheel actor */
            GetALineAndDontArgue(pF, s);
            funk_groove->actor = FindDroneChildActor(pDrone, s);
            if (funk_groove->actor == NULL) {
                FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, pDrone->form->name);
            }

            /* "x", "y" or "z" */
            GetALineAndDontArgue(pF, s);
            axis = GetAxisFromString(s);
            if (axis < 0) {
                FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, pDrone->form->name);
            }
            funk_groove->spinny.axis = axis;

            /* "controlled" or "dronespeed" */
            GetALineAndDontArgue(pF, s);
            speed_control = GetSpeedControlFromString(s);
            if (speed_control < 0) {
                FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, pDrone->form->name);
            }
            funk_groove->spinny.speed_control = speed_control;

            /* "forward" or "reverse" */
            GetALineAndDontArgue(pF,s);
            reverseness = GetReversenessFromString(s);
            if (reverseness < 0) {
                FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, pDrone->form->name);
            }
            funk_groove->spinny.reverse = reverseness;

            funk_groove->spinny.omega = CARPOCALYPSE2_PI_F * GetAScalar(pF);
            break;

        case eFunk_groove_type_steering_groove:
            /* pivot actor */
            GetALineAndDontArgue(pF, s);
            funk_groove->actor = FindDroneChildActor(pDrone, s);
            if (funk_groove->actor == NULL) {
                FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, pDrone->form->name);
            }

            /* "forward" or "reverse" */
            GetALineAndDontArgue(pF, s);
            reverseness = GetReversenessFromString(s);
            if (reverseness < 0) {
                FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, pDrone->form->name);
            }
            funk_groove->steering.reverse = reverseness;
            break;
        }
        pDrone->funk_grooves->count += 1;
    }
}

int C2_HOOK_FASTCALL ReadPastBoundsShapesHeader(FILE* pF) {
    char s[256];

    do {
        if (PFfeof(pF)) {
            return 0;
        }
        GetALineAndDontArgue(pF, s);
    } while (strcmp(s, "START OF BOUNDING SHAPES") != 0);
    DoNotDprintf("ReadPastBoundsShapesHeader() returning TRUE");
    return 1;
}

void C2_HOOK_FASTCALL LoadDrone(int pIndex) {
    tDrone_spec* drone;
    tPath_name path;
    tTWTVFS twt;
    FILE* f;

#ifndef CARPOCALYPSE2_MATCHING
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tDrone_spec, collision_info.shape, 0x118);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tDrone_spec, field_0x46, 0x46);

    C2_HOOK_BUG_ON(sizeof(tDrone_spec) != 0x5d8);
#endif

    drone = &gDrone_specs[pIndex];
    strcpy(path, gApplication_path);
    PathCat(path, path, "DRONES");
    PathCat(path, path, drone->form->name);
    twt = OpenPackFileAndSetTiffLoading(path);
    LoadDroneActorsModels(drone);
    f = OpenDroneFile(drone->form->name);
    if (!ReadPastBoundsShapesHeader(f)) {
        FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, drone->form->name);
    } else {
        DoNotDprintf("DRONE.C: Reading mechanics data for drone of type %s", drone->form->name);
        ReadMechanicsShapes(&drone->collision_info.shape, f);
        if (drone->collision_info.shape != NULL) {
            drone->field_0x46 = 1;
        }
    }
    LoadFunksAndGrooves(drone, f);
    PFfclose(f);
    ClosePackFileAndSetTiffLoading(twt);
}

void C2_HOOK_FASTCALL ResetDroneCrushyModel(const br_model* pSrc, br_model* pDest) {

    pDest->faces = pSrc->faces;
    pDest->nfaces = pSrc->nfaces;
    memcpy(pDest->vertices, pSrc->vertices, pDest->nvertices * sizeof(br_vertex));
}

// FUNCTION: CARMA2_HW 0x0044fda0
void C2_HOOK_FASTCALL LoadPerRaceDroneStuff(void) {

    int i;

    for (i = 0; i < gCount_drone_forms; i++) {
        tDrone_form* form = &gDrone_forms[i];

        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tDrone_form, field_0x6c, 0x6c);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tDrone_form, model_start, 0x70);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tDrone_form, model_end, 0x74);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tDrone_form, model_index, 0x78);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tDrone_form, count_models, 0x7c);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tDrone_form, field_0x80, 0x80);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tDrone_form, models, 0x84);

        form->field_0x6c = 0;
        form->model_start = -1;
        form->model_end = -1;
        form->model_index = -1;
        form->count_models = 0;
        form->field_0x80 = NULL;
        form->models = NULL;
    }

    for (i = 0; i < gCount_drones; i++) {
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tDrone_spec, field_0x6c, 0x6c);

        gDrone_specs[i].field_0x6c += 1;
        LoadDrone(i);
    }

    for (i = 0; i < gCount_drone_forms; i++) {
        tDrone_form* form;
        int count_models;

        form = &gDrone_forms[i];
        if (form->field_0x6c == 0) {
            continue;
        }
        count_models = form->crushability == 0.f ? 0 : MIN(10, form->field_0x6c);
        form->count_models = count_models;
        if (count_models != 0) {
            int j;

            form->models = BrMemCalloc(sizeof(br_model*), count_models, kMem_drone_model_dup);
            form->field_0x80 = BrMemCalloc(count_models, sizeof(tU8), kMem_drone_model_dup);

            for (j = 0; j < count_models; j++) {
                br_model* orig_model;

                orig_model = gDroneStorage.models[form->model_index];
                form->models[j] = BrModelAllocate(orig_model->identifier, orig_model->nvertices, 0);
                form->models[j]->flags |= BR_MODF_UPDATEABLE;
                ResetDroneCrushyModel(orig_model, form->models[j]);
                BrModelAdd(form->models[j]);
            }

        }

    }
}

// FUNCTION: CARMA2_HW 0x00401070
void C2_HOOK_FASTCALL LoadAIWorldForRace(tRace_info* pRace_info) {

    PrintMemoryDump(0,"BEFORE LoadOpponentsCars()");
    LoadOpponentsCars(pRace_info);
    LoadCopCars();
    PrintMemoryDump(0,"AFTER LoadOpponentsCars(), BEFORE LoadPerRaceDroneStuff()");
    LoadPerRaceDroneStuff();
    PrintMemoryDump(0,"AFTER LoadPerRaceDroneStuff()");
}

// FUNCTION: CARMA2_HW 0x004010b0
void C2_HOOK_FASTCALL InitialiseAIWorldForRace(tRace_info* pRace_info) {
    br_bounds3 track_bounds;

    gFirst_drone_processing = 1;
    gTime_stamp_for_this_munging = GetTotalTime();
    if (gTime_stamp_for_this_munging == 0) {
        gTime_stamp_for_this_munging = PDGetTotalTime();
    }
    gFrame_period_for_this_munging = 100;
    gDrone_delta_time = 0.1f;
    BrActorToBounds(&track_bounds, gTrack_actor);
    if (track_bounds.max.v[1] <= track_bounds.min.v[1]) {
        gTrack_drone_min_y = track_bounds.max.v[1];
    } else {
        gTrack_drone_min_y = track_bounds.min.v[1];
    }
    PrintMemoryDump(0, "BEFORE InitOpponents()");
    InitOpponents(pRace_info);
    PrintMemoryDump(0, "AFTER InitOpponents(), BEFORE InitDrones()");
    InitDrones();
    PrintMemoryDump(0, "AFTER InitDrones()");
}