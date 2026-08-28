#include "graphics.h"

#include "car.h"
#include "controls.h"
#include "depth.h"
#include "displays.h"
#include "errors.h"
#include "finteray.h"
#include "globvars.h"
#include "globvrbm.h"
#include "globvrpb.h"
#include "grafdata.h"
#include "graphics.h"
#include "init.h"
#include "input.h"
#include "loading.h"
#include "oil.h"
#include "mainloop.h"
#include "netgame.h"
#include "network.h"
#include "opponent.h"
#include "pedestrn.h"
#include "polyfont.h"
#include "physics.h"
#include "piping.h"
#include "polyfont.h"
#include "powerups.h"
#include "replay.h"
#include "spark.h"
#include "tinted.h"
#include "trig.h"
#include "utility.h"
#include "video.h"
#include "world.h"

#include "platform.h"

#include "brender/brender.h"
#include "brender/br_types.h"

#include "carpocalypse2_macros.h"
#include "carpocalypse2_types.h"

#include "c2_string.h"

#include <float.h>
#include "c2_math.h"

#define SHADOW_D_IGNORE_FLAG 10000.f
#define SHADOW_MAX_RENDER_DISTANCE 36.0f
// GLOBAL: CARMA2_HW 0x006a22b8
extern br_pixelmap* gEvalu;
// GLOBAL: CARMA2_HW 0x0074a620
extern br_colour gColours[9];
// GLOBAL: CARMA2_HW 0x0065cf30
extern br_colour gRGB_colours[9];
// GLOBAL: CARMA2_HW 0x0065fdc8
extern tShadow_level gShadow_level;
// GLOBAL: CARMA2_HW 0x006a22bc
extern br_pixelmap* gRear_pixelmap;
// GLOBAL: CARMA2_HW 0x0059ad30
extern const char* gFont_names[24];
// GLOBAL: CARMA2_HW 0x0074abc0
int gMap_render_x_i;

// GLOBAL: CARMA2_HW 0x0074abbc
int gMap_render_y_i;

// GLOBAL: CARMA2_HW 0x0074abe8
int gMap_render_width_i;

// GLOBAL: CARMA2_HW 0x0074abc8
int gMap_render_height_i;

// GLOBAL: CARMA2_HW 0x006a2448
tClip_details gShadow_clip_planes[8];

// GLOBAL: CARMA2_HW 0x006a23d8
int gFancy_shadow;

// GLOBAL: CARMA2_HW 0x006a27e0
br_material* gShadow_material;

// GLOBAL: CARMA2_HW 0x006a27d0
br_vector3 gShadow_light_ray;

// GLOBAL: CARMA2_HW 0x006a27c0
br_vector3 gShadow_light_z;

// GLOBAL: CARMA2_HW 0x006a27b0
br_vector3 gShadow_light_x;

// GLOBAL: CARMA2_HW 0x006a27e4
br_model* gShadow_model;

// GLOBAL: CARMA2_HW 0x006a2444
br_actor* gShadow_actor;

// GLOBAL: CARMA2_HW 0x0068d8c8
int gTarget_lock_enabled;

// GLOBAL: CARMA2_HW 0x0068d8e0
br_pixelmap* gLit_op_stat;


// GLOBAL: CARMA2_HW 0x0058fdcc
extern int gCursor_tinted_bottom;
// GLOBAL: CARMA2_HW 0x0058fdd8
extern int gCursor_tinted_center;
// GLOBAL: CARMA2_HW 0x00704e44
float gCamera_to_horiz_angle;

// GLOBAL: CARMA2_HW 0x00762100
float gYon_squared;

// GLOBAL: CARMA2_HW 0x006a27dc
int gShadow_clip_plane_count;

// GLOBAL: CARMA2_HW 0x006a23e0
br_vector3 gShadow_points[8];

// GLOBAL: CARMA2_HW 0x00655e00
br_matrix34 gIdentity34 = {
    {
        { 1.0, 0.0, 0.0 },
        { 0.0, 1.0, 0.0 },
        { 0.0, 0.0, 1.0 },
        { 0.0, 0.0, 0.0 },
    }
};

// GLOBAL: CARMA2_HW 0x006a2440
int gShadow_dim_amount;

// GLOBAL: CARMA2_HW 0x006a22c4
int gNumber_of_lollipops;

// GLOBAL: CARMA2_HW 0x006593a0
const int gArrows[2][4][60] = {
    {
        // inner arrow (=fill)
        { 10,  0,  0, -1,  0,  1,  0,  0, -1,  0, -2,  0,  1, -1,  1,  1,  1, -2,  2,  2,  2, },
        { 11,  0,  0, -1,  0,  1,  0,  0, -1,  1, -1,  1, -2, -2,  1, -1,  1,  0,  1,  1,  1,  1,  2, },
        {  9,  0,  0, -2,  0, -1,  0,  1,  0,  0, -1,  1, -1,  2, -2,  0,  1,  0,  2, },
        { 11,  0,  0, -1,  0,  1,  0, -2, -1, -1, -1,  0, -1,  1, -1,  2, -1, -1,  1,  0,  1, -1,  2, },
    },
    {
        // outer arrow (=border)
        { 26,  1, -3,  1, -2,  1, -1,  2, -1,  2,  0,  2,  1,  3,  1,  3,  2,  3,  3,  2,  3,  1,  3,  1,  2,  0,  2, -1,  2,
          -1,  3, -2,  3, -3,  3, -3,  2, -3,  1, -2,  1, -2,  0, -2, -1, -1, -1, -1, -2, -1, -3,  0, -3, },
        { 22,  0, -3,  1, -3,  2, -3,  2, -2,  2, -1,  2,  0,  2,  1,  2,  2,  2,  3,  1,  3,  0,  3,  0,  2, -1,  2, -2,  2,
          -3,  2, -3,  1, -3,  0, -2,  0, -2, -1, -1, -1, -1, -2,  0, -2, },
        { 24,  1, -3,  2, -3,  3, -3,  3, -2,  3, -1,  2, -1,  2,  0,  2,  1,  1,  1,  1,  2,  1,  3,  0,  3, -1,  3, -1,  2,
          -1,  1, -2,  1, -3,  1, -3,  0, -3, -1, -2, -1, -1, -1, -1, -2,  0, -2,  1, -2, },
        { 22, -3, -2, -2, -2, -1, -2,  0, -2,  1, -2,  2, -2,  3, -2,  3, -1,  3,  0,  2,  0,  2,  1,  1,  1,  1,  2,  0,  2,
           0,  3, -1,  3, -2,  3, -2,  2, -2,  1, -2,  0, -3,  0, -3, -1, },
    },
};

// GLOBAL: CARMA2_HW 0x0068d6a8
br_matrix34 gCar_in_map_space;

// GLOBAL: CARMA2_HW 0x0068c858
int gINT_0068c858;

// GLOBAL: CARMA2_HW 0x0068d890
int gINT_0068d890;

// GLOBAL: CARMA2_HW 0x0068d6e0
float gFLOAT_0068d6e0;

// GLOBAL: CARMA2_HW 0x0074ab90
float gFLOAT_0074ab90;

// GLOBAL: CARMA2_HW 0x0068d6d8
br_vector2 gVector2_0068d6d8;

// GLOBAL: CARMA2_HW 0x006569b0
const int gMini_map_glowing_line_animation_indices[5] = {
    0, 1, 2, 1, 0
};

// GLOBAL: CARMA2_HW 0x0065fb20
br_matrix34 gSheer_mat = {
    { { 1.0, 0.0, 0.0 },
      { 0.0, 1.0, 0.0 },
      { 0.0, 0.0, 1.0 },
      { 0.0, 0.0, 0.0 } }
};

// GLOBAL: CARMA2_HW 0x006a2358
int gAR_fudge_headups;

// GLOBAL: CARMA2_HW 0x00659b20
tOpponent_Status gPrevious_opponent_status = eOpponent_status_Uninitialized;

// GLOBAL: CARMA2_HW 0x0068d8d0
tU32 gTime_oppobar_target_wasted;

// GLOBAL: CARMA2_HW 0x006a234c
float gPrevious_rear_yon;

// FUNCTION: CARMA2_HW 0x004b5460
void C2_HOOK_FASTCALL FadePaletteDown(void) {
}

// FUNCTION: CARMA2_HW 0x004e9960
void C2_HOOK_FASTCALL ToggleShadow(void) {

    gShadow_level += 1;
    if (gShadow_level > eShadow_everyone) {
        gShadow_level = eShadow_none;
    }
    switch (gShadow_level) {
    case eShadow_none:
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(kMiscString_ShadowNone));
        break;
    case eShadow_us_only:
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(kMiscString_ShadowUsOnly));
        break;
    case eShadow_us_and_opponents:
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(kMiscString_ShadowUsAndOpponents));
        break;
    case eShadow_everyone:
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(kMiscString_ShadowEveryone));
        break;
    default:
        return;
    }
}

// FUNCTION: CARMA2_HW 0x004e9b90
void C2_HOOK_FASTCALL ShadowMode(void) {

    gFancy_shadow = !gFancy_shadow;
    if (gFancy_shadow) {
        NewTextHeadupSlot(4, 0, 2000, -4, "Translucent shadow");
    } else {
        NewTextHeadupSlot(4, 0, 2000, -4, "Solid shadow");
    }
}

// FUNCTION: CARMA2_HW 0x004b5320
void C2_HOOK_FASTCALL MungePalette(void) {

}

// FUNCTION: CARMA2_HW 0x005184c0
int C2_HOOK_FASTCALL FindBestMatchingPaletteIndex(br_colour pColour) {
    double min_error = DBL_MAX;
    int i;
    int pal_index;
    br_colour* pal_colours;

    pal_index = 127;
    pal_colours = gRender_palette->pixels;
    for (i = 0; i < 256; i++) {
        int dr, dg, db;
        double error;

        dr = ((pal_colours[i] >> 16) & 0xff) - ((pColour >> 16) & 0xff);
        dg = ((pal_colours[i] >>  8) & 0xff) - ((pColour >>  8) & 0xff);
        db = ((pal_colours[i] >>  0) & 0xff) - ((pColour >>  0) & 0xff);
        error = (double)(dr * dr + dg * dg + db * db);
        if (error < min_error) {
            pal_index = i;
            min_error = error;
        }
    }
    return pal_index;
}

// FUNCTION: CARMA2_HW 0x0047d6a0
void C2_HOOK_FASTCALL DisableLights(void) {
    int i;

    for (i = 0; i < gNumber_of_lights; i++) {
        BrLightDisable(gLight_array[i]);
    }
}

// FUNCTION: CARMA2_HW 0x0047c660
int C2_HOOK_FASTCALL SwitchToLoresMode(void) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x004e4b40
void C2_HOOK_FASTCALL AdjustRenderScreenSize(void) {

    ReinitialiseRenderStuff();
    if (gMap_view == 2) {
        gRender_screen->base_x = gMap_render_x_i;
        gRender_screen->base_y = gMap_render_y_i;
        gRender_screen->width = gMap_render_width_i;
        gRender_screen->height = gMap_render_height_i;
    } else {
        gRender_screen->base_x = gProgram_state.current_render_left;
        gRender_screen->base_y = gProgram_state.current_render_top;
        gRender_screen->height = gProgram_state.current_render_bottom - gProgram_state.current_render_top;
        gRender_screen->width = gProgram_state.current_render_right - gProgram_state.current_render_left;
    }
    if (gRender_screen->row_bytes == gRender_screen->width) {
        gRender_screen->flags |= BR_PMF_ROW_WHOLEPIXELS;
    } else {
        gRender_screen->flags &= ~BR_PMF_ROW_WHOLEPIXELS;
    }
    gRender_screen->origin_x = gRender_screen->width / 2;
    gRender_screen->origin_y = gRender_screen->height / 2;
    gWidth = gRender_screen->width;
    gHeight = gRender_screen->height;
    ReinitialiseForwardCamera();
    SetTintedPolySize(gHud_tinted1,
        gRender_screen->base_x,
        gRender_screen->base_y,
        gRender_screen->width,
        gRender_screen->height);
    SetTintedPolySize(gHud_tinted2,
        gRender_screen->base_x,
        gRender_screen->base_y,
        gRender_screen->width,
        gRender_screen->height);
    SetTintedPolySize(gHud_tinted4,
        gRender_screen->base_x,
        gRender_screen->base_y,
        gRender_screen->width,
        gRender_screen->height);
}

// FUNCTION: CARMA2_HW 0x004948a0
void C2_HOOK_FASTCALL InitTargetLock(void) {

    gTarget_lock_enabled = 0;
}

// FUNCTION: CARMA2_HW 0x004948b0
void C2_HOOK_FASTCALL SetIntegerMapRenders(void) {

    gMap_render_width = 128.f;
    gMap_render_height = 80.f;
    gMap_render_x_i = ((int)gMap_render_x) & ~3;
    gMap_render_y_i = ((int)gMap_render_y) & ~1;
    gMap_render_width_i = (int)gMap_render_width;
    gMap_render_height_i = (int)gMap_render_height;
}

// FUNCTION: CARMA2_HW 0x004b5330
void C2_HOOK_FASTCALL ResetPalette(void) {

    InitPaletteAnimate();
    DRSetPalette(gRender_palette);
}

// FUNCTION: CARMA2_HW 0x004b5770
void C2_HOOK_FASTCALL EnsureRenderPalette(void) {

    if (gPalette_munged) {
        ResetPalette();
        gPalette_munged = 0;
    }
}

void C2_HOOK_FASTCALL CalculateWobblitude(tU32 pThe_time) {
    int i;
    tU32 time_going;
    double angle;
    double mod_angle;
    double cosine_over_angle;

    if (gProgram_state.new_view != eView_undefined) {
        return;
    }
    gScreen_wobble_x = 0;
    gScreen_wobble_y = 0;
    for (i = 0; i < CARPOCALYPSE2_ASIZE(gWobble_array); i++) {
        if (gWobble_array[i].time_started != 0) {
            time_going = pThe_time - gWobble_array[i].time_started;
            if (time_going > 1000) {
                gWobble_array[i].time_started = 0;
            } else {
                mod_angle = fmodf(time_going / gWobble_array[i].period, TAU_F);
                if (mod_angle > 1.5f * PI_F) {
                    cosine_over_angle = gCosine_array[(unsigned int)((TAU_F - mod_angle) / PI_F * 128.f)];
                } else if (mod_angle > PI_F) {
                    cosine_over_angle = -gCosine_array[(unsigned int)((mod_angle - PI_F) / PI_F * 128.f)];
                } else if (mod_angle > .5f * PI_F) {
                    cosine_over_angle = -gCosine_array[(unsigned int)((PI_F - mod_angle) / PI_F * 128.f)];
                } else {
                    cosine_over_angle = gCosine_array[(unsigned int)(mod_angle / PI_F * 128.f)];
                }
                angle = cosine_over_angle / (1.f + time_going * .0035f);
                gScreen_wobble_x += (int)(gWobble_array[i].amplitude_x * angle);
                gScreen_wobble_y += (int)(gWobble_array[i].amplitude_y * angle);
            }
        }
    }
    if (gScreen_wobble_x > gCurrent_graf_data->cock_margin_x) {
        gScreen_wobble_x = gCurrent_graf_data->cock_margin_x;
    } else if (gScreen_wobble_x < -gCurrent_graf_data->cock_margin_x) {
        gScreen_wobble_x = -gCurrent_graf_data->cock_margin_x;
    }
    if (gScreen_wobble_y > gCurrent_graf_data->cock_margin_y) {
        gScreen_wobble_y = gCurrent_graf_data->cock_margin_y;
    } else if (gScreen_wobble_y < -gCurrent_graf_data->cock_margin_y) {
        gScreen_wobble_y = -gCurrent_graf_data->cock_margin_y;
    }
    PipeSingleScreenWobble(gScreen_wobble_x, gScreen_wobble_y);
}

// FUNCTION: CARMA2_HW 0x004e4e40
void C2_HOOK_FASTCALL RenderAFrame(int pDepth_mask_on) {
    int i;
    int x_shift;
    int y_shift;
    int cockpit_on;
    char* old_pixels;
    br_matrix34 old_camera_matrix;
    br_matrix34 old_mirror_cam_matrix;
    tU32 the_time;

    gRender_screen->pixels = gBack_screen->pixels;
    the_time = GetTotalTime();
    old_pixels = gRender_screen->pixels;
    cockpit_on = gProgram_state.cockpit_on && gProgram_state.cockpit_image_index >= 0 && gMap_view != 2;
    gMirror_on__graphics = gProgram_state.mirror_on && cockpit_on && gProgram_state.which_view == eView_forward;

    StartRenderingHeadups();
    MapStuffBeforeRender();
    if (!gAction_replay_mode) {
        CalculateWobblitude(the_time);
    }
    if (cockpit_on) {
        if (-gScreen_wobble_x > gX_offset) {
            x_shift = -gX_offset;
        } else if (gScreen_wobble_x + gX_offset + gRender_screen->width > gBack_screen->width) {
            x_shift = gBack_screen->width - gRender_screen->width - gX_offset;
        } else {
            x_shift = gScreen_wobble_x;
        }
        if (-gScreen_wobble_y > gY_offset) {
            y_shift = -gY_offset;
        } else if (gScreen_wobble_y + gY_offset + gRender_screen->height > gBack_screen->height) {
            y_shift = gBack_screen->height - gRender_screen->height - gY_offset;
        } else {
            y_shift = gScreen_wobble_y;
        }
    } else {
        x_shift = 0;
        y_shift = 0;
    }
    BrMatrix34Copy(&old_camera_matrix, &gCamera->t.t.mat);
    if (gMirror_on__graphics) {
        BrMatrix34Copy(&old_mirror_cam_matrix, &gRearview_camera->t.t.mat);
    }
    if (cockpit_on) {
        gSheer_mat.m[2][1] = y_shift / (float)gRender_screen->height;
        gSheer_mat.m[2][0] = -x_shift / (float)gRender_screen->width;
        BrMatrix34Pre(&gCamera->t.t.mat, &gSheer_mat);
        gCamera->t.t.translate.t.v[0] -= gScreen_wobble_x * 1.5f / gRender_screen->width / WORLD_SCALE;
        gCamera->t.t.translate.t.v[1] += gScreen_wobble_y * 1.5f / gRender_screen->width / WORLD_SCALE;
    }
    gRender_screen->pixels = (char*)gRender_screen->pixels + x_shift + y_shift * gRender_screen->row_bytes;
    if (gRender_indent && gMap_view != 2) {
        BrPixelmapRectangleFill(
                gBack_screen,
                0,
                0,
                gGraf_specs[gGraf_spec_index].total_width,
                gProgram_state.current_render_top,
                0);
        BrPixelmapRectangleFill(
                gBack_screen,
                0,
                gProgram_state.current_render_bottom,
                gGraf_specs[gGraf_spec_index].total_width,
                gGraf_specs[gGraf_spec_index].total_height - gProgram_state.current_render_bottom,
                0);
        BrPixelmapRectangleFill(
                gBack_screen,
                0,
                gProgram_state.current_render_top,
                gProgram_state.current_render_left,
                gProgram_state.current_render_bottom - gProgram_state.current_render_top,
                0);
        BrPixelmapRectangleFill(
                gBack_screen,
                gProgram_state.current_render_right,
                gProgram_state.current_render_top,
                gGraf_specs[gGraf_spec_index].total_width - gProgram_state.current_render_right,
                gProgram_state.current_render_bottom - gProgram_state.current_render_top,
                0);
    }
    gRendering_mirror = 0;
    for (i = 0; i < 1; i++) {
        DoACompleteRenderPass(0, &gCamera_to_world, gCamera, gRender_screen, gDepth_buffer);
    }
    gCamera->t.t.mat = old_camera_matrix;
    if (gMirror_on__graphics) {
        DoACompleteRenderPass(1, &gRearview_camera_to_world, gRearview_camera, gRearview_screen, gRearview_depth_buffer);
        gRearview_camera->t.t.mat = old_mirror_cam_matrix;
    }
    RenderTintedPolys();
    if (gMap_view != 2) {
        DimAFewBits();
        DoDamageScreen(the_time);
        if (!gAction_replay_mode || gAR_fudge_headups) {
            DoHeadups(the_time);
        }
        DoInstruments(the_time);
        if (!gAction_replay_mode || gAR_fudge_headups) {
            DrawPowerups(the_time);
        }
    }
    if (gMap_view != 2 && (!gAction_replay_mode || gAR_fudge_headups)) {
        DoTestHeadup();
        DoPSPowerupHeadups();
    }
    MapStuffAfterRender();
    if (gNet_mode != eNet_mode_none) {
        DisplayUserMessage();
    }
    if (gAction_replay_mode) {
        MovieRecordFrame(gBack_screen, gFrame_period);
    }
    if (gAction_replay_mode && !gAR_fudge_headups) {
        DoActionReplayHeadups();
    }
    StopRenderingHeadups();
    if (gAction_replay_mode) {
        SynchronizeActionReplay();
    } else {
        PipeSingleFrameFinish();
    }
    gRender_screen->pixels = old_pixels;
    if (!gPalette_fade_time || GetRaceTime() > gPalette_fade_time + 500u) {
        PDScreenBufferSwap(0);
    }
    if (gAction_replay_mode) {
        DoActionReplayPostSwap();
    }
}

// FUNCTION: CARMA2_HW 0x004e4900
void C2_HOOK_FASTCALL ResetLollipopQueue(void) {

    gNumber_of_lollipops = 0;
}

// FUNCTION: CARMA2_HW 0x004b52b0
void C2_HOOK_FASTCALL RevertPalette(void) {

    memcpy(gRender_palette->pixels, gOrig_render_palette->pixels, 256 * sizeof(br_colour));
    DRSetPalette3(gRender_palette, 1);
}

// FUNCTION: CARMA2_HW 0x004e9680
void C2_HOOK_FASTCALL MungeClipPlane(br_vector3* pLight, tCar_spec* pCar, br_vector3* p1, br_vector3* p2, br_vector3* pOffset) {
    br_vector3 v1;
    br_vector3 v2;
    br_vector3 v3;
    br_vector3 v4;
    br_scalar length;
    br_actor* new_clip;

    BrMatrix34ApplyP(&v1, p1, &pCar->car_master_actor->t.t.mat);
    BrMatrix34ApplyP(&v2, p2, &pCar->car_master_actor->t.t.mat);
    BrVector3Sub(&v3, p2, p1);
    BrVector3Cross(&v4, &v3, pLight);
    if (fabsf(v4.v[0]) >= 0.01f || fabsf(v4.v[1]) >= 0.01f || fabsf(v4.v[2]) >= 0.01f) {
        BrVector3Sub(&v3, p1, pOffset);
        if (BrVector3Dot(&v3, &v4) > 0.f) {
            BrVector3Negate(&v4, &v4);
        }
        BrVector3Normalise(&v3, &v4);
        BrMatrix34ApplyV(&v4, &v3, &pCar->car_master_actor->t.t.mat);

        new_clip = gShadow_clip_planes[gShadow_clip_plane_count].clip;
        ((br_vector4*)new_clip->type_data)->v[0] = v4.v[0];
        ((br_vector4*)new_clip->type_data)->v[1] = v4.v[1];
        ((br_vector4*)new_clip->type_data)->v[2] = v4.v[2];
        ((br_vector4*)new_clip->type_data)->v[3] = -BrVector3Dot(&v1, &v4);
        length = CARPOCALYPSE2_SQR(v1.v[2] - v2.v[2]) + CARPOCALYPSE2_SQR(v1.v[0] - v2.v[0]);
        gShadow_clip_planes[gShadow_clip_plane_count].length = length;
        gShadow_clip_plane_count += 1;
    }
}

// FUNCTION: CARMA2_HW 0x0047b910
br_scalar C2_HOOK_FASTCALL DistanceFromPlane(br_vector3* pPos, br_scalar arg2, br_scalar pA, br_scalar pB, br_scalar pC, br_scalar pD) {

    return fabsf((pPos->v[0] * pA + pPos->v[1] * pB + pPos->v[2] * pC + pD) / (pA * pA + pB * pB + pC * pC));
}

void C2_HOOK_FASTCALL TryThisEdge(tCar_spec* pCar, br_vector3* pLight, int pIndex_1, br_scalar pSign_1, int pIndex_2, br_scalar pSign_2, int pPoint_index_1, int pPoint_index_2, br_vector3* pOffset) {
    br_scalar dot_1;
    br_scalar dot_2;
    br_scalar mult;

    dot_1 = pSign_1 * pLight->v[pIndex_1];
    dot_2 = pSign_2 * pLight->v[pIndex_2];
    mult = dot_1 * dot_2;
    if (mult < 0 || (mult == 0 && (dot_1 > 0 || dot_2 > 0))) {
        if (gShadow_clip_plane_count < 6) {
            MungeClipPlane(pLight, pCar, &gShadow_points[pPoint_index_1], &gShadow_points[pPoint_index_2], pOffset);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004e7650
void C2_HOOK_FASTCALL ProcessShadow(tCar_spec* pCar, br_actor* pWorld, tTrack_spec* pTrack_spec, br_actor* pCamera, br_matrix34* pCamera_to_world_transform, br_scalar pDistance_factor) {
    int i;
    int face_count;
    int oily_count;
    int f_num;
    br_vector3 pos;
    br_vector3 light_ray_car;
    br_vector3 shadow_points_world[8];
    br_vector3 poly_centre;
    br_vector3 car_to_poly;
    br_vector3 ray;
    br_vector3 ray_pos;
    br_vector3 normal;
    br_vector4* clip_normal;
    br_bounds3 bounds;
    br_vector3 center;
    br_scalar oily_size;
    br_scalar highest_underneath;
    br_scalar shadow_scaling_factor;
    br_scalar first_poly_below;
    br_scalar distance;
    br_scalar ray_length;
    tBounds kev_bounds;
    tFace_ref the_list[100];
    tFace_ref* face_ref;
    tFace_ref* list_ptr;
    br_actor* oily_actor;
    br_material* material;
    br_vertex verts[48];
    br_face faces[16];
    tUser_crush_data* user_crush_data;
    br_model* car_crush_model;

#if defined(CARPOCALYPSE2_FIX_BUGS)
    ray_length = 0.f;
#endif

    f_num = 0;
    user_crush_data = pCar->car_model_actor->user;
    car_crush_model = user_crush_data->models[0];
    BrVector3Copy(&bounds.min, &car_crush_model->bounds.min);
    BrVector3Copy(&bounds.max, &car_crush_model->bounds.max);
    BrVector3Set(&gShadow_points[0], bounds.max.v[0], bounds.max.v[1], bounds.max.v[2]);
    BrVector3Set(&gShadow_points[1], bounds.max.v[0], bounds.max.v[1], bounds.min.v[2]);
    BrVector3Set(&gShadow_points[2], bounds.min.v[0], bounds.max.v[1], bounds.min.v[2]);
    BrVector3Set(&gShadow_points[3], bounds.min.v[0], bounds.max.v[1], bounds.max.v[2]);
    BrVector3Set(&gShadow_points[4], bounds.max.v[0], bounds.min.v[1], bounds.max.v[2]);
    BrVector3Set(&gShadow_points[5], bounds.max.v[0], bounds.min.v[1], bounds.min.v[2]);
    BrVector3Set(&gShadow_points[6], bounds.min.v[0], bounds.min.v[1], bounds.min.v[2]);
    BrVector3Set(&gShadow_points[7], bounds.min.v[0], bounds.min.v[1], bounds.max.v[2]);
    gShadow_clip_plane_count = 0;
    BrMatrix34TApplyV(&light_ray_car, &gShadow_light_ray, &pCar->car_master_actor->t.t.mat);
    Vector3Average(&center, &bounds.max, &bounds.min);
    TryThisEdge(pCar, &light_ray_car, 2,  1.f, 1,  1.f, 0, 3, &center);
    TryThisEdge(pCar, &light_ray_car, 2, -1.f, 1,  1.f, 1, 2, &center);
    TryThisEdge(pCar, &light_ray_car, 2, -1.f, 1, -1.f, 6, 5, &center);
    TryThisEdge(pCar, &light_ray_car, 2,  1.f, 1, -1.f, 7, 4, &center);
    TryThisEdge(pCar, &light_ray_car, 0,  1.f, 1,  1.f, 1, 0, &center);
    TryThisEdge(pCar, &light_ray_car, 0, -1.f, 1,  1.f, 2, 3, &center);
    TryThisEdge(pCar, &light_ray_car, 0, -1.f, 1, -1.f, 7, 6, &center);
    TryThisEdge(pCar, &light_ray_car, 0,  1.f, 1, -1.f, 4, 5, &center);
    TryThisEdge(pCar, &light_ray_car, 0,  1.f, 2,  1.f, 4, 0, &center);
    TryThisEdge(pCar, &light_ray_car, 0, -1.f, 2,  1.f, 3, 7, &center);
    TryThisEdge(pCar, &light_ray_car, 0, -1.f, 2, -1.f, 2, 6, &center);
    TryThisEdge(pCar, &light_ray_car, 0,  1.f, 2, -1.f, 5, 1, &center);

    for (i = 0; i < gShadow_clip_plane_count; ++i) {
        BrClipPlaneEnable(gShadow_clip_planes[i].clip);
    }

    face_count = 0;
    if (!gAction_replay_mode && pCar->number_of_wheels_on_ground > 2) {
        face_count = GetPrecalculatedFacesUnderCar(pCar, &face_ref);
    }
    if (face_count != 0) {
        highest_underneath = 0.0;
    } else {
        BrVector3Set(&kev_bounds.original_bounds.min,  1000.f,  1000.f,  1000.f);
        BrVector3Set(&kev_bounds.original_bounds.max, -1000.f, -1000.f, -1000.f);
        for (i = 0; i < CARPOCALYPSE2_ASIZE(shadow_points_world); i++) {
            BrMatrix34ApplyP(&shadow_points_world[i], &gShadow_points[i], &pCar->car_master_actor->t.t.mat);
            if (shadow_points_world[i].v[0] < kev_bounds.original_bounds.min.v[0]) {
                kev_bounds.original_bounds.min.v[0] = shadow_points_world[i].v[0];
            } else if (shadow_points_world[i].v[0] > kev_bounds.original_bounds.max.v[0]) {
                kev_bounds.original_bounds.max.v[0] = shadow_points_world[i].v[0];
            }
            if (shadow_points_world[i].v[1] < kev_bounds.original_bounds.min.v[1]) {
                kev_bounds.original_bounds.min.v[1] = shadow_points_world[i].v[1];
            } else if (shadow_points_world[i].v[1] > kev_bounds.original_bounds.max.v[1]) {
                kev_bounds.original_bounds.max.v[1] = shadow_points_world[i].v[1];
            }
            if (shadow_points_world[i].v[2] < kev_bounds.original_bounds.min.v[2]) {
                kev_bounds.original_bounds.min.v[2] = shadow_points_world[i].v[2];
            } else if (shadow_points_world[i].v[2] > kev_bounds.original_bounds.max.v[2]) {
                kev_bounds.original_bounds.max.v[2] = shadow_points_world[i].v[2];
            }
        }
        kev_bounds.original_bounds.min.v[1] -= 4.4f;
        kev_bounds.mat = &gIdentity34;
        face_count = FindFacesInBox(&kev_bounds, the_list, CARPOCALYPSE2_ASIZE(the_list), NULL);
        face_ref = the_list;
        highest_underneath = 1000.f;
        ray_length = kev_bounds.original_bounds.max.v[1] - kev_bounds.original_bounds.min.v[1];
        BrVector3Set(&ray, 0.f, -ray_length, 0.f);
        BrVector3Copy(&ray_pos, &pCar->car_master_actor->t.t.translate.t);
        ray_pos.v[1] = kev_bounds.original_bounds.max.v[1];
    }
    if (face_count != 0) {
        first_poly_below = -1000.0;
        for (i = 0; i < face_count; i++) {
            list_ptr = &face_ref[i];

            if (list_ptr->normal.v[1] >= -0.1 || (list_ptr->material != NULL && (list_ptr->material->flags & BR_MATF_TWO_SIDED))) {
                if (list_ptr->normal.v[1] < 0.0
                        || (list_ptr->material != NULL && list_ptr->material->identifier != NULL && (list_ptr->material->identifier[0] == '!' || list_ptr->material->identifier[0] == '>'))
                        || ((list_ptr->v[0].v[1] > pCar->pos.v[1] || list_ptr->v[1].v[1] > pCar->pos.v[1] || list_ptr->v[2].v[1] > pCar->pos.v[1]) && list_ptr->normal.v[1] < 0.1f)) {
                    list_ptr->d += SHADOW_D_IGNORE_FLAG;
                }
            } else {
                BrVector3Add(&poly_centre, &list_ptr->v[0], &list_ptr->v[1]);
                BrVector3Add(&poly_centre, &poly_centre, &list_ptr->v[2]);
                BrVector3InvScale(&poly_centre, &poly_centre, 3.f);
                if (poly_centre.v[1] > first_poly_below) {
                    BrVector3Sub(&car_to_poly, &poly_centre, &pCar->car_master_actor->t.t.translate.t);

                    if (BrVector3Dot(&list_ptr->normal, &car_to_poly) > 0.0) {
                        first_poly_below = poly_centre.v[1];
                    }
                }
                list_ptr->d += SHADOW_D_IGNORE_FLAG;
            }
        }
        for (i = 0; i < face_count; i++) {
            list_ptr = &face_ref[i];

            if (list_ptr->d < SHADOW_D_IGNORE_FLAG / 2.f) {
                if (list_ptr->v[0].v[1] >= first_poly_below || list_ptr->v[1].v[1] >= first_poly_below || list_ptr->v[2].v[1] >= first_poly_below) {
                    if (gFancy_shadow) {
                        faces[f_num].material = list_ptr->material;
                        if (gShadow_workaround) {
                            list_ptr->material->ka = 0.75f * gLighting_data.ambient_else;
                            BrMaterialUpdate(list_ptr->material, BR_MATU_LIGHTING);
                        } else if (list_ptr->material != NULL && list_ptr->material->colour_map != NULL && (!(list_ptr->material->flags & BR_MATF_LIGHT))) {
                            list_ptr->material->flags |= BR_MATF_LIGHT | BR_MATF_SMOOTH;
                            BrMaterialUpdate(list_ptr->material, BR_MATU_RENDERING);
                        }
                    } else {
                        faces[f_num].material = gShadow_material;
                    }

                    BrVector3Copy(&verts[3 * f_num + 0].p, &list_ptr->v[0]);
                    BrVector2Copy(&verts[3 * f_num + 0].map, list_ptr->map[0]);
                    BrVector3Copy(&verts[3 * f_num + 1].p, &list_ptr->v[1]);
                    BrVector2Copy(&verts[3 * f_num + 1].map, list_ptr->map[1]);
                    BrVector3Copy(&verts[3 * f_num + 2].p, &list_ptr->v[2]);
                    BrVector2Copy(&verts[3 * f_num + 2].map, list_ptr->map[2]);
                    faces[f_num].vertices[0] = 3 * f_num + 0;
                    faces[f_num].vertices[1] = 3 * f_num + 1;
                    faces[f_num].vertices[2] = 3 * f_num + 2;
                    f_num += 1;
                    if (highest_underneath > 0.0) {
                        CheckSingleFace(list_ptr, &ray_pos, &ray, &normal, &distance, &pos);
                        if (distance < 1.f && ray_length * distance < highest_underneath) {
                            highest_underneath = ray_length * distance;
                        }
                    }
                    if (f_num >= CARPOCALYPSE2_ASIZE(faces)) {
                        break;
                    }
                }
            } else {
                list_ptr->d -= SHADOW_D_IGNORE_FLAG;
            }
        }
        highest_underneath -= (bounds.max.v[1] - bounds.min.v[1]);
        if (highest_underneath > 2.2f) {
            highest_underneath = 2.2f;
        } else if (highest_underneath < 0.f) {
            highest_underneath = 0.0;
        }
        if (gFancy_shadow) {
            gShadow_dim_amount = (int)(2.5f + (2.2f - highest_underneath) * 5.0f / 2.2f);
            for (i = 0; i < gSaved_table_count; i++) {
                gSaved_shade_tables[i].original->height = 1;
                gSaved_shade_tables[i].original->pixels = (tU8*)gDepth_shade_table->pixels + gShadow_dim_amount * gDepth_shade_table->row_bytes;
                BrTableUpdate(gSaved_shade_tables[i].original, BR_TABU_ALL);
            }
        }
        shadow_scaling_factor = 0.4f + (2.2f - highest_underneath) * 0.52f / 2.2f;
        for (i = 0; i < gShadow_clip_plane_count; i++) {
            clip_normal = gShadow_clip_planes[i].clip->type_data;
            distance = DistanceFromPlane(&pCar->car_master_actor->t.t.translate.t, clip_normal->v[2], clip_normal->v[0], clip_normal->v[1], clip_normal->v[2], clip_normal->v[3]);
            BrVector3Set(&gShadow_clip_planes[i].clip->t.t.translate.t,
                (1.f - shadow_scaling_factor) * distance * clip_normal->v[0],
                (1.f - shadow_scaling_factor) * distance * clip_normal->v[1],
                (1.f - shadow_scaling_factor) * distance * clip_normal->v[2]);
        }
        if (f_num != 0) {
            DisableLights();
            BrZbsSceneRenderBegin(gUniverse_actor, gCamera, gRender_screen, gDepth_buffer);
            EnableLights();
            BrSetScreenZOffset(2);
            gShadow_model->vertices = verts;
            gShadow_model->faces = faces;
            gShadow_model->nfaces = f_num;
            gShadow_model->nvertices = 3 * f_num;
            gShadow_actor->render_style = BR_RSTYLE_FACES;
            BrModelAdd(gShadow_model);
            BrZbsSceneRenderAdd(gShadow_actor);
            BrModelRemove(gShadow_model);
            if (pCar->shadow_intersection_flags != 0) {
                oily_count = GetOilSpillCount();
                for (i = 0; i < oily_count; ++i) {
                    if (pCar->shadow_intersection_flags & (1 << i)) {
                        GetOilSpillDetails(i, &oily_actor, &oily_size);
                        if (oily_actor) {
                            MungeIndexedOilsHeightAboveGround(i);
                            BrZbSceneRenderAdd(oily_actor);
                        }
                    }
                }
            }
            BrSetScreenZOffset(0);
            BrZbSceneRenderEnd();

            for (i = 0; i < f_num; i++) {
                if (gFancy_shadow) {
                    material = gShadow_model->faces[i].material;
                    if (gShadow_workaround) {
                        SmoothificateWorldMaterial(material);
                    } else {
                        if (material != NULL && material->colour_map != NULL && (material->flags & BR_MATF_LIGHT)) {
                            material->flags &= ~(BR_MATF_LIGHT | BR_MATF_PRELIT | BR_MATF_SMOOTH);
                            BrMaterialUpdate(material, BR_MATU_RENDERING);
                        }
                    }
                }
            }
        }
    }
    gShadow_actor->render_style = BR_RSTYLE_NONE;
    for (i = 0; i < gShadow_clip_plane_count; i++) {
        BrClipPlaneDisable(gShadow_clip_planes[i].clip);
    }
}

void C2_HOOK_FASTCALL InitNearestCar(void) {

    gINT_0068d6f4 = -1;
    if (gNet_mode != eNet_mode_none && gCurrent_net_game->type == eNet_game_type_foxy) {
        gTarget_lock_car_2 = NULL;
    } else if (gPrevious_opponent_status == eOpponent_status_Wasted && gTarget_lock_car_1 != NULL) {
        if (PDGetTotalTime() - gTime_oppobar_target_wasted > 2500) {
            gTarget_lock_car_2 = NULL;
            gTarget_lock_car_1 = NULL;
            if (gTarget_lock_enabled) {
                gTarget_lock_enabled = 0;
            }
        } else {
            gFLOAT_0074ab90 = 0.f;
        }
    } else if (gTarget_lock_enabled) {
        gFLOAT_0074ab90 = 0.f;
    } else {
        gTarget_lock_car_2 = NULL;
    }
}

void C2_HOOK_FASTCALL StartMap(void) {

    gBack_original_origin_x = gBack_screen->origin_x;
    gBack_original_origin_y = gBack_screen->origin_y;
    gBack_original_base_x = gBack_screen->base_x;
    gBack_original_base_y = gBack_screen->base_y;
    gBack_screen->origin_x = 0;
    gBack_screen->origin_y = 0;
    gBack_screen->base_x = 0;
    gBack_screen->base_y = 0;
}

void C2_HOOK_FASTCALL CopyMapToScreen(void) {

    if (gCurrent_race.map_image != NULL) {
        if (gGraf_data_index == 0) {
            DRPixelmapCopy(gBack_screen, gCurrent_race.map_image);
        } else {
            PossibleUnlock(1);
            DRPixelmapRectangleCopy(gBack_screen,
                -gBack_screen->origin_x,
                -gBack_screen->origin_y,
                gCurrent_race.map_image,
                gCurrent_race.map_image->origin_x,
                gCurrent_race.map_image->origin_y,
                640, 480);
        }
    }
    DimRectangleClipped(gBack_screen,
        gMap_render_x_i - gCurrent_graf_data->map_render_x_marg,
        gMap_render_y_i - gCurrent_graf_data->map_render_y_marg,
        gMap_render_width_i + gMap_render_x_i + gCurrent_graf_data->map_render_x_marg,
        gMap_render_height_i + gMap_render_y_i + gCurrent_graf_data->map_render_y_marg,
        1);
}

// FUNCTION: CARMA2_HW 0x00496be0
void C2_HOOK_FASTCALL MapStuffBeforeRender(void) {
    InitNearestCar();
    if (gCurrent_race.map_image != NULL && gMap_view == 2) {
        StartMap();
        CopyMapToScreen();
    }
}

void C2_HOOK_FASTCALL FoxyStuff(br_matrix34* pMat34, br_actor* pCamera, br_pixelmap* pColour, br_pixelmap* pDepth) {
    int count;
    int i;
    int j;
    tCar_spec* fox_car;

    count = 0;
    if (gNet_mode != eNet_mode_none && (gCurrent_net_game->type == eNet_game_type_6 || gCurrent_net_game->type == eNet_game_type_foxy)) {
        BrZbsSceneRenderBegin(gUniverse_actor, pCamera, pColour, pDepth);
        for (i = 0; i < gNumber_of_net_players; i++) {
            tNet_game_player_info* net_player = &gNet_players[i];
            if ((net_player->field_0x80 || i == gIt_or_fox) && !IsCarCloaked(net_player->car)) {
                fox_car = net_player->car;

                if (fox_car->car_master_actor->render_style != BR_RSTYLE_NONE) {
                    br_actor **wheel_actors;

                    count += 1;
                    wheel_actors = fox_car->wheel_actors;
                    for (j = 0; j < CARPOCALYPSE2_ASIZE(fox_car->wheel_actors); j++) {
                        if (wheel_actors[j] != NULL) {
                            wheel_actors[j]->render_style = BR_RSTYLE_NONE;
                        }
                    }
                    BrZbsSceneRenderAdd(fox_car->car_master_actor);
                }
            }
        }
        BrZbsSceneRenderEnd();
        if (count != 0) {
            if (gHud_tinted4 == -1) {
                gHud_tinted4 = CreateTintedPoly(0, 0, 640, 480, 3, 2, 0, 0);
            }
            TurnTintedPolyOn(gHud_tinted4);
            ProcessTintedPoly(gHud_tinted4);
            RenderTintedPolys();
            TurnTintedPolyOff(gHud_tinted4);
            BrZbsSceneRenderBegin(gUniverse_actor, pCamera, pColour, pDepth);
            for (i = 0; i < gNumber_of_net_players; i++) {
                tNet_game_player_info* net_player = &gNet_players[i];
                if ((net_player->field_0x80 || i == gIt_or_fox)) {
                    fox_car = net_player->car;

                    if (fox_car->car_master_actor->render_style != BR_RSTYLE_NONE) {
                        br_actor **wheel_actors;

                        wheel_actors = fox_car->wheel_actors;
                        for (j = 0; j < CARPOCALYPSE2_ASIZE(fox_car->wheel_actors); j++) {
                            if (wheel_actors[j] != NULL) {
                                wheel_actors[j]->render_style = BR_RSTYLE_DEFAULT;
                                BrZbsSceneRenderAdd(wheel_actors[j]);
                            }
                        }
                    }
                }
            }
            BrZbsSceneRenderEnd();
            fox_car->car_master_actor->render_style = BR_RSTYLE_NONE;
        }
    }
}

int C2_HOOK_FASTCALL ConditionallyFillWithSky(br_pixelmap* pPixelmap) {
    int bgnd_col;

    if (gProgram_state.current_depth_effect.sky_texture != NULL && ((gLast_camera_special_volume == NULL) || gLast_camera_special_volume->sky_col <= -1)) {
        return 0;
    }
    if (gProgram_state.current_depth_effect.type == eDepth_effect_fog || gSwap_depth_effect_type == eDepth_effect_fog) {
        bgnd_col = 0xff;
    } else if (gProgram_state.current_depth_effect.type == eDepth_effect_darkness || gSwap_depth_effect_type == eDepth_effect_darkness) {
        bgnd_col = 0x00;
    } else if (gLast_camera_special_volume != NULL && gLast_camera_special_volume->sky_col >= 0) {
        bgnd_col = gLast_camera_special_volume->sky_col;
    } else {
        bgnd_col = 0x00;
    }

    if (gNet_mode != eNet_mode_none && (gCurrent_net_game->type == eNet_game_type_6 || gCurrent_net_game->type == eNet_game_type_foxy)) {
        SetSkyColour(((tU32*)gRender_palette->pixels)[bgnd_col]);
        return 0;
    } else {

        if (pPixelmap->type != BR_PMT_INDEX_8) {
            bgnd_col = PaletteEntry16Bit(gRender_palette, bgnd_col);
            bgnd_col = (bgnd_col << 16) | bgnd_col;
        }
        BrPixelmapFill(pPixelmap, bgnd_col);
        return 1;
    }
}

void C2_HOOK_FASTCALL ProcessNonTrackActors(br_pixelmap* pRender_buffer, br_pixelmap* pDepth_buffer, br_actor* pCamera, br_matrix34* pCamera_to_world) {

    BrZbSceneRenderAdd(gNon_track_actor);
}

// FUNCTION: CARMA2_HW 0x004e5680
void C2_HOOK_FASTCALL DoARenderPass(br_matrix34* pMat34, br_actor* pCamera, br_pixelmap* pColour, br_pixelmap* pDepth, float pYon_factor, int pShadows, int pEffects) {
    br_camera* camera_data = (br_camera*)pCamera->type_data;
    br_scalar original_yon = camera_data->yon_z;
    int i;

    camera_data->yon_z *= pYon_factor;
    BrPixelmapFill(pDepth, 0xffffffff);
    if (!gAFE) {
        FoxyStuff(pMat34, pCamera, pColour, pDepth);
    }
    if (!ConditionallyFillWithSky(pColour->width == gBack_screen->width ? gBack_screen : pColour)) {
        BrZbsSceneRenderBegin(gUniverse_actor, pCamera, pColour, pDepth);
        DepthEffectSky(pColour, pDepth, pCamera, pMat34);
        BrZbsSceneRenderEnd();
    }
    DoSpecialCameraEffect(pCamera, pMat34);
    if (pShadows) {
        RenderShadows(gUniverse_actor, &gProgram_state.track_spec, pCamera, pMat34);
    }
    FixificateClipulatingPlaneyThings();
    BrZbsSceneRenderBegin(gUniverse_actor, pCamera, pColour, pDepth);
    for (i = CARPOCALYPSE2_ASIZE(gOther_selfs); i > 0; i--) {
        if (i != 2) {
            BrSetScreenZOffset(i);
            BrZbsSceneRenderAdd(gOther_selfs[i - 1]);
        }
    }
    BrSetScreenZOffset(0);
    ProcessTrack(gUniverse_actor, &gProgram_state.track_spec, pCamera, pMat34);
    ProcessNonTrackActors(pColour, pDepth, pCamera, pMat34);
    RenderLimbs();
    RenderLollipops(pColour, pDepth, pCamera, pMat34);
    /* FIXME: DepthEffect(pColour, pDepth) in software render mode? */
    if (pEffects) {
        RenderSparks(pColour, pDepth, pCamera, pMat34, gFrame_period);
        RenderSmoke(pColour, pDepth, pCamera, pMat34, gFrame_period);
        RenderElectroBastardRays(pColour, pDepth, pCamera, pMat34, gFrame_period);
    }
    BrZbsSceneRenderEnd();
    CancelificateClipulatingPlaneyThings();
    camera_data->yon_z = original_yon;
}

// FUNCTION: CARMA2_HW 0x004e54f0
void C2_HOOK_FASTCALL DoACompleteRenderPass(int pMirror, br_matrix34* pCamera_to_world, br_actor* pCamera, br_pixelmap* pScreen, br_pixelmap* pDepth) {
    int i;
    br_matrix34 mat;

    gRendering_mirror = pMirror;
    if (pMirror && gPrevious_rear_yon != ((br_camera*)pCamera->type_data)->yon_z) {
        gPrevious_rear_yon = ((br_camera*)pCamera->type_data)->yon_z;
        MungeRearviewSky();
    }
    if (!gRendering_mirror) {
        gRear_pixelmap->base_x = 0;
        gRear_pixelmap->base_y = 0;
        for (i = 0; i < gCount_extra_renders; i++) {
            tExtra_render *extra_render = &gExtra_renders[i];

            gRear_pixelmap->origin_x = extra_render->material->colour_map->width / 2;
            gRear_pixelmap->origin_y = extra_render->material->colour_map->height / 2;
            gRear_pixelmap->width = extra_render->material->colour_map->width;
            gRear_pixelmap->height = extra_render->material->colour_map->width;
            if (gRear_pixelmap->width == gRear_pixelmap->row_bytes) {
                gRear_pixelmap->flags |= BR_PMF_ROW_WHOLEPIXELS;
            } else {
                gRear_pixelmap->flags &= ~BR_PMF_ROW_WHOLEPIXELS;
            }
            BrActorToActorMatrix34(&mat, extra_render->actor, gUniverse_actor);
            DoARenderPass(&mat, extra_render->actor, gRear_pixelmap, gDepth_buffer, 1.f, 0, 0);
            DRPixelmapCopyMapBlack(extra_render->material->colour_map, gRear_pixelmap);
            BrMaterialUpdate(extra_render->material, BR_MATU_COLOURMAP);
        }
    }
    DoARenderPass(pCamera_to_world, pCamera, pScreen, pDepth, 1.f, !pMirror, !pMirror);
    gRendering_mirror = 0;
}

// FUNCTION: CARMA2_HW 0x00495ba0
void C2_HOOK_FASTCALL DoSomeThingsToCheckpoints(br_pixelmap* pMap, tU32 pTime, tDoSomethingsToCheckpoints_cbfn* pCallback, int pDraw_prev) {
    int i;
    int checkpoint;
    int checkpoints[10];
    int checkpoint_flags;

    for (i = 0; i < gCurrent_race.check_point_count; i++) {
        checkpoints[i] = 0;
    }
    if (gNet_mode == eNet_mode_none) {
        pCallback(pMap, gCheckpoint - 1, pTime, 1);
        checkpoints[gCheckpoint - 1] = 1;
    } else {
        if (gCurrent_net_game->type == eNet_game_type_checkpoint) {
            checkpoint_flags = gNet_players[gThis_net_player_index].score2;
            for (i = 0; i < gCurrent_race.check_point_count; i++, checkpoint_flags >>= 1) {
                if (checkpoint_flags & 0x1) {
                    pCallback(pMap, i, pTime, 1);
                    checkpoints[i] = 1;
                }
            }
        } else if (gCurrent_net_game->type == eNet_game_type_5 || gCurrent_net_game->type == eNet_game_type_4) {
            if (gNet_players[gThis_net_player_index].score2 >= 0) {
                checkpoint = gNet_players[gThis_net_player_index].score2 % gCurrent_race.check_point_count;
                pCallback(pMap, checkpoint, pTime, 1);
                checkpoints[checkpoint] = 1;
            }
        }
    }
    if (pDraw_prev) {
        if (gNet_mode == eNet_mode_none
                || gCurrent_net_game->type == eNet_game_type_checkpoint
                || gCurrent_net_game->type == eNet_game_type_4
                || gCurrent_net_game->type == eNet_game_type_5) {

            for (i = 0; i < gCurrent_race.check_point_count; i++) {

                if (!checkpoints[i]) {
                    pCallback(pMap, i, pTime, 0);
                }
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x004968f0
void C2_HOOK_FASTCALL CalcMapCheckpoint(br_pixelmap* pMap, int pCheckpoint, tU32 pTime, int pTarget) {
    br_vector3 p;

    BrMatrix34ApplyP(&p, &gCurrent_race.checkpoints[pCheckpoint].pos, &gCurrent_race.map_transformation);
    BrVector2Copy(&gCurrent_race.checkpoints[pCheckpoint].map_position, &p);
}

// FUNCTION: CARMA2_HW 0x004969e0
void C2_HOOK_FASTCALL DrawCheckpoint(br_pixelmap* pMap, int pCheckpoint, tU32 pTime, int pTarget) {
    // GLOBAL: CARMA2_HW 0x0068d8e8
    static tU32 last_flash;
    // GLOBAL: CARMA2_HW 0x0068d8ec
    static int flash_rate;

    if ((pCheckpoint >= 0 && pCheckpoint < gCurrent_race.check_point_count
            && gTrack_version >= 1 && pTarget == 0) || Flash(300, &last_flash, &flash_rate)) {

        if (gCurrent_race.checkpoints[pCheckpoint].map_position.v[0] >= 0.f
                && gCurrent_race.checkpoints[pCheckpoint].map_position.v[0] < (float)pMap->width
                && gCurrent_race.checkpoints[pCheckpoint].map_position.v[1] >= 0.f
                && gCurrent_race.checkpoints[pCheckpoint].map_position.v[1] < (float)pMap->height) {

            if (gMap_view == 2) {
                PossibleLock(1);
            }
            DRPixelmapRectangleMaskedCopy(pMap,
                (int)(gCurrent_race.checkpoints[pCheckpoint].map_position.v[0] - (float)gCheckpoint_digit_center_x),
                (int)(gCurrent_race.checkpoints[pCheckpoint].map_position.v[1] - (float)gCheckpoint_digit_center_y),
                gCheckpoint_numbers,
                0,
                pCheckpoint * gCheckpoint_digit_height,
                gCheckpoint_numbers->width,
                gCheckpoint_digit_height);
            if (gMap_view == 2) {
                PossibleUnlock(1);
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x00496940
void C2_HOOK_FASTCALL CalcMapCheckpoint2(br_pixelmap* pMap, int pCheckpoint, tU32 pTime, int pTarget) {
    br_vector3 pos_mapspace;
    br_vector2 p;

    BrMatrix34ApplyP(&pos_mapspace, &gCurrent_race.checkpoints[pCheckpoint].pos, &gCurrent_race.map_transformation);
    BrVector2Sub(&p, &pos_mapspace, &gOrigin_map);
    BrVector2Add(&p, &p, &gOrigin_headup_map);
    BrMatrix23TApplyV(&gCurrent_race.checkpoints[pCheckpoint].map_position,
        &p, &gMatrix23_0068c880);
    gCurrent_race.checkpoints[pCheckpoint].map_position.v[0] += (float)gINT_0074abd4;
    gCurrent_race.checkpoints[pCheckpoint].map_position.v[1] += (float)gINT_0074abd0;
}

// FUNCTION: CARMA2_HW 0x004967c0
void C2_HOOK_FASTCALL DrawMapSmallBlip(br_pixelmap* pScreen, tU32 pTime, const br_vector3 *pPos, int pColour) {
    br_vector3 map_pos;

    if (pTime & 0x100) {
        pColour = 0;
    }
    BrMatrix34ApplyP(&map_pos, pPos, &gCurrent_race.map_transformation);
    if (map_pos.v[0] >= 0.f && map_pos.v[0] < (float)pScreen->width
            && map_pos.v[1] >= 0 && map_pos.v[1] < (float)pScreen->height) {

        if (gMap_view == 2) {
            PossibleLock(1);
        }
        if (gBack_screen->type == BR_PMT_INDEX_8) {
            ((br_uint_8*)pScreen->pixels)[(int)map_pos.v[1] * pScreen->row_bytes + (int)map_pos.v[0]] = pColour;
        } else {
            ((br_uint_16*)pScreen->pixels)[(int)map_pos.v[1] * pScreen->row_bytes + (int)map_pos.v[0]] = PaletteEntry16Bit(gRender_palette, pColour);
        }
        if (gMap_view == 2) {
            PossibleUnlock(1);
        }
    }
}

br_uint_32 C2_HOOK_FASTCALL OppositeColour(br_uint_32 pColour) {
    if (pColour < 0xe0) {
        if ((pColour & 7) < 4) {
            return 0xff;
        } else {
            return 0;
        }
    } else {
        if ((pColour & 0xf) < 8) {
            return 0xff;
        } else {
            return 0;
        }
    }
}

// FUNCTION: CARMA2_HW 0x00495a00
void C2_HOOK_FASTCALL DrawArrow(br_pixelmap* pScreen, br_uint_32 pArrow_index, const br_vector3 *pPos, br_uint_32 pColour) {
    int colours[2];
    int colour;
    int x;
    int y;
    int temp;
    int point_count;
    const int* arrow_ptr;
    int i;
    int j;

    if (pColour == 0xffffffff) {
        colours[0] = 0x0;
        colours[1] = 0x84;
    } else {
        colours[0] = pColour;
        colours[1] = OppositeColour(pColour);
    }

    if (gBack_screen->type != BR_PMT_INDEX_8) {
        colours[0] = PaletteEntry16Bit(gRender_palette, colours[0]);
        colours[1] = PaletteEntry16Bit(gRender_palette, colours[1]);
        if (pColour == 0xffffffff) {
            colours[1] = Colour24BitTo16Bit(0xff00ff);
        }
    }

    for (i = 0; i < CARPOCALYPSE2_ASIZE(colours); i++) {
        colour = colours[i];
        point_count = gArrows[i][pArrow_index & 0x3][0];
        arrow_ptr = &gArrows[i][pArrow_index & 0x3][1];
        for (j = 0; j < point_count; j++, arrow_ptr += 2) {
            if (pArrow_index & 0x8) {
                x = -arrow_ptr[0];
                y = -arrow_ptr[1];
            } else {
                x = arrow_ptr[0];
                y = arrow_ptr[1];
            }
            if (pArrow_index & 0x4) {
                temp = x;
                x = -y;
                y = temp;
            }
            BrPixelmapPixelSet(pScreen,
                (int)(pPos->v[0] + x),
                (int)(pPos->v[1] + y),
                colour);
        }
    }
}

// FUNCTION: CARMA2_HW 0x00496270
void C2_HOOK_FASTCALL DrawMapBlip(br_pixelmap* pScreen, tCar_spec* pCar, tU32 pTime, br_matrix34* pTrans, const br_vector3* pPos, br_uint_32 pColour) {
    tU32 time_diff;
    br_vector3 map_pos;
    float tv1;
    float tv2;
    int local_pos[2];
    int colour;
    int colours[2];
    float bearing;
    int arrow_index;
    int size;

    time_diff = pTime - gMap_time;
    BrMatrix34ApplyP(&map_pos, pPos, &gCurrent_race.map_transformation);
    map_pos.v[2] = 0.0;
    if (((pCar != NULL && pCar->driver == eDriver_oppo) || (pCar != NULL && pCar->driver == eDriver_net_human))
            && (!pCar->knackered || pCar == gTarget_lock_car_1)
            && (gCurrent_race.race_spec->race_type != kRaceType_Cars || pCar->is_race_goal)
            && (gNet_mode == eNet_mode_none
                    || (gCurrent_net_game->type == eNet_game_type_6 && NetPlayerFromCar(pCar)->field_0x80 == 0)
                    || (gCurrent_net_game->type == eNet_game_type_foxy && pCar == gNet_players[gIt_or_fox].car)
                    || (gCurrent_net_game->type != eNet_game_type_6 && gCurrent_net_game->type != eNet_game_type_foxy))) {

        local_pos[0] = (int)((float)(gHeadup_map_half_width + gINT_0068d890 + gINT_0074ab8c) - map_pos.v[0]);
        local_pos[1] = (int)((float)(gHeadup_map_half_height + gINT_0068c858 + gINT_0074ab88) - map_pos.v[1]);
        tv1 = 0.f -map_pos.v[2];
        tv2 = (float)local_pos[1] * (float)local_pos[1] + (float)local_pos[0] * (float)local_pos[0] + tv1 * tv1;
        if (((gTarget_lock_car_2 == NULL
                || (!gTarget_lock_enabled && tv2 < gFLOAT_0074ab90)
                || (gTarget_lock_enabled && pCar == gTarget_lock_car_2)))
                        && !pCar->knackered) {

            gVector2_0068d6d8.v[0] = (float)local_pos[0];
            gVector2_0068d6d8.v[1] = (float)local_pos[1];
            gFLOAT_0068d6e0 = tv1;
            gTarget_lock_car_2 = pCar;
            gFLOAT_0074ab90 = tv2;
        }
    }
    if (gMini_map_visible || gMap_view == 2) {
        BrMatrix34Mul(&gCar_in_map_space, pTrans, &gCurrent_race.map_transformation);
        bearing = FastScalarArcTan2(gCar_in_map_space.m[2][0], gCar_in_map_space.m[2][1]);
        bearing = (360.f - bearing + 12.25f) / 22.5f;
        arrow_index = ((int)bearing) % 16;
        if (pCar == NULL || pCar->driver != eDriver_local_human || (pTime & 0x100)) {
            DrawArrow(pScreen, arrow_index, &map_pos, pColour);
        }
        colours[0] = pColour;
        colours[1] = OppositeColour(pColour);
        if (gBack_screen->type != BR_PMT_INDEX_8) {
            colours[0] = PaletteEntry16Bit(gRender_palette, pColour);
            colours[1] = PaletteEntry16Bit(gRender_palette, colours[1]);
        }
        colour = colours[pTime & 0x100];
        if (gNet_mode != eNet_mode_none
                && gCurrent_net_game->type == eNet_game_type_foxy
                && pCar == gNet_players[gIt_or_fox].car) {

            BrPixelmapLine(pScreen,
                (int)(map_pos.v[0] - 8.f),
                (int)(map_pos.v[1] - 8.f),
                (int)(map_pos.v[0] + 8.f),
                (int)(map_pos.v[1] - 8.f),
                colour);
            BrPixelmapLine(pScreen,
                (int)(map_pos.v[0] - 8.f),
                (int)(map_pos.v[1] + 8.f),
                (int)(map_pos.v[0] + 8.f),
                (int)(map_pos.v[1] + 8.f),
                colour);
            BrPixelmapLine(pScreen,
                (int)(map_pos.v[0] - 8.f),
                (int)(map_pos.v[1] - 8.f),
                (int)(map_pos.v[0] - 8.f),
                (int)(map_pos.v[1] + 8.f),
                colour);
            BrPixelmapLine(pScreen,
                (int)(map_pos.v[0] + 8.f),
                (int)(map_pos.v[1] - 8.f),
                (int)(map_pos.v[0] + 8.f),
                (int)(map_pos.v[1] + 8.f),
                colour);
        }
        if (time_diff <= 500 && pCar != NULL && pCar->driver == eDriver_local_human) {

            size = (35000 - 70 * time_diff) / 500;

            BrPixelmapLine(pScreen,
                (int)(map_pos.v[0] - 0.5f) - size,
                (int)(map_pos.v[1] - 0.5f) - size,
                (int)(map_pos.v[0] + 0.5f) + size,
                (int)(map_pos.v[1] - 0.5f) + size,
                colour);
            BrPixelmapLine(pScreen,
                (int)(map_pos.v[0] - 0.5f) - size,
                (int)(map_pos.v[1] + 0.5f) + size,
                (int)(map_pos.v[0] + 0.5f) + size,
                (int)(map_pos.v[1] + 0.5f) + size,
                colour);
            BrPixelmapLine(pScreen,
                (int)(map_pos.v[0] - 0.5f) - size,
                (int)(map_pos.v[1] - 0.5f) - size,
                (int)(map_pos.v[0] - 0.5f) - size,
                (int)(map_pos.v[1] + 0.5f) + size,
                colour);
            BrPixelmapLine(pScreen,
                (int)(map_pos.v[0] + 0.5f) + size,
                (int)(map_pos.v[1] - 0.5f) - size,
                (int)(map_pos.v[0] + 0.5f) + size,
                (int)(map_pos.v[1] + 0.5f) + size,
                colour);
        }
    }
}

void C2_HOOK_FASTCALL DRPixelmapBlendedLine(br_pixelmap* pMap, int pX1, int pY1, int pX2, int pY2, br_uint_32 pColour, br_pixelmap* pPalette) {

    DRDrawLine(pMap, pX1, pY1, pX2, pY2, pColour);
}

void C2_HOOK_FASTCALL DR8BitFancyDrawLine(br_pixelmap* pMap, int pX1, int pY1, int pX2, int pY2, br_uint_32 pColour, tU32 pTime_period) {
    int anim_index;

    anim_index = (PDGetTotalTime() / pTime_period) % BR_ASIZE(gMini_map_glowing_line_animation_indices);
    DRPixelmapBlendedLine(pMap, pX1, pY1, pX2, pY2, pColour, gMini_map_glowing_line_palettes[gMini_map_glowing_line_animation_indices[anim_index]]);
    if (fabsf((float)(pY2 - pY1)) < fabsf((float)(pX2 - pX1))) {
        DRPixelmapBlendedLine(pMap, pX1, pY1 - 1, pX2, pY2 - 1, pColour, gMini_map_glowing_line_palettes[gMini_map_glowing_line_animation_indices[anim_index]]);
        DRPixelmapBlendedLine(pMap, pX1, pY1 + 1, pX2, pY2 + 1, pColour, gMini_map_glowing_line_palettes[gMini_map_glowing_line_animation_indices[anim_index]]);
    } else {
        DRPixelmapBlendedLine(pMap, pX1 - 1, pY1, pX2 - 1, pY2, pColour, gMini_map_glowing_line_palettes[gMini_map_glowing_line_animation_indices[anim_index]]);
        DRPixelmapBlendedLine(pMap, pX1 + 1, pY1, pX2 + 1, pY2, pColour, gMini_map_glowing_line_palettes[gMini_map_glowing_line_animation_indices[anim_index]]);
    }
}

// FUNCTION: CARMA2_HW 0x0047d2b0
void C2_HOOK_FASTCALL FancyDrawLine(br_pixelmap *pMap, int pX1, int pY1, int pX2, int pY2, br_uint_32 pColour, tU32 pTime_period) {

    if (pMap->type == BR_PMT_INDEX_8) {
        DR8BitFancyDrawLine(pMap, pX1, pY1, pX2, pY2, pColour, pTime_period);
    } else {
        BrPixelmapLine(pMap, pX1, pY1, pX2, pY2, PaletteEntry16Bit(gRender_palette, pColour));
    }
}

// FUNCTION: CARMA2_HW 0x0047d0d0
void C2_HOOK_FASTCALL DR16BitPixelmapRotatedAndFeatheredCopy(br_matrix23* pMat, br_pixelmap* pDest, tS16 pDest_x, tS16 pDest_y, br_pixelmap* pSrc, tS16 pSrc_x, tS16 pSrc_y, tS16 pSrc_width, tS16 pSrc_height) {
    int width_div_2;
    int height_div_2;
    int dy;
    tU16 *ptr_write;
    float src_f_x;
    float src_f_y;
    int width;

    width_div_2 = pSrc_width / 2;
    height_div_2 = pSrc_height / 2;
    for (dy = -height_div_2; dy < height_div_2; dy++, pDest_y++) {
        src_f_x = (float)pSrc_x - (pMat->m[0][0] * (float)width_div_2 + pMat->m[0][1] * (float)dy);
        src_f_y = (float)pSrc_y + (pMat->m[1][0] * (float)width_div_2 + pMat->m[1][1] * (float)dy) + (float)pSrc_height / 2.f;
        if (-width_div_2 < width_div_2) {
            width = 2 * width_div_2;
            ptr_write = (tU16*)((tU8*)pDest->pixels + pDest_y * pDest->row_bytes) + pDest_x;
            for (; width != 0; width--) {
                *ptr_write++ = *((tU16*)((tU8*)pSrc->pixels + ((int)src_f_y * pSrc->row_bytes)) + width_div_2 + (int)src_f_x);
                src_f_x += pMat->m[0][0];
                src_f_y -= pMat->m[1][0];
            }
        }
    }
}

void C2_HOOK_FASTCALL DR8BitPixelmapRotatedAndFeatheredCopy(br_matrix23* pMat, br_pixelmap* pDest, tS16 pDest_x, tS16 pDest_y, br_pixelmap* pSrc, tS16 pSrc_x, tS16 pSrc_y, tS16 pSrc_width, tS16 pSrc_height, int pTrans) {
    int width_div_2;
    int width_near1;
    int width_near2;
    int width_near3;
    int height_div_2;
    int height_near1;
    int height_near2;
    int height_near3;
    float src_f_x;
    float src_f_y;
    int src_x;
    int src_y;
    int dy;
    int dx;
    int dx_bit0;
    int dy_bit1;
    tU8 src_pix;
    tU8* ptr_write;

    width_div_2 = pSrc_width / 2;
    width_near1 = width_div_2 - pSrc_width * 1 / 24;
    width_near2 = width_div_2 - pSrc_width * 2 / 24;
    width_near3 = width_div_2 - pSrc_width * 3 / 24;
    height_div_2 = pSrc_height / 2;
    height_near1 = height_div_2 - pSrc_height * 1 / 24;
    height_near2 = height_div_2 - pSrc_height * 2 / 12;
    height_near3 = height_div_2 - pSrc_height * 3 / 24;
    for (dy = -height_div_2; dy < height_div_2; dy++, pDest_y++) {
        src_f_x = (float)pSrc_x - (pMat->m[0][0] * (float)width_div_2 + pMat->m[0][1] * (float)dy);
        src_f_y = (float)pSrc_y + (pMat->m[1][0] * (float)width_div_2 + pMat->m[1][1] * (float)dy) + (float)pSrc_height / 2.f;
        ptr_write = (tU8*)pDest->pixels + pDest_y * pDest->row_bytes + pDest_x;
        for (dx = -width_div_2; dx < width_div_2; dx++) {
            src_y = (int)src_f_y;
            src_x = (int)src_f_x;
            src_f_x += pMat->m[0][0];
            src_f_y -= pMat->m[1][0];
            if (pTrans) {
                *ptr_write = ((tU8*)gPalette_0074a600->pixels)[*ptr_write + ((tU8*)pSrc->pixels)[src_y * pSrc->row_bytes + width_div_2 + src_x] * 256];
            } else {
                src_pix = ((tU8*)pSrc->pixels)[src_y * pSrc->row_bytes + width_div_2 + src_x];
                dx_bit0 = ((src_pix >> 0) & 0x1) - ((*ptr_write >> 0) & 0x1) + dx;
                dy_bit1 = ((src_pix >> 1) & 0x1) - ((*ptr_write >> 1) & 0x1) + dy;

                if (-width_div_2 <= dx_bit0 && dx_bit0 <= width_div_2
                        && -height_div_2 <= dy_bit1 && dy_bit1 <= height_div_2) {
                    if (!(-width_near1 <= dx_bit0 && dx_bit0 <= width_near1
                            && -height_near1 <= dy_bit1 && dy_bit1 <= height_near1)) {
                        *ptr_write = ((tU8*)gPalette_0074a604->pixels)[*ptr_write + 256 * src_pix];
                    } else if (!(-width_near2 <= dx_bit0 && dx_bit0 <= width_near2
                            && -height_near2 <= dy_bit1 && dy_bit1 <= height_near2)) {
                        *ptr_write = ((tU8*)gPalette_0074a600->pixels)[*ptr_write + 256 * src_pix];
                    } else if (!(-width_near3 <= dx_bit0 && dx_bit0 <= width_near3
                            && -height_near3 <= dy_bit1 && dy_bit1 <= height_near3)) {
                        *ptr_write = ((tU8*)gPalette_0074a5fc->pixels)[*ptr_write + 256 * src_pix];
                    } else {
                        *ptr_write = src_pix;
                    }
                }
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x0047cd40
void C2_HOOK_FASTCALL DRPixelmapRotatedAndFeatheredCopy(br_matrix23* pMat, br_pixelmap* pDest, tS16 pDest_x, tS16 pDest_y, br_pixelmap* pSrc, tS16 pSrc_x, tS16 pSrc_y, tS16 pSrc_width, tS16 pSrc_height, int pTrans) {
    if (pDest->type == BR_PMT_INDEX_8) {
        DR8BitPixelmapRotatedAndFeatheredCopy(pMat, pDest, pDest_x, pDest_y, pSrc, pSrc_x, pSrc_y, pSrc_width, pSrc_height, pTrans);
    } else {
        DR16BitPixelmapRotatedAndFeatheredCopy(pMat, pDest, pDest_x, pDest_y, pSrc, pSrc_x, pSrc_y, pSrc_width, pSrc_height);
    }
}

void C2_HOOK_FASTCALL DRPixelmapCopyMapBlack8Bit(br_pixelmap* pDest, br_pixelmap* pSrc) {
    tU16 dest_row_bytes = pDest->row_bytes;
    tU16 src_row_bytes = pSrc->row_bytes;
    tU8 *dest_pixels = pDest->pixels;
    tU16 dest_width = pDest->width;
    tU16 src_width = pSrc->width;
    tU8 *src_pixels = pSrc->pixels;
    tU32 src_width_dwords = src_width / sizeof(tU32);
    tU32 dwords_remaining;
    int y;

    for (y = 0; y < (int)pSrc->height; y++) {

        for (dwords_remaining = src_width_dwords; dwords_remaining != 0; dwords_remaining--, src_pixels+=4, dest_pixels+=4) {
            tU8 p0 = src_pixels[0];
            tU8 p1 = src_pixels[1];
            tU8 p2 = src_pixels[2];
            tU8 p3 = src_pixels[3];
            dest_pixels[0] = p0 != 0 ? p0 : 0xf0;
            dest_pixels[1] = p1 != 0 ? p1 : 0xf0;
            dest_pixels[2] = p2 != 0 ? p2 : 0xf0;
            dest_pixels[3] = p3 != 0 ? p3 : 0xf0;
        }
        src_pixels += src_row_bytes - src_width;
        dest_pixels += dest_row_bytes - dest_width;
    }
}

// FUNCTION: CARMA2_HW 0x0047d5b0
void C2_HOOK_FASTCALL DRPixelmapCopyMapBlack(br_pixelmap* pDest, br_pixelmap* pSrc) {
    if (pDest->type == BR_PMT_INDEX_8) {
        DRPixelmapCopyMapBlack8Bit(pDest, pSrc);
    } else {
        BrPixelmapCopy(pDest, pSrc);
    }
}

// FUNCTION: CARMA2_HW 0x004e74d0
void C2_HOOK_FASTCALL RenderShadows(br_actor* pWorld, tTrack_spec* pTrack_spec, br_actor* pCamera, br_matrix34* pCamera_to_world_transform) {
    int i;
    int cat;
    int car_count;
    tCar_spec* the_car;
    br_vector3 camera_to_car;
    br_scalar distance_factor;

    if (gShadow_level == eShadow_none) {
        return;
    }
    for (cat = eVehicle_self;; ++cat) {
        if (gShadow_level == eShadow_everyone) {
            if (cat >= eVehicle_not_really) {
                break;
            }
        } else {
            if (cat > (gShadow_level == eShadow_us_and_opponents ? eVehicle_rozzer : eVehicle_self)) {
                break;
            }
        }

        if (cat == eVehicle_self) {
            car_count = 1;
        } else {
            car_count = GetCarCount(cat);
        }
        for (i = 0; i < car_count; i++) {
            if (cat == eVehicle_self) {
                the_car = &gProgram_state.current_car;
            } else {
                the_car = GetCarSpec(cat, i);
            }
            if (!the_car->active) {
                continue;
            }
            if (IsCarCloaked(the_car)) {
                continue;
            }
            if (gAction_replay_mode) {
                continue;
            }

            BrVector3Sub(&camera_to_car, (br_vector3*)gCamera_to_world.m[3], &the_car->car_master_actor->t.t.translate.t);
            distance_factor = BrVector3LengthSquared(&camera_to_car);
            if (distance_factor <= SHADOW_MAX_RENDER_DISTANCE) {
                ProcessShadow(the_car, gUniverse_actor, &gProgram_state.track_spec, gCamera, &gCamera_to_world, distance_factor);
            }
        }
    }
    if (gFancy_shadow) {
        for (i = 0; i < gSaved_table_count; i++) {
            gSaved_shade_tables[i].original->height = gSaved_shade_tables[i].copy->height;
            gSaved_shade_tables[i].original->pixels = gSaved_shade_tables[i].copy->pixels;
            BrTableUpdate(gSaved_shade_tables[i].original, BR_TABU_ALL);
        }
    }
}