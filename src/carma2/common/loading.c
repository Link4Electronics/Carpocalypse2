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
#include "loading1.h"
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
#include "world3.h"

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
#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern char * C2_HOOK_FASTCALL GetALineAndDontArgue(FILE* pF, char* pS);
extern int C2_HOOK_FASTCALL GetAnInt(FILE* pF);
extern br_material* C2_HOOK_FASTCALL GetSimpleMaterial(char* pName, tRendererShadingType pShading_type);
extern char _fgetpos[];
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern char _fsetpos[];
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern tPhysics_joint* C2_HOOK_FASTCALL AllocatePhysicsJoint(int pCount_limits, int pType);
extern br_actor* C2_HOOK_CDECL BrActorAdd();
extern br_actor* C2_HOOK_CDECL BrActorAllocate();
extern void C2_HOOK_CDECL BrActorFree();
extern br_actor* C2_HOOK_CDECL BrActorLoad();
extern br_actor* C2_HOOK_CDECL BrActorRemove();
extern br_material* C2_HOOK_CDECL BrMaterialAdd();
extern br_material* C2_HOOK_CDECL BrMaterialAllocate();
extern void C2_HOOK_CDECL BrMaterialUpdate();
extern void C2_HOOK_CDECL BrMatrix34Copy();
extern void C2_HOOK_CDECL BrMatrix34Identity();
extern void* C2_HOOK_CDECL BrMemAllocate();
extern void* C2_HOOK_CDECL BrMemCalloc();
extern br_model* C2_HOOK_CDECL BrModelAdd();
extern br_model* C2_HOOK_CDECL BrModelAllocate();
extern char* C2_HOOK_CDECL BrResStrDup();
extern void C2_HOOK_FASTCALL ClosePackFileAndSetTiffLoading(tTWTVFS twt);
extern intptr_t C2_HOOK_FASTCALL DRActorEnumRecurse(br_actor* pActor, br_actor_enum_cbfn* callback, void* arg);
extern int C2_HOOK_FASTCALL DRStricmp(const char* p1, const char* p2);
extern FILE* C2_HOOK_FASTCALL DRfopen(const char* pFilename, const char* pMode);
extern void C2_HOOK_CDECL DoNotDprintf();
extern void C2_NORETURN C2_HOOK_CDECL FatalError();
extern int C2_HOOK_FASTCALL FindBestColourMatch(br_colour pColour);
extern char * C2_HOOK_FASTCALL GetALineWithNoPossibleService(FILE* pF, char* pS);
extern br_scalar C2_HOOK_FASTCALL GetAScalar(FILE* pF);
extern void C2_HOOK_FASTCALL GetAString(FILE* pF, char* pString);
extern void C2_HOOK_FASTCALL GetPairOfFloats(FILE* pF, float* pF1, float* pF2);
extern void C2_HOOK_FASTCALL GetThreeFloats(FILE * pF, float* pF1, float* pF2, float* pF3);
extern void C2_HOOK_FASTCALL GlorifyMaterial(br_material** pMaterials, int pCount, tRendererShadingType pShading);
extern void C2_HOOK_FASTCALL LoadAllImagesInDirectory(tBrender_storage* pStorage_space, const char* path);
extern void C2_HOOK_FASTCALL LoadAllMaterialsInDirectory(tBrender_storage* pStorage, const char* pPath, tRendererShadingType pShading);
extern void C2_HOOK_FASTCALL LoadAllModelsInDirectory(tBrender_storage *pStorage, const char* pPath);
extern void C2_HOOK_FASTCALL LoadAllShadeTablesInDirectory(tBrender_storage* pStorage, const char* pPath);
extern tTWTVFS C2_HOOK_FASTCALL OpenPackFileAndSetTiffLoading(const char* path);
extern void C2_NORETURN C2_HOOK_FASTCALL PDFatalError(const char* pThe_str);
extern int C2_HOOK_FASTCALL PFfclose(FILE* pFile);
extern int C2_HOOK_FASTCALL PFfeof(FILE* pFile);
extern void C2_HOOK_FASTCALL PathCat(char* pDestn_str, const char* pStr_1, const char* pStr_2);
extern void C2_HOOK_FASTCALL PossibleService(void);
extern void C2_HOOK_FASTCALL PrintMemoryDump(int pFlags, char* pTitle);
extern void C2_HOOK_FASTCALL ReadMechanicsShapes(tPhysics_shape** pShape, FILE* pF);
extern void C2_HOOK_FASTCALL UpdateCollisionObject(tPhysics_object* pCollision_info);
extern void __cdecl _ftol();
extern tPath_name gApplication_path;
extern int gCount_drone_forms;
extern int gCount_drones;
extern char gCurrent_load_directory[256];
extern char gCurrent_load_name[256];
extern char gDefault_drone_direction[];
extern char gDouble_0058a068[];
extern tBrender_storage gDroneStorage;
extern tDrone_form gDrone_forms[64];
extern float gDrone_render_zero;
extern char gDrone_select_max_distance_squared[];
extern tDrone_spec* gDrone_specs;
extern const int gMini_map_glowing_line_animation_indices[5];
extern char gNext_crush_actor_id[];
extern br_actor* gNon_track_actor;
extern char gPed_flags_0058f2e4[];
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_FASTCALL AddFunkotronics(FILE* pF, int pOwner, int pRef_offset, tCar_crush_buffer* pCar_crush_datas);
extern void C2_HOOK_FASTCALL AddGroovidelics(FILE* pF, int pOwner, br_actor* pParent_actor, int pRef_offset, int pAllowed_to_be_absent);
extern tAdd_to_storage_result C2_HOOK_FASTCALL AddMaterialToStorage(tBrender_storage* pStorage_space, br_material* pThe_mat);
extern void C2_HOOK_FASTCALL AdjustRenderScreenSize(void);
extern int C2_HOOK_CDECL AllocateUserDetailLevel();
extern void C2_HOOK_FASTCALL AllowOpenToFail(void);
extern void C2_HOOK_FASTCALL AttachCrushDataToActorModels(br_actor* pActor, tCar_spec* pCar_spec);
extern int C2_HOOK_CDECL AttachGroovidelic();
extern void C2_HOOK_FASTCALL AutoFogificateMaterials(tRendererShadingType pShading_type);
extern br_pixelmap* C2_HOOK_CDECL BrMapFind();
extern br_material* C2_HOOK_CDECL BrMaterialFind();
extern void C2_HOOK_CDECL BrModelUpdate();
extern br_actor* C2_HOOK_FASTCALL DRActorFindRecurse(br_actor* pSearch_root, const char* pName);
extern void C2_HOOK_FASTCALL DoNotAllowOpenToFail(void);
extern float C2_HOOK_FASTCALL GetAFloat(FILE* pF);
extern void C2_HOOK_FASTCALL GetAVector3(FILE* pF, br_vector3* pV);
extern void C2_HOOK_FASTCALL GetFourInts(FILE* pF, int* pF1, int* pF2, int* pF3, int* pF4);
extern void C2_HOOK_FASTCALL GetThreeInts(FILE* pF, int* pF1, int* pF2, int* pF3);
extern int C2_HOOK_FASTCALL IRandomBetween(int pA, int pB);
extern void C2_HOOK_FASTCALL InitPhysModCrushData(tCar_spec * pCar_spec);
extern void C2_HOOK_FASTCALL LinkCarCrushDatas(br_actor *pActor, tCar_crush_buffer *pBuffer);
extern int C2_HOOK_CDECL LinkCrushData();
extern int C2_HOOK_CDECL LinkCrushModel();
extern int C2_HOOK_FASTCALL LoadCarCrush(tCar_crush_buffer* pCar_crush_buffer, const char* pPath, tBrender_storage* pBrender_storage, tCar_crush_spec** pCar_crush_spec);
extern br_pixelmap* C2_HOOK_FASTCALL LoadPixelmap(const char* pPath_name);
extern void C2_HOOK_FASTCALL PackFileRerevertTiffLoading(void);
extern void C2_HOOK_FASTCALL PackFileRevertTiffLoading(void);
extern void C2_HOOK_FASTCALL ReinitialiseRearviewCamera(void);
#endif
#ifdef CARPOCALYPSE2_MATCHING
#endif
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
void C2_NAKED C2_HOOK_FASTCALL GetFiveScalars(FILE* pF, br_scalar* pS1, br_scalar* pS2, br_scalar* pS3, br_scalar* pS4, br_scalar* pS5) {
#ifndef CARPOCALYPSE2_MATCHING
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
#else
    __asm {
            sub esp, 0x114
            push esi
            push edi
            mov esi, edx
            mov edi, ecx
            call PossibleService
            lea edx, [esp + 0x1c]
            mov ecx, edi
            call GetALineWithNoPossibleService
            lea eax, [esp + 0x1c]
            push offset gPed_flags_0058f2e4+0x138
            push eax
            call strtok
            add esp, 8
            lea ecx, [esp + 0x18]
            push ecx
            push offset gMini_map_glowing_line_animation_indices+0x7dc
            push eax
            call sscanf
            add esp, 0xc
            push offset gPed_flags_0058f2e4+0x138
            push 0
            call strtok
            add esp, 8
            lea edx, [esp + 0x10]
            push edx
            push offset gMini_map_glowing_line_animation_indices+0x7dc
            push eax
            call sscanf
            add esp, 0xc
            push offset gPed_flags_0058f2e4+0x138
            push 0
            call strtok
            add esp, 8
            lea ecx, [esp + 8]
            push ecx
            push offset gMini_map_glowing_line_animation_indices+0x7dc
            push eax
            call sscanf
            add esp, 0xc
            push offset gPed_flags_0058f2e4+0x138
            push 0
            call strtok
            add esp, 8
            lea edx, [esp + 0xc]
            push edx
            push offset gMini_map_glowing_line_animation_indices+0x7dc
            push eax
            call sscanf
            add esp, 0xc
            push offset gPed_flags_0058f2e4+0x138
            push 0
            call strtok
            add esp, 8
            lea ecx, [esp + 0x14]
            push ecx
            push offset gMini_map_glowing_line_animation_indices+0x7dc
            push eax
            call sscanf
            mov edx, dword ptr [esp + 0x24]
            mov eax, dword ptr [esp + 0x12c]
            fld dword ptr [esp + 0x1c]
            mov ecx, dword ptr [esp + 0x130]
            mov dword ptr [esi], edx
            _emit 0xd9
        _emit 0x18
            fld dword ptr [esp + 0x14]
            mov edx, dword ptr [esp + 0x134]
            mov eax, dword ptr [esp + 0x138]
            _emit 0xd9
        _emit 0x19
            fld dword ptr [esp + 0x18]
            add esp, 0xc
            _emit 0xd9
        _emit 0x1a
            fld dword ptr [esp + 0x14]
            pop edi
            pop esi
            _emit 0xd9
        _emit 0x18
            add esp, 0x114
            ret 0x10
    }
#endif
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
int C2_NAKED C2_HOOK_FASTCALL DRfgetpos(FILE* pFile, fpos_t* pos) {
#ifndef CARPOCALYPSE2_MATCHING

    if ((uintptr_t)pFile >= CARPOCALYPSE2_ASIZE(gTwatVfsFiles)) {
        return fgetpos(pFile, pos);
    }
    *(br_uint_64*)pos = (br_uint_64)gTwatVfsFiles[(uintptr_t)pFile].pos;
    gTwatVfsFiles[(uintptr_t)pFile].error = 0;
    return 0;
#else
    __asm {
            push esi
            cmp ecx, 0x33
            mov esi, edx
            jb L004b4ad4
            push esi
            push ecx
            call fgetpos
            add esp, 8
            pop esi
            ret
    L004b4ad4:
            shl ecx, 4
            mov eax, dword ptr [ecx + gTwatVfsFiles+0x4]
            lea ecx, [ecx + gTwatVfsFiles]
            cdq
            mov dword ptr [esi], eax
            mov dword ptr [esi + 4], edx
            mov dword ptr [ecx + 0xc], 0
            xor eax, eax
            pop esi
            ret
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004b4c40
int C2_HOOK_FASTCALL DRferror(FILE* pFile) {
    if ((uintptr_t)pFile >= CARPOCALYPSE2_ASIZE(gTwatVfsFiles)) {
        return ferror(pFile);
    } else {
        return gTwatVfsFiles[(uintptr_t)pFile].error;
    }
}

// FUNCTION: CARMA2_HW 0x004b4c60
void C2_HOOK_FASTCALL DRclearerr(FILE* pFile) {
    tTwatVfsFile* twtFile;

    if ((int)pFile >= CARPOCALYPSE2_ASIZE(gTwatVfsFiles)) {
        clearerr(pFile);
        return;
    }
    twtFile = &gTwatVfsFiles[(uintptr_t)pFile];
    twtFile->error = 0;
}

// FUNCTION: CARMA2_HW 0x004b48c0
int C2_NAKED C2_HOOK_FASTCALL DRfgetc2(FILE* pFile) {
#ifndef CARPOCALYPSE2_MATCHING
    tTwatVfsFile* twtFile;
    int result;

    if ((int)pFile >= CARPOCALYPSE2_ASIZE(gTwatVfsFiles)) {
        return fgetc(pFile);
    }
    twtFile = &gTwatVfsFiles[(uintptr_t)pFile];
    if (twtFile->pos < twtFile->end) {
        twtFile->error = 0;
        result = *(tS8*)twtFile->pos;
        twtFile->pos++;
        return result;
    }
    twtFile->error = -1;
    return EOF;
#else
    __asm {
            cmp ecx, 0x33
            jb L004b48cf
            push ecx
            call fgetc
            add esp, 4
            ret
    L004b48cf:
            shl ecx, 4
            mov edx, dword ptr [ecx + gTwatVfsFiles+0x4]
            mov eax, dword ptr [ecx + gTwatVfsFiles+0x8]
            lea ecx, [ecx + gTwatVfsFiles]
            cmp edx, eax
            jb L004b48ef
            or eax, 0xffffffff
            mov dword ptr [ecx + 0xc], eax
            ret
    L004b48ef:
            mov dword ptr [ecx + 0xc], 0
            movsx eax, byte ptr [edx]
            inc edx
            mov dword ptr [ecx + 4], edx
            ret
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004b4b30
int C2_NAKED C2_HOOK_FASTCALL DRfsetpos(FILE* pF, fpos_t* pos) {
#ifndef CARPOCALYPSE2_MATCHING
    tTwatVfsFile* twtFile;

    if ((int)pF >= CARPOCALYPSE2_ASIZE(gTwatVfsFiles)) {
        return fsetpos(pF, pos);
    }
    twtFile = &gTwatVfsFiles[(uintptr_t)pF];
    if (*(tU8**)pos < twtFile->start || *(tU8**)pos > twtFile->end) {
        twtFile->error = -1;
        return -1;
    }
    twtFile->pos = *(tU8**)pos;
    twtFile->error = 0;
    return 0;
#else
    __asm {
            cmp ecx, 0x33
            jb L004b4b40
            push edx
            push ecx
            call fsetpos
            add esp, 8
            ret
    L004b4b40:
            shl ecx, 4
            lea eax, [ecx + gTwatVfsFiles]
            mov ecx, dword ptr [ecx + gTwatVfsFiles]
            cmp edx, ecx
            jb L004b4b65
            cmp edx, dword ptr [eax + 8]
            ja L004b4b65
            mov dword ptr [eax + 4], edx
            mov dword ptr [eax + 0xc], 0
            xor eax, eax
            ret
    L004b4b65:
            mov dword ptr [eax + 0xc], 0xffffffff
            or eax, 0xffffffff
            ret
    }
#endif
}

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

    fprintf(pF, "NETSETTINGS %d\n\n", pIndex);
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
        case 'l':
            pCar_spec->headup_slots[pIndex][j].justification = eJust_left;
            break;
        case 'r':
            pCar_spec->headup_slots[pIndex][j].justification = eJust_right;
            break;
        case 'c':
            pCar_spec->headup_slots[pIndex][j].justification = eJust_centre;
            break;
        }
        if (s[1] == 'c') {
            pCar_spec->headup_slots[pIndex][j].cockpit_anchored = 1;
        }
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->headup_slots[pIndex][j].dim_left);
        if (pCar_spec->headup_slots[pIndex][j].dim_left >= 0) {
            pCar_spec->headup_slots[pIndex][j].dimmed_background = 1;
            str = strtok(NULL, "\t ,/");
            sscanf(str, "%d", &pCar_spec->headup_slots[pIndex][j].dim_top);
            str = strtok(NULL, "\t ,/");
            sscanf(str, "%d", &pCar_spec->headup_slots[pIndex][j].dim_right);
            str = strtok(NULL, "\t ,/");
            sscanf(str, "%d", &pCar_spec->headup_slots[pIndex][j].dim_bottom);
        } else {
            pCar_spec->headup_slots[pIndex][j].dimmed_background = 0;
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

// FUNCTION: CARMA2_HW 0x0048f6d0
void C2_HOOK_FASTCALL FUN_0048f6d0(tBrender_storage* pStorage_space, FILE* pFile) {
    int count;
    char line[256];
    char* token;

    GetALineAndDontArgue(pFile, line);
    token = strtok(line, "	 ,/");
    sscanf(token, "%d", &count);
    LoadPixelmapsFromListFile(pStorage_space, pFile, count);
}

// FUNCTION: CARMA2_HW 0x00513970
br_pixelmap* C2_HOOK_FASTCALL DRPixelmapLoad(char* pFile_name) {
    br_pixelmap* the_map;

    the_map = BrPixelmapLoad(pFile_name);
    if (the_map != NULL) {
        the_map->row_bytes = (br_int_16)((the_map->row_bytes + sizeof(tS32) - 1) & ~(sizeof(tS32) - 1));
        the_map->origin_x = 0;
        the_map->origin_y = 0;
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
void C2_NAKED C2_HOOK_FASTCALL LoadPanGameDroneInfo(void) {
#ifndef CARPOCALYPSE2_MATCHING
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
#else
    __asm {
            sub esp, 0x41c
            push ebx
            push ebp
            push esi
            push edi
            mov edi, offset gApplication_path
            or ecx, 0xffffffff
            xor eax, eax
            lea edx, [esp + 0x22c]
            repne scasb
            not ecx
            sub edi, ecx
            push offset gDefault_drone_direction+0x5f0
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            lea edx, [esp + 0x230]
            shr ecx, 2
            rep movsd
            mov ecx, eax
            and ecx, 3
            rep movsb
            lea ecx, [esp + 0x230]
            call PathCat
            lea edx, [esp + 0x22c]
            lea ecx, [esp + 0x22c]
            push offset gDefault_drone_direction+0x5e4
            call PathCat
            mov edx, offset gNext_crush_actor_id+0x180
            lea ecx, [esp + 0x22c]
            call DRfopen
            mov ebp, eax
            test ebp, ebp
            mov dword ptr [esp + 0x1c], ebp
            jne L0044eda3
            lea ecx, [esp + 0x22c]
            push ecx
            push 0xaf
            call FatalError
            add esp, 8
    L0044eda3:
            lea edx, [esp + 0x2c]
            mov ecx, ebp
            call GetALineAndDontArgue
            lea edx, [esp + 0x2c]
            push offset gPed_flags_0058f2e4+0x138
            push edx
            call strtok
            add esp, 8
            mov esi, offset gDefault_drone_direction+0x5dc
            lea eax, [esp + 0x2c]
    L0044edc9:
            mov dl, byte ptr [eax]
            mov bl, byte ptr [esi]
            mov cl, dl
            cmp dl, bl
            jne L0044edf1
            test cl, cl
            je L0044eded
            mov dl, byte ptr [eax + 1]
            mov bl, byte ptr [esi + 1]
            mov cl, dl
            cmp dl, bl
            jne L0044edf1
            add eax, 2
            add esi, 2
            test cl, cl
            jne L0044edc9
    L0044eded:
            xor eax, eax
            jmp L0044edf6
    L0044edf1:
            sbb eax, eax
            sbb eax, -1
    L0044edf6:
            test eax, eax
            jne L0044ee2a
            push offset gPed_flags_0058f2e4+0x138
            push eax
            call strtok
            add esp, 8
            lea ecx, [esp + 0x28]
            push ecx
            push offset gPed_flags_0058f2e4+0x134
            push eax
            call sscanf
            add esp, 0xc
            cmp eax, 1
            je L0044ee3f
            lea edx, [esp + 0x22c]
            push edx
            jmp L0044ee32
    L0044ee2a:
            lea eax, [esp + 0x22c]
            push eax
    L0044ee32:
            push 0xaf
            call FatalError
            add esp, 8
    L0044ee3f:
            mov ecx, 0x880
            xor eax, eax
            mov edi, offset gDrone_forms
            lea edx, [esp + 0x2c]
            rep stosd
            mov ecx, ebp
            mov dword ptr [gCount_drone_forms], 0
            call GetALineAndDontArgue
            cmp dword ptr [gCount_drone_forms], 0x40
            jge L0044f623
    L0044ee6f:
            mov esi, offset gDefault_drone_direction+0x5cc
            lea eax, [esp + 0x2c]
    L0044ee78:
            mov dl, byte ptr [eax]
            mov bl, byte ptr [esi]
            mov cl, dl
            cmp dl, bl
            jne L0044eea0
            test cl, cl
            je L0044ee9c
            mov dl, byte ptr [eax + 1]
            mov bl, byte ptr [esi + 1]
            mov cl, dl
            cmp dl, bl
            jne L0044eea0
            add eax, 2
            add esi, 2
            test cl, cl
            jne L0044ee78
    L0044ee9c:
            xor eax, eax
            jmp L0044eea5
    L0044eea0:
            sbb eax, eax
            sbb eax, -1
    L0044eea5:
            test eax, eax
            je L0044f623
            mov ecx, ebp
            call PFfeof
            test eax, eax
            jne L0044f623
            mov edi, offset gApplication_path
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            lea edx, [esp + 0x32c]
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            push offset gDefault_drone_direction+0x5f0
            shr ecx, 2
            rep movsd
            mov ecx, eax
            lea edx, [esp + 0x330]
            and ecx, 3
            mov dword ptr [esp + 0x24], 0xffffffff
            rep movsb
            lea ecx, [esp + 0x330]
            call PathCat
            lea ecx, [esp + 0x2c]
            lea edx, [esp + 0x32c]
            push ecx
            lea ecx, [esp + 0x330]
            call PathCat
            lea ecx, [esp + 0x32c]
            call OpenPackFileAndSetTiffLoading
            lea ecx, [esp + 0x2c]
            mov dword ptr [esp + 0x24], eax
            call OpenDroneFile
            mov edi, eax
            mov eax, dword ptr [gCount_drone_forms]
            mov edx, eax
            mov ecx, edi
            shl edx, 4
            add edx, eax
            mov dword ptr [esp + 0x10], edi
            lea ebp, [edx*8 + gDrone_forms]
            lea edx, [esp + 0x12c]
            mov dword ptr [esp + 0x14], ebp
            call GetALineAndDontArgue
            lea eax, [esp + 0x12c]
            push offset gPed_flags_0058f2e4+0x138
            push eax
            call strtok
            add esp, 8
            mov esi, offset gDefault_drone_direction+0x5dc
            lea eax, [esp + 0x12c]
    L0044ef80:
            mov dl, byte ptr [eax]
            mov bl, byte ptr [esi]
            mov cl, dl
            cmp dl, bl
            jne L0044efa8
            test cl, cl
            je L0044efa4
            mov dl, byte ptr [eax + 1]
            mov bl, byte ptr [esi + 1]
            mov cl, dl
            cmp dl, bl
            jne L0044efa8
            add eax, 2
            add esi, 2
            test cl, cl
            jne L0044ef80
    L0044efa4:
            xor eax, eax
            jmp L0044efad
    L0044efa8:
            sbb eax, eax
            sbb eax, -1
    L0044efad:
            test eax, eax
            jne L0044efde
            push offset gPed_flags_0058f2e4+0x138
            push eax
            call strtok
            add esp, 8
            lea ecx, [esp + 0x18]
            push ecx
            push offset gPed_flags_0058f2e4+0x134
            push eax
            call sscanf
            add esp, 0xc
            cmp eax, 1
            je L0044eff0
            lea edx, [esp + 0x2c]
            push edx
            jmp L0044efe3
    L0044efde:
            lea eax, [esp + 0x2c]
            push eax
    L0044efe3:
            push 0xaf
            call FatalError
            add esp, 8
    L0044eff0:
            cmp dword ptr [esp + 0x18], 2
            jge L0044f009
            lea ecx, [esp + 0x2c]
            push ecx
            push 0xaf
            call FatalError
            add esp, 8
    L0044f009:
            lea edx, [esp + 0x12c]
            mov ecx, edi
            call GetALineAndDontArgue
            lea edx, [esp + 0x12c]
            push offset gPed_flags_0058f2e4+0x138
            push edx
            call strtok
            add esp, 8
            mov esi, eax
            lea ecx, [esp + 0x2c]
    L0044f032:
            mov dl, byte ptr [ecx]
            mov bl, byte ptr [esi]
            mov al, dl
            cmp dl, bl
            jne L0044f05a
            test al, al
            je L0044f056
            mov dl, byte ptr [ecx + 1]
            mov bl, byte ptr [esi + 1]
            mov al, dl
            cmp dl, bl
            jne L0044f05a
            add ecx, 2
            add esi, 2
            test al, al
            jne L0044f032
    L0044f056:
            xor eax, eax
            jmp L0044f05f
    L0044f05a:
            sbb eax, eax
            sbb eax, -1
    L0044f05f:
            test eax, eax
            je L0044f075
            lea eax, [esp + 0x2c]
            push eax
            push 0xaf
            call FatalError
            add esp, 8
    L0044f075:
            lea ecx, [esp + 0x2c]
            push 0xc
            push ecx
            push ebp
            call strncpy
            add esp, 0xc
            lea edx, [esp + 0x12c]
            mov ecx, edi
            call GetALineAndDontArgue
            lea edx, [esp + 0x12c]
            push offset gPed_flags_0058f2e4+0x138
            push edx
            call strtok
            add esp, 8
            xor ebp, ebp
            mov edi, offset gDrone_type_names
    L0044f0af:
            mov ecx, dword ptr [edi]
            mov esi, eax
    L0044f0b3:
            mov bl, byte ptr [ecx]
            mov dl, bl
            cmp bl, byte ptr [esi]
            jne L0044f0d7
            test dl, dl
            je L0044f0d3
            mov bl, byte ptr [ecx + 1]
            mov dl, bl
            cmp bl, byte ptr [esi + 1]
            jne L0044f0d7
            add ecx, 2
            add esi, 2
            test dl, dl
            jne L0044f0b3
    L0044f0d3:
            xor ecx, ecx
            jmp L0044f0dc
    L0044f0d7:
            sbb ecx, ecx
            sbb ecx, -1
    L0044f0dc:
            test ecx, ecx
            je L0044f0f0
            add edi, 4
            inc ebp
            cmp edi, offset gGroove_funk_type_names
            jl L0044f0af
            mov ebp, dword ptr [esp + 0x20]
    L0044f0f0:
            test ebp, ebp
            jge L0044f10c
            lea eax, [esp + 0x2c]
            push eax
            push 0xaf
            call FatalError
            mov ebx, dword ptr [esp + 0x1c]
            add esp, 8
            jmp L0044f113
    L0044f10c:
            mov ebx, dword ptr [esp + 0x14]
            mov dword ptr [ebx + 0x10], ebp
    L0044f113:
            mov esi, dword ptr [esp + 0x10]
            mov ecx, esi
            call GetAScalar
            lea ecx, [ebx + 0x38]
            lea edx, [ebx + 0x34]
            push ecx
            push edx
            fstp dword ptr [ebx + 0x14]
            lea edx, [ebx + 0x30]
            mov ecx, esi
            call GetThreeFloats
            lea edx, [esp + 0x12c]
            mov ecx, esi
            call GetALineAndDontArgue
            lea eax, [esp + 0x12c]
            push offset gPed_flags_0058f2e4+0x138
            push eax
            call strtok
            mov edi, eax
            add esp, 8
            mov esi, offset gDefault_drone_direction+0x5c4
            mov ecx, edi
    L0044f15f:
            mov al, byte ptr [ecx]
            mov dl, al
            cmp al, byte ptr [esi]
            jne L0044f183
            test dl, dl
            je L0044f17f
            mov al, byte ptr [ecx + 1]
            mov dl, al
            cmp al, byte ptr [esi + 1]
            jne L0044f183
            add ecx, 2
            add esi, 2
            test dl, dl
            jne L0044f15f
    L0044f17f:
            xor ecx, ecx
            jmp L0044f188
    L0044f183:
            sbb ecx, ecx
            sbb ecx, -1
    L0044f188:
            test ecx, ecx
            jne L0044f196
            mov eax, dword ptr [ebx + 0x2c]
            or al, 1
            mov dword ptr [ebx + 0x2c], eax
            jmp L0044f1e6
    L0044f196:
            mov esi, offset gDefault_drone_direction+0x5bc
            mov eax, edi
    L0044f19d:
            mov dl, byte ptr [eax]
            mov cl, dl
            cmp dl, byte ptr [esi]
            jne L0044f1c1
            test cl, cl
            je L0044f1bd
            mov dl, byte ptr [eax + 1]
            mov cl, dl
            cmp dl, byte ptr [esi + 1]
            jne L0044f1c1
            add eax, 2
            add esi, 2
            test cl, cl
            jne L0044f19d
    L0044f1bd:
            xor eax, eax
            jmp L0044f1c6
    L0044f1c1:
            sbb eax, eax
            sbb eax, -1
    L0044f1c6:
            test eax, eax
            jne L0044f1d4
            mov eax, dword ptr [ebx + 0x2c]
            and al, 0xfe
            mov dword ptr [ebx + 0x2c], eax
            jmp L0044f1e6
    L0044f1d4:
            lea eax, [esp + 0x2c]
            push eax
            push 0xaf
            call FatalError
            add esp, 8
    L0044f1e6:
            mov ecx, dword ptr [esp + 0x10]
            lea edx, [esp + 0x12c]
            call GetALineAndDontArgue
            lea ecx, [esp + 0x12c]
            push offset gPed_flags_0058f2e4+0x138
            push ecx
            call strtok
            mov edi, eax
            add esp, 8
            mov esi, offset gDefault_drone_direction+0x5b0
            mov ecx, edi
    L0044f214:
            mov al, byte ptr [ecx]
            mov dl, al
            cmp al, byte ptr [esi]
            jne L0044f238
            test dl, dl
            je L0044f234
            mov al, byte ptr [ecx + 1]
            mov dl, al
            cmp al, byte ptr [esi + 1]
            jne L0044f238
            add ecx, 2
            add esi, 2
            test dl, dl
            jne L0044f214
    L0044f234:
            xor ecx, ecx
            jmp L0044f23d
    L0044f238:
            sbb ecx, ecx
            sbb ecx, -1
    L0044f23d:
            test ecx, ecx
            jne L0044f251
            mov esi, dword ptr [esp + 0x10]
            mov ecx, esi
            call GetAScalar
            fstp dword ptr [ebx + 0x18]
            jmp L0044f2c6
    L0044f251:
            mov esi, offset gDefault_drone_direction+0x5a4
            mov eax, edi
    L0044f258:
            mov dl, byte ptr [eax]
            mov cl, dl
            cmp dl, byte ptr [esi]
            jne L0044f27c
            test cl, cl
            je L0044f278
            mov dl, byte ptr [eax + 1]
            mov cl, dl
            cmp dl, byte ptr [esi + 1]
            jne L0044f27c
            add eax, 2
            add esi, 2
            test cl, cl
            jne L0044f258
    L0044f278:
            xor eax, eax
            jmp L0044f281
    L0044f27c:
            sbb eax, eax
            sbb eax, -1
    L0044f281:
            test eax, eax
            jne L0044f2b0
            mov esi, dword ptr [esp + 0x10]
            mov dword ptr [ebx + 0x18], 0xbf800000
            mov ecx, esi
            call GetAScalar
            fstp dword ptr [ebx + 0x24]
            mov ecx, esi
            call GetAScalar
            fstp dword ptr [ebx + 0x20]
            mov ecx, esi
            call GetAScalar
            fstp dword ptr [ebx + 0x1c]
            jmp L0044f2c6
    L0044f2b0:
            lea eax, [esp + 0x2c]
            push eax
            push 0xaf
            call FatalError
            mov esi, dword ptr [esp + 0x18]
            add esp, 8
    L0044f2c6:
            mov ecx, esi
            call GetAScalar
            fcom dword ptr [gDrone_render_zero]
            fstp dword ptr [ebx + 0x28]
            fnstsw ax
            test ah, 0x40
            jne L0044f307
            lea ecx, [ebx + 0x40]
            lea edx, [ebx + 0x3c]
            push ecx
            mov ecx, esi
            call GetPairOfFloats
            lea edx, [ebx + 0x48]
            mov ecx, esi
            push edx
            lea edx, [ebx + 0x44]
            call GetPairOfFloats
            lea eax, [ebx + 0x50]
            lea edx, [ebx + 0x4c]
            push eax
            mov ecx, esi
            call GetPairOfFloats
    L0044f307:
            lea edx, [esp + 0x12c]
            mov ecx, esi
            call GetALineAndDontArgue
            lea ecx, [esp + 0x12c]
            push offset gPed_flags_0058f2e4+0x138
            push ecx
            call strtok
            mov edi, eax
            add esp, 8
            mov esi, offset gDefault_drone_direction+0x59c
            mov ecx, edi
    L0044f333:
            mov al, byte ptr [ecx]
            mov dl, al
            cmp al, byte ptr [esi]
            jne L0044f357
            test dl, dl
            je L0044f353
            mov al, byte ptr [ecx + 1]
            mov dl, al
            cmp al, byte ptr [esi + 1]
            jne L0044f357
            add ecx, 2
            add esi, 2
            test dl, dl
            jne L0044f333
    L0044f353:
            xor ecx, ecx
            jmp L0044f35c
    L0044f357:
            sbb ecx, ecx
            sbb ecx, -1
    L0044f35c:
            test ecx, ecx
            jne L0044f36a
            mov eax, dword ptr [ebx + 0x2c]
            or al, 2
            mov dword ptr [ebx + 0x2c], eax
            jmp L0044f3ba
    L0044f36a:
            mov esi, offset gDefault_drone_direction+0x590
            mov eax, edi
    L0044f371:
            mov dl, byte ptr [eax]
            mov cl, dl
            cmp dl, byte ptr [esi]
            jne L0044f395
            test cl, cl
            je L0044f391
            mov dl, byte ptr [eax + 1]
            mov cl, dl
            cmp dl, byte ptr [esi + 1]
            jne L0044f395
            add eax, 2
            add esi, 2
            test cl, cl
            jne L0044f371
    L0044f391:
            xor eax, eax
            jmp L0044f39a
    L0044f395:
            sbb eax, eax
            sbb eax, -1
    L0044f39a:
            test eax, eax
            jne L0044f3a8
            mov eax, dword ptr [ebx + 0x2c]
            and al, 0xfd
            mov dword ptr [ebx + 0x2c], eax
            jmp L0044f3ba
    L0044f3a8:
            lea eax, [esp + 0x2c]
            push eax
            push 0xaf
            call FatalError
            add esp, 8
    L0044f3ba:
            mov ecx, dword ptr [esp + 0x10]
            lea edx, [esp + 0x12c]
            call GetALineAndDontArgue
            lea ecx, [esp + 0x12c]
            push offset gPed_flags_0058f2e4+0x138
            push ecx
            call strtok
            mov edi, eax
            add esp, 8
            mov esi, offset gDefault_drone_direction+0x584
            mov ecx, edi
    L0044f3e8:
            mov al, byte ptr [ecx]
            mov dl, al
            cmp al, byte ptr [esi]
            jne L0044f40c
            test dl, dl
            je L0044f408
            mov al, byte ptr [ecx + 1]
            mov dl, al
            cmp al, byte ptr [esi + 1]
            jne L0044f40c
            add ecx, 2
            add esi, 2
            test dl, dl
            jne L0044f3e8
    L0044f408:
            xor ecx, ecx
            jmp L0044f411
    L0044f40c:
            sbb ecx, ecx
            sbb ecx, -1
    L0044f411:
            test ecx, ecx
            jne L0044f41f
            mov eax, dword ptr [ebx + 0x2c]
            or al, 0x10
            mov dword ptr [ebx + 0x2c], eax
            jmp L0044f46f
    L0044f41f:
            mov esi, offset gDefault_drone_direction+0x57c
            mov eax, edi
    L0044f426:
            mov dl, byte ptr [eax]
            mov cl, dl
            cmp dl, byte ptr [esi]
            jne L0044f44a
            test cl, cl
            je L0044f446
            mov dl, byte ptr [eax + 1]
            mov cl, dl
            cmp dl, byte ptr [esi + 1]
            jne L0044f44a
            add eax, 2
            add esi, 2
            test cl, cl
            jne L0044f426
    L0044f446:
            xor eax, eax
            jmp L0044f44f
    L0044f44a:
            sbb eax, eax
            sbb eax, -1
    L0044f44f:
            test eax, eax
            jne L0044f45d
            mov eax, dword ptr [ebx + 0x2c]
            and al, 0xef
            mov dword ptr [ebx + 0x2c], eax
            jmp L0044f46f
    L0044f45d:
            lea eax, [esp + 0x2c]
            push eax
            push 0xaf
            call FatalError
            add esp, 8
    L0044f46f:
            mov ecx, dword ptr [esp + 0x10]
            lea edx, [esp + 0x12c]
            call GetALineAndDontArgue
            lea ecx, [esp + 0x12c]
            push offset gPed_flags_0058f2e4+0x138
            push ecx
            call strtok
            mov edi, eax
            add esp, 8
            mov esi, offset gDefault_drone_direction+0x574
            mov ecx, edi
    L0044f49d:
            mov al, byte ptr [ecx]
            mov dl, al
            cmp al, byte ptr [esi]
            jne L0044f4c1
            test dl, dl
            je L0044f4bd
            mov al, byte ptr [ecx + 1]
            mov dl, al
            cmp al, byte ptr [esi + 1]
            jne L0044f4c1
            add ecx, 2
            add esi, 2
            test dl, dl
            jne L0044f49d
    L0044f4bd:
            xor ecx, ecx
            jmp L0044f4c6
    L0044f4c1:
            sbb ecx, ecx
            sbb ecx, -1
    L0044f4c6:
            test ecx, ecx
            jne L0044f4d4
            mov eax, dword ptr [ebx + 0x2c]
            or al, 4
            mov dword ptr [ebx + 0x2c], eax
            jmp L0044f524
    L0044f4d4:
            mov esi, offset gDefault_drone_direction+0x568
            mov eax, edi
    L0044f4db:
            mov dl, byte ptr [eax]
            mov cl, dl
            cmp dl, byte ptr [esi]
            jne L0044f4ff
            test cl, cl
            je L0044f4fb
            mov dl, byte ptr [eax + 1]
            mov cl, dl
            cmp dl, byte ptr [esi + 1]
            jne L0044f4ff
            add eax, 2
            add esi, 2
            test cl, cl
            jne L0044f4db
    L0044f4fb:
            xor eax, eax
            jmp L0044f504
    L0044f4ff:
            sbb eax, eax
            sbb eax, -1
    L0044f504:
            test eax, eax
            jne L0044f512
            mov eax, dword ptr [ebx + 0x2c]
            and al, 0xfb
            mov dword ptr [ebx + 0x2c], eax
            jmp L0044f524
    L0044f512:
            lea eax, [esp + 0x2c]
            push eax
            push 0xaf
            call FatalError
            add esp, 8
    L0044f524:
            mov ecx, dword ptr [ebx + 0x2c]
            mov ebp, 0xfffffff7
            and ecx, ebp
            mov dword ptr [ebx + 0x2c], ecx
            mov eax, dword ptr [esp + 0x18]
            cmp eax, 3
            jl L0044f5ee
            mov ecx, dword ptr [esp + 0x10]
            lea edx, [esp + 0x12c]
            call GetALineAndDontArgue
            lea ecx, [esp + 0x12c]
            push offset gPed_flags_0058f2e4+0x138
            push ecx
            call strtok
            mov edi, eax
            add esp, 8
            mov esi, offset gDefault_drone_direction+0x55c
            mov ecx, edi
    L0044f56c:
            mov al, byte ptr [ecx]
            mov dl, al
            cmp al, byte ptr [esi]
            jne L0044f590
            test dl, dl
            je L0044f58c
            mov al, byte ptr [ecx + 1]
            mov dl, al
            cmp al, byte ptr [esi + 1]
            jne L0044f590
            add ecx, 2
            add esi, 2
            test dl, dl
            jne L0044f56c
    L0044f58c:
            xor ecx, ecx
            jmp L0044f595
    L0044f590:
            sbb ecx, ecx
            sbb ecx, -1
    L0044f595:
            test ecx, ecx
            jne L0044f5a3
            mov eax, dword ptr [ebx + 0x2c]
            or al, 8
            mov dword ptr [ebx + 0x2c], eax
            jmp L0044f5ee
    L0044f5a3:
            mov esi, offset gDefault_drone_direction+0x54c
            mov eax, edi
    L0044f5aa:
            mov dl, byte ptr [eax]
            mov cl, dl
            cmp dl, byte ptr [esi]
            jne L0044f5ce
            test cl, cl
            je L0044f5ca
            mov dl, byte ptr [eax + 1]
            mov cl, dl
            cmp dl, byte ptr [esi + 1]
            jne L0044f5ce
            add eax, 2
            add esi, 2
            test cl, cl
            jne L0044f5aa
    L0044f5ca:
            xor eax, eax
            jmp L0044f5d3
    L0044f5ce:
            sbb eax, eax
            sbb eax, -1
    L0044f5d3:
            test eax, eax
            jne L0044f5dc
            and dword ptr [ebx + 0x2c], ebp
            jmp L0044f5ee
    L0044f5dc:
            lea eax, [esp + 0x2c]
            push eax
            push 0xaf
            call FatalError
            add esp, 8
    L0044f5ee:
            mov ecx, dword ptr [esp + 0x10]
            call PFfclose
            mov ecx, dword ptr [esp + 0x24]
            call ClosePackFileAndSetTiffLoading
            mov ebp, dword ptr [esp + 0x1c]
            lea edx, [esp + 0x2c]
            mov ecx, ebp
            call GetALineAndDontArgue
            mov eax, dword ptr [gCount_drone_forms]
            inc eax
            cmp eax, 0x40
            mov dword ptr [gCount_drone_forms], eax
            jl L0044ee6f
    L0044f623:
            mov ecx, ebp
            call PFfclose
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x41c
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0048c860
void C2_HOOK_FASTCALL LoadRaceInfo(int pRace_index, tRace_info* pRace_info) {
    int i;

    pRace_info->index = pRace_index;
    pRace_info->race_spec = &gRace_list[pRace_index];
    if (gNet_mode == eNet_mode_none) {
        pRace_info->number_of_racers = pRace_info->race_spec->count_explicit_opponents;
    }
    pRace_info->total_laps = pRace_info->race_spec->count_laps;
    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(pRace_info->initial_timer); i++) {
        pRace_info->initial_timer[i] = pRace_info->race_spec->initial_timer[i];
        pRace_info->completion_bonus[i] = pRace_info->race_spec->completion_bonus[i];
        pRace_info->completion_bonus_peds[i] = pRace_info->race_spec->completion_bonus_peds[i];
        pRace_info->completion_bonus_opponents[i] = pRace_info->race_spec->completion_bonus_opponents[i];
    }
}

// FUNCTION: CARMA2_HW 0x004a9600
void C2_HOOK_FASTCALL LoadCopCars(tRace_info* pRace_info) {
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
    strcpy(delim, "	 ,/");
    strcat(delim, "&");

    the_clause = pCar_spec->damage_programs[pImpact_location].clauses;
    for (i = 0; i < count; i++) {
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
            str = strtok(s, "	 ,/");
            for (k = 0; k < (int)CARPOCALYPSE2_ASIZE(gDamage_names); k++) {
                if (strcmp(str, gDamage_names[k]) == 0) {
                    the_clause->effects[j].type = k;
                    break;
                }
            }
            if (the_clause->effects[j].type < 0) {
                FatalError(kFatalError_UnknownDamageType_S, str);
            }
            str = strtok(NULL, "	 ,/");
            sscanf(str, "%f", &the_clause->effects[j].weakness_factor);
        }
        the_clause++;
    }
}

// FUNCTION: CARMA2_HW 0x004f6740
void C2_HOOK_FASTCALL LoadCarMaterials(tBrender_storage* pStorage, const char* pPath, tCar_spec* pCar_spec) {
    gMaterial_shading_for_callback = kRendererShadingType_Specular;
    gStorage_for_callbacks = pStorage;
    pCar_spec->old_material_count = pStorage->materials_count;
    PFForEveryFile(pPath, LoadIfItsAMaterial);
    pCar_spec->new_material_count = pStorage->materials_count;
    gMaterial_shading_for_callback = kRendererShadingType_Undefined;
    SmoothlySetCarMaterialFlags(pStorage, pCar_spec);
}

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x004f67a0
void C2_HOOK_FASTCALL SmoothlySetCarMaterialFlags(tBrender_storage* pStorage, tCar_spec* pCar_spec) {
    int i;
    br_material* material;

    for (i = pCar_spec->old_material_count; i < pCar_spec->new_material_count; i++) {
        material = pStorage->materials[i];
        if (gNbPixelBits == 16) {
            material->ka = 0.6f;
            material->flags &= ~BR_MATF_PRELIT;
            material->flags |= BR_MATF_SMOOTH | BR_MATF_LIGHT;
            material->kd = 0.2f;
            BrMaterialUpdate(material, BR_MATU_ALL);
        }
        material->user = material->colour_map;
    }
}
#pragma auto_inline(on)

// FUNCTION: CARMA2_HW 0x00488f00
void C2_NAKED C2_HOOK_FASTCALL LoadCarShrapnelMaterials(FILE* pF, tCar_spec* pCar_spec) {
#ifndef CARPOCALYPSE2_MATCHING
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
#else
    __asm {
            sub esp, 0x100
            push ebx
            push ebp
            push esi
            mov esi, edx
            mov ebp, ecx
            call GetAnInt
            xor ebx, ebx
            mov dword ptr [esi + 0x6c], eax
            test eax, eax
            jle L00488f5a
            push edi
            lea edi, [esi + 0x84]
    L00488f22:
            lea edx, [esp + 0x10]
            mov ecx, ebp
            call GetALineAndDontArgue
            test esi, esi
            je L00488f3e
            mov eax, dword ptr [esi + 0xc]
            mov edx, 2
            cmp eax, 5
            jg L00488f43
    L00488f3e:
            mov edx, 1
    L00488f43:
            lea ecx, [esp + 0x10]
            call GetSimpleMaterial
            mov dword ptr [edi], eax
            mov eax, dword ptr [esi + 0x6c]
            inc ebx
            add edi, 4
            cmp ebx, eax
            jl L00488f22
            pop edi
    L00488f5a:
            pop esi
            pop ebp
            pop ebx
            add esp, 0x100
            ret
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00488f70
void C2_NAKED C2_HOOK_FASTCALL LoadCar(const char* pCar_name, tDriver pDriver, tCar_spec* pCar_spec, int pOwner, const char* pDriver_name, tBrender_storage* pStorage_space) {
#ifndef CARPOCALYPSE2_MATCHING
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
    AutoFogificateMaterials(kRendererShadingType_Specular);
    for (i = pCar_spec->old_material_count; i < pCar_spec->new_material_count; i++) {
        br_material* material;

        material = pStorage_space->materials[i];
        if (material->identifier != NULL && material->identifier[0] != '\0') {
            material->identifier[0] = (material->flags & BR_MATF_TWO_SIDED) ? 'D' : 'S';
            BrMaterialUpdate(material, BR_MATU_ALL);
        }
    }
    gStorage_for_callbacks = prev_storage;
#else
    __asm {
            sub esp, 0x968
            push ebx
            push ebp
            mov ebp, dword ptr [esp + 0x974]
            push esi
            mov dword ptr [esp + 0x18], ecx
            push edi
            mov ecx, 0x659
            xor eax, eax
            mov edi, ebp
            mov ebx, edx
            rep stosd
            mov eax, dword ptr [gStorage_for_callbacks]
            cmp ebx, 8
            mov dword ptr [esp + 0x18], ebx
            mov dword ptr [esp + 0x68], eax
            mov dword ptr [gCurrent_car_spec], ebp
            jne L00488fce
            mov edi, dword ptr [esp + 0x1c]
            or ecx, 0xffffffff
            xor eax, eax
            repne scasb
            not ecx
            sub edi, ecx
            mov edx, ecx
            mov esi, edi
            mov edi, offset gProgram_state+0x1a5e
            shr ecx, 2
            rep movsd
            mov ecx, edx
            and ecx, 3
            rep movsb
    L00488fce:
            mov eax, dword ptr [gNet_avail_names+0x7ac]
            mov cl, byte ptr [gNet_avail_names+0x7b0]
            mov edi, dword ptr [esp + 0x1c]
            mov byte ptr [gCurrent_load_directory+0x4], cl
            mov dword ptr [gCurrent_load_directory], eax
            or ecx, 0xffffffff
            xor eax, eax
            repne scasb
            not ecx
            sub edi, ecx
            mov edx, ecx
            mov esi, edi
            mov edi, offset gCurrent_load_name
            shr ecx, 2
            rep movsd
            mov ecx, edx
            lea edx, [esp + 0x878]
            and ecx, 3
            rep movsb
            mov edi, offset gCurrent_load_name
            or ecx, 0xffffffff
            repne scasb
            not ecx
            mov eax, offset gCurrent_load_name
            dec ecx
            sub eax, 4
            mov edi, offset gCurrent_load_directory
            mov byte ptr [ecx + eax], 0
            or ecx, 0xffffffff
            xor eax, eax
            repne scasb
            not ecx
            sub edi, ecx
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            lea edx, [esp + 0x778]
            shr ecx, 2
            rep movsd
            mov ecx, eax
            xor eax, eax
            and ecx, 3
            rep movsb
            mov edi, offset gCurrent_load_name
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            mov edx, offset gApplication_path
            shr ecx, 2
            rep movsd
            mov ecx, eax
            and ecx, 3
            rep movsb
            lea ecx, [esp + 0x878]
            push ecx
            lea ecx, [esp + 0x278]
            call PathCat
            lea edx, [esp + 0x778]
            lea ecx, [esp + 0x274]
            push edx
            lea edx, [esp + 0x278]
            call PathCat
            lea ecx, [esp + 0x274]
            call OpenPackFileAndSetTiffLoading
            mov dword ptr [esp + 0x4c], eax
            mov eax, dword ptr [esp + 0x980]
            mov dword ptr [ebp + 0x18c4], 0
            mov dword ptr [ebp + 0xc], ebx
            cmp ebx, 8
            mov dword ptr [ebp], eax
            jne L004890e1
            mov dword ptr [gFunk_groove_flags], 1
            xor edx, edx
            jmp L0048911d
    L004890e1:
            or edx, 0xffffffff
            mov ecx, 1
            mov dword ptr [gGroove_funk_offset], edx
            mov eax, offset gFunk_groove_flags+0x4
    L004890f4:
            cmp dword ptr [eax], 0
            je L00489106
            add eax, 4
            inc ecx
            cmp eax, offset gFunk_groove_flags+0x78
            jl L004890f4
            jmp L00489123
    L00489106:
            lea edx, [ecx + ecx*8]
            mov dword ptr [ebp + 0x1ac], ecx
            mov dword ptr [ecx*4 + gFunk_groove_flags], 1
            shl edx, 2
    L0048911d:
            mov dword ptr [gGroove_funk_offset], edx
    L00489123:
            test edx, edx
            jge L00489131
            push 0x71
            call FatalError
            add esp, 4
    L00489131:
            mov edi, dword ptr [esp + 0x1c]
            mov esi, offset gNet_avail_names+0x7a0
            mov eax, edi
    L0048913c:
            mov dl, byte ptr [eax]
            mov bl, byte ptr [esi]
            mov cl, dl
            cmp dl, bl
            jne L00489164
            test cl, cl
            je L00489160
            mov dl, byte ptr [eax + 1]
            mov bl, byte ptr [esi + 1]
            mov cl, dl
            cmp dl, bl
            jne L00489164
            add eax, 2
            add esi, 2
            test cl, cl
            jne L0048913c
    L00489160:
            xor eax, eax
            jmp L00489169
    L00489164:
            sbb eax, eax
            sbb eax, -1
    L00489169:
            test eax, eax
            je L00489175
            fld dword ptr [gDouble_0058a068+0x1e8]
            jmp L0048917b
    L00489175:
            fld dword ptr [gDouble_0058a068+0x200]
    L0048917b:
            fstp dword ptr [ebp + 0x170c]
            push edi
            lea edx, [esp + 0x278]
            lea ecx, [esp + 0x178]
            call PathCat
            mov edx, offset gNext_crush_actor_id+0x180
            lea ecx, [esp + 0x174]
            call DRfopen
            test eax, eax
            mov dword ptr [esp + 0x10], eax
            jne L004891b8
            push 0x19
            call FatalError
            add esp, 4
    L004891b8:
            lea edi, [esp + 0x274]
            or ecx, 0xffffffff
            xor eax, eax
            lea edx, [esp + 0x174]
            repne scasb
            not ecx
            sub edi, ecx
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            shr ecx, 2
            rep movsd
            mov ecx, eax
            mov eax, dword ptr [gGraf_spec_index]
            and ecx, 3
            rep movsb
            lea ecx, [eax + eax*2]
            lea edx, [eax + ecx*4]
            lea ecx, [esp + 0x174]
            mov eax, dword ptr [edx*4 + gGraf_specs+0x1c]
            lea edx, [esp + 0x174]
            push eax
            call PathCat
            lea edx, [esp + 0x174]
            mov ecx, dword ptr [esp + 0x1c]
            push ecx
            lea ecx, [esp + 0x178]
            call PathCat
            call AllowOpenToFail
            mov edx, offset gNext_crush_actor_id+0x180
            lea ecx, [esp + 0x174]
            call DRfopen
            mov ebx, eax
            mov dword ptr [esp + 0x3c], ebx
            call DoNotAllowOpenToFail
            test ebx, ebx
            jne L004892bf
            push offset gNet_avail_names+0x7ac
            mov edx, offset gApplication_path
            lea ecx, [esp + 0x178]
            call PathCat
            mov eax, dword ptr [gGraf_spec_index]
            lea edx, [eax + eax*2]
            lea eax, [eax + edx*4]
            lea edx, [esp + 0x174]
            mov ecx, dword ptr [eax*4 + gGraf_specs+0x1c]
            push ecx
            lea ecx, [esp + 0x178]
            call PathCat
            lea edx, [esp + 0x174]
            lea ecx, [esp + 0x174]
            push offset gBasic_car_name
            call PathCat
            mov edx, offset gNext_crush_actor_id+0x180
            lea ecx, [esp + 0x174]
            call DRfopen
            mov ebx, eax
            test ebx, ebx
            mov dword ptr [esp + 0x3c], ebx
            jne L004892bf
            push 0x68
            call FatalError
            add esp, 4
    L004892bf:
            mov ecx, dword ptr [esp + 0x10]
            lea edx, [esp + 0x74]
            call GetALineAndDontArgue
            lea edx, [esp + 0x74]
            push offset gPed_flags_0058f2e4+0x138
            push edx
            call strtok
            add esp, 8
            mov edi, offset gDefault_drone_direction+0x5dc
            mov esi, eax
    L004892e5:
            mov cl, byte ptr [esi]
            mov dl, byte ptr [edi]
            mov al, cl
            cmp cl, dl
            jne L0048930d
            test al, al
            je L00489309
            mov dl, byte ptr [esi + 1]
            mov cl, byte ptr [edi + 1]
            mov al, dl
            cmp dl, cl
            jne L0048930d
            add esi, 2
            add edi, 2
            test al, al
            jne L004892e5
    L00489309:
            xor eax, eax
            jmp L00489312
    L0048930d:
            sbb eax, eax
            sbb eax, -1
    L00489312:
            test eax, eax
            jne L00489346
            push offset gPed_flags_0058f2e4+0x138
            push eax
            call strtok
            add esp, 8
            lea ecx, [esp + 0x40]
            push ecx
            push offset gPed_flags_0058f2e4+0x134
            push eax
            call sscanf
            mov ecx, dword ptr [esp + 0x1c]
            add esp, 0xc
            lea edx, [esp + 0x74]
            call GetAString
            jmp L0048934e
    L00489346:
            mov dword ptr [esp + 0x40], 0
    L0048934e:
            mov edi, offset gNet_avail_names+0x79c
            lea esi, [esp + 0x74]
    L00489357:
            mov dl, byte ptr [esi]
            mov cl, byte ptr [edi]
            mov al, dl
            cmp dl, cl
            jne L0048937f
            test al, al
            je L0048937b
            mov cl, byte ptr [esi + 1]
            mov dl, byte ptr [edi + 1]
            mov al, cl
            cmp cl, dl
            jne L0048937f
            add esi, 2
            add edi, 2
            test al, al
            jne L00489357
    L0048937b:
            xor eax, eax
            jmp L00489384
    L0048937f:
            sbb eax, eax
            sbb eax, -1
    L00489384:
            test eax, eax
            jne L00489390
            mov dword ptr [ebp + 0x18c4], eax
            jmp L004893d4
    L00489390:
            mov edi, offset gNet_avail_names+0x794
            lea esi, [esp + 0x74]
    L00489399:
            mov dl, byte ptr [esi]
            mov cl, byte ptr [edi]
            mov al, dl
            cmp dl, cl
            jne L004893c1
            test al, al
            je L004893bd
            mov cl, byte ptr [esi + 1]
            mov dl, byte ptr [edi + 1]
            mov al, cl
            cmp cl, dl
            jne L004893c1
            add esi, 2
            add edi, 2
            test al, al
            jne L00489399
    L004893bd:
            xor eax, eax
            jmp L004893c6
    L004893c1:
            sbb eax, eax
            sbb eax, -1
    L004893c6:
            test eax, eax
            jne L004893e1
            mov dword ptr [ebp + 0x18c4], 1
    L004893d4:
            mov ecx, dword ptr [esp + 0x10]
            lea edx, [esp + 0x74]
            call GetAString
    L004893e1:
            lea edi, [esp + 0x74]
            or ecx, 0xffffffff
            xor eax, eax
            lea edx, [ebp + 0xcc]
            repne scasb
            not ecx
            sub edi, ecx
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            mov edx, dword ptr [esp + 0x1c]
            shr ecx, 2
            rep movsd
            mov ecx, eax
            and ecx, 3
            rep movsb
            lea ecx, [esp + 0x74]
            call DRStricmp
            test eax, eax
            je L00489428
            mov ecx, dword ptr [esp + 0x1c]
            push ecx
            push 0x73
            call FatalError
            add esp, 8
    L00489428:
            mov esi, dword ptr [esp + 0x984]
            cmp byte ptr [esi], 0
            je L0048944a
            lea edi, [ebp + 0xec]
            mov ecx, 8
            rep movsd
            mov byte ptr [ebp + 0x10b], 0
            jmp L00489458
    L0048944a:
            mov byte ptr [ebp + 0xec], 0x58
            mov byte ptr [ebp + 0xed], 0
    L00489458:
            xor eax, eax
            lea ecx, [ebp + 0x18b4]
            mov dword ptr [ebp + 0x1c8], eax
            mov dword ptr [ebp + 0x1cc], eax
            mov dword ptr [ebp + 0x1d4], eax
            mov dword ptr [ebp + 0x13e4], eax
            mov dword ptr [ebp + 0x13e8], eax
            mov dword ptr [ebp + 4], eax
            mov dword ptr [ebp + 0x1d0], 1
    L0048948b:
            mov edx, dword ptr [eax + gCurrent_APO_levels]
            add eax, 4
            mov dword ptr [ecx - 0xc], edx
            mov edx, dword ptr [eax + gCamera_to_world+0x2c]
            mov dword ptr [ecx], edx
            add ecx, 4
            cmp eax, 0xc
            jl L0048948b
            mov edi, dword ptr [esp + 0x10]
            mov ecx, edi
            call GetAFloat
            fstp dword ptr [ebp + 0x18d0]
            lea edx, [esp + 0x74]
            mov ecx, edi
            call GetALineAndDontArgue
            cmp dword ptr [esp + 0x18], 8
            jne L00489f55
            mov eax, dword ptr [esp + 0x4c]
            test eax, eax
            jl L004894db
            call PackFileRevertTiffLoading
    L004894db:
            lea esi, [ebp + 0x1dc]
            mov edi, 3
    L004894e6:
            lea edx, [esp + 0x74]
            mov ecx, ebx
            call GetALineAndDontArgue
            lea eax, [esp + 0x74]
            push offset gPed_flags_0058f2e4+0x138
            push eax
            call strtok
            add esp, 8
            lea edx, [esp + 0x74]
            mov ecx, ebx
            mov dword ptr [esi - 0xa4], 0
            call GetALineAndDontArgue
            lea ecx, [esp + 0x74]
            push offset gPed_flags_0058f2e4+0x138
            push ecx
            call strtok
            add esp, 8
            push esi
            push offset gPed_flags_0058f2e4+0x134
            push eax
            call sscanf
            add esp, 0xc
            push offset gPed_flags_0058f2e4+0x138
            push 0
            call strtok
            add esp, 8
            lea edx, [esi + 0xc]
            push edx
            push offset gPed_flags_0058f2e4+0x134
            push eax
            call sscanf
            add esp, 0xc
            push offset gPed_flags_0058f2e4+0x138
            push 0
            call strtok
            add esp, 8
            lea ecx, [esi + 0x18]
            push ecx
            push offset gPed_flags_0058f2e4+0x134
            push eax
            call sscanf
            add esp, 0xc
            push offset gPed_flags_0058f2e4+0x138
            push 0
            call strtok
            add esp, 8
            lea edx, [esi + 0x24]
            push edx
            push offset gPed_flags_0058f2e4+0x134
            push eax
            call sscanf
            add esp, 0xc
            call PossibleService
            add esi, 4
            dec edi
            jne L004894e6
            push ebp
            xor edx, edx
            mov ecx, ebx
            call LoadSpeedo
            lea edx, [esp + 0x74]
            mov ecx, ebx
            call GetALineAndDontArgue
            call PossibleService
            xor edx, edx
            mov ecx, ebx
            push ebp
            call LoadTacho
            lea edx, [esp + 0x74]
            mov ecx, ebx
            call GetALineAndDontArgue
            call PossibleService
            lea edx, [esp + 0x474]
            mov ecx, ebx
            call GetALineAndDontArgue
            lea eax, [esp + 0x474]
            push offset gPed_flags_0058f2e4+0x138
            push eax
            call strtok
            add esp, 8
            lea ecx, [ebp + 0x2dc]
            push ecx
            push offset gPed_flags_0058f2e4+0x134
            push eax
            call sscanf
            add esp, 0xc
            push offset gPed_flags_0058f2e4+0x138
            push 0
            call strtok
            add esp, 8
            lea edx, [ebp + 0x2e4]
            push edx
            push offset gPed_flags_0058f2e4+0x134
            push eax
            call sscanf
            add esp, 0xc
            push offset gPed_flags_0058f2e4+0x138
            push 0
            call strtok
            add esp, 8
            mov ecx, eax
            call LoadPixelmap
            lea edx, [esp + 0x74]
            mov ecx, ebx
            mov dword ptr [ebp + 0x1a8], eax
            call GetALineAndDontArgue
            call PossibleService
            lea edx, [esp + 0x74]
            mov ecx, ebx
            call GetALineAndDontArgue
            lea eax, [esp + 0x74]
            push offset gPed_flags_0058f2e4+0x138
            push eax
            call strtok
            add esp, 8
            lea esi, [ebp + 0x360]
            push esi
            push offset gPed_flags_0058f2e4+0x134
            push eax
            call sscanf
            mov eax, dword ptr [esi]
            add esp, 0xc
            xor edi, edi
            test eax, eax
            jle L0048977c
            lea esi, [ebp + 0x30c]
    L004896ab:
            lea edx, [esp + 0x74]
            mov ecx, ebx
            call GetALineAndDontArgue
            lea ecx, [esp + 0x74]
            push offset gPed_flags_0058f2e4+0x138
            push ecx
            call strtok
            add esp, 8
            lea edx, [esi - 0x1c]
            push edx
            push offset gPed_flags_0058f2e4+0x134
            push eax
            call sscanf
            add esp, 0xc
            push offset gPed_flags_0058f2e4+0x138
            push 0
            call strtok
            add esp, 8
            push esi
            push offset gPed_flags_0058f2e4+0x134
            push eax
            call sscanf
            add esp, 0xc
            push offset gPed_flags_0058f2e4+0x138
            push 0
            call strtok
            add esp, 8
            mov ecx, eax
            call LoadPixelmap
            mov dword ptr [esi - 0x1a0], eax
            push offset gPed_flags_0058f2e4+0x138
            push 0
            call strtok
            add esp, 8
            lea ecx, [esi + 0x1c]
            push ecx
            push offset gPed_flags_0058f2e4+0x134
            push eax
            call sscanf
            add esp, 0xc
            push offset gPed_flags_0058f2e4+0x138
            push 0
            call strtok
            add esp, 8
            lea edx, [esi + 0x38]
            push edx
            push offset gPed_flags_0058f2e4+0x134
            push eax
            call sscanf
            add esp, 0xc
            push offset gPed_flags_0058f2e4+0x138
            push 0
            call strtok
            add esp, 8
            call PossibleService
            mov eax, dword ptr [ebp + 0x360]
            inc edi
            add esi, 4
            cmp edi, eax
            jl L004896ab
    L0048977c:
            mov esi, dword ptr [esp + 0x10]
            lea edx, [esp + 0x74]
            mov ecx, esi
            mov dword ptr [ebp + 0x2ec], 0x1f40
            call GetALineAndDontArgue
            lea eax, [esp + 0x74]
            push offset gPed_flags_0058f2e4+0x138
            push eax
            call strtok
            add esp, 8
            lea ecx, [ebp + 0x47c]
            push ecx
            push offset gMini_map_glowing_line_animation_indices+0x7dc
            push eax
            call sscanf
            add esp, 0xc
            push offset gPed_flags_0058f2e4+0x138
            push 0
            call strtok
            add esp, 8
            lea edx, [ebp + 0x480]
            push edx
            push offset gMini_map_glowing_line_animation_indices+0x7dc
            push eax
            call sscanf
            add esp, 0xc
            push offset gPed_flags_0058f2e4+0x138
            push 0
            call strtok
            add esp, 8
            lea ecx, [ebp + 0x484]
            push ecx
            push offset gMini_map_glowing_line_animation_indices+0x7dc
            push eax
            call sscanf
            add esp, 0xc
            lea edx, [esp + 0x74]
            mov ecx, esi
            call GetALineAndDontArgue
            lea edx, [esp + 0x74]
            push offset gPed_flags_0058f2e4+0x138
            push edx
            call strtok
            add esp, 8
            lea ecx, [ebp + 0x498]
            push ecx
            push offset gMini_map_glowing_line_animation_indices+0x7dc
            push eax
            call sscanf
            add esp, 0xc
            push offset gPed_flags_0058f2e4+0x138
            push 0
            call strtok
            add esp, 8
            lea edx, [ebp + 0x49c]
            push edx
            push offset gMini_map_glowing_line_animation_indices+0x7dc
            push eax
            call sscanf
            add esp, 0xc
            lea edx, [esp + 0x74]
            mov ecx, esi
            call GetALineAndDontArgue
            lea eax, [esp + 0x74]
            push offset gPed_flags_0058f2e4+0x138
            push eax
            call strtok
            add esp, 8
            lea ecx, [ebp + 0x488]
            push ecx
            push offset gMini_map_glowing_line_animation_indices+0x7dc
            push eax
            call sscanf
            add esp, 0xc
            push offset gPed_flags_0058f2e4+0x138
            push 0
            call strtok
            add esp, 8
            lea edx, [ebp + 0x48c]
            push edx
            push offset gMini_map_glowing_line_animation_indices+0x7dc
            push eax
            call sscanf
            add esp, 0xc
            push offset gPed_flags_0058f2e4+0x138
            push 0
            call strtok
            add esp, 8
            lea ecx, [ebp + 0x490]
            push ecx
            push offset gMini_map_glowing_line_animation_indices+0x7dc
            push eax
            call sscanf
            add esp, 0xc
            push offset gPed_flags_0058f2e4+0x138
            push 0
            call strtok
            add esp, 8
            lea edx, [ebp + 0x494]
            push edx
            push offset gMini_map_glowing_line_animation_indices+0x7dc
            push eax
            call sscanf
            add esp, 0xc
            lea edx, [esp + 0x74]
            mov ecx, ebx
            call GetALineAndDontArgue
            lea eax, [esp + 0x74]
            push offset gPed_flags_0058f2e4+0x138
            push eax
            call strtok
            add esp, 8
            lea ecx, [ebp + 0x20c]
            push ecx
            push offset gPed_flags_0058f2e4+0x134
            push eax
            call sscanf
            add esp, 0xc
            push offset gPed_flags_0058f2e4+0x138
            push 0
            call strtok
            add esp, 8
            lea edx, [ebp + 0x210]
            push edx
            push offset gPed_flags_0058f2e4+0x134
            push eax
            call sscanf
            add esp, 0xc
            push offset gPed_flags_0058f2e4+0x138
            push 0
            call strtok
            add esp, 8
            lea ecx, [ebp + 0x214]
            push ecx
            push offset gPed_flags_0058f2e4+0x134
            push eax
            call sscanf
            add esp, 0xc
            push offset gPed_flags_0058f2e4+0x138
            push 0
            call strtok
            add esp, 8
            lea edx, [ebp + 0x218]
            push edx
            push offset gPed_flags_0058f2e4+0x134
            push eax
            call sscanf
            add esp, 0xc
            lea edx, [esp + 0x74]
            mov ecx, ebx
            call GetALineAndDontArgue
            lea eax, [esp + 0x74]
            push offset gPed_flags_0058f2e4+0x138
            push eax
            call strtok
            add esp, 8
            lea ecx, [ebp + 0x21c]
            push ecx
            push offset gPed_flags_0058f2e4+0x134
            push eax
            call sscanf
            add esp, 0xc
            push offset gPed_flags_0058f2e4+0x138
            push 0
            call strtok
            add esp, 8
            lea edx, [ebp + 0x220]
            push edx
            push offset gPed_flags_0058f2e4+0x134
            push eax
            call sscanf
            add esp, 0xc
            push offset gPed_flags_0058f2e4+0x138
            push 0
            call strtok
            add esp, 8
            lea ecx, [ebp + 0x224]
            push ecx
            push offset gPed_flags_0058f2e4+0x134
            push eax
            call sscanf
            add esp, 0xc
            push offset gPed_flags_0058f2e4+0x138
            push 0
            call strtok
            add esp, 8
            lea edx, [ebp + 0x228]
            push edx
            push offset gPed_flags_0058f2e4+0x134
            push eax
            call sscanf
            add esp, 0xc
            lea edx, [esp + 0x74]
            mov ecx, esi
            call GetALineAndDontArgue
            call PossibleService
            lea eax, [esp + 0x74]
            push offset gPed_flags_0058f2e4+0x138
            push eax
            call strtok
            add esp, 8
            mov ecx, eax
            call LoadPixelmap
            mov dword ptr [ebp + 0x144], eax
            push offset gPed_flags_0058f2e4+0x138
            push 0
            call strtok
            add esp, 8
            mov ecx, eax
            call LoadPixelmap
            mov dword ptr [ebp + 0x148], eax
            push offset gPed_flags_0058f2e4+0x138
            push 0
            call strtok
            add esp, 8
            mov ecx, eax
            call LoadPixelmap
            mov dword ptr [ebp + 0x14c], eax
            push offset gPed_flags_0058f2e4+0x138
            push 0
            call strtok
            add esp, 8
            mov ecx, eax
            call LoadPixelmap
            mov dword ptr [ebp + 0x150], eax
            call PossibleService
            xor esi, esi
    L00489abe:
            cmp esi, 2
            je L00489baa
            mov ecx, dword ptr [esp + 0x3c]
            lea edx, [esp + 0x74]
            call GetALineAndDontArgue
            lea ecx, [esp + 0x74]
            push offset gPed_flags_0058f2e4+0x138
            push ecx
            call strtok
            lea edx, [esi + esi*4]
            add esp, 8
            lea ecx, [esi + edx*2]
            lea edi, [ebp + ecx*4]
            lea edx, [edi + 0x4d8]
            push edx
            push offset gPed_flags_0058f2e4+0x134
            push eax
            call sscanf
            add esp, 0xc
            push offset gPed_flags_0058f2e4+0x138
            push 0
            call strtok
            add esp, 8
            lea ecx, [edi + 0x4dc]
            push ecx
            push offset gPed_flags_0058f2e4+0x134
            push eax
            call sscanf
            add esp, 0xc
            lea ebx, [edi + 0x4ec]
            mov dword ptr [esp + 0x20], 5
    L00489b37:
            push offset gPed_flags_0058f2e4+0x138
            push 0
            call strtok
            add esp, 8
            lea edx, [esp + 0x64]
            push edx
            push offset gMini_map_glowing_line_animation_indices+0x7dc
            push eax
            call sscanf
            fld dword ptr [esp + 0x70]
            fdivr qword ptr [gDouble_0058a068+0x208]
            add esp, 0xc
            fmul qword ptr [gDouble_0058a068+0x210]
            call _ftol
            mov dword ptr [ebx], eax
            mov eax, dword ptr [esp + 0x20]
            add ebx, 4
            dec eax
            mov dword ptr [esp + 0x20], eax
            jne L00489b37
            push offset gPed_flags_0058f2e4+0x138
            push 0
            call strtok
            add esp, 8
            mov ecx, eax
            call LoadPixelmap
            test eax, eax
            mov dword ptr [edi + 0x500], eax
            jne L00489bb4
            push 0x1b
            call FatalError
            add esp, 4
            jmp L00489bb4
    L00489baa:
            mov dword ptr [ebp + 0x558], 0
    L00489bb4:
            lea eax, [esi + esi*4]
            lea ecx, [esi + eax*2]
            mov dword ptr [ebp + ecx*4 + 0x4e0], 0
            call PossibleService
            inc esi
            cmp esi, 0xc
            jl L00489abe
            mov esi, dword ptr [esp + 0x3c]
            lea edx, [esp + 0x74]
            mov ecx, esi
            call GetALineAndDontArgue
            lea edx, [esp + 0x74]
            push offset gPed_flags_0058f2e4+0x138
            push edx
            call strtok
            add esp, 8
            lea ecx, [ebp + 0x368]
            push ecx
            push offset gPed_flags_0058f2e4+0x134
            push eax
            call sscanf
            add esp, 0xc
            push offset gPed_flags_0058f2e4+0x138
            push 0
            call strtok
            add esp, 8
            lea edx, [ebp + 0x36c]
            push edx
            push offset gPed_flags_0058f2e4+0x134
            push eax
            call sscanf
            add esp, 0xc
            push offset gPed_flags_0058f2e4+0x138
            push 0
            call strtok
            add esp, 8
            lea ecx, [ebp + 0x370]
            push ecx
            push offset gPed_flags_0058f2e4+0x134
            push eax
            call sscanf
            add esp, 0xc
            push offset gPed_flags_0058f2e4+0x138
            push 0
            call strtok
            add esp, 8
            lea edx, [ebp + 0x374]
            push edx
            push offset gPed_flags_0058f2e4+0x134
            push eax
            call sscanf
            add esp, 0xc
            push offset gPed_flags_0058f2e4+0x138
            push 0
            call strtok
            add esp, 8
            mov ecx, eax
            call LoadPixelmap
            mov ecx, esi
            mov dword ptr [ebp + 0x168], eax
            lea ebx, [ebp + 0x378]
            call GetAnInt
            mov dword ptr [ebx], eax
            lea eax, [ebp + 0x3c0]
            mov dword ptr [esp + 0x20], eax
            mov dword ptr [esp + 0x14], 2
    L00489cb3:
            mov eax, dword ptr [ebx]
            xor edi, edi
            test eax, eax
            jle L00489cde
            mov esi, dword ptr [esp + 0x20]
    L00489cbf:
            lea ecx, [esi + 0x20]
            lea edx, [esi - 0x20]
            push ecx
            mov ecx, dword ptr [esp + 0x40]
            push esi
            push edx
            lea edx, [esi - 0x40]
            call GetFourInts
            mov eax, dword ptr [ebx]
            inc edi
            add esi, 4
            cmp edi, eax
            jl L00489cbf
    L00489cde:
            mov ecx, dword ptr [esp + 0x20]
            mov eax, dword ptr [esp + 0x14]
            add ebx, 4
            add ecx, 0x10
            dec eax
            mov dword ptr [esp + 0x20], ecx
            mov dword ptr [esp + 0x14], eax
            jne L00489cb3
            mov eax, dword ptr [gGraf_spec_index]
            lea ecx, [eax + eax*2]
            lea edx, [eax + ecx*4]
            lea ecx, [esp + 0x174]
            mov eax, dword ptr [edx*4 + gGraf_specs+0x1c]
            mov edx, offset gApplication_path
            push eax
            call PathCat
            lea edx, [esp + 0x174]
            lea ecx, [esp + 0x174]
            push offset gNet_avail_names+0x788
            call PathCat
            mov edx, offset gNext_crush_actor_id+0x180
            lea ecx, [esp + 0x174]
            call DRfopen
            mov esi, eax
            test esi, esi
            jne L00489d54
            push 0x66
            call FatalError
            add esp, 4
    L00489d54:
            call PossibleService
            xor edx, edx
            mov ecx, esi
            push ebp
            call LoadHeadups
            mov edx, 1
            mov ecx, esi
            push ebp
            call LoadHeadups
            call PossibleService
            mov ecx, esi
            call PFfclose
            mov eax, dword ptr [gCurrent_graf_data]
            mov ecx, offset gProgram_state+0x294
    L00489d86:
            mov edx, dword ptr [eax + 0x104]
            mov edi, dword ptr [ecx - 0xc]
            sub edi, edx
            mov dword ptr [ecx - 0xc], edi
            mov edx, dword ptr [eax + 0x108]
            mov esi, dword ptr [ecx]
            add ecx, 4
            sub esi, edx
            mov dword ptr [ecx - 4], esi
            mov edx, dword ptr [eax + 0x104]
            mov ebx, dword ptr [ecx + 8]
            sub ebx, edx
            mov dword ptr [ecx + 8], ebx
            mov edx, dword ptr [eax + 0x108]
            mov edi, dword ptr [ecx + 0x14]
            sub edi, edx
            cmp ecx, offset gProgram_state+0x2a0
            mov dword ptr [ecx + 0x14], edi
            jl L00489d86
            mov ecx, dword ptr [eax + 0x104]
            mov edi, dword ptr [gProgram_state+0x2b8]
            mov esi, dword ptr [gProgram_state+0x2bc]
            sub edi, ecx
            mov dword ptr [gProgram_state+0x2b8], edi
            mov edx, dword ptr [eax + 0x108]
            sub esi, edx
            mov edx, dword ptr [gProgram_state+0x2c0]
            mov ebx, dword ptr [gProgram_state+0x2c4]
            mov dword ptr [gProgram_state+0x2bc], esi
            mov ecx, dword ptr [eax + 0x104]
            mov edi, dword ptr [gProgram_state+0x2ec]
            sub edx, ecx
            mov esi, dword ptr [gProgram_state+0x2f4]
            mov dword ptr [gProgram_state+0x2c0], edx
            mov edx, dword ptr [eax + 0x108]
            sub ebx, edx
            mov dword ptr [gProgram_state+0x2c4], ebx
            mov ecx, dword ptr [eax + 0x104]
            sub edi, ecx
            mov ebx, dword ptr [gProgram_state+0x34c]
            mov dword ptr [gProgram_state+0x2ec], edi
            mov edx, dword ptr [eax + 0x108]
            sub esi, edx
            mov edx, dword ptr [gProgram_state+0x344]
            mov dword ptr [gProgram_state+0x2f4], esi
            mov ecx, dword ptr [eax + 0x104]
            sub edx, ecx
            mov edi, dword ptr [gProgram_state+0x2dc]
            mov dword ptr [gProgram_state+0x344], edx
            mov edx, dword ptr [eax + 0x108]
            sub ebx, edx
            mov esi, dword ptr [gProgram_state+0x2e4]
            mov dword ptr [gProgram_state+0x34c], ebx
            mov ecx, dword ptr [eax + 0x104]
            sub edi, ecx
            mov dword ptr [gProgram_state+0x2dc], edi
            mov edx, dword ptr [eax + 0x108]
            sub esi, edx
            mov edx, dword ptr [gProgram_state+0x334]
            mov dword ptr [gProgram_state+0x2e4], esi
            mov ecx, dword ptr [eax + 0x104]
            sub edx, ecx
            mov ecx, dword ptr [gProgram_state+0x33c]
            mov dword ptr [gProgram_state+0x334], edx
            mov edx, dword ptr [eax + 0x108]
            sub ecx, edx
            mov dword ptr [gProgram_state+0x33c], ecx
            mov ecx, offset gProgram_state+0x3b8
    L00489ebd:
            mov edx, dword ptr [eax + 0x104]
            mov edi, dword ptr [ecx - 0x1c]
            sub edi, edx
            mov dword ptr [ecx - 0x1c], edi
            mov edx, dword ptr [eax + 0x108]
            mov esi, dword ptr [ecx]
            add ecx, 4
            sub esi, edx
            mov dword ptr [ecx - 4], esi
            mov edx, dword ptr [eax + 0x104]
            mov ebx, dword ptr [ecx + 0x18]
            sub ebx, edx
            mov dword ptr [ecx + 0x18], ebx
            mov edx, dword ptr [eax + 0x108]
            mov edi, dword ptr [ecx + 0x34]
            sub edi, edx
            cmp ecx, offset gProgram_state+0x3d4
            mov dword ptr [ecx + 0x34], edi
            jl L00489ebd
            mov ecx, offset gProgram_state+0x588
    L00489f04:
            mov edx, dword ptr [eax + 0x104]
            mov ebx, dword ptr [ecx - 4]
            sub ebx, edx
            mov dword ptr [ecx - 4], ebx
            mov edx, dword ptr [eax + 0x108]
            mov edi, dword ptr [ecx]
            add ecx, 0x2c
            sub edi, edx
            cmp ecx, offset gProgram_state+0x798
            mov dword ptr [ecx - 0x2c], edi
            jl L00489f04
            call AdjustRenderScreenSize
            call PossibleService
            call ReinitialiseRearviewCamera
            mov ecx, dword ptr [esp + 0x10]
            lea edx, [esp + 0x74]
            call GetALineAndDontArgue
            mov eax, dword ptr [esp + 0x4c]
            test eax, eax
            jl L00489faf
            call PackFileRerevertTiffLoading
            jmp L00489faf
    L00489f55:
            lea edx, [esp + 0x74]
            mov ecx, edi
            call GetALineAndDontArgue
            mov ecx, edi
            call PFfeof
            test eax, eax
            jne L00489fa5
            mov esi, offset gNet_avail_names+0x770
            lea eax, [esp + 0x74]
    L00489f74:
            mov dl, byte ptr [eax]
            mov bl, byte ptr [esi]
            mov cl, dl
            cmp dl, bl
            jne L00489f9c
            test cl, cl
            je L00489f98
            mov dl, byte ptr [eax + 1]
            mov bl, byte ptr [esi + 1]
            mov cl, dl
            cmp dl, bl
            jne L00489f9c
            add eax, 2
            add esi, 2
            test cl, cl
            jne L00489f74
    L00489f98:
            xor eax, eax
            jmp L00489fa1
    L00489f9c:
            sbb eax, eax
            sbb eax, -1
    L00489fa1:
            test eax, eax
            jne L00489f55
    L00489fa5:
            mov dword ptr [ebp + 0x2ec], 0x1f40
    L00489faf:
            call PossibleService
            mov esi, dword ptr [esp + 0x10]
            lea eax, [ebp + 0x478]
            lea ecx, [ebp + 0x474]
            push eax
            push ecx
            lea edx, [ebp + 0x470]
            mov ecx, esi
            call GetThreeInts
            lea edx, [esp + 0x74]
            mov ecx, esi
            call GetAString
            mov esi, offset gNet_avail_names+0x764
            lea eax, [esp + 0x74]
    L00489fe7:
            mov dl, byte ptr [eax]
            mov bl, byte ptr [esi]
            mov cl, dl
            cmp dl, bl
            jne L0048a00f
            test cl, cl
            je L0048a00b
            mov dl, byte ptr [eax + 1]
            mov bl, byte ptr [esi + 1]
            mov cl, dl
            cmp dl, bl
            jne L0048a00f
            add eax, 2
            add esi, 2
            test cl, cl
            jne L00489fe7
    L0048a00b:
            xor eax, eax
            jmp L0048a014
    L0048a00f:
            sbb eax, eax
            sbb eax, -1
    L0048a014:
            mov ebx, dword ptr [esp + 0x10]
            xor ecx, ecx
            test eax, eax
            sete cl
            mov dword ptr [ebp + 0x1c8], ecx
            push 0
            mov edx, ebp
            mov ecx, ebx
            call GetDamageProgram
            mov edx, ebp
            mov ecx, ebx
            push 1
            call GetDamageProgram
            mov edx, ebp
            mov ecx, ebx
            push 2
            call GetDamageProgram
            mov edx, ebp
            mov ecx, ebx
            push 3
            call GetDamageProgram
            mov edx, ebp
            mov ecx, ebx
            push 4
            call GetDamageProgram
            mov edx, ebp
            mov ecx, ebx
            push 5
            call GetDamageProgram
            lea edx, [esp + 0x74]
            mov ecx, ebx
            call GetALineAndDontArgue
            lea edx, [esp + 0x74]
            push offset gPed_flags_0058f2e4+0x138
            push edx
            call strtok
            mov edi, eax
            or ecx, 0xffffffff
            xor eax, eax
            add esp, 8
            repne scasb
            not ecx
            sub edi, ecx
            lea edx, [ebp + 0x10c]
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            push offset gPed_flags_0058f2e4+0x138
            shr ecx, 2
            rep movsd
            mov ecx, eax
            push 0
            and ecx, 3
            rep movsb
            call strtok
            mov edi, eax
            or ecx, 0xffffffff
            xor eax, eax
            add esp, 8
            repne scasb
            not ecx
            sub edi, ecx
            lea edx, [ebp + 0x11a]
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            push offset gPed_flags_0058f2e4+0x138
            shr ecx, 2
            rep movsd
            mov ecx, eax
            push 0
            and ecx, 3
            rep movsb
            call strtok
            mov edi, eax
            or ecx, 0xffffffff
            xor eax, eax
            add esp, 8
            repne scasb
            lea edx, [ebp + 0x128]
            not ecx
            sub edi, ecx
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            lea edx, [esp + 0x274]
            shr ecx, 2
            rep movsd
            mov ecx, eax
            and ecx, 3
            rep movsb
            mov esi, dword ptr [esp + 0x988]
            mov dword ptr [ebp + 0x1a4], 0
            mov ecx, esi
            call LoadAllImagesInDirectory
            lea edx, [esp + 0x274]
            mov ecx, esi
            call LoadAllShadeTablesInDirectory
            lea edx, [esp + 0x274]
            mov ecx, esi
            push ebp
            call LoadCarMaterials
            mov ecx, dword ptr [esi + 4]
            lea edx, [esp + 0x74]
            mov dword ptr [esp + 0x20], ecx
            mov ecx, ebx
            call GetALineAndDontArgue
            lea edx, [esp + 0x74]
            push offset gPed_flags_0058f2e4+0x138
            push edx
            call strtok
            add esp, 8
            lea ebx, [ebp + 0xe14]
            push ebx
            push offset gPed_flags_0058f2e4+0x134
            push eax
            call sscanf
            mov ecx, dword ptr [ebx]
            add esp, 0xc
            inc ecx
            mov eax, ecx
            mov dword ptr [ebx], ecx
            cmp eax, 2
            jle L0048a19c
            mov ecx, offset gNet_avail_names+0x748
            call PDFatalError
    L0048a19c:
            mov dword ptr [ebp + 0xe1c], 0
            mov eax, dword ptr [ebx]
            mov esi, 1
            cmp eax, esi
            jle L0048a1cc
            lea edi, [ebp + 0xe20]
    L0048a1b7:
            mov ecx, dword ptr [esp + 0x10]
            call GetAScalar
            _emit 0xd9
        _emit 0x1f
            mov eax, dword ptr [ebx]
            inc esi
            add edi, 4
            cmp esi, eax
            jl L0048a1b7
    L0048a1cc:
            mov edi, dword ptr [esp + 0x988]
            lea edx, [esp + 0x274]
            mov ecx, edi
            call LoadAllModelsInDirectory
            mov eax, dword ptr [esp + 0x40]
            test eax, eax
            jle L0048a296
            call PossibleService
            mov ecx, dword ptr [esp + 0x10]
            lea edx, [esp + 0x74]
            call GetALineAndDontArgue
            lea eax, [esp + 0x74]
            push offset gPed_flags_0058f2e4+0x138
            push eax
            call strtok
            add esp, 8
            lea edx, [esp + 0x274]
            lea ecx, [esp + 0x174]
            push eax
            call PathCat
            lea esi, [ebp + 0x18d4]
            lea edx, [esp + 0x174]
            push esi
            push edi
            lea ecx, [esp + 0x57c]
            call LoadCarCrush
            test eax, eax
            je L0048a296
            mov ecx, dword ptr [esp + 0x1c]
            lea edx, [esp + 0x274]
            push ecx
            lea ecx, [esp + 0x178]
            call PathCat
            lea edi, [esp + 0x174]
            or ecx, 0xffffffff
            xor eax, eax
            mov edx, dword ptr [gNet_avail_names+0x744]
            repne scasb
            mov eax, dword ptr [esp + 0x988]
            push esi
            not ecx
            dec ecx
            push eax
            mov dword ptr [esp + ecx + 0x179], edx
            lea edx, [esp + 0x17c]
            lea ecx, [esp + 0x57c]
            call LoadCarCrush
    L0048a296:
            push 0
            push 0
            call BrActorAllocate
            add esp, 8
            mov dword ptr [ebp + 0x10], eax
            mov ecx, dword ptr [gNon_track_actor]
            push eax
            push ecx
            call BrActorAdd
            mov eax, dword ptr [esp + 0x20]
            add esp, 8
            cmp eax, 8
            jne L0048a2c7
            mov edx, dword ptr [ebp + 0x10]
            mov dword ptr [gPlayer_car_master_actor], edx
    L0048a2c7:
            lea eax, [esp + 0x778]
            lea edx, [esp + 0x274]
            push eax
            lea ecx, [esp + 0x67c]
            call PathCat
            mov edi, offset gDefault_drone_direction+0x6dc
            or ecx, 0xffffffff
            xor eax, eax
            lea edx, [esp + 0x678]
            repne scasb
            not ecx
            sub edi, ecx
            mov esi, edi
            mov edi, edx
            mov edx, ecx
            or ecx, 0xffffffff
            repne scasb
            mov ecx, edx
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, edx
            lea eax, [esp + 0x678]
            and ecx, 3
            push eax
            rep movsb
            call BrActorLoad
            add esp, 4
            mov dword ptr [ebp + 0xe0c], eax
            test eax, eax
            jne L0048a337
            push 0x47
            call FatalError
            add esp, 4
    L0048a337:
            mov ecx, dword ptr [ebp + 0xe0c]
            mov edx, dword ptr [ebp + 0x10]
            push ecx
            push edx
            call BrActorAdd
            mov ecx, dword ptr [ebp + 0xe0c]
            add esp, 8
            mov edx, offset LinkCrushData
            push 0
            call DRActorEnumRecurse
            mov ebx, dword ptr [ebx]
            mov ecx, dword ptr [ebp + 0xe0c]
            mov eax, dword ptr [esp + 0x20]
            mov dword ptr [esp + 0x34], ebx
            mov ebx, dword ptr [esp + 0x988]
            lea esi, [eax*4]
            mov edx, dword ptr [ebx + 0x2c]
            add edx, esi
            mov dword ptr [esp + 0x2c], edx
            mov edx, dword ptr [ebx + 4]
            sub edx, eax
            lea eax, [esp + 0x2c]
            mov dword ptr [esp + 0x30], edx
            push eax
            mov edx, offset LinkCrushModel
            call DRActorEnumRecurse
            mov edi, dword ptr [ebx + 4]
            xor edx, edx
            mov eax, dword ptr [esp + 0x20]
            sub edi, eax
            mov eax, dword ptr [ebx + 0x2c]
            add eax, esi
            test edi, edi
            mov dword ptr [esp + 0x24], eax
            jle L0048a409
            mov dword ptr [esp + 0x14], eax
    L0048a3b7:
            mov ecx, dword ptr [eax]
            mov esi, offset gNet_avail_names+0x73c
            mov eax, dword ptr [ecx + 4]
    L0048a3c1:
            mov bl, byte ptr [eax]
            mov cl, bl
            cmp bl, byte ptr [esi]
            jne L0048a3e5
            test cl, cl
            je L0048a3e1
            mov bl, byte ptr [eax + 1]
            mov cl, bl
            cmp bl, byte ptr [esi + 1]
            jne L0048a3e5
            add eax, 2
            add esi, 2
            test cl, cl
            jne L0048a3c1
    L0048a3e1:
            xor eax, eax
            jmp L0048a3ea
    L0048a3e5:
            sbb eax, eax
            sbb eax, -1
    L0048a3ea:
            test eax, eax
            je L0048b54d
            mov eax, dword ptr [esp + 0x14]
            inc edx
            add eax, 4
            cmp edx, edi
            mov dword ptr [esp + 0x14], eax
            jl L0048a3b7
            mov ebx, dword ptr [esp + 0x988]
    L0048a409:
            xor eax, eax
    L0048a40b:
            push 0
            push 1
            mov dword ptr [ebp + 0xe28], eax
            call BrActorAllocate
            mov dword ptr [ebp + 0xe10], eax
            mov ecx, dword ptr [ebp + 0xe0c]
            add esp, 8
            mov ecx, dword ptr [ecx + 0x60]
            test ecx, ecx
            je L0048a43b
            mov ecx, dword ptr [ecx]
            test ecx, ecx
            je L0048a43b
            mov dword ptr [eax + 0x18], ecx
            jmp L0048a445
    L0048a43b:
            mov ecx, offset gNet_avail_names+0x720
            call PDFatalError
    L0048a445:
            mov edx, dword ptr [ebp + 0xe0c]
            mov eax, dword ptr [ebp + 0xe10]
            add edx, 0x2c
            add eax, 0x2c
            push edx
            push eax
            call BrMatrix34Copy
            mov eax, dword ptr [esp + 0x48]
            add esp, 8
            test eax, eax
            jle L0048a47b
            mov ecx, dword ptr [ebp + 0xe0c]
            lea edx, [esp + 0x574]
            call LinkCarCrushDatas
    L0048a47b:
            mov ecx, dword ptr [ebp + 0xe0c]
            push ebp
            mov edx, offset AllocateUserDetailLevel
            call DRActorEnumRecurse
            call PossibleService
            mov esi, dword ptr [esp + 0x20]
            mov eax, dword ptr [ebx + 4]
            cmp esi, eax
            jge L0048a4f2
            mov edi, dword ptr [esp + 0x980]
    L0048a4a3:
            mov ecx, dword ptr [ebx + 0x2c]
            mov eax, dword ptr [ecx + esi*4]
            test eax, eax
            je L0048a4ea
            cmp word ptr [eax + 0x12], 0
            je L0048a4ea
            cmp edi, 0x62
            je L0048a4c9
            mov ecx, dword ptr [gAusterity_mode]
            test ecx, ecx
            jne L0048a4c9
            or byte ptr [eax + 0x20], 0x83
            jmp L0048a4dc
    L0048a4c9:
            mov cx, word ptr [eax + 0x20]
            test cl, 0x80
            je L0048a4ea
            and ecx, 0xff7d
            mov word ptr [eax + 0x20], cx
    L0048a4dc:
            push 0x7fff
            push eax
            call BrModelUpdate
            add esp, 8
    L0048a4ea:
            mov eax, dword ptr [ebx + 4]
            inc esi
            cmp esi, eax
            jl L0048a4a3
    L0048a4f2:
            mov esi, dword ptr [esp + 0x10]
            lea edx, [esp + 0x74]
            mov ecx, esi
            call GetAString
            cmp dword ptr [esp + 0x40], 2
            jl L0048a50f
            mov ecx, esi
            call GetAnInt
    L0048a50f:
            mov ecx, dword ptr [esp + 0x10]
            lea edx, [esp + 0x74]
            call GetALineAndDontArgue
            lea edx, [esp + 0x74]
            push offset gPed_flags_0058f2e4+0x138
            push edx
            call strtok
            add esp, 8
            lea edi, [ebp + 0x404]
            push edi
            push offset gPed_flags_0058f2e4+0x134
            push eax
            call sscanf
            mov eax, dword ptr [edi]
            add esp, 0xc
            xor ebx, ebx
            test eax, eax
            jle L0048a599
            lea esi, [ebp + 0x408]
    L0048a551:
            mov ecx, dword ptr [esp + 0x10]
            lea edx, [esp + 0x74]
            call GetALineAndDontArgue
            lea eax, [esp + 0x74]
            push offset gPed_flags_0058f2e4+0x138
            push eax
            call strtok
            add esp, 8
            push esi
            push offset gPed_flags_0058f2e4+0x134
            push eax
            call sscanf
            mov eax, dword ptr [esi]
            add esp, 0xc
            test eax, eax
            jl L0048a58f
            mov ecx, dword ptr [gGroove_funk_offset]
            add ecx, eax
            mov dword ptr [esi], ecx
    L0048a58f:
            mov eax, dword ptr [edi]
            inc ebx
            add esi, 4
            cmp ebx, eax
            jl L0048a551
    L0048a599:
            mov ebx, dword ptr [esp + 0x10]
            lea edx, [esp + 0x74]
            mov ecx, ebx
            call GetALineAndDontArgue
            lea edx, [esp + 0x74]
            push offset gPed_flags_0058f2e4+0x138
            push edx
            call strtok
            add esp, 8
            lea esi, [ebp + 0x420]
            mov edi, 4
    L0048a5c5:
            push esi
            push offset gPed_flags_0058f2e4+0x134
            push eax
            call sscanf
            mov eax, dword ptr [esi]
            add esp, 0xc
            test eax, eax
            jl L0048a5e4
            mov ecx, dword ptr [gGroove_funk_offset]
            add ecx, eax
            mov dword ptr [esi], ecx
    L0048a5e4:
            push offset gPed_flags_0058f2e4+0x138
            push 0
            call strtok
            add esp, 8
            add esi, 4
            dec edi
            jne L0048a5c5
            call PossibleService
            lea edx, [esp + 0x74]
            mov ecx, ebx
            call GetALineAndDontArgue
            lea edx, [esp + 0x74]
            push offset gPed_flags_0058f2e4+0x138
            push edx
            call strtok
            add esp, 8
            lea esi, [ebp + 0x430]
            mov edi, 4
    L0048a626:
            push esi
            push offset gPed_flags_0058f2e4+0x134
            push eax
            call sscanf
            mov eax, dword ptr [esi]
            add esp, 0xc
            test eax, eax
            jl L0048a645
            mov ecx, dword ptr [gGroove_funk_offset]
            add ecx, eax
            mov dword ptr [esi], ecx
    L0048a645:
            push offset gPed_flags_0058f2e4+0x138
            push 0
            call strtok
            add esp, 8
            add esi, 4
            dec edi
            jne L0048a626
            lea edx, [esp + 0x74]
            mov ecx, ebx
            call GetALineAndDontArgue
            lea edx, [esp + 0x74]
            push offset gPed_flags_0058f2e4+0x138
            push edx
            call strtok
            add esp, 8
            lea esi, [ebp + 0x440]
            mov edi, 2
    L0048a682:
            push esi
            push offset gPed_flags_0058f2e4+0x134
            push eax
            call sscanf
            mov eax, dword ptr [esi]
            add esp, 0xc
            test eax, eax
            jl L0048a6a1
            mov ecx, dword ptr [gGroove_funk_offset]
            add ecx, eax
            mov dword ptr [esi], ecx
    L0048a6a1:
            push offset gPed_flags_0058f2e4+0x138
            push 0
            call strtok
            add esp, 8
            add esi, 4
            dec edi
            jne L0048a682
            lea edx, [esp + 0x74]
            mov ecx, ebx
            call GetALineAndDontArgue
            lea edx, [esp + 0x74]
            push offset gPed_flags_0058f2e4+0x138
            push edx
            call strtok
            add esp, 8
            lea esi, [ebp + 0x448]
            mov edi, 2
    L0048a6de:
            push esi
            push offset gPed_flags_0058f2e4+0x134
            push eax
            call sscanf
            mov eax, dword ptr [esi]
            add esp, 0xc
            test eax, eax
            jl L0048a6fd
            mov ecx, dword ptr [gGroove_funk_offset]
            add ecx, eax
            mov dword ptr [esi], ecx
    L0048a6fd:
            push offset gPed_flags_0058f2e4+0x138
            push 0
            call strtok
            add esp, 8
            add esi, 4
            dec edi
            jne L0048a6de
            mov ecx, dword ptr [esp + 0x10]
            lea eax, [ebp + 0x45c]
            lea edi, [ebp + 0x458]
            lea ebx, [ebp + 0x454]
            lea esi, [ebp + 0x450]
            push eax
            push edi
            push ebx
            mov edx, esi
            call GetFourInts
            mov eax, dword ptr [esi]
            test eax, eax
            jl L0048a748
            mov edx, dword ptr [gGroove_funk_offset]
            add edx, eax
            mov dword ptr [esi], edx
    L0048a748:
            mov eax, dword ptr [ebx]
            test eax, eax
            jl L0048a758
            mov ecx, dword ptr [gGroove_funk_offset]
            add ecx, eax
            mov dword ptr [ebx], ecx
    L0048a758:
            mov eax, dword ptr [edi]
            test eax, eax
            jl L0048a768
            mov edx, dword ptr [gGroove_funk_offset]
            add edx, eax
            mov dword ptr [edi], edx
    L0048a768:
            mov eax, dword ptr [ebp + 0x45c]
            test eax, eax
            jl L0048a780
            mov ecx, dword ptr [gGroove_funk_offset]
            add ecx, eax
            mov dword ptr [ebp + 0x45c], ecx
    L0048a780:
            call PossibleService
            mov ecx, dword ptr [esp + 0x10]
            lea eax, [ebp + 0x46c]
            lea edi, [ebp + 0x468]
            lea ebx, [ebp + 0x464]
            lea esi, [ebp + 0x460]
            push eax
            push edi
            push ebx
            mov edx, esi
            call GetFourInts
            mov eax, dword ptr [esi]
            test eax, eax
            jl L0048a7bb
            mov edx, dword ptr [gGroove_funk_offset]
            add edx, eax
            mov dword ptr [esi], edx
    L0048a7bb:
            mov eax, dword ptr [ebx]
            test eax, eax
            jl L0048a7cb
            mov ecx, dword ptr [gGroove_funk_offset]
            add ecx, eax
            mov dword ptr [ebx], ecx
    L0048a7cb:
            mov eax, dword ptr [edi]
            test eax, eax
            jl L0048a7db
            mov edx, dword ptr [gGroove_funk_offset]
            add edx, eax
            mov dword ptr [edi], edx
    L0048a7db:
            mov eax, dword ptr [ebp + 0x46c]
            test eax, eax
            jl L0048a7f3
            mov ecx, dword ptr [gGroove_funk_offset]
            add ecx, eax
            mov dword ptr [ebp + 0x46c], ecx
    L0048a7f3:
            mov esi, dword ptr [esp + 0x10]
            lea edx, [esp + 0x74]
            mov ecx, esi
            call GetALineAndDontArgue
            lea edx, [esp + 0x74]
            push offset gPed_flags_0058f2e4+0x138
            push edx
            call strtok
            add esp, 8
            lea ecx, [esp + 0x48]
            push ecx
            push offset gMini_map_glowing_line_animation_indices+0x7dc
            push eax
            call sscanf
            fld dword ptr [esp + 0x54]
            fadd st(0), st(0)
            add esp, 0xc
            lea edx, [esp + 0x74]
            mov ecx, esi
            fmul qword ptr [gDouble_0058a068+0x218]
            fstp dword ptr [ebp + 0x4b8]
            call GetALineAndDontArgue
            lea edx, [esp + 0x74]
            push offset gPed_flags_0058f2e4+0x138
            push edx
            call strtok
            add esp, 8
            lea ecx, [esp + 0x48]
            push ecx
            push offset gMini_map_glowing_line_animation_indices+0x7dc
            push eax
            call sscanf
            fld dword ptr [esp + 0x54]
            mov edi, dword ptr [esp + 0x24]
            add esp, 0xc
            fadd st(0), st(0)
            xor edx, edx
            cmp edi, 8
            setne dl
            fmul qword ptr [gDouble_0058a068+0x218]
            mov dword ptr [ebp + 0x400], edx
            fstp dword ptr [ebp + 0x4bc]
            call PossibleService
            lea edx, [esp + 0x74]
            mov ecx, esi
            call GetALineAndDontArgue
            mov ecx, dword ptr [gGroove_funk_offset]
            mov edx, dword ptr [esp + 0x980]
            lea eax, [esp + 0x574]
            push eax
            push ecx
            mov ecx, esi
            call AddFunkotronics
            call PossibleService
            mov esi, offset gWheel_actor_names
            lea ebx, [ebp + 0x12ec]
    L0048a8ca:
            mov edx, dword ptr [esi]
            mov ecx, dword ptr [ebp + 0x10]
            call DRActorFindRecurse
            mov dword ptr [ebx], eax
            add esi, 4
            add ebx, 4
            cmp esi, offset gPivot_actor_names
            jl L0048a8ca
            mov esi, offset gPivot_actor_names
            lea ebx, [ebp + 0x1304]
    L0048a8ef:
            mov edx, dword ptr [esi]
            mov ecx, dword ptr [ebp + 0x10]
            call DRActorFindRecurse
            mov dword ptr [ebx], eax
            add esi, 4
            add ebx, 4
            cmp esi, offset gRaces_file_names
            jl L0048a8ef
            lea eax, [ebp + 0x12ec]
            mov edi, 6
            mov ebx, eax
    L0048a916:
            mov edx, dword ptr [ebx]
            test edx, edx
            je L0048aa36
            mov ecx, dword ptr [edx + 0x18]
            test ecx, ecx
            je L0048aa36
            fld dword ptr [ecx + 0x34]
            fadd dword ptr [ecx + 0x40]
            fld dword ptr [ecx + 0x38]
            fld dword ptr [ecx + 0x3c]
            fxch st(2)
            fmul dword ptr [gDouble_0058a068+0x220]
            fxch st(1)
            fadd dword ptr [ecx + 0x44]
            fxch st(2)
            fadd dword ptr [ecx + 0x48]
            fld st(1)
            fcomp dword ptr [gDouble_0058a068+0x1e8]
            fxch st(2)
            fmul dword ptr [gDouble_0058a068+0x220]
            fxch st(2)
            fmul dword ptr [gDouble_0058a068+0x220]
            fnstsw ax
            fstp dword ptr [esp + 0x34]
            test ah, 0x40
            fxch st(1)
            je L0048a990
            fcom dword ptr [gDouble_0058a068+0x1e8]
            fnstsw ax
            test ah, 0x40
            je L0048a990
            fld dword ptr [esp + 0x34]
            fcomp dword ptr [gDouble_0058a068+0x1e8]
            fnstsw ax
            test ah, 0x40
            jne L0048aa32
    L0048a990:
            fld st(1)
            fld st(1)
            fld dword ptr [esp + 0x34]
            fxch st(2)
            fadd dword ptr [edx + 0x50]
            fxch st(1)
            fadd dword ptr [edx + 0x54]
            fxch st(2)
            fadd dword ptr [edx + 0x58]
            fld dword ptr [esp + 0x34]
            fxch st(2)
            fstp dword ptr [edx + 0x50]
            fxch st(2)
            fstp dword ptr [edx + 0x54]
            fxch st(1)
            fstp dword ptr [edx + 0x58]
            fxch st(2)
            fchs
            fxch st(1)
            fchs
            fxch st(2)
            fchs
            fxch st(1)
            fstp dword ptr [esp + 0x2c]
            cmp word ptr [ecx + 0x10], 0
            mov edx, 0
            fxch st(1)
            fstp dword ptr [esp + 0x30]
            fstp dword ptr [esp + 0x34]
            jbe L0048aa25
    L0048a9e2:
            mov esi, dword ptr [ecx + 8]
            mov eax, edx
            and eax, 0xffff
            fld dword ptr [esp + 0x2c]
            lea eax, [eax + eax*4]
            shl eax, 3
            add esi, eax
            inc edx
            _emit 0xd8
        _emit 0x06
            _emit 0xd9
        _emit 0x1e
            mov esi, dword ptr [ecx + 8]
            fld dword ptr [esp + 0x30]
            fadd dword ptr [eax + esi + 4]
            lea esi, [eax + esi + 4]
            _emit 0xd9
        _emit 0x1e
            mov esi, dword ptr [ecx + 8]
            fld dword ptr [esp + 0x34]
            fadd dword ptr [eax + esi + 8]
            lea eax, [eax + esi + 8]
            _emit 0xd9
        _emit 0x18
            cmp dx, word ptr [ecx + 0x10]
            jb L0048a9e2
    L0048aa25:
            push 1
            push ecx
            call BrModelUpdate
            add esp, 8
            jmp L0048aa36
    L0048aa32:
            fstp st(0)
            fstp st(0)
    L0048aa36:
            add ebx, 4
            dec edi
            jne L0048a916
            lea edx, [ebp + 0x1304]
            mov esi, 6
            xor edi, edi
    L0048aa4d:
            mov eax, dword ptr [edx]
            cmp eax, edi
            je L0048aa7e
            mov ecx, dword ptr [eax + 8]
            cmp ecx, edi
            je L0048aa7e
            fld dword ptr [ecx + 0x50]
            fadd dword ptr [eax + 0x50]
            fstp dword ptr [eax + 0x50]
            fld dword ptr [ecx + 0x54]
            fadd dword ptr [eax + 0x54]
            fstp dword ptr [eax + 0x54]
            fld dword ptr [ecx + 0x58]
            fadd dword ptr [eax + 0x58]
            fstp dword ptr [eax + 0x58]
            mov dword ptr [ecx + 0x50], edi
            mov dword ptr [ecx + 0x54], edi
            mov dword ptr [ecx + 0x58], edi
    L0048aa7e:
            add edx, 4
            dec esi
            jne L0048aa4d
            mov ebx, dword ptr [esp + 0x10]
            lea edx, [esp + 0x74]
            mov ecx, ebx
            call GetALineAndDontArgue
            mov edx, dword ptr [gGroove_funk_offset]
            mov eax, dword ptr [ebp + 0x10]
            push 1
            push edx
            mov edx, dword ptr [esp + 0x988]
            push eax
            mov ecx, ebx
            call AddGroovidelics
            cmp dword ptr [esp + 0x40], edi
            jne L0048ab11
    L0048aab4:
            lea edx, [esp + 0x74]
            mov ecx, ebx
            call GetALineAndDontArgue
            cmp eax, edi
            mov dword ptr [esp + 0x24], eax
            je L0048ab05
            lea esi, [esp + 0x74]
            mov ecx, offset gNet_avail_names+0x704
    L0048aad0:
            mov al, byte ptr [ecx]
            mov dl, al
            cmp al, byte ptr [esi]
            jne L0048aaf4
            test dl, dl
            je L0048aaf0
            mov al, byte ptr [ecx + 1]
            mov dl, al
            cmp al, byte ptr [esi + 1]
            jne L0048aaf4
            add ecx, 2
            add esi, 2
            test dl, dl
            jne L0048aad0
    L0048aaf0:
            xor ecx, ecx
            jmp L0048aaf9
    L0048aaf4:
            sbb ecx, ecx
            sbb ecx, -1
    L0048aaf9:
            cmp ecx, edi
            jne L0048aab4
            mov eax, dword ptr [esp + 0x24]
            cmp eax, edi
            jne L0048ab1c
    L0048ab05:
            mov ecx, offset gNet_avail_names+0x6d8
            call PDFatalError
            jmp L0048ab1c
    L0048ab11:
            lea edx, [esp + 0x74]
            mov ecx, ebx
            call GetALineAndDontArgue
    L0048ab1c:
            call PossibleService
            lea edi, [esp + 0x74]
            or ecx, 0xffffffff
            xor eax, eax
            repne scasb
            not ecx
            add ecx, -2
            lea eax, [esp + ecx + 0x74]
            mov cl, byte ptr [esp + ecx + 0x74]
            cmp cl, 0x20
            jne L0048ab47
    L0048ab3e:
            mov cl, byte ptr [eax - 1]
            dec eax
            cmp cl, 0x20
            je L0048ab3e
    L0048ab47:
            mov ebx, dword ptr [esp + 0x10]
            mov ecx, ebx
            call GetAScalar
            fdivr qword ptr [gDouble_0058a068+0x1f0]
            mov ecx, ebx
            fstp dword ptr [ebp + 0x1258]
            call GetAScalar
            fstp dword ptr [ebp + 0x12ac]
            mov ecx, ebx
            call GetAScalar
            fstp dword ptr [ebp + 0x12b0]
            mov ecx, ebx
            call GetAnInt
            mov ecx, ebx
            mov dword ptr [ebp + 0x1364], eax
            call GetAScalar
            fstp dword ptr [esp + 0x54]
            mov ecx, ebx
            call GetAScalar
            fstp dword ptr [esp + 0x44]
            push 0xf6
            push 0x4d8
            call BrMemAllocate
            mov dword ptr [ebp + 8], eax
            mov dword ptr [eax + 0x23c], ebp
            mov ecx, dword ptr [ebp + 8]
            add esp, 8
            mov byte ptr [ecx + 0x238], 1
            mov edx, dword ptr [ebp + 8]
            mov ecx, ebx
            mov dword ptr [edx + 0x1a0], 0xffff
            mov eax, dword ptr [ebp + 8]
            lea edx, [esp + 0x374]
            mov dword ptr [eax + 0x1a4], 0
            mov esi, dword ptr [ebp + 8]
            mov edi, dword ptr [esi + 0x23c]
            call GetAString
            lea edx, [esp + 0x374]
            mov ecx, ebx
            call GetAString
            lea edx, [esp + 0x374]
            mov ecx, ebx
            call GetAString
            mov ecx, dword ptr [edi + 0x10]
            lea edx, [esp + 0x374]
            mov dword ptr [esi], ecx
            mov ecx, ebx
            call GetAString
            lea edx, [esp + 0x14]
            lea eax, [esp + 0x28]
            push edx
            push eax
            lea edx, [esp + 0x2c]
            mov ecx, ebx
            call GetThreeFloats
            mov ecx, dword ptr [esp + 0x24]
            mov edx, dword ptr [esp + 0x28]
            mov eax, dword ptr [esp + 0x14]
            mov dword ptr [esi + 0x14], ecx
            mov ecx, ebx
            mov dword ptr [esi + 0x18], edx
            mov dword ptr [esi + 0x1c], eax
            call GetAScalar
            lea ecx, [esp + 0x18]
            lea edx, [esp + 0x14]
            push ecx
            push edx
            fstp dword ptr [esi + 4]
            lea edx, [esp + 0x28]
            mov ecx, ebx
            call GetThreeFloats
            fld dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x18]
            fld dword ptr [esp + 0x20]
            fxch st(2)
            fmul dword ptr [esp + 0x14]
            fld dword ptr [esi + 4]
            fxch st(2)
            fmul dword ptr [esp + 0x18]
            fld dword ptr [esi + 4]
            fxch st(4)
            fmul dword ptr [esp + 0x20]
            fxch st(2)
            fst dword ptr [esp + 0x18]
            fld st(1)
            fxch st(3)
            fstp dword ptr [esp + 0x14]
            mov ecx, ebx
            fadd dword ptr [esp + 0x14]
            fxch st(2)
            fadd dword ptr [esp + 0x14]
            fxch st(3)
            fmulp st(2), st(0)
            fxch st(3)
            fmulp st(2), st(0)
            fmul dword ptr [gDouble_0058a068+0x1e4]
            fxch st(2)
            fadd dword ptr [esp + 0x18]
            fxch st(1)
            fmul dword ptr [gDouble_0058a068+0x1e4]
            fxch st(2)
            fstp dword ptr [esi + 0x10]
            fxch st(1)
            fstp dword ptr [esi + 0xc]
            fmul dword ptr [esi + 4]
            fmul dword ptr [gDouble_0058a068+0x1e4]
            fstp dword ptr [esi + 8]
            call GetAScalar
            fstp dword ptr [edi + 0x12a4]
            mov ecx, ebx
            call GetAnInt
            mov esi, eax
            cmp esi, 4
            je L0048acfc
            push 0
            call FatalError
            add esp, 4
    L0048acfc:
            test esi, esi
            jle L0048af1c
            mov dword ptr [esp + 0x24], esi
    L0048ad08:
            mov ecx, ebx
            call GetAnInt
            lea edx, [esp + 0x474]
            mov ecx, ebx
            call GetAString
            lea edx, [esp + 0x474]
            mov ecx, ebx
            call GetAString
            lea eax, [esp + 0x20]
            lea ecx, [esp + 0x18]
            push eax
            push ecx
            lea edx, [esp + 0x1c]
            mov ecx, ebx
            call GetThreeFloats
            fld dword ptr [esp + 0x14]
            fcomp dword ptr [gDouble_0058a068+0x1e8]
            mov eax, dword ptr [esp + 0x18]
            mov edx, dword ptr [esp + 0x14]
            mov ecx, dword ptr [esp + 0x20]
            mov dword ptr [esp + 0x30], eax
            mov dword ptr [esp + 0x2c], edx
            mov dword ptr [esp + 0x34], ecx
            fnstsw ax
            test ah, 0x40
            je L0048ae19
            fld dword ptr [esp + 0x18]
            fcomp dword ptr [gDouble_0058a068+0x1e8]
            fnstsw ax
            test ah, 0x40
            je L0048ae19
            fld dword ptr [esp + 0x20]
            fcomp dword ptr [gDouble_0058a068+0x1e8]
            fnstsw ax
            test ah, 0x40
            je L0048ae19
            xor esi, esi
            lea edi, [ebp + 0x12ec]
    L0048ad9f:
            mov eax, dword ptr [edi]
            test eax, eax
            je L0048adb8
            mov edx, dword ptr [eax + 0x14]
            lea ecx, [esp + 0x474]
            call DRStricmp
            test eax, eax
            je L0048adc3
    L0048adb8:
            inc esi
            add edi, 4
            cmp esi, 4
            jl L0048ad9f
            jmp L0048ae19
    L0048adc3:
            mov eax, dword ptr [ebp + esi*4 + 0x12ec]
            fld dword ptr [eax + 0x50]
            fst dword ptr [esp + 0x2c]
            fld dword ptr [eax + 0x54]
            fst dword ptr [esp + 0x30]
            fld dword ptr [eax + 0x58]
            mov eax, dword ptr [eax + 0xc]
            fst dword ptr [esp + 0x34]
            test eax, eax
            je L0048ae05
            mov ecx, dword ptr [ebp + 0x10]
    L0048ade9:
            cmp eax, ecx
            je L0048ae0d
            fld dword ptr [eax + 0x50]
            fld dword ptr [eax + 0x54]
            fxch st(1)
            faddp st(4), st(0)
            faddp st(2), st(0)
            fadd dword ptr [eax + 0x58]
            mov eax, dword ptr [eax + 0xc]
            test eax, eax
            jne L0048ade9
            jmp L0048ae0d
    L0048ae05:
            fstp st(0)
            fstp st(0)
            fstp st(0)
            jmp L0048ae19
    L0048ae0d:
            fstp dword ptr [esp + 0x34]
            fstp dword ptr [esp + 0x30]
            fstp dword ptr [esp + 0x2c]
    L0048ae19:
            mov ecx, ebx
            call GetAnInt
            mov ecx, ebx
            mov esi, eax
            call GetAnInt
            mov ecx, ebx
            call GetAScalar
            fstp dword ptr [esp + 0x1c]
            mov ecx, ebx
            call GetAScalar
            fstp dword ptr [esp + 0x70]
            mov ecx, ebx
            call GetAScalar
            lea edx, [esp + 0x58]
            mov ecx, ebx
            push edx
            lea edx, [esp + 0x2c]
            fstp dword ptr [esp + 0x54]
            call GetPairOfFloats
            mov ecx, ebx
            call GetAScalar
            fstp dword ptr [esp + 0x60]
            mov ecx, ebx
            call GetAScalar
            test esi, esi
            je L0048ae8f
            mov ecx, dword ptr [esp + 0x28]
            mov eax, dword ptr [esp + 0x1c]
            fstp dword ptr [ebp + 0x12c8]
            mov dword ptr [esp + 0x5c], ecx
            mov dword ptr [ebp + 0x1210], eax
            mov ecx, 2
            jmp L0048aebb
    L0048ae8f:
            mov edx, dword ptr [esp + 0x1c]
            mov ecx, dword ptr [esp + 0x58]
            mov eax, dword ptr [esp + 0x28]
            mov dword ptr [ebp + 0x1214], edx
            mov edx, dword ptr [esp + 0x60]
            mov dword ptr [esp + 0x38], ecx
            fstp dword ptr [ebp + 0x12cc]
            mov dword ptr [esp + 0x6c], eax
            mov dword ptr [ebp + 0x12a0], edx
            xor ecx, ecx
    L0048aebb:
            fld dword ptr [esp + 0x2c]
            fcomp dword ptr [gDouble_0058a068+0x1e8]
            fnstsw ax
            test ah, 0x41
            jne L0048aecd
            inc ecx
    L0048aecd:
            fld dword ptr [esp + 0x2c]
            lea eax, [ecx + 0x183]
            mov edx, dword ptr [esp + 0x30]
            lea ecx, [ecx + ecx*2]
            lea eax, [eax + eax*2]
            fstp dword ptr [ebp + eax*4]
            lea eax, [ebp + ecx*4]
            mov ecx, dword ptr [esp + 0x34]
            mov dword ptr [eax + 0x1228], edx
            mov edx, dword ptr [esp + 0x70]
            mov dword ptr [eax + 0x122c], ecx
            mov eax, dword ptr [esp + 0x50]
            mov dword ptr [ebp + 0x12b4], eax
            mov eax, dword ptr [esp + 0x24]
            dec eax
            mov dword ptr [ebp + 0x11fc], edx
            mov dword ptr [esp + 0x24], eax
            jne L0048ad08
    L0048af1c:
            mov ecx, dword ptr [ebp + 8]
            mov edx, ebx
            mov dword ptr [ecx + 0x64], 0x3ecccccd
            mov ecx, dword ptr [ebp + 8]
            add ecx, 0x20
            call ReadMechanicsShapes
            mov ecx, dword ptr [ebp + 8]
            call UpdateCollisionObject
            fld dword ptr [esp + 0x5c]
            fmul qword ptr [gDouble_0058a068+0x1d8]
            fld dword ptr [esp + 0x6c]
            fmul qword ptr [gDouble_0058a068+0x1d8]
            fxch st(1)
            fptan
            mov ecx, dword ptr [ebp + 8]
            fxch st(2)
            fptan
            fld dword ptr [ebp + 0x1258]
            fmul qword ptr [gDouble_0058a068+0x228]
            fld dword ptr [esp + 0x54]
            fxch st(1)
            fstp dword ptr [ebp + 0x1258]
            fld dword ptr [ecx + 0x28]
            fsub qword ptr [gDouble_0058a068+0x230]
            fld st(0)
            fmul qword ptr [gDouble_0058a068+0x238]
            fstp dword ptr [ebp + 0x1220]
            mov edx, dword ptr [ecx + 4]
            mov dword ptr [esp + 0x1c], edx
            fld dword ptr [esp + 0x1c]
            fld dword ptr [ebp + 0x12ac]
            fmul dword ptr [esp + 0x1c]
            fmul dword ptr [gDouble_0058a068+0x240]
            fstp dword ptr [ebp + 0x12ac]
            fld dword ptr [ebp + 0x12b0]
            fmul dword ptr [esp + 0x1c]
            fmul dword ptr [gDouble_0058a068+0x240]
            fstp dword ptr [ebp + 0x12b0]
            fld dword ptr [ecx + 0x14]
            fmul dword ptr [gDouble_0058a068+0x244]
            fstp dword ptr [ebp + 0x58]
            fld dword ptr [ecx + 0x18]
            fmul dword ptr [gDouble_0058a068+0x244]
            fstp dword ptr [ebp + 0x5c]
            fld dword ptr [ecx + 0x1c]
            fmul dword ptr [gDouble_0058a068+0x244]
            fstp dword ptr [ebp + 0x60]
            fild dword ptr [ebp + 0x1364]
            fstp dword ptr [esp + 0x24]
            fxch st(2)
            fmul dword ptr [gDouble_0058a068+0x248]
            fdiv dword ptr [esp + 0x24]
            fmul dword ptr [gDouble_0058a068+0x24c]
            fstp dword ptr [ebp + 0x1368]
            fxch st(4)
            fmul qword ptr [gDouble_0058a068+0x250]
            fxch st(5)
            fstp st(0)
            fmul dword ptr [esp + 0x44]
            fxch st(4)
            fst dword ptr [esp + 0x44]
            fxch st(4)
            fmul dword ptr [esp + 0x24]
            fxch st(2)
            fmul qword ptr [gDouble_0058a068+0x250]
            fxch st(4)
            fst dword ptr [esp + 0x28]
            fxch st(2)
            fstp dword ptr [ebp + 0x136c]
            fxch st(1)
            fstp dword ptr [ebp + 0x1298]
            fxch st(2)
            fstp dword ptr [esp + 0x44]
            fxch st(1)
            fstp st(1)
            fstp st(0)
            fld dword ptr [esp + 0x38]
            fmul qword ptr [gDouble_0058a068+0x1d8]
            fld dword ptr [esp + 0x44]
            fxch st(1)
            fptan
            lea eax, [ebp + 0x122c]
            mov edx, 4
            fxch st(1)
            fmul qword ptr [gDouble_0058a068+0x250]
            fld dword ptr [ebp + 0x1244]
            fxch st(3)
            fstp dword ptr [esp + 0x24]
            fld dword ptr [esp + 0x24]
            fld st(1)
            fxch st(4)
            _emit 0xd8
        _emit 0x20
            fld dword ptr [ebp + 0x1244]
            fxch st(2)
            fstp dword ptr [ebp + 0x1294]
            fxch st(4)
            fstp dword ptr [ebp + 0x129c]
            fsub dword ptr [ecx + 0x1c]
            fxch st(3)
            fstp dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x1c]
            fxch st(3)
            fdiv dword ptr [esp + 0x14]
            fxch st(3)
            fmul dword ptr [gDouble_0058a068+0x258]
            _emit 0xd9
        _emit 0x00
            fxch st(1)
            fstp dword ptr [esp + 0x18]
            fxch st(3)
            fmul dword ptr [esp + 0x18]
            _emit 0xd9
        _emit 0x00
            fsub dword ptr [ebp + 0x1244]
            fxch st(1)
            fsqrt
            fst dword ptr [esp + 0x38]
            fmul dword ptr [esp + 0x24]
            fld dword ptr [ebp + 0x1244]
            fxch st(4)
            fstp st(0)
            fstp dword ptr [ebp + 0x1294]
            fxch st(3)
            fsub dword ptr [ecx + 0x1c]
            fxch st(3)
            fdivp st(3), st(0)
            fxch st(2)
            fmul dword ptr [esp + 0x18]
            fsqrt
            fst dword ptr [esp + 0x38]
            fmul dword ptr [esp + 0x28]
            fstp dword ptr [ebp + 0x1298]
            fsub dword ptr [ecx + 0x1c]
            fdiv dword ptr [esp + 0x14]
            fmul dword ptr [esp + 0x18]
            fsqrt
            fmul st(0), st(1)
            fstp dword ptr [ebp + 0x129c]
            fstp st(0)
    L0048b120:
            fld dword ptr [eax - 8]
            fmul qword ptr [gDouble_0058a068+0x238]
            fstp dword ptr [eax - 8]
            fld dword ptr [eax - 4]
            lea ecx, [eax - 4]
            add eax, 0xc
            fmul qword ptr [gDouble_0058a068+0x238]
            _emit 0xd9
        _emit 0x19
            fld dword ptr [eax - 0xc]
            fmul qword ptr [gDouble_0058a068+0x238]
            fstp dword ptr [eax - 0xc]
            mov esi, dword ptr [ebp + 0x1220]
            dec edx
            mov dword ptr [ecx], esi
            jne L0048b120
            mov ecx, ebx
            call GetAnInt
            mov eax, dword ptr [ebp + 8]
            mov ecx, 6
            lea edx, [esp + 0x474]
            lea esi, [eax + 0x24]
            lea edi, [eax + 0x3c]
            rep movsd
            mov ecx, ebx
            call GetAString
            call PossibleService
            mov ecx, ebx
            call GetAnInt
            xor edi, edi
            mov dword ptr [ebp + 0x6c], eax
            test eax, eax
            jle L0048b1d1
            lea esi, [ebp + 0x84]
    L0048b194:
            lea edx, [esp + 0x474]
            mov ecx, ebx
            call GetALineAndDontArgue
            test ebp, ebp
            je L0048b1b3
            mov eax, dword ptr [ebp + 0xc]
            mov edx, 2
            cmp eax, 5
            jg L0048b1b8
    L0048b1b3:
            mov edx, 1
    L0048b1b8:
            lea ecx, [esp + 0x474]
            call GetSimpleMaterial
            mov dword ptr [esi], eax
            mov eax, dword ptr [ebp + 0x6c]
            inc edi
            add esi, 4
            cmp edi, eax
            jl L0048b194
    L0048b1d1:
            mov eax, dword ptr [ebp + 0xe10]
            xor ecx, ecx
            xor edx, edx
            mov esi, dword ptr [eax + 0x18]
            mov dword ptr [esp + 0x20], edx
            mov dword ptr [esp + 0x24], esi
            mov eax, dword ptr [esi + 0x4c]
            mov cx, word ptr [eax + 8]
            test ecx, ecx
            jle L0048b208
            mov eax, dword ptr [eax + 0x18]
            add eax, 0x1e
    L0048b1f7:
            xor edi, edi
            mov di, word ptr [eax]
            add eax, 0x24
            add edx, edi
            dec ecx
            jne L0048b1f7
            mov dword ptr [esp + 0x20], edx
    L0048b208:
            lea eax, [ebp + 0xac]
            mov dword ptr [esp + 0x14], 1
            mov ebx, eax
            mov dword ptr [esp + 0x28], 0xc
            mov dword ptr [esp + 0x1c], ebx
    L0048b224:
            mov ecx, dword ptr [esp + 0x10]
            call GetAnInt
            mov ecx, dword ptr [esp + 0x20]
            mov word ptr [ebx], ax
            and eax, 0xffff
            cmp eax, ecx
            jl L0048b242
            mov word ptr [ebx], 0
    L0048b242:
            xor edi, edi
            cmp word ptr [ebx], di
            je L0048b24d
            mov dword ptr [esp + 0x14], edi
    L0048b24d:
            mov eax, dword ptr [esi + 0x4c]
            xor ecx, ecx
            mov dword ptr [esp + 0x18], edi
            mov cx, word ptr [eax + 8]
            cmp ecx, edi
            jle L0048b2ca
    L0048b25e:
            mov eax, dword ptr [esi + 0x4c]
            lea ebx, [edi + edi*8]
            xor edx, edx
            mov eax, dword ptr [eax + 0x18]
            lea ebx, [eax + ebx*4]
            xor eax, eax
            mov ax, word ptr [ebx + 0x1e]
            test eax, eax
            jle L0048b2c1
            mov esi, dword ptr [esp + 0x1c]
            mov si, word ptr [esi]
            mov word ptr [esp + 0x38], si
            mov esi, dword ptr [ebx + 0x18]
    L0048b285:
            mov bx, word ptr [esp + 0x38]
            cmp word ptr [esi], bx
            je L0048b2a6
            mov ebx, dword ptr [esp + 0x18]
            add esi, 2
            inc ebx
            inc edx
            cmp edx, eax
            mov dword ptr [esp + 0x18], ebx
            jl L0048b285
            mov esi, dword ptr [esp + 0x24]
            jmp L0048b2c1
    L0048b2a6:
            mov edx, dword ptr [esp + 0x1c]
            mov cx, word ptr [esp + 0x18]
            mov esi, dword ptr [esp + 0x24]
            mov word ptr [edx], cx
            xor ecx, ecx
            mov eax, dword ptr [esi + 0x4c]
            mov cx, word ptr [eax + 8]
            mov edi, ecx
    L0048b2c1:
            inc edi
            cmp edi, ecx
            jl L0048b25e
            mov ebx, dword ptr [esp + 0x1c]
    L0048b2ca:
            mov eax, dword ptr [esp + 0x28]
            add ebx, 2
            dec eax
            mov dword ptr [esp + 0x1c], ebx
            mov dword ptr [esp + 0x28], eax
            jne L0048b224
            mov eax, dword ptr [esp + 0x14]
            test eax, eax
            je L0048b30c
            mov eax, dword ptr [esp + 0x20]
            lea esi, [ebp + 0xac]
            mov edi, 0xc
            lea ebx, [eax - 1]
    L0048b2fa:
            mov edx, ebx
            xor ecx, ecx
            call IRandomBetween
            mov word ptr [esi], ax
            add esi, 2
            dec edi
            jne L0048b2fa
    L0048b30c:
            mov esi, dword ptr [esp + 0x10]
            mov ecx, esi
            call PFfeof
            test eax, eax
            jne L0048b442
    L0048b31f:
            lea edx, [esp + 0x374]
            mov ecx, esi
            call GetAString
            mov edx, offset gNet_avail_names+0x5e8
            lea ecx, [esp + 0x374]
            call DRStricmp
            test eax, eax
            je L0048b442
            mov edx, offset gNet_avail_names+0x6c4
            lea ecx, [esp + 0x374]
            call DRStricmp
            test eax, eax
            jne L0048b37a
            lea edx, [ebp + 0x18d8]
            mov ecx, esi
            call GetAVector3
            lea edx, [ebp + 0x18e4]
            mov ecx, esi
            call GetAVector3
            jmp L0048b42d
    L0048b37a:
            mov edx, offset gNet_avail_names+0x6a8
            lea ecx, [esp + 0x374]
            call DRStricmp
            test eax, eax
            jne L0048b42d
            mov ecx, dword ptr [esp + 0x10]
            call GetAnInt
            test eax, eax
            mov dword ptr [ebp + 0x18f0], eax
            mov dword ptr [esp + 0x24], 0
            jle L0048b42d
            lea esi, [ebp + 0x18f8]
    L0048b3b4:
            mov edi, dword ptr [esp + 0x10]
            lea edx, [esp + 0x374]
            mov ecx, edi
            call GetAString
            lea ecx, [esp + 0x374]
            push ecx
            call BrMaterialFind
            add esp, 4
            mov ecx, edi
            mov dword ptr [esi - 4], eax
            call GetAnInt
            xor edi, edi
            mov dword ptr [esi], eax
            test eax, eax
            jle L0048b417
            lea ebx, [esi + 4]
    L0048b3eb:
            mov ecx, dword ptr [esp + 0x10]
            lea edx, [esp + 0x374]
            call GetAString
            lea edx, [esp + 0x374]
            push edx
            call BrMapFind
            add esp, 4
            mov dword ptr [ebx], eax
            mov eax, dword ptr [esi]
            inc edi
            add ebx, 4
            cmp edi, eax
            jl L0048b3eb
    L0048b417:
            mov eax, dword ptr [esp + 0x24]
            mov ecx, dword ptr [ebp + 0x18f0]
            inc eax
            add esi, 0x14
            cmp eax, ecx
            mov dword ptr [esp + 0x24], eax
            jl L0048b3b4
    L0048b42d:
            mov ecx, dword ptr [esp + 0x10]
            call PFfeof
            mov esi, dword ptr [esp + 0x10]
            test eax, eax
            je L0048b31f
    L0048b442:
            mov ecx, esi
            call PFfclose
            mov ecx, dword ptr [esp + 0x3c]
            call PFfclose
            mov dword ptr [gCurrent_car_spec], 0
            mov ecx, dword ptr [ebp + 0xe0c]
            push 0
            mov edx, offset AttachGroovidelic
            mov byte ptr [ebp + 0x18c8], 0
            call DRActorEnumRecurse
            mov ecx, dword ptr [ebp + 0xe0c]
            mov edx, ebp
            call AttachCrushDataToActorModels
            mov ecx, ebp
            call InitPhysModCrushData
            mov ecx, dword ptr [esp + 0x4c]
            call ClosePackFileAndSetTiffLoading
            push offset gNet_avail_names+0x698
            call BrMaterialAllocate
            mov edi, dword ptr [esp + 0x98c]
            mov esi, eax
            add esp, 4
            mov edx, esi
            mov ecx, edi
            call AddMaterialToStorage
            test eax, eax
            jne L0048b4bf
            push 0x45
            call FatalError
            add esp, 4
    L0048b4bf:
            push esi
            call BrMaterialAdd
            mov eax, dword ptr [ebp + 0x10]
            add esp, 4
            mov ecx, 2
            mov dword ptr [eax + 0x1c], esi
            call AutoFogificateMaterials
            xor esi, esi
            xor ecx, ecx
            mov si, word ptr [ebp + 0x1958]
            mov cx, word ptr [ebp + 0x195a]
            cmp esi, ecx
            jge L0048b536
    L0048b4ee:
            cmp esi, dword ptr [edi + 8]
            jge L0048b536
            mov edx, dword ptr [edi + 0x38]
            mov eax, dword ptr [edx + esi*4]
            mov ecx, dword ptr [eax + 4]
            test ecx, ecx
            je L0048b528
            cmp byte ptr [ecx], 0
            je L0048b528
            mov edx, dword ptr [eax + 0x20]
            push 0x7fff
            and edx, 0x1000
            push eax
            neg edx
            sbb dl, dl
            and dl, 0xf1
            add dl, 0x53
            mov byte ptr [ecx], dl
            call BrMaterialUpdate
            add esp, 8
    L0048b528:
            xor eax, eax
            inc esi
            mov ax, word ptr [ebp + 0x195a]
            cmp esi, eax
            jl L0048b4ee
    L0048b536:
            mov ecx, dword ptr [esp + 0x68]
            pop edi
            pop esi
            pop ebp
            mov dword ptr [gStorage_for_callbacks], ecx
            pop ebx
            add esp, 0x968
            ret 0x10
    L0048b54d:
            mov eax, dword ptr [esp + 0x24]
            mov ebx, dword ptr [esp + 0x988]
            mov eax, dword ptr [eax + edx*4]
            jmp L0048a40b
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0048c920
void C2_HOOK_FASTCALL LoadingNoOp2(void) {
}

static __inline void LoadOpponentsCars_AboutToLoadFirstCar(void) {

    memset(&gFunk_groove_flags[1], 0, (CARPOCALYPSE2_ASIZE(gFunk_groove_flags) - 1) * sizeof(int));
    gGroove_funk_offset = 0;
}

// FUNCTION: CARMA2_HW 0x0048cda0
void C2_HOOK_FASTCALL LoadOpponentsCars(tRace_info* pRace_info) {
    int i;
    int j;
    tOpp_spec* opponent;
    int sod_counter;
    char buffer[256];

    C2_HOOK_BUG_ON(sizeof(tCar_spec) != 6500);

    sod_counter = 0;
    if (gNet_mode == eNet_mode_none) {
        LoadOpponentsCars_AboutToLoadFirstCar();
        PrintMemoryDump(0, "JUST BEFORE LOADING YOUR CAR");
        LoadCar(gOpponents[gProgram_state.current_car_index].car_file_name, eDriver_local_human, &gProgram_state.current_car, 0, gProgram_state.player_name, &gOur_car_storage_space);
        SetCarStorageTexturingLevel(&gOur_car_storage_space, GetCarTexturingLevel(), eCTL_full);
        PrintMemoryDump(0, "IMMEDIATELY AFTER LOADING YOUR CAR");
    }

    gGroove_funk_offset = GROOVE_FUNK_MAX_PER_CAR;
    gCount_opponents = 0;
    if (gNet_mode == eNet_mode_none) {
        for (i = 0; i < pRace_info->number_of_racers; i++) {
            opponent = &pRace_info->opponent_list[i];
            PossibleService();
            if (opponent->index >= 0) {
                opponent->car_spec = BrMemAllocate(sizeof(tCar_spec), kMem_oppo_car_spec);
                if (DRStricmp("MAX DAMAGE", gOpponents[opponent->index].name) == 0) {
                    sod_counter++;
                    sprintf(buffer, "POOR SOD %d", sod_counter);
                    LoadCar(gOpponents[opponent->index].car_file_name, eDriver_oppo, opponent->car_spec, opponent->index, buffer, &gTheir_cars_storage_space);
                } else {
                    LoadCar(gOpponents[opponent->index].car_file_name, eDriver_oppo, opponent->car_spec, opponent->index, gOpponents[opponent->index].name, &gTheir_cars_storage_space);
                }
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
C2_NAKED br_material* C2_HOOK_FASTCALL GetSimpleMaterial(char* pName, tRendererShadingType pShading_type) {
#ifndef CARPOCALYPSE2_MATCHING

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
    gSimple_materials[i]->index_base = FindBestColourMatch(colour);
    gSimple_materials[i]->index_range = 0;
    GlorifyMaterial(&gSimple_materials[i], 1, pShading_type);
    BrMaterialAdd(gSimple_materials[i]);
    return gSimple_materials[i];
#else
    __asm {
            sub esp, 0xc
            push ebx
            push ebp
            push esi
            mov esi, ecx
            push edi
            mov ebp, edx
            cmp byte ptr [esi], 0x4d
            jne L00488e46
            push offset gPed_flags_0058f2e4+0x138
            push esi
            call strtok
            movsx eax, byte ptr [esi + 1]
            movsx ecx, byte ptr [esi + 2]
            lea edx, [eax + eax*4]
            mov eax, dword ptr [gSimple_material_colours]
            add esp, 8
            xor esi, esi
            test eax, eax
            lea edx, [ecx + edx*2 - 0x210]
            je L00488e22
            mov ecx, offset gSimple_material_colours
    L00488e10:
            shr eax, 0x18
            cmp eax, edx
            je L00488e22
            mov eax, dword ptr [ecx + 4]
            add ecx, 4
            inc esi
            test eax, eax
            jne L00488e10
    L00488e22:
            mov eax, dword ptr [esi*4 + gSimple_material_colours]
            test eax, eax
            jne L00488e37
            mov ecx, offset gNet_avail_names+0x67c
            call PDFatalError
    L00488e37:
            mov ebx, dword ptr [esi*4 + gSimple_material_colours]
            and ebx, 0xffffff
            jmp L00488e79
    L00488e46:
            lea eax, [esp + 0x18]
            lea ecx, [esp + 0x14]
            push eax
            lea edx, [esp + 0x14]
            push ecx
            push edx
            push offset gNet_avail_names+0x670
            push esi
            call sscanf
            mov ebx, dword ptr [esp + 0x24]
            mov edx, dword ptr [esp + 0x28]
            mov eax, dword ptr [esp + 0x2c]
            add esp, 0x14
            shl ebx, 8
            or ebx, edx
            shl ebx, 8
            or ebx, eax
    L00488e79:
            mov eax, dword ptr [gSimple_materials]
            xor edi, edi
            test eax, eax
            je L00488e99
            mov ecx, offset gSimple_materials
    L00488e89:
            cmp dword ptr [eax + 8], ebx
            je L00488ee4
            mov eax, dword ptr [ecx + 4]
            add ecx, 4
            inc edi
            test eax, eax
            jne L00488e89
    L00488e99:
            cmp edi, 0x20
            jge L00488ee4
            push offset gNet_avail_names+0x668
            call BrMaterialAllocate
            lea esi, [edi*4 + gSimple_materials]
            add esp, 4
            mov dword ptr [esi], eax
            mov dword ptr [eax + 8], ebx
            mov eax, dword ptr [esi]
            mov ecx, dword ptr [eax + 8]
            call FindBestColourMatch
            mov ecx, dword ptr [esi]
            push ebp
            mov byte ptr [ecx + 0x3c], al
            mov edx, dword ptr [esi]
            mov ecx, esi
            mov byte ptr [edx + 0x3d], 0
            mov edx, 1
            call GlorifyMaterial
            mov eax, dword ptr [esi]
            push eax
            call BrMaterialAdd
            add esp, 4
    L00488ee4:
            mov eax, dword ptr [edi*4 + gSimple_materials]
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0xc
            ret
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
    }
#endif
}

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
    tUser_crush_data* user_crush;

    if (pActor->type != BR_ACTOR_MODEL) {
        return 0;
    }
    user_crush = pActor->user;
    if (user_crush == NULL || user_crush->crush_data != NULL || user_crush->groove != NULL) {
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
void C2_NAKED C2_HOOK_FASTCALL ReadNonCarMechanicsData(FILE* pF, tNon_car_spec* pNon_car_spec) {
#ifndef CARPOCALYPSE2_MATCHING
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
#else
    __asm {
            sub esp, 0x264
            push ebx
            push ebp
            mov ebp, edx
            push esi
            push edi
            mov ebx, ecx
            mov dword ptr [ebp + 0xc], 0
            call GetAnInt
            cmp eax, 0x64
            mov dword ptr [esp + 0x34], eax
            jge L004881fd
            mov dword ptr [ebp], eax
            lea eax, [esp + 0x14]
            lea ecx, [esp + 0x10]
            push eax
            push ecx
            lea edx, [esp + 0x28]
            mov ecx, ebx
            call GetThreeFloats
            mov edx, dword ptr [ebp + 8]
            mov eax, dword ptr [esp + 0x20]
            mov dword ptr [edx + 0x14], eax
            mov ecx, dword ptr [ebp + 8]
            mov edx, dword ptr [esp + 0x10]
            mov dword ptr [ecx + 0x18], edx
            mov eax, dword ptr [ebp + 8]
            mov ecx, dword ptr [esp + 0x14]
            lea edx, [esp + 0x28]
            mov dword ptr [eax + 0x1c], ecx
            lea eax, [esp + 0x2c]
            push edx
            push eax
            lea edx, [esp + 0x38]
            mov ecx, ebx
            call GetThreeFloats
            lea ecx, [esp + 0x14]
            lea edx, [esp + 0x10]
            push ecx
            push edx
            lea edx, [esp + 0x28]
            mov ecx, ebx
            call GetThreeFloats
            mov eax, dword ptr [esp + 0x20]
            mov ecx, dword ptr [esp + 0x10]
            mov edx, dword ptr [esp + 0x14]
            mov dword ptr [esp + 0x50], eax
            mov dword ptr [esp + 0x54], ecx
            lea eax, [esp + 0x14]
            lea ecx, [esp + 0x10]
            push eax
            mov dword ptr [esp + 0x5c], edx
            push ecx
            lea edx, [esp + 0x28]
            mov ecx, ebx
            call GetThreeFloats
            mov ecx, dword ptr [esp + 0x14]
            mov edx, dword ptr [esp + 0x20]
            mov eax, dword ptr [esp + 0x10]
            mov dword ptr [esp + 0x64], ecx
            mov ecx, ebx
            mov dword ptr [esp + 0x5c], edx
            mov dword ptr [esp + 0x60], eax
            call GetAnInt
            mov esi, eax
            cmp esi, 6
            jle L00487fc5
            mov edx, dword ptr [ebp]
            lea eax, [esp + 0x174]
            push edx
            push offset gPed_flags_0058f2e4+0x134
            push eax
            call sprintf
            add esp, 0xc
            lea ecx, [esp + 0x174]
            push ecx
            push 0x5d
            call FatalError
            add esp, 8
    L00487fc5:
            test esi, esi
            jle L00487fe1
    L00487fc9:
            lea edx, [esp + 0x3c]
            lea eax, [esp + 0x44]
            push edx
            push eax
            lea edx, [esp + 0x48]
            mov ecx, ebx
            call GetThreeFloats
            dec esi
            jne L00487fc9
    L00487fe1:
            lea ecx, [esp + 0x10]
            lea edx, [esp + 0x20]
            push ecx
            mov ecx, ebx
            call GetPairOfFloats
            fld dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x20]
            lea edx, [esp + 0x18]
            lea eax, [esp + 0x10]
            fstp dword ptr [ebp + 0xcc]
            push edx
            push eax
            fstp dword ptr [ebp + 0xd0]
            lea edx, [esp + 0x24]
            mov ecx, ebx
            call GetThreeFloats
            mov ecx, ebx
            call GetAScalar
            fst dword ptr [esp + 0x14]
            fmul qword ptr [gDouble_0058a068+0x1c8]
            call _ftol
            and eax, 0xffff
            mov ecx, ebx
            mov dword ptr [esp + 0x20], eax
            fild dword ptr [esp + 0x20]
            fld dword ptr [esp + 0x14]
            fmul qword ptr [gDouble_0058a068+0x1d8]
            fxch st(1)
            fmul qword ptr [gDouble_0058a068+0x1d0]
            fcos
            fld st(1)
            fmul st(0), st(2)
            fxch st(1)
            fstp st(2)
            fxch st(1)
            fstp dword ptr [ebp + 0xd8]
            fstp dword ptr [ebp + 0x18]
            call GetAScalar
            fld dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x18]
            fxch st(2)
            fst dword ptr [esp + 0x20]
            fmul dword ptr [gDouble_0058a068+0x1e0]
            fld dword ptr [esp + 0x1c]
            fxch st(2)
            fmul dword ptr [esp + 0x10]
            fxch st(3)
            fmul dword ptr [esp + 0x18]
            fxch st(2)
            fmul dword ptr [esp + 0x1c]
            fxch st(1)
            fst dword ptr [esp + 0x18]
            fxch st(3)
            fst dword ptr [esp + 0x1c]
            fxch st(2)
            fst dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x10]
            fxch st(4)
            fmul dword ptr [esp + 0x18]
            fxch st(4)
            fadd dword ptr [esp + 0x1c]
            fxch st(2)
            fstp dword ptr [esp + 0x10]
            fxch st(2)
            fadd dword ptr [esp + 0x10]
            fxch st(2)
            fadd dword ptr [esp + 0x10]
            fxch st(1)
            fmul dword ptr [gDouble_0058a068+0x1e4]
            fxch st(2)
            fmul dword ptr [gDouble_0058a068+0x1e4]
            fxch st(1)
            fmul dword ptr [gDouble_0058a068+0x1e4]
            fxch st(3)
            fst dword ptr [esp + 0x10]
            fstp dword ptr [ebp + 0xd4]
            push 0xf7
            push 0x50
            fstp dword ptr [ebp + 0xec]
            fxch st(1)
            fstp dword ptr [ebp + 0xe8]
            fstp dword ptr [ebp + 0xe4]
            call BrMemAllocate
            mov ecx, dword ptr [ebp + 8]
            add esp, 8
            fld dword ptr [esp + 0x14]
            fcomp dword ptr [gDouble_0058a068+0x1e8]
            mov dword ptr [ecx + 0x20], eax
            mov edx, dword ptr [ebp + 8]
            mov ecx, 6
            lea esi, [esp + 0x50]
            mov edi, dword ptr [edx + 0x20]
            fnstsw ax
            add edi, 4
            test ah, 0x40
            rep movsd
            jne L004881d2
            mov edx, 0xdb
            mov ecx, 1
            call AllocatePhysicsJoint
            mov ecx, dword ptr [ebp + 8]
            mov dword ptr [ecx + 0x18c], eax
            mov eax, dword ptr [ebp + 8]
            mov edx, dword ptr [eax + 0x18c]
            mov dword ptr [ebp + 0xf0], edx
            mov eax, dword ptr [eax + 0x18c]
            mov dword ptr [eax], 3
            mov ecx, dword ptr [ebp + 8]
            mov eax, dword ptr [esp + 0x30]
            mov edx, dword ptr [ecx + 0x18c]
            mov dword ptr [edx + 8], eax
            mov ecx, dword ptr [ebp + 8]
            mov eax, dword ptr [esp + 0x2c]
            mov edx, dword ptr [ecx + 0x18c]
            mov dword ptr [edx + 0xc], eax
            mov ecx, dword ptr [ebp + 8]
            mov eax, dword ptr [esp + 0x28]
            mov edx, dword ptr [ecx + 0x18c]
            mov dword ptr [edx + 0x10], eax
            mov ecx, dword ptr [ebp + 8]
            mov edx, dword ptr [ecx + 0x18c]
            mov dword ptr [edx + 0x50], 1
            mov eax, dword ptr [ebp + 8]
            mov ecx, dword ptr [eax + 0x18c]
            mov dword ptr [ecx + 0x54], 0xa
            mov edx, dword ptr [ebp + 8]
            mov ecx, dword ptr [esp + 0x10]
            mov eax, dword ptr [edx + 0x18c]
            mov dword ptr [eax + 0x58], ecx
    L004881d2:
            mov ecx, dword ptr [ebp + 8]
            call UpdateCollisionObject
            mov edx, dword ptr [ebp + 8]
            mov eax, dword ptr [ebp + 0x10]
            mov ecx, 6
            mov dword ptr [edx], eax
            mov ebp, dword ptr [ebp + 8]
            lea esi, [ebp + 0x24]
            lea edi, [ebp + 0x3c]
            rep movsd
    L004881f2:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x264
            ret
    L004881fd:
            mov ecx, ebx
            call GetAScalar
            call _ftol
            lea ecx, [esp + 0x30]
            lea edx, [esp + 0x2c]
            push ecx
            push edx
            lea edx, [esp + 0x30]
            mov ecx, ebx
            mov dword ptr [ebp], eax
            call GetThreeFloats
            mov eax, dword ptr [ebp + 8]
            mov ecx, dword ptr [esp + 0x28]
            mov dword ptr [eax + 0x14], ecx
            mov edx, dword ptr [ebp + 8]
            mov eax, dword ptr [esp + 0x2c]
            mov dword ptr [edx + 0x18], eax
            mov ecx, dword ptr [ebp + 8]
            mov edx, dword ptr [esp + 0x30]
            lea eax, [esp + 0x38]
            mov dword ptr [ecx + 0x1c], edx
            lea ecx, [esp + 0x20]
            push eax
            push ecx
            lea edx, [esp + 0x38]
            mov ecx, ebx
            call GetThreeFloats
            mov ecx, dword ptr [ebp + 8]
            mov edx, ebx
            add ecx, 0x20
            call ReadMechanicsShapes
            lea edx, [esp + 0x2c]
            mov ecx, ebx
            push edx
            lea edx, [esp + 0x2c]
            call GetPairOfFloats
            lea edx, [esp + 0x18]
            mov eax, dword ptr [esp + 0x28]
            mov ecx, dword ptr [esp + 0x2c]
            mov dword ptr [ebp + 0xcc], eax
            lea eax, [esp + 0x10]
            mov dword ptr [ebp + 0xd0], ecx
            push edx
            push eax
            lea edx, [esp + 0x24]
            mov ecx, ebx
            call GetThreeFloats
            mov ecx, ebx
            call GetAScalar
            fst dword ptr [esp + 0x14]
            fmul qword ptr [gDouble_0058a068+0x1c8]
            call _ftol
            and eax, 0xffff
            mov ecx, ebx
            mov dword ptr [esp + 0x28], eax
            fild dword ptr [esp + 0x28]
            fmul qword ptr [gDouble_0058a068+0x1d0]
            fcos
            fstp dword ptr [ebp + 0xd8]
            fld dword ptr [esp + 0x14]
            fmul qword ptr [gDouble_0058a068+0x1d8]
            fld st(0)
            fmul st(0), st(1)
            fstp dword ptr [ebp + 0x18]
            fstp st(0)
            call GetAScalar
            fld dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x18]
            fxch st(2)
            fmul dword ptr [gDouble_0058a068+0x1e0]
            fld dword ptr [esp + 0x1c]
            fxch st(2)
            fmul dword ptr [esp + 0x10]
            fxch st(3)
            fmul dword ptr [esp + 0x18]
            fxch st(2)
            fmul dword ptr [esp + 0x1c]
            fld dword ptr [esp + 0x14]
            fxch st(2)
            fst dword ptr [esp + 0x10]
            fxch st(4)
            fst dword ptr [esp + 0x14]
            fxch st(3)
            fxch st(4)
            fxch st(3)
            fxch st(2)
            fxch st(1)
            fld st(4)
            fxch st(4)
            fmul dword ptr [esp + 0x10]
            fstp dword ptr [esp + 0x18]
            fxch st(4)
            fadd dword ptr [esp + 0x14]
            fxch st(1)
            fcomp dword ptr [gDouble_0058a068+0x1e8]
            fxch st(3)
            fstp dword ptr [esp + 0x10]
            mov ecx, dword ptr [esp + 0x18]
            fadd dword ptr [esp + 0x10]
            fxch st(1)
            fadd dword ptr [esp + 0x10]
            fxch st(2)
            fmul dword ptr [gDouble_0058a068+0x1e4]
            fxch st(1)
            fmul dword ptr [gDouble_0058a068+0x1e4]
            fxch st(2)
            fmul dword ptr [gDouble_0058a068+0x1e4]
            fxch st(2)
            fstp dword ptr [ebp + 0xec]
            fxch st(1)
            fstp dword ptr [ebp + 0xe8]
            fnstsw ax
            mov dword ptr [ebp + 0xd4], ecx
            fstp dword ptr [ebp + 0xe4]
            test ah, 0x40
            jne L00488425
            mov edx, 0xdb
            mov ecx, 1
            call AllocatePhysicsJoint
            mov edx, dword ptr [ebp + 8]
            mov dword ptr [edx + 0x18c], eax
            mov eax, dword ptr [ebp + 8]
            mov ecx, dword ptr [eax + 0x18c]
            mov dword ptr [ebp + 0xf0], ecx
            mov edx, dword ptr [eax + 0x18c]
            mov dword ptr [edx], 3
            mov eax, dword ptr [ebp + 8]
            mov edx, dword ptr [esp + 0x30]
            mov ecx, dword ptr [eax + 0x18c]
            mov dword ptr [ecx + 8], edx
            mov eax, dword ptr [ebp + 8]
            mov edx, dword ptr [esp + 0x20]
            mov ecx, dword ptr [eax + 0x18c]
            mov dword ptr [ecx + 0xc], edx
            mov eax, dword ptr [ebp + 8]
            mov edx, dword ptr [esp + 0x38]
            mov ecx, dword ptr [eax + 0x18c]
            mov dword ptr [ecx + 0x10], edx
            mov eax, dword ptr [ebp + 8]
            mov ecx, dword ptr [eax + 0x18c]
            mov dword ptr [ecx + 0x50], 1
            mov edx, dword ptr [ebp + 8]
            mov eax, dword ptr [edx + 0x18c]
            mov dword ptr [eax + 0x54], 0xa
            mov ecx, dword ptr [ebp + 8]
            mov eax, dword ptr [esp + 0x18]
            mov edx, dword ptr [ecx + 0x18c]
            mov dword ptr [edx + 0x58], eax
    L00488425:
            mov ecx, dword ptr [ebp + 8]
            call UpdateCollisionObject
            mov ecx, dword ptr [ebp + 8]
            mov edx, dword ptr [ebp + 0x10]
            mov dword ptr [ecx], edx
            mov eax, dword ptr [ebp + 8]
            mov ecx, 6
            lea esi, [eax + 0x24]
            lea edi, [eax + 0x3c]
            rep movsd
            mov ecx, ebx
            call GetAnInt
            xor edi, edi
            mov dword ptr [ebp + 0x6c], eax
            test eax, eax
            jle L00488498
            lea esi, [ebp + 0x84]
    L0048845b:
            lea edx, [esp + 0x174]
            mov ecx, ebx
            call GetALineAndDontArgue
            test ebp, ebp
            je L0048847a
            mov eax, dword ptr [ebp + 0xc]
            mov edx, 2
            cmp eax, 5
            jg L0048847f
    L0048847a:
            mov edx, 1
    L0048847f:
            lea ecx, [esp + 0x174]
            call GetSimpleMaterial
            mov dword ptr [esi], eax
            mov eax, dword ptr [ebp + 0x6c]
            inc edi
            add esi, 4
            cmp edi, eax
            jl L0048845b
    L00488498:
            mov eax, dword ptr [ebp + 8]
            mov dword ptr [eax + 0x64], 0x3ecccccd
            mov eax, dword ptr [esp + 0x34]
            cmp eax, 0x64
            jle L004881f2
            mov edi, 0x10000
            mov esi, 0xb
    L004884b9:
            lea edx, [esp + 0x74]
            mov ecx, ebx
            call GetAString
            mov edx, offset gNet_avail_names+0x5e8
            lea ecx, [esp + 0x74]
            call DRStricmp
            test eax, eax
            je L004881f2
            mov edx, offset gNet_avail_names+0x5d8
            lea ecx, [esp + 0x74]
            call DRStricmp
            test eax, eax
            jne L00488501
            mov ecx, ebx
            call GetAScalar
            mov ecx, dword ptr [ebp + 8]
            fsub dword ptr [gDouble_0058a068+0x1bc]
            fstp dword ptr [ecx + 0x60]
            jmp L004884b9
    L00488501:
            mov edx, offset gNet_avail_names+0x5c8
            lea ecx, [esp + 0x74]
            call DRStricmp
            test eax, eax
            jne L00488528
            mov ecx, ebx
            call GetAScalar
            mov edx, dword ptr [ebp + 8]
            fsub dword ptr [gDouble_0058a068+0x1bc]
            fstp dword ptr [edx + 0x64]
            jmp L004884b9
    L00488528:
            mov edx, offset gNet_avail_names+0x5c0
            lea ecx, [esp + 0x74]
            call DRStricmp
            test eax, eax
            jne L0048855f
            mov ecx, ebx
            call GetAScalar
            fstp dword ptr [ebp + 0xdc]
            mov ecx, ebx
            call GetAScalar
            fmul dword ptr [gDouble_0058a068+0x1e0]
            fstp dword ptr [ebp + 0xe0]
            jmp L004884b9
    L0048855f:
            mov edx, offset gNet_avail_names+0x5ac
            lea ecx, [esp + 0x74]
            call DRStricmp
            test eax, eax
            jne L004885ff
            lea eax, [esp + 0x28]
            lea ecx, [esp + 0x38]
            push eax
            push ecx
            lea edx, [esp + 0x3c]
            mov ecx, ebx
            call GetThreeFloats
            mov ecx, ebx
            call GetAScalar
            fstp dword ptr [esp + 0x10]
            mov ecx, ebx
            call GetAnInt
            mov edx, 0xdb
            mov ecx, eax
            call AllocatePhysicsJoint
            mov edx, dword ptr [ebp + 8]
            mov ecx, dword ptr [esp + 0x10]
            mov dword ptr [edx + 0x18c], eax
            mov eax, dword ptr [ebp + 8]
            mov eax, dword ptr [eax + 0x18c]
            mov dword ptr [ebp + 0xf0], eax
            mov dword ptr [eax + 4], ecx
            mov edx, dword ptr [ebp + 0xf0]
            mov eax, dword ptr [esp + 0x34]
            mov dword ptr [edx + 8], eax
            mov ecx, dword ptr [ebp + 0xf0]
            mov edx, dword ptr [esp + 0x38]
            mov dword ptr [ecx + 0xc], edx
            mov eax, dword ptr [ebp + 0xf0]
            mov ecx, dword ptr [esp + 0x28]
            mov dword ptr [eax + 0x10], ecx
            mov edx, dword ptr [ebp + 0xf0]
            mov dword ptr [edx], 3
            jmp L004884b9
    L004885ff:
            mov edx, offset gNet_avail_names+0x598
            lea ecx, [esp + 0x74]
            call DRStricmp
            test eax, eax
            jne L004886b4
            lea eax, [esp + 0x48]
            lea ecx, [esp + 0x30]
            push eax
            push ecx
            lea edx, [esp + 0x34]
            mov ecx, ebx
            call GetThreeFloats
            lea edx, [esp + 0x70]
            lea eax, [esp + 0x6c]
            push edx
            push eax
            lea edx, [esp + 0x70]
            mov ecx, ebx
            call GetThreeFloats
            mov ecx, ebx
            call GetAScalar
            fstp dword ptr [esp + 0x10]
            mov ecx, ebx
            call GetAnInt
            mov edx, 0xdb
            mov ecx, eax
            call AllocatePhysicsJoint
            mov ecx, dword ptr [ebp + 8]
            mov dword ptr [ecx + 0x18c], eax
            mov edx, dword ptr [ebp + 8]
            mov ecx, dword ptr [esp + 0x10]
            mov eax, dword ptr [edx + 0x18c]
            mov dword ptr [ebp + 0xf0], eax
            mov dword ptr [eax + 4], ecx
            mov edx, dword ptr [ebp + 0xf0]
            mov eax, dword ptr [esp + 0x2c]
            mov dword ptr [edx + 8], eax
            mov ecx, dword ptr [ebp + 0xf0]
            mov edx, dword ptr [esp + 0x30]
            mov dword ptr [ecx + 0xc], edx
            mov eax, dword ptr [ebp + 0xf0]
            mov ecx, dword ptr [esp + 0x48]
            mov dword ptr [eax + 0x10], ecx
            mov edx, dword ptr [ebp + 0xf0]
            mov dword ptr [edx], 3
            jmp L004884b9
    L004886b4:
            mov edx, offset gNet_avail_names+0x580
            lea ecx, [esp + 0x74]
            call DRStricmp
            test eax, eax
            jne L004889c4
            lea eax, [esp + 0x44]
            lea ecx, [esp + 0x40]
            push eax
            push ecx
            lea edx, [esp + 0x54]
            mov ecx, ebx
            call GetThreeFloats
            lea edx, [esp + 0x1c]
            lea eax, [esp + 0x14]
            push edx
            push eax
            lea edx, [esp + 0x20]
            mov ecx, ebx
            call GetThreeFloats
            mov ecx, ebx
            call GetAScalar
            fstp dword ptr [esp + 0x3c]
            mov edx, 0xdb
            mov ecx, 1
            call AllocatePhysicsJoint
            mov ecx, dword ptr [ebp + 8]
            fld dword ptr [esp + 0x18]
            mov dword ptr [ecx + 0x18c], eax
            mov edx, dword ptr [ebp + 8]
            mov ecx, dword ptr [esp + 0x4c]
            mov eax, dword ptr [edx + 0x18c]
            mov dword ptr [ebp + 0xf0], eax
            mov dword ptr [eax + 8], ecx
            mov edx, dword ptr [ebp + 0xf0]
            mov eax, dword ptr [esp + 0x40]
            mov dword ptr [edx + 0xc], eax
            mov ecx, dword ptr [ebp + 0xf0]
            mov edx, dword ptr [esp + 0x44]
            fabs
            fld dword ptr [esp + 0x14]
            mov dword ptr [ecx + 0x10], edx
            mov eax, dword ptr [ebp + 0xf0]
            mov ecx, dword ptr [esp + 0x18]
            fabs
            fstp qword ptr [esp + 0x20]
            mov dword ptr [eax + 0x38], ecx
            mov edx, dword ptr [ebp + 0xf0]
            mov eax, dword ptr [esp + 0x14]
            fcom qword ptr [esp + 0x20]
            mov dword ptr [edx + 0x3c], eax
            mov ecx, dword ptr [ebp + 0xf0]
            mov edx, dword ptr [esp + 0x1c]
            mov dword ptr [ecx + 0x40], edx
            mov ecx, dword ptr [ebp + 0xf0]
            fnstsw ax
            add ecx, 0x20
            test ah, 0x41
            je L004887bb
            fld dword ptr [esp + 0x1c]
            fabs
            fxch st(1)
            fcompp
            fnstsw ax
            test ah, 0x41
            je L004887bd
            fld dword ptr [esp + 0x1c]
            fstp dword ptr [ecx + 4]
            fld dword ptr [esp + 0x14]
            fchs
            fstp dword ptr [ecx + 8]
            mov dword ptr [ecx], 0
            jmp L00488802
    L004887bb:
            fstp st(0)
    L004887bd:
            fld qword ptr [esp + 0x20]
            fld dword ptr [esp + 0x1c]
            fabs
            fxch st(1)
            fcompp
            fnstsw ax
            test ah, 0x41
            je L004887ec
            fld dword ptr [esp + 0x1c]
            fchs
            _emit 0xd9
        _emit 0x19
            fld dword ptr [esp + 0x18]
            fchs
            fstp dword ptr [ecx + 8]
            mov dword ptr [ecx + 4], 0
            jmp L00488802
    L004887ec:
            fld dword ptr [esp + 0x14]
            _emit 0xd9
        _emit 0x19
            fld dword ptr [esp + 0x18]
            fchs
            fstp dword ptr [ecx + 4]
            mov dword ptr [ecx + 8], 0
    L00488802:
            fld dword ptr [ecx + 4]
            fld dword ptr [ecx + 8]
            _emit 0xd9
        _emit 0x01
            fxch st(2)
            fst dword ptr [esp + 0x10]
            fxch st(1)
            fst dword ptr [esp + 0x20]
            fxch st(1)
            fmul dword ptr [esp + 0x10]
            fxch st(1)
            fmul dword ptr [esp + 0x20]
            fld st(2)
            fmul st(0), st(3)
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fsqrt
            fstp st(1)
            fcom dword ptr [gDouble_0058a068+0x1ec]
            fnstsw ax
            test ah, 0x41
            jne L00488865
            fdivr qword ptr [gDouble_0058a068+0x1f0]
            fld dword ptr [ecx + 8]
            fld dword ptr [ecx + 4]
            fld st(2)
            _emit 0xd8
        _emit 0x09
            fxch st(1)
            fmul st(0), st(3)
            fxch st(2)
            fmul st(0), st(3)
            fxch st(1)
            _emit 0xd9
        _emit 0x19
            fxch st(1)
            fstp dword ptr [ecx + 4]
            fstp dword ptr [ecx + 8]
            fstp st(0)
            jmp L00488875
    L00488865:
            xor eax, eax
            mov dword ptr [ecx], 0x3f800000
            fstp st(0)
            mov dword ptr [ecx + 4], eax
            mov dword ptr [ecx + 8], eax
    L00488875:
            mov eax, dword ptr [ebp + 0xf0]
            push 0xde
            push 0x14
            fld dword ptr [eax + 0x24]
            fld dword ptr [eax + 0x28]
            fmul dword ptr [esp + 0x1c]
            fxch st(1)
            fmul dword ptr [esp + 0x24]
            fld dword ptr [esp + 0x44]
            fxch st(1)
            fsubp st(2), st(0)
            fxch st(1)
            fstp dword ptr [eax + 0x2c]
            mov eax, dword ptr [ebp + 0xf0]
            fld dword ptr [eax + 0x28]
            fld dword ptr [eax + 0x20]
            fmul dword ptr [esp + 0x24]
            fxch st(1)
            fmul dword ptr [esp + 0x20]
            fsubp st(1), st(0)
            fstp dword ptr [eax + 0x30]
            mov eax, dword ptr [ebp + 0xf0]
            fld dword ptr [eax + 0x20]
            fld dword ptr [eax + 0x24]
            fmul dword ptr [esp + 0x20]
            fxch st(1)
            fmul dword ptr [esp + 0x1c]
            fsubp st(1), st(0)
            fstp dword ptr [eax + 0x34]
            mov eax, dword ptr [ebp + 0xf0]
            mov dword ptr [eax], 5
            mov edx, dword ptr [ebp + 0x100]
            mov ecx, dword ptr [ebp + 0xf0]
            or edx, edi
            mov dword ptr [ebp + 0x100], edx
            mov dword ptr [ecx + 0x50], 1
            mov edx, dword ptr [ebp + 0xf0]
            mov dword ptr [edx + 0x54], esi
            mov eax, dword ptr [ebp + 0xf0]
            fstp dword ptr [eax + 0x58]
            call BrMemAllocate
            add esp, 8
            mov ecx, ebx
            mov dword ptr [ebp + 0xf4], eax
            call GetAScalar
            mov edx, dword ptr [ebp + 0xf4]
            mov ecx, ebx
            _emit 0xd9
        _emit 0x1a
            call GetAScalar
            mov eax, dword ptr [ebp + 0xf4]
            mov ecx, ebx
            fstp dword ptr [eax + 4]
            call GetAScalar
            mov edx, dword ptr [ebp + 0xf4]
            mov ecx, ebx
            fstp dword ptr [edx + 8]
            call GetAScalar
            mov eax, dword ptr [ebp + 0xf4]
            mov ecx, ebx
            fstp dword ptr [eax + 0xc]
            call GetAnInt
            mov ecx, dword ptr [ebp + 0xf4]
            mov dword ptr [ecx + 0x10], eax
            mov eax, dword ptr [ebp + 0xf4]
            fld dword ptr [eax + 8]
            fchs
            fmul qword ptr [gDouble_0058a068+0x1f8]
            fldl2e
            fmulp st(1), st(0)
            fld st(0)
            frndint
            fxch st(1)
            fsub st(0), st(1)
            f2xm1
            fld1
            faddp st(1), st(0)
            fscale
            fstp st(1)
            fstp dword ptr [eax + 8]
            mov eax, dword ptr [ebp + 0xf4]
            fld dword ptr [eax + 0xc]
            fchs
            fmul qword ptr [gDouble_0058a068+0x1f8]
            fldl2e
            fmulp st(1), st(0)
            fld st(0)
            frndint
            fxch st(1)
            fsub st(0), st(1)
            f2xm1
            fld1
            faddp st(1), st(0)
            fscale
            fstp st(1)
            fstp dword ptr [eax + 0xc]
            jmp L004884b9
    L004889c4:
            mov edx, offset gNet_avail_names+0x570
            lea ecx, [esp + 0x74]
            call DRStricmp
            test eax, eax
            jne L004889e5
            or dword ptr [ebp + 0x100], 0x20000
            jmp L004884b9
    L004889e5:
            mov edx, offset gNet_avail_names+0x55c
            lea ecx, [esp + 0x74]
            call DRStricmp
            test eax, eax
            jne L00488a06
            or dword ptr [ebp + 0x100], 0x40000
            jmp L004884b9
    L00488a06:
            mov edx, offset gNet_avail_names+0x548
            lea ecx, [esp + 0x74]
            call DRStricmp
            test eax, eax
            jne L00488a2a
            mov ecx, ebx
            call GetAnInt
            mov byte ptr [ebp + 0xfd], al
            jmp L004884b9
    L00488a2a:
            mov edx, offset gNet_avail_names+0x53c
            lea ecx, [esp + 0x74]
            call DRStricmp
            test eax, eax
            jne L00488a4b
            mov edx, dword ptr [ebp + 8]
            mov byte ptr [edx + 0x4d4], 1
            jmp L004884b9
    L00488a4b:
            mov edx, offset gNet_avail_names+0x530
            lea ecx, [esp + 0x74]
            call DRStricmp
            test eax, eax
            jne L00488a74
            mov eax, dword ptr [ebp + 8]
            mov ecx, dword ptr [eax + 0x19c]
            or ch, 8
            mov dword ptr [eax + 0x19c], ecx
            jmp L004884b9
    L00488a74:
            mov edx, offset gNet_avail_names+0x518
            lea ecx, [esp + 0x74]
            call DRStricmp
            test eax, eax
            jne L00488a9d
            mov eax, dword ptr [ebp + 8]
            mov ecx, dword ptr [eax + 0x19c]
            or cl, 0x80
            mov dword ptr [eax + 0x19c], ecx
            jmp L004884b9
    L00488a9d:
            mov edx, offset gNet_avail_names+0x508
            lea ecx, [esp + 0x74]
            call DRStricmp
            test eax, eax
            jne L00488abe
            or dword ptr [ebp + 0x100], 0x80000
            jmp L004884b9
    L00488abe:
            mov edx, offset gNet_avail_names+0x500
            lea ecx, [esp + 0x74]
            call DRStricmp
            test eax, eax
            jne L00488adf
            or dword ptr [ebp + 0x100], 0x200000
            jmp L004884b9
    L00488adf:
            push 0xae
            call FatalError
            add esp, 4
            jmp L004884b9
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00450600
int C2_HOOK_CDECL LinkyCallback(br_actor* pActor, void* pData) {
    tLinkyCallback_context* context;
    int i;

    context = pData;
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
    return 0;
}

void C2_HOOK_FASTCALL LinkDroneActorsToModelsAndSetRenderStyle(tDrone_spec* pDrone, int pModel_start, int pModel_end) {
    tLinkyCallback_context data;

    data.drone = pDrone;
    data.model_start = pModel_start;
    data.model_end = pModel_end;
    DRActorEnumRecurse(pDrone->model_actor, LinkyCallback, &data);
}

// FUNCTION: CARMA2_HW 0x00450150
void C2_NAKED C2_HOOK_FASTCALL LoadDroneActorsModels(tDrone_spec* pDrone) {
#ifndef CARPOCALYPSE2_MATCHING
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
#else
    __asm {
            sub esp, 0x33c
            push ebx
            push ebp
            push esi
            push edi
            mov ebx, ecx
            call PossibleService
            lea eax, [esp + 0x1c]
            push eax
            call BrMatrix34Identity
            mov eax, dword ptr [ebx + 0xf0]
            xor esi, esi
            add esp, 4
            cmp eax, esi
            je L0045019d
            cmp dword ptr [eax + 0xc], esi
            je L00450188
            push eax
            call BrActorRemove
            add esp, 4
    L00450188:
            mov ecx, dword ptr [ebx + 0xf0]
            push ecx
            call BrActorFree
            add esp, 4
            mov dword ptr [ebx + 0xf0], esi
    L0045019d:
            mov eax, dword ptr [ebx + 0xec]
            cmp eax, esi
            je L004501e1
            add eax, 0x2c
            lea edx, [esp + 0x1c]
            push eax
            push edx
            call BrMatrix34Copy
            mov eax, dword ptr [ebx + 0xec]
            add esp, 8
            cmp dword ptr [eax + 0xc], esi
            je L004501cc
            push eax
            call BrActorRemove
            add esp, 4
    L004501cc:
            mov eax, dword ptr [ebx + 0xec]
            push eax
            call BrActorFree
            add esp, 4
            mov dword ptr [ebx + 0xec], esi
    L004501e1:
            mov ecx, dword ptr [gDefault_drone_direction+0x5f0]
            mov dx, word ptr [gDefault_drone_direction+0x5f4]
            mov al, byte ptr [gDefault_drone_direction+0x5f6]
            mov dword ptr [gCurrent_load_directory], ecx
            mov word ptr [gCurrent_load_directory+0x4], dx
            mov byte ptr [gCurrent_load_directory+0x6], al
            mov edi, dword ptr [ebx + 4]
            or ecx, 0xffffffff
            xor eax, eax
            mov ebp, 3
            repne scasb
            not ecx
            sub edi, ecx
            push offset gCurrent_load_directory
            mov edx, ecx
            mov esi, edi
            mov edi, offset gCurrent_load_name
            shr ecx, 2
            rep movsd
            mov ecx, edx
            lea edx, [esp + 0x50]
            and ecx, ebp
            rep movsb
            mov edi, offset gApplication_path
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            lea edx, [esp + 0x50]
            shr ecx, 2
            rep movsd
            mov ecx, eax
            and ecx, ebp
            rep movsb
            lea ecx, [esp + 0x50]
            call PathCat
            lea edx, [esp + 0x4c]
            lea ecx, [esp + 0x4c]
            push offset gCurrent_load_name
            call PathCat
            mov ecx, dword ptr [ebx + 4]
            mov eax, dword ptr [ecx + 0x70]
            test eax, eax
            jge L004503d6
            mov edx, offset gDefault_drone_direction+0x73c
            xor ecx, ecx
            call PrintMemoryDump
            lea edx, [esp + 0x4c]
            mov ecx, offset gDroneStorage
            call LoadAllImagesInDirectory
            lea edx, [esp + 0x4c]
            mov ecx, offset gDroneStorage
            call LoadAllShadeTablesInDirectory
            mov esi, dword ptr [gDroneStorage+0x8]
            lea edx, [esp + 0x4c]
            push 2
            mov ecx, offset gDroneStorage
            call LoadAllMaterialsInDirectory
            cmp esi, dword ptr [gDroneStorage+0x8]
            jge L0045031e
            mov edi, 4
    L004502cc:
            mov edx, dword ptr [gDroneStorage+0x38]
            mov eax, dword ptr [edx + esi*4]
            test eax, eax
            je L0045030f
            mov ecx, dword ptr [eax + 0x40]
            test ecx, ecx
            je L0045030f
            mov edx, dword ptr [eax + 0x20]
            push 0x7fff
            or edx, ebp
            mov dword ptr [eax + 0x20], edx
            mov eax, dword ptr [gDroneStorage+0x38]
            mov eax, dword ptr [eax + esi*4]
            mov ecx, dword ptr [eax + 0x20]
            or ecx, edi
            mov dword ptr [eax + 0x20], ecx
            mov ecx, dword ptr [gDroneStorage+0x38]
            mov edx, dword ptr [ecx + esi*4]
            push edx
            call BrMaterialUpdate
            add esp, 8
    L0045030f:
            call PossibleService
            mov eax, dword ptr [gDroneStorage+0x8]
            inc esi
            cmp esi, eax
            jl L004502cc
    L0045031e:
            mov eax, dword ptr [ebx + 4]
            mov ecx, dword ptr [gDroneStorage+0x4]
            lea edx, [esp + 0x4c]
            mov dword ptr [eax + 0x70], ecx
            mov ecx, offset gDroneStorage
            call LoadAllModelsInDirectory
            mov eax, dword ptr [ebx + 4]
            mov edx, dword ptr [gDroneStorage+0x4]
            sub edx, dword ptr [eax + 0x70]
            mov dword ptr [eax + 0x74], edx
            mov ecx, dword ptr [ebx + 4]
            fld dword ptr [ecx + 0x28]
            fcomp dword ptr [gDrone_render_zero]
            fnstsw ax
            test ah, 0x40
            jne L004503ca
            mov dword ptr [ecx + 0x78], 0xffffffff
            mov edx, dword ptr [ebx + 4]
            mov esi, dword ptr [edx + 0x70]
            mov eax, dword ptr [edx + 0x74]
            add eax, esi
            cmp esi, eax
            jge L0045039d
    L00450370:
            mov ecx, dword ptr [gDroneStorage+0x2c]
            mov eax, dword ptr [ecx + esi*4]
            mov ecx, dword ptr [eax + 4]
            call DRStricmp
            test eax, eax
            je L00450397
            mov edx, dword ptr [ebx + 4]
            inc esi
            mov ecx, dword ptr [edx + 0x74]
            mov edi, dword ptr [edx + 0x70]
            add ecx, edi
            cmp esi, ecx
            jl L00450370
            jmp L0045039d
    L00450397:
            mov edx, dword ptr [ebx + 4]
            mov dword ptr [edx + 0x78], esi
    L0045039d:
            mov eax, dword ptr [ebx + 4]
            mov ecx, dword ptr [eax + 0x78]
            test ecx, ecx
            jge L004503ca
            push eax
            push eax
            lea eax, [esp + 0x154]
            push offset gDefault_drone_direction+0x710
            push eax
            call sprintf
            add esp, 0x10
            lea ecx, [esp + 0x14c]
            call PDFatalError
    L004503ca:
            mov edx, offset gDefault_drone_direction+0x6ec
            xor ecx, ecx
            call PrintMemoryDump
    L004503d6:
            push 0
            push 0
            call BrActorAllocate
            add esp, 8
            mov dword ptr [ebx + 0xec], eax
            lea ecx, [esp + 0x1c]
            add eax, 0x2c
            push ecx
            push eax
            call BrMatrix34Copy
            mov edx, dword ptr [ebx]
            add esp, 8
            lea eax, [esp + 0x14c]
            push edx
            push offset gDefault_drone_direction+0x6e4
            push eax
            call sprintf
            mov edx, dword ptr [ebx + 0xec]
            add esp, 0xc
            lea ecx, [esp + 0x14c]
            push ecx
            push edx
            call BrResStrDup
            mov ecx, dword ptr [ebx + 0xec]
            add esp, 8
            mov dword ptr [ecx + 0x14], eax
            mov edx, dword ptr [ebx + 0xec]
            mov eax, dword ptr [gNon_track_actor]
            push edx
            push eax
            call BrActorAdd
            mov ecx, dword ptr [ebx + 4]
            add esp, 8
            lea edx, [esp + 0x4c]
            push ecx
            lea ecx, [esp + 0x250]
            call PathCat
            mov edi, offset gDefault_drone_direction+0x6dc
            or ecx, 0xffffffff
            xor eax, eax
            lea edx, [esp + 0x24c]
            repne scasb
            not ecx
            sub edi, ecx
            mov esi, edi
            mov edi, edx
            mov edx, ecx
            or ecx, 0xffffffff
            repne scasb
            mov ecx, edx
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, edx
            lea eax, [esp + 0x24c]
            and ecx, ebp
            push eax
            rep movsb
            call BrActorLoad
            add esp, 4
            mov dword ptr [ebx + 0xf0], eax
            test eax, eax
            jne L004504c9
            mov ecx, dword ptr [ebx + 4]
            lea edx, [esp + 0x14c]
            push ecx
            push offset gDefault_drone_direction+0x6b4
            push edx
            call sprintf
            add esp, 0xc
            lea ecx, [esp + 0x14c]
            call PDFatalError
    L004504c9:
            mov eax, dword ptr [ebx + 4]
            mov dword ptr [esp + 0x10], ebx
            mov ecx, dword ptr [eax + 0x70]
            mov edx, dword ptr [eax + 0x74]
            lea eax, [esp + 0x10]
            mov dword ptr [esp + 0x14], ecx
            mov ecx, dword ptr [ebx + 0xf0]
            mov dword ptr [esp + 0x18], edx
            push eax
            mov edx, offset LinkyCallback
            call DRActorEnumRecurse
            mov ecx, dword ptr [ebx + 0xec]
            mov byte ptr [ecx + 0x20], 1
            mov edx, dword ptr [ebx + 0xf0]
            mov eax, dword ptr [ebx + 0xec]
            push edx
            push eax
            call BrActorAdd
            mov ecx, dword ptr [ebx + 4]
            add esp, 8
            mov edx, dword ptr [ecx + 0x2c]
            test dl, 0x20
            jne L004505eb
            mov eax, dword ptr [ebx + 0xf0]
            fld dword ptr [ecx + 0x44]
            mov eax, dword ptr [eax + 0x18]
            add eax, 0x34
            or edx, 0x20
            fld dword ptr [eax + 0xc]
            _emit 0xd8
        _emit 0x20
            fxch st(1)
            fmul qword ptr [gDrone_select_max_distance_squared+0x84]
            _emit 0xd9
        _emit 0x00
            fxch st(1)
            fmulp st(2), st(0)
            fld dword ptr [ecx + 0x4c]
            fmul qword ptr [gDrone_select_max_distance_squared+0x84]
            fxch st(1)
            faddp st(2), st(0)
            fxch st(1)
            fstp dword ptr [ecx + 0x54]
            fld dword ptr [eax + 0x10]
            fsub dword ptr [eax + 4]
            fld dword ptr [ecx + 0x40]
            fxch st(2)
            fmulp st(1), st(0)
            fxch st(1)
            fmul qword ptr [gDrone_select_max_distance_squared+0x84]
            fxch st(1)
            fadd dword ptr [eax + 4]
            fld dword ptr [ecx + 0x48]
            fxch st(1)
            fstp dword ptr [ecx + 0x58]
            fld dword ptr [eax + 0x14]
            fsub dword ptr [eax + 8]
            fxch st(1)
            fmul qword ptr [gDrone_select_max_distance_squared+0x84]
            fxch st(2)
            fmulp st(1), st(0)
            fld dword ptr [ecx + 0x50]
            fxch st(1)
            fadd dword ptr [eax + 8]
            fxch st(1)
            fmul qword ptr [gDrone_select_max_distance_squared+0x84]
            fld dword ptr [ecx + 0x3c]
            fxch st(2)
            fstp dword ptr [ecx + 0x5c]
            fld dword ptr [eax + 0xc]
            _emit 0xd8
        _emit 0x20
            fld dword ptr [eax + 0xc]
            fxch st(4)
            fmulp st(1), st(0)
            fxch st(2)
            fmul qword ptr [gDrone_select_max_distance_squared+0x84]
            fxch st(2)
            fsubp st(3), st(0)
            fxch st(2)
            fstp dword ptr [ecx + 0x60]
            fld dword ptr [eax + 0x10]
            fsub dword ptr [eax + 4]
            fld dword ptr [eax + 0x10]
            fxch st(3)
            fmulp st(1), st(0)
            fsubp st(2), st(0)
            fxch st(1)
            fstp dword ptr [ecx + 0x64]
            fld dword ptr [eax + 0x14]
            fsub dword ptr [eax + 8]
            fld dword ptr [eax + 0x14]
            fxch st(2)
            fmulp st(1), st(0)
            mov dword ptr [ecx + 0x2c], edx
            fsubp st(1), st(0)
            fstp dword ptr [ecx + 0x68]
    L004505eb:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x33c
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00450b30
intptr_t C2_HOOK_CDECL MrFindy(br_actor* pActor, void* data) {
    const char* name = data;

    if (DRStricmp(pActor->identifier, name) == 0) {
        return (intptr_t)pActor;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00450b50
tDrone_crush_data* C2_HOOK_FASTCALL GetDroneCrushData(tPhysics_object* pObject) {
    tDrone_spec* pDrone;
    int i;

    pDrone = pObject->owner;
    if (pDrone->form->crushability == gDrone_render_zero) {
        return NULL;
    }
    if (pDrone->field_0xf4 < 0) {
        for (i = 0; i < pDrone->form->count_models; i++) {
            if (pDrone->form->field_0x80[i] == 0) {
                pDrone->form->field_0x80[i] = 1;
                pDrone->field_0xf4 = i;
                pDrone->model_actor->model = pDrone->form->models[i];
                goto selected_model;
            }
        }
        return NULL;
    }
selected_model:
    gDrone_crush_data.model = pDrone->model_actor->model;
    gDrone_crush_data.crushability = pDrone->form->crushability;
    gDrone_crush_data.bounds = pDrone->form->field_0x54;
    return &gDrone_crush_data;
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
void C2_NAKED C2_HOOK_FASTCALL LoadFunksAndGrooves(tDrone_spec* pDrone, FILE* pF) {
#ifndef CARPOCALYPSE2_MATCHING

    char the_spare[256];
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
        char s[512];
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
#else
    __asm {
            sub esp, 0x208
            push ebx
            push ebp
            push esi
            push edi
            mov edi, edx
            mov ebp, ecx
            mov ecx, edi
            mov dword ptr [esp + 0x14], edi
            mov dword ptr [esp + 0x10], ebp
            call PFfeof
            test eax, eax
            jne L004506f7
    L004506a1:
            lea edx, [esp + 0x118]
            mov ecx, edi
            call GetALineAndDontArgue
            mov esi, offset gDefault_drone_direction+0x78c
            lea eax, [esp + 0x118]
    L004506bb:
            mov dl, byte ptr [eax]
            mov bl, byte ptr [esi]
            mov cl, dl
            cmp dl, bl
            jne L004506e3
            test cl, cl
            je L004506df
            mov dl, byte ptr [eax + 1]
            mov bl, byte ptr [esi + 1]
            mov cl, dl
            cmp dl, bl
            jne L004506e3
            add eax, 2
            add esi, 2
            test cl, cl
            jne L004506bb
    L004506df:
            xor eax, eax
            jmp L004506e8
    L004506e3:
            sbb eax, eax
            sbb eax, -1
    L004506e8:
            test eax, eax
            je L00450708
            mov ecx, edi
            call PFfeof
            test eax, eax
            je L004506a1
    L004506f7:
            mov eax, dword ptr [ebp + 4]
            push eax
            push 0xaf
            call FatalError
            add esp, 8
    L00450708:
            mov eax, dword ptr [ebp + 0x5d0]
            test eax, eax
            jne L0045072a
            push 0xf2
            push 0xa4
            call BrMemAllocate
            add esp, 8
            mov dword ptr [ebp + 0x5d0], eax
    L0045072a:
            lea edx, [esp + 0x18]
            mov ecx, edi
            call GetALineAndDontArgue
            mov esi, offset gDefault_drone_direction+0x770
            lea eax, [esp + 0x18]
    L0045073e:
            mov dl, byte ptr [eax]
            mov bl, byte ptr [esi]
            mov cl, dl
            cmp dl, bl
            jne L00450766
            test cl, cl
            je L00450762
            mov dl, byte ptr [eax + 1]
            mov bl, byte ptr [esi + 1]
            mov cl, dl
            cmp dl, bl
            jne L00450766
            add eax, 2
            add esi, 2
            test cl, cl
            jne L0045073e
    L00450762:
            xor eax, eax
            jmp L0045076b
    L00450766:
            sbb eax, eax
            sbb eax, -1
    L0045076b:
            test eax, eax
            je L00450b12
            jmp L0045077d
    L00450775:
            mov edi, dword ptr [esp + 0x14]
            mov ebp, dword ptr [esp + 0x10]
    L0045077d:
            mov ecx, edi
            call PFfeof
            test eax, eax
            jne L00450b12
            mov eax, dword ptr [ebp + 0x5d0]
            cmp dword ptr [eax], 0xa
            jl L004507a8
            mov ecx, dword ptr [ebp + 4]
            push ecx
            push 0xaf
            call FatalError
            add esp, 8
    L004507a8:
            mov edx, dword ptr [esp + 0x10]
            mov edi, offset gGroove_funk_type_names
            mov eax, dword ptr [edx + 0x5d0]
            mov ecx, dword ptr [eax]
            shl ecx, 4
            xor ebp, ebp
            lea ebx, [ecx + eax + 4]
    L004507c2:
            mov esi, dword ptr [edi]
            lea eax, [esp + 0x18]
    L004507c8:
            mov dl, byte ptr [eax]
            mov cl, dl
            cmp dl, byte ptr [esi]
            jne L004507ec
            test cl, cl
            je L004507e8
            mov dl, byte ptr [eax + 1]
            mov cl, dl
            cmp dl, byte ptr [esi + 1]
            jne L004507ec
            add eax, 2
            add esi, 2
            test cl, cl
            jne L004507c8
    L004507e8:
            xor eax, eax
            jmp L004507f1
    L004507ec:
            sbb eax, eax
            sbb eax, -1
    L004507f1:
            test eax, eax
            je L00450b1d
            add edi, 4
            inc ebp
            cmp edi, offset gGroove_funk_type_names+0xc
            jb L004507c2
            or eax, 0xffffffff
    L00450808:
            mov byte ptr [ebx], al
            and eax, 0xff
            sub eax, 0
            je L004508e4
            dec eax
            jne L00450ab8
            mov esi, dword ptr [esp + 0x14]
            lea edx, [esp + 0x18]
            mov ecx, esi
            call GetALineAndDontArgue
            mov edi, dword ptr [esp + 0x10]
            lea eax, [esp + 0x18]
            push eax
            mov edx, offset MrFindy
            mov ecx, dword ptr [edi + 0xf0]
            call DRActorEnumRecurse
            test eax, eax
            mov dword ptr [ebx + 4], eax
            jne L0045085f
            mov ecx, dword ptr [edi + 4]
            push ecx
            push 0xaf
            call FatalError
            add esp, 8
    L0045085f:
            lea edx, [esp + 0x18]
            mov ecx, esi
            call GetALineAndDontArgue
            xor ebp, ebp
            mov edi, offset gReverseness_type_names
    L00450871:
            mov esi, dword ptr [edi]
            lea eax, [esp + 0x18]
    L00450877:
            mov dl, byte ptr [eax]
            mov cl, dl
            cmp dl, byte ptr [esi]
            jne L0045089b
            test cl, cl
            je L00450897
            mov dl, byte ptr [eax + 1]
            mov cl, dl
            cmp dl, byte ptr [esi + 1]
            jne L0045089b
            add eax, 2
            add esi, 2
            test cl, cl
            jne L00450877
    L00450897:
            xor eax, eax
            jmp L004508a0
    L0045089b:
            sbb eax, eax
            sbb eax, -1
    L004508a0:
            test eax, eax
            je L004508b2
            add edi, 4
            inc ebp
            cmp edi, offset gSelected_drone_path_node_index
            jb L00450871
            jmp L004508b6
    L004508b2:
            mov esi, ebp
            jmp L004508b9
    L004508b6:
            or esi, 0xffffffff
    L004508b9:
            test esi, esi
            jge L004508d2
            mov eax, dword ptr [esp + 0x10]
            mov ecx, dword ptr [eax + 4]
            push ecx
            push 0xaf
            call FatalError
            add esp, 8
    L004508d2:
            mov eax, dword ptr [ebx + 8]
            xor esi, eax
            and esi, 1
            xor esi, eax
            mov dword ptr [ebx + 8], esi
            jmp L00450ab8
    L004508e4:
            mov esi, dword ptr [esp + 0x14]
            lea edx, [esp + 0x18]
            mov ecx, esi
            call GetALineAndDontArgue
            mov edi, dword ptr [esp + 0x10]
            lea edx, [esp + 0x18]
            push edx
            mov edx, offset MrFindy
            mov ecx, dword ptr [edi + 0xf0]
            call DRActorEnumRecurse
            test eax, eax
            mov dword ptr [ebx + 4], eax
            jne L00450924
            mov eax, dword ptr [edi + 4]
            push eax
            push 0xaf
            call FatalError
            add esp, 8
    L00450924:
            lea edx, [esp + 0x18]
            mov ecx, esi
            call GetALineAndDontArgue
            xor esi, esi
            mov ebp, offset gAxis_names
    L00450936:
            mov edi, dword ptr [ebp]
            lea eax, [esp + 0x18]
    L0045093d:
            mov dl, byte ptr [eax]
            mov cl, dl
            cmp dl, byte ptr [edi]
            jne L00450961
            test cl, cl
            je L0045095d
            mov dl, byte ptr [eax + 1]
            mov cl, dl
            cmp dl, byte ptr [edi + 1]
            jne L00450961
            add eax, 2
            add edi, 2
            test cl, cl
            jne L0045093d
    L0045095d:
            xor eax, eax
            jmp L00450966
    L00450961:
            sbb eax, eax
            sbb eax, -1
    L00450966:
            test eax, eax
            je L00450979
            add ebp, 4
            inc esi
            cmp ebp, offset gAxis_names+0xc
            jb L00450936
            or esi, 0xffffffff
    L00450979:
            test esi, esi
            jge L00450992
            mov eax, dword ptr [esp + 0x10]
            mov ecx, dword ptr [eax + 4]
            push ecx
            push 0xaf
            call FatalError
            add esp, 8
    L00450992:
            mov eax, dword ptr [ebx + 0xc]
            mov ecx, dword ptr [esp + 0x14]
            xor esi, eax
            lea edx, [esp + 0x18]
            and esi, 3
            xor esi, eax
            mov dword ptr [ebx + 0xc], esi
            call GetALineAndDontArgue
            xor esi, esi
            mov ebp, offset gFunk_speed_control_names
    L004509b3:
            mov edi, dword ptr [ebp]
            lea eax, [esp + 0x18]
    L004509ba:
            mov dl, byte ptr [eax]
            mov cl, dl
            cmp dl, byte ptr [edi]
            jne L004509de
            test cl, cl
            je L004509da
            mov dl, byte ptr [eax + 1]
            mov cl, dl
            cmp dl, byte ptr [edi + 1]
            jne L004509de
            add eax, 2
            add edi, 2
            test cl, cl
            jne L004509ba
    L004509da:
            xor eax, eax
            jmp L004509e3
    L004509de:
            sbb eax, eax
            sbb eax, -1
    L004509e3:
            test eax, eax
            je L004509f6
            add ebp, 4
            inc esi
            cmp ebp, offset gReverseness_type_names
            jb L004509b3
            or esi, 0xffffffff
    L004509f6:
            test esi, esi
            jge L00450a0f
            mov eax, dword ptr [esp + 0x10]
            mov ecx, dword ptr [eax + 4]
            push ecx
            push 0xaf
            call FatalError
            add esp, 8
    L00450a0f:
            mov edx, dword ptr [ebx + 0xc]
            mov ecx, dword ptr [esp + 0x14]
            and esi, 1
            and edx, 0xfffffffb
            shl esi, 2
            or esi, edx
            lea edx, [esp + 0x18]
            mov dword ptr [ebx + 0xc], esi
            call GetALineAndDontArgue
            xor ebp, ebp
            mov edi, offset gReverseness_type_names
    L00450a34:
            mov esi, dword ptr [edi]
            lea eax, [esp + 0x18]
    L00450a3a:
            mov dl, byte ptr [eax]
            mov cl, dl
            cmp dl, byte ptr [esi]
            jne L00450a5e
            test cl, cl
            je L00450a5a
            mov dl, byte ptr [eax + 1]
            mov cl, dl
            cmp dl, byte ptr [esi + 1]
            jne L00450a5e
            add eax, 2
            add esi, 2
            test cl, cl
            jne L00450a3a
    L00450a5a:
            xor eax, eax
            jmp L00450a63
    L00450a5e:
            sbb eax, eax
            sbb eax, -1
    L00450a63:
            test eax, eax
            je L00450a75
            add edi, 4
            inc ebp
            cmp edi, offset gSelected_drone_path_node_index
            jb L00450a34
            jmp L00450a79
    L00450a75:
            mov esi, ebp
            jmp L00450a7c
    L00450a79:
            or esi, 0xffffffff
    L00450a7c:
            test esi, esi
            jge L00450a95
            mov eax, dword ptr [esp + 0x10]
            mov ecx, dword ptr [eax + 4]
            push ecx
            push 0xaf
            call FatalError
            add esp, 8
    L00450a95:
            mov edx, dword ptr [ebx + 0xc]
            mov ecx, dword ptr [esp + 0x14]
            and esi, 1
            and edx, 0xfffffff7
            shl esi, 3
            or esi, edx
            mov dword ptr [ebx + 0xc], esi
            call GetAScalar
            fmul qword ptr [gDrone_select_max_distance_squared+0x2c]
            fstp dword ptr [ebx + 8]
    L00450ab8:
            mov eax, dword ptr [esp + 0x10]
            lea edx, [esp + 0x18]
            mov eax, dword ptr [eax + 0x5d0]
            mov ecx, dword ptr [eax]
            inc ecx
            mov dword ptr [eax], ecx
            mov ecx, dword ptr [esp + 0x14]
            call GetALineAndDontArgue
            mov esi, offset gDefault_drone_direction+0x770
            lea eax, [esp + 0x18]
    L00450add:
            mov dl, byte ptr [eax]
            mov bl, byte ptr [esi]
            mov cl, dl
            cmp dl, bl
            jne L00450b05
            test cl, cl
            je L00450b01
            mov dl, byte ptr [eax + 1]
            mov bl, byte ptr [esi + 1]
            mov cl, dl
            cmp dl, bl
            jne L00450b05
            add eax, 2
            add esi, 2
            test cl, cl
            jne L00450add
    L00450b01:
            xor eax, eax
            jmp L00450b0a
    L00450b05:
            sbb eax, eax
            sbb eax, -1
    L00450b0a:
            test eax, eax
            jne L00450775
    L00450b12:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x208
            ret
    L00450b1d:
            mov eax, ebp
            jmp L00450808
    }
#endif
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
void C2_NAKED C2_HOOK_FASTCALL LoadPerRaceDroneStuff(void) {
#ifndef CARPOCALYPSE2_MATCHING

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
#else
    __asm {
            mov edx, dword ptr [gCount_drone_forms]
            sub esp, 0x30c
            push ebx
            xor ebx, ebx
            push ebp
            push esi
            cmp edx, ebx
            push edi
            jle L0044fde1
            mov eax, offset gDrone_forms+0x70
            mov ecx, edx
    L0044fdbd:
            mov dword ptr [eax - 4], ebx
            mov dword ptr [eax], 0xffffffff
            mov dword ptr [eax + 4], ebx
            mov dword ptr [eax + 8], 0xffffffff
            mov dword ptr [eax + 0xc], ebx
            mov dword ptr [eax + 0x10], ebx
            mov dword ptr [eax + 0x14], ebx
            add eax, 0x88
            dec ecx
            jne L0044fdbd
    L0044fde1:
            mov eax, dword ptr [gCount_drones]
            mov dword ptr [esp + 0x10], ebx
            cmp eax, ebx
            jle L0044ffe9
            xor ecx, ecx
            mov dword ptr [esp + 0x14], ecx
            jmp L0044fdfe
    L0044fdfa:
            mov ecx, dword ptr [esp + 0x14]
    L0044fdfe:
            mov eax, dword ptr [gDrone_specs]
            mov edi, offset gApplication_path
            push offset gDefault_drone_direction+0x5f0
            mov eax, dword ptr [eax + ecx + 4]
            mov edx, dword ptr [eax + 0x6c]
            inc edx
            mov dword ptr [eax + 0x6c], edx
            mov edx, dword ptr [gDrone_specs]
            xor eax, eax
            lea ebx, [edx + ecx]
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            lea edx, [esp + 0x120]
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            lea edx, [esp + 0x120]
            shr ecx, 2
            rep movsd
            mov ecx, eax
            and ecx, 3
            rep movsb
            lea ecx, [esp + 0x120]
            call PathCat
            mov ecx, dword ptr [ebx + 4]
            lea edx, [esp + 0x11c]
            push ecx
            lea ecx, [esp + 0x120]
            call PathCat
            lea ecx, [esp + 0x11c]
            call OpenPackFileAndSetTiffLoading
            mov ecx, ebx
            mov dword ptr [esp + 0x18], eax
            call LoadDroneActorsModels
            mov edi, offset gApplication_path
            or ecx, 0xffffffff
            xor eax, eax
            lea edx, [esp + 0x1c]
            repne scasb
            not ecx
            sub edi, ecx
            mov ebp, dword ptr [ebx + 4]
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            push offset gDefault_drone_direction+0x5f0
            shr ecx, 2
            rep movsd
            mov ecx, eax
            lea edx, [esp + 0x20]
            and ecx, 3
            rep movsb
            lea ecx, [esp + 0x20]
            call PathCat
            lea edx, [esp + 0x1c]
            lea ecx, [esp + 0x1c]
            push ebp
            call PathCat
            lea edx, [esp + 0x1c]
            lea ecx, [esp + 0x1c]
            push ebp
            call PathCat
            mov edi, offset gDefault_drone_direction+0x5f8
            or ecx, 0xffffffff
            xor eax, eax
            lea edx, [esp + 0x1c]
            repne scasb
            not ecx
            sub edi, ecx
            mov esi, edi
            mov ebp, ecx
            mov edi, edx
            or ecx, 0xffffffff
            repne scasb
            mov ecx, ebp
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, ebp
            and ecx, 3
            mov edx, offset gNext_crush_actor_id+0x180
            rep movsb
            lea ecx, [esp + 0x1c]
            call DRfopen
            mov edi, eax
            test edi, edi
            jne L0044ff31
            lea eax, [esp + 0x1c]
            push eax
            push 0xaf
            call FatalError
            add esp, 8
    L0044ff31:
            mov ecx, edi
            call PFfeof
            test eax, eax
            jne L0044ff92
    L0044ff3c:
            lea edx, [esp + 0x21c]
            mov ecx, edi
            call GetALineAndDontArgue
            mov esi, offset gDefault_drone_direction+0x698
            lea eax, [esp + 0x21c]
    L0044ff56:
            mov dl, byte ptr [eax]
            mov cl, dl
            cmp dl, byte ptr [esi]
            jne L0044ff7a
            test cl, cl
            je L0044ff76
            mov dl, byte ptr [eax + 1]
            mov cl, dl
            cmp dl, byte ptr [esi + 1]
            jne L0044ff7a
            add eax, 2
            add esi, 2
            test cl, cl
            jne L0044ff56
    L0044ff76:
            xor eax, eax
            jmp L0044ff7f
    L0044ff7a:
            sbb eax, eax
            sbb eax, -1
    L0044ff7f:
            test eax, eax
            je L00450108
            mov ecx, edi
            call PFfeof
            test eax, eax
            je L0044ff3c
    L0044ff92:
            mov ecx, dword ptr [ebx + 4]
            push ecx
            push 0xaf
            call FatalError
            add esp, 8
    L0044ffa3:
            mov edx, edi
            mov ecx, ebx
            call LoadFunksAndGrooves
            mov ecx, edi
            call PFfclose
            mov ecx, dword ptr [esp + 0x18]
            call ClosePackFileAndSetTiffLoading
            mov eax, dword ptr [esp + 0x10]
            mov edx, dword ptr [esp + 0x14]
            mov ecx, dword ptr [gCount_drones]
            inc eax
            add edx, 0x5d8
            cmp eax, ecx
            mov dword ptr [esp + 0x10], eax
            mov dword ptr [esp + 0x14], edx
            jl L0044fdfa
            mov edx, dword ptr [gCount_drone_forms]
            xor ebx, ebx
    L0044ffe9:
            cmp edx, ebx
            mov dword ptr [esp + 0x10], ebx
            jle L004500fd
            mov ebp, offset gDrone_forms+0x6c
    L0044fffa:
            mov ecx, dword ptr [ebp]
            cmp ecx, ebx
            je L004500e6
            fld dword ptr [ebp - 0x44]
            fcomp dword ptr [gDrone_render_zero]
            fnstsw ax
            test ah, 0x40
            jne L00450025
            cmp ecx, 0xa
            jle L00450021
            mov eax, 0xa
            jmp L00450027
    L00450021:
            mov eax, ecx
            jmp L00450027
    L00450025:
            xor eax, eax
    L00450027:
            cmp eax, ebx
            mov dword ptr [ebp + 0x10], eax
            je L004500e6
            push 0xbc
            push 4
            push eax
            call BrMemCalloc
            add esp, 0xc
            mov dword ptr [ebp + 0x18], eax
            mov edx, dword ptr [ebp + 0x10]
            push 0xbc
            push 1
            push edx
            call BrMemCalloc
            mov dword ptr [ebp + 0x14], eax
            mov eax, dword ptr [ebp + 0x10]
            add esp, 0xc
            test eax, eax
            jle L004500de
    L00450062:
            mov eax, dword ptr [ebp + 0xc]
            mov ecx, dword ptr [gDroneStorage+0x2c]
            xor edx, edx
            push 0
            mov esi, dword ptr [ecx + eax*4]
            mov dx, word ptr [esi + 0x10]
            mov eax, dword ptr [esi + 4]
            push edx
            push eax
            call BrModelAllocate
            mov ecx, dword ptr [ebp + 0x18]
            add esp, 0xc
            mov dword ptr [ecx + ebx*4], eax
            mov edx, dword ptr [ebp + 0x18]
            mov eax, dword ptr [edx + ebx*4]
            or byte ptr [eax + 0x20], 0x80
            mov eax, dword ptr [ebp + 0x18]
            mov ecx, dword ptr [esi + 0xc]
            mov eax, dword ptr [eax + ebx*4]
            mov dword ptr [eax + 0xc], ecx
            mov dx, word ptr [esi + 0x12]
            xor ecx, ecx
            mov edi, dword ptr [eax + 8]
            mov cx, word ptr [eax + 0x10]
            mov word ptr [eax + 0x12], dx
            mov esi, dword ptr [esi + 8]
            lea ecx, [ecx + ecx*4]
            shl ecx, 3
            mov eax, ecx
            shr ecx, 2
            rep movsd
            mov ecx, eax
            and ecx, 3
            rep movsb
            mov ecx, dword ptr [ebp + 0x18]
            mov edx, dword ptr [ecx + ebx*4]
            push edx
            call BrModelAdd
            mov eax, dword ptr [ebp + 0x10]
            add esp, 4
            inc ebx
            cmp ebx, eax
            jl L00450062
    L004500de:
            mov edx, dword ptr [gCount_drone_forms]
            xor ebx, ebx
    L004500e6:
            mov eax, dword ptr [esp + 0x10]
            add ebp, 0x88
            inc eax
            cmp eax, edx
            mov dword ptr [esp + 0x10], eax
            jl L0044fffa
    L004500fd:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x30c
            ret
    L00450108:
            push offset gDefault_drone_direction+0x66c
            call DoNotDprintf
            mov eax, dword ptr [ebx + 4]
            add esp, 4
            push eax
            push offset gDefault_drone_direction+0x634
            call DoNotDprintf
            lea esi, [ebx + 0x118]
            add esp, 8
            mov edx, edi
            mov ecx, esi
            call ReadMechanicsShapes
            cmp dword ptr [esi], 0
            je L0044ffa3
            mov byte ptr [ebx + 0x46], 1
            jmp L0044ffa3
    }
#endif
}

#pragma inline_depth(0)
// FUNCTION: CARMA2_HW 0x00401070
void C2_HOOK_FASTCALL LoadAIWorldForRace(tRace_info* pRace_info) {

    PrintMemoryDump(0,"BEFORE LoadOpponentsCars()");
    LoadOpponentsCars(pRace_info);
    LoadCopCars(pRace_info);
    PrintMemoryDump(0,"AFTER LoadOpponentsCars(), BEFORE LoadPerRaceDroneStuff()");
    LoadPerRaceDroneStuff();
    PrintMemoryDump(0,"AFTER LoadPerRaceDroneStuff()");
}
#pragma inline_depth()

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
    if (track_bounds.min.v[1] < track_bounds.max.v[1]) {
        gTrack_drone_min_y = track_bounds.min.v[1];
    } else {
        gTrack_drone_min_y = track_bounds.max.v[1];
    }
    PrintMemoryDump(0, "BEFORE InitOpponents()");
    InitOpponents(pRace_info);
    PrintMemoryDump(0, "AFTER InitOpponents(), BEFORE InitDrones()");
    InitDrones(pRace_info);
    PrintMemoryDump(0, "AFTER InitDrones()");
}
