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
void C2_HOOK_FASTCALL LoadTrack(const char* pFile_name, tTrack_spec* pTrack_spec, tRace_info* pRace_info) {
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
    if (count_noncar_objects > 40) { /* FIXME: magic number */
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
    for (i = 0; i < 40; i++) { /* FIXME: magic */
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
void C2_HOOK_FASTCALL ClearMatertrialSetFromStorageSpace(tBrender_storage* pStorage_space, int pStart, int pEnd) {
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
        if (pStorage_space->flags & 0x1) { /* FIXME: add enum (0x1 -> eStorage_space_flags_No_duplicates*/
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
int C2_HOOK_FASTCALL LoadPixelmapsFromListFile(tBrender_storage* pStorage_space, FILE* pFile, int pCount) {
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
            name = strtok(line, gSkip_lines_delimiters);
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
}

// FUNCTION: CARMA2_HW 0x005019c0
int C2_HOOK_FASTCALL LoadShadeTablesFromListFile(tBrender_storage* pStorage_space, FILE* pFile, int pCount) {
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
            name = strtok(line, gSkip_lines_delimiters);
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
}

// C2PORT-NONMATCHING: best 62.60% | difftest PASS 2000 trials (spec difftest/specs/00502240_LoadModelsFromListFile.json) | reason regalloc-loop
// FUNCTION: CARMA2_HW 0x00502240
int C2_HOOK_FASTCALL LoadModelsFromListFile(tBrender_storage* pStorage_space, FILE* pFile, int pCount) {
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
            name = strtok(line, gSkip_lines_delimiters);
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
