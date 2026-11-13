#include "world.h"

#include "animation.h"
#include "brucetrk.h"
#include "car.h"
#include "crush.h"
#include "depth.h"
#include "drmem.h"
#include "errors.h"
#include "finteray.h"
#include "flicplay.h"
#include "globvars.h"
#include "globvrpb.h"
#include "graphics.h"
#include "input.h"
#include "loading.h"
#include "opponent.h"
#include "pedestrn.h"
#include "piping.h"
#include "powerups.h"
#include "replay.h"
#include "shrapnel.h"
#include "skidmark.h"
#include "smashing.h"
#include "sound.h"
#include "spark.h"
#include "trig.h"
#include "utility.h"

#include "platform.h"

#include <brender/brender.h>
#include "carpocalypse2_macros.h"

#include <tiffio.h>
#include <zlib.h>

#include <ctype.h>
#include "c2_stdlib.h"
#include "c2_string.h"
#include "c2_sys_stat.h"
#include "carpocalypse2_types.h"

#include <assert.h>
#include <c2_sys_stat.h>

#include "c2_math.h"

#define RGB565_R(V) (((V) & 0xf800) >> 11)
#define RGB565_G(V) (((V) & 0x07e0) >> 5)
#define RGB565_B(V) (((V) & 0x001f) >> 0)
#define RGB888_R(V) (((V) >> 16) & 0xff)
#define RGB888_G(V) (((V) >> 8) & 0xff)
#define RGB888_B(V) (((V) >> 0) & 0xff)

#define SAW(T, PERIOD) (fmodf((T), (PERIOD)) / (PERIOD))

#define MOVE_FUNK_PARAMETER(DEST, MODE, PERIOD, TEXTUREBITS, AMPLITUDE, FLASH_VALUE)            \
    do {                                                                                        \
        switch (MODE) { \
        case eMove_continuous:                                                                  \
            if ((PERIOD) == 0.f) { \
                DEST = 0.f;                                                                     \
            } else {                                                                            \
                DEST = (AMPLITUDE)*SAW(f_the_time, (PERIOD));                                   \
            }                                                                                   \
            break;                                                                              \
        case eMove_controlled:                                                                  \
            DEST = (PERIOD) * (AMPLITUDE);                                                      \
            break;                                                                              \
        case eMove_absolute:                                                                    \
            DEST = (PERIOD);                                                                    \
            break;                                                                              \
        case eMove_linear:                                                                      \
            if ((PERIOD) == 0.f) { \
                DEST = 0.f;                                                                     \
            } else {                                                                            \
                DEST = (AMPLITUDE)*MapSawToTriangle(SAW(f_the_time, (PERIOD)));                 \
            }                                                                                   \
            break;                                                                              \
        case eMove_flash:                                                                       \
            if (2 * fmodf(f_the_time, (PERIOD)) > (PERIOD)) { \
                DEST = (FLASH_VALUE);                                                           \
            } else {                                                                            \
                DEST = -(FLASH_VALUE);                                                          \
            }                                                                                   \
            break;                                                                              \
        case eMove_texturebits:                                                                 \
            {                                                                                   \
                int sum_flags = 0;                                                              \
                int ii;                                                                         \
                for (ii = 0; ii < (TEXTUREBITS)->count; ii++) { \
                    if ((TEXTUREBITS)->car->field_0x18cc & (1 << (TEXTUREBITS)->bits[ii])) { \
                        sum_flags |= 1 << ii;                                                   \
                    }                                                                           \
                }                                                                               \
                DEST = (br_scalar)sum_flags;                                                    \
            }                                                                                   \
            break;                                                                              \
        case eMove_harmonic:                                                                    \
        default:                                                                                \
            if ((PERIOD) == 0.f) { \
                DEST = 0.f;                                                                     \
            } else {                                                                            \
                DEST = (AMPLITUDE)*BR_SIN(BrDegreeToAngle(SAW(f_the_time, (PERIOD)) * 360.f));  \
            }                                                                                   \
            break;                                                                              \
        }                                                                                       \
    } while (0)
tAdd_to_storage_result C2_HOOK_FASTCALL AddSoundToStorage(tBrender_storage* pStorage_space, int pSound_id);
br_pixelmap* C2_HOOK_FASTCALL CreatePalettePixelmapFromRGBChannels(br_uint_16* pRed_channel, br_uint_16* pGrn_channel, br_uint_16* pBlu_channel, int pRGB555);
tAdd_to_storage_result C2_HOOK_FASTCALL AddShadeTableToStorage(tBrender_storage* pStorage_space, br_pixelmap* pThe_st);
extern const char* gSoundType_choices[2];

// GLOBAL: CARMA2_HW 0x00591374
extern tCar_texturing_level gCar_texturing_level;
// GLOBAL: CARMA2_HW 0x0059136c
extern tRoad_texturing_level gRoad_texturing_level;
// GLOBAL: CARMA2_HW 0x00591370
extern tWall_texturing_level gWall_texturing_level;
// GLOBAL: CARMA2_HW 0x00591368
extern int gRendering_accessories;
// GLOBAL: CARMA2_HW 0x006aaa20
extern br_pixelmap* gDuplicate_pixelmap;
// GLOBAL: CARMA2_HW 0x006aaa2c
extern int gDisallow_duplicates;
// GLOBAL: CARMA2_HW 0x00660cb8
extern tRendererShadingType gMaterial_shading_for_callback;
// GLOBAL: CARMA2_HW 0x006aaa24
extern br_model* gDuplicate_model;
// GLOBAL: CARMA2_HW 0x006aaa28
extern br_material* gDuplicate_material;
// GLOBAL: CARMA2_HW 0x006a80f8
tDecal gDecals[50];

// GLOBAL: CARMA2_HW 0x006a3940
tQueued_net_smash gQueued_net_smashes[50];

// GLOBAL: CARMA2_HW 0x006a55bc
int gSize_powerup_queue;

// GLOBAL: CARMA2_HW 0x006a6d3c
int gCount_host_smashes;


// GLOBAL: CARMA2_HW 0x006631c0
br_filesystem gZlibBrFileSystem = {
    "Zlib filesystem",
    ZlibFsGetAttributes,
    ZlibFsOpenRead,
    ZlibFsOpenWrite,
    ZlibFsClose,
    ZlibFsEof,
    ZlibFsGetChr,
    ZlibFsPutChr,
    ZlibFsRead,
    ZlibFsWrite,
    ZlibFsGetLine,
    ZlibFsPutLine,
    ZlibFsAdvance,
};

// GLOBAL: CARMA2_HW 0x0068b848
int gGroovidelics_array_size;

// GLOBAL: CARMA2_HW 0x0068b850
tGroovidelic_spec* gGroovidelics_array;


// GLOBAL: CARMA2_HW 0x0065fe88
const char* gSmashable_item_mode_names[6] = {
    "nochange",
    "decal",
    "texturechange",
    "remove",
    "replacemodel",
    "crush",
};

// GLOBAL: CARMA2_HW 0x006b788c
int gCount_smashable_noncars;

// GLOBAL: CARMA2_HW 0x006a55b0
int gCount_smashable_noncar_shrapnel_actors;

// GLOBAL: CARMA2_HW 0x006b7888
tSmashable_environment_name* gSmashable_noncars;

// GLOBAL: CARMA2_HW 0x006a55b8
const char* gSmashable_track_environment_path;


// GLOBAL: CARMA2_HW 0x006a55b4
int gCount_track_smashable_environment_specs;

// GLOBAL: CARMA2_HW 0x006a8288
int gCount_track_smashable_environment_specs_2;

// GLOBAL: CARMA2_HW 0x006a8294
int gINT_006a8294;

// GLOBAL: CARMA2_HW 0x006a3330
int gCapacity_track_smashables;

// GLOBAL: CARMA2_HW 0x006a5138
tSmashable_item_spec* gTrack_smashable_environment_specs;

// GLOBAL: CARMA2_HW 0x006ba4a0
tSmashable_item_spec* gSmashable_track_indexable_triggers[100];


// GLOBAL: CARMA2_HW 0x00660e90
const char* gDepth_effect_names[3] = {
    "dark",
    "fog",
    "colour",
};

// GLOBAL: CARMA2_HW 0x006b6400
char gAdditional_model_path[256];

// GLOBAL: CARMA2_HW 0x006b6500
char gAdditional_actor_path[256];

// GLOBAL: CARMA2_HW 0x006b6620
br_model* gAdditional_models[1000];

// GLOBAL: CARMA2_HW 0x007634b4
int gTrack_version;

// GLOBAL: CARMA2_HW 0x006ab948
int gNumber_of_additional_models;

// GLOBAL: CARMA2_HW 0x006b6600
br_actor* gLast_actor;

// GLOBAL: CARMA2_HW 0x006ab94c
const char* gSelected_actor_name;

// GLOBAL: CARMA2_HW 0x006b75c4
int gCurrent_dropped_actor_index;

// GLOBAL: CARMA2_HW 0x006ab798
br_vector3 gLast_actor_position_cache;

// GLOBAL: CARMA2_HW 0x006ab7a4
br_actor* gLast_actor_position_cache_actor;

// GLOBAL: CARMA2_HW 0x0058b1e8
br_scalar gNegative_one = -1.f;

// GLOBAL: CARMA2_HW 0x0058b1c8
br_scalar gZero_0058b1c8 = 0.f;

// GLOBAL: CARMA2_HW 0x0058b1ec
br_scalar gOne_0058b1ec = 1.f;

// GLOBAL: CARMA2_HW 0x0058b1f0
double gDouble_0058b1f0 = 0.9;

// GLOBAL: CARMA2_HW 0x0058b1e0
double gDouble_0058b1e0 = 0.001;

// GLOBAL: CARMA2_HW 0x0058b1fc
br_scalar gNegative_half = -0.5f;

// GLOBAL: CARMA2_HW 0x006aaf40
tU8* gTrack_flic_buffer;

// GLOBAL: CARMA2_HW 0x006aaf48
tU32 gTrack_flic_buffer_size;

// GLOBAL: CARMA2_HW 0x006aaf50
tFlic_descriptor gTrack_flic_descriptor;


// GLOBAL: CARMA2_HW 0x0079ec2e
br_angle gSky_image_width;

// GLOBAL: CARMA2_HW 0x0079ec2c
br_angle gSky_image_height;

// GLOBAL: CARMA2_HW 0x0079ec30
br_angle gSky_image_underground;

// GLOBAL: CARMA2_HW 0x0074caa8
int gTrack_depth_colour_red;

// GLOBAL: CARMA2_HW 0x0074cf2c
int gTrack_depth_colour_green;

// GLOBAL: CARMA2_HW 0x0074cad0
int gTrack_depth_colour_blue;

// GLOBAL: CARMA2_HW 0x0079ef40
tU8 gNon_car_spec_indices[100];

// GLOBAL: CARMA2_HW 0x0079ed30
tU8 gNon_car_indices[88];

// gGroove_by_proxy_actor follows the repair animation array in the original,
// and the array loop's end compare resolves to it.
// GLOBAL: CARMA2_HW 0x006a7fc8
tRepair_animation gRepair_animations[25] = { 0 };
// GLOBAL: CARMA2_HW 0x0068b854
br_actor* gGroove_by_proxy_actor = NULL;

// GLOBAL: CARMA2_HW 0x00660258
const char* gSpecial_effects_boundary_choices[4] = {
    "BOX",
    "PLANE",
    "DEFAULT",
    "NEW",
};

// GLOBAL: CARMA2_HW 0x0068b878
int gDefault_engine_noise_index;

// GLOBAL: CARMA2_HW 0x00595c38
const char* gSoundGeneratorTypeNames[3] = {
    "NONCAR",
    "ACTOR",
    "POINT",
};

// GLOBAL: CARMA2_HW 0x0068b830
tFunk_temp_buffer* gFunk_temp_vertices;

// GLOBAL: CARMA2_HW 0x0068b84c
tFunkotronic_spec* gFunkotronics_array;

// GLOBAL: CARMA2_HW 0x0068b844
int gFunkotronics_array_size;

// GLOBAL: CARMA2_HW 0x00688b30
tGroove_funk_binding gGroove_funk_bindings[1440];

// GLOBAL: CARMA2_HW 0x00655ba0
const char* gFunk_nature_names[4] = {
    "constant",
    "distance",
    "lastlap",
    "otherlaps",
};

// GLOBAL: CARMA2_HW 0x00655b80
const char* gFunk_move_names[7] = {
    "linear",
    "harmonic",
    "flash",
    "controlled",
    "absolute",
    "continuous",
    "texturebits",
};

// GLOBAL: CARMA2_HW 0x00655b68
const char* gFunk_anim_names[4] = {
    "frames",
    "flic",
    "camera",
    "mirror",
};

// GLOBAL: CARMA2_HW 0x00655b78
const char* gTime_mode_names[2] = {
    "approximate",
    "accurate",
};

// GLOBAL: CARMA2_HW 0x00655bf0
const char* gCamera_animation_names[2] = {
    "static",
    "tracking",
};

// GLOBAL: CARMA2_HW 0x00655bc8
const char* gLollipop_names[3] = {
    "xlollipop",
    "ylollipop",
    "zlollipop",
};

// GLOBAL: CARMA2_HW 0x00655bb0
const char* gGroove_nature_names[2] = {
    "constant",
    "distance",
};

// GLOBAL: CARMA2_HW 0x00655bd8
const char* gGroove_path_names[2] = {
    "straight",
    "circular",
};

// GLOBAL: CARMA2_HW 0x00655be0
const char* gGroove_object_names[4] = {
    "spin",
    "rock",
    "throb",
    "shear",
};

// GLOBAL: CARMA2_HW 0x006a55c0
tStored_smash* gNet_host_smashes;

// GLOBAL: CARMA2_HW 0x006ab7a8
int gDelete_count;

// GLOBAL: CARMA2_HW 0x006aafc8
br_actor* gDelete_list[500];

// GLOBAL: CARMA2_HW 0x006ab940
int gSpec_vol_mode;

// GLOBAL: CARMA2_HW 0x006ab7b0
br_actor* gSpec_vol_actors[100];

// GLOBAL: CARMA2_HW 0x0068b838
tU32 gPrevious_groove_times[2];

#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_CDECL BrActorFree();
extern br_actor* C2_HOOK_CDECL BrActorRemove();
extern br_pixelmap* C2_HOOK_CDECL BrMapRemove();
extern void* C2_HOOK_CDECL BrMemAllocate();
extern void C2_HOOK_CDECL BrMemFree();
extern void C2_HOOK_CDECL BrModelFree();
extern br_model* C2_HOOK_CDECL BrModelRemove();
extern void C2_HOOK_CDECL BrPixelmapFree();
extern void C2_HOOK_FASTCALL ClearOutStorageSpace(tBrender_storage* pStorage_space);
extern intptr_t C2_HOOK_FASTCALL DRActorEnumRecurse(br_actor* pActor, br_actor_enum_cbfn* callback, void* arg);
extern void C2_HOOK_FASTCALL DisposeColumns(tTrack_spec* pTrack_spec);
extern void C2_HOOK_FASTCALL DisposeFunkotronics(int pOwner);
extern void C2_HOOK_FASTCALL DisposeGroovidelics(int pOwner);
extern void C2_HOOK_FASTCALL DisposeKevStuff(void);
extern void C2_HOOK_FASTCALL DisposeOpponentPaths(void);
extern void C2_HOOK_FASTCALL DisposePhysicsObject(tPhysics_object* pObject);
extern void C2_HOOK_FASTCALL DisposeSmashableEnvironment(void);
extern void C2_HOOK_FASTCALL DisposeTexturingMaterials(void);
extern int C2_HOOK_FASTCALL EndFlic(tFlic_descriptor_ptr pFlic_info);
extern void C2_HOOK_FASTCALL FreeSmokeShadeTables(void);
extern tNet_message* C2_HOOK_FASTCALL NetBuildMessage(undefined pArg1, undefined4 pArg2);
extern int C2_HOOK_FASTCALL NetSendMessageToPlayer(tNet_game_details* pNet_game, tNet_message* pMessage, tPlayer_ID pPlayer_id);
extern void C2_HOOK_FASTCALL PossibleService(void);
extern int gCount_queued_smashes;
extern tNet_game_details* gCurrent_net_game;
extern tNet_mode gNet_mode;
extern tPowerup_queue_item gPowerup_queue[50];
extern tProgram_state gProgram_state;
extern tSmash_vertex gSmash_glass_fragments[200];
extern br_actor* gTrack_actor;
extern tBrender_storage gTrack_storage_space;
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern int C2_HOOK_FASTCALL AddMaterials(tBrender_storage* pStorage_space, const char* pPath, tRendererShadingType pShading);
extern br_actor* C2_HOOK_CDECL BrActorAdd();
extern br_uint_32 C2_HOOK_CDECL BrActorEnum();
extern br_uint_32 C2_HOOK_CDECL BrActorSave();
extern br_uint_16 C2_HOOK_CDECL BrActorToActorMatrix34();
extern br_pixelmap* C2_HOOK_CDECL BrMapAdd();
extern br_material* C2_HOOK_CDECL BrMaterialFind();
extern void C2_HOOK_CDECL BrMatrix34ApplyP();
extern void C2_HOOK_CDECL BrMatrix34Copy();
extern void C2_HOOK_CDECL BrMatrix34Identity();
extern void C2_HOOK_CDECL BrMatrix34Pre();
extern void C2_HOOK_CDECL BrMatrix34PreRotateX();
extern void C2_HOOK_CDECL BrMatrix34PreRotateY();
extern void C2_HOOK_CDECL BrMatrix34PreRotateZ();
extern br_model* C2_HOOK_CDECL BrModelAdd();
extern br_model* C2_HOOK_CDECL BrModelAllocate();
extern br_model* C2_HOOK_CDECL BrModelFind();
extern br_uint_32 C2_HOOK_CDECL BrModelSaveMany();
extern void C2_HOOK_CDECL BrModelUpdate();
extern void C2_HOOK_CDECL BrResFree();
extern char* C2_HOOK_CDECL BrResStrDup();
extern void C2_HOOK_CDECL BrTransformToTransform();
extern void C2_HOOK_CDECL BrVector3SetFloat();
extern br_uint_32 C2_HOOK_FASTCALL DRActorEnumRecurseWithTrans(br_actor* pActor, br_matrix34* pMatrix, recurse_with_trans_cbfn* pCall_back, void* pArg);
extern int C2_HOOK_FASTCALL DRStricmp(const char* p1, const char* p2);
extern FILE* C2_HOOK_FASTCALL DRfopen(const char* pFilename, const char* pMode);
extern br_material* C2_HOOK_FASTCALL DisposeSuffixedMaterials(br_model* pModel, tU16 pFace);
extern br_scalar C2_HOOK_FASTCALL DistanceFromFace(const br_vector3* pP, const tFace_ref* pFace);
extern float C2_HOOK_FASTCALL DistanceFromFaceND(const br_vector3* pP, const br_vector3* pN, br_scalar pF);
extern void C2_HOOK_FASTCALL DoEnvSound(tSpecial_volume* pVolume, br_vector3* pP, tSpecial_volume_soundfx_type pType, tSpecial_volume_soundfx_data* pSound_data, float pSound_dist, br_vector3 *pSound_dir);
extern void C2_HOOK_FASTCALL DropActor(int pIndex, int pPowerup);
extern void C2_HOOK_FASTCALL FUN_0040e530(br_actor* pActor);
extern void C2_NORETURN C2_HOOK_CDECL FatalError();
extern int C2_HOOK_FASTCALL FindFacesInBox(tBounds* bnds, tFace_ref* face_list, int max_face, tWorld_callbacks* pWorld_callbacks);
extern void C2_HOOK_FASTCALL FlushAllPedCaches(void);
extern char * C2_HOOK_FASTCALL GetALineAndDontArgue(FILE* pF, char* pS);
extern int C2_HOOK_FASTCALL GetALineAndInterpretCommand(FILE* pF, const char** pString_list, int pCount);
extern br_scalar C2_HOOK_FASTCALL GetAScalar(FILE* pF);
extern void C2_HOOK_FASTCALL GetAString(FILE* pF, char* pString);
extern int C2_HOOK_FASTCALL GetAnInt(FILE* pF);
extern void C2_HOOK_FASTCALL GetPairOfInts(FILE* pF, int* pF1, int* pF2);
extern char* C2_HOOK_FASTCALL GetPowerupMessage(int pIndex);
extern int C2_HOOK_FASTCALL HasThisSuffix(const char* pIdent, const char* pSuffix);
extern br_actor* C2_HOOK_FASTCALL LoadActor(const char* pName);
extern tAdd_to_storage_result C2_HOOK_FASTCALL LoadSingleSound(tBrender_storage* pStorage_space, int pSound_id);
extern void C2_HOOK_FASTCALL LoadSmashableLevels(FILE* pF, tSmashable_level** pSmashable_levels, int* pCount_smashable_levels, int pIs_texture_change, tBrender_storage* pBrender_storage);
extern int C2_HOOK_FASTCALL NewTextHeadupSlot(int pSlot_index, int pFlash_rate, int pLifetime, int pFont_index, const char* pText);
extern void C2_NORETURN C2_HOOK_FASTCALL PDFatalError(const char* pThe_str);
extern int C2_HOOK_FASTCALL PDKeyDown(int pKey_index);
extern int C2_HOOK_FASTCALL PFfclose(FILE* pFile);
extern void C2_HOOK_FASTCALL PathCat(char* pDestn_str, const char* pStr_1, const char* pStr_2);
extern intptr_t C2_HOOK_CDECL ProcessFaceMaterials();
extern void C2_HOOK_FASTCALL ReadExplosion(FILE *pF, tExplosion_animation* pAnimation);
extern void C2_HOOK_FASTCALL ReadNonCarCuboidActivation(FILE* pF, tNon_car_cuboid_activations* pNon_car_cuboid_activations);
extern void C2_HOOK_FASTCALL ReadShrapnelSideEffects(FILE* pF, tShrapnel_side_effects* pShrapnel_side_effects);
extern void C2_HOOK_FASTCALL ReadShrapnelSpec(FILE* pF, tShrapnel_spec* pShrapnel_spec, int* pShrapnel_count);
extern void C2_HOOK_FASTCALL ReadSpillData(FILE *pF,tSlick_spec *pSlick_spec);
extern intptr_t C2_HOOK_CDECL RemoveAdditionalActorModel();
extern void C2_HOOK_FASTCALL RemoveNonCarFromActiveList(br_actor* pActor);
extern br_material* C2_HOOK_FASTCALL RoadUntexToPersp(br_model* pModel, tU16 pFace);
extern void C2_HOOK_FASTCALL SaveAdditionalActors(void);
extern void C2_HOOK_FASTCALL SaveSpecialVolumes(void);
extern void C2_HOOK_FASTCALL SetChangingPowerup(br_actor* pActor);
extern void C2_HOOK_FASTCALL SetSpinningPowerup(br_actor* pActor, int pOpacity);
extern void C2_HOOK_FASTCALL ToggleFlying(void);
extern br_material* C2_HOOK_FASTCALL UnsuffixedMaterial(const char* pOld_ident, const char* pSuffix);
extern br_material* C2_HOOK_FASTCALL WallLinearToPersp(br_model* pModel, tU16 pFace);
extern br_material* C2_HOOK_FASTCALL WallUntexToPersp(br_model* pModel, tU16 pFace);
extern void __cdecl _chkstk();
extern char _CIfmod[];
extern tNon_car_spec* gActive_non_car_list[99];
extern tPath_name gApplication_path;
extern int gCancel_recover;
extern int gCar_flying;
extern tCar_spec* gCar_to_view;
extern char gConst_kangeroo_force_factor[];
extern char gDefault_drone_direction[];
extern char gDouble_0058a068[];
extern char gFLOAT_0058b130[];
extern tFrontend_spec gFrontend_QUIT;
extern float gMass_mine;
extern char gNet_avail_names[];
extern char gNext_crush_actor_id[];
extern br_actor* gNon_track_actor;
extern int gNum_active_non_cars;
extern br_vector3* gOur_pos;
extern char gPed_flags_0058f2e4[];
extern int gRender_alternative_track_actors;
extern const char* gRepeatability_names[4];
extern char gSplit_triangle_vertex_orders[];
extern br_actor* gUniverse_actor;
extern char gWall_texturing_tweakers[];
#endif
// FUNCTION: CARMA2_HW 0x004488f0
tWall_texturing_level C2_HOOK_FASTCALL GetWallTexturingLevel(void) {
    return gWall_texturing_level;
}

// FUNCTION: CARMA2_HW 0x00486c00
int C2_HOOK_FASTCALL IsValidFile(const char* path) {
    struct stat s;

#ifndef _WIN32
    /* Linux is case-sensitive; the retail game's data is not. */
    {
        extern const char* carpocalypse2_fix_path_case(const char* pPath);
        path = carpocalypse2_fix_path_case(path);
    }
#endif
    return stat(path, &s) == 0;
}

// FUNCTION: CARMA2_HW 0x00486be0
int C2_HOOK_FASTCALL GetLastModificationTime(const char* path) {
    struct stat s;
    int res;

#ifndef _WIN32
    /* Linux is case-sensitive; the retail game's data is not. */
    {
        extern const char* carpocalypse2_fix_path_case(const char* pPath);
        path = carpocalypse2_fix_path_case(path);
    }
#endif
    res = stat(path, &s);
    if (res == -1) {
        return 0;
    }
    return (int)s.st_mtime;
}

br_pixelmap* C2_HOOK_FASTCALL Read_DEFAULT_ACT(const char* textureDir, int flags, int *errorCode) {
    int i;
    tPath_name path;
    br_uint_8 defaultActBuffer[0x300];
    FILE* f;
    br_pixelmap* pm;
    br_uint_8* src;
    br_uint_8* dst;

    sprintf(path, "%s%s%s%s%s", textureDir, gDir_separator, "PALETTE", gDir_separator, "DEFAULT.ACT");
    f = DRfopen(path, "rb");
    if (f == NULL) {
        *errorCode = 5;
        return NULL;
    }
    if (fread(defaultActBuffer, CARPOCALYPSE2_ASIZE(defaultActBuffer), 1, (FILE*)f) == 0) {
        PFfclose(f);
        *errorCode = 5;
        return NULL;
    }
    PFfclose(f);
    src = defaultActBuffer;
    if ((flags & kLoadTextureFlags_PalatteRGB555) != 0) {
        pm = BrPixelmapAllocate(BR_PMT_RGB_565, 1, 0x100, NULL, 0);
        if (pm == NULL) {
            *errorCode = 2;
            return NULL;
        }
        dst = pm->pixels;
        for (i = 0; i < 0x100; i++) {
            *(br_uint_32*)dst = ((src[0] & 0xf8) << 8) | ((src[1] & 0xfc) << 3) | (src[2] >> 3);
            src += 3;
            dst += 4;
        }
    } else {
        pm = BrPixelmapAllocate(BR_PMT_RGBX_888, 1, 0x100, NULL, 0);
        if (pm == NULL) {
            *errorCode = 2;
            return NULL;
        }
        dst = pm->pixels;
        dst[0] = 0;
        dst[1] = 0;
        dst[2] = 0;
        src += 3;
        dst += 4;
        for (i = 0; i < 0xff; i++) {
            dst[0] = src[2];
            dst[1] = src[1];
            dst[2] = src[0];
            if (dst[0] == 0 && dst[1] == 0 && dst[2] == 0) {
                dst[0] = 1;
                dst[1] = 1;
                dst[2] = 1;
            }
            src += 3;
            dst += 4;
        }
    }
    pm->identifier = BrResStrDup(pm, "DEFAULT.ACT");
    if (pm->identifier == NULL) {
        BrPixelmapFree(pm);
        *errorCode = 2;
        return NULL;
    }
    return pm;
}

br_uint_8 C2_HOOK_FASTCALL FindBestMatch_ShadeTable(br_colour rgb, br_pixelmap *shadeTable) {
    int i;
    int ref_r, ref_g, ref_b;
    int dr, dg, db;
    br_uint_32 c;
    int min_index;
    int min_error;
    int error;

    ref_r = RGB888_R(rgb);
    ref_g = RGB888_G(rgb);
    ref_b = RGB888_B(rgb);
    if (shadeTable->type == BR_PMT_RGB_555) {
        c = ((br_uint_16*)shadeTable->pixels)[1];
        dr = (RGB565_R(c) << 3) - ref_r;
        dg = (RGB565_G(c) << 2) - ref_g;
        db = (RGB565_B(c) << 3) - ref_b;
        min_index = 1;
        min_error = BR_SQR3(dr, dg, db);
        for (i = 2; i < 256; i++) {
            c = ((br_uint_16*)shadeTable->pixels)[i];
            dr = (RGB565_R(c) << 3) - ref_r;
            dg = (RGB565_G(c) << 2) - ref_g;
            db = (RGB565_B(c) << 3) - ref_b;
            error = BR_SQR3(dr, dg, db);
            if (error < min_error) {
                min_error = error;
                min_index = i;
            }
        }
    } else {
        c = ((br_uint_32 *) shadeTable->pixels)[1];
        dr = RGB888_R(c) - ref_r;
        dg = RGB888_G(c) - ref_g;
        db = RGB888_B(c) - ref_b;
        min_index = 1;
        min_error = BR_SQR3(dr, dg, db);
        for (i = 2; i < 256; i++) {
            c = ((br_uint_32 *) shadeTable->pixels)[i];
            dr = RGB888_R(c) - ref_r;
            dg = RGB888_G(c) - ref_g;
            db = RGB888_B(c) - ref_b;
            error = BR_SQR3(dr, dg, db);
            if (error < min_error) {
                min_error = error;
                min_index = i;
            }
        }
    }
    return min_index;
}

br_pixelmap* C2_HOOK_FASTCALL LoadTiffTexture_MappedToShadeTable(const char* path, br_pixelmap* shadeTable, int* errorCode) {
    TIFF* tif;
    br_pixelmap* pm;
    br_uint_32 height;
    br_uint_32 width;
    br_uint_16 samples_per_pixel;
    br_uint_8* scanlineBuffer;
    br_uint_8* curSrcPixel;
    br_uint_32 x;
    br_uint_32 y;

    tif = TIFFOpen(path, "r");
    if (tif == NULL) {
        *errorCode = 4;
        return NULL;
    }
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
    TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &samples_per_pixel);
    pm = BrPixelmapAllocate(BR_PMT_INDEX_8, width, height, NULL, 0);
    if (pm == NULL) {
        TIFFClose(tif);
        *errorCode = 2;
        return NULL;
    }
    scanlineBuffer = malloc(TIFFScanlineSize(tif));
    if (scanlineBuffer == NULL) {
        TIFFClose(tif);
        BrPixelmapFree(pm);
        *errorCode = 2;
        return NULL;
    }
    for (y = 0; y < height; y++) {
        if (TIFFReadScanline(tif, scanlineBuffer, y, 0) < 1) {
            break;
        }
        curSrcPixel = scanlineBuffer;
        if (samples_per_pixel == 3) {
            for (x = 0; x < width; x++) {
                *(((br_uint_8*)pm->pixels) + y * pm->row_bytes + x) = FindBestMatch_ShadeTable(BR_COLOUR_RGB(curSrcPixel[0], curSrcPixel[1], curSrcPixel[2]), shadeTable);
                curSrcPixel += 3;
            }
        } else {
            for (x = 0; x < width; x++) {
                if (curSrcPixel[3] < 0x80) {
                    *(((br_uint_8 *) pm->pixels) + y * pm->row_bytes + x) = FindBestMatch_ShadeTable(BR_COLOUR_RGB(curSrcPixel[0], curSrcPixel[1], curSrcPixel[2]), shadeTable);
                } else {
                    *(((br_uint_8 *) pm->pixels) + y * pm->row_bytes + x) = 0;
                }
                curSrcPixel += 4;
            }
        }
    }
    free(scanlineBuffer);
    TIFFClose(tif);
    if (y < height) {
        BrPixelmapFree(pm);
        *errorCode = 2;
        return NULL;
    }
    return pm;
}

br_pixelmap* C2_HOOK_FASTCALL LoadTiffTexture_WithShadeTable(const char *path, int flags, int *errorCode) {
    TIFF* tif;
    br_pixelmap* pm;
    br_uint_32 height;
    br_uint_32 width;
    br_uint_16 samples_per_pixel;
    br_uint_8* scanlineBuffer;
    br_uint_8* curSrcPos;
    br_uint_32 x;
    br_uint_32 y;
    br_uint_16* colorMap_Red;
    br_uint_16* colorMap_Green;
    br_uint_16* colorMap_Blue;

    tif = TIFFOpen(path, "r");
    if (tif == NULL) {
        *errorCode = 4;
        return NULL;
    }
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
    TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &samples_per_pixel);
    pm = BrPixelmapAllocate(BR_PMT_INDEX_8, width, height, NULL, 0);
    if (pm == NULL) {
        TIFFClose(tif);
        *errorCode = 2;
        return NULL;
    }
    if (samples_per_pixel == 1) {
        for (y = 0; y < height; y++) {
            TIFFReadScanline(tif, (br_uint_8*)pm->pixels + y * pm->row_bytes, y, 0);
        }
    } else {
        scanlineBuffer = malloc(TIFFScanlineSize(tif));
        if (scanlineBuffer == NULL) {
            TIFFClose(tif);
            BrPixelmapFree(pm);
            *errorCode = 2;
            return NULL;
        }
        for (y = 0; y < height; y++) {
            if (TIFFReadScanline(tif, scanlineBuffer, y, 0) < 1) {
                break;
            }
            curSrcPos = scanlineBuffer;
            for (x = 0; x < width; x++) {
                if (curSrcPos[1] < 0x80) {
                    *(((br_uint_8 *) pm->pixels) + y * pm->row_bytes + x) = 0;
                } else {
                    *(((br_uint_8 *) pm->pixels) + y * pm->row_bytes + x) = curSrcPos[0];
                }
                curSrcPos += samples_per_pixel;
            }
        }
        free(scanlineBuffer);
    }
    if (y < height) {
        TIFFClose(tif);
        BrPixelmapFree(pm);
        *errorCode = 4;
        return NULL;
    }
    if ((flags & kLoadTextureFlags_KeepShadeTable) != 0) {
        TIFFGetField(tif, TIFFTAG_COLORMAP, &colorMap_Red, &colorMap_Green, &colorMap_Blue);
        pm->map = CreatePalettePixelmapFromRGBChannels(colorMap_Red, colorMap_Green, colorMap_Blue, flags & kLoadTextureFlags_PalatteRGB555);
        if (pm->map == NULL) {
            TIFFClose(tif);
            BrPixelmapFree(pm);
            *errorCode = 2;
            return NULL;
        }
        BrResAdd(pm, pm->map);
    }
    TIFFClose(tif);
    return pm;
}

br_pixelmap* C2_HOOK_FASTCALL LoadTiffTexture_16BitRGB(const char *path, int flags, int *errorCode) {
    TIFF* tif;
    br_pixelmap* pm;
    br_uint_32 height;
    br_uint_32 width;
    br_uint_16 samples_per_pixel;
    br_uint_8* scanlineBuffer;
    br_uint_8* curSrcPos;
    br_uint_32 x;
    br_uint_32 y;

    tif = TIFFOpen(path, "r");
    if (tif == NULL) {
        *errorCode = 4;
        return NULL;
    }
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
    TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &samples_per_pixel);
    pm = BrPixelmapAllocate((samples_per_pixel == 3) ? BR_PMT_RGB_565 : ((flags & kLoadTextureFlags_UseARGB1555) ? BR_PMT_ARGB_1555 : BR_PMT_RGBA_4444), width, height, NULL, 0);
    if (pm == NULL) {
        TIFFClose(tif);
        *errorCode = 2;
        return NULL;
    }
    scanlineBuffer = malloc(TIFFScanlineSize(tif));
    if (scanlineBuffer == NULL) {
        TIFFClose(tif);
        BrPixelmapFree(pm);
        *errorCode = 2;
        return NULL;
    }

    for (y = 0; y < height; y++) {
        if (TIFFReadScanline(tif, scanlineBuffer, y, 0) < 1) {
            break;
        }
        curSrcPos = scanlineBuffer;
        if (samples_per_pixel == 3) {
            // RGB888 -> RGB565
            for (x = 0; x < width; x++) {
                *((br_uint_16*)((br_uint_8*)pm->pixels + y * pm->row_bytes + sizeof(br_uint_16) * x)) = ((curSrcPos[0] >> 3) << 11) | ((curSrcPos[1] >> 2) << 5) | (curSrcPos[2] >> 3);
                curSrcPos += 3;
            }
        }
        else if (flags & kLoadTextureFlags_UseARGB1555) {
            // RGBA8888 -> ARGB1555?
            for (x = 0; x < width; x++) {
                *((br_uint_16*)((br_uint_8*)pm->pixels + y * pm->row_bytes + sizeof(br_uint_16) * x)) = ((curSrcPos[3] >> 7) << 15) | ((curSrcPos[0] >> 3) << 10) | ((curSrcPos[1] >> 3) << 5) | (curSrcPos[2] >> 3);
                curSrcPos += 4;
            }
        } else {
            // RGBA8888 -> ARGB4444?
            for (x = 0; x < width; x++) {
                *((br_uint_16*)((br_uint_8*)pm->pixels + y * pm->row_bytes + sizeof(br_uint_16) * x)) = ((curSrcPos[3] >> 4) << 12) | ((curSrcPos[0] >> 4) << 8) | ((curSrcPos[1] >> 4) << 4) | (curSrcPos[2] >> 4);
                curSrcPos += 4;
            }
        }
    }
    free(scanlineBuffer);
    TIFFClose(tif);
    if (y < height) {
        BrPixelmapFree(pm);
        *errorCode = 4;
        return NULL;
    }
    return pm;
}

void C2_HOOK_FASTCALL TwatPIX16(const char* pPath) {

}

// FUNCTION: CARMA2_HW 0x00486860
int C2_HOOK_FASTCALL LoadTiffTextureCB(const char* filePath, tLoadDirectoryStructureCBData* data) {
    int filePathLength;
    tLoadDirectoryStructureCBResult* itemResult;
    br_pixelmap* texture;
    int* errorCode;
    br_pixelmap* palette;
    int loadFlags;
    const char* directory;
    tPath_name textureName;

    filePathLength = strlen(filePath);
    filePathLength -= 4;
    if (filePathLength <= 0) {
        goto return_0;
    }
    if (DRstrcmpi(filePath + filePathLength, ".TIF") != 0) {
        goto return_0;
    }
    sprintf(textureName, "%.*s", filePathLength, filePath);
    if (!data->isTiffx && data->results != NULL) {
        for (itemResult = data->results; itemResult != NULL; itemResult = itemResult->next) {
            if (DRstrcmpi(textureName, itemResult->name) == 0) {
                break;
            }
        }
        if (itemResult != NULL) {
            goto check_tiffx;
        }
    }
    loadFlags = data->loadFlags;
    palette = data->pPalette;
    directory = data->directory;
    errorCode = data->errorCode;
    if ((loadFlags & kLoadTextureFlags_16bbp) == kLoadTextureFlags_16bbp || (texture = LoadTiffTexture_Ex2(directory, textureName, palette, loadFlags, errorCode, 1)) == NULL) {
        texture = LoadTiffTexture_Ex2(directory, textureName, palette, loadFlags, errorCode, 0);
    }
    if (texture != NULL) {
        texture->identifier = BrResStrDup(texture, textureName);
        if (texture->identifier == NULL) {
            BrPixelmapFree(texture);
            *errorCode = 2;
            texture = NULL;
        }
    }
    if (texture != NULL) {
        BrPixelmapFree(texture);
    }
    if (*data->errorCode != 0) {
        return *data->errorCode;
    }
check_tiffx:
    if (data->isTiffx) {
        itemResult = BrMemAllocate(sizeof(*itemResult), BR_MEMORY_APPLICATION);
        if (itemResult == NULL) {
            goto error;
        }
        itemResult->name = BrMemStrDup(textureName);
        if (itemResult->name != NULL) {
            itemResult->next = data->results;
            data->results = itemResult;
            return 0;
        }
    } else {
        goto return_0;
    }
error:
    *data->errorCode = 2;
    return 2;
return_0:
    return 0;
}

// FUNCTION: CARMA2_HW 0x00486760
void C2_HOOK_FASTCALL LoadAllTiffTexturesInDirectory(const char* directory, br_pixelmap* pPalette, int loadFlags, int* errorCode) {
    tLoadDirectoryStructureCBData data;
    tLoadDirectoryStructureCBResult* nextItem;
    tPath_name pathBuffer;

    data.loadFlags = loadFlags | kLoadTextureFlags_SaveBrenderTexture;
    data.errorCode = errorCode;
    data.results = NULL;
    data.directory = directory;
    data.pPalette = pPalette;
    if (!(loadFlags & kLoadTextureFlags_16bbp)) {
        sprintf(pathBuffer, "%s%s%s", directory, gDir_separator, "TIFFX");
        data.isTiffx = 0x1;
        PDEnumPath(pathBuffer, (tEnumPathCallback)LoadTiffTextureCB, &data);
    }
    if (*errorCode != 0) {
        return;
    }
    sprintf(pathBuffer, "%s%s%s", directory, gDir_separator, "TIFFRGB");
    data.isTiffx = 0x0;
    PDEnumPath(pathBuffer, (tEnumPathCallback)LoadTiffTextureCB, &data);

    while (data.results != NULL) {
        nextItem = data.results->next;
        if (data.results->name != NULL) {
            BrMemFree(data.results->name);
        }
        BrMemFree(data.results);
        data.results = nextItem;
    }
}

void UseNativeDirSeparator(char* nativePath, const char* path) {
    size_t i;
    size_t len;
    char c;

    len = strlen(path);
    for (i = 0; i < len; i++) {
        c = path[i];
        if (c == '\\') {
            c = *gDir_separator;
        }
        nativePath[i] = c;
    }
    nativePath[len] = '\0';
}

int C2_HOOK_FASTCALL ResolveTexturePathLink(char* realPath, const char* path) {
    FILE* f;
    tPath_name linkPath;
    tPath_name nativeLinkPath;

    f = DRfopen(path, "rt");
    if (f == NULL) {
        return 0;
    }
    PFfclose(f);
    GetALineAndDontArgue(f, linkPath);
    if (strstr(linkPath, ".TIF") != NULL) {
        UseNativeDirSeparator(nativeLinkPath, linkPath);
    } else if (strstr(linkPath, ".PIX") != NULL) {
        UseNativeDirSeparator(nativeLinkPath, linkPath);
    } else {
        return 0;
    }
    PathCat(linkPath, gApplication_path, nativeLinkPath);
    strcpy(realPath, linkPath);
    return 1;
}

int C2_HOOK_FASTCALL CreatePathLink(const char* targetPath, const char* linkPath) {
    tPath_name buffer;
    tPath_name cleanedTargetRelPath;
    const char* targetRelPath;
    char c;
    int pos;
    FILE *f;

    strcpy(buffer, targetPath);
    targetRelPath = strstr(buffer, "DATA") + strlen("DATA") + 1;
    pos = 0;
    while ((c = *targetRelPath) != '\0') {
        if (c == ':' || c == '\\') {
            c = '\\';
        }
        cleanedTargetRelPath[pos] = c;
        targetRelPath++;
        pos++;
    }
    cleanedTargetRelPath[pos] = '\0';
    f = (FILE*)DRfopen(linkPath, "wt");
    if (f == NULL) {
        return 0;
    }
    fputs(cleanedTargetRelPath, f);
    fclose(f);
    return 1;

}

int C2_HOOK_FASTCALL FindLastOccurrenceOfString_CaseInsensitive(int* offset, const char* haystack, size_t haystackLen, const char* needle) {
    size_t needleLen;
    size_t haystackPos;

    needleLen = strlen(needle);
    if (haystackLen < needleLen) {
        return 0;
    }
    haystackPos = haystackLen - needleLen;
    while (1) {
        if (DRmemicmp(&haystack[haystackPos], needle, needleLen) == 0) {
            *offset = haystackPos;
            return 1;
        }
        if (haystackPos == 0) {
            return 0;
        }
        haystackPos--;
    }
}

// FUNCTION: CARMA2_HW 0x00501fe0
void C2_NAKED C2_HOOK_FASTCALL LoadSomeMaterials(tBrender_storage *pStorage, FILE* pFile, tRendererShadingType pShading) {
#ifndef CARPOCALYPSE2_MATCHING
    char s1[256];
    char s2[256];

    PossibleService();
    GetALineAndDontArgue(pFile, s1);
    PathCat(s2, gApplication_path, "MATERIAL");
    PathCat(s2, s2, strtok(s1, "\t ,/"));
    AddMaterials(pStorage, s2, pShading);
#else
    __asm {
            sub esp, 0x200
            push esi
            push edi
            mov esi, edx
            mov edi, ecx
            call PossibleService
            lea edx, [esp + 0x108]
            mov ecx, esi
            call GetALineAndDontArgue
            lea eax, [esp + 0x108]
            push offset gPed_flags_0058f2e4+0x138
            push eax
            call strtok
            add esp, 8
            mov edx, offset gApplication_path
            lea ecx, [esp + 8]
            mov esi, eax
            push offset gNet_avail_names+0x4b4
            call PathCat
            lea edx, [esp + 8]
            lea ecx, [esp + 8]
            push esi
            call PathCat
            lea edx, [esp + 8]
            mov ecx, dword ptr [esp + 0x20c]
            push ecx
            mov ecx, edi
            call AddMaterials
            pop edi
            pop esi
            add esp, 0x200
            ret 4
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00502d80
C2_NAKED br_pixelmap* C2_HOOK_FASTCALL SwapPixelmapInStorage(br_pixelmap* pNew, br_pixelmap* pOriginal, tBrender_storage* pStorage) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;

    for (i = 0; i < pStorage->pixelmaps_count; i++) {
        if (pStorage->pixelmaps[i] == pOriginal) {
            BrMapRemove(pOriginal);
            BrPixelmapFree(pOriginal);
            pStorage->pixelmaps[i] = pNew;
            BrMapAdd(pNew);
            return pNew;
        }
    }
    PDFatalError("Can't swap in pixelmap");
#else
    __asm {
            push ebx
            push ebp
            mov ebp, dword ptr [esp + 0xc]
            mov ebx, ecx
            push esi
            xor esi, esi
            mov ecx, dword ptr [ebp + 0x10]
            push edi
            test ecx, ecx
            mov edi, edx
            jle L00502de1
            mov eax, dword ptr [ebp + 0x30]
    L00502d98:
            cmp edi, dword ptr [eax]
            je L00502db7
            inc esi
            add eax, 4
            cmp esi, ecx
            jl L00502d98
            mov ecx, offset gMaterial_shading_for_callback+0xc
            call PDFatalError
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 4
    L00502db7:
            push edi
            call BrMapRemove
            add esp, 4
            push edi
            call BrPixelmapFree
            mov eax, dword ptr [ebp + 0x30]
            add esp, 4
            push ebx
            mov dword ptr [eax + esi*4], ebx
            call BrMapAdd
            add esp, 4
            mov eax, ebx
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 4
    L00502de1:
            mov ecx, offset gMaterial_shading_for_callback+0xc
            call PDFatalError
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 4
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004761e0
br_uint_32 C2_HOOK_FASTCALL CalcProximities(br_actor* pActor, br_material* pMat, void* pData) {
    tFunkotronic_spec* the_funk;
    br_face* the_face;
    int i;

    (void)pMat;
    the_funk = pData;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, material, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, proximity_count, 0x150);

    if (pActor->model != NULL) {
        for (i = 0, the_face = pActor->model->faces; i < pActor->model->nfaces; i++, the_face++) {
            if (the_face->material == the_funk->material) {
                the_funk->proximity_count++;
            }
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00476230
br_uint_32 C2_NAKED C2_HOOK_FASTCALL AddProximities(br_actor* pActor, br_material* pMat, void* pData) {
#ifndef CARPOCALYPSE2_MATCHING
    tFunkotronic_spec* the_funk = pData;

    if (pActor->model != NULL) {
        int i;
        for (i = 0; i < pActor->model->nfaces; i++) {
            br_face* the_face = &pActor->model->faces[i];
            if (the_face->material == the_funk->material) {
                int j;

                for (j = 0; j < 3; j++) {
                    BrVector3Copy(
                        &the_funk->proximity_array[the_funk->proximity_count].v[j],
                        &pActor->model->vertices[the_face->vertices[j]].p);
                    gFunk_temp_vertices[the_funk->proximity_count].vertices[j] = &pActor->model->vertices[the_face->vertices[j]];
                }
                BrVector3Normalise(&the_funk->proximity_array[the_funk->proximity_count].n, &the_face->n);
                the_funk->proximity_array[the_funk->proximity_count].d = -the_face->d;
                the_funk->proximity_count += 1;
            }
        }
    }
    return 0;
#else
    __asm {
            sub esp, 0xc
            mov eax, dword ptr [ecx + 0x18]
            push edi
            test eax, eax
            mov dword ptr [esp + 4], ecx
            je L00476425
            mov edi, dword ptr [eax + 0xc]
            cmp word ptr [eax + 0x12], 0
            mov dword ptr [esp + 8], 0
            mov dword ptr [esp + 0xc], edi
            jbe L00476425
            mov edx, dword ptr [esp + 0x14]
            push esi
            push ebp
            push ebx
    L00476264:
            mov eax, dword ptr [edi + 8]
            mov esi, dword ptr [edx + 8]
            cmp eax, esi
            jne L00476401
            xor ebx, ebx
            xor esi, esi
            jmp L0047627c
    L00476278:
            mov ecx, dword ptr [esp + 0x10]
    L0047627c:
            mov ecx, dword ptr [ecx + 0x18]
            xor eax, eax
            mov ax, word ptr [edi]
            add edi, 2
            mov ecx, dword ptr [ecx + 8]
            lea eax, [eax + eax*4]
            lea ecx, [ecx + eax*8]
            mov eax, dword ptr [edx + 0x150]
            lea ebp, [eax + eax*2]
            lea eax, [eax + ebp*4]
            mov ebp, dword ptr [edx + 0x154]
            lea eax, [esi + eax*4]
            add esi, 0xc
            add eax, ebp
            mov ebp, dword ptr [ecx]
            mov dword ptr [eax], ebp
            mov ebp, dword ptr [ecx + 4]
            mov dword ptr [eax + 4], ebp
            mov ecx, dword ptr [ecx + 8]
            mov dword ptr [eax + 8], ecx
            mov eax, dword ptr [esp + 0x10]
            mov ebp, dword ptr [gFunk_temp_vertices]
            mov ecx, dword ptr [eax + 0x18]
            xor eax, eax
            mov ax, word ptr [edi - 2]
            mov ecx, dword ptr [ecx + 8]
            lea eax, [eax + eax*4]
            lea ecx, [ecx + eax*8]
            mov eax, dword ptr [edx + 0x150]
            lea eax, [eax + eax*2]
            lea eax, [ebx + eax*4]
            inc ebx
            cmp esi, 0x24
            mov dword ptr [ebp + eax*4], ecx
            jl L00476278
            mov edi, dword ptr [esp + 0x18]
            fld dword ptr [edi + 0x18]
            fld dword ptr [edi + 0x1c]
            fld dword ptr [edi + 0x20]
            fxch st(2)
            fst dword ptr [esp + 0x20]
            fld st(1)
            fxch st(1)
            fmul dword ptr [esp + 0x20]
            fxch st(1)
            fmul st(0), st(2)
            fxch st(3)
            fst dword ptr [esp + 0x20]
            fmul dword ptr [esp + 0x20]
            fxch st(3)
            faddp st(1), st(0)
            fxch st(2)
            faddp st(2), st(0)
            fxch st(1)
            fsqrt
            fstp st(1)
            fcom dword ptr [gDouble_0058a068+0xf0]
            fnstsw ax
            test ah, 0x41
            jne L0047638b
            fdivr qword ptr [gDouble_0058a068+0xb8]
            mov eax, dword ptr [edx + 0x150]
            lea ecx, [eax + eax*2]
            lea eax, [eax + ecx*4]
            mov ecx, dword ptr [edx + 0x154]
            fld dword ptr [edi + 0x18]
            fmul st(0), st(1)
            fstp dword ptr [ecx + eax*4 + 0x24]
            mov eax, dword ptr [edx + 0x150]
            fld dword ptr [edi + 0x1c]
            lea ecx, [eax + eax*2]
            fmul st(0), st(1)
            lea eax, [eax + ecx*4]
            mov ecx, dword ptr [edx + 0x154]
            fstp dword ptr [ecx + eax*4 + 0x28]
            mov eax, dword ptr [edx + 0x150]
            fld dword ptr [edi + 0x20]
            lea ecx, [eax + eax*2]
            fmul st(0), st(1)
            lea eax, [eax + ecx*4]
            mov ecx, dword ptr [edx + 0x154]
            fstp dword ptr [ecx + eax*4 + 0x2c]
            fstp st(0)
            jmp L004763d5
    L0047638b:
            mov eax, dword ptr [edx + 0x150]
            fstp st(0)
            lea ecx, [eax + eax*2]
            lea eax, [eax + ecx*4]
            mov ecx, dword ptr [edx + 0x154]
            mov dword ptr [ecx + eax*4 + 0x24], 0x3f800000
            mov eax, dword ptr [edx + 0x150]
            mov esi, dword ptr [edx + 0x154]
            lea ecx, [eax + eax*2]
            lea eax, [eax + ecx*4]
            xor ecx, ecx
            mov dword ptr [esi + eax*4 + 0x28], ecx
            mov eax, dword ptr [edx + 0x150]
            lea esi, [eax + eax*2]
            lea eax, [eax + esi*4]
            mov esi, dword ptr [edx + 0x154]
            mov dword ptr [esi + eax*4 + 0x2c], ecx
    L004763d5:
            mov eax, dword ptr [edx + 0x150]
            fld dword ptr [edi + 0x24]
            lea ecx, [eax + eax*2]
            fchs
            lea eax, [eax + ecx*4]
            mov ecx, dword ptr [edx + 0x154]
            fstp dword ptr [ecx + eax*4 + 0x30]
            mov eax, dword ptr [edx + 0x150]
            mov ecx, dword ptr [esp + 0x10]
            inc eax
            mov dword ptr [edx + 0x150], eax
    L00476401:
            mov esi, dword ptr [ecx + 0x18]
            mov eax, dword ptr [esp + 0x14]
            xor ebx, ebx
            inc eax
            mov bx, word ptr [esi + 0x12]
            add edi, 0x28
            cmp eax, ebx
            mov dword ptr [esp + 0x14], eax
            mov dword ptr [esp + 0x18], edi
            jl L00476264
            pop ebx
            pop ebp
            pop esi
    L00476425:
            xor eax, eax
            pop edi
            add esp, 0xc
            ret 4
            nop
            nop
    }
#endif
}

void C2_HOOK_FASTCALL ShiftBoundGrooveFunks(char* pStart, char* pEnd, ptrdiff_t pDelta) {
    int i;

    C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(gGroove_funk_bindings) != 1440);
    C2_HOOK_BUG_ON(sizeof(tGroove_funk_binding) != 0x8);

    for (i = 0; i < CARPOCALYPSE2_ASIZE(gGroove_funk_bindings); i++) {
        if (pStart <= (char*)gGroove_funk_bindings[i].v && (char*)gGroove_funk_bindings[i].v < pEnd) {
            gGroove_funk_bindings[i].v = (float*)((char*)gGroove_funk_bindings[i].v + (pDelta & ~(sizeof(void*) - 1)));
        }
    }
}

tFunkotronic_spec* C2_HOOK_FASTCALL AddNewFunkotronic(void) {
    void* new_array;
    int i;

    C2_HOOK_BUG_ON(sizeof(tFunkotronic_spec) != 0x158);
    C2_HOOK_BUG_ON(sizeof(tFunk_proximity) != 0x34);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, owner, 0x0);

    for (i = 0; i < gFunkotronics_array_size; i++) {
        if (gFunkotronics_array[i].owner == -999) {
            memset(&gFunkotronics_array[i], 0, sizeof(tFunkotronic_spec));
            return &gFunkotronics_array[i];
        }
    }
    gFunkotronics_array_size += 16;
    new_array = BrMemCalloc(gFunkotronics_array_size, sizeof(tFunkotronic_spec), kMem_funk_spec);
    if (gFunkotronics_array != NULL) {
        memcpy(new_array, gFunkotronics_array, (gFunkotronics_array_size - 16) * sizeof(tFunkotronic_spec));
        ShiftBoundGrooveFunks(
                (char*)gFunkotronics_array,
                (char*)&gFunkotronics_array[gFunkotronics_array_size - 16],
                (char*)new_array - (char*)gFunkotronics_array);
        BrMemFree(gFunkotronics_array);
    }
    gFunkotronics_array = new_array;
    for (i = 0; i < 16; i++) {
        gFunkotronics_array[gFunkotronics_array_size - 16 + i].owner = -999;
    }
    return &gFunkotronics_array[gFunkotronics_array_size - 16];
}

br_material* C2_HOOK_FASTCALL TryThisFunkLink(tCar_crush_buffer_entry* pFunk_link, const char* pStr, tFunkotronic_spec* pFunk) {
    int i;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_crush_buffer_entry, count_smashables, 0x38);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_crush_buffer_entry, smashables, 0x3c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_crush_smashable_part, funk_material, 0x44);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_crush_smashable_part, material_name, 0x0);

    if (pFunk_link == NULL) {
        return NULL;
    }
    for (i = 0; i < pFunk_link->count_smashables; i++) {
        if (strcmp(pFunk_link->smashables[i].material_name, pStr) == 0) {
            pFunk_link->smashables[i].funk = pFunk - gFunkotronics_array;
            return pFunk_link->smashables[i].funk_material;
        }
    }
    return NULL;
}

br_material* C2_HOOK_FASTCALL FindSmashableMaterial(const char* pStr, tFunkotronic_spec* pFunk, tCar_crush_buffer* pCar_crush_datas) {
    int i;

    for (i = 0; i < pCar_crush_datas->count_entries; i++) {
        br_material* material;

        material = TryThisFunkLink(pCar_crush_datas->entries[i], pStr, pFunk);
        if (material != NULL) {
            return material;
        }
    }
    return NULL;
}

void C2_HOOK_FASTCALL AddFunkGrooveBinding(int pSlot_number, float* pPeriod_address) {

    if (pSlot_number < 0 || pSlot_number >= CARPOCALYPSE2_ASIZE(gGroove_funk_bindings)) {
        FatalError(kFatalError_DefinedRefNumOfControlledGoorvFunkOutOfRange);
    }

    gGroove_funk_bindings[pSlot_number].v = pPeriod_address;
    gGroove_funk_bindings[pSlot_number].field_0x4 = 0;
    *pPeriod_address = 0.f;
}

static void texture_string_to_bits(tU8 *bits, const char *s) {
    const char CHAR_BITS[] = "THBVLRF";
    unsigned int i;
    for (i = 0; i < strlen(s); i++) {
        unsigned int j;
        for (j = 0; j < strlen(CHAR_BITS); j++) {
            if (CHAR_BITS[j] == s[i]) {
                bits[i] = j;
                break;
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x00474ac0
void C2_HOOK_FASTCALL AddFunkotronics(FILE* pF, int pOwner, int pRef_offset, tCar_crush_buffer* pCar_crush_datas) {
    int first_time;
    char s[256];
    tFunkotronic_spec* the_funk;
    char* str;
    float x_0;
    float x_1;
    int g0;
    int g1;
    float speed1;
    float speed2;
    int i;
    float a_min;
    float d_min;
    float s_min;
    float a_max;
    float d_max;
    float s_max;
    float yon_factor;
    float fov_factor;

#ifndef CARPOCALYPSE2_MATCHING
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, flags, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, material, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, mode, 0xc);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, matrix_mod_type, 0x10);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, matrix_mode, 0x14);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, proximity_count, 0x150);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, proximity_array, 0x154);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, matrix_mod_data.spin_info.period, 0x18);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, matrix_mod_data.texture_info.data, 0x18);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, matrix_mod_data.rock_info.period, 0x18);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, matrix_mod_data.rock_info.x_centre, 0x1c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, matrix_mod_data.rock_info.y_centre, 0x20);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, matrix_mod_data.rock_info.rock_angle, 0x24);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, matrix_mod_data.throb_info.x_period, 0x18);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, matrix_mod_data.throb_info.y_period, 0x1c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, matrix_mod_data.throb_info.x_centre, 0x20);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, matrix_mod_data.throb_info.y_centre, 0x24);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, matrix_mod_data.throb_info.x_magnitude, 0x28);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, matrix_mod_data.throb_info.y_magnitude, 0x2c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, matrix_mod_data.slither_info.x_period, 0x18);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, matrix_mod_data.slither_info.y_period, 0x1c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, matrix_mod_data.slither_info.x_magnitude, 0x20);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, matrix_mod_data.slither_info.y_magnitude, 0x24);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, matrix_mod_data.roll_info.x_period, 0x18);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, matrix_mod_data.roll_info.y_period, 0x1c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, lighting_animation_type, 0x30);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, lighting_animation_data.controlled.period, 0x34);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, lighting_animation_data.texture_info.data, 0x34);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, lighting_animation_data.rock_info.period, 0x34);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, ambient_base, 0x38);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, ambient_delta, 0x3c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, direct_base, 0x40);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, direct_delta, 0x44);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, specular_base, 0x48);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, specular_delta, 0x4c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, texture_animation_type, 0x50);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, time_mode, 0x54);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, last_frame, 0x58);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, texture_animation_data.frames_info.mode, 0x5c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, texture_animation_data.frames_info.controlled.period, 0x60);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, texture_animation_data.frames_info.texture_info.data, 0x60);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, texture_animation_data.frames_info.rock_info.period, 0x60);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, texture_animation_data.frames_info.texture_count, 0x64);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, texture_animation_data.frames_info.current_frame, 0x68);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, texture_animation_data.frames_info.has_matrix, 0x6c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, texture_animation_data.frames_info.textures, 0x70);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, texture_animation_data.frames_info.mat, 0x90);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, texture_animation_data.flic_info.flic_data, 0x5c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, texture_animation_data.flic_info.flic_data_length, 0x60);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, texture_animation_data.flic_info.flic_descriptor, 0x64);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, texture_animation_data.flic_info.flic_descriptor.width, 0xa8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, texture_animation_data.flic_info.flic_descriptor.height, 0xac);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, texture_animation_data.camera_info.mode, 0x5c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, texture_animation_data.camera_info.field_0x60, 0x60);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, texture_animation_data.camera_info.count, 0x64);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, texture_animation_data.camera_info.actors, 0x68);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, texture_animation_data.mirror_info.yon, 0x60);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, texture_animation_data.mirror_info.actor, 0x64);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, texture_animation_data.mirror_info.mat, 0x68);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, texture_animation_data.mirror_info.bounds, 0x98);
    C2_HOOK_BUG_ON(sizeof(tFunk_temp_buffer) != 0x30);
    C2_HOOK_BUG_ON(sizeof(tFunk_texturebits) != 0x28);
    C2_HOOK_BUG_ON(500 * sizeof(tFunk_temp_buffer) != 24000);
#endif

    gFunk_temp_vertices = BrMemAllocate(500 * sizeof(tFunk_temp_buffer), BR_MEMORY_APPLICATION);
    first_time = 1;
    while (!PFfeof(pF)) {
        PossibleService();
        GetALineAndDontArgue(pF, s);
        if (strcmp(s, "END OF FUNK") == 0) {
            break;
        }
        if (!first_time) {
            if (strcmp(s, "NEXT FUNK") != 0) {
                FatalError(kFatalError_FunkotronicFile);
            }
            GetALineAndDontArgue(pF, s);
        }
        first_time = 0;
        str = strtok(s, "\t ,/");

        the_funk = AddNewFunkotronic();
        the_funk->owner = pOwner;
        the_funk->material = NULL;
        if (pCar_crush_datas != NULL) {
            the_funk->material = FindSmashableMaterial(s, the_funk, pCar_crush_datas);
        }
        if (the_funk->material == NULL) {
            the_funk->material = BrMaterialFind(str);
        }
        if (the_funk->material == NULL) {
            FatalError(kFatalError_CannotFindMainMaterialInFunkotronicFile_S, str);
        }
        str = strtok(NULL, "\t ,/");
        if (str != NULL && strcmp(str, "multiple") == 0 && the_funk->material->identifier != NULL) {
            BrResFree(the_funk->material->identifier);
            the_funk->material->identifier = NULL;
        }
        the_funk->mode = GetALineAndInterpretCommand(pF, gFunk_nature_names, CARPOCALYPSE2_ASIZE(gFunk_nature_names));
        the_funk->proximity_count = 0;
        the_funk->proximity_array = NULL;
        if (the_funk->mode == eFunk_mode_distance) {
            DRActorEnumRecurseWithMat(gUniverse_actor, NULL, CalcProximities, the_funk);
            the_funk->proximity_array = BrMemAllocate(the_funk->proximity_count * sizeof(tFunk_proximity), kMem_funk_prox_array);
            the_funk->proximity_count = 0;
            DRActorEnumRecurseWithMat(gUniverse_actor, NULL, AddProximities, the_funk);
        }
        the_funk->matrix_mod_type = GetALineAndInterpretCommand(pF, gFunk_type_names, CARPOCALYPSE2_ASIZE(gFunk_type_names));
        the_funk->flags &= ~0x1;
        if (the_funk->matrix_mod_type != eMatrix_mod_none) {
            the_funk->matrix_mode = GetALineAndInterpretCommand(pF, gFunk_move_names, CARPOCALYPSE2_ASIZE(gFunk_move_names));
        }
        switch (the_funk->matrix_mod_type) {
        case eMatrix_mod_spin:
            switch (the_funk->matrix_mode) {
            case eMove_controlled:
            case eMove_absolute:
                g0 = GetAnInt(pF);
                AddFunkGrooveBinding(pRef_offset + g0, &the_funk->matrix_mod_data.spin_info.period);
                break;
            case eMove_texturebits:
                the_funk->matrix_mod_data.texture_info.data = BrMemAllocate(sizeof(tFunk_texturebits), kMem_funk_spec);
                GetAString(pF, s);
                the_funk->matrix_mod_data.texture_info.data->count = (tU8)strlen(s);
                texture_string_to_bits(the_funk->matrix_mod_data.texture_info.data->bits, s);
                the_funk->matrix_mod_data.texture_info.data->car = gCurrent_car_spec;
                break;
            default:
                x_0 = GetAScalar(pF);
                the_funk->matrix_mod_data.rock_info.period = (x_0 == 0.0f) ? 0.0f : 1000.0f / x_0;
                break;
            }
            break;
        case eMatrix_mod_rock:
            switch (the_funk->matrix_mode) {
            case eMove_controlled:
            case eMove_absolute:
                g0 = GetAnInt(pF);
                AddFunkGrooveBinding(pRef_offset + g0, &the_funk->matrix_mod_data.rock_info.period);
                break;
            case eMove_texturebits:
                the_funk->matrix_mod_data.texture_info.data = BrMemAllocate(sizeof(tFunk_texturebits), kMem_funk_spec);
                GetAString(pF, s);
                the_funk->matrix_mod_data.texture_info.data->count = (tU8)strlen(s);
                texture_string_to_bits(the_funk->matrix_mod_data.texture_info.data->bits, s);
                the_funk->matrix_mod_data.texture_info.data->car = gCurrent_car_spec;
                break;
            default:
                x_0 = GetAScalar(pF);
                the_funk->matrix_mod_data.rock_info.period = (x_0 == 0.0f) ? 0.0f : 1000.0f / x_0;
                break;
            }
            the_funk->matrix_mod_data.rock_info.rock_angle = GetAScalar(pF);
            GetPairOfFloats(pF, &x_0, &x_1);
            the_funk->matrix_mod_data.rock_info.x_centre = x_0 / 100.0f;
            the_funk->matrix_mod_data.rock_info.y_centre = x_1 / 100.0f;
            break;
        case eMatrix_mod_throb:
            switch (the_funk->matrix_mode) {
            case eMove_controlled:
            case eMove_absolute:
                GetPairOfInts(pF, &g0, &g1);
                if (g0 >= 0) {
                    AddFunkGrooveBinding(pRef_offset + g0, &the_funk->matrix_mod_data.throb_info.x_period);
                }
                if (g1 >= 0) {
                    AddFunkGrooveBinding(pRef_offset + g1, &the_funk->matrix_mod_data.throb_info.y_period);
                }
                break;
            default:
                GetPairOfFloats(pF, &speed1, &speed2);
                the_funk->matrix_mod_data.throb_info.x_period = (speed1 == 0.0f) ? 0.0f : 1000.0f / speed1;
                the_funk->matrix_mod_data.throb_info.y_period = (speed2 == 0.0f) ? 0.0f : 1000.0f / speed2;
                break;
            }
            GetPairOfFloatPercents(pF,
                &the_funk->matrix_mod_data.throb_info.x_magnitude,
                &the_funk->matrix_mod_data.throb_info.y_magnitude);
            GetPairOfFloats(pF, &x_0, &x_1);
            the_funk->matrix_mod_data.throb_info.x_centre = x_0 / 100.0f;
            the_funk->matrix_mod_data.throb_info.y_centre = x_1 / 100.0f;
            if (the_funk->matrix_mode != eMove_controlled) {
                if (the_funk->matrix_mod_data.throb_info.x_period == 0.0f) {
                    the_funk->matrix_mod_data.throb_info.x_period = 1.0f;
                    the_funk->matrix_mod_data.throb_info.x_magnitude = 0.0f;
                }
                if (the_funk->matrix_mod_data.throb_info.y_period == 0.0f) {
                    the_funk->matrix_mod_data.throb_info.y_period = 1.0f;
                    the_funk->matrix_mod_data.throb_info.y_magnitude = 0.0f;
                }
            }
            break;
        case eMatrix_mod_slither:
            switch (the_funk->matrix_mode) {
            case eMove_controlled:
            case eMove_absolute:
                GetPairOfInts(pF, &g0, &g1);
                if (g0 >= 0) {
                    AddFunkGrooveBinding(pRef_offset + g0, &the_funk->matrix_mod_data.slither_info.x_period);
                }
                if (g1 >= 0) {
                    AddFunkGrooveBinding(pRef_offset + g1, &the_funk->matrix_mod_data.slither_info.y_period);
                }
                break;
            default:
                GetPairOfFloats(pF, &speed1, &speed2);
                the_funk->matrix_mod_data.slither_info.x_period = (speed1 == 0.0f) ? 0.0f : 1000.0f / speed1;
                the_funk->matrix_mod_data.slither_info.y_period = (speed2 == 0.0f) ? 0.0f : 1000.0f / speed2;
                break;
            }
            GetPairOfFloatPercents(pF,
                &the_funk->matrix_mod_data.slither_info.x_magnitude,
                &the_funk->matrix_mod_data.slither_info.y_magnitude);
            if (the_funk->matrix_mode != eMove_controlled) {
                if (the_funk->matrix_mod_data.slither_info.x_period == 0.0f) {
                    the_funk->matrix_mod_data.slither_info.x_period = 1.0f;
                    the_funk->matrix_mod_data.slither_info.x_magnitude = 0.0f;
                }
                if (the_funk->matrix_mod_data.slither_info.y_period == 0.0f) {
                    the_funk->matrix_mod_data.slither_info.y_period = 1.0f;
                    the_funk->matrix_mod_data.slither_info.y_magnitude = 0.0f;
                }
            }
            break;
        case eMatrix_mod_roll:
            switch (the_funk->matrix_mode) {
            case eMove_controlled:
            case eMove_absolute:
                GetPairOfInts(pF, &g0, &g1);
                if (g0 >= 0) {
                    AddFunkGrooveBinding(pRef_offset + g0, &the_funk->matrix_mod_data.roll_info.x_period);
                }
                if (g1 >= 0) {
                    AddFunkGrooveBinding(pRef_offset + g1, &the_funk->matrix_mod_data.roll_info.y_period);
                }
                break;
            default:
                GetPairOfFloats(pF, &speed1, &speed2);
                the_funk->matrix_mod_data.roll_info.x_period = speed1 == 0.0f ? 0.0f : 1000.0f / speed1;
                the_funk->matrix_mod_data.roll_info.y_period = speed2 == 0.0f ? 0.0f : 1000.0f / speed2;
                break;
            }
            break;
        default:
            break;
        }
        the_funk->lighting_animation_type = GetALineAndInterpretCommand(pF, gFunk_move_names, CARPOCALYPSE2_ASIZE(gFunk_move_names));
        if (the_funk->lighting_animation_type != eMove_none) {
            switch (the_funk->lighting_animation_type) {
            case eMove_controlled:
            case eMove_absolute:
                g0 = GetAnInt(pF);
                AddFunkGrooveBinding(pRef_offset + g1, &the_funk->lighting_animation_data.controlled.period);
                break;
            case eMove_texturebits:
                the_funk->lighting_animation_data.texture_info.data = BrMemAllocate(sizeof(tFunk_texturebits), kMem_funk_spec);
                GetAString(pF, s);
                the_funk->lighting_animation_data.texture_info.data->count = (tU8)strlen(s);
                texture_string_to_bits(the_funk->lighting_animation_data.texture_info.data->bits, s);
                the_funk->lighting_animation_data.texture_info.data->car = gCurrent_car_spec;
                break;
            default:
                x_0 = GetAFloat(pF);
                the_funk->lighting_animation_data.rock_info.period = (x_0 == 0.0f) ? 0.0f : 1000.0f / x_0;
                break;
            }
            GetThreeFloatPercents(pF, &a_min, &d_min, &s_min);
            GetThreeFloatPercents(pF, &a_max, &d_max, &s_max);
            the_funk->ambient_base = (a_min + a_max) / 2.0f;
            the_funk->direct_base = (d_min + d_max) / 2.0f;
            the_funk->specular_base = (s_min + s_max) / 2.0f;
            the_funk->ambient_delta = (a_max - a_min) / 2.0f;
            the_funk->direct_delta = (d_max - d_min) / 2.0f;
            the_funk->specular_delta = (s_max - s_min) / 2.0f;
        }
        the_funk->texture_animation_type = GetALineAndInterpretCommand(pF, gFunk_anim_names, CARPOCALYPSE2_ASIZE(gFunk_anim_names));
        if (the_funk->texture_animation_type != eTexture_animation_none
                && the_funk->texture_animation_type != eTexture_animation_mirror
                && the_funk->texture_animation_type != eTexture_animation_camera) {
            the_funk->time_mode = GetALineAndInterpretCommand(pF, gTime_mode_names, CARPOCALYPSE2_ASIZE(gTime_mode_names));
        }
        if (the_funk->texture_animation_type == eTexture_animation_flic && gAusterity_mode) {
            the_funk->texture_animation_type = eTexture_animation_none;
            GetALineAndDontArgue(pF, s);
        }
        the_funk->last_frame = 0.0f;
        switch (the_funk->texture_animation_type) {
        case eTexture_animation_frames:
            the_funk->texture_animation_data.frames_info.mode = GetALineAndInterpretCommand(pF, gFunk_move_names, CARPOCALYPSE2_ASIZE(gFunk_move_names));
            switch (the_funk->texture_animation_data.frames_info.mode) {
            case eMove_controlled:
            case eMove_absolute:
                g0 = GetAnInt(pF);
                AddFunkGrooveBinding(pRef_offset + g0, &the_funk->texture_animation_data.frames_info.controlled.period);
                break;
            case eMove_texturebits:
                the_funk->texture_animation_data.frames_info.texture_info.data = BrMemAllocate(sizeof(tFunk_texturebits), kMem_funk_spec);
                GetAString(pF, s);
                the_funk->texture_animation_data.frames_info.texture_info.data->count = (tU8)strlen(s);
                texture_string_to_bits(the_funk->texture_animation_data.frames_info.texture_info.data->bits, s);
                the_funk->texture_animation_data.frames_info.texture_info.data->car = gCurrent_car_spec;
                break;
            default:
                x_0 = GetAFloat(pF);
                the_funk->texture_animation_data.frames_info.rock_info.period = (x_0 == 0.0f) ? 0.0f : 1000.0F / x_0;
                break;
            }
            the_funk->texture_animation_data.frames_info.texture_count = (int)GetAScalar(pF);
            the_funk->texture_animation_data.frames_info.current_frame = 0;
            the_funk->texture_animation_data.frames_info.has_matrix = 0;
            for (i = 0; i < the_funk->texture_animation_data.frames_info.texture_count; i++) {
                GetALineAndDontArgue(pF, s);
                str = strtok(s, "\t ,/");
                the_funk->texture_animation_data.frames_info.textures[i] = BrMapFind(str);
                if (the_funk->texture_animation_data.frames_info.textures[i] == NULL) {
                    FatalError(kFatalError_CannotFindAnimationFramePixelmapReferencedInFunkotronicFile);
                }
                BrMatrix23Identity(&the_funk->texture_animation_data.frames_info.mat[i]);
                str = strtok(NULL, "\t ,/");
                if (str != NULL && strlen(str) != 0) {
                    int size_x;
                    int pos_x;
                    int size_y;
                    int pos_y;

                    the_funk->texture_animation_data.frames_info.has_matrix = 1;
                    sscanf(str, "%d", &size_x);
                    str = strtok(NULL, "\t ,/");
                    sscanf(str, "%d", &pos_x);
                    str = strtok(NULL, "\t ,/");
                    sscanf(str, "%d", &size_y);
                    str = strtok(NULL, "\t ,/");
                    sscanf(str, "%d", &pos_y);
                    the_funk->texture_animation_data.frames_info.mat[i].m[0][0] = 1.f / (float)size_x;
                    the_funk->texture_animation_data.frames_info.mat[i].m[1][1] = 1.f / (float)size_y;
                    the_funk->texture_animation_data.frames_info.mat[i].m[2][0] = (float)pos_x / (float)size_x;
                    the_funk->texture_animation_data.frames_info.mat[i].m[2][1] = (float)pos_y / (float)size_y;
                }
            }
            break;
        case eTexture_animation_flic:
            GetAString(pF, s);
            the_funk->texture_animation_data.flic_info.flic_data = NULL;
            if (LoadFlicData(s, &the_funk->texture_animation_data.flic_info.flic_data, &the_funk->texture_animation_data.flic_info.flic_data_length)) {
                char *the_pixels;
                br_pixelmap *the_pixelmap;

                the_funk->texture_animation_data.flic_info.flic_descriptor.data_start = NULL;
                StartFlic(s, -1, &the_funk->texture_animation_data.flic_info.flic_descriptor, the_funk->texture_animation_data.flic_info.flic_data_length, (tS8*)the_funk->texture_animation_data.flic_info.flic_data, NULL, 0, 0, 0);
                the_funk->last_frame = 0.0f;
                the_pixels = BrMemAllocate(
                    the_funk->texture_animation_data.flic_info.flic_descriptor.height
                        * ((the_funk->texture_animation_data.flic_info.flic_descriptor.width + 3) & ~3),
                        kMem_video_pixels);
                if (gScreen->row_bytes < 0) {
                    BrFatal("C:\\Carma2\\Source\\Common\\Funkgroo.c", 732,
                            "Bruce bug at line %d, file C:\\Carma2\\Source\\Common\\Funkgroo.c", 732);
                }
                the_pixelmap = DRPixelmapAllocate(gScreen->type,
                    the_funk->texture_animation_data.flic_info.flic_descriptor.width,
                    the_funk->texture_animation_data.flic_info.flic_descriptor.height,
                    the_pixels,
                    0);
                AssertFlicPixelmap(&the_funk->texture_animation_data.flic_info.flic_descriptor, the_pixelmap);
                the_funk->material->colour_map = the_pixelmap;
                the_funk->material->user = the_pixelmap;
                BrMaterialUpdate(the_funk->material, BR_MATU_ALL);
            } else {
                the_funk->texture_animation_type = eTexture_animation_none;
            }
            break;
        case eTexture_animation_camera:
            the_funk->texture_animation_data.camera_info.mode = GetALineAndInterpretCommand(pF, gCamera_animation_names, CARPOCALYPSE2_ASIZE(gCamera_animation_names));
            GetPairOfFloats(pF, &yon_factor, &fov_factor);
            the_funk->texture_animation_data.camera_info.field_0x60 = GetAnInt(pF);
            the_funk->texture_animation_data.camera_info.count = GetAnInt(pF);
            for (i = 0; i < the_funk->texture_animation_data.camera_info.count; i++) {
                br_actor* actor;
                br_camera* camera;
                br_camera* global_camera;
                br_vector3 cam_pos;

                actor = BrActorAllocate(BR_ACTOR_CAMERA, NULL);
                if (actor == NULL) {
                    FatalError(kFatalError_CannotAllocateCamera);
                }
                the_funk->texture_animation_data.camera_info.actors[i] = actor;
                camera = actor->type_data;
                global_camera = gCamera->type_data;
                camera->type = BR_CAMERA_PERSPECTIVE_FOV;
                camera->field_of_view = (br_angle)(fov_factor * global_camera->field_of_view);
                camera->hither_z = global_camera->hither_z;
                camera->yon_z = yon_factor * global_camera->yon_z;
                actor = BrActorAdd(gUniverse_actor, actor);
                if (actor == NULL) {
                    FatalError(kFatalError_CannotAllocateCamera);
                }
                GetThreeFloats(pF, &cam_pos.v[0], &cam_pos.v[1], &cam_pos.v[2]);
                if (the_funk->texture_animation_data.camera_info.mode == eFunk_camera_static) {
                    br_euler e;
                    float angle1;
                    float angle2;

                    GetPairOfFloats(pF, &angle1, &angle2);
                    e.order = 0x12;
                    e.a = BrDegreeToAngle(angle1);
                    e.b = BrDegreeToAngle(angle2);
                    e.c = 0;
                    BrEulerToMatrix34(&actor->t.t.mat, &e);
                }
                BrVector3Copy(&actor->t.t.translate.t, &cam_pos);
            }
            break;
        case eTexture_animation_mirror:
            if (the_funk->proximity_count == 0) {
                the_funk->texture_animation_data.mirror_info.actor = NULL;
                GetALineAndDontArgue(pF, s);
                GetALineAndDontArgue(pF, s);
            } else {
                br_actor* actor;
                br_camera* camera;
                br_camera* global_camera;
                br_vector3 n;
                br_matrix34 mat;

                global_camera = gCamera->type_data;
                yon_factor = GetAScalar(pF);
                the_funk->texture_animation_data.mirror_info.yon = yon_factor * global_camera->yon_z;
                the_funk->texture_animation_data.mirror_info.field_0x5c = GetAnInt(pF);
                actor = BrActorAllocate(BR_ACTOR_CAMERA, NULL);
                if (actor == NULL) {
                    FatalError(kFatalError_CannotAllocateCamera);
                }
                the_funk->texture_animation_data.mirror_info.actor = actor;
                camera = actor->type_data;
                camera->type = BR_CAMERA_PERSPECTIVE_FOV;
                camera->field_of_view = global_camera->field_of_view;
                camera->hither_z = global_camera->hither_z;
                camera->yon_z = global_camera->yon_z + global_camera->hither_z;
                actor = BrActorAdd(gUniverse_actor, actor);
                if (actor == NULL) {
                    FatalError(kFatalError_CannotAllocateCamera);
                }
                BrVector3Copy(&n, &the_funk->proximity_array->n);
                BrVector3Set((br_vector3*)the_funk->texture_animation_data.mirror_info.mat.m[0],
                    1.f - n.v[0] * n.v[0] + n.v[0] * n.v[0],
                    n.v[0] * n.v[1] + n.v[0] * n.v[1],
                    n.v[0] * n.v[2] + n.v[0] * n.v[2]);
                BrVector3Set((br_vector3*)the_funk->texture_animation_data.mirror_info.mat.m[1],
                    n.v[0] * n.v[1] + n.v[0] * n.v[1],
                    1.f - n.v[1] * n.v[1] + n.v[1] * n.v[1],
                    n.v[1] * n.v[2] + n.v[1] * n.v[2]);
                BrVector3Set((br_vector3*)the_funk->texture_animation_data.mirror_info.mat.m[2],
                    n.v[0] * n.v[2] + n.v[0] * n.v[2],
                    n.v[1] * n.v[2] + n.v[1] * n.v[2],
                    1.f - n.v[2] * n.v[2] + n.v[2] * n.v[2]);
                BrVector3Scale((br_vector3*)the_funk->texture_animation_data.mirror_info.mat.m[3], &n, -2.f * the_funk->proximity_array->d);
                BrVector3Sub((br_vector3*)mat.m[0], &the_funk->proximity_array->v[1], &the_funk->proximity_array->v[0]);
                BrVector3Copy((br_vector3*)mat.m[1], &the_funk->proximity_array->n);
                BrVector3Cross((br_vector3*)mat.m[2], (br_vector3*)mat.m[0], (br_vector3*)mat.m[1]);
                BrVector3SetFloat(&the_funk->texture_animation_data.mirror_info.bounds.min, (float)0x7fffffff, (float)0x7fffffff, (float)0x7fffffff);
                BrVector3SetFloat(&the_funk->texture_animation_data.mirror_info.bounds.max, (float)-0x7fffffff, (float)-0x7fffffff, (float)-0x7fffffff);
                for (i = 0; i < the_funk->proximity_count; i++) {
                    int j;

                    for (j = 0; j < 3; j++) {
                        int k;

                        BrMatrix34ApplyP(&gFunk_temp_vertices[i].points[j],
                            &the_funk->proximity_array[i].v[j], /* FIXME: original has &the_funk->proximity_array->v[j] */
                            &mat);
                        for (k = 0; k < 3; k++) {
                            if (the_funk->texture_animation_data.mirror_info.bounds.min.v[k] > gFunk_temp_vertices[i].points[j].v[k]) {
                                the_funk->texture_animation_data.mirror_info.bounds.min.v[k] = gFunk_temp_vertices[i].points[j].v[k];
                            }
                            if (the_funk->texture_animation_data.mirror_info.bounds.max.v[k] < gFunk_temp_vertices[i].points[j].v[k]) {
                                the_funk->texture_animation_data.mirror_info.bounds.max.v[k] = gFunk_temp_vertices[i].points[j].v[k];
                            }
                        }
                    }
                }
                for (i = 0; i < the_funk->proximity_count; i++) {
                    int j;

                    for (j = 0; j < 3; j++) {
                        int k;

                        gFunk_temp_vertices[i].points[j].v[0] =
                            (gFunk_temp_vertices[i].points[j].v[0] - the_funk->texture_animation_data.mirror_info.bounds.min.v[0])
                                / (the_funk->texture_animation_data.mirror_info.bounds.max.v[0] - the_funk->texture_animation_data.mirror_info.bounds.min.v[0]);
                        gFunk_temp_vertices[i].points[j].v[1] =
                            (gFunk_temp_vertices[i].points[j].v[2] - the_funk->texture_animation_data.mirror_info.bounds.min.v[2])
                                / (the_funk->texture_animation_data.mirror_info.bounds.max.v[2] - the_funk->texture_animation_data.mirror_info.bounds.min.v[2]);
                        for (k = 0; k < 3; k++) {
                            if (the_funk->texture_animation_data.mirror_info.bounds.min.v[k] > gFunk_temp_vertices[i].points[j].v[k]) {
                                the_funk->texture_animation_data.mirror_info.bounds.min.v[k] = gFunk_temp_vertices[i].points[j].v[k];
                            }
                            if (the_funk->texture_animation_data.mirror_info.bounds.max.v[k] < gFunk_temp_vertices[i].points[j].v[k]) {
                                the_funk->texture_animation_data.mirror_info.bounds.max.v[k] = gFunk_temp_vertices[i].points[j].v[k];
                            }
                        }
                    }
                }
            }
            break;
        default:
            break;
        }
    }
    BrMemFree(gFunk_temp_vertices);
}

tGroovidelic_spec* C2_HOOK_FASTCALL AddNewGroovidelic(void) {
    void* new_array;
    int i;

    C2_HOOK_BUG_ON(sizeof(tGroovidelic_spec) != 0x84);

    for (i = 0; i < gGroovidelics_array_size; i++) {
        if (gGroovidelics_array[i].owner == -999) {
            memset(&gGroovidelics_array[i], 0, sizeof(tGroovidelic_spec));
            return &gGroovidelics_array[i];
        }
    }
    gGroovidelics_array_size += 16;
    new_array = BrMemCalloc(gGroovidelics_array_size, sizeof(tGroovidelic_spec), kMem_groove_spec);
    if (gGroovidelics_array != NULL) {
        memcpy(new_array, gGroovidelics_array, (gGroovidelics_array_size - 16) * sizeof(tGroovidelic_spec));
        ShiftBoundGrooveFunks(
                (char*)gGroovidelics_array,
                (char*)&gGroovidelics_array[gGroovidelics_array_size - 16],
                (char*)new_array - (char*)gGroovidelics_array);
        BrMemFree(gGroovidelics_array);
    }
    gGroovidelics_array = new_array;
    for (i = 0; i < 16; i++) {
        gGroovidelics_array[i + gGroovidelics_array_size - 16].owner = -999;
    }
    return &gGroovidelics_array[gGroovidelics_array_size - 16];
}

// FUNCTION: CARMA2_HW 0x00476470
void C2_HOOK_FASTCALL AddGroovidelics(FILE* pF, int pOwner, br_actor* pParent_actor, int pRef_offset, int pAllowed_to_be_absent) {
    int first_time;
    char s[256];
    char* str;
    float x_0;
    float x_1;
    float x_2;
    int d_0;
    int d_1;
    int d_2;
    br_vector3 p;

#ifndef CARPOCALYPSE2_MATCHING
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, owner, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, block_flags, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, actor, 0xc);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, lollipop_mode, 0x10);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, mode, 0x14);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, path_type, 0x18);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, path_mode, 0x1c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, path_interrupt_status, 0x20);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, path_data.straight_info.texture_info.data, 0x28);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, path_data.straight_info.period.value, 0x28);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, path_data.straight_info.x_delta, 0x2c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, path_data.straight_info.y_delta, 0x30);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, path_data.straight_info.z_delta, 0x34);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, path_data.straight_info.centre, 0x38);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, path_data.circular_info.period.value, 0x28);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, path_data.circular_info.texture_info.data, 0x28);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, path_data.circular_info.radius, 0x2c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, path_data.circular_info.centre, 0x30);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, path_data.circular_info.axis, 0x3c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_centre, 0x44);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_position, 0x50);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_type, 0x5c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_mode, 0x60);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_interrupt_status, 0x64);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_data.spin_info.period.value, 0x6c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_data.spin_info.texture_info.data, 0x6c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_data.spin_info.axis, 0x70);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_data.rock_info.period.value, 0x6c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_data.rock_info.texture_info.data, 0x6c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_data.rock_info.max_angle, 0x70);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_data.rock_info.axis, 0x78);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_data.throb_info.x_period, 0x6c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_data.throb_info.y_period, 0x70);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_data.throb_info.z_period, 0x74);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_data.throb_info.x_magnitude, 0x78);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_data.throb_info.y_magnitude, 0x7c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_data.throb_info.z_magnitude, 0x80);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_data.shear_info.x_period, 0x6c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_data.shear_info.y_period, 0x70);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_data.shear_info.z_period, 0x74);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_data.shear_info.x_magnitude, 0x78);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_data.shear_info.y_magnitude, 0x7c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_data.shear_info.z_magnitude, 0x80);
#endif

    first_time = 1;
    while (!PFfeof(pF)) {
        tGroovidelic_spec* the_groove;

        PossibleService();
        GetALineAndDontArgue(pF, s);
        if (strcmp(s, "END OF GROOVE") == 0) {
            break;
        }

        if (!first_time) {
            if (strcmp(s, "NEXT GROOVE") != 0) {
                FatalError(kFatalError_ErrorWithinGroovidelicFile);
            }
            GetALineAndDontArgue(pF, s);
        }
        first_time = 0;

        str = strtok(s, "\t ,/");
        the_groove = AddNewGroovidelic();
        the_groove->owner = pOwner;
        the_groove->block_flags = 0;
        the_groove->actor = DRActorFindRecurse(pParent_actor, str);

        if (the_groove->actor == NULL) {
            if (!pAllowed_to_be_absent && !gAusterity_mode) {
                FatalError(kFatalError_CannotFindActorReferencedInGroovidelicFile_S, str);
            }
            if (gGroove_by_proxy_actor == NULL) {
                gGroove_by_proxy_actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
                gGroove_by_proxy_actor->model = LoadModel("PROXY.DAT");
                BrModelAdd(gGroove_by_proxy_actor->model);
                BrActorAdd(gDont_render_actor, gGroove_by_proxy_actor);
            }
            the_groove->actor = gGroove_by_proxy_actor;
        }
        the_groove->lollipop_mode = GetALineAndInterpretCommand(pF, gLollipop_names, CARPOCALYPSE2_ASIZE(gLollipop_names));
        the_groove->mode = GetALineAndInterpretCommand(pF, gGroove_nature_names, CARPOCALYPSE2_ASIZE(gGroove_nature_names));
        the_groove->path_type = GetALineAndInterpretCommand(pF, gGroove_path_names, CARPOCALYPSE2_ASIZE(gGroove_path_names));
        the_groove->path_interrupt_status = eInterrupt_none;
        the_groove->object_interrupt_status = eInterrupt_none;
        if (the_groove->path_type != eGroove_path_none) {
            the_groove->path_mode = GetALineAndInterpretCommand(pF, gFunk_move_names, CARPOCALYPSE2_ASIZE(gFunk_move_names));
        }
        switch (the_groove->path_type) {
        case eGroove_path_straight:
            GetThreeFloats(pF,
                &the_groove->path_data.straight_info.centre.v[0],
                &the_groove->path_data.straight_info.centre.v[1],
                &the_groove->path_data.straight_info.centre.v[2]);

            if (Vector3IsZero(&the_groove->path_data.straight_info.centre)) {
                BrVector3Copy(&the_groove->path_data.straight_info.centre,
                    &the_groove->actor->t.t.translate.t);
            }
            switch (the_groove->path_mode) {
            case eMove_controlled:
            case eMove_absolute:
                AddFunkGrooveBinding(pRef_offset + GetAnInt(pF), &the_groove->path_data.straight_info.period.value);
                break;
            case eMove_texturebits:
                the_groove->path_data.straight_info.texture_info.data = BrMemAllocate(sizeof(tFunk_texturebits), kMem_funk_spec);
                GetAString(pF, s);
                the_groove->path_data.straight_info.texture_info.data->count = (tU8)strlen(s);
                texture_string_to_bits(the_groove->path_data.straight_info.texture_info.data->bits, s);
                the_groove->path_data.straight_info.texture_info.data->car = gCurrent_car_spec;
                break;
            default:
                x_0 = GetAFloat(pF);
                the_groove->path_data.straight_info.period.value = x_0 == 0.0f ? 0.0f : 1000.0f / x_0;
                break;
            }
            GetThreeFloats(pF,
                &the_groove->path_data.straight_info.x_delta,
                &the_groove->path_data.straight_info.y_delta,
                &the_groove->path_data.straight_info.z_delta);
            break;
        case eGroove_path_circular:
             GetThreeFloats(pF, &p.v[0], &p.v[1], &p.v[2]);
             BrVector3Copy(&the_groove->path_data.circular_info.centre, &p);
             if (Vector3IsZero(&the_groove->path_data.circular_info.centre)) {
                 BrVector3Copy(&the_groove->path_data.circular_info.centre,
                    &the_groove->actor->t.t.translate.t);
             }
            switch (the_groove->path_mode) {
            case eMove_controlled:
            case eMove_absolute:
                AddFunkGrooveBinding(pRef_offset + GetAnInt(pF), &the_groove->path_data.circular_info.period.value);
                break;
            case eMove_texturebits:
                the_groove->path_data.circular_info.texture_info.data = BrMemAllocate(sizeof(tFunk_texturebits), kMem_funk_spec);
                GetAString(pF, s);
                the_groove->path_data.circular_info.texture_info.data->count = (tU8)strlen(s);
                texture_string_to_bits(the_groove->path_data.circular_info.texture_info.data->bits, s);
                the_groove->path_data.circular_info.texture_info.data->car = gCurrent_car_spec;
                break;
            default:
                x_0 = GetAFloat(pF);
                the_groove->path_data.circular_info.period.value = x_0 == 0.0f ? 0.0f : 1000.0f / x_0;
                break;
            }
            the_groove->path_data.circular_info.radius = GetAFloat(pF);
            the_groove->path_data.circular_info.axis =  GetALineAndInterpretCommand(pF, gAxis_names, CARPOCALYPSE2_ASIZE(gAxis_names));
            break;
        default:
            break;
        }

        the_groove->object_type = GetALineAndInterpretCommand(pF, gGroove_object_names, CARPOCALYPSE2_ASIZE(gGroove_object_names));
        BrVector3Copy(&the_groove->object_position, &the_groove->actor->t.t.translate.t);
        if (the_groove->object_type != eGroove_object_none) {
            the_groove->object_mode = GetALineAndInterpretCommand(pF, gFunk_move_names, CARPOCALYPSE2_ASIZE(gFunk_move_names));
        }
        switch (the_groove->object_type) {
        case eGroove_object_spin:
            switch (the_groove->object_mode) {
            case eMove_controlled:
            case eMove_absolute:
                AddFunkGrooveBinding(pRef_offset + GetAnInt(pF), &the_groove->object_data.spin_info.period.value);
                break;
            case eMove_texturebits:
                the_groove->object_data.spin_info.texture_info.data = BrMemAllocate(sizeof(tFunk_texturebits), kMem_funk_spec);
                GetAString(pF, s);
                the_groove->object_data.spin_info.texture_info.data->count = (tU8)strlen(s);
                texture_string_to_bits(the_groove->object_data.spin_info.texture_info.data->bits, s);
                the_groove->object_data.spin_info.texture_info.data->car = gCurrent_car_spec;
                break;
            default:
                x_0 = GetAFloat(pF);
                the_groove->object_data.spin_info.period.value = (x_0 == 0.0f) ? 0.0f : (1000.0f / x_0);
            }
            GetThreeFloats(pF,
                &the_groove->object_centre.v[0],
                &the_groove->object_centre.v[1],
                &the_groove->object_centre.v[2]);
            the_groove->object_data.spin_info.axis = GetALineAndInterpretCommand(pF, gAxis_names, CARPOCALYPSE2_ASIZE(gAxis_names));
            break;
        case eGroove_object_rock:
            switch (the_groove->object_mode) {
            case eMove_controlled:
            case eMove_absolute:
                AddFunkGrooveBinding(pRef_offset + GetAnInt(pF), &the_groove->object_data.spin_info.period.value);
                break;
            case eMove_texturebits:
                the_groove->object_data.rock_info.texture_info.data = BrMemAllocate(sizeof(tFunk_texturebits), kMem_funk_spec);
                GetAString(pF, s);
                the_groove->object_data.rock_info.texture_info.data->count = (tU8)strlen(s);
                texture_string_to_bits(the_groove->object_data.rock_info.texture_info.data->bits, s);
                the_groove->object_data.rock_info.texture_info.data->car = gCurrent_car_spec;
                break;
            default:
                x_0 = GetAFloat(pF);
                the_groove->object_data.spin_info.period.value = (x_0 == 0.0f) ? 0.0f : (1000.0f / x_0);
            }
            GetThreeFloats(pF,
                &the_groove->object_centre.v[0],
                &the_groove->object_centre.v[1],
                &the_groove->object_centre.v[2]);
            the_groove->object_data.rock_info.axis = GetALineAndInterpretCommand(pF, gAxis_names, CARPOCALYPSE2_ASIZE(gAxis_names));
            the_groove->object_data.rock_info.max_angle = GetAFloat(pF);
            break;
        case eGroove_object_throb:
            switch (the_groove->object_mode) {
            case eMove_controlled:
            case eMove_absolute:
                GetThreeInts(pF, &d_0, &d_1, &d_2);
                if (d_0 >= 0) {
                    AddFunkGrooveBinding(pRef_offset + d_0, &the_groove->object_data.throb_info.x_period.value);
                }
                if (d_1 >= 0) {
                    AddFunkGrooveBinding(pRef_offset + d_1, &the_groove->object_data.throb_info.y_period.value);
                }
                if (d_2 >= 0) {
                    AddFunkGrooveBinding(pRef_offset + d_2, &the_groove->object_data.throb_info.z_period.value);
                }
                break;
            default:
                GetThreeFloats(pF, &x_0, &x_1, &x_2);
                the_groove->object_data.throb_info.x_period.value = (x_0 == 0.0f) ? 0.0f : (1000.0f / x_0);
                the_groove->object_data.throb_info.y_period.value = (x_1 == 0.0f) ? 0.0f : (1000.0f / x_1);
                the_groove->object_data.throb_info.z_period.value = (x_2 == 0.0f) ? 0.0f : (1000.0f / x_2);
                break;
            }
            GetThreeFloats(pF,
                &the_groove->object_centre.v[0],
                &the_groove->object_centre.v[1],
                &the_groove->object_centre.v[2]);
            GetThreeFloatPercents(pF,
                &the_groove->object_data.throb_info.x_magnitude,
                &the_groove->object_data.throb_info.y_magnitude,
                &the_groove->object_data.throb_info.z_magnitude);
            break;
        case eGroove_object_shear:
            switch (the_groove->object_mode) {
            case eMove_controlled:
            case eMove_absolute:
                GetThreeInts(pF, &d_0, &d_1, &d_2);
                if (d_0 >= 0) {
                    AddFunkGrooveBinding(pRef_offset + d_0, &the_groove->object_data.shear_info.x_period.value);
                }
                if (d_1 >= 0) {
                    AddFunkGrooveBinding(pRef_offset + d_1, &the_groove->object_data.shear_info.y_period.value);
                }
                if (d_2 >= 0) {
                    AddFunkGrooveBinding(pRef_offset + d_2, &the_groove->object_data.shear_info.z_period.value);
                }
                break;
            default:
                GetThreeFloats(pF, &x_0, &x_1, &x_2);
                the_groove->object_data.shear_info.x_period.value = x_0 == 0.0f ? 0.0f : 1000.0f / x_0;
                the_groove->object_data.shear_info.y_period.value = x_1 == 0.0f ? 0.0f : 1000.0f / x_1;
                the_groove->object_data.shear_info.z_period.value = x_2 == 0.0f ? 0.0f : 1000.0f / x_2;
            }
            GetThreeFloats(pF,
                &the_groove->object_centre.v[0],
                &the_groove->object_centre.v[1],
                &the_groove->object_centre.v[2]);
            GetThreeFloatPercents(pF,
                &the_groove->object_data.shear_info.x_magnitude,
                &the_groove->object_data.shear_info.y_magnitude,
                &the_groove->object_data.shear_info.z_magnitude);
            break;
        default:
            break;
        }
    }
}

// FUNCTION: CARMA2_HW 0x004ff110
tSpecial_volume* C2_HOOK_FASTCALL GetDefaultSpecialVolumeForWater(void) {
    return gDefault_water_spec_vol_real;
}

// FUNCTION: CARMA2_HW 0x00486dc0
void C2_HOOK_FASTCALL ReadGlobalLightingInfo(FILE* pF) {
    /* //////////// GLOBAL LIGHTING DATA /////////// */

    /* RGB for main directional light-source */
    GetThreeInts(pF, &gLighting_data.directional.red, &gLighting_data.directional.green, &gLighting_data.directional.blue);
    /* Ambient/Diffuse light to be used when plaything ambient says 0 */
    GetPairOfScalars(pF, &gLighting_data.ambient_0, &gLighting_data.diffuse_0);
    /* Ambient/Diffuse light to be used when plaything ambient says 1 */
    GetPairOfScalars(pF, &gLighting_data.ambient_1, &gLighting_data.diffuse_1);
    /* Ambient/Diffuse light to be used when plaything ambient says anything else */
    GetPairOfScalars(pF, &gLighting_data.ambient_else, &gLighting_data.diffuse_else);
}

// FUNCTION: CARMA2_HW 0x004f0430
void C2_HOOK_FASTCALL InitRepairAnimations(void) {
    int i;

    C2_HOOK_BUG_ON(sizeof(tRepair_animation) != 0xc);
    C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(gRepair_animations) != 25);

    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gRepair_animations); i++) {
        gRepair_animations[i].field_0x0 = 0;
    }
}

void C2_HOOK_FASTCALL ReadSmokeFireChance(FILE* pF, tSmokeFireChance* pSmokeFireChance) {

    /* %chance fire */
    pSmokeFireChance->chance_fire = GetAnInt(pF);
    if (pSmokeFireChance->chance_fire != 0) {
        pSmokeFireChance->model_1_int = GetAnInt(pF);
        GetPairOfInts(pF, &pSmokeFireChance->model_2_int, &pSmokeFireChance->model_3_int);
    }
}

void C2_HOOK_FASTCALL ReadSmashReplace(tSmashReplace* pReplace, FILE* pF) {
    char s[256];

    /* new model */
    GetAString(pF, s);
    pReplace->model = BrModelFind(s);
    if (pReplace->model == NULL) {
        FatalError(kFatalError_CannotFindSmashActorModel_S, s);
    }
    ReadSmokeFireChance(pF, &pReplace->smoke_fire_chance);
}

void C2_HOOK_FASTCALL ReadSmashTexture(tSmashable_item_spec* pSmash_item, tSmashable_item_spec_texture_change* pTexture_change, FILE* pF) {
    char s[256] = "";

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSmashable_item_spec_texture_change, levels, 0x50);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSmashable_item_spec_texture_change, trigger, 0x44);

    pTexture_change->levels = NULL;
    strcpy(pTexture_change->undefined_0x0, s);

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSmashable_item_spec_texture_change, field_0x40, 0x40);

    pTexture_change->field_0x40 = -1;
    pTexture_change->trigger = pSmash_item->trigger_object;
    pTexture_change->field_0x4c = 0;
    pTexture_change->field_0x40 = -1;
    pTexture_change->field_0x6c = 0;
    pTexture_change->field_0x70 = 0;
    pTexture_change->field_0x74 = 0;
    pTexture_change->field_0x78 = 0;
    pTexture_change->field_0x54 = 0.f;
    pTexture_change->field_0x58 = 0.f;
    pTexture_change->field_0x5c = 0.f;
    pTexture_change->field_0x60 = 1.f;
    pTexture_change->field_0x64 = 0.f;
    pTexture_change->field_0x68 = 0.f;
    LoadSmashableLevels(pF,
        &pTexture_change->levels,
        &pTexture_change->count_levels,
        pSmash_item->mode == kSmashableMode_TextureChange,
        &gTrack_storage_space);
}

void C2_HOOK_FASTCALL ReadSmashSounds(FILE* pF, tConnotations* pConnotations, tBrender_storage* pStorage) {
    int i;

    /* number of possible sounds */
    pConnotations->count_sounds = GetAnInt(pF);

    for (i = 0; i < pConnotations->count_sounds; i++) {
        int sound;

        /* sound id */
        sound = GetAnInt(pF);

        pConnotations->sounds[i] = LoadSingleSound(pStorage, sound);
    }
}

void C2_HOOK_FASTCALL ReadSpecialEffectsSpec(FILE* pF, tSpecial_effects_spec* pSpecial_effects) {

    ReadExplosion(pF, &pSpecial_effects->explosion_animation);
    ReadSpillData(pF, &pSpecial_effects->slick);
}

void C2_HOOK_FASTCALL ReadSideEffects(FILE* pF, tSide_effects* pSide_effects) {

    ReadNonCarCuboidActivation(pF, &pSide_effects->activations);
    ReadShrapnelSideEffects(pF, &pSide_effects->side_effects);

    /* Extensions flags */
    pSide_effects->extension_flags = GetAnInt(pF);
    if (pSide_effects->extension_flags & 0x1) {
        pSide_effects->extension_arg = GetAnInt(pF);
    }
    /* Room turn on code */
    pSide_effects->room_turn_on_code = GetAnInt(pF);
}

void C2_HOOK_FASTCALL ReadVariableChanges(FILE* pF, tVariable_changes* pVariable_changes) {
    int i;

    /* run-time variable changes */
    pVariable_changes->count = GetAnInt(pF);
    for (i = 0; i < pVariable_changes->count; i++) {
        int v1, v2;

        GetPairOfInts(pF, &v1, &v2);
        pVariable_changes->runtime_changes[i].field_0x0 = v2;
        pVariable_changes->runtime_changes[i].field_0x2 = v1;
    }
}

void C2_HOOK_FASTCALL ReadConnotations(FILE* pF, tConnotations* pConnotations, tBrender_storage* pStorage) {

    ReadSmashSounds(pF, pConnotations, pStorage);

#ifndef CARPOCALYPSE2_MATCHING
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSmashable_item_spec, mode_data, 0x14);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSmashable_item_spec_shrapnel, connotations.count_shrapnel, 0x14);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSmashable_item_spec_shrapnel, connotations.shrapnel, 0x18);
#endif

    ReadShrapnelSpec(pF, pConnotations->shrapnel, &pConnotations->count_shrapnel);
    ReadSpecialEffectsSpec(pF, &pConnotations->special_effects);
    ReadSideEffects(pF, &pConnotations->side_effects);
    ReadAward(pF, &pConnotations->award);
    ReadVariableChanges(pF, &pConnotations->variable_changes);
}

// FUNCTION: CARMA2_HW 0x004f0450
void C2_NAKED C2_HOOK_FASTCALL ReadSmashableEnvironment(FILE* pF, const char* pPath) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;
    char s[256];

    gCount_smashable_noncars = 0;
    gCount_smashable_noncar_shrapnel_actors = 0;
    gSmashable_noncars = BrMemAllocate(100 * sizeof(tSmashable_environment_name), kMem_smashable_env_info);
    C2_HOOK_BUG_ON(100 * sizeof(tSmashable_environment_name) != 3200);

    /* Number of smash specs */
    gCount_track_smashable_environment_specs = GetAnInt(pF);

    gCapacity_track_smashables = 5 * (gCount_track_smashable_environment_specs + 30);
    gCount_track_smashable_environment_specs_2 = gCount_track_smashable_environment_specs;
    gTrack_smashable_environment_specs = BrMemAllocate(gCapacity_track_smashables * sizeof(tSmashable_item_spec), kMem_smashable_env_info);
    C2_HOOK_BUG_ON(sizeof(tSmashable_item_spec) != 736);

    gSmashable_track_environment_path = pPath;

    memset(gSmashable_track_indexable_triggers, 0, sizeof(gSmashable_track_indexable_triggers));
    C2_HOOK_BUG_ON(sizeof(gSmashable_track_indexable_triggers) != 400);

    for (i = 0; i < gCount_track_smashable_environment_specs; i++) {
        tSmashable_item_spec* spec = &gTrack_smashable_environment_specs[i];

        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSmashable_item_spec, flags, 0x0);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSmashable_item_spec, field_0x10, 0x10);

        /* Flags */
        spec->flags = GetAnInt(pF);
        spec->field_0x10 = 0;

        /* Name of trigger material */
        GetAString(pF, s);
        if (s[0] == '&') {
            if (s[1] >= '0' && s[1] <= '9' && s[3] == '\0') {
                spec->trigger_type = kSmashableTrigger_Number;
            } else {
                spec->trigger_type = kSmashableTrigger_Model;
            }
        } else {
            if (strstr(s, ".DAT") != NULL || strstr(s, ".dat") != NULL || strstr(s, ".ACT") != NULL || strstr(s, ".act") != NULL) {
                spec->trigger_type = kSmashableTrigger_Model;
            } else {
                spec->trigger_type = kSmashableTrigger_Material;
            }
        }

        switch (spec->trigger_type) {
        case kSmashableTrigger_Material:
            spec->trigger_object.material = BrMaterialFind(s);
            if (spec->trigger_object.material == NULL) {
                FatalError(kFatalError_CannotFindSmashMaterial_S, s);
            }
            break;
        case kSmashableTrigger_Model:
            spec->trigger_object.model = BrModelFind(s);
            if (spec->trigger_object.model == NULL) {
                FatalError(kFatalError_CannotFindSmashModel_S, s);
            }
            break;
        case kSmashableTrigger_Number:
            spec->trigger_object.number.field_0x0 = (s[1] + 24) * 10 + s[2]; /* FIXME: why 24? (-'0' == -48) */
            spec->trigger_object.number.field_0x1 = GetAnInt(pF);
            gSmashable_track_indexable_triggers[spec->trigger_object.number.field_0x0] = spec;
            break;
        }

        /* Mode */
        spec->mode = GetALineAndInterpretCommand(pF, gSmashable_item_mode_names, CARPOCALYPSE2_ASIZE(gSmashable_item_mode_names));
        switch (spec->mode) {
        case kSmashableMode_Decal:
        case kSmashableMode_TextureChange:
            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSmashable_item_spec, mode_data, 0x14);
            ReadSmashTexture(spec, &spec->mode_data.texture_change, pF);
            break;
        default:
            /* Removal threshold */
            spec->mode_data.shrapnel.removal_threshold = GetAScalar(pF);
            ReadConnotations(pF, &spec->mode_data.shrapnel.connotations, &gTrack_storage_space);
            break;
        }
        if (spec->mode == kSmashableMode_ReplaceModel) {
            ReadSmashReplace(&spec->replace, pF);
        }
        /* reserved 1 */
        GetAnInt(pF);
        /* reserved 2 */
        GetAnInt(pF);
        /* reserved 3 */
        GetAnInt(pF);
        GetAnInt(pF);

        spec->field_0x2d4 = 0;
        spec->field_0x2d8 = 0;
        spec->field_0x2dc = 0;
    }
    for (i = gCount_track_smashable_environment_specs; i < gCapacity_track_smashables; i++) {
        tSmashable_item_spec* spec = &gTrack_smashable_environment_specs[i];

        spec->trigger_type = -1;
    }
#else
    __asm {
            sub esp, 0x214
            push ebx
            push ebp
            push esi
            push edi
            xor esi, esi
            push 0xf9
            mov edi, edx
            mov ebx, ecx
            push 0xc80
            mov dword ptr [gCount_smashable_noncars], esi
            mov dword ptr [gCount_smashable_noncar_shrapnel_actors], esi
            call BrMemAllocate
            add esp, 8
            mov ecx, ebx
            mov dword ptr [gSmashable_noncars], eax
            call GetAnInt
            mov dword ptr [gCount_track_smashable_environment_specs], eax
            mov dword ptr [gCount_track_smashable_environment_specs_2], eax
            add eax, 0x1e
            push 0xf9
            lea eax, [eax + eax*4]
            mov dword ptr [gCapacity_track_smashables], eax
            lea ecx, [eax + eax*2]
            shl ecx, 3
            sub ecx, eax
            shl ecx, 5
            push ecx
            call BrMemAllocate
            mov ebp, eax
            mov dword ptr [gSmashable_track_environment_path], edi
            mov ecx, 0x64
            xor eax, eax
            mov edi, offset gSmashable_track_indexable_triggers
            add esp, 8
            rep stosd
            mov eax, dword ptr [gCount_track_smashable_environment_specs]
            xor ecx, ecx
            mov edi, ebp
            cmp eax, esi
            mov dword ptr [gTrack_smashable_environment_specs], ebp
            mov dword ptr [esp + 0x14], ecx
            mov dword ptr [esp + 0x10], edi
            jle L004f0912
            add ebp, 0x2d0
            jmp L004f04fb
    L004f04f5:
            mov edi, dword ptr [esp + 0x10]
            xor esi, esi
    L004f04fb:
            mov ecx, ebx
            call GetAnInt
            lea edx, [esp + 0x24]
            mov ecx, ebx
            mov dword ptr [edi], eax
            mov dword ptr [ebp - 0x2c0], esi
            call GetAString
            mov al, byte ptr [esp + 0x24]
            cmp al, 0x26
            jne L004f0543
            mov cl, byte ptr [esp + 0x25]
            cmp cl, 0x30
            jl L004f053f
            cmp cl, 0x39
            jg L004f053f
            mov cl, byte ptr [esp + 0x27]
            test cl, cl
            jne L004f053f
            mov dword ptr [ebp - 0x2c8], 2
            jmp L004f05ad
    L004f053f:
            cmp al, 0x26
            je L004f05a3
    L004f0543:
            lea edx, [esp + 0x24]
            push offset gFrontend_QUIT+0xbb7c
            push edx
            call strstr
            add esp, 8
            test eax, eax
            jne L004f05a3
            lea eax, [esp + 0x24]
            push offset gSplit_triangle_vertex_orders+0x1a4
            push eax
            call strstr
            add esp, 8
            test eax, eax
            jne L004f05a3
            lea ecx, [esp + 0x24]
            push offset gDefault_drone_direction+0x6dc
            push ecx
            call strstr
            add esp, 8
            test eax, eax
            jne L004f05a3
            lea edx, [esp + 0x24]
            push offset gSplit_triangle_vertex_orders+0x19c
            push edx
            call strstr
            add esp, 8
            test eax, eax
            jne L004f05a3
            mov dword ptr [ebp - 0x2c8], esi
            jmp L004f05ad
    L004f05a3:
            mov dword ptr [ebp - 0x2c8], 1
    L004f05ad:
            mov eax, dword ptr [ebp - 0x2c8]
            sub eax, esi
            je L004f0611
            dec eax
            je L004f05f1
            dec eax
            jne L004f0637
            mov al, byte ptr [esp + 0x25]
            mov cl, 0xa
            add al, 0x18
            imul cl
            mov cl, byte ptr [esp + 0x26]
            add al, cl
            mov ecx, ebx
            mov byte ptr [ebp - 0x2cc], al
            call GetAnInt
            xor edx, edx
            mov byte ptr [ebp - 0x2cb], al
            mov dl, byte ptr [ebp - 0x2cc]
            mov dword ptr [edx*4 + gSmashable_track_indexable_triggers], edi
            jmp L004f0637
    L004f05f1:
            lea eax, [esp + 0x24]
            push eax
            call BrModelFind
            add esp, 4
            cmp eax, esi
            mov dword ptr [ebp - 0x2cc], eax
            jne L004f0637
            lea ecx, [esp + 0x24]
            push ecx
            push 0x79
            jmp L004f062f
    L004f0611:
            lea edx, [esp + 0x24]
            push edx
            call BrMaterialFind
            add esp, 4
            cmp eax, esi
            mov dword ptr [ebp - 0x2cc], eax
            jne L004f0637
            lea eax, [esp + 0x24]
            push eax
            push 0x78
    L004f062f:
            call FatalError
            add esp, 8
    L004f0637:
            push 6
            mov edx, offset gSmashable_item_mode_names
            mov ecx, ebx
            call GetALineAndInterpretCommand
            cmp eax, 1
            mov dword ptr [ebp - 0x2c4], eax
            je L004f0792
            cmp eax, 2
            je L004f0792
            mov ecx, ebx
            call GetAScalar
            fstp dword ptr [ebp - 0x2bc]
            mov ecx, ebx
            call GetAnInt
            xor edi, edi
            cmp eax, esi
            mov dword ptr [ebp - 0x2b8], eax
            jle L004f06a8
            lea esi, [ebp - 0x2b4]
    L004f0683:
            mov ecx, ebx
            call GetAnInt
            mov edx, eax
            mov ecx, offset gTrack_storage_space
            call LoadSingleSound
            mov dword ptr [esi], eax
            mov eax, dword ptr [ebp - 0x2b8]
            inc edi
            add esi, 4
            cmp edi, eax
            jl L004f0683
            xor esi, esi
    L004f06a8:
            lea ecx, [ebp - 0x2a8]
            lea edx, [ebp - 0x2a4]
            push ecx
            mov ecx, ebx
            call ReadShrapnelSpec
            lea edi, [ebp - 0x94]
            mov ecx, ebx
            mov edx, edi
            call ReadExplosion
            lea edx, [edi + 8]
            mov ecx, ebx
            call ReadSpillData
            lea edi, [ebp - 0x5c]
            mov ecx, ebx
            mov edx, edi
            call ReadNonCarCuboidActivation
            lea edx, [edi + 8]
            mov ecx, ebx
            call ReadShrapnelSideEffects
            mov ecx, ebx
            call GetAnInt
            test al, 1
            mov dword ptr [edi + 0x14], eax
            je L004f0703
            mov ecx, ebx
            call GetAnInt
            mov dword ptr [edi + 0x10], eax
    L004f0703:
            mov ecx, ebx
            call GetAnInt
            mov edx, offset gRepeatability_names
            mov ecx, ebx
            push 4
            mov dword ptr [edi + 0x18], eax
            call GetALineAndInterpretCommand
            cmp eax, esi
            mov dword ptr [ebp - 0x40], eax
            je L004f074a
            mov ecx, ebx
            call GetAScalar
            fstp dword ptr [ebp - 0x3c]
            mov ecx, ebx
            call GetAScalar
            fstp dword ptr [ebp - 0x38]
            mov ecx, ebx
            call GetAnInt
            mov ecx, ebx
            mov dword ptr [ebp - 0x34], eax
            call GetAnInt
            mov dword ptr [ebp - 0x30], eax
    L004f074a:
            mov ecx, ebx
            call GetAnInt
            test al, al
            mov byte ptr [ebp - 0x28], al
            jbe L004f084f
            lea edi, [ebp - 0x26]
    L004f075f:
            lea edx, [esp + 0x18]
            mov ecx, ebx
            push edx
            lea edx, [esp + 0x24]
            call GetPairOfInts
            xor edx, edx
            inc esi
            mov ax, word ptr [esp + 0x20]
            mov cx, word ptr [esp + 0x18]
            mov word ptr [edi + 2], ax
            mov word ptr [edi], cx
            mov dl, byte ptr [ebp - 0x28]
            add edi, 4
            cmp esi, edx
            jl L004f075f
            jmp L004f084f
    L004f0792:
            lea edi, [esp + 0x124]
            or ecx, 0xffffffff
            xor eax, eax
            lea edx, [ebp - 0x26c]
            repne scasb
            not ecx
            mov dword ptr [edx], esi
            lea esi, [ebp - 0x2bc]
            sub edi, ecx
            mov dword ptr [esp + 0x1c], esi
            mov eax, ecx
            mov esi, edi
            mov edi, dword ptr [esp + 0x1c]
            push offset gTrack_storage_space
            shr ecx, 2
            rep movsd
            mov ecx, eax
            and ecx, 3
            rep movsb
            or ecx, 0xffffffff
            mov dword ptr [ebp - 0x27c], ecx
            mov eax, dword ptr [ebp - 0x2cc]
            mov dword ptr [ebp - 0x278], eax
            xor eax, eax
            mov dword ptr [ebp - 0x270], eax
            mov dword ptr [ebp - 0x27c], ecx
            mov dword ptr [ebp - 0x250], eax
            mov dword ptr [ebp - 0x24c], eax
            mov dword ptr [ebp - 0x248], eax
            mov dword ptr [ebp - 0x244], eax
            mov dword ptr [ebp - 0x268], eax
            mov dword ptr [ebp - 0x264], eax
            mov dword ptr [ebp - 0x260], eax
            mov dword ptr [ebp - 0x25c], 0x3f800000
            mov dword ptr [ebp - 0x258], eax
            mov dword ptr [ebp - 0x254], eax
            mov edi, dword ptr [ebp - 0x2c4]
            xor ecx, ecx
            cmp edi, 2
            lea eax, [ebp - 0x274]
            sete cl
            push ecx
            push eax
            mov ecx, ebx
            call LoadSmashableLevels
    L004f084f:
            mov eax, dword ptr [ebp - 0x2c4]
            cmp eax, 3
            je L004f08bc
            cmp eax, 4
            jne L004f08bc
            lea edx, [esp + 0x124]
            mov ecx, ebx
            call GetAString
            lea ecx, [esp + 0x124]
            push ecx
            call BrModelFind
            add esp, 4
            mov dword ptr [ebp - 0x10], eax
            test eax, eax
            jne L004f0899
            lea edx, [esp + 0x124]
            push edx
            push 0x81
            call FatalError
            add esp, 8
    L004f0899:
            mov ecx, ebx
            call GetAnInt
            test eax, eax
            mov dword ptr [ebp - 0xc], eax
            je L004f08bc
            mov ecx, ebx
            call GetAnInt
            lea edx, [ebp - 4]
            mov ecx, ebx
            push ebp
            mov dword ptr [ebp - 8], eax
            call GetPairOfInts
    L004f08bc:
            mov ecx, ebx
            call GetAnInt
            mov ecx, ebx
            call GetAnInt
            mov ecx, ebx
            call GetAnInt
            mov ecx, ebx
            call GetAnInt
            mov esi, dword ptr [esp + 0x10]
            xor eax, eax
            mov dword ptr [ebp + 8], eax
            mov dword ptr [ebp + 4], eax
            mov byte ptr [ebp + 0xc], al
            mov eax, dword ptr [esp + 0x14]
            mov ecx, dword ptr [gCount_track_smashable_environment_specs]
            inc eax
            add esi, 0x2e0
            add ebp, 0x2e0
            cmp eax, ecx
            mov dword ptr [esp + 0x14], eax
            mov dword ptr [esp + 0x10], esi
            jl L004f04f5
            mov ecx, eax
            mov edi, esi
    L004f0912:
            cmp ecx, dword ptr [gCapacity_track_smashables]
            jge L004f0933
            lea eax, [edi + 8]
    L004f091d:
            mov dword ptr [eax], 0xffffffff
            mov edx, dword ptr [gCapacity_track_smashables]
            inc ecx
            add eax, 0x2e0
            cmp ecx, edx
            jl L004f091d
    L004f0933:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x214
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004f5470
int C2_HOOK_FASTCALL SmashFaceMustBeUpdateable(br_material* pMaterial) {
    int i;

    for (i = 0; i < gCount_track_smashable_environment_specs; i++) {
        tSmashable_item_spec* smash = &gTrack_smashable_environment_specs[i];

        if (smash->trigger_type == kSmashableTrigger_Material && smash->trigger_object.material == pMaterial
                && ((smash->mode_data.texture_change.count_levels > 0 && smash->mode == kSmashableMode_TextureChange)
                || smash->mode == kSmashableMode_Remove
                || smash->mode == kSmashableMode_ReplaceModel)) {
            return 1;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004f09c0
void C2_HOOK_FASTCALL FreeEnvSmash(tSmashable_item_spec* pSmash) {
    int i;

    if (pSmash->trigger_type != -1) {
        if ((pSmash->mode == kSmashableMode_Decal || pSmash->mode == kSmashableMode_TextureChange) && pSmash->mode_data.texture_change.levels != NULL) {
            if (pSmash->mode_data.texture_change.field_0x70 != NULL) {
                BrMemFree(pSmash->mode_data.texture_change.field_0x70);
            }
            if (pSmash->mode_data.texture_change.field_0x78 != NULL) {
                BrMemFree(pSmash->mode_data.texture_change.field_0x78);
            }
            for (i = 1; i < pSmash->mode_data.texture_change.count_levels; i++) {
                FreeConnotations((tConnotations*)&pSmash->mode_data.texture_change.levels[i].count_sounds, pSmash->field_0x2dc);
            }
            BrMemFree(pSmash->mode_data.texture_change.levels);
        } else {
            FreeConnotations(&pSmash->mode_data.shrapnel.connotations, pSmash->field_0x2dc);
        }
        if (pSmash->flags & 0x80000000) {
            BrMaterialRemove(pSmash->trigger_object.material);
            BrMaterialFree(pSmash->trigger_object.material);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004f0960
void C2_HOOK_FASTCALL DisposeSmashableEnvironment(void) {
    int i;
    tSmashable_item_spec* smash;

    C2_HOOK_BUG_ON(sizeof(tSmashable_item_spec) != 0x2e0);

    for (i = 0, smash = gTrack_smashable_environment_specs; i < gCount_track_smashable_environment_specs; i++, smash++) {
        FreeEnvSmash(smash);
    }
    if (gCount_track_smashable_environment_specs != 0) {
        BrMemFree(gTrack_smashable_environment_specs);
    }
    if (gNet_mode == eNet_mode_host) {
        BrMemFree(gNet_host_smashes);
    }
}

// FUNCTION: CARMA2_HW 0x004f5750
void C2_NAKED C2_HOOK_FASTCALL ReinitSmashing(void) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;

    C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(gSmash_glass_fragments) != 200);
    C2_HOOK_BUG_ON(sizeof(gSmash_glass_fragments[0]) != 0x38);
    C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(gDecals) != 50);
    C2_HOOK_BUG_ON(sizeof(gDecals[0]) != 0x8);
    C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(gQueued_net_smashes) != 50);
    C2_HOOK_BUG_ON(sizeof(gQueued_net_smashes[0]) != 0x38);
    C2_HOOK_BUG_ON(sizeof(tStored_smash) != 0x38);

    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gSmash_glass_fragments); i++) {
        gSmash_glass_fragments[i].end_time = 0;
    }
    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gDecals); i++) {
        gDecals[i].time = 0;
    }
    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gQueued_net_smashes); i++) {
        gQueued_net_smashes[i].field_0x4 = 0;
    }
    gSize_powerup_queue = 0;
    gCount_host_smashes = 0;
    if (gNet_mode == eNet_mode_host) {
        gNet_host_smashes = BrMemAllocate(1000 * sizeof(tStored_smash), kMem_stored_smash);
    }
#else
    __asm {
            mov eax, offset gSmash_glass_fragments+0x8
            xor ecx, ecx
    L004f5757:
            mov dword ptr [eax], ecx
            add eax, 0x38
            cmp eax, offset gSmashable_track_indexable_triggers+0x8
            jl L004f5757
            mov eax, offset gDecals+0x4
    L004f5768:
            mov dword ptr [eax], ecx
            add eax, 8
            cmp eax, offset gCount_queued_smashes
            jl L004f5768
            mov eax, offset gQueued_net_smashes+0x4
    L004f5779:
            mov dword ptr [eax], ecx
            add eax, 0x38
            cmp eax, offset gPowerup_queue+0x4
            jl L004f5779
            mov eax, dword ptr [gNet_mode]
            mov dword ptr [gSize_powerup_queue], ecx
            cmp eax, 2
            mov dword ptr [gCount_host_smashes], ecx
            jne L004f57b2
            push 0xd1
            push 0xdac0
            call BrMemAllocate
            add esp, 8
            mov dword ptr [gNet_host_smashes], eax
    L004f57b2:
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

// FUNCTION: CARMA2_HW 0x004f0940
void C2_HOOK_FASTCALL DisposeSmashEnvNonCars(void) {
    BrMemFree(gSmashable_noncars);
}

// FUNCTION: CARMA2_HW 0x004ffd20
void C2_HOOK_FASTCALL FindInverseAndWorlBox(tSpecial_volume* pSpec) {
    br_bounds bnds;

    BrVector3Set(&bnds.min, -1.0f, -1.0f, -1.0f);
    BrVector3Set(&bnds.max, 1.0f, 1.0f, 1.0f);
    GetNewBoundingBox(&pSpec->boundary.box.bounds, &bnds, &pSpec->boundary.box.mat);
    BrMatrix34Inverse(&pSpec->boundary.box.inv_mat, &pSpec->boundary.box.mat);
}

// FUNCTION: CARMA2_HW 0x004ffd80
void C2_HOOK_FASTCALL ReadSpecialVolumes(FILE* pF) {
    int i;
    int j;
    tSpecial_volume* spec;
    tSpecial_volume_boundary_type boundary_type;

    C2_HOOK_BUG_ON(sizeof(tSpecial_volume) != 0xdc);
    C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(gSpecial_effects_boundary_choices) != 4);

    gDefault_engine_noise_index = GetAnInt(pF);
    gDefault_water_spec_vol_real = &gDefault_default_water_spec_vol;
    gProgram_state.special_volume_count = GetAnInt(pF);
    if (gProgram_state.special_volume_count != 0) {
        gProgram_state.special_volumes = BrMemAllocate(gProgram_state.special_volume_count * sizeof(tSpecial_volume), kMem_special_volume);
        for (i = 0, spec = gProgram_state.special_volumes; i < gProgram_state.special_volume_count; i++, spec++) {
            PossibleService();
            boundary_type = GetALineAndInterpretCommand(pF, gSpecial_effects_boundary_choices, CARPOCALYPSE2_ASIZE(gSpecial_effects_boundary_choices));
            spec->boundary_type = boundary_type;
            if (spec->boundary_type == eFx_boundary_new) {
                spec->boundary_type = eFx_boundary_box;
            }
            switch (spec->boundary_type) {
            case eFx_boundary_box:
                GetThreeScalars(pF, &spec->boundary.box.mat.m[0][0], &spec->boundary.box.mat.m[0][1], &spec->boundary.box.mat.m[0][2]);
                GetThreeScalars(pF, &spec->boundary.box.mat.m[1][0], &spec->boundary.box.mat.m[1][1], &spec->boundary.box.mat.m[1][2]);
                GetThreeScalars(pF, &spec->boundary.box.mat.m[2][0], &spec->boundary.box.mat.m[2][1], &spec->boundary.box.mat.m[2][2]);
                GetThreeScalars(pF, &spec->boundary.box.mat.m[3][0], &spec->boundary.box.mat.m[3][1], &spec->boundary.box.mat.m[3][2]);
                FindInverseAndWorlBox(spec);
                spec->boundary.box.axis_length.v[0] = BrVector3Length((br_vector3*)spec->boundary.box.inv_mat.m[0]);
                spec->boundary.box.axis_length.v[1] = BrVector3Length((br_vector3*)spec->boundary.box.inv_mat.m[1]);
                spec->boundary.box.axis_length.v[2] = BrVector3Length((br_vector3*)spec->boundary.box.inv_mat.m[2]);
                ParseSpecialVolume(pF, spec, NULL, boundary_type == eFx_boundary_box);
                if (spec->soundfx_type != kSoundFx_None) {
                    for (j = 0; j < CARPOCALYPSE2_ASIZE(spec->boundary.box.bounds.min.v); j++) {
                        spec->boundary.box.bounds.min.v[j] -= 10.0f;
                    }
                    for (j = 0; j < CARPOCALYPSE2_ASIZE(spec->boundary.box.bounds.max.v); j++) {
                        spec->boundary.box.bounds.max.v[j] += 10.0f;
                    }
                }
                break;
            case eFx_boundary_plane:
                GetFourScalars(pF, &spec->boundary.plane.v[0], &spec->boundary.plane.v[1], &spec->boundary.plane.v[2], &spec->boundary.plane.v[3]);
                ParseSpecialVolume(pF, spec, NULL, 1);
                break;
            case eFx_boundary_default:
                ParseSpecialVolume(pF, spec, NULL, 0);
                gDefault_water_spec_vol_real = spec;
                break;
            default:
                FatalError(kFatalError_ProblemWithSpecialVolumeNumber_I);
                break;
            }
        }
    }
}

void C2_HOOK_FASTCALL AddExceptionToList(tMaterial_exception** pList, tMaterial_exception* pItem) {

    pItem->next = *pList;
    *pList = pItem;
}

void C2_HOOK_FASTCALL LoadExceptionsFile(const char* pPath) {
    FILE* f;
    char* str;
    char s[256];
    int count;
    int version;

    f = DRfopen(pPath, "rt");
    if (f == NULL) {
        return;
    }
    GetALineAndDontArgue(f, s);
    str = strtok(s, "\t ,");
    if (DRStricmp(str, "VERSION") != 0) {
        FatalError(kFatalError_FileMustStartWith_SS, pPath, "VERSION");
    }
    str = strtok(NULL, "\t ,");
    count = sscanf(str, "%d", &version);
    if (count == 0 || version != 1) {
        FatalError(kFatalError_CantCopeWithVersionFor_SS, str, pPath);
    }
    while (1) {
        tMaterial_exception* matexc;

        GetALineAndDontArgue(f, s);
        str = strtok(s, "\t ,");
        if (DRStricmp(str, "end") == 0) {
            break;
        }
        C2_HOOK_BUG_ON(sizeof(tMaterial_exception) != 12);
        matexc = BrMemAllocate(sizeof(tMaterial_exception), kMem_exception);
        matexc->texture_name = BrMemAllocate(strlen(str) + 1, kMem_misc_string);
        strcpy(matexc->texture_name, str);
        matexc->flags = 0;
        while (1) {
            str = strtok(NULL, "\t ,");
            if (str == NULL) {
                break;
            }
            if (!isalnum(str[0])) {
                break;
            }
            if (DRStricmp(str, "nobilinear") == 0) {
                matexc->flags |= eMaterial_exception_nobilinear;
            } else if (DRStricmp(str, "wrap") == 0) {
                matexc->flags |= eMaterial_exception_wrap;
            } else {
                FatalError(kFatalError_Mysterious_SS, str, pPath);
            }
        }
        AddExceptionToList(&gMaterial_exceptions, matexc);
    }
    PFfclose(f);
}

void C2_HOOK_FASTCALL LoadExceptionsFileForTrack(const char* pTrack_name) {
    tPath_name path;

    sprintf(path, "%s%s%s%s",
               pTrack_name, gDir_separator,
               gRenderer_fixup_basename, gRenderer_fixup_extension);
    LoadExceptionsFile(path);
}

int C2_HOOK_FASTCALL ModelIsATree(br_model* pModel, char* pName_replacement) {
    int i;

    for (i = 0; i < gTree_surgery_pass1_count; i++) {
        if (strstr(pModel->identifier, gTree_surgery_pass1->name) == pModel->identifier) {
            break;
        }
    }
    if (i == gTree_surgery_pass1_count) {
        return 0;
    }
    for (i = 0; i < gTree_surgery_pass2_count; i++) {
        if (strcmp(pModel->identifier, gTree_surgery_pass2->original) == 0) {
            strcpy(pName_replacement, gTree_surgery_pass2->replacement);
            return 1;
        }
    }
    return 0;
}

void C2_HOOK_FASTCALL PerformTreeSurgery(tBrender_storage* pStorage, br_model* pModel, const char* pName_replacement) {
    br_model *replacement;

    replacement = LoadModel(pName_replacement);
    if (replacement == NULL) {
        return;
    }
    BrResFree(pModel->faces);
    BrResFree(pModel->vertices);
    pModel->faces = BrResAllocate(pModel, BrResSize(replacement->faces),
                                  BrResClass(replacement->faces));
    memmove(pModel->faces, replacement->faces, BrResSize(replacement->faces));
    pModel->nfaces = replacement->nfaces;
    pModel->vertices = BrResAllocate(pModel, BrResSize(replacement->vertices),
                                     BrResClass(replacement->vertices));
    memmove(pModel->vertices, replacement->vertices, BrResSize(replacement->vertices));
    pModel->nvertices = replacement->nvertices;
    BrModelUpdate(pModel, BR_MODU_ALL);
}

void C2_HOOK_FASTCALL PossibleTreeSurgery(br_model* pModel) {
    char name_replacement[36];

    if (pModel == NULL || pModel->identifier == NULL) {
        return;
    }
    if (ModelIsATree(pModel, name_replacement)) {
        PerformTreeSurgery(&gTrack_storage_space, pModel, name_replacement);
    }
}

void C2_HOOK_FASTCALL CheckNonCarModelBounds(void) {
    int i;

    for (i = 0; i < gProgram_state.track_spec.count_non_cars; i++) {
        br_actor* actor = gProgram_state.track_spec.non_car_list[i];
        int idx;
        tNon_car_spec* non_car;
        int j;

        if (actor == NULL) {
            continue;
        }
        idx = gNon_car_spec_indices[10 * (actor->identifier[1] - '0') + (actor->identifier[2] - '0')] + 40 - 1;
        non_car = &gProgram_state.non_cars[idx];
        if (non_car == NULL || non_car->collision_info->actor != NULL) {
            continue;
        }
        non_car->collision_info->actor = actor;
        for (j = 0; j < 3; j++) {
            actor->model->bounds.min.v[j] = MIN(actor->model->bounds.min.v[j], non_car->collision_info->bb2.min.v[j]);
            actor->model->bounds.max.v[j] = MAX(actor->model->bounds.max.v[j], non_car->collision_info->bb2.max.v[j]);
        }
    }
}

void C2_HOOK_FASTCALL FreeExceptions(void) {
    tMaterial_exception* material_exception;

    for (material_exception = gMaterial_exceptions; material_exception != NULL; ) {
        tMaterial_exception* next_material_exception = material_exception->next;
        BrMemFree(material_exception->texture_name);
        BrMemFree(material_exception);
        material_exception = next_material_exception;
    }
    gMaterial_exceptions = NULL;
}

// FUNCTION: CARMA2_HW 0x00486db0
void C2_HOOK_FASTCALL LoadStaticLightingForRace(const char* race_lighting_path) {
}

// FUNCTION: CARMA2_HW 0x004489d0
void C2_NAKED C2_HOOK_FASTCALL DisposeTexturingMaterials(void) {
#ifndef CARPOCALYPSE2_MATCHING

    switch (gWall_texturing_level) {
    case eWTL_linear:
        ProcessFaceMaterials(gProgram_state.track_spec.the_actor, WallLinearToPersp);
        break;
    case eWTL_none:
        ProcessFaceMaterials(gProgram_state.track_spec.the_actor, WallUntexToPersp);
        break;
    default:
        break;
    }
    switch (gRoad_texturing_level) {
    case eRTL_none:
        ProcessFaceMaterials(gProgram_state.track_spec.the_actor, RoadUntexToPersp);
        break;
    default:
        break;
    }
    if (gWall_texturing_level != eWTL_full || gRoad_texturing_level != eRTL_full) {
        ProcessFaceMaterials(gProgram_state.track_spec.the_actor, DisposeSuffixedMaterials);
    }
#else
    __asm {
            mov eax, dword ptr [gWall_texturing_level]
            sub esp, 8
            cmp eax, 1
            push ebx
            push ebp
            push esi
            push edi
            jne L00448a8c
            mov esi, dword ptr [gProgram_state+0x1bb4]
            xor ebx, ebx
            mov dword ptr [esp + 0x14], esi
            mov eax, dword ptr [esi + 0x14]
            cmp eax, ebx
            je L00448a01
            cmp byte ptr [eax], 0x26
            je L00448b6c
    L00448a01:
            cmp byte ptr [esi + 0x12], 1
            jne L00448a82
            mov ebp, dword ptr [esi + 0x18]
            cmp ebp, ebx
            je L00448a82
            cmp word ptr [ebp + 0x12], bx
            mov dword ptr [esp + 0x10], ebx
            jbe L00448a6c
    L00448a18:
            mov eax, ebx
            and eax, 0xffff
            lea esi, [eax + eax*4]
            mov eax, dword ptr [ebp + 0xc]
            shl esi, 3
            mov edi, dword ptr [esi + eax + 8]
            test edi, edi
            je L00448a61
            mov ecx, dword ptr [edi + 4]
            mov edx, offset gWall_texturing_tweakers+0x3c
            call HasThisSuffix
            test eax, eax
            je L00448a50
            mov ecx, dword ptr [edi + 4]
            mov edx, offset gWall_texturing_tweakers+0x3c
            call UnsuffixedMaterial
            jmp L00448a52
    L00448a50:
            xor eax, eax
    L00448a52:
            test eax, eax
            je L00448a61
            mov ecx, dword ptr [ebp + 0xc]
            mov dword ptr [esp + 0x10], eax
            mov dword ptr [esi + ecx + 8], eax
    L00448a61:
            inc ebx
            cmp bx, word ptr [ebp + 0x12]
            jb L00448a18
            mov esi, dword ptr [esp + 0x14]
    L00448a6c:
            mov eax, dword ptr [esp + 0x10]
            test eax, eax
            je L00448a82
            push 0x7fff
            push ebp
            call BrModelUpdate
            add esp, 8
    L00448a82:
            push offset WallLinearToPersp
            jmp L00448b5e
    L00448a8c:
            test eax, eax
            jne L00448b6c
            mov esi, dword ptr [gProgram_state+0x1bb4]
            mov dword ptr [esp + 0x14], esi
            mov eax, dword ptr [esi + 0x14]
            test eax, eax
            je L00448aae
            cmp byte ptr [eax], 0x26
            je L00448b6c
    L00448aae:
            cmp byte ptr [esi + 0x12], 1
            jne L00448b59
            mov ebp, dword ptr [esi + 0x18]
            xor ebx, ebx
            cmp ebp, ebx
            je L00448b59
            cmp word ptr [ebp + 0x12], bx
            mov dword ptr [esp + 0x10], ebx
            jbe L00448b43
    L00448acf:
            mov edx, dword ptr [ebp + 0xc]
            mov eax, ebx
            and eax, 0xffff
            lea esi, [eax + eax*4]
            shl esi, 3
            mov edi, dword ptr [esi + edx + 8]
            test edi, edi
            je L00448b38
            mov ecx, dword ptr [edi + 4]
            mov edx, offset gWall_texturing_tweakers+0x34
            call HasThisSuffix
            test eax, eax
            je L00448b07
            mov ecx, dword ptr [edi + 4]
            mov edx, offset gWall_texturing_tweakers+0x34
            call UnsuffixedMaterial
            jmp L00448b29
    L00448b07:
            mov ecx, dword ptr [edi + 4]
            mov edx, offset gWall_texturing_tweakers+0x3c
            call HasThisSuffix
            test eax, eax
            je L00448b27
            mov ecx, dword ptr [edi + 4]
            mov edx, offset gWall_texturing_tweakers+0x3c
            call UnsuffixedMaterial
            jmp L00448b29
    L00448b27:
            xor eax, eax
    L00448b29:
            test eax, eax
            je L00448b38
            mov ecx, dword ptr [ebp + 0xc]
            mov dword ptr [esp + 0x10], eax
            mov dword ptr [esi + ecx + 8], eax
    L00448b38:
            inc ebx
            cmp bx, word ptr [ebp + 0x12]
            jb L00448acf
            mov esi, dword ptr [esp + 0x14]
    L00448b43:
            mov eax, dword ptr [esp + 0x10]
            test eax, eax
            je L00448b59
            push 0x7fff
            push ebp
            call BrModelUpdate
            add esp, 8
    L00448b59:
            push offset WallUntexToPersp
    L00448b5e:
            push offset ProcessFaceMaterials
            push esi
            call BrActorEnum
            add esp, 0xc
    L00448b6c:
            mov eax, dword ptr [gRoad_texturing_level]
            xor ebx, ebx
            cmp eax, ebx
            jne L00448c2a
            mov esi, dword ptr [gProgram_state+0x1bb4]
            mov dword ptr [esp + 0x14], esi
            mov eax, dword ptr [esi + 0x14]
            cmp eax, ebx
            je L00448b95
            cmp byte ptr [eax], 0x26
            je L00448c2a
    L00448b95:
            cmp byte ptr [esi + 0x12], 1
            jne L00448c17
            mov ebp, dword ptr [esi + 0x18]
            cmp ebp, ebx
            je L00448c17
            cmp word ptr [ebp + 0x12], 0
            mov dword ptr [esp + 0x10], ebx
            jbe L00448c01
    L00448bad:
            mov edx, dword ptr [ebp + 0xc]
            mov eax, ebx
            and eax, 0xffff
            lea esi, [eax + eax*4]
            shl esi, 3
            mov edi, dword ptr [esi + edx + 8]
            test edi, edi
            je L00448bf6
            mov ecx, dword ptr [edi + 4]
            mov edx, offset gWall_texturing_tweakers+0x44
            call HasThisSuffix
            test eax, eax
            je L00448be5
            mov ecx, dword ptr [edi + 4]
            mov edx, offset gWall_texturing_tweakers+0x44
            call UnsuffixedMaterial
            jmp L00448be7
    L00448be5:
            xor eax, eax
    L00448be7:
            test eax, eax
            je L00448bf6
            mov ecx, dword ptr [ebp + 0xc]
            mov dword ptr [esp + 0x10], eax
            mov dword ptr [esi + ecx + 8], eax
    L00448bf6:
            inc ebx
            cmp bx, word ptr [ebp + 0x12]
            jb L00448bad
            mov esi, dword ptr [esp + 0x14]
    L00448c01:
            mov eax, dword ptr [esp + 0x10]
            test eax, eax
            je L00448c17
            push 0x7fff
            push ebp
            call BrModelUpdate
            add esp, 8
    L00448c17:
            push offset RoadUntexToPersp
            push offset ProcessFaceMaterials
            push esi
            call BrActorEnum
            add esp, 0xc
    L00448c2a:
            cmp dword ptr [gWall_texturing_level], 2
            jne L00448c40
            cmp dword ptr [gRoad_texturing_level], 1
            je L00448ccb
    L00448c40:
            mov esi, dword ptr [gProgram_state+0x1bb4]
            mov dword ptr [esp + 0x14], esi
            mov eax, dword ptr [esi + 0x14]
            test eax, eax
            je L00448c56
            cmp byte ptr [eax], 0x26
            je L00448ccb
    L00448c56:
            cmp byte ptr [esi + 0x12], 1
            jne L00448cb8
            mov edi, dword ptr [esi + 0x18]
            test edi, edi
            je L00448cb8
            xor ebx, ebx
            xor ebp, ebp
            cmp word ptr [edi + 0x12], bx
            jbe L00448ca6
    L00448c6d:
            mov edx, dword ptr [edi + 0xc]
            mov eax, ebx
            and eax, 0xffff
            lea esi, [eax + eax*4]
            shl esi, 3
            mov eax, dword ptr [esi + edx + 8]
            test eax, eax
            je L00448c9b
            mov edx, ebx
            mov ecx, edi
            call DisposeSuffixedMaterials
            test eax, eax
            je L00448c9b
            mov ecx, dword ptr [edi + 0xc]
            mov ebp, eax
            mov dword ptr [esi + ecx + 8], eax
    L00448c9b:
            inc ebx
            cmp bx, word ptr [edi + 0x12]
            jb L00448c6d
            mov esi, dword ptr [esp + 0x14]
    L00448ca6:
            test ebp, ebp
            je L00448cb8
            push 0x7fff
            push edi
            call BrModelUpdate
            add esp, 8
    L00448cb8:
            push offset DisposeSuffixedMaterials
            push offset ProcessFaceMaterials
            push esi
            call BrActorEnum
            add esp, 0xc
    L00448ccb:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 8
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00506e20
intptr_t C2_HOOK_CDECL RemoveBounds(br_actor* pActor, void* pArg) {
    if (pActor->type == BR_ACTOR_BOUNDS || pActor->type == BR_ACTOR_BOUNDS_CORRECT) {
        BrResFree(pActor->type_data);
        pActor->type_data = NULL;
    }
    return 0;
}

void C2_HOOK_FASTCALL RemoveBoundsStructures(br_actor* pActor) {

    DRActorEnumRecurse(pActor, RemoveBounds, NULL);
}

// FUNCTION: CARMA2_HW 0x00506c20
void C2_NAKED C2_HOOK_FASTCALL FreeTrack(tTrack_spec* pTrack_spec) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;

    DisposeSmashableEnvironment();
    if (gAdditional_actors != NULL) {
        BrActorRemove(gAdditional_actors);
        BrActorFree(gAdditional_actors);
    }
    PossibleService();
    DisposeTexturingMaterials();
    PossibleService();
    RemoveBoundsStructures(pTrack_spec->the_actor);
    PossibleService();
    DisposeColumns(pTrack_spec);
    PossibleService();
    DisposeFunkotronics(-2);
    PossibleService();
    ClearOutStorageSpace(&gTrack_storage_space);
    gProgram_state.current_depth_effect.sky_texture = NULL;
    PossibleService();
    DisposeGroovidelics(-2);
    PossibleService();
    DisposeOpponentPaths();
    PossibleService();
    DisposeKevStuff();
    PossibleService();
    BrActorRemove(pTrack_spec->the_actor);
    BrActorFree(pTrack_spec->the_actor);
    pTrack_spec->the_actor = NULL;
    gTrack_actor = NULL;
    if (gProgram_state.special_volume_count != 0) {
        BrMemFree(gProgram_state.special_volumes);
    }
    if (gProgram_state.special_screens_count != 0) {
        BrMemFree(gProgram_state.special_screens);
    }
    PossibleService();
    for (i = 0; i < 40; i++) {
 /* FIXME: magic number 40 */
        tNon_car_spec *non_car;

        non_car = &gProgram_state.non_cars[i];
        non_car->collision_info->shape = NULL;
        non_car->collision_info->physics_joint1 = NULL;
        non_car->collision_info->physics_joint2 = NULL;
    }
    for (i = 0; i < gProgram_state.num_non_car_spaces; i++) {
        tNon_car_spec* non_car;

        non_car = &gProgram_state.non_cars[i];
        PossibleService();
        /* FIXME: comparison with eDriver_non_car in dethrace */
        if (non_car->driver == eDriver_4 && non_car->actor != NULL) {
            if (non_car->actor->parent != NULL) {
                BrActorRemove(non_car->actor);
            }
            BrActorFree(non_car->actor);
        }
        DisposePhysicsObject(non_car->collision_info);
    }
    if (gProgram_state.non_cars != NULL) {
        BrMemFree(gProgram_state.non_cars);
        /* gProgram_state.non_cars = NULL; */
    }
    FreeSmokeShadeTables();
    if (gTrack_flic_buffer != NULL) {
        BrMapRemove(gProgram_state.default_depth_effect.sky_texture);
        BrMemFree(gTrack_flic_buffer);
        EndFlic(&gTrack_flic_descriptor);
        BrMemFree(gProgram_state.default_depth_effect.sky_texture->pixels);
        gProgram_state.default_depth_effect.sky_texture->pixels = NULL;
        BrPixelmapFree(gProgram_state.default_depth_effect.sky_texture);
        gTrack_flic_buffer = NULL;
    }
#else
    __asm {
            push ebx
            push ebp
            push esi
            push edi
            mov esi, ecx
            call DisposeSmashableEnvironment
            mov eax, dword ptr [gAdditional_actors]
            xor edi, edi
            cmp eax, edi
            je L00506c4d
            push eax
            call BrActorRemove
            mov eax, dword ptr [gAdditional_actors]
            add esp, 4
            push eax
            call BrActorFree
            add esp, 4
    L00506c4d:
            call PossibleService
            call DisposeTexturingMaterials
            call PossibleService
            mov ecx, dword ptr [esi + 0x14]
            mov edx, offset RemoveBounds
            push edi
            call DRActorEnumRecurse
            call PossibleService
            mov ecx, esi
            call DisposeColumns
            call PossibleService
            mov ecx, 0xfffffffe
            call DisposeFunkotronics
            call PossibleService
            mov ecx, offset gTrack_storage_space
            call ClearOutStorageSpace
            mov dword ptr [gProgram_state+0x1bf8], edi
            call PossibleService
            mov ecx, 0xfffffffe
            call DisposeGroovidelics
            call PossibleService
            call DisposeOpponentPaths
            call PossibleService
            call DisposeKevStuff
            call PossibleService
            mov ecx, dword ptr [esi + 0x14]
            push ecx
            call BrActorRemove
            mov edx, dword ptr [esi + 0x14]
            add esp, 4
            push edx
            call BrActorFree
            mov dword ptr [esi + 0x14], edi
            mov eax, dword ptr [gProgram_state+0x1bfc]
            add esp, 4
            cmp eax, edi
            mov dword ptr [gTrack_actor], edi
            je L00506cfa
            mov eax, dword ptr [gProgram_state+0x1c00]
            push eax
            call BrMemFree
            add esp, 4
    L00506cfa:
            cmp dword ptr [gProgram_state+0x1c18], edi
            je L00506d11
            mov ecx, dword ptr [gProgram_state+0x1c1c]
            push ecx
            call BrMemFree
            add esp, 4
    L00506d11:
            call PossibleService
            mov eax, dword ptr [gProgram_state+0x5ec8]
            mov ecx, 0x28
            add eax, 8
    L00506d23:
            mov edx, dword ptr [eax]
            add eax, 0x104
            dec ecx
            mov dword ptr [edx + 0x20], edi
            mov edx, dword ptr [eax - 0x104]
            mov dword ptr [edx + 0x188], edi
            mov edx, dword ptr [eax - 0x104]
            mov dword ptr [edx + 0x18c], edi
            jne L00506d23
            mov ecx, dword ptr [gProgram_state+0x5ecc]
            mov eax, dword ptr [gProgram_state+0x5ec8]
            xor ebx, ebx
            cmp ecx, edi
            jle L00506da7
            lea esi, [eax + 0x10]
            mov ebp, 4
    L00506d61:
            call PossibleService
            cmp dword ptr [esi - 4], ebp
            jne L00506d8a
            mov eax, dword ptr [esi]
            cmp eax, edi
            je L00506d8a
            cmp dword ptr [eax + 0xc], edi
            je L00506d7f
            push eax
            call BrActorRemove
            add esp, 4
    L00506d7f:
            mov eax, dword ptr [esi]
            push eax
            call BrActorFree
            add esp, 4
    L00506d8a:
            mov ecx, dword ptr [esi - 8]
            call DisposePhysicsObject
            mov eax, dword ptr [gProgram_state+0x5ecc]
            inc ebx
            add esi, 0x104
            cmp ebx, eax
            jl L00506d61
            mov eax, dword ptr [gProgram_state+0x5ec8]
    L00506da7:
            cmp eax, edi
            je L00506db4
            push eax
            call BrMemFree
            add esp, 4
    L00506db4:
            call FreeSmokeShadeTables
            cmp dword ptr [gTrack_flic_buffer], edi
            je L00506e17
            mov ecx, dword ptr [gProgram_state+0x1bdc]
            push ecx
            call BrMapRemove
            mov edx, dword ptr [gTrack_flic_buffer]
            add esp, 4
            push edx
            call BrMemFree
            add esp, 4
            mov ecx, offset gTrack_flic_descriptor
            call EndFlic
            mov eax, dword ptr [gProgram_state+0x1bdc]
            mov ecx, dword ptr [eax + 8]
            push ecx
            call BrMemFree
            mov edx, dword ptr [gProgram_state+0x1bdc]
            add esp, 4
            mov dword ptr [edx + 8], edi
            mov eax, dword ptr [gProgram_state+0x1bdc]
            push eax
            call BrPixelmapFree
            add esp, 4
            mov dword ptr [gTrack_flic_buffer], edi
    L00506e17:
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x005074d0
void C2_HOOK_FASTCALL MungeMaterialSV(br_matrix34* pMat, br_material* pMat_1, br_material* pMat_2, int pAxis_0, int pAxis_1) {

    pMat_1->map_transform.m[0][0] = 6.f * BrVector3Length((br_vector3*)pMat->m[pAxis_0]);
    pMat_1->map_transform.m[1][1] = 6.f * BrVector3Length((br_vector3*)pMat->m[pAxis_1]);
    BrMatrix23Copy(&pMat_2->map_transform, &pMat_1->map_transform);
}

// FUNCTION: CARMA2_HW 0x005072b0
br_uint_32 C2_HOOK_FASTCALL ApplyTransToModels(br_actor* pActor, br_matrix34* pMat, void* pData) {
    int i;
    br_vector3 temp_point;

    if (pActor->identifier != NULL && pActor->identifier[0] != '&') {
        if (pActor->model != NULL) {
            for (i = 0; i < pActor->model->nvertices; i++) {
                temp_point = pActor->model->vertices[i].p;
                BrMatrix34ApplyP(&pActor->model->vertices[i].p, &temp_point, pMat);
            }
            BrModelUpdate(pActor->model, BR_MODU_ALL);
        }
        BrMatrix34Identity(&pActor->t.t.mat);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00507210
br_uint_32 C2_HOOK_FASTCALL DeleteBastards(br_actor* pActor, br_matrix34* pMat, void* pData) {
    int i;
    int parent_already_doomed;

    if ((pActor != gAdditional_actors
            && (pActor->identifier == NULL || pActor->identifier[0] == '&')
            && Vector3EqualElements((br_vector3*)pMat->m[3], gZero_0058b1c8, gZero_0058b1c8, gZero_0058b1c8))
        || (pActor->model == NULL && pActor->type == BR_ACTOR_MODEL)) {
        parent_already_doomed = 0;
        for (i = 0; i < gDelete_count; i++) {
            if (gDelete_list[i] == pActor) {
                parent_already_doomed = 1;
                break;
            }
        }
        if (!parent_already_doomed) {
            gDelete_list[gDelete_count] = pActor;
            gDelete_count++;
        }
    }
    return 0;
}

void C2_HOOK_FASTCALL PutAdditionalNonCarsOnToAdditionalActors(void) {
    int i;

    for (i = 0; i < gNum_active_non_cars; i++) {
        tNon_car_spec *non_car = gActive_non_car_list[i];

        if (non_car->car_ID == -1 && non_car->actor->parent == gNon_track_actor) {
            BrActorRemove(non_car->actor);
            BrActorAdd(gAdditional_actors, non_car->actor);
        }
    }
}

void C2_HOOK_FASTCALL PutAdditionalNonCarsOnToNonTrackActor(void) {
    int i;

    for (i = 0; i < gNum_active_non_cars; i++) {
        tNon_car_spec *non_car = gActive_non_car_list[i];

        if (non_car->car_ID == -1 && non_car->actor->parent == gAdditional_actors) {
            BrActorRemove(non_car->actor);
            BrActorAdd(gNon_track_actor, non_car->actor);
        }
    }
}

void C2_HOOK_FASTCALL DeleteAnyZeroBastards(void) {
    int i;

    gDelete_count = 0;
    DRActorEnumRecurseWithTrans(gAdditional_actors, NULL, DeleteBastards, NULL);
    for (i = gDelete_count - 1; i >= 0; i--) {
        BrActorRemove(gDelete_list[i]);
    }
}

// FUNCTION: CARMA2_HW 0x00506f40
void C2_NAKED C2_HOOK_FASTCALL SaveAdditionalActors(void) {
#ifndef CARPOCALYPSE2_MATCHING

    PutAdditionalNonCarsOnToAdditionalActors();
    DeleteAnyZeroBastards();
    if (gLast_actor != NULL) {
        DRActorEnumRecurseWithTrans(gLast_actor, NULL, ApplyTransToModels, NULL);
    }
    if (strstr(gAdditional_actor_path, "autosave") != NULL) {
        BrActorSave(gAdditional_actor_path, gAdditional_actors);
        BrModelSaveMany(gAdditional_model_path, gAdditional_models, gNumber_of_additional_models);
    } else {
        tPath_name path1;
        tPath_name path2;
        char* str;
        int i;

        sprintf(path1, "%s", gAdditional_actor_path);
        str = strstr(path1, ".");
        if (str != NULL) {
            *str = '\0';
        }
        for (i = 0;; i++) {
            FILE* f;

            sprintf(path2, "%s%04d", path1, i);
            f = DRfopen(path2, "rb");
            if (f == NULL) {
                break;
            }
            PFfclose(f);
        }
        strcpy(path2, path1);
        sprintf(path1, "%s%04d.ACT", path2, i);
        BrActorSave(path1, gAdditional_actors);
        BrActorSave(gAdditional_actor_path, gAdditional_actors);
        sprintf(path2, "%s", gAdditional_model_path);
        str = strstr(path2, ".");
        if (str != NULL) {
            *str = '\0';
        }
        sprintf(path1, "%s%04d.DAT", path2, i);
        BrModelSaveMany(path1, gAdditional_models, gNumber_of_additional_models);
        BrModelSaveMany(gAdditional_model_path, gAdditional_models, gNumber_of_additional_models);
    }
    PutAdditionalNonCarsOnToNonTrackActor();
#else
    __asm {
            sub esp, 0x204
            mov eax, dword ptr [gNum_active_non_cars]
            push ebx
            push ebp
            push esi
            push edi
            xor edi, edi
            xor ebp, ebp
            cmp eax, edi
            mov dword ptr [esp + 0x10], edi
            jle L00506fa5
            mov ebx, offset gActive_non_car_list
    L00506f60:
            mov esi, dword ptr [ebx]
            cmp word ptr [esi + 0x80], 0xffff
            jne L00506f98
            mov eax, dword ptr [esi + 0x10]
            mov edx, dword ptr [gNon_track_actor]
            mov ecx, dword ptr [eax + 0xc]
            cmp ecx, edx
            jne L00506f98
            push eax
            call BrActorRemove
            mov edx, dword ptr [esi + 0x10]
            mov eax, dword ptr [gAdditional_actors]
            add esp, 4
            push edx
            push eax
            call BrActorAdd
            add esp, 8
    L00506f98:
            mov eax, dword ptr [gNum_active_non_cars]
            inc edi
            add ebx, 4
            cmp edi, eax
            jl L00506f60
    L00506fa5:
            mov ecx, dword ptr [gAdditional_actors]
            push 0
            push offset DeleteBastards
            xor edx, edx
            mov dword ptr [gDelete_count], 0
            call DRActorEnumRecurseWithTrans
            mov ecx, dword ptr [gDelete_count]
            lea eax, [ecx - 1]
            test eax, eax
            jl L00506feb
            lea esi, [eax*4 + gDelete_list]
            lea edi, [eax + 1]
    L00506fda:
            mov edx, dword ptr [esi]
            push edx
            call BrActorRemove
            add esp, 4
            sub esi, 4
            dec edi
            jne L00506fda
    L00506feb:
            mov ecx, dword ptr [gLast_actor]
            test ecx, ecx
            je L00507003
            push 0
            push offset ApplyTransToModels
            xor edx, edx
            call DRActorEnumRecurseWithTrans
    L00507003:
            push offset gDepth_effect_names+0x20c
            push offset gAdditional_actor_path
            call strstr
            add esp, 8
            test eax, eax
            je L00507039
            mov eax, dword ptr [gAdditional_actors]
            push eax
            push offset gAdditional_actor_path
            call BrActorSave
            mov cx, word ptr [gNumber_of_additional_models]
            add esp, 8
            push ecx
            jmp L0050718f
    L00507039:
            push offset gAdditional_actor_path
            lea edx, [esp + 0x18]
            push offset gCancel_recover+0xbc
            push edx
            call sprintf
            add esp, 0xc
            lea eax, [esp + 0x14]
            push offset gPed_flags_0058f2e4+0x178
            push eax
            call strstr
            add esp, 8
            test eax, eax
            je L00507069
            mov byte ptr [eax], 0
    L00507069:
            lea ecx, [esp + 0x14]
            push ebp
            push ecx
            lea edx, [esp + 0x11c]
            push offset gDepth_effect_names+0x200
            push edx
            call sprintf
            add esp, 0x10
            mov edx, offset gCancel_recover+0x1a4
            lea ecx, [esp + 0x114]
            call DRfopen
            test eax, eax
            je L005070a3
            mov ecx, eax
            call PFfclose
            inc ebp
            jmp L005070ab
    L005070a3:
            mov dword ptr [esp + 0x10], 1
    L005070ab:
            mov eax, dword ptr [esp + 0x10]
            test eax, eax
            je L00507069
            lea edi, [esp + 0x14]
            or ecx, 0xffffffff
            xor eax, eax
            lea edx, [esp + 0x114]
            repne scasb
            not ecx
            sub edi, ecx
            push ebp
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            lea edx, [esp + 0x18]
            shr ecx, 2
            rep movsd
            mov ecx, eax
            and ecx, 3
            rep movsb
            lea ecx, [esp + 0x118]
            push ecx
            push offset gDepth_effect_names+0x200
            push edx
            call sprintf
            mov eax, dword ptr [gAdditional_actors]
            add esp, 0x10
            lea ecx, [esp + 0x14]
            push eax
            push ecx
            call BrActorSave
            mov edx, dword ptr [gAdditional_actors]
            add esp, 8
            push edx
            push offset gAdditional_actor_path
            call BrActorSave
            add esp, 8
            lea eax, [esp + 0x114]
            push offset gAdditional_model_path
            push offset gCancel_recover+0xbc
            push eax
            call sprintf
            add esp, 0xc
            lea ecx, [esp + 0x114]
            push offset gPed_flags_0058f2e4+0x178
            push ecx
            call strstr
            add esp, 8
            test eax, eax
            je L00507153
            mov byte ptr [eax], 0
    L00507153:
            lea edx, [esp + 0x114]
            push ebp
            push edx
            lea eax, [esp + 0x1c]
            push offset gDepth_effect_names+0x1f4
            push eax
            call sprintf
            mov cx, word ptr [gNumber_of_additional_models]
            add esp, 0x10
            lea edx, [esp + 0x14]
            push ecx
            push offset gAdditional_models
            push edx
            call BrModelSaveMany
            mov ax, word ptr [gNumber_of_additional_models]
            add esp, 0xc
            push eax
    L0050718f:
            push offset gAdditional_models
            push offset gAdditional_model_path
            call BrModelSaveMany
            mov eax, dword ptr [gNum_active_non_cars]
            add esp, 0xc
            xor ebx, ebx
            test eax, eax
            jle L005071f6
            mov edi, offset gActive_non_car_list
    L005071b1:
            mov esi, dword ptr [edi]
            cmp word ptr [esi + 0x80], 0xffff
            jne L005071e9
            mov eax, dword ptr [esi + 0x10]
            mov edx, dword ptr [gAdditional_actors]
            mov ecx, dword ptr [eax + 0xc]
            cmp ecx, edx
            jne L005071e9
            push eax
            call BrActorRemove
            mov edx, dword ptr [esi + 0x10]
            mov eax, dword ptr [gNon_track_actor]
            add esp, 4
            push edx
            push eax
            call BrActorAdd
            add esp, 8
    L005071e9:
            mov eax, dword ptr [gNum_active_non_cars]
            inc ebx
            add edi, 4
            cmp ebx, eax
            jl L005071b1
    L005071f6:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x204
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
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004ffa20
void C2_HOOK_FASTCALL SaveSpecialVolumes(void) {
    tPath_name the_path;
    FILE* f;
    int i;
    tSpecial_volume* v;
    const char* screen_name;

    PathCat(the_path, gApplication_path, "SPECSAVE.TXT");
    f = DRfopen(the_path, "wt");
    if (f != NULL) {
        fprintf(f, "// SPECIAL EFFECTS VOLUMES\n\n");
        fprintf(f, "%d\t\t\t\t// # special effects volumes\n\n", gProgram_state.special_volume_count);
        for (i = 0, v = gProgram_state.special_volumes; i < gProgram_state.special_volume_count; i++, v++) {
            switch (v->boundary_type) {
            case eFx_boundary_box:
                fprintf(f, "BOX\n");
                fprintf(f, "%.3f, %.3f, %.3f\n", v->boundary.box.mat.m[0][0], v->boundary.box.mat.m[0][1], v->boundary.box.mat.m[0][2]);
                fprintf(f, "%.3f, %.3f, %.3f\n", v->boundary.box.mat.m[1][0], v->boundary.box.mat.m[1][1], v->boundary.box.mat.m[1][2]);
                fprintf(f, "%.3f, %.3f, %.3f\n", v->boundary.box.mat.m[2][0], v->boundary.box.mat.m[2][1], v->boundary.box.mat.m[2][2]);
                fprintf(f, "%.3f, %.3f, %.3f\n", v->boundary.box.mat.m[3][0], v->boundary.box.mat.m[3][1], v->boundary.box.mat.m[3][2]);
                break;
            case eFx_boundary_plane:
                fprintf(f, "%.3f, %.3f, %.3f, %.3f\n", v->boundary.plane.v[0], v->boundary.plane.v[1], v->boundary.plane.v[2], v->boundary.plane.v[3]);
                break;
            case eFx_boundary_default:
                fprintf(f, "DEFAULT\n");
                break;
            default:
                break;
            }
            fprintf(f, "%.2f\t\t\t\t// gravity multiplier\n", v->gravity_multiplier);
            fprintf(f, "%.2f\t\t\t\t// viscosity multiplier\n", v->viscosity_multiplier);
            fprintf(f, "%.2f\t\t\t\t// Car damage per millisecond\n", v->car_damage_per_ms);
            fprintf(f, "%.2f\t\t\t\t// Pedestrian damage per millisecond\n", v->ped_damage_per_ms);
            fprintf(f, "%d\t\t\t\t\t// camera effect index\n", v->camera_special_effect_index);
            fprintf(f, "%d\t\t\t\t\t// sky colour\n", v->sky_col);
            if (v->screen_pixelmap != NULL) {
                screen_name = v->screen_pixelmap->identifier;
            } else {
                screen_name = "NONE";
            }
            fprintf(f, "%s\t\t\t\t// Windscreen texture to use\n", screen_name);
            fprintf(f, "%d\t\t\t\t\t// Sound ID of entry noise\n", v->entry_noise);
            fprintf(f, "%d\t\t\t\t\t// Sound ID of exit noise\n", v->exit_noise);
            fprintf(f, "%d\t\t\t\t\t// Engine noise index\n", v->engine_noise_index);
            fprintf(f, "%d\t\t\t\t\t// material index\n", v->material_modifier_index);
            if (v->boundary_type == eFx_boundary_box || v->boundary_type == eFx_boundary_plane) {
                if (v->soundfx_type == kSoundFx_None) {
                    fprintf(f, "NONE\t\t\t\t\t// sound type\n");
                } else {
                    fprintf(f, "%s\t\t\t\t\t// sound type\n", gSoundType_choices[v->soundfx_type]);
                    WriteOutSoundSpec(f, &v->soundfx_data);
                }
            }
            fprintf(f, "\n");
        }
        PFfclose(f);
    }
}

void C2_HOOK_FASTCALL DoSaveAdditionalStuff(void) {

    if (gSpec_vol_mode == 0) {
        SaveAdditionalActors();
    } else {
        UpdateSpecVol();
        SaveSpecialVolumes();
    }
}

// FUNCTION: CARMA2_HW 0x00506e80
void C2_HOOK_FASTCALL AnimateSky(void) {
    // GLOBAL: CARMA2_HW 0x006aafc0
    static tU32 prev_sky_animation;
    tU32 now;
    tU32 now2;
    int frame_increment;

    now = GetTotalTime();

    if (gTrack_flic_buffer == NULL) {
        return;
    }
    now2 = now;
    if (now < prev_sky_animation) {
        now2 = 2 * prev_sky_animation - now;
    }
    if (prev_sky_animation == 0) {
        frame_increment = 1;
    } else {
        frame_increment = ((now2 - prev_sky_animation) / (gTrack_flic_descriptor.frame_period)) % (gTrack_flic_descriptor.current_frame + gTrack_flic_descriptor.frames_left);
    }

    for (; frame_increment != 0; frame_increment -= 1) {
        if (PlayNextFlicFrame(&gTrack_flic_descriptor)) {
            EndFlic(&gTrack_flic_descriptor);
            StartFlic(gTrack_flic_descriptor.file_name, -1, &gTrack_flic_descriptor,
                gTrack_flic_buffer_size, (tS8*)gTrack_flic_buffer, NULL, 0, 0, 0);
            AssertFlicPixelmap(&gTrack_flic_descriptor, gProgram_state.default_depth_effect.sky_texture);
        }
        prev_sky_animation = now;
        SkyTextureChanged();
    }
}

static __inline int C2_HOOK_FASTCALL CalculateTextureBitsValue(tFunk_texturebits* pTexturebits) {
    int i;
    int value;

    value = 0;
    for (i = 0; i < pTexturebits->count; i++) {
        if (pTexturebits->car->field_0x18cc & (1 << pTexturebits->bits[i])) {
            value += 1 << i;
        }
    }
    return value;
}

#define GROOVE_SAW(T, PERIOD) fmod((T), PERIOD) / (PERIOD)

#define GROOVE_PATH_SIN_COS(TRIG, TIME, PERIOD, AMPLITUDE) \
    ((PERIOD) != 0.0f \
            ? (br_scalar)(TRIG(BrAngleToRadian(BrDegreeToAngle( \
                              GROOVE_SAW(TIME, PERIOD) * 360.0f))) \
                  * (AMPLITUDE)) \
            : 0.0f)

#define GROOVE_PATH_FLOAT(MODE, TIME, PERIOD, TEXTUREBITS, AMPLITUDE) \
    ((MODE) == eMove_continuous \
            ? ((PERIOD) != 0.0f \
                      ? GROOVE_SAW(TIME, PERIOD) * (AMPLITUDE) \
                      : 0.0) \
            : ((MODE) == eMove_controlled \
                      ? PERIOD * AMPLITUDE \
                      : ((MODE) == eMove_absolute \
                                ? (PERIOD) \
                                : ((MODE) == eMove_linear \
                                          ? ((PERIOD) != 0.0f \
                                                    ? MapSawToTriangle((float)(GROOVE_SAW(TIME, PERIOD))) \
                                                        * (AMPLITUDE) \
                                                    : 0.0f) \
                                          : ((MODE) == eMove_flash \
                                                    ? (fmod((TIME), (PERIOD)) * 2.0 > (PERIOD) \
                                                              ? -(AMPLITUDE) \
                                                              : (AMPLITUDE)) \
                                                    : ((MODE) == eMove_texturebits \
                                                              ? CalculateTextureBitsValue(TEXTUREBITS) \
                                                              : GROOVE_PATH_SIN_COS(sinf, TIME, PERIOD, AMPLITUDE)))))))

#define GROOVE_ANGLE(MODE, TIME, PERIOD, TEXTUREBITS) \
    BrDegreeToAngle(((MODE) == eMove_continuous \
            ? ((PERIOD) != 0.0f ? GROOVE_SAW(TIME, PERIOD) * 360.0 : 0.0) \
            : ((MODE) == eMove_controlled \
                      ? (PERIOD) * 360.0 \
                      : ((MODE) == eMove_absolute \
                                ? (PERIOD) \
                                : ((MODE) == eMove_linear \
                                          ? ((PERIOD) != 0.0f ? MapSawToTriangle((float)(GROOVE_SAW(TIME, PERIOD))) * 360.0 : 0.0) \
                                          : ((MODE) == eMove_flash \
                                                    ? (fmod((TIME), (PERIOD)) * 2.0 > (PERIOD) ? -360.0 : 360.0) \
                                                    : ((MODE) == eMove_texturebits \
                                                              ? CalculateTextureBitsValue(TEXTUREBITS) \
                                                              : ((PERIOD) != 0.0f ? sin(BrAngleToRadian(BrDegreeToAngle(GROOVE_SAW(TIME, PERIOD) * 360.0))) * 360.0 : 0.0))))))))

#define GROOVE_FLOAT(MODE, TIME, PERIOD, TEXTUREBITS, AMPLITUDE) \
    ((MODE) == eMove_continuous \
            ? ((PERIOD) != 0.0f ? GROOVE_SAW(TIME, PERIOD) * (AMPLITUDE) : 0.0) \
            : ((MODE) == eMove_controlled \
                      ? (AMPLITUDE) * (PERIOD) \
                      : ((MODE) == eMove_absolute \
                                ? (PERIOD) \
                                : ((MODE) == eMove_linear \
                                          ? ((PERIOD) != 0.0f ? MapSawToTriangle((float)(GROOVE_SAW(TIME, PERIOD))) * (AMPLITUDE) : 0.0f) \
                                          : ((MODE) == eMove_flash \
                                                    ? (fmod((TIME), (PERIOD)) * 2.0 > (PERIOD) ? -(AMPLITUDE) : (AMPLITUDE)) \
                                                    : ((MODE) == eMove_texturebits \
                                                              ? CalculateTextureBitsValue(TEXTUREBITS) \
                                                              : ((PERIOD) != 0.0f ? BR_SIN(BrDegreeToAngle(GROOVE_SAW(TIME, PERIOD) * 360.0)) * (AMPLITUDE) : 0.0f)))))))

// FUNCTION: CARMA2_HW 0x00478e30
void C2_HOOK_FASTCALL PathGrooveBastard(tGroovidelic_spec* pGroove, tU32 pTime, br_matrix34* pMat, int pInterrupt_it) {
    br_scalar pos;

#ifndef CARPOCALYPSE2_MATCHING
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, path_interrupt_status, 0x20);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, path_resumption_value, 0x24);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, path_data.straight_info.period.value, 0x28);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, path_data.straight_info.x_delta, 0x2c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, path_data.straight_info.y_delta, 0x30);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, path_data.straight_info.z_delta, 0x34);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, path_data.straight_info.centre, 0x38);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, path_data.circular_info.period.value, 0x28);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, path_data.circular_info.centre, 0x30);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_position, 0x50);
#endif

    switch (pGroove->path_type) {
    case eGroove_path_straight:
        if (pGroove->path_data.straight_info.x_delta != 0.0f) {
            pos = pGroove->path_data.straight_info.centre.v[0]
                + (br_scalar)(GROOVE_PATH_FLOAT(pGroove->path_mode, pTime,
                    pGroove->path_data.straight_info.period.value, pGroove->path_data.straight_info.texture_info.data,
                    pGroove->path_data.straight_info.x_delta));
            if (pInterrupt_it) {
                pGroove->path_resumption_value = pos;
                if (pos < pMat->m[3][0]) {
                    pGroove->path_interrupt_status = eInterrupt_less_than;
                } else {
                    pGroove->path_interrupt_status = eInterrupt_greater_than;
                }
                pGroove->actor->t.t.translate.t = pGroove->path_data.straight_info.centre;
                pMat->m[3][0] = pos;
            } else if (pGroove->path_interrupt_status == eInterrupt_none) {
                pGroove->actor->t.t.translate.t = pGroove->path_data.straight_info.centre;
                pMat->m[3][0] = pos;
            } else if (pGroove->path_interrupt_status == eInterrupt_less_than) {
                if (pGroove->path_resumption_value > pos) {
                    pGroove->path_interrupt_status = eInterrupt_none;
                    pGroove->actor->t.t.translate.t = pGroove->path_data.straight_info.centre;
                    pMat->m[3][0] = pos;
                }
            } else if (pGroove->path_resumption_value < pos) {
                pGroove->path_interrupt_status = eInterrupt_none;
                pGroove->actor->t.t.translate.t = pGroove->path_data.straight_info.centre;
                pMat->m[3][0] = pos;
            }
        }

        if (pGroove->path_data.straight_info.y_delta != 0.0f) {
            pos = pGroove->path_data.straight_info.centre.v[1]
                + (br_scalar)(GROOVE_PATH_FLOAT(pGroove->path_mode, pTime,
                    pGroove->path_data.straight_info.period.value, pGroove->path_data.straight_info.texture_info.data,
                    pGroove->path_data.straight_info.y_delta));
            if (pInterrupt_it) {
                pGroove->path_resumption_value = pos;
                if (pos < pMat->m[3][1]) {
                    pGroove->path_interrupt_status = eInterrupt_less_than;
                } else {
                    pGroove->path_interrupt_status = eInterrupt_greater_than;
                }
                pGroove->actor->t.t.translate.t = pGroove->path_data.straight_info.centre;
                pMat->m[3][1] = pos;
            } else if (pGroove->path_interrupt_status == eInterrupt_none) {
                pGroove->actor->t.t.translate.t = pGroove->path_data.straight_info.centre;
                pMat->m[3][1] = pos;
            } else if (pGroove->path_interrupt_status == eInterrupt_less_than) {
                if (pGroove->path_resumption_value > pos) {
                    pGroove->path_interrupt_status = eInterrupt_none;
                    pGroove->actor->t.t.translate.t = pGroove->path_data.straight_info.centre;
                    pMat->m[3][1] = pos;
                }
            } else if (pGroove->path_resumption_value < pos) {
                pGroove->path_interrupt_status = eInterrupt_none;
                pGroove->actor->t.t.translate.t = pGroove->path_data.straight_info.centre;
                pMat->m[3][1] = pos;
            }
        }

        if (pGroove->path_data.straight_info.z_delta != 0.0f) {
            pos = pGroove->path_data.straight_info.centre.v[2]
                + (br_scalar)(GROOVE_PATH_FLOAT(pGroove->path_mode, pTime,
                    pGroove->path_data.straight_info.period.value, pGroove->path_data.straight_info.texture_info.data,
                    pGroove->path_data.straight_info.z_delta));
            if (pInterrupt_it) {
                pGroove->path_resumption_value = pos;
                if (pos < pMat->m[3][2]) {
                    pGroove->path_interrupt_status = eInterrupt_less_than;
                } else {
                    pGroove->path_interrupt_status = eInterrupt_greater_than;
                }
                pGroove->actor->t.t.translate.t = pGroove->path_data.straight_info.centre;
                pMat->m[3][2] = pos;
            } else if (pGroove->path_interrupt_status == eInterrupt_none) {
                pGroove->actor->t.t.translate.t = pGroove->path_data.straight_info.centre;
                pMat->m[3][2] = pos;
            } else if (pGroove->path_interrupt_status == eInterrupt_less_than) {
                if (pGroove->path_resumption_value > pos) {
                    pGroove->path_interrupt_status = eInterrupt_none;
                    pGroove->actor->t.t.translate.t = pGroove->path_data.straight_info.centre;
                    pMat->m[3][2] = pos;
                }
            } else if (pGroove->path_resumption_value < pos) {
                pGroove->path_interrupt_status = eInterrupt_none;
                pGroove->actor->t.t.translate.t = pGroove->path_data.straight_info.centre;
                pMat->m[3][2] = pos;
            }
        }
        pGroove->object_position = pGroove->actor->t.t.translate.t;
        break;
    case eGroove_path_circular:
        pGroove->actor->t.t.translate.t = pGroove->path_data.circular_info.centre;
        if (pGroove->path_data.circular_info.axis == eAxis_y) {
            pMat->m[3][0] = pGroove->path_data.circular_info.centre.v[0]
                + GROOVE_PATH_SIN_COS(cosf, pTime,
                    pGroove->path_data.circular_info.period.value,
                    pGroove->path_data.circular_info.radius);
        } else if (pGroove->path_data.circular_info.axis == eAxis_z) {
            pMat->m[3][0] = pGroove->path_data.circular_info.centre.v[0]
                + GROOVE_PATH_SIN_COS(sinf, pTime,
                    pGroove->path_data.circular_info.period.value,
                    pGroove->path_data.circular_info.radius);
        }

        if (pGroove->path_data.circular_info.axis == eAxis_x) {
            pMat->m[3][1] = pGroove->path_data.circular_info.centre.v[1]
                + GROOVE_PATH_SIN_COS(sinf, pTime,
                    pGroove->path_data.circular_info.period.value,
                    pGroove->path_data.circular_info.radius);
        } else if (pGroove->path_data.circular_info.axis == eAxis_z) {
            pMat->m[3][1] = pGroove->path_data.circular_info.centre.v[1]
                + GROOVE_PATH_SIN_COS(cosf, pTime,
                    pGroove->path_data.circular_info.period.value,
                    pGroove->path_data.circular_info.radius);
        }

        if (pGroove->path_data.circular_info.axis == eAxis_x) {
            pMat->m[3][2] = pGroove->path_data.circular_info.centre.v[2]
                + GROOVE_PATH_SIN_COS(cosf, pTime,
                    pGroove->path_data.circular_info.period.value,
                    pGroove->path_data.circular_info.radius);
        } else if (pGroove->path_data.circular_info.axis == eAxis_y) {
            pMat->m[3][2] = pGroove->path_data.circular_info.centre.v[2]
                + GROOVE_PATH_SIN_COS(sinf, pTime,
                    pGroove->path_data.circular_info.period.value,
                    pGroove->path_data.circular_info.radius);
        }
        pGroove->object_position = pGroove->actor->t.t.translate.t;
        break;
    default:
        break;
    }
}


// FUNCTION: CARMA2_HW 0x00479890
void C2_HOOK_FASTCALL ObjectGrooveBastard(tGroovidelic_spec* pGroove, tU32 pTime, br_matrix34* pMat, int pInterrupt_it) {
    int rock_it;
    br_scalar x_size;
    br_scalar y_size;
    br_scalar z_size;
    br_scalar pos;
    br_bounds* bounds;

#ifndef CARPOCALYPSE2_MATCHING
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_type, 0x5c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_interrupt_status, 0x64);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_resumption_value, 0x68);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_data.spin_info.period, 0x6c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_data.spin_info.axis, 0x70);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_data.rock_info.max_angle, 0x70);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_data.rock_info.current_angle, 0x74);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_data.throb_info.x_period.value, 0x6c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_data.throb_info.y_period.value, 0x70);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_data.throb_info.z_period.value, 0x74);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_data.throb_info.x_magnitude, 0x78);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_data.throb_info.y_magnitude, 0x7c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_data.throb_info.z_magnitude, 0x80);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_data.shear_info.x_period.value, 0x6c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_data.shear_info.y_period.value, 0x70);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_data.shear_info.z_period.value, 0x74);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_data.shear_info.x_magnitude, 0x78);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_data.shear_info.y_magnitude, 0x7c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_data.shear_info.z_magnitude, 0x80);
#endif

    switch (pGroove->object_type) {
    case eGroove_object_spin:
        switch (pGroove->object_data.spin_info.axis) {
        case eAxis_x:
            DRMatrix34PostRotateX(pMat, GROOVE_ANGLE(pGroove->object_mode, pTime, pGroove->object_data.spin_info.period.value, pGroove->object_data.spin_info.texture_info.data));
            break;
        case eAxis_y:
            DRMatrix34PostRotateY(pMat, GROOVE_ANGLE(pGroove->object_mode, pTime, pGroove->object_data.spin_info.period.value, pGroove->object_data.spin_info.texture_info.data));
            break;
        case eAxis_z:
            DRMatrix34PostRotateZ(pMat, GROOVE_ANGLE(pGroove->object_mode, pTime, pGroove->object_data.spin_info.period.value, pGroove->object_data.spin_info.texture_info.data));
            break;
        }
        break;
    case eGroove_object_rock:
        rock_it = 1;
        pos = (br_scalar)(GROOVE_FLOAT(pGroove->object_mode, pTime,
            pGroove->object_data.rock_info.period.value, pGroove->object_data.rock_info.texture_info.data, pGroove->object_data.rock_info.max_angle));

        if (pInterrupt_it) {
            pGroove->object_resumption_value = pos;
            if (pGroove->object_data.rock_info.current_angle > pos) {
                pGroove->object_interrupt_status = eInterrupt_less_than;
            } else {
                pGroove->object_interrupt_status = eInterrupt_greater_than;
            }
        } else if (pGroove->object_interrupt_status == eInterrupt_less_than) {
            if (pGroove->object_resumption_value > pos && !gAction_replay_mode) {
                pGroove->object_interrupt_status = eInterrupt_none;
            } else {
                rock_it = 0;
            }
        } else if (pGroove->object_interrupt_status == eInterrupt_greater_than) {
            if (pGroove->object_resumption_value < pos && !gAction_replay_mode) {
                pGroove->object_interrupt_status = eInterrupt_none;
            } else {
                rock_it = 0;
            }
        }
        if (rock_it) {
            pGroove->object_data.rock_info.current_angle = pos;
        }

        switch (pGroove->object_data.rock_info.axis) {
        case eAxis_x:
            DRMatrix34PostRotateX(pMat, BrDegreeToAngle(pGroove->object_data.rock_info.current_angle));
            break;
        case eAxis_y:
            DRMatrix34PostRotateY(pMat, BrDegreeToAngle(pGroove->object_data.rock_info.current_angle));
            break;
        case eAxis_z:
            DRMatrix34PostRotateZ(pMat, BrDegreeToAngle(pGroove->object_data.rock_info.current_angle));
            break;
        }
        break;
    case eGroove_object_throb:
        BrMatrix34PostScale(pMat,
            (br_scalar)(GROOVE_FLOAT(pGroove->object_mode, pTime,
                    pGroove->object_data.throb_info.x_period.value, pGroove->object_data.throb_info.x_texture_info.data, pGroove->object_data.throb_info.x_magnitude)
                + 1.0),
            (br_scalar)(GROOVE_FLOAT(pGroove->object_mode, pTime,
                    pGroove->object_data.throb_info.y_period.value, pGroove->object_data.throb_info.y_texture_info.data, pGroove->object_data.throb_info.y_magnitude)
                + 1.0),
            (br_scalar)(GROOVE_FLOAT(pGroove->object_mode, pTime,
                    pGroove->object_data.throb_info.z_period.value, pGroove->object_data.throb_info.z_texture_info.data, pGroove->object_data.throb_info.z_magnitude)
                + 1.0));
        /* FALLTHROUGH */
    case eGroove_object_shear:
        bounds = &pGroove->actor->model->bounds;
        x_size = bounds->max.v[0] - bounds->min.v[0];
        y_size = bounds->max.v[1] - bounds->min.v[1];
        z_size = bounds->max.v[2] - bounds->min.v[2];
        if (pGroove->object_data.shear_info.x_magnitude == 0.0f) {
            BrMatrix34PostShearX(pMat,
                (br_scalar)(GROOVE_FLOAT(pGroove->object_mode, pTime,
                    pGroove->object_data.shear_info.y_period.value, pGroove->object_data.shear_info.y_texture_info.data, pGroove->object_data.shear_info.y_magnitude)
                    / y_size),
                (br_scalar)(GROOVE_FLOAT(pGroove->object_mode, pTime,
                    pGroove->object_data.shear_info.z_period.value, pGroove->object_data.shear_info.z_texture_info.data, pGroove->object_data.shear_info.z_magnitude)
                    / z_size));
        } else if (pGroove->object_data.shear_info.y_magnitude == 0.0f) {
            BrMatrix34PostShearY(pMat,
                (br_scalar)(GROOVE_FLOAT(pGroove->object_mode, pTime,
                    pGroove->object_data.shear_info.x_period.value, pGroove->object_data.shear_info.x_texture_info.data, pGroove->object_data.shear_info.x_magnitude)
                    / x_size),
                (br_scalar)(GROOVE_FLOAT(pGroove->object_mode, pTime,
                    pGroove->object_data.shear_info.z_period.value, pGroove->object_data.shear_info.z_texture_info.data, pGroove->object_data.shear_info.z_magnitude)
                    / z_size));
        } else {
            BrMatrix34PostShearZ(pMat,
                (br_scalar)(GROOVE_FLOAT(pGroove->object_mode, pTime,
                    pGroove->object_data.shear_info.x_period.value, pGroove->object_data.shear_info.x_texture_info.data, pGroove->object_data.shear_info.x_magnitude)
                    / x_size),
                (br_scalar)(GROOVE_FLOAT(pGroove->object_mode, pTime,
                    pGroove->object_data.shear_info.y_period.value, pGroove->object_data.shear_info.y_texture_info.data, pGroove->object_data.shear_info.y_magnitude)
                    / y_size));
        }
        break;
    default:
        break;
    }
}


// FUNCTION: CARMA2_HW 0x004e4650
void C2_NAKED C2_HOOK_FASTCALL LollipopizeActor(br_actor* pSubject_actor, br_matrix34* ref_to_world, tLollipop_mode pWhich_axis) {
#ifndef CARPOCALYPSE2_MATCHING
    br_vector3 ref_to_subject;
    br_vector3 fixed_axis;
    br_vector3 vector_a;
    br_vector3 vector_b;
    br_matrix34 subject_to_world;
    br_matrix34 mat;

    BrActorToActorMatrix34(&subject_to_world, pSubject_actor, gNon_track_actor);
    BrVector3Sub(&ref_to_subject, (br_vector3*)ref_to_world->m[3], (br_vector3*)subject_to_world.m[3]);
    switch (pWhich_axis) {
    case eLollipop_x_match:
        BrVector3SetFloat(&vector_a, 1.f, 0.f, 0.f);
        break;
    case eLollipop_y_match:
        BrVector3SetFloat(&vector_a, 0.f, 1.f, 0.f);
        break;
    case eLollipop_z_match:
        BrVector3SetFloat(&vector_a, 0.f, 0.f, 1.f);
        break;
    case eLollipop_none:
        abort();
        break;
    }
    BrVector3Cross(&vector_b, &ref_to_subject, &vector_a);
    BrVector3Normalise(&vector_b, &vector_b);

    BrVector3Cross(&fixed_axis, &vector_a, &vector_b);

    switch (pWhich_axis) {
    case eLollipop_none:
        break;
    case eLollipop_x_match:
        mat.m[0][0] = vector_a.v[0];
        mat.m[1][0] = vector_a.v[1];
        mat.m[2][0] = vector_a.v[2];
        mat.m[0][1] = vector_b.v[0];
        mat.m[1][1] = vector_b.v[1];
        mat.m[2][1] = vector_b.v[2];
        mat.m[0][2] = fixed_axis.v[0];
        mat.m[1][2] = fixed_axis.v[1];
        mat.m[2][2] = fixed_axis.v[2];
        mat.m[3][0] = 0.f;
        mat.m[3][1] = 0.f;
        mat.m[3][2] = 0.f;
        break;
    case eLollipop_y_match:
        mat.m[0][0] = vector_b.v[0];
        mat.m[1][0] = vector_b.v[1];
        mat.m[2][0] = vector_b.v[2];
        mat.m[0][1] = vector_a.v[0];
        mat.m[1][1] = vector_a.v[1];
        mat.m[2][1] = vector_a.v[2];
        mat.m[0][2] = fixed_axis.v[0];
        mat.m[1][2] = fixed_axis.v[1];
        mat.m[2][2] = fixed_axis.v[2];
        mat.m[3][0] = 0.f;
        mat.m[3][1] = 0.f;
        mat.m[3][2] = 0.f;
        break;
    case eLollipop_z_match:
        mat.m[0][0] = vector_b.v[0];
        mat.m[1][0] = vector_b.v[1];
        mat.m[2][0] = vector_b.v[2];
        mat.m[0][1] = fixed_axis.v[0];
        mat.m[1][1] = fixed_axis.v[1];
        mat.m[2][1] = fixed_axis.v[2];
        mat.m[0][2] = vector_a.v[0];
        mat.m[1][2] = vector_a.v[1];
        mat.m[2][2] = vector_a.v[2];
        mat.m[3][0] = 0.f;
        mat.m[3][1] = 0.f;
        mat.m[3][2] = 0.f;
        break;
    }
    BrMatrix34Pre(&pSubject_actor->t.t.mat, &mat);
#else
    __asm {
            sub esp, 0x84
            mov eax, dword ptr [gNon_track_actor]
            push esi
            push edi
            mov edi, ecx
            push eax
            lea ecx, [esp + 0x60]
            push edi
            mov esi, edx
            push ecx
            call BrActorToActorMatrix34
            fld dword ptr [esi + 0x24]
            fsub dword ptr [esp + 0x8c]
            add esp, 0xc
            fstp dword ptr [esp + 0x14]
            fld dword ptr [esi + 0x28]
            fsub dword ptr [esp + 0x84]
            fstp dword ptr [esp + 0x18]
            fld dword ptr [esi + 0x2c]
            fsub dword ptr [esp + 0x88]
            mov esi, dword ptr [esp + 0x90]
            mov eax, esi
            sub eax, 0
            fstp dword ptr [esp + 0x1c]
            je L004e46ce
            dec eax
            je L004e46be
            dec eax
            jne L004e46e4
            push 0x3f800000
            push 0
            lea edx, [esp + 0x10]
            push 0
            push edx
            jmp L004e46dc
    L004e46be:
            push 0
            push 0x3f800000
            lea eax, [esp + 0x10]
            push 0
            push eax
            jmp L004e46dc
    L004e46ce:
            push 0
            push 0
            lea ecx, [esp + 0x10]
            push 0x3f800000
            push ecx
    L004e46dc:
            call BrVector3SetFloat
            add esp, 0x10
    L004e46e4:
            fld dword ptr [esp + 0x10]
            fld dword ptr [esp + 0xc]
            fld dword ptr [esp + 8]
            fld dword ptr [esp + 0x10]
            fxch st(3)
            fmul dword ptr [esp + 0x18]
            fld dword ptr [esp + 0xc]
            fxch st(3)
            fmul dword ptr [esp + 0x1c]
            fld dword ptr [esp + 8]
            fxch st(1)
            fsubp st(2), st(0)
            fxch st(2)
            fmul dword ptr [esp + 0x1c]
            fld st(1)
            fmul st(0), st(2)
            fxch st(5)
            fmul dword ptr [esp + 0x14]
            fxch st(4)
            fmul dword ptr [esp + 0x14]
            fxch st(3)
            fmul dword ptr [esp + 0x18]
            fxch st(4)
            fsubp st(1), st(0)
            fld st(0)
            fxch st(4)
            fsubp st(3), st(0)
            fxch st(3)
            fmul st(0), st(3)
            fld st(2)
            fmul st(0), st(3)
            fxch st(1)
            faddp st(5), st(0)
            faddp st(4), st(0)
            fxch st(3)
            fsqrt
            fcom dword ptr [gConst_kangeroo_force_factor+0xd0]
            fnstsw ax
            test ah, 0x41
            jne L004e4775
            fdivr qword ptr [gConst_kangeroo_force_factor+0xd8]
            fld st(0)
            fld st(1)
            fxch st(1)
            fmulp st(5), st(0)
            fmul st(0), st(3)
            fxch st(1)
            fmul st(0), st(2)
            fxch st(1)
            fstp dword ptr [esp + 0x24]
            fstp dword ptr [esp + 0x28]
            fstp st(0)
            fstp st(0)
            jmp L004e4793
    L004e4775:
            fstp st(0)
            fstp st(0)
            fstp st(0)
            fstp st(0)
            fld dword ptr [gConst_kangeroo_force_factor+0xe0]
            mov dword ptr [esp + 0x24], 0
            mov dword ptr [esp + 0x28], 0
    L004e4793:
            fld dword ptr [esp + 0x28]
            fld dword ptr [esp + 0x24]
            fld st(2)
            fld dword ptr [esp + 0x28]
            fld dword ptr [esp + 0x24]
            fld st(5)
            fxch st(5)
            fmul dword ptr [esp + 0xc]
            fxch st(4)
            fmul dword ptr [esp + 0x10]
            mov eax, esi
            fsubp st(4), st(0)
            fxch st(2)
            fmul dword ptr [esp + 0x10]
            fxch st(3)
            fstp dword ptr [esp + 0x14]
            fmul dword ptr [esp + 8]
            fxch st(1)
            fmul dword ptr [esp + 8]
            fxch st(3)
            fmul dword ptr [esp + 0xc]
            fxch st(1)
            fsubp st(2), st(0)
            sub eax, 0
            fsubp st(2), st(0)
            fstp dword ptr [esp + 0x18]
            fstp dword ptr [esp + 0x1c]
            je L004e4883
            dec eax
            je L004e483d
            dec eax
            jne L004e48e1
            mov edx, dword ptr [esp + 0x24]
            mov eax, dword ptr [esp + 0x28]
            mov ecx, dword ptr [esp + 0x14]
            mov dword ptr [esp + 0x38], edx
            mov edx, dword ptr [esp + 0x18]
            mov dword ptr [esp + 0x44], eax
            mov eax, dword ptr [esp + 0x1c]
            mov dword ptr [esp + 0x30], ecx
            mov ecx, dword ptr [esp + 8]
            mov dword ptr [esp + 0x3c], edx
            mov edx, dword ptr [esp + 0xc]
            mov dword ptr [esp + 0x48], eax
            mov eax, dword ptr [esp + 0x10]
            mov dword ptr [esp + 0x34], ecx
            fstp dword ptr [esp + 0x2c]
            mov dword ptr [esp + 0x40], edx
            mov dword ptr [esp + 0x4c], eax
            jmp L004e48c7
    L004e483d:
            mov ecx, dword ptr [esp + 0x24]
            mov edx, dword ptr [esp + 0x28]
            mov eax, dword ptr [esp + 8]
            mov dword ptr [esp + 0x38], ecx
            mov ecx, dword ptr [esp + 0xc]
            mov dword ptr [esp + 0x44], edx
            mov edx, dword ptr [esp + 0x10]
            mov dword ptr [esp + 0x30], eax
            mov eax, dword ptr [esp + 0x14]
            mov dword ptr [esp + 0x3c], ecx
            mov ecx, dword ptr [esp + 0x18]
            mov dword ptr [esp + 0x48], edx
            mov edx, dword ptr [esp + 0x1c]
            mov dword ptr [esp + 0x34], eax
            fstp dword ptr [esp + 0x2c]
            mov dword ptr [esp + 0x40], ecx
            mov dword ptr [esp + 0x4c], edx
            jmp L004e48c7
    L004e4883:
            mov eax, dword ptr [esp + 8]
            mov ecx, dword ptr [esp + 0xc]
            mov edx, dword ptr [esp + 0x10]
            mov dword ptr [esp + 0x2c], eax
            mov eax, dword ptr [esp + 0x24]
            mov dword ptr [esp + 0x38], ecx
            mov ecx, dword ptr [esp + 0x28]
            mov dword ptr [esp + 0x44], edx
            mov edx, dword ptr [esp + 0x14]
            mov dword ptr [esp + 0x3c], eax
            mov eax, dword ptr [esp + 0x18]
            mov dword ptr [esp + 0x48], ecx
            mov ecx, dword ptr [esp + 0x1c]
            mov dword ptr [esp + 0x34], edx
            fstp dword ptr [esp + 0x30]
            mov dword ptr [esp + 0x40], eax
            mov dword ptr [esp + 0x4c], ecx
    L004e48c7:
            mov dword ptr [esp + 0x50], 0
            mov dword ptr [esp + 0x54], 0
            mov dword ptr [esp + 0x58], 0
            jmp L004e48e3
    L004e48e1:
            fstp st(0)
    L004e48e3:
            lea edx, [esp + 0x2c]
            add edi, 0x2c
            push edx
            push edi
            call BrMatrix34Pre
            add esp, 8
            pop edi
            pop esi
            add esp, 0x84
            ret 4
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00478c80
void C2_HOOK_FASTCALL GrooveThisDelic(tGroovidelic_spec* pGroove, tU32 pTime, int pInterrupt_it) {
    br_actor* the_actor;
    br_vector3 actor_pos;
    br_matrix34* the_mat;
    tInterrupt_status old_path_interrupt;
    tInterrupt_status old_object_interrupt;

    old_path_interrupt = pGroove->path_interrupt_status;
    old_object_interrupt = pGroove->object_interrupt_status;
    the_actor = pGroove->actor;
    pGroove->done_this_frame = 1;
    CalcActorGlobalPos(&actor_pos, the_actor);
    if (((int (C2_HOOK_FASTCALL*)(const br_vector3*, br_scalar))PointOutOfSight)(&actor_pos, pGroove->mode == eGroove_mode_distance ? gYon_squared : 36.f)) {
        return;
    }

    the_mat = &the_actor->t.t.mat;
    if ((!gAction_replay_mode
            || !ARReplayIsReallyPaused()
            || pGroove->path_mode == eMove_controlled
            || pGroove->path_mode == eMove_absolute)
        && !(pGroove->block_flags & 0x1)) {
        PathGrooveBastard(pGroove, pTime, the_mat, pInterrupt_it);
    }
    if ((pGroove->object_type != eGroove_object_none || pGroove->lollipop_mode != eLollipop_none)
        && (!gAction_replay_mode
            || !ARReplayIsReallyPaused()
            || pGroove->object_mode == eMove_controlled
            || pGroove->object_mode == eMove_absolute)
        && !(pGroove->block_flags & 0x2)) {
        the_mat->m[0][0] = 1.0f;
        the_mat->m[0][1] = 0.0f;
        the_mat->m[0][2] = 0.0f;
        the_mat->m[1][0] = 0.0f;
        the_mat->m[1][1] = 1.0f;
        the_mat->m[1][2] = 0.0f;
        the_mat->m[2][0] = 0.0f;
        the_mat->m[2][1] = 0.0f;
        the_mat->m[2][2] = 1.0f;
        the_mat->m[3][0] = -pGroove->object_centre.v[0];
        the_mat->m[3][1] = -pGroove->object_centre.v[1];
        the_mat->m[3][2] = -pGroove->object_centre.v[2];
        ObjectGrooveBastard(pGroove, pTime, the_mat, pInterrupt_it);
        the_mat->m[3][0] = pGroove->object_position.v[0] + the_mat->m[3][0] + pGroove->object_centre.v[0];
        the_mat->m[3][1] = pGroove->object_position.v[1] + the_mat->m[3][1] + pGroove->object_centre.v[1];
        the_mat->m[3][2] = pGroove->object_position.v[2] + the_mat->m[3][2] + pGroove->object_centre.v[2];
        if (pGroove->lollipop_mode != eLollipop_none) {
            LollipopizeActor(pGroove->actor, &gCamera_to_world, pGroove->lollipop_mode);
        }
    }
    if (old_path_interrupt != pGroove->path_interrupt_status || old_object_interrupt != pGroove->object_interrupt_status) {
        PipeSingleGrooveStop(
            pGroove - gGroovidelics_array,
            the_mat,
            pGroove->path_interrupt_status,
            pGroove->object_interrupt_status,
            pGroove->path_resumption_value,
            pGroove->object_resumption_value);
    }
}

// FUNCTION: CARMA2_HW 0x00478c00
void C2_HOOK_FASTCALL GrooveThoseDelics(void) {
    tGroovidelic_spec* the_groove;
    float f_the_time;
    int i;

    if (gGroovidelics_array != NULL) {
        f_the_time = (float)GetTotalTime();
        gPrevious_groove_times[1] = gPrevious_groove_times[0];
        gPrevious_groove_times[0] = (tU32)f_the_time;
        for (i = 0, the_groove = gGroovidelics_array; i < gGroovidelics_array_size; i++, the_groove++) {
            if (the_groove->owner != -999 && !the_groove->done_this_frame) {
                GrooveThisDelic(the_groove, (tU32)f_the_time, 0);
            }
        }
    }
}

float C2_HOOK_FASTCALL MapSawToTriangle(float pNumber) {

    if (pNumber < 0.5) {
        return pNumber * 4.0f - 1.0f;
    } else {
        return 3.0f - pNumber * 4.0f;
    }
}

br_scalar C2_HOOK_FASTCALL NormaliseDegreeAngle(br_scalar pAngle) {

    while (pAngle < .0f) {
        pAngle += 360.f;
    }
    return pAngle;
}

// FUNCTION: CARMA2_HW 0x00477230
void C2_HOOK_FASTCALL FunkThoseTronics(void) {
    int i;
    int j;
    int iteration_count;
    int finished;
    tFunkotronic_spec* the_funk;
    br_vector3* the_proximity;
    tS32 the_time;
    br_matrix23* mat_matrix;
    br_material* the_material;
    float f_the_time;
    float rot_amount;
    float f_time_diff;
    br_vector2 tmp_v2;

    if (gFunkotronics_array == NULL) {
        return;
    }
    DontLetFlicFuckWithPalettes();
    the_time = GetTotalTime();
    f_the_time = (float)the_time;
    for (i = 0; i < gFunkotronics_array_size; i++) {
        the_funk = &gFunkotronics_array[i];
        if (the_funk->owner == -999) {
            continue;
        }
        if (the_funk->flags != 0) {
            continue;
        }
        j = 0;
        if (the_funk->mode == eFunk_mode_distance && the_funk->proximity_array != NULL) {
            for (j = 0; j < the_funk->proximity_count; j++) {
                int k;
                for (k = 0; k < 3; k++) {
                    the_proximity = &the_funk->proximity_array[j].v[k];
                    if (Vector3DistanceSquared(the_proximity, gOur_pos) <= gSight_distance_squared) {
                        br_vector3 tmp;

                        BrVector3Sub(&tmp, the_proximity, (br_vector3*)gCamera_to_world.m[3]);
                        if (BrVector3Dot(&tmp, &the_funk->proximity_array[j].n) < 0.f && BrVector3Dot(&tmp, (br_vector3*)gCamera_to_world.m[2]) < 0.f) {
                            j = -999;
                            break;
                        }
                    }
                }
                if (j < 0) {
                    break;
                }
            }
            if (j >= 0) {
                continue;
            }
        } else if (the_funk->mode == eFunk_mode_last_lap_only && gLap < gTotal_laps) {
            continue;
        } else if (the_funk->mode == eFunk_mode_all_laps_but_last && gLap >= gTotal_laps) {
            continue;
        }

        the_material = the_funk->material;
        mat_matrix = &the_material->map_transform;
        if (!gAction_replay_mode || !ARReplayIsReallyPaused() || the_funk->matrix_mode == eMove_controlled || the_funk->matrix_mode == eMove_absolute) {
            switch (the_funk->matrix_mod_type) {
            case eMatrix_mod_spin:
                BrMatrix23Identity(mat_matrix);
                the_material->map_transform.m[2][0] -= .5f;
                the_material->map_transform.m[2][1] -= .5f;
                if (the_funk->matrix_mod_data.spin_info.period > 0.f) {
                    f_time_diff = 1.f - fmodf(the_funk->matrix_mod_data.spin_info.period, 1.f);
                } else {
                    f_time_diff = fmodf(-the_funk->matrix_mod_data.spin_info.period, 1.f);
                }

                MOVE_FUNK_PARAMETER(rot_amount, the_funk->matrix_mode, f_time_diff, the_funk->matrix_mod_data.texture_info.data, 65536.f, -65536.f);
                DRMatrix23PostRotate(mat_matrix, (br_angle)rot_amount);

                the_material->map_transform.m[2][0] += .5f;
                the_material->map_transform.m[2][1] += .5f;
                break;
            case eMatrix_mod_rock:
                BrMatrix23Identity(mat_matrix);
                the_material->map_transform.m[2][0] -= the_funk->matrix_mod_data.rock_info.x_centre;
                the_material->map_transform.m[2][1] -= the_funk->matrix_mod_data.rock_info.y_centre;

                MOVE_FUNK_PARAMETER(rot_amount,
                    the_funk->matrix_mode,
                    the_funk->matrix_mod_data.rock_info.period,
                    the_funk->matrix_mod_data.texture_info.data,
                    the_funk->matrix_mod_data.rock_info.rock_angle,
                    the_funk->matrix_mod_data.rock_info.rock_angle);
                DRMatrix23PostRotate(mat_matrix, BrDegreeToAngle(NormaliseDegreeAngle(rot_amount)));

                the_material->map_transform.m[2][0] += the_funk->matrix_mod_data.rock_info.x_centre;
                the_material->map_transform.m[2][1] += the_funk->matrix_mod_data.rock_info.y_centre;
                break;
            case eMatrix_mod_throb:
                BrMatrix23Identity(mat_matrix);
                the_material->map_transform.m[2][0] -= the_funk->matrix_mod_data.throb_info.x_centre;
                the_material->map_transform.m[2][1] -= the_funk->matrix_mod_data.throb_info.y_centre;

                MOVE_FUNK_PARAMETER(tmp_v2.v[1],
                    the_funk->matrix_mode,
                    the_funk->matrix_mod_data.throb_info.y_period,
                    the_funk->matrix_mod_data.texture_info.data,
                    the_funk->matrix_mod_data.throb_info.y_magnitude,
                    the_funk->matrix_mod_data.throb_info.y_magnitude);
                MOVE_FUNK_PARAMETER(tmp_v2.v[0],
                    the_funk->matrix_mode,
                    the_funk->matrix_mod_data.throb_info.x_period,
                    the_funk->matrix_mod_data.texture_info.data,
                    the_funk->matrix_mod_data.throb_info.x_magnitude,
                    the_funk->matrix_mod_data.throb_info.x_magnitude);
                BrMatrix23PostScale(mat_matrix, tmp_v2.v[0] + 1.f, tmp_v2.v[1] + 1.f);

                the_material->map_transform.m[2][0] += the_funk->matrix_mod_data.throb_info.x_centre;
                the_material->map_transform.m[2][1] += the_funk->matrix_mod_data.throb_info.y_centre;
                break;
            case eMatrix_mod_slither:
                MOVE_FUNK_PARAMETER(tmp_v2.v[0],
                    the_funk->matrix_mode,
                    the_funk->matrix_mod_data.slither_info.x_period,
                    the_funk->matrix_mod_data.texture_info.data,
                    the_funk->matrix_mod_data.slither_info.x_magnitude,
                    the_funk->matrix_mod_data.slither_info.x_magnitude);
                the_material->map_transform.m[2][0] = tmp_v2.v[0];
                MOVE_FUNK_PARAMETER(tmp_v2.v[1],
                    the_funk->matrix_mode,
                    the_funk->matrix_mod_data.slither_info.y_period,
                    the_funk->matrix_mod_data.texture_info.data,
                    the_funk->matrix_mod_data.slither_info.y_magnitude,
                    the_funk->matrix_mod_data.slither_info.y_magnitude);
                the_material->map_transform.m[2][1] = tmp_v2.v[1];
                break;
            case eMatrix_mod_roll:
                MOVE_FUNK_PARAMETER(tmp_v2.v[0], the_funk->matrix_mode,
                    the_funk->matrix_mod_data.roll_info.x_period,
                    the_funk->matrix_mod_data.texture_info.data,
                    1.f,
                    -1.f);
                the_material->map_transform.m[2][0] = tmp_v2.v[0];
                MOVE_FUNK_PARAMETER(tmp_v2.v[1], the_funk->matrix_mode,
                    the_funk->matrix_mod_data.roll_info.y_period,
                    the_funk->matrix_mod_data.texture_info.data,
                    1.f,
                    -1.f);
                the_material->map_transform.m[2][1] = tmp_v2.v[1];
                break;
            case eMatrix_mod_none:
                break;
            }
            if (the_funk->matrix_mod_type != eMatrix_mod_none) {
                BrMaterialUpdate(the_funk->material, BR_MATU_MAP_TRANSFORM);
            }
        }
        if (the_funk->lighting_animation_type != eMove_none) {
            MOVE_FUNK_PARAMETER(the_material->ka, the_funk->lighting_animation_type,
                the_funk->lighting_animation_data.controlled.period,
                the_funk->lighting_animation_data.texture_info.data,
                the_funk->ambient_delta, -the_funk->ambient_delta);
            the_material->ka += the_funk->ambient_base;

            MOVE_FUNK_PARAMETER(the_material->kd, the_funk->lighting_animation_type,
                the_funk->lighting_animation_data.controlled.period,
                the_funk->lighting_animation_data.texture_info.data,
                the_funk->direct_delta,
                -the_funk->direct_delta);
            the_material->kd += the_funk->direct_base;

            MOVE_FUNK_PARAMETER(the_material->ks, the_funk->lighting_animation_type,
                the_funk->lighting_animation_data.controlled.period,
                the_funk->lighting_animation_data.texture_info.data,
                the_funk->specular_delta,
                -the_funk->specular_delta);
            the_material->ks += the_funk->specular_base;
        }
        switch (the_funk->texture_animation_type) {
        case eTexture_animation_none:
            break;
        case eTexture_animation_frames:
            if (!gAction_replay_mode || !ARReplayIsReallyPaused() ||
                the_funk->mode == eFunk_mode_all_laps_but_last || the_funk->mode == 4) {
                br_uint_16 update_flags;
                br_pixelmap* new_colour_map;

                if (the_funk->time_mode == eTime_mode_accurate) {
                    MOVE_FUNK_PARAMETER(rot_amount, the_funk->texture_animation_data.frames_info.mode,
                        the_funk->texture_animation_data.frames_info.controlled.period,
                        the_funk->texture_animation_data.frames_info.texture_info.data,
                        (br_scalar)the_funk->texture_animation_data.frames_info.texture_count,
                        (br_scalar)-the_funk->texture_animation_data.frames_info.texture_count);
                    the_funk->texture_animation_data.frames_info.current_frame = (int) rot_amount;
                } else if (f_the_time - the_funk->last_frame >= the_funk->texture_animation_data.frames_info.controlled.period) {
                    the_funk->last_frame = f_the_time;
                    the_funk->texture_animation_data.frames_info.current_frame += 1;
                    if (the_funk->texture_animation_data.frames_info.current_frame >=
                        the_funk->texture_animation_data.frames_info.texture_count) {
                        the_funk->texture_animation_data.frames_info.current_frame = 0;
                    }
                }
                update_flags = 0;
                assert(the_funk->texture_animation_data.frames_info.current_frame >= 0);
                assert(the_funk->texture_animation_data.frames_info.current_frame < CARPOCALYPSE2_ASIZE(the_funk->texture_animation_data.frames_info.textures));
                new_colour_map = the_funk->texture_animation_data.frames_info.textures[the_funk->texture_animation_data.frames_info.current_frame];
                if (the_material->colour_map != new_colour_map) {
                    the_material->colour_map = new_colour_map;
                    the_material->user = new_colour_map;
                    update_flags |= BR_MATU_COLOURMAP;
                }
                the_material->colour_map = the_funk->texture_animation_data.frames_info.textures[the_funk->texture_animation_data.frames_info.current_frame];
                if (the_material->colour_map != new_colour_map) {
                    BrMaterialUpdate(the_funk->material, BR_MATU_COLOURMAP);
                }
                if (the_funk->texture_animation_data.frames_info.has_matrix) {
                    br_matrix23 *frame_mat = &the_funk->texture_animation_data.frames_info.mat[the_funk->texture_animation_data.frames_info.current_frame];
                    if (the_material->map_transform.m[0][0] != frame_mat->m[0][0]
                        || the_material->map_transform.m[1][1] != frame_mat->m[1][1]
                        || the_material->map_transform.m[2][0] != frame_mat->m[2][0]
                        || the_material->map_transform.m[2][1] != frame_mat->m[2][1]) {

                        BrMatrix23Copy(&the_material->map_transform, frame_mat);
                        update_flags |= BR_MATU_MAP_TRANSFORM;
                    }
                }
                if (update_flags != 0) {
                    BrMaterialUpdate(the_material, update_flags);
                }
            }
            break;
        case eTexture_animation_flic:
            if (!gAction_replay_mode || !ARReplayIsReallyPaused()) {
                tS32 i_last_frame = (tS32)the_funk->last_frame;
                tS32 i_time_diff;
                if (the_time < i_last_frame) {
                    i_time_diff = 2 * i_last_frame - the_time;
                } else {
                    i_time_diff = the_time;
                }
                if (the_funk->time_mode == eTime_mode_accurate) {
                    if (the_funk->last_frame) {
                        iteration_count = (i_time_diff - i_last_frame) /
                                          the_funk->texture_animation_data.flic_info.flic_descriptor.frame_period;
                    } else {
                        iteration_count = 1;
                    }
                } else {
                    if ((br_scalar)(i_time_diff - the_funk->last_frame) >= the_funk->texture_animation_data.flic_info.flic_descriptor.frame_period) {
                        iteration_count = 1;
                    } else {
                        iteration_count = 0;
                    }
                }
                for (j = 0; j < iteration_count; j++) {
                    finished = PlayNextFlicFrame(&the_funk->texture_animation_data.flic_info.flic_descriptor);
                    BrMapUpdate(the_funk->material->colour_map, BR_MAPU_ALL);
                    BrMaterialUpdate(the_funk->material, BR_MATU_COLOURMAP);
                    if (finished) {
                        EndFlic(&the_funk->texture_animation_data.flic_info.flic_descriptor);
                        StartFlic(
                                the_funk->texture_animation_data.flic_info.flic_descriptor.file_name,
                                -1,
                                &the_funk->texture_animation_data.flic_info.flic_descriptor,
                                the_funk->texture_animation_data.flic_info.flic_data_length,
                                (tS8 *) the_funk->texture_animation_data.flic_info.flic_data,
                                the_material->colour_map, 0, 0, 0);
                    }
                    the_funk->last_frame = f_the_time;
                }
            }
            break;
        case eTexture_animation_camera:
            if (!gAction_replay_mode || !ARReplayIsReallyPaused()) {
                br_vector3 camera_look;
                br_actor *camera_actor;
                if (the_funk->texture_animation_data.camera_info.count == 1) {
                    camera_actor = the_funk->texture_animation_data.camera_info.actors[0];
#ifdef CARPOCALYPSE2_FIX_BUGS
                    BrVector3Sub(&camera_look, &gProgram_state.current_car.pos,
                                 &camera_actor->t.t.translate.t);
#endif
                } else {
                    float closest_distance = (float) 0xffffffffu;
                    camera_actor = NULL;
                    for (i = 0; i < the_funk->texture_animation_data.camera_info.count; i++) {
                        br_vector3 delta;
                        br_actor *act = the_funk->texture_animation_data.camera_info.actors[i];
                        float dist;
                        BrVector3Sub(&delta, &gProgram_state.current_car.pos, &act->t.t.translate.t);
                        dist = BrVector3LengthSquared(&delta);
                        if (dist < closest_distance) {
                            closest_distance = dist;
                            BrVector3Copy(&camera_look, &delta);
                            camera_actor = act;
                        }
                    }
                }
                if (camera_actor != NULL) {
                    if (the_funk->texture_animation_data.camera_info.mode == 1) {
                        camera_actor->t.type = BR_TRANSFORM_LOOK_UP;
                        BrVector3Copy(&camera_actor->t.t.look_up.look, &camera_look);
                        BrVector3Set(&camera_actor->t.t.look_up.up, 0.f, 1.f, 0.f);
                    }
                    AddExtraRender(camera_actor, the_material);
                }
            }
            break;
        case eTexture_animation_mirror:
            if (!gAction_replay_mode || !ARReplayIsReallyPaused()) {
                if (the_funk->texture_animation_data.mirror_info.actor != NULL) {
                    BrMatrix34Mul(&the_funk->texture_animation_data.mirror_info.actor->t.t.mat,
                                  &gCamera_to_world,
                                  &the_funk->texture_animation_data.mirror_info.mat);
                    AddExtraRender(the_funk->texture_animation_data.mirror_info.actor, the_material);
                }
            }
            break;
        }
    }
    LetFlicFuckWithPalettes();
}

// FUNCTION: CARMA2_HW 0x0047b1e0
void C2_HOOK_FASTCALL ResetGrooveFlags(void) {
    int i;
    tGroovidelic_spec* the_groove;

    i = 0;
    if (gGroovidelics_array_size > 0) {
        the_groove = gGroovidelics_array;
        do {
            the_groove->done_this_frame = 0;
            i++;
            the_groove++;
        } while (i < gGroovidelics_array_size);
    }
}

// FUNCTION: CARMA2_HW 0x0047b210
intptr_t C2_HOOK_CDECL DelGrooveRef(br_actor* pActor, void* pArg) {
    tGroovidelic_spec* the_groove;
    int i;

    for (i = 0, the_groove = gGroovidelics_array; i < gGroovidelics_array_size; i++, the_groove++) {
        if (pActor == the_groove->actor) {
            the_groove->owner = -999;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0047b250
void C2_HOOK_FASTCALL DisableFunkotronic(int pIndex) {
    gFunkotronics_array[pIndex].flags |= 0x1;
    PipeSingleFunkEnable(pIndex, 0);
}

// FUNCTION: CARMA2_HW 0x0047b280
void C2_HOOK_FASTCALL EnableFunkotronic(int pIndex) {
    gFunkotronics_array[pIndex].flags &= ~0x1;
    PipeSingleFunkEnable(pIndex, 1);
}

// FUNCTION: CARMA2_HW 0x0047b0e0
void C2_HOOK_FASTCALL StopGroovidelic(br_actor* pActor) {
    tGroovidelic_spec* the_groove;
    int count;
    int i;

    the_groove = gGroovidelics_array;
    count = gGroovidelics_array_size;
    for (i = 0; i < count; i++, the_groove++) {
        if (the_groove->actor == pActor) {
            if (the_groove->path_interrupt_status == eInterrupt_none
                && the_groove->object_interrupt_status == eInterrupt_none) {
                GrooveThisDelic(the_groove, gPrevious_groove_times[1], 1);
            }
            return;
        }
    }
}

// FUNCTION: CARMA2_HW 0x0047b360
tGroovidelic_spec* C2_HOOK_FASTCALL ActorsGroove(br_actor* pActor) {
    tGroovidelic_spec* groove;
    int count;
    int i;

    groove = gGroovidelics_array;
    count = gGroovidelics_array_size;
    for (i = 0; i < count; i++, groove++) {
        if (groove->actor == pActor) {
            return groove;
        }
    }
    return NULL;
}

// FUNCTION: CARMA2_HW 0x0047b390
void C2_NAKED C2_HOOK_FASTCALL FUN_0047b390(br_device_pixelmap* pPixelmap, int pWidth, int pHeight, int pMask) {
#ifndef CARPOCALYPSE2_MATCHING
    br_uint_16* pixels;
    int row_stride;
    int y;
    int x;

    row_stride = pPixelmap->pm_row_bytes / 2;
    pixels = (br_uint_16*)pPixelmap->pm_pixels;
    y = pHeight;
    if (y > 0) do {
        x = pWidth;
        if (x > 0) do {
            br_uint_16 pixel = *pixels++;

            if (pixel != 0) {
                *pixels = pixel | pMask;
            }
            pixels++;
        } while (--x != 0);
        pixels += row_stride;
    } while (--y != 0);
#else
    __asm {
            movsx eax, word ptr [ecx + 0x28]
            push ebx
            mov ebx, edx
            cdq
            push esi
            mov esi, dword ptr [ecx + 8]
            mov ecx, dword ptr [esp + 0xc]
            sub eax, edx
            sar eax, 1
            test ecx, ecx
            jle L0047b3d4
            mov edx, dword ptr [esp + 0x10]
            push edi
            push ebp
            mov ebp, ecx
    L0047b3b0:
            test ebx, ebx
            jle L0047b3cc
            mov edi, ebx
    L0047b3b6:
            mov cx, word ptr [esi]
            add esi, 2
            test cx, cx
            je L0047b3c6
            or ecx, edx
            mov word ptr [esi], cx
    L0047b3c6:
            add esi, 2
            dec edi
            jne L0047b3b6
    L0047b3cc:
            dec ebp
            lea esi, [esi + eax*2]
            jne L0047b3b0
            pop ebp
            pop edi
    L0047b3d4:
            pop esi
            pop ebx
            ret 8
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

// FUNCTION: CARMA2_HW 0x0047b160
void C2_HOOK_FASTCALL EnableGroovidelic(br_actor* pActor, tU32 pBlock_flags) {
    tGroovidelic_spec* the_groove;
    int i;

    the_groove = gGroovidelics_array;
    for (i = 0; i < gGroovidelics_array_size; i++, the_groove++) {
        if (the_groove->actor == pActor) {
            the_groove->block_flags &= ~pBlock_flags;
            return;
        }
    }
}

// FUNCTION: CARMA2_HW 0x004ff120
C2_NAKED tSpecial_volume* C2_HOOK_FASTCALL FindSpecialVolume(br_vector3* pP, tSpecial_volume* pLast_vol, int pArg3) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;
    tSpecial_volume* result;

    result = NULL;
    for (i = 0; i < gProgram_state.special_volume_count && (result == NULL || pArg3); i++) {
        tSpecial_volume* v = &gProgram_state.special_volumes[i];
        tSpecial_volume* sound_volume = NULL;
        tSpecial_volume* containing_volume = sound_volume;
        float sound_dist = 1e7f;
        br_vector3 sound_dir;
        int fartest_bnds_axis;

#ifdef CARPOCALYPSE2_FIX_BUGS
        fartest_bnds_axis = 0;
#endif

        if (v->boundary_type == eFx_boundary_box) {
            if (pP->v[0] > v->boundary.box.bounds.min.v[0]
                    && pP->v[0] < v->boundary.box.bounds.max.v[0]
                    && pP->v[1] > v->boundary.box.bounds.min.v[1]
                    && pP->v[1] < v->boundary.box.bounds.max.v[1]
                    && pP->v[2] > v->boundary.box.bounds.min.v[2]
                    && pP->v[2] < v->boundary.box.bounds.max.v[2]) {
                br_vector3 pnt_norm;
                float fartest_dist;
                int j;

                BrMatrix34ApplyP(&pnt_norm, pP, &v->boundary.box.inv_mat);
                fartest_dist = 0.f;
                for (j = 0; j < 3; j++) {
                    float dist;
                    int bnds_axis;
                    if (pnt_norm.v[j] <= -1.f) {
                        dist = -1.f - pnt_norm.v[j];
                        bnds_axis = j + 3;
                    } else if (pnt_norm.v[j] > 1.f) {
                        dist = pnt_norm.v[j] - 1.f;
                        bnds_axis = j;
                    } else {
                        dist = 0.f;
                    }
                    if (dist > fartest_dist) {
                        fartest_bnds_axis = bnds_axis;
                        fartest_dist = dist;
                    }
                }
                if (fartest_dist != 0.f && v->soundfx_type != kSoundFx_None) {
                    int axis = fartest_bnds_axis % 3;

                    fartest_dist /= v->boundary.box.axis_length.v[axis];
                    if (fartest_dist < 1e7f) {
                        float delta = (float)(fartest_bnds_axis < 3 ? 1 : -1);
                        BrVector3Scale(&sound_dir, (br_vector3*)v->boundary.box.mat.m[axis], delta);
                        sound_volume = v;
                        sound_dist = fartest_dist;
                    }
                } else if (fartest_dist == 0.f) {
                    containing_volume = v;
                }
            }
        } else if (v->boundary_type == eFx_boundary_plane) {
            float dist;

            dist = DistanceFromFaceND(pP, (br_vector3*)&v->boundary.plane, v->boundary.box.mat.m[1][0]);
            containing_volume = v;
            if (dist >= 0.f) {
                containing_volume = NULL;
                if (dist < 1e7f && v->soundfx_type != kSoundFx_None) {
                    BrVector3Copy(&sound_dir, &v->boundary.plane);
                    sound_volume = v;
                    sound_dist = dist;
                }
            }
        }
        if (containing_volume != NULL) {
            sound_dist = 0.f;
            sound_volume = containing_volume;
            if (result == NULL) {
                result = containing_volume;
            }
        }
        if (pArg3 && sound_volume != NULL && sound_dist < 10.f) {
            DoEnvSound(sound_volume, pP, sound_volume->soundfx_type, &sound_volume->soundfx_data, sound_dist, &sound_dir);
        }
    }
    return result;
#else
    __asm {
            sub esp, 0x38
            mov edx, dword ptr [gProgram_state+0x1bfc]
            push ebx
            xor eax, eax
            push ebp
            push esi
            mov esi, dword ptr [gProgram_state+0x1c00]
            cmp edx, eax
            push edi
            mov dword ptr [esp + 0x10], ecx
            mov dword ptr [esp + 0x1c], eax
            mov dword ptr [esp + 0x2c], eax
            jle L004ff3e1
            lea edi, [esi + 4]
    L004ff14c:
            mov eax, dword ptr [esp + 0x1c]
            test eax, eax
            je L004ff160
            mov eax, dword ptr [esp + 0x4c]
            test eax, eax
            je L004ff3e1
    L004ff160:
            mov eax, dword ptr [esi]
            xor ebx, ebx
            xor ebp, ebp
            sub eax, ebx
            mov dword ptr [esp + 0x18], 0x4b189680
            je L004ff1e1
            dec eax
            jne L004ff362
            mov eax, dword ptr [edi + 0xc]
            mov edx, edi
            push eax
            call DistanceFromFaceND
            fst dword ptr [esp + 0x20]
            fcomp dword ptr [gFLOAT_0058b130+0x14]
            fnstsw ax
            test ah, 1
            je L004ff19c
            mov ebp, esi
            jmp L004ff35e
    L004ff19c:
            fld dword ptr [esp + 0x20]
            fcomp dword ptr [gFLOAT_0058b130+0x10]
            fnstsw ax
            test ah, 1
            je L004ff35e
            cmp dword ptr [edi + 0xb0], -1
            je L004ff35e
            mov edx, dword ptr [edi]
            mov ecx, dword ptr [esp + 0x20]
            mov dword ptr [esp + 0x30], edx
            mov eax, dword ptr [edi + 4]
            mov dword ptr [esp + 0x18], ecx
            mov dword ptr [esp + 0x34], eax
            mov ecx, dword ptr [edi + 8]
            mov ebx, esi
            mov dword ptr [esp + 0x38], ecx
            jmp L004ff35e
    L004ff1e1:
            _emit 0xd9
        _emit 0x01
            fcomp dword ptr [edi + 0x6c]
            fnstsw ax
            test ah, 0x41
            jne L004ff362
            _emit 0xd9
        _emit 0x01
            fcomp dword ptr [edi + 0x78]
            fnstsw ax
            test ah, 1
            je L004ff362
            fld dword ptr [ecx + 8]
            fcomp dword ptr [edi + 0x74]
            fnstsw ax
            test ah, 0x41
            jne L004ff362
            fld dword ptr [ecx + 8]
            fcomp dword ptr [edi + 0x80]
            fnstsw ax
            test ah, 1
            je L004ff362
            fld dword ptr [ecx + 4]
            fcomp dword ptr [edi + 0x70]
            fnstsw ax
            test ah, 0x41
            jne L004ff362
            fld dword ptr [ecx + 4]
            fcomp dword ptr [edi + 0x7c]
            fnstsw ax
            test ah, 1
            je L004ff362
            lea edx, [edi + 0x30]
            lea eax, [esp + 0x3c]
            push edx
            push ecx
            push eax
            call BrMatrix34ApplyP
            add esp, 0xc
            mov dword ptr [esp + 0x14], 0
            xor ecx, ecx
            lea edx, [esp + 0x3c]
    L004ff268:
            _emit 0xd9
        _emit 0x02
            fcomp dword ptr [gFLOAT_0058b130+0x18]
            fnstsw ax
            test ah, 1
            je L004ff288
            fld dword ptr [gFLOAT_0058b130+0x18]
            _emit 0xd8
        _emit 0x22
            lea eax, [ecx + 3]
            mov dword ptr [esp + 0x24], eax
            jmp L004ff2ab
    L004ff288:
            _emit 0xd9
        _emit 0x02
            fcomp dword ptr [gFLOAT_0058b130+0x1c]
            fnstsw ax
            test ah, 0x41
            jne L004ff2a5
            _emit 0xd9
        _emit 0x02
            fsub dword ptr [gFLOAT_0058b130+0x1c]
            mov dword ptr [esp + 0x24], ecx
            jmp L004ff2ab
    L004ff2a5:
            fld dword ptr [gFLOAT_0058b130+0x14]
    L004ff2ab:
            fcom dword ptr [esp + 0x14]
            fnstsw ax
            test ah, 0x41
            jne L004ff2c4
            mov eax, dword ptr [esp + 0x24]
            fstp dword ptr [esp + 0x14]
            mov dword ptr [esp + 0x28], eax
            jmp L004ff2c6
    L004ff2c4:
            fstp st(0)
    L004ff2c6:
            inc ecx
            add edx, 4
            cmp ecx, 3
            jl L004ff268
            fld dword ptr [esp + 0x14]
            fcomp dword ptr [gFLOAT_0058b130+0x14]
            fnstsw ax
            test ah, 0x40
            jne L004ff3ef
            cmp dword ptr [edi + 0xb0], -1
            je L004ff3ef
            mov eax, dword ptr [esp + 0x28]
            mov ecx, 3
            cdq
            fld dword ptr [esp + 0x14]
            idiv ecx
            fdiv dword ptr [esi + edx*4 + 0x64]
            fcom dword ptr [gFLOAT_0058b130+0x10]
            fnstsw ax
            test ah, 1
            je L004ff35c
            mov ecx, dword ptr [esp + 0x28]
            xor ebx, ebx
            cmp ecx, 3
            lea eax, [edx + edx*2]
            setl bl
            dec ebx
            lea eax, [esi + eax*4]
            and ebx, 0xfffffffe
            lea ecx, [edx + 1]
            inc ebx
            mov dword ptr [esp + 0x20], ebx
            lea ecx, [ecx + ecx*2]
            fild dword ptr [esp + 0x20]
            fld dword ptr [eax + 4]
            fmul st(0), st(1)
            mov ebx, esi
            fstp dword ptr [esp + 0x30]
            fld dword ptr [eax + 8]
            fmul st(0), st(1)
            fxch st(2)
            fstp dword ptr [esp + 0x18]
            fxch st(1)
            fstp dword ptr [esp + 0x34]
            fld dword ptr [esi + ecx*4]
            fmul st(0), st(1)
            fstp dword ptr [esp + 0x38]
    L004ff35c:
            fstp st(0)
    L004ff35e:
            mov ecx, dword ptr [esp + 0x10]
    L004ff362:
            test ebp, ebp
            je L004ff37c
            mov eax, dword ptr [esp + 0x1c]
            mov dword ptr [esp + 0x18], 0
            test eax, eax
            mov ebx, ebp
            jne L004ff37c
            mov dword ptr [esp + 0x1c], ebp
    L004ff37c:
            mov eax, dword ptr [esp + 0x4c]
            test eax, eax
            je L004ff3be
            test ebx, ebx
            je L004ff3be
            fld dword ptr [esp + 0x18]
            fcomp dword ptr [gFLOAT_0058b130+0x20]
            fnstsw ax
            test ah, 1
            je L004ff3be
            mov eax, dword ptr [esp + 0x18]
            lea edx, [esp + 0x30]
            push edx
            lea edx, [ebx + 0xb8]
            push eax
            mov eax, dword ptr [ebx + 0xb4]
            push edx
            mov edx, ecx
            push eax
            mov ecx, ebx
            call DoEnvSound
            mov ecx, dword ptr [esp + 0x10]
    L004ff3be:
            mov eax, dword ptr [esp + 0x2c]
            mov edx, dword ptr [gProgram_state+0x1bfc]
            inc eax
            add esi, 0xdc
            add edi, 0xdc
            cmp eax, edx
            mov dword ptr [esp + 0x2c], eax
            jl L004ff14c
    L004ff3e1:
            mov eax, dword ptr [esp + 0x1c]
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x38
            ret 4
    L004ff3ef:
            fld dword ptr [esp + 0x14]
            fcomp dword ptr [gFLOAT_0058b130+0x14]
            fnstsw ax
            test ah, 0x40
            je L004ff35e
            mov ebp, esi
            jmp L004ff35e
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004748a0
void C2_HOOK_FAKE_THISCALL ControlBoundFunkGroove(int pSlot_number, undefined4 pArg2, float pValue) {
    if (pSlot_number < 0) {
        return;
    }
    if (pSlot_number >= (int)CARPOCALYPSE2_ASIZE(gGroove_funk_bindings)) {
        FatalError(kFatalError_UsedRefNumOfControlledGroovidelicFunkotronicIsOutOfRange);
    }
    if (!gGroove_funk_bindings[pSlot_number].field_0x4) {
        *gGroove_funk_bindings[pSlot_number].v = pValue;
    }
}

// FUNCTION: CARMA2_HW 0x004748e0
float C2_NAKED C2_HOOK_FAKE_THISCALL ControlBoundFunkGroovePlus(int pSlot_number, undefined4 pArg2, float pValue) {
#ifndef CARPOCALYPSE2_MATCHING

    if (pSlot_number < 0) {
        return 0.f;
    }
    if (pSlot_number >= (int)CARPOCALYPSE2_ASIZE(gGroove_funk_bindings)) {
        FatalError(kFatalError_UsedRefNumOfControlledGroovidelicFunkotronicIsOutOfRange);
    }
    if (!gGroove_funk_bindings[pSlot_number].field_0x4) {
        *gGroove_funk_bindings[pSlot_number].v = fmodf(*gGroove_funk_bindings[pSlot_number].v + pValue, 1.f);
    }
    return *gGroove_funk_bindings[pSlot_number].v;
#else
    __asm {
            push esi
            mov esi, ecx
            test esi, esi
            push edi
            jge L004748f3
            fld dword ptr [gDouble_0058a068+0xb0]
            pop edi
            pop esi
            ret 4
    L004748f3:
            cmp esi, 0x5a0
            jl L00474905
            push 0x49
            call FatalError
            add esp, 4
    L00474905:
            mov al, byte ptr [esi*8 + gGroove_funk_bindings+0x4]
            test al, al
            jne L00474938
            mov edi, dword ptr [esi*8 + gGroove_funk_bindings]
            fld dword ptr [esp + 0xc]
            _emit 0xd8
        _emit 0x07
            fld qword ptr [gDouble_0058a068+0xb8]
            call _CIfmod
            _emit 0xd9
        _emit 0x1f
            mov eax, dword ptr [esi*8 + gGroove_funk_bindings]
            _emit 0xd9
        _emit 0x00
            pop edi
            pop esi
            ret 4
    L00474938:
            mov ecx, dword ptr [esi*8 + gGroove_funk_bindings]
            pop edi
            pop esi
            _emit 0xd9
        _emit 0x01
            ret 4
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00506e50
void C2_HOOK_FASTCALL ProcessTrack(br_actor* pWorld, tTrack_spec* pTrack_spec, br_actor* pCamera, br_matrix34* pCamera_to_world_transform) {
    RenderTrack(pWorld, pTrack_spec, pCamera, pCamera_to_world_transform);
    if (gAdditional_actors) {
        BrZbsSceneRenderAdd(gAdditional_actors);
    }
}

// FUNCTION: CARMA2_HW 0x004f0950
void C2_HOOK_FASTCALL FUN_004f0950(void) {
    gCount_track_smashable_environment_specs = 0;
    gTrack_smashable_environment_specs = NULL;
}

// FUNCTION: CARMA2_HW 0x00507340
void C2_HOOK_FASTCALL FUN_00507340(void) {

    NewTextHeadupSlot(4, 0, 2000, -2, "Saving Additional Stuff");
    SaveAdditionalActors();
}

// FUNCTION: CARMA2_HW 0x00507580
void C2_HOOK_FASTCALL LoadAdditionalStuff(void) {
    int i;

    if (gNum_active_non_cars != 0) {
        NewTextHeadupSlot(4, 0, 2000, -2, "Can't load with active non cars");
        return;
    }
    NewTextHeadupSlot(4, 0, 2000, -2, "Loading Additional Stuff");
    for (i = 0; i < gNumber_of_additional_models; i++) {
        BrModelRemove(gAdditional_models[i]);
        BrModelFree(gAdditional_models[i]);
    }
    BrActorRemove(gAdditional_actors);
    BrActorFree(gAdditional_actors);
    gLast_actor = NULL;
    gNumber_of_additional_models = BrModelLoadMany(gAdditional_model_path, gAdditional_models, 1000);
    for (i = 0; i < gNumber_of_additional_models; i++) {
        gAdditional_models[i]->flags = 0x80;
    }
    BrModelAddMany(gAdditional_models, gNumber_of_additional_models);
    gAdditional_actors = BrActorLoad(gAdditional_actor_path);
    if (gAdditional_actors == NULL) {
        gAdditional_actors = BrActorAllocate(0, NULL);
    }
    BrActorAdd(gUniverse_actor, gAdditional_actors);
}

// FUNCTION: CARMA2_HW 0x00508050
intptr_t C2_HOOK_CDECL FindHighestPowerupActorNumber(br_actor* pActor, int* pHighest) {
    char name[256];
    int number;

    if (pActor->identifier != NULL && pActor->identifier[0] == '@') {
        strcpy(name, &pActor->identifier[4]);
        name[4] = '\0';
        sscanf(name, "%d", &number);
        if (*pHighest < number) {
            *pHighest = number;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x005080d0
intptr_t C2_NAKED C2_HOOK_CDECL DuplicatePowerupActorModel(br_actor* pActor, int* pHighest) {
#ifndef CARPOCALYPSE2_MATCHING
    char actor_name[256];
    char model_name[256];
    br_model* model;
    br_model* new_model;

    if (pActor->identifier != NULL && pActor->identifier[0] != '@') {
        *pHighest += 1;
        strcpy(actor_name, pActor->identifier);
        actor_name[0] = '@';
        strtok(actor_name, ".");
        strcat(actor_name, "0000");
        sprintf(&actor_name[4], "%04d", *pHighest);
        strcpy(model_name, actor_name);
        strcat(actor_name, ".ACT");
        BrResFree(pActor->identifier);
        pActor->identifier = BrResStrDup(pActor, actor_name);
        model = pActor->model;
        if (model != NULL) {
            strcat(model_name, ".DAT");
            new_model = BrModelAllocate(model_name, model->nvertices, model->nfaces);
            memcpy(new_model->vertices, model->vertices, model->nvertices * sizeof(br_vertex));
            memcpy(new_model->faces, model->faces, model->nfaces * sizeof(br_face));
            new_model->flags |= BR_MODF_UPDATEABLE;
            BrModelAdd(new_model);
            BrModelUpdate(new_model, BR_MODU_ALL);
            pActor->model = new_model;
            gAdditional_models[gNumber_of_additional_models] = new_model;
            gNumber_of_additional_models++;
        }
    }
    return 0;
#else
    __asm {
            sub esp, 0x200
            push ebx
            mov ebx, dword ptr [esp + 0x208]
            push ebp
            push esi
            mov eax, dword ptr [ebx + 0x14]
            push edi
            test eax, eax
            je L005082be
            cmp byte ptr [eax], 0x40
            je L005082be
            mov ebp, dword ptr [esp + 0x218]
            xor eax, eax
            lea edx, [esp + 0x10]
            push offset gPed_flags_0058f2e4+0x178
            mov ecx, dword ptr [ebp]
            inc ecx
            mov dword ptr [ebp], ecx
            mov edi, dword ptr [ebx + 0x14]
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            shr ecx, 2
            rep movsd
            mov ecx, eax
            and ecx, 3
            rep movsb
            lea ecx, [esp + 0x14]
            mov byte ptr [esp + 0x14], 0x40
            push ecx
            call strtok
            mov edi, offset gDepth_effect_names+0x32c
            or ecx, 0xffffffff
            xor eax, eax
            add esp, 8
            repne scasb
            not ecx
            sub edi, ecx
            lea edx, [esp + 0x10]
            mov esi, edi
            mov edi, edx
            mov edx, ecx
            or ecx, 0xffffffff
            repne scasb
            mov ecx, edx
            dec edi
            shr ecx, 2
            rep movsd
            mov eax, dword ptr [ebp]
            mov ecx, edx
            and ecx, 3
            push eax
            rep movsb
            lea ecx, [esp + 0x18]
            push offset gRender_alternative_track_actors+0x4
            push ecx
            call sprintf
            lea edi, [esp + 0x1c]
            or ecx, 0xffffffff
            xor eax, eax
            add esp, 0xc
            repne scasb
            not ecx
            sub edi, ecx
            lea edx, [esp + 0x110]
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            lea edx, [esp + 0x10]
            shr ecx, 2
            rep movsd
            mov ecx, eax
            xor eax, eax
            and ecx, 3
            rep movsb
            mov edi, offset gDefault_drone_direction+0x6dc
            or ecx, 0xffffffff
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
            mov eax, dword ptr [ebx + 0x14]
            mov ecx, ebp
            and ecx, 3
            push eax
            rep movsb
            call BrResFree
            add esp, 4
            lea ecx, [esp + 0x10]
            push ecx
            push ebx
            call BrResStrDup
            mov edx, dword ptr [ebx + 0x18]
            add esp, 8
            test edx, edx
            mov dword ptr [ebx + 0x14], eax
            je L005082be
            mov edi, offset gFrontend_QUIT+0xbb7c
            or ecx, 0xffffffff
            xor eax, eax
            lea ebp, [esp + 0x110]
            repne scasb
            not ecx
            sub edi, ecx
            mov esi, edi
            mov edi, ebp
            mov ebp, ecx
            or ecx, 0xffffffff
            repne scasb
            mov ecx, ebp
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, ebp
            and ecx, 3
            rep movsb
            mov ax, word ptr [edx + 0x12]
            xor ecx, ecx
            mov cx, word ptr [edx + 0x10]
            push eax
            lea edx, [esp + 0x114]
            push ecx
            push edx
            call BrModelAllocate
            mov edx, dword ptr [ebx + 0x18]
            mov ebp, eax
            xor eax, eax
            add esp, 0xc
            mov ax, word ptr [edx + 0x10]
            mov esi, dword ptr [edx + 8]
            mov edi, dword ptr [ebp + 8]
            push ebp
            lea ecx, [eax + eax*4]
            shl ecx, 3
            mov eax, ecx
            shr ecx, 2
            rep movsd
            mov ecx, eax
            xor eax, eax
            and ecx, 3
            rep movsb
            mov edx, dword ptr [ebx + 0x18]
            mov edi, dword ptr [ebp + 0xc]
            mov ax, word ptr [edx + 0x12]
            mov esi, dword ptr [edx + 0xc]
            lea ecx, [eax + eax*4]
            shl ecx, 3
            mov edx, ecx
            shr ecx, 2
            rep movsd
            mov ecx, edx
            and ecx, 3
            rep movsb
            mov eax, dword ptr [ebx + 0x18]
            mov cx, word ptr [eax + 0x20]
            mov word ptr [ebp + 0x20], cx
            call BrModelAdd
            mov dword ptr [ebx + 0x18], ebp
            mov eax, dword ptr [gNumber_of_additional_models]
            add esp, 4
            mov dword ptr [eax*4 + gAdditional_models], ebp
            inc eax
            mov dword ptr [gNumber_of_additional_models], eax
    L005082be:
            pop edi
            pop esi
            pop ebp
            xor eax, eax
            pop ebx
            add esp, 0x200
            ret
            nop
            nop
            nop
            nop
            nop
            xor edx, edx
            xor ecx, ecx
            jmp DropActor
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            xor edx, edx
            mov ecx, 1
            jmp DropActor
            nop
            nop
            nop
            nop
            xor edx, edx
            mov ecx, 2
            jmp DropActor
            nop
            nop
            nop
            nop
            xor edx, edx
            mov ecx, 3
            jmp DropActor
            nop
            nop
            nop
            nop
            xor edx, edx
            mov ecx, 4
            jmp DropActor
            nop
            nop
            nop
            nop
            xor edx, edx
            mov ecx, 5
            jmp DropActor
            nop
            nop
            nop
            nop
            xor edx, edx
            mov ecx, 6
            jmp DropActor
            nop
            nop
            nop
            nop
            xor edx, edx
            mov ecx, 7
            jmp DropActor
            nop
            nop
            nop
            nop
            xor edx, edx
            mov ecx, 8
            jmp DropActor
            nop
            nop
            nop
            nop
            xor edx, edx
            mov ecx, 9
            jmp DropActor
            nop
            nop
            nop
            nop
            mov edx, 1
            xor ecx, ecx
            jmp DropActor
            nop
            nop
            nop
            nop
            mov edx, 1
            mov ecx, edx
            jmp DropActor
            nop
            nop
            nop
            nop
            mov edx, 1
            mov ecx, 2
            jmp DropActor
            nop
            mov edx, 1
            mov ecx, 3
            jmp DropActor
            nop
            mov edx, 1
            mov ecx, 4
            jmp DropActor
            nop
            mov edx, 1
            mov ecx, 5
            jmp DropActor
            nop
            mov edx, 1
            mov ecx, 6
            jmp DropActor
            nop
            mov edx, 1
            mov ecx, 7
            jmp DropActor
            nop
            mov edx, 1
            mov ecx, 8
            jmp DropActor
            nop
            mov edx, 1
            mov ecx, 9
            jmp DropActor
            nop
            mov eax, dword ptr [gCar_to_view]
            sub esp, 0x110
            cmp eax, offset gProgram_state+0xac
            push ebx
            push ebp
            push esi
            push edi
            je L00508438
            xor ecx, ecx
            call FUN_0040e530
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x110
            ret
    L00508438:
            mov eax, dword ptr [gAdditional_actors]
            xor ebp, ebp
            mov dword ptr [esp + 0x1c], 0x7f7fffff
            mov ecx, dword ptr [eax + 8]
            test ecx, ecx
            je L00508523
            mov edx, dword ptr [gOur_pos]
    L00508458:
            mov eax, dword ptr [ecx + 0x18]
            test eax, eax
            je L00508519
            fld dword ptr [eax + 0x34]
            fadd dword ptr [eax + 0x40]
            fmul dword ptr [gDouble_0058b1f0+0x8]
            fstp dword ptr [esp + 0x10]
            mov eax, dword ptr [ecx + 0x18]
            fld dword ptr [eax + 0x38]
            fadd dword ptr [eax + 0x44]
            fmul dword ptr [gDouble_0058b1f0+0x8]
            fstp dword ptr [esp + 0x14]
            mov eax, dword ptr [ecx + 0x18]
            fld dword ptr [eax + 0x3c]
            fadd dword ptr [eax + 0x48]
            fmul dword ptr [gDouble_0058b1f0+0x8]
            fstp dword ptr [esp + 0x18]
            fld dword ptr [ecx + 0x50]
            fadd dword ptr [esp + 0x10]
            fstp dword ptr [esp + 0x10]
            fld dword ptr [ecx + 0x54]
            fadd dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x10]
            fxch st(1)
            fstp dword ptr [esp + 0x14]
            fld dword ptr [ecx + 0x58]
            fadd dword ptr [esp + 0x18]
            fld dword ptr [esp + 0x14]
            fxch st(1)
            fstp dword ptr [esp + 0x18]
            fxch st(1)
            _emit 0xd8
        _emit 0x22
            fld dword ptr [esp + 0x18]
            fxch st(1)
            fstp dword ptr [esp + 0x10]
            fxch st(1)
            fsub dword ptr [edx + 4]
            fld dword ptr [esp + 0x10]
            fmul dword ptr [esp + 0x10]
            fxch st(1)
            fstp dword ptr [esp + 0x14]
            fxch st(1)
            fsub dword ptr [edx + 8]
            fld dword ptr [esp + 0x14]
            fmul dword ptr [esp + 0x14]
            fxch st(1)
            fst dword ptr [esp + 0x18]
            fmul dword ptr [esp + 0x18]
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fcom dword ptr [esp + 0x1c]
            fnstsw ax
            test ah, 1
            je L00508517
            fstp dword ptr [esp + 0x1c]
            mov ebp, ecx
            jmp L00508519
    L00508517:
            fstp st(0)
    L00508519:
            mov ecx, dword ptr [ecx]
            test ecx, ecx
            jne L00508458
    L00508523:
            mov eax, dword ptr [gNum_active_non_cars]
            xor edi, edi
            test eax, eax
            jle L005085cc
            mov ebx, offset gActive_non_car_list
    L00508537:
            mov eax, dword ptr [ebx]
            cmp word ptr [eax + 0x80], 0xffff
            jne L005085bb
            mov esi, dword ptr [eax + 0x10]
            mov ecx, dword ptr [eax + 8]
            add ecx, 0x14
            lea eax, [esp + 0x10]
            lea edx, [esi + 0x2c]
            push edx
            push ecx
            push eax
            call BrMatrix34ApplyP
            mov eax, dword ptr [gOur_pos]
            add esp, 0xc
            fld dword ptr [esp + 0x10]
            _emit 0xd8
        _emit 0x20
            fld dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x18]
            fxch st(2)
            fstp dword ptr [esp + 0x10]
            fsub dword ptr [eax + 4]
            fld dword ptr [esp + 0x10]
            fmul dword ptr [esp + 0x10]
            fxch st(1)
            fstp dword ptr [esp + 0x14]
            fxch st(1)
            fsub dword ptr [eax + 8]
            fld dword ptr [esp + 0x14]
            fmul dword ptr [esp + 0x14]
            fxch st(1)
            fst dword ptr [esp + 0x18]
            fmul dword ptr [esp + 0x18]
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fcom dword ptr [esp + 0x1c]
            fnstsw ax
            test ah, 1
            je L005085b9
            fstp dword ptr [esp + 0x1c]
            mov ebp, esi
            jmp L005085bb
    L005085b9:
            fstp st(0)
    L005085bb:
            mov eax, dword ptr [gNum_active_non_cars]
            inc edi
            add ebx, 4
            cmp edi, eax
            jl L00508537
    L005085cc:
            test ebp, ebp
            je L00508678
            mov edi, offset gDepth_effect_names+0x33c
            or ecx, 0xffffffff
            xor eax, eax
            lea edx, [esp + 0x20]
            repne scasb
            not ecx
            sub edi, ecx
            mov dword ptr [gLast_actor], ebp
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            shr ecx, 2
            rep movsd
            mov ecx, eax
            and ecx, 3
            rep movsb
            mov edi, dword ptr [ebp + 0x14]
            test edi, edi
            je L00508630
            or ecx, 0xffffffff
            xor eax, eax
            repne scasb
            not ecx
            sub edi, ecx
            lea edx, [esp + 0x20]
            mov esi, edi
            mov ebx, ecx
            mov edi, edx
            or ecx, 0xffffffff
            repne scasb
            mov ecx, ebx
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, ebx
            and ecx, 3
            rep movsb
    L00508630:
            lea eax, [esp + 0x20]
            xor edx, edx
            push eax
            push -2
            push 0x7d0
            mov ecx, 4
            call NewTextHeadupSlot
            mov eax, dword ptr [ebp + 0x14]
            test eax, eax
            je L00508678
            cmp byte ptr [eax], 0x26
            jne L00508678
            mov al, byte ptr [eax + 1]
            cmp al, 0x30
            jl L00508678
            cmp al, 0x39
            jg L00508678
            mov ecx, dword ptr [gLast_actor]
            call FUN_0040e530
            mov eax, dword ptr [gCar_flying]
            test eax, eax
            jne L00508678
            call ToggleFlying
    L00508678:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x110
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
            sub esp, 0xc
            mov eax, dword ptr [gNum_active_non_cars]
            mov dword ptr [esp + 8], ecx
            test eax, eax
            mov dword ptr [esp + 4], 0
            jle L00508826
            push edi
            push esi
            push ebp
            push ebx
            mov dword ptr [esp + 0x10], 0x79eda0
    L005086b8:
            mov eax, dword ptr [esp + 0x10]
            mov eax, dword ptr [eax]
            cmp word ptr [eax + 0x80], 0xffff
            jne L00508800
            mov esi, dword ptr [eax + 0x10]
            test esi, esi
            je L00508800
            mov ecx, dword ptr [esi + 0x14]
            test ecx, ecx
            je L00508800
            mov edx, dword ptr [esp + 0x18]
            call DRStricmp
            test eax, eax
            jne L00508800
            mov eax, dword ptr [esi + 0x14]
            cmp byte ptr [eax], 0x26
            jne L00508800
            mov al, byte ptr [eax + 1]
            cmp al, 0x30
            jl L00508800
            cmp al, 0x39
            jg L00508800
            mov ecx, dword ptr [esi + 0xc]
            mov eax, dword ptr [gAdditional_actors]
            cmp ecx, eax
            je L00508800
            mov ecx, esi
            call RemoveNonCarFromActiveList
            mov edx, offset RemoveAdditionalActorModel
            mov ecx, esi
            push 0
            call DRActorEnumRecurse
            mov edx, offset DelGrooveRef
            mov ecx, esi
            push 0
            call DRActorEnumRecurse
            push esi
            call BrActorRemove
            add esp, 4
            push esi
            call BrActorFree
            or ecx, 0xffffffff
            mov edi, offset gAdditional_actor_path
            xor eax, eax
            add esp, 4
            repne scasb
            not ecx
            dec ecx
            mov edi, offset gAdditional_actor_path
            mov ebp, ecx
            or ecx, 0xffffffff
            repne scasb
            not ecx
            dec ecx
            mov edi, offset gDepth_effect_names+0x230
            mov ebx, ecx
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            mov dword ptr [gLast_actor], 0
            mov esi, edi
            mov edx, ecx
            mov edi, offset gAdditional_actor_path
            or ecx, 0xffffffff
            repne scasb
            mov ecx, edx
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, edx
            and ecx, 3
            rep movsb
            mov edi, offset gDepth_effect_names+0x230
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            mov esi, edi
            mov edx, ecx
            mov edi, offset gAdditional_model_path
            or ecx, 0xffffffff
            repne scasb
            mov ecx, edx
            dec edi
            shr ecx, 2
            rep movsd
            mov eax, dword ptr [gSpec_vol_mode]
            mov ecx, edx
            and ecx, 3
            test eax, eax
            rep movsb
            je L005087e8
            call SaveSpecialVolumes
            jmp L005087ed
    L005087e8:
            call SaveAdditionalActors
    L005087ed:
            mov byte ptr [ebp + gAdditional_actor_path], 0
            mov byte ptr [ebx + gAdditional_model_path], 0
            call FlushAllPedCaches
    L00508800:
            mov eax, dword ptr [esp + 0x14]
            mov edx, dword ptr [esp + 0x10]
            mov ecx, dword ptr [gNum_active_non_cars]
            inc eax
            add edx, 4
            cmp eax, ecx
            mov dword ptr [esp + 0x14], eax
            mov dword ptr [esp + 0x10], edx
            jl L005086b8
            pop ebx
            pop ebp
            pop esi
            pop edi
    L00508826:
            add esp, 0xc
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00507ea0
intptr_t C2_NAKED C2_HOOK_CDECL FindHighestSpecialActorNumber(br_actor* pActor, int* pHighest) {
#ifndef CARPOCALYPSE2_MATCHING
    char name[256];
    int number;

    if (pActor->identifier != NULL && pActor->identifier[0] == '&' && strlen(pActor->identifier) == 12) {
        strcpy(name, &pActor->identifier[4]);
        strtok(name, ".");
        sscanf(name, "%d", &number);
        if (*pHighest < number) {
            *pHighest = number;
        }
    }
    return 0;
#else
    __asm {
            mov eax, dword ptr [esp + 4]
            sub esp, 0x104
            mov edx, dword ptr [eax + 0x14]
            push esi
            test edx, edx
            push edi
            je L00507f27
            cmp byte ptr [edx], 0x26
            jne L00507f27
            mov edi, edx
            or ecx, 0xffffffff
            xor eax, eax
            repne scasb
            not ecx
            dec ecx
            cmp ecx, 0xc
            jne L00507f1a
            lea edi, [edx + 4]
            or ecx, 0xffffffff
            xor eax, eax
            lea edx, [esp + 0xc]
            repne scasb
            not ecx
            sub edi, ecx
            push offset gPed_flags_0058f2e4+0x178
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            shr ecx, 2
            rep movsd
            mov ecx, eax
            and ecx, 3
            rep movsb
            lea ecx, [esp + 0x10]
            push ecx
            call strtok
            add esp, 8
            lea edx, [esp + 8]
            lea eax, [esp + 0xc]
            push edx
            push offset gPed_flags_0058f2e4+0x134
            push eax
            call sscanf
            mov eax, dword ptr [esp + 0x14]
            add esp, 0xc
    L00507f1a:
            mov ecx, dword ptr [esp + 0x114]
            cmp eax, dword ptr [ecx]
            jle L00507f27
            mov dword ptr [ecx], eax
    L00507f27:
            pop edi
            xor eax, eax
            pop esi
            add esp, 0x104
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00507f40
intptr_t C2_HOOK_CDECL RenameSpecialActor(br_actor* pActor, int pNumber) {
    char actor_name[256];

    if (pActor->identifier != NULL) {
        strcpy(actor_name, pActor->identifier);
        strtok(actor_name, ".");
        if (gCurrent_dropped_actor_index >= 0) {
            sprintf(&actor_name[1], "\xA3%02d", gCurrent_dropped_actor_index);
        }
        strcat(actor_name, "0000");
        sprintf(&actor_name[4], "%04d", pNumber);
        strcat(actor_name, ".ACT");
        BrResFree(pActor->identifier);
        pActor->identifier = BrResStrDup(pActor, actor_name);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x005076a0
void C2_NAKED C2_HOOK_FASTCALL DropActor(int pIndex, int pPowerup) {
#ifndef CARPOCALYPSE2_MATCHING
    FILE* file;
    tPath_name path;
    char actor_name[256];
    char error_text[256];
    char status_text[256];
    tBounds bounds;
    tFace_ref faces[50];
    br_transform new_transform;
    int face_count;
    int face_index;
    int actor_path_length;
    int current_index;
    int mode;
    int i;
    int highest;
    float nearest_distance;
    float distance;
    float axis_x;
    float axis_y;

    current_index = pIndex;
    mode = pPowerup;
    if (gSelected_actor_name == NULL) {
        if (PDKeyDown(4)) {
            pIndex += 0x50;
            current_index = pIndex;
        }
        if (PDKeyDown(2)) {
            pIndex += 0x28;
            current_index = pIndex;
        }
        if (PDKeyDown(1)) {
            pIndex += 0x14;
            current_index = pIndex;
        }
        if (PDKeyDown(0)) {
            pIndex += 10;
            current_index = pIndex;
        }
        PathCat(path, gApplication_path, mode ? "POWRSHIT.TXT" : "ACCESSRY.TXT");
        file = DRfopen(path, "rt");
        if (pIndex >= 0) {
            pIndex += 1;
            do {
                GetAString(file, actor_name);
                pIndex -= 1;
            } while (pIndex != 0);
        }
    } else {
        sprintf(actor_name, "%s", gSelected_actor_name);
        file = NULL;
    }

    gLast_actor = LoadActor(actor_name);
    if (gLast_actor == NULL || gLast_actor->model == NULL) {
        sprintf(error_text, "CAN'T FIND %s", actor_name);
        NewTextHeadupSlot(4, 0, 2000, -2, error_text);
        goto done;
    } else {
        if (gLast_actor->identifier != NULL && gLast_actor->identifier[0] != '&'
                && gLast_actor->model->nvertices == 0) {
            sprintf(error_text, "CAN'T SHIT %s BECAUSE MODEL MUST BE IN REG OR START WITH '-'", actor_name);
            NewTextHeadupSlot(4, 0, 2000, -2, error_text);
            gLast_actor = NULL;
            goto done;
        }
        if (strlen(actor_name) > 3 && gLast_actor->identifier != NULL && strlen(gLast_actor->identifier) > 3) {
            gLast_actor->identifier[0] = actor_name[0];
            gLast_actor->identifier[1] = actor_name[1];
            gLast_actor->identifier[2] = actor_name[2];
        }
        if (gLast_actor->identifier[0] == '&' && gLast_actor->identifier[1] >= '0'
                && gLast_actor->identifier[1] <= '9'
                && gNon_car_indices[gLast_actor->identifier[1] * 10 + gLast_actor->identifier[2]] == 0) {
            sprintf(error_text, "CAN'T SHIT %s BECAUSE YOU DON'T HAVE A NONCAR.TXT FILE", actor_name);
            NewTextHeadupSlot(4, 0, 2000, -2, error_text);
            gLast_actor = NULL;
            goto done;
        }

        BrMatrix34Identity(&gLast_actor->t.t.mat);
        bounds.original_bounds.min.v[0] = -0.05f;
        bounds.original_bounds.min.v[1] = -0.05f;
        bounds.original_bounds.min.v[2] = -0.05f;
        bounds.original_bounds.max.v[0] = 0.05f;
        bounds.original_bounds.max.v[1] = 0.05f;
        bounds.original_bounds.max.v[2] = 0.05f;
        bounds.mat = &gPlayer_car_master_actor->t.t.mat;
        do {
            face_count = FindFacesInBox(&bounds, faces, 50, NULL);
            bounds.original_bounds.min.v[0] += bounds.original_bounds.min.v[0];
            bounds.original_bounds.min.v[1] += bounds.original_bounds.min.v[1];
            bounds.original_bounds.min.v[2] += bounds.original_bounds.min.v[2];
            bounds.original_bounds.max.v[0] += bounds.original_bounds.max.v[0];
            bounds.original_bounds.max.v[1] += bounds.original_bounds.max.v[1];
            bounds.original_bounds.max.v[2] += bounds.original_bounds.max.v[2];
        } while (face_count == 0);

        face_index = -1;
        nearest_distance = 3.402823466e+38F;
        for (i = 0; i < face_count; i++) {
            distance = DistanceFromFace(gOur_pos, &faces[i]);
            if (fabsf(distance) < fabsf(nearest_distance)) {
                face_index = i;
                nearest_distance = distance;
            }
        }
        if (face_index < 0) {
            goto done;
        }

        if (gLast_actor->identifier[0] == '&' && gLast_actor->identifier[1] >= '0'
                && gLast_actor->identifier[1] <= '9' && gLast_actor->model->bounds.min.v[1] < gDouble_0058b1e0) {
            nearest_distance += gLast_actor->model->bounds.min.v[1] - gDouble_0058b1e0;
        }
        nearest_distance = -nearest_distance;
        gLast_actor->t.t.translate.t.v[0] = faces[face_index].normal.v[0] * nearest_distance + gOur_pos->v[0];
        gLast_actor->t.t.translate.t.v[1] = faces[face_index].normal.v[1] * nearest_distance + gOur_pos->v[1];
        gLast_actor->t.t.translate.t.v[2] = faces[face_index].normal.v[2] * nearest_distance + gOur_pos->v[2];

        if (!PDKeyDown(4)) {
            axis_x = gNegative_one;
            axis_y = gZero_0058b1c8;
            if (gDouble_0058b1f0 < fabsf(0.0f - faces[face_index].normal.v[0] * gOne_0058b1ec)) {
                axis_x = gZero_0058b1c8;
                axis_y = gNegative_one;
            }
            new_transform.type = BR_TRANSFORM_LOOK_UP;
            new_transform.t.look_up.look.v[0] = faces[face_index].normal.v[2] * axis_y;
            new_transform.t.look_up.look.v[1] = -faces[face_index].normal.v[2] * axis_x;
            new_transform.t.look_up.look.v[2] = faces[face_index].normal.v[1] * axis_x - faces[face_index].normal.v[0] * axis_y;
            new_transform.t.look_up.up = faces[face_index].normal;
            new_transform.t.look_up.t = gLast_actor->t.t.translate.t;
            BrTransformToTransform(&gLast_actor->t, &new_transform);
        }

        gLast_actor_position_cache_actor = gLast_actor;
        gLast_actor_position_cache.v[0] = gLast_actor->t.t.translate.t.v[0];
        gLast_actor_position_cache.v[1] = gLast_actor->t.t.translate.t.v[1];
        gLast_actor_position_cache.v[2] = gLast_actor->t.t.translate.t.v[2];
        if (mode != 0) {
            if (current_index > 0x41 && current_index < 0x56) {
                SetSpinningPowerup(gLast_actor, 0);
            }
            if (current_index > 0x55 && current_index < 0x58) {
                SetChangingPowerup(gLast_actor);
            }
        }
        if (mode == 0) {
            if (gLast_actor->identifier != NULL && gLast_actor->identifier[0] != '&') {
                highest = 0;
                DRActorEnumRecurse(gUniverse_actor, (br_actor_enum_cbfn*)FindHighestPowerupActorNumber, &highest);
                DRActorEnumRecurse(gLast_actor, (br_actor_enum_cbfn*)DuplicatePowerupActorModel, &highest);
            }
        } else {
            gLast_actor->identifier[0] = '&';
        }
        if (mode == 0) {
            current_index = -1;
        }
        if (gLast_actor->identifier != NULL && gLast_actor->identifier[0] != '@') {
            highest = 0;
            DRActorEnumRecurse(gUniverse_actor, (br_actor_enum_cbfn*)FindHighestSpecialActorNumber, &highest);
            gCurrent_dropped_actor_index = current_index;
            DRActorEnumRecurse(gLast_actor, (br_actor_enum_cbfn*)RenameSpecialActor, (void*)(highest + 1));
        }

        BrActorAdd(gAdditional_actors, gLast_actor);
        actor_path_length = strlen(gAdditional_actor_path);
        strcat(gAdditional_actor_path, " autosave");
        strcat(gAdditional_model_path, " autosave");
        if (gSpec_vol_mode == 0) {
            SaveAdditionalActors();
        } else {
            UpdateSpecVol();
            SaveSpecialVolumes();
        }
        gAdditional_actor_path[actor_path_length] = '\0';
        gAdditional_model_path[strlen(gAdditional_model_path) - strlen(" autosave")] = '\0';
        FlushAllPedCaches();
        if (mode != 0) {
            sprintf(actor_name, "SHAT OUT A %s", GetPowerupMessage(current_index));
            NewTextHeadupSlot(4, 0, 2000, -2, actor_name);
        } else {
            strcpy(status_text, "Shat out ");
            if (gLast_actor->identifier != NULL) {
                strcat(status_text, gLast_actor->identifier);
            }
            NewTextHeadupSlot(4, 0, 2000, -2, status_text);
        }
    }

done:
    if (gSelected_actor_name == NULL) {
        PFfclose(file);
    }
#else
    __asm {
            mov eax, 0x1430
            call _chkstk
            mov eax, dword ptr [gSelected_actor_name]
            push ebx
            push ebp
            push esi
            push edi
            mov edi, edx
            mov esi, ecx
            test eax, eax
            mov dword ptr [esp + 0x20], edi
            mov dword ptr [esp + 0x14], esi
            jne L00507767
            mov ecx, 4
            call PDKeyDown
            test eax, eax
            je L005076dc
            add esi, 0x50
            mov dword ptr [esp + 0x14], esi
    L005076dc:
            mov ecx, 2
            call PDKeyDown
            test eax, eax
            je L005076f1
            add esi, 0x28
            mov dword ptr [esp + 0x14], esi
    L005076f1:
            mov ecx, 1
            call PDKeyDown
            test eax, eax
            je L00507706
            add esi, 0x14
            mov dword ptr [esp + 0x14], esi
    L00507706:
            xor ecx, ecx
            call PDKeyDown
            test eax, eax
            je L00507718
            add esi, 0xa
            mov dword ptr [esp + 0x14], esi
    L00507718:
            test edi, edi
            mov eax, offset gMass_mine+0xec
            jne L00507726
            mov eax, offset gDepth_effect_names+0x31c
    L00507726:
            push eax
            mov edx, offset gApplication_path
            lea ecx, [esp + 0x3a4]
            call PathCat
            mov edx, offset gNext_crush_actor_id+0x180
            lea ecx, [esp + 0x3a0]
            call DRfopen
            mov edi, eax
            test esi, esi
            mov dword ptr [esp + 0x24], edi
            jl L0050777d
            inc esi
    L00507754:
            lea edx, [esp + 0xa0]
            mov ecx, edi
            call GetAString
            dec esi
            jne L00507754
            jmp L0050777d
    L00507767:
            push eax
            lea eax, [esp + 0xa4]
            push offset gCancel_recover+0xbc
            push eax
            call sprintf
            add esp, 0xc
    L0050777d:
            lea ecx, [esp + 0xa0]
            call LoadActor
            mov edx, eax
            test edx, edx
            mov dword ptr [gLast_actor], edx
            je L00507e3e
            mov eax, dword ptr [edx + 0x18]
            test eax, eax
            je L00507e3e
            mov esi, dword ptr [edx + 0x14]
            test esi, esi
            je L005077fe
            cmp byte ptr [esi], 0x26
            je L005077fe
            cmp word ptr [eax + 0x10], 0
            jne L005077fe
            lea ecx, [esp + 0xa0]
            lea edx, [esp + 0x1a0]
            push ecx
            push offset gDepth_effect_names+0x2dc
            push edx
            call sprintf
            add esp, 0xc
            lea eax, [esp + 0x1a0]
            xor edx, edx
            mov ecx, 4
            push eax
            push -2
            push 0x7d0
            call NewTextHeadupSlot
            mov dword ptr [gLast_actor], 0
            jmp L00507e76
    L005077fe:
            lea edi, [esp + 0xa0]
            or ecx, 0xffffffff
            xor eax, eax
            repne scasb
            not ecx
            dec ecx
            cmp ecx, 3
            jbe L00507840
            test esi, esi
            je L00507840
            mov edi, esi
            or ecx, 0xffffffff
            repne scasb
            not ecx
            dec ecx
            cmp ecx, 3
            jbe L00507840
    L00507827:
            mov ecx, dword ptr [edx + 0x14]
            mov dl, byte ptr [esp + eax + 0xa0]
            mov byte ptr [ecx + eax], dl
            mov edx, dword ptr [gLast_actor]
            inc eax
            cmp eax, 3
            jl L00507827
    L00507840:
            mov eax, dword ptr [edx + 0x14]
            cmp byte ptr [eax], 0x26
            jne L005078aa
            mov cl, byte ptr [eax + 1]
            cmp cl, 0x30
            jl L005078aa
            cmp cl, 0x39
            jg L005078aa
            movsx esi, byte ptr [eax + 2]
            movsx eax, cl
            lea eax, [eax + eax*4]
            mov cl, byte ptr [esi + eax*2 + gNon_car_indices]
            test cl, cl
            jne L005078aa
            lea ecx, [esp + 0xa0]
            lea edx, [esp + 0x1a0]
            push ecx
            push offset gDepth_effect_names+0x2a4
            push edx
            call sprintf
            add esp, 0xc
            lea eax, [esp + 0x1a0]
            xor edx, edx
            mov ecx, 4
            push eax
            push -2
            push 0x7d0
            call NewTextHeadupSlot
            xor edx, edx
            mov dword ptr [gLast_actor], edx
    L005078aa:
            test edx, edx
            je L00507e76
            add edx, 0x2c
            push edx
            call BrMatrix34Identity
            mov ecx, dword ptr [gProgram_state+0xbc]
            add esp, 4
            add ecx, 0x2c
            mov dword ptr [esp + 0x2c], 0xbd4ccccd
            mov dword ptr [esp + 0x30], 0xbd4ccccd
            mov dword ptr [esp + 0x34], 0xbd4ccccd
            mov dword ptr [esp + 0x38], 0x3d4ccccd
            mov dword ptr [esp + 0x3c], 0x3d4ccccd
            mov dword ptr [esp + 0x40], 0x3d4ccccd
            mov dword ptr [esp + 0x28], ecx
    L005078fb:
            push 0
            push 0x32
            lea edx, [esp + 0x4a8]
            lea ecx, [esp + 0x30]
            call FindFacesInBox
            fld dword ptr [esp + 0x2c]
            fld dword ptr [esp + 0x30]
            fld dword ptr [esp + 0x34]
            fld dword ptr [esp + 0x38]
            fld dword ptr [esp + 0x3c]
            fld dword ptr [esp + 0x40]
            fxch st(5)
            fadd st(0), st(0)
            mov edi, eax
            test edi, edi
            fstp dword ptr [esp + 0x2c]
            fxch st(3)
            fadd st(0), st(0)
            fxch st(2)
            fadd st(0), st(0)
            fxch st(1)
            fadd st(0), st(0)
            fxch st(3)
            fadd st(0), st(0)
            fxch st(4)
            fadd st(0), st(0)
            fxch st(2)
            fstp dword ptr [esp + 0x30]
            fstp dword ptr [esp + 0x34]
            fxch st(1)
            fstp dword ptr [esp + 0x38]
            fxch st(1)
            fstp dword ptr [esp + 0x3c]
            fstp dword ptr [esp + 0x40]
            je L005078fb
            or ebx, 0xffffffff
            xor esi, esi
            test edi, edi
            mov dword ptr [esp + 0x18], 0x7f7fffff
            jle L005079bf
            lea ebp, [esp + 0x4a0]
    L0050797b:
            mov ecx, dword ptr [gOur_pos]
            mov edx, ebp
            call DistanceFromFace
            fld dword ptr [esp + 0x18]
            fxch st(1)
            fst dword ptr [esp + 0x10]
            fabs
            fxch st(1)
            fabs
            fxch st(1)
            fst dword ptr [esp + 0x1c]
            fxch st(1)
            fst dword ptr [esp + 0x1c]
            fcompp
            fnstsw ax
            test ah, 0x41
            jne L005079b7
            mov edx, dword ptr [esp + 0x10]
            mov ebx, esi
            mov dword ptr [esp + 0x18], edx
    L005079b7:
            inc esi
            add ebp, 0x50
            cmp esi, edi
            jl L0050797b
    L005079bf:
            test ebx, ebx
            jl L00507e76
            mov edx, dword ptr [gLast_actor]
            mov esi, dword ptr [edx + 0x14]
            cmp byte ptr [esi], 0x26
            jne L00507a04
            mov al, byte ptr [esi + 1]
            cmp al, 0x30
            jl L00507a04
            cmp al, 0x39
            jg L00507a04
            mov ecx, dword ptr [edx + 0x18]
            fld dword ptr [ecx + 0x38]
            fcomp qword ptr [gDouble_0058b1e0]
            fnstsw ax
            test ah, 1
            je L00507a04
            fld dword ptr [ecx + 0x38]
            fsub qword ptr [gDouble_0058b1e0]
            fadd dword ptr [esp + 0x18]
            fstp dword ptr [esp + 0x18]
    L00507a04:
            fld dword ptr [esp + 0x18]
            lea eax, [ebx + ebx*4]
            mov ecx, 4
            shl eax, 4
            fld dword ptr [esp + eax + 0x4d4]
            fxch st(1)
            fchs
            fxch st(1)
            fmul st(0), st(1)
            lea esi, [esp + eax + 0x4d4]
            lea ebp, [esp + eax + 0x4d8]
            lea edi, [esp + eax + 0x4dc]
            mov eax, dword ptr [gOur_pos]
            _emit 0xd8
        _emit 0x00
            fstp dword ptr [edx + 0x50]
            _emit 0xd9
        _emit 0x45
        _emit 0x00
            mov edx, dword ptr [gOur_pos]
            mov eax, dword ptr [gLast_actor]
            fmul st(0), st(1)
            fadd dword ptr [edx + 4]
            fstp dword ptr [eax + 0x54]
            _emit 0xd9
        _emit 0x07
            mov edx, dword ptr [gOur_pos]
            mov eax, dword ptr [gLast_actor]
            fmul st(0), st(1)
            fadd dword ptr [edx + 8]
            fstp dword ptr [eax + 0x58]
            fstp st(0)
            call PDKeyDown
            test eax, eax
            jne L00507b3e
            _emit 0xd9
        _emit 0x06
            fmul dword ptr [gOne_0058b1ec]
            fld dword ptr [gNegative_one]
            fxch st(1)
            mov dword ptr [esp + 0x10], eax
            fsubr dword ptr [esp + 0x10]
            fld dword ptr [gZero_0058b1c8]
            fxch st(1)
            fabs
            fst dword ptr [esp + 0x1c]
            fcomp qword ptr [gDouble_0058b1f0]
            fnstsw ax
            test ah, 0x41
            jne L00507abe
            fstp st(0)
            fstp st(0)
            fld dword ptr [gZero_0058b1c8]
            fld dword ptr [gNegative_one]
    L00507abe:
            _emit 0xd9
        _emit 0x07
            fmul st(0), st(1)
            mov eax, dword ptr [esi + 8]
            mov ecx, dword ptr [esi]
            mov edx, dword ptr [esi + 4]
            mov dword ptr [esp + 0x84], eax
            fsub dword ptr [esp + 0x10]
            mov eax, dword ptr [gLast_actor]
            mov dword ptr [esp + 0x7c], ecx
            mov word ptr [esp + 0x6c], 4
            mov dword ptr [esp + 0x80], edx
            lea ecx, [eax + 0x50]
            add eax, 0x28
            fstp dword ptr [esp + 0x70]
            _emit 0xd9
        _emit 0x07
            fmul st(0), st(2)
            fchs
            fstp dword ptr [esp + 0x74]
            _emit 0xd9
        _emit 0x45
        _emit 0x00
            fmul st(0), st(2)
            _emit 0xd9
        _emit 0x06
            fmul st(0), st(2)
            fsubp st(1), st(0)
            fstp dword ptr [esp + 0x78]
            mov edx, dword ptr [ecx]
            mov dword ptr [esp + 0x94], edx
            mov edx, dword ptr [ecx + 4]
            mov dword ptr [esp + 0x98], edx
            lea edx, [esp + 0x6c]
            mov ecx, dword ptr [ecx + 8]
            push edx
            fstp st(0)
            push eax
            mov dword ptr [esp + 0xa4], ecx
            fstp st(0)
            call BrTransformToTransform
            add esp, 8
    L00507b3e:
            mov ecx, dword ptr [gLast_actor]
            mov edi, dword ptr [esp + 0x20]
            mov dword ptr [gLast_actor_position_cache_actor], ecx
            mov ebx, dword ptr [esp + 0x14]
            lea eax, [ecx + 0x50]
            xor ebp, ebp
            cmp edi, ebp
            mov edx, dword ptr [eax]
            mov dword ptr [gLast_actor_position_cache], edx
            mov edx, dword ptr [eax + 4]
            mov dword ptr [gLast_actor_position_cache+0x4], edx
            mov eax, dword ptr [eax + 8]
            mov dword ptr [gLast_actor_position_cache+0x8], eax
            je L00507ba0
            cmp ebx, 0x42
            jl L00507b8b
            cmp ebx, 0x55
            jg L00507b8b
            xor edx, edx
            call SetSpinningPowerup
            mov ecx, dword ptr [gLast_actor]
    L00507b8b:
            cmp ebx, 0x56
            jl L00507ba0
            cmp ebx, 0x57
            jg L00507ba0
            call SetChangingPowerup
            mov ecx, dword ptr [gLast_actor]
    L00507ba0:
            cmp edi, ebp
            jne L00507bde
            mov esi, ecx
            mov eax, dword ptr [esi + 0x14]
            cmp eax, ebp
            je L00507bea
            cmp byte ptr [eax], 0x26
            je L00507bea
            lea ecx, [esp + 0x10]
            mov edx, offset FindHighestPowerupActorNumber
            push ecx
            mov ecx, dword ptr [gUniverse_actor]
            mov dword ptr [esp + 0x14], ebp
            call DRActorEnumRecurse
            lea edx, [esp + 0x10]
            mov ecx, esi
            push edx
            mov edx, offset DuplicatePowerupActorModel
            call DRActorEnumRecurse
            jmp L00507be4
    L00507bde:
            mov eax, dword ptr [ecx + 0x14]
            mov byte ptr [eax], 0x26
    L00507be4:
            mov ecx, dword ptr [gLast_actor]
    L00507bea:
            cmp edi, ebp
            jne L00507bf1
            or ebx, 0xffffffff
    L00507bf1:
            mov edi, dword ptr [gUniverse_actor]
            mov esi, ecx
            mov eax, dword ptr [esi + 0x14]
            cmp eax, ebp
            je L00507c38
            cmp byte ptr [eax], 0x40
            je L00507c38
            lea ecx, [esp + 0x10]
            mov edx, offset FindHighestSpecialActorNumber
            push ecx
            mov ecx, edi
            mov dword ptr [esp + 0x14], ebp
            call DRActorEnumRecurse
            mov ecx, esi
            mov dword ptr [gCurrent_dropped_actor_index], ebx
            mov edx, dword ptr [esp + 0x10]
            inc edx
            push edx
            mov edx, offset RenameSpecialActor
            call DRActorEnumRecurse
            mov ecx, dword ptr [gLast_actor]
    L00507c38:
            mov eax, dword ptr [gAdditional_actors]
            push ecx
            push eax
            call BrActorAdd
            or ecx, 0xffffffff
            mov edi, offset gAdditional_actor_path
            xor eax, eax
            add esp, 8
            repne scasb
            not ecx
            dec ecx
            mov edi, offset gAdditional_actor_path
            mov ebx, ecx
            or ecx, 0xffffffff
            repne scasb
            not ecx
            dec ecx
            mov edi, offset gDepth_effect_names+0x230
            mov ebp, ecx
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            mov esi, edi
            mov edx, ecx
            mov edi, offset gAdditional_actor_path
            or ecx, 0xffffffff
            repne scasb
            mov ecx, edx
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, edx
            and ecx, 3
            rep movsb
            mov edi, offset gDepth_effect_names+0x230
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            mov esi, edi
            mov edx, ecx
            mov edi, offset gAdditional_model_path
            or ecx, 0xffffffff
            repne scasb
            mov ecx, edx
            dec edi
            shr ecx, 2
            rep movsd
            mov eax, dword ptr [gSpec_vol_mode]
            mov ecx, edx
            and ecx, 3
            test eax, eax
            rep movsb
            je L00507d87
            mov edi, dword ptr [gLast_actor]
            test edi, edi
            je L00507cf1
            mov edx, dword ptr [gProgram_state+0x1bfc]
            xor eax, eax
            test edx, edx
            jle L00507cf1
            mov ecx, offset gSpec_vol_actors
    L00507ce5:
            cmp dword ptr [ecx], edi
            je L00507cf4
            inc eax
            add ecx, 4
            cmp eax, edx
            jl L00507ce5
    L00507cf1:
            or eax, 0xffffffff
    L00507cf4:
            test eax, eax
            jl L00507d80
            lea ecx, [eax + eax*4]
            add edi, 0x2c
            push edi
            lea eax, [eax + ecx*2]
            lea edx, [eax + eax*4]
            mov eax, dword ptr [gProgram_state+0x1c00]
            lea esi, [eax + edx*4]
            lea ecx, [esi + 4]
            push ecx
            call BrMatrix34Copy
            add esp, 8
            mov ecx, esi
            call FindInverseAndWorlBox
            mov eax, dword ptr [gLast_actor]
            push 1
            push 0
            mov esi, dword ptr [eax + 0x18]
            lea edi, [eax + 0x2c]
            mov ecx, dword ptr [esi + 0xc]
            mov edx, dword ptr [ecx + 0x2b0]
            push edx
            mov edx, dword ptr [ecx + 0xd0]
            mov ecx, edi
            call MungeMaterialSV
            mov eax, dword ptr [esi + 0xc]
            push 2
            push 1
            mov ecx, dword ptr [eax + 0x3a0]
            mov edx, dword ptr [eax + 0x1c0]
            push ecx
            mov ecx, edi
            call MungeMaterialSV
            mov esi, dword ptr [esi + 0xc]
            mov ecx, edi
            push 2
            push 0
            mov edx, dword ptr [esi + 0x300]
            push edx
            mov edx, dword ptr [esi + 0x120]
            call MungeMaterialSV
    L00507d80:
            call SaveSpecialVolumes
            jmp L00507d8c
    L00507d87:
            call SaveAdditionalActors
    L00507d8c:
            mov byte ptr [ebx + gAdditional_actor_path], 0
            mov byte ptr [ebp + gAdditional_model_path], 0
            call FlushAllPedCaches
            mov eax, dword ptr [esp + 0x20]
            test eax, eax
            je L00507dd3
            mov ecx, dword ptr [esp + 0x14]
            call GetPowerupMessage
            push eax
            lea eax, [esp + 0xa4]
            push offset gDepth_effect_names+0x294
            push eax
            call sprintf
            add esp, 0xc
            lea ecx, [esp + 0xa0]
            push ecx
            jmp L00507e63
    L00507dd3:
            mov edi, offset gDepth_effect_names+0x288
            or ecx, 0xffffffff
            xor eax, eax
            lea ebx, [esp + 0x2a0]
            repne scasb
            not ecx
            sub edi, ecx
            mov edx, dword ptr [gLast_actor]
            mov eax, ecx
            mov esi, edi
            mov edi, ebx
            shr ecx, 2
            rep movsd
            mov ecx, eax
            and ecx, 3
            rep movsb
            mov edi, dword ptr [edx + 0x14]
            test edi, edi
            je L00507e35
            or ecx, 0xffffffff
            xor eax, eax
            repne scasb
            not ecx
            sub edi, ecx
            lea edx, [esp + 0x2a0]
            mov esi, edi
            mov ebx, ecx
            mov edi, edx
            or ecx, 0xffffffff
            repne scasb
            mov ecx, ebx
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, ebx
            and ecx, 3
            rep movsb
    L00507e35:
            lea eax, [esp + 0x2a0]
            jmp L00507e62
    L00507e3e:
            lea ecx, [esp + 0xa0]
            lea edx, [esp + 0x1a0]
            push ecx
            push offset gDepth_effect_names+0x278
            push edx
            call sprintf
            add esp, 0xc
            lea eax, [esp + 0x1a0]
    L00507e62:
            push eax
    L00507e63:
            push -2
            push 0x7d0
            xor edx, edx
            mov ecx, 4
            call NewTextHeadupSlot
    L00507e76:
            mov eax, dword ptr [gSelected_actor_name]
            test eax, eax
            jne L00507e88
            mov ecx, dword ptr [esp + 0x24]
            call PFfclose
    L00507e88:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x1430
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00508e50
intptr_t C2_HOOK_CDECL MoveModelVertices(br_actor* pActor, br_vector3* pOffset) {
    int i;

    if (pActor->model != NULL) {
        i = 0;
        if (pActor->model->nvertices > 0) {
            do {
                pActor->model->vertices[i].p.v[0] += pOffset->v[0];
                pActor->model->vertices[i].p.v[1] += pOffset->v[1];
                pActor->model->vertices[i].p.v[2] += pOffset->v[2];
                i++;
            } while (i < pActor->model->nvertices);
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00508ec0
void C2_HOOK_FASTCALL RecenterActorModels(br_actor* pActor, br_vector3* pOffset) {
    if (pActor->model != NULL) {
        if (gLast_actor_position_cache_actor == gLast_actor) {
            pOffset->v[0] = gNegative_one * gLast_actor_position_cache.v[0];
            pOffset->v[1] = gNegative_one * gLast_actor_position_cache.v[1];
            pOffset->v[2] = gNegative_one * gLast_actor_position_cache.v[2];
        } else {
            pOffset->v[0] = (pActor->model->bounds.max.v[0] + pActor->model->bounds.min.v[0]) * gNegative_half;
            pOffset->v[1] = (pActor->model->bounds.max.v[1] + pActor->model->bounds.min.v[1]) * gNegative_half;
            pOffset->v[2] = (pActor->model->bounds.max.v[2] + pActor->model->bounds.min.v[2]) * gNegative_half;
        }
    } else {
        pOffset->v[0] = 0.f;
        pOffset->v[1] = 0.f;
        pOffset->v[2] = 0.f;
    }
    DRActorEnumRecurse(pActor, MoveModelVertices, pOffset);
    pOffset->v[0] = -pOffset->v[0];
    pOffset->v[1] = -pOffset->v[1];
    pOffset->v[2] = -pOffset->v[2];
}

// FUNCTION: CARMA2_HW 0x00508830
intptr_t C2_NAKED C2_HOOK_CDECL RemoveAdditionalActorModel(br_actor* pActor, void* pArg) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;

    for (i = 0; i < gNumber_of_additional_models; i++) {
        if (pActor->model == gAdditional_models[i]) {
            BrModelRemove(pActor->model);
            BrModelFree(pActor->model);
            memmove(&gAdditional_models[i], &gAdditional_models[i + 1],
                (gNumber_of_additional_models - i - 1) * sizeof(gAdditional_models[0]));
            gNumber_of_additional_models--;
            break;
        }
    }
    return 0;
#else
    __asm {
            mov ecx, dword ptr [gNumber_of_additional_models]
            push esi
            push edi
            xor edi, edi
            test ecx, ecx
            jle L005088a2
            mov esi, dword ptr [esp + 0xc]
            mov eax, offset gAdditional_models
            mov edx, dword ptr [esi + 0x18]
    L0050884a:
            cmp edx, dword ptr [eax]
            je L0050885b
            inc edi
            add eax, 4
            cmp edi, ecx
            jl L0050884a
            xor eax, eax
            pop edi
            pop esi
            ret
    L0050885b:
            push edx
            call BrModelRemove
            mov eax, dword ptr [esi + 0x18]
            add esp, 4
            push eax
            call BrModelFree
            mov eax, dword ptr [gNumber_of_additional_models]
            lea esi, [edi*4 + gAdditional_models+0x4]
            mov ecx, eax
            add esp, 4
            sub ecx, edi
            lea edi, [edi*4 + gAdditional_models]
            lea ecx, [ecx*4 - 4]
            mov edx, ecx
            shr ecx, 2
            rep movsd
            mov ecx, edx
            and ecx, 3
            dec eax
            rep movsb
            mov dword ptr [gNumber_of_additional_models], eax
    L005088a2:
            pop edi
            pop esi
            xor eax, eax
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
    }
#endif
}

// FUNCTION: CARMA2_HW 0x005088b0
void C2_NAKED C2_HOOK_CDECL MurderLastActor(void) {
#ifndef CARPOCALYPSE2_MATCHING
    char text[256];
    int actor_path_length;
    int model_path_length;

    if (gLast_actor != NULL) {
        if (gLast_actor->parent != gAdditional_actors) {
            RemoveNonCarFromActiveList(gLast_actor);
        }
        strcpy(text, "Murdered ");
        if (gLast_actor->identifier != NULL) {
            strcat(text, gLast_actor->identifier);
        }
        NewTextHeadupSlot(4, 0, 2000, -2, text);
        DRActorEnumRecurse(gLast_actor, (br_actor_enum_cbfn*)RemoveAdditionalActorModel, NULL);
        DRActorEnumRecurse(gLast_actor, (br_actor_enum_cbfn*)DelGrooveRef, NULL);
        BrActorRemove(gLast_actor);
        BrActorFree(gLast_actor);
        actor_path_length = strlen(gAdditional_actor_path);
        model_path_length = strlen(gAdditional_actor_path);
        strcat(gAdditional_actor_path, " autosave");
        strcat(gAdditional_model_path, " autosave");
        gLast_actor = NULL;
        if (gSpec_vol_mode != 0) {
            SaveSpecialVolumes();
        } else {
            SaveAdditionalActors();
        }
        gAdditional_actor_path[actor_path_length] = '\0';
        gAdditional_model_path[model_path_length] = '\0';
        FlushAllPedCaches();
    }
#else
    __asm {
            mov edx, dword ptr [gLast_actor]
            sub esp, 0x100
            test edx, edx
            je L00508a3e
            mov eax, dword ptr [edx + 0xc]
            mov ecx, dword ptr [gAdditional_actors]
            push edi
            push esi
            push ebp
            cmp eax, ecx
            push ebx
            je L005088e2
            mov ecx, edx
            call RemoveNonCarFromActiveList
            mov edx, dword ptr [gLast_actor]
    L005088e2:
            mov edi, offset gDepth_effect_names+0x34c
            or ecx, 0xffffffff
            xor eax, eax
            lea ebx, [esp + 0x10]
            repne scasb
            not ecx
            sub edi, ecx
            mov eax, ecx
            mov esi, edi
            mov edi, ebx
            shr ecx, 2
            rep movsd
            mov ecx, eax
            and ecx, 3
            rep movsb
            mov edi, dword ptr [edx + 0x14]
            test edi, edi
            je L00508938
            or ecx, 0xffffffff
            xor eax, eax
            repne scasb
            not ecx
            sub edi, ecx
            lea edx, [esp + 0x10]
            mov esi, edi
            mov ebx, ecx
            mov edi, edx
            or ecx, 0xffffffff
            repne scasb
            mov ecx, ebx
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, ebx
            and ecx, 3
            rep movsb
    L00508938:
            lea eax, [esp + 0x10]
            xor edx, edx
            push eax
            push -2
            push 0x7d0
            mov ecx, 4
            call NewTextHeadupSlot
            mov ecx, dword ptr [gLast_actor]
            mov edx, offset RemoveAdditionalActorModel
            push 0
            call DRActorEnumRecurse
            mov ecx, dword ptr [gLast_actor]
            mov edx, offset DelGrooveRef
            push 0
            call DRActorEnumRecurse
            mov ecx, dword ptr [gLast_actor]
            push ecx
            call BrActorRemove
            mov edx, dword ptr [gLast_actor]
            add esp, 4
            push edx
            call BrActorFree
            or ecx, 0xffffffff
            mov edi, offset gAdditional_actor_path
            xor eax, eax
            add esp, 4
            repne scasb
            not ecx
            dec ecx
            mov edi, offset gAdditional_actor_path
            mov ebp, ecx
            or ecx, 0xffffffff
            repne scasb
            not ecx
            dec ecx
            mov edi, offset gDepth_effect_names+0x230
            mov ebx, ecx
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            mov dword ptr [gLast_actor], 0
            mov esi, edi
            mov edx, ecx
            mov edi, offset gAdditional_actor_path
            or ecx, 0xffffffff
            repne scasb
            mov ecx, edx
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, edx
            and ecx, 3
            rep movsb
            mov edi, offset gDepth_effect_names+0x230
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            mov esi, edi
            mov edx, ecx
            mov edi, offset gAdditional_model_path
            or ecx, 0xffffffff
            repne scasb
            mov ecx, edx
            dec edi
            shr ecx, 2
            rep movsd
            mov eax, dword ptr [gSpec_vol_mode]
            mov ecx, edx
            and ecx, 3
            test eax, eax
            rep movsb
            je L00508a22
            call SaveSpecialVolumes
            jmp L00508a27
    L00508a22:
            call SaveAdditionalActors
    L00508a27:
            mov byte ptr [ebp + gAdditional_actor_path], 0
            mov byte ptr [ebx + gAdditional_model_path], 0
            call FlushAllPedCaches
            pop ebx
            pop ebp
            pop esi
            pop edi
    L00508a3e:
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
            push ecx
            mov ecx, dword ptr [gLast_actor]
            push ebx
            xor ebx, ebx
            cmp ecx, ebx
            je L00508c12
            mov eax, 0x3f800000
            push edi
            mov dword ptr [ecx + 0x2c], eax
            mov ecx, dword ptr [gLast_actor]
            mov edi, offset gAdditional_actor_path
            push esi
            mov dword ptr [ecx + 0x30], ebx
            mov edx, dword ptr [gLast_actor]
            push ebp
            mov dword ptr [edx + 0x34], ebx
            mov ecx, dword ptr [gLast_actor]
            mov dword ptr [ecx + 0x38], ebx
            mov edx, dword ptr [gLast_actor]
            mov dword ptr [edx + 0x3c], eax
            mov ecx, dword ptr [gLast_actor]
            mov dword ptr [ecx + 0x40], ebx
            mov edx, dword ptr [gLast_actor]
            mov dword ptr [edx + 0x44], ebx
            mov ecx, dword ptr [gLast_actor]
            mov dword ptr [ecx + 0x48], ebx
            mov edx, dword ptr [gLast_actor]
            or ecx, 0xffffffff
            mov dword ptr [edx + 0x4c], eax
            xor eax, eax
            repne scasb
            not ecx
            dec ecx
            mov edi, offset gAdditional_actor_path
            mov ebp, ecx
            or ecx, 0xffffffff
            repne scasb
            not ecx
            dec ecx
            mov edi, offset gDepth_effect_names+0x230
            mov dword ptr [esp + 0x10], ecx
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            mov esi, edi
            mov edx, ecx
            mov edi, offset gAdditional_actor_path
            or ecx, 0xffffffff
            repne scasb
            mov ecx, edx
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, edx
            and ecx, 3
            rep movsb
            mov edi, offset gDepth_effect_names+0x230
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            mov esi, edi
            mov edx, ecx
            mov edi, offset gAdditional_model_path
            or ecx, 0xffffffff
            repne scasb
            mov ecx, edx
            dec edi
            shr ecx, 2
            rep movsd
            mov eax, dword ptr [gSpec_vol_mode]
            mov ecx, edx
            and ecx, 3
            cmp eax, ebx
            rep movsb
            je L00508bf5
            mov edi, dword ptr [gLast_actor]
            cmp edi, ebx
            je L00508b61
            mov edx, dword ptr [gProgram_state+0x1bfc]
            xor eax, eax
            cmp edx, ebx
            jle L00508b61
            mov ecx, offset gSpec_vol_actors
    L00508b55:
            cmp dword ptr [ecx], edi
            je L00508b64
            inc eax
            add ecx, 4
            cmp eax, edx
            jl L00508b55
    L00508b61:
            or eax, 0xffffffff
    L00508b64:
            cmp eax, ebx
            jl L00508bee
            lea ecx, [eax + eax*4]
            add edi, 0x2c
            push edi
            lea eax, [eax + ecx*2]
            lea edx, [eax + eax*4]
            mov eax, dword ptr [gProgram_state+0x1c00]
            lea esi, [eax + edx*4]
            lea ecx, [esi + 4]
            push ecx
            call BrMatrix34Copy
            add esp, 8
            mov ecx, esi
            call FindInverseAndWorlBox
            mov eax, dword ptr [gLast_actor]
            push 1
            push ebx
            mov esi, dword ptr [eax + 0x18]
            lea edi, [eax + 0x2c]
            mov ecx, dword ptr [esi + 0xc]
            mov edx, dword ptr [ecx + 0x2b0]
            push edx
            mov edx, dword ptr [ecx + 0xd0]
            mov ecx, edi
            call MungeMaterialSV
            mov eax, dword ptr [esi + 0xc]
            push 2
            push 1
            mov ecx, dword ptr [eax + 0x3a0]
            mov edx, dword ptr [eax + 0x1c0]
            push ecx
            mov ecx, edi
            call MungeMaterialSV
            mov esi, dword ptr [esi + 0xc]
            mov ecx, edi
            push 2
            push ebx
            mov edx, dword ptr [esi + 0x300]
            push edx
            mov edx, dword ptr [esi + 0x120]
            call MungeMaterialSV
    L00508bee:
            call SaveSpecialVolumes
            jmp L00508bfa
    L00508bf5:
            call SaveAdditionalActors
    L00508bfa:
            mov eax, dword ptr [esp + 0x10]
            mov byte ptr [ebp + gAdditional_actor_path], bl
            mov byte ptr [eax + gAdditional_model_path], bl
            call FlushAllPedCaches
            pop ebp
            pop esi
            pop edi
    L00508c12:
            pop ebx
            pop ecx
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
            sub esp, 0x10
            push ebp
            mov ebp, dword ptr [gLast_actor]
            test ebp, ebp
            je L00508e3d
            mov eax, dword ptr [gSpec_vol_mode]
            push edi
            push esi
            push ebx
            test eax, eax
            jne L00508c5b
            mov eax, dword ptr [ebp + 0x14]
            test eax, eax
            je L00508c5b
            cmp byte ptr [eax], 0x40
            jne L00508c5b
            lea edx, [esp + 0x14]
            mov ecx, ebp
            call RecenterActorModels
            mov ebp, dword ptr [gLast_actor]
    L00508c5b:
            mov eax, dword ptr [gDepth_effect_names+0x1f0]
            sub eax, 0
            je L00508c8b
            dec eax
            je L00508c7b
            dec eax
            jne L00508ca2
            add ebp, 0x2c
            push 0x3ffc
            push ebp
            call BrMatrix34PreRotateZ
            jmp L00508c99
    L00508c7b:
            add ebp, 0x2c
            push 0x3ffc
            push ebp
            call BrMatrix34PreRotateY
            jmp L00508c99
    L00508c8b:
            add ebp, 0x2c
            push 0x3ffc
            push ebp
            call BrMatrix34PreRotateX
    L00508c99:
            mov ebp, dword ptr [gLast_actor]
            add esp, 8
    L00508ca2:
            mov eax, dword ptr [gSpec_vol_mode]
            test eax, eax
            jne L00508ce2
            mov eax, dword ptr [ebp + 0x14]
            test eax, eax
            je L00508ce2
            cmp byte ptr [eax], 0x40
            jne L00508ce2
            push 0
            push offset ApplyTransToModels
            xor edx, edx
            mov ecx, ebp
            call DRActorEnumRecurseWithTrans
            mov ecx, dword ptr [gLast_actor]
            lea eax, [esp + 0x14]
            push eax
            mov edx, offset MoveModelVertices
            call DRActorEnumRecurse
            mov ebp, dword ptr [gLast_actor]
    L00508ce2:
            or ecx, 0xffffffff
            mov edi, offset gAdditional_actor_path
            xor eax, eax
            repne scasb
            not ecx
            dec ecx
            mov edi, offset gAdditional_actor_path
            mov ebx, ecx
            or ecx, 0xffffffff
            repne scasb
            not ecx
            dec ecx
            mov edi, offset gDepth_effect_names+0x230
            mov dword ptr [esp + 0x10], ecx
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            mov esi, edi
            mov edx, ecx
            mov edi, offset gAdditional_actor_path
            or ecx, 0xffffffff
            repne scasb
            mov ecx, edx
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, edx
            and ecx, 3
            rep movsb
            mov edi, offset gDepth_effect_names+0x230
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            mov esi, edi
            mov edx, ecx
            mov edi, offset gAdditional_model_path
            or ecx, 0xffffffff
            repne scasb
            mov ecx, edx
            dec edi
            shr ecx, 2
            rep movsd
            mov eax, dword ptr [gSpec_vol_mode]
            mov ecx, edx
            and ecx, 3
            test eax, eax
            rep movsb
            je L00508e1e
            test ebp, ebp
            je L00508d88
            mov edx, dword ptr [gProgram_state+0x1bfc]
            xor eax, eax
            test edx, edx
            jle L00508d88
            mov ecx, offset gSpec_vol_actors
    L00508d7c:
            cmp dword ptr [ecx], ebp
            je L00508d8b
            inc eax
            add ecx, 4
            cmp eax, edx
            jl L00508d7c
    L00508d88:
            or eax, 0xffffffff
    L00508d8b:
            test eax, eax
            jl L00508e17
            lea ecx, [eax + eax*4]
            add ebp, 0x2c
            push ebp
            lea eax, [eax + ecx*2]
            lea edx, [eax + eax*4]
            mov eax, dword ptr [gProgram_state+0x1c00]
            lea esi, [eax + edx*4]
            lea ecx, [esi + 4]
            push ecx
            call BrMatrix34Copy
            add esp, 8
            mov ecx, esi
            call FindInverseAndWorlBox
            mov eax, dword ptr [gLast_actor]
            push 1
            push 0
            mov esi, dword ptr [eax + 0x18]
            lea edi, [eax + 0x2c]
            mov ecx, dword ptr [esi + 0xc]
            mov edx, dword ptr [ecx + 0x2b0]
            push edx
            mov edx, dword ptr [ecx + 0xd0]
            mov ecx, edi
            call MungeMaterialSV
            mov eax, dword ptr [esi + 0xc]
            push 2
            push 1
            mov ecx, dword ptr [eax + 0x3a0]
            mov edx, dword ptr [eax + 0x1c0]
            push ecx
            mov ecx, edi
            call MungeMaterialSV
            mov esi, dword ptr [esi + 0xc]
            mov ecx, edi
            push 2
            push 0
            mov edx, dword ptr [esi + 0x300]
            push edx
            mov edx, dword ptr [esi + 0x120]
            call MungeMaterialSV
    L00508e17:
            call SaveSpecialVolumes
            jmp L00508e23
    L00508e1e:
            call SaveAdditionalActors
    L00508e23:
            mov eax, dword ptr [esp + 0x10]
            mov byte ptr [ebx + gAdditional_actor_path], 0
            mov byte ptr [eax + gAdditional_model_path], 0
            call FlushAllPedCaches
            pop ebx
            pop esi
            pop edi
    L00508e3d:
            pop ebp
            add esp, 0x10
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004f0f80
void C2_NAKED C2_HOOK_FASTCALL FUN_004f0f80(tNet_game_player_info* pArg1) {
#ifndef CARPOCALYPSE2_MATCHING
    tNet_message* message;
    tU8 uVar1;
    tU8 uVar2;
    int i;
    int offset;

    message = NetBuildMessage(0x21, 0);
    uVar1 = *((tU8*)message + 0x18);
    uVar2 = *((tU8*)message + 0x19);
    i = 0;
    if (0 < gCount_host_smashes) {
        offset = 0;
        do {
            memcpy((tU8*)message + 0x18, (tU8*)gNet_host_smashes + offset, 0x38);
            *((tU8*)message + 0x18) = uVar1;
            *(tU32*)((tU8*)message + 0x4c) |= 1;
            *((tU8*)message + 0x19) = uVar2;
            NetSendMessageToPlayer(gCurrent_net_game, message, *(tPlayer_ID*)((tU8*)pArg1 + 0x24));
            i++;
            offset += 0x38;
        } while (i < gCount_host_smashes);
    }
#else
    __asm {
            sub esp, 0x10
            mov dword ptr [esp + 0xc], ecx
            push ebx
            push ebp
            xor edx, edx
            mov cl, 0x21
            call NetBuildMessage
            mov ebp, eax
            mov al, byte ptr [ebp + 0x18]
            mov byte ptr [esp + 0xa], al
            xor eax, eax
            mov cl, byte ptr [ebp + 0x19]
            lea ebx, [ebp + 0x18]
            mov byte ptr [esp + 0xb], cl
            mov ecx, dword ptr [gCount_host_smashes]
            cmp ecx, eax
            mov dword ptr [esp + 0x10], eax
            jle L004f101b
            push edi
            push esi
            mov dword ptr [esp + 0x14], eax
    L004f0fbb:
            mov edx, dword ptr [gNet_host_smashes]
            mov eax, dword ptr [esp + 0x14]
            mov ecx, 0xe
            mov edi, ebx
            lea esi, [edx + eax]
            mov dl, byte ptr [esp + 0x13]
            rep movsd
            mov eax, dword ptr [ebp + 0x4c]
            mov cl, byte ptr [esp + 0x12]
            or al, 1
            mov byte ptr [ebx], cl
            mov dword ptr [ebp + 0x4c], eax
            mov eax, dword ptr [esp + 0x1c]
            mov byte ptr [ebp + 0x19], dl
            mov edx, ebp
            mov ecx, dword ptr [eax + 0x24]
            push ecx
            mov ecx, dword ptr [gCurrent_net_game]
            call NetSendMessageToPlayer
            mov ecx, dword ptr [gCount_host_smashes]
            mov eax, dword ptr [esp + 0x18]
            mov edx, dword ptr [esp + 0x14]
            inc eax
            add edx, 0x38
            cmp eax, ecx
            mov dword ptr [esp + 0x18], eax
            mov dword ptr [esp + 0x14], edx
            jl L004f0fbb
            pop esi
            pop edi
    L004f101b:
            pop ebp
            pop ebx
            add esp, 0x10
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
            nop
            nop
    }
#endif
}
