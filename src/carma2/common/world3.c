#include "world3.h"
#include "world.h"
#include "world2.h"
#include <ctype.h>
#include "loading1.h"
#include "loading.h"
#include "utility.h"
#include "fog.h"
#include "errors.h"
#include "loading3.h"
#include "sound.h"
#include "packfile.h"
#include "brucetrk.h"
#include "globvars.h"
#include "globvrpb.h"
#include "drmem.h"
#include "depth.h"
#include "flicplay.h"
#include "pedestrn.h"
#include "smashing.h"
#include "spark.h"
#include "platform.h"
#ifdef CARPOCALYPSE2_MATCHING
#include "c2_hooks.h"
#endif
#include "carpocalypse2_macros.h"
#ifdef CARPOCALYPSE2_MATCHING
#if defined(_MSC_VER)
#define C2_MATCHING_INLINE __inline
#else
#define C2_MATCHING_INLINE inline
#endif
#else
#define C2_MATCHING_INLINE
#endif

#include <string.h>
extern void C2_HOOK_FASTCALL LoadTrackMaterials(tBrender_storage* pStorage, const char* pPath);
extern void C2_HOOK_FASTCALL LoadTrackModels(tBrender_storage* pStorage, const char* pPath);

/* GLOBAL: CARMA2_HW 0x006b6400 */
extern char gAdditional_model_path[256];
/* GLOBAL: CARMA2_HW 0x006b6500 */
extern char gAdditional_actor_path[256];
/* GLOBAL: CARMA2_HW 0x006b6620 */
extern br_model* gAdditional_models[1000];
/* GLOBAL: CARMA2_HW 0x006ab948 */
extern int gNumber_of_additional_models;
/* GLOBAL: CARMA2_HW 0x006b6600 */
extern br_actor* gLast_actor;
/* GLOBAL: CARMA2_HW 0x006aaf40 */
extern tU8* gTrack_flic_buffer;
/* GLOBAL: CARMA2_HW 0x006aaf48 */
extern tU32 gTrack_flic_buffer_size;
/* GLOBAL: CARMA2_HW 0x006aaf50 */
extern tFlic_descriptor gTrack_flic_descriptor;
/* GLOBAL: CARMA2_HW 0x0074caa8 */
extern int gTrack_depth_colour_red;
/* GLOBAL: CARMA2_HW 0x0074cf2c */
extern int gTrack_depth_colour_green;
/* GLOBAL: CARMA2_HW 0x0074cad0 */
extern int gTrack_depth_colour_blue;
/* GLOBAL: CARMA2_HW 0x00660e90 */
extern const char* gDepth_effect_names[3];
/* GLOBAL: CARMA2_HW 0x0079ef40 */
extern tU8 gNon_car_spec_indices[100];
/* GLOBAL: CARMA2_HW 0x00591368 */
extern int gRendering_accessories;
// GLOBAL: CARMA2_HW 0x00762180
char gCurrent_load_directory[256];

// GLOBAL: CARMA2_HW 0x00761a80
char gCurrent_load_name[256];

// GLOBAL: CARMA2_HW 0x006b75c0
br_actor* gAdditional_actors;

// GLOBAL: CARMA2_HW 0x006aaa2c
int gDisallow_duplicates;

// GLOBAL: CARMA2_HW 0x006aaa20
br_pixelmap* gDuplicate_pixelmap;

// GLOBAL: CARMA2_HW 0x006aaa28
br_material* gDuplicate_material;

// GLOBAL: CARMA2_HW 0x006aaa24
br_model* gDuplicate_model;

// GLOBAL: CARMA2_HW 0x00660cb8
tRendererShadingType gMaterial_shading_for_callback = kRendererShadingType_Undefined;

typedef struct {
    int field_0x0;                   /* 0x21c */
    int field_0x4;                   /* 0x220 */
    int field_0x8;                   /* 0x224 */
    int quad_count;                  /* 0x228 */
    br_vector3 quad_corners[4][4];   /* 0x22c */
    br_vector3 acc;                  /* 0x31c */
    undefined pad_0x10c[8];          /* 0x328 */
} tTrack_col_vol_section;

typedef struct {
    undefined data_field_0x0[8];
    int count;                       /* 0x8 */
    undefined data_field_0xc[0x210]; /* 0xc */
    tTrack_col_vol_section sections[1]; /* 0x21c, stride 0x114 */
    undefined data_field_after[0xec4 - (0x21c + 0x114)];
    float col_vol_direction_x;
    float col_vol_direction_y;
    float col_vol_direction_z;
    float col_vol_direction_w;
} tTrack_loading_data;

tTrack_loading_data gTrack_loading_data;

extern void C2_HOOK_FASTCALL ReadGlobalLightingInfo(FILE* pF);
extern int gTrack_version;
extern tMaterial_exception* gMaterial_exceptions;

#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_CDECL BrMaterialFree();
extern br_material* C2_HOOK_CDECL BrMaterialRemove();
extern void C2_HOOK_FASTCALL RemoveMaterialFromFogification(br_material* pMaterial);
extern tBrender_storage gNet_cars_storage_space;
extern tNet_game_player_info gNet_players[12];
extern int gNumber_of_net_players;
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_FASTCALL AddFunkotronics(FILE* pF, int pOwner, int pRef_offset, tCar_crush_buffer* pCar_crush_datas);
extern void C2_HOOK_FASTCALL AddGroovidelics(FILE* pF, int pOwner, br_actor* pParent_actor, int pRef_offset, int pAllowed_to_be_absent);
extern void C2_HOOK_FASTCALL AssertFlicPixelmap(tFlic_descriptor_ptr pFlic_info, br_pixelmap* pDest_pixelmap);
extern void C2_HOOK_FASTCALL BRPM_convert(br_pixelmap* pMap, int pPixel_type);
extern br_actor* C2_HOOK_CDECL BrActorAdd();
extern br_actor* C2_HOOK_CDECL BrActorAllocate();
extern br_actor* C2_HOOK_CDECL BrActorLoad();
extern br_pixelmap* C2_HOOK_CDECL BrMapAdd();
extern br_pixelmap* C2_HOOK_CDECL BrMapFind();
extern br_pixelmap* C2_HOOK_CDECL BrMapRemove();
extern br_material* C2_HOOK_CDECL BrMaterialAdd();
extern br_material* C2_HOOK_CDECL BrMaterialFind();
extern br_uint_32 C2_HOOK_CDECL BrMaterialLoadMany();
extern void C2_HOOK_CDECL BrMaterialUpdate();
extern void C2_HOOK_CDECL BrMatrix34PostRotateX();
extern void C2_HOOK_CDECL BrMatrix34PostScale();
extern void* C2_HOOK_CDECL BrMemAllocate();
extern void* C2_HOOK_CDECL BrMemCalloc();
extern void C2_HOOK_CDECL BrMemFree();
extern br_model* C2_HOOK_CDECL BrModelAdd();
extern br_uint_32 C2_HOOK_CDECL BrModelAddMany();
extern void C2_HOOK_CDECL BrModelFree();
extern br_uint_32 C2_HOOK_CDECL BrModelLoadMany();
extern void C2_HOOK_CDECL BrModelUpdate();
extern void C2_HOOK_CDECL BrPixelmapFree();
extern br_uint_32 C2_HOOK_CDECL BrPixelmapLoadMany();
extern void* C2_HOOK_CDECL BrResAllocate();
extern br_uint_8 C2_HOOK_CDECL BrResClass();
extern void C2_HOOK_CDECL BrResFree();
extern br_uint_32 C2_HOOK_CDECL BrResSize();
extern br_pixelmap* C2_HOOK_CDECL BrTableAdd();
extern br_pixelmap* C2_HOOK_CDECL BrTableFind();
extern void C2_HOOK_FASTCALL ChangeDepthEffect(void);
extern void C2_HOOK_FASTCALL ClosePackFile(tTWTVFS twt);
extern void C2_HOOK_FASTCALL ClosePackFileAndSetTiffLoading(tTWTVFS twt);
extern intptr_t C2_HOOK_FASTCALL DRActorEnumRecurse(br_actor* pActor, br_actor_enum_cbfn* callback, void* arg);
extern br_pixelmap* C2_HOOK_FASTCALL DRPixelmapAllocate(br_uint_8 pType, br_uint_16 pW, br_uint_16 pH, void* pPixels, int pFlags);
extern int C2_HOOK_FASTCALL DRStricmp(const char* p1, const char* p2);
extern FILE* C2_HOOK_FASTCALL DRfopen(const char* pFilename, const char* pMode);
extern void C2_HOOK_FASTCALL DisposeSmashEnvNonCars(void);
extern void C2_HOOK_FASTCALL DoNotAllowOpenToFail(void);
extern void C2_HOOK_FASTCALL DodgyModelUpdate(br_model* pM);
extern void C2_HOOK_FASTCALL ExtractColumns(tTrack_spec* pTrack_spec);
extern void C2_HOOK_FASTCALL FUN_004472c0(br_model* pModel, tPMFMCB* pCallback);
extern br_material* C2_HOOK_FASTCALL FUN_00448480(void* pArg1, tU16 pIndex);
extern void C2_NORETURN C2_HOOK_CDECL FatalError();
extern void C2_HOOK_FASTCALL FinishUpLoadingPeds(void);
extern float C2_HOOK_FASTCALL GetAFloat(FILE* pF);
extern char * C2_HOOK_FASTCALL GetALineAndDontArgue(FILE* pF, char* pS);
extern int C2_HOOK_FASTCALL GetALineAndInterpretCommand(FILE* pF, const char** pString_list, int pCount);
extern void C2_HOOK_FASTCALL GetAString(FILE* pF, char* pString);
extern int C2_HOOK_FASTCALL GetAnInt(FILE* pF);
extern void C2_HOOK_FASTCALL GetFourScalars(FILE* pF, br_scalar* pS1, br_scalar* pS2, br_scalar* pS3, br_scalar* pS4);
extern void C2_HOOK_FASTCALL GetPairOfInts(FILE* pF, int* pF1, int* pF2);
extern void C2_HOOK_FASTCALL GetSmokeShadeTables(FILE* f);
extern void C2_HOOK_FASTCALL GetThreeInts(FILE* pF, int* pF1, int* pF2, int* pF3);
extern void C2_HOOK_FASTCALL GetThreeScalars(FILE* pF, br_scalar* pS1, br_scalar* pS2, br_scalar* pS3);
extern void C2_HOOK_FASTCALL GlorifyMaterial(br_material** pMaterials, int pCount, tRendererShadingType pShading);
extern void C2_HOOK_FASTCALL LoadAIWorldTrackInfo(FILE* pF);
extern void C2_HOOK_FASTCALL LoadAllImagesInDirectory(tBrender_storage* pStorage_space, const char* path);
extern void C2_HOOK_FASTCALL LoadAllShadeTablesInDirectory(tBrender_storage* pStorage, const char* pPath);
extern int C2_HOOK_FASTCALL LoadFlicData(char* pName, tU8** pData, tU32* pData_length);
extern void C2_HOOK_FASTCALL LoadInKevStuff(FILE* pF);
extern br_model* C2_HOOK_FASTCALL LoadModel(const char* pName);
extern br_material* C2_HOOK_FASTCALL LoadSingleMaterial(tBrender_storage* pStorage_space, const char* pName);
extern br_pixelmap* C2_HOOK_FASTCALL LoadSinglePixelmap(tBrender_storage* pStorage, const char* pName);
extern void C2_HOOK_FASTCALL LoadStaticLightingForRace(const char* race_lighting_path);
extern int C2_HOOK_FASTCALL LoadTextureTryAllLocations(char* pName, br_pixelmap** pMaps, int pCapacity);
extern void C2_HOOK_FASTCALL MungeForwardSky(void);
extern void C2_HOOK_FASTCALL MungeRearviewSky(void);
extern void C2_HOOK_FASTCALL MungeSmashMaterialNames(void);
extern tTWTVFS C2_HOOK_FASTCALL OpenPackFile(const char* path);
extern tTWTVFS C2_HOOK_FASTCALL OpenPackFileAndSetTiffLoading(const char* path);
extern void C2_NORETURN C2_HOOK_FASTCALL PDFatalError(const char* pThe_str);
extern int C2_HOOK_FASTCALL PFfclose(FILE* pFile);
extern void C2_HOOK_FASTCALL PackFileRerevertTiffLoading(void);
extern void C2_HOOK_FASTCALL PackFileRevertTiffLoading(void);
extern void C2_HOOK_FASTCALL PathCat(char* pDestn_str, const char* pStr_1, const char* pStr_2);
extern void C2_HOOK_FASTCALL PossibleService(void);
extern void C2_HOOK_FASTCALL PrintMemoryDump(int pFlags, char* pTitle);
extern void C2_HOOK_FASTCALL ReadNonCarMechanicsData(FILE* pF, tNon_car_spec* pNon_car_spec);
extern void C2_HOOK_FASTCALL ReadPedSpecs(FILE* pF);
extern void C2_HOOK_FASTCALL ReadSmashableEnvironment(FILE* pF, const char* pPath);
extern void C2_HOOK_FASTCALL ReadSoundGenerators(tTrack_spec* pTrack_spec, FILE* pF);
extern void C2_HOOK_FASTCALL ReadSpecialVolumes(FILE* pF);
extern intptr_t C2_HOOK_CDECL SetAccessoryRenderingCB();
extern void C2_HOOK_FASTCALL SmoothificateWorldMaterial(br_material* pMaterial);
extern int C2_HOOK_FASTCALL StartFlic(char* pFile_name, int pIndex, tFlic_descriptor_ptr pFlic_info, tU32 pSize, tS8* pData_ptr, br_pixelmap* pDest_pixelmap, int pX_offset, int pY_offset, int pFrame_rate);
extern void C2_HOOK_FASTCALL Uppercaseificate(char* dest, const char* src);
extern br_material* C2_HOOK_FASTCALL WallPerspToLinear(br_model* pModel, tU16 pFace);
extern br_material* C2_HOOK_FASTCALL WallPerspToUntex(br_model* pModel, tU16 pFace);
extern void __cdecl _chkstk();
extern void __cdecl _ftol();
extern tPath_name gApplication_path;
extern int gAusterity_mode;
extern br_pixelmap* gBack_screen;
extern int gCount_smashable_noncars;
extern int gDRFont_to_polyfont_mapping[24];
extern char gDefault_drone_direction[];
extern tSpecial_volume* gDefault_water_spec_vol_real;
extern const char* gDir_separator;
extern char gFLOAT_0058b130[];
extern char gFlic_bunch[];
extern tFrontend_spec gFrontend_QUIT;
extern int gGraf_spec_index;
extern tGraf_spec gGraf_specs[2];
extern char* gMaterial_names[2];
extern const int gMini_map_glowing_line_animation_indices[5];
extern char gNet_avail_names[];
extern tNet_mode gNet_mode;
extern char gNext_crush_actor_id[];
extern char gNon_car_indices[];
extern float gOpponent_nastyness_frigger;
extern char gPed_flags_0058f2e4[];
extern tProgram_state gProgram_state;
extern char gQuickTime_call_component_with_storage[];
extern char gRace_path[256];
extern const char* gRenderer_fixup_basename;
extern const char* gRenderer_fixup_extension;
extern tRoad_texturing_level gRoad_texturing_level;
extern br_pixelmap* gScreen;
extern br_angle gSky_image_height;
extern br_angle gSky_image_underground;
extern br_angle gSky_image_width;
extern tSmashable_environment_name* gSmashable_noncars;
extern tDepth_effect_type gSwap_depth_effect_type;
extern br_actor* gTrack_actor;
extern tBrender_storage gTrack_storage_space;
extern tTree_surgery_pass1 gTree_surgery_pass1[5];
extern int gTree_surgery_pass1_count;
extern tTree_surgery_pass2 gTree_surgery_pass2[25];
extern int gTree_surgery_pass2_count;
extern br_actor* gUniverse_actor;
extern tWall_texturing_level gWall_texturing_level;
extern br_scalar gYon_multiplier;
#endif
// FUNCTION: CARMA2_HW 0x00504b30
void C2_HOOK_FASTCALL InitTreeSurgery(void) {
    tPath_name the_path;
    FILE* file;
    int i;

    PathCat(the_path, gApplication_path, "TreeSurgery.TXT");
    file = PFfopen(the_path, "rt");
    if (file != NULL) {
        gTree_surgery_pass1_count = GetAnInt(file);
        for (i = 0; i < gTree_surgery_pass1_count; i++) {
            GetAString(file, gTree_surgery_pass1[i].name);
        }
        gTree_surgery_pass2_count = GetAnInt(file);
        for (i = 0; i < gTree_surgery_pass2_count; i++) {
            GetAString(file, gTree_surgery_pass2[i].original);
            GetAString(file, gTree_surgery_pass2[i].replacement);
        }
#ifdef CARPOCALYPSE2_FIX_BUGS
        PFfclose(file);
#endif
    } else {
        gTree_surgery_pass1_count = 0;
        gTree_surgery_pass2_count = 0;
    }
}

// ModelIsATree

// PerformTreeSurgery

// PossibleTreeSurgery

// AddExceptionToList

// LoadExceptionsFile

// LoadExceptionsFileForTrack

// FreeExceptions

// GetFaceFlag

void C2_HOOK_FASTCALL MungeTrackModel(br_model* pModel) {
    int* vertex_index_buffer;
    int (* face_index_buffer)[3];
    int i;

    vertex_index_buffer = BrMemAllocate((pModel->nvertices + 1) * sizeof(int), BR_MEMORY_APPLICATION);
    face_index_buffer = BrMemAllocate(pModel->nfaces * 3 * sizeof(int), BR_MEMORY_APPLICATION);

    for (i = 1; i < pModel->nvertices; i++) {
        if (vertex_index_buffer[i] == 0) {
            int j;

            vertex_index_buffer[i] = i;
            for (j = i + 1; j <= pModel->nvertices; j++) {
                br_vector3 d;

                BrVector3Sub(&d, &pModel->vertices[i - 1].p, &pModel->vertices[j - 1].p);
                if (BrVector3LengthSquared(&d) < 1e-8f) {
                    vertex_index_buffer[j] = i;
                }
            }
        }
    }
    for (i = 0; i < pModel->nfaces; i++) {
        int j;

        for (j = 0; j < 3; j++) {
            face_index_buffer[i][j] = vertex_index_buffer[1 + pModel->faces[i].vertices[j]];
        }
    }
    if (pModel->nfaces > 1) {
        for (i = 0; i < pModel->nfaces - 1; i++) {
            int j;

            for (j = i + 1; j < pModel->nfaces; j++) {
                int l;

                for (l = 0; l < 3; l++) {
                    int m;
                    int s1a = face_index_buffer[i][l];
                    int s1b = face_index_buffer[i][(l + 1) % 3];

                    for (m = 0; m < 3; m++) {
                        int s2a = face_index_buffer[j][m];
                        int s2b = face_index_buffer[j][(m + 1) % 3];

                        if ((s1a == s2a && s1b == s2b) || (s1a == s2b && s1b == s2a)) {
                            br_vector3 d21, o31b, o31a, c2;
                            float d;
                            int s1c, s2c;

                            pModel->faces[j].flags |= 1 << m;
                            BrVector3Sub(&d21, &pModel->vertices[s2b - 1].p, &pModel->vertices[s2a - 1].p);
                            s1c = face_index_buffer[i][(l + 2) % 3];
                            s2c = face_index_buffer[j][(m + 2) % 3];
                            BrVector3Sub(&o31b, &pModel->vertices[s2c - 1].p, &pModel->vertices[s2a - 1].p);
                            BrVector3Sub(&o31a, &pModel->vertices[s1c - 1].p, &pModel->vertices[s2c - 1].p);
                            BrVector3Cross(&c2, &o31b, &d21);
                            d = BrVector3Dot(&o31a, &c2);
                            if (d < .0001f && (((pModel->faces[i].material == NULL ||
                                                 !(pModel->faces[i].material->flags & BR_MATF_TWO_SIDED)) &&
                                                (pModel->faces[j].material == NULL ||
                                                 !(pModel->faces[j].material->flags & BR_MATF_TWO_SIDED))) ||
                                               d >= -.0001f)) {
                                pModel->faces[j].flags |= 1 << l;
                            }
                        }
                    }
                }
            }
        }
    }
    BrMemFree(vertex_index_buffer);
    BrMemFree(face_index_buffer);
    pModel->flags |= BR_MODF_UPDATEABLE;
    BrModelUpdate(pModel, BR_MODU_FACES);
}

// FUNCTION: CARMA2_HW 0x00504bf0
void C2_NAKED C2_HOOK_FASTCALL LoadTrack(const char* pFile_name, tTrack_spec* pTrack_spec, tRace_info* pRace_info) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;
    int j;
    char short_track_name[32];
    char local_directory[256];
    char local_name[256];
    char local_race_path[256];
    char race_lighting_path[256];
    char race_desc_path[256];
    char actor_path[256];
    char s[256];
    FILE* f;
    char *str;
    tTWTVFS twt, twt2;
    br_pixelmap* sky;
    tU16 sky_pixels_high;
    int killed_sky;
    int count_material_modifiers;
    int count_noncar_objects;

    PrintMemoryDump(0, "AT THE START OF LOAD TRACK");
    strcpy(gCurrent_load_directory, "RACES");
    strcpy(gCurrent_load_name, pFile_name);
    gCurrent_load_name[strlen(gCurrent_load_name) - 4] = '\0';

    strcpy(local_directory, gCurrent_load_directory);
    strcpy(local_name, gCurrent_load_name);

    PathCat(gRace_path, gApplication_path, local_directory);
    strcpy(local_race_path, gRace_path);

    LoadExceptionsFileForTrack(local_race_path);

    PathCat(gRace_path, gRace_path, local_name);
    twt = OpenPackFileAndSetTiffLoading(gRace_path);
    PathCat(race_lighting_path, gRace_path, "LIGHTING.TXT");
    PathCat(race_desc_path, gRace_path, pFile_name);
    f = DRfopen(race_desc_path, "rt");
    if (f == NULL) {
        FatalError(kFatalError_CannotOpenRacesFile);
    }

    GetALineAndDontArgue(f, s);
    str = strtok(s, "\t ,/");
    if (strcmp(str, "VERSION") == 0) {
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &gTrack_version);
        if (gTrack_version == 8) {
            gTrack_version = 0;
        }
    } else {
        gTrack_version = 0;
    }

    if (gTrack_version > 0) {
        ReadGlobalLightingInfo(f);
    }

    /* Position of centre of start of grid */
    GetThreeFloats(f,
        &pRace_info->initial_position.v[0],
        &pRace_info->initial_position.v[1],
        &pRace_info->initial_position.v[2]);

    /* Direction that grid faces in */
    pRace_info->initial_yaw = GetAFloat(f);

    /* Laps, checkpoints etc */

    /* # checkpoints */
    pRace_info->check_point_count = GetAnInt(f);
    for (i = 0; i < pRace_info->check_point_count; i++) {
        /* Checkpoint #x */
        PossibleService();

        /* Timer increment for each skill level (ped mode) */
        GetThreeInts(f,
            &pRace_info->checkpoints[i].timer_increments[0],
            &pRace_info->checkpoints[i].timer_increments[1],
            &pRace_info->checkpoints[i].timer_increments[2]);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tRace_info, checkpoints, 0x21c);

        /* # quads for this checkpoint */
        pRace_info->checkpoints[i].count_quads = GetAnInt(f);

        BrVector3Set(&pRace_info->checkpoints[i].pos, 0.f, 0.f, 0.f);

        for (j = 0; j < pRace_info->checkpoints[i].count_quads; j++) {
            br_vector3 tmp1, tmp2;
            int k;

            for (k = 0; k < 4; k++) {
                /* Point #[0-3] */
                GetThreeScalars(f,
                    &pRace_info->checkpoints[i].quads[j].points[k].v[0],
                    &pRace_info->checkpoints[i].quads[j].points[k].v[1],
                    &pRace_info->checkpoints[i].quads[j].points[k].v[2]);
                BrVector3Accumulate(&pRace_info->checkpoints[i].pos, &pRace_info->checkpoints[i].quads[j].points[k]);
            }
            BrVector3Sub(&tmp1,
                &pRace_info->checkpoints[i].quads[j].points[2],
                &pRace_info->checkpoints[i].quads[j].points[0]);
            BrVector3Sub(&tmp2,
                &pRace_info->checkpoints[i].quads[j].points[1],
                &pRace_info->checkpoints[i].quads[j].points[0]);
            BrVector3Cross(&pRace_info->checkpoints[i].normals[j], &tmp2, &tmp1);
        }
        BrVector3InvScale(&pRace_info->checkpoints[i].pos,
            &pRace_info->checkpoints[i].pos,
            (float)(4 * pRace_info->checkpoints[i].count_quads));
    }

    PossibleService();
    strcpy(short_track_name, pFile_name);
    short_track_name[4] = '\0';
    PathCat(race_desc_path, gApplication_path, "RACES");
    PathCat(race_desc_path, race_desc_path, short_track_name);
    twt2 = OpenPackFile(race_desc_path);
    LoadAllImagesInDirectory(&gTrack_storage_space, race_desc_path);
    TWT_Unmount(twt2);

    PossibleService();
    LoadAllImagesInDirectory(&gTrack_storage_space, gRace_path);

    PossibleService();
    LoadAllShadeTablesInDirectory(&gTrack_storage_space, gRace_path);

    PossibleService();
    LoadTrackMaterials(&gTrack_storage_space, gRace_path);

    PossibleService();
    LoadTrackModels(&gTrack_storage_space, gRace_path);

    for (i = 0; i < gTrack_storage_space.models_count; i++) {

        PossibleService();
        MungeTrackModel(gTrack_storage_space.models[i]);
    }
    PrintMemoryDump(0, "JUST LOADED IN TEXTURES/MATS/MODELS FOR TRACK");

    PathCat(actor_path, gRace_path, local_name);
    strcat(actor_path, ".ACT");
    pTrack_spec->the_actor = BrActorLoad(actor_path);
    PrintMemoryDump(0, "AFTER LOADING TRACK ACTORS");
    PossibleService();

    /* Smashable environment specs */
    ReadSmashableEnvironment(f, gRace_path);
    PrintMemoryDump(0, "AFTER LOADING SMASHABLE ENVIRONMENT");

    /* Ped specs */
    ReadPedSpecs(f);
    PossibleService();

    PackFileRevertTiffLoading();
    ExtractColumns(pTrack_spec);
    PackFileRerevertTiffLoading();

    FinishUpLoadingPeds();
    PrintMemoryDump(0, "JUST EXTRACTED COLUMNS AND LOADED IN PEDS");

    gTrack_actor = pTrack_spec->the_actor;
    if (!gRendering_accessories && gNet_mode == eNet_mode_none) {
        int rstyle;

        rstyle = BR_RSTYLE_NONE;
        PossibleService();
        DRActorEnumRecurse(gTrack_actor, SetAccessoryRenderingCB, &rstyle);
    }
    BrActorAdd(gUniverse_actor, pTrack_spec->the_actor);

    /* Additional actor */
    GetALineAndDontArgue(f, s);
    str = strtok(strtok(s, "\t ,/"), ".");
    strcat(str, ".DAT");
    PathCat(gAdditional_model_path, gApplication_path, "MODELS");
    PathCat(gAdditional_model_path, gAdditional_model_path, str);
    PossibleService();
    gNumber_of_additional_models = BrModelLoadMany(gAdditional_model_path, gAdditional_models, CARPOCALYPSE2_ASIZE(gAdditional_models));
    for (i = 0; i < gNumber_of_additional_models; i++) {
        gAdditional_models[i]->flags = BR_MODF_UPDATEABLE;
    }
    BrModelAddMany(gAdditional_models, gNumber_of_additional_models);
    PossibleService();

    str = strtok(strtok(s, "\t ,/"), ".");
    strcat(str, ".ACT");
    PathCat(gAdditional_actor_path, gApplication_path, "ACTORS");
    PathCat(gAdditional_actor_path, gAdditional_actor_path, str);
    gAdditional_actors = BrActorLoad(gAdditional_actor_path);
    if (gAdditional_actors == NULL) {
        gAdditional_actors = BrActorAllocate(BR_ACTOR_NONE, NULL);
    }
    BrActorAdd(gUniverse_actor, gAdditional_actors);
    gLast_actor = NULL;

    /* Name of sky texture pixelmap (or "none") */
    GetAString(f, s);
    if (!gAusterity_mode && strcmp(&s[strlen(s) - 4], ".FLI") == 0) {
        void* flic_pixels;

        gTrack_flic_buffer = NULL;
        if (!LoadFlicData(s, &gTrack_flic_buffer, &gTrack_flic_buffer_size)) {
            FatalError(kFatalError_CantFindFile_S, s);
        }
        StartFlic(s, -1, &gTrack_flic_descriptor, gTrack_flic_buffer_size, (tS8*)gTrack_flic_buffer, NULL, 0, 0, 0);
        flic_pixels = BrMemAllocate((gTrack_flic_descriptor.width + 3) & (~3) * gTrack_flic_descriptor.height, kMem_video_pixels);
        sky = DRPixelmapAllocate(gScreen->type, gTrack_flic_descriptor.width, gTrack_flic_descriptor.height, flic_pixels, 0);
        BrMapAdd(sky);
        AssertFlicPixelmap(&gTrack_flic_descriptor, sky);
    } else {
        gTrack_flic_buffer = NULL;
        sky = BrMapFind(s);
    }
    killed_sky = 0;
    if (gAusterity_mode && sky != NULL) {
        for (i = 0; i < gTrack_storage_space.pixelmaps_count; i++) {
            if (gTrack_storage_space.pixelmaps[i] == sky) {
                BrMapRemove(gTrack_storage_space.pixelmaps[i]);
                BrPixelmapFree(gTrack_storage_space.pixelmaps[i]);
                gTrack_storage_space.pixelmaps[i] = gTrack_storage_space.pixelmaps[gTrack_storage_space.pixelmaps_count - 1];
                gTrack_storage_space.pixelmaps_count--;
                break;
            }
        }
        sky = NULL;
        killed_sky = 1;
    }
    gProgram_state.default_depth_effect.sky_texture = sky;
    if (sky != NULL) {
        sky_pixels_high = gProgram_state.default_depth_effect.sky_texture->height;
    } else {
        sky_pixels_high = 100;
    }
    PossibleService();

    /* Horizontal repetitions of sky texture */
    gSky_image_width = BrDegreeToAngle(360.0 / GetAnInt(f));

    /* Vertical size of sky texture (degrees) */
    gSky_image_height = BrDegreeToAngle(GetAScalar(f));

    /* Position of horizon (pixels below top) */
    gSky_image_underground = gSky_image_height * (sky_pixels_high - GetAnInt(f)) / sky_pixels_high;

    MungeForwardSky();
    MungeRearviewSky();
    PossibleService();

    /* Depth cue mode ("none", "dark" or "fog") */
    gProgram_state.default_depth_effect.type = GetALineAndInterpretCommand(f, gDepth_effect_names, CARPOCALYPSE2_ASIZE(gDepth_effect_names));

    /* Degree of fog/darkness */
    GetPairOfInts(f,
        &gProgram_state.default_depth_effect.start,
        &gProgram_state.default_depth_effect.end);

    /* Depth cue colour (red, green, blue ) */
    GetThreeInts(f,
        &gProgram_state.default_depth_effect.colour.red,
        &gProgram_state.default_depth_effect.colour.green,
        &gProgram_state.default_depth_effect.colour.blue);
    gTrack_depth_colour_red = gProgram_state.default_depth_effect.colour.red;
    gTrack_depth_colour_green = gProgram_state.default_depth_effect.colour.green;
    gTrack_depth_colour_blue = gProgram_state.default_depth_effect.colour.blue;
    ChangeDepthEffect();

    if (killed_sky && gProgram_state.default_depth_effect.type != eDepth_effect_fog) {
        gProgram_state.default_depth_effect.type = eDepth_effect_fog;
        gProgram_state.default_depth_effect.start = 7;
        gProgram_state.default_depth_effect.end = 0;
        gProgram_state.default_depth_effect.colour.red = 0xf8;
        gProgram_state.default_depth_effect.colour.green = 0xf8;
        gProgram_state.default_depth_effect.colour.blue = 0xf8;
    }
    PossibleService();
    ReadSpecialVolumes(f);
    ReadSoundGenerators(pTrack_spec, f);

    /* REFLECTIVE WINDSCREEN SPECIFICATIONS */

    /* Material to use for default screens */
    GetAString(f, s);
    gProgram_state.standard_screen = BrMapFind(s);

    /* Material to use for default screens during darkness */
    GetAString(f, s);
    gProgram_state.standard_screen_dark = BrMapFind(s);

    /* Material to use for default screens during fog */
    GetAString(f, s);
    gProgram_state.standard_screen_fog = BrMapFind(s);

    /* (ignore) # areas with different screens */
    gProgram_state.special_screens_count = GetAnInt(f);
    if (gProgram_state.special_screens_count != 0) {
        gProgram_state.special_screens = BrMemAllocate(sizeof(tSpecial_screen) * gProgram_state.special_screens_count, kMem_special_screen);
        C2_HOOK_BUG_ON(sizeof(tSpecial_screen) != 20);
        for (i = 0; i < gProgram_state.special_screens_count; i++) {
            GetFourScalars(f,
                &gProgram_state.special_screens[i].min_x,
                &gProgram_state.special_screens[i].min_z,
                &gProgram_state.special_screens[i].max_x,
                &gProgram_state.special_screens[i].max_z);
            GetAString(f, s);
            gProgram_state.special_screens[i].material = BrMaterialFind(s);
        }
    }

    PossibleService();

    /* MAP DETAILS */

    /* Map pixelmap name */
    GetAString(f, s);
    pRace_info->map_image = BrMapFind(s);
    if (pRace_info->map_image == NULL) {
        strtok(s, ".");
        pRace_info->map_image = BrMapFind(s);
        if (pRace_info->map_image == NULL) {
            Uppercaseificate(s, s);
            pRace_info->map_image = BrMapFind(s);
        }
    }
    BRPM_convert(pRace_info->map_image, gBack_screen->type);
    PrintMemoryDump(0, "JUST LOADING SKY/SPEC VOLS/SCREENS/MAP");
    /* World->map transformation matrix */
    for (i = 0; i < 4; ++i) {
        GetThreeScalars(f,
            &pRace_info->map_transformation.m[i][0],
            &pRace_info->map_transformation.m[i][1],
            &pRace_info->map_transformation.m[i][2]);
    }
    if (gGraf_specs[gGraf_spec_index].total_width != 640 || gGraf_specs[gGraf_spec_index].total_height != 480) {
        pRace_info->map_transformation.m[3][0] -= 320.f;
        pRace_info->map_transformation.m[3][1] -= 240.f;
        BrMatrix34PostRotateX(&pRace_info->map_transformation, BrDegreeToAngle(90));
        BrMatrix34PostScale(&pRace_info->map_transformation,
            (float)gGraf_specs[gGraf_spec_index].total_width / (float)640,
            0.f,
            (float)gGraf_specs[gGraf_spec_index].total_height / (float)480);
        BrMatrix34PostRotateX(&pRace_info->map_transformation, BrDegreeToAngle(270));
        pRace_info->map_transformation.m[3][0] += (float)gGraf_specs[gGraf_spec_index].total_width / 2;
        pRace_info->map_transformation.m[3][1] += (float)gGraf_specs[gGraf_spec_index].total_height / 2;
    }

    /* ****** START OF FUNK AND GROOVE STUFF ****** */

    /* START OF FUNK */
    GetALineAndDontArgue(f, s);
    AddFunkotronics(f, -2, 30 * GROOVE_FUNK_MAX_PER_CAR, NULL);

    /* FIXME: What is magic 1080 / 30 * 30 * GROOVE_FUNK_MAX_PER_CAR ?*/
    C2_HOOK_BUG_ON(30 * GROOVE_FUNK_MAX_PER_CAR != 1080);

    /* START OF GROOVE */
    GetALineAndDontArgue(f, s);
    C2_HOOK_ASSERT(strcmp(s, "START OF GROOVE") == 0);

    AddGroovidelics(f, -2, gUniverse_actor, 30 * GROOVE_FUNK_MAX_PER_CAR, 0);

    PossibleService();
    PrintMemoryDump(0, "JUST LOADING IN FUNKS AND GROOVES");

    for (i = 0; i < gTrack_storage_space.models_count; i++) {
        br_model* model;

        PossibleService();
        model = gTrack_storage_space.models[i];
        PossibleTreeSurgery(model);
        if (model != NULL && (model->flags & (BR_MODF_KEEP_ORIGINAL | BR_MODF_UPDATEABLE))) {
            if (model->identifier != NULL && model->identifier[0] != '-') {
                model->flags &= ~(BR_MODF_KEEP_ORIGINAL | BR_MODF_UPDATEABLE);
            }
            for (j = 0; j < V11MODEL(model)->ngroups; j++) {
                v11group* v11_group = &V11MODEL(model)->groups[j];
                v11_group->face_colours.material = model->faces[*v11_group->face_user].material;
                if (v11_group->face_colours.material != NULL) {
                    SmoothificateWorldMaterial(v11_group->face_colours.material);
                    if (v11_group->face_colours.material->index_shade == NULL) {
                        v11_group->face_colours.material->index_shade = BrTableFind("DRRENDER.TAB");
                        BrMaterialUpdate(v11_group->face_colours.material, BR_MATU_ALL);
                    }
                }
            }
            if (model->flags & BR_MODF_UPDATEABLE) {
                BrModelUpdate(model, BR_MODU_ALL);
            } else {
                DodgyModelUpdate(model);
            }
        }
    }
    PossibleService();

    PrintMemoryDump(0, "JUST ABOUT TO LOAD IN AI WORLD");
    LoadAIWorldTrackInfo(f);
    PrintMemoryDump(0, "JUST LOADED IN AI WORLD");
    MungeSmashMaterialNames();
    PackFileRevertTiffLoading();

    /* number of material modifiers */
    count_material_modifiers = GetAnInt(f);

    for (i = 0; i < count_material_modifiers; i++) {
        tMaterial_modifiers* modifier = &pRace_info->material_modifiers[i];

        /* car wall friction */
        modifier->car_wall_friction = GetAFloat(f);

        /* tyre road friction */
        modifier->tyre_road_friction = GetAFloat(f);

        /* down force */
        modifier->down_force = GetAFloat(f);

        /* bumpiness */
        modifier->bumpiness = GetAFloat(f);

        /* tyre sound index */
        modifier->tyre_noise_index = GetAnInt(f);

        /* crash sound index */
        modifier->crash_noise_index = GetAnInt(f);

        /* scrape noise index */
        modifier->scrape_noise_index = GetAnInt(f);

        /* sparkiness */
        modifier->sparkiness = GetAFloat(f);

        /* room for expansion */
        modifier->smoke_type = GetAnInt(f);

        /* skid mark material */
        GetAString(f, s);
        str = strtok(s, ".");
        if (strcmp(str, "NONE") == 0) {
            modifier->skid_mark_material = NULL;
        } else if (strcmp(str, "0") == 0) {
            modifier->skid_mark_material = NULL;
        } else if (strcmp(str, "1") == 0) {
            modifier->skid_mark_material = NULL;
        } else {
            strcat(str, ".PIX");
            LoadSinglePixelmap(&gTrack_storage_space, str);
            str[strlen(str) - 4] = '\0';
            strcat(str, ".MAT");
            modifier->skid_mark_material = LoadSingleMaterial(&gTrack_storage_space, str);
        }
    }
    PackFileRerevertTiffLoading();
    for (i = count_material_modifiers; i < CARPOCALYPSE2_ASIZE(pRace_info->material_modifiers) - 1; i++) {
        tMaterial_modifiers* modifier = &pRace_info->material_modifiers[i];

        modifier->car_wall_friction = 1.f;
        modifier->tyre_road_friction = 1.f;
        modifier->down_force = 1.f;
        modifier->bumpiness = 0.f;
        modifier->tyre_noise_index = 0;
        modifier->crash_noise_index = 0;
        modifier->scrape_noise_index = 0;
        modifier->sparkiness = 1.f;
        modifier->smoke_type = 1;
        modifier->skid_mark_material = NULL;
    }
    pRace_info->material_modifiers[CARPOCALYPSE2_ASIZE(pRace_info->material_modifiers) - 1].car_wall_friction = 1.f;
    pRace_info->material_modifiers[CARPOCALYPSE2_ASIZE(pRace_info->material_modifiers) - 1].tyre_road_friction = 1.f;
    pRace_info->material_modifiers[CARPOCALYPSE2_ASIZE(pRace_info->material_modifiers) - 1].down_force = 0.f;
    pRace_info->material_modifiers[CARPOCALYPSE2_ASIZE(pRace_info->material_modifiers) - 1].bumpiness = 0.f;
    pRace_info->material_modifiers[CARPOCALYPSE2_ASIZE(pRace_info->material_modifiers) - 1].tyre_noise_index = -1;
    pRace_info->material_modifiers[CARPOCALYPSE2_ASIZE(pRace_info->material_modifiers) - 1].crash_noise_index = 0;
    pRace_info->material_modifiers[CARPOCALYPSE2_ASIZE(pRace_info->material_modifiers) - 1].scrape_noise_index = 0;
    pRace_info->material_modifiers[CARPOCALYPSE2_ASIZE(pRace_info->material_modifiers) - 1].sparkiness = 0.f;
    pRace_info->material_modifiers[CARPOCALYPSE2_ASIZE(pRace_info->material_modifiers) - 1].smoke_type = 1;
    pRace_info->material_modifiers[CARPOCALYPSE2_ASIZE(pRace_info->material_modifiers) - 1].skid_mark_material = NULL;

    gDefault_water_spec_vol_real->material_modifier_index = 10;

    /* Non CarObjects */
    count_noncar_objects = GetAnInt(f);
    if (count_noncar_objects > 40) {
 /* FIXME: magic number */
        PDFatalError("Too many non-car types");
    }
    gProgram_state.num_non_car_spaces = count_noncar_objects + gCount_smashable_noncars + 40;
    gProgram_state.non_cars = BrMemCalloc(gProgram_state.num_non_car_spaces, sizeof(tNon_car_spec), kMem_non_car_spec);
    C2_HOOK_BUG_ON(sizeof(tNon_car_spec) != 260);
    if (gProgram_state.non_cars == NULL && count_noncar_objects != 0) {
        FatalError(kFatalError_CannotOpenRacesFile);
    }
    memset(gNon_car_spec_indices, 0, sizeof(gNon_car_spec_indices));
    C2_HOOK_BUG_ON(sizeof(gNon_car_spec_indices) != 100);
    for (i = 0; i < 40; i++) {
 /* FIXME: magic */
        tPhysics_object *collision_info;
        tNon_car_spec* non_car;

        non_car = &gProgram_state.non_cars[i];
        non_car->driver = eDriver_non_car_unused_slot;
        collision_info = BrMemAllocate(sizeof(tPhysics_object), kMem_collision_object);
        C2_HOOK_BUG_ON(sizeof(tPhysics_object) != 1240);
        non_car->collision_info = collision_info;
        collision_info->owner = non_car;
        collision_info->flags_0x238 = 1;
        collision_info->field_0x1a0 = 0xffff;
        collision_info->field_0x1a4 = 0;
    }
    for (i = 0; i < count_noncar_objects + gCount_smashable_noncars; i++) {
        tPhysics_object* collision_info;
        tNon_car_spec* non_car;
        char non_cars_path[256];
        FILE* g;

        PossibleService();
        non_car = &gProgram_state.non_cars[40 + i]; /* FIXME: magic number */
        collision_info = BrMemAllocate(sizeof(tPhysics_object), kMem_collision_object);
        C2_HOOK_BUG_ON(sizeof(tPhysics_object) != 1240);
        non_car->collision_info = collision_info;
        collision_info->owner = non_car;
        collision_info->flags_0x238 = 1;
        collision_info->field_0x1a0 = 0xffff;
        collision_info->field_0x1a4 = 0;

        if (i < count_noncar_objects) {
            GetAString(f, s);
        } else {
            strcpy(s, gSmashable_noncars[i - count_noncar_objects]);
        }
        PathCat(non_cars_path, gApplication_path, "NONCARS");
        PathCat(non_cars_path, non_cars_path, s);
        g = DRfopen(non_cars_path, "rt");
        if (g == NULL) {
            FatalError(kFatalError_CantOpen_S, non_cars_path);
        }
        ReadNonCarMechanicsData(g, non_car);
        PossibleService();
        gNon_car_spec_indices[non_car->index] = i + 1;
        PFfclose(g);
    }
    DisposeSmashEnvNonCars();
    CheckNonCarModelBounds();
    GetSmokeShadeTables(f);
    pRace_info->count_network_start_points = GetAnInt(f);
    for (i = 0; i < pRace_info->count_network_start_points; i++) {
        GetThreeScalars(f,
            &pRace_info->net_starts[i].pos.v[0],
            &pRace_info->net_starts[i].pos.v[1],
            &pRace_info->net_starts[i].pos.v[2]);
        pRace_info->net_starts[i].yaw = GetAFloat(f);
    }
    LoadInKevStuff(f);
    gYon_multiplier = GetAFloat(f);
    GetAString(f, s);
    if (DRStricmp(s, pFile_name) != 0) {
        FatalError(kFatalError_FileIsCorrupted_S, pFile_name);
    }
    PFfclose(f);
    FreeExceptions();
    PrintMemoryDump(0, "FINISHED LOADING TRACK");
    LoadStaticLightingForRace(race_lighting_path);
    ClosePackFileAndSetTiffLoading(twt);
#else
    __asm {
            sub esp, 0xaa8
            push ebx
            push ebp
            mov dword ptr [esp + 0x1c], edx
            mov ebx, ecx
            push esi
            push edi
            mov edx, offset gDepth_effect_names+0x1d4
            xor ecx, ecx
            mov dword ptr [esp + 0x5c], ebx
            mov dword ptr [esp + 0x1c], 0
            call PrintMemoryDump
            mov eax, dword ptr [gDepth_effect_names+0x1cc]
            mov cx, word ptr [gDepth_effect_names+0x1d0]
            mov word ptr [gCurrent_load_directory+0x4], cx
            mov dword ptr [gCurrent_load_directory], eax
            or ecx, 0xffffffff
            mov edi, ebx
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
            lea edx, [esp + 0x6b8]
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
            lea edx, [esp + 0x4b8]
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
            lea ecx, [esp + 0x6b8]
            push ecx
            mov ecx, offset gRace_path
            call PathCat
            mov edi, offset gRace_path
            or ecx, 0xffffffff
            xor eax, eax
            lea edx, [esp + 0x7b8]
            repne scasb
            not ecx
            sub edi, ecx
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            mov edx, dword ptr [gRenderer_fixup_basename]
            shr ecx, 2
            rep movsd
            mov ecx, eax
            mov eax, dword ptr [gDir_separator]
            and ecx, 3
            rep movsb
            mov ecx, dword ptr [gRenderer_fixup_extension]
            push ecx
            push edx
            lea ecx, [esp + 0x7c0]
            push eax
            push ecx
            lea edx, [esp + 0x3c8]
            push offset gDepth_effect_names+0x1c0
            push edx
            call sprintf
            mov eax, dword ptr [gNet_avail_names+0x4e4]
            add esp, 0x18
            mov edx, offset gNext_crush_actor_id+0x180
            lea ecx, [esp + 0x3b8]
            mov dword ptr [esp + 0x14], eax
            call DRfopen
            mov ebp, eax
            test ebp, ebp
            je L00504f71
            lea edx, [esp + 0x2b8]
            mov ecx, ebp
            call GetALineAndDontArgue
            lea ecx, [esp + 0x14]
            lea edx, [esp + 0x2b8]
            push ecx
            push edx
            call strtok
            add esp, 8
            mov edx, offset gDefault_drone_direction+0x5dc
            mov ecx, eax
            call DRStricmp
            test eax, eax
            je L00504d9f
            lea eax, [esp + 0x3b8]
            push offset gDefault_drone_direction+0x5dc
            push eax
            push 0xb0
            call FatalError
            add esp, 0xc
    L00504d9f:
            lea ecx, [esp + 0x14]
            push ecx
            push 0
            call strtok
            add esp, 8
            lea edx, [esp + 0x10]
            mov esi, eax
            push edx
            push offset gPed_flags_0058f2e4+0x134
            push esi
            call sscanf
            add esp, 0xc
            test eax, eax
            je L00504dce
            cmp dword ptr [esp + 0x10], 1
            je L00504de4
    L00504dce:
            lea eax, [esp + 0x3b8]
            push eax
            push esi
            push 0xb1
            call FatalError
            add esp, 0xc
    L00504de4:
            lea edx, [esp + 0x2b8]
            mov ecx, ebp
            call GetALineAndDontArgue
            lea ecx, [esp + 0x14]
            lea edx, [esp + 0x2b8]
            push ecx
            push edx
            call strtok
            mov esi, eax
            add esp, 8
            mov edx, offset gDepth_effect_names+0x1bc
            mov ecx, esi
            call DRStricmp
            test eax, eax
            je L00504f6a
    L00504e1d:
            push 0xfe
            push 0xc
            call BrMemAllocate
            mov ebx, eax
            mov edi, esi
            or ecx, 0xffffffff
            xor eax, eax
            add esp, 8
            repne scasb
            not ecx
            push 0xab
            push ecx
            call BrMemAllocate
            mov edx, eax
            mov edi, esi
            or ecx, 0xffffffff
            xor eax, eax
            add esp, 8
            mov dword ptr [ebx + 4], edx
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
            mov dword ptr [ebx + 8], 0
            push ecx
            push 0
            call strtok
            mov esi, eax
            add esp, 8
            test esi, esi
            je L00504f1f
            mov edi, 1
    L00504e90:
            cmp dword ptr [gQuickTime_call_component_with_storage+0x5a0], edi
            jle L00504eab
            movsx edx, byte ptr [esi]
            push 0x107
            push edx
            call _isctype
            add esp, 8
            jmp L00504ebd
    L00504eab:
            movsx eax, byte ptr [esi]
            mov ecx, dword ptr [gQuickTime_call_component_with_storage+0x394]
            mov ax, word ptr [ecx + eax*2]
            and eax, 0x107
    L00504ebd:
            test eax, eax
            je L00504f1f
            mov edx, offset gDepth_effect_names+0x1b0
            mov ecx, esi
            call DRStricmp
            test eax, eax
            jne L00504ed6
            or dword ptr [ebx + 8], edi
            jmp L00504f06
    L00504ed6:
            mov edx, offset gDepth_effect_names+0x1a8
            mov ecx, esi
            call DRStricmp
            test eax, eax
            jne L00504ef0
            mov eax, dword ptr [ebx + 8]
            or al, 8
            mov dword ptr [ebx + 8], eax
            jmp L00504f06
    L00504ef0:
            lea edx, [esp + 0x3b8]
            push edx
            push esi
            push 0xb2
            call FatalError
            add esp, 0xc
    L00504f06:
            lea eax, [esp + 0x14]
            push eax
            push 0
            call strtok
            mov esi, eax
            add esp, 8
            test esi, esi
            jne L00504e90
    L00504f1f:
            mov ecx, dword ptr [gMaterial_exceptions]
            lea edx, [esp + 0x2b8]
            mov dword ptr [ebx], ecx
            mov ecx, ebp
            mov dword ptr [gMaterial_exceptions], ebx
            call GetALineAndDontArgue
            lea edx, [esp + 0x14]
            lea eax, [esp + 0x2b8]
            push edx
            push eax
            call strtok
            mov esi, eax
            add esp, 8
            mov edx, offset gDepth_effect_names+0x1bc
            mov ecx, esi
            call DRStricmp
            test eax, eax
            jne L00504e1d
            mov ebx, dword ptr [esp + 0x5c]
    L00504f6a:
            mov ecx, ebp
            call PFfclose
    L00504f71:
            lea ecx, [esp + 0x4b8]
            mov edx, offset gRace_path
            push ecx
            mov ecx, edx
            call PathCat
            mov ecx, offset gRace_path
            call OpenPackFileAndSetTiffLoading
            mov edx, offset gRace_path
            lea ecx, [esp + 0x8b8]
            push offset gDepth_effect_names+0x198
            mov dword ptr [esp + 0x74], eax
            call PathCat
            mov edx, offset gRace_path
            lea ecx, [esp + 0x1b8]
            push ebx
            call PathCat
            mov edx, offset gNext_crush_actor_id+0x180
            lea ecx, [esp + 0x1b8]
            call DRfopen
            mov edi, eax
            test edi, edi
            mov dword ptr [esp + 0x18], edi
            jne L00504fe0
            push 0x32
            call FatalError
            add esp, 4
    L00504fe0:
            lea edx, [esp + 0x98]
            mov ecx, edi
            call GetALineAndDontArgue
            lea edx, [esp + 0x98]
            push offset gPed_flags_0058f2e4+0x138
            push edx
            call strtok
            add esp, 8
            mov esi, offset gDefault_drone_direction+0x5dc
            lea eax, [esp + 0x98]
    L0050500f:
            mov dl, byte ptr [eax]
            mov bl, byte ptr [esi]
            mov cl, dl
            cmp dl, bl
            jne L00505037
            test cl, cl
            je L00505033
            mov dl, byte ptr [eax + 1]
            mov bl, byte ptr [esi + 1]
            mov cl, dl
            cmp dl, bl
            jne L00505037
            add eax, 2
            add esi, 2
            test cl, cl
            jne L0050500f
    L00505033:
            xor eax, eax
            jmp L0050503c
    L00505037:
            sbb eax, eax
            sbb eax, -1
    L0050503c:
            test eax, eax
            jne L0050506b
            push offset gPed_flags_0058f2e4+0x138
            push eax
            call strtok
            add esp, 8
            push offset gTrack_version
            push offset gPed_flags_0058f2e4+0x134
            push eax
            call sscanf
            mov eax, dword ptr [gTrack_version]
            add esp, 0xc
            cmp eax, 8
            jne L00505072
    L0050506b:
            xor eax, eax
            mov dword ptr [gTrack_version], eax
    L00505072:
            test eax, eax
            jle L0050507d
            mov ecx, edi
            call ReadGlobalLightingInfo
    L0050507d:
            lea edx, [esp + 0x98]
            mov ecx, edi
            call GetALineAndDontArgue
            lea eax, [esp + 0x98]
            push offset gPed_flags_0058f2e4+0x138
            push eax
            call strtok
            add esp, 8
            lea ecx, [esp + 0x38]
            push ecx
            push offset gMini_map_glowing_line_animation_indices+0x7dc
            push eax
            call sscanf
            fld dword ptr [esp + 0x44]
            mov esi, dword ptr [esp + 0xac8]
            add esp, 0xc
            fstp dword ptr [esi + 0xec4]
            push offset gPed_flags_0058f2e4+0x138
            push 0
            call strtok
            add esp, 8
            lea edx, [esp + 0x38]
            push edx
            push offset gMini_map_glowing_line_animation_indices+0x7dc
            push eax
            call sscanf
            mov eax, dword ptr [esp + 0x44]
            add esp, 0xc
            mov dword ptr [esi + 0xec8], eax
            push offset gPed_flags_0058f2e4+0x138
            push 0
            call strtok
            add esp, 8
            lea ecx, [esp + 0x38]
            push ecx
            push offset gMini_map_glowing_line_animation_indices+0x7dc
            push eax
            call sscanf
            mov edx, dword ptr [esp + 0x44]
            add esp, 0xc
            mov dword ptr [esi + 0xecc], edx
            call PossibleService
            lea edx, [esp + 0x98]
            mov ecx, edi
            call GetALineAndDontArgue
            lea eax, [esp + 0x98]
            push offset gPed_flags_0058f2e4+0x138
            push eax
            call strtok
            add esp, 8
            lea ecx, [esp + 0x38]
            push ecx
            push offset gMini_map_glowing_line_animation_indices+0x7dc
            push eax
            call sscanf
            mov edx, dword ptr [esp + 0x44]
            add esp, 0xc
            mov dword ptr [esi + 0xed0], edx
            call PossibleService
            mov ecx, edi
            call GetAnInt
            test eax, eax
            mov dword ptr [esi + 8], eax
            mov dword ptr [esp + 0x14], 0
            jle L00505391
            lea ebx, [esi + 0x31c]
            add esi, 0x2f0
            mov dword ptr [esp + 0x34], 0
            mov dword ptr [esp + 0x30], esi
    L00505198:
            call PossibleService
            mov ecx, dword ptr [esp + 0x18]
            lea esi, [ebx - 0xfc]
            lea eax, [esi + 4]
            lea edx, [esi - 4]
            push eax
            push esi
            call GetThreeInts
            lea edi, [ebx + 4]
            lea ebp, [ebx + 8]
            mov ecx, dword ptr [esp + 0x18]
            mov dword ptr [ebx], 0
            mov dword ptr [edi], 0
            mov dword ptr [esp + 0x3c], edi
            mov dword ptr [esp + 0x44], ebp
            mov dword ptr [ebp], 0
            call GetAnInt
            lea ecx, [ebx - 0xf4]
            mov dword ptr [esp + 0x20], 0
            test eax, eax
            mov dword ptr [esp + 0x10], ecx
            mov dword ptr [ecx], eax
            jle L0050531b
            mov ecx, dword ptr [esp + 0x30]
            add esi, 0x1c
            mov dword ptr [esp + 0x2c], ecx
    L00505205:
            lea edi, [esi - 0xc]
            mov dword ptr [esp + 0x40], 4
    L00505210:
            mov ecx, dword ptr [esp + 0x18]
            lea ebp, [edi + 4]
            push ebp
            lea edx, [edi - 4]
            push edi
            call GetThreeScalars
            _emit 0xd9
        _emit 0x03
            fadd dword ptr [edi - 4]
            mov eax, dword ptr [esp + 0x3c]
            add edi, 0xc
            _emit 0xd9
        _emit 0x1b
            fld dword ptr [edi - 0xc]
            _emit 0xd8
        _emit 0x00
            _emit 0xd9
        _emit 0x18
            mov eax, dword ptr [esp + 0x44]
            _emit 0xd9
        _emit 0x45
        _emit 0x00
            _emit 0xd8
        _emit 0x00
            _emit 0xd9
        _emit 0x18
            mov eax, dword ptr [esp + 0x40]
            dec eax
            mov dword ptr [esp + 0x40], eax
            jne L00505210
            fld dword ptr [esi + 0xc]
            fld dword ptr [esi + 0x10]
            fld dword ptr [esi + 8]
            _emit 0xd9
        _emit 0x06
            fld dword ptr [esi + 4]
            fxch st(4)
            fsub dword ptr [esi - 0xc]
            fld dword ptr [esi - 4]
            fsub dword ptr [esi - 0x10]
            mov ecx, dword ptr [esp + 0x2c]
            mov edx, dword ptr [esp + 0x20]
            mov eax, dword ptr [esp + 0x34]
            mov edi, dword ptr [esp + 0xabc]
            add ecx, 0xc
            add esi, 0x30
            fstp dword ptr [esp + 0x4c]
            fxch st(3)
            fsub dword ptr [esi - 0x38]
            fxch st(2)
            fsub dword ptr [esi - 0x40]
            fxch st(1)
            fsub dword ptr [esi - 0x3c]
            fxch st(4)
            fsub dword ptr [esi - 0x38]
            fxch st(3)
            fst dword ptr [esp + 0x64]
            fld st(2)
            fld st(3)
            fxch st(6)
            fstp dword ptr [esp + 0x50]
            fmul dword ptr [esp + 0x50]
            fxch st(4)
            fstp dword ptr [esp + 0x54]
            fmul dword ptr [esp + 0x54]
            fxch st(1)
            fst dword ptr [esp + 0x60]
            fld dword ptr [esp + 0x60]
            fxch st(1)
            fmul dword ptr [esp + 0x54]
            fxch st(1)
            fmul dword ptr [esp + 0x50]
            fxch st(2)
            fsubp st(4), st(0)
            fxch st(3)
            fstp dword ptr [ecx - 0x10]
            fld dword ptr [esp + 0x64]
            fxch st(4)
            fmul dword ptr [esp + 0x4c]
            fxch st(4)
            fmul dword ptr [esp + 0x4c]
            fxch st(4)
            fsubp st(3), st(0)
            lea eax, [edx + eax + 0x3f]
            mov dword ptr [esp + 0x2c], ecx
            inc edx
            fsubp st(3), st(0)
            fxch st(1)
            fstp dword ptr [ecx - 0xc]
            mov ecx, dword ptr [esp + 0x10]
            lea eax, [eax + eax*2]
            fxch st(1)
            fstp dword ptr [edi + eax*4]
            mov eax, dword ptr [ecx]
            mov dword ptr [esp + 0x20], edx
            cmp edx, eax
            fstp st(0)
            jl L00505205
            mov ebp, dword ptr [esp + 0x44]
            mov edi, dword ptr [esp + 0x3c]
    L0050531b:
            mov edx, dword ptr [ecx]
            mov eax, dword ptr [esp + 0x34]
            _emit 0xd9
        _emit 0x03
            shl edx, 2
            mov dword ptr [esp + 0x10], edx
            add eax, 0x17
            fild dword ptr [esp + 0x10]
            mov dword ptr [esp + 0x34], eax
            mov eax, dword ptr [esp + 0x30]
            fdivp st(1), st(0)
            add eax, 0x114
            mov edx, dword ptr [esp + 0xabc]
            mov dword ptr [esp + 0x30], eax
            add ebx, 0x114
            fstp dword ptr [ebx - 0x114]
            mov eax, dword ptr [ecx]
            _emit 0xd9
        _emit 0x07
            shl eax, 2
            mov dword ptr [esp + 0x10], eax
            mov eax, dword ptr [esp + 0x14]
            fild dword ptr [esp + 0x10]
            fdivp st(1), st(0)
            _emit 0xd9
        _emit 0x1f
            mov ecx, dword ptr [ecx]
            _emit 0xd9
        _emit 0x45
        _emit 0x00
            shl ecx, 2
            mov dword ptr [esp + 0x10], ecx
            inc eax
            fild dword ptr [esp + 0x10]
            mov dword ptr [esp + 0x14], eax
            fdivp st(1), st(0)
            _emit 0xd9
        _emit 0x5d
        _emit 0x00
            cmp eax, dword ptr [edx + 8]
            jl L00505198
    L00505391:
            call PossibleService
            mov edi, dword ptr [esp + 0x5c]
            or ecx, 0xffffffff
            xor eax, eax
            lea edx, [esp + 0x98]
            repne scasb
            not ecx
            sub edi, ecx
            push offset gDepth_effect_names+0x1cc
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            mov edx, offset gApplication_path
            shr ecx, 2
            rep movsd
            mov ecx, eax
            and ecx, 3
            rep movsb
            lea ecx, [esp + 0x1bc]
            mov byte ptr [esp + 0xa0], 0
            call PathCat
            lea ecx, [esp + 0x98]
            lea edx, [esp + 0x1b8]
            push ecx
            lea ecx, [esp + 0x1bc]
            call PathCat
            lea ecx, [esp + 0x1b8]
            call OpenPackFile
            lea edx, [esp + 0x1b8]
            mov ecx, offset gTrack_storage_space
            mov esi, eax
            call LoadAllImagesInDirectory
            mov ecx, esi
            call ClosePackFile
            call PossibleService
            mov edx, offset gRace_path
            mov ecx, offset gTrack_storage_space
            call LoadAllImagesInDirectory
            call PossibleService
            mov edx, offset gRace_path
            mov ecx, offset gTrack_storage_space
            call LoadAllShadeTablesInDirectory
            call PossibleService
            mov edx, offset gRace_path
            mov ecx, offset gTrack_storage_space
            call LoadTrackMaterials
            call PossibleService
            mov edx, offset gRace_path
            mov ecx, offset gTrack_storage_space
            call LoadTrackModels
            mov eax, dword ptr [gTrack_storage_space+0x4]
            mov dword ptr [esp + 0x14], 0
            test eax, eax
            jle L00505980
    L00505482:
            call PossibleService
            mov edx, dword ptr [gTrack_storage_space+0x2c]
            mov eax, dword ptr [esp + 0x14]
            xor ecx, ecx
            push 0x80
            mov ebx, dword ptr [edx + eax*4]
            mov dword ptr [esp + 0x2c], ebx
            mov cx, word ptr [ebx + 0x10]
            lea edx, [ecx*4 + 4]
            push edx
            call BrMemAllocate
            mov dword ptr [esp + 0x48], eax
            mov esi, dword ptr [ebx + 8]
            xor eax, eax
            add esp, 8
            mov ax, word ptr [ebx + 0x12]
            sub esi, 0x28
            push 0x80
            lea eax, [eax + eax*2]
            shl eax, 2
            push eax
            call BrMemAllocate
            mov edi, 1
            add esp, 8
            cmp word ptr [ebx + 0x10], di
            mov ebp, eax
            mov dword ptr [esp + 0x3c], ebp
            jb L00505599
            mov ecx, dword ptr [esp + 0x40]
            lea ebx, [esi + 0x2c]
            lea eax, [ecx + 4]
            mov dword ptr [esp + 0x20], eax
    L005054fa:
            cmp dword ptr [eax], 0
            jne L0050557a
            mov esi, dword ptr [esp + 0x28]
            xor edx, edx
            mov dword ptr [eax], edi
            lea ecx, [edi + 1]
            mov dx, word ptr [esi + 0x10]
            cmp ecx, edx
            jg L0050557a
            mov ebp, dword ptr [esp + 0x28]
            lea esi, [eax + 4]
            lea edx, [ebx + 0x2c]
    L0050551c:
            _emit 0xd9
        _emit 0x03
            fld dword ptr [ebx - 4]
            fsub dword ptr [edx - 8]
            fxch st(1)
            fsub dword ptr [edx - 4]
            fld dword ptr [ebx + 4]
            _emit 0xd8
        _emit 0x22
            fxch st(2)
            fst dword ptr [esp + 0x60]
            fxch st(1)
            fst dword ptr [esp + 0x64]
            fxch st(1)
            fmul dword ptr [esp + 0x60]
            fxch st(1)
            fmul dword ptr [esp + 0x64]
            fld st(2)
            fmul st(0), st(3)
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fcomp dword ptr [gFLOAT_0058b130+0x60]
            fnstsw ax
            test ah, 1
            fstp st(0)
            je L00505561
            mov dword ptr [esi], edi
    L00505561:
            xor eax, eax
            inc ecx
            mov ax, word ptr [ebp + 0x10]
            add edx, 0x28
            add esi, 4
            cmp ecx, eax
            jle L0050551c
            mov ebp, dword ptr [esp + 0x3c]
            mov eax, dword ptr [esp + 0x20]
    L0050557a:
            mov edx, dword ptr [esp + 0x28]
            xor ecx, ecx
            inc edi
            add eax, 4
            mov cx, word ptr [edx + 0x10]
            add ebx, 0x28
            cmp edi, ecx
            mov dword ptr [esp + 0x20], eax
            jle L005054fa
            mov ebx, edx
    L00505599:
            xor ecx, ecx
            cmp word ptr [ebx + 0x12], cx
            mov dword ptr [esp + 0x20], ecx
            jbe L005055eb
            mov dword ptr [esp + 0x2c], ecx
            mov eax, ebp
    L005055ab:
            mov edx, 3
    L005055b0:
            mov esi, dword ptr [ebx + 0xc]
            xor edi, edi
            add eax, 4
            mov di, word ptr [ecx + esi]
            mov esi, dword ptr [esp + 0x40]
            add ecx, 2
            dec edx
            mov esi, dword ptr [esi + edi*4 + 4]
            mov dword ptr [eax - 4], esi
            jne L005055b0
            mov edx, dword ptr [esp + 0x20]
            mov ecx, dword ptr [esp + 0x2c]
            xor esi, esi
            inc edx
            mov si, word ptr [ebx + 0x12]
            add ecx, 0x28
            cmp edx, esi
            mov dword ptr [esp + 0x20], edx
            mov dword ptr [esp + 0x2c], ecx
            jl L005055ab
    L005055eb:
            xor eax, eax
            mov ax, word ptr [ebx + 0x12]
            lea ecx, [eax - 1]
            test ecx, ecx
            jle L00505941
            xor edi, edi
            xor ebx, ebx
            mov ecx, 1
            mov dword ptr [esp + 0x20], edi
            mov dword ptr [esp + 0x2c], ebx
            mov dword ptr [esp + 0x10], ecx
    L00505611:
            cmp ecx, eax
            mov dword ptr [esp + 0x48], ecx
            jge L00505912
            add edi, 0x28
            lea edx, [ebx + 3]
            mov dword ptr [esp + 0x34], edi
            mov dword ptr [esp + 0x30], edx
    L0050562b:
            xor eax, eax
            mov dword ptr [esp + 0x44], eax
    L00505631:
            lea edx, [eax + ebx]
            xor ecx, ecx
            inc eax
            mov esi, 3
            lea edx, [ebp + edx*4]
            mov dword ptr [esp + 0x74], eax
            mov dword ptr [esp + 0x7c], edx
            cdq
            idiv esi
            add edx, ebx
            lea eax, [ebp + edx*4]
            mov dword ptr [esp + 0x80], eax
    L00505658:
            mov edx, dword ptr [esp + 0x7c]
            mov eax, dword ptr [esp + 0x30]
            mov esi, dword ptr [edx]
            lea edx, [ecx + eax]
            mov eax, dword ptr [esp + 0x80]
            mov edi, dword ptr [ebp + edx*4]
            mov ebp, 3
            mov ebx, dword ptr [eax]
            lea eax, [ecx + 1]
            mov dword ptr [esp + 0x78], eax
            cdq
            idiv ebp
            mov ebp, dword ptr [esp + 0x30]
            add edx, ebp
            mov ebp, dword ptr [esp + 0x3c]
            cmp esi, edi
            mov eax, dword ptr [ebp + edx*4]
            mov dword ptr [esp + 0x58], eax
            jne L0050569b
            cmp ebx, eax
            je L005056ab
    L0050569b:
            cmp esi, eax
            jne L005058b7
            cmp edi, ebx
            jne L005058b7
    L005056ab:
            mov eax, ecx
            sub eax, 0
            je L005056ca
            dec eax
            je L005056c3
            dec eax
            je L005056bc
            xor ebx, ebx
            jmp L005056cf
    L005056bc:
            mov ebx, 4
            jmp L005056cf
    L005056c3:
            mov ebx, 2
            jmp L005056cf
    L005056ca:
            mov ebx, 1
    L005056cf:
            mov edx, dword ptr [esp + 0x44]
            mov esi, 3
            add ecx, 2
            lea edi, [edi + edi*4]
            lea eax, [edx + 2]
            mov ebp, dword ptr [esp + 0x3c]
            cdq
            idiv esi
            mov eax, ecx
            mov ecx, esi
            mov esi, dword ptr [esp + 0x34]
            mov dword ptr [esp + 0x84], ecx
            mov ecx, dword ptr [esp + 0x28]
            mov ecx, dword ptr [ecx + 0xc]
            lea esi, [esi + ecx + 0x10]
            mov cl, byte ptr [esi]
            or cl, bl
            mov byte ptr [esi], cl
            mov ecx, dword ptr [esp + 0x28]
            mov ecx, dword ptr [ecx + 8]
            mov dword ptr [esp + 0x6c], ecx
            mov ecx, dword ptr [esp + 0x58]
            mov esi, dword ptr [esp + 0x6c]
            lea ebx, [ecx + ecx*4]
            lea ecx, [esi + edi*8]
            fld dword ptr [esi + ebx*8 - 0x28]
            fld dword ptr [esi + ebx*8 - 0x24]
            fld dword ptr [esi + ebx*8 - 0x20]
            fxch st(2)
            fsub dword ptr [ecx - 0x28]
            fxch st(1)
            fsub dword ptr [ecx - 0x24]
            fxch st(2)
            fsub dword ptr [ecx - 0x20]
            fxch st(1)
            fstp dword ptr [esp + 0x4c]
            lea edi, [esi + ebx*8]
            mov ebx, dword ptr [esp + 0x84]
            mov edi, edx
            cdq
            fxch st(1)
            fstp dword ptr [esp + 0x50]
            idiv ebx
            fstp dword ptr [esp + 0x54]
            mov ebx, dword ptr [esp + 0x2c]
            add edi, ebx
            mov eax, dword ptr [ebp + edi*4]
            lea eax, [eax + eax*4]
            fld dword ptr [esi + eax*8 - 0x24]
            fld dword ptr [esi + eax*8 - 0x28]
            fld dword ptr [esi + eax*8 - 0x20]
            lea eax, [esi + eax*8]
            add edx, dword ptr [esp + 0x30]
            mov eax, dword ptr [ebp + edx*4]
            lea edx, [eax + eax*4]
            fld dword ptr [esi + edx*8 - 0x24]
            fld dword ptr [esi + edx*8 - 0x28]
            fsub dword ptr [ecx - 0x28]
            lea eax, [esi + edx*8]
            fst dword ptr [esp + 0x8c]
            fxch st(1)
            fsub dword ptr [ecx - 0x24]
            fst dword ptr [esp + 0x90]
            fld dword ptr [eax - 0x20]
            fsub dword ptr [ecx - 0x20]
            fld st(0)
            fmul dword ptr [esp + 0x4c]
            fxch st(3)
            fmul dword ptr [esp + 0x54]
            fsubp st(3), st(0)
            fld st(0)
            fxch st(6)
            fsub dword ptr [eax - 0x24]
            fmulp st(3), st(0)
            fxch st(1)
            fmul dword ptr [esp + 0x54]
            fxch st(5)
            fmul dword ptr [esp + 0x50]
            mov edi, dword ptr [esp + 0x20]
            fsubp st(5), st(0)
            fxch st(3)
            fsub dword ptr [eax - 0x28]
            fld dword ptr [esp + 0x8c]
            fld dword ptr [esp + 0x90]
            fxch st(2)
            fmulp st(6), st(0)
            fxch st(5)
            faddp st(2), st(0)
            fxch st(4)
            fmul dword ptr [esp + 0x50]
            fxch st(2)
            fsub dword ptr [eax - 0x20]
            fxch st(4)
            fmul dword ptr [esp + 0x4c]
            mov eax, dword ptr [esp + 0x28]
            fsubp st(2), st(0)
            fxch st(3)
            fmulp st(1), st(0)
            mov ecx, dword ptr [eax + 0xc]
            mov eax, dword ptr [esp + 0x34]
            faddp st(2), st(0)
            fxch st(1)
            mov esi, dword ptr [eax + ecx + 8]
            mov edx, dword ptr [edi + ecx + 8]
            fstp dword ptr [esp + 0x58]
            fstp st(0)
            fld dword ptr [esp + 0x58]
            fcomp qword ptr [gFLOAT_0058b130+0x68]
            fnstsw ax
            test ah, 1
            je L005058bf
            test edx, edx
            je L00505842
            mov eax, dword ptr [edx + 0x20]
            test ah, 0x10
            jne L0050584e
    L00505842:
            test esi, esi
            je L0050585f
            mov eax, dword ptr [esi + 0x20]
            test ah, 0x10
            je L0050585f
    L0050584e:
            fld dword ptr [esp + 0x58]
            fcomp qword ptr [gFLOAT_0058b130+0x70]
            fnstsw ax
            test ah, 1
            jne L005058bf
    L0050585f:
            mov eax, dword ptr [esp + 0x44]
            sub eax, 0
            je L005058a4
            dec eax
            je L00505891
            dec eax
            je L0050587e
            mov dl, byte ptr [edi + ecx + 0x10]
            lea ecx, [edi + ecx + 0x10]
            xor eax, eax
            or dl, al
            mov byte ptr [ecx], dl
            jmp L005058bf
    L0050587e:
            mov dl, byte ptr [edi + ecx + 0x10]
            lea ecx, [edi + ecx + 0x10]
            mov eax, 4
            or dl, al
            mov byte ptr [ecx], dl
            jmp L005058bf
    L00505891:
            mov dl, byte ptr [edi + ecx + 0x10]
            lea ecx, [edi + ecx + 0x10]
            mov eax, 2
            or dl, al
            mov byte ptr [ecx], dl
            jmp L005058bf
    L005058a4:
            mov dl, byte ptr [edi + ecx + 0x10]
            lea ecx, [edi + ecx + 0x10]
            mov eax, 1
            or dl, al
            mov byte ptr [ecx], dl
            jmp L005058bf
    L005058b7:
            mov edi, dword ptr [esp + 0x20]
            mov ebx, dword ptr [esp + 0x2c]
    L005058bf:
            mov ecx, dword ptr [esp + 0x78]
            cmp ecx, 3
            jl L00505658
            mov eax, dword ptr [esp + 0x74]
            cmp eax, 3
            mov dword ptr [esp + 0x44], eax
            jl L00505631
            mov ecx, dword ptr [esp + 0x30]
            mov edx, dword ptr [esp + 0x28]
            mov eax, dword ptr [esp + 0x48]
            mov esi, dword ptr [esp + 0x34]
            add ecx, 3
            inc eax
            mov dword ptr [esp + 0x30], ecx
            xor ecx, ecx
            mov cx, word ptr [edx + 0x12]
            add esi, 0x28
            cmp eax, ecx
            mov dword ptr [esp + 0x48], eax
            mov dword ptr [esp + 0x34], esi
            jl L0050562b
            mov ecx, dword ptr [esp + 0x10]
    L00505912:
            mov edx, dword ptr [esp + 0x28]
            xor eax, eax
            inc ecx
            add ebx, 3
            mov ax, word ptr [edx + 0x12]
            add edi, 0x28
            lea edx, [ecx - 1]
            mov dword ptr [esp + 0x10], ecx
            lea esi, [eax - 1]
            mov dword ptr [esp + 0x2c], ebx
            cmp edx, esi
            mov dword ptr [esp + 0x20], edi
            jl L00505611
            mov ebx, dword ptr [esp + 0x28]
    L00505941:
            push ebp
            call BrMemFree
            mov eax, dword ptr [esp + 0x44]
            add esp, 4
            push eax
            call BrMemFree
            or byte ptr [ebx + 0x20], 0x80
            add esp, 4
            push 0x80
            push ebx
            call BrModelUpdate
            mov eax, dword ptr [esp + 0x1c]
            mov ecx, dword ptr [gTrack_storage_space+0x4]
            add esp, 8
            inc eax
            cmp eax, ecx
            mov dword ptr [esp + 0x14], eax
            jl L00505482
    L00505980:
            mov edx, offset gDepth_effect_names+0x168
            xor ecx, ecx
            call PrintMemoryDump
            lea ecx, [esp + 0x4b8]
            mov edx, offset gRace_path
            push ecx
            lea ecx, [esp + 0x5bc]
            call PathCat
            mov edi, offset gDefault_drone_direction+0x6dc
            or ecx, 0xffffffff
            xor eax, eax
            lea edx, [esp + 0x5b8]
            repne scasb
            not ecx
            sub edi, ecx
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
            lea eax, [esp + 0x5b8]
            and ecx, 3
            push eax
            rep movsb
            call BrActorLoad
            mov esi, dword ptr [esp + 0x28]
            add esp, 4
            mov edx, offset gDepth_effect_names+0x14c
            xor ecx, ecx
            mov dword ptr [esi + 0x14], eax
            call PrintMemoryDump
            call PossibleService
            mov ebp, dword ptr [esp + 0x18]
            mov edx, offset gRace_path
            mov ecx, ebp
            call ReadSmashableEnvironment
            mov edx, offset gDepth_effect_names+0x128
            xor ecx, ecx
            call PrintMemoryDump
            mov ecx, ebp
            call ReadPedSpecs
            call PossibleService
            call PackFileRevertTiffLoading
            mov ecx, esi
            call ExtractColumns
            call PackFileRerevertTiffLoading
            call FinishUpLoadingPeds
            mov edx, offset gDepth_effect_names+0xfc
            xor ecx, ecx
            call PrintMemoryDump
            mov eax, dword ptr [gRendering_accessories]
            mov ecx, dword ptr [esi + 0x14]
            test eax, eax
            mov dword ptr [gTrack_actor], ecx
            jne L00505a8b
            mov eax, dword ptr [gNet_mode]
            test eax, eax
            jne L00505a8b
            mov dword ptr [esp + 0x88], 1
            call PossibleService
            mov ecx, dword ptr [gTrack_actor]
            lea edx, [esp + 0x88]
            push edx
            mov edx, offset SetAccessoryRenderingCB
            call DRActorEnumRecurse
    L00505a8b:
            mov eax, dword ptr [esp + 0x24]
            mov edx, dword ptr [gUniverse_actor]
            mov ecx, dword ptr [eax + 0x14]
            push ecx
            push edx
            call BrActorAdd
            add esp, 8
            lea edx, [esp + 0x98]
            mov ecx, ebp
            call GetALineAndDontArgue
            lea eax, [esp + 0x98]
            push offset gPed_flags_0058f2e4+0x138
            push eax
            call strtok
            add esp, 8
            push offset gPed_flags_0058f2e4+0x178
            push eax
            call strtok
            mov ebx, eax
            mov edi, offset gFrontend_QUIT+0xbb7c
            or ecx, 0xffffffff
            xor eax, eax
            add esp, 8
            repne scasb
            not ecx
            sub edi, ecx
            push offset gNet_avail_names+0x4ac
            mov esi, edi
            mov edx, ecx
            mov edi, ebx
            or ecx, 0xffffffff
            repne scasb
            mov ecx, edx
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, edx
            mov edx, offset gApplication_path
            and ecx, 3
            rep movsb
            mov ecx, offset gAdditional_model_path
            call PathCat
            mov edx, offset gAdditional_model_path
            push ebx
            mov ecx, edx
            call PathCat
            call PossibleService
            push 0x3e8
            push offset gAdditional_models
            push offset gAdditional_model_path
            call BrModelLoadMany
            add esp, 0xc
            xor ecx, ecx
            test eax, eax
            mov dword ptr [gNumber_of_additional_models], eax
            jle L00505b64
            mov edx, offset gAdditional_models
            mov esi, 0x80
    L00505b51:
            mov eax, dword ptr [edx]
            inc ecx
            add edx, 4
            mov word ptr [eax + 0x20], si
            mov eax, dword ptr [gNumber_of_additional_models]
            cmp ecx, eax
            jl L00505b51
    L00505b64:
            push eax
            push offset gAdditional_models
            call BrModelAddMany
            add esp, 8
            call PossibleService
            lea ecx, [esp + 0x98]
            push offset gPed_flags_0058f2e4+0x138
            push ecx
            call strtok
            add esp, 8
            push offset gPed_flags_0058f2e4+0x178
            push eax
            call strtok
            mov ebx, eax
            mov edi, offset gDefault_drone_direction+0x6dc
            or ecx, 0xffffffff
            xor eax, eax
            add esp, 8
            repne scasb
            not ecx
            sub edi, ecx
            push offset gNet_avail_names+0x4a4
            mov esi, edi
            mov edx, ecx
            mov edi, ebx
            or ecx, 0xffffffff
            repne scasb
            mov ecx, edx
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, edx
            mov edx, offset gApplication_path
            and ecx, 3
            rep movsb
            mov ecx, offset gAdditional_actor_path
            call PathCat
            mov edx, offset gAdditional_actor_path
            push ebx
            mov ecx, edx
            call PathCat
            push offset gAdditional_actor_path
            call BrActorLoad
            add esp, 4
            mov dword ptr [gAdditional_actors], eax
            test eax, eax
            jne L00505c10
            xor ebx, ebx
            push ebx
            push ebx
            call BrActorAllocate
            add esp, 8
            mov dword ptr [gAdditional_actors], eax
            jmp L00505c12
    L00505c10:
            xor ebx, ebx
    L00505c12:
            push eax
            mov eax, dword ptr [gUniverse_actor]
            push eax
            call BrActorAdd
            add esp, 8
            lea edx, [esp + 0x98]
            mov ecx, ebp
            mov dword ptr [gLast_actor], ebx
            call GetAString
            cmp dword ptr [gAusterity_mode], ebx
            jne L00505d41
            lea edi, [esp + 0x98]
            or ecx, 0xffffffff
            xor eax, eax
            lea edx, [esp + 0x98]
            repne scasb
            not ecx
            dec ecx
            sub edx, 4
            mov esi, offset gFlic_bunch+0x10fc
            add ecx, edx
    L00505c63:
            mov dl, byte ptr [ecx]
            mov al, dl
            cmp dl, byte ptr [esi]
            jne L00505c87
            test al, al
            je L00505c83
            mov dl, byte ptr [ecx + 1]
            mov al, dl
            cmp dl, byte ptr [esi + 1]
            jne L00505c87
            add ecx, 2
            add esi, 2
            test al, al
            jne L00505c63
    L00505c83:
            xor eax, eax
            jmp L00505c8c
    L00505c87:
            sbb eax, eax
            sbb eax, -1
    L00505c8c:
            cmp eax, ebx
            jne L00505d41
            push offset gTrack_flic_buffer_size
            mov edx, offset gTrack_flic_buffer
            lea ecx, [esp + 0x9c]
            mov dword ptr [gTrack_flic_buffer], ebx
            call LoadFlicData
            test eax, eax
            jne L00505cc6
            lea eax, [esp + 0x98]
            push eax
            push 0x75
            call FatalError
            add esp, 8
    L00505cc6:
            mov ecx, dword ptr [gTrack_flic_buffer]
            mov edx, dword ptr [gTrack_flic_buffer_size]
            push ebx
            push ebx
            push ebx
            push ebx
            push ecx
            push edx
            push offset gTrack_flic_descriptor
            or edx, 0xffffffff
            lea ecx, [esp + 0xb4]
            call StartFlic
            mov eax, dword ptr [gTrack_flic_descriptor+0x44]
            add eax, 3
            push 0xcd
            and al, 0xfc
            imul eax, dword ptr [gTrack_flic_descriptor+0x48]
            push eax
            call BrMemAllocate
            mov cx, word ptr [gTrack_flic_descriptor+0x48]
            add esp, 8
            mov edx, dword ptr [gTrack_flic_descriptor+0x44]
            push ebx
            push eax
            mov eax, dword ptr [gScreen]
            push ecx
            mov cl, byte ptr [eax + 0x2c]
            call DRPixelmapAllocate
            mov esi, eax
            push esi
            call BrMapAdd
            add esp, 4
            mov edx, esi
            mov ecx, offset gTrack_flic_descriptor
            call AssertFlicPixelmap
            jmp L00505d59
    L00505d41:
            lea ecx, [esp + 0x98]
            mov dword ptr [gTrack_flic_buffer], ebx
            push ecx
            call BrMapFind
            add esp, 4
            mov esi, eax
    L00505d59:
            cmp dword ptr [gAusterity_mode], ebx
            je L00505dd8
            cmp esi, ebx
            je L00505dd8
            mov edx, dword ptr [gTrack_storage_space+0x10]
            xor eax, eax
            cmp edx, ebx
            jle L00505dcf
            mov edi, dword ptr [gTrack_storage_space+0x30]
            mov ecx, edi
    L00505d79:
            cmp dword ptr [ecx], esi
            je L00505d8e
            inc eax
            add ecx, 4
            cmp eax, edx
            jl L00505d79
            xor esi, esi
            mov edi, 1
            jmp L00505ddc
    L00505d8e:
            mov edx, dword ptr [edi + eax*4]
            lea esi, [eax*4]
            push edx
            call BrMapRemove
            mov eax, dword ptr [gTrack_storage_space+0x30]
            add esp, 4
            mov ecx, dword ptr [eax + esi]
            push ecx
            call BrPixelmapFree
            mov eax, dword ptr [gTrack_storage_space+0x30]
            mov edx, dword ptr [gTrack_storage_space+0x10]
            add esp, 4
            mov ecx, dword ptr [eax + edx*4 - 4]
            mov dword ptr [eax + esi], ecx
            mov eax, dword ptr [gTrack_storage_space+0x10]
            dec eax
            mov dword ptr [gTrack_storage_space+0x10], eax
    L00505dcf:
            xor esi, esi
            mov edi, 1
            jmp L00505ddc
    L00505dd8:
            mov edi, dword ptr [esp + 0x1c]
    L00505ddc:
            cmp esi, ebx
            mov dword ptr [gProgram_state+0x1bdc], esi
            je L00505dec
            mov si, word ptr [esi + 0x36]
            jmp L00505df1
    L00505dec:
            mov esi, 0x64
    L00505df1:
            call PossibleService
            mov ecx, ebp
            call GetAnInt
            mov dword ptr [esp + 0x1c], eax
            fild dword ptr [esp + 0x1c]
            fdivr dword ptr [gFLOAT_0058b130+0x78]
            fmul qword ptr [gFLOAT_0058b130+0x80]
            call _ftol
            mov ecx, ebp
            mov word ptr [gSky_image_width], ax
            call GetAFloat
            fmul qword ptr [gFLOAT_0058b130+0x80]
            call _ftol
            mov ecx, ebp
            mov word ptr [gSky_image_height], ax
            and esi, 0xffff
            call GetAnInt
            mov edx, eax
            mov eax, esi
            xor ecx, ecx
            sub eax, edx
            mov cx, word ptr [gSky_image_height]
            imul eax, ecx
            cdq
            idiv esi
            mov word ptr [gSky_image_underground], ax
            call MungeForwardSky
            call MungeRearviewSky
            call PossibleService
            mov edx, offset gDepth_effect_names
            mov ecx, ebp
            push 3
            call GetALineAndInterpretCommand
            mov edx, offset gProgram_state+0x1bc8
            mov ecx, ebp
            push offset gProgram_state+0x1bcc
            mov dword ptr [gProgram_state+0x1bc4], eax
            call GetPairOfInts
            mov edx, offset gProgram_state+0x1bd0
            mov ecx, ebp
            push offset gProgram_state+0x1bd8
            push offset gProgram_state+0x1bd4
            call GetThreeInts
            mov edx, dword ptr [gProgram_state+0x1bd0]
            mov eax, dword ptr [gProgram_state+0x1bd4]
            mov ecx, dword ptr [gProgram_state+0x1bd8]
            mov dword ptr [gTrack_depth_colour_red], edx
            mov dword ptr [gTrack_depth_colour_green], eax
            mov dword ptr [gTrack_depth_colour_blue], ecx
            call ChangeDepthEffect
            cmp edi, ebx
            je L00505f08
            mov ecx, dword ptr [gProgram_state+0x1bc4]
            mov eax, 1
            cmp ecx, eax
            je L00505f08
            mov dword ptr [gProgram_state+0x1bc4], eax
            mov eax, 0xf8
            mov dword ptr [gProgram_state+0x1bc8], 7
            mov dword ptr [gProgram_state+0x1bcc], ebx
            mov dword ptr [gProgram_state+0x1bd0], eax
            mov dword ptr [gProgram_state+0x1bd4], eax
            mov dword ptr [gProgram_state+0x1bd8], eax
    L00505f08:
            call PossibleService
            mov ecx, ebp
            call ReadSpecialVolumes
            mov ecx, dword ptr [esp + 0x24]
            mov edx, ebp
            call ReadSoundGenerators
            lea edx, [esp + 0x98]
            mov ecx, ebp
            call GetAString
            lea edx, [esp + 0x98]
            push edx
            call BrMapFind
            add esp, 4
            lea edx, [esp + 0x98]
            mov ecx, ebp
            mov dword ptr [gProgram_state+0x1c0c], eax
            call GetAString
            lea eax, [esp + 0x98]
            push eax
            call BrMapFind
            add esp, 4
            lea edx, [esp + 0x98]
            mov ecx, ebp
            mov dword ptr [gProgram_state+0x1c10], eax
            call GetAString
            lea ecx, [esp + 0x98]
            push ecx
            call BrMapFind
            add esp, 4
            mov ecx, ebp
            mov dword ptr [gProgram_state+0x1c14], eax
            call GetAnInt
            cmp eax, ebx
            mov dword ptr [gProgram_state+0x1c18], eax
            je L0050601c
            lea edx, [eax + eax*4]
            push 0xd3
            shl edx, 2
            push edx
            call BrMemAllocate
            mov ecx, dword ptr [gProgram_state+0x1c18]
            add esp, 8
            xor edi, edi
            cmp ecx, ebx
            mov dword ptr [gProgram_state+0x1c1c], eax
            jle L0050601c
            xor esi, esi
            jmp L00505fca
    L00505fc5:
            mov eax, dword ptr [gProgram_state+0x1c1c]
    L00505fca:
            mov ebx, dword ptr [esp + 0x18]
            lea ecx, [eax + esi + 0x10]
            push ecx
            lea edx, [eax + esi + 0xc]
            lea ecx, [eax + esi + 8]
            push edx
            push ecx
            lea edx, [eax + esi + 4]
            mov ecx, ebx
            call GetFourScalars
            lea edx, [esp + 0x98]
            mov ecx, ebx
            call GetAString
            lea edx, [esp + 0x98]
            push edx
            call BrMaterialFind
            mov ecx, dword ptr [gProgram_state+0x1c1c]
            add esp, 4
            inc edi
            mov dword ptr [ecx + esi], eax
            mov eax, dword ptr [gProgram_state+0x1c18]
            add esi, 0x14
            cmp edi, eax
            jl L00505fc5
    L0050601c:
            call PossibleService
            mov edi, dword ptr [esp + 0x18]
            lea edx, [esp + 0x98]
            mov ecx, edi
            call GetAString
            lea edx, [esp + 0x98]
            push edx
            call BrMapFind
            mov ebx, dword ptr [esp + 0xac0]
            add esp, 4
            test eax, eax
            mov dword ptr [ebx + 0xf04], eax
            jne L005060ac
            lea eax, [esp + 0x98]
            push offset gPed_flags_0058f2e4+0x178
            push eax
            call strtok
            add esp, 8
            lea ecx, [esp + 0x98]
            push ecx
            call BrMapFind
            add esp, 4
            mov dword ptr [ebx + 0xf04], eax
            test eax, eax
            jne L005060ac
            lea edx, [esp + 0x98]
            lea ecx, [esp + 0x98]
            call Uppercaseificate
            lea edx, [esp + 0x98]
            push edx
            call BrMapFind
            add esp, 4
            mov dword ptr [ebx + 0xf04], eax
    L005060ac:
            mov eax, dword ptr [gBack_screen]
            mov ecx, dword ptr [ebx + 0xf04]
            xor edx, edx
            mov dl, byte ptr [eax + 0x2c]
            call BRPM_convert
            mov edx, offset gDepth_effect_names+0xd4
            xor ecx, ecx
            call PrintMemoryDump
            lea esi, [ebx + 0xed8]
            mov ebp, 4
    L005060d8:
            lea ecx, [esi + 4]
            lea edx, [esi - 4]
            push ecx
            push esi
            mov ecx, edi
            call GetThreeScalars
            add esi, 0xc
            dec ebp
            jne L005060d8
            mov eax, dword ptr [gGraf_spec_index]
            lea edx, [eax + eax*2]
            lea eax, [eax + edx*4]
            shl eax, 2
            cmp dword ptr [eax + gGraf_specs+0xc], 0x280
            jne L00506117
            cmp dword ptr [eax + gGraf_specs+0x10], 0x1e0
            je L005061ef
    L00506117:
            fld dword ptr [ebx + 0xef8]
            fsub dword ptr [gFLOAT_0058b130+0x88]
            lea esi, [ebx + 0xed4]
            push 0x3ffc
            push esi
            fstp dword ptr [ebx + 0xef8]
            fld dword ptr [ebx + 0xefc]
            fsub dword ptr [gFLOAT_0058b130+0x8c]
            fstp dword ptr [ebx + 0xefc]
            call BrMatrix34PostRotateX
            mov eax, dword ptr [gGraf_spec_index]
            add esp, 8
            xor ebp, ebp
            lea ecx, [eax + eax*2]
            push ecx
            lea eax, [eax + ecx*4]
            shl eax, 2
            fild dword ptr [eax + gGraf_specs+0x10]
            fmul dword ptr [gFLOAT_0058b130+0x90]
            _emit 0xd9
        _emit 0x1c
        _emit 0x24
            fild dword ptr [eax + gGraf_specs+0xc]
            push ebp
            push ecx
            fmul dword ptr [gFLOAT_0058b130+0x94]
            _emit 0xd9
        _emit 0x1c
        _emit 0x24
            push esi
            call BrMatrix34PostScale
            add esp, 0x10
            push 0xc004
            push esi
            call BrMatrix34PostRotateX
            mov eax, dword ptr [gGraf_spec_index]
            add esp, 8
            lea edx, [eax + eax*2]
            lea eax, [eax + edx*4]
            mov eax, dword ptr [eax*4 + gGraf_specs+0xc]
            cdq
            sub eax, edx
            sar eax, 1
            mov dword ptr [esp + 0x1c], eax
            fild dword ptr [esp + 0x1c]
            fadd dword ptr [ebx + 0xef8]
            fstp dword ptr [ebx + 0xef8]
            mov eax, dword ptr [gGraf_spec_index]
            lea ecx, [eax + eax*2]
            lea edx, [eax + ecx*4]
            mov eax, dword ptr [edx*4 + gGraf_specs+0x10]
            cdq
            sub eax, edx
            sar eax, 1
            mov dword ptr [esp + 0x1c], eax
            fild dword ptr [esp + 0x1c]
            fadd dword ptr [ebx + 0xefc]
            fstp dword ptr [ebx + 0xefc]
            jmp L005061f1
    L005061ef:
            xor ebp, ebp
    L005061f1:
            lea edx, [esp + 0x98]
            mov ecx, edi
            call GetALineAndDontArgue
            mov edx, 0xfffffffe
            mov ecx, edi
            push ebp
            push 0x438
            call AddFunkotronics
            lea edx, [esp + 0x98]
            mov ecx, edi
            call GetALineAndDontArgue
            mov eax, dword ptr [gUniverse_actor]
            mov edx, 0xfffffffe
            push ebp
            push 0x438
            push eax
            mov ecx, edi
            call AddGroovidelics
            call PossibleService
            mov edx, offset gDepth_effect_names+0xb0
            xor ecx, ecx
            call PrintMemoryDump
            mov eax, dword ptr [gTrack_storage_space+0x4]
            mov dword ptr [esp + 0x14], ebp
            cmp eax, ebp
            jle L00506522
            jmp L0050625d
    L0050625b:
            xor ebp, ebp
    L0050625d:
            call PossibleService
            mov ecx, dword ptr [gTrack_storage_space+0x2c]
            mov edx, dword ptr [esp + 0x14]
            mov ebx, dword ptr [ecx + edx*4]
            cmp ebx, ebp
            je L00506420
            mov ebp, dword ptr [ebx + 4]
            test ebp, ebp
            mov dword ptr [esp + 0x1c], ebp
            je L00506420
            mov eax, dword ptr [gTree_surgery_pass1_count]
            xor esi, esi
            test eax, eax
            jle L005062b1
            mov edi, offset gTree_surgery_pass1
    L00506296:
            push edi
            push ebp
            call strstr
            add esp, 8
            cmp eax, ebp
            je L005062b1
            mov eax, dword ptr [gTree_surgery_pass1_count]
            inc esi
            add edi, 8
            cmp esi, eax
            jl L00506296
    L005062b1:
            cmp esi, dword ptr [gTree_surgery_pass1_count]
            je L00506420
            mov eax, dword ptr [gTree_surgery_pass2_count]
            xor edi, edi
            test eax, eax
            jle L00506420
            mov ebp, offset gTree_surgery_pass2
    L005062d1:
            mov eax, dword ptr [esp + 0x1c]
            mov esi, ebp
    L005062d7:
            mov dl, byte ptr [eax]
            mov cl, dl
            cmp dl, byte ptr [esi]
            jne L005062fb
            test cl, cl
            je L005062f7
            mov dl, byte ptr [eax + 1]
            mov cl, dl
            cmp dl, byte ptr [esi + 1]
            jne L005062fb
            add eax, 2
            add esi, 2
            test cl, cl
            jne L005062d7
    L005062f7:
            xor eax, eax
            jmp L00506300
    L005062fb:
            sbb eax, eax
            sbb eax, -1
    L00506300:
            test eax, eax
            je L00506316
            mov eax, dword ptr [gTree_surgery_pass2_count]
            inc edi
            add ebp, 0x30
            cmp edi, eax
            jl L005062d1
            jmp L00506420
    L00506316:
            lea edi, [edi + edi*2]
            or ecx, 0xffffffff
            shl edi, 4
            add edi, offset gTree_surgery_pass2+0x18
            xor eax, eax
            repne scasb
            not ecx
            sub edi, ecx
            lea edx, [esp + 0x198]
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            shr ecx, 2
            rep movsd
            mov ecx, eax
            and ecx, 3
            rep movsb
            lea ecx, [esp + 0x198]
            call LoadModel
            mov ebp, eax
            test ebp, ebp
            je L00506420
            mov ecx, dword ptr [ebx + 0xc]
            push ecx
            call BrResFree
            mov edx, dword ptr [ebx + 8]
            add esp, 4
            push edx
            call BrResFree
            mov eax, dword ptr [ebp + 0xc]
            add esp, 4
            push eax
            call BrResClass
            mov ecx, dword ptr [ebp + 0xc]
            add esp, 4
            push eax
            push ecx
            call BrResSize
            add esp, 4
            push eax
            push ebx
            call BrResAllocate
            add esp, 0xc
            mov dword ptr [ebx + 0xc], eax
            mov edx, dword ptr [ebp + 0xc]
            push edx
            call BrResSize
            mov esi, dword ptr [ebp + 0xc]
            mov edi, dword ptr [ebx + 0xc]
            mov ecx, eax
            add esp, 4
            shr ecx, 2
            rep movsd
            mov ecx, eax
            and ecx, 3
            rep movsb
            mov cx, word ptr [ebp + 0x12]
            mov word ptr [ebx + 0x12], cx
            mov edx, dword ptr [ebp + 8]
            push edx
            call BrResClass
            add esp, 4
            push eax
            mov eax, dword ptr [ebp + 8]
            push eax
            call BrResSize
            add esp, 4
            push eax
            push ebx
            call BrResAllocate
            add esp, 0xc
            mov dword ptr [ebx + 8], eax
            mov ecx, dword ptr [ebp + 8]
            push ecx
            call BrResSize
            mov esi, dword ptr [ebp + 8]
            mov edi, dword ptr [ebx + 8]
            mov ecx, eax
            add esp, 4
            mov edx, ecx
            shr ecx, 2
            rep movsd
            mov ecx, edx
            push 0x7fff
            and ecx, 3
            push ebx
            rep movsb
            mov ax, word ptr [ebp + 0x10]
            mov word ptr [ebx + 0x10], ax
            call BrModelUpdate
            add esp, 8
    L00506420:
            mov ebx, dword ptr [esp + 0x14]
            mov ecx, dword ptr [gTrack_storage_space+0x2c]
            mov eax, dword ptr [ecx + ebx*4]
            test eax, eax
            je L00506503
            mov cx, word ptr [eax + 0x20]
            test cl, 0x82
            je L00506503
            mov edx, dword ptr [eax + 4]
            test edx, edx
            je L00506458
            cmp byte ptr [edx], 0x2d
            je L00506458
            and ecx, 0xff7d
            mov word ptr [eax + 0x20], cx
    L00506458:
            mov edx, dword ptr [gTrack_storage_space+0x2c]
            xor ebp, ebp
            mov eax, dword ptr [edx + ebx*4]
            mov ecx, dword ptr [eax + 0x4c]
            cmp word ptr [ecx + 8], bp
            jbe L005064df
            xor edi, edi
    L0050646e:
            mov edx, dword ptr [gTrack_storage_space+0x2c]
            mov ecx, dword ptr [edx + ebx*4]
            mov eax, dword ptr [ecx + 0x4c]
            mov ecx, dword ptr [ecx + 0xc]
            mov edx, dword ptr [eax + 0x18]
            xor eax, eax
            add edx, edi
            mov esi, dword ptr [edx + 0xc]
            mov edx, dword ptr [edx + 8]
            mov ax, word ptr [esi]
            lea eax, [eax + eax*4]
            mov esi, dword ptr [ecx + eax*8 + 8]
            test esi, esi
            mov dword ptr [edx], esi
            je L005064c6
            mov ecx, esi
            call SmoothificateWorldMaterial
            mov eax, dword ptr [esi + 0x48]
            test eax, eax
            jne L005064c6
            push offset gDepth_effect_names+0xa0
            call BrTableFind
            add esp, 4
            mov dword ptr [esi + 0x48], eax
            push 0x7fff
            push esi
            call BrMaterialUpdate
            add esp, 8
    L005064c6:
            mov eax, dword ptr [gTrack_storage_space+0x2c]
            inc ebp
            add edi, 0x24
            mov ecx, dword ptr [eax + ebx*4]
            xor eax, eax
            mov edx, dword ptr [ecx + 0x4c]
            mov ax, word ptr [edx + 8]
            cmp ebp, eax
            jl L0050646e
    L005064df:
            mov ecx, dword ptr [gTrack_storage_space+0x2c]
            mov ecx, dword ptr [ecx + ebx*4]
            test byte ptr [ecx + 0x20], 0x80
            je L005064fe
            push 0x7fff
            push ecx
            call BrModelUpdate
            add esp, 8
            jmp L00506503
    L005064fe:
            call DodgyModelUpdate
    L00506503:
            mov eax, dword ptr [gTrack_storage_space+0x4]
            inc ebx
            cmp ebx, eax
            mov dword ptr [esp + 0x14], ebx
            jl L0050625b
            mov edi, dword ptr [esp + 0x18]
            mov ebx, dword ptr [esp + 0xabc]
            xor ebp, ebp
    L00506522:
            call PossibleService
            mov edx, offset gDepth_effect_names+0x80
            xor ecx, ecx
            call PrintMemoryDump
            mov ecx, edi
            call LoadAIWorldTrackInfo
            mov edx, offset gDepth_effect_names+0x68
            xor ecx, ecx
            call PrintMemoryDump
            call MungeSmashMaterialNames
            call PackFileRevertTiffLoading
            mov ecx, edi
            call GetAnInt
            cmp eax, ebp
            mov dword ptr [esp + 0x48], eax
            jle L0050679d
            mov edx, eax
            add ebx, 0xf0c
            mov dword ptr [esp + 0x10], edx
            jmp L00506573
    L00506571:
            xor ebp, ebp
    L00506573:
            call PossibleService
            mov ecx, edi
            call GetAFloat
            fstp dword ptr [ebx - 4]
            mov ecx, edi
            call GetAFloat
            _emit 0xd9
        _emit 0x1b
            mov ecx, edi
            call GetAFloat
            fstp dword ptr [ebx + 4]
            mov ecx, edi
            call GetAFloat
            fstp dword ptr [ebx + 8]
            mov ecx, edi
            call GetAnInt
            mov ecx, edi
            mov dword ptr [ebx + 0xc], eax
            call GetAnInt
            mov ecx, edi
            mov dword ptr [ebx + 0x10], eax
            call GetAnInt
            mov ecx, edi
            mov dword ptr [ebx + 0x14], eax
            call GetAFloat
            fstp dword ptr [ebx + 0x18]
            mov ecx, edi
            call GetAnInt
            lea edx, [esp + 0x98]
            mov ecx, edi
            mov dword ptr [ebx + 0x1c], eax
            call GetAString
            lea eax, [esp + 0x98]
            push offset gPed_flags_0058f2e4+0x178
            push eax
            call strtok
            mov dword ptr [esp + 0x2c], eax
            add esp, 8
            mov esi, offset gNet_avail_names+0x7b8
            lea eax, [esp + 0x98]
    L00506604:
            mov dl, byte ptr [eax]
            mov cl, dl
            cmp dl, byte ptr [esi]
            jne L00506628
            test cl, cl
            je L00506624
            mov dl, byte ptr [eax + 1]
            mov cl, dl
            cmp dl, byte ptr [esi + 1]
            jne L00506628
            add eax, 2
            add esi, 2
            test cl, cl
            jne L00506604
    L00506624:
            xor eax, eax
            jmp L0050662d
    L00506628:
            sbb eax, eax
            sbb eax, -1
    L0050662d:
            cmp eax, ebp
            je L0050677b
            mov esi, offset gPed_flags_0058f2e4+0xd8
            lea eax, [esp + 0x98]
    L00506641:
            mov dl, byte ptr [eax]
            mov cl, dl
            cmp dl, byte ptr [esi]
            jne L00506665
            test cl, cl
            je L00506661
            mov dl, byte ptr [eax + 1]
            mov cl, dl
            cmp dl, byte ptr [esi + 1]
            jne L00506665
            add eax, 2
            add esi, 2
            test cl, cl
            jne L00506641
    L00506661:
            xor eax, eax
            jmp L0050666a
    L00506665:
            sbb eax, eax
            sbb eax, -1
    L0050666a:
            cmp eax, ebp
            je L0050677b
            mov esi, offset gOpponent_nastyness_frigger+0x3b8
            lea eax, [esp + 0x98]
    L0050667e:
            mov dl, byte ptr [eax]
            mov cl, dl
            cmp dl, byte ptr [esi]
            jne L005066a2
            test cl, cl
            je L0050669e
            mov dl, byte ptr [eax + 1]
            mov cl, dl
            cmp dl, byte ptr [esi + 1]
            jne L005066a2
            add eax, 2
            add esi, 2
            test cl, cl
            jne L0050667e
    L0050669e:
            xor eax, eax
            jmp L005066a7
    L005066a2:
            sbb eax, eax
            sbb eax, -1
    L005066a7:
            cmp eax, ebp
            je L0050677b
            mov esi, offset gOpponent_nastyness_frigger+0x3b4
            lea eax, [esp + 0x98]
    L005066bb:
            mov dl, byte ptr [eax]
            mov cl, dl
            cmp dl, byte ptr [esi]
            jne L005066df
            test cl, cl
            je L005066db
            mov dl, byte ptr [eax + 1]
            mov cl, dl
            cmp dl, byte ptr [esi + 1]
            jne L005066df
            add eax, 2
            add esi, 2
            test cl, cl
            jne L005066bb
    L005066db:
            xor eax, eax
            jmp L005066e4
    L005066df:
            sbb eax, eax
            sbb eax, -1
    L005066e4:
            cmp eax, ebp
            je L0050677b
            mov edx, dword ptr [esp + 0x24]
            or ecx, 0xffffffff
            mov edi, edx
            xor eax, eax
            repne scasb
            not ecx
            dec ecx
            mov edi, offset gDRFont_to_polyfont_mapping+0x314
            mov ebp, ecx
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            mov eax, ecx
            mov esi, edi
            mov dword ptr [esp + 0x1c], eax
            mov edi, edx
            or ecx, 0xffffffff
            xor eax, eax
            repne scasb
            mov eax, dword ptr [esp + 0x1c]
            dec edi
            mov ecx, eax
            shr ecx, 2
            rep movsd
            mov ecx, eax
            and ecx, 3
            rep movsb
            mov ecx, offset gTrack_storage_space
            call LoadSinglePixelmap
            mov edx, dword ptr [esp + 0x24]
            mov edi, offset gMaterial_names+0x70
            or ecx, 0xffffffff
            xor eax, eax
            mov byte ptr [edx + ebp], 0
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
            rep movsb
            mov ecx, offset gTrack_storage_space
            call LoadSingleMaterial
            mov dword ptr [ebx + 0x20], eax
            jmp L0050677e
    L0050677b:
            mov dword ptr [ebx + 0x20], ebp
    L0050677e:
            mov eax, dword ptr [esp + 0x10]
            mov edi, dword ptr [esp + 0x18]
            add ebx, 0x28
            dec eax
            mov dword ptr [esp + 0x10], eax
            jne L00506571
            mov ebx, dword ptr [esp + 0xabc]
            xor ebp, ebp
    L0050679d:
            call PackFileRerevertTiffLoading
            mov esi, dword ptr [esp + 0x48]
            mov ecx, 0x3f800000
            cmp esi, 0xa
            jge L005067eb
            lea edx, [esi + esi*4]
            lea eax, [ebx + edx*8 + 0xf0c]
            mov edx, 0xa
            sub edx, esi
            mov esi, 1
    L005067c6:
            mov dword ptr [eax - 4], ecx
            mov dword ptr [eax], ecx
            mov dword ptr [eax + 4], ecx
            mov dword ptr [eax + 8], ebp
            mov dword ptr [eax + 0xc], ebp
            mov dword ptr [eax + 0x10], ebp
            mov dword ptr [eax + 0x14], ebp
            mov dword ptr [eax + 0x18], ecx
            mov dword ptr [eax + 0x1c], esi
            mov dword ptr [eax + 0x20], ebp
            add eax, 0x28
            dec edx
            jne L005067c6
            jmp L005067f0
    L005067eb:
            mov esi, 1
    L005067f0:
            mov dword ptr [ebx + 0x1098], ecx
            mov dword ptr [ebx + 0x109c], ecx
            mov dword ptr [ebx + 0x10a0], ebp
            mov dword ptr [ebx + 0x10a4], ebp
            mov dword ptr [ebx + 0x10a8], 0xffffffff
            mov dword ptr [ebx + 0x10ac], ebp
            mov dword ptr [ebx + 0x10b0], ebp
            mov dword ptr [ebx + 0x10b4], ebp
            mov dword ptr [ebx + 0x10b8], esi
            mov dword ptr [ebx + 0x10bc], ebp
            mov eax, dword ptr [gDefault_water_spec_vol_real]
            mov ecx, edi
            mov dword ptr [eax + 0xb0], 0xa
            call GetAnInt
            mov esi, eax
            cmp esi, 0x28
            mov dword ptr [esp + 0x10], esi
            jle L0050685b
            mov ecx, offset gDepth_effect_names+0x50
            call PDFatalError
    L0050685b:
            mov ecx, dword ptr [gCount_smashable_noncars]
            push 0xde
            push 0x104
            lea eax, [esi + ecx + 0x28]
            push eax
            mov dword ptr [gProgram_state+0x5ecc], eax
            call BrMemCalloc
            mov ebp, eax
            xor ebx, ebx
            add esp, 0xc
            cmp ebp, ebx
            jne L00506893
            cmp esi, ebx
            je L00506893
            push 0x32
            call FatalError
            add esp, 4
    L00506893:
            mov ecx, 0x19
            xor eax, eax
            mov edi, offset gNon_car_spec_indices
            mov dword ptr [gProgram_state+0x5ec8], ebp
            rep stosd
            mov esi, 0x28
            mov edi, 0xffff
    L005068b1:
            push 0xf6
            push 0x4d8
            mov dword ptr [ebp + 0xc], ebx
            call BrMemAllocate
            mov dword ptr [ebp + 8], eax
            mov dword ptr [eax + 0x23c], ebp
            mov edx, dword ptr [ebp + 8]
            add esp, 8
            add ebp, 0x104
            dec esi
            mov byte ptr [edx + 0x238], 1
            mov eax, dword ptr [ebp - 0xfc]
            mov dword ptr [eax + 0x1a0], edi
            mov ecx, dword ptr [ebp - 0xfc]
            mov dword ptr [ecx + 0x1a4], ebx
            jne L005068b1
            mov edx, dword ptr [gCount_smashable_noncars]
            mov edi, dword ptr [esp + 0x10]
            xor ebx, ebx
            lea eax, [edi + edx]
            test eax, eax
            jle L00506a52
            mov eax, edi
            mov dword ptr [esp + 0x24], ebx
            shl eax, 5
            mov dword ptr [esp + 0x1c], eax
    L0050691e:
            call PossibleService
            push 0xf6
            push 0x4d8
            call BrMemAllocate
            mov dword ptr [ebp + 8], eax
            mov dword ptr [eax + 0x23c], ebp
            mov ecx, dword ptr [ebp + 8]
            add esp, 8
            cmp ebx, edi
            mov byte ptr [ecx + 0x238], 1
            mov edx, dword ptr [ebp + 8]
            mov dword ptr [edx + 0x1a0], 0xffff
            mov eax, dword ptr [ebp + 8]
            mov dword ptr [eax + 0x1a4], 0
            jge L00506978
            mov ecx, dword ptr [esp + 0x18]
            lea edx, [esp + 0x98]
            call GetAString
            jmp L005069b3
    L00506978:
            mov eax, dword ptr [esp + 0x24]
            mov ecx, dword ptr [esp + 0x1c]
            sub eax, ecx
            mov ecx, dword ptr [gSmashable_noncars]
            lea edx, [esp + 0x98]
            lea edi, [ecx + eax]
            or ecx, 0xffffffff
            xor eax, eax
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
            mov edi, dword ptr [esp + 0x10]
    L005069b3:
            push offset gDepth_effect_names+0x48
            mov edx, offset gApplication_path
            lea ecx, [esp + 0x1bc]
            call PathCat
            lea ecx, [esp + 0x98]
            lea edx, [esp + 0x1b8]
            push ecx
            lea ecx, [esp + 0x1bc]
            call PathCat
            mov edx, offset gNext_crush_actor_id+0x180
            lea ecx, [esp + 0x1b8]
            call DRfopen
            mov esi, eax
            test esi, esi
            jne L00506a0d
            lea edx, [esp + 0x1b8]
            push edx
            push 0x6b
            call FatalError
            add esp, 8
    L00506a0d:
            mov edx, ebp
            mov ecx, esi
            call ReadNonCarMechanicsData
            call PossibleService
            mov ecx, dword ptr [ebp]
            mov al, bl
            inc al
            mov byte ptr [ecx + gNon_car_spec_indices], al
            mov ecx, esi
            call PFfclose
            mov edx, dword ptr [gCount_smashable_noncars]
            mov esi, dword ptr [esp + 0x24]
            inc ebx
            add esi, 0x20
            lea eax, [edi + edx]
            add ebp, 0x104
            cmp ebx, eax
            mov dword ptr [esp + 0x24], esi
            jl L0050691e
    L00506a52:
            call DisposeSmashEnvNonCars
            mov eax, dword ptr [gProgram_state+0x1bbc]
            xor edi, edi
            test eax, eax
            jle L00506b0d
    L00506a66:
            mov ecx, dword ptr [gProgram_state+0x1bc0]
            mov ecx, dword ptr [ecx + edi*4]
            test ecx, ecx
            je L00506aff
            mov eax, dword ptr [ecx + 0x14]
            movsx edx, byte ptr [eax + 1]
            movsx eax, byte ptr [eax + 2]
            lea edx, [edx + edx*4]
            movsx eax, byte ptr [eax + edx*2 + gNon_car_indices]
            add eax, 0x27
            mov edx, eax
            shl edx, 6
            add edx, eax
            mov eax, dword ptr [gProgram_state+0x5ec8]
            lea eax, [eax + edx*4]
            test eax, eax
            je L00506aff
            mov edx, dword ptr [eax + 8]
            cmp dword ptr [edx], 0
            jne L00506aff
            mov dword ptr [edx], ecx
            mov eax, dword ptr [eax + 8]
            mov ecx, dword ptr [ecx + 0x18]
            add eax, 0x3c
            add ecx, 0x34
            mov esi, eax
            sub esi, ecx
            mov ebx, eax
            lea edx, [ecx + 0xc]
            mov ecx, 3
    L00506ac7:
            fld dword ptr [edx - 0xc]
            _emit 0xd9
        _emit 0x03
            fcompp
            fnstsw ax
            test ah, 0x41
            jne L00506ada
            fld dword ptr [edx - 0xc]
            jmp L00506adc
    L00506ada:
            _emit 0xd9
        _emit 0x03
    L00506adc:
            fstp dword ptr [edx - 0xc]
            _emit 0xd9
        _emit 0x02
            fld dword ptr [esi + edx]
            fcompp
            fnstsw ax
            test ah, 1
            je L00506af1
            _emit 0xd9
        _emit 0x02
            jmp L00506af4
    L00506af1:
            fld dword ptr [esi + edx]
    L00506af4:
            _emit 0xd9
        _emit 0x1a
            add ebx, 4
            add edx, 4
            dec ecx
            jne L00506ac7
    L00506aff:
            mov eax, dword ptr [gProgram_state+0x1bbc]
            inc edi
            cmp edi, eax
            jl L00506a66
    L00506b0d:
            mov esi, dword ptr [esp + 0x18]
            mov ecx, esi
            call GetSmokeShadeTables
            mov ecx, esi
            call GetAnInt
            mov ebp, dword ptr [esp + 0xabc]
            xor edi, edi
            test eax, eax
            mov dword ptr [ebp + 0x94], eax
            jle L00506b63
            lea esi, [ebp + 0xa0]
    L00506b38:
            mov ebx, dword ptr [esp + 0x18]
            lea ecx, [esi + 4]
            push ecx
            push esi
            lea edx, [esi - 4]
            mov ecx, ebx
            call GetThreeScalars
            mov ecx, ebx
            call GetAFloat
            fstp dword ptr [esi + 8]
            mov eax, dword ptr [ebp + 0x94]
            inc edi
            add esi, 0x10
            cmp edi, eax
            jl L00506b38
    L00506b63:
            mov esi, dword ptr [esp + 0x18]
            mov ecx, esi
            call LoadInKevStuff
            mov ecx, esi
            call GetAFloat
            fstp dword ptr [gYon_multiplier]
            lea edx, [esp + 0x98]
            mov ecx, esi
            call GetAString
            mov esi, dword ptr [esp + 0x5c]
            lea ecx, [esp + 0x98]
            mov edx, esi
            call DRStricmp
            test eax, eax
            je L00506baa
            push esi
            push 0x73
            call FatalError
            add esp, 8
    L00506baa:
            mov ecx, dword ptr [esp + 0x18]
            call PFfclose
            mov esi, dword ptr [gMaterial_exceptions]
            test esi, esi
            je L00506bda
    L00506bbd:
            mov edx, dword ptr [esi + 4]
            mov edi, dword ptr [esi]
            push edx
            call BrMemFree
            add esp, 4
            push esi
            call BrMemFree
            add esp, 4
            mov esi, edi
            test edi, edi
            jne L00506bbd
    L00506bda:
            mov edx, offset gDepth_effect_names+0x30
            xor ecx, ecx
            mov dword ptr [gMaterial_exceptions], 0
            call PrintMemoryDump
            lea ecx, [esp + 0x8b8]
            call LoadStaticLightingForRace
            mov ecx, dword ptr [esp + 0x70]
            call ClosePackFileAndSetTiffLoading
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0xaa8
            ret 4
    }
#endif
}

// RemoveBounds

// RemoveBoundsStructures

// FreeTrack

// ProcessTrack

// AnimateSky

// CombineBounds

// CheckNonCarModelBounds

// FUNCTION: CARMA2_HW 0x00500d50
void C2_HOOK_FASTCALL InitialiseStorageSpace(int pUnknown, tBrender_storage* pStorage_space, int pMax_pixelmaps, int pMax_shade_tables, int pMax_materials, int pMax_models, int pMax_sounds) {
    pStorage_space->pixelmaps_count = 0;
    pStorage_space->shade_tables_count = 0;
    pStorage_space->materials_count = 0;
    pStorage_space->models_count = 0;
    pStorage_space->sounds_count = 0;
    pStorage_space->flags = pUnknown;
    pStorage_space->max_pixelmaps = pMax_pixelmaps;
    pStorage_space->max_shade_tables = pMax_shade_tables;
    pStorage_space->max_materials = pMax_materials;
    pStorage_space->max_models = pMax_models;
    pStorage_space->max_sounds = pMax_sounds;
    pStorage_space->pixelmaps = BrMemCalloc(pMax_pixelmaps, sizeof(br_pixelmap*), kMem_stor_space_table);
    pStorage_space->shade_tables = BrMemCalloc(pMax_shade_tables, sizeof(br_pixelmap*), kMem_stor_space_table);
    pStorage_space->materials = BrMemCalloc(pMax_materials, sizeof(br_material*), kMem_stor_space_table);
    pStorage_space->models = BrMemCalloc(pMax_models, sizeof(br_model*), kMem_stor_space_table);
    pStorage_space->sounds = BrMemCalloc(pMax_sounds, sizeof(int), kMem_stor_space_table);
    pStorage_space->materialProps = BrMemCalloc(pMax_materials, sizeof(br_material*), kMem_stor_space_table);
}

// FUNCTION: CARMA2_HW 0x00500e10
void C2_HOOK_FASTCALL DisposeStorageSpace(tBrender_storage* pStorage) {
    BrMemFree(pStorage->pixelmaps);
    BrMemFree(pStorage->shade_tables);
    BrMemFree(pStorage->materials);
    BrMemFree(pStorage->models);
    BrMemFree(pStorage->sounds);
    BrMemFree(pStorage->materialProps);
}

// FUNCTION: CARMA2_HW 0x00500e60
void C2_NAKED C2_HOOK_FASTCALL ClearMatertrialSetFromStorageSpace(tBrender_storage* pStorage_space, int pStart, int pEnd) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;
    int move_to;

    for (i = pStart; i < pEnd && i < pStorage_space->materials_count; i++) {
        BrMaterialRemove(pStorage_space->materials[i]);
        BrMaterialFree(pStorage_space->materials[i]);
        RemoveMaterialFromFogification(pStorage_space->materials[i]);
    }
    for (move_to = pStart, i = pEnd; i < pStorage_space->materials_count; ) {
        pStorage_space->materials[move_to++] = pStorage_space->materials[i++];
    }
    pStorage_space->materials_count = move_to;
    if (pStorage_space == &gNet_cars_storage_space) {
        for (i = 0; i < gNumber_of_net_players; i++) {
            tCar_spec* car = gNet_players[i].car;
            if (car != NULL && car->old_material_count >= pEnd) {
                car->old_material_count += pStart - pEnd;
                car->new_material_count += pStart - pEnd;
            }
        }
    }
#else
    __asm {
            push ebx
            push ebp
            push esi
            push edi
            mov edi, dword ptr [esp + 0x14]
            mov ebp, edx
            cmp ebp, edi
            mov esi, ecx
            mov ebx, ebp
            jge L00500ea5
    L00500e72:
            cmp ebx, dword ptr [esi + 8]
            jge L00500ea5
            mov eax, dword ptr [esi + 0x38]
            mov ecx, dword ptr [eax + ebx*4]
            push ecx
            call BrMaterialRemove
            mov edx, dword ptr [esi + 0x38]
            add esp, 4
            mov eax, dword ptr [edx + ebx*4]
            push eax
            call BrMaterialFree
            mov ecx, dword ptr [esi + 0x38]
            add esp, 4
            mov ecx, dword ptr [ecx + ebx*4]
            call RemoveMaterialFromFogification
            inc ebx
            cmp ebx, edi
            jl L00500e72
    L00500ea5:
            mov ecx, dword ptr [esi + 8]
            mov edx, ebp
            cmp edi, ecx
            mov eax, edi
            jge L00500ec4
    L00500eb0:
            mov ecx, dword ptr [esi + 0x38]
            inc eax
            inc edx
            mov ebx, dword ptr [ecx + eax*4 - 4]
            mov dword ptr [ecx + edx*4 - 4], ebx
            mov ecx, dword ptr [esi + 8]
            cmp eax, ecx
            jl L00500eb0
    L00500ec4:
            cmp esi, offset gNet_cars_storage_space
            mov dword ptr [esi + 8], edx
            jne L00500f22
            mov eax, dword ptr [gNumber_of_net_players]
            xor esi, esi
            test eax, eax
            jle L00500f22
            mov edx, offset gNet_players+0xc8
    L00500edf:
            mov eax, dword ptr [edx]
            test eax, eax
            je L00500f12
            mov cx, word ptr [eax + 0x1958]
            mov ebx, ecx
            and ebx, 0xffff
            cmp ebx, edi
            jl L00500f12
            mov ebx, ebp
            sub ebx, edi
            add ebx, ecx
            mov ecx, ebp
            mov word ptr [eax + 0x1958], bx
            mov eax, dword ptr [edx]
            sub ecx, edi
            add word ptr [eax + 0x195a], cx
    L00500f12:
            mov eax, dword ptr [gNumber_of_net_players]
            inc esi
            add edx, 0xd4
            cmp esi, eax
            jl L00500edf
    L00500f22:
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 4
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

// FUNCTION: CARMA2_HW 0x00500f30
void C2_HOOK_FASTCALL ClearOutStorageSpace(tBrender_storage* pStorage_space) {
    int i;

    DRS3StopAllOutletSoundsExceptCDA();
    for (i = 0; i < pStorage_space->pixelmaps_count; i++) {
        if (pStorage_space->pixelmaps[i] != NULL) {
            BrMapRemove(pStorage_space->pixelmaps[i]);
            BrPixelmapFree(pStorage_space->pixelmaps[i]);
        }
    }
    pStorage_space->pixelmaps_count = 0;
    for (i = 0; i < pStorage_space->shade_tables_count; i++) {
        if (pStorage_space->shade_tables[i] != NULL) {
            BrTableRemove(pStorage_space->shade_tables[i]);
            BrPixelmapFree(pStorage_space->shade_tables[i]);
        }
    }
    pStorage_space->shade_tables_count = 0;
    for (i = 0; i < pStorage_space->materials_count; i++) {
        if (pStorage_space->materials[i] != NULL) {
            BrMaterialRemove(pStorage_space->materials[i]);
            BrMaterialFree(pStorage_space->materials[i]);
        }
    }
    pStorage_space->materials_count = 0;
    for (i = 0; i < pStorage_space->models_count; i++) {
        if (pStorage_space->models[i] != NULL) {
            BrModelRemove(pStorage_space->models[i]);
            BrModelFree(pStorage_space->models[i]);
        }
    }
    pStorage_space->models_count = 0;
}

// FUNCTION: CARMA2_HW 0x00501020
tAdd_to_storage_result C2_HOOK_FASTCALL AddPixelmapToStorage(tBrender_storage* pStorage_space, br_pixelmap* pThe_pm) {
    int i;

    gDuplicate_pixelmap = NULL;
    if (pStorage_space->pixelmaps_count < pStorage_space->max_pixelmaps) {
        for (i = 0; i < pStorage_space->pixelmaps_count; i++) {
            if (pStorage_space->pixelmaps[i]->identifier != NULL
                    && pThe_pm->identifier != NULL
                    && strcmp(pStorage_space->pixelmaps[i]->identifier, pThe_pm->identifier) == 0) {
                gDuplicate_pixelmap = pStorage_space->pixelmaps[i];
                return eStorage_duplicate;
            }
        }
        pStorage_space->pixelmaps[pStorage_space->pixelmaps_count] = pThe_pm;
        pStorage_space->pixelmaps_count += 1;
        return eStorage_allocated;
    } else {
        return eStorage_not_enough_room;
    }
}

// FUNCTION: CARMA2_HW 0x005010e0
tAdd_to_storage_result C2_HOOK_FASTCALL AddShadeTableToStorage(tBrender_storage* pStorage_space, br_pixelmap* pThe_st) {
    int i;

    if (pStorage_space->shade_tables_count < pStorage_space->max_shade_tables) {
        for (i = 0; i < pStorage_space->shade_tables_count; i++) {
            if (pStorage_space->shade_tables[i]->identifier != NULL
                && pThe_st->identifier != NULL
                && strcmp(pStorage_space->shade_tables[i]->identifier, pThe_st->identifier) == 0) {
                return eStorage_duplicate;
            }
        }
        pStorage_space->shade_tables[pStorage_space->shade_tables_count] = pThe_st;
        pStorage_space->shade_tables_count += 1;
        return eStorage_allocated;
    } else {
        return eStorage_not_enough_room;
    }
}

// FUNCTION: CARMA2_HW 0x00501190
C2_MATCHING_INLINE tAdd_to_storage_result C2_HOOK_FASTCALL AddMaterialToStorage(tBrender_storage* pStorage_space, br_material* pThe_mat) {
    int i;

    gDuplicate_material = NULL;
    if (pStorage_space->materials_count < pStorage_space->max_materials) {
        for (i = 0; i < pStorage_space->materials_count; i++) {
            if (pStorage_space->materials[i]->identifier != NULL
                    && pThe_mat->identifier != NULL
                    && strcmp(pStorage_space->materials[i]->identifier, pThe_mat->identifier) == 0) {
                gDuplicate_material = pStorage_space->materials[i];
                return eStorage_duplicate;
            }
        }
        pStorage_space->materialProps[pStorage_space->materials_count] = 0; /* FIXME */
        pStorage_space->materials[pStorage_space->materials_count] = pThe_mat;
        pStorage_space->materials_count += 1;
        return eStorage_allocated;
    } else {
        return eStorage_not_enough_room;
    }
}

// FUNCTION: CARMA2_HW 0x00501260
C2_MATCHING_INLINE tAdd_to_storage_result C2_HOOK_FASTCALL AddModelToStorage(tBrender_storage* pStorage_space, br_model* pThe_mod) {
    int i;

    gDuplicate_model = NULL;
    if (pStorage_space->models_count < pStorage_space->max_models) {
        if (pStorage_space->flags & 0x1) {
 /* FIXME: add enum (0x1 -> eStorage_space_flags_No_duplicates*/
            for (i = 0; i < pStorage_space->models_count; i++) {
                if (pStorage_space->models[i] != NULL
                    && pStorage_space->models[i]->identifier != NULL
                        && pThe_mod->identifier != NULL
                        && strcmp(pStorage_space->models[i]->identifier, pThe_mod->identifier) == 0) {
                    gDuplicate_model = pStorage_space->models[i];
                    return eStorage_duplicate;
                }
            }
        }
        pStorage_space->models[pStorage_space->models_count] = pThe_mod;
        pStorage_space->models_count += 1;
        return eStorage_allocated;
    } else {
        return eStorage_not_enough_room;
    }
}

// FUNCTION: CARMA2_HW 0x00501330
C2_MATCHING_INLINE tAdd_to_storage_result C2_HOOK_FASTCALL AddSoundToStorage(tBrender_storage* pStorage_space, int pSound_id) {
    int i;

    if (pStorage_space->sounds_count < pStorage_space->max_sounds) {
        for (i = 0; i < pStorage_space->sounds_count; i++) {
            if (pStorage_space->sounds[i] != 0 && pStorage_space->sounds[i] == pSound_id) {
                return eStorage_duplicate;
            }
        }
        pStorage_space->sounds[pStorage_space->sounds_count] = pSound_id;
        pStorage_space->sounds_count++;
        return eStorage_allocated;
    } else {
        return eStorage_not_enough_room;
    }
}

// FUNCTION: CARMA2_HW 0x005024f0
int C2_HOOK_FASTCALL AddPixelmaps(tBrender_storage* pStorage_space, const char* path) {
    int i;
    int new_ones;
    int total;
    tPath_name path_dirname;
    char path_stem[32];
    br_pixelmap* temp_array[500];

    total = 0;
    if (gDisableTiffConversion) {
        SepDirAndFilename(path, path_dirname, path_stem);
        new_ones = LoadBunchOfPixies(path_dirname, path_stem, temp_array, CARPOCALYPSE2_ASIZE(temp_array));
    } else {
        new_ones = DRPixelmapLoadMany(path, temp_array, CARPOCALYPSE2_ASIZE(temp_array));
    }
    if (new_ones == 0) {
        FatalError(kFatalError_CantLoadPixelmapFile_S, path);
    }
    for (i = 0; i < new_ones; i++) {
        if (temp_array[i] != NULL) {
            EnsurePixelmapAllowed(temp_array[i], 1);
            switch (AddPixelmapToStorage(pStorage_space, temp_array[i])) {
            case eStorage_allocated:
                BrMapAdd(temp_array[i]);
                total += 1;
                break;
            case eStorage_duplicate:
                if (gDisallow_duplicates) {
                    FatalError(kFatalError_DuplicatePixelmap_S, temp_array[i]->identifier);
                } else {
                    BrPixelmapFree(temp_array[i]);
                }
                break;
            case eStorage_not_enough_room:
                FatalError(kFatalError_InsufficientPixelmapSlots);
                break;
            }
        }
    }
    return total;
}

// FUNCTION: CARMA2_HW 0x00501560
br_pixelmap* C2_HOOK_FASTCALL LoadSinglePixelmap(tBrender_storage* pStorage, const char* pName) {
    br_pixelmap* map;
    tAdd_to_storage_result addResult;

    map = LoadPixelmap(pName);
    if (map == NULL) {
        return BrMapFind(pName);
    }

    addResult = AddPixelmapToStorage(pStorage, map);
    switch (addResult) {
    case eStorage_allocated:
        BrMapAdd(map);
        return map;
    case eStorage_duplicate:
        if (gDisallow_duplicates) {
            FatalError(kFatalError_DuplicatePixelmap_S, map->identifier);
        } else {
            BrPixelmapFree(map);
        }
        return gDuplicate_pixelmap;
    case eStorage_not_enough_room:
        FatalError(kFatalError_InsufficientPixelmapSlots);
        break;
    }
    return NULL;
}

// FUNCTION: CARMA2_HW 0x005016a0
br_pixelmap* C2_HOOK_FASTCALL LoadSingleShadeTable(tBrender_storage* pStorage_space, const char* pName) {
    br_pixelmap* temp;

    temp = LoadShadeTable(pName);
    if (temp == NULL) {
        return BrTableFind(pName);
    }

    switch (AddShadeTableToStorage(pStorage_space, temp)) {
    case eStorage_allocated:
        BrTableAdd(temp);
        return temp;
    case eStorage_duplicate:
        if (gDisallow_duplicates) {
            FatalError(kFatalError_DuplicatePixelmap_S, temp->identifier);
        } else {
            BrPixelmapFree(temp);
        }
        return BrTableFind(pName);
    case eStorage_not_enough_room:
        FatalError(kFatalError_InsufficientShadeTableSlots);
        break;
    }

    return NULL;
}

// FUNCTION: CARMA2_HW 0x005017e0
br_material* C2_HOOK_FASTCALL LoadSingleMaterial(tBrender_storage* pStorage_space, const char* pName) {
    br_material* material;

    material = LoadMaterial(pName);
    if (material == NULL) {
        return BrMaterialFind(pName);
    }

    switch (AddMaterialToStorage(pStorage_space, material)) {
    case eStorage_allocated:
        BrMaterialAdd(material);
        return material;
    case eStorage_duplicate:
        if (gDisallow_duplicates) {
            FatalError(kFatalError_DuplicateMaterial_S, material->identifier);
        } else {
            BrMaterialFree(material);
        }
        return gDuplicate_material;
    case eStorage_not_enough_room:
        FatalError(kFatalError_InsufficientMaterialSlots);
        return NULL;
    }
    return NULL;
}

// FUNCTION: CARMA2_HW 0x00501930
tAdd_to_storage_result C2_HOOK_FASTCALL LoadSingleSound(tBrender_storage* pStorage_space, int pSound_id) {
    if (S3GetBufferDescription(pSound_id) != NULL) {
        return pSound_id;
    }

    switch (AddSoundToStorage(pStorage_space, pSound_id)) {

    case eStorage_duplicate:
        return pSound_id;

    case eStorage_allocated:
        S3LoadSample(pSound_id);
        return pSound_id;
    case eStorage_not_enough_room:
        FatalError(kFatalError_InsufficientSoundSlotsInStorageArea);
        break;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00501380
int C2_NAKED C2_HOOK_FASTCALL LoadPixelmapsFromListFile(tBrender_storage* pStorage_space, FILE* pFile, int pCount) {
#ifndef CARPOCALYPSE2_MATCHING
    int total;
    char* name;
    int files_left;
    int pixelmaps_left;
    int i;
    int status;
    br_pixelmap* pixelmaps[500];
    br_pixelmap** volatile pixelmap_cursor;
    char line[256];

    total = 0;
    if (pCount > 0) {
        files_left = pCount;
        do {
            PossibleService();
            GetALineAndDontArgue(pFile, line);
            name = strtok(line, "	 ,/");
            DoNotAllowOpenToFail();
            pixelmaps_left = LoadTextureTryAllLocations(name, pixelmaps, CARPOCALYPSE2_ASIZE(pixelmaps));
            if (pixelmaps_left == 0) {
                FatalError(kFatalError_CantLoadPixelmapFile_S, name);
            }
            if (pixelmaps_left > 0) {
                pixelmap_cursor = pixelmaps;
                do {
                    if (*pixelmap_cursor != NULL) {
                        gDuplicate_pixelmap = NULL;
                        status = pStorage_space->pixelmaps_count;
                        if (status < pStorage_space->max_pixelmaps) {
                            i = 0;
                            if (status > 0) {
                                for (i = 0; i < status; i++) {
                                    if (pStorage_space->pixelmaps[i]->identifier != NULL
                                            && (*pixelmap_cursor)->identifier != NULL
                                            && strcmp(pStorage_space->pixelmaps[i]->identifier, (*pixelmap_cursor)->identifier) == 0) {
                                        gDuplicate_pixelmap = pStorage_space->pixelmaps[i];
                                        status = 1;
                                        goto process_pixelmap_result;
                                    }
                                }
                            }
                            pStorage_space->pixelmaps[status] = *pixelmap_cursor;
                            pStorage_space->pixelmaps_count += 1;
                            status = 2;
                        } else {
                            status = 0;
                        }
process_pixelmap_result:
                        switch (status) {
                        case eStorage_not_enough_room:
                            FatalError(kFatalError_InsufficientPixelmapSlots);
                            break;
                        case eStorage_duplicate:
                            if (gDisallow_duplicates != 0) {
                                FatalError(kFatalError_DuplicatePixelmap_S, (*pixelmap_cursor)->identifier);
                            } else {
                                BrPixelmapFree(*pixelmap_cursor);
                            }
                            break;
                        case eStorage_allocated:
                            BrMapAdd(*pixelmap_cursor);
                            total += 1;
                            break;
                        }
                    }
                    pixelmap_cursor += 1;
                    pixelmaps_left -= 1;
                } while (pixelmaps_left != 0);
            }
            files_left -= 1;
        } while (files_left != 0);
    }
    return total;
#else
    __asm {
            sub esp, 0x8e8
            mov eax, dword ptr [esp + 0x8ec]
            push ebx
            mov ebx, ecx
            mov dword ptr [esp + 0x18], edx
            test eax, eax
            mov dword ptr [esp + 0x10], ebx
            mov dword ptr [esp + 8], 0
            jle L00501544
            push edi
            push esi
            push ebp
            mov dword ptr [esp + 0x20], eax
    L005013af:
            call PossibleService
            mov ecx, dword ptr [esp + 0x24]
            lea edx, [esp + 0x28]
            call GetALineAndDontArgue
            lea eax, [esp + 0x28]
            push offset gPed_flags_0058f2e4+0x138
            push eax
            call strtok
            add esp, 8
            mov edi, eax
            call DoNotAllowOpenToFail
            lea edx, [esp + 0x128]
            mov ecx, edi
            push 0x1f4
            call LoadTextureTryAllLocations
            mov esi, eax
            test esi, esi
            jne L00501400
            push edi
            push 0x4f
            call FatalError
            add esp, 8
            test esi, esi
    L00501400:
            jle L00501532
            lea ecx, [esp + 0x128]
            mov dword ptr [esp + 0x18], esi
            mov dword ptr [esp + 0x10], ecx
    L00501415:
            mov edx, dword ptr [esp + 0x10]
            cmp dword ptr [edx], 0
            je L00501518
            mov dword ptr [gDuplicate_pixelmap], 0
            mov ebp, dword ptr [ebx + 0x10]
            cmp ebp, dword ptr [ebx + 0x18]
            jge L005014bd
            xor edi, edi
            test ebp, ebp
            jle L00501490
            mov edx, dword ptr [ebx + 0x30]
    L00501441:
            mov eax, dword ptr [edx]
            mov eax, dword ptr [eax + 4]
            test eax, eax
            je L00501488
            mov ecx, dword ptr [esp + 0x10]
            mov ecx, dword ptr [ecx]
            mov esi, dword ptr [ecx + 4]
            test esi, esi
            je L00501488
    L00501457:
            mov bl, byte ptr [eax]
            mov cl, bl
            cmp bl, byte ptr [esi]
            jne L0050147b
            test cl, cl
            je L00501477
            mov bl, byte ptr [eax + 1]
            mov cl, bl
            cmp bl, byte ptr [esi + 1]
            jne L0050147b
            add eax, 2
            add esi, 2
            test cl, cl
            jne L00501457
    L00501477:
            xor eax, eax
            jmp L00501480
    L0050147b:
            sbb eax, eax
            sbb eax, -1
    L00501480:
            mov ebx, dword ptr [esp + 0x1c]
            test eax, eax
            je L005014aa
    L00501488:
            inc edi
            add edx, 4
            cmp edi, ebp
            jl L00501441
    L00501490:
            mov edx, dword ptr [esp + 0x10]
            mov ecx, dword ptr [ebx + 0x30]
            mov eax, dword ptr [edx]
            mov dword ptr [ecx + ebp*4], eax
            mov eax, dword ptr [ebx + 0x10]
            inc eax
            mov dword ptr [ebx + 0x10], eax
            mov eax, 2
            jmp L005014bf
    L005014aa:
            mov eax, dword ptr [ebx + 0x30]
            mov edx, dword ptr [eax + edi*4]
            mov eax, 1
            mov dword ptr [gDuplicate_pixelmap], edx
            jmp L005014bf
    L005014bd:
            xor eax, eax
    L005014bf:
            sub eax, 0
            je L0050150e
            dec eax
            je L005014e4
            dec eax
            jne L00501518
            mov eax, dword ptr [esp + 0x10]
            mov ecx, dword ptr [eax]
            push ecx
            call BrMapAdd
            mov eax, dword ptr [esp + 0x18]
            add esp, 4
            inc eax
            mov dword ptr [esp + 0x14], eax
            jmp L00501518
    L005014e4:
            mov eax, dword ptr [gDisallow_duplicates]
            mov edx, dword ptr [esp + 0x10]
            test eax, eax
            mov eax, dword ptr [edx]
            je L00501506
            mov ecx, dword ptr [eax + 4]
            push ecx
            push 0xb5
            call FatalError
            add esp, 8
            jmp L00501518
    L00501506:
            push eax
            call BrPixelmapFree
            jmp L00501515
    L0050150e:
            push 0x43
            call FatalError
    L00501515:
            add esp, 4
    L00501518:
            mov ecx, dword ptr [esp + 0x10]
            mov eax, dword ptr [esp + 0x18]
            add ecx, 4
            dec eax
            mov dword ptr [esp + 0x10], ecx
            mov dword ptr [esp + 0x18], eax
            jne L00501415
    L00501532:
            mov eax, dword ptr [esp + 0x20]
            dec eax
            mov dword ptr [esp + 0x20], eax
            jne L005013af
            pop ebp
            pop esi
            pop edi
    L00501544:
            mov eax, dword ptr [esp + 8]
            pop ebx
            add esp, 0x8e8
            ret 4
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

// FUNCTION: CARMA2_HW 0x005019c0
int C2_NAKED C2_HOOK_FASTCALL LoadShadeTablesFromListFile(tBrender_storage* pStorage_space, FILE* pFile, int pCount) {
#ifndef CARPOCALYPSE2_MATCHING
    int added;
    int total;
    int files_left;
    int tables_left;
    int i;
    int status;
    char* name;
    br_pixelmap* shade_tables[50];
    br_pixelmap** shade_table_cursor;
    br_pixelmap** storage_cursor;
    char line[256];
    tPath_name path;

    total = 0;
    if (pCount > 0) {
        files_left = pCount;
        do {
            PossibleService();
            GetALineAndDontArgue(pFile, line);
            name = strtok(line, "	 ,/");
            PathCat(path, gApplication_path, "SHADETAB");
            PathCat(path, path, name);
            added = 0;
            tables_left = BrPixelmapLoadMany(path, shade_tables, CARPOCALYPSE2_ASIZE(shade_tables));
            if (tables_left == 0) {
                FatalError(kFatalError_CannotLoadShadeTableFileOrItIsEmpty_S, path);
            }
            if (tables_left > 0) {
                shade_table_cursor = shade_tables;
                do {
                    if (*shade_table_cursor != NULL) {
                        status = pStorage_space->shade_tables_count;
                        if (status < pStorage_space->max_shade_tables) {
                            i = 0;
                            if (status > 0) {
                                storage_cursor = pStorage_space->shade_tables;
                                do {
                                    if ((*storage_cursor)->identifier != NULL
                                            && (*shade_table_cursor)->identifier != NULL
                                            && strcmp((*storage_cursor)->identifier, (*shade_table_cursor)->identifier) == 0) {
                                        status = 1;
                                        goto process_shade_table_result;
                                    }
                                    i += 1;
                                    storage_cursor += 1;
                                } while (i < status);
                            }
                            pStorage_space->shade_tables[status] = *shade_table_cursor;
                            pStorage_space->shade_tables_count += 1;
                            status = 2;
                        } else {
                            status = 0;
                        }
process_shade_table_result:
                        switch (status) {
                        case 0:
                            FatalError(kFatalError_InsufficientShadeTableSlots);
                            break;
                        case 1:
                            if (gDisallow_duplicates != 0) {
                                FatalError(kFatalError_DuplicatePixelmap_S, (*shade_table_cursor)->identifier);
                            } else {
                                BrPixelmapFree(*shade_table_cursor);
                            }
                            break;
                        case 2:
                            BrTableAdd(*shade_table_cursor);
                            added += 1;
                            break;
                        }
                    }
                    shade_table_cursor += 1;
                    tables_left -= 1;
                } while (tables_left != 0);
            }
            added = total + added;
            files_left -= 1;
            total = added;
        } while (files_left != 0);
    }
    return total;
#else
    __asm {
            sub esp, 0x2e4
            mov eax, dword ptr [esp + 0x2e8]
            push edi
            mov edi, ecx
            mov dword ptr [esp + 0x1c], edx
            xor ecx, ecx
            mov dword ptr [esp + 0x10], edi
            test eax, eax
            mov dword ptr [esp + 0xc], ecx
            jle L00501ba0
            push esi
            push ebp
            push ebx
            mov dword ptr [esp + 0x24], eax
    L005019ed:
            call PossibleService
            mov ecx, dword ptr [esp + 0x28]
            lea edx, [esp + 0x1f4]
            call GetALineAndDontArgue
            lea eax, [esp + 0x1f4]
            push offset gPed_flags_0058f2e4+0x138
            push eax
            call strtok
            add esp, 8
            mov edx, offset gApplication_path
            lea ecx, [esp + 0x2c]
            mov esi, eax
            push offset gSwap_depth_effect_type+0xdc
            call PathCat
            lea edx, [esp + 0x2c]
            lea ecx, [esp + 0x2c]
            push esi
            call PathCat
            lea ecx, [esp + 0x12c]
            lea edx, [esp + 0x2c]
            push 0x32
            push ecx
            push edx
            mov dword ptr [esp + 0x20], 0
            call BrPixelmapLoadMany
            mov esi, eax
            add esp, 0xc
            test esi, esi
            jne L00501a70
            lea eax, [esp + 0x2c]
            push eax
            push 0x50
            call FatalError
            add esp, 8
            test esi, esi
    L00501a70:
            jle L00501b80
            lea ebx, [esp + 0x12c]
            mov dword ptr [esp + 0x20], esi
            mov dword ptr [esp + 0x10], ebx
    L00501a85:
            cmp dword ptr [ebx], 0
            je L00501b6a
            mov ebp, dword ptr [edi + 0xc]
            mov eax, dword ptr [edi + 0x1c]
            cmp ebp, eax
            jge L00501b15
            xor edx, edx
            test ebp, ebp
            jle L00501af0
            mov edi, dword ptr [edi + 0x34]
    L00501aa1:
            mov ecx, dword ptr [edi]
            mov eax, dword ptr [ecx + 4]
            test eax, eax
            je L00501ae4
            mov ecx, dword ptr [ebx]
            mov esi, dword ptr [ecx + 4]
            test esi, esi
            je L00501ae4
    L00501ab3:
            mov bl, byte ptr [eax]
            mov cl, bl
            cmp bl, byte ptr [esi]
            jne L00501ad7
            test cl, cl
            je L00501ad3
            mov bl, byte ptr [eax + 1]
            mov cl, bl
            cmp bl, byte ptr [esi + 1]
            jne L00501ad7
            add eax, 2
            add esi, 2
            test cl, cl
            jne L00501ab3
    L00501ad3:
            xor eax, eax
            jmp L00501adc
    L00501ad7:
            sbb eax, eax
            sbb eax, -1
    L00501adc:
            test eax, eax
            je L00501b06
            mov ebx, dword ptr [esp + 0x10]
    L00501ae4:
            inc edx
            add edi, 4
            cmp edx, ebp
            jl L00501aa1
            mov edi, dword ptr [esp + 0x1c]
    L00501af0:
            mov edx, dword ptr [edi + 0x34]
            mov eax, dword ptr [ebx]
            mov dword ptr [edx + ebp*4], eax
            mov eax, dword ptr [edi + 0xc]
            inc eax
            mov dword ptr [edi + 0xc], eax
            mov eax, 2
            jmp L00501b17
    L00501b06:
            mov ebx, dword ptr [esp + 0x10]
            mov edi, dword ptr [esp + 0x1c]
            mov eax, 1
            jmp L00501b17
    L00501b15:
            xor eax, eax
    L00501b17:
            sub eax, 0
            je L00501b60
            dec eax
            je L00501b38
            dec eax
            jne L00501b6a
            mov eax, dword ptr [ebx]
            push eax
            call BrTableAdd
            mov eax, dword ptr [esp + 0x18]
            add esp, 4
            inc eax
            mov dword ptr [esp + 0x14], eax
            jmp L00501b6a
    L00501b38:
            mov eax, dword ptr [gDisallow_duplicates]
            test eax, eax
            je L00501b56
            mov ecx, dword ptr [ebx]
            mov edx, dword ptr [ecx + 4]
            push edx
            push 0xb5
            call FatalError
            add esp, 8
            jmp L00501b6a
    L00501b56:
            mov eax, dword ptr [ebx]
            push eax
            call BrPixelmapFree
            jmp L00501b67
    L00501b60:
            push 0x44
            call FatalError
    L00501b67:
            add esp, 4
    L00501b6a:
            mov eax, dword ptr [esp + 0x20]
            add ebx, 4
            dec eax
            mov dword ptr [esp + 0x10], ebx
            mov dword ptr [esp + 0x20], eax
            jne L00501a85
    L00501b80:
            mov ecx, dword ptr [esp + 0x18]
            mov edx, dword ptr [esp + 0x14]
            mov eax, dword ptr [esp + 0x24]
            add ecx, edx
            dec eax
            mov dword ptr [esp + 0x18], ecx
            mov dword ptr [esp + 0x24], eax
            jne L005019ed
            pop ebx
            pop ebp
            pop esi
    L00501ba0:
            mov eax, ecx
            pop edi
            add esp, 0x2e4
            ret 4
            nop
            nop
            nop
            nop
            sub esp, 0x9ec
            mov eax, dword ptr [esp + 0x9f0]
            push ebx
            mov ebx, ecx
            mov dword ptr [esp + 0x1c], edx
            test eax, eax
            mov dword ptr [esp + 0x14], ebx
            mov dword ptr [esp + 8], 0
            jle L00501dbd
            push edi
            push esi
            push ebp
            mov dword ptr [esp + 0x18], eax
    L00501bdf:
            call PossibleService
            mov ecx, dword ptr [esp + 0x28]
            lea edx, [esp + 0x12c]
            call GetALineAndDontArgue
            lea eax, [esp + 0x12c]
            push offset gPed_flags_0058f2e4+0x138
            push eax
            call strtok
            add esp, 8
            mov edx, offset gApplication_path
            lea ecx, [esp + 0x2c]
            mov edi, eax
            push offset gNet_avail_names+0x4b4
            call PathCat
            lea edx, [esp + 0x2c]
            lea ecx, [esp + 0x2c]
            push edi
            call PathCat
            lea ecx, [esp + 0x22c]
            lea edx, [esp + 0x2c]
            push 0x1f4
            push ecx
            push edx
            call BrMaterialLoadMany
            mov esi, eax
            add esp, 0xc
            test esi, esi
            jne L00501c57
            push edi
            push 0x51
            call FatalError
            add esp, 8
    L00501c57:
            mov eax, dword ptr [esp + 0xa04]
            mov edx, esi
            push eax
            lea ecx, [esp + 0x230]
            call GlorifyMaterial
            test esi, esi
            jle L00501dab
            lea ecx, [esp + 0x22c]
            mov dword ptr [esp + 0x1c], esi
            mov dword ptr [esp + 0x10], ecx
    L00501c84:
            mov edx, dword ptr [esp + 0x10]
            mov ecx, dword ptr [edx]
            test ecx, ecx
            mov dword ptr [esp + 0x24], ecx
            je L00501d91
            mov dword ptr [gDuplicate_material], 0
            mov ebp, dword ptr [ebx + 8]
            cmp ebp, dword ptr [ebx + 0x20]
            jge L00501d36
            xor edi, edi
            test ebp, ebp
            jle L00501d02
            mov edx, dword ptr [ebx + 0x38]
    L00501cb5:
            mov eax, dword ptr [edx]
            mov eax, dword ptr [eax + 4]
            test eax, eax
            je L00501cfa
            mov esi, dword ptr [ecx + 4]
            test esi, esi
            je L00501cfa
    L00501cc5:
            mov bl, byte ptr [eax]
            mov cl, bl
            cmp bl, byte ptr [esi]
            jne L00501ce9
            test cl, cl
            je L00501ce5
            mov bl, byte ptr [eax + 1]
            mov cl, bl
            cmp bl, byte ptr [esi + 1]
            jne L00501ce9
            add eax, 2
            add esi, 2
            test cl, cl
            jne L00501cc5
    L00501ce5:
            xor eax, eax
            jmp L00501cee
    L00501ce9:
            sbb eax, eax
            sbb eax, -1
    L00501cee:
            mov ebx, dword ptr [esp + 0x20]
            test eax, eax
            je L00501d23
            mov ecx, dword ptr [esp + 0x24]
    L00501cfa:
            inc edi
            add edx, 4
            cmp edi, ebp
            jl L00501cb5
    L00501d02:
            mov edx, dword ptr [ebx + 0x40]
            mov dword ptr [edx + ebp*4], 0
            mov eax, dword ptr [ebx + 8]
            mov edx, dword ptr [ebx + 0x38]
            mov dword ptr [edx + eax*4], ecx
            mov eax, dword ptr [ebx + 8]
            inc eax
            mov dword ptr [ebx + 8], eax
            mov eax, 2
            jmp L00501d38
    L00501d23:
            mov eax, dword ptr [ebx + 0x38]
            mov ecx, dword ptr [eax + edi*4]
            mov eax, 1
            mov dword ptr [gDuplicate_material], ecx
            jmp L00501d38
    L00501d36:
            xor eax, eax
    L00501d38:
            sub eax, 0
            je L00501d87
            dec eax
            je L00501d5d
            dec eax
            jne L00501d91
            mov eax, dword ptr [esp + 0x10]
            mov ecx, dword ptr [eax]
            push ecx
            call BrMaterialAdd
            mov eax, dword ptr [esp + 0x18]
            add esp, 4
            inc eax
            mov dword ptr [esp + 0x14], eax
            jmp L00501d91
    L00501d5d:
            mov eax, dword ptr [gDisallow_duplicates]
            mov edx, dword ptr [esp + 0x10]
            test eax, eax
            mov eax, dword ptr [edx]
            je L00501d7f
            mov ecx, dword ptr [eax + 4]
            push ecx
            push 0xb4
            call FatalError
            add esp, 8
            jmp L00501d91
    L00501d7f:
            push eax
            call BrMaterialFree
            jmp L00501d8e
    L00501d87:
            push 0x45
            call FatalError
    L00501d8e:
            add esp, 4
    L00501d91:
            mov ecx, dword ptr [esp + 0x10]
            mov eax, dword ptr [esp + 0x1c]
            add ecx, 4
            dec eax
            mov dword ptr [esp + 0x10], ecx
            mov dword ptr [esp + 0x1c], eax
            jne L00501c84
    L00501dab:
            mov eax, dword ptr [esp + 0x18]
            dec eax
            mov dword ptr [esp + 0x18], eax
            jne L00501bdf
            pop ebp
            pop esi
            pop edi
    L00501dbd:
            mov eax, dword ptr [esp + 8]
            pop ebx
            add esp, 0x9ec
            ret 8
            nop
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00502240
int C2_NAKED C2_HOOK_FASTCALL LoadModelsFromListFile(tBrender_storage* pStorage_space, FILE* pFile, int pCount) {
#ifndef CARPOCALYPSE2_MATCHING
    int total;
    int files_left;
    int model_count;
    int i;
    int status;
    char* name;
    br_model* models[2000];
    br_model** model_cursor;
    br_model** storage_cursor;
    char line[256];
    tPath_name path;

    total = 0;
    if (pCount > 0) {
        files_left = pCount;
        do {
            GetALineAndDontArgue(pFile, line);
            name = strtok(line, "	 ,/");
            PathCat(path, gApplication_path, "MODELS");
            PathCat(path, path, name);
            model_count = BrModelLoadMany(path, models, CARPOCALYPSE2_ASIZE(models));
            if (model_count == 0) {
                FatalError(kFatalError_CannotLoadModelFileOrItIsEmpty_S, path);
            }
            if (model_count > 0) {
                model_cursor = models;
                do {
                    status = eStorage_not_enough_room;
                    if (*model_cursor != NULL) {
                        gDuplicate_model = NULL;
                        if (pStorage_space->models_count < pStorage_space->max_models) {
                            if ((pStorage_space->flags & 0x1) && pStorage_space->models_count > 0) {
                                storage_cursor = pStorage_space->models;
                                status = eStorage_not_enough_room;
                                i = 0;
                                do {
                                    if (*storage_cursor != NULL
                                            && (*storage_cursor)->identifier != NULL
                                            && (*model_cursor)->identifier != NULL
                                            && strcmp((*storage_cursor)->identifier, (*model_cursor)->identifier) == 0) {
                                        gDuplicate_model = *storage_cursor;
                                        status = eStorage_duplicate;
                                        goto process_model_result;
                                    }
                                    i++;
                                    storage_cursor++;
                                } while (i < pStorage_space->models_count);
                            }
                            pStorage_space->models[pStorage_space->models_count] = *model_cursor;
                            pStorage_space->models_count += 1;
                            status = eStorage_allocated;
                        }
process_model_result:
                        switch (status) {
                        case eStorage_not_enough_room:
                            FatalError(kFatalError_InsufficientModelSlots);
                            break;
                        case eStorage_duplicate:
                            if (gDisallow_duplicates != 0) {
                                FatalError(kFatalError_DuplicateModel_S, (*model_cursor)->identifier);
                            } else {
                                BrModelFree(*model_cursor);
                            }
                            break;
                        case eStorage_allocated:
                            (*model_cursor)->flags |= BR_MODF_UPDATEABLE;
                            if (gRoad_texturing_level == 0) {
                                ProcessModelFaceMaterials(*model_cursor, (tPMFMCB*)FUN_00448480);
                            }
                            if (gWall_texturing_level == 1) {
                                ProcessModelFaceMaterials(*model_cursor, WallPerspToLinear);
                            }
                            if (gWall_texturing_level == 0) {
                                ProcessModelFaceMaterials(*model_cursor, WallPerspToUntex);
                            }
                            BrModelAdd(*model_cursor);
                            total += 1;
                            break;
                        }
                    }
                    model_cursor += 1;
                    model_count -= 1;
                } while (model_count != 0);
            }
            files_left -= 1;
        } while (files_left != 0);
    }
    return total;
#else
    __asm {
            mov eax, 0x2158
            call _chkstk
            mov eax, dword ptr [esp + 0x215c]
            mov dword ptr [esp + 0x14], edx
            push ebp
            mov ebp, ecx
            test eax, eax
            mov dword ptr [esp + 8], ebp
            mov dword ptr [esp + 0xc], 0
            jle L00502459
            push edi
            push esi
            push ebx
            mov dword ptr [esp + 0x1c], eax
    L00502273:
            mov ecx, dword ptr [esp + 0x24]
            lea edx, [esp + 0x128]
            call GetALineAndDontArgue
            lea eax, [esp + 0x128]
            push offset gPed_flags_0058f2e4+0x138
            push eax
            call strtok
            add esp, 8
            mov edx, offset gApplication_path
            lea ecx, [esp + 0x28]
            mov edi, eax
            push offset gNet_avail_names+0x4ac
            call PathCat
            lea edx, [esp + 0x28]
            lea ecx, [esp + 0x28]
            push edi
            call PathCat
            lea ecx, [esp + 0x228]
            lea edx, [esp + 0x28]
            push 0x7d0
            push ecx
            push edx
            call BrModelLoadMany
            mov esi, eax
            add esp, 0xc
            test esi, esi
            jne L005022e8
            push edi
            push 0x52
            call FatalError
            add esp, 8
            test esi, esi
    L005022e8:
            jle L00502447
            lea ebx, [esp + 0x228]
            mov dword ptr [esp + 0x20], esi
            mov dword ptr [esp + 0x10], ebx
    L005022fd:
            mov ecx, dword ptr [ebx]
            xor eax, eax
            cmp ecx, eax
            je L00502431
            mov dword ptr [gDuplicate_model], eax
            mov edx, dword ptr [ebp + 4]
            cmp edx, dword ptr [ebp + 0x24]
            jge L00502391
            test byte ptr [ebp], 1
            je L0050237d
            xor edi, edi
            cmp edx, eax
            jle L0050237d
            mov ebp, dword ptr [ebp + 0x2c]
    L00502325:
            mov eax, dword ptr [ebp]
            test eax, eax
            je L00502371
            mov eax, dword ptr [eax + 4]
            test eax, eax
            je L00502371
            mov ecx, dword ptr [ebx]
            mov esi, dword ptr [ecx + 4]
            test esi, esi
            je L00502371
    L0050233c:
            mov bl, byte ptr [eax]
            mov cl, bl
            cmp bl, byte ptr [esi]
            jne L00502360
            test cl, cl
            je L0050235c
            mov bl, byte ptr [eax + 1]
            mov cl, bl
            cmp bl, byte ptr [esi + 1]
            jne L00502360
            add eax, 2
            add esi, 2
            test cl, cl
            jne L0050233c
    L0050235c:
            xor eax, eax
            jmp L00502365
    L00502360:
            sbb eax, eax
            sbb eax, -1
    L00502365:
            test eax, eax
            je L00502467
            mov ebx, dword ptr [esp + 0x10]
    L00502371:
            inc edi
            add ebp, 4
            cmp edi, edx
            jl L00502325
            mov ebp, dword ptr [esp + 0x14]
    L0050237d:
            mov ecx, dword ptr [ebp + 0x2c]
            mov eax, dword ptr [ebx]
            mov dword ptr [ecx + edx*4], eax
            mov eax, dword ptr [ebp + 4]
            inc eax
            mov dword ptr [ebp + 4], eax
            mov eax, 2
    L00502391:
            sub eax, 0
            je L00502427
            dec eax
            je L005023ff
            dec eax
            jne L00502431
            mov eax, dword ptr [ebx]
            or byte ptr [eax + 0x20], 0x80
            mov eax, dword ptr [gRoad_texturing_level]
            test eax, eax
            jne L005023bf
            mov ecx, dword ptr [ebx]
            mov edx, offset FUN_00448480
            call FUN_004472c0
    L005023bf:
            cmp dword ptr [gWall_texturing_level], 1
            jne L005023d4
            mov ecx, dword ptr [ebx]
            mov edx, offset WallPerspToLinear
            call FUN_004472c0
    L005023d4:
            mov eax, dword ptr [gWall_texturing_level]
            test eax, eax
            jne L005023e9
            mov ecx, dword ptr [ebx]
            mov edx, offset WallPerspToUntex
            call FUN_004472c0
    L005023e9:
            mov edx, dword ptr [ebx]
            push edx
            call BrModelAdd
            mov eax, dword ptr [esp + 0x1c]
            add esp, 4
            inc eax
            mov dword ptr [esp + 0x18], eax
            jmp L00502431
    L005023ff:
            mov eax, dword ptr [gDisallow_duplicates]
            test eax, eax
            je L0050241d
            mov eax, dword ptr [ebx]
            mov ecx, dword ptr [eax + 4]
            push ecx
            push 0xb3
            call FatalError
            add esp, 8
            jmp L00502431
    L0050241d:
            mov edx, dword ptr [ebx]
            push edx
            call BrModelFree
            jmp L0050242e
    L00502427:
            push 0x46
            call FatalError
    L0050242e:
            add esp, 4
    L00502431:
            mov eax, dword ptr [esp + 0x20]
            add ebx, 4
            dec eax
            mov dword ptr [esp + 0x10], ebx
            mov dword ptr [esp + 0x20], eax
            jne L005022fd
    L00502447:
            mov eax, dword ptr [esp + 0x1c]
            dec eax
            mov dword ptr [esp + 0x1c], eax
            jne L00502273
            pop ebx
            pop esi
            pop edi
    L00502459:
            mov eax, dword ptr [esp + 0xc]
            pop ebp
            add esp, 0x2158
            ret 4
    L00502467:
            mov eax, dword ptr [esp + 0x14]
            mov ebx, dword ptr [esp + 0x10]
            mov ebp, dword ptr [esp + 0x14]
            mov eax, dword ptr [eax + 0x2c]
            mov ecx, dword ptr [eax + edi*4]
            mov eax, 1
            mov dword ptr [gDuplicate_model], ecx
            jmp L00502391
    }
#endif
}

int C2_HOOK_FASTCALL AddShadeTables(tBrender_storage* pStorage_space, const char* pPath) {
    br_pixelmap* temp_array[50];
    int total;
    int i;

    total = BrPixelmapLoadMany(pPath, temp_array, CARPOCALYPSE2_ASIZE(temp_array));
    if (total == 0) {
        FatalError(kFatalError_CannotLoadShadeTableFileOrItIsEmpty_S, pPath);
    }
    for (i = 0; i < total; i++) {
        if (temp_array[i] == NULL) {
            continue;
        }
        switch (AddShadeTableToStorage(pStorage_space, temp_array[i])) {
        case eStorage_allocated:
            BrTableAdd(temp_array[i]);
            break;
        case eStorage_duplicate:
            if (gDisallow_duplicates) {
                FatalError(kFatalError_DuplicatePixelmap_S, temp_array[i]->identifier);
            } else {
                BrPixelmapFree(temp_array[i]);
            }
            break;
        case eStorage_not_enough_room:
            FatalError(kFatalError_InsufficientShadeTableSlots);
            break;
        }
    }
    return total;
}

// FUNCTION: CARMA2_HW 0x00501dd0
void C2_HOOK_FASTCALL LoadModelsFromLineInto(tBrender_storage* pStorage_space, FILE* pFile) {
    tPath_name path;
    char line[256];
    char* name;

    PossibleService();
    GetALineAndDontArgue(pFile, line);
    name = strtok(line, "\t ,/");
    PathCat(path, gApplication_path, "MODELS");
    PathCat(path, path, name);
    AddModels(pStorage_space, path);
}

// FUNCTION: CARMA2_HW 0x00501e40
int C2_HOOK_FASTCALL AddModels(tBrender_storage* pStorage_space, const char* pPath) {
    int i;
    int new_ones;
    int total;
    br_model* temp_array[2000];

    new_ones = 0;
    total = BrModelLoadMany(pPath, temp_array, CARPOCALYPSE2_ASIZE(temp_array));
    WhitenVertexRGB(temp_array, total);
    if (total == 0) {
        FatalError(kFatalError_CannotLoadModelFileOrItIsEmpty_S, pPath);
    }
    for (i = 0; i < total; i++) {
        if (temp_array[i] == NULL) {
            continue;
        }
        switch (AddModelToStorage(pStorage_space, temp_array[i])) {
        case eStorage_allocated:
            temp_array[i]->flags |= BR_MODF_UPDATEABLE;
            temp_array[i]->flags &= ~BR_MODF_DONT_WELD;
            BrModelAdd(temp_array[i]);
            new_ones++;
            break;
        case eStorage_duplicate:
            if (gDisallow_duplicates) {
                FatalError(kFatalError_DuplicateModel_S, temp_array[i]->identifier);
            } else {
                BrModelFree(temp_array[i]);
            }
            break;
        case eStorage_not_enough_room:
            FatalError(kFatalError_InsufficientModelSlots);
            break;
        }
    }
    return new_ones;
}

// FUNCTION: CARMA2_HW 0x00502060
int C2_HOOK_FASTCALL AddMaterials(tBrender_storage* pStorage_space, const char* pPath, tRendererShadingType pShading) {
    int i;
    int new_ones;
    int total;
    br_material* temp_array[500];

    new_ones = 0;
    total = BrMaterialLoadMany(pPath, temp_array, CARPOCALYPSE2_ASIZE(temp_array));
    if (total == 0) {
        FatalError(kFatalError_CannotLoadMaterialFileOrItIsEmpty_S, pPath);
    }
    GlorifyMaterial(temp_array, total, pShading);
    for (i = 0; i < total; ++i) {
        if (temp_array[i] != NULL) {
            switch (AddMaterialToStorage(pStorage_space, temp_array[i])) {
            case eStorage_allocated:
                BrMaterialAdd(temp_array[i]);
                new_ones += 1;
                break;
            case eStorage_duplicate:
                if (gDisallow_duplicates) {
                    FatalError(kFatalError_DuplicateMaterial_S, temp_array[i]->identifier);
                } else {
                    BrMaterialFree(temp_array[i]);
                }
                break;
            case eStorage_not_enough_room:
                FatalError(kFatalError_InsufficientMaterialSlots);
                break;
            }
        }
    }
    return new_ones;
}

// FUNCTION: CARMA2_HW 0x00502210
void C2_HOOK_FASTCALL DodgyModelUpdate(br_model* pM) {
#ifndef CARPOCALYPSE2_MATCHING
    /* Retail drops faces/vertices once the model is prepared to save memory.
     * Readers (ProcessModelFaceMaterials2, the raycasts) need them though:
     * the v11group face_colours union is a 4-byte br_colour array, which
     * cannot hold a material pointer on 64-bit. */
    return;
#else
    BrResFree(pM->faces);
    BrResFree(pM->vertices);
    pM->nfaces = 0;
    pM->nvertices = 0;
    pM->faces = NULL;
    pM->vertices = NULL;
#endif
}

// FUNCTION: CARMA2_HW 0x005024b0
void C2_HOOK_FASTCALL LoadIfItsAPixelmap(const char* pPath) {
    char s[256];

    Uppercaseificate(s, pPath);
    if (strstr(s, ".PIX") != NULL) {
        AddPixelmaps(gStorage_for_callbacks, pPath);
    }
}

// FUNCTION: CARMA2_HW 0x00502490
void C2_HOOK_FASTCALL LoadAllPixelmapsInDirectory(tBrender_storage* pStorage, const char* pPath) {
    gStorage_for_callbacks = pStorage;
    PFForEveryFile(pPath, LoadIfItsAPixelmap);
}

// FUNCTION: CARMA2_HW 0x005026b0
int C2_HOOK_FASTCALL GetFileName(const char *path, tName_list *pList) {
    tPath_name pathCopy;
    tPath_name pathUpper;
    tPath_name dir_path;
    tPath_name stem_path;

    strcpy(pathCopy, path);
    Uppercaseificate(pathUpper, pathCopy);
    if (strstr(pathUpper, ".PIX") == NULL
            && strstr(pathUpper, ".TIF") == NULL) {
        return 0;
    }
    SepDirAndFilename(pathUpper, dir_path, stem_path);
    strcpy(pList->items[pList->size], stem_path);
    if (pList->size < (int)CARPOCALYPSE2_ASIZE(pList->items)) {
        pList->size += 1;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00502780
int C2_HOOK_FASTCALL GetAdditionalFileName(const char* path, tName_list* pList) {
    tPath_name pathCopy;
    tPath_name upperPath;
    tPath_name dir_path;
    tPath_name stem_path;
    int alreadyInList;
    int i;

    strcpy(pathCopy, path);
    Uppercaseificate(upperPath, pathCopy);

    if (strstr(upperPath, ".PIX") == NULL
            && strstr(upperPath, ".P16") == NULL
            && strstr(upperPath, ".P08") == NULL
            && strstr(upperPath, ".TIF") == NULL) {
        return 0;
    }
    SepDirAndFilename(upperPath, dir_path, stem_path);

    for (i = 0, alreadyInList = 0; i < pList->size; i++) {
        if (strcmp(pList->items[i], stem_path) == 0) {
            alreadyInList = 1;
            break;
        }
    }
    if (!alreadyInList) {
        strcpy(pList->items[pList->size], stem_path);
        if (pList->size < (int)CARPOCALYPSE2_ASIZE(pList->items)) {
            pList->size += 1;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x005028f0
void C2_HOOK_FASTCALL LoadAllImagesInDirectory(tBrender_storage* pStorage_space, const char* path) {
    tPath_name pathCopy;
    tPath_name pixPath;
    tPath_name tifPath;
    tName_list list;
    int i;

    C2_HOOK_BUG_ON(sizeof(tBrender_storage) != 68);

    gStorage_for_callbacks = pStorage_space;
    // TwatPIX16(path);
    list.size = 0;
    strcpy(pathCopy, path);
    if (gDisableTiffConversion) {
        PFForEveryFile2(pathCopy, (tEnumPathCallback)GetAdditionalFileName, &list);
    }
    if (!gDisableTiffConversion) {
        PathCat(tifPath, pathCopy, "TIFFX");
        PFForEveryFile2(tifPath, (tEnumPathCallback)GetFileName, &list);
    }
    PathCat(pixPath, pathCopy, "PIX8");
    PFForEveryFile2(pixPath, (tEnumPathCallback)GetAdditionalFileName, &list);
    if (!gDisableTiffConversion) {
        PathCat(tifPath, pathCopy, "TIFFRGB");
        PFForEveryFile2(tifPath, (tEnumPathCallback)GetAdditionalFileName, &list);
    }
    PathCat(pixPath, pathCopy, "PIX16");
    PFForEveryFile2(pixPath, (tEnumPathCallback)GetAdditionalFileName, &list);
    for (i = 0; i < list.size; i++) {
        PathCat(pathCopy, path, list.items[i]);
        AddPixelmaps(gStorage_for_callbacks, pathCopy);
    }
}

// FUNCTION: CARMA2_HW 0x00502a70
void C2_HOOK_FASTCALL LoadIfItsAMaterial(const char* pPath) {
    char path[256];
    tRendererShadingType shading;

    if (gMaterial_shading_for_callback == kRendererShadingType_Undefined) {
        shading = kRendererShadingType_Default;
    } else {
        shading = gMaterial_shading_for_callback;
    }
    Uppercaseificate(path, pPath);
    if (strstr(path, ".MAT") != NULL) {
        AddMaterials(gStorage_for_callbacks, pPath, shading);
    }
}

// FUNCTION: CARMA2_HW 0x00502ad0
void C2_HOOK_FASTCALL LoadAllMaterialsInDirectory(tBrender_storage* pStorage, const char* pPath, tRendererShadingType pShading) {
    gMaterial_shading_for_callback = pShading;
    gStorage_for_callbacks = pStorage;
    PFForEveryFile(pPath, LoadIfItsAMaterial);
    gMaterial_shading_for_callback = kRendererShadingType_Undefined;
}

// FUNCTION: CARMA2_HW 0x00502b20
void C2_HOOK_FASTCALL LoadIfItsAModel(const char* pPath) {
    char s[256];

    Uppercaseificate(s, pPath);
    if (strstr(s, ".DAT") != NULL) {
        AddModels(gStorage_for_callbacks, pPath);
    }
}

// FUNCTION: CARMA2_HW 0x00502b00
void C2_HOOK_FASTCALL LoadAllModelsInDirectory(tBrender_storage *pStorage, const char* pPath) {
    gStorage_for_callbacks = pStorage;
    PFForEveryFile(pPath, LoadIfItsAModel);
}

// FUNCTION: CARMA2_HW 0x00502b80
void C2_HOOK_FASTCALL LoadIfItsAShadeTable(const char* pPath) {
    char path[256];

    Uppercaseificate(path, pPath);
    if (strstr(path, ".TAB") != NULL) {
        AddShadeTables(gStorage_for_callbacks, pPath);
    }
}

// FUNCTION: CARMA2_HW 0x00502b60
void C2_HOOK_FASTCALL LoadAllShadeTablesInDirectory(tBrender_storage* pStorage, const char* pPath) {
    gStorage_for_callbacks = pStorage;
    PFForEveryFile(pPath, LoadIfItsAShadeTable);
}

// FUNCTION: CARMA2_HW 0x00502cf0
void C2_HOOK_FASTCALL LoadAllStuffInDirectory(tBrender_storage* pStorage, const char* pPath, tRendererShadingType pShading) {
    LoadAllShadeTablesInDirectory(pStorage, pPath);
    LoadAllPixelmapsInDirectory(pStorage, pPath);
    LoadAllImagesInDirectory(pStorage, pPath);
    LoadAllMaterialsInDirectory(pStorage, pPath, pShading);
    LoadAllModelsInDirectory(pStorage, pPath);
}

// FUNCTION: CARMA2_HW 0x00502d60
void C2_HOOK_FASTCALL DisallowDuplicates(void) {
    // empty
}

// FUNCTION: CARMA2_HW 0x00502d70
void C2_HOOK_FASTCALL AllowDuplicates(void) {
    gDisallow_duplicates = 0;
}
