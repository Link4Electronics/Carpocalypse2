#include "spark.h"

#include "car.h"
#include "depth.h"
#include "errors.h"
#include "globvars.h"
#include "globvrpb.h"
#include "graphics.h"
#include "loading.h"
#include "opponent.h"
#include "pedestrn.h"
#include "physics.h"
#include "piping.h"
#include "replay.h"
#include "trig.h"
#include "utility.h"

#include "carpocalypse2_macros.h"

#include "c2_string.h"
#include "spark.h"

#include "funks.h"

extern tCar_spec* gRecalc_mass_car;

void C2_HOOK_FASTCALL SingleSplash(tPhysics_object* pObject, const br_matrix34* pMatrix, const br_vector3* pVelocity, const br_vector3* pNormal_velocity, const br_vector3* pOmega, const br_vector3* pPoint, const br_vector3* pNormal, tU32 pTime);

#define CHARS1_TO_INT(A)            ((A) - '0')
#define CHARS2_TO_INT(A, B)         (10 * CHARS1_TO_INT(A) + CHARS1_TO_INT(B))
#define CHARS3_TO_INT(A, B, C)      (10 * CHARS2_TO_INT(A, B) + CHARS1_TO_INT(C))
#define CHARS4_TO_INT(A, B, C, D)   (10 * CHARS3_TO_INT(A, B, C) + CHARS1_TO_INT(D))

typedef struct tFunk_record {           // stride 0x7c
    tU8 field_0x0[0x40];
    int funk_index;                     // 0x40
    tU8 field_0x44[0x7c - 0x44];
} tFunk_record;

typedef struct tFunk_list {
    tU8 field_0x0[0x38];
    tU16 funk_count;                    // 0x38
    tU16 field_0x3a;
    tFunk_record* funk_records;         // 0x3c
} tFunk_list;

typedef struct tFunk_container {        /* br_actor->user of a funk-car burp with bit specs */
    tU8 field_0x0[0x8];
    tFunk_list* funk_list;              // 0x8
} tFunk_container;

typedef struct tCarFunks_master {       // assumed layout from FUNK data in car specs
    tU8 field_0x0[0x270];
    int count_0x270;
    tCar_bit_spec bit_0x274[3];
    int count_0x2b0;
    tCar_bit_spec bit_0x2b4[3];
} tCarFunks_master;
// GLOBAL: CARMA2_HW 0x006b7840
int gShade_list[16];

// GLOBAL: CARMA2_HW 0x00660140
int* gShade_table = &gShade_list[8]; /* FIXME: rename to gDust_table*/

// GLOBAL: CARMA2_HW 0x006a82b4
int gNum_dust_tables;

// GLOBAL: CARMA2_HW 0x006aa588
br_model* gShrapnel_model[2];

// GLOBAL: CARMA2_HW 0x006a9180
tShrapnel gShrapnel[30];

// GLOBAL: CARMA2_HW 0x006aa59c
int gColumn_flags;

// GLOBAL: CARMA2_HW 0x006aa5a0
int gColumn_next;

// GLOBAL: CARMA2_HW 0x006a82ac
int gSplash_next;

// GLOBAL: CARMA2_HW 0x006aa578
int gSpark_next;

// GLOBAL: CARMA2_HW 0x006aa380
br_model* gLollipop_model;

// GLOBAL: CARMA2_HW 0x006a8638
br_pixelmap* gFlame_map[20];

// GLOBAL: CARMA2_HW 0x006a96a8
tSmoke_column gSmoke_column[10];

// GLOBAL: CARMA2_HW 0x006aa570
int gSplash_flags;

// GLOBAL: CARMA2_HW 0x006a8758
br_model* gSplash_model;

// GLOBAL: CARMA2_HW 0x006aa5a4
int gNum_splash_types;

// GLOBAL: CARMA2_HW 0x006a9130
br_material* gSplash_material[20];

// GLOBAL: CARMA2_HW 0x006a82b8
tSplash gSplash[32];

// GLOBAL: CARMA2_HW 0x006aa57c
int gSpark_flags;

// GLOBAL: CARMA2_HW 0x006aa5ac
br_pixelmap* gIt_shade_table;

// GLOBAL: CARMA2_HW 0x006aa568
int gDust_rotate;

// GLOBAL: CARMA2_HW 0x006aa388
tReplay_splash gReplay_splashes[10];

// GLOBAL: CARMA2_HW 0x006aa574
int gReplay_splash_flags;

// GLOBAL: CARMA2_HW 0x006b7884
tU32 gNext_AFE_color_cycle;

// GLOBAL: CARMA2_HW 0x006aa5b0
int gAFE_color_cycle;

// GLOBAL: CARMA2_HW 0x006aa584
int gShrapnel_flags;

// GLOBAL: CARMA2_HW 0x006a82b0
br_camera* gSpark_cam;

// GLOBAL: CARMA2_HW 0x006a8718
br_matrix4 gCameraToScreen;

// GLOBAL: CARMA2_HW 0x006a9b80
tSpark gSparks[32];

// GLOBAL: CARMA2_HW 0x006a8760
tBRender_smoke* gBR_smoke_pointers[35];

// GLOBAL: CARMA2_HW 0x006aa56c
int gN_BR_smoke_structs;

// GLOBAL: CARMA2_HW 0x006a8ce0
tSmoke gSmoke[25];

// GLOBAL: CARMA2_HW 0x006aa590
int gSmoke_flags;

// GLOBAL: CARMA2_HW 0x006aa594
int gN_smoke;

void (C2_HOOK_FASTCALL * StopCarBeingIt_original)(tCar_spec* pCar);
// GLOBAL: CARMA2_HW 0x006b7880
br_material* gBlack_material;

// GLOBAL: CARMA2_HW 0x00660110
int gSmoke_on = 1;

static br_scalar C2_HOOK_CDECL SmokeScalarRound(double pValue) {
    volatile union { br_scalar scalar; tU32 bits; } stored;
    union { br_scalar scalar; tU32 bits; } result;

    stored.scalar = (br_scalar)pValue;
    result.bits = stored.bits;
    return result.scalar;
}

static tU32 C2_HOOK_CDECL SmokeScalarBits(const br_scalar* pValue) {
    tU32 bits;

    memcpy(&bits, pValue, sizeof(bits));
    return bits;
}

static double C2_HOOK_CDECL SplashSubtractFromScalar(double pValue, const br_scalar* pScalar) {
    double result;

#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        mov ecx, pScalar
        fld pValue
        fsubr dword ptr [ecx]
        fstp result
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ volatile (
        "fldl %[value]\n\t"
        "fsubrs %[scalar]\n\t"
        "fstpl %[result]\n\t"
        : [result] "=m" (result)
        : [value] "m" (pValue), [scalar] "m" (*pScalar)
        : "st", "memory");
#else
    result = (double)*pScalar - pValue;
#endif
    return result;
}

static int C2_HOOK_CDECL SplashReplayMoving(void) {
    br_scalar rate;
    tU32 bits;

    rate = ARGetReplayRate();
    bits = SmokeScalarBits(&rate) & 0x7fffffffu;
    return bits != 0 && bits <= 0x7f800000u;
}

static tS32 C2_HOOK_CDECL SparkFtol(double pValue) {
    tU32 words[2];

    memcpy(words, &pValue, sizeof(words));
    if ((words[1] & 0x7fffffffu) >= 0x43e00000u) {
        return 0;
    }
    return (tS32)(tU32)(br_int_64)pValue;
}

static int C2_HOOK_CDECL SparkClipInteger(int pOrigin, int pOther, int pAmount, int pDivisor) {
    tS32 product;

    product = (tS32)(((tU32)pOrigin - (tU32)pOther) * (tU32)pAmount);
    return (tS32)((tU32)pOrigin - (tU32)(product / pDivisor));
}

static void C2_HOOK_CDECL SparkClipDepth(br_vector3* pPoint, const br_vector3* pOther, int pAmount, int pDivisor) {
    pPoint->v[2] = SmokeScalarRound((double)pOther->v[2]
        - (((double)pOther->v[2] - pPoint->v[2]) * pAmount) / pDivisor);
}

// FUNCTION: CARMA2_HW 0x004fb910
void C2_HOOK_FASTCALL GenerateSmokeShades(void) {
    static int rb = 0x00;
    static int gb = 0x00;
    static int bb = 0x00;

    static int rl = 0x40;
    static int gl = 0x40;
    static int bl = 0x40;

    static int rs = 0xc8;
    static int gs = 0xc8;
    static int bs = 0xc8;

    static int rm = 0x80;
    static int gm = 0x80;
    static int bm = 0x80;

    static int rd = 0xfe;
    static int gd = 0xdf;
    static int bd = 0x43;

#define RGB(R, G, B) (((R) << 16) | ((G) << 8) | ((B) << 0))

    gShade_list[0] = RGB(rb, gb, bb);
    gShade_list[1] = RGB(rl, gl, bl);
    gShade_list[3] = RGB(rs, gs, bs);
    gShade_list[4] = gShade_list[3];
    gShade_list[2] = RGB(rm, gm, bm);
    gShade_list[7] = RGB(rd, gd, bd);

#undef RGB

}

// FUNCTION: CARMA2_HW 0x004fdb70
void C2_HOOK_FASTCALL GetSmokeShadeTables(FILE* f) {
    int i;
    int red;
    int green;
    int blue;
    float quarter;
    float half;
    float three_quarter;

    gNum_dust_tables = GetAnInt(f);
    if (gNum_dust_tables > 8) {
        gNum_dust_tables = 8;
    }
    for (i = 0; i < gNum_dust_tables; i++) {
        PossibleService();
        GetThreeInts(f, &red, &green, &blue);
        GetThreeScalars(f, &quarter, &half, &three_quarter);
        gShade_table[i] = (red << 16) | (green << 8) | (blue << 0);
    }
}

// FUNCTION: CARMA2_HW 0x00516240
void C2_HOOK_FASTCALL ModelScale(br_model* pModel, float pScale) {
    int i;

    for (i = 0; i < pModel->nvertices; i++) {

        BrVector3Scale(&pModel->vertices[i].p, &pModel->vertices[i].p, pScale);
    }
    BrModelUpdate(pModel, BR_MODU_ALL);
}

// FUNCTION: CARMA2_HW 0x004f87a0
void C2_HOOK_FASTCALL ResetSparks(void) {

    gSpark_flags = 0;
}

void C2_HOOK_FASTCALL LoadInShrapnel(void) {

    gShrapnel_model[0] = LoadModel("FRAG4.DAT");
    ModelScale(gShrapnel_model[0], 0.5f);
    gShrapnel_model[1] = LoadModel("FRAG5.DAT");
    ModelScale(gShrapnel_model[1], 0.5f);
    BrModelAdd(gShrapnel_model[0]);
    BrModelAdd(gShrapnel_model[1]);
    gBlack_material = GetSimpleMaterial("M14.MAT", 4);
    gNon_track_actor->material = gBlack_material;
}

void C2_HOOK_FASTCALL InitShrapnel(void) {
    int i;

    for (i = 0; i < CARPOCALYPSE2_ASIZE(gShrapnel); i++) {
        gShrapnel[i].actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
        gShrapnel[i].actor->parent = NULL;
        gShrapnel[i].actor->model = gShrapnel_model[i % 2];
        gShrapnel[i].actor->render_style = BR_RSTYLE_DEFAULT;
        gShrapnel[i].actor->t.type = BR_TRANSFORM_MATRIX34;
        gShrapnel[i].actor->material = BrMaterialFind("DEBRIS.MAT");
        gShrapnel[i].age = 0;
        gShrapnel[i].shear1 = FRandomBetween(-2.f, 2.f);
        gShrapnel[i].shear2 = FRandomBetween(-2.f, 2.f);
        BrVector3SetFloat(&gShrapnel[i].axis,
            FRandomBetween(-1.f, 1.f),
            FRandomBetween(-1.f, 1.f),
            FRandomBetween(-1.f, 1.f));
        BrVector3Normalise(&gShrapnel[i].axis, &gShrapnel[i].axis);
    }
}

// FUNCTION: CARMA2_HW 0x004fc3a0
void C2_HOOK_FASTCALL InitFlame(void) {
    int i;
    int j;
    char name[256];
    br_actor* actor;
    br_material* material;

    gColumn_flags = 0;
    gLollipop_model = BrModelAllocate("Lollipop", 4, 2);
    strcpy(name, "FLAMES.PIX");
    strcpy(gCurrent_load_directory, "COMMON");
    if (LoadTextureTryAllLocations(name, gFlame_map, CARPOCALYPSE2_ASIZE(gFlame_map)) != CARPOCALYPSE2_ASIZE(gFlame_map)) {
        FatalError(kFatalError_CantLoadPixelmapFile_S, name);
    }
    C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(gFlame_map) != 20);
    BrMapAddMany(gFlame_map, CARPOCALYPSE2_ASIZE(gFlame_map));

    C2_HOOK_BUG_ON(sizeof(tSmoke_column) != 124);
    C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(gSmoke_column) != 10);
    for (i = 0; i < CARPOCALYPSE2_ASIZE(gSmoke_column); i++) {

        gSmoke_column[i].flame_actor = BrActorAllocate(BR_ACTOR_NONE, NULL);
        for (j = 0; j < CARPOCALYPSE2_ASIZE(gSmoke_column[i].frame_count); j++) {
            actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
            material = BrMaterialAllocate(NULL);
            BrActorAdd(gSmoke_column[i].flame_actor, actor);
            actor->model = gLollipop_model;
            actor->material = material;
            material->flags &= ~BR_MATF_LIGHT;
            material->flags |= BR_MATF_ALWAYS_VISIBLE;
            material->colour_map = gFlame_map[0];
            GlorifyMaterial(&material, 1, kRendererShadingType_AmbientOnly);
            BrMaterialAdd(material);
            gSmoke_column[i].frame_count[j] = 100;
        }
    }
    gLollipop_model->nvertices = 4;
    BrVector3SetFloat(&gLollipop_model->vertices[0].p, -.5f, 0.f, .0f);
    BrVector3SetFloat(&gLollipop_model->vertices[1].p, .5f, 0.f, .0f);
    BrVector3SetFloat(&gLollipop_model->vertices[2].p, .5f, 1.f, .0f);
    BrVector3SetFloat(&gLollipop_model->vertices[3].p, -.5f, 1.f, .0f);
    BrVector2Set(&gLollipop_model->vertices[0].map, 0.f, 1.f);
    BrVector2Set(&gLollipop_model->vertices[1].map, 1.f, 1.f);
    BrVector2Set(&gLollipop_model->vertices[2].map, 1.f, 0.f);
    BrVector2Set(&gLollipop_model->vertices[3].map, 0.f, 0.f);

    gLollipop_model->nfaces = 2;
    gLollipop_model->faces[0].vertices[0] = 0;
    gLollipop_model->faces[0].vertices[1] = 1;
    gLollipop_model->faces[0].vertices[2] = 2;
    gLollipop_model->faces[1].vertices[0] = 0;
    gLollipop_model->faces[1].vertices[1] = 2;
    gLollipop_model->faces[1].vertices[2] = 3;
    gLollipop_model->faces[0].smoothing = 1;
    gLollipop_model->faces[1].smoothing = 1;
    BrModelAdd(gLollipop_model);
}

// FUNCTION: CARMA2_HW 0x004fdde0
void C2_HOOK_FASTCALL InitSplash(FILE* pF) {
    int i;
    int num_files;
    int num;
    br_actor* actor;
    char the_path[256];
    char s[256];
    br_pixelmap* splash_maps[20];

    gSplash_flags = 0;
    gSplash_model = BrModelAllocate("Splash", 4, 2);
    strcpy(gCurrent_load_directory, "COMMON");
    if (pF != NULL) {
        num = GetAnInt(pF);
        gNum_splash_types = 0;
        for (i = 0; i < num; i++) {
            GetAString(pF, s);
#if 0
            PathCat(the_path, the_path, s);
#else
            strcpy(the_path, s);
#endif
            num_files = LoadTextureTryAllLocations(the_path, &splash_maps[gNum_splash_types], CARPOCALYPSE2_ASIZE(splash_maps));
            if (num_files == 0) {
                FatalError(kFatalError_CantLoadPixelmapFile_S, the_path);
            }
            gNum_splash_types += num_files;
        }
    } else {
        strcpy(the_path, "SPLSHBLU.PIX");
        gNum_splash_types = LoadTextureTryAllLocations(the_path, splash_maps, CARPOCALYPSE2_ASIZE(splash_maps));
    }
    BrMapAddMany(splash_maps, gNum_splash_types);
    for (i = 0; i < gNum_splash_types; i++) {
        gSplash_material[i] = BrMaterialAllocate(NULL);
        gSplash_material[i]->flags &= ~BR_MATF_LIGHT;
        gSplash_material[i]->flags |= BR_MATF_ALWAYS_VISIBLE;
        gSplash_material[i]->colour_map = splash_maps[i];
        BrMaterialAdd(gSplash_material[i]);
    }
    gSplash_model->nvertices = 4;
    BrVector3SetFloat(&gSplash_model->vertices[0].p, -0.5f, 0.0f, 0.0f);
    BrVector3SetFloat(&gSplash_model->vertices[1].p,  0.5f, 0.0f, 0.0f);
    BrVector3SetFloat(&gSplash_model->vertices[2].p,  0.5f, 1.0f, 0.0f);
    BrVector3SetFloat(&gSplash_model->vertices[3].p, -0.5f, 1.0f, 0.0f);
    BrVector2Set(&gSplash_model->vertices[0].map, 0.0f, 1.0f);
    BrVector2Set(&gSplash_model->vertices[1].map, 1.0f, 1.0f);
    BrVector2Set(&gSplash_model->vertices[2].map, 1.0f, 0.0f);
    BrVector2Set(&gSplash_model->vertices[3].map, 0.0f, 0.0f);
    gSplash_model->nfaces = 2;
    gSplash_model->faces[0].vertices[0] = 0;
    gSplash_model->faces[0].vertices[1] = 1;
    gSplash_model->faces[0].vertices[2] = 2;
    gSplash_model->faces[1].vertices[0] = 0;
    gSplash_model->faces[1].vertices[1] = 2;
    gSplash_model->faces[1].vertices[2] = 3;
    gSplash_model->faces[0].smoothing = 1;
    gSplash_model->faces[1].smoothing = 1;
    BrModelAdd(gSplash_model);
    for (i = 0; i < CARPOCALYPSE2_ASIZE(gSplash); i++) {
        gSplash[i].actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
        actor = gSplash[i].actor;
        actor->model = gSplash_model;
        if (gNum_splash_types != 0) {
            actor->material = gSplash_material[IRandomBetween(0, gNum_splash_types - 1)];
        } else {
            actor->material = NULL;
        }
        gSplash[i].scale_x = SRandomBetween(0.9f, 1.1f) * (2 * IRandomBetween(0, 1) - 1);
    }
}

// FUNCTION: CARMA2_HW 0x004fdc00
void C2_HOOK_FASTCALL FreeSmokeShadeTables(void) {

}

// FUNCTION: CARMA2_HW 0x004fdc10
void C2_HOOK_FASTCALL LoadInKevStuff(FILE* pF) {

    PackFileRevertTiffLoading();
    PossibleService();
    LoadInShrapnel();
    PossibleService();
    InitShrapnel();
    PossibleService();
    InitFlame();
    PossibleService();
    InitSplash(pF);
    PackFileRerevertTiffLoading();
}

// UNLABELED: CARMA2_HW 0x004fc2e0
void C2_HOOK_FASTCALL DisposeFlames(void) {
    int i;
    int j;
    br_actor* child;

    for (i = 0; i < CARPOCALYPSE2_ASIZE(gFlame_map); i++) {
        BrMapRemove(gFlame_map[i]);
        BrPixelmapFree(gFlame_map[i]);
    }
    for (i = 0; i < CARPOCALYPSE2_ASIZE(gSmoke_column); i++) {
        if ((gColumn_flags & (1 << i)) != 0 && gSmoke_column[i].colour == 0) {
            BrActorRemove(gSmoke_column[i].flame_actor);
        }
        child = gSmoke_column[i].flame_actor->children;
        for (j = 0; j < 3; j++) {
            BrMaterialRemove(child->material);
            BrMaterialFree(child->material);
            child = child->next;
        }
        BrActorFree(gSmoke_column[i].flame_actor);
    }
    BrModelRemove(gLollipop_model);
    BrModelFree(gLollipop_model);
}

// FUNCTION: CARMA2_HW 0x004fe180
void C2_HOOK_FASTCALL DisposeKevStuff(void) {
    int i;

    for (i = 0; i < CARPOCALYPSE2_ASIZE(gShrapnel); i++) {
        if ((gShrapnel_flags & (1 << i)) != 0 && gShrapnel[i].actor->parent != NULL) {
            BrActorRemove(gShrapnel[i].actor);
        }
        BrActorFree(gShrapnel[i].actor);
    }
    BrModelRemove(gShrapnel_model[0]);
    BrModelRemove(gShrapnel_model[1]);
    BrModelFree(gShrapnel_model[0]);
    BrModelFree(gShrapnel_model[1]);
    DisposeFlames();
    for (i = 0; i < gNum_splash_types; i++) {
        BrMapRemove(gSplash_material[i]->colour_map);
        BrPixelmapFree(gSplash_material[i]->colour_map);
        BrMaterialRemove(gSplash_material[i]);
        BrMaterialFree(gSplash_material[i]);
    }
    for (i = 0; i < CARPOCALYPSE2_ASIZE(gSplash); i++) {
        if ((gSplash_flags & (1 << i)) != 0 && gSplash[i].actor->parent != NULL) {
            BrActorRemove(gSplash[i].actor);
        }
        BrActorFree(gSplash[i].actor);
    }
    BrModelRemove(gSplash_model);
    BrModelFree(gSplash_model);
}

// UNLABELED: CARMA2_HW 0x004f9180
void C2_HOOK_CDECL AFEModelUpdate(br_actor* pActor, br_model* pModel, br_material* pMaterial, void* pOrder_table, br_uint_8 pStyle, int pOn_screen) {
    int i;
    int j;
    int time;
    int fade;
    v11model* prepared;
    v11group* group;
    br_vertex* vertex;

    if (gAFE_color_cycle) {
        time = (int)GetTotalTime() >> 1;
        if (pModel != NULL) {
            prepared = pModel->prepared;
            if (prepared->ngroups != 0) {
                for (i = 0; i < prepared->ngroups; i++) {
                    group = &prepared->groups[i];
                    for (j = 0; j < group->nvertices; j++) {
                        vertex = &pModel->vertices[group->vertex_user[j]];
                        if (vertex == NULL) {
                            continue;
                        }
                        fade = time & 0x3fff;
                        if (fade >= 0x100) {
                            fade = 0x3ff - fade;
                        }
                        vertex->red = (br_uint_8)fade;
                        fade = (time + 0x100) & 0x3ff;
                        if (fade >= 0x100) {
                            fade = 0x3ff - fade;
                        }
                        vertex->grn = (br_uint_8)fade;
                        fade = (time - 0x200) & 0x3ff;
                        if (fade >= 0x100) {
                            fade = 0x3ff - fade;
                        }
                        vertex->blu = (br_uint_8)fade;
                    }
                }
            }
            if (pModel->flags & BR_MODF_UPDATEABLE) {
                BrModelUpdate(pModel, BR_MODU_VERTICES);
            }
        }
    }
    BrZsModelRender(pActor, pModel, pMaterial, pOrder_table, pStyle, BrOnScreenCheck(&pModel->bounds), 0);
}

// UNLABELED: CARMA2_HW 0x004f95e0
int C2_HOOK_FASTCALL SetModelAFEUpdateCB(br_model* pModel) {

    pModel->flags |= BR_MODF_CUSTOM;
    pModel->custom = AFEModelUpdate;
    return 1;
}

// UNLABELED: CARMA2_HW 0x004f9600
int C2_HOOK_FASTCALL SetMaterialPrelitCB(br_material* pMaterial) {

    if (pMaterial->flags & BR_MATF_PRELIT) {
        return 0;
    }
    pMaterial->flags |= BR_MATF_PRELIT;
    BrMaterialUpdate(pMaterial, BR_MATU_RENDERING | BR_MATU_LIGHTING);
    return 1;
}

// UNLABELED: CARMA2_HW 0x004fec30
intptr_t C2_HOOK_CDECL ForEveryActorModelNoGrooves(br_actor* pActor, void* pContext) {
    tUser_crush_data* user_crush_data;
    tModelUpdate_cbfn* callback;

    callback = pContext;
    if (pActor->type != BR_ACTOR_MODEL) {
        return 0;
    }
    if (pActor->model == NULL) {
        return 0;
    }
    user_crush_data = pActor->user;
    if (user_crush_data == NULL) {
        return 0;
    }
    if (user_crush_data->groove != NULL) {
        return 0;
    }
    callback(pActor->model);
    return 0;
}

// FUNCTION: CARMA2_HW 0x004fe360
void C2_HOOK_FASTCALL DisposeKevStuffCar(tCar_spec* pCar) {
    int i;
    int j;
    tCar_crush_spec* spec;
    br_actor* actor;
    br_model* model;
    tUser_crush_data* user;
    tCar_crush_buffer_entry* crush;
    br_material* solid_mat;

    if (!gAFE) {
        return;
    }
    spec = pCar->car_crush_spec;
    DRActorEnumRecurse(pCar->car_model_actor, ActorFunks, (void*)MasterDisableFunkotronic);
    for (i = 0; i < spec->field_0x270; i++) {
        actor = spec->field_0x274[i].field_0x0;
        user = (tUser_crush_data*)actor->user;
        if (user == NULL) {
            continue;
        }
        crush = user->crush_data;
        if (crush == NULL) {
            continue;
        }
        if (crush->smashables == NULL) {
            continue;
        }
        for (j = 0; j < crush->count_smashables; j++) {
            int funk_index = crush->smashables[j].funk;
            if (funk_index >= 0) {
                MasterDisableFunkotronic(funk_index);
            }
        }
    }
    for (i = 0; i < spec->field_0x2b0; i++) {
        actor = spec->field_0x2b4[i].field_0x0;
        user = (tUser_crush_data*)actor->user;
        if (user == NULL) {
            continue;
        }
        crush = user->crush_data;
        if (crush == NULL) {
            continue;
        }
        if (crush->smashables == NULL) {
            continue;
        }
        for (j = 0; j < crush->count_smashables; j++) {
            int funk_index = crush->smashables[j].funk;
            if (funk_index >= 0) {
                MasterDisableFunkotronic(funk_index);
            }
        }
    }
    solid_mat = BrMaterialFind("solidgrn");
    if (solid_mat == NULL) {
        return;
    }
    if (solid_mat->colour_map == NULL) {
        return;
    }
    gRecalc_mass_car = pCar;
    pCar->field_0x1960 = solid_mat->colour_map;
    ForEveryCarMaterial(pCar, SetMaterialPrelitCB, 1);
    pCar->shell_model->custom = AFEModelUpdate;
    pCar->shell_model->flags |= BR_MODF_CUSTOM;
    user = (tUser_crush_data*)pCar->car_model_actor->user;
    model = user->models[1];
    if (model != NULL) {
        model->flags |= BR_MODF_CUSTOM;
        model->custom = AFEModelUpdate;
    }
    DRActorEnumRecurse(pCar->car_model_actor, ForEveryActorModelNoGrooves, (void*)SetModelAFEUpdateCB);
    for (i = 0; i < spec->field_0x270; i++) {
        model = spec->field_0x274[i].field_0x0->model;
        model->flags |= BR_MODF_CUSTOM;
        model->custom = AFEModelUpdate;
    }
    for (i = 0; i < spec->field_0x2b0; i++) {
        model = spec->field_0x2b4[i].field_0x0->model;
        model->flags |= BR_MODF_CUSTOM;
        model->custom = AFEModelUpdate;
    }
}

// FUNCTION: CARMA2_HW 0x004fe570
void C2_HOOK_FASTCALL StopCarBeingIt(tCar_spec* pCar) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL MungeSplash(tU32 pTime) {

    if (gNum_splash_types == 0) {
        return;
    }
    if (gAction_replay_mode && ARGetReplayRate() != 0.f) {
        tVehicle_type type;
        int i;

        for (type = eVehicle_net_player; type <= eVehicle_rozzer; type++) {

            for (i = 0; i < (type == eVehicle_self ? 1 : GetCarCount(type)); i++) {
                tCar_spec* car;

                if (type == eVehicle_self) {
                    car = &gProgram_state.current_car;
                } else {
                    car = GetCarSpec(type, i);
                }
                if (car->collision_info->water_d != 10000.f && !(car != NULL && car->driver == eDriver_local_human)) {
                    CreateSplash(car->collision_info, pTime);
                }
            }
        }
        if (gProgram_state.current_car.collision_info->water_d != 10000.f) {
            CreateSplash(gProgram_state.current_car.collision_info, pTime);
        }
        for (i = 0; i < CARPOCALYPSE2_ASIZE(gReplay_splashes); i++) {
            tReplay_splash *replay_splash = &gReplay_splashes[i];

            if (gReplay_splash_flags & (1 << i)) {
                switch (replay_splash->type) {
                case 1:
                    {
                        const char* ident = replay_splash->object.actor->identifier;
                        tPipe_chunk_data *chunk_non_car = FindNextChunk(ePipe_chunk_non_car, CHARS4_TO_INT(ident[4], ident[5], ident[6], ident[7]));
                        if (chunk_non_car != NULL) {
                            br_vector3 vel;
                            br_vector3 rot;

                            GetVelocitiesFromMatrices(&vel, &rot, &replay_splash->object.actor->t.t.mat, &chunk_non_car->data.non_car.matrix, pTime);
                            GeneralCreateSplash(NULL, &vel, &rot, &replay_splash->normal, replay_splash->water_density, &replay_splash->bb, &replay_splash->object.actor->t.t.mat, pTime);
                        }
                    }
                    break;
                case 2:
                    {
                        int id = (replay_splash->object.ped - gPedestrian_array) | (replay_splash->object.ped->field_0x06 << 16);
                        br_actor* actor = GetCharacterActorPtr(replay_splash->object.ped->character);
                        tPipe_chunk_data *chunk_ped_physics = FindNextChunk(ePipe_chunk_ped_physics, id);
                        if (chunk_ped_physics != NULL) {
                            br_vector3 vel;
                            br_vector3 rot;

                            GetVelocitiesFromMatrices(&vel, &rot, &actor->t.t.mat, &chunk_ped_physics->data.ped_physics.matrix, pTime);
                            GeneralCreateSplash(NULL, &vel, &rot, &replay_splash->normal, replay_splash->water_density, &replay_splash->bb, &replay_splash->object.actor->t.t.mat, pTime);
                        }
                    }
                    break;
                case 255:
                    {
                        tPipe_chunk_data *chunk_ped_phil_object = FindNextChunk(ePipe_chunk_phil_object, (uintptr_t)replay_splash->object.phil_object);
                        if (chunk_ped_phil_object != NULL) {
                            br_vector3 vel;
                            br_vector3 rot;

                            GetVelocitiesFromMatrices(&vel, &rot, &replay_splash->object.actor->t.t.mat, &chunk_ped_phil_object->data.phil_object.matrix, pTime);
                            GeneralCreateSplash(NULL, &vel, &rot, &replay_splash->normal, replay_splash->water_density, &replay_splash->bb, &replay_splash->object.actor->t.t.mat, pTime);
                        }
                    }
                    break;
                }
            }
        }
    } else if (!gAction_replay_mode) {
        tPhysics_object* object;

        for (object = PHILGetFirstObject(); object != NULL; object = PHILGetNextObject(object)) {

            if (object->water_d != 10000.f && object->last_special_volume != NULL && object->last_special_volume->gravity_multiplier < 1.f) {
                if (object != NULL && object->owner != NULL && object->flags_0x238 == 1 && object->owner != NULL && ((tCar_spec*)object->owner)->driver == eDriver_local_human) {
                    CreateSplash(object, 100);
                } else {
                    CreateSplash(object, pTime);
                }
            }
        }
    }
    if (gSplash_flags) {
        int i;

        for (i = 0; i < CARPOCALYPSE2_ASIZE(gSplash); i++) {
            tSplash *splash = &gSplash[i];

            if (!(gSplash_flags & (1 << i))) {
                continue;
            }
            if (splash->just_done || (gAction_replay_mode && ARGetReplayRate() == 0.f)) {
                br_scalar dt;

                dt = splash->size * splash->scale_x;
                BrVector3Scale((br_vector3*)splash->actor->t.t.mat.m[0], (br_vector3*)gCamera_to_world.m[0], dt);
                BrVector3Scale((br_vector3*)splash->actor->t.t.mat.m[1], (br_vector3*)gCamera_to_world.m[1], splash->size);
                BrVector3Scale((br_vector3*)splash->actor->t.t.mat.m[2], (br_vector3*)gCamera_to_world.m[2], splash->size);
                if (gProgram_state.cockpit_on) {
                    br_scalar ts;

                    ts = sqrtf(CARPOCALYPSE2_SQR(gCamera_to_world.m[0][2]) + CARPOCALYPSE2_SQR(gCamera_to_world.m[0][0]));
                    DRMatrix34PreRotateZ(&splash->actor->t.t.mat, -FastScalarArcTan2Angle(gCamera_to_world.m[0][1], ts));
                }
                splash->just_done = 0;
            } else {
                gSplash_flags &= ~(1 << i);
                if (splash->actor->parent != NULL) {
                    BrActorRemove(splash->actor);
                }
            }
        }
    }
}

void C2_HOOK_FASTCALL ReplayShrapnel(tU32 pTime) {
    int i;
    br_matrix34* mat;

    for (i = 0; i < CARPOCALYPSE2_ASIZE(gShrapnel); i++) {
        mat = &gShrapnel[i].actor->t.t.mat;
        if (gShrapnel_flags & (1 << i)) {
            gShrapnel[i].age += (tU32)(ARGetReplayRate() * (float)pTime);
            DrMatrix34Rotate(mat, gShrapnel[i].age * BR_ANGLE_DEG(1), &gShrapnel[i].axis);
            BrMatrix34PreShearX(mat, gShrapnel[i].shear1, gShrapnel[i].shear2);
        }
    }
}

void C2_HOOK_FASTCALL KillShrapnel(int i) {

    if (gShrapnel[i].actor->parent != NULL) {
        BrActorRemove(gShrapnel[i].actor);
    }
    gShrapnel_flags &= ~(1 << i);
}

// FUNCTION: CARMA2_HW 0x004f9790
void C2_HOOK_FASTCALL MungeShrapnel(tU32 pTime) {

    MungeSmokeColumn(pTime);
    MungeSplash(pTime);

    if (gAFE) {
        tU32 now = GetTotalTime();
        if (now - gNext_AFE_color_cycle > 100) {
            gAFE_color_cycle = 1;
            gNext_AFE_color_cycle = now;
        } else {
            gAFE_color_cycle = 0;
        }
    }
    if (gAction_replay_mode) {
        ReplayShrapnel(pTime);
    } else {
        int i;

        ARStartPipingSession(ePipe_chunk_shrapnel);
        for (i = 0; i < CARPOCALYPSE2_ASIZE(gShrapnel); i++) {
            br_matrix34* mat;
            br_scalar ts;

            mat = &gShrapnel[i].actor->t.t.mat;
            if (!(gShrapnel_flags & (1 << i))) {
                continue;
            }
            if (gShrapnel[i].age == -1) {
                KillShrapnel(i);
            } else {
                br_vector3 disp;

                if (gShrapnel[i].time_sync != 0) {
                    BrVector3Scale(&disp, &gShrapnel[i].v, (float)gShrapnel[i].time_sync / 1000.f);
                    gShrapnel[i].time_sync = 0;
                } else {
                    BrVector3Scale(&disp, &gShrapnel[i].v, pTime / 1000.0f);
                    gShrapnel[i].age += pTime;
                }
                BrVector3Accumulate((br_vector3*)mat->m[3], &disp);
                gShrapnel[i].v.v[1] -= (float)(10 * pTime) / 1000.f / WORLD_SCALE; /* Fall acceleration */
                DrMatrix34Rotate(mat, BR_ANGLE_DEG(gShrapnel[i].age), &gShrapnel[i].axis);
                BrMatrix34PreShearX(mat, gShrapnel[i].shear1, gShrapnel[i].shear2);
                ts = 1.f - BrVector3Length(&gShrapnel[i].v) / 1.4f * (float)pTime / 1000.f;
                if (ts < .1f) {
                    ts = .1f;
                }
                BrVector3Scale(&gShrapnel[i].v, &gShrapnel[i].v, ts);
                AddShrapnelToPipingSession(i + ((gShrapnel[i].age > 1000 || pTime > gShrapnel[i].age) << 15), (br_vector3*)mat->m[3], gShrapnel[i].age - pTime, gShrapnel[i].actor->material);
                if (gShrapnel[i].age > 1000) {
                    gShrapnel[i].age = -1;
                }
            }
        }
        AREndPipingSession();
    }
}

static void C2_HOOK_FASTCALL ForEveryModelMaterial(br_model* pModel, tMaterialMaybeUpdate_cbfn* pCallback) {
    int i;
    int need_update = 0;

    if (pModel == NULL) {
        return;
    }
    for (i = 0; i < pModel->prepared->ngroups; i++) {
        br_material *material = pModel->faces[pModel->prepared->groups[i].face_user[0]].material;

        if (material != NULL) {
            need_update |= pCallback(material);
        }
    }
    if (need_update) {
        BrModelUpdate(pModel, 0x10);
    }
}

// FUNCTION: CARMA2_HW 0x004fe8a0
intptr_t C2_HOOK_CDECL ForEveryActorMaterial(br_actor* pActor, void *pContext) {
    tUser_crush_data* user_crush_data;
    tMaterialMaybeUpdate_cbfn* callback;

    callback = pContext;
    if (pActor->type != BR_ACTOR_MODEL) {
        return 0;
    }
    if (pActor->model == NULL) {
        return 0;
    }
    user_crush_data = pActor->user;
    if (user_crush_data == NULL) {
        return 0;
    }
    if (pActor->material != NULL) {
        callback(pActor->material);
    }
    ForEveryModelMaterial(pActor->model, callback);
    return 0;
}

// FUNCTION: CARMA2_HW 0x004fe930
intptr_t C2_HOOK_CDECL ForEveryActorMaterialNoGrooves(br_actor* pActor, void* pContext) {
    tUser_crush_data* user_crush_data;
    tMaterialMaybeUpdate_cbfn* callback;

    callback = pContext;
    if (pActor->type != BR_ACTOR_MODEL) {
        return 0;
    }
    if (pActor->model == NULL) {
        return 0;
    }
    user_crush_data = pActor->user;
    if (user_crush_data == NULL) {
        return 0;
    }
    if (user_crush_data->groove != NULL) {
        return 0;
    }
    if (pActor->material != NULL) {
        callback(pActor->material);
    }
    ForEveryModelMaterial(pActor->model, callback);
    return 0;
}

// FUNCTION: CARMA2_HW 0x004fe640
void C2_HOOK_FASTCALL ForEveryCarMaterial(tCar_spec* pCar_spec, tMaterialMaybeUpdate_cbfn* pCallback, int pGrooves) {
    int i;
    tCar_crush_spec* pCrush_spec = pCar_spec->car_crush_spec;
    tUser_crush_data* user_crush_data;

    if (pCar_spec->car_master_actor->material != NULL) {
        pCallback(pCar_spec->car_master_actor->material);
    }
    ForEveryModelMaterial(pCar_spec->shell_model, pCallback);
    user_crush_data = pCar_spec->car_model_actor->user;
    ForEveryModelMaterial(user_crush_data->models[1], pCallback);
    if (pGrooves) {
        DRActorEnumRecurse(pCar_spec->car_model_actor, ForEveryActorMaterial, pCallback);
    } else {
        DRActorEnumRecurse(pCar_spec->car_model_actor, ForEveryActorMaterialNoGrooves, pCallback);
    }
    for (i = 0; i < pCrush_spec->field_0x270; i++) {
        ForEveryModelMaterial(pCrush_spec->field_0x274[i].field_0x0->model, pCallback);
    }
    for (i = 0; i < pCrush_spec->field_0x2b0; i++) {
        ForEveryModelMaterial(pCrush_spec->field_0x2b4[i].field_0x0->model, pCallback);
    }
}

// FUNCTION: CARMA2_HW 0x004fed50
intptr_t C2_HOOK_FASTCALL UnBlendifyMaterialCB(br_material* pMaterial) {

    if (GetBlendificatiousnessOfMaterial(pMaterial) != 100) {
        BlendifyMaterial(pMaterial, 100);
        BrMaterialUpdate(pMaterial, BR_MATU_ALL);
        return 1;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004fed00
intptr_t C2_HOOK_FASTCALL BlendifyMaterialCB(br_material* pMaterial) {

    if (GetBlendificatiousnessOfMaterial(pMaterial) != 25) {
        BlendifyMaterial(pMaterial, 25);
        BrMaterialUpdate(pMaterial, BR_MATU_ALL);
        return 1;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004fca70
void C2_HOOK_FASTCALL StopCarSmokingInstantly(tCar_spec* pCar_spec) {
    int i;

    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gSmoke_column); i++) {
        if (gSmoke_column[i].core.car == pCar_spec) {
            gSmoke_column[i].lifetime = 0;
        }
    }
}

// FUNCTION: CARMA2_HW 0x004fc740
void C2_HOOK_FASTCALL PipeInstantUnSmudge(tCar_spec* pCar_spec) {
    br_model* model;
    br_model* b_model;
    br_actor* actor;
    br_actor* bonny;
    int j;
    int n;
    int v;
    int group;
    tSmudged_vertex data[1000];

    if (!gAction_replay_mode) {
        return;
    }
    actor = pCar_spec->car_actor;
    model = actor->model;
    bonny = pCar_spec->car_actor;
    n = 0;
    if ((model->flags & BR_MODF_KEEP_ORIGINAL) || (model->flags & BR_MODF_UPDATEABLE)) {
        ARStartPipingSession(ePipe_chunk_smudge);
        j = 0;
        for (group = 0; group < model->prepared->ngroups; group++) {
            for (v = 0; v < model->prepared->groups[group].nvertices; v++) {
                if ((model->prepared->groups[group].vertex_colours[v] >> 24) != 0) {
                    data[n].vertex_index = j;
                    data[n].light_index = -((tS32)(model->prepared->groups[group].vertex_colours[v] >> 24));
                    n += 1;
                    model->prepared->groups[group].vertex_colours[v] = 0;
                    if (model->flags & BR_MODF_UPDATEABLE) {
                        model->vertices[model->prepared->groups[group].vertex_user[v]].index = 0;
                    }
                    if (n >= CARPOCALYPSE2_ASIZE(data)) {
                        group = model->prepared->ngroups;
                        break;
                    }
                }
                j += 1;
            }
        }
        if (n != 0) {
            AddSmudgeToPipingSession(pCar_spec->car_ID, 0, n, data);
        }
        if (bonny != actor) {
            /* This branch is never taken */
            b_model = bonny->model;
            n = 0;
            j = 0;
            for (group = 0; group < b_model->prepared->ngroups; group++) {
                for (v = 0; v < b_model->prepared->groups[group].nvertices; v++) {
                    if ((b_model->prepared->groups[group].vertex_colours[v] >> 24) != 0) {
                        data[n].vertex_index = j;
                        data[n].light_index = -b_model->prepared->groups[group].nvertices;
                        n += 1;
                        b_model->prepared->groups[group].vertex_colours[v] = 0;
                        if (b_model->flags & BR_MODF_UPDATEABLE) {
                            b_model->vertices[b_model->prepared->groups[group].vertex_user[v]].index = 0;
                        }
                        if (n >= CARPOCALYPSE2_ASIZE(data)) {
                            group = b_model->prepared->groups[group].nvertices;
                            break;
                        }
                    }
                    j += 1;
                }
            }
            if (n != 0) {
                AddSmudgeToPipingSession(pCar_spec->car_ID, 0, n, data);
            }
        }
        AREndPipingSession();
    }
}

// FUNCTION: CARMA2_HW 0x004fca40
void C2_HOOK_FASTCALL StopCarSmoking(tCar_spec* pCar) {
    int i;

    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gSmoke_column); i++) {
        if (gSmoke_column[i].core.car == pCar && gSmoke_column[i].lifetime > 2000) {
            gSmoke_column[i].lifetime = 2000;
        }
    }
}

// FUNCTION: CARMA2_HW 0x004fca90
void C2_HOOK_FASTCALL StopObjectSmokingInstantly(tPhysics_object* pObject) {
    int i;

    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gSmoke_column); i++) {

        if (gSmoke_column[i].core.collision_info == pObject) {
            gSmoke_column[i].lifetime = 0;
        }
    }
}

// FUNCTION: CARMA2_HW 0x004fcab0
void C2_HOOK_FASTCALL ConditionalSmokeColumn(tCar_spec* pCar, int fire_point, int level) {
    int i;
    int j;
    int flags;
    tU16 fire_value;

    if (level == 0 && (pCar == NULL || pCar->driver < 7)) {
        level = 1;
    }
    flags = gColumn_flags;
    for (i = 0; i < CARPOCALYPSE2_ASIZE(gSmoke_column); i++) {
        if ((flags & (1 << i)) && gSmoke_column[i].core.car == pCar) {
            for (j = 0; j < CARPOCALYPSE2_ASIZE(gSmoke_column); j++) {
                if (gSmoke_column[j].core.car != pCar) {
                    continue;
                }
                if (!(flags & (1 << j)) || gSmoke_column[j].colour > level) {
                    gSmoke_column[j].lifetime = 0x7d0;
                } else if (gSmoke_column[j].lifetime != 0) {
                    return;
                } else {
                    gSmoke_column[j].lifetime = 0x7d0;
                }
            }
            break;
        }
    }
    fire_value = pCar->fire_vertex[fire_point];
    if (pCar->collision_info != NULL && pCar->collision_info->last_special_volume != NULL) {
        if (pCar->collision_info->last_special_volume->gravity_multiplier < 1.0f) {
            return;
        }
    }
    SmudgeCar(pCar, fire_value);
    if (pCar->car_crush_spec != NULL) {
        SmudgeCar(pCar, pCar->fire_vertex[IRandomBetween(0, 0xb)]);
    }
    CreateSmokeColumn2(1, pCar->car_master_actor, pCar, NULL, level, fire_value, 0x989680);
}

// FUNCTION: CARMA2_HW 0x004fa5b0
void C2_HOOK_FASTCALL GenerateContinuousSmoke(tCar_spec* pCar, int pWheel, tU32 pTime) {
    br_vector3 pos;
    br_vector3 v;
    br_vector3 vcs;
    br_vector3 tv;
    br_scalar decay_factor;
    br_scalar ts;
    br_scalar alpha;
    br_scalar beta;
    int colour;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, dust_time, 0x1284);

    if (pTime < (tU32)pCar->dust_time[pWheel]) {
        pCar->dust_time[pWheel] -= pTime;
        return;
    }
    pCar->dust_time[pWheel] += IRandomBetween(200, 400) - pTime;
    if (pCar->dust_time[pWheel] < 0) {
        pCar->dust_time[pWheel] = 0;
    }
    BrVector3Cross(&tv, &pCar->collision_info->omega, &pCar->wpos[pWheel]);
    BrVector3Scale(&vcs, &pCar->collision_info->velocity_car_space, WORLD_SCALE);
    BrVector3Accumulate(&vcs, &tv);
    ts = BrVector3LengthSquared(&vcs);
    if (ts < 25.f) {
        return;
    }
    decay_factor = sqrtf(ts) / 25.f;
    if (decay_factor > 1.f) {
        decay_factor = 1.f;
    }
    BrVector3InvScale(&tv, &pCar->wpos[pWheel], WORLD_SCALE);
    tv.v[1] -= pCar->oldd[pWheel] / WORLD_SCALE;

    alpha = -1000.0f;
    if (vcs.v[2] < 0.0f) {
        alpha = (pCar->collision_info->bb2.max.v[2] - tv.v[2]) / vcs.v[2];
    } else if (vcs.v[2] > 0.0f) {
        alpha = (pCar->collision_info->bb2.min.v[2] - tv.v[2]) / vcs.v[2];
    }

    beta = -1000.0f;
    if (vcs.v[0] < 0.0f) {
        beta = (pCar->collision_info->bb2.max.v[0] - tv.v[0]) / vcs.v[0];
    } else if (vcs.v[0] < 0.0f) {
        beta = (pCar->collision_info->bb2.min.v[0] - tv.v[0]) / vcs.v[0];
    }

    ts = MAX(alpha, beta);
    BrVector3Scale(&pos, &vcs, ts);
    BrVector3Accumulate(&tv, &pos);
    BrMatrix34ApplyP(&pos, &tv, &pCar->car_master_actor->t.t.mat);
    BrMatrix34ApplyV(&v, &vcs, &pCar->car_master_actor->t.t.mat);

    colour = gDust_rotate + gCurrent_race.material_modifiers[pCar->material_index[pWheel]].smoke_type - 2;
    while (colour >= gNum_dust_tables) {
        colour -= gNum_dust_tables;
    }
    CreatePuffOfSmoke(&pos, &v, decay_factor, 2 * decay_factor, colour + 8);
}

// FUNCTION: CARMA2_HW 0x004fb330
void C2_HOOK_FASTCALL CreatePuffOfSmoke(br_vector3* pPos, br_vector3* pV, float pStrength, float pDecay_factor, int pType) {
    br_vector3 d;
    int i;

    if (gSmoke_on == 0) {
        return;
    }

    d.v[0] = pPos->v[0] - gCamera_to_world.m[3][0];
    d.v[1] = pPos->v[1] - gCamera_to_world.m[3][1];
    d.v[2] = pPos->v[2] - gCamera_to_world.m[3][2];
    if (BrVector3LengthSquared(&d) <= 625.f) {
        return;
    }

    i = gN_smoke;
    gSmoke[i].v.v[0] = pV->v[0] * 0.1449275f;
    gSmoke[i].v.v[1] = pV->v[1] * 0.1449275f - 2948170.25f;
    gSmoke[i].v.v[2] = pV->v[2] * 0.1449275f;
    gSmoke[i].pos = *pPos;
    if ((pType & 0xf) == 7) {
        gSmoke[i].radius = 0.1f;
    } else {
        gSmoke[i].radius = 0.05f;
        gSmoke[i].pos.v[1] += 0.04f;
    }
    gSmoke[i].strength = (pStrength < 1.f) ? 1.f : pStrength;
    gSmoke[i].pos.v[1] += 0.04f;
    gSmoke[i].time_sync = gPHIL_mechanics_time_sync;
    gSmoke[i].decay_factor = pDecay_factor;
    gSmoke[i].type = pType;
    gSmoke[i].pipe_me = 1;
    gSmoke_flags |= 1 << i;

    gN_smoke = i + 1;
    if (gN_smoke == CARPOCALYPSE2_ASIZE(gSmoke)) {
        gN_smoke = 0;
    }
}

// FUNCTION: CARMA2_HW 0x004fee70
void C2_HOOK_FASTCALL ClearSplashes(tPhysics_object* pObject) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004fb630
void C2_HOOK_FASTCALL CreateSmokeColumn2(int pType, br_actor* pActor, tCar_spec* pCar, tPhysics_object* pObject, int pColour, int pVertex, tU32 pLifetime) {
    tSmoke_column* old_column;
    tSmoke_column* column;
    tSmoke_column_core core;
    br_actor* flame;
    int i;

    old_column = &gSmoke_column[gColumn_next];
    if (pObject == NULL) {
        pObject = pCar == NULL ? NULL : pCar->collision_info;
    }
    core.field_0x0 = pType;
    core.car = pCar;
    core.collision_info = pObject;
    core.car_actor = pActor;
    if (!gSmoke_on) {
        return;
    }
    if ((gColumn_flags & (1u << (gColumn_next & 31))) != 0) {
        ARStartPipingSession((tPipe_chunk_type)17);
        column = &gSmoke_column[gColumn_next];
        AddSmokeColumnToPipingSession(gColumn_next, &column->core, column->vertex_index, column->colour);
        AREndPipingSession();
        if (gSmoke_column[gColumn_next].colour == 0) {
            ARStartPipingSession((tPipe_chunk_type)18);
            flame = old_column->flame_actor->children;
            for (i = 0; i < 3; i++) {
                if (flame->type == 1) {
                    AddFlameToPipingSession((gColumn_next << 4) + i, old_column->frame_count[i],
                        old_column->scale_x[i], old_column->scale_y[i], old_column->offset_x[i], old_column->offset_z[i]);
                }
                flame->type = 0;
                flame = flame->next;
            }
            AREndPipingSession();
        }
    }
    if (pColour == 0) {
        if ((gColumn_flags & (1u << (gColumn_next & 31))) == 0 || gSmoke_column[gColumn_next].colour != 0) {
            BrActorAdd(gNon_track_actor, gSmoke_column[gColumn_next].flame_actor);
        }
    } else if ((gColumn_flags & (1u << (gColumn_next & 31))) != 0 && gSmoke_column[gColumn_next].colour == 0) {
        BrActorRemove(gSmoke_column[gColumn_next].flame_actor);
    }
    ARStartPipingSession((tPipe_chunk_type)17);
    AddSmokeColumnToPipingSession(gColumn_next, &core, pVertex, pColour);
    AREndPipingSession();
    column = &gSmoke_column[gColumn_next];
    column->core = core;
    column->colour = pColour;
    column->lifetime = pLifetime;
    column->time = 0;
    column->smudge_timer = 1000;
    column->vertex_index = pVertex;
    column->upright = 1;
    gColumn_flags |= 1u << (gColumn_next & 31);
    column->frame_count[0] = 100;
    column->frame_count[1] = 100;
    column->frame_count[2] = 100;
    gColumn_next++;
    if (gColumn_next >= 10) {
        gColumn_next = 0;
    }
}

void C2_HOOK_FASTCALL ReplaySmokeColumn(tU32 pTime) {
    int i;

    for (i = 0; i < CARPOCALYPSE2_ASIZE(gSmoke_column); i++) {
        if (gColumn_flags & (1 << i)) {
            br_vector3 dummy;

            DoSmokeColumn(i, pTime, &dummy);
            if (gSmoke_column[i].colour == 0) {
                FlameAnimate(i, &gSmoke_column[i].pos, pTime);
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x004fba40
void C2_HOOK_FASTCALL MungeSmokeColumn(tU32 pTime) {
    int i;

    if (gColumn_flags == 0) {
        return;
    }
    if (gAction_replay_mode) {
        ReplaySmokeColumn(pTime);
        return;
    }

    gPHIL_mechanics_time_sync = 1;
    for (i = 0; i < CARPOCALYPSE2_ASIZE(gSmoke_column); i++) {
        if (!(gColumn_flags & (1 << i))) {
            continue;
        }
        if (gSmoke_column[i].lifetime >= pTime) {
            tPhysics_object* object;
            tCar_spec* c;
            br_vector3 car_pos;

            gSmoke_column[i].lifetime -= pTime;
            object = gSmoke_column[i].core.collision_info;
            c = gSmoke_column[i].core.car;
            DoSmokeColumn(i, pTime, &car_pos);
            if (gSmoke_column[i].core.field_0x0 == 1 && c->car_crush_spec->field_0x4b8 != 0) {
                if (IsOnNonCarHalf(c, &car_pos)) {
                    gSmoke_column[i].lifetime = 0;
                }
            }
            if (gSmoke_column[i].colour == 0) {
                FlameAnimate(i, &gSmoke_column[i].pos, pTime);
                if (gSmoke_column[i].core.field_0x0 == 1) {
                    if (gSmoke_column[i].smudge_timer >= pTime) {
                        gSmoke_column[i].smudge_timer -= pTime;
                    } else {
                        gSmoke_column[i].smudge_timer += 2000;
                        SmudgeCar(c, gSmoke_column[i].vertex_index);
                        if (c->knackered) {
                            int plane;

                            plane = IRandomBetween(0, CARPOCALYPSE2_ASIZE(gSmoke_column[i].core.car->fire_vertex) - 1);
                            SmudgeCar(c, gSmoke_column[i].core.car->fire_vertex[plane]);
                        }
                    }
                }
            }
            gSmoke_column[i].time += pTime;
            if (gSmoke_column[i].time > 200) {
                br_vector3 pos;
                br_vector3 v;
                br_scalar decay_factor;

                gSmoke_column[i].time -= pTime;
                gSmoke_column[i].count += 1;
                pos.v[0] = gSmoke_column[i].pos.v[0] + SRandomBetween(-.03f, .03f);
                pos.v[1] = gSmoke_column[i].pos.v[1] + (gSmoke_column[i].colour == 0) * .05f;
                pos.v[2] = gSmoke_column[i].pos.v[2] + SRandomBetween(-.03f, .03f);
                if (!(gSmoke_column[i].whiter & 0x2) || IRandomBetween(0, 3)) {
                    if (gSmoke_column[i].whiter > 0) {
                        gSmoke_column[i].whiter = 2;
                    } else {
                        gSmoke_column[i].whiter = -2;
                    }
                } else {
                    gSmoke_column[i].whiter &= 0x1;
                }
                decay_factor = ((float)(gSmoke_column[i].whiter > 0) + 1.f) / 2.f;
                if (gSmoke_column[i].lifetime < 4000) {
                    decay_factor = gSmoke_column[i].lifetime * decay_factor / 4000.0f;
                }
                if (gSmoke_column[i].core.collision_info == NULL) {
                    BrVector3SetFloat(&v, 0.f, 2.8985507f, 0.f);
                } else {
                    BrVector3Cross(&v, &object->omega, &car_pos);
                    BrMatrix34ApplyV(&car_pos, &v, &object->actor->t.t.mat);
                    v.v[0] = car_pos.v[0] + WORLD_SCALE * object->v.v[0];
                    v.v[1] = car_pos.v[1] + WORLD_SCALE * object->v.v[1];
                    v.v[2] = car_pos.v[2] + WORLD_SCALE * object->v.v[2];
                }
                CreatePuffOfSmoke(&pos, &v, decay_factor, decay_factor, gSmoke_column[i].colour + 16);
            }
        } else {
            if (gSmoke_column[i].core.field_0x0 != 0) {
                ARStartPipingSession(ePipe_chunk_smoke_column);
                AddSmokeColumnToPipingSession(i, &gSmoke_column[i], gSmoke_column[i].vertex_index, gSmoke_column[i].colour);
                AREndPipingSession();
            }
            gColumn_flags &= ~(1 << i);
            if (gSmoke_column[i].colour == 0) {
                BrActorRemove(gSmoke_column[i].flame_actor);
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x004fcbb0
void C2_HOOK_FASTCALL GeneralCreateSplash(tPhysics_object* pObject, const br_vector3* pVelocity, const br_vector3* pOmega, const br_vector3* pNormal, br_scalar pWater_distance, const br_bounds3* pBounds, const br_matrix34* pMatrix, tU32 pTime) {
    br_vector3 local_normal;
    br_vector3 world_min;
    br_vector3 local_velocity;
    /* Original spills this projection, but passes local_velocity below. */
    volatile br_vector3 normal_velocity;
    br_vector3 point;
    br_vector3 minimum_point;
    br_vector3 maximum_point;
    br_vector3 span;
    br_vector3 side;
    br_vector3 offset;
    br_bounds3 bounds;
    tCar_spec* car;
    br_scalar plane_distance;
    br_scalar minimum_projection;
    br_scalar maximum_projection;
    br_scalar projection;
    br_scalar length;
    br_scalar near_distance;
    br_scalar far_distance;
    br_scalar height;
    double side_wide[3];
    double wide_height;
    double wide_near;
    double coordinate;
    double wide_projection;
    double lower;
    double upper;
    double dot;
    double value;
    double random;
    double second_random;
    double multiplier;
    int start;
    int axis;
    int a;
    int b;
    int corner;
    int pattern;
    int i;

    value = ((double)pVelocity->v[1] * pVelocity->v[1] + (double)pVelocity->v[2] * pVelocity->v[2])
        + (double)pVelocity->v[0] * pVelocity->v[0];
    if (!(value * 47.61000000000001 >= 1.0)) {
        return;
    }
    BrMatrix34TApplyV(&local_normal, pNormal, pMatrix);
    BrMatrix34ApplyP(&world_min, &pBounds->min, pMatrix);
    lower = ((double)pNormal->v[1] * world_min.v[1] + (double)pNormal->v[2] * world_min.v[2])
        + (double)pNormal->v[0] * world_min.v[0] - pWater_distance;
    upper = lower;
    for (i = 0; i < 3; i++) {
        value = ((double)pBounds->max.v[i] - pBounds->min.v[i]) * local_normal.v[i];
        if (local_normal.v[i] > 0.f) {
            upper += value;
        } else {
            lower += value;
        }
    }
    if (lower * upper > 0.0) {
        return;
    }
    dot = ((double)pVelocity->v[1] * pNormal->v[1] + (double)pVelocity->v[2] * pNormal->v[2])
        + (double)pVelocity->v[0] * pNormal->v[0];
    for (i = 0; i < 3; i++) {
        normal_velocity.v[i] = SmokeScalarRound(dot * local_normal.v[i]);
    }
    (void)&normal_velocity;
    memcpy(&bounds, pBounds, sizeof(bounds));
    bounds.min.v[1] = .01f;
    BrMatrix34TApplyV(&local_velocity, pVelocity, pMatrix);
    plane_distance = SmokeScalarRound((double)pWater_distance
        - (((double)pMatrix->m[3][2] * pNormal->v[2] + (double)pMatrix->m[3][1] * pNormal->v[1])
            + (double)pMatrix->m[3][0] * pNormal->v[0]));
    minimum_projection = 0.f;
    maximum_projection = 0.f;
    start = IRandomBetween(0, 3);
    for (axis = 0; axis < 3; axis++) {
        a = (axis + 1) % 3;
        b = (axis + 2) % 3;
        for (corner = 0; corner < 4; corner++) {
            pattern = start ^ corner;
            point.v[a] = SmokeScalarRound((pattern & 1) ? bounds.min.v[a] : bounds.max.v[a]);
            point.v[b] = SmokeScalarRound((pattern & 2) ? bounds.min.v[b] : bounds.max.v[b]);
            coordinate = (((double)plane_distance - (double)point.v[a] * local_normal.v[a])
                - (double)point.v[b] * local_normal.v[b]) / local_normal.v[axis];
            value = SmokeScalarRound(coordinate);
            if (!(coordinate >= bounds.min.v[axis]) || value > bounds.max.v[axis]) {
                continue;
            }
            point.v[axis] = SmokeScalarRound(value);
            wide_projection = ((double)local_velocity.v[0] * point.v[0] + (double)local_velocity.v[1] * point.v[1])
                + (double)local_velocity.v[2] * point.v[2];
            projection = SmokeScalarRound(wide_projection);
            if (!(wide_projection >= minimum_projection)) {
                if (!(maximum_projection >= 0.f)) {
                    SingleSplash(pObject, pMatrix, pVelocity, &local_velocity, pOmega, &maximum_point, &local_normal, pTime);
                }
                maximum_projection = minimum_projection;
                minimum_projection = projection;
                memcpy(&maximum_point, &minimum_point, sizeof(maximum_point));
                memcpy(&minimum_point, &point, sizeof(minimum_point));
            } else if (!(projection >= maximum_projection)) {
                if (!(maximum_projection >= 0.f)) {
                    SingleSplash(pObject, pMatrix, pVelocity, &local_velocity, pOmega, &maximum_point, &local_normal, pTime);
                }
                maximum_projection = projection;
                memcpy(&maximum_point, &point, sizeof(maximum_point));
            } else {
                SingleSplash(pObject, pMatrix, pVelocity, &local_velocity, pOmega, &point, &local_normal, pTime);
            }
        }
    }
    if (!(maximum_projection >= 0.f)) {
        for (i = 0; i < 3; i++) {
            span.v[i] = SmokeScalarRound((double)maximum_point.v[i] - minimum_point.v[i]);
        }
        random = SRandomBetween(.4f, .6f);
        point.v[0] = SmokeScalarRound((double)minimum_point.v[0] + random * span.v[0]);
        point.v[1] = SmokeScalarRound((double)minimum_point.v[1] + SmokeScalarRound(random * span.v[1]));
        point.v[2] = SmokeScalarRound((double)minimum_point.v[2] + SmokeScalarRound(random * span.v[2]));
        random = SRandomBetween(.2f, .3f);
        for (i = 0; i < 3; i++) {
            minimum_point.v[i] = SmokeScalarRound(random * span.v[i] + minimum_point.v[i]);
        }
        second_random = SRandomBetween(.2f, .3f);
        random = SmokeScalarRound(-second_random);
        dot = -(((double)local_velocity.v[2] * local_normal.v[2] + (double)local_velocity.v[1] * local_normal.v[1])
            + (double)local_velocity.v[0] * local_normal.v[0]);
        for (i = 0; i < 3; i++) {
            side_wide[i] = (double)local_velocity.v[i] + SmokeScalarRound((double)local_normal.v[i] * dot);
            side.v[i] = SmokeScalarRound(side_wide[i]);
        }
        length = SmokeScalarRound(sqrt((side_wide[0] * side.v[0] + side_wide[1] * side.v[1])
            + (double)side.v[2] * side.v[2]));
        if (!(length > 2.384185791015625e-7f)) {
            side.v[0] = 1.f;
            side.v[1] = 0.f;
            side.v[2] = 0.f;
            side_wide[0] = 1.0;
            side_wide[1] = 0.0;
            side_wide[2] = 0.0;
        } else {
            multiplier = 1.0 / length;
            for (i = 0; i < 3; i++) {
                side_wide[i] = multiplier * side.v[i];
                side.v[i] = SmokeScalarRound(side_wide[i]);
                if (i == 2) {
                    side_wide[i] = side.v[i];
                }
            }
        }
        for (i = 0; i < 3; i++) {
            value = side_wide[i] * -.028985507786273956f;
            offset.v[i] = SmokeScalarRound(value * .5f);
            point.v[i] = SmokeScalarRound((double)point.v[i] + SmokeScalarRound(value));
            minimum_point.v[i] = SmokeScalarRound((double)minimum_point.v[i] + offset.v[i]);
            maximum_point.v[i] = SmokeScalarRound((double)maximum_point.v[i]
                + (i == 0 ? -second_random : random) * span.v[i]);
            maximum_point.v[i] = SmokeScalarRound((double)maximum_point.v[i] + offset.v[i]);
        }
        SingleSplash(pObject, pMatrix, pVelocity, &local_velocity, pOmega, &point, &local_normal, pTime);
        SingleSplash(pObject, pMatrix, pVelocity, &local_velocity, pOmega, &minimum_point, &local_normal, pTime);
        SingleSplash(pObject, pMatrix, pVelocity, &local_velocity, pOmega, &maximum_point, &local_normal, pTime);
    } else if (!(minimum_projection >= 0.f)) {
        SingleSplash(pObject, pMatrix, pVelocity, &local_velocity, pOmega, &minimum_point, &local_normal, pTime);
    }
    plane_distance = SmokeScalarRound((double)plane_distance * 6.900000095367432f);
    if (pObject != NULL && pObject->flags_0x238 == 1 && pObject->owner != NULL) {
        car = pObject->owner;
        if (car->driver > 5) {
            for (i = 0; i < 4; i++) {
                value = (i & 1) ? pObject->shape->common.bb.max.v[0] : pObject->shape->common.bb.min.v[0];
                wide_height = (double)pObject->shape->common.bb.max.v[1] * 6.9;
                height = SmokeScalarRound(wide_height);
                wide_near = (double)plane_distance
                    - (((double)car->wpos[i].v[2] * local_normal.v[2] + wide_height * local_normal.v[1])
                        + value * 6.9 * local_normal.v[0]);
                near_distance = SmokeScalarRound(wide_near);
                far_distance = SmokeScalarRound(((double)pObject->shape->common.bb.max.v[1] - .01f)
                    * local_normal.v[1] * 6.900000095367432f + near_distance);
                if ((wide_near > 0.f) != (far_distance > 0.f)) {
                    point.v[0] = SmokeScalarRound(value * 6.9 * .14492753148078918f);
                    point.v[1] = SmokeScalarRound(((double)height - (wide_height - .01)
                        * ((double)near_distance / ((double)near_distance - far_distance))) * .14492753148078918f);
                    point.v[2] = SmokeScalarRound((double)car->wpos[i].v[2] * .14492753148078918f);
                    SingleSplash(pObject, pMatrix, pVelocity, &local_velocity, pOmega, &point, &local_normal, pTime);
                }
            }
        }
    }
}

#pragma inline_depth(0)
// FUNCTION: CARMA2_HW 0x004fda40
void C2_HOOK_FASTCALL CreateSplash(tPhysics_object* pObject, tU32 pTime) {

    GeneralCreateSplash(pObject, &pObject->v, &pObject->omega, &pObject->water_normal, pObject->water_d,
        &pObject->bb1, &pObject->actor->t.t.mat, pTime);
}
#pragma inline_depth()

// FUNCTION: CARMA2_HW 0x004fda70
void C2_HOOK_FASTCALL GetVelocitiesFromMatrices(br_vector3* pVel, br_vector3* pRot, br_matrix34* pMat_now, br_matrix34* pMat_prev, tU32 pTime) {
    br_quat quat_now;
    br_quat quat_prev;
    br_quat quat_speed;
    br_scalar ts;

    ts = (float)pTime / 1000.f;
    BrVector3Sub(pVel, (br_vector3*)pMat_prev->m[3], (br_vector3*)pMat_now->m[3]);
    BrVector3InvScale(pVel, pVel, ts);
    BrMatrix34ToQuat(&quat_now, pMat_now);
    BrMatrix34ToQuat(&quat_prev, pMat_now);
    BrQuatInvert(&quat_now, &quat_now);
    BrQuatMul(&quat_speed, &quat_now, &quat_prev);
    pRot->v[0] = 2 * quat_speed.x / ts;
    pRot->v[1] = 2 * quat_speed.y / ts;
    pRot->v[2] = 2 * quat_speed.z / ts;
}

// FUNCTION: CARMA2_HW 0x004f9e80
void C2_HOOK_FASTCALL DrMatrix34Rotate(br_matrix34* mat, br_angle r, br_vector3* a) {
    br_scalar t;
    br_scalar s;
    br_scalar c;
    br_scalar txy;
    br_scalar txz;
    br_scalar tyz;
    br_scalar sx;
    br_scalar sy;
    br_scalar sz;

    s = FastScalarSinAngle(r);
    c = FastScalarCosAngle(r);
    t = 1.0f - c;
    txy = t * a->v[0] * a->v[1];
    txz = t * a->v[0] * a->v[2];
    tyz = t * a->v[1] * a->v[2];
    sx = a->v[0] * s;
    sy = a->v[1] * s;
    sz = a->v[2] * s;
    mat->m[0][0] = a->v[0] * a->v[0] * t + c;
    mat->m[0][1] = sz + txy;
    mat->m[0][2] = txz - sy;
    mat->m[1][0] = txy - sz;
    mat->m[1][1] = a->v[1] * a->v[1] * t + c;
    mat->m[1][2] = sx + tyz;
    mat->m[2][0] = sy + txz;
    mat->m[2][1] = tyz - sx;
    mat->m[2][2] = a->v[2] * a->v[2] * t + c;
}

// FUNCTION: CARMA2_HW 0x004fc0e0
void C2_HOOK_FASTCALL DoSmokeColumn(int i, tU32 pTime, br_vector3* pRet_car_pos) {
    tSmoke_column* column;
    static const br_scalar upright_tilt_threshold = 0.1f;
    static const br_scalar descent_tilt_threshold = -0.1f;

    column = &gSmoke_column[i];
    if (column->core.field_0x0 == 1) {
        if (column->core.car->car_master_actor->t.t.mat.m[1][1] > upright_tilt_threshold) {
            column->upright = 1;
        }
        if (column->core.car->car_master_actor->t.t.mat.m[1][1] < descent_tilt_threshold) {
            column->upright = 0;
        }
        pRet_car_pos->v[0] = column->core.car->car_actor->model->prepared->groups->vertices[column->vertex_index].p.v[0]
            + column->core.car->car_actor->t.t.mat.m[3][0];
        pRet_car_pos->v[1] = column->core.car->car_actor->model->prepared->groups->vertices[column->vertex_index].p.v[1]
            + column->core.car->car_actor->t.t.mat.m[3][1];
        pRet_car_pos->v[2] = column->core.car->car_actor->model->prepared->groups->vertices[column->vertex_index].p.v[2]
            + column->core.car->car_actor->t.t.mat.m[3][2];
    } else {
        column->upright = 1;
        pRet_car_pos->v[0] = column->core.car_actor->model->prepared->groups->vertices[column->vertex_index].p.v[0];
        pRet_car_pos->v[1] = column->core.car_actor->model->prepared->groups->vertices[column->vertex_index].p.v[1];
        pRet_car_pos->v[2] = column->core.car_actor->model->prepared->groups->vertices[column->vertex_index].p.v[2];
    }
    if (column->core.field_0x0 == 1 && gProgram_state.cockpit_on && column->core.car != NULL
            && column->core.car->driver == eDriver_local_human
            && *(float*)((char*)column->core.car + 0x484) > pRet_car_pos->v[2]) {
        BrMatrix34ApplyP(pRet_car_pos, &column->core.car_actor->model->prepared->groups->vertices[column->vertex_index].p,
            &column->core.car_actor->t.t.mat);
    } else {
        pRet_car_pos->v[1] -= 0.0f;
    }
    if (column->upright == 0) {
        pRet_car_pos->v[1] = column->core.car->collision_info->shape->common.bb.min.v[1] * 0.14492754f;
    }
    BrMatrix34ApplyP(&column->pos, pRet_car_pos, &column->core.car_actor->t.t.mat);
    column->pos.v[1] -= 0.03f;
}

// GLOBAL: CARMA2_HW 0x00660118
static tU8 gFlame_frame_size[20][2] = {
    { 20, 25 }, { 24, 32 }, { 24, 34 }, { 28, 44 }, { 28, 49 },
    { 32, 53 }, { 28, 61 }, { 28, 69 }, { 24, 61 }, { 28, 64 },
    { 24, 70 }, { 20, 65 }, { 20, 64 }, { 20, 74 }, { 28, 55 },
    { 24, 62 }, { 28, 47 }, { 20, 51 }, { 20, 45 }, { 20, 45 },
};

// FUNCTION: CARMA2_HW 0x004fbdd0
void C2_HOOK_FASTCALL FlameAnimate(int pColumn, const br_vector3* pPos, tU32 pTime) {
    tSmoke_column* column;
    br_actor* actor;
    br_actor* flame;
    br_pixelmap* map;
    tPhysics_object* physics;
    tU32 bits;
    int i;
    int width;
    int height;
    int direction;
    br_scalar scale;
    br_scalar sx;
    br_scalar sy;
    double speed_squared;
    int can_start;
    typedef void (C2_HOOK_FASTCALL* raw_flame_fn)(int, int, tU32, tU32, tU32, tU32);
    volatile uintptr_t record_address;

    column = &gSmoke_column[pColumn];
    actor = column->flame_actor;
    DRMatrix34RotateY(&actor->t.t.mat, FastScalarArcTan2Angle(gCamera_to_world.m[2][0], gCamera_to_world.m[2][2]));
    memcpy(&actor->t.t.mat.m[3][0], &pPos->v[0], sizeof(br_scalar));
    memcpy(&actor->t.t.mat.m[3][1], &pPos->v[1], sizeof(br_scalar));
    bits = SmokeScalarBits(&pPos->v[2]);
    flame = actor->children;
    memcpy(&actor->t.t.mat.m[3][2], &bits, sizeof(bits));
    if (gAction_replay_mode) {
        for (i = 0; i < 3; i++) {
            column->frame_count[i] = (tS32)(tU32)(br_int_64)((double)ARGetReplayRate() + column->frame_count[i]);
            if (column->frame_count[i] >= 0 && column->frame_count[i] < 20) {
                flame->type = 1;
                flame->material->colour_map = gFlame_map[column->frame_count[i]];
                BrMaterialUpdate(flame->material, 0x7fff);
                map = gFlame_map[column->frame_count[i]];
                height = (tU16)map->height;
                width = (tU16)map->width;
                sy = SmokeScalarRound((double)height * column->scale_y[i]);
                sx = SmokeScalarRound((double)width * column->scale_x[i]);
                BrMatrix34Scale(&flame->t.t.mat, sx, sy, 1.0f);
                memcpy(&flame->t.t.mat.m[3][0], &column->offset_x[i], sizeof(br_scalar));
                memcpy(&flame->t.t.mat.m[3][2], &column->offset_z[i], sizeof(br_scalar));
            } else {
                flame->type = 0;
            }
            flame = flame->next;
        }
        return;
    }
    record_address = (uintptr_t)AddFlameToPipingSession;
    for (i = 0; i < 3; i++) {
        column->frame_count[i] = (tS32)((tU32)column->frame_count[i] + 1u);
        if (column->frame_count[i] >= 20) {
            ARStartPipingSession((tPipe_chunk_type)18);
            ((raw_flame_fn)record_address)((pColumn << 4) + i, (tS32)((tU32)column->frame_count[i] + 1u), SmokeScalarBits(&column->scale_x[i]), SmokeScalarBits(&column->scale_y[i]), SmokeScalarBits(&column->offset_x[i]), SmokeScalarBits(&column->offset_z[i]));
            AREndPipingSession();
            column->frame_count[i] = IRandomBetween(-5, -1);
            scale = SmokeScalarRound(SRandomBetween(1.0f, 1.5f) * .003000000026077032f);
            direction = IRandomBetween(0, 1) * 2 - 1;
            column->scale_x[i] = SmokeScalarRound((double)direction * scale);
            column->scale_y[i] = SmokeScalarRound(SRandomBetween(.5f, 1.0f) * .003000000026077032f);
            column->offset_x[i] = SmokeScalarRound(SRandomPosNeg(.03f));
            column->offset_z[i] = SmokeScalarRound(SRandomBetween(-.03f, 0.0f));
            flame->type = 0;
        }
        if (column->frame_count[i] == 0) {
            can_start = 1;
            if (column->core.car != NULL) {
                physics = column->core.car->collision_info;
                speed_squared = (double)physics->v.v[0] * physics->v.v[0] + (double)physics->v.v[1] * physics->v.v[1];
                speed_squared += (double)physics->v.v[2] * physics->v.v[2];
                can_start = !(speed_squared >= 11.594202898550725);
            }
            if (can_start && pTime * 30u < column->lifetime) {
                flame->type = 1;
                ARStartPipingSession((tPipe_chunk_type)18);
                ((raw_flame_fn)record_address)((pColumn << 4) + i, (tS32)((tU32)column->frame_count[i] - 1u), SmokeScalarBits(&column->scale_x[i]), SmokeScalarBits(&column->scale_y[i]), SmokeScalarBits(&column->offset_x[i]), SmokeScalarBits(&column->offset_z[i]));
                AREndPipingSession();
            } else {
                column->frame_count[i] = -5;
            }
        }
        if (column->frame_count[i] >= 0) {
            flame->material->colour_map = gFlame_map[column->frame_count[i]];
            BrMaterialUpdate(flame->material, 0x7fff);
            height = gFlame_frame_size[column->frame_count[i]][1];
            width = gFlame_frame_size[column->frame_count[i]][0];
            sy = SmokeScalarRound((double)height * column->scale_y[i]);
            sx = SmokeScalarRound((double)width * column->scale_x[i]);
            BrMatrix34Scale(&flame->t.t.mat, sx, sy, 1.0f);
            memcpy(&flame->t.t.mat.m[3][0], &column->offset_x[i], sizeof(br_scalar));
            memcpy(&flame->t.t.mat.m[3][2], &column->offset_z[i], sizeof(br_scalar));
        }
        flame = flame->next;
    }
}

// FUNCTION: CARMA2_HW 0x0042df70
int C2_HOOK_FASTCALL IsOnNonCarHalf(tCar_spec* pCar, br_vector3 *pPos) {
    tCar_crush_spec* car_crush;

    car_crush = pCar->car_crush_spec;
    return car_crush != NULL
        && car_crush->field_0x4b8 != 0
        && BrVector3Dot(&car_crush->field_0x548, pPos) - car_crush->field_0x554 < 0.f;
}

// FUNCTION: CARMA2_HW 0x004fc9c0
void C2_HOOK_FASTCALL SmudgeCar(tCar_spec* pCar, int fire_point) {

    /* empty */
}

// FUNCTION: CARMA2_HW 0x004f7cb0
void C2_HOOK_FASTCALL SetLineModelCols(tU8 pCol) {

    if (pCol != 0) {
        gLine_model->vertices[0].red = 0xff;
        gLine_model->vertices[0].grn = 0xff;
        gLine_model->vertices[0].blu = 0xff;
        gLine_model->vertices[1].red = 0xff;
        gLine_model->vertices[1].grn = 0xff;
        gLine_model->vertices[1].blu = 0xff;
    } else {
        gLine_model->vertices[0].red = 0xff;
        gLine_model->vertices[0].grn = 0x00;
        gLine_model->vertices[0].blu = 0x00;
        gLine_model->vertices[1].red = 0xff;
        gLine_model->vertices[1].grn = 0xff;
        gLine_model->vertices[1].blu = 0x00;
    }
    BrModelUpdate(gLine_model, BR_MODU_ALL);
}

// FUNCTION: CARMA2_HW 0x004f6de0
int C2_HOOK_FASTCALL DrawLine3DThroughBRender(br_vector3* start, br_vector3* end, br_pixelmap* pScreen, br_pixelmap* pDepth_buffer, float scale, br_pixelmap* shade_table);

// FUNCTION: CARMA2_HW 0x004f6b80
int C2_HOOK_FASTCALL DrawLine3D(br_vector3* start, br_vector3* end, br_pixelmap* pScreen, br_pixelmap* pDepth_buffer, br_pixelmap* shade_table) {
    br_vector3 lStart, lEnd;
    br_vector4 projStart, projEnd;

    if (gNo_2d_effects) {
        gLine_model->vertices[0].p.v[0] = start->v[0];
        gLine_model->vertices[0].p.v[1] = start->v[1];
        gLine_model->vertices[0].p.v[2] = start->v[2];
        gLine_model->vertices[1].p.v[0] = end->v[0];
        gLine_model->vertices[1].p.v[1] = end->v[1];
        gLine_model->vertices[1].p.v[2] = end->v[2];
        BrModelUpdate(gLine_model, BR_MODU_VERTEX_POSITIONS);
        BrZbsSceneRenderAdd(gLine_actor);
        return 0x3e7;
    }

    lStart.v[0] = start->v[0];
    lStart.v[1] = start->v[1];
    lStart.v[2] = start->v[2];
    lEnd.v[0] = end->v[0];
    lEnd.v[1] = end->v[1];
    lEnd.v[2] = end->v[2];

    {
        float clip = -gSpark_cam->hither_z;

        if (lStart.v[2] > clip) goto common_1;
        if (lEnd.v[2] <= clip) goto project;
    common_1:
        if (lStart.v[2] <= clip || lEnd.v[2] <= clip) goto clip_line;
        return 0;

    clip_line:
        {
            float ratio = (lEnd.v[2] + gSpark_cam->hither_z) / (lEnd.v[2] - lStart.v[2]);
            if (lStart.v[2] > clip) {
                lStart.v[0] = lEnd.v[0] - (lEnd.v[0] - lStart.v[0]) * ratio;
                lStart.v[1] = lEnd.v[1] - (lEnd.v[1] - lStart.v[1]) * ratio;
                lStart.v[2] = -gSpark_cam->hither_z;
            }
            if (lEnd.v[2] > -gSpark_cam->hither_z) {
                lEnd.v[0] = lEnd.v[0] - (lEnd.v[0] - lStart.v[0]) * ratio;
                lEnd.v[1] = lEnd.v[1] - (lEnd.v[1] - lStart.v[1]) * ratio;
                lEnd.v[2] = -gSpark_cam->hither_z;
            }
        }
    }
project:

    BrMatrix4ApplyP(&projStart, &lStart, &gCameraToScreen);
    BrMatrix4ApplyP(&projEnd, &lEnd, &gCameraToScreen);

    lStart.v[0] = projStart.v[0] / projStart.v[3];
    lStart.v[1] = projStart.v[1] / projStart.v[3];
    lStart.v[2] = projStart.v[2] / projStart.v[3];
    lEnd.v[0] = projEnd.v[0] / projEnd.v[3];
    lEnd.v[1] = projEnd.v[1] / projEnd.v[3];
    lEnd.v[2] = projEnd.v[2] / projEnd.v[3];

    return DrawLine3DThroughBRender(
        &lStart, &lEnd,
        pScreen, pDepth_buffer, 1.0f, shade_table);
}

// FUNCTION: CARMA2_HW 0x004f6de0
int C2_HOOK_FASTCALL DrawLine3DThroughBRender(br_vector3* start, br_vector3* end, br_pixelmap* pScreen, br_pixelmap* pDepth_buffer, float scale, br_pixelmap* shade_table) {
    int xe;
    int ye;
    br_uint_8* screen_ptr;
    char* shade_tbl;
    int two_dy;
    int bres_error;
    int two_dx;
    int xs;
    int ys;
    int x_direction;
    int y_direction;
    br_scalar z, z_inc;
    int shade_inc;
    int shade_scale;
    br_uint_16* z_ptr;
    int shade_count;
    br_uint_16* zp;

    screen_ptr = (br_uint_8*)pScreen->pixels + pScreen->base_y * pScreen->row_bytes + pScreen->base_x;

    xs = (int)start->v[0] + pScreen->width / 2;
    xe = (int)end->v[0] + pScreen->width / 2;

    ys = pScreen->height / 2 - (int)start->v[1];
    ye = pScreen->height / 2 - (int)end->v[1];

    if (scale < 0.001f || scale > 1.0f) return 0;

    if (xs < 0 || xe < 0) {
        if (xs < 0) {
            int t;
            if (xe < 0) return 0;
            t = xe - xs;
            start->v[2] = end->v[2] - (end->v[2] - start->v[2]) * xe / t;
            ys = ye - (ye - ys) * xe / t;
            xs = 0;
        } else {
            int t = xs - xe;
            end->v[2] = start->v[2] - (start->v[2] - end->v[2]) * xs / t;
            ye = ys - (ys - ye) * xs / t;
            xe = 0;
        }
    }
    if (xs >= pScreen->width || xe >= pScreen->width) {
        if (xs >= pScreen->width) {
            int t;
            int n;
            if (xe >= pScreen->width) return 0;
            t = xe - xs;
            n = xe - pScreen->width + 1;
            start->v[2] = end->v[2] - (end->v[2] - start->v[2]) * n / t;
            ys = ye - (ye - ys) * n / t;
            xs = pScreen->width - 1;
        } else {
            int t = xs - xe;
            int n = xs - pScreen->width + 1;
            end->v[2] = start->v[2] - (start->v[2] - end->v[2]) * n / t;
            ye = ys - (ys - ye) * n / t;
            xe = pScreen->width - 1;
        }
    }

    if (ys < 0 || ye < 0) {
        if (ys < 0) {
            int t;
            if (ye < 0) return 0;
            t = ye - ys;
            start->v[2] = end->v[2] - (end->v[2] - start->v[2]) * ye / t;
            xs = xe - (xe - xs) * ye / t;
            ys = 0;
        } else {
            int t = ys - ye;
            end->v[2] = start->v[2] - (start->v[2] - end->v[2]) * ys / t;
            xe = xs - (xs - xe) * ys / t;
            ye = 0;
        }
    }
    if (ys >= pScreen->height || ye >= pScreen->height) {
        if (ys >= pScreen->height) {
            int t;
            int n;
            if (ye >= pScreen->height) return 0;
            t = ye - ys;
            n = ye - pScreen->height + 1;
            start->v[2] = end->v[2] - (end->v[2] - start->v[2]) * n / t;
            xs = xe - (xe - xs) * n / t;
            ys = pScreen->height - 1;
        } else {
            int t = ys - ye;
            int n = ys - pScreen->height + 1;
            end->v[2] = start->v[2] - (start->v[2] - end->v[2]) * n / t;
            xe = xs - (xs - xe) * n / t;
            ye = pScreen->height - 1;
        }
    }

    z = start->v[2];
    zp = (br_uint_16*)pDepth_buffer->pixels;
    shade_tbl = (char*)shade_table->pixels + shade_table->base_y * shade_table->row_bytes;
    two_dx = 2 * abs(xe - xs);
    two_dy = 2 * abs(ye - ys);
    x_direction = xe - xs < 0 ? -1 : 1;
    y_direction = ye - ys < 0 ? -1 : 1;

    screen_ptr += ys * pScreen->row_bytes + xs;
    z_ptr = zp + (pDepth_buffer->row_bytes / 2) * ys + xs;
    shade_scale = (int)((scale - 0.001f) * shade_table->height);

    if (two_dx > two_dy) {
        shade_inc = 500 * two_dx / shade_scale;
        z_inc = 2.0f * (end->v[2] - start->v[2]) / two_dx;
        bres_error = two_dy - two_dx / 2;
        shade_count = shade_inc;

        for (;;) {
            if ((1.0f - z) * 32768.0f < (float)*z_ptr) {
                *z_ptr = (br_uint_16)((1.0f - z) * 32768.0f);
                *screen_ptr = shade_tbl[*screen_ptr];
            }
            if (xs == xe) break;
            if (bres_error >= 0) {
                screen_ptr += y_direction * pScreen->row_bytes;
                z_ptr += (pDepth_buffer->row_bytes / 2) * y_direction;
                bres_error -= two_dx;
            }
            xs += x_direction;
            screen_ptr += x_direction;
            bres_error += two_dy;
            z_ptr += x_direction;
            z += z_inc;
            shade_count -= 1000;
            if (shade_count <= 0) {
                do {
                    shade_tbl += shade_table->row_bytes;
                    shade_count += shade_inc;
                } while (shade_count <= 0);
            }
        }
    } else {
        shade_inc = 500 * two_dy / shade_scale;
        z_inc = 2.0f * (end->v[2] - start->v[2]) / two_dy;
        bres_error = two_dx - two_dy / 2;
        shade_count = shade_inc;

        for (;;) {
            if ((1.0f - z) * 32768.0f < (float)*z_ptr) {
                *z_ptr = (br_uint_16)((1.0f - z) * 32768.0f);
                *screen_ptr = shade_tbl[*screen_ptr];
            }
            if (ys == ye) break;
            if (bres_error >= 0) {
                screen_ptr += x_direction;
                z_ptr += x_direction;
                bres_error -= two_dy;
            }
            ys += y_direction;
            screen_ptr += y_direction * pScreen->row_bytes;
            bres_error += two_dx;
            z_ptr += (pDepth_buffer->row_bytes / 2) * y_direction;
            z += z_inc;
            shade_count -= 1000;
            if (shade_count <= 0) {
                do {
                    shade_tbl += shade_table->row_bytes;
                    shade_count += shade_inc;
                } while (shade_count <= 0);
            }
        }
    }

    return 1;
}

void C2_HOOK_FASTCALL SetWorldToScreen(br_pixelmap* pScreen) {
    br_matrix4 mat;
    br_matrix4 mat2;

    BrMatrix4Perspective(&mat, gSpark_cam->field_of_view, gSpark_cam->aspect, -gSpark_cam->hither_z, -gSpark_cam->yon_z);
    BrMatrix4Scale(&mat2, (float)(pScreen->width / 2), (float)(pScreen->height / 2), 1.0f);
    BrMatrix4Mul(&gCameraToScreen, &mat, &mat2);
}

void C2_HOOK_FASTCALL ReplaySparks(br_pixelmap* pRender_screen, br_pixelmap* pDepth_buffer, br_actor* pCamera, tU32 pTime) {
    int i;
    br_vector3 pos;
    br_vector3 o;
    br_vector3 p;
    br_vector3 tv;
    br_vector3 new_pos;

    for (i = 0; i < CARPOCALYPSE2_ASIZE(gSparks); i++) {
        if (gSpark_flags & (1 << i)) {
            if (gSparks[i].car != NULL) {
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif
                BrMatrix34ApplyP(&tv, &o, &gSparks[i].car->car_master_actor->t.t.mat);
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
                BrVector3Copy(&o, &tv);
                BrMatrix34ApplyP(&pos, &gSparks[i].pos, &gSparks[i].car->car_master_actor->t.t.mat);
            } else {
                BrVector3Copy(&pos, &gSparks[i].pos);
            }
            BrVector3Add(&o, &pos, &gSparks[i].length);
            if (gRendering_mirror) {
                BrVector3Sub(&tv, &pos, (br_vector3*)gRearview_camera_to_world.m[3]);
                BrMatrix34TApplyV(&new_pos, &tv, &gRearview_camera_to_world);
                BrVector3Sub(&tv, &o, (br_vector3*)gRearview_camera_to_world.m[3]);
                BrMatrix34TApplyV(&p, &tv, &gRearview_camera_to_world);
            } else {
                BrVector3Sub(&tv, &pos, (br_vector3*)gCamera_to_world.m[3]);
                BrMatrix34TApplyV(&new_pos, &tv, &gCamera_to_world);
                BrVector3Sub(&tv, &o, (br_vector3*)gCamera_to_world.m[3]);
                BrMatrix34TApplyV(&p, &tv, &gCamera_to_world);
            }
            if (gNo_2d_effects) {
                SetLineModelCols(gSparks[i].colour);
            }
            if (gSparks[i].colour != 0) {
                DrawLine3D(&p, &new_pos, pRender_screen, pDepth_buffer, gFog_shade_table);
            } else {
                DrawLine3D(&p, &new_pos, pRender_screen, pDepth_buffer, gAcid_shade_table);
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x004f7450
void C2_HOOK_FASTCALL RenderSparks(br_pixelmap* pRender_screen, br_pixelmap* pDepth_buffer, br_actor* pCamera, br_matrix34* pCamera_to_world, tU32 pTime) {
    int i;
    int time;
    br_vector3 tv;
    br_vector3 o;
    br_vector3 p;
    br_vector3 pos;
    br_vector3 new_pos;
    br_scalar ts;

    C2_HOOK_BUG_ON(sizeof(tSpark) != 0x40);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSpark, count, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSpark, pos, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSpark, v, 0x10);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSpark, length, 0x1c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSpark, normal, 0x28);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSpark, time_sync, 0x34);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSpark, car, 0x38);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSpark, colour, 0x3c);

    gSpark_cam = pCamera->type_data;
    SetWorldToScreen(pRender_screen);

    if (gSpark_flags == 0) {
        return;
    }

    if (gNo_2d_effects) {
        BrActorRemove(gLine_actor);
        BrActorAdd(pCamera, gLine_actor);
    }

    if (gAction_replay_mode) {
        ReplaySparks(pRender_screen, pDepth_buffer, pCamera, pTime);
        if (gNo_2d_effects) {
            BrActorRemove(gLine_actor);
            BrActorAdd(gDont_render_actor, gLine_actor);
        }
        return;
    }
    ARStartPipingSession(ePipe_chunk_spark);
    for (i = 0; i < CARPOCALYPSE2_ASIZE(gSparks); i++) {
        if (!((1u << i) & gSpark_flags)) {
            continue;
        }
        if (gSparks[i].count <= 0) {
            gSparks[i].count = 0;
            gSpark_flags &= ~(1u << i);
        }
        ts = BrVector3Dot(&gSparks[i].normal, &gSparks[i].v);
        BrVector3Scale(&tv, &gSparks[i].normal, ts);
        BrVector3Sub(&gSparks[i].v, &gSparks[i].v, &tv);
        if (gSparks[i].time_sync) {
            BrVector3Scale(&o, &gSparks[i].v, gSparks[i].time_sync / 1000.f);
            gSparks[i].count += gSparks[i].time_sync - pTime;
            gSparks[i].time_sync = 0;
        } else {
            BrVector3Scale(&o, &gSparks[i].v, pTime / 1000.f);
            gSparks[i].count -= pTime;
        }
        BrVector3Accumulate(&gSparks[i].pos, &o);
        time = 1000 - gSparks[i].count;
        if (time > 150) {
            time = 150;
        }
        ts = -time / 1000.f;
        if (gSparks[i].colour) {
            ts /= 2.0;
        }
        BrVector3Scale(&gSparks[i].length, &gSparks[i].v, ts);
        ts = pTime * 10.f / (WORLD_SCALE * 100.f);
        if (gSparks[i].car != NULL) {
            BrMatrix34ApplyV(&tv, &gSparks[i].length, &gSparks[i].car->car_master_actor->t.t.mat);
            BrVector3Copy(&gSparks[i].length, &tv);
            BrMatrix34ApplyP(&pos, &gSparks[i].pos, &gSparks[i].car->car_master_actor->t.t.mat);
            BrVector3Copy(&o, &tv);
            gSparks[i].v.v[0] -= gSparks[i].car->car_master_actor->t.t.mat.m[0][1] * ts;
            gSparks[i].v.v[1] -= gSparks[i].car->car_master_actor->t.t.mat.m[1][1] * ts;
            gSparks[i].v.v[2] -= gSparks[i].car->car_master_actor->t.t.mat.m[2][1] * ts;
        } else {
            BrVector3Copy(&pos, &gSparks[i].pos);
            gSparks[i].v.v[1] -= ts;
        }
        AddSparkToPipingSession(i + (gSparks[i].colour << 8), &pos, &gSparks[i].length);
        BrVector3Add(&o, &gSparks[i].length, &pos);
        if (gRendering_mirror) {
            BrVector3Sub(&tv, &pos, (br_vector3*)gRearview_camera_to_world.m[3]);
            BrMatrix34TApplyV(&new_pos, &tv, &gRearview_camera_to_world);
            BrVector3Sub(&tv, &o, (br_vector3*)gRearview_camera_to_world.m[3]);
            BrMatrix34TApplyV(&p, &tv, &gRearview_camera_to_world);
        } else {
            BrVector3Sub(&tv, &pos, (br_vector3*)gCamera_to_world.m[3]);
            BrMatrix34TApplyV(&new_pos, &tv, &gCamera_to_world);
            BrVector3Sub(&tv, &o, (br_vector3*)gCamera_to_world.m[3]);
            BrMatrix34TApplyV(&p, &tv, &gCamera_to_world);
        }
        BrVector3SetFloat(&tv, FRandomBetween(-0.1f, 0.1f), FRandomBetween(-0.1f, 0.1f), FRandomBetween(-0.1f, 0.1f));
        BrVector3Accumulate(&gSparks[i].v, &tv);
        ts = 1.0f - BrVector3Length(&gSparks[i].v) / 1.4f * pTime / 1000.0f;
        if (ts < 0.1f) {
            ts = 0.1f;
        }
        BrVector3Scale(&gSparks[i].v, &gSparks[i].v, ts);
        if (gNo_2d_effects) {
            SetLineModelCols(gSparks[i].colour);
        }
        if (gSparks[i].colour != 0) {
            DrawLine3D(&p, &new_pos, pRender_screen, pDepth_buffer, gFog_shade_table);
        } else {
            DrawLine3D(&p, &new_pos, pRender_screen, pDepth_buffer, gAcid_shade_table);
        }
    }
    AREndPipingSession();
    if (gNo_2d_effects) {
        BrActorRemove(gLine_actor);
        BrActorAdd(gDont_render_actor, gLine_actor);
    }
}

// FUNCTION: CARMA2_HW 0x004fb2f0
int C2_HOOK_CDECL CmpSmokeZ(const void* p1, const void* p2) {
    tBRender_smoke** a;
    tBRender_smoke** b;

    a = (tBRender_smoke**)p1;
    b = (tBRender_smoke**)p2;
    if ((*a)->pos.v[2] < (*b)->pos.v[2]) {
        return -1;
    } else if ((*a)->pos.v[2] > (*b)->pos.v[2]) {
        return 1;
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x004fb1b0
void C2_HOOK_FASTCALL RenderRecordedSmokeCircles(void) {
    int i;
    int j;
    tBRender_smoke* smoke;
    tU8 red;
    tU8 grn;
    tU8 blu;

    BrQsort(gBR_smoke_pointers, gN_BR_smoke_structs, sizeof(void*), CmpSmokeZ);

    for (i = 0; i < gN_BR_smoke_structs; i++) {
        smoke = gBR_smoke_pointers[gN_BR_smoke_structs - 1 - i];
        BrVector3Copy(&gBlend_actor->t.t.translate.t, &smoke->pos);
        gBlend_actor->t.t.mat.m[0][0] = smoke->r;
        gBlend_actor->t.t.mat.m[1][1] = smoke->r / smoke->aspect;
        gBlend_actor->material = smoke->material;
        gBlend_actor->material->extra_prim[1].v.x = BrFloatToFixed(smoke->strength * 150.f);
        BrMaterialUpdate(gBlend_actor->material, BR_MATU_EXTRA_PRIM);

        red = BR_RED(smoke->col);
        grn = BR_GRN(smoke->col);
        blu = BR_BLU(smoke->col);
        for (j = 0; j < smoke->model->nvertices; j++) {
            smoke->model->vertices[j].red = red;
            smoke->model->vertices[j].grn = grn;
            smoke->model->vertices[j].blu = blu;
        }
        BrModelUpdate(smoke->model, BR_MODU_VERTEX_COLOURS);
        gBlend_actor->model = smoke->model;
        BrZbsSceneRenderAdd(gBlend_actor);
    }
}

void C2_HOOK_FASTCALL RecordSmokeCircle(br_vector3* pCent, br_scalar pR, br_scalar pStrength, br_colour pShade, br_scalar pAspect) {

    if (gRendering_mirror) {
        DRMatrix34TApplyP(&gBR_smoke_structs[gN_BR_smoke_structs].pos, pCent, &gRearview_camera_to_world);
    } else {
        DRMatrix34TApplyP(&gBR_smoke_structs[gN_BR_smoke_structs].pos, pCent, &gCamera_to_world);
    }

    gBR_smoke_structs[gN_BR_smoke_structs].r = pR;
    gBR_smoke_structs[gN_BR_smoke_structs].strength = pStrength;
    gBR_smoke_structs[gN_BR_smoke_structs].col = pShade;
    gBR_smoke_structs[gN_BR_smoke_structs].aspect = pAspect;
    gBR_smoke_pointers[gN_BR_smoke_structs] = &gBR_smoke_structs[gN_BR_smoke_structs];
    if (pShade == gShade_list[7]) {
        gBR_smoke_structs[gN_BR_smoke_structs].model = gBlend_model2;
    } else {
        gBR_smoke_structs[gN_BR_smoke_structs].model = gBlend_model;
    }
    gN_BR_smoke_structs += 1;
}

void C2_HOOK_FASTCALL SmokeCircle3D(br_vector3* o, br_scalar r, br_scalar strength, br_scalar pAspect, br_pixelmap* pRender_screen, br_pixelmap* pDepth_buffer, br_colour pShade, br_actor* pCamera) {

    if (gNo_2d_effects) {
        RecordSmokeCircle(o, r, strength, pShade, pAspect);
    }
}

void C2_HOOK_FASTCALL DrawTheGlow(br_pixelmap* pRender_screen, br_pixelmap* pDepth_buffer, br_actor* pCamera) {
    int i;
    br_scalar strength;
    br_vector3 tv;
    tU32 seed;

    if (gColumn_flags != 0) {
        seed = rand();
        srand(GetTotalTime());
for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gSmoke_column); i++) {
            if (((1u << i) & gColumn_flags) != 0 && gSmoke_column[i].colour <= 1) {
                strength = 0.5f;
                if (gSmoke_column[i].lifetime < 4000) {
                    strength = gSmoke_column[i].lifetime * 0.5f / 4000.f;
                }
                BrVector3Set(&tv, gSmoke_column[i].pos.v[0], gSmoke_column[i].pos.v[1] + 0.02f, gSmoke_column[i].pos.v[2]);
                SmokeCircle3D(&tv, 0.07f, strength, SRandomBetween(0.5f, 0.99f), pRender_screen, pDepth_buffer, gShade_list[7], pCamera);
            }
        }
        srand(seed);
    }
}

void C2_HOOK_FASTCALL ReplaySmoke(br_pixelmap* pRender_screen, br_pixelmap* pDepth_buffer, br_actor* pCamera) {
    br_scalar aspect;
    int i;

    for (i = 0; i < CARPOCALYPSE2_ASIZE(gSmoke); i++) {
        if (gSmoke_flags & (1u << i)) {
            aspect = 1.f + (gSmoke[i].radius - .05f) * 4.f / 2.f;
            if (gSmoke[i].type & 0x10) {
                SmokeCircle3D(&gSmoke[i].pos, gSmoke[i].radius / aspect, gSmoke[i].strength, 1.f,
                    pRender_screen, pDepth_buffer, gShade_list[gSmoke[i].type & 0xf], pCamera);
            } else {
                SmokeCircle3D(&gSmoke[i].pos, gSmoke[i].radius, gSmoke[i].strength, aspect,
                    pRender_screen, pDepth_buffer, gShade_list[gSmoke[i].type & 0xf], pCamera);
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x004fa910
void C2_HOOK_FASTCALL RenderSmoke(br_pixelmap* pRender_screen, br_pixelmap* pDepth_buffer, br_actor* pCamera, br_matrix34* pCamera_to_world, tU32 pTime) {
    int i;
    int j;
    br_vector3 tv;
    br_scalar aspect;
    br_scalar ts;
    tU32 seed;
    tU32 not_lonely;

    not_lonely = 0;
    if (gNo_2d_effects) {
        gBlend_actor->render_style = BR_RSTYLE_FACES;
        if (gBlend_actor->parent != NULL) {
            BrActorRemove(gBlend_actor);
        }
        BrActorAdd(pCamera, gBlend_actor);
        gN_BR_smoke_structs = 0;
    }
    DrawTheGlow(pRender_screen, pDepth_buffer, pCamera);

    if (gSmoke_flags == 0) {
        if (gNo_2d_effects) {
            if (gBlend_actor->parent != NULL) {
                BrActorRemove(gBlend_actor);
            }
            BrActorAdd(gDont_render_actor, gBlend_actor);
        }
        return;
    }

    seed = rand();
    if (gAction_replay_mode) {
        ReplaySmoke(pRender_screen, pDepth_buffer, pCamera);
        srand(seed);
        if (gNo_2d_effects) {
            RenderRecordedSmokeCircles();
            if (gBlend_actor->parent != NULL) {
                BrActorRemove(gBlend_actor);
            }
            BrActorAdd(gDont_render_actor, gBlend_actor);
        }
        return;
    }
    ARStartPipingSession(ePipe_chunk_smoke);
    for (i = 0; i < CARPOCALYPSE2_ASIZE(gSmoke); i++) {
        if (gSmoke_flags & (1u << i)) {
            if (gSmoke[i].strength > 0.0) {
                if (gSmoke[i].time_sync != 0) {
                    BrVector3Scale(&tv, &gSmoke[i].v, gSmoke[i].time_sync / 1000.0f);
                    gSmoke[i].time_sync = 0;
                } else {
                    BrVector3Scale(&tv, &gSmoke[i].v, pTime / 1000.0f);
                }
                BrVector3Accumulate(&gSmoke[i].pos, &tv);
            } else {
                gSmoke_flags &= ~(1u << i);
            }
        }
    }
    for (i = 0; i < CARPOCALYPSE2_ASIZE(gSmoke); i++) {
        if (gSmoke_flags & (1u << i)) {
            if ((gSmoke[i].type & 0xf) == 7) {
                not_lonely |= 1u << i;
            } else if (!(not_lonely & (1u << i))) {
                for (j = i + 1; j < CARPOCALYPSE2_ASIZE(gSmoke); j++) {
                    if ((gSmoke_flags & (1u << j))) {
                        BrVector3Sub(&tv, &gSmoke[i].pos, &gSmoke[i].pos);
                        ts = BrVector3LengthSquared(&tv);
                        if (CARPOCALYPSE2_SQR(gSmoke[i].radius + gSmoke[j].radius) > ts) {
                            not_lonely |= (1u << j) | (1u << i);
                            break;
                        }
                    }
                }
            }
            if (!((1u << i) & not_lonely)) {
                gSmoke[i].strength = gSmoke[i].strength / 2.f;
            }
            aspect = 1.f + (gSmoke[i].radius - .05f) * 4.f / 2.f;
            if ((gSmoke[i].type & 0x10)) {
                SmokeCircle3D(&gSmoke[i].pos, gSmoke[i].radius / aspect, gBR_smoke_structs[i].strength, 1.0, pRender_screen, pDepth_buffer, gShade_list[gSmoke[i].type & 0xf], pCamera);
            } else {
                SmokeCircle3D(&gSmoke[i].pos, gSmoke[i].radius, gBR_smoke_structs[i].strength, aspect, pRender_screen, pDepth_buffer, gShade_list[gSmoke[i].type & 0xf], pCamera);
            }
            if (gSmoke[i].pipe_me) {
                AddSmokeToPipingSession(i, gSmoke[i].type, &gSmoke[i].pos, gSmoke[i].radius, gSmoke[i].strength);
            }
            gSmoke[i].radius = pTime / 1000.0f * gSmoke[i].strength * .5f + gSmoke[i].radius;
            gSmoke[i].strength -= pTime * gSmoke[i].decay_factor / 1000.0f;
            if (gSmoke[i].radius > 0.3f) {
                gSmoke[i].radius = 0.3f;
            }
            if (gSmoke[i].strength > 0.0f) {
                ts = 1.0f - pTime * 0.002f;
                if (ts < 0.5f) {
                    ts = 0.5f;
                }
                BrVector3Scale(&gSmoke[i].v, &gSmoke[i].v, ts);
                if (fabs(gSmoke[i].v.v[1]) < 0.43478259f && (gSmoke[i].type & 0xfu) < 7) {
                    if (gSmoke[i].v.v[1] >= 0.0) {
                        gSmoke[i].v.v[1] = 0.43478259f;
                    } else {
                        gSmoke[i].v.v[1] += 0.43478259f;
                    }
                }
            } else {
                gSmoke_flags &= ~(1u << i);
            }
        }
    }
    AREndPipingSession();
    srand(seed);
    if (gNo_2d_effects) {
        RenderRecordedSmokeCircles();
        if (gBlend_actor->parent != NULL) {
            BrActorRemove(gBlend_actor);
        }
        BrActorAdd(gDont_render_actor, gBlend_actor);
    }
}
// DrawDot

// SetWorldToScreen

// DrawLine3DThroughBRender

// DrawLine3D

// DrawLine2D

// SetLineModelCols

// ReplaySparks

// RenderSparks

// CreateSingleSpark

// CreateSparks

// CreateSparkShower

// AdjustSpark

// AdjustShrapnel

// ResetSparks

// ResetShrapnel

// CreateShrapnelShower2

// CreateShrapnelShowerWorld

// CreateShrapnelShower

// random

// InitShrapnel

// LoadInShrapnel

// KillShrapnel

// DisposeShrapnel

// ReplayShrapnel

// ActorFunks

// FUNCTION: CARMA2_HW 0x004f8dc0
intptr_t C2_HOOK_CDECL ActorFunks(br_actor* pActor, void* pCallback);

// FUNCTION: CARMA2_HW 0x004f8ca0
void C2_HOOK_FASTCALL MasterDisableCarFunks(tCar_spec* pCar) {
#ifndef CARPOCALYPSE2_MATCHING
    /* stub: no-op for Linux boot */
#else
    int i;
    int j;
    tCar_crush_spec* spec = pCar->car_crush_spec;
    br_actor* actor;
    tUser_crush_data* user;
    tCar_crush_buffer_entry* crush;

    DRActorEnumRecurse(pCar->car_model_actor, ActorFunks, (void*)MasterDisableFunkotronic);
    for (i = 0; i < spec->field_0x270; i++) {
        actor = spec->field_0x274[i].field_0x0;
        user = (tUser_crush_data*)actor->user;
        if (user == NULL) {
            continue;
        }
        crush = user->crush_data;
        if (crush == NULL) {
            continue;
        }
        if (crush->smashables == NULL) {
            continue;
        }
        for (j = 0; j < crush->count_smashables; j++) {
            int funk_index = crush->smashables[j].funk;
            if (funk_index >= 0) {
                MasterDisableFunkotronic(funk_index);
            }
        }
    }
    for (i = 0; i < spec->field_0x2b0; i++) {
        actor = spec->field_0x2b4[i].field_0x0;
        user = (tUser_crush_data*)actor->user;
        if (user == NULL) {
            continue;
        }
        crush = user->crush_data;
        if (crush == NULL) {
            continue;
        }
        if (crush->smashables == NULL) {
            continue;
        }
        for (j = 0; j < crush->count_smashables; j++) {
            int funk_index = crush->smashables[j].funk;
            if (funk_index >= 0) {
                MasterDisableFunkotronic(funk_index);
            }
        }
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004f8e10
void C2_HOOK_FASTCALL MasterEnableCarFunks(tCar_spec* pCar) {
#ifndef CARPOCALYPSE2_MATCHING
    /* stub: no-op for Linux boot */
#else
    int i;
    int j;
    tCar_crush_spec* spec = pCar->car_crush_spec;
    br_actor* actor;
    tUser_crush_data* user;
    tCar_crush_buffer_entry* crush;

    DRActorEnumRecurse(pCar->car_model_actor, ActorFunks, (void*)MasterEnableFunkotronic);
    for (i = 0; i < spec->field_0x270; i++) {
        actor = spec->field_0x274[i].field_0x0;
        user = (tUser_crush_data*)actor->user;
        if (user == NULL) {
            continue;
        }
        crush = user->crush_data;
        if (crush == NULL) {
            continue;
        }
        if (crush->smashables == NULL) {
            continue;
        }
        for (j = 0; j < crush->count_smashables; j++) {
            int funk_index = crush->smashables[j].funk;
            if (funk_index >= 0) {
                MasterEnableFunkotronic(funk_index);
            }
        }
    }

    for (i = 0; i < spec->field_0x2b0; i++) {
        actor = spec->field_0x2b4[i].field_0x0;
        user = (tUser_crush_data*)actor->user;
        if (user == NULL) {
            continue;
        }
        crush = user->crush_data;
        if (crush == NULL) {
            continue;
        }
        if (crush->smashables == NULL) {
            continue;
        }
        for (j = 0; j < crush->count_smashables; j++) {
            int funk_index = crush->smashables[j].funk;
            if (funk_index >= 0) {
                MasterEnableFunkotronic(funk_index);
            }
        }
    }
#endif
}

// SetPixelmap

// FUNCTION: CARMA2_HW 0x004f8f30
void C2_HOOK_FASTCALL DoCamouflageThing(tCar_spec* pCar) {
#ifndef CARPOCALYPSE2_MATCHING
    BlendifyCar(pCar);
#else
    NOT_IMPLEMENTED();
#endif
}

// DoSolidGraniteThing

// DoTrueColModelThing

// SetFoxModelCallback

// ResetFoxModelCallback

// ResetFoxLighting

// CancelAlternateFoxEffect

// SetFoxLighting

// SetupAlternateFoxEffect

// RestorePixelmap

// FUNCTION: CARMA2_HW 0x004f9620
void C2_HOOK_FASTCALL RestoreCarPixelmaps(tCar_spec* pCar_spec) {
#ifndef CARPOCALYPSE2_MATCHING
    UnBlendifyCar(pCar_spec);
#else
    tCarFunks_master* funks;
    int i;
    int j;
    int funk_index;

    funks = (tCarFunks_master*)*(void**)((char*)pCar_spec + 0x18d4);

    DRActorEnumRecurse(pCar_spec->car_model_actor, ActorFunks, (void*)MasterEnableFunkotronic);

    for (i = 0; i < funks->count_0x270; i++) {
        br_actor* bit_actor = funks->bit_0x274[i].field_0x0;

        if (bit_actor->user != NULL) {
            tFunk_list* funk_list = ((tFunk_container*)bit_actor->user)->funk_list;

            if (funk_list != NULL) {
                if (funk_list->funk_records != NULL) {
                    for (j = 0; j < funk_list->funk_count; j++) {
                        funk_index = funk_list->funk_records[j].funk_index;
                        if (funk_index >= 0) {
                            MasterEnableFunkotronic(funk_index);
                        }
                    }
                }
            }
        }
    }

    for (i = 0; i < funks->count_0x2b0; i++) {
        br_actor* bit_actor = funks->bit_0x2b4[i].field_0x0;

        if (bit_actor->user != NULL) {
            tFunk_list* funk_list = ((tFunk_container*)bit_actor->user)->funk_list;

            if (funk_list != NULL) {
                if (funk_list->funk_records != NULL) {
                    for (j = 0; j < funk_list->funk_count; j++) {
                        funk_index = funk_list->funk_records[j].funk_index;
                        if (funk_index >= 0) {
                            MasterEnableFunkotronic(funk_index);
                        }
                    }
                }
            }
        }
    }

    ForEveryCarMaterial(pCar_spec, RestorePixelmap, 1);
    pCar_spec->field_0x1960 = NULL;
#endif
}

// MungeShrapnel

// DrMatrix34Rotate

// CmpSmokeZ

// FUNCTION: CARMA2_HW 0x004f9fc0
void C2_HOOK_FASTCALL InitSmokeStuff(void) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;

    gBlend_model = BrModelAllocate("BlendModel", 4, 2);
    gBlend_model->flags |= BR_MODF_UPDATEABLE;
    BrVector3SetFloat(&gBlend_model->vertices[0].p, -0.5f, 0.f, 0.f);
    BrVector3SetFloat(&gBlend_model->vertices[1].p, 0.5f, 0.f, 0.f);
    BrVector3SetFloat(&gBlend_model->vertices[2].p, 0.5f, 1.f, 0.f);
    BrVector3SetFloat(&gBlend_model->vertices[3].p, -0.5f, 1.f, 0.f);
    BrVector2Set(&gBlend_model->vertices[0].map, 0.f, 1.f);
    BrVector2Set(&gBlend_model->vertices[1].map, 1.f, 1.f);
    BrVector2Set(&gBlend_model->vertices[2].map, 1.f, 0.f);
    BrVector2Set(&gBlend_model->vertices[3].map, 0.f, 0.f);
    gBlend_model->faces[0].vertices[0] = 0;
    gBlend_model->faces[0].vertices[1] = 1;
    gBlend_model->faces[0].vertices[2] = 2;
    gBlend_model->faces[1].vertices[0] = 0;
    gBlend_model->faces[1].vertices[1] = 2;
    gBlend_model->faces[1].vertices[2] = 3;
    BrModelAdd(gBlend_model);

    gBlend_model2 = BrModelAllocate("BlendModel2", 4, 2);
    gBlend_model2->flags |= BR_MODF_UPDATEABLE;
    BrVector3SetFloat(&gBlend_model2->vertices[0].p, -0.5f, 0.f, 0.f);
    BrVector3SetFloat(&gBlend_model2->vertices[1].p, 0.5f, 0.f, 0.f);
    BrVector3SetFloat(&gBlend_model2->vertices[2].p, 0.5f, 1.f, 0.f);
    BrVector3SetFloat(&gBlend_model2->vertices[3].p, -0.5f, 1.f, 0.f);
    BrVector2Set(&gBlend_model2->vertices[0].map, 0.f, 1.f);
    BrVector2Set(&gBlend_model2->vertices[1].map, 1.f, 1.f);
    BrVector2Set(&gBlend_model2->vertices[2].map, 1.f, 0.f);
    BrVector2Set(&gBlend_model2->vertices[3].map, 0.f, 0.f);
    gBlend_model2->faces[0].vertices[0] = 0;
    gBlend_model2->faces[0].vertices[1] = 1;
    gBlend_model2->faces[0].vertices[2] = 2;
    gBlend_model2->faces[1].vertices[0] = 0;
    gBlend_model2->faces[1].vertices[1] = 2;
    gBlend_model2->faces[1].vertices[2] = 3;
    BrModelAdd(gBlend_model2);

    gBlend_actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    gBlend_actor->render_style = BR_RSTYLE_FACES;
    gBlend_actor->model = gBlend_model;

    for (i = 0; i < CARPOCALYPSE2_ASIZE(gBR_smoke_structs); i++) {
        gBR_smoke_structs[i].material = BrMaterialAllocate(NULL);
        gBR_smoke_structs[i].material->flags &= ~BR_MATF_LIGHT;
        gBR_smoke_structs[i].material->flags |= BR_MATF_ALWAYS_VISIBLE;
        BrMaterialAdd(gBR_smoke_structs[i].material);
    }
#else
    br_pixelmap* pm;
    tPath_name path;
    int i;

    gBlend_model = BrModelAllocate("gBlend_model", 4, 2);
    gBlend_model2 = BrModelAllocate("gBlend_model2", 6, 4);
    gBlend_actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);

    for (i = 0; i < 35; i++) {
        gBR_smoke_structs[i].material = BrMaterialAllocate("some smoke");
        if (gBR_smoke_structs[i].material == NULL) {
            break;
        }
    }

    if (gBlend_model == NULL || i != 35 || gBlend_actor == NULL) {
        FatalError(kFatalError_OOM_S);
    }

    gBlend_actor->identifier = "gBlend_actor";
    gBlend_actor->model = gBlend_model;

    gBlend_model->faces[0].vertices[0] = 0;
    gBlend_model->faces[0].vertices[1] = 1;
    gBlend_model->faces[0].vertices[2] = 2;
    gBlend_model->faces[1].vertices[0] = 2;
    gBlend_model->faces[1].vertices[1] = 3;
    gBlend_model->faces[1].vertices[2] = 0;

    gBlend_model2->faces[0].vertices[0] = 0;
    gBlend_model2->faces[0].vertices[1] = 1;
    gBlend_model2->faces[0].vertices[2] = 2;
    gBlend_model2->faces[1].vertices[0] = 2;
    gBlend_model2->faces[1].vertices[1] = 3;
    gBlend_model2->faces[1].vertices[2] = 0;
    gBlend_model2->faces[2].vertices[0] = 1;
    gBlend_model2->faces[2].vertices[1] = 5;
    gBlend_model2->faces[2].vertices[2] = 2;
    gBlend_model2->faces[3].vertices[0] = 1;
    gBlend_model2->faces[3].vertices[1] = 4;
    gBlend_model2->faces[3].vertices[2] = 5;

    gBlend_model->vertices[0].p.v[0] = -1.0f;
    gBlend_model->vertices[0].p.v[1] = 1.0f;
    gBlend_model->vertices[0].p.v[2] = 0.0f;
    gBlend_model->vertices[1].p.v[0] = -1.0f;
    gBlend_model->vertices[1].p.v[1] = -1.0f;
    gBlend_model->vertices[1].p.v[2] = 0.0f;
    gBlend_model->vertices[2].p.v[0] = 1.0f;
    gBlend_model->vertices[2].p.v[1] = -1.0f;
    gBlend_model->vertices[2].p.v[2] = 0.0f;
    gBlend_model->vertices[3].p.v[0] = 1.0f;
    gBlend_model->vertices[3].p.v[1] = 1.0f;
    gBlend_model->vertices[3].p.v[2] = 0.0f;

    gBlend_model2->vertices[0].p.v[0] = -1.0f;
    gBlend_model2->vertices[0].p.v[1] = 1.0f;
    gBlend_model2->vertices[0].p.v[2] = 0.0f;
    gBlend_model2->vertices[1].p.v[0] = -1.0f;
    gBlend_model2->vertices[1].p.v[1] = 0.0f;
    gBlend_model2->vertices[1].p.v[2] = 0.0f;
    gBlend_model2->vertices[2].p.v[0] = 1.0f;
    gBlend_model2->vertices[2].p.v[1] = 0.0f;
    gBlend_model2->vertices[2].p.v[2] = 0.0f;
    gBlend_model2->vertices[3].p.v[0] = 1.0f;
    gBlend_model2->vertices[3].p.v[1] = 1.0f;
    gBlend_model2->vertices[3].p.v[2] = 0.0f;
    gBlend_model2->vertices[4].p.v[0] = -1.0f;
    gBlend_model2->vertices[4].p.v[1] = -0.25f;
    gBlend_model2->vertices[4].p.v[2] = 0.0f;
    gBlend_model2->vertices[5].p.v[0] = 1.0f;
    gBlend_model2->vertices[5].p.v[1] = -0.25f;
    gBlend_model2->vertices[5].p.v[2] = 0.0f;

    gBlend_model->flags |= BR_MODF_KEEP_ORIGINAL;
    gBlend_model2->flags |= BR_MODF_KEEP_ORIGINAL;

    memcpy(path, "SMOKE.PIX", sizeof("SMOKE.PIX"));
    pm = LoadPixelmap(path);
    if (pm == NULL) {
        FatalError(kFatalError_CantLoadPixelmapFile_S, path);
    }

    pm->map = gRender_palette;
    BrMapAdd(pm);

    for (i = 0; i < 35; i++) {
        gBR_smoke_structs[i].material->flags = BR_MATF_LIGHT | BR_MATF_PRELIT | BR_MATF_SMOOTH | BR_MATF_PERSPECTIVE;
        gBR_smoke_structs[i].material->extra_prim = (br_token_value*)&gDAT_00660148;
        gBR_smoke_structs[i].material->colour_map = pm;
        BrMaterialAdd(gBR_smoke_structs[i].material);
    }

    gBlend_model->vertices[0].map.v[0] = 0.0f;
    gBlend_model->vertices[0].map.v[1] = 100.0f - 100.0f / (br_scalar)pm->height;
    gBlend_model->vertices[1].map.v[0] = 0.0f;
    gBlend_model->vertices[1].map.v[1] = 0.0f;
    gBlend_model->vertices[2].map.v[0] = 100.0f - 100.0f / (br_scalar)pm->width;
    gBlend_model->vertices[2].map.v[1] = 0.0f;
    gBlend_model->vertices[3].map.v[0] = 100.0f - 100.0f / (br_scalar)pm->width;
    gBlend_model->vertices[3].map.v[1] = 100.0f - 100.0f / (br_scalar)pm->height;

    gBlend_model2->vertices[0].map.v[0] = 0.0f;
    gBlend_model2->vertices[0].map.v[1] = 100.0f - 100.0f / (br_scalar)pm->height;
    gBlend_model2->vertices[1].map.v[0] = 0.0f;
    gBlend_model2->vertices[1].map.v[1] = 0.5f;
    gBlend_model2->vertices[2].map.v[0] = 100.0f - 100.0f / (br_scalar)pm->width;
    gBlend_model2->vertices[2].map.v[1] = 0.5f;
    gBlend_model2->vertices[3].map.v[0] = 100.0f - 100.0f / (br_scalar)pm->width;
    gBlend_model2->vertices[3].map.v[1] = 100.0f - 100.0f / (br_scalar)pm->height;
    gBlend_model2->vertices[4].map.v[0] = 0.0f;
    gBlend_model2->vertices[4].map.v[1] = 0.0f;
    gBlend_model2->vertices[5].map.v[0] = 100.0f - 100.0f / (br_scalar)pm->width;
    gBlend_model2->vertices[5].map.v[1] = 0.0f;

    BrModelAdd(gBlend_model);
    BrModelAdd(gBlend_model2);
    BrActorAdd(gDont_render_actor, gBlend_actor);
#endif
}

// RenderRecordedSmokeCircles

// RecordSmokeCircle

// SmokeCircle3D

// ReplaySmoke

// GenerateContinuousSmoke

// RenderSmoke

// CreatePuffOfSmoke

// ResetSmoke

// FUNCTION: CARMA2_HW 0x004fb520
void C2_HOOK_FASTCALL AdjustSmoke(int pIndex, tU8 pType, br_vector3* pPos, br_scalar pRadius, br_scalar pStrength) {
    gSmoke[pIndex].type = pType;
    memcpy(&gSmoke[pIndex].radius, &pRadius, sizeof(pRadius));
    memcpy(&gSmoke[pIndex].strength, &pStrength, sizeof(pStrength));
    memcpy(&gSmoke[pIndex].pos.v[0], &pPos->v[0], sizeof(br_scalar));
    memcpy(&gSmoke[pIndex].pos.v[1], &pPos->v[1], sizeof(br_scalar));
    memcpy(&gSmoke[pIndex].pos.v[2], &pPos->v[2], sizeof(br_scalar));
    gSmoke_flags |= 1u << pIndex;
}

// ActorError

// FUNCTION: CARMA2_HW 0x004fb580
void C2_HOOK_FASTCALL AdjustSmokeColumn(int pIndex, const tSmoke_column_core* pCore, int pVertex, int pColour) {
    tSmoke_column* column;
    tU32 bit;

    bit = 1u << (pIndex & 31);
    gColumn_flags ^= bit;
    column = &gSmoke_column[pIndex];
    /* Four separate MOVs preserve the original forward overlap ordering. */
    column->core.field_0x0 = pCore->field_0x0;
    column->core.car = pCore->car;
    column->core.collision_info = pCore->collision_info;
    column->core.car_actor = pCore->car_actor;
    column->vertex_index = pVertex;
    column->colour = pColour;
    column->frame_count[0] = 100;
    column->frame_count[1] = 100;
    column->frame_count[2] = 100;
    if (pColour == 0) {
        if ((gColumn_flags & bit) != 0) {
            BrActorAdd(gNon_track_actor, column->flame_actor);
        } else if (column->flame_actor->depth != 0) {
            BrActorRemove(column->flame_actor);
        }
    }
}

// CreateSmokeColumn2

// FUNCTION: CARMA2_HW 0x004fb890
void C2_HOOK_FASTCALL CreateSmokeColumn(tCar_spec* pCar, int pType, int pRandom, int pLifetime) {

    if (pCar->collision_info->last_special_volume != NULL
        && pCar->collision_info->last_special_volume->gravity_multiplier < 1.0f) {
        return;
    }
    SmudgeCar(pCar, pRandom);
    if (pCar->knackered) {
        SmudgeCar(pCar, pCar->fire_vertex[IRandomBetween(0, 0xb)]);
    }
    CreateSmokeColumn2(1, pCar->car_master_actor, pCar, NULL, pType, pRandom, pLifetime);
}

// GenerateSmokeShades

// FUNCTION: CARMA2_HW 0x004fb9c0
void C2_HOOK_FASTCALL GenerateItFoxShadeTable(void) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;
    br_pixelmap* pm;

    pm = BrPixelmapAllocate(BR_PMT_INDEX_8, 64, 1, NULL, 0);
    for (i = 0; i < 64; i++) {
        ((tU8*)pm->pixels)[i] = (tU8)(i * 4);
    }
    gIt_shade_table = pm;
    BrTableAdd(pm);
#else
    int i;
    br_pixelmap* pm;

    pm = BrPixelmapAllocate(BR_PMT_INDEX_8, 64, 1, NULL, 0);
    for (i = 0; i < 64; i++) {
        ((tU8*)pm->pixels)[i] = (tU8)(i * 4);
    }
    gIt_shade_table = pm;
    BrTableAdd(pm);
#endif
}

// FUNCTION: CARMA2_HW 0x004fba00
void C2_HOOK_FASTCALL AdjustFlame(int pIndex, int pFrame, br_scalar pScale_x, br_scalar pScale_y, br_scalar pOffset_x, br_scalar pOffset_z) {
    tSmoke_column* column;
    int flame;
    int i;
    tU32 bits[4];
    br_scalar* dest[4];

    column = &gSmoke_column[pIndex >> 4];
    flame = pIndex & 15;
    column->frame_count[flame] = pFrame;
    memcpy(&bits[0], &pScale_x, 4);
    memcpy(&bits[1], &pScale_y, 4);
    memcpy(&bits[2], &pOffset_x, 4);
    memcpy(&bits[3], &pOffset_z, 4);
    dest[0] = &column->scale_x[flame];
    dest[1] = &column->scale_y[flame];
    dest[2] = &column->offset_x[flame];
    dest[3] = &column->offset_z[flame];
    for (i = 0; i < 4; i++) {
        /* Original FLD/FSTP stores quiet sNaNs, preserving sign and payload. */
        if ((bits[i] & 0x7fc00000u) == 0x7f800000u && (bits[i] & 0x003fffffu) != 0) {
            bits[i] |= 0x00400000u;
        }
        memcpy(dest[i], &bits[i], 4);
    }
}

// ReplayFlame

// FlameAnimate

// DoSmokeColumn

// ReplaySmokeColumn

// MungeSmokeColumn

// FUNCTION: CARMA2_HW 0x004fc2e0
void C2_HOOK_FASTCALL DisposeFlame(void) {
    int i;
    int j;
    br_actor* child;

    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gFlame_map); i++) {
        BrMapRemove(gFlame_map[i]);
        BrPixelmapFree(gFlame_map[i]);
    }
    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gSmoke_column); i++) {
        if ((gColumn_flags & (1u << i)) != 0 && gSmoke_column[i].colour == 0) {
            BrActorRemove(gSmoke_column[i].flame_actor);
        }
        child = gSmoke_column[i].flame_actor->children;
        for (j = 0; j < 3; j++) {
            BrMaterialRemove(child->material);
            BrMaterialFree(child->material);
            child = child->next;
        }
        BrActorFree(gSmoke_column[i].flame_actor);
    }
    BrModelRemove(gLollipop_model);
    BrModelFree(gLollipop_model);
}

// InitFlame

// InitSplash

// DisposeSplash

// DrawTheGlow

// PipeInstantUnSmudge

// SmudgeCar

// FUNCTION: CARMA2_HW 0x004fc9d0
void C2_HOOK_FASTCALL SetSmokeOn(int pSmoke_on) {

    gSmoke_on = pSmoke_on;
}

// FUNCTION: CARMA2_HW 0x004fca30
int C2_HOOK_FASTCALL GetSmokeOn(void) {

    return gSmoke_on;
}

// StopCarSmoking

// StopCarSmokingInstantly

// StopObjectSmokingInstantly

// ConditionalSmokeColumn

// SingleSplash

// FUNCTION: CARMA2_HW 0x004fd530
void C2_HOOK_FASTCALL SingleSplash(tPhysics_object* pObject, const br_matrix34* pMatrix, const br_vector3* pVelocity, const br_vector3* pNormal_velocity, const br_vector3* pOmega, const br_vector3* pPoint, const br_vector3* pNormal, tU32 pTime) {
    br_vector3 cross;
    br_vector3 velocity;
    br_vector3 recoil;
    br_scalar scaled;
    br_scalar speed;
    br_scalar size;
    br_scalar range;
    double dot;
    double random;
    double wide;
    tCar_spec* car;
    int index;
    int i;

    BrMatrix34ApplyP((br_vector3*)gSplash[gSplash_next].actor->t.t.mat.m[3], pPoint, pMatrix);
    cross.v[0] = SmokeScalarRound((double)pOmega->v[1] * pPoint->v[2] - (double)pOmega->v[2] * pPoint->v[1]);
    cross.v[1] = SmokeScalarRound((double)pOmega->v[2] * pPoint->v[0] - (double)pOmega->v[0] * pPoint->v[2]);
    cross.v[2] = SmokeScalarRound((double)pOmega->v[0] * pPoint->v[1] - (double)pOmega->v[1] * pPoint->v[0]);
    BrMatrix34ApplyV(&velocity, &cross, pMatrix);
    for (i = 0; i < 3; i++) {
        scaled = SmokeScalarRound((double)pVelocity->v[i] * 6.9);
        velocity.v[i] = SmokeScalarRound((double)velocity.v[i] + scaled);
    }
    speed = SmokeScalarRound(sqrt(((double)velocity.v[0] * velocity.v[0]
        + (double)velocity.v[1] * velocity.v[1]) + (double)velocity.v[2] * velocity.v[2]));
    dot = ((double)pNormal->v[1] * velocity.v[1] + (double)pNormal->v[2] * velocity.v[2])
        + (double)pNormal->v[0] * velocity.v[0];
    wide = SplashSubtractFromScalar(fabs(dot) * -5.f, &speed) * .006666666828095913f - -.04782608523964882f;
    size = SmokeScalarRound(wide);
    if (wide > .5f) {
        size = .5f;
    }
    dot = ((double)pNormal_velocity->v[2] * pPoint->v[2] + (double)pNormal_velocity->v[1] * pPoint->v[1])
        + (double)pNormal_velocity->v[0] * pPoint->v[0];
    if (!(dot >= 0.0)) {
        size = SmokeScalarRound((double)size * .5f);
    }
    index = gSplash_next;
    gSplash[index].size = SmokeScalarRound(SRandomBetween(SmokeScalarRound((double)size * .5f), size));
    if (!((tU32)gSplash_flags & (1u << (gSplash_next & 31)))) {
        BrActorAdd(gNon_track_actor, gSplash[index].actor);
    }
    gSplash_flags = (int)((tU32)gSplash_flags | (1u << (gSplash_next & 31)));
    gSplash[gSplash_next].just_done = 1;
    if (pObject != NULL && !gAction_replay_mode) {
        car = pObject->flags_0x238 == 1 ? (tCar_spec*)pObject->owner : NULL;
        if (car != NULL) {
            random = SRandomBetween(0.f, 1.f);
            if (random < (double)pTime * .003f) {
                range = SmokeScalarRound(sqrt((double)speed * .014285714365541935f) * 15.f);
                for (i = 0; i < 3; i++) {
                    wide = (double)velocity.v[i] * .14492753148078918f;
                    velocity.v[i] = SmokeScalarRound(wide);
                    recoil.v[i] = SmokeScalarRound(wide * -.1f);
                }
                if (range > 15.f) {
                    range = 15.f;
                }
                random = SRandomBetween(5.f, range);
                recoil.v[1] = SmokeScalarRound((double)recoil.v[1] - random * -.14492753623188406);
                BrMatrix34TApplyV(&velocity, &recoil, &pObject->actor->t.t.mat);
                cross.v[0] = SmokeScalarRound(((double)pObject->water_normal.v[2] * velocity.v[1]
                    - (double)pObject->water_normal.v[1] * velocity.v[2]) * .5f);
                cross.v[1] = SmokeScalarRound(((double)pObject->water_normal.v[0] * velocity.v[2]
                    - (double)pObject->water_normal.v[2] * velocity.v[0]) * .5f);
                cross.v[2] = SmokeScalarRound(((double)pObject->water_normal.v[1] * velocity.v[0]
                    - (double)pObject->water_normal.v[0] * velocity.v[1]) * .5f);
                dot = ((double)pPoint->v[1] * cross.v[1] + (double)pPoint->v[2] * cross.v[2])
                    + (double)pPoint->v[0] * cross.v[0];
                for (i = 0; i < 3; i++) {
                    velocity.v[i] = SmokeScalarRound(dot > 0.0 ? (double)velocity.v[i] + cross.v[i]
                        : (double)velocity.v[i] - cross.v[i]);
                }
                index = gSpark_next;
                gSparks[index].pos.v[0] = pPoint->v[0];
                gSparks[index].pos.v[1] = pPoint->v[1];
                gSparks[index].pos.v[2] = pPoint->v[2];
                BrVector3SetFloat(&gSparks[index].normal, 0.f, 0.f, 0.f);
                index = gSpark_next;
                memcpy(&gSparks[index].v, &velocity, sizeof(velocity));
                gSparks[index].count = 500;
                gSparks[index].car = car;
                gSparks[index].time_sync = 1;
    gSparks[index].colour = 1;
                gSpark_flags = (int)((tU32)gSpark_flags | (1u << (index & 31)));
                gSpark_next = (int)((tU32)index + 1u);
                if (gSpark_next >= 32) {
                    gSpark_next = 0;
                }
            }
        }
    }
    gSplash_next = (int)((tU32)gSplash_next + 1u);
    if ((tU32)gSplash_next >= 32u) {
        gSplash_next = 0;
    }
}

// GeneralCreateSplash

// CreateSplash

// GetVelocitiesFromMatrices

// MungeSplash

// GetSmokeShadeTables

// FreeSmokeShadeTables

// LoadInKevStuff

// DisposeKevStuff

// DisposeKevStuffCar

// MakeCarIt

// StopCarBeingIt

// ForEveryModelMaterial

// ForEveryActorMaterial

// ForEveryActorMaterialNoGrooves

// ForEveryCarMaterial

// ForEveryCarModelCB

// ForEveryCarModel

// MightBlendifyMaterial

// BlendifyMaterialCB

// FUNCTION: CARMA2_HW 0x004fecf0
void C2_HOOK_FASTCALL BlendifyCar(tCar_spec* pCar) {

    ForEveryCarMaterial(pCar, BlendifyMaterialCB, 1);
}

// UnBlendifyMaterialCB

// FUNCTION: CARMA2_HW 0x004fed40
void C2_HOOK_FASTCALL UnBlendifyCar(tCar_spec* pCar_spec) {

    ForEveryCarMaterial(pCar_spec, UnBlendifyMaterialCB, 1);
}

// FUNCTION: CARMA2_HW 0x004fed90
int C2_HOOK_FASTCALL IsCarSmoking(tCar_spec* pCar) {
    int i;

    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gSmoke_column); i++) {
        if ((gColumn_flags & (1 << i)) && gSmoke_column[i].core.car == pCar) {
            return 1;
        }
    }
    return 0;
}

// ClearSplashReplay

// GetSplashIndex

// ClearSplashes

// AdjustSplashReplay

// InitReplaySplashes
