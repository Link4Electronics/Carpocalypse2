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
#include "loading.h"
#include "opponent.h"
#include "pedestrn.h"
#include "piping.h"
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
        switch (MODE) {                                                                         \
        case eMove_continuous:                                                                  \
            if ((PERIOD) == 0.f) {                                                              \
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
            if ((PERIOD) == 0.f) {                                                              \
                DEST = 0.f;                                                                     \
            } else {                                                                            \
                DEST = (AMPLITUDE)*MapSawToTriangle(SAW(f_the_time, (PERIOD)));                 \
            }                                                                                   \
            break;                                                                              \
        case eMove_flash:                                                                       \
            if (2 * fmodf(f_the_time, (PERIOD)) > (PERIOD)) {                                   \
                DEST = (FLASH_VALUE);                                                           \
            } else {                                                                            \
                DEST = -(FLASH_VALUE);                                                          \
            }                                                                                   \
            break;                                                                              \
        case eMove_texturebits:                                                                 \
            {                                                                                   \
                int sum_flags = 0;                                                              \
                int ii;                                                                         \
                for (ii = 0; ii < (TEXTUREBITS)->count; ii++) {                                 \
                    if ((TEXTUREBITS)->car->field_0x18cc & (1 << (TEXTUREBITS)->bits[ii])) {    \
                        sum_flags |= 1 << ii;                                                   \
                    }                                                                           \
                }                                                                               \
                DEST = (br_scalar)sum_flags;                                                    \
            }                                                                                   \
            break;                                                                              \
        case eMove_harmonic:                                                                    \
        default:                                                                                \
            if ((PERIOD) == 0.f) {                                                              \
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
// GLOBAL: CARMA2_HW 0x00660268
const char* gSoundType_Choices[2] = {"SATURATED", "SCATTERED"};

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

// GLOBAL: CARMA2_HW 0x006a7fc8
tRepair_animation gRepair_animations[25];

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

// GLOBAL: CARMA2_HW 0x00655b50
const char* gFunk_type_names[5] = {
    "spin",
    "rock",
    "throb",
    "slither",
    "roll",
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

// GLOBAL: CARMA2_HW 0x0068b854
br_actor* gGroove_by_proxy_actor;

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
tNet_stored_smash* gNet_host_smashes;

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

// FUNCTION: CARMA2_HW 0x004488f0
tWall_texturing_level C2_HOOK_FASTCALL GetWallTexturingLevel(void) {

    return gWall_texturing_level;
}

// FUNCTION: CARMA2_HW 0x00486c00
int C2_HOOK_FASTCALL IsValidFile(const char* path) {
    struct_c2_stat32 s;

    return c2_stat32(path, &s) == 0;
}

// FUNCTION: CARMA2_HW 0x00486be0
int C2_HOOK_FASTCALL GetLastModificationTime(const char* path) {
    struct_c2_stat32 s;
    int res;

    res = c2_stat32(path, &s);
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

// FUNCTION: CARMA2_HW 0x004862b0
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

// FUNCTION: CARMA2_HW 0x00485fe0
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

// FUNCTION: CARMA2_HW 0x004864a0
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
    size_t filePathLength;
    tPath_name textureName;
    tLoadDirectoryStructureCBResult* itemResult;
    br_pixelmap* texture;

    filePathLength = strlen(filePath);
    if (DRstrcmpi(filePath + filePathLength - 4, ".TIF") != 0) {
        return 0;
    }
    sprintf(textureName, "%.*s", filePathLength - 4, filePath);
    if (!data->isTiffx && data->results != NULL) {
        for (itemResult = data->results; itemResult != NULL; itemResult = itemResult->next) {
            if (DRstrcmpi(textureName, itemResult->name) == 0) {
                return 0;
            }
        }
    }
    if (!(data->loadFlags & kLoadTextureFlags_16bbp)) {
        texture = LoadTiffTexture_Ex2(data->directory, textureName, data->pPalette, data->loadFlags, data->errorCode, 1);
        if (texture == NULL) {
            texture = LoadTiffTexture_Ex2(data->directory, textureName, data->pPalette, data->loadFlags, data->errorCode, 0);
        }
        if (texture != NULL) {
            texture->identifier = BrResStrDup(texture, textureName);
            if (texture->identifier == NULL) {
                BrPixelmapFree(texture);
                *data->errorCode = 2;
                texture = NULL;
            }
        }
        if (texture != NULL) {
            BrPixelmapFree(texture);
        }
    }
    if (*data->errorCode != 0) {
        return *data->errorCode;
    }
    if (!data->isTiffx) {
        return 0;
    }
    itemResult = BrMemAllocate(sizeof(*itemResult), BR_MEMORY_APPLICATION);
    if (itemResult == NULL) {
        *data->errorCode = 2;
        return 2;
    }
    itemResult->name = BrMemStrDup(textureName);
    if (itemResult->name == NULL) {
        *data->errorCode = 2;
        return 2;
    }
    itemResult->next = data->results;
    data->results = itemResult;
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

// FUNCTION: CARMA2_HW 0x004869e0
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

// FUNCTION: CARMA2_HW 0x00486b20
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

// FUNCTION: CARMA2_HW 0x0047d850
int C2_HOOK_FASTCALL DRstrcmpi(const char* str1, const char* str2) {
    return strcasecmp(str1, str2);
}

// FUNCTION: CARMA2_HW 0x0047d860
int C2_HOOK_FASTCALL DRmemicmp(const char* str1, const char* str2, size_t count) {
    return c2_memicmp(str1, str2, count);
}

// FUNCTION: CARMA2_HW 0x00486240
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
void C2_HOOK_FASTCALL LoadSomeMaterials(tBrender_storage *pStorage, FILE* pFile, tRendererShadingType pShading) {
    char s1[256];
    char s2[256];

    PossibleService();
    GetALineAndDontArgue(pFile, s1);
    PathCat(s2, gApplication_path, "MATERIAL");
    PathCat(s2, s2, strtok(s1, "\t ,/"));
    AddMaterials(pStorage, s2, pShading);
}

// FUNCTION: CARMA2_HW 0x00502d80
br_pixelmap* C2_HOOK_FASTCALL SwapPixelmapInStorage(br_pixelmap* pNew, br_pixelmap* pOriginal, tBrender_storage* pStorage) {
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
}

// FUNCTION: CARMA2_HW 0x004761e0
br_uint_32 C2_HOOK_FASTCALL CalcProximities(br_actor* pActor, br_material* pMat, void* pData) {
    int i;
    tFunkotronic_spec* the_funk = pData;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, material, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, proximity_count, 0x150);

    if (pActor->model != NULL) {
        for (i = 0; i < pActor->model->nfaces; i++) {
            br_face* the_face;

            the_face = &pActor->model->faces[i];
            if (the_face->material == the_funk->material) {
                the_funk->proximity_count += 1;
            }
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00476230
br_uint_32 C2_HOOK_FASTCALL AddProximities(br_actor* pActor, br_material* pMat, void* pData) {
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

void C2_HOOK_FASTCALL InitRepairAnimations(void) {
    int i;

    C2_HOOK_BUG_ON(sizeof(tRepair_animation) != 0xc);
    C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(gRepair_animations) != 25);

    for (i = 0; i < CARPOCALYPSE2_ASIZE(gRepair_animations); i++) {
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
void C2_HOOK_FASTCALL ReadSmashableEnvironment(FILE* pF, const char* pPath) {
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
}

// FUNCTION: CARMA2_HW 0x004f5470
int C2_HOOK_FASTCALL SmashFaceMustBeUpdateable(br_material *pMaterial) {
    int i;

    for (i = 0; i < gCount_track_smashable_environment_specs; i++) {
        tSmashable_item_spec *spec = &gTrack_smashable_environment_specs[i];

        if ((spec->trigger_type == kSmashableTrigger_Material && spec->trigger_object.material == pMaterial)
                && ((spec->mode == kSmashableMode_TextureChange && spec->mode_data.texture_change.count_levels > 0)
                || spec->mode == kSmashableMode_Remove
                || spec->mode == kSmashableMode_ReplaceModel)) {
            C2_HOOK_ASSERT(SmashFaceMustBeUpdateable_original(pMaterial) == 1);
            return 1;
        }
    }
    C2_HOOK_ASSERT(SmashFaceMustBeUpdateable_original(pMaterial) == 0);
    return 0;
}

// FUNCTION: CARMA2_HW 0x004f09c0
void C2_HOOK_FASTCALL FreeEnvSmash(tSmashable_item_spec* pSmash) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004f0960
void C2_HOOK_FASTCALL DisposeSmashableEnvironment(void) {
    int i;

    for (i = 0; i < gCount_track_smashable_environment_specs; i++) {
        tSmashable_item_spec* smash = &gTrack_smashable_environment_specs[i];

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
void C2_HOOK_FASTCALL ReinitSmashing(void) {
    int i;

    C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(gSmash_glass_fragments) != 200);
    C2_HOOK_BUG_ON(sizeof(gSmash_glass_fragments[0]) != 0x38);
    C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(gDecals) != 50);
    C2_HOOK_BUG_ON(sizeof(gDecals[0]) != 0x8);
    C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(gQueued_net_smashes) != 50);
    C2_HOOK_BUG_ON(sizeof(gQueued_net_smashes[0]) != 0x38);
    C2_HOOK_BUG_ON(sizeof(tStored_smash) != 0x38);

    for (i = 0; i < CARPOCALYPSE2_ASIZE(gSmash_glass_fragments); i++) {
        gSmash_glass_fragments[i].end_time = 0;
    }
    for (i = 0; i < CARPOCALYPSE2_ASIZE(gDecals); i++) {
        gDecals[i].time = 0;
    }
    for (i = 0; i < CARPOCALYPSE2_ASIZE(gQueued_net_smashes); i++) {
        gQueued_net_smashes[i].field_0x4 = 0;
    }
    gSize_powerup_queue = 0;
    gCount_host_smashes = 0;
    if (gNet_mode == eNet_mode_host) {
        gNet_host_smashes = BrMemAllocate(1000 * sizeof(tStored_smash), kMem_stored_smash);
    }
}

// FUNCTION: CARMA2_HW 0x004f0940
void C2_HOOK_FASTCALL DisposeSmashEnvNonCars(void) {

    BrMemFree(gSmashable_noncars);
}

// FUNCTION: CARMA2_HW 0x004ffd80
void C2_HOOK_FASTCALL ReadSpecialVolumes(FILE* pF) {

    /* DEFAULT ENGINE NOISE */
    gDefault_engine_noise_index = GetAnInt(pF);

    gDefault_water_spec_vol_real = &gDefault_default_water_spec_vol;

    /* # special effects volumes */
    gProgram_state.special_volume_count = GetAnInt(pF);
    if (gProgram_state.special_volume_count != 0) {
        int i;

        C2_HOOK_BUG_ON(sizeof(tSpecial_volume) != 0xdc);
        gProgram_state.special_volumes = BrMemAllocate(gProgram_state.special_volume_count * sizeof(tSpecial_volume), kMem_special_volume);
        for (i = 0; i < gProgram_state.special_volume_count; i++) {
            br_bounds unit_bounds;
            tSpecial_volume* spec = &gProgram_state.special_volumes[i];

            PossibleService();

            C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(gSpecial_effects_boundary_choices) != 4);
            spec->boundary_type = GetALineAndInterpretCommand(pF, gSpecial_effects_boundary_choices, CARPOCALYPSE2_ASIZE(gSpecial_effects_boundary_choices));
            if (spec->boundary_type == eFx_boundary_new) {
                spec->boundary_type = eFx_boundary_box;
            }
            switch (spec->boundary_type) {
            case eFx_boundary_box:
                GetThreeScalars(pF, &spec->boundary.box.mat.m[0][0], &spec->boundary.box.mat.m[0][1], &spec->boundary.box.mat.m[0][2]);
                GetThreeScalars(pF, &spec->boundary.box.mat.m[1][0], &spec->boundary.box.mat.m[1][1], &spec->boundary.box.mat.m[1][2]);
                GetThreeScalars(pF, &spec->boundary.box.mat.m[2][0], &spec->boundary.box.mat.m[2][1], &spec->boundary.box.mat.m[2][2]);
                GetThreeScalars(pF, &spec->boundary.box.mat.m[3][0], &spec->boundary.box.mat.m[3][1], &spec->boundary.box.mat.m[3][2]);
                unit_bounds.min.v[0] = -1.0f;
                unit_bounds.min.v[1] = -1.0f;
                unit_bounds.min.v[2] = -1.0f;
                unit_bounds.max.v[0] =  1.0f;
                unit_bounds.max.v[1] =  1.0f;
                unit_bounds.max.v[2] =  1.0f;
                GetNewBoundingBox(&spec->boundary.box.bounds, &unit_bounds, &spec->boundary.box.mat);
                BrMatrix34Inverse(&spec->boundary.box.inv_mat, &spec->boundary.box.mat);
                spec->boundary.box.axis_length.v[0] = BrVector3Length((br_vector3*)spec->boundary.box.inv_mat.m[0]);
                spec->boundary.box.axis_length.v[1] = BrVector3Length((br_vector3*)spec->boundary.box.inv_mat.m[1]);
                spec->boundary.box.axis_length.v[2] = BrVector3Length((br_vector3*)spec->boundary.box.inv_mat.m[2]);
                ParseSpecialVolume(pF, spec, NULL, spec->boundary_type == eFx_boundary_box);
                if (spec->soundfx_type != kSoundFx_None) {
                    int j;

                    for (j = 0; j < 3; j++) {
                        spec->boundary.box.bounds.min.v[j] -= 10.f;
                        spec->boundary.box.bounds.max.v[j] += 10.f;
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
                FatalError(kFatalError_ProblemWithSpecialVolumeNumber_I, i);
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

void C2_HOOK_FASTCALL LoadStaticLightingForRace(const char* race_lighting_path) {

}

// FUNCTION: CARMA2_HW 0x004489d0
void C2_HOOK_FASTCALL DisposeTexturingMaterials(void) {

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
void C2_HOOK_FASTCALL FreeTrack(tTrack_spec* pTrack_spec) {
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
    for (i = 0; i < 40; i++) { /* FIXME: magic number 40 */
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
}

// FUNCTION: CARMA2_HW 0x004ffd20
void C2_HOOK_FASTCALL FindInverseAndWorldBox(tSpecial_volume* pSpec) {
    br_bounds bnds;

    BrVector3Set(&bnds.min, -1.f, -1.f, -1.f);
    BrVector3Set(&bnds.max,  1.f,  1.f,  1.f);
    GetNewBoundingBox(&pSpec->boundary.box.bounds, &bnds, &pSpec->boundary.box.mat);
    BrMatrix34Inverse(&pSpec->boundary.box.inv_mat, &pSpec->boundary.box.mat);
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

    if (pActor->identifier == NULL) {
        return 0;
    }
    if (pActor->identifier[0] == '&') {
        return 0;
    }
    if (pActor->model != NULL) {
        for (i = 0; i < pActor->model->nvertices; i++) {
            br_vector3 p;

            BrVector3Copy(&p, &pActor->model->vertices[i].p);
            BrMatrix34ApplyP(&pActor->model->vertices[i].p, &p, pMat);
        }
        BrModelUpdate(pActor->model, BR_MODU_ALL);
    }
    BrMatrix34Identity(&pActor->t.t.mat);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00507210
br_uint_32 C2_HOOK_FASTCALL DeleteBastards(br_actor* pActor, br_matrix34* pMat, void* pData) {

    if ((pActor != gAdditional_actors && (pActor->identifier == NULL || pActor->identifier[0] == '&') && Vector3IsZero((br_vector3*)pMat->m[3]))
            || (pActor->model == NULL && pActor->type == BR_ACTOR_MODEL)) {
        int i;

        for (i = 0; i < gDelete_count; i++) {
            if (gDelete_list[i] == pActor) {
                return 0;
            }
        }
        gDelete_list[gDelete_count] = pActor;
        gDelete_count += 1;
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
void C2_HOOK_FASTCALL SaveAdditionalActors(void) {

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
}

// FUNCTION: CARMA2_HW 0x004ffa20
void C2_HOOK_FASTCALL SaveSpecialVolumes(void) {
    tPath_name the_path;
    FILE* f;
    int i;

    PathCat(the_path, gApplication_path, "SPECSAVE.TXT");
    f = DRfopen(the_path, "wt");
    if (f == NULL) {
        return;
    }
    fprintf(f, "// SPECIAL EFFECTS VOLUMES\n\n");
    fprintf(f, "%d\t\t\t\t// # special effects volumes\n\n", gProgram_state.special_volume_count);
    for (i = 0; i < gProgram_state.special_volume_count; i++) {
        tSpecial_volume* v;

        v = &gProgram_state.special_volumes[i];
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
        fprintf(f, "%s\t\t\t\t// Windscreen texture to use\n", (v->screen_pixelmap != NULL) ? v->screen_pixelmap->identifier : "NONE");
        fprintf(f, "%d\t\t\t\t\t// Sound ID of entry noise\n", v->entry_noise);
        fprintf(f, "%d\t\t\t\t\t// Sound ID of exit noise\n", v->exit_noise);
        fprintf(f, "%d\t\t\t\t\t// Engine noise index\n", v->engine_noise_index);
        fprintf(f, "%d\t\t\t\t\t// material index\n", v->material_modifier_index);
        if (v->boundary_type == eFx_boundary_box || v->boundary_type == eFx_boundary_plane) {
            if (v->soundfx_type == kSoundFx_None) {
                fprintf(f,"NONE\t\t\t\t\t// sound type\n");
            } else {
                fprintf(f,"%s\t\t\t\t\t// sound type\n", gSoundType_Choices[v->soundfx_type]);
                WriteOutSoundSpec(f, &v->soundfx_data);
            }
        }
        fprintf(f, "\n");
    }
    PFfclose(f);
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

// FUNCTION: CARMA2_HW 0x00478e30
void C2_HOOK_FASTCALL PathGrooveBastard(tGroovidelic_spec* pGroove, tU32 pTime, br_matrix34* pMat, int pInterrupt_it) {
    br_scalar pos;
    float f_the_time = (float)pTime;

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

            MOVE_FUNK_PARAMETER(pos, pGroove->path_mode, pGroove->path_data.straight_info.period.value, pGroove->path_data.straight_info.texture_info.data, pGroove->path_data.straight_info.x_delta, -pGroove->path_data.straight_info.x_delta);
            pos += pGroove->path_data.straight_info.centre.v[0];
            if (pInterrupt_it) {
                pGroove->path_resumption_value = pos;
                if (pos >= pMat->m[3][0]) {
                    pGroove->path_interrupt_status = eInterrupt_greater_than;
                } else {
                    pGroove->path_interrupt_status = eInterrupt_less_than;
                }
                pMat->m[3][0] = pos;
                BrVector3Copy(&pGroove->actor->t.t.translate.t, &pGroove->path_data.straight_info.centre);
            } else {
                if (pGroove->path_interrupt_status == eInterrupt_none) {
                    pMat->m[3][0] = pos;
                    BrVector3Copy(&pGroove->actor->t.t.translate.t, &pGroove->path_data.straight_info.centre);
                } else if (pGroove->path_interrupt_status == eInterrupt_less_than) {
                    if (pos < pGroove->path_resumption_value) {
                        pGroove->path_interrupt_status = eInterrupt_none;
                        pMat->m[3][0] = pos;
                        BrVector3Copy(&pGroove->actor->t.t.translate.t, &pGroove->path_data.straight_info.centre);
                    }
                } else if (pos > pGroove->path_resumption_value) {
                    pGroove->path_interrupt_status = eInterrupt_none;
                    pMat->m[3][0] = pos;
                    BrVector3Copy(&pGroove->actor->t.t.translate.t, &pGroove->path_data.straight_info.centre);
                }
            }
        }

        if (pGroove->path_data.straight_info.y_delta != 0.0f) {

            MOVE_FUNK_PARAMETER(pos, pGroove->path_mode, pGroove->path_data.straight_info.period.value, pGroove->path_data.straight_info.texture_info.data, pGroove->path_data.straight_info.y_delta, -pGroove->path_data.straight_info.y_delta);
            pos += pGroove->path_data.straight_info.centre.v[1];
            if (pInterrupt_it) {
                pGroove->path_resumption_value = pos;
                if (pos >= pMat->m[3][1]) {
                    pGroove->path_interrupt_status = eInterrupt_greater_than;
                } else {
                    pGroove->path_interrupt_status = eInterrupt_less_than;
                }
                pMat->m[3][1] = pos;
                BrVector3Copy(&pGroove->actor->t.t.translate.t, &pGroove->path_data.straight_info.centre);
            } else {
                if (pGroove->path_interrupt_status == eInterrupt_none) {
                    pMat->m[3][1] = pos;
                    BrVector3Copy(&pGroove->actor->t.t.translate.t, &pGroove->path_data.straight_info.centre);
                } else if (pGroove->path_interrupt_status == eInterrupt_less_than) {
                    if (pos < pGroove->path_resumption_value) {
                        pGroove->path_interrupt_status = eInterrupt_none;
                        pMat->m[3][1] = pos;
                        BrVector3Copy(&pGroove->actor->t.t.translate.t, &pGroove->path_data.straight_info.centre);
                    }
                } else if (pos > pGroove->path_resumption_value) {
                    pGroove->path_interrupt_status = eInterrupt_none;
                    pMat->m[3][1] = pos;
                    BrVector3Copy(&pGroove->actor->t.t.translate.t, &pGroove->path_data.straight_info.centre);
                }
            }
        }

        if (pGroove->path_data.straight_info.z_delta != 0.0f) {

            MOVE_FUNK_PARAMETER(pos, pGroove->path_mode, pGroove->path_data.straight_info.period.value, pGroove->path_data.straight_info.texture_info.data, pGroove->path_data.straight_info.z_delta, -pGroove->path_data.straight_info.z_delta);
            pos += pGroove->path_data.straight_info.centre.v[2];
            if (pInterrupt_it) {
                pGroove->path_resumption_value = pos;
                if (pos >= pMat->m[3][2]) {
                    pGroove->path_interrupt_status = eInterrupt_greater_than;
                } else {
                    pGroove->path_interrupt_status = eInterrupt_less_than;
                }
                BrVector3Copy(&pGroove->actor->t.t.translate.t, &pGroove->path_data.straight_info.centre);
                pMat->m[3][2] = pos;
            } else {
                if (pGroove->path_interrupt_status == eInterrupt_none) {
                    BrVector3Copy(&pGroove->actor->t.t.translate.t, &pGroove->path_data.straight_info.centre);
                    pMat->m[3][2] = pos;
                } else if (pGroove->path_interrupt_status == eInterrupt_less_than) {
                    if (pos < pGroove->path_resumption_value) {
                        pGroove->path_interrupt_status = eInterrupt_none;
                        BrVector3Copy(&pGroove->actor->t.t.translate.t, &pGroove->path_data.straight_info.centre);
                        pMat->m[3][2] = pos;
                    }
                } else if (pos > pGroove->path_resumption_value) {
                    pGroove->path_interrupt_status = eInterrupt_none;
                    BrVector3Copy(&pGroove->actor->t.t.translate.t, &pGroove->path_data.straight_info.centre);
                    pMat->m[3][2] = pos;
                }
            }
        }
        BrVector3Copy(&pGroove->object_position, &pGroove->actor->t.t.translate.t);
        break;
    case eGroove_path_circular:
        BrVector3Copy(&pGroove->actor->t.t.translate.t, &pGroove->path_data.circular_info.centre);
        if (pGroove->path_data.circular_info.axis == eAxis_y) {
            if (pGroove->path_data.circular_info.period.value == 0.0f) {
                pos = 0.f;
            } else {
                pos = cosf(BrAngleToRadian(BrDegreeToAngle(fmodf((float)pTime, pGroove->path_data.circular_info.period.value) / pGroove->path_data.circular_info.period.value * 360.0f))) * pGroove->path_data.circular_info.radius;
            }
            pMat->m[3][0] = pGroove->path_data.circular_info.centre.v[0] + pos;
        } else if (pGroove->path_data.circular_info.axis == eAxis_z) {
            if (pGroove->path_data.circular_info.period.value == 0.0f) {
                pos = 0.f;
            } else {
                pos = sinf(BrAngleToRadian(BrDegreeToAngle(fmodf((float)pTime, pGroove->path_data.circular_info.period.value) / pGroove->path_data.circular_info.period.value * 360.0f))) * pGroove->path_data.circular_info.radius;
            }
            pMat->m[3][0] = pGroove->path_data.circular_info.centre.v[0] + pos;
        }

        if (pGroove->path_data.circular_info.axis == eAxis_x) {
            if (pGroove->path_data.circular_info.period.value == 0.0f) {
                pos = 0.f;
            } else {
                pos = sinf(BrAngleToRadian(BrDegreeToAngle(fmodf((float)pTime, pGroove->path_data.circular_info.period.value) / pGroove->path_data.circular_info.period.value * 360.0f))) * pGroove->path_data.circular_info.radius;
            }
            pMat->m[3][1] = pGroove->path_data.circular_info.centre.v[1] + pos;
        } else if (pGroove->path_data.circular_info.axis == eAxis_z) {
            if (pGroove->path_data.circular_info.period.value == 0.0f) {
                pos = 0.f;
            } else {
                pos = cosf(BrAngleToRadian(BrDegreeToAngle(fmodf((float)pTime, pGroove->path_data.circular_info.period.value) / pGroove->path_data.circular_info.period.value * 360.0f))) * pGroove->path_data.circular_info.radius;
            }
            pMat->m[3][1] = pGroove->path_data.circular_info.centre.v[1] + pos;
        }

        if (pGroove->path_data.circular_info.axis == eAxis_x) {
            if (pGroove->path_data.circular_info.period.value == 0.0f) {
                pos = 0.f;
            } else {
                pos = cosf(BrAngleToRadian(BrDegreeToAngle(fmodf((float)pTime, pGroove->path_data.circular_info.period.value) / pGroove->path_data.circular_info.period.value * 360.0f))) * pGroove->path_data.circular_info.radius;
            }
            pMat->m[3][2] = pGroove->path_data.circular_info.centre.v[1] + pos;
        } else if (pGroove->path_data.circular_info.axis == eAxis_z) {
            if (pGroove->path_data.circular_info.period.value == 0.0f) {
                pos = 0.f;
            } else {
                pos = sinf(BrAngleToRadian(BrDegreeToAngle(fmodf((float)pTime, pGroove->path_data.circular_info.period.value) / pGroove->path_data.circular_info.period.value * 360.0f))) * pGroove->path_data.circular_info.radius;
            }
            pMat->m[3][2] = pGroove->path_data.circular_info.centre.v[1] + pos;
        }
        BrVector3Copy(&pGroove->object_position, &pGroove->actor->t.t.translate.t);
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
    float f_the_time = (float)pTime;

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
            MOVE_FUNK_PARAMETER(pos, pGroove->object_mode, pGroove->object_data.spin_info.period.value, pGroove->object_data.spin_info.texture_info.data, 360.f, -360.f);
            DRMatrix34PostRotateX(pMat, BrDegreeToAngle(pos));
            break;
        case eAxis_y:
            MOVE_FUNK_PARAMETER(pos, pGroove->object_mode, pGroove->object_data.spin_info.period.value, pGroove->object_data.spin_info.texture_info.data, 360.f, -360.f);
            DRMatrix34PostRotateY(pMat, BrDegreeToAngle(pos));
            break;
        case eAxis_z:
            MOVE_FUNK_PARAMETER(pos, pGroove->object_mode, pGroove->object_data.spin_info.period.value, pGroove->object_data.spin_info.texture_info.data, 360.f, -360.f);
            DRMatrix34PostRotateZ(pMat, BrDegreeToAngle(pos));
            break;
        }
        break;
    case eGroove_object_rock:
        MOVE_FUNK_PARAMETER(pos, pGroove->object_mode, pGroove->object_data.rock_info.period.value, pGroove->object_data.rock_info.texture_info.data, pGroove->object_data.rock_info.max_angle, pGroove->object_data.rock_info.max_angle);
        rock_it = 1;
        if (pInterrupt_it) {
            pGroove->object_resumption_value = pos;
            if (pos >= pGroove->object_data.rock_info.current_angle) {
                pGroove->object_interrupt_status = eInterrupt_greater_than;
            } else {
                pGroove->object_interrupt_status = eInterrupt_less_than;
            }
        } else {
            if (pGroove->object_interrupt_status == eInterrupt_less_than) {
                if (pos >= pGroove->object_resumption_value || gAction_replay_mode) {
                    rock_it = 0;
                } else {
                    pGroove->object_interrupt_status = eInterrupt_none;
                }
            } else if (pGroove->object_interrupt_status == eInterrupt_greater_than) {
                if (pos <= pGroove->object_resumption_value || gAction_replay_mode) {
                    rock_it = 0;
                } else {
                    pGroove->object_interrupt_status = eInterrupt_none;
                }
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
        MOVE_FUNK_PARAMETER(z_size, pGroove->object_mode, pGroove->object_data.throb_info.z_period.value, pGroove->object_data.throb_info.z_texture_info.data, pGroove->object_data.throb_info.z_magnitude, -pGroove->object_data.throb_info.z_magnitude);
        MOVE_FUNK_PARAMETER(y_size, pGroove->object_mode, pGroove->object_data.throb_info.y_period.value, pGroove->object_data.throb_info.y_texture_info.data, pGroove->object_data.throb_info.y_magnitude, -pGroove->object_data.throb_info.y_magnitude);
        MOVE_FUNK_PARAMETER(x_size, pGroove->object_mode, pGroove->object_data.throb_info.x_period.value, pGroove->object_data.throb_info.x_texture_info.data, pGroove->object_data.throb_info.x_magnitude, -pGroove->object_data.throb_info.x_magnitude);

        BrMatrix34PostScale(pMat, x_size + 1.f, y_size + 1.f, z_size + 1.f);
        /* FALLTHROUGH */
    case eGroove_object_shear:
        bounds = &pGroove->actor->model->bounds;
        if (pGroove->object_data.shear_info.x_magnitude == 0.0) {
            MOVE_FUNK_PARAMETER(z_size, pGroove->object_mode, pGroove->object_data.shear_info.z_period.value, pGroove->object_data.shear_info.z_texture_info.data, pGroove->object_data.shear_info.z_magnitude, -pGroove->object_data.shear_info.z_magnitude);
            MOVE_FUNK_PARAMETER(y_size, pGroove->object_mode, pGroove->object_data.shear_info.y_period.value, pGroove->object_data.shear_info.y_texture_info.data, pGroove->object_data.shear_info.y_magnitude, -pGroove->object_data.shear_info.y_magnitude);
            BrMatrix34PostShearX(pMat, y_size / (bounds->max.v[1] - bounds->min.v[1]), z_size / bounds->max.v[2] - bounds->min.v[2]);
        } else if (pGroove->object_data.shear_info.y_magnitude == 0.0) {
            MOVE_FUNK_PARAMETER(z_size, pGroove->object_mode, pGroove->object_data.shear_info.z_period.value, pGroove->object_data.shear_info.z_texture_info.data, pGroove->object_data.shear_info.z_magnitude, -pGroove->object_data.shear_info.z_magnitude);
            MOVE_FUNK_PARAMETER(x_size, pGroove->object_mode, pGroove->object_data.shear_info.x_period.value, pGroove->object_data.shear_info.x_texture_info.data, pGroove->object_data.shear_info.x_magnitude, -pGroove->object_data.shear_info.x_magnitude);
            BrMatrix34PostShearY(pMat, x_size / (bounds->max.v[0] - bounds->min.v[0]), z_size / (bounds->max.v[2] - bounds->min.v[2]));
        } else {
            MOVE_FUNK_PARAMETER(y_size, pGroove->object_mode, pGroove->object_data.shear_info.y_period.value, pGroove->object_data.shear_info.y_texture_info.data, pGroove->object_data.shear_info.y_magnitude, -pGroove->object_data.shear_info.y_magnitude);
            MOVE_FUNK_PARAMETER(x_size, pGroove->object_mode, pGroove->object_data.shear_info.x_period.value, pGroove->object_data.shear_info.x_texture_info.data, pGroove->object_data.shear_info.x_magnitude, -pGroove->object_data.shear_info.x_magnitude);
            BrMatrix34PostShearZ(pMat, x_size / (bounds->max.v[0] - bounds->min.v[0]), y_size / (bounds->max.v[1] - bounds->min.v[1]));
        }
        break;
    default:
        return;
    }
}

// FUNCTION: CARMA2_HW 0x004e4650
void C2_HOOK_FASTCALL LollipopizeActor(br_actor* pSubject_actor, br_matrix34* ref_to_world, tLollipop_mode pWhich_axis) {
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
    if (PointOutOfSight(&actor_pos, 0, pGroove->mode == eGroove_mode_distance ? gYon_squared : 36.f)) {
        return;
    }

    the_mat = &the_actor->t.t.mat;
    if ((!gAction_replay_mode
        || !ARReplayIsReallyPaused()
        || pGroove->path_mode == eMove_controlled
        || pGroove->path_mode == eMove_absolute) && !(pGroove->block_flags & 0x1)) {
        PathGrooveBastard(pGroove, pTime, the_mat, pInterrupt_it);
    }
    if (((pGroove->object_type != eGroove_object_none || pGroove->lollipop_mode != eLollipop_none)
            && (!gAction_replay_mode
                    || !ARReplayIsReallyPaused()
                    || pGroove->object_mode == eMove_controlled
                    || pGroove->object_mode == eMove_absolute)) & !(pGroove->block_flags & 0x2)) {
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
        the_actor->t.t.mat.m[3][0] += pGroove->object_position.v[0] + pGroove->object_centre.v[0];
        the_actor->t.t.mat.m[3][1] += pGroove->object_position.v[1] + pGroove->object_centre.v[1];
        the_actor->t.t.mat.m[3][2] += pGroove->object_position.v[2] + pGroove->object_centre.v[2];
        if (pGroove->lollipop_mode != eLollipop_none) {
            LollipopizeActor(pGroove->actor, &gCamera_to_world, pGroove->lollipop_mode);
        }
    }
    if (pGroove->path_interrupt_status != old_path_interrupt || pGroove->object_interrupt_status != old_object_interrupt) {
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
    int i;
    float f_the_time;

    if (gGroovidelics_array != NULL) {
        f_the_time = (float)GetTotalTime();
        gPrevious_groove_times[1] = gPrevious_groove_times[0];
        gPrevious_groove_times[0] = (tU32)f_the_time;

        for (i = 0; i < gGroovidelics_array_size; i++) {
            tGroovidelic_spec* the_groove;

            the_groove = &gGroovidelics_array[i];
            if (the_groove->owner != -999 && !the_groove->done_this_frame) {
                GrooveThisDelic(the_groove, (tU32)f_the_time, 0);
            }
        }
    }
}

float C2_HOOK_FASTCALL MapSawToTriangle(float pNumber) {

    if (pNumber >= 0.5) {
        return 3.0f - pNumber * 4.0f;
    } else {
        return pNumber * 4.0f - 1.0f;
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

    for (i = 0; i < gGroovidelics_array_size; i++) {
        tGroovidelic_spec* the_groove;

        the_groove = &gGroovidelics_array[i];
        the_groove->done_this_frame = 0;
    }
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
void C2_HOOK_FASTCALL StopGroovidelic(br_actor* actor) {
    int i;

    for (i = 0; i < gGroovidelics_array_size; i++) {
        tGroovidelic_spec* groove = &gGroovidelics_array[i];
        if (groove->path_interrupt_status == eInterrupt_none &&
                groove->object_interrupt_status == eInterrupt_none) {
            GrooveThisDelic(groove, gPrevious_groove_times[i], 1);
        }
    }
}

// FUNCTION: CARMA2_HW 0x0047b360
tGroovidelic_spec* C2_HOOK_FASTCALL ActorsGroove(br_actor* pActor) {
    int i;

    for (i = 0; i < gGroovidelics_array_size; i++) {
        tGroovidelic_spec* groove = &gGroovidelics_array[i];

        if (groove->actor == pActor) {
            return groove;
        }
    }
    return NULL;
}

// FUNCTION: CARMA2_HW 0x0047b160
void C2_HOOK_FASTCALL EnableGroovidelic(br_actor *pActor, tU32 pBlock_flags) {
    int i;

    for (i = 0; i < gGroovidelics_array_size; i++) {
        tGroovidelic_spec* groove = &gGroovidelics_array[i];

        if (groove->actor == pActor) {
            groove->block_flags &= ~pBlock_flags;
        }
    }
}

// FUNCTION: CARMA2_HW 0x004ff120
tSpecial_volume* C2_HOOK_FASTCALL FindSpecialVolume(br_vector3* pP, tSpecial_volume* pLast_vol, int pArg3) {
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
}

// FUNCTION: CARMA2_HW 0x004748a0
void C2_HOOK_FAKE_THISCALL ControlBoundFunkGroove(int pSlot_number, undefined4 pArg2, float pValue) {

    if (pSlot_number < 0) {
        return;
    }
    if (pSlot_number >= CARPOCALYPSE2_ASIZE(gGroove_funk_bindings)) {
        FatalError(kFatalError_UsedRefNumOfControlledGroovidelicFunkotronicIsOutOfRange);
    }
    if (!gGroove_funk_bindings[pSlot_number].field_0x4) {
        *gGroove_funk_bindings[pSlot_number].v = pValue;
    }
}

// FUNCTION: CARMA2_HW 0x004748e0
float C2_HOOK_FAKE_THISCALL ControlBoundFunkGroovePlus(int pSlot_number, undefined4 pArg2, float pValue) {

    if (pSlot_number < 0) {
        return 0.f;
    }
    if (pSlot_number >= CARPOCALYPSE2_ASIZE(gGroove_funk_bindings)) {
        FatalError(kFatalError_UsedRefNumOfControlledGroovidelicFunkotronicIsOutOfRange);
    }
    if (!gGroove_funk_bindings[pSlot_number].field_0x4) {
        *gGroove_funk_bindings[pSlot_number].v = fmodf(*gGroove_funk_bindings[pSlot_number].v + pValue, 1.f);
    }
    return *gGroove_funk_bindings[pSlot_number].v;
}

// FUNCTION: CARMA2_HW 0x00506e50
void C2_HOOK_FASTCALL ProcessTrack(br_actor* pWorld, tTrack_spec* pTrack_spec, br_actor* pCamera, br_matrix34* pCamera_to_world_transform) {

    RenderTrack(pWorld, pTrack_spec, pCamera, pCamera_to_world_transform);
    if (gAdditional_actors) {
        BrZbsSceneRenderAdd(gAdditional_actors);
    }
}