#include "graphics1.h"

#include "init.h"
#include "displays.h"
#include "font.h"
#include "mainloop.h"
#include "utility.h"
#include "errors.h"
#include "loading3.h"
#include "brender/brender.h"
#include "globvars.h"
#include "globvrbm.h"
#include "globvrpb.h"
#include "platform.h"
#include "carpocalypse2_types.h"
#include "carpocalypse2_macros.h"
// GLOBAL: CARMA2_HW 0x00703e24
int gWidth;

// GLOBAL: CARMA2_HW 0x00703e20
int gHeight;

// GLOBAL: CARMA2_HW 0x00705060
int gX_offset;

// GLOBAL: CARMA2_HW 0x006baa2c
int gY_offset;

// GLOBAL: CARMA2_HW 0x0068be30
int gBrZb_initialized;

// GLOBAL: CARMA2_HW 0x006a22bc
br_pixelmap* gRear_pixelmap;

// GLOBAL: CARMA2_HW 0x006a22b8
br_pixelmap* gEvalu;

// GLOBAL: CARMA2_HW 0x006a22f8
tWobble_spec gWobble_array[5];

// GLOBAL: CARMA2_HW 0x00705080
float gCosine_array[64];

// GLOBAL: CARMA2_HW 0x0065fdc8
tShadow_level gShadow_level = eShadow_us_only;

// GLOBAL: CARMA2_HW 0x0068be38
int gRender_indent;

// GLOBAL: CARMA2_HW 0x006baa40
tU8 gTemporary_physics_render_buffer[300000];

// GLOBAL: CARMA2_HW 0x00705188
int gScreen_wobble_x;

// GLOBAL: CARMA2_HW 0x00705184
int gScreen_wobble_y;

// GLOBAL: CARMA2_HW 0x00703e28
int gHud_actor_storage_size;

// GLOBAL: CARMA2_HW 0x00704e60
br_actor* gHud_actor_storage[128];

// GLOBAL: CARMA2_HW 0x00655e54
int gHeadup_detail_level = 5;

// MungeClipPlane

// TryThisEdge

// ProcessShadow

// RenderShadows

// FUNCTION: CARMA2_HW 0x004e9940
void C2_HOOK_FASTCALL SetShadowLevel(tShadow_level pLevel) {

    gShadow_level = pLevel;
}

// FUNCTION: CARMA2_HW 0x004e9950
tShadow_level C2_HOOK_FASTCALL GetShadowLevel(void) {

    return gShadow_level;
}

// GetShadowLevel

// ToggleShadow

// GLOBAL: CARMA2_HW 0x006a2444
extern br_actor* gShadow_actor;

// GLOBAL: CARMA2_HW 0x006a27e4
extern br_model* gShadow_model;

// GLOBAL: CARMA2_HW 0x006a27e0
extern br_material* gShadow_material;

// GLOBAL: CARMA2_HW 0x006a27ec
extern br_vector3 gShadow_light_ray;

// GLOBAL: CARMA2_HW 0x006a27d8
extern br_vector3 gShadow_light_x;

// GLOBAL: CARMA2_HW 0x006a27e8
extern br_vector3 gShadow_light_z;

// GLOBAL: CARMA2_HW 0x006a0840
extern tClip_details gShadow_clip_planes[8];

// FUNCTION: CARMA2_HW 0x004e99d0
void C2_HOOK_FASTCALL InitShadow(void) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;

    gShadow_actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    gShadow_model = BrModelAllocate(NULL, 48, 16);
    gShadow_actor->model = gShadow_model;
    gShadow_actor->render_style = BR_RSTYLE_NONE;

    gShadow_material = BrMaterialAllocate(NULL);
    gShadow_material->flags &= ~BR_MATF_LIGHT;
    gShadow_material->flags |= BR_MATF_ALWAYS_VISIBLE;
    BrMaterialAdd(gShadow_material);

    BrVector3Set(&gShadow_light_ray, 0.f, -1.f, 0.f);
    BrVector3Set(&gShadow_light_x, 1.f, 0.f, 0.f);
    BrVector3Set(&gShadow_light_z, 0.f, 0.f, 1.f);

    for (i = 0; i < CARPOCALYPSE2_ASIZE(gShadow_clip_planes); i++) {
        gShadow_clip_planes[i].clip = BrActorAllocate(BR_ACTOR_CLIP_PLANE, NULL);
    }
#else
    NOT_IMPLEMENTED();
#endif
}

// SaveShadeTable

// SaveShadeTables

// GLOBAL: CARMA2_HW 0x006a2810
extern tSaved_table gSaved_shade_tables[100];

// GLOBAL: CARMA2_HW 0x006a26d8
extern int gSaved_table_count;

// FUNCTION: CARMA2_HW 0x004e9b60
void C2_HOOK_FASTCALL DisposeSavedShadeTables(void) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;

    for (i = 0; i < CARPOCALYPSE2_ASIZE(gSaved_shade_tables); i++) {
        if (gSaved_shade_tables[i].copy != NULL) {
            BrMemFree(gSaved_shade_tables[i].copy);
            gSaved_shade_tables[i].copy = NULL;
        }
    }
    gSaved_table_count = 0;
#else
    NOT_IMPLEMENTED();
#endif
}

// ShadowMode

// LollipopizeActor

// ResetLollipopQueue

// RenderLollipops

// SetupDepthBuffer

// FUNCTION: CARMA2_HW 0x004e4980
void C2_HOOK_FASTCALL SetBRenderScreenAndBuffers(int pX_offset, int pY_offset, int pWidth, int pHeight) {

    PDAllocateScreenAndBack();
    if (pWidth == 0) {
        pWidth = gBack_screen->width;
    }
    if (pHeight == 0) {
        pHeight = gBack_screen->height;
    }
    gRender_screen = DRPixelmapAllocateSub(gBack_screen, pX_offset, pY_offset, pWidth, pHeight);
    gWidth = pWidth;
    gHeight = pHeight;
    gX_offset = pX_offset;
    gY_offset = pY_offset;
    if (gGraf_specs[gGraf_spec_index].doubled) {
        gScreen->base_x = (gGraf_specs[gGraf_spec_index].phys_width - 2 * gGraf_specs[gGraf_spec_index].total_width) / 2;
        gScreen->base_y = (gGraf_specs[gGraf_spec_index].phys_height - 2 * gGraf_specs[gGraf_spec_index].total_height) / 2;
    } else {
        gScreen->base_x = (gGraf_specs[gGraf_spec_index].phys_width - gGraf_specs[gGraf_spec_index].total_width) / 2;
        gScreen->base_y = (gGraf_specs[gGraf_spec_index].phys_height - gGraf_specs[gGraf_spec_index].total_height) / 2;
    }

    gScreen->origin_x = 0;
    gScreen->origin_y = 0;
    if (gBack_screen == NULL) {
        FatalError(kFatalError_AllocateOffScreenBuffer);
    }

    if (gDepth_buffer != NULL) {
        BrPixelmapFree(gDepth_buffer);
        gDepth_buffer = NULL;
    }
    gDepth_buffer = BrPixelmapMatch(gBack_screen, BR_PMMATCH_DEPTH_16);
    if (gDepth_buffer == NULL) {
        FatalError(kFatalError_AllocateZBuffer);
    }

    BrZbsBegin(gRender_screen->type, gDepth_buffer->type, gTemporary_physics_render_buffer, sizeof(gTemporary_physics_render_buffer));
    gBrZb_initialized = 1;
    gRear_pixelmap = DRPixelmapAllocate(gScreen->type, 64, 64, NULL, 0);
    BrMapAdd(gRear_pixelmap);
}

// AdjustRenderScreenSize

// ScreenSmaller

// ScreenLarger

// FUNCTION: CARMA2_HW 0x004e4d30
void C2_HOOK_FASTCALL ClearWobbles(void) {
    int i;

    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gWobble_array); ++i) {
        gWobble_array[i].time_started = 0;
    }
}

// FUNCTION: CARMA2_HW 0x004e4d50
void C2_HOOK_FASTCALL InitWobbleStuff(void) {
    int i;

    ClearWobbles();
    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gCosine_array); i++) {
        gCosine_array[i] = (float)cos((double)i / 64.0 * 3.141592653589793 / 2.0);
    }

    gEvalu = LoadPixelmap("Evalu01.PIX");
}

// NewScreenWobble

// SetScreenWobble

// ResetScreenWobble

// CalculateWobblitude

// ProcessNonTrackActors

// ConditionallyFillWithSky

// FoxyStuff

// DoARenderPass

// DoACompleteRenderPass

// RenderAFrame

// FUNCTION: CARMA2_HW 0x004e5a70
void C2_HOOK_FASTCALL StartRenderingHeadups(void) {

    CleanPolyFontDanglers();
    gHud_actor_storage_size = 0;
    gRender_poly_text = 0;
}

// FUNCTION: CARMA2_HW 0x004e5ad0
void C2_HOOK_FASTCALL RenderThisHeadup(br_actor* pActor) {

    if (gHud_actor_storage_size >= (int)CARPOCALYPSE2_ASIZE(gHud_actor_storage)) {
        BrFailure("Not enough HUD actor storage");
    }
    gHud_actor_storage[gHud_actor_storage_size] = pActor;
    gHud_actor_storage_size += 1;
}

void C2_HOOK_FASTCALL CleanPolyFontDangler(void) {
    int i;
    br_actor* a;

    for (i = 0; i < gCount_polyfont_glyph_actors; i++) {
        a = gPolyfont_glyph_actors[i];
        if (a->parent != NULL) {
            BrActorRemove(a);
        }
    }
    gCount_polyfont_glyph_actors = 0;
}

// FUNCTION: CARMA2_HW 0x004e5b00
void C2_HOOK_FASTCALL StopRenderingHeadups(void) {
    int original_origin_x;
    int original_origin_y;
    int original_base_x;
    int original_base_Y;
    int i;
    br_actor* a;

    if (gCount_polyfont_glyph_actors != 0) {
        BrActorAdd(gHUD_root, gString_root_actor);
    }
    for (i = 0; i < gHud_actor_storage_size; i++) {
        a = gHud_actor_storage[i];
        if (a != NULL && a->prev == NULL) {
            BrActorAdd(gHUD_root, a);
        }
    }
    original_origin_x = gRender_screen->origin_x;
    original_origin_y = gRender_screen->origin_y;
    original_base_x = gRender_screen->base_x;
    original_base_Y = gRender_screen->base_y;
    gRender_screen->origin_x = 0;
    gRender_screen->origin_y = 0;
    gRender_screen->base_x = 0;
    gRender_screen->base_y = 0;
    BrZbSceneRender(gHUD_root, gHUD_camera, gRender_screen, gDepth_buffer);
    gRender_screen->origin_x = original_origin_x;
    gRender_screen->origin_y = original_origin_y;
    gRender_screen->base_x = original_base_x;
    gRender_screen->base_y = original_base_Y;
    if (gString_root_actor->parent != NULL) {
        BrActorRemove(gString_root_actor);
    }
    for (i = 0; i < gHud_actor_storage_size; i++) {
        a = gHud_actor_storage[i];
        if (a != NULL && a->parent != NULL) {
            BrActorRemove(a);
        }
    }
    gHud_actor_storage_size = 0;
    CleanPolyFontDangler();
    gRender_poly_text = 1;
}

// FUNCTION: CARMA2_HW 0x004e5c70
void C2_HOOK_FASTCALL CleanPolyFontDanglers(void) {
    int i;

    for (i = 0; i < gCount_polyfont_glyph_actors; i++) {
        br_actor* actor = gPolyfont_glyph_actors[i];
        if (actor->parent != NULL) {
            BrActorRemove(actor);
        }
    }
    gCount_polyfont_glyph_actors = 0;
}

// FUNCTION: CARMA2_HW 0x0044b6a0
void C2_HOOK_FASTCALL DoTestHeadup(void) {
    // GLOBAL: CARMA2_HW 0x006815c0
    static int do_headup_material;

    do_headup_material = !do_headup_material;
    if (do_headup_material) {
        BrMapUpdate(gCurrent_rev, BR_MAPU_ALL);
        gStatbarHUD1_material->colour_map = gCurrent_rev;
        BrMaterialUpdate(gStatbarHUD1_material, BR_MATU_COLOURMAP);
    } else {
        BrMapUpdate(gDamage_hud, BR_MAPU_ALL);
        gStatbarHUD5_material->colour_map = gDamage_hud;
        BrMaterialUpdate(gStatbarHUD5_material, BR_MATU_COLOURMAP);
    }
    if (gNet_mode == eNet_mode_none) {
        BrMatrix34Translate(&gArmour_actor->t.t.mat, 0.f, 0.f, 0.f);
        BrMatrix34Translate(&gPower_actor->t.t.mat, 0.f, 0.f, 0.f);
        BrMatrix34Translate(&gOffense_actor->t.t.mat, 0.f, 0.f, 0.f);
        if (gHeadup_detail_level % 3 > 0) {
            BrMatrix34Translate(&gStatbarHUD1_actor->t.t.mat, 142.f, 0.f, 0.f);
            BrMatrix34Translate(&gStatbarHUD5_actor->t.t.mat, -178.f, 0.f, 0.f);
            RenderThisHeadup(gStatbarHUD1_actor);
            RenderThisHeadup(gStatbarHUD5_actor);
            RenderThisHeadup(gTimerLeftHUD_actor);
            RenderThisHeadup(gTimerRightHUD_actor);
            if (gHeadup_detail_level % 3 > 1) {
                RenderThisHeadup(gArmour_actor);
                RenderThisHeadup(gPower_actor);
                RenderThisHeadup(gOffense_actor);
                RenderThisHeadup(gStatbarRightHUD_actor);
                RenderThisHeadup(gStatbarHUD3_actor);
                BrMatrix34Translate(&gStatbarHUD1_actor->t.t.mat, 0.f, 0.f, 0.f);
                BrMatrix34Translate(&gStatbarHUD5_actor->t.t.mat, 0.f, 0.f, 0.f);
            } else {
                PolyFontText(gHeadup_oppo_ped_text, 335, 50, kPolyfont_ingame_medium_blue, eJust_centre, 0);
            }
        }
        if (gHeadup_detail_level >= 1) {
            RenderThisHeadup(gHeadup_actor);
        }
    } else {
        if (gHeadup_detail_level % 3 >= 1) {
            BrMatrix34Translate(&gStatbarHUD5_actor->t.t.mat, -448.f, 0.f, 0.f);
            RenderThisHeadup(gStatbarHUD5_actor);
            RenderThisHeadup(gStatbarHUD1_actor);
            if (gHeadup_detail_level % 3 >= 2) {
                BrMatrix34Translate(&gArmour_actor->t.t.mat, 48.f, 35.f, 0.f);
                BrMatrix34Translate(&gPower_actor->t.t.mat, -102.f, 5.f, 0.f);
                BrMatrix34Translate(&gOffense_actor->t.t.mat, -252.f, -25.f, 0.f);
                RenderThisHeadup(gArmour_actor);
                RenderThisHeadup(gPower_actor);
                RenderThisHeadup(gOffense_actor);
            }
        }
        if (gHeadup_detail_level >= 3) {
            RenderThisHeadup(gHeadup_actor);
        }
    }
}